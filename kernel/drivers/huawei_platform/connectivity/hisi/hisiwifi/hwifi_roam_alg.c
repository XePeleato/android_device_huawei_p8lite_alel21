


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __ROAM__

/*
 * 1 Header File Including
 */
#include "common.h"
#include "hwifi_utils.h"
#include "hwifi_cfg80211.h"
#include "hwifi_roam_main.h"
#include "hwifi_roam_alg.h"

#include <linux/ieee80211.h>
#include <linux/synclink.h>
#include <linux/types.h>

/*
 * 2 Global Variable Definition
 */


/*
 * 3 Function Definition
 */

/*
 * Prototype    : hwifi_roam_candidate_ap_alloc
 * Description  : alloc mem for a new candidate ap
 * Input        : void
 * Output       : None
 * Return Value : the addr of new mem
 * Calls        : null
 * Called By    : hwifi_recv_roam_scan_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
struct roam_scan_candidate_ap_node * hwifi_roam_candidate_ap_alloc(void)
{
    return (struct roam_scan_candidate_ap_node *)OS_MALLOC_ATOMIC(sizeof(struct roam_scan_candidate_ap_node));
}


/*
 * Prototype    : hwifi_roam_candidate_ap_free
 * Description  : free mem of a candidate ap
 * Input        : the mem addr of a candidate ap
 * Output       : None
 * Return Value : void
 * Calls        : null
 * Called By    : hwifi_recv_roam_scan_report、hwifi_roam_clear
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_roam_candidate_ap_free(struct roam_scan_candidate_ap_node *ap_node)
{
    HWIFI_ASSERT(NULL != ap_node);

    OS_MEM_FREE(ap_node);

    return;
}


/*
 * Prototype    : hwifi_roam_candidate_ap_get
 * Description  : increase the user num of candidate ap to avoid the mem free by other thread
 * Input        : the mem addr of a candidate ap
 * Output       : None
 * Return Value : void
 * Calls        : null
 * Called By    : hwifi_recv_roam_scan_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_roam_candidate_ap_get(struct roam_scan_candidate_ap_node *ap_node)
{
    HWIFI_ASSERT(NULL != ap_node);

    spin_lock_bh(&ap_node->ap_node_lock);
    ap_node->user_num ++;
    spin_unlock_bh(&ap_node->ap_node_lock);
    return;
}


/*
 * Prototype    : hwifi_roam_candidate_ap_put
 * Description  : decrease the user num of candidate ap
 * Input        : the mem addr of a candidate ap
 * Output       : None
 * Return Value : void
 * Calls        : null
 * Called By    : hwifi_recv_roam_scan_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_roam_candidate_ap_put(struct roam_candidate_ap_stru *ap_info)
{

    struct roam_scan_candidate_ap_node *ap_node;

    HWIFI_ASSERT(NULL != ap_info);

    ap_node = (struct roam_scan_candidate_ap_node *)container_of(ap_info, struct roam_scan_candidate_ap_node, ap_info);

    spin_lock_bh(&ap_node->ap_node_lock);

    if (ap_node->user_num > 0)
    {
        ap_node->user_num --;

        spin_unlock_bh(&ap_node->ap_node_lock);

        return;
    }

    spin_unlock_bh(&ap_node->ap_node_lock);

    hwifi_roam_candidate_ap_free(ap_node);

    return;
}

/*
 * Prototype    : hwifi_roam_candidate_ap_add
 * Description  : add the candidate ap into the list of roam_info,
                : then update the candidate ap info according to the scan report msg.
 * Input        : roam_info : all info of roam
                : buf : scan report msg
                : candidate_ap : the mem addr of a candidate ap
 * Output       : None
 * Return Value : void
 * Calls        : null
 * Called By    : hwifi_recv_roam_scan_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_candidate_ap_add(struct roam_all_stru *roam_info, uint8 *buf, struct roam_scan_candidate_ap_node *candidate_ap)
{
    struct roam_scan_report_msg *msg;
    struct cfg_struct           *cfg;
    uint8                   *beacon_ie;
    uint32                   beacon_ie_len;
    uint8                   *ssid_ie;
    uint8                   *bssload_ie;
    int32                   utilization;
    int32                   calc_rssi;
    uint16                  sta_count;
    struct ieee80211_mgmt   *new_ap_beacon;

    HWIFI_ASSERT((NULL != roam_info) && (NULL != buf) && (NULL != candidate_ap));

    cfg = roam_to_cfg(roam_info);
    HWIFI_ASSERT(NULL != cfg);

    /* utilization初始化为255，如果不支持BSS LOAD IE，则对应的容量计算出来为0 */
    utilization = 255;
    sta_count   = 0;

    /* get scan result form WID msg */
    msg = (struct roam_scan_report_msg *)buf;

    /* 如果信道非法，直接返回，不处理 */
    if (msg->scan_report.band >= ROAM_PARAM_BAND_BUTT)
    {
        HWIFI_ERROR("Invalid. band = %d \n", msg->scan_report.band);
        return -EFAIL;
    }

    new_ap_beacon = (struct ieee80211_mgmt *)msg->scan_report.frame;

    /* get channel utilizationg in BSS LOAD IE */
    beacon_ie = new_ap_beacon->u.beacon.variable;
    beacon_ie_len = msg->scan_report.length - offsetof(struct ieee80211_mgmt, u.beacon.variable);

    /* filter the ap if ssid diamatch */
    ssid_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_SSID, beacon_ie, beacon_ie_len);
    if ((NULL != ssid_ie) && (hwifi_roam_ssid_is_dismatch(cfg, (*(ssid_ie + 1)), (ssid_ie + 2))))
    {
        HWIFI_ERROR("Ssid is dismatch.\n");
        return -EFAIL;
    }

    bssload_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_QBSS_LOAD, beacon_ie, beacon_ie_len);
    if (NULL != bssload_ie)
    {
        HWIFI_INFO("Bssload_ie is exist.\n");
        utilization = *(bssload_ie + 4);
        sta_count =   (*(bssload_ie + 2) & 0x00ff) + ((*(bssload_ie + 3) << 8) & 0xff00);
    }

    calc_rssi   = msg->scan_report.rssi - 255;
    candidate_ap->ap_info.rssi                   = calc_rssi & 0XFF;
    candidate_ap->ap_info.channel                = msg->scan_report.channel;
    candidate_ap->ap_info.band                   = msg->scan_report.band;
    candidate_ap->ap_info.channel_utilization    = utilization;
    candidate_ap->ap_info.sta_count              = sta_count;
    candidate_ap->ap_info.hw_mode                = hwifi_roam_judge_ap_hw_mode(msg->scan_report.length, msg->scan_report.frame, msg->scan_report.band);
    candidate_ap->ap_info.avail_channel_capacity = hwifi_roam_calc_avail_channel_capacity_to_utility(&cfg->roam, &candidate_ap->ap_info);

    memcpy(candidate_ap->ap_info.bssid, new_ap_beacon->bssid, MAC_LEN);

    HWIFI_DEBUG("New ap info1: bssid = "MACFMT"\n", MAC2STR(new_ap_beacon->bssid));
    HWIFI_DEBUG("New ap info2: rssi = %d, channel = %u, band = %u\n", candidate_ap->ap_info.rssi, candidate_ap->ap_info.channel, candidate_ap->ap_info.band);
    HWIFI_DEBUG("New ap info3: channel_utilization = %d, hw_mode = %u, avail_channel_capacity = %d\n", candidate_ap->ap_info.channel_utilization, candidate_ap->ap_info.hw_mode, candidate_ap->ap_info.avail_channel_capacity);

    candidate_ap->ap_info.frame_len = msg->scan_report.length;
    candidate_ap->ap_info.frame     = OS_MALLOC_ATOMIC(msg->scan_report.length);
    if (NULL == candidate_ap->ap_info.frame)
    {
        /* candidate_ap在外层失败处会释放 */
        HWIFI_ERROR("Failed to alloc ap_info.frame\n");
        return -EFAIL;
    }

    OS_MEM_CPY(candidate_ap->ap_info.frame, msg->scan_report.frame, msg->scan_report.length);

    /* add to the candidate ap list */
    spin_lock_bh(&roam_info->ap_list.list_lock);
    list_add(&candidate_ap->list, &roam_info->ap_list.list);
    spin_unlock_bh(&roam_info->ap_list.list_lock);

    return SUCC;
}



/*
 * Prototype    : hwifi_roam_candidate_ap_update
 * Description  : update the candidate ap info according to the scan report msg.
 * Input        : roam_info : all info of roam
                : buf : scan report msg
                : candidate_ap : the mem addr of a candidate ap
 * Output       : None
 * Return Value : void
 * Calls        : null
 * Called By    : hwifi_recv_roam_scan_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_candidate_ap_update(struct roam_all_stru *roam_info, uint8 *buf, struct roam_candidate_ap_stru *candidate_ap)
{
    struct roam_scan_report_msg *msg;
    struct cfg_struct           *cfg;
    uint8                   *beacon_ie;
    uint32                   beacon_ie_len;
    uint8                   *ssid_ie;
    uint8                   *bssload_ie;
    uint8                   *frame_tmp;
    int32                    utilization;
    uint16                   sta_count;
    int32                    report_rssi;
    int32                    calc_rssi;
    struct ieee80211_mgmt   *new_ap_beacon;
    struct roam_scan_candidate_ap_node *ap_node;

    HWIFI_ASSERT((NULL != roam_info) && (NULL != buf) && (NULL != candidate_ap));

    cfg = roam_to_cfg(roam_info);
    HWIFI_ASSERT(NULL != cfg);

    /* utilization初始化为255，如果不支持BSS LOAD IE，则对应的容量计算出来为0 */
    utilization = 255;
    sta_count   = 0;

    /* get scan result form WID msg */
    msg = (struct roam_scan_report_msg *)buf;


    ap_node = (struct roam_scan_candidate_ap_node *)container_of(candidate_ap, struct roam_scan_candidate_ap_node, ap_info);


    /* 如果信道非法，直接返回，不处理 */
    if (msg->scan_report.band >= ROAM_PARAM_BAND_BUTT)
    {
        HWIFI_ERROR("Invalid. band = %d \n", msg->scan_report.band);
        return -EFAIL;
    }

    new_ap_beacon = (struct ieee80211_mgmt *)msg->scan_report.frame;

    /* get channel utilizationg in BSS LOAD IE */
    beacon_ie = new_ap_beacon->u.beacon.variable;
    beacon_ie_len = msg->scan_report.length - offsetof(struct ieee80211_mgmt, u.beacon.variable);

    /* filter the ap if ssid diamatch */
    ssid_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_SSID, beacon_ie, beacon_ie_len);
    if ((NULL != ssid_ie) && (hwifi_roam_ssid_is_dismatch(cfg, (*(ssid_ie + 1)), (ssid_ie + 2))))
    {
        HWIFI_ERROR("Ssid is dismatch.\n");
        return -EFAIL;
    }

    bssload_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_QBSS_LOAD, beacon_ie, beacon_ie_len);
    if (NULL != bssload_ie)
    {
        utilization = *(bssload_ie + 4);
        sta_count =   (*(bssload_ie + 2) & 0x00ff) + ((*(bssload_ie + 3) << 8) & 0xff00);
    }


    spin_lock_bh(&ap_node->ap_node_lock);

    calc_rssi   = (candidate_ap->rssi -255 ) * 7 / 8;
    report_rssi = (msg->scan_report.rssi - 255)/8;
    candidate_ap->rssi                   = (report_rssi + calc_rssi) & 0xff;
    HWIFI_DEBUG("New ap info0: rssi = %d, cal_rssi = %d, report_rssi = %d\n", candidate_ap->rssi, calc_rssi, report_rssi);
    candidate_ap->channel                = msg->scan_report.channel;
    candidate_ap->band                   = msg->scan_report.band;
    candidate_ap->channel_utilization    = utilization;
    candidate_ap->sta_count              = sta_count;
    candidate_ap->hw_mode                = hwifi_roam_judge_ap_hw_mode(msg->scan_report.length, msg->scan_report.frame, msg->scan_report.band);
    candidate_ap->avail_channel_capacity = hwifi_roam_calc_avail_channel_capacity_to_utility(roam_info, candidate_ap);

    memcpy(candidate_ap->bssid, new_ap_beacon->bssid, MAC_LEN);

    HWIFI_DEBUG("New ap info1: bssid = "MACFMT"\n", MAC2STR(new_ap_beacon->bssid));
    HWIFI_DEBUG("New ap info2: rssi = %d, channel = %u, band = %u\n", candidate_ap->rssi, candidate_ap->channel, candidate_ap->band);
    HWIFI_DEBUG("New ap info3: channel_utilization = %d, hw_mode = %u, avail_channel_capacity = %d\n", candidate_ap->channel_utilization, candidate_ap->hw_mode, candidate_ap->avail_channel_capacity);

    /* 先申请新内存, 如果申请失败则不更新beacon/probe_resp帧 */
    frame_tmp = OS_MALLOC_ATOMIC(msg->scan_report.length);
    if (NULL == frame_tmp)
    {
        HWIFI_ERROR("Failed to alloc tmp frame!");
        spin_unlock_bh(&ap_node->ap_node_lock);
        return -EFAIL;
    }

    /* 申请成功后，先释放原有beacon/probe_resp帧，再更新帧 */
    if (NULL != candidate_ap->frame)
    {
        OS_MEM_FREE(candidate_ap->frame);
    }

    candidate_ap->frame_len = msg->scan_report.length;
    candidate_ap->frame     = frame_tmp;
    OS_MEM_CPY(candidate_ap->frame, msg->scan_report.frame, msg->scan_report.length);

    spin_unlock_bh(&ap_node->ap_node_lock);

    return SUCC;

}


