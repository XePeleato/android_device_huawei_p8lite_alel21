#ifndef _IPS_MNTN_H_
#define _IPS_MNTN_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "om.h"


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define IPS_IFNAMSIZ                    (16)                    /*虚拟网卡名称长度*/
#define IPS_END_NAME                    (16)
#define IPS_APP_CMD_INFO_LEN            (200)                   /*捕获APP控制命令的最长长度*/
#define IPS_MNTN_INFO_LEN               (10000)                 /*捕获TCP/IP协议栈基本信息最长长度*/
#define IPS_MNTN_TRACE_MAX_BYTE_LEN     (1514)                  /*捕获TCP/IP协议栈报文最长长度 */
#define IPS_MNTN_NAT_MAPPING_INFO_LEN   (40)                    /*捕获TCP/IP协议栈NAT映射信息基本长度*/
#define IPS_MNTN_INFO_TYPE_OFFSET       (4)                     /*TCP/IP协议栈基础信息类型偏移*/
#define IPS_MNTN_INFO_LEN_OFFSET        (4)                     /*TCP/IP协议栈基础信息长度偏移*/

/* IP 数据包可维可测*/
#define IP_IPV4_PROTO_VERSION        4      /* IPV4协议版本号 */
#define IP_IPV4_HEAD_LEN             20     /* IPV4数据头头部长度 */
#define IP_IPV4_PROTO_TCP            6      /* IPV4的TCP协议号 */
#define IP_IPV4_PROTO_UDP            17     /* IPV4的UDP协议号 */
#define IP_IPV4_PROTO_ICMP           1      /* IPV4的ICMP协议号 */
#define IP_IPV4_ICMP_ECHO_REQUEST    8      /* IPV4的ICMP的TYPE ECHO REQ */
#define IP_IPV4_ICMP_ECHO_REPLY      0      /* IPV4的ICMP的TYPE ECHO REPLY */
#define IP_IPV4_DATA_LEN_POS         2      /* IPV4的IP数据包长度字段偏移 */
#define IP_IPV4_IDENTIFY_POS         4      /* IPV4的IP数据包IDENTIFY字段偏移 */
#define IP_IPV4_PROTO_POS            9      /* IPV4的IP数据包协议字段偏移 */
#define IP_IPV4_ICMP_IDENTIFY_POS    4      /* 去除IP头，ICMP的IDENTIFY字段偏移 */
#define IP_IPV4_ICMP_SN_POS          6      /* 去除IP头，ICMP的SN字段偏移 */
#define IP_IPV4_TCP_SEQ_POS          4      /* 去除IP头，TCP的SEQ字段偏移 */
#define IP_IPV4_TCP_ACK_POS          8      /* 去除IP头，TCP的ACK字段偏移 */
#define IP_IPV4_DST_PORT_POS         2      /* 去除IP头，IPV4的目的端口字段偏移 */
#define IP_PROTO_VERSION_POS         4      /* IP数据包的协议版本字段偏移 */

#define IP_IPV6_PROTO_VERSION        6      /* IPV6协议版本号 */
#define IP_IPV6_HEAD_LEN             40     /* IPV6数据头头部长度 */
#define IP_IPV6_PROTO_TCP            6      /* IPV6的TCP协议号 */
#define IP_IPV6_PROTO_UDP            17     /* IPV6的UDP协议号 */
#define IP_IPV6_PROTO_ICMP           58     /* IPV6的ICMP协议号 */
#define IP_IPV6_ICMP_ECHO_REQUEST    128    /* IPV6的ICMP的TYPE ECHO REQ */
#define IP_IPV6_ICMP_ECHO_REPLY      129    /* IPV6的ICMP的TYPE ECHO REPLY */
#define IP_IPV6_DATA_LEN_POS         4      /* IPV6的IP数据包长度字段偏移 */
#define IP_IPV6_PROTO_POS            6      /* IPV6的IP数据包协议字段偏移 */
#define IP_IPV6_ICMP_IDENTIFY_POS    4      /* 去除IP头，ICMP的IDENTIFY字段偏移 */
#define IP_IPV6_TCP_SEQ_POS          4      /* 去除IP头，TCP的SEQ字段偏移 */
#define IP_IPV6_TCP_ACK_POS          8      /* 去除IP头，TCP的ACK字段偏移 */
#define IP_IPV6_DST_PORT_POS         2      /* 去除IP头，IPV6的目的端口字段偏移 */

