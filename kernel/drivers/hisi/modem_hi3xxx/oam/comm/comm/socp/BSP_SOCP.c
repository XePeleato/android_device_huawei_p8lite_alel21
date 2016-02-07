/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_SOCP.c
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件主要完成socp通道的申请和配置，以及读写数据处理
*
*   修改记录 :  2011年2月19日  v1.00  yangzhi  创建
*************************************************************************/

#include <product_config.h>

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)

#include "BSP_SOCP_DRV.h"
#include "OamSpecTaskDef.h"
#include "omnvinterface.h"
#include "NVIM_Interface.h"
#include "om.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e767  修改人: m00128685；检视人：l46160 原因简述:打点日志文件宏ID定义*/
#define    THIS_FILE_ID        PS_FILE_ID_SOCP_C
/*lint +e767  修改人: m00128685；检视人：l46160 */

SOCP_GBLSTATE_STRU              g_strSocpStat;    /* 记录当前全局状态和通道配置信息 */

SOCP_DEBUGINFO_STRU             g_stSocpDebugInfo;      /* 记录在SOCP运行状态中的过程信息 */

SOCP_LOG_INFO_STRU              g_stSocpLog;            /* 记录在SOCP运行状态中的log信息 */

VOS_SPINLOCK                    g_stSocpSpinLock;       /* 自旋锁，用来作SOCP临界资源保护 */

#ifdef SOCP_ACORE
SOCP_ENC_DST_BUF_LOG_CFG_STRU   g_stEncDstBufLogConfig={0}; /* LOG2.0 SOCP BUFFER大小、地址、超时配置、使能标志 */

VOS_SPINLOCK                    g_stSocpFlushSpinLock;  /* 自旋锁，用来作SOCP Flush EncDst 临界资源保护 */
#endif

/*lint -e662 -e661*/
/*****************************************************************************
* 函 数 名  : OM_Inner_Log
*
* 功能描述  : 记录OM的内部log数据
*
* 输入参数  :  pucStr       OM的输出字符串信息
               ulP1         保存参数1
               ulP2         保存参数2
               pstLogData   保存数据的Log缓冲区
               ulFileID     保存数据产生的文件
               lLineNo      保存数据产生的行号
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
VOS_VOID OM_Inner_Log(const VOS_CHAR         *pucStr,
                            VOS_UINT32              ulP1,
                            VOS_UINT32              ulP2,
                            INNER_LOG_DATA_STRU     *pstLogData ,
                            VOS_UINT32              ulFileID,
                            VOS_INT32               lLineNo)
{
    VOS_UINT32  ulCounter;

#ifdef OM_DEBUG_PRINT
    vos_printf("\r\n T:0x%x ", OM_GetSlice());
    vos_printf("%s ",pucStr);
    vos_printf("F:%d,L:%d,P1:0x%x,P2:0x%x",ulFileID,lLineNo,ulP1,ulP2);
#endif  /* end of OM_DEBUG_PRINT */

    ulCounter = pstLogData->ulCnt;

    pstLogData->astLogData[ulCounter].ulFileNO  = ulFileID;
    pstLogData->astLogData[ulCounter].lLineNO   = lLineNo;
    pstLogData->astLogData[ulCounter].ulP1      = ulP1;
    pstLogData->astLogData[ulCounter].ulP2      = ulP2;
#ifdef SOCP_ACORE
    pstLogData->astLogData[ulCounter].ulSlice   = OM_GetSlice();
#endif

#ifdef SOCP_CCORE
    /* 因为涉及寄存器访问，因此每16次读一次slice值 */
    if (0 == ulCounter%16)
    {
        pstLogData->astLogData[ulCounter].ulSlice   = OM_GetSlice();
    }
#endif

    pstLogData->ulCnt                           = (ulCounter+1)%INNER_LOG_DATA_MAX;

    return;
}

/*****************************************************************************
* 函 数 名  : SOCP_REG_SAVE
*
* 功能描述  : 单板出现异常，保存当前的SOCP的寄存器信息
*
* 输入参数  :  ulErrorNO       异常错误号
               ulPara          错误参数
               pucData         错误数据
               ulDataLen       错误数据长度
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
VOS_VOID SOCP_Log_Save(cb_buf_t *pstData)
{
    VOS_UINT8       *pucData;
    VOS_UINT32      ulDataLen;

    if(VOS_TRUE != g_strSocpStat.bInitFlag) /*初始化未完成不保存*/
    {
        pstData->ulDataLen = 0;
        VOS_MemSet(pstData->aucName, 0, EXCH_CB_NAME_SIZE);

        return;
    }

    /* 填写数据信息 */
    VOS_StrNCpy(pstData->aucName, "SOCP LOG Info", EXCH_CB_NAME_SIZE);

    /* 计算需要保存的log全局变量的大小,多申请内存为分割标记字段 */
    ulDataLen = SOCP_REGISTER_SIZE + sizeof(g_strSocpStat)+sizeof(g_stSocpLog)+(3*sizeof(VOS_UINT32));

    /* 申请内存 */
#ifdef OAM_UT_DEBUG     /* only for UT */
    pucData = (VOS_UINT8*)VOS_MemAlloc(0,0,ulDataLen);
#else
#if (VOS_OS_VER == VOS_LINUX)
    pucData = (VOS_UINT8*)kmalloc(ulDataLen, GFP_KERNEL);
#elif (VOS_OS_VER == VOS_RTOSCK)
    pucData = (VOS_UINT8*)SRE_MemCacheAlloc(ulDataLen, MEM_ADDR_ALIGN_004);
#else
    pucData = (VOS_UINT8*)malloc(ulDataLen);
#endif  /*(VOS_OS_VER == VOS_LINUX)*/
#endif  /*OAM_UT_DEBUG*/

    /*内存申请失败保存部分信息*/
    if(VOS_NULL_PTR == pucData)
    {
        pstData->pucData    = (VOS_UINT8*)&g_stSocpLog;
        pstData->ulDataLen  = sizeof(g_stSocpLog);
    }
    else    /*复制数据内容*/
    {
        /*lint -e810*/
        pstData->pucData    = pucData;

        pstData->ulDataLen  = ulDataLen;

        /*保存当前IP REG信息*/
        VOS_MemSet(pucData, SOCP_ERROR_INFO_TAG, sizeof(VOS_UINT32));

        pucData += sizeof(VOS_UINT32);

        VOS_MemCpy(pucData, (unsigned char *)g_strSocpStat.uBaseAddr, SOCP_REGISTER_SIZE);

        pucData += SOCP_REGISTER_SIZE;

        /*保存当前通道控制信息*/
        VOS_MemSet(pucData, SOCP_ERROR_INFO_TAG, sizeof(VOS_UINT32));

        pucData += sizeof(VOS_UINT32);

        VOS_MemCpy(pucData, &g_strSocpStat, sizeof(g_strSocpStat));

        pucData += sizeof(g_strSocpStat);

        /*保存当前通道log信息*/
        VOS_MemSet(pucData, SOCP_ERROR_INFO_TAG, sizeof(VOS_UINT32));

        pucData += sizeof(VOS_UINT32);

        VOS_MemCpy(pucData, &g_stSocpLog, sizeof(g_stSocpLog));
        /*lint +e810*/
    }

    return; /*返回，单板重启不需要释放内存*/
}

/*****************************************************************************
* 函 数 名  : SocpGetIdleBuffer
*
* 功能描述  : 查询空闲缓冲区
*
* 输入参数  :  pRingBuffer       待查询的环形buffer
               pRWBuffer         输出的环形buffer
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
VOS_VOID SocpGetIdleBuffer(SOCP_RING_BUF_STRU   *pRingBuffer,
                                 SOCP_BUFFER_RW_STRU  *pRWBuffer)
{
    if(pRingBuffer->u32Write < pRingBuffer->u32Read)
    {
        /* 读指针大于写指针，直接计算 */
        pRWBuffer->pBuffer = (VOS_UINT8 *)(pRingBuffer->u32Write);
        pRWBuffer->u32Size = (VOS_UINT32)(pRingBuffer->u32Read - pRingBuffer->u32Write - 1);
        pRWBuffer->pRbBuffer = (VOS_UINT8 *)VOS_NULL_PTR;
        pRWBuffer->u32RbSize = 0;
    }
    else
    {
        /* 写指针大于读指针，需要考虑回卷 */
        if(pRingBuffer->u32Read != pRingBuffer->u32Start)
        {
            pRWBuffer->pBuffer = (VOS_UINT8 *)(pRingBuffer->u32Write);
            pRWBuffer->u32Size = (VOS_UINT32)(pRingBuffer->u32End - pRingBuffer->u32Write + 1);
            pRWBuffer->pRbBuffer = (VOS_UINT8 *)(pRingBuffer->u32Start);
            pRWBuffer->u32RbSize = (VOS_UINT32)(pRingBuffer->u32Read - pRingBuffer->u32Start - 1);
        }
        else
        {
            pRWBuffer->pBuffer = (VOS_UINT8 *)(pRingBuffer->u32Write);
            pRWBuffer->u32Size = (VOS_UINT32)(pRingBuffer->u32End - pRingBuffer->u32Write);
            pRWBuffer->pRbBuffer = (VOS_UINT8 *)VOS_NULL_PTR;
            pRWBuffer->u32RbSize = 0;
        }
    }

    return;
}

/*****************************************************************************
* 函 数 名  : SocpGetDataBuffer
*
* 功能描述  : 获取空闲缓冲区的数据
*
* 输入参数  :  pRingBuffer       待查询的环形buffer
               pRWBuffer         输出的环形buffer
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
VOS_VOID SocpGetDataBuffer(SOCP_RING_BUF_STRU  *pRingBuffer,
                                  SOCP_BUFFER_RW_STRU *pRWBuffer)
{
    if (pRingBuffer->u32Read <= pRingBuffer->u32Write)
    {
        /* 写指针大于读指针，直接计算 */
        pRWBuffer->pBuffer   = (VOS_UINT8 *)(pRingBuffer->u32Read);
        pRWBuffer->u32Size   = (VOS_UINT32)(pRingBuffer->u32Write - pRingBuffer->u32Read);
        pRWBuffer->pRbBuffer = (VOS_UINT8 *)VOS_NULL_PTR;
        pRWBuffer->u32RbSize = 0;
    }
    else
    {
        /* 读指针大于写指针，需要考虑回卷 */
        pRWBuffer->pBuffer   = (VOS_UINT8 *)(pRingBuffer->u32Read);
        pRWBuffer->u32Size   = (VOS_UINT32)(pRingBuffer->u32End - pRingBuffer->u32Read + 1);
        pRWBuffer->pRbBuffer = (VOS_UINT8 *)(pRingBuffer->u32Start);
        pRWBuffer->u32RbSize = (VOS_UINT32)(pRingBuffer->u32Write - pRingBuffer->u32Start);
    }

    return;
}

/*****************************************************************************
* 函 数 名  : SocpWriteDone
*
* 功能描述  : 更新缓冲区的写指针
*
* 输入参数  :  pRingBuffer       待更新的环形buffer
               u32Size           更新的数据长度
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
VOS_VOID SocpWriteDone(SOCP_RING_BUF_STRU *pRingBuffer, VOS_UINT32 u32Size)
{
    VOS_UINT32                             tmp_size;
    VOS_UINT32                             rb_size;

    tmp_size = pRingBuffer->u32End - pRingBuffer->u32Write + 1;

    if (tmp_size > u32Size)
    {
        (VOS_VOID)SOCP_FLUSH_CACHE(pRingBuffer->u32Write, u32Size);
        pRingBuffer->u32Write += u32Size;
    }
    else
    {
        rb_size = u32Size - tmp_size;

        (VOS_VOID)SOCP_FLUSH_CACHE(pRingBuffer->u32Write, tmp_size);
        (VOS_VOID)SOCP_FLUSH_CACHE(pRingBuffer->u32Start, rb_size);
        pRingBuffer->u32Write = pRingBuffer->u32Start + rb_size;
    }

    return;
}

/*****************************************************************************
* 函 数 名  : SocpReadDone
*
* 功能描述  : 更新缓冲区的读指针
*
* 输入参数  :  pRingBuffer       待更新的环形buffer
               u32Size           更新的数据长度
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
VOS_VOID SocpReadDone(SOCP_RING_BUF_STRU *pRingBuffer, VOS_UINT32 u32Size)
{
    pRingBuffer->u32Read += u32Size;

    if (pRingBuffer->u32Read > pRingBuffer->u32End)
    {
        pRingBuffer->u32Read -= pRingBuffer->u32Length;
    }

    return;
}

/*****************************************************************************
* 函 数 名  : SOCP_CleanEncSrcChan
*
* 功能描述  : 清空编码源通道
*
* 输入参数  : enSrcChanID       编码通道号
*
* 输出参数  : VOS_OK/BSP_ERR_SOCP_INVALID_CHAN
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
VOS_UINT32 SOCP_CleanEncSrcChan(SOCP_CODER_SRC_ENUM_U32 enSrcChanID)
{
    VOS_UINT32                          ulResetFlag;
    VOS_UINT32                          i;
    VOS_UINT32                          ulChanID;
    VOS_UINT32                          ulChanType;

    ulChanID        = SOCP_REAL_CHAN_ID(enSrcChanID);
    ulChanType      = SOCP_REAL_CHAN_TYPE(enSrcChanID);

    SOCP_CHECK_CHAN_TYPE(ulChanType, SOCP_CODER_SRC_CHAN);

    /* 复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_ENCRST, ulChanID, 1, 1);

    /* 等待通道自清 */
    for(i=0; i< SOCP_RESET_TIME; i++)
    {
        ulResetFlag = SOCP_REG_GETBITS(SOCP_REG_ENCRST, ulChanID, 1);

#ifndef OAM_UT_DEBUG     /* only for UT */
        if(0 == ulResetFlag)
        {
            break;
        }
#endif
    }

    if(SOCP_RESET_TIME == i)
    {
        SOCP_LOG_WARNING("SocpCleanEncChan failed!\n", ulChanID, 0);
    }

    return VOS_OK;
}


#ifdef SOCP_ACORE
/*****************************************************************************
* 函 数 名  : SocpGetNoEmptyEncDstChan
*
* 功能描述  : SOCP编码目的通道是否有数据
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_U8 0:无数据 非0:对应通道置位
*****************************************************************************/
VOS_UINT8 SocpGetNoEmptyEncDstChan(VOS_VOID)
{
    VOS_UINT8                   ucChanSet = 0;
    SOCP_BUFFER_RW_STRU         stBuff;
    VOS_UINT32                  i;

    for(i=0; i<SOCP_MAX_ENCDST_CHN; i++)
    {
        VOS_MemSet(&stBuff, 0x0, sizeof(SOCP_BUFFER_RW_STRU));

        if(VOS_OK != BSP_SOCP_GetReadBuff(SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, i), &stBuff))
        {
            continue;
        }

        if(stBuff.u32Size + stBuff.u32RbSize > 0)
        {
            ucChanSet = (VOS_UINT8)(ucChanSet | (1 << i));
        }
    }

    return ucChanSet;
}


VOS_UINT32  SOCP_MsConvertToCycle(VOS_UINT32 ulDelayTimeInMs)
{
    /* ulDelayTimeInMs最大为1200000ms，ulDelayTimeInMs * 2289结果最大为32位数据，故可以采用如下算法计算 */
    return ((ulDelayTimeInMs * 2289) / 1000);
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_GetSDLogCfg
*
* 功能描述  : 查询刷新后LOG2.0 SOCP超时配置信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : SOCP_ENC_DST_BUF_LOG_CFG_STRU指针
*****************************************************************************/
VOS_UINT32 BSP_SOCP_GetSDLogCfg(VOS_UINT32 *pulEnable, VOS_UINT32 *pulTimerLen, VOS_UINT32 *pulBufferSize)
{
    if ((VOS_NULL_PTR == pulTimerLen) || (VOS_NULL_PTR == pulEnable) || (VOS_NULL_PTR == pulBufferSize))
    {
        return VOS_ERR;
    }

    *pulTimerLen = g_stEncDstBufLogConfig.ulCurTimeout;

    if (SOCP_ENC_DEST_LOG_DELAY_ON == g_stEncDstBufLogConfig.logOnFlag)
    {
        *pulEnable = VOS_TRUE;
    }
    else
    {
        *pulEnable = VOS_FALSE;
    }

    *pulBufferSize = g_stEncDstBufLogConfig.BufferSize;

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_SetLogIntTimeOut
*
* 功能描述  : 使用默认的超时时间来设置LOG2.0 SOCP超时配置
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
VOS_VOID BSP_SOCP_SetLogIntTimeOutDefautl(VOS_VOID)
{
    if (SOCP_ENC_DEST_LOG_DELAY_ON == g_stEncDstBufLogConfig.logOnFlag)
    {
        SOCP_REG_WRITE(SOCP_REG_INTTIMEOUT, SOCP_TRANS_TIMEOUT_DEFAULT);    /* 超时传输中断超时配置 */

        /* 设置SOCP的超时时间为10MS */
        g_stEncDstBufLogConfig.ulCurTimeout = 10;
    }

    return;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_SetLogIntTimeOut
*
* 功能描述  : 使用CMD LINE中的超时时间来设置LOG2.0 SOCP超时配置
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
VOS_VOID BSP_SOCP_SetLogIntTimeOut(VOS_VOID)
{
    if (SOCP_ENC_DEST_LOG_DELAY_ON == g_stEncDstBufLogConfig.logOnFlag)
    {
        SOCP_REG_WRITE(SOCP_REG_INTTIMEOUT, SOCP_MsConvertToCycle(g_stEncDstBufLogConfig.overTime));

        /* 设置SOCP的超时时间为DTS中的超时时间 */
        g_stEncDstBufLogConfig.ulCurTimeout = g_stEncDstBufLogConfig.overTime;
    }

    return;
}


/*****************************************************************************
* 函 数 名  : SocpGetLogCfg
*
* 功能描述  : 获取LOG2.0 SOCP水线、超时配置信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : SOCP_ENC_DST_BUF_LOG_CFG_STRU指针
*****************************************************************************/
SOCP_ENC_DST_BUF_LOG_CFG_STRU * SocpGetSDLogCfg(VOS_VOID)
{
    return &g_stEncDstBufLogConfig;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_EncDstBufFlush
*
* 功能描述  : SOCP编码目的buffer缓存数据立即输出
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : VOS_INT32 VOS_OK:成功 MDRV_ERROR:失败
*****************************************************************************/
VOS_UINT32 BSP_SOCP_EncDstBufFlush(VOS_VOID)
{
    SOCP_ENC_DST_BUF_LOG_CFG_STRU   *stcfg;

    /* 读取配置 */
    stcfg = SocpGetSDLogCfg();

    /* LOG2.0未启动 */
    if (SOCP_ENC_DEST_LOG_DELAY_ON != stcfg->logOnFlag)
    {
        return VOS_ERR;
    }

    /* 让EncDstCh1任务处理SOCP数据 */
    if (VOS_OK != VOS_SmV(g_strSocpStat.u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(SOCP_CODER_DST_OM_IND)]))
    {
        return VOS_ERR;
    }

    g_stSocpDebugInfo.stSocpSDLogInfo[SOCP_REAL_CHAN_ID(SOCP_CODER_DST_OM_IND)].ulFlushBufferNum++;

    g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(SOCP_CODER_DST_OM_IND)].ulFlushNum++;
    g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(SOCP_CODER_DST_OM_IND)].ulFlushSlice = OM_GetSlice();

    return VOS_OK;
}


/*****************************************************************************
* 函 数 名  : SocpEncDstTaskProc
*
* 功能描述  : A核编码目的通道任务公共处理
* 输入参数  : ulRealChannelId - 实际编码目的通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SocpEncDstTaskProc(VOS_UINT32 ulRealChannelId)
{
    VOS_UINT32                          ulCBTime = 0;
    VOS_UINT32                          ulChanId;

    /* 检测通道是否配置*/
    if (SOCP_CHN_SET != g_strSocpStat.sEncDstChan[ulRealChannelId].u32SetStat)
    {
        return;
    }

    g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpEncDstTskTrfCbOriCnt[ulRealChannelId]++;

    if (VOS_NULL_PTR != g_strSocpStat.sEncDstChan[ulRealChannelId].read_cb)
    {
        ulChanId    = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, ulRealChannelId);

        ulCBTime    = OM_GetSlice();

        (VOS_VOID)g_strSocpStat.sEncDstChan[ulRealChannelId].read_cb(ulChanId);

        ulCBTime    = OM_GetSlice()-ulCBTime;     /* 获取回调函数的执行时间 */

        SOCP_LOG_INFO("SocpEncDstTaskProc Dst Channel CallBack",ulChanId, ulCBTime);
    }

    g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpEncDstTskTrfCbCnt[ulRealChannelId]++;

    return;
}


