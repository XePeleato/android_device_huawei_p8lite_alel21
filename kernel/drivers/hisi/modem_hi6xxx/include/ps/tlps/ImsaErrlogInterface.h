

#ifndef __IMSAERRLOGINTERFACE_H__
#define __IMSAERRLOGINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include    "omerrorlog.h"

#pragma  pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
#define IMSA_ERR_LOG_IPV6_STRING_LEN                      (46)

#define IMSA_RING_BUFFER_SIZE                           (1024)              /* IMSA层的环形buffer的大小 */

#define IMSA_ERR_LOG_CTRL_LEVEL_NULL                     (0)                     /* ErrLog等级未定义*/
#define IMSA_ERR_LOG_CTRL_LEVEL_CRITICAL                 (1)                     /* ErrLog等级紧急 */
#define IMSA_ERR_LOG_CTRL_LEVEL_MAJOR                    (2)                     /* ErrLog等级重要 */
#define IMSA_ERR_LOG_CTRL_LEVEL_MINOR                    (3)                     /* ErrLog等级次要 */
#define IMSA_ERR_LOG_CTRL_LEVEL_WARNING                  (4)                     /* ErrLog等级提示 */



/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
enum IMSA_USIM_STATUS_ENUM
{
    IMSA_USIM_STATUS_UNAVAILABLE        = 0x01,            /**< 卡不可用，包括读取必须卡参数失败、PS域卡无效等 */
    IMSA_USIM_STATUS_AVAILABLE          = 0x02,            /**< 卡有效可用 */

    IMSA_USIM_STATUS_BUTT
};
typedef VOS_UINT8  IMSA_USIM_STATUS_ENUM_UINT8;


enum IMSA_OM_ERR_LOG_MSG_TYPE_ENUM
{
    ID_IMSA_OM_REG_ERROR_LOG_IND           = 0x01,                /* _H2ASN_MsgChoice IMSA_REG_ERROR_LOG_INFO_STRU */
    ID_IMSA_OM_CALL_ERROR_LOG_IND          = 0x02,                /* _H2ASN_MsgChoice IMSA_CALL_ERROR_LOG_INFO_STRU */


    ID_IMSA_OM_ERR_LOG_MSG_TYPE_BUTT
};
typedef VOS_UINT32    IMSA_OM_ERR_LOG_MSG_TYPE_ENUM_UINT32;


enum IMSA_ERR_LOG_MSG_TYPE_ENUM
{
    IMSA_ERR_LOG_MSG_SET_ERR_REPORT = 0x01, /* 上层触发故障上报 */
    IMSA_ERR_LOG_MSG_ERR_REPORT_END = 0x02, /* 故障上报结束 */
    IMSA_ERR_LOG_MSG_ON_OFF         = 0x03, /* 控制上类型开关 */
    IMSA_ERR_LOG_MSG_ERR_REPORT     = 0x04, /* 故障上报 */
    IMSA_ERR_LOG_MSG_FTM_REPORT     = 0x05, /* 工程模式主动上报 */
    IMSA_ERR_LOG_MSG_RESULT         = 0x06, /* 上报Result */
    IMSA_ERR_LOG_MSG_FTM_REQ        = 0x07, /* 工程模式命令 */
    IMSA_ERR_LOG_MSG_FTM_CNF        = 0x08, /* 工程模式响应 */


    IMSA_ERR_LOG_MSG_TYPE_BUTT
};
typedef VOS_UINT32    IMSA_ERR_LOG_MSG_TYPE_ENUM_UINT32;


enum IMSA_ERR_LOG_ALARM_TYPE_ENUM
{
    IMSA_ERR_LOG_ALARM_TYPE_COMMUNICATION               = 0x01,         /* 表示通信 */
    IMSA_ERR_LOG_ALARM_TYPE_TRANSACTION_QULITY          = 0x02,         /* 表示业务质量 */
    IMSA_ERR_LOG_ALARM_TYPE_PROCESS_ERROR               = 0x03,         /* 表示处理出错 */
    IMSA_ERR_LOG_ALARM_TYPE_EQIPMENT_ERROR              = 0x04,         /* 表示设备故障 */
    IMSA_ERR_LOG_ALARM_TYPE_ENVIR_ERROR                 = 0x05,         /* 表示环境故障 */

