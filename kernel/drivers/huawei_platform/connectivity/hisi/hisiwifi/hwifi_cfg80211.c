


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include <net/cfg80211.h>
#include <linux/gfp.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/ieee80211.h>
#include <../hw-ini/hisi_ini.h>
#include <linux/mtd/hisi_nve_interface.h>


#include "common.h"
#include "hi110x.h"
#include "hwifi_event.h"
#include "hwifi_cfg80211.h"
#include "hwifi_utils.h"
#include "hwifi_msg.h"
#include "hwifi_regdomain.h"
#include "station_mgmt.h"
#include "hwifi_scan.h"
#include "hwifi_wpa_ioctl.h" /* for wl_pwrm_set */
#include "hwifi_wl_config_ioctl.h"
#include "hwifi_perform.h"
#include "hi110x_pm.h"
#include "hwifi_hcc.h"

#include "hwifi_config.h"



#ifdef __ROAM__
#include "hwifi_roam_main.h"
#endif

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
#include "hwifi_aggre.h"
#endif


#define MAX_PROBED_SSID_NUM 1
#define MAX_SCAN_IE_LEN        1000

int32 hwifi_p2p_flag_set(struct cfg_struct *cfg, int32 flag);

/*
 * 2 Global Variable Definition
 */

/* information elements struct */
#pragma pack(1)
struct hwifi_elements_stru
{
    uint8   element_id;
    uint8   element_len;
    uint8   element_data[0];
};
#pragma pack()

static const uint8 broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8 zeroaddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


uint32 band_5g_enabled = FALSE;  /* TBD 5G使能开关，当前默认开启，发布时需要修改为默认关闭 */
uint8 g_ap_coex_2040m_enabled = 0;    /*ap 模式下非认证版本默认 20M 40m共存关闭，认证版本默认开启 */

uint8 g_sta_2040m_enabled = 0;
uint8 g_ap_2040m_enabled = 0;

module_param(band_5g_enabled, uint, S_IRUGO);

uint32  g_coex_level = 62500;
module_param(g_coex_level, uint, S_IRUGO|S_IWUSR);



#define HWIFI_MAC_HEADER           24
/* frame body exclude mac header */
#define HWIFI_BEACON_SSID_OFFSET   12

#ifdef MAC_802_11W
#define MAX_IGTK_KEY_INDEX         5
#endif

#define MAX_KEY_INDEX              3
#define MIN_KEY_INDEX              0

#define DEAUTH_TYPE 0
#define DISASSOC_TYPE 1

/* p2p type */
#define P2P_GO     2
#define P2P_CLIENT 1

/* ps */
#define DISABLED           0
#define ENABLED            1
#define LEGACY_POWERSAVE   3
#define MAX_LEGACY_PS      4
#define MAX_CTWINDOW       127
#define PS_PARAM_NO_CHANGE (-1)

#define RATETAB_ENT(_rate, _rateid, _flags)     \
{                                                                     \
        .bitrate        = (_rate),                                  \
        .hw_value       = (_rateid),                                \
        .flags          = (_flags),                                 \
}

#define CHAN2G(_channel, _freq, _flags)  \
{                       \
    .band                   = IEEE80211_BAND_2GHZ,          \
    .center_freq            = (_freq),                      \
    .hw_value               = (_channel),                   \
    .flags                  = (_flags),                     \
    .max_antenna_gain       = 0,                            \
    .max_power              = 30,                           \
}

#define CHAN5G(_channel, _flags) \
{                                              \
    .band                   = IEEE80211_BAND_5GHZ,          \
    .center_freq            = 5000 + (5 * (_channel)),      \
    .hw_value               = (_channel),                   \
    .flags                  = (_flags),                     \
    .max_antenna_gain       = 0,                            \
    .max_power              = 30,                           \
}

#define CHAN4_9G(_channel, _flags) \
{                                              \
    .band                   = IEEE80211_BAND_5GHZ,          \
    .center_freq            = 4000 + (5 * (_channel)),      \
    .hw_value               = (_channel),                   \
    .flags                  = (_flags),                     \
    .max_antenna_gain       = 0,                            \
    .max_power              = 30,                           \
}

/* rates supported by device  */
STATIC struct ieee80211_rate hi110x_rates[] = {
    RATETAB_ENT(10,  0x1,   0),
    RATETAB_ENT(20,  0x2,   0),
    RATETAB_ENT(55,  0x4,   0),
    RATETAB_ENT(110, 0x8,   0),
    RATETAB_ENT(60,  0x10,  0),
    RATETAB_ENT(90,  0x20,  0),
    RATETAB_ENT(120, 0x40,  0),
    RATETAB_ENT(180, 0x80,  0),
    RATETAB_ENT(240, 0x100, 0),
    RATETAB_ENT(360, 0x200, 0),
    RATETAB_ENT(480, 0x400, 0),
    RATETAB_ENT(540, 0x800, 0),
};

#define hi110x_a_rates           (hi110x_rates + 4)
#define hi110x_a_rates_size      8
#define hi110x_g_rates           (hi110x_rates + 0)
#define hi110x_g_rates_size      12

/* channels supported by 2.4G */
STATIC struct ieee80211_channel hi110x_2ghz_channels[] = {
    CHAN2G(1, 2412, 0),
    CHAN2G(2, 2417, 0),
    CHAN2G(3, 2422, 0),
    CHAN2G(4, 2427, 0),
    CHAN2G(5, 2432, 0),
    CHAN2G(6, 2437, 0),
    CHAN2G(7, 2442, 0),
    CHAN2G(8, 2447, 0),
    CHAN2G(9, 2452, 0),
    CHAN2G(10, 2457, 0),
    CHAN2G(11, 2462, 0),
    CHAN2G(12, 2467, 0),
    CHAN2G(13, 2472, 0),
    CHAN2G(14, 2484, 0),
};

/* channels supported by 5G */
STATIC struct ieee80211_channel hi110x_5ghz_channels[] = {
    CHAN5G(36, 0),
    CHAN5G(40, 0),
    CHAN5G(44, 0),
    CHAN5G(48, 0),
    CHAN5G(52, 0),
    CHAN5G(56, 0),
    CHAN5G(60, 0),
    CHAN5G(64, 0),
    CHAN5G(100, 0),
    CHAN5G(104, 0),
    CHAN5G(108, 0),
    CHAN5G(112, 0),
    CHAN5G(116, 0),
    CHAN5G(120, 0),
    CHAN5G(124, 0),
    CHAN5G(128, 0),
    CHAN5G(132, 0),
    CHAN5G(136, 0),
    CHAN5G(140, 0),
    CHAN5G(149, 0),
    CHAN5G(153, 0),
    CHAN5G(157, 0),
    CHAN5G(161, 0),
    CHAN5G(165, 0),
    CHAN4_9G(184, 0),
    CHAN4_9G(188, 0),
    CHAN4_9G(192, 0),
    CHAN4_9G(196, 0),
};

/* band information supported by 2.4G */
STATIC struct ieee80211_supported_band hi110x_band_2ghz = {
    .channels   = hi110x_2ghz_channels,
    .n_channels = ARRAY_SIZE(hi110x_2ghz_channels),
    .bitrates   = hi110x_g_rates,
    .n_bitrates = hi110x_g_rates_size,
    .ht_cap = {
        .ht_supported = TRUE,
        .cap = IEEE80211_HT_CAP_SUP_WIDTH_20_40,
    },
};

/* band information supported by 5G */

STATIC struct ieee80211_supported_band hi110x_band_5ghz = {
    .channels   = hi110x_5ghz_channels,
    .n_channels = ARRAY_SIZE(hi110x_5ghz_channels),
    .bitrates   = hi110x_a_rates,
    .n_bitrates = hi110x_a_rates_size,
    .ht_cap = {
        .ht_supported = TRUE,
        .cap = IEEE80211_HT_CAP_SUP_WIDTH_20_40,
    },
};

/* cipher suites supported by device */
STATIC const uint32 hi110x_cipher_suites[] = {
    WLAN_CIPHER_SUITE_WEP40,
    WLAN_CIPHER_SUITE_WEP104,
    WLAN_CIPHER_SUITE_TKIP,
    WLAN_CIPHER_SUITE_CCMP,
    WLAN_CIPHER_SUITE_AES_CMAC,
    WLAN_CIPHER_SUITE_SMS4,
};

/*
 * 3 Function Definition
 */

/*
 * Prototype    : ap_install_wep_key
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/29
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC int32 ap_install_wep_key(struct cfg_struct *cfg)
{
    struct hwifi_wep_key_param param;
    struct priv_key_param   *key;
    int32                    key_index;
    int32                    ret;

    memset(&param, 0, sizeof(param));

    param.default_index = cfg->key_mgmt.default_index;

    for (key_index = 0; key_index < MAX_KEY_NUM; key_index++)
    {
        key = &cfg->key_mgmt.gtk[key_index];

        if ((WLAN_CIPHER_SUITE_WEP40 == key->cipher)
            || (WLAN_CIPHER_SUITE_WEP104 == key->cipher))
        {
            HWIFI_INFO("set wep key,key_index=%d,key_len=%d,cipher=0x%08x", key_index, key->key_len, key->cipher);
            param.keys[key_index].index = key_index;
            param.keys[key_index].len = key->key_len;
            memcpy(param.keys[key_index].key, key->key, key->key_len);
        }
    }

    ret = hwifi_set_static_wep_key(cfg, &param);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to set static wep key!");
        return -EFAIL;
    }

    return ret;
}


int32 hwifi_set_mac(struct cfg_struct *cfg, uint8* mac)
{
    int32 ret;

    if ((NULL == cfg) || (NULL == mac))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    ret = wl_set_mac(cfg, mac);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to set mac:"MACFMT, MAC2STR(mac));
        return -EFAIL;
    }

    return ret;
}

/*
 * Prototype    : wl_set_pwrm_late
 * Description  :
 * Input        : None
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/30
 *     Author       :
 *     Modification : Created function
 *
 */
void wl_set_pwrm_late(struct work_struct * work)
{
    struct cfg_struct *cfg;
    int                ret = SUCC;

    cfg = container_of(work, struct cfg_struct, pwrm_set_work.work);




    cfg->hi110x_dev->pm_info->connect_finished = TRUE;
    HWIFI_DEBUG("Set early suspend when timeout");
    hi110x_suspend_action(cfg->hi110x_dev->pm_info);


    /* 如果Android高级选项选择:1、始终保持关联 2、仅限充电时(充电状态) */
    if (g_keepconnect_switch)
    {
        /* 设置最小省电模式 */
        ret = hi110x_pm_powermode_set(cfg, HI110X_POWERMODE_MIN);
        if (ret < 0)
        {
            HWIFI_WARNING("set min powermode wid failed...");
        }
    }

    return;

}

/*
 * Prototype    : __abort_scan
 * Description  : abort current scan request
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/10
 *     Author       : kf74588
 *     Modification : Created function
 *
 */


STATIC void __abort_scan(struct scan_struct *scan)
{
    struct cfg_struct *cfg;

    HWIFI_ASSERT(NULL != scan);
    cfg = container_of(scan, struct cfg_struct, scan);

    HWIFI_DEBUG("Abort scan");
    
    smp_mb();
    while(NULL != scan->request)
    {
        cfg->scan.complete = TRUE;
        wake_up_interruptible(&cfg->wait_queue);
        schedule();
        smp_mb();
    }
}

/*
 * Prototype    : hwifi_scan_done
 * Description  : complete scan event
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/10
 *     Author       : kf74588
 *     Modification : Created function
 *
 */


void hwifi_scan_done(struct cfg_struct *cfg)
{
    __abort_scan(&cfg->scan);
}

/*
 * Prototype    : stop_conn
 * Description  : stop current conn request
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/10
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC void stop_conn(struct conn_struct *conn)
{

    HWIFI_ASSERT(NULL != conn);
    /* TODO */
}


/*
 * Prototype    : hwifi_cfg80211_scan
 * Description  : scan request from cfg80211
 * Input        : struct wiphy *wiphy
                  struct net_device *dev
                  struct cfg80211_scan_request *request
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/20
 *     Author       : kf74588
 *     Modification : Created function
 *
 */

STATIC int32 hwifi_cfg80211_scan(struct wiphy                *wiphy,
                                        struct cfg80211_scan_request *request)
{
    struct cfg_struct      *cfg;
    struct hi110x_device   *hi110x_dev;
    int32 ret;

    HWIFI_ASSERT((NULL != wiphy) && (NULL != request));

    HWIFI_DEBUG("n_ssids = %d, ssids=0x%p", request->n_ssids, request->ssids);

    cfg = wiphy_priv(wiphy);
    hi110x_dev = cfg->hi110x_dev;

    if((FALSE == cfg->hi110x_dev->pm_info->connect_finished)
       && (IS_CONNECTED(cfg)))
    {
        /* if connected,but ip address is not config,scan is not allowed,because
           scan would switch to other channel and this could not receive the dhcp packet
        */
        HWIFI_INFO("config ip address now,ignore background scan!");
        /* 注意:不能返回-EBUSY,-EBUSY在上层wpa_supplicant看来是处于扫描中,影响逻辑判断 */
        return -EPERM;
    }

#if 0
    if (!hi110x_status_ready(hi110x_dev))
    {
        return -EIO;
    }
    if (test_bit(HI_STATUS_SCANNING, &hi110x_dev->status))
    {
        HWIFI_WARNING("Scanning already");
        return -EAGAIN;
    }
    if (test_bit(HI_STATUS_SCAN_ABORTING, &hi110x_dev->status))
    {
        HWIFI_WARNING("Scan aborting");
        return -EAGAIN;
    }

    set_bit(HI_STATUS_SCANNING, &hi110x_dev->status);
#endif

#ifdef __ROAM__
    mutex_lock(&cfg->roam.roam_scan_lock);

    if (IN_ROAMING(&cfg->roam))
    {
        HWIFI_INFO("in roaming,return scan failed.");
        mutex_unlock(&cfg->roam.roam_scan_lock);
    	
        /* 注意:不能返回-EBUSY,-EBUSY在上层wpa_supplicant看来是处于扫描中,影响逻辑判断 */
        return -EPERM;
    }
#endif
    ret = wait_event_interruptible(cfg->wait_queue, NULL == cfg->scan.request);
    if (ret < 0)
    {
        HWIFI_WARNING("Failed to wait event,ret=%d", ret);
        return -EFAIL;
    }

    /* update scanning flag */
    cfg->scan.is_scanning = TRUE;

    /* save current scan request */
    cfg->scan.request = request;

    queue_work(cfg->scan_wq, &cfg->scan.work);

    return SUCC;
}




int32 hwifi_get_cipher_suite(uint32 cipher_suite, uint8 *crypto)
{
    switch(cipher_suite)
    {
        case 0:
            HWIFI_INFO("Not set cipher suite!");
            break;
        case WLAN_CIPHER_SUITE_WEP40:
            *crypto      |= HWIFI_WEP;
            break;
        case WLAN_CIPHER_SUITE_WEP104:
            *crypto      |= HWIFI_WEP104;
            break;
        case WLAN_CIPHER_SUITE_TKIP:
            *crypto      |= HWIFI_TKIP;
            break;
        case WLAN_CIPHER_SUITE_CCMP:
        case WLAN_CIPHER_SUITE_AES_CMAC:
            *crypto      |= HWIFI_CCMP;
            break;
        default:
            HWIFI_WARNING("Invalid can not parse cipher suite:%d!",cipher_suite);
            return -EFAIL;
    }

    return SUCC;
}

 /*
 * Prototype    : set_conn_auth_type
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    : hwifi_cfg80211_connect
 *
 *   History        :
 *   1.Date         : 2012/9/20
 *     Author       :
 *     Modification : Created function
 *
 */
 STATIC int32 set_conn_auth_type(struct cfg_struct              *cfg,
                                        struct hwifi_conn_param_stru   *param,
                                        struct cfg80211_connect_params *sme)
{
    /* set auth type */
    switch(sme->auth_type)
    {
        case NL80211_AUTHTYPE_OPEN_SYSTEM:
            param->auth_type = HWIFI_AUTHTYPE_OPEN_SYSTEM;
            break;
        case NL80211_AUTHTYPE_SHARED_KEY:
            param->auth_type = HWIFI_AUTHTYPE_SHARED_KEY;

            HWIFI_DEBUG("CONNECT : sme->key_len = %d", sme->key_len);
            break;
        case NL80211_AUTHTYPE_AUTOMATIC:
            /* first use open system, if failed, use shared key */
            if ((sme->key_len != 0) && (0 == sme->crypto.n_akm_suites))
            {
                cfg->conn.auto_authtype = TRUE;
            }
            else
            {
                cfg->conn.auto_authtype = FALSE;
            }

            param->auth_type = HWIFI_AUTHTYPE_OPEN_SYSTEM;
            break;
        default:
            HWIFI_WARNING("Invalid can not parse auth type:%d!",sme->auth_type);
            return -EFAIL;
    }

