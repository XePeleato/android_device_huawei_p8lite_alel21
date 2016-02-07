

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "omprivate.h"
#include "omsdlog.h"
#include "cpm.h"
#include "OmApp.h"
#include "NVIM_Interface.h"
#include "fastlz.h" /* 开源压缩算法 */

#if (VOS_OS_VER == VOS_LINUX)
#include <linux/scatterlist.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_OM_SD_LOG_C
/*lint +e767 */

/*****************************************************************************
2 全局变量定义
*****************************************************************************/
/* 用于缓存在SD卡中写入的trace */
OM_SD_BUFFER_INNER_DATA_STRU       *g_pstSdBuffer;

/* 写SD卡的标志，在PC侧导出SD卡trace时该标志置为不可写 */
VOS_UINT32                          g_ulWriteSDLogFlag = VOS_OK;

/* 记录读取SD卡trace的错误 */
VOS_UINT32                          g_ulReadSDErrType  = OM_SD_OK;

/* 记录写SD卡trace的错误 */
VOS_UINT32                          g_ulWriteSDErrType = OM_SD_OK;

/* 记录读SD卡cluster的数目及从那个cluster开始读 */
OM_SD_READ_INFO_STRU                g_stReadInfo;

/* 保存SD卡的头部控制信息 */
OM_SD_HEAD_STRU                     *g_pstHeadInfo;

VOS_UINT32                          g_ulInterval = 10;

VOS_UINT8*                          g_pucBuffer;

/* 保存SD卡的Debug 信息 */
OM_SD_DEBUG_INFO                    g_stOmSDDebugInfo;

#if (VOS_LINUX == VOS_OS_VER)

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
VOS_CHAR                            g_acFSLogType[] = "ucmx";
#else
VOS_CHAR                            g_acFSLogType[] = "ucm";
#endif

#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
VOS_CHAR                            g_acFSLogDir[OM_SD_LOG_PATH_MAX_LENGTH]     = "/mnvm3:0/Log";
VOS_CHAR                            g_acFSLogCfgPath[OM_SD_LOG_PATH_MAX_LENGTH] = "/mnvm3:0/Log/Config";
OM_FLASH_DEBUG_INFO_STRU            g_stFSLogFileInfo =
                                        {VOS_FALSE, 0, 0, 0, OM_FLASH_DEFAULT_FILE_SIZE, 0, 0, 0, "/mnvm3:0/Log/OmLog"};
#else
VOS_CHAR                            g_acFSLogDir[OM_SD_LOG_PATH_MAX_LENGTH]     = "/online/modemlog/Log";
VOS_CHAR                            g_acFSLogCfgPath[OM_SD_LOG_PATH_MAX_LENGTH] = "/online/modemlog/Log/Config";
OM_FLASH_DEBUG_INFO_STRU            g_stFSLogFileInfo =
                                        {VOS_FALSE, 0, 0, 0, OM_FLASH_DEFAULT_FILE_SIZE, 0, 0, 0,"/online/modemlog/Log/OmLog"};
#endif
#elif (VOS_WIN32 == VOS_OS_VER)
VOS_CHAR                            g_acFSLogDir[] = "";
VOS_CHAR                            g_acFSLogCfgPath[] = "";
OM_FLASH_DEBUG_INFO_STRU            g_stFSLogFileInfo;

#endif

VOS_UINT32                          g_ulLogMaxCout = LOG_WRITE_MAX_COUNT;

OM_FLASH_CFG_INFO_STRU              g_stFSCfgFileInfo;

NV_FLASH_LOG_RECORD_STRU            g_stFlashLogCfg;

VOS_UINT32                          g_ulFSlogStatus;

VOS_UINT32                          g_ulFSLogErrFlag = LOG_OPERATION_OK;

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
/* 存放压缩后数据，包括分隔符以及压缩完数据长度 */
OM_UCMX_DATA_INFO_STRU              g_stCompressData;

/* 存放组包数据 */
VOS_UINT8                           g_aucPackData[OM_MAX_PACK_DATA_LEN];
VOS_UINT32                          g_ulPackLen = 0;