/*
 * Prototype    : hwifi_roam_judge_ap_hw_mode
 * Description  : judge the hw mode of the candidate ap info according to the beacon/probe response which
                : reported in the sanning process.
 * Input        : frame_len : length of the beacon/probe response
                : frame : addr of the beacon/probe response
                : band : band of the ap
 * Output       : None
 * Return Value : hw_mode : hw_mode of ap
 * Calls        : null
 * Called By    : hwifi_roam_candidate_ap_add、hwifi_roam_candidate_ap_update
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
uint32 hwifi_roam_judge_ap_hw_mode(uint16 frame_len, uint8 *frame, uint8 band)
{
    struct ieee80211_mgmt *ap_beacon;
    uint8 *beacon_ie;
    uint16 beacon_capability;
    uint32 hw_mode;
    uint8 *ht_cap_ie;
    uint16 ht_cap_info;
    uint16 beacon_ie_len;

    HWIFI_ASSERT(NULL != frame);

    hw_mode = 0;
    ap_beacon = (struct ieee80211_mgmt *)frame;

    /* get channel utilizationg in BSS LOAD IE */
    beacon_ie = ap_beacon->u.beacon.variable;
    beacon_ie_len = frame_len - offsetof(struct ieee80211_mgmt, u.beacon.variable);

    /* filter the ap if ssid diamatch */
    ht_cap_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_HT_CAPABILITY, beacon_ie, beacon_ie_len);

    /* 说明支持802.11n, 此处只区分信道带宽，不区分频段 */
    if (NULL != ht_cap_ie)
    {
        ht_cap_info = *(ht_cap_ie + 2);

        /* 说明支持20M信道 */
        if (0 == (ht_cap_info & BIT1))
        {
            HWIFI_DEBUG(": AP support HT 20M. ht_cap_info = %u\n", ht_cap_info);
            hw_mode = ROAM_PROTOCOL_11N_20;
        }
        else /* 40M */
        {
            HWIFI_DEBUG(": AP support HT 40M. ht_cap_info = %u\n", ht_cap_info);
            hw_mode = ROAM_PROTOCOL_11N_40;
        }

        /* 说明已经支持802.11n, 其他能力不再判断，直接返回 */
        return hw_mode;
    }

    beacon_capability = OS_LE16_TO_CPU(ap_beacon->u.beacon.capab_info);
    /* 判断频段及DSSS-OFDM能力: 1)11G:2G频段且支持OFDM  2)11B:2G频段且不支持OFDM  */
    /* 2)11A:5G频段                                                               */
    if ((band == ROAM_PARAM_BAND_2G) && ((0 != (beacon_capability & BIT13))))
    {
        HWIFI_DEBUG(": AP support 802.11G. beacon_capability = %u\n", beacon_capability);
        hw_mode = ROAM_PROTOCOL_11G;
    }
    else if ((band == ROAM_PARAM_BAND_2G) && ((0 == (beacon_capability & BIT13))))
    {
        HWIFI_DEBUG(": AP support 802.11B. beacon_capability = %u\n", beacon_capability);
        hw_mode = ROAM_PROTOCOL_11B;
    }
    else if(band == ROAM_PARAM_BAND_5G)
    {
        HWIFI_DEBUG(": AP support 802.11A. beacon_capability = %u\n", beacon_capability);
        hw_mode = ROAM_PROTOCOL_11A;
    }

    return hw_mode;
}

/*
 * Prototype    : hwifi_roam_lookup_capacity_by_rssi
 * Description  : lookup capacity according to the rssi
 * Input        : pst_rssi_table : the table of capacity with different level of rssi
                : rssi : rssi of ap
 * Output       : None
 * Return Value : the capacity according to the rssi
 * Calls        : null
 * Called By    : hwifi_roam_rssi_mapping_capacity
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_lookup_capacity_by_rssi(roam_rssi_capacity_stru *pst_rssi_table, int rssi)
{
    uint32 rssi_loop;
    uint32 rssi_level_idx;

    HWIFI_ASSERT(NULL != pst_rssi_table);

    rssi_level_idx = ROAM_RSSI_LEVEL;

    for(rssi_loop = 0; rssi_loop < ROAM_RSSI_LEVEL; rssi_loop++)
    {
        if (rssi >= pst_rssi_table[rssi_loop].rssi)
        {
            rssi_level_idx = rssi_loop;
        }
    }

    if (rssi_level_idx >= ROAM_RSSI_LEVEL)
    {
        return 0;
    }

    return pst_rssi_table[rssi_level_idx].capacity_kbps;
}

/*
 * Prototype    : hwifi_roam_rssi_mapping_capacity
 * Description  : lookup capacity according to hw mode and rssi of ap
 * Input        : pst_roam_info : the all info of roam
                : pst_ap_info : the candidate ap
 * Output       : None
 * Return Value : the capacity
 * Calls        : null
 * Called By    : hwifi_roam_calc_avail_channel_capacity_to_utility
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_rssi_mapping_capacity(struct roam_all_stru *pst_roam_info, struct roam_candidate_ap_stru *pst_ap_info)
{
    int32 capacity = 0;

    HWIFI_ASSERT((NULL != pst_roam_info) && (NULL != pst_ap_info));

    /* 说明当前STA可支持802.11n 40M带宽的最大能力，因此只要判断AP能力即可 */
    switch (pst_ap_info->hw_mode)
    {
        case ROAM_PROTOCOL_11B:
            capacity = hwifi_roam_lookup_capacity_by_rssi(pst_roam_info->alg_param.dot11b_table, (pst_ap_info->rssi - 255));
            break;

        case ROAM_PROTOCOL_11G:
            capacity = hwifi_roam_lookup_capacity_by_rssi(pst_roam_info->alg_param.dot11g_ofdm_table, (pst_ap_info->rssi - 255));

            break;

        case ROAM_PROTOCOL_11A:
           capacity = hwifi_roam_lookup_capacity_by_rssi(pst_roam_info->alg_param.dot11a_ofdm_table, (pst_ap_info->rssi - 255));
           break;

        case ROAM_PROTOCOL_11N_20:
           capacity = hwifi_roam_lookup_capacity_by_rssi(pst_roam_info->alg_param.ht_2G_ofdm_table, (pst_ap_info->rssi - 255));
           break;

        /*
        case ROAM_PROTOCOL_11N_20:
           capacity = hwifi_roam_lookup_capacity_by_rssi(pst_roam_info->alg_param.ht20_5G_ofdm_table, pst_ap_info->rssi);
           break;
        */

        case ROAM_PROTOCOL_11N_40:
            capacity = hwifi_roam_lookup_capacity_by_rssi(pst_roam_info->alg_param.ht40_5G_ofdm_table, (pst_ap_info->rssi - 255));
            break;

        default:
            break;
    }

    return capacity;
}


/*
 * Prototype    : hwifi_roam_calc_avail_channel_capacity_to_utility
 * Description  : caculate the channel capacity according to channel utilization
 * Input        : pst_roam_info : the all info of roam
                : pst_ap_info : the candidate ap
 * Output       : None
 * Return Value : the channel capacity
 * Calls        : null
 * Called By    : hwifi_roam_candidate_ap_add、hwifi_roam_candidate_ap_update
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_calc_avail_channel_capacity_to_utility(struct roam_all_stru *pst_roam_info, struct roam_candidate_ap_stru *pst_ap_info)
{
    int32 capacity = 0;
    int32 avail_channel_capacity = 0;

    HWIFI_ASSERT((NULL != pst_roam_info) && (NULL != pst_ap_info));

    /* 根据协议模式(pst_ap_info->hw_mode)、以及rssi值获得RSSI对应典型容量 */
    capacity = hwifi_roam_rssi_mapping_capacity(pst_roam_info, pst_ap_info);

    avail_channel_capacity = capacity * (255 - pst_ap_info->channel_utilization) / 255 / ROAM_CONCURRENT_USER_NUMBER;

    HWIFI_INFO("hw_mode = %d, rssi = %d, capacity = %d, avail_channel_capacity = %d \n", pst_ap_info->hw_mode, (pst_ap_info->rssi - 255), capacity, avail_channel_capacity);

    return avail_channel_capacity;
}