    HWIFI_INFO("auth type is %d.", (int32)param->auth_type);

    return SUCC;
}

/*
* Prototype    : set_wpa_version
* Description  :
* Input        :
* Output       : None
* Return Value : None
* Calls        :
* Called By    : hwifi_cfg80211_connect
*
*   History        :
*   1.Date         : 2012/9/20
*     Author       :
*     Modification : Created function
*
*/
STATIC int32 set_wpa_version(struct hwifi_conn_param_stru *param, struct cfg80211_connect_params *sme)
{
    switch(sme->crypto.wpa_versions)
    {
        case 0:
            HWIFI_INFO("CONNECT : not use wpa/wpa2!");
            break;
        case NL80211_WPA_VERSION_1:
            param->crypto_type |= HWIFI_WPA;
            break;
        case NL80211_WPA_VERSION_2:
            param->crypto_type |= HWIFI_WPA2;
            break;

        case NL80211_WAPI_VERSION_1:
            param->crypto_type |= HWIFI_WAPI;
            break;

        default:
            HWIFI_WARNING("Invalid can not parse crypto type:%d",sme->crypto.wpa_versions);
            return -EFAIL;
    }

    HWIFI_INFO("crypto type is %u.", param->crypto_type);

    return SUCC;
}

/*
* Prototype    : set_akm_suites
* Description  :
* Input        :
* Output       : None
* Return Value : None
* Calls        :
* Called By    : hwifi_cfg80211_connect
*
*   History        :
*   1.Date         : 2012/9/20
*     Author       :
*     Modification : Created function
*
*/
STATIC int32 set_akm_suites(struct hwifi_conn_param_stru *param, struct cfg80211_connect_params *sme)
{
    if (0 != sme->crypto.n_akm_suites)
    {
        switch(sme->crypto.akm_suites[0])
        {
            case WLAN_AKM_SUITE_PSK:
            case WLAN_AKM_SUITE_WAPI_PSK:
                HWIFI_DEBUG("CONNECT : use psk auth");
                break;

            case WLAN_AKM_SUITE_8021X:
                param->auth_mode |= HWIFI_8021X;
                break;

            case WLAN_AKM_SUITE_WAPI_CERT:             /* added for wapi cert */
                param->auth_mode |= HWIFI_WAPI_CERT;
                break;

            default:
                HWIFI_WARNING("Invalid can not parse akm suite:%d!",sme->crypto.akm_suites[0]);
                return -EFAIL;
        }
    }

    HWIFI_INFO("auth mode is %u.", param->auth_mode);

    return SUCC;
}

/*
* Prototype    : set_wep_key
* Description  :
* Input        :
* Output       : None
* Return Value : None
* Calls        :
* Called By    : hwifi_cfg80211_connect
*
*   History        :
*   1.Date         : 2012/9/20
*     Author       :
*     Modification : Created function
*
*/
STATIC int32 set_wep_key(struct hwifi_conn_param_stru *param, struct cfg80211_connect_params *sme)
{
    if ((0 != sme->key_len) && (0 == sme->crypto.n_akm_suites))
    {
        if (sme->key_idx > MAX_KEY_INDEX)
        {
            HWIFI_WARNING("Invalid wep key index:%d!",sme->key_idx);
            return -EFAIL;
        }
        param->wep_key_index = sme->key_idx;
        param->wep_key_len   = sme->key_len;

        if (NULL == sme->key)
        {
            HWIFI_WARNING("Invalid NULL sem->key,key_idx=%d,key_len=%d.", sme->key_idx, sme->key_len);
            return -EFAIL;
        }
        memcpy(param->wep_key, sme->key, sme->key_len);
    }

    return SUCC;
}

void hwifi_get_max_rate(struct cfg_struct *cfg, struct scanned_bss_info* bss_info, struct rate_info *rate_info)
{
    struct hwifi_elements_stru  *supp_rate_ie;
    struct hwifi_elements_stru  *ext_supp_rate_ie;
    struct hwifi_elements_stru  *ht_ie;

    int rate = 0;
    const u8 *ie;
    u16 ht_cap;

    uint8 *beacon_ie;
    uint32 beacon_ie_len;

    beacon_ie = bss_info->mgmt->u.beacon.variable;
    beacon_ie_len = bss_info->mgmt_len - HWIFI_MAC_HEADER - HWIFI_BEACON_SSID_OFFSET;

    supp_rate_ie     = (struct hwifi_elements_stru  *)cfg80211_find_ie(WLAN_EID_SUPP_RATES, beacon_ie , beacon_ie_len);
    ext_supp_rate_ie = (struct hwifi_elements_stru  *)cfg80211_find_ie(WLAN_EID_EXT_SUPP_RATES, beacon_ie , beacon_ie_len);
    ht_ie            = (struct hwifi_elements_stru  *)cfg80211_find_ie(WLAN_EID_HT_CAPABILITY, beacon_ie , beacon_ie_len);

    if( ht_ie )/*如果支持ht*/
    {
        rate_info->mcs = 7 ;
        rate_info->flags |= RATE_INFO_FLAGS_MCS;
        rate_info->legacy = 10*65 ; /*内核根据mcs和flag计算11n的速率*/
        ht_cap = WPA_GET_LE16(ht_ie->element_data);
				
        if((ht_cap & (u16)(1 << 1)) && cfg->sta_info.fc_enabled)/*bit1:支持40M带宽*/
        {
            rate_info->flags |= RATE_INFO_FLAGS_40_MHZ_WIDTH;
        }
        if(ht_cap & (u16)(1 << 5) )/*bit5:支持short GI*/
        {
            rate_info->flags |= RATE_INFO_FLAGS_SHORT_GI;
        }


    }
    else/*不支持ht的情况*/
    {
        int index = 0;

        if(supp_rate_ie )
        {
            ie = (u8 *)supp_rate_ie;
            for (index = 0; ie && index < ie[1]; index++) {
                if ((ie[index + 2] & 0x7f) > rate)
                    rate = ie[index + 2] & 0x7f;
            }
        }
        if(ext_supp_rate_ie)
        {
            ie = (u8 *)ext_supp_rate_ie;
            for (index = 0; ie && index < ie[1]; index++) {
                if ((ie[index + 2] & 0x7f) > rate)
                    rate = ie[index + 2] & 0x7f;
            }
        }
        rate_info->legacy = rate *5 ; /*(rate/2)*10,in kbps*/
        rate_info->mcs = 0;
        rate_info->flags = 0;

  }
  HWIFI_DEBUG(" rate_info legacy = %d,mcs = %d, flags = %02x",
              rate_info->legacy,rate_info->mcs,rate_info->flags );
  return ;
}

#ifdef __ROAM__
/*
 * Prototype    : save_conn_ap_beacon
 * Description  : save connect ap beacon frame or probe response frame
 * Input        : struct cfg_struct *cfg
                  int32 ie_len
 * Output       : None
 * Return Value : SUCC or -EFAIL
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/6/17
 *     Author       : dengwenhua
 *     Modification : Created function
 *
 */
STATIC void save_conn_ap_frame(struct cfg_struct *cfg, struct scanned_bss_info* bss_info)
{   
    struct ieee80211_mgmt *mgmt;
    int32 mgmt_len;
    
    HWIFI_ASSERT((NULL != cfg) && (NULL != bss_info));

    mgmt = bss_info->mgmt;
    mgmt_len = bss_info->mgmt_len;

    cfg->conn.bss.band = (bss_info->freq <= HWIFI_MAX_FREQ_2G) ? ROAM_PARAM_BAND_2G : ROAM_PARAM_BAND_5G;
    
    if(cfg->conn.bss.mgmt_len && (NULL != cfg->conn.bss.mgmt))
    {
        /* 删除旧的AP 信息 */
        kfree(cfg->conn.bss.mgmt);
        cfg->conn.bss.mgmt = NULL;
        cfg->conn.bss.mgmt_len = 0;
    }
   
    if(0 < mgmt_len)
    {
        HWIFI_ASSERT(NULL != mgmt);
        /* 保存AP beacon报文或probe response报文 */
        cfg->conn.bss.mgmt = (struct ieee80211_mgmt *)kzalloc(mgmt_len, GFP_KERNEL);
        if (NULL == cfg->conn.bss.mgmt)
        {            
            HWIFI_WARNING("Failed to alloc memory for conn.bss.mgmt,but continue to connect.");
        }
        else
        {
            memcpy(cfg->conn.bss.mgmt, mgmt, mgmt_len);
            cfg->conn.bss.mgmt_len = mgmt_len;
        }
    }
}
#endif


/*
 * Prototype    : hwifi_cfg80211_connect
 * Description  : connect request from cfg80211
 * Input        : struct wiphy *wiphy
                  struct net_device *dev
                  struct cfg80211_connect_params *sme
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/27
 *     Author       : xudayong
 *     Modification : Created function
 *
 */

STATIC int32 hwifi_cfg80211_connect(struct wiphy                  *wiphy,
                                            struct net_device              *ndev,
                                            struct cfg80211_connect_params *sme)
{
    struct cfg_struct             *cfg;           /* wireless config struct */
    struct hwifi_conn_param_stru conn_param;    /* connect param struct */
    int32                        ret;
    struct scanned_bss_info* bss_info;
    struct rate_info rate;

    HWIFI_ASSERT(NULL != wiphy);
    HWIFI_ASSERT(NULL != ndev);
    HWIFI_ASSERT(NULL != sme);
    HWIFI_ASSERT(NULL != sme->ssid);

    cfg = wiphy_priv(wiphy);
    memset(&conn_param, 0, sizeof(conn_param));

    /* solve a scanning problem */
    cfg->conn.status = HWIFI_CONNECTING;

    if (NULL != sme->bssid)
    {
        HWIFI_INFO("find scanned bss by BSSID:"MACFMT, MAC2STR(sme->bssid));
        bss_info = find_scanned_bss_by_bssid(&cfg->scan.bss_mgmt, sme->bssid);
    }
    else
    {
        bss_info = find_scanned_bss_by_ssid(&cfg->scan.bss_mgmt, sme->ssid, sme->ssid_len);
    }

    if (NULL == bss_info)
    {
        HWIFI_WARNING("Can not find scanned bss info before connect!");
        cfg->conn.status = HWIFI_DISCONNECTED;
        return -EFAIL;
    }
    cfg->latest_signal = bss_info->signal ;
    HWIFI_DEBUG("scan bss signal %d, latest signal %d",
                 bss_info->signal, cfg->latest_signal);
    /*获取基础速率集合扩展速率集*/
     memset(&rate, 0, sizeof(rate));
     hwifi_get_max_rate(cfg, bss_info, &rate);
     cfg->rate.legacy = rate.legacy;
     cfg->rate.mcs = rate.mcs;
     cfg->rate.flags = rate.flags;
    conn_param.bss_info = bss_info;

	/* 注connect_finished不能在其他地方设置false，需严格与内核地址变动保持一致 */
    //cfg->hi110x_dev->pm_info->connect_finished = FALSE;

    /* add channel */
    conn_param.channel = (uint8)ieee80211_frequency_to_channel(bss_info->freq);
    cfg->sta_info.curr_channel = conn_param.channel;


    /* set ssid */
    conn_param.ssid     = sme->ssid;
    conn_param.ssid_len = sme->ssid_len;

    /* if there is bssid, set bssid */
    conn_param.bssid      = sme->bssid;
    conn_param.bssid_flag = (NULL != sme->bssid) ? TRUE:FALSE;

    ret = set_conn_auth_type(cfg, &conn_param, sme);
    if (ret < 0)
    {
        put_scanned_bss(bss_info);
        cfg->conn.status = HWIFI_DISCONNECTED;
        return -EFAIL;
    }

    if ((WEP40_KEY_LEN == sme->key_len) && (0 == sme->crypto.n_akm_suites))
    {
        conn_param.crypto_type |= HWIFI_WEP;
    }
    else if ((WEP104_KEY_LEN == sme->key_len) && (0 == sme->crypto.n_akm_suites))
    {
        conn_param.crypto_type |= HWIFI_WEP104;
    }
    else if ((0 == sme->key_len) || (0 != sme->crypto.n_akm_suites))
    {
        /* continue */
    }
    else
    {
        HWIFI_WARNING("Invalid lenth of wep key!");
        put_scanned_bss(bss_info);
        cfg->conn.status = HWIFI_DISCONNECTED;
        return -EFAIL;
    }


    /* get wpa version */
    ret = set_wpa_version(&conn_param, sme);
    if (ret < 0)
    {
        put_scanned_bss(bss_info);
        cfg->conn.status = HWIFI_DISCONNECTED;
        return -EFAIL;
    }

    /* get group crypto settings */
    ret = hwifi_get_cipher_suite(sme->crypto.cipher_group, &conn_param.group_crypto);
    if (ret < 0)
    {
        HWIFI_WARNING("Failed to get group key cipher suite");
        put_scanned_bss(bss_info);
        cfg->conn.status = HWIFI_DISCONNECTED;
        return -EFAIL;
    }

    /* get pairwise crypto settings */
    if (0 != sme->crypto.n_ciphers_pairwise)
    {
        ret = hwifi_get_cipher_suite(sme->crypto.ciphers_pairwise[0], &conn_param.pairwise_crypto);
        if (SUCC != ret)
        {
            HWIFI_WARNING("Failed to get pairwise key cipher suite!");
            put_scanned_bss(bss_info);
            cfg->conn.status = HWIFI_DISCONNECTED;
            return -EFAIL;
        }
    }

    /* mic error measure */
    if ((TRUE == cfg->tkip_measure_flags) && (FALSE == conn_param.bssid_flag))
    {
        /* if this case, let it to connect this ap */
        HWIFI_DEBUG("There is no bssid info, so we cann't know that it is the same ap or not");
    }
    else if ((TRUE == cfg->tkip_measure_flags)
            && (0 == memcmp(conn_param.bssid, cfg->conn.bss.bssid, MAC_LEN))
            && (0 == memcmp(conn_param.ssid, cfg->conn.bss.ssid, conn_param.ssid_len))
            && ((conn_param.group_crypto & HWIFI_TKIP) || (conn_param.pairwise_crypto & HWIFI_TKIP)))
    {
        HWIFI_WARNING("There is mic error measure, you cann't connect to the same ap with TKIP");
        cfg->conn.status = HWIFI_DISCONNECTED;
        return -EFAIL;
    }

    /* encrypt or not */
    if (0 != conn_param.crypto_type)
    {
        conn_param.crypto_type |= HWIFI_ENCRYPT;
    }

    if ((0 == conn_param.pairwise_crypto) && (0 == conn_param.group_crypto) && (conn_param.crypto_type & HWIFI_WPA))
    {
        /* 如果单播组播加密信息为空，且crypto_type为wpa1，则置crypto_type为空，解决wpa_supplicant的bug，即
         * 如果有wps IE信息元素，wpa_supplicant会置wpa version为1，而不判断是否实际包含wpa ie。
         */
        conn_param.crypto_type = 0;
    }

    /* get AKM suites */
    ret = set_akm_suites(&conn_param, sme);
    if (ret < 0)
    {
        put_scanned_bss(bss_info);
        cfg->conn.status = HWIFI_DISCONNECTED;
        return -EFAIL;
    }

    HWIFI_INFO("CONNECT : crypto_type is %u, group key is %u, pairwise key is %u," 
                "channel is %d, auth_type is %d.", 
                 conn_param.crypto_type, conn_param.group_crypto , conn_param.pairwise_crypto, 
                 conn_param.channel, conn_param.auth_type);

    /* get wep key and key index */
    ret = set_wep_key(&conn_param, sme);
    if (ret < 0)
    {
        put_scanned_bss(bss_info);
        cfg->conn.status = HWIFI_DISCONNECTED;
        return -EFAIL;
    }

    /* p2p added get ie */
    if (sme->crypto.wpa_versions != NL80211_WAPI_VERSION_1)
    {
        conn_param.ie     = sme->ie;
        conn_param.ie_len = sme->ie_len;
    }
    /* p2p added */


#ifdef __ROAM__
/* memset cfg->conn.bss 前释放bss结构体成员中动态申的内存 */
    if(NULL != cfg->conn.bss.mgmt)
    {
        /* 删除旧的AP 信息 */
        kfree(cfg->conn.bss.mgmt);
        cfg->conn.bss.mgmt = NULL;
        cfg->conn.bss.mgmt_len = 0;
    }

    if(NULL != cfg->conn.bss.assoc_ie)
    {
        /* 删除旧的IE */
        kfree(cfg->conn.bss.assoc_ie);
        cfg->conn.bss.assoc_ie = NULL;
        cfg->conn.bss.assoc_ie_len = 0;
    }
#endif

