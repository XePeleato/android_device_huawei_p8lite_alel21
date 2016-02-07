/******************************************************************************
  Copyright    : 2005-2011, Huawei Tech. Co., Ltd.
  File name    : PsRegRpt.h
  Author       :
  Version      : V700R001
  Date         : 2011-3-23
  Description  :
  Function List:
        ---
        ---
        ---
  History      :
  1. Date:2011-3-23
     Author:
     Modification:Create

******************************************************************************/

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "omrl.h"
#include "product_config.h"

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define ID_OM_TRANS_PRIMID                      (0x5001)

#define ID_OM_APP_W_REG_IND                     (0xF501) /*W模式下上报寄存器内容的消息*/
#define ID_OM_APP_G_REG_IND                     (0xE501) /*G模式下上报寄存器内容的消息*/
#define ID_APP_OM_REG_ADD_REQ                   (0xF502) /*寄存器添加消息，SDT填写，OM透传 */
#define ID_APP_OM_REG_DEL_REQ                   (0xF503) /*寄存器删除消息，SDT填写，OM透传 */
#define ID_OM_APP_REG_ADD_CNF                   (0xF504) /*寄存器添加消息回复，UE填写，OM 透传给PC */
#define ID_OM_APP_REG_DEL_CNF                   (0xF505) /*寄存器删除消息回复，UE填写，OM 透传给PC */

#define ID_APP_OM_REG_START_REQ                 (0xFF01) /* 开始抓寄存器请求 */
#define ID_APP_OM_REG_START_CNF                 (0xFF02) /* 抓寄存器回复 */
#define ID_OM_APP_REG_IND                       (0xFF03) /* 上报寄存器内容的消息 */

#define ID_APP_OM_REG_STOP_REQ                  (0xFF05) /* 停止寄存器上报消息请求 */
#define ID_APP_OM_REG_STOP_CNF                  (0xFF06) /* 停止寄存器上报消息回复 */

#define ID_APP_OM_REG_DIFF_REQ                  (0xFF07) /* 设置差异上报次数请求 */
#define ID_APP_OM_REG_DIFF_CNF                  (0xFF08) /* 设置差异上报次数回复 */

#define PS_REG_RPT_MAX_NUM                      (1024)   /* 寄存器缓冲Buffer的最大个数 */
#define PS_REG_RPT_G_MAX_NUM                    (800)    /* 寄存器缓冲Buffer的最大个数 */

#define PS_REG_RPT_MAX_BITMAP_LEN               (32)     /* 最大BIT MAP长度 1024/32 */

#define PS_REG_RPT_HEAD_NUM                     (1)      /* 寄存器CD Buffer中OM消息头所占CD List的个数 */
#define PS_REG_RPT_CFG_MAX_TIME                 (100)    /* 寄存器配置时最长的配置等待时间 */
#define PS_REG_ADDR_STEP                        (4)      /* 连续2个寄存器地址的差值 */
#define PS_REG_RETRY_MAX_NUM                    (1000)   /* 收到add/del命令时，最大的重试次数 */
#define PS_REG_ADDR_LEN                         (4)      /* 寄存器地址长度 */

#define PS_REG_RPT_RBUF_NUM                     (0x3+1+1+2) /* 因为G模的帧中断是4.615ms,10ms内最多缓存3条寄存器上报消息,环形队列需增加一个单元用来判断是否满，最后一个"1",防止来不及上报 */

#define PS_REG_RPT_FREQ_LEN                     10      /*上报频率为10毫秒*/

#define PS_REG_RPT_BUF_NUM                      (2)     /* 上报缓存buffer数量，乒乓上报 */

#define PS_REG_RPT_SEND_FREQUENCY               (3)     /* 上报帧频:累计三帧上报一帧 */

#define PS_REG_RPT_DIFF_NUM                     (300)   /* 差异上报最大次数:达到该次数后需上报一帧全的 */

#define PS_REG_RPT_DMA_DEST_BUFFER              (PS_REG_RPT_MAX_NUM * sizeof(VOS_UINT32)) /* 最大存放寄存器值目的Buffer */

#define PS_REG_RPT_GET_BITMAP_NUM(n)            ((n)>>5) /* 寄存器个数除32 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum PS_REG_RPT_MODE_ENUM
{
    PS_REG_RPT_MODE_0MS = 0,
    PS_REG_RPT_MODE_TIMER,
    PS_REG_RPT_MODE_G_INT,
    PS_REG_RPT_MODE_G_PLUS_W_INT,
    PS_REG_RPT_MODE_BUTT
};
typedef VOS_UINT8  PS_REG_RPT_MODE_ENUM_UINT8;

