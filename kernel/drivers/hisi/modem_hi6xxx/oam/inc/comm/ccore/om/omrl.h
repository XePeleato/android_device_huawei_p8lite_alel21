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

#ifndef __OMRL_H__
#define __OMRL_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omringbuffer.h"
#include "pslog.h"
#include "PsTypeDef.h"
#include "om.h"
#include "OmToolInterface.h"
#if((VOS_OS_VER == VOS_LINUX) || (VOS_OSA_CPU == OSA_CPU_ACPU))
#include "PsLogAdapterApp.h"
#else
#include "pslogadapter.h"
#endif
#include "SOCPInterface.h"
#include "ombufmngr.h"
#include "OmCommonPpm.h"
#include <product_config.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* GU datatype */
#define OM_RL_DATATYPE_GU                           (0x1)

/* datatype 字段长度 */
#define OM_RL_DATATYPE_LEN                          (0x1)

/* 工具下发数据包组包后最大长度 */
#define OM_RL_TOTAL_MSG_MAX_LEN                     (8*1024)


#define OM_RL_HDLC_FRAME                            0x0
#define OM_RL_USB_FRAME                             0x1

/*Indicates current OM's state.*/
#define OM_STATE_IDLE                               0 /*IDLE态，OM不能正常与工具进行通信*/
#define OM_STATE_ACTIVE                             1 /*ACTIVE态，OM能够正常工作*/

#define OM_HDLC_MAX_RATE_SIZE                       (2)
#define OM_HDLC_MAX_HEAD_SIZE                       (6)

#define OM_APP_MSG_SEGMENT_HEADER_LENGTH            (4U)
#define APP_OM_HEADER_LENGTH                        (4U)
#define OM_APP_MSG_SEGMENT_LEN                      (4U*1024U)  /* Modify By Lx SOCP  */
#define OM_APP_SPLIT_MSG_LEN                        (OM_APP_MSG_SEGMENT_LEN + OM_APP_MSG_SEGMENT_HEADER_LENGTH)

/*#define OM_DRV_MAX_IO_LEN                           (OM_APP_MSG_SEGMENT_LEN          \
 */
/*                                                    + sizeof(OM_APP_MSG_SEG_EX_STRU) \
 */
/*                                                    + sizeof(VOS_UINT32))                  tail aaaa5555 */

#define OM_SWITCH_DATA_LEN                          15  /*建立链路和释放链路数据的长度*/
#define OM_SWITCH_REQ_OFFSET                        17  /*回复消息中用来标明结果的偏移长度*/
#define OM_SWITCH_CRC_OFFSET                        21  /*回复消息中用来标明FCS校验的偏移长度*/

#define OM_USB_EXIST                                1

#define UEPS_PID_OMRL                               WUEAPP_PID_MUX
#define OM_HDLC_DEST_MAX                            ((OM_APP_MSG_SEGMENT_LEN*2) + 6)
/*Buffer length provided the HDLC API*/
#define OM_HDLC_BUF_MAX_LEN                         (OM_APP_SPLIT_MSG_LEN + OM_USB_FRAME_LEN + OM_RL_DATATYPE_LEN + OM_HDLC_FCS_LEN)

/*在计算OM_APP_MSG_SEGMENT_STRU结构size时多余的一个字节*/
#define OM_MSG_SEGMENT_REDUNDANT                    1

#define OM_USB_FRAME_HEAD                           0xaaaa5555
#define OM_USB_FRAME_TAIL                           0x5555aaaa

/* 支持CBT抓LOG */
#define OM_CBT_LOG_ENABLE_FLAG                      0x5a5a5a5a

#define OM_USB_HEAD_LEN                             4
#define OM_USB_TAIL_LEN                             4

/*帧长度为OM_USB_FRAME_HEAD长度 + OM_USB_FRAME_TAIL长度*/
#define OM_USB_FRAME_LEN                            (OM_USB_HEAD_LEN + OM_USB_TAIL_LEN)

#define OM_CONFIG_HEAD_SIZE                         (9)       /*3(PC保留字段) + 4(消息长度) + 2(PC保留字段)*/

#define OM_EST_LINK_SIZE                            (24)

#define OM_DATA_MAX_LENGTH                          (0x7FFFFFFF)

