/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AdsUpLink.c
  版 本 号   : 初稿
  作    者   : 鲁琳/l60609
  生成日期   : 2011年12月6日
  最近修改   :
  功能描述   : ADS上行处理
  函数列表   :
  修改历史   :
  1.日    期   : 2011年12月6日
    作    者   : 鲁琳/l60609
    修改内容   : 创建文件

******************************************************************************/
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "AdsUpLink.h"
#include "AdsDebug.h"
#include "IpsMntn.h"

#include "AcpuReset.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"

#include "AdsFilter.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define    THIS_FILE_ID                 PS_FILE_ID_ADS_UPLINK_C
/*lint +e767*/


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID ADS_UL_StartDsFlowStats(
    VOS_UINT8                           ucInstance,
    VOS_UINT8                           ucRabId
)
{
    /* 如果上行队列存在, 则启动流量统计定时器 */
    if (VOS_OK == ADS_UL_IsQueueExistent(ucInstance, ucRabId))
    {
        ADS_StartTimer(ACPU_PID_ADS_UL,
                       ADS_GET_DSFLOW_TMR_ID(ucInstance),
                       TI_ADS_DSFLOW_STATS_LEN);
    }

    return;
}
VOS_VOID ADS_UL_StopDsFlowStats(
    VOS_UINT8                           ucInstance,
    VOS_UINT8                           ucRabId
)
{
    /* 如果所有上行队列已不存在，则停止流量统计定时器并清空流量统计信息 */
    if (VOS_FALSE == ADS_UL_IsAnyQueueExist(ucInstance))
    {
        ADS_StopTimer(ACPU_PID_ADS_UL, ADS_GET_DSFLOW_TMR_ID(ucInstance), ADS_TIMER_STOP_CAUSE_USER);
        ADS_InitStatsInfoCtx(ucInstance);
    }

    return;
}
VOS_VOID ADS_UL_StartRptStatsInfoTimer(
    VOS_UINT8                           ucInstance,
    VOS_UINT8                           ucRabId
)
{
    /* 如果上行队列存在, 则启动上报统计信息定时器 */
    if (VOS_OK == ADS_UL_IsQueueExistent(ucInstance, ucRabId))
    {
        ADS_StartTimer(ACPU_PID_ADS_UL,
                       TI_ADS_RPT_STATS_INFO,
                       TI_ADS_RPT_STATS_LEN);
    }

    return;
}
VOS_VOID ADS_UL_StopRptStatsInfoTimer(VOS_VOID)
{
    VOS_UINT8                           ucInsIndex;

    for (ucInsIndex = 0; ucInsIndex < ADS_INSTANCE_MAX_NUM; ucInsIndex++)
    {
        if (VOS_FALSE == ADS_UL_IsAnyQueueExist(ucInsIndex))
        {
            ADS_StopTimer(ACPU_PID_ADS_UL, TI_ADS_RPT_STATS_INFO, ADS_TIMER_STOP_CAUSE_USER);
        }
    }
}


VOS_UINT32 ADS_UL_IsSendPermit(VOS_VOID)
{
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    return ADS_UL_GET_MODEM_SND_PERMIT_FLAG(ADS_INSTANCE_INDEX_0);
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */
}


VOS_UINT32 ADS_UL_SendPacket(
    IMM_ZC_STRU                        *pstData,
    VOS_UINT8                           ucRabId
)
{
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                           ucInstanceIndex;
    VOS_UINT8                           ucRealRabId;

    /* 判断是否为空数据包 */
    if (VOS_NULL_PTR == pstData)
    {
        ADS_WARNING_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendPacket: pstData is null!");
        return VOS_ERR;
    }

    ucInstanceIndex = ADS_GET_MODEM_ID_FROM_EX_RAB_ID(ucRabId);
    ucRealRabId     = ADS_GET_RAB_ID_FROM_EX_RAB_ID(ucRabId);

    /* 增加上行接收数据统计个数 */
    ADS_DBG_RECV_UL_PKT_NUM(ucInstanceIndex, 1);

    /* 流量统计 */
    ADS_ULFlowAdd(ucInstanceIndex, pstData->len);

    /* 追踪上行接收数据 */
    ADS_MNTN_TraceRcvUlData();

    /* 统计上行周期性收到的数据字节数，用于流量查询 */
    ADS_RECV_UL_PERIOD_PKT_NUM(ucInstanceIndex, pstData->len);

    /* 将pstData插入到ucRabId对应的缓存队列中 */
    if (VOS_OK != ADS_UL_InsertQueue(ucInstanceIndex, pstData, ucRealRabId))
    {
        ADS_DBG_UL_ENQUE_FAIL_NUM(ucInstanceIndex, 1);
        ADS_WARNING_LOG1(ACPU_PID_ADS_UL, "ADS_UL_SendPacket: insert fail, rab id !", ucRealRabId);
        return VOS_ERR;
    }

    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    return VOS_OK;
}

/* Added by L47619 for V3R3 Share-PDP Project, 2013-6-3, begin */
/*****************************************************************************
 函 数 名  : ADS_UL_SendPacketEx
 功能描述  : 上行发送数据扩展接口, 使用该接口发送的数据在ADS记录其信息, 用于
             下行数据过滤匹配, 该接口必须和ADS_DL_RegFilterDataCallback配合
             使用, 只有使用ADS_DL_RegFilterDataCallback注册过下行过滤回调后,
             下行数据才需要根据使用该接口记录的信息进行过滤
 输入参数  : pstData  - IMM数据
             enIpType - IP类型
             ucRabId  - RABID [5, 15]
 输出参数  : 无
 返 回 值  : VOS_OK   - 去注册成功
             VOS_ERR  - 去注册失败
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年6月3日
    作    者   : L47619
    修改内容   : V3R3 Share-PDP项目修改
*****************************************************************************/
VOS_UINT32 ADS_UL_SendPacketEx(
    IMM_ZC_STRU                        *pstData,
    ADS_PKT_TYPE_ENUM_UINT8             enIpType,
    VOS_UINT8                           ucRabId
)
{
    ADS_DL_RAB_INFO_STRU               *pstDlRabInfo;
    VOS_UINT8                           ucInstanceIndex;
    VOS_UINT8                           ucRealRabId;

    /* 判断是否为空数据包 */
    if (VOS_NULL_PTR == pstData)
    {
        ADS_WARNING_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendPacketEx: pstData is null!");
        return VOS_ERR;
    }

    ucInstanceIndex = ADS_GET_MODEM_ID_FROM_EX_RAB_ID(ucRabId);
    ucRealRabId     = ADS_GET_RAB_ID_FROM_EX_RAB_ID(ucRabId);

    /* 判断是否已经注册过下行过滤回调函数，若注册过，则需要进行过滤信息的提取，否则直接发送报文 */
    pstDlRabInfo = ADS_DL_GET_RAB_INFO_PTR(ucInstanceIndex, ucRealRabId);
    if (VOS_NULL_PTR != pstDlRabInfo->pRcvDlFilterDataFunc)
    {
        /* 调用过滤上行数据包处理函数 */
        ADS_FILTER_ProcUlPacket(pstData, enIpType);
    }

    /* 增加上行接收数据统计个数 */
    ADS_DBG_RECV_UL_PKT_NUM(ucInstanceIndex, 1);

    /* 流量统计 */
    ADS_ULFlowAdd(ucInstanceIndex, pstData->len);

    /* 追踪上行接收数据 */
    ADS_MNTN_TraceRcvUlData();

    /* 统计上行周期性收到的数据字节数，用于流量查询 */
    ADS_RECV_UL_PERIOD_PKT_NUM(ucInstanceIndex, pstData->len);

    /* 将pstData插入到ucRabId对应的缓存队列中 */
    if (VOS_OK != ADS_UL_InsertQueue(ucInstanceIndex, pstData, ucRealRabId))
    {
        ADS_DBG_UL_ENQUE_FAIL_NUM(ucInstanceIndex, 1);
        ADS_WARNING_LOG1(ACPU_PID_ADS_UL, "ADS_UL_SendPacketEx: insert fail, rab id !", ucRealRabId);
        return VOS_ERR;
    }

    return VOS_OK;

}
/* Added by L47619 for V3R3 Share-PDP Project, 2013-6-3, end */

/* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */

IMM_ZC_STRU* ADS_UL_GetInstanceNextQueueNode(
    VOS_UINT8                           ucInstanceIndex,
    VOS_UINT8                          *pucRabId
)
{
    VOS_UINT32                          i;
    VOS_UINT8                          *pucCurIndex;
    ADS_UL_CTX_STRU                    *pstAdsUlCtx;
    IMM_ZC_STRU                        *pstNode;

    pstAdsUlCtx = ADS_GetUlCtx(ucInstanceIndex);

    pucCurIndex = &pstAdsUlCtx->ucAdsUlCurIndex;

    pstNode     = VOS_NULL_PTR;

    /* 优先级方式 */
    for (i = 0; i < ADS_RAB_NUM_MAX; i++)
    {
        /* 因为队列已经有序，当前队列无效则代表后面所有队列都无效
           需跳过后面所有无效队列，继续从头查找 */
        if (VOS_NULL_PTR == ADS_UL_GET_QUEUE_LINK_INFO(ucInstanceIndex, *pucCurIndex))
        {
            i += ADS_RAB_NUM_MAX - (*pucCurIndex + 1U);

            *pucCurIndex = 0;

            continue;
        }

        /* 队列为有效队列但无数据时，继续向后查找 */
        if (0 == ADS_UL_GET_QUEUE_LINK_INFO(ucInstanceIndex, *pucCurIndex)->qlen)
        {
            /* 发送下一个队列的数据时，将本队列记录数清空 */
            ADS_UL_CLR_RECORD_NUM_IN_WEIGHTED(ucInstanceIndex, *pucCurIndex);

            *pucCurIndex = (*pucCurIndex + 1) % ADS_RAB_NUM_MAX;

            continue;
        }

        /* Modified by l60609 for DSDA Phase II, 2012-12-05, Begin */
        /* 当前队列不允许发送， 继续向后查找 */
        if (VOS_FALSE == ADS_UL_GET_RAB_SND_PERMIT_FLAG(ucInstanceIndex, ADS_UL_GET_PRIO_QUEUE_INDEX(ucInstanceIndex, *pucCurIndex)))
        {
            *pucCurIndex = (*pucCurIndex + 1) % ADS_RAB_NUM_MAX;

            continue;
        }
        /* Modified by l60609 for DSDA Phase II, 2012-12-05, End */

        /* 根据优先级等级对应的加权数发送数据 */
        /* 优先级高的先发送 */
        if (ADS_UL_GET_RECORD_NUM_IN_WEIGHTED(ucInstanceIndex, *pucCurIndex) < ADS_UL_GET_QUEUE_PRI_WEIGHTED_NUM(ucInstanceIndex, *pucCurIndex))
        {
            /* 获取队列头结点 */
            pstNode = IMM_ZcDequeueHead(ADS_UL_GET_QUEUE_LINK_INFO(ucInstanceIndex, *pucCurIndex));

            /* 获取该结点的RabId */
            *pucRabId = ADS_UL_GET_PRIO_QUEUE_INDEX(ucInstanceIndex, *pucCurIndex);

            /* 本队列记录数增加 1*/
            ADS_UL_SET_RECORD_NUM_IN_WEIGHTED(ucInstanceIndex, *pucCurIndex, 1);

            /* 如果已经发送完本队列的优先级加权数个数的数据，则跳到下个队列发送数据 */
            if (ADS_UL_GET_RECORD_NUM_IN_WEIGHTED(ucInstanceIndex, *pucCurIndex) == ADS_UL_GET_QUEUE_PRI_WEIGHTED_NUM(ucInstanceIndex, *pucCurIndex))
            {
                /* 发送下一个队列的数据时，将本队列记录数清空 */
                ADS_UL_CLR_RECORD_NUM_IN_WEIGHTED(ucInstanceIndex, *pucCurIndex);

                *pucCurIndex = (*pucCurIndex + 1) % ADS_RAB_NUM_MAX;
            }

            break;
        }
    }


    return pstNode;
}

/*****************************************************************************
 函 数 名  : ADS_UL_GetNextQueueNode
 功能描述  : DSDA Phase II:双实例
 输入参数  : VOS_UINT8 *pucRabId
 输出参数  : 无
 返 回 值  : IMM_ZC_STRU*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月24日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
IMM_ZC_STRU* ADS_UL_GetNextQueueNode(
    VOS_UINT8                          *pucRabId,
    VOS_UINT8                          *pucInstanceIndex
)
{
    ADS_CTX_STRU                       *pstAdsCtx = VOS_NULL_PTR;
    IMM_ZC_STRU                        *pstNode = VOS_NULL_PTR;
    VOS_UINT8                           i;
    VOS_UINT8                           ucCurInstanceIndex;

    pstAdsCtx = ADS_GetAllCtx();

    ucCurInstanceIndex = pstAdsCtx->ucAdsCurInstanceIndex;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++)
    {
        ucCurInstanceIndex = (ucCurInstanceIndex + i) % ADS_INSTANCE_MAX_NUM;

        pstNode = ADS_UL_GetInstanceNextQueueNode(ucCurInstanceIndex, pucRabId);

        if (VOS_NULL_PTR != pstNode)
        {
            break;
        }
    }

    /* 返回实例号用于配置BD时填modem id */
    *pucInstanceIndex = ucCurInstanceIndex;

    /* 记录下次从哪个实例中去数据 */
    pstAdsCtx->ucAdsCurInstanceIndex = (ucCurInstanceIndex + 1U) % ADS_INSTANCE_MAX_NUM;

    return pstNode;
}

/* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

#if(FEATURE_OFF == FEATURE_SKB_EXP)
/* Added by l60609 for AP适配项目 ，2012-08-30 Begin */
/*****************************************************************************
 函 数 名  : ADS_UL_SaveIpfUlSrcMem
 功能描述  : 在上行配置完IPF BD后，保存需要释放的源内存
 输入参数  : const IMM_ZC_STRU                  *pstImmZcNode
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月30日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_UL_SaveIpfUlSrcMem(IMM_ZC_STRU *pstImmZcNode)
{
    IMM_ZC_HEAD_STRU                   *pstUlFreeQue = VOS_NULL_PTR;

    /* 较少对性能的影响，调用函数能保证pstIpfUlBdBuff和ulSaveNum的有效性，所有不进行参数检查 */

    pstUlFreeQue = ADS_UL_IPF_SRCMEM_FREE_QUE();

    /* 插入队列 */
    IMM_ZcQueueTail(pstUlFreeQue, pstImmZcNode);

    ADS_DBG_UL_IPF_SAVE_SRCMEM_SUCC_NUM(1);

    return;
}

/*****************************************************************************
 函 数 名  : ADS_UL_FreeIpfUlConfigSuccSrcMem
 功能描述  : 释放源内存队列中可释放的已经配置成功的源系统内存
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月30日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_UL_FreeIpfUlConfigSuccSrcMem(VOS_VOID)
{
    IMM_ZC_HEAD_STRU                 *pstUlFreeQue = VOS_NULL_PTR;
    VOS_UINT32                        ulIdleBD;
    VOS_UINT32                        ulBusyBD;
    VOS_UINT32                        ulCanFree;
    VOS_UINT32                        ulQueCnt;
    VOS_UINT32                        ulCnt;
    IMM_ZC_STRU                      *pFreeNode = VOS_NULL_PTR;

    pstUlFreeQue = ADS_UL_IPF_SRCMEM_FREE_QUE();

    ulQueCnt = IMM_ZcQueueLen(pstUlFreeQue);
    if (0 == ulQueCnt)
    {
        return;
    }

    /* get busy bd num */
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    ulIdleBD = mdrv_ipf_get_uldesc_num();
#else
    ulIdleBD = mdrv_ipf_get_ulbd_num();
#endif
    ulBusyBD = IPF_ULBD_DESC_SIZE - ulIdleBD;
    if (ulQueCnt >= ulBusyBD)
    {
        ulCanFree = ulQueCnt - ulBusyBD;
    }
    else
    {
        ADS_ERROR_LOG2(ACPU_PID_ADS_UL, "ADS_UL_FreeIpfUlSrcMem: Buff Num Less IPF Busy BD Num.",ulQueCnt,ulBusyBD);
        ADS_DBG_UL_IPF_FREE_SRCMEM_ERROR(1);
        return;
    }

    /*free src mem*/
    for (ulCnt = 0; ulCnt < ulCanFree; ulCnt++)
    {
        pFreeNode = IMM_ZcDequeueHead(pstUlFreeQue);

        IMM_ZcFree(pFreeNode);

        ADS_DBG_UL_IPF_FREE_SRCMEM_NUM(1);
    }

    return;
}

