/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_port.c
  Description     :
  History         :
     1.y00228784       2013-7-25   Draft Enact

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
#include "diag_port.h"
#include "diag_common.h"
#include "msp_drx.h"
#include "SCMProc.h"
#include "diag_debug.h"
/*lint -restore*/

/*lint -save -e767 原因:Log打印*/
#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_PORT_C
/*lint -restore*/

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


DIAG_PORT_STAT_INFO_STRU    g_diagPort[EN_DIAG_PORT_BEARER_LAST];
DIAG_PORT_CTRL_INFO_STRU    g_diagPort_ctrl;
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
DIAG_PORT_DATA_BUF_STRU     g_diagPortDataBuf;
VOS_SEM g_diagPortSem = (VOS_SEM)0;
#endif
/*****************************************************************************
  3 Function
*****************************************************************************/

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
/*****************************************************************************
 Function Name   : diag_PortDataBufIsEmpty
 Description     : buffer链表是否为空
 Input           : VOS_VOID
 Output          : None
 Return          : 0:空， 0xFFFFFFFF:非空

 History         :
    1.z00212940      2014-2-21  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_PortDataBufIsEmpty(VOS_VOID)
{
    if((g_diagPortDataBuf.dataList.prev == &(g_diagPortDataBuf.dataList)) &&
        (g_diagPortDataBuf.dataList.next == &(g_diagPortDataBuf.dataList)))
    {
        return ERR_MSP_SUCCESS;
    }

    return ERR_MSP_FAILURE;
}

/*****************************************************************************
 Function Name   : diag_PortAddToDataBuf
 Description     : 添加数据节点到buffer链表
 Input           : DIAG_PORT_PHY_BEAR_ENUM enPort, VOS_UINT8 * data, VOS_UINT32 dataSize
 Output          : None
 Return          : VOS_VOID

 History         :
    1.z00212940      2014-2-21  Draft Enact

*****************************************************************************/
VOS_VOID diag_PortAddToDataBuf(DIAG_PORT_PHY_BEAR_ENUM enPort, VOS_UINT8 * data, VOS_UINT32 dataSize)
{
    DIAG_PORT_DATA_BUF_STRU * pNewNode = NULL;

    /*申请一个节点大小*/
    pNewNode = (DIAG_PORT_DATA_BUF_STRU *)VOS_MemAlloc(MSP_PID_DIAG_APP_AGENT, DYNAMIC_MEM_PT, sizeof(DIAG_PORT_DATA_BUF_STRU));
    if (NULL == pNewNode)
    {
        diag_printf("%s: alloc mem for new node fail\n", __FUNCTION__);
        return;
    }

    /*申请数据空间*/
    pNewNode->dataBuf = (VOS_UINT8 *)VOS_MemAlloc(MSP_PID_DIAG_APP_AGENT, DYNAMIC_MEM_PT, dataSize);
    if (NULL == pNewNode->dataBuf)
    {
        diag_printf("%s: alloc mem for data buf fail\n", __FUNCTION__);
        return;
    }
    pNewNode->port     = enPort;
    pNewNode->dataLen  = dataSize;
    VOS_MemCpy(pNewNode->dataBuf, data, dataSize);

    (VOS_VOID)VOS_SmP(g_diagPortSem,0);
    /* 插入节点到链表尾部 */
    blist_add_tail(&pNewNode->dataList, &g_diagPortDataBuf.dataList);
    (VOS_VOID)VOS_SmV(g_diagPortSem);
}

/*****************************************************************************
 Function Name   : diag_PortGetNodeFromDataBuf
 Description     : 从buffer链表获取一个数据节点
 Input           : None
 Output          : None
 Return          : DIAG_PORT_DATA_BUF_STRU *

 History         :
    1.z00212940      2014-2-21  Draft Enact

*****************************************************************************/
DIAG_PORT_DATA_BUF_STRU * diag_PortGetNodeFromDataBuf(VOS_VOID)
{
    DIAG_PORT_DATA_BUF_STRU * pNode = NULL;

    (VOS_VOID)VOS_SmP(g_diagPortSem,0);
    if(ERR_MSP_SUCCESS == diag_PortDataBufIsEmpty())
    {
        (VOS_VOID)VOS_SmV(g_diagPortSem);
        return NULL;
    }

    pNode = blist_entry(g_diagPortDataBuf.dataList.next, DIAG_PORT_DATA_BUF_STRU, dataList);
    blist_del(&pNode->dataList);
    (VOS_VOID)VOS_SmV(g_diagPortSem);

    return pNode;
}