    IMSA_ERR_LOG_ALARM_TYPE_BUTT
};
typedef VOS_UINT16  IMSA_ERR_LOG_ALARM_TYPE_ENUM_UINT16;

/*****************************************************************************
 枚举名    :   IMSA_ERR_LOG_ERRLOG_PROCESS_EVENT_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  :  表示当前IMSA具体是哪一个流程
*****************************************************************************/
enum IMSA_ERR_LOG_ALM_ID_ENUM
{
    IMSA_ERR_LOG_ALM_CALL_FAIL_EVENT        = 0x01,    /* volte呼叫流程 */
    IMSA_ERR_LOG_ALM_REG_FAIL_EVENT         = 0x02,    /* IMS注册失败 */
    IMSA_ERR_LOG_ALM_MNTN                   = 0x03,    /* 溢出计数 */

    IMSA_ERR_LOG_ALM_BUTT
};
typedef VOS_UINT16  IMSA_ERR_LOG_ALM_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    :   IMSA_ERR_LOG_VOPS_STATUS_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  :  网侧是否具备IMS over PS能力
*****************************************************************************/
enum IMSA_ERR_LOG_VOPS_STATUS_ENUM
{
    IMSA_ERR_LOG_VOPS_STATUS_NOT_SUPPORT            = 0x01,         /* 支持 */
    IMSA_ERR_LOG_VOPS_STATUS_SUPPORT                = 0x02,         /* 不支持 */
    IMSA_ERR_LOG_VOPS_STATUS_BUTT
};
typedef VOS_UINT8 IMSA_ERR_LOG_VOPS_STATUS_ENUM_UINT8;

/*****************************************************************************
 枚举名    :   IMSA_ERR_LOG_PS_SERVICE_STATUS_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  :  当前网络PS服务状态
*****************************************************************************/
enum IMSA_ERR_LOG_PS_SERVICE_STATUS_ENUM
{
    IMSA_ERR_LOG_PS_SERVICE_STATUS_NORMAL_SERVICE       = 0x01,     /* 正常服务 */
    IMSA_ERR_LOG_PS_SERVICE_STATUS_LIMITED_SERVICE      = 0x02,     /* 受限服务 */
    IMSA_ERR_LOG_PS_SERVICE_STATUS_NO_SERVICE           = 0x03,     /* 无服务 */
    IMSA_ERR_LOG_PS_SERVICE_STATUS_BUTT
};
typedef VOS_UINT8 IMSA_ERR_LOG_PS_SERVICE_STATUS_ENUM_UINT8;


/*****************************************************************************
 枚举名    :   IMSA_ERR_LOG_PDN_CONN_STATUS_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  :  PDN连接状态
*****************************************************************************/
enum IMSA_ERR_LOG_PDN_CONN_STATUS_ENUM
{
    IMSA_ERR_LOG_PDN_CONN_STATUS_IDLE               = 0x01,         /* 不存在连接 */
    IMSA_ERR_LOG_PDN_CONN_STATUS_CONNING            = 0x02,         /* 正在建立连接 */
    IMSA_ERR_LOG_PDN_CONN_STATUS_RELEASING          = 0x03,         /* 正在释放连接 */
    IMSA_ERR_LOG_PDN_CONN_STATUS_CONN               = 0x04,         /* 存在连接 */
    IMSA_ERR_LOG_PDN_CONN_STATUS_BUTT
};
typedef VOS_UINT8  IMSA_ERR_LOG_PDN_CONN_STATUS_ENUM_UINT8;

