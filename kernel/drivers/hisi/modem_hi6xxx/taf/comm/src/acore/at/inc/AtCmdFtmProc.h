/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AtCmdFtmProc.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2013年3月13日
  最近修改   :
  功能描述   : AtCmdFtmProc.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年3月13日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/

#ifndef __ATCMDFTMPROC_H__
#define __ATCMDFTMPROC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "AtCtx.h"
#include "AtParse.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* LOG3.5的端口定义: USB */
#define AT_LOG_PORT_USB                 (0)

/* LOG3.5的端口定义: VCOM */
#define AT_LOG_PORT_VCOM                (1)

#define AT_JAM_DETECT_DEFAULT_METHOD                        (2)
#define AT_JAM_DETECT_DEFAULT_THRESHOLD                     (10)
#define AT_JAM_DETECT_DEFAULT_FREQ_NUM                      (30)




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

VOS_UINT32 At_SetLogPortPara(VOS_UINT8 ucIndex);

VOS_UINT32 At_QryLogPortPara(VOS_UINT8 ucIndex);

VOS_UINT32 At_SetDpdtTestFlagPara(TAF_UINT8 ucIndex);
VOS_UINT32 At_SetDpdtPara(TAF_UINT8 ucIndex);
VOS_UINT32 At_SetQryDpdtPara(TAF_UINT8 ucIndex);
VOS_UINT32 AT_RcvMtaSetDpdtTestFlagCnf(VOS_VOID *pMsg);
VOS_UINT32 AT_RcvMtaSetDpdtValueCnf(VOS_VOID *pMsg);
VOS_UINT32 AT_RcvMtaQryDpdtValueCnf(VOS_VOID *pMsg);



VOS_UINT32 At_SetJamDetectPara(VOS_UINT8 ucIndex);
VOS_UINT32 At_QryJamDetectPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_RcvMtaSetJamDetectCnf(
    VOS_VOID                           *pMsg
);
VOS_UINT32 AT_RcvMtaJamDetectInd(
    VOS_VOID                           *pMsg
);

VOS_UINT32 AT_SetRatFreqLock(VOS_UINT8 ucIndex);

VOS_UINT32 AT_QryRatFreqLock(VOS_UINT8 ucIndex);

VOS_UINT32 AT_RcvMtaSetRatFreqLockCnf(
    VOS_VOID                           *pMsg
);

VOS_UINT32 AT_RcvMtaSetGFreqLockCnf(
    VOS_VOID                           *pMsg
);
VOS_UINT32 AT_SetGFreqLock(VOS_UINT8 ucIndex);




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

#endif /* end of AtCmdFtmProc.h */