/*****************************************************************************
 Function Name   : diag_PortSendDataToSocp
 Description     : 数据节点发送到SOCP
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.z00212940      2014-2-21  Draft Enact

*****************************************************************************/
VOS_VOID diag_PortSendDataToSocp(DIAG_PORT_DATA_BUF_STRU * dataNode)
{
    VOS_UINT32 ret;

    ret = SCM_SendDecoderSrc(DIAG_PORT_GET_DECODE_SRC(dataNode->port), dataNode->dataBuf, dataNode->dataLen);
    if(ret)
    {
        diag_printf("[%s]:SCM_SendDecoderSrc err 0x%x!!!!!!!!\n",__func__,ret);
    }
    VOS_MemFree(MSP_PID_DIAG_APP_AGENT, dataNode->dataBuf);
    VOS_MemFree(MSP_PID_DIAG_APP_AGENT, dataNode);
}
#endif


VOS_VOID diag_PortSetTraceSw(VOS_BOOL sw)
{
    g_diagPort_ctrl.ulTraceDebugSw = sw;
    return;
}

/*****************************************************************************
 函 数 名  : diag_PortSetConnStaCB
 功能描述  : 通道连接事件处理注册函数
 输入参数  : pfnReg: 回调函数指针

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
	 作    者  : heliping
	 修改内容  : Creat Function
*****************************************************************************/

VOS_VOID diag_PortSetConnStaCB(DIAG_PORT_CONNECT_STA_PFN pfnReg)
{
	if (NULL == pfnReg)
	{
		return ;
	}

	g_diagPort_ctrl.pFnConnCallBack = pfnReg;

	return ;
}

VOS_UINT32 diag_PhyWriteAsync(DIAG_PORT_PHY_BEAR_ENUM enPort,VOS_UINT8 *pucDataBuf,VOS_UINT32 ulLen)
{
    VOS_INT32 ret = ERR_MSP_SUCCESS;
    ACM_WR_ASYNC_INFO  stVcom  = {0};
    VOS_INT32 slHandle = 0;

    if(DIAG_PORT_GET_STATE(enPort) ==ACM_EVT_DEV_SUSPEND)
    {
    	SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(enPort),ulLen);
    	return ERR_MSP_SUCCESS;
    }

    slHandle = DIAG_PORT_GET_HANDLE(enPort);

    if(slHandle == UDI_INVALID_HANDLE)
    {
         SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(enPort),ulLen);
        return ERR_MSP_FAILURE;
    }

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    /* 工具已下发断开连接命令，丢数据 */
    if(!(g_ulDiagCfgInfo & (1 << 1)))
    {
        SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(enPort),ulLen);
        return ERR_MSP_SUCCESS;
    }
#endif

#ifdef FEATURE_UPGRADE_TL
    stVcom.pBuffer = (VOS_CHAR*)pucDataBuf;
#else
    stVcom.pVirAddr = (VOS_CHAR*)pucDataBuf;
    /*lint -save -e746 -e718*/
    stVcom.pPhyAddr = (VOS_CHAR*)SCM_CoderDestMemVirtToPhy(DIAG_PORT_GET_CODE_DES(enPort), pucDataBuf);
    /*lint -restore*/
#endif
    stVcom.u32Size = ulLen;

    DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_WRT+(VOS_UINT32)enPort), ulLen, 0, 0);

    ret = udi_ioctl((int)slHandle, ACM_IOCTL_WRITE_ASYNC, &stVcom);

    if(ret == ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_WRT_SUCC+(VOS_UINT32)enPort), ulLen, 0, 0);
        return ERR_MSP_SUCCESS;
    }

    SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(enPort),ulLen);
    return  (VOS_UINT32)ret;/* [false alarm]:屏蔽Fortify */
}

