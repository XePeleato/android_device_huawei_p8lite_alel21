/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name    : cfg_api.h
 * Version       : Initial Draft
 * Author        : Hash
 * Created       : 2012/9/29
 * Last Modified :
 * Description   : cfg api for net device driver
 * Function List :
 * History       :
 * 1.Date    : 2012/9/29
      Author      : Hash
      Modification: Created file
 *
 */

#ifndef __CFG_API_H__
#define __CFG_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */


/*
 * 2 Macro Definition
 */


/*
 * 3 Enum Type Definition
 */


/*
 * 4 Global Variable Declaring
 */
extern int32 exception_enable;


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
struct wireless_dev;
struct hi110x_device;
struct net_device;
/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
/* get struct from cfg struct */
extern struct wiphy* wl_get_wiphy(struct cfg_struct *cfg);

extern void wl_set_ndev(struct cfg_struct *cfg, struct net_device *ndev);
extern int32 hwifi_set_mac(struct cfg_struct *cfg, uint8* mac);

extern void wl_set_wdev(struct cfg_struct *cfg, struct wireless_dev *wdev);
/* alloc and free cfg struct */
extern struct cfg_struct* hwifi_cfg80211_alloc(struct hi110x_device *adapter, struct device *dev);
extern void hwifi_cfg80211_free(struct cfg_struct *cfg);

extern int32 hwifi_p2p_flag_set(struct cfg_struct *cfg,int32 flag);
extern bool hwifi_p2p_flag_get(struct cfg_struct *cfg);
extern int32 hwifi_get_cur_mode(struct cfg_struct *cfg);

extern int32 wl_set_p2p_ie(struct cfg_struct *cfg,
                                uint8             *beacon_ie,
                                uint16             beacon_ie_len,
                                uint8             *probe_rsp_ie,
                                uint16             probe_rsp_ie_len,
                                uint8             *asoc_rsp_ie,
                                uint16             asoc_rsp_ie_len);

extern int32 hwifi_set_auth_alg(struct cfg_struct *cfg, uint8 auth_alg);
extern int32 hwifi_set_hw_mode(struct cfg_struct *cfg, uint8 hwmode);
extern int32 hwifi_set_ht_enabled(struct cfg_struct *cfg, uint8 ht_enabled);
extern int32 hwifi_set_ssid(struct cfg_struct *cfg, int8 *ssid);
extern int32 hwifi_set_auto_channel(struct cfg_struct *cfg, int8 auto_channel);
extern int8 hwifi_get_auto_channel(struct cfg_struct *cfg);
extern int32 hwifi_get_cur_channel(struct cfg_struct *cfg);
extern void* hwifi_get_sta_req_ie(struct cfg_struct *cfg, uint8* mac);
extern uint16 hwifi_get_current_freq(struct cfg_struct *cfg);

extern int32 wl_set_p2p_ps(struct cfg_struct *cfg, int32 ctwindow, int32 opps, int32 legacy_ps);
extern int32 wl_set_p2p_noa(struct cfg_struct *cfg, uint8 count, int32 start, int32 duration);
extern int32  wl_get_p2p_noa(struct cfg_struct *cfg, uint8 *buf, uint8 buf_len);
#ifdef WLAN_PERFORM_DEBUG
extern void wlan_disable_perform_debugging(struct hi110x_device* hi110x_dev,uint8 *buf);
extern void wlan_set_and_enable_perform_debugging(struct hi110x_device* hi110x_dev,uint8 *buf);
#endif

extern int32 hwifi_get_active_status(void);

extern int32 wl_set_wmm_uapsd(struct cfg_struct *cfg, uint8 *buf);

extern int32 wl_set_ba(struct cfg_struct *cfg, uint8 *buf);
extern int32 wl_set_ampdu(struct cfg_struct *cfg, uint8 *buf);
extern int32 wl_set_amsdu(struct cfg_struct *cfg, uint8 *buf);
extern int32 wl_get_amsdu(struct cfg_struct *cfg);
extern int32 wl_get_ba(struct cfg_struct *cfg);
extern int32 wl_get_ampdu(struct cfg_struct *cfg);
extern void  wl_wmm_set_mode(struct cfg_struct *cfg, uint8 wmm_mode);

extern void hwifi_scan_done(struct cfg_struct *cfg);

/* set sta num */
extern int32 wl_set_max_sta_num(struct cfg_struct *cfg, int32 num);

/* set ap uapsd */
extern int32 wl_set_wmm_ap_uapsd(struct cfg_struct *cfg, uint8 enabled);

extern int32 hwifi_set_tkip_measure(struct cfg_struct *cfg, int32 disabled);

#ifdef __ROAM__
extern int32 roam_alg_param(struct cfg_struct* cfg, int param_cmd , int32 *param_value, int32 *get);
extern int32 roam_need_data_buffering(struct cfg_struct* cfg, struct sk_buff *skb);
extern int32 roam_data_buffering(struct cfg_struct* cfg, struct sk_buff *skb);
#endif

extern int32 hwifi_set_init_flag(struct cfg_struct *cfg, uint8 flags);
extern int32 hwifi_wlan_close(struct net_device *ndev);
extern int32 hwifi_wlan_open(struct net_device *ndev);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of cfg_api.h */