VOS_VOID SocpEncDstTaskChannel(
    VOS_UINT32      ulSocpEncDestCh,
    VOS_UINT32      ulPara2,
    VOS_UINT32      ulPara3,
    VOS_UINT32      ulPara4
)
{
    if (SOCP_CODER_DST_BUTT <= ulSocpEncDestCh)
    {
        SOCP_LOG_ERR("SocpEncDstTaskChannel: ulSocpEncDestCh is abnormal", VOS_NULL, VOS_NULL);
        return;
    }

    for(;;)
    {
        if(VOS_OK != VOS_SmP(g_strSocpStat.u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(ulSocpEncDestCh)], 0))
        {
#if defined(OAM_UT_DEBUG) || defined(DMT)   /*only for UT and ST*/
            return;
#endif
            g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulSocpEncDestCh)].ulTaskErrNum++;
            g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulSocpEncDestCh)].ulTaskErrSlice = OM_GetSlice();

            continue;
        }

        g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulSocpEncDestCh)].ulTaskNum++;
        g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulSocpEncDestCh)].ulTaskSlice = OM_GetSlice();


        SocpEncDstTaskProc(SOCP_REAL_CHAN_ID(ulSocpEncDestCh));

#if defined(OAM_UT_DEBUG) || defined(DMT)   /*only for UT and ST*/
        return;
#endif
    }
}

#if 0

/*****************************************************************************
* 函 数 名  : SocpDecDstTask
*
* 功能描述  : 模块任务函数:解码目的，双核
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SocpDecDstTask(VOS_VOID)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulIntTfrState = 0;
    VOS_UINT32                          ulIntOvfState = 0;
    VOS_UINT32                          ulChanId;
    VOS_ULONG                           ulLockLevel;
    VOS_UINT32                          ulCBTime = 0;

    for(;;)
    {
        if(VOS_OK != VOS_SmP(g_strSocpStat.u32DecDstSemID, 0))
        {
#if defined(OAM_UT_DEBUG) || defined(DMT)   /*only for UT and ST*/
            return;
#endif
            continue;
        }

        /*lLockKey                        = VOS_SplIMP();*/
        VOS_SpinLockIntLock(&g_stSocpSpinLock, ulLockLevel);

        ulIntTfrState                   = g_strSocpStat.u32IntDecDstTfr;
        g_strSocpStat.u32IntDecDstTfr   = 0;

        ulIntOvfState                   = g_strSocpStat.u32IntDecDstOvf;
        g_strSocpStat.u32IntDecDstOvf   = 0;

        /*VOS_Splx(lLockKey);*/
        VOS_SpinUnlockIntUnlock(&g_stSocpSpinLock, ulLockLevel);

        /* 处理解码传输完成中断*/
        if (0 != ulIntTfrState)
        {
            for (i=0; i<SOCP_MAX_DECDST_CHN; i++)
            {
                /* 检测通道是否申请*/
                if (SOCP_CHN_SET != g_strSocpStat.sDecDstChan[i].u32SetStat)
                {
                    continue;
                }

                if (0 != (ulIntTfrState & ((VOS_UINT32)1 << i)))
                {
                    g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpDecDstTskTrfCbOriCnt[i]++;

                    if (VOS_NULL_PTR != g_strSocpStat.sDecDstChan[i].read_cb)
                    {
                        ulChanId = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN, i);

                        ulCBTime = OM_GetSlice();

                        (VOS_VOID)g_strSocpStat.sDecDstChan[i].read_cb(ulChanId);

                        ulCBTime = OM_GetSlice()-ulCBTime;  /* 获取回调函数的执行时间 */

                        SOCP_LOG_INFO("SocpDecDstTask: Dst Trans CB", ulChanId, ulCBTime);
                    }

                    g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpDecDstTskTrfCbCnt[i]++;
                }
            }
        }
        /* 处理解码目的 buffer 溢出中断*/
        else if (0 != ulIntOvfState)
        {
            for (i=0; i<SOCP_MAX_DECDST_CHN; i++)
            {
                /* 检测通道是否申请*/
                if (SOCP_CHN_SET != g_strSocpStat.sDecDstChan[i].u32SetStat)
                {
                    continue;
                }

                if (0 != (ulIntOvfState & ((VOS_UINT32)1 << i)))
                {
                    g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpDecDstTskOvfCbOriCnt[i]++;

                    ulChanId = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN, i);

                    ulCBTime = OM_GetSlice();   /* 获取通道溢出时间 */

                    SOCP_LOG_WARNING("SocpDecDstTask: Dst Overflow CB", ulChanId, ulCBTime);

                    if (VOS_NULL_PTR != g_strSocpStat.sDecDstChan[i].read_cb)
                    {
                        (VOS_VOID)g_strSocpStat.sDecDstChan[i].read_cb(ulChanId);

                        ulCBTime = OM_GetSlice()-ulCBTime;  /* 获取回调函数的执行时间 */

                        SOCP_LOG_INFO("SocpDecDstTask: Dst Overflow CB Out", ulChanId, ulCBTime);
                    }

                    g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpDecDstTskOvfCbCnt[i]++;
                }
            }
        }
        else
        {

        }

#if defined(OAM_UT_DEBUG) || defined(DMT)   /*only for UT and ST*/
        return;
#endif
    }
}
#endif

/*****************************************************************************
* 函 数 名   : SocpHandlerEncSrc
*
* 功能描述  : 编码源通道处理函数
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/

/*lint -e774*/
VOS_VOID SocpHandlerEncSrc(VOS_VOID)
{
    VOS_UINT32  IntFlag     = 0;
    VOS_UINT32  IntState    = 0;
    VOS_UINT32  FlagMask    = 0;
    VOS_UINT32  FlagState   = 0;
    VOS_UINT32  IntMask     = 0;
    VOS_UINT32  i           = 0;

    FlagMask    = SOCP_APP_ENC_FLAGINT_MASK;
    FlagState   = SOCP_REG_APP_INTSTAT1;
    IntMask     = SOCP_REG_APP_MASK1;

    /*read and clear the interrupt flags*/
    SOCP_REG_READ(SOCP_REG_GBL_INTSTAT, IntFlag);

    if (IntFlag & FlagMask)
    {
        SOCP_REG_READ(FlagState, IntState);
        SOCP_REG_WRITE(SOCP_REG_ENC_RAWINT1, IntState);

        for (i=0; i<SOCP_MAX_ENCSRC_CHN; i++)
        {
            if (IntState & ((VOS_UINT32)1 << i))
            {
                if(SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(i), 31, 1))
                {
                    SOCP_REG_SETBITS(IntMask, i, 1, 1);
                }

                g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpEncSrcIsrHeadIntCnt[i]++;
            }
        }
    }

    return;
}

/*lint +e774*/

/*****************************************************************************
* 函 数 名   : SocpHandlerEncDstTrfInt
*
* 功能描述  : 编码目的传输中断处理函数
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SocpHandlerEncDstTrfInt(VOS_VOID)
{
    VOS_UINT32  IntState = 0;
    VOS_UINT32  i;
    VOS_UINT32  mask;
    VOS_UINT32  ulChannelId;

    SOCP_REG_READ(SOCP_REG_ENC_INTSTAT0, IntState);
    SOCP_REG_READ(SOCP_REG_ENC_MASK0, mask);
    SOCP_REG_WRITE(SOCP_REG_ENC_MASK0, (IntState | mask));   /* mask int 2011.7.27 by yangzhi */
    SOCP_REG_WRITE(SOCP_REG_ENC_RAWINT0, IntState);

    /*屏蔽阈值溢出中断，避免传输过程中上报，传输完成之后会再打开*/
    SOCP_REG_READ(SOCP_REG_ENC_MASK2, mask);
    SOCP_REG_WRITE(SOCP_REG_ENC_MASK2, (mask|((VOS_UINT32)(IntState<<SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN))));

    for (i = 0; i < SOCP_MAX_ENCDST_CHN; i++)
    {
        if (IntState & ((VOS_UINT32)1 << i))
        {
            if (0 == i)
            {
                /* 对应SOCP_CODER_DST_LOM_CNF通道 */
                ulChannelId = SOCP_CODER_DST_OM_CNF;

                g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntNum++;
                g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntSlice = OM_GetSlice();

                VOS_SmV(g_strSocpStat.u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(ulChannelId)]);
            }
            else if(1 == i)
            {
                /* 对应SOCP_CODER_DST_LOM_IND通道 */
                ulChannelId = SOCP_CODER_DST_OM_IND;

                g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntNum++;
                g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntSlice = OM_GetSlice();

                VOS_SmV(g_strSocpStat.u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(ulChannelId)]);
            }
            else
            {
                /* 其他通道不处理，记录异常 */
                g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpEncDstIsrTrfIntINvalidChannelCnt[i]++;
            }

            g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpEncDstIsrTrfIntCnt[i]++;
        }
    }

    return ;
}

/*****************************************************************************
* 函 数 名   : SocpHandlerEncDstOutOvfInt
*
* 功能描述  : 编码目的溢出中断处理函数
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SocpHandlerEncDstOutOvfInt(VOS_VOID)
{
    VOS_UINT32  IntState = 0;
    VOS_BOOL    bHandleChannel0 = VOS_FALSE;
    VOS_BOOL    bHandleChannel1 = VOS_FALSE;
    VOS_UINT32  i;
    VOS_UINT32  mask;
    VOS_UINT32  ulChannelId;

    SOCP_REG_READ(SOCP_REG_ENC_INTSTAT2, IntState);

    /* 编码目的buffer上溢中断 */
    if(0 != (IntState & SOCP_ENC_DST_BUFF_THRESHOLD_OVF_MASK))
    {
        SOCP_REG_READ(SOCP_REG_ENC_MASK2, mask);
        SOCP_REG_WRITE(SOCP_REG_ENC_MASK2, (IntState|mask));    /*屏蔽中断Mask*/
        SOCP_REG_WRITE(SOCP_REG_ENC_RAWINT2, IntState);         /*清空中断状态*/

        for (i = 0; i < SOCP_MAX_ENCDST_CHN; i++)
        {
            if (IntState & ((VOS_UINT32)1 << (i + SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN)))
            {
                if (0 == i)
                {
                    /* 对应SOCP_CODER_DST_LOM_CNF通道 */
                    bHandleChannel0 = VOS_TRUE;
                }
                else if(1 == i)
                {
                    /* 对应SOCP_CODER_DST_LOM_IND通道 */
                    bHandleChannel1 = VOS_TRUE;
                }
                else
                {
                    /* 其他通道不处理 */
                }

                g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpEncDstIsrThresholdOvfIntCnt[i]++;
            }
        }
    }

    if(bHandleChannel0)
    {
        ulChannelId = SOCP_CODER_DST_OM_CNF;

        g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntNum++;
        g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntSlice = OM_GetSlice();

        VOS_SmV(g_strSocpStat.u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(ulChannelId)]);
    }

    if(bHandleChannel1)
    {
        ulChannelId = SOCP_CODER_DST_OM_IND;

        g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntNum++;
        g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntSlice = OM_GetSlice();

        VOS_SmV(g_strSocpStat.u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(ulChannelId)]);
    }

    return ;
}

/*****************************************************************************
* 函 数 名   : SocpHandlerEncDst
*
* 功能描述  : 编码目的中断处理函数
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SocpHandlerEncDst(VOS_VOID)
{
    VOS_UINT32  IntFlag = 0;

    /*读取全局中断状态*/
    SOCP_REG_READ(SOCP_REG_GBL_INTSTAT, IntFlag);

    /*编码目的传输中断*/
    if (IntFlag & SOCP_APP_ENC_TFRINT_MASK)
    {
        SocpHandlerEncDstTrfInt();
    }
    /* 上溢中断与DMA溢出中断共用一个寄存器，DMA溢出中断不处理，由通道配置保证不会上报，
       通道溢出中断和传输中断同时就处理一个，先处理传输中断，存在溢出中断的时候不会有传输中断上报 */
    else if (IntFlag & SOCP_APP_ENC_OUTOVFINT_MASK)
    {
        SocpHandlerEncDstOutOvfInt();
    }
    else
    {
    }

    return ;
}

/*****************************************************************************
* 函 数 名   : SocpHandlerDecSrc
*
* 功能描述  : 解码源通道中断处理函数
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SocpHandlerDecSrc(VOS_VOID)
{
    VOS_UINT32  IntFlag  = 0;
    VOS_UINT32  IntState = 0;
    VOS_UINT32  ulmask;
    VOS_UINT32  i = 0;

    /*编码输入错误*/
    SOCP_REG_READ(SOCP_REG_GBL_INTSTAT, IntFlag);

    if (IntFlag & SOCP_DEC_INERRINT_MASK)
    {
        /* 清中断 */
        SOCP_REG_READ(SOCP_REG_DEC_INTSTAT1, IntState);
        SOCP_REG_WRITE(SOCP_REG_DEC_RAWINT1, IntState);

        /* 屏蔽解码源中断 */
        SOCP_REG_READ(SOCP_REG_DEC_MASK1, ulmask);
        SOCP_REG_WRITE(SOCP_REG_DEC_MASK1, (IntState | ulmask));

        for (i = 0; i < SOCP_MAX_DECSRC_CHN; i++)
        {
            if ((IntState >> i) & 0x1111110)
            {
                g_stSocpDebugInfo.sSocpDebugDecSrc.u32SocpDecSrcIsrErrIntCnt[i]++;
            }
        }
    }

    return;
}

/*****************************************************************************
* 函 数 名   : SocpHandlerDecDst
*
* 功能描述  : 解码目的通道中断处理函数
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/

/*lint -e774*/
VOS_VOID SocpHandlerDecDst(VOS_VOID)
{
    VOS_UINT32  IntFlag  = 0;
    VOS_UINT32  IntState = 0;
    VOS_UINT32  TfMask   = 0;
    VOS_UINT32  OvMask   = 0;
    VOS_UINT32  i        = 0;

    TfMask    = SOCP_CORE0_DEC_TFRINT_MASK;
    OvMask  = SOCP_CORE0_DEC_OUTOVFINT_MASK;

    /*解码传输中断*/
    SOCP_REG_READ(SOCP_REG_GBL_INTSTAT, IntFlag);
    if (IntFlag & TfMask)
    {
        VOS_UINT32 mask;

        SOCP_REG_READ(SOCP_REG_DEC_CORE0ISTAT0, IntState);
        IntState = IntState & 0xffff;
        SOCP_REG_READ(SOCP_REG_DEC_CORE0MASK0, mask);
        SOCP_REG_WRITE(SOCP_REG_DEC_CORE0MASK0, (IntState | mask));  /*added by yangzhi 2011.7.27 */
        SOCP_REG_WRITE(SOCP_REG_DEC_RAWINT0, IntState);

        SOCP_REG_READ(SOCP_REG_DEC_CORE0MASK2, mask);
        SOCP_REG_WRITE(SOCP_REG_DEC_CORE0MASK2, (IntState | mask)); /*屏蔽上溢中断*/

        for (i = 0; i < SOCP_MAX_DECDST_CHN; i++)
        {
            if (IntState & ((VOS_UINT32)1 << i))
            {
                g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpDecDstIsrTrfIntCnt[i]++;
            }
        }
    }
    /*解码目的buffer 上溢*/
    else if (IntFlag & OvMask)
    {
        SOCP_REG_READ(SOCP_REG_DEC_CORE0ISTAT2, IntState);
        IntState = IntState & 0xffff;
        SOCP_REG_WRITE(SOCP_REG_DEC_RAWINT2, IntState);

        SOCP_REG_WRITE(SOCP_REG_DEC_CORE0MASK2, IntState); /*屏蔽上溢中断*/

        for (i = 0; i < SOCP_MAX_DECDST_CHN; i++)
        {
            if (IntState & ((VOS_UINT32)1 << i))
            {
                g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpDecDstIsrOvfIntCnt[i]++;
            }
        }
    }
    else
    {
    }

    return;
}

/*lint +e774*/

