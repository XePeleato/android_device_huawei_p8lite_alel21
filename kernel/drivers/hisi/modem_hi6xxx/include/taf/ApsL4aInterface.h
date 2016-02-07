
#ifndef _APS_L4A_INTERFACE_H_
#define _APS_L4A_INTERFACE_H_

/*****************************************************************************
  1 其他头文件包含
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
#define APS_L4A_MAX_IPV6_PREFIX_NUM             (6)                             /* 最大IPv6地址前缀数量 */

#define APS_L4A_MAX_APN_LEN                     (99)                            /* APN最大长度 */
#define APS_L4A_MAX_IPV4_ADDR_LEN               (4)
#define APS_L4A_MAX_IPV6_ADDR_LEN               (16)

#define APS_L4A_MAX_ACCESS_NUM_LEN              (32)                            /* L模定义, 暂不使用 */
#define APS_L4A_MAX_USERNAME_LEN                (255)                           /* 鉴权用户名长度 */
#define APS_L4A_MAX_PASSWORD_LEN                (255)                           /* 鉴权密码长度 */

#define APS_L4A_MAX_SDF_PF_NUM                  (16)

#define APS_L4A_LTE_MAX_CID                     (31)

#define APS_L4A_AUTHDATA_MAX_USERNAME_LEN       (127)
#define APS_L4A_AUTHDATA_MAX_PASSWORD_LEN       (127)

#define APS_L4A_ERR_COMMON_SECTION_BEGIN        (0)
#define APS_L4A_ERR_SM_SECTION_BEGIN            (200)
#define APS_L4A_ERR_SM_NW_SECTION_BEGIN         (400)

/*****************************************************************************
  Maximum length of challenge used in authentication. The maximum length
  challenge name is the same as challenge.
*****************************************************************************/
#define APS_L4A_PPP_CHAP_CHALLNGE_LEN       (16)

/*****************************************************************************
  Maximum length of challenge name used in authentication.
*****************************************************************************/
#define APS_L4A_PPP_CHAP_CHALLNGE_NAME_LEN  (16)

/*****************************************************************************
  Maximum length of response used in authentication.
*****************************************************************************/
#define APS_L4A_PPP_CHAP_RESPONSE_LEN   (16)


/*****************************************************************************
  Maximum length of username used in authentication. It should be equal to
  the AUTHLEN(include one byte length) defined in PppInterface.h
*****************************************************************************/
#define APS_L4A_PPP_MAX_USERNAME_LEN    (99)


/*****************************************************************************
  Maximum length of password used in authentication. It should be equal to
  the PASSWORDLEN(include one byte length) defined in PppInterface.h
*****************************************************************************/
#define APS_L4A_PPP_MAX_PASSWORD_LEN    (99)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 枚举名: APS_L4A_ERR_CODE_ENUM
 枚举说明: APS和L4A之间定义的错误码
 1.日    期   : 2011年12月29日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_ERR_CODE_ENUM
{
    APS_L4A_SUCCESS                                         = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 0),
    APS_L4A_FAILURE                                         = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 1),
    APS_L4A_ERR_NULL_PTR                                    = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 2),
    APS_L4A_ERR_PARA_ERROR                                  = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 3),
    APS_L4A_ERR_TIME_OUT                                    = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 4),
    APS_L4A_ERR_ID_INVALID                                  = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 5),
    APS_L4A_ERR_NUM_VALUE_INVALID                           = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 6),
    APS_L4A_ERR_NUM_LEN_INVALID                             = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 7),
    APS_L4A_ERR_CAPABILITY_ERROR                            = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 8),
    APS_L4A_ERR_CLIENTID_NO_FREE                            = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 9),
    APS_L4A_ERR_CALLBACK_FUNC_ERROR                         = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 10),
    APS_L4A_ERR_MSG_DECODING_FAIL                           = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 11),
    APS_L4A_ERR_TI_ALLOC_FAIL                               = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 12),
    APS_L4A_ERR_TI_GET_FAIL                                 = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 13),
    APS_L4A_ERR_CMD_TYPE_ERROR                              = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 14),
    APS_L4A_ERR_MUX_LINK_EST_FAIL                           = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 15),
    APS_L4A_ERR_USIM_SIM_CARD_NOTEXIST                      = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 16),
    APS_L4A_ERR_CLIENTID_NOT_EXIST                          = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 17),
    APS_L4A_ERR_NEED_PIN1                                   = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 18),
    APS_L4A_ERR_NEED_PUK1                                   = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 19),
    APS_L4A_ERR_USIM_SIM_INVALIDATION                       = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 20),
    APS_L4A_ERR_UNSPECIFIED_ERROR                           = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 21),
    APS_L4A_ERR_INSUFFICIENT_RESOURCES                      = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 22),
    APS_L4A_ERR_CONN_OR_NO_IMSI                             = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 23),
    APS_L4A_ERR_NOT_SUPPORT_NOW                             = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 24),
    APS_L4A_ERR_NOT_STARTED_YET                             = (APS_L4A_ERR_COMMON_SECTION_BEGIN + 25),

    APS_L4A_ERR_SM_CALL_CID_INVALID                         = (APS_L4A_ERR_SM_SECTION_BEGIN + 0),
    APS_L4A_ERR_SM_CALL_CID_ACTIVE                          = (APS_L4A_ERR_SM_SECTION_BEGIN + 1),
    APS_L4A_ERR_SM_CALL_MOD_CID_NOT_MATCH                   = (APS_L4A_ERR_SM_SECTION_BEGIN + 2),
    APS_L4A_ERR_SM_CALL_CID_NOT_MATCH_BEARER                = (APS_L4A_ERR_SM_SECTION_BEGIN + 3),
    APS_L4A_ERR_SM_BEARER_TYPE_NOT_DEDICATED                = (APS_L4A_ERR_SM_SECTION_BEGIN + 4),
    APS_L4A_ERR_SM_NO_INCOMING_CALL                         = (APS_L4A_ERR_SM_SECTION_BEGIN + 5),
    APS_L4A_ERR_SM_LLC_OR_SNDCP_FAIL                        = (APS_L4A_ERR_SM_SECTION_BEGIN + 6),
    APS_L4A_ERR_SM_REACTIV_REQ                              = (APS_L4A_ERR_SM_SECTION_BEGIN + 7),
    APS_L4A_ERR_SM_FEATURE_NOT_SUPPORT                      = (APS_L4A_ERR_SM_SECTION_BEGIN + 8),
    APS_L4A_ERR_SM_GPRS_ATTACH_FAIL                         = (APS_L4A_ERR_SM_SECTION_BEGIN + 9),
    APS_L4A_ERR_SM_OTHER_BEARER_CONT_EXIST                  = (APS_L4A_ERR_SM_SECTION_BEGIN + 10),
    APS_L4A_ERR_SM_NSAPI_IN_USE                             = (APS_L4A_ERR_SM_SECTION_BEGIN + 11),
    APS_L4A_ERR_SM_COLLISION_WITH_NW                        = (APS_L4A_ERR_SM_SECTION_BEGIN + 12),
    APS_L4A_ERR_SM_BEARER_INACTIVE                          = (APS_L4A_ERR_SM_SECTION_BEGIN + 13),
    APS_L4A_ERR_SM_DETACHED                                 = (APS_L4A_ERR_SM_SECTION_BEGIN + 14),
    APS_L4A_ERR_SM_LINK_CID_INVALID                         = (APS_L4A_ERR_SM_SECTION_BEGIN + 15),
    APS_L4A_ERR_SM_LINK_BEARER_INACTIVE                     = (APS_L4A_ERR_SM_SECTION_BEGIN + 16),
    APS_L4A_ERR_SM_APN_LEN_ILLEGAL                          = (APS_L4A_ERR_SM_SECTION_BEGIN + 17),
    APS_L4A_ERR_SM_APN_SYNTACTICAL_ERROR                    = (APS_L4A_ERR_SM_SECTION_BEGIN + 18),
    APS_L4A_ERR_SM_SET_APN_BEFORE_SET_AUTH                  = (APS_L4A_ERR_SM_SECTION_BEGIN + 19),
    APS_L4A_ERR_SM_AUTH_TYPE_ILLEGAL                        = (APS_L4A_ERR_SM_SECTION_BEGIN + 20),
    APS_L4A_ERR_SM_USER_NAME_TOO_LONG                       = (APS_L4A_ERR_SM_SECTION_BEGIN + 21),
    APS_L4A_ERR_SM_USER_PASSWORD_TOO_LONG                   = (APS_L4A_ERR_SM_SECTION_BEGIN + 22),
    APS_L4A_ERR_SM_ACCESS_NUM_TOO_LONG                      = (APS_L4A_ERR_SM_SECTION_BEGIN + 23),
    APS_L4A_ERR_SM_CALL_CID_IN_OPERATION                    = (APS_L4A_ERR_SM_SECTION_BEGIN + 24),
    APS_L4A_ERR_SM_BEARER_TYPE_NOT_DEFAULT                  = (APS_L4A_ERR_SM_SECTION_BEGIN + 25),
    APS_L4A_ERR_SM_BEARER_TYPE_ILLEGAL                      = (APS_L4A_ERR_SM_SECTION_BEGIN + 26),
    APS_L4A_ERR_SM_MUST_EXIST_DEFAULT_TYPE_CID              = (APS_L4A_ERR_SM_SECTION_BEGIN + 27),
    APS_L4A_ERR_SM_PDN_TYPE_ILLEGAL                         = (APS_L4A_ERR_SM_SECTION_BEGIN + 28),
    APS_L4A_ERR_SM_IPV4_ADDR_ALLOC_TYPE_ILLEGAL             = (APS_L4A_ERR_SM_SECTION_BEGIN + 29),
    APS_L4A_ERR_SM_SUSPENDED                                = (APS_L4A_ERR_SM_SECTION_BEGIN + 30),

    APS_L4A_ERR_SM_NW_OPERATOR_DETERMINED_BARRING           = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 0),
    APS_L4A_ERR_SM_NW_INSUFFICIENT_RESOURCES                = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 1),
    APS_L4A_ERR_SM_NW_MISSING_OR_UKNOWN_APN                 = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 2),
    APS_L4A_ERR_SM_NW_UNKNOWN_PDN_TYPE                      = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 3),
    APS_L4A_ERR_SM_NW_USER_AUTH_FAIL                        = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 4),
    APS_L4A_ERR_SM_NW_REQ_REJ_BY_SGW_OR_PGW                 = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 5),
    APS_L4A_ERR_SM_NW_REQ_REJ_UNSPECITY                     = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 6),
    APS_L4A_ERR_SM_NW_SERVICE_OPTION_NOT_SUPPORT            = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 7),
    APS_L4A_ERR_SM_NW_REQ_SERVICE_NOT_SUBSCRIBE             = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 8),
    APS_L4A_ERR_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER         = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 9),
    APS_L4A_ERR_SM_NW_PTI_ALREADY_IN_USE                    = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 10),
    APS_L4A_ERR_SM_NW_REGULAR_DEACT                         = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 11),
    APS_L4A_ERR_SM_NW_EPS_QOS_NOT_ACCEPT                    = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 12),
    APS_L4A_ERR_SM_NW_NET_FAIL                              = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 13),
    APS_L4A_ERR_SM_NW_SEMANTIC_ERR_IN_TFT                   = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 14),
    APS_L4A_ERR_SM_NW_SYNTACTIC_ERR_IN_TFT                  = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 15),
    APS_L4A_ERR_SM_NW_INVALID_EPS_BERER_IDENTITY            = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 16),
    APS_L4A_ERR_SM_NW_SEMANTIC_ERR_IN_PACKET_FILTER         = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 17),
    APS_L4A_ERR_SM_NW_SYNCTACTIC_ERR_IN_PACKET_FILTER       = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 18),
    APS_L4A_ERR_SM_NW_BEARER_WITHOUT_TFT_ACT                = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 19),
    APS_L4A_ERR_SM_NW_PTI_MISMATICH                         = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 20),
    APS_L4A_ERR_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED          = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 21),
    APS_L4A_ERR_SM_NW_PDN_TPYE_IPV4_ONLY_ALLOWED            = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 22),
    APS_L4A_ERR_SM_NW_PDN_TPYE_IPV6_ONLY_ALLOWED            = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 23),
    APS_L4A_ERR_SM_NW_SINGLE_ADDR_BERERS_ONLY_ALLOWED       = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 24),
    APS_L4A_ERR_SM_NW_ESM_INFO_NOT_RECEIVED                 = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 25),
    APS_L4A_ERR_SM_NW_PDN_CONN_NOT_EXIST                    = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 26),
    APS_L4A_ERR_SM_NW_MULTI_PDN_CONN_FOR_ONE_APN_NOT_ALLOWED = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 27),
    APS_L4A_ERR_SM_NW_COLLISION_WITH_NW_INTIAL_REQEST       = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 28),
    APS_L4A_ERR_SM_NW_INVALID_PTI_VALUE                     = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 29),
    APS_L4A_ERR_SM_NW_SYNCTACTIC_INCORRECT_MSG              = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 30),
    APS_L4A_ERR_SM_NW_INVALID_MANDATORY_INFOR               = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 31),
    APS_L4A_ERR_SM_NW_MSG_TYPE_NON_EXIST                    = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 32),
    APS_L4A_ERR_SM_NW_MSG_TYPE_NOT_COMPATIBLE_WITH_PROT     = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 33),
    APS_L4A_ERR_SM_NW_INFOR_ELEM_NON_EXIST                  = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 34),
    APS_L4A_ERR_SM_NW_CONDITIONAL_IE_ERROR                  = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 35),
    APS_L4A_ERR_SM_NW_MSG_NOT_COMPATIBLE_WITH_PROT          = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 36),
    APS_L4A_ERR_SM_NW_PROT_ERR_UNSPECIFIED                  = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 37),
    APS_L4A_ERR_SM_NW_APN_RESTRICTION_INCOMPATIBLE_WITH_ACT_EPS_BEARER = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 38),
    APS_L4A_ERR_SM_NW_UNSUPPORTED_QCI_VALUE                 = (APS_L4A_ERR_SM_NW_SECTION_BEGIN + 39),

    APS_L4A_ERR_BUTT
};
typedef VOS_UINT32 APS_L4A_ERR_CODE_ENUM_UINT32;


