/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name    : hwifi_command.h
 * Version       : Initial Draft
 * Author        : Hash
 * Created       : 2012/9/21
 * Last Modified :
 * Description   : common.h header file
 * Function List :
 * History       :
 * 1.Date    : 2012/9/21
      Author      : Hash
      Modification: Created file
 *
 */

#ifndef __HWIFI_COMMAND_H__
#define __HWIFI_COMMAND_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include "hwifi_event.h"
#include "cfg80211_stru.h"

/*
 * 2 Macro Definition
 */
#define TK1_LEN                16
#define TK2_LEN                16
#define MIC_RX_LEN             8
#define MIC_TX_LEN             8
#define MIC_RX_OFFSET          24
#define MIC_TX_OFFSET          16
#define KEY_RSC_LEN            8

#define WID_11b_connect_PHY_ACTIVE_REG_1   0xff
#define WID_11b_connect_ACTIVE_REG_VAL_1   0x0
#define WID_11b_connect_PHY_ACTIVE_REG_2   0x83
#define WID_11b_connect_ACTIVE_REG_VAL_2   0x70
#define WID_11b_connect_PHY_ACTIVE_REG_3   0x8b
#define WID_11b_connect_ACTIVE_REG_VAL_3   0x20
#define WID_11b_connect_PHY_ACTIVE_REG_4   0x66
#define WID_11b_connect_ACTIVE_REG_VAL_4   0x1
#define WID_11b_connect_PHY_ACTIVE_REG_5   0x65
#define WID_11b_connect_ACTIVE_REG_VAL_5   0x1

#define WID_11b_disconnect_PHY_ACTIVE_REG_1   0xff
#define WID_11b_disconnect_ACTIVE_REG_VAL_1   0x0
#define WID_11b_disconnect_PHY_ACTIVE_REG_2   0x83
#define WID_11b_disconnect_ACTIVE_REG_VAL_2   0x60
#define WID_11b_disconnect_PHY_ACTIVE_REG_3   0x8b
#define WID_11b_disconnect_ACTIVE_REG_VAL_3   0x0



#define WID_G_MIXED_11B_2_MODE              2
#define WID_AUTO_RATE                       0
#define WID_QOS                             1
#define WID_SAVE_POWER                      1
#define WID_NO_ENCRYP_MODEL                 0
#define WID_OPEN_AUTHENTICATION             1
#define WID_OPEN_HT                         1
#define WID_2040_ENABLE_OPEN                1
#define WID_2040_ENABLE_CLOSE               0
#define WID_CHAR_RESERVE_SHORT_GI_CLOSE     0
#define WID_2040_COEXISTENCE_OPEN    1

#define CONNECTED_STA_PRE       0

#define WID_UAPSD_CONFIG_BUF_LEN   5 /* MAX_SP_Length, AC_BK, AC_BE, AC_VI, AC_VO */

#define MSG_TYPE_WRITE      'W'
#define MSG_TYPE_RESPONSE   'R'
#define MSG_TYPE_QUERY      'Q'

/* wid len */
#define CHAR_WID_LEN         1
#define SHORT_WID_LEN        2
#define INT_WID_LEN          4

#define MAX_MSG_LEN                1446

/* macro to print send cmd return result*/
#define PRINT_SEND_CMD_RET(cmd_name, ret) \
({\
    if (SUCC != ret) \
        { \
        HWIFI_WARNING("fail to send %s cmd msg", cmd_name); \
    } \
    else \
    { \
        HWIFI_DEBUG("succeed to send %s cmd msg", cmd_name); \
    } \
 })

/* macro to print report event return result*/
#define PRINT_REPORT_EVT_RET(event_name, ret) \
({\
    if (SUCC != ret) \
        { \
        HWIFI_WARNING("fail to report %s event", event_name); \
    } \
    else \
    { \
        HWIFI_DEBUG("succeed to report %s event", event_name); \
    } \
 })

/*
 * 3 Enum Type Definition
 */


/*
 * 4 Global Variable Declaring
 */

enum hw_mode {
    HWIFI_MODE_B_ONLY = 0,
    HWIFI_MODE_G_ONLY,       /* A mode also use this value */
    HWIFI_MODE_BG_1,     /* we use this for bg mode*/
    HWIFI_MODE_BG_2,
    HWIFI_NUM_MODES
};
#define HWIFI_MODE_A_ONLY HWIFI_MODE_G_ONLY


