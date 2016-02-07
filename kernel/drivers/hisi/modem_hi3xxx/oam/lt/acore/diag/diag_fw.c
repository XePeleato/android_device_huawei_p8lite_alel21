/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_fw.c
  Description     :
  History         :
     1.w00182550       2012-11-20   Draft Enact

******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
/*lint -save -e537*/
#include  "diag_fw.h"
#include  "diag_api.h"
#include  "diag_cfg.h"
#include  "msp_errno.h"
#include  "msp_drx.h"
#include  "diag_debug.h"
#include  "diag_common.h"
#include  "diag_cmd_id.h"
#include  "diag_app_agent.h"
#include  "SCMProc.h"
#include "diag_vcom.h"
#include "diag_tcp.h"
#include  "drx_msp_api.h"
#include  "diag_port.h"

/*lint -restore*/
/*lint -save -e767 原因:Log打印*/
#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_FW_C
/*lint -restore +e767*/
/*lint --e{565, 718, 746} */
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/

DIAG_FW_RX_INFO_STRU g_stDiagFwNodeInfoStru = {0};

VOS_SEM g_DiagFwNodeSem =(VOS_SEM)0;

/*****************************************************************************
  3 Function
*****************************************************************************/

/*****************************************************************************
 Function Name   : diag_FwGetDiagNodeInfo
 Description     : 获取DIAG FW缓存的链表信息
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-22  Draft Enact

*****************************************************************************/
DIAG_FW_RX_INFO_STRU* diag_FwGetDiagNodeInfo(VOS_VOID)
{
    return &g_stDiagFwNodeInfoStru;
}

/*****************************************************************************
 Function Name   : diag_FwGetExeCmdNode
 Description     : 通道命令ID查找执行节点
 Input           :VOS_UINT32 ulCmdId
 Output          : None
 Return          : DIAG_FW_RX_INFO_STRU*

 History         :
    1.w00182550      2012-11-22  Draft Enact

*****************************************************************************/
DIAG_FW_RX_INFO_STRU* diag_FwGetExeCmdNode(VOS_UINT32 ulCmdId)
{
    DIAG_FW_RX_INFO_STRU* pHeadNode = NULL;
    DIAG_FW_RX_INFO_STRU* pTempNode = NULL;
    LIST_S* me = NULL;

    /* 获取链表头 */
    pHeadNode= diag_FwGetDiagNodeInfo();

    /*添加信号量保护*/
    (VOS_VOID)VOS_SmP(g_DiagFwNodeSem,0);

    /* 在链表中查找每个子命令结点*/
    blist_for_each(me, &pHeadNode->HiDiagFwList)
    {
        pTempNode = blist_entry(me, DIAG_FW_RX_INFO_STRU, HiDiagFwList);

        /* 找到需要的节点*/
        if (ulCmdId == pTempNode->ulCmdId)
        {
            (VOS_VOID)VOS_SmV(g_DiagFwNodeSem);
            return pTempNode;
        }
    }
    (VOS_VOID)VOS_SmV(g_DiagFwNodeSem);
    return NULL;
}

/*lint -save -e40*/ /* PID 定义找不到错误*/

/*****************************************************************************
 Function Name   : diag_FwDelCmdNode
 Description     : 删除已经处理完的节点
 Input           :DIAG_FW_RX_INFO_STRU* pstCmdNode
 Output          : None
 Return          : VOS_VOID

 History         :
    1.w00182550      2012-11-22  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwDelCmdNode(VOS_UINT32 ulCmdId)
{

    DIAG_FW_RX_INFO_STRU* pHeadNode = NULL;
    DIAG_FW_RX_INFO_STRU* pTempNode = NULL;
    LIST_S* me = NULL;

    /* 获取链表头 */
    pHeadNode = diag_FwGetDiagNodeInfo();

    /*遍历链表查找相同cmd id*/
    (VOS_VOID)VOS_SmP(g_DiagFwNodeSem,0);
    blist_for_each(me, &pHeadNode->HiDiagFwList)
    {
        pTempNode = blist_entry(me, DIAG_FW_RX_INFO_STRU, HiDiagFwList);
        if(ulCmdId == pTempNode->ulCmdId)
            break;
    }
    (VOS_VOID)VOS_SmV(g_DiagFwNodeSem);
    /*未找到则直接返回*/
    if(NULL == pTempNode)
    {
        return;
    }
    if(ulCmdId != pTempNode->ulCmdId)
    {
        return;
    }
    /*添加信号量保护*/
    (VOS_VOID)VOS_SmP(g_DiagFwNodeSem,0);

    /*删除节点*/
    blist_del(&pTempNode->HiDiagFwList);

    (VOS_VOID)VOS_SmV(g_DiagFwNodeSem);

    /*释放内存*/
    VOS_MemFree(MSP_PID_DIAG_FW,pTempNode);

    return ;
}


