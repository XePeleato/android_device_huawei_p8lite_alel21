/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AdsDebug.h
  版 本 号   : 初稿
  作    者   : S62952
  生成日期   : 2012年1月20日
  最近修改   :
  功能描述   : AdsTest.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年1月20日
    作    者   : S62952
    修改内容   : 创建文件

******************************************************************************/

#ifndef __ADSTEST_H__
#define __ADSTEST_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
/* Added by l60609 for AP适配项目 ，2012-08-30 Begin */
#include "product_config.h"
/* Added by l60609 for AP适配项目 ，2012-08-30 End */
#include "AdsCtx.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "msp_diag.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define ADS_LATENCY_LOG(ModuleId, String)   \
            ADS_LATENCY_OM_LOG(__FILE__, __LINE__, ModuleId, LOG_LEVEL_ERROR, String)

/* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
/*上行统计信息*/
#define  ADS_DBG_RECV_UL_PKT_NUM(i, n)              (g_stAdsStats.astAdsSpecStatsInfo[i].ulUlRecvPktNum += (n))
#define  ADS_DBG_UL_SAVE_BUFF_PKT_NUM(i, n)         (g_stAdsStats.astAdsSpecStatsInfo[i].ulUlSaveBuffPktNum += (n))
#define  ADS_DBG_UL_ENQUE_FAIL_NUM(i, n)            (g_stAdsStats.astAdsSpecStatsInfo[i].ulUlEnQueFail += (n))
#define  ADS_DBG_UL_CFG_FAIL_FREE_NUM(i, n)         (g_stAdsStats.astAdsSpecStatsInfo[i].ulUlCfgFailFreeNum += (n))
#define  ADS_DBG_UL_CURRT_BDCD_NUM(i, n)            (g_stAdsStats.astAdsSpecStatsInfo[i].ulUlCurrtBDCDNum += (n))

/*下行统计信息*/
#define  ADS_DBG_RECV_DL_PKT_NUM(i, n)              (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRecvPktNum += (n))
#define  ADS_DBG_SEND_DL_PKT_NUM(i, n)              (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlSendPktNum += (n))
#define  ADS_DBG_DL_IPF_ERR_PKT_NUM(i, n)           (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlIPFErrNum += (n))
#define  ADS_DBG_DL_IPV6_BIG_PKT_NUM(i, n)          (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlIpv6BigPktum += (n))
#define  ADS_DBG_DL_RECV_NDCLIENT_PKT_NUM(i, n)     (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRecvNDClintPktum += (n))
#define  ADS_DBG_DL_RABID_ERR_PKT_NUM(i, n)         (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRabIdErrPktum += (n))
#define  ADS_DBG_DL_RECV_DHCPV4V6_PKT_NUM(i, n)     (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRecvDHCPV4V6Pktum += (n))
#define  ADS_DBG_DL_RECV_IPV6_LINK_LOCAL_ADRESS_PKT_NUM(i, n)  (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRecvIPV6LinkLocalAddressPktum += (n))
/* 只在V3R3上使用 */
#define  ADS_DBG_DL_TRANS_FROM_IMMZC_ERR_NUM(n)     (g_stAdsStats.astAdsSpecStatsInfo[0].ulDlTransformImmZcErr += (n))
#define  ADS_DBG_DL_FREE_NUM(i, n)                  (g_stAdsStats.astAdsSpecStatsInfo[i].ulDlFreeNum += (n))

#define  ADS_DBG_UL_CFG_IPF_HAVE_NO_BDCD(n)         (g_stAdsStats.stUlComStatsInfo.ulUlCfgIpfHaveNoBDCD += (n))
#define  ADS_DBG_UL_SEND_BUFF_PKT_NUM(n)            (g_stAdsStats.stUlComStatsInfo.ulUlSendBuffPktNum += (n))
#define  ADS_DBG_UL_SEND_PKT_FAIL_NUM(n)            (g_stAdsStats.stUlComStatsInfo.ulUlIpfSendPktFailNum += (n))
#define  ADS_DBG_UL_PROC_EVENT_NUM(n)               (g_stAdsStats.stUlComStatsInfo.ulUlProcEventNum += (n))

