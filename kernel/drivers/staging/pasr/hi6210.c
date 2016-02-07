/*
 * Device driver for regulators in Hi6421 IC
 *
 * Copyright (C) 2013 Linaro Ltd. Guodong Xu <guodong.xu@linaro.org>
 *
 * Porting to hi6210 by Young Choi.
 * Copyright (C) 2014 Hisilicon, Young Choi <cuiyong1@hisilicon.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <linux/io.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/pasr.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/hwspinlock.h>
#include <linux/types.h>


#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>


#define ACCESS_DDR_MR17_BY_AP

#define MASK_BTYE(n)                        (0xFF << (n * 8))
#define CLEAR_BYTE(n)                       (~(0xFF << (n * 8)))
#define SCBAKDATA(n)                        (0x314 + ((n) << 2))

#define RANK_DISABLE(n)                     (1 << n)
#define SFC_RANK(n)                         (1 << (n + 16))
#define SFC_CMD_REQ                         (1)
#define SFC_MR8                             (8)
#define SFC_MR17                            (17)
#define SFC_LOAD_MR                         (2)
#define SFC_READ_MR                         (7)
#define DDR_MR8_DENSITY_OFF                 (2)
#define DDR_MR8_DENSITY_MASK                (0xF << DDR_MR8_DENSITY_OFF)
#define DDR_RANK_NR_OFF                     (20)
#define DDR_RANK_NR_MASK                    (0x3 << DDR_RANK_NR_OFF)
#define DDR_RANK_SIZE_512M                      (0x20000000)
#define DDR_RANK_SIZE_1024M                      (0x40000000)
#define DDR_MR17_MASK                       (0xFF)
#define DDR_MR17_RANK_512M_MASK            (0x00)
#define DDR_MR17_RANK_1024M_MASK            (0x00)
#define DDR_HWLOCK_ID                       (22)
#define DDR_HWLOCK_TIMEOUT                  (1000)

#define DDRC_DMC_OFFSET                     (0x0000)
#define DDRC_CFG_WORKMODE2               (DDRC_DMC_OFFSET + 0x044)
#define DDRC_CFG_DDRMODE                 (DDRC_DMC_OFFSET + 0x050)
#define DDRC_CFG_SFC                     (DDRC_DMC_OFFSET + 0x210)
#define DDRC_CFG_SFC_ADDR1               (DDRC_DMC_OFFSET + 0x218)
#define DDRC_HIS_SFC_RDATA0              (DDRC_DMC_OFFSET + 0x4A8)
#define DDRC_CTRL_SFC                    (DDRC_DMC_OFFSET + 0x00C)

struct ddr_rank_info {
	u32 addr;
	u32 size;
	u32 memreg;
};

static struct hwspinlock *ddr_hwlock = NULL;
static void __iomem *ddrc_base = NULL;
static spinlock_t pasr_lock;

#ifndef ACCESS_DDR_MR17_BY_AP
static void __iomem *sysctrl_base = NULL;
#endif

static u32 rank_nr = 0;
static struct ddr_rank_info rank_info[PASR_MAX_DIE_NR];

static ssize_t show_mem_regs(struct device *dev,
			struct device_attribute *devattr, char *buf);

static DEVICE_ATTR(memregs, S_IRUGO, show_mem_regs, NULL);

static struct attribute *hi6210_pasr_attributes[] = {
	&dev_attr_memregs.attr,
	NULL,
};

static const struct attribute_group hi6210_attr_group = {
	.attrs = hi6210_pasr_attributes,
};

static ssize_t show_mem_regs(struct device *dev,
			struct device_attribute *devattr, char *buf)
{
	struct ddr_rank_info *info;
	int i,size = 0;

	for(i = 0; i < rank_nr; i++) {
		info = &rank_info[i];
		size += snprintf(buf + size, PAGE_SIZE / rank_nr,
			"%s: rank%d addr=0x%08x, size = 0x%08x, mem_reg = 0x%08x\n",
			__func__, i, info->addr, info->size, info->memreg);
	}

	return size;
}

static void hi6210_pasr_refresh_disable(int rank)
{
	u32 value;

	value = readl(ddrc_base + DDRC_CFG_WORKMODE2) | RANK_DISABLE(rank);
	writel(value, ddrc_base + DDRC_CFG_WORKMODE2);
}

static void hi6210_pasr_refresh_enable(int rank)
{
	u32 value;

	value = readl(ddrc_base + DDRC_CFG_WORKMODE2) & (~RANK_DISABLE(rank));
	writel(value, ddrc_base + DDRC_CFG_WORKMODE2);
}

/* send sfc cmd to rank */
static void hi6210_pasr_send_sfc_cmd(u32 cmd, int rank)
{
	int timeout = 500;

	writel(cmd, ddrc_base + DDRC_CFG_SFC);

	writel(SFC_RANK(rank), ddrc_base + DDRC_CFG_SFC_ADDR1);

	writel(SFC_CMD_REQ, ddrc_base + DDRC_CTRL_SFC);

	while ((readl(ddrc_base + DDRC_CTRL_SFC)) & SFC_CMD_REQ) {
		if (timeout-- < 0) { printk(KERN_CRIT"%s: wait cmd done timeout!\n", __func__);
			break;
		}
	}
}

