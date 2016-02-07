/******************************************************************************

                  版权所有 (C), 2005-2007, 华为技术有限公司


  文 件 名   : IpsMntn.c
  版 本 号   : 初稿
  作    者   : 冯家麟
  生成日期   : 2011/7/22
  最近修改   :
  功能描述   : IPS MNTN模块实现
  函数列表   :

  说明       :
*****************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "v_typdef.h"
#include "IpsMntn.h"
#include "TTFComm.h"
#include "OmApp.h"
#include "v_id.h"
#include "vos.h"



#include "NetfilterEx.h"


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_ACPU_IPS_MNTN_C
/*lint -e767 */

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*******************************************************************************
  3 枚举定义
*******************************************************************************/

/*****************************************************************************
  2 结构定义
*****************************************************************************/

/*****************************************************************************
  3 全局变量声明
*****************************************************************************/
#if (FEATURE_ON == FEATURE_NFEXT)
IPS_MNTN_TRACE_CONFIG_REQ_STRU          g_stIpsTraceMsgCfg;     /*TCP/IP协议栈可维可测配置信息*/
IPS_MNTN_INFO_ENTITY                    g_stIpsMntnInfoEntity;  /*TCP/IP协议栈可维可测实体*/
#endif      /* #if (FEATURE_ON == FEATURE_NFEXT) */

IPS_MNTN_IP_INFO_CONFIG_REQ_STRU        g_stIpsMntnIPInfoCfg;   /*IP数据包配置信息*/

/*****************************************************************************
  4 消息头定义
*****************************************************************************/

/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/


VOS_VOID IPS_MNTN_FillMsg(OM_APP_TRACE_STRU *pstAppTrace, VOS_UINT32 ulMsgName, VOS_UINT32 ulMsgLen)
{
    if (VOS_NULL_PTR == pstAppTrace)
    {
        return;
    }

    /* 填充信息 */
    pstAppTrace->stAppHeader.ucFuncType = OM_TRANS_FUNC;
    pstAppTrace->stAppHeader.usLength   = (VOS_UINT16)(ulMsgLen - VOS_OM_HEADER_LEN);

    OM_AcpuAddSNTime(&(pstAppTrace->stAppHeader.ulSn),
                    &(pstAppTrace->stAppHeader.ulTimeStamp));
    pstAppTrace->usPrimId   = (VOS_UINT16)ulMsgName;
    pstAppTrace->usToolId   = 0;
}
VOS_VOID IPS_MNTN_SndCfgCnf2Om
(
    VOS_UINT16      usPrimId,
    VOS_UINT32      ulTransMsgContentLen,
    VOS_VOID       *pTransMsgContent
)
{
    VOS_UINT32                   ulIpsMntnToOmMsgLen;
    VOS_UINT32                   ulOffsetLen;
    PS_OM_TRANS_IND_STRU        *pstIpsMntnToOmMsg = VOS_NULL_PTR;

    /*申请消息*/
    TTF_GET_OFFSET(ulIpsMntnToOmMsgLen, PS_OM_TRANS_IND_STRU, aucData);
    ulIpsMntnToOmMsgLen += ulTransMsgContentLen;

    pstIpsMntnToOmMsg   = (PS_OM_TRANS_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_NFEXT, ulIpsMntnToOmMsgLen);
    if (VOS_NULL_PTR == pstIpsMntnToOmMsg)
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "IPS, IPS_MNTN_SndCfgCnf2Om, ERROR, Call PS_ALLOC_MSG_WITH_HEADER_LEN fail!");
        return;
    }

    /* 填充 到 OM的透明消息 */
    IPS_FILL_MSG_HEADER(pstIpsMntnToOmMsg, ACPU_PID_OM);

    TTF_GET_OFFSET(ulOffsetLen, PS_OM_TRANS_IND_STRU, ulSn);

    pstIpsMntnToOmMsg->usTransPrimId    = OM_TRANS_PRIMID;          /*透传消息固定填为0x5001*/
    pstIpsMntnToOmMsg->ucFuncType       = OM_TRANS_FUNC;            /*透传功能*/
    pstIpsMntnToOmMsg->usAppLength      = (VOS_UINT16)(ulIpsMntnToOmMsgLen - ulOffsetLen); /*减去usAppLength及其之前的ie长度*/
    pstIpsMntnToOmMsg->ulSn             = 0;                        /*暂时填0*/
    pstIpsMntnToOmMsg->ulTimeStamp      = OM_GetSlice();
    pstIpsMntnToOmMsg->usPrimId         = usPrimId;                 /*根据输入消息ID填写*/
    pstIpsMntnToOmMsg->usToolsId        = 0;                        /*工具ID,目前暂没有使用*/

    /* 填充确认信息 */
    if ( (0 != ulTransMsgContentLen) && (VOS_NULL_PTR != pTransMsgContent) )
    {
        PS_MEM_CPY(pstIpsMntnToOmMsg->aucData, pTransMsgContent, ulTransMsgContentLen);
    }

    if ( VOS_OK != PS_SEND_MSG(ACPU_PID_NFEXT, pstIpsMntnToOmMsg) )
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "IPS, IPS_MNTN_SndCfgCnf2Om, ERROR, Call PS_SEND_MSG fail!");
    }

    return;
}

