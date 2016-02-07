/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 *
 * File Name     : hwifi_regdomain.c
 * Version       : Initial Draft
 * Author        : Hash
 * Created       : 2012/9/21
 * Last Modified :
 * Description   : build and send cmd to device
 * Function List :
 * History       :
 * 1.Date        : 2012/9/21
 *   Author      : Hash
 *   Modification: Created file
 *
 */


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include <net/cfg80211.h>

#include "common.h"
#include "hwifi_event.h"
#include "cfg80211_stru.h"
#include "hwifi_utils.h"
#include "hwifi_cfg80211.h"
#include "hwifi_regdomain.h"
#include "hwifi_regdb.h"

#ifdef __ROAM__
#include "hwifi_roam_alg.h"
#endif


/*
 * 2 Global Variable Definition
 */
int countrycode_change_by_ie_enabled = TRUE;
/* default world regulatory domain */
const struct ieee80211_regdomain g_hwifi_regdom = {
    .n_reg_rules = 5,
    .alpha2 =  "99",
    .reg_rules = {

    /* IEEE 802.11b/g, channels 1..13 */
    REG_RULE(2412-10, 2472+10, 40, 6, 20, 0),

    /* IEEE 802.11 channel 14 - Only JP enables
     * this and for 802.11b only
     */
    /*
    REG_RULE(2484-10, 2484+10, 20, 6, 20,
    NL80211_RRF_PASSIVE_SCAN |
    NL80211_RRF_NO_IBSS |
    NL80211_RRF_NO_OFDM),
    */

    /* channel 36 - 64 */
    REG_RULE(5150-10, 5350+10, 40, 6, 20, 0),

    /* channel 100 - 165 */
    REG_RULE(5470-10, 5850+10, 40, 6, 20, 0),

    /* IEEE 802.11 channel 184,188,192,196 for JP 4.9G */
    REG_RULE(4920-10, 4980+10, 40, 6, 23, 0),
    }
};

/* 2G Freq Map */
struct hwifi_freq_map FREQ_2G_MAP[14] = {
    { 2412, 1, 0},
    { 2417, 2, 1},
    { 2422, 3, 2},
    { 2427, 4, 3},
    { 2432, 5, 4},
    { 2437, 6, 5},
    { 2442, 7, 6},
    { 2447, 8, 7},
    { 2452, 9, 8},
    { 2457, 10, 9},
    { 2462, 11, 10},
    { 2467, 12, 11},
    { 2472, 13, 12},
    { 2484, 14, 13},
};
uint32 SIZE_FREQ_2G_MAP = ARRAY_SIZE(FREQ_2G_MAP);

/* 5G Freq Map */
struct hwifi_freq_map FREQ_5G_MAP[28] = {
    { 5180, 36, 0},
    { 5200, 40, 1},
    { 5220, 44, 2},
    { 5240, 48, 3},
    { 5260, 52, 4},
    { 5280, 56, 5},
    { 5300, 60, 6},
    { 5320, 64, 7},
    { 5500, 100, 8},
    { 5520, 104, 9},
    { 5540, 108, 10},
    { 5560, 112, 11},
    { 5580, 116, 12},
    { 5600, 120, 13},
    { 5620, 124, 14},
    { 5640, 128, 15},
    { 5660, 132, 16},
    { 5680, 136, 17},
    { 5700, 140, 18},
    { 5745, 149, 19},
    { 5765, 153, 20},
    { 5785, 157, 21},
    { 5805, 161, 22},
    { 5825, 165, 23},
    /* for JP 4.9G */
    { 4920, 184, 24},
    { 4940, 188, 25},
    { 4960, 192, 26},
    { 4980, 196, 27},
};
uint32 SIZE_FREQ_5G_MAP = ARRAY_SIZE(FREQ_5G_MAP);


struct hwifi_freq_range_map freq_range_map[3] = {
{ FREQ_OF_2G, 2400, 2494},/* 2.4G */
{ FREQ_OF_5G, 5000, 5870},/* 5G */
{ FREQ_OF_5G, 4900, 4999},/* 4.9G, belong to IEEE80211_BAND_5GHZ class */
};

