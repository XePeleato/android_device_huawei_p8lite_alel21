

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*lint -e767*/
#define THIS_FILE_ID                     PS_FILE_ID_IMM_MNTN_C
/*lint +e767*/

#include "ImmInterface.h"
#if (FEATURE_ON == FEATURE_SKB_EXP)


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "IMMmemMntn.h"
#include "om.h"
#include "OmApp.h"
#include "IMMmemRB.h"
#include "skbuff.h"
#include "PsCommonDef.h"
#include <asm/atomic.h>

const VOS_UINT8             g_ulImmBlkMemCheck = 1;             /* 控制是否跟踪IMM BLK MEM */


#define IMM_BLK_MEM_DEBUG_SWITCH_ON     (1 == g_ulImmBlkMemCheck)

/* 内存检查事件当前时戳 */
VOS_UINT32                              g_ulCurrentTimeSlice = 0;

/* 内存块占用时间超时门限 1s */
const VOS_UINT32                        g_ulOverTimeSliceThreshold = 32768*1;

/* 内存泄露事件上报次数监控全局变量 */
atomic_t                                g_stIMMBlkMemAlertEvtCnt = ATOMIC_INIT(0);

/* 内存申请失败事件上报次数监控全局变量 */
atomic_t                                g_stIMMBlkMemAlocFailEvtCnt = ATOMIC_INIT(0);

/* IMM 内存池可维可测各事件上报的最多的内存块数 */
const VOS_UINT16                        EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG = 41;

/* IMM 每次发生内存上报事件时，每次所能内存事件次数门限 */
VOS_UINT32                              g_ulImmEventRptCntThresholdPerTime = 1;

VOS_UINT8                               g_ucPrintDetail = IMM_FALSE;

atomic_t                                g_stCheckLeakFlag = ATOMIC_INIT(0);

extern VOS_UINT32                       g_ulImmMemRbFreeSem;

/*****************************************************************************
  3 函数实现
*****************************************************************************/



VOS_UINT8 IMM_MntnCheckReportMemInfoStatus( IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8 enTrigType )
{

    /*
    下面三种情况上报事件
    1、检查出内存泄露:IMM_MEM_TRIG_TYPE_LEAK
    2、第一次发生内存申请失败:IMM_MEM_TRIG_TYPE_ALLOC_FAIL
    3、第一次发生内存不足告警:IMM_MEM_TRIG_TYPE_ALERT
    */

    if ( IMM_MEM_TRIG_TYPE_LEAK == enTrigType )
    {
        return IMM_TRUE;
    }
    else if( IMM_MEM_TRIG_TYPE_ALLOC_FAIL == enTrigType )
    {
        if ( 0 == atomic_read(&g_stIMMBlkMemAlocFailEvtCnt) )
        {
            atomic_inc(&g_stIMMBlkMemAlocFailEvtCnt);
        }
    }
    else if( IMM_MEM_TRIG_TYPE_ALERT == enTrigType )
    {
        if ( 0 == atomic_read(&g_stIMMBlkMemAlertEvtCnt) )
        {
            atomic_inc(&g_stIMMBlkMemAlertEvtCnt);
        }
    }
    else
    {
        return IMM_FALSE;
    }

    return IMM_TRUE;

}



VOS_VOID IMM_MntnPrintBlkMemPoolUsedInfo( IMM_MEM_POOL_STRU  *pstImmMemPoolInfo )
{
#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
    VOS_UINT16                          usImmMemNum;
    IMM_BLK_MEM_DEBUG_STRU             *pstImmMemDebugInfo;
#endif
    IMM_MEM_STRU                       *pMem;


    if (VOS_NULL_PTR == pstImmMemPoolInfo)
    {
       IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_WARNING, "pstImmMemPoolInfo is NULL ! \n");
       return ;
    }


    if (IMM_TRUE != pstImmMemPoolInfo->ucUsedFlag)
    {
       IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_WARNING, "POOL not uesed ! \n");

       return ;
    }

    pMem = pstImmMemPoolInfo->pstImmMemStStartAddr;

    IMM_LOG2(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "Pool ID %u Blk Mem used %u  \n", pMem->enPoolId, pstImmMemPoolInfo->usImmMemUsedCnt);

    if( IMM_FALSE == g_ucPrintDetail )
    {
        return;
    }