/*****************************************************************************
 函 数 名  : ADS_UL_FreeIpfUlConfigFailSrcMem
 功能描述  : 释放IPF配置失败的源系统内存
 输入参数  : VOS_UINT32 ulNeedFreeCnt
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月4日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_UL_FreeIpfUlConfigFailSrcMem(VOS_UINT32 ulNeedFreeCnt)
{
    IMM_ZC_HEAD_STRU                 *pstUlFreeQue = VOS_NULL_PTR;
    VOS_UINT32                        ulQueCnt;
    VOS_UINT32                        ulCnt;
    IMM_ZC_STRU                      *pFreeNode = VOS_NULL_PTR;

    pstUlFreeQue = ADS_UL_IPF_SRCMEM_FREE_QUE();

    ulQueCnt = IMM_ZcQueueLen(pstUlFreeQue);
    if (0 == ulQueCnt)
    {
        return;
    }

    if (ulNeedFreeCnt > ulQueCnt)
    {
        ADS_ERROR_LOG2(ACPU_PID_ADS_UL, "ADS_UL_FreeIpfUlConfigFailSrcMem: Queue Num Less Need Free Num.",ulQueCnt,ulNeedFreeCnt);
        ADS_DBG_UL_IPF_FREE_SRCMEM_ERROR(1);
        return;
    }

    /*free src mem*/
    for (ulCnt = 0; ulCnt < ulNeedFreeCnt; ulCnt++)
    {
        pFreeNode = IMM_ZcDequeueTail(pstUlFreeQue);

        IMM_ZcFree(pFreeNode);

        ADS_DBG_UL_IPF_FREE_SRCMEM_NUM(1);
    }

    return;
}

/*****************************************************************************
 函 数 名  : ADS_UL_ClearIpfUlSrcMem
 功能描述  : 清空上行IPF Src Mem
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月31日
    作    者   : l60609
    修改内容   : 新生成函数

  2.日    期   : 2012年11月24日
    作    者   : l60609
    修改内容   : DSDA Phase II:双实例
*****************************************************************************/
VOS_VOID ADS_UL_ClearIpfUlSrcMem(VOS_VOID)
{
    IMM_ZC_HEAD_STRU                 *pstUlFreeQue = VOS_NULL_PTR;
    VOS_UINT32                        ulQueCnt;
    VOS_UINT32                        ulCnt;
    IMM_ZC_STRU                      *pstFreeNode = VOS_NULL_PTR;
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                         i;
    VOS_UINT32                        ulIpfUlBdNum;

    pstUlFreeQue = ADS_UL_IPF_SRCMEM_FREE_QUE();

    ulQueCnt = IMM_ZcQueueLen(pstUlFreeQue);
    if (0 == ulQueCnt)
    {
        return;
    }

    /* 所有的PDP都去激活后，并且BD已经全部空闲，即上行数据全部搬完，才清空上行源内存队列 */
    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++)
    {
        if (VOS_FALSE == ADS_UL_CheckAllQueueEmpty(i))
        {
            return;
        }
    }

#if(FEATURE_OFF == FEATURE_SKB_EXP)
    ulIpfUlBdNum = mdrv_ipf_get_uldesc_num();
#else
    ulIpfUlBdNum = mdrv_ipf_get_ulbd_num();
#endif

    /* 空闲BD最多63个 */
    if (IPF_ULBD_DESC_SIZE != ulIpfUlBdNum)
    {
        return;
    }
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    /*free src mem*/
    for (ulCnt = 0; ulCnt < ulQueCnt; ulCnt++)
    {
        pstFreeNode = IMM_ZcDequeueHead(pstUlFreeQue);

        IMM_ZcFree(pstFreeNode);

        ADS_DBG_UL_IPF_FREE_SRCMEM_NUM(1);
    }

    return;
}

/* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
/*****************************************************************************
 函 数 名  : ADS_UL_SetMaxQueueLength
 功能描述  : 为了调试需要，支持动态调整上行队列长度
 输入参数  : VOS_UINT32                          ulLength,
             VOS_UINT8                           ucInstanceIndex
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月3日
    作    者   : l60609
    修改内容   : 新生成函数

  2.日    期   : 2012年11月24日
    作    者   : l60609
    修改内容   : DSDA Phase II:双实例
*****************************************************************************/
VOS_VOID ADS_UL_SetMaxQueueLength(
    VOS_UINT32                          ulLength,
    VOS_UINT8                           ucInstanceIndex
)
{
    g_stAdsCtx.astAdsSpecCtx[ucInstanceIndex].stAdsUlCtx.ulUlMaxQueueLength = ulLength;
}
/* Modified by l60609 for DSDA Phase II, 2012-11-24, End */
/* Added by l60609 for AP适配项目 ，2012-08-30 End */


VOS_VOID ADS_UL_ConfigBD(VOS_UINT32 ulBdNum)
{
    VOS_UINT32                          ulCnt;
    VOS_INT32                           lRslt;
    VOS_UINT8                           ucRabId;
    IMM_ZC_STRU                        *pstImmZcNode;
    IPF_CONFIG_ULPARAM_S               *pstIpfConfigUlParam;
    VOS_UINT8                           ucInstanceIndex;
    VOS_UINT32                          i;
#ifdef CONFIG_ARM64
    struct device                       dev;
    VOS_UINT64                          dma_mask = 0xffffffffULL;

    VOS_MemSet(&dev, 0, (VOS_SIZE_T)sizeof(dev));

    dev.dma_mask = &(dma_mask);
#endif

    for (ulCnt = 0; ulCnt < ulBdNum; ulCnt++)
    {
        pstImmZcNode = ADS_UL_GetNextQueueNode(&ucRabId, &ucInstanceIndex);

        if (VOS_NULL_PTR == pstImmZcNode)
        {
            break;
        }

        IPS_MNTN_TraceIpInfo(pstImmZcNode,ID_IPS_TRACE_IP_ADS_UL);

        /* 获取配置IPF的BD信息 */
        pstIpfConfigUlParam = ADS_UL_GET_BD_CFG_PARA_PTR(ulCnt);

        pstIpfConfigUlParam->u32Data      = (VOS_UINT32)virt_to_phys((VOS_VOID *)pstImmZcNode->data);

        pstIpfConfigUlParam->u16Len       = (VOS_UINT16)pstImmZcNode->len;
        pstIpfConfigUlParam->u16UsrField1 = (VOS_UINT16)ADS_UL_BUILD_BD_USER_FIELD_1(ucInstanceIndex, ucRabId);
        pstIpfConfigUlParam->u32UsrField3 = (VOS_UINT)pstImmZcNode->tstamp.tv64;

        /* ADS 上行数据打印开关 */
        if (VOS_TRUE == g_ulAdsPrintUlDataFlg)
        {
            vos_printf("ADS_UL_ConfigBD: ucInstanceIndex is %d, ucRabId is %d, attribute is %d\r\n", ucInstanceIndex, ucRabId, pstIpfConfigUlParam->u16UsrField1);

            vos_printf("ADS_UL_ConfigBD: Data Len is %d.\r\n", pstImmZcNode->len);

            vos_printf("ADS_UL_ConfigBD: Data Content is :\r\n");

            for (i = 0; i < pstImmZcNode->len; i++)
            {
                if (pstImmZcNode->data[i] > 0xf)
                {
                    vos_printf("%x", pstImmZcNode->data[i]);
                }
                else
                {
                    vos_printf("0%x", pstImmZcNode->data[i]);
                }
            }

            vos_printf("\r\n");
        }

        /* Attribute: 中断使能，过滤加搬移，过滤器组号modem0用0，modem1用1 */
        pstIpfConfigUlParam->u16Attribute = (VOS_UINT16)ADS_UL_BUILD_BD_ATTRIBUTE(VOS_FALSE, IPF_MODE_FILTERANDTRANS, ADS_UL_GET_BD_FC_HEAD(ucInstanceIndex));

        /* 需要将数据写回DDR，IPF从DDR中读数据 */

#ifdef CONFIG_ARM64
        ADS_CACHE_FLUSH_WITH_DEV(&dev, pstImmZcNode->data, pstIpfConfigUlParam->u16Len);
#else
        ADS_CACHE_FLUSH(pstImmZcNode->data, pstIpfConfigUlParam->u16Len);
#endif

        /* 将已配置的BD源内存保存到源内存队列 */
        ADS_UL_SaveIpfUlSrcMem(pstImmZcNode);
    }

    /* 实际从队列中取的数据个数如果为0，异常 */
    if (0 == ulCnt)
    {
        return;
    }

    /* 最后一个BD配置中断使能 */
    pstIpfConfigUlParam = ADS_UL_GET_BD_CFG_PARA_PTR(0);
    ADS_UL_SET_BD_ATTR_INT_FLAG(pstIpfConfigUlParam[ulCnt -1].u16Attribute);

    /* 配置BD、写入IPF */
    lRslt = mdrv_ipf_config_ulbd(ulCnt, ADS_UL_GET_BD_CFG_PARA_PTR(0));
    if (IPF_SUCCESS != lRslt)
    {
        /* 写IPF失败的处理 */
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_ConfigBD: IPF CONFIG FAIL!");

        /* IPF配置失败，需要释放源内存 */
        ADS_UL_FreeIpfUlConfigFailSrcMem(ulCnt);

        ADS_DBG_UL_SEND_PKT_FAIL_NUM(ulCnt);

        return;
    }

    ADS_DBG_UL_SEND_BUFF_PKT_NUM(ulCnt);

    /* 追踪上行发送数据 */
    ADS_MNTN_TraceSndUlData();

    return;
}