#define IP_IPV4_VER_AND_HEAD         0x45   /* IPv4版本号字段的值 */
#define IP_IPV6_VER_AND_HEAD         0x60   /* IPv6版本号字段的值 */
#define MAC_HEAD_LEN                 14     /* MAC头长度 */

/* 从IP数据包中提取数据，并把网络字节序转换为主机字节序 */
#define IP_GET_VAL_NTOH_U8(data,pos)    ((data)[pos])
#define IP_GET_VAL_NTOH_U16(data,pos)   (((data)[pos]<<8)|((data)[pos+1]))
#define IP_GET_VAL_NTOH_U32(data,pos)   (((data)[pos]<<24)|((data)[pos+1]<<16)|((data)[pos+2]<<8)|((data)[pos+3]))

/*填充消息头*/
#define IPS_FILL_MSG_HEADER(pSendMsg, RecvPid)    \
        (pSendMsg)->ulReceiverCpuId  = VOS_LOCAL_CPUID; \
        (pSendMsg)->ulReceiverPid    = RecvPid;

#define MNTN_IP_INFO_LEN                ((VOS_UINT32)sizeof(IPS_MNTN_IP_INFO_STRU))

#define MNTN_FLOW_CTRL_INFO_LEN         ((VOS_UINT32)sizeof(IPS_MNTN_FLOW_CTRL_STRU))

/*******************************************************************************
  3 枚举定义
*******************************************************************************/
enum IPS_MNTN_TRACE_CONFIG_TYPE_ENUM
{
    ID_OM_IPS_ADVANCED_TRACE_CONFIG_REQ = 0xD013,               /* 捕获TCP/IP协议栈 报文配置*/
    ID_IPS_OM_ADVANCED_TRACE_CONFIG_CNF = 0xD014,               /* 捕获TCP/IP协议栈 报文指示*/

    ID_OM_IPS_MNTN_INFO_CONFIG_REQ      = 0xD015,               /* 周期性捕获TCP/IP协议栈 基础信息配置*/
    ID_IPS_OM_MNTN_INFO_CONFIG_CNF      = 0xD016,               /* 周期性捕获TCP/IP协议栈 基础信息配置指示*/

    ID_OM_IPS_MNTN_IP_INFO_CONFIG_REQ   = 0xD017,               /* 捕获IP 数据包信息配置*/
    ID_IPS_OM_MNTN_IP_INFO_CONFIG_CNF   = 0xD018,               /* 捕获IP 数据包信息配置指示*/

    ID_IPS_MNTN_TRACE_CONFIG_TYPE_BUTT
};
typedef VOS_UINT16 IPS_MNTN_TRACE_CONFIG_TYPE_ENUM_UINT16;

enum IPS_MNTN_INFO_ACTION_ENUM
{
    IPS_MNTN_INFO_REPORT_STOP           = 0,                    /*开启周期性捕获TCP/IP协议栈基础信息*/
    IPS_MNTN_INFO_REPORT_START          = 1,                    /*关闭周期性捕获TCP/IP协议栈基础信息*/

    IPS_MNTN_INFO_REPORT_BUTT
};
typedef VOS_UINT16 IPS_MNTN_INFO_ACTION_ENUM_UINT16;

