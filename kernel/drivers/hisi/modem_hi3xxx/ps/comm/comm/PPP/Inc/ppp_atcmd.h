/************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : ppp_atcmd.h
  Author       : ---
  Version      : V200R001
  Date         : 2005-11-18
  Description  : 该头文件为AT模块"创建PPP链路"、"释放PPP链路"、
                  "PPP模块接收网侧指示的配置信息"提供的对应API函数提供
                  引用性说明。

  Function List:
        ---
        ---
        ---
  History      :
  1. Date:2005-11-18
     Author: ---fanzhibin 49086
     Modification:Create
  2.日    期   : 2006年08月09日
    作    者   : 蒋丽萍j60010247
    修改内容   : 问题单A32D03479，在PC机上实现时将#pragma pack(0)和#pragma pack()加编译开关
 ************************************************************************/

#include "vos.h"
#include "PppInterface.h"

#pragma pack(4)

extern VOS_UINT32 PPP_ProcTeConfigInfo (VOS_UINT16 usPppId, PPP_REQ_CONFIG_INFO_STRU *pPppReqConfigInfo);
extern VOS_UINT32 PPP_ProcPppRelEvent (VOS_UINT16 usPppId);
extern VOS_UINT32 PPP_ProcPppDisconnEvent (VOS_UINT16 usPppId);
extern VOS_UINT32 Ppp_SndPPPDisconnIndtoAT(VOS_UINT16 usPppId);
#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

