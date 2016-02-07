/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: FileSysInterface.h                                                 */
/*                                                                           */
/* Author:                                                                   */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-07                                                             */
/*                                                                           */
/* Description: Announcement of File System Interface.                               */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-09-18                                                          */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/
#ifndef __FILESYS_INTERFACE_H__
#define __FILESYS_INTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "ccore/mdrv_rfile.h"

#pragma pack(4)


/*****************************************************************************
 Balong协议栈封装的结构体定义
*****************************************************************************/

typedef int                 DRV_STATUS;
typedef unsigned int        DRV_SIZE_T;
typedef long                DRV_OFF_T;
//typedef	unsigned long       DRV_INO_T;

#if (defined BSP_CORE_APP)
typedef int FILE;
#endif
/*****************************************************************************
 Balong协议栈封装的函数声明
*****************************************************************************/

#pragma pack(0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


