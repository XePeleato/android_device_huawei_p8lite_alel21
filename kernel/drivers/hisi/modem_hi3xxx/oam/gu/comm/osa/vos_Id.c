/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: vos_Id.c                                                        */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement FID&PID                                            */
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

#include "vos_Id.h"
#include "v_iddef.inc"
#include "v_id.inc"
#include "v_timer.h"
#include "v_int.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 modify:x51137; review:h59254; cause:print log */
#define    THIS_FILE_ID        PS_FILE_ID_VOS_ID_C
/*lint +e767 modify:x51137; review:h59254; */

/* recoed PID info VOS_PID_BUTT comes from v_id.inc */
VOS_INT                 vos_PidRecordsNumber;
VOS_PID_RECORD          vos_PidRecords[VOS_PID_BUTT-VOS_PID_DOPRAEND];

/* recoed FID info VOS_FID_BUTT comes from v_id.inc */
VOS_INT                 vos_FidCtrlBlkNumber;
VOS_FID_CONTROL_BLOCK   vos_FidCtrlBlk[VOS_FID_BUTT];

#if ((VOS_VXWORKS == VOS_OS_VER) || (VOS_NUCLEUS == VOS_OS_VER))
/* the map of PRI between FID and VxWorks or Nucleus */
VOS_UINT32 vos_FidTaskRealPriority[VOS_PRIORITY_NUM]
    = { 150, 146, 144, 142, 140, 138, 136, 134, 132, 130, 128, 126, 124, 122};
#endif

#if (VOS_WIN32 == VOS_OS_VER)
/* the map of PRI between FID and Win32 */
VOS_UINT32 vos_FidTaskRealPriority[VOS_PRIORITY_NUM]
    = { 32, 33, 64, 65, 96 ,97, 128, 129, 160, 161, 192, 193, 224, 225 };
#endif

#if (VOS_LINUX == VOS_OS_VER)
/* the map of PRI between FID and Linux */
VOS_UINT32 vos_FidTaskRealPriority[VOS_PRIORITY_NUM]
    = { 30, 32, 35, 38, 41 ,44, 47, 51, 55, 59, 63, 67, 71, 75 };
#endif

#if (VOS_RTOSCK == VOS_OS_VER)
/* the map of PRI between FID and VxWorks or Nucleus */
VOS_UINT32 vos_FidTaskRealPriority[VOS_PRIORITY_NUM]
    = { 48, 47, 46, 43, 42, 41, 40, 38, 36, 34, 33, 32, 30, 28};
#endif




/* default value of task stack size */
#define TASK_STACK_SIZE                      32768

/* which should be del when only one FID exists */
#define DEFAULT_FID_QUEUE_NUM                1

/* the base value of Fid's Init */
#define VOS_FID_INIT_ERR_BASE                0x80000000
#define VOS_PID_INIT_ERR_BASE                0x80000000

VOS_UINT32      g_ulOmFidInit = VOS_NULL ;
VOS_UINT32      g_ulOmPidInit = VOS_NULL ;

VOS_UINT16      g_usFidInitStep;
VOS_UINT16      g_usFidInitId;
VOS_UINT16      g_usPidInitStep;
VOS_UINT16      g_usPidInitId;

#if VOS_YES == VOS_ANALYZE_PID_MEM

/* 自旋锁，用来作PID 内存消耗统计临界资源保护 */
VOS_SPINLOCK             g_stVosPidMemAnaSpinLock;

#endif

extern VOS_UINT32 g_ulVosOutsideStep;

#ifndef DMT
/*****************************************************************************
 Function   : MOD_RegFidPidRSP
 Description: map the PID and FID
 Input      : ulPID -- PID
            : ulFID -- the belone of PID
            : priority -- PID's priority
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 MOD_RegFidPidRSP( VOS_UINT32 ulPID, VOS_UINT32 ulFID,
                             VOS_INT priority)
{
    if ( ulPID <= VOS_PID_DOPRAEND-1 )
    {
        return VOS_OK;
    }

    if( ulPID >= VOS_PID_BUTT )
    {
        VOS_SetErrorNo(VOS_ERRNO_REG_FID_PID_INVALID_PID);
        return(VOS_ERRNO_REG_FID_PID_INVALID_FID);
    }

    if( VOS_ID_PID_NULL != vos_PidRecords[ulPID-VOS_PID_DOPRAEND].Pid )
    {
        Print("# VOS_RegisterPIDInfo multiple register.\r\n");
    }

    if( priority > VOS_PRIORITY_P6 )
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGMPRI_INPUT2INVALID);
        return(VOS_ERRNO_FIDPID_REGMPRI_INPUT2INVALID);
    }

    vos_PidRecords[ulPID-VOS_PID_DOPRAEND].Pid = (int)ulPID;
    vos_PidRecords[ulPID-VOS_PID_DOPRAEND].Fid = (int)ulFID;
    vos_PidRecords[ulPID-VOS_PID_DOPRAEND].priority = priority;

    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_RegisterPIDInfo
 Description: record PID info
 Input      : ulPID -- PID
            : pfnInitFun -- Init FUN of the pid
            : pfnMsgFun -- process functin of the pid
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/

VOS_UINT32 VOS_RegisterPIDInfo( VOS_PID ulPID,
                                Init_Fun_Type pfnInitFun,
                                Msg_Fun_Type pfnMsgFun)
{
    if((ulPID >= VOS_PID_BUTT)||(ulPID < VOS_PID_DOPRAEND))
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGPIDI_INPUTPIDINVALID);
        LogPrint1("\r\nVOS_RegisterPIDInfo: Register Wrong PID %d", (VOS_INT)ulPID);
        return(VOS_ERRNO_FIDPID_REGPIDI_INPUTPIDINVALID);
    }

    if(pfnInitFun != VOS_NULL_PTR)
    {
        vos_PidRecords[ulPID-VOS_PID_DOPRAEND].InitFunc      = pfnInitFun;
    }

    if(pfnMsgFun == VOS_NULL_PTR)
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGPIDI_INPUTFUNCARENULL);
        return(VOS_ERRNO_FIDPID_REGPIDI_INPUTFUNCARENULL);
    }
    else
    {
        vos_PidRecords[ulPID-VOS_PID_DOPRAEND].MsgFunction   = pfnMsgFun;
    }

    return(VOS_OK);
}
#endif /* DMT */

