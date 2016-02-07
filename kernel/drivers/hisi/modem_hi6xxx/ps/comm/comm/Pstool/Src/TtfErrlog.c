


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "TtfErrlog.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TTF_ERR_LOG_C


#if (FEATURE_PTM == FEATURE_ON)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
TTF_MNTN_ERR_LOG_ENT_STRU               g_stTtfMntnErrlogEnt = {0};
TTF_MNTN_ERR_LOG_RPT_CB                 apErrLogRptCb[MODEM_ID_BUTT][MODULE_ID_BUTT];

/* ALM ID从1开始的，所以数组0的取值为BUTT */
VOS_UINT8                               g_aucTtfErrlogAlmLevTbl[TTF_ERR_LOG_ALM_ID_BUTT] =
{
    TTF_ERR_LOG_ALM_LEV_BUTT,
    TTF_ERR_LOG_ALM_LEV_MAJOR,
    TTF_ERR_LOG_ALM_LEV_CRITICAL,
    TTF_ERR_LOG_ALM_LEV_CRITICAL,
    TTF_ERR_LOG_ALM_LEV_CRITICAL,
    TTF_ERR_LOG_ALM_LEV_MAJOR,
    TTF_ERR_LOG_ALM_LEV_CRITICAL,
    TTF_ERR_LOG_ALM_LEV_MAJOR
};

TTF_MNTN_ERR_LOG_WRLC_LI_ERR_CTX_STRU   g_stErrLogLiErrCtx;


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958 */

VOS_VOID TTF_MNTN_InitErrlogBuffer(MODEM_ID_ENUM_UINT16  enModemId)
{
    if (VOS_NULL_PTR != TTF_ERR_LOG_ENT_BUF_GET(enModemId))
    {
        return;
    }

    TTF_ERR_LOG_ENT_BUF_SET(enModemId, (VOS_UINT8 *)PS_ALLOC_STATIC_MEM(UEPS_PID_SN, TTF_ERR_LOG_BUF_SIZE));

    TTF_ERR_LOG_ENT_RINGID_SET(enModemId, OM_RingBufferCreateEx((VOS_CHAR *)TTF_ERR_LOG_ENT_BUF_GET(enModemId), TTF_ERR_LOG_BUF_SIZE));
}



VOS_UINT32 TTF_MNTN_InitErrLogEnt(VOS_UINT32 ulPid)
{
    MODEM_ID_ENUM_UINT16        enModemId;
    VOS_UINT32                  ulRslt;

    if (VOS_YES == TTF_ERR_LOG_ENT_STATE_GET())
    {
        return VOS_OK;
    }

    PS_MEM_SET(&g_stTtfMntnErrlogEnt, 0, sizeof(TTF_MNTN_ERR_LOG_ENT_STRU));
    PS_MEM_SET(apErrLogRptCb, 0, sizeof(apErrLogRptCb));

    /* 创建互斥信号量，每个Modem 1个 */
    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        ulRslt    = VOS_SmMCreate("TFEL", VOS_SEMA4_FIFO,
                        (VOS_SEM *)(&TTF_ERR_LOG_ENT_SEM_GET(enModemId)));

        if (VOS_OK != ulRslt)
        {
            PS_LOG2(ulPid, 0, PS_PRINT_ERROR,
                "TTF_MNTN_InitErrLogEnt: Create TTF_ERR_LOG_ENT_SEM<1>, ulRslt<2> fail!", enModemId, (VOS_INT32)ulRslt);

            return ulRslt;
        }


        /* 读NV项获取当前上报状态 */
        ulRslt = NV_ReadEx(enModemId, en_NV_Item_ErrLogCtrlInfo,
                   &g_stTtfMntnErrlogEnt.stCtrlInfo[enModemId],
                   sizeof(NV_ID_ERR_LOG_CTRL_INFO_STRU));

        if (VOS_OK != ulRslt)
        {
            PS_LOG1(ulPid, 0, PS_PRINT_ERROR,
                "TTF_MNTN_InitErrLogEnt: NV_Read fail!",  (VOS_INT32)ulRslt);

            TTF_ERR_LOG_ENT_CTRL_STATUS_SET(enModemId, OM_APP_STATUS_CLOSE);
        }

        TTF_MNTN_InitErrlogBuffer(enModemId);
    }

    PS_MEM_SET(&g_stErrLogLiErrCtx, 0, sizeof(TTF_MNTN_ERR_LOG_WRLC_LI_ERR_CTX_STRU));

    TTF_ERR_LOG_ENT_STATE_SET(VOS_YES);

    return VOS_OK;
}


