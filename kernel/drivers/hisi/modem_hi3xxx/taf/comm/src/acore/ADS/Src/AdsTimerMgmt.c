/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AdsTimer.c
  版 本 号   : 初稿
  作    者   : 鲁琳/l60609
  生成日期   : 2011年12月7日
  最近修改   :
  功能描述   : ADS的定时器文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年12月7日
    作    者   : 鲁琳/l60609
    修改内容   : 创建文件

******************************************************************************/
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "AdsTimerMgmt.h"
#include "AdsCtx.h"
#include "AdsDownLink.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define    THIS_FILE_ID                 PS_FILE_ID_ADS_TIMERMGMT_C
/*lint +e767*/

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

ADS_TIMER_PRECISION_STRU                g_astAdsTmrPrecisionTbl[] =
{
    { TI_ADS_UL_SEND,                   VOS_TIMER_PRECISION_5  },
    { TI_ADS_DSFLOW_STATS_0,            VOS_TIMER_NO_PRECISION },
    { TI_ADS_DSFLOW_STATS_1,            VOS_TIMER_NO_PRECISION },
    { TI_ADS_DL_ADQ_EMPTY,              VOS_TIMER_PRECISION_0  },
    { TI_ADS_DL_PROTECT,                VOS_TIMER_PRECISION_5  },
    { TI_ADS_RPT_STATS_INFO,            VOS_TIMER_NO_PRECISION },
    { TI_ADS_UL_DATA_STAT,              VOS_TIMER_PRECISION_5 }
};


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID  ADS_MNTN_TraceTimerOperation(
    VOS_UINT32                          ulPid,
    ADS_TIMER_ID_ENUM_UINT32            enTimerId,
    VOS_UINT32                          ulTimerLen,
    ADS_TIMER_OPERATION_TYPE_ENUM_UINT8 enTimerAction,
    ADS_TIMER_STOP_CAUSE_ENUM_UINT8     enStopCause
)
{
    ADS_TIMER_INFO_STRU                 stMsg;

    PS_MEM_SET(&stMsg, 0x00, sizeof(ADS_TIMER_INFO_STRU));

    stMsg.ulSenderCpuId     = VOS_LOCAL_CPUID;
    stMsg.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    stMsg.ulSenderPid       = ulPid;
    stMsg.ulReceiverPid     = VOS_PID_TIMER;
    stMsg.ulLength          = sizeof(ADS_TIMER_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    stMsg.enTimerId         = enTimerId;
    stMsg.ulTimerLen        = ulTimerLen;
    stMsg.enTimerAction     = enTimerAction;
    stMsg.enTimerStopCause  = enStopCause;

    OM_AcpuTraceMsgHook(&stMsg);

    return;
}


VOS_TIMER_PRECISION_ENUM_UINT32 ADS_GetTimerPrecision(ADS_TIMER_ID_ENUM_UINT32 enTimerId)
{
    VOS_UINT32                          i;

    for (i = 0; i < ADS_ARRAY_SIZE(g_astAdsTmrPrecisionTbl); i++)
    {
        if (enTimerId == g_astAdsTmrPrecisionTbl[i].enTimerId)
        {
            return g_astAdsTmrPrecisionTbl[i].enPrecision;
        }
    }

    return VOS_TIMER_PRECISION_5;
}

/* Added by l60609 for DSDA Phase II, 2012-12-20, Begin */
#if (FEATURE_OFF == FEATURE_SKB_EXP)
VOS_VOID ADS_DL_StartAdqEmptyTimer(VOS_VOID)
{
    ADS_TIMER_CTX_STRU                 *pstTiCtx;
    VOS_TIMER_PRECISION_ENUM_UINT32     enTmrPrecision;
    VOS_UINT32                          ulRslt;

    pstTiCtx = ADS_GetTiCtx();

    /* 如果该定时器已经启动则直接返回 */
    if (ADS_TIMER_STATUS_RUNNING == pstTiCtx[TI_ADS_DL_ADQ_EMPTY].enTimerStatus)
    {
        return;
    }

    enTmrPrecision = ADS_GetTimerPrecision(TI_ADS_DL_ADQ_EMPTY);

    ulRslt = VOS_StartCallBackRelTimer(&(pstTiCtx[TI_ADS_DL_ADQ_EMPTY].hTimer),
                                       ACPU_PID_ADS_DL,
                                       TI_ADS_DL_ADQ_EMPTY_LEN,
                                       TI_ADS_DL_ADQ_EMPTY,
                                       0,
                                       VOS_RELTIMER_NOLOOP,
                                       ADS_DL_RcvTiAdqEmptyExpired,
                                       enTmrPrecision);

    if (VOS_OK != ulRslt)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_StartAdqEmptyTimer:timer is fail start!");
        return;
    }

    pstTiCtx[TI_ADS_DL_ADQ_EMPTY].enTimerStatus = ADS_TIMER_STATUS_RUNNING;

    return;
}
#endif
/* Added by l60609 for DSDA Phase II, 2012-12-20, End */