/*****************************************************************************
 Function   : PidInitFuncDefault
 Description: the default Init function of pid
 Input      : InitPhrase -- the step of INit
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 PidInitFuncDefault ( enum VOS_INIT_PHASE_DEFINE InitPhrase )
{
    return( VOS_OK );
}

/*****************************************************************************
 Function   : MsgProcFuncDefault
 Description: the default process function of pid
 Input      : pMsg -- the processing message
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID MsgProcFuncDefault( MsgBlock * pMsg )
{
    return;
}

/*****************************************************************************
 Function   : VOS_PidCtrlBlkInit
 Description: Init the control block of PID
 Input      : void
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_PidCtrlBlkInit(VOS_VOID)
{
    int i;

    for(i=VOS_PID_DOPRAEND; i<VOS_PID_BUTT; i++)
    {
        vos_PidRecords[i-VOS_PID_DOPRAEND].Pid          = VOS_ID_PID_NULL;
        vos_PidRecords[i-VOS_PID_DOPRAEND].Fid          = VOS_ID_FID_NULL;

        vos_PidRecords[i-VOS_PID_DOPRAEND].InitFunc     = PidInitFuncDefault;
        vos_PidRecords[i-VOS_PID_DOPRAEND].MsgFunction  = MsgProcFuncDefault;
        vos_PidRecords[i-VOS_PID_DOPRAEND].priority     = VOS_PRIORITY_NULL;
#if VOS_YES == VOS_ANALYZE_PID_MEM
        vos_PidRecords[i-VOS_PID_DOPRAEND].ulMsgSize    = VOS_NULL;
        vos_PidRecords[i-VOS_PID_DOPRAEND].ulMemSize    = VOS_NULL;
        vos_PidRecords[i-VOS_PID_DOPRAEND].ulMsgPeakSize   = VOS_NULL;
        vos_PidRecords[i-VOS_PID_DOPRAEND].ulMemPeakSize   = VOS_NULL;
#endif
    }

    /* which comes from v_id.inc */
    REG_FID_PID_RSP();

#if VOS_YES == VOS_ANALYZE_PID_MEM
    VOS_SpinLockInit(&g_stVosPidMemAnaSpinLock);
#endif

    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_PidsInitOnePhase
 Description: one step of PID initialization
 Input      : InitPhrase -- the step of INit
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_PidsInitOnePhase( enum VOS_INIT_PHASE_DEFINE InitPhrase)
{
    int             i;
    VOS_UINT32      ulReturnValue, ulErrorCounter;

    ulErrorCounter  = 0;

    for(i=VOS_PID_DOPRAEND; i<VOS_PID_BUTT; i++)
    {
        g_usPidInitId = (VOS_UINT16)i;
        
        if( vos_PidRecords[i-VOS_PID_DOPRAEND].InitFunc == VOS_NULL_PTR)
        {
            continue;
        }

        ulReturnValue = vos_PidRecords[i-VOS_PID_DOPRAEND].InitFunc(InitPhrase);

        if(ulReturnValue != VOS_OK)
        {
            Print1("# VOS_PidsInitOnePhase Error. Pid 0x%x\r\n",i);
            g_ulOmPidInit |= (i & VOS_NULL_WORD);
            ulErrorCounter++;
        }
    }

    return(ulErrorCounter);
}

