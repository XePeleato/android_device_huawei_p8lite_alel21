

#ifndef __CFG80211_STRU_H__
#define __CFG80211_STRU_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "common.h"
#include <linux/netlink.h>

/*****************************************************************************/
/* Data Types                                                                */
/*****************************************************************************/

#pragma pack(1)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 上报的消息类型I,R,W eg. */
#define MSG_TYPE_REPORT_STATUS   'I'

#define AP_TO_STA_MAX_NUM       8

#ifdef MAC_802_11W
#define NETLINK_PMF (NETLINK_HISI_WIFI_PMF)   /* 此数值不能和linux已有定义冲突 */
#endif

#define HISI_WIFI_DBB_LEN               256

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* HOST侧下发命令类型定义 */
enum HOST_CMD_TYPE_ENUM
{
    HOST_CMD_SCAN       = 0,            /* 扫描请求命令 */
    HOST_CMD_CONNECT    = 1,            /* 关联请求命令 */
    HOST_CMD_DISCONNECT = 2,            /* 去关联请求命令 */
    HOST_CMD_STATS      = 3,            /* 查询请求命令 */
    HOST_CMD_MODE       = 4,            /* 模式切换命令 */
    HOST_CMD_CONFIG     = 5,            /* 配置命令 */
    HOST_CMD_MGMT_FRAME = 6,            /* 下发管理帧命令 */
    HOST_CMD_LISTEN     = 7,            /* 下发监听命令 */
    HOST_CMD_INIT       = 8,            /* 上电初始化配置 */
    HOST_CMD_CANCEL_LISTEN = 9,         /*  取消device监听状态 */
#ifdef __ROAM__
    HOST_CMD_ROAM       = 10,
#endif
#ifdef MAC_802_11W
    HOST_CMD_PMF        = 11,            /* PMF配置命令 */
#endif
    HOST_CMD_INVALID       ,            /* 无效命令,用于执行完命令后清空 */

    HOST_CMD_BUTT
};

/* HOST侧下发命令类型定义 */
enum DEVICE_STATUS_TYPE_ENUM
{
    DEVICE_STATUS_NET_INFO          = 0,    /* 扫描AP状态上报 */
    DEVICE_STATUS_SCAN_COMP         = 1,    /* 扫描结束上报 */
    DEVICE_STATUS_ASOC_COMP         = 2,    /* 关联结果上报 */
    DEVICE_STATUS_DISCONNECT        = 3,    /* DISCONNECT上报 */
    DEVICE_STATUS_STATS             = 4,    /* 统计查询上报 */
    DEVICE_STATUS_CONNECT           = 5,    /* AP模式下STA成功关联上AP上报 */
    DEVICE_STATUS_MODE              = 6,    /* 模式切换后状态上报 */
    DEVICE_STATUS_CONFIG            = 7,    /* 配置命令状态上报 */
    DEVICE_STATUS_FRAME             = 8,    /* 上报管理帧：0-Probe request 1-P2P action*/
    DEVICE_STATUS_INIT              = 9,    /* 上电初始化状态上报 */
    DEVICE_STATUS_READY             = 10,   /* Device准备完毕，等待启动加载 */
    DEVICE_STATUS_CURRENT           = 11,   /* 当前状态查询上报 */
#ifdef MAC_802_11W
    DEVICE_STATUS_PMF               = 12,   /* 上报加密管理帧: 0-Disassociation 1-Deauthentication 2-SA Query Response */
#endif

#ifdef __ROAM__
    DEVICE_STATUS_ROAM              = 13,
#endif
    DEVICE_STATUS_SPEC_BA_SESSION   = 14,   /* 上报指定mac地址的ba会话情况 */
    DEVICE_STATUS_ALL_BA_SESSION    = 15,   /* 上报所有mac地址的ba会话情况 */

    DEVICE_STATUS_TKIP_MIC_FAILURE  = 16,   /* MIC Failure report 上报 */
    DEVICE_EC212_REPORT             = 17,

    DEVICE_STATUS_BUTT
};

/* 上报管理帧子类型定义 */
enum DEVICE_FRAME_TYPE_ENUM
{
    DEVICE_FRAME_PROBE_REQUEST  = 0,    /* Probe request帧 */
    DEVICE_FRAME_ACTION         = 1,    /* P2P action帧 */
    DEVICE_FRAME_ASSOC_REQ      = 2,    /* Assoc Req frame */
    DEVICE_FRAME_REASSOC_REQ    = 3,    /* ReAssoc Req frame */

    DEVICE_FRAME_BUTT
};

/* 关联结果类型定义 */
enum ASOC_STATUS_TYPE_ENUM
{
    ASOC_STATUS_SUCC        = 0,    /* 关联成功 */
    ASOC_STATUS_JOIN_FAILED = 1,    /* JOIN失败 */
    ASOC_STATUS_AUTH_FAILED = 2,    /* AUTH失败 */
    ASOC_STATUS_ASOC_FAILED = 3,    /* ASOC失败 */

    ASOC_STATUS_BUTT
};
#define JOIN_FAILED_STATUS_0    0 /* JOIN失败状态码 */
#define JOIN_FAILED_STATUS_1    1 /* JOIN失败状态码 */