/*****************************************************************************
 枚举名    :   IMSA_ERR_LOG_REG_STATUS_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  :  IMS注册状态
*****************************************************************************/
enum IMSA_ERR_LOG_REG_STATUS_ENUM
{
    IMSA_ERR_LOG_REG_STATUS_NOT_REGISTER            = 0x01,     /* 未注册状态 */
    IMSA_ERR_LOG_REG_STATUS_REGISTERING             = 0x02,     /* 正在注册状态 */
    IMSA_ERR_LOG_REG_STATUS_REGISTERED              = 0x03,     /* 已注册状态 */
    IMSA_ERR_LOG_REG_STATUS_DEREGING                = 0x04,     /* 正在去注册状态 */
    IMSA_ERR_LOG_REG_STATUS_WAIT_RETRY              = 0x05,     /* 正在等待重新尝试状态 */
    IMSA_ERR_LOG_REG_STATUS_PENDING                 = 0x06,     /* PENDING状态 */
    IMSA_ERR_LOG_REG_STATUS_ROLLING_BACK            = 0x07,     /* ROLLINGBACK状态 */
    IMSA_ERR_LOG_REG_STATUS_BUTT
};
typedef VOS_UINT8 IMSA_ERR_LOG_REG_STATUS_ENUM_UINT8;

/*****************************************************************************
 枚举名    :  IMSA_ERR_LOG_CALL_STATUS_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  :  呼叫状态
*****************************************************************************/
enum IMSA_ERR_LOG_CALL_STATUS_ENUM
{
    IMSA_ERR_LOG_CALL_STATUS_IDLE                   = 0x01,     /* IDLE状态     */
    IMSA_ERR_LOG_CALL_STATUS_DIALING                = 0x02,     /* DIALING状态  */
    IMSA_ERR_LOG_CALL_STATUS_TRYING                 = 0x03,     /* TRYING状态   */
    IMSA_ERR_LOG_CALL_STATUS_ALERTING               = 0x04,     /* ALERTING状态 */
    IMSA_ERR_LOG_CALL_STATUS_ACTIVE                 = 0x05,     /* ACTIVE状态   */
    IMSA_ERR_LOG_CALL_STATUS_INCOMING               = 0x06,     /* INCOMING状态 */
    IMSA_ERR_LOG_CALL_STATUS_HELD                   = 0x07,     /* HELD状态     */
    IMSA_ERR_LOG_CALL_STATUS_WAITING                = 0x08,     /* WAITING状态  */
    IMSA_ERR_LOG_CALL_STATUS_BUTT
};
typedef VOS_UINT8 IMSA_ERR_LOG_CALL_STATUS_ENUM_UINT8;

/*****************************************************************************
 枚举名    :  IMSA_ERR_LOG_MPTY_STATE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  :  是否多方呼叫
*****************************************************************************/
enum IMSA_ERR_LOG_MPTY_STATE_ENUM
{
    IMSA_ERR_LOG_CALL_NOT_IN_MPTY                   = 0x01,     /* 不是多方呼叫 */
    IMSA_ERR_LOG_CALL_IN_MPTY                       = 0x02,     /* 是多方呼叫 */
    IMSA_ERR_LOG_CALL_MPYT_STATE_BUTT
};
typedef VOS_UINT8  IMSA_ERR_LOG_MPTY_STATE_ENUM_UINT8;

/*****************************************************************************
 枚举名    :  IMSA_ERR_LOG_REGISTER_REASON_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  :  是否多方呼叫
*****************************************************************************/
enum IMSA_ERR_LOG_REGISTER_REASON_ENUM
{
    IMSA_ERR_LOG_REGISTER_REASON_ADDR_PARAM_NEW         = 0x01,    /* 使用新的地址队发起注册 */
    IMSA_ERR_LOG_REGISTER_REASON_ADDR_PARAM_SAME        = 0x02,  /* 使用当前地址队发起注册 */
    IMSA_ERR_LOG_REGISTER_REASON_ADDR_PARAM_NEXT        = 0x03,    /* 使用下个地址队发起注册 */
    IMSA_ERR_LOG_REGISTER_REASON_ADDR_PARAM_RESTORATION = 0x04,  /* 如果有其他地址队，则使用其他地址对发起注册，否则使用当前地址对发起注册 */
    IMSA_ERR_LOG_REGISTER_REASON_BUTT
};
typedef VOS_UINT8  IMSA_ERR_LOG_REGISTER_REASON_ENUM_UINT8;

