/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AdsDownLink.h
  版 本 号   : 初稿
  作    者   : 鲁琳/l60609
  生成日期   : 2011年12月7日
  最近修改   :
  功能描述   : AdsDlProcData.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年12月7日
    作    者   : 鲁琳/l60609
    修改内容   : 创建文件

******************************************************************************/

#ifndef __ADSDOWNLINK_H__
#define __ADSDOWNLINK_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "AdsCtx.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 定义ADS下行IP TYPE */
#define ADS_DL_IP_TYPE_FLAG                     (0x0200)

/* bd_cd_noeqBD中len和CD的长度不等错误提示 */
#define ADS_DL_IPF_RD_RSLT_BDCD_LEN_ERR_FLG     (0x4000)    /* 0100 0000 0000 0000 B */

/* pkt_parse_err数据解析错误指示 */
#define ADS_DL_IPF_RD_RSLT_PKT_PARSE_ERR_FLG    (0x2000)    /* 0010 0000 0000 0000 B */

/* bd_pkt_noeqBD中len和IP包头指示的len不等错误指示 */
#define ADS_DL_IPF_RD_RSLT_BD_PKT_LEN_ERR_FLG   (0x1000)    /* 0001 0000 0000 0000 B */

/* head_len_err IPV4长度错误指示信号 */
#define ADS_DL_IPF_RD_RSLT_HDR_LEN_ERR_FLG      (0x0800)    /* 0000 1000 0000 0000 B */

/* version_err版本号错误指示 */
#define ADS_DL_IPF_RD_RSLT_VER_ERR_FLG          (0x0400)    /* 0000 0100 0000 0000 B */

/* 定义IPF BEARID 错包标志位 */
#define ADS_DL_IPF_BEARID_ERR_PKT_FLAG          (ADS_DL_IPF_RD_RSLT_BDCD_LEN_ERR_FLG |  \
                                                 ADS_DL_IPF_RD_RSLT_PKT_PARSE_ERR_FLG | \
                                                 ADS_DL_IPF_RD_RSLT_HDR_LEN_ERR_FLG |   \
                                                 ADS_DL_IPF_RD_RSLT_VER_ERR_FLG)

/* 检查是RD Result, 是否为异常Packet */
#define ADS_DL_IPF_RD_RSLT_IS_ERR_PKT(usRslt)       ((usRslt) & ADS_DL_IPF_BEARID_ERR_PKT_FLAG)

/* Added by l60609 for V3R3 PPP RPOJECT 2013-06-07, Begin */
/* 检查是RD Result, 是否为Version错包 */
#define ADS_DL_IPF_RD_RSLT_IS_VER_ERR_PKT(usRslt)   ((usRslt) & ADS_DL_IPF_RD_RSLT_VER_ERR_FLG)
/* Added by l60609 for V3R3 PPP RPOJECT 2013-06-07, End */

/* 从RD Out域获取搬移后的数据包 */
#if(FEATURE_OFF == FEATURE_SKB_EXP)
#define ADS_DL_GET_DATA_FROM_IPF_OUT(ulOut)         (phys_to_virt(ulOut))
#else
#define ADS_DL_GET_DATA_FROM_IPF_OUT(ulOut)         ((VOS_UINT8*)(ulOut))
#endif

/* 从RD PktLen域获取搬移后的数据包长度 */
#define ADS_DL_GET_LEN_FROM_IPF_PKT_LEN(usPktLen)   ((VOS_UINT16)(usPktLen))

/* 从RD User Field 1域获取C核TTF Memory */
#define ADS_DL_GET_TTFMEM_FROM_IPF_USR2(ulUsr2)     ((VOS_VOID*)(ulUsr2))

/* 从RD Result域获取C核IP包类型 */
#define ADS_DL_GET_IP_TYPE_FROM_IPF_RSLT(usRslt)    ((ADS_PKT_TYPE_ENUM_UINT8)(((ADS_DL_IPF_RESULT_STRU*)&(usRslt))->usIpType))

/* Modified by l60609 for DSDA Phase II, 2012-11-24, Begins */
/* RD的user field 1，高1byte为Modem id，低1byte为Rab Id，获取MODEM ID */
#define ADS_DL_GET_MODEM_ID_FROM_RD_USER_FIELD1(i)  ((i >> 8) & 0xFF)

/* RD的user field 1，高1byte为Modem id，低1byte为Rab Id，获取RAB ID */
#define ADS_DL_GET_RAB_ID_FROM_RD_USER_FIELD1(i)    (i & 0xFFU)