VOS_UINT32 TTF_MNTN_ErrLogRcvCtrlInd(MsgBlock *pstMsg, void *p)
{
    OM_ERROR_LOG_CTRL_IND_STRU *pstCtrlInd;
    MODEM_ID_ENUM_UINT16        enModemId;

    pstCtrlInd = (OM_ERROR_LOG_CTRL_IND_STRU *)pstMsg;

    enModemId = VOS_GetModemIDFromPid(pstCtrlInd->ulReceiverPid);

    TTF_ERR_LOG_ENT_CTRL_STATUS_SET(enModemId, pstCtrlInd->ucAlmStatus);
    TTF_ERR_LOG_ENT_CTRL_LEV_SET(enModemId, pstCtrlInd->ucAlmLevel);

    return VOS_OK;
}


VOS_VOID TTF_MNTN_ErrlogBufRpt(VOS_UINT32 ulSenderPid, VOS_UINT32 ulReceiverPid, MODEM_ID_ENUM_UINT16 enModemId)
{
    VOS_UINT32                      ulRslt;
    VOS_UINT32                      ulBufSize;
    VOS_UINT32                      ulMsgLen;
    OM_RING_ID                      pRingId;
    VOS_UINT32                      ulCpuID;
    OM_ERR_LOG_REPORT_CNF_STRU     *pstLogRpt;
    VOS_UINT32                      ulOffSetLen;

    if (enModemId >= MODEM_ID_BUTT)
    {
        PS_LOG1(ulSenderPid, 0, PS_PRINT_ERROR,
            "TTF_MNTN_ErrlogBufRpt: enModemId Invalid!", enModemId);

        return;
    }

    ulCpuID         = VOS_GET_CPU_ID(ulReceiverPid);

    if ( ulCpuID >= VOS_SUPPORT_CPU_NUM_MAX )
    {
        PS_LOG1(ulSenderPid, 0, PS_PRINT_ERROR,
            "TTF_MNTN_ErrlogBufRpt: ulReceiverPid Invalid!", (VOS_INT32)ulReceiverPid);

        return;
    }

    pRingId   = TTF_ERR_LOG_ENT_RINGID_GET(enModemId);

    /* Default没有异常 */
    ulBufSize = 0;

    if (VOS_NULL_PTR != pRingId)
    {
        /* 保留的长度复位 */
        TTF_ERR_LOG_ENT_RST_BUF_RSV_LEN(enModemId);

        /* 将各模块本地缓存的数据写入Buffer */
        TTF_MNTN_ErrlogCbRun(enModemId);

        ulRslt = VOS_SmP(TTF_ERR_LOG_ENT_SEM_GET(enModemId), TTF_ERR_SEM_TIMEOUT_LEN);

        if(VOS_OK != ulRslt)
        {
            PS_LOG2(ulSenderPid, 0, PS_PRINT_WARNING,
                "TTF_MNTN_ErrlogBufRpt: VOS_SmP <1>, ulRslt<2> fail!", enModemId, (VOS_INT32)ulRslt);

            return;
        }

        ulBufSize = (VOS_UINT32)OM_RingBufferNBytes(pRingId);

        if (ulBufSize > TTF_ERR_LOG_BUF_SIZE)
        {
            OM_RingBufferFlush(pRingId);

            PS_LOG1(ulSenderPid, 0, PS_PRINT_WARNING,
                "TTF_MNTN_ErrlogBufGet: ulBufSize invalid!", (VOS_INT32)ulBufSize);

            ulBufSize = 0;
        }

        VOS_SmV(TTF_ERR_LOG_ENT_SEM_GET(enModemId));
    }

    TTF_GET_OFFSET(ulOffSetLen, OM_ERR_LOG_REPORT_CNF_STRU, aucContent[0]);
    ulMsgLen  = ulOffSetLen + ulBufSize;
    pstLogRpt = (OM_ERR_LOG_REPORT_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, ulMsgLen);

    if (VOS_NULL_PTR == pstLogRpt)
    {
        return;
    }

    if (0 != ulBufSize)
    {
        TTF_MNTN_ErrlogBufGet(ulSenderPid, enModemId, (VOS_CHAR *)(pstLogRpt->aucContent), ulBufSize);
    }

    pstLogRpt->ulReceiverPid    = ulReceiverPid;
    pstLogRpt->ulMsgName        = ID_OM_ERR_LOG_REPORT_CNF;
    pstLogRpt->ulMsgType        = OM_ERR_LOG_MSG_ERR_REPORT;
    pstLogRpt->ulMsgSN          = VOS_GetSlice();
    pstLogRpt->ulRptlen         = ulBufSize;

    PS_SEND_MSG(UEPS_PID_SN, pstLogRpt);
}
VOS_UINT32 TTF_MNTN_ErrLogRcvRptReq(MsgBlock *pstMsg, void *p)
{
    OM_ERR_LOG_REPORT_REQ_STRU *pstRptReq;

    pstRptReq = (OM_ERR_LOG_REPORT_REQ_STRU *)pstMsg;

    /* 收到OM消息的ReciverPid作为发送PID, 发送PID作为回复消息的接收PID */
    TTF_MNTN_ErrlogBufRpt(pstRptReq->ulReceiverPid, pstRptReq->ulSenderPid, pstRptReq->usModemID);

    return VOS_OK;
}
VOS_VOID TTF_MNTN_ErrlogBufGet(VOS_UINT32 ulPid, MODEM_ID_ENUM_UINT16  enModemId, VOS_CHAR *pBuffer, VOS_UINT32 ulBytes)
{
    VOS_UINT32              ulRslt;
    OM_RING_ID              pRingId;


    if (VOS_NULL_PTR == pBuffer)
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR,
            "TTF_MNTN_ErrlogBufGet: pBuffer NULL PTR");

        return;
    }

    if (enModemId >= MODEM_ID_BUTT)
    {
        PS_LOG1(ulPid, 0, PS_PRINT_ERROR,
            "TTF_MNTN_ErrlogBufGet: Invalid ModemId ", enModemId);

        return;
    }

    pRingId   = TTF_ERR_LOG_ENT_RINGID_GET(enModemId);

    if (VOS_NULL_PTR == pRingId)
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR,
            "TTF_MNTN_ErrlogBufGet: pRingId NULL PTR");

        return;
    }

    ulRslt = VOS_SmP(TTF_ERR_LOG_ENT_SEM_GET(enModemId), TTF_ERR_SEM_TIMEOUT_LEN);

    if(VOS_OK != ulRslt)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
            "TTF_MNTN_ErrlogBufGet: VOS_SmP <1>, ulRslt<2> fail!", enModemId, (VOS_INT32)ulRslt);

        return;
    }

    ulRslt = (VOS_UINT32)OM_RingBufferGet(pRingId, pBuffer, (VOS_INT)ulBytes);

    if (ulRslt != ulBytes)
    {
        OM_RingBufferFlush(pRingId);

        PS_LOG2(ulPid, 0, PS_PRINT_ERROR,
            "TTF_MNTN_ErrlogBufGet: ERROR OM_RingBufferGet fail!", (VOS_INT32)ulRslt, (VOS_INT32)ulBytes);
    }

    VOS_SmV(TTF_ERR_LOG_ENT_SEM_GET(enModemId));
}


