/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : BSP_TIMER_PHY.c
  版 本 号   : 初稿
  作    者   : 蔡喜 220237
  生成日期   : 2013年3月30日
  最近修改   :
  功能描述   : 提供Timer寄存器控制功能
  函数列表   :
              timer_32bit_count_set
              timer_input_clk_get
              timer_int_clear
              timer_int_mask
              timer_int_status_get
              timer_int_unmask
              timer_load_get
              timer_load_set
              timer_raw_int_status_get
              timer_start
              timer_stop
              timer_sys_ctrl_set
              timer_value_get
              timer_is_running
  修改历史   :
  1.日    期   : 2013年3月30日
    作    者   : 蔡喜 220237
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/spinlock.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <linux/hisi/hi6xxx-platform.h>
#include <linux/io.h>
#include "soc_baseaddr_interface.h"
#include "soc_timer_interface.h"
#include "BSP.h"
#include "bsp_timer_phy.h"
#include "soc_ao_sctrl_interface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
 /*对timer控制寄存器的使能位读写互斥保护*/
extern spinlock_t timer_en_lock;

/* 划分到A核的所有Timer的基地址 */
unsigned int* pulTimerBase[TIMER_TOTAL_NUM + CCORE_TIMER_EXP] = {0};

#ifdef _DRV_LLT_
SOC_TIMER_TIMERN_CONTROL_UNION   g_SOC_TIMER_TIMERN_CONTROL_UNION;
SOC_TIMER_TIMERN_INTCLR_UNION   g_SOC_TIMER_TIMERN_INTCLR_UNION;
SOC_TIMER_TIMERN_RIS_UNION   g_SOC_TIMER_TIMERN_RIS_UNION;
SOC_TIMER_TIMERN_MIS_UNION   g_SOC_TIMER_TIMERN_MIS_UNION;
unsigned int g_SCtrl_TIMER_EN1_REG;
unsigned int g_SCtrl_TIMER_EN0_REG;
#define SOC_SCTRL_SC_TIMER_EN1_ADDR(base)  &g_SCtrl_TIMER_EN1_REG
#define SOC_SCTRL_SC_TIMER_EN0_ADDR(base)  &g_SCtrl_TIMER_EN0_REG
#define TIMER_MIS_REG(base)    (&g_SOC_TIMER_TIMERN_MIS_UNION)
#define TIMER_RIS_REG(base)    (&g_SOC_TIMER_TIMERN_RIS_UNION)
#define TIMER_INTC_REG(base)    (&g_SOC_TIMER_TIMERN_INTCLR_UNION)
#endif



/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : timer_find_phy_id
 功能描述  : 根据传入的逻辑timer ID, 转换为物理timer ID, 该物理ID为该核能访问的有效timer ID
 输入参数  : ucTimerId        Timer ID
 输出参数  : 无
 返 回 值  :   成功:返回相应的物理timerID
                          失败:返回-1
 调用函数  :
 被调函数  :

*****************************************************************************/
signed int timer_find_phy_id (unsigned char timerID)
{
    signed int ret = 0;
    switch (timerID)
    {
        case ACORE_SYSTEM_TIMER_ID:
            ret = TIMER_ACPU_SYSTEM_ID_PHY;
            break;
        case TIMER_ACPU_FREE_RUN:
            ret = TIMER_ACPU_FREE_RUN_PHY;
            break;
        case TIMER_ACPU_OSA_ID:
            ret = TIMER_ACPU_OSA_ID_PHY;
            break;
        case ACORE_SOFTTIMER_ID:
            ret = TIMER_ACPU_SOFTTIMER_ID_PHY;
            break;

        case TIMER_ACPU_IDLE_ID:
            ret = TIMER_ACPU_IDLE_ID_PHY;
            break;
        case TIMER_ACPU_OM_TCXO_ID:
            ret = TIMER_ACPU_OM_TCXO_ID_PHY;
            break;
        /* 目前只为TIMER_CCPU_CPUVIEW_ID配权限，使其可以读取c核timer17的value值 */
        case TIMER_CCPU_CPUVIEW_ID:
            ret = TIMER_TOTAL_NUM + 0;
            break;

        default:
            ret = -1;
            break;
    }
    return ret;
}
void timer_load_set(unsigned char ucTimerId, unsigned int ulValue)
{
    SOC_TIMER_TIMERN_CONTROL_UNION *pTIMER_CNTL_REG = NULL;
	if(NULL == pulTimerBase[ucTimerId])
	{
		printk("pulTimerBase[ucTimerId] is NULL");
		return;
	}
    pTIMER_CNTL_REG = (SOC_TIMER_TIMERN_CONTROL_UNION *)TIMER_CNTL_REG((unsigned char*)pulTimerBase[ucTimerId]);

    /* 置cntl寄存器为32位计数模式，默认为16位计数 */
    pTIMER_CNTL_REG->reg.timersize = SET_BIT_WIHT_TRUE;

    /* 将ulValue写入load寄存器 */
    TIMER_REG_WRITE(TIMER_LOAD_REG((unsigned long)pulTimerBase[ucTimerId]), ulValue);
}


