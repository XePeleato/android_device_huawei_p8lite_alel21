/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : Omapprl.c
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2008年6月11日
  最近修改   :
  功能描述   : R接口数据转发模块
  函数列表   :
  修改历史   :
  1.日    期   : 2008年6月11日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "omrl.h"
#include "omringbuffer.h"
#include "omprivate.h"
#include "cpm.h"
#include "OmHdlcInterface.h"
#include "OmAppRl.h"
#include "OmApp.h"
#include "omsdlog.h"
#include "OmToolInterface.h"
#include "SCMProc.h"
#include "ombufmngr.h"
#include "AtAppVcomInterface.h"
#include "Omappagent.h"
#include "CbtPpm.h"
#include "SCMSoftDecode.h"
#include "SOCPInterface.h"
#include "product_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -e767 修改人:马昊 00128685;检视人:徐铖 51137;原因:Log打印 */
#define    THIS_FILE_ID        PS_FILE_ID_ACPU_OMRL_C
/*lint +e767 修改人:马昊 00128685;检视人:徐铖 51137; */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* CNF通道控制信息全局变量 */
OMRL_RCV_CHAN_CTRL_INFO_STRU            g_stAcpuCnfCtrlInfo;

/* CBT通道控制信息全局变量 */
OMRL_RCV_CHAN_CTRL_INFO_STRU            g_stAcpuCbtCtrlInfo;

/*记录开始时间*/
VOS_UINT32                              g_ulAcpuLastTick = 0;

/*记录字节长度*/
VOS_UINT32                              g_ulAcpuLastByteCnt = 0;

/* 用来控制OM的打印输出 */
#ifdef OAM_UT_DEBUG
VOS_UINT32                              g_ulOmAcpuDbgFlag = VOS_NULL_DWORD;
#else
VOS_UINT32                              g_ulOmAcpuDbgFlag = VOS_FALSE;
#endif

OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stAcpuTxCnfCtrlInfo;

OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stAcpuTxIndCtrlInfo;

OMRL_CBT_HDLC_ENCODE_MEM_CTRL           g_stCbtHdlcEncBuf;

VOS_UINT32                              g_ulCBTLogEnable = 0;

