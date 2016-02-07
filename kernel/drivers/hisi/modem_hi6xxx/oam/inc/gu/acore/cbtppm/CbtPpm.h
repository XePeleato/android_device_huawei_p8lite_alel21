/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : CbtPpm.h
  版 本 号   : 初稿
  作    者   : h59254
  生成日期   : 2014年5月31日
  最近修改   :
  功能描述   : CbtPpm.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年5月31日
    作    者   : h59254
    修改内容   : 创建文件
******************************************************************************/

#ifndef __CBT_PPM_H__
#define __CBT_PPM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omnvinterface.h"
#include "OmCommonPpm.h"
#include "omrl.h"


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
 结构名    : CBTPPM_PORT_DEBUG_INFO_STUR
 协议表格  : 无
 结构说明  : USB CBT端口初始化调试统计信息

 修改历史      :
  1.日    期   : 2015年3月20日
    作    者   : H00300778
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulCbtPortSemErr;
    VOS_UINT32                          ulCbtPortRegOpen;
    VOS_UINT32                          ulCbtPortRegClose;
}CBTPPM_PORT_DEBUG_INFO_STUR;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_UINT32 CBTPPM_OamCbtPortDataSnd(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen);

VOS_UINT32 CBTPPM_OamUsbCbtSendData(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen);

VOS_VOID   CBTPPM_OamUsbCbtPortClose(VOS_VOID);

VOS_VOID   CBTPPM_OamUsbCbtPortOpen(VOS_VOID);

VOS_INT32  CBTPPM_OamUsbCbtReadDataCB(VOS_VOID);

VOS_VOID   CBTPPM_OamUsbCbtWriteDataCB(VOS_UINT8* pucVirData, VOS_UINT8* pucPhyData, VOS_INT lLen);

VOS_VOID   CBTPPM_OamUsbCbtStatusCB(ACM_EVT_E enPortState);

VOS_VOID   CBTPPM_OamCbtPortDataInit(OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                                 VOS_VOID                            *pReadCB,
                                                 VOS_VOID                            *pWriteCB,
                                                 VOS_VOID                            *pStateCB);

VOS_UINT32 CBTPPM_OamUsbCbtPortInit(VOS_VOID);

VOS_INT    CBTPPM_OamVComCbtReadData(VOS_UINT8 ucDevIndex, VOS_UINT8 *pData, VOS_UINT32 ullength);

VOS_UINT32 CBTPPM_OamVComCbtSendData(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen);

VOS_VOID   CBTPPM_OamVComCbtPortInit(VOS_VOID);

VOS_VOID   CBTPPM_OamCbtPortInit(VOS_VOID);

VOS_UINT32 PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel);

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

#endif /* end of cpm.h */