/* WLAN Identifiers */
typedef enum {
    WID_NIL                            = -1,
    /* EMAC Character WID list */
    WID_BSS_TYPE                       = 0x0000,
    WID_CURRENT_TX_RATE                = 0x0001,
    WID_PRIMARY_CHANNEL                = 0x0002,
    WID_PREAMBLE                       = 0x0003,
    WID_11G_OPERATING_MODE             = 0x0004,
    WID_STATUS                         = 0x0005,
    WID_SCAN_TYPE                      = 0x0007,
    WID_PRIVACY_INVOKED                = 0x0008,
    WID_KEY_ID                         = 0x0009,
    WID_QOS_ENABLE                     = 0x000A,
    WID_POWER_MANAGEMENT               = 0x000B,
    WID_11I_MODE                    = 0x000C,
    WID_AUTH_TYPE                      = 0x000D,
    WID_SITE_SURVEY                    = 0x000E,
    WID_LISTEN_INTERVAL                = 0x000F,
    WID_FILTER_GROUP_DATA              = 0x00E3,
    WID_HI1101_PS_SWITCH               = 0x00E4,
    WID_DTIM_PERIOD                    = 0x0010,
    WID_ACK_POLICY                     = 0x0011,
    WID_RESET                          = 0x0012,
    WID_BCAST_SSID                     = 0x0015,
    WID_DEL_TYPE                       = 0x0016,
    WID_READ_ADDR_SDRAM                = 0x0017,
    WID_TX_POWER_LEVEL_11A             = 0x0018,
    WID_REKEY_POLICY                   = 0x0019,
    WID_SHORT_SLOT_ALLOWED             = 0x001A,
    WID_PHY_ACTIVE_REG                 = 0x001B,
    WID_TX_POWER_LEVEL_11B             = 0x001D,
    WID_START_SCAN_REQ                 = 0x001E,
    WID_RSSI                           = 0x001F,
    WID_JOIN_REQ                       = 0x0020,
    WID_USER_CONTROL_ON_TX_POWER       = 0x0027,
    WID_MEMORY_ACCESS_8BIT             = 0x0029,
    WID_UAPSD_SUPPORT_AP               = 0x002A,
    WID_CURRENT_MAC_STATUS             = 0x0031,
    WID_AUTO_RX_SENSITIVITY            = 0x0032,
    WID_DATAFLOW_CONTROL               = 0x0033,
    WID_SCAN_FILTER                    = 0x0036,
    WID_LINK_LOSS_THRESHOLD            = 0x0037,
    WID_AUTORATE_TYPE                  = 0x0038,
    WID_802_11H_DFS_MODE               = 0x003B,
    WID_802_11H_TPC_MODE               = 0x003C,
    WID_WPS_ENABLE                     = 0x0041,
    WID_WPS_START                      = 0x0043,
    WID_WPS_DEV_MODE                   = 0x0044,
    WID_OOB_RESET_REQ                  = 0x0046,
    WID_ENABLE_INT_SUPP                = 0x0047,
    WID_DEVICE_MODE                    = 0x0048,
    WID_WAPI_MODE                      = 0x0060,                      /*char*/
    WID_PS_MODE                        = 0x0070,
    WID_PS_ENABLE                      = 0x0071,


    /* NMAC Character WID list */
    WID_11N_PROT_MECH                  = 0x0080,
    WID_11N_ERP_PROT_TYPE              = 0x0081,
    WID_11N_ENABLE                     = 0x0082,
    WID_11N_OPERATING_TYPE             = 0x0083,
    WID_11N_OBSS_NONHT_DETECTION       = 0x0084,
    WID_11N_HT_PROT_TYPE               = 0x0085,
    WID_11N_RIFS_PROT_ENABLE           = 0x0086,
    WID_11N_SMPS_MODE                  = 0x0087,
    WID_11N_CURRENT_TX_MCS             = 0x0088,
    WID_11N_PRINT_STATS                = 0x0089,
    WID_HUT_FCS_CORRUPT_MODE           = 0x008A,
    WID_HUT_RESTART                    = 0x008B,
    WID_HUT_TX_FORMAT                  = 0x008C,
    WID_11N_SHORT_GI_ENABLE            = 0x008D,
    WID_HUT_BANDWIDTH                  = 0x008E,
    WID_HUT_OP_BAND                    = 0x008F,
    WID_HUT_STBC                       = 0x0090,
    WID_HUT_ESS                        = 0x0091,
    WID_HUT_ANTSET                     = 0x0092,
    WID_HUT_HT_OP_MODE                 = 0x0093,
    WID_RIFS_MODE                      = 0x0094,
    WID_HUT_SMOOTHING_REC              = 0x0095,
    WID_HUT_SOUNDING_PKT               = 0x0096,
    WID_HUT_HT_CODING                  = 0x0097,
    WID_HUT_TEST_DIR                   = 0x0098,
    WID_HUT_PHY_TEST_MODE              = 0x009A,
    WID_HUT_PHY_TEST_RATE_HI           = 0x009B,
    WID_HUT_PHY_TEST_RATE_LO           = 0x009C,
    WID_HUT_DISABLE_RXQ_REPLENISH      = 0x009D,
    WID_HUT_KEY_ORIGIN                 = 0x009E,
    WID_HUT_BCST_PERCENT               = 0x009F,
    WID_HUT_GROUP_CIPHER_TYPE          = 0x00A0,
    WID_TX_ABORT_CONFIG                = 0x00A1,
    WID_HOST_DATA_IF_TYPE              = 0x00A2,
    WID_HOST_CONFIG_IF_TYPE            = 0x00A3,
    WID_HUT_TSF_TEST_MODE              = 0x00A4,
    WID_HUT_PKT_TSSI_VALUE             = 0x00A5,
    WID_REG_TSSI_11B_VALUE             = 0x00A6,
    WID_REG_TSSI_11G_VALUE             = 0x00A7,
    WID_REG_TSSI_11N_VALUE             = 0x00A8,
    WID_TX_CALIBRATION                 = 0x00A9,
    WID_DSCR_TSSI_11B_VALUE            = 0x00AA,
    WID_DSCR_TSSI_11G_VALUE            = 0x00AB,
    WID_DSCR_TSSI_11N_VALUE            = 0x00AC,
    WID_HUT_RSSI_EX                    = 0x00AD,
    WID_HUT_ADJ_RSSI_EX                = 0x00AE,
    WID_11N_IMMEDIATE_BA_ENABLED       = 0x00AF,
    WID_11N_TXOP_PROT_DISABLE          = 0x00B0,
    WID_TX_POWER_LEVEL_11N             = 0x00B1,
    WID_2040_COEXISTENCE               = 0x00C1,
    WID_HUT_FC_TXOP_MOD                = 0x00C2,
    WID_HUT_FC_PROT_TYPE               = 0x00C3,
    WID_HUT_SEC_CCA_ASSERT             = 0x00C4,
    WID_2040_ENABLE                    = 0x00C5,
    WID_2040_40MHZ_INTOLERANT          = 0x00C7,
    WID_11N_CURRENT_TX_BW              = 0x00C8,
    WID_TX_POWER_LEVEL_11N_40MHZ       = 0x00C9,

   /* TPC char WID list */
   WID_OLTPC_SWITCH                    = 0x00D1,
   WID_OLTPC_UPPOWERCON                = 0x00D2,
   WID_OLTPC_ACTIVE                    = 0x00D3,
   WID_OLTPC_RFCHLOSS                  = 0x00D4,
   WID_CURRENT_TX_POW                  = 0x00D7,
    /* arp offload */
   WID_ARP_OFFLOAD_ENABLE              = 0x00E1,

   WID_KEEP_ALIVE_ENABLE               = 0x00E2,

    /* Custom Character WID list */
    WID_P2P_ENABLE                     = 0x0201,
    WID_P2P_DISCOVERABLE               = 0x0202,
    WID_P2P_LISTEN_CHAN                = 0x0203,
    WID_P2P_FIND_TO                    = 0x0204,
    WID_P2P_GO_INT_VAL                 = 0x0205,
    WID_P2P_PERSIST_GRP                = 0x0206,
    WID_P2P_AUTO_GO                    = 0x0207,
    WID_P2P_INTRA_BSS                  = 0x0208,
    WID_P2P_CT_WINDOW                  = 0x0209,

    /* DFX Character WID list */
    /* OTA */
    WID_DFX_MAC_STATICS                = 0x0300,  /* 打印统计 */
    WID_DFX_OTA_ON                     = 0x0301,  /* 打开OTA */
    WID_DFX_OTA_OFF                    = 0x0302,  /* 关闭OTA */
    WID_DFX_FUNC_TRACE                 = 0x0303,  /* 函数轨迹跟踪 */

    WID_DFX_RUNTC                      = 0x0311, /* run test case */
    WID_DFX_STATUS                     = 0x0312, /* 状态 */
    WID_DFX_HWAR                       = 0x0313, /* 硬件自动速率调整 */

    WID_11B_SCALE                       = 0x0314, /* 11B scale */
    WID_11A_SCALE                       = 0x0315, /* 11A scale */

    WID_AP_INDEX                        = 0x0316, /* get AP index */

    /* DFS  Character WID list */
    WID_DFS_CHAN_SWITCH_MODE            = 0x0321, /* DFS 信道切换模式 */
    WID_DFS_CHAN_SWITCH_CNT             = 0x0322, /* DFS 信道切换计数 */
    WID_DFS_RADAR_STANDARD              = 0x0323, /* 雷达检测标准 */

    WID_HOST_CMD                        = 0x0400,   /* 无线配置下发命令WID */
    WID_HOST_RST                        = 0x0401,   /* 复位WID */
    WID_CHAR_STATS                      = 0x0402,   /* 统计查询命令 */
    WID_CHAR_HOST_IF                    = 0x0403,   /* */
    WID_MODE_CHANGE                     = 0x0404,   /* 模式切换命令 */
    WID_START_AP_NETWORK                = 0x0405,   /* AP完成ADD Beacon帧后建立网络使能 */
    WID_MAX_NUM_STA                     = 0x0407,   /* 配置STA的最大用户数 */
    WID_CHAR_RESERVE                    = 0x0408,   /* 配置40M short gi */
    WID_AMSDU_LEN_FIX                   = 0x0409,   /* 加密AMSDU长度不够问题 */
    WID_CHAR_RESERVE02                  = 0x040A,   /* 预留CHAR类型WID接口 */

    WID_11I_ENCYPT_MODE                 = 0x0420,
    WID_11I_PAIRWISE_CIPHER_MODE        = 0x0421,
    WID_11I_GROUP_CIPHER_MODE           = 0x0422,
#ifdef MAC_802_11W
    WID_PMF_ACTIVATED                   = 0x0423,
    WID_UNPMF_ALLOWED                   = 0x0424,
#endif
    WID_BA_CONTROL_SET                  = 0x0440,

    WID_DFX_LOG_ON                      = 0x0500, /* log swtich: 开启日志 */
    WID_DFX_LOG_OFF                     = 0x0501, /* log swtich: 关闭日志 */

    /* EMAC Short WID list */
    WID_RTS_THRESHOLD                  = 0x1000,
    WID_FRAG_THRESHOLD                 = 0x1001,
    WID_SHORT_RETRY_LIMIT              = 0x1002,
    WID_LONG_RETRY_LIMIT               = 0x1003,
    WID_BEACON_INTERVAL                = 0x1006,
    WID_MEMORY_ACCESS_16BIT            = 0x1008,
    WID_RX_SENSE                       = 0x100B,
    WID_ACTIVE_SCAN_TIME               = 0x100C,
    WID_PASSIVE_SCAN_TIME              = 0x100D,
    WID_SITE_SURVEY_SCAN_TIME          = 0x100E,
    WID_JOIN_START_TIMEOUT             = 0x100F,
    WID_AUTH_TIMEOUT                   = 0x1010,
    WID_ASOC_TIMEOUT                   = 0x1011,
    WID_11I_PROTOCOL_TIMEOUT           = 0x1012,
    WID_EAPOL_RESPONSE_TIMEOUT         = 0x1013,
    WID_WPS_PASS_ID                    = 0x1017,
    WID_WPS_CONFIG_METHOD              = 0x1018,

    WID_USER_PREF_CHANNEL              = 0x1020,
    WID_CURR_OPER_CHANNEL              = 0x1021,

    WID_PA_GAIN                        = 0x1072, /* PA Gain Scale */
    WID_UPC_GAIN                       = 0x1073, /* UPC Gain Scale */

    WID_UPC_STATUS                     = 0x1074, /* CHECK UPC */
    WID_SHORT_RESERVE                  = 0x1075, /* 预留SHORT类型WID接口 */


    /* NMAC Short WID list */
    WID_HUT_FRAME_LEN                  = 0x1081,
    WID_HUT_TXOP_LIMIT                 = 0x1082,
    WID_HUT_SIG_QUAL_AVG               = 0x1083,
    WID_HUT_SIG_QUAL_AVG_CNT           = 0x1084,
    WID_11N_SIG_QUAL_VAL               = 0x1085,
    WID_HUT_RSSI_EX_COUNT              = 0x1086,
    WID_CCA_THRESHOLD                  = 0x1087,

    /* Custom Short WID list */
    WID_OLTPC_POWER_MIN                = 0x1090,
    WID_OLTPC_ESTIMATE_PERIOD          = 0x1091,

    WID_FLCTL_MEM_THRESHOLD_UP         = 0x1092,
    WID_FLCTL_MEM_THRESHOLD_DOWN       = 0x1093,
    WID_CALI_MASK_CTRL                 = 0x1094,

    /* Lte coex set bit */
    WID_LTE_COEX_ENABLE                = 0x1095,

    /* EMAC Integer WID list */
    WID_FAILED_COUNT                   = 0x2000,
    WID_RETRY_COUNT                    = 0x2001,
    WID_MULTIPLE_RETRY_COUNT           = 0x2002,
    WID_FRAME_DUPLICATE_COUNT          = 0x2003,
    WID_ACK_FAILURE_COUNT              = 0x2004,
    WID_RECEIVED_FRAGMENT_COUNT        = 0x2005,
    WID_MCAST_RECEIVED_FRAME_COUNT     = 0x2006,
    WID_FCS_ERROR_COUNT                = 0x2007,
    WID_SUCCESS_FRAME_COUNT            = 0x2008,
    WID_HUT_TX_COUNT                   = 0x200A,
    WID_TX_FRAGMENT_COUNT              = 0x200B,
    WID_TX_MULTICAST_FRAME_COUNT       = 0x200C,
    WID_RTS_SUCCESS_COUNT              = 0x200D,
    WID_RTS_FAILURE_COUNT              = 0x200E,
    WID_WEP_UNDECRYPTABLE_COUNT        = 0x200F,
    WID_REKEY_PERIOD                   = 0x2010,
    WID_REKEY_PACKET_COUNT             = 0x2011,
    WID_1X_SERV_ADDR                   = 0x2012,
    WID_STACK_IP_ADDR                  = 0x2013,
    WID_STACK_NETMASK_ADDR             = 0x2014,
    WID_HW_RX_COUNT                    = 0x2015,
    WID_MEMORY_ADDRESS                 = 0x201E,
    WID_MEMORY_ACCESS_32BIT            = 0x201F,
    WID_RF_REG_VAL                     = 0x2021,
    WID_DEV_OS_VERSION                 = 0x2025,
    WID_CHANNEL                         = 0x2026, /* 指定信道 */
    WID_SEND_CHANNEL                   = 0x2027,

    /* NMAC Integer WID list */
    WID_11N_PHY_ACTIVE_REG_VAL         = 0x2080,
    WID_HUT_NUM_TX_PKTS                = 0x2081,
    WID_HUT_TX_TIME_TAKEN              = 0x2082,
    WID_HUT_TX_TEST_TIME               = 0x2083,
    WID_KEEP_ALIVE_TIME                = 0x2101,

    /* Custom Integer WID list */

    /* DFX Integer WID list */
    WID_DFX_MEM_VIEW                   = 0x2301, /* 查看指定地址内存上的值 */
    WID_11N_BA_AMPDU_PARAM             = 0x2400,

    WID_DISCONNECT                     = 0x2404,
    WID_INT_RESERVE                    = 0x2406,  /* 预留INT类型WID接口, WiFi RF认证获取接收包会使用此WID, 和Device侧一致 */

    /* EMAC String WID list */
    WID_SSID                           = 0x3000,
    WID_FIRMWARE_VERSION               = 0x3001,
    WID_OPERATIONAL_RATE_SET           = 0x3002,
    WID_BSSID                          = 0x3003,
    WID_WEP_KEY_VALUE                  = 0x3004,
    WID_11I_PSK                        = 0x3008,
    WID_11E_P_ACTION_REQ               = 0x3009,
    WID_1X_KEY                         = 0x300A,
    WID_HARDWARE_VERSION               = 0x300B,
    WID_MAC_ADDR                       = 0x300C,
    WID_HUT_DEST_ADDR                  = 0x300D,
    WID_MISC_TEST_MODES                = 0x300E,
    WID_PHY_VERSION                    = 0x300F,
    WID_SUPP_USERNAME                  = 0x3010,
    WID_SUPP_PASSWORD                  = 0x3011,
    WID_SITE_SURVEY_RESULTS            = 0x3012,
    WID_RX_POWER_LEVEL                 = 0x3013,
    WID_ADD_WEP_KEY                    = 0x3019,
    WID_REMOVE_WEP_KEY                 = 0x301A,
    WID_ADD_PTK                        = 0x301B,
    WID_ADD_RX_GTK                     = 0x301C,
    WID_ADD_TX_GTK                     = 0x301D,
    WID_REMOVE_KEY                     = 0x301E,
    WID_ASSOC_REQ_INFO                 = 0x301F,
    WID_ASSOC_RES_INFO                 = 0x3020,
    WID_WPS_STATUS                     = 0x3024,
    WID_WPS_PIN                        = 0x3025,
    WID_MANUFACTURER                   = 0x3026,
    WID_MODEL_NAME                     = 0x3027,
    WID_MODEL_NUM                      = 0x3028,
    WID_DEVICE_NAME                    = 0x3029,

    WID_SET_DEFAULT_KEY                = 0x3059,
    WID_ADD_WPIKEY                     = 0x3060,                      /*str*/
    WID_REMOVE_WPIKEY                  = 0x3061,                      /*str*/

    /* NMAC String WID list */
    WID_11N_P_ACTION_REQ               = 0x3080,
    WID_HUT_TEST_ID                    = 0x3081,
    WID_PMKID_INFO                     = 0x3082,
    WID_FIRMWARE_INFO                  = 0x3083,

    /* Custom String WID list */
    WID_SERIAL_NUMBER                  = 0x3102,

    WID_P2P_TARGET_DEV_ID              = 0x3201,
    WID_P2P_INVIT_DEV_ID               = 0x3202,
    WID_P2P_PERSIST_CRED               = 0x3203,
    WID_P2P_NOA_SCHEDULE               = 0x3204,

    /* DFX String WID list */
    WID_DFX_PING                       = 0x3301,
    WID_DFX_CONTX                      = 0x3302,
    WID_COUNTRY_CODE                   = 0x3305,/* regulatory country code */
    WID_BA_INFO_QUERY                  = 0x3333,

    /* EMAC Binary WID list */
    WID_UAPSD_CONFIG                   = 0x4001,
    WID_UAPSD_STATUS                   = 0x4002,
    WID_WMM_AP_AC_PARAMS               = 0x4003,
    WID_WMM_STA_AC_PARAMS              = 0x4004,
    WID_NETWORK_INFO                   = 0x4005,

    WID_WPS_CRED_LIST                  = 0x4006,
    WID_PRIM_DEV_TYPE                  = 0x4007,
    WID_STA_JOIN_INFO                  = 0x4008,
    WID_CONNECTED_STA_LIST             = 0x4009,

    /* 指定信道扫描 */
    WID_BIN_CHANNEL_LIST               = 0x4100,
    WID_BIN_RC_BMAP                    = 0x4101,
    WID_BIN_RC_INFO                    = 0x4102,
    WID_BIN_FRAME                   = 0x4103,

     /* p2p */
    WID_FRAME                           = 0x4105,  /* HOST 下发P2P 管理帧内容 */
    WID_IE                              = 0x4106,  /* HOST 下发的wps IE 内容 */
    WID_IE_BEACON                      = 0x4107,
    WID_IE_PROBERSP                    = 0x4108,   /* 用于P2P扫描回应probe response报文携带的WPS P2P IE */
    WID_IE_ASOCRSP                     = 0x4109,
    WID_IE_PROBERSP_GO                 = 0x410A,   /* 用于GO回应probe response报文携带的WPS P2P IE */
    /* p2p end */
    /* NMAC Binary WID list */
    WID_11N_AUTORATE_TABLE             = 0x4080,
    WID_HUT_TX_PATTERN                 = 0x4081,
    WID_HUT_STATS                      = 0x4082,
    WID_HUT_LOG_STATS                  = 0x4083,

    /* Custom Binary WID list */
    WID_P2P_REQ_DEV_TYPE               = 0x4201,

    WID_TSSI_CMP_TAB                   = 0x4202,
    WID_RSSI_CMP_TAB                   = 0x4203,

#ifdef __ROAM__
    WID_ROAM                            = 0x4300,
#endif
#ifdef MAC_802_11W
    WID_PMF                            = 0x4400, /* host下发pmf消息 */
#endif

    /* software anti-jamming WID begin */
    /* Clock Frequency Plan WID */
    WID_FREQ_PLAN                      = 0x4501,

    /* Single Voice WID */
    WID_SINGLE_TYPE                    = 0x4505,

    /* DC OFFSET */
    WID_DC_OFFSET_TYPE                 = 0x4506,

    /* Miscellaneous WIDs */
    WID_ALL                            = 0x7FFE,
    WID_MAX                            = 0xFFFF
} WID_T;



