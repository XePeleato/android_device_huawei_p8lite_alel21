

#ifndef __TTFERRLOG_H__
#define __TTFERRLOG_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsCommonDef.h"
#include "pslog.h"
#include "NVIM_Interface.h"
#include "omnvinterface.h"
#include "omringbuffer.h"
#include "omerrorlog.h"
#include "TTFComm.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*  提供的SAPI个数，不支持TOM  */
#define LL_SAPI_NUMBER                              (6)
#define LL_INVALID_LLE_INDEX                        (6)
#define LL_MAX_SAPI_VALUE                           (11)

#define IS_WUEPS_PID_RABM(x)                        ((I0_WUEPS_PID_RABM == x) || (I1_WUEPS_PID_RABM == x))
#define IS_WUEPS_PID_PDCP(x)                        (WUEPS_PID_PDCP == x)
#define IS_WUEPS_PID_RLC(x)                         (WUEPS_PID_RLC == x)
#define IS_WUEPS_PID_MAC(x)                         (WUEPS_PID_MAC == x)
#define IS_UEPS_PID_SN(x)                           ((I0_UEPS_PID_SN == x) || (I1_UEPS_PID_SN == x))
#define IS_UEPS_PID_LL(x)                           ((I0_UEPS_PID_LL == x) || (I1_UEPS_PID_LL == x))
#define IS_UEPS_PID_GRM(x)                          ((I0_UEPS_PID_GRM == x) || (I1_UEPS_PID_GRM == x))
#define IS_UEPS_PID_DL(x)                           ((I0_UEPS_PID_DL == x) || (I1_UEPS_PID_DL == x))

#define TTF_ARRAY_SIZE(x)                           (sizeof(x) / sizeof(x[0]))

#if (FEATURE_PTM == FEATURE_OFF)
#define TTF_MNTN_InitErrLogEnt(ulPid)    (VOS_OK)
#define GTTF_MNTN_ErrlogTbfAbnmlEvt(ulPid, usType)
#define GTTF_MNTN_ErrlogLlCrcErrEvt(ucSapi, pFcsData)
#define GTTF_MNTN_ErrlogMdlErrEvt(ulPid, pstMdlErr)
#define TTF_MNTN_ErrlogTtfMemAllocFail(ulPid, ucPoolId, usFileId,  usLine,  ulAllocFailCnt)
#define WTTF_MNTN_ErrlogRlcResetEvt(ucRbId, enResetType)
#define WTTF_MNTN_ErrlogRlcLiErrEvt(ucRbId, enLiErrType)
#define WTTF_MNTN_ErrlogFlushRlcErrEvt()
#define WTTF_MNTN_ErrlogTfciFailEvt(ucMacState, enType)

#endif

#if (FEATURE_PTM == FEATURE_ON)
#define TTF_ERR_LOG_BUF_SIZE                        (2 * 1024)
#define TTF_ERR_SEM_TIMEOUT_LEN                     (5000)
#define TTF_ERR_LOG_RCD_MEM_CNT                     (20)

#define TTF_ERR_LOG_ENT_STATE_GET()                 (g_stTtfMntnErrlogEnt.ucState)
#define TTF_ERR_LOG_ENT_BUF_GET(modemId)            (g_stTtfMntnErrlogEnt.pRingMem[modemId])
#define TTF_ERR_LOG_ENT_RINGID_GET(modemId)         (g_stTtfMntnErrlogEnt.pRingId[modemId])
#define TTF_ERR_LOG_ENT_SEM_GET(modemId)            (g_stTtfMntnErrlogEnt.ulSaveLogSem[modemId])

#define TTF_ERR_LOG_ENT_STATE_SET(x)                (g_stTtfMntnErrlogEnt.ucState = (x))
#define TTF_ERR_LOG_ENT_BUF_SET(modemId, x)         (g_stTtfMntnErrlogEnt.pRingMem[modemId] = (x))
#define TTF_ERR_LOG_ENT_RINGID_SET(modemId, x)      (g_stTtfMntnErrlogEnt.pRingId[modemId] = (x))
#define TTF_ERR_LOG_ENT_SEM_SET(modemId, x)         (g_stTtfMntnErrlogEnt.ulSaveLogSem[modemId] = (x))