/* WID Data Types */
typedef enum {WID_CHAR     = 0,
              WID_SHORT    = 1,
              WID_INT      = 2,
              WID_STR      = 3,
              WID_BIN_DATA = 4
} WID_TYPE_T;

#ifdef MAC_802_11W
/*****************************************************************************
  枚举名  : PMF_CMD_TYPE_ENUM
  协议表格:
  枚举说明: Host下发配置命令类型
*****************************************************************************/
enum PMF_CMD_TYPE_ENUM
{
    PMF_CMD_TYPE_EXECUTE_ACTION = 0,
    PMF_CMD_TYPE_SEND_SA,

    PMF_CMD_BUTT
};
typedef unsigned char pmf_cmd_type_enum_uword8;

/*****************************************************************************
  枚举名  : PMF_ACTION_ENUM
  协议表格:
  枚举说明: Host处理去关联/去认证，以及SA查询匹配的结果
*****************************************************************************/
enum PMF_ACTION_ENUM
{
    PMF_PROC_DISCONNECT        = 0,   /* 接收到正确Deauth/Disassoc，STA与AP断开连接 */
    PMF_PROC_KEEP_CONNECT      = 1,   /* 接收到无效Deauth/Disassoc，STA与AP继续保持关联 */
    PMF_PROC_SA_QUERY_MISMATCH = 2,   /* SA查询匹配失败，STA与AP断开连接 */

    PMF_PROC_TYPE_BUTT
};
typedef unsigned char pmf_action_enum_uword8;
#endif

/* coexist CMD type */
typedef enum
{
    LTECOEX_CONFIG_CMD      = 1,
    LTECOEX_USEDCHAN_CMD    = 2,
    LTECOEX_MEASCHAN_CMD    = 3,
    LTECOEX_PRIORITY_CMD    = 4,
    LTECOEX_AFHSTATE_CMD    = 5,
    LTECOEX_CMD_BUTT,
}COEX_CMD_TYPE_ENUM;

/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
struct cfg_struct;
struct hwifi_scan_param_stru;
struct hwifi_conn_param_stru;
struct hwifi_disconn_param_stru;
struct hwifi_mode_param_stru;
struct hwifi_beacon_param_stru;
struct hwifi_bss_param_stru;
struct hwifi_pmksa_stru;
struct hwifi_stats_param_stru;
struct priv_key_param;
struct hwifi_wep_key_param;
struct hwifi_wiphy_param;
struct hwifi_hwmode_param_stru;

#pragma pack(1)


struct hwifi_11b_connect_msg {
    struct hwifi_msg_header msg_hdr;
    struct char_wid  phy_active_reg_1;
    struct int_wid   hw_11n_phy_active_reg_val_1;
    struct char_wid  phy_active_reg_2;
    struct int_wid   hw_11n_phy_active_reg_val_2;
    struct char_wid  phy_active_reg_3;
    struct int_wid   hw_11n_phy_active_reg_val_3;
};

struct hwifi_11b_disconnect_msg {
    struct hwifi_msg_header msg_hdr;
    struct char_wid  phy_active_reg_1;
    struct int_wid   hw_11n_phy_active_reg_val_1;
    struct char_wid  phy_active_reg_2;
    struct int_wid   hw_11n_phy_active_reg_val_2;
    struct char_wid  phy_active_reg_3;
    struct int_wid   hw_11n_phy_active_reg_val_3;
};




/* set uapsd msg */
struct set_uapsd_msg{
    struct hwifi_msg_header msg_header;
    struct binary_wid       wmm_uapsd_buf;
} __attribute__((packed));

/* set ap support wmm ps */
struct set_ap_uapsd_msg{
    struct hwifi_msg_header msg_header;
    struct char_wid         ap_uapsd;
};

/* query msg header */
struct hwifi_query_msg_header
{
    uint8  msg_type;       /* msg type:W or Q */
    uint8  msg_seq;        /* msg seq */
    uint16 msg_len;        /* msg len */
    uint16 wid_list[0];   /* query wid */
};

/* scan msg */
struct scan_msg {
    struct hwifi_msg_header msg_hdr;
    struct char_wid         scan_type_wid;
    struct short_wid        scan_band_wid;
};

/* scan channel */
struct channel_list_wid {
    uint16  wid;
    uint16  len;
    uint8   n_channels;
    uint8   channels[0];
};

