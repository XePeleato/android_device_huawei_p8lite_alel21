#include "omerrorlog.h"
#include "omnvinterface.h"
#include "OmApp.h"
#include "AtAppVcomInterface.h"


#if(FEATURE_ON == FEATURE_PTM)

extern OM_APP_MSG_RECORD_STRU       g_stOmAppMsgRecord;

extern VOS_UINT32    g_aulModem0ErrLogDetail[OM_MAX_MODULE_ID];
extern VOS_UINT32    g_aulModem0FTMDetail[OM_MAX_MODULE_ID];
extern APP_OM_FTM_MSG_PID_STRU    g_astModem0FTMMsgModule[30];


#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
extern VOS_UINT32    g_aulModem1ErrLogDetail[OM_MAX_MODULE_ID];
extern VOS_UINT32    g_aulModem1FTMDetail[OM_MAX_MODULE_ID];
extern APP_OM_FTM_MSG_PID_STRU    g_astModem1FTMMsgModule[30];
#endif

extern VOS_INT  OM_AcpuReadVComData(VOS_UINT8 ucDevIndex, VOS_UINT8 *pData, VOS_UINT32 uslength);
extern VOS_VOID OM_AcpuMsgProc(MsgBlock* pMsg);
extern VOS_VOID OM_AcpuErrLogInfoShow(VOS_VOID);
extern VOS_INT OM_AcpuRcvAppMsgCheck(APP_OM_REQ_ERR_LOG_STRU *pstAppOmReqErrLog);
extern VOS_VOID OM_AcpuRcvAPCtrl(VOS_UINT32 ultype,          VOS_UINT32 ulValuse,
    VOS_UINT16 usModemID,       VOS_UINT8  ucAlmStatus,
    VOS_UINT8  ucAlmLevel);
extern VOS_VOID OM_AcpuReportErrLog(VOS_UINT16 usModemID, VOS_UINT16 usFaultId);
extern VOS_VOID OM_AcpuErrLogHook(VOS_UINT8 *pucData, VOS_UINT32 ulLen, VOS_UINT32 ulDateType);
#endif