/*****************************************************************************
 Function Name   : diag_FwAddCmdToList
 Description     : 添加解码后数据到缓存中
 Input           :VOS_VOID * pstReq
                VOS_UINT32 ulRcvlen
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
DIAG_FW_RX_INFO_STRU* diag_FwAddCmdToList(VOS_UINT8 * pstReq, VOS_UINT32 ulRcvlen)
{
    DIAG_FW_RX_INFO_STRU* pHeadNode = NULL;
    DIAG_FW_RX_INFO_STRU* pNewNode = NULL;
    VOS_UINT32 ret;

    /*申请一个节点大小*/
    pNewNode = VOS_MemAlloc(MSP_PID_DIAG_FW,DYNAMIC_MEM_PT,sizeof(DIAG_FW_RX_INFO_STRU) + ulRcvlen);
    if (NULL == pNewNode)
    {
        return NULL;
    }

    VOS_MemSet(pNewNode, 0, ulRcvlen + sizeof(DIAG_FW_RX_INFO_STRU));

    /*将新来的命令缓存到节点中*/
    VOS_MemCpy(pNewNode->ucDiagRcvData, pstReq, ulRcvlen);

    /*将CMDID取出来，用于后面查找执行节点*/
    pNewNode->ulCmdId = (((MSP_DIAG_HEAD_STRU*)(pNewNode->ucDiagRcvData+sizeof(MSP_SOCP_HEAD_STRU)))->ulID);

    DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_LIST_CMD,pNewNode->ulCmdId,ulRcvlen,0);

    /*启动定时器，以便AGENT模块没有回复时DIAG FW能正常工作*/
    ret = VOS_StartRelTimer(&pNewNode->fwCtrlTimer, MSP_PID_DIAG_FW,DIAG_CMD_TIMEOUT_TIME_LEN, 0,pNewNode->ulCmdId,VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION);
    if(ret != ERR_MSP_SUCCESS)
    {
        diag_printf("VOS_StartRelTimer fail [%s]\n",__func__);
    }
    /* 获取链表头 */
    pHeadNode = diag_FwGetDiagNodeInfo();

    /*添加信号量保护*/
    (VOS_VOID)VOS_SmP(g_DiagFwNodeSem,0);

    /* 插入节点到链表尾部 */
    blist_add_tail(&pNewNode->HiDiagFwList, &pHeadNode->HiDiagFwList);

    (VOS_VOID)VOS_SmV(g_DiagFwNodeSem);

    return pNewNode;
}

/*****************************************************************************
 Function Name   : diag_FwTimeOutPackt
 Description     : 组超时SOCP包给HSO回复
 Input           :VOS_VOID * pstRxinfo
                VOS_UINT8 * pstSocpPackt
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-28  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwTimeOutPackt(VOS_VOID * pstRxinfo, VOS_UINT8 * pstSocpPackt)
{
    VOS_UINT64 ulTime = 0;

    /* 从REQ 数据pstRxinfo中复制出原有SOCP和DIAG HEADER，组SOCP头*/
    VOS_MemCpy(pstSocpPackt, ((DIAG_FW_RX_INFO_STRU*)pstRxinfo)->ucDiagRcvData,
        sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));

    /* 组SOCP 包*/
    ulTime                           = diag_GetFrameTime();
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->usSId              = MSP_SOCP_SID_DIAG_SERVER;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->usSSId             = MSP_SOCP_HIGH4BIT_SSID;  // 高四位
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ucServiceSessionId = MSP_SOCP_SERVER_SESSION_ID;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ucMsgType          = MSP_SOCP_MSG_TYPE_IND;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ulMsgTransId       = (g_DiagLogPktNum.ulIndNum)++;
    VOS_MemCpy(((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->aucTimeData, &ulTime, sizeof(VOS_UINT64));

    /* 组包结构化ID(DIAG HEADER)*/
    ((MSP_DIAG_HEAD_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU)))->ulID =
        (MSP_STRU_ID_GET_28_31_FIELD(MSP_STRU_ID_28_31_GROUP_MSP) | MSP_STRU_ID_GET_16_23_FIELD(MSP_STRU_ID_16_23_SYS_CMD)\
        | MSP_STRU_ID_GET_0_15_FIELD(DIAG_CMD_TIMER_OUT_IND));
    ((MSP_DIAG_HEAD_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU)))->ulDataSize =
        sizeof(DIAG_CMD_TIMER_OUT_IND_STRU);


    /*组包DIAG_CMD_TIMER_OUT_IND_STRU，应该再发送超时消息处做*/
    ((DIAG_CMD_TIMER_OUT_IND_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU) +
        sizeof(MSP_DIAG_HEAD_STRU)))->ulCmdID =
        ((MSP_DIAG_HEAD_STRU*)(((DIAG_FW_RX_INFO_STRU*)pstRxinfo)->ucDiagRcvData + sizeof(MSP_SOCP_HEAD_STRU)))->ulID;
}

/*****************************************************************************
 Function Name   : diag_FwCmdTimeOutProc
 Description     : 超时命令处理过程
 Input           :VOS_VOID * pstRxinfo
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-28  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FwCmdTimeOutProc(VOS_UINT32 ulCmdId)
{
    VOS_UINT32 ulTmpLen =0;
    VOS_UINT32 ret = ERR_MSP_FAILURE;
    VOS_UINT8 * pBuf =NULL;

    DIAG_FW_RX_INFO_STRU* pHeadNode = NULL;
    DIAG_FW_RX_INFO_STRU* pTempNode = NULL;
    LIST_S* me = NULL;

    /* 获取链表头 */
    pHeadNode= diag_FwGetDiagNodeInfo();

    /*添加信号量保护*/
    (VOS_VOID)VOS_SmP(g_DiagFwNodeSem,0);

    /* 在链表中查找每个子命令结点*/
    blist_for_each(me, &pHeadNode->HiDiagFwList)
    {
        pTempNode = blist_entry(me, DIAG_FW_RX_INFO_STRU, HiDiagFwList);

        /* 找到需要的节点*/
        if (ulCmdId == pTempNode->ulCmdId)
        {
            (VOS_VOID)VOS_SmV(g_DiagFwNodeSem);

            diag_printf("fw cmd 0x%x timer out, node found!\n", pTempNode->ulCmdId);
            ulTmpLen = (sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU)+ sizeof(DIAG_CMD_TIMER_OUT_IND_STRU));

            /*申请编码源buf*/
            diag_LockCoderSrcBuf();
            pBuf = diag_GetCoderSrcBuf(ulTmpLen,SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
            if(NULL == pBuf)
            {
                diag_UnLockCoderSrcBuf();
                return ERR_MSP_FAILURE;
            }
            /*打包SOCP头*/
            diag_FwTimeOutPackt(pTempNode, pBuf);

            /*拷贝要发送内容到buf中，并发送给SOCP*/
            ret = diag_SendCoderSrcToSocp(pBuf,ulTmpLen,SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
            diag_UnLockCoderSrcBuf();
            return ret;
        }
    }

    (VOS_VOID)VOS_SmV(g_DiagFwNodeSem);

    return ERR_MSP_INVALID_PARAMETER;

}