/*
 * 3 Function Definition
 */

 STATIC uint8   hwifi_regdomain_freq_range_get(uint32 start_freq, uint32 end_freq)
{
        uint8   freq_range = 0;
        uint32  i;

        uint32  range_map_size = ARRAY_SIZE(freq_range_map);

        for (i = 0; i < range_map_size; i++)
        {
            if (start_freq >= freq_range_map[i].start_freq && end_freq <= freq_range_map[i].end_freq)
            {
                freq_range = freq_range_map[i].freq_range;
                break;
            }
        }

        if (i == range_map_size)
        {
            return  FREQ_OF_OTHERS;
        }

        return freq_range;
}


STATIC uint32 hwifi_regdomain_chan_get(uint32 start_freq, uint32 end_freq, uint8 freq_range, uint8 bw)
{
    uint32                 i;
    uint32                 j;
    uint32                 chan_bmap = 0;
    uint32                 max_loop;
    uint32                 shift_value = 1;

    max_loop = ARRAY_SIZE(FREQ_5G_MAP);

    /* handle freq which didn support in firmware, fill channel bitmap 0 */
    if (freq_range == FREQ_OF_OTHERS || bw == BANDWIDTH_OTHERS)
    {
        return 0;
    }

    /* handle  regulatory rules for JAPAN 14 Channel */
    if (start_freq == CHANNEL14_FERQ_JAPAN_START && end_freq == CHENNEL14_FREQ_JAPAN_END)
    {
        chan_bmap |= (shift_value << FREQ_2G_MAP[13].chan_idx);/* JAPAN 14 Channel */
        return chan_bmap;
    }

    for (i = start_freq + 10; i <= (end_freq - 10); i++)
    {
        for (j = 0; j < max_loop; j++)
        {
            /* find freq */
            if ((j < ARRAY_SIZE(FREQ_2G_MAP)) && (FREQ_2G_MAP[j].freq == i))
            {
                chan_bmap |= (shift_value << FREQ_2G_MAP[j].chan_idx);
                break;
            }

            if (FREQ_5G_MAP[j].freq == i)
            {
                chan_bmap |= (shift_value << FREQ_5G_MAP[j].chan_idx);
                break;
            }
        }
    }

    return chan_bmap;
}


const struct ieee80211_regdomain    *hwifi_regdomain_find_db(struct regulatory_request *request, int32 *db_index)
 {
    const struct ieee80211_regdomain    *regdom = NULL;
    uint32                               i;

    HWIFI_ASSERT(NULL != request);

    for (i = 0; i < g_regdom_rules_db_size; i++)
    {
        if (!memcmp(request->alpha2, g_regdom_rules_db[i]->alpha2, 2))
        {
            regdom = g_regdom_rules_db[i];
            *db_index = i;
            break;
        }
    }

    if (i == g_regdom_rules_db_size)
    {
        return NULL;
    }

    return  regdom;
}


 STATIC uint8 hwifi_regdomain_get_bw_map(uint8 bw)
 {
    uint8       bw_map;

    switch (bw)
    {
        case 40:
                bw_map = BANDWIDTH_40MHZ;
                break;
        case 20:
                bw_map = BANDWIDTH_20MHZ;
                break;
        default:
                bw_map = BANDWIDTH_OTHERS;
                break;
    };

    return bw_map;
 }


void hwifi_regdomain_fill_country_code(struct hwifi_regdom_country_code_wid *country_code_wid,
                                                          const uint8                          *country_code)
{
    HWIFI_ASSERT((NULL != country_code_wid) && (NULL != country_code));

    country_code_wid->wid = OS_CPU_TO_LE16(WID_COUNTRY_CODE);
    country_code_wid->len = WID_COUNTRY_CODE_LEN;
    country_code_wid->code[0] = country_code[0];
    country_code_wid->code[1] = country_code[1];
}