enum APS_L4A_MSG_ID_ENUM
{
    ID_APS_L4A_SET_CGACT_REQ            = 0x0001020A,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGACT_REQ_STRU */
    ID_APS_L4A_SET_CGCMOD_REQ           = 0x0001022A,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGCMOD_REQ_STRU */
    ID_APS_L4A_SET_NDISCONN_REQ         = 0x00010291,                           /* _H2ASN_MsgChoice APS_L4A_SET_NDISCONN_REQ_STRU */
    ID_APS_L4A_SET_CGDCONT_REQ          = 0x00010222,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGDCONT_REQ_STRU */
    ID_APS_L4A_SET_CGDSCONT_REQ         = 0x00010226,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGDSCONT_REQ_STRU */
    ID_APS_L4A_SET_CGTFT_REQ            = 0x0001022C,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGTFT_REQ_STRU */
    ID_APS_L4A_SET_CGANS_REQ            = 0x00010212,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGANS_REQ_STRU */
    ID_APS_L4A_SET_CGAUTO_REQ           = 0x00010214,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGAUTO_REQ_STRU */
    ID_APS_L4A_SET_CGCONTRDP_REQ        = 0x00010232,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGCONTRDP_REQ_STRU */
    ID_APS_L4A_SET_CGSCONTRDP_REQ       = 0x00010234,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGSCONTRDP_REQ_STRU */
    ID_APS_L4A_SET_CGTFTRDP_REQ         = 0x00010230,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGTFTRDP_REQ_STRU */
    ID_APS_L4A_SET_CGEQOS_REQ           = 0x00010240,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGEQOS_REQ_STRU */
    ID_APS_L4A_SET_CGEQOSRDP_REQ        = 0x00010244,                           /* _H2ASN_MsgChoice APS_L4A_SET_CGEQOSRDP_REQ_STRU */
    ID_APS_L4A_SET_AUTHDATA_REQ         = 0x00010298,                           /* _H2ASN_MsgChoice APS_L4A_SET_AUTHDATA_REQ_STRU */
    ID_APS_L4A_IPV6_INFO_NOTIFY_IND     = 0x00010299,                           /* _H2ASN_MsgChoice APS_L4A_IPV6_INFO_NOTIFY_IND_STRU */
    ID_APS_L4A_ABORT_REQ                = 0x0001029A,                           /* _H2ASN_MsgChoice APS_L4A_ABORT_REQ_STRU */

    ID_APS_L4A_GET_LTE_CS_REQ           = 0x000102A4,                           /* _H2ASN_MsgChoice APS_L4A_GET_LTE_CS_REQ_STRU */
    ID_APS_L4A_SET_PDPROFMOD_REQ        = 0x000102A5,                           /* _H2ASN_MsgChoice APS_L4A_SET_PDPROFMOD_REQ_STRU */
    ID_APS_L4A_GET_CEMODE_REQ           = 0x000102A6,


    ID_APS_L4A_PPP_DIAL_REQ             = 0x000102A7,                           /* _H2ASN_MsgChoice APS_L4A_PPP_DIAL_REQ_STRU */
    ID_APS_L4A_PS_CALL_END_REQ          = 0x000102A8,                           /* _H2ASN_MsgChoice APS_L4A_PS_CALL_END_REQ_STRU */

    ID_L4A_APS_PDP_ACTIVATE_CNF         = 0x1001,                               /* _H2ASN_MsgChoice APS_L4A_PDP_ACTIVATE_CNF_STRU */
    ID_L4A_APS_PDP_ACTIVATE_REJ         = 0x1002,                               /* _H2ASN_MsgChoice APS_L4A_PDP_ACTIVATE_REJ_STRU */
    ID_L4A_APS_PDP_ACTIVATE_IND         = 0x1003,                               /* _H2ASN_MsgChoice APS_L4A_PDP_ACTIVATE_IND_STRU */
    ID_L4A_APS_PDP_MODIFY_CNF           = 0x1004,                               /* _H2ASN_MsgChoice APS_L4A_PDP_MODIFY_CNF_STRU */
    ID_L4A_APS_PDP_MODIFY_REJ           = 0x1005,                               /* _H2ASN_MsgChoice APS_L4A_PDP_MODIFY_REJ_STRU */
    ID_L4A_APS_PDP_MODIFY_IND           = 0x1006,                               /* _H2ASN_MsgChoice APS_L4A_PDP_MODIFY_IND_STRU */
    ID_L4A_APS_PDP_DEACTIVATE_CNF       = 0x1007,                               /* _H2ASN_MsgChoice APS_L4A_PDP_DEACTIVATE_CNF_STRU */
    ID_L4A_APS_PDP_DEACTIVATE_IND       = 0x1008,                               /* _H2ASN_MsgChoice APS_L4A_PDP_DEACTIVATE_IND_STRU */
    ID_L4A_APS_SET_NDISCONN_CNF         = 0x1009,                               /* _H2ASN_MsgChoice APS_L4A_SET_NDISCONN_CNF_STRU */
    ID_L4A_APS_SET_CGDCONT_CNF          = 0x100A,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGDCONT_CNF_STRU */
    ID_L4A_APS_SET_CGDSCONT_CNF         = 0x100B,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGDSCONT_CNF_STRU */
    ID_L4A_APS_SET_CGTFT_CNF            = 0x100C,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGTFT_CNF_STRU */
    ID_L4A_APS_SET_CGANS_CNF            = 0x100D,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGANS_CNF_STRU */
    ID_L4A_APS_SET_CGAUTO_CNF           = 0x100E,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGAUTO_CNF_STRU */
    ID_L4A_APS_SET_CGCONTRDP_CNF        = 0x100F,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGCONTRDP_CNF_STRU */
    ID_L4A_APS_SET_CGSCONTRDP_CNF       = 0x1010,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGSCONTRDP_CNF_STRU */
    ID_L4A_APS_SET_CGTFTRDP_CNF         = 0x1011,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGTFTRDP_CNF_STRU */
    ID_L4A_APS_SET_CGEQOS_CNF           = 0x1012,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGEQOS_CNF_STRU */
    ID_L4A_APS_SET_CGEQOSRDP_CNF        = 0x1013,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGEQOSRDP_CNF_STRU */
    ID_L4A_APS_SET_AUTHDATA_CNF         = 0x1014,                               /* _H2ASN_MsgChoice APS_L4A_SET_AUTHDATA_CNF_STRU */

    ID_L4A_APS_SET_CGACT_CNF            = 0x1015,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGACT_CNF_STRU */
    ID_L4A_APS_SET_CGCMOD_CNF           = 0x1016,                               /* _H2ASN_MsgChoice APS_L4A_SET_CGCMOD_CNF_STRU */
    ID_L4A_APS_PDP_DEACTIVATE_REJ       = 0x1017,                               /* _H2ASN_MsgChoice APS_L4A_PDP_DEACTIVATE_REJ_STRU */

    ID_L4A_APS_GET_LTE_CS_CNF           = 0x1018,                               /* _H2ASN_MsgChoice APS_L4A_GET_LTE_CS_CNF_STRU */
    ID_L4A_APS_SET_PDPROFMOD_CNF        = 0x1019,                               /* _H2ASN_MsgChoice APS_L4A_SET_PDPROFMOD_CNF_STRU */
    ID_L4A_APS_GET_CEMODE_CNF           = 0x101d,                               /* _H2ASN_MsgChoice APS_L4A_GET_LTE_CS_CNF_STRU */


