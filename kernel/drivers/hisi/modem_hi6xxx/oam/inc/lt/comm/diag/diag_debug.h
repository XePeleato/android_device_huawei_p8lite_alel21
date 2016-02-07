/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_Debug.h
  Description     : diag_Debug.h header file
  History         :
     1.w00182550       2012-11-19     Draft Enact
     2.
******************************************************************************/

#ifndef __DIAG_DEBUG_H__
#define __DIAG_DEBUG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#include  <diag_buf_ctrl.h>

#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/

#define DIAG_DEBUG_SDM_FUN(enType,ulRserved1,ulRserved2,ulRserved3)  \
diag_Debug(enType,ulRserved1,ulRserved2,ulRserved3)

#define DIAG_PRINT_INFO
#ifdef DIAG_PRINT_INFO
#define diag_printf( fmt...) do { \
	vos_printf(fmt); \
	} while (0)
#else
#define diag_printf( fmt...)
#endif

#define DIAG_DEBUG_API               (0)
#define DIAG_DEBUG_API_AIR           (1)
#define DIAG_DEBUG_API_LAYER         (2)
#define DIAG_DEBUG_API_USERPLANE     (3)
#define DIAG_DEBUG_API_EVENT         (4)
#define DIAG_DEBUG_API_REPORT_INFO   (5)
#define DIAG_DEBUG_API_PRINTF        (6)
#define DIAG_DEBUG_API_PRINT         (7)
#define DIAG_DEBUG_VOLTE             (8)


#define DECODER_DEST_READCB_LOGPATH "/modem_log/DecoderDestReadCB.txt"
#define USB_CTRL_RDCB_LOGPATH       "/modem_log/UsbCtrlRdCB.txt"