enum IPS_MNTN_RESULT_TYPE_ENUM
{
    IPS_MNTN_RESULT_OK                  = 0,
    IPS_MNTN_RESULT_START_TIMER_FAIL    = 1,
    IPS_MNTN_RESULT_ALLOC_MEM_FAIL      = 2,
    IPS_MNTN_RESULT_ALLOC_MSG_FAIL      = 3,
    IPS_MNTN_RESULT_INPUT_PARAM_ERR     = 4,
    IPS_MNTN_RESULT_LOCAL_PARAM_ERR     = 5,

    IPS_MNTN_RESULT_BUTT
};
typedef VOS_UINT16  IPS_MNTN_RESULT_TYPE_ENUM_UINT16;

enum IPS_MNTN_TIMER_NAME_ENUM
{
    IPS_MNTN_INFO_TIMER                 = 0x00000001,           /*周期性捕获TCP/IP协议栈基础信息定时器*/

    IPS_MNTN_TIMER_NAME_BUTT
};
typedef VOS_UINT32 IPS_MNTN_TIMER_NAME_ENUM_UINT32;

enum IPS_MNTN_TRACE_CHOSEN_ENUM
{
    IPS_MNTN_TRACE_NULL_CHOSEN               =   0,             /*不捕获报文信息*/
    IPS_MNTN_TRACE_MSG_HEADER_CHOSEN         =   1,             /*捕获报文头部*/
    IPS_MNTN_TRACE_CONFIGURABLE_LEN_CHOSEN   =   2,             /*按照配置捕获报文*/
    IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN     =   3,             /*捕获报文全部内容*/

    IPS_MNTN_TRACE_MSG_CHOSEN_BUTT
};
typedef VOS_UINT32   IPS_MNTN_TRACE_CHOSEN_ENUM_UINT32;

/*TCP/IP协议栈可维可测捕获的消息标识*/
enum IPS_MNTN_TRACE_MSG_TYPE_ENUM
{
    ID_IPS_TRACE_INPUT_DATA_INFO            = 0xD017,               /*TCP/IP可维可测接收报文*/
    ID_IPS_TRACE_OUTPUT_DATA_INFO           = 0xD018,               /*TCP/IP可维可测发送报文*/
    ID_IPS_TRACE_BRIDGE_DATA_INFO           = 0xD019,               /*TCP/IP网桥中转报文*/
    ID_IPS_TRACE_RECV_ARP_PKT               = 0xD01A,               /*TCP/IP协议栈接收的ARP控制报文*/
    ID_IPS_TRACE_SEND_ARP_PKT               = 0xD01B,               /*TCP/IP协议栈发送的ARP控制报文*/
    ID_IPS_TRACE_RECV_DHCPC_PKT             = 0xD01C,               /*TCP/IP协议栈接收的DHCP控制报文*/
    ID_IPS_TRACE_SEND_DHCPC_PKT             = 0xD01D,               /*TCP/IP协议栈发送的DHCP控制报文*/
    ID_IPS_TRACE_RECV_DHCPS_PKT             = 0xD01E,               /*TCP/IP协议栈接收的DHCP控制报文*/
    ID_IPS_TRACE_SEND_DHCPS_PKT             = 0xD01F,               /*TCP/IP协议栈发送的DHCP控制报文*/
    ID_IPS_TRACE_APP_CMD                    = 0xD020,               /*APP控制命令信息*/
    ID_IPS_TRACE_MNTN_INFO                  = 0xD021,               /*TCP/IP协议栈基础信息*/
    ID_IPS_TRACE_BRIDGE_PRE_ROUTING_INFO    = 0xD022,
    ID_IPS_TRACE_BRIDGE_POST_ROUTING_INFO   = 0xD023,
    ID_IPS_TRACE_BRIDGE_LOCAL_IN_INFO       = 0xD024,
    ID_IPS_TRACE_BRIDGE_LOCAL_OUT_INFO      = 0xD025,
    ID_IPS_TRACE_IP4_FORWARD_INFO           = 0xD026,
    ID_IPS_TRACE_IP4_LOCAL_IN_INFO          = 0xD027,
    ID_IPS_TRACE_IP4_LOCAL_OUT_INFO         = 0xD028,
    ID_IPS_TRACE_IP6_FORWARD_INFO           = 0xD029,
    ID_IPS_TRACE_IP6_LOCAL_IN_INFO          = 0xD02A,
    ID_IPS_TRACE_IP6_LOCAL_OUT_INFO         = 0xD02B,