/* 可谓可测 */
OM_COMPRESS_DEBUG_OG_STRU           g_stCompressDebugLog;
#endif
/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 OM_SDInit(VOS_VOID)
{
    VOS_MemSet(&g_stOmSDDebugInfo, 0, sizeof(OM_CHANNLE_PORT_CFG_STRU));

    CPM_PhySendReg(CPM_SD_PORT, (CPM_SEND_FUNC)OM_SDWriteCard);

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)VOS_CacheMemAlloc(OM_SD_BLOCK_SIZE);

    if(VOS_NULL_PTR == g_pstHeadInfo)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SDInit:Alloc Head Info Memory fail!");

        return VOS_ERR;
    }

    g_pstSdBuffer = (OM_SD_BUFFER_INNER_DATA_STRU *)VOS_CacheMemAlloc(sizeof(OM_SD_BUFFER_INNER_DATA_STRU));

    if(VOS_NULL_PTR == g_pstSdBuffer)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SDInit:Alloc SD Buffer Memory fail!");

        return VOS_ERR;
    }

    /* 初始化缓冲区初始值 */
    g_pstSdBuffer->ulDataLen = 0;

    return VOS_OK;
}
VOS_UINT32 OM_SDReadHeadInfo(VOS_VOID)
{
    VOS_INT          lRet;
    VOS_UINT         ulSize;

    /* 调用底软接口读取SD卡的头部控制信息，头部控制信息存储在第0个block */
    mdrv_sd_sg_init_table((void *)g_pstHeadInfo, OM_SD_BLOCK_SIZE);

    lRet = mdrv_sd_multi_transfer(0, 1, OM_SD_BLOCK_SIZE, RDFlAG);

    if(VOS_OK != lRet)
    {
        vos_printf("OM_SDReadHeadInfo:err DRV_SD_TRANSFER\n");

        g_ulWriteSDErrType = OM_ERR_DRVAPI;

        return VOS_ERR;
    }

    /* 如果SD卡为新卡,初始化头部控制信息 */
    if((OM_SD_MAGIC_NUMBER != g_pstHeadInfo->ulMagicNum)
        || (OM_SD_OPPOSITE_MAGIC_NUMBER != g_pstHeadInfo->ulOppositeMagicNum))
    {
        /* 获取SD卡容量 */
        ulSize = (VOS_UINT32)mdrv_sd_get_capacity();

        if(0 == ulSize)
        {
            vos_printf("OM_SDReadHeadInfo:ulSize is 0\n");

            g_ulWriteSDErrType = OM_ERR_VALIDPARA;

            return VOS_ERR;
        }

        /* 计算SD卡总的cluster数目，预留10%的余量 */
        g_pstHeadInfo->ulTotalCNum = (ulSize - (ulSize/10)) / OM_SD_BLOCK_NUM_ONE_CLUSTER;

        g_pstHeadInfo->ulMagicNum = OM_SD_MAGIC_NUMBER;

        g_pstHeadInfo->ulOppositeMagicNum = OM_SD_OPPOSITE_MAGIC_NUMBER;

        g_pstHeadInfo->ulLoopOutFlag = 0;

        g_pstHeadInfo->ulNextCid = 1;

        /* 初始化SD卡头部控制信息 */
        mdrv_sd_sg_init_table((void *)g_pstHeadInfo, OM_SD_BLOCK_SIZE);
        lRet = mdrv_sd_multi_transfer(0, 1, OM_SD_BLOCK_SIZE, WRFlAG);

        if(VOS_OK != lRet)
        {
            vos_printf("OM_SDFlushHeadInfo:err DRV_SD_TRANSFER\n");

            g_ulWriteSDErrType = OM_ERR_DRVAPI;

            return VOS_ERR;
        }

    }

    return VOS_OK;

}
VOS_UINT32 OM_SDFlushHeadInfo(VOS_VOID)
{
    VOS_INT lRet;

    g_pstHeadInfo->ulNextCid++;

    /* nextCID的范围始终在 1－(g_stHeadInfo.ulTotalCNum -1)之间，第0个cluster用于保存头部控制信息*/
    if(0 == (g_pstHeadInfo->ulNextCid % g_pstHeadInfo->ulTotalCNum))
    {
        g_pstHeadInfo->ulNextCid = 1;

        g_pstHeadInfo->ulLoopOutFlag = 1;
    }

    /* 更新头部控制信息 */

    mdrv_sd_sg_init_table((void *)g_pstHeadInfo, OM_SD_BLOCK_SIZE);

    lRet = mdrv_sd_multi_transfer(0, 1, OM_SD_BLOCK_SIZE, WRFlAG);

    if(VOS_OK != lRet)
    {
        vos_printf("OM_SDFlushHeadInfo:err DRV_SD_TRANSFER\n");

        g_ulWriteSDErrType = OM_ERR_DRVAPI;

        return VOS_ERR;
    }

    return VOS_OK;

}


VOS_UINT32 OM_SDWriteCard(VOS_UINT8* pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT16 usLen)
{
    VOS_UINT32 ulCurBufDataLen;
    VOS_INT    lRet;
    VOS_UINT32 ulSecAddr;

    ulCurBufDataLen = g_pstSdBuffer->ulDataLen;

    /* 如果目标buffer剩余空间不够存储本次TRACE */
    if(OM_SD_BUFFER_MAX_DATA_LEN < (ulCurBufDataLen + usLen))
    {
        if(VOS_ERR == OM_SDReadHeadInfo())
        {
            vos_printf("OM_SDWriteBuffer:SD card NOT IN\n");

            g_stOmSDDebugInfo.ulReadHeadErrLenth += usLen;
            return VOS_ERR;
        }

        /* 计算写SD卡的起始block */
        ulSecAddr                        = g_pstHeadInfo->ulNextCid * OM_SD_BLOCK_NUM_ONE_CLUSTER;

        g_pstSdBuffer->ulClusterId         = g_pstHeadInfo->ulNextCid;

        g_pstSdBuffer->ulOppositeClusterId = ~g_pstHeadInfo->ulNextCid;

        /* 写SD卡失败时记录错误，立即退出 */
        mdrv_sd_sg_init_table((void *)g_pstSdBuffer, OM_SD_BLOCK_NUM_ONE_CLUSTER*OM_SD_BLOCK_SIZE);
        lRet = mdrv_sd_multi_transfer(ulSecAddr, OM_SD_BLOCK_NUM_ONE_CLUSTER, OM_SD_BLOCK_SIZE, WRFlAG);

        g_stOmSDDebugInfo.ulTatalBlock += OM_SD_BLOCK_NUM_ONE_CLUSTER;

        g_stOmSDDebugInfo.ulDataLenth  += g_pstSdBuffer->ulDataLen;

        if(g_stOmSDDebugInfo.ulDataLenth > OM_SD_MAX_DATA_LENTH)
        {
            g_stOmSDDebugInfo.ulOverNum++;
            g_stOmSDDebugInfo.ulDataLenth -= OM_SD_MAX_DATA_LENTH;
        }

        if(VOS_OK != lRet)
        {
            vos_printf("OM_SDWriteBuffer:err DRV_SD_TRANSFER\n");

            g_ulWriteSDErrType = OM_ERR_DRVAPI;

            return VOS_ERR;
        }

        /* 将数据写到SD卡后将buffer的写满标志及长度字段清空 */
        g_pstSdBuffer->ulDataLen = 0;

        OM_SDFlushHeadInfo();

        ulCurBufDataLen = 0;
    }

    /* copy数据 */
    VOS_MemCpy((g_pstSdBuffer->aucBuffer + ulCurBufDataLen), pucVirAddr, usLen);

    /* 更新长度 */
    g_pstSdBuffer->ulDataLen += (VOS_UINT32)usLen;

    g_stOmSDDebugInfo.ulRemainLenth = g_pstSdBuffer->ulDataLen;

    return VOS_OK;

}


