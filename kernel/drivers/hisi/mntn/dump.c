/*
 *  linux/arch/arm/mach-k3v2/dump.c
 *
 * balong memory/register proc-fs  dump implementation
 *
 * Copyright (C) 2012 Hisilicon, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/mm.h>
#include <linux/sizes.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
//#include <mach/boardid.h>   /*add by y65256 for wdg log save*/
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/highmem.h>

#ifdef CONFIG_ARCH_HI6XXX
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <general_sram_map.h>
#include <soc_baseaddr_interface.h>
#include <soc_pmctrl_interface.h>
#include "soc_peri_sctrl_interface.h"
#include "soc_modem_sctrl_interface.h"
#include "soc_ao_sctrl_interface.h"
#include "MemoryMap.h"
#include <excDrv.h>
#endif

#include <asm/uaccess.h>
#include <linux/hisi/util.h>
#include <linux/hisi/reset.h>

/*#include <bsp_ao_sctrl.h>*/

#define TRANSFER_BASE       (16)

#define MODEM_SYS_MEM_SAVE_SIZE 0x06F00000
#define STRINGLEN 9
#define MEM_DUMP_EVENT_FLAG     0x80000000
#define MAX_LEN_OF_RSTLOGADDR_STR  30  /*modified by z00210919*/

unsigned int g_dump_flag = 0;
unsigned int g_ddr_size = 0x80000000;/*just for compile by x00195127*/

#ifdef CONFIG_ARCH_HI6XXX
#define AO_SCTRL_SC_SYS_CTRL1    SOC_AO_SCTRL_SC_SYS_CTRL1_ADDR(g_ao_sc_map_addr)
static void __iomem *g_ao_sc_map_addr = NULL;
#endif

extern void  etb_nve_read(unsigned char *config);
extern unsigned long hisi_get_reserve_mem_size(void);
extern int memblock_free(phys_addr_t base, phys_addr_t size);

char core_flag[STRINGLEN];
struct proc_dir_entry *core_trace, *core_flag_file;


/*dump file information, set to file->private_data*/
struct dump_info
{
    void  *p;                        /*dump region phy/virtual address*/
    loff_t size;                     /*dump region size*/
};

static inline void *memdump_remap_type(unsigned long phys_addr, size_t size, pgprot_t pgprot)
{
    int i;
    u8* vaddr;
    int npages = PAGE_ALIGN((phys_addr & (PAGE_SIZE - 1)) + size) >> PAGE_SHIFT;
    unsigned long offset = phys_addr & (PAGE_SIZE - 1);
    struct page **pages;
    pages = vmalloc(sizeof(struct page *) * npages);
    if (!pages)
    {
        printk(KERN_ERR "%s: vmalloc return NULL!\n", __FUNCTION__);
        return NULL;
    }
    pages[0] = phys_to_page(phys_addr);
    for (i = 0; i < npages - 1 ; i++) {
        pages[i + 1] = pages[i] + 1;
    }
    vaddr = (u8*)vmap(pages, npages, VM_MAP, pgprot);
    if(vaddr == 0)
    {
        printk(KERN_ERR "%s: vmap return NULL!\n", __FUNCTION__);
    }
    else
    {
        vaddr += offset;
    }
    vfree(pages);
    return (void *)vaddr;
}
#define memdump_remap(phys_addr, size) memdump_remap_type(phys_addr, size, PAGE_KERNEL)
#define memdump_unmap(vaddr) vunmap(vaddr)

