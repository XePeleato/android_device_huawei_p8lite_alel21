/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : SOCPInterface.h
  版 本 号      : 初稿
  作    者      : zhuli
  生成日期      : 2012年8月3日
  最近修改      :
  功能描述      : 该头文件为SOCP API的头文件，主要提供给外部部件使用
  函数列表      :
  修改历史      :
  1.日    期    : 2012年8月3日
    作    者    : zhuli
    修改内容    : 创建文件

******************************************************************************/
#ifndef _SOCP_INTERFACE_H_
#define _SOCP_INTERFACE_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "product_config.h"

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
#include "mdrv.h"
#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/**************************************************************************
  2 宏定义
**************************************************************************/
#define SOCP_CODER_SRC_CHAN                 0x00
#define SOCP_CODER_DEST_CHAN                0x01
#define SOCP_DECODER_SRC_CHAN               0x02
#define SOCP_DECODER_DEST_CHAN              0x03

#define SOCP_CHAN_DEF(chan_type, chan_id)   ((chan_type<<16)|chan_id)
#define SOCP_REAL_CHAN_ID(unique_chan_id)   (unique_chan_id & 0xFFFF)
#define SOCP_REAL_CHAN_TYPE(unique_chan_id) (unique_chan_id>>16)

/**************************************************************************
  3 枚举定义
**************************************************************************/
#if ((FEATURE_ON == FEATURE_SOCP_CHANNEL_REDUCE)&&(VOS_OS_VER != VOS_WIN32))
/*编码源通道ID枚举定义*/
enum SOCP_CODER_SRC_ENUM
{
    SOCP_CODER_SRC_GU_CNF1      = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,0),  /* GU OM诊断消息 */
    SOCP_CODER_SRC_GU_CNF2      = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,1),  /* GU OM诊断消息 */
    SOCP_CODER_SRC_GUBBP1       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,2), /* GUBBP诊断消息 */
    SOCP_CODER_SRC_GUBBP2       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,3), /* GUBBP诊断消息 */
    SOCP_CODER_SRC_GUDSP1       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,4), /* GUDSP诊断消息 */
    SOCP_CODER_SRC_GUDSP2       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,5), /* GUDSP诊断消息 */
    SOCP_CODER_SRC_GU_IND1      = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,6), /* 媒体诊断消息 */
    SOCP_CODER_SRC_GU_IND2      = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,7), /* 媒体诊断消息 */
    SOCP_CODER_SRC_BUTT
};
#else
/*编码源通道ID枚举定义*/
enum SOCP_CODER_SRC_ENUM
{
    SOCP_CODER_SRC_LOM_CNF1     = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,0),  /* LTE OM命令响应(A核) */
    SOCP_CODER_SRC_HDLCAT_CNF   = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,1),  /* HDLC流式AT命令响应(A核) */
    SOCP_CODER_SRC_LOM_IND1     = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,2),  /* LTE OM诊断消息(A核) */
    SOCP_CODER_SRC_GU_CNF1      = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,3),  /* GU OM命令响应(A核) */
    SOCP_CODER_SRC_LOM_IND2     = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,4),  /* LTE OM诊断消息环形(C核) */
    SOCP_CODER_SRC_LOM_IND3     = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,5),  /* LTE OM诊断消息环形(C核) */
    SOCP_CODER_SRC_LOM_CNF2     = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,6),  /* LTE OM命令响应(C核) */
    SOCP_CODER_SRC_GU_CNF2      = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,7),  /* GU OM命令响应(C核) */
    SOCP_CODER_SRC_RFU          = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,8),  /* 保留 */
    SOCP_CODER_SRC_HIFI         = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,9),  /* GU HIFI诊断消息 */
    SOCP_CODER_SRC_GU_IND1      = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,10), /* GU OM诊断消息(A核) */
    SOCP_CODER_SRC_GU_IND2      = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,11), /* GU OM诊断消息(C核) */
    SOCP_CODER_SRC_MCU1         = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,12), /* MCU诊断消息 */
    SOCP_CODER_SRC_MCU2         = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,13), /* MCU诊断消息 */
    SOCP_CODER_SRC_LDSP1        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,14), /* LDSP诊断消息 */
    SOCP_CODER_SRC_LDSP2        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,15), /* LDSP诊断消息 */
    SOCP_CODER_SRC_LBBP1        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,16), /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP2        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,17), /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP3        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,18), /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP4        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,19), /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP5        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,20), /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP6        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,21), /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP7        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,22), /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP8        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,23), /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP9        = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,24), /* LBBP诊断消息 */
    SOCP_CODER_SRC_GUBBP1       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,25), /* GUBBP诊断消息 */
    SOCP_CODER_SRC_GUBBP2       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,26), /* GUBBP诊断消息 */
    SOCP_CODER_SRC_GUDSP1       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,27), /* GUDSP诊断消息 */
    SOCP_CODER_SRC_GUDSP2       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,28), /* GUDSP诊断消息 */
    SOCP_CODER_SRC_TDDSP1       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,29), /* TDDSP诊断消息 */
    SOCP_CODER_SRC_TDBBP1       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,30), /* TDBBP诊断消息 */
    SOCP_CODER_SRC_TDBBP2       = SOCP_CHAN_DEF(SOCP_CODER_SRC_CHAN,31), /* TDBBP诊断消息 */
    SOCP_CODER_SRC_BUTT
};
#endif
typedef VOS_UINT32 SOCP_CODER_SRC_ENUM_U32;

