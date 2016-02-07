/******************************************************************************

                  版权所有 (C), 2006, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AdsDebug.c
  版 本 号   : 初稿
  作    者   : s62952
  生成日期   : 2012年01月12日
  最近修改   :

  功能描述   : ADS调试信息
  函数列表   :

  修改历史   :
  1.日    期   : 2012年01月12日
    作    者   : s62952
    修改内容   : 创建文件
******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define    THIS_FILE_ID                 PS_FILE_ID_ADS_DEBUG_C
/*lint +e767*/


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "AdsDebug.h"
#include "AdsCtx.h"
#include "vos.h"
#include "v_timer.h"

#define ADS_FLOW_DL_DEFAULT_RPT_THRESHOLD   (5000000)
#define ADS_FLOW_UL_DEFAULT_RPT_THRESHOLD   (500000)

/*****************************************************************************
  2 外部函数声明
*****************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
******************************************************************************/
ADS_STATS_INFO_STRU                     g_stAdsStats;
VOS_UINT32                              g_ulAdsLatencySwitch = 0;
VOS_UINT32                              g_ulAdsDebugFlg = VOS_FALSE;

HTIMER                                  g_pDebugRdhTimer;
VOS_UINT32                              g_ulDebugCurRdIntNum;
VOS_UINT32                              g_ulDebugCurRdNum;
RCV_DL_DATA_FUNC                        g_pDebugRcvFunc = VOS_NULL_PTR;

VOS_UINT32                              g_ulAdsPrintUlDataFlg = VOS_FALSE;

/******************************************************************************
   5 函数实现
******************************************************************************/
VOS_VOID ADS_SetPrintUlDataFlg(VOS_UINT32 ulFlg)
{
    g_ulAdsPrintUlDataFlg = ulFlg;
}

VOS_VOID ADS_Debug(VOS_VOID)
{
    vos_printf("ADS_Debug 5\r\n");
}
VOS_VOID ADS_RecordRcvFunc(VOS_UINT8 ucRabId)
{
    /* 记录下行接收函数 */
    g_pDebugRcvFunc = ADS_DL_GET_DATA_CALLBACK_FUNC(0, ucRabId);
    vos_printf("ADS_RecordRcvFunc:g_TestRcvFunc is %d\r\n", g_pDebugRcvFunc);
}

VOS_VOID ADS_ResumeRcvFunc(VOS_UINT8 ucRabId)
{
    vos_printf("ADS_ResumeRcvFunc:\r\n");
    ADS_DL_RegDlDataCallback(ucRabId, g_pDebugRcvFunc);
}
VOS_VOID ADS_SetEmptyRcvFunc(VOS_UINT8 ucRabId)
{
    vos_printf("ADS_SetEmptyRcvFunc:\r\n");
    ADS_DL_RegDlDataCallback(ucRabId, VOS_NULL_PTR);
}

VOS_VOID ADS_Debug_DL_RdNum(VOS_VOID)
{
    VOS_UINT32                          ulRd;
    ulRd = mdrv_ipf_get_dlrd_num();

    vos_printf("ADS_Debug_DL_RdNum: ulRd is %d\r\n", ulRd);
}

VOS_VOID ADS_Debug_DL_ProtectedTi(VOS_VOID)
{
    vos_printf("ADS_Debug_DL_ProtectedTi\r\n");
    ADS_DL_StartProtectTimer();
}

VOS_VOID ADS_Debug_RcvTiRDIntEventExpired(
    VOS_UINT32                          ulParam,
    VOS_UINT32                          ulTimerName
)
{
    VOS_UINT32                          ulNum;
    ulNum = g_stAdsStats.stDlComStatsInfo.ulDlRecvIPFBreakNum - g_ulDebugCurRdIntNum;
    vos_printf("ADS_DL_RcvTiRDIntEventExpired rd INT num is %d\r\n", ulNum);
    ulNum = g_stAdsStats.stDlComStatsInfo.ulDlRecvRDNum - g_ulDebugCurRdNum;
    vos_printf("ADS_DL_RcvTiRDIntEventExpired rd num is %d\r\n", ulNum);
}
VOS_VOID ADS_Debug_RD_Int_Event(VOS_UINT32 ulTiLen)
{
    g_ulDebugCurRdIntNum = g_stAdsStats.stDlComStatsInfo.ulDlRecvIPFBreakNum;
    g_ulDebugCurRdNum    = g_stAdsStats.stDlComStatsInfo.ulDlRecvRDNum;

    VOS_StartCallBackRelTimer(&g_pDebugRdhTimer,
                              ACPU_PID_ADS_DL,
                              ulTiLen,
                              TI_ADS_TIMER_BUTT,
                              0,
                              VOS_RELTIMER_NOLOOP,
                              ADS_Debug_RcvTiRDIntEventExpired,
                              VOS_TIMER_NO_PRECISION);
}

