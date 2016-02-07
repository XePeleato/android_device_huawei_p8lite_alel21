/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : SCMSoftDecode.h
  版 本 号      : 初稿
  作    者      : H59254
  生成日期      : 2014年5月31日
  最近修改      :
  功能描述      : 该头文件为SOCP API的头文件，主要提供给外部部件使用
  函数列表      :
  修改历史      :
  1.日    期    : 2012年8月3日
    作    者    : zhuli
    修改内容    : 创建文件

******************************************************************************/
#ifndef _SCM_SOFTDECODE_H_
#define _SCM_SOFTDECODE_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omringbuffer.h"
#include "OmHdlcInterface.h"

//#include "SCMProc.h"



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/**************************************************************************
  2 宏定义
**************************************************************************/
#define SCM_DATA_RCV_BUFFER_SIZE    (100*1024)      /* SCM数据接收buffer大小 */
#define SCM_DATA_RCV_PKT_SIZE       (8*1024)        /* SCM数据接收PKT大小 */


/**************************************************************************
  3 枚举定义
**************************************************************************/

/**************************************************************************
  4 结构体定义
**************************************************************************/

/*****************************************************************************
结构名    : SCM_DATE_RCV_CTRL_STRU
结构说明  : SCM数据接收的控制结构
*****************************************************************************/
typedef struct
{
    VOS_SEM                     SmID;
    OM_RING_ID                  rngOmRbufId;
    VOS_CHAR                   *pucBuffer;
}SCM_DATA_RCV_CTRL_STRU;

#if 0
/*****************************************************************************
结构名    : SCM_DATA_RCV_HEADER_STRU
结构说明  : SCM数据接收时放入环形缓冲区的数据头
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  ulLen;
}SCM_DATA_RCV_HEADER_STRU;
#endif

/*****************************************************************************
结构名    : SCM_MAINTENANCE_INFO_STRU
结构说明  : SCM软解码模块可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulDataLen; /* 接收或发送数据长度 */
    VOS_UINT32                          ulNum;     /* 接收或发送数据次数 */
}SCM_MAINTENANCE_INFO_STRU;

typedef struct
{
    VOS_UINT32                          ulSemCreatErr;
    VOS_UINT32                          ulSemGiveErr;
    VOS_UINT32                          ulRingBufferCreatErr;
    VOS_UINT32                          ulTaskIdErr;
    VOS_UINT32                          ulBufferNotEnough;
    VOS_UINT32                          ulRingBufferFlush;
    VOS_UINT32                          ulRingBufferPutErr;
}SCM_SOFTDECODE_RB_INFO_STRU;


/*****************************************************************************
结构名    : OM_ACPU_PC_UE_SUC_STRU
结构说明  : OM模块正常接收可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    SCM_SOFTDECODE_RB_INFO_STRU         stRbInfo;
    SCM_MAINTENANCE_INFO_STRU           stPutInfo;
    SCM_MAINTENANCE_INFO_STRU           stGetInfo;
    SCM_MAINTENANCE_INFO_STRU           stHdlcDecapData;
    VOS_UINT32                          ulFrameDecapErr;
    VOS_UINT32                          ulHdlcInitErr;
    VOS_UINT32                          ulDataTypeErr;
    VOS_UINT32                          ulCpmRegLogicRcvSuc;
}SCM_SOFTDECODE_INFO_STRU;

/**************************************************************************
  6 函数定义
**************************************************************************/

#if ((OSA_CPU_ACPU == VOS_OSA_CPU)|| (VOS_WIN32 == VOS_OS_VER))

VOS_UINT32 SCM_SoftDecodeCfgDataRcv(VOS_UINT8 *pucBuffer, VOS_UINT32 ulLen);

VOS_UINT32 SCM_SoftDecodeDataRcv(VOS_UINT8 *pucBuffer, VOS_UINT32 ulLen);

VOS_UINT32 SCM_SoftDecodeAcpuRcvData(
    OM_HDLC_STRU                       *pstHdlcCtrl,
    VOS_UINT8                          *pucData,
    VOS_UINT32                          ulLen);

VOS_UINT32 CBTSCM_SoftDecodeAcpuRcvData(
    OM_HDLC_STRU                       *pstHdlcCtrl,
    VOS_UINT8                          *pucData,
    VOS_UINT32                          ulLen);

VOS_UINT32 SCM_SoftDecodeCfgRcvTaskInit(VOS_VOID);

VOS_UINT32 SCM_SoftDecodeCfgHdlcInit(OM_HDLC_STRU *pstHdlc);

VOS_UINT32 CBTSCM_SoftDecodeReqRcvTaskInit(VOS_VOID);

VOS_VOID   SCM_SoftDecodeCfgRcvSelfTask(
    VOS_UINT32                          ulPara1,
    VOS_UINT32                          ulPara2,
    VOS_UINT32                          ulPara3,
    VOS_UINT32                          ulPara4);

VOS_VOID   CBTSCM_SoftDecodeReqRcvSelfTask(
    VOS_UINT32                          ulPara1,
    VOS_UINT32                          ulPara2,
    VOS_UINT32                          ulPara3,
    VOS_UINT32                          ulPara4);



#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


