/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : omerrorlog.c
  版 本 号      : 初稿
  作    者      : d00212987
  生成日期      : 2013年8月27日
  最近修改      :
  功能描述      : 商用Errlog&工程模式上报处理
  函数列表      :
  修改历史      :
  1.日    期    : 2013年8月27日
    作    者    : d00212987
    修改内容    : 创建文件

******************************************************************************/
/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "OmApp.h"
#include "omprivate.h"
#include "OmAppRl.h"
#include "omerrorlog.h"
#include "AtAppVcomInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/*lint -e767 修改人:d00212987;检视人:徐铖 51137;原因:Log打印 */
#define    THIS_FILE_ID        PS_FILE_ID_OM_ERRORLOG_C
/*lint +e767 修改人:d00212987;检视人:徐铖 51137; */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
OM_APP_MSG_RECORD_STRU                  g_stOmAppMsgRecord; /* OM收到AP需要在全局变量中记录内容 */

#if(FEATURE_ON == FEATURE_PTM)

#if (VOS_LINUX == VOS_OS_VER)
struct semaphore                        g_stOmRxErrorLogBuffSem;
#endif

OM_VCOM_DEBUG_INFO                      g_stErrLogVcomDebugInfo   = {0};

OM_ERR_LOG_DEBUG_INFO                   g_stRcvUESendAP           = {0};

HTIMER                                  g_AcpuErrLogFullTmr       = VOS_NULL_PTR ;

/* Error Log 上报涉及PID */
VOS_UINT32    g_aulModem0ErrLogDetail[OM_MAX_MODULE_ID]= {
                         I0_WUEPS_PID_MMC,I0_UEPS_PID_MTA,I0_WUEPS_PID_CC, I0_WUEPS_PID_USIM,
                         0,               I0_UEPS_PID_GAS,0,               0,
                         I0_UEPS_PID_SN,  0,              WUEPS_PID_WRR,   0,
                         0,               0,              0,               DSP_PID_WPHY,
                         I0_DSP_PID_APM,  I0_DSP_PID_GPHY,0,               0,
                         PS_PID_ERRC,     TPS_PID_RRC,    0,               0,
                         PS_PID_MM,       PS_PID_ESM,     0,               0,
                         TPS_PID_MAC,     PS_PID_MAC_UL,  0,               0};
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
VOS_UINT32    g_aulModem1ErrLogDetail[OM_MAX_MODULE_ID]= {
                         I1_WUEPS_PID_MMC,I1_UEPS_PID_MTA,I1_WUEPS_PID_CC, I1_WUEPS_PID_USIM,
                         0,               I1_UEPS_PID_GAS,0,               0,
                         I1_UEPS_PID_SN,  0,              WUEPS_PID_WRR,   0,
                         0,               0,              0,               DSP_PID_WPHY,
                         I1_DSP_PID_APM,  I1_DSP_PID_GPHY,0,               0,
                         PS_PID_ERRC,     TPS_PID_RRC,    0,               0,
                         PS_PID_MM,       PS_PID_ESM,     0,               0,
                         TPS_PID_MAC,     PS_PID_MAC_UL,  0,               0};
#endif

/* 工程模式主动上报涉及PID */
VOS_UINT32    g_aulModem0FTMDetail[OM_MAX_MODULE_ID]={
                         I0_WUEPS_PID_MMC,I0_WUEPS_PID_MM,I0_WUEPS_PID_GMM,WUEPS_PID_OM_CALLBACK,
                         0,               0,               0,              0,
                         0,               0,               0,              0,
                         WUEPS_PID_WRR,   0,               0,              DSP_PID_WPHY,
                         I0_DSP_PID_APM,  I0_DSP_PID_GPHY, 0,              0,
                         PS_PID_ERRC,     TPS_PID_RRC,     0,              0,
                         PS_PID_MM,       PS_PID_ESM,      0,              0,
                         0,               0,               0,              0};

#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
VOS_UINT32    g_aulModem1FTMDetail[OM_MAX_MODULE_ID]={
                         I1_WUEPS_PID_MMC,I1_WUEPS_PID_MM,I1_WUEPS_PID_GMM,WUEPS_PID_OM_CALLBACK,
                         0,               0,              0,               0,
                         0,               0,              0,               0,
                         WUEPS_PID_WRR,   0,              0,               DSP_PID_WPHY,
                         I1_DSP_PID_APM,  I1_DSP_PID_GPHY,0,               0,
                         PS_PID_ERRC,     TPS_PID_RRC,    0,               0,
                         PS_PID_MM,       PS_PID_ESM,     0,               0,
                         0,               0,              0,               0};
#endif

/* 工程模式命令上报组件对应PID */
/* 数组中两个moduleID只是为了验证此功能增加，并没有实际用到 */
APP_OM_FTM_MSG_PID_STRU    g_astModem0FTMMsgModule[]={
                            {OM_ERR_LOG_MOUDLE_ID_IMS,  0},
                            {OM_ERR_LOG_MOUDLE_ID_IMSA, 0},
                           };

#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
/* 数组中两个moduleID只是为了验证此功能增加，并没有实际用到 */
APP_OM_FTM_MSG_PID_STRU    g_astModem1FTMMsgModule[]={
                            {OM_ERR_LOG_MOUDLE_ID_IMS,  0},
                            {OM_ERR_LOG_MOUDLE_ID_IMSA, 0},
                           };
#endif

/*****************************************************************************
  3 外部引用声明
*****************************************************************************/
extern VOS_SPINLOCK      g_stVosErrLogSendSpinLock;  /* 自旋锁，用来作g_ulErrLogReportSend的临界资源保护 */

/*****************************************************************************
  4 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : OM_AcpuRcvMsgFinish
 功能描述  : 各组件给Om上报消息完毕
 输入参数  : 无

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuRcvMsgFinish(VOS_VOID)
{
    OM_APP_REPORT_STATUS_STRU               stOmAppReportStatus;

    VOS_MemSet(&stOmAppReportStatus, 0, sizeof(stOmAppReportStatus));
    stOmAppReportStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ERR_REPORT_END;
    stOmAppReportStatus.stOmHeader.ulMsgSN   = g_stErrLogVcomDebugInfo.ulVCOMSendSn++;
    stOmAppReportStatus.stOmHeader.ulMsgLen  = (sizeof(OM_APP_REPORT_STATUS_STRU) - sizeof(OM_ALARM_MSG_HEAD_STRU));
    stOmAppReportStatus.usModemID            = g_stOmAppMsgRecord.usModemId;
    stOmAppReportStatus.ulReportStatus       = g_stOmAppMsgRecord.ulErrLogReportSend;

    OM_AcpuSendVComData((VOS_UINT8 *)&stOmAppReportStatus, sizeof(OM_APP_REPORT_STATUS_STRU));

    /* 防止消息再次下发不成功 */
    g_stOmAppMsgRecord.ulErrLogReportSend = OM_AP_NO_MSG_SEND;

    return ;
}

