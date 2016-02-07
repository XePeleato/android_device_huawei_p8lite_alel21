


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __ROAM__
/*
 * 1 Header File Including
 */
#include <linux/hardirq.h>
#include <linux/kernel.h>

#include "hwifi_utils.h"
#include "hwifi_roam_fsm.h"
#include "hwifi_roam_main.h"
#include "hwifi_roam_alg.h"
#include "hwifi_cfg80211.h"
#include "hi110x_pm.h"
#include "hwifi_dev_err.h"

#include "hwifi_msg.h"

/*
 * 2 Global Variable Definition
 */

#define fsm_to_roam(fsm) container_of((fsm), struct roam_all_stru, roam_fsm)


static const char* sm_names[]  = {
    "sm_disconn",
    "sm_connect",
    "sm_scanning",
    "sm_roaming",
    "sm_origin",
};

/*
 * 3 Function Definition
 */
void roam_enter_disconn(struct roam_fsm_stru *fsm, void *data);
void roam_enter_connect(struct roam_fsm_stru *fsm, void *data);
void roam_enter_scanning(struct roam_fsm_stru *fsm, void *data);
void roam_enter_roaming(struct roam_fsm_stru *fsm, void *data);
void roam_enter_origin(struct roam_fsm_stru *fsm, void *data);



STATIC void roam_timeout_function(unsigned long data)
{
    struct roam_fsm_stru *fsm;
    FSM_HANDLER handler;

    fsm = (struct roam_fsm_stru *)data;

    HWIFI_INFO("%s: timer timeout", sm_names[fsm->state]);

    handler = fsm->handler[fsm->state][ROAM_FSM_INPUT_TIMEOUT];

    handler(fsm, NULL);
}


STATIC void start_roam_timer(struct roam_fsm_stru *fsm, unsigned long timeout_ms)
{
    if (fsm->is_roam_on)
    {
        HWIFI_DEBUG("Start %lu ms timer ", timeout_ms);

        fsm->timer.function = roam_timeout_function;
        fsm->timer.data = (unsigned long) fsm;

        fsm->timer.expires = jiffies + msecs_to_jiffies(timeout_ms);
        add_timer(&fsm->timer);
    }
}


STATIC void stop_roam_timer(struct roam_fsm_stru *fsm)
{
    if (timer_pending(&fsm->timer))
    {
        HWIFI_DEBUG("Stop timer begin");
        if (in_interrupt())
        {
            del_timer(&fsm->timer);
        }
        else
        {
            del_timer_sync(&fsm->timer);
        }

        HWIFI_DEBUG("Stop timer end");
    }
}

#ifdef WLAN_POWER_MANAGEMENT
STATIC void roam_power_mgr(struct roam_all_stru *roam, enum roam_fsm_enum old_state, enum roam_fsm_enum next_state)
{
    struct cfg_struct *cfg;

    if(NULL == roam)
    {
        HWIFI_WARNING("cfg is null argument.");
        return;
    }

    cfg = roam_to_cfg(roam);

    if(IS_ROAMING_STATE(next_state) && !roam->power_manager_flag)
    {
        HWIFI_INFO("roam start,stop power manager");
        roam->power_manager_flag = TRUE;

        /* 关闭低功耗，告知低功耗当前漫游状态 */
        cfg->hi110x_dev->pm_info->set_roam_state(cfg->hi110x_dev, TRUE);
        hi110x_stop_powermgmt(cfg->hi110x_dev);
    }
    else
    {
        if((next_state == ROAM_FSM_STATE_DISCONN) && (roam->power_manager_flag))
        {
            /* 去关联，如果之前设置过漫游低功耗标志，则说明是漫游失败去关联 */
            HWIFI_INFO("roam end,roam failed and enter disconnect.");
            roam->power_manager_flag = FALSE;
            cfg->hi110x_dev->pm_info->set_roam_state(cfg->hi110x_dev, FALSE);
            hi110x_start_powermgmt(cfg->hi110x_dev, FALSE);
        }
        else if((next_state == ROAM_FSM_STATE_CONNECT)
                 && (ROAM_FSM_STATE_ORIGIN == old_state)
                 && (roam->power_manager_flag))
        {
            /* 漫游过程失败，回归到以前AP */
            HWIFI_INFO("roam end,roam failed and back to old ap.");

            roam->power_manager_flag = FALSE;
            cfg->hi110x_dev->pm_info->set_roam_state(cfg->hi110x_dev, FALSE);
            hi110x_start_powermgmt(cfg->hi110x_dev, TRUE);
        }
    }
}
#endif