/*****************************************************************************
 枚举名    :  IMSA_ERR_LOG_REG_FAIL_REASON_ENUM_UINT8
 协议表格  :
 ASN.1描述 :
 枚举说明  :  注册失败的原因值
*****************************************************************************/
enum IMSA_ERR_LOG_REG_FAIL_REASON_ENUM
{
    IMSA_ERR_LOG_REG_FAIL_REASON_REMOTE             = 0x01,     /**< 被服务器拒绝，具体拒绝原因值详见对应数据结构中的Status Code，根据不同的原因值可能会携带一些其它的头域 */
    IMSA_ERR_LOG_REG_FAIL_REASON_TIMEOUT            = 0x02,     /**< 发送或接收SIP报文超时，具体超时的定时器详见对应数据结构中的 Timer Id */
    IMSA_ERR_LOG_REG_FAIL_REASON_TRANSPORT          = 0x03,     /**< 发送或接收SIP报文时，底层传输发生错误 */
    IMSA_ERR_LOG_REG_FAIL_REASON_NO_ADDR_PAIR       = 0x04,     /**< 没有可以再使用的注册地址对 */
    IMSA_ERR_LOG_REG_FAIL_REASON_BUTT
};
typedef VOS_UINT8  IMSA_ERR_LOG_REG_FAIL_REASON_ENUM_UINT8;


enum IMSA_ERR_LOG_PDNREJ_CAUSE_ENUM
{
    /* IMSA内部错误 */
    IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_PARA_ERR                              = 1,    /**< CONN模块命令执行失败，由于参数错误 */
    IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_TIMER_EXP                             = 2,    /**< CONN模块命令执行失败，由于定时器超时 */
    IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_CONN_RELEASING                        = 3,    /**< CONN模块命令执行失败，由于正在释放连接 */
    IMSA_ERR_LOG_PDNREJ_CAUSE_PDP_ACTIVATE_LIMIT                         = 4,    /**< CONN模块命令执行失败，由于正在释放连接 */
    IMSA_ERR_LOG_PDNREJ_CAUSE_SAME_APN_OPERATING                         = 5,    /**< CONN模块命令执行失败，由于正在释放连接 */
    IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_OTHERS                                = 6,    /**< CONN模块命令执行失败，由于其他原因 */

