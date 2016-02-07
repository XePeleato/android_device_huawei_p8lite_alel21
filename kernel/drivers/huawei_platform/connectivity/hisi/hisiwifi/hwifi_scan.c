


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*
 * 1 Header File Including
 */
#include <linux/sched.h>
#include <net/cfg80211.h>
#include <linux/version.h>
#include <linux/time.h>
#include "common.h"
#include "hwifi_utils.h"
#include "hwifi_event.h"
#include "hwifi_cfg80211.h"
#include "hwifi_msg.h"
#ifdef __ROAM__
#include "hwifi_roam_alg.h"
#include "hi110x_pm.h"
#include "hi110x.h"
#endif
/*
 * 2 Global Variable Definition
 */

#define SCANNED_BSS_EXPIRE (3)*HZ

#define OPERATION_ON_5G_BAND 0x8000

#ifdef __ROAM__	


void update_roam_scan_info(struct cfg_struct *cfg)
{
    if(NULL == cfg)
    {
        HWIFI_WARNING("Argument cfg is null.");
        return;
    }
    if(cfg->roam.roam_fsm.bg_scan_effect_flag)
    {
        if(cfg->roam.roam_fsm.find_roam_new_ap_flag)
        {
            cfg->roam.roam_fsm.roam_scan_miss_count = 0;

            HWIFI_DEBUG("Find a same ssid ap,roam_scan_miss_count=%d,roam_scan_interval_s=%u", 
                        cfg->roam.roam_fsm.roam_scan_miss_count,
                        cfg->roam.roam_fsm.roam_scan_interval_s);
        }
        else
        {
            cfg->roam.roam_fsm.roam_scan_miss_count++;
            
            HWIFI_DEBUG("cant not Find a same ssid ap,roam_scan_miss_count=%d,roam_scan_interval_s=%u", 
                         cfg->roam.roam_fsm.roam_scan_miss_count,
                         cfg->roam.roam_fsm.roam_scan_interval_s);
        }
    }
}


void find_can_roamed_ap(struct cfg_struct *cfg, struct scanned_bss_info* bss)
{
    uint8                 *beacon_ie;
    uint32                 beacon_ie_len;
    uint8                 *ssid_ie;
    struct ieee80211_mgmt *new_ap_beacon;
    
    if((NULL == cfg) || (NULL == bss))
    {
        HWIFI_WARNING("Argument wiphy or bss is null.");
        return;
    }

    if(!cfg->roam.roam_fsm.is_roam_on)
    {
        HWIFI_DEBUG("roam function is not on.");
        return;
    }
    
    if(cfg->roam.roam_fsm.specify_ssid_scan_flag)
    {
        HWIFI_DEBUG("specify ssid scan,ignore this scan results.");
        return;
    }

    if ((HWIFI_CONNECTED != cfg->conn.status) || (FALSE == cfg->hi110x_dev->pm_info->connect_finished))
    {
        HWIFI_DEBUG("not connect status(%d) or getting ipaddress now,ignore this scan results.", (int32)cfg->conn.status);
        return;

    }

    cfg->roam.roam_fsm.bg_scan_effect_flag = TRUE;

    if(cfg->roam.roam_fsm.find_roam_new_ap_flag)
    {
        HWIFI_DEBUG("have find a same ap,ignore this scan results.\n"); 
        return;
    }

    new_ap_beacon = bss->mgmt;
    HWIFI_DEBUG("BSSID:"MACFMT, MAC2STR(new_ap_beacon->bssid));

            
    /* get channel utilizationg in BSS LOAD IE */
    beacon_ie = new_ap_beacon->u.beacon.variable;
    beacon_ie_len = bss->mgmt_len - offsetof(struct ieee80211_mgmt, u.beacon.variable);

    /* filter the ap if ssid diamatch */
    ssid_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_SSID, beacon_ie, beacon_ie_len);

    if (NULL == ssid_ie)
    {
        HWIFI_DEBUG("ssid is null\n");
        return;
    }

    if ((NULL != ssid_ie) && (hwifi_roam_ssid_is_dismatch(cfg, (*(ssid_ie + 1)), (ssid_ie + 2))))
    {
        HWIFI_DEBUG("ssid is dismatch.\n");
        return;
    }

    if (hwifi_roam_ie_is_mismatch(cfg, (uint8 *)new_ap_beacon, bss->mgmt_len))
    {
        HWIFI_DEBUG("ie is dismatch.\n");
        return ;
    }

    if (0 == OS_MEM_CMP(cfg->conn.bss.bssid, new_ap_beacon->bssid, MAC_LEN))
    {
        HWIFI_DEBUG("ignore current connect ap.\n");
        return;
    }

    if (TRUE == hwifi_roam_is_in_ap_blacklist(&cfg->roam, new_ap_beacon->bssid))
    {
        HWIFI_DEBUG("bssid in ap blacklist.\n");
        return;
    }
    
    HWIFI_DEBUG("find a same ssid,open roam scan flag.\n");

    cfg->roam.roam_fsm.find_roam_new_ap_flag = 1;
    return;
}
#endif