/*****************************************************************************
 Function   : VOS_PidsInit
 Description: Init PID
 Input      : void
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_PidsInit(VOS_VOID)
{
    VOS_UINT32 ulStartUpFailStage = VOS_PID_INIT_ERR_BASE;

    VOS_UINT32 ulInitPhase;

    for (ulInitPhase = VOS_IP_LOAD_CONFIG;
         ulInitPhase <= VOS_IP_BUTT; ulInitPhase++)
    {
        g_usPidInitStep = (VOS_UINT16)ulInitPhase;
        
        if ( VOS_OK
            != VOS_PidsInitOnePhase( (enum VOS_INIT_PHASE_DEFINE)ulInitPhase) )
        {
            Print1("# VOS_PidsInitOnePhase VOS_IP_LOAD_CONFIG Error. InitPhase: 0x%x\r\n",ulInitPhase);

            g_ulOmPidInit |= (ulInitPhase << 16);

            ulStartUpFailStage |= ulInitPhase;

            return(ulStartUpFailStage);
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : FidInitFuncDefault
 Description: default init function of FID
 Input      : one step of Init
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 FidInitFuncDefault ( enum VOS_INIT_PHASE_DEFINE InitPhrase )
{
    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_FidCtrlBlkInit
 Description: Init the control block of FID
 Input      : void
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_FidCtrlBlkInit(VOS_VOID)
{
    VOS_UINT32               i;
    int                      j;

    VOS_FID_CONTROL_BLOCK    *pFidCtrlBlk = vos_FidCtrlBlk;
    FID_Static_Table_Type    *pFidStaticTable;

    for(i=0; i<(VOS_UINT32)VOS_FID_BUTT; i++)
    {
        pFidCtrlBlk->Fid            = (VOS_INT)i;

        pFidCtrlBlk->InitFunc = FidInitFuncDefault;
        pFidCtrlBlk->TaskFunction = vos_FidTask;
        pFidCtrlBlk->Priority = vos_FidTaskRealPriority[VOS_PRIORITY_BASE];
        pFidCtrlBlk->StackSize  = TASK_STACK_SIZE;
        for ( j=0; j<VOS_MAX_SELF_TASK_OF_FID; j++ )
        {
            pFidCtrlBlk->SelfProcTaskFunc[j] = VOS_NULL_PTR;
            pFidCtrlBlk->SelfProcTaskTid[j] = VOS_NULL_DWORD;
        }
        /* which should be del when only one FID exists */
        pFidCtrlBlk->MAXQidSize = DEFAULT_FID_QUEUE_NUM;
        pFidCtrlBlk++;
    }

    for(i=(VOS_UINT32)VOS_FID_DOPRAEND; i<(VOS_UINT32)VOS_FID_BUTT; i++)
    {
        pFidCtrlBlk     = &vos_FidCtrlBlk[i];
        pFidStaticTable = &g_aFidStaticTable[i];

        if(pFidStaticTable->ulFID != (unsigned long)i)
        {
            Print2("# Fid Init Fid invalid ulFID %d I %d",
                (int)pFidStaticTable->ulFID, i );

            return(VOS_ERR);
        }

        for(j=0; j<VOS_ID_MAX_FID_NAME_LENGTH; j++)
        {
            pFidCtrlBlk->Name[j]    = (char)(pFidStaticTable->achFIDName[j]);
        }
        pFidCtrlBlk->Name[j-1]      = '\0';

        pFidCtrlBlk->InitFunc       = pFidStaticTable->pfnInitFun;
        pFidCtrlBlk->StartOrder     = pFidStaticTable->ulStartOrder;

        if( 0 != pFidStaticTable->ulStackSize )
        {
            pFidCtrlBlk->StackSize  = pFidStaticTable->ulStackSize;

        }
        else
        {
            pFidCtrlBlk->StackSize  = TASK_STACK_SIZE;
        }

        /* which should be del when only one FID exists */
        if( 0 != pFidStaticTable->ulQueueSize )
        {
            pFidCtrlBlk->MAXQidSize  = pFidStaticTable->ulQueueSize;

        }
    }

    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_FidsInitOnePhase
 Description: one step of FID initialization
 Input      : InitPhrase -- the step of INit
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_FidsInitOnePhase(enum VOS_INIT_PHASE_DEFINE InitPhrase)
{
    VOS_UINT32      i;
    VOS_UINT32      ulReturnValue, ulErrorCounter;

    ulErrorCounter  = 0;

    for(i=(VOS_UINT32)VOS_FID_DOPRAEND; i<(VOS_UINT32)VOS_FID_BUTT; i++)
    {
        g_usFidInitId = (VOS_UINT16)i;
        
        if(vos_FidCtrlBlk[i].InitFunc == VOS_NULL_PTR)
        {
            continue;
        }

        ulReturnValue = vos_FidCtrlBlk[i].InitFunc(InitPhrase);

        if(ulReturnValue != VOS_OK)
        {
            Print4("# %s InitPhrase %d i %d ulReturnValue %x",
                vos_FidCtrlBlk[i].Name, InitPhrase, i, ulReturnValue);

            g_ulOmFidInit |= (i & VOS_NULL_WORD);

            ulErrorCounter++;
        }
    }

    return(ulErrorCounter);
}

/*****************************************************************************
 Function   : VOS_FidsInit
 Description: Init PID
 Input      : void
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_FidsInit(VOS_VOID)
{
    VOS_UINT32 ulStartUpFailStage = VOS_FID_INIT_ERR_BASE;

    VOS_UINT32 ulInitPhase;

    for (ulInitPhase = VOS_IP_LOAD_CONFIG;
         ulInitPhase <= VOS_IP_BUTT; ulInitPhase++)
    {
        g_usFidInitStep = (VOS_UINT16)ulInitPhase;
        
        if (VOS_OK
            != VOS_FidsInitOnePhase( (enum VOS_INIT_PHASE_DEFINE)ulInitPhase) )
        {
            Print("# VOS_FidsInitOnePhase VOS_IP_LOAD_CONFIG Error.\r\n");

            g_ulOmFidInit |= (ulInitPhase << 16);

            ulStartUpFailStage |= ulInitPhase;

            return(ulStartUpFailStage);
        }
    }

    return VOS_OK;
}

#ifndef DMT
/*****************************************************************************
 Function   : VOS_RegisterMsgTaskPrio
 Description: Register priority of FID message handling task.
 Input      : ulFID    -- Function module Identifier
            : TaskPrio -- priority of normal message handling task
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_RegisterMsgTaskPrio( VOS_FID ulFID,
                                    enum VOS_PRIORITY_DEFINE TaskPrio )
{
    if(ulFID >= VOS_FID_BUTT)
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGMPRI_INPUT1INVALID);
        return(VOS_ERRNO_FIDPID_REGMPRI_INPUT1INVALID);
    }

    if(TaskPrio > VOS_PRIORITY_P6)
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGMPRI_INPUT2INVALID);
        return(VOS_ERRNO_FIDPID_REGMPRI_INPUT2INVALID);
    }

    vos_FidCtrlBlk[ulFID].Priority = vos_FidTaskRealPriority[TaskPrio];

    return(VOS_OK);
}
#endif /* DMT */