    memset(&cfg->conn.bss, 0, sizeof(cfg->conn.bss));

    memcpy(cfg->conn.bss.ssid, conn_param.ssid, conn_param.ssid_len);
    cfg->conn.bss.ssid_len = conn_param.ssid_len;

    cfg->conn.bss.auth_type = conn_param.auth_type;
    cfg->conn.bss.crypto_type = conn_param.crypto_type;

#ifdef __ROAM__
    save_conn_ap_frame(cfg, bss_info);
#endif

    cfg->conn.bss.channel = conn_param.channel;

    ret = hwifi_regdomain_update_by_ie(cfg, (uint8 *)bss_info->mgmt, bss_info->mgmt_len);
    if (ret < 0)
    {
        HWIFI_INFO("start connect:hwifi regdom update by country_ie fail.");
    }

    cancel_delayed_work_sync(&cfg->pwrm_set_work);
    HWIFI_DEBUG("Disable power save when connecting");
    wl_pwrm_enable(cfg, POWER_SAVE_DISABLE);

    schedule_delayed_work(&cfg->pwrm_set_work, 120*HZ);

    /* send msg */
    ret = hwifi_start_conn(cfg, &conn_param);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to connect");
        put_scanned_bss(bss_info);

        /*
         * if start connect fail, restore previous regulatory domain rules before
         * connect.
         */
        ret = hwifi_regdomain_update(cfg);
        if (ret != SUCC)
        {
            HWIFI_WARNING("start connect: Failed to update regdomain!");
        }

        cfg->conn.status = HWIFI_DISCONNECTED;
        
        return -EFAIL;
    }
#if 1
    /* STA swf antijam config */
    sfw_antijam_dynamic_config(cfg);
#endif
    put_scanned_bss(bss_info);

    return SUCC;
}


/*
 * Prototype    : hwifi_cfg80211_disconnect
 * Description  : disconnect request from cfg80211
 * Input        : struct wiphy *wiphy
                  struct net_device *dev
                  uin16 reason_code
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/27
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
STATIC int32 hwifi_cfg80211_disconnect(struct wiphy            *wiphy,
                                                struct net_device       *ndev,
                                                uint16                   reason_code)
{
    struct hwifi_disconn_param_stru  disconn_param;     /* connect param struct */
    struct cfg_struct               *cfg;               /* wireless config struct */
    int32                            ret;

    HWIFI_ASSERT(NULL != wiphy);
    HWIFI_ASSERT(NULL != ndev);

    cfg = wiphy_priv(wiphy);
    memset(&disconn_param, 0, sizeof(disconn_param));

    cancel_delayed_work_sync(&cfg->pwrm_set_work);

    HWIFI_INFO( "disconnect reason code is %u", reason_code);
    
    wl_pwrm_enable(cfg, POWER_SAVE_DISABLE);
    
	/* 注connect_finished不能在其他地方设置false，需严格与内核地址变动保持一致 */
    //cfg->hi110x_dev->pm_info->connect_finished = FALSE;

    /* check status */
    if (HWIFI_DISCONNECTED == cfg->conn.status)
    {
        HWIFI_INFO( "The Station has disconnected! return directly!");
        return SUCC;
    }

    hwifi_scan_done(cfg);

    /* save disconnect reason code */
    disconn_param.reason_code = reason_code;

    cfg->conn.status = HWIFI_DISCONNECTED;

    cfg->latest_signal = 0;
#ifdef __ROAM__
    roam_process_disconnect(&cfg->roam);
#endif

    /*
    * if disconnect actively, restore previous regulatory domain rules before
    * connect.
    */
    strncpy(cfg->beacon_ie_country_code, INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN);

    ret = hwifi_regdomain_update(cfg);
    if (ret != SUCC)
    {
        HWIFI_WARNING("Failed to update regdomain!");
    }

    /* send msg */
    ret = hwifi_start_disconn(cfg, &disconn_param);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to disconnect!");
        return -EFAIL;
    }

#ifdef WLAN_POWER_MANAGEMENT
    if (g_powermgmt_switch)
    {
        /* 去关联时，将watchdog timer删除 */
        if (cfg->hi110x_dev->pm_info != NULL)
        {
            ret = hi110x_pm_stop_wdg(cfg->hi110x_dev->pm_info);
            if (ret < 0)
            {
                HWIFI_WARNING("stop watchdog timer Failed");
            }
        }
        else
        {
            HWIFI_WARNING("pm_info ptr is NULL...");
        }
    }
#endif
    /*cancel net work exception detect timer.*/
    hcc_net_work_detect_deinit(cfg->hi110x_dev->hcc);


    return SUCC;
}



/*
* Prototype    : get_stats_struct
* Description  :
* Input        :
* Output       : None
* Return Value : None
* Calls        :
* Called By    : hwifi_get_station
*
*   History        :
*   1.Date         : 2012/9/20
*     Author       :
*     Modification : Created function
*
*/
STATIC struct stats_struct *get_stats_struct(struct cfg_struct *cfg, uint8 *mac)
{
    struct stats_struct *stats = NULL;

    if (IS_STA(cfg))
    {
        /* if current mode is STA, mac must be bssid */
        if (0 != memcmp(mac,  cfg->conn.bss.bssid, MAC_LEN))
        {
            HWIFI_WARNING("STA mode : mac("MACFMT") not match with bssid("MACFMT")", MAC2STR(mac), MAC2STR(cfg->conn.bss.bssid));
            return NULL;
        }

        stats = &cfg->conn.bss.stats;
        stats->aid = 0;
    }
    else if (IS_AP(cfg))
    {
        /* if current mode is AP, mac must match with one of STAs */

        struct conn_sta_info *sta;

        sta = find_by_mac(&cfg->ap_info.sta_mgmt, mac);

        if (NULL == sta)
        {
            HWIFI_WARNING("AP mode : find sta by mac("MACFMT") failed", MAC2STR(mac));
            return NULL;
        }

        stats      = &sta->stats;
        stats->aid = sta->aid;
    }
    else
    {
        HWIFI_WARNING("Current mode(%d) is not STA or AP!", cfg->hi110x_dev->mode.current_mode);
        return NULL;
    }

    return stats;
}

/*
* Prototype    : fill_station_info
* Description  :
* Input        :
* Output       : None
* Return Value : None
* Calls        :
* Called By    : hwifi_get_station
*
*   History        :
*   1.Date         : 2012/9/20
*     Author       :
*     Modification : Created function
*
*/
STATIC void fill_station_info(struct stats_struct *stats,  struct station_info  *sinfo)
{
    struct cfg_struct    *cfg;
    struct conn_struct   *conn;
    struct conn_bss_info *bss;
    bss  = container_of(stats, struct conn_bss_info, stats);
    conn = container_of(bss, struct conn_struct, bss);
    cfg  = container_of(conn, struct cfg_struct, conn);
    /* get connected time in seconds */
    sinfo->filled         |= STATION_INFO_CONNECTED_TIME;
    sinfo->connected_time  = (jiffies - stats->connect_ts) / HZ;
    HWIFI_DEBUG("Connected time %d seconds", sinfo->connected_time);

    /* fill station info */
    if (stats->updated != FALSE)
    {
        sinfo->filled |= STATION_INFO_SIGNAL;

        sinfo->signal = stats->signal;
        if( 0 == sinfo->signal )
        {
            sinfo->signal = cfg->latest_signal;
        }

        sinfo->filled |= STATION_INFO_RX_PACKETS;
        sinfo->filled |= STATION_INFO_TX_PACKETS;

        sinfo->rx_packets = stats->rx_packets;
        sinfo->tx_packets = stats->tx_packets;

        sinfo->filled   |= STATION_INFO_RX_BYTES;
        sinfo->filled   |= STATION_INFO_TX_BYTES;
        sinfo->rx_bytes  = stats->rx_bytes;
        sinfo->tx_bytes  = stats->tx_bytes;


        sinfo->filled |= STATION_INFO_TX_RETRIES;
        sinfo->filled |= STATION_INFO_TX_FAILED;
        sinfo->filled |= STATION_INFO_RX_DROP_MISC;
        sinfo->filled |= STATION_INFO_INACTIVE_TIME;
        sinfo->inactive_time    = 0; /* 由于当前device不支持此项的统计，默认设置为0 */

        sinfo->tx_retries       = stats->tx_retries;
        /* 当wifi高级选项:避开状态不佳的网络 打开时 */
        /* 在强信号下此统计可能会导致android上层误报率统计过大最终导致强制断网,这里清0规避此问题 */
        sinfo->tx_failed        = 0;     //stats->tx_failed;

        sinfo->rx_dropped_misc  = stats->rx_dropped_misc;

        sinfo->filled |= STATION_INFO_TX_BITRATE ;
        if( stats->tx_rate.legacy ==0 )
        {
            sinfo->txrate.legacy   = cfg->rate.legacy;
            sinfo->txrate.flags    = cfg->rate.flags;
            sinfo->txrate.mcs      = cfg->rate.mcs;

        }
        else
        {
        sinfo->txrate.legacy   = stats->tx_rate.legacy;
        sinfo->txrate.flags = stats->tx_rate.flags;
        sinfo->txrate.mcs = stats->tx_rate.mcs;
        }
        /* stats->updated = FALSE; */
    }

    HWIFI_DEBUG("fill_station_info sinfo->txrate.legacy =%d,flg=%02X,mcs=%d,"
                 "sinfo->signal=%d,stats->signal=%d,cfg->latest_signal=%d",
                 sinfo->txrate.legacy ,sinfo->txrate.flags,sinfo->txrate.mcs,
                 sinfo->signal,stats->signal,cfg->latest_signal);
}

/*
 * Prototype    : hwifi_get_station
 * Description  :
 * Input        : struct wiphy *wiphy
                  struct net_device *dev
                  uint8 *mac
 * Output       : struct station_info  *sinfo
 * Return Value : int32
 * Calls        :
 * Called By    : cfg80211.ko
 *
 *   History        :
 *   1.Date         : 2012/8/14
 *     Author       : kf74588
 *     Modification : Created function
 *
 */

STATIC int32 hwifi_cfg80211_get_station(struct wiphy         *wiphy,
                                                struct net_device    *ndev,
                                                uint8                *mac,
                                                struct station_info  *sinfo)
{
    struct hwifi_stats_param_stru stats_param;       /* stats cmd paramter */
    struct cfg_struct            *cfg;               /* wireless config struct */

    struct stats_struct      *stats;                /* station statistic information struct */
    long                      left;                 /* left time in jiffies */
    int32                     ret;

    HWIFI_ASSERT(NULL != wiphy);
    HWIFI_ASSERT(NULL != ndev);
    HWIFI_ASSERT(NULL != mac);
    HWIFI_ASSERT(NULL != sinfo);

    cfg = wiphy_priv(wiphy);

    memset(&stats_param, 0, sizeof(stats_param));

    stats = get_stats_struct(cfg, mac);
    if (NULL == stats)
    {
        return -EFAIL;
    }

    if (IS_STA(cfg))
    {
        /* STA mode,直接读取上次随心跳上报的sta信息 */
        HWIFI_DEBUG("sta mode get station info direct!");
        stats->updated = TRUE;
        fill_station_info(stats, sinfo);
        return SUCC;
    }

    stats_param.aid = stats->aid;

    /* send stats cmd to device */
    stats->updated = FALSE;
    ret = hwifi_query_stats(cfg, &stats_param);

    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to query stats!");
        return -EFAIL;
    }

    /* wait for stats result or timeout */
    left = wait_event_interruptible_timeout(cfg->wait_queue,
                                            (stats->updated != FALSE),
                                            (5 * HZ));
    if (left == 0)
    {
        HWIFI_WARNING("Get station info timeout");
        return -ETIMEDOUT;
    }
    else if (left < 0)
    {
        HWIFI_WARNING("Get station info error,ret=%ld", left);
        return left;
    }

    HWIFI_DEBUG("Get station info succ");

    fill_station_info(stats, sinfo);

    return SUCC;
}

/*
 * Prototype    : hwifi_del_station
 * Description  :
 * Input        : struct cfg_struct *cfg
                  uint8 *mac
                  uint32 reason_code
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/14
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC int32 hwifi_cfg80211_del_station(struct cfg_struct *cfg, uint8 *mac, uint16 reason_code, uint8 type)
{
    struct hwifi_disconn_param_stru   disconn_param;     /* connect param struct */
    struct conn_sta_info             *sta;               /* station information struct */
    int32 ret;

    /* only AP can delete station */
    if (!IS_AP(cfg))
    {
        HWIFI_WARNING("Current mode(%d) is not AP!", cfg->hi110x_dev->mode.current_mode);
        return -EFAIL;
    }

    if(NULL == mac)
    {
        HWIFI_WARNING("Invalid NULL mac!");
        return -EFAIL;
    }

    HWIFI_INFO("Delete station mac=["MACFMT"],reason_code=%u,type=%d", MAC2STR(mac), reason_code, type);

    memset(&disconn_param, 0, sizeof(disconn_param));

    if (0xff == (mac[0] & mac[1] & mac[2] & mac[3] & mac[4] & mac[5]))
    {
        HWIFI_INFO("Del station: all");
        disconn_param.aid = 0;
        removeall_conn_sta(&cfg->ap_info.sta_mgmt);
    }
    else
    {
        /* find station aid according to mac */
        sta = find_by_mac(&cfg->ap_info.sta_mgmt, mac);
        if (NULL == sta)
        {
            HWIFI_WARNING("find sta by mac"MACFMT" failed.", MAC2STR(mac));
            return -EFAIL;
        }

        disconn_param.aid = (uint16)(sta->aid);
        HWIFI_INFO("Sta aid = %d", sta->aid);

        if (0 == sta->aid)
        {
            HWIFI_INFO("Sta has been deleted");
            return SUCC;
        }
        remove_conn_sta(&cfg->ap_info.sta_mgmt, sta);
    }

    disconn_param.reason_code = reason_code;
    disconn_param.type = type;

    /* send delete cmd to device */
    ret = hwifi_start_disconn(cfg, &disconn_param);
    if (SUCC != ret)
    {
        HWIFI_INFO("hwifi_start_disconn failed,ret=%d", ret);
        return -EFAIL;
    }



    return SUCC;
}
int32 hwifi_cfg80211_set_pmksa(struct wiphy            *wiphy,
                                        struct net_device       *netdev,
                                        struct cfg80211_pmksa   *pmksa)
{
    struct hwifi_pmksa_stru st_pmksa;      /* pmksa infor struct */
    struct cfg_struct      *cfg;
    int32                   ret;

    HWIFI_ASSERT(NULL != pmksa);
    HWIFI_ASSERT(NULL != wiphy);
    HWIFI_ASSERT(NULL != netdev);

    memset(&st_pmksa, 0, sizeof(st_pmksa));

    cfg = wiphy_priv(wiphy);

    if (NULL == pmksa->bssid)
    {
        HWIFI_WARNING("Invalid NULL pmksa bssid!");
        return -EFAIL;
    }

    if (NULL == pmksa->pmkid)
    {
        HWIFI_WARNING("Invalid NULL pmksa pmkid");
        return -EFAIL;
    }

    /* set bssid and pmkid */
    memcpy(st_pmksa.bssid, pmksa->bssid, MAC_LEN);
    memcpy(st_pmksa.pmkid, pmksa->pmkid, PMKID_LEN);

    HWIFI_INFO("Start set pmksa,bssid:"MACFMT, MAC2STR(pmksa->bssid));
    ret = hwifi_start_set_pmksa(cfg, &st_pmksa);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to set pmksa,bssid:"MACFMT, MAC2STR(pmksa->bssid));
        return -EFAIL;
    }

    return ret;
}



int hwifi_cfg80211_add_key( struct wiphy            *wiphy,
                                    struct net_device       *netdev,
                                    unsigned char            key_index,
                                    bool                     pairwise,
                                    const unsigned char     *mac_addr,
                                    struct key_params       *params)
{
    uint8  *ap_addr;

    struct cfg_struct       *cfg;
    struct priv_key_param   *key;

    if ((NULL == wiphy) || (NULL == netdev) || (NULL == params))
    {
        HWIFI_ERROR("Invalid NULL params!");
        return -EFAIL;
    }
    cfg = wiphy_priv(wiphy);

    /* info log start */
    HWIFI_INFO("key_index:0x%x, pairwise:%d,cipher type = 0x%x, key len = %d; seq len = %d", 
                key_index, pairwise, params->cipher, params->key_len, params->seq_len);
    if (mac_addr)
    {
        HWIFI_INFO("Mac: "MACFMT,MAC2STR(mac_addr));
    }
    else
    {
        HWIFI_INFO("Mac: (null)");
    }