    ID_L4A_APS_PPP_DIAL_CNF             = 0x101E,                               /* _H2ASN_MsgChoice APS_L4A_PPP_DIAL_CNF_STRU */
    ID_L4A_APS_PS_CALL_END_CNF          = 0x101F,                               /* _H2ASN_MsgChoice APS_L4A_PS_CALL_END_CNF_STRU */

    ID_APS_L4A_MSG_BUTT
};
typedef VOS_UINT32 APS_L4A_MSG_ID_ENUM_UINT32;

/*****************************************************************************
 枚举名: APS_L4A_PDP_TYPE_ENUM_UINT8
 枚举说明: APS和L4A之间PDP的类型
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_PDP_TYPE_ENUM
{
    APS_L4A_PDP_IPV4                            = 0x01,                         /* IPV4类型 */
    APS_L4A_PDP_IPV6                            = 0x02,                         /* IPV6类型 */
    APS_L4A_PDP_IPV4V6                          = 0x03,                         /* IPV4V6类型 */
    APS_L4A_PDP_PPP                             = 0x04,                         /* PPP类型 */

    APS_L4A_PDP_BUTT                            = 0xFF
};
typedef VOS_UINT8 APS_L4A_PDP_TYPE_ENUM_UINT8;


/*****************************************************************************
 枚举名: APS_L4A_PDP_DATA_COMP_ENUM_UINT8
 枚举说明: PDP数据压缩方式
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_PDP_DATA_COMP_ENUM
{
    APS_L4A_PDP_DATA_COMP_OFF                   = 0x00,                         /* default if value is omitted */
    APS_L4A_PDP_DATA_COMP_ON                    = 0x01,                         /* manufacturer preferred compression */
    APS_L4A_PDP_DATA_COMP_V42                   = 0x02,                         /* V42压缩算法 */

    APS_L4A_PDP_DATA_COMP_BUTT                  = 0xFF
};
typedef VOS_UINT8 APS_L4A_PDP_DATA_COMP_ENUM_UINT8;

/*****************************************************************************
 枚举名: APS_L4A_PDP_HEAD_COMP_ENUM_UINT8
 枚举说明: PDP头压缩方式
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_PDP_HEAD_COMP_ENUM
{
    APS_L4A_PDP_HEAD_COMP_OFF                   = 0x00,                         /* default if value is omitted */
    APS_L4A_PDP_HEAD_COMP_ON                    = 0x01,                         /* manufacturer preferred compression */
    APS_L4A_PDP_HEAD_COMP_RFC1144               = 0x02,                         /* RFC1144压缩算法，applicable for SNDCP only */
    APS_L4A_PDP_HEAD_COMP_RFC2507               = 0x03,                         /* RFC2507，applicable for SNDCP only */

    APS_L4A_PDP_HEAD_COMP_BUTT                  = 0xFF
};
typedef VOS_UINT8 APS_L4A_PDP_HEAD_COMP_ENUM_UINT8;

/*****************************************************************************
 枚举名: APS_L4A_TRAFFIC_CLASS_ENUM_UINT8
 枚举说明: Traffic Class类型的定义
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_TRAFFIC_CLASS_ENUM
{
    APS_L4A_TRAFFIC_CONVERSATIONAL_CLASS        = 0x00,                         /* Conversational class */
    APS_L4A_TRAFFIC_STREAMING_CLASS             = 0x01,                         /* Streaming class */
    APS_L4A_TRAFFIC_BACKGROUND_CLASS            = 0x02,                         /* Interactive class */
    APS_L4A_TRAFFIC_SUBSCRIBED_VALUE            = 0x03,                         /* Background class */

    APS_L4A_TRAFFIC_CLASS_BUTT                  = 0xFF
};
typedef VOS_UINT8 APS_L4A_TRAFFIC_CLASS_ENUM_UINT8;

/*****************************************************************************
 枚举名: APS_L4A_TRAFFIC_CLASS_ENUM_UINT8
 枚举说明: 发送优先级的定义
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_DELIVER_ORDER_ENUM
{
    APS_L4A_DELIVER_WITHOUT_DELIVER_ORDER       = 0x00,                         /* 0 : Without delivery order */
    APS_L4A_DELIVER_WITH_DELIVER_ORDER          = 0x01,                         /* 1 : With delivery order */
    APS_L4A_DELIVER_SUBSCRIBED_VAL              = 0x02,                         /* 2 : Subscribed value */

    APS_L4A_DELIVER_ORDER_BUTT                  = 0xFF
};
typedef VOS_UINT8 APS_L4A_DELIVER_ORDER_ENUM_UINT8;

/*****************************************************************************
 枚举名: APS_L4A_ERR_SDU_DELIVERED_ENUM_UINT8
 枚举说明: 错误的 SDU是否被发送
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_ERR_SDU_DELIVERED_ENUM
{
    APS_L4A_ERR_SDU_NOT_DELIVERED               = 0x00,                         /* 0 : Erroneous SDUs are not delivered */
    APS_L4A_ERR_SDU_DELIVERED                   = 0x01,                         /* 1 : Erroneous SDUs are delivered */
    APS_L4A_ERR_SDU_NOT_DETECT                  = 0x02,                         /* 2 : No detect */
    APS_L4A_ERR_SDU_SUBSCRIBED_VAl              = 0x03,                         /* 3 : subscribed value */

    APS_L4A_ERR_SDU_DELIVERED_BUTT              = 0xFF
};
typedef VOS_UINT8 APS_L4A_ERR_SDU_DELIVERED_ENUM_UINT8;


/*****************************************************************************
 枚举名: APS_L4A_RESIDUAL_BER_ENUM_UINT8
 枚举说明: SDU残留比特误码率
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_RESIDUAL_BER_ENUM
{
    APS_L4A_RESIDUAL_BER_SUBSCRIBED_VAL         = 0x00,                         /* 0 : subscribed value */
    APS_L4A_RESIDUAL_BER_5E2                    = 0x01,                         /* 1 : 5*10-2 */
    APS_L4A_RESIDUAL_BER_1E2                    = 0x02,                         /* 2 : 1*10-2 */
    APS_L4A_RESIDUAL_BER_5E3                    = 0x03,                         /* 3 : 5*10-3 */
    APS_L4A_RESIDUAL_BER_4E3                    = 0x04,                         /* 4 : 4*10-3 */
    APS_L4A_RESIDUAL_BER_1E3                    = 0x05,                         /* 5 : 1*10-3 */
    APS_L4A_RESIDUAL_BER_1E4                    = 0x06,                         /* 6 : 1*10-4 */
    APS_L4A_RESIDUAL_BER_1E5                    = 0x07,                         /* 7 : 1*10-5 */
    APS_L4A_RESIDUAL_BER_1E6                    = 0x08,                         /* 8 : 1*10-6 */
    APS_L4A_RESIDUAL_BER_6E8                    = 0x09,                         /* 9 : 6*10-8 */

    APS_L4A_RESIDUAL_BER_BUTT                   = 0xFF
};
typedef VOS_UINT8 APS_L4A_RESIDUAL_BER_ENUM_UINT8;



/*****************************************************************************
 枚举名: APS_L4A_SDU_ERR_RATIO_ENUM_UINT8
 枚举说明: SDU误码率
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_SDU_ERR_RATIO_ENUM
{
    APS_L4A_SDU_ERR_RATIO_SUBSCRIBED_VAL        = 0x00,                           /* 0 : subscribed value */
    APS_L4A_SDU_ERR_RATIO_1E2                   = 0x01,                           /* 1 : 5*10-2 */
    APS_L4A_SDU_ERR_RATIO_7E3                   = 0x02,                           /* 2 : 1*10-2 */
    APS_L4A_SDU_ERR_RATIO_1E3                   = 0x03,                           /* 3 : 5*10-3 */
    APS_L4A_SDU_ERR_RATIO_1E4                   = 0x04,                           /* 4 : 4*10-3 */
    APS_L4A_SDU_ERR_RATIO_1E5                   = 0x05,                           /* 5 : 1*10-3 */
    APS_L4A_SDU_ERR_RATIO_1E6                   = 0x06,                           /* 6 : 1*10-4 */
    APS_L4A_SDU_ERR_RATIO_1E1                   = 0x07,                           /* 7 : 1*10-5 */

    APS_L4A_SDU_ERR_RATIO_BUTT                  = 0xFF
};
typedef VOS_UINT8 APS_L4A_SDU_ERR_RATIO_ENUM_UINT8;

/*****************************************************************************
 枚举名: APS_L4A_TRAFFIC_HANDLE_PRIO_ENUM_UINT8
 枚举说明: 流量处理优先级
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_TRAFFIC_HANDLE_PRIO_ENUM
{
    APS_L4A_TRAFFIC_HANDLE_PRIO_SUBSCRIBED      = 0x00,                           /* 0 : Subscribed value */
    APS_L4A_TRAFFIC_HANDLE_PRIO_LEV1            = 0x01,                           /* 1 : Priority level 1 */
    APS_L4A_TRAFFIC_HANDLE_PRIO_LEV2            = 0x02,                           /* 2 : Priority level 2 */
    APS_L4A_TRAFFIC_HANDLE_PRIO_LEV3            = 0x03,                           /* 3 : Priority level 3 */

    APS_L4A_TRAFFIC_HANDLE_PRIO_BUTT            = 0xFF
};
typedef VOS_UINT8 APS_L4A_TRAFFIC_HANDLE_PRIO_ENUM_UINT8;

/*****************************************************************************
 枚举名: APS_L4A_PDP_STATE_ENUM_UINT8
 枚举说明: PDP状态
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_PDP_STATE_ENUM
{
    APS_L4A_PDP_STATE_INACTIVE                  = 0x00,                         /* PDP没激活 */
    APS_L4A_PDP_STATE_ACTIVE                    = 0x01,                         /* PDP已激活 */

    APS_L4A_PDP_STATE_BUTT                      = 0xFF
};
typedef VOS_UINT32 APS_L4A_PDP_STATE_ENUM_UINT32;

/*****************************************************************************
 枚举名: APS_L4A_TFT_FILTER_ENUM_UINT8
 枚举说明: TFT过滤器方向
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_TFT_FILTER_ENUM
{
    APS_L4A_TFT_FILTER_PRE_REL7                 = 0x00,                         /* 0 - Pre-Release 7 TFT filter (see 3GPP TS 24.008 [8], table 10.5.162) */
    APS_L4A_TFT_FILTER_UPLINK                   = 0x01,                         /* 1 - Uplink */
    APS_L4A_TFT_FILTER_DOWNLINK                 = 0x02,                         /* 2 - Downlink */
    APS_L4A_TFT_FILTER_BIDIRECTION              = 0x03,                         /* 3 - Birectional (Up & Downlink) (default if omitted) */

    APS_L4A_TFT_FILTER_BUTT                     = 0xFF
};
typedef VOS_UINT8 APS_L4A_TFT_FILTER_ENUM_UINT8;

