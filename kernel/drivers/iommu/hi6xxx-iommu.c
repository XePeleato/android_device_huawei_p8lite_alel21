/*
 * Copyright (C) 20013-2013 hisilicon. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
#define pr_fmt(fmt) "[hi6xxx-smmu]  " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/iommu.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <asm/cacheflush.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/ion.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/hisi/hi6xxx-iomap.h>

#include "soc_media_sctrl_interface.h"
#include "soc_baseaddr_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_pmctrl_interface.h"
#include "soc_ao_sctrl_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_smmu_interface.h"
#include "hi6xxx-iommu.h"

typedef unsigned int smmu_pte_t;
struct hi6xxx_smmu_chip{
	unsigned int   irq;
	unsigned char  chip_state;
	unsigned char  chip_ver;
	unsigned char  reserve[2];
	irq_handler_t  smmu_isr;
	void __iomem   *reg_base;
	struct clk     *smmu_peri_clk;
	struct clk     *smmu_clk;
	struct clk     *media_sc_clk;
};

struct hi6xxx_smmu_dev {
	struct iommu_domain *domain;
	struct hi6xxx_smmu_chip *smmu_chip;
	smmu_pte_t   *pgtbl_vir;
	dma_addr_t  pgtbl_phy;
	size_t    pgtbl_sz;
	unsigned int iova_start;
	unsigned int iova_sz;
	unsigned int align;
	unsigned int pg_sz;
};
struct hi6xxx_smmu_dev smmu_dev_keeper = {0};

static DEFINE_MUTEX(hi6xxx_smmu_mutex);
static DEFINE_SPINLOCK(iommu_spinlock);
static unsigned int smmu_regs_value[SMMU_REGS_MAX] = {0};
static int smmu_ddr_size;

void dump_pgtbl(int start, int end)
{
	int i;
	for (i = start; i<end; i++){
		printk(KERN_ERR"[0x%08x] ",smmu_dev_keeper.pgtbl_vir[i]);
	}
}

void dump_smmu_dev_keeper(void)
{
	printk(KERN_ERR"iova_start %#x ",smmu_dev_keeper.iova_start);
	printk(KERN_ERR"iova_sz %#x ",smmu_dev_keeper.iova_sz);
	printk(KERN_ERR"align %#x ",smmu_dev_keeper.align);
	printk(KERN_ERR"pg_sz %#x ",smmu_dev_keeper.pg_sz);
	printk(KERN_ERR"pgtbl_vir %p ",smmu_dev_keeper.pgtbl_vir);
	printk(KERN_ERR"pgtbl_phy %#x ",(unsigned int)smmu_dev_keeper.pgtbl_phy);
	printk(KERN_ERR"pgtbl_sz 0x%lx ",smmu_dev_keeper.pgtbl_sz);
	printk(KERN_ERR"\n ");

}

/**
 * at V8R2 Platform preload ops is a stub
 */
static inline void preload_tlb(struct hi6xxx_smmu_dev *smmu_dev,
	unsigned long iova, size_t size)
{

	return;
}

static inline void invalid_tlb(struct hi6xxx_smmu_dev * smmu_dev,
	unsigned long iova, size_t size)
{
	unsigned long flags;
	unsigned int smmu_ctrl = 0;
	unsigned int inv_cnt = 10000;
	unsigned int start = 0;
	unsigned int end   = 0;
	unsigned int pg_sz = smmu_dev->pg_sz;
	dma_addr_t smmu_pgtbl_phy = smmu_dev->pgtbl_phy;
	struct hi6xxx_smmu_chip *smmu_chip = smmu_dev->smmu_chip;

	/**
	 * protect control register from access simutaneously
	 */
	spin_lock_irqsave(&iommu_spinlock,flags);

	/*lint -e666*/
	start = iova >> order_base_2(pg_sz);
	end   = start + (size >> order_base_2(pg_sz));
	/*lint +e666*/

	writel(smmu_pgtbl_phy + start * sizeof(smmu_pte_t),smmu_chip->reg_base + SMMU_START_OFFSET);
	writel((smmu_pgtbl_phy + end  * sizeof(smmu_pte_t)),smmu_chip->reg_base + SMMU_END_OFFSET);

	smmu_ctrl = readl(smmu_chip->reg_base + SMMU_CTRL_OFFSET);
	smmu_ctrl = smmu_ctrl | (1<<10);
	writel(smmu_ctrl,smmu_chip->reg_base + SMMU_CTRL_OFFSET);
	do{
		smmu_ctrl = readl(smmu_chip->reg_base + SMMU_CTRL_OFFSET);
		if ( 0x0 == (smmu_ctrl & (1<<10))){
			spin_unlock_irqrestore(&iommu_spinlock,flags);
			return;
		}
	}while(inv_cnt--);

	spin_unlock_irqrestore(&iommu_spinlock,flags);

	WARN_ON((!inv_cnt)&&((smmu_ctrl & 0x400) != 0));

	return;
}

