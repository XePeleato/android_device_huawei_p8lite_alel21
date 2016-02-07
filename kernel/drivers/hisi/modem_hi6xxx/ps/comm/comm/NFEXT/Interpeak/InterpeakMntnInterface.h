/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : InterpeakMntnInterface.h
  版 本 号   : 初稿
  作    者   : caikai
  生成日期   : 2013年7月30日
  最近修改   :
  功能描述   : InterpeakMntn.c 的接口文件，供外部模块调用
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : 创建文件

******************************************************************************/

#ifndef __INTERPEAKMNTNINTERFACE_H__
#define __INTERPEAKMNTNINTERFACE_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#if (VOS_OS_VER == VOS_WIN32)
#include  "VxWorksStub.h"
#else
#if ((VOS_OS_VER == VOS_RTOSCK) || ((VOS_OS_VER == VOS_WIN32) && (VOS_PC_TEST == VOS_RTOSCK)))

#else
#include <ipcom_type.h>
#endif
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define IPS_MNTN_CCORE_INFO_LEN               (10000)           /*TCP/IP协议栈基础信息最长长度*/

#define IPS_MNTN_CCORE_INFO_TYPE_LEN          (2)
#define IPS_MNTN_CCORE_INFO_TYPE_OFFSET       (4)               /*TCP/IP协议栈基础信息类型偏移*/
#define IPS_MNTN_CCORE_INFO_LEN_OFFSET        (4)               /*TCP/IP协议栈基础信息长度偏移*/

#define IPS_MNTN_CCORE_OK                     (0)
#define IPS_MNTN_CCORE_ERROR                  (-1)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*TCP/IP协议栈基础信息类型*/
enum IPS_MNTN_CCORE_INFO_TYPE_ENUM
{
    ID_IPS_MNTN_CCORE_IFCONFIG_INFO           = 0x1001,               /*虚拟网卡信息*/
    ID_IPS_MNTN_CCORE_NETSTAT_ROUTES_INFO     = 0x1002,               /*路由信息*/
    ID_IPS_MNTN_CCORE_NETSTAT_ENTRIES_INFO    = 0x1003,               /*连接状态信息*/
    ID_IPS_MNTN_CCORE_NAT_RULES_INFO          = 0x1004,               /*NAT表规则信息*/
    ID_IPS_MNTN_CCORE_NAT_STATS_INFO          = 0x1005,               /*NAT表状态信息*/
    ID_IPS_MNTN_CCORE_NAT_PROXIES_INFO        = 0x1006,               /*NAT表协议信息*/
    ID_IPS_MNTN_CCORE_NAT_MAPPINGS_INFO       = 0x1007,               /*NAT表映射信息*/

    ID_IPS_MNTN_CCORE_NAT_TYPE_BUTT           = 0xffff
};
typedef unsigned short IPS_MNTN_CCORE_INFO_TYPE_ENUM_UINT16;

/* Interpeak可维可测回调函数下标指示 */
enum IPS_MNTN_CCORE_CB_TYPE_ENUM
{
    IPS_MNTN_CCORE_INPUT_DATA_CB        = 0,    /*TCP/IP可维可测接收报文*/
    IPS_MNTN_CCORE_OUTPUT_DATA_CB,              /*TCP/IP可维可测发送报文*/
    IPS_MNTN_CCORE_BRIDGE_DATA_CB,              /*TCP/IP网桥中转报文*/
    IPS_MNTN_CCORE_RECV_ARP_CB,                 /*TCP/IP协议栈接收的ARP控制报文*/
    IPS_MNTN_CCORE_SEND_ARP_CB,                 /*TCP/IP协议栈发送的ARP控制报文*/
    IPS_MNTN_CCORE_RECV_DHCPC_CB,               /*TCP/IP协议栈接收的DHCP控制报文*/
    IPS_MNTN_CCORE_SEND_DHCPC_CB,               /*TCP/IP协议栈发送的DHCP控制报文*/
    IPS_MNTN_CCORE_RECV_DHCPS_CB,               /*TCP/IP协议栈接收的DHCP控制报文*/
    IPS_MNTN_CCORE_SEND_DHCPS_CB,               /*TCP/IP协议栈发送的DHCP控制报文*/
    IPS_MNTN_CCORE_APP_CMD_CB,                  /*APP控制命令信息*/

    IPS_MNTN_CCORE_CB_TYPE_BUTT
};
typedef unsigned short  IPS_MNTN_CCORE_CB_TYPE_ENUM_UINT16;

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
/*****************************************************************************
 结构名    : IPS_MNTN_CCORE_CAPTURE_CB_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : Interpeak可维可测回调函数参数结构
*****************************************************************************/
typedef struct
{
    unsigned char      *paucNetIfName;  /* 虚拟网卡信息*/
    unsigned char      *paucSrcPort;    /* 发送虚拟网卡信息，网桥使用*/
    unsigned char      *paucDestPort;   /* 接受虚拟网卡信息，网桥使用*/
    unsigned char      *pucData;        /* 线性数据指针,内容为报文信息或者APP命令 */
    unsigned short      usLen;          /* 报文数据长度 */
    unsigned char       aucRsv[2];      /* 报文数据长度 */
}IPS_MNTN_CCORE_CAPTURE_PARA_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/* 报文或者命令捕获回调函数 */
typedef void (*IPS_MNTN_CCORE_CAPTURE_CB)(IPS_MNTN_CCORE_CAPTURE_PARA_STRU *pstIpsMntnCapturePara);


/*****************************************************************************
  10 函数声明
*****************************************************************************/
#if ((VOS_OS_VER == VOS_RTOSCK) || ((VOS_OS_VER == VOS_WIN32) && (VOS_PC_TEST == VOS_RTOSCK)))

#else

/*lint -esym(752,IPS_MNTN_CCORE_RegExtFunc)*/
extern int IPS_MNTN_CCORE_RegExtFunc(IPS_MNTN_CCORE_CAPTURE_CB pCaptureCb, IPS_MNTN_CCORE_CB_TYPE_ENUM_UINT16 enCbType);

/*lint -esym(752,IPS_MNTN_CCORE_InterpeakMntnInit)*/
extern int IPS_MNTN_CCORE_InterpeakMntnInit(void);

/*lint -esym(752,IPS_MNTN_CCORE_GetNetstatEntries)*/
extern int IPS_MNTN_CCORE_GetNetstatEntries
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
);

/*lint -esym(752,IPS_MNTN_CCORE_GetNatRules)*/
extern int IPS_MNTN_CCORE_GetNatRules
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
);

/*lint -esym(752,IPS_MNTN_CCORE_GetNatStats)*/
extern int IPS_MNTN_CCORE_GetNatStats
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
);

/*lint -esym(752,IPS_MNTN_CCORE_GetNatProxies)*/
extern int IPS_MNTN_CCORE_GetNatProxies
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
);

/*lint -esym(752,IPS_MNTN_CCORE_GetIpsInfo)*/
extern int IPS_MNTN_CCORE_GetIpsInfo
(
    unsigned char                      *pucData,
    unsigned int                       *pulLen,
    unsigned short                      usType
);

/*lint -esym(752,IPS_MNTN_CCORE_GetNatMappings)*/
extern int IPS_MNTN_CCORE_GetNatMappings
(
    unsigned char                  *pucData,
    int                             ulNum
);
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of InterpeakMntnInterface.h */




