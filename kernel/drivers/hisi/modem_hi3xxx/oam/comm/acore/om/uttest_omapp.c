
#include "omprivate.h"
#include "OmAppRl.h"


VOS_INT32 uttest_OM_AcpuRegisterReqProc_case3(VOS_VOID)
{
    VOS_UINT8 ucModuleId = 50;

    g_astAcpuOmFuncIdProcTbl[ucModuleId].ulFuncType = ucModuleId;

    return OM_AcpuRegisterReqProc(ucModuleId, VOS_NULL_PTR);
}



VOS_INT32 uttest_OM_AcpuRegisterReqProc_case4(VOS_VOID)
{
    VOS_UINT8 ucModuleId = 50;

    g_astAcpuOmFuncIdProcTbl[ucModuleId].ulFuncType = 0;

    return OM_AcpuRegisterReqProc(ucModuleId, VOS_NULL_PTR);
}


VOS_UINT32 uttest_OM_AcpuSendDataMNTNChannel_case1(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    OM_RSP_PACKET_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    usMsgLen = 0;

    if (VOS_ERR != OM_AcpuSendDataMNTNChannel(&stCtrlInfo, &stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataMNTNChannel_case2(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    OM_RSP_PACKET_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    usMsgLen = sizeof(OM_RSP_PACKET_STRU);

    if (VOS_ERR != OM_AcpuSendDataMNTNChannel(&stCtrlInfo, VOS_NULL_PTR, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataMNTNChannel_case3(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    OM_RSP_PACKET_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    usMsgLen = sizeof(OM_RSP_PACKET_STRU);

    if (VOS_ERR != OM_AcpuSendDataMNTNChannel(&stCtrlInfo, &stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataMNTNChannel_case4(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    OM_APP_MSG_EX_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    usMsgLen = sizeof(OM_APP_MSG_EX_STRU);

    g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

    stMsg.ucFuncType = OM_FS_FUNC;

    if (VOS_ERR != OM_AcpuSendDataMNTNChannel(&stCtrlInfo, (OM_RSP_PACKET_STRU*)&stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataMNTNChannel_case5(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    OM_APP_MSG_EX_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    usMsgLen = sizeof(OM_RSP_PACKET_STRU);

    g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

    stMsg.ucFuncType = OM_QUERY_FUNC;
    stMsg.usPrimId   = OM_APP_ESTABLISH_CNF;

    if (VOS_OK != OM_AcpuSendDataMNTNChannel(&stCtrlInfo, (OM_RSP_PACKET_STRU*)&stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataCbtChannel_case1(VOS_VOID)
{
    OM_APP_MSG_EX_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    usMsgLen = sizeof(OM_APP_MSG_EX_STRU);

    g_stAcpuCbtCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

    stMsg.ucFuncType    = OM_WRF_FUNC;

    if (VOS_OK != OM_AcpuSendDataCbtChannel((OM_RSP_PACKET_STRU*)&stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataCbtChannel_case2(VOS_VOID)
{
    OM_APP_MSG_EX_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    usMsgLen = sizeof(OM_APP_MSG_EX_STRU);

    g_stAcpuCbtCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

    stMsg.ucFuncType    = OM_QUERY_FUNC;
    stMsg.usPrimId      = OM_APP_ESTABLISH_CNF;

    if (VOS_ERR != OM_AcpuSendDataCbtChannel((OM_RSP_PACKET_STRU*)&stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataCbtChannel_case3(VOS_VOID)
{
    OM_APP_MSG_EX_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    usMsgLen = sizeof(OM_APP_MSG_EX_STRU);

    g_stAcpuCbtCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

    stMsg.ucFuncType    = OM_QUERY_FUNC;
    stMsg.usPrimId      = OM_APP_ESTABLISH_CNF;

    if (VOS_ERR != OM_AcpuSendDataCbtChannel((OM_RSP_PACKET_STRU*)&stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataCbtChannel_case4(VOS_VOID)
{
    OM_APP_MSG_EX_STRU                 *pstMsg;
    VOS_UINT16                          usMsgLen;

    pstMsg   = (OM_APP_MSG_EX_STRU*)malloc(5*1024);

    usMsgLen = 5*1024;

    g_stAcpuCbtCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

    pstMsg->ucFuncType    = OM_QUERY_FUNC;
    pstMsg->usPrimId      = OM_APP_ESTABLISH_CNF;

    if (VOS_ERR != OM_AcpuSendDataCbtChannel((OM_RSP_PACKET_STRU*)pstMsg, usMsgLen))
    {
        free(pstMsg);

        return VOS_ERR;
    }

    free(pstMsg);

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataCbtChannel_case5(VOS_VOID)
{
    OM_APP_MSG_EX_STRU                 *pstMsg;
    VOS_UINT16                          usMsgLen;

    pstMsg   = (OM_APP_MSG_EX_STRU*)malloc(5*1024);

    usMsgLen = 5*1024;

    g_stAcpuCbtCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

    pstMsg->ucFuncType    = OM_QUERY_FUNC;
    pstMsg->usPrimId      = OM_APP_ESTABLISH_CNF;

    if (VOS_OK != OM_AcpuSendDataCbtChannel((OM_RSP_PACKET_STRU*)pstMsg, usMsgLen))
    {
        free(pstMsg);

        return VOS_ERR;
    }

    free(pstMsg);

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataChannel_case1(VOS_VOID)
{
    OM_RSP_PACKET_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    if (VOS_OK != OM_AcpuSendDataChannel(OM_LOGIC_CHANNEL_CNF, &stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataChannel_case2(VOS_VOID)
{
    OM_RSP_PACKET_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    if (VOS_OK != OM_AcpuSendDataChannel(OM_LOGIC_CHANNEL_IND, &stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataChannel_case3(VOS_VOID)
{
    OM_RSP_PACKET_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    if (VOS_OK != OM_AcpuSendDataChannel(OM_LOGIC_CHANNEL_CBT, &stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuSendDataChannel_case4(VOS_VOID)
{
    OM_RSP_PACKET_STRU                  stMsg;
    VOS_UINT16                          usMsgLen;

    if (VOS_ERR != OM_AcpuSendDataChannel(OM_LOGIC_CHANNEL_BUTT, &stMsg, usMsgLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID uttest_OM_AcpuSendResultChannel_case1(VOS_VOID)
{
    OM_AcpuSendResultChannel(OM_LOGIC_CHANNEL_CBT, OM_GRF_FUNC, VOS_OK, OM_APP_ESTABLISH_CNF);
}


VOS_VOID uttest_OM_AcpuSendResult_case1(VOS_VOID)
{
    OM_AcpuSendResult(OM_GRF_FUNC, VOS_OK, OM_APP_ESTABLISH_CNF);
}


VOS_UINT32 uttest_OM_AcpuSendData_case1(VOS_VOID)
{
    OM_RSP_PACKET_STRU                  stMsg;

    return OM_AcpuSendData(&stMsg, sizeof(stMsg));
}


VOS_VOID uttest_OM_AcpuSendContentChannel_case1(VOS_VOID)
{
    OM_RSP_PACKET_STRU                  stOmToAppMsg;

    OM_AcpuSendContentChannel(OM_LOGIC_CHANNEL_CBT, OM_GRF_FUNC, &stOmToAppMsg, OM_APP_ESTABLISH_CNF);
}


VOS_VOID uttest_OM_AcpuSendContent_case1(VOS_VOID)
{
    OM_RSP_PACKET_STRU                  stOmToAppMsg;

    OM_AcpuSendContent(OM_GRF_FUNC, &stOmToAppMsg, OM_APP_ESTABLISH_CNF);
}


VOS_VOID uttest_OM_AcpuConnecInfoTimeOutMsgProc_case1(VOS_VOID)
{
    g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

    OM_AcpuConnecInfoTimeOutMsgProc();
}


VOS_VOID uttest_OM_AcpuAtMsgProc_case1(VOS_VOID)
{
    AT_OM_MSG_STRU                      stAtOmMsg;

    stAtOmMsg.ulMsgName = AT_OM_HSIC_PORT_CONNECT;

    OM_AcpuAtMsgProc((MsgBlock*)&stAtOmMsg);
}


VOS_VOID uttest_OM_AcpuAtMsgProc_case2(VOS_VOID)
{
    AT_OM_MSG_STRU                      stAtOmMsg;

    stAtOmMsg.ulMsgName = AT_OM_HSIC_PORT_DISCONNECT;

    OM_AcpuAtMsgProc((MsgBlock*)&stAtOmMsg);
}


VOS_VOID uttest_OM_AcpuMsgProc_case1(VOS_VOID)
{
    VOS_UINT8                           aucMsg[60];
    MsgBlock                           *pMsg;

    memset(&aucMsg, 0, sizeof(aucMsg));

    pMsg = (MsgBlock *)aucMsg;

    pMsg->ulSenderPid = UEPS_PID_OMRL;

    OM_AcpuMsgProc(pMsg);
}


VOS_UINT32 uttest_OM_AcpuInit_case1(VOS_VOID)
{
    return OM_AcpuInit();
}

unsigned int uttest_NV_ReadEx_stub10(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength)
{
    OM_CHANNLE_PORT_CFG_STRU            stPortCfg;

    stPortCfg.enPortNum = CPM_OM_PORT_TYPE_FS;

    memcpy(pItem, &stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU));

    return VOS_OK;
}


VOS_VOID uttest_OM_AutoConfigProc_case1(VOS_VOID)
{
    OM_AutoConfigProc();
}


VOS_UINT32 uttest_OM_AcpuPidInit_case1(VOS_VOID)
{
    return OM_AcpuPidInit(VOS_IP_BUTT);
}


VOS_UINT32 uttest_OM_AcpuFidInit_case1(VOS_VOID)
{
    if (VOS_ERR != OM_AcpuFidInit(VOS_IP_LOAD_CONFIG))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuFidInit_case2(VOS_VOID)
{
    if (VOS_ERR != OM_AcpuFidInit(VOS_IP_LOAD_CONFIG))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 uttest_OM_AcpuFidInit_case3(VOS_VOID)
{
    return OM_AcpuFidInit(VOS_IP_LOAD_CONFIG);
}
