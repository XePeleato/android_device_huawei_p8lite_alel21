/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_queue.h                                                       */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement queue                                              */
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

#ifndef _VOS_QUEUE_H
#define _VOS_QUEUE_H

#include "v_typdef.h"
#include "v_msg.h"
#include "v_sem.h"
#include "vos_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/* errno definiens */
#define VOS_ERRNO_QUEUE_CREATE_LENISZERO                    0x20050005
#define VOS_ERRNO_QUEUE_CREATE_SIZEISZERO                   0x20050006
#define VOS_ERRNO_QUEUE_CREATE_OPTINVALID                   0x20050007
#define VOS_ERRNO_QUEUE_CREATE_NAMEISNULL                   0x20050008
#define VOS_ERRNO_QUEUE_CREATE_OUTPUTISNULL                 0x20050009
#define VOS_ERRNO_QUEUE_CREATE_NOFREECB                     0x2005000A
#define VOS_ERRNO_QUEUE_CREATE_OSALFAIL                     0x2005000B
#define VOS_ERRNO_QUEUE_WRITE_QUEIDINVALID                  0x20050028
#define VOS_ERRNO_QUEUE_WRITE_BUFADDRISNULL                 0x20050029
#define VOS_ERRNO_QUEUE_WRITE_BUFSIZEINVALID                0x2005002A
#define VOS_ERRNO_QUEUE_FULL                                0x2005FFFF


#define VOS_DEFAULT_QUEUE_SIZE                              50

typedef enum tagVosQueueMode_E
{
    VOS_MSG_Q_FIFO,
    VOS_MSG_Q_PRIORITY,
    VOS_QUEUE_MODE_BUTT
} VOS_QUEUE_MODE;

/* the Max queue number supported by VOS */
#if (VOS_WIN32 == VOS_OS_VER)
#define VOS_MAX_QUEUE_NUMBER                               100
#else
#define VOS_MAX_QUEUE_NUMBER                               80
#endif

VOS_VOID VOS_QueueCtrlBlkInit(VOS_VOID);

VOS_UINT32 VOS_FixedQueueCreate( VOS_UINT32 ulLength,
                            VOS_UINT32 *pulQueueID,
                            VOS_UINT32 ulQueueOption,
                            VOS_UINT32 ulMaxMsgSize,
                            VOS_UINT32 ulQueueNum );

VOS_UINT32 VOS_FixedQueueWrite( VOS_UINT32 ulQueueID, VOS_VOID * pBufferAddr,
                           VOS_UINT32 ulBufferSize, VOS_UINT32 Pri,
                           VOS_UINT32 ulPid );

VOS_UINT32 VOS_FixedQueueWriteDirect( VOS_UINT32 ulQueueID, VOS_VOID * pBufferAddr, VOS_UINT32 Pri);

VOS_UINT32 VOS_FixedQueueRead( VOS_UINT32 ulQueueID, VOS_UINT32 ulTimeOut,
                          VOS_UINT_PTR *pBufferAddr, VOS_UINT32 ulBufferSize );

VOS_VOID* VOS_FixedQueueReadMsg( VOS_UINT32 ulFidID );

VOS_SEM VOS_GetSemIDFromQueue(VOS_UINT32 ulQueue);

VOS_VOID* VOS_GetMsg( VOS_UINT32 ulTaskID );

VOS_VOID* VOS_OutMsg( VOS_UINT32 ulQueueID );

VOS_UINT32 VOS_GetQueueSizeFromFid(VOS_UINT32 ulFid);

VOS_VOID VOS_QueuePrintFull( VOS_UINT32 ulQueue, VOS_CHAR *pcBuf, VOS_UINT32 ulLen);

VOS_UINT32 VOS_CheckTaskQueue(VOS_UINT32 ulPid,VOS_UINT32 ulEntries);

VOS_VOID VOS_ClearQueueInfoByFid(VOS_UINT32 ulFid);



#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_QUEUE_H */