VOS_VOID ADS_ShowPeriodPktNum(VOS_VOID)
{
    vos_printf("ADS_ShowPeriodPktNum Cur DL PeriodPktNum is %d\r\n", g_stAdsCtx.astAdsSpecCtx[0].stAdsStatsInfoCtx.stDLDataStats.ulDLCurDataRate);
    vos_printf("ADS_ShowPeriodPktNum Cur UL PeriodPktNum is %d\r\n", g_stAdsCtx.astAdsSpecCtx[0].stAdsStatsInfoCtx.stULDataStats.ulULCurDataRate);
}
VOS_VOID ADS_ShowEntityStats(VOS_VOID)
{
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                           i;
    VOS_UINT8                           j;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++)
    {
        vos_printf("ADS Modem i 上行允许发送标识                             %d\n", i);

        for (j = ADS_RAB_ID_MIN; j <= ADS_RAB_ID_MAX; j++)
        {
            vos_printf("ADS RabId_%d, 上行允许发送标识 %d\n", j, g_stAdsCtx.astAdsSpecCtx[i].stAdsUlCtx.astAdsUlQueue[j].ucRabSndPermitFlg);

            if (VOS_NULL_PTR != g_stAdsCtx.astAdsSpecCtx[i].stAdsUlCtx.astAdsUlQueue[j].pstAdsUlLink)
            {
                vos_printf("ADS Queue length is %d\n", IMM_ZcQueueLen(g_stAdsCtx.astAdsSpecCtx[i].stAdsUlCtx.astAdsUlQueue[j].pstAdsUlLink));
            }

            vos_printf("ADS DL RabId is %d\r\n", g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.astAdsDlRabInfo[j - ADS_RAB_ID_OFFSET].ucRabId);
            vos_printf("ADS DL Rcv Func is 0x%x\r\n", g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.astAdsDlRabInfo[j - ADS_RAB_ID_OFFSET].pRcvDlDataFunc);
        }
    }

    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */
    vos_printf("ADS上行正在发送标识                             %d\n",g_stAdsCtx.stAdsIpfCtx.ucSendingFlg);
    vos_printf("\r\n");

    return;
}