/*解码源通道ID枚举定义*/
enum SOCP_DECODER_SRC_ENUM
{
    SOCP_DECODER_SRC_LOM        = SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN,0),   /* LTE OM命令 */
    SOCP_DECODER_SRC_HDLC_AT    = SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN,1),   /* HDLC编码的流式AT命令 */
    SOCP_DECODER_SRC_GUOM       = SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN,2),   /* GU OM命令 */
    SOCP_DECODER_SRC_RFU        = SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN,3),   /* 保留 */
    SOCP_DECODER_SRC_BUTT
};
typedef VOS_UINT32 SOCP_DECODER_SRC_ENUM_U32;

#if ((FEATURE_ON == FEATURE_SOCP_CHANNEL_REDUCE)&&(VOS_OS_VER != VOS_WIN32))
/*编码目的通道ID枚举定义*/
enum SOCP_CODER_DST_ENUM
{
    SOCP_CODER_DST_OM_CNF        = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,0), /* GU OM目的通道 */
    SOCP_CODER_DST_OM_IND        = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,1), /* OM命令上报输出 */
    SOCP_CODER_DST_BUTT
};
#else
/*编码目的通道ID枚举定义*/
enum SOCP_CODER_DST_ENUM
{
    SOCP_CODER_DST_OM_CNF        = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,0), /* OM命令响应输出 */
    SOCP_CODER_DST_OM_IND        = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,1), /* OM命令上报输出 */
    SOCP_CODER_DST_HDLC_AT       = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,2), /* 流式AT命令响应输出,目前北京不再使用 */
    SOCP_CODER_DST_RFU0          = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,3), /* GU OM目的通道 */
    SOCP_CODER_DST_RFU1          = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,4), /* 保留 */
    SOCP_CODER_DST_RFU2          = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,5), /* 保留 */
    SOCP_CODER_DST_RFU3          = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN,6), /* 保留 */
    SOCP_CODER_DST_BUTT
};
#endif
typedef VOS_UINT32 SOCP_CODER_DST_ENUM_U32;

/*解码目的通道ID枚举定义,如果有新的解码目的通道，在SOCP_DECODER_DST_RFU之后添加*/
enum SOCP_DECODER_DST_ENUM
{
    SOCP_DECODER_DST_LOM        = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN,0),  /* LTE OM命令 */
    SOCP_DECODER_DST_HDLC_AT    = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN,1),  /* HDLC编码的流式AT命令 */
    SOCP_DECODER_DST_GUOM       = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN,2),  /* GU OM命令 */
    SOCP_DECODER_DST_RFU        = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN,3),  /* 保留 */
    SOCP_DECODER_CBT            = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN,16), /* GU CBT */
    SOCP_DECODER_DST_BUTT
};
typedef VOS_UINT32 SOCP_DECODER_DST_ENUM_U32;