VOS_VOID OM_SDReadCluster(VOS_UINT32 ulPacketSn)
{
    VOS_UINT32                  ulCurReadCID;
    VOS_INT                     lRet;
    OM_APP_MSG_EX_STRU          *pstOmToAppMsg;
    OM_APP_READ_SD_STRU         *pstOmReadSD;
    VOS_UINT32                  ulSecAddr;
    VOS_UINT32                  ulMinValue = g_ulInterval;
    VOS_UINT32                  i;

    /* PC侧请求的数据包序号超过需要读的序号时 */
    if(ulPacketSn > (g_stReadInfo.ulTotalNeedCNum - 1))
    {
        g_ulReadSDErrType = OM_ERR_VALIDPARA;

        OM_AcpuSendResult(OM_READSD_FUNC, OM_ERR_VALIDPARA, APP_OM_READ_SD_CNF);

        vos_printf("OM_SDReadCluster:err ulPacketSn\n");

        return;
    }

    /* 计算本次发送给PC侧的IND个数 */
    if(ulMinValue > (g_stReadInfo.ulTotalNeedCNum - ulPacketSn))
    {
       ulMinValue = g_stReadInfo.ulTotalNeedCNum - ulPacketSn;
    }

    for(i = 0; i < ulMinValue; i++)
    {
        ulCurReadCID = g_stReadInfo.ulStartCID + ulPacketSn;

        /* 跳过第0个cluster的头部控制信息读取 */
        if(ulCurReadCID > (g_pstHeadInfo->ulTotalCNum - 1))
        {
            ulSecAddr = ((ulCurReadCID % g_pstHeadInfo->ulTotalCNum) + 1) * OM_SD_BLOCK_NUM_ONE_CLUSTER;
        }
        else
        {
            ulSecAddr = ulCurReadCID * OM_SD_BLOCK_NUM_ONE_CLUSTER;
        }

        /* 读SD卡中的数据 */
        mdrv_sd_sg_init_table((void *)g_pstSdBuffer, OM_SD_BLOCK_NUM_ONE_CLUSTER*OM_SD_BLOCK_SIZE);

        lRet =  mdrv_sd_multi_transfer(ulSecAddr, OM_SD_BLOCK_NUM_ONE_CLUSTER, OM_SD_BLOCK_SIZE, RDFlAG);

        if(VOS_OK != lRet)
        {
            g_ulReadSDErrType = OM_ERR_DRVAPI;

            OM_AcpuSendResult(OM_READSD_FUNC, OM_ERR_DRVAPI, APP_OM_READ_SD_CNF);

            vos_printf("OM_SDReadCluster:err DRV_SD_TRANSFER\n");

            return;
        }

        pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)g_pstSdBuffer->aucReserved;

        pstOmReadSD = (OM_APP_READ_SD_STRU*)(pstOmToAppMsg->aucPara);

        pstOmReadSD->ulClusterId = g_pstSdBuffer->ulClusterId;

        pstOmReadSD->ulSn = ulPacketSn;

        /* 如果cluster的CID字段和~CID字段不是取反的关系的话，则认为当前cluster数据损坏 */
        if(g_pstSdBuffer->ulClusterId != (~g_pstSdBuffer->ulOppositeClusterId))
        {
            g_ulReadSDErrType = OM_ERR_DATAERR;

            pstOmReadSD->ulErrCode = VOS_ERR;

            pstOmToAppMsg->usLength = OM_APP_MSG_EX_LEN - VOS_OM_HEADER_LEN + OM_READ_SD_HEAD_LEN;

            vos_printf("OM_SDReadCluster:data err\n");
        }
        else
        {
            pstOmReadSD->ulErrCode = VOS_OK;

            pstOmToAppMsg->usLength = (VOS_UINT16)(OM_APP_MSG_EX_LEN - VOS_OM_HEADER_LEN + OM_READ_SD_HEAD_LEN
                                      + g_pstSdBuffer->ulDataLen);
        }

        OM_AcpuSendContent(OM_READSD_FUNC, (OM_RSP_PACKET_STRU *)(g_pstSdBuffer->aucReserved), APP_OM_READ_SD_IND);

        ulPacketSn++;
    }

    return;
}
VOS_UINT32 OM_SDGetReadPosition(VOS_UINT32 ulNeedSize, VOS_UINT32 ulOffsetSize)
{
    VOS_UINT32                      ulTotalClusterNum;
    VOS_UINT32                      ulOffsetClusterNum;

    ulOffsetClusterNum = (ulOffsetSize + OM_SD_CLUSTER_DATA_LEN - 1)/OM_SD_CLUSTER_DATA_LEN;

    g_stReadInfo.ulTotalNeedCNum = (ulNeedSize + OM_SD_CLUSTER_DATA_LEN - 1)/OM_SD_CLUSTER_DATA_LEN;

    ulTotalClusterNum = g_stReadInfo.ulTotalNeedCNum + ulOffsetClusterNum;

    if (0 == g_pstHeadInfo->ulLoopOutFlag)
    {
        /* 用户偏移的数据大于SD卡中已有数据 */
        if (ulOffsetClusterNum >= (g_pstHeadInfo->ulNextCid - 1))
        {
            g_stReadInfo.ulTotalNeedCNum = 0;

            g_ulReadSDErrType = OM_ERR_EMPTY;

            return OM_ERR_EMPTY;
        }

        /* 用户需要的数据大于SD卡中已有数据 */
        if(ulTotalClusterNum > (g_pstHeadInfo->ulNextCid - 1))
        {
            g_stReadInfo.ulStartCID = 1;

            g_stReadInfo.ulTotalNeedCNum = (g_pstHeadInfo->ulNextCid - 1) - ulOffsetClusterNum;

            g_ulReadSDErrType = OM_ERR_NOENOUGHDATA;

            return OM_SD_OK;
        }

        /* 用户需要的数据小于SD卡中已有数据 */
        g_stReadInfo.ulStartCID = g_pstHeadInfo->ulNextCid - ulTotalClusterNum;

        return OM_SD_OK;

    }

    /* 保存SD卡数据有翻转，并且用户偏移的数据大于SD卡中已有数据*/
    if (ulOffsetClusterNum >= (g_pstHeadInfo->ulTotalCNum - 1))
    {
        g_stReadInfo.ulTotalNeedCNum = 0;

        g_ulReadSDErrType = OM_ERR_EMPTY;

        return OM_ERR_EMPTY;
    }

    /* 第0个cluster用于保存头部控制信息 */
    if(ulTotalClusterNum > (g_pstHeadInfo->ulTotalCNum - 1))
    {
        g_stReadInfo.ulTotalNeedCNum = (g_pstHeadInfo->ulTotalCNum - 1) - ulOffsetClusterNum;

        /* SD卡发生翻转时，存储最早的数据为下一个待写入的cluster数据 */
        g_stReadInfo.ulStartCID = g_pstHeadInfo->ulNextCid;

        return OM_SD_OK;
    }

    /* 实际需要的数据小于SD卡中的数据 */
    if(g_pstHeadInfo->ulNextCid > ulTotalClusterNum)
    {
        g_stReadInfo.ulStartCID = g_pstHeadInfo->ulNextCid - ulTotalClusterNum;
    }
    else
    {
        g_stReadInfo.ulStartCID = g_pstHeadInfo->ulTotalCNum - (ulTotalClusterNum - (g_pstHeadInfo->ulNextCid - 1));
    }

    return OM_SD_OK;
}
VOS_UINT32 OM_SDParaCheck(VOS_UINT32 ulDataSize, VOS_UINT32 *pulErrNo)
{
    VOS_INT lRet;

    if(0 == ulDataSize)
    {
        *pulErrNo = OM_ERR_VALIDPARA;

        return VOS_ERR;
    }

    /* 调用底软接口读取SD卡的头部控制信息，头部控制信息存储在第0个block */
    mdrv_sd_sg_init_table((void *)g_pstHeadInfo, OM_SD_BLOCK_SIZE);

    lRet = mdrv_sd_multi_transfer(0, 1, OM_SD_BLOCK_SIZE, RDFlAG);

    if(VOS_OK != lRet)
    {
        vos_printf("OM_SDInit:err DRV_SD_TRANSFER\n");

        g_ulWriteSDErrType = OM_ERR_DRVAPI;

        *pulErrNo = OM_ERR_DRVAPI;

        return VOS_ERR;
    }

    /* 如果SD卡为新卡,初始化头部控制信息 */
    if((OM_SD_MAGIC_NUMBER != g_pstHeadInfo->ulMagicNum)
        || (OM_SD_OPPOSITE_MAGIC_NUMBER != g_pstHeadInfo->ulOppositeMagicNum))
    {
       *pulErrNo = OM_ERR_NEWSDCARD;

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID OM_SDReadReqProc(APP_OM_READ_SD_REQ_STRU * pstReadSDReq)
{
    VOS_UINT32                      ulErrNo = OM_SD_BUTT;
    VOS_UINT32                      ulRet;
    OM_APP_MSG_EX_STRU              *pstOmToAppMsg;
    OM_APP_READ_SD_CNF_STRU         *pstOmToAppCnf;

    ulRet = OM_SDParaCheck(pstReadSDReq->ulTotalSize, &ulErrNo);

    if(VOS_OK != ulRet)
    {
        OM_AcpuSendResult(OM_READSD_FUNC, ulErrNo, APP_OM_READ_SD_CNF);

        vos_printf("OM_SDReadReqProc:reply APP_OM_READ_SD_CNF\n");

        g_ulReadSDErrType = ulErrNo;

        return;
    }

    /* 获取实际需要读得cluster数目及起始读的位置 */
    ulRet = OM_SDGetReadPosition(pstReadSDReq->ulTotalSize, pstReadSDReq->ulOffsetSize);

    /*计算发送给工具侧消息包的总长度*/
    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, OM_APP_MSG_EX_LEN + sizeof(OM_APP_READ_SD_CNF_STRU));

    if(VOS_NULL_PTR == pstOmToAppMsg)
    {
        g_ulReadSDErrType = OM_ERR_MALLOCSPACE;

        OM_AcpuSendResult(OM_READSD_FUNC, OM_ERR_MALLOCSPACE, APP_OM_READ_SD_CNF);

        return;
    }

    /* 填充消息字段并将消息发送给PC侧 */
    pstOmToAppMsg->usLength = (OM_APP_MSG_EX_LEN + sizeof(OM_APP_READ_SD_CNF_STRU) - VOS_OM_HEADER_LEN);

    pstOmToAppCnf = (OM_APP_READ_SD_CNF_STRU*)(pstOmToAppMsg->aucPara);

    pstOmToAppCnf->ulResult     = ulRet;

    pstOmToAppCnf->ulReportCNum = g_stReadInfo.ulTotalNeedCNum;

    if(0 == g_pstHeadInfo->ulLoopOutFlag)
    {
        pstOmToAppCnf->ulHaveCNum = g_pstHeadInfo->ulNextCid - 1;
    }
    else
    {
        pstOmToAppCnf->ulHaveCNum = g_pstHeadInfo->ulTotalCNum - 1;
    }

    OM_AcpuSendContent(OM_READSD_FUNC, (OM_RSP_PACKET_STRU*)pstOmToAppMsg, APP_OM_READ_SD_CNF);

    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);

    return;

}
VOS_VOID OM_ReadSdMsgProc(OM_REQ_PACKET_STRU *pstRspPacket, OM_RSP_FUNC *pstRspFuncPtr)
{
    APP_OM_MSG_EX_STRU              *pstAppToOmMsg;

   /* 检测SD卡是否在位 */
    if(VOS_OK != mdrv_sd_get_capacity())
    {
        g_ulReadSDErrType = OM_ERR_NOCARD;

        vos_printf("\r\n OM_ReadSdMsgProc: mdrv_sd_get_capacity return ERROR");

        OM_AcpuSendResult(OM_READSD_FUNC, OM_ERR_NOCARD, APP_OM_READ_SD_CNF);

        return;
    }

    pstAppToOmMsg   = (APP_OM_MSG_EX_STRU*)pstRspPacket;

    /* 处理PC侧导出SD卡数据的消息 */
    if(APP_OM_READ_SD_DATA_REQ == pstAppToOmMsg->usPrimId)
    {
        OM_SDReadReqProc((APP_OM_READ_SD_REQ_STRU *)(pstAppToOmMsg->aucPara));
    }
    else if(APP_OM_READ_SD_CLUSTER_REQ == pstAppToOmMsg->usPrimId)
    {
        OM_SDReadCluster(*((VOS_UINT32 *)pstAppToOmMsg->aucPara));
    }
    else
    {
        /* do nothing */
    }

    return;
}