extern VOS_UINT32                                   g_ulOmCcpuDbgFlag ;

extern VOS_UINT32                                   g_ulFTMFlag;

/* CCPU 码流 DEBUG 信息 */
#define OM_CCPU_DEBUG_TRACE(pucData, ulDataLen, ulSwitch) \
    if(VOS_FALSE != (g_ulOmCcpuDbgFlag&ulSwitch)) \
    { \
        VOS_UINT32 ulOmDbgIndex; \
        vos_printf("\n%s, Data Len: = %d\n", __FUNCTION__, ulDataLen); \
        for (ulOmDbgIndex = 0 ; ulOmDbgIndex < ulDataLen; ulOmDbgIndex++) \
        { \
            vos_printf("%02x ", *((VOS_UINT8*)pucData + ulOmDbgIndex)); \
        } \
        vos_printf("\r\n"); \
    } \

#define OM_BIT_N(num)               (0x01 << (num))

/* 用来控制OM的打印输出 */
#ifdef OAM_UT_DEBUG
#define OM_CCPU_RECV_ICC            OM_BIT_N(1)
#else
#define OM_CCPU_RECV_ICC            OM_BIT_N(0)
#endif

#define OM_CCPU_PUT_OM_BUFF         OM_BIT_N(1)

#define OM_CCPU_ERRLOG_RCV          OM_BIT_N(7)
#define OM_CCPU_ERRLOG_PRINT        OM_BIT_N(8)

/* 商用ErrLog 打印 */
#define OM_CCPU_ERR_LOG(string) \
    if(VOS_FALSE != (g_ulOmCcpuDbgFlag & OM_CCPU_ERRLOG_PRINT)) \
    {\
        vos_printf("%s.\r\n",string);\
    }\

#define OM_CCPU_ERR_LOG1(string, para1) \
    if(VOS_FALSE != (g_ulOmCcpuDbgFlag & OM_CCPU_ERRLOG_PRINT)) \
    {\
        vos_printf("%s %d.\r\n", string, para1);\
    }\

#define OM_CCPU_ERR_LOG2(string, para1, para2) \
    if(VOS_FALSE != (g_ulOmCcpuDbgFlag & OM_CCPU_ERRLOG_PRINT)) \
    {\
        vos_printf("%s %d, %d.\r\n", string, para1, para2);\
    }\

#define OM_GET_SOCP_STATUS                (1)
#define OM_GET_SOCP_DEBUG_INFO            (2)
#define OM_SEND_DEBUG_INFO_MAX_LEN        (2*1024)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum OM_PORT_MODE_ENUM
{
     AT_MODE = 0,
     OM_MODE,
     DIAG_MODE,
     MODE_BUTT
};

typedef VOS_UINT32     OM_PORT_MODE_ENUM_UINT32;


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

typedef struct
{
    VOS_OM_HEADER
    VOS_UINT8                      aucPara[4]; /*The content*/
}APP_OM_GU_MSG_STRU;


typedef struct
{
    VOS_UINT16      usPacketLen;        /*帧长度*/
    VOS_UINT16      usRemainLen;        /*缓冲区中已经有的数据长度*/
    VOS_UINT8       *pucRemainData;     /*指向缓冲区的指针*/
}OM_REMAIN_FRAME_STRU;

typedef struct _OM_BUFFER_POOL
{
    struct _OM_BUFFER_POOL  *pstNext;   /*指向下一个缓冲区*/
    VOS_UINT32              ulLen;      /*缓冲区已经使用的长度*/
    VOS_UINT8               *pucPacket; /*指向缓冲区分配的内存空间*/
} OM_BUFFER_POOL;

typedef struct
{
    VOS_UINT32              ulPoolNum;  /*缓冲区的个数*/
    OM_BUFFER_POOL          *pstWrite;  /*指向用来写入数据的缓冲区*/
    OM_BUFFER_POOL          *pstRead;   /*指向用来读取数据的缓冲区*/
} OM_BUFFER_QUEUE;

