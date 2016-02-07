/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : OmOperator.h
  版 本 号   : 初稿
  作    者   : 甘兰47350
  生成日期   : 2009年10月27日

  最近修改   :
  功能描述   : OmOperator.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2009年10月27日
    作    者   : 甘兰47350
    修改内容   : 创建文件

******************************************************************************/

#ifndef __OM_OPERATOR_H__
#define __OM_OPERATOR_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "vos.h"
#include "omprivate.h"

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum LMT_RAT_MODE
{
    LMT_RAT_WCDMA = 0,
    LMT_RAT_GSM,
    LMT_RAT_BUTT
};

enum OM_MODEM_MODE
{
    OM_MODEM_TYPE_0 = 0,
    OM_MODEM_TYPE_1,
    OM_MODEM_TYPE_2,
    OM_MODEM_BUTT
};

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID     OM_RfMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);
extern VOS_UINT32   MMA_GetOperatorLockPwd(VOS_UINT8 *pucPwd);
extern VOS_UINT32   AT_ResumeOm2AtMode(VOS_VOID);
extern VOS_UINT32   MMA_VerifyOperatorLockPwd(VOS_UINT8 *pucPwd);


extern VOS_UINT32 OM_ReadNv(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 OM_WriteNv(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 OM_SysCtrlCmd(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel, VOS_UINT16 usClientId, VOS_UINT32 ulMode, MODEM_ID_ENUM_UINT16 enModemID);

extern VOS_UINT32 OM_SetFTMReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 OM_QueryModemNumReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,VOS_UINT16 usReturnPrimId);

extern VOS_VOID   OM_RfMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);

extern VOS_VOID   OM_MsgProc(MsgBlock* pMsg);

extern VOS_VOID   OM_ConfigMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of OmOpeator.h */