/*
 * Prototype    : hwifi_roam_ssid_is_dismatch
 * Description  : judge the input ssid whether it is not matched the ssid of current connected ap
 * Input        : cfg : addr of hi1101 cfg
                : ssid_len : length of ssid
                : ssid : content of ssid
 * Output       : None
 * Return Value : TRUE/FALSE
 * Calls        : null
 * Called By    : hwifi_recv_roam_scan_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
bool hwifi_roam_ssid_is_dismatch(struct cfg_struct *cfg, uint8 ssid_len, uint8 *ssid)
{
    HWIFI_ASSERT((NULL != cfg) && (NULL != ssid));

    if (cfg->conn.bss.ssid_len != ssid_len)
    {
        return TRUE;
    }

    /* SSID不匹配 */
    if (0 != OS_MEM_CMP(cfg->conn.bss.ssid, ssid, ssid_len))
    {
        return TRUE;
    }

    return FALSE;
}

uint8 * wpa_bss_get_vendor_ie(uint8 *beacon_ie, uint32 beacon_ie_len, uint32 vendor_type)
{
	uint8 *end, *pos;

	pos = beacon_ie;
	end = pos + beacon_ie_len;

	while (pos + 1 < end)
	{
		if (pos + 2 + pos[1] > end)
		{
			break;
		}
		if (pos[0] == WLAN_EID_VENDOR_SPECIFIC && pos[1] >= 4 &&
		    vendor_type == WPA_GET_BE32(&pos[2]))
			return pos;
		pos += 2 + pos[1];
	}

	return NULL;
}

STATIC int32 get_wpa_cipher_suite(uint8 *ie, int32 ie_len, roam_wpa_cipher_suite_t *data)
{
    uint8 *pos;
    int32  left;
    int32  i;
    int32  count;

    if ((NULL == ie) || (NULL == data))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    /* pos前移8个字节，信息元素id 1个字节，长度1个字节，oui 4个字节，version 2个字节*/
    pos = ie + 8;
    left = ie_len - 6;  /* ie_len不包括信息元素id 1个字节，长度1个字节 */

    if (left >= WPA_SELECTOR_LEN)
    {
        /* 组播密钥解析 */
        if (RSN_SELECTOR_GET(pos) == WPA_CIPHER_SUITE_TKIP)
        {
            data->group_crypto = HWIFI_TKIP;
        }
        else if(RSN_SELECTOR_GET(pos) == WPA_CIPHER_SUITE_CCMP)
        {
            data->group_crypto = HWIFI_CCMP;
        }
        else
        {
            HWIFI_WARNING("Invalid not support cipher,group crypto parse failed.");
            return -EFAIL;
        }
        pos += WPA_SELECTOR_LEN;
        left -= WPA_SELECTOR_LEN;
    }
    else
    {
        HWIFI_WARNING("Invalid too short left len(%d),group cipher parse failed ",left);
        return -EFAIL;
    }

    if (left >= 2)
    {
         /* 单播密钥解析 */
        count = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * WPA_SELECTOR_LEN)
        {
            HWIFI_WARNING("Invalid too short left len(%d),pairwise key parse failed. ",left);
            return -EFAIL;
        }

        for (i = 0; i < count; i++)
        {
            /* 单播密钥解析 */
            if (RSN_SELECTOR_GET(pos) == WPA_CIPHER_SUITE_TKIP)
            {
                data->pairwise_crypto |= HWIFI_TKIP;
            }
            else if (RSN_SELECTOR_GET(pos) == WPA_CIPHER_SUITE_CCMP)
            {
                data->pairwise_crypto |= HWIFI_CCMP;
            }
            else
            {
                HWIFI_WARNING("Invalid not support cipher!");
                return -EFAIL;
            }
            pos += WPA_SELECTOR_LEN;
            left -= WPA_SELECTOR_LEN;
        }
    }
    else
    {
        HWIFI_WARNING("Invalid too left len(%d),group cipher parse failed ",left);
        return -EFAIL;
    }

    if (left >= 2)
    {
        /* 认证类型解析 */
        count = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * WPA_SELECTOR_LEN)
        {
            HWIFI_WARNING("Invalid too short left len(%d),AKM parse failed ",left);
            return -EFAIL;
        }
        for (i = 0; i < count; i++)
        {
            HWIFI_INFO("auth key management,type:%x",RSN_SELECTOR_GET(pos));
            if (RSN_SELECTOR_GET(pos) == WPA_AUTH_KEY_MGMT_UNSPEC_802_1X)
            {
                data->auth_type |= HWIFI_8021X;
            }
            else if(RSN_SELECTOR_GET(pos) == WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X)
            {
                HWIFI_INFO("auth key management is WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X");
            } else {
                HWIFI_INFO("Invalid not support auth key management,type:%x",RSN_SELECTOR_GET(pos));
            }
            pos += WPA_SELECTOR_LEN;
            left -= WPA_SELECTOR_LEN;
        }
    }
    else
    {
        HWIFI_WARNING("Invalid too short left len(%d),group cipher parse failed ",left);
        return -EFAIL;
    }

    return SUCC;
}

STATIC int32 get_rsn_cipher_suite(uint8 *ie, int32 ie_len, roam_wpa_cipher_suite_t *data)
{
    uint8 *pos;
    int32 left;
    int32 i;
    int32 count;

    if ((NULL == ie) || (NULL == data))
    {
        HWIFI_WARNING("Invalid NULL param!");
        return -EFAIL;
    }

    pos = (ie + 2);  /* skip element id,and element len */
    left = ie_len;

    /* skip version,2bytes */
    pos += 2;
    left -= 2;

    if (left >= RSN_SELECTOR_LEN)
    {
        /* 组播密钥解析 */
        if (RSN_SELECTOR_GET(pos) == RSN_CIPHER_SUITE_TKIP)
        {
            data->group_crypto = HWIFI_TKIP;
        }
        else if(RSN_SELECTOR_GET(pos) == RSN_CIPHER_SUITE_CCMP)
        {
            data->group_crypto = HWIFI_CCMP;
        }
        else
        {
            HWIFI_WARNING("Invalid not support cipher!");
            return -EFAIL;
        }
        pos += RSN_SELECTOR_LEN;
        left -= RSN_SELECTOR_LEN;
    }
    else
    {
        HWIFI_WARNING("parse group crypto failed,left len(%d) is too short", left);
        return -EFAIL;
    }

    if (left >= 2)
    {
        data->pairwise_crypto = 0;
        /* 单播密钥个数 */
        count = WPA_GET_LE16(pos);

        pos += 2;
        left -= 2;

        if (count == 0 || left < count * RSN_SELECTOR_LEN)
        {
            HWIFI_WARNING("Invalid too short left len:(%d) and zero count!%d",left, count);
            return -EFAIL;
        }

        for (i = 0; i < count; i++)
        {
            /* 单播密钥解析 */
            if (RSN_SELECTOR_GET(pos) == RSN_CIPHER_SUITE_TKIP)
            {
                data->pairwise_crypto |= HWIFI_TKIP;
            }
            else if(RSN_SELECTOR_GET(pos) == RSN_CIPHER_SUITE_CCMP)
            {
                data->pairwise_crypto |= HWIFI_CCMP;
            }
            else
            {
                HWIFI_WARNING("Invalid not support cipher type:%d",RSN_SELECTOR_GET(pos));
                return -EFAIL;
            }

            pos += RSN_SELECTOR_LEN;
            left -= RSN_SELECTOR_LEN;
        }

    }
    else
    {
        HWIFI_WARNING("parse pairwise crypto failed,left len(%d) is too short", left);
        return -EFAIL;
    }


    if (left >= 2)
    {
        /* 解析认证类型 WPA-PSK or WPA-EAP */
        data->auth_type = 0;

        /* 支持的认证类型个数 */
        count = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;

        if (count == 0 || left < count * RSN_SELECTOR_LEN)
        {
            HWIFI_WARNING("Invalid too short left len(%d),AKM parse failed",left);
            return -EFAIL;
        }
        for (i = 0; i < count; i++)
        {
            HWIFI_INFO("auth type,%x",RSN_SELECTOR_GET(pos));
            if (RSN_SELECTOR_GET(pos) == RSN_AUTH_KEY_MGMT_UNSPEC_802_1X)
            {
                data->auth_type |= HWIFI_8021X;
            }
            else if(RSN_SELECTOR_GET(pos) == RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X)
            {
                HWIFI_INFO("auth type is RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X!");
            } else {
                HWIFI_INFO("Invalid auth type,%x",RSN_SELECTOR_GET(pos));
            }
            pos += RSN_SELECTOR_LEN;
            left -= RSN_SELECTOR_LEN;
        }

    }
    else
    {
        HWIFI_WARNING("parse auth type failed,left len(%d) is too short.", left);
        return -EFAIL;
    }

    return SUCC;
}


