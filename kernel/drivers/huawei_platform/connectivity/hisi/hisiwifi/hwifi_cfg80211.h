

#ifndef __HWIFI_CFG80211_H__
#define __HWIFI_CFG80211_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include "common.h"
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <net/cfg80211.h>

#include "hwifi_scan.h"
#include "hwifi_aggre.h"
#include "hwifi_wpa_ioctl.h" /* for wl_pwrm_set */
//#include "hwifi_wl_config_ioctl.h"
#include "hwifi_sfw_antijam.h"
#include "hwifi_tps.h"

#ifdef __ROAM__
#include "hwifi_roam_main.h"
#endif

#include "hwifi_rfauth_ioctl.h"

/*
 * 2 Macro Definition
 */
#define IS_STA(cfg)        (HI_MODE_STA == cfg->hi110x_dev->mode.current_mode)
#define IS_AP(cfg)         (HI_MODE_AP == cfg->hi110x_dev->mode.current_mode)
#define P2P_ON(cfg)        ((cfg)->p2p_info.on)
#define IS_P2P_ON(cfg)     ((cfg)->p2p_info.on)

#define RSN_SELECTOR_LEN 4
#define WPA_SELECTOR_LEN 4

#define WPA_GET_BE32(a) ((((uint32) (a)[0]) << 24) | (((uint32) (a)[1]) << 16) | \
        (((uint32) (a)[2]) << 8) | ((uint32) (a)[3]))

#define WPA_GET_BE16(a) ((uint16) (((a)[0] << 8) | (a)[1]))
#define WPA_GET_LE16(a) ((uint16) (((a)[1] << 8) | (a)[0]))


#define RSN_SELECTOR_GET(a)               WPA_GET_BE32((const uint8 *) (a))

#define WPA_CIPHER_SUITE_TKIP             0x0050f202
#define WPA_CIPHER_SUITE_CCMP             0x0050f204
#define WPA_AUTH_KEY_MGMT_UNSPEC_802_1X   0x0050f201
#define WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X 0x0050f202

#define RSN_CIPHER_SUITE_TKIP             0x000fac02
#define RSN_CIPHER_SUITE_CCMP             0x000fac04
#define RSN_AUTH_KEY_MGMT_UNSPEC_802_1X   0x000fac01
#define RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X 0x000fac02

#define WPA_IE_VENDOR_TYPE                0x0050f201
#define WPS_IE_VENDOR_TYPE                0x0050f204
#define P2P_IE_VENDOR_TYPE                0x506f9a09


#define IS_CONNECTED(cfg)  (HWIFI_CONNECTED == (cfg)->conn.status)

/* crypto status */
#define HWIFI_ENCRYPT        (1 << 0)
#define HWIFI_WEP            (1 << 1)
#define HWIFI_WEP104         ((1 << 2) | (1 << 1))
#define HWIFI_WPA            (1 << 3)
#define HWIFI_WPA2           (1 << 4)
#define HWIFI_CCMP           (1 << 5)
#define HWIFI_TKIP           (1 << 6)

#define HWIFI_WAPI           (1 << 5)

/* auth */
#define HWIFI_8021X          (1 << 2)
#define HWIFI_WAPI_CERT      (1 << 3)

#define HWIFI_40M_PLUS       (1 << 8 )
#define HWIFI_40M_MINUS      ((1 << 8) | (1 << 9))
#define HWIFI_BAND_5G        (1 << 15)



#define HISI_CONFIG_WIFI_DEFAULT_MODE               5
#define HISI_CONFIG_WIFI_NORMAL_MODE                0
#define HISI_CONFIG_WIFI_PERFORMANCE_MODE           1
#define HISI_CONFIG_WIFI_CERTIFY_MODE               2
#define NV_WLAN_NUM                                 193
#define NV_WLAN_VALID_SIZE                          12

#define INVALID_COUNTRY_CODE                        ("00")
#define DEFAULT_COUNTRY_CODE                        ("99")
#define COUNTRY_CODE_LEN                             2




#define WIFI_CURRT_TX_POW_INVALID                  -1

#define WIFI_DEVICE_LOG_MAX                        63
#define WIFI_DEVICE_LOG_INVALID                    -1
#define WIFI_DEVICE_LOG_ON                         1
#define WIFI_DEVICE_LOG_OFF                        0


#define VERIFY_CODE 1101


/*
 * 3 Enum Type Definition
 */

enum hwifi_connect_status_enum
{
    HWIFI_DISCONNECTED = 0,          /* disconnected status */
    HWIFI_CONNECTING,                /* connecting status */
    HWIFI_CONNECTED,                 /* connected status */

    HWIFI_STATUS_BUTT
};

/*
 * 4 Global Variable Declaring
 */
/* TBD */
extern uint32           g_coex_level;

/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */


/* statistic information */
struct stats_struct
{
    int32  aid;
    uint32 updated;     /* got statistic information */
    uint32 connect_ts;  /* connect timestamp */