    /* 网桥流控可维可测上报 */
    ID_IPS_TRACE_BR_FORWARD_FLOW_CTRL_START = 0xD02C,
    ID_IPS_TRACE_BR_FORWARD_FLOW_CTRL_STOP  = 0xD02D,

    ID_IPS_TRACE_MSG_TYPE_BUTT
};
typedef VOS_UINT16  IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16;

/*TCP/IP协议栈基础信息类型*/
enum IPS_MNTN_INFO_TYPE_ENUM
{
    ID_IPS_MNTN_IFCONFIG_INFO           = 0x0001,               /*虚拟网卡信息*/
    ID_IPS_MNTN_NETSTAT_ROUTES_INFO     = 0x0002,               /*路由信息*/
    ID_IPS_MNTN_NETSTAT_ENTRIES_INFO    = 0x0003,               /*连接状态信息*/
    ID_IPS_MNTN_NAT_RULES_INFO          = 0x0004,               /*NAT表规则信息*/
    ID_IPS_MNTN_NAT_STATS_INFO          = 0x0005,               /*NAT表状态信息*/
    ID_IPS_MNTN_NAT_PROXIES_INFO        = 0x0006,               /*NAT表协议信息*/
    ID_IPS_MNTN_NAT_MAPPINGS_INFO       = 0x0007,               /*NAT表映射信息*/

    ID_IPS_MNTN_NAT_TYPE_BUTT           = 0xffff
};
typedef VOS_UINT16 IPS_MNTN_INFO_TYPE_ENUM_UINT16;

/*****************************************************************************
  4 结构定义
*****************************************************************************/
#pragma pack(4)



typedef struct
{
    IPS_MNTN_TRACE_CHOSEN_ENUM_UINT32   ulChoice;
    VOS_UINT32                          ulTraceDataLen;         /* 当选择TRACE_CONFIGURABLE_LEN_CHOSEN时，此值有效 */

}IPS_MNTN_TRACE_CONFIG_STRU;

typedef IPS_MNTN_TRACE_CONFIG_STRU  IPS_MNTN_BRIDGE_TRACE_CFG_STRU;

typedef IPS_MNTN_TRACE_CONFIG_STRU  IPS_MNTN_INPUT_TRACE_CFG_STRU;

typedef IPS_MNTN_TRACE_CONFIG_STRU  IPS_MNTN_OUTPUT_TRACE_CFG_STRU;

typedef struct
{
    IPS_MNTN_BRIDGE_TRACE_CFG_STRU      stBridgeArpTraceCfg;        /*配置捕获网桥消息*/
    IPS_MNTN_INPUT_TRACE_CFG_STRU       stPreRoutingTraceCfg;       /*配置捕获TCP/IP协议栈接收消息*/
    IPS_MNTN_OUTPUT_TRACE_CFG_STRU      stPostRoutingTraceCfg;      /*配置捕获TCP/IP协议栈发送消息*/
    VOS_UINT8                           aucRsv[100];                /* 保留位，为后期可扩展 */
}IPS_MNTN_TRACE_CONFIG_REQ_STRU;


typedef struct
{
    PS_RSLT_CODE_ENUM_UINT32            enRslt;                 /* 返回查询结构成功或失败，PS_SUCC表示成功，PS_FAIL表示失败 */

}IPS_MNTN_TRACE_CONFIG_CNF_STRU;