/* connect msg */
struct connect_msg {
    /* msg header struct */
    struct hwifi_msg_header msg_hdr;

    /* crypto type:wep or wpa */
    struct char_wid         _11i_mode;

    /* auth type */
    struct char_wid         auth_type;

    /* ssid */
    struct string_wid       ssid;
};

/* wep key struct */
struct wep_key{
    uint16  wepkey_wid;
    uint8   wepkey_wid_len;
    uint8   set_flag;       /**/
    uint8   key_index;
    uint8   key_len;
    uint8   key_val[0];
};

/* crypto info */
struct crypto_info{
    /* pairwise key */
    struct char_wid pairwise;

    /* group key */
    struct char_wid group_key;
};

/* disconnect msg */
struct disconn_msg {
    struct hwifi_msg_header msg_hdr;
    struct int_wid          disconnect;
    struct char_wid         type;
};

/* change mode msg */
struct mode_msg{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         mode;
};

/* pmksa struct when n_pmksa = 1 */
struct pmksa_wid{
    uint8     n_pmksa;
    uint8     bssid[MAC_LEN];
    uint8     pmkid[PMKID_LEN];
};

/* pmksa msg */
struct pmksa_msg{
    struct hwifi_msg_header msg_hdr;
    uint16                  pmksa_wid;
    uint8                   pmksa_wid_len;
    struct pmksa_wid        pmksa_wid_value;
};

/* stat msg */
struct stats_msg {
    struct hwifi_msg_header msg_hdr;
    struct char_wid         stats_wid;
};


/* ipa msg */
struct set_ipa_msg {
    struct hwifi_msg_header msg_hdr;
    struct int_wid         ipa_wid;
};


/* set mac msg */
struct set_mac_msg {
    struct hwifi_msg_header  msg_hdr;
    struct string_wid        mac_wid;
};


/* regdom */
struct hwifi_regdom_rc_bmap_wid {
    uint16  wid;
    uint16  len;
    uint8   rc_bmap[8];
    uint8   rc_bmap_checksum;
};

struct  hwifi_regdom_country_code_wid {
    uint16  wid;
    uint8   len;
    uint8   code[2];
};

struct hwifi_regdom_radar_code_wid {
    uint16  wid;
    uint8   len;
    uint8   code;
};

struct hwifi_regdom_rc_info_wid {
    uint16  wid;
    uint16  len;
    uint8   regdom_class_num;       /* regdom class number,eg: US (6 rules)-> 0,1,2,3,4,5 */
    uint8   freq;                   /* frequency */
    uint8   bw;                     /* bandwidth */
    uint8   rc_beahaviour;          /* regulatory class behaviour */
    uint8   coverage_class;         /* regulatory coveryage class */
    uint8   rc_max_reg_tx_power;    /* reguatory class max tx power */
    uint8   rc_min_rf_tx_power;     /* regulatory class min rf tx power */
    uint8   rc_max_rf_tx_power_a;   /* regulatory class max rf tx power in 802.11a */
    uint8   rc_max_rf_tx_power_b;   /* regulatory class max rf tx power in 802.11b */
    uint8   rc_max_rf_tx_power_n;   /* regulatory class max rf tx power in 802.11n */
    uint32  chan_bmap;              /* regulatory class channel set */
    uint8   rc_max_rf_tx_power_n40; /* regulatory class max rf tx power in 802.11n 40Mhz */
    uint8   rc_info_checksum;
};

struct  hwifi_regdom_msg_wid {
    struct hwifi_msg_header              msg_hdr;
    struct hwifi_regdom_country_code_wid country_code_wid;  /* regulatory country code wid */
    struct hwifi_regdom_radar_code_wid   radar_code_wid;    /* radar standard code wid */
    struct hwifi_regdom_rc_bmap_wid      rc_bmap_wid;       /* regulatory class bmap wid */
    struct hwifi_regdom_rc_info_wid      rc_info_wid[];     /* regulatory info wid */
};


struct hwifi_regdom_param{
    struct hwifi_regdom_country_code_wid country_code_wid;  /* regulatory country code wid */
    struct hwifi_regdom_radar_code_wid   radar_code_wid;    /* radar standard code wid */
    struct hwifi_regdom_rc_bmap_wid      rc_bmap_wid;       /* regulatory class bmap wid */
    struct hwifi_regdom_rc_info_wid      rc_info_wid[];     /* regulatory info wid */
};