/*****************************************************************************
 Function Name   : diag_FwSocpCnfPackt
 Description     : 组SOCP包给HSO回复
 Input           :DIAG_FW_RX_INFO_STRU * pstRxinfo
                MSP_SOCP_HEAD_STRU * pstSocpPackt
                VOS_UINT32 ulCmdParaLen
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwSocpCnfPackt(DIAG_FW_RX_INFO_STRU * pstRxinfo, VOS_UINT8 * pstSocpPackt,VOS_UINT32 ulCmdParaLen)
{
    /*lint -save -e958*/
    VOS_UINT64 ulTime = 0;
    /*lint -restore*/
    if ((NULL == pstSocpPackt)||(NULL ==pstRxinfo))
    {
        return;
    }

    /* 从REQ 数据pstRxinfo中复制出原有SOCP和DIAG HEADER，组SOCP头*/
    VOS_MemCpy(pstSocpPackt, (pstRxinfo)->ucDiagRcvData,
        sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));

    /*更改SOCP头中一些需要更改的字段*/
    ulTime                           = diag_GetFrameTime();
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->usSId              = MSP_SOCP_SID_DIAG_SERVER;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->usSSId             = MSP_SOCP_HIGH4BIT_SSID_ACPU << 4;    // 高四位
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ucServiceSessionId = MSP_SOCP_SERVER_SESSION_ID;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ucMsgType          = MSP_SOCP_MSG_TYPE_CNF;
    VOS_MemCpy(((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->aucTimeData, &ulTime, 8);  // 暂时使用 TO DO

    /* 组包结构化ID(DIAG HEADER)*/
    ((MSP_DIAG_HEAD_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU)))->ulDataSize =
        sizeof(MSP_DIAG_DATA_CNF_STRU) + ulCmdParaLen;

    /* 组包MSP_DIAG_DATA_CNF_STRU,取出节点的AUID*/
    ((MSP_DIAG_DATA_CNF_STRU*)(pstSocpPackt +
        sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU)))->ulAuid =
        ((MSP_DIAG_DATA_REQ_STRU*)(((DIAG_FW_RX_INFO_STRU*)pstRxinfo)->ucDiagRcvData +
            sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU)))->ulAuid;
	((MSP_DIAG_DATA_CNF_STRU*)(pstSocpPackt +
        sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU)))->ulSn =
        ((MSP_DIAG_DATA_REQ_STRU*)(((DIAG_FW_RX_INFO_STRU*)pstRxinfo)->ucDiagRcvData +
            sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU)))->ulSn;
}

/*****************************************************************************
 Function Name   : diag_FwCmdCnfProc
 Description     : 将命令回复打包编码
 Input           :DIAG_FW_RX_INFO_STRU * pstRxinfo
                    VOS_UINT8* pData
                VOS_UINT32 ulLen
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-23  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FwCmdCnfProc(DIAG_FW_RX_INFO_STRU * pstRxinfo,VOS_UINT8* pData,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulTmpLen =0;
    VOS_UINT32 ret = ERR_MSP_FAILURE;
    VOS_UINT8 * pBuf =NULL;

    ulTmpLen = (DIAG_FULL_HEAD_LEN + ulLen);

    /*申请编码源buf*/
    diag_LockCoderSrcBuf();
    pBuf = diag_GetCoderSrcBuf(ulTmpLen,SOCP_CODER_SRC_LOM_CNF);
    if(NULL == pBuf)
    {
        diag_UnLockCoderSrcBuf();
        return ERR_MSP_FAILURE;
    }

    /*打包SOCP头*/
    diag_FwSocpCnfPackt(pstRxinfo, pBuf,ulLen);

    VOS_MemCpy(pBuf + DIAG_FULL_HEAD_LEN, pData, ulLen);

    /*发送给SOCP*/
    ret = diag_SendCoderSrcToSocp(pBuf,ulTmpLen,SOCP_CODER_SRC_LOM_CNF);
    diag_UnLockCoderSrcBuf();
    return ret;
}

/*****************************************************************************
 Function Name   : diag_FwAnsyCmdIndProc
 Description     : 将PS透传的IND命令打包编码
 Input           :VOS_UINT8* pData,
                    VOS_UINT32 ulLen
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-23  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FwAnsyCmdIndProc(VOS_UINT8* pData,VOS_UINT32 ulLen,VOS_UINT32 ulCmdId)
{
    VOS_UINT32 ulTmpLen =0;
    VOS_UINT32 ret = ERR_MSP_FAILURE;
    VOS_UINT8 * pBuf =NULL;

    ulTmpLen = (sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU)+ ulLen);

    /*检查DIAG是否初始化且HSO是否连接上*/
    if(!DIAG_IS_CONN_ON)
    {
        return ERR_MSP_NO_INITILIZATION;
    }

    /*申请编码源buf*/
    diag_LockCoderSrcBuf();
    pBuf = diag_GetCoderSrcBuf(ulTmpLen,SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
    if(NULL == pBuf)
    {
        diag_UnLockCoderSrcBuf();
        return ERR_MSP_FAILURE;
    }

    /*打包SOCP头*/
    diag_SocpIndCmdPkt(pBuf, MSP_STRU_ID_16_23_PS_TRANS, ulLen, (VOS_UINT16)ulCmdId);
    VOS_MemCpy(pBuf + sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU), pData, ulLen);

    /*发送给SOCP*/
    ret = diag_SendCoderSrcToSocp(pBuf,ulTmpLen,SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
    diag_UnLockCoderSrcBuf();
    return ret;
}

