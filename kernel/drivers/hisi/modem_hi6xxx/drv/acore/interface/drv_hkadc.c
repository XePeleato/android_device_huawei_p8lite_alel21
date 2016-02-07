/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_hkadc.c
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
extern int DRV_HKADC_GET_TEMP(HKADC_TEMP_PROTECT_E log_chan,
                unsigned short* volt,
                short* temp, HKADC_CONV_MODE_E ulMode);

/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if 0
/*****************************************************************************
 函 数 名  : DRV_GET_BATTERY_ADC
 功能描述  : 获取电池电压采样值
 输入参数  : 无
 输出参数  : pslData： 指向电池电压的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
BSP_S32 DRV_GET_BATTERY_ADC(BSP_S32 * pslData)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#if( FEATURE_CHARGE == FEATURE_ON )
    return BSP_HKADC_BatADCRead(pslData);
#else
    DRV_PRINTF("Function DRV_GET_BATTERY_ADC unsupport!\n");
    return BSP_ERROR;
#endif
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*************************************************
 函 数 名   : DRV_HKADC_BAT_VOLT_GET
 功能描述   : 返回当前电池电压值
 输入参数   : pslData : 电池电压值
 输出参数   : pslData : 电池电压值
 返 回 值   :0:获取成功
            -1:获取失败
*************************************************/
BSP_S32 DRV_HKADC_BAT_VOLT_GET(BSP_S32 *ps32Data)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#if( FEATURE_CHARGE == FEATURE_ON )
    return BSP_HKADC_BatVoltGet(ps32Data);
#else
    DRV_PRINTF("Function DRV_HKADC_BAT_VOLT_GET unsupport!\n");
    return BSP_ERROR;
#endif
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/* add for taf/lt */
BSP_S32 BSP_HKADC_PaValueGet(BSP_U16 *value)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    short temp = 0;
    return DRV_HKADC_GET_TEMP(HKADC_TEMP_PA0, value, &temp, HKADC_CONV_DELAY);
#endif
#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}
#endif
/*****************************************************************************
     *  函 数 名  : mdrv_hkadc_get_temp
     *  功能描述  :  获取HKADC通道的电压和温度值。
     *  输入参数  :
     *
     *  输出参数  : 无
     *  返 回 值  :  0         操作成功。
        *                           其他    操作失败。
         *
     ******************************************************************************/
int mdrv_hkadc_get_temp (HKADC_TEMP_PROTECT_E enMode, unsigned short* pusVolt, short* psTemp, HKADC_CONV_MODE_E ulMode)
{
    return DRV_HKADC_GET_TEMP(enMode, pusVolt, psTemp, ulMode);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

