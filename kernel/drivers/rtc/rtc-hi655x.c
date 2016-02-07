#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/reboot.h>
#include <linux/syscalls.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/mfd/hisi_hi6xxx_pmic.h>
#include <linux/of.h>
#include <linux/rtc.h>
#include <linux/sizes.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <linux/hisi/hi6xxx-platform.h>
#include <linux/hisi/hw_cmdline_parse.h>

#include "soc_smart_interface.h"
#include "soc_baseaddr_interface.h"
#include "soc_ao_sctrl_interface.h"
#include "soc_irqs.h"
#include "general_sram_map.h"

#define	PMU_RTC_NAME			"HI6552-PMU-RTC"
#define	SOC_RTC_NAME			"HI6210-SOC-RTC"

/*
 * Externel RTC Register definitions 8bit(PMU)
 */
#define	PMU_RTCDR0			SOC_SMART_RTCDR0_ADDR(0)		/* RTC Data  register 0 bit[7:0] */
#define	PMU_RTCDR1			SOC_SMART_RTCDR1_ADDR(0)		/* RTC Data  register 1 bit[15:8] */
#define	PMU_RTCDR2			SOC_SMART_RTCDR2_ADDR(0)		/* RTC Data  register 2 bit[23:16] */
#define	PMU_RTCDR3			SOC_SMART_RTCDR3_ADDR(0)		/* RTC Data  register 3 bit[31:24] */

#define	PMU_RTCLR0				SOC_SMART_RTCLR0_ADDR(0)		/* RTC Data load register 0 bit[7:0]*/
#define	PMU_RTCLR1				SOC_SMART_RTCLR1_ADDR(0)		/* RTC Data load register 1 bi[15:8]*/
#define	PMU_RTCLR2				SOC_SMART_RTCLR2_ADDR(0)		/* RTC Data load register 2 bit[23:16]*/
#define	PMU_RTCLR3				SOC_SMART_RTCLR3_ADDR(0)		/* RTC Data load register 3 bit[31:24]*/

#define	PMU_RTCCTRL			SOC_SMART_RTCCTRL_ADDR(0) 	/* RTC Control register: 读RTC控制位,1 可读取实际当前值 */

#define	PMU_RTCMR0			SOC_SMART_RTCMR0_ADDR(0)		/* RTC Match register 0 bit[7:0]*/
#define	PMU_RTCMR1			SOC_SMART_RTCMR1_ADDR(0)		/* RTC Match register 1 bi[15:8]*/
#define	PMU_RTCMR2			SOC_SMART_RTCMR2_ADDR(0)		/* RTC Match register 2 bit[23:16]*/
#define	PMU_RTCMR3			SOC_SMART_RTCMR3_ADDR(0)		/* RTC Match register 3 bit[31:24]*/

#define	PMU_REG_IRQ3			SOC_SMART_IRQ3_ADDR(0)		/* W1C&RO */
#define	PMU_REG_IRQ3_IMSC		SOC_SMART_IRQ3_MASK_ADDR(0)	/* IRQ3 Interrupt mask and set register: 0-enable */
#define	PMU_RTC_BIT	    		(1 << 4)							/* Alarm interrupt bit */

/*
  * Internal RTC Register 32bit(SOC)
  */

#define	SOC_RTCDR				0x000	/* Data  register */
#define	SOC_RTCMR				0x004	/* Match register */
#define	SOC_RTCLR				0x008	/* Data load register */
#define	SOC_RTCCR				0x00c	/* Control register  1:enable,0:disable*/
#define	SOC_RTCIMSC			        0x010	/* Interrupt mask and set register  1:enable,0:disable*/
#define	SOC_RTCRIS				0x014	/* Raw interrupt status register  1:interrupted*/
#define	SOC_RTCMIS				0x018  	/* Masked interrupt status register  1:interrupted*/
#define	SOC_RTCICR				0x01C	/* Interrupt clear register  1:clear interrupt*/
#define	SOC_RTCALARM_INT		       (1 << 0)	/* Alarm interrupt bit */
#define	SOC_RTC0CLK_BIT		               (1 << 25) /* RTC0的始终通过AO_SCRTL寄存器SC_PERIPH_CLKEN4[25]和SC_PERIPH_CLKDIS4[25]来控制开关 */

#ifdef _DRV_LLT_
#define static
#endif

