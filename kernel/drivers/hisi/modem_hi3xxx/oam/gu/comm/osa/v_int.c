/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_int.c                                                         */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement int function                                       */
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
/*                                                                           */
/*****************************************************************************/

#include "vos_config.h"
#include "v_typdef.h"
#include "v_int.h"
#include "v_IO.h"
#include "v_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#if (VOS_VXWORKS == VOS_OS_VER)

#if (VOS_DEBUG == VOS_CHECK_INTLOCK)

/* VOS 支持的最大中断嵌套数目 */
#define VOS_NESTED_INTLOCK_MAX_NUM      (128)

/* VOS 统计的锁中断时间间隔 单位slice 30us */
#define VOS_INTLOCK_MAX_INTERVAL        (900)

/* 嵌套信息记录 */
typedef struct
{
    VOS_UINT32                          ulSuffix; /* 数组下标 */
    VOS_NESTED_LOCK_INFO_STRU           astNestedInfo[VOS_NESTED_INTLOCK_MAX_NUM];
}VOS_NESTED_INTLOCK_STRU;

VOS_NESTED_INTLOCK_STRU g_stVosIntLockNestedInfo = {0};

/*****************************************************************************
 Function   : V_IntLock
 Description: Turn off the interrupt
 Input      : file line
 Return     : CPSR;
 Other      : none
 *****************************************************************************/
VOS_INT V_IntLock(VOS_UINT32 ulFileID, VOS_INT32 lLineNo)
{
    VOS_INT lFlag;

    lFlag = intLock();

    if ( VOS_NESTED_INTLOCK_MAX_NUM <= g_stVosIntLockNestedInfo.ulSuffix )
    {
        g_stVosIntLockNestedInfo.ulSuffix++;

        LogPrint2("# Info: intlock nested too big.F %d L %d.\r\n", (VOS_INT32)ulFileID, (VOS_INT32)lLineNo);

        return lFlag;
    }

    g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].ulFile  = ulFileID;
    g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].lLine   = lLineNo;
    g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].ulSlice = VOS_GetSlice();

    g_stVosIntLockNestedInfo.ulSuffix++;

    return lFlag;

}

/*****************************************************************************
 Function   : V_IntUnlock
 Description: Turn on the interrupt
 Input      : CPSR
 Return     : void;
 Other      : none
 *****************************************************************************/
VOS_VOID V_IntUnlock(VOS_INT lLockKey)
{
    g_stVosIntLockNestedInfo.ulSuffix--;

    if ( VOS_NESTED_INTLOCK_MAX_NUM <= g_stVosIntLockNestedInfo.ulSuffix )
    {
        intUnlock(lLockKey);

        return;
    }

    if ( VOS_INTLOCK_MAX_INTERVAL <
        (VOS_GetSlice() - g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].ulSlice) )
    {
         LogPrint2("# Info: intlock time too big.F %d L %d.\r\n",
            (VOS_INT32)g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].ulFile,
            (VOS_INT32)g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].lLine);
    }

    intUnlock(lLockKey);

    return;
}


#endif

#endif

#if (VOS_RTOSCK == VOS_OS_VER)

#if (VOS_DEBUG == VOS_CHECK_INTLOCK)

/* VOS 支持的最大中断嵌套数目 */
#define VOS_NESTED_INTLOCK_MAX_NUM      (128)

/* VOS 统计的锁中断时间间隔 单位slice 30us */
#define VOS_INTLOCK_MAX_INTERVAL        (900)

/* 嵌套信息记录 */
typedef struct
{
    VOS_UINT32                          ulSuffix; /* 数组下标 */
    VOS_NESTED_LOCK_INFO_STRU           astNestedInfo[VOS_NESTED_INTLOCK_MAX_NUM];
}VOS_NESTED_INTLOCK_STRU;

VOS_NESTED_INTLOCK_STRU g_stVosIntLockNestedInfo = {0};

/*****************************************************************************
 Function   : V_IntLock
 Description: Turn off the interrupt
 Input      : file line
 Return     : CPSR;
 Other      : none
 *****************************************************************************/
