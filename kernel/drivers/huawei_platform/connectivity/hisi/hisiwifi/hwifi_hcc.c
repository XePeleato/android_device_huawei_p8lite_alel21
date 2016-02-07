


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
//#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/netdevice.h>
#include <linux/stat.h>   /* for S_IRUGO|S_IWUSR */
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/jiffies.h>
#include "common.h"
#include "hwifi_hcc.h"
#include "hi110x.h"
#include "hi110x_pm.h"
#include "hwifi_utils.h"
#include "hwifi_netdev.h"
#include "hwifi_perform.h"

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
#include "hwifi_aggre.h"
#endif

#include "hwifi_dev_err.h"

#include "hwifi_config.h"

#include "hwifi_cfg80211.h"

/*
 * 2 Global Variable Definition
 */
uint32 hcc_assemble_count = HCC_DEFAULT_ASSEMBLE_NUM;
module_param(hcc_assemble_count, uint, S_IRUGO|S_IWUSR);

static int    peak_debug = FALSE;
module_param(peak_debug, int, S_IRUGO|S_IWUSR);

extern int32 exception_enable;
#define ASSEMBLE_RESET_DISABLE     0
#define ASSEMBLE_RESET_ENABLE      1

#define HCC_PKT_LIMIT    (32)

/*millisecond*/
#define HCC_NET_STREAM_PERIOD_TIME  (10*1000)

/*Lower than 1% , we should disconnect.*/
#define HCC_NET_STREAM_PERCENT_LIMIT    (1)


volatile uint32 g_flowctrl_flag         = FLOWCTRL_ON;
volatile uint32 g_assemble_reset_flag   = ASSEMBLE_RESET_DISABLE;

extern int32 g_lCpuId;
extern int32 wl_aggre_mode_lock_set(struct cfg_struct *cfg, uint8 enabled);

/*queue string add by z00262551*/
#define QUEUE_STR_LEN    (HCC_QUEUE_COUNT + 1)
static char* hcc_queue_str[QUEUE_STR_LEN] = {
    "CTRL_QUEUE",
    "DATA_HI_QUEUE",
    "DATA_LO_QUEUE",
    "DATA_TCP_DATA_QUEUE",
    "DATA_TCP_ACK_QUEUE",
#ifdef WMM_OPT_FOR_AUTH
    "DATA_UDP_BK_QUEUE",
    "DATA_UDP_BE_QUEUE",
    "DATA_UDP_VI_QUEUE",
    "DATA_UDP_VO_QUEUE",
#endif
    "QUEUE_VALID"
};

static char* hcc_net_stream_str[STREAM_COUNT] = {
    [STREAM_UDP_VO] = "ac_udp_vo",
    [STREAM_UDP_VI] = "ac_udp_vi",
    [STREAM_UDP_RTP] = "udp_rtp",
    [STREAM_UDP_RTCP] = "udp_rtcp",
};

static char* hcc_action_str[HCC_ACTION_COUNT] = {
    [HCC_TYPE_DATA] = "DATA PACKAGE",
    [HCC_TYPE_HCC_CTRL] = "HCC CTRL PACKAGE",
    [HCC_TYPE_BEATHEART] = "BEATHEART PACKAGE",
    [HCC_TYPE_WIFI_CTRL] = "WIFI CTRL PACKAGE",
    [HCC_TYPE_OAM] = "OAM PACKAGE",
    [HCC_TYPE_TEST] = "TEST PACKAGE",
    [HCC_TYPE_DEV_ERROR] = "DEV ERROR PACKAGE",
    [HCC_TYPE_UNKOWN] = "UNKOWN PACKAGE"
};

#define HCC_FLUSH_ALL   (0xFFFFFFFFUL)

typedef enum _hcc_send_mode_
 {
    hcc_single_send  = 0,
    hcc_assem_send,
    hcc_mode_count
 }hcc_send_mode;

static char* hcc_send_mode_str[hcc_mode_count] = {
    "hcc send single package",
    "hcc send assemble package",
};
static char* hcc_chan_stat_str[hcc_valid + 1] = {
    "stop",
    "running",
    "sleeping",
    "abort",
    "vaild"
};

static char* hcc_dir_stat_str[HCC_DIR_COUNT] = {
    "hcc tx",
    "hcc rx"
};

static char* net_stream_sm_str[HNET_STREAM_SM_COUNT] = {
    [HNET_STREAM_DISCONN] = "DISCONN",
    [HNET_STREAM_CONNTING] = "CONNTING",
    [HNET_STREAM_CONNETED] ="CONNETED"
};

#define SET_PKT_MORE_FLAG(skb, val)     do {\
                                            header = (struct hcc_header *)(skb)->data; \
                                            header->more = (val);                      \
                                        } while (0)

/*
 * 3 Function Definition
 */
 STATIC int32 hcc_send_single_data_packet(struct hcc_handler* hcc, struct sk_buff_head *head,hcc_queue_type type,uint32 next_assemble_cnt);
 STATIC int32 hcc_send_assemble_data_packet(struct hcc_handler *hcc, struct sk_buff_head* head,hcc_queue_type type,uint32 assembled_cnt, uint32 next_assemble_cnt);
 STATIC inline int32 hcc_send_assemble_reset(struct hcc_handler *hcc);
 STATIC void hcc_bt_opt_exit(struct hcc_handler   *hcc);

char* hcc_get_action_str(hcc_action_type action)
{
    action = MIN(action, (hcc_action_type)HCC_TYPE_UNKOWN);
    return hcc_action_str[action] ? :"reserved action";
}

char* hcc_get_chan_stat_str(hcc_state state)
{
    state = MIN(state, (hcc_state)hcc_valid);
    return hcc_chan_stat_str[state];
}

char* hcc_get_net_stream_sm_str(hwifi_net_stream_sm sm_stat)
{
    if(HWIFI_WARN_ON(sm_stat >= HNET_STREAM_SM_COUNT))
    {
        return "unkown stream";
    }
    return net_stream_sm_str[sm_stat];
}

hwifi_net_stream_type hcc_find_stream_by_name(struct hcc_handler* hcc,
                                                                                                                        const char* name)
{
    int i;
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!name);
    for(i = 0; i < STREAM_COUNT; i++)
    {
        if(!strcmp(name, hcc->net_stream.stream[i].name))
            break;
    }
    return i;
}

int hcc_queues_flow_ctrl_len(struct hcc_handler* hcc,
                                        hcc_chan_type dir)
{
    HWIFI_WARN_ON(HCC_TX == dir);
    return atomic_read(&hcc->hcc_queues[dir].wait_count);
}
STATIC inline void hcc_flow_ctrl_len_add(struct hcc_handler* hcc,
                                        hcc_chan_type dir, int i)
{
    HWIFI_WARN_ON(HCC_TX == dir);
    atomic_add(i, &hcc->hcc_queues[dir].wait_count);
}
STATIC inline void hcc_flow_ctrl_len_sub(struct hcc_handler* hcc,
                                        hcc_chan_type dir, int i)
{
    HWIFI_WARN_ON(HCC_TX == dir);
    atomic_sub(i, &hcc->hcc_queues[dir].wait_count);
    if(unlikely(atomic_read(&hcc->hcc_queues[dir].wait_count)<0))
    {
        HWIFI_ERROR("Error %s wait count:%d",
                    hcc_get_dir_str(dir),atomic_read(&hcc->hcc_queues[dir].wait_count));
    }
}

/*the skb count not under flow ctrl, 0 for no data, 1 for have data*/
STATIC int _queues_flow_not_ctrl_len_check(struct hcc_handler* hcc, hcc_chan_type dir)
{
    int i ;
    for(i = 0; i < HCC_QUEUE_COUNT;i++)
    {
            if(!hcc->hcc_queues[dir].queues[i].flow_ctrl.enable)
            {
                if(skb_queue_len(&hcc->hcc_queues[dir].queues[i].data_queue))
                {
                        return TRUE;
                }
            }
    }
    return FALSE;
}

/*wether the hcc flow ctrl queues have data.*/
STATIC int _queues_flow_ctrl_len_check(struct hcc_handler* hcc, hcc_chan_type dir)
{
    int i ;
    for(i = 0; i < HCC_QUEUE_COUNT;i++)
    {
            if(hcc->hcc_queues[dir].queues[i].flow_ctrl.enable)
            {
                if(skb_queue_len(&hcc->hcc_queues[dir].queues[i].data_queue))
                {
                        return TRUE;
                }
            }
    }
    return FALSE;
}

/*wether the hcc queue have data.*/
STATIC int _queues_len_check(struct hcc_handler* hcc, hcc_chan_type dir)
{
    int i ;
    for(i = 0; i < HCC_QUEUE_COUNT;i++)
    {
            if(skb_queue_len(&hcc->hcc_queues[dir].queues[i].data_queue))
            {
                    return TRUE;
            }
    }
    return FALSE;
}

/*the hcc_trans_thread wait event condition check*/
STATIC int  hcc_thread_wait_event_cond_check(struct hcc_handler* hcc)
{
    HWIFI_BUG_ON(!hcc);
    /*please first check the condition which may be ok likely to reduce the cpu mips*/
    return (_queues_flow_ctrl_len_check(hcc, HCC_TX) && (g_flowctrl_flag == FLOWCTRL_ON))
                   ||(_queues_len_check(hcc, HCC_RX))
                   ||(_queues_flow_not_ctrl_len_check(hcc, HCC_TX));
}

int hcc_queue_flow_ctrl_len(struct hcc_handler* hcc,
                                        hcc_queue_type type,
                                        hcc_chan_type dir)
{
    HWIFI_WARN_ON(HCC_RX == dir);
    return atomic_read(&hcc->hcc_queues[dir].queues[type].flow_ctrl.wait_count);
}
STATIC inline void hcc_queue_flow_ctrl_add(struct hcc_handler* hcc,
                                        hcc_queue_type type,
                                        hcc_chan_type dir, int i)
{
    HWIFI_WARN_ON(HCC_RX == dir);
    atomic_add(i, &hcc->hcc_queues[dir].queues[type].flow_ctrl.wait_count);
}
STATIC inline void hcc_queue_flow_ctrl_sub(struct hcc_handler* hcc,
                                        hcc_queue_type type,
                                        hcc_chan_type dir, int i)
{
    HWIFI_WARN_ON(HCC_RX == dir);
    atomic_sub(i, &hcc->hcc_queues[dir].queues[type].flow_ctrl.wait_count);
    if(unlikely(atomic_read(&hcc->hcc_queues[dir].queues[type].flow_ctrl.wait_count)<0))
    {
        HWIFI_ERROR("Error %s queue %s wait count:%d",
                    hcc_get_dir_str(dir),hcc_get_queue_str(type),
                    atomic_read(&hcc->hcc_queues[dir].queues[type].flow_ctrl.wait_count));
    }
}
STATIC void hcc_queues_flow_ctrl_rx_handle(struct hcc_handler* hcc,
                                        hcc_queue_type type,
                                        hcc_chan_type dir,
                                        bool reduce,int i)
{
    unsigned long jiffies_timeout;
    struct hi110x_sdio *hi_sdio =  bus_to_sdio(hcc->io_dev);
    HWIFI_WARN_ON(HCC_RX != dir);
    /*5s timeout*/
    jiffies_timeout = jiffies + msecs_to_jiffies(HCC_RX_WL_TIMEOUT);
    while(1)
    {
        if(time_after(jiffies, jiffies_timeout))
        {
            HWIFI_WARNING("the rx queue still have %d skbs after rx thread block %d ms",
                                               hcc_queues_flow_ctrl_len(hi_sdio->hi110x_dev->hcc, HCC_RX),
                                               HCC_RX_WL_TIMEOUT);
            /*hsdio_union_log_submit(hi_sdio); */
            break;
        }
        if(hcc_queues_flow_ctrl_len(hi_sdio->hi110x_dev->hcc, HCC_RX)> HCC_RX_HI_WATERLINE)
        {
            /*rx queue block too many skbs, we  schedule one period util the hcc_rx process the skbs*/
            schedule_timeout_interruptible(1);
        }
        else
        {
            break;
        }

        if(hcc_queues_flow_ctrl_len(hi_sdio->hi110x_dev->hcc, HCC_RX) < HCC_RX_LO_WATERLINE)
        {
            break;
        }
    }
}

STATIC void hcc_queues_flow_ctrl_tx_handle(struct hcc_handler* hcc,
                                        hcc_queue_type type,
                                        hcc_chan_type dir,
                                        bool reduce,int i)
{
    if(!i)
    {
        return;
    }
    HWIFI_WARN_ON(HCC_TX != dir);
    if(!hcc->hcc_queues[dir].queues[type].flow_ctrl.enable)
    {
        HWIFI_PR_DEBUG("[%s][%s] %s:%s %d skbs", hcc_get_dir_str(dir),
                    hcc_get_queue_str(type),
                    hcc->hcc_queues[dir].queues[type].flow_ctrl.enable?"flow_ctrl":""
                    ,reduce?"del":"add",i);
        return;
    }
    spin_lock_bh(&hcc->hcc_queues[dir].flow_ctrl_lock);
    /*hcc tx*/
    if(reduce)
    {
        hcc_queue_flow_ctrl_sub(hcc, type,dir, i);
        /*hcc_flow_ctrl_len_sub(hcc, dir, i);*/
        HWIFI_DEBUG("%s,queue %s,After process %d skbs, still reamin %d",
                    hcc_get_dir_str(dir),hcc_get_queue_str(type),
                    i, hcc_queue_flow_ctrl_len(hcc,type, dir));
        if(__netif_subqueue_stopped(hcc->hi110x_dev->netdev, type))
        {
            if(hcc_queue_flow_ctrl_len(hcc, type,dir) <= hcc->hcc_queues[dir].queues[type].flow_ctrl.lo_waterline)
            {
                hcc_netif_txflowcontrol(hcc, type,FLOWCTRL_ON);
            }
        }
        if (waitqueue_active(&hcc->wait_queue))
        {
            if(hcc_queue_flow_ctrl_len(hcc, type, dir) <= hcc->hcc_queues[dir].queues[type].flow_ctrl.lo_waterline)
            {
                wake_up_interruptible(&hcc->wait_queue);
                HWIFI_DEBUG("wake up hcc-extern-send wait queue");
            }
        }
    }else
    {
        /*hcc_flow_ctrl_len_add(hcc, dir, i);*/
        hcc_queue_flow_ctrl_add(hcc, type, dir, i);
        HWIFI_DEBUG("%s,queue %s,After add %d skbs, now we have %d",
                    hcc_get_dir_str(dir),hcc_get_queue_str(type),
                    i, hcc_queue_flow_ctrl_len(hcc,type, dir));
            if(hcc_queue_flow_ctrl_len(hcc, type, dir) > hcc->hcc_queues[dir].queues[type].flow_ctrl.hi_waterline)
                hcc_netif_txflowcontrol(hcc, type ,FLOWCTRL_OFF);
    }
    spin_unlock_bh(&hcc->hcc_queues[dir].flow_ctrl_lock);
}
void hcc_queues_flow_ctrl_handle(struct hcc_handler* hcc,
                                        hcc_queue_type type,
                                        hcc_chan_type dir,
                                        bool reduce,int i)
{
    if(unlikely(!hcc->hcc_queues[dir].flow_ctrl_handle))
    {
        HWIFI_WARNING("%s flow ctrl handle should not be null!", hcc_get_dir_str(dir));
        return;
    }

    hcc->hcc_queues[dir].flow_ctrl_handle(hcc, type, dir, reduce, i);
}

/*do something when the skb dequeue from the queue*/
STATIC void hcc_trans_skb_dequeue_handle(struct hcc_handler *hcc,hcc_queue_type  type,
                                                                                                               hcc_chan_type  dir, int len_t)
{
    if(!len_t)
        return;
    hcc_queues_len_sub(hcc,dir, len_t);
    hi110x_wake_release_lock(hcc->hi110x_dev->pm_info, len_t);
}

void hcc_set_trans_limit(struct hcc_handler* hcc, hcc_chan_type chan_type,
                          hcc_queue_type   queue_type, uint32 limit)
{
       if(HWIFI_WARN_ON(queue_type >= HCC_QUEUE_COUNT))
       {
            return;
       }
       HWIFI_DEBUG("set %s trans_ limit:%s limit[%u]",
                      (chan_type == HCC_TX)? "hcc_tx":"hcc_rx",
                      hcc_get_queue_str(queue_type),
                      limit);
       hcc->hcc_queues[chan_type].queues[queue_type].queue_trans_limit = limit;
}
EXPORT_SYMBOL(hcc_set_trans_limit);

void hcc_trans_limit_default_config(struct hcc_handler* hcc, enum hi110x_network_mode mode)
{
    if(unlikely(!hcc))
    {
        HWIFI_WARNING("hcc handler is null");
        return;
    }
#if 0
    if(HI_MODE_STA == mode)
    {
       hcc_set_trans_limit(hcc, HCC_RX, DATA_TCP_ACK_QUEUE, 17);
       hcc_set_trans_limit(hcc, HCC_RX, DATA_LO_QUEUE, 17);
       hcc_set_trans_limit(hcc, HCC_TX, DATA_LO_QUEUE, 17);
       hcc_set_trans_limit(hcc, HCC_TX, DATA_TCP_DATA_QUEUE, HCC_FLUSH_ALL);
    }else if(HI_MODE_AP == mode)
    {
       hcc_set_trans_limit(hcc, HCC_TX, DATA_TCP_ACK_QUEUE, 17);
       hcc_set_trans_limit(hcc, HCC_TX, DATA_LO_QUEUE, 17);
       hcc_set_trans_limit(hcc, HCC_RX, DATA_LO_QUEUE, 17);
       hcc_set_trans_limit(hcc, HCC_RX, DATA_TCP_DATA_QUEUE, HCC_FLUSH_ALL);
    }else
    {
        HWIFI_INFO("the mode %d did not config trans limit", mode);
    }
#endif
}

void hcc_trans_dyn_pri_control(struct hcc_handler* hcc, int enable)
{
    HWIFI_BUG_ON(!hcc);
    if(hcc->sys_hcc_dyn_pri_mask)
    {
        return;
    }
    HWIFI_DEBUG("dynamic pri set %s ==> %s",
                hcc->sys_hcc_dyn_pri_enable ? "enable":"disable",
                enable ?  "enable":"disable");
    hcc->sys_hcc_dyn_pri_enable = !!enable;
}
void hcc_sched_transfer(struct hcc_handler* hcc)
{
        HWIFI_BUG_ON(!hcc);
        HWIFI_PR_DEBUG("sched hcc transfer");
        wake_up_interruptible(&hcc->transfer_wait_queue);
}
EXPORT_SYMBOL(hcc_sched_transfer);


 char* hcc_get_queue_str(hcc_queue_type q_type)
 {
    uint32 queue_str_len = sizeof(hcc_queue_str)/sizeof(char*);
    if(HWIFI_WARN(QUEUE_STR_LEN != queue_str_len,
       "wrong queue str request, [QUEUE_STR_LEN=%u][queue_str_len=%u]",
       QUEUE_STR_LEN, queue_str_len))
    {
        return "QUEUE_VALID";
    }
    if(HWIFI_WARN_ON(q_type >= HCC_QUEUE_COUNT))
    {
        HWIFI_WARNING("wrong hcc_queue_type [%d]", q_type);
        q_type  = HCC_QUEUE_COUNT;
    }
    return hcc_queue_str[q_type];
 }

char* hcc_get_dir_str(hcc_chan_type dir)
{
    if(HWIFI_WARN_ON(dir >= HCC_DIR_COUNT))
    {
        HWIFI_WARNING("wrong hcc dir type [%d]", dir);
        return "unkown dir";
    }
    return hcc_dir_stat_str[dir];
}


STATIC  inline int hcc_tx_chan_is_vaild(struct hcc_handler* hcc)
{
    /*if hcc is under running or sleeping,
        whe should continue send the data, host will
        wakeup device*/
    int hcc_stat = hcc_get_tx_queue_status(hcc);
    if(likely((hcc_running == hcc_stat) || (hcc_sleeping == hcc_stat)))
    {
        return TRUE;
    }
    if(printk_ratelimit())
    {
        HWIFI_INFO("hcc channel %s, this transfer data was ignored!", hcc_get_chan_stat_str(hcc_stat));
    }
    return FALSE;
}