struct soc_rtc_dev {
	struct rtc_device *rtc_dev;
	void __iomem      *baseaddr;
};

void __iomem *soc_rtc_baseaddr = NULL;
static int pmu_rtc_irq = -1;
extern struct rtc_wkalrm poweroff_rtc_alarm;
static struct timer_list rtc_timer;

#ifndef _DRV_LLT_UT_

unsigned int get_pd_charge_flag(void);
static unsigned int pd_charge_flag = 0;

/**
 * parse powerdown charge cmdline which is passed from fastoot. *
 * Format : pd_charge=0 or 1             *
 */

static int __init early_parse_pdcharge_cmdline(char * p)
{
    if(p)
    {
        if(!strcmp(p,"charger"))
        {
            pd_charge_flag = 1;
        }
        else
        {
            pd_charge_flag = 0;
        }

        printk("power down charge p:%s, pd_charge_flag :%u\n", p, pd_charge_flag);
    }

    return 0;
}

early_param("androidboot.mode", early_parse_pdcharge_cmdline);
unsigned int get_pd_charge_flag(void)
{
	return (pd_charge_flag);
}
EXPORT_SYMBOL(get_pd_charge_flag);

#endif




/* Read external rtc time */
static void pmu_get_time(unsigned long  *time)
{
	*time = (hi6xxx_pmic_reg_read(PMU_RTCDR3) << 24) | (hi6xxx_pmic_reg_read(PMU_RTCDR2) << 16) |
			(hi6xxx_pmic_reg_read(PMU_RTCDR1) << 8) | hi6xxx_pmic_reg_read(PMU_RTCDR0);
}

/* Set external rtc time*/
static void pmu_set_time(unsigned long time)
{
	unsigned char temp_val;

	/*write operation must start form low bit*/
	temp_val = time & 0xFF;
	hi6xxx_pmic_reg_write(PMU_RTCLR0, temp_val);
	temp_val =  (time >> 8) & 0xFF;
	hi6xxx_pmic_reg_write(PMU_RTCLR1, temp_val);
	temp_val =  (time >> 16) & 0xFF;
	hi6xxx_pmic_reg_write(PMU_RTCLR2, temp_val);
	temp_val =  (time >> 24) & 0xFF;
	hi6xxx_pmic_reg_write(PMU_RTCLR3, temp_val);
}

static void pmu_get_alarm_time(unsigned long  *time)
{
	*time =  (hi6xxx_pmic_reg_read(PMU_RTCMR3) << 24) | \
			(hi6xxx_pmic_reg_read(PMU_RTCMR2) << 16) | \
			(hi6xxx_pmic_reg_read(PMU_RTCMR1) << 8) | \
			 hi6xxx_pmic_reg_read(PMU_RTCMR0);
}

static void pmu_set_alarm_time(unsigned long time)
{
	unsigned char temp_val;

	/*write operation must start form low bit*/
	temp_val = time & 0xFF;
	hi6xxx_pmic_reg_write(PMU_RTCMR0, temp_val);
	temp_val =  (time >> 8) & 0xFF;
	hi6xxx_pmic_reg_write(PMU_RTCMR1, temp_val);
	temp_val =  (time >> 16) & 0xFF;
	hi6xxx_pmic_reg_write(PMU_RTCMR2, temp_val);
	temp_val =  (time >> 24) & 0xFF;
	hi6xxx_pmic_reg_write(PMU_RTCMR3, temp_val);

	temp_val = hi6xxx_pmic_reg_read(PMU_REG_IRQ3_IMSC);
	temp_val &= ~PMU_RTC_BIT;
	hi6xxx_pmic_reg_write(PMU_REG_IRQ3_IMSC, temp_val);
}

void hisi_pmu_rtc_readtime(struct rtc_time *tm)
{
        unsigned long time;

	pmu_get_time(&time);
	rtc_time_to_tm(time, tm);
}
void hisi_pmu_rtc_setalarmtime(unsigned long time)
{
      pmu_set_alarm_time(time);
}
/* Read external rtc alarm time*/
static void pmu_read_alarm(struct rtc_time *tm)
{
	unsigned long time;

	pmu_get_alarm_time(&time);
	rtc_time_to_tm(time, tm);
}

