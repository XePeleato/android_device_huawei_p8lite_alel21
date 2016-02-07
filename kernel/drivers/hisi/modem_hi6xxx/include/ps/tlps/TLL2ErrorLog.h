/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : TLL2Errorlog.h
  Description     : TLL2Errorlog.h header file
  History           :
     1.l00213440      2013-9-15  Draft
******************************************************************************/

#ifndef __TLL2Errorlog_H__
#define __TLL2Errorlog_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "product_config.h"
#include "omnvinterface.h"
#include "SysNvId.h"

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/
/*begin:stub for errorlog test*/
//#define FEATURE_PTM    FEATURE_ON
/*end:stub for errorlog test*/
#define TDS_ERRLOG_MAX_FAR_CELL_NUM                64
#define TDS_ERRLOG_PS_MAX_UE_ID_LENGTH             6

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 Enum
*****************************************************************************/

/*****************************************************************************
 枚举名    : TL_L2_ERROR_CODE_ENUM
 枚举说明  : TL_L2错误码类型
*****************************************************************************/
enum TL_L2_ERROR_CODE_ENUM
{
    TL_MAC_RAR_NOTMATCH = 1,
    TL_MAC_RAR_TIMEOUT,
    TL_MAC_CONTENT_TIMEOUT,
    TL_MAC_CONTENT_NOTMATCH,
    TL_MAC_PREBLE_FAIL,
    TL_MAC_MSG3_FAIL,
    TL_MAC_TA_TIMEOUT,
    TL_MAC_SR_FAIL,
    TL_RLC_MAXRETXPDU,
    TL_RLC_MEM_FAIL,    
    TL_PDCP_FAIL,
    TDS_ERRLOG_MAC_RANDOM_ACC_FAIL,
    TDS_ERRLOG_MAC_SYNC_FAIL,
    
};
typedef VOS_UINT8 TL_L2_ERROR_CODE_ENUM_UINT8;

/*****************************************************************************
 枚举名    : TL_L2_ERRLOG_ALMLEVEL_ENUM
 枚举说明  : TL_L2错误码类型
*****************************************************************************/
enum TL_L2_ERRLOG_ALMLEVEL_ENUM
{
    TL_L2_ERRLOG_ALMLEVEL_CRITICAL   = 1, /*紧急*/
    TL_L2_ERRLOG_ALMLEVEL_MAJOR      = 2, /*重要*/
    TL_L2_ERRLOG_ALMLEVEL_MINOR      = 3, /*次要*/
    TL_L2_ERRLOG_ALMLEVEL_WARING     = 4, /*提示*/

    TL_L2_ERRLOG_ALMLEVEL_BUTT
};
typedef VOS_UINT16  TL_L2_ERRLOG_ALMLEVEL_ENUM_UINT16;

/*****************************************************************************
 枚举名    : TL_L2_ERRLOG_MODULE_TYPE
 枚举说明  : TL_L2错误模块
*****************************************************************************/
enum TL_L2_ERRLOG_MODULE_TYPE
{
    TL_L2_ERRLOG_PDCP,
    TL_L2_ERRLOG_RLC,
    TL_L2_ERRLOG_MAC,

    TL_L2_ERRLOG_BUTT
};
typedef VOS_UINT8  TL_L2_ERRLOG_MODULE_TYPE_UINT8;

/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_PUB_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 实体的头部信息，维护实体的状态以及RB与LHID的映射关系
*****************************************************************************/
typedef struct
{
    VOS_UINT8                   ucRbId;
    VOS_UINT8                   ucLchId;
    VOS_UINT8                   enRlcState;
    VOS_UINT8                   ucRev;
}LTE_ERRLOG_RLC_PUB_INFO_STRU;

/*****************************************************************************
 结构名    : LRLC_TM_RX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TM接收实体
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_RLC_PUB_INFO_STRU          stPubInfo;
}LTE_ERRLOG_RLC_TM_RX_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_UM_PDUHDR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RLC UM RLC头信息
*****************************************************************************/

typedef struct
{
    VOS_UINT8                       enPduHdrType;       /*消息头类型 5bit、10bit*/
    VOS_UINT8                       ucPduHdrLen;        /*消息头长度 1、2字节*/
    VOS_UINT16                      usRev;
}LTE_ERRLOG_RLC_UM_PDUHDR_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_UM_RX_WIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RLC UM接收侧的窗口维护变量
*****************************************************************************/
typedef struct
{
    VOS_UINT16                      usVRur;         /*VR(UR)*/
    VOS_UINT16                      usVRux;         /*VR(UX)*/
    VOS_UINT16                      usVRuh;         /*VR(UH)*/
    VOS_UINT16                      usRev;
}LTE_ERRLOG_RLC_UM_RX_WIN_STRU;

