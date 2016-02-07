/************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : ppp_input.h
  Author       : ---
  Version      : V200R001
  Date         : 2005-11-18
  Description  : 该文件给出了PPP模块供AT_I/O与AT_DATA模块调用的输入函数
                 的引用说明。

  Function List:
        ---
        ---
        ---
  History      :
  1. Date:2005-11-18
     Author: ---fanzhibin 49086
     Modification:Create
  2. Date:2006-05-17
     Author: L47619
     Modification:Modified for PN:A32D04462
 ************************************************************************/
#ifndef __PPP_INPUT_H__
#define __PPP_INPUT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "ppp_public.h"

#pragma pack(4)

/*****************************************************************************
  2 消息类型定义
*****************************************************************************/
enum PPP_MSG_TYPE_ENUM
{
#if (PPP_FEATURE == PPP_FEATURE_PPP)
    PPP_DATA_PROC_NOTIFY = 1,       /*PPP数据处理指示*/
    PPP_AT_CTRL_OPERATION,          /* _H2ASN_MsgChoice PPP_AT_CTRL_OPERATION_MSG */
    PPP_PACKET_FROM_GGSN,           /*为了加快传输速度，从GGSN来的报文改为直接调用，非消息机制*/
    PPP_RECV_PROTO_PACKET_TYPE,     /*PPP模块接收到协商包*/
    PPP_SEND_PROTO_PACKET_TYPE,     /*PPP模块发送协商包*/
    AT_PPP_CREATE_PPP_REQ,          /*PPP模块收到AT的CREATE PPP指示*/
    AT_PPP_RELEASE_PPP_REQ,         /*PPP模块收到AT的RELEASE PPP指示*/
    AT_PPP_CREATE_RAW_PPP_REQ,      /*PPP模块收到AT的CREATE RAW PPP指示*/
    AT_PPP_RELEASE_RAW_PPP_REQ,     /*PPP模块收到AT的RELEASE RAW PPP指示*/
    AT_PPP_RECV_RELEASE_IND,        /*PPP模块收到AT的RELEASE PPP IND指示*/
    AT_PPP_RECV_CONFIG_INFO_IND,    /*PPP模块收到AT的CONFIG INFO IND指示*/
    PPP_AT_RECV_CONFIG_INFO_REQ,    /*PPP模块向AT发送CONFIG INFO REQ指示*/
    PPP_CONFIG_INFO_PROC_NOTIFY,    /*PPP模块处理PDP激活指示消息*/
    PPP_PROTOCOL_RELEASED_IND,      /*PPP模块协议终止消息*/
    PPP_RECV_PCO_IPCP_REJ_IND,      /*PPP模块收到的PCO中包含IPCP Reject*/
    PPP_RECV_PCO_IPCP_ACK_IND,      /*PPP模块收到的PCO中包含IPCP ACK*/
    PPP_RECV_PCO_IPCP_NAK_IND,       /*PPP模块收到的PCO中包含IPCP NAK*/
#endif
#ifdef WTTF_PC_ST_SWITCH
    AT_PPP_UL_DATA_REQ,
    PPP_AT_DL_DATA_IND
#endif

};
typedef VOS_UINT32  PPP_MSG_TYPE_ENUM_UINT32;


/*****************************************************************************
  3 宏定义
*****************************************************************************/
/* PPP 协议包处理方向: 接收或发送*/
#define PPP_SEND_OUT_PROTOCOL_FRAME 0
#define PPP_RECV_IN_PROTOCOL_FRAME  1

/*****************************************************************************
  4 枚举定义
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


typedef struct
{
    VOS_MSG_HEADER        /* 消息头 */        /* _H2ASN_Skip */
    VOS_UINT32  ulMsgname;
    VOS_UINT32  ulPppPhase;
    VOS_INT32   ulLcpState;
    VOS_INT32   ulIpcpState;
    VOS_UINT16  usPppId;
    VOS_UINT16  ulDataLen;
} PPP_FRAME_MNTN_INFO_STRU;


typedef struct
{
    VOS_MSG_HEADER        /* 消息头 */        /* _H2ASN_Skip */
    VOS_UINT32  ulMsgname;
    VOS_UINT16  usPppId;
    VOS_UINT16  usReserved;
    VOS_UINT32  ulPppPhase;
    VOS_INT32   ulLcpState;
    VOS_INT32   ulIpcpState;
} PPP_EVENT_MNTN_INFO_STRU;


typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32     ulMsgType;
    union
    {
        PPP_ID        m_PppId;
    }u ;
}PPP_MSG;