enum truth_value_t
{
    tv_true =  1,
    tv_false = 2
};

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
/* char type wid */
struct char_wid{
    uint16 wid;
    uint8  len;
    uint8  val;
};

/* short type wid */
struct short_wid{
    uint16  wid;
    uint8   len;
    uint16  val;
};

/* int type wid */
struct int_wid{
    uint16  wid;
    uint8   len;
    uint32  val;
};

/* binary type wid */
struct binary_wid{
    uint16 wid;
    uint16 len;
    uint8  val[0];
};

/* string type wid */
struct string_wid{
    uint16 wid;
    uint8  len;
    uint8  val[0];
};

/* msg header struct */
struct hwifi_msg_header
{
    uint8           msg_type;       /* msg type:W or Q */
    uint8           msg_seq;        /* msg seq */
    uint16          msg_len;        /* msg len */
    struct char_wid host_cmd;
};


typedef struct
{
    UWORD32 ulLow;      /* 低32BIT数 */
    UWORD32 ulHigh;     /* 高32BIT数 */
}UWORD64_STRU;

/* 上报消息的头部 */
typedef struct
{
    UWORD8  ucMsgType;      /* 消息类型，I,R,W eg. */
    UWORD8  ucMsgIndex;     /* 消息编号，每上传一次消息后都会递增 */
    UWORD16 usMsgLength;    /* 消息长度 */
    UWORD16 usMsgStatus;    /* 上报状态类别 */
    UWORD16 usMsgSubStatus; /* 上报状态类别子类型，0-成功，其他值见协议 */
}CFG_MSG_HEADER_STRU;
/* 上报查询结果消息的头部 */
typedef struct
{
    UWORD8  ucMsgType;       /* 消息类型，R */
    UWORD8  ucMsgIndex;      /* 消息编号，每上传一次消息后都会递增 */
    UWORD16 usMsgLength;     /* 消息长度 */
    UWORD16 usWID;           /* WID号 */
    UWORD8  ucLength;        /* 长度 */
    UWORD8  ucInfo[];        /* 内容 */
}CFG_QUERY_INFO_STRU;