/**
 * at V8R2 Platform pgtbl is no cached so not need flush it
 */
static inline void flush_pgtbl(struct hi6xxx_smmu_dev *smmu_dev,
	unsigned long iova, size_t size)
{
	return;
}

static inline void set_smmu_pte(volatile smmu_pte_t *pte,
	uint32_t phys_addr)
{
	if(!(*pte&PAGE_ENTRY_VALID)){
		*pte = (phys_addr | PAGE_ENTRY_VALID);
	}else{
		pr_err("set pte is err,please check now\n");
	}

	/*pr_err("%#x  ",*pte);*/
}

static inline void clear_pte(volatile smmu_pte_t *pte)
{
	if((*pte&PAGE_ENTRY_VALID)){
		*pte = 0;
	}else{
		pr_err("clear pte is err,please check now\n");
	}
}


static int map_iova(struct hi6xxx_smmu_dev *smmu_dev,
	unsigned long iova, phys_addr_t paddr, size_t size)
{
	size_t maped_size = 0;
	smmu_pte_t *pgtable = NULL;
	unsigned long offset = 0;
	unsigned int pg_sz = smmu_dev->pg_sz;

	pgtable = smmu_dev->pgtbl_vir;
	if (!pgtable) {
		pr_err("[%s]error: page table not ready!\n", __func__);
		return -EINVAL;
	}

	/*lint -e666*/
	offset = iova >> order_base_2(pg_sz);
	/*lint +e666*/

	for (maped_size = 0; maped_size < size; maped_size += pg_sz, offset++) {
		set_smmu_pte((pgtable + offset), (uint32_t)(paddr + maped_size));
	}

	return 0;
}

static int unmap_iova(struct hi6xxx_smmu_dev *smmu_dev,
		unsigned long iova, size_t size)
{
	size_t maped_size = 0;
	smmu_pte_t *pgtable = NULL;
	unsigned long offset = 0;
	unsigned int pg_sz = smmu_dev->pg_sz;

	pgtable = smmu_dev->pgtbl_vir;
	if (!pgtable) {
		pr_err("[%s]error: page table not ready!\n", __func__);
		return -EINVAL;
	}

	/*lint -e666*/
	offset = iova >> order_base_2(pg_sz);
	/*lint +e666*/

	for (maped_size = 0; maped_size < size; maped_size += pg_sz, offset++) {
		clear_pte(pgtable + offset);
	}

	return 0;
}

static dma_addr_t get_phys(struct scatterlist *sg)
{
	/*
	 * Try sg_dma_address first so that we can
	 * map carveout regions that do not have a
	 * struct page associated with them.
	 */
	dma_addr_t dma_addr = sg_dma_address(sg);
	if (dma_addr == 0)
		dma_addr = sg_phys(sg);
	return dma_addr;
}

void build_ops(struct hi6xxx_smmu_dev *smmu_dev,struct iommu_ops *smmu_ops)
{
	smmu_ops->pgsize_bitmap = smmu_dev->pg_sz;
}

static smmu_pte_t *alloc_pgtbl(struct device *dev,size_t size,dma_addr_t *phy,int mode)
{

	void *pgtbl_vir = NULL;

	/**
	 * dma memory alloc
	 */
	dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	pgtbl_vir = dma_alloc_coherent(dev,size,phy,GFP_KERNEL);
	if(!pgtbl_vir){
		WARN_ON(1);
		pr_err("dma_alloc_coherent first time is failed\n");
		return NULL;
	}

	/**
	 * smmu pgtbl base must align SZ_1M
	 */
	if (*phy & (SZ_1M - 1)) {/* not align SZ_1M*/
		dma_free_coherent(dev,size,pgtbl_vir,phy);
		pgtbl_vir = NULL;
		*phy = 0;
		pgtbl_vir = dma_alloc_coherent(dev,size * 2,phy,GFP_KERNEL);
		if(!pgtbl_vir){
			pr_err("dma_alloc_coherent again is failed\n");
			return NULL;
		}
		*phy = (*phy + (SZ_1M - 1)) & (~(SZ_1M - 1));
		pgtbl_vir = (void*)(((unsigned long)pgtbl_vir + (SZ_1M - 1)) & (~(SZ_1M - 1)));
	}

	return (smmu_pte_t *)pgtbl_vir;
}

