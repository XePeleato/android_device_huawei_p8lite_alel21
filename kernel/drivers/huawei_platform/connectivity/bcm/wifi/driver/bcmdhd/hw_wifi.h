#ifndef __HW_WIFI_H__
#define __HW_WIFI_H__

/************修改记录************************
版本:				日期:		解决问题:
HUAWEI-2014.001:  	0529		solve wifi panic
HUAWEI-2014.002:       0609		solve scan_done panic.
HUAWEI-2014.003:       0613             throughput optimize.
HUAWEI-2014.004:       0626             solve wdev_cleanup_work panic.
HUAWEI-2014.005:       0702             solve country code problem.
HUAWEI-2014.006:       0725             use huawei customize country code.
HUAWEI-2014.007:       0728             set bcn_timeout for beacon loss and roaming problem.
**********************************************/
#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>
#include <bcmutils.h>
#include <dngl_stats.h>
#include <dhd.h>
#include <wlioctl.h>
#include <wl_iw.h>
#include <huawei_platform/dsm/dsm_pub.h>

#define        HUAWEI_VERSION_STR ", HUAWEI-2014.007"

#ifdef HW_WIFI_WAKEUP_SRC_PARSE
#define WIFI_WAKESRC_TAG "WIFI wake src: "
extern volatile bool g_wifi_firstwake;
#endif

#define   HW_5G_CUSTOM_ROAM_TRIGGER_SETTING  -70  /* dBm default roam trigger 5 band , used by dhd_preinit_ioctls func*/

extern void get_customized_country_code_for_hw(char *country_iso_code, wl_country_t *cspec);
extern uint hw_get_bcn_timeout(void);
extern void hw_register_wifi_dsm_client(void);
extern void hw_wifi_dsm_client_notify(int dsm_id, const char *fmt, ...);
extern int hw_skip_dpc_in_suspend(void);
extern void hw_resched_dpc_ifneed(struct net_device *ndev);
#ifdef HW_PATCH_DISABLE_TCP_TIMESTAMPS
extern void hw_set_connect_status(struct net_device *ndev, int status);
#endif
extern void hw_dhd_check_and_disable_timestamps(void); /* called in ipv4/tcp_input.c */
#ifdef HW_WIFI_WAKEUP_SRC_PARSE
extern void parse_packet(struct sk_buff *skb);
#endif

#endif