#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
    for (usImmMemNum = 0;
            usImmMemNum < pstImmMemPoolInfo->usImmMemTotalCnt;
            usImmMemNum ++)
    {
        pMem = pstImmMemPoolInfo->pstImmMemStStartAddr + usImmMemNum;
        pstImmMemDebugInfo = &pMem->stDbgInfo;

        if ( MEM_BLK_STATE_FREE != pstImmMemDebugInfo->enMemStateFlag)
        {
            IMM_LOG4(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "Alloc: File %4d L %5d T %11u, Blk Mem Level: %4u, ",
                (VOS_INT32)pstImmMemDebugInfo->usAllocFileID,
                (VOS_INT32)pstImmMemDebugInfo->usAllocLineNum,
                (VOS_INT32)pstImmMemDebugInfo->ulAllocTick,
                usImmMemNum);
/*lint -e713*/
            IMM_LOG4(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "Status: %d, Status: %d, TraceFile:%4d, TraceLine:%5d, TraceTick:%11d, \n",
                pstImmMemDebugInfo->enMemStateFlag,
                (VOS_INT32)pstImmMemDebugInfo->usTraceFileID,
                (VOS_INT32)pstImmMemDebugInfo->usTraceLineNum,
                (VOS_INT32)pstImmMemDebugInfo->ulTraceTick);
/*lint +e713*/

            /* 打印PDU内容 */
            IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "Data ptr: 0x%x\n ", (VOS_INT32)pMem->pstMemBlk);
        }
    }
#endif

}/* IMM_MntnPrintBlkMemPoolUsedInfo */



VOS_VOID IMM_MntnShowUsedBlkMemInfo(VOS_VOID)
{
    IMM_MEM_POOL_STRU                  *pstImmMemPoolInfo;
    VOS_UINT8                           ucPoolId;


    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_ShowUsedBlkMemInfo:Begin.\r\n");

    for (ucPoolId = 0; ucPoolId < IMM_MEM_POOL_ID_BUTT; ucPoolId++)
    {
        pstImmMemPoolInfo = IMM_MemPoolGet(ucPoolId);

        IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_ShowUsedBlkMemInfo print pool %d info begin: \n", ucPoolId );
        IMM_MntnPrintBlkMemPoolUsedInfo(pstImmMemPoolInfo);
        IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_ShowUsedBlkMemInfo print pool %d info end! \n", ucPoolId);
    }

    return ;
}/*IMM_MntnShowUsedBlkMemInfo */

VOS_VOID IMM_MntnMemUseBlkCntRpt
(
    VOS_UINT8                               ucPoolId,
    IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8  enTrigType
)
{
    VOS_UINT32                              ulTotalCnt;
    VOS_UINT32                              ulLength;
    VOS_UINT32                              ulDataLen;
    PS_OM_EVENT_IND_STRU                   *pstImmOmEventInd;
    IMM_MEM_POOL_STRU                      *pstPoolInfo;
    IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU    *pstImmOmEventBlkMemUsedInfo;


    pstPoolInfo     = IMM_MEM_GET_POOL(ucPoolId);

    ulDataLen = offsetof(IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo);

    /*消息总长度*/
    ulLength  = offsetof(PS_OM_EVENT_IND_STRU, aucData) + ulDataLen;

    /*申请消息内存*/
    pstImmOmEventInd    = (PS_OM_EVENT_IND_STRU *)PS_MEM_ALLOC(UEPS_PID_IMM_RB_FREE, ulLength);

    if (VOS_NULL_PTR == pstImmOmEventInd )
    {
        IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_BlkMemUsedInfoEventRpt: MemAlloc Fail!\n");
        return;
    }

    ulTotalCnt      = pstPoolInfo->usImmMemUsedCnt;

    /*填写消息内容*/
    pstImmOmEventInd->ulLength      = ulLength - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
    pstImmOmEventInd->usEventId     = IMM_EVENT_BLK_MEM_USED_INFO;
    pstImmOmEventInd->ulModuleId    = UEPS_PID_IMM_RB_FREE;

    pstImmOmEventBlkMemUsedInfo     = (IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU *)pstImmOmEventInd->aucData;
    pstImmOmEventBlkMemUsedInfo->ucPoolId       = ucPoolId;
    pstImmOmEventBlkMemUsedInfo->enTrigType     = enTrigType;
    pstImmOmEventBlkMemUsedInfo->ucNo           = 1;
    pstImmOmEventBlkMemUsedInfo->usRptCnt       = 0;
    pstImmOmEventBlkMemUsedInfo->ulTotalUsedCnt = ulTotalCnt;

    /*调用OM的事件上报接口*/
    if ( PS_SUCC != OM_AcpuEvent((PS_OM_EVENT_IND_STRU*)pstImmOmEventInd))
    {
        IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_BlkMemUsedInfoEventRpt: OM Send Event Fail! \n");
    }
    /*释放申请的内存*/
    PS_MEM_FREE(UEPS_PID_IMM_RB_FREE, pstImmOmEventInd);

    return;
}/* IMM_MntnMemUseBlkCntRpt */



