/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : Om.c
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2008年5月3日
  最近修改      :
  功能描述      : 该C文件给出了OM模块的实现
  函数列表      :
  修改历史      :
  1.日    期    : 2008年5月3日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"
#include "OmApp.h"
#include "omprivate.h"
#include "pslog.h"
#include "omsdlog.h"
#include "cpm.h"
#include "Omappagent.h"
#include "NVIM_Interface.h"
#include "OamSpecTaskDef.h"
#include "GU_MSPComm.h"
#include "OmAppRl.h"
#include "TafNvInterface.h"
#include "DrvNvInterface.h"
#include "SCMProc.h"
#include "OmHsicPpm.h"
#include "SCMSoftDecode.h"
#include "CbtPpm.h"
#include "v_private.h"
#include "product_config.h"

#if (RAT_MODE != RAT_GU)
#include "msp_nvim.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/*lint -e767 修改人：甘兰47350；检视人：李霄46160；原因简述：LOG方案设计需要*/
#define    THIS_FILE_ID        PS_FILE_ID_ACPU_OM_C
/*lint +e767 修改人：甘兰47350；检视人：lixiao；*/

/*caution: no more 30 char */
#define OAM_PLATFORM_VERSION "V700R001C01B010"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*用来保存TRACE和EVENT上报的过滤信息*/
OM_TRACE_EVENT_CONFIG_PS_STRU           g_stAcpuTraceEventConfig;

/*序列号，用来TRACE,EVENT消息的序号*/
/*VOS_UINT32                            g_ulAcpuOMMsgSN         = 1;*/
#if ( (VOS_LINUX == VOS_OS_VER) || (VOS_WIN32 == VOS_OS_VER) )
atomic_t                                g_ulAcpuOMMsgSN;
#endif

/*保存当前的工具ID号*/
VOS_UINT16                              g_usAcpuToolId          = 0;

/* 用来同步NV读写和ErrorLog输出操作,保证每次只有一个任务在进行操作*/
VOS_SEM                                 g_ulOmAcpuSyncSem;

/* 二进制信号量，用来等待返回结果 */
VOS_SEM                                 g_ulOmAcpuCnfSem;

/*互斥信号量，用来保护OM HSIC关联 */
VOS_SEM                                 g_ulOmTxCbtSem;

/* 保存回复消息的指针 */
MsgBlock                               *g_pstOmAcpuCnfMsg  = VOS_NULL_PTR;

/*OM BUF FULL 周期上报CPU消息定时器*/
HTIMER                                  g_stAcpuRegularBufFullTmr = VOS_NULL_PTR ;

/* 建链信息周期上报CPU消息定时器 */
HTIMER                                  g_stAcpuConnectInfoTmr = VOS_NULL_PTR ;

/* 记录收到消息信息的buffer及当前长度 */
OM_RECORD_BUF_STRU                      g_astAcpuRecordInfo[VOS_EXC_DUMP_MEM_NUM_BUTT];

/* 记录CCORE 当前状态*/
OM_CPU_STATUS_INFO                      g_astCcoreStatus[OM_CPU_STATUS_BUTT];

#define NO_CONFIG_BIT                   (0)
#define TRACE_CONFIG_BIT                (BIT_N(0))
#define EVENT_CONFIG_BIT                (BIT_N(1))
#define TRACE_MASK_CONFIG_BIT           (BIT_N(2))


/*****************************************************************************
  3 函数声明
*****************************************************************************/
extern VOS_VOID   Log_AcpuOmMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);
extern VOS_UINT32 WuepsDiagPidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_VOID   DIAG_AcpuMsgProc(struct MsgCB *pMsg);

extern VOS_UINT32 OM_AcpuAgentPidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_VOID   OM_AcpuAgentMsgProc(MsgBlock* pMsg);

extern VOS_VOID Om_AcpuBbpConfigMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);

extern VOS_VOID SCM_StopAllSrcChan(VOS_VOID);

extern VOS_VOID OM_AcpuErrLogMsgProc(MsgBlock* pMsg);

extern VOS_VOID OM_AcpuICCInfoWriteLog(VOS_UINT32 ulParam, VOS_UINT32 ulTimerName);

#if(FEATURE_ON == FEATURE_PTM)
extern VOS_VOID OM_AcpuRcvMsgFinish(VOS_VOID);
#endif
/*****************************************************************************
 Prototype      : OM_AcpuAddSNTime
 Description    : Add the Sequence number and Time stamp into the packet.
 Input          : pulSN        - The pointer of the sequence number.
                  pulTimeStamp - The pointer of the time stamp.
 Output         : void
 Return Value   : void

 History        : ---
    Date        : 2008-05-3
    Author      : g47350
    Modification: Created function
 *****************************************************************************/
VOS_VOID OM_AcpuAddSNTime(VOS_UINT32 *pulSN, VOS_UINT32 *pulTimeStamp)
{
    /* VOS_INT   lLockKey; */

    /* lLockKey = VOS_SplIMP(); */

#if ( (VOS_LINUX == VOS_OS_VER) || (VOS_WIN32 == VOS_OS_VER) )
    *(pulSN) = (VOS_UINT32)atomic_inc_return(&g_ulAcpuOMMsgSN);
#endif

    /* VOS_Splx(lLockKey); */

    if (VOS_NULL_PTR != pulTimeStamp)
    {
        *(pulTimeStamp) = OM_GetSlice();
    }
}

/*****************************************************************************
 Prototype       : OM_AcpuAddAppHeader
 Description     : Add the APP header to the front of msg.
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2008-03-20
    Author       : g47350
    Modification : Created function

    Data         : 2008-9-13
    Author       : 甘兰 47350.
    Modification : 添加对LOG增加消息头的操作
 *****************************************************************************/
