


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __ROAM__

/*
 * 1 Header File Including
 */
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <net/arp.h>
#include <linux/if_arp.h>
#include <linux/inetdevice.h>

#include "common.h"

#include "cfg80211_stru.h"
#include "hwifi_utils.h"
#include "hwifi_hcc.h"
#include "hwifi_cfg80211.h"
#include "hwifi_roam_main.h"



/*
 * 2 Global Variable Definition
 */
extern struct hwifi_freq_map FREQ_2G_MAP[];
extern struct hwifi_freq_map FREQ_5G_MAP[];
extern uint32 SIZE_FREQ_2G_MAP;
extern uint32 SIZE_FREQ_5G_MAP;

int32 g_roam_switch = 0;

int     g_trigger_rssi_threshold = ROAM_TRIGGER_RSSI_THRESHOLD;
uint32  g_trigger_count_threshold = ROAM_TRIGGER_COUNT_THRESHOLD;

/*
 * 3 Function Definition
 */



int32 roam_alg_param(struct cfg_struct* cfg, int param_cmd , int32 *param_value, int32 *get)
{
    int32 ret = SUCC;
    struct roam_all_stru *roam_info;

    HWIFI_ASSERT((NULL != cfg) && (NULL != param_value) && (NULL != get));

    roam_info = &cfg->roam;
    HWIFI_ASSERT(NULL != roam_info);

    *get = FALSE;
    if (IS_GET_ROAM_CMD(param_cmd))
    {
        *get = TRUE;
    }

    switch(param_cmd)
    {
        case ROAM_PARAM_CMD_COMMIT:
            /* set roam scanning params to device */
            roam_set_params(roam_info);
            break;

        /* SET */
        case ROAM_PARAM_CMD_SET_ON:
            if (*param_value)
            {
                roam_process_on(roam_info);
            }
            else
            {
                roam_process_off(roam_info);
            }
            break;

        case ROAM_PARAM_CMD_SET_FOREIGN_DEWELL:
            roam_info->alg_param.scan_param.single_user_foreign_dwell_time_ms = *param_value;
            break;

        case ROAM_PARAM_CMD_SET_HOME_DEWELL:
            roam_info->alg_param.scan_param.single_user_home_dwell_time_ms = *param_value;
            break;

        case ROAM_PARAM_CMD_SET_CONCURRENT_USER:
            roam_info->alg_param.scan_param.concurrent_user_number = *param_value;
            break;

        case ROAM_PARAM_CMD_SET_IS_ORTHOGONAL:
            roam_info->alg_param.scan_param.is_orthogonal = *param_value;
            hwifi_roam_update_channels_orthogonal_flag(roam_info);
            roam_update_scan_channels(roam_info);
            break;

        case ROAM_PARAM_CMD_SET_BAND:
            roam_info->alg_param.scan_param.band = *param_value;
            hwifi_roam_update_channels_band_flag(roam_info);
            roam_update_scan_channels(roam_info);
            break;

        case ROAM_PARAM_CMD_SET_TRIGGER_RSSI_THRESHOLD:
            roam_info->alg_param.trigger_param.trigger_rssi_threshold = *param_value;
            break;

        case ROAM_PARAM_CMD_SET_TRIGGER_COUNT_THRESHOLD:
            roam_info->alg_param.trigger_param.trigger_count_threshold = *param_value;
            break;

        case ROAM_PARAM_CMD_SET_PINGPONG_TIME_THRESHOL:
            roam_info->alg_param.execute_param.pingpong_expire_time_s = *param_value;
            break;
        case ROAM_PARAM_CMD_SET_PINGPONG_RSSI_THRESHOLD:
            roam_info->alg_param.execute_param.pingpong_rssi_threshold= *param_value;
            break;

        case ROAM_PARAM_CMD_SET_BLACKLIST_INVALID_TIME:
            roam_info->alg_param.execute_param.blacklist_expire_time_s = *param_value;
            break;

        case ROAM_PARAM_CMD_SET_GARP_SENT_COUNT:
            roam_info->alg_param.execute_param.down_route_learning_packet_number = *param_value;
            break;

        case ROAM_PARAM_CMD_SET_SINGLEUSER_THROUGHPUT_THRESHOLD:
            roam_info->alg_param.throughput_threshold_kbps = *param_value;
            break;

        /* GET */
        case ROAM_PARAM_CMD_GET_ON:
            *param_value = roam_info->roam_fsm.is_roam_on;
            break;

        case ROAM_PARAM_CMD_GET_FOREIGN_DEWELL:
            *param_value = roam_info->alg_param.scan_param.single_user_foreign_dwell_time_ms;
            break;

        case ROAM_PARAM_CMD_GET_HOME_DEWELL:
            *param_value = roam_info->alg_param.scan_param.single_user_home_dwell_time_ms;
            break;

        case ROAM_PARAM_CMD_GET_CONCURRENT_USER:
            *param_value = roam_info->alg_param.scan_param.concurrent_user_number;
            break;

        case ROAM_PARAM_CMD_GET_IS_ORTHOGONAL:
            *param_value = roam_info->alg_param.scan_param.is_orthogonal;
            break;

        case ROAM_PARAM_CMD_GET_BAND:
            *param_value = roam_info->alg_param.scan_param.band;
            break;

        case ROAM_PARAM_CMD_GET_TRIGGER_RSSI_THRESHOLD:
            *param_value = roam_info->alg_param.trigger_param.trigger_rssi_threshold;
            break;

        case ROAM_PARAM_CMD_GET_TRIGGER_COUNT_THRESHOLD:
            *param_value = roam_info->alg_param.trigger_param.trigger_count_threshold;
            break;

        case ROAM_PARAM_CMD_GET_PINGPONG_TIME_THRESHOL:
            *param_value = roam_info->alg_param.execute_param.pingpong_expire_time_s;
            break;
        case ROAM_PARAM_CMD_GET_PINGPONG_RSSI_THRESHOLD:
            *param_value = roam_info->alg_param.execute_param.pingpong_rssi_threshold;
            break;

        case ROAM_PARAM_CMD_GET_BLACKLIST_INVALID_TIME:
            *param_value = roam_info->alg_param.execute_param.blacklist_expire_time_s;
            break;

        case ROAM_PARAM_CMD_GET_GARP_SENT_COUNT:
            *param_value = roam_info->alg_param.execute_param.down_route_learning_packet_number;
            break;

        case ROAM_PARAM_CMD_GET_SINGLEUSER_THROUGHPUT_THRESHOLD:
            *param_value = roam_info->alg_param.throughput_threshold_kbps;
            break;

        default:
        break;
    }

    return ret;
}