STATIC void hcc_dev_flow_ctrl_timeout_isr(struct hcc_handler *hcc)
{
    if(unlikely(!hcc || !hcc->hi110x_dev || !hcc->hi110x_dev->cfg))
    {
        HWIFI_WARNING("ptr is null!");
        return;
    }
    /*flowctrl lock too much time.*/
    HWIFI_INFO("Device flow ctrl had off for %u ms, it's a long time", jiffies_to_msecs(hcc->dev_flow_ctrl.timeout));

    /*Try to send cmd --runtc 251 to query the device mem*/
    hwifi_send_dfx_wid_char(hcc->hi110x_dev->cfg, WID_DFX_RUNTC, 251);
    /*reference to the deivce funtion DFX_GetMacStatics*/
    hwifi_send_dfx_wid_char(hcc->hi110x_dev->cfg, WID_DFX_MAC_STATICS, 9);

    hcc->dev_flow_ctrl.cpu_timeout_time = cpu_clock(UINT_MAX);

    /*Please check the dfx message by sdt*/

    /*Then we try to force to open dev flow ctrl*/
    if(hcc->dev_flow_ctrl.force_ctrl_on)
    {
        HWIFI_INFO("Try to force open dev flow ctrl...");
        hcc_dev_flowctrl_on(hcc, 0);
    }
    else
    {
        HWIFI_INFO("Force dev flow function disabled.");
    }

}

STATIC void hcc_bt_opt_tcp_data_timeout_isr(hcc_bt_opt_item* item)
{
    HWIFI_BUG_ON(!item);
    HWIFI_BUG_ON(!item->hcc);

    if(!item->enable)
        return;

    /*Just care abot STA Mode.*/
    if(HI_MODE_STA != item->hcc->hi110x_dev->mode.current_mode)
        return;

    spin_lock_bh(&item->hcc->bt_opt.lock);
    item->hcc->bt_opt.stat=0x11;
    /*Tcp broken, notfiy device.*/
    hwifi_set_bt_opt(item->hcc->hi110x_dev->cfg, item->hcc->bt_opt.stat);
    spin_unlock_bh(&item->hcc->bt_opt.lock);

}

#ifdef WMM_OPT_FOR_AUTH
STATIC void hcc_wmm_auth_timeout_isr(struct hcc_handler *hcc)
{
    hcc->wmm_info.wmm_high_prio_ac_drop_switch = TRUE;
    HWIFI_INFO("10s has gone ");
}

hcc_queue_type hcc_filter_udp_qos_type(struct wlan_ip_header* ip_hdr, struct hcc_handler *hcc, hcc_chan_type dir)
{
    uint8  priority;
    uint8  offset = 0;
    uint32 current_ts = jiffies;
    uint8  i;
    uint8  old_ac_cnt = 0;
    uint8  new_ac_cnt = 0;
    hcc_queue_type tmp_queue_type = HCC_QUEUE_COUNT;

    HWIFI_BUG_ON(!ip_hdr);

    if (IPV4_VERSION == (ip_hdr->ip_version & 0xF0))
    {
        priority = ip_hdr->ip_diffservice & 0xE0;
    }
    else if (IPV6_VERSION == (ip_hdr->ip_version & 0xF0))
    {
        priority = ((ip_hdr->ip_diffservice & 0x0F) << 4) & 0xE0;
    }
    else
    {
        HWIFI_ERROR("error IP Ver pkt!");
        return tmp_queue_type; /*other version*/
    }

    if (HCC_RX == dir)
        offset = 4;    /*rx high 4 bit*/

    /*judge if the ac flow is out of date or not*/
    for (i = 0; i < AC_COUNT; i++)
    {
        if ((0 != (hcc->wmm_info.ac_bitmap & (1 << i))) &&
        (TRUE == time_before_eq((unsigned long)(hcc->wmm_info.ac_last_ts[i] + msecs_to_jiffies(WMM_AC_TIMEOUT)), (unsigned long)current_ts)))
        {
            hcc->wmm_info.ac_bitmap &= (~(1 << i));
            HWIFI_INFO("ac%d timeout ", i);
        }

        if ((i < RX_AC_BK) && (0 != (hcc->wmm_info.ac_bitmap & (1 << i))))
            old_ac_cnt++;
    }

    if ((old_ac_cnt < hcc->wmm_info.current_ac_cnt) && (2 == hcc->wmm_info.current_ac_cnt))
    {
        hcc->wmm_info.wmm_high_prio_ac_drop_switch = FALSE;
        HWIFI_INFO("clear auth timer flag");
    }

    switch (priority)
    {
        case 0x20:             /* IP-PL1 */
        case 0x40:             /* IP-PL2 */
        {
            hcc->wmm_info.ac_bitmap |= 1 << (TX_AC_BK + offset);
            hcc->wmm_info.ac_last_ts[TX_AC_BK + offset] = jiffies;
            tmp_queue_type = DATA_UDP_BK_QUEUE;  /* background */
        }
        break;
        case 0x80:           /* IP-PL4 */
        case 0xA0:           /* IP-PL5 */
        {
            hcc->wmm_info.ac_bitmap |= 1 << (TX_AC_VI + offset);
            hcc->wmm_info.ac_last_ts[TX_AC_VI + offset] = jiffies;
            tmp_queue_type = DATA_UDP_VI_QUEUE; /* Video */
        }
        break;
        case 0xC0:           /* IP-PL6 */
        case 0xE0:           /* IP-PL7 */
        {
            hcc->wmm_info.ac_bitmap |= 1 << (TX_AC_VO + offset);
            hcc->wmm_info.ac_last_ts[TX_AC_VO + offset] = jiffies;
            tmp_queue_type = DATA_UDP_VO_QUEUE; /* Voice */
        }
        break;
        default:
        {

            hcc->wmm_info.ac_bitmap |= 1 << (TX_AC_BE + offset);
            hcc->wmm_info.ac_last_ts[TX_AC_BE + offset] = jiffies;
            tmp_queue_type = DATA_UDP_BE_QUEUE;   /* Best Effort */
        }
        break;

    }

    for (i = 0; i < RX_AC_BK; i++)
    {
        if (0 != (hcc->wmm_info.ac_bitmap & (1 << i)))
            new_ac_cnt++;
    }

    hcc->wmm_info.current_ac_cnt = new_ac_cnt;

    /*when two ac exist firstly, start timer*/
    if ((hcc->wmm_info.current_ac_cnt > old_ac_cnt) && (2 == hcc->wmm_info.current_ac_cnt))
    {
        mod_timer(&hcc->wmm_info.wmm_auth_timer, jiffies + WMM_AUTH_TIMEOUT * HZ);
        HWIFI_INFO("start auth timer, old_ac_cnt = %d, new_ac_cnt = %d ", old_ac_cnt, new_ac_cnt);
    }
    return tmp_queue_type;
}
#endif

#ifdef WMM_OPT_FOR_AUTH
hcc_queue_type hcc_filter_spec_type(uint8* data, struct hcc_handler *hcc, hcc_chan_type dir)
#else
hcc_queue_type hcc_filter_tcp_type(uint8* data)
#endif
{
    /*find the tcp data, tcp ack*/
    uint16 ip_type;
    int32  ip_pkt_len;
    int32  ip_hdr_len;
    int32  tcp_hdr_len;
    struct wlan_tcp_header *tcp_hdr;
    struct wlan_ip_header  *ip_hdr;
    struct wlan_ethmac_header *eth_hdr;
#ifdef WMM_OPT_FOR_AUTH
    hcc_queue_type ret_type;
#endif
    HWIFI_BUG_ON(!data);

    eth_hdr = (struct wlan_ethmac_header *)data;
    ip_type = ((eth_hdr->mac_protocol[0] << 8) | (eth_hdr->mac_protocol[1]));
    if (IP_TYPE != ip_type)
    {
        HWIFI_PR_DEBUG("This is not ip package, eth header type:0x%x", ip_type);
        return HCC_QUEUE_COUNT;
    }
    ip_hdr = (struct wlan_ip_header *)(data + sizeof(struct wlan_ethmac_header));
    if (TCP_PROTOCOL != ip_hdr->ip_protocol)
    {
        HWIFI_PR_DEBUG("This is not tcp package, ip header type: 0x%x", ip_hdr->ip_protocol);
    #ifdef WMM_OPT_FOR_AUTH
        if ((TRUE == hcc->sys_wmm_ac_opt_enable)
                    && (UDP_PROTOCOL == ip_hdr->ip_protocol))
        {
            ret_type = hcc_filter_udp_qos_type(ip_hdr, hcc, dir);
            return ret_type;
        }
        else
    #endif
        return HCC_QUEUE_COUNT;
    }

    tcp_hdr    = (struct wlan_tcp_header *)((void*)ip_hdr + sizeof(struct wlan_ip_header));
    ip_pkt_len  = WLAN_PERFORM_GET_BE16(ip_hdr->ip_length);
    ip_hdr_len     = LOW_HALF_BYTE(ip_hdr->ip_version) * 4;
    tcp_hdr_len    = HIGH_HALF_BYTE(tcp_hdr->tcp_offset)* 4;
    if ((ip_pkt_len - tcp_hdr_len) != ip_hdr_len)
    {
        /*tcp data*/
        HWIFI_PR_DEBUG("Tcp data ip_pkt_len=%d tcp_hdr_len=%d ip_hdr_len=%d",
                    ip_pkt_len, tcp_hdr_len, ip_hdr_len);
        return DATA_TCP_DATA_QUEUE;
    }
    else
    {
        /*tcp ack*/
        HWIFI_PR_DEBUG("Tcp ack ip_pkt_len=%d tcp_hdr_len=%d ip_hdr_len=%d",
                    ip_pkt_len, tcp_hdr_len, ip_hdr_len);
        return DATA_TCP_ACK_QUEUE;
    }
}


void hcc_netif_txflowcontrol(struct hcc_handler* hcc, int32 queue_mapping,uint32 state)
{
    if (unlikely((NULL == hcc) || (NULL == hcc->hi110x_dev) || (NULL == hcc->hi110x_dev->netdev)))
    {
        HWIFI_WARNING("Error NULL params!");
        return;
    }

    if (!test_bit(HI_STATUS_WLAN_READY, &hcc->hi110x_dev->status))
    {
        HWIFI_WARNING("Network is not open");
        return ;
    }

    /*hcc->net_queue_flowcontrol = state;*/

    if (FLOWCTRL_ON == state)
    {
        netif_wake_subqueue(hcc->hi110x_dev->netdev, queue_mapping);
        HWIFI_DEBUG("Net start transferring");
    }
    else
    {
        netif_stop_subqueue(hcc->hi110x_dev->netdev, queue_mapping);
        HWIFI_DEBUG("Net stop transferring");
    }
}
void hcc_tx_start_queue(struct hcc_handler* hcc, uint8 locked)
{
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!hcc->hi110x_dev);
    HWIFI_INFO("Start hcc tx queue!");
    if(locked)
        hcc_tx_transfer_lock(hcc);

    netif_tx_start_all_queues(hcc->hi110x_dev->netdev);

    if(locked)
        hcc_tx_transfer_unlock(hcc);

}


void hcc_tx_stop_queue(struct hcc_handler* hcc, uint8 locked)
{
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!hcc->hi110x_dev);
    HWIFI_INFO("Stop hcc tx queue!");
    if(locked)
        hcc_tx_transfer_lock(hcc);

    /*stop all network layer transfer*/
    netif_tx_stop_all_queues(hcc->hi110x_dev->netdev);

    if(locked)
        hcc_tx_transfer_unlock(hcc);
}
void hcc_tx_wake_queue(struct hcc_handler* hcc, uint8 locked)
{
    HWIFI_BUG_ON(!hcc);
    /*hcc_tx_start_queue(hcc, locked);*/
    /*wake up hcc tx thread.*/
    hcc_sched_transfer(hcc);
    /*netif_wake_queue(hcc->hi110x_dev->netdev);*/

}
void hcc_set_tx_queue_status(struct hcc_handler* hcc, hcc_state state)
{
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(state >= hcc_valid);
    if(unlikely(state == atomic_read(&hcc->hcc_chan_status)))
    {
        HWIFI_WARNING("Hcc was already under %s![%pF]", hcc_chan_stat_str[state], __builtin_return_address(0));
        return;
    }

    HWIFI_INFO("Hcc chan state switch[%s ==> %s][%pF]",
               hcc_chan_stat_str[atomic_read(&hcc->hcc_chan_status)],
               hcc_chan_stat_str[state],
               __builtin_return_address(0));
    atomic_set(&hcc->hcc_chan_status, state);
}


hcc_state hcc_get_tx_queue_status(struct hcc_handler* hcc)
{
    HWIFI_BUG_ON(!hcc);
    if(HWIFI_WARN_ON(atomic_read(&hcc->hcc_chan_status) >= hcc_valid))
    {
        /*This never should happened*/
        hcc_set_tx_queue_status(hcc, hcc_stop);
        return hcc_stop;
    }
    return atomic_read(&hcc->hcc_chan_status);
}

STATIC void hcc_action_count_reset(struct hcc_handler  *hcc)
{
    int i;
    for(i = 0;i < HCC_DIR_COUNT;i++)
    {
        memset(hcc->hcc_queues[i].hcc_action_count, 0,
               sizeof(hcc->hcc_queues[i].hcc_action_count));
    }
}

STATIC inline void hcc_tx_queue_reset(struct hcc_handler* hcc, hcc_queue_type type)
{
    uint32 freed_total_count = 0;
    uint32 freed_flow_count = 0;
    struct sk_buff *skb;
    HWIFI_BUG_ON(type >= HCC_QUEUE_COUNT);
    while(!!(skb = skb_dequeue(&hcc->hcc_queues[HCC_TX].queues[type].data_queue)))
    {
        dev_kfree_skb(skb);
        freed_total_count++;
        if(TRUE==hcc->hcc_queues[HCC_TX].queues[type].flow_ctrl.enable)
        {
            freed_flow_count++;
        }
    }

    hcc_trans_skb_dequeue_handle(hcc, type, HCC_TX, freed_total_count);

    if(freed_flow_count)
    {
        spin_lock_bh(&hcc->hcc_queues[HCC_TX].flow_ctrl_lock);
        HWIFI_INFO("Sub flow ctrl queue  %s %u skbs,wait count:%d",
                    hcc_get_queue_str(type), freed_flow_count, atomic_read(&hcc->hcc_queues[HCC_TX].wait_count));
        hcc_queue_flow_ctrl_sub(hcc, type,HCC_TX, freed_flow_count);
        spin_unlock_bh(&hcc->hcc_queues[HCC_TX].flow_ctrl_lock);
    }
    else
    {
        if(freed_total_count)
        {
            HWIFI_INFO("Clean hcc tx queue %s [remain len=%u]",
                        hcc_get_queue_str(type), freed_total_count);
        }
    }
}

void hcc_tx_queue_all_reset(struct hcc_handler* hcc, uint8 locked)
{
    int i;
    if(locked)
        hcc_tx_transfer_lock(hcc);
    /*destory all hcc tx queues*/
    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        spin_lock_bh(&hcc->hcc_queues[HCC_TX].queues[i].data_queue_lock);
        hcc_tx_queue_reset(hcc, i);
        spin_unlock_bh(&hcc->hcc_queues[HCC_TX].queues[i].data_queue_lock);
    }
    if(locked)
        hcc_tx_transfer_unlock(hcc);
}


 void hcc_transfer_reset(struct hcc_handler* hcc, uint8 flag)
 {
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!hcc->hi110x_dev);
    if(TRUE == flag)
    {
        hcc->dev_flow_ctrl.flow_ctrl_off_time = jiffies;
        hcc->dev_flow_ctrl.flow_ctrl_on_time = jiffies;

		hcc_bt_opt_exit(hcc);
        hcc_net_work_detect_deinit(hcc);
        hcc_tx_stop_queue(hcc, TRANS_UNLOCK);
        hcc_dev_flowctrl_off(hcc, HWIFI_HOST2DEV);
        /*if need, whe should clear hcc tx queue when down the wlan,
        when suspend we never should clear them*/
        hcc_tx_queue_all_reset(hcc, TRANS_UNLOCK);
        hcc_rx_queue_all_reset(hcc, TRANS_UNLOCK);
        /*need to reset asseminfo wlan had power down*/
        hcc_tx_asseminfo_reset(hcc, TRANS_UNLOCK);
        hcc_action_count_reset(hcc);
    }
    else
    {
        hcc->dev_flow_ctrl.flow_ctrl_off_time = jiffies;
        hcc->dev_flow_ctrl.flow_ctrl_on_time = jiffies;
        hcc_tx_asseminfo_reset(hcc, TRANS_UNLOCK);
        hcc_rx_queue_all_reset(hcc, TRANS_UNLOCK);
        hcc_tx_queue_all_reset(hcc, TRANS_UNLOCK);
        hcc_dev_flowctrl_on(hcc, 0);
        hcc_tx_start_queue(hcc, TRANS_UNLOCK);
    }
 }
EXPORT_SYMBOL(hcc_transfer_reset);
 void hcc_queues_reset(struct hcc_handler* hcc, uint8 locked)
 {
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!hcc->hi110x_dev);
    hcc_tx_queue_all_reset(hcc, locked);
    hcc_rx_queue_all_reset(hcc, locked);
 }
EXPORT_SYMBOL(hcc_queues_reset);

STATIC inline void hcc_rx_queue_reset(struct hcc_handler* hcc, hcc_queue_type type)
{
    uint32 freed_total_count = 0;
    struct sk_buff *skb;
    HWIFI_BUG_ON(type >= HCC_QUEUE_COUNT);
    while(!!(skb = skb_dequeue(&hcc->hcc_queues[HCC_RX].queues[type].data_queue)))
    {
        dev_kfree_skb(skb);
        freed_total_count++;
    }
    if(freed_total_count)
    {
        spin_lock_bh(&hcc->hcc_queues[HCC_RX].flow_ctrl_lock);
        HWIFI_INFO("Sub flow ctrl queue  %s %u skbs,wait count:%d",
                    hcc_get_queue_str(type), freed_total_count,
                    atomic_read(&hcc->hcc_queues[HCC_RX].wait_count));
        hcc_flow_ctrl_len_sub(hcc, HCC_RX, freed_total_count);
        spin_unlock_bh(&hcc->hcc_queues[HCC_RX].flow_ctrl_lock);
    }
    hcc_trans_skb_dequeue_handle(hcc, type, HCC_RX, freed_total_count);
}

void hcc_rx_queue_all_reset(struct hcc_handler* hcc, uint8 locked)
{
    int i;
    if(locked)
    {
        hsdio_rx_transfer_lock(bus_to_sdio(hcc->io_dev));
    }
    hcc_rx_split_lock(hcc);
    /*destory all hcc rx queues*/
    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        spin_lock_bh(&hcc->hcc_queues[HCC_RX].queues[i].data_queue_lock);
        hcc_rx_queue_reset(hcc, i);
        spin_unlock_bh(&hcc->hcc_queues[HCC_RX].queues[i].data_queue_lock);
    }
    hcc_rx_split_unlock(hcc);
    if(locked)
    {
        hsdio_rx_transfer_unlock(bus_to_sdio(hcc->io_dev));
    }
}
uint32 check_txqueue_status(struct hcc_handler* hcc)
{
    uint32  queue_len = 0;
    uint32  i;

    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        queue_len += skb_queue_len(&hcc->hcc_queues[HCC_TX].queues[i].data_queue);
    }

    return queue_len;
}


