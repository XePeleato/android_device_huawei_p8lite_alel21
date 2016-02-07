/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_ifc.c
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
#if (defined BSP_CORE_APP)
#include <linux/module.h>
#endif
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

/*****************************************************************************
* 函 数 名  : BSP_IFC_RegFunc
*
* 功能描述  : 注册IFC回调函数接口
*
* 输入参数  : IFC_MODULE_E enModuleId    模块ID
*             BSP_IFC_REG_FUNC pFunc     回调函数指针
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*
* 其它说明  : 无
*
*****************************************************************************/
#if (defined BSP_CORE_APP)
BSP_U32 BSP_IFC_RegFunc(IFC_MODULE_E enModuleId, BSP_IFC_REG_FUNC pFunc)
{    
    return BSP_ERR_MODULE_NOT_INITED;
}
#endif
/*****************************************************************************
* 函 数 名  : BSP_IFC_Send
*
* 功能描述  : IFC发送消息接口
*
* 输入参数  : BSP_VOID * pMspBody           发送消息内容
*             BSP_U32 u32Len               pMspBody的长度
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_IFC_SEND_FAIL
*
* 其它说明  : 无
*
*****************************************************************************/
#if (defined BSP_CORE_APP)
BSP_U32 BSP_IFC_Send(BSP_VOID * pMspBody, BSP_U32 u32Len)
{    
    return BSP_ERR_MODULE_NOT_INITED;
}
#endif

#if (defined BSP_CORE_APP)
EXPORT_SYMBOL(BSP_IFC_RegFunc);
EXPORT_SYMBOL(BSP_IFC_Send);
#endif



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

