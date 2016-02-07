/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name    : hwifi_aggre.h
 * Version       : Initial Draft
 * Author        : yWX164715
 * Created       : 2013/03/18
 * Last Modified :
 * Description   : hwifi_aggre.h header file
 * Function List :
 * History       :
 * 1.Date    : 2013/03/18
      Author      : yWX164715
      Modification: Created file
 *
 */

#ifndef __HWIFI_AGGRE_H__
#define __HWIFI_AGGRE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include "hwifi_msg.h"

/*
 * 2 Macro Definition
 */

/* Teardown/BA/A-mpdu/A-msdu */
#define N_P_ACTION_REQ      0x7
#define BA_CATEGORY         0x3

#define A_MSDU_START        0
#define A_MSDU_END          1
#define DEF_MSDU_NUM        0x10
#define MAX_AMSDU_SIZE      0xEFF
#define MAX_AMSDU_TIMEOUT   0xA     /* ms */

#define A_MPDU_START        2
#define A_MPDU_END          3

#define MLME_ADDBA_REQ_TYPE 0
#define MLME_DELBA_REQ_TYPE 2

#define MAX_AMPDU_TIMEOUT   0x64
#define AMPDU_AGGRE_MAX_NUM 16

#define AGGRE_NUM_AC                      4
#define AGGRE_AC_BK                       0     /* background */
#define AGGRE_AC_BE                       1     /* best effort */
#define AGGRE_AC_VI                       2     /* video */
#define AGGRE_AC_VO                       3     /* voice */

/*
 * 3 Enum Type Definition
 */


/*
 * 4 Global Variable Declaring
 */
#ifdef WLAN_ARRG_DYNAMIC_CONTROL
extern unsigned long g_data_rate_down;
extern unsigned long g_data_rate_up;
#endif


/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
/* match user data struct */
#pragma pack(1)

 /*kernel AMSDU BA and AMPDU param */
 struct aggre_cfg_param
 {
     uint8 ba_action_type;
     uint8 amsdu_action_type;
     uint8 ampdu_action_type;
     uint8 mac_addr[MAC_LEN];
     uint8 TID;
     uint8 max_num;
 };

 /* kernel query mac_address ba session param  */
 struct query_ba_param
 {
     uint8   flag;
     uint8   mac_addr[MAC_LEN];
     uint8   conn_num;
     struct ba_sess_query_res_stru ba_sess_res[AP_TO_STA_MAX_NUM];
 };

 /* kernel host config ba session threshold: BA_TX, BA_RX, BA_RETRY */
 struct host_ba_cfg_param
 {
     uint8   max_tx;
     uint8   max_rx;
     uint16  max_retry;
 };

 struct host_ba_aggre_num_param
 {
     uint8   aggre_set_tag;
     uint8   max_aggre_num;
     uint8   method;
 };

 /* WID */

/*teardown wid to clear AMPDU and BA information */
struct set_teardown_wid{
    uint16  wid;
    uint8   wid_len;
    uint8   context_len;    /* msg length 3 */
    uint8   msg_type;       /* 7 */
    uint8   flags;          /* 0xFF */
};

/* set ba_wid */
struct set_ba_wid{
    uint16  wid;
    uint8   wid_len;
    uint8   context_len;
    uint8   msg_type;
    uint8   action_type;
    uint8   bssid[MAC_LEN];
    uint8   TID;
};

struct set_addba_wid{
    struct set_ba_wid ba_wid;
    uint8   ba_policy;
    uint16  buf_size;
    uint16  ba_timeout;
    uint16  addba_timeout;
    uint8   max_frame;
    uint16  buf_timeout;
};

struct set_delba_wid{
    struct set_ba_wid ba_wid;
    uint8   direction;
    uint8   result_code;
};

struct set_amsdu_wid{
    uint16  wid;
    uint8   wid_len;
    uint8   context_len;
    uint8   msg_type;
    uint8   action_type;              /* 0 for start, 1 for stop */
    uint8   bssid[MAC_LEN];
    uint8   TID;
};