int inform_single_beacon_frame(struct scanned_bss_info* bss, void *data)
{
    struct cfg80211_bss      *cbss;
    struct wiphy             *wiphy;
    struct ieee80211_channel *channel; /* channel struct in linux kernel */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
    struct timespec ts;
#else
    struct timeval tv;
#endif

    wiphy = (struct wiphy *)data;

    channel = ieee80211_get_channel(wiphy, bss->freq);

    HWIFI_DEBUG("Beacon: freq = %d ,signal = %d,bssid =" MACFMT,
                 bss->freq,bss->signal, MAC2STR(bss->mgmt->bssid));

    /*硬件上报rssi有时会为0，统一将上报的0修改为一中间值*/
    bss->signal = bss->signal<0 ? bss->signal : (-68); /* rssi may be 0 */


#ifdef __ROAM__	
    find_can_roamed_ap(wiphy_priv(wiphy), bss);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
    get_monotonic_boottime(&ts);
    bss->mgmt->u.probe_resp.timestamp = ((u64)ts.tv_sec*1000000)
        + ts.tv_nsec / 1000; 
#else
    do_gettimeofday(&tv);
    bss->mgmt->u.probe_resp.timestamp = ((u64)tv.tv_sec*1000000)
        + tv.tv_usec;
#endif

    /* bss information been scanned */
    cbss = cfg80211_inform_bss_frame(wiphy, channel, bss->mgmt, bss->mgmt_len,  bss->signal * 100, GFP_KERNEL);
    if (cbss != NULL)
    {
        cfg80211_put_bss(wiphy, cbss);
    }

    return SUCC;
}

/*
 * Prototype    : set_scan_ssid
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    : hwifi_cfg80211_scan
 *
 *   History        :
 *   1.Date         : 2012/9/20
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC void set_scan_ssid(struct cfg_struct *cfg, struct hwifi_scan_param_stru *param, struct cfg80211_scan_request  *request)
{
#if 0
    if (HWIFI_CONNECTED != cfg->conn.status)
    {
        if ((0 == request->n_ssids) || (0 == request->ssids[0].ssid_len))
        {
            param->type = HWIFI_PASSIVE_SCAN; /* passive scan */
            param->ssid_num = 0;

            return;
        }
    }
#endif
    param->type = HWIFI_ACTIVE_SCAN; /* active scan */
    param->ssid_num = 0;

    if ((request->n_ssids > 0) && (NULL != request->ssids) && (request->ssids[0].ssid_len > 0))
    {
        param->ssid_num = 1;
        param->ssids[0].ssid_len = request->ssids[0].ssid_len;
        param->ssids[0].ssid     = request->ssids[0].ssid;
        HWIFI_DEBUG("Set scan ssid:%s",request->ssids[0].ssid);
    }
}