static u32 hi6210_pasr_read_ddr_mr(int rank, int target)
{
	u32 value = 0, cmd;

	if (hwspin_lock_timeout(ddr_hwlock, DDR_HWLOCK_TIMEOUT)) {
		pr_err("%s: hwspinlock timeout!\n", __func__);
		return value;
	}

	hi6210_pasr_refresh_disable(rank);
	cmd = ((target << 4) | SFC_READ_MR);    
	hi6210_pasr_send_sfc_cmd(cmd, rank);
	value = readl(ddrc_base + DDRC_HIS_SFC_RDATA0);
	hi6210_pasr_refresh_enable(rank);

	hwspin_unlock(ddr_hwlock);

	return value;
}

static void hi6210_pasr_write_ddr_mr(u32 value , int rank, int target)
{
	u32 cmd;

	if (hwspin_lock_timeout(ddr_hwlock, DDR_HWLOCK_TIMEOUT)) {
		pr_err("%s: hwspinlock timeout!\n", __func__);
		return ;
	}

	hi6210_pasr_refresh_disable(rank);
	cmd = ((value << 12) | (target << 4) | SFC_LOAD_MR);
	hi6210_pasr_send_sfc_cmd(cmd, rank);
	hi6210_pasr_refresh_enable(rank);

	hwspin_unlock(ddr_hwlock);
}

static u32 hi6210_pasr_mask_reserved_ddr_region(u32 value, int rank)
{
	u32 size = rank_info[rank].size;
	value &= DDR_MR17_MASK;

	if(rank)
		return value;

	switch (size) {
	case DDR_RANK_SIZE_512M:
		value |= DDR_MR17_RANK_512M_MASK; /* FIXME */
		break;
	case DDR_RANK_SIZE_1024M:
		value |= DDR_MR17_RANK_1024M_MASK;
		break;
	default:
		printk(KERN_CRIT "%s: invalid rank size %u.\n",
			__func__, value);
		break;
	}

	return value;
}

#ifndef ACCESS_DDR_MR17_BY_AP
static void hi6210_pasr_write_sysctrl_reg(u32 value, int rank)
{
	u32 reg = readl(sysctrl_base + SCBAKDATA(11));
	reg &= CLEAR_BYTE(rank);
	reg |= (value << (rank * 8));
	writel(reg, sysctrl_base + SCBAKDATA(11));
}
#endif

static void hi6210_pasr_apply_mask(u32 *mem_reg, void *cookie)
{
	int rank = (int)cookie;
	u32 value = *mem_reg;
    unsigned int i = 0;
    printk(KERN_ERR"%s: %d: rank: %d; value: 0x%x\n", __func__, __LINE__, rank, value);

	if ((rank >= rank_nr) || (rank < 0)) {
		printk(KERN_CRIT "%s: invalid rank value %d.\n",
				__func__, rank);
		return ;
	}

	rank_info[rank].memreg = value; /* the reg value */
	//value = hi6210_pasr_mask_reserved_ddr_region(value, rank);  /* after masked */

#define PASS_TO_MCU
#ifndef ACCESS_DDR_MR17_BY_AP
	if (sysctrl_base != NULL)
		hi6210_pasr_write_sysctrl_reg(value, rank);
#elif defined(PASS_TO_MCU)
#define MEM_DDR_PASR_ADDR  (phys_addr_t)(DDR_PASR_ADDR)
    unsigned int *pasr_addr = (unsigned int *)ioremap(MEM_DDR_PASR_ADDR+4, SZ_4K);
    pasr_addr[rank*2] = rank;
    pasr_addr[rank*2+1] = value;
    printk(KERN_ERR"pass pasr argument for use of mcu [0x%x]:0x%x, [0x%x]:0x%x\n", 
                       &pasr_addr[rank*2], pasr_addr[rank*2], &pasr_addr[rank*2+1], pasr_addr[rank*2+1]);
    for(i=0;i<4;i++) {
        printk(KERN_ERR"[0x%x]:0x%x\n", &pasr_addr[i], pasr_addr[i]);         
    }
#else
	if (ddr_hwlock != NULL)
		hi6210_pasr_write_ddr_mr(value, rank, SFC_MR17);
#endif
}