#ifdef WLAN_PERFORM_OPT
int32 hcc_build_header(struct hcc_handler *hcc, uint8 mtype, uint8 stype, struct sk_buff *skb)
#else
STATIC int32 hcc_build_header(struct hcc_handler *hcc, uint8 mtype, uint8 stype, struct sk_buff *skb)
#endif
{
    struct hcc_header      *header;
    uint32                  payload_len;
    uint32                  header_room;

    if (unlikely((NULL == hcc) || (mtype >= HCC_TYPE_MAX) || (NULL == skb)))
    {
        HWIFI_WARNING("Error param: hcc = 0x%p, skb = 0x%p, main type = %u", hcc, skb, mtype);
        return -EFAIL;
    }

    payload_len = skb->len;

    /* hcc header room and 4 bytes align */
    header_room = sizeof(*header);

    /* TBD: not allow skb copy */
    if ((skb_headroom(skb) < header_room)&&
        (SUCC != pskb_expand_head(skb, header_room-skb_headroom(skb), 0, GFP_ATOMIC)))
    {
        HWIFI_WARNING("There is no enough header room( %u )in skb", header_room);
        return -EFAIL;
    }
    /* enclose hcc header */
    header = (struct hcc_header*)__skb_push(skb, header_room);
    header->type    = HCC_MK_TYPE(mtype, stype);
    header->pad     = 0;
    header->more    = 0;
    header->option  = 0;
    header->pay_len = payload_len;
    header->seq     = ((uint32)(atomic_inc_return(&hcc->tx_seq))) & 0xFF;
    if(HWIFI_WARN(
        skb->len > (SINGLE_PACKET_MAX_SIZE - HCC_ASSEMBLE_INFO_SIZE),
        "skb len too long %u > %u",
        skb->len,
        (SINGLE_PACKET_MAX_SIZE - HCC_ASSEMBLE_INFO_SIZE)))
    {
        HWIFI_ERROR("skb data too large mtype:%s stype:%u", hcc_get_action_str(mtype), stype);
    }
    return SUCC;
}
STATIC inline void hcc_skb_tx_queue_tail(struct hcc_handler *hcc, uint32 queue_ind, struct sk_buff *skb)
{
    /* enqueue data skb to list tail */
    struct sk_buff_head*         data_queue =
            &hcc->hcc_queues[HCC_TX].queues[queue_ind].data_queue;
    spinlock_t*                  data_queue_lock =
            &hcc->hcc_queues[HCC_TX].queues[queue_ind].data_queue_lock;
    spin_lock_bh(data_queue_lock);
#ifdef HCC_DEBUG
    atomic_inc_return(&hcc->debug.hcc_tx_count);
#endif
    HWIFI_PR_DEBUG("hcc_tx==>queue:%s, seq:%u, queue len:%u", hcc_get_queue_str(queue_ind),
                                           ((struct hcc_header*)skb->data)->seq,
                                           skb_queue_len(data_queue));
    /*Check whether the list is too long,
    this should never happened because we have flow ctrl when tx*/
    if(unlikely(skb_queue_len(data_queue) > HCC_TX_HI_WATERLINE*2))
    {
        if(printk_ratelimit())
            HWIFI_WARNING("Tx:%s queue too long qlen: %u \n",
                            hcc_get_queue_str(queue_ind), skb_queue_len(data_queue));
    }
    hcc_debug_peak_inc(hcc, HCC_TX, queue_ind, skb_queue_len(data_queue));
    skb_queue_tail(data_queue, skb);
    spin_unlock_bh(data_queue_lock);
    return;
}


STATIC inline void hcc_skb_rx_queue_tail(struct hcc_handler *hcc, uint32 queue_ind, struct sk_buff *skb)
{
    /* enqueue data skb to list tail */
    spinlock_t * lock = &hcc->hcc_queues[HCC_RX].queues[queue_ind].data_queue_lock;
    struct sk_buff_head * head = &hcc->hcc_queues[HCC_RX].queues[queue_ind].data_queue;

    spin_lock_bh(lock);
    hcc_debug_peak_inc(hcc, HCC_RX, queue_ind, skb_queue_len(head));
    skb_queue_tail(head, skb);
    spin_unlock_bh(lock);
    return;
}
/*
 * hcc type to queue mapping
 * HCC_TYPE_DATA      0 -> 2    DATA_LO_QUEUE
 * HCC_TYPE_HCC_CTRL  1 -> 0    CTRL_QUEUE
 *                    2 -> 2    DATA_LO_QUEUE
 * HCC_TYPE_WIFI_CTRL 3 -> 0    CTRL_QUEUE
                      4 -> 2    DATA_LO_QUEUE
                      5 -> 2    DATA_LO_QUEUE
 * HCC_TYPE_OAM       6 -> 0    CTRL_QUEUE
 * HCC_TYPE_TEST      7 -> 2    DATA_LO_QUEUE
 */
static const uint8 mtype_to_queue_map[HCC_TYPE_MAX] = {
    [HCC_TYPE_DATA] = DATA_LO_QUEUE,
    [HCC_TYPE_HCC_CTRL] = CTRL_QUEUE,
    [HCC_TYPE_BEATHEART] =  DATA_LO_QUEUE,
    [HCC_TYPE_WIFI_CTRL] = CTRL_QUEUE,
    [4] = DATA_LO_QUEUE,
    [5] = DATA_LO_QUEUE,
    [HCC_TYPE_OAM] = CTRL_QUEUE,
    [HCC_TYPE_TEST] = DATA_LO_QUEUE };

STATIC uint32 hcc_get_rx_pkt_queue_ind(struct hcc_handler* hcc, uint8 mtype, struct sk_buff *skb)
{
    /*the skb include hcc header*/
    hcc_queue_type ret;
    uint32  ind;
    uint16  hcc_offset;
    uint16  hcc_assem_len;
    struct hcc_header       *header;
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!skb);
    header = (struct hcc_header *)skb->data;
    HWIFI_WARN(mtype >= HCC_TYPE_MAX, "error mtype[%u]", mtype);
    mtype = MIN((uint8)(HCC_TYPE_MAX-1), mtype);
    ind = mtype_to_queue_map[mtype];

    hcc_assem_len = HCC_ASSEMBLE_INFO_SIZE;

    hcc_offset = sizeof(struct hcc_header) +  header->pad + (header->option ? hcc_assem_len : 0);

    if(unlikely(mtype >= HCC_TYPE_MAX))
    {
        HWIFI_WARNING("unkown rx mtype: %u", mtype);
        /*unkown master type add to lo queue*/
        return DATA_LO_QUEUE;
    }

    if (HCC_TYPE_DATA == mtype)
    {

        /*filter the HCC_TYPE_DATA to TCP DATA QUEUE & TCP ACK QUEUE*/
        if(hcc->sys_tcp_rx_ack_opt_enable)
        {
        #ifdef WMM_OPT_FOR_AUTH
            ret = hcc_filter_spec_type(skb->data + hcc_offset, hcc, HCC_RX);
        #else
            ret = hcc_filter_tcp_type(skb->data + hcc_offset);
        #endif
            if(ret != HCC_QUEUE_COUNT)
            {
                HWIFI_PR_DEBUG("tcp skb rx to %s ,len %d", hcc_get_queue_str(ret), skb->len);
                ind = ret;
            }
        }

    }
    HWIFI_PR_DEBUG("get rx package queue mtype[%u] ==>ind[%u][%s]", mtype,ind, hcc_get_queue_str(ind));
    HWIFI_WARN_ON(ind >= HCC_QUEUE_COUNT);
    return ind;
}

uint32 hcc_get_tx_net_queue_mapping(struct hcc_handler* hcc, struct sk_buff *skb)
{
    hcc_queue_type ret = DATA_LO_QUEUE;
    HWIFI_BUG_ON(!hcc);
    if(hcc->sys_tcp_tx_ack_opt_enable)
    {
#ifdef WMM_OPT_FOR_AUTH
        ret = hcc_filter_spec_type(skb->data, hcc, HCC_TX);
#else
        ret = hcc_filter_tcp_type(skb->data);
#endif
    }
    return ret;
}

STATIC uint32 hcc_get_tx_pkt_queue_ind(struct hcc_handler* hcc, uint8 mtype, struct sk_buff *skb)
{
    uint32  ind;

    uint16  queue;

    HWIFI_WARN(mtype >= HCC_TYPE_MAX, "error mtype[%u]", mtype);
    mtype = MIN((uint8)(HCC_TYPE_MAX-1), mtype);

    ind = mtype_to_queue_map[mtype];

    if (HCC_TYPE_DATA == mtype)
    {

        queue = skb_get_queue_mapping(skb);

        HWIFI_DEBUG("Packet from net queue:%d", queue);
        if(likely(queue <  HCC_QUEUE_COUNT))
        {
            ind = queue;
        }
        else
        {
            WARN_ON_ONCE(1);
            HWIFI_INFO("unexcept queue type:%u", queue);
            ind = DATA_LO_QUEUE;
        }

    }
    HWIFI_PR_DEBUG("get tx package queue mtype[%u] ==>ind[%u][%s]", mtype,ind,hcc_get_queue_str(ind));
    return ind;
}


/*
 * Prototype    : hcc_tx
 * Description  : enclose hcc head and send
 * Input        : struct hcc_handler *hcc
                  char main_type
                  char sub_type
                  struct sk_buff *skb
 * Output       : None
 * Return Value : 0 -succ, -1 failed,then the caller should free skb
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/7
 *     Author       :
 *     Modification : Created function
 *
 */