VOS_UINT32 IMM_MntnTimeDiff(VOS_UINT32 ulTimeStart, VOS_UINT32 ulTimeEnd)
{
    VOS_UINT32                          ulTimeDiff;


    if (ulTimeEnd > ulTimeStart)
    {
        ulTimeDiff = ulTimeEnd - ulTimeStart;
    }
    else
    {
        /* 如果起始时间大于结束时间，说明发生翻转，只认为发生翻转一次 */
        ulTimeDiff = PS_NULL_UINT32 + ulTimeEnd - ulTimeStart + 1;
    }

    return ulTimeDiff;
} /* IMM_MntnTimeDiff */


#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)

VOS_UINT32 IMM_MntnGetOverMemBlkCnt( VOS_UINT8 ucPoolId )
{
    IMM_MEM_POOL_STRU                  *pstImmMemPool;
    IMM_BLK_MEM_DEBUG_STRU             *pstImmMemDebugInfo;
    IMM_MEM_STRU                       *pstMem;
    VOS_UINT32                          ulBlkMemNum;
    VOS_UINT32                          ulTotolCnt = 0;


    /* 获取当前CPU Slice */
    g_ulCurrentTimeSlice = OM_GetSlice();
    pstImmMemPool        = IMM_MEM_GET_POOL(ucPoolId);

    /* 遍历内存池每块内存 */
    for (ulBlkMemNum = 0; ulBlkMemNum < pstImmMemPool->usImmMemTotalCnt; ++ulBlkMemNum )
    {
        pstMem = pstImmMemPool->pstImmMemStStartAddr + ulBlkMemNum;
        pstImmMemDebugInfo = &pstMem->stDbgInfo;

        /* 内存块状态检查 */
        if ( MEM_BLK_STATE_FREE == pstImmMemDebugInfo->enMemStateFlag )
        {
            continue;
        }

        /* 判断内存申请时戳与当前时戳之差是否超过门限 */
        if ( g_ulOverTimeSliceThreshold <= IMM_MntnTimeDiff(pstImmMemDebugInfo->ulAllocTick, g_ulCurrentTimeSlice))
        {
            ++ ulTotolCnt;
        }
    }

    return ulTotolCnt;
}/* IMM_MntnGetOverMemBlkCnt */