unsigned int uttest_OM_AcpuReadVComData_case1(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus={0};

    // 执行检查
    if (VOS_ERR != OM_AcpuReadVComData(13,(VOS_UINT8*)&stAppOmCtrlStatus,sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case2(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    VOS_UINT8               *pauc=VOS_NULL_PTR;

    // 执行检查
    if (VOS_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, pauc, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case3(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    VOS_UINT8               *pauc=VOS_NULL_PTR;

    // 执行检查
    if (VOS_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, pauc, sizeof(APP_OM_CTRL_STATUS_STRU)-5))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

/************************************************************************/
/*                   下发Error Log开关                                  */
/************************************************************************/

unsigned int uttest_OM_AcpuReadVComData_case4(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_ERR_LOG;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_BUTT;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;


    // 执行检查
    if (OM_APP_SEND_MODEM_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case5(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_ERR_LOG;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_0;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (OM_APP_OMACPU_WRITE_NV_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}



unsigned int uttest_OM_AcpuReadVComData_case6(void)
{

#if ((FEATURE_ON == FEATURE_PTM)&&(FEATURE_ON == FEATURE_MULTI_MODEM))
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_ERR_LOG;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_1;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (OM_APP_OMACPU_WRITE_NV_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}



unsigned int uttest_OM_AcpuReadVComData_case7(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_ERR_LOG;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_0;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (OM_APP_OMACPU_ALLOC_MSG_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case8(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_ERR_LOG;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_0;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (VOS_OK != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

/************************************************************************/
/*                   下发FTM 模式开关                                   */
/************************************************************************/

unsigned int uttest_OM_AcpuReadVComData_case9(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_FTM;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_BUTT;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (OM_APP_SEND_MODEM_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case10(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_FTM;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_0;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (OM_APP_OMACPU_READ_NV_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case11(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_FTM_REQ_STRU                  stAppOmFtmReq = {0};

    stAppOmFtmReq.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REQ;
    stAppOmFtmReq.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmFtmReq.stOmHeader.ulMsgLen  = 5*sizeof(VOS_UINT32);
    stAppOmFtmReq.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_GUNAS;
    stAppOmFtmReq.usModemID            = MODEM_ID_0;
    stAppOmFtmReq.usProjectMsgID       = OM_APP_STATUS_CLOSE;
    stAppOmFtmReq.ulProjectLength           = sizeof(VOS_UINT32);

    // 执行检查
    if (OM_APP_MSG_MODULE_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmFtmReq, sizeof(APP_OM_FTM_REQ_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case12(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_FTM_REQ_STRU                  stAppOmFtmReq = {0};

    stAppOmFtmReq.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REQ;
    stAppOmFtmReq.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmFtmReq.stOmHeader.ulMsgLen  = 5*sizeof(VOS_UINT32);
    stAppOmFtmReq.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_BUTT;
    stAppOmFtmReq.usModemID            = MODEM_ID_0;
    stAppOmFtmReq.usProjectMsgID       = OM_APP_STATUS_CLOSE;
    stAppOmFtmReq.ulProjectLength           = sizeof(VOS_UINT32);

    // 执行检查
    if (OM_APP_MSG_MODULE_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmFtmReq, sizeof(APP_OM_FTM_REQ_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case13(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_FTM_REQ_STRU                  stAppOmFtmReq = {0};

    stAppOmFtmReq.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REQ;
    stAppOmFtmReq.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmFtmReq.stOmHeader.ulMsgLen  = 5*sizeof(VOS_UINT32);
    stAppOmFtmReq.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_IMS;
    stAppOmFtmReq.usModemID            = MODEM_ID_0;
    stAppOmFtmReq.usProjectMsgID       = OM_APP_STATUS_CLOSE;
    stAppOmFtmReq.ulProjectLength           = sizeof(VOS_UINT32);

    g_astModem0FTMMsgModule[0].ulMsgModuleID = OM_ERR_LOG_MOUDLE_ID_IMS;
    g_astModem0FTMMsgModule[0].ulPID         = WUEPS_PID_OM_CALLBACK;
    // 执行检查
    if (OM_APP_MSG_OK != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmFtmReq, sizeof(APP_OM_FTM_REQ_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case14(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_FTM_REQ_STRU                  stAppOmFtmReq = {0};

    stAppOmFtmReq.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REQ;
    stAppOmFtmReq.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmFtmReq.stOmHeader.ulMsgLen  = 5*sizeof(VOS_UINT32);
    stAppOmFtmReq.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_IMS;
    stAppOmFtmReq.usModemID            = 9;
    stAppOmFtmReq.usProjectMsgID       = OM_APP_STATUS_CLOSE;
    stAppOmFtmReq.ulProjectLength           = sizeof(VOS_UINT32);

    g_astModem0FTMMsgModule[0].ulMsgModuleID = OM_ERR_LOG_MOUDLE_ID_IMS;
    g_astModem0FTMMsgModule[0].ulPID         = WUEPS_PID_OM_CALLBACK;

    // 执行检查
    if (OM_APP_SEND_MODEM_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmFtmReq, sizeof(APP_OM_FTM_REQ_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case15(void)
{
#if ((FEATURE_ON == FEATURE_PTM)&&( FEATURE_ON == FEATURE_MULTI_MODEM ))
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_FTM;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_1;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (OM_APP_OMACPU_READ_NV_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case16(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_FTM;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_0;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (OM_APP_OMACPU_ALLOC_MSG_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

NV_ID_FTM_DETAIL_STRU               g_stNvFTMDetail;

VOS_UINT32 uttest_NV_ReadEx_stub2(MODEM_ID_ENUM_UINT16     enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    g_stNvFTMDetail.ulFTMDetail = 0x03;

    memcpy(pItem, &g_stNvFTMDetail, ulLength);

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case17(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};
    NV_ID_FTM_DETAIL_STRU               stNvFTMDetail;

    stNvFTMDetail.ulFTMDetail = 0x03;

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_FTM;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_0;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (VOS_OK != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case18(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_CTRL_STATUS_STRU              stAppOmCtrlStatus = {0};

    stAppOmCtrlStatus.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ON_OFF;
    stAppOmCtrlStatus.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmCtrlStatus.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmCtrlStatus.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmCtrlStatus.usModemID            = MODEM_ID_0;
    stAppOmCtrlStatus.ucAlmStatus          = OM_APP_STATUS_CLOSE;
    stAppOmCtrlStatus.ucAlmLevel           = 0x01;

    // 执行检查
    if (OM_APP_MSG_MODULE_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmCtrlStatus, sizeof(APP_OM_CTRL_STATUS_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

/************************************************************************/
/*                   下发故障模式上报请求                               */
/************************************************************************/

unsigned int uttest_OM_AcpuReadVComData_case19(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_BUTT;
    stAppOmReqErrLog.usFaultId            = 0x23;

    // 执行检查
    if (OM_APP_SEND_MODEM_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case20(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_BUTT;
    stAppOmReqErrLog.usFaultId            = 0x23;

    // 执行检查
    if (VOS_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}



unsigned int uttest_OM_AcpuReadVComData_case21(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_0;
    stAppOmReqErrLog.usFaultId            = 0x45;

    // 执行检查
    if (OM_APP_SEND_FAULT_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case22(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_0;
    stAppOmReqErrLog.usFaultId            = 0x01;

    // 执行检查
    if (OM_APP_OMACPU_READ_NV_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

NV_ID_ERR_LOG_CTRL_INFO_STRU        g_stNvErrLogCtrlInfo;


VOS_UINT32 uttest_NV_ReadEx_stub3(MODEM_ID_ENUM_UINT16     enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    g_stNvErrLogCtrlInfo.ucAlmStatus = OM_APP_STATUS_CLOSE;

    memcpy(pItem, &g_stNvErrLogCtrlInfo, ulLength);

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case23(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_0;
    stAppOmReqErrLog.usFaultId            = 0x04;

    // 执行检查
    if (OM_APP_ERRLOG_SWITCH_CLOSE_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

VOS_UINT32 uttest_NV_ReadEx_stub4(MODEM_ID_ENUM_UINT16     enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    g_stNvErrLogCtrlInfo.ucAlmStatus = OM_APP_STATUS_OPEN;

    memcpy(pItem, &g_stNvErrLogCtrlInfo, ulLength);

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case24(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_0;
    stAppOmReqErrLog.usFaultId            = 0x04;

   g_stOmAppMsgRecord.ulErrLogReportSend = 0x01;

    // 执行检查
   if (OM_APP_REPORT_NOT_FINISH_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
   {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

NV_ALARM_ID_RELATIONSHIP_STRU       g_stAlarmIdRelationship = {0};

VOS_UINT32 uttest_NV_ReadEx_stub5(MODEM_ID_ENUM_UINT16     enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{

    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmid       = 0x01;
    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmidDetail = 0x01;

    memcpy(pItem, &g_stAlarmIdRelationship, ulLength);

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case25(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_0;
    stAppOmReqErrLog.usFaultId            = 0x04;

    g_stOmAppMsgRecord.ulErrLogReportSend = 0x00;

    // 执行检查
    if (OM_APP_NO_FAULT_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

   return VOS_OK;
}

VOS_UINT32 uttest_NV_ReadEx_stub6(MODEM_ID_ENUM_UINT16     enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{

    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmid       = 0x01;
    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmidDetail = 0x01;

    memcpy(pItem, &g_stAlarmIdRelationship, ulLength);

    return VOS_ERR;
}


unsigned int uttest_OM_AcpuReadVComData_case26(void)
{
#if (FEATURE_ON == FEATURE_PTM)
   APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

   stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
   stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
   stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
   stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
   stAppOmReqErrLog.usModemID            = MODEM_ID_0;
   stAppOmReqErrLog.usFaultId            = 0x04;

   g_stOmAppMsgRecord.ulErrLogReportSend = 0x01;

   // 执行检查
   if (OM_APP_OMACPU_READ_NV_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
   {
       return VOS_ERR;
   }
#endif

   return VOS_OK;
}

VOS_UINT32 uttest_NV_ReadEx_stub7(MODEM_ID_ENUM_UINT16     enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmid       = 0x01;
    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmidDetail = 0x01;

    memcpy(pItem, &g_stAlarmIdRelationship, ulLength);

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case27(void)
{
#if (FEATURE_ON == FEATURE_PTM)

   APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

   stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
   stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
   stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
   stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
   stAppOmReqErrLog.usModemID            = MODEM_ID_0;
   stAppOmReqErrLog.usFaultId            = 0x04;

   // 执行检查
   if (OM_APP_NO_FAULT_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
   {
       return VOS_ERR;
   }
#endif


   return VOS_OK;
}

VOS_UINT32 uttest_NV_ReadEx_stub8(MODEM_ID_ENUM_UINT16     enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmid       = 0x04;
    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmidDetail = 0x01;

    memcpy(pItem, &g_stAlarmIdRelationship, ulLength);

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case28(void)
{
#if (FEATURE_ON == FEATURE_PTM)

   APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};


   stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
   stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
   stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
   stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
   stAppOmReqErrLog.usModemID            = MODEM_ID_0;
   stAppOmReqErrLog.usFaultId            = 0x04;

   g_stOmAppMsgRecord.usModemId    = MODEM_ID_0;

   g_stOmAppMsgRecord.ulErrLogReportSend = 0x03;

   /* modem ID 检查*/
   if (MODEM_ID_0 == g_stOmAppMsgRecord.usModemId)
   {
       g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem0ErrLogDetail;
   }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
   else if (MODEM_ID_1 == g_stOmAppMsgRecord.usModemId)
   {
       g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem1ErrLogDetail;
   }
#endif


   // 执行检查
   if (VOS_OK != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
   {
       return VOS_ERR;
   }
#endif

    return VOS_OK;
}
unsigned int uttest_OM_AcpuReadVComData_case29(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_0;
    stAppOmReqErrLog.usFaultId            = 0x04;

    g_stOmAppMsgRecord.usModemId    = MODEM_ID_0;
    g_stOmAppMsgRecord.ulErrLogReportSend = 0x03;

    /* modem ID 检查*/
    if (MODEM_ID_0 == g_stOmAppMsgRecord.usModemId)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem0ErrLogDetail;
    }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == g_stOmAppMsgRecord.usModemId)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem1ErrLogDetail;
    }
#endif

    // 执行检查
    if (OM_APP_ERRLOG_START_TIMER_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
unsigned int uttest_OM_AcpuReadVComData_case30(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_0;
    stAppOmReqErrLog.usFaultId            = 0x04;

    g_stOmAppMsgRecord.usModemId    = MODEM_ID_0;
    g_stOmAppMsgRecord.ulErrLogReportSend = 0x03;

    /* modem ID 检查*/
    if (MODEM_ID_0 == g_stOmAppMsgRecord.usModemId)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem0ErrLogDetail;
    }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == g_stOmAppMsgRecord.usModemId)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem1ErrLogDetail;
    }
#endif

    // 执行检查
    if (OM_APP_OMACPU_ALLOC_MSG_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

VOS_UINT32 uttest_NV_ReadEx_stub9(MODEM_ID_ENUM_UINT16     enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmid       = 0x04;
    g_stAlarmIdRelationship.astOmAlarmidRelationship[0].ulAlarmidDetail = 0x04;

    memcpy(pItem, &g_stAlarmIdRelationship, ulLength);

    return VOS_OK;
}
unsigned int uttest_OM_AcpuReadVComData_case31(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_REQ_ERR_LOG_STRU              stAppOmReqErrLog = {0};

    stAppOmReqErrLog.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_SET_ERR_REPORT;
    stAppOmReqErrLog.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmReqErrLog.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);
    stAppOmReqErrLog.ulMsgModuleID        = OM_APP_SWITCH_MSG_ID_BUTT;
    stAppOmReqErrLog.usModemID            = MODEM_ID_0;
    stAppOmReqErrLog.usFaultId            = 0x04;

    g_stOmAppMsgRecord.usModemId    = MODEM_ID_0;
    g_stOmAppMsgRecord.ulErrLogReportSend = 0x03;

    /* modem ID 检查*/
    if (MODEM_ID_0 == g_stOmAppMsgRecord.usModemId)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem0ErrLogDetail;
    }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == g_stOmAppMsgRecord.usModemId)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem1ErrLogDetail;
    }
#endif

    // 执行检查
    if (VOS_OK != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmReqErrLog, sizeof(APP_OM_REQ_ERR_LOG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
unsigned int uttest_OM_AcpuReadVComData_case32(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_FTM_REQ_STRU                  stAppOmFtmReq = {0};

    stAppOmFtmReq.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REQ;
    stAppOmFtmReq.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmFtmReq.stOmHeader.ulMsgLen  = 5*sizeof(VOS_UINT32);
    stAppOmFtmReq.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_BUTT;
    stAppOmFtmReq.usModemID            = MODEM_ID_1;
    stAppOmFtmReq.usProjectMsgID       = OM_APP_STATUS_CLOSE;
    stAppOmFtmReq.ulProjectLength           = sizeof(VOS_UINT32);

    g_astModem1FTMMsgModule[0].ulMsgModuleID = OM_ERR_LOG_MOUDLE_ID_IMS;
    g_astModem1FTMMsgModule[0].ulPID         = WUEPS_PID_OM_CALLBACK;

    // 执行检查
    if (OM_APP_MSG_MODULE_ID_ERR != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmFtmReq, sizeof(APP_OM_FTM_REQ_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


unsigned int uttest_OM_AcpuReadVComData_case33(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_FTM_REQ_STRU                  stAppOmFtmReq = {0};

    stAppOmFtmReq.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REQ;
    stAppOmFtmReq.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmFtmReq.stOmHeader.ulMsgLen  = 5*sizeof(VOS_UINT32);
    stAppOmFtmReq.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_IMS;
    stAppOmFtmReq.usModemID            = MODEM_ID_1;
    stAppOmFtmReq.usProjectMsgID       = OM_APP_STATUS_CLOSE;
    stAppOmFtmReq.ulProjectLength      = sizeof(VOS_UINT32);

    g_astModem1FTMMsgModule[0].ulMsgModuleID = OM_ERR_LOG_MOUDLE_ID_IMS;
    g_astModem1FTMMsgModule[0].ulPID         = WUEPS_PID_OM_CALLBACK;

    // 执行检查
    if (OM_APP_MSG_OK != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmFtmReq, sizeof(APP_OM_FTM_REQ_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

unsigned int uttest_OM_AcpuReadVComData_case34(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    APP_OM_FTM_REQ_STRU                  stAppOmFtmReq = {0};

    stAppOmFtmReq.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REQ;
    stAppOmFtmReq.stOmHeader.ulMsgSN   = VOS_OK;
    stAppOmFtmReq.stOmHeader.ulMsgLen  = 5*sizeof(VOS_UINT32);
    stAppOmFtmReq.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_IMS;
    stAppOmFtmReq.usModemID            = MODEM_ID_1;
    stAppOmFtmReq.usProjectMsgID       = OM_APP_STATUS_CLOSE;
    stAppOmFtmReq.ulProjectLength      = sizeof(VOS_UINT32);

    g_astModem1FTMMsgModule[0].ulMsgModuleID = OM_ERR_LOG_MOUDLE_ID_IMS;
    g_astModem1FTMMsgModule[0].ulPID         = WUEPS_PID_OM_CALLBACK;

    // 执行检查
    if (OM_APP_MSG_OK != OM_AcpuReadVComData(APP_VCOM_DEV_INDEX_ERRLOG, (VOS_UINT8*)&stAppOmFtmReq, sizeof(APP_OM_FTM_REQ_STRU)))
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}



/************************************************************************/
/*                         各组件上报消息内容                           */
/************************************************************************/

void uttest_OM_AcpuMsgProc_case1(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_RCV_DATA_INFO_STRU              stOmRcvDataInfo;

    stOmRcvDataInfo.ulMsgName            = ID_OM_FTM_CTRL_IND;
    stOmRcvDataInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ERR_REPORT;
    stOmRcvDataInfo.stOmHeader.ulMsgSN   = VOS_OK;
    stOmRcvDataInfo.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);

    OM_AcpuMsgProc((MsgBlock*)&stOmRcvDataInfo);
#endif
}


void uttest_OM_AcpuMsgProc_case2(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_RCV_DATA_INFO_STRU              stOmRcvDataInfo;

    stOmRcvDataInfo.ulSenderPid          = I0_WUEPS_PID_MMC;
    stOmRcvDataInfo.ulMsgName            = ID_OM_ERR_LOG_REPORT_CNF;
    stOmRcvDataInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ERR_REPORT;
    stOmRcvDataInfo.stOmHeader.ulMsgSN   = VOS_OK;
    stOmRcvDataInfo.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);

    g_stOmAppMsgRecord.pulErrorLogModule = VOS_NULL_PTR;

    OM_AcpuMsgProc((MsgBlock*)&stOmRcvDataInfo);
#endif
}

void uttest_OM_AcpuMsgProc_case3(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_RCV_DATA_INFO_STRU              stOmRcvDataInfo;

    stOmRcvDataInfo.ulSenderPid          = I0_WUEPS_PID_MMC;
    stOmRcvDataInfo.ulMsgName            = ID_OM_ERR_LOG_REPORT_CNF;
    stOmRcvDataInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ERR_REPORT;
    stOmRcvDataInfo.stOmHeader.ulMsgSN   = VOS_OK;
    stOmRcvDataInfo.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);

    g_stOmAppMsgRecord.usModemId    = MODEM_ID_0;
    g_stOmAppMsgRecord.ulErrLogReportSend = 0x03;

    /* modem ID 检查*/
    if (MODEM_ID_0 == g_stOmAppMsgRecord.usModemId)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem0ErrLogDetail;
    }
#if ( FEATURE_ON == FEATURE_MULTI_MODEM )
    else if (MODEM_ID_1 == g_stOmAppMsgRecord.usModemId)
    {
        g_stOmAppMsgRecord.pulErrorLogModule = g_aulModem1ErrLogDetail;
    }
#endif

    OM_AcpuMsgProc((MsgBlock*)&stOmRcvDataInfo);

    stOmRcvDataInfo.ulSenderPid          = I0_UEPS_PID_MTA;

    OM_AcpuMsgProc((MsgBlock*)&stOmRcvDataInfo);
#endif
}
void uttest_OM_AcpuMsgProc_case4(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_RCV_DATA_INFO_STRU              stOmRcvDataInfo;

    stOmRcvDataInfo.ulSenderPid          = I0_WUEPS_PID_MMC;
    stOmRcvDataInfo.ulMsgName            = ID_OM_FTM_REPROT_IND;
    stOmRcvDataInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REPORT;
    stOmRcvDataInfo.stOmHeader.ulMsgSN   = VOS_OK;
    stOmRcvDataInfo.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);

    stOmRcvDataInfo.ulSenderPid          = I0_WUEPS_PID_MMC;

    OM_AcpuMsgProc((MsgBlock*)&stOmRcvDataInfo);
#endif
}


void uttest_OM_AcpuMsgProc_case5(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_RCV_DATA_INFO_STRU              stOmRcvDataInfo;

    stOmRcvDataInfo.ulSenderPid          = I0_WUEPS_PID_MMC;
    stOmRcvDataInfo.ulMsgName            = ID_OM_FTM_REPROT_IND;
    stOmRcvDataInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_REPORT;
    stOmRcvDataInfo.stOmHeader.ulMsgSN   = VOS_OK;
    stOmRcvDataInfo.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);

    stOmRcvDataInfo.ulSenderPid          = I0_WUEPS_PID_MMC;

    g_stOmAppMsgRecord.pulFTMModule      = VOS_NULL_PTR;

    OM_AcpuMsgProc((MsgBlock*)&stOmRcvDataInfo);
#endif
}


void uttest_OM_AcpuMsgProc_case6(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_RCV_DATA_INFO_STRU              stOmRcvDataInfo;

    stOmRcvDataInfo.ulSenderPid          = I0_WUEPS_PID_MMC;
    stOmRcvDataInfo.ulMsgName            = ID_OM_FTM_REPROT_IND;
    stOmRcvDataInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_TYPE_BUTT;
    stOmRcvDataInfo.stOmHeader.ulMsgSN   = VOS_OK;
    stOmRcvDataInfo.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);

    OM_AcpuMsgProc((MsgBlock*)&stOmRcvDataInfo);
    OM_AcpuErrLogInfoShow();
    OM_AcpuRcvAPCtrl(OM_ERR_LOG_MSG_ON_OFF, OM_APP_SWITCH_MSG_ID_ERR_LOG,0, 1,2);
    OM_AcpuReportErrLog(1, 0);
#endif
}


void uttest_OM_AcpuMsgProc_case7(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_RCV_DATA_INFO_STRU              stOmRcvDataInfo;

    stOmRcvDataInfo.ulSenderPid          = WUEPS_PID_OM_CALLBACK;
    stOmRcvDataInfo.ulMsgName            = ID_OM_FTM_REQUIRE_CNF;
    stOmRcvDataInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_CNF;
    stOmRcvDataInfo.stOmHeader.ulMsgSN   = VOS_OK;
    stOmRcvDataInfo.stOmHeader.ulMsgLen  = 2*sizeof(VOS_UINT32);

    g_stOmAppMsgRecord.pulFTMModule      = VOS_NULL_PTR;

    OM_AcpuMsgProc((MsgBlock*)&stOmRcvDataInfo);
#endif
}


void uttest_OM_AcpuMsgProc_case8(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_FTM_REPROT_IND_STRU             *pstOmFtmReportInd;
    OM_APP_FTM_CNF_STRU                 stOmAppFtmCnfInfo;

    pstOmFtmReportInd = (OM_FTM_REPROT_IND_STRU*)malloc(24+sizeof(OM_APP_FTM_CNF_STRU));
    if (VOS_NULL_PTR == pstOmFtmReportInd)
    {
        return;
    }

    pstOmFtmReportInd->ulSenderPid          = WUEPS_PID_OM_CALLBACK;
    pstOmFtmReportInd->ulMsgName            = ID_OM_FTM_REQUIRE_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgSN   = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.stOmHeader.ulMsgLen  = 4*sizeof(VOS_UINT32);

    memcpy((VOS_UINT8*)&pstOmFtmReportInd->ulMsgType, &stOmAppFtmCnfInfo, sizeof(stOmAppFtmCnfInfo));

    OM_AcpuMsgProc((MsgBlock*)pstOmFtmReportInd);
#endif
}


void uttest_OM_AcpuMsgProc_case9(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_FTM_REPROT_IND_STRU             *pstOmFtmReportInd;
    OM_APP_FTM_CNF_STRU                 stOmAppFtmCnfInfo;

    pstOmFtmReportInd = (OM_FTM_REPROT_IND_STRU*)malloc(24+sizeof(OM_APP_FTM_CNF_STRU));
    if (VOS_NULL_PTR == pstOmFtmReportInd)
    {
        return;
    }

    pstOmFtmReportInd->ulSenderPid          = WUEPS_PID_OM_CALLBACK;
    pstOmFtmReportInd->ulMsgName            = ID_OM_FTM_REQUIRE_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgSN   = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.stOmHeader.ulMsgLen  = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.usModemID            = MODEM_ID_0;

    memcpy((VOS_UINT8*)&pstOmFtmReportInd->ulMsgType, &stOmAppFtmCnfInfo, sizeof(stOmAppFtmCnfInfo));

    OM_AcpuMsgProc((MsgBlock*)pstOmFtmReportInd);
#endif
}


void uttest_OM_AcpuMsgProc_case10(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_FTM_REPROT_IND_STRU             *pstOmFtmReportInd;
    OM_APP_FTM_CNF_STRU                 stOmAppFtmCnfInfo;

    pstOmFtmReportInd = (OM_FTM_REPROT_IND_STRU*)malloc(24+sizeof(OM_APP_FTM_CNF_STRU));
    if (VOS_NULL_PTR == pstOmFtmReportInd)
    {
        return;
    }

    pstOmFtmReportInd->ulSenderPid          = WUEPS_PID_OM_CALLBACK;
    pstOmFtmReportInd->ulMsgName            = ID_OM_FTM_REQUIRE_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgSN   = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.stOmHeader.ulMsgLen  = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.usModemID            = MODEM_ID_0;
    stOmAppFtmCnfInfo.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_IMS;

    memcpy((VOS_UINT8*)&pstOmFtmReportInd->ulMsgType, &stOmAppFtmCnfInfo, sizeof(stOmAppFtmCnfInfo));

    OM_AcpuMsgProc((MsgBlock*)pstOmFtmReportInd);
#endif
}

void uttest_OM_AcpuMsgProc_case11(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_FTM_REPROT_IND_STRU             *pstOmFtmReportInd;
    OM_APP_FTM_CNF_STRU                 stOmAppFtmCnfInfo;

    pstOmFtmReportInd = (OM_FTM_REPROT_IND_STRU*)malloc(24+sizeof(OM_APP_FTM_CNF_STRU));
    if (VOS_NULL_PTR == pstOmFtmReportInd)
    {
        return;
    }

    pstOmFtmReportInd->ulSenderPid          = WUEPS_PID_OM_CALLBACK;
    pstOmFtmReportInd->ulMsgName            = ID_OM_FTM_REQUIRE_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgSN   = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.stOmHeader.ulMsgLen  = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.usModemID            = MODEM_ID_1;

    memcpy((VOS_UINT8*)&pstOmFtmReportInd->ulMsgType, &stOmAppFtmCnfInfo, sizeof(stOmAppFtmCnfInfo));

    OM_AcpuMsgProc((MsgBlock*)pstOmFtmReportInd);
#endif
}


void uttest_OM_AcpuMsgProc_case12(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_FTM_REPROT_IND_STRU             *pstOmFtmReportInd;
    OM_APP_FTM_CNF_STRU                 stOmAppFtmCnfInfo;

    pstOmFtmReportInd = (OM_FTM_REPROT_IND_STRU*)malloc(24+sizeof(OM_APP_FTM_CNF_STRU));
    if (VOS_NULL_PTR == pstOmFtmReportInd)
    {
        return;
    }

    pstOmFtmReportInd->ulSenderPid          = WUEPS_PID_OM_CALLBACK;
    pstOmFtmReportInd->ulMsgName            = ID_OM_FTM_REQUIRE_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FTM_CNF;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgSN   = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.stOmHeader.ulMsgLen  = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.usModemID            = MODEM_ID_1;
    stOmAppFtmCnfInfo.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_IMS;

    memcpy((VOS_UINT8*)&pstOmFtmReportInd->ulMsgType, &stOmAppFtmCnfInfo, sizeof(stOmAppFtmCnfInfo));

    OM_AcpuMsgProc((MsgBlock*)pstOmFtmReportInd);
#endif
}


void uttest_OM_AcpuMsgProc_case13(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_FTM_REPROT_IND_STRU             *pstOmFtmReportInd;
    OM_APP_FTM_CNF_STRU                 stOmAppFtmCnfInfo;

    pstOmFtmReportInd = (OM_FTM_REPROT_IND_STRU*)malloc(24+sizeof(OM_APP_FTM_CNF_STRU));
    if (VOS_NULL_PTR == pstOmFtmReportInd)
    {
        return;
    }

    pstOmFtmReportInd->ulSenderPid          = WUEPS_PID_OM_CALLBACK;
    pstOmFtmReportInd->ulMsgName            = ID_OM_FAULT_ERR_LOG_IND;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_FAULT_REPORT;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgSN   = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.stOmHeader.ulMsgLen  = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.usModemID            = MODEM_ID_1;
    stOmAppFtmCnfInfo.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_IMS;

    memcpy((VOS_UINT8*)&pstOmFtmReportInd->ulMsgType, &stOmAppFtmCnfInfo, sizeof(stOmAppFtmCnfInfo));

    OM_AcpuMsgProc((MsgBlock*)pstOmFtmReportInd);
#endif
}


void uttest_OM_AcpuMsgProc_case14(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    OM_FTM_REPROT_IND_STRU             *pstOmFtmReportInd;
    OM_APP_FTM_CNF_STRU                 stOmAppFtmCnfInfo;

    pstOmFtmReportInd = (OM_FTM_REPROT_IND_STRU*)malloc(24+sizeof(OM_APP_FTM_CNF_STRU));
    if (VOS_NULL_PTR == pstOmFtmReportInd)
    {
        return;
    }

    pstOmFtmReportInd->ulSenderPid          = WUEPS_PID_OM_CALLBACK;
    pstOmFtmReportInd->ulMsgName            = ID_OM_FAULT_ERR_LOG_IND;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgType = OM_ERR_LOG_MSG_ALARM_REPORT;
    stOmAppFtmCnfInfo.stOmHeader.ulMsgSN   = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.stOmHeader.ulMsgLen  = 4*sizeof(VOS_UINT32);
    stOmAppFtmCnfInfo.usModemID            = MODEM_ID_1;
    stOmAppFtmCnfInfo.ulMsgModuleID        = OM_ERR_LOG_MOUDLE_ID_IMS;

    memcpy((VOS_UINT8*)&pstOmFtmReportInd->ulMsgType, &stOmAppFtmCnfInfo, sizeof(stOmAppFtmCnfInfo));

    OM_AcpuMsgProc((MsgBlock*)pstOmFtmReportInd);
#endif
}


void uttest_OM_AcpuErrLogHook_case1(void)
{
#if (FEATURE_ON == FEATURE_PTM)
    unsigned char                       aucData[100];
    OM_AcpuErrLogHook(aucData, 100, 00);
#endif
}