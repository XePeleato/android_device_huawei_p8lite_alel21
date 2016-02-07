/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_timer.c
  版 本 号   : 初稿
  作    者   : c61362
  生成日期   : 2012年3月2日
  最近修改   :
  功能描述   : 底软给上层软件封装的接口层
  修改历史   :
  1.日    期   : 2012年3月2日
    作    者   : c61362
    修改内容   : 新建Drvinterface.c

  2.日    期   : 2013年2月19日
    作    者   : 蔡喜 220237
    修改内容   : 由Drvinterface.c拆分所得

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/


#include "BSP.h"
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#include "stdio.h"
#ifdef __OS_RTOSCK__
#else
#include <vxWorks.h>
#include <private/vmLibP.h>
#include <cacheLib.h>
#include "arm_pbxa9.h"
#include <cacheLib.h>
#endif
#include "BSP_TIMER.h"
#include "sys_config.h"
#include "soc_baseaddr_interface.h"
#include "string.h"
#endif

#if (defined BSP_CORE_APP)
#include <bsp_timer.h>
#endif

#include "soc_rtc_interface.h"
#include "soc_baseaddr_interface.h"
#include "soc_timer_interface.h"
#if defined (CHIP_BB_HI6210)
#include "soc_peri_sctrl_interface.h"
#elif defined (CHIP_BB_HI6620)
#include "soc_sctrl_interface.h"
#endif
#include <soc_ao_sctrl_interface.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 函数声明
*****************************************************************************/
extern unsigned int get_rtc_timer_value(void);
/*****************************************************************************
  3 函数实现
*****************************************************************************/
extern unsigned int omTimerGet(void);
/********************************************************************************************************************
 函 数 名  : BSP_GetSliceValue
 功能描述  : 获取时间戳Timer的当前值
 输出参数  :
 返 回 值  :
********************************************************************************************************************/
unsigned int BSP_GetSliceValue(void)
{
    return omTimerGet();
}

extern BSP_S32 BSP_TIMER_GetRestTime(BSP_U32 clk_logic_id, BSP_U32 unitType, BSP_U32 * pRestTime);

int mdrv_timer_get_rest_time(unsigned int usrClkId, unsigned int unitType, unsigned int * pRestTime)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_TIMER_GetRestTime(usrClkId, unitType, pRestTime);
#endif

#if (defined BSP_CORE_APP)
    return BSP_TIMER_GetRestTime(usrClkId, unitType, pRestTime);
#endif
}

extern BSP_S32 BSP_TIMER_UsrClkStart
(
    BSP_U32     clk_logic_id,
    FUNCPTR_1   routine,
    BSP_S32     arg,
    BSP_U32     timerValue,
    BSP_U32     mode,
    BSP_U32     unitType
);

int mdrv_timer_start
(
    unsigned int     usrClkId,
    FUNCPTR_1        routine,
    int              arg,
    unsigned int     timerValue,
    unsigned int     mode,
    unsigned int     unitType
)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_TIMER_UsrClkStart(usrClkId, routine, arg, timerValue, mode, unitType);
#endif

#if (defined BSP_CORE_APP)
    return BSP_TIMER_UsrClkStart(usrClkId, routine, arg, timerValue, mode, unitType);
#endif
}

extern BSP_S32 BSP_TIMER_UsrClkDisable(BSP_U32 clk_logic_id);

int mdrv_timer_stop(unsigned int usrClkId)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_TIMER_UsrClkDisable(usrClkId);
#endif

#if (defined BSP_CORE_APP)
    return BSP_TIMER_UsrClkDisable(usrClkId);
#endif
}

/*****************************************************************************
 函 数 名  : BSP_GET_RTC_VALUE
 功能描述  : 读内rtc寄存器，获得当前RTC的值
 输入参数  : 无
 输出参数  : 无
 返回值    ：RTC值，单位s
 修改说明  :
             V9sft A、C核规格实现(A核在mach_info.c文件中，C核在BSP_TIMER.c文件中)
             V3sft A、C核打桩
*****************************************************************************/

unsigned int BSP_GET_RTC_VALUE(void)
{
    BSP_U32 result;

    result = get_rtc_timer_value();
    return result;
}

unsigned int mdrv_rtc_get_value(void)
{
    return BSP_GET_RTC_VALUE();
}

/************************************************************************
     *  FUNCTION
     *       mdrv_timer_get_normal_timestamp
     * DESCRIPTION
     *       GetSliceValue
     * INPUTS
     *       NONE
     * OUTPUTS
     *       NONE
             获取时间戳，时间戳返回值为递增值
*************************************************************************/
extern unsigned int omTimerGet(void);
unsigned int mdrv_timer_get_normal_timestamp(void)
{
    return omTimerGet();
}

/*****************************************************************************
 函 数 名  : mdrv_timer_get_accuracy_timestamp
 功能描述  : 用来获取高精度的定时器的值，给协议栈提供的接口。
 输入参数  : 无
 输出参数  : unsigned int  *pulHigh32bitValue 存储高32bit值
             unsigned int  *pulLow32bitValue  存储低32bit值
 返回值    ：0 成功
            -1 失败
*****************************************************************************/
int mdrv_timer_get_accuracy_timestamp(unsigned int  *pulHigh32bitValue,  unsigned int  *pulLow32bitValue)
{
    return bsp_get_accuracy_timestamp(pulHigh32bitValue,pulLow32bitValue);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

