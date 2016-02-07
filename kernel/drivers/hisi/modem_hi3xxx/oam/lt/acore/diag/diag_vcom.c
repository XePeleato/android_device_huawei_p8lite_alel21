/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_vcom.c
  Description     :
  History         :
     1.y00228784       2013-7-25   Draft Enact

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "diag_port.h"
#include "diag_common.h"

/*lint -save -e767 原因:Log打印*/
#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_VCOM_C
/*lint -restore*/

VOS_VOID diag_ShowVcomStatus(VOS_VOID);
VOS_VOID diag_VcomCtrlEvtCB(APP_VCOM_EVT_UINT32 event);
VOS_VOID diag_VcomAppEvtCB(APP_VCOM_EVT_UINT32 event);
VOS_UINT32 diag_VcomRead(VOS_UINT8 ucDevIndex, VOS_UINT8 *pData, VOS_UINT32 uslength);

/* CTRL、APP端口状态,0:ctrl,1:app */
static APP_VCOM_EVT_UINT32 g_diagVcomState[2];

static VOS_VOID diag_VcomInfo_Init(VOS_VOID)
{
	/*VCOM CTRL*/
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_CTRL].ucChanStat        = ACM_EVT_DEV_SUSPEND;
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_CTRL].ucHdlcFlag        = EN_HDLC_DATA;
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_CTRL].ulCodeDesChanId   = SOCP_CODER_DST_LOM_CNF;
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_CTRL].ulDecodeSrcChanId = SOCP_DECODER_SRC_LOM;
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_CTRL].ulRecivBufLen     = 0;
    g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_CTRL].slPortHandle      = UDI_INVALID_HANDLE;

	/*VCOM IND*/
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_APP].ucChanStat         = ACM_EVT_DEV_SUSPEND;
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_APP].ucHdlcFlag         = EN_HDLC_DATA;
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_APP].ulCodeDesChanId    = SOCP_CODER_DST_LOM_IND;
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_APP].ulDecodeSrcChanId  = 0;
	g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_APP].ulRecivBufLen      = 0;
    g_diagPort[EN_DIAG_VCOM_BEABER_DIAG_APP].slPortHandle       = UDI_INVALID_HANDLE;

    g_diagVcomState[0] = APP_VCOM_EVT_RELEASE;  /* CTRL */
    g_diagVcomState[1] = APP_VCOM_EVT_RELEASE;  /* APP */
}

/*lint -save -e958*/
VOS_UINT32 diag_VcomWriteSync(DIAG_PORT_PHY_BEAR_ENUM enPort,
                                 APP_VCOM_DEV_INDEX_UINT8 enDevIndex,
                                 VOS_UINT8 *pucDataBuf,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;

    diag_TraceDebug(pucDataBuf, ulLen);

    if(DIAG_PORT_GET_STATE(enPort) == ACM_EVT_DEV_READY)
    {
        ulRet = APP_VCOM_Send(enDevIndex,pucDataBuf,ulLen);
        SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(enPort),ulLen);
    }
    else
    {
        diag_printf("[%s],enport :%d,status :%d\n",__FUNCTION__,enPort,DIAG_PORT_GET_STATE(enPort));
        SCM_RlsDestBuf(DIAG_PORT_GET_CODE_DES(enPort),ulLen);
        return ERR_MSP_FAILURE;
    }
    return ulRet;
}
/*lint -restore*/

VOS_VOID diag_ShowVcomStatus(VOS_VOID)
{
    diag_printf("**********VCOM STATUS**********\n");
    diag_printf("VCOM CTRL: %s\n", (g_diagVcomState[0] == APP_VCOM_EVT_RELEASE) ? "CLOSED" : "OPEN");
    diag_printf("VCOM APP : %s\n", (g_diagVcomState[1] == APP_VCOM_EVT_RELEASE) ? "CLOSED" : "OPEN");
}

VOS_VOID diag_VcomCtrlEvtCB(APP_VCOM_EVT_UINT32 event)
{
    DIAG_PORT_CONNECT_STA_PFN pfnConn = DIAG_PORT_GET_CONN_CALLBACK();

    g_diagVcomState[0] = event;
 
    if(APP_VCOM_EVT_OPEN == event)
    {
		DIAG_PORT_CHAN_STATE_SWITCH(EN_DIAG_VCOM_BEABER_DIAG_CTRL, ACM_EVT_DEV_READY);
        return;
    }
    else
    {
		DIAG_PORT_CHAN_STATE_SWITCH(EN_DIAG_VCOM_BEABER_DIAG_CTRL, ACM_EVT_DEV_SUSPEND);
        if((g_diagVcomState[0] != APP_VCOM_EVT_RELEASE) || (g_diagVcomState[1] != APP_VCOM_EVT_RELEASE))
        {
            return;
        }
        //通知逻辑通道状态
        if(pfnConn!=NULL)
        {
            pfnConn(DIAG_DISCONN);
        }
    }
}