/*****************************************************************************
 枚举名: APS_L4A_AUTO_ANS_MODE_ENUM_UINT32
 枚举说明: 自动应答模式
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
enum APS_L4A_AUTO_ANS_MODE_ENUM
{
    APS_L4A_AUTO_ANS_MODE_TURN_OFF              = 0x00,                         /* 0 turn off automatic response for Packet Domain only */
    APS_L4A_AUTO_ANS_MODE_TURN_ON               = 0x01,                         /* 1 turn on automatic response for Packet Domain only */
    APS_L4A_AUTO_ANS_MODE_PS_ONLY               = 0x02,                         /* 2 modem compatibility mode, Packet Domain only */
    APS_L4A_AUTO_ANS_MODE_PS_CS                 = 0x03,                         /* 3 modem compatibility mode, Packet Domain and circuit switched calls (default) */
    APS_L4A_AUTO_ANS_MODE_PS_ONLY_NEG_RSP       = 0x04,                         /* 4 turn on automatic negative response for Packet Domain only */

    APS_L4A_AUTO_ANS_MODE_BUTT                  = 0xFF
};
typedef VOS_UINT32 APS_L4A_AUTO_ANS_MODE_ENUM_UINT32;
enum APS_L4A_IPV4_ADDR_ALLOC_TYPE_ENUM
{
   APS_L4A_IPV4_ADDR_ALLOC_TYPE_NAS_SIGNALING   = 0x00,
   APS_L4A_IPV4_ADDR_ALLOC_TYPE_DHCP            = 0x01,

   APS_L4A_IPV4_ADDR_ALLOC_TYPE_BUTT            = 0xFF
};
typedef VOS_UINT8 APS_L4A_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT8;


enum APS_L4A_BEARER_TYPE_ENUM
{
    APS_L4A_BEARER_TYPE_DEFAULT                 = 0x00,                         /* 缺省承载 */
    APS_L4A_BEARER_TYPE_DEDICATED               = 0x01,                         /* 专有承载 */

    APS_L4A_BEARER_TYPE_BUTT                    = 0xFF
};
typedef VOS_UINT8 APS_L4A_BEARER_TYPE_ENUM_UINT8;


enum APS_L4A_GW_AUTH_TYPE_ENUM
{
    APS_L4A_GW_AUTH_TYPE_NONE                   = 0x00,
    APS_L4A_GW_AUTH_TYPE_PAP                    = 0x01,
    APS_L4A_GW_AUTH_TYPE_CHAP                   = 0x02,

    APS_L4A_GW_AUTH_TYPE_BUTT                   = 0xFF
};
typedef VOS_UINT32 APS_L4A_GW_AUTH_TYPE_ENUM_UINT32;


enum APS_L4A_BEARER_MANAGE_TYPE_ENUM
{
    APS_L4A_BEARER_MANAGE_TYPE_ACCEPT           = 0x00,                         /* 接受 */
    APS_L4A_BEARER_MANAGE_TYPE_REJ              = 0x01,                         /* 拒绝 */

    APS_L4A_BEARER_MANANGE_TYPE_BUTT            = 0xFF
};
typedef VOS_UINT8 APS_L4A_BEARER_MANAGE_TYPE_ENUM_UINT8;


enum APS_L4A_BEARER_MANAGE_MODE_ENUM
{
    APS_L4A_BEARER_MANAGE_MODE_AUTO             = 0x00,                         /* 自动 */
    APS_L4A_BEARER_MANAGE_MODE_MANUAL           = 0x01,                         /* 手动 */

    APS_L4A_BEARER_MANAGE_MODE_BUTT             = 0xFF
};
typedef VOS_UINT8 APS_L4A_BEARER_MANAGE_MODE_ENUM_UINT8;


enum APS_L4A_AUTH_TYPE_ENUM
{
    APS_L4A_AUTH_TYPE_NONE              = 0x00,
    APS_L4A_AUTH_TYPE_PAP               = 0x01,                                 /* _H2ASN_IeChoice_Export APS_L4A_PAP_PARAM_STRU */
    APS_L4A_AUTH_TYPE_CHAP              = 0x02,                                 /* _H2ASN_IeChoice_Export APS_L4A_CHAP_PARAM_STRU */
    APS_L4A_AUTH_TYPE_BUTT              = 0xFF
};
typedef VOS_UINT8 APS_L4A_AUTH_TYPE_ENUM_UINT8;


enum APS_L4A_EMC_IND_ENUM
{
    APS_L4A_PDP_NOT_FOR_EMC             = 0x00,
    APS_L4A_PDP_FOR_EMC                 = 0x01,

    APS_L4A_EMC_IND_BUTT
};
typedef VOS_UINT8 APS_L4A_EMC_IND_ENUM_UINT8;


enum APS_L4A_PCSCF_DISCOVERY_ENUM
{
    APS_L4A_PCSCF_DISCOVERY_NOT_INFLUENCED  = 0x00,
    APS_L4A_PCSCF_DISCOVERY_THROUGH_NAS_SIG = 0x01,
    APS_L4A_PCSCF_DISCOVERY_THROUGH_DHCP    = 0x02,

    APS_L4A_PCSCF_DISCOVERY_BUTT
};
typedef VOS_UINT8 APS_L4A_PCSCF_DISCOVERY_ENUM_UINT8;


enum APS_L4A_IMS_CN_SIG_FLAG_ENUM
{
    APS_L4A_PDP_NOT_FOR_IMS_CN_SIG_ONLY     = 0x00,
    APS_L4A_PDP_FOR_IMS_CN_SIG_ONLY         = 0x01,

    APS_L4A_IMS_CN_SIG_FLAG_BUTT
};
typedef VOS_UINT8 APS_L4A_IMS_CN_SIG_FLAG_ENUM_UINT8;


/* Modified by l60609 for L-C互操作项目, 2014-01-06, Begin */
/*****************************************************************************
 枚举名  : APS_L4A_PDN_REQUEST_TYPE_ENUM
 枚举说明: Table 10.5.173/3GPP TS24.008: Request type information element

 修改历史      :
  1.日    期   : 2014年01月06日
    作    者   : l60609
    修改内容   : 新增枚举
*****************************************************************************/
enum APS_L4A_PDN_REQUEST_TYPE_ENUM
{
    /* initial request: when the MS is establishing connectivity to an additional PDN for the first time */
    APS_L4A_PDN_REQUEST_TYPE_INITIAL     = 0x1,

    /* Handover: when the connectivity to a PDN is established upon handover from a non-3GPP access network
        and the MS was connected to that PDN before the handover to the 3GPP access network */
    APS_L4A_PDN_REQUEST_TYPE_HANDOVER    = 0x2,

    /* Unused. If received, the network shall interpret this as "initial request". */
    APS_L4A_PDN_REQUEST_TYPE_UNUSED      = 0x3,

    /* emergency: the network shall use the APN or the GGSN/PDN GW configured for emergency bearer services. */
    APS_L4A_PDN_REQUEST_TYPE_EMERGENCY   = 0x4,

    APS_L4A_PDN_REQUEST_TYPE_BUTT
};

typedef VOS_UINT8 APS_L4A_PDN_REQUEST_TYPE_ENUM_UINT8;

/* Modified by l60609 for L-C互操作项目, 2014-01-06, End */


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
 结构名    : APS_L4A_APPCTRL_STRU
 结构说明  : 为支持多通道并发，需要和L4A定义应用的控制头，用于指明是哪个client对应的回复
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulPid;                                  /* 针对AT或者DIAG的多应用 L4A发消息给该PID */
    VOS_UINT32                          ulClientId;                             /* 多AT窗口 */
    VOS_UINT32                          ulOpId;                                 /* 操作字，针对多次请求，每次请求加一,让用户结合 */
} APS_L4A_APPCTRL_STRU;
typedef struct
{
    APS_L4A_GW_AUTH_TYPE_ENUM_UINT32    enGwAuthType;
    VOS_UINT8                           ucAccNumLen;
    VOS_UINT8                           aucRsv1[3];
    VOS_UINT8                           auAccessNum[APS_L4A_MAX_ACCESS_NUM_LEN];/*此参数保留，暂时不使用*/
    VOS_UINT8                           ucUserNameLen;
    VOS_UINT8                           aucRsv2[3];
    VOS_UINT8                           aucUserName[APS_L4A_MAX_USERNAME_LEN];
    VOS_UINT8                           ucPwdLen;
    VOS_UINT8                           aucRsv3[1];
    VOS_UINT8                           aucPwd[APS_L4A_MAX_PASSWORD_LEN];
}APS_L4A_GW_AUTH_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucApnLen;
    VOS_UINT8                           aucApnName[APS_L4A_MAX_APN_LEN];
}APS_L4A_APN_INFO_STRU;


typedef struct
{
    APS_L4A_BEARER_MANAGE_MODE_ENUM_UINT8   enMode;                             /* 承载管理模式 */
    APS_L4A_BEARER_MANAGE_TYPE_ENUM_UINT8   enType;                             /* 承载管理类型 */
    VOS_UINT8                               aucReserved[2];                     /* 四字节对齐，保留 */
}APS_L4A_PDP_MANAGE_INFO_STRU;
typedef struct
{
    /* 0 QCI is selected by network
       [1 - 4]value range for guranteed bit rate Traffic Flows
       [5 - 9]value range for non-guarenteed bit rate Traffic Flows */
    VOS_UINT8                           ucQCI;

    VOS_UINT8                           ucReserved[3];

    /* DL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulDLGBR;

    /* UL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulULGBR;

    /* DL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulDLMBR;

    /* UL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulULMBR;

}APS_L4A_EPS_QOS_STRU;

/*****************************************************************************
 结构名    : APS_L4A_PDP_TFT_STRU
 结构说明  : PDP上下文的报文过滤器(TFT)的参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpRmtIpv4AddrAndMask        : 1;
    VOS_UINT32                          bitOpRmtIpv6AddrAndMask        : 1;
    VOS_UINT32                          bitOpProtocolId                : 1;
    VOS_UINT32                          bitOpSingleLocalPort           : 1;
    VOS_UINT32                          bitOpLocalPortRange            : 1;
    VOS_UINT32                          bitOpSingleRemotePort          : 1;
    VOS_UINT32                          bitOpRemotePortRange           : 1;
    VOS_UINT32                          bitOpSecuParaIndex             : 1;
    VOS_UINT32                          bitOpTypeOfService             : 1;
    VOS_UINT32                          bitOpFlowLabelType             : 1;
    VOS_UINT32                          bitOpSpare                     : 22;

    VOS_UINT8                           ucPacketFilterId;
    VOS_UINT8                           ucNwPacketFilterId;
    VOS_UINT8                           enDirection;
    VOS_UINT8                           ucPrecedence;             /*packet filter evaluation precedence*/

    VOS_UINT32                          ulSecuParaIndex;          /*SPI*/
    VOS_UINT16                          usSingleLcPort;
    VOS_UINT16                          usLcPortHighLimit;
    VOS_UINT16                          usLcPortLowLimit;
    VOS_UINT16                          usSingleRmtPort;
    VOS_UINT16                          usRmtPortHighLimit;
    VOS_UINT16                          usRmtPortLowLimit;
    VOS_UINT8                           ucProtocolId;             /*协议号*/
    VOS_UINT8                           ucTypeOfService;          /*TOS*/
    VOS_UINT8                           ucTypeOfServiceMask;      /*TOS Mask*/
    VOS_UINT8                           aucReserved[1];

    VOS_UINT8                           aucRmtIpv4AddreTAFss[APS_L4A_MAX_IPV4_ADDR_LEN];
                                                                  /*ucSourceIpAddress[0]为IP地址高字节位
                                                                    ucSourceIpAddress[3]为低字节位*/
    VOS_UINT8                           aucRmtIpv4Mask[APS_L4A_MAX_IPV4_ADDR_LEN];
                                                                  /*ucSourceIpMask[0]为IP地址高字节位 ,
                                                                    ucSourceIpMask[3]为低字节位*/
    VOS_UINT8                           aucRmtIpv6Address[APS_L4A_MAX_IPV6_ADDR_LEN];
                                                                  /*ucRmtIpv6Address[0]为IPv6接口标识高字节位
                                                                    ucRmtIpv6Address[7]为IPv6接口标识低字节位*/
    VOS_UINT8                           aucRmtIpv6Mask[APS_L4A_MAX_IPV6_ADDR_LEN];
                                                                  /*ucRmtIpv6Mask[0]为高字节位
                                                                    ucRmtIpv6Mask[7]为低字节位*/

    VOS_UINT32                          ulFlowLabelType;          /*FlowLabelType*/

}APS_L4A_PDP_TFT_STRU;