#define  ADS_DBG_DL_RECV_IPF_BREAK_NUM(n)           (g_stAdsStats.stDlComStatsInfo.ulDlRecvIPFBreakNum += (n))
#define  ADS_DBG_DL_RECV_RD_ZERO_NUM(n)             (g_stAdsStats.stDlComStatsInfo.ulDlRecvRDZeroNum += (n))
#define  ADS_DBG_DL_RECV_RD_NUM(n)                  (g_stAdsStats.stDlComStatsInfo.ulDlRecvRDNum += (n))
#define  ADS_DBG_DL_PROC_EVENT_NUM(n)               (g_stAdsStats.stDlComStatsInfo.ulDlProcEventNum += (n))
#define  ADS_DBG_DL_PROC_RD_EVENT_NUM(n)            (g_stAdsStats.stDlComStatsInfo.ulDlRdEventNum += (n))
#define  ADS_DBG_DL_PROC_AD_EMPTY_EVENT_NUM(n)      (g_stAdsStats.stDlComStatsInfo.ulDlAdEmptyNum += (n))
#define  ADS_DBG_DL_PROC_OTHER_EVENT_NUM(n)         (g_stAdsStats.stDlComStatsInfo.ulDlOtherEvent += (n))
#define  ADS_DBG_DL_PROC_ALL_EVENT_NUM(n)           (g_stAdsStats.stDlComStatsInfo.ulDlAllEvent += (n))
#define  ADS_DBG_DL_PROC_EMPTY_EVENT_NUM(n)         (g_stAdsStats.stDlComStatsInfo.ulDlEmptyEvent += (n))

/* Added by l60609 for AP适配项目 ，2012-08-30 Begin */
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    /* 上行源内存释放统计 */
#define ADS_DBG_UL_IPF_SAVE_SRCMEM_FAIL_NUM(n)      (g_stAdsStats.stUlComStatsInfo.ulUlIpfSaveSrcMemFail  += (n))
#define ADS_DBG_UL_IPF_SAVE_SRCMEM_SUCC_NUM(n)      (g_stAdsStats.stUlComStatsInfo.ulUlIpfSaveSrcMemSucc  += (n))
#define ADS_DBG_UL_IPF_FREE_SRCMEM_NUM(n)           (g_stAdsStats.stUlComStatsInfo.ulUlIpfFreeSrcMemNum   += (n))
#define ADS_DBG_UL_IPF_FREE_SRCMEM_ERROR(n)         (g_stAdsStats.stUlComStatsInfo.ulUlIpfFreeSrcMemErr   += (n))
#define ADS_DBG_UL_DROPPED_PACKET_NUM(n)            (g_stAdsStats.stUlComStatsInfo.ulUlDroppedPacketNum   += (n))

#define ADS_DBG_UL_LEVEL_ONE_CNT(n)                  (g_stAdsStats.stUlComStatsInfo.ulLevelOneCnt   += (n))
#define ADS_DBG_UL_LEVEL_TWO_CNT(n)                  (g_stAdsStats.stUlComStatsInfo.ulLevelTwoCnt   += (n))
#define ADS_DBG_UL_LEVEL_THREE_CNT(n)                (g_stAdsStats.stUlComStatsInfo.ulLevelThreeCnt += (n))
#define ADS_DBG_UL_LEVEL_FOUR_CNT(n)                 (g_stAdsStats.stUlComStatsInfo.ulLevelFourCnt  += (n))

    /* 下行申请系统内存统计 */
#define ADS_DBG_DL_ALLOC_SYS_MEM_FAIL_NUM(n)        (g_stAdsStats.stDlComStatsInfo.ulDlAllocSysMemFailNum   += (n))
#define ADS_DBG_DL_ALLOC_SYS_MEM_SUCC_NUM(n)        (g_stAdsStats.stDlComStatsInfo.ulDlAllocSysMemSuccNum   += (n))