/*****************************************************************************
函 数 名  : writeAtData
功能描述  : DIAG写数据总入口
输入参数  : ucQueId: 数据类型
          aucDataBuf: 发送数据指针
          ulLen:发送长度
输出参数  :
返 回 值  : ERR_MSP_FAILURE/ERR_MSP_SUCCESS
调用函数  :
被调函数  : diag_WriteData
修改历史  :
1.日    期  : 2012年8月27日
  作    者  : heliping
  修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 diag_WriteData( VOS_UINT8* aucDataBuf, VOS_UINT32 enChanID,VOS_UINT32 ulLen)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32 usSentLen   = 0;

    diag_TraceDebug(aucDataBuf, ulLen);

	if (DIAG_PORT_GET_CONN_PORT() == EN_DIAG_SOCKET_BEARER_TCP_20248)
    {
		ret = diag_TcpSendData(EN_TCP_PORT_DIAG,aucDataBuf, ulLen,&usSentLen);
		SCM_RlsDestBuf(enChanID,ulLen);
        return ret;
    }
    else if (DIAG_PORT_GET_CONN_PORT() == EN_DIAG_USB_BEARER_DIAG_CTRL)
    {
        //回复和主动上报分开通道发送
        if(enChanID == SOCP_CODER_DST_LOM_CNF)
        {
        	return diag_PhyWriteAsync(EN_DIAG_USB_BEARER_DIAG_CTRL,(VOS_UINT8 *)aucDataBuf,(VOS_UINT32)ulLen);
        }
        else if(enChanID == SOCP_CODER_DST_LOM_IND)
        {
        	if( EN_DIAG_SEND_LOG_TO_USB == diag_GetLogSendType())
            {
                return diag_PhyWriteAsync(EN_DIAG_USB_BEARER_DIAG_APP,(VOS_UINT8 *)aucDataBuf,(VOS_UINT32)ulLen);
            }
            else
            {
                drx_msp_fid_vote_lock(DRX_DIAG_LOG_SAVE_VOTE);
                diag_StorDataToSD(aucDataBuf, ulLen);

                /* 该处需要直接read data done*/
                SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(EN_DIAG_USB_BEARER_DIAG_APP),ulLen);
                drx_msp_fid_vote_unlock(DRX_DIAG_LOG_SAVE_VOTE);
                return ERR_MSP_SUCCESS;
            }
        }
    }
	else if (DIAG_PORT_GET_CONN_PORT() == EN_DIAG_HSIC_BEARER_DIAG_CTRL)
    {
        //回复和主动上报分开通道发送
        if(enChanID == SOCP_CODER_DST_LOM_CNF)
        {
        	return diag_PhyWriteAsync(EN_DIAG_HSIC_BEARER_DIAG_CTRL,(VOS_UINT8 *)aucDataBuf,(VOS_UINT32)ulLen);
        }
        else if(enChanID == SOCP_CODER_DST_LOM_IND)
        {
        	return diag_PhyWriteAsync(EN_DIAG_HSIC_BEARER_DIAG_APP,(VOS_UINT8 *)aucDataBuf,(VOS_UINT32)ulLen);
        }
    }
    /*APP VCOM*/
	else if (DIAG_PORT_GET_CONN_PORT() == EN_DIAG_VCOM_BEABER_DIAG_CTRL)
    {
        //回复和主动上报分开通道发送
        if(enChanID == SOCP_CODER_DST_LOM_CNF)
        {
        	return diag_VcomWriteSync(EN_DIAG_VCOM_BEABER_DIAG_CTRL,DIAG_APPVCOM_CHAN_CTRL,\
                (VOS_UINT8 *)aucDataBuf,(VOS_UINT32)ulLen);

        }
        else if(enChanID == SOCP_CODER_DST_LOM_IND)
        {
        	return diag_VcomWriteSync(EN_DIAG_VCOM_BEABER_DIAG_APP,DIAG_APPVCOM_CHAN_APP,\
                (VOS_UINT8 *)aucDataBuf,(VOS_UINT32)ulLen);
        }
    }
    else
    {
        return ERR_MSP_FAILURE;
    }

    return ERR_MSP_SUCCESS;
}