#define TTF_ERR_LOG_ENT_UPDT_BUF_RSV_LEN(modemId, x)    (g_stTtfMntnErrlogEnt.ulBufRsvLen[modemId] += (x))
#define TTF_ERR_LOG_ENT_RST_BUF_RSV_LEN(modemId)        (g_stTtfMntnErrlogEnt.ulBufRsvLen[modemId] = (0))
#define TTF_ERR_LOG_ENT_GET_BUF_RSV_LEN(modemId)     (g_stTtfMntnErrlogEnt.ulBufRsvLen[modemId])

#define TTF_ERR_LOG_NEED_RPT_LEV(enModemId, lev) \
        ((OM_APP_STATUS_OPEN == g_stTtfMntnErrlogEnt.stCtrlInfo[enModemId].ucAlmStatus) \
        && (lev <= g_stTtfMntnErrlogEnt.stCtrlInfo[enModemId].ucAlmLevel))

#define TTF_ERR_LOG_ENT_CTRL_STATUS_SET(enModemId, status) \
        (g_stTtfMntnErrlogEnt.stCtrlInfo[enModemId].ucAlmStatus = (status))

#define TTF_ERR_LOG_ENT_CTRL_LEV_SET(enModemId, lev) \
        (g_stTtfMntnErrlogEnt.stCtrlInfo[enModemId].ucAlmLevel = (lev))

#define TTF_ERR_LOG_ENT_CTRL_STATUS_GET(enModemId) \
        (g_stTtfMntnErrlogEnt.stCtrlInfo[enModemId].ucAlmStatus)

#define TTF_ERR_LOG_ENT_CTRL_LEV_GET(enModemId) \
        (g_stTtfMntnErrlogEnt.stCtrlInfo[enModemId].ucAlmLevel)



#define TTF_ERR_LOG_RPT_CB_SET(modemId, moduleId, cb)   (apErrLogRptCb[modemId][moduleId] = (cb))
#define TTF_ERR_LOG_RPT_CB_GET(modemId, moduleId)       (apErrLogRptCb[modemId][moduleId])

#define TTF_ERR_LOG_GET_ALM_LEV(AlmId)                  (g_aucTtfErrlogAlmLevTbl[AlmId])


    /* 0表示通信故障，TTF整个属于通信模块，只显示通信故障 */
#define TTF_ERR_LOG_ALM_TYPE_COMUNICATION             (0)

#define TTF_ERR_LOG_FILL_HEADER(pstHeader, ModemId, AlmId, AlmLev, ulLen) \
{ \
    (pstHeader)->ulMsgModuleId     = OM_ERR_LOG_MOUDLE_ID_GUL2; \
    (pstHeader)->usModemId         = ModemId; \
    (pstHeader)->usAlmId           = AlmId; \
    (pstHeader)->usAlmLevel        = AlmLev; \
    (pstHeader)->usAlmType         = TTF_ERR_LOG_ALM_TYPE_COMUNICATION; \
    (pstHeader)->usAlmLowSlice     = VOS_GetSlice(); \
    (pstHeader)->usAlmHighSlice    = 0; \
    (pstHeader)->ulAlmLength       = ulLen; \
}


#define TTF_ERR_LOG_GET_LLC_CRC_ERR_INFO(ucSapi)    &g_stErrlogLlcCrcErrCtx.astLlCrcErrInfo[ucSapi]
#define TTF_ERR_LOG_UPDT_LLC_CRC_ERR_SAPI(ucSapi) \
{\
    g_stErrlogLlcCrcErrCtx.aucCrcErrSapi[g_stErrlogLlcCrcErrCtx.ucCrcErrCnt] = ucSapi; \
    g_stErrlogLlcCrcErrCtx.ucCrcErrCnt++; \
}

#define TTF_ERR_LOG_GET_SAPI_CRC_ERR_CNT()          g_stErrlogLlcCrcErrCtx.ucCrcErrCnt
#define TTF_ERR_LOG_GET_SAPI_BY_INDX(ucIndx)        g_stErrlogLlcCrcErrCtx.aucCrcErrSapi[ucIndx]

#define TTF_ERR_LOG_RST_LLC_CRC_ERR_CTX_INFO()      PS_MEM_SET(&g_stErrlogLlcCrcErrCtx, 0, sizeof(TTF_MNTN_ERR_LOG_GLLC_CRC_ERR_CTX_STRU))

#define TTF_ERR_LOG_LL_IS_DUMMY_BLOCK(data)         (0 == ((data[0]^data[1]) | (data[1]^data[2])))

#define UTRAN_MAX_RB_ID                             (33)
#define UTRAN_MAX_RB_NUM                            (UTRAN_MAX_RB_ID + 1)