/*****************************************************************************
 Function Name   : diag_FwForwardCmdToAgent
 Description     : 将命令转发给AGENT模块处理
 Input           :DIAG_FW_RX_INFO_STRU* pData
                VOS_UINT32 ulLen
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-22  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FwForwardCmdToAgent(DIAG_FW_RX_INFO_STRU * pNode,VOS_UINT32 ulSendSize)
{
    VOS_UINT32 app_ret = ERR_MSP_SUCCESS;
    VOS_UINT32 comm_ret = ERR_MSP_SUCCESS;
    VOS_UINT32 ulCmdId = 0;
    VOS_UINT32 ulConCmdId = 0;
    VOS_UINT32 ulDisConCmdId = 0;

    ulConCmdId = (MSP_STRU_ID_GET_28_31_FIELD(MSP_STRU_ID_28_31_GROUP_MSP)
                 |MSP_STRU_ID_GET_16_23_FIELD(MSP_STRU_ID_16_23_SYS_CMD)
                 |MSP_STRU_ID_GET_0_15_FIELD(DIAG_CMD_HOST_CONNECT));
    ulDisConCmdId = (MSP_STRU_ID_GET_28_31_FIELD(MSP_STRU_ID_28_31_GROUP_MSP)
                 |MSP_STRU_ID_GET_16_23_FIELD(MSP_STRU_ID_16_23_SYS_CMD)
                 |MSP_STRU_ID_GET_0_15_FIELD(DIAG_CMD_HOST_DISCONNECT));

    ulCmdId = (((MSP_DIAG_HEAD_STRU*)DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pNode->ucDiagRcvData))->ulID);

    if((MDRV_RESET_CB_BEFORE == g_DiagResetingCcore)&&(ulCmdId == ulConCmdId))/*单独复位时,如果是Connect命令,返回错误*/
    {
        return ERR_MSP_UNAVAILABLE;
    }
    else if((MDRV_RESET_CB_BEFORE == g_DiagResetingCcore)&&(ulCmdId == ulDisConCmdId))/*单独复位时,如果是DisConnect命令,只给A核*/
    {
        /*将命令转发给APP AGENT 处理*/
        app_ret = diag_SendMsg(MSP_PID_DIAG_FW, MSP_PID_DIAG_APP_AGENT,ID_MSG_DIAG_CMD_REQ_FW_TO_APP_AGENT,pNode->ucDiagRcvData, ulSendSize);
        if(app_ret != ERR_MSP_SUCCESS)
        {
           DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_DECODE_CMD_ENTRY_ERROR, 0, 0, 4);
        }
        return app_ret;
    }
    /*将命令转发给APP AGENT 处理*/
    app_ret = diag_SendMsg(MSP_PID_DIAG_FW, MSP_PID_DIAG_APP_AGENT,ID_MSG_DIAG_CMD_REQ_FW_TO_APP_AGENT,pNode->ucDiagRcvData, ulSendSize);
    if(app_ret != ERR_MSP_SUCCESS)
    {
       DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_DECODE_CMD_ENTRY_ERROR, 0, 0, 2);
    }

     /*将命令转发给AGENT 处理*/
    comm_ret = diag_SendMsg(MSP_PID_DIAG_FW, MSP_PID_DIAG_AGENT,ID_MSG_DIAG_CMD_REQ_FW_TO_AGENT,pNode->ucDiagRcvData, ulSendSize);
    if(comm_ret != ERR_MSP_SUCCESS)
    {
       DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_DECODE_CMD_ENTRY_ERROR, 0, 0, 3);
    }

    /* 两个消息都发送失败，才返回ERROR，删除节点 */
    return (app_ret&comm_ret);
}
/*****************************************************************************
 Function Name   : diag_FwDealTimeOutCmdEntry
 Description     : 处理超时数据，组包回复给HSO
 Input           :VOS_VOID* pData
 Output          : None
 Return          : VOS_VOID

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwDealTimeOutCmdEntry(VOS_UINT32 ulCmdId)
{

    VOS_UINT32 ret = ERR_MSP_FAILURE;

    /*打超时包回复*/
    ret = diag_FwCmdTimeOutProc(ulCmdId);
    if(ret != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_TIME_OUT_ENTRY_ERROR,ret,0,0);
    }

    /*删除超时处理节点*/
    if(ret != ERR_MSP_INVALID_PARAMETER)
    {
        diag_FwDelCmdNode(ulCmdId);
    }

    return ;
}


/*****************************************************************************
 Function Name   : diag_FwDealDecodeCmdEntry
 Description     : 解码后数据处理入口
 Input           :VOS_VOID* pData  ,VOS_UINT32 ulLen
 Output          : None
 Return          : VOS_VOID

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwDealDecodeCmdEntry(VOS_UINT8* pData,VOS_UINT32 ulLen)
{
    DIAG_FW_RX_INFO_STRU *pNode =VOS_NULL;
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    if((NULL == pData)||(0 == ulLen))
    {
       return;
    }

    /*将解码后数据放入链表中*/
    pNode = diag_FwAddCmdToList(pData,ulLen);
    if (NULL == pNode)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_DECODE_CMD_ENTRY_ERROR,ret,ulLen,1);
        return ;
    }

    /*转发命令给对应的AGENT模块处理*/
    ret = diag_FwForwardCmdToAgent(pNode,ulLen);
    if (ERR_MSP_SUCCESS != ret)
    {
        /*删除定时器*/
        VOS_StopRelTimer(&pNode->fwCtrlTimer);

        /* 命令处理完毕，删除节点*/
        diag_FwDelCmdNode(pNode->ulCmdId);
    }
    return ;
}