/* 扫描AP上报 */
typedef struct
{
    CFG_MSG_HEADER_STRU stMsgHdr;           /* 消息头部 */
    SWORD8              cRssi;              /* 信号强度 */
    UWORD8              ucChan;             /* 信道号 */
    UWORD16             usNetWorkMsgLength; /* 后续的Beacon或ProbeRsp帧长度，不包括FCS */
}CFG_MSG_NETWORK_INFO_STRU;

/* 扫描结束事件上报 */
typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;       /* 消息头部 */
}CFG_MSG_SCAN_COMP_STRU;

/* 关联结果事件上报 */
typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;           /* 消息头部 */
    UWORD16                 usCode;             /* 状态码 */
    UWORD8                  aucApMacAddr[6];    /* 关联成功的AP的MAC地址 */
}CFG_MSG_CONNECT_RESULT_STRU;

/* 去关联上报 */
typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;           /* 消息头部 */
    UWORD8                  ucAid;              /* 0-sta, 非0-AP */
}CFG_MSG_DISCONNECT_RESULT_STRU;

typedef struct
{
    UWORD8         ucTxRateIdx;     /* 发包速率索引 */
    UWORD8         ucTxRate;        /* 发包速率*/
}TX_RATE_STRU;

/* 统计查询上报具体统计信息 */
typedef struct
{
    UWORD32         ulRxPkt;        /* 关联后接收到的包数 */
    UWORD32         ulTxPkt;        /* 关联后发送的包数 */
    UWORD64_STRU    stRxPktBit;     /* 关联后接收的字节数 */
    UWORD64_STRU    stTxPktBit;     /* 关联后发送的字节数 */
    UWORD32         ulTxRetry;      /* 关联后重传次数 */
    UWORD32         ulTxFailed;     /* 关联后发送失败 */
    UWORD32         ulRxDropped;    /* 关联后接收失败 */
    SWORD8          cRssi;          /* 功率 */
    UWORD8          ucTxRateIdx;        /* 发包速率索引 */
    UWORD8          ucTxRateFlg;        /* 发包速率flg*/
}DEVICE_STATS_STRU;

