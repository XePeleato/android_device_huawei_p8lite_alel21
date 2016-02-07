
#ifndef __HWIFI_ROAM_ALG_H__
#define __HWIFI_ROAM_ALG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __ROAM__

/*
 * 1 Other Header File Including
 */
#include "common.h"
#include "hwifi_utils.h"
#include "hwifi_regdomain.h"

/*
 * 2 Macro Definition
 */
#define ROAM_RSSI_LEVEL    3

#define ROAM_BLACKLIST_AP_NUMBER   20
#define ROAM_EXT_AP_NUMBER   20
#define ROAM_BLACKLIST_COUNT_LIMIT 2
 /*
 * 3 Enum Type Definition
 */

/*  */
typedef enum
{
    ROAM_PARAM_BAND_2G = 0,  /*  */
    ROAM_PARAM_BAND_5G,
    ROAM_PARAM_BAND_BOTH,

    ROAM_PARAM_BAND_BUTT
} roam_param_band_enum;

typedef enum
{
     ROAM_PROTOCOL_11B    = 0x00000001,
     ROAM_PROTOCOL_11G    = 0x00000010,
     ROAM_PROTOCOL_11A    = 0x00000100,
     ROAM_PROTOCOL_11N_20 = 0x00001000,
     ROAM_PROTOCOL_11N_40 = 0x00010000,

     ROAM_PROTOCOL_BUTT
} roam_protocol_enum;

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

struct roam_candidate_ap_stru
{
    uint32    ap_idx;  /*  */
    uint8     bssid[6];
    int8      rssi;
    uint16    sta_count;
    int32     channel_utilization;
    int32     avail_channel_capacity;
    uint32    hw_mode;
    uint8     channel;
    uint8     band;
    uint16    frame_len;
    uint8    *frame;
};

typedef struct{
    uint8         bssid[6];
    int32         count;           /* 加入黑名单次数 */
    unsigned long time_stamp;      /*  */
} roam_ap_stru;

typedef struct
{
    spinlock_t    lock;
    uint32        nums;
    roam_ap_stru  blacks[ROAM_BLACKLIST_AP_NUMBER];
}roam_ap_black_list_stru;

/*  */
typedef struct
{
    spinlock_t        lock;
    uint32        nums;
    roam_ap_stru      ext_ap[ROAM_EXT_AP_NUMBER];
}roam_ext_ap_stru;

struct roam_scan_candidate_ap_node
{
    spinlock_t       ap_node_lock;
    struct list_head list;
    uint8  user_num;
    struct roam_candidate_ap_stru  ap_info;
};

struct roam_scan_candidate_ap_list
{
    spinlock_t    list_lock;
    struct list_head list;
};

typedef struct
{
    int rssi;
    uint32 capacity_kbps;
} roam_rssi_capacity_stru;

typedef struct
{
    uint32 single_user_foreign_dwell_time_ms;
    uint32 single_user_home_dwell_time_ms;
    uint32 concurrent_user_number;
    int32 is_orthogonal;
    roam_param_band_enum band;
} roam_scan_param_t;

typedef struct roam_wlan_trigger_param
{
    int trigger_rssi_threshold;
    uint32 trigger_count_threshold;
} roam_trigger_param_t;

typedef struct roam_wlan_execute_param
{
    uint32 pingpong_expire_time_s;
    int32 pingpong_rssi_threshold;
    uint32 blacklist_expire_time_s;
    uint32 down_route_learning_packet_number;
} roam_execute_param_t;

typedef struct roam_tx_packet_failed_param
{
    int32 time_interval;              /* 取最近time_interval时间内的报文发送数据来统计报文发送失败率 */
    int32 tx_failed_scale_threshold;  /* 报文发送失败率触发漫游门限,eg:失败率70%,值为70 */
    int32 rssi_threshold;             /* 触发漫游的rssi门限，必现大于等于这个门限值 */
    uint32 tx_packet_num_threshold;   /* 触发漫游的发送报文总数门限 */
    int32 tx_rate_threshold;          /* 触发漫游的发送速率最高上限 */

    int32 record_count;               /* 每计数到time_interval计数发送失败率，达到门限触发漫游扫描 */
    int32 tx_failed_trigger_flag;     /* 报文发送失败率过高触发漫游标志 */

    uint32 tx_success_pkt[2];         /* tx_success_pkt[0]存在第0秒统计值,tx_success_pkt[1]存在第time_interval秒统计值 */
    uint32 tx_failed_pkt[2];          /* tx_failed_pkt[0]存在第0秒统计值,tx_failed_pkt[1]存在第time_interval秒统计值 */
}roam_tx_packet_failed_param_t;