void hwifi_regdomain_fill_radar_code(struct hwifi_regdom_country_code_wid *country_code_wid,
                                                       struct hwifi_regdom_radar_code_wid   *radar_code_wid,
                                                       int32                                 db_index)
{
    HWIFI_ASSERT((NULL != country_code_wid) && (NULL != radar_code_wid));

    radar_code_wid->wid  = OS_CPU_TO_LE16(WID_DFS_RADAR_STANDARD);
    radar_code_wid->len  = WID_RADAR_CODE_LEN;

    if (country_code_wid->code[0] == '9' && country_code_wid->code[1] == '9')
    {
        /* if current regulatory domain request is customize world regulatory domain
         * set radar stadard as US FCC
         */
        radar_code_wid->code = DFS_RADAR_FCC;
    }
    else
    {
        radar_code_wid->code = g_regdom_dfs_radar_db[db_index];
    }
}


 void hwifi_regdomain_fill_rc_bmap(const struct ieee80211_regdomain *regdom,
                                                    struct hwifi_regdom_rc_bmap_wid *rc_bmap_wid)
 {
    uint8   bmap_val[8] = { 0 };
    uint32  i;

    HWIFI_ASSERT((NULL != regdom) && (NULL != rc_bmap_wid));

    for (i = 1; i <= regdom->n_reg_rules; i++)
    {
        /*
         * the first Bit can't use,index from second Bit;
         * eg:regulatory domain has 3 rules,bmap_val -> 0000,1110
         */
        bmap_val[i / 8] |= (1 << (i % 8));
    }

    rc_bmap_wid->wid = OS_CPU_TO_LE16(WID_BIN_RC_BMAP);
    rc_bmap_wid->len = WID_RC_BMAP_LEN;
    OS_MEM_CPY(rc_bmap_wid->rc_bmap, bmap_val, WID_RC_BMAP_LEN);

    /* fill checksum for rc bmap */
    rc_bmap_wid->rc_bmap_checksum = bytes_check_sum(rc_bmap_wid->rc_bmap, WID_RC_BMAP_LEN);

}
 void hwifi_regdomain_fill_rc_info(const struct ieee80211_regdomain *regdom, struct hwifi_regdom_rc_info_wid *rc_info_wid)
 {
    uint32  start_freq;
    uint32  end_freq;
    uint8   bw;
    uint8   i;/* assume max number of regulatory rules 127 */

    HWIFI_ASSERT((NULL != regdom) && (NULL != rc_info_wid));

    for (i = 0; i < regdom->n_reg_rules; i++)
    {
        /* fill rc info wid */
        rc_info_wid[i].wid = OS_CPU_TO_LE16(WID_BIN_RC_INFO);
        rc_info_wid[i].len = WID_RC_INFO_LEN;
        rc_info_wid[i].regdom_class_num = i + 1;

        HWIFI_DEBUG("regdom_class_num = %d", rc_info_wid[i].regdom_class_num);

        rc_info_wid[i].chan_bmap = 0;

        /* fill freq */
        start_freq = regdom->reg_rules[i].freq_range.start_freq_khz / 1000;
        end_freq = regdom->reg_rules[i].freq_range.end_freq_khz / 1000;
        rc_info_wid[i].freq = hwifi_regdomain_freq_range_get(start_freq, end_freq);

        /* freq max bw */
        bw = regdom->reg_rules[i].freq_range.max_bandwidth_khz / 1000;
        rc_info_wid[i].bw = hwifi_regdomain_get_bw_map(bw);

        /* fill channels */
        rc_info_wid[i].chan_bmap = hwifi_regdomain_chan_get(start_freq, end_freq, rc_info_wid[i].freq, rc_info_wid[i].bw);

        HWIFI_DEBUG("Regdom chan_bmap = %x", rc_info_wid[i].chan_bmap);

        /* fill behavior */
        if (regdom->reg_rules[i].flags & NL80211_RRF_DFS)
        {
            rc_info_wid[i].rc_beahaviour = HWIFI_REGDOM_RC_DFS_CHECK;
        }

        /* fill coverage_class
         * fill regulatory class max tx power
         */
        rc_info_wid[i].coverage_class = 0;
        rc_info_wid[i].rc_max_reg_tx_power = regdom->reg_rules[i].power_rule.max_eirp / 100;

        /* fill regulatory class
         * min of rf tx power
         * max rf tx power 11a
         * max rf tx power 11b
         * max rf tx power 11n
         * max rf tx power 40Mhz
         * check sum
         */
        rc_info_wid[i].rc_max_rf_tx_power_a = RC_MAX_RF_TX_POWER_A;
        rc_info_wid[i].rc_max_rf_tx_power_b = RC_MAX_RF_TX_POWER_B;
        rc_info_wid[i].rc_max_rf_tx_power_n = RC_MAX_RF_TX_POWER_N;
        rc_info_wid[i].rc_max_rf_tx_power_n40 = RC_MAX_RF_TX_POWER_40M;
        rc_info_wid[i].rc_info_checksum = bytes_check_sum(&(rc_info_wid[i].regdom_class_num), WID_RC_INFO_LEN);
    }

 }