#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
enum PS_REG_SYS_MODE_ENUM
{
    PS_REG_SYS_MODE_WCDMA = 0,
    PS_REG_SYS_MODE_GSM,
    PS_REG_SYS_MODE_GSM1,
    PS_REG_SYS_MODE_BUTT
};
#else
enum PS_REG_SYS_MODE_ENUM
{
    PS_REG_SYS_MODE_WCDMA = 0,
    PS_REG_SYS_MODE_GSM,
    PS_REG_SYS_MODE_BUTT
};
#endif
typedef VOS_UINT32  PS_REG_SYS_MODE_ENUM_UINT32;

enum PS_REG_ERR_CODE_ENUM
{
    PS_REG_OK = 0,                  /* 配置成功 */
    PS_REG_MODE_ERR,                /* 模式指示错误 */
    PS_REG_MODE_NUM_ERR,            /* 工具下发模式个数错误 */
    PS_REG_MSG_LEN_ERR,             /* 工具下发请求消息长度错误 */
    PS_REG_NUM_OVERFLOW,            /* 捕获寄存器个数溢出 */
    PS_REG_NUM_ERR,                 /* 捕获寄存器地址个数有误 */
    PS_REG_ALIGN_ERR,               /* 寄存器地址没有4字节对齐 */
    PS_REG_BUSY_ERR,                /* 寄存器忙，配置失败 */
    PS_REG_RPT_NOT_ENABLE,          /* 寄存器上报没有使能 */
    PS_REG_RPT_DMA_INIT_FAIL,       /* 寄存器上报初始化失败 */
    PS_REG_STOP_DAM_FAIL,           /* 寄存器上报停止DMA失败 */
    PS_REG_STOP_ERR,                /* 寄存器上报停止错误 */
    PS_REG_NUM_ZERO,                /* 捕获寄存器个数为0 */
    PS_W_REG_NUM_TOO_LARGE,         /* 寄存器捕获W个数溢出 */
    PS_G_REG_NUM_TOO_LARGE,         /* 寄存器捕获G个数溢出 */
    PS_G1_REG_NUM_TOO_LARGE,        /* 寄存器捕获G1个数溢出 */
    PS_REG_ADDR_NOT_MATCH_MODE,     /* 寄存器地址与MODE不匹配 */
    PS_REG_QUEUE_ERR,               /* 寄存器地址排序错误 */
    PS_REG_ERR_BUTT
};
typedef VOS_UINT32  PS_REG_ERR_CODE_ENUM_UINT32;

enum PS_REG_MSG_END_TAG_ENUM
{
    PS_REG_MSG_TAG_START = 0,
    PS_REG_MSG_TAG_END,
    PS_REG_MSG_TAG_BUFF_BUTT
};
typedef VOS_UINT16  PS_REG_MSG_TAG_ENUM_UINT16;

enum PS_REG_BUFF_STATE_ENUM
{
    PS_REG_BUFF_STATE_READY = 0,                        /* Buffer处于可用状态 */
    PS_REG_BUFF_STATE_USING,                            /* Buffer被帧中断时使用   */
    PS_REG_BUFF_STATE_CFG,                              /* Buffer被配置请求时使用 */
    PS_REG_BUFF_STATE_BUTT
};
typedef VOS_UINT32  PS_REG_BUFF_STATE_ENUM_UINT32;

enum PS_REG_DEL_ENUM
{
    PS_REG_DEL_NO_FOUND = 0,                            /* 没有找到相同的寄存器地址          */
    PS_REG_DEL_ONE_ITEM,                                /* 删除一个寄存器地址                */
    PS_REG_DEL_CHG_LEN,                                 /* 删除时，仅需改变寄存器消息片长度  */
    PS_REG_DEL_SPLIT_TWO_ITEMS,                         /* 删除时，需要拆成两项              */
    PS_REG_DEL_BUTT
};
typedef VOS_UINT32  PS_REG_DEL_ENUM_UINT32;

