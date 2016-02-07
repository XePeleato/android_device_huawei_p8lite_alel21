/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : BSP_TIMER_PHY.h
  版 本 号   : 初稿
  作    者   : 蔡喜 220237
  生成日期   : 2013年3月30日
  最近修改   :
  功能描述   : Bsp_timer_phy.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年3月30日
    作    者   : 蔡喜 220237
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef __BSP_TIMER_PHY_H__
#define __BSP_TIMER_PHY_H__

#include "soc_timer_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define TIMER_PRINT_ERR             KERN_ERR
#define TIMER_PRINT_INFO            KERN_INFO

#define TIMER_LOAD_REG(base)      SOC_TIMER_TIMERN_LOAD_ADDR(base, 0)
#define TIMER_VALUE_REG(base)     SOC_TIMER_TIMERN_VALUE_ADDR(base, 0)
#define TIMER_CNTL_REG(base)      SOC_TIMER_TIMERN_CONTROL_ADDR(base, 0)
#define TIMER_INTC_REG(base)      SOC_TIMER_TIMERN_INTCLR_ADDR(base, 0)
#define TIMER_RIS_REG(base)       SOC_TIMER_TIMERN_RIS_ADDR(base, 0)
#define TIMER_MIS_REG(base)       SOC_TIMER_TIMERN_MIS_ADDR(base, 0)

#define TIMER_TOTAL_NUM 18          /* V9R1 SFT Acore的Timer资源总数 */

#define TIMER_ACPU_ID_MAX  6
#define TIMER_ACPU_ID_BEGIN 8

#ifdef _DRV_LLT_
extern SOC_TIMER_TIMERN_CONTROL_UNION   g_SOC_TIMER_TIMERN_CONTROL_UNION;
#define TIMER_CNTL_REG(base)    (&g_SOC_TIMER_TIMERN_CONTROL_UNION)
#endif


#ifndef TIMER_REG_READ
#ifdef _DRV_LLT_
#define TIMER_REG_READ(reg,result)  ((result) = 1)
#else
#define TIMER_REG_READ(reg,result) BSP_REG_READ(0, reg, result)
#endif
#endif

#ifndef TIMER_REG_WRITE
#ifdef _DRV_LLT_
#define TIMER_REG_WRITE(reg,data)  1
#else
#define TIMER_REG_WRITE(reg,data)  BSP_REG_WRITE(0, reg, data)
#endif
#endif

/* 输入时钟配置 */
#define SLEEP_CLK_FREQ (32768)
#define TCXO_CLK_FREQ  (19200000)
#define TIMER_LOAD_MAX_VALUE    (0xFFFFFFFF)


#define SC_TIMER_EN0_Ctrl_ENABLE_START_INDEX    (16)        /* 系统控制器SC_TIMER_EN0中使能时钟配置的起始位置 */
#define SC_TIMER_EN1_Ctrl_ENABLE_START_INDEX    (16)        /* 系统控制器SC_TIMER_EN1中使能时钟配置的起始位置 */
#define SC_TIMER_EN1_START_INDEX                ( 8)        /* 系统控制器SC_TIMER_EN1中控制时钟的起始位置 */
#define SCtrl_BIT_FOR_TIMER                     ( 2)        /* 系统控制器SC_TIMER_EN0,1,2中配置每个timer用的位数 */

#define SET_BIT_WIHT_TRUE   0x1       /* 设置该比特位为1 */
#define SET_BIT_WIHT_FALSE  0x0       /* 设置该比特位为0 */
#define SET_BITS_WIHT_TRUETRUE  0x3   /* 设置这2个比特位为11 */

#define TIMER_INVALID_ADDR              0xFFFFFFFF   /* 无效地址 */

typedef enum
{
    TIMER_PCLK                  = 0,/* 参考时钟选择pclk_mcpu */
#if defined(BOARD_SFT)
    TIMER_TCXO_OFF_IN_SLEEP     = TIMER_PCLK, /*B50逻辑无单独19MHZ*//* 1. 参考时钟选择19.2MHz，且进入sleep后自动关闭。 */
    TIMER_TCXO_SWITCH_IN_SLEEP  = TIMER_PCLK, /*B50逻辑无单独19MHZ*//* 2.参考时钟选择19.2MHz，且进入sleep后自动切换为32KHz */
#elif defined(BOARD_ASIC)
    TIMER_TCXO_OFF_IN_SLEEP         = 1,
    TIMER_TCXO_SWITCH_IN_SLEEP      = 2,
#endif
    TIMER_32K                   = 3 /* 参考时钟选择32KHz */
}TIMER_TYPE;

/*****************************************************************************
  3 枚举定义 
*****************************************************************************/

typedef enum
{
    TIMER_ACPU_SYSTEM_ID_PHY    = 0,
    TIMER_ACPU_FREE_RUN_PHY     = 1, 
    TIMER_ACPU_OSA_ID_PHY       = 2,   
    TIMER_ACPU_SOFTTIMER_ID_PHY = 3, 
    TIMER_ACPU_IDLE_ID_PHY      = 12,
    TIMER_ACPU_OM_TCXO_ID_PHY   = 13,
    TIMER_ID_MAX_PHY
} DRV_TIMER_ID_PHY;



/* C核的8_1号Timer 读value寄存器 */
#define CCORE_TIMER_EXP 1
#define ulCoreTimer8_1  17

#define SC_TIMER_EN0_START_TIMER    0
#define SC_TIMER_EN1_START_TIMER    8

#define TIMER_REG_SIZE 0x1000

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern void timer_32bit_count_set(unsigned char timerId);
extern unsigned int timer_input_clk_get(unsigned char timerId);
extern void timer_int_clear(unsigned char timerId);
extern void timer_int_mask(unsigned char timerId);
extern unsigned int timer_int_status_get(unsigned char timerId);
extern void timer_int_unmask(unsigned char timerId);
extern unsigned int timer_load_get(unsigned char timerId);
extern void timer_load_set(unsigned char ucTimerId, unsigned int ulValue);
extern unsigned int timer_raw_int_status_get(unsigned char timerId);
extern void timer_start(unsigned char timerId, unsigned int mode);
extern void timer_stop(unsigned char timerId);
extern signed int timer_sys_ctrl_set(void);
extern unsigned int timer_value_get(unsigned char timerId);
extern signed int timer_find_phy_id (unsigned char timerID);
extern signed int timer_is_running(unsigned char timerId);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of BSP_TIMER_PHY.h */