/*****************************************************************************
 Function   : VOS_RegisterTaskPrio
 Description: Register real priority of FID message handling task.
 Input      : ulFID    -- Function module Identifier
            : TaskPrio -- priority of normal message handling task
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_RegisterTaskPrio( VOS_FID ulFID, VOS_UINT32 ulTaskRealPri)
{
    if(ulFID >= VOS_FID_BUTT)
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGMPRI_INPUT1INVALID);
        return(VOS_ERRNO_FIDPID_REGMPRI_INPUT1INVALID);
    }

    vos_FidCtrlBlk[ulFID].Priority = ulTaskRealPri;

    return(VOS_OK);
}

/*****************************************************************************
 Function   : CreateFidsQueque
 Description: create queue of one FID
 Input      : void
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 CreateFidsQueque(VOS_VOID)
{
    VOS_UINT32          i;
    VOS_UINT32          ulReturnValue;

    for(i=(VOS_UINT32)VOS_FID_DOPRAEND; i<(VOS_UINT32)VOS_FID_BUTT; i++)
    {
        ulReturnValue   = VOS_FixedQueueCreate( VOS_FID_QUEUE_LENGTH
                                     , &(vos_FidCtrlBlk[i].Qid)
                                     , VOS_MSG_Q_FIFO
                                     , VOS_FID_MAX_MSG_LENGTH
                                     , vos_FidCtrlBlk[i].MAXQidSize );

        if( ulReturnValue != VOS_OK )
        {
            Print1("# create %d FID queue error.\r\n", i);

            return( ulReturnValue );
        }
    }

    return( VOS_OK );
}


VOS_MSG_HOOK_FUNC   vos_MsgHook = VOS_NULL_PTR;

/*****************************************************************************
 Function   : VOS_RegisterMsgGetHook
 Description: Register the hook for VOS_GetMsg
 Input      : pfnMsgHook -- Hook function
 Return     : Old function hook
 Other      : This hook would be called after getting message by system.
              If VOS_OK was returned by the hook function, the message
              processing would go on, otherwise the message would be
              thrown away and the corresponding handling function for this
              message would not be called.
 *****************************************************************************/
VOS_MSG_HOOK_FUNC VOS_RegisterMsgGetHook( VOS_MSG_HOOK_FUNC pfnMsgHook )
{
    VOS_MSG_HOOK_FUNC   oldMsgHook;

    oldMsgHook          = vos_MsgHook;
    vos_MsgHook         = pfnMsgHook;

    return(oldMsgHook);
}

/*****************************************************************************
 Function   : vos_FidTask
 Description: the process entry of every FID
 Input      : ulQueueID -- the queue of the FID
            : the ID of the FID
 Return     : void
 *****************************************************************************/
VOS_VOID vos_FidTask( VOS_UINT32 ulQueueID, VOS_UINT32 FID_value,
                      VOS_UINT32 Para1, VOS_UINT32 Para2 )
{

    MSG_CB                  *pMsg;
    VOS_UINT32              *pulMsgAddr;
    VOS_UINT32              iThePid;
    VOS_UINT_PTR            Msg_Address;

    for(;;)
    {
        if ( VOS_ERR
            == VOS_FixedQueueRead(ulQueueID, 0, &Msg_Address, VOS_FID_MAX_MSG_LENGTH))
        {
            LogPrint1("# FID fetch message error. the Q ID is %d.\r\n",
                (int)ulQueueID);

            continue;
        }

        Msg_Address += VOS_MSG_BLK_HEAD_LEN;

        pMsg = (MSG_CB *)Msg_Address;

        iThePid = pMsg->ulReceiverPid;

        if(iThePid >= VOS_PID_BUTT)
        {
            LogPrint("# vos_FidTask Pid too big.\r\n");

            pulMsgAddr = (VOS_UINT32 *)pMsg;
            LogPrint4("# Msg :S pid %08X R Pid %08X Length %08X Name %08X.\r\n",
                (VOS_INT)(*(pulMsgAddr + 1)), (VOS_INT)(*(pulMsgAddr + 3)),
                (VOS_INT)(*(pulMsgAddr + 4)), (VOS_INT)(*(pulMsgAddr + 5)));

            VOS_FreeMsg( iThePid, pMsg );

            continue;
        }

        if( FID_value == vos_PidRecords[iThePid-VOS_PID_DOPRAEND].Fid )
        {
            if ( VOS_NULL_PTR != vos_MsgHook )
            {
                (vos_MsgHook)(pMsg);
            }
            (vos_PidRecords[iThePid-VOS_PID_DOPRAEND].MsgFunction)(pMsg);
        }
        else
        {
            LogPrint("# vos_FidTask Pid not belong the Fid.\r\n");

            pulMsgAddr = (VOS_UINT32 *)pMsg;
            LogPrint4("# Msg :S pid %08X R Pid %08X Length %08X Name %08X.\r\n",
                (VOS_INT)(*(pulMsgAddr + 1)), (VOS_INT)(*(pulMsgAddr + 3)),
                (VOS_INT)(*(pulMsgAddr + 4)), (VOS_INT)(*(pulMsgAddr + 5)));
        }

        VOS_FreeMsg( iThePid, pMsg );
    }
}

