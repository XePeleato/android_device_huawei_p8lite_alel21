#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <soc_irqs.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/io.h>
#include <asm/io.h>
#include <soc_mddrc_dmc_interface.h>
#include <soc_mddrc_security_interface.h>
#include <linux/hisi/hi6xxx-platform.h>
#include <linux/hisi/reg_ops.h>
#include "soc_pmctrl_interface.h"

#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */

#include "../mntn/excDrv.h"
#include "soc_ao_sctrl_interface.h"
#include <soc_baseaddr_interface.h>
#include <soc_mddrc_dmc_interface.h> /* config ddr autorefresh in sram_reboot() */
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>

#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>

#ifdef CONFIG_DEBUG_FS
typedef union
{
    u32 ddrc_his_data[14];
    struct
    {
        u32 ddrc_his_flux_wr;
        u32 ddrc_his_flux_rd;
        u32 ddrc_his_flux_wcmd;
        u32 ddrc_his_flux_rcmd;
        u32 ddrc_his_fluxid_wr;
        u32 ddrc_his_fluxid_rd;
        u32 ddrc_his_fluxid_wcmd;
        u32 ddrc_his_fluxid_rcmd;
        u32 ddrc_his_wlatcnt0;
        u32 ddrc_his_wlatcnt1;
        u32 ddrc_his_rlatcnt0;
        u32 ddrc_his_rlatcnt1;
        u32 ddrc_his_inhere_rlat_cnt;
        u32 ddrc_his_cmd_sum;
    }ddrc_reg;
} DDRC_HIS_DATA_UNION;

struct ddrc_dmc_data {
    u64 ddrc_timer;
    DDRC_HIS_DATA_UNION u_ddrc_his_data;
};

unsigned int count;
struct ddrc_dmc_data *ddrc_datas;
struct dentry *ddrc_dmc_root;
unsigned int interval;
unsigned int ddrc_freq;
unsigned int sta_id;
unsigned int sta_id_mask;

static void __iomem *ddrc_dmc0_flux_base_virt = 0;

#define DDRC_FLUX_INT    (IRQ_GIC_START + 80)

#define MAX_DDRC_DMC_DATA                    0x80000 //512k
#define REG_BASE_DDRC_DMC0_FLUX_VIRT         ddrc_dmc0_flux_base_virt

#ifdef _DRV_LLT_
#define STATIC
#else
#define STATIC    static
#endif

void ddrc_flux_irq(int irq)
{
    int reg_val = 0;
    if (ddrc_datas != NULL) {
        ddrc_datas[count].ddrc_timer = sched_clock();
        memcpy(ddrc_datas[count].u_ddrc_his_data.ddrc_his_data,
                SOC_MDDRC_DMC_DDRC_HIS_FLUX_WR_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT),14*4);
    }
    if (count < (MAX_DDRC_DMC_DATA - 1))
        count++;
    reg_val = (irq & 0x00000002) ? 1<<1:1<<9;
    writel(reg_val, SOC_MDDRC_DMC_DDRC_RINT_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT));
    writel(1, SOC_MDDRC_DMC_DDRC_CTRL_PERF_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT));
}

irqreturn_t ddrc_dmc_handle(int irq, void *dev)
{
    int ddrc_int;

    ddrc_int = readl(SOC_MDDRC_DMC_DDRC_INTSTS_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT));
    if (ddrc_int & 0x00000202) {
        ddrc_flux_irq(ddrc_int);
    }

    return IRQ_HANDLED;
}

STATIC void *ddrc_data_seq_start(struct seq_file *m, loff_t *pos)
{
    if (*pos >= count)
        return NULL;
    return pos;
}

STATIC void *ddrc_data_seq_next(struct seq_file *m, void *v, loff_t *pos)
{
    (*pos)++;
    if (*pos >= count)
        return NULL;
    return pos;
}

STATIC void ddrc_data_seq_stop(struct seq_file *m, void *v)
{
}