/*****************************************************************************
 函 数 名  : OM_AcpuSendAppResult
 功能描述  : OM给AP回复处理结果
 输入参数  : ulRest: 给APP发送的

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuSendAppResult(VOS_UINT32 ulRest, VOS_UINT16  usModemId)
{
    OM_APP_RESULT_CNF_STRU               stOmAppResultCnf;

    VOS_MemSet(&stOmAppResultCnf, 0, sizeof(stOmAppResultCnf));
    stOmAppResultCnf.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_RESULT;
    stOmAppResultCnf.stOmHeader.ulMsgSN   = g_stErrLogVcomDebugInfo.ulVCOMSendSn++;
    stOmAppResultCnf.stOmHeader.ulMsgLen  = (sizeof(OM_APP_RESULT_CNF_STRU) - sizeof(OM_ALARM_MSG_HEAD_STRU));
    stOmAppResultCnf.usModemID            = usModemId;
    stOmAppResultCnf.ulStatus             = ulRest;

    OM_AcpuSendVComData((VOS_UINT8 *)&stOmAppResultCnf, sizeof(OM_APP_RESULT_CNF_STRU));

    return ;
}

/*****************************************************************************
 函 数 名  : OM_AcpuSwitchOnOffErrLog
 功能描述  : AP下发 Error Log开关命令，写入NV项同时给各组件发送开关状态
 输入参数  : pstAppOmCtrlStatus : 收到数据

 输出参数  : 无
 返 回 值  : OK/ERR
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT OM_AcpuSwitchOnOffErrLog(APP_OM_CTRL_STATUS_STRU  *pstAppOmCtrlStatus)
{
    NV_ID_ERR_LOG_CTRL_INFO_STRU         stNvErrLogCtrlInfo;
    OM_ERROR_LOG_CTRL_IND_STRU          *pOmErrorLogCtrlInd;
    VOS_UINT32                          *pulErrorLogModule;
    VOS_UINT32                           i;

    /* 开关内容 */
    stNvErrLogCtrlInfo.ucAlmStatus = pstAppOmCtrlStatus->ucAlmStatus;
    stNvErrLogCtrlInfo.ucAlmLevel  = pstAppOmCtrlStatus->ucAlmLevel;

    /* modem ID 检查*/
    if (MODEM_ID_0 == pstAppOmCtrlStatus->usModemID)
    {
        pulErrorLogModule = g_aulModem0ErrLogDetail;
    }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == pstAppOmCtrlStatus->usModemID)
    {
        pulErrorLogModule = g_aulModem1ErrLogDetail;
    }
#endif
    else
    {
        OM_ERR_LOG("OM_AcpuSwitchOnOffErrLog: modem id error!\r\n ");
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrNum++;
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrLen += (pstAppOmCtrlStatus->stOmHeader.ulMsgLen + sizeof(OM_ALARM_MSG_HEAD_STRU));
        OM_AcpuSendAppResult(OM_APP_SEND_MODEM_ID_ERR, pstAppOmCtrlStatus->usModemID);
        return OM_APP_SEND_MODEM_ID_ERR;
    }

    if (VOS_OK !=NV_WriteEx(pstAppOmCtrlStatus->usModemID,
                            en_NV_Item_ErrLogCtrlInfo,
                            &stNvErrLogCtrlInfo,
                            sizeof(NV_ID_ERR_LOG_CTRL_INFO_STRU)))
    {
        /* 给Ap 回复消息 */
        OM_ERR_LOG("OM_AcpuSwitchOnOffErrLog: nv write fail!\r\n ");
        OM_AcpuSendAppResult(OM_APP_OMACPU_WRITE_NV_ERR, pstAppOmCtrlStatus->usModemID);
        return OM_APP_OMACPU_WRITE_NV_ERR;
    }

    for (i=0; i < OM_MAX_MODULE_ID; i++)
    {
        if (OM_PID_NULL != pulErrorLogModule[i])
        {
            pOmErrorLogCtrlInd = (OM_ERROR_LOG_CTRL_IND_STRU*)VOS_AllocMsg(ACPU_PID_OM,
                                  (sizeof(OM_ERROR_LOG_CTRL_IND_STRU) - VOS_MSG_HEAD_LENGTH));
            if (VOS_NULL_PTR == pOmErrorLogCtrlInd)
            {
                OM_AcpuSendAppResult(OM_APP_OMACPU_ALLOC_MSG_ERR, pstAppOmCtrlStatus->usModemID);
                return OM_APP_OMACPU_ALLOC_MSG_ERR;
            }

            pOmErrorLogCtrlInd->ulReceiverPid = pulErrorLogModule[i];
            pOmErrorLogCtrlInd->ulMsgName     = ID_OM_ERR_LOG_CTRL_IND;
            pOmErrorLogCtrlInd->usModemID     = pstAppOmCtrlStatus->usModemID;
            pOmErrorLogCtrlInd->ucAlmStatus   = pstAppOmCtrlStatus->ucAlmStatus;
            pOmErrorLogCtrlInd->ucAlmLevel    = pstAppOmCtrlStatus->ucAlmLevel;
            (VOS_VOID)VOS_SendMsg(ACPU_PID_OM, pOmErrorLogCtrlInd);
        }
    }

    /* 给AP回复消息 */
    OM_AcpuSendAppResult(OM_APP_MSG_OK, pstAppOmCtrlStatus->usModemID);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_AcpuReportFTMMsg
 功能描述  : 根据NV项配置向各组件下发工程模式 上报请求
 输入参数  : pstAppOmCtrlStatus: 收到数据

 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT OM_AcpuSwitchOnOffFTM(APP_OM_CTRL_STATUS_STRU *pstAppOmCtrlStatus)
{
    NV_ID_FTM_DETAIL_STRU               stNvFTMDetail;
    OM_FTM_CTRL_IND_STRU               *pstOmFtmCtrlInd;
    VOS_UINT32                          ulRest;
    VOS_UINT32                          i;

    /* modem ID 检查*/
    if (MODEM_ID_0 == pstAppOmCtrlStatus->usModemID)
    {
        g_stOmAppMsgRecord.pulFTMModule = g_aulModem0FTMDetail;
    }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == pstAppOmCtrlStatus->usModemID)
    {
        g_stOmAppMsgRecord.pulFTMModule = g_aulModem1FTMDetail;
    }