int32 roam_need_data_buffering(struct cfg_struct* cfg, struct sk_buff *skb)
{
    struct ethhdr *ethhdr;
    struct roam_all_stru* roam;

    roam = &cfg->roam;

    if (!roam->is_buffering)
    {
        return FALSE;
    }

    ethhdr = (struct ethhdr *)skb->data;
    if ( htons(ETH_P_PAE) == ethhdr->h_proto)
    {
        HWIFI_INFO("EAPOL");
        return FALSE;
    }

    if ( htons(ETH_P_ARP) == ethhdr->h_proto)
    {
        HWIFI_INFO("ARP");
        return FALSE;
    }

    return TRUE;
}



int32 roam_data_buffering(struct cfg_struct* cfg, struct sk_buff *skb)
{
    struct roam_all_stru* roam;

    roam = &cfg->roam;
    skb_queue_tail(&roam->data_queue, skb);

    return SUCC;
}



void roam_send_garp(struct roam_all_stru* roam)
{
    struct cfg_struct *cfg;
    struct in_device *in_dev;
    struct in_ifaddr *ifa;
    uint8 loop;

    HWIFI_ASSERT((NULL != roam));

    cfg = roam_to_cfg(roam);
    HWIFI_ASSERT((NULL != cfg) && (NULL != cfg->hi110x_dev) && (NULL != cfg->hi110x_dev->netdev));

    in_dev = cfg->hi110x_dev->netdev->ip_ptr;

    if (NULL == in_dev)
    {
        HWIFI_INFO("roam_send_garp: in_dev is null.");
        return ;
    }

    HWIFI_INFO("roam_send_garp: netdev->flags = %u", cfg->hi110x_dev->netdev->flags);

    for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next)
    {
        for (loop = 0; loop < roam->alg_param.execute_param.down_route_learning_packet_number; loop++)
        {
            HWIFI_DEBUG("roam_send_garp: loop = %u, ifa_local = %u, dev_addr =" MACFMT,
                        loop, ifa->ifa_local, MAC2STR(cfg->hi110x_dev->netdev->dev_addr));

            arp_send(ARPOP_REQUEST, ETH_P_ARP,
                 ifa->ifa_local, cfg->hi110x_dev->netdev,
                 ifa->ifa_local, NULL,
                 cfg->hi110x_dev->netdev->dev_addr, NULL);
         }
    }

    return ;
}