/*****************************************************************************
* 函 数 名   : SocpAppInthandler
*
* 功能描述  : APP 核中断处理函数
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_UINT32 SocpAppInthandler(VOS_VOID)
{
    VOS_ULONG                           ulLockLevel;

    VOS_SpinLockIntLock(&g_stSocpSpinLock, ulLockLevel);

    g_stSocpDebugInfo.sSocpDebugGBl.u32SocpAppEtrIntCnt++;

    SocpHandlerEncSrc();

    SocpHandlerEncDst();

    SocpHandlerDecSrc();

    SocpHandlerDecDst();

    g_stSocpDebugInfo.sSocpDebugGBl.u32SocpAppSucIntCnt++;

    VOS_SpinUnlockIntUnlock(&g_stSocpSpinLock, ulLockLevel);

    return 1;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_DecoderSetDestChan
*
* 功能描述  : 解码目的通道申请及配置函数
*
* 输入参数  : pAttr           解码目的通道配置参数
*             pDestChanID     初始化解码目的通道ID
*
* 输出参数  :
*
* 返 回 值  : 申请及配置成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_DecoderSetDestChan(SOCP_DECODER_DST_ENUM_U32 enDestChanID,
                                                    SOCP_DECODER_DEST_CHAN_STRU *pAttr)
{
    VOS_UINT32             start;
    VOS_ULONG              ulStartTmp;
    VOS_UINT32             end;
    VOS_ULONG              ulEndTmp;
    VOS_UINT32             bufThreshold;
    VOS_UINT32             buflength;
    VOS_UINT32             u32ChanID;
    VOS_UINT32             u32SrcChanID;
    VOS_UINT32             u32ChanType;
    SOCP_DECDST_CHAN_STRU  *pChan;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pAttr);
    SOCP_CHECK_DATA_TYPE(pAttr->eDataType);

    u32ChanID    = SOCP_REAL_CHAN_ID(enDestChanID);
    u32ChanType  = SOCP_REAL_CHAN_TYPE(enDestChanID);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_DECODER_DEST_CHAN);
    SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_DECDST_CHN);

    u32SrcChanID = SOCP_REAL_CHAN_ID(pAttr->u32SrcChanID);
    u32ChanType  = SOCP_REAL_CHAN_TYPE(pAttr->u32SrcChanID);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_DECODER_SRC_CHAN);
    SOCP_CHECK_CHAN_ID(u32SrcChanID, SOCP_MAX_DECSRC_CHN);

    /* 判断给定的地址和长度是否为八字节倍数*/
    ulStartTmp   = (VOS_ULONG)pAttr->sDecoderDstSetBuf.pucOutputStart;
    start        = (VOS_UINT32)(ulStartTmp & 0xFFFFFFFFUL);
    ulEndTmp     = (VOS_ULONG)pAttr->sDecoderDstSetBuf.pucOutputEnd;
    end          = (VOS_UINT32)(ulEndTmp & 0xFFFFFFFFUL);
    bufThreshold    = pAttr->sDecoderDstSetBuf.u32Threshold;

    SOCP_CHECK_PARA(start);
    SOCP_CHECK_8BYTESALIGN(start);
    SOCP_CHECK_PARA(end);
    SOCP_CHECK_BUF_ADDR(start, end);
    SOCP_CHECK_PARA(bufThreshold);

    if (bufThreshold > SOCP_DEC_DST_TH_MAX)
    {
        SOCP_LOG_ERR("DecoderAllocDestChan: buffer threshold is too large", bufThreshold, 0);
        return BSP_ERR_SOCP_INVALID_PARA;
    }

    buflength = end - start + 1;

    SOCP_CHECK_8BYTESALIGN(buflength);

    if (buflength > SOCP_DEC_DST_BUFLGTH_MAX)
    {
        SOCP_LOG_ERR("DecoderAllocDestChan: buffer length is too large", buflength, 0);
        return BSP_ERR_SOCP_INVALID_PARA;
    }

    /* 写入起始地址到源buffer起始地址寄存器*/
    SOCP_REG_SETBITS(SOCP_REG_DECDEST_BUFCFG(u32ChanID), 24, 8, pAttr->eDataType);
    SOCP_REG_WRITE(SOCP_REG_DECDEST_BUFRPTR(u32ChanID), SOCP_VIRT_PHY(start));
    SOCP_REG_WRITE(SOCP_REG_DECDEST_BUFWPTR(u32ChanID), SOCP_VIRT_PHY(start));
    SOCP_REG_WRITE(SOCP_REG_DECDEST_BUFADDR(u32ChanID), SOCP_VIRT_PHY(start));
    SOCP_REG_SETBITS(SOCP_REG_DECDEST_BUFCFG(u32ChanID), 0, 16, buflength);
    SOCP_REG_SETBITS(SOCP_REG_DECDEST_BUFCFG(u32ChanID), 16, 8, bufThreshold);

    /* 保存参数 */
    pChan = &g_strSocpStat.sDecDstChan[u32ChanID];

    pChan->eDataType              = pAttr->eDataType;
    pChan->sDecDstBuf.u32Start    = start;
    pChan->sDecDstBuf.u32End      = end;
    pChan->sDecDstBuf.u32Length   = buflength;
    pChan->sDecDstBuf.u32Read     = start;
    pChan->sDecDstBuf.u32Write    = start;

    /* 通道已经配置 */
    pChan->u32SetStat             = SOCP_CHN_SET;

    /* 先清中断，再打开中断*/
    SOCP_REG_SETBITS(SOCP_REG_DEC_RAWINT0,      u32ChanID, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK0,   u32ChanID, 1, 0);

    SOCP_REG_SETBITS(SOCP_REG_DEC_RAWINT2,      u32ChanID, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK2,   u32ChanID, 1, 0);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_CoderSetDestChanAttr
*
* 功能描述  : 编码目的通道配置函数
*
* 输入参数  : u32DestChanID      编码目的通道ID
              pDestAttr          编码目的通道配置参数
*
* 输出参数  : 无
* 返 回 值  : 配置成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_CoderSetDestChanAttr(VOS_UINT32                 u32DestChanID,
                                         SOCP_CODER_DEST_CHAN_S    *pDestAttr)
{
    VOS_UINT32             start;
    VOS_ULONG              ulStartTmp;
    VOS_UINT32             end;
    VOS_ULONG              ulEndTmp;
    VOS_UINT32             bufThreshold;
    VOS_UINT32             buflength;
    VOS_UINT32             u32ChanID;
    VOS_UINT32             u32ChanType;
    VOS_UINT32             u32Thrh;
    SOCP_ENCDST_CHAN_STRU  *pChan;
    VOS_UINT32             ulVersion;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pDestAttr);

    u32ChanID   = SOCP_REAL_CHAN_ID(u32DestChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32DestChanID);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_CODER_DEST_CHAN);
    SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_ENCDST_CHN);

    ulStartTmp  = (VOS_ULONG)pDestAttr->sCoderSetDstBuf.pucOutputStart;
    start       = (VOS_UINT32)(ulStartTmp & 0xFFFFFFFFUL);

    ulEndTmp    = (VOS_ULONG)pDestAttr->sCoderSetDstBuf.pucOutputEnd;
    end         = (VOS_UINT32)(ulEndTmp & 0xFFFFFFFFUL);
    bufThreshold= pDestAttr->sCoderSetDstBuf.u32Threshold;
    u32Thrh     = pDestAttr->u32EncDstThrh;

    SOCP_CHECK_PARA(start);
    SOCP_CHECK_8BYTESALIGN(start);
    SOCP_CHECK_PARA(end);
    SOCP_CHECK_BUF_ADDR(start, end);
    SOCP_CHECK_PARA(bufThreshold);

    buflength = end - start + 1;
    SOCP_CHECK_8BYTESALIGN(buflength);

    SOCP_REG_READ(SOCP_REG_VERSION, ulVersion);

    if (SOCP_OLD_VER == ulVersion)
    {
        if (buflength > SOCP_ENC_DST_BUFLGTH_MAX)
        {
            SOCP_LOG_ERR("CoderSetDestChanAttr: buffer length is too large", buflength, 0);
            return BSP_ERR_SOCP_INVALID_PARA;
        }

        if (bufThreshold > SOCP_ENC_DST_TH_MAX)
        {
            SOCP_LOG_ERR("CoderSetDestChanAttr: buffer threshold is too large", bufThreshold, 0);
            return BSP_ERR_SOCP_INVALID_PARA;
        }
    }

    if(u32Thrh < SOCP_ENC_DST_THRH_MIN)
    {
        SOCP_LOG_ERR("CoderSetDestChanAttr: buffer thrh is too large", u32Thrh, 0);
        return BSP_ERR_SOCP_INVALID_PARA;
    }

    pChan = &g_strSocpStat.sEncDstChan[u32ChanID];

    /* 如果经过配置则不能再次配置,通道复位之后只配置一次 */
    if (SOCP_CHN_SET == pChan->u32SetStat)
    {
        SOCP_LOG_ERR("CoderSetDestChanAttr: channel can't be set twice!", u32ChanID, 0);
        return BSP_ERR_SOCP_SET_FAIL;
    }

    /* 使用配置参数进行配置 */
    /* 写入起始地址到目的buffer起始地址寄存器*/
    SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFADDR(u32ChanID),SOCP_VIRT_PHY(start));
    SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFRPTR(u32ChanID),SOCP_VIRT_PHY(start));
    SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFWPTR(u32ChanID),SOCP_VIRT_PHY(start));

    /* SOCP在V8R2上进行对源通道配置进行扩展 */
    if (SOCP_OLD_VER == ulVersion)
    {
        SOCP_REG_SETBITS(SOCP_REG_ENCDEST_BUFCFG(u32ChanID), 0, 21, buflength);
        SOCP_REG_SETBITS(SOCP_REG_ENCDEST_BUFCFG(u32ChanID), 21, 11, bufThreshold);
    }
    else
    {
        SOCP_REG_SETBITS(SOCP_REG_ENCDEST_BUFCFG(u32ChanID), 0, 31, buflength);
        SOCP_REG_SETBITS(SOCP_REG_ENCDEST_INTTHRH(u32ChanID), 0, 31, bufThreshold*1024);
    }

    SOCP_REG_SETBITS(SOCP_REG_ENCDEST_BUFTHRH(u32ChanID), 0, 31, u32Thrh);

    /* 在g_strSocpStat中保存参数*/
    pChan->u32ChanID              = u32ChanID;
    pChan->u32Thrh                = u32Thrh;
    pChan->sEncDstBuf.u32Start    = start;
    pChan->sEncDstBuf.u32End      = end;
    pChan->sEncDstBuf.u32Write    = start;
    pChan->sEncDstBuf.u32Read     = start;
    pChan->sEncDstBuf.u32Length   = buflength;

    /* 表明该通道已经配置 */
    pChan->u32SetStat             = SOCP_CHN_SET;

    /* 先清中断，再打开中断 */
    SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT0, u32ChanID, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_ENC_MASK0, u32ChanID, 1, 0);

    SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT2, u32ChanID, 1, 1);
    /*SOCP_REG_SETBITS(SOCP_REG_ENC_MASK2, u32ChanID, 1, 0);*//*不再判断这个溢出中断，通道配置保证*/

    /* 编码目的buffer阈值溢出中断 */
    SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT2, u32ChanID+SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_ENC_MASK2, u32ChanID+SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN, 1, 0);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_DecoderSetSrcChanAttr
*
* 功能描述  : 解码源通道配置函数
*
* 输入参数  : u32SrcChanID    解码源通道ID
*             pInputAttr      解码源通道配置参数
*
* 输出参数  :
*
* 返 回 值  : 配置成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_DecoderSetSrcChanAttr(VOS_UINT32                u32SrcChanID,
                                                        SOCP_DECODER_SRC_CHAN_STRU *pInputAttr)
{
    VOS_UINT32             start;
    VOS_ULONG              ulStartTmp;
    VOS_UINT32             end;
    VOS_ULONG              ulEndTmp;
    VOS_UINT32             buflength = 0;
    VOS_UINT32             u32ChanID;
    VOS_UINT32             u32ChanType;
    VOS_UINT32             i;
    VOS_UINT32             u32ResetFlag;
    SOCP_DECSRC_CHAN_STRU  *pDecSrcChan;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pInputAttr);

    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_DECODER_SRC_CHAN);
    SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_DECSRC_CHN);
    SOCP_CHECK_DATA_TYPE_EN(pInputAttr->eDataTypeEn);

    ulStartTmp = (VOS_ULONG)pInputAttr->sDecoderSetSrcBuf.pucInputStart;
    start      = (VOS_UINT32)(ulStartTmp & 0xFFFFFFFFUL);
    ulEndTmp   = (VOS_ULONG)pInputAttr->sDecoderSetSrcBuf.pucInputEnd;
    end        = (VOS_UINT32)(ulEndTmp & 0xFFFFFFFFUL);

    SOCP_CHECK_PARA(start);
    SOCP_CHECK_8BYTESALIGN(start);
    SOCP_CHECK_PARA(end);
    SOCP_CHECK_BUF_ADDR(start, end);

    buflength = end - start + 1;

    SOCP_CHECK_8BYTESALIGN(buflength);

    if (buflength > SOCP_DEC_SRC_BUFLGTH_MAX)
    {
        SOCP_LOG_ERR("DecoderSetSrcChanAttr: buffer length is too large", buflength, 0);
        return BSP_ERR_SOCP_INVALID_PARA;
    }

    pDecSrcChan = &g_strSocpStat.sDecSrcChan[u32ChanID];

    if (SOCP_CHN_SET == pDecSrcChan->u32SetStat)
    {
        SOCP_LOG_ERR("DecoderSetSrcChanAttr: channel has been configed", u32ChanID, 0);
        return BSP_ERR_SOCP_DECSRC_SET;
    }

    /* 首先复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_DECRST, u32ChanID, 1, 1);

    /* 等待通道复位状态自清 */
    for (i = 0; i < SOCP_RESET_TIME; i++)
    {
        u32ResetFlag = SOCP_REG_GETBITS(SOCP_REG_DECRST, u32ChanID, 1);
        if (0 == u32ResetFlag)
        {
            break;
        }
    }

    if (SOCP_RESET_TIME == i)
    {
        SOCP_LOG_WARNING("DecoderSetSrcChanAttr: reset channel failed!\n", u32ChanID, 0);
    }

    /* 使用配置参数进行配置 */
    SOCP_REG_WRITE(SOCP_REG_DECSRC_BUFWPTR(u32ChanID),SOCP_VIRT_PHY(start));
    SOCP_REG_WRITE(SOCP_REG_DECSRC_BUFRPTR(u32ChanID),SOCP_VIRT_PHY(start));
    SOCP_REG_WRITE(SOCP_REG_DECSRC_BUFADDR(u32ChanID),SOCP_VIRT_PHY(start));

    SOCP_REG_SETBITS(SOCP_REG_DECSRC_BUFCFG0(u32ChanID), 0, 16, buflength);
    SOCP_REG_SETBITS(SOCP_REG_DECSRC_BUFCFG0(u32ChanID), 31, 1, pInputAttr->eDataTypeEn);
    SOCP_REG_SETBITS(SOCP_REG_DECSRC_RDQCFG(u32ChanID), 29, 1, 0);

    /* 在全局变量中保存安装的参数 */
    pDecSrcChan->u32ChanID              = u32ChanID;
    pDecSrcChan->eDataTypeEn            = pInputAttr->eDataTypeEn;
    pDecSrcChan->sDecSrcBuf.u32Start    = start;
    pDecSrcChan->sDecSrcBuf.u32End      = end;
    pDecSrcChan->sDecSrcBuf.u32Length   = buflength;
    pDecSrcChan->sDecSrcBuf.u32Read     = start;
    pDecSrcChan->sDecSrcBuf.u32Write    = start;

    pDecSrcChan->u32SetStat = SOCP_CHN_SET;

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名  : SocpDecSrcStart
*
* 功能描述  : 解码启动函数
*
* 输入参数  : ulRealChanID      通道ID
* 输出参数  :
*
* 返 回 值  : 启动成功与否的标识码
*****************************************************************************/
/*lint -e529*/
VOS_UINT32 SocpDecSrcStart(VOS_UINT32 ulRealChanID)
{
    VOS_UINT32 i;
    VOS_UINT32 ulIntIDMask  = 0;
    VOS_UINT32 ulLogic      = 1;

    SOCP_CHECK_CHAN_ID(ulRealChanID, SOCP_MAX_DECSRC_CHN);
    SOCP_CHECK_DECSRC_SET(ulRealChanID);

    /* 打开rd完成中断*/
    if (SOCP_DECSRC_CHNMODE_LIST == g_strSocpStat.sDecSrcChan[ulRealChanID].eChnMode)
    {
        SOCP_REG_SETBITS(SOCP_REG_DEC_RAWINT1, ulRealChanID, 1, 1);
        SOCP_REG_SETBITS(SOCP_REG_DEC_MASK1, ulRealChanID, 1, 0);
    }

    for (i = 1; i < SOCP_DEC_SRCINT_NUM; i++)
    {
        ulIntIDMask  = SOCP_REG_GETBITS(SOCP_REG_DEC_RAWINT1, i * 4, 4);
        ulIntIDMask |= 1 << ulRealChanID;
        SOCP_REG_SETBITS(SOCP_REG_DEC_RAWINT1, (i*4), 4, ulIntIDMask);

        ulIntIDMask  = SOCP_REG_GETBITS(SOCP_REG_DEC_MASK1, i * 4, 4);
        ulIntIDMask &= ~(ulLogic << ulRealChanID);
        SOCP_REG_SETBITS(SOCP_REG_DEC_MASK1, (i*4), 4, ulIntIDMask);
    }

    /* 设置打开状态*/
    SOCP_REG_SETBITS(SOCP_REG_DECSRC_BUFCFG0(ulRealChanID), 30, 1, 1);

    g_strSocpStat.sDecSrcChan[ulRealChanID].u32ChanEn = SOCP_CHN_ENABLE;

    g_stSocpDebugInfo.sSocpDebugDecSrc.u32SocpStartDecSrcCnt[ulRealChanID]++;

    return VOS_OK;
}
/*lint +e529*/

/*****************************************************************************
* 函 数 名  : BSP_SOCP_RegisterReadCB
*
* 功能描述  : 读数据回调函数注册函数
*
* 输入参数  : u32DestChanID  目的通道ID
              ReadCB         读数据回调函数
*
* 输出参数  :
*
* 返 回 值  : 注册成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_RegisterReadCB(VOS_UINT32 u32DestChanID, socp_read_cb ReadCB)
{
    VOS_UINT32 u32RealChanID;
    VOS_UINT32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 获取通道类型和实际的通道ID */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32DestChanID);
    u32ChanType   = SOCP_REAL_CHAN_TYPE(u32DestChanID);

    if (SOCP_DECODER_DEST_CHAN == u32ChanType) /* 解码通道 */
    {
        SOCP_CHECK_CHAN_ID(u32RealChanID, SOCP_MAX_DECDST_CHN);
        SOCP_CHECK_DECDST_SET(u32RealChanID);

        g_strSocpStat.sDecDstChan[u32RealChanID].read_cb = ReadCB;

        g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpRegReadCBDecDstCnt[u32RealChanID]++;
    }

    else if (SOCP_CODER_DEST_CHAN == u32ChanType)/* 编码通道 */
    {
        SOCP_CHECK_CHAN_ID(u32RealChanID, SOCP_MAX_ENCDST_CHN);
        SOCP_CHECK_ENCDST_SET(u32RealChanID);

        /* 设置对应通道的回调函数*/
        g_strSocpStat.sEncDstChan[u32RealChanID].read_cb = ReadCB;

        g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpRegReadCBEncDstCnt[u32RealChanID]++;
    }
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_RegisterReadCB: invalid Channel type", u32DestChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_GetReadBuff
*
* 功能描述  : 获取读数据buffer函数
*
* 输入参数  : u32DestChanID  目的通道buffer

