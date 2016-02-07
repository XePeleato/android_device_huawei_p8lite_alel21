/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_pmu.c
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

#ifndef _DRV_LLT_
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#include "BSP_PMU_INTERFACE.h"
#endif
#endif
/*****************************************************************************
  2 函数声明
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : BSP_PMU_GetPMUState
 功能描述  : 获取PMU模块开机方式、充电状态、电池电量、电池在位状态。
 输入参数  : 无。
 输出参数  : Pmu_State :开机方式、充电状态、电池电量、电池在位状态。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
unsigned int DRV_GET_PMU_STATE(void*  Pmu_State)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return (unsigned int)(-1);
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

#define printk(a)


BSP_S32 BSP_PMU_AptDisable(BSP_VOID)
{
    printk("stub: BSP_PMU_AptEnable\n");
	return 0;
}


BSP_S32 BSP_PMU_AptEnable(BSP_VOID)
{
    printk("stub  monan :BSP_PMU_AptEnable. \n");
    return 0;
}


BSP_BOOL BSP_PMU_AptIsEnable(BSP_VOID)
{
    printk("stub: BSP_PMU_AptIsEnable\n");
    return 0;
}

int HI6451_BoardInit(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    printk("stub  monan :HI6451_BoardInit. \n");
    return 0;
#endif
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