STATIC int ddrc_data_seq_show(struct seq_file *m, void *v)
{
    int i;
    u64 sBeginTime;
    unsigned long nsBeginTime;
    struct tm tm_rtc;
    i = *(int *)v;

    if (i == 0) {
        seq_printf(m, "sta_id:%d,sta_id_mask:%d,ddrc_freq:%d,interval:%dms\n",
                sta_id, sta_id_mask, ddrc_freq, interval);
        seq_printf(m, "index,time,rtc_time,\
                      ddrc_his_flux_wr,ddrc_his_flux_rd,\
                      ddrc_his_flux_wcmd,ddrc_his_flux_wcmd,\
                      ddrc_his_fluxid_wr,ddrc_his_fluxid_rd,\
                      ddrc_his_fluxid_wcmd,ddrc_his_fluxid_rcmd,\
                      ddrc_his_wlatcnt0,ddrc_his_wlatcnt1,\
                      ddrc_his_rlatcnt0,ddrc_his_rlatcnt1,\
                      ddrc_his_inhere_rlat_cnt,ddrc_his_cmd_sum\n");
    }

    if (ddrc_datas != NULL) {
        sBeginTime = ddrc_datas[i].ddrc_timer;
        nsBeginTime = do_div(sBeginTime, 1000000000);
        time_to_tm((time_t)sBeginTime, 0, &tm_rtc);
        seq_printf(m, "%d,%05lu.%06lu,", i, (unsigned long)sBeginTime, nsBeginTime/1000);
        seq_printf(m, "[%lu:%.2d:%.2d %.2d:%.2d:%.2d],",
            1900 + tm_rtc.tm_year, tm_rtc.tm_mon + 1, tm_rtc.tm_mday,
            tm_rtc.tm_hour, tm_rtc.tm_min, tm_rtc.tm_sec);
        seq_printf(m, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_flux_wr,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_flux_rd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_flux_wcmd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_flux_rcmd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_fluxid_wr,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_fluxid_rd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_fluxid_wcmd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_fluxid_rcmd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_wlatcnt0,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_wlatcnt1,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_rlatcnt0,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_rlatcnt1,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_inhere_rlat_cnt,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_cmd_sum
        );
    }

    return 0;
}

static struct seq_operations ddrc_data_seq_ops = {
    .start = ddrc_data_seq_start,
    .next  = ddrc_data_seq_next,
    .stop  = ddrc_data_seq_stop,
    .show  = ddrc_data_seq_show
};

STATIC int ddrc_data_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &ddrc_data_seq_ops);
}

static const struct file_operations ddrc_data_fops = {
    .open = ddrc_data_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

ssize_t ddrc_init_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    u32 freq_interval;

    if (ddrc_datas == NULL) {
        ddrc_datas = (struct ddrc_dmc_data *)vmalloc(MAX_DDRC_DMC_DATA*sizeof(struct ddrc_dmc_data));
    }
    if (interval == 0) {
        interval = 10;
    }
    if (ddrc_freq == 0) {
        ddrc_freq = 50000;
    };
    freq_interval = ddrc_freq/16;	//实际统计周期为perf_prd*16*tclk（Tclk为DDRC总线时钟周期）
    freq_interval *= interval; 		//sft ddr 50M
    pr_info("%s:freq_interval:%d, interval: %dms, sta_id:%d sta_id_mask:%d\n", __func__,ddrc_freq, interval, sta_id, sta_id_mask);
    count = 0;
    //memset(ddrc_datas, 0, MAX_DDRC_DMC_DATA*sizeof(struct ddrc_dmc_data));
    writel(freq_interval | 0x10000000, SOC_MDDRC_DMC_DDRC_CFG_PERF_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT));

    writel(sta_id, SOC_MDDRC_DMC_DDRC_CFG_STAID_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT));
    writel(sta_id_mask, SOC_MDDRC_DMC_DDRC_CFG_STAIDMSK_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT));

    return (ssize_t)cnt;
}

static const struct file_operations ddrc_init_fops = {
    .write = ddrc_init_write,
};

ssize_t ddrc_exit_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    struct ddrc_dmc_data *tmp;
    tmp = ddrc_datas;
    if (ddrc_datas != NULL) {
        ddrc_datas = 0;
        vfree(tmp);
    }
    pr_info("%s\n", __func__);

    return (ssize_t)cnt;
}

static const struct file_operations ddrc_exit_fops = {
    .write = ddrc_exit_write,
};

/* 0: irq is not setup
   1: irq is enable
   2: irq is disable
   */
static unsigned int irq_setup_flag = 0;

ssize_t on_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{

    int ret = -1;

    SOC_MDDRC_DMC_DDRC_INTMSK_UNION *int_mask;


    pr_info("%s +.\n", __func__);

    /* setup ddrc interrupt for statistic function */
    if (irq_setup_flag == 0)
    {
        ret = request_irq(DDRC_FLUX_INT, ddrc_dmc_handle, IRQF_NO_SUSPEND, "ddrc_dmc", NULL);
	  if (ret) {
	      printk("%s: irq_request gic_1_irq %d failed!   \n","DDRC on write", DDRC_FLUX_INT);
            return -1;
        }
        irq_setup_flag = 1; /*irq is enabled*/
    }

    if (irq_setup_flag == 2) {
        enable_irq(DDRC_FLUX_INT);
        irq_setup_flag = 1;
    }

#ifdef _DRV_LLT_
#else
    int_mask = (SOC_MDDRC_DMC_DDRC_INTMSK_UNION *)SOC_MDDRC_DMC_DDRC_INTMSK_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT);
    int_mask->reg.flux_int_mask = 0;
    writel(1, SOC_MDDRC_DMC_DDRC_CTRL_PERF_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT));
