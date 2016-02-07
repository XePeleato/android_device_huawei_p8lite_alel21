

#ifndef __OM_COMMON_PPM_H__
#define __OM_COMMON_PPM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "omnvinterface.h"
#include "mdrv.h"
#include "SOCPInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define USB_MAX_DATA_LEN            (60*1024)   /*USB发送的最大数据长度*/

#define SOCP_SEND_DATA_PARA_CHECK(pstDebugInfo, ulDataLen, pucData) \
    if ((0 == ulDataLen)\
    || (VOS_NULL_PTR == pucData))\
    {\
        pstDebugInfo->ulUSBSendCBAbnormalNum++;\
        return;\
    }\

/*******************************************************************************
  3 枚举定义
*******************************************************************************/
/* UDI设备句柄 */
enum OM_PROT_HANDLE_ENUM
{
    OM_USB_IND_PORT_HANDLE      =   0,
    OM_USB_CFG_PORT_HANDLE      =   1,
    OM_USB_CBT_PORT_HANDLE      =   2,
    OM_HSIC_IND_PORT_HANDLE     =   3,
    OM_HSIC_CFG_PORT_HANDLE     =   4,
    OM_PORT_HANDLE_BUTT
};

typedef VOS_UINT32  OM_PROT_HANDLE_ENUM_UINT32;

enum OM_LOGIC_CHANNEL_ENUM
{
     OM_LOGIC_CHANNEL_CBT = 0,
     OM_LOGIC_CHANNEL_CNF,
     OM_LOGIC_CHANNEL_IND,
     OM_LOGIC_CHANNEL_BUTT
};

typedef VOS_UINT32     OM_LOGIC_CHANNEL_ENUM_UINT32;


/*****************************************************************************
  4 结构体定义
*****************************************************************************/

typedef struct
{
    VOS_SEM                             ulPseudoSyncSemId;   /* 伪造为同步接口需使用的信号量 */
    VOS_UINT8                          *pucAsyncCBData;      /* DRV_UDI_IOCTL接口异步返回后传入的数据指针 */
    VOS_UINT32                          ulLen;               /* DRV_UDI_IOCTL接口异步返回后返回的实际处理数据长度 */
    VOS_UINT32                          ulRsv;               /* Reserve */
}OM_PSEUDO_SYNC_STRU;

/*****************************************************************************
 结构名    : PPM_PORT_CFG_INFO_STRU
 协议表格  : 无
 结构说明  : PPM端口切换统计信息

 修改历史      :
  1.日    期   : 2014年5月25日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulPortTypeErr;
    VOS_UINT32                          ulSwitchFail;
    VOS_UINT32                          ulSwitchSucc;
    VOS_UINT32                          ulStartSlice;
    VOS_UINT32                          ulEndSlice;
}PPM_PORT_CFG_INFO_STRU;

/*****************************************************************************
 结构名    : PPM_PORT_DEBUG_INFO_STUR
 协议表格  : 无
 结构说明  : PPM端口初始化调试统计信息

 修改历史      :
  1.日    期   : 2015年3月20日
    作    者   : H00300778
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulUsbCnfSemErr;
    VOS_UINT32                          ulUsbCnfRegOpen;
    VOS_UINT32                          ulUsbCnfRegClose;
    VOS_UINT32                          ulUsbIndSemErr;
    VOS_UINT32                          ulUsbIndRegOpen;
    VOS_UINT32                          ulUsbIndRegClose;
}PPM_PORT_DEBUG_INFO_STUR;


/*****************************************************************************
  4 函数声明
*****************************************************************************/
#if((VOS_OS_VER == VOS_LINUX) || (VOS_OSA_CPU == OSA_CPU_ACPU))
extern VOS_UINT32 PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel);

extern VOS_UINT32 PPM_DisconnectTLPort(VOS_VOID);

extern VOS_VOID   PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel);

extern VOS_INT32  PPM_ReadPortData(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, UDI_HANDLE UdiHandle, OM_PROT_HANDLE_ENUM_UINT32 enHandle);

extern VOS_VOID   PPM_PortWriteAsyCB(OM_PROT_HANDLE_ENUM_UINT32 enHandle, VOS_UINT8* pucData, VOS_INT lLen);

extern VOS_VOID   PPM_ReadPortDataInit(CPM_PHY_PORT_ENUM_UINT32        enPhyPort,
                                    OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                    VOS_VOID                            *pReadCB,
                                    VOS_VOID                            *pWriteCB,
                                    VOS_VOID                            *pStateCB);

extern VOS_UINT32 PPM_UdiRegCallBackFun(UDI_HANDLE enHandle, VOS_UINT32 ulCmdType, VOS_VOID* pFunc);

extern VOS_VOID   PPM_PortCloseProc(OM_PROT_HANDLE_ENUM_UINT32  enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort);

extern VOS_UINT32 PPM_PortSend(OM_PROT_HANDLE_ENUM_UINT32 enHandle, VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen);

extern VOS_VOID   PPM_PortStatus(OM_PROT_HANDLE_ENUM_UINT32 enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort,ACM_EVT_E enPortState);

extern VOS_VOID   PPM_GetSendDataLen(SOCP_CODER_DST_ENUM_U32 enChanID, VOS_UINT32 ulDataLen, VOS_UINT32 *pulSendDataLen, CPM_PHY_PORT_ENUM_UINT32 *penPhyport);

extern VOS_UINT32 PPM_PortInit(VOS_VOID);

extern VOS_UINT32 PPM_InitPhyPort(VOS_VOID);
#endif

extern VOS_UINT32 PPM_UsbPortInit(VOS_VOID);

extern VOS_VOID   PPM_HsicPortInit(VOS_VOID);

extern VOS_VOID   PPM_VComPortInit(VOS_VOID);

extern VOS_UINT32 SCM_RlsDestBuf(VOS_UINT32 ulChanlID, VOS_UINT32 ulReadSize);

extern VOS_UINT8* SCM_CoderDestMemVirtToPhy(VOS_UINT32 ulDstChID, VOS_UINT8 *pucVirtAddr);
/*****************************************************************************
  5 全局变量声明
*****************************************************************************/
extern UDI_HANDLE                              g_astOMPortUDIHandle[OM_PORT_HANDLE_BUTT];

/* USB承载的OM IND端口中，伪造为同步接口使用的数据结构体 */
extern OM_PSEUDO_SYNC_STRU                     g_stUsbIndPseudoSync;

/* USB承载的OM CNF端口中，伪造为同步接口使用的数据结构体 */
extern OM_PSEUDO_SYNC_STRU                     g_stUsbCfgPseudoSync;

/*****************************************************************************
  6 OTHERS定义
*****************************************************************************/




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

#endif /* end of OmCommonPpm.h*/