/*****************************************************************************
 Function   : CreateFidsTask
 Description: create tasks of all FIDs
 Input      : void
            : void
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 CreateFidsTask(VOS_VOID)
{
    VOS_UINT32              i;
    int                     ubyIndex;
    VOS_UINT32              ulReturnValue;
    VOS_UINT32              Argument[VOS_TARG_NUM] = {0};
    VOS_CHAR                acSelfTaskName[VOS_MAX_LENGTH_TASK_NAME];

    Argument[3] = VOS_NULL_DWORD;

    for(i=VOS_FID_DOPRAEND; i<VOS_FID_BUTT; i++)
    {
        vos_FidCtrlBlk[i].TaskArgument[0] = vos_FidCtrlBlk[i].Qid;
        vos_FidCtrlBlk[i].TaskArgument[1] = i;
        vos_FidCtrlBlk[i].TaskArgument[3] = VOS_NULL_DWORD;

        ulReturnValue   = VOS_CreateTask( vos_FidCtrlBlk[i].Name
                                        , &(vos_FidCtrlBlk[i].Tid)
                                        , vos_FidCtrlBlk[i].TaskFunction
                                        , vos_FidCtrlBlk[i].Priority
                                        , vos_FidCtrlBlk[i].StackSize
                                        , vos_FidCtrlBlk[i].TaskArgument);
        if( ulReturnValue != VOS_OK )
        {
            Print("# VOS_CreateTask Fail in CreateFidsTask.\r\n");
            return( ulReturnValue );
        }

        VOS_RecordFIDTIDInfo( i, vos_FidCtrlBlk[i].Tid );

        for( ubyIndex = 0; ubyIndex < VOS_MAX_SELF_TASK_OF_FID; ubyIndex++ )
        {
            if( vos_FidCtrlBlk[i].SelfProcTaskFunc[ubyIndex] != VOS_NULL_PTR )
            {
                VOS_nsprintf(acSelfTaskName, VOS_MAX_LENGTH_TASK_NAME-1, "F%d_I%d",i,ubyIndex);

                ulReturnValue   = VOS_CreateTask( acSelfTaskName,
                            &(vos_FidCtrlBlk[i].SelfProcTaskTid[ubyIndex]),
                            vos_FidCtrlBlk[i].SelfProcTaskFunc[ubyIndex],
                            vos_FidCtrlBlk[i].SelfProcTaskPriority[ubyIndex],
                            vos_FidCtrlBlk[i].SelfProcTaskStack[ubyIndex],
                            Argument);
                if( ulReturnValue != VOS_OK )
                {
                    Print("# VOS_CreateSelfTask Fail in CreateFidsTask.\r\n");
                    return( ulReturnValue );
                }
            }
        }
    }

    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_RegisterSelfTask
 Description: Register a self-handling task
 Input      : ulFID       -- Function module Identifier
              pfnSelfTask -- entry of self-handling task
              TaskPrio    -- priority of self-handling task
 Return     : index of self-handling task of this FID(one FID can have several
              self-handling task) and VOS_NULL_BYTE indicates failure.
 *****************************************************************************/
VOS_UINT8 VOS_RegisterSelfTask( VOS_FID                   ulFID,
                                VOS_TASK_ENTRY_TYPE       pfnSelfTask,
                                enum VOS_PRIORITY_DEFINE  TaskPrio,
                                VOS_UINT32                ulStackSize)
{
    VOS_UINT8   ubyIndex;

    for( ubyIndex = 0; ubyIndex < VOS_MAX_SELF_TASK_OF_FID; ubyIndex++ )
    {
        if( ( VOS_NULL_PTR == vos_FidCtrlBlk[ulFID].SelfProcTaskFunc[ubyIndex] ) \
            && ( VOS_NULL_DWORD == vos_FidCtrlBlk[ulFID].SelfProcTaskTid[ubyIndex]))
        {
            break;
        }
    }

    if( ubyIndex >= VOS_MAX_SELF_TASK_OF_FID )
    {
        Print("# VOS_RegisterSelfTask error.\r\n");

        return VOS_NULL_BYTE;
    }

    vos_FidCtrlBlk[ulFID].SelfProcTaskFunc[ubyIndex]    = pfnSelfTask;
    vos_FidCtrlBlk[ulFID].SelfProcTaskPriority[ubyIndex]
        = vos_FidTaskRealPriority[TaskPrio];
    vos_FidCtrlBlk[ulFID].SelfProcTaskStack[ubyIndex] = ulStackSize;

    return ubyIndex;
}

/*****************************************************************************
 Function   : VOS_RegisterSelfTaskPrio
 Description: Register a self-handling task
 Input      : ulFID       -- Function module Identifier
              pfnSelfTask -- entry of self-handling task
              TaskPrio    -- real priority of self-handling task
 Return     : index of self-handling task of this FID(one FID can have several
              self-handling task) and VOS_NULL_BYTE indicates failure.
 *****************************************************************************/
