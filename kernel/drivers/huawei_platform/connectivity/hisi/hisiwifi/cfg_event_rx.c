


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include "common.h"
#include "hwifi_utils.h"
#include "hi110x.h"
#include "hwifi_event.h"
#include "hwifi_cfg80211.h"
#include "station_mgmt.h"
#include "hwifi_regdomain.h"
#include "hwifi_scan.h"
#include "hwifi_init.h"

#include <linux/sched.h>
#include "hi110x_pm.h"

#ifdef __ROAM__
#include "hwifi_roam_main.h"
#endif

#include "hwifi_hcc.h"

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
#include "hwifi_aggre.h"
#endif

/*
 * 2 Global Variable Definition
 */
struct hwifi_inform_ops
{
    int32 (*connect_result)(struct cfg_struct *cfg, struct hwifi_conn_result_stru *event);
    int32  (*disconnect_result)(struct cfg_struct *cfg, struct hwifi_disconn_result_stru *event);
    void  (*sta_join)(struct cfg_struct *cfg, struct hwifi_sta_join_stru *event);
    void  (*sta_leave)(struct cfg_struct *cfg, struct hwifi_sta_leave_stru *event);
    int32 (*report_frame)(struct cfg_struct *cfg, struct mgmt_report_event_stru *event);
    void  (*init_param_result)(struct cfg_struct *cfg, struct hwifi_init_res_stru *event);
    int32 (*recv_channel_info)(struct cfg_struct *cfg, int8 *buf, int32 len);

};

#define MHZ_TO_KHZ(freq) ((freq) * 1000)

/*
 * 3 Function Definition
 */
extern int32 hwifi_iface_process(struct hi110x_device *hi110x_dev, int32 state);


uint16 hwifi_get_connect_status(int32 status)
{
    uint16 connect_status;
    switch(status)
    {
        case UNSPECIFIED_FAILED:
             connect_status = WLAN_STATUS_UNSPECIFIED_FAILURE;
             break;
        case AUTH_FAILED:
             connect_status = WLAN_STATUS_AUTH_TIMEOUT;
             break;
        case ASOC_FAILED:
             connect_status = WLAN_STATUS_UNSPECIFIED_FAILURE;
             break;
        case CONN_SUCC:
             connect_status = WLAN_STATUS_SUCCESS;
             break;
        default:
             HWIFI_WARNING("the connect status(%d) is wrong!", status);
             connect_status = WLAN_STATUS_UNSPECIFIED_FAILURE;
             break;
    }

    return connect_status;
}



void hwifi_update_scan_time(struct cfg_struct *cfg, uint8 *bssid)
{
    struct scanned_bss_info     *bss;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != bssid);

    /* find scanned bss info */
    bss = find_scanned_bss_by_bssid(&cfg->scan.bss_mgmt, bssid);

    if (NULL == bss)
    {
        HWIFI_WARNING("Failed to find bss by bssid:"MACFMT,MAC2STR(bssid));
        put_scanned_bss(bss);
        return;
    }

    cfg->conn.bss.freq = bss->freq;
    HWIFI_DEBUG("CONNECT_RESULT : update scan time of ap:"MACFMT,
            MAC2STR(bssid));

    inform_single_beacon_frame(bss, cfg->wiphy);
    put_scanned_bss(bss);
}