    /*----------------------------------------------------------------------
       TAF上报的SM网络原因值, 取值范围[0x0100, 0x01FF]
       由于3GPP协议已经定义了具体的(E)SM网络原因值, (E)SM上报的取值为协议
       定义的原因值加上偏移量(0x100)
       (1) SM Cause : Refer to TS 24.008 section 10.5.6.6
       (2) ESM Cause: Refer to TS 24.301 section 9.9.4.4
    *---------------------------------------------------------------------*/
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_OPERATOR_DETERMINED_BARRING          =  8,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MBMS_BC_INSUFFICIENT                 =  24,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_LLC_OR_SNDCP_FAILURE                 =  25,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_INSUFFICIENT_RESOURCES               =  26,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MISSING_OR_UKNOWN_APN                =  27,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_UNKNOWN_PDP_ADDR_OR_TYPE             =  28,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_USER_AUTHENTICATION_FAIL             =  29,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_ACTIVATION_REJECTED_BY_GGSN          =  30,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_ACTIVATION_REJECTED_UNSPECIFIED      =  31,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SERVICE_OPTION_NOT_SUPPORTED         =  32,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_REQUESTED_SERVICE_NOT_SUBSCRIBED     =  33,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER        =  34,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_NSAPI_ALREADY_USED                   =  35,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_REGULAR_DEACTIVATION                 =  36,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_QOS_NOT_ACCEPTED                     =  37,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_NETWORK_FAILURE                      =  38,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_REACTIVATION_REQUESTED               =  39,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_FEATURE_NOT_SUPPORT                  =  40,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SEMANTIC_ERR_IN_TFT                  =  41,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SYNTACTIC_ERR_IN_TFT                 =  42,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_UKNOWN_PDP_CONTEXT                   =  43,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SEMANTIC_ERR_IN_PACKET_FILTER        =  44,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SYNCTACTIC_ERR_IN_PACKET_FILTER      =  45,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PDP_CONTEXT_WITHOUT_TFT_ACTIVATED    =  46,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MULTICAST_GROUP_MEMBERHHSHIP_TIMEOUT =  47,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_REQUEST_REJECTED_BCM_VIOLATION       =  48,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED         =  49,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PDP_TYPE_IPV4_ONLY_ALLOWED           =  50,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PDP_TYPE_IPV6_ONLY_ALLOWED           =  51,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SINGLE_ADDR_BEARERS_ONLY_ALLOWED     =  52,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_INFORMATION_NOT_RECEIVED             =  53,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PDN_CONNECTION_DOES_NOT_EXIST        =  54,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SAME_APN_MULTI_PDN_CONNECTION_NOT_ALLOWED =  55,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_COLLISION_WITH_NW_INITIATED_REQUEST  =  56,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_UNSUPPORTED_QCI_VALUE                =  59,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_BEARER_HANDLING_NOT_SUPPORTED        =  60,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_INVALID_TI                           =  81,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SEMANTICALLY_INCORRECT_MESSAGE       =  95,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_INVALID_MANDATORY_INFO               =  96,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MSG_TYPE_NON_EXISTENT                =  97,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MSG_TYPE_NOT_COMPATIBLE              =  98,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_IE_NON_EXISTENT                      =  99,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_CONDITIONAL_IE_ERR                   =  100,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MSG_NOT_COMPATIBLE                   =  101,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED             =  111,
    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_APN_RESTRICTION_INCOMPATIBLE         =  112,

    IMSA_ERR_LOG_PDNREJ_CAUSE_BUTT
};
typedef VOS_UINT32 IMSA_ERR_LOG_PDNREJ_CAUSE_ENUM_UINT32;


/*****************************************************************************
 枚举名    :  IMSA_ERR_LOG_CALL_FAIL_REASON_ENUM_UINT8
 协议表格  :
 ASN.1描述 :
 枚举说明  :  呼叫失败的原因值
*****************************************************************************/
enum IMSA_ERR_LOG_CALL_FAIL_REASON_ENUM
{
    IMSA_ERR_LOG_CALL_FAIL_REASON_REMOTE                                             = 1,     /**< 被服务器拒绝，具体拒绝原因值详见对应数据结构中的Status Code，根据不同的原因值可能会携带一些其它的头域 */
    IMSA_ERR_LOG_CALL_FAIL_REASON_TIMEOUT                                            = 2,     /**< 发送或接收SIP报文超时，具体超时的定时器详见对应数据结构中的 Timer Id */
    IMSA_ERR_LOG_CALL_FAIL_REASON_NOT_SUPPORTED_CALL_TYPE                            = 3,     /**< 呼叫类型不支持 */
    IMSA_ERR_LOG_CALL_FAIL_REASON_NOT_ALLOW_NEW_CALL                                 = 4,     /**< 不允许发起新的呼叫 */
    IMSA_ERR_LOG_CALL_FAIL_REASON_SERVICE_NOT_AVAILABLE                              = 5,     /**< 对应的服务不可用 */
    IMSA_ERR_LOG_CALL_FAIL_REASON_RES_READY_FAIL                                     = 6,     /**< 资源预留失败 */