bool hwifi_roam_wpa_rsn_ie_is_mismatch(struct cfg_struct *cfg, int32 beacon_ie_len, uint8 *beacon_ie)
{
    uint8 *curr_wpa_ie = NULL;
    uint8 *curr_rsn_ie = NULL;
    uint8 curr_wpa_ie_len = 0;
    uint8 curr_rsn_ie_len = 0;

    uint8 *new_ap_wpa_ie = NULL;
    uint8 *new_ap_rsn_ie = NULL;
    uint8 new_ap_wpa_ie_len = 0;
    uint8 new_ap_rsn_ie_len = 0;

    uint8 *curr_beacon_ie = NULL;
    int32 curr_beacon_ie_len = 0;

    roam_wpa_cipher_suite_t curr_ap_cipher_suite;
    roam_wpa_cipher_suite_t new_ap_cipher_suite;
    int32 ret;

    if((NULL == cfg) ||(NULL == beacon_ie))
    {
        HWIFI_WARNING("argument cfg or beacon_ie is null.");
        return TRUE;
    }

    if(!cfg->conn.bss.mgmt_len || (NULL == cfg->conn.bss.mgmt))
    {
        HWIFI_WARNING("there is no beacon frame in connected ap info.");
        return TRUE;
    }

    curr_beacon_ie = cfg->conn.bss.mgmt->u.beacon.variable;
    curr_beacon_ie_len = cfg->conn.bss.mgmt_len - offsetof(struct ieee80211_mgmt, u.beacon.variable);

    curr_wpa_ie = wpa_bss_get_vendor_ie(curr_beacon_ie, curr_beacon_ie_len, WPA_IE_VENDOR_TYPE);
    if(NULL != curr_wpa_ie)
    {
        curr_wpa_ie_len = curr_wpa_ie[1];
    }

    curr_rsn_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_RSN, curr_beacon_ie, curr_beacon_ie_len);
    if(NULL != curr_rsn_ie)
    {
        curr_rsn_ie_len = curr_rsn_ie[1];
    }

    new_ap_wpa_ie = wpa_bss_get_vendor_ie(beacon_ie, beacon_ie_len, WPA_IE_VENDOR_TYPE);
    if(NULL != new_ap_wpa_ie)
    {
        new_ap_wpa_ie_len = new_ap_wpa_ie[1];
    }

    new_ap_rsn_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_RSN, beacon_ie, beacon_ie_len);
    if(NULL != new_ap_rsn_ie)
    {
        new_ap_rsn_ie_len = new_ap_rsn_ie[1];
    }

    HWIFI_INFO("curr_wpa_ie_len=%u,curr_rsn_ie_len=%u,new_ap_wpa_ie_len=%u,new_ap_rsn_ie_len=%u",
                curr_wpa_ie_len, curr_rsn_ie_len, new_ap_wpa_ie_len, new_ap_rsn_ie_len);

    if((curr_wpa_ie_len != new_ap_wpa_ie_len) || (curr_rsn_ie_len != new_ap_rsn_ie_len))
    {
        return TRUE;
    }

    /* 最少都需要大于8个字节 */
    if(curr_wpa_ie_len > 8)
    {
        /* WPA信息元素id 1个字节，长度1个字节，oui 4个字节，version 2个字节,只比较加密方式,前8个字节不比较 */
        memset(&curr_ap_cipher_suite, 0, sizeof(curr_ap_cipher_suite));
        memset(&new_ap_cipher_suite, 0, sizeof(new_ap_cipher_suite));
        ret = get_wpa_cipher_suite(curr_wpa_ie, curr_wpa_ie_len, &curr_ap_cipher_suite);
        ret |= get_wpa_cipher_suite(new_ap_wpa_ie, new_ap_wpa_ie_len, &new_ap_cipher_suite);

        HWIFI_INFO("current wpa cipher_suite,auth_type:%d,group_crypto=%d,pairwise_crypto=%d", 
                    curr_ap_cipher_suite.auth_type, curr_ap_cipher_suite.group_crypto, curr_ap_cipher_suite.pairwise_crypto);
        HWIFI_INFO("current wpa cipher_suite,auth_type:%d,group_crypto=%d,pairwise_crypto=%d", 
                    new_ap_cipher_suite.auth_type, new_ap_cipher_suite.group_crypto, new_ap_cipher_suite.pairwise_crypto);            

        if(ret != SUCC)
        {
            HWIFI_INFO("get_wpa_cipher_suite failed."); 
            return TRUE;
        }
        
        if(0 != OS_MEM_CMP(&curr_ap_cipher_suite, &new_ap_cipher_suite,  sizeof(curr_ap_cipher_suite))) 
        {
           HWIFI_INFO("the new ap wpa ie does not match with IE in current connect ap."); 
           return TRUE;
        }
    }

    /* 最少都需要大于6个字节，前4个字节，后面2个字节的能力信息 */
    if(curr_rsn_ie_len > 6)
    {
        memset(&curr_ap_cipher_suite, 0, sizeof(curr_ap_cipher_suite));
        memset(&new_ap_cipher_suite, 0, sizeof(new_ap_cipher_suite));
        ret = get_rsn_cipher_suite(curr_rsn_ie, curr_rsn_ie_len, &curr_ap_cipher_suite);
        ret = get_rsn_cipher_suite(new_ap_rsn_ie, new_ap_rsn_ie_len, &new_ap_cipher_suite);

        HWIFI_INFO("current rsn cipher_suite,auth_type:%d,group_crypto=%d,pairwise_crypto=%d", 
                    curr_ap_cipher_suite.auth_type, curr_ap_cipher_suite.group_crypto, curr_ap_cipher_suite.pairwise_crypto);
        HWIFI_INFO("current rsn cipher_suite,auth_type:%d,group_crypto=%d,pairwise_crypto=%d", 
                    new_ap_cipher_suite.auth_type, new_ap_cipher_suite.group_crypto, new_ap_cipher_suite.pairwise_crypto); 
        
        if(ret != SUCC)
        {
            HWIFI_INFO("get_rsn_cipher_suite failed."); 
            return TRUE;
        }
        
        if(0 != OS_MEM_CMP(&curr_ap_cipher_suite, &new_ap_cipher_suite,  sizeof(new_ap_cipher_suite))) 
        {
           HWIFI_INFO("the new ap rsn ie does not match with IE in current connect ap."); 
           return TRUE;
        }
    }

    return FALSE;
}
bool hwifi_roam_wapi_ie_is_mismatch(struct cfg_struct *cfg, int32 beacon_ie_len, uint8 *beacon_ie)
{
    uint8 *curr_wapi_ie = NULL;
    uint8 curr_wapi_ie_len = 0;

    uint8 *new_ap_wapi_ie = NULL;
    uint8 new_ap_wapi_ie_len = 0;

    int32 akm_num = 0;
    int32 pair_num = 0;
    int32 group_num = 0; 

    uint8 *curr_beacon_ie = NULL;
    int32 curr_beacon_ie_len = 0;

    if((NULL == cfg) ||(NULL == beacon_ie))
    {
        HWIFI_WARNING("argument cfg or beacon_ie is null.");
        return TRUE;
    }
    

    if(!cfg->conn.bss.mgmt_len || (NULL == cfg->conn.bss.mgmt))
    {
        HWIFI_WARNING("there is no beacon frame in connected ap info.");
        return TRUE;
    }
    
    curr_beacon_ie = cfg->conn.bss.mgmt->u.beacon.variable;
    curr_beacon_ie_len = cfg->conn.bss.mgmt_len - offsetof(struct ieee80211_mgmt, u.beacon.variable);

     /*
     * WAPI 信息元素
     * 属性名称:   元素标  长度  版本  AKM套   AKM套件      单播密钥  单播密      组播密        WAPI能  BKID  BKID
     *             识ID                件计数               套件计数  码套件      钥套件        力信息  计数  列表
     * 八位位组数:  1       1     2     2       4*m            2       4*n          4             2       2    16*s
     * 数      值: 0X44     22  (1,0)  (1,0) (0,0x14,0x72,1) (1,0) (0,0X14,0X72,1)(0,0X14,0X72,1)(0,0)  (0,0)
     *
     */
    curr_wapi_ie = (uint8 *)cfg80211_find_ie(0x44, curr_beacon_ie, curr_beacon_ie_len);
    if(NULL != curr_wapi_ie)
    {
        curr_wapi_ie_len = curr_wapi_ie[1];
    }

    new_ap_wapi_ie = (uint8 *)cfg80211_find_ie(0x44, beacon_ie, beacon_ie_len);
    if(NULL != new_ap_wapi_ie)
    {
        new_ap_wapi_ie_len = new_ap_wapi_ie[1];
    }
    
    
    HWIFI_INFO("curr_wapi_ie = %u, new_ap_wapi_ie=%u.", curr_wapi_ie_len, new_ap_wapi_ie_len);

    if(curr_wapi_ie_len != new_ap_wapi_ie_len)
    {
        return TRUE;
    }

    if(6 < curr_wapi_ie_len)
    {
        akm_num = WPA_GET_LE16(curr_wapi_ie + 4);
        if(curr_wapi_ie_len > (4 + 2 + (4 * akm_num) + 2))
        {
            pair_num = WPA_GET_LE16(curr_wapi_ie + (4 + 2 + (4 * akm_num)));
        }
        else
        {
            HWIFI_WARNING("curr_wapi_ie_len is too short:%u. can not calc pair_num", curr_wapi_ie_len); 
            return TRUE;
        }
        
        if(curr_wapi_ie_len > (4 + 2 + (4 * akm_num) + 2 + (4 * pair_num) + 2))
        {
           group_num = WPA_GET_LE16(curr_wapi_ie + (4 + 2 + (4 * akm_num) + 2 + (4 * pair_num)));
        }
        else
        {
            HWIFI_WARNING("curr_wapi_ie_len is too short:%u. can not calc group_num", curr_wapi_ie_len); 
            return TRUE;
        }

        if(curr_wapi_ie_len > (2 + 4 * akm_num + 2 + 4 * pair_num + 2 + 4 * group_num))
        {
            /* 只比较wapi加密信息套件，不比较能力信息以及BKID等信息 */
            if(0 != OS_MEM_CMP(curr_wapi_ie + 4, new_ap_wapi_ie + 4, (2 + 4 * akm_num + 2 + 4 * pair_num + 2 + 4 * group_num)))
            {
               HWIFI_INFO("the new ap wapi ie does not match with IE in current connect ap."); 
               return TRUE;
            }
        }
        else
        {
            HWIFI_WARNING("curr_wapi_ie_len is error:%u.", curr_wapi_ie_len); 
            return TRUE;
        }
    }

    return FALSE;   
}

bool hwifi_roam_open_ie_is_mismatch(struct cfg_struct *cfg, int32 beacon_len, uint8 *beacon)
{
    struct ieee80211_mgmt *curr_mgmt;
    struct ieee80211_mgmt *new_ap_mgmt;

    uint16 curr_beacon_capability;
    uint16 new_ap_beacon_capability;

    if((NULL == cfg) ||(NULL == beacon))
    {
        HWIFI_WARNING("argument cfg or beacon is null.");
        return TRUE;
    }
    
    
    if(!cfg->conn.bss.mgmt_len || (NULL == cfg->conn.bss.mgmt))
    {
        HWIFI_WARNING("there is no beacon frame in connected ap info,cfg->conn.bss.mgmt_len=%u", cfg->conn.bss.mgmt_len);
        return TRUE;
    }
    
    curr_mgmt = cfg->conn.bss.mgmt;
    new_ap_mgmt = (struct ieee80211_mgmt *)beacon;

    
    curr_beacon_capability = OS_LE16_TO_CPU(curr_mgmt->u.beacon.capab_info);
    new_ap_beacon_capability = OS_LE16_TO_CPU(new_ap_mgmt->u.beacon.capab_info);

    
    HWIFI_INFO("curr_beacon_capability = 0x%.2x, new_ap_beacon_capability = 0x%.2x", 
                curr_beacon_capability, new_ap_beacon_capability);
    
    if((WLAN_CAPABILITY_PRIVACY & curr_beacon_capability)
      && (!(WLAN_CAPABILITY_PRIVACY & new_ap_beacon_capability)))
    {
        return TRUE;
    }

    
    if((!(WLAN_CAPABILITY_PRIVACY & curr_beacon_capability))
      && ((WLAN_CAPABILITY_PRIVACY & new_ap_beacon_capability)))
    {
        return TRUE;
    }
    
    return FALSE;
}


bool hwifi_roam_ie_is_mismatch(struct cfg_struct *cfg, uint8 *new_beacon_frame, int32 new_beacon_len)
{
    struct ieee80211_mgmt   *new_ap_beacon;
    uint8                   *beacon_ie;
    int32                   beacon_ie_len;
    
    if((NULL == cfg) ||(NULL == new_beacon_frame))
    {
        HWIFI_WARNING("argument cfg or new_beacon_frame is null.");
        return TRUE;
    }

    new_ap_beacon = (struct ieee80211_mgmt *)new_beacon_frame;
    beacon_ie = new_ap_beacon->u.beacon.variable;
    beacon_ie_len = new_beacon_len - offsetof(struct ieee80211_mgmt, u.beacon.variable);

    if(beacon_ie_len <= 0)
    {
        HWIFI_WARNING("beacon_ie_len(%d) is too short.", beacon_ie_len);
        return TRUE;
    }
    
    /* wep加密不漫游，无法区别加密index */
    if(HWIFI_AUTHTYPE_SHARED_KEY == cfg->conn.bss.auth_type) 
    {
        HWIFI_INFO("WEP SHARED method,dismatch");
        return TRUE;
    }
    if((HWIFI_AUTHTYPE_OPEN_SYSTEM == cfg->conn.bss.auth_type) 
        && ((HWIFI_WEP | HWIFI_WEP104) & cfg->conn.bss.crypto_type))
    {
        HWIFI_INFO("WEP OPEN method,dismatch");
        return TRUE;
    }

    if(hwifi_roam_open_ie_is_mismatch(cfg, new_beacon_len, new_beacon_frame))
    {
        return TRUE;
    }
    
    if(hwifi_roam_wpa_rsn_ie_is_mismatch(cfg, beacon_ie_len, beacon_ie))
    {
        return TRUE;
    }

    if(hwifi_roam_wapi_ie_is_mismatch(cfg, beacon_ie_len, beacon_ie))
    {
        return TRUE;
    }

    return FALSE;
}