static void reload_regs(struct hi6xxx_smmu_chip* smmu_chip)
{
	writel(smmu_regs_value[2],smmu_chip->reg_base + SMMU_PTBR_OFFSET);
	writel(smmu_regs_value[5],smmu_chip->reg_base + SMMU_STATUS_OFFSET);
	writel(smmu_regs_value[6],smmu_chip->reg_base + SMMU_AXIID_OFFSET);
	writel(smmu_regs_value[7],smmu_chip->reg_base + SMMU_SEC_START_OFFSET);
	writel(smmu_regs_value[8],smmu_chip->reg_base + SMMU_SEC_END_OFFSET);
	writel(smmu_regs_value[9],smmu_chip->reg_base + SMMU_VERSION_OFFSET);
	writel(smmu_regs_value[10],smmu_chip->reg_base + SMMU_IPTSRC_OFFSET);
	writel(smmu_regs_value[11],smmu_chip->reg_base + SMMU_IPTPA_OFFSET);
	writel(smmu_regs_value[12],smmu_chip->reg_base + SMMU_TRBA_OFFSET);
	writel(smmu_regs_value[13],smmu_chip->reg_base + SMMU_BYS_START_OFFSET);
	writel(smmu_regs_value[14],smmu_chip->reg_base + SMMU_BYS_END_OFFSET);
}


static void restore_regs(struct hi6xxx_smmu_chip* smmu_chip)
{
	smmu_regs_value[0]= readl((void *)(smmu_chip->reg_base + SMMU_CTRL_OFFSET));
	smmu_regs_value[1]= readl((void *)(smmu_chip->reg_base + SMMU_ENABLE_OFFSET));
	smmu_regs_value[2]= readl((void *)(smmu_chip->reg_base + SMMU_PTBR_OFFSET));
	smmu_regs_value[3]= readl((void *)(smmu_chip->reg_base + SMMU_START_OFFSET));
	smmu_regs_value[4]= readl((void *)(smmu_chip->reg_base + SMMU_END_OFFSET));
	smmu_regs_value[5]= readl((void *)(smmu_chip->reg_base + SMMU_STATUS_OFFSET));
	smmu_regs_value[6]= readl((void *)(smmu_chip->reg_base + SMMU_AXIID_OFFSET));
	smmu_regs_value[7]= readl((void *)(smmu_chip->reg_base + SMMU_SEC_START_OFFSET));
	smmu_regs_value[8]= readl((void *)(smmu_chip->reg_base + SMMU_SEC_END_OFFSET));
	smmu_regs_value[9]= readl((void *)(smmu_chip->reg_base + SMMU_VERSION_OFFSET));
	smmu_regs_value[10]= readl((void *)(smmu_chip->reg_base + SMMU_IPTSRC_OFFSET));
	smmu_regs_value[11]= readl((void *)(smmu_chip->reg_base + SMMU_IPTPA_OFFSET));
	smmu_regs_value[12]= readl((void *)(smmu_chip->reg_base + SMMU_TRBA_OFFSET));
	smmu_regs_value[13] = readl((void *)(smmu_chip->reg_base + SMMU_BYS_START_OFFSET));
	smmu_regs_value[14] = readl((void *)(smmu_chip->reg_base + SMMU_BYS_END_OFFSET));
}