/*TTF Timer Management*/
typedef struct
{
    VOS_UINT8                       enTimerState;       /* Timer状态*/
    VOS_UINT8                       aucReserve[3];      /* 4字节对齐，保留 */
    VOS_UINT32                      ulTimerValue;       /* Timer时长 */
    VOS_UINT32                      ulStartTime;        /* 启动时间点 */
}LTE_ERRLOG_UP_TIMER_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_TMR_NODE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RLC TMR数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                 ulTmrVal;
    LTE_ERRLOG_UP_TIMER_STRU             stTtfTmr;
}LTE_ERRLOG_RLC_TMR_NODE_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_SDU_SGMNT
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存SDU分片的信息，接收侧使用
*****************************************************************************/
typedef struct
{
    VOS_UINT8                   ucUsedFlag; /*SDU分片的有效性标志*/
    VOS_UINT8                   ucLsfFlag;            /*标记此SDU所属的PDU是否已接收到最后一个PDU分片*/
    VOS_UINT16                  usPduSn;    /*记录当前分段属于那个PDU*/
    VOS_UINT32                  ulSduLen;
}LTE_ERRLOG_RLC_SDU_SGMNT;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_UM_RX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : UM接收模式
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_RLC_PUB_INFO_STRU              stPubInfo;

    LTE_ERRLOG_RLC_UM_PDUHDR_STRU             stPduHdr;       /*RLC头信息*/
    LTE_ERRLOG_RLC_UM_RX_WIN_STRU             stRxWinVar;     /*接收侧窗口维护变量*/

    /*定时器*/
    LTE_ERRLOG_RLC_TMR_NODE_STRU              stReorderTmr;   /*重排序定时器*/

    /*队列信息*/
    LTE_ERRLOG_RLC_SDU_SGMNT                  stSduSgmnt;                     /*SDU分片信息*/

    /*常量*/
    VOS_UINT16                      usSnModule;         /*SN的系数  32、1024*/
    VOS_UINT16                      usWinSize;          /*窗口大小*/

}LTE_ERRLOG_RLC_UM_RX_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_AM_RX_WIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AM接收侧窗口状态变量结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                      usVRr;     /*VR(R)*/
    VOS_UINT16                      usVRmr;    /*VR(MR)*/
    VOS_UINT16                      usVRx;     /*VR(X)*/
    VOS_UINT16                      usVRh;     /*VR(H)*/
    VOS_UINT16                      usVRms;    /*VR(MS)*/
    VOS_UINT16                      usRev;
}LTE_ERRLOG_RLC_AM_RX_WIN_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_AM_RX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AM接收侧实体结构
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_RLC_PUB_INFO_STRU              stPubInfo;
    LTE_ERRLOG_RLC_AM_RX_WIN_STRU             stRxWinVar;             /*窗口变量*/

    /*是否使用状态定时器的标志*/
    VOS_UINT8                       ucCtrlRptFlag;          /*状态报告标志*/
    VOS_UINT8                       ucStatusProhibitFlag;   /*T_STATUS_PROHIBIT标志*/
    VOS_UINT16                      usPollingSn;            /*带polling位的SN值*/

    /*定时器*/
    LTE_ERRLOG_RLC_TMR_NODE_STRU              stReorderTmr;           /*重排序定时器*/
    LTE_ERRLOG_RLC_TMR_NODE_STRU              stTStatusProhibit;      /*禁止定时器*/

    /*队列*/
    LTE_ERRLOG_RLC_SDU_SGMNT                  stSduSgmnt;             /*SDU 分片信息*/
}LTE_ERRLOG_RLC_AM_RX_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_RX_ENTITY_STRU;
 协议表格  :
 ASN.1描述 :
 结构说明  : RLC接收侧实体的结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                       ucUsedFlag;     /*是否被使用标志*/
    VOS_UINT8                       ucRbID;
    VOS_UINT8                       ucLchID;
    VOS_UINT8                       enChoice;      /*AM, UM, TM*/

    union
    {
        LTE_ERRLOG_RLC_TM_RX_STRU             stTmRx;         /*TM实体*/
        LTE_ERRLOG_RLC_UM_RX_STRU             stUmRx;         /*UM实体*/
        LTE_ERRLOG_RLC_AM_RX_STRU             stAmRx;         /*AM实体*/
    }u;

}LTE_ERRLOG_RLC_RX_ENTITY_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_TM_TX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TM发送实体
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_RLC_PUB_INFO_STRU          stPubInfo;
}LTE_ERRLOG_RLC_TM_TX_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_UM_TX_WIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RLC UM发送侧窗口维护结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulVTus;             /*状态变量 VT(US)*/
}LTE_ERRLOG_RLC_UM_TX_WIN_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_UM_TX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : UM发送实体
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_RLC_PUB_INFO_STRU              stPubInfo;

    LTE_ERRLOG_RLC_UM_PDUHDR_STRU             stPduHdr;           /*RLC头信息*/
    LTE_ERRLOG_RLC_UM_TX_WIN_STRU             stTxWinVar;         /*RLC发送侧窗口状态变量*/

    /*常量*/
    VOS_UINT16                      usSnModule;         /*SN的系数  32、1024*/
    VOS_UINT16                      usRev;
}LTE_ERRLOG_RLC_UM_TX_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_AM_TX_WIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RLC AM发送侧的状态变量结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                      usVTa;   /*VT(ACK)*/
    VOS_UINT16                      usVTms;  /*Max Send*/
    VOS_UINT16                      usVTs;   /*Send*/
    VOS_UINT16                      usRev;
}LTE_ERRLOG_RLC_AM_TX_WIN_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_AM_POLL_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : poll结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                       ucPollFlag;         /*p位标致*/
    VOS_UINT8                       ucRev;
    VOS_UINT8                       ucPollPduFlag;      /*使用发送PDU数来触发PoLL标志*/
    VOS_UINT8                       ucPollByteFlag;     /*使用发送byte数来触发PoLL标志*/

    VOS_UINT32                      ulCurTxPduNum;      /*当前发送的PDU数*/
    VOS_UINT32                      ulCurTxByteNum;    /*当前发送的BYTE数*/

    /*定时器*/
    LTE_ERRLOG_RLC_TMR_NODE_STRU              stPollTmr;
    VOS_UINT32                      ulTPollRxTxSn;      /*T_poll_retransmit保存的SN*/

    /*配置参数*/
    VOS_UINT32                      ulPduPoll;          /*触发POLL的PDU数*/
    VOS_UINT32                      ulBytePoll;         /*触发POLL的byte数*/
}LTE_ERRLOG_RLC_AM_POLL_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_AM_CTRL_PDU_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 控制PDU结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                       enCtrlPduState; /*状态*/
    VOS_UINT8                       aucRev[3];
    VOS_UINT32                      ulPduLen;       /*控制PDU的长度*/
}LTE_ERRLOG_RLC_AM_CTRL_PDU_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_AM_TX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 发送侧实体结构
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_RLC_PUB_INFO_STRU              stPubInfo;

    LTE_ERRLOG_RLC_AM_TX_WIN_STRU             stTxWinVar;     /*发送侧的状态变量*/
    LTE_ERRLOG_RLC_AM_POLL_INFO_STRU          stPollInfo;     /*Poll结构*/
    VOS_UINT16                      usSduSgmntSn;   /*SDU分片的SN，指示SDU时使用*/
    VOS_UINT8                       ucRptLinkDownFlag; /*是否向RRC上报链路故障开关*/
    VOS_UINT8                       ucRev;

    LTE_ERRLOG_RLC_AM_CTRL_PDU_STRU           stCtrlPdu;      /*RLC发送侧只保存最新的状态PDU*/

    VOS_UINT32                      ulPduMaxRetxNum;        /*最大的重传次数*/
    VOS_UINT32                      ulCtrlMaxRetxNum;       /*控制PDU的最大重传次数*/
}LTE_ERRLOG_RLC_AM_TX_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_TX_ENTITY_STRU;
 协议表格  :
 ASN.1描述 :
 结构说明  : RLC发送侧实体结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                       ucUsedFlag;     /*是否被使用标志*/
    VOS_UINT8                       ucRbID;
    VOS_UINT8                       ucLchID;
    VOS_UINT8                       enChoice;      /*AM, UM, TM*/

    union
    {
        LTE_ERRLOG_RLC_TM_TX_STRU             stTmTx;         /*TM实体*/
        LTE_ERRLOG_RLC_UM_TX_STRU             stUmTx;         /*UM实体*/
        LTE_ERRLOG_RLC_AM_TX_STRU             stAmTx;         /*AM实体*/
    }u;

}LTE_ERRLOG_RLC_TX_ENTITY_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_UL_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LRLC的上行统计量结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulRxPdcpSdu;             /*RLC收到PDCP的SDU数*/
    VOS_UINT32 ulRxPdcpDropSdu;         /*RLC收到PDCP通知丢弃SDU的数*/
    VOS_UINT32 ulRlcDropSdu;            /*RLC收到消息实际丢弃的包*/
    VOS_UINT32 ulIndTxSduSucc;          /*RLC 指示PDCP发送成功的SDU数*/
    VOS_UINT32 ulIndTxSduFail;          /*Rlc 指示PDCP发送失败的SDU数*/
    VOS_UINT32 ulQueFullDropSdu;        /*SDU队列满丢弃SDU数*/
    VOS_UINT32 ulDropSduNoRsn;          /*其他原因丢弃SDU数*/

    VOS_UINT32 ulMacNtfSendNum;         /*MAC通知发送次数*/
    VOS_UINT32 ulTxPudToMac;            /*发送到MAC的PDU数*/
    VOS_UINT32 ulMacNtfRlcNoRes;        /*MAC调度时,RLC没有资源*/
    VOS_UINT32 ulTxNullPduToMac;        /*返回的空PDU的次数*/
    VOS_UINT32 ulMacNtfHarqFail;        /*MAC通知HARQ失败次数*/
    VOS_UINT32 ulResNotEnough;          /*MAC调度资源不够*/

    VOS_UINT32 ulRxCtrlPdu;             /*收到ENB的状态PDU次数*/
    VOS_UINT32 ulRxDlCtrlPdu;           /*收到下行的状态报告数*/
    VOS_UINT32 ulRxCtrlPduNackSNOutWIn; /*上行受到的NACK SN在窗外*/
    VOS_UINT32 ulTxCtrlPdu;             /*发送到ENB的状态PDU数目*/
    VOS_UINT32 ulTotalReTxPdu;          /*重传PDU的次数，每重传一次加一*/
    VOS_UINT32 ulReTxPollTmr;           /*重传定时器的启动个数*/
    VOS_UINT32 ulIndRrcLinkErr;         /*向L3指示链路失败*/
    VOS_UINT32 ulIndRrcRlcErr;          /*向L3指示错误的次数，RLC内部错误*/

    VOS_UINT32 ulRxRrcRelease;          /*收到RRC的释放次数*/
    VOS_UINT32 ulRrcCfg;                /*收到配置次数*/
    VOS_UINT32 ulRrcCfgFail;            /*配置失败次数*/
    VOS_UINT32 ulReestablish;           /*重建立次数*/
    VOS_UINT32 ulReestablishFail;       /*重建立失败次数*/
    VOS_UINT32 ulReTxSegNum;            /*发送的重分段分片的次数*/
    VOS_UINT32 ulInputPdu;              /*根据状态报告放入重传队列的Pdu*/
    VOS_UINT32 ulInputPduSeg;           /*根据状态报告放入重传队列的Pdu分片*/
}LTE_ERRLOG_RLC_UL_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_DL_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LRLC的下行统计量结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulRxMacPdu;              /*收到MAC发送的RLC pdu数量*/
    VOS_UINT32 ulDropMacPdu;            /*被丢弃的PDU数量*/
    VOS_UINT32 ulProcMacPduFail;        /*MAC的PDU处理失败*/
    VOS_UINT32 ulReRecvSnDrop;          /*重复的SN丢弃数*/
    VOS_UINT32 ulReorderTmrOut;         /*重排序超时次数*/

    VOS_UINT32 ulRxCtrlPdu;             /*收到MAC发送的状态PDU的数量*/
    VOS_UINT32 ulTxEnbCtrlPdu;          /*向RLC上行发送ENB状态PDU的数量*/
    VOS_UINT32 ulTxDlCtrlPdu;           /*向RLC上行发送下行状态PDU的数量*/
    VOS_UINT32 ulTxDlCtrlPduFail;       /*向RLC上行发送下行状态PDU失败的数量*/

    VOS_UINT32 ulTxSduToPdcpSucc;       /*发送到PDCP的SDU的成功数目*/
    VOS_UINT32 ulTxSduToPdcpFail;       /*发送到PDCP的SDU的失败数目*/
    VOS_UINT32 ulAllocSduMsgFail;       /*发送SDU到PDCP申请内存失败*/

    VOS_UINT32 ulResmPdufail;           /*重组失败丢弃的PDU*/
    VOS_UINT32 ulDropHeadSgmnt;         /*丢弃头分片*/
    VOS_UINT32 ulDropTailSgmnt;         /*丢弃尾分片*/

    VOS_UINT32 ulStartTmrFail;          /*启动定时器失败*/
    VOS_UINT32 ulStopTmrFail;           /*启动定时器失败*/

    VOS_UINT32 ulIndRrcRlcMemErr;       /*向L3指示内存耗尽次数，内存保护错误*/
    VOS_UINT32 ulIndRrcRlcErr;          /*向L3指示错误的次数，RLC内部错误*/
    VOS_UINT32 ulRlcDlCfg;              /*配置次数*/
    VOS_UINT32 ulRlcDlCfgFail;          /*配置失败次数*/
    VOS_UINT32 ulRlcDlRealse;           /*释放次数*/
    VOS_UINT32 ulReCfg;                 /*重建次数*/
    VOS_UINT32 ulReCfgFail;             /*重建失败次数*/
    VOS_UINT32 ulRecePduSeg;             /*Rlc接收到的Pdu分片数量*/
    VOS_UINT32 ulAssemPduSeg;             /*重组的重分段的Pdu数量*/

    VOS_UINT32 ulPduLiError;             /*PDU中LI字段异常统计*/
    VOS_UINT32 ulPduLsfError;            /*PDU分段或重分段链表中LSF字段异常*/
    VOS_UINT32 ulPduRedupDiscard;         /*PDU重复被丢弃数量*/

    VOS_UINT32 ulRxPduSize;               /*接收PDU字节数*/
}LTE_ERRLOG_RLC_DL_STAT_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_ULSCH_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulPeriodBsrTimer;
    VOS_UINT32                          ulRetxBsrTimer;

    VOS_UINT8                           ucUlMaxHarqTxNum;
    VOS_UINT8                           enTtiBundling;
    VOS_UINT8                           enExtendedBsrSizes;
    VOS_UINT8                           aucRev[1];

}LTE_ERRLOG_MAC_ULSCH_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_PHR_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulPeriodPhrTimer;
    VOS_UINT32                          ulProhibitPhrTimer;
    VOS_UINT8                           ucDlPathLossChange;
    VOS_UINT8                           enExtendPhr;
    VOS_UINT8                           enSimulPUCCH_PUSCH;
    VOS_UINT8                           aucRev[1];
}LTE_ERRLOG_MAC_PHR_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_SR_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSrPeriod;
    VOS_UINT8                           ucSrOffset;
    VOS_UINT16                          usSrTransMax;

    VOS_UINT8                           usSrCfgFlag;/*SR配置信息是否有效，0:无效，1:有效*/
    VOS_UINT8                           aucRev1[1];
    VOS_UINT16                          usSrProhibitTimer;

}LTE_ERRLOG_MAC_SR_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_GROUPA_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucPblGrpASize;                          /*Group A所对应的Preamble个数:如果空口没有，就配置成 0 */
    VOS_UINT8                           aucRev1[1];
    VOS_UINT16                          usMessageSizeGroupA;                    /*协议36.321的参数MESSAGE_SIZE_GROUP_A*/

    VOS_INT8                            cMessagePowerOffsetGroupB;              /*协议36.321的参数MESSAGE_POWER_OFFSET_GROUP_B,取值协议待定*/
    VOS_UINT8                           aucRev2[3];

} LTE_ERRLOG_MAC_GROUPA_CONFIG_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_RACH_COMM_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitGroupAValid  :1;                     /*Group A信息是否有效标志，0:无效，1:有效*/
    VOS_UINT32                          bitSpare        :31;

    LTE_ERRLOG_MAC_GROUPA_CONFIG_STRU             stGroupAConfig;

    VOS_UINT8                           ucPblNum;                               /*Group A和B所对应的总Preamble个数*/
    VOS_UINT8                           ucPblPwrStep;                           /*Preamble功率爬升因子*/
    VOS_INT16                           sPblInitTargetPwr;                      /*Preamble在eNB的期望接收功率*/

    VOS_UINT8                           ucPblMaxTxNum;                          /*随机接入Preamble最大发送次数*/
    VOS_UINT8                           ucRarWindowSize;                        /*接收随机接入响应消息的窗口*/
    VOS_UINT8                           ucContentTimer;                         /*竞争解决定时器*/
    VOS_UINT8                           ucMaxHarqMsg3Tx;                        /*Msg3进行HARQ的最大发送次数*/

    VOS_INT8                            cPMax;                                  /*最大发射功率，缺省时使用UE能力中的最大发射功率*/
    VOS_INT8                            cDeltaPblMsg3;                          /*Preamble与Msg3之间的OFFSET*/
    VOS_UINT8                           ucSysULRBNum;                           /*上行系统带宽*/
    VOS_UINT8                           aucRev1[1];
} LTE_ERRLOG_MAC_RACH_COMM_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_PRACH_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucPrachConfigIndex;                     /*已经确认:Prach-ConfigurationIndex,0-63*/
    VOS_UINT8                           ucPrachFormat;
    VOS_UINT8                           aucRev[2];
}LTE_ERRLOG_MAC_PRACH_CONFIG_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_RACH_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_MAC_RACH_COMM_INFO_STRU            stRachCommInfo;                         /*36.331 6.3.2节RACH-ConfigCommon*/
    LTE_ERRLOG_MAC_PRACH_CONFIG_STRU              stPrachConfig;                          /*36.331 6.3.2节PRACH-Configuration PRACH各种配置*/

    VOS_UINT8                           ucUeIdLength;                           /*UE长度，暂定字节为单位*/
    VOS_UINT8                           aucRev2[1];
    VOS_UINT8                           aucUeId[TDS_ERRLOG_PS_MAX_UE_ID_LENGTH];           /*ue id的格式和类型*/

}LTE_ERRLOG_MAC_RACH_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_TA_TIMER_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usCellTaTimer;
    VOS_UINT16                          usTaTimer;
}LTE_ERRLOG_MAC_TA_TIMER_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_RNTI_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucCRntiValid;                           /*C-RNTI是否有效标志,0:无效,1:有效*/
    VOS_UINT8                           aucRev[3];
    VOS_UINT16                          usCRnti;                                /*C-RNTI值*/
    VOS_UINT16                          usTmpCRnti;                             /*临时C-RNTI*/
}LTE_ERRLOG_MAC_RNTI_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_SPS_DL_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usSpsIntervalDL;
    VOS_UINT8                           aucRev[2];

}LTE_ERRLOG_MAC_SPS_DL_CONFIG_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_SPS_UL_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usSpsIntervalUL;
    VOS_UINT8                           ucImplicitReleaseAfter;
    VOS_UINT8                           aucRev[1];

}LTE_ERRLOG_MAC_SPS_UL_CONFIG_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_SPS_CONFIG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitSpsDlValid:1;
    VOS_UINT32                          bitSpsUlValid:1;
    VOS_UINT32                          bitSpare:30;

    LTE_ERRLOG_MAC_SPS_DL_CONFIG_STRU             stSpsDlConfig;
    LTE_ERRLOG_MAC_SPS_UL_CONFIG_STRU             stSpsUlConfig;

}LTE_ERRLOG_MAC_SPS_CONFIG_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_TDD_CONFIG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSubFrameAssig;

    VOS_UINT8                           aucRev[3];

}LTE_ERRLOG_MAC_TDD_CONFIG_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_CELL_STATUS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucConfiged;                             /*小区配置状态 1:已配置; 0:未配置*/
    VOS_UINT8                           ucActived;                              /*小区激活状态 1:激活 0:去激活*/
    VOS_UINT8                           aucRev[2];
}LTE_ERRLOG_MAC_CELL_STATUS_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_CA_CONFIG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_MAC_CELL_STATUS_STRU               astLmacStatus[8];
    VOS_UINT32                          ulScellDeactTimer;
}LTE_ERRLOG_MAC_CA_CONFIG_INFO_STRU;

