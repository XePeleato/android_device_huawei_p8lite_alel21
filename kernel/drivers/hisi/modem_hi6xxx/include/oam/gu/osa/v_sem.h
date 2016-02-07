/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_sem.h                                                         */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement semaphore                                          */
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

#ifndef _VOS_SEM_H
#define _VOS_SEM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "vos_config.h"

/* errno definiens */
#define VOS_ERRNO_SEMA4_CCREATE_NAME_NULL                   0x2002005A
#define VOS_ERRNO_SEMA4_CCREATE_INVALID_SMID                0x20020001
#define VOS_ERRNO_SEMA4_CCREATE_FLAG_ERR                    0x20020002
#define VOS_ERRNO_SEMA4_CCREATE_OBJTFULL                    0x20020003
#define VOS_ERRNO_SEMA4_CCREATE_OSALCREATEERR               0x20020004
#define VOS_TRAID_SEMA4_CCREATE                             0x20020037
#define VOS_EVENTID_SEMA4_CCREATED                          0x2002004B

#define VOS_ERRNO_SEMA4_P_IDERR                             0x20020010
#define VOS_ERRNO_SEMA4_P_NOTACTIVE                         0x20020011
#define VOS_ERRNO_SEMA4_P_TYPEERR                           0x20020013
#define VOS_ERRNO_SEMA4_P_CANOTP                            0x20020014
#define VOS_ERRNO_SEMA4_P_TIMEOUT                           0x20020015

#define VOS_ERRNO_SEMA4_V_IDERR                             0x2002001C
#define VOS_ERRNO_SEMA4_V_NOTACTIVE                         0x2002001D
#define VOS_ERRNO_SEMA4_V_M_CANOTV                          0x2002001F
#define VOS_ERRNO_SEMA4_V_CANOTV                            0x20020020
#define VOS_TRAID_SEMA4_V                                   0x20020041
#define VOS_ERRNO_SEMA4_BCREATE_INVALID_INIT                0x20020007

#define VOS_ERRNO_SEMA4_FULL                                0x2002FFFF



#define VOS_SEMA4_FIFO                                      0x00000001
#define VOS_SEMA4_PRIOR                                     0x00000002
#define VOS_SEMA4_DELETE_SAFE                               0x00000004
#define VOS_SEMA4_INVERSION_SAFE                            0x00000008
#define VOS_SEMA4_GLOBAL                                    0x00000010

/* the state of control block */
#define VOS_SEM_CTRL_BLK_IDLE                               0
#define VOS_SEM_CTRL_BLK_BUSY                               1

/* errno define */
#define VOS_MAX_SEM_ID_NULL                                 VOS_NULL_PTR

#define VOS_MAX_SEM_NAME_LENGTH                             8

/* define SEM's type */
#define VOS_SEM_TYPE_COUNT                                  0
#define VOS_SEM_TYPE_BINARY                                 1
#define VOS_SEM_TYPE_MUTEX                                  2

#if (VOS_WIN32 == VOS_OS_VER)
/* the Max Sem number supported by VOS */
#define VOS_MAX_SEM_NUMBER                                  300
#else
/* the Max Sem number supported by VOS */
#define VOS_MAX_SEM_NUMBER                                  200
#endif

typedef VOS_UINT_PTR                    VOS_SEM;

VOS_VOID VOS_SemCtrlBlkInit(VOS_VOID);


VOS_UINT32 VOS_SmMCreate( VOS_CHAR   Sm_Name[4],
                          VOS_UINT32 Flags,
                          VOS_SEM *Sm_ID );

VOS_UINT32 VOS_SmCCreate( VOS_CHAR   acSmName[4],
                          VOS_UINT32 ulSmInit,
                          VOS_UINT32 ulFlags,
                          VOS_SEM *pulSmID );

VOS_UINT32 VOS_SmBCreate( VOS_CHAR Sm_Name[4],
                          VOS_UINT32 Sm_Init,
                          VOS_UINT32 Flags,
                          VOS_SEM * Sm_ID );

VOS_UINT32 VOS_SmCreate( VOS_CHAR Sm_Name[4],
                         VOS_UINT32 Sm_Init,
                         VOS_UINT32 Flags,
                         VOS_SEM * Sm_ID );

VOS_UINT32 VOS_SmDelete( VOS_SEM Sm_ID );

VOS_UINT32 VOS_SmP( VOS_SEM Sm_ID, VOS_UINT32 ulTimeOutInMillSec );

VOS_UINT32 VOS_SmV( VOS_SEM Sm_ID );

VOS_UINT32 VOS_Sm_AsyP( VOS_SEM Sm_ID );

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_SEM_H */
