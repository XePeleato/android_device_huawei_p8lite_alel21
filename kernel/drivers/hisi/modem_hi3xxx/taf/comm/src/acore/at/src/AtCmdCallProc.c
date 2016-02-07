/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AtCmdCallProc.c
  版 本 号   : 初稿
  作    者   : l60609
  生成日期   : 2013年3月13日
  最近修改   :
  功能描述   : 呼叫命令处理文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年3月13日
    作    者   : l60609
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "AtCmdCallProc.h"

#include "AtSndMsg.h"
#include "ATCmdProc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -save -e960 */
#define    THIS_FILE_ID                 PS_FILE_ID_AT_CMD_CALL_PROC_C
/*lint -restore */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if (FEATURE_ON == FEATURE_ECALL)

VOS_UINT32 AT_SetCecallPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    MN_CALL_ORIG_PARAM_STRU             stCecallPara;

    /* 参数必须为1。
       参数个数为1时，参数长度不可能为0，
       即"AT+CECALL="情况下，gucAtParaIndex为0 */
    if (1 != gucAtParaIndex)
    {
        return AT_ERROR;
    }

    PS_MEM_SET(&stCecallPara, 0x0, sizeof(stCecallPara));

    switch (gastAtParaList[0].ulParaValue)
    {
        case 0:
            stCecallPara.enCallType = MN_CALL_TYPE_TEST;
            break;
        case 1:
            stCecallPara.enCallType = MN_CALL_TYPE_RECFGURATION;
            break;
        case 2:
            stCecallPara.enCallType = MN_CALL_TYPE_MIEC;
            break;
        case 3:
            stCecallPara.enCallType = MN_CALL_TYPE_AIEC;
            break;
        default:
            return AT_ERROR;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   MN_CALL_APP_ORIG_REQ,
                                   &stCecallPara,
                                   sizeof(stCecallPara),
                                   I0_WUEPS_PID_TAF);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CECALL_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_SetEclstartPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    MN_CALL_ORIG_PARAM_STRU             stCecallPara;
    MN_CALL_CALLED_NUM_STRU             stDialNumber;

    /*  判断1: 必选参数的长度不能为0 */
    if ((0 == gastAtParaList[0].usParaLen)
     || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_ERROR;
    }

    /* 判断2: 参数个数不对, 判断1已保证参数个数>=2 */
    if (gucAtParaIndex > 3)
    {
        return AT_ERROR;
    }

    /* 判断3:  AT^ECLSTART=1,1, 这种情况是错的 */
    if ((3 == gucAtParaIndex)
     && (0 == gastAtParaList[2].usParaLen))
    {
        return AT_ERROR;
    }

    /* 初始化 */
    PS_MEM_SET(&stDialNumber, 0x0, sizeof(stDialNumber));

    /* 带拨号号码 */
    if (gastAtParaList[2].usParaLen > 0)
    {
        /* 号码的最大长度不能超过18 */
        if (gastAtParaList[2].usParaLen > ECALL_MAX_DIAL_NUM_LEN)
        {
            return AT_ERROR;
        }

        /* 检查并转换电话号码 */
        if (VOS_OK !=  AT_FillCalledNumPara(gastAtParaList[2].aucPara,
                                            gastAtParaList[2].usParaLen,
                                            &stDialNumber))
        {
            AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
            return AT_ERROR;
        }
    }

    /* 初始化 */
    PS_MEM_SET(&stCecallPara, 0x0, sizeof(stCecallPara));

    /* 发紧急呼叫时，<activation_type>参数有效，非紧急呼叫时,<activation_type>参数没有作用 --产品线:t00192572 */
    /* ^ECLSTART=x,0  发起测试call */
    if (0 == gastAtParaList[1].ulParaValue)
    {
        stCecallPara.enCallType = MN_CALL_TYPE_TEST;

        /* 只有测试呼叫才下发电话号码，紧急呼叫不下发电话号码 */
        PS_MEM_CPY(&stCecallPara.stDialNumber, &stDialNumber, sizeof(stDialNumber));
    }
    else
    {
        /* ^ECLSTART=0,1  用户发起紧急call */
        if (0 == gastAtParaList[0].ulParaValue)
        {
            stCecallPara.enCallType = MN_CALL_TYPE_MIEC;
        }
        /* ^ECLSTART=1,1  自动发起紧急call */
        else
        {
            stCecallPara.enCallType = MN_CALL_TYPE_AIEC;
        }
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   MN_CALL_APP_ORIG_REQ,
                                   &stCecallPara,
                                   sizeof(stCecallPara),
                                   I0_WUEPS_PID_TAF);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ECLSTART_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 AT_SetEclstopPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    MN_CALL_SUPS_PARAM_STRU             stEclstopPara;

    /* 不带参数的设置命令 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 初始化 */
    PS_MEM_SET(&stEclstopPara, 0x0, sizeof(stEclstopPara));

    stEclstopPara.enCallSupsCmd = MN_CALL_SUPS_CMD_REL_ECALL;

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   MN_CALL_APP_SUPS_CMD_REQ,
                                   &stEclstopPara,
                                   sizeof(stEclstopPara),
                                   I0_WUEPS_PID_TAF);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ECLSTOP_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_SetEclcfgPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    APP_VC_MSG_SET_ECALL_CFG_REQ_STRU   stEclcfgSetPara;

    /* 判断一: 必选参数的长度不能为0 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    /* 判断二: 参数个数不对, 判断一已经保证参数个数>=1 */
    if (gucAtParaIndex > 2)
    {
        return AT_ERROR;
    }

    /* 判断3:  AT^ECLCFG=0, 这种情况是错的 */
    if ((2 == gucAtParaIndex)
     && (0 == gastAtParaList[1].usParaLen))
    {
        return AT_ERROR;
    }

    stEclcfgSetPara.enMode = (APP_VC_ECALL_MSD_MODE_ENUM_UINT16)gastAtParaList[0].ulParaValue;

    if (0 == gastAtParaList[1].usParaLen)
    {
        stEclcfgSetPara.enVocConfig = APP_VC_ECALL_VOC_CONFIG_NOT_ABANDON;
    }
    else
    {
        stEclcfgSetPara.enVocConfig = (APP_VC_ECALL_VOC_CONFIG_ENUM_UINT16)gastAtParaList[1].ulParaValue;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   APP_VC_MSG_SET_ECALL_CFG_REQ,
                                   &stEclcfgSetPara,
                                   sizeof(stEclcfgSetPara),
                                   I0_WUEPS_PID_VC);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ECLCFG_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 AT_SetEclmsdPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    APP_VC_MSG_SET_MSD_REQ_STRU         stEclmsdPara;

    /* 参数必须为1。
       参数个数为1时，参数长度不可能为0，
       即"AT+CELMSD="情况下，gucAtParaIndex为0 */
    if (1 != gucAtParaIndex)
    {
        return AT_ERROR;
    }

    /* 参数长度不对
       十六进制文本字符串，采用十六进制数据编码方式，字符串长度为280个字节，表示的是MSD协议要求的140个字节原始数据
    */
    if (((APP_VC_MSD_DATA_LEN * 2) != gastAtParaList[0].usParaLen))
    {
        return AT_ERROR;
    }

    if (AT_FAILURE == At_AsciiString2HexSimple(stEclmsdPara.aucMsdData,
                                               gastAtParaList[0].aucPara,
                                               APP_VC_MSD_DATA_LEN * 2))
    {
        return AT_ERROR;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   APP_VC_MSG_SET_MSD_REQ,
                                   &stEclmsdPara,
                                   sizeof(stEclmsdPara),
                                   I0_WUEPS_PID_VC);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ECLMSD_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_QryCecallPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   MN_CALL_QRY_ECALL_INFO_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_TAF);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CECALL_QRY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_QryEclcfgPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   APP_VC_MSG_QRY_ECALL_CFG_REQ,
                                   VOS_NULL,
                                   0,
                                   I0_WUEPS_PID_VC);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ECLCFG_QRY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_QryEclmsdPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   APP_VC_MSG_QRY_MSD_REQ,
                                   VOS_NULL,
                                   0,
                                   I0_WUEPS_PID_VC);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ECLMSD_QRY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_TestEclstartPara(VOS_UINT8 ucIndex)
{

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: (0,1),(0,1)",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    return AT_OK;
}