/*****************************************************************************
 结构名    :LTE_ERRLOG_MAC_RA_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucValid;                                /*用于描述该信息是否有效指示,0:无效,1:有效*/
    VOS_UINT8                           ucRaPblId;                              /*随机接入选择的PreambleID*/
    VOS_UINT8                           ucRaPblGrpId;                           /*随机接入选择的Preamble群号*/
    VOS_UINT8                           ucRarWindowSize;

    VOS_UINT16                          usPathLoss;                             /*用于选择Group A或B的实际路损，该值需要确认获取方式 */
    VOS_UINT16                          usMsg3Size;                             /*用于选择Group A或B的消息3大小值 */

    VOS_UINT8                           enRaTrigger;                            /*随机接入触发源：0:RRC,1:MAC,2:PDCCH */
    VOS_UINT8                           enRaCause;                              /*随机接入触发原因：*/
    VOS_UINT8                           enRaType;                               /*随机接入类型*/
    VOS_UINT8                           ucRaTxNum;                              /*随机接入重发计数器：*/

    VOS_INT16                           sRaTxPower;                             /*随机接入实际发射功率*/
    VOS_UINT16                          usBackOff;                              /*BI值*/

    VOS_UINT16                          usOpId;                                 /*操作号*/
    VOS_UINT8                           ucRaPrachMaskIndex;
    VOS_UINT8                           ucRrcOriginMsg3Size;

    VOS_UINT16                          usDetalPrampup;                         /*随机接入累计步长功率值*/
    VOS_UINT16                          usMacPhyOpId;
}LTE_ERRLOG_MAC_RA_CTRL_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_BSR_DYN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           enPeriBsrFlag;                          /*BSR标志,TRUE为要上报,FALSE为不要上报*/
    VOS_UINT8                           enRegBsrFlag;                           /*Regular BSR标志,TRUE为要触发SR，FALSE为不触发SR*/

    VOS_UINT8                           ucMaxLchPri;
    VOS_UINT8                           ucLcgNum;

    VOS_UINT8                           aucLcgId[4];                             /*存放逻辑信道组ID*/
    VOS_UINT8                           enSrMaskTrigBsr;
    VOS_UINT8                           aucRev1[3];

}LTE_ERRLOG_MAC_BSR_DYN_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_SR_DYN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           enSrState;
    VOS_UINT8                           ucSrCounter;                            /*SR累计次数*/
    VOS_UINT16                          usSrDynMaxNum;

    VOS_UINT16                          usSrLastFrame;                          /*SR上次发送的帧号*/
    VOS_UINT8                           ucSrLastSubFrame;                       /*SR上次发送的子帧号*/
    VOS_UINT8                           ucSrSendNotCnf;


    VOS_UINT16                          usSrSendNotCnfCount;                    /*SR发送连续得不到CNF的计数*/
    VOS_UINT16                          usSrOpId;

}LTE_ERRLOG_MAC_SR_DYN_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_LPHY_DECREASE_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT16                         usPowerBackoff;
    VOS_UINT16                         usFrame;
    VOS_UINT8                          ucSubFram;
    VOS_UINT8                          aucRev[3];
}LTE_ERRLOG_MAC_LPHY_DECREASE_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_PHR_DYN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usCurPhrPathLost;                       /*当前最新的路损线性值，已转为dB*/
    VOS_UINT16                          usLastPhrPathLost;                      /*上一次PHR上报时的路损线性值,已转为dB*/
    VOS_UINT16                          usCurPhrPowerBackoff;                   /*当前最新的功率回退值，已转为dB*/
    VOS_UINT16                          usLastPhrPowerBackoff;                  /*上一次PHR上报时的功率回退值，已转为dB*/
    VOS_UINT8                           enPeriPhrFlag;                          /*周期PHR上报标志,TRUE为要上报,FALSE为不要上报*/
    VOS_UINT8                           enProhibitPhrFlag;                      /* Prohibit PHR定时器超时触发标志,TRUE为要上报,FALSE为不要上报 */
    VOS_UINT8                           aucRev[2];
    LTE_ERRLOG_MAC_LPHY_DECREASE_INFO_STRU        stPrePBInfo;                            /*前一次Power Backoff信息*/
    LTE_ERRLOG_MAC_LPHY_DECREASE_INFO_STRU        stLatestPBInfo;                         /*最近一次Power Backoff信息*/
}LTE_ERRLOG_MAC_PHR_DYN_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_TA_DYN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           enTaRunFlag;                           /* TA超时定时器，若没有缓存则不立即触发随机接入，若有则触发 */
    VOS_UINT8                           aucRev1[3];
}LTE_ERRLOG_MAC_TA_DYN_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_SPS_DYN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSpsPadCouter;
    VOS_UINT8                           enUlSpsGrantFlag;
    VOS_UINT8                           aucRev1[2];
}LTE_ERRLOG_MAC_SPS_DYN_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_CTRL_ELEM_DYN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_MAC_BSR_DYN_INFO_STRU              stLmacBsrDynInfo;
    LTE_ERRLOG_MAC_SR_DYN_INFO_STRU               stLmacSrDynInfo;
    LTE_ERRLOG_MAC_PHR_DYN_INFO_STRU              stLmacPhrDynInfo;
    LTE_ERRLOG_MAC_TA_DYN_INFO_STRU               stLmacTaDynInfo;
    LTE_ERRLOG_MAC_SPS_DYN_INFO_STRU              stLmacSpsDynInfo;
}LTE_ERRLOG_MAC_CTRL_ELEM_DYN_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_ENTITY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           enLMacState;                            /*MAC 实体状态*/
    VOS_UINT8                           ucCurrSubFrame;                         /*当前子帧号*/
    VOS_UINT16                          usCurrFrame;                            /*当前帧号*/

    VOS_UINT8                           enLmacFrameType;
    VOS_UINT8                           aucRev1[3];

    LTE_ERRLOG_MAC_ULSCH_INFO_STRU                stLmacUlschInfo;                        /*MAC上行传输信道的相关参数信息*/
    LTE_ERRLOG_MAC_PHR_INFO_STRU                  stLmacPhrInfo;                          /*PHR相关参数信息*/
    LTE_ERRLOG_MAC_SR_INFO_STRU                   stLmacSrInfo;                           /*调度请求相关参数信息*/
    LTE_ERRLOG_MAC_RACH_INFO_STRU                 stLmacRachInfo;                         /*RACH信道相关信息*/
    LTE_ERRLOG_MAC_TA_TIMER_INFO_STRU             stLmacTaTimerInfo;                      /*TA Timer相关信息*/
    LTE_ERRLOG_MAC_RNTI_INFO_STRU                 stLmacRntiInfo;                         /*RNTI 相关信息*/
    LTE_ERRLOG_MAC_SPS_CONFIG_INFO_STRU           stLmacSpsConfig;                        /*半静态调度相关信息*/
    LTE_ERRLOG_MAC_TDD_CONFIG_INFO_STRU           stLmacTddConfig;                        /*TDD相关信息*/
    //LTE_ERRLOG_MAC_CA_CONFIG_INFO_STRU            stLmacCaConifg;                         /*小区配置和激活状态信息*/

    LTE_ERRLOG_MAC_RA_CTRL_STRU                   stLmacRaCtrl;                            /*随机接入过程动态参数*/

    LTE_ERRLOG_MAC_CTRL_ELEM_DYN_INFO_STRU        stLmacCtrlElemDynInfo;                  /*MAC 控制元以及SR动态控制信息，包括标志位等*/

}LTE_ERRLOG_MAC_ENTITY_STRU;