struct set_sta_2040_enable_msg{
    struct hwifi_msg_header   msg_hdr;
    struct char_wid           wid_2040_enable;
    struct char_wid           wid_char_reserve;
};


/* set protocol msg for initializing device.*/
struct set_init_protocol_msg {
    struct hwifi_msg_header msg_hdr;

    struct char_wid   wid_11g_operating_mode;
    struct char_wid   current_tx_rate;
    struct char_wid   qos_enable;
    struct char_wid   power_management;
    struct char_wid   wid_11i_mode;
    struct char_wid   auth_type;
    struct char_wid   wid_11n_enable;
    struct char_wid   wid_2040_enable;
    struct char_wid   wid_char_reserve;
    struct char_wid   wid_2040_coexistence_enable;
    struct int_wid    ba_session_param;
    struct string_wid mac_addr;

    struct short_wid  lte_coex_paras;
};


/* report mgmt frame struct */
struct mgmt_buf{
    uint16   mgmt_len;
    uint8    mgmt_val[0];
};

/* mode response struct */
struct mode_resp{
    uint8       msg_type;
    uint8       g_index;
    uint16      msg_len;
    uint16      dev_status;
    uint16      sub_status;
    uint8       dev_mode;
};

/* char,short,int,string wid header */
struct _hwifi_wid_header{
    short   wid;
    char    len;
};

struct _hwifi_ptk_data{
    char    des_addr[MAC_LEN];
    char    key_len;
    char    tk1[TK1_LEN];
    union
    {
        char tk2[TK2_LEN];
        struct
        {
            char michel_rx[MIC_RX_LEN];
            char michel_tx[MIC_TX_LEN];
        } mic;
    };
};

struct _hwifi_gtk_data{
    char    des_addr[MAC_LEN];
    char    key_rsc[KEY_RSC_LEN];
    char    key_index;
    char    key_len;
    char    tk1[TK1_LEN];
    union
    {
        char tk2[TK2_LEN];
        struct
        {
            char michel_rx[MIC_RX_LEN];
            char michel_tx[MIC_TX_LEN];
        } mic;
    };
};

/* add key msg */
struct _add_key_info{
    struct hwifi_msg_header    header;
    struct _hwifi_wid_header   wid_hdr;
    union
    {
        struct _hwifi_ptk_data ptk_dat;
        struct _hwifi_gtk_data gtk_dat;
    };
};

/* set key msg */
struct _set_defkey_info
{
    struct hwifi_msg_header     header;
    struct _hwifi_wid_header    wid_hdr;
    union
    {
        struct
        {
            char    mac_addr[MAC_LEN];
            char    key_index;
            char    type;
        } gtk_ptk;
    };
};

/* remove key msg */
struct _remove_key_info
{
    struct hwifi_msg_header     header;
    struct _hwifi_wid_header    wid_hdr;
    union
    {
        struct
        {
            char    mac_addr[MAC_LEN];
            char    key_index;
        } wep;
        struct
        {
            char    mac_addr[MAC_LEN];
            char    key_index;
            char    type;
        } ptk_gtk;
    };
};

struct wapi_key
{
    struct hwifi_msg_header  header;
    struct _hwifi_wid_header wid_hdr;

    uint8   keytype;
    uint8   keyid;
    uint8   ap_addr[MAC_LEN];
    uint8   sta_addr[MAC_LEN];
    uint32  wpi_ek_len;
    uint8   wpi_ek[WAPI_KEY_LEN];
    uint32  wpi_ck_len;
    uint8   wpi_ck[WAPI_KEY_LEN];
    uint8   pn[WAPI_PN_LEN];
};

/* set p2p on/off switch */
struct p2p_flag_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         p2p_flag;
    struct char_wid         p2p_gmode;
};

struct mgmt_tx_msg
{
    struct hwifi_msg_header msg_hdr;
    struct int_wid          channel;
    struct binary_wid       frame_buf;
};

struct p2p_listen_msg {
    struct hwifi_msg_header msg_hdr;
    struct int_wid         channel_wid;
};

struct channel_msg
{
    struct hwifi_msg_header msg_hdr;
    struct short_wid        channel_info;
    struct char_wid         enable_20M_40M;
    struct char_wid         coexist_20M_40M;
    struct char_wid         ht_enabled;
};