static void smmu_enable(struct device *dev,struct hi6xxx_smmu_dev *smmu_dev)
{
	const unsigned int* smmu_ctrl;
	unsigned int  smmu_ctrl_val;
	size_t  pgtbl_sz = smmu_dev->pgtbl_sz;
	dma_addr_t smmu_pgtbl_phy = smmu_dev->pgtbl_phy;
	struct device_node *np  = dev->of_node;
	struct hi6xxx_smmu_chip *smmu_chip = smmu_dev->smmu_chip;

	smmu_chip->chip_state = SMMU_INIT;
	/**
	 * some smmu regs set at fastboot
	 *
	 * 0x000001A6:Iommu page size 8K DDRsize 1G,VA size 512MB TLB size 128KB
	 * 0x00000160:iommu page size 4K DDRsize 512MB,VA size 256MB TLB size 128KB
	 * 0x00000164:iommu page size 4K DDRsize 1G,VA size 256MB TLB size 128KB
     */

	smmu_ctrl = of_get_property(np, "hisi,smmu-ctrl", NULL);
	if (smmu_ctrl){
		if (smmu_ddr_size == SZ_2G){
			pr_info("SZ_2G the size of ddr\n");
			writel(be32_to_cpu(*smmu_ctrl)|(0x2<<2),(smmu_chip->reg_base + SMMU_CTRL_OFFSET));
		}
		else if (smmu_ddr_size == SZ_1G){
			pr_info("SZ_1G the size of ddr\n");
			writel(be32_to_cpu(*smmu_ctrl)|(0x1<<2),(smmu_chip->reg_base + SMMU_CTRL_OFFSET));
		}
		else {
			pr_err("no the size of ddr\n");
		}
	}

	/**
	 * when the interrupt of invalid pte happend,
	 * smmu will access address 0x0.
	 */
	writel(0x00000000,(smmu_chip->reg_base + SMMU_IPTPA_OFFSET));

	/**
	 * clear smmu interrupt
	 */
	writel(0xff,(smmu_chip->reg_base + SMMU_INTCLR_OFFSET));

	if(request_irq(smmu_chip->irq,smmu_chip->smmu_isr,IRQF_DISABLED,"hi6xxx-smmu-isr",(void *)smmu_chip)){
		BUG_ON(1);
	}

	writel(smmu_pgtbl_phy,(smmu_chip->reg_base + SMMU_PTBR_OFFSET));
	writel(smmu_pgtbl_phy,(smmu_chip->reg_base + SMMU_START_OFFSET));
	writel(smmu_pgtbl_phy + (pgtbl_sz + sizeof(smmu_pte_t)),(smmu_chip->reg_base + SMMU_END_OFFSET));
	smmu_ctrl_val = readl(smmu_chip->reg_base + SMMU_CTRL_OFFSET);
	smmu_ctrl_val = smmu_ctrl_val | (1<<10);

	smmu_chip->chip_state = SMMU_RUNNING;

	/*
	writel(smmu_ctrl_val,smmu_chip->reg_base + SMMU_CTRL_OFFSET);
	do{
		smmu_ctrl_val = readl(smmu_chip->reg_base + SMMU_CTRL_OFFSET);
		if ( 0x0 == (smmu_ctrl_val & 0x400)) {
			pr_err("smmu_ctrl_val 0x%x invalid success\n",smmu_ctrl_val);
			break;
		}
		pr_err("invalid failed\n");
	}while(inv_cnt--);

	BUG_ON((inv_cnt == 0)&&((smmu_ctrl_val & 0x400) != 0));
	*/
}

static int hi6xxx_smmu_domain_init(struct iommu_domain *domain)
{
	struct hi6xxx_smmu_dev *smmu_dev = &smmu_dev_keeper;

	/**
	 * register smmu_dev_keeper @ domain
	 */
	domain->capablity.iova_start = smmu_dev->iova_start;
	domain->capablity.iova_end   = smmu_dev->iova_start + smmu_dev->iova_sz;
	domain->capablity.iova_align = smmu_dev->align;
	domain->capablity.pg_sz = smmu_dev->pg_sz;
	domain->capablity.off_on = true;
	domain->priv = smmu_dev;

	smmu_dev->domain = domain;

	return 0;
}

static void hi6xxx_smmu_domain_destroy(struct iommu_domain *domain)
{
	struct hi6xxx_smmu_dev *smmu_dev = &smmu_dev_keeper;

	domain->capablity.iova_start = 0;
	domain->capablity.iova_end   = 0;
	domain->capablity.iova_align = 0;
	domain->capablity.pg_sz = 0;
	domain->capablity.off_on = false;
	domain->priv = NULL;

	smmu_dev->domain = NULL;

	return;
}
static int hi6xxx_smmu_map_range(struct iommu_domain *domain, unsigned long iova,
		struct scatterlist *sg, size_t size, int prot)
{
	struct hi6xxx_smmu_dev *smmu_dev;
	phys_addr_t phys_addr;
	unsigned long iova_tmp = iova;
	unsigned int pg_sz;
	size_t maped_size = 0;
	int ret = 0;


