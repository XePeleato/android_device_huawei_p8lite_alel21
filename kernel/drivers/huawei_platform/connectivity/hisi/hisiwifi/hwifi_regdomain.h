

#ifndef __HWIFI_REGDOMAIN_H__
#define __HWIFI_REGDOMAIN_H__

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
#define WID_RADAR_CODE_LEN      1 /* radar standard code len */
#define WID_COUNTRY_CODE_LEN    2 /* country code len */
#define WID_RC_BMAP_LEN         8 /* regulatory domain bmap len  */
#define WID_RC_INFO_LEN         15 /* rc info len */

#define CHANNEL14_FERQ_JAPAN_START  2474 /* 2.4G JAPAN START */
#define CHENNEL14_FREQ_JAPAN_END    2494 /* 2.4G JAPAN END */

#define START_FREQ_2G_RANGE     2400 /* 2.4G FREQ RANGE START*/
#define END_FREQ_2G_RANGE       2494 /* 2.4G FREQ RANGE END */

#define START_FREQ_JP_4G_RANGE  4900 /* JAPAN 4.9G START FREQ */
#define END_FREQ_JP_4G_RANGE    4999 /* JAPAN 4.9G END FREQ */

#define START_FREQ_5G_RANGE     5000 /* 5G FREQ RANGE START*/
#define END_FREQ_5G_RANGE       5870 /* 5G FREQ RANGE END */

#define FREQ_OF_2G              0
#define FREQ_OF_5G              1
#define FREQ_OF_OTHERS          2

#define BANDWIDTH_20MHZ         2   /* value define according to firmware */
#define BANDWIDTH_40MHZ         4
#define BANDWIDTH_OTHERS        5

/* regulatory domain class,US */
#define HWIFI_REGDOM_RC_DFS_CHECK   (1 << 0)

#define RC_MAX_RF_TX_POWER_A    12
#define RC_MAX_RF_TX_POWER_B    12
#define RC_MAX_RF_TX_POWER_N    12
#define RC_MAX_RF_TX_POWER_40M  12

#define RC_INFO_CHECKSUM_LEN    1
#define RC_BMAP_CHECKSUM_LEN    1

#define MIN_REGDOMAIN_RULES     1
#define MAX_REGDOMAIN_RULES     64

/*
 * 3 Enum Type Definition
 */

/*
 * 4 Global Variable Declaring
 */
extern const struct ieee80211_regdomain g_hwifi_regdom;

/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */

/*
 * 7 STRUCT Type Definition
 */


/* Freq, Channel, Index */
struct hwifi_freq_map {
    uint32  freq;
    uint32  chan;
    uint32  chan_idx;
};

/* freq_range, start_freq, end_freq */
struct hwifi_freq_range_map {
    uint8   freq_range;
    uint32  start_freq;
    uint32  end_freq;
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
extern int32 hwifi_regdomain_send_to_firmware(struct wiphy *wiphy, struct regulatory_request *request);
extern int32 hwifi_regdomain_update_by_ie(struct cfg_struct *cfg, uint8 *bss_mgmt, uint32 mgmt_len);
extern int32 hwifi_regdomain_update(struct cfg_struct *cfg);
extern int32 hwifi_get_countrycode_by_priority(struct cfg_struct *cfg,uint8 *country_code);
extern void hwifi_regdomain_fill_country_code(struct hwifi_regdom_country_code_wid *country_code_wid, const uint8 *country_code);
extern void hwifi_regdomain_fill_radar_code(struct hwifi_regdom_country_code_wid *country_code_wid,
                                                       struct hwifi_regdom_radar_code_wid   *radar_code_wid,
                                                       int32                                 db_index);
extern void hwifi_regdomain_fill_rc_bmap(const struct ieee80211_regdomain *regdom, struct hwifi_regdom_rc_bmap_wid *rc_bmap_wid);
extern void hwifi_regdomain_fill_rc_info(const struct ieee80211_regdomain *regdom, struct hwifi_regdom_rc_info_wid *rc_info_wid);
extern const struct ieee80211_regdomain *hwifi_regdomain_find_db(struct regulatory_request *request, int32 *db_index);
extern const struct ieee80211_regdomain * hwifi_find_countrycode_regdom(struct cfg_struct *cfg);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_regdomain.h */