/*****************************************************************************
 Function Name   : diag_FwDealAppAgentCnfEntry
 Description     : 从APP AGENT PID接收到的回复处理入口
 Input           :VOS_VOID* pData   ,VOS_UINT32 ulLen
 Output          : None
 Return          : VOS_VOID

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwDealAppAgentCnfEntry(VOS_UINT8* pData,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulCmdId =0;
    DIAG_FW_RX_INFO_STRU *pstRxinfo =NULL;
    VOS_UINT32 ret = ERR_MSP_FAILURE;

    if((NULL == pData)||(0 == ulLen))
    {
       return;
    }

    /*根据CMDID查找节点*/
    ulCmdId = ((DIAG_CMD_AGENT_TO_FW_CNF_STRU*)pData)->ulCmdId;
    pstRxinfo = diag_FwGetExeCmdNode(ulCmdId);
    if(NULL == pstRxinfo)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_APP_CNF_ENTRY_ERROR, ulCmdId, ulLen, 0);
        return;
    }

    /*删除定时器*/
    VOS_StopRelTimer(&pstRxinfo->fwCtrlTimer);

    /* MSP普通命令回复，按照CNF命令包组包*/
    ret = diag_FwCmdCnfProc(pstRxinfo,((DIAG_CMD_AGENT_TO_FW_CNF_STRU*)pData)->aucData,(ulLen-sizeof(DIAG_CMD_AGENT_TO_FW_CNF_STRU)));
    if(ERR_MSP_SUCCESS !=ret)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_APP_CNF_ENTRY_ERROR,ret,ulCmdId,1);
    }

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    if(MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId) == DIAG_CMD_HOST_DISCONNECT)
    {
        if(ERR_MSP_SUCCESS != DRV_SOCP_VOTE(SOCP_VOTE_DIAG_APP, SOCP_VOTE_FOR_SLEEP))
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DIS_CONN_CFG, 0, 1, 1);
        }
        g_diagSocpIsEnable = FALSE;
    }
#endif

    /* 命令处理完毕，删除节点*/
    diag_FwDelCmdNode(pstRxinfo->ulCmdId);

    return;
}

/*****************************************************************************
 Function Name   : diag_FwDealAgentCnfEntry
 Description     : 从DIAG AGENT收到的回复处理入口
 Input           :VOS_VOID* pData   ,VOS_UINT32 ulLen
 Output          : None
 Return          : VOS_VOID

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwDealAgentCnfEntry(VOS_UINT8* pData,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulCmdId =0;
    DIAG_FW_RX_INFO_STRU *pstRxinfo =NULL;
    VOS_UINT32 ret = ERR_MSP_FAILURE;

    if((NULL == pData)||(0 == ulLen))
    {
       return;
    }

    /*根据CMDID查找节点*/
    ulCmdId = ((DIAG_CMD_AGENT_TO_FW_CNF_STRU*)pData)->ulCmdId;

    pstRxinfo = diag_FwGetExeCmdNode(ulCmdId);
    if(DIAG_IS_PS_CMD(ulCmdId&0xffff))
    {
        /* 找不到节点，说明是IND命令*/
        if(NULL == pstRxinfo)
        {
            /* 按照IND命令包组包*/
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_AGENT_PS_IND_ENTRY, ulCmdId, ulLen, 1);
            ret = diag_FwAnsyCmdIndProc(((DIAG_CMD_AGENT_TO_FW_CNF_STRU*)pData)->aucData,(ulLen-sizeof(DIAG_CMD_AGENT_TO_FW_CNF_STRU)),ulCmdId);
            if(ERR_MSP_SUCCESS !=ret)
            {
                DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_AGENT_PS_IND_ENTRY_ERROR,ret,ulCmdId,1);
            }
            return;
        }

        /*删除定时器*/
        VOS_StopRelTimer(&pstRxinfo->fwCtrlTimer);

        /* PS透传命令回复，按照CNF命令包组包*/
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_AGENT_PS_CNF_ENTRY, ulCmdId, ulLen, 2);
        ret = diag_FwCmdCnfProc(pstRxinfo,((DIAG_CMD_AGENT_TO_FW_CNF_STRU*)pData)->aucData,(ulLen-sizeof(DIAG_CMD_AGENT_TO_FW_CNF_STRU)));
        if(ERR_MSP_SUCCESS !=ret)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_AGENT_PS_CNF_ENTRY_ERROR,ret,ulCmdId,1);
        }

        /* 命令处理完毕，删除节点*/
        diag_FwDelCmdNode(pstRxinfo->ulCmdId);
        return;
    }

    if(NULL == pstRxinfo)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_AGENT_CNF_ENTRY_ERROR, ulCmdId, ulLen, 0);
        return;
    }

    /*删除定时器*/
    VOS_StopRelTimer(&pstRxinfo->fwCtrlTimer);

    /* MSP普通命令回复，按照CNF命令包组包*/
    ret = diag_FwCmdCnfProc(pstRxinfo,((DIAG_CMD_AGENT_TO_FW_CNF_STRU*)pData)->aucData,(ulLen-sizeof(DIAG_CMD_AGENT_TO_FW_CNF_STRU)));
    if(ERR_MSP_SUCCESS !=ret)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_AGENT_CNF_ENTRY_ERROR,ret,ulCmdId,1);
    }

    /* 命令处理完毕，删除节点*/
    diag_FwDelCmdNode(pstRxinfo->ulCmdId);

    return;
}