STATIC void roam_fsm_step(struct roam_fsm_stru *fsm, enum roam_fsm_enum next_state)
{
    struct roam_all_stru *roam;
    roam = fsm_to_roam(fsm);

    HWIFI_INFO("Form %s to %s", sm_names[fsm->state], sm_names[next_state]);

    if (next_state == fsm->state)
    {
        HWIFI_WARNING("Switch to same sm");
        return;
    }

    //TODO clean

    /* stop timer */
    stop_roam_timer(fsm);

    /* if exit from roaming sm, release ref count of candidate */
    if (ROAM_FSM_STATE_ROAMING == fsm->state && NULL != fsm->candidate)
    {
        hwifi_roam_candidate_ap_put(fsm->candidate);
        fsm->candidate = NULL;
    }

    /* if exit from scanning sm, back to home channel */
    if (ROAM_FSM_STATE_SCANNING == fsm->state && ROAM_FSM_STATE_ROAMING != next_state)
    {
        roam_go_home_channel(roam, fsm->connected_band, fsm->connected_channel);
    }

    /* 如果进入漫游状态，关闭低功耗 */
#ifdef WLAN_POWER_MANAGEMENT
    roam_power_mgr(roam, fsm->state, next_state);
#endif

    switch(next_state)
    {
        case ROAM_FSM_STATE_DISCONN:
            roam_enter_disconn(fsm, NULL);
            break;
        case ROAM_FSM_STATE_CONNECT:
            roam_enter_connect(fsm, NULL);
            break;
        case ROAM_FSM_STATE_SCANNING:
            roam_enter_scanning(fsm, NULL);
            break;
        case ROAM_FSM_STATE_ROAMING:
            roam_enter_roaming(fsm, NULL);
            break;
        case ROAM_FSM_STATE_ORIGIN:
            roam_enter_origin(fsm, NULL);
            break;
        default:
            break;
    }
}



void roam_handle_donothing(struct roam_fsm_stru *fsm, void *data)
{
    /* do nothing */
    HWIFI_DEBUG("%s: do nothing", sm_names[fsm->state]);
}

void roam_handle_disconnect(struct roam_fsm_stru *fsm, void *data)
{
    roam_fsm_step(fsm, ROAM_FSM_STATE_DISCONN);
}



void roam_handle_off(struct roam_fsm_stru *fsm, void *data)
{
    enum roam_fsm_enum last_fsm;

    last_fsm = fsm->state;

    fsm->is_roam_on = FALSE;

    if (last_fsm != ROAM_FSM_STATE_DISCONN && last_fsm != ROAM_FSM_STATE_CONNECT)
    {
        roam_fsm_step(fsm, ROAM_FSM_STATE_CONNECT);
    }
}




void roam_enter_disconn(struct roam_fsm_stru *fsm, void *data)
{
    struct roam_all_stru *roam;
    roam = fsm_to_roam(fsm);

    fsm->state = ROAM_FSM_STATE_DISCONN;

    /* stop buffering */
    roam_stop_buffering(roam);

    /* clear all buffered data when roaming */
    roam_clean_buffering(roam);
}



void roam_disconn_handle_connect(struct roam_fsm_stru *fsm, void *data)
{
    struct cfg_struct *cfg;
    /* 每次去关联，device都会清空漫游配置参数，因此在关联上后，重新下发漫游配置参数到device */
    struct roam_all_stru *roam;
    roam = fsm_to_roam(fsm);
    roam_set_params(roam);

    cfg = roam_to_cfg(roam);
    /* 关联上重新下发一次扫描上报标志,防止DEVICE出现异常导致标志位复位 */
    if(cfg->hi110x_dev->pm_info->in_suspend)
    {
        /* 暗屏 */
        hwifi_set_dev_roam_report_enabled(cfg, ROAM_DEVICE_SCAN_TRIGGER_REPORT_DISABLE);
    }
    else
    {
        /* 亮屏 */
        hwifi_set_dev_roam_report_enabled(cfg, ROAM_DEVICE_SCAN_TRIGGER_REPORT_ENABLE);
    }

    fsm->roamed_ts = jiffies;

    roam->roam_fsm.scan_ts = jiffies;
    fsm->roam_scan_miss_count = 0;
    fsm->roam_failed_count = 0;

    fsm->roam_scan_interval_s = 60;  /* 60s作为初始值,以免cfg出现null没有赋值的情况 */

    fsm->force_trigger_scan_flag = 0;

    if(cfg != NULL)
    {
        fsm->roam_scan_interval_s = calc_scan_interval(roam, cfg->latest_signal);
    }

    roam_fsm_step(fsm, ROAM_FSM_STATE_CONNECT);
}