    /* IMS上报的错误*/
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_MULTIPLE_CHOICES                               = 300,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_MOVED_PERMANENTLY                              = 301,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_MOVED_TEMPORARILY                              = 302,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_USE_PROXY                                      = 305,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_ALTERNATIVE_SERVICE                            = 380,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BAD_REQUEST                                    = 400,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_UNAUTHORIZED                                   = 401,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_PAYMENT_REQUIRED                               = 402,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_FORBIDDEN                                      = 403,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_NOT_FOUND                                      = 404,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_METHOD_NOT_ALLOWED                             = 405,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_NOT_ACCEPTABLE                                 = 406,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_PROXY_AUTHENTICATION_REQUIRED                  = 407,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_REQUEST_TIMEOUT                                = 408,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_CONFLICT_DEPRECATED                            = 409,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_GONE                                           = 410,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_CONDITIONAL_REQUEST_FAILED                     = 412,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_REQUEST_ENTITY_TOO_LARGE                       = 413,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_REQUEST_URI_TOO_LONG                           = 414,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_UNSUPPORTED_MEDIA_TYPE                         = 415,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_UNSUPPORTED_URI_SCHEME                         = 416,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_UNKNOWN_RESOURCE_PRIORITY                      = 417,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BAD_EXTENSION                                  = 420,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_EXTENSION_REQUIRED                             = 421,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_SESSION_INTERVAL_TOO_SMALL                     = 422,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_INTERVAL_TOO_BRIEF                             = 423,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BAD_LOCATION_INFORMATION                       = 424,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_USE_IDENTITY_HEADER                            = 428,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_PROVIDE_REFERRER_IDENTITY                      = 429,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_FLOW_FAILED                                    = 430,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_ANONYMITY_DISALLOWED                           = 433,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BAD_IDENTITY_INFO                              = 436,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_UNSUPPORTED_CERTIFICATE                        = 437,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_INVALID_IDENTITY_HEADER                        = 438,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_FIRST_HOP_LACKS_OUTBOUND_SUPPORT               = 439,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_MAX_BREADTH_EXCEEDED                           = 440,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BAD_INFO_PACKEAGE                              = 469,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_CONSENT_NEEDED                                 = 470,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_TEMPORARILY_UNAVAILABLE                        = 480,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_CALL_TRANSACTION_DOES_NOT_EXIST                = 481,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_LOOP_DETECTED                                  = 482,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_TOO_MANY_HOPS                                  = 483,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_ADDRESS_INCOMPLETE                             = 484,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_AMBIGUOUS                                      = 485,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BUSY_HERE                                      = 486,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_REQUEST_TERMINATED                             = 487,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_NOT_ACCEPTABLE_HERE                            = 488,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BAD_EVENT                                      = 489,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_REQUEST_PENDING                                = 491,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_UNDECIPHERABLE                                 = 493,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_SECURITY_AGREEMENT_REQUIRED                    = 494,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_SERVER_INTERNAL_ERROR                          = 500,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_NOT_IMPLEMENTED                                = 501,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BAD_GATEWAY                                    = 502,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_SERVICE_UNAVAILABLE                            = 503,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_SERVER_TIME_OUT                                = 504,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_VERSION_NOT_SUPPORTED                          = 505,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_MESSAGE_TOO_LARGE                              = 513,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_PRECONDITION_FAILURE                           = 580,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_BUSY_EVERYWHERE                                = 600,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_DECLINE                                        = 603,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_DOES_NOT_EXIST_ANYWHERE                        = 604,
    IMSA_ERR_LOG_CALL_FAIL_REASON_IMS_GLOBAL_NOT_ACCEPTABLE                          = 606,