VOS_VOID OM_AcpuAddAppHeader (VOS_UINT8 ucFuncType, VOS_UINT8* pucInputMsg,
                              OM_APP_HEADER_STRU* pstAppHeader, VOS_UINT32 ulTraceMsgLen)
{
    PS_OM_EVENT_IND_STRU   *pstrEvent;
    OM_APP_TRACE_STRU      *pstAppTrace;
    OM_APP_EVENT_STRU      *pstAppEvent;
    OM_APP_LOG_STRU        *pstAppLog;

    pstAppHeader->ucFuncType = ucFuncType;
    pstAppHeader->usLength = (VOS_UINT16)(ulTraceMsgLen - VOS_OM_HEADER_LEN);
    OM_AcpuAddSNTime(&(pstAppHeader->ulSn), &(pstAppHeader->ulTimeStamp));
    /*Complete different procedures by function type.*/
    switch (ucFuncType)
    {
        case OM_TRACE_FUNC:
        case OM_AIR_FUNC:
            pstAppTrace = (OM_APP_TRACE_STRU*)pstAppHeader;
            pstAppTrace->usPrimId = OM_APP_TRACE_IND;
            VOS_MemCpy(pstAppTrace->aucPara, pucInputMsg, ulTraceMsgLen - OM_APP_TRACE_LEN);
            break;

        case OM_EVENT_FUNC:
            pstrEvent = (PS_OM_EVENT_IND_STRU*)pucInputMsg;
            pstAppEvent = (OM_APP_EVENT_STRU*)pstAppHeader;
            pstAppEvent->ulModuleId = pstrEvent->ulModuleId;
            pstAppEvent->usEventId =  pstrEvent->usEventId;
            VOS_MemCpy(pstAppEvent->aucPara, pstrEvent->aucData, ulTraceMsgLen - OM_APP_EVENT_LEN);
            break;

        case OM_LOG_FUNC:
            pstAppLog = (OM_APP_LOG_STRU*)pstAppHeader;
            pstAppLog->usPrimId = OM_APP_PRINT_IND;
            VOS_MemCpy(&(pstAppLog->ulModuleId), pucInputMsg,
                         (ulTraceMsgLen - OM_APP_LOG_LEN) + LOG_MODULE_ID_LEN);
            break;
         default:
            break;
    }
    return;
}
/*****************************************************************************
 Prototype      : OM_AcpuSendDataMNTNChannel
 Description    : OM sends the data to tool side.
 Input          : pstCtrlInfo -- 指向IND或CNF通道的控制变量时
                  ucFuncType      - The function type of msg.
                  ulResult        - The execution result.
                  usReturnPrimId  - The returned PrimId.
 Output         : void
 Return Value   : VOS_OK - Success.

 History        : ---
    Date        : 2008-06-19
    Author      : g47350
    Modification: Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuSendDataMNTNChannel(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, OM_RSP_PACKET_STRU *pucMsg, VOS_UINT16 usMsgLen)
{
    OM_APP_MSG_EX_STRU                 *pstAppMsg;
    OM_UE_PC_SUC_STRU                  *pstUeToPcSucRecord = &(pstCtrlInfo->stUeToPcSucRecord);
    OM_UE_PC_FAIL_STRU                 *pstUeToPcErrRecord = &(pstCtrlInfo->stUeToPcErrRecord);

    pstUeToPcSucRecord->stTotalData.ulDataLen += usMsgLen;
    pstUeToPcSucRecord->stTotalData.ulNum++;

    /* 参数检测 */
    if ( (0 == usMsgLen) || (VOS_NULL_PTR == pucMsg ) )
    {
        LogPrint("The input parameter is wrong.\n");

        pstUeToPcErrRecord->stParaErrData.ulDataLen += usMsgLen;
        pstUeToPcErrRecord->stParaErrData.ulNum++;

        return VOS_ERR;
    }

    if( VOS_FALSE != VOS_CheckInterrupt() )
    {
        pstUeToPcErrRecord->stIntData.ulDataLen += usMsgLen;
        pstUeToPcErrRecord->stIntData.ulNum++;

        return VOS_ERR;
    }

    pstAppMsg = (OM_APP_MSG_EX_STRU*)pucMsg;

    if ((OM_STATE_ACTIVE == g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff)
        || ((OM_QUERY_FUNC == pstAppMsg->ucFuncType)
            &&(OM_APP_ESTABLISH_CNF == pstAppMsg->usPrimId)))
    {
        if(VOS_OK != OMRL_AcpuMsgSplit(pstCtrlInfo, (VOS_UINT8*)pucMsg, usMsgLen))
        {
            pstUeToPcErrRecord->stLostData.ulDataLen += usMsgLen;
            pstUeToPcErrRecord->stLostData.ulNum++;
        }

        return VOS_OK;
    }

    pstUeToPcErrRecord->stLinkErrData.ulDataLen += usMsgLen;
    pstUeToPcErrRecord->stLinkErrData.ulNum++;

    return VOS_ERR;
}
VOS_UINT32 OM_AcpuSendDataCbtChannel(OM_RSP_PACKET_STRU *pucMsg, VOS_UINT16 usMsgLen)
{
    VOS_UINT8                           ucCurSegNum = 1; /*当前段序号*/
    VOS_UINT_PTR                        ulTempAddress;
    /*指向OM分包头内容的指针*/
    OM_APP_MSG_SEG_EX_STRU              stSegMsgEx;
    VOS_UINT8                           ucMsgCnt    = 1; /*分段的数量*/
    OM_APP_MSG_EX_STRU                 *pstAppMsg;
    VOS_UINT8                          *pucBuf = VOS_NULL_PTR;
    VOS_UINT8                          *pucTmpBuf;
    VOS_UINT32                          ulTmpValue = OM_USB_FRAME_TAIL;

    pstAppMsg = (OM_APP_MSG_EX_STRU *)pucMsg;

    if ((OM_STATE_ACTIVE == g_stAcpuCbtCtrlInfo.ulOMSwitchOnOff)
        || ((OM_QUERY_FUNC == pstAppMsg->ucFuncType)
            &&(OM_APP_ESTABLISH_CNF == pstAppMsg->usPrimId)))
    {

        /************************ 拆包然后hdlc编码后发送 ***********************/
        /* 计算分包个数 */
        ucMsgCnt = (VOS_UINT8)(((usMsgLen + OM_APP_MSG_SEGMENT_LEN) - 1)/OM_APP_MSG_SEGMENT_LEN);

        /*分配分包结构的内存空间*/
        pucTmpBuf = (VOS_UINT8*)VOS_MemAlloc(WUEPS_PID_OM,
                    DYNAMIC_MEM_PT, sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_MSG_SEGMENT_LEN + OM_RL_DATATYPE_LEN + OM_USB_TAIL_LEN);

        if (VOS_NULL_PTR == pucTmpBuf)
        {
            LogPrint("OM_UsbFrameInit:VOS_MemAlloc g_pstSegMsgEx Failed!\n");
            return VOS_ERR;
        }

        /* 每包需要添加一字节datatype，表示GU数据 */
        pucTmpBuf[0] = SCM_DATA_TYPE_GU;
        pucBuf = pucTmpBuf + OM_RL_DATATYPE_LEN;

        stSegMsgEx.stSocpHdr.ucCPUID   = OM_TOOL_CCPU_ID;
        stSegMsgEx.ulFrameHead         = OM_USB_FRAME_HEAD;

        ulTempAddress = (VOS_UINT_PTR)pucMsg;

        /* 填充SOCP头中的RTC值 */
        stSegMsgEx.stSocpHdr.ulRTC = (VOS_UINT32)mdrv_rtc_get_value();
        stSegMsgEx.stMsgSeg.ucSegNum = ucMsgCnt;
        stSegMsgEx.stMsgSeg.usSegLen = OM_APP_SPLIT_MSG_LEN;

        /* 大于最大分包大小的数据，按照最大分包大小进行数据发送的处理 */
        for (ucCurSegNum = 1; ucCurSegNum < ucMsgCnt; ucCurSegNum++)
        {
            stSegMsgEx.stMsgSeg.ucSegSn = ucCurSegNum;

            VOS_MemCpy(pucBuf, &stSegMsgEx, sizeof(stSegMsgEx));
            pucBuf += sizeof(stSegMsgEx);

            VOS_MemCpy(pucBuf, (VOS_UINT8*)ulTempAddress, OM_APP_MSG_SEGMENT_LEN);
            pucBuf += OM_APP_MSG_SEGMENT_LEN;

            VOS_MemCpy(pucBuf, (VOS_UINT8*)&ulTmpValue, OM_USB_TAIL_LEN);

            /* 消息头前加上长度并调用USB接口发送出去 */
            if (VOS_OK != OMRL_AcpuSendCbtData(pucTmpBuf, sizeof(OM_APP_MSG_SEG_EX_STRU)+ OM_RL_DATATYPE_LEN + OM_APP_MSG_SEGMENT_LEN + OM_USB_TAIL_LEN))
            {
                VOS_MemFree(WUEPS_PID_OM, pucTmpBuf);

                return VOS_ERR;
            }

            pucBuf = pucTmpBuf + OM_RL_DATATYPE_LEN;

            ulTempAddress += OM_APP_MSG_SEGMENT_LEN;

            /* 计算剩余数据包大小 */
            usMsgLen -= OM_APP_MSG_SEGMENT_LEN;
        }

        /*设置消息头参数*/
        stSegMsgEx.stMsgSeg.usSegLen = (VOS_UINT16)(usMsgLen + OM_APP_MSG_SEGMENT_HEADER_LENGTH);
        stSegMsgEx.stMsgSeg.ucSegSn  = ucCurSegNum;

        /*为了保持Ringbuffer中存放的数据包4字节对齐*/
        usMsgLen = (usMsgLen + 3U) & ~0x03U;

        VOS_MemCpy(pucBuf, &stSegMsgEx, sizeof(stSegMsgEx));
        pucBuf += sizeof(stSegMsgEx);

        VOS_MemCpy(pucBuf, (VOS_UINT8*)ulTempAddress, usMsgLen);
        pucBuf += usMsgLen;

        VOS_MemCpy(pucBuf, (VOS_UINT8*)&ulTmpValue, OM_USB_TAIL_LEN);

        /* 消息头前加上长度并调用USB接口发送出去 */
        if ( VOS_OK != OMRL_AcpuSendCbtData(pucTmpBuf, sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_RL_DATATYPE_LEN + usMsgLen + OM_USB_TAIL_LEN))
        {
            VOS_MemFree(WUEPS_PID_OM, pucTmpBuf);

            return VOS_ERR;
        }

        VOS_MemFree(WUEPS_PID_OM, pucTmpBuf);
    }

    return VOS_OK;
}


VOS_UINT32 OM_AcpuSendDataChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel, OM_RSP_PACKET_STRU *pucMsg, VOS_UINT16 usMsgLen)
{
    if (OM_LOGIC_CHANNEL_CNF == enChannel)
    {
        return OM_AcpuSendDataMNTNChannel(&g_stAcpuTxCnfCtrlInfo, pucMsg, usMsgLen);
    }

    if (OM_LOGIC_CHANNEL_IND == enChannel)
    {
        return OM_AcpuSendDataMNTNChannel(&g_stAcpuTxIndCtrlInfo, pucMsg, usMsgLen);
    }

    if (OM_LOGIC_CHANNEL_CBT == enChannel)
    {
        return OM_AcpuSendDataCbtChannel(pucMsg, usMsgLen);
    }

    /* 打印错误 */
    LogPrint("OM_AcpuSendDataChannel, enChannel is error.\n");

    return VOS_ERR;
}
VOS_VOID OM_AcpuSendResultChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel, VOS_UINT8 ucFuncType,
                            VOS_UINT32 ulResult, VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU stOmToAppMsg;

    stOmToAppMsg.ucFuncType = ucFuncType;
    stOmToAppMsg.usLength   = OM_APP_MSG_EX_LEN;
    stOmToAppMsg.usPrimId   = usReturnPrimId;
    stOmToAppMsg.usToolId   = g_usAcpuToolId;
    *((VOS_UINT32*)(stOmToAppMsg.aucPara)) = ulResult;

    OM_AcpuSendDataChannel(enChannel,(OM_RSP_PACKET_STRU*)&stOmToAppMsg, (VOS_UINT16)sizeof(OM_APP_MSG_EX_STRU));

    return;
}