typedef struct
{
    VOS_UINT32                                ulSendShortBsr;
    VOS_UINT32                                ulSendLongBsr;
    VOS_UINT32                                ulSendAbnormBsr;
    VOS_UINT32                                ulSendPadBsr;
    VOS_UINT32                                ulSendPadShortBsr;
    VOS_UINT32                                ulSendPadLongBsr;
    VOS_UINT32                                ulSendTruncatedBsr;
    VOS_UINT32                                ulCancelBsr;
    VOS_UINT32                                ulLongToShortBsr;

}LTE_ERRLOG_MAC_DEBUG_BSR_INFO_STRU;

typedef struct
{
    VOS_UINT32                                ulRecvSchFrmInd;                  /*MAC收到的调度帧指示*/
    VOS_UINT32                                ulRecvUnexptedSchFrm;             /*MAC收到不连续的帧号与子帧号统计*/
    VOS_UINT32                                ulRecvSameFrame;                  /*MAC收到相同的帧号与子帧号统计*/

    VOS_UINT32                                ulTotalSchNotFinishNum;           /*MAC1ms调度没有完成的次数*/

    VOS_UINT32                                ulMacSchedMacPduSuccNum;
    VOS_UINT32                                ulMacSchedMacPduFailNum;          /*MAC 调度组包异常统计*/
    VOS_UINT32                                ulMacPduNotFree;                  /*由于子帧不连续等导致MAC PDU源端没有及时释放*/
    VOS_UINT32                                ulMacRecvRlcPduTotal;

    VOS_UINT32                                ulSendPhr;
    VOS_UINT32                                ulGetPhrErr;
    VOS_UINT32                                ulSendSr;
    VOS_UINT32                                ulPhyCancelSr;
    VOS_UINT32                                ulMacCancelSr;
    VOS_UINT32                                ulTaTimeOutNum;
    VOS_UINT32                                ulSCellDeacTimeOutNum;
    VOS_UINT32                                ulSendPadding;
    VOS_UINT32                                ulSendPadHead;

    VOS_UINT32                                ulGetRlcNuLLBuffNum;              /*获取RLC缓存信息为0的统计*/
    VOS_UINT32                                ulGetCcchInActiveNum;             /*激活态下查询到CCCH为非空统计*/
    VOS_UINT32                                ulGetLchPriFromRlcFail;           /*RLC没有对逻辑信道正确排序*/

    VOS_UINT32                                ulMsg3NotContainHoCmp;            /*切换完成消息没有在Msg3统计*/

    VOS_UINT32                                ulUlGrantNotSchErr;               /*上行授权错过调度时刻的统计*/

    VOS_UINT32                                ulPhyReleaseUlSps;                /*Phy释放上行半静态指示*/
    VOS_UINT32                                ulMacReleaseUlSps;                /*MAC释放上行半静态指示*/

    VOS_UINT32                                ulForbidDspSleepFail;             /*强制DSP不睡眠失败*/
    VOS_UINT32                                ulWriteDediMailBoxFail;           /*写专有邮箱失败*/

    LTE_ERRLOG_MAC_DEBUG_BSR_INFO_STRU                  stBsr;
}LTE_ERRLOG_MAC_DEBUG_UL_STAT_INFO_STRU;