/*
 * Prototype    : set_scan_channel
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    : hwifi_cfg80211_scan
 *
 *   History        :
 *   1.Date         : 2012/9/20
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC void set_scan_channel(struct cfg80211_scan_request *request,
                                   uint8                         *channels_2g,
                                   int32                         *n_channels_2g,
                                   uint8                         *channels_5g,
                                   int32                         *n_channels_5g)
{
    int32 i;

    int32 n_2g = 0;
    int32 n_5g = 0;

    for (i = 0; i < request->n_channels; i++)
    {
        uint16 freq = request->channels[i]->center_freq;
        uint32 channel = ieee80211_frequency_to_channel(freq);

        if (freq <= HWIFI_MAX_FREQ_2G)
        {
            channels_2g[n_2g++] = channel;
        }
        else
        {
            channels_5g[n_5g++] = channel;
        }
        HWIFI_DEBUG("scan channel:%d",channel);
    }

    *n_channels_2g = n_2g;
    *n_channels_5g = n_5g;
}


/*
 * Prototype    : wait_for_scan_complete
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    : scan_work_func
 *
 *   History        :
 *   1.Date         : 2012/9/20
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC int32 wait_for_scan_complete(struct cfg_struct *cfg, struct hwifi_scan_param_stru *param)
{
    int32 ret;
    long  left;
    long  timeout;

    /* send scan cmd to device */
    ret = hwifi_start_scan(cfg, param);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to scan!");
        return -EFAIL;
    }

    cfg->scan.complete = FALSE;

    timeout = 2 * HZ;

    left = wait_event_interruptible_timeout(cfg->wait_queue, TRUE == cfg->scan.complete, timeout);

    if (0 == left)
    {
        HWIFI_WARNING("Scan wait for %ld ms timeout", timeout * 1000 / HZ);
        return -EFAIL;
    }
    else if(left < 0)
    {
        HWIFI_WARNING("Scan wait for %ld ms error,ret=%ld", timeout * 1000 / HZ, left);
        return -EFAIL;
    }
    else
    {
        HWIFI_DEBUG("Scan wait for %ld ms complete", timeout * 1000 / HZ);
        return SUCC;
    }
}

/*
 * Prototype    : scan_work_func
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    : hwifi_cfg80211_scan
 *
 *   History        :
 *   1.Date         : 2012/9/20
 *     Author       : kf74588
 *     Modification : Created function
 *
 */