/*****************************************************************************
 Function Name   : diag_FwGlobalInit
 Description     :  DIAG FW 用到的全局结构体初始化
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FwGlobalInit(VOS_VOID)
{
    DIAG_FW_RX_INFO_STRU* pHeadNode = NULL;
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    /* 获取链表头 */
    pHeadNode= diag_FwGetDiagNodeInfo();

    /* 初始化请求链表 */
    blist_head_init(&pHeadNode->HiDiagFwList);

    /* 创建节点保护信号量*/
    ret = VOS_SmBCreate(NULL, 1, VOS_SEMA4_FIFO,&g_DiagFwNodeSem);

    return ret;
}

/*****************************************************************************
 Function Name   : diag_FwSetChanSta
 Description     : 给M核发送连接状态，在USB拔除等情况使用到
 Input           :VOS_UINT32 flag
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-15  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwSetChanSta(VOS_UINT32 flag)
{

    /*如果当前要设置状态和现在状态一致，则不需改变*/
    if(((g_ulDiagCfgInfo&0x2)>>1)== flag)
    {
        return;
    }
    else
    {
        diag_CfgResetAllSwt();
#if (FEATURE_BSP_LCH_OM == FEATURE_ON)
        mdrv_om_set_hsoflag(0);
#endif
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
        (VOS_VOID)DRV_SOCP_VOTE(SOCP_VOTE_DIAG_APP, SOCP_VOTE_FOR_SLEEP);
        g_diagSocpIsEnable = FALSE;
#endif

        /*将状态发送给M核*/
        diag_SendMsg(MSP_PID_DIAG_FW,MSP_PID_DIAG_AGENT,ID_MSG_DIAG_HSO_DISCONN_IND,(VOS_UINT8*)&flag,sizeof(VOS_UINT32));
    }

    return;

}

/*****************************************************************************
 Function Name   : diag_FwMsgProcInit
 Description     : DIAG FW 消息处理初始化
 Input           :enum VOS_INIT_PHASE_DEFINE ip
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FwMsgProcInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    if (ip == VOS_IP_RESTART)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_INIT,ret,0,0);

        /*全局变量初始化*/
        ret = diag_FwGlobalInit();
        if(ret != ERR_MSP_SUCCESS)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_INIT_ERROR,ret,0,1);
            return ret;
        }

        /*DIAG SOCP BUF初始化*/
        ret = diag_BufCtrlGlobalInit();
        if(ret != ERR_MSP_SUCCESS)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_INIT_ERROR,ret,0,2);
            return ret;
        }

        /*注册给SCM的解码目的回调*/
        ret = SCM_RegDecoderDestProc(SOCP_DECODER_DST_LOM,(SCM_DECODERDESTFUCN)Diag_SocpDecodeDesDataProc);
        if(ret != ERR_MSP_SUCCESS)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_INIT_ERROR,ret,0,3);
            return ret;
        }

        /*设置DIAG初始化bit*/
        ret = diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);
        if(ret != ERR_MSP_SUCCESS)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_INIT_ERROR,ret,0,4);
            return ret;
        }
#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
        diag_PortSetConnStaCB((DIAG_PORT_CONNECT_STA_PFN)diag_FwSetChanSta );
#endif
    }

    return ret;
}

/*****************************************************************************
 Function Name   : diag_FwMsgProc
 Description     : DIAG FW接收到的消息处理入口
 Input           :MsgBlock* pMsgBlock
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_FwMsgProc(MsgBlock* pMsgBlock)
{
    REL_TIMER_MSG *pTimer =NULL;
    DIAG_DATA_MSG_STRU* pMsgTmp =NULL;
    /*lint --e{740, 142} */
    /*入参判断*/
    if (NULL == pMsgBlock)
    {
        return;
    }

    /*任务开始处理，不允许睡眠*/
	drx_msp_fid_vote_lock(DRX_DIAG_FW_VOTE);
    /*lint -save -e30*/
    /*根据发送PID，执行不同处理*/
    switch(pMsgBlock->ulSenderPid)
    {
        /*超时消息，按照超时包格式，打包回复*/
        case DOPRA_PID_TIMER:
            diag_printf("fw cmd timer out!\n");
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_TIME_OUT_ENTRY,0,0,0);
            pTimer   = (REL_TIMER_MSG*)pMsgBlock;
            diag_FwDealTimeOutCmdEntry(pTimer->ulPara);
            break;
#if 0
        /*处理HDLC解码后数据*/
        case MSP_PID_DIAG_FW:

            pMsgTmp = (DIAG_DATA_MSG_STRU*)pMsgBlock;
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_DECODE_CMD_ENTRY,0,pMsgTmp->ulLen,1);
            diag_FwDealDecodeCmdEntry(pMsgTmp->pContext,pMsgTmp->ulLen);
            break;
#endif
        /*处理A核发过来的回复*/
        case MSP_PID_DIAG_APP_AGENT:

            pMsgTmp = (DIAG_DATA_MSG_STRU*)pMsgBlock;
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_APP_CNF_ENTRY,0,pMsgTmp->ulLen,2);
            diag_FwDealAppAgentCnfEntry(pMsgTmp->pContext,pMsgTmp->ulLen);
            break;

        /*处理M核发过来的回复 */
        case MSP_PID_DIAG_AGENT:

            pMsgTmp = (DIAG_DATA_MSG_STRU*)pMsgBlock;
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_AGENT_CNF_ENTRY,0,pMsgTmp->ulLen,3);
            diag_FwDealAgentCnfEntry(pMsgTmp->pContext,pMsgTmp->ulLen);
            break;