/* die per rank */
static u32 hi6210_pasr_die_per_rank(u8 mr8_value)
{
    unsigned int io_width = mr8_value >> 6;
    int die_num = 0; 
    switch (io_width)
    {    
       case (0): 
            die_num = 1; 
            break;
       case (1): 
            die_num = 2; 
            break;
       default:
		    printk(KERN_CRIT "%s: invalid mr8 value %u.\n",
				__func__, mr8_value);
		    break;
    }    
    printk(KERN_ERR"%d die per rank\n", die_num);
    return die_num;  

}

/* get size per rank */
static u32 hi6210_pasr_get_rank_size(int rank)
{
	u32 size = 0;
    u8 density = 0;
	u8 orig_mr8_value = hi6210_pasr_read_ddr_mr(rank, SFC_MR8);

	density = (orig_mr8_value & DDR_MR8_DENSITY_MASK) >> DDR_MR8_DENSITY_OFF; 

	switch (density) {
	case 0x6:
		size = DDR_RANK_SIZE_512M;
		break;
	case 0x7:
		size = DDR_RANK_SIZE_1024M;
		break;
	default:
		printk(KERN_CRIT "%s: invalid density value %u.\n",
				__func__, density);
		break;
	}

    size *= hi6210_pasr_die_per_rank(orig_mr8_value);
	return size;
}


static void hi6210_pasr_get_rank_info(void)
{
	int i;
	u32 size;
	unsigned long flags = 0;

	rank_nr = readl(ddrc_base + DDRC_CFG_DDRMODE) & DDR_RANK_NR_MASK;
	rank_nr = (rank_nr >> DDR_RANK_NR_OFF) + 1;   /* num of rank in ddr chip */
	memset(rank_info, 0, sizeof(rank_info));

	for (i = 0; i < rank_nr; i++) {
		spin_lock_irqsave(&pasr_lock, flags);
		size = hi6210_pasr_get_rank_size(i);
		spin_unlock_irqrestore(&pasr_lock, flags);

		if (size == 0)
			break;

		rank_info[i].size = size;

		if (i > 0)
			rank_info[i].addr = rank_info[i-1].addr + rank_info[i-1].size;
	}
}

static int hi6210_pasr_probe(struct platform_device *pdev)
{
	struct device_node *np;
	struct device *dev = &pdev->dev;
	int err, i;

	dev_crit(dev, "%s: enter\n", __func__);
	//printk(KERN_ERR"%s: enter\n", __func__);

	np = of_find_compatible_node(NULL, NULL, "hisilicon,ddr_pasr");
	if (IS_ERR(np)) {
		dev_err(dev, "can not find hisilicon,pasr node\n");
		return -ENODEV;
	}

#ifndef ACCESS_DDR_MR17_BY_AP
	sysctrl_base = of_iomap(np, 2);
	if (sysctrl_base == NULL) {
		dev_err(dev, "failed to iomap ddrc regs 1!\n");
		return -ENXIO;
	}
#endif

	ddrc_base = of_iomap(np, 0);
	if (ddrc_base == NULL) {
		dev_err(dev, "failed to iomap ddrc regs 2!\n");
		return -ENXIO;
	}

	spin_lock_init(&pasr_lock);

	ddr_hwlock = hwspin_lock_request_specific(DDR_HWLOCK_ID);
	if (ddr_hwlock == NULL) {
		dev_err(dev, "pasr get hwspin_lock failed\n");
		return -EBUSY;
	}

	hi6210_pasr_get_rank_info();

	for (i = 0; i < rank_nr; i++) {
        printk(KERN_ERR"rank_info[%d]: addr: 0x%x; size: 0x%x; memreg: 0x%x\n", i, rank_info[i].addr, rank_info[i].size, rank_info[i].memreg);
		if (pasr_register_mask_function(rank_info[i].addr,
					&hi6210_pasr_apply_mask, (void *)i)) {
			dev_err(dev, "pasr register failed, rank %d\n", i);
		}
	}

	err = sysfs_create_group(&pdev->dev.kobj, &hi6210_attr_group);
	if (err < 0) {
		dev_err(dev, "create sysfs group failed (%d)\n", err);
		return err;
	}

	dev_crit(dev, "%s: exit\n", __func__);

	return 0;
}

static int hi6210_pasr_remove(struct platform_device *pdev)
{
	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id of_hi6210_pasr_match_tbl[] = {
	{
		.compatible = "hisilicon,ddr_pasr",
	},
	{ /* end */ }
};
MODULE_DEVICE_TABLE(of, of_hi6210_pasr_match_tbl);
#endif

static struct platform_driver hi6210_pasr_driver = {
	.driver = {
		.name	= "hi6210_pasr",
		.owner	= THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(of_hi6210_pasr_match_tbl),
#endif
	},
	.probe	= hi6210_pasr_probe,
	.remove	= hi6210_pasr_remove,
};
module_platform_driver(hi6210_pasr_driver);

MODULE_AUTHOR("WUXX wujianhua1<@hisilicon.com>");
MODULE_DESCRIPTION("Hi6210 PASR driver");
MODULE_LICENSE("GPL v2");
