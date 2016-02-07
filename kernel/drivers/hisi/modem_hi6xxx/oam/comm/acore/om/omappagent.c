/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : omappagent.c
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2008年5月3日
  最近修改      :
  功能描述      : 该C文件给出了OM模块的实现
  函数列表      :
  修改历史      :
  1.日    期    : 2008年5月3日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "OmApp.h"
#include "Omappagent.h"
#include "OmAppRl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/*lint -e767 修改人：甘兰47350；检视人：李霄46160；原因简述：LOG方案设计需要*/
#define    THIS_FILE_ID        PS_FILE_ID_ACPU_OMAGENT_C
/*lint +e767 修改人：甘兰47350；检视人：lixiao；*/

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
extern MsgBlock         *g_pstOmAcpuCnfMsg;
extern VOS_SEM           g_ulOmAcpuCnfSem;
extern VOS_SEM           g_ulOmAcpuSyncSem;

extern VOS_UINT32        OM_AcpuInit(VOS_VOID);

extern VOS_VOID          PPM_HsicConnectProc(VOS_VOID);

extern VOS_UINT32        OM_AutoConfigProc(VOS_VOID);

/*****************************************************************************
 Prototype       : OM_CAgentMsgProc
 Description     : 处理来自CCPU AGENT的消息.
 Input           : pMsg -- The pointer of the msg.
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_AcpuAgentMsgProc(MsgBlock* pMsg)
{
    VOS_UINT16                  usPrimId;

    /* 从消息前两个字节中取出原语ID */
    usPrimId = *(VOS_UINT16*)(pMsg->aucValue);

    OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_1, pMsg->ulSenderPid, ACPU_PID_OMAGENT, *((VOS_UINT32*)pMsg->aucValue));

    /* 判断是否为CCCPU AGENT发来的回复消息 */
    if (IS_CAGENT_CNF_MSG(usPrimId))
    {
        /* 判断消息是否被释放 */
        if (VOS_NULL_PTR == g_pstOmAcpuCnfMsg)
        {
            /* 标记该消息不用释放 */
            VOS_ReserveMsg(ACPU_PID_OMAGENT, pMsg);

            g_pstOmAcpuCnfMsg = pMsg;

            /* 释放信号量，使得调用API任务继续运行 */
            VOS_SmV(g_ulOmAcpuCnfSem);
        }

        /* CCPU 已经OK,可以进行SD卡配置信息 */
        if(OM_AUTOCONFIG_CNF == usPrimId)
        {
            OM_AutoConfigProc();
        }

        OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

        return;
    }

    /* 否则为CCPU发来不需要回复的请求消息 */
    if (OM_OLED_CLEAR_REQ == usPrimId)
    {
        OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

        return;
    }

    if (OM_OLED_DISPLAY_REQ == usPrimId)
    {
        OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

        return;
    }

    /* Modified by h59254 for AP-Modem Personalisation Project, 2012/04/12, begin */
    if (OM_HSIC_CONNECT_REQ == usPrimId)
    {
        PPM_HsicConnectProc();

        OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

        return;
    }
    /* Modified by h59254 for AP-Modem Personalisation Project, 2012/04/12, end */

    if(OM_RECORD_DBU_INFO_REQ == usPrimId)
    {
        OM_AcpuLogShowToFile(VOS_FALSE);
    }

    if (OM_CONNECT_INFO_CNF == usPrimId)
    {
        OMRL_AcpuConnectInfoReportEvent((OM_CONNECT_INFO_CNF_STRU *)pMsg);
    }

    OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

    return;
}

