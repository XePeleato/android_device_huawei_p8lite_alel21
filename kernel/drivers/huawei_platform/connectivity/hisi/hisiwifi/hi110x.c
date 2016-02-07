


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include "hi110x.h"
#include "hwifi_utils.h"
#include "hwifi_hcc.h"
#include "hwifi_cfgapi.h"
#include "hwifi_netdev.h"
#include "hi110x_pm.h"
#include "hwifi_tps.h"
#include "hwifi_perform.h"

/*
 * 2 Global Variable Definition
 */
struct hi110x_device* g_hi110x_dev /*= NULL*/;  /* for extern interface */

int g_wlan_host_version = 295;

/*dump hi110x_device information*/
STATIC int hi110x_log_dump(hwifi_union_log* log)
{
    struct hi110x_device* io_dev = (struct hi110x_device*)log->data;
    if(!io_dev)
    {
        HWIFI_UNION_PRINT(log, "io dev ptr is null\n");
        return 0;
    }
    HWIFI_UNION_PRINT(log, "status:%lu\n", io_dev->status);
    HWIFI_UNION_PRINT(log, "current mode:%d, next mode:%d\n", io_dev->mode.current_mode,
    io_dev->mode.next_mode);
    return 0;
}

STATIC hwifi_union_log hi110x_log =
{
    .module = "hi110x",
    .cb = hi110x_log_dump
};

/*
 * 3 Function Definition
 */

struct hi110x_device* hi110x_alloc(struct device *dev, struct iodevice *io_dev)
{
    struct hi110x_device    *hi110x_dev;
    struct hi110x_pm_info   *pm_info;
    struct hcc_handler      *hcc;
    struct net_device       *ndev;
    struct cfg_struct       *cfg;
    struct tps_struct       *tps;
    HWIFI_ASSERT(NULL != dev);
    /*
     * 1. hi110x_device
     * 2. hcc
     * 3. cfg
     * 4. netdevice
     */

    /* hi110x device alloc */
    hi110x_dev = kzalloc(sizeof(*hi110x_dev), GFP_KERNEL);
    if (NULL == hi110x_dev)
    {
        HWIFI_ERROR("Failed to alloc hi110x dev!");
        return NULL;
    }

    /*alloc tps interface struct*/
    tps = kzalloc(sizeof(*tps), GFP_KERNEL);
    if (NULL == tps)
    {
        HWIFI_ERROR("Failed to alloc tps!");
        kfree(hi110x_dev);

        return NULL;
    }
    hi110x_dev->tps = tps;

    /* hcc alloc */
    hcc = hcc_alloc(hi110x_dev, io_dev);
    if (NULL == hcc)
    {
        HWIFI_ERROR("Failed to alloc hcc!");
        kfree(tps);
        kfree(hi110x_dev);

        return NULL;
    }
    hi110x_dev->hcc = hcc;

    /* alloc and regist cfg80211 */
    cfg = hwifi_cfg80211_alloc(hi110x_dev, dev);
    if (NULL == cfg)
    {
        HWIFI_ERROR("Failed to alloc cfg80211!");
        hcc_release(hi110x_dev->hcc);
        kfree(tps);
        kfree(hi110x_dev);

        return NULL;
    }
    hi110x_dev->cfg = cfg;

#ifdef WLAN_POWER_MANAGEMENT
    /* alloc pm interface struct */
    pm_info = hi110x_pm_alloc(hi110x_dev, dev);
    if (NULL == pm_info)
    {
        HWIFI_ERROR("Failed to alloc hi110x pm_info!");
        hwifi_cfg80211_free(cfg);
        hcc_release(hi110x_dev->hcc);
        kfree(tps);
        kfree(hi110x_dev);

        return NULL;
    }
    hi110x_dev->pm_info = pm_info;

    /* hold wifi wakelock */
    hi110x_wake_lock(hi110x_dev->pm_info);
#endif


    /* alloc and register netdev */
    ndev = hwifi_netdev_register(hi110x_dev, dev);
    if (NULL == ndev)
    {
        HWIFI_ERROR("Failed to register netdev!");
        hwifi_cfg80211_free(cfg);
        hcc_release(hcc);
        hi110x_pm_free(pm_info);
        kfree(tps);
        kfree(hi110x_dev);

        return NULL;
    }
    hi110x_dev->netdev   = ndev;
    wl_set_ndev(cfg, ndev);
    wl_set_wdev(cfg, ndev->ieee80211_ptr);

    g_hi110x_dev = hi110x_dev;
    hwifi_union_log_register(&hi110x_log,(void*)hi110x_dev);
    return hi110x_dev;
}
void hi110x_release(struct hi110x_device *hi110x_dev)
{
    HWIFI_ASSERT((NULL != hi110x_dev));

    hwifi_netdev_unregister(hi110x_dev->netdev);
    hi110x_dev->netdev = NULL;

    hwifi_cfg80211_free(hi110x_dev->cfg);
    hi110x_dev->cfg = NULL;

    hcc_release(hi110x_dev->hcc);
    hi110x_dev->hcc = NULL;

    hi110x_pm_free(hi110x_dev->pm_info);
    hi110x_dev->pm_info = NULL;

    kfree(hi110x_dev->tps);
    hi110x_dev->tps=NULL;

    kfree(hi110x_dev);
    g_hi110x_dev = NULL;

}


int32 hi110x_status_ready(struct hi110x_device *hi110x_dev)
{
    if(!bus_get_state(hi110x_dev->hcc->io_dev))
    {
        HWIFI_WARNING("Bus is not ready");
        return FALSE;
    }

    if (!test_bit(HI_STATUS_DEV_READY, &hi110x_dev->status))
    {
        HWIFI_WARNING("Device is not ready");
        return FALSE;
    }

    if (!test_bit(HI_STATUS_WLAN_READY, &hi110x_dev->status))
    {
        HWIFI_WARNING("Network is not ready");
        return FALSE;
    }

    return TRUE;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