struct beacon_msg
{
    struct hwifi_msg_header  msg_hdr;
    struct char_wid          ap_network;
    struct short_wid         interval;       /* beacon interval struct */
    struct char_wid          dtim;
    struct char_wid          encrypt_mode;
    struct char_wid          pair_encrypt;
    struct char_wid          group_encrypt;
    struct char_wid          auth_type;
    struct short_wid         channel_info;
    struct char_wid          enable_20M_40M;
    struct char_wid          coexist_20M_40M;
    struct char_wid          ht_enabled;
    struct char_wid          broad_ssid;     /* 是否隐藏ssid */
    struct string_wid        ssid;           /* ssid struct */
};

struct p2p_ps_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         ctwindow;
};

struct ps_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         powersave;
};

struct set_listeninterval_msg
{
    struct hwifi_msg_header msg_header;
    struct char_wid         listeninterval;
};

struct set_pktfilter_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         pktfilter;
};

struct set_keepalive_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         keepalive;
};

struct enable_externTSF_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         enable_externTSF;
};

struct set_powermode_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         power_mode;
};

struct set_hips_switch_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         hips_switch;
};

struct fit_freq_msg
{
    struct hwifi_msg_header msg_hdr;
    struct binary_wid       fit_freq;
};

struct shutdown_RF_msg
{
    struct hwifi_msg_header msg_hdr;
    struct binary_wid       RF_shutdown;
};

struct shutdown_powersave_msg
{
    struct hwifi_msg_header msg_hdr;
    struct binary_wid       shutdown_powersave;
};

struct bt_opt_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid       bt_opt;
};

struct aggre_num_msg
{
    struct hwifi_msg_header msg_hdr;
    struct binary_wid       aggre_num;
};

struct set_oltpc_maxpower_msg
{
    struct hwifi_msg_header msg_hdr;
    struct short_wid        set_oltpc_maxpower;
};

struct noa_wid
{
    uint16 wid;
    uint8  len;
    uint8  noa_len;
    uint8  noa_num;
    uint8  noa1_cnt;
    uint32 noa1_dur;
    uint32 noa1_int;
    uint32 noa1_start;
    uint8  noa2_cnt;
    uint32 noa2_dur;
    uint32 noa2_int;
    uint32 noa2_start;
};

struct noa_msg
{
    struct hwifi_msg_header msg_hdr;
    struct noa_wid          noa;
};

struct sta_num_msg
{
    struct hwifi_msg_header msg_hdr;
    struct char_wid         sta_num;
};

struct lte_coex_paras_msg
{
    struct hwifi_msg_header msg_hdr;
    struct short_wid         lte_coex_paras;
};

#ifdef MAC_802_11W
/*******************************************************************************
    PMF下发消息结构
*******************************************************************************/
struct pmf_cmd_to_device_stru
{
    pmf_cmd_type_enum_uword8   enCmdType;
    pmf_action_enum_uword8     enAction;
};

struct pmf_send_sa_query_to_device_stru
{
    pmf_cmd_type_enum_uword8   enCmdType;
    uint8                      ucResv[1];
    uint16                    usLength;
    uint8                     ucFrame[0];
};

struct pmf_cfg_msg
{
    struct hwifi_msg_header         msg_hdr;
    struct binary_wid               pmf_wid;
    struct pmf_cmd_to_device_stru   cfg_param;
};

struct pmf_send_frame_msg
{
    struct hwifi_msg_header                   msg_hdr;
    struct binary_wid                         pmf_wid;
    struct pmf_send_sa_query_to_device_stru   send_frame_param;
};

struct pmf_msg
{
    struct hwifi_msg_header                   msg_hdr;
    struct char_wid                           pmf_activated;
    struct char_wid                           pmf_allowed;
};
#endif

#pragma pack()


/*
 * 8 UNION Type Definition
 */