/*错误码定义*/
enum SOCP_ERR_ENUM
{
    BSP_ERR_SOCP_BASE           = VOS_OK,
    BSP_ERR_SOCP_NULL,
    BSP_ERR_SOCP_NOT_INIT,
    BSP_ERR_SOCP_MEM_ALLOC,
    BSP_ERR_SOCP_SEM_CREATE,
    BSP_ERR_SOCP_TSK_CREATE,
    BSP_ERR_SOCP_INVALID_CHAN,
    BSP_ERR_SOCP_INVALID_PARA,
    BSP_ERR_SOCP_NO_CHAN,
    BSP_ERR_SOCP_SET_FAIL,
    BSP_ERR_SOCP_TIMEOUT,
    BSP_ERR_SOCP_NOT_8BYTESALIGN,
    BSP_ERR_SOCP_CHAN_RUNNING,
    BSP_ERR_SOCP_CHAN_MODE,
    BSP_ERR_SOCP_DEST_CHAN,
    BSP_ERR_SOCP_DECSRC_SET,
    BSP_ERR_SOCP_BUTT
};

#define SOCP_MAX_ENCSRC_CHN         (SOCP_REAL_CHAN_ID(SOCP_CODER_SRC_BUTT))
#define SOCP_MAX_ENCDST_CHN         (SOCP_REAL_CHAN_ID(SOCP_CODER_DST_BUTT))
#define SOCP_MAX_DECSRC_CHN         (SOCP_REAL_CHAN_ID(SOCP_DECODER_SRC_BUTT))
#define SOCP_MAX_DECDST_CHN         (SOCP_REAL_CHAN_ID(SOCP_DECODER_DST_BUTT))


enum SOCP_BD_TYPE_ENUM
{
    SOCP_BD_DATA            = 0,    /* SOCP BD数据空间使用实际数据 */
    SOCP_BD_LIST            = 1,    /* SOCP BD数据空间使用实际数据的指针 */
    SOCP_BD_BUTT
};
typedef VOS_UINT16 SOCP_BD_TYPE_ENUM_UINT16;

enum SOCP_HDLC_FLAG_ENUM
{
    SOCP_HDLC_ENABLE         = 0,    /* 当前HDLC功能打开 */
    SOCP_HDLC_DISABLE        = 1,    /* 当前HDLC功能关闭 */
    SOCP_HDLC_FLAG_BUTT
};

enum SOCP_STATE_ENUM
{
    SOCP_IDLE               = 0,    /* SOCP处于空闲态 */
    SOCP_BUSY,                      /* SOCP处忙 */
    SOCP_UNKNOWN_BUTT,              /*  未知状态 */
};
typedef VOS_UINT32 SOCP_STATE_ENUM_UIN32;

enum SOCP_EVENT_ENUM
{
    SOCP_EVENT_PKT_HEADER_ERROR         = 0x1,    /* 包头检测错误，"HISI" */
    SOCP_EVENT_OUTBUFFER_OVERFLOW       = 0x2,    /* 目的buffer上溢 */
    SOCP_EVENT_RDBUFFER_OVERFLOW        = 0x4,    /* RDbuffer上溢 */
    SOCP_EVENT_DECODER_UNDERFLOW        = 0x8,    /* 解码源buffer下溢 */
    SOCP_EVENT_PKT_LENGTH_ERROR         = 0x10,   /* 解码包长度检测错误 */
    SOCP_EVENT_CRC_ERROR                = 0x20,   /* 解码CRC校验错误 */
    SOCP_EVENT_DATA_TYPE_ERROR          = 0x40,   /* 解码数据类型错误*/
    SOCP_EVENT_HDLC_HEADER_ERROR        = 0x80,   /* 解码HDLC检测错误 */
    SOCP_EVENT_OUTBUFFER_THRESHOLD_OVERFLOW = 0x100,/* 目的buffer阀值上溢 */
    SOCP_EVENT_BUTT
};
typedef VOS_UINT32 SOCP_EVENT_ENUM_UIN32;

/* 编码源的数据结构模式 */
enum SOCP_ENCSRC_CHNMODE_ENUM
{
    SOCP_ENCSRC_CHNMODE_CTSPACKET       = 0,    /* 连续数据包环形缓冲区 */
    SOCP_ENCSRC_CHNMODE_FIXPACKET,              /* 固定长度数据包环形缓冲区 */
    SOCP_ENCSRC_CHNMODE_LIST,                   /* 链式环形缓冲区 */
    SOCP_ENCSRC_CHNMODE_BUTT
};
typedef VOS_UINT32 SOCP_ENCSRC_CHNMODE_ENUM_UIN32;