* 输出参数  : pBuffer        获取的读数据buffer
*
* 返 回 值  : 获取成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_GetReadBuff(VOS_UINT32 u32DestChanID, SOCP_BUFFER_RW_STRU *pBuffer)
{
    VOS_UINT32  u32ChanID;
    VOS_UINT32  u32ChanType;
    VOS_UINT32  uPAddr;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pBuffer);

    SOCP_LOG_INFO("BSP_SOCP_GetReadBuff: Channel is", u32DestChanID, 0);

    /* 获得实际通道id */
    u32ChanID           = SOCP_REAL_CHAN_ID(u32DestChanID);
    u32ChanType         = SOCP_REAL_CHAN_TYPE(u32DestChanID);
    pBuffer->u32Size    = 0;
    pBuffer->u32RbSize  = 0;

    if (SOCP_DECODER_DEST_CHAN == u32ChanType) /* 解码通道 */
    {
        g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpGetReadBufDecDstEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_DECDST_CHN);
        SOCP_CHECK_DECDST_SET(u32ChanID);

        /* 根据读写指针获取buffer */
        SOCP_REG_READ(SOCP_REG_DECDEST_BUFRPTR(u32ChanID), uPAddr);
        g_strSocpStat.sDecDstChan[u32ChanID].sDecDstBuf.u32Read = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_DECDEST_BUFWPTR(u32ChanID), uPAddr);
        g_strSocpStat.sDecDstChan[u32ChanID].sDecDstBuf.u32Write= (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetDataBuffer(&g_strSocpStat.sDecDstChan[u32ChanID].sDecDstBuf, pBuffer);

        SOCP_LOG_INFO("BSP_SOCP_GetReadBuff: Channel is", u32DestChanID, 0);

        g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpGetReadBufDecDstSucCnt[u32ChanID]++;
    }
    else if (SOCP_CODER_DEST_CHAN == u32ChanType)
    {
        g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpGetReadBufEncDstEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_ENCDST_CHN);
        SOCP_CHECK_ENCDST_SET(u32ChanID);

        /* 根据读写指针获取buffer */
        SOCP_REG_READ(SOCP_REG_ENCDEST_BUFRPTR(u32ChanID), uPAddr);
        g_strSocpStat.sEncDstChan[u32ChanID].sEncDstBuf.u32Read = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_ENCDEST_BUFWPTR(u32ChanID), uPAddr);
        g_strSocpStat.sEncDstChan[u32ChanID].sEncDstBuf.u32Write= (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetDataBuffer(&g_strSocpStat.sEncDstChan[u32ChanID].sEncDstBuf, pBuffer);

        SOCP_LOG_INFO("BSP_SOCP_GetReadBuff: Channel is", u32DestChanID, (pBuffer->u32RbSize+pBuffer->u32Size));

        g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpGetReadBufEncDstSucCnt[u32ChanID]++;
    }
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_GetReadBuff: invalid Channel type", u32DestChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_ReadDataDone
*
* 功能描述  : 读数据完成函数
*
* 输入参数  : u32DestChanID    目的通道ID
*             u32ReadSize      读取数据大小
* 输出参数  : 无
*
* 返 回 值  : 读数据成功与否的标识码
*****************************************************************************/

VOS_UINT32 BSP_SOCP_ReadDataDone(VOS_UINT32 u32DestChanID, VOS_UINT32 u32ReadSize)
{
    VOS_UINT32                      u32ChanID;
    VOS_UINT32                      u32ChanType;
    VOS_ULONG                       ulLockLevel;
    SOCP_BUFFER_RW_STRU             stRwBuff;
    VOS_UINT32                      uPAddr;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    SOCP_LOG_INFO("BSP_SOCP_ReadDataDone: Channel ID and len is", u32DestChanID, u32ReadSize);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32DestChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32DestChanID);

    if (SOCP_DECODER_DEST_CHAN == u32ChanType) /* 解码通道 */
    {
        SOCP_DECDST_CHAN_STRU *pChan;

        g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpReadDoneDecDstEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_DECDST_CHN);
        SOCP_CHECK_DECDST_SET(u32ChanID);

        pChan = &g_strSocpStat.sDecDstChan[u32ChanID];

        SOCP_REG_READ(SOCP_REG_DECDEST_BUFWPTR(u32ChanID), uPAddr);
        pChan->sDecDstBuf.u32Write= (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_DECDEST_BUFRPTR(u32ChanID), uPAddr);
        pChan->sDecDstBuf.u32Read = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetDataBuffer(&pChan->sDecDstBuf, &stRwBuff);

        if(stRwBuff.u32Size + stRwBuff.u32RbSize < u32ReadSize)
        {
            SOCP_LOG_ERR("BSP_SOCP_ReadDataDone: dec dst too large read size. write ptr is, read ptr is", pChan->sDecDstBuf.u32Write,
                      pChan->sDecDstBuf.u32Read);

            SOCP_LOG_ERR("BSP_SOCP_ReadDataDone: dec dst too large read size. u32Size is 0x%x, u32RbSize is 0x%x\n",
                      stRwBuff.u32Size, stRwBuff.u32RbSize);

            /*s32LockKey = (VOS_INT32)VOS_SplIMP();*/
            VOS_SpinLockIntLock(&g_stSocpSpinLock, ulLockLevel);

            /* added by yangzhi 2011.7.25 */
            SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK0, u32ChanID, 1, 0);

            /*清空溢出中断状态，打开溢出中断屏蔽*/
            SOCP_REG_SETBITS(SOCP_REG_DEC_RAWINT2,      u32ChanID, 1, 1);
            SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK2,   u32ChanID, 1, 0);

            /*VOS_Splx((VOS_INT32)s32LockKey);*/
            VOS_SpinUnlockIntUnlock(&g_stSocpSpinLock, ulLockLevel);

            g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpReadDoneDecDstFailCnt[u32ChanID]++;

            return BSP_ERR_SOCP_INVALID_PARA;
        }

        /* 设置读写指针 */
        SocpReadDone(&pChan->sDecDstBuf, u32ReadSize);

        /* 写入写指针到写指针寄存器*/
        uPAddr = SOCP_VIRT_PHY(pChan->sDecDstBuf.u32Read);
        SOCP_REG_WRITE(SOCP_REG_DECDEST_BUFRPTR(u32ChanID), uPAddr);

        /*s32LockKey = (VOS_INT32)VOS_SplIMP(); */        /*锁中断*/
        VOS_SpinLockIntLock(&g_stSocpSpinLock, ulLockLevel);

        /* added by yangzhi 2011.7.25 */
        SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK0, u32ChanID, 1, 0);

        /*清空溢出中断状态，打开溢出中断屏蔽*/
        SOCP_REG_SETBITS(SOCP_REG_DEC_RAWINT2,      u32ChanID, 1, 1);
        SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK2,   u32ChanID, 1, 0);

        /*VOS_Splx((VOS_INT32)s32LockKey);*/
        VOS_SpinUnlockIntUnlock(&g_stSocpSpinLock, ulLockLevel);

        g_stSocpDebugInfo.sSocpDebugDecDst.u32SocpReadDoneDecDstSucCnt[u32ChanID]++;
    }
    else if (SOCP_CODER_DEST_CHAN == u32ChanType)/* 编码通道 */
    {
        SOCP_ENCDST_CHAN_STRU *pChan;

        g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpReadDoneEncDstEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_ENCDST_CHN);
        SOCP_CHECK_ENCDST_SET(u32ChanID);

        g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpReadDoneValidEncDstCnt[u32ChanID]++;

        pChan = &g_strSocpStat.sEncDstChan[u32ChanID];

        SOCP_REG_READ(SOCP_REG_ENCDEST_BUFWPTR(u32ChanID), uPAddr);
        pChan->sEncDstBuf.u32Write= SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_ENCDEST_BUFRPTR(u32ChanID), uPAddr);
        pChan->sEncDstBuf.u32Read = SOCP_PHY_VIRT(uPAddr);

        SocpGetDataBuffer(&pChan->sEncDstBuf, &stRwBuff);

        if (stRwBuff.u32Size + stRwBuff.u32RbSize < u32ReadSize)
        {
            SOCP_LOG_ERR("BSP_SOCP_ReadDataDone: enc dst too large read size. write ptr is, read ptr is",
                            pChan->sEncDstBuf.u32Write, pChan->sEncDstBuf.u32Read);

            SOCP_LOG_ERR("BSP_SOCP_ReadDataDone: enc dst too large read size. u32Size is, u32RbSize is",
                            stRwBuff.u32Size, stRwBuff.u32RbSize);

            /*s32LockKey = (VOS_INT32)VOS_SplIMP();*/
            VOS_SpinLockIntLock(&g_stSocpSpinLock, ulLockLevel);

            SOCP_REG_SETBITS(SOCP_REG_ENC_MASK0, u32ChanID, 1, 0);

            /*清空阈值上溢中断，打开阈值上溢中断，这个中断可以再次上报*/
            SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT2, u32ChanID+SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN, 1, 1);
            SOCP_REG_SETBITS(SOCP_REG_ENC_MASK2, u32ChanID+SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN, 1, 0);

            g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpReadDoneEncDstFailCnt[u32ChanID]++;

            /*VOS_Splx((VOS_INT32)s32LockKey);*/
            VOS_SpinUnlockIntUnlock(&g_stSocpSpinLock, ulLockLevel);

            return BSP_ERR_SOCP_INVALID_PARA;
        }

        /* 设置读写指针 */
        SocpReadDone(&pChan->sEncDstBuf, u32ReadSize);

        /* 写入写指针到写指针寄存器*/
        uPAddr = SOCP_VIRT_PHY(pChan->sEncDstBuf.u32Read);
        SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFRPTR(u32ChanID), uPAddr);

        /*s32LockKey = (VOS_INT32)VOS_SplIMP();*/
        VOS_SpinLockIntLock(&g_stSocpSpinLock, ulLockLevel);

        SOCP_REG_SETBITS(SOCP_REG_ENC_MASK0, u32ChanID, 1, 0);

        /*清空阈值上溢中断，打开阈值上溢中断，这个中断可以再次上报*/
        SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT2, u32ChanID+SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN, 1, 1);
        SOCP_REG_SETBITS(SOCP_REG_ENC_MASK2, u32ChanID+SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN, 1, 0);

        /*VOS_Splx((VOS_INT32)s32LockKey);*/
        VOS_SpinUnlockIntUnlock(&g_stSocpSpinLock, ulLockLevel);

        g_stSocpDebugInfo.sSocpDebugEncDst.u32SocpReadDoneEncDstSucCnt[u32ChanID]++;
    }
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_ReadDataDone: invalid Channel type", u32DestChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    VOS_FlushCpuWriteBuf();

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名   : BSP_SOCP_GetSrcChannelReg
*
* 功能描述  : 读取指定编码源通道的寄存器
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID BSP_SOCP_GetEncSrcReg(VOS_UINT32 *pulData, VOS_UINT32 ulDataMax)
{
    VOS_UINT32      i;

    /* 读取 0 到 7总共8个通道的配置寄存器，每个通道有9个32bit 寄存器，总共 288 个byte */
    if ( ulDataMax < 72 )/* 长度的单位是4byte，不够保存 */
    {
        return;
    }

    for(i=0; i<8; i++)
    {
        VOS_MemCpy((pulData+(i*9)), (VOS_VOID*)(g_strSocpStat.uBaseAddr+SOCP_REG_ENCSRC_BUFWPTR(i)), 9*sizeof(VOS_UINT32));
    }

    return;
}

#endif /*end of SOCP_ACORE*/

/*****************************************************************************
* 函 数 名  : SocpInitGobal
*
* 功能描述  : 模块初始化函数,初始化全局变量
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/
VOS_VOID SocpInitGobal(VOS_VOID)
{
#ifdef SOCP_ACORE
    VOS_UINT32 i;
#endif

    VOS_MemSet(&g_stSocpDebugInfo, 0x0 ,sizeof(SOCP_DEBUGINFO_STRU));

    VOS_MemSet(&g_strSocpStat, 0x0 ,sizeof(SOCP_GBLSTATE_STRU));

    VOS_MemSet(&g_stSocpLog, 0, sizeof(SOCP_LOG_INFO_STRU));

#ifdef SOCP_ACORE
    for(i=0; i<SOCP_MAX_ENCDST_CHN; i++)
    {
        g_strSocpStat.sEncDstChan[i].u32Thrh        = 0;
        g_strSocpStat.sEncDstChan[i].u32SetStat     = SOCP_CHN_UNSET;
    }

    for(i=0; i<SOCP_MAX_DECSRC_CHN; i++)
    {
        g_strSocpStat.sDecSrcChan[i].u32ChanID      = i;
        g_strSocpStat.sDecSrcChan[i].u32ChanEn      = SOCP_CHN_DISABLE;
        g_strSocpStat.sDecSrcChan[i].eDataTypeEn    = SOCP_DATA_TYPE_EN_BUTT;
        g_strSocpStat.sDecSrcChan[i].u32SetStat     = SOCP_CHN_UNSET;
    }

    for(i=0; i<SOCP_MAX_DECDST_CHN; i++)
    {
        g_strSocpStat.sDecDstChan[i].u32ChanID      = i;
        g_strSocpStat.sDecDstChan[i].u32SetStat     = SOCP_CHN_UNSET;
        g_strSocpStat.sDecDstChan[i].eDataType      = SOCP_DATA_TYPE_BUTT;
    }
#endif

    return;
}

#ifdef SOCP_ACORE
/*****************************************************************************
* 函 数 名  : SocpAcpuCreateTask
*
* 功能描述  : 任务创建函数
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 创建成功与否的标识码
*****************************************************************************/

VOS_INT32 SocpAcpuCreateTask(VOS_VOID)
{
    VOS_UINT32      aulOmCnfArguments[4] = {SOCP_CODER_DST_OM_CNF,0,0,0};
    VOS_UINT32      aulOmIndArguments[4] = {SOCP_CODER_DST_OM_IND,0,0,0};
    VOS_UINT32      ulTaskID;
    VOS_UINT32      ulChannelId;

#if 0
    /* 编码源通道任务的信号量*/
    if(VOS_OK != VOS_SmBCreate("SCP1", 0, VOS_SEMA4_FIFO, &g_strSocpStat.u32EncSrcSemID))
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create u32EncSrcSemID failed", 0, 0);

        return BSP_ERR_SOCP_SEM_CREATE;
    }

    /* 编码源通道任务的创建 "EncSrc"*/
    if( VOS_OK != VOS_CreateTask( "EncSrc",
                                    &ulTaskID,
                                    (VOS_TASK_ENTRY_TYPE)SocpEncSrcTask,
                                    SOCP_ENCSRC_SELFTASK_PRIO,
                                    SOCP_ENCSRC_TASK_STACK_SIZE,
                                    aulArguments) )
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create SocpEncSrcTask failed", 0, 0);

        return BSP_ERR_SOCP_TSK_CREATE;
    }
#endif

    /* 编码目的通道SOCP_CODER_DST_OM_CNF任务的信号量*/
    ulChannelId = SOCP_CODER_DST_OM_CNF;
    if(VOS_OK != VOS_SmCCreate("CH0SCP", 0, VOS_SEMA4_FIFO, &g_strSocpStat.u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(ulChannelId)]))
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create CH0SCP sem failed", 0, 0);

        return BSP_ERR_SOCP_SEM_CREATE;
    }

    /* 编码目的通道SOCP_CODER_DST_OM_CNF任务创建 "EncDstCh0"*/
    if( VOS_OK != VOS_CreateTask(  "EncDstCh0",
                                    &ulTaskID,
                                    (VOS_TASK_ENTRY_TYPE)SocpEncDstTaskChannel,
                                    SOCP_ENCDST_SELFTASK_PRIO,
                                    SOCP_ENCDST_TASK_STACK_SIZE,
                                    aulOmCnfArguments) )
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create SocpEncDstTaskChannel(cnf) failed",0,0);

        return BSP_ERR_SOCP_TSK_CREATE;
    }

    /* 编码目的通道SOCP_CODER_DST_OM_IND任务的信号量*/
    ulChannelId = SOCP_CODER_DST_OM_IND;
    if(VOS_OK != VOS_SmCCreate("CH1SCP", 0, VOS_SEMA4_FIFO, &g_strSocpStat.u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(ulChannelId)]))
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create CH1SCP failed", 0, 0);

        return BSP_ERR_SOCP_SEM_CREATE;
    }

    /* 编码目的通道SOCP_CODER_DST_OM_IND任务创建 "EncDstCh1"*/
    if( VOS_OK != VOS_CreateTask(  "EncDstCh1",
                                    &ulTaskID,
                                    (VOS_TASK_ENTRY_TYPE)SocpEncDstTaskChannel,
                                    SOCP_ENCDST_SELFTASK_PRIO,
                                    SOCP_ENCDST_TASK_STACK_SIZE,
                                    aulOmIndArguments) )
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create SocpEncDstTaskChannel(ind) failed",0,0);

        return BSP_ERR_SOCP_TSK_CREATE;
    }

#if 0   /* 等待底软提供接口 */
    /* 注册给底软，异常复位的时候保存EncDst栈信息 */
    DRV_EXCH_TASK_INFO_ADD((int)ulTaskID);
#endif

#if 0
    /* 解码源通道任务信号量*/
    if(VOS_OK != VOS_SmBCreate("SCP3", 0, VOS_SEMA4_FIFO, &g_strSocpStat.u32DecSrcSemID))
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create u32DecSrcSemID failed", 0, 0);

        return BSP_ERR_SOCP_SEM_CREATE;
    }

    /* 解码源通道任务创建 "DecSrc"*/
    if ( VOS_OK != VOS_CreateTask( "DecSrc",
                                    &ulTaskID,
                                    (VOS_TASK_ENTRY_TYPE)SocpDecSrcTask,
                                    SOCP_DECSRC_SELFTASK_PRIO,
                                    SOCP_DECSRC_TASK_STACK_SIZE,
                                    aulArguments) )
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create u32DecSrcTskID failed", 0, 0);

        return BSP_ERR_SOCP_TSK_CREATE;
    }

    /* 解码目的通道任务信号量*/
    if(VOS_OK != VOS_SmBCreate("SCP4", 0, VOS_SEMA4_FIFO, &g_strSocpStat.u32DecDstSemID))
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create u32DecDstSemID failed", 0, 0);

        return BSP_ERR_SOCP_SEM_CREATE;
    }

    /* 解码目的通道任务的创建 "DecDst"*/
    if ( VOS_OK != VOS_CreateTask( "DecDst",
                                    &ulTaskID,
                                    (VOS_TASK_ENTRY_TYPE)SocpDecDstTask,
                                    SOCP_DECDST_SELFTASK_PRIO,
                                    SOCP_DECDST_TASK_STACK_SIZE,
                                    aulArguments) )
    {
        SOCP_LOG_ERR("SocpAcpuCreateTask: create u32DecDstTskID failed", 0, 0);

        return BSP_ERR_SOCP_TSK_CREATE;
    }
#endif

#if 0   /* 等待底软提供接口 */
    /* 注册给底软，异常复位的时候保存DecDst栈信息 */
    DRV_EXCH_TASK_INFO_ADD((int)ulTaskID);
#endif

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名  : SocpCoreInit
*
* 功能描述  : 模块初始化函数, 负责初始化芯片
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/

