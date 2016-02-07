/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_app_agent.h
  Description     : diag_app_agent.c header file
  History         :
     1.w00182550       2012-12-5     Draft Enact
     2.
******************************************************************************/

#ifndef __DIAG_APP_AGENT_H__
#define __DIAG_APP_AGENT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#include  "mdrv.h"
#include  "diag_cmd_param_plugin.h"

#include  "msp_nvim.h"

#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/

#define NV_ITEM_MAXSIZE				 2048
#define MAX_HNV_LENGTH 				 0x80000
#define NVM_FLASH_BLOCK_SIZE 		(64*2048)
#define NVM_BACKUP_VERSION_FILE_BLOCK_NUM 4

/* PS 和GU透传命令字在结构的位置*/
#define DIAG_GUPS_TRANS_CMD_HEAD_LEN       (20+4)
#define DIAG_LPS_TRANS_CMD_HEAD_LEN        (20+4+12)
#define DIAG_PS_TRANS_HEAD_LEN             (20)

#define DIAG_PS_GU_MSGID_BASE            (0x11a1)
#define DIAG_PS_GU_MSGID_MAX             (0x11a3)
#define DIAG_PS_GU_CNF_MSGID_BASE        (0x1121)
#define DIAG_PS_GU_CNF_MSGID_MAX         (0x1123)

#define DIAG_IS_GUPS_MSG_ID(msgId)      (((msgId<=DIAG_PS_GU_MSGID_MAX) && (msgId >= DIAG_PS_GU_MSGID_BASE) \
                                        ||((msgId<=DIAG_PS_GU_CNF_MSGID_MAX) && (msgId >=DIAG_PS_GU_CNF_MSGID_BASE ))))

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/
typedef struct _DIAG_NV_BIN_LOAD_INFOR
{
    VOS_UINT32 ulExpectBlockIndex;/*期望HSO传入的块索引*/
    VOS_UINT32 ulCnfBlockIndex;   /*HSO期望传入的块索引*/
    VOS_UINT32 ulNvLen;           /*已写入长度*/
    VOS_UINT8 *pucBinData ;       /*用于储存写入数据*/
    HTIMER     NvLoadTimer;      /*TIMER*/
} DIAG_NV_BIN_LOAD_INFOR;

typedef struct
{
    VOS_UINT32  ulVoteId;
    VOS_UINT32  ulVoteType;
    VOS_UINT32  ulVoteTime;
} DIAG_SOCP_VOTE_INFO_STRU;

extern DRV_RESET_CB_MOMENT_E g_DiagResetingCcore;
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
extern VOS_UINT32 diag_PsTransProcEntry(VOS_UINT8* pstReq);
extern VOS_UINT32 diag_PsTransCnfProc(VOS_UINT8* pstCnf ,VOS_UINT32 ulLen);

extern VOS_UINT32 diag_AppAgentDispatchCmd(VOS_UINT8* pstReq );
extern VOS_VOID diag_AppAgentMsgProc(MsgBlock* pMsgBlock);
extern VOS_UINT32 diag_AppAgentMsgProcInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 diag_NvProcEntry (VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_NvRdProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_NvWrProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_LogSaveProc (VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_BOOL diag_AppAgentGetSleepFlag(VOS_VOID);
extern VOS_UINT32 diag_BspLogProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_BspSysviewProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_BspProcEntry(VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_BspUtraceProcEntry (VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);
extern VOS_INT diag_ResetCcoreCB(DRV_RESET_CB_MOMENT_E enParam, int userdata);

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

#endif /* end of diag_app_agent.h */