//*****************************************************************************/
// 功能描述  :处理HDLC编码目标通道数据，初始化时注册到CDM模块
// 参数说明  : ulChanID:目标通道ID
//                         pBuf: 数据指针
//                        ulDataSize: 数据长度
//
// 返回值    :  ERR_MSP_SUCCESS   操作成功
//                       ERR_MSP_INVALID_PARAMETER    参数出错
//*****************************************************************************/
VOS_UINT32 diag_SocpCodeDesDataProc(SOCP_CODER_DST_ENUM_U32 enChanID,VOS_UINT8 *pucData, VOS_UINT8 *pucPHYData,VOS_UINT32 ulSize)
{
    VOS_UINT32 Ret;
    VOS_UINT32 send_len = 0;

    /*SCM 代码中没有限制发送最大长度*/
    if(DIAG_SOCP_SEND_SIZE_MAX < ulSize)
    {
        send_len = DIAG_SOCP_SEND_SIZE_MAX;
    }
    else
    {
        send_len = ulSize;
    }
    DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_CODE_PACKET_RCVE, enChanID, (VOS_UINT32)send_len, 0);

    if(pucData == NULL)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_CODE_PACKET_RCVE_ERROR, enChanID, (VOS_UINT32)send_len, 1);
        return ERR_MSP_INVALID_PARAMETER;
    }

    Ret = diag_WriteData(pucData,enChanID,send_len);
    if(ERR_MSP_SUCCESS != Ret)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_CODE_PACKET_RCVE_ERROR, enChanID, (VOS_UINT32)send_len, 3);
    }
    return Ret;

}


/*****************************************************************************
 函 数 名  : diag_PortEvtCB
 功能描述  : 通道连接事件处理注册函数
 输入参数  : pfnReg: 回调函数指针

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
	 作    者  : heliping
	 修改内容  : Creat Function
*****************************************************************************/

VOS_VOID diag_PortEvtCB(ACM_EVT_E  ulEvt,DIAG_PORT_PHY_BEAR_ENUM enPort)
{
    DIAG_PORT_CONNECT_STA_PFN pfnConn = DIAG_PORT_GET_CONN_CALLBACK();

    DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_EVT+(VOS_UINT32)enPort), ulEvt, 0, 0);

    if(ACM_EVT_DEV_READY==ulEvt)
    {
		DIAG_PORT_CHAN_STATE_SWITCH(enPort,ACM_EVT_DEV_READY);
        return ;
    }
    else
    {
        if(diag_GetLogSendType() == EN_DIAG_SEND_LOG_TO_SD)
        {
            return ;
        }

        //通知逻辑通道状态
        if(pfnConn!=NULL)
        {
            pfnConn(DIAG_DISCONN);
        }
		DIAG_PORT_CHAN_STATE_SWITCH(enPort,ACM_EVT_DEV_SUSPEND);
    }
    return;
}

/*****************************************************************************
函 数 名  : diag_PortWrtCB
功能描述  :
输入参数  :
输出参数  :
返 回 值  :
调用函数  :
被调函数  :
修改历史  :
	1.日	期	: 2012年8月27日
	 作    者  : heliping
	 修改内容  : Creat Function

*****************************************************************************/
VOS_VOID diag_PortWrtCB (DIAG_PORT_PHY_BEAR_ENUM enPort,VOS_CHAR* pDoneBuff, VOS_INT s32DoneSize)
{

    DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_WRT_CB+(VOS_UINT32)enPort),0, (VOS_UINT32)s32DoneSize, 0);

    if(s32DoneSize < 0)
    {

        DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_WRT_CB_ERR+(VOS_UINT32)enPort),0 , (VOS_UINT32)s32DoneSize, 0);

         //释放编码目标通道的buf
        SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(enPort),0);
        return ;
    }


    //释放编码目标通道的buf
    SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(enPort),(VOS_UINT32)s32DoneSize);

     return;
}