VOS_VOID SocpCoreInit(VOS_VOID)
{
    VOS_UINT32 i;
    VOS_UINT32 u32ResetValue = 0;

    /* 对全局寄存器进行复位 */
    SOCP_REG_WRITE(SOCP_REG_GBLRST, 1);

    /* 等待通道复位状态自清 */
    for (i = 0; i < SOCP_RESET_TIME; i++)
    {
        SOCP_REG_READ(SOCP_REG_GBLRST, u32ResetValue);
        if (0 == u32ResetValue)
        {
            break;
        }
    }

    if (SOCP_RESET_TIME == i)
    {
        SOCP_LOG_WARNING("SocpCoreInit: reset global regs failed!", 0, 0);
    }

    /* 配置SOCP全局软复位控制寄存器GLOBAL_CTRL:bit[1]为1，编码通道采用INT_TIMEOUT(0x024)，且使用的寄存器
       位宽为32bit；解码通道采用DEC_INT_TIMEOUT(0x20) */
    SOCP_REG_SETBITS(SOCP_REG_GBLRST, 1, 1, 0x1);           /* 更新Bit1 */

    /* 增加ram中寄存器的复位操作*/
    for (i = 0; i < SOCP_REAL_CHAN_ID(SOCP_CODER_SRC_BUTT); i++)
    {
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(i), 0);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFCFG0(i), 0);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQWPTR(i), 0);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQRPTR(i), 0);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQADDR(i), 0);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQCFG(i),  0);
    }

    for (i = 0; i < SOCP_REAL_CHAN_ID(SOCP_CODER_DST_BUTT); i++)
    {
        SOCP_REG_WRITE(SOCP_REG_ENCDEST_BUFADDR(i), 0);
    }

    return;
}

/*lint -save -e102 -e10 -e2 -e40 -e533 -e31 -e830 -e522 -e718 -e746 -e702 -e565   -e64 -e23 -e63 -e26 -e578 -e132*/
extern unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);

/*****************************************************************************
* 函 数 名  : SocpLogBufferSizeParse
*
* 功能描述  : 在代码编译阶段将CMD LINE中的BUFFER大小参数解析出来
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
VOS_VOID SocpLogBufferSizeParse(VOS_CHAR *pucChar)
{
    VOS_UINT32      ulBufferSize;

    /* Buffer的大小以Byte为单位，原则上不大于200M，不小于1M */
    ulBufferSize = (VOS_UINT32)simple_strtoul(pucChar, VOS_NULL, VOS_NULL);

    if ((ulBufferSize > (50 *1024 *1024))
      || (ulBufferSize < (1 *1024 *1024)))
    {
        return;
    }

    /* 为了保持ulBufferSize的长度8字节对齐,如果长度不是8字节对齐地址也不会 */
    if (VOS_NULL != (ulBufferSize % 8))
    {
        return;
    }

    g_stEncDstBufLogConfig.BufferSize = ulBufferSize;
    g_stEncDstBufLogConfig.logOnFlag  = SOCP_ENC_DEST_LOG_DELAY_ON;

    return;
}

#if (!defined(DMT)) && (VOS_WIN32 != VOS_OS_VER)
early_param("mdmlogsize", SocpLogBufferSizeParse);
#endif

/*****************************************************************************
* 函 数 名  : SocpLogTimeoutParse
*
* 功能描述  : 在代码编译阶段将CMD LINE中的TIMEOUT大小参数解析出来
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
VOS_VOID SocpLogTimeoutParse(VOS_CHAR *pucChar)
{
    VOS_UINT32      ulTimeout;

    /* 输入字符串以秒为单位，需要再转换成毫秒，至少为1秒，不大于20分钟 */
    ulTimeout = (VOS_UINT32)simple_strtoul(pucChar, VOS_NULL, VOS_NULL);

    if (1200 < ulTimeout)
    {
        ulTimeout = 1200;
    }

    ulTimeout *= 1000;

    g_stEncDstBufLogConfig.overTime = ulTimeout;
    g_stEncDstBufLogConfig.ulCurTimeout = ulTimeout;

    return;
}

#if (!defined(DMT)) && (VOS_WIN32 != VOS_OS_VER)
early_param("mdmlogtime", SocpLogTimeoutParse);
#endif

/*****************************************************************************
* 函 数 名  : SocpLogBaseAddrParse
*
* 功能描述  : 在代码编译阶段将CMD LINE中的基地址参数解析出来
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
VOS_VOID SocpLogBaseAddrParse(VOS_CHAR *pucChar)
{
    VOS_UINT32      ulBaseAddr;

    ulBaseAddr = (VOS_UINT32)simple_strtoul(pucChar, VOS_NULL, VOS_NULL);

    /* 物理地址是32位的实地址并且是8字节对齐的 */
    if ((VOS_NULL != (ulBaseAddr % 8))
        || (VOS_NULL == ulBaseAddr))
    {
        g_stEncDstBufLogConfig.logOnFlag = VOS_FALSE;

        return;
    }

    g_stEncDstBufLogConfig.ulPhyBufferAddr = ulBaseAddr;

    return;
}

#if (!defined(DMT)) && (VOS_WIN32 != VOS_OS_VER)
early_param("mdmlogbase", SocpLogBaseAddrParse);
#endif

/*****************************************************************************
* 函 数 名  : SocpLogCfgShow
*
* 功能描述  : 打印延时写入的配置信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
VOS_VOID SocpLogCfgShow(VOS_VOID)
{
    vos_printf("\r\n SocpLogCfgShow: overTime %d", g_stEncDstBufLogConfig.overTime);
    vos_printf("\r\n SocpLogCfgShow: Current Time Out%d", g_stEncDstBufLogConfig.ulCurTimeout);
    vos_printf("\r\n SocpLogCfgShow: BufferSize %d", g_stEncDstBufLogConfig.BufferSize);
    vos_printf("\r\n SocpLogCfgShow: logOnFlag %d", g_stEncDstBufLogConfig.logOnFlag);
    vos_printf("\r\n SocpLogCfgShow: PhyBufferAddr 0x%X", g_stEncDstBufLogConfig.ulPhyBufferAddr);

    return;
}


/*****************************************************************************
 函 数 名  : SCM_MemRemap
 功能描述  : 用于LOG延迟写入时把数据从SOCP通道的缓冲中发送到VCOM端口把实地址转换成虚地址
             底软实现，仅做移植到COMM
 输入参数  : phys_addr:需要REMAP的物理地址
             size:     需要REMAP的数据大小
 输出参数  : 无
 返 回 值  : REMAP后的虚拟地址

 修改历史      :
  1.日    期   : 2014年8月11日
    作    者   : h59254
    修改内容   : V8R1 LOG延迟写入新增
*****************************************************************************/
VOS_VOID *SocpMemRemap(unsigned long phys_addr, size_t size)
{
#if (!defined(DMT)) && (VOS_WIN32 != VOS_OS_VER)
    int i;
    u8* vaddr;
    int npages = PAGE_ALIGN((phys_addr & (PAGE_SIZE - 1)) + size) >> PAGE_SHIFT;
    unsigned long offset = phys_addr & (PAGE_SIZE - 1);
    struct page **pages;

    pages = vmalloc(sizeof(struct page *) * npages);
    if (!pages)
    {
        return VOS_NULL_PTR;
    }

    pages[0] = phys_to_page(phys_addr);

    for (i = 0; i < npages - 1 ; i++)
    {
        pages[i + 1] = pages[i] + 1;
    }

    vaddr = (u8*)vmap(pages, npages, VM_MAP, pgprot_dmacoherent(PAGE_KERNEL));

    if ( VOS_NULL_PTR != vaddr )

    {
        vaddr += offset;
    }

    vfree(pages);

    return (void *)vaddr;
#else
    return (void *)phys_addr;
#endif
}

/*****************************************************************************
* 函 数 名  : SocpLogBufferInit
*
* 功能描述  : 在代码初始化阶段将LOG延迟输出使用的内存申请出来
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
VOS_VOID SocpLogBufferInit(VOS_VOID)
{
    if (SOCP_ENC_DEST_LOG_DELAY_ON != g_stEncDstBufLogConfig.logOnFlag)
    {
        return;
    }

    if (VOS_NULL == g_stEncDstBufLogConfig.ulPhyBufferAddr)
    {
        g_stEncDstBufLogConfig.logOnFlag = VOS_FALSE;

        return;
    }

    g_stEncDstBufLogConfig.pVirBuffer = SocpMemRemap((VOS_ULONG)g_stEncDstBufLogConfig.ulPhyBufferAddr,
                                                     g_stEncDstBufLogConfig.BufferSize);


    if (VOS_NULL_PTR == g_stEncDstBufLogConfig.pVirBuffer)
    {
        g_stEncDstBufLogConfig.logOnFlag = VOS_FALSE;
    }

    return;
}

#if (!defined(DMT)) && (VOS_WIN32 != VOS_OS_VER)
arch_initcall(SocpLogBufferInit);
#endif
/*lint -restore */

/*****************************************************************************
* 函 数 名  : SocpAcpuInit
*
* 功能描述  : 模块初始化函数,初始化Acpu的相关资源
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/
VOS_INT32 SocpAcpuInit(VOS_VOID)
{
    VOS_UINT_PTR                        uBaseAddr;
    VOS_INT32                           s32IntLevel;
    VOS_INT32                           s32Result;
    SOCP_ENC_DST_BUF_LOG_CFG_STRU       *pstcfg;

    vos_printf("\r\n ---------------ACPU zzz SocpInit--------------------\r\n");

    pstcfg = SocpGetSDLogCfg();

    uBaseAddr = (VOS_UINT_PTR)mdrv_misc_get_ip_baseaddr(BSP_IP_TYPE_SOCP);

#if(VOS_OS_VER == VOS_LINUX)
    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)ioremap(uBaseAddr, SOCP_REGISTER_SIZE);

    if(0 == g_strSocpStat.uBaseAddr)
    {
        SOCP_LOG_ERR("SocpAcpuInit: Socp Reg Base Addr ioremap Error", 0, 0);
        return MDRV_ERROR;
    }
#else
    g_strSocpStat.uBaseAddr = uBaseAddr;
#endif

    SocpCoreInit(); /*A核初始化芯片*/

    /* APP CPU时，需要屏蔽的中断 */
    SOCP_REG_SETBITS(SOCP_REG_ENC_MASK0, 0, 7, 0x7f);           /* 更新Bit0~6 */
    SOCP_REG_WRITE(SOCP_REG_APP_MASK1, VOS_NULL_DWORD);
    SOCP_REG_SETBITS(SOCP_REG_ENC_MASK2, 0, 7, 0x7f);           /* 更新Bit0~6 */
    SOCP_REG_WRITE(SOCP_REG_APP_MASK3, VOS_NULL_DWORD);
    SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK0, 0, 16, VOS_NULL_WORD);    /* 更新Bit0~15 */
    SOCP_REG_SETBITS(SOCP_REG_DEC_MASK1, 4, 24, 0xffffff);              /* 更新Bit4~27 */
    SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK2, 0, 16, VOS_NULL_WORD);    /* 更新Bit0~15 */

    /* 屏蔽上溢中断 */
    SOCP_REG_SETBITS(SOCP_REG_ENC_MASK2, 16, 7, 0x7f);      /* 更新Bit16~22 */

    /* 解码目的通道超时配置暂不需要，将解码目的超时寄存器DEC_INT_TIMEOUT:bit[0~31]配为全0 */
    SOCP_REG_WRITE(SOCP_REG_DECINTTIMEOUT, 0);

    if (SOCP_ENC_DEST_LOG_DELAY_ON == pstcfg->logOnFlag)
    {
        /* 配置为NV项en_NV_ID_SOCP_SDLOG_CFG中配置的延迟时间 */
        SOCP_REG_WRITE(SOCP_REG_INTTIMEOUT, SOCP_MsConvertToCycle(pstcfg->overTime));
    }
    else
    {
        SOCP_REG_WRITE(SOCP_REG_INTTIMEOUT, SOCP_TRANS_TIMEOUT_DEFAULT);    /* 超时传输中断超时配置 */
    }

    SOCP_REG_SETBITS(SOCP_REG_BUFTIMEOUT, 31, 1, 0);                    /* 源目的buffer溢出超时中断丢包使能 */
    SOCP_REG_SETBITS(SOCP_REG_BUFTIMEOUT, 0, 16, SOCP_OVERFLOW_TIMEOUT_DEFAULT); /* 源目的buffer溢出超时配置 */

#if 0
    /* 配置解码通路包长度配置寄存器*/
    SOCP_REG_SETBITS(SOCP_REG_DEC_PKTLEN, 0, 12, SOCP_DEC_PKTLGTH_MAX); /* 解码后最大数据包大小，以KB为单位。默认为2KB */
    SOCP_REG_SETBITS(SOCP_REG_DEC_PKTLEN, 12, 5, SOCP_DEC_PKTLGTH_MIN); /* 解码后最小数据包大小，字节为单位 */
#endif

     /* 设置自动时钟门控 */
    SOCP_REG_WRITE(SOCP_REG_CLKCTRL, VOS_NULL_DWORD); /* RAM自动门控控制 */

    s32IntLevel = mdrv_int_get_num(BSP_INT_TYPE_SOCP_A);

    if (MDRV_ERROR == s32IntLevel)
    {
        SOCP_LOG_ERR("SocpAcpuInit: Query app core int failed", 0, 0);
        return MDRV_ERROR;
    }

    /* 挂中断 */
    if (VOS_OK != mdrv_int_connect(s32IntLevel, (VOIDFUNCPTR)SocpAppInthandler, 0))
    {
        SOCP_LOG_ERR("SocpAcpuInit: connect app core int failed", 0, 0);
        return MDRV_ERROR;
    }

    if (VOS_OK != mdrv_int_enable(s32IntLevel))
    {
        SOCP_LOG_ERR("SocpAcpuInit: enable int failed", 0, 0);
        return MDRV_ERROR;
    }

    s32Result = SocpAcpuCreateTask();

    if (VOS_OK != s32Result)
    {
        SOCP_LOG_ERR("SocpAcpuInit: create task failed", (VOS_UINT32)s32Result, 0);

        return MDRV_ERROR;
    }

    vos_printf("\r\n ---------------ACPU SocpInit end--------------------\r\n");

    return VOS_OK;
}
#endif

#ifdef SOCP_CCORE
/*****************************************************************************
* 函 数 名  : SocpCcpuInit
*
* 功能描述  : 模块初始化函数,初始化Ccpu的相关资源
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/
VOS_INT32 SocpCcpuInit(VOS_VOID)
{
    VOS_INT32     s32IntLevel;

#if 0 /* log2.0代码先注掉 */
    NV_SOCP_LOG_CFG_STRU nvCfg = {0};
    SOCP_ENC_DST_BUF_LOG_CFG_STRU * logCfg;

    logCfg = BSP_SOCP_GetLogCfg();
#endif

    vos_printf("\r\n ---------------CCPU zzz SocpInit--------------------\r\n");

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)mdrv_misc_get_ip_baseaddr(BSP_IP_TYPE_SOCP);

    if(VOS_NULL == g_strSocpStat.uBaseAddr)
    {
        SOCP_LOG_ERR("SocpCcpuInit: Query SOCP base addr failed",0,0);
        return MDRV_ERROR;
    }

#if 0 /* log2.0代码先注掉 */
#if(FEATURE_LTE == FEATURE_ON)
    ret = NVM_Read(EN_NV_ID_SOCP_LOG_CFG, (VOID *)&nvCfg, sizeof(NV_SOCP_LOG_CFG_STRU));
    if(ret != VOS_OK)
    {
        printf("SocpInit READ NV FAIL\n\n\n\n");
        nvCfg.ulSocpDelayWriteFlg = FALSE;
        logCfg->logOnFlag = FALSE;
    }
#endif
#endif

    /* 配置传输中断超时时间和buffer溢出超时时间 */
#if 0
    #if(FEATURE_LTE == FEATURE_ON)
    if( TRUE == nvCfg.ulSocpDelayWriteFlg )
    {
        SOCP_REG_WRITE(SOCP_REG_INTTIMEOUT, 0);
    }
    #endif
#endif

    /* Modem Cpu时，需要屏蔽中断 */
    SOCP_REG_WRITE(SOCP_REG_MODEM_MASK1, VOS_NULL_DWORD); /* 编码通路源数据包头出错中断屏蔽控制位 MODEM-cpu配置 */
    SOCP_REG_WRITE(SOCP_REG_MODEM_MASK3, VOS_NULL_DWORD); /* CORE1编码通路RD环形完成中断屏蔽控制位 */
    SOCP_REG_SETBITS(SOCP_REG_DEC_CORE1MASK0, 0, 16, VOS_NULL_WORD);   /* core1 DATA-TYPE 错误*/
    SOCP_REG_SETBITS(SOCP_REG_DEC_CORE1MASK2, 0, 16, VOS_NULL_WORD);   /* core1解码DATA-TYPE */

    /* 屏蔽编码传输中断 */
    SOCP_REG_WRITE(SOCP_REG_MODEM_ENC_MASK0, 0x7f);         /* core1编码通路传输中断屏蔽控制位 */

    /* 屏蔽编码目的buffer上溢中断 */
    SOCP_REG_SETBITS(SOCP_REG_MODEM_ENC_MASK2, 0, 7, 0x7f); /* core1编码目的buffer剩余空间不够一次dma传输时上溢中断屏蔽控制 */
    SOCP_REG_SETBITS(SOCP_REG_MODEM_ENC_MASK2, 16, 7,0x7f); /* core1编码目的buffer剩余空间小于配置的阈值时上溢中断屏蔽控制 */

    /* 屏蔽解码通道异常中断 */
    SOCP_REG_WRITE(SOCP_REG_APP_DEC_MASK1, VOS_NULL_DWORD); /* core1解码通路 */

    s32IntLevel = mdrv_int_get_num(BSP_INT_TYPE_SOCP_C);

    if(MDRV_ERROR == s32IntLevel)
    {
        SOCP_LOG_ERR("SocpCcpuInit: Query modem core int failed",0,0);
        return MDRV_ERROR;
    }

#if 0   /* 保留此处代码用于后续扩展 */
    /* 挂Ccpu Socp中断 ，需要调试才可以使能*/
    if(VOS_OK != DRV_VICINT_CONNECT((VOIDFUNCPTR *)s32IntLevel, (VOIDFUNCPTR)SocpModemInthandler, 0))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SOCP, "SocpCcpuInit: connect modem core int failed.\n",0,0,0,0,0,0);
        return MDRV_ERROR;
    }

    if (VOS_OK != mdrv_int_enable(s32IntLevel))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SOCP, "SocpCcpuInit: enable int failed.\n", 0, 0, 0, 0, 0, 0);
        return MDRV_ERROR;
    }

    if (VOS_OK != SocpCreateTask())
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SOCP, "SocpAcpuCreateTask: create task failed(0x%x).\n", s32Result);

        return MDRV_ERROR;
    }
#endif

    vos_printf("\r\n ---------------CCPU SocpInit end--------------------\r\n");

    return VOS_OK;
}
#endif