#else
/*****************************************************************************
 函 数 名  : ADS_UL_ProcIpfFailConfig
 功能描述  : ADS配置上行BD写入IPF失败的处理
 输入参数  : VOS_UINT32                          ulBdNum,
             IPF_CONFIG_ULPARAM_S               *pstIpfConfigUlPara
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月7日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

  2.日    期   : 2012年9月4日
    作    者   : l60609
    修改内容   : AP适配项目：需要释放源内存
*****************************************************************************/
VOS_VOID ADS_UL_ProcIpfFailConfig(
    VOS_UINT32                          ulBdNum,
    IPF_CONFIG_ULPARAM_S               *pstIpfConfigUlPara
)
{
    VOS_UINT32                          i;
    IMM_MEM_STRU                       *pstImmMem;

    for (i = 0; i < ulBdNum; i++)
    {
        pstImmMem = (IMM_MEM_STRU *)pstIpfConfigUlPara[i].u32UsrField2;

        /* 释放转换后的IMM数据 */
        IMM_MemFree(pstImmMem);
    }

    return;
}
VOS_VOID ADS_UL_ConfigBD(VOS_UINT32 ulBdNum)
{
    VOS_UINT32                          ulCnt;
    VOS_INT32                           lRslt;
    VOS_UINT8                           ucRabId;
    IMM_ZC_STRU                        *pstImmZcNode;
    IPF_CONFIG_ULPARAM_S               *pstIpfConfigUlParam;
    VOS_UINT8                           ucInstanceIndex;
    IMM_MEM_STRU                       *pstImmMemNode = VOS_NULL_PTR;

    for (ulCnt = 0; ulCnt < ulBdNum; ulCnt++)
    {
        pstImmZcNode = ADS_UL_GetNextQueueNode(&ucRabId, &ucInstanceIndex);

        if (VOS_NULL_PTR == pstImmZcNode)
        {
            break;
        }

        IPS_MNTN_TraceIpInfo(pstImmZcNode,ID_IPS_TRACE_IP_ADS_UL);

        /* 把IMM_Zc零拷贝控制节点转换成IMM_Mem控制节点 */
        pstImmMemNode = IMM_ZcMapToImmMem(pstImmZcNode);

        if (VOS_NULL_PTR == pstImmMemNode)
        {
            ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_ConfigBD: IMM_ZcMapToImmMem return NULL!");

            break;
        }

        /* 获取配置IPF的BD信息 */
        pstIpfConfigUlParam = ADS_UL_GET_BD_CFG_PARA_PTR(ulCnt);

        pstIpfConfigUlParam->u32Data      = (VOS_UINT32)mdrv_virt_to_phy(MEM_TTF_MODE, (VOS_VOID *)pstImmZcNode->data);
        /* 上行只过滤不搬移，源地址和目的地址相同 */
        pstIpfConfigUlParam->u32DesAddr   = pstIpfConfigUlParam->u32Data;
        pstIpfConfigUlParam->u16Len       = (VOS_UINT16)pstImmZcNode->len;
        pstIpfConfigUlParam->u16UsrField1 = (VOS_UINT16)ADS_UL_BUILD_BD_USER_FIELD_1(ucInstanceIndex, ucRabId);
        pstIpfConfigUlParam->u32UsrField2 = (VOS_UINT32)pstImmMemNode;

#if (FEATURE_ON == FEATURE_TTFMEM_CACHE)
        ADS_CACHE_FLUSH(pstImmZcNode->data, pstIpfConfigUlParam->u16Len);
#endif
        /* 释放SK_BUFF 头 */
        IMM_ZcHeadFree(pstImmZcNode);
    }

    /* 实际从队列中取的数据个数如果为0，异常 */
    if (0 == ulCnt)
    {
        return;
    }

    /* 配置BD、写入IPF */
    lRslt = mdrv_ipf_config_ulbd(ulCnt,
                                 ADS_UL_GET_BD_CFG_PARA_PTR(0),
                                 BSP_TRUE);
    if (IPF_SUCCESS != lRslt)
    {
        /* 写IPF失败的处理 */
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_ConfigBD: IPF CONFIG FAIL!");

        ADS_UL_ProcIpfFailConfig(ulCnt, ADS_UL_GET_BD_CFG_PARA_PTR(0));

        ADS_DBG_UL_SEND_PKT_FAIL_NUM(ulCnt);

        return;
    }

    ADS_DBG_UL_SEND_BUFF_PKT_NUM(ulCnt);

    /* 追踪上行发送数据 */
    ADS_MNTN_TraceSndUlData();

    return;
}
#endif


VOS_VOID ADS_UL_ProcLinkData(VOS_VOID)
{
    VOS_UINT32                          ulAllUlQueueDataNum;
    VOS_UINT32                          ulIpfUlBdNum;
    VOS_UINT32                          ulSndBdNum;

    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    /* 判断当前是否允许发送，如果不允许发送，直接退出 */
    if ((VOS_FALSE == ADS_UL_GET_MODEM_SND_PERMIT_FLAG(ADS_INSTANCE_INDEX_0))
     && (VOS_FALSE == ADS_UL_GET_MODEM_SND_PERMIT_FLAG((ADS_INSTANCE_INDEX_0 + 1) % ADS_INSTANCE_MAX_NUM)))
    {
        ADS_NORMAL_LOG(ACPU_PID_ADS_UL, "ADS_UL_ProcLinkData: not permit send!");
        return;
    }
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    /* 处理队列时中的数据 */
    for (;;)
    {

        /* 获取上行可以发送的BD数。 */
#if(FEATURE_OFF == FEATURE_SKB_EXP)
        ulIpfUlBdNum = mdrv_ipf_get_uldesc_num();
#else
        ulIpfUlBdNum = mdrv_ipf_get_ulbd_num();
#endif
        if (0 == ulIpfUlBdNum)
        {
            ADS_DBG_UL_CFG_IPF_HAVE_NO_BDCD(1);

            /* 设置发送结束标志 */
            ADS_UL_SET_SENDING_FLAG(VOS_FALSE);

            /* 启动定时器退出 */
            ADS_StartTimer(ACPU_PID_ADS_UL, TI_ADS_UL_SEND, ADS_UL_GET_PROTECT_TIMER_LEN());
            break;
        }


        /* 设置正在发送标志 */
        ADS_UL_SET_SENDING_FLAG(VOS_TRUE);

        /* 获取当前所有队列中的数据包个数 */
        ulAllUlQueueDataNum = ADS_UL_GetAllQueueDataNum();

        /* 计算当前可发送的BD数目 */
        ulSndBdNum = PS_MIN(ulIpfUlBdNum, ulAllUlQueueDataNum);

        /* Added by l60609 for AP适配项目 ，2012-08-31 Begin */
#if(FEATURE_OFF == FEATURE_SKB_EXP)
        ADS_UL_FreeIpfUlConfigSuccSrcMem();
#endif
        /* Added by l60609 for AP适配项目 ，2012-08-31 End */

        /* 配置BD，写入IPF */
        ADS_UL_ConfigBD(ulSndBdNum);

        /* 获取当前所有队列中的数据包个数 */
        ulAllUlQueueDataNum = ADS_UL_GetAllQueueDataNum();

        /* 当前队列中没有数据，退出，等待下次队列由空变为非空处理 */
        if (0 == ulAllUlQueueDataNum)
        {
            /* 设置发送结束标志 */
            ADS_UL_SET_SENDING_FLAG(VOS_FALSE);

            break;
        }
        /* 当前队列中有数据，但是需要继续攒包 */
        else if (ulAllUlQueueDataNum <= ADS_UL_SEND_DATA_NUM_THREDHOLD)
        {
            ADS_StartTimer(ACPU_PID_ADS_UL, TI_ADS_UL_SEND, ADS_UL_GET_PROTECT_TIMER_LEN());

            /* 设置发送结束标志 */
            ADS_UL_SET_SENDING_FLAG(VOS_FALSE);

            break;
        }
        else
        {
            continue;
        }
    }

    return;
}