VOS_VOID OM_AcpuSendResult(VOS_UINT8 ucFuncType,
                            VOS_UINT32 ulResult, VOS_UINT16 usReturnPrimId)
{
    OM_AcpuSendResultChannel(OM_LOGIC_CHANNEL_IND, ucFuncType, ulResult, usReturnPrimId);

    return;
}

/*****************************************************************************
 Prototype      : OM_AcpuSendData
 Description    : OM sends the data to tool side.
 Input          : ucFuncType      - The function type of msg.
                  ulResult        - The execution result.
 Output         : void
 Return Value   : VOS_OK - Success.

 History        : ---
    Date        : 2008-06-19
    Author      : g47350
    Modification: Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuSendData(OM_RSP_PACKET_STRU *pucMsg, VOS_UINT16 usMsgLen)
{
     return OM_AcpuSendDataChannel(OM_LOGIC_CHANNEL_IND, pucMsg, usMsgLen);
}


VOS_VOID OM_AcpuSendContentChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel,VOS_UINT8 ucFuncType,
                             OM_RSP_PACKET_STRU *pstOmToAppMsg, VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU *pstTmpMsg;

    pstTmpMsg = (OM_APP_MSG_EX_STRU*)pstOmToAppMsg;

    pstTmpMsg->ucFuncType = ucFuncType;
    pstTmpMsg->usPrimId   = usReturnPrimId;
    pstTmpMsg->usToolId   = g_usAcpuToolId;

    OM_AcpuSendDataChannel(enChannel, pstOmToAppMsg,
            (VOS_UINT16)(pstOmToAppMsg->usLength + VOS_OM_HEADER_LEN));
    return;
}


VOS_VOID OM_AcpuSendContent(VOS_UINT8 ucFuncType,
                             OM_RSP_PACKET_STRU *pstOmToAppMsg, VOS_UINT16 usReturnPrimId)
{
    OM_AcpuSendContentChannel(OM_LOGIC_CHANNEL_IND, ucFuncType, pstOmToAppMsg, usReturnPrimId);

    return;
}

/*****************************************************************************
 函 数 名  : OM_AppGreenChannel
 功能描述  : 绿色通道，OM提供给外部的发送接口
 输入参数  : ucFuncType  - 功能ID
             usPrimId    - 原语值
             pucData     - 指向发送数据的指针
             usLen       - 发送数据的长度
 输出参数  : 无
 返 回 值  : VOS_OK  - 成功
             VOS_ERR - 失败

 修改历史      :
  1.日    期   : 2009年6月22日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 OM_AppGreenChannel(VOS_UINT8 ucFuncType, VOS_UINT16 usPrimId,
                              VOS_UINT8 *pucData, VOS_UINT16 usLen)
{
    OM_APP_TRACE_STRU      *pstAppMsg;
    VOS_UINT32              ulResult;

    pstAppMsg = (OM_APP_TRACE_STRU*)VOS_MemAlloc(ACPU_PID_OM,
                                         DYNAMIC_MEM_PT, usLen + OM_APP_TRACE_LEN);
    if (NULL == pstAppMsg)
    {
        PS_LOG(ACPU_PID_OM, 0, PS_PRINT_ERROR, "OM_AppGreenChannel:VOS_MemAlloc.\n");

        return VOS_ERR;
    }

    pstAppMsg->stAppHeader.ucFuncType = ucFuncType;
    pstAppMsg->stAppHeader.usLength   = (VOS_UINT16)((usLen + OM_APP_TRACE_LEN) - VOS_OM_HEADER_LEN);
    pstAppMsg->usPrimId = usPrimId;
    pstAppMsg->usToolId = 0;

    OM_AcpuAddSNTime(&(pstAppMsg->stAppHeader.ulSn), &(pstAppMsg->stAppHeader.ulTimeStamp));

    /*假如内容长度为0，表示没有发送内容*/
    if ((0 != usLen) && (VOS_NULL_PTR != pucData))
    {
        VOS_MemCpy(pstAppMsg->aucPara, pucData, usLen);
    }

    ulResult = OM_AcpuSendData((OM_RSP_PACKET_STRU*)pstAppMsg, (VOS_UINT16)(usLen + OM_APP_TRACE_LEN));

    VOS_MemFree(ACPU_PID_OM, pstAppMsg);

    return ulResult;
}


/*****************************************************************************
 函 数 名  : OM_AcpuSendLog
 功能描述  : 将ACPU's LOG数据通过OM输出到工具侧。
 输入参数  : pucLogData  - 指向发送数据的指针
             lLength     - 发送数据的长度
 输出参数  : 无
 返 回 值  : VOS_OK  - 成功
             VOS_ERR - 失败

 修改历史      :
  1.日    期   : 2011年7月1日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数,添加可维可测第三阶段需求
*****************************************************************************/
VOS_UINT32 OM_AcpuSendLog(VOS_UINT8 *pucLogData, VOS_UINT32 ulLength)
{
    VOS_UINT8   *pucLogMsg;
    VOS_UINT32   ulLogMsgLen;

    /*计算发送给工具侧LOG消息包的总长度，由于OM_APP_LOG_LEN中
    已经包括了ModuleId的长度，故需要将其减去*/
    ulLogMsgLen = (ulLength + OM_APP_LOG_LEN) - LOG_MODULE_ID_LEN;
    pucLogMsg = (VOS_UINT8*)VOS_MemAlloc(ACPU_PID_OM,
                                           DYNAMIC_MEM_PT, ulLogMsgLen);
    if (VOS_NULL_PTR == pucLogMsg)
    {
        vos_printf("OM_AcpuSendLog, VOS_MemAlloc FAIL!\n");
        return VOS_ERR;
    }

    OM_AcpuAddAppHeader(OM_LOG_FUNC, pucLogData,
          (OM_APP_HEADER_STRU*)pucLogMsg, ulLogMsgLen);

    /*Sent to the tool side.*/
    OM_AcpuSendData((OM_RSP_PACKET_STRU*)pucLogMsg, (VOS_UINT16)ulLogMsgLen);

    VOS_MemFree(ACPU_PID_OM, pucLogMsg);

    return VOS_OK;
}
VOS_UINT32 OM_AcpuCheckMsgPID(VOS_UINT32 ulModuleId, VOS_UINT32 ulConfigBit)
{
    VOS_UINT32 ulResult;

    ulResult = NO_CONFIG_BIT;

    if ((VOS_PID_DOPRAEND <= ulModuleId) && (VOS_PID_BUTT > ulModuleId))
    {
        ulResult = (g_stAcpuTraceEventConfig.aulRptConfigLevl[ulModuleId - VOS_PID_DOPRAEND] & ulConfigBit);
    }

    return ulResult;
}


VOS_VOID OM_AcpuSavePSConfigInfo(VOS_UINT32* pulModuleIds,VOS_UINT32 ulModuleNum, VOS_UINT32 ulConfigBit)
{
    VOS_UINT32 ulModuleIndex;
    VOS_UINT32 ulModuleId;

    if (OM_ALL_MSG_ON == ulModuleNum)
    {
        for (ulModuleIndex = 0; ulModuleIndex < OM_MAX_PS_MODULE_NUM; ulModuleIndex++)
        {
            g_stAcpuTraceEventConfig.aulRptConfigLevl[ulModuleIndex] |= ulConfigBit;
        }

        return;
    }

    /* 清除上次配置 */
    for (ulModuleIndex = 0; ulModuleIndex < OM_MAX_PS_MODULE_NUM; ulModuleIndex++)
    {
        g_stAcpuTraceEventConfig.aulRptConfigLevl[ulModuleIndex] &= (~ulConfigBit);
    }

    /* 重新配置 */
    for (ulModuleIndex = 0; ulModuleIndex < ulModuleNum; ulModuleIndex++)
    {
        ulModuleId = pulModuleIds[ulModuleIndex];

        if ((VOS_PID_DOPRAEND <= ulModuleId) && (VOS_PID_BUTT > ulModuleId) )
        {
            g_stAcpuTraceEventConfig.aulRptConfigLevl[ulModuleId-VOS_PID_DOPRAEND] |= ulConfigBit;
        }
    }

    return;
}