	smmu_dev = domain->priv;
	if (!smmu_dev) {
		ret = -EINVAL;
		goto error;
	}
	pg_sz = smmu_dev->pg_sz;
	if (!IS_ALIGNED(iova, pg_sz)) {
		pr_err("iova is not aligned to %x!\n",pg_sz);
		ret = -EINVAL;
		goto error;
	}
	if (!IS_ALIGNED(size, pg_sz)) {
		WARN_ON(1);
		ret = -EINVAL;
		goto error;
	}

	while (sg) {
		phys_addr = get_phys(sg);

		ret = map_iova(smmu_dev, iova, phys_addr, sg->length);
		if (ret) {
			pr_err("hi6xxx smmu map failed!\n");
			break;
		}

		/**
		 * unsigned int --> unsigned long
		 */
		iova += (unsigned long)ALIGN(sg->length, pg_sz);

		/**
		 * unsigned int --> size_t
		 */
		maped_size += (size_t)ALIGN(sg->length, pg_sz);
		if (maped_size >= size) {
			break;
		}

		sg = sg_next(sg);
	};

	flush_pgtbl(smmu_dev, iova_tmp, size);
	invalid_tlb(smmu_dev, iova_tmp, size);
error:

	return ret;
}
static size_t hi6xxx_smmu_unmap_range(struct iommu_domain *domain, unsigned long iova,
	      size_t size)
{
	struct hi6xxx_smmu_dev *smmu_dev;
	unsigned int pg_sz;
	int ret = 0;


	smmu_dev = domain->priv;
	if (!smmu_dev) {
		pr_err("domain NULL!\n");
		goto error;
	}
	pg_sz = smmu_dev->pg_sz;

	if (!IS_ALIGNED(iova, pg_sz)) {
		pr_err("iova is not aligned to %x!\n",pg_sz);
		goto error;
	}

	ret = unmap_iova(smmu_dev, iova, size);
	if (ret) {
		pr_err("unmap failed!\n");
		goto error;
	}

	flush_pgtbl(smmu_dev, iova, size);
	invalid_tlb(smmu_dev, iova, size);

	return size;
error:
	return 0;
}

/* this function at HI6XXX is just a stub*/
int hi6xxx_smmu_domain_get_attr(struct iommu_domain *domain,enum iommu_attr attr, void *data)
{
	return 0;
}

static int hi6xxx_smmu_map(struct iommu_domain *domain, unsigned long iova,
		phys_addr_t paddr, size_t size, int prot)
{
	int ret = 0;
    WARN_ON(1);
	return ret;
}

static size_t hi6xxx_smmu_unmap(struct iommu_domain *domain, unsigned long iova,
		size_t size)
{
	WARN_ON(1);
	return 0;
}

int hi6xxx_smmu_map_tile(struct iommu_domain *domain, unsigned long iova,
		struct scatterlist *sg, size_t size, int prot,
		struct tile_format *format)
{
	WARN_ON(1);
	return 0;
}

size_t hi6xxx_smmu_unmap_tile(struct iommu_domain *domain, unsigned long iova,
	     size_t size)
{
	WARN_ON(1);
	return 0;
}


static phys_addr_t hi6xxx_smmu_iova_to_phys(struct iommu_domain *domain,
			dma_addr_t iova)
{
	WARN_ON(1);
	return 0;
}

static int hi6xxx_smmu_domain_has_cap(struct iommu_domain *domain,
		unsigned long cap)
{
	return 0;
}

static struct iommu_ops hi6xxx_smmu_ops = {
	.domain_init = hi6xxx_smmu_domain_init,
	.domain_destroy = hi6xxx_smmu_domain_destroy,

	.map_range = hi6xxx_smmu_map_range,
	.unmap_range = hi6xxx_smmu_unmap_range,
	.domain_get_attr = hi6xxx_smmu_domain_get_attr,

	/*they are all stubs*/
	.map = hi6xxx_smmu_map,
	.unmap = hi6xxx_smmu_unmap,
	.map_tile = hi6xxx_smmu_map_tile,
	.unmap_tile = hi6xxx_smmu_unmap_tile,
	.iova_to_phys = hi6xxx_smmu_iova_to_phys,
	.domain_has_cap = hi6xxx_smmu_domain_has_cap,
};