/* Set external rtc alarm time*/
static int pmu_write_alarm(struct rtc_time *tm)
{
	unsigned long time;
	int err;

	err = rtc_valid_tm(tm);
	if (err != 0) {
		return err;
	}
	rtc_tm_to_time(tm, &time);
	pmu_set_alarm_time(time);

        return 0;
}

/* Read internal rtc time*/
static int soc_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	unsigned long time;

        /* use pmu rtc time */
	pmu_get_time(&time);
	rtc_time_to_tm(time, tm);

	return 0;
}

void rtc_timer_fun(void)
{
	unsigned long pmu_time = 0;
	printk(KERN_INFO "==Sync PMU time to SOC Again: ChipBug==\n");
	pmu_get_time(&pmu_time);
	writel(pmu_time, soc_rtc_baseaddr + SOC_RTCLR);
}

/* Set internal rtc time*/
static int soc_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	int err;
	unsigned long time;
	struct soc_rtc_dev *prtc = dev_get_drvdata(dev);

	err = rtc_valid_tm(tm);
	if (err != 0) {
		printk(KERN_ERR "soc_rtc_set_time Time is invalid \n");
		return err;
	}
	rtc_tm_to_time(tm, &time);

	writel(time, prtc->baseaddr + SOC_RTCLR);
    /* set pmu rtc */
	pmu_set_time(time);

	printk(KERN_INFO  "soc_rtc_set_time, time: %d-%d-%d %d:%d:%d\n",
	       tm->tm_year+1900, tm->tm_mon+1,
	       tm->tm_mday, tm->tm_hour,
	       tm->tm_min, tm->tm_sec);

	return 0;
}

static int soc_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	unsigned int imsc;
	struct soc_rtc_dev *prtc = dev_get_drvdata(dev);

	/* Clear any pending alarm interrupts. */
	writel(SOC_RTCALARM_INT, prtc->baseaddr + SOC_RTCICR);

	imsc = readl(prtc->baseaddr + SOC_RTCIMSC);
	if (enabled == 1)
		writel(imsc | SOC_RTCALARM_INT, prtc->baseaddr + SOC_RTCIMSC);
	else
		writel(imsc & ~SOC_RTCALARM_INT, prtc->baseaddr + SOC_RTCIMSC);

	return 0;
}

/* Read internal rtc alarm time*/
static int soc_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct soc_rtc_dev *prtc = dev_get_drvdata(dev);
	unsigned long time;

	time = readl(prtc->baseaddr + SOC_RTCMR);

	rtc_time_to_tm(time, &alarm->time);

	alarm->pending = readl(prtc->baseaddr + SOC_RTCRIS) & SOC_RTCALARM_INT;
	alarm->enabled = readl(prtc->baseaddr + SOC_RTCIMSC) & SOC_RTCALARM_INT;

	return 0;
}

/* Set internal rtc alarm time*/
static int soc_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct soc_rtc_dev *prtc = dev_get_drvdata(dev);
	unsigned long time;
	int ret;

	printk(KERN_INFO "soc_rtc_set_alarm time %d-%d-%d %d:%d:%d\n",
	       alarm->time.tm_year+1900, alarm->time.tm_mon+1,
	       alarm->time.tm_mday, alarm->time.tm_hour,
	       alarm->time.tm_min, alarm->time.tm_sec);

	ret = rtc_valid_tm(&alarm->time);
	if (ret == 0) {
		ret = rtc_tm_to_time(&alarm->time, &time);
		if (ret == 0) {
			printk(KERN_INFO "[%s:%d] Time[0x%x] will writed to RTCMR\n",
			       __FUNCTION__, __LINE__,time);
			writel(time, prtc->baseaddr + SOC_RTCMR);
			soc_rtc_alarm_irq_enable(dev, alarm->enabled);
		}
	}
	else{
		printk(KERN_ERR "soc_rtc_set_alarm Alarm time is invalid \n");
	}
	printk(KERN_INFO "[%s:%d] RTC Register dump: RTCMR=[0x%x], RTCDR=[0x%x]\n",
	       __FUNCTION__, __LINE__, readl(prtc->baseaddr + SOC_RTCMR),
	       readl(prtc->baseaddr + SOC_RTCDR));
	return ret;
}