#if (FEATURE_ON == FEATURE_NFEXT)
VOS_UINT32 IPS_MNTN_TransMsg
(
    VOS_UINT8                              *pucTransMsg,
    VOS_UINT32                              ulTransMsgContentLen,
    IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16     enMsgName
)
{
    VOS_UINT32                          ulTraceMsgLen;
    NF_EXT_DATA_RING_BUF_STRU           stBufData;
    OM_APP_TRACE_STRU                  *pstAppTrace = VOS_NULL_PTR;

    if (VOS_NULL_PTR == pucTransMsg)
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "IPS, IPS_MNTN_TransMsg, ERROR, pucTransMsg is NULL ");

        return VOS_ERR;
    }

    /*申请消息*/
    TTF_GET_OFFSET(ulTraceMsgLen, OM_APP_TRACE_STRU, aucPara);  /* [false alarm]:TTF_GET_OFFSET为标准库函数调用，不会有NULL指针问题 */
    ulTraceMsgLen   += ulTransMsgContentLen;
    pstAppTrace     = (OM_APP_TRACE_STRU *)pucTransMsg;

    IPS_MNTN_FillMsg(pstAppTrace, enMsgName, ulTraceMsgLen);

    stBufData.pData         = (VOS_UINT8 *)pstAppTrace;
    stBufData.usDataLen     = (VOS_UINT16)ulTraceMsgLen;

    if (VOS_OK != NFExt_AddDataToRingBuf(&stBufData))
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_WARNING,
            "IPS, IPS_MNTN_TransMsg, ERROR, NFExt_AddDataToRingBuf Fail! ");

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 IPS_MNTN_GetPktLenByTraceCfg
(
    IPS_MNTN_TRACE_CONFIG_STRU     *pstTraceCfg,
    VOS_UINT16                      usPktLen,
    VOS_UINT32                     *pulDataLen
)
{
    *pulDataLen    =   0;

    /*判断定时器是否启动*/
    switch ( pstTraceCfg->ulChoice )
    {
        case IPS_MNTN_TRACE_NULL_CHOSEN:
            return PS_FALSE;
        case IPS_MNTN_TRACE_MSG_HEADER_CHOSEN:
            break;
        case IPS_MNTN_TRACE_CONFIGURABLE_LEN_CHOSEN:
            *pulDataLen = TTF_MIN(pstTraceCfg->ulTraceDataLen, usPktLen);
            break;
        case IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN:
            *pulDataLen = usPktLen;
            break;
        default:
            return PS_FALSE;
    }

    return PS_TRUE;
}
VOS_VOID IPS_MNTN_PktInfoCB
(
    const VOS_UINT8                        *pucNetIfName,
    const VOS_UINT8                        *pucPktData,
    VOS_UINT16                              usPktLen,
    IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16     usType
)
{
    VOS_UINT8                          *pucTraceData;
    IPS_MNTN_TRACE_CONFIG_STRU         *pstTraceCfg;
    IPS_MNTN_PKT_INFO_STRU             *pstTraceMsg;
    VOS_UINT32                          ulTransDataLen;
    VOS_UINT32                          ulOffsetLen;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulRst;

    ulTransDataLen  =   0;
    ulDataLen       =   0;

    if ( VOS_NULL_PTR == pucPktData )
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "IPS, IPS_MNTN_PktInfoCB, ERROR, paucPktData is NULL ");
        return;
    }

    /*根据消息类型选择对应配置信息*/
    if(ID_IPS_TRACE_INPUT_DATA_INFO == usType)
    {
        pstTraceCfg = &(g_stIpsTraceMsgCfg.stPreRoutingTraceCfg);
    }
    else if(ID_IPS_TRACE_OUTPUT_DATA_INFO == usType)
    {
        pstTraceCfg = &(g_stIpsTraceMsgCfg.stPostRoutingTraceCfg);
    }
    else if(usType < ID_IPS_TRACE_MSG_TYPE_BUTT)
    {
        pstTraceCfg = &(g_stIpsTraceMsgCfg.stPreRoutingTraceCfg);
    }
    else
    {
        /*打印警告信息,消息类型不匹配*/
        PS_LOG( ACPU_PID_NFEXT, 0, PS_PRINT_ERROR,
                "IPS, IPS_MNTN_PktInfoCB : ERROR : usType is not Expectes!" );
        return;
    }

    /*根据消息配置信息，获取输出报文长度*/
    ulRst = IPS_MNTN_GetPktLenByTraceCfg(pstTraceCfg, usPktLen, &ulDataLen);
    if(PS_FALSE == ulRst)
    {
        /*不捕获该报文*/
        return;
    }

    ulDataLen           = PS_MIN(IPS_MNTN_TRACE_MAX_BYTE_LEN, ulDataLen);
    TTF_GET_OFFSET(ulTransDataLen, IPS_MNTN_PKT_INFO_STRU, aucData);
    ulTransDataLen      += ulDataLen;

    /* 从Linux内核申请内存 */
    TTF_GET_OFFSET(ulOffsetLen, OM_APP_TRACE_STRU, aucPara);    /* [false alarm]:TTF_GET_OFFSET为标准库函数调用，不会有NULL指针问题 */
    pucTraceData = NF_EXT_MEM_ALLOC(ACPU_PID_NFEXT, ulTransDataLen + ulOffsetLen);

    if (VOS_NULL_PTR == pucTraceData)
    {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_ALLOC_MEM_FAIL);

        return;
    }

    /*构建透明消息*/
    pstTraceMsg         = (IPS_MNTN_PKT_INFO_STRU *)(pucTraceData + ulOffsetLen);
    pstTraceMsg->usLen  = usPktLen;
    mdrv_memcpy(pstTraceMsg->aucNetIfName, pucNetIfName, IPS_IFNAMSIZ);
    /*lint -e669 */
    mdrv_memcpy(pstTraceMsg->aucData, pucPktData, ulDataLen);
    /*lint +e669 */

    if (VOS_OK != IPS_MNTN_TransMsg(pucTraceData, ulTransDataLen, usType))
    {
        NF_EXT_MEM_FREE(ACPU_PID_NFEXT, pucTraceData);
    }
}
VOS_VOID IPS_MNTN_BridgePktInfoCB
(
    const VOS_UINT8                        *pucSrcPort,
    const VOS_UINT8                        *pucDestPort,
    VOS_UINT8                              *pucPktData,
    VOS_UINT16                              usPktLen,
    IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16     usType
)
{
    VOS_UINT8                          *pucTraceData;
    IPS_MNTN_TRACE_CONFIG_STRU         *pstTraceCfg;
    IPS_MNTN_BRIDGE_PKT_INFO_STRU      *pstTraceMsg;
    VOS_UINT32                          ulTransDataLen;
    VOS_UINT32                          ulOffsetLen;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulRst;

    ulTransDataLen  =   0;
    ulDataLen       =   0;

    if ( VOS_NULL_PTR == pucPktData )
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "IPS, IPS_MNTN_BridgePktInfoCB, ERROR, paucPktData is NULL ");
        return;
    }

    /*根据消息类型选择对应配置信息*/
    if( usType < ID_IPS_TRACE_MSG_TYPE_BUTT )
    {
        pstTraceCfg = &(g_stIpsTraceMsgCfg.stBridgeArpTraceCfg);
    }
    else
    {
        /*打印警告信息,消息类型不匹配*/
        PS_LOG( ACPU_PID_NFEXT, 0, PS_PRINT_ERROR,
                "IPS, IPS_MNTN_BridgePktInfoCB : ERROR : usType is not Expectes!" );
        return;
    }

    /*根据消息配置信息，获取输出报文长度*/
    ulRst = IPS_MNTN_GetPktLenByTraceCfg(pstTraceCfg, usPktLen, &ulDataLen);
    if(PS_FALSE == ulRst)
    {
        /*不捕获该报文*/
        return;
    }

    ulDataLen           = PS_MIN(IPS_MNTN_TRACE_MAX_BYTE_LEN, ulDataLen);
    TTF_GET_OFFSET(ulTransDataLen, IPS_MNTN_BRIDGE_PKT_INFO_STRU, aucData);
    ulTransDataLen      += ulDataLen;

    /* 从Linux内核申请内存 */
    TTF_GET_OFFSET(ulOffsetLen, OM_APP_TRACE_STRU, aucPara);    /* [false alarm]:TTF_GET_OFFSET为标准库函数调用，不会有NULL指针问题 */
    pucTraceData = NF_EXT_MEM_ALLOC(ACPU_PID_NFEXT, ulTransDataLen + ulOffsetLen);

    if (VOS_NULL_PTR == pucTraceData)
    {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_ALLOC_MEM_FAIL);

        return;
    }

    /*构建透明消息*/
    pstTraceMsg = (IPS_MNTN_BRIDGE_PKT_INFO_STRU *)(pucTraceData + ulOffsetLen);
    pstTraceMsg->usLen  = usPktLen;

    if (VOS_NULL_PTR != pucSrcPort)
    {
        mdrv_memcpy(pstTraceMsg->aucSrcPort, pucSrcPort, IPS_END_NAME);
    }
    if (VOS_NULL_PTR != pucDestPort)
    {
        mdrv_memcpy(pstTraceMsg->aucDestPort, pucDestPort, IPS_END_NAME);
    }

    /*lint -e669 */
    mdrv_memcpy(pstTraceMsg->aucData, pucPktData, ulDataLen);
    /*lint +e669 */

    if (VOS_OK != IPS_MNTN_TransMsg(pucTraceData, ulTransDataLen, usType))
    {
        NF_EXT_MEM_FREE(ACPU_PID_NFEXT, pucTraceData);
    }

}
VOS_VOID IPS_MNTN_CtrlPktInfoCB
(
    const VOS_UINT8                        *pucNetIfName,
    const VOS_UINT8                        *pucPktData,
    VOS_UINT16                              usPktLen,
    IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16     usType
)
{

    VOS_UINT8                          *pucTraceData;
    IPS_MNTN_PKT_INFO_STRU             *pstTraceMsg;
    VOS_UINT32                          ulTransDataLen;
    VOS_UINT32                          ulOffsetLen;
    VOS_UINT32                          ulDataLen;

    ulTransDataLen  =   0;
    ulDataLen       =   0;

    if ( VOS_NULL_PTR == pucPktData )
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "IPS, IPS_MNTN_CtrlPktInfoCB, ERROR, pucPktData is NULL ");
        return;
    }

    ulDataLen           = PS_MIN(IPS_MNTN_TRACE_MAX_BYTE_LEN, usPktLen);
    TTF_GET_OFFSET(ulTransDataLen, IPS_MNTN_PKT_INFO_STRU, aucData);
    ulTransDataLen      += ulDataLen;

    /* 从Linux内核申请内存 */
    TTF_GET_OFFSET(ulOffsetLen, OM_APP_TRACE_STRU, aucPara);    /* [false alarm]:TTF_GET_OFFSET为标准库函数调用，不会有NULL指针问题 */
    pucTraceData = NF_EXT_MEM_ALLOC(ACPU_PID_NFEXT, ulTransDataLen + ulOffsetLen);

    if (VOS_NULL_PTR == pucTraceData)
    {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_ALLOC_MEM_FAIL);

        return;
    }

    /*构建透明消息*/
    pstTraceMsg = (IPS_MNTN_PKT_INFO_STRU *)(pucTraceData + ulOffsetLen);
    pstTraceMsg->usLen  = usPktLen;
    mdrv_memcpy(pstTraceMsg->aucNetIfName, pucNetIfName, IPS_IFNAMSIZ);
    /*lint -e669 */
    mdrv_memcpy(pstTraceMsg->aucData, pucPktData, ulDataLen);
    /*lint +e669 */

    if (VOS_OK != IPS_MNTN_TransMsg(pucTraceData, ulTransDataLen, usType))
    {
        NF_EXT_MEM_FREE(ACPU_PID_NFEXT, pucTraceData);

    }
}
PS_BOOL_ENUM_UINT8  IPS_MNTN_BridgeTraceCfgChkParam(IPS_MNTN_TRACE_CONFIG_REQ_STRU *pRcvMsg)
{
    if ( (IPS_MNTN_TRACE_NULL_CHOSEN != pRcvMsg->stBridgeArpTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_MSG_HEADER_CHOSEN != pRcvMsg->stBridgeArpTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_CONFIGURABLE_LEN_CHOSEN != pRcvMsg->stBridgeArpTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN != pRcvMsg->stBridgeArpTraceCfg.ulChoice) )
    {
        return PS_FALSE;
    }

    return PS_TRUE;
}