static irqreturn_t hi6xxx_smmu_isr(int irq, void *dev_id)
{
	int          i;
	int          index;
	unsigned int irq_stat;
	unsigned int irq_rawstat;
	unsigned int pgt = 0;
	unsigned int pc_pgt = 0;
	struct hi6xxx_smmu_chip *chip = (struct hi6xxx_smmu_chip *)dev_id;

	irq_stat = readl(chip->reg_base + SMMU_MINTSTS_OFFSET);
	irq_rawstat = readl(chip->reg_base + SMMU_RINTSTS_OFFSET);
	pr_err(" irq raw status is %#x \n",irq_rawstat);

	/**
	 * clear smmu interrupt
	 */
	writel(0xff,chip->reg_base + SMMU_INTCLR_OFFSET);
	pgt = readl(chip->reg_base + SMMU_PTBR_OFFSET);
	pc_pgt = readl(chip->reg_base + SMMU_IPTSRC_OFFSET);

	/**
	 * dump all register
	 */
	for (i = 0; i<SMMU_REGS_SGMT_END; i += 4){
		printk(KERN_ERR "[%08x] ",readl(chip->reg_base+i));
	}

	if (chip->chip_ver == SMMU_CHIP_ID_V200){
		printk("\n dmup ops stream regs\n");
		for (i = SMMU_REGS_OPS_SEGMT_START; i < SMMU_REGS_OPS_SEGMT_NUMB; i++){
			printk("[%08x] [%08x] ",readl(chip->reg_base + i*4), readl(chip->reg_base + (i+1)*4));
		}

		printk("\n dmup axi id regs\n");
		for (i = SMMU_REGS_AXI_SEGMT_START; i < SMMU_REGS_AXI_SEGMT_NUMB; i++){
			printk("[%08x] ",readl(chip->reg_base + i*4));
		}
	}
	/**
	 * dump key register of smmu
	 */
	pr_err("\n irq status= %08x\n"
		" SMMU_CTRL_OFFSET = %08x\n"
		" SMMU_ENABLE_OFFSET = %08x\n"
		" SMMU_PTBR_OFFSET = %08x\n"
		" SMMU_START_OFFSET = %08x\n"
		" SMMU_END_OFFSET = %08x\n"
		" SMMU_IPTSRC_OFFSET = %08x\n",
		irq_stat,
		readl(chip->reg_base + SMMU_CTRL_OFFSET),
		readl(chip->reg_base + SMMU_ENABLE_OFFSET),
		readl(chip->reg_base + SMMU_PTBR_OFFSET),
		readl(chip->reg_base + SMMU_START_OFFSET),
		readl(chip->reg_base + SMMU_END_OFFSET),
		readl(chip->reg_base + SMMU_IPTSRC_OFFSET));
	/**
	 * test bit0 to bit5
	 */
	if (irq_stat & 0x1){
		pr_err("page_size L1 TLB_size ddr_size configure error \n");
	}else if (irq_stat & (1<<1)){
		index = (pc_pgt - pgt)/4;
		pr_err("page table entry is error:IOMMU_VA=0x%x \n",(index * SZ_4K));
	}else if (irq_stat & (1<<2)) {
		pr_err("AXI master0 receive error response \n");
	}else if (irq_stat & (1<<3)) {
		pr_err("AXI master1 receive error response \n");
	}else if (irq_stat & (1<<4)) {
		pr_err("AXI master0 access timeout,not receive peer's response \n");
	}else if (irq_stat & (1<<5)) {
		pr_err("AXI master1 access timeout,not receive peer's response \n");
	}

	BUG_ON(irq_stat&0x3f);

	return IRQ_HANDLED;
}