/* 保存SD卡的Debug 信息 */
extern OM_SD_DEBUG_INFO                 g_stOmSDDebugInfo;
extern OM_VCOM_DEBUG_INFO               g_stVComDebugInfo[3];
extern OM_RECORD_BUF_STRU               g_astAcpuRecordInfo[VOS_EXC_DUMP_MEM_NUM_BUTT];

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 OMRL_AcpuCpuIdToPid(VOS_UINT8 ucCpuId, VOS_UINT32 *pulPid)
{

      switch(ucCpuId)
      {
        case OM_TOOL_RSV_ID:
        case OM_TOOL_CCPU_ID:
            *pulPid = WUEPS_PID_OM;/* ccpu om pid */
            break;
        case OM_TOOL_ACPU_ID:
            *pulPid = ACPU_PID_OM; /* acpu om pid */
            break;
        case OM_TOOL_HIFI_ID:
            *pulPid = DSP_PID_HIFI_OM;/* HIFI om pid */
            break;
        case OM_TOOL_MCU_ID:
            *pulPid = ACPU_PID_MCU_OM;/* MCU om pid */
            break;
        default:
            /* 打印错误 */
            return VOS_ERR;
      }

      return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OMRL_JudgeCbtMsg
 功能描述  : 对Func Type进行判断，是否是CBT相关消息
 输入参数  : ucFuncType:   Func Type

 输出参数  : 无
 返 回 值  : VOS_TRUE/VOS_FALSE
 修改历史  :
   2.日    期   : 2014年1月23日
     作    者   : x51137
     修改内容   : CBT抓取LOG新增
*****************************************************************************/
VOS_BOOL OMRL_JudgeCbtMsg(VOS_UINT8   ucFuncType)
{
    if ( (OM_FS_FUNC == ((ucFuncType) & OM_FUNCID_VALUE_BITS))
                 ||(OM_WRF_FUNC == ((ucFuncType) & OM_FUNCID_VALUE_BITS))
                 ||(OM_GRF_FUNC == ((ucFuncType) & OM_FUNCID_VALUE_BITS))
                 ||(OM_QUERY_FUNC == ((ucFuncType) & OM_FUNCID_VALUE_BITS))
                 ||(OM_NV_FUNC == ((ucFuncType) & OM_FUNCID_VALUE_BITS))
                 ||(OM_NOSIG_FUNC == ((ucFuncType) & OM_FUNCID_VALUE_BITS))
                 ||(OM_CBT_TRANS_FUNC == ((ucFuncType) & OM_FUNCID_VALUE_BITS)) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 OMRL_AcpuMsgDispatch(OMRL_RCV_CHAN_CTRL_INFO_STRU *pstCtrlInfo)
{
    VOS_UINT32                          ulRslt;
    OMRL_MSG_COMBINE_INFO_STRU          *pstCombineInfo;
    APP_OM_MSG_EX_STRU                  *pstAppToOmMsg;

    pstCombineInfo = &(pstCtrlInfo->stMsgCombineInfo);

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)(pstCombineInfo->pstWholeMsg)->aucValue;
    pstAppToOmMsg->ucCpuId = (VOS_UINT8)pstCtrlInfo->enChannel;

    /*lint -e40*/
    OM_ACPU_DEBUG_CHANNEL_TRACE(pstCtrlInfo->enChannel, (pstCombineInfo->pstWholeMsg)->aucValue, (pstCombineInfo->pstWholeMsg)->ulLength, OM_ACPU_DISPATCH_MSG);
    /*lint +e40*/

    if (OM_LOGIC_CHANNEL_CBT == pstCtrlInfo->enChannel)
    {
        /* 取消息中的function type判断是不是属于校准数据 */
        if (VOS_TRUE != OMRL_JudgeCbtMsg(pstAppToOmMsg->ucFuncType))
        {
            /* 释放消息空间 */
            VOS_FreeMsg(ACPU_PID_OM, pstCombineInfo->pstWholeMsg);

            pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

            /* 如果不是校准通道的数据则断掉校准通道的链路 */
            return PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_CBT);
        }

        /* 通过OSA 消息发送给CBT */
        if (WUEPS_PID_OM == (pstCombineInfo->pstWholeMsg)->ulReceiverPid)/* 发送到C核 */
        {
            (pstCombineInfo->pstWholeMsg)->ulReceiverPid = CCPU_PID_CBT;

            pstCtrlInfo->stPcToUeSucRecord.stCcpuData.ulDataLen += (pstCombineInfo->pstWholeMsg)->ulLength;
            pstCtrlInfo->stPcToUeSucRecord.stCcpuData.ulNum++;

            ulRslt = VOS_SendMsg(ACPU_PID_OM, (pstCombineInfo->pstWholeMsg));
        }
        else /* 发送到A核 */
        {
            (pstCombineInfo->pstWholeMsg)->ulReceiverPid = ACPU_PID_CBT;

            pstCtrlInfo->stPcToUeSucRecord.stAcpuData.ulDataLen += (pstCombineInfo->pstWholeMsg)->ulLength;
            pstCtrlInfo->stPcToUeSucRecord.stAcpuData.ulNum++;

            ulRslt = VOS_SendMsg(ACPU_PID_OM, (pstCombineInfo->pstWholeMsg));
        }

        pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

        if (VOS_OK != ulRslt )
        {
            return VOS_ERR;
        }

        return VOS_OK;

    }

    /* CCPU的消息通过ICC通道发送 */
    if (WUEPS_PID_OM == (pstCombineInfo->pstWholeMsg)->ulReceiverPid)
    {
        pstCtrlInfo->stPcToUeSucRecord.stCcpuData.ulDataLen += (pstCombineInfo->pstWholeMsg)->ulLength;
        pstCtrlInfo->stPcToUeSucRecord.stCcpuData.ulNum++;

        ulRslt = GU_OamSndPcMsgToCcpu(pstCtrlInfo, (pstCombineInfo->pstWholeMsg)->aucValue, (pstCombineInfo->pstWholeMsg)->ulLength);

        /* 释放消息空间 */
        VOS_FreeMsg(ACPU_PID_OM, pstCombineInfo->pstWholeMsg);
    }
    else if (ACPU_PID_OM == (pstCombineInfo->pstWholeMsg)->ulReceiverPid)
    {
        /* 其他cpu的消息通过VOS_SendMsg发送 */
        pstCtrlInfo->stPcToUeSucRecord.stAcpuData.ulDataLen += (pstCombineInfo->pstWholeMsg)->ulLength;
        pstCtrlInfo->stPcToUeSucRecord.stAcpuData.ulNum++;

        ulRslt = VOS_SendMsg(ACPU_PID_OM, (pstCombineInfo->pstWholeMsg));
    }
    else if (DSP_PID_HIFI_OM == (pstCombineInfo->pstWholeMsg)->ulReceiverPid)
    {
        /* 其他cpu的消息通过VOS_SendMsg发送 */
        pstCtrlInfo->stPcToUeSucRecord.stHifiData.ulDataLen += (pstCombineInfo->pstWholeMsg)->ulLength;
        pstCtrlInfo->stPcToUeSucRecord.stHifiData.ulNum++;

        ulRslt = VOS_SendMsg(ACPU_PID_OM, (pstCombineInfo->pstWholeMsg));
    }
    else
    {
        /* 其他cpu的消息通过VOS_SendMsg发送 */
        pstCtrlInfo->stPcToUeSucRecord.stMcuData.ulDataLen += (pstCombineInfo->pstWholeMsg)->ulLength;
        pstCtrlInfo->stPcToUeSucRecord.stMcuData.ulNum++;

        ulRslt = VOS_SendMsg(ACPU_PID_OM, (pstCombineInfo->pstWholeMsg));
    }

    pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

    if (VOS_OK != ulRslt )
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 OMRL_AcpuMsgCombine(OMRL_RCV_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucData, VOS_UINT16 usLen)
{
    VOS_UINT32                       ulRslt = VOS_OK;/*Get the return value of PS_SEND_MSG.*/
    VOS_UINT8                        ucCurrentSegSn;/*Reserve the current packet's SN.*/
    OM_APP_MSG_SEGMENT_STRU          *pstOmAppSeg;
    APP_OM_GU_MSG_STRU               *pstAppOmMsg;
    OMRL_MSG_COMBINE_INFO_STRU       *pstCombineInfo;

    pstCombineInfo = &(pstCtrlInfo->stMsgCombineInfo);

    /*Check the input parameter's validity.Make sure empty segment can't pass*/
    if ( (VOS_NULL_PTR == pucData) ||
        (usLen <= sizeof(OM_APP_MSG_SEGMENT_STRU)) )
    {
        LogPrint("OMRL_AcpuMsgCombine: The input is wrong.");

        pstCtrlInfo->stPcToUeErrRecord.usLenTooShortErr++;

        return VOS_ERR;
    }

    pstOmAppSeg = (OM_APP_MSG_SEGMENT_STRU *)pucData;

    /*Check the input parameter's validity.*/
    if (usLen != ((pstOmAppSeg->usSegLen + 3) & ~0x3))
    {
        LogPrint("OMRL_AcpuMsgCombine: The input length is wrong.");

        pstCtrlInfo->stPcToUeErrRecord.usSegLenErr++;

        return VOS_ERR;
    }

    usLen = pstOmAppSeg->usSegLen;

    ucCurrentSegSn = pstOmAppSeg->ucSegSn;

    /*When the MSG packet is new, we need initialize the static variable.*/
    if (1 == ucCurrentSegSn)
    {
        if (VOS_NULL_PTR != pstCombineInfo->pstWholeMsg)
        {
            VOS_FreeMsg(ACPU_PID_OM, pstCombineInfo->pstWholeMsg);
            pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

            pstCtrlInfo->stPcToUeErrRecord.usPacketLostErr++;
        }

        /*The new MSG packet is coming.*/
        pstCombineInfo->ucExpectedSegSn = 1;
        pstCombineInfo->ulMoveLen       = 0;

        if (usLen < (sizeof(APP_OM_GU_MSG_STRU) + OM_APP_MSG_SEGMENT_HEADER_LENGTH))
        {
            LogPrint("OMRL_AcpuMsgCombine: The input parameter is wrong.");

            pstCtrlInfo->stPcToUeErrRecord.usMsgLenErr++;

            return VOS_ERR;
        }

        pstAppOmMsg = (APP_OM_GU_MSG_STRU *)(pucData + OM_APP_MSG_SEGMENT_HEADER_LENGTH);

        pstCombineInfo->ulTotalMsgLen = pstAppOmMsg->usLength + APP_OM_HEADER_LENGTH;

        /* 组包完成后判断是否大于阈值 */
        if (OM_RL_TOTAL_MSG_MAX_LEN < pstCombineInfo->ulTotalMsgLen)
        {
            pstCtrlInfo->stPcToUeErrRecord.usMsgTooLongErr++;

            LogPrint1("OMRL_AcpuMsgCombine: receive first msg pack is too long %d;\r\n",
                        (VOS_INT32)pstCombineInfo->ulTotalMsgLen);

            /* 底软最大支持保存1024字节的内容，底软没有提供宏，这里直接使用数字 */
            VOS_ProtectionReboot(OAM_PC_LENGTH_TOO_BIG, (VOS_INT)pstCombineInfo->ulTotalMsgLen, 0, (VOS_CHAR *)pucData, 1024);

            return VOS_ERR;
        }

        /*Allocate the memory space.*/
        pstCombineInfo->pstWholeMsg = VOS_AllocMsg(ACPU_PID_OM,  pstCombineInfo->ulTotalMsgLen);
        if (VOS_NULL_PTR == pstCombineInfo->pstWholeMsg)
        {
            LogPrint("OMRL_AcpuMsgCombine: VOS_AllocMsg fail.");

            pstCtrlInfo->stPcToUeErrRecord.usAllocMsg++;

            return VOS_ERR;
        }

        /* 根据CPU ID查找到相应的PID，对于工具下发数据帧，用VOS_OM_HEADER结构中保留字段表示CPUID */
        ulRslt = OMRL_AcpuCpuIdToPid(pstAppOmMsg->ucCpuId, &(pstCombineInfo->pstWholeMsg)->ulReceiverPid);
        if (VOS_OK != ulRslt)
        {
            /* 释放消息空间 */
            VOS_FreeMsg(ACPU_PID_OM, pstCombineInfo->pstWholeMsg);

            pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

            pstCtrlInfo->stPcToUeErrRecord.usCpuIdErr++;

            /* 此处错误直接返回，不用记录，函数里面已经记录了CPU ID错误的情况 */
            return VOS_ERR;
        }

        pstCombineInfo->ucTotalSegNum = pstOmAppSeg->ucSegNum;
    }

    if ( pstCombineInfo->ucTotalSegNum != pstOmAppSeg->ucSegNum )
    {
        LogPrint("OMRL_AcpuMsgCombine: The length of the packet is biger than the size of allocated memory.");

        /* 释放消息空间 */
        VOS_FreeMsg(ACPU_PID_OM, pstCombineInfo->pstWholeMsg);

        pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

        pstCtrlInfo->stPcToUeErrRecord.usSegNumErr++;

        return VOS_ERR;
    }

    /*The expected MSG pakcet is lost.*/
    if (pstCombineInfo->ucExpectedSegSn != ucCurrentSegSn)
    {
        /*Print the error info.*/
        LogPrint2("OMRL_AcpuMsgCombine: expected SN is %d, current SN is %d.", pstCombineInfo->ucExpectedSegSn, ucCurrentSegSn);

        /* 释放消息空间 */
        VOS_FreeMsg(ACPU_PID_OM, pstCombineInfo->pstWholeMsg);

        pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

        pstCtrlInfo->stPcToUeErrRecord.usMsgSnErr++;

        return VOS_ERR;
    }

    /*We make sure that the memory can't be violated.*/
    if (pstCombineInfo->ulTotalMsgLen < (pstCombineInfo->ulMoveLen + (usLen - OM_APP_MSG_SEGMENT_HEADER_LENGTH)))
    {
        LogPrint("OMRL_AcpuMsgCombine: The length of the packet is biger than the size of allocated memory.\n");

        /* 释放消息空间 */
        VOS_FreeMsg(ACPU_PID_OM, pstCombineInfo->pstWholeMsg);

        pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

        pstCtrlInfo->stPcToUeErrRecord.usNoMemErr++;

        return VOS_ERR;
    }

    VOS_MemCpy((VOS_UINT8*)((pstCombineInfo->pstWholeMsg)->aucValue) + pstCombineInfo->ulMoveLen,
               pucData + OM_APP_MSG_SEGMENT_HEADER_LENGTH, usLen - OM_APP_MSG_SEGMENT_HEADER_LENGTH);
    pstCombineInfo->ulMoveLen += usLen - OM_APP_MSG_SEGMENT_HEADER_LENGTH;
    pstCombineInfo->ucExpectedSegSn++;

    /*If the current MSG packet is an complete packet.*/
    if (pstCombineInfo->ucTotalSegNum == ucCurrentSegSn)
    {
        /* 组包完成后判断是否大于阈值 */
        if (OM_RL_TOTAL_MSG_MAX_LEN < pstCombineInfo->ulTotalMsgLen)
        {
            pstCtrlInfo->stPcToUeErrRecord.usMsgTooLongErr++;

            /* 释放消息空间 */
            VOS_FreeMsg(ACPU_PID_OM, pstCombineInfo->pstWholeMsg);

            pstCombineInfo->pstWholeMsg = VOS_NULL_PTR;

            return VOS_ERR;
        }

        /*It will send the MSG to OM module.*/
        ulRslt = OMRL_AcpuMsgDispatch(pstCtrlInfo);

        return ulRslt;
    }

    return VOS_OK;
}


VOS_UINT32 OMRL_AcpuSendMsg(VOS_UINT8 *pucData, VOS_UINT32 ulDataLen, VOS_UINT32 ulSenderPid, VOS_UINT32 ulReceiverPid)
{

    MsgBlock           *pstAppOmMsg = VOS_NULL_PTR;

    pstAppOmMsg = VOS_AllocMsg(ulSenderPid, ulDataLen);
    if (VOS_NULL_PTR == pstAppOmMsg)
    {
        return VOS_ERR;
    }

    pstAppOmMsg->ulReceiverPid = ulReceiverPid;

    /* copy 消息内容 */
    VOS_MemCpy((VOS_UINT8 *)pstAppOmMsg->aucValue, pucData, ulDataLen);

    return VOS_SendMsg(ulSenderPid, pstAppOmMsg);

}
VOS_UINT32 OMRL_AcpuHeadProc(OMRL_RCV_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucData, VOS_UINT32 ulSize)
{
    VOS_UINT32                          ulValue;
    VOS_INT                             lRet;
    VOS_UINT8                           aucLinkData[] = {0x55, 0x55, 0xaa, 0xaa, 0x10, 0x00, 0x01, 0x01,
                                                         0x0a, 0x01, 0x08, 0x00, 0xb1, 0x80, 0x00, 0x00,
                                                         0x01, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0x55, 0x55};

    pstCtrlInfo->stPcToUeSucRecord.stTotalData.ulDataLen += ulSize;
    pstCtrlInfo->stPcToUeSucRecord.stTotalData.ulNum++;

    /*lint -e40*/
    OM_ACPU_DEBUG_CHANNEL_TRACE(pstCtrlInfo->enChannel, pucData, ulSize, OM_ACPU_SEND_USB);
    /*lint +e40*/

    if (OM_USB_FRAME_LEN >= ulSize)
    {
        /* 打印错误 */
        LogPrint("OMRL_AcpuHeadProc: The data length is error.");
        pstCtrlInfo->stPcToUeErrRecord.usPacketLenErr++;
        return VOS_ERR;
    }

    VOS_MemCpy(&ulValue, pucData, sizeof(VOS_UINT32));

    /* 判断数据包头是不是0xaaaa5555 */
    if (OM_USB_FRAME_HEAD != ulValue)
    {
        /* 记录错误 */
        pstCtrlInfo->stPcToUeErrRecord.usDataHeadErr++;
        return VOS_ERR;
    }

    VOS_MemCpy(&ulValue, (pucData + ulSize - OM_USB_TAIL_LEN), sizeof(VOS_UINT32));

    /* 判断数据包尾是不是0x5555aaaa */
    if (OM_USB_FRAME_TAIL != ulValue)
    {
        /* 记录错误 */
        pstCtrlInfo->stPcToUeErrRecord.usDataHeadErr++;
        return VOS_ERR;
    }

    /* 收到普通消息时先判断当前链路状态 */
    lRet = VOS_MemCmp(pucData, aucLinkData, ulSize);

    if ((OM_STATE_IDLE == pstCtrlInfo->ulOMSwitchOnOff)
        && (VOS_OK != lRet))
    {
        pstCtrlInfo->stPcToUeErrRecord.usLinkStatusErr++;
        return VOS_ERR;
    }

    /* 数据指针和长度更新*/
    pucData += OM_USB_HEAD_LEN;
    ulSize  -= OM_USB_FRAME_LEN;

    /* 组包 */
    return OMRL_AcpuMsgCombine(pstCtrlInfo, pucData, (VOS_UINT16)ulSize);
}


VOS_UINT32 OMRL_RcvChannelData(OMRL_RCV_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucData, VOS_UINT32 ulSize)
{
    return OMRL_AcpuHeadProc(pstCtrlInfo, pucData, ulSize);
}


VOS_UINT32 OMRL_RcvCbtChannel(SOCP_DECODER_DST_ENUM_U32 enChanlID,VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT8 *pucRBData, VOS_UINT32 ulRBSize)
{
    if ((VOS_NULL_PTR != pucRBData)||(0 != ulRBSize))
    {
        /* 打印错误 */
        LogPrint("OMRL_RcvCbtChannel:para error!\n");
    }

    return OMRL_RcvChannelData(&g_stAcpuCbtCtrlInfo, pucData, ulSize);
}
VOS_UINT32 OMRL_RcvCnfChannel(SOCP_DECODER_DST_ENUM_U32 enChanlID,VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT8 *pucRBData, VOS_UINT32 ulRBSize)
{
    if ((VOS_NULL_PTR != pucRBData)||(0 != ulRBSize))
    {
        /* 打印错误 */
        LogPrint("OMRL_RcvCnfChannel:para error!\n");
    }

    return OMRL_RcvChannelData(&g_stAcpuCnfCtrlInfo, pucData, ulSize);
}

/*****************************************************************************
 Prototype       : OMRL_AcpuConnectInfoReportEvent
 Description     :
 Input           : pstConnectInfo - 指向消息地址
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2014-04-09
    Author       : h59254
    Modification : Created function
 *****************************************************************************/
VOS_VOID OMRL_AcpuConnectInfoReportEvent(OM_CONNECT_INFO_CNF_STRU *pstConnectInfo)
{
    OM_CONNECT_INFO_EVENT_STRU         *pstAppEvent;
    VOS_UINT32                          ulEventLen;

    /* 构造EVENT事件 */
    ulEventLen = sizeof(OM_CONNECT_INFO_EVENT_STRU);

    pstAppEvent = (OM_CONNECT_INFO_EVENT_STRU *)VOS_MemAlloc(ACPU_PID_OM, DYNAMIC_MEM_PT, ulEventLen);

    if (VOS_NULL_PTR == pstAppEvent)
    {
        return;
    }

    pstAppEvent->stAppHeader.ucFuncType = OM_EVENT_FUNC;
    pstAppEvent->stAppHeader.usLength   = (VOS_UINT16)(ulEventLen - VOS_OM_HEADER_LEN);
    pstAppEvent->ulModuleId             = ACPU_PID_OM;
    pstAppEvent->usEventId              = OAM_EVENT_CONNECT_INFO;
    pstAppEvent->ulOMSwitchOnOff        = pstConnectInfo->ulOMSwitchOnOff;

    OM_AcpuAddSNTime(&(pstAppEvent->stAppHeader.ulSn), &(pstAppEvent->stAppHeader.ulTimeStamp));

    VOS_MemCpy(&pstAppEvent->stUe2PcIndErrInfo, &pstConnectInfo->stUe2PcIndErrInfo, sizeof(OM_UE_PC_FAIL_STRU));

    VOS_MemCpy(&pstAppEvent->stUe2PcCnfErrInfo, &pstConnectInfo->stUe2PcCnfErrInfo, sizeof(OM_UE_PC_FAIL_STRU));

    VOS_MemCpy(pstAppEvent->aucPidCfg, pstConnectInfo->aucPidCfg, sizeof(VOS_UINT32) * (VOS_CPU_ID_0_PID_BUTT - VOS_PID_CPU_ID_0_DOPRAEND));

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
    BSP_SOCP_GetEncSrcReg(pstAppEvent->aucReg, OM_MAX_SOCP_REG_NUM);
#endif

    if (VOS_OK != OM_AcpuSendData((OM_RSP_PACKET_STRU *)pstAppEvent, (VOS_UINT16)ulEventLen))
    {
        LogPrint("OMRL_AcpuConnectInfoReportEvent:OM_AcpuSendData Failed!\n");
    }

    /* 释放消息内容空间 */
    VOS_MemFree(ACPU_PID_OM, pstAppEvent);

    return;
}


VOS_UINT32 OMRL_AcpuReportEvent(VOS_VOID)
{
#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
    PS_OM_EVENT_IND_STRU    *pstEventInd;
    VOS_UINT32               ulEventLenth;
    VOS_UINT32               ulTemp = 0x5a5a5a5a;
    VOS_UINT32               ulIndex;
    VOS_UINT32               ulSendLen;
    OM_SEND_DEBUG_INFO_STRU  astAcpuDebuginfoTab2[3];
    VOS_UINT_PTR             ulAddr;
    VOS_UINT16               usDebugEventID;

    /* 存放A核调测信息的 大小和地址,分包上报消息 */
    OM_SEND_DEBUG_INFO_STRU  astAcpuDbuginfoTab1[] = {
        {sizeof(g_stAcpuCnfCtrlInfo.stPcToUeSucRecord),                 (VOS_UINT8*)&g_stAcpuCnfCtrlInfo.stPcToUeSucRecord},
        {sizeof(g_stAcpuCbtCtrlInfo.stPcToUeSucRecord),                 (VOS_UINT8*)&g_stAcpuCbtCtrlInfo.stPcToUeSucRecord},
        {sizeof(g_stAcpuTxCnfCtrlInfo.stUeToPcSucRecord),               (VOS_UINT8*)&g_stAcpuTxCnfCtrlInfo.stUeToPcSucRecord},
        {sizeof(g_stAcpuTxIndCtrlInfo.stUeToPcSucRecord),               (VOS_UINT8*)&g_stAcpuTxIndCtrlInfo.stUeToPcSucRecord},
        {sizeof(g_stAcpuDebugInfo),                       (VOS_UINT8*)&g_stAcpuDebugInfo      },
        {sizeof(g_stOmSDDebugInfo),                       (VOS_UINT8*)&g_stOmSDDebugInfo      },
        {sizeof(g_stVComDebugInfo),                       (VOS_UINT8*)&g_stVComDebugInfo[0]   },
        {sizeof(g_astAcpuRecordInfo),                     (VOS_UINT8*)&g_astAcpuRecordInfo[0]    },
        {sizeof(g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff),     (VOS_UINT8*)&g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff  },
#ifdef SCM_SNCHECK
        {sizeof(VOS_UINT32),                     (VOS_UINT8*)&g_ulSnErrCount    },
#endif
    };

    astAcpuDebuginfoTab2[0].pucAddr = (VOS_UINT8*)SCM_GetDebugLogInfo();
    astAcpuDebuginfoTab2[0].ulLenth = SCM_GetDebugLogInfoLen();

    astAcpuDebuginfoTab2[1].pucAddr = (VOS_UINT8*)SOCP_GetDebugLogInfo(OM_GET_SOCP_STATUS);
    astAcpuDebuginfoTab2[1].ulLenth = SOCP_GetDebugLogInfoLen(OM_GET_SOCP_STATUS);

    astAcpuDebuginfoTab2[2].pucAddr = (VOS_UINT8*)SOCP_GetDebugLogInfo(OM_GET_SOCP_DEBUG_INFO);
    astAcpuDebuginfoTab2[2].ulLenth = SOCP_GetDebugLogInfoLen(OM_GET_SOCP_DEBUG_INFO);

    /* ---------------第一包数据 astAcpuDbuginfoTab1[]  begin----------- */

    ulEventLenth = (sizeof(PS_OM_EVENT_IND_STRU)-(4 * sizeof(VOS_UINT8)));
    for (ulIndex=0; ulIndex<(sizeof(astAcpuDbuginfoTab1)/sizeof(astAcpuDbuginfoTab1[0])); ulIndex ++)
    {
        ulEventLenth += astAcpuDbuginfoTab1[ulIndex].ulLenth;
    }

    /* 加上分隔符号长度 */
    ulEventLenth += (ulIndex - 1) * (VOS_UINT32)sizeof(VOS_UINT32);
    pstEventInd = (PS_OM_EVENT_IND_STRU *)VOS_MemAlloc(ACPU_PID_OM, DYNAMIC_MEM_PT, ulEventLenth);
    if (VOS_NULL_PTR == pstEventInd)
    {
        LogPrint("OMRL_AcpuReportEvent:Alloc Memory Failed!\n");
        return VOS_ERR;
    }

    pstEventInd->ulLength   = ulEventLenth;
    pstEventInd->usEventId  = OAM_DEGUG_EVENT_ID1;
    pstEventInd->ulModuleId = ACPU_PID_OM;

    /*lint -e419 -e416 */
    /* pc侧到ue侧时正常接收数据信息 */
    VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData,
        (VOS_UINT8*)&g_stAcpuCnfCtrlInfo.stPcToUeSucRecord,
        sizeof(OM_ACPU_PC_UE_SUC_STRU));
     /*lint +e419 +e416 */

    ulEventLenth = 0;
    for (ulIndex=1;ulIndex<(sizeof(astAcpuDbuginfoTab1)/sizeof(astAcpuDbuginfoTab1[0])); ulIndex ++)
    {
        /*lint -e419 -e416 */
        /* 加上分隔符号 */
        ulEventLenth += astAcpuDbuginfoTab1[ulIndex-1].ulLenth;
        VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData + ulEventLenth,
                   (VOS_UINT8 *)&ulTemp, sizeof(VOS_UINT32));

        ulEventLenth += (VOS_UINT32)sizeof(VOS_UINT32);
        VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData + ulEventLenth,
            astAcpuDbuginfoTab1[ulIndex].pucAddr, astAcpuDbuginfoTab1[ulIndex].ulLenth);
         /*lint +e419 +e416 */
    }
    /*lint +e419 +e416 */

    if (VOS_OK != OM_AcpuEvent(pstEventInd))
    {
        PS_LOG(ACPU_PID_OM, VOS_NULL, PS_PRINT_WARNING, "OMRL_AcpuReportEvent: send event fail.");
    }
    /* 释放消息内容空间 */
    VOS_MemFree(ACPU_PID_OM, pstEventInd);

    /* ---------------第一包数据 astAcpuDbuginfoTab1[]  end----------- */

    /* ---------------astAcpuDbuginfoTab2[] 每项数据较大拆包发送，每包最大发送2k----------- */
    /* Event Id */
    usDebugEventID  = OAM_DEGUG_EVENT_ID2;
    /* 一次性申请 2k 空间，最后释放 */
    pstEventInd = (PS_OM_EVENT_IND_STRU *)VOS_MemAlloc(ACPU_PID_OM, DYNAMIC_MEM_PT,
                                          OM_SEND_DEBUG_INFO_MAX_LEN + sizeof(PS_OM_EVENT_IND_STRU)-(4 * sizeof(VOS_UINT8)));
    if (VOS_NULL_PTR == pstEventInd)
    {
        LogPrint("OMRL_AcpuReportEvent:Alloc Memory Failed!\n");
        return VOS_ERR;
    }

    /* 数据拆分发送 */
    for(ulIndex =0; ulIndex<(sizeof(astAcpuDebuginfoTab2)/sizeof(astAcpuDebuginfoTab2[0])); ulIndex++)
    {
        /* 分配内容空间 最大为 2k*/
        ulSendLen = 0;

        while(ulSendLen < astAcpuDebuginfoTab2[ulIndex].ulLenth)
        {
            if((astAcpuDebuginfoTab2[ulIndex].ulLenth - ulSendLen) > OM_SEND_DEBUG_INFO_MAX_LEN)
            {
                ulEventLenth = OM_SEND_DEBUG_INFO_MAX_LEN;
            }
            else
            {
                ulEventLenth = astAcpuDebuginfoTab2[ulIndex].ulLenth - ulSendLen;
            }

            pstEventInd->ulLength   = ulEventLenth;
            pstEventInd->usEventId  = usDebugEventID++;
            pstEventInd->ulModuleId = ACPU_PID_OM;

            /* make pclint happy  begin */
            ulAddr = (VOS_UINT_PTR)pstEventInd->aucData;
            VOS_MemCpy((VOS_UINT8 *)ulAddr, astAcpuDebuginfoTab2[ulIndex].pucAddr + ulSendLen, ulEventLenth);
            /* make pclint happy  end */

            ulSendLen += ulEventLenth;

            if (VOS_OK != OM_AcpuEvent(pstEventInd))
            {
                PS_LOG(ACPU_PID_OM, VOS_NULL, PS_PRINT_WARNING, "OMRL_AcpuReportEvent: send event fail.");
            }
        }
    }

    /* 释放消息内容空间 */
    VOS_MemFree(ACPU_PID_OM, pstEventInd);