/*
 * Prototype    : hwifi_connect_result
 * Description  : report connect result
 * Input        : struct cfg_struct *cfg
                  struct hwifi_conn_result_stru *event
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/28
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
STATIC int32 hwifi_connect_result(struct cfg_struct             *cfg,
                                        struct hwifi_conn_result_stru *event)
{
    uint16                       connect_status;
#ifdef __ROAM__
    enum crypto_type_enum crypto_type;
#endif

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    if (HWIFI_CONNECTING != cfg->conn.status)
    {
        HWIFI_WARNING("CONNECT_RESULT error : current status(%d) is not connecting!", cfg->conn.status);
        return -EFAIL;
    }

    /* get connect status for kernal */
    connect_status = hwifi_get_connect_status(event->connect_status);

    /* change connect status */
    if (WLAN_STATUS_SUCCESS == connect_status)
    {
        cfg->conn.status = HWIFI_CONNECTED;

        /* save connected BSS information*/
        memcpy(cfg->conn.bss.bssid,  event->bssid, MAC_LEN);
        cfg->conn.bss.stats.connect_ts = jiffies;
    }
    else
    {
        HWIFI_INFO("connect_status is %d, fail reason code is %d", event->connect_status, event->status_code);
        cfg->conn.status = HWIFI_DISCONNECTED;
        cfg->latest_signal = 0;
    }

    HWIFI_INFO("CONNECT_RESULT : %s, crypto_type=%u", connect_status ? "connect failed" : "connect success", 
                cfg->conn.bss.crypto_type);

    if(WLAN_STATUS_SUCCESS == connect_status)
    {
        /* update bss time */
        hwifi_update_scan_time(cfg, event->bssid);
    }

    /* report connect result to linux */
    cfg80211_connect_result(cfg->ndev,
                            event->bssid,
                            event->req_ie,
                            event->req_ie_len,
                            event->resp_ie,
                            event->resp_ie_len,
                            connect_status,
                            GFP_KERNEL);

    /* if AP country ie exist, regulatory domain rules update according to country ie country code */
    if (HWIFI_CONNECTED == cfg->conn.status)
    {
    #ifdef __ROAM__
        if (!(HWIFI_ENCRYPT & cfg->conn.bss.crypto_type))
        {
            crypto_type = CONNECTED_OPEN;
        }
        else if ((HWIFI_WEP104 | HWIFI_WEP) & cfg->conn.bss.crypto_type)
        {
            crypto_type = CONNECTED_WEP;
        }
        else if (HWIFI_TKIP & cfg->conn.bss.crypto_type)
        {
            crypto_type = CONNECTED_TKIP;
        }
        else if (HWIFI_CCMP & cfg->conn.bss.crypto_type)
        {
            crypto_type = CONNECTED_CCMP;
        }
        else if (HWIFI_WAPI & cfg->conn.bss.crypto_type)
        {
            crypto_type = CONNECTED_WAPI;
        }
        else
        {
            crypto_type = CONNECTED_OPEN;
        }

        roam_process_connect(&cfg->roam, crypto_type, cfg->conn.bss.band, cfg->conn.bss.channel);
    #endif

    #ifdef WLAN_ARRG_DYNAMIC_CONTROL
        hwifi_start_timer(cfg);
    #endif

#ifdef WLAN_POWER_MANAGEMENT
        if (g_powermgmt_switch)
        {
            /* 关联成功后，启动watchdog timer */
            if (cfg->hi110x_dev->pm_info != NULL)
            {
                hi110x_pm_start_wdg(cfg->hi110x_dev->pm_info);
            }
            else
            {
                HWIFI_WARNING("Invalid NULL cfg->hi110x_dev->pm_info!");
            }
        }
#endif
    }

    return SUCC;
}
STATIC int32 hwifi_disconnect_result(struct cfg_struct               *cfg,
                                           struct hwifi_disconn_result_stru *event)
{
    int32   ret;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    cfg->conn.status = HWIFI_DISCONNECTED;
    cfg->latest_signal = 0;
#ifdef __ROAM__
    roam_process_disconnect(&cfg->roam);
#endif

    HWIFI_INFO("device report disconnect, reason code:%u", event->reason_code);
    /* 解决device处理太慢导致去关联只扫描5G的问题 */
    msleep(100);
    /* report disconnect result */
    cfg80211_disconnected(cfg->ndev,
                          event->reason_code,
                          event->disconn_ie,
                          event->disconn_ie_len,
                          GFP_KERNEL);