VOS_INT V_IntLock(VOS_UINT32 ulFileID, VOS_INT32 lLineNo)
{
    VOS_INT lFlag;

    lFlag = (VOS_INT)SRE_IntLock();

    if ( VOS_NESTED_INTLOCK_MAX_NUM <= g_stVosIntLockNestedInfo.ulSuffix )
    {
        g_stVosIntLockNestedInfo.ulSuffix++;

        LogPrint2("# Info: intlock nested too big.F %d L %d.\r\n", (VOS_INT32)ulFileID, (VOS_INT32)lLineNo);

        return lFlag;
    }

    g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].ulFile  = ulFileID;
    g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].lLine   = lLineNo;
    g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].ulSlice = VOS_GetSlice();

    g_stVosIntLockNestedInfo.ulSuffix++;

    return lFlag;

}

/*****************************************************************************
 Function   : V_IntUnlock
 Description: Turn on the interrupt
 Input      : CPSR
 Return     : void;
 Other      : none
 *****************************************************************************/
VOS_VOID V_IntUnlock(VOS_INT lLockKey)
{
    g_stVosIntLockNestedInfo.ulSuffix--;

    if ( VOS_NESTED_INTLOCK_MAX_NUM <= g_stVosIntLockNestedInfo.ulSuffix )
    {
        SRE_IntRestore((VOS_UINT32)lLockKey);

        return;
    }

    if ( VOS_INTLOCK_MAX_INTERVAL <
        (VOS_GetSlice() - g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].ulSlice) )
    {
         LogPrint2("# Info: intlock time too big.F %d L %d.\r\n",
            (VOS_INT32)g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].ulFile,
            (VOS_INT32)g_stVosIntLockNestedInfo.astNestedInfo[g_stVosIntLockNestedInfo.ulSuffix].lLine);
    }

    SRE_IntRestore((VOS_UINT32)lLockKey);

    return;
}


#endif

#endif

#if (VOS_WIN32 == VOS_OS_VER)

CRITICAL_SECTION VOS_CriticalSection;

/*****************************************************************************
 Function   : VOS_SplInit()
 Description: Initialize the interrupt
 Input      : None
 Return     : Nnoe
 *****************************************************************************/
VOS_VOID VOS_SplInit()
{
    InitializeCriticalSection( &VOS_CriticalSection );
}

/*****************************************************************************
 Function   : VOS_SplIMP()
 Description: Turn off the interrupt
 Input      : None
 Return     : VOS_OK;
 Other      : none
 *****************************************************************************/
VOS_INT32 VOS_SplIMP()
{
    EnterCriticalSection( &VOS_CriticalSection );
    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_Splx()
 Description: Turn on the interrupt
 Input      : s -- value returned by VOS_SplIMP()
 Return     : None
 *****************************************************************************/
VOS_VOID VOS_Splx( VOS_INT32 s )
{
    LeaveCriticalSection ( &VOS_CriticalSection );
}

/*****************************************************************************
 Function   : atomic_inc_return()
 Description: add atomic Var then return
 Input      : atomic Var
 Return     : current value
 *****************************************************************************/
VOS_UINT32 atomic_inc_return(atomic_t *pstV)
{
    VOS_INT32   lFlags;
    VOS_UINT32  ulCounter;

    lFlags = VOS_SplIMP();

    (pstV->ulCounter)++;

    ulCounter = pstV->ulCounter;

    VOS_Splx(lFlags);

    return ulCounter;
}

#endif

#if (VOS_LINUX == VOS_OS_VER)
/*****************************************************************************
 Function   : VOS_SplIMP()
 Description: Turn off the interrupt
 Input      : None
 Return     : VOS_OK;
 Other      : none
 *****************************************************************************/
VOS_ULONG VOS_SplIMP(VOS_VOID)
{
    VOS_ULONG  flags;

    local_irq_save(flags);

    return flags;
}

/*****************************************************************************
 Function   : VOS_Splx()
 Description: Turn on the interrupt
 Input      : s -- value returned by VOS_SplIMP()
 Return     : None
 *****************************************************************************/
VOS_VOID VOS_Splx( VOS_ULONG s )
{
    local_irq_restore(s);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