/*****************************************************************************
 结构名    : APS_L4A_PDP_DNS_STRU
 结构说明  : PDP中DNS的参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpPrimDnsAddr    : 1;
    VOS_UINT32                          bitOpSecDnsAddr     : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           aucPrimDnsAddr[APS_L4A_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecDnsAddr[APS_L4A_MAX_IPV4_ADDR_LEN];

}APS_L4A_PDP_DNS_STRU;


typedef struct
{
    VOS_UINT32                          bitOpPrimPcscfAddr  : 1;
    VOS_UINT32                          bitOpSecPcscfAddr   : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           aucPrimPcscfAddr[APS_L4A_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecPcscfAddr[APS_L4A_MAX_IPV4_ADDR_LEN];

} APS_L4A_PDP_PCSCF_STRU;


typedef struct
{
    VOS_UINT32                          bitOpGateWayAddr    : 1;
    VOS_UINT32                          bitOpSpare          : 31;

    VOS_UINT8                           aucGateWayAddr[APS_L4A_MAX_IPV4_ADDR_LEN];

} APS_L4A_PDP_GATEWAY_STRU;


typedef struct
{
    VOS_UINT32                          bitOpPrimDnsAddr    : 1;
    VOS_UINT32                          bitOpSecDnsAddr     : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           aucPrimDnsAddr[APS_L4A_MAX_IPV6_ADDR_LEN];
    VOS_UINT8                           aucSecDnsAddr[APS_L4A_MAX_IPV6_ADDR_LEN];

} APS_L4A_PDP_IPV6_DNS_STRU;


typedef struct
{
    VOS_UINT8                           ucPrefixLen;
    VOS_UINT8                           aucReserved[3];

    VOS_UINT8                           aucPrefix[APS_L4A_MAX_IPV6_ADDR_LEN];

} APS_L4A_PDP_IPV6_PREFIX_STRU;


/*****************************************************************************
 结构名    : APS_L4A_PDP_ADDR_STRU
 结构说明  : PDP地址
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    APS_L4A_PDP_TYPE_ENUM_UINT8         enPdpType;
    VOS_UINT8                           aucReserved[3];
    VOS_UINT8                           aucIpv4Addr[APS_L4A_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucIpv6Addr[APS_L4A_MAX_IPV6_ADDR_LEN];

} APS_L4A_PDP_ADDR_STRU;


/*****************************************************************************
 结构名    : APS_L4A_PDP_NBNS_STRU
 结构说明  : NBNS(WINS)地址
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpPrimNbnsAddr   : 1;
    VOS_UINT32                          bitOpSecNbnsAddr    : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           aucPrimNbnsAddr[APS_L4A_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecNbnsAddr[APS_L4A_MAX_IPV4_ADDR_LEN];

}APS_L4A_PDP_NBNS_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT32                          bitOpLinkCid        : 1;
    VOS_UINT32                          bitOpEpsQos         : 1;
    VOS_UINT32                          bitOpEsmCause       : 1;
    VOS_UINT32                          bitOpSpare          : 29;

    VOS_UINT16                          usClientId;                             /* 客户端ID     */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID     */
    VOS_UINT8                           ucCid;                                  /* CID          */
    VOS_UINT32                          ulLinkCid;                              /* Linked CID   */
    VOS_UINT32                          ulRabId;                                /* 承载ID       */
    VOS_UINT32                          ulEsmCause;                             /* ESM Cause    */
    VOS_UINT32                          ulDedicatedBearerFlg;                   /* 专用承载标识 */
    APS_L4A_PDP_ADDR_STRU               stIpAddrInfo;                           /* IP地址       */
    APS_L4A_EPS_QOS_STRU                stEpsQosInfo;                           /* EPS QOS      */
    APS_L4A_PDP_DNS_STRU                stDnsAddrInfo;                          /* DNS地址      */
    APS_L4A_PDP_IPV6_DNS_STRU           stIpv6DnsAddrInfo;                      /* IPv6 DNS地址 */
    APS_L4A_PDP_NBNS_STRU               stNbnsAddrInfo;                         /* NBNS地址     */
    APS_L4A_PDP_GATEWAY_STRU            stGateWayAddrInfo;                      /* 网关地址     */
    APS_L4A_PDP_PCSCF_STRU              stPcscfAddrInfo;                        /* P-CSCF地址   */

} APS_L4A_PDP_ACTIVATE_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT16                          usClientId;                             /* 客户端ID */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID */
    VOS_UINT8                           ucCid;                                  /* CID      */
    VOS_UINT32                          ulEsmCause;                             /* ESM Cause */

} APS_L4A_PDP_ACTIVATE_REJ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT32                          bitOpApn            : 1;
    VOS_UINT32                          bitOpLinkCid        : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT16                          usClientId;                             /* 客户端ID */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID */
    VOS_UINT8                           ucCid;                                  /* CID      */
    VOS_UINT32                          ulLinkCid;
    VOS_UINT8                           aucApn[APS_L4A_MAX_APN_LEN + 1];        /* APN      */
    APS_L4A_PDP_ADDR_STRU               stIpAddrInfo;                           /* IP地址  */

} APS_L4A_PDP_ACTIVATE_IND_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT32                          bitOpEpsQos         : 1;
    VOS_UINT32                          bitOpSpare          : 31;

    VOS_UINT16                          usClientId;                             /* 客户端ID */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID */
    VOS_UINT8                           ucCid;                                  /* CID      */
    VOS_UINT32                          ulRabId;                                /* 承载ID   */
    APS_L4A_EPS_QOS_STRU                stEpsQosInfo;                           /* EPS QOS  */

} APS_L4A_PDP_MODIFY_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT16                          usClientId;                             /* 客户端ID */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID */
    VOS_UINT8                           ucCid;                                  /* CID      */
    VOS_UINT32                          ulEsmCause;                             /* ESM Cause */

} APS_L4A_PDP_MODIFY_REJ_STRU;


typedef APS_L4A_PDP_MODIFY_CNF_STRU APS_L4A_PDP_MODIFY_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT32                          bitOpLinkCid        : 1;
    VOS_UINT32                          bitOpSpare          : 31;

    VOS_UINT16                          usClientId;                             /* 客户端ID   */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID   */
    VOS_UINT8                           ucCid;                                  /* CID        */
    VOS_UINT32                          ulLinkCid;                              /* Linked CID */
    APS_L4A_PDP_ADDR_STRU               stIpAddrInfo;                           /* IP地址    */

} APS_L4A_PDP_DEACTIVATE_CNF_STRU;