PS_BOOL_ENUM_UINT8  IPS_MNTN_TraceCfgChkParam(IPS_MNTN_TRACE_CONFIG_REQ_STRU *pRcvMsg)
{
    if ( (IPS_MNTN_TRACE_NULL_CHOSEN != pRcvMsg->stPreRoutingTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_MSG_HEADER_CHOSEN != pRcvMsg->stPreRoutingTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_CONFIGURABLE_LEN_CHOSEN != pRcvMsg->stPreRoutingTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN != pRcvMsg->stPreRoutingTraceCfg.ulChoice) )
    {
        return PS_FALSE;
    }

    if ( (IPS_MNTN_TRACE_NULL_CHOSEN != pRcvMsg->stPostRoutingTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_MSG_HEADER_CHOSEN != pRcvMsg->stPostRoutingTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_CONFIGURABLE_LEN_CHOSEN != pRcvMsg->stPostRoutingTraceCfg.ulChoice)
        && (IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN != pRcvMsg->stPostRoutingTraceCfg.ulChoice) )
    {
        return PS_FALSE;
    }

    return PS_TRUE;
}


PS_BOOL_ENUM_UINT8  IPS_MNTN_TraceAdvancedCfgChkParam(IPS_MNTN_TRACE_CONFIG_REQ_STRU *pRcvMsg)
{
    /*TCP/IP协议栈报文钩包配置参数检查*/
    if(PS_FALSE == IPS_MNTN_TraceCfgChkParam(pRcvMsg))
    {
        return PS_FALSE;
    }

    /*网桥中转报文钩包配置参数检查*/
    if(PS_FALSE == IPS_MNTN_BridgeTraceCfgChkParam(pRcvMsg))
    {
        return  PS_FALSE;
    }

    return PS_TRUE;

}


