/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VcApi.c
  版 本 号   : 初稿
  作    者   : 胡文 44270
  生成日期   : 2009年7月05日
  最近修改   : 2007年7月05日
  功能描述   : 实现VC模块的功能
  函数列表   :
  修改历史   :
  1.日    期   : 2009年7月05日
    作    者   : 胡文 44270
    修改内容   : 创建文件
******************************************************************************/

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "AppVcApi.h"
#include "vos.h"
#include "PsCommonDef.h"
#include "ATCmdProc.h"

#ifdef  __cplusplus
  #if  __cplusplus
      extern "C"{
  #endif
#endif



/*lint -e767 -e960*/
#define    THIS_FILE_ID        PS_FILE_ID_VC_API_C
/*lint +e767 +e960*/

/*****************************************************************************
   2 函数实现
*****************************************************************************/


VOS_UINT32  APP_VC_SetVoiceVolume(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT8                           ucVoiceVolume
)
{
    /*构造消息发送消息给VC模块*/
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;

    /* 申请消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                  sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_SetVoiceVolume: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    pstMsg->clientId                    = ClientId;
    pstMsg->opId                        = OpId;
    pstMsg->enMsgName                   = APP_VC_MSG_REQ_SET_VOLUME;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverPid               = AT_GetDestPid(ClientId, I0_WUEPS_PID_VC);
    pstMsg->aucContent[0]               = ucVoiceVolume;
    pstMsg->aucContent[1]               = 0;
    pstMsg->aucContent[2]               = 0;
    pstMsg->aucContent[3]               = 0;

    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_SetVoiceVolume: SEND MSG FAIL.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32  APP_VC_SetVoiceMode(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT8                           ucVoiceMode,
    VOS_UINT32                          ulTaskPid
)
{
    /*构造消息发送消息给VC模块*/
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;

    /* 申请消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                  sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_SetVoiceMode: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    pstMsg->clientId                    = ClientId;
    pstMsg->opId                        = OpId;
    pstMsg->enMsgName                   = APP_VC_MSG_REQ_SET_MODE;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverPid               = ulTaskPid;
    pstMsg->aucContent[0]               = ucVoiceMode;
    pstMsg->aucContent[1]               = 0;
    pstMsg->aucContent[2]               = 0;
    pstMsg->aucContent[3]               = 0;

    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_SetVoiceMode: SEND MSG FAIL.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 APP_VC_SetVoicePort(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    APP_VC_VOICE_PORT_ENUM_U8           ucVoicePort
)
{
    /*构造消息发送消息给VC模块*/
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;


    /* 申请消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                  sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_SetVoicePort: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    pstMsg->clientId                    = ClientId;
    pstMsg->opId                        = OpId;
    pstMsg->enMsgName                   = APP_VC_MSG_REQ_SET_PORT;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverPid               = AT_GetDestPid(ClientId, I0_WUEPS_PID_VC);
    pstMsg->aucContent[0]               = ucVoicePort;
    pstMsg->aucContent[1]               = 0;
    pstMsg->aucContent[2]               = 0;
    pstMsg->aucContent[3]               = 0;

    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_GetVoiceMode: SEND MSG FAIL.");
        return VOS_ERR;
    }
    return VOS_OK;

}


VOS_UINT32 APP_VC_GetVoiceMode(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    /*构造消息发送消息给VC模块*/
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;

    /* 申请消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                  sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_GetVoiceMode: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    pstMsg->clientId                    = ClientId;
    pstMsg->opId                        = OpId;
    pstMsg->enMsgName                   = APP_VC_MSG_REQ_QRY_MODE;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverPid               = AT_GetDestPid(ClientId, I0_WUEPS_PID_VC);
    pstMsg->aucContent[0]               = 0;
    pstMsg->aucContent[1]               = 0;
    pstMsg->aucContent[2]               = 0;
    pstMsg->aucContent[3]               = 0;

    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_GetVoiceMode: SEND MSG FAIL.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 APP_VC_GetVoicePort(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    /*构造消息发送消息给VC模块*/
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;

    /* 申请消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                  sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_GetVoicePort: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    /* 组装消息 */
    pstMsg->clientId                    = ClientId;
    pstMsg->opId                        = OpId;
    pstMsg->enMsgName                   = APP_VC_MSG_REQ_QRY_PORT;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverPid               = AT_GetDestPid(ClientId, I0_WUEPS_PID_VC);
    pstMsg->aucContent[0]               = 0;
    pstMsg->aucContent[1]               = 0;
    pstMsg->aucContent[2]               = 0;
    pstMsg->aucContent[3]               = 0;

    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_GetVoicePort: SEND MSG FAIL.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VC_PHY_DEVICE_MODE_ENUM_U16  APP_VC_AppVcVoiceMode2VcPhyVoiceMode(
    APP_VC_VOICE_MODE_ENUM_U16          usVoiceMode
)
{
    switch(usVoiceMode)
    {
        case APP_VC_VOICE_MODE_PCVOICE:
            return VC_PHY_DEVICE_MODE_PCVOICE;

        case APP_VC_VOICE_MODE_EARPHONE:
            return VC_PHY_DEVICE_MODE_EARPHONE;

        case APP_VC_VOICE_MODE_HANDSET:
            return VC_PHY_DEVICE_MODE_HANDSET;

        case APP_VC_VOICE_MODE_HANDS_FREE:
            return VC_PHY_DEVICE_MODE_HANDS_FREE;

        default:
            AT_WARN_LOG1("APP_VC_AppVcVoiceMode2VcPhyVoiceMode: wrong usVoiceMode ", usVoiceMode);
            return VC_PHY_DEVICE_MODE_BUTT;
    }

}


APP_VC_VOICE_MODE_ENUM_U16  APP_VC_VcPhyVoiceMode2AppVcVoiceMode(
    VC_PHY_DEVICE_MODE_ENUM_U16         usVoiceMode
)
{
    switch(usVoiceMode)
    {
        case VC_PHY_DEVICE_MODE_PCVOICE:
            return APP_VC_VOICE_MODE_PCVOICE;

        case VC_PHY_DEVICE_MODE_EARPHONE:
            return APP_VC_VOICE_MODE_EARPHONE;

        case VC_PHY_DEVICE_MODE_HANDSET:
            return APP_VC_VOICE_MODE_HANDSET;

        case VC_PHY_DEVICE_MODE_HANDS_FREE:
            return APP_VC_VOICE_MODE_HANDS_FREE;

        default:
            AT_WARN_LOG1("APP_VC_VcPhyVoiceMode2AppVcVoiceMode: wrong usVoiceMode ", usVoiceMode);
            return APP_VC_VOICE_MODE_BUTT;
    }

}


VOS_UINT32  APP_VC_GetVoiceVolume(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    /*构造消息发送消息给VC模块*/
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;

    /* 申请消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                  sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_GetVoiceVolume: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    pstMsg->clientId                    = ClientId;
    pstMsg->opId                        = OpId;
    pstMsg->enMsgName                   = APP_VC_MSG_REQ_QRY_VOLUME;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverPid               = AT_GetDestPid(ClientId, I0_WUEPS_PID_VC);
    pstMsg->aucContent[0]               = 0;
    pstMsg->aucContent[1]               = 0;
    pstMsg->aucContent[2]               = 0;
    pstMsg->aucContent[3]               = 0;

    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_GetVoiceVolume: SEND MSG FAIL.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 APP_VC_SetMuteStatus(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    APP_VC_MUTE_STATUS_ENUM_UINT8       enMuteStatus
)
{
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;

    /* 构造消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        WUEPS_PID_AT,
                                        sizeof(APP_VC_REQ_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_SetMuteStatus: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR *)pstMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头 */
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = AT_GetDestPid(usClientId, I0_WUEPS_PID_VC);
    pstMsg->enMsgName       = APP_VC_MSG_SET_MUTE_STATUS_REQ;

    /* 填写消息内容 */
    pstMsg->clientId        = usClientId;
    pstMsg->opId            = ucOpId;
    pstMsg->aucContent[0]   = enMuteStatus;

    /* 发送消息 */
    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_SetMuteStatus: SEND MSG FAIL.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 APP_VC_GetMuteStatus(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;

    /* 构造消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        WUEPS_PID_AT,
                                        sizeof(APP_VC_REQ_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_SetMuteStatus: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR *)pstMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头 */
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = AT_GetDestPid(usClientId, I0_WUEPS_PID_VC);
    pstMsg->enMsgName       = APP_VC_MSG_GET_MUTE_STATUS_REQ;

    /* 填写消息内容 */
    pstMsg->clientId        = usClientId;
    pstMsg->opId            = ucOpId;

    /* 发送消息 */
    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_SetMuteStatus: SEND MSG FAIL.");
        return VOS_ERR;
    }

    return VOS_OK;
}

/* Added by L47619 for VOICE_LOOP, 2013/07/05, begin */
/*****************************************************************************
 函 数 名  : APP_VC_SetModemLoop
 功能描述  : 设置进入或退出语音换回
 输入参数  : usClientId   - 客户端ID
             ucOpId       - 操作码ID
             ucModemLoop  - 进入或退出语音环回状态
 输出参数  : 无
 返 回 值  : VOS_OK       - 成功
             VOS_ERR      - 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月8日
    作    者   : L47619
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 APP_VC_SetModemLoop(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucModemLoop
)
{
    VOS_UINT32                          ulRslt;
    APP_VC_REQ_MSG_STRU                *pstMsg;

    /* 构造消息 */
    pstMsg = (APP_VC_REQ_MSG_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        WUEPS_PID_AT,
                                        sizeof(APP_VC_REQ_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("APP_VC_SetModemLoop: ALLOC MSG FAIL.");
        return VOS_ERR;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR *)pstMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(APP_VC_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头 */
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = AT_GetDestPid(usClientId, I0_WUEPS_PID_VC);
    pstMsg->enMsgName       = APP_VC_MSG_SET_MODEMLOOP_REQ;

    /* 填写消息内容 */
    pstMsg->clientId        = usClientId;
    pstMsg->opId            = ucOpId;
    pstMsg->aucContent[0]   = ucModemLoop;

    /* 发送消息 */
    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMsg);
    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("APP_VC_SetModemLoop: SEND MSG FAIL.");
        return VOS_ERR;
    }

    return VOS_OK;
}
/* Added by L47619 for VOICE_LOOP, 2013/07/05, end */

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