/*****************************************************************************
函 数 名  : diag_PortRdCB
功能描述  :
输入参数  :
输出参数  :
返 回 值  :
调用函数  :
被调函数  :
修改历史  :
	1.日	期	: 2012年8月27日
	 作    者  : heliping
	 修改内容  : Creat Function

*****************************************************************************/
VOS_VOID diag_PortRdCB(DIAG_PORT_PHY_BEAR_ENUM enPort)
{
    ACM_WR_ASYNC_INFO   acmInfo = {};
    UDI_HANDLE ulUdiHandle;
    VOS_UINT32 ret;
    VOS_UINT32 ulDataLen;
    VOS_CHAR*  pdata;
    static VOS_UINT32 ulTotalLen = 0;
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    DIAG_PORT_DATA_BUF_STRU * dataNode;
#endif

    ulUdiHandle = DIAG_PORT_GET_HANDLE(enPort);
    if(ulUdiHandle == UDI_INVALID_HANDLE)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_UDI_READ_END,0,0,1);
        diag_printf("[%s]:invalid handle!!!!!!!!\n",__func__);
        return;
    }
    ret = (VOS_UINT32)udi_ioctl(ulUdiHandle,UDI_ACM_IOCTL_GET_READ_BUFFER_CB,&acmInfo);
    if(ret != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_UDI_READ_END,0,0,2);
        diag_printf("[%s]:get read buffer callback err 0x%x!!!!!!!!\n",__func__,ret);
        return;
    }

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    /*端口切换，丢弃链表中数据*/
    if(DIAG_PORT_GET_CONN_PORT() != enPort)
    {
        for(;;)
        {
            dataNode = diag_PortGetNodeFromDataBuf();
            if(dataNode == NULL)
            {
                break;
            }
            VOS_MemFree(MSP_PID_DIAG_APP_AGENT, dataNode->dataBuf);
            VOS_MemFree(MSP_PID_DIAG_APP_AGENT, dataNode);
        }
    }
#endif
    ulDataLen = acmInfo.u32Size;
#ifdef FEATURE_UPGRADE_TL
    pdata = acmInfo.pBuffer;
#else
    pdata = acmInfo.pVirAddr;
#endif

    ulTotalLen += ulDataLen;

    DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_UDI_READ_START,ulTotalLen,ulDataLen,0);

    /*设置当前物理通道及通道连接状态*/
    DIAG_PORT_PORT_SWITCH(enPort);

    DIAG_PORT_CHAN_STATE_SWITCH(enPort,ACM_EVT_DEV_READY);
	DIAG_PORT_CHAN_STATE_SWITCH(enPort+1,ACM_EVT_DEV_READY);
    diag_PrintToFile(UsbCtrlRdCB_LogPath, pdata, ulDataLen);
    diag_TraceDebug(pdata, ulDataLen);

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    /*SOCP不可用或者缓存非空*/
    if((g_diagSocpIsEnable != TRUE) || (ERR_MSP_SUCCESS != diag_PortDataBufIsEmpty()))
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_UDI_READ_END,0,0,3);
        diag_PortAddToDataBuf(enPort, (VOS_UINT8 *)(pdata),ulDataLen);
        ret = (VOS_UINT32)udi_ioctl(ulUdiHandle,UDI_ACM_IOCTL_RETUR_BUFFER_CB,&acmInfo);
        if(ret)
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_UDI_READ_END,0,0,4);
            diag_printf("[%s]:return buffer to acm err 0x%x!!!!!!!!\n",__func__,ret);
        }
        if(ERR_MSP_SUCCESS != diag_SendMsg(MSP_PID_DIAG_APP_AGENT, MSP_PID_DIAG_APP_AGENT,
            ID_MSG_DIAG_CMD_PORT_REQ_TO_APP_AGENT, (VOS_UINT8 *)&ret, sizeof(VOS_UINT32)))
        {
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_UDI_READ_END,0,0,5);
            diag_printf("[%s]:send msg to diag app agent fail!!!!!!!!\n",__func__);
        }
        return;
    }
