/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : OmRl.h
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2008年6月11日
  最近修改   :
  功能描述   : OmRl.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2008年6月11日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/

#ifndef __OMAPPRL_H__
#define __OMAPPRL_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omrl.h"
#include "OmHdlcInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
extern VOS_UINT32                          g_ulOmAcpuDbgFlag ;


#define BIT_N(num)          (0x01 << (num))

#define OM_ACPU_RECV_USB        BIT_N(0)
#define OM_ACPU_DISPATCH_MSG    BIT_N(1)
#define OM_ACPU_SEND_SOCP       BIT_N(2)
#define OM_ACPU_SEND_USB        BIT_N(3)
#define OM_ACPU_USB_CB          BIT_N(4)
#define OM_ACPU_DIAG            BIT_N(5)
#define OM_ACPU_ERRLOG_SEND     BIT_N(6)
#define OM_ACPU_ERRLOG_RCV      BIT_N(7)
#define OM_ACPU_ERRLOG_PRINT    BIT_N(8)
#define OM_ACPU_RECV_CBT        BIT_N(9)
#define OM_ACPU_VCOM_CB         BIT_N(10)
#define OM_ACPU_SN_CHECK        BIT_N(11)

#define OM_ACPU_DEBUG_TRACE(pucData, ulDataLen, ulSwitch) \
    if(VOS_FALSE != (g_ulOmAcpuDbgFlag&ulSwitch)) \
    { \
        VOS_UINT32 ulOmDbgIndex; \
        vos_printf("\n%s, Data Len: = %d\n", __FUNCTION__, ulDataLen); \
        for (ulOmDbgIndex = 0 ; ulOmDbgIndex < ulDataLen; ulOmDbgIndex++) \
        { \
            vos_printf("%02x ", *((VOS_UINT8*)pucData + ulOmDbgIndex)); \
        } \
        vos_printf("\r\n"); \
    } \


#define OM_ACPU_DEBUG_CHANNEL_TRACE(enChanID, pucData, ulDataLen, ulSwitch) \
    if(VOS_FALSE != (g_ulOmAcpuDbgFlag&ulSwitch)) \
    { \
        VOS_UINT32 ulOmDbgIndex; \
        vos_printf("\n%s, channal ID: = %d, Data Len: = %d\n", __FUNCTION__, enChanID, ulDataLen); \
        for (ulOmDbgIndex = 0 ; ulOmDbgIndex < ulDataLen; ulOmDbgIndex++) \
        { \
            vos_printf("%02x ", *((VOS_UINT8*)pucData + ulOmDbgIndex)); \
        } \
        vos_printf("\r\n"); \
    } \

/* 商用ErrLog 打印 */
#define OM_ERR_LOG(string) \
    if(VOS_FALSE != (g_ulOmAcpuDbgFlag & OM_ACPU_ERRLOG_PRINT)) \
    {\
        vos_printf("%s.\r\n",string);\
    }\

#define OM_ERR_LOG1(string, para1) \
    if(VOS_FALSE != (g_ulOmAcpuDbgFlag & OM_ACPU_ERRLOG_PRINT)) \
    {\
        vos_printf("%s %d.\r\n", string, para1);\
    }\

#define OM_ERR_LOG2(string, para1, para2) \
    if(VOS_FALSE != (g_ulOmAcpuDbgFlag & OM_ACPU_ERRLOG_PRINT)) \
    {\
        vos_printf("%s %d, %d.\r\n", string, para1, para2);\
    }\




/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
结构名    : OMRL_MSG_COMBINE_INFO_STRU
结构说明  : OMRL模块完成组包需要的信息结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucTotalSegNum;      /* 总的分段数 */
    VOS_UINT8                           ucExpectedSegSn;    /* 组包函数中期望得到的消息包序号 */
    VOS_UINT16                          usRsv;
    VOS_UINT32                          ulTotalMsgLen;      /* 组包函数中组包后消息总的长度 */
    VOS_UINT32                          ulMoveLen;          /* 组包函数中每放入一消息包后的偏移 */
    MsgBlock                           *pstWholeMsg;        /*指向组包函数中完整消息包*/
}OMRL_MSG_COMBINE_INFO_STRU;