/* 解码源的数据结构模式 */
enum SOCP_DECSRC_CHNMODE_ENUM
{
    SOCP_DECSRC_CHNMODE_BYTES        = 0,       /* 连续字节环形缓冲区 */
    SOCP_DECSRC_CHNMODE_LIST,                   /* 链式环形缓冲区 */
    SOCP_DECSRC_CHNMODE_BUTT
};
typedef VOS_UINT32 SOCP_DECSRC_CHNMODE_ENUM_UIN32;

/* 超时选择及使能数据结构体 */
enum SOCP_TIMEOUT_EN_ENUM
{
    SOCP_TIMEOUT_BUFOVF_DISABLE        = 0,       /* buffer溢出，不上报中断 */
    SOCP_TIMEOUT_BUFOVF_ENABLE,                   /* buffer溢出，超时计数上报中断 */
    SOCP_TIMEOUT_TRF,                             /* 传输中断超时计数 */
    SOCP_TIMEOUT_BUTT
};
typedef VOS_UINT32 SOCP_TIMEOUT_EN_ENUM_UIN32;

/* 同一类型的通道不同优先级枚举值*/
enum SOCP_CHAN_PRIORITY_ENUM
{
    SOCP_CHAN_PRIORITY_0     = 0,               /* 最低优先级*/
    SOCP_CHAN_PRIORITY_1,                       /* 次低优先级*/
    SOCP_CHAN_PRIORITY_2,                       /* 次高优先级*/
    SOCP_CHAN_PRIORITY_3,                       /* 最高优先级*/
    SOCP_CHAN_PRIORITY_BUTT
};
typedef VOS_UINT32 SOCP_CHAN_PRIORITY_ENUM_UIN32;

/* 数据类型枚举值*/
enum SOCP_DATA_TYPE_ENUM
{
    SOCP_DATA_TYPE_0            = 0,            /* LTE OAM数据 */
    SOCP_DATA_TYPE_1,                           /* GU OAM数据 */
    SOCP_DATA_TYPE_2,                           /* 保留 */
    SOCP_DATA_TYPE_3,                           /* 保留 */
    SOCP_DATA_TYPE_BUTT
};
typedef VOS_UINT32 SOCP_DATA_TYPE_ENUM_UIN32;

/* BBP 数采模式，数据丢弃或覆盖 */
enum SOCP_BBP_DS_MODE_ENUM
{
    SOCP_BBP_DS_MODE_DROP           = 0,        /* 数据丢弃 */
    SOCP_BBP_DS_MODE_OVERRIDE,                  /* 数据覆盖 */
    SOCP_BBP_DS_MODE_BUTT
};
typedef VOS_UINT32 SOCP_BBP_DS_MODE_ENUM_UIN32;

/* 编码源通道data type 使能位 */
enum SOCP_DATA_TYPE_EN_ENUM
{
    SOCP_DATA_TYPE_EN           = 0,        /* data type 使能，默认值 */
    SOCP_DATA_TYPE_DIS,                     /* data type 不使能 */
    SOCP_DATA_TYPE_EN_BUTT
};
typedef VOS_UINT32 SOCP_DATA_TYPE_EN_ENUM_UIN32;

/* 编码源通道debug 使能位 */
enum SOCP_ENC_DEBUG_EN_ENUM
{
    SOCP_ENC_DEBUG_DIS          = 0,       /* debug 不使能，默认值 */
    SOCP_ENC_DEBUG_EN,                     /* debug 使能 */
    SOCP_ENC_DEBUG_EN_BUTT
};
typedef VOS_UINT32 SOCP_ENC_DEBUG_EN_ENUM_UIN32;

/* 数据类型枚举值*/
enum SCM_DATA_TYPE_ENUM
{
    SCM_DATA_TYPE_TL            = 0,            /* LTE OAM数据 */
    SCM_DATA_TYPE_GU,                           /* GU OAM数据 */
    SCM_DATA_TYPE_BUTT
};
typedef VOS_UINT8 SOCP_DATA_TYPE_ENUM_UIN8;