typedef struct
{
    IPS_MNTN_INFO_ACTION_ENUM_UINT16    enCommand;
    VOS_UINT16                          usTimeLen;              /*单位：秒 */
}IPS_MNTN_INFO_CFG_REQ_STRU;

typedef struct
{
    IPS_MNTN_INFO_ACTION_ENUM_UINT16    enCommand;
    IPS_MNTN_RESULT_TYPE_ENUM_UINT16    enRslt;
}IPS_MNTN_INFO_CFG_CNF_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_INFO_ENTITY
 结构说明  : 捕获TCP/IP协议栈基本信息实体
*****************************************************************************/
typedef struct
{
    HTIMER                              hIpsMntnTimer;          /*定时器*/
    VOS_UINT32                          ulIpsMntnTimerLen;      /*定时器长度，固定1s*/
    IPS_MNTN_INFO_ACTION_ENUM_UINT16    enIpsMntnFlag;          /*捕获TCP/IP协议栈基本信息开启标志*/
    VOS_UINT8                           aucData[2];
}IPS_MNTN_INFO_ENTITY;

/*****************************************************************************
 结构名    : IPS_MNTN_PKT_INFO_STRU
 结构说明  : TCP/IP协议栈报文捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               aucNetIfName[IPS_IFNAMSIZ]; /* 虚拟网卡名称 */
    VOS_UINT16                              usLen;
    VOS_UINT8                               aucData[2];
} IPS_MNTN_PKT_INFO_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_BRIDGE_PKT_INFO_STRU
 结构说明  : 网桥中转报文捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               aucSrcPort[IPS_END_NAME];   /* 发送虚拟网卡信息 */
    VOS_UINT8                               aucDestPort[IPS_END_NAME];  /* 接收虚拟网卡名称 */
    VOS_UINT16                              usLen;
    VOS_UINT8                               aucData[2];
} IPS_MNTN_BRIDGE_PKT_INFO_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_APP_CMD_INFO_STRU
 结构说明  : APP控制命令捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usLen;
    VOS_UINT8                               aucData[2];
} IPS_MNTN_APP_CMD_INFO_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_INFO_STRU
 结构说明  : APP控制命令捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usType;
    VOS_UINT8                               aucData[2];
    VOS_UINT32                              ulLen;
} IPS_MNTN_INFO_STRU;


typedef struct
{
    VOS_UINT32  ulFcType;
    VOS_UINT8   aucRsv[4];
}IPS_MNTN_FLOW_CTRL_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_IP_INFO_CONFIG_REQ_STRU
 结构说明  : IP报文捕获配置
*****************************************************************************/
typedef struct
{
    VOS_UINT8    enAdsIPConfig;
    VOS_UINT8    enUsbIPConfig;
    VOS_UINT8    aucRsv[2];
}IPS_MNTN_IP_INFO_CONFIG_REQ_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_IP_INFO_STRU
 结构说明  : IP协议栈报文捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               ucIpVer;        /* IP 协议版本 */
    VOS_UINT8                               ucL4Proto;      /* 协议类型ICMP,TCP,UDP */
    VOS_UINT8                               ucIcmpType;     /* ICMP报文类型*/
    VOS_UINT8                               aucRsv[1];
    VOS_UINT16                              usDataLen;      /* IP 数据包长度*/
    VOS_UINT16                              usIp4Id;        /* IPv4 IDENTIFY字段 */
    VOS_UINT16                              usSrcPort;      /* IP 源端口号*/
    VOS_UINT16                              usDstPort;      /* IP 目的端口号 */
    VOS_UINT32                              ulL4Id;         /* 层4的IDENTIFY,ICMP IDENTIFY+SN,TCP SEQ */
    VOS_UINT32                              ulTcpAckSeq;    /* TCP ACK SEQ */
} IPS_MNTN_IP_INFO_STRU;

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif
/*****************************************************************************
  5 全局变量声明
*****************************************************************************/