typedef struct
{
    VOS_UINT32                                ulDlTbRecvIntRsp;                 /*下行TB接收中断响应统计*/
    VOS_UINT32                                ulDlTbRecvIntErr;                 /*下行TB接收错误中断统计*/
    VOS_UINT32                                ulDlForbidDspSleepFail;           /*下行TB接收中断中禁止DSP睡眠失败统计*/
    VOS_UINT32                                ulSemTakeTotal;                   /*MAC下行取得信号量统计*/

    VOS_UINT32                                ulActDemultipleSucc;
    VOS_UINT32                                ulActDemultipleFail;
    VOS_UINT32                                ulActSendRlcErrorNum;
    VOS_UINT32                                ulActSendRlcNum;
    VOS_UINT32                                ulActDemulOneLchErr;
    VOS_UINT32                                ulActRecvPadding;
    VOS_UINT32                                ulActRecvTa;
    VOS_UINT32                                ulActRecvScellActivation;
    VOS_UINT32                                ulActRecvDrx;

    VOS_UINT32                                ulDlTbAddrNull;                   /*下行TB内存地址为空的统计*/
    VOS_UINT32                                ulDlTbMemCheckErr;                /*下行TB内存地址检查错误*/
    VOS_UINT32                                ulDlTbCheckErr;                   /*下行TB CRC或重复数据检查错误*/
    VOS_UINT32                                ulDlTbCellIndexErr;               /*下行TB 小区ID错误*/
    VOS_UINT32                                ulDlTbSCellDeactived;             /*下行TB对应的SCell状态为去激活*/
    VOS_UINT32                                ulOterStateRecvTb;
    VOS_UINT32                                ulRadnomStateRecvTb;
    VOS_UINT32                                ulRadnomStateRecvTbErr;
    VOS_UINT32                                ulHoEnQueTb;
    VOS_UINT32                                ulHoDeQueTb;
    VOS_UINT32                                ulHoDeQueTbErr;
    VOS_UINT32                                ulMacPdcchRecvTb;

    VOS_UINT32                                ulRecvBcchTb;
    VOS_UINT32                                ulRecvPbchTb;
    VOS_UINT32                                ulRecvPchTb;

    VOS_UINT32                                ulDlTbIntAllocMemFail;            /*下行浮动中断分配内存失败个数*/
    VOS_UINT32                                ulDlLteMasterGetMemFail;          /*LTE主模下DSP获取内存失败*/
    VOS_UINT32                                ulDlLteSlaveGetMemFail;           /*LTE从模下DSP获取内存失败*/
    VOS_UINT32                                ulDlTbRecvQueFull;                /*下行TB接收队列满*/
    VOS_UINT32                                ulDlOtherErrInd;                  /*DSP上报的其他错误类型*/

}LTE_ERRLOG_MAC_DEBUG_DL_STAT_INFO_STRU;

typedef struct
{
    VOS_UINT32                                ulSchMsg3Num;                     /*调度新数据Msg3的次数*/
    VOS_UINT32                                ulSchMsg3ReTransNum;              /*调度重传Msg3的次数*/
    VOS_UINT32                                ulGetMsg3FromRlcFail;
}LTE_ERRLOG_MAC_DEBUG_ONRAND_SCHED_INFO_STRU;