/*****************************************************************************
结构名    : OM_CCPU_ICC_REC_SUC_STRU
结构说明  : CCPU ICC通道成功接收的可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulICCOmRecMsgNum;       /*当前接收的总消息数*/
    VOS_UINT32                      ulICCOmRecLen;          /*当前接收的总消息长度*/
    VOS_UINT32                      ulICCOmRecSlice;        /*当前接收的最后一条数据的时间*/
}OM_CCPU_ICC_REC_SUC_STRU;

/*****************************************************************************
结构名    : OM_CCPU_ICC_REC_FAIL_STRU
结构说明  : CCPU ICC通道接收失败的可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulICCOmParaErrNum;      /*当前接收的错误记录*/
    VOS_UINT32                      ulICCOmRecErrMsgNum;    /*当前接收的错误总消息数*/
    VOS_UINT32                      ulICCOmRecErrLen;       /*当前接收的错误总消息长度*/
    VOS_UINT32                      ulICCOmRecErrSlice;     /*当前接收的错误消息最后一条数据的时间*/
}OM_CCPU_ICC_REC_FAIL_STRU;

/*****************************************************************************
结构名    : OM_ACPU_ICC_SEND_SUC_STRU
结构说明  : ACPU ICC通道成功发送的可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulICCOmSendMsgNum;      /*当前发送的总消息数*/
    VOS_UINT32                          ulICCOmSendLen;         /*当前发送的总消息长度*/
    VOS_UINT32                          ulICCOmSendSlice;       /*当前发送的最后一条数据的时间*/
}OM_ACPU_ICC_SEND_SUC_STRU;

/*****************************************************************************
结构名    : OM_ACPU_ICC_SEND_FAIL_STRU
结构说明  : ACPU ICC通道发送失败的可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulICCOmSendErrNum;      /*当前发送的错误记录*/
    VOS_UINT32                          ulICCOmSendErrLen;      /*当前发送的错误总消息长度*/
    VOS_UINT32                          ulICCOmSendErrSlice;    /*当前发送的错误消息最后一条数据的时间*/
}OM_ACPU_ICC_SEND_FAIL_STRU;

/*****************************************************************************
结构名    : OM_MAINTENANCE_INFO_STRU
结构说明  : OM模块可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulDataLen; /* 接收或发送数据长度 */
    VOS_UINT32                          ulNum;     /* 接收或发送数据次数 */
}OM_MAINTENANCE_INFO_STRU;

/*****************************************************************************
结构名    : OM_CCPU_PC_UE_SUC_STRU
结构说明  : OM模块正常接收可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    OM_MAINTENANCE_INFO_STRU            stTotalData;       /* 记录接收到的总数据 */
    OM_MAINTENANCE_INFO_STRU            stRealData;        /* 记录真正发到om模块的数据 */
    OM_CCPU_ICC_REC_SUC_STRU            stICCRecSUCInfo;   /* 记录CCPU成功接收的ICC数据 */
}OM_CCPU_PC_UE_SUC_STRU;

/*****************************************************************************
结构名    : OM_CCPU_PC_UE_FAIL_STRU
结构说明  : OM模块接收失败可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    OM_MAINTENANCE_INFO_STRU            stParaErrData;     /* 记录因为参数错误丢掉的数据 */
    OM_MAINTENANCE_INFO_STRU            stSendErrData;     /* 记录发给OM模块时出错丢掉的数据*/
    OM_CCPU_ICC_REC_FAIL_STRU           stICCRecFailInfo;  /* 记录CCPU接收失败的ICC数据 */
}OM_CCPU_PC_UE_FAIL_STRU;

/*****************************************************************************
结构名    : OM_ACPU_PC_UE_RELEASE_INFO_STRU
结构说明  : OM模块断链操作信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulRlsAp2Modem;     /* AP到Modem切换 */
    VOS_UINT32                          ulRlsModem2Ap;     /* Modem到AP切换 */
    VOS_UINT32                          ulRlsDrvSuspend;   /* DRV挂起 */
    VOS_UINT32                          ulRlsPortClose;    /* 端口关闭 */
    VOS_UINT32                          ulRlsPortSwitch;   /* 端口切换 */
    VOS_UINT32                          ulSlice;           /* 最后一次收到断链的时间 */
}OM_ACPU_PC_UE_RELEASE_INFO_STRU;