/*****************************************************************************
 结构名    : APS_L4A_PDP_DEACTIVATE_REJ_STRU
 结构说明  : ID_APS_L4A_PDP_DEACTIVATE_REJ消息结构体
 1.日    期   : 2011年12月29日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT32                          bitOpLinkCid        : 1;
    VOS_UINT32                          bitOpSpare          : 31;

    VOS_UINT16                          usClientId;                             /* 客户端ID   */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID   */
    VOS_UINT8                           ucCid;                                  /* CID        */
    VOS_UINT32                          ulLinkCid;                              /* Linked CID */
    VOS_UINT32                          ulEsmCause;                             /* 失败的原因值 */

} APS_L4A_PDP_DEACTIVATE_REJ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT32                          bitOpLinkCid        : 1;
    VOS_UINT32                          bitOpSpare          : 31;

    VOS_UINT16                          usClientId;                             /* 客户端ID   */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID   */
    VOS_UINT8                           ucCid;                                  /* CID        */
    VOS_UINT32                          ulLinkCid;                              /* Linked CID */
    APS_L4A_PDP_ADDR_STRU               stIpAddrInfo;                           /* IP地址    */
    VOS_UINT32                          ulEsmCause;                             /* ESM Cause  */

} APS_L4A_PDP_DEACTIVATE_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT16                          usClientId;                             /* 客户端ID   */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID   */
    VOS_UINT8                           ucCid;                                  /* CID      */
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_NDISCONN_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_CGDCONT_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_CGDSCONT_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_CGTFT_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT16                          usClientId;                             /* 客户端ID   */
    VOS_UINT8                           ucOpId;                                 /* 操作码ID   */
    VOS_UINT8                           aucReserved[1];
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_CGANS_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_CGAUTO_CNF_STRU;
typedef struct
{
    VOS_UINT32                          bitOpBearerId        : 1;               /* BearerId */
    VOS_UINT32                          bitOpApn             : 1;               /* aucApn */
    VOS_UINT32                          bitOpIpAddr          : 1;               /* aucIpaddr */
    VOS_UINT32                          bitOpSubMask         : 1;               /* aucSubnetMask */
    VOS_UINT32                          bitOpGwAddr          : 1;               /* aucGWAddr */
    VOS_UINT32                          bitOpDNSPrimAddr     : 1;               /* aucDNSPrimAddr */
    VOS_UINT32                          bitOpDNSSecAddr      : 1;               /* aucDNSSecAddr */
    VOS_UINT32                          bitOpPCSCFPrimAddr   : 1;               /* aucPCSCFPrimAddr */
    VOS_UINT32                          bitOpPCSCFSecAddr    : 1;               /* aucPCSCFSecAddr */
    VOS_UINT32                          bitOpImCnSignalFlg   : 1;               /* enImCnSignalFlg */
    VOS_UINT32                          bitOpPadding         : 22;

    VOS_UINT8                           ucPrimayCid;                            /* default EPS bearer context */
    VOS_UINT8                           ucBearerId;                             /* a numeric parameter which identifies the bearer */

    /* the IM CN subsystem-related signalling flag */
    APS_L4A_IMS_CN_SIG_FLAG_ENUM_UINT8  enImCnSignalFlg;

    VOS_UINT8                           aucReserved[1];

    /* Access Pointer Name */
    VOS_UINT8                           aucApn[APS_L4A_MAX_APN_LEN + 1];

    /* the IP Address of the MT */
    APS_L4A_PDP_ADDR_STRU               stPdpAddr;

    /* the subnet mask for the IP Address of the MT */
    APS_L4A_PDP_ADDR_STRU               stSubnetMask;

    /* the Gateway Address of the MT */
    APS_L4A_PDP_ADDR_STRU               stGWAddr;

    /* the IP Address of the primary DNS Server */
    APS_L4A_PDP_ADDR_STRU               stDNSPrimAddr;

    /* the IP address of the secondary DNS Server */
    APS_L4A_PDP_ADDR_STRU               stDNSSecAddr;

    /* the IP Address of the primary P-CSCF Server */
    APS_L4A_PDP_ADDR_STRU               stPCSCFPrimAddr;

    /* the IP Address of the secondary P-CSCF Server */
    APS_L4A_PDP_ADDR_STRU               stPCSCFSecAddr;

} APS_L4A_PRIM_PDP_DYNAMIC_EXT_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码   */
    VOS_UINT32                          ulCidNum;                               /* CID数量  */
    APS_L4A_PRIM_PDP_DYNAMIC_EXT_STRU   astPrimParaInfo[0];                     /* 缺省承载动态参数 */

} APS_L4A_SET_CGCONTRDP_CNF_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SEC_PDP_DYNAMIC_EXT_STRU
 结构说明  :
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucCid;

    /* default EPS bearer context */
    VOS_UINT8                           ucPrimaryCid;

    /* a numeric parameter which identifies the bearer */
    VOS_UINT8                           ucBearerId;

    VOS_UINT8                           ucReserved[1];

} APS_L4A_SEC_PDP_DYNAMIC_EXT_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码   */
    VOS_UINT32                          ulCidNum;                               /* CID数量  */
    APS_L4A_SEC_PDP_DYNAMIC_EXT_STRU    astSecParaInfo[0];                      /* 专用承载动态参数 */

} APS_L4A_SET_CGSCONTRDP_CNF_STRU;

/*****************************************************************************
 结构名    : APS_L4A_PDP_TFT_EXT_STRU
 结构说明  :
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpPktFilterId    : 1;                /* ucPacketFilterId */
    VOS_UINT32                          bitOpPrecedence     : 1;                /* ucPrecedence */
    VOS_UINT32                          bitOpSrcIp          : 1;                /* aucSourceIpAddr,Mask */
    VOS_UINT32                          bitOpProtocolId     : 1;                /* ucProtocolId */
    VOS_UINT32                          bitOpDestPortRange  : 1;                /* destination port range */
    VOS_UINT32                          bitOpSrcPortRange   : 1;                /* source port range */
    VOS_UINT32                          bitOpSpi            : 1;                /* ipsec security parameter index */
    VOS_UINT32                          bitOpTosMask        : 1;                /* type of service */
    VOS_UINT32                          bitOpFlowLable      : 1;                /* ulFlowLable */
    VOS_UINT32                          bitOpDirection      : 1;                /* Direction */
    VOS_UINT32                          bitOpNwPktFilterId  : 1;                /* NWPacketFltId */
    VOS_UINT32                          bitOpSpare          : 21;

    VOS_UINT8                           ucPacketFilterId;                       /* Packet Filter Id,value range from 1 to 16 */
    VOS_UINT8                           ucPrecedence;                           /* evaluation precedence index,The value range is from 0 to 255 */
    VOS_UINT8                           ucProtocolId;                           /* protocol number,value range from 0 to 255 */
    VOS_UINT8                           aucReserved;
    APS_L4A_PDP_ADDR_STRU               stSourceIpaddr;
    APS_L4A_PDP_ADDR_STRU               stSourceIpMask;
    VOS_UINT16                          usLowDestPort;                          /* value range from 0 to 65535 */
    VOS_UINT16                          usHighDestPort;                         /* value range from 0 to 65535 */
    VOS_UINT16                          usLowSourcePort;                        /* value range from 0 to 65535 */
    VOS_UINT16                          usHighSourcePort;                       /* value range from 0 to 65535 */
    VOS_UINT32                          ulSecuParaIndex;
    VOS_UINT8                           ucTypeOfService;                        /* value range from 0 to 255 */
    VOS_UINT8                           ucTypeOfServiceMask;                    /* value range from 0 to 255 */
    APS_L4A_TFT_FILTER_ENUM_UINT8       enTftFilterDirection;
    VOS_UINT8                           ucNwPktFilterId;                        /* value range from 1 to 16 */
    VOS_UINT32                          ulFlowLable;                            /* value range is from 00000 to FFFFF */
} APS_L4A_PDP_TFT_EXT_STRU;


/*****************************************************************************
 结构名    : APS_L4A_SDF_PF_TFT_STRU
 结构说明  :
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulPfNum;
    APS_L4A_PDP_TFT_EXT_STRU            astTftInfo[APS_L4A_MAX_SDF_PF_NUM];

} APS_L4A_SDF_PF_TFT_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码      */
    VOS_UINT32                          ulCidNum;                               /* CID数量     */
    APS_L4A_SDF_PF_TFT_STRU             astPfTftInfo[0];                        /* 动态TFT参数 */

} APS_L4A_SET_CGTFTRDP_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_CGEQOS_CNF_STRU;

/*****************************************************************************
 结构名    : APS_L4A_EPS_QOS_EXT_STRU
 结构说明  :
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpQCI            : 1;                /* QCI */
    VOS_UINT32                          bitOpDLGBR          : 1;                /* DLGBR */
    VOS_UINT32                          bitOpULGBR          : 1;                /* ULGBR */
    VOS_UINT32                          bitOpDLMBR          : 1;                /* DLMBR */
    VOS_UINT32                          bitOpULMBR          : 1;                /* ULMBR */
    VOS_UINT32                          bitOpSpare          : 27;

    VOS_UINT8                           ucCid;

    /* 0 QCI is selected by network
       [1 - 4]value range for guranteed bit rate Traffic Flows
       [5 - 9]value range for non-guarenteed bit rate Traffic Flows */
    VOS_UINT8                           ucQCI;

    VOS_UINT8                           ucReserved[2];

    /* DL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulDLGBR;

    /* UL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulULGBR;

    /* DL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulDLMBR;

    /* UL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulULMBR;

}APS_L4A_EPS_QOS_EXT_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码      */
    VOS_UINT32                          ulCidNum;                               /* CID数量     */
    APS_L4A_EPS_QOS_EXT_STRU            astEpsQosInfo[0];                       /* 动态EPS QOS */

} APS_L4A_SET_CGEQOSRDP_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_AUTHDATA_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT8                           ucRabId;
    VOS_UINT8                           ucIpv6PrefixNum;                        /* IPv6地址前缀数量 */
    VOS_UINT8                           aucReserved[2];
    APS_L4A_PDP_IPV6_PREFIX_STRU        astIpv6PrefixArray[APS_L4A_MAX_IPV6_PREFIX_NUM];

} APS_L4A_IPV6_INFO_NOTIFY_IND_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT8                           ucCid;
    VOS_UINT8                           aucReserved[3];
} APS_L4A_ABORT_REQ_STRU;


/*****************************************************************************
 结构名    : APS_L4A_APPCTRL_STRU
 结构说明  : 为支持多通道并发，需要和L4A定义应用的控制头，用于指明是哪个client对应的回复
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;

    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;
    VOS_UINT8                           aucReserved[1];
    VOS_UINT32                          ulErrorCode;

} APS_L4A_SET_CGACT_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;

    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;
    VOS_UINT8                           aucReserved[1];
    VOS_UINT32                          ulErrorCode;

} APS_L4A_SET_CGCMOD_CNF_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_CGACT_REQ_STRU
 结构说明  : AT命令发起CGACT激活请求是，发送消息给L4A
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    APS_L4A_PDP_STATE_ENUM_UINT32       enState;                                /* 0 - deactivated,1 - activated */
    VOS_UINT32                          aulCid[APS_L4A_LTE_MAX_CID+1];          /* cid:1~L4A_LTE_MAX_CID */
    VOS_UINT32                          aulLinkCid[APS_L4A_LTE_MAX_CID+1];      /* 承载对应LinkCid,不操作承载的LinkCid设为0xFF */
    VOS_UINT32                          aulBearerType[APS_L4A_LTE_MAX_CID+1];   /* 承载类型,不操作承载设为0xFF */

} APS_L4A_SET_CGACT_REQ_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_CGCMOD_REQ_STRU
 结构说明  : AT命令发起CGMOD PDP Modify操作，发送消息给L4A
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          aulCid[APS_L4A_LTE_MAX_CID+1];          /* cid:1~L4A_LTE_MAX_CID */

} APS_L4A_SET_CGCMOD_REQ_STRU;
typedef struct
{
    VOS_UINT32                          bitOpIpv4AddrAlloc  : 1;
    VOS_UINT32                          bitOpPcscfDiscovery : 1;
    VOS_UINT32                          bitOpImCnSignalFlg  : 1;
    VOS_UINT32                          bitOpSpare          : 29;

    VOS_UINT32                          ulCid;
    VOS_UINT16                          usAuthType;
    APS_L4A_PDP_TYPE_ENUM_UINT8         enPdpType;
    APS_L4A_PDN_REQUEST_TYPE_ENUM_UINT8 enPdpRequestType;
    VOS_UINT8                           ucAPNLen;
    VOS_UINT8                           aucAPN[APS_L4A_MAX_APN_LEN + 1];
    VOS_UINT8                           ucUserNameLen;
    VOS_UINT8                           aucUserName[APS_L4A_MAX_USERNAME_LEN + 1];
    VOS_UINT8                           ucPwdLen;
    VOS_UINT8                           aucPassword[APS_L4A_MAX_PASSWORD_LEN + 1];
    APS_L4A_EMC_IND_ENUM_UINT8          enEmergencyInd;
    APS_L4A_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT8 enIpv4AddrAlloc;
    APS_L4A_PCSCF_DISCOVERY_ENUM_UINT8  enPcscfDiscovery;
    APS_L4A_IMS_CN_SIG_FLAG_ENUM_UINT8  enImCnSignalFlg;
    VOS_UINT8                           aucRsv1[1];
}APS_L4A_NDISCONN_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_NDISCONN_REQ_STRU
 结构说明  : AT命令NDISDUP发起NDISCONN PDP激活操作，发送消息给L4A
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    APS_L4A_NDISCONN_STRU               stDialPara;

} APS_L4A_SET_NDISCONN_REQ_STRU;