typedef struct
{
    VOS_UINT32                                ulMacRaReq;                       /*MAC主动发起的RA请求次数*/
    VOS_UINT32                                ulPdcchNonContentRaReq;            /*PDCCH Order发起的非竞争RA请求次数*/
    VOS_UINT32                                ulPdcchContentRaReq;              /*PDCCH Order发起的竞争RA请求次数*/
    VOS_UINT32                                ulRrcNonContentRaReq;             /*RRC发起的非竞争RA请求次数*/
    VOS_UINT32                                ulRrcContentRaReq;                /*RRC发起的竞争RA请求次数*/
    VOS_UINT32                                ulSelectGroupA;                   /*RA过程中选择群组A的次数*/
    VOS_UINT32                                ulSelectGroupB;                   /*RA过程中选择群组B的次数*/

    VOS_UINT32                                ulRecvPhyRaCnf;                   /*MAC收到PHY的RA确认*/
    VOS_UINT32                                ulRepeatRaReq;                    /*重复发给物理层RA请求次数*/
    VOS_UINT32                                ulMaxRaReq;                       /*达到发给物理层RA请求最大次数*/

    VOS_UINT32                                ulRarTimerOut;
    VOS_UINT32                                ulContentTimerOut;
    VOS_UINT32                                ulRaCnfTimerOut;

    VOS_UINT32                                ulRaCauseFail;                    /*RA原因错误*/
    VOS_UINT32                                ulSelectGroupABFail;                   /*RA过程中选择群组B的次数*/

    VOS_UINT32                                ulRecvRarNum;
    VOS_UINT32                                ulRecvRarSucc;
    VOS_UINT32                                ulRecvRarFail;
    VOS_UINT32                                ulRarBiNum;                                                    /*RAR解析到BI成功*/
    VOS_UINT32                                ulRarProcSucc;                                                    /*RAR解析到BI成功*/
    VOS_UINT32                                ulNonContentSucc;                                                    /*RAR解析到BI成功*/

    VOS_UINT32                                ulRecvContentionNum;
    VOS_UINT32                                ulWaitContentionProcFail;

    VOS_UINT32                                ulContentTcrntiSucc;                                                    /*RAR解析到BI成功*/
    VOS_UINT32                                ulContentCrntiSucc;                                                    /*RAR解析到BI成功*/
    VOS_UINT32                                ulContentNoMatch;                                                    /*RAR解析到BI成功*/

    LTE_ERRLOG_MAC_DEBUG_ONRAND_SCHED_INFO_STRU         stOnRand;

    VOS_UINT32                                ulMacNewMsg3Rrctrig;
    VOS_UINT32                                ulMacNewMsg3Mactrig;
    VOS_UINT32                                ulMacNewMsg3MacPdu;
    VOS_UINT32                                ulHoMsg3NoRecvCmp;                /*消息3没有收到Ho Complete消息*/
    VOS_UINT32                                ulMacRetranMsg3Num;
    VOS_UINT32                                ulMacActiveRetranMsg3Num;
    VOS_UINT32                                ulMacOverMsg3MaxRetran;

    VOS_UINT32                                ulStateErrorDrop;
    VOS_UINT32                                ulSendRlcSetupOkNum;              /*MAC下行竞争解决匹配成功后解析eNB发给UE的CCCH数据,MAC成功发到RLC*/
    VOS_UINT32                                ulRarSchedNotFinishNum;           /*RAR授权调度MAC PDU在n-2子帧头还未完成搬移的统计量*/
    VOS_UINT32                                ulNoRarCfgCnfNum;                 /*MAC在n-2子帧头还未读到DSP对RAR CONFIG的确认原语*/
}LTE_ERRLOG_MAC_DEBUG_RA_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UL_ENTITY_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP 上行实体参数
*****************************************************************************/
typedef struct
{
    /*配置参数*/
    VOS_UINT8                           enDiscardTimerFlag;
    VOS_UINT8                           ucHdrLen;           /*PDCP头长度*/
    VOS_UINT16                          usSnMod;            /*序列号的模*/
    VOS_UINT32                          ulMaxPdcpSn;        /*最大的PDCP序列号:4095*/
    VOS_UINT32                          ulDiscardTimerLen;  /*丢弃定时器时长*/
}LTE_ERRLOG_PDCP_UL_ENTITY_PARA_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UL_VARIABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP 上行实体变量维护结构
*****************************************************************************/
typedef struct
{
    /*序列号维护*/
    VOS_UINT32                          ulNextTxSn;         /*下一个要发送SDU的序列号*/
    VOS_UINT32                          ulTxHfn;            /*发送序列号的溢出计数器*/
    VOS_UINT32                          ulCount;            /*用于加解密、完整性验证的COUNT值*/
}LTE_ERRLOG_PDCP_UL_VARIABLE_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UL_BSR_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LPDCP上行BSR结果
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulRecvSduLen;
    VOS_UINT32                          ulSendSduLen;
    VOS_UINT32                          ulReTxPduLen;
}LTE_ERRLOG_PDCP_UL_BSR_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_AM_UL_ENTITY_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP AM模式，上行实体参数
*****************************************************************************/
typedef struct
{
    /*配置参数*/
    VOS_UINT8                           enStatReportFlag;
    VOS_UINT8                           aucRev[3];

}LTE_ERRLOG_PDCP_AM_UL_ENTITY_PARA_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_AM_UL_VARIABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP AM模式，变量维护结构
*****************************************************************************/
typedef struct
{
    /*序列号维护*/
    VOS_UINT32                          ulNextAckSn;        /*AM模式待确认的最小序列号*/
}LTE_ERRLOG_PDCP_AM_UL_VARIABLE_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_AM_UL_ENTITY_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP AM模式，上行实体信息结构
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_PDCP_AM_UL_ENTITY_PARA_STRU        stAmUlPara;         /*实体参数*/
    LTE_ERRLOG_PDCP_AM_UL_VARIABLE_STRU           stVariable;         /*变量维护结构*/
}LTE_ERRLOG_PDCP_AM_UL_ENTITY_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UM_UL_ENTITY_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP UM模式，上行实体参数结构
*****************************************************************************/
typedef struct
{
    /*配置参数*/
    VOS_UINT8                           enUmSnBit;          /*UM模式序列号位数*/
    VOS_UINT8                           aucRev[3];
}LTE_ERRLOG_PDCP_UM_UL_ENTITY_PARA_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UM_UL_VARIABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP UM模式，变量维护结构
*****************************************************************************/
typedef struct
{
    /*暂无*/
    VOS_UINT32                          ulRev;
}LTE_ERRLOG_PDCP_UM_UL_VARIABLE_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UM_UL_ENTITY_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP UM模式，上行实体信息结构
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_PDCP_UM_UL_ENTITY_PARA_STRU        stUmUlPara;         /*实体参数*/
    LTE_ERRLOG_PDCP_UM_UL_VARIABLE_STRU           stVariable;         /*序列号维护结构*/
}LTE_ERRLOG_PDCP_UM_UL_ENTITY_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UL_ENTITY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP上行实体结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRbId;             /*[0,31],实体对应的RBID*/
    VOS_UINT8                           enUlState;          /*LPDCP上行实体的状态*/
    VOS_UINT8                           enRlcModeChoice;    /*PDCP实体数据传输采用的RLC模式 */
    VOS_UINT8                           ucRev;

    LTE_ERRLOG_PDCP_UL_ENTITY_PARA_STRU           stUlPara;
    LTE_ERRLOG_PDCP_UL_VARIABLE_STRU              stUlVariable;

    LTE_ERRLOG_PDCP_UL_BSR_INFO_STRU              stBsrInfo;          /*BSR统计*/
    union
    {
        LTE_ERRLOG_PDCP_AM_UL_ENTITY_INFO_STRU    stAmEntity;
        LTE_ERRLOG_PDCP_UM_UL_ENTITY_INFO_STRU    stUmEntity;
    }u;
}LTE_ERRLOG_PDCP_UL_ENTITY_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_DL_ENTITY_PARA_STRU;
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP 下行实体参数结构
*****************************************************************************/
typedef struct
{
    /*配置参数*/
    VOS_UINT8                           ucHdrLen;           /*PDCP头长度*/
    VOS_UINT8                           ucRev;
    VOS_UINT16                          usSnMod;            /*序列号的模*/
    VOS_UINT32                          ulMaxPdcpSn;        /*最大的PDCP序列号*/
}LTE_ERRLOG_PDCP_DL_ENTITY_PARA_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_DL_VARIABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP 下行变量维护结构
*****************************************************************************/
typedef struct
{
    /*序列号维护变量*/
    VOS_UINT32                          ulNextRxSn;
    VOS_UINT32                          ulRxHfn;            /*接收序列号的溢出计数器*/
    VOS_UINT32                          ulCount;            /*用于加解密、完整性验证的COUNT值*/
}LTE_ERRLOG_PDCP_DL_VARIABLE_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_AM_DL_ENTITY_PARA_STRU;
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP AM模式，下行实体参数结构
*****************************************************************************/
typedef struct
{
    /*配置参数*/
    VOS_UINT8                           enStatReportFlag;
    VOS_UINT8                           aucRev[3];

}LTE_ERRLOG_PDCP_AM_DL_ENTITY_PARA_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_AM_DL_VARIABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP AM模式，下行变量维护结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulLastSubmitSn;     /*最近一个递交的SDU对应的SN*/
    VOS_UINT32                          ulSduCnt;     /*SDU计数*/
}LTE_ERRLOG_PDCP_AM_DL_VARIABLE_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_AM_DL_ENTITY_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP AM模式，下行实体信息结构
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_PDCP_AM_DL_ENTITY_PARA_STRU        stAmDlPara;         /*实体参数*/
    LTE_ERRLOG_PDCP_AM_DL_VARIABLE_STRU           stVariable;         /*序列号维护结构*/
}LTE_ERRLOG_PDCP_AM_DL_ENTITY_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UM_DL_ENTITY_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP UM模式，下行实体参数结构
*****************************************************************************/
typedef struct
{
    /*配置参数*/
    VOS_UINT8                           enUmSnBit;          /*UM模式序列号位数*/
    VOS_UINT8                           aucRev[3];
}LTE_ERRLOG_PDCP_UM_DL_ENTITY_PARA_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UM_DL_VARIABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP UM模式，序列号维护变量结构
*****************************************************************************/
typedef struct
{
    /*暂无*/
    VOS_UINT32                          ulRev;
}LTE_ERRLOG_PDCP_UM_DL_VARIABLE_STRU;

/*****************************************************************************
 结构名    : LPDCP_UM_DL_ENTITY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP UM模式，下行实体信息结构
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_PDCP_UM_DL_ENTITY_PARA_STRU        stUmDlPara;         /*实体参数*/
    LTE_ERRLOG_PDCP_UM_DL_VARIABLE_STRU           stVariable;         /*序列号维护结构*/
}LTE_ERRLOG_PDCP_UM_DL_ENTITY_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_DL_ENTITY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP下行实体结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRbId;             /*[0,31],实体对应的RBID*/
    VOS_UINT8                           enDlState;          /*LPDCP下行实体的状态*/
    VOS_UINT8                           enRlcModeChoice;    /*PDCP实体数据传输采用的RLC模式 */
    VOS_UINT8                           ucRev;

    LTE_ERRLOG_PDCP_DL_ENTITY_PARA_STRU           stDlPara;
    LTE_ERRLOG_PDCP_DL_VARIABLE_STRU              stDlVariable;

    union
    {
        LTE_ERRLOG_PDCP_AM_DL_ENTITY_INFO_STRU    stAmEntity;
        LTE_ERRLOG_PDCP_UM_DL_ENTITY_INFO_STRU    stUmEntity;
    }u;
}LTE_ERRLOG_PDCP_DL_ENTITY_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_UL_DT_RBID_STAT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP上行各RBID对应接收和发送的SDU、PDU相关统计量
*****************************************************************************/
typedef struct
{
    VOS_UINT32      ulUlRxSduCnt;           /*接收到的SDU统计*/
    VOS_UINT32      ulUlTxPduCnt;           /*发送出去的PDU统计*/
    VOS_UINT32      ulUlAckedSduCnt;        /*已收到确认的SDU统计*/

    VOS_UINT32      ulUlDiscardMaxCnt; /*丢弃最大值统计*/
    VOS_UINT32      ulUlDisSduCnt;          /*定时器超时丢弃SDU统计*/
    VOS_UINT32      ulUlBufFullDisSduCnt;   /*重传buf超限丢弃SDU统计*/
    VOS_UINT32      ulUlReestDiscardCnt;   /*重建立丢弃统计*/

    VOS_UINT32      ulUlInvalidSduCnt;      /*接收到的非法SDU统计*/
    VOS_UINT32      ulUlTxCtrlPduNum;       /*实体发送的控制PDU统计*/
    VOS_UINT32      ulUlAckEndSnError;      /*上行RLCACK SN错误*/

    VOS_UINT32      ulUlReestStopAckSn;
    VOS_UINT32      ulUlReestStopNextTxSn;
    VOS_UINT32      ulUlReestStopHfn;
    VOS_UINT32      ulUlReestContAckSn;
    VOS_UINT32      ulUlReestContNextTxSn;
    VOS_UINT32      ulUlReestContFirstTxSn;
    VOS_UINT32      ulUlReestContHfn;

    VOS_UINT32      ulCompSuccCnt;          /*压缩成功次数*/
    VOS_UINT32      ulCompFailCnt;          /*压缩失败次数*/
    VOS_UINT32      ulRohcCompCnt;          /*压缩处理次数*/

    VOS_UINT32      ulSduEnQueFailCnt;

}LTE_ERRLOG_PDCP_UL_DT_RBID_STAT_STRU;

