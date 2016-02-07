/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: vos_config.h                                                    */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/* 2. Date: 2006-10                                                          */
/*    Author: Xu Cheng                                                       */
/*    Modification: Standardize code                                         */
/*                                                                           */
/*****************************************************************************/


#ifndef _V_CONFIG_H
#define _V_CONFIG_H

#include "dopra_def.h"
#include "PsLogdef.h"
#include "product_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#define VOS_PID_TIMER                   1

#define MILLISECONDS_PER_TICK           10 /* 宏值不能为0，否则函数VOS_TmTickToMillSec会出异常 */

#ifndef VOS_OS_VER
#define VOS_OS_VER                      VOS_VXWORKS
#endif

#ifndef VOS_HARDWARE_PLATFORM
#define VOS_HARDWARE_PLATFORM           VOS_ARM
#endif

#ifndef VOS_CPU_TYPE
#define VOS_CPU_TYPE                    VOS_ARM
#endif

#ifndef VOS_MEMORY_CHECK
#define VOS_MEMORY_CHECK                VOS_YES
#endif

#ifndef VOS_MEMORY_COUNT
#define VOS_MEMORY_COUNT                VOS_YES
#endif

#ifndef VOS_TIMER_CHECK
#define VOS_TIMER_CHECK                 VOS_YES
#endif

#ifndef VOS_CHECK_PARA
#define VOS_CHECK_PARA                  VOS_YES
#endif

/*
define the byte order according to the VOS_HARDWARE_PLATFORM
*/
#define VOS_BYTE_ORDER                  VOS_LITTLE_ENDIAN

#ifndef VOS_DOPRA_VER
#define VOS_DOPRA_VER                   VOS_DEBUG
#endif

#define VOS_WATCHDOG_VERSION            VOS_RELEASE

#define HARD_DOG_ENABLE                 VOS_RELEASE

#define VOS_ANALYZE_PID_MEM             VOS_NO

#define VOS_FILE_ID                     THIS_FILE_ID

#define VOS_CHECK_INTLOCK               VOS_RELEASE

#define VOS_CHECK_TASKLOCK              VOS_RELEASE

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_CONFIG_H */