#if (FEATURE_ON == FEATURE_MERGE_OM_CHAN)
        case WUEAPP_PID_MUX:
            pMsgTmp = (DIAG_DATA_MSG_STRU*)pMsgBlock;
            if(ID_MSG_DIAG_HSO_DISCONN_IND == pMsgTmp->ulMsgId)
            {
                diag_FwSetChanSta(0);
            }
            else
            {
                diag_printf(" rcv from WUEAPP_PID_MUX err,msg id :0x%x\n",pMsgTmp->ulMsgId);
            }
            break;
#endif
        /*接收到其他PID发过来的错误数据，不予处理 */
        default:
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_FW_ERROR_ENTRY,0,0,4);
            break;
    }
    /*lint -restore +e30*/
    /*任务处理完成，允许睡眠*/
	drx_msp_fid_vote_unlock(DRX_DIAG_FW_VOTE);

    return;
}

VOS_UINT32 diag_FwDecodeDataCheck(VOS_UINT8 * pDataBuf)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
	MSP_SOCP_HEAD_STRU   *pstSocpStru =NULL;
	MSP_DIAG_HEAD_STRU   *pstDiagStru =NULL;

    pstSocpStru = (MSP_SOCP_HEAD_STRU *)pDataBuf;
    pstDiagStru = (MSP_DIAG_HEAD_STRU *)(pDataBuf + sizeof(MSP_SOCP_HEAD_STRU));

    /*先判断SOCP头是否正确*/
    if(pstSocpStru->usSId != MSP_SOCP_SID_DIAG_SERVER)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, pstSocpStru->usSId, (VOS_UINT32)0, 3);
        return ERR_MSP_NOT_EXIST;
    }

    if(((pstSocpStru->usSSId)>>4 != MSP_SOCP_HIGH4BIT_SSID_MCPU)
    && ((pstSocpStru->usSSId)>>4 != MSP_SOCP_HIGH4BIT_SSID_LTE_DSP)
    && ((pstSocpStru->usSSId)>>4 != MSP_SOCP_HIGH4BIT_SSID_TDS_DSP))
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, (pstSocpStru->usSSId)>>4, (VOS_UINT32)0, 4);
        return ERR_MSP_NOT_SUPPORT;
    }

    /*再判断DIAG 头是否正确*/
    if(((pstDiagStru->ulID)>>28 != MSP_STRU_ID_28_31_GROUP_MSP)
    && ((pstDiagStru->ulID)>>28 != MSP_STRU_ID_28_31_GROUP_PS)
    && ((pstDiagStru->ulID)>>28 != MSP_STRU_ID_28_31_GROUP_LTE_DSP)
    && ((pstDiagStru->ulID)>>28 != MSP_STRU_ID_28_31_GROUP_TDS_DSP)
    && ((pstDiagStru->ulID)>>28 != MSP_STRU_ID_28_31_GROUP_BSP))
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, (pstDiagStru->ulID)>>28, (VOS_UINT32)0, 5);
        return ERR_MSP_INVALID_PARAMETER;
    }

    return ret;
}

/*****************************************************************************
 Function Name   : diag_FwSendDecodeCmdToFw
 Description     : 将解码数据发给DIAG FW自己处理
 Input           :VOS_UINT8 * pDataBuf
                VOS_UINT16 ulLen
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FwSendDecodeCmdToFw(VOS_UINT8 * pDataBuf, VOS_UINT32 ulLen)
{
    /*lint -save -e958*/
   VOS_UINT32 ret = ERR_MSP_SUCCESS;
    /*lint -restore*/
    /*参数检查，确认HSO下发数据是否正确*/
    ret = diag_FwDecodeDataCheck(pDataBuf);
    if(ret != ERR_MSP_SUCCESS)
    {
        diag_printf("diag_FwDecodeDataCheck ERROR!\n");
        return ret;
    }

    diag_FwDealDecodeCmdEntry(pDataBuf,ulLen);
#if 0
    /*发送给DIAG FW进行处理*/
    ret = diag_SendMsg(MSP_PID_DIAG_FW, MSP_PID_DIAG_FW,ID_MSG_DIAG_CMD_REQ_FW_TO_FW,pDataBuf, ulLen);
    if(ret != ERR_MSP_SUCCESS)
    {
       DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 6);
    }
#endif
    return ret;
}


/*****************************************************************************
 Function Name   : MSP_AppDiagFidInit
 Description     : A核DIAG 任务注册
 Input           :enum VOS_INIT_PHASE_DEFINE ip
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_UINT32 MSP_AppDiagFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ulRelVal = 0;

    switch (ip)
    {
        case VOS_IP_LOAD_CONFIG:
            drx_msp_init();
#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
            ulRelVal = diag_PortInit();
            if (ulRelVal != VOS_OK)
            {
                return VOS_ERR;
            }
#endif
            ulRelVal = VOS_RegisterPIDInfo(MSP_PID_DIAG_FW, (Init_Fun_Type) diag_FwMsgProcInit, (Msg_Fun_Type) diag_FwMsgProc);

            if (ulRelVal != VOS_OK)
            {
                return VOS_ERR;
            }

            ulRelVal = VOS_RegisterPIDInfo(MSP_PID_DIAG_APP_AGENT, (Init_Fun_Type) diag_AppAgentMsgProcInit, (Msg_Fun_Type) diag_AppAgentMsgProc);

            if (ulRelVal != VOS_OK)
            {
                return VOS_ERR;
            }

            ulRelVal = VOS_RegisterMsgTaskPrio(MSP_FID_DIAG_ACPU, VOS_PRIORITY_M2);
            if (ulRelVal != VOS_OK)
            {
                return VOS_ERR;
            }
#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
            ulRelVal = VOS_RegisterSelfTask(MSP_FID_DIAG_ACPU, (VOS_TASK_ENTRY_TYPE)diag_PortTask, VOS_PRIORITY_M2, 0x2000);    //lint !e64
            if (VOS_NULL_BYTE  == ulRelVal)
            {
                return VOS_ERR;
            }
#endif
            break;
        default:
            break;
    }

    return VOS_OK;
}
/*lint -restore +e40*/