extern uint8 g_ec212_count;
/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
extern int32  hwifi_sta_2040_enable_ctrl_set(struct cfg_struct *cfg,uint8 enabled);
extern uint16 hwifi_channel_index_to_freq(uint8 index);
extern int32 hwifi_start_scan(struct cfg_struct *cfg, struct hwifi_scan_param_stru *scan_param);
extern int32 hwifi_start_conn(struct cfg_struct *cfg, struct hwifi_conn_param_stru *conn_param);
extern int32 hwifi_start_disconn(struct cfg_struct *cfg, struct hwifi_disconn_param_stru *disconn_param);
extern int32 hwifi_start_change_mode(struct cfg_struct *cfg, struct hwifi_mode_param_stru *mode_param);
extern int32 hwifi_set_short_gi(struct cfg_struct *cfg, uint8 short_gi_enabled);
extern int32 hwifi_start_addset_beacon(struct cfg_struct *cfg, struct hwifi_beacon_param_stru *beacon_param);
extern int32 hwifi_start_set_bss(struct cfg_struct *cfg, struct hwifi_bss_param_stru *bss_param);
extern int32 hwifi_start_set_pmksa(struct cfg_struct *cfg, struct hwifi_pmksa_stru *pmksa_param);
extern int32 hwifi_query_stats(struct cfg_struct *cfg, struct hwifi_stats_param_stru *stats_param);
extern int32 hwifi_init_hw_params(struct cfg_struct *cfg);

extern int32 hwifi_set_static_wep_key(struct cfg_struct *cfg, struct hwifi_wep_key_param* param);
extern int32 hwifi_start_set_wiphy(struct cfg_struct *cfg, struct hwifi_wiphy_param *param);
extern int32 hwifi_start_set_hwmode(struct cfg_struct *cfg, struct hwifi_hwmode_param_stru *hwmode_param);

/* key management */
extern int hwifi_cmd_add_key(struct cfg_struct *cfg, struct priv_key_param *key, int8 *mac_addr, bool pairwise, int8 index);
extern int hwifi_cmd_set_defkey(struct cfg_struct *cfg, struct priv_key_param *key, int8 *mac_addr, uint8 index, bool unicast);
extern int hwifi_cmd_remove_key(struct cfg_struct *cfg, int8 *mac_addr, uint8 index, bool unicast, uint16 cipher);
/* key management */
extern int32 hwifi_cmd_add_wapi_key(struct cfg_struct *cfg, bool pairwise, int8 index, int8 *mac_addr, struct priv_key_param *key);
extern int32 hwifi_cmd_remove_wapi_key(struct cfg_struct *cfg, bool pairwise, int8 index, int8 *mac_addr);

/* send management frame */
extern int32 hwifi_send_mgmt_frame(struct cfg_struct *cfg,uint32 channel,const uint8* buf,int32 len);
extern int32 hwifi_send_p2p_flag(struct cfg_struct *cfg, int32 flag);
extern int32 hwifi_send_p2p_listen(struct cfg_struct *cfg, int32 channel);
extern int32 hwifi_cancel_p2p_listen(struct cfg_struct *cfg);

extern int32 hwifi_set_bt_opt(struct cfg_struct *cfg, int value);


extern int32 wl_send_channel(struct cfg_struct *cfg, uint16 channel, uint8 enable_20M_40M, uint8 coexist_20M_40M, uint8 ht_enabled);
extern int32 wl_get_channel_info(struct cfg_struct  *cfg);

extern int32 hwifi_set_pktfilter(struct cfg_struct *cfg, int32 value);
extern int32 hwifi_set_listeninterval(struct cfg_struct *cfg, int32 value);
extern int32 hwifi_set_powermode(struct cfg_struct *cfg, int32 value);
int32 cfg_event_rx_init(struct cfg_struct *cfg);
void cfg_event_rx_exit(struct cfg_struct *cfg);

/* WMM UAPSD */
extern int32  hwifi_set_wmm_uapsd(struct cfg_struct *cfg, struct wmm_uapsd_param *param);

#if 0
/* power save */
extern int32  hwifi_cfg80211_set_power(struct cfg_struct *cfg, uint32 ps_mode);
/* power save */
extern int32 hwifi_cmd_set_ps(struct cfg_struct *cfg, uint8 ps);

#endif
extern int32 hwifi_cmd_set_noa(struct cfg_struct *cfg, struct hwifi_p2p_noa_param *noa_param);
extern int32 hwifi_cmd_set_p2p_ps(struct cfg_struct *cfg, uint8 ctwindow);
extern int32 wl_set_mac(struct cfg_struct *cfg, uint8* mac);
extern uint8 bytes_check_sum(uint8* buf, uint32 len);

extern struct sk_buff * hwifi_alloc_skb_for_cmd(uint32 len);
extern int32 hwifi_send_cmd(struct cfg_struct *cfg, struct sk_buff  *skb);

extern void hwifi_fill_msg_hdr(struct hwifi_msg_header *header, uint8 cmd_type, uint16 msg_len);

