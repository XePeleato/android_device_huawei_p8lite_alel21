/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_usb.c
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

/*****************************************************************************
 Function Name   : diag_UsbCtrlEvtCB
 Description     : USB控制通道事件回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_VOID

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_UsbCtrlEvtCB(ACM_EVT_E  ulEvt)
{
    diag_PortEvtCB(ulEvt,EN_DIAG_USB_BEARER_DIAG_CTRL);
}

/*****************************************************************************
 Function Name   : diag_UsbAppEvtCB
 Description     : USB数据通道事件回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_VOID

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_UsbAppEvtCB(ACM_EVT_E  ulEvt)
{
    diag_PortEvtCB(ulEvt,EN_DIAG_USB_BEARER_DIAG_APP);
}

/*****************************************************************************
 Function Name   : diag_UsbCtrlRdCB
 Description     : USB控制通道读回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_VOID

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_UsbCtrlRdCB(VOS_VOID)
{
    diag_PortRdCB(EN_DIAG_USB_BEARER_DIAG_CTRL);
}
/*****************************************************************************
 Function Name   : diag_UsbAppRdCB
 Description     : USB控制通道读回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_VOID

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_UsbAppRdCB(VOS_VOID)
{
    diag_PortRdCB(EN_DIAG_USB_BEARER_DIAG_APP);
}
/*****************************************************************************
 Function Name   : diag_UsbCtrlWrtCB
 Description     : USB控制通道写回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
#ifdef FEATURE_UPGRADE_TL
VOS_VOID diag_UsbCtrlWrtCB (VOS_CHAR* pDoneBuff,VOS_INT s32DoneSize)
#else
VOS_VOID diag_UsbCtrlWrtCB (VOS_CHAR* pDoneBuff, VOS_CHAR* phyAddr,VOS_INT s32DoneSize)
#endif
{
    diag_PortWrtCB(EN_DIAG_USB_BEARER_DIAG_CTRL,pDoneBuff,s32DoneSize);
}

/*****************************************************************************
 Function Name   : diag_UsbAppWrtCB
 Description     : USB数据通道写回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
#ifdef FEATURE_UPGRADE_TL
VOS_VOID diag_UsbAppWrtCB (VOS_CHAR* pDoneBuff, VOS_INT s32DoneSize)
#else
VOS_VOID diag_UsbAppWrtCB (VOS_CHAR* pDoneBuff,VOS_CHAR* phyAddr, VOS_INT s32DoneSize)
#endif
{
    diag_PortWrtCB(EN_DIAG_USB_BEARER_DIAG_APP,pDoneBuff,s32DoneSize);
}

/*****************************************************************************
 Function Name   : diag_UsbCtrlClose
 Description     : USB控制通道关闭回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_UsbCtrlClose(VOS_VOID)
{
	return diag_PortClose(EN_DIAG_USB_BEARER_DIAG_CTRL);
}

/*****************************************************************************
 Function Name   : diag_UsbAppClose
 Description     : USB数据上报通道关闭回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_UsbAppClose(VOS_VOID)
{
	return diag_PortClose(EN_DIAG_USB_BEARER_DIAG_APP);
}

/*****************************************************************************
 Function Name   : diag_UsbCtrlOpen
 Description     : USB控制通道打开回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_UsbCtrlOpen(VOS_VOID)
{
	return diag_PortOpen(EN_DIAG_USB_BEARER_DIAG_CTRL,\
                         (UDI_ACM_DEV_TYPE_E)UDI_USB_ACM_GPS,\
                         diag_UsbCtrlEvtCB,\
                         diag_UsbCtrlWrtCB,\
                         diag_UsbCtrlRdCB);

}

/*****************************************************************************
 Function Name   : diag_UsbAppOpen
 Description     : USB数据上报通道打开回调
 Input           : VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.y00228784      2013-2-19  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_UsbAppOpen(VOS_VOID)
{
	return diag_PortOpen(EN_DIAG_USB_BEARER_DIAG_APP,\
                         (UDI_ACM_DEV_TYPE_E)UDI_USB_ACM_LTE_DIAG,\
                         diag_UsbAppEvtCB,\
                         diag_UsbAppWrtCB,\
                         diag_UsbAppRdCB);
}

/*****************************************************************************
 Function Name   : diag_UsbOpen
 Description     : 打开通道
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-2-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_UsbOpen(VOS_VOID)
{
	VOS_UINT32 ulRet = 0;

	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_INIT, 0, 0, 0);

	/*打开 DIAG USB CTRL CNF通道*/
	ulRet = diag_UsbCtrlOpen();
	if(ERR_MSP_SUCCESS!=ulRet)
	{
		vos_printf("diag_UsbCtrlOpen failed.\n");
		DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_INIT_ERR,0, 0, 1);
	}

	/*打开 DIAG USB DATA IND通道*/
	ulRet = diag_UsbAppOpen();
	if(ERR_MSP_SUCCESS!=ulRet)
	{
		vos_printf("diag_UsbAppOpen failed.\n");
		DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_INIT_ERR,0, 0, 2);
	}
}