/*****************************************************************************
 Function Name   : Diag_SocpDecodeDesDataProc
 Description     : HDLC解码目标通道数据处理函数。初始化时注册到SCM模块
 Input           :SOCP_DECODER_DST_ENUM_U32 enChanID
                VOS_UINT8 *pucData
                VOS_UINT32 ulSize
                VOS_UINT8 *pucRBData
                VOS_UINT32 ulRBSize
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
VOS_UINT32 Diag_SocpDecodeDesDataProc(SOCP_DECODER_DST_ENUM_U32 enChanID,VOS_UINT8 *pucData, VOS_UINT32 ulSize,VOS_UINT8 *pucRBData, VOS_UINT32 ulRBSize)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
    VOS_UINT32 ulTotalLen = 0;
    VOS_UINT8* pData;
    VOS_UINT8* pCurData;
    VOS_UINT32 ulCurLen = 0;
    DIAG_SOCP_DECODE_PACKET_HEAD* HeadInfo;


    /*入参检查*/
    if(pucData == VOS_NULL)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 0);
        return ERR_MSP_INVALID_PARAMETER;
    }

    ulTotalLen = ulSize + ulRBSize;
    if(!ulTotalLen)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 1);
        return ERR_MSP_INVALID_PARAMETER;
    }

    pData = VOS_MemAlloc(MSP_PID_DIAG_FW,DYNAMIC_MEM_PT,ulTotalLen);
    if(pData == VOS_NULL)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 2);
        return ERR_MSP_MALLOC_FAILUE;
    }

    VOS_MemCpy(pData,pucData,ulSize);

    /*回卷指针可能为空*/
    if((VOS_NULL != pucRBData)&&(0 != ulRBSize))
    {
        VOS_MemCpy(pData+ulSize,pucRBData,ulRBSize);
    }
    /*游标指针*/
    pCurData = pData;
    while(1)/*lint !e716*/
    {
        HeadInfo = (DIAG_SOCP_DECODE_PACKET_HEAD*)pCurData;

        /*没有HISI包头，认为无效包*/
        if( HeadInfo->ulMagic != DIAG_SOCP_DECODE_HEAD_MAGIC )
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, HeadInfo->ulDatalen, HeadInfo->ulMagic, 3);
            diag_printf("err packet head 0x%x ,0x%x!\n",HeadInfo->ulMagic,HeadInfo->ulDatalen);
            break;
        }
        /*剩余数据长度小于包头记录长度，认为无效包*/
        if( HeadInfo->ulDatalen >(ulTotalLen-ulCurLen-sizeof(DIAG_SOCP_DECODE_PACKET_HEAD)) )
        {
            diag_printf("%s enter here %d!\n",__func__,__LINE__);
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, HeadInfo->ulDatalen, ulCurLen, 4);
            break;
        }

        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE, enChanID, (VOS_UINT32)(HeadInfo->ulDatalen), 0);

        /*发送单个包*/
        ulRet = diag_FwSendDecodeCmdToFw(HeadInfo->data,HeadInfo->ulDatalen);
        if(ulRet)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, ulRet, 0, 5);
            break;
        }

        /*移动指针到下一个包*/
        ulCurLen += (sizeof(DIAG_SOCP_DECODE_PACKET_HEAD) + HeadInfo->ulDatalen);

        /*下一个包的起始长度加上包头长度不能超过数据总长度*/
        if((ulCurLen + sizeof(DIAG_SOCP_DECODE_PACKET_HEAD)) >= ulTotalLen)
        {
            break;
        }
        pCurData += ulCurLen;
    }

    VOS_MemFree(MSP_PID_DIAG_FW,pData);
    return ulRet;
}
#else
VOS_UINT32 Diag_SocpDecodeDesDataProc(SOCP_DECODER_DST_ENUM_U32 enChanID,VOS_UINT8 *pucData, VOS_UINT32 ulSize,VOS_UINT8 *pucRBData, VOS_UINT32 ulRBSize)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
    VOS_UINT32 ulTotalLen = 0;
    VOS_UINT8* pData;

    /*入参检查*/
    if(pucData == VOS_NULL)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 0);
        return ERR_MSP_INVALID_PARAMETER;
    }

    ulTotalLen = ulSize + ulRBSize;
    if(!ulTotalLen)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 1);
        return ERR_MSP_INVALID_PARAMETER;
    }

    pData = VOS_MemAlloc(MSP_PID_DIAG_FW,DYNAMIC_MEM_PT,ulTotalLen);
    if(pData == VOS_NULL)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 2);
        return ERR_MSP_MALLOC_FAILUE;
    }

    VOS_MemCpy(pData,pucData,ulSize);

    /*回卷指针可能为空*/
    if((VOS_NULL != pucRBData)&&(0 != ulRBSize))
    {
        VOS_MemCpy(pData+ulSize,pucRBData,ulRBSize);
    }

    DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE, enChanID, (VOS_UINT32)ulTotalLen, 0);

    ulRet = diag_FwSendDecodeCmdToFw(pData,ulTotalLen);
    if(ulRet)
    {
        diag_printf("diag_FwSendDecodeCmdToFw failed ,ulRet = 0x%x\n",ulRet);
    }

    VOS_MemFree(MSP_PID_DIAG_FW,pData);
    return ulRet;
}

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