    uint32 rx_packets;
    uint32 tx_packets;
    uint32 rx_bytes;
    uint32 tx_bytes;
    int32   signal;
    struct rate_info tx_rate;
    uint32 tx_retries;
    uint32 tx_failed;
    uint32 rx_dropped_misc;
};

struct conn_bss_info
{
    uint8   ssid[IEEE80211_MAX_SSID_LEN];
    uint8   ssid_len;
    uint8   bssid[MAC_LEN];
    uint16  freq;
    uint32 auth_type;
    uint32 crypto_type;
    uint32 band;
    uint32 channel;

#ifdef __ROAM__
    /* save connected ap beacon packet */
    uint32 mgmt_len;
    struct ieee80211_mgmt *mgmt;
    int32  assoc_ie_len;
    uint8  *assoc_ie;
#endif

    struct stats_struct  stats;
};

struct conn_sta_info
{
    uint32      flags;                  /* status of station... */
    int32       aid;
    uint8       mac[MAC_LEN];
#define MAX_REQ_IE_LEN 1024
    struct {
        uint32  ie_len;
        uint8   ie[MAX_REQ_IE_LEN];
    } assoc_req;

    struct priv_key_param   ptk;
    struct stats_struct  stats;          /* statistic informations of station */
};

struct conn_struct
{
    int32                             auto_authtype; /* auto authtype for wep */
    enum   hwifi_connect_status_enum  status;  /* connect status */
    struct sk_buff                   *reconn_msg;    /* param for reconnect with shared key */
    struct conn_bss_info              bss;
};


struct scanned_bss_mgmt;
struct scan_struct
{
    uint32                           is_scanning;   /* scanning flag */
    struct cfg80211_scan_request    *request;       /* current scan request */
    struct work_struct               work;
    int32                            complete;

    struct scanned_bss_mgmt          bss_mgmt;
};

struct key_mgmt_struct
{
    uint8 default_index;

#ifdef MAC_802_11W
    uint8 ptk_key_index;
    uint8 gtk_key_index;
    uint8 igtk_key_index;

    uint8 mgmt_mul_addr[6];
#endif

    uint8 uni_addr[6];
    char                    mul_addr[6];
#define MAX_KEY_NUM 4
    struct priv_key_param   ptk[MAX_KEY_NUM]; /* ptk key cache */
    struct priv_key_param   gtk[MAX_KEY_NUM]; /* gtk or wep key cache */

#ifdef MAC_802_11W
    struct priv_key_param   igtk[6];          /* igtk key cache */
#endif
};


struct sta_mgmt_struct
{
#define MAX_CONN_STA_CNT 10
    struct conn_sta_info    stations[MAX_CONN_STA_CNT];         /* memory address of station info array  */
    int32                   generation;
};


struct param_mgmt_struct
{
    struct mutex mutex;
    struct completion complete;
    int32 flag;
};

#pragma pack(1)
struct p2p_noa
{
    uint8  cnt;
    int32  dur;
    int32  interval;
    int32  start;
};
#pragma pack()

struct p2p_ct
{
    uint8 opps;
    uint8 ctwindow;
};

struct p2p_info_struct
{
    int32 on; /* p2p on/off switch */

#if 0
    uint8  index;    /* for noa attr in p2p ie, 0~255 */
#endif

    struct timer_list listen_timer;
    spinlock_t timer_lock;
    uint64 last_roc_id;
    struct ieee80211_channel remain_on_chan;
    struct p2p_noa noa_info;
    struct p2p_ct  ct_info;
     /* 保存wpa_supplciant FIND阶段下发的监听状态信息
     * hwifi_cfg80211_remain_on_channel接口需要根据此参数判断是否下发监听命令到device还是直接返不支持错误
     * 此参数为TRUE说明是FIND阶段下发的监听状态，如果当前是GO模式，或client模式应阻止下发到device侧
     * 否则是发送管理报文切换到指定的监听信道，需要下发到device侧
    */
    uint8 find_listen_state;
};

/* save DC_OFFSET configuration args */
struct dc_offset_info
{
    uint8 vga;
    uint8 max_i;
    uint8 min_i;
    uint8 max_q;
    uint8 min_q;
    uint8 weigth;
};

struct aggre_info_struct
{
    spinlock_t lock;

    uint8 aggre_mode_enabled;               /* auto aggreagtion mode set */

    struct host_ba_cfg_param            host_ba_cfg;   /* max_tx, max_rx, max_retry */
    struct query_ba_param               ba_param_res;

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
    uint8             aggre_func;
    unsigned long     rate;
    unsigned long     short_frame_pps;
    int32             aggr_start;
    struct timer_list data_rate_timer;
#endif
};

struct sf_antijam_struct
{
    struct dc_offset_info dc_offset;    /* save DC_OFFSET configuration args */
};

struct device_log_struct
{
    int32 device_log_flag[WIFI_DEVICE_LOG_MAX];
};


