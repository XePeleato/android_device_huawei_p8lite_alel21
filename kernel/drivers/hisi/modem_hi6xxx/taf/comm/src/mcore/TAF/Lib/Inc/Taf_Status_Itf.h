#ifndef  __STA_TAFM_H__
#define  __STA_TAFM_H__

#include "vos.h"
#include "TafAppMma.h"

/*===========================================================================*/

/*****************************************************************************
*  Copyright (C), 2004-2005, Huawei Tech. Co., Ltd.                         *
*                                                                           *
*  FileName: sta_tafm.h                                                     *
*                                                                           *
*  Author:   Roger     Version :   1.00     Date:  2005.3.30                *
*                                                                           *
*  Description:      模块描述                                             *
*    MAPS1000 STATUS 模块TAFM外部调用接口说明文件                           *
*                                                                           *
*  Version:          版本信息                                             *
*    MAPS1000 V100R001                                                      *
*                                                                           *
*  Function List:    主要函数及其功能                                     *
*                                                                           *
*  History:          历史修改记录                                         *
*      <author>  <time>   <version >   <desc>                               *
*      Roger    050330     1.0     build this file                        *
*                                                                           *
   1.日    期  : 2006年12月4日
     作    者  : luojian id:60022475
     修改内容  : 增加 #pragma pack(4)，问题单号:A32D07779
*****************************************************************************/


/*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*=================================================================*/
#pragma pack(4)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*----------------------  常量定义 ----------------------*/
/*=======================================================*/
/*=======================================================*/


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*----------------  接口参数数据结构定义 ----------------*/
/*=======================================================*/
/*--------- Sta_PhoneStart 接口参数结构 -------*/


/*--------- Sta_SoftReset 接口参数结构 --------*/
#define     STA_PH_RESET_ALL        0   /* 重启整个模块 */
#define     STA_PH_RESET_FSM        1   /* 状态机复位 */
#define     STA_PH_RESET_FLAG       2   /* 状态机标志复位 */
#define     STA_PH_RESET_DATA       3   /* 其他全局数据复位 */
#define     STA_PH_RESET_INIT       4   /* 协议栈初始化调用 */

/*=======================================================*/


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*----------- STATUS 给TAFM提供的API接口函数 ------------*/
/*=======================================================*/


/*--------------- PLMN搜索过程  ---------------*/
VOS_UINT32 Sta_PlmnList ( VOS_UINT32 ulOpID );

/*------------- PLMN搜索过程终止 --------------*/
VOS_UINT32 Sta_PlmnListAbort ( VOS_UINT32 ulOpID );

/*------------ PLMN重选（自动模式）------------*/
VOS_UINT32 Sta_PlmnReselectAuto ( VOS_UINT32 ulOpID );

/*------------ PLMN重选（手动模式）------------*/
VOS_UINT32 Sta_PlmnReselectManual ( VOS_UINT32 ulOpID );

/*--------------- 选择指定的PLMN --------------*/
VOS_UINT32 Sta_PlmnSelect ( VOS_UINT32 ulOpID, TAF_PLMN_ID_STRU PlmnId, TAF_PH_RA_MODE AccessMode, VOS_UINT8 ucReselMode);

/*------ 向上层应用发送协议栈消息的接口 -------*/

/* Sta_MsgInd函数移动位置 */

 /*-------------- DETACH过程请求 --------------*/
VOS_UINT32 Sta_Detach ( VOS_UINT32 ulOpID, VOS_UINT32 uDetachCause, TAF_MMA_DETACH_CAUSE_ENUM_UINT8     enDetachCause);

 /*------------- ATTACH过程请求 ---------------*/

VOS_UINT32 Sta_Attach ( VOS_UINT32 ulOpID, VOS_UINT32 uAttatchType, TAF_MMA_EPS_ATTACH_REASON_ENUM_UINT8 enAttachReason);

 /*------- STATUS定时器超时事件处理过程 -------*/
VOS_UINT32 Sta_TimeoutProc ( REL_TIMER_MSG *pMsg );

 /*---------- STATUS模块软件复位函数 ----------*/
VOS_UINT32 Sta_SoftReset ( VOS_UINT8 ucResetMode );

/*=======================================================*/

/*=================================================================*/
#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

/*****************************************************************************/

/*===========================================================================*/
#endif      /* __STA_TAFM_H__*/

/***** End of the file *****/