VOS_VOID OM_AcpuShowPSConfigInfo(VOS_UINT32 ulModuleId)
{
    if ((ulModuleId >= VOS_PID_BUTT) || (ulModuleId < VOS_PID_DOPRAEND))
    {
        vos_printf("OM_AcpuShowPSConfigInfo: Module Id is wrong.");
        return;
    }

    vos_printf("OM_AcpuShowPSConfigInfo: ModuleId is %d, Config Level is %d\n",
        ulModuleId, g_stAcpuTraceEventConfig.aulRptConfigLevl[ulModuleId - VOS_PID_DOPRAEND]);
}

/*lint -e416 -e661 -e662 -e717 修改人:甘兰*/

/*****************************************************************************
 Prototype       : OM_ACPUConfigMsgProc
 Description     : Complete the config of the ACPU's TRACE, EVENT.
 Input           : pstAppToOMMsg -- The pointer of the msg received from APP.
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_AcpuConfigMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr)
{
    APP_OM_MSG_EX_STRU             *pstAppToOmMsg;
    OM_TRACE_CONFIG_REQ_STRU       *pstTraceConfig;
    OM_TRACE_MASK_CONFIG_REQ_STRU  *pstTraceMaskCfg;
    OM_EVENT_CONFIG_TABLE_STRU     *pstEventConfig;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pRspPacket;

    switch (pstAppToOmMsg->ucFuncType)
    {
        case OM_TRACE_FUNC:
            if (APP_OM_TRACE_REQ == pstAppToOmMsg->usPrimId)
            {
                pstTraceConfig = (OM_TRACE_CONFIG_REQ_STRU*)(pstAppToOmMsg->aucPara);

                VOS_MemCpy(&g_stAcpuTraceEventConfig,pstTraceConfig,sizeof(VOS_UINT32));
                OM_AcpuSavePSConfigInfo(pstTraceConfig->aulModuleId,pstTraceConfig->ulModuleNum, TRACE_CONFIG_BIT);
            }

            if (APP_OM_TRACE_MASK_REQ == pstAppToOmMsg->usPrimId)
            {
                pstTraceMaskCfg = (OM_TRACE_MASK_CONFIG_REQ_STRU*)(pstAppToOmMsg->aucPara);

                OM_AcpuSavePSConfigInfo(pstTraceMaskCfg->aulModuleId, pstTraceMaskCfg->ulModuleNum, TRACE_MASK_CONFIG_BIT);
            }

            break;

        case OM_EVENT_FUNC:

            pstEventConfig = (OM_EVENT_CONFIG_TABLE_STRU*)(pstAppToOmMsg->aucPara);

            OM_AcpuSavePSConfigInfo(pstEventConfig->aulModuleId,pstEventConfig->ulModuleNum, EVENT_CONFIG_BIT);

            break;

        default:
            break;
    }

    return;
}

/*Global map table used to find the function according the FuncId.*/
OM_FUNCID_PROC_STRU g_astAcpuOmFuncIdProcTbl[OM_FUNCID_MAX_NUM] =
{
    {VOS_NULL_PTR,                0,                LEVEL_NORMAL},
    {OM_AcpuConfigMsgProc,        OM_TRACE_FUNC,    LEVEL_NORMAL},
    {VOS_NULL_PTR,                OM_AIR_FUNC,      LEVEL_NORMAL},
    {VOS_NULL_PTR,                OM_FS_FUNC,       LEVEL_NORMAL},
    {VOS_NULL_PTR,                OM_TRANS_FUNC,    LEVEL_NORMAL},
    {OM_AcpuConfigMsgProc,        OM_EVENT_FUNC,    LEVEL_NORMAL},
    {Log_AcpuOmMsgProc,           OM_LOG_FUNC,      LEVEL_NORMAL},
    {VOS_NULL_PTR,                OM_WRF_FUNC,      LEVEL_NORMAL},
    {VOS_NULL_PTR,                OM_GRF_FUNC,      LEVEL_NORMAL},
    {OM_AcpuQueryMsgProc,         OM_AUTH_NV_FUNC,  LEVEL_NORMAL}, /* 工具读写NV的Functype是9 */
    {OM_AcpuQueryMsgProc,         OM_QUERY_FUNC,    LEVEL_NORMAL}, /*用于处理链路消息*/
    {OM_AcpuQueryMsgProc,         OM_NV_FUNC,       LEVEL_NORMAL},
    {VOS_NULL_PTR,                OM_SIGCAL_FUNC,   LEVEL_NORMAL},
    {VOS_NULL_PTR,                OM_GREEN_FUNC,    LEVEL_NORMAL},
    {OM_PrintfMsgProc,            OM_PRINTF_FUNC,   LEVEL_NORMAL},
    {OM_ReadSdMsgProc,            OM_READSD_FUNC,   LEVEL_NORMAL},
    {VOS_NULL_PTR,                OM_NOSIG_FUNC,    LEVEL_NORMAL},
    {Om_AcpuBbpConfigMsgProc,     OM_BBP_FUNC,      LEVEL_NORMAL}   /* BBP可维可测项目增加,j00174725 2012-11-26 */
};



VOS_INT32  OM_AcpuRegisterReqProc(VOS_UINT8 ucModuleId,
                                                OM_REQUEST_PROCEDURE *pReqPro)
{
    if ((OM_FUNCID_ACPU_PART_NUM >= ucModuleId) || (OM_FUNCID_MAX_NUM <= ucModuleId)
        || (0 != g_astAcpuOmFuncIdProcTbl[ucModuleId].ulFuncType))
    {
        return VOS_ERR;
    }

    g_astAcpuOmFuncIdProcTbl[ucModuleId].ulFuncType = (VOS_UINT32)ucModuleId;
    g_astAcpuOmFuncIdProcTbl[ucModuleId].pOmFuncProc = pReqPro;

    return VOS_OK;
}