VOS_VOID IPS_MNTN_TraceAdvancedCfgReq(VOS_VOID *pMsg)
{
    PS_BOOL_ENUM_UINT8                  enResult;
    IPS_MNTN_TRACE_CONFIG_REQ_STRU     *pRcvMsg;
    IPS_MNTN_TRACE_CONFIG_CNF_STRU      stIpsTraceCfgCnf;

    pRcvMsg = (IPS_MNTN_TRACE_CONFIG_REQ_STRU *) (((OM_PS_PARAM_REQ_STRU *)pMsg)->aucData);

    /*检测配置参数是否合法*/
    enResult = IPS_MNTN_TraceAdvancedCfgChkParam(pRcvMsg);

    if ( PS_FALSE == enResult )
    {
        /*如果参数检测不合法，向OM回复配置失败*/
        stIpsTraceCfgCnf.enRslt =   PS_FAIL;
        IPS_MNTN_SndCfgCnf2Om(ID_IPS_OM_ADVANCED_TRACE_CONFIG_CNF,
            sizeof(IPS_MNTN_TRACE_CONFIG_CNF_STRU), &stIpsTraceCfgCnf );
        PS_LOG(ACPU_PID_NFEXT, 0, PS_PRINT_ERROR,
            "Warning: IPS_MNTN_TraceAdvancedCfgReq check fail");

        return;
    }

    enResult = NFExt_ConfigEffective(pRcvMsg);
    if( PS_FALSE == enResult )
    {
        /*如果注册钩子函数失败，向OM回复配置失败*/
        stIpsTraceCfgCnf.enRslt =   PS_FAIL;
        IPS_MNTN_SndCfgCnf2Om(ID_IPS_OM_ADVANCED_TRACE_CONFIG_CNF,
            sizeof(IPS_MNTN_TRACE_CONFIG_CNF_STRU), &stIpsTraceCfgCnf );
        PS_LOG(ACPU_PID_NFEXT, 0, PS_PRINT_ERROR,
            "Warning: IPS_MNTN_SetCallbackFunc fail");

        return;
    }

    /*保存配置参数*/
    PS_MEM_CPY(&g_stIpsTraceMsgCfg, pRcvMsg, sizeof(IPS_MNTN_TRACE_CONFIG_REQ_STRU));

    /*向OM回复配置成功*/
    stIpsTraceCfgCnf.enRslt =   PS_SUCC;
    IPS_MNTN_SndCfgCnf2Om(ID_IPS_OM_ADVANCED_TRACE_CONFIG_CNF,
            sizeof(IPS_MNTN_TRACE_CONFIG_CNF_STRU), &stIpsTraceCfgCnf );

}
VOS_VOID IPS_MNTN_SendTranMsgDirect(OM_APP_TRACE_STRU *pstAppTrace, VOS_UINT32 ulMsgContenLen, VOS_UINT32 ulMsgName)
{
    VOS_UINT32                          ulTraceMsgLen;

    if (VOS_NULL_PTR == pstAppTrace)
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "IPS, IPS_MNTN_SendFlowCtrlMsg, ERROR, pucTransMsg is NULL ");

        return;
    }

    TTF_GET_OFFSET(ulTraceMsgLen, OM_APP_TRACE_STRU, aucPara);  /* [false alarm]:TTF_GET_OFFSET为标准库函数调用，不会有NULL指针问题 */
    ulTraceMsgLen   += ulMsgContenLen;

    /* 转为OM透明消息结构 */
    IPS_MNTN_FillMsg(pstAppTrace, ulMsgName, ulTraceMsgLen);

    OM_AcpuSendData((OM_RSP_PACKET_STRU*)pstAppTrace, (VOS_UINT16)ulTraceMsgLen);
}