/*
 * Prototype    : hwifi_roam_is_current_assoc_ap
 * Description  : judge the bssid whether it is matched the bssid of current connected ap
 * Input        : roam : the all info of roam
                : bssid : addr of bssid
 * Output       : None
 * Return Value : TRUE/FALSE
 * Calls        : null
 * Called By    : hwifi_roam_select_candidate_ap
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
bool hwifi_roam_is_current_assoc_ap(struct roam_all_stru *roam, uint8 *bssid)
{
    struct cfg_struct *cfg;

    HWIFI_ASSERT((NULL != roam) && (NULL != bssid));

    cfg = roam_to_cfg(roam);
    HWIFI_ASSERT(NULL != cfg);

    /* bSSID匹配 */
    if (0 == OS_MEM_CMP(cfg->conn.bss.bssid, bssid, MAC_LEN))
    {
        return TRUE;
    }

    return FALSE;
}

/*
 * Prototype    : hwifi_roam_find_candidate_ap_by_bssid
 * Description  : find candidate ap by bssid
 * Input        : ap_list : the list of all candidate ap
                : bssid : addr of bssid
 * Output       : None
 * Return Value : the addr of candidate ap
 * Calls        : null
 * Called By    : hwifi_recv_roam_scan_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
struct roam_candidate_ap_stru* hwifi_roam_find_candidate_ap_by_bssid(struct roam_scan_candidate_ap_list *ap_list, uint8* bssid)
{
    struct roam_scan_candidate_ap_node *candidate_ap;
    struct list_head                   *list, *list_t;

    if((NULL == ap_list) || (NULL == bssid))
    {
        return NULL;
    }

    candidate_ap = NULL;

    spin_lock_bh(&ap_list->list_lock);

    list_t = list = &ap_list->list;

    while(list->next != &ap_list->list)
    {
        list = list->next;

        if(NULL == list)
        {
            list_t->next = list_t;
            list_t->prev = list_t;
            spin_unlock_bh(&ap_list->list_lock);
            HWIFI_WARNING("ap_list null recover");
            return NULL;
        }

        candidate_ap = list_entry(list, struct roam_scan_candidate_ap_node, list);
        if ((NULL!= candidate_ap) && (0 == OS_MEM_CMP(bssid, candidate_ap->ap_info.bssid, MAC_LEN)))
        {
            /* 先增加引用计数再返回 */
            hwifi_roam_candidate_ap_get(candidate_ap);
            spin_unlock_bh(&ap_list->list_lock);
            return &candidate_ap->ap_info;
        }
    }

    spin_unlock_bh(&ap_list->list_lock);

    return NULL;
}

/*
 * Prototype    : hwifi_roam_is_in_ap_blacklist
 * Description  : judge the ap whether it is already in ap black list
 * Input        : ap_bssid : bssid of ap
                : roam_info : all info of roam
 * Output       : None
 * Return Value : TRUE/FALSE
 * Calls        : null
 * Called By    : hwifi_roam_select_candidate_ap
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
bool hwifi_roam_is_in_ap_blacklist(struct roam_all_stru *roam_info, uint8 *ap_bssid )
{
    uint32 blacklist_loop;
    uint8  bssid_zero[MAC_LEN] = {0};

    HWIFI_ASSERT((NULL != ap_bssid) && (NULL != roam_info));

    /* 先更新黑名单，再查找是否在黑名单中 */
    hwifi_roam_update_blacklist(roam_info, NULL);

    spin_lock_bh(&roam_info->black_list.lock);

    for (blacklist_loop = 0; blacklist_loop < ROAM_BLACKLIST_AP_NUMBER; blacklist_loop++)
    {
        if (0 != OS_MEM_CMP(bssid_zero, roam_info->black_list.blacks[blacklist_loop].bssid, MAC_LEN))
        {
            HWIFI_DEBUG("Bssid of ap is in blacklist = "MACFMT". timestamp = %lu. blacklist count = %d \n", MAC2STR(roam_info->black_list.blacks[blacklist_loop].bssid),
                    roam_info->black_list.blacks[blacklist_loop].time_stamp,
                    roam_info->black_list.blacks[blacklist_loop].count);
        }

        if (0 == OS_MEM_CMP(ap_bssid, roam_info->black_list.blacks[blacklist_loop].bssid, MAC_LEN))
        {
            /* 在黑名单中的次数大于等于ROAM_BLACKLIST_COUNT_LIMIT(2次)才不进行选择关联 */
            if(roam_info->black_list.blacks[blacklist_loop].count >= ROAM_BLACKLIST_COUNT_LIMIT)
            {
                spin_unlock_bh(&roam_info->black_list.lock);
                return TRUE;
            }
            else
            {
                spin_unlock_bh(&roam_info->black_list.lock);
                return FALSE;
            }
            
        }
    }

    spin_unlock_bh(&roam_info->black_list.lock);
    return FALSE;
}

/*
 * Prototype    : hwifi_roam_is_pingpong
 * Description  : judge roam process whether it is in pingpong
 * Input        : ap_bssid : bssid of ap
                : roam_info : all info of roam
 * Output       : None
 * Return Value : TRUE/FALSE
 * Calls        : null
 * Called By    : hwifi_roam_select_candidate_ap
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
bool hwifi_roam_is_pingpong(struct roam_all_stru *roam_info, uint8 *ap_bssid)
{
    uint32        ap_loop;
    unsigned long now;
    unsigned long pingpong_time;
    uint8  bssid_zero[MAC_LEN] = {0};

    HWIFI_ASSERT((NULL != ap_bssid) && (NULL != roam_info));

    /* 先更新黑名单，再查找是否在黑名单中 */
    hwifi_roam_update_ext_ap(roam_info, NULL);

    now = jiffies;

    pingpong_time = (unsigned long)roam_info->alg_param.execute_param.pingpong_expire_time_s;


    spin_lock_bh(&roam_info->ext_ap.lock);
    for (ap_loop = 0; ap_loop < ROAM_EXT_AP_NUMBER; ap_loop++)
    {
        if (0 != OS_MEM_CMP(bssid_zero, roam_info->ext_ap.ext_ap[ap_loop].bssid, MAC_LEN))
        {
            HWIFI_DEBUG("Bssid of ap is in ext ap list = "MACFMT". timestamp = %lu\n", MAC2STR(roam_info->ext_ap.ext_ap[ap_loop].bssid), roam_info->ext_ap.ext_ap[ap_loop].time_stamp);
        }


        if ((0 == OS_MEM_CMP(ap_bssid, roam_info->ext_ap.ext_ap[ap_loop].bssid, MAC_LEN))
            && (time_before(now, (roam_info->ext_ap.ext_ap[ap_loop].time_stamp + pingpong_time * HZ))))
        {
            spin_unlock_bh(&roam_info->ext_ap.lock);
            return TRUE;
        }
    }

    spin_unlock_bh(&roam_info->ext_ap.lock);
    return FALSE;
}
int32 calc_min_diff_rssi(struct roam_all_stru *pst_roam_info, int32 rssi_current)
{
    int32 min_diff_rssi = 0;
    
    if(rssi_current <= ROAM_RSSI_THRESHOLD_1)
    {
        /* 当前AP的信号强度在-80及以上（小于等于-80）， 新AP与旧AP默认漫游门限差值为>= 4db */
        min_diff_rssi = pst_roam_info->roam_fsm.min_diff_rssi_1;
    }
    else if(rssi_current <= ROAM_RSSI_THRESHOLD_2)
    {
        /* 当前AP的信号强度在-75及以上（小于等于-75），新AP与旧AP默认漫游门限差值为>= 6db */
        min_diff_rssi = pst_roam_info->roam_fsm.min_diff_rssi_2;
    }
    else if(rssi_current <= ROAM_RSSI_THRESHOLD_3)
    {
        /* 当前AP的信号强度在-70及以上（小于等于-70），新AP与旧AP默认漫游门限差值为>= 8 */
        min_diff_rssi = pst_roam_info->roam_fsm.min_diff_rssi_3;
    }
    else
    {
        /* 当前AP信号强度高于-70 新AP与旧AP默认漫游门限差值为>= 10 */
        min_diff_rssi = pst_roam_info->roam_fsm.min_diff_rssi_4;
    }
    HWIFI_DEBUG("rssi_current=%d,min_diff_rssi=%d", rssi_current, min_diff_rssi);
    return min_diff_rssi;
}


int32 calc_scan_interval(struct roam_all_stru *pst_roam_info, int32 rssi_current)
{
    int32 min_scan_interval = 0;
    
    if(rssi_current <= ROAM_RSSI_THRESHOLD_1)
    {
        /* 当前AP的信号强度在-80及以上（小于等于-80）漫游扫描间隔10s */
        min_scan_interval = pst_roam_info->roam_fsm.min_scan_interval_1;
    }
    else if(rssi_current <= ROAM_RSSI_THRESHOLD_2)
    {
        /* 当前AP的信号强度在-75及以上（小于等于-75）漫游扫描间隔15s */
        min_scan_interval = pst_roam_info->roam_fsm.min_scan_interval_2;
    }
    else if(rssi_current <= ROAM_RSSI_THRESHOLD_3)
    {
        /* 当前AP的信号强度在-70及以上（小于等于-70）漫游扫描间隔20s */
        min_scan_interval = pst_roam_info->roam_fsm.min_scan_interval_3;
    }
    else
    {
        /* 当前AP信号强度高于-70 漫游扫描间隔60s */
        min_scan_interval = pst_roam_info->roam_fsm.min_scan_interval_4;
    }
    HWIFI_DEBUG("rssi_current=%d,min_scan_interval=%d", rssi_current, min_scan_interval);
    return min_scan_interval;
}