VOS_VOID IMM_MntnMemTraceInfoRptSaveMemData
(
    VOS_UINT8  ucPoolId,
    IMM_BLK_MEM_TRACE_INFO_STRU *pstBlkMemTraceInfo,
    IMM_MEM_STRU *pMem
)
{

    if ( ucPoolId != pMem->enPoolId )
    {
        IMM_LOG2(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_MntnMemTraceInfoRptSaveMemData, ucPoolId %d, pMem->enPoolId %d Mismatch! \n", ucPoolId, pMem->enPoolId);
        return;
    }

    if ( IMM_MEM_POOL_ID_SHARE == ucPoolId )
    {
        if ( NULL != pMem->pstMemBlk )
        {
            pstBlkMemTraceInfo->aucMemData[0]   = pMem->pstMemBlk[0];
            pstBlkMemTraceInfo->aucMemData[1]   = pMem->pstMemBlk[1];
            pstBlkMemTraceInfo->aucMemData[2]   = pMem->pstMemBlk[2];
            pstBlkMemTraceInfo->aucMemData[3]   = pMem->pstMemBlk[3];
        }
        else
        {
            VOS_MemSet(pstBlkMemTraceInfo->aucMemData, 0, 4);
            IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_MntnMemTraceInfoRptSaveMemData, ucPoolId %d Mem Ptr is Null! \n", ucPoolId);
        }

        return;
    }

    if ( IMM_MEM_POOL_ID_EXT == ucPoolId )
    {
        *((VOS_UINT32 *)pstBlkMemTraceInfo->aucMemData) = (VOS_UINT32)pMem->pstMemBlk;
    }

    return;
}/* IMM_MntnMemTraceInfoRptSaveMemData */

VOS_VOID IMM_MntnMemUsedBlkTraceInfoRpt
(
    VOS_UINT8  ucPoolId,
    IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8 enTrigType
)
{
    VOS_UINT32                              ulTotalCnt;
    VOS_UINT32                              ulLength;
    VOS_UINT32                              ulDataLen;
    PS_OM_EVENT_IND_STRU                   *pstImmOmEventInd;
    IMM_MEM_POOL_STRU                      *pstPoolInfo;
    IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU    *pstImmOmEventBlkMemUsedInfo;
    IMM_BLK_MEM_TRACE_INFO_STRU            *pstBlkMemTraceInfo;
    VOS_UINT32                              ulBlkMemNum;
    IMM_MEM_STRU                           *pMem;
    IMM_BLK_MEM_DEBUG_STRU                 *pstImmMemDebugInfo;
    VOS_UINT32                              ulOffset;
    VOS_UINT8                               ucRptNo = 0;
    VOS_UINT16                              usRptCnt = 0;
    VOS_UINT16                              usRptEventCnt = 0;


    pstPoolInfo = IMM_MEM_GET_POOL(ucPoolId);

    ulTotalCnt  = pstPoolInfo->usImmMemUsedCnt;

    /* 上报内存跟踪信息 */
    ulDataLen           = offsetof(IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo) + ( EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG * sizeof(IMM_BLK_MEM_TRACE_INFO_STRU));

    /*消息总长度*/
    ulLength            = offsetof(PS_OM_EVENT_IND_STRU,aucData) + ulDataLen;

    /*申请消息内存*/
    pstImmOmEventInd    = (PS_OM_EVENT_IND_STRU *)PS_MEM_ALLOC(UEPS_PID_IMM_RB_FREE, ulLength);

    if ( VOS_NULL_PTR == pstImmOmEventInd )
    {
        IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_BlkMemUsedInfoEventRpt: MemAlloc Fail!");
        return;
    }

    if ( IMM_TRUE == pstPoolInfo->ucUsedFlag )
    {
        for (ulBlkMemNum = 0; ulBlkMemNum < pstPoolInfo->usImmMemTotalCnt; ulBlkMemNum ++)
        {
            pMem = pstPoolInfo->pstImmMemStStartAddr + ulBlkMemNum;
            pstImmMemDebugInfo = &pMem->stDbgInfo;

            if (MEM_BLK_STATE_FREE != pstImmMemDebugInfo->enMemStateFlag)
            {
                ulOffset    =  offsetof(IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo)  + ( usRptCnt * sizeof(IMM_BLK_MEM_TRACE_INFO_STRU) );

                pstBlkMemTraceInfo  = (IMM_BLK_MEM_TRACE_INFO_STRU *)(pstImmOmEventInd->aucData + ulOffset );
                pstBlkMemTraceInfo->ulAllocTick         = pstImmMemDebugInfo->ulAllocTick;
                pstBlkMemTraceInfo->usAllocFileId       = pstImmMemDebugInfo->usAllocFileID;
                pstBlkMemTraceInfo->usAllocLineNum      = pstImmMemDebugInfo->usAllocLineNum;

                pstBlkMemTraceInfo->ulTraceTick         = pstImmMemDebugInfo->ulTraceTick;
                pstBlkMemTraceInfo->usTraceFileId       = (pstImmMemDebugInfo->usTraceFileID);
                pstBlkMemTraceInfo->usTraceLineNum      = (pstImmMemDebugInfo->usTraceLineNum);

                IMM_MntnMemTraceInfoRptSaveMemData(ucPoolId, pstBlkMemTraceInfo, pMem);

                usRptCnt ++;

                if (  EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG == usRptCnt )/* 分批发送，每笔最多发送一千块 */
                {
                    ucRptNo++;
                    usRptCnt = 0;
                    /*填写消息内容*/

                    pstImmOmEventInd->ulLength      = ulLength - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
                    pstImmOmEventInd->usEventId     = IMM_EVENT_BLK_MEM_USED_INFO;
                    pstImmOmEventInd->ulModuleId    = UEPS_PID_IMM_RB_FREE;

                    pstImmOmEventBlkMemUsedInfo     = (IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU *)pstImmOmEventInd->aucData;
                    pstImmOmEventBlkMemUsedInfo->ucPoolId       = ucPoolId;
                    pstImmOmEventBlkMemUsedInfo->enTrigType     = enTrigType;
                    pstImmOmEventBlkMemUsedInfo->ucNo           = ucRptNo;
                    pstImmOmEventBlkMemUsedInfo->usRptCnt       = EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG;
                    pstImmOmEventBlkMemUsedInfo->ulTotalUsedCnt = ulTotalCnt;

                    /*调用OM的事件上报接口*/
                    if ( PS_SUCC != OM_AcpuEvent((PS_OM_EVENT_IND_STRU*)pstImmOmEventInd))
                    {
                        IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_BlkMemUsedInfoEventRpt: OM Send Event Fail! \n");
                    }

                    ++usRptEventCnt;
                }

                if ( g_ulImmEventRptCntThresholdPerTime < usRptEventCnt )
                {
                    /* 释放申请的内存 */
                    PS_MEM_FREE(UEPS_PID_IMM_RB_FREE, pstImmOmEventInd);
                    return;
                }

            }
        }
    }

    if ( 0 != usRptCnt )/* 最后一笔数据块数不足 EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG */
    {
        ucRptNo++;

        ulDataLen   = ( usRptCnt * sizeof(IMM_BLK_MEM_TRACE_INFO_STRU))  + offsetof(IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo) ;

        ulLength    = offsetof(PS_OM_EVENT_IND_STRU, aucData) + ulDataLen;

        /*填写消息内容*/
        pstImmOmEventInd->ulLength      = ulLength - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
        pstImmOmEventInd->usEventId     = IMM_EVENT_BLK_MEM_USED_INFO;
        pstImmOmEventInd->ulModuleId    = UEPS_PID_IMM_RB_FREE;

        pstImmOmEventBlkMemUsedInfo     = (IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU *)pstImmOmEventInd->aucData;
        pstImmOmEventBlkMemUsedInfo->ucPoolId       = ucPoolId;
        pstImmOmEventBlkMemUsedInfo->enTrigType     = enTrigType;
        pstImmOmEventBlkMemUsedInfo->ucNo           = ucRptNo;
        pstImmOmEventBlkMemUsedInfo->usRptCnt       = usRptCnt;
        pstImmOmEventBlkMemUsedInfo->ulTotalUsedCnt = ulTotalCnt;

        /*调用OM的事件上报接口*/
        if ( PS_SUCC != OM_AcpuEvent((PS_OM_EVENT_IND_STRU*)pstImmOmEventInd))
        {
            IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_BlkMemUsedInfoEventRpt: OM Send Event Fail! \n");
        }
    }

    /*释放申请的内存*/
    PS_MEM_FREE(UEPS_PID_IMM_RB_FREE, pstImmOmEventInd);

    return ;

}
VOS_UINT32 IMM_MntnReportMemLeakMsg( VOS_UINT32 ulPid )
{
    IMM_MEM_LEAK_INFO_IND_MSG          *pstImmMemLeakInfoInd;

    /*申请消息  */
    pstImmMemLeakInfoInd = (IMM_MEM_LEAK_INFO_IND_MSG *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                               ulPid,
                                               sizeof(IMM_MEM_LEAK_INFO_IND_MSG));

    /* 内存申请失败，返回 */
    if ( VOS_NULL_PTR == pstImmMemLeakInfoInd )
    {
        return VOS_ERR;
    }

    /*填写消息内容*/
    pstImmMemLeakInfoInd->ulReceiverPid     = ulPid;
    pstImmMemLeakInfoInd->enMsgID           = ID_IMM_MEM_LEAK_INFO_IND;

    /* 发送消息 */
    PS_SEND_MSG(ulPid, pstImmMemLeakInfoInd);

    return VOS_OK;

}



VOS_VOID IMM_MntnMemOverTimeInfoRpt( VOS_VOID )
{
    VOS_UINT32                              ulTotalCnt;
    VOS_UINT32                              ulLength;
    VOS_UINT32                              ulDataLen;
    PS_OM_EVENT_IND_STRU                   *pstImmOmEventInd;
    IMM_MEM_POOL_STRU                      *pstPoolInfo;
    IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU    *pstImmOmEventBlkMemUsedInfo;
    IMM_BLK_MEM_TRACE_INFO_STRU            *pstBlkMemTraceInfo;
    IMM_BLK_MEM_DEBUG_STRU                 *pstImmMemDebugInfo;
    VOS_UINT32                              ulBlkMemNum;
    IMM_MEM_STRU                           *pMem;
    VOS_UINT32                              ulOffset;
    VOS_UINT8                               ucRptNo = 0;
    VOS_UINT16                              usRptCnt = 0;
    VOS_UINT16                              usNeedRptCnt;
    VOS_UINT32                              ulDrvRsvMemCnt;


    /* 获取内存池 */
    pstPoolInfo = IMM_MEM_GET_POOL(IMM_MEM_POOL_ID_SHARE);

    /* 获取超时的内存块个数 */
    ulTotalCnt  = IMM_MntnGetOverMemBlkCnt(IMM_MEM_POOL_ID_SHARE);

    /* 上报内存跟踪信息 */
    ulDataLen           = offsetof(IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo) + ( EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG * sizeof(IMM_BLK_MEM_TRACE_INFO_STRU));

    /*消息总长度*/
    ulLength            = offsetof(PS_OM_EVENT_IND_STRU,aucData) + ulDataLen;

    /*申请消息内存*/
    pstImmOmEventInd    = (PS_OM_EVENT_IND_STRU *)PS_MEM_ALLOC(UEPS_PID_IMM_RB_FREE, ulLength);

    if ( VOS_NULL_PTR == pstImmOmEventInd )
    {
        IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_MntnMemOverTimeInfoRpt: MemAlloc Fail!");
        return;
    }

    ulDrvRsvMemCnt = DRV_GET_PREMALLOC_SKB_NUM();

    /* 驱动会预申请内存，小于驱动预申请内存不算内存泄露 */
    if(ulTotalCnt <= ulDrvRsvMemCnt)
    {
        usNeedRptCnt = 0;
    }
    else if( EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG  < ulTotalCnt )/* 最多发送 EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG 块 */
    {
        usNeedRptCnt = EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG ;
    }
    else
    {
        usNeedRptCnt = (VOS_UINT16)(ulTotalCnt - 1);
    }

    for ( ulBlkMemNum = 0; ulBlkMemNum < pstPoolInfo->usImmMemTotalCnt; ++ulBlkMemNum )
    {
        pMem = pstPoolInfo->pstImmMemStStartAddr + ulBlkMemNum;
        pstImmMemDebugInfo = &pMem->stDbgInfo;

        if ( MEM_BLK_STATE_FREE != pstImmMemDebugInfo->enMemStateFlag )
        {

            if ( g_ulOverTimeSliceThreshold > IMM_MntnTimeDiff(pstImmMemDebugInfo->ulAllocTick, g_ulCurrentTimeSlice))
            {
                continue;
            }

            if( usNeedRptCnt > 0 )
            {
                ulOffset    =  offsetof(IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo)  + ( usRptCnt * sizeof(IMM_BLK_MEM_TRACE_INFO_STRU) );

                pstBlkMemTraceInfo  = (IMM_BLK_MEM_TRACE_INFO_STRU *)(pstImmOmEventInd->aucData + ulOffset );
                pstBlkMemTraceInfo->ulAllocTick         = pstImmMemDebugInfo->ulAllocTick;
                pstBlkMemTraceInfo->usAllocFileId       = pstImmMemDebugInfo->usAllocFileID;
                pstBlkMemTraceInfo->usAllocLineNum      = pstImmMemDebugInfo->usAllocLineNum;

                pstBlkMemTraceInfo->ulTraceTick         = pstImmMemDebugInfo->ulTraceTick;
                pstBlkMemTraceInfo->usTraceFileId       = (pstImmMemDebugInfo->usTraceFileID);
                pstBlkMemTraceInfo->usTraceLineNum      = (pstImmMemDebugInfo->usTraceLineNum);

                if ( NULL !=pMem->pstMemBlk )
                {
                    pstBlkMemTraceInfo->aucMemData[0]       = pMem->pstMemBlk[0];
                    pstBlkMemTraceInfo->aucMemData[1]       = pMem->pstMemBlk[1];
                    pstBlkMemTraceInfo->aucMemData[2]       = pMem->pstMemBlk[2];
                    pstBlkMemTraceInfo->aucMemData[3]       = pMem->pstMemBlk[3];
                }

                ++usRptCnt;
                usNeedRptCnt --;
            }
        }
    }
    /*填写消息内容*/

    ucRptNo = 1;


    pstImmOmEventInd->ulLength      = ulLength - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
    pstImmOmEventInd->usEventId     = IMM_EVENT_BLK_MEM_USED_INFO;
    pstImmOmEventInd->ulModuleId    = UEPS_PID_IMM_RB_FREE;

    pstImmOmEventBlkMemUsedInfo     = (IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU *)pstImmOmEventInd->aucData;
    pstImmOmEventBlkMemUsedInfo->ucPoolId       = IMM_MEM_POOL_ID_SHARE;
    pstImmOmEventBlkMemUsedInfo->enTrigType     = IMM_MEM_TRIG_TYPE_OVERTIME_OCCUPANCY;
    pstImmOmEventBlkMemUsedInfo->ucNo           = ucRptNo;
    pstImmOmEventBlkMemUsedInfo->usRptCnt       = usRptCnt;

    /* USB需要提前申请一块数据块，因此小于等于1,不是泄露内存*/
    if(ulTotalCnt > ulDrvRsvMemCnt)
    {
        pstImmOmEventBlkMemUsedInfo->ulTotalUsedCnt = ulTotalCnt - 1;
        IMM_MntnReportMemLeakMsg(UEPS_PID_IMM_RB_FREE);
    }
    else
    {
        pstImmOmEventBlkMemUsedInfo->ulTotalUsedCnt = 0;
    }


    /*调用OM的事件上报接口*/
    if ( PS_SUCC != OM_AcpuEvent((PS_OM_EVENT_IND_STRU*)pstImmOmEventInd))
    {
        PS_LOG(UEPS_PID_IMM_RB_FREE, 0, IMM_PRINT_ERROR, "IMM_MntnMemOverTimeInfoRpt: OM Send Event Fail! \n");
    }

    /* 释放申请的内存 */
    PS_MEM_FREE(UEPS_PID_IMM_RB_FREE, pstImmOmEventInd);

    return;
}/* IMM_MntnMemOverTimeInfoRpt */



VOS_VOID  IMM_MntnCheckPoolOverTimeOccupancy(VOS_VOID)
{
    IMM_MEM_POOL_STRU              *pstImmMemPool;


    pstImmMemPool = IMM_MEM_GET_POOL(IMM_MEM_POOL_ID_SHARE);

    if ( IMM_TRUE != pstImmMemPool->ucUsedFlag )
    {
        IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_WARNING, "IMM_MntnCheckPoolOverTimeOccupancy ,Pool %d is not used! \n", IMM_MEM_POOL_ID_SHARE);
        return ;
    }

    /* 上报内存超时事件 */
    IMM_MntnMemOverTimeInfoRpt();

    return ;
} /* IMM_MntnCheckPoolOverTimeOccupancy */