/**************************************************************************
  3 结构定义
**************************************************************************/
/*用于SOCP通道的BD数据片结构*/
typedef struct
{
    VOS_UINT32                      ulDataAddr;     /* 数据的指针 */
    VOS_UINT16                      usMsgLen;       /* 数据长度 */
    SOCP_BD_TYPE_ENUM_UINT16        enDataType;     /* 实际数据类型，是链表还是真实数据 */
}SOCP_BD_DATA_STRU;

/*用于SOCP通道的RD数据片结构*/
typedef struct
{
    VOS_UINT32                      ulDataAddr;     /*数据指针*/
    VOS_UINT16                      usMsgLen;       /*数据长度*/
    SOCP_BD_TYPE_ENUM_UINT16        enDataType;     /*实际数据类型，是链表还是真实数据*/
}SOCP_RD_DATA_STRU;

/* 解码通路包长配置结构体 */
typedef struct
{
    VOS_UINT32                 u32PktMax;         /*包长度最大值*/
    VOS_UINT32                 u32PktMin;         /*包长度最小值*/
}SOCP_DEC_PKTLGTH_STRU;


/* 通用源通道buffer结构体定义*/
typedef struct
{
    VOS_UINT8                 *pucInputStart;      /* 输入通道起始地址*/
    VOS_UINT8                 *pucInputEnd;        /* 输入通道结束地址*/
    VOS_UINT8                 *pucRDStart;         /* RD buffer起始地址*/
    VOS_UINT8                 *pucRDEnd;           /* RD buffer结束地址*/
    VOS_UINT32                 u32RDThreshold;     /* RD buffer数据上报阈值*/
    VOS_UINT32                 ulRsv;              /* reserve */
}SOCP_SRC_SETBUF_STRU;

/* 通用目的通道buffer结构体定义*/
typedef struct
{
    VOS_UINT8                 *pucOutputStart;     /* 输出通道起始地址*/
    VOS_UINT8                 *pucOutputEnd;       /* 输出通道结束地址*/
    VOS_UINT32                 u32Threshold;       /* 输出通道门限值 */
    VOS_UINT32                 ulRsv;              /* reserve */
}SOCP_DST_SETBUF_STRU;

/* 编码源通道结构体定义*/
typedef struct
{
    VOS_UINT32                      u32DestChanID;      /* 目标通道ID*/
    VOS_UINT32                      u32BypassEn;        /* 通道bypass使能*/
    SOCP_DATA_TYPE_ENUM_UIN32       eDataType;          /* 数据类型，指明数据封装协议，用于复用多平台*/
    SOCP_DATA_TYPE_EN_ENUM_UIN32    eDataTypeEn;        /* 数据类型使能位*/
    SOCP_ENC_DEBUG_EN_ENUM_UIN32    eDebugEn;           /* 调试位使能*/
    SOCP_ENCSRC_CHNMODE_ENUM_UIN32  eMode;              /* 通道数据模式*/
    SOCP_CHAN_PRIORITY_ENUM_UIN32   ePriority;          /* 通道优先级*/
    SOCP_SRC_SETBUF_STRU            sCoderSetSrcBuf;
}SOCP_CODER_SRC_CHAN_S;

/* 编码目的配置结构体定义*/
typedef struct
{
    VOS_UINT32                 u32EncDstThrh;     /* 编码目的通道阈值门限，仲裁通道时使用*/
    SOCP_DST_SETBUF_STRU       sCoderSetDstBuf;
}SOCP_CODER_DEST_CHAN_S;

/* 解码源通道结构体定义*/
typedef struct
{
    SOCP_DATA_TYPE_EN_ENUM_UIN32    eDataTypeEn;        /* 数据类型使能位*/
    SOCP_DECSRC_CHNMODE_ENUM_UIN32  eMode;              /* 通道模式*/
    SOCP_SRC_SETBUF_STRU            sDecoderSetSrcBuf;
}SOCP_DECODER_SRC_CHAN_STRU;