/*****************************************************************************
结构名    : OM_ACPU_PC_UE_SUC_STRU
结构说明  : OM模块正常接收可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    OM_MAINTENANCE_INFO_STRU            stTotalData;       /* 记录接收到的总数据 */
    OM_MAINTENANCE_INFO_STRU            stHdlcDecapData;
    OM_MAINTENANCE_INFO_STRU            stLinkData;        /* 记录收到的链路数据 */
    OM_MAINTENANCE_INFO_STRU            stCcpuData;        /* 记录发给CCPU的数据 */
    OM_MAINTENANCE_INFO_STRU            stAcpuData;        /* 记录发给ACPU的数据 */
    OM_MAINTENANCE_INFO_STRU            stMcuData;         /* 记录发给MCU的数据 */
    OM_MAINTENANCE_INFO_STRU            stHifiData;        /* 记录发给HIFI的数据 */
    OM_ACPU_ICC_SEND_SUC_STRU           stICCSendSUCInfo;  /* 记录ACPU成功发送的ICC数据 */
    OM_MAINTENANCE_INFO_STRU            stRlsData;         /* 记录工具发送的Release数据 */
    OM_ACPU_PC_UE_RELEASE_INFO_STRU     stRlsInfo;         /* 记录Release信息 */
}OM_ACPU_PC_UE_SUC_STRU;

/*****************************************************************************
结构名    : OM_ACPU_PC_UE_FAIL_STRU
结构说明  : OM模块异常接收可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    OM_MAINTENANCE_INFO_STRU            stLostData;         /* 记录丢掉的数据 */
    VOS_UINT32                          ulFrameDecapErr;    /* 记录发生解帧错误的次数 */

    VOS_UINT16                          usPacketLenErr;     /* 记录发生包长度小于OM_USB_FRAME_LEN错误的次数 */
    VOS_UINT16                          usLenTooShortErr;   /* 记录发生数据长度小于分段头的错误次数 */
    VOS_UINT16                          usSegLenErr;        /* 记录发生段长度错误次数 */
    VOS_UINT16                          usSegNumErr;        /* 记录发生段数目错误次数 */

    VOS_UINT16                          usDatatypeErr;      /* 记录发生datatype字段错误次数 */
    VOS_UINT16                          usMsgTooLongErr;    /* 记录发生组包后消息大于8K的错误次数 */
    VOS_UINT16                          usCpuIdErr;         /* 记录发生CPU id错误次数 */
    VOS_UINT16                          usNoMemErr;         /* 记录发生空间不足的错误次数 */

    VOS_UINT16                          usDataHeadErr;      /* 记录发生数据帧头部错误次数 */
    VOS_UINT16                          usMsgLenErr;        /* 记录发生消息长度错误次数 */
    VOS_UINT16                          usMsgSnErr;         /* 记录发生消息序号错误次数 */
    VOS_UINT16                          usPacketLostErr;    /* 记录发生包丢失的错误次数 */

    VOS_UINT16                          usLinkDataLenErr;   /* 记录链路消息长度错误的次数 */
    VOS_UINT16                          usLinkStatusErr;    /* 记录链路关闭时收到消息包的次数 */
    VOS_UINT16                          usAllocMsg;         /* 记录消息分配失败的次数 */
    VOS_UINT16                          usRcv;

    OM_ACPU_ICC_SEND_FAIL_STRU          stICCSendFailInfo;  /* 记录ACPU成功发送的ICC数据 */
}OM_ACPU_PC_UE_FAIL_STRU;

/*****************************************************************************
结构名    : OM_UE_PC_SUC_STRU
结构说明  : OM模块正常发送可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    OM_MAINTENANCE_INFO_STRU            stTotalData;         /* 记录协议栈发送的总数据 */
    OM_MAINTENANCE_INFO_STRU            stRealSendData;      /* 记录真正发到SOCP的数据 */
    OM_MAINTENANCE_INFO_STRU            stRealRlsData;       /* 记录真正释放SOCP的数据 */
    VOS_UINT32                          ulMaxUsedBufSize;    /* 记录使用buf的最大值 */
}OM_UE_PC_SUC_STRU;