    switch(params->cipher)
    {
        case WLAN_CIPHER_SUITE_USE_GROUP:
            HWIFI_INFO("Cipher : USE_GROUP");
            break;
        case WLAN_CIPHER_SUITE_WEP40:
            HWIFI_INFO("Cipher : WEP40");
            break;
        case WLAN_CIPHER_SUITE_WEP104:
            HWIFI_INFO("Cipher : WEP104");
            break;
        case WLAN_CIPHER_SUITE_TKIP:
            HWIFI_INFO("Cipher : TKIP");
            break;
        case WLAN_CIPHER_SUITE_CCMP:
            HWIFI_INFO("cipher : CCMP");
            break;
        case WLAN_CIPHER_SUITE_AES_CMAC:
            HWIFI_INFO("Cipher : AES_CMAC");
            break;
        case WLAN_CIPHER_SUITE_SMS4:
            HWIFI_INFO("Cipher : SMS4");
            break;
    }
    /* info log end */


    if(key_index > MAX_IGTK_KEY_INDEX)
    {
        return -EINVAL;
    }

    if (IS_AP(cfg))
    {
        int32 delay_install = FALSE;
        const uint8 *mac;

        mac = (mac_addr)? mac_addr : broadcast;

        if ( WLAN_CIPHER_SUITE_WEP40 == params->cipher
                || WLAN_CIPHER_SUITE_WEP104 == params->cipher)
        {
            /* save static wep key, install it in add_beacon */
            key = &cfg->key_mgmt.gtk[key_index];
            cfg->ap_info.pairwise_cipher = params->cipher;
            cfg->ap_info.group_cipher = params->cipher;
            delay_install = TRUE;
        }
        else
        {
            if (pairwise)
            {
                struct conn_sta_info *sta;
                sta = find_by_mac(&cfg->ap_info.sta_mgmt, mac);
                if (NULL == sta)
                {
                    HWIFI_WARNING("Couldn't find sta, mac may be incorect, return directly");
                    return -EINVAL;
                }
                key = &sta->ptk;
                cfg->ap_info.pairwise_cipher = params->cipher;
            }
            else
            {
                key = &cfg->key_mgmt.gtk[key_index];
                cfg->ap_info.group_cipher = params->cipher;
            }
        }

        memcpy(key->key, params->key, params->key_len);
        key->key_len    = params->key_len;
        memcpy(key->seq, params->seq, params->seq_len);
        key->seq_len    = params->seq_len;
        key->cipher     = params->cipher;

        if (delay_install)
        {
            return SUCC;
        }

        return hwifi_cmd_add_key(cfg, key, (int8 *)mac, pairwise, key_index);

        //return SUCC;
    }



    if(pairwise)
    {
    /* save the key index */
    #ifdef MAC_802_11W
        cfg->key_mgmt.ptk_key_index = key_index;
    #endif

        key = &cfg->key_mgmt.ptk[key_index];
        if (NULL != mac_addr)
        {
            memcpy(cfg->key_mgmt.uni_addr, mac_addr, 6);
        }
    }
    else
    {
        #ifdef MAC_802_11W
        if (WLAN_CIPHER_SUITE_AES_CMAC == params->cipher)
        {
            /* save the key index */

            cfg->key_mgmt.igtk_key_index = key_index;


            key = &cfg->key_mgmt.igtk[key_index];
            if (NULL != mac_addr)
            {
                memcpy(cfg->key_mgmt.mgmt_mul_addr, mac_addr, 6);
            }

            /* PMF功能当前没有在device侧做加解密，保存密钥后，无需下发密钥，直接返回 */
            memcpy(key->key, params->key, params->key_len);
            key->key_len    = params->key_len;
            memcpy(key->seq, params->seq, params->seq_len);
            key->seq_len    = params->seq_len;
            key->cipher     = params->cipher;

            HWIFI_INFO("Receive AES_CMAC.");
            return SUCC;
        }
        else
        #endif
        {
            /* save the key index */
        #ifdef MAC_802_11W
            cfg->key_mgmt.gtk_key_index = key_index;
        #endif

            key = &cfg->key_mgmt.gtk[key_index];
            if (NULL != mac_addr)
            {
                memcpy(cfg->key_mgmt.mul_addr, mac_addr, 6);
            }
        }
    }

    memcpy(key->key, params->key, params->key_len);
    key->key_len    = params->key_len;
    memcpy(key->seq, params->seq, params->seq_len);
    key->seq_len    = params->seq_len;
    key->cipher     = params->cipher;

    if (WLAN_CIPHER_SUITE_SMS4 == key->cipher)
    {
        /* just for sta mode, ap mode need to do */
        ap_addr = cfg->conn.bss.bssid;
        return hwifi_cmd_add_wapi_key(cfg, pairwise, key_index, ap_addr, key);
    }

    return hwifi_cmd_add_key(cfg, key, (char *)cfg->key_mgmt.uni_addr, pairwise, key_index);
}


int hwifi_cfg80211_get_key(struct wiphy      *wiphy,
                                   struct net_device *netdev,
                                   uint8              key_index,
                                   bool               pairwise,
                                   const uint8       *mac_addr,
                                   void              *cookie,
                                   void               (*callback)(void *cookie, struct key_params*))
{
    struct cfg_struct *cfg;
    struct priv_key_param   *priv_key;
    struct key_params        key;

    cfg = wiphy_priv(wiphy);

    /* info log start */
    HWIFI_INFO("key_index:0x%x, pairwise:%d", key_index, pairwise);
    if (mac_addr)
    {
        HWIFI_INFO("Mac:"MACFMT,MAC2STR(mac_addr));
    }
    else
    {
        HWIFI_INFO("Mac: (null)");
    }
    /* info log end */

    if(key_index > 3)
    {
        HWIFI_WARNING("Invalid key index:%d!",key_index);
        return -EINVAL;
    }

    if (IS_AP(cfg))
    {
        if (pairwise)
        {
            struct conn_sta_info *sta;
            sta = find_by_mac(&cfg->ap_info.sta_mgmt, mac_addr);
            if (NULL == sta)
            {
                return -EINVAL;
            }
            priv_key = &sta->ptk;
        }
        else
        {
            priv_key = &cfg->key_mgmt.gtk[key_index];
        }

        memset(&key, 0, sizeof(key));

        key.key = priv_key->key;
        key.key_len = priv_key->key_len;

        key.seq = priv_key->seq;
        key.seq_len = priv_key->seq_len;
        key.cipher  = priv_key->cipher;

        callback(cookie, &key);

        return 0;
    }

    if(pairwise)
    {
        priv_key = &cfg->key_mgmt.ptk[key_index];
    }
    else
    {
        priv_key = &cfg->key_mgmt.gtk[key_index];
    }

    memset(&key, 0, sizeof(key));
    key.key = priv_key->key;
    key.key_len = priv_key->key_len;
    key.seq = priv_key->seq;
    key.seq_len = priv_key->seq_len;
    key.cipher  = priv_key->cipher;

    callback(cookie, &key);

    return 0;
}


int hwifi_cfg80211_set_default_key(struct  wiphy      *wiphy,
                                             struct  net_device *netdev,
                                             uint8               key_index,
                                             bool                unicast,
                                             bool                multicast)
{
    struct cfg_struct *cfg;
    struct priv_key_param   *priv_key;
    int ret = 0;
    cfg = wiphy_priv(wiphy);

    /* info log start */
    HWIFI_INFO("Key_index:0x%x, unicast:%d, multicast:%d", key_index, unicast, multicast);
    /* info log end */

    if(key_index > 3)
    {
        HWIFI_WARNING("SET_DEFAULT_KEY : key_index = %d, out of range", key_index);
        return -EINVAL;
    }

    if (IS_AP(cfg))
    {
        if ((WLAN_CIPHER_SUITE_WEP40 == cfg->ap_info.group_cipher)
        || (WLAN_CIPHER_SUITE_WEP104 == cfg->ap_info.group_cipher))
        {
            cfg->key_mgmt.default_index = key_index;
            return SUCC;
        }
        else
        {
        }
    }


    if (unicast && multicast)
    {
        priv_key    = &cfg->key_mgmt.gtk[key_index];
    }
    else if (unicast)
    {
        priv_key    = &cfg->key_mgmt.ptk[key_index];
    }
    else if (multicast)
    {
        priv_key    = &cfg->key_mgmt.gtk[key_index];
    }
    else
    {
        HWIFI_WARNING("Invalid mode not ptk or gtk!");
        return -EINVAL;
    }

    /* if sms4, wep40, wep104, don't need to enable key, return direct */
    if ((WLAN_CIPHER_SUITE_SMS4 == priv_key->cipher)
       || (WLAN_CIPHER_SUITE_WEP40 == priv_key->cipher)
       || (WLAN_CIPHER_SUITE_WEP104 == priv_key->cipher))
    {
        HWIFI_INFO("Don't need to enable key");
        return SUCC;
    }

    ret = hwifi_cmd_set_defkey(cfg, priv_key, cfg->key_mgmt.uni_addr, key_index, unicast);
    /*
     * in this case need to enable both pairwise and group key
     */
    if (unicast && multicast)
    {
        ret |= hwifi_cmd_set_defkey(cfg, priv_key, cfg->key_mgmt.uni_addr, key_index, !unicast);
    }

/* 4次握手，先设置密钥，再发送第四帧给AP，如果此时下发数据，则对方无法解密 */
/* 在wpa_supplicant真正成功后，下发数据 */
#if 0
#ifdef __ROAM__
    if (unicast)
    {
        roam_process_addkey(&cfg->roam);
    }
#endif
#endif
    return ret;
}


int hwifi_cfg80211_remove_key(struct wiphy       *wiphy,
                                        struct net_device  *netdev,
                                        uint8               key_index,
                                        bool                pairwise,
                                        const uint8        *mac_addr)
{
    struct cfg_struct     *cfg;
    struct priv_key_param *priv_key;
    int                    ret = 0;
    uint8                 *ap_addr;

    cfg = wiphy_priv(wiphy);

    /* info log start */
    HWIFI_INFO("Key_index:0x%x, pairwise:%d", key_index, pairwise);
    if (mac_addr)
    {
        HWIFI_INFO("Mac:"MACFMT, MAC2STR(mac_addr));
    }
    else
    {
        HWIFI_INFO("Mac: (null)");
    }
    /* info log end */

    if (key_index > MAX_IGTK_KEY_INDEX)
    {
        HWIFI_WARNING("REMOVE_KEY : key_index = %d, out of range", key_index);
        return -EINVAL;
    }

    if (IS_AP(cfg))
    {
        const uint8 *mac;
        mac = (mac_addr)? mac_addr : broadcast;

        if (pairwise)
        {
            struct conn_sta_info *sta;
            sta = find_by_mac(&cfg->ap_info.sta_mgmt, mac);
            if (NULL == sta)
            {
                return -EINVAL;
            }
            priv_key = &sta->ptk;
        }
        else
        {
            priv_key = &cfg->key_mgmt.gtk[key_index];
        }

        if (priv_key->key_len != 0)
        {
            ret = hwifi_cmd_remove_key(cfg, (char *)mac, key_index, pairwise, priv_key->cipher);
            if (ret == 0)
            {
                memset(priv_key, 0, sizeof(*priv_key));
            }
        }
        else
        {
            HWIFI_INFO("REMOVE_KEY : %s key [index = %d] is empty, no need to remove\n",pairwise ? "[ptk]" : "[gtk]", key_index);
        }

        return ret;
    }

    if(pairwise)
    {
        HWIFI_INFO("REMOVE_KEY : remove ptk key, index[%d]", key_index);
        priv_key = &cfg->key_mgmt.ptk[key_index];
    }
    else
    {
        HWIFI_INFO("REMOVE_KEY : remove gtk key, index[%d]",key_index);
    #ifdef MAC_802_11W
        if (key_index > 3)
        {
            priv_key = &cfg->key_mgmt.igtk[key_index];
        }
        else
    #endif
        {
            priv_key = &cfg->key_mgmt.gtk[key_index];
        }
    }

    if (priv_key->key_len != 0)
    {
        if(WLAN_CIPHER_SUITE_SMS4 == priv_key->cipher)
        {
            ap_addr = cfg->conn.bss.bssid;
            ret = hwifi_cmd_remove_wapi_key(cfg, pairwise, key_index, ap_addr);
        }
    #ifdef MAC_802_11W
        else if (WLAN_CIPHER_SUITE_AES_CMAC == priv_key->cipher)
        {
            ret = 0;
        }
    #endif
        else
        {
            ret = hwifi_cmd_remove_key(cfg, (char *)cfg->key_mgmt.uni_addr, key_index, pairwise, priv_key->cipher);
        }

        if (ret == 0)
        {
            memset(priv_key, 0, sizeof(*priv_key));
        }
    }
    else
    {
        HWIFI_INFO("REMOVE_KEY : %s key [index = %d] is empty, no need to remove\n",pairwise ? "[ptk]" : "[gtk]", key_index);
    }

    return ret;
}



int32 hwifi_ap_init(struct cfg_struct *cfg)
{

    HWIFI_ASSERT(NULL != cfg);

    HWIFI_DEBUG("Init ap mode!");

    station_mgmt_init(&cfg->ap_info.sta_mgmt);

    cfg->aggre_info.aggr_start = FALSE;
    cfg->aggre_info.aggre_func = TRUE;

    HWIFI_DEBUG("AP_INIT : init successful");
    return SUCC;
}


int32 hwifi_ap_deinit(struct cfg_struct *cfg)
{
    HWIFI_ASSERT(NULL != cfg);

    HWIFI_DEBUG("Deinit ap mode!");
    cfg->hi110x_dev->mode.current_mode = HI_MODE_STA;
    cfg->hi110x_dev->mode.next_mode    = HI_MODE_STA;
    cfg->conn.status                   = HWIFI_DISCONNECTED;
    cfg->conn.auto_authtype            = FALSE;
    cfg->conn.reconn_msg               = NULL;
    cfg->beacon_set                    = FALSE;
    cfg->aggre_info.aggr_start         = FALSE;
    cfg->aggre_info.aggre_func         = TRUE;

    HWIFI_DEBUG("AP_DEINIT : deinit ap successful");
    return SUCC;
}



static const char* itype2name(enum nl80211_iftype  type)
{
    switch(type)
    {
        case NL80211_IFTYPE_AP:
            return "AP";
        case NL80211_IFTYPE_P2P_GO:
            return "GO";
        case NL80211_IFTYPE_STATION:
            return "STA";
        case NL80211_IFTYPE_P2P_CLIENT:
            return "P2P client";
        default:
            break;

    }

    return "other";
}


int32 hwifi_cfg80211_change_mode(struct wiphy       *wiphy,
                                            struct net_device   *ndev,
                                            enum nl80211_iftype  type,
                                            uint32              *flags,
                                            struct vif_params   *params)
{
    struct hwifi_mode_param_stru    mode_param;    /* mode param struct */
    struct cfg_struct              *cfg;
    int32                           ret;
    int32                           p2p_mode = P2P_CLIENT;

    HWIFI_ASSERT(NULL != wiphy);
    HWIFI_ASSERT(NULL != ndev);

    cfg = wiphy_priv(wiphy);

    /* info log start */
    HWIFI_INFO("Change mode  form %s to %s",
            itype2name(ndev->ieee80211_ptr->iftype), itype2name(type));
    /* info log end */

    switch(type)
    {
        case NL80211_IFTYPE_AP:
        case NL80211_IFTYPE_P2P_GO:
            cfg->hi110x_dev->mode.next_mode  = HI_MODE_AP;
            break;
        case NL80211_IFTYPE_STATION:
        case NL80211_IFTYPE_P2P_CLIENT:
            cfg->hi110x_dev->mode.next_mode  = HI_MODE_STA;
            break;
        case NL80211_IFTYPE_AP_VLAN:
        case NL80211_IFTYPE_MONITOR:
        case NL80211_IFTYPE_WDS:
        case NL80211_IFTYPE_MESH_POINT:
        case NL80211_IFTYPE_ADHOC:
            HWIFI_WARNING("Not support the mode now!");
            return -EOPNOTSUPP;
        default:
            HWIFI_WARNING("Invalid mode can not parse!");
            return -EINVAL;
    }

    if (cfg->hi110x_dev->mode.current_mode == cfg->hi110x_dev->mode.next_mode)
    {
        HWIFI_INFO("Mode need not been modified, return directly");
        ndev->ieee80211_ptr->iftype       = type;

        return SUCC;
    }

    memset(&mode_param, 0, sizeof(mode_param));
    mode_param.next_mode = cfg->hi110x_dev->mode.next_mode;

