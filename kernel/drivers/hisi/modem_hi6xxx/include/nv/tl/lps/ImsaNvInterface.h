

#ifndef __IMSANVINTERFACE_H__
#define __IMSANVINTERFACE_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
#pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/
#define IMSA_PASSWORD_LEN               (129)
#define IMSA_IMPI_LEN                   (129)
#define IMSA_IMPU_LEN                   (129)
#define IMSA_HOME_NET_DOMAIN_NAME_LEN   (129)

#define IMSA_IMS_MAX_REDIAL_CAUSE_NUM   (16)
#if 0
#define IMS_PARM_STRING_SIZE            (129)
#endif

#define IMSA_NV_IPV4_ADDR_LEN           (4)
#define IMSA_NV_IPV6_ADDR_LEN           (16)


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



/*****************************************************************************
  4 Enum
*****************************************************************************/
enum IMSA_PCSCF_DISCOVERY_POLICY_ENUM
{
    IMSA_PCSCF_DISCOVERY_POLICY_PCO           = 0,      /**< PCO方式获取P-CSCF */
    IMSA_PCSCF_DISCOVERY_POLICY_NV            = 1,      /**< NV配置P-CSCF */
    IMSA_PCSCF_DISCOVERY_POLICY_BUTT
};
typedef VOS_UINT8 IMSA_PCSCF_DISCOVERY_POLICY_ENUM_UINT8;


/*****************************************************************************
  5 STRUCT
*****************************************************************************/

typedef struct
{
    VOS_UINT8                           ucGsmImsSupportFlag;    /**< GSM IMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucUtranImsSupportFlag; /**< UNTRAN IMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucLteImsSupportFlag;    /**< LTE IMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucGsmEmsSupportFlag;    /**< GSM EMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucUtranEmsSupportFlag; /**< UNTRAN EMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucLteEmsSupportFlag;    /**< LTE EMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;

}IMSA_NV_IMS_RAT_SUPPORT_STRU;

typedef struct
{
    VOS_UINT16                          usImsMinPort;           /* IMS端口号范围下限 */
    VOS_UINT16                          usImsMaxPort;           /* IMS端口号范围上限 */
#if 0
    VOS_UINT32                          ulSip;
    VOS_UINT32                          ulAudio;                /**< audio rtp port */
    VOS_UINT32                          ulAudioPoolSize;        /**< audio rtp pool size */
    VOS_UINT32                          ulVideo;                /**< video rtp port */
    VOS_UINT32                          ulVideoPoolSize;        /**< video rtp pool size */
    VOS_UINT32                          ulProtectedSip;         /**< protected sip port */
    VOS_UINT32                          ulProtectedSipPoolSize; /**< protected sip pool size */
#endif
}IMSA_NV_SIP_PORT_CONFIG_STRU;

typedef struct
{
    VOS_UINT8                           ucIsCallRedialSupportFlg;   /* 是否支持IMS域内重播功能，VOS_TRUE:支持；VOS_FALSE:不支持*/
    VOS_UINT8                           ucRedialCauseNum;           /* 重播原因值个数 */
    VOS_UINT8                           aucReserved1[2];
    VOS_UINT32                          ulCallRedialMaxTime;        /* IMS域内重播功能打开时，重播最大时长，单位毫秒 */
    VOS_UINT32                          ulCallRedialInterval;       /* IMS域内重播功能打开时，重拨间隔，单位毫秒 */
    VOS_UINT32                          ulCallRedialMaxNum;         /* IMS域内重播功能打开时，重播最大次数  */
    VOS_UINT16                          ausRedialCauseList[IMSA_IMS_MAX_REDIAL_CAUSE_NUM];       /* 重播原因值列表 */
}IMSA_NV_IMS_REDIAL_CFG_STRU;

typedef struct
{
    VOS_UINT8                           ucCMCCCustomDeregFlag;    /**< 中国移动定制需求, 条件满足时去注册使能项*/
    VOS_UINT8                           ucCMCCCustomTcallFlag;    /**< 中国移动定制需求, Tcall SilentRedial使能项*/
    VOS_UINT8                           ucCMCCCustomTqosFlag;     /**< 中国移动定制需求, Tqos SilentRedial使能项*/
    VOS_UINT8                           aucReserved[1];

    VOS_UINT32                          ulTcallTimerLen;            /**< 中国移动定制需求, Tcall timer定时器时长, 单位为ms*/
    VOS_UINT32                          ulTqosTimerLen;             /**< 中国移动定制需求, Tqos timer定时器时长, 单位为ms*/
} IMSA_NV_CMCC_CUSTOM_REQ_STRU;