VOS_VOID IPS_MNTN_FlowCtrl(VOS_UINT32 ulFcType, IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16  usType)
{
    IPS_MNTN_FLOW_CTRL_STRU            *pstFlowCtrlMsg;
    VOS_UINT32                          ulOffsetLen;
    VOS_UINT8                          *pucTraceData;

    TTF_GET_OFFSET(ulOffsetLen, OM_APP_TRACE_STRU, aucPara);    /* [false alarm]:TTF_GET_OFFSET为标准库函数调用，不会有NULL指针问题 */
    pucTraceData = NF_EXT_MEM_ALLOC(ACPU_PID_NFEXT, MNTN_FLOW_CTRL_INFO_LEN + ulOffsetLen);

    pstFlowCtrlMsg = (IPS_MNTN_FLOW_CTRL_STRU *)((VOS_UINT_PTR)pucTraceData + ulOffsetLen);

    pstFlowCtrlMsg->ulFcType = ulFcType;

    IPS_MNTN_SendTranMsgDirect((OM_APP_TRACE_STRU *)pucTraceData, MNTN_FLOW_CTRL_INFO_LEN, usType);

    NF_EXT_MEM_FREE(ACPU_PID_NFEXT, pucTraceData);

    return;
}

#endif      /* #if (FEATURE_ON == FEATURE_NFEXT) */