/*****************************************************************************
 函 数 名  : ADS_UL_SendCdsStopSendDataRsp
 功能描述  : ADS给CDS回复ID_CDS_ADS_STOP_SENDDATA_RSP消息
 输入参数  : MODEM_ID_ENUM_UINT16                enModemId,
             VOS_UINT8                           ucRabId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月13日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数
  2.日    期   : 2012年12月14日
    作    者   : l60609
    修改内容   : DSDA Phase II
*****************************************************************************/
VOS_VOID ADS_UL_SendCdsStopSendDataRsp(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId
)
{
    VOS_UINT32                          ulResult;
    CDS_ADS_STOP_SENDDATA_RSP_STRU     *pstStopSendDataRsp;

    /* 申请消息 */
    pstStopSendDataRsp = (CDS_ADS_STOP_SENDDATA_RSP_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ACPU_PID_ADS_UL,
                                             sizeof(CDS_ADS_STOP_SENDDATA_RSP_STRU));

    /* 内存申请失败，返回 */
    if( VOS_NULL_PTR == pstStopSendDataRsp )
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendCdsStopSendDataRsp: pstStopSendDataRsp is null!");
        return;
    }

    PS_MEM_SET((VOS_INT8 *)pstStopSendDataRsp + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(CDS_ADS_STOP_SENDDATA_RSP_STRU) - VOS_MSG_HEAD_LENGTH));

    /*填写消息内容*/
    pstStopSendDataRsp->ulReceiverPid = UEPS_PID_CDS;
    pstStopSendDataRsp->enMsgId       = ID_CDS_ADS_STOP_SENDDATA_RSP;
    /* Modified by l60609 for DSDA Phase II, 2012-12-14, Begin */
    pstStopSendDataRsp->enModemId     = enModemId;
    pstStopSendDataRsp->ucRabId       = ucRabId;
    /* Modified by l60609 for DSDA Phase II, 2012-12-14, End */

    /* 调用VOS发送原语 */
    ulResult = PS_SEND_MSG(ACPU_PID_ADS_UL, pstStopSendDataRsp);

    if(VOS_OK != ulResult)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendCdsStopSendDataRsp: Send Msg Fail!");
    }

    return;
}

/*****************************************************************************
 函 数 名  : ADS_UL_SendCdsStartSendDataRsp
 功能描述  : ADS给CDS回复ID_CDS_ADS_START_SENDDATA_RSP消息
 输入参数  : MODEM_ID_ENUM_UINT16                enModemId,
             VOS_UINT8                           ucRabId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月13日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数
  2.日    期   : 2012年12月14日
    作    者   : l60609
    修改内容   : DSDA Phase II
*****************************************************************************/
VOS_VOID ADS_UL_SendCdsStartSendDataRsp(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId
)
{
    VOS_UINT32                          ulResult;
    CDS_ADS_START_SENDDATA_RSP_STRU    *pstStartSendDataRsp;

    /*申请消息  */
    pstStartSendDataRsp = (CDS_ADS_START_SENDDATA_RSP_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ACPU_PID_ADS_UL,
                                             sizeof(CDS_ADS_START_SENDDATA_RSP_STRU));

    /* 内存申请失败，返回 */
    if( VOS_NULL_PTR == pstStartSendDataRsp )
    {
        return;
    }

    PS_MEM_SET((VOS_INT8 *)pstStartSendDataRsp + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(CDS_ADS_START_SENDDATA_RSP_STRU) - VOS_MSG_HEAD_LENGTH));

    /*填写消息内容*/
    pstStartSendDataRsp->ulReceiverPid = UEPS_PID_CDS;
    pstStartSendDataRsp->enMsgId       = ID_CDS_ADS_START_SENDDATA_RSP;
    /* Modified by l60609 for DSDA Phase II, 2012-12-14, Begin */
    pstStartSendDataRsp->enModemId     = enModemId;
    pstStartSendDataRsp->ucRabId       = ucRabId;
    /* Modified by l60609 for DSDA Phase II, 2012-12-14, End */

    /* 调用VOS发送原语 */
    ulResult = PS_SEND_MSG(ACPU_PID_ADS_UL, pstStartSendDataRsp);

    if(VOS_OK != ulResult)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendCdsStartSendDataRsp: Send Msg Fail!");
    }

    return;
}

/*****************************************************************************
 函 数 名  : ADS_UL_SendCdsClearDataRsp
 功能描述  : ADS给CDS回复ID_CDS_ADS_CLEAR_DATA_RSP消息
 输入参数  : MODEM_ID_ENUM_UINT16                enModemId,
             VOS_UINT8                           ucRabId
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月13日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID ADS_UL_SendCdsClearDataRsp(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId
)
{
    VOS_UINT32                          ulResult;
    CDS_ADS_CLEAR_DATA_RSP_STRU        *pstClearDataRsp;

    /*申请消息  */
    pstClearDataRsp = (CDS_ADS_CLEAR_DATA_RSP_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ACPU_PID_ADS_UL,
                                             sizeof(CDS_ADS_CLEAR_DATA_RSP_STRU));

    /* 内存申请失败，返回 */
    if( VOS_NULL_PTR == pstClearDataRsp )
    {
        return;
    }

    PS_MEM_SET((VOS_INT8 *)pstClearDataRsp + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(CDS_ADS_CLEAR_DATA_RSP_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    pstClearDataRsp->ulReceiverPid = UEPS_PID_CDS;
    pstClearDataRsp->enMsgId       = ID_CDS_ADS_CLEAR_DATA_RSP;
    /* Modified by l60609 for DSDA Phase II, 2012-12-14, Begin */
    pstClearDataRsp->enModemId     = enModemId;
    /* Modified by l60609 for DSDA Phase II, 2012-12-14, End */
    pstClearDataRsp->ucRabId       = ucRabId;

    /* 调用VOS发送原语 */
    ulResult = PS_SEND_MSG(ACPU_PID_ADS_UL, pstClearDataRsp);

    if(VOS_OK != ulResult)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendCdsClearDataRsp: Send Msg Fail!");
    }

    return;
}