/*****************************************************************************
结构名    : OMRL_RCV_CHAN_CTRL_INFO_STRU
结构说明  : OMRL模块通道控制信息结构
*****************************************************************************/
typedef struct
{
    OM_LOGIC_CHANNEL_ENUM_UINT32        enChannel;
    VOS_UINT32                          ulOMSwitchOnOff;
    OMRL_MSG_COMBINE_INFO_STRU          stMsgCombineInfo;

    OM_ACPU_PC_UE_FAIL_STRU             stPcToUeErrRecord;
    OM_ACPU_PC_UE_SUC_STRU              stPcToUeSucRecord;
}OMRL_RCV_CHAN_CTRL_INFO_STRU;

/*****************************************************************************
结构名    : OMRL_CBT_HDLC_ENCODE_MEM_CTRL
结构说明  : OMRL CBT校准通道HDLC解析控制信息结构(uncache)
*****************************************************************************/
typedef struct
{
    VOS_UINT8                          *pucBuf;         /* 指向缓冲区虚拟首地址 */
    VOS_UINT8                          *pucRealBuf;     /* 指向缓冲区实首地址 */
    VOS_UINT32                          ulBufSize;      /* 保存缓冲区总大小 */
    VOS_UINT32                          ulRsv;          /* Reserve */
}OMRL_CBT_HDLC_ENCODE_MEM_CTRL;

/*****************************************************************************
  7 全局变量声明
*****************************************************************************/
/* CNF通道控制信息全局变量 */
extern OMRL_RCV_CHAN_CTRL_INFO_STRU            g_stAcpuCnfCtrlInfo;

/* CBT通道控制信息全局变量 */
extern OMRL_RCV_CHAN_CTRL_INFO_STRU            g_stAcpuCbtCtrlInfo;

extern OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stAcpuTxCnfCtrlInfo;
extern OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stAcpuTxIndCtrlInfo;

extern VOS_SEM                                 g_ulOmTxCbtSem;

/*记录开始时间*/
extern VOS_UINT32                              g_ulAcpuLastTick;

/*记录字节长度*/
extern VOS_UINT32                              g_ulAcpuLastByteCnt;

#ifdef SCM_SNCHECK
extern OM_HDLC_STRU                            g_astSnCheckHdlcEntity[2];
#endif

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 GU_OamSndPcMsgToCcpu(OMRL_RCV_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucData, VOS_UINT32 ulSize);
VOS_UINT32 OMRL_RcvCnfChannel(SOCP_DECODER_DST_ENUM_U32 enChanlID,VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT8 *pucRBData, VOS_UINT32 ulRBSize);
VOS_UINT32 OMRL_RcvCbtChannel(SOCP_DECODER_DST_ENUM_U32 enChanlID,VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT8 *pucRBData, VOS_UINT32 ulRBSize);
VOS_UINT32 OMRL_AcpuCpuIdToPid(VOS_UINT8 ucCpuId, VOS_UINT32 *pulPid);
VOS_UINT32 OMRL_AcpuMsgCombine(OMRL_RCV_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucData, VOS_UINT16 usLen);
VOS_UINT32 OMRL_AcpuSendMsg(VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT32 ulSenderPid, VOS_UINT32 ulReceiverPid);
VOS_UINT32 OMRL_AcpuReportEvent(VOS_VOID);
VOS_UINT32 OMRL_AcpuInit(VOS_VOID);
VOS_UINT32 OMRL_AcpuUsbFrameInit(VOS_VOID);
VOS_UINT32 OMRL_AcpuHeadProc(OMRL_RCV_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucData, VOS_UINT32 ulSize);
VOS_UINT32 OMRL_AcpuSendCbtData(VOS_UINT8  *pucSrc, VOS_UINT16  usSrcLen);

VOS_UINT32 OMRL_AcpuSendMsg(VOS_UINT8 *pucData, VOS_UINT32 ulSize,
                            VOS_UINT32 ulSenderPid, VOS_UINT32 ulReceiverPid);
VOS_UINT32 OMRL_AcpuReleaseSocpData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo);
VOS_UINT32 OMRL_AcpuSendSocpData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucHead, VOS_UINT32 ulHeadLen,
                            VOS_UINT8 *pucData, VOS_UINT32 ulDataLen);
VOS_UINT32 OMRL_AcpuIsBDEnough(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 ucMsgCnt);
VOS_UINT32 OMRL_AcpuMsgSplit(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucMsg, VOS_UINT16 usMsgLen);

VOS_VOID   OmAcpuRate(VOS_VOID);

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

#endif /* end of OmRl.h */