/*read dump file content*/
static ssize_t dump_phy_mem_proc_file_read(struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{
    struct dump_info *info = (struct dump_info *)file->private_data;
    void __iomem *p;
    ssize_t copy;

    if (!info){
        printk(KERN_ERR"the proc file don't be created in advance.\n");
        return 0;
    }
    if (*off > info->size){
        printk(KERN_ERR"read offset error 1\n");
        return 0;
    }

    if (*off == info->size){
        /*end of file*/
        return 0;
    }

    copy = (ssize_t)min(bytes, (size_t)(info->size - *off));

    p = memdump_remap((phys_addr_t)((char*)info->p + *off), copy);
    if (NULL == p){
        printk(KERN_ERR"%s ioremap fail \n", __FUNCTION__);
        return -ENOMEM;
    }

    if (copy_to_user(userbuf, p, copy)){
        printk(KERN_ERR"%s copy to user error \n", __FUNCTION__);
        copy = -EFAULT;
        goto copy_err;
    }

    *off += copy;

copy_err:
    memdump_unmap(p);
    return copy;
}

static int dump_phy_mem_proc_file_open(struct inode *inode, struct file *file)
{
    file->private_data = PDE_DATA(inode);

    return 0;
}

static int dump_phy_mem_proc_file_release(struct inode *inode, struct file *file)
{
    file->private_data = NULL;

    return 0;

}

static const struct file_operations dump_phy_mem_proc_fops = {
    .open = dump_phy_mem_proc_file_open,
    .read = dump_phy_mem_proc_file_read,
    .release = dump_phy_mem_proc_file_release,
};

/* create memory dump file to dump phy memory */
void create_dump_phy_mem_proc_file(char *name, unsigned int phy_addr, size_t size)
{
    struct dump_info *info;

    /*as a public interface, we should check the parameter*/
    if ((NULL == name ) ||(0 == phy_addr) || (0 == size)){
        printk(KERN_ERR"%s %d parameter error : name 0x%p phy_addr 0x%d size %lu\r\n", __FUNCTION__, __LINE__, name, \
            phy_addr, size);
        return;
    }

    info = (struct dump_info *)kmalloc(sizeof(struct dump_info), GFP_KERNEL);
    if (NULL == info){
        printk(KERN_ERR"%s kmalloc fail !\r\n", __FUNCTION__);
        return;
    }

    info->p = (void *)((unsigned long)phy_addr);
    info->size= size;

    balong_create_memory_proc_entry(name, S_IRUGO | S_IWUGO, &dump_phy_mem_proc_fops, info);

    return;
}


#define DUMP(name, base, iosize, string, directory, prop)\
static int __init name ## _dump_phy_mem_proc_file_init(void)\
{\
    struct dump_info *info = NULL;\
\
    if (!prop){\
        return 0;\
    }\
\
    info = (struct dump_info *)kmalloc(sizeof(struct dump_info), GFP_KERNEL);\
    if (NULL == info){\
        printk(KERN_ERR"%s kmalloc fail !\r\n", __FUNCTION__);\
        return -ENOMEM;\
    }\
\
    info->p = (void *)base;\
    info->size = iosize;\
\
    balong_create_ ## directory ## _proc_entry(string, S_IRUGO, &dump_phy_mem_proc_fops, info);\
\
    return 0;\
}\
\
static void __exit name ## _dump_phy_mem_proc_file_exit(void)\
{\
    if (!prop){\
        return;\
    }\
\
    balong_remove_ ## directory ## _proc_entry(string);\
\
    return;\
}\
module_init(name ## _dump_phy_mem_proc_file_init);\
module_exit(name ## _dump_phy_mem_proc_file_exit);

/*definition of register/memory dump in ascii mode*/

#ifdef CONFIG_ARCH_HI6XXX
/*memory/register dump declaration*/
DUMP(mcu_mem, MCU_SYS_MEM_ADDR, MCU_SYS_MEM_SIZE, "mcu", memory, himntn_gobal_resetlog)
DUMP(tele_mntn_mem, TELE_MNTN_AREA_ADDR, TELE_MNTN_AREA_SIZE, "telemntn", memory, himntn_gobal_resetlog)
DUMP(modem_mem, MODEM_SYS_MEM_ADDR, MODEM_SYS_MEM_SAVE_SIZE, "modem", memory, himntn_gobal_resetlog)
DUMP(sram_acpu_mem, SOC_SRAM_OFF_BASE_ADDR, SRAM_SIZE, "sram_acpu", memory, himntn_gobal_resetlog)
DUMP(mcu_log, MCU_DUMP_LOG_ADDR, MCU_DUMP_LOG_SIZE, "mcu", log, himntn_gobal_resetlog)
DUMP(android_log, ANDROID_DUMP_LOG_ADDR, ANDROID_DUMP_LOG_SIZE, "android", log, himntn_gobal_resetlog)
static int dump_sram_mcu_mem_proc_file_open(struct inode *inode, struct file *file)
{
    struct dump_info *info = NULL;
    void __iomem * sys_ctrl1_addr;
    unsigned int sc_ctrl1;

    info = (struct dump_info *)kmalloc(sizeof(struct dump_info), GFP_KERNEL);
    if (NULL == info){
        printk(KERN_ERR"%s kmalloc fail !\r\n", __FUNCTION__);
        return -ENOMEM;
    }

    info->p = (void *)REG_BASE_SRAM_MCU;
    info->size = REG_SRAM_MCU_IOSIZE;
    file->private_data = (void *)info;

    sys_ctrl1_addr = AO_SCTRL_SC_SYS_CTRL1;
    sc_ctrl1 = readl(sys_ctrl1_addr);
    sc_ctrl1 |= 0x80000000;
    sc_ctrl1 &= ~0x00008000;
    writel(sc_ctrl1, sys_ctrl1_addr);

    return 0;
}

static int dump_sram_mcu_mem_proc_file_release(struct inode *inode, struct file *file)
{
    struct dump_info *info = (struct dump_info *)file->private_data;
    void __iomem * sys_ctrl1_addr;
    unsigned int sc_ctrl1;

    kfree(info);

    sys_ctrl1_addr = AO_SCTRL_SC_SYS_CTRL1;
    sc_ctrl1 = readl(sys_ctrl1_addr);
    sc_ctrl1 |= 0x80008000;
    writel(sc_ctrl1, sys_ctrl1_addr);

    file->private_data = NULL;
    return 0;
}

static const struct file_operations sram_mcu_mem_proc_fops = {
    .open = dump_sram_mcu_mem_proc_file_open,
    .read = dump_phy_mem_proc_file_read,
    .release = dump_sram_mcu_mem_proc_file_release,
};

static int __init sram_mcu_mem_dump_proc_init(void)
{
    if (!himntn_gobal_resetlog){
        return 0;
    }
    balong_create_memory_proc_entry("sram_mcu", S_IRUGO, &sram_mcu_mem_proc_fops, NULL);

    g_ao_sc_map_addr = ioremap(REG_BASE_SC_ON, REG_SC_ON_IOSIZE);
    if (NULL == g_ao_sc_map_addr)
    {
        printk(KERN_ERR"%s: g_ao_sc_map_addr remap error.\n", __FUNCTION__);
        return -ENOMEM;
    }

    return 0;
}

static void __exit sram_mcu_mem_dump_proc_exit(void)
{
    if (!himntn_gobal_resetlog){
        return;
    }
    balong_remove_memory_proc_entry("sram_mcu");
    iounmap(g_ao_sc_map_addr);

    return;
}
module_init(sram_mcu_mem_dump_proc_init);
module_exit(sram_mcu_mem_dump_proc_exit);

static phys_addr_t g_BitMapOfAbnRst = 0;
static int __init early_parse_bitmap_abnrst_cmdline(char *p)
{
    char bitmapabnrst[MAX_LEN_OF_RSTLOGADDR_STR];
    char *endptr = NULL;

    hisi_io_memset(bitmapabnrst, 0, MAX_LEN_OF_RSTLOGADDR_STR);

        strncpy(bitmapabnrst, p, MAX_LEN_OF_RSTLOGADDR_STR - 1);
        bitmapabnrst[MAX_LEN_OF_RSTLOGADDR_STR - 1] = '\0';

    g_BitMapOfAbnRst = simple_strtoul(bitmapabnrst, &endptr, TRANSFER_BASE);

    printk(KERN_ERR "[early_parse_bitmap_abnrst_cmdline]bitmap of abnormal reset flag p:%s, g_BitMapOfAbnRst :0x%lx\n", (const char *)p, (unsigned long)g_BitMapOfAbnRst);

    if (g_BitMapOfAbnRst & MEM_DUMP_EVENT_FLAG)
    {
        g_dump_flag = 0xFACE; /* set flag to reserve 1G~1.5G memory addr */
    }

    return 0;
}
early_param("bitmapofabnrst", early_parse_bitmap_abnrst_cmdline);
#endif

/*保存fastboot传递过来的异常区地址信息*/
phys_addr_t g_memdump_addr = 0;
phys_addr_t g_memdump_end = 0;
unsigned int  g_memdump_size = 0;
#ifndef CONFIG_ARCH_HI6XXX
int himntn_gobal_resetlog =0;
#endif
#define MAX_MEMDUMP_NAME 24

struct memdump
{
    char name[MAX_MEMDUMP_NAME];
    unsigned int base;
    unsigned int size;
};

static ssize_t dump_end_proc_read(struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{
    phys_addr_t addr;
    struct page *page;

    for (addr = g_memdump_addr; addr < (g_memdump_addr+g_memdump_size); addr += PAGE_SIZE) {
        page = pfn_to_page(addr >> PAGE_SHIFT);
      free_reserved_page(page);
#ifdef CONFIG_HIGHMEM
       if (PageHighMem(page))
         totalhigh_pages ++;
#endif
    }

    memblock_free(g_memdump_addr,g_memdump_size);
	
    pr_err("dump_end_proc_read:g_memdump_addr=0x%x,g_memdump_end=0x%x,g_memdump_size=0x%x \n",(unsigned int)g_memdump_addr,(unsigned int)g_memdump_end,g_memdump_size);
    pr_info("%s:addr%lu\n", __func__, (unsigned long)addr);
    g_memdump_addr = 0;
    g_memdump_end = 0;
    g_memdump_size = 0;
    return 0;
}

static const struct file_operations dump_end_proc_fops = {
    .read = dump_end_proc_read,
};

static int __init early_parse_memdumpaddr_cmdline(char *p)
{
    char memdump_addr[MAX_LEN_OF_RSTLOGADDR_STR];
    char memdump_end[MAX_LEN_OF_RSTLOGADDR_STR];
    char memdump_size[MAX_LEN_OF_RSTLOGADDR_STR];
    char *endptr = NULL;

    memcpy(memdump_addr, p, 10);
    memdump_addr[10] = 0;
    memcpy(memdump_end, p+11, 10);
    memdump_end[10] = 0;    
    memcpy(memdump_size, p+22, 10);
    memdump_size[10] = 0; 
	
    g_memdump_addr = simple_strtoul(memdump_addr, &endptr, TRANSFER_BASE);
    g_memdump_end  = simple_strtoul(memdump_end, &endptr, TRANSFER_BASE);
    g_memdump_size = simple_strtoul(memdump_size, &endptr, TRANSFER_BASE);
    pr_err("[early_parse_memdumpaddr_cmdline] p:%s, g_memdump_addr:0x%lx g_memdump_end:0x%lx,g_memdump_size:0x%x\n", (const char *)p, (unsigned long)g_memdump_addr, (unsigned long)g_memdump_end,g_memdump_size);
#ifndef CONFIG_ARCH_HI6XXX
    himntn_gobal_resetlog = 1;
#endif
    return 0;
}
early_param("memdump_addr", early_parse_memdumpaddr_cmdline);

static int __init memdump_init(void)
{
    struct memdump *mem_info;
    void __iomem *memdump_head;

    if (!himntn_gobal_resetlog){
        return 0;
    }
    if (g_memdump_addr == 0)
        return 0;

    /* to free the reserve mem of memdump*/ 
    balong_create_memory_proc_entry("dump_end", S_IRUGO, &dump_end_proc_fops, NULL);
    memdump_head = memdump_remap(g_memdump_addr, PAGE_SIZE);
    mem_info = (struct memdump *)memdump_head;

    while(mem_info->name[0] != 0) {
        pr_err("%s,name:%s\n", __func__, mem_info->name);          
        pr_err("%s:base:0x%8.8x, size:0x%8.8x\n", __func__, mem_info->base, mem_info->size);   
        create_dump_phy_mem_proc_file(mem_info->name, (mem_info->base),  mem_info->size);
        mem_info ++;
    }

    memdump_unmap(memdump_head);
    
    return 0;
}
arch_initcall(memdump_init);

#ifdef CONFIG_ARCH_HI6XXX
static ssize_t reboot_proc_read(struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{
    char buf[12];
    int strlen = 0;
    ssize_t copy;

    strlen = snprintf(buf, sizeof(buf), "0x%lx", (unsigned long)g_BitMapOfAbnRst);

    if (*off > strlen){
        printk(KERN_ERR"read offset error 3\n");
        return 0;
    }

    if (*off == strlen){
        /*end of file*/
        return 0;
    }

    copy = (ssize_t)min(bytes, (size_t)(strlen - *off));

    if (copy_to_user(userbuf, buf + *off, copy)){
        printk(KERN_ERR"%s copy to user error \n", __FUNCTION__);
        copy = -EFAULT;
        goto copy_err;
    }

    *off += copy;

copy_err:

    return copy;
}

static const struct file_operations reboot_proc_fops = {
    .read = reboot_proc_read,
};
static int __init create_reboot_proc(void)
{
    balong_create_reboot_proc_entry("reboot", S_IRUGO, &reboot_proc_fops, NULL);
    return 0;
}
arch_initcall(create_reboot_proc);

#endif

static int proc_balong_reset_show(struct seq_file *m, void *v)
{

    seq_printf(m, "%d", !himntn_gobal_resetlog);

    return 0;
}

static int proc_balong_reset_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_balong_reset_show, NULL);
}

static const struct file_operations proc_balong_reset_fops = {
    .open = proc_balong_reset_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init proc_balong_reset_init(void)
{
    balong_create_stats_proc_entry("close", S_IRUGO, &proc_balong_reset_fops, NULL);

    return 0;
}
module_init(proc_balong_reset_init);

/*Added by y65256 for wdg log save On 2013-6-8 End*/
MODULE_DESCRIPTION("Hisilicon Memory/Register Dump Module");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuyiping <xuyiping@huawei.com>");


