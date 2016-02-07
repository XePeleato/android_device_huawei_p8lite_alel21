#ifndef __PCSC_PROC_H__
#define __PCSC_PROC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "vos.h"
#include "pslog.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define PS_USIM_GET_STATUS_IND                  27
#define SI_PIH_PCSC_DATA_CNF                    0xA5

/*******************************************************************************
  3 枚举定义
*******************************************************************************/
enum USIMM_CARD_TYPE_ENUM
{
    USIMM_CARD_SIM           = 0,
    USIMM_CARD_USIM          = 1,
    USIMM_CARD_ROM_SIM       = 2,
    USIMM_CARD_ISIM          = 3,             /*ISIM卡*/
    USIMM_CARD_NOCARD        = 255,
    USIMM_CARD_TYPE_BUTT
};
typedef VOS_UINT32      USIMM_CARD_TYPE_ENUM_UINT32;

enum USIMM_CARD_SERVIC_ENUM
{
    USIMM_CARD_SERVIC_ABSENT        =0,     /* 无卡 */
    USIMM_CARD_SERVIC_UNAVAILABLE   =1,  /* 有卡,服务不可用 */
    USIMM_CARD_SERVIC_SIM_PIN       =2,    /* SIM卡服务由于PIN码原因不可用 */
    USIMM_CARD_SERVIC_SIM_PUK       =3,    /* SIM卡服务由于PUK码原因不可用 */
    USIMM_CARD_SERVIC_NET_LCOK      =4,   /* SIM卡服务由于网络锁定原因不可用 */
    USIMM_CARD_SERVIC_IMSI_LCOK     =5,   /* SIM卡服务由于IMSI锁定原因不可用 */
    USIMM_CARD_SERVIC_AVAILABLE     =6,   /* 服务可用 */
    USIMM_CARD_SERVIC_BUTT
};
typedef VOS_UINT32      USIMM_CARD_SERVIC_ENUM_UINT32;

enum SI_PIH_EVENT_ENUM
{
    SI_PIH_EVENT_PIN_OPER_CNF       = 1,      /* 操作PIN码返回结果        */
    SI_PIH_EVENT_PIN_QUERY_CNF      = 2,      /* 查询PIN码返回结果        */
    SI_PIH_EVENT_PIN_INFO_IND       = 3,      /* 开机PIN码主动上报        */
    SI_PIH_EVENT_SIM_INFO_IND       = 4,      /* SIM卡信息上报            */
    SI_PIH_EVENT_GENERIC_ACCESS_CNF = 5,      /* +CSIM 回复               */
    SI_PIH_EVENT_RESTRIC_ACCESS_CNF = 6,      /* +CRSM 回复               */
    SI_PIH_EVENT_FDN_CNF            = 7,      /* FDN回复                  */
    SI_PIH_EVENT_BDN_CNF            = 8,      /* BDN回复                  */
    SI_PIH_EVENT_PERO_LOCK_CNF      = 9,      /* 锁卡回复                 */
    SI_PIH_EVENT_BUTT
};
typedef VOS_UINT32  SI_PIH_EVENT;

enum SI_PIH_REQ_ENUM
{
    SI_PIH_NULL_REQ                 = 0,
    SI_PIH_FDN_ENABLE_REQ           = 1,
    SI_PIH_FDN_DISALBE_REQ          = 2,
    SI_PIH_GACCESS_REQ              = 3,
    SI_PIH_BDN_QUERY_REQ            = 4,
    SI_PIH_FDN_QUERY_REQ            = 5,
    SI_PIH_PCSC_DATA_REQ            = 6,
    SI_PIH_REQ_BUTT
};
typedef VOS_UINT32      SI_PIH_REQ_ENUM_UINT32;


/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;       /* 消息名 */
    VOS_UINT16                          usClient;        /* 客户端ID */
    VOS_UINT8                           ucOpID;
    VOS_UINT8                           ucRsv;
    SI_PIH_EVENT                        ulEventType;
}SI_PIH_MSG_HEADER_STRU;

typedef struct
{
    SI_PIH_MSG_HEADER_STRU          stMsgHeader;
    VOS_UINT32                      ulMsgType;
    VOS_UINT32                      ulCmdType;
    VOS_UINT32                      ulCmdLen;
    VOS_UINT8                       aucAPDU[8];
}SI_PIH_PCSC_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                      ulMsgName;
    USIMM_CARD_TYPE_ENUM_UINT32     enCardType;     /* 卡类型:SIM、USIM、ROM-SIM  */
    USIMM_CARD_SERVIC_ENUM_UINT32   enCardStatus;   /* 卡状态*/
    VOS_UINT8                       aucIMSI[9];     /* IMSI号*/
    VOS_UINT8                       ucIMSILen;      /* IMSI长度 */
    VOS_UINT8                       ucReason;       /* 上报原因*/
    VOS_UINT8                       ucRsv;
}PS_USIM_STATUS_IND_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                      ulMsgName;          /* 消息类型 */
    VOS_UINT32                      ulResult;           /* PC/SC命令执行结果 */
    VOS_UINT32                      ulCmdType;          /* 命令类型 */
    VOS_UINT32                      ulRspLen;           /* 命令执行得到的数据长度 */
    VOS_UINT8                       aucContent[4];      /* 数据内容 */
}SI_PIH_PCSC_CNF_STRU;




#ifdef __cplusplus
#if __cplusplus
        }
#endif
#endif

#endif