void roam_start_buffering(struct roam_all_stru* roam)
{
    roam->is_buffering = TRUE;
}



void roam_stop_buffering(struct roam_all_stru* roam)
{
    roam->is_buffering = FALSE;
}



void roam_send_buffering(struct roam_all_stru* roam)
{
    queue_work(roam->wq, &roam->work);
}



void roam_clean_buffering(struct roam_all_stru* roam)
{
    skb_queue_purge(&roam->data_queue);
}



STATIC void send_buffering_work_func(struct work_struct *work)
{
    struct hcc_handler   *hcc;
    struct roam_all_stru *roam;

    roam = container_of(work, struct roam_all_stru, work);
    hcc = roam_to_cfg(roam)->hi110x_dev->hcc;

    while((!roam->is_buffering) && !skb_queue_empty(&roam->data_queue))
    {
        struct sk_buff     *skb;

        skb = skb_dequeue(&roam->data_queue);

        if (hcc_tx(hcc, HCC_TYPE_DATA, HCC_SUBTYPE_ANY, skb) == SUCC)
        {
            /**/
        }
        else
        {
            HWIFI_WARNING("Failed to send buff!");
            dev_kfree_skb_any(skb);
        }
    }
}

int32 roam_need_scan(struct roam_all_stru* roam)
{
    unsigned long now;
    unsigned long interval;

    now = jiffies;

    HWIFI_DEBUG("roam_scan_miss_count:%u,roam_scan_interval_s:%u,current state:%d,roam_failed_count:%d",
                roam->roam_fsm.roam_scan_miss_count, roam->roam_fsm.roam_scan_interval_s,
                roam->roam_fsm.state, roam->roam_fsm.roam_failed_count);

    if(roam->roam_fsm.roam_scan_miss_count > ROAM_SCAN_MAX_MISS_COUNT)
    {
        HWIFI_INFO("no need trigger scan: roam_scan_miss_count is %d(more than max:%d).",
                        roam->roam_fsm.roam_scan_miss_count, ROAM_SCAN_MAX_MISS_COUNT);
        return FALSE;

    }

    if(IN_ROAMING(roam))
    {
        HWIFI_INFO("no need trigger scan: in roaming.");
        return FALSE;
    }

    /* device连续一段时间没有收到beacon报文，直接触发漫游扫描 */
    if(roam->roam_fsm.force_trigger_scan_flag)
    {
        HWIFI_INFO("force trigger roam scan.");
        roam->roam_fsm.scan_ts = jiffies;
        return TRUE;
    }


    interval = (roam->alg_param.scan_param.concurrent_user_number) *
            (roam->alg_param.scan_param.single_user_home_dwell_time_ms
         + roam->alg_param.scan_param.single_user_foreign_dwell_time_ms);

    if(roam->alg_param.tx_pkt_param.tx_failed_trigger_flag)
    {
        /* 报文发送失败率过高导致触发漫游扫描 */
        if (time_before(now, roam->roam_fsm.scan_ts + msecs_to_jiffies(roam->alg_param.tx_pkt_param.time_interval * 1000)
                                    + msecs_to_jiffies(interval)))
        {
            HWIFI_INFO("No need trigger scan: scan interval is too short,time_interval=%d",
                        roam->alg_param.tx_pkt_param.time_interval);
            return FALSE;
        }
        roam->roam_fsm.scan_ts = jiffies;
        return TRUE;
    }


    /* roam_scan_interval_s是漫游扫描的间隔时间,间隔时间从扫描结束到下一次扫描开始的时间 */
    if (time_before(now, roam->roam_fsm.scan_ts + msecs_to_jiffies(roam->roam_fsm.roam_scan_interval_s*1000)
                                    + msecs_to_jiffies(interval)))
    {
        if((roam->roam_fsm.roam_failed_count > 0) && (roam->roam_fsm.roam_failed_count < ROAM_CONNECT_FAILED_COUNT_LIMIT))
        {
            /* 漫游失败后，允许立即再进行漫游扫描, 但只允许1次 */
            HWIFI_INFO("scan interval is too short,roam_scan_interval_s=%d, but roam_failed_count=%d,allow roam scan.",
                        roam->roam_fsm.roam_scan_interval_s, roam->roam_fsm.roam_failed_count);

            roam->roam_fsm.scan_ts = jiffies;
            return TRUE;
        }

        HWIFI_INFO("No need trigger scan: scan interval is too short,roam_scan_interval_s=%d,roam_failed_count=%d",
                        roam->roam_fsm.roam_scan_interval_s, roam->roam_fsm.roam_failed_count);
        return FALSE;
    }

    roam->roam_fsm.scan_ts = jiffies;

    return TRUE;
}



