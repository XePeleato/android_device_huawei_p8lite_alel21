

#ifndef __HWIFI_SCAN_H__
#define __HWIFI_SCAN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */

#include "common.h"
#include "hwifi_utils.h"
#include <linux/ieee80211.h>

/*
 * 2 Macro Definition
 */


/*
 * 3 Enum Type Definition
 */


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

struct scanned_bss_info
{
    struct mutex    mutex;
    int ref;

    struct list_head list;
    unsigned long ts;
    int32   index;
    int32   signal;
    uint16  freq;
    uint32  mgmt_len;

    struct ieee80211_mgmt   mgmt[0];
};

struct scanned_bss_mgmt
{
    struct mutex    mutex;
    struct list_head bss_list;
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
struct scan_struct;

extern int scan_init(struct scan_struct *scan);
extern int inform_single_beacon_frame(struct scanned_bss_info* bss, void *data);


extern struct scanned_bss_info* alloc_scanned_bss(int32 rssi, uint16  freq, uint8 *mgmt, int mgmt_len);
extern void put_scanned_bss(struct scanned_bss_info* bss_info);

typedef int (*SCAN_CB_F)(struct scanned_bss_info* bss_info, void *data);
extern int for_each_scanned_bss(struct scanned_bss_mgmt *bss_mgmt, SCAN_CB_F cb, void *data); 
extern int update_bss_info(struct scanned_bss_mgmt *bss_mgmt, struct scanned_bss_info* bss_info);
extern void clean_scanned_bss(struct scanned_bss_mgmt *bss_mgmt);
extern void clean_scanned_bss_immediately(struct scanned_bss_mgmt *bss_mgmt);
extern int scanned_bss_mgmt_init(struct scanned_bss_mgmt *bss_mgmt);

/* must call put_scanned_bss() to release bss_info */
extern struct scanned_bss_info* find_scanned_bss_by_bssid(struct scanned_bss_mgmt *bss_mgmt, uint8* bssid);
extern struct scanned_bss_info* find_scanned_bss_by_ssid(struct scanned_bss_mgmt *bss_mgmt, uint8* ssid, int32 len);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_scan.h */