typedef struct
{
    VOS_MSG_HEADER                              /* 消息头 */        /*_H2ASN_Skip*/
    PPP_MSG_TYPE_ENUM_UINT32    ulMsgType;      /* 消息类型 */      /*_H2ASN_Skip*/
}PPP_DATA_PROC_NOTIFY_MSG;


typedef struct
{
    VOS_MSG_HEADER                                      /* 消息头 */        /*_H2ASN_Skip*/
    PPP_MSG_TYPE_ENUM_UINT32            ulMsgType;      /* 消息类型 */      /*_H2ASN_Skip*/
    PPP_ID                              usPppId;
    VOS_UINT8                           aucRsv[2];
    PPP_AT_CTRL_OPER_TYPE_ENUM_UINT32   ulCtrlOpType;   /* AT向PPP发送的控制操作的类型 */
} PPP_AT_CTRL_OPERATION_MSG;


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
#ifdef WTTF_PC_ST_SWITCH

typedef struct
{
    VOS_UINT32  ulAccm;
    VOS_UINT32  ulProtoComp;
    VOS_UINT32  ulAcfComp;
} PPP_HDLC_CTRL_STRU;

/* 零拷贝结构桩，下行数据最大1502字节，上行数据最大1536字节 */
typedef struct
{
    VOS_UINT16                          usDataLen;
    VOS_UINT16                          aucData[PPP_ZC_MAX_DATA_LEN];
}PPP_STUB_ZC_UL_DATA_REQ_STRU;

typedef struct
{
    VOS_UINT16                          usDataLen;
    VOS_UINT16                          aucData[PPP_ZC_MAX_DATA_LEN];
}PPP_STUB_ZC_DL_DATA_IND_STRU;

typedef struct
{
    VOS_MSG_HEADER                                                      /* _H2ASN_Skip */
    PPP_MSG_TYPE_ENUM_UINT32      ulMsgType;          /* _H2ASN_IeChoice_Export PPP_ZC_DATA_MSG_TYPE_ENUM_UINT16 */
    union                                                               /* _H2ASN_Skip */
    {                                                                   /* _H2ASN_Skip */
        PPP_STUB_ZC_UL_DATA_REQ_STRU    stUlDataReq;                    /* _H2ASN_Skip */
        PPP_STUB_ZC_DL_DATA_IND_STRU    stDlDataInd;
        /******************************************************************************************************
            _H2ASN_IeChoice_When        PPP_ZC_DATA_MSG_TYPE_ENUM_UINT16
        ******************************************************************************************************/
    }u; /* _H2ASN_Skip */
}PPP_STUB_ZC_DATA_MSG_STRU;

#endif

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/* *****************************************************************************
  9 全局变量声明
***************************************************************************** */
extern PPP_DATA_Q_CTRL_ST       g_PppDataQCtrl;


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    PPP_MSG_TYPE_ENUM_UINT32            enMsgID;    /*_H2ASN_MsgChoice_Export PPP_MSG_TYPE_ENUM_UINT32*/

    VOS_UINT8                           aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          PPP_MSG_TYPE_ENUM_UINT32
    ****************************************************************************/
}PPP_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    PPP_MSG_DATA                    stMsgData;
}PppInterface_MSG;


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 PPP_ProcAtCtrlOper(struct MsgCB * pMsg);

extern VOS_VOID Ppp_MBufFrameMntnInfo(struct ppp_mbuf *bp, VOS_UINT16 usProto, VOS_UINT32 ulDir);
extern VOS_VOID Ppp_TtfMemFrameMntnInfo(PPP_ZC_STRU *pstMem, VOS_UINT16 usProto, VOS_UINT32 ulDir);
extern VOS_VOID Ppp_FrameMntnInfo(PPP_FRAME_MNTN_INFO_STRU *ptrPppMntnSt, VOS_UINT32 ulDir, VOS_UINT16 ulDataLen);
extern VOS_VOID Ppp_EventMntnInfo(VOS_UINT16 usPppID, VOS_UINT32 ulEvent);
extern VOS_VOID Ppp_RcvConfigInfoIndMntnInfo(VOS_UINT16 usPppID, AT_PPP_IND_CONFIG_INFO_STRU *ptrIndConfigInfo);
extern VOS_VOID Ppp_RcvConfigInfoReqMntnInfo(VOS_UINT16 usPppID, PPP_REQ_CONFIG_INFO_STRU *ptrReqConfigInfo);

extern VOS_UINT32 PPP_SendPulledData(VOS_UINT16 usPppId, PPP_ZC_STRU *pstMem);
extern VOS_UINT32 PPP_SendPushedData(VOS_UINT16 usPppId, VOS_UINT8 * pucHdlcFrameBuf, VOS_UINT16 usLen);


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
#endif /*end of __PPP_INPUT_H__*/