VOS_VOID  IMM_MntnRptPoolLeakInfo(VOS_VOID)
{
    if ( VOS_TRUE == atomic_read(&g_stCheckLeakFlag) )
    {
        /* 调用超时检查 */
        IMM_MntnCheckPoolOverTimeOccupancy();

        atomic_set(&g_stCheckLeakFlag, VOS_FALSE);
    }
}

#endif
VOS_VOID IMM_MntnMemUsedInfoEventRpt
(
    VOS_UINT8  ucPoolId,
    IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8 enTrigType
)
{

    if ( IMM_TRUE != IMM_MntnCheckReportMemInfoStatus(enTrigType) )
    {
        return;
    }

/*没有打开内存调试编译开关，或者没有打开内存调试使用开关，仅上报IMM Blk MEM 泄露总块数 */
/*lint -e506 -e774 */
#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    if ( !IMM_BLK_MEM_DEBUG_SWITCH_ON )
#endif
/*lint +e506 +e774 */
    {
        IMM_MntnMemUseBlkCntRpt(ucPoolId, enTrigType);
        return;
    }

#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    /* 打开了内存调试编译和使用开关，上报内存跟踪信息 */
    IMM_MntnMemUsedBlkTraceInfoRpt(ucPoolId, enTrigType);
    return ;
#endif


}/* IMM_MntnMemUsedInfoEventRpt s*/

