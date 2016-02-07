/* 头文件包含 */
#include "OmAppRl.h"
#include "Omappagent.h"

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_JudgeCbtMsg_case1(VOS_VOID)
{
    return OMRL_JudgeCbtMsg(OM_FS_FUNC);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_JudgeCbtMsg_case2(VOS_VOID)
{
    return OMRL_JudgeCbtMsg(OM_AIR_FUNC);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgDispatch_case1(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucAppToOmMsg[200];
    APP_OM_MSG_EX_STRU                 *pstAppToOmMsg;

    stCtrlInfo.enChannel    = OM_LOGIC_CHANNEL_CBT;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg = (MsgBlock *)aucAppToOmMsg;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulLength = sizeof(aucAppToOmMsg) - VOS_MSG_HEAD_LENGTH;;
    pstAppToOmMsg = (APP_OM_MSG_EX_STRU *)(stCtrlInfo.stMsgCombineInfo.pstWholeMsg->aucValue);

    pstAppToOmMsg->ucFuncType = OM_AIR_FUNC;

    return OMRL_AcpuMsgDispatch(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgDispatch_case2(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucAppToOmMsg[200];
    APP_OM_MSG_EX_STRU                 *pstAppToOmMsg;

    stCtrlInfo.enChannel    = OM_LOGIC_CHANNEL_CBT;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg = (MsgBlock *)aucAppToOmMsg;

    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulReceiverPid = WUEPS_PID_OM;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulLength      = sizeof(aucAppToOmMsg) - VOS_MSG_HEAD_LENGTH;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU *)(stCtrlInfo.stMsgCombineInfo.pstWholeMsg->aucValue);

    pstAppToOmMsg->ucFuncType = OM_NOSIG_FUNC;

    return OMRL_AcpuMsgDispatch(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgDispatch_case3(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucAppToOmMsg[200];
    APP_OM_MSG_EX_STRU                 *pstAppToOmMsg;

    stCtrlInfo.enChannel    = OM_LOGIC_CHANNEL_CNF;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg = (MsgBlock *)aucAppToOmMsg;

    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulReceiverPid = WUEPS_PID_OM;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulLength      = sizeof(aucAppToOmMsg) - VOS_MSG_HEAD_LENGTH;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU *)(stCtrlInfo.stMsgCombineInfo.pstWholeMsg->aucValue);

    pstAppToOmMsg->ucFuncType = OM_NOSIG_FUNC;

    return OMRL_AcpuMsgDispatch(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgDispatch_case4(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucAppToOmMsg[200];
    APP_OM_MSG_EX_STRU                 *pstAppToOmMsg;

    stCtrlInfo.enChannel    = OM_LOGIC_CHANNEL_CNF;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg = (MsgBlock *)aucAppToOmMsg;

    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulReceiverPid = ACPU_PID_OM;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulLength      = sizeof(aucAppToOmMsg) - VOS_MSG_HEAD_LENGTH;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU *)(stCtrlInfo.stMsgCombineInfo.pstWholeMsg->aucValue);

    pstAppToOmMsg->ucFuncType = OM_GREEN_FUNC;

    return OMRL_AcpuMsgDispatch(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgDispatch_case5(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucAppToOmMsg[200];
    APP_OM_MSG_EX_STRU                 *pstAppToOmMsg;

    stCtrlInfo.enChannel    = OM_LOGIC_CHANNEL_CNF;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg = (MsgBlock *)aucAppToOmMsg;

    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulReceiverPid = DSP_PID_HIFI_OM;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulLength      = sizeof(aucAppToOmMsg) - VOS_MSG_HEAD_LENGTH;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU *)(stCtrlInfo.stMsgCombineInfo.pstWholeMsg->aucValue);

    pstAppToOmMsg->ucFuncType = OM_GREEN_FUNC;

    return OMRL_AcpuMsgDispatch(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgDispatch_case6(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucAppToOmMsg[200];
    APP_OM_MSG_EX_STRU                 *pstAppToOmMsg;

    stCtrlInfo.enChannel    = OM_LOGIC_CHANNEL_CNF;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg = (MsgBlock *)aucAppToOmMsg;

    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulReceiverPid = WUEPS_PID_USIM;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg->ulLength      = sizeof(aucAppToOmMsg) - VOS_MSG_HEAD_LENGTH;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU *)(stCtrlInfo.stMsgCombineInfo.pstWholeMsg->aucValue);

    pstAppToOmMsg->ucFuncType = OM_GREEN_FUNC;

    return OMRL_AcpuMsgDispatch(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgCombine_case1(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucData[100];
    VOS_UINT16                          usLen = sizeof(aucData);

    return OMRL_AcpuMsgCombine(&stCtrlInfo, VOS_NULL_PTR, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgCombine_case2(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucData[100];
    VOS_UINT16                          usLen = sizeof(aucData);
    OM_APP_MSG_SEGMENT_STRU            *pstOmAppMsgSeg;

    pstOmAppMsgSeg = (OM_APP_MSG_SEGMENT_STRU *)aucData;

    pstOmAppMsgSeg->usSegLen = usLen + 1;

    return OMRL_AcpuMsgCombine(&stCtrlInfo, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgCombine_case3(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucData[100];
    VOS_UINT16                          usLen = sizeof(aucData);
    OM_APP_MSG_SEGMENT_STRU            *pstOmAppMsgSeg;

    pstOmAppMsgSeg = (OM_APP_MSG_SEGMENT_STRU *)aucData;

    pstOmAppMsgSeg->usSegLen = usLen;
    pstOmAppMsgSeg->ucSegNum = 3;
    pstOmAppMsgSeg->ucSegSn  = 2;


    stCtrlInfo.stMsgCombineInfo.ucTotalSegNum = 4;

    return OMRL_AcpuMsgCombine(&stCtrlInfo, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgCombine_case4(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucData[100];
    VOS_UINT16                          usLen = sizeof(aucData);
    OM_APP_MSG_SEGMENT_STRU            *pstOmAppMsgSeg;

    pstOmAppMsgSeg = (OM_APP_MSG_SEGMENT_STRU *)aucData;

    pstOmAppMsgSeg->usSegLen = usLen;
    pstOmAppMsgSeg->ucSegNum = 5;
    pstOmAppMsgSeg->ucSegSn  = 3;


    stCtrlInfo.stMsgCombineInfo.ucTotalSegNum = 5;
    stCtrlInfo.stMsgCombineInfo.ucExpectedSegSn = 2;

    return OMRL_AcpuMsgCombine(&stCtrlInfo, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgCombine_case5(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucData[100];
    VOS_UINT16                          usLen = sizeof(aucData);
    OM_APP_MSG_SEGMENT_STRU            *pstOmAppMsgSeg;

    pstOmAppMsgSeg = (OM_APP_MSG_SEGMENT_STRU *)aucData;

    pstOmAppMsgSeg->usSegLen = usLen;
    pstOmAppMsgSeg->ucSegNum = 5;
    pstOmAppMsgSeg->ucSegSn  = 2;


    stCtrlInfo.stMsgCombineInfo.ucTotalSegNum = 5;
    stCtrlInfo.stMsgCombineInfo.ucExpectedSegSn = 2;
    stCtrlInfo.stMsgCombineInfo.ulTotalMsgLen = 150;
    stCtrlInfo.stMsgCombineInfo.ulMoveLen = 100;

    return OMRL_AcpuMsgCombine(&stCtrlInfo, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgCombine_case6(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucData[100];
    VOS_UINT8                           aucMsg[1000];
    VOS_UINT16                          usLen = sizeof(aucData);
    OM_APP_MSG_SEGMENT_STRU            *pstOmAppMsgSeg;

    pstOmAppMsgSeg = (OM_APP_MSG_SEGMENT_STRU *)aucData;

    pstOmAppMsgSeg->usSegLen = usLen;
    pstOmAppMsgSeg->ucSegNum = 5;
    pstOmAppMsgSeg->ucSegSn  = 5;


    stCtrlInfo.stMsgCombineInfo.ucTotalSegNum = 5;
    stCtrlInfo.stMsgCombineInfo.ucExpectedSegSn = 5;
    stCtrlInfo.stMsgCombineInfo.ulTotalMsgLen = OM_RL_TOTAL_MSG_MAX_LEN + 1;
    stCtrlInfo.stMsgCombineInfo.ulMoveLen = 100;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg = (MsgBlock *)aucMsg;

    return OMRL_AcpuMsgCombine(&stCtrlInfo, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgCombine_case7(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucData[100];
    VOS_UINT8                           aucMsg[1000];
    VOS_UINT16                          usLen = sizeof(aucData);
    OM_APP_MSG_SEGMENT_STRU            *pstOmAppMsgSeg;

    pstOmAppMsgSeg = (OM_APP_MSG_SEGMENT_STRU *)aucData;

    pstOmAppMsgSeg->usSegLen = usLen;
    pstOmAppMsgSeg->ucSegNum = 5;
    pstOmAppMsgSeg->ucSegSn  = 5;


    stCtrlInfo.stMsgCombineInfo.ucTotalSegNum = 5;
    stCtrlInfo.stMsgCombineInfo.ucExpectedSegSn = 5;
    stCtrlInfo.stMsgCombineInfo.ulTotalMsgLen = 400;
    stCtrlInfo.stMsgCombineInfo.ulMoveLen = 100;
    stCtrlInfo.stMsgCombineInfo.pstWholeMsg = (MsgBlock *)aucMsg;

    return OMRL_AcpuMsgCombine(&stCtrlInfo, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_RcvChannelData_case1(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU        stCtrlInfo;
    VOS_UINT8                           aucData[100];
    VOS_UINT16                          usLen = sizeof(aucData);

    return OMRL_RcvChannelData(&stCtrlInfo, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_RcvCbtChannel_case1(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32           enChanlID=SOCP_DECODER_CBT;
    VOS_UINT8                           aucData[100];
    VOS_UINT16                          usLen = sizeof(aucData);

    return OMRL_RcvCbtChannel(enChanlID, aucData, usLen, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_RcvCnfChannel_case1(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32           enChanlID=SOCP_DECODER_CBT;
    VOS_UINT8                           aucData[100];
    VOS_UINT16                          usLen = sizeof(aucData);

    return OMRL_RcvCnfChannel(enChanlID, aucData, usLen, aucData, usLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_OMRL_AcpuConnectInfoReportEvent_case1(VOS_VOID)
{
    OM_CONNECT_INFO_CNF_STRU            stConnectInfo;

    OMRL_AcpuConnectInfoReportEvent(&stConnectInfo);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_OMRL_AcpuConnectInfoReportEvent_case2(VOS_VOID)
{
    OM_CONNECT_INFO_CNF_STRU            stConnectInfo;

    OMRL_AcpuConnectInfoReportEvent(&stConnectInfo);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuReportEvent_case1(VOS_VOID)
{
    return OMRL_AcpuReportEvent();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuReportEvent_case2(VOS_VOID)
{
    return OMRL_AcpuReportEvent();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuReportEvent_case3(VOS_VOID)
{
    return OMRL_AcpuReportEvent();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuSendCbtData_case1(VOS_VOID)
{
    VOS_CHAR                            aucData[200];

    return OMRL_AcpuSendCbtData(aucData, sizeof(aucData));
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuSendCbtData_case2(VOS_VOID)
{
    VOS_CHAR                            aucData[200];

    return OMRL_AcpuSendCbtData(aucData, sizeof(aucData));
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuSendCbtData_case3(VOS_VOID)
{
    VOS_CHAR                            aucData[200];

    return OMRL_AcpuSendCbtData(aucData, sizeof(aucData));
}


/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuReleaseSocpData_case1(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;

    return OMRL_AcpuReleaseSocpData(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuReleaseSocpData_case2(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;

    return OMRL_AcpuReleaseSocpData(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuReleaseSocpData_case3(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;

    return OMRL_AcpuReleaseSocpData(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuReleaseSocpData_case4(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;

    return OMRL_AcpuReleaseSocpData(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuReleaseSocpData_case5(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;

    return OMRL_AcpuReleaseSocpData(&stCtrlInfo);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuSendSocpData_case1(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucHead[10];
    VOS_UINT32                          ulHeadLen=sizeof(aucHead);
    VOS_UINT8                           aucData[200];
    VOS_UINT32                          ulDataLen = sizeof(aucData);

    return OMRL_AcpuSendSocpData(&stCtrlInfo, aucHead, ulHeadLen, aucData, ulDataLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuSendSocpData_case2(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucHead[10];
    VOS_UINT32                          ulHeadLen=sizeof(aucHead);
    VOS_UINT8                           aucData[200];
    VOS_UINT32                          ulDataLen = sizeof(aucData);

    return OMRL_AcpuSendSocpData(&stCtrlInfo, aucHead, ulHeadLen, aucData, ulDataLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuSendSocpData_case3(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucHead[10];
    VOS_UINT32                          ulHeadLen=sizeof(aucHead);
    VOS_UINT8                           aucData[200];
    VOS_UINT32                          ulDataLen = sizeof(aucData);

    return OMRL_AcpuSendSocpData(&stCtrlInfo, aucHead, ulHeadLen, aucData, ulDataLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuSendSocpData_case4(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucHead[10];
    VOS_UINT32                          ulHeadLen=sizeof(aucHead);
    VOS_UINT8                           aucData[200];
    VOS_UINT32                          ulDataLen = sizeof(aucData);

    stCtrlInfo.stUeToPcSucRecord.ulMaxUsedBufSize = 100;

    return OMRL_AcpuSendSocpData(&stCtrlInfo, aucHead, ulHeadLen, aucData, ulDataLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuIsBDEnough_case1(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;

    return OMRL_AcpuIsBDEnough(&stCtrlInfo, 100);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuIsBDEnough_case2(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;

    return OMRL_AcpuIsBDEnough(&stCtrlInfo, 100);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuIsBDEnough_case3(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;

    return OMRL_AcpuIsBDEnough(&stCtrlInfo, 100);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuProcessSendData_case1(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[100];
    VOS_UINT8                           ucMsgCnt = 1;
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);

    return OMRL_AcpuProcessSendData(&stCtrlInfo, aucMsg, ucMsgCnt, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuProcessSendData_case2(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[5000];
    VOS_UINT8                           ucMsgCnt = 2;
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    pstAppTrace = (OM_APP_TRACE_STRU_EX *)aucMsg;
    pstAppTrace->stAppHeader.ucFuncType = OM_TRACE_FUNC;
    pstAppTrace->usPrimId               = OM_APP_TRACE_IND;

    return OMRL_AcpuProcessSendData(&stCtrlInfo, aucMsg, ucMsgCnt, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuProcessSendData_case3(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[5000];
    VOS_UINT8                           ucMsgCnt = 3;
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    pstAppTrace = (OM_APP_TRACE_STRU_EX *)aucMsg;
    pstAppTrace->stAppHeader.ucFuncType = OM_TRACE_FUNC;
    pstAppTrace->usPrimId               = OM_APP_TRACE_IND;

    return OMRL_AcpuProcessSendData(&stCtrlInfo, aucMsg, ucMsgCnt, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuProcessSendData_case4(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[1000];
    VOS_UINT8                           ucMsgCnt = 1;
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    pstAppTrace = (OM_APP_TRACE_STRU_EX *)aucMsg;
    pstAppTrace->stAppHeader.ucFuncType = OM_TRACE_FUNC;
    pstAppTrace->usPrimId               = OM_APP_TRACE_IND;

    return OMRL_AcpuProcessSendData(&stCtrlInfo, aucMsg, ucMsgCnt, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuProcessSendData_case5(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[5000];
    VOS_UINT8                           ucMsgCnt = 2;
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    pstAppTrace = (OM_APP_TRACE_STRU_EX *)aucMsg;
    pstAppTrace->stAppHeader.ucFuncType = OM_TRACE_FUNC;
    pstAppTrace->usPrimId               = OM_APP_TRACE_IND;

    return OMRL_AcpuProcessSendData(&stCtrlInfo, aucMsg, ucMsgCnt, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuProcessSendData_case6(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[5000];
    VOS_UINT8                           ucMsgCnt = 2;
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    pstAppTrace = (OM_APP_TRACE_STRU_EX *)aucMsg;
    pstAppTrace->stAppHeader.ucFuncType = OM_TRACE_FUNC;
    pstAppTrace->usPrimId               = OM_APP_TRACE_IND;

    return OMRL_AcpuProcessSendData(&stCtrlInfo, aucMsg, ucMsgCnt, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgSplit_case1(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[5000];
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    return OMRL_AcpuMsgSplit(&stCtrlInfo, aucMsg, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgSplit_case2(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[5000];
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    return OMRL_AcpuMsgSplit(&stCtrlInfo, aucMsg, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgSplit_case3(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[5000];
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    return OMRL_AcpuMsgSplit(&stCtrlInfo, aucMsg, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_OMRL_AcpuMsgSplit_case4(VOS_VOID)
{
    OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU    stCtrlInfo;
    VOS_UINT8                           aucMsg[5000];
    VOS_UINT16                          usMsgLen=sizeof(aucMsg);
    OM_APP_TRACE_STRU_EX               *pstAppTrace;

    return OMRL_AcpuMsgSplit(&stCtrlInfo, aucMsg, usMsgLen);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_OMRL_Show_case1(VOS_VOID)
{
    OmAcpuSendIndSucShow();

    OmAcpuSendCnfSucShow();

    OmAcpuSendIndErrShow();

    OmAcpuSendCnfErrShow();

    OmAcpuRcvCbtSucShow();

    OmAcpuRcvCnfSucShow();

    OmAcpuRcvCbtErrShow();

    OmAcpuRcvCnfErrShow();

    OmAcpuShowCombineInfo();

    return;
}






VOS_UINT32 uttest_OMRL_AcpuHeadProc_case1(VOS_VOID)
{
	OMRL_RCV_CHAN_CTRL_INFO_STRU stCtrlInfo = {0};
	VOS_UINT8                           aucLinkData[] = {0x55, 0x55, 0xaa, 0xaa, 0x10, 0x00, 0x01, 0x01,
		0x0a, 0x00, 0x08, 0x00, 0xb1, 0x80, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0x55, 0x55};
	VOS_UINT32 ulSize = 2;

	return OMRL_AcpuHeadProc(&stCtrlInfo, aucLinkData, ulSize);
}


VOS_UINT32 uttest_OMRL_AcpuHeadProc_case2(VOS_VOID)
{
	OMRL_RCV_CHAN_CTRL_INFO_STRU stCtrlInfo = {0};
	VOS_UINT8                           aucLinkData[] = {0x00, 0x55, 0xaa, 0xaa, 0x10, 0x00, 0x01, 0x01,
		0x0a, 0x00, 0x08, 0x00, 0xb1, 0x80, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0x55, 0x55};
	VOS_UINT32 ulSize = sizeof(aucLinkData);

	return OMRL_AcpuHeadProc(&stCtrlInfo, aucLinkData, ulSize);
}

VOS_UINT32 uttest_OMRL_AcpuHeadProc_case3(VOS_VOID)
{
	OMRL_RCV_CHAN_CTRL_INFO_STRU stCtrlInfo = {0};
	VOS_UINT8                           aucLinkData[] = {0x55, 0x55, 0xaa, 0xaa, 0x10, 0x00, 0x01, 0x01,
		0x0a, 0x00, 0x08, 0x00, 0xb1, 0x80, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0x55, 0x00};
	VOS_UINT32 ulSize = sizeof(aucLinkData);

	return OMRL_AcpuHeadProc(&stCtrlInfo, aucLinkData, ulSize);
}

VOS_UINT32 uttest_OMRL_AcpuHeadProc_case4(VOS_VOID)
{
	OMRL_RCV_CHAN_CTRL_INFO_STRU stCtrlInfo = {0};
	VOS_UINT8                           aucLinkData[] = {0x55, 0x55, 0xaa, 0xaa, 0x10, 0x00, 0x01, 0x01,
		0x0a, 0x00, 0x08, 0x00, 0xb3, 0x80, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0x55, 0x55};
	VOS_UINT32 ulSize = sizeof(aucLinkData);

	stCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

	return OMRL_AcpuHeadProc(&stCtrlInfo, aucLinkData, ulSize);
}

VOS_UINT32 uttest_OMRL_AcpuHeadProc_case5(VOS_VOID)
{
	OMRL_RCV_CHAN_CTRL_INFO_STRU stCtrlInfo = {0};
	VOS_UINT8                           aucLinkData[] = {0x55, 0x55, 0xaa, 0xaa, 0x10, 0x00, 0x01, 0x01,
		0x0a, 0x00, 0x08, 0x00, 0xb1, 0x80, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0x55, 0x55};

	VOS_UINT32 ulSize = sizeof(aucLinkData);

	stCtrlInfo.ulOMSwitchOnOff = OM_STATE_IDLE;

	return OMRL_AcpuHeadProc(&stCtrlInfo, aucLinkData, ulSize);
}

VOS_UINT32 uttest_OMRL_AcpuUsbFrameInit_case1(VOS_VOID)
{
	return OMRL_AcpuUsbFrameInit();
}

VOS_VOID uttest_OmAcpuInitVar_case1(VOS_VOID)
{
	OmAcpuInitVar();
}

VOS_UINT32 uttest_OMRL_AcpuInit_case1(VOS_VOID)
{
	return OMRL_AcpuInit();
}
