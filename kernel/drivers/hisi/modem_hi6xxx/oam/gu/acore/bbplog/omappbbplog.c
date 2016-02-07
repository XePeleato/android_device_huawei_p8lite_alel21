
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "om.h"
#include "omprivate.h"
#include "ombbplog.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/*lint -e767 修改人：蒋德彬 */
#define    THIS_FILE_ID        PS_FILE_ID_OM_APP_BBP_LOG_C
/*lint +e767 修改人：蒋德彬 */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 记录从底软获取到的DUMP内存地址和大小 */
BBP_DUMP_GET_FIX_DDR_ADDR               g_stBbpDumpFixDdr;

/*****************************************************************************
  3 外部函数声明
*****************************************************************************/
extern VOS_UINT32 Om_AcpuDumpReboot(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 Om_QueryMsgFunc(OM_REQ_PACKET_STRU *pRspPacket, OM_MSG_FUN_STRU *pstMsgFuncTable, VOS_UINT32 ulTotalNum);

/*****************************************************************************
  4 消息处理函数映射表
*****************************************************************************/
/*PC下发的 BBP采数与可维可测消息处理函数映射表 */
OM_MSG_FUN_STRU                         g_astAcpuOmBbpMsgFunTbl[] =
{
    {Om_AcpuDumpReboot        ,    APP_OM_BBP_DUMP_DUMP_REQ,    OM_APP_BBP_DUMP_DUMP_CNF},
};


/*****************************************************************************
  5 函数实现
*****************************************************************************/


VOS_UINT32 Om_AcpuDumpReboot(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    BBP_DUMP_CHOOSE_STRU               *pstPcMsg;

    pstPcMsg        = (BBP_DUMP_CHOOSE_STRU*)pstAppToOmMsg->aucPara;

    if (BBP_DUMP_CHOOSE_YES == pstPcMsg->ulChooseRelt)
    {
        LogPrint("Om_AcpuDumpReboot: Reboot\r\n");

        /* 调用DRV 接口复位进入 fastboot模式 */
        mdrv_edma_bbp_sample_reboot();
    }

    return VOS_OK;
}
VOS_VOID Om_AcpuBbpConfigMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr)
{
    APP_OM_MSG_EX_STRU                 *pstAppToOmMsg;
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulTotalNum;
    VOS_UINT16                          usPrimId;
    VOS_UINT16                          usReturnPrimId;
    VOS_UINT32                          ulResult = VOS_ERR;

    pstAppToOmMsg   = (APP_OM_MSG_EX_STRU*)pRspPacket;
    ulTotalNum      = sizeof(g_astAcpuOmBbpMsgFunTbl) / sizeof(OM_MSG_FUN_STRU);
    usPrimId = pstAppToOmMsg->usPrimId;
    /* 查询处理函数 */
    for (ulIndex = 0; ulIndex < ulTotalNum; ulIndex++)
    {
        if (usPrimId == (VOS_UINT16)(g_astAcpuOmBbpMsgFunTbl[ulIndex].ulPrimId))
        {
            usReturnPrimId = (VOS_UINT16)(g_astAcpuOmBbpMsgFunTbl[ulIndex].ulReturnPrimId);
            ulResult = g_astAcpuOmBbpMsgFunTbl[ulIndex].pfFun(pstAppToOmMsg, usReturnPrimId);
            break;
        }
    }

    /* 没有查询到处理函数  */
    if (ulIndex == ulTotalNum)
    {
        OM_AcpuSendResultChannel(pstAppToOmMsg->ucCpuId, pstAppToOmMsg->ucFuncType, ulResult, usPrimId);
    }
    return;

}


VOS_VOID Om_AcpuQueryDumpMsgProc(MsgBlock* pMsg)
{
    BBP_DUMP_MSG_STRU                  *pstBbpDumpMsg;
    VOS_UINT16                          usPrimId;

    usPrimId = *(VOS_UINT16*)(pMsg->aucValue);
    if (BBP_QUERY_DUMP_INFO_REQ == usPrimId)
    {
        /* 申请消息发送给CCPU */
        pstBbpDumpMsg = (BBP_DUMP_MSG_STRU*)VOS_AllocMsg(ACPU_PID_PAM_OM,
                                sizeof(BBP_DUMP_MSG_STRU) - VOS_MSG_HEAD_LENGTH);

        /* 分配消息失败 */
        if (VOS_NULL_PTR == pstBbpDumpMsg)
        {
            return;
        }

        pstBbpDumpMsg->ulReceiverPid           = CCPU_PID_PAM_OM;
        pstBbpDumpMsg->usPrimId                = BBP_QUERY_DUMP_INFO_CNF;
        pstBbpDumpMsg->stDumpDdrInfo.ulPhyAddr = g_stBbpDumpFixDdr.ulPhyAddr;
        pstBbpDumpMsg->stDumpDdrInfo.ulPhySize = g_stBbpDumpFixDdr.ulPhySize;

        (VOS_VOID)VOS_SendMsg(ACPU_PID_PAM_OM, pstBbpDumpMsg);
    }

    return;
}

/*lint -save -e102 -e10 -e2 -e40 -e533 -e31 -e830 -e522 -e718 -e746 -e702 -e565   -e64 -e23 -e63 -e26 -e578 -e132*/
extern unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);

/*****************************************************************************
* 函 数 名  : Om_QueryDumpPhyAddr
*
* 功能描述  : 在代码编译阶段将CMD LINE中的DumpPhyAddr地址解析出来
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
VOS_VOID Om_QueryDumpPhyAddr(VOS_CHAR *pucChar)
{
    g_stBbpDumpFixDdr.ulPhyAddr = (VOS_UINT32)simple_strtoul(pucChar, VOS_NULL, VOS_NULL);

    /* 物理地址是32位的实地址并且是8字节对齐的 */
    if ((VOS_NULL != (g_stBbpDumpFixDdr.ulPhyAddr % 4))
        || (VOS_NULL == g_stBbpDumpFixDdr.ulPhyAddr))
    {
        g_stBbpDumpFixDdr.ulPhyAddr = VOS_NULL;
    }

    return;
}

#if (!defined(DMT)) && (VOS_WIN32 != VOS_OS_VER)
early_param("mdmreglogbase", Om_QueryDumpPhyAddr);
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
VOS_VOID Om_QueryDumpPhySize(VOS_CHAR *pucChar)
{
    /* Buffer的大小以Byte为单位 */
    g_stBbpDumpFixDdr.ulPhySize = (VOS_UINT32)simple_strtoul(pucChar, VOS_NULL, VOS_NULL);

    return;
}

#if (!defined(DMT)) && (VOS_WIN32 != VOS_OS_VER)
early_param("mdmreglogsize", Om_QueryDumpPhySize);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