PS_BOOL_ENUM_UINT8  IPS_MNTN_IPInfoCfgChkParam(IPS_MNTN_IP_INFO_CONFIG_REQ_STRU *pRcvIpInfoCfg)
{
    if ((PS_TRUE != pRcvIpInfoCfg->enAdsIPConfig) && (PS_FALSE != pRcvIpInfoCfg->enAdsIPConfig))
    {
        return PS_FALSE;
    }

    if ((PS_TRUE != pRcvIpInfoCfg->enUsbIPConfig) && (PS_FALSE != pRcvIpInfoCfg->enUsbIPConfig))
    {
        return PS_FALSE;
    }

    return PS_TRUE;
}


VOS_VOID IPS_MNTN_IPInfoCfgReq(VOS_VOID *pMsg)
{
    PS_BOOL_ENUM_UINT8                   enResult;
    IPS_MNTN_IP_INFO_CONFIG_REQ_STRU     *pRcvIpInfoCfg;
    IPS_MNTN_TRACE_CONFIG_CNF_STRU       stIpsTraceCfgCnf;

    pRcvIpInfoCfg   = (IPS_MNTN_IP_INFO_CONFIG_REQ_STRU *) (((OM_PS_PARAM_REQ_STRU *)pMsg)->aucData);

    /*检测配置参数是否合法*/
    enResult        = IPS_MNTN_IPInfoCfgChkParam(pRcvIpInfoCfg);

    if ( PS_FALSE == enResult )
    {
        /*如果参数检测不合法，向OM回复配置失败*/
        stIpsTraceCfgCnf.enRslt = PS_FAIL;
        IPS_MNTN_SndCfgCnf2Om(ID_IPS_OM_MNTN_IP_INFO_CONFIG_CNF,
            sizeof(IPS_MNTN_TRACE_CONFIG_CNF_STRU), &stIpsTraceCfgCnf );
        PS_LOG(ACPU_PID_NFEXT, 0, PS_PRINT_ERROR, "Warning: IPS_MNTN_IPInfoCfgReq check fail");

        return;
    }

    /*保存配置参数*/
    PS_MEM_CPY(&g_stIpsMntnIPInfoCfg, pRcvIpInfoCfg, sizeof(IPS_MNTN_IP_INFO_CONFIG_REQ_STRU));

    if (PS_TRUE == g_stIpsMntnIPInfoCfg.enUsbIPConfig)
    {
        DRV_USB_REG_IPS_TRACECB(IPS_MNTN_TraceIpInfo);
    }
    else
    {
        DRV_USB_REG_IPS_TRACECB(NULL);
    }

    /*向OM回复配置成功*/
    stIpsTraceCfgCnf.enRslt = PS_SUCC;
    IPS_MNTN_SndCfgCnf2Om(ID_IPS_OM_MNTN_IP_INFO_CONFIG_CNF,
            sizeof(IPS_MNTN_TRACE_CONFIG_CNF_STRU), &stIpsTraceCfgCnf );
}