VOS_VOID OM_PrintfSDError(VOS_VOID)
{
    vos_printf("\r\nThe SD Error Type is:       %d", g_ulReadSDErrType);

    vos_printf("\r\nWrite To DRV Total Block:   %d", g_stOmSDDebugInfo.ulTatalBlock);

    vos_printf("\r\nWrite To SD Total Over Num: %d", g_stOmSDDebugInfo.ulOverNum);

    vos_printf("\r\nWrite To SD  Length :       %d", g_stOmSDDebugInfo.ulDataLenth);

    vos_printf("\r\nRemain Length:              %d", g_stOmSDDebugInfo.ulRemainLenth);

    vos_printf("\r\nRead Head Err Lost Length:  %d", g_stOmSDDebugInfo.ulReadHeadErrLenth);
}

#if (VOS_LINUX == VOS_OS_VER)
VOS_UINT32 OM_FSFlushCfgFile(VOS_VOID)
{
    VOS_INT                             lCfgFile;
    VOS_INT                             lReturnLen;
    VOS_UINT32                          sd_fs = 0;

    sd_fs = SD_FS_GETFS();
    SD_FS_SETFS(KERNEL_DS);

    lCfgFile = SD_FS_OPEN(g_acFSLogCfgPath, O_CREAT|O_TRUNC|O_RDWR, OM_SD_FILE_MODE);

    if (lCfgFile < 0)
    {
        SD_FS_SETFS(sd_fs);
        return VOS_ERR;
    }

    lReturnLen = SD_FS_WRITE(lCfgFile, (VOS_CHAR *)&g_stFSCfgFileInfo, sizeof(OM_FLASH_CFG_INFO_STRU));

    SD_FS_CLOSE(lCfgFile);

    SD_FS_SYNC();

    SD_FS_SETFS(sd_fs);

    if (sizeof(OM_FLASH_CFG_INFO_STRU) != lReturnLen)
    {
        g_stFSLogFileInfo.ulErrLog  = LOG_OPERATION_WRITE_FILE_FAIL;

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 OM_FSInitCfgFile(VOS_VOID)
{
    VOS_INT                             lCfgFile;
    VOS_INT                             lReturnLen;
    VOS_UINT32                          sd_fs = 0;

    sd_fs = SD_FS_GETFS();
    SD_FS_SETFS(KERNEL_DS);

    /* 打开Config文件 */
    lCfgFile = SD_FS_OPEN(g_acFSLogCfgPath, O_RDONLY, OM_SD_FILE_MODE);

    /* 文件不存在 */
    if (lCfgFile < 0)
    {
        /* 判断LOG目录是否已经存在 */
        if (VOS_OK != SD_FS_ACCESS(g_acFSLogDir, 0))
        {
            /* 如果目录不存在则创建LOG目录 */
            if(SD_FS_MKDIR(g_acFSLogDir, 0) < 0)
            {
                SD_FS_SETFS(sd_fs);
                return VOS_ERR;
            }
        }

        /* 如果配置文件不存在，则创建新文件并初始化内容 */
        lCfgFile = SD_FS_OPEN(g_acFSLogCfgPath, O_CREAT|O_TRUNC|O_RDWR, OM_SD_FILE_MODE);

        if (lCfgFile < 0)
        {
            SD_FS_SETFS(sd_fs);
            return VOS_ERR;
        }

        lReturnLen = SD_FS_WRITE(lCfgFile, (VOS_CHAR *)&g_stFSCfgFileInfo, sizeof(OM_FLASH_CFG_INFO_STRU));
    }
    else
    {
        lReturnLen = SD_FS_READ(lCfgFile, (VOS_CHAR *)&g_stFSCfgFileInfo, sizeof(OM_FLASH_CFG_INFO_STRU));
    }

    SD_FS_CLOSE(lCfgFile);

    if (sizeof(OM_FLASH_CFG_INFO_STRU) != lReturnLen)
    {
        SD_FS_SETFS(sd_fs);
        return VOS_ERR;
    }

    SD_FS_SETFS(sd_fs);
    return VOS_OK;
}
VOS_UINT32 OM_FSCheckSpace(VOS_VOID)
{
    VOS_CHAR                            acFilePath[OM_SD_LOG_PATH_MAX_LENGTH] = {0};
    VOS_UINT32                          sd_fs = 0;

    /* 比较当前文件存储是否达到上限 */
    if ((g_stFSCfgFileInfo.ulFileMaxId - g_stFSCfgFileInfo.ulFileMinId) < g_stFlashLogCfg.ulGULogFileSize)
    {
        return VOS_OK;
    }

    sd_fs = SD_FS_GETFS();
    SD_FS_SETFS(KERNEL_DS);

    /* 删除旧文件以保证空间可用 */
    while (g_stFSCfgFileInfo.ulFileMinId < g_stFSCfgFileInfo.ulFileMaxId)
    {
        VOS_sprintf((VOS_CHAR *)acFilePath, "%s_%04d.%s",
            g_stFSLogFileInfo.acName,
            g_stFSCfgFileInfo.ulFileMinId,
            g_acFSLogType);

        g_stFSCfgFileInfo.ulFileMinId++;

        SD_FS_RMFILE(acFilePath);

        if ((g_stFSCfgFileInfo.ulFileMaxId - g_stFSCfgFileInfo.ulFileMinId) < g_stFlashLogCfg.ulGULogFileSize)
        {
            SD_FS_SETFS(sd_fs);

            OM_FSFlushCfgFile();

            return VOS_OK;
        }
    }

    SD_FS_SETFS(sd_fs);

    OM_FSFlushCfgFile();

    return VOS_ERR;
}

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)

VOS_UINT32 OM_AddHeadInfoToFile(OM_UCMX_HEAD_INFO_STRU *pstOmUcmxHeadInfo)
{
    pstOmUcmxHeadInfo->usFileTag         = UCMX_FILE_HEAD_TAG;
    pstOmUcmxHeadInfo->ucFileVersion     = 1;
    pstOmUcmxHeadInfo->ucHeadSegNum      = 1;                               /* 文件头个数 */
    pstOmUcmxHeadInfo->ulSn              = g_stFSCfgFileInfo.ulFileMaxId;   /* 文件分组时的文件编号 */

    pstOmUcmxHeadInfo->enSEGType3        = OM_LOG_HEAD_SEG_UE_INFO;
    pstOmUcmxHeadInfo->ucHeadLength3     = 48;
    /* IMEI 号 */
    if (VOS_OK != NV_Read(en_NV_Item_IMEI, pstOmUcmxHeadInfo->aucImei, OM_IMEI_NV_LEN))
    {
        vos_printf("OM_AddHeadInfoToFile: read Imei fail");
        return VOS_ERR;
    }

    /* 版本类型 */
#if (VOS_WIN32 == VOS_OS_VER)
    pstOmUcmxHeadInfo->ucUEBoardType = OM_WIN32_PLATFORM;
#else
#ifdef BOARD_ASIC
        pstOmUcmxHeadInfo->ucUEBoardType = OM_ASIC_PLATFORM;
#else
        pstOmUcmxHeadInfo->ucUEBoardType = OM_FPGA_PLATFORM;
#endif
#endif  /*(VOS_WIN32 == VOS_OS_VER)*/

    /* 最大版本长度 30 字符+ \0 */
    VOS_MemCpy((VOS_CHAR*)pstOmUcmxHeadInfo->aucUEVersion, PRODUCT_CFG_FULL_VERSION_STR, (VER_MAX_LENGTH+1));

    return VOS_OK;

}
#endif


VOS_UINT32 OM_FSCheckFileIsFull(VOS_UINT32 ulLength)
{
    VOS_CHAR                            acFilePath[OM_SD_LOG_PATH_MAX_LENGTH] = {0};
    VOS_INT                             lTempHandle;
    VOS_UINT32                          sd_fs = 0;

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
    OM_UCMX_HEAD_INFO_STRU              stOmUcmxHeadInfo;
    VOS_INT                             lReturnLen;
#endif

    /*判断当前写入长度是否使得LOG溢出*/
    if (g_stFSLogFileInfo.ulFileMaxSize >=
            ((VOS_INT)ulLength + g_stFSLogFileInfo.ulFileSize))
    {
        return VOS_OK;
    }

    sd_fs = SD_FS_GETFS();
    SD_FS_SETFS(KERNEL_DS);

    /* 关闭正在写的文件 */
    SD_FS_CLOSE(g_stFSLogFileInfo.lFileHandle);

    if (VOS_OK != OM_FSCheckSpace())
    {
        SD_FS_SETFS(sd_fs);

        return VOS_ERR;
    }

    VOS_sprintf(acFilePath, "%s_%04d.%s",
                g_stFSLogFileInfo.acName,
                g_stFSCfgFileInfo.ulFileMaxId,
                g_acFSLogType);

    /*打开LOG文件，并且初始化为空，如果此文件不存在，则创建此文件*/
    lTempHandle = SD_FS_OPEN(acFilePath, O_CREAT|O_RDWR|O_TRUNC, OM_SD_FILE_MODE);

    /*打开失败*/
    if (lTempHandle < 0)
    {
        g_stFSLogFileInfo.bIsWritten    = VOS_FALSE;
        g_stFSLogFileInfo.lFileHandle   = -1;

        g_stFSLogFileInfo.ulErrLog  = LOG_OPERATION_OPEN_FILE_FAIL;

        SD_FS_SETFS(sd_fs);

        return VOS_ERR;
    }

    g_stFSLogFileInfo.lFileHandle   = lTempHandle;
    g_stFSLogFileInfo.ulFileSize    = 0;

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
    /* LOG压缩算法开启，需要增加头文件信息 */
    if(VOS_OK != OM_AddHeadInfoToFile(&stOmUcmxHeadInfo))
    {
        return VOS_ERR;
    }

    /*写入LOG内容*/
    lReturnLen = SD_FS_WRITE(g_stFSLogFileInfo.lFileHandle, (VOS_CHAR*)&stOmUcmxHeadInfo, sizeof(stOmUcmxHeadInfo));

    if (lReturnLen != sizeof(stOmUcmxHeadInfo))
    {
        g_stFSLogFileInfo.ulWriteFailNum++;
        g_stFSLogFileInfo.ulErrLog  = LOG_OPERATION_WRITE_FILE_FAIL;
        SD_FS_SETFS(sd_fs);

        return VOS_ERR;
    }

    SD_FS_FILE_SYNC(g_stFSLogFileInfo.lFileHandle);
    if (lReturnLen < 0)
    {
        /* make pclint happy*/
    }
#endif

    g_stFSCfgFileInfo.ulFileMaxId++;

    SD_FS_SETFS(sd_fs);

    OM_FSFlushCfgFile();

    return VOS_OK;
}
VOS_UINT32 OM_FSCheckWriteCount(VOS_VOID)
{
    VOS_CHAR                            acFilePath[OM_SD_LOG_PATH_MAX_LENGTH] = {0};
    VOS_INT                             lTempHandle;
    VOS_UINT32                          sd_fs = 0;

    /* 当计数到时，将文件关闭，并重新打开文件 */
    if (0 != g_stFSLogFileInfo.ulRemainCount)
    {
        return VOS_OK;
    }

    sd_fs = SD_FS_GETFS();
    SD_FS_SETFS(KERNEL_DS);

    SD_FS_CLOSE(g_stFSLogFileInfo.lFileHandle);

    VOS_sprintf(acFilePath, "%s_%04d",
                g_stFSLogFileInfo.acName,
                g_stFSCfgFileInfo.ulFileMaxId - 1);

    lTempHandle = SD_FS_OPEN(acFilePath,O_CREAT|O_RDWR|O_APPEND, OM_SD_FILE_MODE);

    /*打开失败*/
    if (lTempHandle < 0)
    {
        g_stFSLogFileInfo.bIsWritten    = VOS_FALSE;
        g_stFSLogFileInfo.lFileHandle   = -1;

        g_stFSLogFileInfo.ulErrLog  = LOG_OPERATION_OPEN_FILE_FAIL;

        SD_FS_SETFS(sd_fs);

        return VOS_ERR;
    }

    g_stFSLogFileInfo.lFileHandle     = lTempHandle;
    g_stFSLogFileInfo.ulRemainCount   = g_ulLogMaxCout;

    SD_FS_SETFS(sd_fs);
    return VOS_OK;
}


VOS_UINT32 OM_FSWriteLogFile(VOS_CHAR *pcLogData, VOS_UINT32 ulLength)
{
    VOS_INT                             lReturnLen;
    VOS_UINT32                          sd_fs = 0;

    /*判断当前文件是否可写*/
    if (VOS_FALSE == g_stFSLogFileInfo.bIsWritten)
    {
        return VOS_ERR;
    }

    /*判断写入长度是否超过了文件的最大长度*/
    if (g_stFSLogFileInfo.ulFileMaxSize < ulLength)
    {
        g_stFSLogFileInfo.ulErrLog  = LOG_OPERATION_LENGTH_TOOBIG;
        return VOS_ERR;
    }

    if (VOS_OK != OM_FSCheckFileIsFull(ulLength))
    {
        return VOS_ERR;
    }

    if (VOS_OK != OM_FSCheckWriteCount())
    {
        return VOS_ERR;
    }

    sd_fs = SD_FS_GETFS();
    SD_FS_SETFS(KERNEL_DS);

    /*写入LOG内容*/
    lReturnLen = SD_FS_WRITE(g_stFSLogFileInfo.lFileHandle, pcLogData, ulLength);

    if (ulLength != lReturnLen)
    {
        g_stFSLogFileInfo.ulWriteFailNum++;
        g_stFSLogFileInfo.ulErrLog  = LOG_OPERATION_WRITE_FILE_FAIL;

        SD_FS_SETFS(sd_fs);

        return VOS_ERR;
    }

    /* 更新LOG文件写的次数和实际大小 */
    g_stFSLogFileInfo.ulFileSize += ulLength;

    lReturnLen = SD_FS_FILE_SYNC(g_stFSLogFileInfo.lFileHandle);

    if (lReturnLen < 0)
    {
        /* make pclint happy*/
    }

    SD_FS_SETFS(sd_fs);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
VOS_UINT32 OM_CompressRcvLog(VOS_CHAR *pcLogData, VOS_UINT32 ulLength)
{
    VOS_UINT32                          ulCompressDataLen = 0;
    VOS_UINT32                          ulTempPackLen     = 0; /* 临时存放未压缩数据长度 */
    VOS_UINT32                          ulRemainLen       = 0; /* 余数长度 */
    VOS_UINT32                          ulNextPackLen     = 0; /* 留作下一包压缩数据 */
    VOS_UINT32                          ulIndexMax        = 0;
    VOS_UINT32                          ulIndex           = 0;

    g_stCompressDebugLog.ulRcvNum++;
    g_stCompressDebugLog.ulRcvLen += ulLength;

    /* 分隔标记 */
    g_stCompressData.ulDataTag    = UCMX_DATA_TAG;

    /* 每次压数据大小固定，算出余数压缩量 */
    ulRemainLen = ((ulLength + g_ulPackLen) % OM_MAX_PACK_DATA_LEN);
    ulIndexMax  = (ulLength + g_ulPackLen)/OM_MAX_PACK_DATA_LEN;

    for (ulIndex=0; ulIndex<ulIndexMax; ulIndex++)
    {
        VOS_MemCpy(g_aucPackData + g_ulPackLen, pcLogData + ((ulIndex * OM_MAX_PACK_DATA_LEN) - ulTempPackLen),
                   (OM_MAX_PACK_DATA_LEN - g_ulPackLen));

        if (0 == ulIndex)
        {
            /* 已经组包内容长度 */
            ulTempPackLen = g_ulPackLen;
        }

        ulCompressDataLen = fastlz_compress(g_aucPackData, OM_MAX_PACK_DATA_LEN, g_stCompressData.aucData);

        g_stCompressDebugLog.ulCompressLen += ulCompressDataLen;

        g_stCompressData.ulDataLength = ulCompressDataLen;
        OM_FSWriteLogFile((VOS_CHAR *)&g_stCompressData, (ulCompressDataLen + (2*sizeof(VOS_UINT32))));

        /* 组包长度清零 */
        g_ulPackLen   = 0;

    }

    /* 留作下一包压缩 */
    ulNextPackLen = (ulRemainLen - g_ulPackLen);
    VOS_MemCpy(g_aucPackData + g_ulPackLen, pcLogData + (ulLength - ulNextPackLen), ulNextPackLen);
    g_ulPackLen += ulNextPackLen;

    return VOS_OK;

}
#endif

#endif

#if (VOS_LINUX == VOS_OS_VER)

VOS_VOID OM_FSShowInfo(VOS_VOID)
{
    vos_printf("File can write ===== %d\r\n", g_stFSLogFileInfo.bIsWritten);
    vos_printf("File Handle ======== %d\r\n", g_stFSLogFileInfo.lFileHandle);
    vos_printf("File RemainCount === %d\r\n", g_stFSLogFileInfo.ulRemainCount);
    vos_printf("File current Size == %d\r\n", g_stFSLogFileInfo.ulFileSize);
    vos_printf("File Max Size ====== %d\r\n", g_stFSLogFileInfo.ulFileMaxSize);
    vos_printf("File Error Info ==== %d\r\n", g_stFSLogFileInfo.ulErrLog);
    vos_printf("File Write Error === %d\r\n", g_stFSLogFileInfo.ulWriteFailNum);

    vos_printf("File Min ID ======== %d\r\n", g_stFSCfgFileInfo.ulFileMinId);
    vos_printf("File Max ID ======== %d\r\n", g_stFSCfgFileInfo.ulFileMaxId);

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
    vos_printf("Rcv Log len   = %d\r\n", g_stCompressDebugLog.ulRcvLen);
    vos_printf("Rcv Log Num   = %d\r\n", g_stCompressDebugLog.ulRcvNum);
    vos_printf("Compress Log len= %d\r\n", g_stCompressDebugLog.ulCompressLen);
    vos_printf("pack Log len= %d\r\n", g_ulPackLen);

#endif
    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