/* 统计查询上报 */
typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;           /* 消息头部 */
    UWORD8                  aucApMacAddr[6];    /* AP:查询指定STA的MAC地址；STA:AP的MAC地址 */
    DEVICE_STATS_STRU       stStatInfo;         /* 流量统计信息 */
}CFG_MSG_STAT_QUERY_RESULT_STRU;

/* 设置元素或参数状态上报 */
typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;       /* 消息头部 */
}CFG_MSG_SET_RESULT_STRU;

/* DEVICE READY状态上报 */
typedef struct
{
   CFG_MSG_HEADER_STRU     stMsgHdr;       /* 消息头部 */
   UWORD8                  ucMode;         /* 0-STA，1-AP */
}CFG_MSG_MODE_RESULT_STRU;

/* STA关联后AP上报 */
typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;           /* 消息头部 */
    UWORD8                  ucAid;              /* 关联的STA的AID */
    UWORD8                  aucApMacAddr[6];    /* 成功关联上AP的STA的MAC地址 */

    UWORD8                  ucStaType;          /* 0表示STA未关联上AP;1-STA仅支持11A速率(5GHZ)，2-11B(2.4G),3-11B(2.4G),4-11AB,5-11N,6-11N*/
    UWORD8                  uc11gInfo;          /* bit0:1-支持前导码，0-不支持；bit1:1-支持短时隙，0-不支持*/
    UWORD16                 usWMM;              /* WMM信息 */
    UWORD16                 us11nCap;           /* 11N能力信息 */
    UWORD8                  ucCipherType;       /* 加密类型 */
}CFG_MSG_ASSOC_STA_STRU;