struct sta_info_struct
{

    /*wl_config*/
    uint8               rate;                   /* set rate */
    uint8               test_case;                   /* test case */
    uint8               mcs;                    /* set mcs rate */
    uint8               qos_enabled;            /* qos set enabled */
    uint8               rifs_prot_enabled;      /* rifs prot enabled */
    uint8               ack_policy_enabled;     /* ack policy enabled */
    uint8               txop_prot_enabled;      /* txop prot enabled */
    uint8               erp_prot;               /* erp prot enabled */
    uint8               htprot;                 /* htprot enabled */
    uint8               gi_enabled;             /* gi enabled */
    uint8               ht_capability_enabled;  /* ht enabled */
    uint8               pwrm;                   /* curreent power management */
    uint8               manual_pwrm;            /* manual set pwrm */
    uint8               ps_enabled;             /* power save enable */
    uint8               prot;                   /* prot mode set */
    uint8               coex_2040_enabled;      /* sta mode 2040 coex mode set */
    uint8               fc_enabled;             /* sta mode fc param enabled */
    uint8               gmode;                  /* set 11g operating mode  */
    uint8               nmode;                  /* set 11n operating mode */
    int32               lint;                   /* set listen interval */
    uint8               pream;                  /* set preamble type */
    uint8               ctxbw;                  /* set tx channel bandwidth */
    uint8               intol;                  /* set 40MHz Intolerance mode */
    uint8               hwar;                   /* set hardware auto rate */
    uint8               pk_mode;
    uint8               curr_channel;           /* current channel */

    int32               currenttxpow;           /* current tx power */
};

struct ap_info_struct
{
    uint8  dot11_auth_mode;    /* open:0,shared:1*/
    uint8  ht_enabled;
    uint8  hw_mode;       /* b:0,g:1,a:2 */
    uint8  auto_channel;  /* open: 1, close :0*/
    uint8  curr_channel;
    uint32 dot1x_akm;
    uint32 pairwise_cipher;
    uint32 group_cipher;
    uint8  ssid[IEEE80211_MAX_SSID_LEN];
    uint8  ssid_len;
    uint8  bssid[MAC_LEN];
    struct sta_mgmt_struct      sta_mgmt;
    /*
    channel_info:
    BIT15：0->2.4G，1->5G
    BIT9-8：0->SCN，1->SCA，3->SCB（20M情况上设置为SCN，40M情况选择SCA或SCB）
    BIT7-0：主信道号
    如果参数低8BITS配置为0，则AP在建立网络过程表示自动信道选择。
    */
    uint16 channel_info;
    uint8 coex_2040_enabled;    /* ap mode 2040 coex mode set */
    uint8 fc_enabled;           /* ap mode fc param enabled */
    uint8 amsdu_certify_enabled;    /* amsdu certify mode set */
    uint8 psm_certify_enabled;    /* psm certify mode set */
};

struct cfg_struct
{
    spinlock_t              lock;
    wait_queue_head_t       wait_queue;


    struct workqueue_struct *scan_wq;
    struct delayed_work      pwrm_set_work;

    struct wiphy            *wiphy;
    struct net_device       *ndev;      //TODO
    struct wireless_dev     *wdev;      //new
    struct hi110x_device    *hi110x_dev;    //TODO

    struct scan_struct          scan;
    struct conn_struct          conn;
    struct key_mgmt_struct      key_mgmt;
    struct param_mgmt_struct    param_mgmt;
    struct sta_info_struct      sta_info;
    struct p2p_info_struct      p2p_info;

    struct aggre_info_struct    aggre_info;
    struct dc_offset_param      dc_offset;

    struct ap_info_struct       ap_info;
#ifdef MAC_802_11W
    struct pmf_info_stru        pmf_info;
#endif
    uint64 send_action_id;
    uint8                       init_flag;   // init device

    struct completion           init_done;


    /* regulatory country code */
    int8                        beacon_ie_country_code[COUNTRY_CODE_LEN+1];    /* AP国家码 */
    int8                        sim_country_code[COUNTRY_CODE_LEN+1];          /* sim卡国家码 */
    int8                        custom_country_code[COUNTRY_CODE_LEN+1];       /* 定制化国家码 */
    int8                        current_country_code[COUNTRY_CODE_LEN+1];       /* 当前使用国家码 */

    const struct ieee80211_regdomain *regdom;

    struct  device_log_struct   device_log;


    struct  wifi_android_ioctl_data_struct_pri android_ioctl;


    uint8                       beacon_set;
    int32                       tkip_measure_flags;
#ifdef __ROAM__
    struct roam_all_stru roam;
#endif

    struct mutex dev_err_lock;

    int32 latest_signal;
    struct rate_info rate;
};

#ifdef __ROAM__
#define cfg_to_roam(cfg) (&(cfg)->roam)
#endif

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
int32 hwifi_ap_deinit(struct cfg_struct *cfg);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_cfg80211.h */