int32 hwifi_regdomain_send_to_firmware(struct wiphy *wiphy, struct regulatory_request *request)
{
    struct sk_buff                          *skb;
    const struct ieee80211_regdomain        *regdom;
    struct hwifi_regdom_msg_wid             *regdom_msg;/* pointer to regulatory domain */
    struct cfg_struct                       *cfg;
    uint16                                   msg_len; /* msg len */
    uint32                                   db_index = 0;/* index of regulatory rules db */

    int32                                    ret;

    HWIFI_ASSERT((NULL != wiphy) && (NULL != request));

    HWIFI_INFO("Hwifi cfg80211 regulatory domain cmd for domain  %c%c\n", request->alpha2[0], request->alpha2[1]);

    /* get cfg handle */
    cfg = wiphy_priv(wiphy);

    /* find hwifi regulatory rules data base */
    if (request->alpha2[0] == '9' && request->alpha2[1] == '9')
    {
        /* if request country code 99, use default hwifi regdom */
        regdom = &g_hwifi_regdom;
    }
    else
    {
        /* search regulatory database */
        regdom = hwifi_regdomain_find_db(request, &db_index);
        if (NULL == regdom)
        {
            HWIFI_WARNING("Failed to search regulatory datebase!");

            /* if can not match country code regulatory domain,keep current regulatory domain */
            return -EFAIL;
        }
    }

    if (regdom->n_reg_rules < MIN_REGDOMAIN_RULES  || regdom->n_reg_rules > MAX_REGDOMAIN_RULES)
    {
        HWIFI_WARNING("Invalid n_reg_rules:%d",regdom->n_reg_rules);
        return -EFAIL;
    }

    /* malloc for regdom msg */
    msg_len = sizeof(struct hwifi_regdom_msg_wid)
              + regdom->n_reg_rules * sizeof(struct hwifi_regdom_rc_info_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_len);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    regdom_msg = (struct hwifi_regdom_msg_wid *)skb_put(skb, msg_len);

    /* fill msg header */
    hwifi_fill_msg_hdr(&regdom_msg->msg_hdr, HOST_CMD_CONFIG, msg_len);

    /* fill country code wid */
    hwifi_regdomain_fill_country_code(&regdom_msg->country_code_wid, request->alpha2);

    /* fill radar stadard code */
    hwifi_regdomain_fill_radar_code(&regdom_msg->country_code_wid, &regdom_msg->radar_code_wid, db_index);

    /* fill regulatory class bmap */
    hwifi_regdomain_fill_rc_bmap(regdom, &regdom_msg->rc_bmap_wid);

    /* fill regulatory class general info */
    hwifi_regdomain_fill_rc_info(regdom, regdom_msg->rc_info_wid);

#ifdef __ROAM__
    hwifi_roam_update_channels_regdomain_flag(&cfg->roam, regdom, regdom_msg->rc_info_wid, FREQ_2G_MAP, ARRAY_SIZE(FREQ_2G_MAP), FREQ_5G_MAP, ARRAY_SIZE(FREQ_5G_MAP));
#endif

    /* send regulatory domain info cmd to device, buffer free in hwifi send cmd side */
    ret = hwifi_send_cmd(cfg, skb);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to send regulatory domain info msg");

        return ret;
    }

    HWIFI_INFO("succeed to send regulatory domain info msg");

    cfg->regdom = regdom;

    return ret;
}
STATIC uint8 hwifi_regdomain_isalpha_upper(char letter)
{
    if ( letter >= 65 && letter <= 90)
    {
        return TRUE;
    }

    return FALSE;
}


 STATIC uint8 hwifi_regdomain_isalpha2(const uint8 *country_code)
{
   HWIFI_ASSERT(NULL != country_code);

   if ( hwifi_regdomain_isalpha_upper(country_code[0]) && hwifi_regdomain_isalpha_upper(country_code[1]))
   {
        return TRUE;
   }

   return FALSE;
}