#endif

    ret = SCM_SendDecoderSrc(DIAG_PORT_GET_DECODE_SRC(enPort),(VOS_UINT8 *)(pdata),ulDataLen);
    if(ret)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_UDI_READ_END,0,0,6);
        diag_printf("[%s]:SCM_SendDecoderSrc err 0x%x!!!!!!!!\n",__func__,ret);
    }

    ret = (VOS_UINT32)udi_ioctl(ulUdiHandle,UDI_ACM_IOCTL_RETUR_BUFFER_CB,&acmInfo);
    if(ret)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_UDI_READ_END,0,0,7);
        diag_printf("[%s]:SCM_SendDecoderSrc err 0x%x!!!!!!!!\n",__func__,ret);
    }

    return;

}
/*****************************************************************************
函 数 名  : diag_PortClose
功能描述  :
输入参数  :
输出参数  :
返 回 值  :
调用函数  :
被调函数  :
修改历史  :
	1.日	期	: 2012年8月27日
	 作    者  : heliping
	 修改内容  : Creat Function

*****************************************************************************/
VOS_UINT32 diag_PortClose(DIAG_PORT_PHY_BEAR_ENUM enPort)
{
    VOS_INT32 slVcomRet = 0;
    VOS_INT32 slHandle = UDI_INVALID_HANDLE;

    slHandle = DIAG_PORT_GET_HANDLE(enPort);

    if(slHandle ==UDI_INVALID_HANDLE)
    {
        return ERR_MSP_SUCCESS;
    }

    DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_CLOSE+(VOS_UINT32)enPort), 0, 0, 0);

    slVcomRet = udi_close(slHandle);
    if (slVcomRet == ERR_MSP_SUCCESS)
    {
		DIAG_PORT_HANDLE_SWITCH(enPort,UDI_INVALID_HANDLE);
		DIAG_PORT_CHAN_STATE_SWITCH(enPort,ACM_EVT_DEV_SUSPEND);
        return (VOS_UINT32)slVcomRet;
    }

    DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_CLOSE_ERR+(VOS_UINT32)enPort), (VOS_UINT32)slVcomRet, 0, 0);

    return ERR_MSP_FAILURE;

}
/*****************************************************************************
函 数 名  : diag_PortOpen
功能描述  :
输入参数  :
输出参数  :
返 回 值  :
调用函数  :
被调函数  :
修改历史  :
	1.日	期	: 2012年8月27日
	 作    者  : heliping
	 修改内容  : Creat Function

*****************************************************************************/
VOS_UINT32 diag_PortOpen(DIAG_PORT_PHY_BEAR_ENUM enPort,UDI_ACM_DEV_TYPE_E devid,\
    VOS_VOID * pEvtCB,VOS_VOID* pWrtCB,VOS_VOID* pRdCB)
{

	UDI_HANDLE slUartHd  = 0;
	VOS_UINT32 ret		= ERR_MSP_SUCCESS;
	UDI_OPEN_PARAM  stParam;
    ACM_READ_BUFF_INFO  stReadParam;

	if(DIAG_PORT_GET_HANDLE(enPort) != UDI_INVALID_HANDLE)
	{
		return ERR_MSP_SUCCESS;
	}

	DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_OPEN+(VOS_UINT32)enPort), 0, 0, 0);
    /* coverity[mixed_enums] */
	stParam.devid = (UDI_DEVICE_ID)devid;
	slUartHd = udi_open(&stParam);

    if(UDI_INVALID_HANDLE == slUartHd)
    {
        return ERR_MSP_FAILURE;
    }

    DIAG_PORT_HANDLE_SWITCH(enPort,slUartHd);   /*修改端口句柄*/
    DIAG_PORT_CHAN_STATE_SWITCH(enPort,ACM_EVT_DEV_READY);/*打开之后修改端口状态*/

    stReadParam.u32BuffSize = DIAG_DATA_READ_BUFFER_SIZE;
    stReadParam.u32BuffNum  = DIAG_DATA_MAX_BUFFER_COUNT;

    ret = (VOS_UINT32)udi_ioctl(slUartHd,ACM_IOCTL_RELLOC_READ_BUFF,&stReadParam);
    if(ret)
    {
        DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_OPEN_ERR+(VOS_UINT32)enPort), (VOS_UINT32)slUartHd, ret, 1);
        goto ERR_OUT;
    }

    ret = (VOS_UINT32)udi_ioctl((int)slUartHd,ACM_IOCTL_SET_READ_CB, pRdCB);
    if(ret)
    {
        DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_OPEN_ERR+(VOS_UINT32)enPort), (VOS_UINT32)slUartHd, ret, 3);
        goto ERR_OUT;
    }
    ret = (VOS_UINT32)udi_ioctl((int)slUartHd,ACM_IOCTL_SET_WRITE_CB, pWrtCB);
    if(ret)
    {
        DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_OPEN_ERR+(VOS_UINT32)enPort), (VOS_UINT32)slUartHd, ret, 4);
        goto ERR_OUT;
    }
    ret = (VOS_UINT32)udi_ioctl((int)slUartHd, (int)ACM_IOCTL_SET_EVT_CB, pEvtCB);
    if(ret)
    {
        DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_OPEN_ERR+(VOS_UINT32)enPort) ,(VOS_UINT32)slUartHd, ret, 5);
        goto ERR_OUT;
    }

    ret = (VOS_UINT32)udi_ioctl((int)slUartHd, (int)ACM_IOCTL_WRITE_DO_COPY, 0);
    if(ret)
    {
        DIAG_DEBUG_SDM_FUN((DIAG_DEBUG_MSG_ID_ENUM)(EN_DIAG_DEBUG_TCP_OPEN_ERR+(VOS_UINT32)enPort), (VOS_UINT32)slUartHd, ret, 6);
        goto ERR_OUT;
    }
    return ERR_MSP_SUCCESS;