STATIC void scan_work_func(struct work_struct *work)
{
    struct cfg_struct             *cfg;
    struct scan_struct            *scan;
    struct cfg80211_scan_request  *request;

    struct hwifi_scan_param_stru   param;  /* scan param */

    uint8   *channels_2G;
    uint8   *channels_5G;
    int32    n_channels_2G;
    int32    n_channels_5G;

    scan    = container_of(work, struct scan_struct, work);
    cfg     = container_of(scan, struct cfg_struct, scan);
    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
#ifdef __ROAM__	
	    mutex_unlock(&cfg->roam.roam_scan_lock);
#endif
        return;
    }

    request = scan->request;

    if (NULL == request)
    {
        HWIFI_INFO("Invalid NULL scan request!");
#ifdef __ROAM__	
	    mutex_unlock(&cfg->roam.roam_scan_lock);
#endif
        return;
    }

    memset(&param, 0, sizeof(param));
    channels_2G = OS_MALLOC_GFP(request->n_channels);
    channels_5G = OS_MALLOC_GFP(request->n_channels);
    HWIFI_DEBUG("channels_2G=%p, channels_5G=%p", channels_2G, channels_5G);

    /* clear scanned bss */
    clean_scanned_bss(&cfg->scan.bss_mgmt);

    if (NULL != channels_2G && NULL != channels_5G)
    {
        /* set scan channel list */
        set_scan_channel(request, channels_2G, &n_channels_2G, channels_5G, &n_channels_5G);
        HWIFI_DEBUG("Scan: channel num is %d, 2.4G: %d, 5G: %d", request->n_channels, n_channels_2G, n_channels_5G);

        set_scan_ssid(cfg, &param, request);
        HWIFI_DEBUG("Scan: scan type is %s", param.type ? "active scan" : "passive scan");

        /* set ie */
        param.ie     = request->ie;
        param.ie_len = request->ie_len;


        if (HWIFI_CONNECTED == cfg->conn.status)
        {
            if (cfg->conn.bss.freq <= HWIFI_MAX_FREQ_2G)
            {
                HWIFI_INFO("Status connected ,only scan 2.4G.");
                /*5g debug,modify by z00262551, when connected, we still scan all freq.*/
                /*n_channels_5G = 0;*/
            }
            else
            {
                HWIFI_INFO("Status connected ,only scan 5G.");
                /*n_channels_2G = 0;*/
            }
        }
        else if(IS_AP(cfg) && IS_P2P_ON(cfg))
        {
             /*
            channel_info:
            BIT15：0->2.4G，1->5G
            BIT9-8：0->SCN，1->SCA，3->SCB（20M情况上设置为SCN，40M情况选择SCA或SCB）
            BIT7-0：主信道号
            如果参数低8BITS配置为0，则AP在建立网络过程表示自动信道选择。
            */
            if(cfg->ap_info.channel_info & OPERATION_ON_5G_BAND)
            {
                HWIFI_INFO("Ap operation on 5G, only scan 5G");
                n_channels_2G = 0;
            }
            else
            {
                HWIFI_INFO("Ap operation on 2.4G, only scan 2.4G");
                n_channels_5G = 0;
            }
        }

        if (n_channels_2G > 0)
        {
            param.n_channels = n_channels_2G;
            param.channels   = channels_2G;
            param.band       = BAND_2G;

            wait_for_scan_complete(cfg, &param);
        }

        if (n_channels_5G > 0)
        {
            param.n_channels = n_channels_5G;
            param.channels   = channels_5G;
            param.band       = BAND_5G;

            wait_for_scan_complete(cfg, &param);
        }
    }

    if (NULL != channels_2G)
    {
        OS_MEM_FREE(channels_2G);
    }

    if (NULL != channels_5G)
    {
        OS_MEM_FREE(channels_5G);
    }

    /* if have current scan request, abort scanning */
    if (NULL != scan->request)
    {
        HWIFI_DEBUG("Stop SCAN request");

#ifdef __ROAM__	
        cfg->roam.roam_fsm.bg_scan_effect_flag = FALSE;
        cfg->roam.roam_fsm.find_roam_new_ap_flag = FALSE;
        cfg->roam.roam_fsm.specify_ssid_scan_flag = (param.ssid_num == 0 ? FALSE : TRUE);
#endif

        for_each_scanned_bss(&cfg->scan.bss_mgmt, inform_single_beacon_frame, cfg->wiphy);

#ifdef __ROAM__	
        update_roam_scan_info(cfg);
#endif

        /* notify kernel about scan complete reason */
        cfg80211_scan_done(scan->request, 0);

        scan->request = NULL;
        smp_mb();
        wake_up_interruptible(&cfg->wait_queue);
    }

    /* update scanning flag */
    scan->is_scanning = FALSE;
    HWIFI_DEBUG("Scan work done");
#ifdef __ROAM__	
    mutex_unlock(&cfg->roam.roam_scan_lock);
#endif
}