VOS_UINT32  IMM_MntnCheckPoolLeak(VOS_UINT8 ucPoolId)
{
    IMM_MEM_POOL_STRU              *pstImmMemPool;
    VOS_UINT32                      ulDrvRsvMemCnt;


    if ( IMM_MEM_POOL_ID_BUTT <= ucPoolId )
    {
        IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_ERROR, "IMM_MemCheckPoolLeak,Poolid %d is invalid! \n", ucPoolId);
        return VOS_FALSE;
    }

    /* 获取内存池 */
    pstImmMemPool = IMM_MEM_GET_POOL(ucPoolId);

    /* 检查内存池是否使用 */
    if ( IMM_TRUE != pstImmMemPool->ucUsedFlag )
    {
        IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_WARNING, "IMM_MemCheckPoolLeak,Pool %d is not used! \n", ucPoolId);
        return VOS_FALSE;
    }

    /* 获取驱动保留Mem数量 */
    ulDrvRsvMemCnt = DRV_GET_PREMALLOC_SKB_NUM();

    /*检查内存是否泄露 */
    if ( ulDrvRsvMemCnt >= pstImmMemPool->usImmMemUsedCnt )
    {
        return VOS_FALSE;
    }

    IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_WARNING, "IMM_MEM_BLK Is Leak, UsedCnt %d ! \n",
        pstImmMemPool->usImmMemUsedCnt);

    /* 向SDT上报内存泄露事件 */
    IMM_MntnMemUsedInfoEventRpt(ucPoolId, IMM_MEM_TRIG_TYPE_LEAK);

    return VOS_TRUE;
} /* IMM_MntnCheckPoolLeak */



