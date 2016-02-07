/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : bsp_timer.c
  版 本 号   : 初稿
  作    者   : 王振伟 176398
  生成日期   : 2012年9月20日
  最近修改   :
  功能描述   : timer驱动
  函数列表   :
  修改历史   :
  1.日    期   : 2012年9月20日
    作    者   : 王振伟 176398
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/module.h>
#include <linux/spinlock.h>
#include <asm/io.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */

#include <linux/hisi/hi6xxx-platform.h>
#include "bsp_timer_phy_om.h"
#include "soc_timer_interface.h"
#include "bsp_timer_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* base address(virtual addr) of timer1 registers */
unsigned long timer1_base = 0;

/* om timer是否初始化flag */
TIMER_INIT_FLAG g_om_timer_inited = TIMER_UNINITED;

spinlock_t g_sysctrl_lock = SPIN_LOCK_UNLOCKED;
/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : init_om_hardware_timer
 功能描述  : 初始化OM使用的timer
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月7日
    作    者   : 王振伟 176398
    修改内容   : 新生成函数

*****************************************************************************/
int __init init_om_hardware_timer(void)
{
    volatile unsigned int tmp_val = 0;
    unsigned long timer1_addr = 0;
    unsigned long sctrl_addr = HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);

    timer1_base = (unsigned long)ioremap(OSA_TIMER_REG_BASE, OSA_TIMER_REG_SIZE);
    if (!timer1_base)
    {
        printk(KERN_ERR "timer1 reg ioremap error.\n");
        return -1;
    }

    timer1_addr = timer1_base;

    /*对系统控制器的操作，使用spin_lock锁住*/
    spin_lock(&g_sysctrl_lock);

    /*选择32K时钟*/
    tmp_val = readl(SOC_AO_SCTRL_SC_TIMER_EN0_ADDR(sctrl_addr));
    tmp_val = (tmp_val & SC_TIMER_EN0_MSK_TIMER1_0_MSK) | (TIMER_32K << SC_TIMER_EN0_MSK_TIMER1_0_START_BIT);
    tmp_val = (tmp_val & SC_TIMER_EN0_TIMER1_0_MSK) | (TIMER_32K << SC_TIMER_EN0_TIMER1_0_START_BIT);
    writel(tmp_val, SOC_AO_SCTRL_SC_TIMER_EN0_ADDR(sctrl_addr));
    /*释放中断互斥*/
    spin_unlock(&g_sysctrl_lock);

    /*初始化timer*/
    tmp_val = TIMER_DEF_TC_DISABLE | TIMER_DEF_TC_PERIODIC | TIMER_DEF_TC_INTDISABLE |\
            TIMER_DEF_TC_CLK_DIV_1 | TIMER_DEF_TC_32BITS_MODE;
    tmp_val = (tmp_val & TIMER_DEF_TC_ONESHOT_MODE_BIT_MSK) | TIMER_DEF_TC_ONCE_MODE;
    writel(tmp_val, SOC_TIMER_TIMERN_CONTROL_ADDR(timer1_addr, OSA_TIMER_INDEX));

    g_om_timer_inited = TIMER_INITED;

    return 0;
}

/************************************************************************
 * FUNCTION
 *      start_om_hard_timer
 * DESCRIPTION
 *       start hard timer
 * INPUTS
 *       value -- timer's value.uint is 32K cycle
 * OUTPUTS
 *       NONE
 *************************************************************************/