unsigned int timer_load_get(unsigned char timerId)
{
    unsigned int result;
    TIMER_REG_READ(TIMER_LOAD_REG((unsigned long)pulTimerBase[timerId]), result);

    return result;
}


unsigned int timer_value_get(unsigned char timerId)
{
    unsigned int result;
    TIMER_REG_READ(TIMER_VALUE_REG((unsigned long)pulTimerBase[timerId]), result);

    return result;
}


signed int timer_is_running(unsigned char timerId)
{
    /* 依据定时器timerId，读取寄存器TIMERN_CONTROL[7] */
    SOC_TIMER_TIMERN_CONTROL_UNION *pTIMER_CNTL_REG = (SOC_TIMER_TIMERN_CONTROL_UNION *)TIMER_CNTL_REG((unsigned char*)pulTimerBase[timerId]);
    if (SET_BIT_WIHT_TRUE == pTIMER_CNTL_REG->reg.timeren)
    {
        return BSP_OK;
    }

    return BSP_ERROR;
}
void timer_start(unsigned char timerId, unsigned int mode)
{
    unsigned long flags;
    unsigned long base = HISI_VA_ADDRESS(REG_BASE_SC_ON);
    unsigned char dualTimerId = (unsigned char)timerId/2;

    /* 对定时器timerId，写寄存器TIMERN_CONTROL[7]，配置定时器停止 */
    SOC_TIMER_TIMERN_CONTROL_UNION *pTIMER_CNTL_REG =
        (SOC_TIMER_TIMERN_CONTROL_UNION *)TIMER_CNTL_REG((unsigned char*)pulTimerBase[timerId]);

    /*对timer控制寄存器的使能位读写互斥保护*/
    spin_lock_irqsave(&timer_en_lock, flags);

    /*使能dualt-timer的总线和工作时钟*/
    /*SC_PERIPH_CLKEN4 写1有效，写0无效*/
    TIMER_REG_WRITE(SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(base), SET_BITS_WIHT_TRUETRUE<<(2*dualTimerId+1));

    pTIMER_CNTL_REG->reg.timeren = SET_BIT_WIHT_FALSE;

    /*周期模式*/
    if ( (DRV_TIMER_MODE_E)mode == TIMER_PERIOD_COUNT)
    {
        /* 对定时器timerId，写寄存器TIMERN_CONTROL[0~1]和TIMERN_CONTROL[5,6,7]，
           配置定时器为32bit计数、周期模式且使能 */
        pTIMER_CNTL_REG->reg.oneshot = SET_BIT_WIHT_FALSE;
        pTIMER_CNTL_REG->reg.timersize = SET_BIT_WIHT_TRUE;
        pTIMER_CNTL_REG->reg.intenable = SET_BIT_WIHT_TRUE;
        pTIMER_CNTL_REG->reg.timermode = SET_BIT_WIHT_TRUE;
        pTIMER_CNTL_REG->reg.timeren = SET_BIT_WIHT_TRUE;
    }
    /*单次模式*/
    else if((DRV_TIMER_MODE_E)mode == TIMER_ONCE_COUNT)
    {
        /* 对定时器timerId，写寄存器TIMERN_CONTROL[0~1]和TIMERN_CONTROL[5,7]，
           配置定时器为32bit计数、单次模式且使能 */
        pTIMER_CNTL_REG->reg.oneshot = SET_BIT_WIHT_TRUE;
        pTIMER_CNTL_REG->reg.timersize = SET_BIT_WIHT_TRUE;
        pTIMER_CNTL_REG->reg.intenable = SET_BIT_WIHT_TRUE;
        pTIMER_CNTL_REG->reg.timeren = SET_BIT_WIHT_TRUE;
    }
    else
    {
        return;
    }

    spin_unlock_irqrestore(&timer_en_lock, flags);
}


