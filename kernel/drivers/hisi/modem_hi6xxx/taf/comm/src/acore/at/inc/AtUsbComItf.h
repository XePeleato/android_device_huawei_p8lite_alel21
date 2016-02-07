/************************************************************************
  Copyright   : 2005-2007, Huawei Tech. Co., Ltd.
  File name   : AtUsbComItf.h
  Author      : l47619
  Version     : BalongV100R001
  Date        : 2008-06-03
  Description : 该头文件定义了AT和USB COM驱动模块所用的接口
  History     :
  1. Date:2008-06-03
     Author: L47619
     Modification:Create
************************************************************************/

#ifndef __AT_USB_COM_ITF__
#define __AT_USB_COM_ITF__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*底软接口返回值*/
#define     AT_DRV_SUCCESS      0
#define     AT_DRV_FAILURE     -1

/*****************************************************************************
  3 接口定义
*****************************************************************************/
#pragma pack(4)


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of AtUsbComItf.h*/