#endif
    return VOS_OK;
}


VOS_UINT32 OMRL_AcpuSendCbtData(VOS_UINT8  *pucSrc, VOS_UINT16  usSrcLen)
{
    VOS_UINT16                         usHdlcEncLen;
    VOS_UINT32                         ulResult;

    /*进行互斥操作*/
    if ( VOS_OK != VOS_SmP(g_ulOmTxCbtSem, OM_PV_TIMEOUT) )
    {
        LogPrint("OMRL_AcpuMsgSplit, Error, OmTxBuffSem VOS_SmP Failed.\n");

        return VOS_ERR;
    }

    /* 做HDLC编码 */
    if ( VOS_OK != Om_HdlcEncap(pucSrc,
                                usSrcLen,
                                g_stCbtHdlcEncBuf.pucBuf,
                                (VOS_UINT16)g_stCbtHdlcEncBuf.ulBufSize,
                                &usHdlcEncLen) )
    {
        VOS_SmV(g_ulOmTxCbtSem);

        return VOS_ERR;
    }

    ulResult = CBTPPM_OamCbtPortDataSnd(g_stCbtHdlcEncBuf.pucBuf, g_stCbtHdlcEncBuf.pucRealBuf,usHdlcEncLen);

    VOS_SmV(g_ulOmTxCbtSem);

    return ulResult;
}