void timer_stop(unsigned char timerId)
{
    unsigned long flags;



    /* 对定时器timerId，写寄存器TIMERN_CONTROL[7]，配置定时器停止 */
    SOC_TIMER_TIMERN_CONTROL_UNION *pTIMER_CNTL_REG =
          (SOC_TIMER_TIMERN_CONTROL_UNION *)TIMER_CNTL_REG((unsigned char*)pulTimerBase[timerId]);

    /*计算同dual-timer的另一个timerId*/
    unsigned char another_timerId = (timerId % 2) ? (timerId-1):(timerId+1);

    /*对timer控制寄存器的使能位读写互斥保护*/
    spin_lock_irqsave(&timer_en_lock, flags);

    pTIMER_CNTL_REG->reg.timeren = SET_BIT_WIHT_FALSE;

    /*判断同dualt-timer的另一个timer的使能状态*/
    if (BSP_OK != timer_is_running(another_timerId))
    {
        /*如果未使能，关闭此dual-timer的总线和工作时钟*/
        /*TIMER_REG_WRITE(SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(base), SET_BITS_WIHT_TRUETRUE<<(2*dualTimerId+1));*/
    }

    spin_unlock_irqrestore(&timer_en_lock, flags);
}


void timer_int_mask(unsigned char timerId)
{
    /* 对定时器timerId，写寄存器TIMERN_CONTROL[5]，配置为中断屏蔽状态 */
    SOC_TIMER_TIMERN_CONTROL_UNION *pTIMER_CNTL_REG =
        (SOC_TIMER_TIMERN_CONTROL_UNION *)TIMER_CNTL_REG((unsigned char*)pulTimerBase[timerId]);
    pTIMER_CNTL_REG->reg.intenable = SET_BIT_WIHT_FALSE;
}
void timer_int_unmask(unsigned char timerId)
{
    /* 对定时器timerId，写寄存器TIMERN_CONTROL[5]，配置为中断解屏蔽状态 */
    SOC_TIMER_TIMERN_CONTROL_UNION *pTIMER_CNTL_REG =
        (SOC_TIMER_TIMERN_CONTROL_UNION *)TIMER_CNTL_REG((unsigned char*)pulTimerBase[timerId]);
    pTIMER_CNTL_REG->reg.intenable = SET_BIT_WIHT_TRUE;
}
void timer_int_clear(unsigned char timerId)
{
    /* 对定时器timerId，写寄存器TIMERN_INTCLR[0]清除中断 */
    SOC_TIMER_TIMERN_INTCLR_UNION *pTIMER_INTC_REG =
        (SOC_TIMER_TIMERN_INTCLR_UNION *)TIMER_INTC_REG((unsigned char*)pulTimerBase[timerId]);
    pTIMER_INTC_REG->reg.timern_intclr = SET_BIT_WIHT_TRUE;
}
unsigned int timer_int_status_get(unsigned char timerId)
{
    /* 对定时器timerId，读寄存器TIMERN_MIS[0]，并返回该值 */
    SOC_TIMER_TIMERN_MIS_UNION *pTIMER_MIS_REG =
        (SOC_TIMER_TIMERN_MIS_UNION *)TIMER_MIS_REG((unsigned char*)pulTimerBase[timerId]);

    return(pTIMER_MIS_REG->reg.timernmis);
}
unsigned int timer_raw_int_status_get(unsigned char timerId)
{
    /* 对定时器timerId，读寄存器TIMERN_RIS[0]，并返回该值 */
    SOC_TIMER_TIMERN_RIS_UNION *pTIMER_RIS_REG =
        (SOC_TIMER_TIMERN_RIS_UNION *)TIMER_RIS_REG((unsigned char*)pulTimerBase[timerId]);

    return(pTIMER_RIS_REG->reg.timernris);
}
void  timer_32bit_count_set(unsigned char timerId)
{
    /* 对定时器timerId，写寄存器TIMERN_CONTROL[1]，配置为32-bit模式 */
    SOC_TIMER_TIMERN_CONTROL_UNION *pTIMER_CNTL_REG =
        (SOC_TIMER_TIMERN_CONTROL_UNION *)TIMER_CNTL_REG((unsigned char*)pulTimerBase[timerId]);
    pTIMER_CNTL_REG->reg.timersize = SET_BIT_WIHT_TRUE;
}
unsigned int timer_input_clk_get(unsigned char timerId)
{
    switch (timerId)
    {
        /* Timer时钟频率为32KHz */
        case TIMER_ACPU_OSA_ID_PHY:
        case TIMER_ACPU_SOFTTIMER_ID_PHY:
        case TIMER_ACPU_OM_TCXO_ID_PHY:
            return SLEEP_CLK_FREQ;

        /* Timer时钟频率为19.2MHz */
        case TIMER_ACPU_SYSTEM_ID_PHY:
        case TIMER_ACPU_FREE_RUN_PHY:
        case TIMER_ACPU_IDLE_ID_PHY:
            return TCXO_CLK_FREQ;

        default:
            return BSP_FALSE;
    }
}