VOS_UINT8 VOS_RegisterSelfTaskPrio( VOS_FID             ulFID ,
                                    VOS_TASK_ENTRY_TYPE       pfnSelfTask,
                                    VOS_UINT32                ulTaskRealPri,
                                    VOS_UINT32                ulStackSize)
{
    VOS_UINT8   ubyIndex;

    for( ubyIndex = 0; ubyIndex < VOS_MAX_SELF_TASK_OF_FID; ubyIndex++ )
    {
        if( ( VOS_NULL_PTR == vos_FidCtrlBlk[ulFID].SelfProcTaskFunc[ubyIndex] ) \
            && ( VOS_NULL_DWORD == vos_FidCtrlBlk[ulFID].SelfProcTaskTid[ubyIndex]))
        {
            break;
        }
    }

    if( ubyIndex >= VOS_MAX_SELF_TASK_OF_FID )
    {
        Print("# VOS_RegisterSelfTask error.\r\n");

        return VOS_NULL_BYTE;
    }

    vos_FidCtrlBlk[ulFID].SelfProcTaskFunc[ubyIndex]    = pfnSelfTask;
    vos_FidCtrlBlk[ulFID].SelfProcTaskPriority[ubyIndex] = ulTaskRealPri;
    vos_FidCtrlBlk[ulFID].SelfProcTaskStack[ubyIndex] = ulStackSize;

    return ubyIndex;
}

/*****************************************************************************
 Function   : VOS_SuspendFidTask
 Description: Suspend task of FID
 Input      : ulFID       -- Function module Identifiers
            : void
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_SuspendFidTask(VOS_FID ulFid)
{
    if ((VOS_FID_DOPRAEND > ulFid) || (VOS_FID_BUTT <= ulFid))
    {
        return VOS_ERR;
    }

    return VOS_SuspendTask( vos_FidCtrlBlk[ulFid].Tid );
}

/*****************************************************************************
 Function   : VOS_ResumeFidTask
 Description: Resume task of FID
 Input      : ulFID       -- Function module Identifiers
            : void
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_ResumeFidTask(VOS_FID ulFid)
{
    if ((VOS_FID_DOPRAEND > ulFid) || (VOS_FID_BUTT <= ulFid))
    {
        return VOS_ERR;
    }

    return VOS_ResumeTask( vos_FidCtrlBlk[ulFid].Tid );
}

/*****************************************************************************
 Function   : VOS_SuspendFidsTask
 Description: Suspend tasks of all FIDs
 Input      : void
            : void
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_SuspendFidsTask(VOS_VOID)
{
    VOS_UINT32              i;
    int                     ubyIndex;
    VOS_UINT32              ulReturnValue;

    for(i=VOS_FID_DOPRAEND; i<VOS_FID_BUTT; i++)
    {
        ulReturnValue = VOS_SuspendTask( vos_FidCtrlBlk[i].Tid );
        if( ulReturnValue != VOS_OK )
        {
            Print("# SuspendFidsTask Fail 1 in SuspendFidsTask.\r\n");
            return( ulReturnValue );
        }

        for( ubyIndex = 0; ubyIndex < VOS_MAX_SELF_TASK_OF_FID; ubyIndex++ )
        {
            if( vos_FidCtrlBlk[i].SelfProcTaskFunc[ubyIndex] != VOS_NULL_PTR )
            {
                ulReturnValue
                    = VOS_SuspendTask( vos_FidCtrlBlk[i].SelfProcTaskTid[ubyIndex]);
                if( ulReturnValue != VOS_OK )
                {
                    Print("# SuspendFidsTask Fail 2 in SuspendFidsTask.\r\n");

                    return( ulReturnValue );
                }
            }
        }
    }

    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_ResumeFidsTask
 Description: Resume tasks of all FIDs
 Input      : void
            : void
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_ResumeFidsTask(VOS_VOID)
{
    VOS_UINT32              i;
    int                     ubyIndex;
    VOS_UINT32              ulReturnValue;

    for(i=VOS_FID_DOPRAEND; i<VOS_FID_BUTT; i++)
    {
        ulReturnValue = VOS_ResumeTask( vos_FidCtrlBlk[i].Tid );
        if( ulReturnValue != VOS_OK )
        {
            Print("# ResumeFidsTask Fail 1 in ResumeFidsTask.\r\n");
            return( ulReturnValue );
        }

        for( ubyIndex = 0; ubyIndex < VOS_MAX_SELF_TASK_OF_FID; ubyIndex++ )
        {
            if( vos_FidCtrlBlk[i].SelfProcTaskFunc[ubyIndex] != VOS_NULL_PTR )
            {
                ulReturnValue
                    = VOS_ResumeTask( vos_FidCtrlBlk[i].SelfProcTaskTid[ubyIndex]);
                if( ulReturnValue != VOS_OK )
                {
                    Print("# ResumeFidsTask Fail 2 in ResumeFidsTask.\r\n");

                    return( ulReturnValue );
                }
            }
        }
    }

    return( VOS_OK );
}


/*****************************************************************************
 Function   : VOS_SuspendAllTask
 Description: suspend all FID & selftask
 Input      : invaild
 Return     : void
 Other      : only for designer
 *****************************************************************************/
VOS_VOID VOS_SuspendAllTask( VOS_UINT32 Para0, VOS_UINT32 Para1,
                             VOS_UINT32 Para2, VOS_UINT32 Para3 )
{
    if ( VOS_OK != VOS_SuspendFidsTask() )
    {
        Print("# SUSPED FID error.\r\n");
    }

    if ( VOS_OK != VOS_SuspendTask( vos_TimerTaskId ) )
    {
        Print("# SUSPED VOS timer task error.\r\n");
    }

    if ( VOS_OK != VOS_SuspendTask( RTC_TimerTaskId ) )
    {
        Print("# SUSPED RTC timer task error.\r\n");
    }
}