void roam_enter_connect(struct roam_fsm_stru *fsm, void *data)
{
    struct roam_all_stru *roam;
    roam = fsm_to_roam(fsm);

    fsm->state = ROAM_FSM_STATE_CONNECT;

    HWIFI_DEBUG("fsm->connected_type is:%d", fsm->connected_type);

    /* open/wep resume data sending, send garp */
    /* wpa/rsn/wapi only send garp */
    switch(fsm->connected_type)
    {
        case CONNECTED_OPEN:
        case CONNECTED_WEP:
            /* 统一在wpa_supplicant下发关联成功成功后发送缓存数据 */
            #if 0
            roam_stop_buffering(roam);
            roam_send_buffering(roam);
            #endif
        case CONNECTED_TKIP:
        case CONNECTED_CCMP:
        case CONNECTED_WAPI:
            /* 此时4次握手尚未完成，发送arp数据对方无法解密 */
            /* 统一在wpa_supplicant下发关联成功成功后发送缓存数据及arp报文 */
            #if 0
            if (fsm->is_roam_on)
            {
                roam_send_garp(roam);
            }
            #endif
            break;
        default:
            break;
    }
}

void roam_connect_handle_trigger(struct roam_fsm_stru *fsm, void *data)
{
    struct cfg_struct *cfg;
    struct roam_all_stru *roam;
    roam = fsm_to_roam(fsm);

    if (!fsm->is_roam_on)
    {
        return;
    }

    cfg = roam_to_cfg(roam);
    if(!cfg->hi110x_dev->pm_info->connect_finished)
    {
        HWIFI_INFO("IP address is not set,connect is not complete,do not roam to new ap.");
        return;
    }

    if(P2P_ON(cfg))
    {
        HWIFI_INFO("p2p mode,ignore roam trigger event.");
        return;
    }

#ifdef WLAN_POWER_MANAGEMENT
    if(cfg->hi110x_dev->pm_info->in_suspend)
    {
        HWIFI_INFO("screen off,do not roam to new ap.");
        hi110x_set_suspend(cfg->hi110x_dev->pm_info, cfg->hi110x_dev->pm_info->in_suspend);
        return;
    }
#endif
    if (roam_need_scan(roam))
    {
        /* 漫游扫描，降低bt优先级 */
        hwifi_reduce_bt_priority();
        mutex_lock(&roam->roam_scan_lock);
        /* suspend data sending */

        roam_fsm_step(fsm, ROAM_FSM_STATE_SCANNING);
        mutex_unlock(&roam->roam_scan_lock);
    }
}



void roam_connect_handle_addkey(struct roam_fsm_stru *fsm, void *data)
{
    struct roam_all_stru *roam;
    roam = fsm_to_roam(fsm);

    HWIFI_DEBUG("connected,send buffer data.");

    /* resume data sending */
    roam_stop_buffering(roam);

    roam_send_buffering(roam);

    if (fsm->is_roam_on)
    {
        roam_send_garp(roam);
    }
}

void roam_enter_scanning(struct roam_fsm_stru *fsm, void *data)
{
    struct roam_all_stru *roam;

    roam = fsm_to_roam(fsm);
    fsm->state = ROAM_FSM_STATE_SCANNING;

    /* setting scan params */
    fsm->scan_probereq_count = ROAM_SCANNING_PROBE_REQ_COUNT;
    fsm->scan_channel_index = fsm->scan_channel_count - 1;
    fsm->next_subsm = ROAM_SCANNING_GO_FOREIGN;

    HWIFI_DEBUG("scan_channel_index = %d,scan_channel_count=%d", fsm->scan_channel_index, fsm->scan_channel_count);

    /* clear all last scanned candidate */
    hwifi_roam_clear(roam);

    /* start timer */
    start_roam_timer(fsm, 0);
}