void roam_update_scan_channels(struct roam_all_stru* roam)
{
    struct cfg_struct *cfg;

    HWIFI_ASSERT(NULL != roam);

    cfg = roam_to_cfg(roam);
    HWIFI_ASSERT(NULL != cfg);

    spin_lock_bh(&roam->roam_fsm.lock);
    hwifi_roam_update_scan_channels(roam);
    spin_unlock_bh(&roam->roam_fsm.lock);
}



void roam_set_params(struct roam_all_stru* roam)
{
    struct cfg_struct *cfg;
    struct roam_cfg_stru cfg_param;

    HWIFI_ASSERT(NULL != roam);

    cfg = roam_to_cfg(roam);

    HWIFI_ASSERT(NULL != cfg);

    OS_MEM_SET(&cfg_param, 0, sizeof(struct roam_cfg_stru));

    cfg_param.cmdtype       = ROAM_CMD_TYPE_CFG;
    cfg_param.status        = roam->roam_fsm.is_roam_on;
    cfg_param.rssithreshold = roam->alg_param.trigger_param.trigger_rssi_threshold;
    cfg_param.triggercnt    = roam->alg_param.trigger_param.trigger_count_threshold;
    cfg_param.timeoutcnt    = ROAMING_MAX_TIMEOUT_CNT;

    HWIFI_INFO("Roam:%d, rssi_threshold:0x%x, count_threshold:%d, timeoutcnt:%d",
            cfg_param.status, cfg_param.rssithreshold, cfg_param.triggercnt, cfg_param.timeoutcnt);

    hwifi_start_roam_set_param(cfg, &cfg_param);

    return;
}

void roam_go_home_channel(struct roam_all_stru* roam, int band, int channel)
{
    struct cfg_struct *cfg;
    struct roam_stay_home_stru home_param;

    HWIFI_ASSERT(NULL != roam);

    cfg = roam_to_cfg(roam);

    HWIFI_ASSERT(NULL != cfg);

    OS_MEM_SET(&home_param, 0, sizeof(struct roam_stay_home_stru));

    home_param.cmdtype  = ROAM_CMD_TYPE_STAY_HOME;
    home_param.band     = band;
    home_param.channel  = channel;

    HWIFI_INFO("Band:%d, channel:%d\n", home_param.band, home_param.channel);

    hwifi_start_stay_home(cfg, &home_param);

    return;
}

void roam_go_foreign_channel(struct roam_all_stru* roam, int band, int channel)
{
    struct cfg_struct *cfg;
    struct roam_scan_cfg_ch_stru scan_param;

    HWIFI_ASSERT(NULL != roam);

    cfg = roam_to_cfg(roam);

    HWIFI_ASSERT(NULL != cfg);

    OS_MEM_SET(&scan_param, 0, sizeof(struct roam_scan_cfg_ch_stru));

    scan_param.cmdtype  = ROAM_CMD_TYPE_SINGLE_CH;
    scan_param.band     = band;
    scan_param.channel  = channel;

    HWIFI_INFO("Band:%d, channel:%d", scan_param.band, scan_param.channel);

    hwifi_start_roam_scan(cfg, &scan_param);

    return;
}