/*****************************************************************************
 Function   : VOS_RegisterMsgTaskEntry
 Description: Register message handling task's entry of FID
 Input      : ulFID      -- Function module Identifier
              pfnMsgTask -- message handling task's entry
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_RegisterMsgTaskEntry( VOS_FID ulFID, VOS_VOIDFUNCPTR pfnMsgTask )
{
    if( ulFID >= VOS_FID_BUTT )
    {
        LogPrint("VOS_RegisterMsgTaskEntry FID invaild.\r\n");
        return VOS_ERR;
    }

    if( VOS_NULL_PTR == pfnMsgTask )
    {
        LogPrint("VOS_RegisterMsgTaskEntry task null.\r\n");
        return VOS_ERR;
    }

    vos_FidCtrlBlk[ulFID].TaskFunction = (VOS_TASK_ENTRY_TYPE)pfnMsgTask;

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_GetQueueIDFromFid
 Description: get the queue ID of a FId
 Input      : ulFID      -- Function module Identifier

 Return     : the queue ID
 *****************************************************************************/
VOS_UINT32 VOS_GetQueueIDFromFid(VOS_UINT32 ulFid)
{
    if( ulFid >= VOS_FID_BUTT )
    {
        LogPrint("VOS_GetQueueIDFromFid FID invaild.\r\n");
        return 0xffffffff;
    }

    return vos_FidCtrlBlk[ulFid].Qid;
}

/*****************************************************************************
 Function   : VOS_ShowFidsQueueInfo
 Description: print used msg's useage info
 Input      : void
 Return     : void
 Other      : only for designer
 *****************************************************************************/
VOS_VOID VOS_ShowFidsQueueInfo(VOS_UINT32 Para0, VOS_UINT32 Para1,
                                VOS_UINT32 Para2, VOS_UINT32 Para3 )
{
    VOS_UINT32              i;
    VOS_UINT32              *pulMsgAddr;
    VOS_UINT32              ulCount;

    for( i=VOS_FID_DOPRAEND; i<VOS_FID_BUTT; i++ )
    {
        Print2("# FID %d %s queue info.\r\n", i, vos_FidCtrlBlk[i].Name);

        ulCount = 0;

        pulMsgAddr = (VOS_UINT32 *)VOS_OutMsg(vos_FidCtrlBlk[i].Qid );

        while ( VOS_NULL_PTR != pulMsgAddr )
        {
            ulCount++;

            if ( ulCount > VOS_FID_QUEUE_LENGTH )
            {
                LogPrint("# VOS_ShowFidsQueueInfo error.\r\n");
            }

            Print4("# Msg :S pid %08X R Pid %08X Length %08X Name %08X.\r\n",
                    *(pulMsgAddr + 1), *(pulMsgAddr + 3),
                    *(pulMsgAddr + 4), *(pulMsgAddr + 5));

            pulMsgAddr = (VOS_UINT32 *)VOS_OutMsg(vos_FidCtrlBlk[i].Qid );
        }
    }

    return;
}

#if VOS_YES == VOS_ANALYZE_PID_MEM
/*****************************************************************************
 Function   : VOS_IncreasePidMemory
 Description: Increase Pid Memory
 Input      : pid size
 Return     : void
 Other      : only for designer
 *****************************************************************************/
VOS_VOID VOS_IncreasePidMemory(VOS_PID ulPid, VOS_UINT32 ulSize, VOS_UINT32 ulType)
{
    VOS_ULONG                   ulLockLevel;

    /*intLockLevel = VOS_SplIMP();*/
    VOS_SpinLockIntLock(&g_stVosPidMemAnaSpinLock, ulLockLevel);

    if ( VOS_LOCATION_MEM == ulType )
    {
        vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMemSize += ulSize;

        if ( vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMemSize > vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMemPeakSize )
        {
            vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMemPeakSize = vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMemSize;
        }
    }
    else
    {
        vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMsgSize += ulSize;

        if ( vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMsgSize > vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMsgPeakSize )
        {
            vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMsgPeakSize = vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMsgSize;
        }
    }

    /*VOS_Splx(intLockLevel);*/
    VOS_SpinUnlockIntUnlock(&g_stVosPidMemAnaSpinLock, ulLockLevel);

    return;
}

/*****************************************************************************
 Function   : VOS_DecreasePidMemory
 Description: Decrease Pid Memory
 Input      : pid size
 Return     : void
 Other      : only for designer
 *****************************************************************************/
VOS_VOID VOS_DecreasePidMemory(VOS_PID ulPid, VOS_UINT32 ulSize, VOS_UINT32 ulType)
{
    VOS_ULONG                   ulLockLevel;

    /*intLockLevel = VOS_SplIMP();*/
    VOS_SpinLockIntLock(&g_stVosPidMemAnaSpinLock, ulLockLevel);

    if ( VOS_LOCATION_MEM == ulType )
    {
        vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMemSize -= ulSize;
    }
    else
    {
        vos_PidRecords[ulPid-VOS_PID_DOPRAEND].ulMsgSize -= ulSize;
    }

    /*VOS_Splx(intLockLevel);*/
    VOS_SpinUnlockIntUnlock(&g_stVosPidMemAnaSpinLock, ulLockLevel);

    return;
}
#endif

/*****************************************************************************
 Function   : VOS_AnalyzePidMemory
 Description: print used msg's useage info
 Input      : void
 Return     : void
 Other      : only for designer
 *****************************************************************************/