VOS_VOID OM_AcpuRegisterDrvCB(VOS_VOID)
{
    DRV_MSP_PROC_REG(OM_REQUEST_PROC, (BSP_MspProc)OM_AcpuRegisterReqProc);

    DRV_MSP_PROC_REG(OM_ADDSNTIME_PROC, (BSP_MspProc)OM_AcpuAddSNTime);

    return;
}


 VOS_UINT32 OM_AcpuStartEventTimer(VOS_VOID)
{
    VOS_UINT32 ulRtn;

    ulRtn = VOS_StartRelTimer(&g_stAcpuRegularBufFullTmr, ACPU_PID_OM,
                              OM_BUFF_TIMER_LENTH,
                              OM_BUFF_TIMER, 0,
                              VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION);

    if (VOS_OK != ulRtn)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 Prototype       : OM_AcpuStartConnectEventTimerStart
 Description     : OM建链信息上报事件定时器启动函数
 Input           : None
 Output          : None
 Return Value    :

 History         : ---
    Date         : 2014-04-09
    Author       : h59254
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuConnectInfoEventTimerStart(VOS_VOID)
{
    VOS_UINT32 ulRtn;

    ulRtn = VOS_StartRelTimer(&g_stAcpuConnectInfoTmr, ACPU_PID_OM,
                              OM_CONNECT_TIMER_LENTH,
                              OM_CONNECT_INFO_TIMER, 0,
                              VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION);

    if (VOS_OK != ulRtn)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 Prototype       : OM_AcpuConnectInfoReqSnd
 Description     : OM建链信息请求函数，发到CCPU处理
 Input           : None
 Output          : None
 Return Value    :

 History         : ---
    Date         : 2014-04-09
    Author       : h59254
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_AcpuConnectInfoReqSnd(VOS_VOID)
{
    MsgBlock       *pMsg;

    pMsg = (MsgBlock *)VOS_AllocMsg(ACPU_PID_OMAGENT, sizeof(MsgBlock) - VOS_MSG_HEAD_LENGTH);

    pMsg->ulReceiverPid = CCPU_PID_OMAGENT;
    *(VOS_UINT16*)(pMsg->aucValue) = OM_CONNECT_INFO_REQ;

    (VOS_VOID)VOS_SendMsg(ACPU_PID_OMAGENT, pMsg);

    return;
}

/*****************************************************************************
 Prototype       : OM_AcpuConnecInfoTimeOutMsgProc
 Description     : OM建链信息上报事件定时器超时消息处理函数
 Input           : None
 Output          : None
 Return Value    :

 History         : ---
    Date         : 2014-04-09
    Author       : h59254
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_AcpuConnecInfoTimeOutMsgProc(VOS_VOID)
{
    if (OM_STATE_ACTIVE == g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff)
    {
        OM_AcpuConnectInfoReqSnd();

        if (VOS_OK != OM_AcpuConnectInfoEventTimerStart())
        {
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_AcpuConnecInfoTimeOutMsgProc: OM_AcpuStartConnectEventTimerStart failed.\n");
        }
    }

    return;
}


VOS_VOID OM_AcpuTimerMsgProc(MsgBlock* pMsg)
{
    VOS_UINT32 ulName;

    ulName = ((REL_TIMER_MSG*)pMsg)->ulName;

    if (OM_BUFF_TIMER == ulName)
    {
        OMRL_AcpuReportEvent();

        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_AcpuTimerMsgProc:Recv timer and Send Event.");

        if (VOS_OK !=  OM_AcpuStartEventTimer())
        {
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_AcpuTimerMsgProc:Start timer fail\r\n.");

            return ;
        }
    }
#if(FEATURE_ON == FEATURE_PTM)
    else if(OM_ERRLOG_TIMER == ulName)
    {
        OM_AcpuRcvMsgFinish();
    }
#endif
    else if (OM_CONNECT_INFO_TIMER == ulName)
    {
        OM_AcpuConnecInfoTimeOutMsgProc();
    }
    else
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_TimerMsgProc:Unknown timer.");
    }
}


#if 0
/*****************************************************************************
 Prototype       : Om_PortSwitchProc
 Description     :
 Input           : pMsg -- The pointer of the msg.
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2012-03-05
    Author       : h59254
    Modification : Created function
 *****************************************************************************/
VOS_VOID Om_PortSwitchProc(OM_PORT_SWITCH_MODE_ENUM_UINT32 ulSwitchMode)
{
    if (OM_PORT_SWITCH_AP2MODEM == ulSwitchMode)
    {
        GU_OamDisconnectPorts(CPM_HSIC_PORT);

        g_stAcpuPcToUeSucRecord.stRlsInfo.ulRlsAp2Modem++;
        g_stAcpuPcToUeSucRecord.stRlsInfo.ulSlice = OM_GetSlice();
    }
    else if (OM_PORT_SWITCH_MODEM2AP == ulSwitchMode)
    {
        GU_OamDisconnectPorts(CPM_APP_PORT);

        g_stAcpuPcToUeSucRecord.stRlsInfo.ulRlsModem2Ap++;
        g_stAcpuPcToUeSucRecord.stRlsInfo.ulSlice = OM_GetSlice();

        CPM_ConnectPorts(CPM_HSIC_PORT, CPM_OM_COMM);
    }
    else
    {
        /*do nothing*/
    }
    return;
}
#endif
/*****************************************************************************
 Prototype       : OM_AcpuAtMsgProc
 Description     : A核上OM处理AT消息处理函数
 Input           : pMsg -- The pointer of the msg.
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2012-03-05
    Author       : h59254
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_AcpuAtMsgProc(MsgBlock *pMsg)
{
    AT_OM_MSG_STRU                     *pstAtOmMsg;

    pstAtOmMsg = (AT_OM_MSG_STRU *)pMsg;

    switch (pstAtOmMsg->ulMsgName)
    {
        case AT_OM_PORT_SWITCH:
            //Om_PortSwitchProc(((OM_PORT_SWITCH_MSG_STRU *)pstAtOmMsg)->ulSwitchMode);
            break;
        case AT_OM_HSIC_PORT_CONNECT:
            PPM_HsicConnectProc();
            break;
        case AT_OM_HSIC_PORT_DISCONNECT:
            PPM_HsicDisconnectProc();
            break;
        default:
            break;
    }

    return;
}

/*****************************************************************************
 Prototype       : OM_AcpuMsgProc
 Description     : Handle all messages sent to ACPU OM.
 Input           : pMsg -- The pointer of the msg.
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_AcpuMsgProc(MsgBlock* pMsg)
{
    APP_OM_MSG_EX_STRU    *pstAppToOmMsg;
    OM_REQUEST_PROCEDURE  *pOmFuncProc;
    VOS_UINT32             ulCurrentId;
    VOS_UINT32             ulOriginalId;

    OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_1, pMsg->ulSenderPid, ACPU_PID_OM, *((VOS_UINT32*)pMsg->aucValue));

    /* 商用Error log 上报 */
    OM_AcpuErrLogMsgProc(pMsg);

    /*处理来自工具侧的消息*/
    if ((ACPU_PID_OM == pMsg->ulSenderPid) || (UEPS_PID_OMRL == pMsg->ulSenderPid))
    {
        pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pMsg->aucValue;
        ulOriginalId = (VOS_UINT32)(pstAppToOmMsg->ucFuncType&OM_FUNCID_VALUE_BITS);

        ulCurrentId = g_astAcpuOmFuncIdProcTbl[ulOriginalId].ulFuncType;
        pOmFuncProc = g_astAcpuOmFuncIdProcTbl[ulOriginalId].pOmFuncProc;

        if ((VOS_NULL_PTR == pOmFuncProc) || (ulOriginalId != ulCurrentId))
        {
            PS_LOG1(ACPU_PID_OM, 0, PS_PRINT_ERROR,
                    "OM_AcpuMsgProc: Error Func Id.\n",
                    (VOS_INT32)ulOriginalId);

            OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

            return;
        }

        if (OM_FUNCID_PART_NUM >= ulOriginalId)
        {
            g_usAcpuToolId = pstAppToOmMsg->usToolId;

            pOmFuncProc((OM_REQ_PACKET_STRU*)pstAppToOmMsg, OM_AcpuSendData);

        }
        /*交由ACPU CallBack任务来处理*/
        else if ((OM_FUNCID_ACPU_PART_NUM < ulOriginalId) && (OM_FUNCID_MAX_NUM > ulOriginalId))
        {
            VOS_ReserveMsg(WUEPS_PID_OM, pMsg);

            pMsg->ulReceiverPid = ACPU_PID_OM_CALLBACK;

            (VOS_VOID)VOS_SendMsg(ACPU_PID_OM_CALLBACK, pMsg);
        }
        else
        {
            PS_LOG1(ACPU_PID_OM, 0, PS_PRINT_ERROR,
                    "OM_AcpuMsgProc: Error Func Id: %d.\n", (VOS_INT32)ulOriginalId);
        }
    }
    else if (VOS_PID_TIMER == pMsg->ulSenderPid)
    {
        OM_AcpuTimerMsgProc(pMsg);
    }
    else if(WUEPS_PID_AT == pMsg->ulSenderPid)
    {
        OM_AcpuAtMsgProc(pMsg);
    }
    else
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_WARNING, "OM_AcpuMsgProc: Send Pid Err.\n");
    }

    OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

    return;
}

/*lint +e416 +e661 +e662 +e717*/
/*****************************************************************************
 Prototype       : OM_AcpuTraceMsgHook
 Description     : OM' hook function
 Input           : pMsg -- The pointer of the msg.
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuTraceMsgHook(VOS_VOID* pMsg)
{
    MsgBlock                    *pNewMsg;
    VOS_UINT32                   ulTraceMsgLen;
    OM_APP_TRACE_STRU_EX         stAppTrace;
    VOS_UINT32                   ulPidConfigFlag;

    ulPidConfigFlag = NO_CONFIG_BIT;

    if (VOS_NULL_PTR == pMsg)
    {
        PS_LOG(ACPU_PID_OM, 0, PS_PRINT_ERROR, "OM_AcpuTraceMsgHook: Input para is NULL.\n");
        return VOS_OK;
    }

    pNewMsg = (MsgBlock*)pMsg;

    /*Check if the current msg is system msg.*/
    if (pNewMsg->ulSenderPid < VOS_PID_DOPRAEND)
    {
        if (0 == g_stAcpuTraceEventConfig.usSysMsg)
        {
            PS_LOG(ACPU_PID_OM, 0, PS_PRINT_INFO, "OM_AcpuTraceMsgHook, System message is filtered.!\n");
            return VOS_OK;
        }
    }
    /*If the current msg is not system msg, we should filter the msgs according the config sent by tools.*/
    else
    {
        if (TRACE_MASK_CONFIG_BIT == OM_AcpuCheckMsgPID(pNewMsg->ulReceiverPid, TRACE_MASK_CONFIG_BIT))
        {
            return VOS_OK;
        }

        ulPidConfigFlag = OM_AcpuCheckMsgPID(pNewMsg->ulSenderPid, TRACE_CONFIG_BIT);

        ulPidConfigFlag |= OM_AcpuCheckMsgPID(pNewMsg->ulReceiverPid, TRACE_CONFIG_BIT);

        if (NO_CONFIG_BIT == ulPidConfigFlag)
        {
            return VOS_OK;
        }
    }

#if 0
    /*Filer the msg by the interface by TTF group.*/
    bResult =  TTF_FilterMsg(pNewMsg);
    if (PS_FALSE == bResult)
    {
        return VOS_OK;
    }