static struct rtc_class_ops hi6210_rtc_ops = {
	.read_time = soc_rtc_read_time,
	.set_time = soc_rtc_set_time,
	.read_alarm = soc_rtc_read_alarm,
	.set_alarm = soc_rtc_set_alarm,
	.alarm_irq_enable = soc_rtc_alarm_irq_enable,
};

static int oem_rtc_reboot_thread(void *u)
{
	printk(KERN_INFO "Entering Rebooting Causeed by Alarm...\n");
	emergency_remount();
	sys_sync();
	kernel_restart("oem_rtc");

	/* should not be here */
	panic("oem_rtc");
	return 0;
}

static void oem_rtc_reboot(unsigned long t)
{
	kernel_thread(oem_rtc_reboot_thread, NULL, CLONE_FS | CLONE_FILES);
}

static DECLARE_TASKLET(oem_rtc_reboot_tasklet, oem_rtc_reboot, 0);

static irqreturn_t pmu_rtc_interrupt(int irq, void *dev_id)
{

	/* pmic(hi6xxx_pmic.c) clear all sub_interrupts */

	if(unlikely(get_pd_charge_flag())) {
		tasklet_schedule(&oem_rtc_reboot_tasklet);
        }

	return IRQ_HANDLED;
}

static irqreturn_t soc_rtc_interrupt(int irq, void *dev_id)
{
	struct soc_rtc_dev *prtc = dev_id;
	unsigned int rtcmis;
	unsigned long events = 0;

    printk(KERN_INFO "[%s:%d] RTC Register dump: RTCMR=[0x%x], RTCDR=[0x%x]\n",
	       __FUNCTION__, __LINE__, readl(prtc->baseaddr + SOC_RTCMR),
	       readl(prtc->baseaddr + SOC_RTCDR));

	rtcmis = readl(prtc->baseaddr + SOC_RTCMIS); /*read interrupt*/
	if (rtcmis) {
		writel(rtcmis, prtc->baseaddr + SOC_RTCICR); /*clear interrupt*/
		printk(KERN_INFO "[%s:%d] RTC TIMER IRQ status [0x%x]\n", rtcmis);
		if (rtcmis & SOC_RTCALARM_INT)
			events |= (RTC_AF | RTC_IRQF);
		rtc_update_irq(prtc->rtc_dev, 1, events);

		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static void hi6210_rtc_shutdown(struct platform_device *pdev)
{
	if(unlikely(get_pd_charge_flag()))
		return;

	if (poweroff_rtc_alarm.enabled) {
		pmu_write_alarm(&poweroff_rtc_alarm.time);
	} else {
		pmu_set_alarm_time(0);
	}
}


static int hi6210_rtc_remove(struct platform_device *pdev)
{
	struct soc_rtc_dev *prtc = dev_get_drvdata(&pdev->dev);

	free_irq(pmu_rtc_irq, prtc->rtc_dev);
	free_irq(IRQ_RTC, prtc->rtc_dev);

	platform_set_drvdata(pdev, NULL);

	rtc_device_unregister(prtc->rtc_dev);
	iounmap(prtc->baseaddr);
	kfree(prtc);

	return 0;
}

unsigned int get_rtc_timer_value(void)
{
	unsigned int result = 0;

	if(soc_rtc_baseaddr != NULL)
		result = readl(soc_rtc_baseaddr+SOC_RTCDR);
	return result;
}

static int hi6210_rtc_probe(struct platform_device *pdev)
{
	int ret;
	struct soc_rtc_dev *prtc = NULL;
	unsigned long pmu_time = 0;
	unsigned long soc_time = 0;
	unsigned long alarm_time = 0;
	struct rtc_time pmu_time_tm;
	struct rtc_time soc_time_tm;
	struct rtc_time alarm_time_tm;
	struct device_node *np = NULL;
	int board_id = -1;
	unsigned long val = 0;
	void __iomem      *regaddr, *pmu_clk_addr;
	unsigned long pmu_clk_frequency = 0;

	void __iomem *base = (void __iomem *)HISI_VA_ADDRESS(REG_BASE_SC_ON);

	printk(KERN_INFO "hi6210 RTC init----------------\n");
#if 0
	if (NULL == dev_get_drvdata(pdev->dev.parent)) {
        	printk(KERN_ERR "failed to get parent resource!\n");
		ret = -ENODEV;
	 	goto out;
	 }
#endif

	np = of_find_compatible_node(NULL, NULL, "hisilicon,hi6552-pmu-rtc");
        if (np) {
        	if (of_property_read_u32(np, "board_id",&board_id)){
            		printk(KERN_ERR "RTC Failed to get boardtype!!!!\n");
	     		ret = -ENOMEM;
	    		goto out;
	 	}
      	}

	if (pdev->dev.of_node == NULL) {
		printk(KERN_ERR "Get PMU RTC of_node error\n");
		ret = -ENODEV;
		goto out;
	}

	prtc = devm_kzalloc(&pdev->dev, sizeof(struct soc_rtc_dev), GFP_KERNEL);
	if (!prtc) {
		printk(KERN_ERR "devm_kzalloc error\n");
		ret = -ENOMEM;
		goto out;
	}
	platform_set_drvdata(pdev, prtc);

	pmu_rtc_irq = platform_get_irq_byname(pdev, "hi6552_pmu_rtc");
	if (pmu_rtc_irq < 0) {
		printk(KERN_ERR "platform_get_irq_byname error\n");
		ret = -ENOENT;
		goto out_no_rtc;
	}

	prtc->baseaddr = ioremap(SOC_RTC0_BASE_ADDR, SZ_4K);
	printk(KERN_INFO "SOC_RTC0_BASE_ADDR=%#x, Virt_addr=0x%p\n", SOC_RTC0_BASE_ADDR, prtc->baseaddr);
	if (prtc->baseaddr == NULL) {
		printk(KERN_ERR "ioremap error\n");
		ret = -EIO;
		goto out_no_rtc;
	}

	soc_rtc_baseaddr = prtc->baseaddr;
	device_init_wakeup(&pdev->dev, 1);

	prtc->rtc_dev= rtc_device_register(SOC_RTC_NAME, &pdev->dev, &hi6210_rtc_ops, THIS_MODULE);
	if (IS_ERR(prtc->rtc_dev)) {
		printk(KERN_ERR "rtc_device_register error\n");
		ret = PTR_ERR(prtc->rtc_dev);
		goto out_no_rtc;
	}

	if (request_irq(IRQ_RTC, soc_rtc_interrupt, IRQF_DISABLED, SOC_RTC_NAME, prtc)) {
		printk(KERN_ERR "request_irq error\n");
		ret = -EIO;
		goto out_no_irq;
	}

	if (request_irq(pmu_rtc_irq, pmu_rtc_interrupt, IRQF_DISABLED, PMU_RTC_NAME, prtc)) {
		printk(KERN_ERR "request_irq error\n");
		ret = -EIO;
		free_irq(IRQ_RTC, prtc->rtc_dev);
		goto out_no_irq;
	}

	/* enable pmu rtc, 上电重启默认打开,可写0手动关闭 */
	hi6xxx_pmic_reg_write(PMU_RTCCTRL, 0x01);

	/* get PMU RTC time */
	pmu_get_time(&pmu_time);
	rtc_time_to_tm(pmu_time, &pmu_time_tm);
	printk(KERN_INFO "RTC time %d-%d-%d %d:%d:%d\n",
	       pmu_time_tm.tm_year, pmu_time_tm.tm_mon,
	       pmu_time_tm.tm_mday, pmu_time_tm.tm_hour,
	       pmu_time_tm.tm_min, pmu_time_tm.tm_sec);

	/* get PMU RTC alarm */
	pmu_read_alarm(&alarm_time_tm);
	printk(KERN_INFO "Last alarm %d-%d-%d %d:%d:%d\n",
	       alarm_time_tm.tm_year, alarm_time_tm.tm_mon,
	       alarm_time_tm.tm_mday, alarm_time_tm.tm_hour,
	       alarm_time_tm.tm_min, alarm_time_tm.tm_sec);

    /*PMU时钟源选择: SFT 默认32764， ASIC--read from sram, which writed by pmu in fastboot*/
    /* sram -- clk fre, equals to SOC Register val + 0x1 */
    pmu_clk_addr = ioremap(MEMORY_AXI_32K_CLK_TYPE_ADDR, SZ_4K);
    pmu_clk_frequency = readl(pmu_clk_addr);
    if(0 == board_id)
	{
		val = ((0x1<<15) | (pmu_clk_frequency - 0x1));
	}
	else
	{
		val = ((0x0<<15) | (pmu_clk_frequency - 0x1));
	}
	regaddr = ioremap(SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_ADDR(REG_BASE_SC_ON), SZ_4K);
	writel(val, regaddr);

    /* 打开1Hz的时钟门控，即将0bit配1，reg属性:写0无效 */
	writel(SOC_RTC0CLK_BIT, SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(base));

	/* enable soc rtc */
	writel(1, prtc->baseaddr + SOC_RTCCR);
	/* sync pmu rtc to soc rtc */
	writel(pmu_time, prtc->baseaddr + SOC_RTCLR);

	/* get soc rtc initial value */
	soc_time = readl(prtc->baseaddr + SOC_RTCDR);
	rtc_time_to_tm(soc_time, &soc_time_tm);
	printk(KERN_INFO "Init AP RTC %d-%d-%d %d:%d:%d\n",
			soc_time_tm.tm_year, soc_time_tm.tm_mon,
			soc_time_tm.tm_mday, soc_time_tm.tm_hour,
			soc_time_tm.tm_min, soc_time_tm.tm_sec);

	rtc_tm_to_time(&alarm_time_tm, &alarm_time);
	if (unlikely(get_pd_charge_flag() && (alarm_time > pmu_time))) {
		poweroff_rtc_alarm.enabled = 1;
		memcpy(&poweroff_rtc_alarm.time, &alarm_time_tm, sizeof(struct rtc_time));
		printk(KERN_INFO "set to poweroff alarm %d-%d-%d %d:%d:%d\n",
			poweroff_rtc_alarm.time.tm_year,
			poweroff_rtc_alarm.time.tm_mon,
			poweroff_rtc_alarm.time.tm_mday,
			poweroff_rtc_alarm.time.tm_hour,
			poweroff_rtc_alarm.time.tm_min,
			poweroff_rtc_alarm.time.tm_sec);
	}

	init_timer(&rtc_timer);
	rtc_timer.expires =jiffies + HZ;
	rtc_timer.function = &rtc_timer_fun;
	add_timer(&rtc_timer);
	printk(KERN_INFO "hi6210 RTC init end----------------\n");
	iounmap(regaddr);
	iounmap(pmu_clk_addr);

	return 0;

out_no_irq:
	iounmap(prtc->baseaddr);
	rtc_device_unregister(prtc->rtc_dev);

out_no_rtc:
	kfree(prtc);
	platform_set_drvdata(pdev, NULL);

out:
	return ret;
}

#ifdef CONFIG_PM_SLEEP
static int hi6210_rtc_suspend(struct device *dev)
{
    int ret = 0;

	if(device_may_wakeup(dev)){
   		ret = enable_irq_wake(IRQ_RTC);
	}

	return ret;
}

static int hi6210_rtc_resume(struct device *dev)
{
    int ret = 0;
    unsigned long pmu_time =0;

	if(device_may_wakeup(dev)){
   		ret = disable_irq_wake(IRQ_RTC);
	}
	pmu_get_time(&pmu_time);
	writel(pmu_time, soc_rtc_baseaddr + SOC_RTCLR);

	return ret;
}
#endif
static SIMPLE_DEV_PM_OPS(hi6210_rtc_pm_ops, hi6210_rtc_suspend, hi6210_rtc_resume);

static struct of_device_id hi6552_rtc_of_match[] = {
	{ .compatible = "hisilicon,hi6552-pmu-rtc" },
	{ }
};
#ifdef _DRV_LLT_
#else
MODULE_DEVICE_TABLE(of, hi6552_rtc_of_match);
#endif
static struct platform_driver hi6210_rtc_driver = {
	.probe  = hi6210_rtc_probe,
	.remove = hi6210_rtc_remove,
	.shutdown  = hi6210_rtc_shutdown,
	.driver	= {
		.name =	SOC_RTC_NAME,
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hi6552_rtc_of_match),
		.pm	= &hi6210_rtc_pm_ops,
	},
};

static int __init hi6210_init(void)
{
	return platform_driver_register(&hi6210_rtc_driver);
}

static void __exit hi6210_exit(void)
{
	platform_driver_unregister(&hi6210_rtc_driver);
}

module_init(hi6210_init);
module_exit(hi6210_exit);

MODULE_AUTHOR("Hisilicon Co. Ltd");
MODULE_DESCRIPTION("hi6210 RTC Driver");
MODULE_LICENSE("GPL");