VOS_UINT32 VOS_AnalyzePidMemory(VOS_VOID *pBUffer, VOS_UINT32 ulSize, VOS_UINT32 *pulNum)
{
#if VOS_YES == VOS_ANALYZE_PID_MEM
    VOS_UINT32             ulTempSize;
    VOS_PID_PEAK_MEM_STRU  *pstTempPtr ;
    VOS_INT                i;

    if ( (VOS_NULL_PTR == pBUffer) || (VOS_NULL_PTR == pulNum) )
    {
        return VOS_ERR;
    }

    ulTempSize = (VOS_PID_BUTT - VOS_PID_DOPRAEND);

    *pulNum = ulTempSize;

    ulTempSize *= sizeof(VOS_PID_PEAK_MEM_STRU);/* 4byte msg, 4byte mem */

    if ( ulTempSize > ulSize )
    {
        return VOS_ERR;
    }

    pstTempPtr = (VOS_PID_PEAK_MEM_STRU *)pBUffer;

    for(i=VOS_PID_DOPRAEND; i<VOS_PID_BUTT; i++)
    {
        pstTempPtr->ulPid         = vos_PidRecords[i-VOS_PID_DOPRAEND].Pid;
        pstTempPtr->ulMsgPeakSize = vos_PidRecords[i-VOS_PID_DOPRAEND].ulMsgPeakSize;
        pstTempPtr->ulMemPeakSize = vos_PidRecords[i-VOS_PID_DOPRAEND].ulMemPeakSize;

        pstTempPtr++;
    }

    return VOS_OK;
#else
    return VOS_ERR;
#endif
}

/*****************************************************************************
 Function   : VOS_GetTidFromPid
 Description: Get Tid From Pid
 Input      : Pid
 Return     :
 Other      : only for designer
 *****************************************************************************/
VOS_UINT32 VOS_GetTCBFromPid(VOS_UINT32 ulPid)
{
    VOS_UINT32         ulFid;

    if((ulPid < VOS_PID_DOPRAEND)||(ulPid >= VOS_PID_BUTT))
    {
        Print1("VOS_GetTCBFromPid: Wrong Pid %d\r\n", ulPid);

        return VOS_NULL;
    }

    ulFid = (VOS_UINT32)(vos_PidRecords[ulPid-VOS_PID_DOPRAEND].Fid);

    if ((ulFid < (VOS_UINT32)VOS_FID_DOPRAEND) || (ulFid >= (VOS_UINT32)VOS_FID_BUTT))
    {
        Print2("VOS_GetTCBFromPid Wrong PID %d. FID %d\r\n", (VOS_INT)ulPid, (VOS_INT)ulFid);

        return VOS_NULL;
    }

    return (VOS_UINT32)VOS_GetTCBFromTID(vos_FidCtrlBlk[ulFid].Tid);

}

/*****************************************************************************
 Function   : VOS_GetModemIDFromPid
 Description: Get Modem ID From Pid
 Input      : Pid
 Return     : Modem ID
 Other      : 
 *****************************************************************************/
MODEM_ID_ENUM_UINT16 VOS_GetModemIDFromPid(VOS_UINT32 ulPid)
{
    if((ulPid < PC_PID_TOOL)&&(ulPid > VOS_PID_CPU_ID_0_DOPRAEND))
    {
        return MODEM_ID_1;
    }

    if((ulPid > PC_PID_TOOL)&&(ulPid < VOS_CPU_ID_0_PID_BUTT))
    {
        return MODEM_ID_0;
    }

    return MODEM_ID_BUTT;
}

/*****************************************************************************
 Function   : VOS_CheckPSPidValidity
 Description: check Protocol Stack's Pid validity
 Input      : Pid
 Return     : VOS_TRUE/VOS_FALSE
 Other      : 
 *****************************************************************************/
VOS_BOOL VOS_CheckPSPidValidity(VOS_UINT32 ulPid)
{
    if (((VOS_CPU_ID_0_PID_RESERVED < ulPid)&&(ulPid < VOS_CPU_ID_0_PID_BUTT))
        ||((VOS_CPU_ID_1_PID_RESERVED < ulPid)&&(ulPid < VOS_CPU_ID_1_PID_BUTT)))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}

/*****************************************************************************
 Function   : VOS_OsaGlobalShow
 Description: global variable show
 Input      : none
 Return     : none
 Other      :
 *****************************************************************************/
VOS_VOID VOS_OsaGlobalShow(VOS_VOID)
{
    vos_printf("VOS_OsaGlobalShow:g_ulOmFidInit = %d\n", g_ulOmFidInit);
    vos_printf("VOS_OsaGlobalShow:g_ulOmPidInit = %d\n", g_ulOmPidInit);
    vos_printf("VOS_OsaGlobalShow:g_usFidInitStep = %d\n", g_usFidInitStep);
    vos_printf("VOS_OsaGlobalShow:g_usFidInitId = %d\n", g_usFidInitId);
    vos_printf("VOS_OsaGlobalShow:g_usPidInitStep = %d\n", g_usPidInitStep);
    vos_printf("VOS_OsaGlobalShow:g_ulVosOutsideStep = %d\n", g_ulVosOutsideStep);
    vos_printf("VOS_OsaGlobalShow:g_usPidInitId = %d\n", g_usPidInitId);

    return;
}

/*****************************************************************************
 Function   : VOS_GetFIdRelPri
 Description: get the real priority of a task
 Input      : ulFidPri
              pulTaskRealPri
 Return     : VOS_OK/VOS_ERR
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_GetFIdRelPri(enum VOS_PRIORITY_DEFINE ulFidPri, VOS_UINT32 *pulTaskRealPri)
{
    if (ulFidPri > VOS_PRIORITY_P6)
    {
        return VOS_ERR;
    }

    if (VOS_NULL_PTR == pulTaskRealPri)
    {
        return VOS_ERR;
    }

    *pulTaskRealPri = vos_FidTaskRealPriority[ulFidPri];

    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