    /* send msg */
    ret = hwifi_start_change_mode(cfg, &mode_param);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to change mode!");
        return -EIO;
    }

    /* wait for change complete ack */
    ret = wait_event_timeout(cfg->wait_queue, (cfg->hi110x_dev->mode.current_mode == cfg->hi110x_dev->mode.next_mode), 3 * HZ);
    if (0 == ret)
    {
        HWIFI_WARNING("Wait event queue time out!");
        return -EFAIL;
    }
    HWIFI_INFO("Wait event queue ret = %d, cur_mode=%d", ret, cfg->hi110x_dev->mode.current_mode);

    /* get the change mode result */
    if (cfg->hi110x_dev->mode.current_mode != cfg->hi110x_dev->mode.next_mode)
    {
        HWIFI_WARNING("Failed to change mode to what we want,cur_mode=%d,next_mode=%d.", 
                        cfg->hi110x_dev->mode.current_mode, cfg->hi110x_dev->mode.next_mode);
        return -EFAIL;
    }

    /* update current mode */
    ndev->ieee80211_ptr->iftype       = type;



    /* init mode */
    if (IS_AP(cfg))
    {
        p2p_mode = P2P_GO;
        ret = hwifi_ap_init(cfg);
    }
    else if (IS_STA(cfg))
    {
        p2p_mode = P2P_CLIENT;
        ret = hwifi_ap_deinit(cfg);
    }

    if ((SUCC == ret) && (P2P_ON(cfg)))
    {
        ret = hwifi_p2p_flag_set(cfg, p2p_mode);
    }


#ifdef WLAN_ARRG_DYNAMIC_CONTROL
    hwifi_stop_timer(cfg);
#endif

    return ret;
}


int32  hwifi_check_beacon_param(int16    *dst_param,
                                           int32     src_param,
                                           int32     maxvalue,
                                           int32     minvalue,
                                           int32     no_change)
{
    if (NULL == dst_param)
    {
        HWIFI_WARNING("Invalid NULL dst param!");
        return -EFAIL;
    }

    if ((src_param <= maxvalue) && (src_param >= minvalue))
    {
        *dst_param = (uint16)src_param;
    }
    else if (no_change == src_param)
    {
        *dst_param = no_change;
        HWIFI_INFO("The param doesn't change!");
    }
    else
    {
       HWIFI_WARNING("Invalid param!");
       return -EFAIL;
    }

    return SUCC;
}



int32 wpa_parse_wpa_ie_rsn(struct hwifi_elements_stru *hdr, struct hwifi_beacon_param_stru *data)
{
    uint8 *pos;
    int32 left;
    int32 i;
    int32 count;

    if ((NULL == hdr) || (NULL == data))
    {
        HWIFI_WARNING("Invalid NULL param!");
        return -EFAIL;
    }

    data->crypto_mode |= HWIFI_WPA2;

    pos = (uint8 *)(hdr + 1);
    left = hdr->element_len - sizeof(struct hwifi_elements_stru);

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
            if (RSN_SELECTOR_GET(pos) == RSN_AUTH_KEY_MGMT_UNSPEC_802_1X)
            {
                data->auth_type |= HWIFI_8021X;
            }
            else if(RSN_SELECTOR_GET(pos) != RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X)
            {
                HWIFI_WARNING("Invalid auth type!");
                return -EFAIL;
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


static int32 wpa_parse_wpa_ie_wpa(const uint8 *ie, int32 ie_len, struct hwifi_beacon_param_stru *data)
{
    const uint8 *pos;
    int32  left;
    int32  i;
    int32  count;

    if ((NULL == ie) || (NULL == data))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    data->crypto_mode |= HWIFI_WPA;

    /* pos前移8个字节，信息元素id 1个字节，长度1个字节，oui 4个字节，version 2个字节*/
    pos = ie + 8;
    left = ie_len - 8;

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
            if (RSN_SELECTOR_GET(pos) == WPA_AUTH_KEY_MGMT_UNSPEC_802_1X)
            {
                data->auth_type |= HWIFI_8021X;
            }
            else if(RSN_SELECTOR_GET(pos) != WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X)
            {
                HWIFI_WARNING("Invalid not support auth key management.");
                return -EFAIL;
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
static int32 parse_ie(const uint8 *ie, int32 ie_len, struct hwifi_beacon_param_stru *data)
{
    const uint8   *pos;
    uint32   oui;
    uint32   left = ie_len;
    uint8    id;
    uint8    elen;

    pos = ie;
    while (left >= 2)
    {
        id = *pos++;
        elen = *pos++;
        left -= 2;

        if (elen > left)
        {
            HWIFI_WARNING("Invalid element len(%d) longer than left len(%d),parse failed.", elen, left);
            return -EFAIL;
        }

        if (WLAN_EID_VENDOR_SPECIFIC == id)
        {
            oui = WPA_GET_BE32(pos);
            switch (oui)
            {
                case WPA_IE_VENDOR_TYPE:
                    /* 减2是为了将pos指向这个IE的 ID */
                    wpa_parse_wpa_ie_wpa(pos - 2, elen + 2, data);
                    break;

                case WPS_IE_VENDOR_TYPE:
                    data->wps_ie     = pos  - 2;
                    data->wps_ie_len = elen + 2;
                    break;

                case P2P_IE_VENDOR_TYPE:
                    data->p2p_ie     = pos  - 2;
                    data->p2p_ie_len = elen + 2;
                    break;

                default:
                    break;

            }
        }

        left -= elen;
        pos += elen;
    }

    return SUCC;
}
int32 hwifi_get_cur_channel(struct cfg_struct *cfg)
{
    int32 ret;

    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return -EFAIL;
    }

    cfg->ap_info.curr_channel = SUCC;

    ret = wl_get_channel_info(cfg);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to get channel info!");
        return -EFAIL;
    }

    ret = wait_event_interruptible_timeout(cfg->wait_queue, (SUCC != cfg->ap_info.curr_channel), 5 * HZ);
    if (0 == ret)
    {
        HWIFI_WARNING("wait_event_queue time out!");
        return -EFAIL;
    }
    else if (ret < 0)
    {
        HWIFI_WARNING("wait_event_queue internal error,ret=%d", ret);
        return -EFAIL;
    }

    HWIFI_DEBUG("curr_channel=%d",cfg->ap_info.curr_channel);
    return cfg->ap_info.curr_channel;

}


STATIC void hwifi_cfg80211_get_channel_info(struct cfg_struct *cfg, struct cfg80211_chan_def *chan_def, struct hwifi_beacon_param_stru  *beacon_param)
{
    enum nl80211_channel_type channel_type;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != chan_def);
    HWIFI_ASSERT(NULL != beacon_param);

    /*
    channel_info:
    BIT15：0->2.4G，1->5G
    BIT9-8：0->SCN，1->SCA，3->SCB（20M情况上设置为SCN，40M情况选择SCA或SCB）
    BIT7-0：主信道号
    如果参数低8BITS配置为0，则AP在建立网络过程表示自动信道选择。
    */
    if ((cfg->ap_info.auto_channel) && (!IS_P2P_ON(cfg)))
    {
        /* auto channel 当p2p开启，没有自动信道功能 */
	 beacon_param->channel = 0;
    }
    else
    {
        cfg->ap_info.curr_channel = (0xFF & chan_def->chan->hw_value);
        beacon_param->channel = (0xFF & chan_def->chan->hw_value);
    }

   channel_type = cfg80211_get_chandef_type((const struct cfg80211_chan_def *)chan_def);

    if((NL80211_CHAN_HT40MINUS == channel_type) && (cfg->ap_info.ht_enabled))
    {
        beacon_param->enable_20M_40M = 1;
        beacon_param->channel |= HWIFI_40M_MINUS;
    }
    else if((NL80211_CHAN_HT40PLUS == channel_type) && (cfg->ap_info.ht_enabled))
    {
        beacon_param->enable_20M_40M = 1;
        beacon_param->channel |= HWIFI_40M_PLUS;
    }

    if (IEEE80211_BAND_5GHZ == chan_def->chan->band)
    {
        beacon_param->channel |= HWIFI_BAND_5G;
    }

    beacon_param->ht_enabled = cfg->ap_info.ht_enabled;

    if (255 != cfg->ap_info.fc_enabled)
    {
        beacon_param->enable_20M_40M = cfg->ap_info.fc_enabled;
    }

    cfg->ap_info.channel_info =  beacon_param->channel;

    HWIFI_INFO("channel=0x%.2x,enable_20M_40M=%d,coexist_20M_40M=%d,ht_enabled=%d",
                 beacon_param->channel, beacon_param->enable_20M_40M, beacon_param->coexist_20M_40M, cfg->ap_info.ht_enabled);

}
STATIC int32 hwifi_cfg80211_stop_ap(struct wiphy *wiphy, struct net_device *dev)
{
	/* use to clear kernel status */
	return SUCC;
}
STATIC int32 hwifi_cfg80211_start_ap(struct wiphy *wiphy, 
                                          struct net_device        *ndev,
			                     struct cfg80211_ap_settings *settings)
{
    struct hwifi_beacon_param_stru  beacon_param;  /* beacon infor struct */
    struct ieee80211_mgmt          *mgmt;
    struct cfg_struct              *cfg;
    struct hwifi_elements_stru     *rsn_ie;
    struct hwifi_elements_stru     *ht_cap_ie;
    uint16                          ht_cap_info;
    uint8                           short_gi_enabled;
    uint16                          beacon_capability;
    struct priv_key_param          *key;
    int32                           key_index;
    int32                           ret;

	if ((NULL == wiphy) || (NULL == ndev) || (NULL == settings) || (NULL == settings->beacon.head))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    cfg = wiphy_priv(wiphy);

    memset(&beacon_param, 0, sizeof(struct hwifi_beacon_param_stru));
	beacon_param.interval = settings->beacon_interval;
	beacon_param.dtim_period = settings->dtim_period;
    hwifi_cfg80211_get_channel_info(cfg, &settings->chandef, &beacon_param);
	beacon_param.ssid = settings->ssid;
	beacon_param.ssid_len = settings->ssid_len;
	cfg->ap_info.ssid_len = settings->ssid_len;
	memcpy(cfg->ap_info.ssid, settings->ssid, settings->ssid_len);

	if (NL80211_HIDDEN_SSID_NOT_IN_USE == settings->hidden_ssid)
    {
        /* 隐藏SSID两种情况:(1)长度为0，(2)长度为实际ssid长度，但内容为空 */
	      beacon_param.ssid_is_hidden = 0;
        }
	else if ((NL80211_HIDDEN_SSID_ZERO_CONTENTS == settings->hidden_ssid) || (NL80211_HIDDEN_SSID_ZERO_LEN == settings->hidden_ssid))
        {
                beacon_param.ssid_is_hidden = 1;
            }

    ht_cap_info = 0;
    short_gi_enabled = 0;

	ht_cap_ie = (struct hwifi_elements_stru*)cfg80211_find_ie(WLAN_EID_HT_CAPABILITY, settings->beacon.tail, settings->beacon.tail_len);
    if(ht_cap_ie)
    {
        /* ht_capabilities_info在前2个字节 */
        ht_cap_info = WPA_GET_LE16(ht_cap_ie->element_data);

        HWIFI_INFO("ht_cap_len=%d,ht_capabilities_info=0x%04x",
                        ht_cap_ie->element_len, ht_cap_info);

        if((IEEE80211_HT_CAP_SGI_20 & ht_cap_info) || (IEEE80211_HT_CAP_SGI_40 & ht_cap_info))
        {
            /* 当前device不区别SGI_20，SGI_40，只要有一个开启，则短GI开启 */
            short_gi_enabled = 1;
        }
        else
        {
            short_gi_enabled = 0;
        }
    }

    HWIFI_INFO("ht_capabilities_info=0x%04x,short_gi_enabled=%d", ht_cap_info, short_gi_enabled);
    ret = hwifi_set_short_gi(cfg, short_gi_enabled);
    if (SUCC != ret)
    {
        HWIFI_ERROR("Failed to set short GI short_gi_enabled :%d",short_gi_enabled);
        return -EFAIL;
    }

    /* get IE */
	if (NULL != settings->beacon.tail)
    {
        /* get RSN ie */
	    rsn_ie = (struct hwifi_elements_stru*)cfg80211_find_ie(WLAN_EID_RSN, settings->beacon.tail, settings->beacon.tail_len);
        if (NULL != rsn_ie)
        {
            /* 根据RSN 信息元素解析出认证类型 */
            ret = wpa_parse_wpa_ie_rsn(rsn_ie, &beacon_param);
            if (SUCC != ret)
            {
                HWIFI_WARNING("Failed to parse wpa ie rsn!");
                return -EFAIL;
            }
        }

        /* 查找wpa信息元素，并解析出认证类型，获取wps，p2p信息元素 */
	    ret = parse_ie(settings->beacon.tail, settings->beacon.tail_len, &beacon_param);
        if (SUCC != ret)
        {
            HWIFI_WARNING("Failed to parse ie!");
            return -EFAIL;
        }
    }

    if (beacon_param.crypto_mode)
    {
        beacon_param.crypto_mode |= HWIFI_ENCRYPT;
    }
    else
    {
        /* 优先判断是否是wpa-psk认证，如果不是，判断是否是wep加密 */
	    mgmt = (struct ieee80211_mgmt *)settings->beacon.head;
        beacon_capability = OS_LE16_TO_CPU(mgmt->u.beacon.capab_info);
        HWIFI_DEBUG("Beacon_capability = 0x%.2x", beacon_capability);
        if(WLAN_CAPABILITY_PRIVACY & beacon_capability)
        {
            /* WEP 加密方式 */
            key_index = cfg->key_mgmt.default_index;
            HWIFI_DEBUG("Wep key_index(%d)", key_index);
            if ((0 > key_index)|| (MAX_KEY_NUM < key_index))
            {
                HWIFI_WARNING("Wep key_index(%d) is not set", key_index);
                return -EFAIL;
            }

            key = &cfg->key_mgmt.gtk[key_index];
            if (WLAN_CIPHER_SUITE_WEP40 == key->cipher)
            {
                beacon_param.crypto_mode |= HWIFI_WEP;
            }
            else if(WLAN_CIPHER_SUITE_WEP104 == key->cipher)
            {
                beacon_param.crypto_mode |= HWIFI_WEP104;
            }
            else
            {
                HWIFI_WARNING("Invalid not support cipher:%u", key->cipher);
                return -EFAIL;
            }

            beacon_param.crypto_mode |= HWIFI_ENCRYPT;
            ret = ap_install_wep_key(cfg);
            if (SUCC != ret)
            {
                HWIFI_WARNING("Failed to install wep key!");
                return -EFAIL;
            }
        }
    }

    if (HWIFI_AUTHTYPE_SHARED_KEY == cfg->ap_info.dot11_auth_mode)
    {
        /* 只能为WEP SHARE KEY */
        if ((HWIFI_WPA2 | HWIFI_WPA) & beacon_param.crypto_mode)
        {
            HWIFI_WARNING("Both user wep shared key and wpa or wpa2,set failed.");
            return -EFAIL;
        }
        beacon_param.auth_type = HWIFI_AUTHTYPE_SHARED_KEY;
    }
    else
    {
        beacon_param.auth_type |= HWIFI_AUTHTYPE_OPEN_SYSTEM;
    }

    /* send msg */
    HWIFI_INFO("Start add beacon,auth_type=%d,crypto_mode=%d,pairwise_crypt=%d,group_crypt=%d",
                beacon_param.auth_type, beacon_param.crypto_mode, beacon_param.pairwise_crypto, beacon_param.group_crypto);

    ret = hwifi_start_addset_beacon(cfg, &beacon_param);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to start addset beacon!");
        return -EFAIL;
    }

    /* 管制域设置标志 */
    cfg->beacon_set = TRUE;

    /* AP swf antijam config */
    sfw_antijam_dynamic_config(cfg);

    return ret;

}



int32  hwifi_cfg80211_change_bss(struct wiphy           *wiphy,
                                           struct net_device      *ndev,
                                           struct bss_parameters  *bss_params)
{
    struct hwifi_hwmode_param_stru hwmode_param;
    struct cfg_struct           *cfg;         /* wireless config struct */
    int32                        ret;

    HWIFI_ASSERT(NULL != bss_params);
    HWIFI_ASSERT(NULL != wiphy);
    HWIFI_ASSERT(NULL != ndev);

    cfg = wiphy_priv(wiphy);

    ret = -EOPNOTSUPP;

    if (NULL != bss_params->basic_rates)
    {
        memset(&hwmode_param, 0, sizeof(hwmode_param));
        hwmode_param.hw_mode = cfg->ap_info.hw_mode;

        HWIFI_INFO("Start set hw_mode:%d", hwmode_param.hw_mode);

        ret = hwifi_start_set_hwmode(cfg, &hwmode_param);
        if(SUCC != ret)
        {
            HWIFI_WARNING("CHANGE_BSS : start set hwmode failed!");
            return -EFAIL;
        }
    }


