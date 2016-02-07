/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : test_rtc.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2014年04月10日
  最近修改   :
  功能描述   : UT测试脚本
  函数列表   :
  修改历史   :
  1.日    期   : 2014年04月10日
    作    者   : 
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
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
#include <linux/sizes.h>

#include "soc_smart_interface.h"
#include "soc_baseaddr_interface.h"
#include "soc_irqs.h"
#include "bsp_ao_sctrl.h"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

#define	PMU_RTC_NAME			"HI6552-PMU-RTC"
#define	SOC_RTC_NAME			"HI6210-SOC-RTC"

/*
 * Externel RTC Register definitions 8bit(PMU)
 */
#define	PMU_RTCDR0			SOC_SMART_RTCDR0_ADDR(0)		/* RTC Data  register 0 bit[7:0] */
#define	PMU_RTCDR1			SOC_SMART_RTCDR1_ADDR(0)		/* RTC Data  register 1 bit[15:8] */
#define	PMU_RTCDR2			SOC_SMART_RTCDR2_ADDR(0)		/* RTC Data  register 2 bit[23:16] */
#define	PMU_RTCDR3			SOC_SMART_RTCDR3_ADDR(0)		/* RTC Data  register 3 bit[31:24] */

#define	PMU_RTCLR0			SOC_SMART_RTCLR0_ADDR(0)		/* RTC Data load register 0 bit[7:0]*/
#define	PMU_RTCLR1			SOC_SMART_RTCLR1_ADDR(0)		/* RTC Data load register 1 bi[15:8]*/
#define	PMU_RTCLR2			SOC_SMART_RTCLR2_ADDR(0)		/* RTC Data load register 2 bit[23:16]*/
#define	PMU_RTCLR3			SOC_SMART_RTCLR3_ADDR(0)		/* RTC Data load register 3 bit[31:24]*/

#define	PMU_RTCCTRL			SOC_SMART_RTCCTRL_ADDR(0) 		/* RTC Control register: 读RTC控制位,1 可读取实际当前值 */

#define	PMU_RTCMR0			SOC_SMART_RTCMR0_ADDR(0)		/* RTC Match register 0 bit[7:0]*/
#define	PMU_RTCMR1			SOC_SMART_RTCMR1_ADDR(0)		/* RTC Match register 1 bi[15:8]*/
#define	PMU_RTCMR2			SOC_SMART_RTCMR2_ADDR(0)		/* RTC Match register 2 bit[23:16]*/
#define	PMU_RTCMR3			SOC_SMART_RTCMR3_ADDR(0)		/* RTC Match register 3 bit[31:24]*/

#define	PMU_REG_IRQ3			SOC_SMART_IRQ3_ADDR(0)			/* W1C&RO */
#define	PMU_REG_IRQ3_IMSC		SOC_SMART_IRQ3_MASK_ADDR(0)		/* IRQ3 Interrupt mask and set register: 0-enable */
#define	PMU_RTC_BIT	    		(1 << 4)				/* Alarm interrupt bit */

/* 
  * Internal RTC Register 32bit(SOC)
  */
//#define	SOC_RTC0_BASE_ADDR	0xF8003000
//#define	SOC_RTC1_BASE_ADDR	0xF8004000
#define	SOC_RTCDR			0x000	/* Data  register */
#define	SOC_RTCMR			0x004	/* Match register */
#define	SOC_RTCLR			0x008	/* Data load register */
#define	SOC_RTCCR			0x00c	/* Control register  1:enable,0:disable*/
#define	SOC_RTCIMSC			0x010	/* Interrupt mask and set register  1:enable,0:disable*/
#define	SOC_RTCRIS			0x014	/* Raw interrupt status register  1:interrupted*/
#define	SOC_RTCMIS			0x018  	/* Masked interrupt status register  1:interrupted*/
#define	SOC_RTCICR			0x01C	/* Interrupt clear register  1:clear interrupt*/
#define	SOC_RTCALARM_INT		(1 << 0)	/* Alarm interrupt bit */
#define	SOC_RTC0CLK_BIT			(1 << 25) /* RTC0的始终通过AO_SCRTL寄存器SC_PERIPH_CLKEN4[25]和SC_PERIPH_CLKDIS4[25]来控制开关 */
//#define	SOC_RTC1CLK_BIT		(1 << 26)