/*****************************************************************************
 Function Name   : diag_UsbClose
 Description     : 关闭通道
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-2-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_UsbClose(VOS_VOID)
{
	VOS_UINT32 ulRet = 0;

	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_DISABLE,0, 0, 0);

	/*关闭DIAG USB CTRL通道*/
	ulRet = diag_UsbCtrlClose();
	if(ERR_MSP_SUCCESS!=ulRet)
	{
		DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_DISABLE_ERR,0, 0, 1);
	}

	/*关闭DIAG USB DATA通道*/
	ulRet = diag_UsbAppClose();
	if(ERR_MSP_SUCCESS!=ulRet)
	{
		DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_DISABLE_ERR,0, 0, 2);
	}
}

/*****************************************************************************
 Function Name   : diag_UsbInfo_Init
 Description     :
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-2-19  Draft Enact

*****************************************************************************/
VOS_VOID diag_UsbInfo_Init(VOS_VOID)
{
	/*USB CTRL*/
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_CTRL].ucChanStat        = ACM_EVT_DEV_SUSPEND;
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_CTRL].ucHdlcFlag        = EN_HDLC_DATA;
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_CTRL].ulCodeDesChanId   = SOCP_CODER_DST_LOM_CNF;
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_CTRL].ulDecodeSrcChanId = SOCP_DECODER_SRC_LOM;
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_CTRL].ulRecivBufLen     = 0;
    g_diagPort[EN_DIAG_USB_BEARER_DIAG_CTRL].slPortHandle      = UDI_INVALID_HANDLE;

	/*USB IND*/
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_APP].ucChanStat         = ACM_EVT_DEV_SUSPEND;
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_APP].ucHdlcFlag         = EN_HDLC_DATA;
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_APP].ulCodeDesChanId    = SOCP_CODER_DST_LOM_IND;
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_APP].ulDecodeSrcChanId  = 0;
	g_diagPort[EN_DIAG_USB_BEARER_DIAG_APP].ulRecivBufLen      = 0;
    g_diagPort[EN_DIAG_USB_BEARER_DIAG_APP].slPortHandle       = UDI_INVALID_HANDLE;

}


/*****************************************************************************
 Function Name   : diag_UsbInit
 Description     :
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-2-19  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_UsbInit(VOS_VOID)
{
    /*global info init*/
    diag_UsbInfo_Init();

	/* 注册USB通道打开与关闭回调*/
	DRV_USB_REGUDI_ENABLECB(diag_UsbOpen);
    DRV_USB_REGUDI_DISABLECB(diag_UsbClose);
    DIAG_PORT_INIT_STATE_SWITCH(EN_DIAG_USB_BEARER_DIAG_CTRL,EN_PORT_INIT_SUCC);
    DIAG_PORT_INIT_STATE_SWITCH(EN_DIAG_USB_BEARER_DIAG_APP,EN_PORT_INIT_SUCC);
    return ERR_MSP_SUCCESS;
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