typedef struct
{
    PS_REG_SYS_MODE_ENUM_UINT32 enSysMode;
    VOS_UINT32                  ulCnt;      /* 上报的寄存器个数 */
    VOS_UINT32                  aulRegValue[PS_REG_RPT_MAX_NUM];
} PS_REG_RPT_VALUE_STRU;

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
    VOS_UINT32                      ulRegAddr;          /* 指向消息片buffer起始地址，字节对齐 */
    VOS_UINT16                      usMsgLen;           /* 消息片长度指示                     */
    PS_REG_MSG_TAG_ENUM_UINT16      usMsgTag;           /* 消息包指针类型                     */
} OM_SOCP_CD_LIST_STRU;

typedef struct
{
    VOS_UINT32                  ulCnt;       /* 上报的寄存器个数 */
    VOS_UINT32                  aulRegValue[PS_REG_RPT_MAX_NUM];
} PS_REG_RPT_CFG_STRU;


typedef struct
{
    VOS_UINT32                  ulCnt;       /* 上报的寄存器个数 */
    VOS_UINT32                  aulRegAddr[PS_REG_RPT_MAX_NUM];
} PS_REG_REQ_ADDR_STRU;

/*===============  PS_REG_RPT_SIMPLE_RPT_MSG  ====================*/
/*======= 此结构是简化模式上报寄存器值，与PS_REG_RPT_MSG区别的是没有对应的地址，地址被PC侧维护 =======*/
typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16                  usPrimId;
    VOS_UINT16                  usToolId;
    VOS_MSG_HEADER
    VOS_UINT16                  usMsgName;  /*目前为保留字段*/
    VOS_UINT16                  usCnt;      /* 上报的寄存器个数 */

}PS_REG_RPT_IND_CONTENT_HEAD;

#define PS_REG_RPT_MAX_FREQUENCY_BUFFER  (sizeof(PS_REG_RPT_IND_CONTENT_HEAD)+(PS_REG_RPT_MAX_BITMAP_LEN*sizeof(VOS_UINT32))+(PS_REG_RPT_MAX_NUM * sizeof(VOS_UINT32)))
typedef struct
{
    OM_APP_HEADER_STRU          stMsgHeader;
    VOS_UINT16                  usPrimId;
    VOS_UINT16                  usToolId;
    VOS_MSG_HEADER
    /* 为缓解OM通道流量，只上报寄存器的值，寄存器对应的地址被由PC端维护 */
    VOS_UINT8                  aucValue[PS_REG_RPT_MAX_FREQUENCY_BUFFER * PS_REG_RPT_SEND_FREQUENCY];
} PS_REG_RPT_IND_MSG;
typedef struct
{
    VOS_BOOL                bRptEnableStatus;
    VOS_BOOL                bEdma0StartStatus;
    VOS_BOOL                bEdma1StartStatus;
    VOS_UINT32              aulSendCount[PS_REG_SYS_MODE_BUTT];/* 记录每个模上报的次数 */
    VOS_UINT32              ulSendBufIndex;                    /* 记录乒乓buf索引 */
    VOS_UINT32              ulDmaDestWriteIndex;               /* 记录DmaDest写索引 */

    VOS_UINT32              aulDmaDestWriteStatus[PS_REG_RPT_BUF_NUM][PS_REG_RPT_SEND_FREQUENCY]; /* 目的buf写标记 */
    VOS_UINT32              aulDmaDestWriteSysMode[PS_REG_RPT_BUF_NUM][PS_REG_RPT_SEND_FREQUENCY]; /* 目的buf对应写模式W/G/G1? */
} PS_REG_RPT_CONTROL_STRU;

/* 给工具发的透传消息 */
typedef struct
{
    OM_APP_HEADER_STRU  stAppHeader;
    VOS_UINT16          usPrimId;
    VOS_UINT16          usToolId;
    VOS_UINT8           aucPara[4];
}OM_APP_TRANS_CNF_STRU;

typedef struct
{
    VOS_UINT32          ulDmaIdleErrTime;
    VOS_UINT32          ulDmaHeadErrTime;
    VOS_UINT32          ulDmaStartErrTime;
    VOS_UINT32          ulDmaSuccTime;
    VOS_UINT32          ulDmaIsrTime;
    VOS_UINT32          ulDmaIsrSuccTime;
    VOS_UINT32          ulDmaIsrErrTime;
    VOS_UINT32          ulDmaTaskTime;
    VOS_UINT32          ulDmaIntState;
    VOS_UINT32          ulMemDmaStartTime;
    VOS_UINT32          ulMemDmaIsrTime;

    VOS_UINT32          ulDmaDestIdleErrTime;
    VOS_UINT32          ulDmaDestBusyErrTime;
    VOS_UINT32          ulChan1ArgErrTime;
    VOS_UINT32          ulModemErrTime;

}DMAREG_INFO_STRU;

