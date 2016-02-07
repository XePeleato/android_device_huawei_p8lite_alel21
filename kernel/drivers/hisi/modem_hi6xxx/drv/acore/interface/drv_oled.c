/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_oled.c
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
#include "sys_config.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern int printf(const char *format, ...);
/*****************************************************************************
  2 函数声明
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
*  Function:  DRV_OLED_CLEAR_WHOLE_SCREEN
*  Description: oled clear *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
BSP_VOID DRV_OLED_CLEAR_WHOLE_SCREEN(BSP_VOID)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)    
#if( FEATURE_OLED == FEATURE_ON )
    oledClearWholeScreen();
#else
    DRV_PRINTF("Function DRV_OLED_CLEAR_WHOLE_SCREEN unsupport!\n");
#endif
#endif

#if (defined BSP_CORE_APP)
    DRV_PRINTF("Function DRV_OLED_CLEAR_WHOLE_SCREEN unsupport!\n");
#endif
}

/*****************************************************************************
*  Function:  DRV_OLED_UPDATE_STATE_DISPLAY
*  Description: oled display right or not right  *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
void DRV_OLED_UPDATE_STATE_DISPLAY(int UpdateStatus)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#if( FEATURE_OLED == FEATURE_ON )
    oledUpdateDisplay(UpdateStatus);
#else
    DRV_PRINTF("Function DRV_OLED_UPDATE_STATE_DISPLAY unsupport!\n");
#endif
#endif

#if (defined BSP_CORE_APP)
    DRV_PRINTF("Function DRV_OLED_UPDATE_STATE_DISPLAY unsupport!\n");
#endif
}

/*****************************************************************************
*  Function:  DRV_OLED_UPDATE_DISPLAY
*  Description: oled display right or not right  *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
void DRV_OLED_UPDATE_DISPLAY(int UpdateStatus)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)    
#if( FEATURE_OLED == FEATURE_ON )
    oledUpdateDisplay(UpdateStatus);
#else
    DRV_PRINTF("Function DRV_OLED_UPDATE_DISPLAY unsupport!\n");
#endif
#endif

#if (defined BSP_CORE_APP)
    DRV_PRINTF("Function DRV_OLED_UPDATE_DISPLAY unsupport!\n");
#endif
}

/*****************************************************************************
 函 数 名  : DRV_OLED_STRING_DISPLAY
 功能描述  : oled字符串显示函数
 输入参数  :
 *           要显示字符串的起始坐标
 *           要显示的字符串
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
BSP_VOID DRV_OLED_STRING_DISPLAY(UINT8 ucX, UINT8 ucY, UINT8 *pucStr)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#if( FEATURE_OLED == FEATURE_ON )
    oledStringDisplay(ucX,ucY,pucStr);
#else
    DRV_PRINTF("Function DRV_OLED_STRING_DISPLAYs unsupport!\n");
#endif
#endif

#if (defined BSP_CORE_APP)
    DRV_PRINTF("Function DRV_OLED_STRING_DISPLAYs unsupport!\n");
#endif
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