/*****************************************************************************
 结构名    : LPDCP_DL_RBID_STAT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP下行各RBID接收和发送的PDU、SDU相关统计量
*****************************************************************************/
typedef struct
{
    VOS_UINT32      ulDlRxPduCnt;           /*接收的PDU数量*/
    VOS_UINT32      ulDlTxSduCnt;           /*发送的SDU数量*/
    VOS_UINT32      ulDlDiscPduCnt;         /*重排序窗口外丢弃统计*/
    VOS_UINT32      ulDlDupPduCnt;          /*重复接收PDU统计*/
    VOS_UINT32      ulDlRxCtlPduCnt;        /*接收控制PDU统计*/
    VOS_UINT32      ulDlTxStrpPduCnt;       /*发送状态报告PDU统计*/
    VOS_UINT32      ulDlTxRohcPduCnt;       /*发送ROHC反馈PDU统计*/
    VOS_UINT32      ulDlErrPduRlcIndCnt;    /*RLC数据指示PDU错误统计*/
    VOS_UINT32      ulDlExcepFreePduCnt;    /*异常释放PDU统计*/
    VOS_UINT32      ulDlReestStopLastSubmitSn;
    VOS_UINT32      ulDlReestStopNextRxSn;
    VOS_UINT32      ulDlReestContLastSubmitSn;
    VOS_UINT32      ulDlReestContHfn;
    VOS_UINT32      ulDlReestStopHfn;
    VOS_UINT32      ulDlReestContNextRxSn;
    VOS_UINT32      ulDlReestContFirstRxSn;

    VOS_UINT32      ulDlExecReordCnt;        /*下行实体建立以来，重排序执行次数*/
    VOS_UINT32      ulDlCipherBufFullCnt;       /*下行CIPEHR入口buf计数*/
    VOS_UINT32      ulDlAmRlcDataErrCnt;
    VOS_UINT32      ulDlRlcDataErrCnt;

    VOS_UINT32      ulDlUnCompFailCnt;      /*SDU解压缩失败数量*/
    VOS_UINT32      ulHoUnCompFailCnt;      /*切换解压缩失败数量*/
    VOS_UINT32      ulDeCompSuccCnt;        /*解压缩成功次数*/
    VOS_UINT32      ulDeCompFailCnt;        /*解压缩失败次数*/
    VOS_UINT32      ulRohcDeCompCnt;        /*解压缩处理次数*/

}LTE_ERRLOG_PDCP_DL_DT_RBID_STAT_STRU;

/*****************************************************************************
 结构名    : LPDCP_UL_SECU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP安全信息结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          enCipherAlg;
    VOS_UINT32                          enIntegrityProtAlg;
    VOS_UINT8                           ucSrbCiphKeyIndex;
    VOS_UINT8                           ucDrbCiphKeyIndex;
    VOS_UINT8                           ucIntegrityKeyIndex;
    VOS_UINT8                           aucRev;
}LTE_ERRLOG_PDCP_SECU_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_PDCP_FAIL_STRU
 结构说明  : PDCP 错误信息上报内容
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_PDCP_UL_ENTITY_STRU    stPdcpUlEntity;
    LTE_ERRLOG_PDCP_DL_ENTITY_STRU    stPdcpDlEntity;
    LTE_ERRLOG_PDCP_UL_DT_RBID_STAT_STRU      stUlDtRbidStat;
    LTE_ERRLOG_PDCP_DL_DT_RBID_STAT_STRU      stDlDtRbidStat;
    LTE_ERRLOG_PDCP_SECU_INFO_STRU    stSecuInfo;

}LTE_PDCP_FAIL_STRU;

/*****************************************************************************
 结构名    : LTE_RLC_FAIL_STRU
 结构说明  : RLC错误信息上报内容
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_RLC_RX_ENTITY_STRU     stRlcRxEntity;
    LTE_ERRLOG_RLC_TX_ENTITY_STRU     stRlcTxEntity;
    LTE_ERRLOG_RLC_UL_STAT_INFO_STRU  stRlcULStatInfo;
    LTE_ERRLOG_RLC_DL_STAT_INFO_STRU  stRlcDLStatInfo;
    
}LTE_RLC_FAIL_STRU;

/*****************************************************************************
 结构名    : LTE_MAC_FAIL_STRU
 结构说明  : MAC错误信息上报内容
*****************************************************************************/
typedef struct
{
    LTE_ERRLOG_MAC_ENTITY_STRU  stMacEnt;
    LTE_ERRLOG_MAC_DEBUG_UL_STAT_INFO_STRU  stMacULStat;
    LTE_ERRLOG_MAC_DEBUG_DL_STAT_INFO_STRU  stMacDLStat;
    LTE_ERRLOG_MAC_DEBUG_RA_INFO_STRU  stMacRaStat;
    
}LTE_MAC_FAIL_STRU;

/*****************************************************************************
 结构名    : LTE_L2_ERRLOG_INFO_STRU
 结构说明  : ERROR LOG 数据内容结构
*****************************************************************************/
typedef struct
{
    TL_L2_ERRLOG_ALMLEVEL_ENUM_UINT16    almLevel;
    TL_L2_ERROR_CODE_ENUM_UINT8          errorCode;
    TL_L2_ERRLOG_MODULE_TYPE_UINT8       moduleType;
    
    union
    {
        LTE_PDCP_FAIL_STRU                   stPdcpFailInfo;
        LTE_RLC_FAIL_STRU                    stRlcFailInfo;
        LTE_MAC_FAIL_STRU                    stMacFailInfo;
    }u;
    
}LTE_L2_ERRLOG_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_L2_OM_ERRLOG_INFO_STRU
 结构说明  : ERROR LOG 消息结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                        ulMsgModuleID;    /* 子模块ID */
    VOS_UINT16                        usModemId;         /* 00：主卡, 01：副卡 ,10/11:保留  */
    VOS_UINT16                        usALMID;             /*  sub sys,modeule,sub module   暂不使用   */
    VOS_UINT16                        usALMLevel;         /*上报级别; Warning:0x04; Minor:0x03; Major:0x02;  Critical:0x01; */
    VOS_UINT16                        usALMType;         /* 故障&告警类型; 通信:0x00; 业务质量:0x01; 处理出错:0x02; 设备故障:0x03; 环境故障:0x04;*/
    VOS_UINT32                        usAlmLowSlice;     /*时间戳*/
    VOS_UINT32                        usAlmHighSlice;
    VOS_UINT32                        ulAlmLength;
    LTE_L2_ERRLOG_INFO_STRU           stAlmInfo;
}LTE_L2_OM_ERRLOG_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_L2_OM_ERRLOG_REPORT_STRU
 结构说明  : errorlog消息上报结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                              ulMsgName;
    VOS_UINT32                              ulMsgType;
    VOS_UINT32                              ulMsgSN;
    VOS_UINT32                              ulRptlen;             /* 故障内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    LTE_L2_OM_ERRLOG_INFO_STRU              stErrLogInfo;
}LTE_L2_OM_ERRLOG_REPORT_STRU;

typedef struct
{
  VOS_UINT16       frequency;
  VOS_UINT8        syncId;
  VOS_UINT8        basicMidamble;
}TDS_ERRLOG_CellDesc;

typedef struct
{
  VOS_UINT8                      timeslot;
  VOS_UINT16                     chanCodes;/*bitmap:sf=16*/
  VOS_UINT8                      midambleAllocationMode;/*midambleShift is useless if defaultMidamble,*/
                                                 /*commonMidamble*/
  VOS_UINT8                      midambleShift;
  VOS_UINT8                      midambleConfiguration;
  VOS_UINT8                      wi;
}TDS_ERRLOG_FpachInfo;


typedef struct
{
  TDS_ERRLOG_CellDesc            cellDesc;
  TDS_ERRLOG_FpachInfo           fpachInfo;
  VOS_UINT8                      syncUlCode;
  VOS_UINT8                      prxUppchDes;
  VOS_UINT8                      pathLoss;
  VOS_UINT8                      powerRampStep;
  VOS_UINT8                      maxSync;
  VOS_UINT32                     uRnti;
  VOS_UINT32                     cRnti;
  VOS_UINT8                      syncNeeded;
  VOS_UINT8                      n313;
  VOS_UINT16                     n312;
  VOS_UINT16                     n315;
  VOS_UINT8                      t312;
  VOS_UINT8                      t313;
  VOS_UINT8                      rbNum;
  VOS_UINT8                      rabNum;
  VOS_UINT8                      trchNum;
  VOS_UINT8                      harqInfo;
  VOS_UINT8                      eDchMacdFlowNum;
}TDS_ERRLOG_CMAC_DCH_CONFIG_REQ;