/*****************************************************************************
* 函 数 名  : socpInit
*
* 功能描述  : 模块初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/

/*lint -e529*/
VOS_INT32 SocpInit(VOS_VOID)
{
    VOS_INT32 s32Result;

    if(VOS_TRUE == g_strSocpStat.bInitFlag)
    {
        return VOS_OK;
    }

    DRV_EXCH_CUST_FUNC_REG((exchCBReg)SOCP_Log_Save);

    VOS_SpinLockInit(&g_stSocpSpinLock);

    SocpInitGobal();    /*初始化 A和C 的全局变量*/

#ifdef SOCP_ACORE
    s32Result = SocpAcpuInit();
#endif

#ifdef SOCP_CCORE
    s32Result = SocpCcpuInit();
#endif

    if(VOS_OK != s32Result)
    {
        SOCP_LOG_ERR("SocpInit: Init Func failed", (VOS_UINT32)s32Result, 0);

        return s32Result;
    }

    /* 设置初始化状态 */
    g_strSocpStat.bInitFlag = VOS_TRUE;

    return VOS_OK;
}
/*****************************************************************************
* 函 数 名  : BSP_SOCP_CoderSetSrcChan
*
* 功能描述  : 编码源通道配置函数
*
* 输入参数  : pSrcAttr     编码源通道配置参数
*             ulSrcChanID  编码源通道ID
*
* 输出参数  : 无
*
* 返 回 值  : 申请及配置成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_CoderSetSrcChan(SOCP_CODER_SRC_ENUM_U32    enSrcChanID,
                                    SOCP_CODER_SRC_CHAN_S     *pSrcAttr)
{
    VOS_UINT32             start;
    VOS_ULONG              ulStartTmp;
    VOS_UINT32             end;
    VOS_ULONG              ulEndTmp;
    VOS_UINT32             rdstart         = 0;
    VOS_ULONG              ulRdStart       = 0;
    VOS_UINT32             rdend           = 0;
    VOS_ULONG              ulRdEnd         = 0;
    VOS_UINT32             buflength       = 0;
    VOS_UINT32             Rdbuflength     = 0;
    VOS_UINT32             srcChanId;
    VOS_UINT32             u32SrcChanType;
    VOS_UINT32             u32DstChanID;
    VOS_UINT32             u32DstChanType;
    VOS_UINT32             ResetFlag;
    VOS_UINT32             i;
    SOCP_ENCSRC_CHAN_STRU *pChan;
    VOS_UINT32             ulVersion;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pSrcAttr);
    SOCP_CHECK_CHAN_PRIORITY(pSrcAttr->ePriority);
    SOCP_CHECK_DATA_TYPE(pSrcAttr->eDataType);
    SOCP_CHECK_DATA_TYPE_EN(pSrcAttr->eDataTypeEn);
    SOCP_CHECK_ENC_DEBUG_EN(pSrcAttr->eDebugEn);

    srcChanId       = SOCP_REAL_CHAN_ID(enSrcChanID);
    u32SrcChanType  = SOCP_REAL_CHAN_TYPE(enSrcChanID);

    SOCP_CHECK_CHAN_TYPE(u32SrcChanType, SOCP_CODER_SRC_CHAN);
    SOCP_CHECK_CHAN_ID(srcChanId, SOCP_MAX_ENCSRC_CHN);

    u32DstChanID   = SOCP_REAL_CHAN_ID(pSrcAttr->u32DestChanID);
    u32DstChanType = SOCP_REAL_CHAN_TYPE(pSrcAttr->u32DestChanID);

    SOCP_CHECK_CHAN_TYPE(u32DstChanType, SOCP_CODER_DEST_CHAN);
    SOCP_CHECK_CHAN_ID(u32DstChanID, SOCP_MAX_ENCDST_CHN);

    if ((SOCP_ENCSRC_CHNMODE_CTSPACKET != pSrcAttr->eMode)
        && (SOCP_ENCSRC_CHNMODE_LIST != pSrcAttr->eMode))
    {
        SOCP_LOG_ERR("CoderAllocSrcChan: chnnel mode is invalid ", pSrcAttr->eMode, 0);

        return BSP_ERR_SOCP_INVALID_PARA;
    }

    /* 使用配置参数进行配置 */
    /* 判断起始地址是否8字节对齐 */
    ulStartTmp = (VOS_ULONG)pSrcAttr->sCoderSetSrcBuf.pucInputStart;
    start      = (VOS_UINT32)(ulStartTmp & 0xFFFFFFFFUL);
    ulEndTmp   = (VOS_ULONG)pSrcAttr->sCoderSetSrcBuf.pucInputEnd;
    end        = (VOS_UINT32)(ulEndTmp & 0xFFFFFFFFUL);

    SOCP_CHECK_PARA(start);
    SOCP_CHECK_8BYTESALIGN(start);
    SOCP_CHECK_PARA(end);
    SOCP_CHECK_BUF_ADDR(start, end);

    buflength = end - start + 1;

    SOCP_CHECK_8BYTESALIGN(buflength);

    SOCP_REG_READ(SOCP_REG_VERSION, ulVersion);

    if (SOCP_OLD_VER == ulVersion)
    {
        if(buflength > SOCP_ENC_SRC_BUFLGTH_MAX)
        {
            SOCP_LOG_ERR("CoderAllocSrcChan: buffer length is too large", buflength, 0);

            return BSP_ERR_SOCP_INVALID_PARA;
        }
    }

    /* 如果是用链表缓冲区，则配置RDbuffer的起始地址和长度 */
    if(SOCP_ENCSRC_CHNMODE_LIST == pSrcAttr->eMode)
    {
        /* 判断RDBuffer的起始地址是否8字节对齐 */
        ulRdStart = (VOS_ULONG)pSrcAttr->sCoderSetSrcBuf.pucRDStart;
        rdstart = (VOS_UINT32)(ulRdStart & 0xFFFFFFFFUL);

        ulRdEnd   = (VOS_ULONG)pSrcAttr->sCoderSetSrcBuf.pucRDEnd;
        rdend   = (VOS_UINT32)(ulRdEnd & 0xFFFFFFFFUL);

        SOCP_CHECK_PARA(rdstart);
        SOCP_CHECK_8BYTESALIGN(rdstart);
        SOCP_CHECK_PARA(rdend);
        SOCP_CHECK_BUF_ADDR(rdstart, rdend);
        /* SOCP_CHECK_PARA(pSrcAttr->sCoderSetSrcBuf.u32RDThreshold); 暂时不需要配置参数*/

        Rdbuflength = rdend - rdstart + 1;

        SOCP_CHECK_8BYTESALIGN(Rdbuflength);

        if(Rdbuflength > SOCP_ENC_SRC_RDLGTH_MAX)
        {
            SOCP_LOG_ERR("CoderAllocSrcChan: RD buffer length is too large", Rdbuflength, 0);

            return BSP_ERR_SOCP_INVALID_PARA;
        }
    }

    /* 复位通道 */
    SOCP_REG_SETBITS(SOCP_REG_ENCRST, srcChanId, 1, 1);

    /* 等待通道自清 */
    for (i = 0; i < SOCP_RESET_TIME; i++)
    {
        ResetFlag = SOCP_REG_GETBITS(SOCP_REG_ENCRST, srcChanId, 1);

        if(0 == ResetFlag)
        {
            break;
        }
    }

    if (SOCP_RESET_TIME == i)
    {
        SOCP_LOG_ERR("CoderAllocSrcChan: reset channel failed!", srcChanId, 0);
    }

    /* 写入起始地址到源buffer起始地址寄存器*/
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFADDR(srcChanId),SOCP_VIRT_PHY(start));
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(srcChanId),SOCP_VIRT_PHY(start));
    SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFRPTR(srcChanId),SOCP_VIRT_PHY(start));

    /* SOCP在V8R2上进行对源通道配置进行扩展 */
    if (SOCP_OLD_VER == ulVersion)
    {
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(srcChanId), 0, 27, buflength);
    }
    else
    {
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG0(srcChanId), 0, 31, buflength);
    }

    /* 如果是用链表缓冲区，则配置RDbuffer的起始地址和长度 */
    if(SOCP_ENCSRC_CHNMODE_LIST == pSrcAttr->eMode)
    {
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQADDR(srcChanId),SOCP_VIRT_PHY(rdstart));
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQRPTR(srcChanId),SOCP_VIRT_PHY(rdstart));
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQWPTR(srcChanId),SOCP_VIRT_PHY(rdstart));
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_RDQCFG(srcChanId), 0, 16, Rdbuflength);
    }

    /*配置其它参数*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 1, 2, pSrcAttr->eMode);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 4, 4, pSrcAttr->u32DestChanID);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 8, 2, pSrcAttr->ePriority);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 10, 1, pSrcAttr->u32BypassEn);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 11, 1, pSrcAttr->eDataTypeEn);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 31, 1, pSrcAttr->eDebugEn);
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(srcChanId), 16, 8, pSrcAttr->eDataType);

    /* 在g_strSocpStat中保存参数*/
    pChan = &g_strSocpStat.sEncSrcChan[srcChanId];

    pChan->u32SetStat             = SOCP_CHN_SET;
    pChan->eChnMode               = pSrcAttr->eMode;
    pChan->sEncSrcBuf.u32Start    = start;
    pChan->sEncSrcBuf.u32End      = end;
    pChan->sEncSrcBuf.u32Write    = start;
    pChan->sEncSrcBuf.u32Read     = start;
    pChan->sEncSrcBuf.u32Length   = buflength;
    pChan->sEncSrcBuf.u32IdleSize = 0;

    if(SOCP_ENCSRC_CHNMODE_LIST == pSrcAttr->eMode)
    {
        pChan->sRdBuf.u32Start    = rdstart;
        pChan->sRdBuf.u32End      = rdend;
        pChan->sRdBuf.u32Write    = rdstart;
        pChan->sRdBuf.u32Read     = rdstart;
        pChan->sRdBuf.u32Length   = Rdbuflength;
    }

    return VOS_OK;
}


/*****************************************************************************
* 函 数 名  : SocpEncSrcStart
*
* 功能描述  : 编码或者解码启动函数
*
* 输入参数  : ulRealChanID      通道ID
* 输出参数  :
*
* 返 回 值  : 启动成功与否的标识码
*****************************************************************************/
/*lint -e529*/
VOS_UINT32 SocpEncSrcStart(VOS_UINT32 ulRealChanID)
{
    VOS_UINT32      ulDstId;
#ifdef SOCP_CCORE
    VOS_UINT32      ulStart;
#endif

    SOCP_CHECK_CHAN_ID(ulRealChanID, SOCP_MAX_ENCSRC_CHN);
    SOCP_CHECK_ENCSRC_SET(ulRealChanID);

    /* 编码通道 */
    ulDstId = SOCP_REG_GETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulRealChanID), 4, 4);

#ifdef SOCP_ACORE
    if (SOCP_CHN_SET != g_strSocpStat.sEncDstChan[ulDstId].u32SetStat)
    {
        SOCP_LOG_ERR("BSP_SOCP_Start: Acpu enc src's dst Channel is valid", ulRealChanID, 0);
        return BSP_ERR_SOCP_DEST_CHAN;
    }

    SOCP_REG_SETBITS(SOCP_REG_ENC_RAWINT1, ulRealChanID, 1, 1);
    SOCP_REG_SETBITS(SOCP_REG_APP_MASK1, ulRealChanID, 1, 0);
#endif

#ifdef SOCP_CCORE
    /* 通过寄存器判断 */
    SOCP_REG_READ(SOCP_REG_ENCDEST_BUFADDR(ulDstId), ulStart);

    if (0 == ulStart)
    {
        SOCP_LOG_ERR("BSP_SOCP_Start: Ccpu enc dst Channel is valid", ulRealChanID, 0);
        return BSP_ERR_SOCP_DEST_CHAN;
    }
#endif

    /* 设置打开状态*/
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulRealChanID), 0, 1, 1);

    g_strSocpStat.sEncSrcChan[ulRealChanID].u32ChanEn = SOCP_CHN_ENABLE;
    g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpStartEncSrcCnt[ulRealChanID]++;

    return VOS_OK;
}

/*lint +e529*/

/*****************************************************************************
* 函 数 名  : BSP_SOCP_Start
*
* 功能描述  : 编码或者解码启动函数
*
* 输入参数  : u32SrcChanID      通道ID
* 输出参数  :
*
* 返 回 值  : 启动成功与否的标识码
*****************************************************************************/

/*lint -e529*/
VOS_UINT32 BSP_SOCP_Start(VOS_UINT32 u32SrcChanID)
{
    VOS_UINT32 u32RealChanID;
    VOS_UINT32 u32ChanType;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断通道ID是否有效 */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        return SocpEncSrcStart(u32RealChanID);
    }

#ifdef SOCP_ACORE
    else if (SOCP_DECODER_SRC_CHAN == u32ChanType)
    {
        return SocpDecSrcStart(u32RealChanID);
    }
#endif
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_Start: invalid Channel type", u32SrcChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }
}

/*lint +e529*/

/*****************************************************************************
* 函 数 名  : BSP_SOCP_Stop
*
* 功能描述  : 编码或者解码停止函数
*
* 输入参数  : u32SrcChanID      通道ID
*
* 输出参数  :
*
* 返 回 值  : 停止成功与否的标识码
*****************************************************************************/

/*lint -e529*/
VOS_UINT32 BSP_SOCP_Stop(VOS_UINT32 u32SrcChanID)
{
    VOS_UINT32 u32RealChanID;
    VOS_UINT32 u32ChanType;

#ifdef SOCP_ACORE
    VOS_UINT32 IntIDMask = 0;
    VOS_UINT32 i;
#endif

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    SOCP_LOG_ERR("BSP_SOCP_Stop Channel ID", u32SrcChanID, 0);

    /* 判断通道ID是否有效 */
    u32RealChanID = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        SOCP_CHECK_CHAN_ID(u32RealChanID, SOCP_MAX_ENCSRC_CHN);
        SOCP_CHECK_ENCSRC_SET(u32RealChanID);

        /* 关闭中断*/
#ifdef SOCP_ACORE
        SOCP_REG_SETBITS(SOCP_REG_APP_MASK1, u32RealChanID, 1, 1);
#endif

        /* 设置通道关闭状态*/
        SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(u32RealChanID), 0, 1, 0);

        g_strSocpStat.sEncSrcChan[u32RealChanID].u32ChanEn = SOCP_CHN_DISABLE;

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpStopEncSrcCnt[u32RealChanID]++;
    }

#ifdef SOCP_ACORE
    else if (SOCP_DECODER_SRC_CHAN == u32ChanType)
    {
        SOCP_CHECK_CHAN_ID(u32RealChanID, SOCP_MAX_DECSRC_CHN);
        SOCP_CHECK_DECSRC_SET(u32RealChanID);

        /* 关闭中断*/
        if (SOCP_DECSRC_CHNMODE_LIST == g_strSocpStat.sDecSrcChan[u32RealChanID].eChnMode)
        {
            SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK0, u32RealChanID, 1, 1);
        }

        for (i = 1; i < SOCP_DEC_SRCINT_NUM; i++)
        {
            IntIDMask  = SOCP_REG_GETBITS(SOCP_REG_DEC_MASK1, i * 4, 4);
            IntIDMask |= 1 << u32RealChanID;
            SOCP_REG_SETBITS(SOCP_REG_DEC_CORE0MASK0, i * 4, 4, IntIDMask);
        }

        /* 设置通道关闭状态*/
        SOCP_REG_SETBITS(SOCP_REG_DECSRC_BUFCFG0(u32RealChanID), 30, 1, 0);

        g_strSocpStat.sDecSrcChan[u32RealChanID].u32ChanEn = SOCP_CHN_DISABLE;

        g_stSocpDebugInfo.sSocpDebugDecSrc.u32SocpStopDecSrcCnt[u32RealChanID]++;
    }
#endif
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_Stop: invalid Channel type", u32SrcChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return VOS_OK;
}

/*lint +e529*/
/*****************************************************************************
* 函 数 名  : BSP_SOCP_GetWriteBuff
*
* 功能描述  : 上层获取写数据buffer函数
*
* 输入参数  : u32SrcChanID    源通道ID
* 输出参数  : pBuff           获取的buffer
*
* 返 回 值  : 获取成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_GetWriteBuff(VOS_UINT32 u32SrcChanID, SOCP_BUFFER_RW_STRU *pBuff)
{
    VOS_UINT32 u32ChanID;
    VOS_UINT32 u32ChanType;
    VOS_UINT32 uPAddr;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pBuff);

    SOCP_LOG_INFO("BSP_SOCP_GetWriteBuff: Channel id ", u32SrcChanID, 0);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpGetWBufEncSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_ENCSRC_CHN);
        SOCP_CHECK_ENCSRC_SET(u32ChanID);

        /* 根据读写指针获取buffer */
        SOCP_REG_READ(SOCP_REG_ENCSRC_BUFRPTR(u32ChanID), uPAddr);

        g_strSocpStat.sEncSrcChan[u32ChanID].sEncSrcBuf.u32Read =(VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_ENCSRC_BUFWPTR(u32ChanID), uPAddr);

        g_strSocpStat.sEncSrcChan[u32ChanID].sEncSrcBuf.u32Write= (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetIdleBuffer(&g_strSocpStat.sEncSrcChan[u32ChanID].sEncSrcBuf, pBuff);

        SOCP_LOG_INFO("BSP_SOCP_GetWriteBuff: Enc Src Buffer Info", (VOS_UINT32)pBuff->pBuffer, pBuff->u32Size);

        SOCP_LOG_INFO("BSP_SOCP_GetWriteBuff: Enc Src Buffer Rb Info", (VOS_UINT32)pBuff->pRbBuffer, pBuff->u32RbSize);

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpGetWBufEncSrcSucCnt[u32ChanID]++;
    }
#ifdef SOCP_ACORE
    else if (SOCP_DECODER_SRC_CHAN == u32ChanType) /* 解码通道 */
    {
        g_stSocpDebugInfo.sSocpDebugDecSrc.u32SocpGetWBufDecSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_DECSRC_CHN);
        SOCP_CHECK_DECSRC_SET(u32ChanID);

        SOCP_REG_READ(SOCP_REG_DECSRC_BUFRPTR(u32ChanID), uPAddr);

        g_strSocpStat.sDecSrcChan[u32ChanID].sDecSrcBuf.u32Read = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_DECSRC_BUFWPTR(u32ChanID), uPAddr);

        g_strSocpStat.sDecSrcChan[u32ChanID].sDecSrcBuf.u32Write= (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetIdleBuffer(&g_strSocpStat.sDecSrcChan[u32ChanID].sDecSrcBuf, pBuff);

        SOCP_LOG_INFO("BSP_SOCP_GetWriteBuff: Dec Src Buffer Info", (VOS_UINT32)pBuff->pBuffer, pBuff->u32Size);

        SOCP_LOG_INFO("BSP_SOCP_GetWriteBuff: Dec Src Buffer Rb Info", (VOS_UINT32)pBuff->pRbBuffer, pBuff->u32RbSize);

        g_stSocpDebugInfo.sSocpDebugDecSrc.u32SocpGetWBufDecSrcSucCnt[u32ChanID]++;
    }
#endif  /*SOCP_ACORE*/
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_GetWriteBuff: invalid Channel type", u32SrcChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_SOCP_WriteDone
*
* 功能描述  : 写数据完成函数
*
* 输入参数  : u32SrcChanID    源通道ID
              u32WrtSize      写入数据的长度