/*****************************************************************************
结构名    : OM_UE_PC_FAIL_STRU
结构说明  : OM模块异常发送可维可测信息记录结构
*****************************************************************************/
typedef struct
{
    OM_MAINTENANCE_INFO_STRU            stIntData;           /* 记录因为中断中调用丢掉的数据 */
    OM_MAINTENANCE_INFO_STRU            stLostData;          /* 记录因为各种错误丢掉的数据 */
    OM_MAINTENANCE_INFO_STRU            stLinkErrData;       /* 记录因为链路关闭丢掉的数据 */
    OM_MAINTENANCE_INFO_STRU            stParaErrData;       /* 记录因为参数错误丢掉的数据 */

    VOS_UINT16                          usSemErr;            /* 记录获取信号量失败的次数 */
    VOS_UINT16                          usRlsRdErr;          /* 记录获取已发送RD失败的次数 */
    VOS_UINT16                          usRlsMemErr;         /* 记录获取释放缓冲区内存失败的次数 */
    VOS_UINT16                          usBDLackErr;         /* 记录BD不足的次数 */
    VOS_UINT16                          usGetBDErr;          /* 记录获取BDAPI返回错误的次数 */
    VOS_UINT16                          usNoMemErr;          /* 记录内存不足的次数 */

    VOS_UINT16                          usSendSocpDataErr;   /* 记录发送数据到SOCP错误的次数 */
    VOS_UINT16                          usSendAddrErr;       /* 记录发送地址转换错误 */

    VOS_UINT16                          usRlsAddrErr;        /* 记录释放地址转换错误 */
    VOS_UINT16                          usSendInIsrErr;      /* 记录在中断中发送数据错误次数 */
}OM_UE_PC_FAIL_STRU;

/* 上报可维可测是消息结构体 */
typedef struct
{
    VOS_UINT32                         ulLenth;
    VOS_UINT8                         *pucAddr;

}OM_SEND_DEBUG_INFO_STRU;


typedef struct
{
    OM_LOGIC_CHANNEL_ENUM_UINT32        enChannel;
    VOS_SEM                             OmTxBuffSem;
    OM_BUF_CTRL_STRU                    OmBufCtrl;
    SOCP_CODER_SRC_ENUM_U32             enChanlID;

    OM_UE_PC_SUC_STRU                   stUeToPcSucRecord;
    OM_UE_PC_FAIL_STRU                  stUeToPcErrRecord;
}OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU;

/*****************************************************************************
结构名    : OMRL_TX_CHAN_CTRL_INFO_STRU
结构说明  : OMRL模块通道控制信息结构
*****************************************************************************/

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/* 记录CCPU接收到PC侧数据的正常信息 */
extern OM_CCPU_PC_UE_SUC_STRU                  g_stPcToUeSucRecord;

/* 记录CCPU接收到PC侧数据的异常信息 */
extern OM_CCPU_PC_UE_FAIL_STRU                 g_stPcToUeErrRecord;

extern VOS_UINT32                              g_ulCbtSwitchOnOff;


/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_UINT32 OMRL_Init(VOS_VOID);
VOS_UINT32 OMRL_MsgSplit(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucMsg, VOS_UINT16 usMsgLen);
VOS_UINT32 OMRL_UsbFrameInit(VOS_VOID);
VOS_UINT32 OMRL_CcpuReportEvent(VOS_VOID);
VOS_UINT32 OMRL_SendMsg(VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT32 ulSenderPid, VOS_UINT32 ulReceiverPid);
VOS_UINT32 OMRL_ReleaseSocpData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo);
VOS_UINT32 OMRL_SendSocpData(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo,
                             VOS_UINT8 *pucHead, VOS_UINT32 ulHeadLen,
                             VOS_UINT8 *pucData, VOS_UINT32 ulDataLen);
VOS_UINT32 OMRL_IsBDEnough(OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 ucMsgCnt);

VOS_UINT32 SCM_GetDebugLogInfoLen(VOS_VOID);
VOS_UINT8* SCM_GetDebugLogInfo(VOS_VOID);
#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
VOS_UINT32 SOCP_GetDebugLogInfoLen(VOS_UINT32  ulType);
VOS_UINT8* SOCP_GetDebugLogInfo(VOS_UINT32  ulType);
#endif
VOS_VOID OM_LogShowToFile(VOS_BOOL bIsSendMsg);

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

