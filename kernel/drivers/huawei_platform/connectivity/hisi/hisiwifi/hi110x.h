

#ifndef __HI110X_H__
#define __HI110X_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include <net/cfg80211.h>

#include "common.h"
#include "hwifi_bus.h"


/*
 * 2 Macro Definition
 */


/*
 * 3 Enum Type Definition
 */
/* status bit index */
enum hi110x_status{
    HI_STATUS_BUS_READY    = 0,     /* sdio ready , assem transfer ok*/
    HI_STATUS_DEV_READY,            /* device ps ready */
    HI_STATUS_WLAN_READY,           /* network ready */
    HI_STATUS_SCANNING,
    HI_STATUS_SCAN_ABORTING,
    HI_STATUS_CONNECTING,
    HI_STATUS_CONNECTED,

    HI_STATUS_FIRMWARE_LOADING,
};

enum hi110x_network_mode{
    HI_MODE_STA    = 0,
    HI_MODE_AP,
    HI_MODE_P2P_CLIENT,
    HI_MODE_P2P_GO,
    HI_MODE_P2P_DEV,
};
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

struct hcc_handler;
struct cfg_struct;
struct hi110x_pm_info;
struct net_device;
struct device;
struct tps_struct;
struct wireless_dev;

struct mode_struct
{
    enum hi110x_network_mode        current_mode; /* current mode */
    enum hi110x_network_mode        next_mode;    /* mode that change to */
    struct completion               complete;
};

struct hi110x_device
{
    struct hcc_handler             *hcc;
    struct hi110x_pm_info          *pm_info;
    struct cfg_struct              *cfg;
    struct net_device              *netdev;
    unsigned long                   status;
    struct mode_struct              mode;
    struct tps_struct              *tps;
};
typedef enum HI1101_PERFORM_TRIGGER
{
    Hi1101_OFF = 0,
    Hi1101_ON
}HI1101_PERFORM_TRIGGER_ENUM;

typedef enum HI1101_PERFORM_DIRECTION
{
    Hi1101_RX = 0,
    Hi1101_TX,
    Hi1101_ALL
}HI1101_PERFORM_DIRECTION_ENUM;

struct wlan_perform_cfg_interface
{
    uint8   sdio_bypass;
    uint8   network_bypass;
    uint8   sdio_statistic;
    uint8   network_statistic;
    uint32  mpdu_pkt_num;
    uint8   trigger_mode;
    uint8   tx_ack_num;           /* the num of ack that send out */
    uint8   drop_print;
};

struct wlan_perform_statistic_impl
{
    atomic_t                        rx_network_perform_pktcnt;

    unsigned long                   rx_sdio_sum_buffer_len;
    unsigned long                   rx_sdio_usecs;


    unsigned long                   rx_net_sum_mpdu_len;
    unsigned long                   rx_net_usecs;


    atomic_t                        tx_network_perform_pktcnt;

    unsigned long                   tx_sdio_sum_buffer_len;
    unsigned long                   tx_sdio_usecs;

    unsigned long                   tx_sdio_sum_aggr_num;
    unsigned long                   tx_work_sched_num;

    unsigned long                   tx_net_sum_mpdu_len;
    unsigned long                   tx_net_usecs;

    /* rx tx×Ü³¤¶È */
    atomic_t                        rwtotal_perform_pktcnt;
    unsigned long                   rwtotal_sdio_sum_buffer_len;
    unsigned long                   rwtotal_sdio_usecs;

    unsigned long                   total_sdio_rate;
    unsigned long                   total_pps_rate;
};

extern struct wlan_perform_statistic_impl *g_perform_statistic;

struct wlan_perform_cfg_impl
{
    uint8   sdio_bypass;
    uint8   network_bypass;
    uint8   sdio_statistic;
    uint8   network_statistic;

    uint32  mpdu_pkt_num;         /* mpdu pkt num */
    uint32  total_sdio_pkt_num;   /* total sdio pkt num */

    uint8   trigger_mode;
    uint8   tx_ack_num;           /* the num of ack that send out */
    uint8   trigger_direction;

    struct timer_list statis_timer;
};
extern struct wlan_perform_cfg_impl *g_perform_cfg_info;




extern int g_wlan_host_version;

extern struct hi110x_device* g_hi110x_dev;

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
struct hi110x_device* hi110x_alloc(struct device *dev, struct iodevice *io_dev);
void hi110x_release(struct hi110x_device *hi110x_dev);
int32 hi110x_status_ready(struct hi110x_device *hi110x_dev);



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hi110x.h */