/* 下行AD统计  */
#define ADS_DBG_DL_GET_AD_FAIL_NUM(n)               (g_stAdsStats.stDlComStatsInfo.ulDlGetAdFailNum         += (n))
#define ADS_DBG_DL_CONFIG_AD_SUCC_NUM(i, n)         (g_stAdsStats.stDlComStatsInfo.aulDlConfigAdSuccNum[i]  += (n))
#define ADS_DBG_DL_CONFIG_AD_FAIL_NUM(i, n)         (g_stAdsStats.stDlComStatsInfo.aulDlConfigAdFailNum[i]  += (n))
#define ADS_DBG_DL_RECV_ADQ_EMPTY_BREAK_NUM(n)      (g_stAdsStats.stDlComStatsInfo.ulDlRecvAdqEmptyBreakNum += (n))
#define ADS_DBG_DL_PROC_ADQ_EMPTY_EVENT_NUM(n)      (g_stAdsStats.stDlComStatsInfo.ulDlProcAdqEmptyEventNum += (n))
#define ADS_DBG_DL_GET_ADQ_EMPTY_NUM(i, n)          (g_stAdsStats.stDlComStatsInfo.aulDlGetAdqEmptyNum[i]   += (n))
#define ADS_DBG_DL_ALLOC_AD_FAIL_NUM(i, n)          (g_stAdsStats.stDlComStatsInfo.aulDlAllocAdFailNum[i]   += (n))
#define ADS_DBG_DL_ALLOC_AD_SUCC_NUM(i, n)          (g_stAdsStats.stDlComStatsInfo.aulDlAllocAdSuccNum[i]   += (n))

#endif
/* Added by l60609 for AP适配项目 ，2012-08-30 End */

/* Modified by l60609 for DSDA Phase II, 2012-11-24, End */
#define  ADS_DBG_DL_RD_TI_PROTECT_EXPIRED_NUM(n)    (g_stAdsStats.stDlComStatsInfo.ulDlRdTiProtectExpiredNum += (n))
#define  ADS_DBG_DL_RD_PROTECT_EVENT_NUM(n)         (g_stAdsStats.stDlComStatsInfo.ulDlRdProctectEventNum += (n))
#define  ADS_DBG_DL_FC_ACTIVATE_NUM(n)              (g_stAdsStats.stDlComStatsInfo.ulDlFcTmrExpiredCnt += (n))
#define  ADS_DBG_DL_FC_TMR_EXP_NUM(n)               (g_stAdsStats.stDlComStatsInfo.ulDlFcActivateCnt += (n))