extern VOS_CHAR DecoderDestReadCB_LogPath[sizeof(DECODER_DEST_READCB_LOGPATH)];
extern VOS_CHAR UsbCtrlRdCB_LogPath[sizeof(USB_CTRL_RDCB_LOGPATH)];
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
typedef enum
{
     /*hdlc编解码部分SDM*/
/*00*/    EN_DIAG_DEBUG_CODE_PACKET_START,
/*01*/    EN_DIAG_DEBUG_CODE_PACKET_START_ERROR,
/*00*/    EN_DIAG_DEBUG_CODE_PACKET_RCVE,
/*00*/    EN_DIAG_DEBUG_CODE_PACKET_RCVE_ERROR,

/*00*/    EN_DIAG_DEBUG_DECODE_PACKET_START,
/*00*/    EN_DIAG_DEBUG_DECODE_PACKET_START_ERROR,
/*00*/    EN_DIAG_DEBUG_DECODE_PACKET_RCVE,
/*01*/    EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR,

/*02*/    EN_DIAG_DEBUG_GET_SRC_BUF_START,
/*03*/    EN_DIAG_DEBUG_GET_SRC_BUF_START_ERROR,

    /*diag fw部分SDM*/
/*06*/    EN_DIAG_DEBUG_FW_INIT,
/*07*/    EN_DIAG_DEBUG_FW_INIT_ERROR,
		   EN_DIAG_DEBUG_FW_LIST_CMD,
/*08*/    EN_DIAG_DEBUG_FW_DECODE_CMD_ENTRY,
/*09*/    EN_DIAG_DEBUG_FW_DECODE_CMD_ENTRY_ERROR,
/*10*/    EN_DIAG_DEBUG_FW_TIME_OUT_ENTRY,
/*11*/    EN_DIAG_DEBUG_FW_TIME_OUT_ENTRY_ERROR,
/*12*/    EN_DIAG_DEBUG_FW_APP_CNF_ENTRY,
/*13*/    EN_DIAG_DEBUG_FW_APP_CNF_ENTRY_ERROR,
/*14*/    EN_DIAG_DEBUG_FW_AGENT_CNF_ENTRY,
/*15*/    EN_DIAG_DEBUG_FW_AGENT_CNF_ENTRY_ERROR,
/*16*/    EN_DIAG_DEBUG_FW_AGENT_PS_CNF_ENTRY,
/*17*/    EN_DIAG_DEBUG_FW_AGENT_PS_CNF_ENTRY_ERROR,
/*18*/    EN_DIAG_DEBUG_FW_AGENT_PS_IND_ENTRY,
/*19*/    EN_DIAG_DEBUG_FW_AGENT_PS_IND_ENTRY_ERROR,
/*20*/    EN_DIAG_DEBUG_FW_ERROR_ENTRY,
/*21*/    EN_DIAG_DEBUG_FW_ERROR_ENTRY_ERROR,

    /*diag api部分SDM*/
/*22*/    EN_SDM_DIAG_PRINTFV,
/*23*/    EN_SDM_DIAG_PRINTFV_ERR,
/*24*/    EN_SDM_DIAG_ReportEventLog,
/*25*/    EN_SDM_DIAG_ReportEventLog_ERR,
/*26*/    EN_SDM_DIAG_ReportAirMessageLog,
/*27*/    EN_SDM_DIAG_ReportAirMessageLog_ERR,
/*28*/    EN_SDM_DIAG_ReportVoLTELog_ERR,
/*29*/    EN_SDM_DIAG_ReportLayerMessageLog,
/*30*/    EN_SDM_DIAG_ReportLayerMessageLog_ERR,
/*31*/    EN_SDM_DIAG_ReportUserPlaneMessageLog,
/*32*/    EN_SDM_DIAG_ReportUserPlaneMessageLog_ERR,
/*33*/    EN_SDM_DIAG_ReportStatLog,
/*34*/    EN_SDM_DIAG_ReportStatLog_ERR,
          EN_SDM_DIAG_DOT,
          EN_SDM_DIAG_DOT_ERR,

    /*diag 开关设置部分SDM*/
/*34*/    EN_DIAG_DEBUG_MSG,
/*35*/    EN_DIAG_DEBUG_MSG_ERR,
/*36*/    EN_DIAG_DEBUG_CONN_CFG,
/*37*/    EN_DIAG_DEBUG_DIS_CONN_CFG,
/*38*/    EN_DIAG_DEBUG_LAYER_CFG,
/*39*/    EN_DIAG_DEBUG_PRINT_CFG,
/*40*/    EN_DIAG_DEBUG_EVENT_CFG,
/*41*/    EN_DIAG_DEBUG_AIR_CFG,
		   EN_DIAG_DEBUG_USEPLANE_CFG,


     /*diag app agent部分SDM*/
/*42*/    EN_DIAG_DEBUG_APP_AGENT_TIME_OUT_ENTRY,
/*43*/    EN_DIAG_DEBUG_APP_AGENT_DISPATCH_CMD,
/*44*/   EN_DIAG_DEBUG_NV_RD,
		  EN_DIAG_DEBUG_NV_WR,
		  EN_DIAG_DEBUG_NV_IMPORT,
		  EN_DIAG_DEBUG_NV_EXPORT,
		  EN_DIAG_DEBUG_LOG_SAVE,


     /*fs 操作部分SDM*/
          EN_DIAG_DEBUG_FS_SCAN_DIR,
          EN_DIAG_DEBUG_FS_SCAN_DIR_ERROR,

     /*diag agent部分SDM*/
/*45*/    EN_DIAG_DEBUG_AGENT_INIT,
/*46*/    EN_DIAG_DEBUG_AGENT_INIT_ERROR,
/*47*/    EN_DIAG_DEBUG_AGENT_DISPATCH_CMD,
		  EN_DIAG_DEBUG_REG_RD,
		  EN_DIAG_DEBUG_REG_WR,
		  EN_DIAG_DEBUG_BBP_LOG,
		  EN_DIAG_DEBUG_BBP_SAMPLE,
		  EN_DIAG_DEBUG_PS_TRANS_REQ,
		  EN_DIAG_DEBUG_PS_TRANS_CNF,
		  EN_DIAG_DEBUG_LTE_DSP_CNF,
		  EN_DIAG_DEBUG_TDS_DSP_CNF,

		 EN_DIAG_DEBUG_VCOM_INIT,
		 EN_DIAG_DEBUG_VCOM_INIT_ERR,
		 EN_DIAG_DEBUG_VCOM_DISABLE,
		 EN_DIAG_DEBUG_VCOM_DISABLE_ERR,
		 EN_DIAG_DEBUG_TCP_OPEN,
		 EN_DIAG_DEBUG_VCOM1_OPEN,
		 EN_DIAG_DEBUG_VCOM2_OPEN,
		 EN_DIAG_DEBUG_HSIC1_OPEN,
		 EN_DIAG_DEBUG_HSIC2_OPEN,
		 EN_DIAG_DEBUG_TCP_OPEN_ERR,
		 EN_DIAG_DEBUG_VCOM1_OPEN_ERR,
		 EN_DIAG_DEBUG_VCOM2_OPEN_ERR,
	 	 EN_DIAG_DEBUG_HSIC1_OPEN_ERR,
		 EN_DIAG_DEBUG_HSIC2_OPEN_ERR,
		 EN_DIAG_DEBUG_TCP_CLOSE,
		 EN_DIAG_DEBUG_VCOM1_CLOSE,
		 EN_DIAG_DEBUG_VCOM2_CLOSE,
		 EN_DIAG_DEBUG_HSIC1_CLOSE,
		 EN_DIAG_DEBUG_HSIC2_CLOSE,
		 EN_DIAG_DEBUG_TCP_CLOSE_ERR,
		 EN_DIAG_DEBUG_VCOM1_CLOSE_ERR,
		 EN_DIAG_DEBUG_VCOM2_CLOSE_ERR,
		 EN_DIAG_DEBUG_HSIC1_CLOSE_ERR,
	 	 EN_DIAG_DEBUG_HSIC2_CLOSE_ERR,
	 	 EN_DIAG_DEBUG_APPVCOM_READ_ERR,
	 	 EN_DIAG_DEBUG_APPVCOM_SEND_ERR,
	 	 EN_DIAG_DEBUG_TCP_EVT,
		 EN_DIAG_DEBUG_VCOM1_EVT,
		 EN_DIAG_DEBUG_VCOM2_EVT,
		 EN_DIAG_DEBUG_HSIC1_EVT,
		 EN_DIAG_DEBUG_HSIC2_EVT,
		 EN_DIAG_DEBUG_TCP_WRT,
		 EN_DIAG_DEBUG_VCOM1_WRT,
		 EN_DIAG_DEBUG_VCOM2_WRT,
		 EN_DIAG_DEBUG_HSIC1_WRT,
		 EN_DIAG_DEBUG_HSIC2_WRT,

	 	EN_DIAG_DEBUG_TCP_WRT_SUCC,
	 	 EN_DIAG_DEBUG_VCOM1_WRT_SUCC,
		 EN_DIAG_DEBUG_VCOM2_WRT_SUCC,
		 EN_DIAG_DEBUG_HSIC1_WRT_SUCC,
		 EN_DIAG_DEBUG_HSIC2_WRT_SUCC,

	 	EN_DIAG_DEBUG_TCP_WRT_CB,
		 EN_DIAG_DEBUG_VCOM1_WRT_CB,
		 EN_DIAG_DEBUG_VCOM2_WRT_CB,
		  EN_DIAG_DEBUG_HSIC1_WRT_CB,
		 EN_DIAG_DEBUG_HSIC2_WRT_CB,

	 	EN_DIAG_DEBUG_TCP_WRT_CB_ERR,
		 EN_DIAG_DEBUG_VCOM1_WRT_CB_ERR,
		 EN_DIAG_DEBUG_VCOM2_WRT_CB_ERR,
		 EN_DIAG_DEBUG_HSIC1_WRT_CB_ERR,
		EN_DIAG_DEBUG_HSIC2_WRT_CB_ERR,

	 	EN_DIAG_DEBUG_UDI_READ_START,
	 	EN_DIAG_DEBUG_UDI_READ_END,


		 EN_DIAG_TCP_SOCKET_ACP,
		 EN_DIAG_TCP_SOCKET_CLOSE,
		 EN_DIAG_TCP_SOCKET_CLOSE_FAIL,
		 EN_DIAG_TCP_SOCKET_RECV,
		 EN_DIAG_TCP_SOCKET_RECV_ERROR,
		 EN_DIAG_TCP_SOCKET_SEND,
		 EN_DIAG_TCP_SOCKET_SEND_ERROR,
		 EN_DIAG_TCP_TCP_RECONNECT,
		 EN_DIAG_TCP_TCP_RECONNECT_ERR,

/*47*/   EN_DIAG_DEBUG_BBP_AGENT_TIME_OUT_ENTRY,
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
        /* DIAG APP SOCP投票 */
         EN_DIAG_APP_SOCP_VOTE,
         EN_DIAG_APP_SOCP_VOTE_ERR,
#endif
         /* DIAG AGENT与LDSP邮箱交互 */
         EN_DIAG_AGENT_LDSP_MB_MSG,
/*47*/   EN_DIAG_DEBUG_INFO_MAX
} DIAG_DEBUG_MSG_ID_ENUM;

/*****************************************************************************
   5 STRUCT
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulCalledNum;  /* 调用次数或者消息次数,或者表明该函数是否被调用*/
    VOS_UINT32 ulRserved1;
    VOS_UINT32 ulRserved2;
    VOS_UINT32 ulRserved3;
    VOS_UINT32 ulRtcTime;
} DIAG_DEBUG_INFO_TBL_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID Debug_DiagApi(VOS_UINT32 ulApiType);
extern VOS_VOID Debug_DiagFw(VOS_VOID);
extern VOS_VOID diag_Debug(DIAG_DEBUG_MSG_ID_ENUM ulType,
                            VOS_UINT32 ulRserved1, VOS_UINT32 ulRserved2, VOS_UINT32 ulRserved3);
extern DIAG_DEBUG_INFO_TBL_STRU* diag_DebugGetInfo(VOS_VOID);
VOS_VOID diag_PrintToFile(VOS_CHAR* logpath, VOS_CHAR* pdata, VOS_UINT len);
VOS_VOID Debug_DiagCreateFile(VOS_VOID);
VOS_VOID Debug_DiagDspMb(VOS_VOID);

/*****************************************************************************
  9 OTHERS
*****************************************************************************/










#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of diag_Debug.h */