VOS_VOID IPS_MNTN_Ipv4DataParse(IPS_MNTN_IP_INFO_STRU *pstIpInfo,VOS_UINT8 *pData)
{
    switch (pstIpInfo->ucL4Proto)
    {
        case IP_IPV4_PROTO_TCP:
            pstIpInfo->usSrcPort      = IP_GET_VAL_NTOH_U16(pData, IP_IPV4_HEAD_LEN);
            pstIpInfo->usDstPort      = IP_GET_VAL_NTOH_U16(pData, (IP_IPV4_HEAD_LEN + IP_IPV4_DST_PORT_POS));
            pstIpInfo->ulL4Id         = IP_GET_VAL_NTOH_U32(pData, (IP_IPV4_HEAD_LEN + IP_IPV4_TCP_SEQ_POS));
            pstIpInfo->ulTcpAckSeq    = IP_GET_VAL_NTOH_U32(pData, (IP_IPV4_HEAD_LEN + IP_IPV4_TCP_ACK_POS));
            break;

        case IP_IPV4_PROTO_UDP:
            pstIpInfo->usSrcPort      = IP_GET_VAL_NTOH_U16(pData, IP_IPV4_HEAD_LEN);
            pstIpInfo->usDstPort      = IP_GET_VAL_NTOH_U16(pData, (IP_IPV4_HEAD_LEN + IP_IPV4_DST_PORT_POS));
            break;

        case IP_IPV4_PROTO_ICMP:
            pstIpInfo->ucIcmpType     = IP_GET_VAL_NTOH_U8(pData, IP_IPV4_HEAD_LEN);
            if ((IP_IPV4_ICMP_ECHO_REQUEST == pstIpInfo->ucIcmpType)
                ||(IP_IPV4_ICMP_ECHO_REPLY == pstIpInfo->ucIcmpType))
            {
                pstIpInfo->ulL4Id     = IP_GET_VAL_NTOH_U32(pData, (IP_IPV4_HEAD_LEN + IP_IPV4_ICMP_IDENTIFY_POS));
            }
            break;

        default:
            break;
    }

    return;
}


VOS_VOID IPS_MNTN_Ipv6DataParse(IPS_MNTN_IP_INFO_STRU *pstIpInfo,VOS_UINT8 *pData)
{
    switch (pstIpInfo->ucL4Proto)
    {
        case IP_IPV6_PROTO_TCP:
            pstIpInfo->usSrcPort      = IP_GET_VAL_NTOH_U16(pData, IP_IPV6_HEAD_LEN);
            pstIpInfo->usDstPort      = IP_GET_VAL_NTOH_U16(pData, (IP_IPV6_HEAD_LEN + IP_IPV6_DST_PORT_POS));
            pstIpInfo->ulL4Id         = IP_GET_VAL_NTOH_U32(pData, (IP_IPV6_HEAD_LEN + IP_IPV6_TCP_SEQ_POS));
            pstIpInfo->ulTcpAckSeq    = IP_GET_VAL_NTOH_U32(pData, (IP_IPV6_HEAD_LEN + IP_IPV6_TCP_ACK_POS));
            break;

        case IP_IPV6_PROTO_UDP:
            pstIpInfo->usSrcPort      = IP_GET_VAL_NTOH_U16(pData, IP_IPV6_HEAD_LEN);
            pstIpInfo->usDstPort      = IP_GET_VAL_NTOH_U16(pData, (IP_IPV6_HEAD_LEN + IP_IPV6_DST_PORT_POS));
            break;

        case IP_IPV6_PROTO_ICMP:
            pstIpInfo->ucIcmpType     = IP_GET_VAL_NTOH_U8(pData, IP_IPV6_HEAD_LEN);
            if ((IP_IPV6_ICMP_ECHO_REQUEST == pstIpInfo->ucIcmpType)
                ||(IP_IPV6_ICMP_ECHO_REPLY == pstIpInfo->ucIcmpType))
            {
                pstIpInfo->ulL4Id     = IP_GET_VAL_NTOH_U32(pData, (IP_IPV6_HEAD_LEN + IP_IPV4_ICMP_IDENTIFY_POS));
            }
            break;

        default:
            break;
    }

    return;
}


VOS_UINT32 IPS_MNTN_GetIPInfoCfg(VOS_UINT16 usType)
{
    if ((ID_IPS_TRACE_IP_ADS_UL == usType)||(ID_IPS_TRACE_IP_ADS_DL == usType))
    {
        return g_stIpsMntnIPInfoCfg.enAdsIPConfig;
    }

    if ((ID_IPS_TRACE_IP_USB_UL == usType)||(ID_IPS_TRACE_IP_USB_DL == usType))
    {
        return g_stIpsMntnIPInfoCfg.enUsbIPConfig;
    }

    return PS_FALSE;
}


