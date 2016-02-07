/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : omnosig.h
  版 本 号   : 初稿
  作    者   : 甘兰47350
  生成日期   : 2011年9月27日

  最近修改   :
  功能描述   : omnosig.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年9月27日
    作    者   : 甘兰47350
    修改内容   : 创建文件

******************************************************************************/

#ifndef __OM_NOSIG_H__
#define __OM_NOSIG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "vos.h"
#include "omprivate.h"

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*OM<->APP : NON SIGALING Transparent Message.*/
#define APP_OM_NON_SIG_BT_TRAN_REQ      (0x1601)
#define OM_APP_NON_SIG_BT_TRAN_CNF      (0x1602)

/*OM<->APP : Get the WCDMA SYNC Status.*/
#define APP_OM_W_SYNC_STATUS_REQ        (0x1603)
#define OM_APP_W_SYNC_STATUS_CNF        (0x1604)

/*OM<->APP : Get the GMSK/EDGE DownLink Status.*/
#define APP_OM_GE_DL_STATUS_REQ         (0x1605)
#define OM_APP_GE_DL_STATUS_CNF         (0x1606)

/*OM<->APP : Get the EDGE BLER.*/
#define APP_OM_EDGE_BLER_REQ            (0x1607)
#define OM_APP_EDGE_BLER_CNF            (0x1608)

/*OM<->APP : Get the EDGE BLER.*/
#define APP_OM_W_BER_DATA_REQ           (0x1609)
#define OM_APP_W_BER_DATA_CNF           (0x160a)
#define OM_APP_W_BER_DATA_IND           (0x160b)

#define OM_FRAME_NUM_REG_MASKBIT                      (0x7FF)

/* 读取寄存器的延时长度 */
#define OM_EDGE_BLER_TASK_DELAY         (2000)

/* 等待DSP回复的超时长度 */
#define OM_NOSIG_WAIT_TIME_LEN          (5000)

/* 最大帧数*/
#define OM_FRAME_MAX_NUM                (2715648)

/*寄存器高位对应帧数*/
#define OM_HIHG_ADDR_FRAME_NUM                        (1326)

/*下发消息对应所需时间*/
#define OM_DL_MSG_FRAME_NUM                           (50)

/*译码邮箱最大数据长度，数据用32位数表示 */
#define OM_R99_MAILBOX_MAX_PHY_DATA_IND_LEN           ((328)*4)

/*上行传输信道个数*/
#define OM_UL_TRANS_CHANNEL_NUM                       (2)

/*R99 译码邮箱索引*/
#define OM_R99_DECODE_BUF_INDEX_INIT                  (0x0)
#define OM_R99_DECODE_BUF_INDEX_FIRST                 (0x1)
#define OM_R99_DECODE_BUF_INDEX_SECOND                (0x2)

#define OM_OFFSET_OF(s, m) ((VOS_SIZE_T)&(((s *)0)->m))

#define OM_BER_DATA_MAX_SIZE            (1024)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum OM_W_SNYC_STATUS
{
    OM_W_SYNC_STATUS_SYNC,
    OM_W_SYNC_STATUS_OUT_OF_SYNC,
    OM_W_SYNC_STATUS_UNKNOWN,
    OM_W_SYNC_STATUS_BUTT
};

typedef VOS_UINT32 OM_W_SNYC_STATUS_ENUM_UINT32;

enum OM_GE_DL_MODE
{
    OM_GE_DL_MODE_GSM,
    OM_GE_DL_MODE_EDGE,
    OM_GE_DL_MODE_BUTT
};

typedef VOS_UINT32 OM_GE_DL_MODE_ENUM_UINT32;

enum OM_R99_MAILBOX_MUTEX_ENUM
{
    OM_R99_MAILBOX_MUTEX_DATA_NOT_EXIST             = 0,        /* 共享变量中无数据，  可写，不可读;*/
    OM_R99_MAILBOX_MUTEX_DATA_EXIST                 = 1,        /* 共享变量中有数据，不可写，  可读;*/
    OM_R99_MAILBOX_MUTEX_BUTT
};

typedef VOS_UINT16 OM_R99_MAILBOX_MUTEX_ENUM_UINT16;
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
    VOS_UINT16 usEnableReport;  /* 是否使能消息上报,0不上报，1上报 */
    VOS_UINT16 usRcvMsgPrimId;  /* 等待消息ID */
    VOS_UINT16 usSyncStatus;    /* 保存W同步状态 */
    VOS_UINT8  ucFuncType;      /* Functype */
    VOS_UINT8  ucReserve;       /* 保留字 */
    VOS_SEM    ulNoSigSyncSem;  /* 串行等待消息同步信号量 */
    VOS_UINT32 ulTotalFrameNum; /* 上报的BER总帧数 */
    VOS_UINT32 ulCurFrameNum;   /* 上报的BER当前帧数 */
}OM_NOSIG_CTRL_STRU;

typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;   /*Indicates current msg type.*/
    VOS_UINT16             usToolId;   /*Not used now.*/
    VOS_UINT32             ulFirstErrNum;
    VOS_UINT32             ulFirstTotalNum;
    VOS_UINT32             ulSecondErrNum;
    VOS_UINT32             ulSecondTotalNum;
}OM_EDGE_BLER_STRU;

typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;    /*Indicates current msg type.*/
    VOS_UINT16             usReverse1;  /*Not used now.*/
    VOS_UINT16             usModeType;  /*1:GSM 2: EDGE*/
    VOS_UINT16             usReverse2;  /*Not used now.*/
}OM_GE_DL_STATUS_REQ_STRU;

typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;   /*Indicates current msg type.*/
    VOS_UINT16             usToolId;   /*Not used now.*/
    VOS_UINT16             usBerValue[2];
}OM_GE_DL_STATUS_STRU;

typedef struct
{
    VOS_UINT16  usNsMsgType;    /* VOS_FALSE:下发消息, VOS_TRUE:等待CNF或者IND消息 */
    VOS_UINT16  usNsMsgLen;     /* 消息的长度 */
    VOS_UINT16  usRcvMsgPrimId; /* 返回消息的ID值 */
    VOS_UINT16  usReserve;      /* 保留 */
}OM_NOSIG_MSG_HEAD_STRU;


typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;   /*Indicates current msg type.*/
    VOS_UINT16             usToolId;   /*Not used now.*/
    VOS_UINT16             usFrameNum;
    VOS_UINT16             usReserve;
}APP_OM_W_BER_DATA_REQ_STRU;

typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;   /*Indicates current msg type.*/
    VOS_UINT16             usToolId;   /*Not used now.*/
    VOS_UINT32             ulResult;
}APP_OM_W_BER_DATA_CNF_STRU;

typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;            /*Indicates current msg type.*/
    VOS_UINT16             usToolId;            /*Not used now.*/
    VOS_UINT16             usTotalFrameNum;
    VOS_UINT16             usCurFrameNum;
    VOS_UINT32             ulDataLen;           /* 数据长度,单位:字节,在数值上是4的整数倍,范围[0..WTTFPHY_MAX_PHY_DATA_REQ_LEN-1] */
    VOS_UINT8              aucData[OM_BER_DATA_MAX_SIZE]; /* 上行数据块 */
}APP_OM_W_BER_DATA_IND_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_UINT32 OM_BtTransProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
VOS_UINT32 OM_WSyncStatusProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
VOS_UINT32 OM_EdgeBlerProc (APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
VOS_UINT32 OM_GeDlStatusProc (APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
VOS_UINT32 OM_BerConfigProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
VOS_VOID   OM_LoopTestProc(VOS_VOID);


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

#endif /* end of omnosig.h */