/*****************************************************************************
  6 函数声明
*****************************************************************************/
extern VOS_VOID IPS_MNTN_SndCfgCnf2Om
(
    VOS_UINT16      usPrimId,
    VOS_UINT32      ulTransMsgContentLen,
    VOS_VOID       *pTransMsgContent
);

extern VOS_VOID IPS_MNTN_TraceAdvancedCfgReq(VOS_VOID *pMsg);

/* IP 数据包可维可测*/
extern VOS_VOID IPS_MNTN_TraceIpInfo(struct sk_buff *skb,VOS_UINT16 usType);
extern VOS_VOID IPS_MNTN_IPInfoCfgReq(VOS_VOID *pMsg);
extern VOS_VOID IPS_MNTN_FillMsg(OM_APP_TRACE_STRU *pstAppTrace, VOS_UINT32 ulMsgName, VOS_UINT32 ulMsgLen);
extern VOS_UINT32 IPS_MNTN_TransMsg
(
    VOS_UINT8                              *pucTransMsg,
    VOS_UINT32                              ulTransMsgContentLen,
    IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16     enMsgName
);
extern VOS_UINT32 IPS_MNTN_GetPktLenByTraceCfg
(
    IPS_MNTN_TRACE_CONFIG_STRU     *pstTraceCfg,
    VOS_UINT16                      usPktLen,
    VOS_UINT32                     *pulDataLen
);
extern VOS_VOID IPS_MNTN_PktInfoCB
(
    const VOS_UINT8                        *pucNetIfName,
    const VOS_UINT8                        *pucPktData,
    VOS_UINT16                              usPktLen,
    IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16     usType
);
extern VOS_VOID IPS_MNTN_BridgePktInfoCB
(
    const VOS_UINT8                        *pucSrcPort,
    const VOS_UINT8                        *pucDestPort,
    VOS_UINT8                              *pucPktData,
    VOS_UINT16                              usPktLen,
    IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16     usType
);
extern VOS_VOID IPS_MNTN_CtrlPktInfoCB
(
    const VOS_UINT8                        *pucNetIfName,
    const VOS_UINT8                        *pucPktData,
    VOS_UINT16                              usPktLen,
    IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16     usType
);
extern PS_BOOL_ENUM_UINT8  IPS_MNTN_BridgeTraceCfgChkParam(IPS_MNTN_TRACE_CONFIG_REQ_STRU *pRcvMsg);
extern PS_BOOL_ENUM_UINT8  IPS_MNTN_TraceCfgChkParam(IPS_MNTN_TRACE_CONFIG_REQ_STRU *pRcvMsg);
extern PS_BOOL_ENUM_UINT8  IPS_MNTN_TraceAdvancedCfgChkParam(IPS_MNTN_TRACE_CONFIG_REQ_STRU *pRcvMsg);
extern VOS_VOID IPS_MNTN_SendTranMsgDirect(OM_APP_TRACE_STRU *pstAppTrace, VOS_UINT32 ulMsgContenLen, VOS_UINT32 ulMsgName);
extern VOS_VOID IPS_MNTN_FlowCtrl(VOS_UINT32 ulFcType, IPS_MNTN_TRACE_MSG_TYPE_ENUM_UINT16  usType);
extern PS_BOOL_ENUM_UINT8  IPS_MNTN_IPInfoCfgChkParam(IPS_MNTN_IP_INFO_CONFIG_REQ_STRU *pRcvIpInfoCfg);
extern VOS_VOID IPS_MNTN_Ipv4DataParse(IPS_MNTN_IP_INFO_STRU *pstIpInfo,VOS_UINT8 *pData);
extern VOS_VOID IPS_MNTN_Ipv6DataParse(IPS_MNTN_IP_INFO_STRU *pstIpInfo,VOS_UINT8 *pData);
extern VOS_UINT32 IPS_MNTN_GetIPInfoCfg(VOS_UINT16 usType);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /*_IPS_MNTN_H_*/