VOS_VOID IPS_MNTN_TraceIpInfo(struct sk_buff *skb,VOS_UINT16 usType)
{
    IPS_MNTN_IP_INFO_STRU       *pstIpInfo;
    VOS_UINT32                  ulRst;
    VOS_UINT32                  ulTraceMsgLen;
    VOS_UINT8                   *pIpData;
    VOS_UINT16                  usIpDataLen;
    VOS_UINT32                  ulOffsetLen;
    VOS_UINT8                  *pucTraceData;

    ulRst = IPS_MNTN_GetIPInfoCfg(usType);
    if(PS_FALSE == ulRst)
    {
        /*不捕获该报文*/
        return;
    }

    if ((VOS_NULL_PTR == skb)||(NULL == skb->data))
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "IPS, IPS_MNTN_TraceIpInfo, ERROR, skb or skb->data is NULL ");
        return;
    }

    /* USB抓取的skb data可能是IP数据包或者是带MAC头的IP数据包*/
    pIpData         = skb->data;

    if (IP_IPV4_VER_AND_HEAD == *pIpData)
    {
        usIpDataLen = IP_GET_VAL_NTOH_U16(pIpData, IP_IPV4_DATA_LEN_POS);
    }
    else if (IP_IPV6_VER_AND_HEAD == *pIpData)
    {
        usIpDataLen = IP_GET_VAL_NTOH_U16(pIpData, IP_IPV6_DATA_LEN_POS);
    }
    else
    {
        usIpDataLen = 0;
    }

    if (usIpDataLen != skb->len)
    {
        pIpData     += MAC_HEAD_LEN;
    }

    /*构建透明消息*/
    TTF_GET_OFFSET(ulOffsetLen, OM_APP_TRACE_STRU, aucPara);    /* [false alarm]:TTF_GET_OFFSET为标准库函数调用，不会有NULL指针问题 */
    pucTraceData            = NF_EXT_MEM_ALLOC(ACPU_PID_NFEXT, MNTN_IP_INFO_LEN + ulOffsetLen);

    if (VOS_NULL_PTR == pucTraceData)
    {
        PS_LOG(ACPU_PID_NFEXT, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "NF_EXT_MEM_ALLOC pucTraceData fail");
        return;
    }

    pstIpInfo               = (IPS_MNTN_IP_INFO_STRU *)((VOS_UINT_PTR)pucTraceData + ulOffsetLen);
    pstIpInfo->ucIpVer      = *pIpData>>IP_PROTO_VERSION_POS;

    /* IP报文解析TCP,UDP,ICMP，不解析IP的扩展头*/
    if (IP_IPV4_PROTO_VERSION == pstIpInfo->ucIpVer)
    {
        pstIpInfo->usDataLen  = IP_GET_VAL_NTOH_U16(pIpData, IP_IPV4_DATA_LEN_POS);
        if ((pstIpInfo->usDataLen == skb->len)||(pstIpInfo->usDataLen == (skb->len - MAC_HEAD_LEN)))
        {
            pstIpInfo->usIp4Id    = IP_GET_VAL_NTOH_U16(pIpData, IP_IPV4_IDENTIFY_POS);
            pstIpInfo->ucL4Proto  = IP_GET_VAL_NTOH_U8(pIpData, IP_IPV4_PROTO_POS);

            IPS_MNTN_Ipv4DataParse(pstIpInfo, pIpData);
        }
    }

    if (IP_IPV6_PROTO_VERSION == pstIpInfo->ucIpVer)
    {
        pstIpInfo->usDataLen  = IP_GET_VAL_NTOH_U16(pIpData, IP_IPV6_DATA_LEN_POS);
        if ((pstIpInfo->usDataLen == skb->len)||(pstIpInfo->usDataLen == (skb->len - MAC_HEAD_LEN)))
        {
            pstIpInfo->ucL4Proto  = IP_GET_VAL_NTOH_U8(pIpData, IP_IPV6_PROTO_POS);
            pstIpInfo->usIp4Id    = 0;

            IPS_MNTN_Ipv6DataParse(pstIpInfo, pIpData);
        }
    }

    ulTraceMsgLen = MNTN_IP_INFO_LEN + ulOffsetLen;
    IPS_MNTN_FillMsg((OM_APP_TRACE_STRU *)pucTraceData, usType, ulTraceMsgLen);

    OM_AcpuSendData((OM_RSP_PACKET_STRU*)pucTraceData, (VOS_UINT16)ulTraceMsgLen);

    NF_EXT_MEM_FREE(ACPU_PID_NFEXT, pucTraceData);
    return;
}