typedef struct st_roam_wlan_alg_param
{
    roam_scan_param_t      scan_param;
    roam_trigger_param_t   trigger_param;
    roam_execute_param_t   execute_param;
    int throughput_threshold_kbps;
    roam_rssi_capacity_stru   dot11b_table[ROAM_RSSI_LEVEL];
    roam_rssi_capacity_stru   dot11g_ofdm_table[ROAM_RSSI_LEVEL];
    roam_rssi_capacity_stru   dot11a_ofdm_table[ROAM_RSSI_LEVEL];
    roam_rssi_capacity_stru   ht_2G_ofdm_table[ROAM_RSSI_LEVEL];
    roam_rssi_capacity_stru   ht20_5G_ofdm_table[ROAM_RSSI_LEVEL];
    roam_rssi_capacity_stru   ht40_5G_ofdm_table[ROAM_RSSI_LEVEL];
    roam_tx_packet_failed_param_t tx_pkt_param;
} roam_wlan_alg_param_t;

typedef struct roam_wpa_cipher_suite
{
    uint8    auth_type;
    uint8    group_crypto;
    uint8    pairwise_crypto;
}roam_wpa_cipher_suite_t;


/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */


struct roam_all_stru;
struct cfg_struct;

extern struct roam_scan_candidate_ap_node * hwifi_roam_candidate_ap_alloc(void);
extern void hwifi_roam_candidate_ap_free(struct roam_scan_candidate_ap_node *ap_node);
extern void hwifi_roam_candidate_ap_get(struct roam_scan_candidate_ap_node *ap_node);
extern void hwifi_roam_candidate_ap_put(struct roam_candidate_ap_stru *ap_info);
extern int32 hwifi_roam_candidate_ap_add(struct roam_all_stru *roam_info, uint8 *buf, struct roam_scan_candidate_ap_node *candidate_ap);
extern int32 hwifi_roam_candidate_ap_update(struct roam_all_stru *roam_info, uint8 *buf, struct roam_candidate_ap_stru *candidate_ap);
extern bool hwifi_roam_ssid_is_dismatch(struct cfg_struct *cfg, uint8 ssid_len, uint8 *ssid);
extern bool hwifi_roam_ie_is_mismatch(struct cfg_struct *cfg, uint8 *new_beacon_frame, int32 new_beacon_len);
extern int32 calc_scan_interval(struct roam_all_stru *pst_roam_info, int32 rssi_current);
extern uint32 hwifi_roam_judge_ap_hw_mode(uint16 frame_len, uint8 *frame, uint8 band);
extern int32 hwifi_roam_lookup_capacity_by_rssi(roam_rssi_capacity_stru *pst_rssi_table, int rssi);
extern int32 hwifi_roam_rssi_mapping_capacity(struct roam_all_stru *pst_roam_info, struct roam_candidate_ap_stru *pst_ap_info);
extern int32 hwifi_roam_calc_avail_channel_capacity_to_utility(struct roam_all_stru *pst_roam_info, struct roam_candidate_ap_stru *pst_ap_info);
extern struct roam_candidate_ap_stru* hwifi_roam_find_candidate_ap_by_bssid(struct roam_scan_candidate_ap_list *ap_list, uint8* bssid);
extern bool hwifi_roam_is_in_ap_blacklist(struct roam_all_stru *roam_info, uint8 *ap_bssid);
extern bool hwifi_roam_is_pingpong(struct roam_all_stru *roam_info, uint8 *ap_bssid);

extern void hwifi_roam_update_blacklist(struct roam_all_stru *pst_roam_info, uint8 *bssid);
extern void hwifi_roam_blacklist_bssid_clear(struct roam_all_stru *pst_roam_info, uint8 *bssid);
extern void hwifi_roam_update_ext_ap(struct roam_all_stru *pst_roam_info, uint8 *bssid);
extern struct roam_candidate_ap_stru *hwifi_roam_select_candidate_ap(struct roam_all_stru *pst_roam_info);
extern void hwifi_roam_clear(struct roam_all_stru *pst_roam_info);
extern int32 hwifi_roam_update_scan_channels(struct roam_all_stru *roam);
extern void hwifi_roam_update_channels_regdomain_flag(struct roam_all_stru *roam, const struct ieee80211_regdomain *regdom, const struct hwifi_regdom_rc_info_wid *rc_info_wid, struct hwifi_freq_map *freq_map_2G, uint8 map_size_2G,struct hwifi_freq_map *freq_map_5G, uint8 map_size_5G);
extern int32 hwifi_roam_update_channels_band_flag(struct roam_all_stru *roam);
extern int32 hwifi_roam_update_channels_orthogonal_flag(struct roam_all_stru *roam);
extern int32 hwifi_roam_init_domain_channels(struct roam_all_stru *roam, struct hwifi_freq_map *freq_map_2G, uint8 map_size_2G, struct hwifi_freq_map *freq_map_5G, uint8 map_size_5G);


#endif /* __ROAM__ */


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


#endif /* end of hwifi_roam_alg.h */