static int hi6xxx_smmu_probe(struct platform_device *pdev)
{
	int ret;
	size_t sz;
	u32 range[4];
	dma_addr_t  pgtbl_phy;
	unsigned int *ver = NULL;
	struct resource *res = NULL;
	struct hi6xxx_smmu_chip *smmu_chip  = NULL;
	struct device_node *np  = pdev->dev.of_node;

	pr_info("in %s smmu driver is start\n",__func__);

	smmu_chip = devm_kzalloc(&pdev->dev, sizeof(*smmu_chip), GFP_KERNEL);
	if (!smmu_chip){
		pr_err("in %s devm_kzalloc is failed\n",__func__);
		return -ENOMEM;
	}
	smmu_dev_keeper.smmu_chip = smmu_chip;

	/**
	 * get smmu version
	 */
	ver = (unsigned int *)of_get_property(np, "hisi,smmu-version", NULL);
	if(ver){
		smmu_chip->chip_ver = be32_to_cpu(*ver);
		pr_info("in %s smmu version is %u\n",__func__,be32_to_cpu(*ver));
	}

	/**
	 * get IOMEM res
	 */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		pr_err("platform_get_resource err\n");
		return -ENOENT;
	}
	if (!request_mem_region(res->start,resource_size(res),pdev->name)) {
		pr_err("cannot claim register memory\n");
		return -ENOMEM;
	}
	smmu_chip->reg_base = ioremap(res->start,resource_size(res));
	if (!smmu_chip->reg_base) {
		pr_err("cannot map register memory\n");
		return -ENOMEM;
	}

	/**
	 * enable clks
	 */
	smmu_chip->media_sc_clk = of_clk_get(np, 2);
	if(!smmu_chip->media_sc_clk){
		pr_err("in %s of media_sc_clk failed\n",__func__);
		return -ENODEV;
	}
	if (0 != clk_prepare_enable(smmu_chip->media_sc_clk)) {
		pr_err("clk_prepare_enable media_sc_clk is falied\n");
		return -ENODEV;
	}

	smmu_chip->smmu_peri_clk  = of_clk_get(np, 0);
	if(!smmu_chip->smmu_peri_clk){
		pr_err("in %s of get clk peri failed\n",__func__);
		return -ENODEV;
	}
	if (0 != clk_prepare_enable(smmu_chip->smmu_peri_clk)) {
		pr_err("clk_prepare_enable smmu_peri_clk is falied\n");
		return -ENODEV;
	}

	smmu_chip->smmu_clk  = of_clk_get(np, 1);
	if(!smmu_chip->smmu_clk){
		pr_err("in %s get smmu_clk failed\n",__func__);
		return -ENODEV;
	}
	if (0 != clk_prepare_enable(smmu_chip->smmu_clk)) {
		pr_err("clk_prepare_enable smmu_clk is falied\n");
		return -ENODEV;
	}

	/**
	 * "hisi,smmu-iodomain = <0x2000 0x20000000 0x2000 0x2000>" at dts
	 */
	if (!of_property_read_u32_array(np, "hisi,smmu-iodomain", range,
			ARRAY_SIZE(range))) {
		smmu_dev_keeper.iova_start = range[0];
		smmu_dev_keeper.iova_sz    = range[1];
		smmu_dev_keeper.pg_sz      = range[2];
		smmu_dev_keeper.align      = range[3];


	}else{
		/*default setting*/
		pr_err("in %s get param from default setting\n",__func__);
		smmu_dev_keeper.iova_start = 0x2000;
		smmu_dev_keeper.iova_sz    = 0x20000000;
		smmu_dev_keeper.pg_sz      = 0x1000;
		smmu_dev_keeper.align      = 0x1000;

	}

	/**
	 * because of at v8rx pgtbl base must align SZ_1M
	 * we can set domain = 1G, page size = 4k; or domain = 1G,page size = 8k
	 */
	sz = smmu_dev_keeper.iova_sz/smmu_dev_keeper.pg_sz*sizeof(smmu_pte_t);
	smmu_dev_keeper.pgtbl_sz = sz;
	//sz = ALIGN(sz + (SZ_1M - 1))&(~(SZ_1M - 1));
	sz = ALIGN(sz ,SZ_1M);
	smmu_dev_keeper.pgtbl_vir = alloc_pgtbl(&pdev->dev,sz,&pgtbl_phy,0);
	if (!smmu_dev_keeper.pgtbl_vir){
		pr_err("alloc pgtbl is failed\n");
		return -ENOMEM;
	}
	smmu_dev_keeper.pgtbl_phy = pgtbl_phy;
	memset(smmu_dev_keeper.pgtbl_vir, 0, SZ_1M);

	/**
	 * get IRQ res
	 */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res){
		pr_err("in %s get IRQ IS failed\n",__func__);
		return -ENODEV;
	}
	smmu_chip->irq = res->start;
	smmu_chip->smmu_isr = hi6xxx_smmu_isr;

	/**
	 * smmu core init,but some is set at fastboot.
	 * in fastboot function smmu_enable()
	 * in this function enable irq,and register smmu irq must after enable
	 */
	smmu_enable(&pdev->dev,&smmu_dev_keeper);

	build_ops(&smmu_dev_keeper,&hi6xxx_smmu_ops);
	ret = bus_set_iommu(&platform_bus_type, &hi6xxx_smmu_ops);
	if (!!ret){
		pr_err("bus_set smmu is failed \n");
		return ret;
	}

	pr_info("in %s smmu driver is end\n",__func__);

	return 0;
}