#endif

    /*Construct the msg sent to tool side.*/
    ulTraceMsgLen = OM_APP_TRACE_LEN + VOS_MSG_HEAD_LENGTH + pNewMsg->ulLength;

    stAppTrace.stAppHeader.ucFuncType = OM_TRACE_FUNC;
    stAppTrace.stAppHeader.usLength   = (VOS_UINT16)(ulTraceMsgLen - VOS_OM_HEADER_LEN);

    OM_AcpuAddSNTime(&(stAppTrace.stAppHeader.ulSn),
                    &(stAppTrace.stAppHeader.ulTimeStamp));
    stAppTrace.usPrimId   = OM_APP_TRACE_IND;
    *((VOS_UINT_PTR*)stAppTrace.aucPara)  = (VOS_UINT_PTR)pNewMsg;

    /*Sent to the tool side.*/
    OM_AcpuSendData((OM_RSP_PACKET_STRU*)&stAppTrace, (VOS_UINT16)ulTraceMsgLen);

    return VOS_OK;
}

/*****************************************************************************
 Prototype       : OM_AcpuEvent
 Description     :
 Input           : pMsg -- The pointer of the msg.
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2008-03-20
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuEvent(PS_OM_EVENT_IND_STRU *pstEvent)
{
    VOS_UINT32  ulTotalLen;
    VOS_UINT8  *pucEventPara = VOS_NULL_PTR;

    if (VOS_NULL_PTR == pstEvent)
    {
        PS_LOG( ACPU_PID_OM, 0, PS_PRINT_ERROR," OM_AcpuEvent:Input para is NULL\n");
        return VOS_ERR;
    }

    if (NO_CONFIG_BIT == OM_AcpuCheckMsgPID(pstEvent->ulModuleId, EVENT_CONFIG_BIT))
    {
        PS_LOG( ACPU_PID_OM, 0, PS_PRINT_ERROR," OM_AcpuEvent: Event Module Id is not configed\n");
        return VOS_ERR;
    }

    /*Construct the event msg sent to tool side.*/
    ulTotalLen = (VOS_UINT32)sizeof(OM_APP_HEADER_STRU) + pstEvent->ulLength;
    pucEventPara = (VOS_UINT8 *)VOS_MemAlloc(ACPU_PID_OM, DYNAMIC_MEM_PT, ulTotalLen);

    if (VOS_NULL_PTR == pucEventPara)
    {
        PS_LOG( ACPU_PID_OM, 0, PS_PRINT_ERROR," OM_AcpuEvent:VOS_MemAlloc Failed\n");
        return VOS_ERR;
    }

    OM_AcpuAddAppHeader(OM_EVENT_FUNC, (VOS_UINT8*)pstEvent,
                                 (OM_APP_HEADER_STRU*)pucEventPara, ulTotalLen);

    OM_AcpuSendData((OM_RSP_PACKET_STRU*)pucEventPara, (VOS_UINT16)ulTotalLen);

    VOS_MemFree(ACPU_PID_OM, pucEventPara);

    return VOS_OK;
}

/* 与C核OM接口有冲突，确保ST工程编译通过 */
#if (VOS_WIN32 != VOS_OS_VER)
/*****************************************************************************
 Prototype       : OM_GetSlice
 Description     : 根据当前是否为ASIC版本，提供不同的时间戳
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2008-12-08
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 OM_GetSlice(VOS_VOID)
{
    /* Add for L Only version */
    if (PROTOCOL_TYPE_LTE_ONLY == VOS_GetProtocolType())
    {
        return VOS_GetTick();
    }
    /* Add for L Only version */

#if (VOS_WIN32 != VOS_OS_VER)
    return mdrv_timer_get_normal_timestamp();
#else
    return VOS_GetTick();
#endif
}
#endif

#if (VOS_WIN32 == VOS_OS_VER)
VOS_VOID OM_AcpuRecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_ENUM_UINT32 enNumber)
{
    return;
}
VOS_VOID OM_AcpuRecordInfoStart(VOS_EXC_DUMP_MEM_NUM_ENUM_UINT32 enNumber, VOS_UINT32 ulSendPid, VOS_UINT32 ulRcvPid, VOS_UINT32 ulMsgName)
{
    return;
}
#else

VOS_VOID OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_ENUM_UINT32 enNumber)
{
    VOS_UINT32 *pulBuf;

    if(VOS_EXC_DUMP_MEM_NUM_BUTT <= enNumber)
    {
        return;
    }

    if(VOS_NULL_PTR == g_astAcpuRecordInfo[enNumber].pucBuf)
    {
        return;
    }

    if(0 == g_astAcpuRecordInfo[enNumber].ulLen)
    {
        return;
    }

    /* 在start中已经变更了记录endslice的长度，因此此处回退四个字节填写endslice的值 */
    pulBuf = (VOS_UINT32*)(g_astAcpuRecordInfo[enNumber].pucBuf + g_astAcpuRecordInfo[enNumber].ulLen - sizeof(VOS_UINT32));

    *pulBuf = VOS_GetSlice();

    return;
}

/*****************************************************************************
 Prototype       : OM_RecordInfoStart
 Description     : A核保留桩函数
 Input           : ulNumer -- 任务号
                   ulSendPid -- 发送PID
                   ulRcvPid -- 接收PID
                   ulMsgName -- 消息名称
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2012-07-09
    Author       : s00702770
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_ENUM_UINT32 enNumber, VOS_UINT32 ulSendPid, VOS_UINT32 ulRcvPid, VOS_UINT32 ulMsgName)
{
    OM_RECORD_INFO_STRU     stRecordInfo;

    if(VOS_EXC_DUMP_MEM_NUM_BUTT <= enNumber)
    {
       return;
    }

    if(VOS_NULL_PTR == g_astAcpuRecordInfo[enNumber].pucBuf)
    {
       return;
    }

    g_astAcpuRecordInfo[enNumber].ulLen %= VOS_TASK_DUMP_INFO_SIZE;

    stRecordInfo.usSendPid      = (VOS_UINT16)ulSendPid;
    stRecordInfo.usRcvPid       = (VOS_UINT16)ulRcvPid;
    stRecordInfo.ulMsgName      = ulMsgName;
    stRecordInfo.ulSliceStart   = VOS_GetSlice();
    stRecordInfo.ulSliceEnd     = 0;

    VOS_MemCpy((g_astAcpuRecordInfo[enNumber].pucBuf + g_astAcpuRecordInfo[enNumber].ulLen),
              &stRecordInfo, sizeof(OM_RECORD_INFO_STRU));

    g_astAcpuRecordInfo[enNumber].ulLen += (VOS_UINT32)sizeof(OM_RECORD_INFO_STRU);

    return;
}


VOS_VOID OM_RecordMemInit(VOS_VOID)
{
   VOS_UINT32 i;

   VOS_MemSet(g_astAcpuRecordInfo, 0, sizeof(g_astAcpuRecordInfo));

   /* 分配每个模块记录可谓可测信息的空间 */
   for(i = 0; i < VOS_EXC_DUMP_MEM_NUM_BUTT; i++)
   {
      g_astAcpuRecordInfo[i].pucBuf = (VOS_UINT8*)VOS_ExcDumpMemAlloc(i);

      if(VOS_NULL_PTR == g_astAcpuRecordInfo[i].pucBuf)
      {
          return;
      }
   }

   return;
}

#endif

#if (VOS_OS_VER == VOS_LINUX)

#define NV_APP_HEAD     VOS_UINT16  usNvId;\
                        VOS_UINT16  usLength;\
                        VOS_UINT32  ulResult;

#define NV_APP_HEAD_LEN     8

#define NV_USER_MAX_LEN     (1000)

#define NV_VFILE_CRT_LEVEL  (0660)


typedef struct
{
    NV_APP_HEAD
    VOS_UINT8   aucData[NV_USER_MAX_LEN];
}NV_APP_DATA_STRU;

static ssize_t NV_LinuxUserRead(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    NV_APP_DATA_STRU    stNvData;

    VOS_MemSet(&stNvData, 0, sizeof(stNvData));

    if(len > sizeof(NV_APP_DATA_STRU))
    {
        printk("\r\n NV_LinuxUserRead: The Input Error Len is %d:", (VOS_INT)len);
        return VOS_ERROR;
    }

    if(VOS_OK != copy_from_user(&stNvData, buf, len))   /*拷贝用户空间数据到内核空间上面*/
    {
        printk("\r\n NV_LinuxUserRead: copy_from_user is Error");
        return VOS_ERROR;
    }

    if((stNvData.usLength > NV_USER_MAX_LEN)||(stNvData.usLength == 0))
    {
        printk("\r\n NV_LinuxUserRead: The Data len Error: %d", stNvData.usLength);
        return VOS_ERROR;
    }

    stNvData.ulResult = NV_ReadEx(MODEM_ID_0,stNvData.usNvId, stNvData.aucData, stNvData.usLength);    /*调用 Nv 读取接口*/

    if(NV_OK != stNvData.ulResult)
    {
        printk("\r\n NV_LinuxUserRead: Read Nv Data Error %d", (VOS_INT)stNvData.ulResult);
    }

    if(VOS_OK != copy_to_user(buf, &stNvData, len))
    {
        printk("\r\n NV_LinuxUserRead: copy_to_user is Error");
        return VOS_ERROR;
    }

    return VOS_OK;
}