#define ADS_DBG_SET_SEM_INIT_FLAG(flag)              (g_stAdsStats.stResetStatsinfo.ulSemInitFlg = (flag))
#define ADS_UL_DBG_SAVE_BINARY_SEM_ID(sem_id)        (g_stAdsStats.stResetStatsinfo.hULBinarySemId = (sem_id))
#define ADS_DL_DBG_SAVE_BINARY_SEM_ID(sem_id)        (g_stAdsStats.stResetStatsinfo.hDLBinarySemId = (sem_id))
#define ADS_UL_DBG_CREATE_BINARY_SEM_FAIL_NUM(n)     (g_stAdsStats.stResetStatsinfo.ulULCreateBinarySemFailNum += (n))
#define ADS_DL_DBG_CREATE_BINARY_SEM_FAIL_NUM(n)     (g_stAdsStats.stResetStatsinfo.ulDLCreateBinarySemFailNum += (n))
#define ADS_UL_DBG_LOCK_BINARY_SEM_FAIL_NUM(n)       (g_stAdsStats.stResetStatsinfo.ulULLockBinarySemFailNum += (n))
#define ADS_DL_DBG_LOCK_BINARY_SEM_FAIL_NUM(n)       (g_stAdsStats.stResetStatsinfo.ulDLLockBinarySemFailNum += (n))
#define ADS_UL_DBG_SAVE_LAST_BIN_SEM_ERR_RSLT(rslt)  (g_stAdsStats.stResetStatsinfo.ulULLastBinarySemErrRslt = (rslt))
#define ADS_DL_DBG_SAVE_LAST_BIN_SEM_ERR_RSLT(rslt)  (g_stAdsStats.stResetStatsinfo.ulDLLastBinarySemErrRslt = (rslt))
#define ADS_UL_DBG_SAVE_CCPU_RESET_SUCCESS_NUM(n)    (g_stAdsStats.stResetStatsinfo.ulULResetSucessNum += (n))
#define ADS_DL_DBG_SAVE_CCPU_RESET_SUCCESS_NUM(n)    (g_stAdsStats.stResetStatsinfo.ulDLResetSucessNum += (n))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 枚举名    : ADS_FLOW_DEBUG_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : ADS流量上报Debug开关
*****************************************************************************/
enum ADS_FLOW_DEBUG_ENUM
{
    ADS_FLOW_DEBUG_OFF                  = 0,
    ADS_FLOW_DEBUG_DL_ON                = 1,
    ADS_FLOW_DEBUG_UL_ON                = 2,
    ADS_FLOW_DEBUG_ALL_ON               = 3,
    ADS_FLOW_DEBUG_BUTT
};

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
/*****************************************************************************
结构名    : ADS_SPEC_STATS_INFO_STRU
结构说明  : ADS每个实例专有的数据统计上下文

  1.日    期   : 2012年11月23日
    作    者   : 鲁琳/l60609
    修改内容   : DSDA Phase II: 定义ADS每个实例专有的数据统计上下文
*****************************************************************************/
typedef struct
{
    /*ADS上行统计信息*/
    VOS_UINT32              ulUlRecvPktNum;                                     /*ADS收到上行数据的个数*/
    VOS_UINT32              ulUlSaveBuffPktNum;                                 /*ADS上行缓存包个数*/
    VOS_UINT32              ulUlEnQueFail;                                      /*ADS上行入队失败次数*/
    VOS_UINT32              ulUlCfgFailFreeNum;                                 /*上行配置BD失败后，释放SDU个数*/
    VOS_UINT32              ulUlCurrtBDCDNum;                                   /*当前可配置BD/CD个数*/

    /*下行RD统计信息*/
    VOS_UINT32              ulDlRecvPktNum;                                     /*ADS收到下行数据的个数*/
    VOS_UINT32              ulDlSendPktNum;                                     /*ADS发生给APP数据的个数*/
    VOS_UINT32              ulDlIPFErrNum;                                      /*IPF过滤错误包个数*/
    VOS_UINT32              ulDlIpv6BigPktum;                                   /*获取到大于MTU的IPV6包次数*/
    VOS_UINT32              ulDlRabIdErrPktum;                                  /*RAB ID错误的数据包的个数*/
    VOS_UINT32              ulDlRecvNDClintPktum;                               /*需要转发给NDClient数据包的个数*/
    VOS_UINT32              ulDlRecvDHCPV4V6Pktum;                              /*需要转发给NDClient数据包的个数*/
    VOS_UINT32              ulDlRecvIPV6LinkLocalAddressPktum;                  /*需要转发给NDClient数据包的个数*/

    VOS_UINT32              ulDlTransformImmZcErr;                              /*需要转发给NDClient数据包的个数*/
    VOS_UINT32              ulDlFreeNum;                                        /*ADS错误释放C核内存的次数*/

    /* 流量统计信息 */
    VOS_UINT32              ulDLFlowInfo;                                       /*ADS下行流量Byte计数*/
    VOS_UINT32              ulDLStartSlice;                                     /*ADS下行流量统计开始时间*/
    VOS_UINT32              ulDLEndSlice;                                       /*ADS下行流量统计结束时间*/

    VOS_UINT32              ulULFlowInfo;                                       /*ADS上行流量Byte计数*/
    VOS_UINT32              ulULStartSlice;                                     /*ADS上行流量统计开始时间*/
    VOS_UINT32              ulULEndSlice;                                       /*ADS上行流量统计结束时间*/
}ADS_SPEC_STATS_INFO_STRU;


