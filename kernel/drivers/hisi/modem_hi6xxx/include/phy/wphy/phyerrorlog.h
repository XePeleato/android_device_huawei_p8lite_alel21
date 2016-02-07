/******************************************************************************

                  版权所有 (C), 2003-2013, 华为技术有限公司

 ******************************************************************************
  文 件 名   : phyerrorlog.h
  版 本 号   : 初稿
  作    者   : 罗青全/57356
  生成日期   : 2013年9月21日
  最近修改   :
  功能描述   : 商用ERRORLOG及工程模式上报使用的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年09月21日
    作    者   : 罗青全/57356
    修改内容   : 创建文件

******************************************************************************/

#ifndef __PHY_ERRORLOG_H__
#define __PHY_ERRORLOG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omerrorlog.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/



#define PHY_ERR_LOG_CTRL_LEVEL_NULL             (0)                             /* ErrLog等级未定义*/
#define PHY_ERR_LOG_CTRL_LEVEL_CRITICAL         (1)                             /* ErrLog等级为紧急 */
#define PHY_ERR_LOG_CTRL_LEVEL_MAJOR            (2)                             /* ErrLog等级为重要 */
#define PHY_ERR_LOG_CTRL_LEVEL_MINOR            (3)                             /* ErrLog等级为次要 */
#define PHY_ERR_LOG_CTRL_LEVEL_WARNING          (4)                             /* ErrLog等级为提示 */


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 枚举名    : PHY_ERR_LOG_ALM_TYPE_ENUM
 枚举说明  : 故障告警类型
 1.日    期   : 2013年09月23日
   作    者   : luoqingquan
   修改内容   : 新建
*****************************************************************************/
enum PHY_ERR_LOG_ALM_TYPE_ENUM
{
    PHY_ERR_LOG_ALM_TYPE_COMMUNICATION    = 0x00,                               /* 通信 */
    PHY_ERR_LOG_ALM_TYPE_TRAFFIC_QUALITY  = 0x01,                               /* 业务质量 */
    PHY_ERR_LOG_ALM_TYPE_DEAL_WRONG       = 0x02,                               /* 处理出错 */
    PHY_ERR_LOG_ALM_TYPE_DEVICE_FAILURE   = 0x03,                               /* 设备故障 */
    PHY_ERR_LOG_ALM_TYPE_ENV_FAILURE      = 0x04,                               /* 环境故障 */
    PHY_ERR_LOG_ALM_TYPE_BUTT
};
typedef VOS_UINT16 PHY_ERR_LOG_ALM_TYPE_ENUM_UINT16;


/*****************************************************************************
 枚举名    : PHY_ERR_LOG_ALM_ID_ENUM
 枚举说明  : 故障告警ID
 1.日    期   : 2013年09月23日
   作    者   : luoqingquan
   修改内容   : 新建
*****************************************************************************/
enum PHY_ERR_LOG_ALM_ID_ENUM
{
    /* 缓存故障 */
    PHY_ERR_LOG_ALM_WCDMA_OUT_SYNC              = 0x01,                         /* 失步 */
    PHY_ERR_LOG_ALM_WCDMA_BCCH_FAIL             = 0x02,                         /* BCCH失败 */
    PHY_ERR_LOG_ALM_WCDMA_RL_FAIL               = 0x03,                         /* 链路失败 */
    PHY_ERR_LOG_ALM_GSM_DEC_FAIL                = 0x04,                         /* DSC记录 */
    PHY_ERR_LOG_ALM_GSM_RXLEV_MEAS              = 0x05,                         /* 处理记录 */
    PHY_ERR_LOG_ALM_GSM_FB_SB_IND               = 0x06,                         /* 搜索记录 */
    PHY_ERR_LOG_ALM_RING_BUFFER_OVERFLOW        = 0x07,                         /* 环形buffer溢出记录 */

    /* 语音类故障告警 */
    PHY_ERR_LOG_ALM_CS_RLB                      = 0x1001,                       /* CS 链路质量差 */
    PHY_ERR_LOG_ALM_CS_NO_UL_DATA               = 0x1002,                       /* CS 无上行数据 */
    PHY_ERR_LOG_ALM_GSM_CS_ERROR_DETECT         = 0x1003,                       /* 检测到异常error */