typedef struct
{
  VOS_UINT8                          sccpchId;
  VOS_UINT8                          trchNum;
  VOS_UINT8                          tfcNum;
  VOS_UINT16                         channelisationCode;
}TDS_ERRLOG_SccpchInfo;

typedef struct
{
  VOS_UINT16                     usOpId;
  VOS_UINT32                     uRnti;
  VOS_UINT32                     cRnti;
  VOS_UINT8                      rbNum;
  VOS_UINT8                      ascNum;
  TDS_ERRLOG_SccpchInfo   sccpchInfo;
  VOS_UINT8                      fachMeasOccasionCoeff;
  VOS_UINT8                      measNum;
  VOS_INT16                     framNumDiff;
  VOS_UINT8                      cellUpdate;
  VOS_UINT8                      barredAndIntraNotAllowedNum;
}TDS_ERRLOG_CMAC_FCH_CONFIG_REQ;

typedef struct
{
  VOS_UINT8                      measNum;/*no BA ：0*/
  TDS_ERRLOG_CellDesc            cellList[TDS_ERRLOG_MAX_FAR_CELL_NUM];
  VOS_UINT8                      icsType;/*sel ：0；res ：1；only measurement：2    7:search plmn*/
  VOS_UINT8                      rrcMode;/*idle mode: 0; connected mode:1*/
  VOS_UINT8                      syncStatus; /*20090409*/
  VOS_UINT8                      dmPara;/*suspend TDD meas :0; deactive L1G :1; 3G to 2G fail, return 3G;  20090928  PDM100 UMAC*/
}TDS_ERRLOG_CMAC_FIND_BCH_REQ;

typedef struct
{
  TDS_ERRLOG_CellDesc            cellDesc;
  VOS_INT16                      framNumDiff;
  VOS_UINT8                      scanRlcFlag;/*not need scan rlc when entering FCH state:0;need scan rlc:1*/
  VOS_UINT8                      maxAllowedUlTxPower;
  VOS_UINT8                      syncUlCode;/*bitmap*/
  VOS_UINT8                      prxUppchDes;
  VOS_UINT8                      pathLoss;
  VOS_UINT8                      powerRampStep;
  VOS_UINT8                      maxSync;/*MAX_SYNC_NUM_UL_Transmissions*/
  VOS_UINT8                      mmax;
  VOS_UINT8                      dynamicPersLevel;/*N for P(N)＝2^-(N-1)*/
  VOS_UINT8                      ascNum;
}TDS_ERRLOG_CMAC_RANDOM_ACC_REQ;


/*****************************************************************************
 结构名    : TDS_MAC_FAIL_STRU
 结构说明  : MAC错误信息上报内容
*****************************************************************************/
typedef struct
{
    VOS_UINT32                        stMacCurrentState;
    TDS_ERRLOG_CMAC_DCH_CONFIG_REQ    stMacDchConfigInfo;
    TDS_ERRLOG_CMAC_FCH_CONFIG_REQ    stMacFchConfigInfo;
    TDS_ERRLOG_CMAC_FIND_BCH_REQ      stMacBchInfo;
    TDS_ERRLOG_CMAC_RANDOM_ACC_REQ    stMacRAInfo;
    
}TDS_MAC_FAIL_STRU;

typedef struct
{
  VOS_UINT8                        rbIdentity;
  VOS_UINT8                        upMode;
  VOS_UINT8                        dwMode;
  VOS_UINT8                        ctrlOrUsr;
  VOS_UINT16                       currentState;
}TDS_ERRLOG_RLC_TmUmInfo;

typedef struct
{
  VOS_UINT16                       pduSize;
  VOS_UINT8                        transmissionRlcDiscardChoice;
  VOS_UINT16                       timerDiscard;
  VOS_UINT8                        maxMrw;
  VOS_UINT16                       timerMrw;
  VOS_UINT8                        maxDat;
  VOS_UINT16                       transmissionWindowSize;
  VOS_UINT16                       timerRst;
  VOS_UINT8                        maxRst;
  VOS_UINT8                        PollingInfoPresent;      /*0: no pollingInfo; 1: have pollingInfo*/
  VOS_UINT16                       timerPollProhibit;       /*optional, 0xff indicate  don't usel*/
  VOS_UINT16                       timerPoll;               /*optional, 0xff indicate  don't usel*/
  VOS_UINT8                        pollPdu;                 /*optional, 0xff indicate  don't usel*/
  VOS_UINT8                        pollSdu;                 /*optional, 0xff indicate  don't usel*/
  VOS_UINT8                        lastTransmissionPduPoll;
  VOS_UINT8                        lastRetransmissionPduPoll;
  VOS_UINT8                        pollWindow;              /*optional, 0xff indicate  don't usel*/
  VOS_UINT8                        timerPollPeriodic;       /*optional, 0xff indicate  don't usel*/
 }TDS_ERRLOG_SaveUpAmParas;

typedef struct
{
  VOS_UINT16                       pduSize;/*20060710*/
  VOS_UINT8                        inSequenceDelivery;      /* 0:unSequenceDelivery 1:inSequenceDelivery*/
  VOS_UINT16                       receivingWindowSize;
  VOS_UINT16                       timerStatusProhibit;     /*optional, 0xff indicate timer don't use*/
  VOS_UINT8                        missingPduIndicator;
  VOS_UINT8                        timerStatusPeriodic;     /*optional, 0xff indicate timer don't use*/
 }TDS_ERRLOG_SaveAmDwParas;

typedef struct
{
  VOS_UINT8                        rbIdentity;
  VOS_UINT8                        ctrlOrUsr;
  VOS_UINT16                       currentState;
  VOS_UINT16                       previousState;
  TDS_ERRLOG_SaveUpAmParas           upParas;
  TDS_ERRLOG_SaveAmDwParas           dwParas;
}TDS_ERRLOG_RLC_AmInfo;


/*****************************************************************************
 结构名    : TDS_RLC_FAIL_STRU
 结构说明  : RLC错误信息上报内容
*****************************************************************************/
typedef struct
{

    TDS_ERRLOG_RLC_TmUmInfo     stRlcTmUmInst;
    TDS_ERRLOG_RLC_AmInfo       stRlcAmInst;
}TDS_RLC_FAIL_STRU;

/*****************************************************************************
 结构名    : TDS_PDCP_FAIL_STRU
 结构说明  : pdcp错误信息上报内容
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  usRev;
}TDS_PDCP_FAIL_STRU;


/*****************************************************************************
 结构名    : TDS_L2_ERRLOG_INFO_STRU
 结构说明  : ERROR LOG 数据内容结构
*****************************************************************************/
typedef struct
{
    TL_L2_ERRLOG_ALMLEVEL_ENUM_UINT16    almLevel;
    TL_L2_ERROR_CODE_ENUM_UINT8          errorCode;
    TL_L2_ERRLOG_MODULE_TYPE_UINT8       moduleType;
    
    union
    {
        TDS_PDCP_FAIL_STRU                   stPdcpFailInfo;
        TDS_RLC_FAIL_STRU                    stRlcFailInfo;
        TDS_MAC_FAIL_STRU                    stMacFailInfo;
    }u;
    
}TDS_L2_ERRLOG_INFO_STRU;

/*****************************************************************************
 结构名    : TDS_L2_OM_ERRLOG_INFO_STRU
 结构说明  : ERROR LOG 消息结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                        ulMsgModuleID;    /* 子模块ID */
    VOS_UINT16                        usModemId;         /* 00：主卡, 01：副卡 ,10/11:保留  */
    VOS_UINT16                        usALMID;             /*  sub sys,modeule,sub module   暂不使用   */
    VOS_UINT16                        usALMLevel;         /*上报级别; Warning:0x04; Minor:0x03; Major:0x02;  Critical:0x01; */
    VOS_UINT16                        usALMType;         /* 故障&告警类型; 通信:0x00; 业务质量:0x01; 处理出错:0x02; 设备故障:0x03; 环境故障:0x04;*/
    VOS_UINT32                        usAlmLowSlice;     /*时间戳*/
    VOS_UINT32                        usAlmHighSlice;
    VOS_UINT32                        ulAlmLength;
    TDS_L2_ERRLOG_INFO_STRU           stAlmInfo;
}TDS_L2_OM_ERRLOG_INFO_STRU;

/*****************************************************************************
 结构名    : TDS_L2_OM_ERRLOG_REPORT_STRU
 结构说明  : errorlog消息上报结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                              ulMsgName;
    VOS_UINT32                              ulMsgType;
    VOS_UINT32                              ulMsgSN;
    VOS_UINT32                              ulRptlen;             /* 故障内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    TDS_L2_OM_ERRLOG_INFO_STRU              stErrLogInfo;
}TDS_L2_OM_ERRLOG_REPORT_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/



#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TLL2Errorlog.h */