void roam_send_probe_req(struct roam_all_stru* roam)
{
    struct cfg_struct *cfg;
    struct roam_scan_probe_req_stru probe_param;

    HWIFI_ASSERT(NULL != roam);

    cfg = roam_to_cfg(roam);

    HWIFI_ASSERT(NULL != cfg);

    OS_MEM_SET(&probe_param, 0, sizeof(struct roam_scan_probe_req_stru));

    probe_param.cmdtype  = ROAM_CMD_TYPE_SEND_PROBE_REQ;

    hwifi_start_roam_send_probe_req(cfg, &probe_param);

    return;
}

int32 roam_try_roaming(struct roam_all_stru* roam, struct roam_candidate_ap_stru *candidate)
{
    struct cfg_struct *cfg;
    struct roam_roaming_stru roaming_param;
    int ret;

    HWIFI_ASSERT(NULL != roam);

    cfg = roam_to_cfg(roam);

    HWIFI_ASSERT(NULL != cfg);

    OS_MEM_SET(&roaming_param, 0, sizeof(struct roam_roaming_stru));

    roaming_param.cmdtype = ROAM_CMD_TYPE_SEL_NEW_AP;
    roaming_param.band    = candidate->band;
    roaming_param.channel = candidate->channel;
    roaming_param.length  = candidate->frame_len;

    OS_MEM_CPY(roaming_param.bssid, candidate->bssid, MAC_LEN);

    HWIFI_INFO("Band:%d, channel:%d, bssid:" MACFMT,
                roaming_param.band, roaming_param.channel,
                MAC2STR(roaming_param.bssid));

    ret = hwifi_start_roaming(cfg, &roaming_param, candidate->frame);

    return ret;
}

int32 roam_roaming_timeout(struct roam_all_stru* roam, uint8 *bssid)
{
    struct cfg_struct *cfg;
    struct roam_roaming_timeout_stru roam_timeout_param;
    int ret;

    HWIFI_ASSERT(NULL != roam);

    cfg = roam_to_cfg(roam);

    HWIFI_ASSERT(NULL != cfg);

    OS_MEM_SET(&roam_timeout_param, 0, sizeof(struct roam_roaming_timeout_stru));

    roam_timeout_param.cmdtype = ROAM_CMD_TYPE_SEL_NEW_AP_TIMEOUT;
    memcpy(roam_timeout_param.bssid, bssid, MAC_LEN);

    ret = hwifi_start_roaming_timeout(cfg, &roam_timeout_param);

    return ret;
}