static ssize_t NV_LinuxUserWrite(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    NV_APP_DATA_STRU    stNvData;

    VOS_MemSet(&stNvData, 0, sizeof(stNvData));

    if(len > sizeof(NV_APP_DATA_STRU))
    {
        printk("\r\n NV_LinuxUserRead: The Input Error Len is %d:", (VOS_INT)len);
        return VOS_ERROR;
    }

    if(VOS_OK != copy_from_user(&stNvData, buf, len))   /*拷贝用户空间数据到内核空间上面*/
    {
        printk("\r\n NV_LinuxUserRead: copy_from_user is Error");
        return VOS_ERROR;
    }

    if((0 == stNvData.usLength)||(stNvData.usLength > NV_USER_MAX_LEN))
    {
        printk("\r\n NV_LinuxUserWrite: NV Length Error %d", stNvData.usLength);
        return VOS_ERROR;
    }

    stNvData.ulResult = NV_WriteEx(MODEM_ID_0,stNvData.usNvId, stNvData.aucData, stNvData.usLength);

    if(VOS_OK != stNvData.ulResult)
    {
        printk("\r\n NV_LinuxUserWrite: NV Write Error %d", (VOS_INT)stNvData.ulResult);
    }
#if 0
    if(VOS_OK != copy_to_user(buf, &stNvData, NV_APP_HEAD_LEN))
    {
        printk("\r\n NV_LinuxUserRead: copy_to_user is Error");
        return VOS_ERROR;
    }
#endif
    return VOS_OK;
}

static const struct file_operations g_stNV_FOPS =
{
    .owner      = THIS_MODULE,
    .write      = NV_LinuxUserWrite,
    .read       = NV_LinuxUserRead,
};

VOS_UINT32 NV_VFInit(VOS_VOID)
{
    if(VOS_NULL_PTR == proc_create("OmNv", NV_VFILE_CRT_LEVEL, VOS_NULL_PTR, &g_stNV_FOPS))
    {
        LogPrint("\r\nNv_VFInit: proc_create Return NULL");

        return VOS_ERR;
    }

    return VOS_OK;
}

#endif

/*****************************************************************************
 函 数 名  : OM_AcpuRegNvInit
 功能描述  : 由于NV模块下移到底软，部分依赖于上层的接口需要协议栈启动后注册。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_OK - 成功
             VOS_ERR - 失败
 修改历史      :
  1.日    期   : 2013年2月17日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 OM_AcpuRegNvInit(VOS_VOID)
{
#if (VOS_WIN32 == VOS_OS_VER)
    /* Init NV's code for reading NV */
    if (VOS_OK != NV_Init())
    {
        return VOS_ERR;
    }
#endif

#if (VOS_OS_VER == VOS_LINUX)
    NV_VFInit();
#endif

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
    DRV_MSP_PROC_REG(OM_WRITE_NV_PROC, (BSP_MspProc)NV_WriteEx);
#endif

    return VOS_OK;
}
VOS_UINT32 OM_AcpuGetLogPath(VOS_CHAR *pucBuf, VOS_CHAR *pucOldPath, VOS_CHAR *pucUnitaryPath)
{
    NV_PRODUCT_TYPE_FOR_LOG_DIRECTORY   stLogDir;

    if (VOS_OK != NV_Read(en_NV_Item_ProductTypeForLogDirectory, &stLogDir, sizeof(stLogDir)))
    {
        /* 读NV失败,直接返回 */
        return VOS_ERR;
    }

    /*0:MBB V3R3 stick/E5,etc.; 1:V3R3 M2M & V7R2; 2:V9R1 phone; 3:K3V3&V8R1;*/
    if ((1 == stLogDir.usProductType) || (3 == stLogDir.usProductType))
    {
        VOS_StrCpy(pucBuf, pucUnitaryPath);
    }
    else
    {
        VOS_StrCpy(pucBuf, pucOldPath);
    }

    return VOS_OK;
}


/*****************************************************************************
 Prototype       : OM_AcpuInit
 Description     : OM' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuInit(VOS_VOID)
{
    /* 原子变量附初始值为0，因为使用时是先加一再返回的 */
#if ( (VOS_LINUX == VOS_OS_VER) || (VOS_WIN32 == VOS_OS_VER) )
    atomic_set(&g_ulAcpuOMMsgSN, 0);
#endif

#if (VOS_WIN32 == VOS_OS_VER)
    OM_RegisterMsgHook(OM_AcpuTraceMsgHook, OM_MSG_HOOK_LTE);
#else
    /* 向OSA挂接钩子函数 */
    VOS_RegisterMsgGetHook(OM_AcpuTraceMsgHook);
#endif

    /* 分配互斥信号量 */
    if (VOS_OK != VOS_SmMCreate("SYNC", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_ulOmAcpuSyncSem))
    {
        return VOS_ERR;
    }

    /* Modified by h59254 for AP-Modem Personalisation Project, 2012/04/12, begin */
    /* 分配互斥信号量 */
    if (VOS_OK != VOS_SmMCreate("HSIC", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_ulOmAcpuHsicSem))
    {
        return VOS_ERR;
    }
    /* Modified by h59254 for AP-Modem Personalisation Project, 2012/04/12, end */

    /* 分配二进制信号量 */
    if (VOS_OK != VOS_SmBCreate( "CNF", 0, VOS_SEMA4_FIFO, &g_ulOmAcpuCnfSem))
    {
        return VOS_ERR;
    }

    /* 创建发送信号量 */
    if (VOS_OK != VOS_SmMCreate("TXCBT", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_ulOmTxCbtSem))
    {
        LogPrint("OM_AcpuInit: Error, VOS_SmMCreate Fail\n");
        return VOS_ERR;
    }

    /* 初始化OM逻辑通道 */
    if (VOS_OK != OMRL_AcpuInit())
    {
        return VOS_ERR;
    }

    /*注册函数给底软使用*/
    OM_AcpuRegisterDrvCB();

    /* 初始化PS配置信息 */
    VOS_MemSet(&g_stAcpuTraceEventConfig, 0, sizeof(OM_TRACE_EVENT_CONFIG_PS_STRU));

    /* 初始化C核复位记录全局变量 */
    VOS_MemSet(g_astCcoreStatus,0,sizeof(g_astCcoreStatus));

    if (VOS_OK != OM_PrintfInit())
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_AcpuInit: OM_Printf_Init failed.\n");
    }

    if(VOS_OK != OM_AcpuStartEventTimer())
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_AcpuInit: OM_AcpuStartEventTimer failed.\n");

        return VOS_ERR;
    }

    /* A核初始化鉴权全局变量 */
    OM_AcpuInitAuthVariable();

    return VOS_OK;
}
VOS_VOID OM_AutoConfigProc(VOS_VOID)
{
    OM_CHANNLE_PORT_CFG_STRU    stPortCfg;
    VOS_UINT8                   aucEstablishReq[] = {0x0a,0x00,0x08,0x00,
                                                     0xb1,0x80,0x00,0x00,
                                                     0x01,0x00,0x00,0x00};
    FILE*                       lCfgFile = DRV_FILE_NULL;
    VOS_UINT8                   aucHeadData[OM_CONFIG_HEAD_SIZE];
    VOS_INT32                   lReadSize;
    VOS_UINT32                  ulMsgLen = 0;
    MsgBlock                    *pstMsgBlk;
    VOS_CHAR                    aucConfigPath[100];

    if (NV_OK != NV_Read(en_NV_Item_Om_Port_Type, &stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU)))
    {
        return;
    }

    /* 判断当前是否为SD卡方式输出 */
    //if ( (CPM_SD_PORT == (stPortCfg.enPortNum + CPM_APP_PORT)) ||
      //  (CPM_FS_PORT == (stPortCfg.enPortNum + CPM_APP_PORT)))
    if ( (CPM_OM_PORT_TYPE_SD == stPortCfg.enPortNum)
       || (CPM_OM_PORT_TYPE_FS == stPortCfg.enPortNum))

    {
        /*SD卡保存Log和FLASH保存Log场景，需要进行配置文件初始化流程*/
    }
    else
    {
        return;
    }

    g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff   = OM_STATE_ACTIVE;

    /* 通知CCPU */
    (VOS_VOID)GU_OamSndPcMsgToCcpu(&g_stAcpuCnfCtrlInfo, (VOS_UINT8*)aucEstablishReq, sizeof(aucEstablishReq));

    if (VOS_OK != OM_AcpuGetLogPath(aucConfigPath, OM_CONFIG_PATH, OM_CONFIG_UNITARY_PATH))
    {
        return;
    }

    /*读取配置文件*/
    lCfgFile = mdrv_file_open(aucConfigPath, "rb");
    if (DRV_FILE_NULL == lCfgFile)
    {
        LogPrint("OM_AutoConfig: The config file is not exsit!\n");
        return;
    }

    mdrv_file_seek(lCfgFile, 0, 0);

    for(;;)
    {
        lReadSize = mdrv_file_read((VOS_CHAR*)aucHeadData, sizeof(VOS_CHAR), OM_CONFIG_HEAD_SIZE, lCfgFile);
        if (OM_CONFIG_HEAD_SIZE != lReadSize)
        {
            break;
        }

        /*获得配置消息的长度, 前3个字节为工具保留字段*/
        VOS_MemCpy(&ulMsgLen, (VOS_UINT8*)(aucHeadData + 3), sizeof(VOS_UINT32));

        /*对消息长度进行合法值判断，由于PC与单板交互的长度字段为2字节,
        所以使用0xffff进行判断*/
        if (ulMsgLen > 0xffff)
        {
            LogPrint1("OM_AutoConfig: The Msg Len is too big : %d!\n", (VOS_INT32)ulMsgLen);
            break;
        }

        pstMsgBlk = VOS_AllocMsg(UEPS_PID_OMRL, ulMsgLen);
        if (VOS_NULL_PTR == pstMsgBlk)
        {
            break;
        }

        if (ulMsgLen != (VOS_UINT32)mdrv_file_read((VOS_CHAR*)(pstMsgBlk->aucValue), sizeof(VOS_CHAR), (VOS_UINT32)ulMsgLen, lCfgFile))
        {
            VOS_FreeMsg(UEPS_PID_OMRL, pstMsgBlk);
            break;
        }

        if (VOS_OK != OMRL_AcpuCpuIdToPid(pstMsgBlk->aucValue[1],&pstMsgBlk->ulReceiverPid))
        {
            VOS_FreeMsg(UEPS_PID_OMRL, pstMsgBlk);
            continue;
        }

        /*将配置消息发送给OM处理*/
        if (VOS_OK != VOS_SendMsg(UEPS_PID_OMRL, pstMsgBlk))
        {
            break;
        }
    }

    mdrv_file_close(lCfgFile);

    return;
}