signed int timer_sys_ctrl_set(void)
{
    unsigned int *pSCtrl_TIMER_EN0_REG = (unsigned int *)(HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_TIMER_EN0_ADDR(SOC_AO_SCTRL_BASE_ADDR)));
    unsigned int *pSCtrl_TIMER_EN1_REG = (unsigned int *)(HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_TIMER_EN1_ADDR(SOC_AO_SCTRL_BASE_ADDR)));
    SOC_AO_SCTRL_SC_TIMER_EN0_UNION SCtrl_TIMER_EN0_REG;
    SOC_AO_SCTRL_SC_TIMER_EN1_UNION SCtrl_TIMER_EN1_REG;

    /* 设置timer0,2,3 */
    SCtrl_TIMER_EN0_REG.value = 0;
    /*A 核低功耗使用dts配置timer0，不在此处配置*/
#if 0
    SCtrl_TIMER_EN0_REG.reg.timer_en0_1acpu_dualtimer0_en0 = TIMER_PCLK;
    SCtrl_TIMER_EN0_REG.reg.timer_en0_msk_1acpu_dualtimer0_en0 = SET_BITS_WIHT_TRUETRUE;
#endif
    SCtrl_TIMER_EN0_REG.reg.timer_en0_5acpu_dualtimer1_en0 = TIMER_32K;
    SCtrl_TIMER_EN0_REG.reg.timer_en0_msk_5acpu_dualtimer1_en0 = SET_BITS_WIHT_TRUETRUE;

    SCtrl_TIMER_EN0_REG.reg.timer_en0_7acpu_dualtimer1_en1 = TIMER_32K;
    SCtrl_TIMER_EN0_REG.reg.timer_en0_msk_7acpu_dualtimer1_en1 = SET_BITS_WIHT_TRUETRUE;

    *pSCtrl_TIMER_EN0_REG = SCtrl_TIMER_EN0_REG.value;


    SCtrl_TIMER_EN1_REG.value = 0;
    SCtrl_TIMER_EN1_REG.reg.timer_en1_9acpu_dualtimer6_en0 = TIMER_PCLK;
    SCtrl_TIMER_EN1_REG.reg.timer_en1_msk_9acpu_dualtimer6_en0 = SET_BITS_WIHT_TRUETRUE;

    SCtrl_TIMER_EN1_REG.reg.timer_en1_11acpu_dualtimer6_en1 = TIMER_32K;
    SCtrl_TIMER_EN1_REG.reg.timer_en1_msk_11acpu_dualtimer6_en1 = SET_BITS_WIHT_TRUETRUE;

    *pSCtrl_TIMER_EN1_REG = SCtrl_TIMER_EN1_REG.value;

    return BSP_OK;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