VOS_VOID ADS_ShowULProcStats(VOS_VOID)
{
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                           i;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++)
    {
        vos_printf("********************ADS上行统计信息, Instance is %d************************\n", i);
        vos_printf("ADS收到上行数据的个数                           %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulUlRecvPktNum);

        g_stAdsStats.astAdsSpecStatsInfo[i].ulUlSaveBuffPktNum = ADS_UL_GetInstanceAllQueueDataNum(i);

        vos_printf("ADS上行缓存中数据包的个数                       %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulUlSaveBuffPktNum);
        vos_printf("ADS上行入队失败的次数                           %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulUlEnQueFail);
    }

    vos_printf("********************ADS上行统计信息, Common************************\n");
    vos_printf("配置上行IPF没有获得BD/CD次数                    %d\n",g_stAdsStats.stUlComStatsInfo.ulUlCfgIpfHaveNoBDCD);
    vos_printf("ADS发送上行包数量                               %d\n",g_stAdsStats.stUlComStatsInfo.ulUlSendBuffPktNum);
    vos_printf("ADS处理上行事件次数                             %d\n",g_stAdsStats.stUlComStatsInfo.ulUlProcEventNum);

    /* Added by l60609 for AP适配项目 ，2012-08-30 Begin */
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    vos_printf("上行保存源内存fail个数                          %d\n",g_stAdsStats.stUlComStatsInfo.ulUlIpfSaveSrcMemFail);
    vos_printf("上行保存源内存succ个数                          %d\n",g_stAdsStats.stUlComStatsInfo.ulUlIpfSaveSrcMemSucc);
    vos_printf("上行释放源内存个数                              %d\n",g_stAdsStats.stUlComStatsInfo.ulUlIpfFreeSrcMemNum);
    vos_printf("上行释放内存错误                                %d\n",g_stAdsStats.stUlComStatsInfo.ulUlIpfFreeSrcMemErr);
    vos_printf("上行超过队列长度，丢包个数                      %d\n",g_stAdsStats.stUlComStatsInfo.ulUlDroppedPacketNum);

    vos_printf("上行水线边界1配置次数                           %d\n",g_stAdsStats.stUlComStatsInfo.ulLevelOneCnt);
    vos_printf("上行水线边界2配置次数                           %d\n",g_stAdsStats.stUlComStatsInfo.ulLevelTwoCnt);
    vos_printf("上行水线边界3配置次数                           %d\n",g_stAdsStats.stUlComStatsInfo.ulLevelThreeCnt);
    vos_printf("上行水线边界4配置次数                           %d\n",g_stAdsStats.stUlComStatsInfo.ulLevelFourCnt);

    vos_printf("上行赞包门限1                                   %d\n",g_stAdsCtx.stAdsIpfCtx.stUlAssemParmInfo.stThresholdLevel.ulThreshold1);
    vos_printf("上行赞包门限2                                   %d\n",g_stAdsCtx.stAdsIpfCtx.stUlAssemParmInfo.stThresholdLevel.ulThreshold2);
    vos_printf("上行赞包门限3                                   %d\n",g_stAdsCtx.stAdsIpfCtx.stUlAssemParmInfo.stThresholdLevel.ulThreshold3);
    vos_printf("上行赞包门限4                                   %d\n",g_stAdsCtx.stAdsIpfCtx.stUlAssemParmInfo.stThresholdLevel.ulThreshold4);
    vos_printf("当前赞包门限值                                  %d\n",g_stAdsCtx.stAdsIpfCtx.ulThredHoldNum);


#endif
    /* Added by l60609 for AP适配项目 ，2012-08-30 End */
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    vos_printf("\r\n");

    return;
}


VOS_VOID ADS_ShowDLProcStats(VOS_VOID)
{
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                           i;

    /*GU模上行统计量*/
    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++)
    {
        vos_printf("********************ADS下行统计信息, Instance is %d************************\n", i);
        vos_printf("ADS发送给APP数据的个数                          %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlSendPktNum);
        vos_printf("IPF过滤错误包个数                               %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlIPFErrNum);
        vos_printf("获取到大于MTU的IPV6包次数                       %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlIpv6BigPktum);
        vos_printf("RAB ID错误的数据包的个数                        %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRabIdErrPktum);
        vos_printf("需要转发给NDClient数据包的个数                  %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRecvNDClintPktum);
        vos_printf("收到DHCPV4V6，目前未处理的包                    %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRecvDHCPV4V6Pktum);
        vos_printf("收到IPV6LinkLocalAddress 数据个数               %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlRecvIPV6LinkLocalAddressPktum);
        vos_printf("转换IMMZC失败的次数                             %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlTransformImmZcErr);
        vos_printf("ADS下行释放内存的次数                           %d\n",g_stAdsStats.astAdsSpecStatsInfo[i].ulDlFreeNum);
#if 0
        vos_printf("ADS下行流控使能标识                             %d\n",g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.stFcAssemInfo.ulEnableMask);
        vos_printf("ADS下行流控启动阈值                             %d\n",g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.stFcAssemInfo.ulRateUpLev);
        vos_printf("ADS下行流控解除阈值                             %d\n",g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.stFcAssemInfo.ulRateDownLev);
        vos_printf("ADS下行流控RD数量统计周期(ms)                   %d\n",g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.stFcAssemInfo.ulExpireTmrLen);
        vos_printf("ADS下行流控激活标识                             %d\n",g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.stFcAssemInfo.ulFcActiveFlg);
        vos_printf("ADS下行流控时间计数                             %d\n",g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.stFcAssemInfo.ulTmrCnt);
        vos_printf("ADS下行流控RD计数                               %d\n",g_stAdsCtx.astAdsSpecCtx[i].stAdsDlCtx.stFcAssemInfo.ulRdCnt);
#endif
    }

    vos_printf("********************ADS下行统计信息, Common************************\n");
    vos_printf("ADS下行获取RD的个数                             %d\n",g_stAdsStats.stDlComStatsInfo.ulDlRecvRDNum);
    vos_printf("ADS下行获取RD为0的次数                          %d\n",g_stAdsStats.stDlComStatsInfo.ulDlRecvRDZeroNum);
    vos_printf("ADS下行收到IPF中断的次数                        %d\n",g_stAdsStats.stDlComStatsInfo.ulDlRecvIPFBreakNum);
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    vos_printf("ADS下行空中断次数                               %d\n",g_stAdsStats.stDlComStatsInfo.ulDlRecvAdqEmptyBreakNum);
    vos_printf("ADS下行ADQ0为空的次数                           %d\n",g_stAdsStats.stDlComStatsInfo.aulDlGetAdqEmptyNum[IPF_EMPTY_ADQ0] + g_stAdsStats.stDlComStatsInfo.aulDlGetAdqEmptyNum[IPF_EMPTY_ADQ]);
    vos_printf("ADS下行ADQ1为空的次数                           %d\n",g_stAdsStats.stDlComStatsInfo.aulDlGetAdqEmptyNum[IPF_EMPTY_ADQ1] + g_stAdsStats.stDlComStatsInfo.aulDlGetAdqEmptyNum[IPF_EMPTY_ADQ]);
#endif
    vos_printf("ADS处理下行ALL事件次数                          %d\n",g_stAdsStats.stDlComStatsInfo.ulDlAllEvent);
    vos_printf("ADS处理下行RD事件次数                           %d\n",g_stAdsStats.stDlComStatsInfo.ulDlRdEventNum);
    vos_printf("ADS处理下行ADQ EMPTY事件次数                    %d\n",g_stAdsStats.stDlComStatsInfo.ulDlAdEmptyNum);
    vos_printf("ADS处理下行OTHER事件次数                        %d\n",g_stAdsStats.stDlComStatsInfo.ulDlOtherEvent);
    vos_printf("ADS处理下行EMPTY事件次数                        %d\n",g_stAdsStats.stDlComStatsInfo.ulDlEmptyEvent);

    /* Added by l60609 for AP适配项目 ，2012-08-30 Begin */
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    vos_printf("ADS下行获取AD失败次数                           %d\n",g_stAdsStats.stDlComStatsInfo.ulDlGetAdFailNum);
    vos_printf("ADS下行配置AD0成功次数                          %d\n",g_stAdsStats.stDlComStatsInfo.aulDlConfigAdSuccNum[IPF_AD_0]);
    vos_printf("ADS下行配置AD1成功次数                          %d\n",g_stAdsStats.stDlComStatsInfo.aulDlConfigAdSuccNum[IPF_AD_1]);
    vos_printf("ADS下行配置AD0失败次数                          %d\n",g_stAdsStats.stDlComStatsInfo.aulDlConfigAdFailNum[IPF_AD_0]);
    vos_printf("ADS下行配置AD1失败次数                          %d\n",g_stAdsStats.stDlComStatsInfo.aulDlConfigAdFailNum[IPF_AD_1]);
    vos_printf("ADS下行申请AD0成功的次数                        %d\n",g_stAdsStats.stDlComStatsInfo.aulDlAllocAdSuccNum[IPF_AD_0]);
    vos_printf("ADS下行申请AD1成功的次数                        %d\n",g_stAdsStats.stDlComStatsInfo.aulDlAllocAdSuccNum[IPF_AD_1]);
    vos_printf("ADS下行申请AD0失败的次数                        %d\n",g_stAdsStats.stDlComStatsInfo.aulDlAllocAdFailNum[IPF_AD_0]);
    vos_printf("ADS下行申请AD1失败的次数                        %d\n",g_stAdsStats.stDlComStatsInfo.aulDlAllocAdFailNum[IPF_AD_1]);

#endif
    /* Added by l60609 for AP适配项目 ，2012-08-30 End */
    vos_printf("ADS下行保护定时器超时次数                       %d\n",g_stAdsStats.stDlComStatsInfo.ulDlRdTiProtectExpiredNum);
    vos_printf("ADS下行保护定时器超时事件次数                   %d\n",g_stAdsStats.stDlComStatsInfo.ulDlRdProctectEventNum);
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */
    vos_printf("ADS下行流控超时计数                             %d\n",g_stAdsStats.stDlComStatsInfo.ulDlFcTmrExpiredCnt);
    vos_printf("ADS下行流控激活计数                             %d\n",g_stAdsStats.stDlComStatsInfo.ulDlFcActivateCnt);

    vos_printf("\r\n");

    return;
}
VOS_VOID ADS_ShowResetStats(VOS_VOID)
{
    vos_printf("模块初始化标识                              %d\n", g_stAdsStats.stResetStatsinfo.ulSemInitFlg);
    vos_printf("当前的上行二进制信号量                      %p\n", g_stAdsCtx.hULResetSem);
    vos_printf("当前的下行二进制信号量                      %p\n", g_stAdsCtx.hDLResetSem);
    vos_printf("创建的上行二进制信号量                      %x\n", g_stAdsStats.stResetStatsinfo.hULBinarySemId);
    vos_printf("创建的下行二进制信号量                      %x\n", g_stAdsStats.stResetStatsinfo.hDLBinarySemId);
    vos_printf("创建上行二进制信号量失败次数                %d\n", g_stAdsStats.stResetStatsinfo.ulULCreateBinarySemFailNum);
    vos_printf("创建下行二进制信号量失败次数                %d\n", g_stAdsStats.stResetStatsinfo.ulDLCreateBinarySemFailNum);
    vos_printf("锁上行二进制信号量失败次数                  %d\n", g_stAdsStats.stResetStatsinfo.ulULLockBinarySemFailNum);
    vos_printf("锁下行二进制信号量失败次数                  %d\n", g_stAdsStats.stResetStatsinfo.ulDLLockBinarySemFailNum);
    vos_printf("最后一次锁上行二进制信号量失败原因          %x\n", g_stAdsStats.stResetStatsinfo.ulULLastBinarySemErrRslt);
    vos_printf("最后一次锁下行二进制信号量失败原因          %x\n", g_stAdsStats.stResetStatsinfo.ulDLLastBinarySemErrRslt);
    vos_printf("上行复位成功的次数                          %d\n", g_stAdsStats.stResetStatsinfo.ulULResetSucessNum);
    vos_printf("下行复位成功的次数                          %d\n", g_stAdsStats.stResetStatsinfo.ulDLResetSucessNum);
}


VOS_VOID ADS_Help(VOS_VOID)
{

    vos_printf("********************ADS软调信息************************\n");
    vos_printf("ADS_ShowEntityStats                      显示ADS实体信息\n");
    vos_printf("ADS_ShowULProcStats                      显示ADS上行统计信息\n");
    vos_printf("ADS_ShowDLProcStats                      显示ADS下行统计信息\n");
    vos_printf("ADS_ShowResetStats                       显示ADS复位状态信息\n");

    return;
}


VOS_VOID ADS_ResetDebugInfo(VOS_VOID)
{
    VOS_UINT8                           i;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++)
    {
        PS_MEM_SET(&g_stAdsStats.astAdsSpecStatsInfo[i], 0, sizeof(ADS_SPEC_STATS_INFO_STRU));
    }

    PS_MEM_SET(&g_stAdsStats.stUlComStatsInfo, 0 , sizeof(ADS_UL_COM_STATS_INFO_STRU));
    PS_MEM_SET(&g_stAdsStats.stDlComStatsInfo, 0 , sizeof(ADS_DL_COM_STATS_INFO_STRU));

    return;
}

/*****************************************************************************
 函 数 名  : ADS_MNTN_OpenLatency
 功能描述  : 开启用户面数据追踪轨迹
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月19日
    作    者   : huibo
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_MNTN_OpenLatency(VOS_VOID)
{
    g_ulAdsLatencySwitch = 1;

    return;
}

/*****************************************************************************
 函 数 名  : ADS_MNTN_CloseLatency
 功能描述  : 关闭用户面数据追踪轨迹
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月19日
    作    者   : huibo
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_MNTN_CloseLatency(VOS_VOID)
{
    g_ulAdsLatencySwitch = 0;

    return;
}

/*****************************************************************************
 函 数 名  : ADS_MNTN_TraceRcvUlData
 功能描述  : 追踪用户面上行接收数据
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月19日
    作    者   : huibo
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_MNTN_TraceRcvUlData(VOS_VOID)
{
    if (1 == g_ulAdsLatencySwitch)
    {
        ADS_LATENCY_LOG(ACPU_PID_ADS_UL, "User plane latency trace: ADS Rcv UL data");
    }

    return;
}

/*****************************************************************************
 函 数 名  : ADS_MNTN_TraceSndUlData
 功能描述  : 追踪用户面上行发送数据
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月19日
    作    者   : huibo
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_MNTN_TraceSndUlData(VOS_VOID)
{
    if (1 == g_ulAdsLatencySwitch)
    {
        ADS_LATENCY_LOG(ACPU_PID_ADS_UL, "User plane latency trace: ADS Send UL data");
    }

    return;
}

/*****************************************************************************
 函 数 名  : ADS_MNTN_TraceRcvDlData
 功能描述  : 追踪用户面下行接收数据
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月19日
    作    者   : huibo
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_MNTN_TraceRcvDlData(VOS_VOID)
{
    if (1 == g_ulAdsLatencySwitch)
    {
        ADS_LATENCY_LOG(ACPU_PID_ADS_DL, "User plane latency trace: ADS Rcv DL data");
    }

    return;
}

/*****************************************************************************
 Function Name   : ADS_OM_LOG
 Description     : A核的打印
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.z00103912      2011-6-8  Draft Enact
*****************************************************************************/
VOS_VOID ADS_LATENCY_OM_LOG( const VOS_CHAR  *pcFileName,  VOS_UINT32  ulLineNum,
                           VOS_UINT32      ulModuleId,   VOS_UINT32 ulLevel,
                           const VOS_CHAR  *pcString )
{
    #if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT32          ulRslt = 0;

    ulRslt = DIAG_PrintfV(DIAG_ID( ulModuleId,ulLevel ), (VOS_CHAR*)pcFileName, ulLineNum, (VOS_CHAR*)("%s"), pcString);
    if (PS_SUCC != ulRslt)
    {
        return;
    }
    #endif

    return;
}


VOS_VOID ADS_SetFlowDebugFlag(VOS_UINT32  ulFlowDebugFlag)
{
    switch (ulFlowDebugFlag)
    {
        case ADS_FLOW_DEBUG_DL_ON:
            g_stAdsStats.stDlComStatsInfo.ulDLFlowDebugFlag     = PS_TRUE;
            g_stAdsStats.stUlComStatsInfo.ulULFlowDebugFlag     = PS_FALSE;
            g_stAdsStats.stDlComStatsInfo.ulDLFlowRptThreshold  = ADS_FLOW_DL_DEFAULT_RPT_THRESHOLD;
            break;

        case ADS_FLOW_DEBUG_UL_ON:
            g_stAdsStats.stDlComStatsInfo.ulDLFlowDebugFlag     = PS_FALSE;
            g_stAdsStats.stUlComStatsInfo.ulULFlowDebugFlag     = PS_TRUE;
            g_stAdsStats.stUlComStatsInfo.ulULFlowRptThreshold  = ADS_FLOW_UL_DEFAULT_RPT_THRESHOLD;
            break;

        case ADS_FLOW_DEBUG_ALL_ON:
            g_stAdsStats.stDlComStatsInfo.ulDLFlowDebugFlag     = PS_TRUE;
            g_stAdsStats.stUlComStatsInfo.ulULFlowDebugFlag     = PS_TRUE;
            g_stAdsStats.stDlComStatsInfo.ulDLFlowRptThreshold  = ADS_FLOW_DL_DEFAULT_RPT_THRESHOLD;
            g_stAdsStats.stUlComStatsInfo.ulULFlowRptThreshold  = ADS_FLOW_UL_DEFAULT_RPT_THRESHOLD;
            break;

        default:
            g_stAdsStats.stDlComStatsInfo.ulDLFlowDebugFlag     = PS_FALSE;
            g_stAdsStats.stUlComStatsInfo.ulULFlowDebugFlag     = PS_FALSE;
            break;
    }

    return;
}


VOS_VOID ADS_SetFlowDLRptThreshold(VOS_UINT32  ulFlowDLRptThreshold)
{
    g_stAdsStats.stDlComStatsInfo.ulDLFlowRptThreshold  = ulFlowDLRptThreshold;
    return;
}


VOS_VOID ADS_SetFlowULRptThreshold(VOS_UINT32  ulFlowULRptThreshold)
{
    g_stAdsStats.stUlComStatsInfo.ulULFlowRptThreshold  = ulFlowULRptThreshold;
    return;
}


VOS_VOID ADS_DLFlowAdd(VOS_UINT8  ucIndex, VOS_UINT32  ulSduLen)
{
    ADS_SPEC_STATS_INFO_STRU           *pstAdsSpecStatsInfo;

    if (PS_TRUE == g_stAdsStats.stDlComStatsInfo.ulDLFlowDebugFlag)
    {
        pstAdsSpecStatsInfo = &(g_stAdsStats.astAdsSpecStatsInfo[ucIndex]);

        /* 流量统计 */
        pstAdsSpecStatsInfo->ulDLFlowInfo += ulSduLen;

        /* 流量统计上报 */
        if (pstAdsSpecStatsInfo->ulDLFlowInfo >= g_stAdsStats.stDlComStatsInfo.ulDLFlowRptThreshold)
        {
            pstAdsSpecStatsInfo->ulDLEndSlice   = VOS_GetSlice();

            vos_printf("ADS Index = %d, DL Flow Info = %10d, Pkt Num = %10d, Slice = %10d, Time = %10d\n",
                ucIndex, pstAdsSpecStatsInfo->ulDLFlowInfo,
                pstAdsSpecStatsInfo->ulDlSendPktNum, pstAdsSpecStatsInfo->ulDLEndSlice,
                (pstAdsSpecStatsInfo->ulDLEndSlice - pstAdsSpecStatsInfo->ulDLStartSlice));

            pstAdsSpecStatsInfo->ulDLStartSlice = pstAdsSpecStatsInfo->ulDLEndSlice;
            pstAdsSpecStatsInfo->ulDLFlowInfo   = 0;
        }
    }

    return;
}


VOS_VOID ADS_ULFlowAdd(VOS_UINT8  ucIndex, VOS_UINT32  ulSduLen)
{
    ADS_SPEC_STATS_INFO_STRU           *pstAdsSpecStatsInfo;

    if (PS_TRUE == g_stAdsStats.stUlComStatsInfo.ulULFlowDebugFlag)
    {
        pstAdsSpecStatsInfo = &(g_stAdsStats.astAdsSpecStatsInfo[ucIndex]);

        /* 流量统计 */
        pstAdsSpecStatsInfo->ulULFlowInfo += ulSduLen;

        /* 流量统计上报 */
        if (pstAdsSpecStatsInfo->ulULFlowInfo >= g_stAdsStats.stUlComStatsInfo.ulULFlowRptThreshold)
        {
            pstAdsSpecStatsInfo->ulULEndSlice   = VOS_GetSlice();

            vos_printf("ADS Index = %d, UL Flow Info = %10d, Pkt Num = %10d, Slice = %10d, Time = %10d\n",
                ucIndex, pstAdsSpecStatsInfo->ulULFlowInfo,
                pstAdsSpecStatsInfo->ulUlRecvPktNum, pstAdsSpecStatsInfo->ulULEndSlice,
                (pstAdsSpecStatsInfo->ulULEndSlice - pstAdsSpecStatsInfo->ulULStartSlice));

            pstAdsSpecStatsInfo->ulULStartSlice = pstAdsSpecStatsInfo->ulULEndSlice;
            pstAdsSpecStatsInfo->ulULFlowInfo   = 0;
        }
    }

    return;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


