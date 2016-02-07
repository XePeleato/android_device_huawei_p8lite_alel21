/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name    : hwifi_event.h
 * Version       : Initial Draft
 * Author        : Hash
 * Created       : 2012/9/22
 * Last Modified :
 * Description   : common.h header file
 * Function List :
 * History       :
 * 1.Date    : 2012/9/22
      Author      : Hash
      Modification: Created file
 *
 */

#ifndef __HWIFI_EVENT_H__
#define __HWIFI_EVENT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */

#include "hi110x.h"

/*
 * 2 Macro Definition
 */



#define WEP40_KEY_LEN              5
#define WEP104_KEY_LEN             13

#define WMM_DISABLE                0
#define WMM_ENABLE                 1

/* Start of zhangwei 64406 2013-03-26 B295 ÐÞ¶©¸³Öµ´íÎó */
#define WPA_KEY_LEN                32
#define WPA_SEQ_LEN                16
/* End of zhangwei 64406 2013-03-26 B295 ÐÞ¶©¸³Öµ´íÎó */
//#define HWIFI_NO_PARAM_CHANGE     -1

/*
 * 3 Enum Type Definition
 */


/* auth type */
enum hwifi_auth_type_enum
{
    HWIFI_AUTHTYPE_OPEN_SYSTEM = 1,     /* open system */
    HWIFI_AUTHTYPE_SHARED_KEY,          /* shared key */

    /* TODO: other mode */
    HWIFI_AUTHTYPE_BUTT
};

/* connect result */
enum conn_status_enum
{
    CONN_SUCC = 0,
    AUTH_FAILED,
    ASOC_FAILED,
    UNSPECIFIED_FAILED,
};


enum hwifi_key_type {
    HWIFI_KEYTYPE_GROUP,
    HWIFI_KEYTYPE_PAIRWISE,
    HWIFI_KEYTYPE_PEERKEY,

    NUM_HWIFI_KEYTYPES
};

/*
 * 4 Global Variable Declaring
 */


/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
struct scanned_bss_info;


struct priv_key_param
{
    uint32          cipher;
    int             key_len;
    int             seq_len;
    unsigned char   key[WPA_KEY_LEN];
    unsigned char   seq[WPA_SEQ_LEN];
};


/* AP info which is found */
struct scanned_bss_event
{
    uint16  freq;                 /* center frequency */
    int32   rssi;                    /* signal strength */
    uint8  *mgmt;
    uint16  mgmt_len;
};

/* finish scan info struct */
struct scan_complete_stru
{
    uint16 reason;
};

/* connect result info struct */
struct hwifi_conn_result_stru
{
    uint8   bssid[MAC_LEN];
    uint8  *req_ie;
    uint32  req_ie_len;
    uint8  *resp_ie;
    uint32  resp_ie_len;
    int32   connect_status;
    uint16  status_code;
};

#ifdef __ROAM__
/* roamed ap info  */
struct hwifi_roamed_event
{
    uint32  band;
    uint32  channel;
    uint8  *bssid;
    uint8  *req_ie;
    uint32  req_ie_len;
    uint8  *resp_ie;
    uint32  resp_ie_len;
};
#endif

/* disconnect result info struct */
struct hwifi_disconn_result_stru
{
    uint16  reason_code;         /* disconnect reason code */
    uint8  *disconn_ie;         /* disconnect frame ie */
    uint32  disconn_ie_len;      /* disconnect frame ie length */
};

/* set mode result struct */
struct hwifi_set_mode_stru
{
    uint8                        set_mode_res;
    enum hi110x_network_mode     hwifi_recv_mode;
};


/* change param info result struct */
struct hwifi_change_res_stru
{
    uint8    set_param_res;
};

/* init device information result struct */
struct hwifi_init_res_stru
{
    uint8    init_param_res;
};

/* device ready result struct */
struct hwifi_status_ready_stru
{
    uint8   ready_status_res;
};


/* statistic information result*/
struct hwifi_stats_result_stru
{
    uint8   *mac;       /* MAC of station */
    uint32   rx_packets;
    uint32   tx_packets;
    uint32   rx_bytes;
    uint32   tx_bytes;
    int32    signal;
    struct rate_info    txrate;
    uint32   tx_retries;
    uint32   tx_failed;
    uint32   rx_dropped_misc;
};