/* set amsdu wid */
struct set_start_amsdu_wid{
    struct  set_amsdu_wid amsdu_wid;
    uint8   max_num;
    uint16  amsdu_max_size;
    uint16  timeout;
};


/*set ampdu wid */
struct set_ampdu_wid{
    uint16  wid;
    uint8   wid_len;
    uint8   context_len;
    uint8   msg_type;
    uint8   action_type;
    uint8   bssid[MAC_LEN];
    uint8   TID;
};

struct set_start_ampdu_wid{
    struct  set_ampdu_wid ampdu_wid;
    uint8   max_mpdu;
    uint16  timeout;
    uint8   ack_policy;
};


/*set query ba wid */
struct set_query_ba_wid{
    uint16  wid;
    uint8   wid_len;
    uint8   flag;
    uint8   bssid[MAC_LEN];
};

/*set aggre mode wid */
struct set_aggre_mode_wid{
    uint16  wid;
    uint8   wid_len;
    uint8   aggre_mode;
};

/*set aggre config wid */
struct set_aggre_cfg_wid{
    uint16  wid;
    uint8   wid_len;
    uint8   max_tx;
    uint8   max_rx;
    uint16  max_retry;
};

/*set teardown msg */
struct set_teardown_msg{
    struct hwifi_msg_header msg_header;
    struct set_teardown_wid set_teardown;
};

/*set BA msg */
struct set_addba_msg{
    struct hwifi_msg_header    msg_header;
    struct set_addba_wid       set_ba;
};

struct set_delba_msg{
    struct hwifi_msg_header    msg_header;
    struct set_delba_wid       set_ba;
};


/*set ampdu msg */
struct set_start_ampdu_msg{
    struct hwifi_msg_header     msg_header;
    struct set_start_ampdu_wid  set_ampdu;
};

struct set_end_ampdu_msg{
    struct hwifi_msg_header msg_header;
    struct set_ampdu_wid    set_ampdu;
};


/* set amsdu msg */
struct set_end_amsdu_msg{
    struct hwifi_msg_header msg_header;
    struct set_amsdu_wid    set_amsdu;
};

struct set_start_amsdu_msg{
    struct hwifi_msg_header       msg_header;
    struct set_start_amsdu_wid    set_amsdu;
};

 /*set query ba msg */
 struct set_query_ba_msg{
     struct hwifi_msg_header msg_header;
     struct set_query_ba_wid set_query_ba;
 };

 /*set aggre mode msg */
 struct set_aggre_mode_msg{
     struct hwifi_msg_header msg_header;
     struct set_aggre_mode_wid set_aggre_mode;
 };

 /*set aggre config msg */
  struct set_aggre_cfg_msg{
      struct hwifi_msg_header msg_header;
      struct set_aggre_cfg_wid set_aggre_cfg;
  };

#pragma pack()

 /*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */

extern int32 wl_query_ba_set(struct cfg_struct *cfg, struct query_ba_param *param);
extern int32 wl_aggre_mode_set(struct cfg_struct *cfg, uint8 enabled);
extern int32 wl_aggre_cfg_set(struct cfg_struct *cfg, struct host_ba_cfg_param *param);

extern int32 wl_aggre_mode_get(struct cfg_struct *cfg, uint8 *aggre_mode);
extern int32 wl_aggre_cfg_get(struct cfg_struct *cfg, struct host_ba_cfg_param *host_ba_param);

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
extern int32 hwifi_start_timer(struct cfg_struct *cfg);
extern int32 hwifi_stop_timer(struct cfg_struct *cfg);
extern int32 hwifi_save_flow_info(struct sk_buff *skb, struct cfg_struct *cfg);
extern void hwifi_aggr_init(struct cfg_struct *cfg);
extern int32 wl_set_aggre_func(struct cfg_struct *cfg, uint8 enabled);
extern int32 wl_get_aggre_func(struct cfg_struct *cfg, uint8 *aggre_func);
#endif

#ifdef __cplusplus
    #if __cplusplus
         }
    #endif
#endif

#endif /* end of hwifi_aggre.h */