typedef struct
{
    VOS_UINT32              ulUlCfgIpfHaveNoBDCD;                               /*配置下行IPF没有获得BD/CD次数*/
    VOS_UINT32              ulUlSendBuffPktNum;                                 /*ADS发送上行缓存包数量*/
    VOS_UINT32              ulUlIpfSendPktFailNum;                              /*调用IPF发生失败的次数*/
    VOS_UINT32              ulUlProcEventNum;                                   /*ADS处理上行事件次数*/
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    VOS_UINT32              ulUlIpfSaveSrcMemFail;                              /* 上行IPF源内存队列不足导致内存*/
    VOS_UINT32              ulUlIpfSaveSrcMemSucc;                              /* 上行保存源内存个数 */
    VOS_UINT32              ulUlIpfFreeSrcMemNum;                               /* 上行释放源内存个数 */
    VOS_UINT32              ulUlIpfFreeSrcMemErr;                               /* 上行释放内存错误 */
    VOS_UINT32              ulUlDroppedPacketNum;                               /* 上行超长丢包个数 */

    VOS_UINT32              ulLevelOneCnt;
    VOS_UINT32              ulLevelTwoCnt;
    VOS_UINT32              ulLevelThreeCnt;
    VOS_UINT32              ulLevelFourCnt;

#endif
    /* 流量统计信息 */
    VOS_UINT32              ulULFlowDebugFlag;                                  /*上行流量Debug开关*/
    VOS_UINT32              ulULFlowRptThreshold;                               /*上行流量上报门限*/
}ADS_UL_COM_STATS_INFO_STRU;


typedef struct
{
    VOS_UINT32              ulDlRecvIPFBreakNum;                                /*下行收到IPF事件的个数*/
    VOS_UINT32              ulDlRecvRDNum;                                      /*下行获取RD的次数*/
    VOS_UINT32              ulDlRecvRDZeroNum;                                  /*下行获取RD为0的次数*/
    VOS_UINT32              ulDlProcEventNum;                                   /*ADS处理下行事件次数*/
    VOS_UINT32              ulDlRdEventNum;
    VOS_UINT32              ulDlAdEmptyNum;
    VOS_UINT32              ulDlOtherEvent;
    VOS_UINT32              ulDlAllEvent;
    VOS_UINT32              ulDlEmptyEvent;
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    VOS_UINT32              ulDlAllocSysMemSuccNum;                             /* ADS下行申请系统内存成功次数 */
    VOS_UINT32              ulDlAllocSysMemFailNum;                             /* ADS下行申请系统内存失败次数 */
    VOS_UINT32              ulDlGetAdFailNum;                                   /* ADS下行获取AD失败次数 */
    VOS_UINT32              aulDlConfigAdSuccNum[IPF_AD_MAX];                   /* ADS下行配置AD成功次数 */
    VOS_UINT32              aulDlConfigAdFailNum[IPF_AD_MAX];                   /* ADS下行配置AD失败次数 */
    VOS_UINT32              ulDlRecvAdqEmptyBreakNum;                           /* ADS下行空中断次数 */
    VOS_UINT32              ulDlProcAdqEmptyEventNum;                           /* ADS下行空中断事件次数 */
    VOS_UINT32              aulDlGetAdqEmptyNum[IPF_EMPTY_MAX];                 /* ADS下行ADQ为空的次数 */
    VOS_UINT32              aulDlAllocAdSuccNum[IPF_AD_MAX];                    /* ADS下行申请AD成功的次数 */
    VOS_UINT32              aulDlAllocAdFailNum[IPF_AD_MAX];                    /* ADS下行申请AD失败的次数 */
#endif
    VOS_UINT32              ulDlRdTiProtectExpiredNum;                          /*ADS发送下行RD保护定时器超时次数*/
    VOS_UINT32              ulDlRdProctectEventNum;                             /*ADS发送下行RD保护事件次数*/
    VOS_UINT32              ulDlFcTmrExpiredCnt;
    VOS_UINT32              ulDlFcActivateCnt;
    /* 流量统计信息 */
    VOS_UINT32              ulDLFlowDebugFlag;                                  /*下行流量Debug开关*/
    VOS_UINT32              ulDLFlowRptThreshold;                               /*下行流量上报门限*/
}ADS_DL_COM_STATS_INFO_STRU;
typedef struct
{
    VOS_SEM                 hULBinarySemId;                                     /* 上行二进制信号量ID */
    VOS_SEM                 hDLBinarySemId;                                     /* 下行二进制信号量ID */
    VOS_UINT32              ulSemInitFlg;                                       /* 初始化标识, VOS_TRUE: 成功; VOS_FALSE: 失败 */
    VOS_UINT32              ulULCreateBinarySemFailNum;                         /* 创建上行二进制信号量失败次数 */
    VOS_UINT32              ulDLCreateBinarySemFailNum;                         /* 创建下行二进制信号量失败次数 */
    VOS_UINT32              ulULLockBinarySemFailNum;                           /* 锁上行二进制信号量失败次数 */
    VOS_UINT32              ulDLLockBinarySemFailNum;                           /* 锁下行二进制信号量失败次数 */
    VOS_UINT32              ulULLastBinarySemErrRslt;                           /* 最后一次上行锁二进制信号量失败结果 */
    VOS_UINT32              ulDLLastBinarySemErrRslt;                           /* 最后一次下行锁二进制信号量失败结果 */
    VOS_UINT32              ulULResetSucessNum;                                 /* C核复位上行成功的次数 */
    VOS_UINT32              ulDLResetSucessNum;                                 /* C核复位下行成功的次数 */
}ADS_RESET_STATS_INFO_STRU;
typedef struct
{
    ADS_SPEC_STATS_INFO_STRU        astAdsSpecStatsInfo[ADS_INSTANCE_MAX_NUM];  /* 每个实例专有的统计信息 */
    ADS_UL_COM_STATS_INFO_STRU      stUlComStatsInfo;
    ADS_DL_COM_STATS_INFO_STRU      stDlComStatsInfo;
    ADS_RESET_STATS_INFO_STRU       stResetStatsinfo;

}ADS_STATS_INFO_STRU;
/* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

typedef struct
{
    VOS_UINT32                          ulAdType;
    VOS_UINT32                          ulAdNum;
    VOS_UINT32                          ulBeginTi;
    VOS_UINT32                          ulMiddleTi;
    VOS_UINT32                          ulEndTi;
}ADS_DL_DEBUG_AD_TIMER_STRU;

typedef struct
{
    VOS_UINT32                          ulBeginTi;
    VOS_UINT32                          ulEndTi;
}ADS_DL_DEBUG_RD_TIMER_STRU;

typedef struct
{
    VOS_UINT32                          ulBeginTi;
    VOS_UINT32                          ulEndTi;
}ADS_DL_DEBUG_TASK_TIMER_STRU;
typedef struct
{
    ADS_LOG_HEAD_STRU                   stLogHeader;
    ADS_STATS_INFO_STRU                 stAdsStats;                             /* ADS的统计信息 */
}ADS_LOG_RPT_STATS_INFO_STRU;

