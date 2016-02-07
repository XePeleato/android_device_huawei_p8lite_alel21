/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AtCmdSimProc.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2013年3月13日
  最近修改   :
  功能描述   : AtCmdSimProc.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年3月13日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/

#ifndef __ATCMDSIMROC_H__
#define __ATCMDSIMPROC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "AtCtx.h"
#include "AtParse.h"
#include "ATCmdProc.h"
#include "TafNvInterface.h"
#include "siapppih.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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
extern VOS_UINT32 At_Base16Decode(VOS_CHAR *pcData, VOS_UINT32 ulDataLen, VOS_UINT8* pucDst);
extern VOS_UINT16 At_Hex2Base16(VOS_UINT32 MaxLength,VOS_CHAR *headaddr,VOS_CHAR *pucDst,VOS_UINT8 *pucSrc,VOS_UINT16 usSrcLen);

#if (FEATURE_ON == FEATURE_VSIM)
extern VOS_UINT32 At_SetHvsDHPara(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_QryHvsDHPara(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_TestHvsDHPara(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_QryHvsContPara(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_SetHvsstPara(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_QryHvsstPara(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_TestHvsstPara(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_DealRSFWVsim(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_SetHvpDHPara(VOS_UINT8 ucIndex);
extern VOS_UINT32 At_TestHvpDHPara(VOS_UINT8 ucIndex);
extern VOS_UINT16 At_HvsstQueryCnf(SI_PIH_EVENT_INFO_STRU *pstEvent);
extern VOS_UINT16 At_HvsContQueryCnf(SI_PIH_EVENT_INFO_STRU *pstEvent);
extern VOS_UINT16 AT_HvsDHQueryCnf(SI_PIH_EVENT_INFO_STRU *pstEvent);
#endif

extern VOS_UINT16 AT_UiccAuthCnf(TAF_UINT8 ucIndex,SI_PIH_EVENT_INFO_STRU *pstEvent);
extern VOS_UINT16 AT_UiccAccessFileCnf(TAF_UINT8 ucIndex,SI_PIH_EVENT_INFO_STRU *pstEvent);



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

#endif /* end of AtCmdSimProc.h */