/*
 * Prototype    : hwifi_roam_select_candidate_ap
 * Description  : select the best candidate ap from the candidate ap list
 * Input        : roam_info : all info of roam
 * Output       : None
 * Return Value : candidate ap
 * Calls        : null
 * Called By    : roam_scanning_handle_timeout
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
struct roam_candidate_ap_stru *hwifi_roam_select_candidate_ap(struct roam_all_stru *pst_roam_info)
{
    struct cfg_struct *cfg;
    struct roam_scan_candidate_ap_node *max_capacity_ap;
    struct roam_scan_candidate_ap_node *max_rssi_ap;
    struct roam_scan_candidate_ap_node *candidate_ap_result;
    struct roam_scan_candidate_ap_node *each_ap;
    struct list_head *list;
    int32  capacity = 0;
    int32  ap_rssi = 0;
    int32  max_rssi = 0;
    int32  roam_scan_candidate_ap_count = 0;   /* 记录可供漫游选择的AP的数目 */
    int32 rssi_current; 
    int32 min_diff_rssi = 0;
    HWIFI_ASSERT(NULL != pst_roam_info);

    max_capacity_ap = NULL;
    max_rssi_ap     = NULL;
    each_ap         = NULL;
    candidate_ap_result = NULL;

    spin_lock_bh(&pst_roam_info->ap_list.list_lock);

    list = &pst_roam_info->ap_list.list;

    if (list->next == list)
    {
        HWIFI_INFO("Candidate empty,roam_scan_miss_count=%u, roam_scan_interval_s=%u",
                    pst_roam_info->roam_fsm.roam_scan_miss_count, pst_roam_info->roam_fsm.roam_scan_interval_s);

        pst_roam_info->roam_fsm.roam_scan_miss_count++;

        pst_roam_info->roam_fsm.force_trigger_scan_flag = 0;

        pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 0;

        spin_unlock_bh(&pst_roam_info->ap_list.list_lock);

        return NULL;
    }

    /* 遍历每个ap */
    while (list->next != &pst_roam_info->ap_list.list)
    {
        list = list->next;

        each_ap = list_entry(list, struct roam_scan_candidate_ap_node, list);

        if (NULL == each_ap)
        {
            continue;
        }

        /* 排除在黑名单中的AP */
        if (TRUE == hwifi_roam_is_in_ap_blacklist(pst_roam_info, each_ap->ap_info.bssid))
        {
            HWIFI_INFO("ap is in blacklist. bssid = "MACFMT".\n", MAC2STR(each_ap->ap_info.bssid));
            continue;
        }

        /* ssid不匹配的AP已在scan report时滤除，不必再考虑 */

        /* 找出avail_channel_capacity最大者AP */
        if (NULL == max_capacity_ap)
        {
            capacity = each_ap->ap_info.avail_channel_capacity;
            max_capacity_ap = each_ap;
        }
        else if (each_ap->ap_info.avail_channel_capacity >= capacity)
        {
            capacity = each_ap->ap_info.avail_channel_capacity;
            max_capacity_ap = each_ap;
        }

        /* 找出RSSI最大者AP */
        ap_rssi = each_ap->ap_info.rssi;
        if (NULL == max_rssi_ap)
        {
            max_rssi = ap_rssi;
            max_rssi_ap = each_ap;
        }
        else if (ap_rssi > max_rssi)
        {
            max_rssi    = ap_rssi;
            max_rssi_ap = each_ap;
        }
        roam_scan_candidate_ap_count++;
    }


    HWIFI_DEBUG("roam_scan_miss_count=%u, roam_scan_interval_s=%u, roam_scan_candidate_ap_count=%d", 
                    pst_roam_info->roam_fsm.roam_scan_miss_count, 
                    pst_roam_info->roam_fsm.roam_scan_interval_s, 
                    roam_scan_candidate_ap_count);
    if(NULL != max_rssi_ap)
    {
        if (hwifi_roam_is_current_assoc_ap(pst_roam_info, max_rssi_ap->ap_info.bssid)
            &&(1 == roam_scan_candidate_ap_count))
        {
            /* 如果附近查找到的相同ssid的AP是自己且个数为1 */
            pst_roam_info->roam_fsm.roam_scan_miss_count++;
        }
        else
        {
            /* 如果附近存在相同ssid的AP，且不在黑名单中，则打开漫游扫描 */
            pst_roam_info->roam_fsm.roam_scan_miss_count = 0;
        }
    }
    else
    {
        /* 如果附近存在相同ssid的AP，但都在黑名单中，则按长周期30s间隔扫描 */
        pst_roam_info->roam_fsm.roam_scan_miss_count++;
    }

    /* 满足容量需求中，取RSSI最大者; 在不满足容量的网络中，选择RSSI最大者。*/
    if ((NULL != max_capacity_ap) && (max_capacity_ap->ap_info.avail_channel_capacity >= pst_roam_info->alg_param.throughput_threshold_kbps))
    {
        HWIFI_INFO(": max_capacity_ap [select succ] which bssid =" MACFMT, MAC2STR(max_capacity_ap->ap_info.bssid));

        candidate_ap_result = max_capacity_ap;
    }
    else if(NULL != max_rssi_ap)
    {

        HWIFI_INFO(": max_rssi_ap [select succ] bssid =" MACFMT, MAC2STR(max_rssi_ap->ap_info.bssid));

        candidate_ap_result = max_rssi_ap;
    }

    /* 未选择到AP，返回NULL */
    if (NULL == candidate_ap_result)
    {
        pst_roam_info->roam_fsm.force_trigger_scan_flag = 0;

        pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 0;

        spin_unlock_bh(&pst_roam_info->ap_list.list_lock);
        return NULL;
    }

    /* 如果选出的ap为当前已关联AP，则返回NULL；避免选择比当前网络条件更差的AP */
    if (hwifi_roam_is_current_assoc_ap(pst_roam_info, candidate_ap_result->ap_info.bssid))
    {
        HWIFI_INFO(": candidate_ap_result [is current ap].");

        pst_roam_info->roam_fsm.force_trigger_scan_flag = 0;

        pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 0;

        spin_unlock_bh(&pst_roam_info->ap_list.list_lock);

        return NULL;
    }

    cfg = roam_to_cfg(pst_roam_info);;
    if(NULL == cfg)
    {
        HWIFI_WARNING("cfg is null.");

        pst_roam_info->roam_fsm.force_trigger_scan_flag = 0;

        pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 0;

        spin_unlock_bh(&pst_roam_info->ap_list.list_lock);

        return NULL;
    }

    rssi_current = cfg->latest_signal;

    /*
     * 当前AP的信号强度在-80及以上（小于等于-80），只要  新AP信号强度 减去  当前AP信号强度 大于等于 4 ，就漫游。（即 -80 可以漫游到 -78或大于-78），忽视 pingpang漫游时间限制
     * 当前AP的信号强度在-75及以上（小于等于-75），只要  新AP信号强度 减去  当前AP信号强度 大于等于 6 ，就漫游。（即 -75 可以漫游到 -72或大于-72），忽视 pingpang漫游时间限制
     * 当前AP的信号强度在-70及以上（小于等于-70），只要  新AP信号强度 减去  当前AP信号强度 大于等于 8 ，就漫游。（即 -70 可以漫游到 -65或大于-65），且新AP到旧AP 满足5s pingpang时间。
     */
    min_diff_rssi = calc_min_diff_rssi(pst_roam_info, rssi_current);
    HWIFI_INFO("current ap rssi:%d,select ap rssi:%d,min_diff_rssi:%d", rssi_current, max_rssi, min_diff_rssi);

    if((max_rssi - rssi_current) < min_diff_rssi)
    {
        if(pst_roam_info->roam_fsm.force_trigger_scan_flag)
        {
            HWIFI_INFO("force_trigger_scan_flag is enable,ignore rssi compare.");
        }
        else if(pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag)
        {
            HWIFI_INFO("tx_failed_trigger_flag is enable");
            if(max_rssi <= rssi_current)
            {
                pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 0;

                HWIFI_INFO("max_rssi <= rssi_current,ignore this select AP.");
                spin_unlock_bh(&pst_roam_info->ap_list.list_lock);
                return NULL;
            }
        }
        else
        {
            HWIFI_INFO("max_rssi - rssi_current < min_diff_rssi(%d),ignore this select AP.", min_diff_rssi);

            pst_roam_info->roam_fsm.force_trigger_scan_flag = 0;
            spin_unlock_bh(&pst_roam_info->ap_list.list_lock);
            return NULL;
        }
    }

    pst_roam_info->roam_fsm.force_trigger_scan_flag = 0;

    pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 0;

    /* 记录漫游时选择目标AP的rssi */
    pst_roam_info->roam_fsm.target_ap_rssi = max_rssi;

    #if 0
    /* 如果选出的ap为pingpong，返回NULL */
    if (TRUE == hwifi_roam_is_pingpong(pst_roam_info, candidate_ap_result->ap_info.bssid))
    {
        HWIFI_INFO(": candidate_ap_result [is in pingpong].");
        spin_unlock_bh(&pst_roam_info->ap_list.list_lock);
        return NULL;
    }
    #endif
    hwifi_roam_candidate_ap_get(candidate_ap_result);
    spin_unlock_bh(&pst_roam_info->ap_list.list_lock);

    return &candidate_ap_result->ap_info;
}

/*
 * Prototype    : hwifi_roam_clear
 * Description  : clear the candidate ap list
 * Input        : roam_info : all info of roam
 * Output       : None
 * Return Value : void
 * Calls        : null
 * Called By    : roam_enter_scanning
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_roam_clear(struct roam_all_stru *pst_roam_info)
{
    struct list_head *list;
    struct roam_scan_candidate_ap_node *candidate_ap;
    HWIFI_ASSERT(NULL != pst_roam_info);

    /*  */
    spin_lock_bh(&pst_roam_info->ap_list.list_lock);

    list = &pst_roam_info->ap_list.list;
    while(list->next != &pst_roam_info->ap_list.list)
    {

        candidate_ap = list_entry(list->next, struct roam_scan_candidate_ap_node, list);

        /* 先拆链，再释放空间 */
        list_del(list->next);
        if (NULL!= candidate_ap)
        {
            if (NULL != candidate_ap->ap_info.frame)
            {
                OS_MEM_FREE(candidate_ap->ap_info.frame);
            }

            hwifi_roam_candidate_ap_put(&candidate_ap->ap_info);
        }
    }

    spin_unlock_bh(&pst_roam_info->ap_list.list_lock);

    return;
}