/* Modified by l60609 for L-C互操作项目, 2014-1-14, begin */
/*****************************************************************************
 函 数 名  : ADS_UL_ProcPdpStatusInd
 功能描述  : 处理PDP状态变化函数
 输入参数  : MsgBlock *pMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年1月7日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 ADS_UL_ProcPdpStatusInd(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId,
    ADS_PDP_STATUS_ENUM_UINT8           enPdpStatus,
    ADS_QCI_TYPE_ENUM_UINT8             enQciType,
    ADS_PDP_TYPE_ENUM_UINT8             enPdpType
)
{
    VOS_UINT8                           ucInstanceIndex;
    ADS_CDS_IPF_PKT_TYPE_ENUM_UINT8     enPktType;
    ADS_QCI_TYPE_ENUM_UINT8             enPrio;

    enPktType                           = ADS_CDS_IPF_PKT_TYPE_IP;

    ucInstanceIndex                     = (VOS_UINT8)enModemId;

    enPrio                              = enQciType;

    /* RabId合法性检查 */
    if (VOS_OK != ADS_IsValidRabId(ucRabId))
    {
        ADS_WARNING_LOG1(ACPU_PID_ADS_UL, "ADS_UL_ProcPdpStatusInd: ucRabId is ", ucRabId);
        return VOS_ERR;
    }

    /* 如果不采用优先级，则修改所有PDP的QCI为相同优先级，根据排序算法这样可以使先激活的PDP优先处理 */
    if (VOS_FALSE == g_stAdsCtx.astAdsSpecCtx[ucInstanceIndex].stAdsUlCtx.stQueuePriNv.ulStatus)
    {
        enPrio = ADS_QCI_TYPE_QCI9_NONGBR;
    }

    if (ADS_PDP_PPP == enPdpType)
    {
        enPktType = ADS_CDS_IPF_PKT_TYPE_PPP;
    }

    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    /* 根据PDP状态分别进行处理 */
    switch(enPdpStatus)
    {
        /* PDP激活 */
        case ADS_PDP_STATUS_ACT:

            /* 创建缓存队列 */
            ADS_UL_CreateQueue(ucInstanceIndex, ucRabId, enPrio, enPktType);

            /* 启动流量统计 */
            ADS_UL_StartDsFlowStats(ucInstanceIndex, ucRabId);

            /* 启动上报统计信息定时器 */
            ADS_UL_StartRptStatsInfoTimer(ucInstanceIndex, ucRabId);

            break;

        /* PDP修改 */
        case ADS_PDP_STATUS_MODIFY:

            /* 将修改的队列信息更新到上行队列管理中 */
            ADS_UL_UpdateQueueInPdpModified(ucInstanceIndex, enPrio, ucRabId);

            break;

        /* PDP去激活 */
        case ADS_PDP_STATUS_DEACT:

            /* 销毁缓存队列 */
            ADS_UL_DestroyQueue(ucInstanceIndex, ucRabId);

            /* Added by l60609 for AP适配项目 ，2012-08-31 Begin */
#if(FEATURE_OFF == FEATURE_SKB_EXP)
            /* 当所有的PDP都去激活后，清空源内存队列 */
            ADS_UL_ClearIpfUlSrcMem();
#endif
            /* Added by l60609 for AP适配项目 ，2012-08-31 End */

            /* 停止流量统计 */
            ADS_UL_StopDsFlowStats(ucInstanceIndex, ucRabId);

            /* 关闭上报统计信息定时器 */
            ADS_UL_StopRptStatsInfoTimer();

            break;

        default:
            break;
    }
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    return VOS_OK;
}


VOS_UINT32 ADS_UL_RcvTafPdpStatusInd(MsgBlock *pMsg)
{
    ADS_PDP_STATUS_IND_STRU            *pstPdpStatusInd;
    VOS_UINT32                          ulRslt;

    pstPdpStatusInd = (ADS_PDP_STATUS_IND_STRU *)pMsg;

    ulRslt = ADS_UL_ProcPdpStatusInd(pstPdpStatusInd->enModemId,
                                     pstPdpStatusInd->ucRabId,
                                     pstPdpStatusInd->enPdpStatus,
                                     pstPdpStatusInd->enQciType,
                                     pstPdpStatusInd->enPdpType);

    return ulRslt;
}
/* Modified by l60609 for L-C互操作项目, 2014-1-14, end */

/*****************************************************************************
 函 数 名  : ADS_UL_RcvCdsStopSendDataInd
 功能描述  : 处理ID_CDS_ADS_STOP_SENDDATA_IND消息
 输入参数  : MsgBlock *pMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月7日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

  2.日    期   : 2012年11月24日
    作    者   : l60609
    修改内容   : DSDA Phase II:双实例
*****************************************************************************/
VOS_UINT32 ADS_UL_RcvCdsStopSendDataInd(MsgBlock *pMsg)
{
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                           ucInstanceIndex;
    VOS_UINT8                           ucRabId;
    CDS_ADS_STOP_SENDDATA_IND_STRU     *pstStopSendDataIndMsg;

    pstStopSendDataIndMsg = (CDS_ADS_STOP_SENDDATA_IND_STRU *)(pMsg);

    ucInstanceIndex = (VOS_UINT8)pstStopSendDataIndMsg->enModemId;
    ucRabId = pstStopSendDataIndMsg->ucRabId;

    /* 将对应是否允许发送的标志置为VOS_FALSE */
    ADS_UL_SET_RAB_SND_PERMIT_FLAG(ucInstanceIndex, ucRabId, VOS_FALSE);

    /* 判断该modem的所有RAB都不允许发送 */
    if (VOS_FALSE == ADS_UL_GetSpecInstanceSndPermitFlg(ucInstanceIndex))
    {
        ADS_UL_SET_MODEM_SND_PERMIT_FLAG(ucInstanceIndex, VOS_FALSE);
    }

    /* 所有承载都不允许发送，停止ADS_UL_SEND_TIMER定时器 */
    if (VOS_TRUE == ADS_UL_IsAllRabNotSndPermitFlg())
    {
        ADS_StopTimer(ACPU_PID_ADS_UL, TI_ADS_UL_SEND, ADS_TIMER_STOP_CAUSE_USER);
    }

    /* 回复ID_CDS_ADS_STOP_SENDDATA_RSP消息 */
    ADS_UL_SendCdsStopSendDataRsp(pstStopSendDataIndMsg->enModemId, pstStopSendDataIndMsg->ucRabId);
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    return VOS_OK;
}