STATIC uint8 hwifi_regdomain_isdefault(const uint8 *country_code)
{
    HWIFI_ASSERT(NULL != country_code);

    if ( country_code[0] == '9' && country_code[1] == '9')
    {
        return TRUE;
    }

    return FALSE;
}

/*
 * Prototype    : hwifi_get_countrycode_by_priority
 * Description  : set countrycode by priority
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : None
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/12/05
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32 hwifi_get_countrycode_by_priority(struct cfg_struct *cfg,uint8 *country_code)
{

    /* The priority of country code :
     * AP  mode : sim_coutry_code > custom_country_code > 99(default country code)
     * STA mode : beacon_ie_country_code > sim_coutry_code > custom_country_code > 99(default country code)
    */
    if(!IS_AP(cfg))
    {
        if(strncmp(cfg->beacon_ie_country_code, INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN) != 0)
        {
            strncpy(country_code, cfg->beacon_ie_country_code, COUNTRY_CODE_LEN);

            HWIFI_INFO("Set country code by beacon ie : %c%c!",country_code[0],country_code[1]);
            return SUCC;
        }
    }

    if(strncmp(cfg->sim_country_code, INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN) != 0)
    {
        strncpy(country_code, cfg->sim_country_code, COUNTRY_CODE_LEN);

        HWIFI_INFO("Set country code by sim card : %c%c!",country_code[0],country_code[1]);
    }
    else if(strncmp(cfg->custom_country_code, INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN) != 0)
    {
        strncpy(country_code, cfg->custom_country_code, COUNTRY_CODE_LEN);

        HWIFI_INFO("Set country code by Hisi custom infomation : %c%c!",country_code[0],country_code[1]);
    }
    else
    {
        strncpy(country_code, DEFAULT_COUNTRY_CODE, COUNTRY_CODE_LEN);

        HWIFI_INFO("Use default country code 99!");
    }

    return SUCC;

}