VOS_UINT32 AT_TestEclmsdPara(VOS_UINT8 ucIndex)
{
    /* ^eclmsd不支持测试命令，
        该函数可以在eclmsd测试命令时返回ERROR，而不是"(MSD)" */
    return AT_ERROR;
}

#endif


VOS_UINT32 At_RcvVcMsgDtmfDecoderIndProc(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    APP_VC_DTMF_DECODER_IND_STRU       *pstDtmfInd;
    VOS_UINT8                           ucIndex;
    VOS_CHAR                            aucOutput[2];

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgDtmfDecoderIndProc:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 初始化 */
    pstDtmfInd = (APP_VC_DTMF_DECODER_IND_STRU *)pstData->aucContent;
    aucOutput[0] = pstDtmfInd->ucDtmfCode;
    aucOutput[1] = '\0';

    /* 输出查询结果 */
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^DDTMF: %s%s",
                                                    gaucAtCrLf,
                                                    aucOutput,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}

/* Added by l60609 for CDMA 1X Iteration 2, 2014-9-5, begin */
/*****************************************************************************
 函 数 名  : AT_SetHflashPara
 功能描述  : AT^HFLASH=<phone number> 的设置函数

             <CR><LF>OK<CR><LF>
             有MS相关错误时：
             <CR><LF>+CME ERROR: <err><CR><LF>

             本命令实现FLASH功能， 首先对命令参数进行合法性检查，包含：号码及长度。约束：合法的字符包括ASCII码数字，号码长度范围：0~32。
             然后发送FLASH命令

             当没有处于通话状态时, 需上报一个错误。
             当有呼叫等待的时候，发送不带电话号码的FLASH命令接听第三方呼叫。
             当通话时，发送带电话号码的FLASH命令进行对第三方的呼叫。
             其他情况报错。
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年9月5日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_SetHflashPara(VOS_UINT8 ucIndex)
{
    /* TO DO: 下个迭代实现 */

    /* 发送MN_CALL_APP_SEND_FLASH_REQ消息 */
    return AT_OK;
}

/*****************************************************************************
 函 数 名  : AT_RcvTafCallSndFlashCnf
 功能描述  : 处理TAF_CALL_EVT_SEND_FLASH_CNF事件
 输入参数  : MN_AT_IND_EVT_STRU                 *pEvtInfo
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年9月10日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_RcvTafCallSndFlashCnf(
    MN_AT_IND_EVT_STRU                 *pEvtInfo
)
{
    /* TO DO: 下个迭代实现 */

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : AT_RcvTafCallSndFlashRslt
 功能描述  : 处理TAF_CALL_EVT_SEND_FLASH_RSLT事件
 输入参数  : MN_AT_IND_EVT_STRU                 *pEvtInfo
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年9月10日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_RcvTafCallSndFlashRslt(
    MN_AT_IND_EVT_STRU                 *pEvtInfo
)
{
    /* TO DO: 下个迭代实现 */

    return VOS_OK;
}


/* Added by l60609 for CDMA 1X Iteration 2, 2014-9-5, end */


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