/* p2p added */
typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;           /* 消息头部 */
    UWORD8                  ucChan;             /* 信道号 */
    UWORD16                 usAid;
    UWORD16                 usMgmtFrmLength; /* 后续的帧长度，不包括FCS */
}CFG_MSG_MGMT_FRAME_STRU;
/* p2p added */

typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;       /* 消息头部 */
    UWORD8                  SourceAddr[6];  /* 接收帧的源地址 */
    UWORD8                  ucKeyType;      /* 是否为组播:1-组播,0-单播  */
}CFG_MSG_TKIP_MIC_ERROR_STRU;

#ifdef MAC_802_11W
/* PMF add */



/*****************************************************************************
  枚举名  : DEVICE_REPORT_PROTECTED_FRAME_TYPE
  协议表格:
  枚举说明: Device侧上报的加密管理帧类型
*****************************************************************************/
enum DEVICE_REPORT_PROTECTED_FRAME_TYPE
{
    DEVICE_REPORT_PROTECTED_DISASSOC = 0,   /* Disassociation帧 */
    DEVICE_REPORT_PROTECTED_DEAUTH   = 1,   /* Deauthentication帧 */
    DEVICE_REPORT_PROTECTED_ACTION   = 2,   /* ACTION帧(目前只有SA Query Response帧) */