/* 解码目的通道结构体定义*/
typedef struct
{
    VOS_UINT32                 u32SrcChanID;       /* 数据来源通道ID*/
    SOCP_DATA_TYPE_ENUM_UIN32  eDataType;          /* 数据类型，指明数据封装协议，用于复用多平台*/
    SOCP_DST_SETBUF_STRU       sDecoderDstSetBuf;
}SOCP_DECODER_DEST_CHAN_STRU;

/* 通用buffer描述结构体定义*/
typedef struct
{
    VOS_UINT8       *pBuffer;                        /* buffer指针*/
    VOS_UINT8       *pRbBuffer;                      /* 回卷buffer指针*/
    VOS_UINT32      u32Size;                        /* 可用buffer大小*/
    VOS_UINT32      u32RbSize;                      /* 回卷buffer大小*/
}SOCP_BUFFER_RW_STRU;

/* 解码源通道错误计数结构体定义*/
typedef struct
{
    VOS_UINT32     u32PktlengthCnt;                /* 包长检测错误计数*/
    VOS_UINT32     u32CrcCnt;                      /* CRC校验错误计数*/
    VOS_UINT32     u32DataTypeCnt;                 /* 解码数据类型检验错误计数*/
    VOS_UINT32     u32HdlcHeaderCnt;               /* 0x7E校验错误*/
}SOCP_DECODER_ERROR_CNT_STRU;

/* 解码通路包长配置结构体 */
typedef struct
{
    VOS_UINT32                 u32RsvIDMin;        /*编码源保留通道ID最小值*/
    VOS_UINT32                 u32RsvIDMax;        /*编码源保留通道ID最大值*/
}SOCP_ENCSRC_RSVCHN_SCOPE_STRU;

typedef VOS_UINT32 (*socp_event_cb)(VOS_UINT32 u32ChanID, SOCP_EVENT_ENUM_UIN32 u32Event, VOS_UINT32 u32Param);
typedef VOS_UINT32 (*socp_read_cb)(VOS_UINT32 u32ChanID);
typedef VOS_UINT32 (*socp_rd_cb)(VOS_UINT32 u32ChanID);

#define INNER_LOG_DATA_MAX                   0x40

typedef struct
{
    VOS_UINT32                  ulSlice;
    VOS_UINT32                  ulFileNO;
    VOS_INT32                   lLineNO;
    VOS_UINT32                  ulP1;
    VOS_UINT32                  ulP2;
}INNER_LOG_RECORD_STRU;

typedef struct
{
    VOS_UINT32                  ulCnt;
    INNER_LOG_RECORD_STRU       astLogData[INNER_LOG_DATA_MAX];
}INNER_LOG_DATA_STRU;

/**************************************************************************
  函数声明
**************************************************************************/
extern VOS_VOID OM_Inner_Log(const VOS_CHAR *pucStr, VOS_UINT32 ulP1, VOS_UINT32 ulP2,
                          INNER_LOG_DATA_STRU *pstLogData ,VOS_UINT32 ulFileID, VOS_INT32 lLineNo);

#define OM_INNER_LOG( pucStr, ulP1, ulP2, pstLogData )\
    OM_Inner_Log( pucStr, ulP1, ulP2, pstLogData, VOS_FILE_ID, __LINE__ )

/*****************************************************************************
 函 数 名  : mdrv_socp_corder_set_src_chan
 功能描述  : 此接口完成某一编码源通道的配置，返回函数执行的结果。
 输入参数  : enSrcChanID:SOCP编码器的目标通道ID。
             pDestAttr:SOCP编码器目标通道参数结构体指针。
 输出参数  : 无。
 返 回 值  : MDRV_OK:编码目的通道设置成功。
             other:编码目的通道设置失败错误码。
*****************************************************************************/
VOS_UINT32 BSP_SOCP_CoderSetSrcChan(SOCP_CODER_SRC_ENUM_U32 enSrcChanID,SOCP_CODER_SRC_CHAN_S *pSrcAttr);
#define mdrv_socp_corder_set_src_chan(enSrcChanID, pSrcAttr) \
            BSP_SOCP_CoderSetSrcChan(enSrcChanID, pSrcAttr)

