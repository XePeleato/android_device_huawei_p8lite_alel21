/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_wdt.c
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


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/************************************************************************
 * FUNCTION
 *       BSP_ClearZSPWatchDogInt
 * DESCRIPTION
 *       完成ZSP Watch Dog中断清除
 * INPUTS
 *       NONE
 * OUTPUTS
 *       BSP_OK/BSP_ERR
 *************************************************************************/

BSP_BOOL BSP_ClearZSPWatchDogInt(BSP_VOID)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_TRUE;
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/************************************************************************
 * FUNCTION
 *       BSP_ClearHIFIWatchDogInt
 * DESCRIPTION
 *       完成HIFI Watch Dog中断清除
 * INPUTS
 *       NONE
 * OUTPUTS
 *       BSP_OK/BSP_ERR
 *************************************************************************/

BSP_BOOL BSP_ClearHIFIWatchDogInt(BSP_VOID)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_TRUE;
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
 函 数 名  : BSP_HIFIWatchDogIntEnalbe
 功能描述  : HIFI Watch Dog中断使能
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
BSP_VOID BSP_HIFIWatchDogIntEnalbe(int level)
{
    return;
}

/*****************************************************************************
 函 数 名  : BSP_ZspWatchDogIntEnalbe
 功能描述  : ZSP Watch Dog中断使能
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
BSP_VOID BSP_ZSPWatchDogIntEnalbe(int level)
{
    return;
}

#if 0
/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed
*
* 功能描述  : 重置看门狗计数寄存器（喂狗）
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_HardwareFeed(BSP_U8 wdtId)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;
#endif

#if (defined BSP_CORE_APP)
    return BSP_OK;
#endif
}
#endif

/*****************************************************************************
*  函 数 名  : mdrv_wdt_feed
*  功能描述  : 重置开门狗
*  输入参数  : wdtId    开门狗ID
*
*  输出参数  : 无
*  返 回 值  :  0       操作成功。
*               其他    操作失败。
*
******************************************************************************/
int  mdrv_wdt_feed(unsigned char wdtId)
{
    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