static int hi6xxx_smmu_resume(struct platform_device *pdev)
{
	int wait_cnt = 10000;
	unsigned int reg = 0;
	unsigned int value = 0;
	size_t    pgtbl_sz = smmu_dev_keeper.pgtbl_sz;
	dma_addr_t  pgtbl_phy = smmu_dev_keeper.pgtbl_phy;
	struct hi6xxx_smmu_chip *smmu_chip = smmu_dev_keeper.smmu_chip;

	/**
	 * enable smmu peri clk
	 */
	if(smmu_chip->media_sc_clk){
		if (0 != clk_prepare_enable(smmu_chip->media_sc_clk)) {
			pr_err("clk_prepare_enable media_sc_clk is falied\n");
			return -ENODEV;
		}
	}

	/**
	 * enable smmu media clk
	 */
	if(smmu_chip->smmu_clk){
		if (0 != clk_prepare_enable(smmu_chip->smmu_clk)) {
			pr_err("clk_prepare_enable smmu_clk is falied\n");
			return -ENODEV;
		}
	}

	/**
	 * disreset SMMU
	 */
	value = readl((void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
	writel((value|(0x1<<7)),(void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));

	/**
	 * bypass disable smmu
	 */
	value = readl((void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
	writel((value&(~(0x1<<2))),(void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));

	reload_regs(smmu_chip);

	writel(pgtbl_phy + sizeof(smmu_pte_t),smmu_chip->reg_base + SMMU_START_OFFSET);
	writel(pgtbl_phy + pgtbl_sz,smmu_chip->reg_base + SMMU_END_OFFSET);

	/**
	 * enable smmu
	 */
	writel(smmu_regs_value[1],smmu_chip->reg_base+SMMU_ENABLE_OFFSET);
	writel((smmu_regs_value[0]|(1<<10)),smmu_chip->reg_base+SMMU_CTRL_OFFSET);
	while(--wait_cnt) {
		reg = readl((smmu_chip->reg_base+SMMU_CTRL_OFFSET));
		if ( 0x0 == (reg & 0x400)){
			pr_err("smmu init success\n");
			break;
		}
	}

	WARN_ON(!wait_cnt);
	smmu_chip->chip_state = SMMU_RUNNING;
	return 0;
}


static int hi6xxx_smmu_suspend(struct platform_device *pdev , pm_message_t state)
{
	unsigned int value = 0;
	struct hi6xxx_smmu_chip *smmu_chip = smmu_dev_keeper.smmu_chip;

	restore_regs(smmu_chip);

	value = readl((void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
	writel((value|(0x1<<2)),(void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));

	/**
	* reset SMMU
	*/
	value = readl((void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
	writel((value&(~(0x1<<7))),(void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));

	if(smmu_chip->smmu_clk){
		clk_disable_unprepare(smmu_chip->smmu_clk);
	}

	if(smmu_chip->media_sc_clk){
		clk_disable_unprepare(smmu_chip->media_sc_clk);
	}

	smmu_chip->chip_state = SMMU_SUSPEND;

	return 0;
}

static struct of_device_id of_smmu_match_tbl[] = {
	{
		.compatible = "hisi,hi6xxx-smmu",
	},
	{ /* end */ }
};

static struct platform_driver hi6xxx_smmu_driver = {
	.driver  ={
		.owner = THIS_MODULE,
		.name = "hi6xxx-smmu",
		.of_match_table = of_smmu_match_tbl,
	},
	.probe  =  hi6xxx_smmu_probe,
#ifdef CONFIG_PM
	.suspend = hi6xxx_smmu_suspend,
	.resume  = hi6xxx_smmu_resume,
#endif
};

static int __init hi6xxx_smmu_init(void)
{
	int ret;

	ret = platform_driver_register(&hi6xxx_smmu_driver);

	return ret;
}
subsys_initcall(hi6xxx_smmu_init);

/**
 * get ddr size from cmdline
 */
static int __init early_hisi_ddr_size(char *p)
{
	phys_addr_t size;
	char *endp;
	size  = memparse(p, &endp);
	smmu_ddr_size = size;
	return 0;
}
early_param("iommu_ddr_size", early_hisi_ddr_size);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("hisilicon");