#endif
    else
    {
        OM_ERR_LOG("OM_AcpuSwitchOnOffFTM: modem id error!\r\n ");
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrNum++;
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrLen += (pstAppOmCtrlStatus->stOmHeader.ulMsgLen + sizeof(OM_ALARM_MSG_HEAD_STRU));
        OM_AcpuSendAppResult(OM_APP_SEND_MODEM_ID_ERR, pstAppOmCtrlStatus->usModemID);
        return OM_APP_SEND_MODEM_ID_ERR;
    }

    ulRest = NV_ReadEx(pstAppOmCtrlStatus->usModemID, en_NV_Item_FTMDetail,
                       (VOS_VOID*)&stNvFTMDetail, sizeof(NV_ID_FTM_DETAIL_STRU));
    if (VOS_OK != ulRest)
    {
        OM_ERR_LOG("OM_AcpuSwitchOnOffFTM: nv read faile!\r\n ");
        OM_AcpuSendAppResult(OM_APP_OMACPU_READ_NV_ERR, pstAppOmCtrlStatus->usModemID);
        return OM_APP_OMACPU_READ_NV_ERR;
    }

    g_stOmAppMsgRecord.ulFTMReportSend = stNvFTMDetail.ulFTMDetail;

    /* 因各组件任务优先级较高，先给AP回复消息。后便存在失败的场景，但可维可测，不以过度考虑 */
    OM_AcpuSendAppResult(OM_APP_MSG_OK, pstAppOmCtrlStatus->usModemID);

    /* 根据工程模式相关性，向对应PID发送消息 */
    for (i=0; i<OM_MAX_MODULE_ID; i++)
    {
        if ((0 !=(BIT_N(i) & g_stOmAppMsgRecord.ulFTMReportSend)) && (OM_PID_NULL != g_stOmAppMsgRecord.pulFTMModule[i]))
        {
            /* 给对应的PID发送消息 */
            pstOmFtmCtrlInd = (OM_FTM_CTRL_IND_STRU*)VOS_AllocMsg(ACPU_PID_OM,
                                  (sizeof(OM_FTM_CTRL_IND_STRU) - VOS_MSG_HEAD_LENGTH));
            if (VOS_NULL_PTR == pstOmFtmCtrlInd)
            {
                OM_AcpuSendAppResult(OM_APP_OMACPU_ALLOC_MSG_ERR, pstAppOmCtrlStatus->usModemID);
                return OM_APP_OMACPU_ALLOC_MSG_ERR;
            }

            pstOmFtmCtrlInd->ulReceiverPid = g_stOmAppMsgRecord.pulFTMModule[i];
            pstOmFtmCtrlInd->ulMsgName     = ID_OM_FTM_CTRL_IND;
            pstOmFtmCtrlInd->usModemID     = pstAppOmCtrlStatus->usModemID;
            pstOmFtmCtrlInd->ucActionFlag  = pstAppOmCtrlStatus->ucAlmStatus;

            (VOS_VOID)VOS_SendMsg(ACPU_PID_OM, pstOmFtmCtrlInd);
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_AcpuSwitchOnOff
 功能描述  : 判断是Error log开关还是工程模式开关
 输入参数  : pucData    : 收到数据
             ulLen : 数据长度

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT OM_AcpuSwitchOnOff(VOS_UINT8 *pucData, VOS_UINT32 ulLen)
{
    APP_OM_CTRL_STATUS_STRU              *pstAppOmCtrlStatus;

    pstAppOmCtrlStatus = (APP_OM_CTRL_STATUS_STRU*)pucData;

    if (OM_APP_SWITCH_MSG_ID_ERR_LOG == pstAppOmCtrlStatus->ulMsgModuleID)
    {
        /* Error log 开关 */
        return OM_AcpuSwitchOnOffErrLog(pstAppOmCtrlStatus);
    }
    else if (OM_APP_SWITCH_MSG_ID_FTM == pstAppOmCtrlStatus->ulMsgModuleID)
    {
        /* 工程模式开关 */
        return OM_AcpuSwitchOnOffFTM(pstAppOmCtrlStatus);
    }
    else
    {
        /* 异常信息上报 */
        OM_ERR_LOG("OM_AcpuSwitchOnOff: Msg Module id error!\r\n ");
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrNum++;
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrLen += ulLen;
        OM_AcpuSendAppResult(OM_APP_MSG_MODULE_ID_ERR, pstAppOmCtrlStatus->usModemID);
    }

    return OM_APP_MSG_MODULE_ID_ERR;
}

/*****************************************************************************
 函 数 名  : OM_AcpuRcvAppMsgCheck
 功能描述  : 参数检查
 输入参数  : pstAppOmReqErrLog    : 收到数据

 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT OM_AcpuRcvAppMsgCheck(APP_OM_REQ_ERR_LOG_STRU *pstAppOmReqErrLog)
{
    NV_ID_ERR_LOG_CTRL_INFO_STRU        stNvErrLogCtrlInfo;
    VOS_UINT32                          ulRest;

    /* modem ID 检查*/
    if (MODEM_ID_0 == pstAppOmReqErrLog->usModemID)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem0ErrLogDetail;
    }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == pstAppOmReqErrLog->usModemID)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem1ErrLogDetail;
    }
#endif
    else
    {
        OM_ERR_LOG("OM_AcpuRcvAppMsgCheck: modem id error!\r\n ");
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrNum++;
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrLen += (pstAppOmReqErrLog->stOmHeader.ulMsgLen + sizeof(OM_ALARM_MSG_HEAD_STRU));
        OM_AcpuSendAppResult(OM_APP_SEND_MODEM_ID_ERR, pstAppOmReqErrLog->usModemID);
        return OM_APP_SEND_MODEM_ID_ERR;
    }

    g_stOmAppMsgRecord.usModemId = pstAppOmReqErrLog->usModemID;

    /* 检查故障相关性的合法性 */
    if ( OM_MAX_FAULT_ID < pstAppOmReqErrLog->usFaultId)
    {
        OM_ERR_LOG1("OM_AcpuRcvAppMsgCheck: fault id error, id is %d!\r\n ", pstAppOmReqErrLog->usFaultId);
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrNum++;
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrLen += (pstAppOmReqErrLog->stOmHeader.ulMsgLen + sizeof(OM_ALARM_MSG_HEAD_STRU));
        OM_AcpuSendAppResult(OM_APP_SEND_FAULT_ID_ERR, pstAppOmReqErrLog->usModemID);
        return OM_APP_SEND_FAULT_ID_ERR;
    }

    /* 判断开关是否打开 */
    ulRest = NV_ReadEx(pstAppOmReqErrLog->usModemID,
                       en_NV_Item_ErrLogCtrlInfo,
                       (VOS_VOID*)&stNvErrLogCtrlInfo,
                       sizeof(NV_ID_ERR_LOG_CTRL_INFO_STRU));
    if (VOS_OK != ulRest)
    {
        OM_ERR_LOG1("OM_AcpuRcvAppMsgCheck: nv read fail nv id is: %d!\r\n",en_NV_Item_ErrLogCtrlInfo);
        OM_AcpuSendAppResult(OM_APP_OMACPU_READ_NV_ERR, pstAppOmReqErrLog->usModemID);
        return OM_APP_OMACPU_READ_NV_ERR;
    }

    /* 判断开关是否打开 */
    if (OM_APP_STATUS_CLOSE == stNvErrLogCtrlInfo.ucAlmStatus)
    {
        OM_ERR_LOG("OM_AcpuRcvAppMsgCheck: error log switch is close!\r\n");
        OM_AcpuSendAppResult(OM_APP_ERRLOG_SWITCH_CLOSE_ERR, pstAppOmReqErrLog->usModemID);
        return OM_APP_ERRLOG_SWITCH_CLOSE_ERR;
    }

    /* 如果没有上报完成 */
    if (OM_AP_SEND_MSG_FINISH != g_stOmAppMsgRecord.ulErrLogReportSend)
    {
        OM_ERR_LOG1("OM_AcpuRcvAppMsgCheck: error log report not end: %d!\r\n ", g_stOmAppMsgRecord.ulErrLogReportSend);
        OM_AcpuSendAppResult(OM_APP_REPORT_NOT_FINISH_ERR, pstAppOmReqErrLog->usModemID);
        return OM_APP_REPORT_NOT_FINISH_ERR;
    }

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : OM_AcpuRcvAppFaultIDCheck
 功能描述  : 参数检查
 输入参数  : pstAppOmReqErrLog    : 收到数据

 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT OM_AcpuRcvAppFaultIDCheck(APP_OM_REQ_ERR_LOG_STRU *pstAppOmReqErrLog)
{
    NV_ALARM_ID_RELATIONSHIP_STRU       stAlarmIdRelationship;
    VOS_UINT32                          ulFaultId = OM_ERR_FAULT_ID;
    VOS_ULONG                           ulLockLevel;
    VOS_UINT32                          i;

    if (VOS_OK != NV_ReadEx(pstAppOmReqErrLog->usModemID,
                       en_NV_Item_AlarmidRelationship,
                       (VOS_VOID*)&stAlarmIdRelationship,
                       sizeof(NV_ALARM_ID_RELATIONSHIP_STRU)))
    {
        OM_ERR_LOG1("OM_AcpuReportErrLogMsg: nv read fail nv id is: %d!\r\n", en_NV_Item_AlarmidRelationship);
        OM_AcpuSendAppResult(OM_APP_OMACPU_READ_NV_ERR, pstAppOmReqErrLog->usModemID);
        return OM_APP_OMACPU_READ_NV_ERR;
    }

    /* 根据告警标识查找告警相关性 */
    for (i=0; i < OM_MAX_FAULT_ID; i++)
    {
        if (pstAppOmReqErrLog->usFaultId == stAlarmIdRelationship.astOmAlarmidRelationship[i].ulAlarmid)
        {
            ulFaultId = i;
            break;
        }
    }

    /* 没有找到故障场景相关ID */
    if(OM_ERR_FAULT_ID == ulFaultId)
    {
        OM_ERR_LOG1("OM_AcpuReportErrLogMsg: no this fault id!\r\n ", pstAppOmReqErrLog->usFaultId);
        OM_AcpuSendAppResult(OM_APP_NO_FAULT_ID_ERR, pstAppOmReqErrLog->usModemID);
        return OM_APP_NO_FAULT_ID_ERR;
    }

    /*VOS_Splx(intLockLevel);*/
    VOS_SpinLockIntLock(&g_stVosErrLogSendSpinLock, ulLockLevel);

    g_stOmAppMsgRecord.ulErrLogReportSend = stAlarmIdRelationship.astOmAlarmidRelationship[ulFaultId].ulAlarmidDetail;

    VOS_SpinUnlockIntUnlock(&g_stVosErrLogSendSpinLock, ulLockLevel);

    return VOS_OK;

}