/* p2p added - mgmt frame report */
struct mgmt_report_event_stru
{
    uint16  freq;                 /* center frequency */
    uint16  type;
#define MGMT_FRAME_PROBE_REQUEST 0
#define MGMT_FRAME_ACTION        1
#define MGMT_FRAME_ASSOC_REQ     2
#define MGMT_FRAME_REASSOC_REQ   3
    uint8  *mgmt;
    uint16  mgmt_len;
    union {
        uint16 value;
        uint16 aid;
    } u;
};

struct mic_error_report_stru
{
    uint8 *addr;
    enum hwifi_key_type key_type;
    int32  key_id;
};

struct hwifi_sta_join_stru
{
    uint8    success;
    uint8    aid;
    uint8   *mac;
};

struct hwifi_sta_leave_stru
{
    uint8    aid;
};

struct hwifi_current_channel
{
    uint8 channel;
    uint8 band;
};

/* scan param struct */
struct hwifi_scan_param_stru
{
    struct {
        uint8   *ssid;       /* ssid array */
        uint8    ssid_len;    /* length of the array */
    } ssids[8];
    int32               ssid_num;
    uint32              type;
#define HWIFI_ACTIVE_SCAN          1
#define HWIFI_PASSIVE_SCAN         0
    const uint8        *ie;
    uint32              ie_len;
    uint32              band;
#define BAND_2G (1 << 0)
#define BAND_5G (1 << 1)
#define BAND_ALL (BAND_2G | BAND_5G)
    uint8               n_channels;
    uint8              *channels;
};

/* connect param struct */
struct hwifi_conn_param_stru
{
    uint8                     bssid_flag;           /* flag for bssid */
    uint8                    *bssid;    /* BSSID of AP */
    uint8                    *ssid;  /* SSID of AP */
    uint16                    ssid_len;             /* length of SSID */
    enum hwifi_auth_type_enum auth_type;            /* auth type */
    uint8                     group_crypto;
    uint8                     pairwise_crypto;
    uint8                     crypto_type;
    uint8                     auth_mode;
    uint8                     wep_key_len;
    uint8                     wep_key[WEP104_KEY_LEN];
    uint8                     wep_key_index;
    uint8                     *ie;
    uint32                    ie_len;
    struct scanned_bss_info   *bss_info;
    uint8                     channel;
    /* TODO: so on */
};

/* disconnect param struct */
struct hwifi_disconn_param_stru
{
    uint16 reason_code; /* disconnect reason code */
    uint16 aid;
    uint8  type;
};

/* change mode struct */
struct hwifi_mode_param_stru
{
    enum hi110x_network_mode next_mode;
};

/* Beacon infor struct */

struct hwifi_beacon_param_stru
{
    uint16   interval;            /* beacon interval */
    uint8    dtim_period;         /* DTIM period */
    const  uint8   *ssid;                /* ap ssid */
    uint16   ssid_len;            /* length of SSID */
    uint8    ssid_is_hidden;      /* 1:hidden ssid,0:not hidden */

    uint8    auth_type;
    uint8    crypto_mode;
    uint8    group_crypto;
    uint8    pairwise_crypto;
    const  uint8   *p2p_ie;              /* p2p ie */
    uint8    p2p_ie_len;          /* p2p ie len */
    const  uint8   *wps_ie;              /* wps ie */
    uint8    wps_ie_len;          /* wps ie len */
    uint16       channel;
    uint8        enable_20M_40M;
    uint8        coexist_20M_40M;
    uint8        ht_enabled;
};
/* BSS infor struct */
struct hwifi_bss_param_stru
{
#define BSS_PARAM_NO_CHANGE     (-1)
#define BSS_PARAM_DISABLE   0
#define BSS_PARAM_ENABLE    1
    int32   cts_prot;         /* Whether to use CTS protection:0 = no, 1 = yes, -1 = do not change */
    int32   short_preamble;   /* Whether the use of short preambles is allowed:0 = no, 1 = yes, -1 = do not change */
    int32   short_slot_time;  /* Whether the use of short slot time is allowed:0 = no, 1 = yes, -1 = do not change */

    int32   ht_opmode;            /* ht_opmode: HT Operation mode:u16 = opmode, -1 = do not change */
};

struct hwifi_hwmode_param_stru
{
#define HW_MODE_B 0
#define HW_MODE_G 1
#define HW_MODE_A 2
#define HW_MODE_NUM 3
    uint8 hw_mode;
};