VOS_VOID TTF_MNTN_ErrlogBufPut(VOS_UINT32 ulPid,  VOS_CHAR *pBuffer, VOS_UINT32 ulBytes)
{
    MODEM_ID_ENUM_UINT16    enModemId;
    VOS_UINT32              ulRslt;
    VOS_UINT32              ulBuffFreeSize;
    OM_RING_ID              pRingId;


    if (VOS_NULL_PTR == pBuffer)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING,
            "TTF_MNTN_ErrlogBufPut: pBuffer NULL PTR");

        return;
    }

    enModemId = VOS_GetModemIDFromPid(ulPid);

    if (enModemId >= MODEM_ID_BUTT)
    {
        PS_LOG1(ulPid, 0, PS_PRINT_ERROR,
            "TTF_MNTN_ErrlogBufPut: Invalid ModemId ", enModemId);

        return;
    }

    pRingId   = TTF_ERR_LOG_ENT_RINGID_GET(enModemId);

    if (VOS_NULL_PTR == pRingId)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING,
            "TTF_MNTN_ErrlogBufPut: pRingId NULL PTR");

        return;
    }

    ulRslt = VOS_SmP(TTF_ERR_LOG_ENT_SEM_GET(enModemId), TTF_ERR_SEM_TIMEOUT_LEN);

    if(VOS_OK != ulRslt)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
            "TTF_MNTN_ErrlogBufPut: Create TTF_ERR_LOG_ENT_SEM<1>, ulRslt<2> fail!", enModemId, (VOS_INT32)ulRslt);

        return;
    }

    ulBuffFreeSize  =(VOS_UINT32) OM_RingBufferFreeBytes(pRingId);

    /* Buffer已经不够了，就不再往里面写数据了 */
    if (ulBuffFreeSize >= (ulBytes + TTF_ERR_LOG_ENT_GET_BUF_RSV_LEN(enModemId)))
    {
        ulRslt = (VOS_UINT32)OM_RingBufferPut(pRingId, pBuffer, (VOS_INT)ulBytes);

        if (ulBytes != ulRslt)
        {
            OM_RingBufferFlush(pRingId);

            PS_LOG2(ulPid, 0, PS_PRINT_ERROR,
                "TTF_MNTN_ErrlogBufGet: ERROR OM_RingBufferPut fail!",(VOS_INT32)ulRslt, (VOS_INT32)ulBytes);
        }
    }

    VOS_SmV(TTF_ERR_LOG_ENT_SEM_GET(enModemId));
}
VOS_UINT8 TTF_MNTN_ErrLogGetModuleId(VOS_UINT32 ulPid)
{
    VOS_UINT32 ulIndx;
    TTF_MNTN_ERR_LOG_PID_MODULE_STRU astPidModuleTb[] =
    {
        {I0_UEPS_PID_SN,       MODULE_ID_SN},
        {I1_UEPS_PID_SN,       MODULE_ID_SN},
        {I0_UEPS_PID_GRM,      MODULE_ID_GRM},
        {I1_UEPS_PID_GRM,      MODULE_ID_GRM},
        {I0_UEPS_PID_LL,       MODULE_ID_LLC},
        {I1_UEPS_PID_LL,       MODULE_ID_LLC},
        {I0_UEPS_PID_DL,       MODULE_ID_DL},
        {I1_UEPS_PID_DL,       MODULE_ID_DL},
        {WUEPS_PID_PDCP,       MODULE_ID_PDCP},
        {WUEPS_PID_RLC,        MODULE_ID_WRM},
        {WUEPS_PID_MAC,        MODULE_ID_WRM}
    };

    for (ulIndx = 0; ulIndx < TTF_ARRAY_SIZE(astPidModuleTb); ulIndx++)
    {
        if (astPidModuleTb[ulIndx].ulPid == ulPid)
        {
            return (VOS_UINT8)astPidModuleTb[ulIndx].ulModuleId;
        }
    }

    return MODULE_ID_BUTT;
}