/*****************************************************************************
 函 数 名  : OMRL_AcpuReleaseSocpData
 功能描述  : 释放OM已发送数据内存
 输入参数  : VOS_VOID

 输出参数  : 无

 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年07月31日
    作    者   : g47350
    修改内容   : V900R001项目新增函数

*****************************************************************************/
VOS_UINT32 OMRL_AcpuReleaseSocpData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo)
{
    VOS_UINT_PTR                        DataAddr;
    VOS_UINT32                          ulDataLen;
    VOS_UINT8                           *pucRealAddr;
    SOCP_CODER_SRC_ENUM_U32             enChanlID;
    OM_BUF_CTRL_STRU                    *pstCtrlBuf = VOS_NULL_PTR;

    enChanlID = pstCtrlInfo->enChanlID;
    pstCtrlBuf = &(pstCtrlInfo->OmBufCtrl);

#if 0

    VOS_BOOL            bLoopFlag = VOS_TRUE;

    /* 为了测试充分，这里先按一个一个RD释放 */
    do
    {
        /* 从SCM获得已发送消息总长度 */
        if (VOS_OK != SCM_RlsSrcRDFirst(SOCP_CODER_SRC_GU_OM1, &ulDataAddr, &ulDataLen))
        {
            g_stAcpuUeToPcErrRecord.usRlsRdErr++;

            return VOS_ERR;
        }

        /* 已经没有RD */
        if ((0 == ulDataAddr) || (0 == ulDataLen))
        {
            break;
        }

        pucRealAddr = (VOS_UINT8*)OM_AddrRealToVirt(&g_stAcpuOmBufCtrl, (VOS_UINT8*)ulDataAddr);

        if (VOS_NULL_PTR == pucRealAddr)
        {
            g_stAcpuUeToPcErrRecord.usRlsAddrErr++;

            return VOS_ERR;
        }

        /* 释放OM缓冲区内存 */
        if (VOS_OK != OM_ReleaseTraceMem(&g_stAcpuOmBufCtrl, pucRealAddr, (VOS_INT32)ulDataLen))
        {
            g_stAcpuUeToPcErrRecord.usRlsMemErr++;

            return VOS_ERR;
        }

    }while(bLoopFlag);

#else
    /* 清空所有已发送RD */
    if (VOS_OK != SCM_RlsSrcRDAll(enChanlID, &DataAddr, &ulDataLen))
    {
        pstCtrlInfo->stUeToPcErrRecord.usRlsRdErr++;

        return VOS_ERR;
    }

    /* 已经没有RD */
    if ((0 == DataAddr) || (0 == ulDataLen))
    {
        return VOS_OK;
    }

    pstCtrlInfo->stUeToPcSucRecord.stRealRlsData.ulDataLen += ulDataLen;
    pstCtrlInfo->stUeToPcSucRecord.stRealRlsData.ulNum++;

    pucRealAddr = (VOS_UINT8*)OM_AddrRealToVirt(pstCtrlBuf, (VOS_UINT8*)DataAddr);

    if (VOS_NULL_PTR == pucRealAddr)
    {
        pstCtrlInfo->stUeToPcErrRecord.usRlsAddrErr++;

        return VOS_ERR;
    }

    /* 根据已发送RD总长度释放内存 */
    if (VOS_OK != OM_ReleaseTraceMem(pstCtrlBuf, pucRealAddr, (VOS_INT32)ulDataLen))
    {
        pstCtrlInfo->stUeToPcErrRecord.usRlsMemErr++;

        g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OMRL_AcpuSendSocpData
 功能描述  : 将发送数据挂接到SOCP.
 输入参数  : pucHead   : 数据分包头指针
             ulHeadLen : 数据分包头长度
             pucData   : 数据体指针
             ulDataLen : 数据体长度

 输出参数  : 无

 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年07月31日
    作    者   : g47350
    修改内容   : V900R001项目新增函数

*****************************************************************************/
VOS_UINT32 OMRL_AcpuSendSocpData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucHead, VOS_UINT32 ulHeadLen,
                            VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    VOS_UINT8                          *pucMemAddr;
    VOS_INT32                           lUsedBytes;
    VOS_UINT8                          *pucRealMemAddr;
    SOCP_CODER_SRC_ENUM_U32             enChanlID;
    OM_BUF_CTRL_STRU                   *pstCtrlBuf = VOS_NULL_PTR;

    enChanlID = pstCtrlInfo->enChanlID;
    pstCtrlBuf = &(pstCtrlInfo->OmBufCtrl);

    pucMemAddr = (VOS_UINT8*)OM_AllocTraceMem(pstCtrlBuf, (VOS_INT32)(ulHeadLen + ulDataLen + sizeof(VOS_UINT32)));

    if (VOS_NULL_PTR == pucMemAddr)
    {
        pstCtrlInfo->stUeToPcErrRecord.usNoMemErr++;

        return VOS_ERR;
    }

    /* 拷贝数据头 */
    VOS_MemCpy(pucMemAddr, pucHead, ulHeadLen);

    /* 拷贝数据内容 */
    VOS_MemCpy(pucMemAddr + ulHeadLen, pucData, ulDataLen);

    /* 填写桢尾 */
    *(VOS_UINT32*)(pucMemAddr + ulHeadLen + ulDataLen) = OM_USB_FRAME_TAIL;

    VOS_FlushCpuWriteBuf();

    pucRealMemAddr = (VOS_UINT8*)OM_AddrVirtToReal(pstCtrlBuf, pucMemAddr);

    if (VOS_NULL_PTR == pucRealMemAddr)
    {
        pstCtrlInfo->stUeToPcErrRecord.usSendAddrErr++;

        return VOS_ERR;
    }

    /* 调用SCM提供的接口将数据发送给SOCP */
    if (VOS_OK != SCM_SendCoderSrc(enChanlID, pucRealMemAddr, ulHeadLen + ulDataLen + (VOS_UINT32)sizeof(VOS_UINT32)))
    {
        pstCtrlInfo->stUeToPcErrRecord.usSendSocpDataErr++;

        return VOS_ERR;
    }

    pstCtrlInfo->stUeToPcSucRecord.stRealSendData.ulDataLen += (ulHeadLen + ulDataLen + (VOS_UINT32)sizeof(VOS_UINT32));
    pstCtrlInfo->stUeToPcSucRecord.stRealSendData.ulNum++;

    lUsedBytes = OM_TraceMemNBytes(pstCtrlBuf);

    if (pstCtrlInfo->stUeToPcSucRecord.ulMaxUsedBufSize < (VOS_UINT32)lUsedBytes)
    {
        pstCtrlInfo->stUeToPcSucRecord.ulMaxUsedBufSize = (VOS_UINT32)lUsedBytes;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OMRL_AcpuIsBDEnough
 功能描述  : 判断剩余BD是否满足.
 输入参数  : ucMsgCnt -- 发送消息分包个数

 输出参数  : 无

 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年07月31日
    作    者   : g47350
    修改内容   : V900R001项目新增函数

*****************************************************************************/
VOS_UINT32 OMRL_AcpuIsBDEnough(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 ucMsgCnt)
{
    VOS_UINT32                          ulFreeBdNum = 0;
    SOCP_CODER_SRC_ENUM_U32             enChanlID;

    enChanlID = pstCtrlInfo->enChanlID;

    /* 获取OM源通道剩余BD个数 */
    if (VOS_OK != SCM_GetBDFreeNum(enChanlID, &ulFreeBdNum))
    {
        pstCtrlInfo->stUeToPcErrRecord.usGetBDErr++;

        return VOS_ERR;
    }

    /* 判断剩余BD是否满足分包个数 */
    if (ucMsgCnt > ulFreeBdNum)
    {
        pstCtrlInfo->stUeToPcErrRecord.usBDLackErr++;

        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OMRL_ProcessSendData
 功能描述  : 将可维可测数据发送的SOCP模块
 输入参数  : pucMsg    -- 指向发送数据的指针
             usMsgLen  -- 发送数据的长度
             ucMsgCnt  -- 待发送数据包个数
 输出参数  : 无
 返 回 值  : VOS_UINT32

 修改历史      :
  1.日    期   : 2009年3月28日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OMRL_AcpuProcessSendData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucMsg, VOS_UINT8 ucMsgCnt, VOS_UINT16 usMsgLen)
{
    VOS_UINT8                           ucCurSegNum = 1; /*当前段序号*/
    VOS_UINT_PTR                        ulTempAddress;
    VOS_BOOL                            bIsHead     = VOS_FALSE;
    OM_APP_TRACE_STRU_EX               *pstAppTrace;
    /*指向OM分包头内容的指针*/
    OM_APP_MSG_SEG_EX_STRU             *pstSegMsgEx = VOS_NULL_PTR;

    /*分配分包结构的内存空间*/
    pstSegMsgEx = (OM_APP_MSG_SEG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM,
                DYNAMIC_MEM_PT, sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN);

    if (VOS_NULL_PTR == pstSegMsgEx)
    {
        LogPrint("OM_UsbFrameInit:VOS_MemAlloc g_pstSegMsgEx Failed!\n");
        return VOS_ERR;
    }

    pstSegMsgEx->stSocpHdr.ucCPUID   = OM_TOOL_ACPU_ID;
    pstSegMsgEx->ulFrameHead         = OM_USB_FRAME_HEAD;

    pstAppTrace = (OM_APP_TRACE_STRU_EX*)pucMsg;

    ulTempAddress = (VOS_UINT_PTR)pucMsg;

    if ((OM_TRACE_FUNC == pstAppTrace->stAppHeader.ucFuncType)
        &&(OM_APP_TRACE_IND == pstAppTrace->usPrimId))
    {
        ulTempAddress = *(VOS_UINT_PTR*)(pstAppTrace->aucPara);
        VOS_MemCpy(pstSegMsgEx + 1, pucMsg, OM_APP_TRACE_LEN);
        bIsHead = VOS_TRUE;
    }

    /* 填充SOCP头中的RTC值 */
    pstSegMsgEx->stSocpHdr.ulRTC = (VOS_UINT32)mdrv_rtc_get_value();
    pstSegMsgEx->stMsgSeg.ucSegNum = ucMsgCnt;
    pstSegMsgEx->stMsgSeg.usSegLen = OM_APP_SPLIT_MSG_LEN;

    /* 大于最大分包大小的数据，按照最大分包大小进行数据发送的处理 */
    for (ucCurSegNum = 1; ucCurSegNum < ucMsgCnt; ucCurSegNum++)
    {
        pstSegMsgEx->stMsgSeg.ucSegSn = ucCurSegNum;

        if (VOS_TRUE == bIsHead)
        {
            /* 消息头前加上长度信并放入环形缓冲区*/
            if (VOS_OK != OMRL_AcpuSendSocpData(pstCtrlInfo, (VOS_UINT8 *)pstSegMsgEx,
                (sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN),
                (VOS_UINT8*)ulTempAddress, OM_APP_MSG_SEGMENT_LEN - OM_APP_TRACE_LEN))
            {
                VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

                return VOS_ERR;
            }

            bIsHead       =  VOS_FALSE;
            ulTempAddress += (OM_APP_MSG_SEGMENT_LEN - OM_APP_TRACE_LEN);
        }
        else
        {
            /* 消息头前加上长度信并放入环形缓冲区*/
            if (VOS_OK != OMRL_AcpuSendSocpData(pstCtrlInfo, (VOS_UINT8 *)pstSegMsgEx,
                 sizeof(OM_APP_MSG_SEG_EX_STRU),
                (VOS_UINT8*)ulTempAddress, OM_APP_MSG_SEGMENT_LEN))
            {
                VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

                return VOS_ERR;
            }

            ulTempAddress += OM_APP_MSG_SEGMENT_LEN;
        }

        /* 计算剩余数据包大小 */
        usMsgLen -= OM_APP_MSG_SEGMENT_LEN;
    }

    /*设置消息头参数*/
    pstSegMsgEx->stMsgSeg.usSegLen = (VOS_UINT16)(usMsgLen + OM_APP_MSG_SEGMENT_HEADER_LENGTH);
    pstSegMsgEx->stMsgSeg.ucSegSn  = ucCurSegNum;

    /*为了保持Ringbuffer中存放的数据包4字节对齐*/
    usMsgLen = (usMsgLen + 3U) & ~0x03U;

    if (VOS_TRUE == bIsHead)
    {
        /* 消息头前加上长度并放入环形缓冲区 */
        if ( VOS_OK != OMRL_AcpuSendSocpData(pstCtrlInfo, (VOS_UINT8 *)pstSegMsgEx,
                            (sizeof(OM_APP_MSG_SEG_EX_STRU) + OM_APP_TRACE_LEN),
                            (VOS_UINT8*)ulTempAddress,
                            usMsgLen - OM_APP_TRACE_LEN))
        {
            VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

            return VOS_ERR;
        }
    }
    else
    {
        /* 消息头前加上长度并放入环形缓冲区 */
        if ( VOS_OK != OMRL_AcpuSendSocpData(pstCtrlInfo, (VOS_UINT8 *)pstSegMsgEx,
                                        sizeof(OM_APP_MSG_SEG_EX_STRU),
                                        (VOS_UINT8*)ulTempAddress,
                                        usMsgLen))
        {
            VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

            return VOS_ERR;
        }
    }

    VOS_MemFree(WUEPS_PID_OM, pstSegMsgEx);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OMRL_AcpuMsgSplit
 功能描述  : 当在USB上进行通信时，使用的扩展分包函数
 输入参数  : pstCtrlInfo -- 指向IND或CNF通道的控制变量
             pucMsg    -- 指向发送数据的指针
             usMsgLen  -- 发送数据的长度
 输出参数  : 无
 返 回 值  : VOS_UINT32

 修改历史      :
  1.日    期   : 2009年3月28日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OMRL_AcpuMsgSplit(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucMsg, VOS_UINT16 usMsgLen)
{
    VOS_UINT8                           ucMsgCnt    = 1; /*分段的数量*/
    VOS_UINT32                          ulRet;

    /*进行互斥操作*/
    if ( VOS_OK != VOS_SmP(pstCtrlInfo->OmTxBuffSem, OM_PV_TIMEOUT) )
    {
        LogPrint("OMRL_AcpuMsgSplit, Error, OmTxBuffSem VOS_SmP Failed.\n");

        pstCtrlInfo->stUeToPcErrRecord.usSemErr++;
        return VOS_ERR;
    }

    /* 释放已发送数据内存 */
    if (VOS_OK != OMRL_AcpuReleaseSocpData(pstCtrlInfo))
    {
        VOS_SmV(pstCtrlInfo->OmTxBuffSem);
        return VOS_ERR;
    }

    /* 计算分包个数 */
    ucMsgCnt = (VOS_UINT8)(((usMsgLen + OM_APP_MSG_SEGMENT_LEN) - 1)/OM_APP_MSG_SEGMENT_LEN);

    /* 获取OM源通道剩余BD个数 */
    if (VOS_OK != OMRL_AcpuIsBDEnough(pstCtrlInfo, ucMsgCnt))
    {
        VOS_SmV(pstCtrlInfo->OmTxBuffSem);
        return VOS_ERR;
    }

    ulRet = OMRL_AcpuProcessSendData(pstCtrlInfo, pucMsg, ucMsgCnt, usMsgLen);

    VOS_SmV(pstCtrlInfo->OmTxBuffSem);

    return ulRet;
}

/*****************************************************************************
 函 数 名  : OMRL_AcpuUsbFrameInit
 功能描述  : 当为虚拟串口时，需要初始化缓冲区
 输入参数  : VOS_VOID
 输出参数  : VOS_VOID
 返 回 值  : VOS_UINT32
 修改历史  :
   1.日    期  : 2009年3月28日
     作    者  : g47350
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 OMRL_AcpuUsbFrameInit(VOS_VOID)
{
    VOS_UINT_PTR    ulRealAddr;

    VOS_MemSet(&g_stAcpuTxCnfCtrlInfo, 0, sizeof(g_stAcpuTxCnfCtrlInfo));

    VOS_MemSet(&g_stAcpuTxIndCtrlInfo, 0, sizeof(g_stAcpuTxIndCtrlInfo));

    g_stAcpuTxCnfCtrlInfo.enChanlID = SOCP_CODER_SRC_GU_CNF1;

    g_stAcpuTxCnfCtrlInfo.enChannel = OM_LOGIC_CHANNEL_CNF;

    g_stAcpuTxIndCtrlInfo.enChanlID = SOCP_CODER_SRC_GU_IND1;

    g_stAcpuTxIndCtrlInfo.enChannel = OM_LOGIC_CHANNEL_IND;

    /* 创建OM CNF发送缓冲区 */
    if (VOS_OK != OM_CreateTraceBuffer(&(g_stAcpuTxCnfCtrlInfo.OmBufCtrl), OM_CNF_BUFFER_SIZE))
    {
        LogPrint("OMRL_AcpuUsbFrameInit:OM_CreateTraceBuffer Failed!\n");

        return VOS_ERR;
    }

    /* 创建OM IND发送缓冲区 */
    if (VOS_OK != OM_CreateTraceBuffer(&(g_stAcpuTxIndCtrlInfo.OmBufCtrl), OM_UPLINK_BUFFER_SIZE))
    {
        LogPrint("OMRL_AcpuUsbFrameInit:OM_CreateTraceBuffer Failed!\n");

        return VOS_ERR;
    }

    /* 创建发送信号量 */
    if (VOS_OK != VOS_SmMCreate("TXCNF", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_stAcpuTxCnfCtrlInfo.OmTxBuffSem))
    {
        LogPrint("OMRL_AcpuUsbFrameInit: Error, VOS_SmMCreate Fail\n");
        return VOS_ERR;
    }

    /* 创建发送信号量 */
    if (VOS_OK != VOS_SmMCreate("TXIND", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_stAcpuTxIndCtrlInfo.OmTxBuffSem))
    {
        LogPrint("OMRL_AcpuUsbFrameInit: Error, VOS_SmMCreate Fail\n");
        return VOS_ERR;
    }

    /* 申请CBT通道HDLC编码使用的uncached memory */
    VOS_MemSet(&g_stCbtHdlcEncBuf, 0, sizeof(g_stCbtHdlcEncBuf));

    g_stCbtHdlcEncBuf.pucBuf = (VOS_UINT8 *)VOS_UnCacheMemAlloc(2 * OM_HDLC_BUF_MAX_LEN, &ulRealAddr);
    if (VOS_NULL_PTR == g_stCbtHdlcEncBuf.pucBuf)
    {
        LogPrint("OMRL_AcpuUsbFrameInit: VOS_UnCacheMemAlloc failed \n");
        return VOS_ERR;
    }

    /* 保存buf实地址 */
    g_stCbtHdlcEncBuf.pucRealBuf = (VOS_UINT8 *)ulRealAddr;
    g_stCbtHdlcEncBuf.ulBufSize  = 2 * OM_HDLC_BUF_MAX_LEN;

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OmAcpuInitVar
 功能描述  : 初始化记录错误信息的全局变量
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2009年5月18日
     作    者  : g47350
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OmAcpuInitVar(VOS_VOID)
{
    VOS_MemSet(&g_stAcpuCnfCtrlInfo, 0, sizeof(g_stAcpuCnfCtrlInfo));

    /* 首包序号为1 */
    g_stAcpuCnfCtrlInfo.stMsgCombineInfo.ucExpectedSegSn = 1;
    g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;
    g_stAcpuCnfCtrlInfo.enChannel = OM_LOGIC_CHANNEL_CNF;

    VOS_MemSet(&g_stAcpuCbtCtrlInfo, 0, sizeof(g_stAcpuCbtCtrlInfo));

    /* 首包序号为1 */
    g_stAcpuCbtCtrlInfo.stMsgCombineInfo.ucExpectedSegSn = 1;
    g_stAcpuCbtCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;
    g_stAcpuCbtCtrlInfo.enChannel = OM_LOGIC_CHANNEL_CBT;

    return;
}

/*****************************************************************************
 函 数 名     : OMRL_AcpuInit
 功能描述  : OMRL模块的初始化函数
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2008年7月1日
     作    者  : g47350
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 OMRL_AcpuInit(VOS_VOID)
{
    OmAcpuInitVar();

    if (VOS_OK != OMRL_AcpuUsbFrameInit())
    {
        LogPrint("OMRL_AcpuInit:OMRL_AcpuUsbFrameInit Fail.\n");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID OmAcpuSendIndSucShow(VOS_VOID)
{
    OM_UE_PC_SUC_STRU                  *pstUeToPcSucRecord = &(g_stAcpuTxIndCtrlInfo.stUeToPcSucRecord);

    vos_printf("\r\n *****IND channel success info show *******\r\n");

    vos_printf("Total Data Send by Ps: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stTotalData.ulNum, pstUeToPcSucRecord->stTotalData.ulDataLen);

    vos_printf("Real Data send to Socp: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stRealSendData.ulNum, pstUeToPcSucRecord->stRealSendData.ulDataLen);

    vos_printf("Real Data release by Socp: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stRealRlsData.ulNum, pstUeToPcSucRecord->stRealRlsData.ulDataLen);

    vos_printf("Max Used Buffer Size: size:%6u\n", pstUeToPcSucRecord->ulMaxUsedBufSize);

    return;
}


VOS_VOID OmAcpuSendCnfSucShow(VOS_VOID)
{
    OM_UE_PC_SUC_STRU                  *pstUeToPcSucRecord = &(g_stAcpuTxCnfCtrlInfo.stUeToPcSucRecord);

    vos_printf("\r\n *****CNF channel success info show *******\r\n");

    vos_printf("Total Data Send by Ps: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stTotalData.ulNum, pstUeToPcSucRecord->stTotalData.ulDataLen);

    vos_printf("Real Data send to Socp: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stRealSendData.ulNum, pstUeToPcSucRecord->stRealSendData.ulDataLen);

    vos_printf("Real Data release by Socp: num:%6u, len:%6u\n",
                pstUeToPcSucRecord->stRealRlsData.ulNum, pstUeToPcSucRecord->stRealRlsData.ulDataLen);

    vos_printf("Max Used Buffer Size: size:%6u\n", pstUeToPcSucRecord->ulMaxUsedBufSize);

    return;
}


VOS_VOID OmAcpuSendIndErrShow(VOS_VOID)
{
    OM_UE_PC_FAIL_STRU                  *pstUeToPcErrRecord = &(g_stAcpuTxIndCtrlInfo.stUeToPcErrRecord);

    vos_printf("\r\n *****IND channel error info show *******\r\n");

    vos_printf("Interrput Lost Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stIntData.ulNum, pstUeToPcErrRecord->stIntData.ulDataLen);

    vos_printf("Total Lost Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stLostData.ulNum, pstUeToPcErrRecord->stLostData.ulDataLen);

    vos_printf("Link Err Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stLinkErrData.ulNum, pstUeToPcErrRecord->stLinkErrData.ulDataLen);

    vos_printf("Para Err Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stParaErrData.ulNum, pstUeToPcErrRecord->stParaErrData.ulDataLen);

    vos_printf("num of sem take err: num:%6u\n", pstUeToPcErrRecord->usSemErr);

    vos_printf("num of release RD err: num:%6u\n", pstUeToPcErrRecord->usRlsRdErr);

    vos_printf("num of release Mem err: num:%6u\n", pstUeToPcErrRecord->usRlsMemErr);

    vos_printf("num of BD Lack err: num:%6u\n", pstUeToPcErrRecord->usBDLackErr);

    vos_printf("num of get BD err: num:%6u\n", pstUeToPcErrRecord->usGetBDErr);

    vos_printf("num of No Mem err: num:%6u\n", pstUeToPcErrRecord->usNoMemErr);

    vos_printf("num of Send Socp Data err: num:%6u\n", pstUeToPcErrRecord->usSendSocpDataErr);

    vos_printf("num of transfer sending addr err: num:%6u\n", pstUeToPcErrRecord->usSendAddrErr);

    vos_printf("num of transfer Release addr err: num:%6u\n", pstUeToPcErrRecord->usRlsAddrErr);

    return;
}


VOS_VOID OmAcpuSendCnfErrShow(VOS_VOID)
{
    OM_UE_PC_FAIL_STRU                  *pstUeToPcErrRecord = &(g_stAcpuTxCnfCtrlInfo.stUeToPcErrRecord);

    vos_printf("\r\n *****CNF channel error info show *******\r\n");

    vos_printf("Interrput Lost Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stIntData.ulNum, pstUeToPcErrRecord->stIntData.ulDataLen);

    vos_printf("Total Lost Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stLostData.ulNum, pstUeToPcErrRecord->stLostData.ulDataLen);

    vos_printf("Link Err Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stLinkErrData.ulNum, pstUeToPcErrRecord->stLinkErrData.ulDataLen);

    vos_printf("Para Err Data: num:%6u, len:%6u\n",
                pstUeToPcErrRecord->stParaErrData.ulNum, pstUeToPcErrRecord->stParaErrData.ulDataLen);

    vos_printf("num of sem take err: num:%6u\n", pstUeToPcErrRecord->usSemErr);

    vos_printf("num of release RD err: num:%6u\n", pstUeToPcErrRecord->usRlsRdErr);

    vos_printf("num of release Mem err: num:%6u\n", pstUeToPcErrRecord->usRlsMemErr);

    vos_printf("num of BD Lack err: num:%6u\n", pstUeToPcErrRecord->usBDLackErr);

    vos_printf("num of get BD err: num:%6u\n", pstUeToPcErrRecord->usGetBDErr);

    vos_printf("num of No Mem err: num:%6u\n", pstUeToPcErrRecord->usNoMemErr);

    vos_printf("num of Send Socp Data err: num:%6u\n", pstUeToPcErrRecord->usSendSocpDataErr);

    vos_printf("num of transfer sending addr err: num:%6u\n", pstUeToPcErrRecord->usSendAddrErr);

    vos_printf("num of transfer Release addr err: num:%6u\n", pstUeToPcErrRecord->usRlsAddrErr);

    return;
}


VOS_VOID OmAcpuRcvCbtSucShow(VOS_VOID)
{
    OM_ACPU_PC_UE_SUC_STRU              *pstPcToUeSucRecord = &(g_stAcpuCbtCtrlInfo.stPcToUeSucRecord);

    vos_printf("\r\n *****CBT receive channel success info show *******\r\n");

    vos_printf("Total Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stTotalData.ulNum, pstPcToUeSucRecord->stTotalData.ulDataLen);

    vos_printf("Data after Hdlc decap:   num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stHdlcDecapData.ulNum, pstPcToUeSucRecord->stHdlcDecapData.ulDataLen);

    vos_printf("Link Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stLinkData.ulNum, pstPcToUeSucRecord->stLinkData.ulDataLen);

    vos_printf("CCPU Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stCcpuData.ulNum, pstPcToUeSucRecord->stCcpuData.ulDataLen);

    vos_printf("ACPU Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stAcpuData.ulNum, pstPcToUeSucRecord->stAcpuData.ulDataLen);

    vos_printf("HIFI Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stHifiData.ulNum, pstPcToUeSucRecord->stHifiData.ulDataLen);

    vos_printf("MCU Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stMcuData.ulNum, pstPcToUeSucRecord->stMcuData.ulDataLen);

    vos_printf("Data Send by ICC: num:%6u, len:%6u, slice:%6u\n",
                pstPcToUeSucRecord->stICCSendSUCInfo.ulICCOmSendMsgNum,
                pstPcToUeSucRecord->stICCSendSUCInfo.ulICCOmSendLen,
                pstPcToUeSucRecord->stICCSendSUCInfo.ulICCOmSendSlice);

    vos_printf("Rls Data Recv From PC: num:%6u, slice:%6u\n",
                pstPcToUeSucRecord->stRlsData.ulNum, pstPcToUeSucRecord->stRlsData.ulDataLen);

    return;
}


VOS_VOID OmAcpuRcvCnfSucShow(VOS_VOID)
{
    OM_ACPU_PC_UE_SUC_STRU              *pstPcToUeSucRecord = &(g_stAcpuCnfCtrlInfo.stPcToUeSucRecord);

    vos_printf("\r\n *****IND channel success info show *******\r\n");

    vos_printf("Total Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stTotalData.ulNum, pstPcToUeSucRecord->stTotalData.ulDataLen);

    vos_printf("Data after Hdlc decap:   num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stHdlcDecapData.ulNum, pstPcToUeSucRecord->stHdlcDecapData.ulDataLen);

    vos_printf("Link Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stLinkData.ulNum, pstPcToUeSucRecord->stLinkData.ulDataLen);

    vos_printf("CCPU Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stCcpuData.ulNum, pstPcToUeSucRecord->stCcpuData.ulDataLen);

    vos_printf("ACPU Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stAcpuData.ulNum, pstPcToUeSucRecord->stAcpuData.ulDataLen);

    vos_printf("HIFI Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stHifiData.ulNum, pstPcToUeSucRecord->stHifiData.ulDataLen);

    vos_printf("MCU Data Recv From PC: num:%6u, len:%6u\n",
                pstPcToUeSucRecord->stMcuData.ulNum, pstPcToUeSucRecord->stMcuData.ulDataLen);

    vos_printf("Data Send by ICC: num:%6u, len:%6u, slice:%6u\n",
                pstPcToUeSucRecord->stICCSendSUCInfo.ulICCOmSendMsgNum,
                pstPcToUeSucRecord->stICCSendSUCInfo.ulICCOmSendLen,
                pstPcToUeSucRecord->stICCSendSUCInfo.ulICCOmSendSlice);

    vos_printf("Rls Data Recv From PC: num:%6u, slice:%6u\n",
                pstPcToUeSucRecord->stRlsData.ulNum, pstPcToUeSucRecord->stRlsData.ulDataLen);

    return;
}


VOS_VOID OmAcpuRcvCbtErrShow(VOS_VOID)
{
    OM_ACPU_PC_UE_FAIL_STRU             *pstPcToUeErrRecord = &(g_stAcpuCbtCtrlInfo.stPcToUeErrRecord);

    vos_printf("\r\n *****CBT receive channel error info show *******\r\n");

    vos_printf("Total Lost Data Recv From PC: num:%6u, len:%6u\n",
            pstPcToUeErrRecord->stLostData.ulNum, pstPcToUeErrRecord->stLostData.ulDataLen);

    vos_printf("Number of Frame Decap Err: num:%6u\n", pstPcToUeErrRecord->ulFrameDecapErr);

    vos_printf("Number of Packet Len Err: num:%6u\n", pstPcToUeErrRecord->usPacketLenErr);

    vos_printf("Number of Too Short Err: num:%6u\n", pstPcToUeErrRecord->usLenTooShortErr);

    vos_printf("Number of Seg Len Err: num:%6u\n", pstPcToUeErrRecord->usSegLenErr);

    vos_printf("Number of Seg Num Err: num:%6u\n", pstPcToUeErrRecord->usSegNumErr);

    vos_printf("Number of DataType Err: num:%6u\n", pstPcToUeErrRecord->usDatatypeErr);

    vos_printf("Number of Too Long Err: num:%6u\n", pstPcToUeErrRecord->usMsgTooLongErr);

    vos_printf("Number of Cpu Id Err: num:%6u\n", pstPcToUeErrRecord->usCpuIdErr);

    vos_printf("Number of No Mem Err: num:%6u\n", pstPcToUeErrRecord->usNoMemErr);

    vos_printf("Number of Data Head Err: num:%6u\n", pstPcToUeErrRecord->usDataHeadErr);

    vos_printf("Number of Msg Len Err: num:%6u\n", pstPcToUeErrRecord->usMsgLenErr);

    vos_printf("Number of Msg Sn Err: num:%6u\n", pstPcToUeErrRecord->usMsgSnErr);

    vos_printf("Number of Packet Lost Err: num:%6u\n", pstPcToUeErrRecord->usPacketLostErr);

    vos_printf("Number of Link Data Len Err: num:%6u\n", pstPcToUeErrRecord->usLinkDataLenErr);

    vos_printf("Number of Alloc msg Mem: num: %6u\n",pstPcToUeErrRecord->usAllocMsg);

    vos_printf("Number of Lost Msg Because Switch Is IDLE: num: %6u\n",pstPcToUeErrRecord->usLinkStatusErr);

    vos_printf("Data Send err by ICC: num:%6u, len:%6u, slice:%6u\n",
            pstPcToUeErrRecord->stICCSendFailInfo.ulICCOmSendErrNum,
            pstPcToUeErrRecord->stICCSendFailInfo.ulICCOmSendErrLen,
            pstPcToUeErrRecord->stICCSendFailInfo.ulICCOmSendErrSlice);

    return;
}


VOS_VOID OmAcpuRcvCnfErrShow(VOS_VOID)
{
    OM_ACPU_PC_UE_FAIL_STRU             *pstPcToUeErrRecord = &(g_stAcpuCnfCtrlInfo.stPcToUeErrRecord);

    vos_printf("\r\n *****CNF receive channel error info show *******\r\n");

    vos_printf("Total Lost Data Recv From PC: num:%6u, len:%6u\n",
            pstPcToUeErrRecord->stLostData.ulNum, pstPcToUeErrRecord->stLostData.ulDataLen);

    vos_printf("Number of Frame Decap Err: num:%6u\n", pstPcToUeErrRecord->ulFrameDecapErr);

    vos_printf("Number of Packet Len Err: num:%6u\n", pstPcToUeErrRecord->usPacketLenErr);

    vos_printf("Number of Too Short Err: num:%6u\n", pstPcToUeErrRecord->usLenTooShortErr);

    vos_printf("Number of Seg Len Err: num:%6u\n", pstPcToUeErrRecord->usSegLenErr);

    vos_printf("Number of Seg Num Err: num:%6u\n", pstPcToUeErrRecord->usSegNumErr);

    vos_printf("Number of DataType Err: num:%6u\n", pstPcToUeErrRecord->usDatatypeErr);

    vos_printf("Number of Too Long Err: num:%6u\n", pstPcToUeErrRecord->usMsgTooLongErr);

    vos_printf("Number of Cpu Id Err: num:%6u\n", pstPcToUeErrRecord->usCpuIdErr);

    vos_printf("Number of No Mem Err: num:%6u\n", pstPcToUeErrRecord->usNoMemErr);

    vos_printf("Number of Data Head Err: num:%6u\n", pstPcToUeErrRecord->usDataHeadErr);

    vos_printf("Number of Msg Len Err: num:%6u\n", pstPcToUeErrRecord->usMsgLenErr);

    vos_printf("Number of Msg Sn Err: num:%6u\n", pstPcToUeErrRecord->usMsgSnErr);

    vos_printf("Number of Packet Lost Err: num:%6u\n", pstPcToUeErrRecord->usPacketLostErr);

    vos_printf("Number of Link Data Len Err: num:%6u\n", pstPcToUeErrRecord->usLinkDataLenErr);

    vos_printf("Number of Alloc msg Mem: num: %6u\n",pstPcToUeErrRecord->usAllocMsg);

    vos_printf("Number of Lost Msg Because Switch Is IDLE: num: %6u\n",pstPcToUeErrRecord->usLinkStatusErr);

    vos_printf("Data Send err by ICC: num:%6u, len:%6u, slice:%6u\n",
            pstPcToUeErrRecord->stICCSendFailInfo.ulICCOmSendErrNum,
            pstPcToUeErrRecord->stICCSendFailInfo.ulICCOmSendErrLen,
            pstPcToUeErrRecord->stICCSendFailInfo.ulICCOmSendErrSlice);

    return;
}

/*****************************************************************************
 函 数 名  : OmOpenLog
 功能描述  : 打印当前OM通道的状态
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2009年5月18日
     作    者  : g47350
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OmOpenLog(VOS_UINT32 ulFlag)
{
    g_ulOmAcpuDbgFlag = ulFlag;

    return;
}



VOS_VOID OmAcpuRate(VOS_VOID)
{
    VOS_UINT32            ulCurrTick;                    /*记录现在统计时间*/
    VOS_UINT32            ulDeltaTick;                   /*记录统计时间*/
    VOS_UINT32            ulDelData;                     /*记录统计数据大小*/
    VOS_UINT32            ulBandWidth;                   /*记录统计数据大小*/

    vos_printf("OmAcpuRate Show(ALL Data To UDI):\n");

    ulCurrTick = VOS_GetTick();

    if (ulCurrTick < g_ulAcpuLastTick)
    {
        ulDeltaTick = (0xFFFFFFFF - g_ulAcpuLastTick) + ulCurrTick;
    }
    else
    {
        ulDeltaTick = ulCurrTick - g_ulAcpuLastTick;
    }

    ulDelData      = g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendLen + g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendLen - g_ulAcpuLastByteCnt;
    g_ulAcpuLastByteCnt  = g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendLen + g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendLen;

    ulBandWidth    = (ulDelData/ulDeltaTick)/10;

    /*统计OM在ulDeltaTick时间(单位10ms)内发送了ulTotalLen字节数据*/
    vos_printf("OmAcpuRate,Tick <%u> Byte <%u>, BandWidth <%u>.\n",
                 ulDeltaTick, ulDelData, ulBandWidth);

    g_ulAcpuLastTick = ulCurrTick;

    vos_printf("OmAcpuRate Show End\n");
}

VOS_VOID OmAcpuShowCombineInfo(VOS_VOID)
{
    vos_printf("cnf channel pstWholeMsg:        0x%p\r\n",g_stAcpuCnfCtrlInfo.stMsgCombineInfo.pstWholeMsg);
    vos_printf("cnf channel ucExpectedSegSn:    0x%x\r\n",g_stAcpuCnfCtrlInfo.stMsgCombineInfo.ucExpectedSegSn);
    vos_printf("cnf channel ucTotalSegNum:      0x%x\r\n",g_stAcpuCnfCtrlInfo.stMsgCombineInfo.ucTotalSegNum);
    vos_printf("cnf channel ulMoveLen:          0x%x\r\n",g_stAcpuCnfCtrlInfo.stMsgCombineInfo.ulMoveLen);
    vos_printf("cnf channel ulTotalMsgLen:      0x%x\r\n",g_stAcpuCnfCtrlInfo.stMsgCombineInfo.ulTotalMsgLen);
    vos_printf("cbt channel pstWholeMsg:        0x%p\r\n",g_stAcpuCbtCtrlInfo.stMsgCombineInfo.pstWholeMsg);
    vos_printf("cbt channel ucExpectedSegSn:    0x%x\r\n",g_stAcpuCbtCtrlInfo.stMsgCombineInfo.ucExpectedSegSn);
    vos_printf("cbt channel ucTotalSegNum:      0x%x\r\n",g_stAcpuCbtCtrlInfo.stMsgCombineInfo.ucTotalSegNum);
    vos_printf("cbt channel ulMoveLen:          0x%x\r\n",g_stAcpuCbtCtrlInfo.stMsgCombineInfo.ulMoveLen);
    vos_printf("cbt channel ulTotalMsgLen:      0x%x\r\n",g_stAcpuCbtCtrlInfo.stMsgCombineInfo.ulTotalMsgLen);
}


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

