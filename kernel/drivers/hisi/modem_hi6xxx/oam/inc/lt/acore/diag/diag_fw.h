/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_fw.h
  Description     : diag_fw.c header file
  History         :
     1.w00182550       2012-11-20     Draft Enact
     2.
******************************************************************************/

#ifndef __DIAG_FW_H__
#define __DIAG_FW_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#include  "blist.h"
#include  "diag_cmd_param_plugin.h"
#include  "mdrv.h"
#include "SOCPInterface.h"
#include <diag_message_id.h>
#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/



/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/
typedef struct _DIAG_FW_RX_INFO_STRU
{
    /*命令ID*/
    VOS_UINT32 ulCmdId;

    /*每个节点启一个TIMER，判断是否超时*/
    HTIMER fwCtrlTimer;

    /*指向下一个缓存节点命令数据*/
    LIST_S HiDiagFwList;

    /*HSO下发的完整SOCP包数据*/
    VOS_UINT8 ucDiagRcvData[0];
}DIAG_FW_RX_INFO_STRU;

#define DIAG_SOCP_DECODE_HEAD_MAGIC 0x48495349 /*HISI*/
typedef struct
{
    VOS_UINT32 ulMagic;
    VOS_UINT32 ulDatalen;
    VOS_UINT8  data[0];
}DIAG_SOCP_DECODE_PACKET_HEAD;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern DIAG_FW_RX_INFO_STRU* diag_FwAddCmdToList(VOS_UINT8 * pstReq, VOS_UINT32 ulRcvlen);
extern VOS_UINT32 diag_FwAnsyCmdIndProc(VOS_UINT8* pData,VOS_UINT32 ulLen,VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_FwCmdCnfProc(DIAG_FW_RX_INFO_STRU * pstRxinfo,VOS_UINT8* pData,VOS_UINT32 ulLen);
extern VOS_UINT32 diag_FwCmdTimeOutProc(VOS_UINT32 ulCmdId);
extern VOS_VOID diag_FwDealAgentCnfEntry(VOS_UINT8* pData,VOS_UINT32 ulLen);
extern VOS_VOID diag_FwDealAppAgentCnfEntry(VOS_UINT8* pData,VOS_UINT32 ulLen);
extern VOS_VOID diag_FwDealDecodeCmdEntry(VOS_UINT8* pData,VOS_UINT32 ulLen);
extern VOS_VOID diag_FwDealTimeOutCmdEntry(VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_FwDecodeDataCheck(VOS_UINT8 * pDataBuf);
extern VOS_VOID diag_FwDelCmdNode(VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_FwForwardCmdToAgent(DIAG_FW_RX_INFO_STRU * pNode,VOS_UINT32 ulLen);
extern DIAG_FW_RX_INFO_STRU* diag_FwGetDiagNodeInfo(VOS_VOID);
extern DIAG_FW_RX_INFO_STRU* diag_FwGetExeCmdNode(VOS_UINT32 ulCmdId);
extern VOS_UINT32 diag_FwGlobalInit(VOS_VOID);
extern VOS_VOID diag_FwMsgProc(MsgBlock* pMsgBlock);
extern VOS_UINT32 diag_FwMsgProcInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 diag_FwSendDecodeCmdToFw(VOS_UINT8 * pDataBuf, VOS_UINT32 ulLen);
extern VOS_VOID diag_FwSocpCnfPackt(DIAG_FW_RX_INFO_STRU * pstRxinfo, VOS_UINT8 * pstSocpPackt,VOS_UINT32 ulCmdParaLen);
extern VOS_VOID diag_FwTimeOutPackt(VOS_VOID * pstRxinfo, VOS_UINT8 * pstSocpPackt);
extern VOS_UINT32 Diag_SocpDecodeDesDataProc(SOCP_DECODER_DST_ENUM_U32 enChanID,VOS_UINT8 *pucData, VOS_UINT32 ulSize,VOS_UINT8 *pucRBData, VOS_UINT32 ulRBSize);
extern VOS_UINT32 MSP_AppDiagFidInit(enum VOS_INIT_PHASE_DEFINE ip);
VOS_VOID diag_FwSetChanSta(VOS_UINT32 flag);


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

#endif /* end of diag_fw.h */
