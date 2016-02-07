/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_sec.c
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
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern int BSP_SEC_AlreadyUse(UINT8* pData);
extern int BSP_SEC_Start(void);
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*************************************************
 函 数 名       : DRV_SECURE_SUPPORT
 功能描述   : 当前版本是否支持安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
BSP_S32 DRV_SECURE_SUPPORT(BSP_U8 *pu8Data)
{
    return 0;
}

/*************************************************
 函 数 名       : DRV_SECURE_ALREADY_USE
 功能描述   : 查询当前版本是否已经启用安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
BSP_S32 DRV_SECURE_ALREADY_USE(BSP_U8 *pu8Data)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_SEC_AlreadyUse(pu8Data);
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*************************************************
 函 数 名       : DRV_START_SECURE
 功能描述   : 启用安全启动
 输入参数   :
 输出参数   :
 返 回 值      : OK/ERROR
*************************************************/
BSP_S32 DRV_START_SECURE(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_SEC_Start();
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}
BSP_U32 DRV_SEC_CHECK(void)
{
    return 0;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