#endif

    return (ssize_t)cnt;
}

static const struct file_operations on_fops = {
    .write = on_write,
};

ssize_t off_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{

    SOC_MDDRC_DMC_DDRC_INTMSK_UNION *int_mask;
    pr_info("%s +.\n", __func__);

    if (irq_setup_flag == 1) {
        disable_irq(DDRC_FLUX_INT);
        irq_setup_flag = 2;
    }

#ifdef _DRV_LLT_
#else
    int_mask = (SOC_MDDRC_DMC_DDRC_INTMSK_UNION *)SOC_MDDRC_DMC_DDRC_INTMSK_ADDR(REG_BASE_DDRC_DMC0_FLUX_VIRT);
    int_mask->reg.flux_int_mask = 1;
#endif

    return (ssize_t)cnt;
}

static const struct file_operations off_fops = {
    .write = off_write,
};

void ext_ddrc_dmc_on(void)
{
    on_write(NULL, NULL, 0, NULL);
}

void ext_ddrc_dmc_init(unsigned int ext_interval)
{
    interval = ext_interval;
    ddrc_init_write(NULL, NULL, 0, NULL);
}

void ext_ddrc_dmc_off(void)
{
    off_write(NULL, NULL, 0, NULL);
}

static struct of_device_id ddrc_flux_of_device_ids[] = {
    {.compatible = "hisilicon,ddrc-flux"},
    {},
};

static int ddrc_flux_device_probe(struct platform_device *pdev)
{
    pr_info("start %s, %d\n",__func__,__LINE__);
    ddrc_datas = NULL;
    ddrc_dmc_root = debugfs_create_dir("ddrc_dmc", NULL);

    debugfs_create_u32("interval", S_IRUGO | S_IWUSR, ddrc_dmc_root, &interval);
    debugfs_create_u32("ddrc_freq", S_IRUGO | S_IWUSR, ddrc_dmc_root, &ddrc_freq);
    debugfs_create_u32("sta_id", S_IRUGO | S_IWUSR, ddrc_dmc_root, &sta_id);
    debugfs_create_u32("sta_id_mask", S_IRUGO | S_IWUSR, ddrc_dmc_root, &sta_id_mask);
    debugfs_create_file("ddrc_init", S_IWUSR|S_IWGRP, ddrc_dmc_root, NULL, &ddrc_init_fops);
    debugfs_create_file("ddrc_exit", S_IWUSR|S_IWGRP, ddrc_dmc_root, NULL, &ddrc_exit_fops);
    debugfs_create_file("on", S_IWUSR, ddrc_dmc_root, NULL, &on_fops);
    debugfs_create_file("off", S_IWUSR, ddrc_dmc_root, NULL, &off_fops);
    debugfs_create_file("ddrc_data", S_IRUGO, ddrc_dmc_root, NULL, &ddrc_data_fops);

    ddrc_dmc0_flux_base_virt = ioremap(SOC_MDDRC_DMC_BASE_ADDR, PAGE_ALIGN(SZ_4K));
    if (0 == ddrc_dmc0_flux_base_virt) {
        pr_err("%s: ioremap failed\n",__func__);
        return -1;
    }
	
    pr_info("%s:exit! \n",__func__);
    return 0;
}

static struct platform_driver ddrc_flux_driver = {
    .driver		= {
		.name	= "ddrc-flux",
		.owner	= THIS_MODULE,
		.of_match_table = ddrc_flux_of_device_ids,
    },
    .probe		= ddrc_flux_device_probe,
};

STATIC int __init ddrc_flux_init(void)
{
    pr_info("init %s, %d\n",__func__,__LINE__);
    return(platform_driver_register(&ddrc_flux_driver));
}

STATIC void __exit ddrc_flux_exit(void)
{
    iounmap(REG_BASE_DDRC_DMC0_FLUX_VIRT);
    platform_driver_unregister(&ddrc_flux_driver);
}

#else
static int __init ddrc_flux_init(void)
{
    return 0;
}
static void __exit ddrc_flux_exit(void)
{

}
#endif
module_init(ddrc_flux_init);
module_exit(ddrc_flux_exit);

MODULE_DESCRIPTION("DDRC DMC Driver");
MODULE_LICENSE("GPL");