/*****************************************************************************
 函 数 名  : mdrv_socp_coder_set_dest_chan_attr
 功能描述  : 此接口完成某一编码目标通道的配置，返回函数执行的结果。
 输入参数  : u32DestChanID:SOCP编码器的目标通道ID。
             pDestAttr:SOCP编码器目标通道参数结构体指针。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码目的通道设置成功。
             SOCP_ERROR:编码目的通道设置失败。
*****************************************************************************/
VOS_UINT32 BSP_SOCP_CoderSetDestChanAttr(VOS_UINT32 u32DestChanID,SOCP_CODER_DEST_CHAN_S *pDestAttr);
#define mdrv_socp_coder_set_dest_chan_attr(u32DestChanID, pDestAttr) \
            BSP_SOCP_CoderSetDestChanAttr(u32DestChanID, pDestAttr)

/*****************************************************************************
 函 数 名  : BSP_SOCP_Start
 功能描述  : 此接口用于源通道，启动编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码启动成功。
             SOCP_ERROR:编码或解码启动失败。
*****************************************************************************/
VOS_UINT32 BSP_SOCP_Start(VOS_UINT32 u32SrcChanID);
#define mdrv_socp_start(u32SrcChanID) \
            BSP_SOCP_Start(u32SrcChanID)

/*****************************************************************************
 函 数 名  : BSP_SOCP_Stop
 功能描述  : 此接口用于源通道，停止编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码停止成功。
             SOCP_ERROR:编码或解码停止失败。
*****************************************************************************/
VOS_UINT32 BSP_SOCP_Stop(VOS_UINT32 u32SrcChanID);
#define mdrv_socp_stop(u32SrcChanID) \
            BSP_SOCP_Stop(u32SrcChanID)

/*****************************************************************************
 函 数 名      : BSP_SOCP_GetWriteBuff
 功能描述  :此接口用于获取写数据buffer。
 输入参数  : u32SrcChanID:源通道ID
                  pBuff:           :写数据buffer

 输出参数  : 无。
 返 回 值      : SOCP_OK:获取写数据buffer成功。
                             SOCP_ERROR:获取写数据buffer失败
*****************************************************************************/
VOS_UINT32 BSP_SOCP_GetWriteBuff(VOS_UINT32 u32SrcChanID, SOCP_BUFFER_RW_STRU *pBuff);
#define mdrv_socp_get_write_buff(u32SrcChanID, pBuff) \
            BSP_SOCP_GetWriteBuff( u32SrcChanID, pBuff)

/*****************************************************************************
 函 数 名      : BSP_SOCP_WriteDone
 功能描述  :该接口用于数据的写操作，提供写入数据的长度。
 输入参数  : u32SrcChanID:源通道ID
                  u32WrtSize:   已写入数据的长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:写入数据成功。
                             SOCP_ERROR:写入数据失败
*****************************************************************************/
VOS_UINT32 BSP_SOCP_WriteDone(VOS_UINT32 u32SrcChanID, VOS_UINT32 u32WrtSize);
#define mdrv_socp_write_done(u32SrcChanID, u32WrtSize) \
            BSP_SOCP_WriteDone(u32SrcChanID, u32WrtSize)

/*****************************************************************************
 函 数 名      : BSP_SOCP_GetRDBuffer
 功能描述  :该此接口用于获取RD buffer的数据指针。
 输入参数  : u32SrcChanID:源通道ID
                  pBuff:  RD buffer
 输出参数  : 无。
 返 回 值      : SOCP_OK:获取RD环形缓冲区成功
                             SOCP_ERROR:获取RD环形缓冲区失败
*****************************************************************************/
VOS_UINT32 BSP_SOCP_GetRDBuffer(VOS_UINT32 u32SrcChanID, SOCP_BUFFER_RW_STRU *pBuff);
#define mdrv_socp_get_rd_buffer(u32SrcChanID, pBuff) \
            BSP_SOCP_GetRDBuffer( u32SrcChanID, pBuff)

/*****************************************************************************
 函 数 名      : BSP_SOCP_ReadRDDone
 功能描述  :此接口用于上层通知SOCP驱动，从RD buffer中实际读取的数据。
 输入参数  : u32SrcChanID:源通道ID
                  u32RDSize:  从RD buffer中实际读取的数据长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:读取RDbuffer中的数据成功
                             SOCP_ERROR:读取RDbuffer中的数据失败
*****************************************************************************/
VOS_UINT32 BSP_SOCP_ReadRDDone(VOS_UINT32 u32SrcChanID, VOS_UINT32 u32RDSize);
#define mdrv_socp_read_rd_done(u32SrcChanID, u32RDSize) \
            BSP_SOCP_ReadRDDone(u32SrcChanID, u32RDSize)

