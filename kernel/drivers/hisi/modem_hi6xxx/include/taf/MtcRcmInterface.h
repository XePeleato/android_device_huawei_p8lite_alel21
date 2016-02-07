/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MtcRcmInterface.h
  版 本 号   : 初稿
  作    者   : 蒋德彬/00174725
  生成日期   : 2014年06月16日
  最近修改   :
  功能描述   : MTC和RCM之间的接口
  函数列表   :
  修改历史   :
  1.日    期   : 2014年06月16日
    作    者   : 蒋德彬/00174725
    修改内容   : 创建文件

******************************************************************************/

#ifndef __MTCRCMINTERFACE_H__
#define __MTCRCMINTERFACE_H__

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MTC_RCM_MAX_MODEM_NUM              (MTC_RCM_MODEM_BUTT)                 /* 支持的最大modem个数 */


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
枚举名    : MTC_RCM_MSG_ID_ENUM
结构说明  : MTC和RCM的之间的消息

  1.日    期   : 2014年06月16日
    作    者   : 蒋德彬/00174725
    修改内容  :  定义MTC和RCM之间的消息
*****************************************************************************/
enum MTC_RCM_MSG_ID_ENUM
{
    /* 消息方向: MTC->RCM */
    ID_MTC_RCM_MODEM_STATUS_NOTIFY      = 0x1818,                              /* MTC通知RCM modem连接状态 *//* _H2ASN_MsgChoice MTC_RCM_MODEM_STATE_NOTIFY_STRU */

    /* 消息方向: RCM->MTC */

    ID_MTC_RCM_MSG_ID_ENUM_BUTT

};
typedef VOS_UINT32  MTC_RCM_MSG_ID_ENUM_UINT32;

/*****************************************************************************
枚举名    : MTC_RCM_MODEM_TYPE_ENUM
结构说明  : Modem ID 定义

  1.日    期   : 2014年06月16日
    作    者   : 蒋德彬/00174725
    修改内容  :  定义modem ID
*****************************************************************************/
enum MTC_RCM_MODEM_TYPE_ENUM
{
    MTC_RCM_MODEM_0                     = MODEM_ID_0,
    MTC_RCM_MODEM_1,
    MTC_RCM_MODEM_CDMA,

    MTC_RCM_MODEM_BUTT
};
typedef VOS_UINT16 MTC_RCM_MODEM_TYPE_ENUM_UINT16;


enum MTC_RCM_CONN_STATE_ENUM
{
    MTC_RCM_CONN_NO_EXIST                   = 0x00,                            /* 不存在连接状态 */
    MTC_RCM_CONN_EXIST                      = 0x01,                            /* 存在连接状态 */

    MTC_RCM_CONN_STATE_BUTT
};
typedef VOS_UINT16 MTC_RCM_CONN_STATE_ENUM_UINT16;

/*****************************************************************************
枚举名    : MTC_RCM_MODEM_POWER_STATE_ENUM
结构说明  : 开机状态枚举

  1.日    期   : 2014年06月16日
    作    者   : 蒋德彬/j00174725
    修改内容   : K3V3 多模多天线特性
*****************************************************************************/
enum MTC_RCM_MODEM_POWER_STATE_ENUM
{
    MTC_RCM_MODEM_POWER_OFF             = 0x00,                                /* 关机 */
    MTC_RCM_MODEM_POWER_ON              = 0x01,                                /* 开机 */

    MTC_RCM_MODEM_POWER_STATE_BUTT
};
typedef VOS_UINT16  MTC_RCM_MODEM_POWER_STATE_ENUM_UINT16;

/*****************************************************************************
枚举名    : MTC_RCM_USIMM_CARD_SERVIC_ENUM
结构说明  : 卡状态

  1.日    期   : 2014年06月16日
    作    者   : 蒋德彬/j00174725
    修改内容   : K3V3 多模多天线特性
*****************************************************************************/

enum MTC_RCM_USIMM_CARD_SERVIC_ENUM
{
    MTC_RCM_USIMM_CARD_SERVIC_ABSENT        =0,                                 /* 无卡 */
    MTC_RCM_USIMM_CARD_SERVIC_UNAVAILABLE   =1,                                 /* 有卡,服务不可用 */
    MTC_RCM_USIMM_CARD_SERVIC_SIM_PIN       =2,                                 /* SIM卡服务由于PIN码原因不可用 */
    MTC_RCM_USIMM_CARD_SERVIC_SIM_PUK       =3,                                 /* SIM卡服务由于PUK码原因不可用 */
    MTC_RCM_USIMM_CARD_SERVIC_NET_LCOK      =4,                                 /* SIM卡服务由于网络锁定原因不可用 */
    MTC_RCM_USIMM_CARD_SERVIC_IMSI_LCOK     =5,                                 /* SIM卡服务由于IMSI锁定原因不可用 */
    MTC_RCM_USIMM_CARD_SERVIC_AVAILABLE     =6,                                 /* 服务可用 */

    MTC_RCM_USIMM_CARD_SERVIC_BUTT
};
typedef VOS_UINT16      MTC_RCM_USIMM_CARD_SERVIC_ENUM_UINT16;

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
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
}MTC_RCM_MSG_HEADER_STRU;


typedef struct
{
    MTC_RCM_MODEM_TYPE_ENUM_UINT16          enModemType;                        /* MODEM ID */
    MTC_RCM_CONN_STATE_ENUM_UINT16          enCsSrvConnState;                   /* CS连接状态 */
    MTC_RCM_CONN_STATE_ENUM_UINT16          enPsSrvConnState;                   /* PS连接状态 */
    MTC_RCM_MODEM_POWER_STATE_ENUM_UINT16   enModemPowerState;                  /* 开关机状态 */
    MTC_RCM_USIMM_CARD_SERVIC_ENUM_UINT16   enUsimmCardState;                   /* 卡状态 */

    VOS_UINT16                              usRsv;                              /* 保留位 */
} MTC_RCM_MODEM_STATE_INFO_STRU;


typedef struct
{
    MTC_RCM_MSG_HEADER_STRU             stMsgHeader;                            /*_H2ASN_Skip*/
    MTC_RCM_MODEM_STATE_INFO_STRU       astModemState[MTC_RCM_MAX_MODEM_NUM];
} MTC_RCM_MODEM_STATE_NOTIFY_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/


/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    MTC_RCM_MSG_ID_ENUM_UINT32          enMsgId;                                /*_H2ASN_MsgChoice_Export MTC_RCM_MSG_ID_ENUM_UINT32*/
    VOS_UINT8                           aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          MTC_RCM_MSG_ID_ENUM_UINT32
    ****************************************************************************/
}MTC_RCM_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    MTC_RCM_MSG_DATA                    stMsgData;
}MtcRcmInterface_MSG;

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

#endif