int32 hwifi_roam_alg_param_init(roam_wlan_alg_param_t *pst_param)
{
    pst_param->scan_param.single_user_foreign_dwell_time_ms = ROAM_SINGLE_USER_FOREIGN_CHANNEL_DEWELL_TIME;
    pst_param->scan_param.single_user_home_dwell_time_ms    = ROAM_SINGLE_USER_HOME_CHANNEL_DEWELL_TIME;
    pst_param->scan_param.concurrent_user_number            = ROAM_CONCURRENT_USER_NUMBER;
    pst_param->scan_param.is_orthogonal                     = TRUE;
    pst_param->scan_param.band                              = ROAM_PARAM_BAND_2G;

    pst_param->trigger_param.trigger_rssi_threshold = g_trigger_rssi_threshold;     //ROAM_TRIGGER_RSSI_THRESHOLD;
    pst_param->trigger_param.trigger_count_threshold = g_trigger_count_threshold;   //ROAM_TRIGGER_COUNT_THRESHOLD;

    pst_param->tx_pkt_param.rssi_threshold = ROAM_TRIGGER_RSSI_DUE_TO_TX_FAILED;
    pst_param->tx_pkt_param.time_interval = ROAM_SCAN_INTERVAL_DUE_TO_TX_FAILED;
    pst_param->tx_pkt_param.tx_failed_scale_threshold = ROAM_TX_FAILED_SCALE_THRESHOLD;
    pst_param->tx_pkt_param.tx_packet_num_threshold = ROAM_TX_PACKET_NUM_THRESHOLD;
    pst_param->tx_pkt_param.tx_rate_threshold = ROAM_TX_RATE_THRESHOLD;

    pst_param->tx_pkt_param.record_count = 0;
    pst_param->tx_pkt_param.tx_failed_trigger_flag = 0;
    memset(pst_param->tx_pkt_param.tx_failed_pkt, 0 ,sizeof(pst_param->tx_pkt_param.tx_failed_pkt));
    memset(pst_param->tx_pkt_param.tx_success_pkt, 0 ,sizeof(pst_param->tx_pkt_param.tx_success_pkt));

    pst_param->execute_param.pingpong_expire_time_s = ROAM_PINGPONG_EXPIRE_TIME;
    pst_param->execute_param.pingpong_rssi_threshold = ROAM_PINGPONG_RSSI_THRESHOLD;
    pst_param->execute_param.blacklist_expire_time_s = ROAM_BLACKLIST_EXPIRE_TIME;
    pst_param->execute_param.down_route_learning_packet_number = ROAM_DOWN_ROUTE_LEARNING_PACKET_NUMBER;

    pst_param->throughput_threshold_kbps = 1000;

    pst_param->dot11b_table[0].rssi          = -94;
    pst_param->dot11b_table[0].capacity_kbps = 900;
    pst_param->dot11b_table[1].rssi          = -92;
    pst_param->dot11b_table[1].capacity_kbps = 4900;
    pst_param->dot11b_table[2].rssi          = -88;
    pst_param->dot11b_table[2].capacity_kbps = 8100;

    pst_param->dot11g_ofdm_table[0].rssi          = -90;
    pst_param->dot11g_ofdm_table[0].capacity_kbps = 5400;
    pst_param->dot11g_ofdm_table[1].rssi          = -82;
    pst_param->dot11g_ofdm_table[1].capacity_kbps = 17300;
    pst_param->dot11g_ofdm_table[2].rssi          = -75;
    pst_param->dot11g_ofdm_table[2].capacity_kbps = 29300;

    pst_param->ht_2G_ofdm_table[0].rssi          = -90;
    pst_param->ht_2G_ofdm_table[0].capacity_kbps = 5800;
    pst_param->ht_2G_ofdm_table[1].rssi          = -80;
    pst_param->ht_2G_ofdm_table[1].capacity_kbps = 35300;
    pst_param->ht_2G_ofdm_table[2].rssi          = -72;
    pst_param->ht_2G_ofdm_table[2].capacity_kbps = 58800;

    pst_param->dot11a_ofdm_table[0].rssi          = -90;
    pst_param->dot11a_ofdm_table[0].capacity_kbps = 5400;
    pst_param->dot11a_ofdm_table[1].rssi          = -82;
    pst_param->dot11a_ofdm_table[1].capacity_kbps = 17300;
    pst_param->dot11a_ofdm_table[2].rssi          = -75;
    pst_param->dot11a_ofdm_table[2].capacity_kbps = 29300;

    pst_param->ht20_5G_ofdm_table[0].rssi          = -90;
    pst_param->ht20_5G_ofdm_table[0].capacity_kbps = 5800;
    pst_param->ht20_5G_ofdm_table[1].rssi          = -80;
    pst_param->ht20_5G_ofdm_table[1].capacity_kbps = 35300;
    pst_param->ht20_5G_ofdm_table[2].rssi          = -72;
    pst_param->ht20_5G_ofdm_table[2].capacity_kbps = 58800;

    pst_param->ht40_5G_ofdm_table[0].rssi          = -87;
    pst_param->ht40_5G_ofdm_table[0].capacity_kbps = 11600;
    pst_param->ht40_5G_ofdm_table[1].rssi          = -77;
    pst_param->ht40_5G_ofdm_table[1].capacity_kbps = 70500;
    pst_param->ht40_5G_ofdm_table[2].rssi          = -75;
    pst_param->ht40_5G_ofdm_table[2].capacity_kbps = 128100;

    return SUCC;
}