    DEVICE_REPORT_PROTECTED_BUTT
};


struct pmf_info_stru
{
    struct sock *nl_sk;
    uint32 user_pid;

    uint8    dot11RSNAProtectedManagementFramesActivated;
    uint8    dot11RSNAUnprotectedManagementFramesAllowed;
};






/*******************************************************************************
    PMF上报消息结构
*******************************************************************************/
typedef struct
{
    CFG_MSG_HEADER_STRU   stMsgHdr;       /* 消息头部 */
    UWORD8                ucMFPR;         /* Management Frame Protection Required */
    UWORD8                ucMFPC;         /* Maximum index of rate supported */
    UWORD8                resv[2];
    UWORD32               ptk_type;         /* BUTT:ptk不存在  CCM:AES_128_CCM  */
    UWORD32               igtk_type;        /* BUTT:igtk不存在 CMAC:AES-128-CMAC  */
    UWORD8                ptk_index;
    UWORD8                ptk_key[16];
    UWORD8                igtk_index;
    UWORD8                igtk_key[16];
    UWORD8                dot11RSNAEnabled; /* MIB项 */
    UWORD8                aucPn[6];       /* PN码(48位无符号数) */
    UWORD8                resv2[1];
    UWORD16               usFrameLenght;  /* Deauthentication帧、Disassociation帧或SA Query Response帧长度(不包括FCS) */
    UWORD8                ucFrame[0];     /* Deauthentication帧、Disassociation帧或SA Query Response帧完整内容 */
}CFG_MSG_PROTECTED_MGMT_STRU;
/* PMF add */
#endif

#ifdef __ROAM__
typedef enum
{
    ROAM_CMD_TYPE_CFG                   = 0,
    ROAM_CMD_TYPE_SINGLE_CH             = 1,
    ROAM_CMD_TYPE_SEND_PROBE_REQ        = 2,
    ROAM_CMD_TYPE_STAY_HOME             = 3,
    ROAM_CMD_TYPE_SEL_NEW_AP            = 4,
    ROAM_CMD_TYPE_SEL_NEW_AP_TIMEOUT    = 5,

    ROAM_CMD_TYPE_BUTT
}ROAM_CMD_TYPE_ENUM;
typedef uint8 roam_cmd_type_enum_uint8;

typedef enum
{
    ROAM_SUB_STATUS_CFG_RESULT      = 0,
    ROAM_SUB_STATUS_TRIGGER         = 1,
    ROAM_SUB_STATUS_SCAN_RESULT     = 2,
    ROAM_SUB_STATUS_NEW_AP_ASSOC    = 3,
    ROAM_SUB_STATUS_RETURN_TO_OLD   = 4,
    ROAM_SUB_STATUS_TRIGGER_FORCE   = 5,

    ROAM_SUB_STATUS_BUTT
}ROAM_SUB_STATUS_ENUM;
typedef uint16 roam_sub_status_enum_uint16;

typedef struct
{
    UWORD8                      ucMsgType;      /* 消息类型，I,R,W eg. */
    UWORD8                      ucMsgIndex;     /* 消息编号，每上传一次消息后都会递增 */
    UWORD16                     usMsgLength;    /* 消息长度 */
    UWORD16                     usMsgStatus;    /* 上报状态类别  DEVICE_STATUS_ROAM */

    /* 0: roam cfg result 1: Trigger 2: scan  result 3: NEW AP Assoc 4: Return_to_old,其他值无效 */
    roam_sub_status_enum_uint16 enMsgSubStatus; /* 上报状态类别子类型 */
}roam_rpt_msg_header;

/* 0:漫游配置 */
struct roam_cfg_stru
{
    roam_cmd_type_enum_uint8        cmdtype;                /* 0:漫游配置 1:配置扫描信道 2:返回home信道 3:New AP Roam 4:New AP Assoc Timeout, 5:触发发送PROBE REQ消息 */
    int8                            status;                 /* 0:关闭ROAM, 1:打开ROAM */
    int8                            rssithreshold;          /* 单位1dBm */
    int8                            triggercnt;             /* 连续满足次数 */
    uint8               timeoutcnt;             /* roaming max timeout count */
};

/* roam scan req 1:配置扫描信道 */
struct roam_scan_cfg_ch_stru
{
    roam_cmd_type_enum_uint8        cmdtype;                /* 0:漫游配置 1:配置扫描信道 2:返回home信道 3:New AP Roam 4:New AP Assoc Timeout, 5:触发发送PROBE REQ消息 */
    uint8                           band;       /* 扫描频段 */
    uint8                           channel;    /* 扫描信道号 */
};

/* roam scan req 2:触发发送PROBE REQ消息 */
struct roam_scan_probe_req_stru
{
    roam_cmd_type_enum_uint8       cmdtype;                /* 0:漫游配置 1:配置扫描信道 2:返回home信道 3:New AP Roam 4:New AP Assoc Timeout, 5:触发发送PROBE REQ消息 */
};

/*  3:返回home信道 */
struct roam_stay_home_stru
{
    roam_cmd_type_enum_uint8        cmdtype;                /* 0:漫游配置 1:配置扫描信道 2:返回home信道 3:New AP Roam 4:New AP Assoc Timeout, 5:触发发送PROBE REQ消息 */
    uint8                           band;         /* 扫描频段 */
    uint8                           channel;      /* 扫描信道号 */
};

/* 切换AP请求 4:New AP Roam */
struct roam_roaming_stru
{
    roam_cmd_type_enum_uint8        cmdtype;                /* 0:漫游配置 1:配置扫描信道 2:返回home信道 3:New AP Roam 4:New AP Assoc Timeout, 5:触发发送PROBE REQ消息 */
    uint8                           bssid[6];
    uint8                           band;           /* 切换AP所在频段 */
    uint8                           channel;        /* 切换AP所在信道 */
    uint16                          length;
    uint8                           frame[0];
};

 /* 切换AP超时通知 5:New AP Assoc Timeout */