VOS_VOID TTF_MNTN_ErrlogCbReg(VOS_UINT32 ulPid, TTF_MNTN_ERR_LOG_RPT_CB pErrlogCb)
{
    TTF_MODULE_ID_ENUM_UINT8 enModuleId;
    MODEM_ID_ENUM_UINT16     enModemId;


    enModuleId = TTF_MNTN_ErrLogGetModuleId(ulPid);

    if (MODULE_ID_BUTT <= enModuleId)
    {
        return;
    }

    enModemId = VOS_GetModemIDFromPid(ulPid);

    if (enModemId >= MODEM_ID_BUTT)
    {
        PS_LOG1(ulPid, 0, PS_PRINT_ERROR,
            "TTF_MNTN_ErrlogCbReg: Invalid ModemId ", enModemId);

        return;
    }

    TTF_ERR_LOG_RPT_CB_SET(enModemId, enModuleId, pErrlogCb);
}


VOS_VOID TTF_MNTN_ErrlogCbRun(MODEM_ID_ENUM_UINT16 enModemId)
{
    VOS_UINT32                  ulIndx;
    TTF_MNTN_ERR_LOG_RPT_CB     pErrlogRptCb;

    if (enModemId >= MODEM_ID_BUTT)
    {
        return;
    }

    for(ulIndx = 0; ulIndx < MODULE_ID_BUTT; ulIndx++)
    {
        pErrlogRptCb = TTF_ERR_LOG_RPT_CB_GET(enModemId, ulIndx);
        if (VOS_NULL_PTR != pErrlogRptCb)
        {
            pErrlogRptCb();
        }
    }
}