VOS_VOID diag_VcomAppEvtCB(APP_VCOM_EVT_UINT32 event)
{
    DIAG_PORT_CONNECT_STA_PFN pfnConn = DIAG_PORT_GET_CONN_CALLBACK();

    g_diagVcomState[1] = event;
 
    if(APP_VCOM_EVT_OPEN == event)
    {
		DIAG_PORT_CHAN_STATE_SWITCH(EN_DIAG_VCOM_BEABER_DIAG_APP, ACM_EVT_DEV_READY);
        return;
    }
    else
    {
		DIAG_PORT_CHAN_STATE_SWITCH(EN_DIAG_VCOM_BEABER_DIAG_APP, ACM_EVT_DEV_SUSPEND);
        if((g_diagVcomState[0] != APP_VCOM_EVT_RELEASE) || (g_diagVcomState[1] != APP_VCOM_EVT_RELEASE))
        {
            return;
        }
        //通知逻辑通道状态
        if(pfnConn!=NULL)
        {
            pfnConn(DIAG_DISCONN);
        }
    }
}


/*****************************************************************************
 Function Name   : diag_VcomRead
 Description     : 注册给TAF的回调函数，用于读取VCOM口下发的数据
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.Y00228784      2013-7-25  Draft Enact

*****************************************************************************/
/*lint -save -e958*/
VOS_UINT32 diag_VcomRead(VOS_UINT8 ucDevIndex, VOS_UINT8 *pData, VOS_UINT32 uslength)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    DIAG_PORT_DATA_BUF_STRU * dataNode;
#endif    
    /*lint -restore*/
    if((ucDevIndex != DIAG_APPVCOM_CHAN_CTRL)||(NULL == pData)||(0 == uslength))
    {
        diag_printf("[%s]:INVALID PARAMETER ! ucDevIndex :0x%x,uslength :0x%x\n",__FUNCTION__,ucDevIndex,uslength);
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_APPVCOM_READ_ERR,0, uslength, 1);
        return ERR_MSP_INVALID_PARAMETER;
    }

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    /*端口切换，丢弃链表中数据*/
    if(DIAG_PORT_GET_CONN_PORT() != EN_DIAG_VCOM_BEABER_DIAG_CTRL)
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

    /*设置当前物理通道*/
    DIAG_PORT_PORT_SWITCH(EN_DIAG_VCOM_BEABER_DIAG_CTRL);
    diag_TraceDebug(pData, uslength);

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    /*SOCP不可用或者缓存非空*/
    if((g_diagSocpIsEnable != TRUE) || (ERR_MSP_SUCCESS != diag_PortDataBufIsEmpty()))
    {
        diag_PortAddToDataBuf(EN_DIAG_VCOM_BEABER_DIAG_CTRL, (VOS_UINT8 *)pData, uslength);
        if(ERR_MSP_SUCCESS != diag_SendMsg(MSP_PID_DIAG_APP_AGENT, MSP_PID_DIAG_APP_AGENT, 
            ID_MSG_DIAG_CMD_PORT_REQ_TO_APP_AGENT, (VOS_UINT8 *)&ulRet, sizeof(VOS_UINT32)))
        {
            diag_printf("[%s]:send msg to diag app agent fail!!!!!!!!\n",__func__);
        }
        return ERR_MSP_SUCCESS;
    }
#endif

    ulRet = SCM_SendDecoderSrc(SOCP_DECODER_SRC_LOM,pData,uslength);
    if(ulRet != ERR_MSP_SUCCESS)
    {
        diag_printf("[%s]:SCM_SendDecoderSrc fail ! ulRet :0x%x\n",__FUNCTION__,ulRet);
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_APPVCOM_READ_ERR,ulRet, 0, 2);
        return ulRet;
    }
    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 Function Name   : diag_VcomInit
 Description     : HISC通道初始化
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.Y00228784      2013-7-25  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_VcomInit(VOS_VOID)
{
    diag_VcomInfo_Init();
#if (FEATURE_ON == FEATURE_VCOM_EXT)
    /*注册APP VCOM读回调*/
    APP_VCOM_RegDataCallback(DIAG_APPVCOM_CHAN_CTRL, (SEND_UL_AT_FUNC)diag_VcomRead);
    /*CTRL口事件回调*/
    APP_VCOM_RegEvtCallback(DIAG_APPVCOM_CHAN_CTRL, (EVENT_FUNC)diag_VcomCtrlEvtCB);
    /*APP口事件回调*/
    APP_VCOM_RegEvtCallback(DIAG_APPVCOM_CHAN_APP, (EVENT_FUNC)diag_VcomAppEvtCB);
    DIAG_PORT_INIT_STATE_SWITCH(EN_DIAG_VCOM_BEABER_DIAG_CTRL,EN_PORT_INIT_SUCC);
    DIAG_PORT_INIT_STATE_SWITCH(EN_DIAG_VCOM_BEABER_DIAG_APP,EN_PORT_INIT_SUCC);
#endif

    return ERR_MSP_SUCCESS;

}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