VOS_UINT32  IMM_MntnCheckLeak(void)
{
    VOS_UINT8                           ucPoolId;
    VOS_UINT32                          ulRet    = VOS_FALSE;
    VOS_UINT32                          ulFinal  = VOS_FALSE;


    for ( ucPoolId = IMM_MEM_POOL_ID_SHARE; ucPoolId < IMM_MEM_POOL_ID_BUTT; ++ucPoolId )
    {

        ulRet = IMM_MntnCheckPoolLeak(ucPoolId);

        if ( VOS_TRUE == ulRet )
        {
            ulFinal = VOS_TRUE;
        }
     }

    return ulFinal;
} /* IMM_MntnCheckLeak */

VOS_VOID IMM_MntnCheckMemPoolLeak(VOS_UINT8 ucCheckType)
{
    switch(ucCheckType)
    {
        case IMM_MNTN_CHECK_POOL_LEAK_TYPE_USDCNT:
        IMM_MntnCheckLeak();
        break;

#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
        case IMM_MNTN_CHECK_POOL_LEAK_TYPE_OVERTIME:
        IMM_MntnCheckPoolOverTimeOccupancy();
        break;
#endif
        default:
        IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_WARNING, "IMM_MntnCheckMemPoolLeak Invalid type %d \n", ucCheckType);
        break;
    }

    return;
}/* IMM_MntnCheckMemPoolLeak */

VOS_VOID IMM_MntnAcpuCheckPoolLeak( VOS_VOID )
{
    if ( VOS_NULL == g_ulImmMemRbFreeSem )
    {
        return ;
    }

    atomic_set(&g_stCheckLeakFlag, 1);

    VOS_SmV(g_ulImmMemRbFreeSem);

    return;
}/* IMM_MntnAcpuCheckPoolLeak */