int hcc_tx(struct hcc_handler *hcc, uint8 mtype, uint8 stype, struct sk_buff *skb)
{
    int ret = SUCC;
    uint32      queue_ind;
    /*If hcc transfer channel stopped return failed*/
    if(unlikely(FALSE == hcc_tx_chan_is_vaild(hcc)))
    {
        HWIFI_INFO("hcc tx chan is closed");
        return -EFAIL;
    }
    mtype &= ((1 << MAIN_TYPE_BITS) - 1);
    queue_ind = hcc_get_tx_pkt_queue_ind(hcc, mtype, skb);

#ifdef WLAN_PERFORM_OPT
    /*清零cb*/
    memset (&(skb->cb), 0, sizeof(skb->cb));
#endif

/* Start of zhangwei 64406 2013-03-27 B295 增加预编译宏 */
#ifdef WLAN_PERFORM_DEBUG
    if (HCC_TYPE_DATA == mtype)
    {
        if ((g_perform_cfg_info->trigger_mode) && ((Hi1101_TX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
        {
            hwifi_perform_tx_network_statistic(skb->len);
        }

        /* statistic performance at sdio isr, make sdio bypass */
        if(g_perform_cfg_info->trigger_mode && g_perform_cfg_info->network_bypass && ((Hi1101_TX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
        {
            hwifi_perform_tx_network_bypass(skb);
            return SUCC;
        }
    }
#endif
    /* End of zhangwei 64406 2013-03-27 B295 增加预编译宏 */

     /*
     * caculate net packet num for flow control,
     * when pkt count > up-waterline, close flow
     * only count of DATA_LO_QUEUE
     */

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
    hwifi_save_flow_info(skb, hcc->hi110x_dev->cfg);
#endif

#ifdef WLAN_PERFORM_OPT

    /*如果是高优先级数据，改写subtype，供dev做特殊处理*/
    if (DATA_HI_QUEUE == queue_ind)
    {
        stype = HCC_TX_HI_SUBTYPE;
    }
#endif

    if (unlikely(SUCC != hcc_build_header(hcc, mtype, stype, skb)))
    {
        HWIFI_WARNING("Failed to build hcc header!");
        return -EFAIL;
    }

    /*move tx tcp filter to hcc_trans_process*/

    /*such as oam transfer, sdio test pkt etc.*/
    if(!in_interrupt())
    {
        while(hcc->hcc_queues[HCC_TX].queues[queue_ind].flow_ctrl.enable &&
              hcc_queue_flow_ctrl_len(hcc, queue_ind, HCC_TX)
              > hcc->hcc_queues[HCC_TX].queues[queue_ind].flow_ctrl.hi_waterline)
        {
            ret = wait_event_interruptible_timeout(hcc->wait_queue,
                                            hcc_queue_flow_ctrl_len(hcc,queue_ind,  HCC_TX) <= hcc->hcc_queues[HCC_TX].queues[queue_ind].flow_ctrl.lo_waterline,
                                            60*HZ);
            if(0 == ret)
            {
                HWIFI_ERROR("hcc flow control wait event timeout! too much time locked");
                hcc_transfer_lock(hcc);
                hwifi_union_log_dump(KERN_DEBUG);
                hcc_transfer_unlock(hcc);
            } else if(-ERESTARTSYS == ret)
            {
                HWIFI_WARNING("wifi task %s was interrupted by a signal", current->comm);
                return -EFAIL;
            }
        }
    }
    /*If we close the hcc channel , we expect the channel close immediately,
        so we must add the tx skb wake lock, it's very important for the
        netdev stop function.*/
    spin_lock_bh(&hcc->tx_skb_wake_lock);
    /*If hcc transfer channel stopped return failed*/
    if(unlikely(FALSE == hcc_tx_chan_is_vaild(hcc)))
    {
        spin_unlock_bh(&hcc->tx_skb_wake_lock);
        HWIFI_INFO("hcc tx chan is closed.");
        return -EFAIL;
    }

    hcc_queues_flow_ctrl_handle(hcc,queue_ind, HCC_TX, FALSE, 1);
    mtype = MIN(mtype, (uint8)HCC_TYPE_UNKOWN);
    hcc->hcc_queues[HCC_TX].hcc_action_count[mtype]++;

    hcc_queues_len_add(hcc,HCC_TX, 1);
    /*get wake lock before insert skb into queue.*/
    hi110x_wake_lock(hcc->hi110x_dev->pm_info);
    hcc_skb_tx_queue_tail(hcc, queue_ind, skb);

    hcc_sched_transfer(hcc);
    spin_unlock_bh(&hcc->tx_skb_wake_lock);
    return SUCC;
}


int32 hcc_dispose_pkts(struct hcc_handler *hcc, uint8 *buf, uint32 len)
{
    uint32              idx = 0;
    uint32              hcc_len;
    int32               ret;
    struct hcc_header  *header;
    struct sk_buff     *skb;

    while (idx < len)
    {
        header = (struct hcc_header *)(buf + idx);

        hcc_len = sizeof(*header) + header->pad + header->pay_len;
        if (header->option)
        {
            hcc_len += 2; /* next pkt len */
        }
        hcc_len = ALIGN(hcc_len, HCC_PACKET_ALIGN_LEN);

        /* calibration */
        if (idx + hcc_len > len)
        {
            HWIFI_WARNING("Invalid data len excced total len(%u > %u)", (idx + hcc_len), len);
            break;
        }

        skb = dev_alloc_skb(hcc_len);
        if (NULL == skb)
        {
            HWIFI_WARNING("Failed to alloc skb!");
            return -EFAIL;
        }

        /* copy pkt from buffer to skb */
        memcpy(skb_put(skb, hcc_len), header, hcc_len);
        idx += hcc_len;

        ret = hcc_rx(hcc, skb);
        if (ret != SUCC)
        {
            HWIFI_WARNING("Failed to hcc rx data!");
        }
        if (!header->more)
        {
            break;
        }

    }

    return SUCC;
}
int hcc_rx(struct hcc_handler *hcc, struct sk_buff *skb)
{
    struct hcc_header       *header;
    struct hcc_rx_action    *rx_action;
    uint8                    mtype;
    uint8                    stype;
    int32                    ret;

    HWIFI_ASSERT(NULL != hcc);
    HWIFI_ASSERT(NULL != skb);

    if (unlikely(skb->len < sizeof(*header)))
    {
        HWIFI_ERROR("Invalid too short skb->len:%d",skb->len);
        dev_kfree_skb_any(skb);
        return -EFAIL;
    }

    /* for performance, only dispatch */
    header = (struct hcc_header *)skb->data;

    HWIFI_DEBUG("type=0x%x, seq=%d, more=%d ,option=%d, pad=%d, payload_len=%d",
        header->type, header->seq, header->more,
        header->option, header->pad, header->pay_len);

#if 0
    {
        uint8 rx_seq = ((uint32)atomic_read(&hcc->rx_seq)) & 0xFF;
        if (rx_seq != header->seq)
        {
            /* TBD drop pkt caculate */
            HWIFI_WARNING("hcc occur packet losing.seq:%u, expect seq:%u", header->seq, rx_seq);
            atomic_set(&hcc->rx_seq, header->seq+1);
        }
        else
        {
             atomic_inc(&hcc->rx_seq);
        }
    }
#endif

    mtype       = HCC_MAIN_TYPE(header->type);
    stype       = HCC_SUB_TYPE(header->type);
    rx_action   = &hcc->rx_actions[mtype];

    if (unlikely(NULL == rx_action->rx_cb))
    {
        HWIFI_WARNING("rx_cb is not ready");
        dev_kfree_skb_any(skb);
        return -EFAIL;
    }

    /* remove hcc header */
    skb_pull(skb, (sizeof(struct hcc_header) +  header->pad + (header->option ? HCC_ASSEMBLE_INFO_SIZE : 0)));

    /* get the real pkt len */
    skb_trim(skb, header->pay_len);

    /* process relative rx handle by main/sub type */
    mutex_lock(&rx_action->mutex_lock);
    ret = rx_action->rx_cb(mtype, stype, skb, rx_action->context);
    if (unlikely(ret < 0))
    {
        HWIFI_WARNING("Failed to process rx %s pkt![main/sub(%u/%u)]",
                    hcc_get_action_str(mtype)
                    ,mtype, stype);
        mutex_unlock(&rx_action->mutex_lock);
        return -EFAIL;
    }
    HWIFI_DEBUG("Sucuess to process rx %s package", hcc_get_action_str(mtype));
    mtype = MIN(mtype, (uint8)HCC_TYPE_UNKOWN);
    hcc->hcc_queues[HCC_RX].hcc_action_count[mtype]++;
    mutex_unlock(&rx_action->mutex_lock);

    return SUCC;
}
STATIC inline int32 expand_assemble_info_space(struct sk_buff* skb)
{
    struct hcc_header       hdr;
    struct hcc_header      *new_hdr;
    uint8                  *assem_info;

    if (!((struct hcc_header *)skb->data)->option) /* already has assemble_info */
    {
        if (HWIFI_WARN((skb_headroom(skb) < HCC_ASSEMBLE_INFO_SIZE),
                        "there is no space for assemble info expanding(%d)",
                        skb_headroom(skb)))
        {
            return -EFAIL;
        }

        hdr      = *((struct hcc_header *)skb->data);
        new_hdr  = (struct hcc_header *)(__skb_push(skb, HCC_ASSEMBLE_INFO_SIZE));
        *new_hdr = hdr;
        new_hdr->option = 1;
    }
    else
    {
        new_hdr = (struct hcc_header *)skb->data;
    }

    assem_info = (uint8*)&new_hdr[1];
    memset(assem_info, 0, HCC_ASSEMBLE_INFO_SIZE);

    return SUCC;
}

int32 hcc_rx_add_to_queues(struct hcc_handler* hcc, struct sk_buff *skb)
{
    uint8   mtype;
    uint32  ind;
    struct hcc_header       *header;
    header = (struct hcc_header *)skb->data;

    mtype = HCC_MAIN_TYPE(header->type);
    ind = hcc_get_rx_pkt_queue_ind(hcc, mtype, skb);
    hcc_skb_rx_queue_tail(hcc, ind, skb);

    return 0;
}
STATIC void hcc_rx_data_separate_into_queues(struct hcc_handler* hcc, struct sk_buff_head *head)
{
    int count = 0;
    uint8  flag = 0;
    struct sk_buff *skb;
    struct hcc_header  *header;

    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!head);
    HWIFI_DEBUG("rx separte skb list len==>%d", skb_queue_len(head));

    while(!skb_queue_empty(head))
    {
        /*list from head, needn't to add lock*/
        skb = skb_dequeue(head);
        header = (struct hcc_header  *)skb->data;
        if(!header->more)
        {
            /*the end of the list*/
            flag = 1;
            HWIFI_WARN(!skb_queue_empty(head), "This skb is not last one, hcc header error!");
        }
        /*get wakelock before add skb to rx queue, and unlock after dequeue*/
        hi110x_wake_lock(hcc->hi110x_dev->pm_info);
        hcc_rx_add_to_queues(hcc, skb);
        count++;
    }
    schedule();
    hcc_sched_transfer(hcc);
    HWIFI_WARN(!flag, "did not find the last pkt!");
}
 STATIC int skb_align_to_sg_format(struct sk_buff *skb)
 {
    int ret = SUCC;
    unsigned long              need_pad;
    HWIFI_BUG_ON(!skb);
    need_pad = ((unsigned long)skb->data) & 0x03;
    /*data buffer must align to 4 bytes, mmc host control request.*/
    if(unlikely(need_pad))
    {
        /*2 bytes align*/
        if((need_pad & 0x01)|| (((need_pad & 0x10) + skb->len) > SINGLE_PACKET_MAX_SIZE))
        {
            /*if 2 bytes not align or cross the 1536 bytes, no need expand skb->len,
            we must copy skb buffer when send the skb buffer.*/
            HWIFI_DEBUG("[need pad 0x%lx][skb len: %u]", need_pad, skb->len);
        }
        else
        {
            /*2 bytes align need  pad*/
            struct hcc_header  *old_hdr;
            struct hcc_header  *new_hdr;
            uint32              sz = (uint32)sizeof(struct hcc_header);

            old_hdr = (struct hcc_header *)skb->data;
            new_hdr = (struct hcc_header*)(__skb_push(skb, need_pad));
            if (old_hdr->option)
            {
                sz += HCC_ASSEMBLE_INFO_SIZE;
            }
            memmove(new_hdr, old_hdr, sz);
            new_hdr->pad = need_pad;
        }

    }

    /*skb buffer len must 8 bytes align, we should do this after expand skb->len*/
    if(likely((skb->len < ALIGN(skb->len, HCC_PACKET_ALIGN_LEN))))
    {
        uint32 need_align = ALIGN(skb->len, HCC_PACKET_ALIGN_LEN) - skb->len;
        HWIFI_DEBUG("[skb raw len: %d][need_align:%u][tail_len:%d]\n", skb->len,need_align, skb_tailroom(skb));
        if(need_align > skb_tailroom(skb))
        {
            HWIFI_DEBUG("expand before [skb raw len: %d][need_align:%u][tail_len:%d]\n", skb->len,need_align, skb_tailroom(skb));
            /*expand the tailroom*/
            ret = pskb_expand_head(skb, 0, need_align, GFP_ATOMIC);
            /*if expand failed, we always put skb tailroom  to make sure
            the hcc channel ok, I feel so sad...*/
            if(HWIFI_WARN_ON(ret))
            {
                printk(KERN_DEBUG"tailroom not enough,and expand failed !check the system memory!");
                print_hex_dump_bytes("skb: ", DUMP_PREFIX_ADDRESS, skb->data, skb->len);
            }

            HWIFI_DEBUG("Expand after [skb raw len: %d][need_align:%u][tail_len:%d]\n", skb->len,need_align, skb_tailroom(skb));
        }
        __skb_put(skb,need_align);
    }
    return ret;
 }


STATIC uint32 fill_packet_assemble_info_sg(struct hcc_handler *hcc, struct sk_buff *skb, uint32 can_expand, struct sk_buff_head *queue, uint32 next_assemble_cnt)
{
    struct sk_buff         *next_skb;
    struct hcc_header      *header;
    uint8                  *assem_info;
    uint32                  idx;

    if (next_assemble_cnt <= 1)
    {
        /*
         * that is say it not need assembly
         * when assemble_info_cnt=1
         */
        return 0;
    }

    next_skb = queue->next;

    if (can_expand)
    {
        if (unlikely(expand_assemble_info_space(skb) != SUCC))
        {
            HWIFI_WARNING("Failed to expand assemble info space!");
            return 0;
        }
    }
    else
    {
        if (!((struct hcc_header *)skb->data)->option)
        {
            HWIFI_WARNING("Failed to expand assemble info space!");
            if(HWIFI_DEBUG_CONDTION())
                print_hex_dump_bytes("Tx fill skb: ", DUMP_PREFIX_ADDRESS, skb->data, skb->len);
            return 0;
        }
    }

    memset(hcc->assem_info, 0, HCC_ASSEMBLE_INFO_SIZE);
    /*
     * because expand_assemble_info_space may change header position
     * so the following 2 lines must be set after expand_assemble_info_space
     */
    header     = (struct hcc_header *)skb->data;
    assem_info = (uint8*)&header[1];
    for(idx = 0; idx < next_assemble_cnt; idx++, next_skb = next_skb->next)
    {
        /*
         * last packet need expand for next assemble info
         * if no space for next pkg or next pkg's assemble-info expand
         * the current pkt should be regarded as the last packet.
         *
         */
        HWIFI_PR_DEBUG("fill assemble indx[%u] next_skb[%p] len[%u]",
                        idx, next_skb,next_skb->len);
        /*FIXME? Check the condition*/
        if (((next_assemble_cnt - 1) == idx)
            || (skb_queue_is_last(queue,next_skb)))
        {
            /*FIXEME? TBD 异常处理，分配聚合信息空间失败下一包应该单包发送ext_assemble_cnt=0
            idx = 0, option=0*/
            expand_assemble_info_space(next_skb);
           /* break in next loop */
        }
        /*FIXEME? This should never failed!*/
        skb_align_to_sg_format(next_skb);
        hcc->assem_info[idx] = (ALIGN(next_skb->len, HCC_PACKET_ALIGN_LEN)) >> 3;
        HWIFI_PR_DEBUG("[idx:%d]write asseminfo:[%u]", idx, hcc->assem_info[idx] << 3);
        assem_info[idx]      = hcc->assem_info[idx];
    }
    HWIFI_PR_DEBUG("ASSEMBLE [%u]", idx);
    return idx;
}



STATIC int32 __send_single_packet_sg(struct hcc_handler* hcc, struct sk_buff *skb)
{
    int32               ret;
    unsigned long              need_pad;
    /*single pkt transfer, use 2 sg items 1 sg for aligned*/
    struct scatterlist sg[2];

    /* because fill_packet_assemble_info  can change skb len ,
       so this after fill_packet_assemble_info */
    if (HWIFI_WARN_ON(skb->len > SINGLE_PACKET_MAX_SIZE))
    {
        HWIFI_WARNING("Invalid too long skb packet len = %d", skb->len);
        hcc->assembled_cnt = 0;
        return -EFAIL;
    }

    sg_init_table(sg, 2);
    /* sdio host controller need 4 bytes aligned */
    need_pad = ((unsigned long)skb->data) & 0x03;
    if (unlikely(need_pad))
    {
        /*
           net data address in Device need 2-byte align,
           so do copy when 1 or 3 padding.
           skb->len < SINGLE_PACKET_MAX_SIZE, but after padding ,
           len can large than SINGLE_PACKET_MAX_SIZE, so copy too.
           other, just copy hcc header.
         */
        if ((need_pad & 0x01) /* TBD: control data need padding? */
            || ((need_pad + skb->len) > SINGLE_PACKET_MAX_SIZE))
        {
            if(unlikely(skb->len > hcc->tx_buf_size))
            {
                HWIFI_WARNING("Invalid skb len(%d) larger than tx buffer size(%d)",
                    skb->len, hcc->tx_buf_size);
                hcc->assembled_cnt = 0;
                return -EFAIL;
            }
            /* create new skb to make assemble description header */
            memcpy(hcc->tx_buf, skb->data, ALIGN(skb->len, HCC_PACKET_ALIGN_LEN));
            sg_set_buf(&sg[0], hcc->tx_buf, ALIGN(skb->len, HCC_PACKET_ALIGN_LEN));
        }
        else
        {
            struct hcc_header  *old_hdr;
            struct hcc_header  *new_hdr;
            uint32              sz = (uint32)sizeof(struct hcc_header);

            old_hdr = (struct hcc_header *)skb->data;
            new_hdr = (struct hcc_header*)(__skb_push(skb, need_pad));
            if (old_hdr->option)
            {
                sz += HCC_ASSEMBLE_INFO_SIZE;
            }
            memmove(new_hdr, old_hdr, sz);
            new_hdr->pad = need_pad;
            sg_set_buf(&sg[0], skb->data, ALIGN(skb->len, HCC_PACKET_ALIGN_LEN));
        }
    }
    else
    {
        /*TBD*/
        sg_set_buf(&sg[0], skb->data, ALIGN(skb->len, HCC_PACKET_ALIGN_LEN));
    }

    /* send single pkt */
    ret = bus_transfer_sg(hcc->io_dev, sg, 1, ALIGN(skb->len, HCC_PACKET_ALIGN_LEN));
    if (unlikely(ret != SUCC))
    {
        HWIFI_ERROR("Failed to send single pkt! [skb->len:%d][ret = %d]", skb->len, ret);
        hcc->assembled_cnt = 0;
        if(HWIFI_DEBUG_CONDTION())
            dump_sg_buffer(KERN_EMERG,sg, 1);
        return -EFAIL;
    }
    assem_debug_tx_count_incr(hcc, 0);
    return SUCC;
}


STATIC  int hcc_rx_pkt_list_verfiy(struct hi110x_sdio *hi_sdio,
                                    struct sk_buff_head *pkt_sk_list_head)
{
    int i = 0;
    int ret  = SUCC;
    struct sk_buff   *skb;
    struct hcc_header  *header;
    HWIFI_BUG_ON(!hi_sdio);
    skb_queue_walk(pkt_sk_list_head, skb)
    {
        header = (struct hcc_header *)skb->data;
        if(skb_queue_is_last(pkt_sk_list_head, skb))
        {
            /*more flag should be 0,option flag should be 0*/
            if(header->more)
            {
                HWIFI_ERROR("Hcc header:[%d] more flag error, last pkt, more should be 0!", i);
                ret = -EFAIL;
                goto failed;
            }
        }
        else
        {
            /*more flag should be 1*/
            if(!header->more)
            {
                HWIFI_ERROR("Hcc header:[%d] more flag error, not last pkt, more should be 1!", i);
                ret = -EFAIL;
                goto failed;
            }
            if(header->option)
            {
                HWIFI_ERROR("Hcc header:[%d] option flag error, not last pkt, option should be 0!", i);
                ret = -EFAIL;
                goto failed;
            }
        }

        /*pay len check*/
        if(header->pay_len + sizeof(struct hcc_header)  > skb->len)
        {
           HWIFI_ERROR("Hcc header:[%d] pay len error,[pay_len:%d]+ [hcc header:%zu] > [skb_len:%d]!!",
                        i, header->pay_len, sizeof(struct hcc_header), skb->len);
           ret = -EFAIL;
           goto failed;
        }
        i++;
    }
    return ret;
failed:
    return ret;
}
int hcc_get_nextpkt_skb_info(struct hi110x_sdio *hi_sdio,
                                    struct sk_buff_head *pkt_sk_list_head)
{
    int idx = 0;
    int alloc_failed = 0;
    struct sk_buff         *skb;
    struct hcc_header  *header;
    struct sk_buff_head pkt_head_t;
    rx_skb_node* rx_node;
    int err_flag = SUCC;
    struct hcc_handler* hcc = hi_sdio->hi110x_dev->hcc;

    HWIFI_BUG_ON(skb_queue_empty(pkt_sk_list_head));

    /*detail check for rx skb list under debug mode!*/
    if(HWIFI_DEBUG_CONDTION())
    {
        err_flag = hcc_rx_pkt_list_verfiy(hi_sdio, pkt_sk_list_head);
    }

    skb_queue_head_init(&pkt_head_t);
    /*The sdio data just receviced*/
    /*fetch last skb.*/
    skb = pkt_sk_list_head->prev;
    header = (struct hcc_header *)skb->data;
    HWIFI_DEBUG("Sdio Rx last pkt hcc info:type=0x%x, seq=%d, more=%d ,option=%d, pad=%d, payload_len=%d skb_len: %d",
    header->type, header->seq, header->more,
    header->option, header->pad, header->pay_len, skb->len);
    if(likely(!header->more))
    {
        if(header->option)
        {
            int i;
            uint32 next_sum_len;
            int step;
            /*last skb buffer include aseem info, build next skb buffer list.*/
            /*next 16 bytes contain aseem info*/
            uint8 * assem;
retry:
            idx = 0;
            next_sum_len = 0;
            step = (HCC_ASSEMBLE_INFO_SIZE/HCC_MAX_ASSEMBLE_NUM);
            assem = (uint8*)&header[1];
            for(i=0;i < HCC_ASSEMBLE_INFO_SIZE; i += step)
            {
                uint8 len = *(assem + i);
                if(0 == len)
                {
                    /*end*/
                    break;
                }
                else
                {
                    struct sk_buff *skb_t;
                    uint32 len_t = (uint32 )(len << 3);
                    /*#warning debug,simulator memory leak
                    dev_alloc_skb(len_t);*/
                    if(unlikely(alloc_failed))
                    {
                        /*had alloc failed use reserved mem*/
                        skb_t = hisdio_reserve_skb_alloc(hi_sdio, len_t);
                        HWIFI_BUG_ON(!skb_t);
                    }
                    else
                    {
                        /*alloc normally.*/
                        skb_t =  dev_alloc_skb(len_t);
                        if(unlikely(!skb_t))
                        {
                            alloc_failed = 1;
                            skb_queue_purge(&pkt_head_t);
                            skb_queue_head_init(&pkt_head_t);
                            set_bit(HSDIO_RX_ERROR_NEXT_PKT_BUSY, &hi_sdio->rx_skb_err);
                            if(printk_ratelimit())
                                HWIFI_WARNING("rx hcc skb alloc failed![len:%u]", len_t);
                            goto retry;
                        }
                        else
                        {
                            clear_bit(HSDIO_RX_ERROR_NEXT_PKT_BUSY, &hi_sdio->rx_skb_err);
                        }
                    }
                    /*(important!)assem pkt len must 32 bytes aligned
                    (host controller idma read request).*/
                    HWIFI_WARN(len_t & 0x1F, "i:%d assem pkts len must 32 bytes aligned, %u is invaild",
                                i, len_t);
                    skb_put(skb_t, len_t);
                    next_sum_len += len_t;
                    __skb_queue_tail(&pkt_head_t, skb_t);
                    idx++;
                }
            }

            hi_sdio->next_pkt_len = next_sum_len;
        }
        else
        {
            /*receive single pkt next time.*/
            hi_sdio->next_pkt_len = 0;
        }
    }
    else
    {
        HWIFI_ERROR("Sdio Rx assem info error! Last skb's more flag should be zero");
        err_flag = -EFAIL;
    }

    if(unlikely(SUCC != err_flag))
    {
        dump_skb_buffer(KERN_DEBUG, pkt_sk_list_head);
        hi_sdio->next_pkt_len = 0;
        skb_queue_purge(&pkt_head_t);
    }

    if(HWIFI_DEBUG_CONDTION())
    {
        if(SUCC == err_flag)
        {
            if(hi_sdio->next_pkt_len){
                /*dump assem info*/
                printk(KERN_DEBUG"Sdio Rx assem info,invaild util got zero\n");
                print_hex_dump_bytes("Last Skb's Assem Info: ", DUMP_PREFIX_ADDRESS,
                                    (void*)&header[1], HCC_ASSEMBLE_INFO_SIZE);
            }else{
                HWIFI_DEBUG("Sdio Rx Receive Single Package Next Interrupt.");
            }
        }
    }

    /*If this rx transfer used reserved skb mem, we should drop it.*/
    if(unlikely(test_and_clear_bit(HSDIO_RX_ERROR_TRANS_BUSY, &hi_sdio->rx_skb_err)))
    {
        HWIFI_INFO("transfer pkt list used reserved mem, so drop it this time [len:%u]",
                    pkt_sk_list_head->qlen);
        hcc_drop_skb_sg(hi_sdio, pkt_sk_list_head);
        HWIFI_WARN_ON(!skb_queue_empty(pkt_sk_list_head));
        skb_queue_splice(&pkt_head_t, pkt_sk_list_head);
        /*alloc system mem failed, take a rest for dispose thread*/
        msleep(jiffies_to_msecs(HWIFI_SCHEDULE_PERIOD));
        return SUCC;
    }

    assem_debug_rx_count_incr(hi_sdio->hi110x_dev->hcc, MIN(idx,HCC_MAX_ASSEMBLE_NUM));

    HWIFI_DEBUG("Data irq finished, wait for next sdio rx transmission,  request bytes:%u,skb num: %d]\n\n", hi_sdio->next_pkt_len, idx);
    if(hcc->sys_hcc_rx_opt_enable)
    {
        /*submit the receive skb list and separate into different queues*/
        hcc_flow_ctrl_len_add(hcc, HCC_RX, skb_queue_len(pkt_sk_list_head));
        /*must add count before sched the transfer thread,
           Important:we must add count first then put skb to queue, reverse when del skb.*/
        hcc_queues_len_add(hcc,HCC_RX,   skb_queue_len(pkt_sk_list_head));
        hcc_rx_data_separate_into_queues(hi_sdio->hi110x_dev->hcc, pkt_sk_list_head);
    }else{
        unsigned long flags;
        /*If not debug mode, we should use  rx dispose thread.*/
        if(!(rx_node = kmalloc(sizeof(rx_skb_node), GFP_ATOMIC)))
        {
            HWIFI_ERROR("Failed to alloc rx node![%zu]", sizeof(rx_skb_node));
            goto failed;
        }
        hcc_flow_ctrl_len_add(hcc, HCC_RX, skb_queue_len(pkt_sk_list_head));
        hcc_queues_len_add(hcc,HCC_RX,   skb_queue_len(pkt_sk_list_head));
        skb_queue_head_init(&rx_node->rx_head);
        skb_queue_splice_init(pkt_sk_list_head, &rx_node->rx_head);
        spin_lock_irqsave(&hi_sdio->rx_skb_pool_lock, flags);
        list_add_tail(&rx_node->list, &hi_sdio->rx_skb_pool_list);
        spin_unlock_irqrestore(&hi_sdio->rx_skb_pool_lock,flags);
    }
    HWIFI_WARN_ON(!skb_queue_empty(pkt_sk_list_head));
    skb_queue_splice(&pkt_head_t, pkt_sk_list_head);
    return SUCC;
failed:
    HWIFI_WARNING("kmalloc rx node failed, drop this rx transfer data..., mem too low..");
    skb_queue_purge(pkt_sk_list_head);
    skb_queue_head_init(pkt_sk_list_head);
    skb_queue_splice(&pkt_head_t, pkt_sk_list_head);
    return -EFAIL;
}

int32 hcc_dispose_skb_sg(struct hcc_handler *hcc, struct sk_buff_head * head)
{
    uint8  flag = 0;
    int32  ret;
    struct sk_buff *skb;
    struct hcc_header  *header;

    /*FIXME? internal call function, needn't add aseert, remove later*/
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!head);
    HWIFI_DEBUG("rx ==> %s", skb_queue_empty(head)? "no package":"have package");
    hcc_flow_ctrl_len_sub(hcc, HCC_RX, skb_queue_len(head));
    hcc_queues_len_sub(hcc,HCC_RX,  skb_queue_len(head));
    while(!skb_queue_empty(head))
    {
        skb = skb_dequeue(head);
        header = (struct hcc_header  *)skb->data;
        if(!header->more)
        {
            /*the end of the list*/
            flag = 1;
            HWIFI_WARN(!skb_queue_empty(head), "This skb is not last one, hcc header error!");
        }
        if(unlikely(ret = hcc_rx( hcc, skb)))
        {
            HWIFI_WARNING("upload pkt failed");
        }
    }
    HWIFI_WARN(!flag, "did not find the last pkt!");
    return SUCC;
}

int32 hcc_drop_skb_sg(struct hi110x_sdio *hi_sdio, struct sk_buff_head * head)
{
    struct sk_buff *skb;
    struct hcc_header  *header;
    HWIFI_BUG_ON(!hi_sdio);
    HWIFI_BUG_ON(!head);
    HWIFI_DEBUG("++drop this rx data++");
    if(HWIFI_DEBUG_CONDTION()){
        skb_queue_walk(head, skb)
        {
            header = (struct hcc_header  *)skb->data;
            HWIFI_DEBUG("dropped pkt:type=0x%x, seq=%d, more=%d ,option=%d, pad=%d, payload_len=%d",
            header->type, header->seq, header->more,
            header->option, header->pad, header->pay_len);
        }
    }
    hisdio_reserve_skb_free_list_init(hi_sdio, head);
    HWIFI_DEBUG("--drop this rx data--");
    return SUCC;
}

STATIC void hcc_trans_queue_filter(struct hcc_handler *hcc, struct sk_buff_head  *head_t,
                                    hcc_queue_type type, hcc_chan_type dir)
{
    if(hcc->hcc_queues[dir].queues[type].filter)
    {
        uint32 old_len, new_len;
        old_len = skb_queue_len(head_t);
        hcc->hcc_queues[dir].queues[type].filter(hcc, type, dir, head_t);
        new_len = skb_queue_len(head_t);
        HWIFI_DEBUG("[%s]Before filter len:%u,After filter len:%u",hcc_get_queue_str(type)
                    ,old_len, new_len);
        if(unlikely(new_len > old_len))
        {
            HWIFI_ERROR("The filter len:%u is more than before filter:%u",
                new_len, old_len);
        }else
        {
            hcc->hcc_queues[dir].queues[type].drop_count += (old_len - new_len);
        }
    }
}
STATIC int hcc_send_tx_queue_single(struct hcc_handler *hcc, hcc_queue_type type, uint32 max_len)
{
    int old_len,new_len, len_t;
    uint32 transfer_len, index;
    spinlock_t*  lock;
    struct sk_buff *skb;
    struct sk_buff_head* head;
    struct sk_buff_head  head_t;
    HWIFI_BUG_ON(!hcc);
    HWIFI_WARN_ON(!max_len);
    head = &hcc->hcc_queues[HCC_TX].queues[type].data_queue;
    lock = &hcc->hcc_queues[HCC_TX].queues[type].data_queue_lock;

    if(skb_queue_empty(head))
    {
        return 0;
    }

    skb_queue_head_init(&head_t);

    spin_lock_bh(lock);
    skb_queue_splice_tail_init(head, &head_t);
    spin_unlock_bh(lock);
    /*filter the queue*/
    new_len = old_len = skb_queue_len(&head_t);
    hcc_trans_queue_filter(hcc, &head_t, type, HCC_TX);
    new_len = skb_queue_len(&head_t);
    len_t =  old_len - new_len;

    hcc_queues_flow_ctrl_handle(hcc,type, HCC_TX, TRUE, len_t);
    hcc_trans_skb_dequeue_handle(hcc, type, HCC_TX, len_t);
    if(skb_queue_empty(&head_t))
    {
        return len_t;
    }

    if(max_len < skb_queue_len(&head_t))
    {
        transfer_len = max_len;
    } else {
        transfer_len = skb_queue_len(&head_t);
    }

    HWIFI_DEBUG("%s, queue:%s, request transfer skb num:%u, queue len:%u", hcc_send_mode_str[hcc_single_send],hcc_get_queue_str(type),
                                           transfer_len,
                                           skb_queue_len(&head_t));

    new_len = old_len = skb_queue_len(&head_t);

    for(index = 0; index < transfer_len; index++ )
    {
        if(unlikely(TRUE != hcc_tx_chan_is_vaild(hcc)))
        {
            /*If hcc channel closed, we return the skb to queue list.*/
            schedule();
            break;
        }
        if(TRUE == hcc->hcc_queues[HCC_TX].queues[type].flow_ctrl.enable)
        {
            if (g_flowctrl_flag != FLOWCTRL_ON)
            {
                HWIFI_DEBUG("flow ctrl is off, filter this tx transfer...");
                hcc->assembled_cnt = 0;
                schedule();
                /*hcc_sched_transfer(hcc, hcc_sched_restore, 1);*/
                break;
            }
        }
        skb = skb_dequeue(&head_t);
        if(unlikely(!skb))
        {
            HWIFI_ERROR("skb should not be null, request transfer len:%u, index:%u",
                        transfer_len, index);
            break;
        }
#ifdef HCC_DEBUG
        atomic_inc_return(&hcc->debug.hcc_single_count);
#endif
        HWIFI_PR_DEBUG("%s, queue:%s, seq:%u", hcc_send_mode_str[hcc_single_send],
                                               hcc_get_queue_str(type),
                                               ((struct hcc_header*)skb->data)->seq);

        if (__send_single_packet_sg(hcc, skb) != SUCC)
        {
            HWIFI_WARNING("send single packet failed");
            dev_kfree_skb(skb);
            break;
        }
        dev_kfree_skb(skb);

    }

    if(unlikely(transfer_len != index))
    {
        HWIFI_INFO( "transfer_len[%u] != index[%u] \n", transfer_len,index);
    }
    HWIFI_DEBUG("%s, queue:%s send over, still reamin:%u",
                    hcc_send_mode_str[hcc_single_send],
                    hcc_get_queue_str(type),
                    skb_queue_len(&head_t));
    new_len = skb_queue_len(&head_t);
    len_t = old_len - new_len;
    HWIFI_WARN_ON(len_t < 0);
    hcc_queues_flow_ctrl_handle(hcc,type, HCC_TX, TRUE, len_t);
    hcc_trans_skb_dequeue_handle(hcc, type, HCC_TX, len_t);

    spin_lock_bh(lock);
    skb_queue_splice_init(&head_t, head);
    spin_unlock_bh(lock);
    return index;
}
STATIC int hcc_send_tx_queue_assemble(struct hcc_handler *hcc, hcc_queue_type type, uint32 max_len)
{
    int old_len,new_len,len_t;
    int ret = 0;
    int trans_len = 0;
    uint32      pkt_cnt;
    spinlock_t*  lock;
    uint32 remain = max_len;
    uint32 next_len = 0;
    struct sk_buff_head* head;
    struct sk_buff_head  head_t;
    head = &hcc->hcc_queues[HCC_TX].queues[type].data_queue;
    lock = &hcc->hcc_queues[HCC_TX].queues[type].data_queue_lock;
    skb_queue_head_init(&head_t);
    HWIFI_DEBUG("%s to device ==> %s len:%d",
                            hcc_send_mode_str[hcc_assem_send],
                            hcc_get_queue_str(type),
                            skb_queue_len(head));
    spin_lock_bh(lock);
    skb_queue_splice_tail_init(head, &head_t);
    spin_unlock_bh(lock);
    new_len = old_len = skb_queue_len(&head_t);
    hcc_trans_queue_filter(hcc, &head_t, type, HCC_TX);
    new_len = skb_queue_len(&head_t);
    len_t = old_len - new_len;
    HWIFI_WARN(len_t < 0, "Then list[%d] is longer than before filter [%d]",
                new_len,old_len);

    hcc_queues_flow_ctrl_handle(hcc,type, HCC_TX, TRUE, len_t);
    hcc_trans_skb_dequeue_handle(hcc, type, HCC_TX, len_t);

    do
    {
        new_len = old_len = skb_queue_len(&head_t);
        if(unlikely(TRUE != hcc_tx_chan_is_vaild(hcc)))
        {
            hcc->assembled_cnt = 0;
            /*If hcc channel closed, we return the skb to queue list.*/
            schedule();
            break;
        }
        if(TRUE == hcc->hcc_queues[HCC_TX].queues[type].flow_ctrl.enable)
        {
            if (g_flowctrl_flag != FLOWCTRL_ON)
            {
                HWIFI_DEBUG("flow ctrl is off, filter this tx transfer...");
                if(hcc->sys_hcc_tx_assem_opt_enable)
                {
                    if(hcc->assembled_cnt)
                        hcc_send_assemble_reset(hcc);
                }
                else
                    hcc->assembled_cnt = 0;
                schedule();
                /*hcc_sched_transfer(hcc, hcc_sched_restore, 1);*/
                break;
            }
        }

        /*
         * if pkt send to DEV calibration is error,
         * make hcc->assembled_cnt = 0
         * restart to send sing pkt
         */
        if (ASSEMBLE_RESET_ENABLE == g_assemble_reset_flag)
        {
            hcc_send_assemble_reset(hcc);
            g_assemble_reset_flag = ASSEMBLE_RESET_DISABLE;
            HWIFI_WARNING("packet verify failed, reset assembled_cnt");
        }

        pkt_cnt = skb_queue_len(&head_t);
        HWIFI_PR_DEBUG("pkt_cnt[%u], remain[%u]", pkt_cnt, remain);
        if (!pkt_cnt || !remain)
        {
            if(unlikely(hcc->assembled_cnt))
            {
                HWIFI_WARNING("hcc->assembled_cnt[%u] should be 0,[pkt_cnt:%d][remain:%d]",
                            hcc->assembled_cnt, pkt_cnt, remain);
                HWIFI_WARNING("[%30s]:next_len:%d", hcc_get_queue_str(type), next_len);
            }
            break;
        }
        if(0 == hcc->assembled_cnt)
        {
            remain--;
            next_len = MIN(MIN((pkt_cnt-1), hcc_assemble_count), remain);
            HWIFI_DEBUG("SINGLE:next_len[%u] remain[%u]", next_len, remain);
            hcc_send_single_data_packet(hcc, &head_t,type, next_len);
            trans_len++;
        } else {
            if(likely(remain >= hcc->assembled_cnt))
            {
                remain -= hcc->assembled_cnt;
            }
            else
            {
                HWIFI_WARNING("remain[%u] < hcc->assembled_cnt[%u]??", remain, hcc->assembled_cnt);
                remain = 0;
            }
            next_len = MIN(MIN((pkt_cnt - hcc->assembled_cnt), hcc_assemble_count), remain);
            HWIFI_DEBUG("ASSEMBLE:next_len[%u] remain[%u]", next_len, remain);
            hcc_send_assemble_data_packet(hcc,&head_t,type,
                                          hcc->assembled_cnt,
                                          next_len);
            trans_len += hcc->assembled_cnt;
        }

        if (unlikely(ret != SUCC))
        {
            hcc->assembled_cnt = 0;
            break;
        }
    #ifdef WLAN_PERFORM_DEBUG
        if ((g_perform_cfg_info->trigger_mode) && ((Hi1101_TX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
        {
            g_perform_statistic->tx_sdio_sum_aggr_num++;
        }
    #endif
        new_len = skb_queue_len(&head_t);
        len_t = old_len - new_len;
        HWIFI_WARN(len_t < 0, "Then list[%d] is longer than before sent out [%d]",
                    new_len,old_len);

        hcc_queues_flow_ctrl_handle(hcc,type, HCC_TX, TRUE, len_t);
        hcc_trans_skb_dequeue_handle(hcc, type, HCC_TX, len_t);

    /*Protect The Rx Thread Process*/
        schedule();
    }while(1);
    new_len = skb_queue_len(&head_t);
    len_t = old_len - new_len;
    HWIFI_WARN(len_t < 0, "Then list[%d] is longer than before sent out [%d]",
                new_len,old_len);

    hcc_queues_flow_ctrl_handle(hcc,type, HCC_TX, TRUE, len_t);
    hcc_trans_skb_dequeue_handle(hcc, type, HCC_TX, len_t);

    spin_lock_bh(lock);
    skb_queue_splice_init(&head_t, head);
    spin_unlock_bh(lock);

#ifdef WLAN_PERFORM_DEBUG
    if ((g_perform_cfg_info->trigger_mode) && ((Hi1101_TX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
    {
        g_perform_statistic->tx_work_sched_num ++;
    }
#endif

    return trans_len;
}
int hcc_send_tx_queue(struct hcc_handler *hcc, hcc_queue_type type, hcc_send_mode send_mode)
{
    int32       ret = 0;
    uint32 max_len;
    HWIFI_BUG_ON(!hcc);
    /* pair smp_wmb in hcc_dev_flowctrl_on/off */
    if(unlikely(type >= HCC_QUEUE_COUNT))
    {
        HWIFI_WARNING("unkown hcc queue type %d", type);
        return ret;
    }

    if(!skb_queue_len(&hcc->hcc_queues[HCC_TX].queues[type].data_queue))
    {
        HWIFI_PR_DEBUG("%s: queue %s is empty", hcc_send_mode_str[send_mode],
                                             hcc_get_queue_str(type));
        return ret;
    }

    /*If the tx queue not under flow ctrl we must use single send mode*/
    if((FALSE == hcc->hcc_queues[HCC_TX].queues[type].flow_ctrl.enable)
     && (hcc_single_send != send_mode))
    {
        send_mode = hcc_single_send;
        HWIFI_DEBUG("Must use single send mode when the queue not under flow ctrl, force single send...");
    }

    smp_rmb();
    hcc_tx_transfer_lock(hcc);
    if(unlikely(hcc->assembled_cnt))
    {
        HWIFI_WARNING("next assembed_cnt should be 0 but now is %u", hcc->assembled_cnt);
        HWIFI_WARNING("[%-30s]", hcc_get_queue_str(type));
        /*hcc->assembled_cnt = 0;*/
        if(SUCC != hcc_send_assemble_reset(hcc))
        {
            hcc_tx_transfer_unlock(hcc);
            return 1;
        }
    }
    max_len = hcc->hcc_queues[HCC_TX].queues[type].queue_trans_limit;
    HWIFI_PR_DEBUG("max_len:%u", max_len);
    if(hcc_single_send == send_mode)
    {
        ret = hcc_send_tx_queue_single(hcc, type, max_len);
    }
    else if(hcc_assem_send == send_mode)
    {
        ret = hcc_send_tx_queue_assemble(hcc, type, max_len);
    } else {
        HWIFI_WARNING("unkown send mode :%d", send_mode);
    }

    hcc_tx_transfer_unlock(hcc);
#ifdef HCC_DEBUG
    atomic_add_return(ret, &hcc->hcc_queues[HCC_TX].queues[type].queue_count);
#endif
    return ret;
}
int hcc_send_rx_queue(struct hcc_handler *hcc, hcc_queue_type type)
{
    int32  ret = 1;
    uint32 max_len;
    int old_len,new_len, len_t;
    uint32 transfer_len, index;
    spinlock_t*  lock;
    struct sk_buff *skb;
    struct sk_buff_head* head;
    struct sk_buff_head  head_t;
    struct hi110x_sdio *hi_sdio;
    HWIFI_BUG_ON(!hcc);
    hi_sdio = bus_to_sdio(hcc->io_dev);
    skb_queue_head_init(&head_t);
    head = &hcc->hcc_queues[HCC_RX].queues[type].data_queue;
    lock = &hcc->hcc_queues[HCC_RX].queues[type].data_queue_lock;
    hcc_rx_split_lock(hcc);
    spin_lock_bh(lock);
    if(!skb_queue_len(head))
    {
        ret = 0;
        spin_unlock_bh(lock);
        hcc_rx_split_unlock(hcc);
        HWIFI_PR_DEBUG("rx queue %s is empty", hcc_get_queue_str(type));
        return ret;
    }
    skb_queue_splice_tail_init(head, &head_t);
    spin_unlock_bh(lock);

    old_len = new_len = skb_queue_len(&head_t);
    hcc_trans_queue_filter(hcc, &head_t, type, HCC_RX);
    new_len = skb_queue_len(&head_t);
    HWIFI_WARN(new_len > old_len,"new len:%d, old len:%d\n", new_len, old_len);
    /*rx had no queues flow ctrl, rx flow ctrl used to protect schedule*/
    len_t = old_len - new_len;

    hcc_flow_ctrl_len_sub(hcc, HCC_RX, len_t);
    hcc_trans_skb_dequeue_handle(hcc, type, HCC_RX, len_t);


    max_len = hcc->hcc_queues[HCC_RX].queues[type].queue_trans_limit;
    HWIFI_PR_DEBUG("max_len:%u", max_len);
    transfer_len = min(max_len, skb_queue_len(&head_t));
    HWIFI_DEBUG("[%s]:max_len[%u], head.qlen[%u],ret[%u],transfer_len[%u]",
                   hcc_get_queue_str(type),max_len, skb_queue_len(&head_t), ret, transfer_len);
    for(index = 0; index < transfer_len; index++)
    {
        int ret_t;
        skb = skb_dequeue(&head_t);
        if(unlikely(!skb))
            break;
#ifdef HCC_DEBUG
        atomic_inc_return(&hcc->hcc_queues[HCC_RX].queues[type].queue_count);
#endif
        HWIFI_PR_DEBUG("hcc rx==>skb index[%u]", index);
        ret_t = hcc_rx(hcc, skb);
        if(unlikely(ret_t))
        {
            HWIFI_WARNING("upload pkt failed [ret=%d]", ret_t);
        }

    hcc_flow_ctrl_len_sub(hcc, HCC_RX, 1);
    hcc_trans_skb_dequeue_handle(hcc, type, HCC_RX, 1);

    /*use wait event the cpu load too high*/
    /*hcc_queues_flow_ctrl_handle(hcc, type, HCC_RX, 1, 1);*/

    }

    spin_lock_bh(lock);
    skb_queue_splice_init(&head_t, head);
    spin_unlock_bh(lock);
    hcc_rx_split_unlock(hcc);
    HWIFI_PR_DEBUG("sent rx queue to net layer ==> %s len:%d", hcc_get_queue_str(type), transfer_len);
    HWIFI_WARN(transfer_len != index, "transfer_len[%u] != index[%u] ", transfer_len,index);

    return index;
}
STATIC int32 hcc_send_single_data_packet(struct hcc_handler* hcc, struct sk_buff_head *head,
                                        hcc_queue_type type,
                                        uint32 next_assemble_cnt)
{
    int32               ret = SUCC;
    struct sk_buff     *skb;
    skb = skb_dequeue(head);
    if (NULL == skb)
    {
        HWIFI_ERROR("skb is NULL [len:%d]", skb_queue_len(head));
        return SUCC;
    }
    HWIFI_PR_DEBUG("deal:%s next assemable, queue:%s, seq:%u, queue len:%u", hcc_send_mode_str[hcc_single_send],
                                       "unkown",
                                       ((struct hcc_header*)skb->data)->seq,
                                       skb_queue_len(head));
#ifdef HCC_DEBUG
    atomic_inc_return(&hcc->debug.hcc_single_count);
#endif

    hcc->assembled_cnt = fill_packet_assemble_info_sg(hcc, skb, 1, head, next_assemble_cnt);
    ret = __send_single_packet_sg(hcc, skb);
    if (ret != SUCC)
    {
        HWIFI_WARNING("Failed to send single packet");
    }
    dev_kfree_skb(skb);

    return ret;
}


STATIC int32 hcc_send_assemble_data_packet(struct hcc_handler *hcc, struct sk_buff_head* head,
                                          hcc_queue_type type,
                                         uint32 assembled_cnt, uint32 next_assemble_cnt)
{
    int32               ret = SUCC;
    struct sk_buff     *skb;
    struct hcc_header  *header;
    uint32              idx;
    int32               sum_len = 0;
    int32               copy_len = 0;
    unsigned long              need_pad;
    uint32              sg_len;
    struct sk_buff_head         tx_head;
    struct scatterlist sg[HCC_SDIO_SG_ASSEMBLE_NUM];

    if(HWIFI_WARN_ON(0 == assembled_cnt))
    {
        return SUCC;
    }

    skb_queue_head_init(&tx_head);
    sg_init_table(sg, HCC_SDIO_SG_ASSEMBLE_NUM);

    /*list next assembled_cnt skb packages*/
    for(idx = 0; idx < assembled_cnt; idx++)
    {
        skb = skb_dequeue(head);
        if(unlikely(!skb))
        {
            int i = 0;
            HWIFI_ERROR("unexpect break, can not fetch skb,idx[%u],assembled_cnt[%u],qlen[%u]",
                        idx, assembled_cnt, skb_queue_len(head));
            HWIFI_ERROR("+++dump assem_info last fill++++");
            for(i = 0; i < HCC_MAX_ASSEMBLE_NUM; i++)
            {
                HWIFI_ERROR("%d:%u", i, hcc->assem_info[i]);
            }
            HWIFI_ERROR("---dump assem_info last fill---");
            hcc->assembled_cnt = 0;
            break;
        }
#ifdef HCC_DEBUG
        atomic_inc_return(&hcc->debug.hcc_assem_count);
#endif
        HWIFI_PR_DEBUG("deal:%s, assem send, seq:%u, queue len:%u", hcc_send_mode_str[hcc_assem_send],
                                               ((struct hcc_header*)skb->data)->seq,
                                               skb_queue_len(head));
        if(HWIFI_WARN(!skb, "Skb should not be null [idx:%u][ass_cnt:%u]", idx, assembled_cnt))
        {
            /*if failed we should remove skb buffer and send next package singly*/
            /*This should never happened*/
            HWIFI_WARNING("Skb should not be null [idx:%u][ass_cnt:%u]\n", idx, assembled_cnt);
            hcc->assembled_cnt = 0;
            break;
        }

        __skb_queue_tail(&tx_head, skb);
        /*skb->len had aligned by function skb_align_to_sg_format*/
        HWIFI_BUG_ON(skb->len & 0x7);

        if(HWIFI_WARN(((skb->len) >> 3) != hcc->assem_info[idx],
                        "len:%d, assem_info[%u]:%u", skb->len,idx,
                        hcc->assem_info[idx]))
        {
            HWIFI_WARNING("len:%d, assem_info[%u]:%u", skb->len,idx,
                        hcc->assem_info[idx]);
            hcc->assembled_cnt = 0;
            break;
        }

        /* fill hcc header information */
        if((assembled_cnt - 1) != idx)
        {
            SET_PKT_MORE_FLAG(skb, 1);
        }
        else /* last one */
        {
            /* for assem len has set, so can't change skb len */
            hcc->assembled_cnt = fill_packet_assemble_info_sg(hcc, skb, 0, head, next_assemble_cnt);
            SET_PKT_MORE_FLAG(skb, 0);
        }

        need_pad = ((unsigned long)skb->data) & 0x03;
        if(unlikely((need_pad & 0x1) ||
           (((need_pad & 0x10) + skb->len) > SINGLE_PACKET_MAX_SIZE)))
        {
            /*tx_buf should be enough, add assert to debug.*/
            HWIFI_BUG_ON(copy_len + skb->len > hcc->tx_buf_size);
            /*copy skb buffer to tx_buffer*/
            memcpy(hcc->tx_buf + copy_len, skb->data, skb->len);
            sg_set_buf(&sg[idx], hcc->tx_buf + copy_len, skb->len);
            /*because skb->len had aligned to 8 bytes so we needn't align again.*/
            copy_len += skb->len;
        }
        else
        {
            sg_set_buf(&sg[idx], skb->data, skb->len);
        }
        sum_len += skb->len;

        HWIFI_PR_DEBUG("sg mode=>hcc:pad:%d, more:%d, option:%d, payload_len:%d, sumlen:%d",
                    header->pad, header->more, header->option, header->pay_len, sum_len);
        HWIFI_PR_DEBUG("sg mode=>skb:data:%p, len:%d, sg:buffer:%p, len:%u, copy_len:%d",
                    skb->data, skb->len, sg_virt(&sg[idx]), sg[idx].length, copy_len);

    }

    HWIFI_BUG_ON(((unsigned long)hcc->tx_buf+ copy_len)& 0x3);
    sg_len = assembled_cnt;

    if(likely(idx == assembled_cnt))
    {
        /*no err we can send sg buffer normally*/
        /* for assem len has set, so can't change skb len */
        ret = bus_transfer_sg(hcc->io_dev, sg, sg_len, sum_len);
        if (unlikely(ret != SUCC))
        {
            HWIFI_ERROR("Failed to send assemble sg![ret:%d][sum_len:%d]",
                            ret, sum_len);
            hcc->assembled_cnt = 0;
            /*FIXME? Debug*/
            if(HWIFI_DEBUG_CONDTION())
                dump_sg_buffer(KERN_EMERG,sg, sg_len);
            ret =  -EFAIL;
        }
    }
    else
    {
        HWIFI_ERROR("[idx:%u][assembled_cnt:%u]", idx, assembled_cnt);
        ret = -EFAIL;
    }
    if(likely(!ret))
    {
        assem_debug_tx_count_incr(hcc, MIN(sg_len, (uint32)HCC_MAX_ASSEMBLE_NUM));
    }

    /*free the skb buffer just sent out.*/
    skb_queue_purge(&tx_head);

    return ret;
}


STATIC inline int32 hcc_send_assemble_reset(struct hcc_handler *hcc)
{
    struct hcc_header header;
    int32             ret;

    header.type    = HCC_MK_TYPE(HCC_TYPE_HCC_CTRL, 0);
    header.pad     = 0;
    header.more    = 0;
    header.option  = 0;
    header.pay_len = 0;
    header.seq     = 0;

    ret = bus_transfer(hcc->io_dev, (unsigned char *)&header, sizeof(struct hcc_header));
    if (ret != SUCC)
    {
        HWIFI_WARNING("Failed to send assemble reset len = %zu", sizeof(struct hcc_header));
        return -EFAIL;
    }

    hcc->assembled_cnt = 0;

    HWIFI_DEBUG("send hcc ctrl to reset assembled info");

    return SUCC;
}
#if 0

STATIC int32 hcc_send_hi_ctrl_packets(struct hcc_handler* hcc)
{
    struct sk_buff_head     skb_queue_temp;
    struct sk_buff         *skb;

    if ((skb_queue_len(&hcc->hcc_queues[HCC_TX].queues[CTRL_QUEUE].data_queue) == 0)
        && (skb_queue_len(&hcc->hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].data_queue) == 0))
    {
        return SUCC;
    }

    if ((hcc->assembled_cnt > 0)
        && (hcc_send_assemble_reset(hcc) != SUCC))
    {
        return -EFAIL;
    }

    __skb_queue_head_init(&skb_queue_temp);

    while (1)
    {
        /* 获取队列中全部数据 */
        spin_lock_bh(&hcc->hcc_queues[HCC_TX].queues[CTRL_QUEUE].data_queue_lock);
        skb_queue_splice_tail_init(&hcc->hcc_queues[HCC_TX].queues[CTRL_QUEUE].data_queue, &skb_queue_temp);
        spin_unlock_bh(&hcc->hcc_queues[HCC_TX].queues[CTRL_QUEUE].data_queue_lock);

        spin_lock_bh(&hcc->hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].data_queue_lock);
        skb_queue_splice_tail_init(&hcc->hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].data_queue, &skb_queue_temp);
        spin_unlock_bh(&hcc->hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].data_queue_lock);

        if (skb_queue_len(&skb_queue_temp) == 0)
        {
            break;
        }

        /* one by one send data*/
        while ((skb = __skb_dequeue(&skb_queue_temp)) != NULL)
        {
            if (__send_single_packet_sg(hcc, skb) != SUCC)
            {
                HWIFI_WARNING("Failed to send nonassembled packet");
                dev_kfree_skb(skb);
                __skb_queue_purge(&skb_queue_temp);
                return -EFAIL;
            }
            dev_kfree_skb(skb);
        }
    }

    return SUCC;
}
#endif


#define TCP_SCHED_PARM  (10000)
STATIC int hcc_transfer_process(struct hcc_handler *hcc)
{
    uint32 ret = 0;
    uint32 dyn_pri_enable;

    /*HCC_FLUSH_ALL 默认最大个数发送，具体一次发送多少个SKB 待实测调整*/
    HWIFI_BUG_ON(!hcc);

    if(HI_MODE_STA == hcc->hi110x_dev->mode.current_mode )
    {
        /*Tx Tcp Data queue > Rx Tcp Ack
          Rx Tcp Data > Tx Tcp Ack
          Tx Tcp Data queue > Rx Tcp Data queue*/
        ret |= hcc_send_rx_queue(hcc, CTRL_QUEUE);
        ret |= hcc_send_tx_queue(hcc, CTRL_QUEUE, hcc_single_send);

        ret |= hcc_send_rx_queue(hcc, DATA_HI_QUEUE);
        ret |= hcc_send_tx_queue(hcc, DATA_HI_QUEUE, hcc_single_send);

       /*process hcc rx ctrl and high data queue*/

        ret |= hcc_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
        ret |= hcc_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE, hcc_assem_send);

        dyn_pri_enable = hcc->sys_hcc_dyn_pri_enable;
        if(dyn_pri_enable)
        {
            ret |= hcc_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);
        }else{
            ret |= hcc_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);
            ret |= hcc_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE, hcc_assem_send);
        }

        /*Tx Lo < Rx Lo*/
        ret |= hcc_send_rx_queue(hcc, DATA_LO_QUEUE);
        ret |= hcc_send_tx_queue(hcc, DATA_LO_QUEUE, hcc_assem_send);

    #ifdef WMM_OPT_FOR_AUTH
        if (TRUE == hcc->sys_wmm_ac_opt_enable)
        {
            ret |= hcc_send_tx_queue(hcc, DATA_UDP_VO_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_UDP_VO_QUEUE);
            ret |= hcc_send_tx_queue(hcc, DATA_UDP_VI_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_UDP_VI_QUEUE);
            ret |= hcc_send_tx_queue(hcc, DATA_UDP_BE_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_UDP_BE_QUEUE);
            ret |= hcc_send_tx_queue(hcc, DATA_UDP_BK_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_UDP_BK_QUEUE);
        }
    #endif
    }
    else if(HI_MODE_AP == hcc->hi110x_dev->mode.current_mode)
    {
        /*Tx Tcp Data queue > Rx Tcp Ack
          Rx Tcp Data > Tx Tcp Ack
          Tx Tcp Data queue < Rx Tcp Data queue*/
        ret |= hcc_send_tx_queue(hcc, CTRL_QUEUE, hcc_single_send);
        ret |= hcc_send_rx_queue(hcc, CTRL_QUEUE);

        ret |= hcc_send_tx_queue(hcc, DATA_HI_QUEUE, hcc_single_send);
        ret |= hcc_send_rx_queue(hcc, DATA_HI_QUEUE);

        /*process hcc rx ctrl and high data queue*/

        ret |= hcc_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE, hcc_assem_send);
        ret |= hcc_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);

        ret |= hcc_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
        ret |= hcc_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE, hcc_assem_send);

        /*Tx Lo < Rx Lo*/
        hcc_send_tx_queue(hcc, DATA_LO_QUEUE, hcc_assem_send);
        ret |= hcc_send_rx_queue(hcc, DATA_LO_QUEUE);

    #ifdef WMM_OPT_FOR_AUTH
        if (TRUE == hcc->sys_wmm_ac_opt_enable)
        {
            ret |= hcc_send_tx_queue(hcc, DATA_UDP_VO_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_UDP_VO_QUEUE);
            ret |= hcc_send_tx_queue(hcc, DATA_UDP_VI_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_UDP_VI_QUEUE);
            ret |= hcc_send_tx_queue(hcc, DATA_UDP_BE_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_UDP_BE_QUEUE);
            ret |= hcc_send_tx_queue(hcc, DATA_UDP_BK_QUEUE, hcc_assem_send);
            ret |= hcc_send_rx_queue(hcc, DATA_UDP_BK_QUEUE);
        }
    #endif
    }
    else
    {
        /*default process sequence*/
        ret |= hcc_send_tx_queue(hcc, CTRL_QUEUE, hcc_single_send);
        ret |= hcc_send_tx_queue(hcc, DATA_HI_QUEUE, hcc_single_send);

        /*process hcc rx ctrl and high data queue*/
        ret |= hcc_send_rx_queue(hcc, CTRL_QUEUE);
        ret |= hcc_send_rx_queue(hcc, DATA_HI_QUEUE);

        ret |= hcc_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
        ret |= hcc_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);
        ret |= hcc_send_rx_queue(hcc, DATA_LO_QUEUE);
        ret |= hcc_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE, hcc_assem_send);
        ret |= hcc_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE, hcc_assem_send);
        ret |= hcc_send_tx_queue(hcc, DATA_LO_QUEUE, hcc_assem_send);
    }

    return ret;
}