/* 下行AD0数据包的长度 */
#define ADS_DL_AD0_DATA_LEN                          (448)

/* 下行AD1数据包的长度 */
#define ADS_DL_AD1_DATA_LEN                          (1536 + 14)
/* Modified by l60609 for DSDA Phase II, 2012-11-24, End */

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


typedef struct
{
    VOS_UINT16                          usBearedId:6;
    VOS_UINT16                          usIpSegInfo:2;
    VOS_UINT16                          usUpperLayer:1;
    VOS_UINT16                          usIpType:1;
    VOS_UINT16                          usVerErr:1;
    VOS_UINT16                          usHdrLenErr:1;
    VOS_UINT16                          usPktLenNotEq:1;
    VOS_UINT16                          usPktLenErr:1;
    VOS_UINT16                          usBdCdNotEq:1;
    VOS_UINT16                          usResv:1;
} ADS_DL_IPF_RESULT_STRU;

/*****************************************************************************
结构名    : ADS_DL_RD_STRU
结构说明  : ADS保存下行RD信息

  1.日    期   : 2011年12月6日
    作    者   : 鲁琳/l60609
    修改内容   : 定义ADS保存下行RD信息
*****************************************************************************/
typedef struct
{
    VOS_UINT8                            ucRabId;                               /* Rab Id*/
    ADS_PKT_TYPE_ENUM_UINT8              enPktType;                             /* Pkt Type*/
    VOS_UINT8                            aucRsv[6];                             /* 保留 */
    CDS_ADS_DL_IPF_BEARER_ID_ENUM_UINT32 enBearId;                              /* Bear Id*/
    VOS_UINT32                           ulLen;                                 /* 搬移后的数据包长度 */
    VOS_UINT8                           *pucData;                               /* 搬移后的数据包指针 */
    VOS_VOID                            *pTtfMem;                               /* 目的TTF指针*/
} ADS_DL_RD_STRU;



/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_VOID ADS_DL_ProcMsg(MsgBlock* pMsg);
VOS_VOID ADS_DL_ProcRd(
    IPF_RD_DESC_S                      *pstRdDesc
);
VOS_UINT32 ADS_DL_RcvAtMsg(MsgBlock* pMsg);
VOS_UINT32 ADS_DL_RcvTafPdpStatusInd(MsgBlock *pMsg);
VOS_UINT32 ADS_DL_RcvCdsMsg(MsgBlock *pMsg);
VOS_INT32 ADS_DL_IpfIntCB(VOS_VOID);
VOS_VOID ADS_DL_SendNdClientDataInd(
    IPF_RD_DESC_S                      *pstRdDesc
);
#if 0
VOS_VOID ADS_DL_SendNdClientErrInd(
    IPF_RD_DESC_S                      *pstRdDesc
);
#endif

VOS_VOID ADS_DL_ProcIpfResult(VOS_VOID);
VOS_VOID ADS_DL_RcvTiProtectExpired(
    VOS_UINT32                          ulParam,
    VOS_UINT32                          ulTimerName
);
VOS_UINT32 ADS_DL_IsFcAssemTuneNeeded(VOS_UINT32 ulRdNum);

/* Added by l60609 for DSDA Phase II, 2012-12-20, Begin */
#if (FEATURE_OFF == FEATURE_SKB_EXP)
VOS_INT32 ADS_DL_IpfAdqEmptyCB(IPF_ADQ_EMPTY_E eAdqEmpty);
VOS_VOID ADS_DL_ProcIpfAdqEmtpyEvent(VOS_VOID);
VOS_VOID ADS_DL_RcvTiAdqEmptyExpired(
    VOS_UINT32                          ulParam,
    VOS_UINT32                          ulTimerName
);
VOS_VOID ADS_DL_ConfigAdq(
    IPF_AD_TYPE_E                       enAdType,
    VOS_UINT                            ulIpfAdNum,
    VOS_UINT                           *pulActAdNum
);
VOS_VOID ADS_DL_ProcAdq(VOS_VOID);
VOS_VOID ADS_DL_InitAdq(VOS_VOID);
#endif
/* Added by l60609 for DSDA Phase II, 2012-12-20, End */

VOS_UINT32 ADS_DL_RcvCcpuResetStartInd(
    MsgBlock                           *pstMsg
);
VOS_UINT32 ADS_DL_RcvCcpuResetEndInd(
    MsgBlock                           *pstMsg
);
VOS_VOID ADS_DL_FreeIpfUsedAd0(VOS_VOID);
VOS_VOID ADS_DL_FreeIpfUsedAd1(VOS_VOID);

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

#endif /* end of AdsDlProcData.h */