/*
 * Prototype    : hwifi_find_countrycode_regdom
 * Description  : find regdom of countrycode
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/12/20
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
const struct ieee80211_regdomain * hwifi_find_countrycode_regdom(struct cfg_struct *cfg)
{
    uint32                                  db_index = 0;
    struct regulatory_request               request;
    const struct ieee80211_regdomain       *regdom;

    hwifi_get_countrycode_by_priority(cfg, cfg->current_country_code);

    request.alpha2[0] = cfg->current_country_code[0];
    request.alpha2[1] = cfg->current_country_code[1];

    HWIFI_INFO("country_code = %c%c", cfg->current_country_code[0], cfg->current_country_code[1]);


    if (request.alpha2[0] == '9' && request.alpha2[1] == '9')
    {
        /* if request country code 99, use default hwifi regdom */
        regdom = &g_hwifi_regdom;
    }
    else
    {
        /* search regulatory database */
        regdom = hwifi_regdomain_find_db(&request, &db_index);
        if (NULL == regdom)
        {
            HWIFI_WARNING("Failed to search regulatory datebase!");

            /* if can not match country code regulatory domain,set to default 99 */
            regdom = &g_hwifi_regdom;
            strncpy(cfg->current_country_code, "99", COUNTRY_CODE_LEN);
            strncpy(cfg->custom_country_code, "99", COUNTRY_CODE_LEN);
        }
    }

    if (regdom->n_reg_rules < MIN_REGDOMAIN_RULES  || regdom->n_reg_rules > MAX_REGDOMAIN_RULES)
    {
        HWIFI_WARNING("Invalid n_reg_rules:%d",regdom->n_reg_rules);
    }

    cfg->regdom = regdom;

    return regdom;

}
int32 hwifi_regdomain_update(struct cfg_struct *cfg)
{
    struct regulatory_request       request;
    uint8                          country_code[10];
    int32                           ret;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != cfg->wiphy);


    ret = hwifi_get_countrycode_by_priority(cfg,country_code);
    if(-EFAIL == ret)
    {
        HWIFI_WARNING("Failed to set country code by priority!");
        return -EFAIL;
    }

    if (!hwifi_regdomain_isalpha2(country_code) && !hwifi_regdomain_isdefault(country_code))
    {
        HWIFI_WARNING("Country code not alpha code or default:%c%c", country_code[0], country_code[1]);
        return -EFAIL;
    }

    if (strncmp(cfg->current_country_code, country_code, COUNTRY_CODE_LEN) != 0)
    {

        request.alpha2[0] = country_code[0];
        request.alpha2[1] = country_code[1];

        ret = hwifi_regdomain_send_to_firmware(cfg->wiphy, &request);
        if (ret != SUCC)
        {
            HWIFI_WARNING("Failed to send regdomian(%c%c) to firmware!", request.alpha2[0], request.alpha2[1]);
            return -EFAIL;
        }

        HWIFI_INFO("hwifi regdom(%c%c) update successfully.", request.alpha2[0], request.alpha2[1]);

        /* update current country code */
        strncpy(cfg->current_country_code, country_code, COUNTRY_CODE_LEN);
    }
    else
    {
        HWIFI_INFO("No need hwifi regdom update.");
    }

    return SUCC;
}
int32 hwifi_regdomain_update_by_ie(struct cfg_struct *cfg, uint8 *bss_mgmt, uint32 mgmt_len)
{
    struct ieee80211_mgmt       *mgmt = NULL;
    const uint8                 *country_ie;
    const uint8                 *country_code;
    uint8                       *ssid_ie;
    uint32                       ssid_ie_len;
    int32                        ret;

    HWIFI_ASSERT(NULL != cfg);

    /* if g_countrycode_ie_flag is on,country code will always be updated by ie.*/



    if(!countrycode_change_by_ie_enabled)
    {
        HWIFI_DEBUG("Can not update country code by ie when g_countrycode_ie_flag is off");
        return SUCC;
    }

    mgmt = (struct ieee80211_mgmt *)bss_mgmt;
    if (NULL == mgmt)
    {
        HWIFI_WARNING("Mgmt frame is NULL.");

        /* if mgmt frame is NULL,keep current regulatory domain */
        return SUCC;
    }

    ssid_ie = mgmt->u.beacon.variable;
    ssid_ie_len = mgmt_len - offsetof(struct ieee80211_mgmt, u.beacon.variable);

    country_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_COUNTRY, ssid_ie, ssid_ie_len);
    if (NULL == country_ie)
    {
        HWIFI_WARNING("Cannot find country in mgmt.");
        strncpy(cfg->beacon_ie_country_code, INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN);
        
        /* if country_ie empty,keep current regulatory domain */
        return SUCC;
    }

    /* country_ie + 2, the start of country_ie data */
    country_code = country_ie + COUNTRY_CODE_LEN;
    strncpy(cfg->beacon_ie_country_code, country_code, COUNTRY_CODE_LEN);

    /* check current country code */
    ret = hwifi_regdomain_update(cfg);
    if (ret != SUCC)
    {
        HWIFI_WARNING("Faied to update regdomain!");
        return -EFAIL;
    }

    HWIFI_INFO("Hwifi regdom update by country ie[%c%c] successfully.",country_code[0],country_code[1]);

    return SUCC;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