    return ret;
}
 STATIC int32 hwifi_cfg80211_set_power_mgmt(struct wiphy         *wiphy,
                                                   struct net_device *ndev,
                                                   bool               enabled,
                                                   int32              timeout)
{
    struct cfg_struct           *cfg;         /* wireless config struct */
    int32                        ret;
    uint32                       ps_mode;

    HWIFI_ASSERT(NULL != wiphy);
    HWIFI_ASSERT(NULL != ndev);

    cfg = wiphy_priv(wiphy);

    if(HWIFI_DISCONNECTED == cfg->conn.status)
    {
        HWIFI_DEBUG("current status is HWIFI_DISCONNECTED,reject set power save");
        return 0;
    }


    if (g_keepconnect_switch)
    {
        HWIFI_INFO("Plugged on or always connect");
        ps_mode = HI110X_POWERMODE_MIN;
    }
    else
    {
        /**
         * P2P模式配置省电模式1，屏亮灭不改变省电模式
         */
        if (IS_P2P_ON(cfg))
        {
            ps_mode = HI110X_POWERMODE_MIN;
        }
        else
        {
            if(cfg->hi110x_dev->pm_info->in_suspend)
            {
                /* 暗屏,TBD:根据需求改变,DTIM1功耗不满足要求，更改为DTIM3 */
                ps_mode = HI110X_POWERMODE_MAX;
            }
            else
            {
                /* 亮屏 */
                ps_mode = HI110X_POWERMODE_MIN;
            }
        }
    }

    ps_mode = enabled ?
              ((cfg->sta_info.manual_pwrm != 0xFF) ? cfg->sta_info.manual_pwrm : ps_mode)
              : HI110X_POWERMODE_OFF;

    HWIFI_DEBUG("Cfg80211 set power save mode %u,screen_state=%d, manual_pwrm=0x%x,enabled=%d,timeout=%d",
                    ps_mode, cfg->hi110x_dev->pm_info->screen_state, cfg->sta_info.manual_pwrm, enabled, timeout);

    /* 如果上层有需要关闭省电的场景，在关闭省电时，停止watchdog */
    if (false == ps_mode)
    {
        hi110x_pm_stop_wdg(cfg->hi110x_dev->pm_info);
    }
    cancel_delayed_work_sync(&cfg->pwrm_set_work);

    ret = wl_pwrm_set(cfg, ps_mode);
    if(SUCC != ret)
    {
        HWIFI_WARNING("wl_pwrm_set return failed,ret=%d",ret);
        return -EFAIL;
    }

    if(ps_mode)
    {
        ret = wl_pwrm_enable(cfg, POWER_SAVE_ENABLE);
    }
    else
    {
        ret = wl_pwrm_enable(cfg, POWER_SAVE_DISABLE);
    }

    if(SUCC != ret)
    {
        HWIFI_WARNING("wl_pwrm_enable return failed,ret=%d",ret);
        return -EFAIL;
    }

    return ret;
}
int32    hwifi_cfg80211_mgmt_tx (struct wiphy               *wiphy,
                                        struct wireless_dev      *wdev,
                                        struct ieee80211_channel   *chan,
                                        bool                        offchan,
                                        uint32                      wait,
                                        const u8                   *buf,
                                        size_t                      len,
                                        bool                        no_cck,
                                        bool                        dont_wait_for_ack,
                                        u64                        *cookie)
{
    const struct ieee80211_mgmt *mgmt;
    struct cfg_struct           *cfg;
    uint32                       channel;
    int                          ret;

    if ((NULL == wiphy) || (NULL == wdev) || (NULL == buf))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    cfg = wiphy_priv(wiphy);
    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return -EFAIL;
    }

    mgmt    = (const struct ieee80211_mgmt *)buf;
    channel = ieee80211_frequency_to_channel(chan->center_freq);

    if (ieee80211_is_probe_resp(mgmt->frame_control))
    {
        *cookie = 0; /* set cookie default value */
        //ret = hwifi_send_mgmt_frame(cfg,channel,buf,len);
        /* host should not send PROE RESPONSE,
           device will send immediately when receive probe request packet */
        return SUCC;
    }
    else if(ieee80211_is_action(mgmt->frame_control))
    {
        *cookie = cfg->send_action_id++;   /* cookie值上层调用需要判断是否是这次的发送导致的callback */
        if (*cookie == 0)
        {
            *cookie = cfg->send_action_id++;
        }

		/* 为避免P2P Action报文发出后，定时器可能马上超时导致无法接收对端的Action报文的问题
		 * 每次发送完成Action报文后，更新监听定时器为500毫秒，以便有足够时间等待对端回应Action报文 */
		ret = mod_timer_pending(&(cfg->p2p_info.listen_timer), (jiffies + msecs_to_jiffies(500)));
		HWIFI_DEBUG("mod_timer_pending ret = %d", ret);

        ret = hwifi_send_mgmt_frame(cfg,channel,buf,len);
        if (SUCC == ret)
        {
            HWIFI_DEBUG("Send action success,cookie=%llu", *cookie);
            cfg80211_mgmt_tx_status(wdev, *cookie, buf, len, TRUE, GFP_KERNEL);
        }
        else
        {
            HWIFI_WARNING("Send action failed,cookie=%llu", *cookie);
            cfg80211_mgmt_tx_status(wdev, *cookie, buf, len, FALSE, GFP_KERNEL);
        }
    }
    else if(ieee80211_is_deauth(mgmt->frame_control))
    {
        ret = hwifi_cfg80211_del_station(cfg, (uint8 *)(mgmt->da), (uint16)(mgmt->u.deauth.reason_code), DEAUTH_TYPE);
    }
    else if(ieee80211_is_disassoc(mgmt->frame_control))
    {
        ret = hwifi_cfg80211_del_station(cfg, (uint8 *)(mgmt->da), (uint16)(mgmt->u.disassoc.reason_code), DISASSOC_TYPE);
    }
    else
    {
        HWIFI_WARNING("Frame type:0x%.2x is not allowed to send.", mgmt->frame_control);
        ret = -EFAIL;
    }

    return ret;

}


int32 hwifi_p2p_flag_set(struct cfg_struct *cfg, int32 flag)
{
    int32 ret;
    struct hi110x_device *hi110x_dev;
    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return -EFAIL;
    }

    HWIFI_INFO("current p2p flag is %d, set p2p flag:%d to device,", P2P_ON(cfg), flag);

    if (flag == P2P_ON(cfg))
    {
        return SUCC;
    }
    hi110x_dev = cfg->hi110x_dev ;
    ret = hwifi_send_p2p_flag(cfg, flag);
    if (SUCC != ret)
    {
		/* 保持驱动与上层P2P状态标志一致,忽略此错误，仅保留打印信息
		 * wlan up会重新下发此标志 */
		HWIFI_WARNING("Failed to set p2p flag to device.");
    }

    hwifi_scan_done(cfg);

    P2P_ON(cfg) = flag;

    HWIFI_INFO("success set p2p flag,current p2p flag is %d", P2P_ON(cfg));
    return SUCC;
}


bool hwifi_p2p_flag_get(struct cfg_struct *cfg)
{
    return cfg->p2p_info.on;
}


int32 hwifi_get_cur_mode(struct cfg_struct *cfg)
{
    int32 cur_mode;

    HWIFI_ASSERT(cfg != NULL);

    if (IS_P2P_ON(cfg))
    {
        if (IS_CONNECTED(cfg))
        {
            if (IS_STA(cfg))
            {
                /* P2P Client mode */
                cur_mode = HI_MODE_P2P_CLIENT;
            }
            else
            {
                /* P2P GO mode */
                cur_mode = HI_MODE_P2P_GO;
            }
        }
        else
        {
            /* P2P mode */
            cur_mode = HI_MODE_P2P_DEV;
        }
    }
    else
    {
        if (IS_STA(cfg))
        {
            /* STA mode */
            cur_mode = HI_MODE_STA;
        }
        else
        {
            /* AP mode */
            cur_mode = HI_MODE_AP;
        }
    }

    return cur_mode;
}


/*
 * Prototype    : hwifi_p2p_listen_complete
 * Description  :
 * Input        :struct cfg_struct *cfg
                :struct net_device *ndev
 * Output       : None
 * Return Value : SUCC:success,-EFAIL:failed.
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/15
 *     Author       : liuyong lwx144625
 *     Modification : Created function
 *
 */
void hwifi_p2p_listen_complete(unsigned long handle)
{
    struct cfg_struct *cfg;
    int32              ret;

    cfg = (struct cfg_struct *)handle;
    if (timer_pending(&cfg->p2p_info.listen_timer))
    {
        spin_lock_bh(&cfg->p2p_info.timer_lock);
        del_timer_sync(&cfg->p2p_info.listen_timer);
        spin_unlock_bh(&cfg->p2p_info.timer_lock);
    }

    /* 通知device监听状态结束 */
    ret = hwifi_cancel_p2p_listen(cfg);
    if (SUCC != ret)
    {
        /* 虽然下发到device取消监听失败，但依旧要返回到上层，定时器只会调用一次 */
        HWIFI_WARNING("hwifi_cancel_p2p_listen failed");
    }
    /* 监听结束，监听状态设置为false */
    cfg->p2p_info.find_listen_state = FALSE;
    cfg80211_remain_on_channel_expired(cfg->wdev, cfg->p2p_info.last_roc_id, &cfg->p2p_info.remain_on_chan,
         GFP_ATOMIC);
}
#ifdef WLAN_PERFORM_DEBUG
static void dump_perform_cfg_info(void)
{
    printk("\n perform dump info\n");
    HWIFI_PERFORM("sdio_bypass:%d", g_perform_cfg_info->sdio_bypass);
    HWIFI_PERFORM("network_bypass:%d", g_perform_cfg_info->network_bypass);
    HWIFI_PERFORM("sdio_statistic:%d", g_perform_cfg_info->sdio_statistic);
    HWIFI_PERFORM("network_statistic:%d", g_perform_cfg_info->network_statistic);
    HWIFI_PERFORM("mpdu_pkt_num:%u", g_perform_cfg_info->mpdu_pkt_num);
    HWIFI_PERFORM("trigger_mode:%d", g_perform_cfg_info->trigger_mode);
    HWIFI_PERFORM("trigger_direction:%d", g_perform_cfg_info->trigger_direction);
    HWIFI_PERFORM("tcp ack tx num: %d", g_perform_cfg_info->tx_ack_num);
}

static void dump_perform_statistic_info(void)
{
    printk("\n perform dump statistic\n");
    HWIFI_PERFORM("rx_network_perform_pktcnt:%d", atomic_read(&(g_perform_statistic->rx_network_perform_pktcnt)));
    HWIFI_PERFORM("rx_sdio_sum_buffer_len:%lu", g_perform_statistic->rx_sdio_sum_buffer_len);
    HWIFI_PERFORM("rx_sdio_usecs:%lu", g_perform_statistic->rx_sdio_usecs);
    HWIFI_PERFORM("rx_net_sum_mpdu_len:%lu", g_perform_statistic->rx_net_sum_mpdu_len);
    HWIFI_PERFORM("rx_net_usecs:%lu", g_perform_statistic->rx_net_usecs);

    HWIFI_PERFORM("tx_network_perform_pktcnt:%d", atomic_read(&(g_perform_statistic->tx_network_perform_pktcnt)));
    HWIFI_PERFORM("tx_sdio_sum_buffer_len:%lu", g_perform_statistic->tx_sdio_sum_buffer_len);
    HWIFI_PERFORM("tx_sdio_usecs:%lu", g_perform_statistic->tx_sdio_usecs);
    HWIFI_PERFORM("tx_net_sum_mpdu_len:%lu", g_perform_statistic->tx_net_sum_mpdu_len);
    HWIFI_PERFORM("tx_net_usecs:%lu", g_perform_statistic->tx_net_usecs);
}

static void dump_perform_tcp_info(struct hi110x_device* hi110x_dev)
{
    HWIFI_BUG_ON(!hi110x_dev);
    HWIFI_BUG_ON(!hi110x_dev->hcc);
    printk("\n perform tcpinfo dump info\n");
    HWIFI_PERFORM("tcp ack tx num: %d",    hi110x_dev->hcc->hcc_queues[HCC_TX].ack_limit);
    HWIFI_PERFORM("tcp ack rx num: %d",    hi110x_dev->hcc->hcc_queues[HCC_RX].ack_limit);
    HWIFI_PERFORM("tcp ack tx enable: %d", hi110x_dev->hcc->sys_tcp_tx_ack_opt_enable);
    HWIFI_PERFORM("tcp ack rx enable: %d", hi110x_dev->hcc->sys_tcp_rx_ack_opt_enable);

}

/*****************************************************************************
 函 数 名  : wlan_set_and_enable_perform_debugging
 功能描述  : 设置并自动使能性能维测
 输入参数  : uint8 *buf
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月23日
    作    者   : 张炜 64406
    修改内容   : 新生成函数

*****************************************************************************/
void wlan_set_and_enable_perform_debugging(struct hi110x_device* hi110x_dev,uint8 *buf)
{
    struct wlan_perform_cfg_interface *perform_info;

    HWIFI_ASSERT(NULL != buf);
    perform_info = (struct wlan_perform_cfg_interface *)buf;

    dump_perform_cfg_info();
    dump_perform_tcp_info(hi110x_dev);
    memcpy(g_perform_cfg_info, perform_info, sizeof(struct wlan_perform_cfg_interface));

    /* Start of zhangwei 64406 2013-02-27 B295 增量上行打印 */
    g_perform_cfg_info->trigger_direction = g_perform_cfg_info->mpdu_pkt_num % 1000;
    g_perform_cfg_info->mpdu_pkt_num -= g_perform_cfg_info->trigger_direction;
    /* End of zhangwei 64406 2013-02-27 B295 增量上行打印 */
#ifdef WLAN_PERFORM_OPT
    hi110x_dev->hcc->hcc_queues[HCC_TX].filter_info.ack_limit = g_perform_cfg_info->tx_ack_num;
    hi110x_dev->hcc->hcc_queues[HCC_RX].filter_info.ack_limit = g_perform_cfg_info->tx_ack_num;

    if (g_perform_cfg_info->tx_ack_num != 0)
    {
        hi110x_dev->hcc->sys_tcp_tx_ack_opt_enable = 1;
        g_perform_cfg_info->trigger_mode  = 0;
    }
    else
    {
        hi110x_dev->hcc->sys_tcp_tx_ack_opt_enable = 0;/*tx_ack_num为0时，关闭优化*/
    }

    if (perform_info->drop_print)
    {
        g_perform_cfg_info->trigger_mode  = 0;
    }
#endif
    dump_perform_tcp_info();
    dump_perform_cfg_info();
    dump_perform_statistic_info();
}
/*****************************************************************************
 函 数 名  : wlan_disable_perform_debugging
 功能描述  : 禁用性能调测
 输入参数  : uint8 *buf
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月23日
    作    者   : 张炜 64406
    修改内容   : 新生成函数

*****************************************************************************/
void wlan_disable_perform_debugging(uint8 *buf)
{
    struct wlan_perform_cfg_interface *perform_info;

    HWIFI_ASSERT(NULL != buf);

    perform_info = (struct wlan_perform_cfg_interface *)buf;

    memset(g_perform_cfg_info, 0, sizeof(struct wlan_perform_cfg_impl));
    memset(g_perform_tcp_info, 0, sizeof(struct wlan_perform_tcp_impl));

    dump_perform_statistic_info();
    //dump_perform_cfg_info();
    //dump_perform_tcp_info();

    hwifi_perform_init_rx_statistic();
    hwifi_perform_init_tx_statistic();

}
#endif

/*
 * Prototype    : hwifi_get_active_status
 * Description  : get wifi work status
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/10/11
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_get_active_status()
{
    int32 ret;

    if (0 != g_cur_cpufreq)
    {
        return TRUE;
    }

    if (g_perform_statistic->rwtotal_sdio_sum_buffer_len > g_coex_level)
    {
        ret = TRUE;
    }
    else
    {
        ret = FALSE;
    }

    return ret;
}

/*
 * Prototype    : hwifi_p2p_discover_listen
 * Description  :
 * Input        :struct cfg_struct *cfg
                :int32 channel
                :uint32 duration_ms

 * Output       : None
 * Return Value : SUCC:success,-EFAIL:failed.
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/15
 *     Author       : liuyong lwx144625
 *     Modification : Created function
 *
 */