#define TTF_ERR_LOG_CB_SHOW(enModemId, ulModuleId) \
    vos_printf("Modem:%d            %s      cb:0x%x \r\n", enModemId, #ulModuleId, apErrLogRptCb[enModemId][ulModuleId])
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum TTF_MODULE_ID_ENUM
{
    MODULE_ID_PDCP = 0,
    MODULE_ID_WRM,
    MODULE_ID_GRM,
    MODULE_ID_LLC,
    MODULE_ID_SN,
    MODULE_ID_DL,
    MODULE_ID_BUTT
};

enum TTF_ERR_LOG_RPT_STATUS_ENUM
{
    TTF_ERR_LOG_RPT_CLOSE = 0,
    TTF_ERR_LOG_RPT_OPEN,
    TTF_ERR_LOG_RPT_BUTT
};

typedef VOS_UINT8 TTF_MODULE_ID_ENUM_UINT8;


enum TTF_ERR_LOG_ALM_ID_ENUM
{
    TTF_ERR_LOG_ALM_ID_WRLC_RESET = 1,
    TTF_ERR_LOG_ALM_ID_WRLC_LI_ERR,
    TTF_ERR_LOG_ALM_ID_WRLC_TFCI_FAIL,
    TTF_ERR_LOG_ALM_ID_TTF_MEM_FAIL,
    TTF_ERR_LOG_ALM_ID_GPRS_TBF_ABNML,
    TTF_ERR_LOG_ALM_ID_GLLC_CRC_ERR,
    TTF_ERR_LOG_ALM_ID_GSM_MDL_ERR,
    TTF_ERR_LOG_ALM_ID_BUTT
};

typedef VOS_UINT8 TTF_ERR_LOG_ALM_ID_ENUM_UINT8;

enum TTF_ERR_LOG_ALM_LEV_ENUM
{
    TTF_ERR_LOG_ALM_LEV_CRITICAL = 0,
    TTF_ERR_LOG_ALM_LEV_MAJOR,
    TTF_ERR_LOG_ALM_LEV_MINOR,
    TTF_ERR_LOG_ALM_LEV_WARNING,
    TTF_ERR_LOG_ALM_LEV_BUTT,
};

typedef VOS_UINT8 TTF_ERR_LOG_ALM_LEV_ENUM_UINT8;

enum TTF_MNTN_ERR_LOG_WRLC_RESET_TYPE
{
    TTF_MNTN_ERR_LOG_WRLC_RESET_RCV_ERR_STATUS_REPORT,
    TTF_MNTN_ERR_LOG_WRLC_RESET_RCV_RESET_PDU,
    TTF_MNTN_ERR_LOG_WRLC_RESET_TX_DATA_REACH_MST,
    TTF_MNTN_ERR_LOG_WRLC_RESET_TX_MRW_REACH_MST,
    TTF_MNTN_ERR_LOG_WRLC_RESET_BUTT
};
typedef VOS_UINT8 TTF_MNTN_ERR_LOG_WRLC_RESET_TYPE_ENUM_UINT8;

enum TTF_MNTN_ERR_LOG_LI_ERR_TYPE
{
    TTF_MNTN_ERR_LOG_LI_1ST,                /* judge from Li, the Sdu should be the the 1st Sdu in the Pdu,but actually not */
    TTF_MNTN_ERR_LOG_LI_LAST,               /* judge from Li, the Sdu should be the the last Sdu in the Pdu,but actually not */
    TTF_MNTN_ERR_LOG_LI_HEAD,               /* judge from Li, the Sdu should be the the lst or 2nd Sdu in the Pdu,but actually not */
    TTF_MNTN_ERR_LOG_LI_RSV,                /* use the Li not support */
    TTF_MNTN_ERR_LOG_LI_TOO_SHORT,
    TTF_MNTN_ERR_LOG_LI_TOO_LONG,
    TTF_MNTN_ERR_LOG_LI_OFFSET_ERR,         /* pre Li > this Li */
    TTF_MNTN_ERR_LOG_LI_PADDING,            /* padding LI but not the last LI */
    TTF_MNTN_ERR_LOG_LI_BUTT
};
typedef VOS_UINT8  TTF_MNTN_ERR_LOG_LI_ERR_TYPE_ENUM8;

enum TTF_MNTN_ERR_LOG_TFC_ERR_TYPE
{
    TTF_MNTN_ERR_LOG_TFC_ERR_NO_CHOICE,
    TTF_MNTN_ERR_LOG_TFC_ERR_NO_LOCH_LABEL,
    TTF_MNTN_ERR_LOG_TFC_ERR_BUTT
};
typedef VOS_UINT8 TTF_MNTN_ERR_LOG_TFC_ERR_NO_CHOICE_ENUM8;


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct
{
    VOS_UINT8                    ucState;
    VOS_UINT8                    aucRsv[3];
    NV_ID_ERR_LOG_CTRL_INFO_STRU stCtrlInfo[MODEM_ID_BUTT];
    OM_RING_ID                   pRingId[MODEM_ID_BUTT];
    VOS_UINT32                   ulBufRsvLen[MODEM_ID_BUTT];
    VOS_SEM                      ulSaveLogSem[MODEM_ID_BUTT];
    VOS_UINT8                   *pRingMem[MODEM_ID_BUTT];
}TTF_MNTN_ERR_LOG_ENT_STRU;

typedef struct
{
    VOS_UINT16              usAllocFileId;
    VOS_UINT16              usAllocLine;
    VOS_UINT16              usTraceFileId;
    VOS_UINT16              usTraceLine;
}TTF_MNTN_ERR_LOG_MEM_ALLOC_INFO_STRU;

typedef struct
{
    OM_ERR_LOG_HEADER_STRU  stHeader;
    VOS_UINT8               ucPoolId;
    VOS_UINT8               aucRsv[3];
    VOS_UINT16              usFileId;
    VOS_UINT16              usLine;
    VOS_UINT32              ulAllocFailCnt;
    TTF_MNTN_ERR_LOG_MEM_ALLOC_INFO_STRU astAllocInfo[TTF_ERR_LOG_RCD_MEM_CNT];
}TTF_MNTN_ERR_LOG_MEM_ALLOC_FAIL_STRU;

typedef struct
{
    OM_ERR_LOG_HEADER_STRU  stHeader;
    VOS_UINT16              usAbnmlType;
    VOS_UINT8               aucRsv[2];
}TTF_MNTN_ERR_LOG_GRLC_TBF_ABNML_STRU;

typedef struct
{
    VOS_UINT8               ucSapi;
    VOS_UINT8               aucRsv[3];
    VOS_UINT32              ulLstUpdtTs;
    VOS_UINT32              ulCrcErrCnt;
}TTF_MNTN_GLLC_CRC_ERR_INFO_STRU;

typedef struct
{
    OM_ERR_LOG_HEADER_STRU  stHeader;
    TTF_MNTN_GLLC_CRC_ERR_INFO_STRU stLlcCrcErrInfo;
}TTF_MNTN_ERR_LOG_GLLC_CRC_ERR_STRU;

typedef struct
{
    VOS_UINT8               ucSapi;
    VOS_UINT8               ucChanType;
    VOS_UINT8               ucErrCause;
    VOS_UINT8               ucRsv;
}TTF_MNTN_GDL_MDL_ERR_INFO_STRU;

typedef struct
{
    OM_ERR_LOG_HEADER_STRU          stHeader;
    TTF_MNTN_GDL_MDL_ERR_INFO_STRU  stDlMdlErrInfo;
}TTF_MNTN_ERR_LOG_GDL_MDL_ERR_STRU;

typedef struct
{
    VOS_UINT8                           aucCrcErrSapi[LL_SAPI_NUMBER];
    VOS_UINT8                           aucRsv1[2];
    TTF_MNTN_ERR_LOG_GLLC_CRC_ERR_STRU  astLlCrcErrInfo[LL_SAPI_NUMBER];
    VOS_UINT8                           ucCrcErrCnt;
    VOS_UINT8                           aucRsv2[3];
}TTF_MNTN_ERR_LOG_GLLC_CRC_ERR_CTX_STRU;


typedef VOS_VOID (*TTF_MNTN_ERR_LOG_RPT_CB)(VOS_VOID);

typedef struct
{
    OM_ERR_LOG_HEADER_STRU                          stHeader;
    TTF_MNTN_ERR_LOG_WRLC_RESET_TYPE_ENUM_UINT8     enResetType;
    VOS_UINT8                                       ucRbId;
    VOS_UINT16                                      usResetCnt;
}TTF_MNTN_ERR_LOG_WRLC_RESET_STRU;

typedef struct
{
    VOS_UINT32                                      ulLstUpdtTs;
    VOS_UINT8                                       ucRbId;
    TTF_MNTN_ERR_LOG_LI_ERR_TYPE_ENUM8              enType;
    VOS_UINT8                                       aucRsv[2];
    VOS_UINT32                                      ulLiErrCnt;
}TTF_MNTN_WRLC_LI_ERR_INFO_STRU;

typedef struct
{
    OM_ERR_LOG_HEADER_STRU          stHeader;
    TTF_MNTN_WRLC_LI_ERR_INFO_STRU  stLiErrInfo;
}TTF_MNTN_ERR_LOG_WRLC_LI_ERR_STRU;

typedef struct
{
    TTF_MNTN_ERR_LOG_WRLC_LI_ERR_STRU   astWrlcLiErrInfo[UTRAN_MAX_RB_NUM];
    VOS_UINT8                           aucLiErrRbs[UTRAN_MAX_RB_NUM];
    VOS_UINT8                           ucLiErrRbCnt;
    VOS_UINT8                           aucRsv;
}TTF_MNTN_ERR_LOG_WRLC_LI_ERR_CTX_STRU;

typedef struct
{
    OM_ERR_LOG_HEADER_STRU                      stHeader;
    VOS_UINT8                                   ucMacState;
    TTF_MNTN_ERR_LOG_TFC_ERR_NO_CHOICE_ENUM8    enType;
    VOS_UINT8                                   aucRsv[2];
}WTTF_MNTN_TFCI_FAIL_STRU;

typedef struct
{
    VOS_UINT32                                  ulPid;
    VOS_UINT32                                  ulModuleId;
}TTF_MNTN_ERR_LOG_PID_MODULE_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_UINT32 TTF_MNTN_InitErrLogEnt(VOS_UINT32 ulPid);

extern VOS_VOID   TTF_MNTN_ErrlogCbReg(VOS_UINT32 ulPid, TTF_MNTN_ERR_LOG_RPT_CB pErrlogCb);

extern VOS_VOID   TTF_MNTN_ErrlogCbRun(MODEM_ID_ENUM_UINT16 enModemId);

extern VOS_VOID   TTF_MNTN_ErrlogBufPut(VOS_UINT32 ulPid,  VOS_CHAR *pBuffer, VOS_UINT32 ulBytes);

extern VOS_UINT32 TTF_MNTN_ErrLogRcvCtrlInd(MsgBlock *pstMsg, void *p);

extern VOS_UINT32 TTF_MNTN_ErrLogRcvRptReq(MsgBlock *pstMsg, void *p);

extern VOS_VOID   TTF_MNTN_ErrlogBufGet(VOS_UINT32 ulPid, MODEM_ID_ENUM_UINT16  enModemId, VOS_CHAR *pBuffer, VOS_UINT32 ulBytes);

extern VOS_VOID   GTTF_MNTN_ErrlogLlCrcErrEvt(VOS_UINT8 ucSapi, VOS_UINT8 *pData);

extern VOS_VOID   GTTF_MNTN_ErrlogMdlErrEvt(VOS_UINT32 ulPid, VOS_VOID *pMdlErr);

extern VOS_VOID   TTF_MNTN_ErrlogTtfMemAllocFail(VOS_UINT32 ulPid, VOS_UINT8 ucPoolId, VOS_UINT16 usFileId, VOS_UINT16 usLine, VOS_UINT32 ulAllocFailCnt);

extern VOS_VOID   GTTF_MNTN_ErrlogTbfAbnmlEvt(VOS_UINT32 ulPid, VOS_UINT16 usType);

extern VOS_VOID WTTF_MNTN_ErrlogRlcResetEvt(VOS_UINT8 ucRbId,
        TTF_MNTN_ERR_LOG_WRLC_RESET_TYPE_ENUM_UINT8 enResetType);
extern VOS_VOID WTTF_MNTN_ErrlogRlcLiErrEvt(VOS_UINT8 ucRbId,
    TTF_MNTN_ERR_LOG_LI_ERR_TYPE_ENUM8 enLiErrType);
extern VOS_VOID WTTF_MNTN_ErrlogFlushRlcErrEvt(VOS_VOID);
extern VOS_VOID  WTTF_MNTN_ErrlogTfciFailEvt(VOS_UINT8 ucMacState,
        TTF_MNTN_ERR_LOG_TFC_ERR_NO_CHOICE_ENUM8 enType);


extern VOS_UINT8                    g_aucTtfErrlogAlmLevTbl[TTF_ERR_LOG_ALM_ID_BUTT];
extern TTF_MNTN_ERR_LOG_ENT_STRU    g_stTtfMntnErrlogEnt;

#endif

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TtfErrlog.h */