extern int32 hwifi_fit_cpufreq(struct cfg_struct *cfg, uint8 value);
extern int32 hwifi_shutdown_RF(struct cfg_struct *cfg);
extern int32 hwifi_shutdown_powersave(struct cfg_struct *cfg);
extern int32 hwifi_set_hips_switch(struct cfg_struct *cfg, int32 value);

extern int32 hwifi_keepalive_enable(struct cfg_struct *cfg, int32 value);
extern int32 hwifi_set_oltpc_maxpower(struct cfg_struct *cfg, int32 value);

#ifdef __ROAM__
extern int32 hwifi_start_roam_set_param(struct cfg_struct *cfg, struct roam_cfg_stru *cfg_param);
extern int32 hwifi_start_roam_scan(struct cfg_struct *cfg, struct roam_scan_cfg_ch_stru *scan_param);
extern int32 hwifi_start_roam_send_probe_req(struct cfg_struct *cfg, struct roam_scan_probe_req_stru *scan_param);
extern int32 hwifi_start_roaming(struct cfg_struct *cfg, struct roam_roaming_stru *param, uint8 *frame);
extern int32 hwifi_start_roaming_timeout(struct cfg_struct *cfg, struct roam_roaming_timeout_stru *param);
extern int32 hwifi_start_stay_home(struct cfg_struct *cfg, struct roam_stay_home_stru *home_param);
extern int32 hwifi_set_dev_roam_report_enabled(struct cfg_struct *cfg, uint8 enabled);
#endif
#ifdef MAC_802_11W
extern int32 hwifi_pmf_send_nl_msg(struct cfg_struct *cfg, uint8 *buf, uint32 msg_len);
extern void hwifi_pmf_resv_nl_msg(struct sk_buff *skb);
extern int32 hwifi_pmf_init(struct cfg_struct *cfg);
extern int32 hwifi_send_pmf_cmd(struct cfg_struct *cfg, uint8 *buf, uint32 buf_len);
extern int32 hwifi_recv_pmf_report(struct cfg_struct *cfg, uint8 *buf);
extern void hwifi_pmf_exit(struct cfg_struct *cfg);
extern int32 pmf_param(struct cfg_struct* cfg, int param_cmd , int32 *param_value, int32 *get);

#endif

extern int32 hwifi_process_disconn_event(struct cfg_struct *cfg);

extern int32 wl_get_dbb_info(struct cfg_struct  *cfg);

extern int32 wl_get_upc_info(struct cfg_struct  *cfg);

extern int32 hwifi_set_lte_coex_paras(struct cfg_struct *cfg, int8 lte_paras);

extern void hwifi_fill_query_msg_hdr(struct hwifi_query_msg_header *header, uint16 msg_len);

static inline void hwifi_fill_char_wid(struct char_wid *wid, uint16 wid_value,uint8 vlaue)
{
    wid->wid = OS_CPU_TO_LE16(wid_value);
    wid->len = CHAR_WID_LEN;
    wid->val = vlaue;
}

static  inline void hwifi_fill_short_wid(struct short_wid *wid, uint16 wid_value,uint16 vlaue)
{
    wid->wid = OS_CPU_TO_LE16(wid_value);
    wid->len = SHORT_WID_LEN;
    wid->val = OS_CPU_TO_LE16(vlaue);
}

static inline void hwifi_fill_int_wid(struct int_wid *wid, uint16 wid_value,int32 vlaue)
{
    wid->wid = OS_CPU_TO_LE16(wid_value);
    wid->len = INT_WID_LEN;
    wid->val = OS_CPU_TO_LE32(vlaue);
}

static inline void hwifi_fill_binary_wid(struct binary_wid *wid, uint16 wid_value, uint16 val_len,const uint8 *vlaue)
{
    wid->wid = OS_CPU_TO_LE16(wid_value);
    wid->len = OS_CPU_TO_LE16(val_len);
    if (val_len != 0)
    {
       OS_MEM_CPY(wid->val, vlaue, val_len);
    }
}

static inline void hwifi_fill_string_wid(struct string_wid *wid, uint16 wid_value, uint8 val_len,uint8 *vlaue)
{
    wid->wid = OS_CPU_TO_LE16(wid_value);
    wid->len = val_len;
    OS_MEM_CPY(wid->val, vlaue, val_len);
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_command.h */