int32 hwifi_p2p_discover_listen(struct cfg_struct *cfg, int32 channel, uint32 duration_ms)
{
#define INIT_TIMER(timer, func, duration, extra_delay)    \
    do {                   \
        init_timer(timer); \
        timer->function = func; \
        timer->expires = jiffies + msecs_to_jiffies(duration + extra_delay); \
        timer->data = (unsigned long) cfg; \
        add_timer(timer); \
    } while (0);

    int ret;
    struct timer_list *_timer;

    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return -EFAIL;
    }

    if (timer_pending(&(cfg->p2p_info.listen_timer)))
    {
        HWIFI_DEBUG("Previous LISTEN is not completed yet\n");
        return SUCC;
    }

    ret = hwifi_send_p2p_listen(cfg, channel);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to set p2p listen channel(%d)", channel);
        return ret;
    }

    _timer = &cfg->p2p_info.listen_timer;
    /* 100毫秒，额外的超时时候，sdio传输以及device侧信道切换时间 */
    INIT_TIMER(_timer, hwifi_p2p_listen_complete, duration_ms, 100);

    HWIFI_INFO("Succeed to set listen channel(%d),duration_ms:%u.\n",channel, (duration_ms + 100));

#undef INIT_TIMER

    return ret;

}

/*
 * Prototype    : hwifi_cfg80211_remain_on_channel
 * Description  : device remain on channel for duration
 * Input        : struct wiphy *wiphy
                : struct net_device *ndev
                : struct ieee80211_channel *chan
                : enum nl80211_channel_type channel_type
                : uint32 duration
 * Output       : u64 *cookie
 * Return Value : SUCC:success,-EFAIL:failed.
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/15
 *     Author       : liuyong lwx144625
 *     Modification : Created function
 *
 */
int32 hwifi_cfg80211_remain_on_channel(struct wiphy             *wiphy,
                                       struct wireless_dev     *wdev,
                                       struct ieee80211_channel *chan,
                                       uint32                    duration,
                                       uint64                   *cookie)
{
    struct cfg_struct  *cfg;         /* wireless config struct */
    int32               target_channel;
    int32               ret;

    if ((NULL == wiphy) || (NULL == wdev) || (NULL == chan))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    cfg                               = wiphy_priv(wiphy);
    target_channel                    = ieee80211_frequency_to_channel(chan->center_freq);
    cfg->p2p_info.remain_on_chan      = *chan;

    if(!IS_P2P_ON(cfg))
    {
        HWIFI_WARNING("P2P_LISTEN: P2P state machine is not on");
        return -EFAIL;
    }

    /* 此参数为TRUE说明是FIND阶段下发的监听状态，如果当前是GO或CLIENT模式应阻止下发到device侧
     * 否则是发送管理报文切换到指定的监听信道，需要下发到device侧
     */
    if (TRUE == cfg->p2p_info.find_listen_state)
    {
        if((HI_MODE_AP == cfg->hi110x_dev->mode.current_mode)
          ||(HWIFI_CONNECTED == cfg->conn.status))
        {
            HWIFI_WARNING("P2P_LISTEN: GO/CLIENT mode can not enter listen state,current_mode=%d,status=%d",
                            (int32)cfg->hi110x_dev->mode.current_mode, (int32)cfg->conn.status);
            cfg->p2p_info.find_listen_state = FALSE;
            return -EFAIL;
        }
    }

    *cookie = ++cfg->p2p_info.last_roc_id;
    if (0 == *cookie)
    {
        *cookie = ++cfg->p2p_info.last_roc_id;
    }

    ret = hwifi_p2p_discover_listen(cfg, target_channel, duration);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to set listen channel!");
        return -EFAIL;
    }

    cfg80211_ready_on_channel(wdev, *cookie, chan, duration, GFP_KERNEL);

    return SUCC;

}

/*
 * Prototype    : hwifi_cfg80211_cancel_remain_on_channel
 * Description  : cancel remain on channel
 * Input        : struct wiphy *wiphy
                : struct net_device *ndev
                : uint64 cookie
 * Output       :
 * Return Value : SUCC:success,-EFAIL:failed.
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/15
 *     Author       : liuyong lwx144625
 *     Modification : Created function
 *
 */
int32 hwifi_cfg80211_cancel_remain_on_channel(struct wiphy      *wiphy,
                                              struct wireless_dev     *wdev,
                                              uint64             cookie)
{
    struct cfg_struct *cfg;

    if ((NULL == wiphy) || (NULL == wdev))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    cfg = wiphy_priv(wiphy);

    /* cookie值，listen channel设置标志，在函数hwifi_cfg80211_remain_on_channel设置 */
    if (cfg->p2p_info.last_roc_id != cookie)
    {
        HWIFI_WARNING("It is not set listen channel before");
        return -EFAIL;
    }

    if (timer_pending(&cfg->p2p_info.listen_timer))
    {
        hwifi_p2p_listen_complete((unsigned long)cfg);
    }

    return SUCC;
}

/* receive and send management frame filter type */
static const struct ieee80211_txrx_stypes hwifi_mgmt_stypes[NUM_NL80211_IFTYPES] =
{
    [NL80211_IFTYPE_STATION] =
    {
        .tx = BIT(IEEE80211_STYPE_ACTION >> 4)
            | BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
        .rx = BIT(IEEE80211_STYPE_ACTION >> 4)
            | BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
    },
    [NL80211_IFTYPE_P2P_CLIENT] =
    {
        .tx = BIT(IEEE80211_STYPE_ACTION >> 4)
            | BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
        .rx = BIT(IEEE80211_STYPE_ACTION >> 4)
            | BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
    },
    [NL80211_IFTYPE_P2P_GO] =
    {
        .tx = 0xffff,
        .rx = 0xffff
    },
    [NL80211_IFTYPE_AP] =
    {
        .tx = 0xffff,
        .rx = 0xffff
    },
};



STATIC int32 hwifi_cfg80211_set_monitor_channel(struct wiphy *wiphy,
                                        struct cfg80211_chan_def *chandef)
{
    struct cfg_struct  *cfg;
    uint16              channel_info = 0;
    uint8               enable_20M_40M;
    int32               ret;

    if ((NULL == wiphy) || (NULL == chandef))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    cfg = (struct cfg_struct *)wiphy_priv(wiphy);
    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return -EFAIL;
    }

    /* 默认device 为20M，不支持20M，40M共存 */
    enable_20M_40M = 0;

    /*
    channel_info:
    BIT15：0->2.4G，1->5G
    BIT9-8：0->SCN，1->SCA，3->SCB（20M情况上设置为SCN，40M情况选择SCA或SCB）
    BIT7-0：主信道号
    如果参数低8BITS配置为0，则AP在建立网络过程表示自动信道选择。
    */
    if ((cfg->ap_info.auto_channel) && (!IS_P2P_ON(cfg)))
    {
        /* auto channel 当p2p开启，没有自动信道功能 */
        channel_info = 0;
    }
    else
    {
        cfg->ap_info.curr_channel = (0xFF & chandef->chan->hw_value);
        channel_info = (0xFF & chandef->chan->hw_value);
    }
/*
    if((NL80211_CHAN_HT40MINUS == channel_type) && (cfg->ap_info.ht_enabled))
    {
        enable_20M_40M = 1;
        channel_info |= HWIFI_40M_MINUS;
    }
    else if((NL80211_CHAN_HT40PLUS == channel_type) && (cfg->ap_info.ht_enabled))
    {
        enable_20M_40M = 1;
        channel_info |= HWIFI_40M_PLUS;
    }
*/
    if((NL80211_CHAN_WIDTH_40 == chandef->width) && (cfg->ap_info.ht_enabled))
    {
        enable_20M_40M = 1;
    }

    if (IEEE80211_BAND_5GHZ == chandef->chan->band)
    {
        channel_info |= HWIFI_BAND_5G;
    }
    cfg->ap_info.channel_info = channel_info;

    if (255 != cfg->ap_info.fc_enabled)
    {
        enable_20M_40M = cfg->ap_info.fc_enabled;
    }

    HWIFI_INFO("channel=0x%.2x,enable_20M_40M=%d,coexist_20M_40M=%d,ht_enabled=%d",
                channel_info, enable_20M_40M, cfg->ap_info.coex_2040_enabled, cfg->ap_info.ht_enabled);

    /* send msg */
    ret = wl_send_channel(cfg, channel_info, enable_20M_40M, cfg->ap_info.coex_2040_enabled, cfg->ap_info.ht_enabled);
    if(SUCC != ret)
    {
        HWIFI_WARNING("wl_send_channel failed!");
        return -EFAIL;
    }



    return ret;

}
int32 hwifi_set_auth_alg(struct cfg_struct *cfg, uint8 auth_alg)
{
    cfg->ap_info.dot11_auth_mode = auth_alg;
    HWIFI_INFO("Set dot11_auth_mode = %d", cfg->ap_info.dot11_auth_mode);
    return SUCC;
}


int32 hwifi_set_hw_mode(struct cfg_struct *cfg, uint8 hwmode)
{
    cfg->ap_info.hw_mode = hwmode;
    return SUCC;
}


int32 hwifi_set_ht_enabled(struct cfg_struct *cfg, uint8 ht_enabled)
{
    HWIFI_DEBUG("HT is %s",ht_enabled?"Enabled":"Disabled");
    if (ht_enabled)
    {
        cfg->ap_info.ht_enabled = 1;
    }
    else
    {
        cfg->ap_info.ht_enabled = 0;
    }
    return SUCC;
}


int32 hwifi_set_ssid(struct cfg_struct *cfg, int8 *ssid)
{
    if (NULL == ssid)
    {
        return -EFAIL;
    }

    cfg->ap_info.ssid_len = strlen(ssid);
    if (IEEE80211_MAX_SSID_LEN < cfg->ap_info.ssid_len)
    {
        HWIFI_WARNING("SSID len(%d) is too long,only first 32 bytes is used", cfg->ap_info.ssid_len);
        cfg->ap_info.ssid_len = IEEE80211_MAX_SSID_LEN;
    }

    strncpy(cfg->ap_info.ssid, ssid, cfg->ap_info.ssid_len);

    return SUCC;
}


int32 hwifi_set_auto_channel(struct cfg_struct *cfg, int8 auto_channel)
{
    cfg->ap_info.auto_channel = auto_channel;
    return SUCC;
}


int8 hwifi_get_auto_channel(struct cfg_struct *cfg)
{
    return cfg->ap_info.auto_channel;
}

STATIC int32 hwifi_cfg80211_set_wiphy_params(struct wiphy *wiphy, u32 changed)
{
    int ret;
    struct cfg_struct *cfg;
    struct hwifi_wiphy_param param;

    if (NULL == wiphy)
    {
        HWIFI_WARNING("Invalid NULL wiphy!");
        return -EFAIL;
    }

    cfg = wiphy_priv(wiphy);
    memset(&param, 0, sizeof(struct hwifi_wiphy_param));

    if (changed & WIPHY_PARAM_FRAG_THRESHOLD)
    {
#define MAX_FRAG_THRESHOLD 7536
#define MIN_FRAG_THRESHOLD 256
        HWIFI_INFO("wiphy->frag_threshold = %08X", wiphy->frag_threshold);
        param.frag_threshold = wiphy->frag_threshold;
        param.frag_threshold_changed = TRUE;

        if (param.frag_threshold > MAX_FRAG_THRESHOLD)
        {
            param.frag_threshold = MAX_FRAG_THRESHOLD;
        }
        if (param.frag_threshold < MIN_FRAG_THRESHOLD)
        {
            param.frag_threshold = MIN_FRAG_THRESHOLD;
        }
    }

    if (changed & WIPHY_PARAM_RTS_THRESHOLD)
    {
#define MIN_RTS_THRESHOLD 256
#define MAX_RTS_THRESHOLD 0xFFFF
        HWIFI_INFO("wiphy->rts_threshold = %08X", wiphy->rts_threshold);
        param.rts_threshold = wiphy->rts_threshold;
        param.rts_threshold_changed = TRUE;

        if (param.rts_threshold < MIN_RTS_THRESHOLD)
        {
            param.rts_threshold = MIN_RTS_THRESHOLD;
        }

        if (param.rts_threshold > MAX_RTS_THRESHOLD)
        {
            param.rts_threshold = MAX_RTS_THRESHOLD;
        }
    }

    if ((FALSE == param.frag_threshold_changed)
    && (FALSE == param.rts_threshold_changed))
    {
        HWIFI_INFO("Wiphy params have no change, return directly");
        return SUCC;
    }

    HWIFI_INFO("Start set wiphy params");
    ret = hwifi_start_set_wiphy(cfg, &param);

    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to set wiphy params!");
        return -EFAIL;
    }

    return ret;
}

STATIC struct cfg80211_ops  g_hwifi_cfg80211_ops = {
    .change_virtual_intf      = hwifi_cfg80211_change_mode,
    .scan                     = hwifi_cfg80211_scan,
    .connect                  = hwifi_cfg80211_connect,
    .disconnect               = hwifi_cfg80211_disconnect,
    .get_station              = hwifi_cfg80211_get_station,
    .start_ap                    = hwifi_cfg80211_start_ap,
    .stop_ap                    = hwifi_cfg80211_stop_ap,
    .change_bss               = hwifi_cfg80211_change_bss,
    .set_wiphy_params         = hwifi_cfg80211_set_wiphy_params,
    .set_pmksa                = hwifi_cfg80211_set_pmksa,
    .add_key                  = hwifi_cfg80211_add_key,
    .get_key                  = hwifi_cfg80211_get_key,
    .del_key                  = hwifi_cfg80211_remove_key,
    .set_default_key          = hwifi_cfg80211_set_default_key,
    .mgmt_tx                  = hwifi_cfg80211_mgmt_tx,
    .set_monitor_channel      = hwifi_cfg80211_set_monitor_channel,
    .remain_on_channel        = hwifi_cfg80211_remain_on_channel,
    .set_power_mgmt           = hwifi_cfg80211_set_power_mgmt,
    .cancel_remain_on_channel = hwifi_cfg80211_cancel_remain_on_channel,
};

/*
 * Prototype    : hwifi_init_cfg
 * Description  : alloc and init wireless configuration private struct
 * Input        : struct hwifi_private *priv
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/21
 *     Author       : kf74588
 *     Modification : Created function
 *
 */

/* TBD: return val */
void hwifi_init_cfg(struct cfg_struct *cfg)
{
    HWIFI_ASSERT(NULL != cfg);

    /* init structure members */
    spin_lock_init(&cfg->lock);

    init_waitqueue_head(&cfg->wait_queue);

    cfg->scan_wq = create_singlethread_workqueue("hi110x_service");
    INIT_DELAYED_WORK(&cfg->pwrm_set_work, wl_set_pwrm_late);

    scan_init(&cfg->scan);

    cfg->conn.status = HWIFI_DISCONNECTED;
    cfg->conn.auto_authtype = FALSE;
    cfg->conn.reconn_msg    = NULL;
    memset(&cfg->conn.bss, 0, sizeof(cfg->conn.bss));
    memset(cfg->key_mgmt.mul_addr, 0, 6);
    memset(cfg->key_mgmt.uni_addr, 0, 6);
    memset(cfg->key_mgmt.ptk, 0, sizeof(cfg->key_mgmt.ptk));
    memset(cfg->key_mgmt.gtk, 0, sizeof(cfg->key_mgmt.gtk));

    cfg->hi110x_dev->mode.current_mode = HI_MODE_STA;
    cfg->hi110x_dev->mode.next_mode    = HI_MODE_STA;

    HWIFI_BUG_ON(!cfg->hi110x_dev);
    hcc_trans_limit_default_config(cfg->hi110x_dev->hcc, cfg->hi110x_dev->mode.current_mode);

    init_completion(&cfg->hi110x_dev->mode.complete);

    init_completion(&cfg->init_done);

    P2P_ON(cfg)                    = FALSE;
#if 0
    cfg->p2p_info.index            = 0;
#endif
    cfg->p2p_info.last_roc_id      = 0;
    cfg->p2p_info.ct_info.ctwindow = 0;
    cfg->p2p_info.ct_info.opps     = 0;

    cfg->tkip_measure_flags      = FALSE;

    /*init device*/
    cfg->init_flag               = FALSE;
    cfg->beacon_set              = FALSE;

    cfg->latest_signal = 0;
    cfg->sta_info.manual_pwrm    = 0xFF;
    cfg->sta_info.lint           = 3;
    cfg->sta_info.hwar           = 1;
    cfg->sta_info.gmode          = WID_G_MIXED_11B_2_MODE;   // old = 1 modify by luolingzhi
    cfg->sta_info.rifs_prot_enabled = 1;
    cfg->sta_info.qos_enabled           = WID_QOS;
    cfg->sta_info.ht_capability_enabled = WID_OPEN_HT;
    cfg->ap_info.coex_2040_enabled = g_ap_coex_2040m_enabled;
    cfg->hi110x_dev->tps->oltpc_active  = TRUE;
    cfg->hi110x_dev->tps->oltpc_switch  = TRUE;
    init_timer(&(cfg->p2p_info.listen_timer));
    cfg->p2p_info.listen_timer.function = hwifi_p2p_listen_complete;
    cfg->p2p_info.listen_timer.data = (unsigned long) cfg;

    spin_lock_init(&cfg->p2p_info.timer_lock);
    station_mgmt_init(&cfg->ap_info.sta_mgmt);
#ifdef MAC_802_11W
    hwifi_pmf_init(cfg);
#endif

#ifdef __ROAM__
    /* init roam struct */
    hwifi_roam_main_init(&cfg->roam);
#endif

    cfg->sta_info.fc_enabled = g_sta_2040m_enabled;
    cfg->ap_info.fc_enabled = g_ap_2040m_enabled;

    cfg_event_rx_init(cfg);

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
    hwifi_aggr_init(cfg);
#endif

    mutex_init(&cfg->dev_err_lock);

#ifdef INI_CONFIG
    //hisi_ini_config_wifi(cfg);
#endif
}