*
* 输出参数  :
*
* 返 回 值  : 操作完成与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_WriteDone(VOS_UINT32 u32SrcChanID, VOS_UINT32 u32WrtSize)
{
    VOS_UINT32             u32ChanID;
    VOS_UINT32             u32ChanType;
    VOS_UINT32             u32WrtPad;
    SOCP_BUFFER_RW_STRU    stRwBuff;
    VOS_UINT32             uPAddr;
#if(VOS_OS_VER == VOS_WIN32)    /* for PC Stub */
    VOS_UINT32          ulWritePtr;
#endif
    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(u32WrtSize);

    SOCP_LOG_INFO("BSP_SOCP_WriteDone: Channel id ", u32SrcChanID, u32WrtSize);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        SOCP_ENCSRC_CHAN_STRU *pChan;

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpWriteDoneEncSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_ENCSRC_CHN);
        SOCP_CHECK_ENCSRC_SET(u32ChanID);

        pChan = &g_strSocpStat.sEncSrcChan[u32ChanID];

        u32WrtPad = u32WrtSize % 8;

        if (0 != u32WrtPad)
        {
            u32WrtSize += (8 - u32WrtPad);
        }

        SOCP_REG_READ(SOCP_REG_ENCSRC_BUFWPTR(u32ChanID), uPAddr);

        pChan->sEncSrcBuf.u32Write= (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_ENCSRC_BUFRPTR(u32ChanID), uPAddr);

        pChan->sEncSrcBuf.u32Read = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetIdleBuffer(&pChan->sEncSrcBuf, &stRwBuff);

        if (stRwBuff.u32Size + stRwBuff.u32RbSize < u32WrtSize)
        {
            SOCP_LOG_ERR("BSP_SOCP_WriteDone: enc src too large write size. write ptr is, read ptr is",
                      pChan->sEncSrcBuf.u32Write, pChan->sEncSrcBuf.u32Read);

            SOCP_LOG_ERR("BSP_SOCP_WriteDone: enc src too large write size. u32Size is, u32RbSize is",
                      stRwBuff.u32Size, stRwBuff.u32RbSize);

            g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpWriteDoneEncSrcFailCnt[u32ChanID]++;

            return BSP_ERR_SOCP_INVALID_PARA;
        }

#if(VOS_OS_VER == VOS_WIN32)    /* for PC Stub */
        ulWritePtr = pChan->sEncSrcBuf.u32Write;
#endif

        /* 设置读写指针 */
        SocpWriteDone(&pChan->sEncSrcBuf, u32WrtSize);

        /* 写入写指针到写指针寄存器*/
        uPAddr = SOCP_VIRT_PHY(pChan->sEncSrcBuf.u32Write);

        SOCP_REG_WRITE(SOCP_REG_ENCSRC_BUFWPTR(u32ChanID), uPAddr);

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpWriteDoneEncSrcSucCnt[u32ChanID]++;
    }

#ifdef SOCP_ACORE
    else  if(SOCP_DECODER_SRC_CHAN == u32ChanType) /* 解码通道 */
    {
        SOCP_DECSRC_CHAN_STRU  *pChan;

        g_stSocpDebugInfo.sSocpDebugDecSrc.u32SocpWriteDoneDecSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_DECSRC_CHN);
        SOCP_CHECK_DECSRC_SET(u32ChanID);

        pChan = &g_strSocpStat.sDecSrcChan[u32ChanID];

        SOCP_REG_READ(SOCP_REG_DECSRC_BUFWPTR(u32ChanID), uPAddr);

        pChan->sDecSrcBuf.u32Write= (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_DECSRC_BUFRPTR(u32ChanID), uPAddr);

        pChan->sDecSrcBuf.u32Read = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetIdleBuffer(&pChan->sDecSrcBuf, &stRwBuff);

        if (stRwBuff.u32Size + stRwBuff.u32RbSize < u32WrtSize)
        {
            SOCP_LOG_ERR("BSP_SOCP_WriteDone: dec src too large write size. write ptr is, read ptr is",
                      pChan->sDecSrcBuf.u32Write, pChan->sDecSrcBuf.u32Read);

            SOCP_LOG_ERR("BSP_SOCP_WriteDone: dec src too large write size. u32Size is, u32RbSize is",
                      stRwBuff.u32Size, stRwBuff.u32RbSize);

            g_stSocpDebugInfo.sSocpDebugDecSrc.u32SocpWriteDoneDecSrcFailCnt[u32ChanID]++;

            return BSP_ERR_SOCP_INVALID_PARA;
        }

#if(VOS_OS_VER == VOS_WIN32)    /* for PC Stub */
        ulWritePtr = pChan->sDecSrcBuf.u32Write;
#endif

        /* 设置读写指针 */
        SocpWriteDone(&pChan->sDecSrcBuf, u32WrtSize);

        /* 写入写指针到写指针寄存器*/
        uPAddr = SOCP_VIRT_PHY(pChan->sDecSrcBuf.u32Write);

        SOCP_REG_WRITE(SOCP_REG_DECSRC_BUFWPTR(u32ChanID), uPAddr);

        g_stSocpDebugInfo.sSocpDebugDecSrc.u32SocpWriteDoneDecSrcSucCnt[u32ChanID]++;
    }
#endif
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_WriteDone: invalid Channel type", u32SrcChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

#if(VOS_OS_VER == VOS_WIN32)    /* for PC Stub */
    Stub_SendSocpData(u32SrcChanID, (VOS_UINT8*)ulWritePtr, u32WrtSize);
#endif

    VOS_FlushCpuWriteBuf();

    return VOS_OK;
}


/*****************************************************************************
* 函 数 名  : BSP_SOCP_GetRDBuffer
*
* 功能描述  : 获取RDbuffer函数
*
* 输入参数  : u32SrcChanID    源通道ID
*
* 输出参数  : pBuff           获取的RDbuffer
*
* 返 回 值  : 获取成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_GetRDBuffer(VOS_UINT32 u32SrcChanID, SOCP_BUFFER_RW_STRU *pBuff)
{
    VOS_UINT32     u32ChanID;
    VOS_UINT32     u32ChanType;
    VOS_UINT32     uPAddr;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(pBuff);

    SOCP_LOG_INFO("BSP_SOCP_GetRDBuffer: Channel is", u32SrcChanID, 0);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpGetRdBufEncSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_ENCSRC_CHN);
        SOCP_CHECK_ENCSRC_SET(u32ChanID);

        /* 根据读写指针获取buffer */
        SOCP_REG_READ(SOCP_REG_ENCSRC_RDQRPTR(u32ChanID), uPAddr);
        g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf.u32Read = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_ENCSRC_RDQWPTR(u32ChanID), uPAddr);
        g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf.u32Write= (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetDataBuffer(&g_strSocpStat.sEncSrcChan[u32ChanID].sRdBuf, pBuff);

        SOCP_LOG_INFO("BSP_SOCP_GetRDBuffer: pBuff Info is", (VOS_UINT32)pBuff->pBuffer, pBuff->u32Size);

        SOCP_LOG_INFO("BSP_SOCP_GetRDBuffer: pBuff Rb Info is", (VOS_UINT32)pBuff->pRbBuffer, pBuff->u32RbSize);

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpGetRdBufEncSrcSucCnt[u32ChanID]++;
    }
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_GetRDBuffer: invalid Channel type", u32SrcChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    return VOS_OK;
}


/*****************************************************************************
* 函 数 名  : BSP_SOCP_ReadRDDone
*
* 功能描述  : 读取RDbuffer数据完成函数
*
* 输入参数  : u32SrcChanID   源通道ID
              u32RDSize      读取的RDbuffer数据长度
*
* 输出参数  :
*
* 返 回 值  : 读取成功与否的标识码
*****************************************************************************/
VOS_UINT32 BSP_SOCP_ReadRDDone(VOS_UINT32 u32SrcChanID, VOS_UINT32 u32RDSize)
{
    VOS_UINT32             u32ChanID;
    VOS_UINT32             u32ChanType;
    SOCP_BUFFER_RW_STRU    stRwBuff;
    VOS_UINT32             uPAddr;

    /* 判断是否已经初始化 */
    SOCP_CHECK_INIT();

    /* 判断参数有效性 */
    SOCP_CHECK_PARA(u32RDSize);

    SOCP_LOG_INFO("BSP_SOCP_ReadRDDone: Read RD Done", u32SrcChanID, u32RDSize);

    /* 获得实际通道id */
    u32ChanID   = SOCP_REAL_CHAN_ID(u32SrcChanID);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32SrcChanID);

    /* 编码通道 */
    if (SOCP_CODER_SRC_CHAN == u32ChanType)
    {
        SOCP_ENCSRC_CHAN_STRU *pChan;

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpReadRdDoneEncSrcEtrCnt[u32ChanID]++;

        /* 检验通道id */
        SOCP_CHECK_CHAN_ID(u32ChanID, SOCP_MAX_ENCSRC_CHN);
        SOCP_CHECK_ENCSRC_SET(u32ChanID);

        pChan = &g_strSocpStat.sEncSrcChan[u32ChanID];
        g_strSocpStat.sEncSrcChan[u32ChanID].u32LastRdSize = 0;

        /* 设置读写指针 */
        SOCP_REG_READ(SOCP_REG_ENCSRC_RDQWPTR(u32ChanID), uPAddr);
        pChan->sRdBuf.u32Write = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SOCP_REG_READ(SOCP_REG_ENCSRC_RDQRPTR(u32ChanID), uPAddr);
        pChan->sRdBuf.u32Read  = (VOS_UINT32)SOCP_PHY_VIRT(uPAddr);

        SocpGetDataBuffer(&pChan->sRdBuf, &stRwBuff);

        if ((stRwBuff.u32Size + stRwBuff.u32RbSize) < u32RDSize)
        {
            SOCP_LOG_ERR("BSP_SOCP_ReadRDDone: enc src too large rd size, write ptr is, read ptr is",
                      pChan->sRdBuf.u32Write, pChan->sRdBuf.u32Read);

            SOCP_LOG_ERR("BSP_SOCP_ReadRDDone: enc src too large rd size u32Size is, u32RbSize is",
                      stRwBuff.u32Size, stRwBuff.u32RbSize);

            g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpReadRdDoneEncSrcFailCnt[u32ChanID]++;

            return BSP_ERR_SOCP_INVALID_PARA;
        }

        SocpReadDone(&pChan->sRdBuf, u32RDSize);

        /* 写入写指针到写指针寄存器*/
        uPAddr= SOCP_VIRT_PHY(pChan->sRdBuf.u32Read);
        SOCP_REG_WRITE(SOCP_REG_ENCSRC_RDQRPTR(u32ChanID), uPAddr);

        g_stSocpDebugInfo.sSocpDebugEncSrc.u32SocpReadRdDoneEncSrcSucCnt[u32ChanID]++;
    }
    else
    {
        SOCP_LOG_ERR("BSP_SOCP_ReadRDDone: invalid Channel type", u32SrcChanID, u32ChanType);
        return BSP_ERR_SOCP_INVALID_CHAN;
    }

    VOS_FlushCpuWriteBuf();

    return VOS_OK;
}

/*以下目的通道专用
 */