typedef struct
{
    VOS_UINT8                           ucNoCardEmcCallSupportFlag;/**< 无卡紧急呼使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucUeRelMediaPdpFLag;    /**< UE主动释放媒体承载标识 */
    VOS_UINT8                           ucImsLocDeregSigPdpRelFlag;/**< IMS普通信令被动释放后本地IMS去注册标识 */
    VOS_UINT8                           ucRetryTimes;              /**< IMS注册失败再尝试注册次数，0为默认值，为无限次 */
    VOS_UINT32                          ulMaxTime;              /**< max time，单位为s */
    VOS_UINT32                          ulBaseTime;             /**< base time，单位为s */
    VOS_UINT32                          ulRegFailRetryIntervel; /**< 注册失败重试间隔，单位为s，0表示非固定间隔 */
    VOS_UINT32                          ulRegTimerLen;
    IMSA_NV_IMS_REDIAL_CFG_STRU         stImsRedialCfg;         /**< IMS域内重播配置 */
    IMSA_NV_CMCC_CUSTOM_REQ_STRU        stCMCCCustomReq;        /**< 中国移动定制需求 */
}IMSA_NV_IMSA_CONFIG_STRU;

typedef struct
{
    VOS_UINT8                           ucVoiceCallOnImsSupportFlag;        /**< IMS语音使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucVideoCallOnImsSupportFlag;        /**< IMS视频使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucSmsOnImsSupportFlag;              /**< IMS短信使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucSrvccOnImsSupportFlag;            /**< IMS Srvcc使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucSrvccMidCallOnImsSupportFlag;     /**< IMS SrvccMidCall使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucSrvccAlertingOnImsSupportFlag;    /**< IMS SrvccAlerting使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucSrvccPreAlertingOnImsSupportFlag; /**< IMS SrvccPreAlerting使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucSrvccTiFlag;                      /**< IMS SrvccPreAlerting,VOS_TRUE :0 R10版本,VOS_FALSE R11之后版本 */
}IMS_NV_IMS_CAP_STRU;


typedef struct
{
    VOS_UINT8                           ucUserInfoFlag;   /**< 0:从USIM读IMPI，IMPU,DOMAIN   1:从NV读取IMPI，IMPU,DOMAIN */
    VOS_UINT8                           ucAuthType;                         /**< IMS 鉴权方式  0:AKA  1:DIGIST 2:AKA_IPSEC */
    VOS_UINT8                           aucReserved[2];
    VOS_INT8                            acImpi[IMSA_IMPI_LEN]; /**< 默认49字节 */
    VOS_INT8                            acTImpu[IMSA_IMPU_LEN]; /**< 默认53字节 */
    VOS_INT8                            acHomeNetDomainName[IMSA_HOME_NET_DOMAIN_NAME_LEN]; /**< 默认33字节 */
    VOS_INT8                            aucPassWord[IMSA_PASSWORD_LEN];     /**< 鉴权方式为DIGIST时:设置密码 */
}IMSA_NV_IMS_CONFIG_STRU;

typedef struct
{
    VOS_UINT32                          bitOpPrimPcscfAddr  : 1;
    VOS_UINT32                          bitOpSecPcscfAddr   : 1; /**< 如果bitOpSecPcscfAddr为1，bitOpPrimPcscfAddr也必须为1*/
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           aucPrimPcscfAddr[IMSA_NV_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecPcscfAddr[IMSA_NV_IPV4_ADDR_LEN];
} IMSA_NV_PDP_IPV4_PCSCF_STRU;

typedef struct
{
    VOS_UINT32                          bitOpPrimPcscfAddr  : 1;
    VOS_UINT32                          bitOpSecPcscfAddr   : 1; /**< 如果bitOpSecPcscfAddr为1，bitOpPrimPcscfAddr也必须为1*/
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           aucPrimPcscfAddr[IMSA_NV_IPV6_ADDR_LEN];
    VOS_UINT8                           aucSecPcscfAddr[IMSA_NV_IPV6_ADDR_LEN];
} IMSA_NV_PDP_IPV6_PCSCF_STRU;


typedef struct
{
    IMSA_PCSCF_DISCOVERY_POLICY_ENUM_UINT8  enPcscfDiscoverPolicy;  /**< P-CSCF获取策略 */
    VOS_UINT8                               aucReserved[3];
    IMSA_NV_PDP_IPV4_PCSCF_STRU             stIpv4Pcscf;            /**< 承载IPv4 P-CSCF信息 */
    IMSA_NV_PDP_IPV6_PCSCF_STRU             stIpv6Pcscf;            /**< 承载IPv6 P-CSCF信息 */
}IMSA_NV_PCSCF_DISCOVERY_POLICY_STRU;

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

#endif /* end of ImsaNvInterface.h */