struct roam_roaming_timeout_stru
{
    roam_cmd_type_enum_uint8        cmdtype;                /* 0:漫游配置 1:配置扫描信道 2:返回home信道 3:New AP Roam 4:New AP Assoc Timeout, 5:触发发送PROBE REQ消息 */
    uint8                           bssid[6];
};

/* roam scan report */
struct roam_scan_report_stru
{
    uint8       band;           /* 切换AP所在频段 */
    uint8       channel;
    int8        rssi;     /* 单位1dBm */
    uint16      length;   /* BEACON帧或PROBERSP帧长度 */
    uint8       frame[0]; /* BEACON帧或PROBERSP完整内容 */
};

/* roam new ap assoc comp */
struct roam_new_ap_assoc_comp_stru
{
    uint8     band;                 /* 扫描频段 */
    uint8     channel;              /* 扫描信道号 */
    uint16    assoc_req_length;     /* assoc_req帧长度 */
    uint16    assoc_rsp_length;     /* assoc_rsp帧长度 */
    uint8     frame[0];             /* assoc_req/assoc_rsp完整内容 */
};


/*******************************************************************************
    下发消息结构
*******************************************************************************/
struct roam_cfg_msg
{
    struct hwifi_msg_header  msg_hdr;
    struct binary_wid        roam_wid;
    struct roam_cfg_stru     cfg_param;
};

/* roam scan msg */
struct roam_scan_cfg_ch_msg
{
    struct hwifi_msg_header         msg_hdr;
    struct binary_wid               roam_wid;
    struct roam_scan_cfg_ch_stru    scan_param;
};

/* roam scan msg */
struct roam_scan_probe_req_msg
{
    struct hwifi_msg_header  msg_hdr;
    struct binary_wid        roam_wid;
    struct roam_scan_probe_req_stru    scan_param;
};

/* roam stay home channel msg */
struct roam_stay_home_channel_msg
{
    struct hwifi_msg_header     msg_hdr;
    struct binary_wid           roam_wid;
    struct roam_stay_home_stru  home_param;
};

/* roam roaming msg */
struct roam_roaming_msg
{
    struct hwifi_msg_header  msg_hdr;
    struct binary_wid        roam_wid;
    struct roam_roaming_stru roaming_param;
};

/* roam roaming timeout msg */
struct roam_roaming_timeout_msg
{
    struct hwifi_msg_header             msg_hdr;
    struct binary_wid                   roam_wid;
    struct roam_roaming_timeout_stru    roaming_timeout_param;
};
/******************************************************************************/

/*******************************************************************************
    上报消息结构
*******************************************************************************/
struct roam_cfg_report_msg
{
    roam_rpt_msg_header     msg_header;
    uint8                   cfg_seq;
};

struct roam_trigger_msg
{
    roam_rpt_msg_header     msg_header;
};

/* scan report msg */
struct roam_scan_report_msg
{
    roam_rpt_msg_header             msg_header;
    struct roam_scan_report_stru    scan_report;
};

/* roam new ap assoc comp msg */
struct roam_roaming_result_msg
{
    roam_rpt_msg_header                 msg_header;
    struct roam_new_ap_assoc_comp_stru  new_ap_assoc_comp;
};
#endif

/* query ba session */
typedef struct
{
    UWORD8                  ucAid;                      /* AP模式下属于STA的关联id号，STA模式下默认为0 */
    UWORD8                  aucDestMacAddr[MAC_LEN];    /* BA会话对方MAC地址 */
    UWORD8                  ucTxBaSessionCount;         /* 发送方向建立的BA会话数 */
    UWORD8                  ucRxBaSessionCount;         /* 接收方向建立的BA会话数 */
    UWORD8                  aucTxBaTid[MAX_BA_SESS];    /* 根据ucTxBaSessionCount获取该数组的有效位中的TID值 */
    UWORD8                  aucRxBaTid[MAX_BA_SESS];    /* 根据ucTxBaSessionCount获取该数组的有效位中的TID值 */
}CFG_MSG_BA_SESS_STRU;

typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;                           /* 消息头部 */
    CFG_MSG_BA_SESS_STRU    ba_sess_data;    /* 上报mac对应的ba会话结果 */
}CFG_MSG_BA_SESSION_STRU;

/* query all ba session */
typedef struct
{
    UWORD8                  aucDestMacAddr[MAC_LEN];    /* BA会话对方MAC地址 */
    UWORD8                  ucTxBaTidsMap;              /* 发送方向建立的BA会话TID位图 */
    UWORD8                  ucRxBaTidsMap;              /* 接收方向建立的BA会话TID位图 */
}CFG_MSG_ALL_BA_SESS_STRU;

typedef struct
{
    CFG_MSG_HEADER_STRU     stMsgHdr;                           /* 消息头部 */
    CFG_MSG_ALL_BA_SESS_STRU    ba_sess_data[AP_TO_STA_MAX_NUM];    /* 上报mac对应的ba会话结果 */
}CFG_MSG_ALL_BA_SESSION_STRU;
/* EC212 */
typedef struct
{
	    CFG_MSG_HEADER_STRU     stMsgHdr;       /*TBD*/
			UWORD8                  ucKeyId;
			UWORD8                  ucKeyNum;
		  UWORD8                  ucType;
			UWORD8                  sta_addr[6];
		  UWORD8                  ucReserve;
}CFG_MSG_EC212_STRU;
/******************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


#pragma pack()
/*****************************************************************************
  10 函数声明
*****************************************************************************/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of cfg80211_stru.h */