int start_om_hard_timer(unsigned int value)
{
    volatile unsigned int tmp_val = 0;
    unsigned long timer1_addr = 0;

    if(TIMER_UNINITED == g_om_timer_inited)
    {
        printk("%s: om hardware timer uninitialized\n",__func__);
        return -1;
    }

    timer1_addr = timer1_base;

    /*停止timer*/
    tmp_val = readl(SOC_TIMER_TIMERN_CONTROL_ADDR(timer1_addr, OSA_TIMER_INDEX));
    tmp_val = (tmp_val & TIMER_DEF_TC_ENABLE_BIT_MSK) | TIMER_DEF_TC_DISABLE;
    writel(tmp_val, SOC_TIMER_TIMERN_CONTROL_ADDR(timer1_addr, OSA_TIMER_INDEX));

    /*将超时值配置到timer load寄存器*/
    writel(value, SOC_TIMER_TIMERN_LOAD_ADDR(timer1_addr, OSA_TIMER_INDEX));

    /*启动timer*/
    writel(TIMER_INT_CLEAR, SOC_TIMER_TIMERN_INTCLR_ADDR(timer1_addr, OSA_TIMER_INDEX));/* clear timer int*/
    tmp_val = (tmp_val & TIMER_DEF_TC_ENABLE_BIT_MSK) | TIMER_DEF_TC_ENABLE;
    tmp_val = (tmp_val & TIMER_DEF_TC_INT_BIT_MSK)| TIMER_DEF_TC_INTENABLE;
    writel(tmp_val, SOC_TIMER_TIMERN_CONTROL_ADDR(timer1_addr, OSA_TIMER_INDEX));

    return 0;
}


/************************************************************************
 * FUNCTION
 *       stop_om_hard_timer
 * DESCRIPTION
 *       Stop hard timer
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
int stop_om_hard_timer(void)
{
    volatile unsigned int value = 0;
    unsigned long timer1_addr = 0;

    if(TIMER_UNINITED == g_om_timer_inited)
    {
        printk("%s: om hardware timer uninitialized\n",__func__);
        return -1;
    }

    timer1_addr = timer1_base;

    value = readl(SOC_TIMER_TIMERN_CONTROL_ADDR(timer1_addr, OSA_TIMER_INDEX));
    value = (value & TIMER_DEF_TC_ENABLE_BIT_MSK) | TIMER_DEF_TC_DISABLE;
    value = (value & TIMER_DEF_TC_INT_BIT_MSK) | TIMER_DEF_TC_INTDISABLE;
    writel(value, SOC_TIMER_TIMERN_CONTROL_ADDR(timer1_addr, OSA_TIMER_INDEX));

    return 0;
}

/************************************************************************
 * FUNCTION
 *       get_om_hard_timer_curtime
 * DESCRIPTION
 *       Get hard timer Current Value
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
unsigned int get_om_hard_timer_curtime(void)
{
    unsigned int readValue = 0;

    unsigned long timer1_addr = 0;

    if(TIMER_UNINITED == g_om_timer_inited)
    {
        printk("%s: om hardware timer uninitialized\n",__func__);
        return 0;
    }

    timer1_addr = timer1_base;

    readValue = readl(SOC_TIMER_TIMERN_VALUE_ADDR(timer1_addr, OSA_TIMER_INDEX));

    return readValue;
}

/************************************************************************
 * FUNCTION
 *       clear_om_hard_timer_int
 * DESCRIPTION
 *       clear om hard timer interrupt
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
int clear_om_hard_timer_int(void)
{


    unsigned long timer1_addr = 0;

    if(TIMER_UNINITED == g_om_timer_inited)
    {
        printk("%s: om hardware timer uninitialized\n",__func__);
        return -1;
    }

    timer1_addr = timer1_base;

    writel(TIMER_INT_CLEAR, SOC_TIMER_TIMERN_INTCLR_ADDR(timer1_addr, OSA_TIMER_INDEX));

    return 0;
}

/************************************************************************
 * FUNCTION
 *       GetHardTimerCurTime
 * DESCRIPTION
 *       Get hard timer Current Value
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
unsigned int GetHardTimerCurTime(void)
{
    unsigned int readValue;

    readValue = get_om_hard_timer_curtime();

    return readValue;
}

module_init(init_om_hardware_timer);
EXPORT_SYMBOL(start_om_hard_timer);
EXPORT_SYMBOL(stop_om_hard_timer);
EXPORT_SYMBOL(get_om_hard_timer_curtime);
EXPORT_SYMBOL(clear_om_hard_timer_int);
EXPORT_SYMBOL(GetHardTimerCurTime);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