/*****************************************************************************
 Prototype       : OM_AcpuPidInit
 Description     : ACPU OM PID' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    OM_AUTOCONFIG_REQ_STRU *pstAutoConfigReq;

    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
            return OM_AcpuInit();

        case VOS_IP_INITIAL:
            pstAutoConfigReq = (OM_AUTOCONFIG_REQ_STRU*)VOS_AllocMsg(ACPU_PID_OMAGENT,
                                sizeof(OM_AUTOCONFIG_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

            /* 分配消息失败 */
            if (VOS_NULL_PTR == pstAutoConfigReq)
            {
                return VOS_ERR;
            }

            pstAutoConfigReq->ulReceiverPid = CCPU_PID_OMAGENT;
            pstAutoConfigReq->usPrimId      = OM_AUTOCONFIG_REQ;

            return VOS_SendMsg(ACPU_PID_OMAGENT, pstAutoConfigReq);

        case VOS_IP_RESTART:
            DRV_MSP_PROC_REG (OM_MNTN_ERRLOG, (BSP_MspProc)MNTN_ErrorLog);
            CBTPPM_OamCbtPortInit();
            return COMM_Init();

        default:
            break;
    }

    return VOS_OK;
}



/*****************************************************************************
 Prototype       : OM_AcpuSelfTaskReg
 Description     : ACPU OM FID' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuSelfTaskReg(VOS_VOID)
{
    VOS_UINT32                              ulRslt;
    NAS_NVIM_SYSTEM_APP_CONFIG_STRU         stAPPConfig;


#if ( VOS_WIN32 != VOS_OS_VER )
    ulRslt = VOS_RegisterSelfTaskPrio(ACPU_FID_OM,
                                 (VOS_TASK_ENTRY_TYPE)Log_SelfTask,
                                 COMM_LOG_SELFTASK_PRIO, LOG_SELF_TASK_STACK_SIZE);
    if ( VOS_NULL_BYTE == ulRslt )
    {
        return VOS_ERR;
    }
#endif

    /*读取应用在NV中的配置为1的时候才使能*/
    if(NV_OK != NV_Read(en_NV_Item_System_APP_Config, &stAPPConfig, sizeof(NAS_NVIM_SYSTEM_APP_CONFIG_STRU)))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_AcpuSelfTaskReg:Read NV en_NV_Item_System_APP_Config fail!");

        stAPPConfig.usSysAppConfigType = SYSTEM_APP_BUTT;
    }

    if(SYSTEM_APP_WEBUI== stAPPConfig.usSysAppConfigType)
    {
        vos_printf("\r\nOM_AcpuSelfTaskReg: Reg the OMPrintf Task");

        /* 增加OM_printf模块自处理任务 */
        ulRslt = VOS_RegisterSelfTaskPrio(ACPU_FID_OM,
                                     (VOS_TASK_ENTRY_TYPE)OM_PrintfTask,
                                     COMM_PRINTF_SELFTASK_PRIO, OM_PRINTF_TASK_STACK_SIZE);
        if ( VOS_NULL_BYTE == ulRslt )
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


VOS_INT OM_AcpuCcoreResetCB(DRV_RESET_CB_MOMENT_E enParam, VOS_INT lUserData)
{
    if(MDRV_RESET_CB_BEFORE == enParam) /* 复位前调用*/
    {
        g_astCcoreStatus[OM_CPU_STATUS_RESET].ulResetNum++;
        g_astCcoreStatus[OM_CPU_STATUS_RESET].ulSlice = OM_GetSlice();

        OM_AcpuSendResult(OM_QUERY_FUNC, g_astCcoreStatus[OM_CPU_STATUS_RESET].ulResetNum, OM_APP_CCORE_RESET_CNF);

        SCM_StopAllSrcChan();
    }
    else if(MDRV_RESET_CB_AFTER == enParam) /*复位后*/
    {
        g_astCcoreStatus[OM_CPU_STATUS_OK].ulResetNum++;
        g_astCcoreStatus[OM_CPU_STATUS_OK].ulSlice = OM_GetSlice();
    }
    else
    {
        vos_printf("OM_AcpuCcoreResetCB:param err\r\n");
    }

    return VOS_OK;

}


VOS_VOID Om_ShowCcpuResetInfo(VOS_VOID)
{
    vos_printf("Ccpu Reset Num:     %d \r\n",  g_astCcoreStatus[OM_CPU_STATUS_RESET].ulResetNum);
    vos_printf("Ccpu Reset Time:    0x%x \r\n",  g_astCcoreStatus[OM_CPU_STATUS_RESET].ulSlice);
    vos_printf("Ccpu Ok Num:        %d \r\n",  g_astCcoreStatus[OM_CPU_STATUS_OK].ulResetNum);
    vos_printf("Ccpu Ok Time:       0x%x \r\n",  g_astCcoreStatus[OM_CPU_STATUS_OK].ulSlice);
}

/*****************************************************************************
 Prototype       : OM_AcpuFidInit
 Description     : ACPU OM FID' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 OM_AcpuFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32                          ulRslt;
    VOS_INT                             lResut;

    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
        {
            ulRslt = VOS_RegisterPIDInfo(ACPU_PID_OM,
                                        (Init_Fun_Type)OM_AcpuPidInit,
                                        (Msg_Fun_Type)OM_AcpuMsgProc);
            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            ulRslt = VOS_RegisterPIDInfo(ACPU_PID_OMAGENT,
                                        VOS_NULL_PTR,
                                        (Msg_Fun_Type)OM_AcpuAgentMsgProc);
            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            ulRslt = VOS_RegisterMsgTaskPrio(ACPU_FID_OM, VOS_PRIORITY_M2);
            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            if(VOS_OK != OM_AcpuSelfTaskReg())
            {
                return VOS_ERR;
            }

            /* 注册C核单独复位函数 */
            lResut = mdrv_sysboot_register_reset_notify(OAM_MODELE_NAME,(pdrv_reset_cbfun)OM_AcpuCcoreResetCB,
                VOS_NULL,OAM_CCORE_RESET_CBFUN_PRI);

            if( VOS_OK != lResut )
            {
                vos_printf("OM_AcpuFidInit: Reg Ccore Reset CB FUN Fail\r\n");

                return VOS_ERR;
            }

            /* OM CFG通道数据接收自处理任务创建 */
            if (VOS_OK != SCM_SoftDecodeCfgRcvTaskInit())
            {
                return VOS_ERR;
            }

            break;
        }

        default:
            break;
    }
    return VOS_OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