VOS_VOID GTTF_MNTN_ErrlogTbfAbnmlEvt(VOS_UINT32 ulPid, VOS_UINT16 usType)
{
    TTF_MNTN_ERR_LOG_GRLC_TBF_ABNML_STRU stTbfAbnml;
    VOS_UINT8                            ucAlmLev;
    MODEM_ID_ENUM_UINT16                 enModemId;

    ucAlmLev    = TTF_ERR_LOG_GET_ALM_LEV(TTF_ERR_LOG_ALM_ID_GPRS_TBF_ABNML);
    enModemId   = VOS_GetModemIDFromPid(ulPid);

    if (!TTF_ERR_LOG_NEED_RPT_LEV(enModemId, ucAlmLev))
    {
        return;
    }

    TTF_ERR_LOG_FILL_HEADER(
        &stTbfAbnml.stHeader,
        enModemId,
        TTF_ERR_LOG_ALM_ID_GPRS_TBF_ABNML,
        ucAlmLev,
        (sizeof(TTF_MNTN_ERR_LOG_GRLC_TBF_ABNML_STRU) - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stTbfAbnml.usAbnmlType = (VOS_UINT16)usType;

    TTF_MNTN_ErrlogBufPut(ulPid, (VOS_CHAR *)&stTbfAbnml, sizeof(TTF_MNTN_ERR_LOG_GRLC_TBF_ABNML_STRU));
}


VOS_VOID GTTF_MNTN_ErrlogMdlErrEvt(VOS_UINT32 ulPid, VOS_VOID *pMdlErr)
{
    TTF_MNTN_ERR_LOG_GDL_MDL_ERR_STRU       stMdlErrEvt;
    VOS_UINT8                               ucAlmLev;
    MODEM_ID_ENUM_UINT16                    enModemId;

    if (VOS_NULL_PTR == pMdlErr)
    {
        return;
    }

    ucAlmLev    = TTF_ERR_LOG_GET_ALM_LEV(TTF_ERR_LOG_ALM_ID_GSM_MDL_ERR);
    enModemId   = VOS_GetModemIDFromPid(ulPid);

    if (!TTF_ERR_LOG_NEED_RPT_LEV(enModemId, ucAlmLev))
    {
        return;
    }

    TTF_ERR_LOG_FILL_HEADER(
        &stMdlErrEvt.stHeader,
        enModemId,
        TTF_ERR_LOG_ALM_ID_GSM_MDL_ERR,
        ucAlmLev,
        (sizeof(TTF_MNTN_ERR_LOG_GDL_MDL_ERR_STRU) - sizeof(OM_ERR_LOG_HEADER_STRU)));

    PS_MEM_CPY(&stMdlErrEvt.stDlMdlErrInfo, pMdlErr, sizeof(TTF_MNTN_GDL_MDL_ERR_INFO_STRU));

    TTF_MNTN_ErrlogBufPut(ulPid, (VOS_CHAR *)&stMdlErrEvt, sizeof(TTF_MNTN_ERR_LOG_GDL_MDL_ERR_STRU));
}

/*****************************************************************************
 函 数 名  : WTTF_MNTN_ErrlogRlcResetEvt
 功能描述  : RLC Reset event record
 输入参数  : VOS_UINT8 ucRbId                                           - Rb Id
             TTF_MNTN_ERR_LOG_WRLC_RESET_TYPE_ENUM_UINT8 enResetType    - Rlc reset type

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013.09.20
    作    者   : x59651
    修改内容   : create new function

*****************************************************************************/
VOS_VOID WTTF_MNTN_ErrlogRlcResetEvt(VOS_UINT8 ucRbId,
    TTF_MNTN_ERR_LOG_WRLC_RESET_TYPE_ENUM_UINT8 enResetType)
{
    TTF_MNTN_ERR_LOG_WRLC_RESET_STRU    stTtfMntnWrlcReset;
    VOS_UINT8                           ucAlmLev;
    MODEM_ID_ENUM_UINT16                enModemId;

    ucAlmLev    = TTF_ERR_LOG_GET_ALM_LEV(TTF_ERR_LOG_ALM_ID_WRLC_RESET);
    enModemId   = VOS_GetModemIDFromPid(WUEPS_PID_RLC);

    if (!TTF_ERR_LOG_NEED_RPT_LEV(enModemId, ucAlmLev))
    {
        return;
    }

    TTF_ERR_LOG_FILL_HEADER(
        &stTtfMntnWrlcReset.stHeader,
        enModemId,
        TTF_ERR_LOG_ALM_ID_WRLC_RESET,
        ucAlmLev,
        (sizeof(TTF_MNTN_ERR_LOG_WRLC_RESET_STRU) - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stTtfMntnWrlcReset.ucRbId       = ucRbId;
    stTtfMntnWrlcReset.enResetType  = enResetType;
    stTtfMntnWrlcReset.usResetCnt   = 1;

    /* 生成Reset事件 */
    TTF_MNTN_ErrlogBufPut(WUEPS_PID_RLC, (VOS_CHAR *)&stTtfMntnWrlcReset, sizeof(TTF_MNTN_ERR_LOG_WRLC_RESET_STRU));
}

/*****************************************************************************
 函 数 名  : WTTF_MNTN_ErrlogRlcLiErrEvt
 功能描述  : record Li error info
 输入参数  : VOS_UINT8 ucRbId                               - the Rb Id
             TTF_MNTN_ERR_LOG_LI_ERR_TYPE_ENUM8 ucLiErrType - the Li error type

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013.09.20
    作    者   : x59651
    修改内容   : create new function
*****************************************************************************/
VOS_VOID WTTF_MNTN_ErrlogRlcLiErrEvt(VOS_UINT8 ucRbId,
    TTF_MNTN_ERR_LOG_LI_ERR_TYPE_ENUM8 enLiErrType)
{
    VOS_UINT8                           ucAlmLev;
    MODEM_ID_ENUM_UINT16                enModemId;

    if (ucRbId > UTRAN_MAX_RB_ID)
    {
        PS_LOG1(WUEPS_PID_RLC, 0, PS_PRINT_WARNING,
            "WTTF_MNTN_ErrlogRlcLiErrEvt, ucRbId exceed the range.", ucRbId);
        return;
    }


    /* check need th record the Li error number */
    ucAlmLev    = TTF_ERR_LOG_GET_ALM_LEV(TTF_ERR_LOG_ALM_ID_WRLC_LI_ERR);
    enModemId   = VOS_GetModemIDFromPid(WUEPS_PID_RLC);


    if (!TTF_ERR_LOG_NEED_RPT_LEV(enModemId, ucAlmLev))
    {
        return;
    }

    if (0 == g_stErrLogLiErrCtx.ucLiErrRbCnt)
    {
        /* register the Li error callback function */
        TTF_MNTN_ErrlogCbReg(WUEPS_PID_RLC, WTTF_MNTN_ErrlogFlushRlcErrEvt);
    }

    if (0 == g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId].stLiErrInfo.ulLiErrCnt)
    {
        /* if ucLiErrRbCnt is full,but the rb is avaible, the g_stErrLogLiErrCtx is fail */
        if (g_stErrLogLiErrCtx.ucLiErrRbCnt >= UTRAN_MAX_RB_NUM)
        {
            PS_MEM_SET(&g_stErrLogLiErrCtx, 0 ,sizeof(TTF_MNTN_ERR_LOG_WRLC_LI_ERR_CTX_STRU));

            PS_LOG1(WUEPS_PID_RLC, 0, PS_PRINT_WARNING,
                "WTTF_MNTN_ErrlogRlcLiErrEvt, g_stErrLogLiErrCtx.ucLiErrRbCnt exceed the range.",
                g_stErrLogLiErrCtx.ucLiErrRbCnt);
            return;
        }

        g_stErrLogLiErrCtx.aucLiErrRbs[g_stErrLogLiErrCtx.ucLiErrRbCnt]   = ucRbId;
        g_stErrLogLiErrCtx.ucLiErrRbCnt ++;

        TTF_ERR_LOG_FILL_HEADER(
            &g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId].stHeader,
            enModemId,
            TTF_ERR_LOG_ALM_ID_WRLC_LI_ERR,
            ucAlmLev,
            (sizeof(TTF_MNTN_ERR_LOG_WRLC_LI_ERR_STRU) - sizeof(OM_ERR_LOG_HEADER_STRU)));

        g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId].stLiErrInfo.ulLstUpdtTs = 0;
        g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId].stLiErrInfo.ulLiErrCnt  = 1;
        g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId].stLiErrInfo.ucRbId      = ucRbId;

        TTF_ERR_LOG_ENT_UPDT_BUF_RSV_LEN(VOS_GetModemIDFromPid(WUEPS_PID_RLC), sizeof(TTF_MNTN_ERR_LOG_WRLC_LI_ERR_STRU));
    }
    else
    {
        g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId].stLiErrInfo.ulLstUpdtTs = VOS_GetSlice();
        g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId].stLiErrInfo.ulLiErrCnt ++;
    }

    g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId].stLiErrInfo.enType  = enLiErrType;

    return;
}