    /* 业务类故障告警 */
    PHY_ERR_LOG_ALM_WCDMA_TX_POWER_DROP_DETECT  = 0x2001,                       /* 检测到发射功率太低 */
    PHY_ERR_LOG_ALM_WCDMA_PILOT_BER_RISE_DETECT = 0x2002,                       /* 检测到pilotber陡升 */

    PHY_ERR_LOG_ALM_ID_BUTT
};
typedef VOS_UINT16 PHY_ERR_LOG_ALM_ID_ENUM_UINT16;


/*****************************************************************************
 枚举名    : PHY_FTM_LOG_ID_ENUM
 枚举说明  : 工程模式上报的功能ID
 1.日    期   : 2013年08月27日
   作    者   : l57356
   修改内容   : 新建
*****************************************************************************/
enum PHY_FTM_LOG_ID_ENUM
{
    PHY_FTM_LOG_ID_WPHY,
    PHY_FTM_LOG_ID_GPHY,

    PHY_FTM_LOG_ID_BUTT,
};
typedef PHY_UINT16 PHY_FTM_LOG_ID_ENUM_UINT16;


/*****************************************************************************
 枚举名    : PHY_WCDMA_RL_OUT_OF_SYNC_REASON_ENUM
 枚举说明  : 注册的域信息
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*****************************************************************************/
enum PHY_WCDMA_RL_OUT_OF_SYNC_REASON_ENUM
{
    PHY_WCDMA_RL_OUT_OF_SYNC_REASON_DEMOD        = 0x00,                        /* 上报原因为解调 */
    PHY_WCDMA_RL_OUT_OF_SYNC_REASON_DECODE       = 0x01,                        /* 上报原因为译码 */
    PHY_WCDMA_RL_OUT_OF_SYNC_REASON_BUTT
};
typedef VOS_UINT16 PHY_WCDMA_RL_OUT_OF_SYNC_REASON_ENUM_UINT16;



/*****************************************************************************
 枚举名    : PHY_WCDMA_BCCH_FAIL_REASON_ENUM
 枚举说明  : 注册的域信息
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*****************************************************************************/
enum PHY_WCDMA_BCCH_FAIL_REASON_ENUM
{
    PHY_WCDMA_BCCH_FAIL_REASON_SFN_UPDATE_FAIL   = 0x00,                        /* 上报原因为SFN更新 */
    PHY_WCDMA_BCCH_FAIL_REASON_CRC_ERR           = 0x01,                        /* 上报原因为CRC错误 */
    PHY_WCDMA_BCCH_FAIL_REASON_SCHEDULE_FAIL     = 0x02,                        /* 上报原因为调度错误 */
    PHY_WCDMA_BCCH_FAIL_REASON_WORK_TIME_ERR     = 0x03,                        /* 上报原因为定时错误 */
    PHY_WCDMA_BCCH_FAIL_REASON_BUTT
};
typedef VOS_UINT16 PHY_WCDMA_BCCH_FAIL_REASON_ENUM_UINT16;


/*****************************************************************************
 枚举名    : PHY_GSM_RPT_REASON_ENUM
 枚举说明  : 注册的域信息
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*****************************************************************************/
enum PHY_GSM_RPT_REASON_ENUM
{
    PHY_GSM_RPT_REASON_DSC                      = 0x00,                         /* 上报原因为DSC错误 */
    PHY_GSM_RPT_REASON_CRC_ERR                  = 0x01,                         /* 上报原因为CRC错误 */
    PHY_GSM_RPT_REASON_BUTT
};
typedef VOS_UINT16 PHY_GSM_RPT_REASON_ENUM_UINT16;