    /*
     * if disconnect passively, restore previous regulatory domain rules before
     * connect.
     */
    strncpy(cfg->beacon_ie_country_code,INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN);
    ret = hwifi_regdomain_update(cfg);
    if (ret != SUCC)
    {
        HWIFI_WARNING("Failed to update regdomain!");
    }


#ifdef WLAN_ARRG_DYNAMIC_CONTROL
    hwifi_stop_timer(cfg);
#endif
    cancel_delayed_work_sync(&cfg->pwrm_set_work);
    hcc_net_work_detect_deinit(cfg->hi110x_dev->hcc);
    return SUCC;
}

/*
 * Prototype    : hwifi_sta_join
 * Description  :
 * Input        : struct cfg_struct  *cfg
 *                struct hwifi_sta_join_stru     *event
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/14
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC void  hwifi_sta_join(struct cfg_struct *cfg, struct hwifi_sta_join_stru *event)
{
    struct conn_sta_info *sta;
    struct station_info sinfo;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    sta = find_by_aid(&cfg->ap_info.sta_mgmt, event->aid);
    if (NULL == sta)
    {
        HWIFI_WARNING("STA_JOIN : can not find sta,aid=%d,mac="MACFMT, event->aid, MAC2STR(event->mac));
        return;
    }

    if (!event->success)
    {
        HWIFI_WARNING("Failed to send assoc response frame! aid=%d, mac="MACFMT, event->aid, MAC2STR(event->mac));
        remove_conn_sta(&cfg->ap_info.sta_mgmt, sta);

        return;
    }

    sta->stats.connect_ts = jiffies;

    /* send event to application */
    memset(&sinfo, 0, sizeof(sinfo));
    /* TODO: sinfo.generation */

    HWIFI_INFO("STA_JOIN : report added station to kernel,aid=%d,mac="MACFMT, event->aid, MAC2STR(event->mac));
    cfg80211_new_sta(cfg->ndev, sta->mac, &sinfo, GFP_KERNEL);

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
    hwifi_start_timer(cfg);
#endif
}