/*****************************************************************************
* 函 数 名   : SOCP_Help
*
* 功能描述  : 获取socp打印信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_Help(VOS_VOID)
{
    vos_printf("\r |*************************************|\n");
    vos_printf("\r SOCP_ShowDebugGbl   : 查看全局统计信息:通道申请、配置和中断总计数，无参数\n");
    vos_printf("\r SOCP_ShowEncSrcChanCur : 查看编码源通道属性，参数为通道ID\n");
    vos_printf("\r SOCP_ShowEncSrcChanAdd : 查看编码源通道操作统计值，参数为通道ID\n");
    vos_printf("\r SOCP_ShowEncSrcChanAll : 查看所有编码源通道属性和统计值，无参数\n");
#ifdef SOCP_ACORE
    vos_printf("\r SOCP_ShowEncDstChanCur : 查看编码目的通道属性，参数为通道ID\n");
    vos_printf("\r SOCP_ShowEncDstChanAdd : 查看编码目的通道操作统计值，参数为通道ID\n");
    vos_printf("\r SOCP_ShowEncDstChanAll : 查看所有编码目的通道属性和统计值，无参数\n");
    vos_printf("\r SOCP_ShowDecSrcChanCur : 查看解码源通道属性，参数为通道ID\n");
    vos_printf("\r SOCP_ShowDecSrcChanAdd : 查看解码源通道操作统计值，参数为通道ID\n");
    vos_printf("\r SOCP_ShowDecSrcChanAll : 查看所有解码源通道属性和统计值，无参数\n");
#endif
    vos_printf("\r SOCP_ShowDecDstChanCur : 查看解码目的通道属性，参数为通道ID\n");
    vos_printf("\r SOCP_ShowDecDstChanAdd : 查看解码目的通道操作统计值，参数为通道ID\n");
    vos_printf("\r SOCP_ShowDecDstChanAll : 查看所有解码目的通道属性和统计值，无参数\n");

    vos_printf("\r SOCP_debugCntShow : 查看全部统计信息，无参数\n");
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowDebugGbl
*
* 功能描述  : 显示全局debug 计数信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_ShowDebugGbl(VOS_VOID)
{
    SOCP_DEBUG_GBL_STRU *sSocpDebugGblInfo;

    sSocpDebugGblInfo = &g_stSocpDebugInfo.sSocpDebugGBl;

    vos_printf("\r SOCP全局状态维护信息:\n");
    vos_printf("\r socp基地址:                                            : 0x%x\n",
           (VOS_INT32)g_strSocpStat.uBaseAddr);

#ifdef SOCP_ACORE
    vos_printf("\r socp进入APP中断的次数          : 0x%x\n", (VOS_INT32)sSocpDebugGblInfo->u32SocpAppEtrIntCnt);
    vos_printf("\r socp完成APP中断的次数          : 0x%x\n", (VOS_INT32)sSocpDebugGblInfo->u32SocpAppSucIntCnt);
#endif

#ifdef SOCP_CCORE
    vos_printf("\r socp进入Modem中断的次数        : 0x%x\n", (VOS_INT32)sSocpDebugGblInfo->u32SocpModemEtrIntCnt);
    vos_printf("\r socp完成Modem中断的次数        : 0x%x\n", (VOS_INT32)sSocpDebugGblInfo->u32SocpModemSucIntCnt);
#endif

    return;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowEncSrcChanCur
*
* 功能描述  : 打印编码源通道当前属性
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/

/*lint -e529*/
VOS_UINT32 SOCP_ShowEncSrcChanCur(VOS_UINT32 u32UniqueId)
{
    VOS_UINT32 u32RealId   = 0;
    VOS_UINT32 u32ChanType = 0;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_CODER_SRC_CHAN);

    vos_printf("================== 申请的编码源通道 0x%x  属性:=================\n", u32UniqueId);
    vos_printf("通道分配状态:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32SetStat);
    vos_printf("通道使能状态:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32ChanEn);
    vos_printf("通道数据格式类型:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].eChnMode);
    vos_printf("通道buffer 起始地址:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Start);
    vos_printf("通道buffer 结束地址:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32End);
    vos_printf("通道buffer 读指针:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Read);
    vos_printf("通道buffer 写指针:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Write);
    vos_printf("通道buffer 长度:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Length);

    if (SOCP_ENCSRC_CHNMODE_LIST == g_strSocpStat.sEncSrcChan[u32RealId].eChnMode)
    {
        vos_printf("通道RD buffer 起始地址:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Start);
        vos_printf("通道RD buffer 结束地址:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32End);
        vos_printf("通道RD buffer 读指针:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Read);
        vos_printf("通道RD buffer 写指针:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Write);
        vos_printf("通道RD buffer 长度:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Length);
    }

    return VOS_OK;
}

/*lint +e529*/

/*****************************************************************************
* 函 数 名   : SOCP_ShowEncSrcChanAdd
*
* 功能描述  : 打印编码源通道累计统计值
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_UINT32 SOCP_ShowEncSrcChanAdd(VOS_UINT32 u32UniqueId)
{
    VOS_UINT32             u32ChanType;
    VOS_UINT32             u32RealChanID;
    SOCP_DEBUG_ENCSRC_STRU *sSocpAddDebugEncSrc;

    u32RealChanID   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType     = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_CODER_SRC_CHAN);

    sSocpAddDebugEncSrc = &g_stSocpDebugInfo.sSocpDebugEncSrc;


    vos_printf("================== 编码源通道 0x%x  累计统计值:=================\n",
            u32UniqueId);
    vos_printf("\r socp启动编码源通道成功的次数                           : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpStartEncSrcCnt[u32RealChanID]);
    vos_printf("\r socp停止编码源通道成功的次数                           : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpStopEncSrcCnt[u32RealChanID]);
    vos_printf("\r socp注册编码源通道异常处理函数的次数               : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpRegEventEncSrcCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道尝试获得写buffer的次数                  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpGetWBufEncSrcEtrCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道获得写buffer成功的次数                  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpGetWBufEncSrcSucCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道尝试更新写buffer指针的次数            : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpWriteDoneEncSrcEtrCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道更新写buffer指针成功的次数            : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpWriteDoneEncSrcSucCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道更新写buffer指针失败的次数            : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpWriteDoneEncSrcFailCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道尝试获得RD buffer的次数                   : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpGetRdBufEncSrcEtrCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道获得RD buffer成功的次数                   : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpGetRdBufEncSrcSucCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道尝试更新RDbuffer指针的次数              : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcEtrCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道更新RDbuffer指针成功的次数              : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcSucCnt[u32RealChanID]);
    vos_printf("\r socp编码源通道更新RDbuffer指针失败的次数              : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcFailCnt[u32RealChanID]);
    vos_printf("\r socp ISR 中进入编码源通道包头错误中断次数                  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpEncSrcIsrHeadIntCnt[u32RealChanID]);
    vos_printf("\r socp ISR 中进入编码源通道Rd 完成中断次数                     : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncSrc->u32SocpEncSrcIsrRdIntCnt[u32RealChanID]);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowEncSrcChanAll
*
* 功能描述  : 打印所有编码源通道信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_ShowEncSrcChanAll(VOS_VOID)
{
    VOS_UINT32 i;

    for (i=0; i<SOCP_MAX_ENCSRC_CHN; i++)
    {
        (VOS_VOID)SOCP_ShowEncSrcChanCur(i);
        (VOS_VOID)SOCP_ShowEncSrcChanAdd(i);
    }

    return;
}

#ifdef SOCP_ACORE
/*****************************************************************************
* 函 数 名   : SOCP_ShowEncDstSemInfo
*
* 功能描述  : 打印编码目的通道Sem信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_ShowEncDstSemInfo(VOS_VOID)
{
    VOS_UINT8       ucLoop;
    VOS_UINT32      ulChannelId;

    for (ucLoop = 0; ucLoop < SOCP_MAX_ENCDST_CHN; ucLoop++)
    {
        if (0 == ucLoop)
        {
            ulChannelId = SOCP_CODER_DST_OM_CNF;
        }
        else
        {
            ulChannelId = SOCP_CODER_DST_OM_IND;
        }

        vos_printf("CH0 延时写入超时释放:\t\t%d\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulTimerNum);
        vos_printf("CH0 延时写入超时释放时间:\t\t0x%x\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulTimerSlice);
        vos_printf("CH0 延时写入flush释放:\t\t%d\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulFlushNum);
        vos_printf("CH0 延时写入flush释放时间:\t\t0x%x\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulFlushSlice);
        vos_printf("CH0 中断释放:\t\t%d\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntNum);
        vos_printf("CH0 中断释放时间:\t\t0x%x\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulIntSlice);
        vos_printf("CH0 任务获取失败:\t\t%d\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulTaskErrNum);
        vos_printf("CH0 任务获取失败时间:\t\t0x%x\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulTaskErrSlice);
        vos_printf("CH0 任务获取成功:\t\t%d\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulTaskNum);
        vos_printf("CH0 任务获取成功时间:\t\t0x%x\n", g_stSocpDebugInfo.stSocpEncDstSemInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulTaskSlice);

    }

    return;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowEncDstChanCur
*
* 功能描述  : 打印编码目的通道信息
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_UINT32 SOCP_ShowEncDstChanCur(VOS_UINT32 u32UniqueId)
{
    VOS_UINT32 u32RealId   = 0;
    VOS_UINT32 u32ChanType = 0;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_CODER_DEST_CHAN);

    vos_printf("================== 编码目的通道 0x%x  属性:=================\n", u32UniqueId);
    vos_printf("通道ID:\t\t%d\n", g_strSocpStat.sEncDstChan[u32RealId].u32ChanID);
    vos_printf("通道配置状态:\t\t%d\n", g_strSocpStat.sEncDstChan[u32RealId].u32SetStat);
    vos_printf("通道buffer 起始地址:\t\t0x%x\n", g_strSocpStat.sEncDstChan[u32RealId].sEncDstBuf.u32Start);
    vos_printf("通道buffer 结束地址:\t\t0x%x\n", g_strSocpStat.sEncDstChan[u32RealId].sEncDstBuf.u32End);
    vos_printf("通道buffer 读指针:\t\t0x%x\n", g_strSocpStat.sEncDstChan[u32RealId].sEncDstBuf.u32Read);
    vos_printf("通道buffer 写指针:\t\t0x%x\n", g_strSocpStat.sEncDstChan[u32RealId].sEncDstBuf.u32Write);
    vos_printf("通道buffer 长度:\t\t0x%x\n", g_strSocpStat.sEncDstChan[u32RealId].sEncDstBuf.u32Length);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowEncDstChanAdd
*
* 功能描述  : 打印编码目的通道累计统计值
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_UINT32 SOCP_ShowEncDstChanAdd(VOS_UINT32 u32UniqueId)
{
    VOS_UINT32              u32RealChanID;
    VOS_UINT32              u32ChanType = 0;
    SOCP_DEBUG_ENCDST_STRU *sSocpAddDebugEncDst;

    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_CODER_DEST_CHAN);

    u32RealChanID = SOCP_REAL_CHAN_ID(u32UniqueId);

    sSocpAddDebugEncDst = &g_stSocpDebugInfo.sSocpDebugEncDst;

    vos_printf("================== 编码目的通道 0x%x  累计统计值:=================\n",
            u32UniqueId);
    vos_printf("\r socp注册编码目的通道异常处理函数的次数           : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpRegEventEncDstCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道注册读数据回调函数成功的次数   : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpRegReadCBEncDstCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道尝试获得读buffer 的次数                : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpGetReadBufEncDstEtrCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道获得读buffer成功的次数                : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpGetReadBufEncDstSucCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道尝试更新读buffer指针的次数          : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpReadDoneEncDstEtrCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道更新读buffer指针成功的次数          : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpReadDoneEncDstSucCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道更新读buffer指针失败的次数          : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpReadDoneEncDstFailCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道更新读buffer指针移动非0 字节成功的次数    : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpReadDoneValidEncDstCnt[u32RealChanID]);
    vos_printf("\r socpISR 中进入编码目的通道传输完成中断次数  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstIsrTrfIntCnt[u32RealChanID]);
    vos_printf("\r socpISR 中进入编码目的通道传输完成中断，但通道异常，统计次数  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstIsrTrfIntINvalidChannelCnt[u32RealChanID]);
    vos_printf("\r socp任务中回调编码目的通道传输完成中断处理函数次数  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstTskTrfCbOriCnt[u32RealChanID]);
    vos_printf("\r socp回调编码目的通道传输完成中断处理函数成功的次数  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstTskTrfCbCnt[u32RealChanID]);
    vos_printf("\r socp任务中回调编码目的通道buf 溢出中断处理函数次数    : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstTskOvfCbOriCnt[u32RealChanID]);
    vos_printf("\r socp回调编码目的通道buf 溢出中断处理函数成功的次数    : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstTskOvfCbCnt[u32RealChanID]);
    vos_printf("\r socpISR 中进入编码目的通道buf阈值溢出中断次数    : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstIsrThresholdOvfIntCnt[u32RealChanID]);
    vos_printf("\r socp任务中回调编码目的通道buf阈值溢出中断处理函数次数    : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstTskThresholdOvfCbOriCnt[u32RealChanID]);
    vos_printf("\r socp回调编码目的通道buf阈值溢出中断处理函数成功的次数    : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstTskThresholdOvfCbCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道响应AT^LOGSAVE命令主动保存log次数    : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstLogSaveOriCnt[u32RealChanID]);
    vos_printf("\r socp编码目的通道响应AT^LOGSAVE命令主动保存log完成次数    : 0x%x\n",
           (VOS_INT32)sSocpAddDebugEncDst->u32SocpEncDstLogSaveCnt[u32RealChanID]);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowEncDstChanAll
*
* 功能描述  : 打印编码目的通道信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_ShowEncDstChanAll(VOS_VOID)
{
    VOS_UINT32 i;
    VOS_UINT32 u32UniqueId = 0;

    for (i = 0; i < SOCP_MAX_ENCDST_CHN; i++)
    {
        u32UniqueId = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, i);
        (VOS_VOID)SOCP_ShowEncDstChanCur(u32UniqueId);
        (VOS_VOID)SOCP_ShowEncDstChanAdd(u32UniqueId);
    }

    return;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowDecSrcChanCur
*
* 功能描述  : 打印解码源通道信息
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_UINT32 SOCP_ShowDecSrcChanCur(VOS_UINT32 u32UniqueId)
{
    VOS_UINT32 u32RealId   = 0;
    VOS_UINT32 u32ChanType = 0;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_DECODER_SRC_CHAN);

    vos_printf("================== 解码源通道 0x%x  属性:=================\n", u32UniqueId);
    vos_printf("通道ID:\t\t%d\n",               g_strSocpStat.sDecSrcChan[u32RealId].u32ChanID);
    vos_printf("通道配置状态:\t\t%d\n",         g_strSocpStat.sDecSrcChan[u32RealId].u32SetStat);
    vos_printf("通道使能状态:\t\t%d\n",         g_strSocpStat.sDecSrcChan[u32RealId].u32ChanEn);
    vos_printf("通道模式:\t\t%d\n",             g_strSocpStat.sDecSrcChan[u32RealId].eChnMode);
    vos_printf("通道buffer 起始地址:\t\t0x%x\n",g_strSocpStat.sDecSrcChan[u32RealId].sDecSrcBuf.u32Start);
    vos_printf("通道buffer 结束地址:\t\t0x%x\n",g_strSocpStat.sDecSrcChan[u32RealId].sDecSrcBuf.u32End);
    vos_printf("通道buffer 读指针:\t\t0x%x\n",  g_strSocpStat.sDecSrcChan[u32RealId].sDecSrcBuf.u32Read);
    vos_printf("通道buffer 写指针:\t\t0x%x\n",  g_strSocpStat.sDecSrcChan[u32RealId].sDecSrcBuf.u32Write);
    vos_printf("通道buffer 长度:\t\t0x%x\n",    g_strSocpStat.sDecSrcChan[u32RealId].sDecSrcBuf.u32Length);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowDecSrcChanAdd
*
* 功能描述  : 打印解码源通道累计统计值
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_UINT32 SOCP_ShowDecSrcChanAdd(VOS_UINT32 u32UniqueId)
{
    VOS_UINT32             u32RealChanID;
    SOCP_DEBUG_DECSRC_STRU *sSocpAddDebugDecSrc;
    VOS_UINT32             u32ChanType = 0;

    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_DECODER_SRC_CHAN);

    u32RealChanID = SOCP_REAL_CHAN_ID(u32UniqueId);

    sSocpAddDebugDecSrc = &g_stSocpDebugInfo.sSocpDebugDecSrc;

    vos_printf("================== 解码源通道 0x%x  累计统计值:=================\n",
            u32UniqueId);
    vos_printf("\r socp启动解码源通道成功的次数                           : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpStartDecSrcCnt[u32RealChanID]);
    vos_printf("\r socp停止解码源通道成功的次数                           : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpStopDecSrcCnt[u32RealChanID]);
    vos_printf("\r socp注册解码源通道异常处理函数的次数              : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpRegEventDecSrcCnt[u32RealChanID]);
    vos_printf("\r socp解码源通道尝试获得写buffer的次数                  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpGetWBufDecSrcEtrCnt[u32RealChanID]);
    vos_printf("\r socp解码源通道获得写buffer成功的次数                  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpGetWBufDecSrcSucCnt[u32RealChanID]);
    vos_printf("\r socp解码源通道尝试更新写buffer指针的次数            : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpWriteDoneDecSrcEtrCnt[u32RealChanID]);
    vos_printf("\r socp解码源通道更新写buffer指针成功的次数            : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpWriteDoneDecSrcSucCnt[u32RealChanID]);
    vos_printf("\r socp解码源通道更新写buffer指针失败的次数            : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpWriteDoneDecSrcFailCnt[u32RealChanID]);
    vos_printf("\r socpISR 中进入解码源通道错误中断次数                      : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecSrc->u32SocpDecSrcIsrErrIntCnt[u32RealChanID]);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowDecSrcChanAll
*
* 功能描述  : 打印解码源通道信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_ShowDecSrcChanAll(VOS_VOID)
{
    VOS_UINT32 i;
    VOS_UINT32 u32UniqueId = 0;

    for (i = 0; i < SOCP_MAX_DECSRC_CHN; i++)
    {
        u32UniqueId = SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, i);
        (VOS_VOID)SOCP_ShowDecSrcChanCur(u32UniqueId);
        (VOS_VOID)SOCP_ShowDecSrcChanAdd(u32UniqueId);
    }

    return;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowDecDstChanCur
*
* 功能描述  : 打印解码目的通道信息
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_UINT32 SOCP_ShowDecDstChanCur(VOS_UINT32 u32UniqueId)
{
    VOS_UINT32 u32RealId   = 0;
    VOS_UINT32 u32ChanType = 0;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_DECODER_DEST_CHAN);

    vos_printf("================== 解码目的通道 0x%x  属性:=================\n", u32UniqueId);
    vos_printf("通道ID:\t\t%d\n",               g_strSocpStat.sDecDstChan[u32RealId].u32ChanID);
    vos_printf("通道使用模类型:\t\t%d\n",       g_strSocpStat.sDecDstChan[u32RealId].eDataType);
    vos_printf("通道buffer 起始地址:\t\t0x%x\n",g_strSocpStat.sDecDstChan[u32RealId].sDecDstBuf.u32Start);
    vos_printf("通道buffer 结束地址:\t\t0x%x\n",g_strSocpStat.sDecDstChan[u32RealId].sDecDstBuf.u32End);
    vos_printf("通道buffer 读指针:\t\t0x%x\n",  g_strSocpStat.sDecDstChan[u32RealId].sDecDstBuf.u32Read);
    vos_printf("通道buffer 写指针:\t\t0x%x\n",  g_strSocpStat.sDecDstChan[u32RealId].sDecDstBuf.u32Write);
    vos_printf("通道buffer 长度:\t\t0x%x\n",    g_strSocpStat.sDecDstChan[u32RealId].sDecDstBuf.u32Length);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowDecDstChanAdd
*
* 功能描述  : 打印解码目的通道累计统计值
*
* 输入参数  : 通道ID
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_UINT32 SOCP_ShowDecDstChanAdd(VOS_UINT32 u32UniqueId)
{
    VOS_UINT32             u32RealChanID;
    SOCP_DEBUG_DECDST_STRU *sSocpAddDebugDecDst;
    VOS_UINT32             u32ChanType = 0;

    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    SOCP_CHECK_CHAN_TYPE(u32ChanType, SOCP_DECODER_DEST_CHAN);

    u32RealChanID = SOCP_REAL_CHAN_ID(u32UniqueId);

    sSocpAddDebugDecDst = &g_stSocpDebugInfo.sSocpDebugDecDst;

    vos_printf("================== 解码目的通道 0x%x  累计统计值:=================\n",
            u32UniqueId);
    vos_printf("\r socp注册解码目的通道异常处理函数的次数           : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpRegEventDecDstCnt[u32RealChanID]);
    vos_printf("\r socp解码目的通道注册读数据回调函数成功的次数   : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpRegReadCBDecDstCnt[u32RealChanID]);
    vos_printf("\r socp解码目的通道尝试获得读buffer的次数                : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpGetReadBufDecDstEtrCnt[u32RealChanID]);
    vos_printf("\r socp解码目的通道获得读buffer成功的次数                : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpGetReadBufDecDstSucCnt[u32RealChanID]);
    vos_printf("\r socp解码目的通道尝试更新读buffer指针的次数          : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpReadDoneDecDstEtrCnt[u32RealChanID]);
    vos_printf("\r socp解码目的通道更新读buffer指针成功的次数          : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpReadDoneDecDstSucCnt[u32RealChanID]);
    vos_printf("\r socp解码目的通道更新读buffer指针失败的次数          : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpReadDoneDecDstFailCnt[u32RealChanID]);
    vos_printf("\r socp解码目的通道更新读buffer指针移动0 字节成功的次数   : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpReadDoneZeroDecDstCnt[u32RealChanID]);
    vos_printf("\r socp解码目的通道更新读buffer指针移动非0 字节成功的次数: 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpReadDoneValidDecDstCnt[u32RealChanID]);
    vos_printf("\r socpISR 中进入解码目的通道传输完成中断次数                : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpDecDstIsrTrfIntCnt[u32RealChanID]);
    vos_printf("\r socpISR 中进入解码目的通道buf 溢出中断次数                  : 0x%x\n",
           (VOS_INT32)sSocpAddDebugDecDst->u32SocpDecDstIsrOvfIntCnt[u32RealChanID]);

    return VOS_OK;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowDecDstChanAll
*
* 功能描述  : 打印解码目的通道信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_ShowDecDstChanAll(VOS_VOID)
{
    VOS_UINT32 i;
    VOS_UINT32 u32UniqueId = 0;

    for (i = 0; i < SOCP_MAX_DECDST_CHN; i++)
    {
        u32UniqueId = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN, i);
        (VOS_VOID)SOCP_ShowDecDstChanCur(u32UniqueId);
        (VOS_VOID)SOCP_ShowDecDstChanAdd(u32UniqueId);
    }

    return;
}

/*****************************************************************************
* 函 数 名   : SOCP_ShowDecDstChanAll
*
* 功能描述  : 打印解码目的通道信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_ShowSDLogInfo(VOS_VOID)
{
    SOCP_ENC_DST_BUF_LOG_CFG_STRU      *pstcfg;
    VOS_UINT8                           ucLoop;
    VOS_UINT32                          ulChannelId;

    pstcfg = SocpGetSDLogCfg();

    vos_printf("\r\n The SD Log Flag is         %d",    pstcfg->logOnFlag);
    vos_printf("\r\n The Over Time is           %d ms", pstcfg->overTime);

    for (ucLoop = 0; ucLoop < SOCP_MAX_ENCDST_CHN; ucLoop++)
    {
        if (0 == ucLoop)
        {
            ulChannelId = SOCP_CODER_DST_OM_CNF;
        }
        else
        {
            ulChannelId = SOCP_CODER_DST_OM_IND;
        }

        vos_printf("\r\n EncDstCh0 Start Time num is   %d", g_stSocpDebugInfo.stSocpSDLogInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulStartTimeNum);
        vos_printf("\r\n EncDstCh0 Time Out Run Num is %d", g_stSocpDebugInfo.stSocpSDLogInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulEncDstRunNum);
        vos_printf("\r\n EncDstCh0 Flush num is        %d", g_stSocpDebugInfo.stSocpSDLogInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulFlushBufferNum);
        vos_printf("\r\n EncDstCh0 Flush Time Out is   %d", g_stSocpDebugInfo.stSocpSDLogInfo[SOCP_REAL_CHAN_ID(ulChannelId)].ulFlushTimeOutNum);
        vos_printf("\r\n");
    }

    return;
}

#endif

/*****************************************************************************
* 函 数 名   : SOCP_debugCntShow
*
* 功能描述  : 显示debug 计数信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值   : 无
*****************************************************************************/
VOS_VOID SOCP_debugCntShow(VOS_VOID)
{
    SOCP_ShowDebugGbl();
    SOCP_ShowEncSrcChanAll();
#ifdef SOCP_ACORE
    SOCP_ShowEncDstChanAll();
    SOCP_ShowDecSrcChanAll();
    SOCP_ShowDecDstChanAll();
#endif

    return;
}

/*****************************************************************************
 函 数 名  : SOCP_Stop1SrcChan
 功能描述  : disable 一个SOCP编码源通道
 输入参数  : ulSrcChanID 通道ID
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年5月2日
     作    者  : j0174725s
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID SOCP_Stop1SrcChan(VOS_UINT32 ulSrcChanID)
{
    VOS_UINT32                          ulRealChanID;

    /* 获取真实通道ID */

    ulRealChanID = SOCP_REAL_CHAN_ID(ulSrcChanID);

    /* 设置通道关闭 */
    SOCP_REG_SETBITS(SOCP_REG_ENCSRC_BUFCFG1(ulRealChanID), 0, 1, 0);

    return ;
}


VOS_VOID SOCP_LogToFile(FILE *fp)
{
    mdrv_file_write((VOS_VOID*)&g_strSocpStat, sizeof(VOS_CHAR), sizeof(g_strSocpStat), fp);
    mdrv_file_write((VOS_VOID*)&g_stSocpDebugInfo, sizeof(VOS_CHAR), sizeof(g_stSocpDebugInfo), fp);
}


VOS_UINT8* SOCP_GetDebugLogInfo(VOS_UINT32  ulType)
{
    /* 1表示 socp statue */
    if (1 == ulType)
    {
        return (VOS_UINT8*)&g_strSocpStat;
    }
    /* 否则表示Debug Info */
    return (VOS_UINT8*)&g_stSocpDebugInfo;
}


VOS_UINT32 SOCP_GetDebugLogInfoLen(VOS_UINT32  ulType)
{
    /* 1表示 socp statue */
    if (1 == ulType)
    {
        return (VOS_UINT32)sizeof(g_strSocpStat);
    }

    /* 否则表示Debug Info */
    return (VOS_UINT32)sizeof(g_stSocpDebugInfo);
}
/*lint +e662 +e661*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