enum PHY_GSM_CS_ERROR_REASON_ENUM
{
    PHY_GSM_CS_ERROR_RLB            = 0,                         /* CS链路差 */
    PHY_GSM_CS_ERROR_NO_UL_DATA     = 1,                         /* 无上行语音数据 */
    PHY_GSM_CS_ERROR_DETECT         = 2,                         /* 异常error */
    PHY_GSM_CS_ERROR_REASON_BUTT
};
typedef VOS_UINT16 PHY_GSM_CS_ERROR_REASON_ENUM_UINT16;
enum PHY_WCDMA_CS_ERROR_REASON_ENUM
{
    PHY_WCDMA_CS_ERROR_RLB          = 0,                        /* CS链路差 */
    PHY_WCDMA_CS_ERROR_NO_UL_DATA   = 1,                        /* 无上行语音数据 */
    PHY_WCDMA_CS_ERROR_REASON_BUTT
};

typedef VOS_UINT16 PHY_WCDMA_CS_ERROR_REASON_ENUM_UINT16;
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



/*******************************************************************************
 结构名    : PHY_WCDMA_ERRLOG_FG_INFO_STRU
 结构说明  : WCDMA生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/
typedef struct
{
    VOS_UINT32                                  ulFgPos;
    VOS_UINT32                                  ulFgPower;
}PHY_WCDMA_ERRLOG_FG_INFO_STRU;


/*******************************************************************************
 结构名    : PHY_ERRLOG_WCDMA_RL_OUT_OF_SYNC_EVENT_STRU
 结构说明  : WCDMA生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                      stHeader;
    PHY_WCDMA_RL_OUT_OF_SYNC_REASON_ENUM_UINT16 enRlOutOfSyncReason;
    VOS_INT16                                   sTxPwr;
    VOS_INT16                                   sCQI;
    VOS_INT16                                   sRsv;
    VOS_UINT32                                  ulDemodSyncInfo;
    VOS_UINT32                                  ulDecSyncInfo;
    VOS_UINT32                                  ulDecOutSyncInfo;
    VOS_UINT16                                  usFgEn;
    VOS_UINT16                                  usFgLock;
    PHY_WCDMA_ERRLOG_FG_INFO_STRU               stFgInfo;
}PHY_ERRLOG_WCDMA_RL_OUT_OF_SYNC_EVENT_STRU;



/*******************************************************************************
 结构名    : PHY_ERRLOG_WCDMA_RL_FAIL_EVENT_STRU
 结构说明  : WCDMA生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/

typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_WCDMA_BCCH_FAIL_REASON_ENUM_UINT16  enBcchFailReason;
    VOS_INT16                               sRsv;
    VOS_UINT16                              usFgEn;
    VOS_UINT16                              usFgLock;
    PHY_WCDMA_ERRLOG_FG_INFO_STRU           astFgInfo[3];
    VOS_UINT16                              usBcchCrcErrNum;
    VOS_UINT16                              usBgSchdlFailNum;
}PHY_ERRLOG_WCDMA_BCCH_FAIL_EVENT_STRU;




/*******************************************************************************
 结构名    : PHY_ERRLOG_WCDMA_RL_FAIL_EVENT_STRU
 结构说明  : WCDMA生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/

typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    VOS_INT16                               sTxPwr;
    VOS_INT16                               sCQI;
    VOS_UINT32                              ulDemodSyncInfo;
    VOS_UINT32                              ulDecSyncInfo;
    VOS_UINT32                              ulDecOutSyncInfo;
    VOS_UINT16                              usFgEn;
    VOS_UINT16                              usFgLock;
    PHY_WCDMA_ERRLOG_FG_INFO_STRU           stFgInfo;
}PHY_ERRLOG_WCDMA_RL_FAIL_EVENT_STRU;





/*******************************************************************************
 结构名    : PHY_ERRLOG_GSM_DECODE_FAIL_EVENT_STRU
 结构说明  : GSM生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_GSM_RPT_REASON_ENUM_UINT16          enRptReason;
    VOS_UINT16                              usArfcn;
    VOS_UINT16                              usRxlev;
    VOS_UINT16                              usDscInitValue;
    VOS_UINT32                              ulCurFn;
    VOS_UINT16                              usAfc;
    VOS_UINT16                              usTxPwr;
    VOS_UINT16                              usCI;
    VOS_INT16                               sToa;
    VOS_UINT32                              ulCarrirIn;
    VOS_UINT32                              ulInterfereIn;
    VOS_UINT32                              ulCarrirOut;
    VOS_UINT32                              ulInterfereOut;
    VOS_UINT32                              ulLsDcValue;
    VOS_UINT32                              ulRemoveDc;
    VOS_UINT32                              ulBurstRemoveDc;
    VOS_UINT32                              aulDemodData[2];
}PHY_ERRLOG_GSM_DECODE_FAIL_EVENT_STRU;




/*******************************************************************************
 结构名    : PHY_ERRLOG_GSM_DECODE_FAIL_EVENT_STRU
 结构说明  : GSM生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    VOS_UINT16                              usArfcn;
    VOS_UINT16                              usAfc;
    VOS_UINT32                              ulCurFn;
    VOS_UINT32                              uwReg5250;
    VOS_UINT32                              uwDrssi;
    VOS_UINT32                              uwFastDrssi;
    VOS_UINT32                              ulLsDcValue;
    VOS_UINT32                              ulRemoveDc;
    VOS_UINT32                              ulBurstRemoveDc;
}PHY_ERRLOG_GSM_RXLEV_MEAS_EVENT_STRU;




/*******************************************************************************
 结构名    : PHY_ERRLOG_GSM_FB_SB_EVENT_STRU
 结构说明  : GSM生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    VOS_UINT16                              usArfcn;
    VOS_UINT16                              usAfc;
    VOS_UINT32                              ulSrchMode;
    VOS_UINT32                              ulRptStatus;
    VOS_UINT32                              ulSbData;
    VOS_UINT32                              ulCurFn;
    VOS_UINT32                              ulReg5250;
    VOS_UINT32                              ulDrssi;
    VOS_UINT32                              ulFastDrssi;
    VOS_UINT32                              ulLsDcValue;
    VOS_UINT32                              ulRemoveDc;
    VOS_UINT32                              ulBurstRemoveDc;
    VOS_UINT32                              aulDemodData[2];
}PHY_ERRLOG_GSM_FB_SB_EVENT_STRU;




/*******************************************************************************
 结构名    : PHY_FTM_WCDMA_DCH_PARA_EVENT_STRU
 结构说明  : WCDMA生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/
typedef struct
{
    OM_FTM_HEADER_STRU                      stHeader;
    VOS_INT16                               sTxPwr;
    VOS_INT16                               sCQI;
    VOS_UINT32                              ulErrBlk;
    VOS_UINT32                              ulDecBlk;
}PHY_FTM_WCDMA_DCH_PARA_EVENT_STRU;


/*******************************************************************************
 结构名    : PHY_FTM_GSM_DCH_PARA_EVENT_STRU
 结构说明  : GSM生成和更新后上报的消息结构体
 1.日    期   : 2013年09月21日
   作    者   : l57356
   修改内容   : 新建
*******************************************************************************/
typedef struct
{
    OM_FTM_HEADER_STRU                      stHeader;
    VOS_UINT16                              usTxPwr;
    VOS_UINT16                              usErrBlk;
    VOS_UINT16                              usDecBlk;
    VOS_UINT16                              usResv;
}PHY_FTM_GSM_DCH_PARA_EVENT_STRU;


typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_GSM_CS_ERROR_REASON_ENUM_UINT16     enRptReason;
    VOS_UINT16                              usChannelType;                      /* 信道类型 */
    VOS_UINT16                              usAssignRxTs;                       /* 业务时隙 */
    VOS_UINT16                              usRxlev;                            /* 信号强度 */
    VOS_UINT16                              usCIAverage;                        /* CI平均值 */
    VOS_UINT16                              usErrBitSub;                        /* SUB 误bit数量 */
    VOS_UINT16                              usTotalBitSub;                      /* SUB bit统计总量 */
    VOS_UINT16                              usResv;
}PHY_ERRLOG_GSM_CS_RLB_EVENT_STRU;


/*******************************************************************************
 结构名    : PHY_ERRLOG_GSM_CS_NO_UL_DATA_EVENT_STRU
 结构说明  : GSM无上行语音数据时上报的消息结构体
 1.日    期   : 2014年7月5日
   作    者   : lmx
   修改内容   : 新建
*******************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_GSM_CS_ERROR_REASON_ENUM_UINT16     enCsErrorReason;
    VOS_UINT16                              usChannelType;
    VOS_UINT16                              usAssignRxTs;
    VOS_UINT16                              usResv;
    VOS_UINT32                              ulFn;                                   /* 帧号 */
    VOS_UINT32                              ulQb;                                   /* QB 0~4999 */
    VOS_UINT32                              ulTimeSlice;                            /* 发送时刻时戳 */
    VOS_UINT16                              usChanCodeTime;                        /* 上行编码时间, 单位:0.1 ms */
    VOS_UINT16                              usChanDecodeTime;
}PHY_ERRLOG_GSM_CS_NO_UL_DATA_EVENT_STRU;



typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_GSM_CS_ERROR_REASON_ENUM_UINT16     enRptReason;
    VOS_UINT16                              usChannelType;                      /* 信道类型 */
    VOS_UINT16                              usAssignRxTs;                       /* 业务时隙 */
    VOS_UINT16                              usResv;
}PHY_ERRLOG_GSM_CS_ERROR_DETECT_EVENT_STRU;



typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    VOS_UINT32                              ulOverflowCnt;                      /* 环形buffer溢出次数 */
}PHY_ERRLOG_RING_BUFFER_OVERFLOW_EVENT_STRU;
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_WCDMA_CS_ERROR_REASON_ENUM_UINT16   enCsErrorReason;                    /* 语音异常告警类型 */
    PHY_UINT16                              uhwChannelType;                     /* 信道类型 */
    PHY_UINT16                              uhwCpichEcN0;                       /* CPICH Ec/N0 */
    PHY_UINT16                              uhwCpichRscp;                       /* CPICH RSCP */
    PHY_UINT16                              uhwSIR;                             /* 语音信道SIR平均值 */
    PHY_UINT16                              uhwCRCErrBlkNum;                    /* CRC错误的语音block数量 */
    PHY_UINT16                              uhwTotalCRCBlkNum;                  /* 语音block统计总量 */
    PHY_UINT16                              uhwErrPilotBitNum;                  /* Pilot bit错误数量 */
    PHY_UINT16                              uhwTotalPilotBitNum;                /* Pilot bit统计总量 */
    PHY_UINT16                              uhwDCHOutOfSyncInd;                 /* DCH out of sync 指示*/
}PHY_ERRLOG_WCDMA_CS_ERROR_RLB_EVENT_STRU;


typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_WCDMA_CS_ERROR_REASON_ENUM_UINT16   enCsErrorReason;                    /* 语音异常告警类型 */
    PHY_UINT16                              uhwChannelType;                     /* 信道类型 */
    PHY_UINT16                              uhwFrameNum;                         /* 最近一次发送语音帧号 */
    PHY_UINT16                              uhwSlotNum;                          /*  最近一次发送语音时隙号*/
    PHY_UINT16                              uhwUlTFCI;                           /*  最近一次发送语音的TFCI值*/
    PHY_UINT16                              uhwReseved;
}PHY_MNTN_WCDMA_CS_ERROR_NO_UL_DATA_EVENT_STRU;
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_INT16                               shwTxPwr;                           /* 当前发射功率，单位0.1dBm */
    PHY_UINT16                              uhwReseved;
}PHY_MNTN_WCDMA_TX_POWER_DROP_EVENT_STRU;
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                  stHeader;
    PHY_UINT16                              uhwPrePilotBer;                     /* 前个窗内的pilot ber，50表示50% */
    PHY_UINT16                              uhwCurrPilotBer;                    /* 后个窗内的pilot ber，10表示10% */
}PHY_MNTN_WCDMA_PILOT_BER_RISE_EVENT_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


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

#endif /* end of phyerrorlog.h */