void roam_scanning_handle_timeout(struct roam_fsm_stru *fsm, void *data)
{
    int32 foreign_time_ms;
    int32 home_time_ms;
    int32 band, channel;
    struct roam_all_stru *roam;

    roam = fsm_to_roam(fsm);

    foreign_time_ms = roam->alg_param.scan_param.single_user_foreign_dwell_time_ms
                        * roam->alg_param.scan_param.concurrent_user_number;
    home_time_ms = roam->alg_param.scan_param.single_user_home_dwell_time_ms
                        * roam->alg_param.scan_param.concurrent_user_number;

    HWIFI_DEBUG("current_subsm = %d,scan_channel_index=%d,foreign_time_ms=%d, home_time_ms=%d",
            fsm->next_subsm, fsm->scan_channel_index, foreign_time_ms, home_time_ms);

    switch(fsm->next_subsm)
    {
        case ROAM_SCANNING_GO_FOREIGN:
            band = fsm->scan_channels[fsm->scan_channel_index].band;
            channel = fsm->scan_channels[fsm->scan_channel_index].channel;
            roam_go_foreign_channel(roam, band, channel);
            fsm->scan_probereq_remainder = fsm->scan_probereq_count;
            fsm->next_subsm = ROAM_SCANNING_SEND_PROBEREQ;
        case ROAM_SCANNING_SEND_PROBEREQ:
            roam_send_probe_req(roam);
            fsm->scan_probereq_remainder--;

            /* if complete sending all probe request, go back to home channel */
            if (fsm->scan_probereq_remainder <= 0)
            {
                fsm->next_subsm = ROAM_SCANNING_GO_HOME;
            }

            start_roam_timer(fsm, foreign_time_ms / (fsm->scan_probereq_count));
            break;
        case ROAM_SCANNING_GO_HOME:
            roam_go_home_channel(roam, fsm->connected_band, fsm->connected_channel);
            fsm->scan_channel_index--;

            /* if complete all channels scan,
             *  go to select roaming candidate,
             *  else countinue scan foreign channel
             */
            fsm->next_subsm = (fsm->scan_channel_index < 0) ?
                                    ROAM_SCANNING_GO_ROAMING : ROAM_SCANNING_GO_FOREIGN;

            start_roam_timer(fsm, home_time_ms);
            break;
        case ROAM_SCANNING_GO_ROAMING:
            roam_fsm_step(fsm, ROAM_FSM_STATE_ROAMING);
            break;
            default:
        break;
    }

    HWIFI_DEBUG("next_subsm = %d", fsm->next_subsm);
}




void roam_enter_roaming(struct roam_fsm_stru *fsm, void *data)
{
    int ret;
    struct roam_all_stru *roam;
    struct cfg_struct *cfg;

    fsm->state = ROAM_FSM_STATE_ROAMING;

    roam = fsm_to_roam(fsm);

    cfg = roam_to_cfg(roam);

    fsm->candidate = hwifi_roam_select_candidate_ap(roam);
    if (NULL == fsm->candidate)
    {
#ifdef WLAN_POWER_MANAGEMENT
        roam->power_manager_flag = FALSE;
        cfg->hi110x_dev->pm_info->set_roam_state(cfg->hi110x_dev, FALSE);
        hi110x_start_powermgmt(cfg->hi110x_dev, TRUE);
#endif

        /* no candidate available, back to connect state */
        HWIFI_INFO("No candidate available");
        roam_fsm_step(fsm, ROAM_FSM_STATE_CONNECT);
        return;
    }

    roam_start_buffering(roam);

    /* 漫游关联，降低bt优先级 */
    hwifi_reduce_bt_priority();

    ret = roam_try_roaming(roam, fsm->candidate);

    if (SUCC != ret)
    {
#ifdef WLAN_POWER_MANAGEMENT
        roam->power_manager_flag = FALSE;
        cfg->hi110x_dev->pm_info->set_roam_state(cfg->hi110x_dev, FALSE);
        hi110x_start_powermgmt(cfg->hi110x_dev, TRUE);
#endif

        roam_stop_buffering(roam);
        roam_send_buffering(roam);

        /* sending roaming switch cmd fail, back to connect state */
        roam_fsm_step(fsm, ROAM_FSM_STATE_CONNECT);
        return;
    }

    /* wait for return_to_old or roamed msg */
    start_roam_timer(fsm, ROAM_ROAMING_TIMEOUT_MS);
}