STATIC int32 hcc_trans_thread(void *data)
{
    int ret = 0;
    struct sched_param       param;
    struct hcc_handler      *hcc;
    hcc = (struct hcc_handler *)data;

    HWIFI_BUG_ON(!hcc);

    allow_signal(SIGTERM);
    printk(KERN_DEBUG"Hisi Wifi sched ctrl hcc trans thread normal speed\n");
    param.sched_priority = 0;
    hi110x_set_thread_property(current,SCHED_NORMAL,
                                &param,
                                 HI110X_HCC_TRANS_THREAD_NICE);

    while(1)
    {
        ret = wait_event_interruptible(hcc->transfer_wait_queue,hcc_thread_wait_event_cond_check(hcc));
        if(unlikely(-ERESTARTSYS == ret))
        {
            HWIFI_WARNING("wifi task %s was interrupted by a signal", current->comm);
            break;
        }
        if(unlikely(kthread_should_stop()))
        {
            break;
        }
        HWIFI_PR_DEBUG("++hcc transfer process++");

        if(!hcc_transfer_process(hcc))
        {
            /*Did not process any skb data*/
            schedule();
        }
        HWIFI_PR_DEBUG("--hcc transfer process+--");
        /*move the unlock action to the dequeue function.*/
        /*hi110x_wake_unlock(hcc->hi110x_dev->pm_info);*/
    }
    HWIFI_INFO("Sdio Trans Thread Terminated");
    return SUCC;

}
STATIC int32 hcc_rx_cb_default(uint8 mtype, uint8 stype, struct sk_buff *skb, void *context)
{
    dev_kfree_skb_any(skb);
    return SUCC;
}