typedef struct
{
    VOS_UINT32          ulStartSlice;
    VOS_UINT32          ulDmaStartTime;
    VOS_UINT32          ulSmVTime;
    VOS_UINT32          ulSmVPTime;
    VOS_UINT32          ulSmVMaxTime;
    VOS_UINT32          ulSmPMaxTime;
    VOS_UINT32          ulIsrMaxTime;
    VOS_UINT32          ulMemcpyMaxTime;
    VOS_UINT32          ulSocpMaxTime;
    VOS_UINT32          ulMaxTime;
}DMA_REG_TIME_STRU;
/*===============  OM_REG_RPT_STOP_STRU  ====================*/
/*======= 此结构暂作为停止所有寄存器上报的原语请求 =======*/

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                      usMsgId;
    VOS_UINT16                      usToolsId;
    VOS_UINT32                      ulSysModeNum;
    VOS_UINT32                      ulValueLenth;
    VOS_UINT8                       aucValue[4];
}APP_OM_REG_START_REQ_STRU;


typedef struct
{
    VOS_UINT32                      ulLenth;
    PS_REG_SYS_MODE_ENUM_UINT32     enSysMode;
    VOS_UINT32                      ulRegNum;      /*寄存器个数*/
    VOS_UINT32                      aulRegAddr[1];
}APP_OM_REG_START_MODE_STRU;
typedef struct
{
    PS_REG_SYS_MODE_ENUM_UINT32     enSysMode;
    VOS_UINT32                      ulRegNum;
    VOS_UINT8                       aucAddr[4];
}OM_APP_REG_MODE_NUM_STRU;


typedef struct
{
    VOS_UINT32                      ulErrCode;
    VOS_UINT32                      ulSysModeNum;
    VOS_UINT8                       aucAddr[4];
}OM_APP_REG_START_CNF_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                      usMsgId;
    VOS_UINT16                      usToolsId;
    PS_REG_SYS_MODE_ENUM_UINT32     enSysMode;
}OM_REG_RPT_STOP_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                      usMsgId;
    VOS_UINT16                      usToolsId;
    VOS_UINT32                      ulRptDiffNum;
}OM_REG_RPT_DIFF_STRU;

typedef struct
{
    OM_APP_MSG_SEG_EX_STRU          stOMSocpHeader; /* SOCP头信息 */

    OM_APP_HEADER_STRU              stAppHeader;
    VOS_UINT16                      usPrimId;
    VOS_UINT16                      usToolId;
    VOS_MSG_HEADER
    VOS_UINT16                      usMsgName;  /*目前为保留字段*/
    VOS_UINT16                      usTick;
    VOS_UINT16                      usFreq;     /* 上报频率，以10ms为单位 ，目前为保留字段*/
    PS_REG_RPT_MODE_ENUM_UINT8      enRptMode;  /* 上报模式: 帧中断，或定时器上报 ，目前为保留字段*/
    VOS_UINT8                       ucRsv;
    VOS_UINT32                      ulFN;       /* 在W模表示CFN(只有8bit)，在G模表示FN(共32位) ，目前为保留字段*/
    VOS_UINT16                      usSfn;      /* W模表示SFN, G模不使用 ，目前为保留字段*/
    VOS_UINT16                      usCnt;      /* 上报的寄存器个数 */
} PS_REG_OM_HEADER_MSG;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern PS_REG_RPT_CFG_STRU     g_astPsRegRptCfg[PS_REG_SYS_MODE_BUTT];

extern PS_REG_RPT_VALUE_STRU   g_astPsRegRptBuf[PS_REG_RPT_RBUF_NUM];

extern VOS_UINT32              g_ulPsReg;
/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_UINT32 PsRegNumberGet(PS_REG_SYS_MODE_ENUM_UINT32 enSysMode);
extern VOS_UINT32 PsRegSysModeGet(VOS_UINT32 ulSendBufIndex, VOS_UINT32 ulSendFrequency);
extern VOS_VOID PsRegCapture(VOS_UINT32 ulSysMode);
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