/*****************************************************************************
  8 全局变量声明
*****************************************************************************/

extern ADS_STATS_INFO_STRU                     g_stAdsStats;
extern VOS_UINT32                              g_ulAdsPrintUlDataFlg;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID ADS_LATENCY_OM_LOG(const VOS_CHAR  *pcFileName,  VOS_UINT32  ulLineNum,
                                   VOS_UINT32      ulModuleId,   VOS_UINT32 ulLevel,
                                   const VOS_CHAR  *pcString);

extern VOS_VOID ADS_MNTN_TraceRcvUlData(VOS_VOID);
extern VOS_VOID ADS_MNTN_TraceSndUlData(VOS_VOID);
extern VOS_VOID ADS_MNTN_TraceRcvDlData(VOS_VOID);

VOS_VOID ADS_ResetDebugInfo(VOS_VOID);

extern VOS_VOID ADS_DLFlowAdd(VOS_UINT8  ucIndex, VOS_UINT32  ulSduLen);
extern VOS_VOID ADS_ULFlowAdd(VOS_UINT8  ucIndex, VOS_UINT32  ulSduLen);
VOS_VOID ADS_SetFlowDebugFlag(VOS_UINT32  ulFlowDebugFlag);
VOS_VOID ADS_SetFlowDLRptThreshold(VOS_UINT32  ulFlowDLRptThreshold);
VOS_VOID ADS_SetFlowULRptThreshold(VOS_UINT32  ulFlowULRptThreshold);

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

#endif /* end of AdsTest.h */