int32 hcc_rx_register(struct hcc_handler *hcc, uint8 mtype, hcc_rx_cb_func rx_cb, void *context)
{
    struct hcc_rx_action    *rx_action;

    if ((NULL == hcc) || (NULL == rx_cb) || (mtype >= HCC_TYPE_MAX))
    {
        HWIFI_WARNING("Invalid params:hcc = %p, rx_cb_func = %p, main type = %u", hcc, rx_cb, mtype);
        return -EFAIL;
    }
    HWIFI_DEBUG("Register rx_cb function :%p",rx_cb);
    rx_action = &hcc->rx_actions[mtype];

    mutex_lock(&rx_action->mutex_lock);
    rx_action->rx_cb = rx_cb;
    rx_action->context = context;
    rx_action->registered = 1;
    mutex_unlock(&rx_action->mutex_lock);

    return SUCC;
}


void hcc_rx_unregister(struct hcc_handler *hcc, uint8 mtype)
{
    struct hcc_rx_action    *rx_action;

    if ((NULL == hcc) || (mtype >= HCC_TYPE_MAX))
    {
        HWIFI_WARNING("Invalid params:hcc = %p, main type = %u", hcc, mtype);
        return;
    }

    rx_action = &hcc->rx_actions[mtype];

    mutex_lock(&rx_action->mutex_lock);
    rx_action->rx_cb = hcc_rx_cb_default;
    rx_action->registered = 0;
    mutex_unlock(&rx_action->mutex_lock);
}


int hcc_net_stream_connected(struct hcc_handler    *hcc,
                                                                                              hwifi_net_stream_type stream_t)
{
    return !(HNET_STREAM_DISCONN == hcc->net_stream.stream[stream_t].sm_stat);
}


int hcc_net_streams_connected(struct hcc_handler    *hcc)
{
    /*need't sync with aggre.c, scan the throughput */
    int i;
    for(i = 0; i < STREAM_COUNT; i++)
    {
        if(hcc_net_stream_connected(hcc, i))
        {
            return TRUE;
        }
    }
    return FALSE;
}

STATIC void hcc_net_stream_sm_recount(struct hcc_handler    *hcc,
                                                                                                           hwifi_net_stream_item* stream)
{
        stream->sm_tx_pack = hcc->hi110x_dev->netdev->stats.tx_packets;
        stream->pkt_count = 0;
}

STATIC void hcc_net_stream_sm_change(struct hcc_handler    *hcc,
                                                                                                           hwifi_net_stream_item* stream,
                                                                                                           hwifi_net_stream_sm next_sm_stat)
{
    HWIFI_INFO("%s  sm change from %s to %s",
                                stream->name,
                                hcc_get_net_stream_sm_str(stream->sm_stat),
                                hcc_get_net_stream_sm_str(next_sm_stat));

    if(HNET_STREAM_DISCONN == stream->sm_stat)
    {
        if(HNET_STREAM_CONNTING == next_sm_stat)
        {
            /*HNET_STREAM_DISCONN ==> HNET_STREAM_CONNTING*/
            hcc_net_stream_sm_recount(hcc, stream);
        }
    }
    else if(HNET_STREAM_CONNTING ==  stream->sm_stat)
    {
        if(HNET_STREAM_CONNETED== next_sm_stat)
        {
            /*HNET_STREAM_CONNTING ==> HNET_STREAM_CONNETED*/
            hcc_net_stream_sm_recount(hcc, stream);
        }
        else if(HNET_STREAM_DISCONN == next_sm_stat)
        {
            /*HNET_STREAM_CONNTING ==> HNET_STREAM_DISCONN*/
            hcc_net_stream_sm_recount(hcc, stream);
        }
    }

    stream->sm_stat = next_sm_stat;

}


int  hcc_net_stream_try_connect(struct hcc_handler    *hcc,
                                                                                  hwifi_net_stream_type stream_t)
{
    if(hcc->net_stream.stream[stream_t].ignored)
    {
        /*do not connect if whe do not care this stream.*/
        return FALSE;
    }

    hcc->net_stream.stream[stream_t].pkt_count++;

    /*start timer immediately*/
    if( !hcc_net_stream_connected(hcc, stream_t))
    {
        hcc_net_stream_sm_change(hcc,
                                                            &hcc->net_stream.stream[stream_t],
                                                            HNET_STREAM_CONNTING);
        mod_timer(&hcc->net_stream.stream[stream_t].timer,
                              jiffies + hcc->net_stream.stream[stream_t].connecting_period);
    }

    return TRUE;
}

STATIC void hcc_net_stream_disconnect_timer(unsigned long data)
{
    unsigned long period = 0;

    hwifi_net_stream_item*  stream = (hwifi_net_stream_item*)data;
    hwifi_net_stream* net_stream;
    struct hcc_handler    *hcc;
    net_stream=
                    (hwifi_net_stream*)((unsigned long)stream - sizeof(hwifi_net_stream_item)*stream->index);
    HWIFI_BUG_ON(!net_stream);
    hcc = container_of(net_stream, struct hcc_handler, net_stream);
    HWIFI_BUG_ON(!hcc);
    HWIFI_DEBUG("deal with stream %s", stream->name);
    HWIFI_BUG_ON(!hcc_net_stream_connected(hcc, stream->index));

    /*if connecting*/
    if(HNET_STREAM_CONNTING == stream->sm_stat)
    {
        /*try to connect time periold arrived*/
        if(stream->pkt_count >= stream->connecting_pkts_limit)
        {
            /*stream connect sucuess.*/
            HWIFI_INFO("%lu more than limit %lu, connected.",
                                        stream->pkt_count, stream->connecting_pkts_limit);
            hcc_net_stream_sm_change(hcc, stream, HNET_STREAM_CONNETED);
            /*try to del ba.*/
            wl_aggre_mode_lock_set(hcc->hi110x_dev->cfg, TRUE);
            period = stream->disconn_period;
        }
        else
        {
            /*connect fail, break.*/
            HWIFI_INFO("%lu less than limit %lu, disconnect.",
                                        stream->pkt_count, stream->connecting_pkts_limit);
            period = 0;
            hcc_net_stream_sm_change(hcc, stream, HNET_STREAM_DISCONN);
        }

    }
    else if(HNET_STREAM_CONNETED == stream->sm_stat)
    {
        /*stat the percent.*/
        unsigned long tx_pkts_curr, tx_pkts_old, tx_pkts_trans, pkt_count;

        tx_pkts_curr = hcc->hi110x_dev->netdev->stats.tx_packets;
        tx_pkts_old = stream->sm_pkt_count;
        tx_pkts_trans = (tx_pkts_curr > tx_pkts_old)?(tx_pkts_curr - tx_pkts_old):1;
        pkt_count = stream->pkt_count;

        stream->sm_pkt_count      = tx_pkts_curr;
        stream->pkt_count = 0;

        HWIFI_INFO("stream %s transfer  %lu pkts, total %lu", stream->name, pkt_count, tx_pkts_trans);
        /*percentage*/
        if(((pkt_count*100) / tx_pkts_trans) <= stream->disconn_percent_limit)
        {
            /*disconnect*/
            period = 0;
            hcc_net_stream_sm_change(hcc, stream, HNET_STREAM_DISCONN);
        }
        else
        {
            /*hold connected.*/
            period = stream->disconn_period;
            HWIFI_INFO("still connected, keep scan... %u later",
                                        jiffies_to_msecs(stream->disconn_period));
        }
    }
    else if(HNET_STREAM_DISCONN == stream->sm_stat)
    {
        HWIFI_WARNING("why timeout in stat disconnect ?");
    }
    else
    {
        HWIFI_ERROR("wrong stat machine type:%d", stream->sm_stat);
    }

    if(period)
        mod_timer(&stream->timer,jiffies+period);
}

STATIC void hcc_net_stream_init(struct hcc_handler    *hcc)
{
    int i;
    spin_lock_init(&hcc->net_stream.lock);
    for(i = 0; i < STREAM_COUNT; i++)
    {
        hcc->net_stream.stream[i].index = i;
        hcc->net_stream.stream[i].pkt_count = 0;
        init_timer(&hcc->net_stream.stream[i].timer);
        hcc->net_stream.stream[i].timer.data = (unsigned long)&hcc->net_stream.stream[i];
        hcc->net_stream.stream[i].timer.function = (void*)hcc_net_stream_disconnect_timer;
        hcc->net_stream.stream[i].name = hcc_net_stream_str[i]?:"unkown";
        hcc->net_stream.stream[i].disconn_percent_limit = HCC_NET_STREAM_PERCENT_LIMIT;
        hcc->net_stream.stream[i].sm_stat= HNET_STREAM_DISCONN;

        hcc->net_stream.stream[i].connecting_period = 3*HZ;
        hcc->net_stream.stream[i].disconn_period = 10*HZ;

        /*when over this, we think stream connect sucuess.*/
        hcc->net_stream.stream[i].connecting_pkts_limit = 30;

        hcc->net_stream.stream[i].ignored = FALSE;
    }
    HWIFI_BUG_ON((unsigned long)hcc->net_stream.stream != ((unsigned long)&hcc->net_stream));
}

STATIC void hcc_net_stream_deinit(struct hcc_handler    *hcc)
{
    int i;
    for(i = 0; i < STREAM_COUNT; i++)
    {
        del_timer_sync(&hcc->net_stream.stream[i].timer);
    }
    /*memset(&hcc->net_stream, 0, sizeof(hwifi_net_stream));*/
}

/*Net work exception detect*/
STATIC int hcc_cfg_connected(struct hcc_handler    *hcc)
{
    HWIFI_BUG_ON(!hcc);
    if(!hcc || !hcc->hi110x_dev || !hcc->hi110x_dev->cfg)
    {
        HWIFI_INFO("ptr null.");
        return FALSE;
    }

    if(TRUE != hcc->net_work_detect.enable)
    {
        return FALSE;
    }

    if(HI_MODE_STA == hcc->hi110x_dev->mode.current_mode)
    {
        if(HWIFI_CONNECTED == hcc->hi110x_dev->cfg->conn.status)
        {
            /*Sta mode, and connected.*/
            return TRUE;
        }
    }
    HWIFI_DEBUG("current mode:%d,conn status:%d ",
                                hcc->hi110x_dev->mode.current_mode,
                                hcc->hi110x_dev->cfg->conn.status);
    return FALSE;
}
STATIC void hcc_net_work_detect_timer(unsigned long data)
{
    struct hcc_handler    *hcc = (struct hcc_handler    *)data;
    /*if had disconnect, we do not care about the driver state*/
    if(TRUE !=hcc_cfg_connected(hcc))
        return;
    hcc->net_work_detect.driver_state++;

    if(!hcc->net_work_detect.first_time)
        hcc->net_work_detect.first_time = cpu_clock(UINT_MAX);
    else
        hcc->net_work_detect.last_time = cpu_clock(UINT_MAX);

    HWIFI_INFO("net stream may be issue, notify the app detect.");
}