VOS_VOID ADS_DL_StartProtectTimer(VOS_VOID)
{
    ADS_TIMER_CTX_STRU                 *pstTiCtx;
    VOS_TIMER_PRECISION_ENUM_UINT32     enTmrPrecision;
    VOS_UINT32                          ulRslt;

    pstTiCtx = ADS_GetTiCtx();

    /* 如果该定时器已经启动则直接返回 */
    if (ADS_TIMER_STATUS_RUNNING == pstTiCtx[TI_ADS_DL_PROTECT].enTimerStatus)
    {
        return;
    }

    enTmrPrecision = ADS_GetTimerPrecision(TI_ADS_DL_PROTECT);

    ulRslt = VOS_StartCallBackRelTimer(&(pstTiCtx[TI_ADS_DL_PROTECT].hTimer),
                                       ACPU_PID_ADS_DL,
                                       TI_ADS_DL_PROTECT_LEN,
                                       TI_ADS_DL_PROTECT,
                                       0,
                                       VOS_RELTIMER_NOLOOP,
                                       ADS_DL_RcvTiProtectExpired,
                                       enTmrPrecision);

    if (VOS_OK != ulRslt)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_StartProtectTimer:timer is fail start!");
        return;
    }

    pstTiCtx[TI_ADS_DL_PROTECT].enTimerStatus = ADS_TIMER_STATUS_RUNNING;

    return;
}
VOS_VOID  ADS_StartTimer(
    VOS_UINT32                          ulPid,
    ADS_TIMER_ID_ENUM_UINT32            enTimerId,
    VOS_UINT32                          ulLen
)
{
    VOS_UINT32                          ulRet;
    ADS_TIMER_CTX_STRU                 *pstTiCtx;
    VOS_TIMER_PRECISION_ENUM_UINT32     enTmrPrecision;

    /* 输入参数检查 */
    if (0 == ulLen)
    {
        ADS_ERROR_LOG1(ACPU_PID_ADS_UL, "ADS_StartTimer:ulLen is",ulLen);
        return;
    }

    if (ulLen >= VOS_TIMER_MAX_LENGTH)
    {
        ulLen = VOS_TIMER_MAX_LENGTH - 1;
    }

    /* 不在使用的定时器范围内 */
    if (enTimerId >= ADS_MAX_TIMER_NUM)
    {
        return;
    }

    pstTiCtx = ADS_GetTiCtx();

    if (ADS_TIMER_STATUS_RUNNING == pstTiCtx[enTimerId].enTimerStatus)
    {
        return;
    }

    enTmrPrecision = ADS_GetTimerPrecision(enTimerId);

    ulRet = VOS_StartRelTimer(&(pstTiCtx[enTimerId].hTimer),
                              ulPid,
                              ulLen,
                              enTimerId,
                              0,
                              VOS_RELTIMER_NOLOOP,
                              enTmrPrecision);

    if (VOS_OK != ulRet)
    {
        ADS_ERROR_LOG(ulPid, "ADS_StartTimer:timer is fail start!");
        return;
    }

    pstTiCtx[enTimerId].enTimerStatus = ADS_TIMER_STATUS_RUNNING;

    /*勾包ADS_TIMER_INFO_STRU*/
    ADS_MNTN_TraceTimerOperation(ulPid, enTimerId, ulLen, ADS_TIMER_OPERATION_START, ADS_TIMER_STOP_CAUSE_ENUM_BUTT);

    return;
}

/*****************************************************************************
 函 数 名  : ADS_StopTimer
 功能描述  : ADS停止定时器
 输入参数  : VOS_UINT32                          ulPid
             ADS_TIMER_ID_ENUM_UINT32            enTimerId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月7日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_StopTimer(
    VOS_UINT32                          ulPid,
    ADS_TIMER_ID_ENUM_UINT32            enTimerId,
    ADS_TIMER_STOP_CAUSE_ENUM_UINT8     enStopCause
)
{
    ADS_TIMER_CTX_STRU                 *pstTiCtx;

    pstTiCtx = ADS_GetTiCtx();

    /* 不在使用的定时器范围内 */
    if (enTimerId >= ADS_MAX_TIMER_NUM)
    {
        return;
    }

    if (ADS_TIMER_STATUS_RUNNING != pstTiCtx[enTimerId].enTimerStatus)
    {
        return;
    }

    /* 停止VOS定时器: 当定时器的指针已经为空的时候, 说明其已经停止或者超时 */
    if (VOS_NULL_PTR != pstTiCtx[enTimerId].hTimer)
    {
        VOS_StopRelTimer(&(pstTiCtx[enTimerId].hTimer));
    }

    pstTiCtx[enTimerId].hTimer        = VOS_NULL_PTR;
    pstTiCtx[enTimerId].enTimerStatus = ADS_TIMER_STATUS_STOP;

    /*勾包ADS_TIMER_INFO_STRU*/
    ADS_MNTN_TraceTimerOperation(ulPid, enTimerId, 0, ADS_TIMER_OPERATION_STOP, enStopCause);

    return;
}
ADS_TIMER_STATUS_ENUM_UINT8 ADS_GetTimerStatus(
    VOS_UINT32                          ulPid,
    ADS_TIMER_ID_ENUM_UINT32            enTimerId
)
{
    ADS_TIMER_CTX_STRU                 *pstTiCtx;
    ADS_TIMER_STATUS_ENUM_UINT8         enTimerStatus;

    pstTiCtx  = ADS_GetTiCtx();

    enTimerStatus = ADS_TIMER_STATUS_STOP;

    if (enTimerId < ADS_MAX_TIMER_NUM)
    {
        enTimerStatus = pstTiCtx[enTimerId].enTimerStatus;
    }

    return enTimerStatus;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