/*****************************************************************************
 函 数 名  : WTTF_MNTN_ErrlogFlushRlcErrEvt
 功能描述  : report Li error event
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013.09.20
    作    者   : x59651
    修改内容   : create new function

*****************************************************************************/
VOS_VOID WTTF_MNTN_ErrlogFlushRlcErrEvt(VOS_VOID)
{
    VOS_UINT8                           ucAlmLev;
    VOS_UINT8                           ucRbIdIdx;
    VOS_UINT8                           ucRbId;
    MODEM_ID_ENUM_UINT16                enModemId;

    ucAlmLev = TTF_ERR_LOG_GET_ALM_LEV(TTF_ERR_LOG_ALM_ID_WRLC_LI_ERR);
    enModemId   = VOS_GetModemIDFromPid(WUEPS_PID_RLC);

    if (!TTF_ERR_LOG_NEED_RPT_LEV(enModemId, ucAlmLev))
    {
        return;
    }

    for (ucRbIdIdx = 0; ucRbIdIdx < g_stErrLogLiErrCtx.ucLiErrRbCnt; ucRbIdIdx++)
    {
        ucRbId = g_stErrLogLiErrCtx.aucLiErrRbs[ucRbIdIdx];

        if (ucRbId > UTRAN_MAX_RB_ID)
        {
            PS_MEM_SET(&g_stErrLogLiErrCtx, 0 ,sizeof(TTF_MNTN_ERR_LOG_WRLC_LI_ERR_CTX_STRU));
            PS_LOG2(WUEPS_PID_RLC, 0, PS_PRINT_WARNING,
                "WTTF_MNTN_ErrlogFlushRlcErrEvt, the rbIdx <1> in g_stErrLogLiErrCtx.ucLiErrRbCnt is RbId <2>.",
                ucRbIdIdx, ucRbId);
            return;
        }
        /* 异常事件写入缓存中 */
        TTF_MNTN_ErrlogBufPut(WUEPS_PID_RLC, (VOS_CHAR *)&(g_stErrLogLiErrCtx.astWrlcLiErrInfo[ucRbId]),
            sizeof(TTF_MNTN_ERR_LOG_WRLC_LI_ERR_STRU));
    }

    PS_MEM_SET(&g_stErrLogLiErrCtx, 0 ,sizeof(TTF_MNTN_ERR_LOG_WRLC_LI_ERR_CTX_STRU));

    return;
}