void roam_roaming_handle_roamed(struct roam_fsm_stru *fsm, void *data)
{
    struct roam_all_stru *roam;

    roam = fsm_to_roam(fsm);

    if (NULL == fsm->candidate)
    {
        HWIFI_WARNING("the ap candidate has been deleted");
        return;
    }
    hwifi_roam_update_ext_ap(roam, fsm->candidate->bssid);

    fsm->roamed_ts = jiffies;
    fsm->roam_failed_count = 0;
    hwifi_roam_blacklist_bssid_clear(roam, fsm->candidate->bssid);
    roam_fsm_step(fsm, ROAM_FSM_STATE_CONNECT);
}



void roam_roaming_handle_return(struct roam_fsm_stru *fsm, void *data)
{
    struct roam_all_stru *roam;

    if (NULL == fsm->candidate)
        {
            HWIFI_WARNING("The Ap candidate has been deleted");
            return;
        }
    roam = fsm_to_roam(fsm);
    hwifi_roam_update_blacklist(roam, fsm->candidate->bssid);
    fsm->roam_failed_count++;
    roam_fsm_step(fsm, ROAM_FSM_STATE_ORIGIN);
}



void roam_roaming_handle_timeout(struct roam_fsm_stru *fsm, void *data)
{
    struct roam_all_stru *roam;

    roam = fsm_to_roam(fsm);

    if (NULL != fsm->candidate)
    {
        /* send timeout msg to device */
        roam_roaming_timeout(roam, fsm->candidate->bssid);

        /* wait for return_to_old or roamed msg */
        start_roam_timer(fsm, ROAM_ROAMING_TIMEOUT_MS);
    }
}





void roam_enter_origin(struct roam_fsm_stru *fsm, void *data)
{

    fsm->state = ROAM_FSM_STATE_ORIGIN;

    /* 立即回到关联状态，无需等待 */
    start_roam_timer(fsm, 0);
}

void roam_origin_handle_timeout(struct roam_fsm_stru *fsm, void *data)
{
    struct roam_all_stru *roam;

    roam_fsm_step(fsm, ROAM_FSM_STATE_CONNECT);

    if(NULL != fsm)
    {
        roam = fsm_to_roam(fsm);
        roam_stop_buffering(roam);
        roam_send_buffering(roam);
    }
}




int roam_fsm_init(struct roam_fsm_stru *fsm)
{
    uint32 fsm_state_loop;
    uint32 fsm_input_loop;

    for(fsm_state_loop = 0; fsm_state_loop < ROAM_FSM_STATE_BUTT; fsm_state_loop++)
    {
        for(fsm_input_loop = 0; fsm_input_loop < ROAM_FSM_INPUT_BUTT; fsm_input_loop++)
        {
            fsm->handler[fsm_state_loop][fsm_input_loop] = roam_handle_donothing;
        }

        fsm->handler[fsm_state_loop][ROAM_FSM_INPUT_OFF] = roam_handle_off;
        fsm->handler[fsm_state_loop][ROAM_FSM_INPUT_DISCONNET] = roam_handle_disconnect;
    }


    fsm->handler[ROAM_FSM_STATE_DISCONN][ROAM_FSM_INPUT_CONNECT] = roam_disconn_handle_connect;

    fsm->handler[ROAM_FSM_STATE_CONNECT][ROAM_FSM_INPUT_TRIGGER] = roam_connect_handle_trigger;
    fsm->handler[ROAM_FSM_STATE_CONNECT][ROAM_FSM_INPUT_ADDKEY] = roam_connect_handle_addkey;

    fsm->handler[ROAM_FSM_STATE_SCANNING][ROAM_FSM_INPUT_TIMEOUT] = roam_scanning_handle_timeout;

    fsm->handler[ROAM_FSM_STATE_ROAMING][ROAM_FSM_INPUT_ROAMED] = roam_roaming_handle_roamed;
    fsm->handler[ROAM_FSM_STATE_ROAMING][ROAM_FSM_INPUT_RETURN] = roam_roaming_handle_return;
    fsm->handler[ROAM_FSM_STATE_ROAMING][ROAM_FSM_INPUT_TIMEOUT] = roam_roaming_handle_timeout;

    fsm->handler[ROAM_FSM_STATE_ORIGIN][ROAM_FSM_INPUT_TIMEOUT] = roam_origin_handle_timeout;


    spin_lock_init(&fsm->lock);
    init_timer(&fsm->timer);

    /* init roam sm to  ROAM_FSM_STATE_DISCONN*/
    fsm->state = ROAM_FSM_STATE_DISCONN;

    HWIFI_DEBUG("Succeed to init fsm roam!");
    return SUCC;
}