/*
 * Prototype    : hwifi_exit_cfg
 * Description  : free wireless configuration private struct
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/21
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
void hwifi_exit_cfg(struct cfg_struct *cfg)
{
    HWIFI_ASSERT(NULL != cfg);

    cfg_event_rx_exit(cfg);

#ifdef __ROAM__
    hwifi_roam_main_exit(&cfg->roam);
#endif

    /* if have current scan request, abort scanning */
    hwifi_scan_done(cfg);

    /* if current connected, disconnect  */
    stop_conn(&cfg->conn);

    if (NULL != cfg->conn.reconn_msg)
    {
        dev_kfree_skb_any(cfg->conn.reconn_msg);
        cfg->conn.reconn_msg = NULL;
    }

    cancel_delayed_work_sync(&cfg->pwrm_set_work);

    destroy_workqueue(cfg->scan_wq);
#ifdef MAC_802_11W
    hwifi_pmf_exit(cfg);
#endif

   mutex_destroy(&cfg->dev_err_lock);
}

/*
 * Prototype    : hwifi_get_wiphy
 * Description  : get wiphy form cfg struct
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : struct wiphy*
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/29
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
struct wiphy *wl_get_wiphy(struct cfg_struct *cfg)
{
    HWIFI_ASSERT(NULL != cfg);
    return cfg->wiphy;
}

/*
 * Prototype    : hwifi_set_ndev
 * Description  : set ndev of cfg_struct
 * Input        : None
 * Output       : None
 * Return Value : voi
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/29
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void wl_set_ndev(struct cfg_struct *cfg, struct net_device *ndev)
{
    HWIFI_ASSERT((NULL != cfg) && (NULL != ndev));
    cfg->ndev = ndev;
}
/*
 * Prototype    : hwifi_set_wdev
 * Description  : set wdev of cfg_struct
 * Input        : None
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/3/17
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void wl_set_wdev(struct cfg_struct *cfg, struct wireless_dev *wdev)
{
    HWIFI_ASSERT((NULL != cfg) && (NULL != wdev));
    cfg->wdev = wdev;
}

/*
 * Prototype    : hwifi_get_sta_req_ie
 * Description  :
 * Input        : sta mac address
 * Output       : ie length, ie
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/16
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
void *hwifi_get_sta_req_ie(struct cfg_struct *cfg, uint8* mac)
{
    struct conn_sta_info*  sta;

    if (!IS_AP(cfg))
    {
        HWIFI_WARNING("Only AP mode can get sta assoc request ie");
        return NULL;
    }

    sta = find_by_mac(&cfg->ap_info.sta_mgmt, mac);

    if (NULL == sta)
    {
        HWIFI_WARNING("Can not found sta by mac:" MACFMT, MAC2STR(mac));
        return NULL;
    }

    return &sta->assoc_req;

}

/*
 * Prototype    : wl_set_noa
 * Description  : start or stop p2p noa
                  start from wpa_supplicant means interval
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/4
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 wl_set_p2p_noa(struct cfg_struct *cfg, uint8 count, int32 interval, int32 duration)
{
    int32 ret;
    int32 starttime = 100;

    struct hwifi_p2p_noa_param noa_param;
    HWIFI_ASSERT(NULL != cfg);
    if(!IS_P2P_ON(cfg))
    {
        HWIFI_WARNING("SET_NOA: P2P state machine is not on\n");
        return -EFAIL;
    }

    /* TBD : set start time */

    noa_param.count     = count;
    noa_param.interval  = interval * 1000;
    noa_param.duration  = duration * 1000;
    noa_param.starttime = starttime;

    ret = hwifi_cmd_set_noa(cfg, &noa_param);

    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to set noa information");
        return ret;
    }
    cfg->p2p_info.noa_info.cnt         = count;
    cfg->p2p_info.noa_info.interval    = interval;
    cfg->p2p_info.noa_info.dur         = duration;
    cfg->p2p_info.noa_info.start       = starttime;
    return ret;
}

/*
 * Prototype    : wl_get_p2p_noa
 * Description  : get p2p noa or ctwindow info
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/11
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32  wl_get_p2p_noa(struct cfg_struct *cfg, uint8 *buf, uint8 buf_len)
{
    uint8 *pos;
    uint8  opps = 0;
    uint8  ctwindow = 0;
    uint8  len;
    struct p2p_noa *noa;
    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    pos = buf;
    len = 0;

    if(!IS_P2P_ON(cfg))
    {
        HWIFI_WARNING("SET_NOA: P2P state machine is not on\n");
        return -EFAIL;
    }

    if ((0 == cfg->p2p_info.ct_info.opps) && (0 == cfg->p2p_info.noa_info.cnt))
    {
        return len;
    }

    /* device will update noa attr, we just to set a default value */
#if 0
   *pos = cfg->p2p_info.index;
    cfg->p2p_info.index++;
#endif

   *pos = 0;
    pos++;
    len++;

    /* ctwindow and opps, 8 bits, first bit : opps vlaue and other bits : ctwindow value */
    if(cfg->p2p_info.ct_info.opps != 0)
    {
        opps = (1 << 7);
    }
    if (cfg->p2p_info.ct_info.ctwindow != 0)
    {
        ctwindow = (~(1 << 7)) & cfg->p2p_info.ct_info.ctwindow;
    }
   *pos = opps | ctwindow;
    pos++;
    len++;

    /* noa */
    if (0 != cfg->p2p_info.noa_info.cnt)
    {
        noa           = (struct p2p_noa *)pos;
        noa->cnt      = cfg->p2p_info.noa_info.cnt;
        noa->dur      = cfg->p2p_info.noa_info.dur * 1000;      /* change from ms to us */
        noa->interval = cfg->p2p_info.noa_info.interval * 1000; /* change from ms to us */
        noa->start    = cfg->p2p_info.noa_info.start;
        len          += (uint8)sizeof(struct p2p_noa);
    }

    if (len > buf_len)
    {
        HWIFI_WARNING("GET_NOA : buff is not enough");
        return -EFAIL;
    }

    return len;
}

/*
 * Prototype    : wl_set_p2p_ps
 * Description  :
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/4
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 wl_set_p2p_ps(struct cfg_struct *cfg, int32 ctwindow, int32 opps, int32 legacy_ps)
{
    int32 ret = -EFAIL;

    /* disabled, legacy ps, max legacy ps */
    //uint8 map_legacy_ps[3] = {DISABLED, LEGACY_POWERSAVE, MAX_LEGACY_PS};

    /* map for ctwindow: disabled, ctwindow value */
    uint8 map_opps[2] = {DISABLED, DISABLED}; /* map for ctwindow: disabled, ctwindow value */

    HWIFI_ASSERT(NULL != cfg);
    if(!IS_P2P_ON(cfg))
    {
        HWIFI_WARNING("SET_NOA: P2P state machine is not on\n");
        return -EFAIL;
    }

    /* legacy_ps 不能超过数组map_legacy_ps长度 */
    if ((opps < PS_PARAM_NO_CHANGE) || (opps > ENABLED)
    || (ctwindow < PS_PARAM_NO_CHANGE) || (ctwindow > MAX_CTWINDOW)
    || (legacy_ps < PS_PARAM_NO_CHANGE) || (legacy_ps > 2))
    {
        HWIFI_WARNING("The params of p2p power save is wrong!"
                      "oppps: %d,ctwindow: %d, legacy_ps: %d", opps, ctwindow, legacy_ps);
        return -1;
    }

    if (legacy_ps != PS_PARAM_NO_CHANGE)
    {
        /*
        ret = wl_pwrm_set(cfg, map_legacy_ps[legacy_ps]); hwifi_cmd_set_ps(cfg, map_legacy_ps[legacy_ps]);
        return ret; */
        /* no legacy ps in ap mode */
        return SUCC;
    }

    if (ctwindow != PS_PARAM_NO_CHANGE)
    {
        cfg->p2p_info.ct_info.ctwindow = (uint8)ctwindow;
        HWIFI_DEBUG("Save ctwindows value, ctwindow: %d", cfg->p2p_info.ct_info.ctwindow);
        return SUCC;
    }

    if ((opps != PS_PARAM_NO_CHANGE) && (ctwindow != 0))
    {
        map_opps[1] = cfg->p2p_info.ct_info.ctwindow;
        ret = hwifi_cmd_set_p2p_ps(cfg, map_opps[opps]);
    }

    if (SUCC == ret)
    {
        cfg->p2p_info.ct_info.opps = (uint8)opps;
    }

    return ret;
}
int32 wl_set_wmm_uapsd(struct cfg_struct *cfg, uint8 *buf)
{
    struct wmm_uapsd_param   *param;
    int32                     ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    if(IS_STA(cfg))
    {
        if ((HWIFI_CONNECTED == cfg->conn.status)
            || (HWIFI_CONNECTING == cfg->conn.status)
            || (TRUE == cfg->scan.is_scanning)
           )
        {
            HWIFI_WARNING("current sta status can not support STA wmm uapsd.\n");
            return -EFAIL;
        }
    }else if(IS_AP(cfg))
    {
        HWIFI_WARNING("Sta wmm uapsd config can not support in AP mode.\n");
        return -EFAIL;
    }

    param = (struct wmm_uapsd_param *)buf;

    HWIFI_DEBUG("\nwmm_uapsd_param->ac_be = %d\n"
                "\nwmm_uapsd_param->ac_bk  = %d\n"
                "\nwmm_uapsd_param->ac_vi  = %d\n"
                "\nwmm_uapsd_param->ac_vo  = %d\n",
                param->ac_be, param->ac_bk, param->ac_vi, param->ac_vo);

    ret = hwifi_set_wmm_uapsd(cfg, param);
    if (SUCC != ret)
    {
            HWIFI_ERROR("Set uapsd error");
            return ret;
    }

    return SUCC;
}

/*
 * Prototype    : hwifi_set_tkip_measure
 * Description  : set tkip measure start or stop flags
 * Input        : None
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
int32 hwifi_set_tkip_measure(struct cfg_struct *cfg, int disabled)
{
    HWIFI_ASSERT(NULL != cfg);
    cfg->tkip_measure_flags = disabled;
    HWIFI_INFO("Tkip measure is %s", disabled ? "enabled" : "disabled");
    return SUCC;
}

/*
 * Prototype    : hwifi_set_init_flag
 * Description  : set device init flags
 * Input        : None
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
int32 hwifi_set_init_flag(struct cfg_struct *cfg, uint8 flags)
{
    HWIFI_ASSERT(NULL != cfg);
    cfg->init_flag = flags;
    HWIFI_INFO("set init flags to %s", flags ? "TRUE" : "FALSE");
    return SUCC;
}

/*dump hi110x_device information*/
STATIC int hwifi_cfg80211_log_dump(hwifi_union_log* log)
{
    struct cfg_struct* cfg = (struct cfg_struct*)log->data;
    if(!cfg)
    {
        HWIFI_UNION_PRINT(log, "cfg ptr is null\n");
        return 0;
    }
    HWIFI_UNION_PRINT(log, "init_flag:%u, is_scanning:%d, connect status:%d\n",
                            cfg->init_flag,
                            cfg->scan.is_scanning,
                            cfg->conn.status);
    HWIFI_UNION_PRINT(log, "p2p_info:p2p is %s\n",P2P_ON(cfg)?"on":"off");
    HWIFI_UNION_PRINT(log, "sta_info:curr_channel %u, ap_info: curr_chan %u\n",
                       cfg->sta_info.curr_channel,
                       cfg->ap_info.curr_channel);
    /*dump aggre info*/
    HWIFI_UNION_PRINT(log, "aggred %s\n", cfg->aggre_info.aggr_start ? "enabled":"disabled");
    return 0;
}

STATIC hwifi_union_log hwifi_cfg80211_log =
{
    .module = "cfg80211",
    .cb = hwifi_cfg80211_log_dump
};

/*
 * Prototype    : hwifi_cfg80211_alloc
 * Description  : alloc wiphy and register
 * Input        : struct device *dev
 * Output       : None
 * Return Value : struct cfg_struct*
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/1
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
struct cfg_struct* hwifi_cfg80211_alloc(struct hi110x_device* hi110x_dev, struct device *dev)
{
    struct cfg_struct* cfg;
    struct wiphy *wiphy;
    int32 ret;
    int32 log_num;

    int8 *country_code_point;

    HWIFI_ASSERT(NULL != dev);

    wiphy = wiphy_new(&g_hwifi_cfg80211_ops, sizeof(struct cfg_struct));
    if (NULL == wiphy)
    {
        HWIFI_WARNING("Failed to alloc new wiphy!");
         return NULL;
    }

    set_wiphy_dev(wiphy, dev);
    cfg = wiphy_priv(wiphy);
    memset(cfg, 0, sizeof(struct cfg_struct));
    cfg->wiphy = wiphy;

    /* set params supported by wireless physical device */
    wiphy->interface_modes            = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP)
                                      | BIT(NL80211_IFTYPE_P2P_CLIENT) | BIT(NL80211_IFTYPE_P2P_GO);  /* working in STA mode */
    wiphy->max_scan_ssids             = MAX_PROBED_SSID_NUM;
    wiphy->max_scan_ie_len            = MAX_SCAN_IE_LEN;
    wiphy->bands[IEEE80211_BAND_2GHZ] = &hi110x_band_2ghz;        /* band information supported by 2.4G */
    if (TRUE == band_5g_enabled)
    {
        wiphy->bands[IEEE80211_BAND_5GHZ] = &hi110x_band_5ghz;        /* band information supported by 5G */
    }

    wiphy->max_remain_on_channel_duration = 5000;                 /* 5000，5秒时间，最长监听时间参考业界值 */

    wiphy->signal_type                = CFG80211_SIGNAL_TYPE_MBM;

    wiphy->cipher_suites              = hi110x_cipher_suites;
    wiphy->n_cipher_suites            = ARRAY_SIZE(hi110x_cipher_suites);
    wiphy->mgmt_stypes                = hwifi_mgmt_stypes;

    /* disable power save */
    wiphy->flags &= ~WIPHY_FLAG_PS_ON_BY_DEFAULT;

    wiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;
    /* Apply World regulatory domain */
    /* World regulatory domain,Customize,Ignore cfg80211 static domain */
    wiphy->flags |= WIPHY_FLAG_CUSTOM_REGULATORY;
    wiphy_apply_custom_regulatory(wiphy, &g_hwifi_regdom);
    /* init current_country_code */
    country_code_point = hwifi_grec_point();

    strncpy(cfg->current_country_code, country_code_point, COUNTRY_CODE_LEN);
    strncpy(cfg->custom_country_code, country_code_point, COUNTRY_CODE_LEN);

    /* Init country code to invalid value */
    strncpy(cfg->sim_country_code, INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN);
    strncpy(cfg->beacon_ie_country_code, INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN);

    /* Init wifi log module level to invalid value */
    for(log_num =0;log_num < WIFI_DEVICE_LOG_MAX;log_num++)
    {
        cfg->device_log.device_log_flag[log_num]= WIFI_DEVICE_LOG_INVALID;
    }

    cfg->sta_info.currenttxpow = WIFI_CURRT_TX_POW_INVALID;

    /* register wireless physical device */
    ret = wiphy_register(wiphy);
    if (ret < 0)
    {
        wiphy_free(wiphy);
        HWIFI_WARNING("Failed to register wiphy!");

        return NULL;
    }
    /* TBD: return val */

    cfg->hi110x_dev = hi110x_dev;

    hwifi_init_cfg(cfg);
    hwifi_union_log_register(&hwifi_cfg80211_log,(void*)cfg);
    return cfg;
}

/*
 * Prototype    : hwifi_cfg80211_free
 * Description  : unregister and free wiphy
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/1
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
void hwifi_cfg80211_free(struct cfg_struct *cfg)
{
    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return;
    }

    HWIFI_ASSERT(NULL != cfg->wiphy);

    hwifi_exit_cfg(cfg);

    /* unregister wireless physical device */
    wiphy_unregister(cfg->wiphy);
    wiphy_free(cfg->wiphy);
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