/*
 * Prototype    : hwifi_sta_leave
 * Description  :
 * Input        : struct cfg_struct  *cfg
 *                struct hwifi_sta_leave_stru    *event
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/14
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC void  hwifi_sta_leave(struct cfg_struct *cfg, struct hwifi_sta_leave_stru *event)
{
    struct conn_sta_info *sta;
    uint8  mac[MAC_LEN];

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    memset(mac, 0, MAC_LEN);

    sta = find_by_aid(&cfg->ap_info.sta_mgmt, event->aid);

    if (NULL != sta)
    {
        memcpy(mac, sta->mac, MAC_LEN);
        remove_conn_sta(&cfg->ap_info.sta_mgmt, sta);

        HWIFI_INFO("STA_LEAVE : report the station that leaves ap to the kernel,aid=%d,mac="MACFMT, event->aid, MAC2STR(mac));
        cfg80211_del_sta(cfg->ndev, mac, GFP_KERNEL);
    }
    else
    {
        HWIFI_WARNING("STA_LEAVE : failed to find the sta,aid=%d", event->aid);
    }
}


/*
 * Prototype    : hwifi_report_mgmt_frame
 * Description  : report mgmt frame
 * Input        : None
 * Output       : None
 * Return Value : SUCC or -EFAIL
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/16
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_report_mgmt_frame(struct cfg_struct *cfg, struct mgmt_report_event_stru *event)
{
    int32  ret;
    struct ieee80211_mgmt   *mgmt;

    if ((NULL == cfg) || (NULL == event) || (NULL == cfg->ndev) || (NULL == event->mgmt))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    mgmt = (struct ieee80211_mgmt *)(event->mgmt);



    if (IS_AP(cfg))
    {
        /* if receive assoc request frame or reassoc request frame,
         * add a sta_info, save assoc request IE in sta_info */
        if ((MGMT_FRAME_ASSOC_REQ == event->type)
        || (MGMT_FRAME_REASSOC_REQ == event->type))
        {
            struct conn_sta_info *sta;
            int ie_offset;
            int ie_len;
            uint8 *ie;

            sta = add_conn_sta(&cfg->ap_info.sta_mgmt, event->u.aid, mgmt->sa);
            if (NULL == sta)
            {
                HWIFI_WARNING("Ap failed to add conn STA by aid:%d,mac="MACFMT, event->u.aid, MAC2STR(mgmt->sa));
                return -EFAIL;
            }

            mgmt = (struct ieee80211_mgmt *)event->mgmt;
            if (MGMT_FRAME_ASSOC_REQ == event->type)
            {
                ie = (uint8*) mgmt->u.assoc_req.variable;
            }
            else
            {
                ie = (uint8*) mgmt->u.reassoc_req.variable;
            }

            ie_offset = (ie - (uint8*)mgmt);
            ie_len = event->mgmt_len - ie_offset;

            if (ie_len > MAX_REQ_IE_LEN)
            {
                HWIFI_WARNING("Invalid sta assoc req ie length is %d, longer than %d", ie_len, MAX_REQ_IE_LEN);
                ie_len = MAX_REQ_IE_LEN;
            }

            sta->assoc_req.ie_len = ie_len;
            memcpy(sta->assoc_req.ie, ie, ie_len);

            HWIFI_INFO("AP success add conn STA by aid:%d, mac:"MACFMT ",assoc_req ie_len:%d", event->u.aid, MAC2STR(mgmt->sa), ie_len);
            
        }
        /* GO mode should report management frame */
        if(!IS_P2P_ON(cfg))
        {
            return SUCC;
        }
    }

    /* report mgmt frame */
    ret = cfg80211_rx_mgmt(cfg->wdev, event->freq, 0, event->mgmt, event->mgmt_len, GFP_ATOMIC);
    if (TRUE == ret)
    {
        HWIFI_DEBUG("Report frame sucessful,type=%d,freq=%d,mgmt_len=%d, p2p mode is %d,current_mode=%d", 
                    event->type, event->freq, event->mgmt_len, IS_P2P_ON(cfg), cfg->hi110x_dev->mode.current_mode);
        return SUCC;
    }

    HWIFI_WARNING("The frame is rejected,type=%d,freq=%d,mgmt_len=%d, p2p mode is %d,current_mode=%d", 
                    event->type, event->freq, event->mgmt_len, IS_P2P_ON(cfg), cfg->hi110x_dev->mode.current_mode);

    return -EFAIL;
}


STATIC void hwifi_init_param_result(struct cfg_struct *cfg, struct hwifi_init_res_stru *event)
{
    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    /* get init_param result */
    if (SUCC == event->init_param_res)
    {
        HWIFI_INFO("Succeed to init!");
        cfg->init_flag = TRUE;

        complete(&cfg->init_done);
    }
    else
    {
        HWIFI_WARNING("Failed to init,init_param_res:%u", event->init_param_res);
        cfg->init_flag = FALSE;
    }



    return;
}
STATIC int32 hwifi_save_channel_info(struct cfg_struct *cfg, int8 *buf, int32 len)
{
    if ((NULL == cfg)||(NULL == buf))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    /* device信道信息上报，信道信息长度为2字节，其中低字节代表信道号 */
    if (len != 2)
    {
        HWIFI_WARNING("Invalid length:%d is not equeue to 2!",len);
        return -EFAIL;
    }

    cfg->ap_info.curr_channel = buf[0];

    HWIFI_DEBUG("Current channel:%d",cfg->ap_info.curr_channel);
    wake_up_interruptible(&cfg->wait_queue);

    return SUCC;
}
int32 hwifi_dev_ready_event(struct cfg_struct *cfg, struct hwifi_status_ready_stru *event)
{
    int32                   init_flag;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    /* get status result */
    if (SUCC != event->ready_status_res)
    {
        HWIFI_WARNING("Device is not ready");
        return -EFAIL;
    }

    /*wlan device is ready for data transfer*/
    set_bit(HI_STATUS_DEV_READY, &cfg->hi110x_dev->status);
    /*hcc_tx_transfer_lock(cfg->hi110x_dev->hcc);*/
    hcc_set_tx_queue_status(cfg->hi110x_dev->hcc, hcc_running);
    hcc_tx_queue_all_reset(cfg->hi110x_dev->hcc, TRANS_UNLOCK);
    hcc_tx_wake_queue(cfg->hi110x_dev->hcc, TRANS_UNLOCK);
    /*hcc_tx_transfer_unlock(cfg->hi110x_dev->hcc);*/

    init_flag = hwifi_init_hw_params(cfg);
    if( SUCC == init_flag )
    {
        HWIFI_DEBUG("hi110x init device param success");
    }else{
        HWIFI_WARNING("hi110x init device param failed");
        return -EFAIL;
    }



    return SUCC;
}