/*****************************************************************************
 函 数 名  : WTTF_MNTN_ErrlogTfciFailEvt
 功能描述  : report TFC select fail event
 输入参数  : VOS_UINT8 ucMacState                               - MAC UL state
             TTF_MNTN_ERR_LOG_TFC_ERR_NO_CHOICE_ENUM8 enType    - TFC select fail type
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013.09.21
    作    者   : x59651
    修改内容   : create new function

*****************************************************************************/
VOS_VOID  WTTF_MNTN_ErrlogTfciFailEvt(VOS_UINT8 ucMacState,
    TTF_MNTN_ERR_LOG_TFC_ERR_NO_CHOICE_ENUM8 enType)
{
     VOS_UINT8                      ucAlmLev;
     WTTF_MNTN_TFCI_FAIL_STRU       stTtfMNtnTfciFail;
     MODEM_ID_ENUM_UINT16           enModemId;

     ucAlmLev       = TTF_ERR_LOG_GET_ALM_LEV(TTF_ERR_LOG_ALM_ID_WRLC_TFCI_FAIL);
     enModemId      = VOS_GetModemIDFromPid(WUEPS_PID_RLC);

     if (!TTF_ERR_LOG_NEED_RPT_LEV(enModemId, ucAlmLev))
     {
         return;
     }

     TTF_ERR_LOG_FILL_HEADER(
             &stTtfMNtnTfciFail.stHeader,
             enModemId,
             TTF_ERR_LOG_ALM_ID_WRLC_TFCI_FAIL,
             ucAlmLev,
             (sizeof(WTTF_MNTN_TFCI_FAIL_STRU) - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stTtfMNtnTfciFail.ucMacState    = ucMacState;
    stTtfMNtnTfciFail.enType        = enType;

    /* 异常事件写入缓存中 */
    TTF_MNTN_ErrlogBufPut(WUEPS_PID_MAC, (VOS_CHAR *)&stTtfMNtnTfciFail,
        sizeof(WTTF_MNTN_TFCI_FAIL_STRU));

    return;
}
VOS_VOID TTF_MNTN_ErrlogShow(VOS_VOID)
{
    VOS_UINT32 ulIndx;

    vos_printf("Errlog 实体状态:        %u \r\n", TTF_ERR_LOG_ENT_STATE_GET());

    for(ulIndx = 0; ulIndx < MODEM_ID_BUTT; ulIndx++)
    {
        vos_printf("\r\n******************MODEM ID %u*************************\r\n", ulIndx);
        vos_printf("Errlog Modem %d 开关状态:        %u \r\n", ulIndx, TTF_ERR_LOG_ENT_CTRL_STATUS_GET(ulIndx));
        vos_printf("Errlog Modem %d 当前上报级别:    %u \r\n", ulIndx, TTF_ERR_LOG_ENT_CTRL_LEV_GET(ulIndx));
        vos_printf("Errlog Modem %d RingBuff Mem Addr:   0x%x \r\n", ulIndx, TTF_ERR_LOG_ENT_BUF_GET(ulIndx));
        vos_printf("Errlog Modem %d RingBuff ID:         0x%x \r\n", ulIndx, TTF_ERR_LOG_ENT_RINGID_GET(ulIndx));
        vos_printf("Errlog Modem %d SemId:               0x%x \r\n", ulIndx, TTF_ERR_LOG_ENT_SEM_GET(ulIndx));
        vos_printf("Errlog Modem %d Buffer保留长度:      %u \r\n", ulIndx, TTF_ERR_LOG_ENT_GET_BUF_RSV_LEN(ulIndx));
        vos_printf("Errlog Modem %d Buffer中的数据长度:      %u \r\n", ulIndx, OM_RingBufferNBytes(TTF_ERR_LOG_ENT_RINGID_GET(ulIndx)));

        TTF_ERR_LOG_CB_SHOW(ulIndx, MODULE_ID_PDCP);
        TTF_ERR_LOG_CB_SHOW(ulIndx, MODULE_ID_WRM);
        TTF_ERR_LOG_CB_SHOW(ulIndx, MODULE_ID_GRM);
        TTF_ERR_LOG_CB_SHOW(ulIndx, MODULE_ID_LLC);
        TTF_ERR_LOG_CB_SHOW(ulIndx, MODULE_ID_SN);
        TTF_ERR_LOG_CB_SHOW(ulIndx, MODULE_ID_DL);
    }
}
/*lint -restore */

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