struct soc_rtc_dev {
	struct rtc_device *rtc_dev;
	void __iomem      *baseaddr;
};

struct soc_rtc_dev prtc = {
	.baseaddr = (void __iomem *)0x1234,
};

static int pmu_rtc_irq = -1;
extern unsigned int get_pd_charge_flag(void);
//extern struct rtc_wkalrm poweroff_rtc_alarm;
struct rtc_wkalrm poweroff_rtc_alarm = {0,0,{0}};


/*****************************************************************************
  3 函数声明
*****************************************************************************/
extern void pmu_get_time(unsigned long  *time);
extern void pmu_set_time(unsigned long time);
extern void pmu_get_alarm_time(unsigned long  *time);
extern void pmu_set_alarm_time(unsigned long time);
extern void pmu_read_alarm(struct rtc_time *tm);
extern int pmu_write_alarm(struct rtc_time *tm);
extern int soc_rtc_read_time(struct device *dev, struct rtc_time *tm);
extern int soc_rtc_set_time(struct device *dev, struct rtc_time *tm);
extern int soc_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled);
extern int soc_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm);
extern int soc_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm); 
extern int oem_rtc_reboot_thread(void *u);
extern void oem_rtc_reboot(unsigned long t);
extern irqreturn_t pmu_rtc_interrupt(int irq, void *dev_id);
extern irqreturn_t soc_rtc_interrupt(int irq, void *dev_id);
extern void hi6210_rtc_shutdown(struct platform_device *pdev);
extern int hi6210_rtc_remove(struct platform_device *pdev);
extern int hi6210_rtc_probe(struct platform_device *pdev);


int ut_rtc_valid_tm_zero()
{
	return 0;
}

int ut_rtc_valid_tm_one()
{
	return -1;
}

void *rtc_dev_get_drvdata(const struct device *dev)
{
	return &prtc;
}

inline void ut_tasklet_schedule(struct tasklet_struct *tasklet)
{
	return ;
}