VOS_UINT32 ADS_UL_RcvCdsStartSendDataInd(MsgBlock *pMsg)
{
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                           ucInstanceIndex;
    VOS_UINT8                           ucRabId;
    CDS_ADS_START_SENDDATA_IND_STRU    *pstStartSendDataIndMsg;

    pstStartSendDataIndMsg = (CDS_ADS_START_SENDDATA_IND_STRU *)(pMsg);

    ucInstanceIndex = (VOS_UINT8)pstStartSendDataIndMsg->enModemId;
    ucRabId = (VOS_UINT8)pstStartSendDataIndMsg->ucRabId;

    /* 将对应是否允许发送的标志置为VOS_TRUE */
    ADS_UL_SET_RAB_SND_PERMIT_FLAG(ucInstanceIndex, ucRabId, VOS_TRUE);

    ADS_UL_SET_MODEM_SND_PERMIT_FLAG(ucInstanceIndex, VOS_TRUE);

    /* 回复ID_CDS_ADS_START_SENDDATA_RSP消息 */
    ADS_UL_SendCdsStartSendDataRsp(pstStartSendDataIndMsg->enModemId, pstStartSendDataIndMsg->ucRabId);
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    /* 触发上行缓存处理事件 */
    ADS_UL_SndEvent(ADS_UL_EVENT_DATA_PROC);

    return VOS_OK;
}
VOS_UINT32 ADS_UL_RcvCdsClearDataInd(MsgBlock *pMsg)
{
    VOS_UINT8                           ucRabId;
    ADS_UL_CTX_STRU                    *pstAdsUlCtx;
    CDS_ADS_CLEAR_DATA_IND_STRU        *pstClearData;
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                           ucInstanceIndex;

    pstClearData = (CDS_ADS_CLEAR_DATA_IND_STRU *)pMsg;

    /* AT接口修改后，获取此值 */
    ucInstanceIndex = (VOS_UINT8)pstClearData->enModemId;

    pstAdsUlCtx = ADS_GetUlCtx(ucInstanceIndex);

    ucRabId = pstClearData->ucRabId;

    if (CDS_ADS_ALL_RABID == pstClearData->ucRabId)
    {
        for (ucRabId = ADS_RAB_ID_MIN; ucRabId <= ADS_RAB_ID_MAX; ucRabId++)
        {
            if (VOS_OK == ADS_UL_IsQueueExistent(ucInstanceIndex, ucRabId))
            {
                /* 销毁队列中的数据 */
                /* Modified by l60609 for AP适配项目 ，2012-09-10 Begin */
                ADS_UL_ClearQueue(ucRabId,
                                  pstAdsUlCtx->astAdsUlQueue[ucRabId].pstAdsUlLink);
                /* Modified by l60609 for AP适配项目 ，2012-09-10 End */
            }
        }
    }
    else
    {
        /* 判断RabId是否为有效值 */
        if (VOS_OK == ADS_IsValidRabId(pstClearData->ucRabId))
        {
            /* 结点存在，但队列不存在 */
            if (VOS_OK == ADS_UL_IsQueueExistent(ucInstanceIndex, pstClearData->ucRabId))
            {
                /* 销毁队列中的数据 */
                /* Modified by l60609 for AP适配项目 ，2012-09-10 Begin */
                ADS_UL_ClearQueue(ucRabId,
                                  pstAdsUlCtx->astAdsUlQueue[ucRabId].pstAdsUlLink);
                /* Modified by l60609 for AP适配项目 ，2012-09-10 End */
            }
            else
            {
                ADS_WARNING_LOG(ACPU_PID_ADS_UL, "ADS_UL_RcvCdsClearDataInd:Queue is not exist");
            }
        }
        else
        {
            ADS_WARNING_LOG1(ACPU_PID_ADS_UL, "ADS_UL_RcvCdsClearDataInd: Invalid RabId is", pstClearData->ucRabId);
        }
    }

    /* 回复ID_CDS_ADS_CLEAR_DATA_RSP消息 */
    ADS_UL_SendCdsClearDataRsp(pstClearData->enModemId, pstClearData->ucRabId);
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : ADS_UL_RcvCdsIpPacketMsg
 功能描述  : 接收ID_CDS_ADS_IP_PACKET_IND消息
 输入参数  : MsgBlock *pMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月7日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

  2.日    期   : 2012年8月31日
    作    者   : l60609
    修改内容   : AP适配项目：IMM接口修改

  3.日    期   : 2012年12月21日
    作    者   : l60609
    修改内容   : DSDA Phase II
*****************************************************************************/
VOS_UINT32 ADS_UL_RcvCdsIpPacketMsg(MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    ADS_NDIS_DATA_IND_STRU             *pstAdsNdisDataInd;
    IMM_ZC_STRU                        *pstZcData;
    CDS_ADS_DATA_IND_STRU              *pstDataInd;
    VOS_CHAR                           *pstZcPutData;

    pstDataInd = (CDS_ADS_DATA_IND_STRU *)pMsg;

    /* 申请消息  */
    pstAdsNdisDataInd = (ADS_NDIS_DATA_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ACPU_PID_ADS_UL,
                                             sizeof(ADS_NDIS_DATA_IND_STRU));

    if (VOS_NULL_PTR == pstAdsNdisDataInd)
    {
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8 *)pstAdsNdisDataInd + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(ADS_NDIS_DATA_IND_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    pstAdsNdisDataInd->ulReceiverPid  = PS_PID_APP_NDIS;
    pstAdsNdisDataInd->enMsgId        = ID_ADS_NDIS_DATA_IND;
    pstAdsNdisDataInd->enModemId      = pstDataInd->enModemId;
    pstAdsNdisDataInd->ucRabId        = pstDataInd->ucRabId;
    pstAdsNdisDataInd->enIpPacketType = pstDataInd->enIpPacketType;

    pstZcData = (IMM_ZC_STRU *)IMM_ZcStaticAlloc((VOS_UINT32)pstDataInd->usLen);

    if (VOS_NULL_PTR == pstZcData)
    {
        PS_MEM_FREE(ACPU_PID_ADS_UL, pstAdsNdisDataInd);

        return VOS_ERR;
    }

    /* Modified by l60609 for DSDA Phase II, 2012-12-21, Begin */

    /*此步骤不能少，用来偏移数据尾指针*/
    /* Modified by l60609 for AP适配项目 ，2012-08-30 Begin */
    pstZcPutData = (VOS_CHAR *)IMM_ZcPut(pstZcData, pstDataInd->usLen);
    /* Modified by l60609 for AP适配项目 ，2012-08-30 End */

    PS_MEM_CPY(pstZcPutData, pstDataInd->aucData, pstDataInd->usLen);
    /* Modified by l60609 for DSDA Phase II, 2012-12-21, End */

    pstAdsNdisDataInd->pstSkBuff = pstZcData;

    /* 调用VOS发送原语 */
    ulResult = PS_SEND_MSG(ACPU_PID_ADS_UL, pstAdsNdisDataInd);

    if(VOS_OK != ulResult)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_RcvCdsIpPacketMsg: Send Msg Fail!");

        IMM_ZcFree(pstZcData);

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 ADS_UL_RcvCcpuResetStartInd(
    MsgBlock                           *pstMsg
)
{
    VOS_UINT8                           ucInsIndex;
    VOS_UINT8                           ucRabIndex;
    VOS_UINT8                           ucTiIndex;
    ADS_CTX_STRU                       *pstAdsCtx = VOS_NULL_PTR;

    pstAdsCtx = ADS_GetAllCtx();

    /* 清空所有上行缓存队列 */
    for (ucInsIndex = 0; ucInsIndex < ADS_INSTANCE_MAX_NUM; ucInsIndex++)
    {
        for (ucRabIndex = 0; ucRabIndex < ADS_RAB_ID_MAX + 1; ucRabIndex++)
        {
            ADS_UL_DestroyQueue(ucInsIndex, ucRabIndex);
        }
    }

#if (FEATURE_OFF == FEATURE_SKB_EXP)
    /* 清空源内存队列 */
    ADS_UL_ClearIpfUlSrcMem();
#endif

    /* 停止所有启动的定时器 */
    for (ucTiIndex = 0; ucTiIndex < ADS_MAX_TIMER_NUM; ucTiIndex++)
    {
        ADS_StopTimer(ACPU_PID_ADS_UL, ucTiIndex, ADS_TIMER_STOP_CAUSE_USER);
    }

    /* 初始化每个实例的上下文 */
    ADS_ResetSpecCtx();

    /* 重置IPF相关的上下文 */
    ADS_ResetIpfCtx();

    /* 重置当前实例索引值 */
    pstAdsCtx->ucAdsCurInstanceIndex = ADS_INSTANCE_INDEX_0;

    /* 重置ADS Filter过滤上下文 */
    ADS_FILTER_Reset();

    ADS_ResetDebugInfo();

    /* 释放信号量，使得调用API任务继续运行 */
    VOS_SmV(ADS_GetULResetSem());

    return VOS_OK;
}


VOS_VOID ADS_UL_RcvTiDsFlowStatsExpired(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT32                          ulTaBytes;
    VOS_UINT32                          ulRate;
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, Begin */
    VOS_UINT8                           ucInstance;

    ucInstance = ADS_INSTANCE_INDEX_0;

    /* 根据ulTimerName获取实例index */
    if (TI_ADS_DSFLOW_STATS_0 != ulTimerName)
    {
        ucInstance = (ucInstance + 1) % ADS_INSTANCE_MAX_NUM;
    }

    /* 如果没有上行队列存在, 无需统计流量 */
    if (VOS_FALSE == ADS_UL_IsAnyQueueExist(ucInstance))
    {
        ADS_NORMAL_LOG(ACPU_PID_ADS_DL, "ADS_UL_RcvTiDsFlowStatsExpired: no queue is exist!");
        return;
    }

    /* 获取2秒的下行数据个数 */
    ulTaBytes = ADS_GET_DL_PERIOD_PKT_NUM(ucInstance);

    /* 每2秒钟计算一次,单位为:byte/s */
    ulRate = ulTaBytes>>1;
    ADS_SET_CURRENT_DL_RATE(ucInstance, ulRate);

    /* 获取2秒的上行流量 */
    ulTaBytes = ADS_GET_UL_PERIOD_PKT_NUM(ucInstance);

    /* 每2秒钟计算一次,单位为:byte/s */
    ulRate = ulTaBytes>>1;
    ADS_SET_CURRENT_UL_RATE(ucInstance, ulRate);

    /* 每个流量统计周期结束后，需要将周期统计Byte数清除 */
    ADS_CLEAR_UL_PERIOD_PKT_NUM(ucInstance);
    ADS_CLEAR_DL_PERIOD_PKT_NUM(ucInstance);
    /* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

    /* 启动流量统计定时器*/
    ADS_StartTimer(ACPU_PID_ADS_UL,
                   ulTimerName,
                   TI_ADS_DSFLOW_STATS_LEN);

    return ;
}


VOS_VOID ADS_UL_RcvTiRptStatsInfoExpired(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulParam
)
{
    ADS_LOG_RPT_STATS_INFO_STRU        *pstMsg = VOS_NULL_PTR;

    /* 将记录的统计信息通过SDT消息勾出来 */
    pstMsg = (ADS_LOG_RPT_STATS_INFO_STRU*)PS_MEM_ALLOC(ACPU_PID_ADS_UL, sizeof(ADS_LOG_RPT_STATS_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_RcvTiRptStatsInfoExpired: ERROR:Alloc Mem Fail");
        return;
    }

    PS_MEM_SET(pstMsg, 0, sizeof(ADS_LOG_RPT_STATS_INFO_STRU));

    pstMsg->stLogHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stLogHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stLogHeader.ulSenderPid     = ACPU_PID_ADS_UL;
    pstMsg->stLogHeader.ulReceiverPid   = ACPU_PID_ADS_UL;
    pstMsg->stLogHeader.ulLength        = sizeof(ADS_LOG_RPT_STATS_INFO_STRU) - VOS_MSG_HEAD_LENGTH;;
    pstMsg->stLogHeader.enMsgId         = ID_ADS_LOG_RPT_STATS_INFO;

    PS_MEM_CPY(&pstMsg->stAdsStats, &g_stAdsStats, sizeof(g_stAdsStats));

    OM_AcpuTraceMsgHook(pstMsg);

    PS_MEM_FREE(ACPU_PID_ADS_UL, pstMsg);

    /* 重新启动上报统计信息定时器 */
    ADS_StartTimer(ACPU_PID_ADS_UL, ulTimerName, TI_ADS_RPT_STATS_LEN);

    return ;
}



VOS_VOID ADS_UL_RcvTiDataStatExpired(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT32                          ulStatPktNum;

    ulStatPktNum = ADS_UL_GET_STAT_PKT_NUM();

    /* 根据数据包个数调整赞包门限 */
    if (ulStatPktNum < ADS_UL_GET_WATER_LEVEL_ONE())
    {
        ADS_UL_SET_SEND_DATA_NUM_THREDHOLD(ADS_UL_DATA_THRESHOLD_ONE);
        ADS_DBG_UL_LEVEL_ONE_CNT(1);
    }
    else if (ulStatPktNum <  ADS_UL_GET_WATER_LEVEL_TWO())
    {
        ADS_UL_SET_SEND_DATA_NUM_THREDHOLD(ADS_UL_DATA_THRESHOLD_TWO);
        ADS_DBG_UL_LEVEL_TWO_CNT(1);
    }
    else if (ulStatPktNum <  ADS_UL_GET_WATER_LEVEL_THREE())
    {
        ADS_UL_SET_SEND_DATA_NUM_THREDHOLD(ADS_UL_DATA_THRESHOLD_THREE);
        ADS_DBG_UL_LEVEL_THREE_CNT(1);
    }
    else
    {
        ADS_UL_SET_SEND_DATA_NUM_THREDHOLD(ADS_UL_DATA_THRESHOLD_FOUR);
        ADS_DBG_UL_LEVEL_FOUR_CNT(1);
    }

    /* 100ms内没有数据包则该定时器不再启动 */
    if (0 != ulStatPktNum)
    {
        /* 重新启动上行统计定时器 */
        ADS_StartTimer(ACPU_PID_ADS_UL, TI_ADS_UL_DATA_STAT, ADS_UL_GET_STAT_TIMER_LEN());
    }

    /* 清空统计包的个数 */
    ADS_UL_CLR_STAT_PKT_NUM();

    return;
}


VOS_UINT32 ADS_UL_RcvTafMsg(MsgBlock *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;


    switch(pstMsg->ulMsgName)
    {
        case ID_APS_ADS_PDP_STATUS_IND:
            ADS_UL_RcvTafPdpStatusInd(pMsg);
            break;

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : ADS_UL_RcvCdsMsg
 功能描述  : 接收来自CDS的消息
 输入参数  : MsgBlock *pMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月6日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 ADS_UL_RcvCdsMsg(MsgBlock *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    switch(pstMsg->ulMsgName)
    {
        case ID_CDS_ADS_STOP_SENDDATA_IND:
            ADS_UL_RcvCdsStopSendDataInd(pMsg);
            break;

        case ID_CDS_ADS_START_SENDDATA_IND:
            ADS_UL_RcvCdsStartSendDataInd(pMsg);
            break;

        case ID_CDS_ADS_CLEAR_DATA_IND:
            ADS_UL_RcvCdsClearDataInd(pMsg);
            break;

        case ID_CDS_ADS_IP_PACKET_IND:
            ADS_UL_RcvCdsIpPacketMsg(pMsg);
            break;

        default:
            break;
    }

    return VOS_OK;
}


VOS_UINT32 ADS_UL_RcvTimerMsg(MsgBlock *pMsg)
{
    REL_TIMER_MSG                      *pstTimerMsg;

    pstTimerMsg = (REL_TIMER_MSG *)pMsg;

    /* 停止该定时器 */
    ADS_StopTimer(ACPU_PID_ADS_UL, pstTimerMsg->ulName, ADS_TIMER_STOP_CAUSE_TIMEOUT);

    switch (pstTimerMsg->ulName)
    {
        /* 处理ADS_UL_SEND_TIMER定时器超时 */
        case TI_ADS_UL_SEND:
            /* 处理上行数据 */
            ADS_UL_ProcLinkData();
            break;

        case TI_ADS_DSFLOW_STATS_0:
        case TI_ADS_DSFLOW_STATS_1:
            ADS_UL_RcvTiDsFlowStatsExpired(pstTimerMsg->ulName, pstTimerMsg->ulPara);
            break;

        case TI_ADS_RPT_STATS_INFO:
            ADS_UL_RcvTiRptStatsInfoExpired(pstTimerMsg->ulName, pstTimerMsg->ulPara);
            break;

        case TI_ADS_UL_DATA_STAT:
            ADS_UL_RcvTiDataStatExpired(pstTimerMsg->ulName, pstTimerMsg->ulPara);
            break;

        default:
            break;
    }

    return VOS_OK;
}
VOS_UINT32 ADS_UL_RcvAdsUlMsg(MsgBlock *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    /* Modified by l60609 for L-C互操作项目, 2014-01-07, Begin */
    switch(pstMsg->ulMsgName)
    {
        case ID_ADS_CCPU_RESET_START_IND:
            ADS_UL_RcvCcpuResetStartInd(pMsg);
            break;

        case ID_ADS_CCPU_RESET_END_IND:

            /* do nothing */
            ADS_NORMAL_LOG(ACPU_PID_ADS_UL, "ADS_DL_RcvAdsDlMsg: rcv ID_CCPU_ADS_UL_RESET_END_IND");
            break;

        default:
            ADS_NORMAL_LOG1(ACPU_PID_ADS_UL, "ADS_UL_RcvAdsUlMsg: rcv error msg id %d\r\n", pstMsg->ulMsgName);
            break;
    }
    /* Modified by l60609 for L-C互操作项目, 2014-01-07, End */

    return VOS_OK;
}
VOS_VOID ADS_UL_ProcMsg(MsgBlock* pMsg)
{
    if (VOS_NULL_PTR == pMsg)
    {
        return;
    }

    /* 消息的分发处理 */
    switch ( pMsg->ulSenderPid )
    {
        /* 来自Timer的消息 */
        case VOS_PID_TIMER:
            ADS_UL_RcvTimerMsg(pMsg);
            return;

        /* 来自TAF的消息 */
        case I0_WUEPS_PID_TAF:
        case I1_WUEPS_PID_TAF:
            ADS_UL_RcvTafMsg(pMsg);
            return;


        /* 来自CDS的消息 */
        case UEPS_PID_CDS:
            ADS_UL_RcvCdsMsg(pMsg);
            return;

        /* 来自ADS UL的消息 */
        case ACPU_PID_ADS_UL:
            ADS_UL_RcvAdsUlMsg(pMsg);
            return;

        default:
            return;
    }
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