void roam_fsm_exit(struct roam_fsm_stru *fsm)
{
    stop_roam_timer(fsm);
}

void roam_process_on(struct roam_all_stru* roam)
{
    if (!roam)
    {
        HWIFI_WARNING("Invalid NULL roam");
        return;
    }

    roam->roam_fsm.is_roam_on = TRUE;
}

void roam_process_off(struct roam_all_stru* roam)
{
    FSM_HANDLER handler;

    if (!roam)
    {
        HWIFI_WARNING("Invalid NULL roam");
        return;
    }

    HWIFI_DEBUG("%s handler", sm_names[roam->roam_fsm.state]);
    handler = roam->roam_fsm.handler[roam->roam_fsm.state][ROAM_FSM_INPUT_OFF];

    handler(&roam->roam_fsm, NULL);
}

void roam_process_connect(struct roam_all_stru* roam, enum crypto_type_enum crypto_type, uint32 band, uint32 channel)
{
    FSM_HANDLER handler;

    if (!roam)
    {
        HWIFI_WARNING("Invalid NULL roam!");
        return;
    }

    /* save current connected ap's info */
    roam->roam_fsm.connected_type = crypto_type;
    roam->roam_fsm.connected_band = band;
    roam->roam_fsm.connected_channel = channel;

    /* update roam scan channel list */
    //if (roam->roam_fsm.is_roam_on)
    {
        roam_update_scan_channels(roam);
    }

    HWIFI_INFO("%s handler", sm_names[roam->roam_fsm.state]);

    handler = roam->roam_fsm.handler[roam->roam_fsm.state][ROAM_FSM_INPUT_CONNECT];

    handler(&roam->roam_fsm, NULL);

    /* update roaming params */
    /*roam_set_params(roam);*/
}
void roam_process_disconnect(struct roam_all_stru* roam)
{
    FSM_HANDLER handler;

    if (!roam)
    {
        HWIFI_WARNING("Invalid NULL roam");
        return;
    }

    HWIFI_DEBUG("%s handler", sm_names[roam->roam_fsm.state]);
    handler = roam->roam_fsm.handler[roam->roam_fsm.state][ROAM_FSM_INPUT_DISCONNET];

    handler(&roam->roam_fsm, NULL);
}

void roam_process_addkey(struct roam_all_stru* roam)
{
    FSM_HANDLER handler;

    if (!roam)
    {
        HWIFI_WARNING("Invalid NULL roam");
        return;
    }

    HWIFI_DEBUG("%s handler", sm_names[roam->roam_fsm.state]);
    handler = roam->roam_fsm.handler[roam->roam_fsm.state][ROAM_FSM_INPUT_ADDKEY];

    handler(&roam->roam_fsm, NULL);
}

void roam_process_trigger(struct roam_all_stru* roam)
{
    FSM_HANDLER handler;

    if (!roam)
    {
        HWIFI_WARNING("Invalid NULL roam!");
        return;
    }

    HWIFI_DEBUG("%s handler", sm_names[roam->roam_fsm.state]);
    handler = roam->roam_fsm.handler[roam->roam_fsm.state][ROAM_FSM_INPUT_TRIGGER];

    handler(&roam->roam_fsm, NULL);
}


void roam_process_roamed(struct roam_all_stru* roam, uint32 band, uint32 channel)
{
    FSM_HANDLER handler;

    if (!roam)
    {
        HWIFI_WARNING("Invalid NULL roam!");
        return;
    }
    roam->roam_fsm.connected_band = band;
    roam->roam_fsm.connected_channel = channel;

    /* update roam scan channel list */
    //if (roam->roam_fsm.is_roam_on)
    {
        roam_update_scan_channels(roam);
    }

    HWIFI_DEBUG("%s handler", sm_names[roam->roam_fsm.state]);
    handler = roam->roam_fsm.handler[roam->roam_fsm.state][ROAM_FSM_INPUT_ROAMED];

    handler(&roam->roam_fsm, NULL);
}

void roam_process_return(struct roam_all_stru* roam)
{
    FSM_HANDLER handler;

    if (!roam)
    {
        HWIFI_WARNING("Invalid NULL roam!");
        return;
    }

    HWIFI_DEBUG("%s handler", sm_names[roam->roam_fsm.state]);
    handler = roam->roam_fsm.handler[roam->roam_fsm.state][ROAM_FSM_INPUT_RETURN];

    handler(&roam->roam_fsm, NULL);
}


#endif /* __ROAM__ */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