VOS_VOID IMM_MemGetAllocFailCnt(VOS_VOID)
{
    VOS_UINT16                          usLoop;


    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemGetAllocFailCnt Start: \n");

    IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemGetAllocFailCnt Threshold: %d \n", g_usImmAllocFailCntThreshold);

#if ( FEATURE_ON == FEATURE_SKB_EXP )
    /* 打印 SKB 控制结点申请失败次数 */
    skb_get_fail_cnt();
#endif

    for( usLoop = 0; usLoop < IMM_MEM_POOL_ID_BUTT; ++usLoop )
    {
        IMM_LOG1(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM Mem Pool ID: %d \n", usLoop);
        IMM_LOG3(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM Mem Alloc Fail Cnt: %d * %d + %d \n",
                    *g_apusImmExcThresholdCnt[usLoop], g_usImmAllocFailCntThreshold, *g_apusImmAllocFailCnt[usLoop]);
    }

    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemGetAllocFailCnt End. \n");

    return;
}/* IMM_MemGetAllocFailCnt */



VOS_VOID IMM_MemSetAllocFailThreshold(VOS_UINT16 usThreshold)
{
    g_usImmAllocFailCntThreshold = usThreshold;

    return;
}/* IMM_MemSetAllocFailThreshold */

VOS_VOID IMM_MemReSetAllocFailStatus(VOS_VOID)
{
    VOS_UINT16                          usLoop;

#if ( FEATURE_ON == FEATURE_SKB_EXP )
    skb_reset_fail_cnt();
#endif

    for( usLoop = 0; usLoop < IMM_MEM_POOL_ID_BUTT; ++usLoop )
    {
        *g_apusImmExcThresholdCnt[usLoop]  = 0;
        *g_apusImmAllocFailCnt[usLoop]     = 0;
    }

    return;
}/* IMM_MemReSetAllocFailStatus */

VOS_VOID IMM_MntnInit( VOS_VOID )
{

#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
    IMM_MemRegMntnFuncCallBack(OM_GetSlice, IMM_MntnMemUsedInfoEventRpt);
#else
    IMM_MemRegMntnFuncCallBack(VOS_NULL_PTR, IMM_MntnMemUsedInfoEventRpt);
#endif

    return;
}/* IMM_MntnInit */

VOS_VOID IMM_MemPrintDetail(VOS_UINT8 ucChoice)
{
    g_ucPrintDetail = ucChoice;

    return;
}/* IMM_MemPrintDetail */

VOS_VOID IMM_MemHelp( VOS_VOID )
{
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "********************IMM_MEM软调信息************************\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MntnCheckPoolLeak(ulPoolId):            检查IMM_MEM内存泄露，其中ulPoolId:\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "                                            0--SHARE; 1--EXT;\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MntnShowUsedBlkMemInfo(VOS_VOID)        查看内存的使用信息(g_ucPrintDetail =1时打印出详细信息)。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemSetTraceFunc(ucChoice)               配置函数跟踪轨迹打印，ucChoice = 0 关闭轨迹打印。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemPrintDetail(ucChoice)                控制在使用IMM_MntnShowUsedBlkMemInfo时，是否开启使用信息打印。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemGetAllocFailCnt(VOS_VOID)            获取内存申请失败次数信息。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemSetAllocFailThreshold(usThreshold)   配置内存申请失败上报门限。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemReSetAllocFailStatus(VOS_VOID)       重置内存申请失败统计信息。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemShowMntnInfo(VOS_VOID)               获取IMM 内存可维可测信息。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemClearMntnInfo(VOS_VOID)              清除IMM 内存可维可测信息。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemGetFreeMntnEntityAddr(VOS_VOID)      获取 IMM FREE 内存可维可测实体地址。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_MemGetRbMntnEntityAddr(VOS_VOID)        获取 IMM RB 内存可维可测实体地址。\n");
    IMM_LOG(UEPS_PID_IMM_RB_FREE, IMM_PRINT_NORMAL, "IMM_RbMemPrintIsrCnt(VOS_VOID)              A核收到IPF中断次数。\n");

    return;
}


#if(IMM_DEBUG_TRACE_FUNC == FEATURE_ON)

VOS_VOID IMM_MemSetTraceFunc(VOS_UINT8 ucChoice)
{
    g_ucImmMemDebugPrint = ucChoice;

    return;
}/* IMM_MemSetTraceFunc */
#endif


/******************************************************************************/
/*******************************V9R1版本***************************************/
/******************************************************************************/
#else

VOS_VOID IMM_MntnAcpuCheckPoolLeak( VOS_VOID )
{
    return;
}/* IMM_MntnAcpuCheckPoolLeak */


#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