ERR_OUT:
    udi_close(slUartHd);

    DIAG_PORT_HANDLE_SWITCH(enPort,UDI_INVALID_HANDLE);   /*修改端口句柄*/
    DIAG_PORT_CHAN_STATE_SWITCH(enPort,ACM_EVT_DEV_SUSPEND);/*打开之后修改端口状态*/

    return ret;

}
#if 0
/*****************************************************************************
函 数 名  : diag_LogPortSwitch
功能描述  :
输入参数  :
输出参数  :
返 回 值  :
调用函数  :
被调函数  :
修改历史  :
	1.日	期	: 2012年8月27日
	 作    者  : heliping
	 修改内容  : Creat Function

*****************************************************************************/
VOS_UINT32 diag_LogPortSwitch(VOS_UINT32 ulLogPort)
{
    VOS_UINT32 ulDevChan;

    if(ulLogPort == DIAG_LOG_PORT_VCOM)
    {
        ulDevChan = EN_DIAG_VCOM_BEABER_DIAG_CTRL;
    }
    else if(ulLogPort == DIAG_LOG_PORT_USB)
    {
        ulDevChan = EN_DIAG_USB_BEARER_DIAG_CTRL;
    }
    else
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    if(DIAG_PORT_GET_CONN_PORT() != ulDevChan)
    {
        /*关闭当前端口，打开指定端口*/
        DIAG_PORT_PORT_SWITCH((DIAG_PORT_PHY_BEAR_ENUM)ulDevChan);
    }
    return ERR_MSP_SUCCESS;
}
/*****************************************************************************
函 数 名  : diag_getLogPort
功能描述  : 获取当前log 上报接口
输入参数  :
输出参数  :
返 回 值  :
调用函数  :
被调函数  :
修改历史  :
	1.日	期	: 2012年8月27日
	 作    者  : heliping
	 修改内容  : Creat Function

*****************************************************************************/
VOS_UINT32 diag_getLogPort(VOS_VOID)
{
    if(DIAG_PORT_GET_CONN_PORT() == EN_DIAG_VCOM_BEABER_DIAG_CTRL)
    {
        return DIAG_LOG_PORT_VCOM;
    }
    else if(DIAG_PORT_GET_CONN_PORT() == EN_DIAG_USB_BEARER_DIAG_CTRL)
    {
        return DIAG_LOG_PORT_USB;
    }
    else
    {
        diag_printf("[%s],channel: %d\n",__FUNCTION__,DIAG_PORT_GET_CONN_PORT());
        return ERR_MSP_FAILURE;
    }
}
#endif
/*****************************************************************************
 Function Name   : DMS_GetHsicPortStatus
 Description     :
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_UINT32 DMS_GetHsicPortStatus(VOS_VOID)
{
    return DIAG_PORT_GET_STATE(EN_DIAG_HSIC_BEARER_DIAG_CTRL) \
        & DIAG_PORT_GET_STATE(EN_DIAG_HSIC_BEARER_DIAG_APP);
}

/*****************************************************************************
 Function Name   : diag_PortCtrlInfo_Init
 Description     : 诊断通道初始信息初始化
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_PortCtrlInfo_Init(VOS_VOID)
{
    g_diagPort_ctrl.ancPort          = EN_DIAG_USB_BEARER_DIAG_CTRL;
    g_diagPort_ctrl.ucPortSleepFlag  = TRUE;
    g_diagPort_ctrl.pFnConnCallBack  = NULL;
    g_diagPort_ctrl.ulTraceDebugSw   = FALSE;
}
/*****************************************************************************
 Function Name   : diag_PortInit
 Description     : 诊断通道初始化
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-2-19  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_PortInit(VOS_VOID)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    diag_PortCtrlInfo_Init();

    /*USB init*/
    ret = diag_UsbInit();
    if(ret)
    {
        diag_printf("[%s]:usb init err!\n",__FUNCTION__);
    }

    /*HSIC init*/
    ret = diag_HiscInit();
    if(ret)
    {
        diag_printf("[%s]:hsic init err!\n",__FUNCTION__);
    }

    /*VCOM init*/
    ret = diag_VcomInit();
    if(ret)
    {
        diag_printf("[%s]:vcom init err!\n",__FUNCTION__);
    }

    /*TCP SOCKET init*/
    ret = diag_SocketInit();
    if(ret)
    {
        diag_printf("[%s]:socket init err!\n",__FUNCTION__);
    }

    /*SD init*/
    ret = diag_SdInit();
    if(ret)
    {
        diag_printf("[%s]:sd init err!\n",__FUNCTION__);
    }

    // 向CDM模块注册编码目标通道数据处理函数
    SCM_RegCoderDestProc(SOCP_CODER_DST_LOM_CNF,(SCM_CODERDESTFUCN)diag_SocpCodeDesDataProc);
    SCM_RegCoderDestProc(SOCP_CODER_DST_LOM_IND,(SCM_CODERDESTFUCN)diag_SocpCodeDesDataProc);

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    blist_head_init(&g_diagPortDataBuf.dataList);
    if(ERR_MSP_SUCCESS != VOS_SmBCreate(NULL, 1, VOS_SEMA4_FIFO, &g_diagPortSem))
    {
        diag_printf("[%s]:port sem init err!\n",__FUNCTION__);
    }
#endif
    return ERR_MSP_SUCCESS;
}
/*****************************************************************************
函 数 名  : diag_PortTask
功能描述  : 注册自处理任务
输入参数  :
输出参数  :
返 回 值  :
调用函数  :
被调函数  :
修改历史  :
*****************************************************************************/
VOS_VOID diag_PortTask(VOS_VOID)
{
    /*lint -save -e716 */
    /* coverity[no_escpe] */
    while(1)
    {
    /*lint -restore*/
        /*TO DO*/
        if(DIAG_PORT_GET_INIT(EN_DIAG_SOCKET_BEARER_TCP_20248) == EN_PORT_INIT_SUCC)
        {
            /* listen on TCP port to monitor incoming connections*/
            /*diag_BindListenTcpSocket();*/

            /* receive TCP data on sockets*/
            /*diag_AcceptRecvFromSocket();*/
            VOS_TaskDelay(10000);
        }
        else
        {
            VOS_TaskDelay(10000);
        }
        /* coverity[loop_bottom] */
    }
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