/*****************************************************************************
 函 数 名      : BSP_SOCP_RegisterReadCB
 功能描述  :该接口用于注册读数据的回调函数。
 输入参数  : u32DestChanID:目标通道ID
                  ReadCB: 事件回调函数
 输出参数  : 无。
 返 回 值      : SOCP_OK:注册读数据回调函数成功
                             SOCP_ERROR:注册读数据回调函数失败
*****************************************************************************/
extern VOS_UINT32 BSP_SOCP_RegisterReadCB( VOS_UINT32 u32DestChanID, socp_read_cb ReadCB);

/*****************************************************************************
 函 数 名      : BSP_SOCP_RegisterReadCB
 功能描述  :该此接口用于获取读数据缓冲区指针。
 输入参数  : u32DestChanID:目标通道ID
                  ReadCB: 读数据buffer
 输出参数  : 无。
 返 回 值      : SOCP_OK:获取读数据缓冲区成功。
                             SOCP_ERROR:获取读数据缓冲区成功。
*****************************************************************************/
extern VOS_UINT32 BSP_SOCP_GetReadBuff( VOS_UINT32 u32DestChanID,SOCP_BUFFER_RW_STRU *pBuffer);

/*****************************************************************************
 函 数 名      : BSP_SOCP_ReadDataDone
 功能描述  :该接口用于上层告诉SOCP驱动，从目标通道中读走的实际数据。
 输入参数  : u32DestChanID:目标通道ID
                  u32ReadSize: 已读出数据的长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:读数据成功。
                             SOCP_ERROR:读数据失败
*****************************************************************************/
extern VOS_UINT32 BSP_SOCP_ReadDataDone(VOS_UINT32 u32DestChanID,VOS_UINT32 u32ReadSize);

/*****************************************************************************
 函 数 名  : SOCP_CleanEncSrcChan
 功能描述  :该接口用于上层告诉SOCP驱动，清空SOCP的编码源通道的配置寄存器。
 输入参数  : u32SrcChanID:通道ID
 输出参数  : 无。
 返 回 值  : VOS_OK:成功
             VOS_ERR:失败
*****************************************************************************/
extern VOS_UINT32 SOCP_CleanEncSrcChan(SOCP_CODER_SRC_ENUM_U32 enSrcChanID);

extern VOS_VOID BSP_SOCP_StartSDLogTimerFail(VOS_VOID);

VOS_UINT32 BSP_SOCP_EncDstBufFlush(VOS_VOID);
#define mdrv_socp_enc_dst_buf_flush() \
            BSP_SOCP_EncDstBufFlush()

VOS_UINT32 BSP_SOCP_GetSDLogCfg(VOS_UINT32 *pulEnable, VOS_UINT32 *pulTimerLen, VOS_UINT32 *pulBufferSize);
#define mdrv_socp_get_sd_logcfg(pulEnable, pulTimerLen, pulBufferSize) \
            BSP_SOCP_GetSDLogCfg(pulEnable, pulTimerLen, pulBufferSize)

VOS_VOID BSP_SOCP_SetLogIntTimeOut(VOS_VOID);
#define mdrv_socp_set_log_int_timeout() \
            BSP_SOCP_SetLogIntTimeOut()

VOS_VOID BSP_SOCP_SetLogIntTimeOutDefautl(VOS_VOID);
#define mdrv_socp_set_log_int_default_timeout() \
            BSP_SOCP_SetLogIntTimeOutDefautl()

extern VOS_VOID BSP_SOCP_GetEncSrcReg(VOS_UINT32 *pulData, VOS_UINT32 ulDataMax);

/* C核单独复位项目增加,j00174725 2013-05-02 */
extern VOS_VOID SOCP_Stop1SrcChan(VOS_UINT32 ulSrcChanID);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#else
#include "mdrv_socp.h"
#include "mdrv_socp_common.h"
#endif

#endif