    IMSA_ERR_LOG_CALL_FAIL_REASON_BUTT
};
typedef VOS_UINT16  IMSA_ERR_LOG_CALL_FAIL_REASON_ENUM_UINT16;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*****************************************************************************
 结构名称   :IMSA_ERR_LOG_CALL_FAIL_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : volte呼叫失败信息结构
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                          stHeader;           /*消息头*/
    IMSA_ERR_LOG_REG_STATUS_ENUM_UINT8              enNormRegStatus;      /* IMS普通注册注册状态 */
    IMSA_ERR_LOG_REG_STATUS_ENUM_UINT8              enEmcRegStatus;      /* IMS紧急注册状态 */
    IMSA_ERR_LOG_VOPS_STATUS_ENUM_UINT8             enVopsStatus;     /* 网侧是否具备IMS over PS能力 */
    IMSA_ERR_LOG_CALL_STATUS_ENUM_UINT8             enCallStatus;     /* 呼叫状态 */
    IMSA_ERR_LOG_CALL_FAIL_REASON_ENUM_UINT16       enCallFailReason;   /* 呼叫失败原因值 */
    VOS_UINT8                                       ucIsVoiceMediaExist;   /* 网侧是否建立专有承载 */
    VOS_UINT8                                       ucIsVideoMediaExist;   /* 网侧是否建立专有承载 */
    IMSA_ERR_LOG_MPTY_STATE_ENUM_UINT8              enMpty;           /* 是否MPTY */
    IMSA_ERR_LOG_PDN_CONN_STATUS_ENUM_UINT8         enPdnConnStatus;  /* PDN连接状态 */
    //IMSA_ERR_LOG_PDP_PCSCF_ADDRESS_STRU             stPcscfAddress;   /* P-CSCF地址*/
    VOS_CHAR                                        aucPcscfAddr[IMSA_ERR_LOG_IPV6_STRING_LEN+1];
    VOS_UINT8                                       aucRsv[3];
} IMSA_ERR_LOG_CALL_FAIL_STRU;

/*****************************************************************************
 结构名称   :IMSA_ERR_LOG_REG_FAIL_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : IMS注册失败结构
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                          stHeader;            /*消息头*/
    IMSA_ERR_LOG_VOPS_STATUS_ENUM_UINT8             enVopsStatus;        /* 网侧是否具备IMS over PS能力 */
    IMSA_ERR_LOG_PS_SERVICE_STATUS_ENUM_UINT8       enPsServiceStatus;   /* 当前网络PS服务状态 */
    IMSA_ERR_LOG_PDN_CONN_STATUS_ENUM_UINT8         enPdnConnStatus;     /* PDN连接状态 */
    IMSA_ERR_LOG_REGISTER_REASON_ENUM_UINT8         enRegisterReason;    /* 注册发起的原因 */
    IMSA_ERR_LOG_REG_FAIL_REASON_ENUM_UINT8         enRegFailReason;     /* 注册失败的原因 */
    VOS_CHAR                                        aucPcscfAddr[IMSA_ERR_LOG_IPV6_STRING_LEN+1];
    IMSA_USIM_STATUS_ENUM_UINT8                     enUsimStatus;        /* 卡是否有效 */
    VOS_UINT8                                       ausRsv[3];
    IMSA_ERR_LOG_PDNREJ_CAUSE_ENUM_UINT32           enPdnConnRejCause;   /* IMS PDN连接被拒原因值 */
} IMSA_ERR_LOG_REG_FAIL_STRU;

/*****************************************************************************
 结构名称   :IMSA_ERR_LOG_ERRLOG_IMS_REGISTER_FAILURE_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : IMS注册失败结构
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU                      stHeader;           /*消息头*/
    VOS_UINT32                                  ulCount;                            /* 缓存区溢出计数 */
} IMSA_ERR_LOG_MNTN_EVENT_STRU;

/*****************************************************************************
 结构名称   : IMSA_ERR_LOG_REPORT_REQ_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : OM发送给IMSA的ERROR REQ消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
    VOS_UINT16                          usModemID;
    VOS_UINT8                           aucRsv[2];
}IMSA_ERR_LOG_REPORT_REQ_STRU;

/*****************************************************************************
 结构名称   : IMSA_ERR_LOG_ERR_LOG_REPORT_CNF_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : IMSA回复ERRLOG应答数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulMsgType;
    VOS_UINT32                          ulMsgSN;
    VOS_UINT32                          ulRptlen;      /* 故障内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    VOS_UINT8                           aucContent[4]; /* 故障内容 */
} IMSA_ERR_LOG_REPORT_CNF_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    IMSA_ERR_LOG_REG_FAIL_STRU          stImsaRegErrlog;
}IMSA_REG_ERROR_LOG_INFO_STRU;
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    IMSA_ERR_LOG_CALL_FAIL_STRU         stImsaCallErrlog;
}IMSA_CALL_ERROR_LOG_INFO_STRU;

/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/










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

#endif /* end of LnasErrlogInterface.h */