/* notice oprations implement  */
STATIC struct hwifi_inform_ops  cfg80211_inform_ops = {
    .connect_result     = hwifi_connect_result,
    .disconnect_result  = hwifi_disconnect_result,
    .sta_join           = hwifi_sta_join,
    .sta_leave          = hwifi_sta_leave,
    .report_frame       = hwifi_report_mgmt_frame,
    .init_param_result  = hwifi_init_param_result,
    .recv_channel_info  = hwifi_save_channel_info,

};

STATIC struct hwifi_inform_ops *g_inform_ops = &cfg80211_inform_ops;

/*
 * Prototype    : hwifi_reg_bw_fit
 * Description  : whether freq fits bandwith or not
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/11/27
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int8 hwifi_reg_bw_fit(const struct ieee80211_freq_range *freq_range,
                           uint32 center_freq_khz,
                           uint32 bw_khz)
{
    uint32 start_freq_khz, end_freq_khz;

    HWIFI_ASSERT(NULL != freq_range);

    start_freq_khz = center_freq_khz - (bw_khz/2);
    end_freq_khz = center_freq_khz + (bw_khz/2);

    if (start_freq_khz >= freq_range->start_freq_khz &&
        end_freq_khz <= freq_range->end_freq_khz)
        return TRUE;

    return FALSE;
}

/*
 * Prototype    : hwifi_freq_in_rule
 * Description  : whether freq is not in regdom rules or not
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/11/27
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_freq_in_rule(struct cfg_struct *cfg, uint32 center_freq_khz)
{
    int32 i, ret;
    int8 band_rule_found;

    HWIFI_ASSERT(NULL != cfg);

    for (i = 0; i < cfg->regdom->n_reg_rules; i++) {
        const struct ieee80211_reg_rule *rr;
        const struct ieee80211_freq_range *fr = NULL;
        band_rule_found = FALSE;

        rr = &cfg->regdom->reg_rules[i];
        fr = &rr->freq_range;

        /* if a frequency is in a frequency band */
#define ONE_GHZ_IN_KHZ  1000000
        if (abs(center_freq_khz - fr->start_freq_khz) <= (2 * ONE_GHZ_IN_KHZ))
        {
            band_rule_found = TRUE;
        }
        if (abs(center_freq_khz - fr->end_freq_khz) <= (2 * ONE_GHZ_IN_KHZ))
        {
            band_rule_found = TRUE;
        }
#undef ONE_GHZ_IN_KHZ

        /* fit the bandwidth rules */
        ret = hwifi_reg_bw_fit(fr, center_freq_khz, MHZ_TO_KHZ(20));

        if (ret && band_rule_found)
        {
            return SUCC;
        }
    }

    return -EFAIL;
}