/*****************************************************************************
 函 数 名  : OM_AcpuReportErrLogMsg
 功能描述  : 根据NV项配置向各组件下发Errorlog 上报请求
 输入参数  : pucData    : 收到数据
             ulLen : 数据长度

 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT OM_AcpuReportErrLogMsg(VOS_UINT8 *pucData, VOS_UINT32 ulLen)
{
    APP_OM_REQ_ERR_LOG_STRU            *pstAppOmReqErrLog;
    OM_ERR_LOG_REPORT_REQ_STRU         *pstOmErrLogReportReq;
    VOS_UINT32                          ulSendPidCount = 0;
    VOS_INT                             lRest;
    VOS_UINT32                          i;

    pstAppOmReqErrLog = (APP_OM_REQ_ERR_LOG_STRU*)pucData;

    /* 收到AP消息检查 */
    lRest = OM_AcpuRcvAppMsgCheck(pstAppOmReqErrLog);
    if (VOS_OK != lRest)
    {
        return lRest;
    }

    /* 故障场景编号检查 */
    lRest = OM_AcpuRcvAppFaultIDCheck(pstAppOmReqErrLog);
    if (VOS_OK != lRest)
    {
        return lRest;
    }

    /* 因各组件任务优先级较高，先给AP回复消息。后便存在失败的场景，但可维可测，不以过度考虑 */
    OM_AcpuSendAppResult(OM_APP_MSG_OK, pstAppOmReqErrLog->usModemID);

    /* 根据告警相关性，向对应PID发送消息 */
    for (i=0; i<OM_MAX_MODULE_ID; i++)
    {
        if ((0 != (BIT_N(i) & g_stOmAppMsgRecord.ulErrLogReportSend)) && (0 != g_stOmAppMsgRecord.pulErrorLogModule[i]))
        {
            ulSendPidCount++;
            /* 给对应的PID发送消息 */
            pstOmErrLogReportReq  = (OM_ERR_LOG_REPORT_REQ_STRU*)VOS_AllocMsg(ACPU_PID_OM,
                                     (sizeof(OM_ERR_LOG_REPORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

            if (VOS_NULL_PTR == pstOmErrLogReportReq)
            {
                OM_AcpuSendAppResult(OM_APP_OMACPU_ALLOC_MSG_ERR, pstAppOmReqErrLog->usModemID);
                return OM_APP_OMACPU_ALLOC_MSG_ERR;
            }
            pstOmErrLogReportReq->ulReceiverPid = g_stOmAppMsgRecord.pulErrorLogModule[i];
            pstOmErrLogReportReq->ulMsgName     = ID_OM_ERR_LOG_REPORT_REQ;
            pstOmErrLogReportReq->usModemID     = pstAppOmReqErrLog->usModemID;

            (VOS_VOID)VOS_SendMsg(ACPU_PID_OM, pstOmErrLogReportReq);
        }
    }

    /* 假如没有告警相关性PID，直接给AP回复消息上报完毕 */
    if (0 == ulSendPidCount)
    {
        g_stOmAppMsgRecord.usModemId = pstAppOmReqErrLog->usModemID;
        OM_AcpuRcvMsgFinish();
        return VOS_OK;
    }

    /* 起5s定时器 */
    g_AcpuErrLogFullTmr = VOS_NULL_PTR;
    if (VOS_OK != VOS_StartRelTimer(&g_AcpuErrLogFullTmr, ACPU_PID_OM, OM_ERRLOG_TIMER_LENTH,
                                    OM_ERRLOG_TIMER, 0, VOS_RELTIMER_NOLOOP, VOS_TIMER_PRECISION_5))
    {
        OM_ERR_LOG("\r\n OM_AcpuReportErrLogMsg: StartDrxTimer fail\n");
        return OM_APP_ERRLOG_START_TIMER_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  :  OM_AcpuErrLogHook
 功能描述  :  可维可测，收到的消息内容发往log工具一份
 输入参数  :  pucData    : 收到数据
              ulLen      : 数据长度
              ulDateType : 消息类型,收到的VCOM消息，还是发送给VCOM消息

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年9月19日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuErrLogHook(VOS_UINT8 *pucData, VOS_UINT32 ulLen, VOS_UINT32 ulDateType)
{
    OM_ERRLOG_TRANS_MSG_STRU           *pstOmErrLogMsg;
    VOS_UINT32                          ulTotalLen;

    ulTotalLen = offsetof(OM_ERRLOG_TRANS_MSG_STRU, aucValue) + ulLen;

    pstOmErrLogMsg = (OM_ERRLOG_TRANS_MSG_STRU*)VOS_MemAlloc(ACPU_PID_OM, DYNAMIC_MEM_PT, ulTotalLen);

    if (VOS_NULL_PTR == pstOmErrLogMsg)
    {
        OM_ERR_LOG("OM_AcpuErrLogHook: VOS_AllocMem failure.\n");
        return;
    }

    VOS_MemCpy((VOS_VOID*)pstOmErrLogMsg->aucValue, (VOS_VOID*)pucData, ulLen);

    pstOmErrLogMsg->ucFuncType     = OM_TRANS_FUNC;
    pstOmErrLogMsg->usLength       = (VOS_UINT16)(ulTotalLen -sizeof(VOS_UINT32));
    pstOmErrLogMsg->usPrimId       = OM_APP_ERRLOG_HOOK_IND;
    pstOmErrLogMsg->ulDateType     = ulDateType;

    OM_AcpuAddSNTime(&(pstOmErrLogMsg->ulSn), &(pstOmErrLogMsg->ulTimeStamp));

    OM_AcpuSendData((OM_RSP_PACKET_STRU*)pstOmErrLogMsg, (VOS_UINT16)ulTotalLen);

    VOS_MemFree(ACPU_PID_OM, pstOmErrLogMsg);

    return;
}

/*****************************************************************************
 函 数 名  :  OM_AcpuSendVComData
 功能描述  :  调NAS接口，把数据发走
 输入参数  :  ucDevIndex: 物理端口
              pucData    : 收到数据
              ulLen : 数据长度

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuSendVComData(VOS_UINT8 *pucData, VOS_UINT32 ulLen)
{
    /* 调用注册函数，给Vcom发数据 */
    g_stErrLogVcomDebugInfo.ulVCOMSendNum++;
    g_stErrLogVcomDebugInfo.ulVCOMSendLen += ulLen;

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE((VOS_UINT8*)pucData, ulLen, OM_ACPU_ERRLOG_SEND);
    /*lint +e40*/

    /* 增加Trans勾包 */
    OM_AcpuErrLogHook(pucData, ulLen, OM_ERRLOG_SEND_MSG);

    if(VOS_OK != APP_VCOM_Send(APP_VCOM_DEV_INDEX_ERRLOG, pucData, ulLen))
    {
        g_stErrLogVcomDebugInfo.ulVCOMSendErrNum++;
        g_stErrLogVcomDebugInfo.ulVCOMSendErrLen += ulLen;

        OM_ERR_LOG("\r\n Info: OM_AcpuSendVComData: Nas Send Msg fail\n");

        return ;
    }
    return ;
}

/*****************************************************************************
 函 数 名  : OM_AcpuSendFTMMsgOther
 功能描述  : 收到AP工程模式命令
 输入参数  : pstAppOmReqFtm : 发送数据
             ulLen          : 发送数据长度
             ulPID          : 接收PID

 输出参数  : 无
 返 回 值  : VOS_VOID

 修改历史  :
   1.日    期  : 2014年2月14日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuSendFTMMsgOther(APP_OM_FTM_REQ_STRU *pstAppOmFtmReq, VOS_UINT32 ulLen, VOS_UINT32 ulPID)
{
    OM_FTM_REQUIRE_STRU                *pstOmFtmReq;

    if (VOS_NULL == ulPID)
    {
        return;
    }

    /* 工程模式命令上报通知对应的组件 */
    pstOmFtmReq  = (OM_FTM_REQUIRE_STRU*)VOS_AllocMsg(ACPU_PID_OM,
                                         (ulLen + sizeof(VOS_UINT32) + sizeof(VOS_UINT16)));

    if (VOS_NULL_PTR == pstOmFtmReq)
    {
        OM_AcpuSendAppResult(OM_APP_OMACPU_ALLOC_MSG_ERR, pstAppOmFtmReq->usModemID);
        OM_ERR_LOG("OM_AcpuSendFTMMsgOther:malloc msg fail\r\n");
        return ;
    }
    pstOmFtmReq->ulReceiverPid = ulPID;
    pstOmFtmReq->ulMsgName     = ID_OM_FTM_REQUIRE_IND;
    pstOmFtmReq->usModemID     = pstAppOmFtmReq->usModemID;
    VOS_MemCpy((VOS_VOID*)pstOmFtmReq->aucContent, (VOS_VOID*)pstAppOmFtmReq, ulLen);

    /* 提前回复消息 */
    OM_AcpuSendAppResult(OM_APP_MSG_OK, pstAppOmFtmReq->usModemID);

    (VOS_VOID)VOS_SendMsg(ACPU_PID_OM, pstOmFtmReq);

    return ;
}

/*****************************************************************************
 函 数 名  : OM_AcpuFTMMsgReq
 功能描述  : 收到AP工程模式命令
 输入参数  : pucData    : 收到数据
             ulLen      : 收到数据长度

 输出参数  : 无
 返 回 值  : VOS_INT
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2014年2月14日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT OM_AcpuFTMMsgReq(VOS_UINT8 *pucData, VOS_UINT32 ulLen)
{
    APP_OM_FTM_REQ_STRU                *pstAppOmReqFtm;
    VOS_UINT32                          ulIndex;

    pstAppOmReqFtm = (APP_OM_FTM_REQ_STRU*)pucData;

    /* modem ID 检查*/
    if (MODEM_ID_0 == pstAppOmReqFtm->usModemID)
    {
        for (ulIndex=0; ulIndex<(sizeof(g_astModem0FTMMsgModule) / sizeof(g_astModem0FTMMsgModule[0])); ulIndex++)
        {
            if (pstAppOmReqFtm->ulMsgModuleID == g_astModem0FTMMsgModule[ulIndex].ulMsgModuleID)
            {
                OM_AcpuSendFTMMsgOther(pstAppOmReqFtm, ulLen, g_astModem0FTMMsgModule[ulIndex].ulPID);
                break;
            }
        }

        if ( (sizeof(g_astModem0FTMMsgModule) / sizeof(g_astModem0FTMMsgModule[0])) <= ulIndex )
        {
            OM_ERR_LOG1("OM_AcpuFTMMsgReq:MsgModule ID is error %d\r\n", pstAppOmReqFtm->ulMsgModuleID);
            return OM_APP_MSG_MODULE_ID_ERR;
        }
    }

#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == pstAppOmReqFtm->usModemID)
    {
        for (ulIndex=0; ulIndex<(sizeof(g_astModem1FTMMsgModule) / sizeof(g_astModem1FTMMsgModule[0])); ulIndex++)
        {
            if (pstAppOmReqFtm->ulMsgModuleID == g_astModem1FTMMsgModule[ulIndex].ulMsgModuleID)
            {
                OM_AcpuSendFTMMsgOther(pstAppOmReqFtm, ulLen, g_astModem1FTMMsgModule[ulIndex].ulPID);
                break;
            }
        }

        if ( (sizeof(g_astModem0FTMMsgModule) / sizeof(g_astModem0FTMMsgModule[0])) <= ulIndex )
        {
            OM_ERR_LOG1("OM_AcpuFTMMsgReq:MsgModule ID is error %d\r\n", pstAppOmReqFtm->ulMsgModuleID);
            return OM_APP_MSG_MODULE_ID_ERR;
        }
    }
#endif
    else
    {
        OM_ERR_LOG("OM_AcpuFTMMsgReq: modem id error!\r\n ");
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrNum++;
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrLen += ulLen;
        OM_AcpuSendAppResult(OM_APP_SEND_MODEM_ID_ERR, pstAppOmReqFtm->usModemID);
        return OM_APP_SEND_MODEM_ID_ERR;
    }

    return OM_APP_MSG_OK;
}

/*****************************************************************************
 函 数 名  :  OM_AcpuRcvErrLogMsgCheck
 功能描述  :  收到各组件消息，检查是否需要上报
 输入参数  :  pstOmRcvDataInfo    : 收到数据

 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2014年2月18日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 OM_AcpuRcvErrLogMsgCheck(OM_RCV_DATA_INFO_STRU *pstOmRcvDataInfo, VOS_UINT32 *pulSendPidCount)
{
    VOS_ULONG                           ulLockLevel;
    VOS_UINT32                          i;

    /* Error Log 上报 */
    if (VOS_NULL_PTR == g_stOmAppMsgRecord.pulErrorLogModule)
    {
        OM_ERR_LOG("OM_AcpuRcvMsgCheck: not expect report Err log msg!\r\n ");
        return VOS_ERR;
    }

    g_stRcvUESendAP.ulErrLogRcvNum++;
    g_stRcvUESendAP.ulErrLogRcvLen += pstOmRcvDataInfo->stOmHeader.ulMsgLen;

    /* 记录对应组件已上报消息 */
    for (i=0; i<OM_MAX_MODULE_ID; i++)
    {
        if ((g_stOmAppMsgRecord.pulErrorLogModule[i] == pstOmRcvDataInfo->ulSenderPid)
            && (OM_PID_NULL != g_stOmAppMsgRecord.pulErrorLogModule[i])
            && (0 != (BIT_N(i) & g_stOmAppMsgRecord.ulErrLogReportSend)))
        {
            (*pulSendPidCount)++;
            g_stRcvUESendAP.ulErrLogSendNum++;
            g_stRcvUESendAP.ulErrLogSendLen += pstOmRcvDataInfo->stOmHeader.ulMsgLen;

            /*VOS_Splx(intLockLevel);*/
            VOS_SpinLockIntLock(&g_stVosErrLogSendSpinLock, ulLockLevel);

            /* 将相关PID置0，表明已经上报过 */
            g_stOmAppMsgRecord.ulErrLogReportSend = g_stOmAppMsgRecord.ulErrLogReportSend & (~((VOS_UINT32)(BIT_N(i))));

            VOS_SpinUnlockIntUnlock(&g_stVosErrLogSendSpinLock, ulLockLevel);
            break;
        }
    }

    OM_ERR_LOG2("OM_AcpuRcvMsgCheck:error log Module id is %d, g_stOmAppMsgRecord.ulErrLogReportSend is!\r\n ",
                 g_stOmAppMsgRecord.pulErrorLogModule[i], g_stOmAppMsgRecord.ulErrLogReportSend);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  :  OM_AcpuRcvFTMReportMsgCheck
 功能描述  :  收到各组件消息，检查是否需要上报
 输入参数  :  pstOmRcvDataInfo    : 收到数据

 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2014年2月18日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 OM_AcpuRcvFTMReportMsgCheck(OM_RCV_DATA_INFO_STRU *pstOmRcvDataInfo, VOS_UINT32 *pulSendPidCount)
{
    VOS_UINT32                          i;

    /* 工程模式主动上报 */
    if (VOS_NULL_PTR == g_stOmAppMsgRecord.pulFTMModule)
    {
        OM_ERR_LOG("OM_AcpuRcvMsgCheck: not expect report Ftm msg!\r\n ");
        return VOS_ERR;
    }

    /* 工程模式上报 */
    g_stRcvUESendAP.ulFTMRcvNum++;
    g_stRcvUESendAP.ulFTMRcvLen += pstOmRcvDataInfo->stOmHeader.ulMsgLen;
    for(i=0; i<OM_MAX_MODULE_ID; i++)
    {
        if ((g_stOmAppMsgRecord.pulFTMModule[i] == pstOmRcvDataInfo->ulSenderPid)
             && (0 !=(BIT_N(i) & g_stOmAppMsgRecord.ulFTMReportSend))
             && (OM_PID_NULL != g_stOmAppMsgRecord.pulFTMModule[i]))
        {
            (*pulSendPidCount)++;
            g_stRcvUESendAP.ulFTMSendNum++;
            g_stRcvUESendAP.ulFTMSendLen += pstOmRcvDataInfo->stOmHeader.ulMsgLen;

            break;
        }
    }

    OM_ERR_LOG2("OM_AcpuRcvMsgCheck: Module id is %d, g_stOmAppMsgRecord.ulFTMReportSend is!\r\n ",
                 g_stOmAppMsgRecord.pulFTMModule[i], g_stOmAppMsgRecord.ulFTMReportSend);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  :  OM_AcpuRcvFTMCnfMsgCheck
 功能描述  :  收到各组件消息，检查是否需要上报
 输入参数  :  pstOmRcvDataInfo    : 收到数据

 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2014年2月18日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 OM_AcpuRcvFTMCnfMsgCheck(OM_RCV_DATA_INFO_STRU *pstOmRcvDataInfo, VOS_UINT32 *pulSendPidCount)
{
    OM_APP_FTM_CNF_STRU                *pstOmAppFtmCnf;
    VOS_UINT32                          ulIndex;

    /* 工程模式响应上报最小消息长度 */
    if ((4*sizeof(VOS_UINT32)) > pstOmRcvDataInfo->stOmHeader.ulMsgLen)
    {
        OM_ERR_LOG1("OM_AcpuRcvFTMCnfMsgCheck:MsgModule ID is error %d\r\n", pstOmRcvDataInfo->stOmHeader.ulMsgLen);
        return OM_APP_MSG_LENGTH_ERR;
    }

    pstOmAppFtmCnf= (OM_APP_FTM_CNF_STRU*)(&pstOmRcvDataInfo->stOmHeader.ulMsgType);

    if (MODEM_ID_0 == pstOmAppFtmCnf->usModemID)
    {
        for (ulIndex=0; ulIndex<(sizeof(g_astModem0FTMMsgModule) / sizeof(g_astModem0FTMMsgModule[0])); ulIndex++)
        {
            if ((g_astModem0FTMMsgModule[ulIndex].ulMsgModuleID == pstOmAppFtmCnf->ulMsgModuleID)
                && (g_astModem0FTMMsgModule[ulIndex].ulPID == pstOmRcvDataInfo->ulSenderPid))
            {
               (*pulSendPidCount)++;
                break;
            }
        }

        if ((sizeof(g_astModem0FTMMsgModule) / sizeof(g_astModem0FTMMsgModule[0])) <= ulIndex)
        {
            OM_ERR_LOG1("OM_AcpuFTMMsgReq:MsgModule ID is error %d\r\n", pstOmAppFtmCnf->ulMsgModuleID);
            return OM_APP_MSG_MODULE_ID_ERR;
        }
    }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == pstOmAppFtmCnf->usModemID)
    {
        for (ulIndex=0; ulIndex<(sizeof(g_astModem1FTMMsgModule) / sizeof(g_astModem1FTMMsgModule[0])); ulIndex++)
        {
            if ((g_astModem1FTMMsgModule[ulIndex].ulMsgModuleID == pstOmAppFtmCnf->ulMsgModuleID)
                && (g_astModem1FTMMsgModule[ulIndex].ulPID == pstOmRcvDataInfo->ulSenderPid))
            {
               (*pulSendPidCount)++;
                break;
            }
        }

        if ((sizeof(g_astModem1FTMMsgModule) / sizeof(g_astModem1FTMMsgModule[0])) <= ulIndex)
        {
            OM_ERR_LOG1("OM_AcpuFTMMsgReq:MsgModule ID is error %d\r\n", pstOmAppFtmCnf->ulMsgModuleID);
            return OM_APP_MSG_MODULE_ID_ERR;
        }
    }
#endif
    else
    {
        return VOS_ERR;
    }

    return VOS_OK;

}
/*****************************************************************************
 函 数 名  :  OM_AcpuRcvMsgCheck
 功能描述  :  收到各组件消息，检查是否需要上报
 输入参数  :  pstOmRcvDataInfo    : 收到数据

 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 OM_AcpuRcvMsgCheck(OM_RCV_DATA_INFO_STRU *pstOmRcvDataInfo)
{
    VOS_UINT32                          ulSendPidCount = 0;

    switch(pstOmRcvDataInfo->stOmHeader.ulMsgType)
    {
        case OM_ERR_LOG_MSG_ERR_REPORT:
            /* Error Log 上报 */
            if(VOS_OK != OM_AcpuRcvErrLogMsgCheck(pstOmRcvDataInfo, &ulSendPidCount))
            {
                return VOS_ERR;
            }
            break;

        case OM_ERR_LOG_MSG_FTM_REPORT:
            /* 工程模式主动上报 */
            if(VOS_OK != OM_AcpuRcvFTMReportMsgCheck(pstOmRcvDataInfo, &ulSendPidCount))
            {
                return VOS_ERR;
            }
            break;

        case OM_ERR_LOG_MSG_FTM_CNF:
            /* 工程模式请求上报 */
            if(VOS_OK != OM_AcpuRcvFTMCnfMsgCheck(pstOmRcvDataInfo, &ulSendPidCount))
            {
                return VOS_ERR;
            }
            break;

        case OM_ERR_LOG_MSG_FAULT_REPORT:
            /* 平台检测故障主动上报 */
            ulSendPidCount++;
            break;

        case OM_ERR_LOG_MSG_ALARM_REPORT:
            /* 平台检测告警主动上报 */
            ulSendPidCount++;
            break;
        default:
            /* 异常 */
            OM_ERR_LOG("OM_AcpuRcvMsgCheck: Msg type error!\r\n ");
            return VOS_ERR;
    }

    /* 不是预期上报,丢弃消息 */
    if (OM_AP_NO_MSG_SEND == ulSendPidCount)
    {
        OM_ERR_LOG("OM_AcpuRcvMsgCheck: No msg Send!\r\n ");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_AcpuErrLogInfoShow
 功能描述  : 用于打印 Error log 调测信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无

 修改历史  :
   1.日    期  : 2013年08月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuErrLogInfoShow(VOS_VOID)
{
    vos_printf("\r\nVCom Send sn is           %d", g_stErrLogVcomDebugInfo.ulVCOMSendSn);
    vos_printf("\r\nVCom Send num is           %d", g_stErrLogVcomDebugInfo.ulVCOMSendNum);
    vos_printf("\r\nVCom Send Len is           %d", g_stErrLogVcomDebugInfo.ulVCOMSendLen);

    vos_printf("\r\nVCom Send Error num is         %d", g_stErrLogVcomDebugInfo.ulVCOMSendErrNum);
    vos_printf("\r\nVCom Send Error Len is         %d\r\n", g_stErrLogVcomDebugInfo.ulVCOMSendErrLen);

    vos_printf("\r\nVCom receive sn is           %d", g_stErrLogVcomDebugInfo.ulVCOMRcvSn);
    vos_printf("\r\nVCom receive num is           %d", g_stErrLogVcomDebugInfo.ulVCOMRcvNum);
    vos_printf("\r\nVCom receive Len is           %d", g_stErrLogVcomDebugInfo.ulVCOMRcvLen);

    vos_printf("\r\nVCom receive Error num is         %d", g_stErrLogVcomDebugInfo.ulVCOMRcvErrNum);
    vos_printf("\r\nVCom receive Error Len is         %d\r\n", g_stErrLogVcomDebugInfo.ulVCOMRcvErrLen);


    vos_printf("\r\nOM Rcv Err log msg num is         %d", g_stRcvUESendAP.ulErrLogRcvNum);
    vos_printf("\r\nOM Rcv Err log msg len is         %d", g_stRcvUESendAP.ulErrLogRcvLen);

    vos_printf("\r\nOM Rcv FTM msg num is         %d", g_stRcvUESendAP.ulFTMRcvNum);
    vos_printf("\r\nOM Rcv FTM msg len is         %d", g_stRcvUESendAP.ulFTMRcvLen);

    vos_printf("\r\nOM Send Err log msg num is         %d", g_stRcvUESendAP.ulErrLogSendNum);
    vos_printf("\r\nOM Send Err log msg len is         %d", g_stRcvUESendAP.ulErrLogSendLen);

    vos_printf("\r\nOM Send FTM msg num is         %d", g_stRcvUESendAP.ulFTMSendNum);
    vos_printf("\r\nOM Send FTM msg len is         %d\r\n", g_stRcvUESendAP.ulFTMSendLen);

    return;
}

/*****************************************************************************
 函 数 名  : OM_AcpuRcvAPCtrl
 功能描述  : 测试桩函数 : 模拟AP发送开关消息
 输入参数  :  ultype    : 消息类型
              ulValuse  : 消息名称
              usModemID : MODEM id
              ucAlmStatus :开关名称
              ucAlmLevel  :告警级别

 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuRcvAPCtrl(VOS_UINT32 ultype,          VOS_UINT32 ulValuse,
                          VOS_UINT16 usModemID,       VOS_UINT8  ucAlmStatus,
                          VOS_UINT8  ucAlmLevel)
{
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus;

    VOS_MemSet(&stAppOmCtrlStatus, 0, sizeof(stAppOmCtrlStatus));

    stAppOmCtrlStatus.stOmHeader.ulMsgType = ultype;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = ulValuse;
    stAppOmCtrlStatus.usModemID            = usModemID;
    stAppOmCtrlStatus.ucAlmStatus          = ucAlmStatus;
    stAppOmCtrlStatus.ucAlmLevel           = ucAlmLevel;

    OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU));
}


/*****************************************************************************
 函 数 名  : OM_AcpuReportErrLog
 功能描述  : 测试桩函数:模拟AP请求上报Error log
 输入参数  : usModemID : MODEM id
             usFaultId :告警相关性

 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuReportErrLog(VOS_UINT16 usModemID, VOS_UINT16 usFaultId)
{
    APP_OM_REQ_ERR_LOG_STRU              stAppOmCtrlStatus;

    VOS_MemSet(&stAppOmCtrlStatus, 0, sizeof(stAppOmCtrlStatus));

    stAppOmCtrlStatus.stOmHeader.ulMsgType = 0x01;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = 2;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = 0x01;
    stAppOmCtrlStatus.usModemID            = usModemID;
    stAppOmCtrlStatus.usFaultId            = usFaultId;

    OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_REQ_ERR_LOG_STRU));
}

#endif

/*****************************************************************************
 函 数 名  : OM_AcpuErrLogMsgProc
 功能描述  : OM收到各业务模块上报消息处理
 输入参数  : pMsg    : 收到数据

 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID OM_AcpuErrLogMsgProc(MsgBlock* pMsg)
{
#if(FEATURE_ON == FEATURE_PTM)
    OM_RCV_DATA_INFO_STRU              *pstOmRcvDataInfo;

    pstOmRcvDataInfo = (OM_RCV_DATA_INFO_STRU*)pMsg;

    /* 判断消息ID是否为规定消息ID */
    if ((ID_OM_FTM_REPROT_IND != pstOmRcvDataInfo->ulMsgName)
         && (ID_OM_ERR_LOG_REPORT_CNF != pstOmRcvDataInfo->ulMsgName)
         && (ID_OM_FTM_REQUIRE_CNF != pstOmRcvDataInfo->ulMsgName)
         && (ID_OM_FAULT_ERR_LOG_IND != pstOmRcvDataInfo->ulMsgName)
         && (ID_OM_ALARM_ERR_LOG_IND != pstOmRcvDataInfo->ulMsgName))
    {
        OM_ERR_LOG("OM_AcpuErrLogMsgProc: not error Log msg.\n");
        return ;
    }

    /*  收到消息检查是否需要上报 */
    if (VOS_OK != OM_AcpuRcvMsgCheck(pstOmRcvDataInfo))
    {
        OM_ERR_LOG("OM_AcpuErrLogMsgProc: rcv msg error!\r\n ");
        return ;
    }

    pstOmRcvDataInfo->stOmHeader.ulMsgSN = g_stErrLogVcomDebugInfo.ulVCOMSendSn++;

    OM_AcpuSendVComData((VOS_UINT8*)&pstOmRcvDataInfo->stOmHeader.ulMsgType,
                        pstOmRcvDataInfo->stOmHeader.ulMsgLen + sizeof(OM_ALARM_MSG_HEAD_STRU));

    if ((OM_ERR_LOG_MSG_ERR_REPORT == pstOmRcvDataInfo->stOmHeader.ulMsgType)
        && (OM_AP_SEND_MSG_FINISH == g_stOmAppMsgRecord.ulErrLogReportSend))
    {
        /* 停定时器 */
        VOS_StopRelTimer(&g_AcpuErrLogFullTmr);
        OM_AcpuRcvMsgFinish();
    }

    OM_ERR_LOG("OM_AcpuErrLogMsgProc: rcv msg Send Seccussr\n ");

#endif

    return ;
}