void hwifi_roam_alg_param_exit(roam_wlan_alg_param_t *pst_param)
{
}


int  hwifi_roam_main_init(struct roam_all_stru *roam)
{
    struct cfg_struct *cfg;

    HWIFI_ASSERT(NULL != roam);

    memset(roam, 0, sizeof(struct roam_all_stru));

    cfg = roam_to_cfg(roam);

    cfg->conn.bss.mgmt_len = 0;
    cfg->conn.bss.mgmt = NULL;
    cfg->conn.bss.assoc_ie_len = 0;
    cfg->conn.bss.assoc_ie = NULL;

    roam->roam_fsm.min_diff_rssi_1 = 4;  /* 对应 <= ROAM_RSSI_THRESHOLD_1 -80 漫游rssi默认间隔 4db */
    roam->roam_fsm.min_diff_rssi_2 = 6;  /* 对应 <= ROAM_RSSI_THRESHOLD_2 -75 漫游rssi默认间隔 6db */
    roam->roam_fsm.min_diff_rssi_3 = 8;  /* 对应 <= ROAM_RSSI_THRESHOLD_3 -70 漫游rssi默认间隔 8db */
    roam->roam_fsm.min_diff_rssi_4 = 10; /* 对应 > ROAM_RSSI_THRESHOLD_3  -70 漫游rssi默认间隔 10db */

    roam->roam_fsm.min_scan_interval_1 = 10; /* 对应 <= ROAM_RSSI_THRESHOLD_1 -80 漫游扫描间隔 10s */
    roam->roam_fsm.min_scan_interval_2 = 15; /* 对应 <= ROAM_RSSI_THRESHOLD_2 -75 漫游扫描间隔 15s */
    roam->roam_fsm.min_scan_interval_3 = 20; /* 对应 <= ROAM_RSSI_THRESHOLD_3 -70 漫游扫描间隔 20s */
    roam->roam_fsm.min_scan_interval_4 = 60; /* 对应 > ROAM_RSSI_THRESHOLD_3  -70 漫游扫描间隔 60s */

    roam->roam_fsm.roam_scan_miss_count = 0;



    INIT_LIST_HEAD(&roam->ap_list.list);
    spin_lock_init(&roam->ap_list.list_lock);
    spin_lock_init(&roam->black_list.lock);
    spin_lock_init(&roam->ext_ap.lock);
    mutex_init(&roam->roam_scan_lock);

    hwifi_roam_alg_param_init(&roam->alg_param);

    roam->is_buffering = FALSE;
    skb_queue_head_init(&roam->data_queue);
    INIT_WORK(&roam->work, send_buffering_work_func);
    roam->wq = create_singlethread_workqueue("hi110x_roam");

    roam_fsm_init(&roam->roam_fsm);

    hwifi_roam_init_domain_channels(roam, FREQ_2G_MAP, SIZE_FREQ_2G_MAP, FREQ_5G_MAP, SIZE_FREQ_5G_MAP);

    roam->roam_fsm.is_roam_on = g_roam_switch;

    return SUCC;
}

void hwifi_roam_main_exit(struct roam_all_stru *roam)
{
    struct cfg_struct *cfg;

    HWIFI_ASSERT(NULL != roam);

    cfg = roam_to_cfg(roam);

    if(cfg->conn.bss.mgmt_len && (NULL != cfg->conn.bss.mgmt))
    {
        cfg->conn.bss.mgmt_len = 0;
        kfree(cfg->conn.bss.mgmt);
        cfg->conn.bss.mgmt = NULL;
    }

    if(cfg->conn.bss.assoc_ie_len && (NULL != cfg->conn.bss.assoc_ie))
    {
        cfg->conn.bss.assoc_ie_len = 0;
        kfree(cfg->conn.bss.assoc_ie);
        cfg->conn.bss.assoc_ie = NULL;
    }


    //TODO clean
    skb_queue_purge(&roam->data_queue);
    destroy_workqueue(roam->wq);

    roam_fsm_exit(&roam->roam_fsm);
    hwifi_roam_alg_param_exit(&roam->alg_param);

    /*  */
    hwifi_roam_clear(roam);
}

#endif /* __ROAM__ */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