int32 report_scanned_bss(struct cfg_struct *cfg, struct scanned_bss_event *event)
{
    struct scanned_bss_info  *bss;
    int32 ret = FALSE;
    uint32 freq_khz;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    freq_khz = MHZ_TO_KHZ(event->freq);
    ret = hwifi_freq_in_rule(cfg, freq_khz);

    if (SUCC != ret)
    {
        HWIFI_INFO("ap(freq: %d) is not in regdom rules", event->freq);
        return SUCC;
    }

    bss = alloc_scanned_bss(event->rssi, event->freq, event->mgmt, event->mgmt_len);

    if (bss)
    {
        update_bss_info(&cfg->scan.bss_mgmt, bss);
    }
    else
    {
        HWIFI_WARNING("alloc_scanned_bss failed, ap(freq: %d)", event->freq);
        return -EFAIL;
    }

    return SUCC;
}


int32 report_scan_done(struct cfg_struct *cfg, struct scan_complete_stru *event)
{
    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    cfg->scan.complete = TRUE;

    HWIFI_DEBUG("Wake up wait queue");
    wake_up_interruptible(&cfg->wait_queue);

    return SUCC;
}


int32 report_connect_event(struct cfg_struct *cfg, struct hwifi_conn_result_stru *event)
{
    int32 ret;
    ret = g_inform_ops->connect_result(cfg, event);
    return ret;
}


int32 report_disconnect_event(struct cfg_struct *cfg, struct hwifi_disconn_result_stru *event)
{
    g_inform_ops->disconnect_result(cfg, event);
    return SUCC;
}


int32 report_stats_result(struct cfg_struct *cfg, struct hwifi_stats_result_stru *event)
{
    struct stats_struct *stats;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    if (IS_STA(cfg))
    {
        stats = &cfg->conn.bss.stats;
    }
    else if (IS_AP(cfg))
    {
        struct conn_sta_info* sta;

        sta = find_by_mac(&cfg->ap_info.sta_mgmt, event->mac);

        if(NULL == sta)
        {
            HWIFI_WARNING("find_by_mac failed,mac:"MACFMT, MAC2STR(event->mac));
            return -EFAIL;
        }
        stats = &sta->stats;
    }
    else
    {
        HWIFI_WARNING("Current mode(%d) is wrong", cfg->hi110x_dev->mode.current_mode);
        return -EFAIL;
    }

    stats->rx_packets       = event->rx_packets;
    stats->tx_packets       = event->tx_packets;
    stats->rx_bytes         = event->rx_bytes;
    stats->tx_bytes         = event->tx_bytes;
    stats->signal           = event->signal;
    stats->tx_rate.flags = event->txrate.flags;
    stats->tx_rate.mcs = event->txrate.mcs;
    stats->tx_rate.legacy = event->txrate.legacy;
    stats->tx_retries       = event->tx_retries;
    stats->tx_failed        = event->tx_failed;
    stats->rx_dropped_misc  = event->rx_dropped_misc;

    stats->updated          = TRUE;
    HWIFI_DEBUG("report_stats_result:stats->tx_rate.legacy=%d,stats->signal = %d",
                  stats->tx_rate.legacy,stats->signal);
    if (IS_STA(cfg))
    {
        return SUCC;
    }

    wake_up_interruptible(&cfg->wait_queue);
    return SUCC;
}


int32 report_sta_join_event(struct cfg_struct *cfg, struct hwifi_sta_join_stru *event)
{
    g_inform_ops->sta_join(cfg, event);
    return SUCC;
}


int32 report_sta_leave_event(struct cfg_struct *cfg, struct hwifi_sta_leave_stru *event)
{
    g_inform_ops->sta_leave(cfg, event);
    return SUCC;
}


int32 report_change_mode_resp(struct cfg_struct *cfg, struct hwifi_set_mode_stru *event)
{
    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);

    /* get set_mode result */
    if ((cfg->hi110x_dev->mode.next_mode == event->hwifi_recv_mode)
            && (SUCC == event->set_mode_res))
    {
        cfg->hi110x_dev->mode.current_mode = cfg->hi110x_dev->mode.next_mode;
    }

    HWIFI_INFO("Report change_mode result,recv mode=%d, curr mode=%d", event->hwifi_recv_mode, cfg->hi110x_dev->mode.current_mode);
    wake_up(&cfg->wait_queue);
    return SUCC;
}
int32 report_set_param_resp(struct cfg_struct *cfg, struct hwifi_change_res_stru *event)
{
    HWIFI_DEBUG("Receive config param response");
    return SUCC;
}