/*
 * Prototype    : scan_init
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/21
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
int scan_init(struct scan_struct *scan)
{
    scan->is_scanning = FALSE;
    scan->request     = NULL;
    INIT_WORK(&scan->work, scan_work_func);

    scanned_bss_mgmt_init(&scan->bss_mgmt);
    return SUCC;
}



/*
 * Prototype    : alloc_scanned_bss
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/19
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
struct scanned_bss_info* alloc_scanned_bss(int32 rssi, uint16  freq, uint8 *mgmt, int mgmt_len)
{
    struct scanned_bss_info  *bss;

    bss = kzalloc(sizeof(struct scanned_bss_info) + mgmt_len, GFP_KERNEL);
    if (NULL == bss)
    {
        HWIFI_WARNING("Failed to alloc memory for scanned bss!");
        return NULL;
    }

    bss->ts = jiffies;
    bss->signal   = rssi;
    bss->freq     = freq;

    bss->mgmt_len = mgmt_len;
    memcpy(bss->mgmt, mgmt, mgmt_len);
    HWIFI_DEBUG("Recv scanned bss:%p, [freq]= %d, [bssid]="MACFMT,bss,bss->freq,MAC2STR(bss->mgmt->bssid));

    mutex_init(&bss->mutex);
    INIT_LIST_HEAD(&bss->list);
    bss->ref = 1;

    return bss;
}

/*
 * Prototype    : get_scanned_bss
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/19
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
struct scanned_bss_info* get_scanned_bss(struct scanned_bss_info* bss_info)
{
    if (NULL == bss_info)
    {
        return NULL;
    }

    mutex_lock(&bss_info->mutex);

    if (0 == bss_info->ref)
    {
        mutex_unlock(&bss_info->mutex);
        return NULL;
    }

    bss_info->ref++;
    mutex_unlock(&bss_info->mutex);
    return bss_info;
}

/*
 * Prototype    : put_scanned_bss
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/19
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
void put_scanned_bss(struct scanned_bss_info* bss_info)
{
    if (NULL == bss_info)
    {
        return;
    }

    mutex_lock(&bss_info->mutex);
    bss_info->ref--;
    if (0 == bss_info->ref)
    {
        mutex_unlock(&bss_info->mutex);
        HWIFI_DEBUG("Release scanned bss 0x%p", bss_info);
        kfree(bss_info);

        return;
    }
    mutex_unlock(&bss_info->mutex);
}


/*
 * Prototype    : for_each_scanned_bss
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/19
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
int for_each_scanned_bss(struct scanned_bss_mgmt *bss_mgmt, SCAN_CB_F cb, void *data)
{
    struct list_head *list;
    struct scanned_bss_info* bss;

    if (NULL == bss_mgmt)
    {
        HWIFI_WARNING("Invalid NULL bss_mgmt");
        return -EFAIL;
    }

    mutex_lock(&bss_mgmt->mutex);
    list = &bss_mgmt->bss_list;

    while(list->next != &bss_mgmt->bss_list)
    {
        list = list->next;

        bss = list_entry(list, struct scanned_bss_info, list);
        if (NULL != bss)
        {
            cb(bss, data);
        }
    }

    mutex_unlock(&bss_mgmt->mutex);

    return SUCC;
}


STATIC struct scanned_bss_info* __find_scanned_bss_by_bssid(struct scanned_bss_mgmt *bss_mgmt, uint8* bssid)
{
    struct list_head *list;
    struct scanned_bss_info  *bss;

    list = &bss_mgmt->bss_list;

    while(list->next != &bss_mgmt->bss_list)
    {
        list = list->next;

        bss = list_entry(list, struct scanned_bss_info, list);
        if ((NULL!= bss) && (0 == memcmp(bssid, bss->mgmt->bssid, MAC_LEN)))
        {
            return bss;
        }
    }

    return NULL;
}



struct scanned_bss_info* find_scanned_bss_by_bssid(struct scanned_bss_mgmt *bss_mgmt, uint8* bssid)
{
    struct scanned_bss_info  *bss;

    if (NULL == bss_mgmt)
    {
        return NULL;
    }

    mutex_lock(&bss_mgmt->mutex);
    bss = __find_scanned_bss_by_bssid(bss_mgmt, bssid);
    bss = get_scanned_bss(bss);
    mutex_unlock(&bss_mgmt->mutex);

    return bss;
}

STATIC struct scanned_bss_info* __find_scanned_bss_by_ssid(struct scanned_bss_mgmt *bss_mgmt, uint8* ssid, int32 len)
{
    struct list_head *list;
    struct scanned_bss_info *bss;


    list = &bss_mgmt->bss_list;

    while(list->next != &bss_mgmt->bss_list)
    {
        uint8 *ssid_ie;

        list = list->next;

        bss = list_entry(list, struct scanned_bss_info, list);

        if (NULL == bss)
        {
            continue;
        }

        ssid_ie = bss->mgmt->u.beacon.variable;

        if (WLAN_EID_SSID != ssid_ie[0])
        {
            continue;
        }

        if (len != ssid_ie[1])
        {
            continue;
        }

        if (0 == memcmp(ssid_ie + 2, ssid, len))
        {
            return bss;
        }
    }


    return NULL;
}


struct scanned_bss_info* find_scanned_bss_by_ssid(struct scanned_bss_mgmt *bss_mgmt, uint8* ssid, int32 len)
{
    struct scanned_bss_info *bss;

    if (NULL == bss_mgmt)
    {
        return NULL;
    }
    mutex_lock(&bss_mgmt->mutex);
    bss = __find_scanned_bss_by_ssid(bss_mgmt, ssid, len);
    bss = get_scanned_bss(bss);
    mutex_unlock(&bss_mgmt->mutex);

    return bss;
}

int update_bss_info(struct scanned_bss_mgmt *bss_mgmt, struct scanned_bss_info* bss)
{
    struct scanned_bss_info  *old_bss;

    if (NULL == bss_mgmt)
    {
        return -EFAIL;
    }

    bss->mgmt->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON);

    mutex_lock(&bss_mgmt->mutex);
    old_bss = __find_scanned_bss_by_bssid(bss_mgmt, bss->mgmt->bssid);

    if (NULL != old_bss)
    {
        if(old_bss->signal > bss->signal)
        {
	        bss->signal = old_bss->signal;
		}
				
		HWIFI_DEBUG("new_signal:%d, old_signal:%d \n",  bss->signal, old_bss->signal);
        list_del(&old_bss->list);
        put_scanned_bss(old_bss);
    }

    list_add(&bss->list, &bss_mgmt->bss_list);
    HWIFI_INFO("Add done:" MACFMT " ,signal:%d,freq:%u.",  MAC2STR(bss->mgmt->bssid), bss->signal, bss->freq);
    mutex_unlock(&bss_mgmt->mutex);

    return SUCC;
}

/*
 * Prototype    : clean_scanned_bss
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/20
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
void clean_scanned_bss(struct scanned_bss_mgmt *bss_mgmt)
{
    struct scanned_bss_info  *bss;
    struct list_head *list;
     unsigned long now;

    if (NULL == bss_mgmt)
    {
        return;
    }

    mutex_lock(&bss_mgmt->mutex);

    list = &bss_mgmt->bss_list;
    /*
    while(list != list->next)
    {
        bss = list_entry(list->next, struct scanned_bss_info, list);
        list_del(&bss->list);
        put_scanned_bss(bss);
    }
    */
    while(list->next != &bss_mgmt->bss_list)
    {
        now = jiffies;
        bss = list_entry(list->next, struct scanned_bss_info, list);

        if (time_before(now, bss->ts + SCANNED_BSS_EXPIRE))
        {
            list = list->next;
            continue;
        }

        list_del(&bss->list);
        put_scanned_bss(bss);
    }
    mutex_unlock(&bss_mgmt->mutex);

    HWIFI_DEBUG("Cleaned scanned results");
}


void clean_scanned_bss_immediately(struct scanned_bss_mgmt *bss_mgmt)
{
    struct scanned_bss_info  *bss;
    struct list_head *list;

    if (NULL == bss_mgmt)
    {
        return;
    }

    mutex_lock(&bss_mgmt->mutex);

    list = &bss_mgmt->bss_list;

    while(list != list->next)
    {
        bss = list_entry(list->next, struct scanned_bss_info, list);
        list_del(&bss->list);
        put_scanned_bss(bss);
    }

    mutex_unlock(&bss_mgmt->mutex);

    HWIFI_DEBUG("Cleaned scanned results immediately.");
}

/*
 * Prototype    : scanned_bss_mgmt_init
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/20
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
int scanned_bss_mgmt_init(struct scanned_bss_mgmt *bss_mgmt)
{
    if (NULL == bss_mgmt)
    {
        return -EFAIL;
    }

    mutex_init(&bss_mgmt->mutex);

    INIT_LIST_HEAD(&bss_mgmt->bss_list);
    return SUCC;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

