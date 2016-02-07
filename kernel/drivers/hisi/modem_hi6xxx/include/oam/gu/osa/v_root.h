/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_root.h                                                        */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: used bu vos_main.c                                           */
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

#ifndef _V_ROOT_H
#define _V_ROOT_H

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /* __cplusplus */
#endif /* __cplusplus */

enum VOS_STARTUP_PHASE
{
    VOS_STARTUP_INIT_DOPRA_SOFEWARE_RESOURCE   = 0,           /* DOPRA resource initialization                               */
    VOS_STARTUP_SET_TIME_INTERRUPT             = 1,           /* Set time interrupt and calculate the base value of cpu load */
    VOS_STARTUP_CREATE_TICK_TASK               = 2,           /* Create TICK task                                            */
    VOS_STARTUP_CREATE_ROOT_TASK               = 3,           /* Create ROOT task                                            */
    VOS_STARTUP_SUSPEND_MAIN_TASK              = 4            /* System startup complete. Suspend the main task              */
};

enum VOS_STARTUP_MODEL                                        /* System startup model            */
{
    VOS_STARTUP_NORMAL_MODEL                   = 0,           /* Normal model, restart if failed */
    VOS_STARTUP_DEBUG_MODEL                    = 1            /* Debug model, suspend if failed  */
};


VOS_UINT32 VOS_Startup( enum VOS_STARTUP_PHASE ph );

VOS_BOOL   VOS_IsSystemStartUp(VOS_VOID);

VOS_UINT32 VOS_GetStartUpOrder(VOS_VOID);

VOS_VOID   VOS_SetStartUpModel( VOS_UINT32 ulStartUpModel );

VOS_UINT32 VOS_GetStartUpModel(VOS_VOID);

VOS_VOID   VOS_NotifyDOPRAGoNext(VOS_VOID);

VOS_UINT32 VOS_RegModuleInfo( VOS_UINT32 ulModID, VOS_VOID* pFuncLib );

VOS_UINT32 VOS_GetModuleInfo( VOS_UINT32 ulModID, VOS_VOID* *pFuncLib );

VOS_UINT32 VOS_InitTrace( VOS_UINT32 ulLevel, VOS_CHAR * format, ... );

VOS_UINT32 VOS_SetInitTraceLevel( VOS_UINT32 ulLevel );

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif


