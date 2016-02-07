/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AtCmdFtmProc.c
  版 本 号   : 初稿
  作    者   : l60609
  生成日期   : 2013年3月13日
  最近修改   :
  功能描述   : 工程测试命令处理文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年3月13日
    作    者   : l60609
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "AtCmdFtmProc.h"
#include "ATCmdProc.h"
#include "AtMtaInterface.h"

/* Added by z60575 for LOG35_TL, 2013-8-9 begin */
#if (FEATURE_ON == FEATURE_LTE)
#include "msp_diag.h"
#endif
/* Added by z60575 for LOG35_TL, 2013-8-9 end */

#include "AtTestParaCmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 -e960*/
#define    THIS_FILE_ID                 PS_FILE_ID_AT_CMD_FTM_PROC_C
/*lint +e767 +e960*/

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : At_SetLogPortPara
 功能描述  : ^LOGPORT的设置函数
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月20日
    作    者   : l60609
    修改内容   : 新生成函数
  2.日    期   : 2013年8月9日
    作    者   : z60575
    修改内容   : LOG35_TL调用TL接口
*****************************************************************************/
VOS_UINT32 At_SetLogPortPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulOmLogPort;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多或没有 */
    if ((2 < gucAtParaIndex) || (0 == gucAtParaIndex))
    {
        return AT_ERROR;
    }

    if (AT_LOG_PORT_USB == gastAtParaList[0].ulParaValue)
    {
        ulOmLogPort = CPM_OM_PORT_TYPE_USB;
    }
    else
    {
        ulOmLogPort = CPM_OM_PORT_TYPE_VCOM;
    }

    /* 参数只有一个，默认永久生效 */
    if (1 == gucAtParaIndex)
    {
        gastAtParaList[1].ulParaValue = VOS_TRUE;
    }

    /* 调用OM的接口 */
    ulRslt = PPM_LogPortSwitch(ulOmLogPort, gastAtParaList[1].ulParaValue);

    if (VOS_OK == ulRslt)
    {
        return AT_OK;
    }
    else
    {
        return AT_ERROR;
    }
}

/*****************************************************************************
 函 数 名  : At_QryLogPortPara
 功能描述  : ^LOGPORT的查询函数
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月20日
    作    者   : l60609
    修改内容   : 新生成函数
  2.日    期   : 2013年8月9日
    作    者   : z60575
    修改内容   : LOG35_TL调用TL接口
*****************************************************************************/
VOS_UINT32 At_QryLogPortPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulOmLogPort;
    VOS_UINT32                          ulAtLogPort;
    VOS_UINT32                          ulRslt;

    usLength                            = 0;
    ulOmLogPort                         = AT_LOG_PORT_USB;

    ulRslt = PPM_QueryLogPort(&ulOmLogPort);

    if (VOS_OK != ulRslt)
    {
        return AT_ERROR;
    }

    if (COMM_LOG_PORT_USB == ulOmLogPort)
    {
        ulAtLogPort = AT_LOG_PORT_USB;
    }
    else
    {
        ulAtLogPort = AT_LOG_PORT_VCOM;
    }

    usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr,
                                       (VOS_CHAR*)pgucAtSndCodeAddr,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr,
                                       (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                       "%d",
                                       ulAtLogPort);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 At_SetDpdtTestFlagPara(VOS_UINT8 ucIndex)
{
    AT_MTA_SET_DPDTTEST_FLAG_REQ_STRU   stAtCmd;
    VOS_UINT32                          ulRst;

    /* 参数检查 */
    if (2 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT发送至MTA的消息结构赋值 */
    PS_MEM_SET(&stAtCmd, 0, sizeof(AT_MTA_SET_DPDTTEST_FLAG_REQ_STRU));
    stAtCmd.enRatMode = (AT_MTA_CMD_RATMODE_ENUM_UINT8)gastAtParaList[0].ulParaValue;
    stAtCmd.ucFlag    = (VOS_UINT8)gastAtParaList[1].ulParaValue;

    /* 发送消息给C核处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_SET_DPDTTEST_FLAG_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_SET_DPDTTEST_FLAG_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DPDTTEST_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 At_SetDpdtPara(VOS_UINT8 ucIndex)
{
    AT_MTA_SET_DPDT_VALUE_REQ_STRU      stAtCmd;
    VOS_UINT32                          ulRst;

    /* 参数检查 */
    if (2 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT发送至MTA的消息结构赋值 */
    PS_MEM_SET(&stAtCmd, 0, sizeof(AT_MTA_SET_DPDT_VALUE_REQ_STRU));
    stAtCmd.enRatMode   = (AT_MTA_CMD_RATMODE_ENUM_UINT8)gastAtParaList[0].ulParaValue;
    stAtCmd.ulDpdtValue = gastAtParaList[1].ulParaValue;

    /* 发送消息给C核处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_SET_DPDT_VALUE_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_SET_DPDT_VALUE_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DPDT_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}
VOS_UINT32 At_SetQryDpdtPara(VOS_UINT8 ucIndex)
{
    AT_MTA_QRY_DPDT_VALUE_REQ_STRU      stAtCmd;
    VOS_UINT32                          ulRst;

    /* 参数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT发送至MTA的消息结构赋值 */
    PS_MEM_SET(&stAtCmd, 0, sizeof(AT_MTA_QRY_DPDT_VALUE_REQ_STRU));
    stAtCmd.enRatMode = (AT_MTA_CMD_RATMODE_ENUM_UINT8)gastAtParaList[0].ulParaValue;

    /* 发送消息给C核处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_QRY_DPDT_VALUE_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_QRY_DPDT_VALUE_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DPDTQRY_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 AT_RcvMtaSetDpdtTestFlagCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                    *pstMtaMsg         = VOS_NULL_PTR;
    MTA_AT_SET_DPDTTEST_FLAG_CNF_STRU  *pstSetDpdtFlagCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex   = 0;
    pstMtaMsg = (AT_MTA_MSG_STRU *)pMsg;
    pstSetDpdtFlagCnf = (MTA_AT_SET_DPDTTEST_FLAG_CNF_STRU *)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetDpdtTestFlagCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetDpdtTestFlagCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_DPDTTEST_SET */
    if (AT_CMD_DPDTTEST_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetDpdtTestFlagCnf: WARNING:Not AT_CMD_DPDTTEST_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if (MTA_AT_RESULT_NO_ERROR == pstSetDpdtFlagCnf->enResult)
    {
        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvMtaSetDpdtValueCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                    *pstMtaMsg          = VOS_NULL_PTR;
    MTA_AT_SET_DPDT_VALUE_CNF_STRU     *pstSetDpdtValueCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    pstMtaMsg          = (AT_MTA_MSG_STRU *)pMsg;
    pstSetDpdtValueCnf = (MTA_AT_SET_DPDT_VALUE_CNF_STRU *)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetDpdtValueCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetDpdtValueCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_DPDT_SET */
    if (AT_CMD_DPDT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetDpdtValueCnf: WARNING:Not AT_CMD_DPDT_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if (MTA_AT_RESULT_NO_ERROR == pstSetDpdtValueCnf->enResult)
    {
        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvMtaQryDpdtValueCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                    *pstMtaMsg          = VOS_NULL_PTR;
    MTA_AT_QRY_DPDT_VALUE_CNF_STRU     *pstQryDpdtValueCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    pstMtaMsg          = (AT_MTA_MSG_STRU *)pMsg;
    pstQryDpdtValueCnf = (MTA_AT_QRY_DPDT_VALUE_CNF_STRU *)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryDpdtValueCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryDpdtValueCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_DPDTQRY_SET */
    if (AT_CMD_DPDTQRY_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaQryDpdtValueCnf: WARNING:Not AT_CMD_DPDTQRY_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if (MTA_AT_RESULT_NO_ERROR == pstQryDpdtValueCnf->enResult)
    {
        ulResult = AT_OK;
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            "^DPDTQRY: %d",
                                            pstQryDpdtValueCnf->ulDpdtValue);
    }
    else
    {
        ulResult = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
VOS_UINT32 At_SetJamDetectPara(VOS_UINT8 ucIndex)
{
    AT_MTA_SET_JAM_DETECT_REQ_STRU      stAtCmd;
    NV_NAS_JAM_DETECT_CFG_STRU          stNvJamCfg;
    VOS_UINT32                          ulRst;

    /* 参数检查 */
    if ((gucAtParaIndex < 1) || (gucAtParaIndex > 4))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT发送至MTA的消息结构赋值 */
    PS_MEM_SET(&stAtCmd, 0, sizeof(AT_MTA_SET_JAM_DETECT_REQ_STRU));

    stAtCmd.ucFlag         = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stAtCmd.ucMethod       = AT_JAM_DETECT_DEFAULT_METHOD;

    if (0 == gastAtParaList[2].usParaLen)
    {
        stAtCmd.ucThreshold    = AT_JAM_DETECT_DEFAULT_THRESHOLD;
    }
    else
    {
        stAtCmd.ucThreshold    = (VOS_UINT8)gastAtParaList[2].ulParaValue;
    }

    if (0 == gastAtParaList[3].usParaLen)
    {
        stAtCmd.ucFreqNum      = AT_JAM_DETECT_DEFAULT_FREQ_NUM;
    }
    else
    {
        stAtCmd.ucFreqNum      = (VOS_UINT8)gastAtParaList[3].ulParaValue;
    }

    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_JAM_DETECT_CFG, &stNvJamCfg, sizeof(stNvJamCfg)))
    {
        AT_WARN_LOG("At_SetJamDetectPara():en_NV_Item_JAM_DETECT_CFG NV Read Fail!");
        return AT_ERROR;
    }

    /* 配置参数保存在NV消息结构中 */
    stNvJamCfg.ucMode      = stAtCmd.ucFlag;
    stNvJamCfg.ucMethod    = stAtCmd.ucMethod;
    stNvJamCfg.ucThreshold = stAtCmd.ucThreshold;
    stNvJamCfg.ucFreqNum   = stAtCmd.ucFreqNum;

    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_JAM_DETECT_CFG, &stNvJamCfg, sizeof(stNvJamCfg)))
    {
        AT_WARN_LOG("At_SetJamDetectPara():en_NV_Item_JAM_DETECT_CFG NV Write Fail!");
        return AT_ERROR;
    }

    /* 发送消息给C核处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_SET_JAM_DETECT_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_SET_JAM_DETECT_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_JDETEX_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 At_QryJamDetectPara(VOS_UINT8 ucIndex)
{
    NV_NAS_JAM_DETECT_CFG_STRU          stNvJamDetect;

    PS_MEM_SET(&stNvJamDetect, 0x0, sizeof(stNvJamDetect));

    /* 通过读取NV来获取Jam Detect当前配置值 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0,
                           en_NV_Item_JAM_DETECT_CFG,
                           &stNvJamDetect,
                           sizeof(NV_NAS_JAM_DETECT_CFG_STRU)))
    {
        AT_WARN_LOG("At_QryJamDetectPara: NV_ReadEx en_NV_Item_JAM_DETECT_CFG fail!");
        return AT_ERROR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: %d,%d,%d,%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    stNvJamDetect.ucMode,
                                                    stNvJamDetect.ucMethod,
                                                    stNvJamDetect.ucThreshold,
                                                    stNvJamDetect.ucFreqNum);

    return AT_OK;
}
VOS_UINT32 AT_RcvMtaSetJamDetectCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg;
    MTA_AT_SET_JAM_DETECT_CNF_STRU         *pstSetCnf;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_SET_JAM_DETECT_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult     = AT_OK;
    ucIndex      = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetJamDetectCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetJamDetectCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_JDETEX_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetJamDetectCnf : Current Option is not AT_CMD_JDETEX_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR != pstSetCnf->enResult)
    {
        ulResult = AT_ERROR;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaJamDetectInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_JAM_DETECT_IND_STRU         *pstJamDetectInd;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstJamDetectInd     = (MTA_AT_JAM_DETECT_IND_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaJamDetectInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    /* 上报干扰检测结果 */
    if (MTA_AT_JAM_RESULT_JAM_DISCOVERED == pstJamDetectInd->enJamResult)
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^JDET: JAMMED%s",
                                                        gaucAtCrLf,
                                                        gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);
    }

    if (MTA_AT_JAM_RESULT_JAM_DISAPPEARED == pstJamDetectInd->enJamResult)
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^JDET: DETECTING%s",
                                                        gaucAtCrLf,
                                                        gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);
    }
    return VOS_OK;
}
VOS_UINT32 AT_SetRatFreqLock(VOS_UINT8 ucIndex)
{
    AT_MTA_SET_FREQ_LOCK_REQ_STRU       stAtCmd;
    VOS_UINT32                          ulRst;

    /* 参数个数检查 */
    if ((gucAtParaIndex < 1) || (gucAtParaIndex > 4))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0, sizeof(AT_MTA_SET_FREQ_LOCK_REQ_STRU));
    stAtCmd.ucEnableFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果锁频功能关闭，直接写Nv, 返回AT_OK */
    if (VOS_FALSE == stAtCmd.ucEnableFlg)
    {
        if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_FREQ_LOCK_CFG, &stAtCmd, sizeof(stAtCmd)))
        {
            AT_ERR_LOG("AT_SetRatFreqLock(): en_NV_Item_FREQ_LOCK_CFG NV Write Fail!");
            return AT_ERROR;
        }

        return AT_OK;
    }

    stAtCmd.usLockedFreq = (VOS_UINT16)gastAtParaList[1].ulParaValue;

    /* 解析命令中的接入模式 */
    if (0 == gastAtParaList[2].usParaLen)
    {
        stAtCmd.enRatMode = AT_MTA_FREQLOCK_RATMODE_WCDMA;
    }
    else
    {
        stAtCmd.enRatMode = (AT_MTA_FREQLOCK_RATMODE_ENUM_UINT8)gastAtParaList[2].ulParaValue;
    }

    /* 解析命令中的BAND信息 */
    if (0 == gastAtParaList[3].usParaLen)
    {
        if (AT_MTA_FREQLOCK_RATMODE_GSM == stAtCmd.enRatMode)
        {
            return AT_ERROR;
        }
        stAtCmd.enBand = AT_MTA_GSM_BAND_BUTT;
    }
    else
    {
        stAtCmd.enBand = (AT_MTA_GSM_BAND_ENUM_UINT16)gastAtParaList[3].ulParaValue;
    }

    /* 发送消息给C核处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_SET_FREQ_LOCK_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_SET_FREQ_LOCK_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_RATFREQLOCK_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 AT_RcvMtaSetRatFreqLockCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_SET_FREQ_LOCK_CNF_STRU      *pstSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_SET_FREQ_LOCK_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetRatFreqLockCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetRatFreqLockCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_RATFREQLOCK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetRatFreqLockCnf : Current Option is not AT_CMD_RATFREQLOCK_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}
VOS_UINT32 AT_QryRatFreqLock(VOS_UINT8 ucIndex)
{
    TAF_NVIM_FREQ_LOCK_CFG_STRU         stNvFreqLockCfg;

    PS_MEM_SET(&stNvFreqLockCfg, 0x0, sizeof(stNvFreqLockCfg));

    /* 通过读取NV来获取Freq Lock当前配置值 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0,
                           en_NV_Item_FREQ_LOCK_CFG,
                           &stNvFreqLockCfg,
                           sizeof(TAF_NVIM_FREQ_LOCK_CFG_STRU)))
    {
        AT_WARN_LOG("At_QryRatFreqLock: NV_ReadEx en_NV_Item_FREQ_LOCK_CFG fail!");
        return AT_ERROR;
    }

    /* 锁频功能关闭情况下只上报开关值:0 */
    if (VOS_FALSE == stNvFreqLockCfg.ucEnableFlg)
    {
        /* 查询结果上报 */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        stNvFreqLockCfg.ucEnableFlg);

        return AT_OK;
    }

    /* 锁频功能设置在G模上，查询需要上报BAND信息 */
    if (TAF_NVIM_RAT_MODE_GSM == stNvFreqLockCfg.enRatMode)
    {
        /* 查询结果上报 */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d,%d,\"0%d\",\"0%d\"",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        stNvFreqLockCfg.ucEnableFlg,
                                                        stNvFreqLockCfg.usLockedFreq,
                                                        stNvFreqLockCfg.enRatMode,
                                                        stNvFreqLockCfg.enBand);
    }
    else
    {
        /* 查询结果上报 */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d,%d,\"0%d\"",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        stNvFreqLockCfg.ucEnableFlg,
                                                        stNvFreqLockCfg.usLockedFreq,
                                                        stNvFreqLockCfg.enRatMode);
    }

    return AT_OK;
}


VOS_UINT32 AT_SetGFreqLock(VOS_UINT8 ucIndex)
{
    AT_MTA_SET_GSM_FREQLOCK_REQ_STRU    stGFreqLockInfo;
    VOS_UINT32                          ulRst;

    /* 参数个数检查 */
    if ((gucAtParaIndex != 1) && (gucAtParaIndex != 3))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 初始化 */
    PS_MEM_SET(&stGFreqLockInfo, 0, sizeof(AT_MTA_SET_GSM_FREQLOCK_REQ_STRU));

    /* 参数有效性检查 */
    /* 第一个参数必须带 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stGFreqLockInfo.enableFlag = (PS_BOOL_ENUM_UINT8)gastAtParaList[0].ulParaValue;

    /* 若启动锁频，则必须带第二个参数和第三个参数 */
    if (PS_TRUE == stGFreqLockInfo.enableFlag)
    {
        if ( (0 == gastAtParaList[1].usParaLen)
          || (0 == gastAtParaList[2].usParaLen) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        else
        {
            stGFreqLockInfo.usFreq = (VOS_UINT16)gastAtParaList[1].ulParaValue;
            stGFreqLockInfo.enBand = (AT_MTA_GSM_BAND_ENUM_UINT16)gastAtParaList[2].ulParaValue;
        }
    }

    /* 发送消息给C核处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_SET_GSM_FREQLOCK_REQ,
                                   &stGFreqLockInfo,
                                   sizeof(AT_MTA_SET_GSM_FREQLOCK_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_GSM_FREQLOCK_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_RcvMtaSetGFreqLockCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_SET_GSM_FREQLOCK_CNF_STRU   *pstSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_SET_GSM_FREQLOCK_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetGFreqLockCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetGFreqLockCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_GSM_FREQLOCK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetGFreqLockCnf : Current Option is not AT_CMD_GSM_FREQLOCK_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