typedef struct
{
    VOS_UINT32                          bitOpPdpType         : 1;               /* PdpType */
    VOS_UINT32                          bitOpApn             : 1;               /* aucApn */
    VOS_UINT32                          bitOpPdpAddr         : 1;               /* aucPdpAddr */
    VOS_UINT32                          bitOpPdpDcomp        : 1;               /* PdpDcomp */
    VOS_UINT32                          bitOpPdpHcomp        : 1;               /* PdpHcomp */
    VOS_UINT32                          bitOpIPV4ADDRALLOC   : 1;               /* IPV4ADDRALLOC */
    VOS_UINT32                          bitOpEmergency       : 1;               /* Emergency Indication */
    VOS_UINT32                          bitOpPcscfDiscovery  : 1;               /* P-CSCF discovery */
    VOS_UINT32                          bitOpImCnSignalFlg   : 1;               /* IM CN Signalling Flag */
    VOS_UINT32                          bitOpPadding         : 23;

    VOS_UINT8                           ucCid;
    VOS_UINT8                           aucApn[APS_L4A_MAX_APN_LEN + 1];
    VOS_UINT8                           aucReserved1[3];
    APS_L4A_PDP_TYPE_ENUM_UINT8         enPdpType;
    VOS_UINT8                           aucReserved2[3];
    APS_L4A_PDP_ADDR_STRU               stPdpAddr;
    APS_L4A_PDP_DATA_COMP_ENUM_UINT8    enPdpDataComp;
    APS_L4A_PDP_HEAD_COMP_ENUM_UINT8    enPdpHeadComp;
    VOS_UINT8                           ucIPV4ADDRALLOC;                        /* 0 - IPv4 Address Allocation through NAS Signaling (default if omitted)
                                                                                   1 - IPv4 Address Allocated through DHCP */
    APS_L4A_EMC_IND_ENUM_UINT8          enEmergency;                            /* 0 - PDP context is not for emergency bearer services
                                                                                   1 - PDP context is for emergency bearer services */
    APS_L4A_PCSCF_DISCOVERY_ENUM_UINT8  enPcscfDiscovery;                       /* 0 - Preference of P-CSCF address discovery not influenced by +CGDCONT */
                                                                                /* 1 - Preference of P-CSCF address discovery through NAS Signalling */
                                                                                /* 2 - Preference of P-CSCF address discovery through DHCP */

    APS_L4A_IMS_CN_SIG_FLAG_ENUM_UINT8  enImCnSignalFlg;                        /* 0 - UE indicates that the PDP context is not for IM CN subsystem-related signalling only */
                                                                                /* 1 - UE indicates that the PDP context is for IM CN subsystem-related signalling only */

    VOS_UINT8                           aucReserved3[2];
}APS_L4A_PRIM_PDP_EXT_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_CGDCONT_REQ_STRU
 结构说明  : AT命令CGDCONT发起设置主PDP上下文操作，发送消息给L4A
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    APS_L4A_PRIM_PDP_EXT_STRU           stCgdcontInfo;

}APS_L4A_SET_CGDCONT_REQ_STRU;


/*****************************************************************************
 结构名    : APS_L4A_SEC_PDP_EXT_STRU
 结构说明  : 辅PDP上下文信息
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpPrimaryCid      : 1;               /* ucPrimaryCid */
    VOS_UINT32                          bitOpPdpDcomp        : 1;               /* PdpDcomp */
    VOS_UINT32                          bitOpPdpHcomp        : 1;               /* PdpHcomp */
    VOS_UINT32                          bitOpPadding         : 29;

    VOS_UINT8                           ucSecondaryCid;                         /* dedicated EPS Bearer context */
    VOS_UINT8                           ucPrimaryCid;                           /* Default EPS Bearer context */
    APS_L4A_PDP_DATA_COMP_ENUM_UINT8    enPdpDataComp;
    APS_L4A_PDP_HEAD_COMP_ENUM_UINT8    enPdpHeadComp;

}APS_L4A_SEC_PDP_EXT_STRU;


/*****************************************************************************
 结构名    : APS_L4A_SET_CGDSCONT_REQ_STRU
 结构说明  : AT命令CGDSCONT发起设置辅PDP上下文操作，发送消息给L4A
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    APS_L4A_SEC_PDP_EXT_STRU            stCgdscontInfo;

}APS_L4A_SET_CGDSCONT_REQ_STRU;


/*****************************************************************************
 结构名    : APS_L4A_SET_CGTFT_REQ_STRU
 结构说明  : AT命令CGTFT发起设置PDP 上下文的报文过滤器操作，发送消息给L4A
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulCid;
    APS_L4A_PDP_TFT_EXT_STRU            stTftInfo;

}APS_L4A_SET_CGTFT_REQ_STRU;


/*****************************************************************************
 结构名    : APS_L4A_SET_CGANS_REQ_STRU
 结构说明  : AT命令CGANS,应答PDP激活请求
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulResponse;                             /* 0 reject the request; */
                                                                                /* 1 accept and request that the PDP context be activated */
    VOS_UINT8                           ucL2P[20];                              /* TODO */
    VOS_UINT8                           ucCid;
    VOS_UINT8                           aucRsv[3];
}APS_L4A_SET_CGANS_REQ_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_CGAUTO_REQ_STRU
 结构说明  : AT命令CGAUTO,设置自动应答模式
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    APS_L4A_AUTO_ANS_MODE_ENUM_UINT32   enAnsMode;
    VOS_UINT32                          ulTimeout;                              /* 用户回复IND的超时设定, ms为单位, 超过该时间未反馈则自动接受 */

}APS_L4A_SET_CGAUTO_REQ_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_CGPADDR_REQ_STRU
 结构说明  : AT命令CGPADDR,查询PDP地址
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          aulCid[APS_L4A_LTE_MAX_CID+1];          /* cid:1~L4A_LTE_MAX_CID */

}APS_L4A_SET_CGPADDR_REQ_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_CGCONTRDP_REQ_STRU
 结构说明  : AT命令CGCONTRDP,获取CID对应的主PDP动态参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT8                           aucCid[APS_L4A_LTE_MAX_CID + 1];                                  /* 0xff-if the parameter <cid> is omitted */

}APS_L4A_SET_CGCONTRDP_REQ_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_CGSCONTRDP_REQ_STRU
 结构说明  : AT命令CGSCONTRDP,获取CID对应的辅PDP动态参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT8                           aucCid[APS_L4A_LTE_MAX_CID + 1];                                  /* 0xff-if the parameter <cid> is omitted */

}APS_L4A_SET_CGSCONTRDP_REQ_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_CGTFTRDP_REQ_STRU
 结构说明  : AT命令CGTFTRDP,获取TFT动态参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT8                           aucCid[APS_L4A_LTE_MAX_CID + 1];                                  /* 0xff-if the parameter <cid> is omitted */

}APS_L4A_SET_CGTFTRDP_REQ_STRU;

/*****************************************************************************
 结构名    : APS_L4A_PS_CGEQOS_STRU
 结构说明  : EQOS的参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpQCI         : 1;                   /* ValQCI */
    VOS_UINT32                          bitOpDLGBR       : 1;                   /* ValDLGBR */
    VOS_UINT32                          bitOpULGBR       : 1;                   /* ValULGBR */
    VOS_UINT32                          bitOpDLMBR       : 1;                   /* ValDLMBR */
    VOS_UINT32                          bitOpULMBR       : 1;                   /* ValULMBR */
    VOS_UINT32                          bitOpPadding     : 27;

    VOS_UINT8                           ucCid;
    VOS_UINT8                           ucValQCI;                               /* 0 QCI is selected by network
                                                                                  [1 - 4]value range for guranteed bit rate Traffic Flows
                                                                                  [5 - 9]value range for non-guarenteed bit rate Traffic Flows */
    VOS_UINT8                           aucReserved[2];
    VOS_UINT32                          ulValDLGBR;                             /* DL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulValULGBR;                             /* UL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulValDLMBR;                             /* DL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulValULMBR;                             /* UL MBR in case of GBR QCI, The value is in kbit/s */

}APS_L4A_PS_CGEQOS_STRU;


/*****************************************************************************
 结构名    : APS_L4A_SET_CGEQOS_REQ_STRU
 结构说明  : AT命令CGEQOS,设置EQOS参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    APS_L4A_PS_CGEQOS_STRU              stCgeqosInfo;

}APS_L4A_SET_CGEQOS_REQ_STRU;



/*****************************************************************************
 结构名    : APS_L4A_SET_CGEQOSRDP_REQ_STRU
 结构说明  : AT命令CGEQOSRDP,获取EQOS参数参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT8                           aucCid[APS_L4A_LTE_MAX_CID + 1];

}APS_L4A_SET_CGEQOSRDP_REQ_STRU;

/*****************************************************************************
 结构名    : APS_L4A_PDP_AUTHDATA_STRU
 结构说明  : AUTH参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucCid;                                  /* 1~16 */
    VOS_UINT8                           ucDefined;                              /* 0:undefined,1:defined */
    VOS_UINT16                          usAuthType;                             /* 0,1,2,0xFF */
    VOS_UINT32                          ulPLMN;                                 /* 16进制存储 */
    VOS_UINT8                           ucPwdLen;
    VOS_UINT8                           aucReserved1[3];
    VOS_UINT8                           aucPassword[APS_L4A_AUTHDATA_MAX_PASSWORD_LEN + 1];
    VOS_UINT8                           ucUserNameLen;
    VOS_UINT8                           aucReserved2[3];
    VOS_UINT8                           aucUserName[APS_L4A_AUTHDATA_MAX_USERNAME_LEN + 1];

}APS_L4A_PDP_AUTHDATA_STRU;