/*****************************************************************************
 函 数 名  : MNTN_ErrorLog
 功能描述  : 将错误信息记录到ERRORLOG中
 输入参数  : cFileName: 调用函数处的文件名
             ulFileId: 调用函数处的文件ID
             ulLine: 调用函数处在文件中行号
             ulErrNo: 错误号索引
             pRecord: 记录存放位置
             ulLen: 记录整个pRecord的长度，包括记录头，单位：字节
 输出参数  : VOID
 返 回 值  : VOS_ERR:函数执行过程中出现错误
             VOS_OK:函数执行正常
 修改历史      :
  1.日    期   : 2011年7月1日
    作    者   : g47350
    修改内容   : 新生成函数
*****************************************************************************/
unsigned int MNTN_ErrorLog(char * cFileName, unsigned int ulFileId, unsigned int ulLine,
                unsigned int ulErrNo, void *pRecord, unsigned int ulLen)
{
    ERRORLOG_REQ_STRU       *pstErrorLogReq;
    ERRORLOG_CNF_STRU       *pstErrorLogCnf;
    VOS_UINT32              ulResult;

    /* 参数检测 */
    if ((VOS_NULL_PTR == cFileName) || (VOS_NULL_PTR == pRecord))
    {
        return OM_ACPU_PARA_ERR;
    }

#if (VOS_OS_VER == VOS_LINUX)
    if( in_interrupt() )
    {
        return OM_ACPU_RUN_IRQ;
    }
#endif

    pstErrorLogReq = (ERRORLOG_REQ_STRU*)VOS_AllocMsg(ACPU_PID_OMAGENT,
                                                ERRORLOG_HEAD_LEN + ulLen);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstErrorLogReq)
    {
        return OM_ACPU_ALLOC_FAIL;
    }

    pstErrorLogReq->ulReceiverPid = CCPU_PID_OMAGENT;
    pstErrorLogReq->usPrimId      = ERRORLOG_REQ;
    pstErrorLogReq->ulFileId      = ulFileId;
    pstErrorLogReq->ulLine        = ulLine;
    pstErrorLogReq->ulErrNo       = ulErrNo;
    pstErrorLogReq->ulLen         = ulLen;

    /* 为了确保aucFileName最后字节为'\0',拷贝长度需要加1 */
    VOS_MemCpy(pstErrorLogReq->aucFileName, cFileName, VOS_StrLen(cFileName) + 1);
    VOS_MemCpy(pstErrorLogReq->aucData, pRecord, ulLen);


    /* 如果有任务正在进行中，需要等待其完成 */
    if (VOS_OK != VOS_SmP(g_ulOmAcpuSyncSem, 0))
    {
        VOS_FreeMsg(ACPU_PID_OMAGENT, pstErrorLogReq);

        return OM_ACPU_SYNC_TIMEOUT;
    }

    /* 将请求消息发送给CCPU */
    if (VOS_OK != VOS_SendMsg(ACPU_PID_OMAGENT, pstErrorLogReq))
    {
        VOS_SmV(g_ulOmAcpuSyncSem);

        return OM_ACPU_SEND_FAIL;
    }

    /* 等待CCPU的回复 */
    if (VOS_OK != VOS_SmP(g_ulOmAcpuCnfSem, WAITING_CNF_TIMEOUT_LEN))
    {
        VOS_SmV(g_ulOmAcpuSyncSem);

        return OM_ACPU_CNF_TIMEOUT;
    }

    pstErrorLogCnf = (ERRORLOG_CNF_STRU*)g_pstOmAcpuCnfMsg;

    /* 判断回复消息内容是否正确 */
    if (ERRORLOG_CNF != pstErrorLogCnf->usPrimId)
    {
        VOS_FreeReservedMsg(ACPU_PID_OMAGENT, g_pstOmAcpuCnfMsg);

        VOS_SmV(g_ulOmAcpuSyncSem);

        return OM_ACPU_CNF_ERR;
    }

    ulResult = (VOS_UINT32)pstErrorLogCnf->usResult;

    VOS_FreeReservedMsg(ACPU_PID_OMAGENT, g_pstOmAcpuCnfMsg);

    VOS_SmV(g_ulOmAcpuSyncSem);

    return ulResult;
}

/*****************************************************************************
 函 数 名  : OM_Acpu_WriteLogFile
 功能描述  : 将错误信息记录到ERRORLOG中
 输入参数  : cFileName: log文件名，包含路径信息
             pRecord: 记录数据内容
             ulLen: 记录数据长度
 输出参数  : VOID
 返 回 值  : 其他:函数执行过程中出现错误
             VOS_OK:函数执行正常
 修改历史      :
  1.日    期   : 2012年5月8日
    作    者   : zhuli
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 OM_Acpu_WriteLogFile(VOS_CHAR * cFileName, void *pRecord, VOS_UINT32 ulLen)
{
    OM_WRITELOG_REQ_STRU    *pstLogReq;

    /* 参数检测 */
    if ((VOS_NULL_PTR == cFileName) || (VOS_NULL_PTR == pRecord))
    {
        return OM_ACPU_PARA_ERR;
    }

    pstLogReq = (OM_WRITELOG_REQ_STRU*)VOS_AllocMsg(ACPU_PID_OMAGENT,
                                            (sizeof(OM_WRITELOG_REQ_STRU)-VOS_MSG_HEAD_LENGTH)+ulLen);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstLogReq)
    {
        return OM_ACPU_ALLOC_FAIL;
    }

    pstLogReq->ulReceiverPid = CCPU_PID_OMAGENT;
    pstLogReq->usPrimId      = OM_WRITE_LOG_REQ;
    pstLogReq->ulLen         = ulLen;

    /* 为了确保aucFileName最后字节为'\0',拷贝长度需要加1 */
    VOS_MemCpy(pstLogReq->aucFileName, cFileName, VOS_StrLen(cFileName)+1);
    VOS_MemCpy(pstLogReq->aucData, pRecord, ulLen);

    /* 将请求消息发送给CCPU */
    if (VOS_OK != VOS_SendMsg(ACPU_PID_OMAGENT, pstLogReq))
    {
        return OM_ACPU_SEND_FAIL;
    }

    return VOS_OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