STATIC  int hcc_net_work_detect_init(struct hcc_handler    *hcc)
{
    hcc->net_work_detect.detect_timer.data        = (unsigned long)hcc;
    hcc->net_work_detect.detect_timer.function = hcc_net_work_detect_timer ;
    init_timer(&hcc->net_work_detect.detect_timer);
    hcc->net_work_detect.enable= TRUE;

    hcc->net_work_detect.driver_state = 0;

    /*5 seconds*/
    hcc->net_work_detect.timeout = 5*HZ;

    return SUCC;
}

void hcc_net_work_detect_deinit(struct hcc_handler    *hcc)
{
    /*sync del timer.*/
    if(!hcc)
        return;
    HWIFI_DEBUG("deinit net work detect timer");
    del_timer_sync(&hcc->net_work_detect.detect_timer);
}

 void hcc_net_work_state_detect(struct hcc_handler    *hcc)
 {
    if(unlikely(!hcc))
        return;
    if(timer_pending(&hcc->net_work_detect.detect_timer))
    {
        HWIFI_DEBUG("net work detect timer exist");
        return;
    }
    if(TRUE != hcc_cfg_connected(hcc))
    {
        HWIFI_DEBUG("cfg was disconnected.");
        return;
    }
    HWIFI_DEBUG("try to detect net work state");
    mod_timer(&hcc->net_work_detect.detect_timer, jiffies + hcc->net_work_detect.timeout);
 }

 void hcc_net_work_state_detect_cancel(struct hcc_handler    *hcc)
 {
     if(unlikely(!hcc))
        return;
    if(!hcc->net_work_detect.enable)
        return;
    HWIFI_BUG_ON(in_interrupt());
    HWIFI_DEBUG("cancel detect");
    del_timer_sync(&hcc->net_work_detect.detect_timer);
 }


STATIC void hcc_rx_actions_init(struct hcc_handler *hcc)
{
    uint32 i;

    for (i=0; i<HCC_TYPE_MAX; i++)
    {
        hcc->rx_actions[i].registered = 0;
        hcc->rx_actions[i].context = NULL;
        hcc->rx_actions[i].rx_cb = hcc_rx_cb_default;
        mutex_init(&hcc->rx_actions[i].mutex_lock);
    }
}

#ifdef HCC_DEBUG
STATIC int hcc_debug_alloc_peak_stat(struct hcc_handler *hcc)
{
    int i ;
    if(!hcc->debug.peak_debug)
        return 0;
    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        hcc->hcc_queues[HCC_TX].queues[i].queue_peak_count = vmalloc(HCC_QUEUE_PEAK_STAT_LEN);
        if(!hcc->hcc_queues[HCC_TX].queues[i].queue_peak_count)
        {
            HWIFI_ERROR("tx %s vmalloc len:%zu failed!", hcc_get_queue_str(i),
                        HCC_QUEUE_PEAK_STAT_LEN);
            return  -EFAIL;
        }
        memset(hcc->hcc_queues[HCC_TX].queues[i].queue_peak_count, 0, HCC_QUEUE_PEAK_STAT_LEN);

        hcc->hcc_queues[HCC_RX].queues[i].queue_peak_count = vmalloc(HCC_QUEUE_PEAK_STAT_LEN);
        if(!hcc->hcc_queues[HCC_RX].queues[i].queue_peak_count)
        {
            HWIFI_ERROR("rx %s vmalloc len:%zu failed!", hcc_get_queue_str(i),
                        HCC_QUEUE_PEAK_STAT_LEN);
            return  -EFAIL;
        }
        memset(hcc->hcc_queues[HCC_RX].queues[i].queue_peak_count, 0, HCC_QUEUE_PEAK_STAT_LEN);
    }
    return SUCC;
}

STATIC void hcc_debug_free_peak_stat(struct hcc_handler *hcc)
{
    int i ;
    if(!hcc->debug.peak_debug)
        return;
    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        if(hcc->hcc_queues[HCC_TX].queues[i].queue_peak_count)
        {
            vfree(hcc->hcc_queues[HCC_TX].queues[i].queue_peak_count);
            hcc->hcc_queues[HCC_TX].queues[i].queue_peak_count = NULL;
        }

        if(hcc->hcc_queues[HCC_RX].queues[i].queue_peak_count)
        {
            vfree(hcc->hcc_queues[HCC_RX].queues[i].queue_peak_count);
            hcc->hcc_queues[HCC_RX].queues[i].queue_peak_count = NULL;
        }
    }
}

STATIC void hcc_debug_peak_stat_exit(struct hcc_handler *hcc)
{
    HWIFI_BUG_ON(!hcc);
    if(!hcc->debug.peak_debug)
        return;
    hcc_debug_free_peak_stat(hcc);
}

STATIC int hcc_debug_peak_stat_init(struct hcc_handler *hcc)
{
    int i;
    int ret = SUCC;
    HWIFI_BUG_ON(!hcc);
    if(!hcc->debug.peak_debug)
        return 0;
    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        hcc->hcc_queues[HCC_TX].queues[i].queue_peak_count = NULL;
        hcc->hcc_queues[HCC_RX].queues[i].queue_peak_count = NULL;
    }

    /*alloc memory*/
    ret = hcc_debug_alloc_peak_stat(hcc);
    if(ret)
        goto failed;

    HWIFI_INFO("hcc debug peak stat init sucuess.");
    return ret;
failed:
    /*free memory*/
    hcc_debug_free_peak_stat(hcc);
    return ret;
}

void hcc_debug_peak_inc(struct hcc_handler   *hcc, hcc_chan_type dir,
                        hcc_queue_type type,unsigned long count)
{
    if(likely(!hcc->debug.peak_debug))
        return;
    count = MIN(count, (unsigned long)(HCC_QUEUE_PEAK_STAT_COUNT - 1));
    hcc->hcc_queues[dir].queues[type].queue_peak_count[count]++;
}

void hcc_debug_peak_reset(struct hcc_handler   *hcc)
{
    int i,j;
    if(!hcc->debug.peak_debug)
    {
        printk(KERN_DEBUG"peak debug had been off\n");
        return;
    }
    for(i = 0;i < HCC_DIR_COUNT;i++)
    {
        for(j = 0; j < HCC_QUEUE_COUNT;j++)
        {
            memset(hcc->hcc_queues[i].queues[j].queue_peak_count, 0, HCC_QUEUE_PEAK_STAT_LEN);
        }
    }
    return;
}

void hcc_debug_peak_show(struct hcc_handler   *hcc, hwifi_union_log* log)
{
    int i,j,k;
    if(!hcc->debug.peak_debug)
    {
        HWIFI_UNION_PRINT(log,"peak debug had been off\n");
        return;
    }
    for(i = 0;i < HCC_DIR_COUNT;i++)
    {
        HWIFI_UNION_PRINT(log, "%s peak\n", hcc_get_dir_str(i));
        for(j = 0; j < HCC_QUEUE_COUNT;j++)
        {
            int newline =0;
            HWIFI_UNION_PRINT(log,"queue:%s debug peak len:0~%u\n",
                                hcc_get_queue_str(j), HCC_QUEUE_PEAK_STAT_COUNT - 1);
            for(k = 0; k < HCC_QUEUE_PEAK_STAT_COUNT - 1; k++)
            {
                if(hcc->hcc_queues[i].queues[j].queue_peak_count[k])
                {
                    newline++;
                    HWIFI_UNION_PRINT(log,"[%4u]:%10lu ", k,
                            hcc->hcc_queues[i].queues[j].queue_peak_count[k]);
                    if(!(newline % 8))
                    {
                        HWIFI_UNION_PRINT(log,"\n");
                    }
                }
            }
            HWIFI_UNION_PRINT(log,"\n");
            schedule();
        }
        HWIFI_UNION_PRINT(log,"\n");
    }
    HWIFI_UNION_PRINT(log,"\n");
}

STATIC void hcc_debug_init(struct hcc_handler   *hcc)
{
    int i;
    HWIFI_BUG_ON(!hcc);
    atomic_set(&hcc->debug.hcc_assem_count, 0);
    atomic_set(&hcc->debug.hcc_tx_count, 0);
    atomic_set(&hcc->debug.hcc_single_count, 0);
    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        atomic_set(&hcc->hcc_queues[HCC_TX].queues[i].queue_count, 0);
        atomic_set(&hcc->hcc_queues[HCC_RX].queues[i].queue_count, 0);
    }
}
#else
void hcc_debug_peak_inc(struct hcc_handler   *hcc, hcc_chan_type dir,
                        hcc_queue_type type,unsigned long count)
{
    return;
}

void hcc_debug_peak_reset(struct hcc_handler   *hcc)
{
    return;
}
#endif

STATIC void hcc_trans_limit_parm_init(struct hcc_handler   *hcc)
{
    int i;

    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        hcc->hcc_queues[HCC_TX].queues[i].queue_trans_limit = HCC_FLUSH_ALL;
        hcc->hcc_queues[HCC_RX].queues[i].queue_trans_limit = HCC_FLUSH_ALL;
    }

    hcc->hcc_queues[HCC_RX].queues[DATA_LO_QUEUE].queue_trans_limit       = hwifi_get_param_int(HISI_CONFIG_WIFI_HCC_RX_LO_QUEUE);
    hcc->hcc_queues[HCC_RX].queues[DATA_TCP_DATA_QUEUE].queue_trans_limit = hwifi_get_param_int(HISI_CONFIG_WIFI_HCC_RX_TCP_DATA_QUEUE);
    hcc->hcc_queues[HCC_RX].queues[DATA_TCP_ACK_QUEUE].queue_trans_limit  = hwifi_get_param_int(HISI_CONFIG_WIFI_HCC_RX_TCP_ACK_QUEUE);
    hcc->hcc_queues[HCC_TX].queues[DATA_LO_QUEUE].queue_trans_limit       = hwifi_get_param_int(HISI_CONFIG_WIFI_HCC_TX_LO_QUEUE);
    hcc->hcc_queues[HCC_TX].queues[DATA_TCP_ACK_QUEUE].queue_trans_limit  = hwifi_get_param_int(HISI_CONFIG_WIFI_HCC_TX_TCP_DATA_QUEUE);
    hcc->hcc_queues[HCC_TX].queues[DATA_TCP_ACK_QUEUE].queue_trans_limit  = hwifi_get_param_int(HISI_CONFIG_WIFI_HCC_TX_TCP_ACK_QUEUE);

    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_BK_QUEUE].queue_trans_limit = 20;
    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_BE_QUEUE].queue_trans_limit = 20;
    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_VI_QUEUE].queue_trans_limit = 20;
    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_VO_QUEUE].queue_trans_limit = 20;
#if 0
    hcc->hcc_queues[HCC_RX].queues[DATA_UDP_BK_QUEUE].queue_trans_limit = 20;
    hcc->hcc_queues[HCC_RX].queues[DATA_UDP_BE_QUEUE].queue_trans_limit = 20;
    hcc->hcc_queues[HCC_RX].queues[DATA_UDP_VI_QUEUE].queue_trans_limit = 20;
    hcc->hcc_queues[HCC_RX].queues[DATA_UDP_VO_QUEUE].queue_trans_limit = 20;
#endif



}

void hcc_action_count_dump(struct hcc_handler  *hcc, hwifi_union_log* log)
{
    int i,j;
    HWIFI_UNION_PRINT(log, "dump hcc action info:\n");
    for(i = 0;i < HCC_DIR_COUNT;i++)
    {
        unsigned long long total_len = 0;
        HWIFI_UNION_PRINT(log,"%s action count:\n",hcc_get_dir_str(i));
        for(j = 0;j < HCC_ACTION_COUNT;j++)
        {
            HWIFI_UNION_PRINT(log,"%s: \t %llu \n",hcc_get_action_str(j),
                    hcc->hcc_queues[i].hcc_action_count[j]);
            total_len+=hcc->hcc_queues[i].hcc_action_count[j];
        }
        HWIFI_UNION_PRINT(log, "total len:%llu \n",total_len);
    }
    HWIFI_UNION_PRINT(log,"\n");
}

void hcc_dev_flow_ctrl_info_dump(struct hcc_handler  *hcc, hwifi_union_log* log)
{
    int i = 0;
    char buff_t[50]={0};
    HWIFI_UNION_PRINT(log,"hcc dev flow ctrl info:\n");
    for(i = 0; i<= HCC_DEV_FLOW_INFO_MAX_TRACE_SECONDS; i++)
    {
       if(hcc->dev_flow_ctrl.flow_info[i].count)
       {
            memset(buff_t, 0, 50);
            if(i)
            {
                hcc_cpu_time_to_str(buff_t, 50,
                                    hcc->dev_flow_ctrl.flow_info[i].cpu_time);
            }
            else
            {
               snprintf(buff_t,  sizeof(buff_t), "untraced");
            }
            HWIFI_UNION_PRINT(log,"locked cost time %u-%u s, lock count[%llu],last update time %s",
                    i,i+1,
                    hcc->dev_flow_ctrl.flow_info[i].count
                    ,buff_t);
       }
    }
    HWIFI_UNION_PRINT(log, "\n");
}

void hcc_dev_excpt_info_dump(struct hcc_handler  *hcc, hwifi_union_log* log)
{
    HWIFI_UNION_PRINT(log,"WiFi exception %s\n",  exception_enable?"enabled":"disabled");
    if(hcc->excp_count)
    {
        HWIFI_UNION_PRINT(log,"Had occured %u wifi exceptions\n", hcc->excp_count);
        hi110x_print_cpu_time_str(hcc->excp_cpu_time_first,
                            log->loglevel, "First exception occured at ");
        if(hcc->excp_first_func)
        {
            HWIFI_UNION_PRINT(log, " First exception submit by %pF \n",hcc->excp_first_func);
        }
        hi110x_print_cpu_time_str(hcc->excp_cpu_time_last,
                    log->loglevel, "Last  exception occured at ");
        if(hcc->excp_last_func)
        {
            HWIFI_UNION_PRINT(log, " Last exception  submit by %pF \n",hcc->excp_last_func);
        }
    }
    else
    {
        HWIFI_UNION_PRINT(log,"No WiFi Exceptions, WiFi driver work pretty good.");
    }
    if(hcc->dev_flow_ctrl.cpu_timeout_time)
    {
        HWIFI_UNION_PRINT(log, "Device flowctrl maybe deadlock before, please check sdt log\n");
        hi110x_print_cpu_time_str(hcc->dev_flow_ctrl.cpu_timeout_time,
                    log->loglevel, "    Last flowctrl  timeout at ");
    }
    HWIFI_UNION_PRINT(log,"\n");
    if(hcc->net_work_detect.driver_state)
    {
        /*wifi net work exception detected.*/
        HWIFI_UNION_PRINT(log,"WiFi Net work exception:\n");
        HWIFI_UNION_PRINT(log,"  recovery_count=%d\n", hcc->net_work_detect.driver_state);
        if(hcc->net_work_detect.first_time)
            hi110x_print_cpu_time_str(hcc->net_work_detect.first_time, log->loglevel, "First net work issure at");
        if(hcc->net_work_detect.last_time)
            hi110x_print_cpu_time_str(hcc->net_work_detect.last_time, log->loglevel, "First net work issure at");
        HWIFI_UNION_PRINT(log,"\n");
    }
}

void hcc_trans_stat_dump(struct hcc_handler  *hcc, hwifi_union_log* log)
{
    int i,j;
    unsigned long total_len = 0;
    struct hi110x_sdio* hi_sdio = NULL;
    hi_sdio = bus_to_sdio(hcc->io_dev);
    HWIFI_UNION_PRINT(log,"hcc debug information dump:\n");
    HWIFI_UNION_PRINT(log,"tx assem info:\n");
    for(i = 0;i < sizeof(hcc->assem_tx_count)/sizeof(uint32);i++)
    {
        HWIFI_UNION_PRINT(log,"[%s][%2.2d][%d]\n", i?"a":"x", i, hcc->assem_tx_count[i]);
        total_len += (i?:1)*hcc->assem_tx_count[i];
    }
    HWIFI_UNION_PRINT(log,"tx total:%lu\n",total_len);
    total_len = 0;
    HWIFI_UNION_PRINT(log,"rx assem info:\n");
    for(i = 0;i < sizeof(hcc->assem_rx_count)/sizeof(uint32);i++)
    {
        HWIFI_UNION_PRINT(log,"[%s][%2.2d][%d]\n",i?"a":"x", i, hcc->assem_rx_count[i]);
        total_len += (i?:1)*hcc->assem_rx_count[i];
    }
    HWIFI_UNION_PRINT(log,"rx total:%lu\n\n",total_len);

    HWIFI_UNION_PRINT(log,"rx & tx queues status:\n");
    HWIFI_UNION_PRINT(log,"%-30s %10s  %3s %10s %10s\n", "QUEUE", "LEN","FLOWCTRL","LO_WLINE","HI_WLINE");
    for(i = 0;i < HCC_QUEUE_COUNT;i++)
    {
        HWIFI_UNION_PRINT(log,"%-30s [%10u]  %4s %10d %10d\n",
                    hcc_get_queue_str(i),
                    skb_queue_len(&hcc->hcc_queues[HCC_RX].queues[i].data_queue),
                    hcc->hcc_queues[HCC_RX].queues[i].flow_ctrl.enable?"CTRL":"N/A",
                    hcc->hcc_queues[HCC_RX].queues[i].flow_ctrl.lo_waterline,
                    hcc->hcc_queues[HCC_RX].queues[i].flow_ctrl.hi_waterline);
    }
    HWIFI_UNION_PRINT(log,"rx:wait count:%d\n",
                    atomic_read(&hcc->hcc_queues[HCC_RX].wait_count));
    HWIFI_UNION_PRINT(log, "rx:total count:%d\n",
                     atomic_read(&hcc->hcc_queues[HCC_RX].count));
    HWIFI_UNION_PRINT(log,"\n");
    HWIFI_UNION_PRINT(log,"%-30s %10s  %3s %s %s %10s %10s\n", "QUEUE","LEN", "STAT","FLOWCTRL","LO_WLINE","HI_WLINE","waitcount");
    for(i = 0;i < HCC_QUEUE_COUNT;i++)
    {
        HWIFI_UNION_PRINT(log,"%-30s [%10u]  %3s %4s %10d %10d %10d\n",
                    hcc_get_queue_str(i),
                    skb_queue_len(&hcc->hcc_queues[HCC_TX].queues[i].data_queue),
                    __netif_subqueue_stopped(hcc->hi110x_dev->netdev, i)?"OFF":"ON",
                    hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.enable?"CTRL":"N/A",
                    hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.lo_waterline,
                    hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.hi_waterline,
                    hcc_queue_flow_ctrl_len( hcc, i, HCC_TX));
    }
    HWIFI_UNION_PRINT(log,"tx:wait count:%d\n\n",
                    atomic_read(&hcc->hcc_queues[HCC_TX].wait_count));
    HWIFI_UNION_PRINT(log, "tx:total count:%d\n",
                     atomic_read(&hcc->hcc_queues[HCC_TX].count));
    HWIFI_UNION_PRINT(log,"hcc channel stat:\n");
    HWIFI_UNION_PRINT(log,"hcc_tx_count \t[%d]\n",
                    atomic_read(&hcc->debug.hcc_tx_count));
    HWIFI_UNION_PRINT(log,"hcc_single_count \t[%d]\n",
                    atomic_read(&hcc->debug.hcc_single_count));
    HWIFI_UNION_PRINT(log,"hcc_assem_count \t[%d]\n\n",
                    atomic_read(&hcc->debug.hcc_assem_count));
    HWIFI_UNION_PRINT(log,"\ntx queues count\n");
    total_len = 0;
    for(i=0; i<HCC_QUEUE_COUNT; i++)
    {
         int len = atomic_read(&hcc->hcc_queues[HCC_TX].queues[i].queue_count);
         HWIFI_UNION_PRINT(log,"%-30s  \tqlen[%u]\n",
                          hcc_get_queue_str(i),
                          len);
         total_len += len;
    }
    HWIFI_UNION_PRINT(log,"tx queues total count[%lu]\n",total_len);
    total_len = 0;
    HWIFI_UNION_PRINT(log,"\nrx queues count\n");
    for(i=0; i<HCC_QUEUE_COUNT; i++)
    {
         int len = atomic_read(&hcc->hcc_queues[HCC_RX].queues[i].queue_count);
         HWIFI_UNION_PRINT(log,"%-30s  \tqlen[%u]\n",
                          hcc_get_queue_str(i),
                          len);
         total_len += len;
    }
    HWIFI_UNION_PRINT(log,"rx queues total count[%lu]\n", total_len);
    /*dump_kernel_interrupts(loglevel);*/

    for(j=0; j < HCC_DIR_COUNT; j++)
    {
        unsigned long long total_drop = 0;
        HWIFI_UNION_PRINT(log,"\n%s queues drop count\n",
                                    hcc_get_dir_str(j));
        for(i=0; i<HCC_QUEUE_COUNT; i++)
        {
            if(hcc->hcc_queues[j].queues[i].drop_count)
            {
                total_drop += hcc->hcc_queues[j].queues[i].drop_count;
                HWIFI_UNION_PRINT(log,"%-30s  \t: %llu\n",
                                hcc_get_queue_str(i),
                                hcc->hcc_queues[j].queues[i].drop_count);
            }
        }
        HWIFI_UNION_PRINT(log,"\n%s total count:%llu\n",
                            hcc_get_dir_str(j), total_drop);
    }
    HWIFI_UNION_PRINT(log,"\n");

    HWIFI_UNION_PRINT(log, "flow info:current state: %s, on-count: %u, off-count: %u, assemble-reset-count: %u\n",
                                                (hcc_get_dev_flowctrl() == FLOWCTRL_ON ? "ON" : "OFF"),
                                                hcc->dev_flowctrl_on_count,
                                                hcc->dev_flowctrl_off_count,
                                                hcc->dev_assemble_reset_count);

    HWIFI_UNION_PRINT(log,"wifi debug level:[loglevel=%u]\n",
                 loglevel);
    HWIFI_UNION_PRINT(log,"flow ctrl stat: %s, on-count: %u, off-count: %u, assemble-reset-count: %u\n",
                (hcc_get_dev_flowctrl() == FLOWCTRL_ON ? "ON" : "OFF"),
                hcc->dev_flowctrl_on_count,
                hcc->dev_flowctrl_off_count,
                hcc->dev_assemble_reset_count
                );
    HWIFI_UNION_PRINT(log,"chan stat: %s \n", hcc_get_chan_stat_str(atomic_read(&hcc->hcc_chan_status)));

    if(hi_sdio)
        HWIFI_UNION_PRINT(log,"hcc sched transfer policy [%d] \n",
                    hcc->hcc_transfer_task->policy);
    HWIFI_UNION_PRINT(log,"\n");
}