/*
 * Prototype    : hwifi_roam_update_blacklist
 * Description  : update the ap black list: 1) if bssid is null, then remove candidate
                : aps whose timestamp is older than aging time. 2) if bssid is not null, add
                : the ap into the blacklist
 * Input        : pst_roam_info : all info of roam
                : bssid : addr of bssid
 * Output       : None
 * Return Value : void
 * Calls        : null
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_roam_update_blacklist(struct roam_all_stru *pst_roam_info, uint8 *bssid)
{
    unsigned long   now;
    int             old_time;
    uint32          oldest_ap_idx;
    uint32          black_list_loop;
    uint8           bssid_tmp[MAC_LEN] = {0};
    roam_ap_black_list_stru *black;

    HWIFI_ASSERT(NULL != pst_roam_info);

    now = jiffies;
    old_time = pst_roam_info->alg_param.execute_param.blacklist_expire_time_s;
    black = &pst_roam_info->black_list;

    HWIFI_ASSERT(black->nums <= ROAM_BLACKLIST_AP_NUMBER);

    /* 先按老化规格更新ap黑名单 */
    spin_lock_bh(&pst_roam_info->black_list.lock);
    for (black_list_loop = 0; black_list_loop < ROAM_BLACKLIST_AP_NUMBER; black_list_loop++)
    {
        if ((0 != OS_MEM_CMP(black->blacks[black_list_loop].bssid, bssid_tmp, MAC_LEN))
             && (time_before((black->blacks[black_list_loop].time_stamp + old_time * HZ), now)))
        {
            HWIFI_INFO("Delete a ap in blaclist. bssid = "MACFMT"\n", MAC2STR(black->blacks[black_list_loop].bssid));

            OS_MEM_SET(black->blacks[black_list_loop].bssid, 0, MAC_LEN);
            black->nums--;
        }
    }

    /* 没有新ap加入黑名单 */
    if (NULL == bssid)
    {
        spin_unlock_bh(&pst_roam_info->black_list.lock);
        return;
    }

    /* 如果黑名单已满，且均没有老化，先提出一个最老的黑名单 */
    if (black->nums == ROAM_BLACKLIST_AP_NUMBER)
    {
        oldest_ap_idx = 0;
        for (black_list_loop = 1; black_list_loop < ROAM_BLACKLIST_AP_NUMBER; black_list_loop++)
        {
            if (time_before(black->blacks[black_list_loop].time_stamp, black->blacks[oldest_ap_idx].time_stamp))
            {
                oldest_ap_idx = black_list_loop;
            }
        }

        OS_MEM_SET(black->blacks[oldest_ap_idx].bssid, 0, MAC_LEN);
        black->nums--;
    }

    for (black_list_loop = 0; black_list_loop < ROAM_BLACKLIST_AP_NUMBER; black_list_loop++)
    {
        if (0 == OS_MEM_CMP(bssid, black->blacks[black_list_loop].bssid, MAC_LEN))
        {
            /* 已经在黑名单中，增加黑名单次数,并更新此bssid的黑名单时间 */
            black->blacks[black_list_loop].count++;
            black->blacks[black_list_loop].time_stamp = now;
            HWIFI_INFO("update blacklist bssid:" MACFMT " count to %d\n", MAC2STR(bssid), black->blacks[black_list_loop].count);

            spin_unlock_bh(&pst_roam_info->black_list.lock);
            return;
        }
    }

    /* 加入黑名单 */
    for (black_list_loop = 0; black_list_loop < ROAM_BLACKLIST_AP_NUMBER; black_list_loop++)
    {
        if (0 == OS_MEM_CMP(black->blacks[black_list_loop].bssid, bssid_tmp, MAC_LEN))
        {
            HWIFI_INFO("Add a ap into blacklist. bssid = "MACFMT"\n", MAC2STR(bssid));

            OS_MEM_CPY(black->blacks[black_list_loop].bssid, bssid, MAC_LEN);
            black->blacks[black_list_loop].time_stamp = now;
            black->blacks[black_list_loop].count = 1;
            black->nums++;
            break;
        }
    }

    spin_unlock_bh(&pst_roam_info->black_list.lock);
    return;
}
void hwifi_roam_blacklist_bssid_clear(struct roam_all_stru *pst_roam_info, uint8 *bssid)
{
    uint32 black_list_loop;
    roam_ap_black_list_stru *black;

    HWIFI_ASSERT(NULL != pst_roam_info);

    black = &pst_roam_info->black_list;

    if(NULL == bssid)
    {
        return;
    }

    spin_lock_bh(&pst_roam_info->black_list.lock);
    
    for (black_list_loop = 0; black_list_loop < ROAM_BLACKLIST_AP_NUMBER; black_list_loop++)
    {
        if (0 == OS_MEM_CMP(bssid, black->blacks[black_list_loop].bssid, MAC_LEN))
        {
            HWIFI_INFO("clear bssid:" MACFMT " from blacklist.", MAC2STR(bssid));
            OS_MEM_SET(black->blacks[black_list_loop].bssid, 0, MAC_LEN);
            black->nums--;
            break;
        }
    }
    
    spin_unlock_bh(&pst_roam_info->black_list.lock);
    return;
}

/*
 * Prototype    : hwifi_roam_update_ext_ap
 * Description  : update the ap black list: 1) if bssid is null, then remove ext
                : aps whose timestamp is older than aging time. 2) if bssid is not null, add
                : the ap into the ext ap
 * Input        : pst_roam_info : all info of roam
                : bssid : addr of bssid
 * Output       : None
 * Return Value : void
 * Calls        : null
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_roam_update_ext_ap(struct roam_all_stru *pst_roam_info, uint8 *bssid)
{
    unsigned long   now;
    int             old_time;
    uint32          oldest_ap_idx;
    uint32          ap_loop;
    uint8           bssid_tmp[MAC_LEN] = {0};
    roam_ext_ap_stru *ext_ap;

    HWIFI_ASSERT(NULL != pst_roam_info);

    now = jiffies;
    old_time = pst_roam_info->alg_param.execute_param.pingpong_expire_time_s;
    ext_ap = &pst_roam_info->ext_ap;

    HWIFI_ASSERT(ext_ap->nums <= ROAM_EXT_AP_NUMBER);

    /* 先按老化规格更新ap黑名单 */

    spin_lock_bh(&pst_roam_info->ext_ap.lock);
    for (ap_loop = 0; ap_loop < ROAM_EXT_AP_NUMBER; ap_loop++)
    {
        if ((0 != OS_MEM_CMP(ext_ap->ext_ap[ap_loop].bssid, bssid_tmp, MAC_LEN))
             && (time_before((ext_ap->ext_ap[ap_loop].time_stamp + old_time * HZ), now)))
        {
            HWIFI_INFO("Delete a old ap in ext list. bssid = "MACFMT"\n", MAC2STR(ext_ap->ext_ap[ap_loop].bssid));

            OS_MEM_SET(ext_ap->ext_ap[ap_loop].bssid, 0, MAC_LEN);
            ext_ap->nums--;
        }
    }

    /* 没有新ap加入历史优选 */
    if (NULL == bssid)
    {
        spin_unlock_bh(&pst_roam_info->ext_ap.lock);
        return;
    }

    /* 如果历史优选已满，且均没有老化，先剔除一个最老的名单 */
    if (ext_ap->nums == ROAM_EXT_AP_NUMBER)
    {
        oldest_ap_idx = 0;
        for (ap_loop = 1; ap_loop < ROAM_EXT_AP_NUMBER; ap_loop++)
        {
            if (time_before(ext_ap->ext_ap[ap_loop].time_stamp, ext_ap->ext_ap[oldest_ap_idx].time_stamp))
            {
                oldest_ap_idx = ap_loop;
            }
        }
        HWIFI_INFO("Delete a ap in ext list [is full]. bssid = "MACFMT"\n", MAC2STR(ext_ap->ext_ap[oldest_ap_idx].bssid));

        OS_MEM_SET(ext_ap->ext_ap[oldest_ap_idx].bssid, 0, MAC_LEN);
        ext_ap->nums--;
    }

    /* 加入历史优选AP列表 */
    for (ap_loop = 0; ap_loop < ROAM_EXT_AP_NUMBER; ap_loop++)
    {
        if (0 == OS_MEM_CMP(ext_ap->ext_ap[ap_loop].bssid, bssid_tmp, MAC_LEN))
        {
            HWIFI_INFO("Add a ap into ext list. bssid = "MACFMT"\n", MAC2STR(bssid));

            OS_MEM_CPY(ext_ap->ext_ap[ap_loop].bssid, bssid, MAC_LEN);
            ext_ap->ext_ap[ap_loop].time_stamp = now;
            ext_ap->nums++;
            break;
        }
    }

    spin_unlock_bh(&pst_roam_info->ext_ap.lock);
    return;
}


/*
 * Prototype    : hwifi_roam_channel_is_orthogonal
 * Description  :
 * Input        : channel : channel no
 * Output       : None
 * Return Value : true or false
 * Calls        : null
 * Called By    : none
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
bool hwifi_roam_channel_is_orthogonal(uint8 channel)
{

    if ((channel >= HWIFI_MIN_CHANNEL_2G ) && (channel <= HWIFI_MAX_CHANNEL_2G ))
    {
        if ((1 == channel) || (6 == channel) ||(11 == channel))
        {
            return true;
        }

        return false;
    }


    if ((channel >= HWIFI_MIN_CHANNEL_5G ) && (channel <= HWIFI_MAX_CHANNEL_4_9G ))
    {
        if ((36 == channel) || (37 == channel) ||(38 == channel))
        {
            return false;
        }

        return true;
    }

    return false;
}


/*
 * Prototype    : hwifi_roam_update_scan_channels
 * Description  : handle the roam scan msg reported by device
 * Input        : roam : all info of roam
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_update_scan_channels
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_update_scan_channels(struct roam_all_stru *roam)
{
    uint32 loop;
    uint32 domain_ch_idx;
    uint32 scan_channel_idx;

    HWIFI_ASSERT(NULL != roam);

    roam->roam_fsm.scan_channel_count = 0;
    OS_MEM_SET(roam->roam_fsm.scan_channels, 0xff, HWIFI_ROAM_SCAN_CHANNEL_MAX_NUM * sizeof(struct roam_scan_channel_stru));

    scan_channel_idx = 0;

    HWIFI_INFO("is_orthogonal = %d, band = %d.", roam->alg_param.scan_param.is_orthogonal, roam->alg_param.scan_param.band);

    /* 更新STA 5G频段信道列表 */
    for (domain_ch_idx = 0; (domain_ch_idx <= HWIFI_MAX_CHANNEL_4_9G)&&(scan_channel_idx < HWIFI_ROAM_SCAN_CHANNEL_MAX_NUM); domain_ch_idx++ )
    {
        /* 1)判断频段标志 */
        if (ROAM_MATCH_ALL_MASK ==  roam->roam_fsm.domain_channels[domain_ch_idx].flag)
        {
            roam->roam_fsm.scan_channels[scan_channel_idx].band     = roam->roam_fsm.domain_channels[domain_ch_idx].band;
            roam->roam_fsm.scan_channels[scan_channel_idx].channel  = roam->roam_fsm.domain_channels[domain_ch_idx].channel;
            roam->roam_fsm.scan_channel_count++;

            scan_channel_idx++;
        }
    }

    for (loop = 0; loop < HWIFI_ROAM_SCAN_CHANNEL_MAX_NUM; loop++)
    {
        if (0xff != roam->roam_fsm.scan_channels[loop].band)
        {
            HWIFI_INFO("band = %d, channel = %d.", roam->roam_fsm.scan_channels[loop].band, roam->roam_fsm.scan_channels[loop].channel);
        }
    }

    return SUCC;
}


void  hwifi_roam_bit_clear_bit_four_byte(uint32 *pul_byte, uint32 nr)
{
    *pul_byte &= ~((uint32)(1 << nr));
}

uint8  hwifi_roam_bit_find_first_bit_four_byte(uint32 ul_byte)
{
    uint8 uc_ret = 0;

    if (!(ul_byte & 0xffff))
    {
        ul_byte >>= 16;
        uc_ret += 16;
    }

    if (!(ul_byte & 0xff))
    {
        ul_byte >>= 8;
        uc_ret += 8;
    }

    if (!(ul_byte & 0xf))
    {
        ul_byte >>= 4;
        uc_ret += 4;
    }

    if (!(ul_byte & 3))
    {
        ul_byte >>= 2;
        uc_ret += 2;
    }

    if (!(ul_byte & 1))
    {
        //ul_byte >>= 1;
        uc_ret += 1;
    }

    return uc_ret;
}