/*****************************************************************************
 被测函数  : pmu_get_time
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_pmu_get_time(void)
{
	unsigned long time = 0x12341234;
    pmu_get_time(&time);
	return 0;
}


/*****************************************************************************
 被测函数  : pmu_set_time
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_pmu_set_time(void)
{
	unsigned long time = 0x12341234;
    pmu_set_time(time);
	return 0;
}


/*****************************************************************************
 被测函数  : pmu_get_alarm_time
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_pmu_get_alarm_time(void)
{
	unsigned long time = 0x12341234;
	pmu_get_alarm_time(&time);
	return 0;
}


/*****************************************************************************
 被测函数  : pmu_set_alarm_time
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_pmu_set_alarm_time(void)
{
	unsigned long time = 0x12341234;
    pmu_set_alarm_time(time);
	return 0;
}


/*****************************************************************************
 被测函数  : pmu_read_alarm
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_pmu_read_alarm(void)
{
	struct rtc_time tm = {
		.tm_sec = 11,
		.tm_min = 22,
		.tm_hour = 3,
		.tm_mday = 1,
		.tm_mon = 2,
		.tm_year = 22,
		.tm_wday = 1,
		.tm_yday = 2,
		.tm_isdst = 1
	};
    pmu_read_alarm(&tm);
}


/*****************************************************************************
 被测函数  : pmu_write_alarm
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_pmu_write_alarm(void)
{
	int ret = 0;
	struct rtc_time tm = {
		.tm_sec = 11,
		.tm_min = 22,
		.tm_hour = 3,
		.tm_mday = 1,
		.tm_mon = 2,
		.tm_year = 200,
		.tm_wday = 1,
		.tm_yday = 2,
		.tm_isdst = 1
	};
    ret = pmu_write_alarm(&tm);
	return ret;
}


/*****************************************************************************
 被测函数  : soc_rtc_read_time
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_soc_rtc_read_time(void)
{
    int ret = 0;
	struct device *dev = NULL;
	struct rtc_time tm = {
		.tm_sec = 11,
		.tm_min = 22,
		.tm_hour = 3,
		.tm_mday = 1,
		.tm_mon = 2,
		.tm_year = 22,
		.tm_wday = 1,
		.tm_yday = 2,
		.tm_isdst = 1
	};
    ret = soc_rtc_read_time(dev,&tm);

	return ret;
}

/*****************************************************************************
 被测函数  : soc_rtc_set_time
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_soc_rtc_set_time(void)
{
    int ret = 0;
	struct device *dev = NULL;
	struct rtc_time tm = {
		.tm_sec = 11,
		.tm_min = 22,
		.tm_hour = 3,
		.tm_mday = 1,
		.tm_mon = 2,
		.tm_year = 22,
		.tm_wday = 1,
		.tm_yday = 2,
		.tm_isdst = 1
	};
    ret = soc_rtc_set_time(dev,&tm);
	return ret;
}


/*****************************************************************************
 被测函数  : soc_rtc_alarm_irq_enable
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_soc_rtc_alarm_irq_enable(unsigned int enabled)
{
    int ret = 0;
    struct device *dev;

    ret = soc_rtc_alarm_irq_enable(dev,enabled);
	return ret;
}


/*****************************************************************************
 被测函数  : soc_rtc_read_alarm
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_soc_rtc_read_alarm(void)
{
    int ret = 0;
    struct device *dev;
	struct rtc_wkalrm alarm = {0,0,{0}};
	
	ret = soc_rtc_read_alarm(dev,&alarm);
	
	return ret;
}


/*****************************************************************************
 被测函数  : soc_rtc_set_alarm
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_soc_rtc_set_alarm(void)
{
    int ret = 0;
    struct device *dev;
    struct rtc_wkalrm alarm = {0,0,{0}};

    ret = soc_rtc_set_alarm(dev,&alarm);
	
	return ret;
}


/*****************************************************************************
 被测函数  : oem_rtc_reboot_thread
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_oem_rtc_reboot_thread(void)
{
    int ret = 0;
    ret = oem_rtc_reboot_thread(NULL);
	
	return ret;
}


/*****************************************************************************
 被测函数  : oem_rtc_reboot
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_oem_rtc_reboot(void)
{
	unsigned int t = 12;
    oem_rtc_reboot(t);
	return 0;
}


/*****************************************************************************
 被测函数  : pmu_rtc_interrupt
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_pmu_rtc_interrupt(void)
{
    int ret = 0;
    int irq;

    ret = pmu_rtc_interrupt(irq,NULL);
	
	return ret;
}

/*****************************************************************************
 被测函数  : soc_rtc_interrupt
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_soc_rtc_interrupt(void)
{
    int ret = 0;
    int irq;

    ret = soc_rtc_interrupt(irq,&prtc);
	return ret;
}

/*****************************************************************************
 被测函数  : hi6210_rtc_shutdown
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_hi6210_rtc_shutdown()
{
	struct platform_device *ut_pdev;
	hi6210_rtc_shutdown(ut_pdev);
	return 0;
}

/*****************************************************************************
 被测函数  : hi6210_rtc_remove
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_hi6210_rtc_remove()
{
	int ret = 0;
	struct platform_device *ut_pdev;
	ret = hi6210_rtc_remove(ut_pdev);
	return ret;
}

/*****************************************************************************
 被测函数  : hi6210_rtc_probe
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_hi6210_rtc_probe_nodev(struct platform_device *pdev)
{
	int ret = 0;
	struct platform_device ut_pdev;
	ut_pdev.dev.of_node = NULL;
	ret = hi6210_rtc_probe(&ut_pdev);
	return ret;
}

/*****************************************************************************
 被测函数  : hi6210_rtc_probe
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年4月10日
    作    者   : 乔志华 WX205565
    修改内容   : 新生成UT用例

*****************************************************************************/

int test_hi6210_rtc_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct platform_device ut_pdev;
	struct device_node ut_node;
	ut_pdev.dev.of_node = &ut_node;
	ret = hi6210_rtc_probe(&ut_pdev);
	if(ret > 0) {
		ret = 1;
	}
	return ret;
}
