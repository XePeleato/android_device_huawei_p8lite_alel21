/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_version.c
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
#include "board_diff.h"
#include "sys_config.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_VOID WriteChipVersion(BSP_U32 ulAddress, BSP_U32 ucType);
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : mdrv_misc_support_check
 功能描述  : 查询模块是否支持
 输入参数  : enModuleType: 需要查询的模块类型
 输出参数  : 无
 返回值    ：BSP_MODULE_SUPPORT或BSP_MODULE_UNSUPPORT
*****************************************************************************/
BSP_MODULE_SUPPORT_E mdrv_misc_support_check(BSP_MODULE_TYPE_E enModuleType)
{
    BSP_MODULE_SUPPORT_E enRet = BSP_MODULE_SUPPORT;

    switch(enModuleType)
    {
        case BSP_MODULE_TYPE_LOCALFLASH:

            enRet = BSP_MODULE_SUPPORT;

            break;
        default:
            enRet = BSP_MODULE_UNSUPPORT;
            break;
    }

    return enRet;
}

void mdrv_ver_init_platform(BSP_VOID)
{
    return;
}

int mdrv_ver_get_memberinfo(void ** ppVersionInfo, unsigned int * ullength )
{
	return 0;
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

