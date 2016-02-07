/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AtCmdCallProc.h
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 2013年3月13日
  最近修改   :
  功能描述   : AtCmdCallProc.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年3月13日
    作    者   : 
    修改内容   : 创建文件

******************************************************************************/

#ifndef __ATCMDCALLPROC_H__
#define __ATCMDCALLPROC_H__

/*****************************************************************************
  1 其他头文件包含                            
*****************************************************************************/
#include "AtCtx.h"
#include "AtParse.h"
/* Added by l60609 for CDMA 1X Iteration 2, 2014-9-5, begin */
#include "AtMnInterface.h"
/* Added by l60609 for CDMA 1X Iteration 2, 2014-9-5, end */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义 
*****************************************************************************/

#define AT_CALL_D_GI_PARA_LEN                          (2)

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
#if (FEATURE_ON == FEATURE_ECALL)
VOS_UINT32 AT_SetCecallPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_QryCecallPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_SetEclstartPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_TestEclstartPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_SetEclstopPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_SetEclcfgPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_QryEclcfgPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_SetEclmsdPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_QryEclmsdPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_TestEclmsdPara(VOS_UINT8 ucIndex);
#endif

VOS_UINT32 At_RcvVcMsgDtmfDecoderIndProc(
    MN_AT_IND_EVT_STRU                 *pstData
);

/* Added by l60609 for CDMA 1X Iteration 2, 2014-9-5, begin */
VOS_UINT32 AT_SetHflashPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_RcvTafCallSndFlashCnf(
    MN_AT_IND_EVT_STRU                 *pEvtInfo
);
VOS_UINT32 AT_RcvTafCallSndFlashRslt(
    MN_AT_IND_EVT_STRU                 *pEvtInfo
);

/* Added by l60609 for CDMA 1X Iteration 2, 2014-9-5, end */



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

#endif /* end of AtCmdCallProc.h */