/*
 * Prototype    : report_mgmt_frame
 * Description  :
 * Input        : None
 * Output       : None
 * Return Value : SUCC or -EFAIL
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/16
 *     Author       : Hash
 *     Modification : Created function
 *
 */
int32  report_mgmt_frame(struct cfg_struct *cfg, struct mgmt_report_event_stru *event)
{
    int32 ret;
    ret = g_inform_ops->report_frame(cfg, event);
    return ret;
}


int32 report_init_param_resp(struct cfg_struct *cfg, struct hwifi_init_res_stru *event)
{
    g_inform_ops->init_param_result(cfg, event);
    return SUCC;
}


int32 report_recv_channel_info(struct cfg_struct *cfg, int8 *buf, int32 len)
{
    g_inform_ops->recv_channel_info(cfg, buf, len);
    return SUCC;
}

#ifdef __ROAM__


int32 report_roamed_event(struct cfg_struct *cfg, struct hwifi_roamed_event *event)
{
    int32 freq;
    struct ieee80211_channel *channel;
    struct wiphy             *wiphy;
    enum ieee80211_band band;

    HWIFI_INFO("Roamed ap:" MACFMT, MAC2STR(event->bssid));
            
    /* save new info of new ap */
    memcpy(cfg->conn.bss.bssid,  event->bssid, MAC_LEN);
    /* notify roaming sm */
    roam_process_roamed(&cfg->roam, event->band, event->channel);
    /* update bss time */
    hwifi_update_scan_time(cfg, event->bssid);

    /* notify linux kernel */
    wiphy = cfg->wiphy;
    band = (ROAM_PARAM_BAND_2G == event->band)? IEEE80211_BAND_2GHZ : IEEE80211_BAND_5GHZ;
    freq = ieee80211_channel_to_frequency(event->channel, band);
    channel = ieee80211_get_channel(wiphy, freq);
    cfg80211_roamed(cfg->ndev, channel, event->bssid,
                    event->req_ie, event->req_ie_len,
                    event->resp_ie, event->resp_ie_len, GFP_KERNEL);
    return SUCC;
}
#endif

/*
 * Prototype    : hwifi_get_key_type
 * Description  : change from hwifi_key_type to kernel key_type
 * Input        : None
 * Output       : None
 * Return Value : enum nl80211_key_type
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/11
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
enum nl80211_key_type hwifi_get_key_type(int32 key_type)
{
    enum nl80211_key_type key;
    HWIFI_DEBUG("Get key type:%d",key_type);
    switch(key_type)
    {
        case HWIFI_KEYTYPE_GROUP:
            key = NL80211_KEYTYPE_GROUP;
            break;

        case HWIFI_KEYTYPE_PAIRWISE:
            key = NL80211_KEYTYPE_PAIRWISE;
            break;

        case HWIFI_KEYTYPE_PEERKEY:
            key = NL80211_KEYTYPE_PEERKEY;
            break;

        default:
            key = NUM_NL80211_KEYTYPES;
            break;
    }

    return key;
}

/*
 * Prototype    : report_mic_error_event
 * Description  : report mic error event to app
 * Input        : struct cfg_struct *cfg
                struct mic_error_report_stru *event
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/11
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 report_mic_error_event(struct cfg_struct *cfg, struct mic_error_report_stru *event)
{
    enum nl80211_key_type key_type;
    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != event);
    key_type = hwifi_get_key_type(event->key_type);
    cfg80211_michael_mic_failure(cfg->ndev,
                                 event->addr,
                                 key_type,
                                 event->key_id,
                                 NULL,
                                 GFP_KERNEL);
    return SUCC;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

