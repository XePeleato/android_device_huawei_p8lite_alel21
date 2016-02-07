/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_agent.h
  Description     : diag_agent.c header file
  History         :
     1.w00182550       2012-12-10     Draft Enact
     2.
******************************************************************************/

#ifndef __DIAG_AGENT_H__
#define __DIAG_AGENT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "vos.h"
#include "diag_cmd_param_plugin.h"
#include <diag_message_id.h>
#include "msp_diag.h"
#include "diag_common.h"

#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/

/* 该长度是写入RTT的要去掉的字节长度*/
#define DRA_RTT_NOT_NEED_CHARS_LEN            sizeof(MSP_DIAG_DATA_REQ_STRU)
#define DIAG_PS_TRANS_HEAD_LEN                (20)


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/
/* DSP对DIAG发送邮箱消息的回复 */
typedef struct
{
    VOS_UINT32  ulMsgId;
    VOS_UINT32  ulOpId;
    VOS_UINT32  ulCnf;
} DIAG_MSG_FROM_DSP_STRU;

/* 发给DSP的SOCP状态变化请求 */
typedef struct
{
    VOS_UINT32  ulMsgId;
    VOS_UINT32  ulOpId;
    VOS_UINT32  ulReq;      /* 0:SOCP不可用，1:SOCP可用 */
} DIAG_MSG_TO_DSP_STRU;

typedef struct
{
    VOS_MSG_HEADER
    DIAG_MSG_FROM_DSP_STRU  ulMsgData;
} DIAG_MSG_DSP_CNF_TO_AGENT_STRU;

enum DIAG_SOCP_STATE_ENUM
{
    SOCP_DISABLE,       /* SOCP不可用 */
    SOCP_ENABLE,        /* SOCP可用 */
    SOCP_STATE_BUTT
};
typedef unsigned int DIAG_SOCP_STATE_ENUM_U32;

enum DIAG_LDSP_STATE_ENUM
{
    LDSP_NOT_INIT,      /* 未初始化 */
    LDSP_INITING,       /* 正在初始化 */
    LDSP_INITED,        /* 初始化完成 */
    LDSP_SOCP_ENABLE,   /* DIAG已通知LDSP SOCP可用 */
    LDSP_SOCP_DISABLE,  /* DIAG已通知LDSP SOCP不可用 */
    LDSP_STATE_BUTT
};
typedef unsigned int DIAG_LDSP_STATE_ENUM_U32;

typedef struct
{
    VOS_UINT32  ulSendOpId;
    VOS_UINT32  ulRevOpId;
} DIAG_LDSP_DEBUG_INFO;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/
extern VOS_UINT32 diag_DspTransCnfProc(VOS_UINT8 * aucSocpPacket);
extern VOS_UINT32 diag_AgentDispatchCmd(VOS_UINT8* pstReq);
extern VOS_VOID diag_AgentMsgProc(MsgBlock* pMsgBlock);
extern VOS_UINT32 diag_AgentMsgProcInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_VOID diag_AgentTimeOutProcEntry(VOS_VOID* pstPara);
extern VOS_UINT32 diag_BbpProcEntry(VOS_UINT8* pstReq ,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_DrxDataSampleProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_DrxLogSampleProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_DspTransProcEntry(VOS_UINT8* pstReq ,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_DrxSampleGenProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_DrxSampleGetAddrProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_DrxSampleGetChnSizeProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_DrxSampleGetVersionProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_DrxSampleAbleChnProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_RegRdProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_RegWrProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_SetRegValue(DIAG_CMD_REG_WR_PARA_STRU * pWrtPara);
extern VOS_UINT32 MSP_DiagFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 diag_GtrProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_BspLogProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_BspSysviewProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_BspProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_VOID diag_EnableSocpChan(VOS_VOID);
VOS_VOID diag_InitSocpChan(VOS_VOID);
VOS_VOID diag_AgentPsTransRcv(MsgBlock* pMsgBlock);
VOS_VOID diag_ShowDspInfo(VOS_VOID);
VOS_VOID diag_AgentSendMsgToDsp(DIAG_SOCP_STATE_ENUM_U32 state);
VOS_VOID diag_MailboxCb(MAILBOX_INT_TYPE_E enIntType);
VOS_VOID diag_PortDisconnMsgProc(VOS_VOID);
VOS_VOID diag_AgentConnectCmdProc(DIAG_CONNECT_CMD_ENUM_U32 connCmd);
VOS_VOID diag_MailboxSelfTask(VOS_VOID);




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

#endif /* end of diag_agent.h */
