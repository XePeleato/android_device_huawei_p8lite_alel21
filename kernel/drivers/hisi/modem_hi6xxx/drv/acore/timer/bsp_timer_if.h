/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : BSP_TIMER.h
  版 本 号   : 初稿
  作    者   : 蔡喜 220237
  生成日期   : 2012年7月25日
  最近修改   :
  功能描述   : BSP_TIMER.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年7月25日
    作    者   : 蔡喜 220237
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "soc_baseaddr_interface.h"
#include "soc_peri_sctrl_interface.h"
#include <soc_ao_sctrl_interface.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 定时器使能位*/
#define TIMER_DEF_TC_ENABLE    (1 << 7)   /* Timer使能*/
#define TIMER_DEF_TC_DISABLE   (0 << 7)     /* Timer禁止*/
#define TIMER_DEF_TC_ENABLE_BIT_MSK       (~(1 << 7)) /* Timer开关位mask*/

/* 定时器的计数模式位*/
#define TIMER_DEF_TC_PERIODIC    (1 << 6)       /* user-defined count mode*/
#define TIMER_DEF_TC_FREERUN     (0 << 6)       /* free-running mode*/
#define TIMER_DEF_TC_COUNT_MODE_BIT_MSK       (~(1 << 6)) /* Timer计数模式位mask*/

/* TimerXRIS中断屏蔽位*/
#define TIMER_DEF_TC_INTENABLE   (1 << 5)       /* 不屏蔽该中断*/
#define TIMER_DEF_TC_INTDISABLE  (0 << 5)     /* 屏蔽该中断 */
#define TIMER_DEF_TC_INT_BIT_MSK       (~(1 << 5)) /* Timer中断屏蔽位mask*/

/* 定时器分频值*/
#define TIMER_DEF_TC_CLK_DIV_1   (0 << 2)       /* 1分频*/
#define TIMER_DEF_TC_CLK_DIV_16  (1 << 2)       /* 16分频 */
#define TIMER_DEF_TC_CLK_DIV_256 (2 << 2)       /* 256分频*/
#define TIMER_DEF_TC_CLK_DIV_BIT_MSK (~(3 << 2))    /* 分频位mask */

/* 定时器计数模式*/
#define TIMER_DEF_TC_16BITS_MODE    (0 << 1)   /* 16位计数模式*/
#define TIMER_DEF_TC_32BITS_MODE    (1 << 1)     /* 32位计数模式*/
#define TIMER_DEF_TC_BITS_MODE_BIT_MSK (~(1 << 1))    /* 计数模式位mask */
/* 定时器计数模式选择*/
#define TIMER_DEF_TC_ONCE_MODE    0x1   /* 一次计数模式*/
#define TIMER_DEF_TC_PERIOD_MODE  0x0     /* 周期性计数模式*/
#define TIMER_DEF_TC_ONESHOT_MODE_BIT_MSK (~(1 << 0))    /* oneshot位mask */

/*timer clk select bits*/
#define SC_TIMER_EN0_TIMER0_0_START_BIT 0
#define SC_TIMER_EN0_TIMER0_1_START_BIT 2

#define SC_TIMER_EN0_MSK_TIMER1_0_START_BIT 20
#define SC_TIMER_EN0_MSK_TIMER1_1_START_BIT 22

#define SC_TIMER_EN0_TIMER1_0_START_BIT 4
#define SC_TIMER_EN0_TIMER1_1_START_BIT 6

#define SC_TIMER_EN0_TIMER0_0_MSK (~(3 << SC_TIMER_EN0_TIMER0_0_START_BIT))
#define SC_TIMER_EN0_TIMER0_1_MSK (~(3 << SC_TIMER_EN0_TIMER0_1_START_BIT))

#define SC_TIMER_EN0_MSK_TIMER1_0_MSK (~(3 << SC_TIMER_EN0_MSK_TIMER1_0_START_BIT))
#define SC_TIMER_EN0_MSK_TIMER1_1_MSK (~(3 << SC_TIMER_EN0_MSK_TIMER1_1_START_BIT))

#define SC_TIMER_EN0_TIMER1_0_MSK (~(3 << SC_TIMER_EN0_TIMER1_0_START_BIT))
#define SC_TIMER_EN0_TIMER1_1_MSK (~(3 << SC_TIMER_EN0_TIMER1_1_START_BIT))

/*OM TIMER reg base & index*/
#define OSA_TIMER_REG_BASE SOC_Timer1_BASE_ADDR
#define OSA_TIMER_REG_SIZE 0x1000
#define OSA_TIMER_INDEX 0

#define TIMER_INT_CLEAR 1
#define TIMER_INT_VALID 1

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
typedef enum
{
    TIMER_PCLK                  = 0,/* 参考时钟选择pclk_mcpu */
    TIMER_TCXO_OFF_IN_SLEEP     = 1,/* 参考时钟选择19.2MHz，且进入sleep后自动关闭。 */
    TIMER_TCXO_SWITCH_IN_SLEEP  = 2,/* 参考时钟选择19.2MHz，且进入sleep后自动切换为32KHz */
    TIMER_32K                   = 3 /* 参考时钟选择32KHz */
}TIMER_TYPE;

typedef enum
{
    TIMER_UNINITED     = 0,/* timer未初始化 */
    TIMER_INITED       = 1,/* timer已初始化*/
}TIMER_INIT_FLAG;
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
int start_om_hard_timer(unsigned int value);
int stop_om_hard_timer(void);
unsigned int get_om_hard_timer_curtime(void);
int clear_om_hard_timer_int(void);
unsigned int GetHardTimerCurTime(void);




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of BSP_TIMER.h */