/*
 * Prototype    : hwifi_roam_calc_channels
 * Description  : handle the roam scan msg reported by device
 * Input        : roam : all info of roam
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_update_scan_channels
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_roam_calc_channels(struct roam_all_stru *roam, uint8 band, uint32 channel, struct hwifi_freq_map *freq_map, uint8 map_size)
{
    uint32  channel_bit;
    uint32  freq_map_idx = 0;

    HWIFI_ASSERT((NULL != roam) && (NULL != freq_map));
    HWIFI_ASSERT(( band == ROAM_PARAM_BAND_2G) || (band == ROAM_PARAM_BAND_5G));

    channel_bit = channel;

    spin_lock_bh(&roam->roam_fsm.lock);

    while(channel_bit > 0)
    {
        freq_map_idx = hwifi_roam_bit_find_first_bit_four_byte(channel_bit);
        if (freq_map_idx >= map_size)
        {
            HWIFI_ERROR("freq_map_idx is error. freq_map_idx = %d, map_size = %d.", freq_map_idx, map_size);
            hwifi_roam_bit_clear_bit_four_byte(&channel_bit, freq_map_idx);
            continue;
        }


        if ((freq_map[freq_map_idx].chan > HWIFI_MAX_CHANNEL_4_9G))
        {
            HWIFI_ERROR("Channel is error. channel = %d.", freq_map[freq_map_idx].chan);

            hwifi_roam_bit_clear_bit_four_byte(&channel_bit, freq_map_idx);
            continue;
        }

        /* 此处先将flag设置为MATCH */
        roam->roam_fsm.domain_channels[freq_map[freq_map_idx].chan].band     = band;
        roam->roam_fsm.domain_channels[freq_map[freq_map_idx].chan].channel  = freq_map[freq_map_idx].chan;
        roam->roam_fsm.domain_channels[freq_map[freq_map_idx].chan].flag    |= ROAM_MATCH_REG_DOMAIN;

        hwifi_roam_bit_clear_bit_four_byte(&channel_bit, freq_map_idx);

    }

    spin_unlock_bh(&roam->roam_fsm.lock);

    return ;
}





/*
 * Prototype    : hwifi_roam_update_channels_orthogonal_flag
 * Description  : update domain channels flag
 * Input        : roam : all info of roam
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_update_scan_channels
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_update_channels_orthogonal_flag(struct roam_all_stru *roam)
{
    uint32 loop;

    HWIFI_ASSERT(NULL != roam);

    spin_lock_bh(&roam->roam_fsm.lock);

    /* 更新STA 5G频段信道列表 */
    for(loop = 0; loop <= HWIFI_MAX_CHANNEL_4_9G; loop++)
    {
        /* 先清理标志 */
        roam->roam_fsm.domain_channels[loop].flag &= ROAM_CLEAR_FLAG_ORTHOGONAL;

        if ((loop < HWIFI_MIN_CHANNEL_2G) || ((loop > HWIFI_MAX_CHANNEL_2G) && (loop < HWIFI_MIN_CHANNEL_5G)))
        {
            continue;
        }

        if (false == roam->alg_param.scan_param.is_orthogonal)
        {
            roam->roam_fsm.domain_channels[loop].flag |= ROAM_MATCH_ORTHOGONAL;
            continue;
        }

        if (true == hwifi_roam_channel_is_orthogonal(loop))
        {
            roam->roam_fsm.domain_channels[loop].flag |= ROAM_MATCH_ORTHOGONAL;
            continue;
        }

    }

    for (loop = 0; loop <= HWIFI_MAX_CHANNEL_4_9G; loop++)
    {
        if (0xff != roam->roam_fsm.domain_channels[loop].band)
        {
            HWIFI_DEBUG("band = %d, channel = %d, flag = %d.", roam->roam_fsm.domain_channels[loop].band, roam->roam_fsm.domain_channels[loop].channel, roam->roam_fsm.domain_channels[loop].flag);
        }
    }

    spin_unlock_bh(&roam->roam_fsm.lock);

    return SUCC;
}


/*
 * Prototype    : hwifi_roam_update_channels_band_flag
 * Description  : update domain channels flag
 * Input        : roam : all info of roam
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_update_scan_channels
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_update_channels_band_flag(struct roam_all_stru *roam)
{
    uint32 loop;

    HWIFI_ASSERT(NULL != roam);

    spin_lock_bh(&roam->roam_fsm.lock);

    /* 更新STA 5G频段信道列表 */
    for(loop = 0; loop <= HWIFI_MAX_CHANNEL_4_9G; loop++)
    {
        /* 先清理标志 */
        roam->roam_fsm.domain_channels[loop].flag &= ROAM_CLEAR_FLAG_BAND;

        /* 判断5g频段 */
        if ((ROAM_PARAM_BAND_5G == roam->roam_fsm.domain_channels[loop].band)
           && ((ROAM_PARAM_BAND_5G == roam->alg_param.scan_param.band) || (ROAM_PARAM_BAND_BOTH == roam->alg_param.scan_param.band)))
        {
            roam->roam_fsm.domain_channels[loop].flag |= ROAM_MATCH_BAND;
        }

        /* 判断2g频段 */
        if ((ROAM_PARAM_BAND_2G == roam->roam_fsm.domain_channels[loop].band)
             && ((ROAM_PARAM_BAND_2G == roam->alg_param.scan_param.band) || (ROAM_PARAM_BAND_BOTH == roam->alg_param.scan_param.band)))
        {
            roam->roam_fsm.domain_channels[loop].flag |= ROAM_MATCH_BAND;
        }
    }

    for (loop = 0; loop <= HWIFI_MAX_CHANNEL_4_9G; loop++)
    {
        if (0xff != roam->roam_fsm.domain_channels[loop].band)
        {
            HWIFI_DEBUG("band = %d, channel = %d, flag = %d.", roam->roam_fsm.domain_channels[loop].band, roam->roam_fsm.domain_channels[loop].channel, roam->roam_fsm.domain_channels[loop].flag);
        }
    }

    spin_unlock_bh(&roam->roam_fsm.lock);

    return SUCC;
}

/*
 * Prototype    : hwifi_roam_update_regdomain_channels
 * Description  : handle the roam scan msg reported by device
 * Input        : roam : all info of roam
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_update_scan_channels
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_roam_update_channels_regdomain_flag(struct roam_all_stru *roam, const struct ieee80211_regdomain *regdom, const struct hwifi_regdom_rc_info_wid *rc_info_wid, struct hwifi_freq_map *freq_map_2G, uint8 map_size_2G,struct hwifi_freq_map *freq_map_5G, uint8 map_size_5G)
{
    uint32  i;
    uint32 loop;
    uint32 count;

    HWIFI_ASSERT((NULL != roam) && (NULL != regdom) && (NULL != rc_info_wid));
    HWIFI_ASSERT((NULL != freq_map_2G) && (NULL != freq_map_5G));

    HWIFI_INFO("n_reg_rules = %d.", regdom->n_reg_rules);


    /* 1) 先统一清理一遍管制域的标志位信息 */
    for (loop =0; loop <= HWIFI_MAX_CHANNEL_4_9G; loop++)
    {
        /* 1.1) 保留漫游扫描算法相关的标志位，清除管制域的标志位信息 */
        roam->roam_fsm.domain_channels[loop].flag &= ROAM_CLEAR_REG_DOMAIN;
    }

    /* 2) 在根据当前最新下发的管制域信息更新标志位 */
    for (i = 0; i < regdom->n_reg_rules; i++)
    {
        HWIFI_INFO("i = %d, freq = %d, channel = %d.", i, rc_info_wid[i].freq, rc_info_wid[i].chan_bmap);

        if (FREQ_OF_2G == rc_info_wid[i].freq)
        {
            hwifi_roam_calc_channels(roam, ROAM_PARAM_BAND_2G, rc_info_wid[i].chan_bmap, freq_map_2G, map_size_2G);
        }
        else if (FREQ_OF_5G == rc_info_wid[i].freq)
        {
            hwifi_roam_calc_channels(roam, ROAM_PARAM_BAND_5G, rc_info_wid[i].chan_bmap, freq_map_5G, map_size_5G);
        }
        else
        {
            continue;
        }
    }

    count = 0;

    for (loop =0; loop <= HWIFI_MAX_CHANNEL_4_9G; loop++)
    {
        if (0xff != roam->roam_fsm.domain_channels[loop].band)
        {
            count ++;
            HWIFI_DEBUG("count =%d, band = %d, channel = %d, flag=%d", count, roam->roam_fsm.domain_channels[loop].band, roam->roam_fsm.domain_channels[loop].channel, roam->roam_fsm.domain_channels[loop].flag);
        }
    }
    return ;
}

/*
 * Prototype    : hwifi_roam_init_domain_channels
 * Description  : init scan channels
 * Input        : roam : all info of roam
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_update_scan_channels
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_roam_init_domain_channels(struct roam_all_stru *roam, struct hwifi_freq_map *freq_map_2G, uint8 map_size_2G, struct hwifi_freq_map *freq_map_5G, uint8 map_size_5G)
{
    uint32 loop;

    HWIFI_ASSERT(NULL != roam);

    spin_lock_bh(&roam->roam_fsm.lock);

    OS_MEM_SET(roam->roam_fsm.domain_channels, 0, (HWIFI_MAX_CHANNEL_4_9G + 1) * sizeof(struct roam_scan_channel_stru));

    for(loop = 0; loop <= HWIFI_MAX_CHANNEL_4_9G; loop++)
    {
        roam->roam_fsm.domain_channels[loop].band = 0xff;
    }

    /* 更新STA 2G频段信道列表 */
    for(loop = 0; loop < map_size_2G; loop++)
    {
        if ((freq_map_2G[loop].chan >= HWIFI_MIN_CHANNEL_2G) && (freq_map_2G[loop].chan <= HWIFI_MAX_CHANNEL_2G))
        {
            roam->roam_fsm.domain_channels[freq_map_2G[loop].chan].band = ROAM_PARAM_BAND_2G;
            roam->roam_fsm.domain_channels[freq_map_2G[loop].chan].channel = freq_map_2G[loop].chan;
            roam->roam_fsm.domain_channels[freq_map_2G[loop].chan].flag = ROAM_MATCH_REG_DOMAIN;
        }
    }

    /* 更新STA 5G频段信道列表 */
    for(loop = 0; loop < map_size_5G; loop++)
    {
        if ((freq_map_5G[loop].chan >= HWIFI_MIN_CHANNEL_5G) && (freq_map_5G[loop].chan <= HWIFI_MAX_CHANNEL_4_9G))
        {
            roam->roam_fsm.domain_channels[freq_map_5G[loop].chan].band = ROAM_PARAM_BAND_5G;
            roam->roam_fsm.domain_channels[freq_map_5G[loop].chan].channel = freq_map_5G[loop].chan;
            roam->roam_fsm.domain_channels[freq_map_5G[loop].chan].flag = ROAM_MATCH_REG_DOMAIN;
        }
    }

    spin_unlock_bh(&roam->roam_fsm.lock);

    hwifi_roam_update_channels_band_flag(roam);

    hwifi_roam_update_channels_orthogonal_flag(roam);

    return SUCC;
}


#endif /* __ROAM__ */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