struct hwifi_pmksa_stru
{
    uint8   bssid[MAC_LEN];
    uint8   pmkid[PMKID_LEN];
};

/* statistic param struct */
struct hwifi_stats_param_stru
{
    uint8   aid;                 /* aid for station, 0 for ap */
};



/* STA WMM UAPSD Param */
struct  wmm_uapsd_param       /* STA WMM UAPSD(Enable/Disable) */
{
    uint8          max_sp_length;
    uint8          ac_be;             /* STA Power Save AC_BE, Enable/Disable */
    uint8          ac_bk;             /* STA Power Save AC_BK, Enable/Disable */
    uint8          ac_vi;             /* STA Power Save AC_VI, Enable/Disable */
    uint8          ac_vo;             /* STA Power Save AC_VO, Enable/Disable */
};


/* wiphy param struct */
struct hwifi_wiphy_param
{
    uint8 frag_threshold_changed;
    uint8 rts_threshold_changed;
    uint32 frag_threshold;
    uint32 rts_threshold;
};
/* install static wep key cmd struct */
struct hwifi_wep_key_param
{
#define MAX_WEP_KEY_COUNT 4
    uint8 default_index;
    struct {
        uint8 index;
        uint8 len;
        uint8 key[WEP104_KEY_LEN];
    } keys[MAX_WEP_KEY_COUNT];
};

/* p2p noa param */
struct hwifi_p2p_noa_param
{
    uint8 count;
    int32 interval;
    int32 duration;
    int32 starttime;
};


struct ba_sess_query_res_stru
{
    uint8 id;
    uint8 mac_addr[MAC_LEN];
    uint8 tx_ba_num;
    uint8 rx_ba_num;
    uint8 tx_tid[MAX_BA_SESS];
    uint8 rx_tid[MAX_BA_SESS];
};

/* Clock Frequency Plan param */
struct clk_fplan_param
{
    uint32  cmu_cfg_2_addr;
    uint16  cmu_cfg_2_40m;
    uint32  p160m_dvalue_en_0_addr;
    uint16  p160m_dvalue_en_0_40m;
    uint32  p160m_dvalue_addr;
    uint16  p160m_dvalue_40m;
    uint32  p160m_dvalue_en_1_addr;
    uint16  p160m_dvalue_en_1_40m;
    uint32  p80m_dvalue_en_0_addr;
    uint16  p80m_dvalue_en_0_40m;
    uint32  p80m_dvalue_addr;
    uint16  p80m_dvalue_40m;
    uint32  p80m_dvalue_en_1_addr;
    uint16  p80m_dvalue_en_1_40m;
    uint32  aon_bfg_antii_clkmux_ctl_addr;
    uint16  aon_bfg_antii_clkmux_ctl_40m;
    uint32  aon_wl_antii_clkmux_ctl_addr;
    uint16  aon_wl_antii_clkmux_ctl_40m;
    uint32  cmu_gt_wl_addr;
    uint16  cmu_gt_wl_40m;
};

/* Single Voice Anti-jam param */
struct sv_antijam_param
{
    uint8   spur_ctrl_cfg_addr;
    uint32  spur_ctrl_cfg;          /* bit[0]:0 2.4Ghz or bit[0]:1 5.0Ghz */
    uint8   spur_phase_step_cfg_addr;
    uint32  spur_phase_step_cfg;
    uint8   spur_cfg_addr;
    uint32  spur_cfg;
    uint8   r_tone_amp_th_addr;
    uint32  r_tone_amp_th;
    uint8   fft_weight_en_addr;
    uint32  fft_weight_en;
    uint8   subcarr_idx0_addr;
    uint32  subcarr_idx0;
    uint8   subcarr_idx1_addr;
    uint32  subcarr_idx1;
    uint8   fft_shift_bit_addr;
    uint32  fft_shift_bit;          /* fft weigth enable signal */
    uint8   subcarr_idx2_addr;
    uint32  subcarr_idx2;           /* subcarr according to map */
    uint8   subcarr_idx3_addr;
    uint32  subcarr_idx3;
};

/* DC Offset param */
struct dc_offset_param
{
    uint8 vga;
    uint8 min_i;
    uint8 max_i;
    uint8 min_q;
    uint8 max_q;
    uint8 weigth;
    uint8 comp_swap;
    uint8 vlsb;
    uint8 cali_interv;
};

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_event.h */