STATIC int hwifi_hcc_log_dump(hwifi_union_log* log)
{
    struct hcc_handler* hcc = (struct hcc_handler*)log->data;
    if(!hcc)
    {
        HWIFI_UNION_PRINT(log, "hcc is null!");
        return 0;
    }
    hcc_debug_peak_show(hcc, log);
    hcc_dev_excpt_info_dump(hcc, log);
    hcc_dev_flow_ctrl_info_dump(hcc, log);
    hcc_trans_stat_dump(hcc, log);
    hcc_action_count_dump(hcc, log);
    return 0;
}

STATIC hwifi_union_log hwifi_hcc_log =
{
    .module = "hcc",
    .cb = hwifi_hcc_log_dump
};

STATIC void hcc_queue_water_line_init(struct hcc_handler      *hcc)
{
    int i = 0;
    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.hi_waterline = 256;
        hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.lo_waterline = 128;
    }

    /*special wl define*/
    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_BK_QUEUE].flow_ctrl.lo_waterline =  15;
    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_BK_QUEUE].flow_ctrl.hi_waterline =  30;

    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_BE_QUEUE].flow_ctrl.lo_waterline =  20;
    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_BE_QUEUE].flow_ctrl.hi_waterline =  40;

    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_VI_QUEUE].flow_ctrl.lo_waterline =  30;
    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_VI_QUEUE].flow_ctrl.hi_waterline =  60;

    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_VO_QUEUE].flow_ctrl.lo_waterline =  40;
    hcc->hcc_queues[HCC_TX].queues[DATA_UDP_VO_QUEUE].flow_ctrl.hi_waterline =  80;
}

int hcc_tx_tcp_ack_filter(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data)
{
        struct sk_buff_head  *head = (struct sk_buff_head  *)data;
        int ret = hwifi_perform_tx_tcp_ack_filter(hcc, type, dir, data);
        if(skb_queue_len(head))
        {
            /*If have tcp ack, we update the timer*/
            hcc_net_work_state_detect(hcc);
        }
        return ret;
}

int hwifi_tcp_data_bt_opt_filter(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data)
{
    struct sk_buff_head  *head = (struct sk_buff_head  *)data;
		struct sk_buff * skb;

    if(!skb_queue_len(head))
    {
        return 0;
    }

    if(!hcc->bt_opt.item[dir].enable)
        return 0;

    if(HI_MODE_STA != hcc->hi110x_dev->mode.current_mode)
        return 0;

    /*Protect the chariot action,not good but useful*/
	skb = skb_peek(head);
	if (skb->len < 1000)
	{
		return 0;
	}

    /*If Tcp Data came, we think it's a tcp connect and send bt opt wid to device
      If we lost the connect 1.5s, we notify the device, support tx/rx config.*/
    spin_lock_bh(&hcc->bt_opt.lock);
    if(hcc->bt_opt.stat != 0x10)
    {
        /*First tcp data, send wid to device*/
        hcc->bt_opt.stat = 0x10;
        hwifi_set_bt_opt(hcc->hi110x_dev->cfg, hcc->bt_opt.stat);
    }
    mod_timer(&hcc->bt_opt.item[dir].bt_opt_timer, jiffies + hcc->bt_opt.item[dir].timeout);
    spin_unlock_bh(&hcc->bt_opt.lock);
    return 0;
}

int hcc_tx_tcp_data_count(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data)
{
        struct sk_buff_head  *head = (struct sk_buff_head  *)data;
        if(skb_queue_len(head))
        {
            /*If have tcp data, we update the timer*/
            hcc_net_work_state_detect(hcc);
            hwifi_tcp_data_bt_opt_filter(hcc, type, dir, data);
        }
        return SUCC;
}

int hwifi_rx_tcp_data_filter(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data)
{
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!data);
    HWIFI_BUG_ON(DATA_TCP_DATA_QUEUE!= type);
    HWIFI_BUG_ON(HCC_RX != dir);
    hwifi_tcp_data_bt_opt_filter(hcc, type, dir, data);

    return 0;
}

STATIC void hcc_bt_opt_init(struct hcc_handler   *hcc)
{
    int i = 0;
    for(i = 0; i < HCC_DIR_COUNT; i++)
    {
        init_timer(&hcc->bt_opt.item[i].bt_opt_timer);
        hcc->bt_opt.item[i].dir_type = i;
        hcc->bt_opt.item[i].hcc = hcc;
        hcc->bt_opt.item[i].bt_opt_timer.data = (unsigned long)&hcc->bt_opt.item[i];
        hcc->bt_opt.item[i].bt_opt_timer.function = (void*)hcc_bt_opt_tcp_data_timeout_isr;
        hcc->bt_opt.item[i].enable = FALSE;
        /*default 3 seconds*/
        hcc->bt_opt.item[i].timeout = msecs_to_jiffies(3000);
    }

    spin_lock_init(&hcc->bt_opt.lock);

    /*Tx OFF, Rx On*/
    hcc->bt_opt.item[HCC_RX].enable = TRUE;
}

STATIC void hcc_bt_opt_exit(struct hcc_handler   *hcc)
{
    int i = 0;
    for(i = 0; i < HCC_DIR_COUNT; i++)
    {
        del_timer_sync(&hcc->bt_opt.item[i].bt_opt_timer);
    }
}

/*
 * Prototype    : hcc_alloc
 * Description  : alloc hcc struct and init
 * Input        : struct hi110x_device *hi110x_dev
 * Output       : None
 * Return Value : struct hcc_handle*
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/21
 *     Author       :
 *     Modification : Created function
 *
 */
struct hcc_handler* hcc_alloc(struct hi110x_device *hi110x_dev, struct iodevice *io_dev)
{
    struct hcc_handler      *hcc;
    uint32                   i;
#ifdef WLAN_PERFORM_OPT
    int32                    ret;
#endif

    HWIFI_ASSERT((NULL != hi110x_dev) || (NULL != io_dev));

    hcc = kzalloc(sizeof(struct hcc_handler), GFP_KERNEL);
    if (NULL == hcc)
    {
        HWIFI_ERROR("Failed to alloc hcc memory!");
        return NULL;
    }

    hcc->hi110x_dev = hi110x_dev;
    hcc->io_dev = io_dev;

    atomic_set(&hcc->rx_seq, 0);
    atomic_set(&hcc->tx_seq, 0);

    hcc->tcp_down_quotiety    = 2;
    hcc->tcp_up_quotiety      = 2;
    hcc->sys_hcc_dyn_pri_enable = 0x1;
    hcc->sys_hcc_dyn_pri_mask =   0;
    hcc->sys_tcp_rx_ack_opt_enable = TRUE;
    hcc->sys_tcp_tx_ack_opt_enable = TRUE;
    hcc->sys_hcc_rx_opt_enable  = TRUE;
    hcc->sys_hcc_tx_assem_opt_enable = TRUE;

#ifdef WMM_OPT_FOR_AUTH
    hcc->sys_wmm_ac_opt_enable = hwifi_get_param_int(HISI_CONFIG_WIFI_WMM_AC_OPT);


    init_timer(&hcc->wmm_info.wmm_auth_timer);
    hcc->wmm_info.wmm_auth_timer.data   = (unsigned long)hcc;
    hcc->wmm_info.wmm_auth_timer.function   = (void *)hcc_wmm_auth_timeout_isr;
#endif
#ifdef HCC_DEBUG
    hcc->debug.peak_debug= peak_debug;
    hcc_debug_init(hcc);
    if(hcc_debug_peak_stat_init(hcc))
    {
        kfree(hcc);
        return NULL;
    }
#endif

    hcc_trans_limit_parm_init(hcc);
    for(i = 0; i< HCC_DIR_COUNT; i++)
    {
        spin_lock_init(&hcc->hcc_queues[i].flow_ctrl_lock);
    }
    /*Init hcc flow ctrl handle functions*/
    hcc->hcc_queues[HCC_TX].flow_ctrl_handle = hcc_queues_flow_ctrl_tx_handle;
    hcc->hcc_queues[HCC_RX].flow_ctrl_handle = hcc_queues_flow_ctrl_rx_handle;
    for (i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        skb_queue_head_init(&hcc->hcc_queues[HCC_TX].queues[i].data_queue);
        /* Start of zhangwei 64406 2013-03-26 B295 增加同步保护机制 */
        spin_lock_init(&hcc->hcc_queues[HCC_TX].queues[i].data_queue_lock);
        /* End of zhangwei 64406 2013-03-26 B295 增加同步保护机制 */
        hcc->hcc_queues[HCC_TX].queues[i].filter = NULL;

        /*hcc rx queue init*/
        skb_queue_head_init(&hcc->hcc_queues[HCC_RX].queues[i].data_queue);
        spin_lock_init(&hcc->hcc_queues[HCC_RX].queues[i].data_queue_lock);
        hcc->hcc_queues[HCC_RX].queues[i].filter = NULL;
        hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.enable = TRUE;
        hcc->hcc_queues[HCC_RX].queues[i].flow_ctrl.enable = TRUE;
        atomic_set(&hcc->hcc_queues[HCC_RX].queues[i].flow_ctrl.wait_count, 0);
    }

    /*the rx queues:CTRL_QUEUE DATA_HI_QUEUE did not under hcc flow control*/
    hcc->hcc_queues[HCC_TX].queues[CTRL_QUEUE].flow_ctrl.enable = FALSE;
    hcc->hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].flow_ctrl.enable = FALSE;
    hcc_queue_water_line_init(hcc);

#ifdef WLAN_PERFORM_OPT
    hcc->hcc_queues[HCC_RX].filter_info.ack_limit = hwifi_get_param_int(HISI_CONFIG_WIFI_RX_ACK_LIMIT);
    hcc->hcc_queues[HCC_TX].filter_info.ack_limit = hwifi_get_param_int(HISI_CONFIG_WIFI_TX_ACK_LIMIT);

    hcc->hcc_queues[HCC_RX].queues[DATA_TCP_DATA_QUEUE].filter = hwifi_rx_tcp_data_filter;
    hcc->hcc_queues[HCC_RX].queues[DATA_TCP_ACK_QUEUE].filter = hwifi_perform_rx_tcp_ack_filter;
    hcc->hcc_queues[HCC_TX].queues[DATA_TCP_ACK_QUEUE].filter = hcc_tx_tcp_ack_filter;
    hcc->hcc_queues[HCC_TX].queues[DATA_TCP_DATA_QUEUE].filter = hcc_tx_tcp_data_count;

    /*net work exception detect*/
#endif
#if 0
    for (i = DATA_UDP_BK_QUEUE; i < HCC_QUEUE_COUNT; i++)
    {
        hcc->hcc_queues[HCC_RX].queues[i].filter = hwifi_perform_wmm_ac_filter_reg;
        hcc->hcc_queues[HCC_TX].queues[i].filter = hwifi_perform_wmm_ac_filter_reg;
    }
#endif
    mutex_init(&hcc->rx_split_lock);
    mutex_init(&hcc->tx_transfer_lock);
    spin_lock_init(&hcc->assem_c_lock);
    spin_lock_init(&hcc->tx_skb_wake_lock);
    skb_queue_head_init(&hcc->tx_queue);

    init_waitqueue_head(&hcc->wait_queue);

    hcc_rx_actions_init(hcc);

    hcc->dev_flow_ctrl.flow_ctrl_off_time = jiffies;
    hcc->dev_flow_ctrl.flow_ctrl_on_time = jiffies;
    init_timer(&hcc->dev_flow_ctrl.flow_timer);

    hcc->dev_flow_ctrl.flow_timer.data = (unsigned long)hcc;
    hcc->dev_flow_ctrl.flow_timer.function = (void*)hcc_dev_flow_ctrl_timeout_isr;
    hcc->dev_flow_ctrl.timeout = HCC_DEV_FLOW_OFF_TIMEOUT;
    /*when this is enable , we force to open dev flow ctrl when timeout.*/
    hcc->dev_flow_ctrl.force_ctrl_on = FALSE;

    hcc_bt_opt_init(hcc);

    atomic_set(&hcc->hcc_chan_status, hcc_stop);

    atomic_set(&hcc->hcc_queues[HCC_TX].wait_count, 0);
    atomic_set(&hcc->hcc_queues[HCC_RX].wait_count, 0);
    atomic_set(&hcc->hcc_queues[HCC_TX].count, 0);
    atomic_set(&hcc->hcc_queues[HCC_RX].count, 0);
    hcc->assembled_cnt = 0;
    hcc_assemble_count = MIN(hcc_assemble_count, (uint32)HCC_MAX_ASSEMBLE_NUM);
    HWIFI_PERFORM("Hcc max assemble count %u", hcc_assemble_count);

    hcc->tx_buf_size = MIN(bus_get_max_transfer_size(io_dev), (uint32)TXRX_BUF_MAX_SIZE);
    HWIFI_INFO("Hcc max tx buf size %u", hcc->tx_buf_size);
    hwifi_union_log_register(&hwifi_hcc_log, (void*)hcc);
    hcc->tx_buf = (uint8 *)kzalloc(hcc->tx_buf_size, GFP_KERNEL);
    if (NULL == hcc->tx_buf)
    {
        HWIFI_WARNING("Failed to alloc tx buf memory!");
        mutex_destroy(&hcc->tx_transfer_lock);
        mutex_destroy(&hcc->rx_split_lock);
        kfree(hcc);
        return NULL;
    }
#ifdef WLAN_PERFORM_OPT
    ret = hwifi_perform_init_filter_tcp_ack_pool(hcc);
    if (ret < 0)
    {
        HWIFI_ERROR("Failed to init tcp ack pool!");
#ifdef HCC_DEBUG
        hcc_debug_peak_stat_exit(hcc);
#endif
        kfree(hcc->tx_buf);
        mutex_destroy(&hcc->tx_transfer_lock);
        mutex_destroy(&hcc->rx_split_lock);
        kfree(hcc);
        return NULL;
    }
#endif

    if ( init_dev_excp_handler(hcc)!= 0)
    {
        HWIFI_ERROR("Failed to init dev error param!");
        deinit_dev_excp_handler(hcc);
#ifdef HCC_DEBUG
        hcc_debug_peak_stat_exit(hcc);
#endif

    #ifdef WLAN_PERFORM_OPT
        hwifi_perform_deinit_list(hcc);
    #endif

        kfree(hcc->tx_buf);
        mutex_destroy(&hcc->tx_transfer_lock);
        mutex_destroy(&hcc->rx_split_lock);
        kfree(hcc);

        return NULL;
    }

    /*Init transfer wait event.*/
    init_waitqueue_head(&hcc->transfer_wait_queue);
    /*创建发送线程*/
    hcc->hcc_transfer_task = hi110x_thread_create(hcc_trans_thread,
                        (struct hcc_handler*)hcc,
                       NULL,
                        "hcc_trans_thread",
                        HI110X_HCC_TRANS_THREAD_POLICY,
                        HCC_TRANS_THERAD_PRIORITY, g_lCpuId);
    if (NULL == hcc->hcc_transfer_task)
    {
#ifdef HCC_DEBUG
        hcc_debug_peak_stat_exit(hcc);
#endif

        deinit_dev_excp_handler(hcc);

        HWIFI_ERROR("Failed to create sdio_tx_tsk thread!");
        return NULL;
    }

    spin_lock_init(&hcc->hcc_comm_wq.lock);
    hcc->hcc_comm_wq.wq= create_singlethread_workqueue("hcc_comm_wq");
    if (!hcc->hcc_comm_wq.wq)
    {
#ifdef HCC_DEBUG
            hcc_debug_peak_stat_exit(hcc);
#endif
            HWIFI_ERROR("Failed to create hcc_comm_wq!");
            kfree(hcc->tx_buf);
            mutex_destroy(&hcc->tx_transfer_lock);
            mutex_destroy(&hcc->rx_split_lock);
            kfree(hcc);

        #ifdef WLAN_PERFORM_OPT
            hwifi_perform_deinit_list(hcc);
        #endif

            deinit_dev_excp_handler(hcc);

            return NULL;
    }

    spin_lock_init(&hcc->excp_lock);

    assem_debug_count_reset(hcc);
    hcc_net_stream_init(hcc);
    hcc_net_work_detect_init(hcc);

    return hcc;
}

/*
 * Prototype    : hcc_release
 * Description  : release hcc
 * Input        : struct hcc_handle*
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/1
 *     Author       :
 *     Modification : Created function
 *
 */
void hcc_release(struct hcc_handler* hcc)
{
    hcc_net_work_detect_deinit(hcc);
    hcc_net_stream_deinit(hcc);
    destroy_workqueue(hcc->hcc_comm_wq.wq);
    hcc->hcc_comm_wq.wq = NULL;

    if (hcc->hcc_transfer_task != NULL)
    {
        hi110x_thread_stop(hcc->hcc_transfer_task, NULL);
        hcc->hcc_transfer_task = NULL;
    }

    deinit_dev_excp_handler(hcc);

#ifdef WLAN_PERFORM_OPT
    hwifi_perform_deinit_list(hcc);
#endif
#ifdef HCC_DEBUG
    hcc_debug_peak_stat_exit(hcc);
#endif
    atomic_set(&hcc->hcc_chan_status, hcc_stop);
    kfree(hcc->tx_buf);
    hcc->tx_buf = NULL;
    hcc_dev_flowctr_timer_del(hcc);
#ifdef WMM_OPT_FOR_AUTH
    del_timer_sync(&hcc->wmm_info.wmm_auth_timer);
#endif
    del_timer_sync(&hcc->dev_flow_ctrl.flow_timer);
    hcc_bt_opt_exit(hcc);
    /* clear queues */
    hcc_rx_queue_all_reset(hcc, TRANS_LOCK);
    hcc_tx_queue_all_reset(hcc, TRANS_LOCK);

    skb_queue_purge(&hcc->tx_queue);
    mutex_destroy(&hcc->tx_transfer_lock);
    mutex_destroy(&hcc->rx_split_lock);
    kfree(hcc);
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