/*****************************************************************************
 函 数 名  : OM_AcpuReadVComData
 功能描述  : NAS把收到数据调用OM 接口读走
 输入参数  :  ucDevIndex: 物理端口
              pucData    : 收到数据
              ulLen : 数据长度

 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月27日
     作    者  : d00212987
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT OM_AcpuReadVComData(VOS_UINT8 ucDevIndex, VOS_UINT8 *pucData, VOS_UINT32 ulLen)
{
#if(FEATURE_ON == FEATURE_PTM)
    OM_ALARM_MSG_HEAD_STRU             *pstOmAlarmMsgHead;

    if (APP_VCOM_DEV_INDEX_ERRLOG != ucDevIndex)
    {
        OM_ERR_LOG1("\r\n OM_AcpuReadVComData:PhyPort port is error: %d\n", ucDevIndex);
        OM_AcpuSendAppResult(OM_APP_VCOM_DEV_INDEX_ERR, MODEM_ID_BUTT);

        return VOS_ERR;
    }

#if (VOS_LINUX == VOS_OS_VER)
     down(&g_stOmRxErrorLogBuffSem);
#endif

    g_stErrLogVcomDebugInfo.ulVCOMRcvNum++;
    g_stErrLogVcomDebugInfo.ulVCOMRcvLen += ulLen;

    if ((VOS_NULL_PTR == pucData) || (sizeof(APP_OM_CTRL_STATUS_STRU)> ulLen))
    {
        OM_ERR_LOG1("\r\n OM_AcpuReadVComData:Send data is NULL or len is error:%d\n", ulLen);
        OM_AcpuSendAppResult(OM_APP_MSG_LENGTH_ERR, MODEM_ID_BUTT);
#if (VOS_LINUX == VOS_OS_VER)
        up(&g_stOmRxErrorLogBuffSem);
#endif
        return VOS_ERR;
    }

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE((VOS_UINT8*)pucData, ulLen, OM_ACPU_ERRLOG_RCV);
    /*lint +e40*/

    /* 增加Trans勾包 */
    OM_AcpuErrLogHook(pucData, ulLen, OM_ERRLOG_RCV_MSG);

    /* 根据消息头判断命令类型 */
    pstOmAlarmMsgHead = (OM_ALARM_MSG_HEAD_STRU *)pucData;

    g_stErrLogVcomDebugInfo.ulVCOMRcvSn = pstOmAlarmMsgHead->ulMsgSN;

    if (OM_ERR_LOG_MSG_ON_OFF == pstOmAlarmMsgHead->ulMsgType)
    {
#if (VOS_LINUX == VOS_OS_VER)
        up(&g_stOmRxErrorLogBuffSem);
#endif
        return OM_AcpuSwitchOnOff(pucData, ulLen);
    }
    else if(OM_ERR_LOG_MSG_SET_ERR_REPORT == pstOmAlarmMsgHead->ulMsgType)
    {
#if (VOS_LINUX == VOS_OS_VER)
        up(&g_stOmRxErrorLogBuffSem);
#endif
        return OM_AcpuReportErrLogMsg(pucData, ulLen);
    }
    else if(OM_ERR_LOG_MSG_FTM_REQ == pstOmAlarmMsgHead->ulMsgType)
    {
#if (VOS_LINUX == VOS_OS_VER)
        up(&g_stOmRxErrorLogBuffSem);
#endif
        return OM_AcpuFTMMsgReq(pucData, ulLen);
    }
    else
    {
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrNum++;
        g_stErrLogVcomDebugInfo.ulVCOMRcvErrLen += ulLen;
        OM_ERR_LOG("\r\n OM_AcpuReadVComData:Recv msg type is err\n");
        OM_AcpuSendAppResult(OM_APP_MSG_TYPE_ERR, MODEM_ID_BUTT);
    }

#if (VOS_LINUX == VOS_OS_VER)
    up(&g_stOmRxErrorLogBuffSem);
#endif

#endif

    return VOS_ERR;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