/*****************************************************************************
 结构名    : APS_L4A_SET_AUTHDATA_REQ_STRU
 结构说明  : AT命令CGEQOSRDP,获取EQOS参数参数
 1.日    期   : 2011年12月12日
   作    者   : h44270
   修改内容   : Added for PS融合
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    APS_L4A_PDP_AUTHDATA_STRU           stAuthData;

}APS_L4A_SET_AUTHDATA_REQ_STRU;


typedef struct
{
    VOS_UINT32                          bitOpPdnType            : 1;
    VOS_UINT32                          bitOpApn                : 1;
    VOS_UINT32                          bitOpSdfQos             : 1;
    VOS_UINT32                          bitOpLinkdCId           : 1;
    VOS_UINT32                          bitIpv4AddrAllocType    : 1;
    VOS_UINT32                          bitOpGwAuthInfo         : 1;
    VOS_UINT32                          bitOpEmergencyInd       : 1;
    VOS_UINT32                          bitOpPcscfDiscovery     : 1;
    VOS_UINT32                          bitOpImsCnSignalFlag    : 1;
    VOS_UINT32                          bitOpSpare              : 23;


    APS_L4A_PDP_TYPE_ENUM_UINT8         enPdnType;
    APS_L4A_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT8 enIpv4AddrAllocType;
    APS_L4A_BEARER_TYPE_ENUM_UINT8      enBearerCntxtType;                      /* SM上下文对应承载的类型 */

    APS_L4A_EMC_IND_ENUM_UINT8          enEmergencyInd;
    APS_L4A_PCSCF_DISCOVERY_ENUM_UINT8  enPcscfDiscovery;
    APS_L4A_IMS_CN_SIG_FLAG_ENUM_UINT8  enImsCnSignalFlag;
    VOS_UINT8                           aucReserved[2];

    VOS_UINT32                          ulCid;                                  /* 承载上下文ID */
    VOS_UINT32                          ulLinkdCid;                             /* 关联CID */

    VOS_UINT32                          ulPfNum;
    APS_L4A_EPS_QOS_STRU                stSdfQosInfo;
    APS_L4A_APN_INFO_STRU               stApnInfo;
    APS_L4A_GW_AUTH_INFO_STRU           stGwAuthInfo;
    APS_L4A_PDP_TFT_STRU                astCntxtTftInfo[APS_L4A_MAX_SDF_PF_NUM];

}APS_L4A_SDF_PARA_STRU;

typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;

}APS_L4A_GET_CEMODE_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;

    VOS_UINT32                          ulCurrentUeMode;                        /*当前UE模式*/
    VOS_UINT32                          ulSupportModeCnt;                       /*UE能够支持的模式的个数*/
    VOS_UINT32                          aulSupportModeList[4];                  /*UE能够支持的模式*/
} APS_L4A_GET_CEMODE_CNF_STRU;

typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;

}APS_L4A_GET_LTE_CS_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

    VOS_UINT8                             ucSmsOverSGs;
    VOS_UINT8                             ucSmsOverIMS;
    VOS_UINT8                             ucCSFB;
    VOS_UINT8                             ucSrVcc;
    VOS_UINT8                             ucVoLGA;
    VOS_UINT8                             aucRsv[3];
} APS_L4A_GET_LTE_CS_CNF_STRU;

typedef struct
{
    VOS_UINT32                          bitOpPdpType        : 1;                /* PdpType */
    VOS_UINT32                          bitOpApn            : 1;                /* aucApn */
    VOS_UINT32                          bitOpUsername       : 1;                /* AuthUsername */
    VOS_UINT32                          bitOpPassword       : 1;                /* AuthPassword */
    VOS_UINT32                          bitOpAuthType       : 1;                /* AuthType */
    VOS_UINT32                          bitOpSpare          : 27;

    VOS_UINT8                           ucDefined;                              /* 0:undefined, 1:defined */

    VOS_UINT8                           ucCid;

    /* 1 IP Internet Protocol (IETF STD 5) */
    /* 2 IPV6 Internet Protocol, version 6 (IETF RFC 2460) */
    /* 3 IPV4V6 Virtual <PDP_type> introduced to handle dual IP stack UE capability. (See 3GPP TS 24.301 [83]) */
    APS_L4A_PDP_TYPE_ENUM_UINT8             enPdpType;

    VOS_UINT8                           aucReserved1[1];

    /* Access Point Name */
    VOS_UINT8                           aucApn[APS_L4A_MAX_APN_LEN + 1];

    VOS_UINT8                           aucPassWord[APS_L4A_AUTHDATA_MAX_USERNAME_LEN + 1];
    VOS_UINT8                           ucPwdLen;
    VOS_UINT8                           aucReserved2[3];
    VOS_UINT8                           aucUserName[APS_L4A_AUTHDATA_MAX_PASSWORD_LEN + 1];
    VOS_UINT8                           ucUserNameLen;
    VOS_UINT8                           ucAuthType;
    VOS_UINT8                           aucReserved3[2];

}APS_L4A_PDP_PDPROFMOD_STRU;

typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    APS_L4A_PDP_PDPROFMOD_STRU          stPdprofmod;

}APS_L4A_SET_PDPROFMOD_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulErrorCode;                            /* 错误码     */

} APS_L4A_SET_PDPROFMOD_CNF_STRU;
typedef struct
{
    VOS_UINT32                          ulChallengeLen;
    VOS_UINT8                           aucChallenge[APS_L4A_PPP_CHAP_CHALLNGE_LEN];
    VOS_UINT32                          ulChallengeNameLen;
    VOS_UINT8                           aucChallengeName[APS_L4A_PPP_CHAP_CHALLNGE_NAME_LEN];
    VOS_UINT32                          ulResponseLen;
    VOS_UINT8                           aucResponse[APS_L4A_PPP_CHAP_RESPONSE_LEN];
    VOS_UINT8                           ucResponseNameLen;
    VOS_UINT8                           aucResponseName[APS_L4A_PPP_MAX_USERNAME_LEN];

} APS_L4A_CHAP_PARAM_STRU;


typedef struct
{
    VOS_UINT8                           ucUsernameLen;
    VOS_UINT8                           aucUsername[APS_L4A_PPP_MAX_USERNAME_LEN];
    VOS_UINT8                           ucPasswordLen;
    VOS_UINT8                           aucPassword[APS_L4A_PPP_MAX_PASSWORD_LEN];

} APS_L4A_PAP_PARAM_STRU;


typedef struct
{
    APS_L4A_AUTH_TYPE_ENUM_UINT8        enAuthType;                             /* _H2ASN_IeChoice_Export APS_L4A_AUTH_TYPE_ENUM_UINT8 */
    VOS_UINT8                           ucAuthId;
    VOS_UINT8                           aucRsv[2];
    union                                                                       /* _H2ASN_Skip */
    {/* _H2ASN_Skip */
        APS_L4A_CHAP_PARAM_STRU         stChapParam;                            /* _H2ASN_Skip */
        APS_L4A_PAP_PARAM_STRU          stPapParam;
        /***********************************************************************
            _H2ASN_IeChoice_When                    APS_L4A_AUTH_TYPE_ENUM_UINT8
        ***********************************************************************/
    } unAuthParam;/* _H2ASN_Skip */

} APS_L4A_AUTH_INFO_STRU;
typedef struct
{
    VOS_UINT32                          bitOpIpAddr         :1;
    VOS_UINT32                          bitOpPrimDns        :1;
    VOS_UINT32                          bitOpSecDns         :1;
    VOS_UINT32                          bitOpPrimNbns       :1;
    VOS_UINT32                          bitOpSecNbns        :1;
    VOS_UINT32                          bitOpSpare          :27;

    VOS_UINT8                           aucIpAddr[APS_L4A_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucPrimDns[APS_L4A_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecDns[APS_L4A_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucPrimNbns[APS_L4A_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecNbns[APS_L4A_MAX_IPV4_ADDR_LEN];

} APS_L4A_IPCP_INFO_STRU;


typedef struct
{
    APS_L4A_AUTH_INFO_STRU              stAuthInfo;
    APS_L4A_IPCP_INFO_STRU              stIpcpInfo;

} APS_L4A_PPP_INFO_STRU;


typedef struct
{
    VOS_MSG_HEADER                                          /* OSA Header */    /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;            /* Msg ID     */    /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stAppCtrl;          /* Client infomation */
    VOS_UINT32                          ulCid;              /* CID               */
    APS_L4A_PPP_INFO_STRU               stPppInfo;          /* PPP information   */

} APS_L4A_PPP_DIAL_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                          /* OSA Header */    /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;            /* Msg ID     */    /* _H2ASN_Skip */

    VOS_UINT16                          usClientId;         /* Client ID    */
    VOS_UINT8                           ucOpId;             /* Operation ID */
    VOS_UINT8                           ucCid;              /* CID          */
    VOS_UINT32                          ulErrorCode;        /* Error Code   */

} APS_L4A_PPP_DIAL_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    APS_L4A_APPCTRL_STRU                stCtrl;
    VOS_UINT32                          ulCid;

} APS_L4A_PS_CALL_END_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_Skip */

    VOS_UINT16                          usClientId;                             /* Client ID   */
    VOS_UINT8                           ucOpId;                                 /* Operation ID*/
    VOS_UINT8                           ucCid;                                  /* CID         */
    VOS_UINT32                          ulErrorCode;                            /* Error Code  */

} APS_L4A_PS_CALL_END_CNF_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    APS_L4A_MSG_ID_ENUM_UINT32          enMsgId;                                /* _H2ASN_MsgChoice_Export APS_L4A_MSG_ID_ENUM_UINT32 */
    VOS_UINT8                           aucMsg[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          APS_L4A_MSG_ID_ENUM_UINT32
    ****************************************************************************/
} APS_L4A_INTERFACE_MSG_DATA;
/* _H2ASN_Length UINT32 */

typedef struct
{
    VOS_MSG_HEADER
    APS_L4A_INTERFACE_MSG_DATA          stMsgData;

} ApsL4aInterface_MSG;


/*****************************************************************************
  10 函数声明
*****************************************************************************/

/*****************************************************************************
 函 数 名  : TAF_APS_GetCidSdfParaInfo
 功能描述  : 获取CID对应的SDF参数, 由APS保证获取的SDF参数有效, 如果CID对应的
             SDF参数无效, 返回VOS_ERR, 否则返回VOS_OK
 输入参数  : ucCid                      - CID
 输出参数  : pstSdfParaInfo             - CID对应的SDF参数
 返 回 值  : VOS_OK                     - CID对应的SDF参数有效, 获取成功
             VOS_ERR                    - CID对应的SDF参数无效, 获取失败
 调用函数  :
 被调函数  :
*****************************************************************************/
extern VOS_UINT32 TAF_APS_GetCidSdfParaInfo(
    VOS_UINT8                           ucCid,
    APS_L4A_SDF_PARA_STRU              *pstSdfParaInfo
);

/*****************************************************************************
 函 数 名  : TAF_APS_GetPdpManageInfo
 功能描述  : 获取PDP管理模式(自动应答/拒绝), 无失败场景
 输入参数  : 无
 输出参数  : pstPdpManageInfo           - PDP管理模式
 返 回 值  : VOS_OK                     - 获取PDP管理模式成功
             VOS_ERR                    - 获取PDP管理模式失败
 调用函数  :
 被调函数  :
*****************************************************************************/
extern VOS_UINT32 TAF_APS_GetPdpManageInfo(
    APS_L4A_PDP_MANAGE_INFO_STRU       *pstPdpManageInfo
);



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
