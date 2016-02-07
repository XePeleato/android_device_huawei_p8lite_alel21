

#ifndef __HWIFI_HCC_H__
#define __HWIFI_HCC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include <linux/wait.h>         /* wait_queue_head_t */
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include "hwifi_bus.h"
#include "hsdio.h"
#include "hi110x.h"

/*
 * 2 Macro Definition
 */
#define ASSEMBLE_RESET_DISABLE          0
#define ASSEMBLE_RESET_ENABLE           1

#define TX_FLOWCTL_QUE                  1       /* 定义发送队列1为受流控影响 */
#define HCC_MAX_HEAD_SIZE               24

#define HCC_MAX_ASSEMBLE_NUM            16
#define HCC_DEFAULT_ASSEMBLE_NUM        8
#define HCC_ASSEMBLE_INFO_SIZE          16

#define TXRX_BUF_MAX_SIZE               (32*1024)

/* type define
*sync with hcc_action_str in hwifi_hcc.c*/
typedef enum _hcc_action_type__
{
    HCC_TYPE_DATA = 0,
    HCC_TYPE_HCC_CTRL = 1,
    HCC_TYPE_BEATHEART = 2,
    HCC_TYPE_WIFI_CTRL = 3 ,
    HCC_TYPE_OAM = 6,
    HCC_TYPE_TEST= 7,
    HCC_TYPE_DEV_ERROR = 8,
    HCC_TYPE_UNKOWN,
    HCC_ACTION_COUNT
}hcc_action_type;

#define HCC_SUBTYPE_ANY                 0xFF
#define HCC_TYPE_MAX                    (HCC_TYPE_UNKOWN)

#define MAIN_TYPE_BITS                  3
#define SUB_TYPE_BITS                   5
#define AGGRINFO_BITS                   0
#define HCC_MK_TYPE(main, sub)          ((((main) & ((1 << MAIN_TYPE_BITS) - 1)) << SUB_TYPE_BITS) | ((sub) & ((1 << SUB_TYPE_BITS) - 1)))
#define HCC_MAIN_TYPE(type)             (((type) >> SUB_TYPE_BITS) & ((1 << MAIN_TYPE_BITS) - 1))
#define HCC_SUB_TYPE(type)              ((type) & ((1 << SUB_TYPE_BITS) - 1))
#define HCC_HAVE_AGGRINFO(info)         ((info) & (1 << AGGRINFO_BITS))

#define HCC_TX_HI_WATERLINE             (256)
#define HCC_TX_LO_WATERLINE             (HCC_TX_HI_WATERLINE/2)
#define HCC_RX_HI_WATERLINE                    (2000)
#define HCC_RX_LO_WATERLINE                    (1000)

/*the rx flow ctrl timeout (ms)*/
#define HCC_RX_WL_TIMEOUT               (5000)

#define SINGLE_PACKET_MAX_SIZE     1536U

#define HCC_PACKET_ALIGN_LEN       (8)
#define HCC_SKB_TAIL_ALGIN_LEN      HCC_PACKET_ALIGN_LEN

#define HCC_IDLE_PERIOD         (msecs_to_jiffies(10))
#define HCC_NORMAL_PERIOD      (0)
#define HCC_REST_PERIOD         (1)
#define HCC_MAX_TIMEOUT_THRESHOLD   (HZ)
#define IPV4_VERSION              0x40
#define IPV6_VERSION              0x60

#ifdef WMM_OPT_FOR_AUTH
#define WMM_AC_TIMEOUT          1000
#define WMM_AUTH_TIMEOUT        10
#endif
/*
 * 3 Enum Type Definition
 */
 /*If you add a queue ,
 please modify hcc_queue_str in hwifi_hcc.c at the same time*/
typedef enum _hcc_queue_type_
{
    CTRL_QUEUE      = 0,
    DATA_HI_QUEUE,
    DATA_LO_QUEUE,
    DATA_TCP_DATA_QUEUE,
    DATA_TCP_ACK_QUEUE,
#ifdef WMM_OPT_FOR_AUTH
    DATA_UDP_BK_QUEUE,
    DATA_UDP_BE_QUEUE,
    DATA_UDP_VI_QUEUE,
    DATA_UDP_VO_QUEUE,
#endif
    HCC_QUEUE_COUNT
}hcc_queue_type;
#define NET_TX_QUEUES                   (HCC_QUEUE_COUNT)       /* 定义HCC_QUEUE_COUNT个网络发送队列 */
#define NET_RX_QUEUES                   1       /* 定义1个网络接收队列 */

typedef enum _hwifi_net_stream_type_{
    STREAM_UDP_VO = 0,
    STREAM_UDP_VI,
    STREAM_UDP_RTP,
    STREAM_UDP_RTCP,
    STREAM_COUNT
}hwifi_net_stream_type;

/*refer to net_stream_sm_str*/
typedef enum _hwifi_net_stream__sm_
{
    HNET_STREAM_DISCONN,
    HNET_STREAM_CONNTING,/*Try to connect stat*/
    HNET_STREAM_CONNETED,
    HNET_STREAM_SM_COUNT
}hwifi_net_stream_sm;

typedef struct _hwifi_net_stream_item_
{
    char* name;
    hwifi_net_stream_type index;
    /*1:don't care about stream,0: should do something with it.*/
    int ignored;

    hwifi_net_stream_sm sm_stat;
    unsigned long  sm_pkt_count;
    unsigned long  connecting_period;
    unsigned long  disconn_period;

    unsigned long connecting_pkts_limit;
    unsigned long disconn_percent_limit;

    unsigned long sm_tx_pack;

    /*the packages when connecting, when disconnect we clean it.*/
    unsigned long  pkt_count;

    /*start when stream connect.*/
    struct timer_list timer;
}hwifi_net_stream_item;

typedef struct _hwifi_net_stream_
{
    /*stream must be the first item*/
    hwifi_net_stream_item stream[STREAM_COUNT];

    spinlock_t lock;
}hwifi_net_stream;

#ifdef WMM_OPT_FOR_AUTH
typedef enum _ip_pkt_prior_
{
    TX_AC_BK,
    TX_AC_BE,
    TX_AC_VI,
    TX_AC_VO,
    RX_AC_BK,
    RX_AC_BE,
    RX_AC_VI,
    RX_AC_VO,
    AC_COUNT
} ip_pkt_prior;
#endif

typedef enum _ac_user_pro_
{
    AC_BK,
    AC_BE,
    AC_VI,
    AC_VO,
    AC_NUMBER
}ac_user_pro;

/*refer to hcc_dir_stat_str in hwifi_hcc.c*/
typedef enum _hcc_chan_type_
{
    HCC_TX,
    HCC_RX,
    HCC_DIR_COUNT
}hcc_chan_type;

enum
{
   HCC_TX_HI_SUBTYPE = 1,
   HCC_SUBTYPE_COUNT
};

typedef enum _change_sched_mode_
{
    CHANGE_DATA_PRIOR_TO_ACK = 1,
    CHANGE_ACK_PRIOR_TO_DATA,
    CHANGE_NOTHING
}change_sched_mode;

typedef struct _hcc_net_work_detect_
{
    /*whether we need this detect.*/
    int enable;

    /*record the first net work exception time 
        and the last one*/
    unsigned long long first_time;
    unsigned long long last_time;

    /*timeout for net work detect*/
    unsigned long timeout;
    
    int  driver_state;
    struct timer_list detect_timer;
}hcc_net_work_detect;


/*
 * 4 Global Variable Declaring
 */
extern volatile uint32 g_flowctrl_flag;
extern volatile uint32 g_assemble_reset_flag;
extern int intr_mode;

/*
 * 5 Message Header Definition
 */

/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */

/* hcc protocol header */
struct hcc_header
{
    uint8      seq;            /* sequence number */
    uint8      type;           /* main type[7..5] sub type[4..0] */
    uint16     more   :1;      /* if set 1, have more pkt */
    uint16     option :1;      /* if option=1,
                                  dev->host,mean pkt have next_pkt_len field,
                                  host->dev,mean pkt have assemble_info field */
    uint16     pad    :2;
    uint16     pay_len:12;    /* LOW MSB */
};

struct hi110x_device;

typedef int (* hcc_rx_cb_func)(unsigned char mtype, unsigned char stype, struct sk_buff *skb, void *context);
typedef int (* hcc_trans_cb_func)(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data);
typedef void (*hcc_trans_flowctrl_handle_cb)(struct hcc_handler* hcc,  hcc_queue_type type,  hcc_chan_type dir,  bool reduce,int i);
//typedef void (*hcc_sched_transfer_cb)(struct hcc_handler* hcc, hcc_sched_type type,int sched_count);
struct hcc_rx_action
{
    struct mutex        mutex_lock;
    int32               registered;
    hcc_rx_cb_func      rx_cb;
    void               *context;
};
/*indicate the direction, host  between device*/
typedef enum _hwifi_trans_flow_dir_
{
        HWIFI_DEV2HOST,/*device to host*/
        HWIFI_HOST2DEV,/*host to device*/
        HWIFI_FLOW_DIR_COUNT
}hwifi_trans_flow_dir;

/*hcc state enum, sync with hcc_chan_stat_str*/
typedef enum _hcc_state_{
    hcc_stop = 0,
    hcc_running,
    hcc_sleeping,
    hcc_abort,
    hcc_valid,
}hcc_state;
#define HCC_QUEUE_PEAK_STAT_COUNT   (4096)
#define HCC_QUEUE_PEAK_STAT_LEN     (sizeof(unsigned long)*HCC_QUEUE_PEAK_STAT_COUNT)
typedef struct _hcc_flow_ctrl_
{
    /*high waterline & low waterline*/
    int32       hi_waterline;
    int32       lo_waterline;
    /*each queue had kernel net flow contrl count, except enable=0*/
    atomic_t                     wait_count;
    /*enable: indicate this queue,
      1:ctrl by flow ctrl 0: not under ctrl*/
    bool                        enable;
}hcc_flow_ctrl;
typedef struct _hcc_trans_queue_
{
    hcc_flow_ctrl       flow_ctrl;

    struct sk_buff_head         data_queue;
    spinlock_t                  data_queue_lock;
#ifdef HCC_DEBUG
    atomic_t                    queue_count;
    unsigned long *             queue_peak_count;
#endif
    uint32                queue_trans_limit;
    hcc_trans_cb_func           filter;
    /*the skb number we dropped by filter*/
    unsigned long long          drop_count;
}hcc_trans_queue;

#ifdef WLAN_PERFORM_OPT
#define HCC_TCP_STREAM  20
typedef struct _wlan_perform_tcp_impl_
{
    unsigned long long drop_count;
    unsigned long long send_count;
    unsigned long long dup_ack_count;
}wlan_perform_tcp_impl;

typedef struct _wlan_perform_tcp_impls_
{
    int32 ack_limit;
    unsigned long long ignored_count;
    wlan_perform_tcp_impl tcp_info[HCC_TCP_STREAM];
}wlan_perform_tcp_impls;

struct skb_tcp_cb
{
    int     index;
    uint32  tcp_type;
    uint32  tcp_ack_no;
};

struct wlan_perform_tcp
{
    /*hcc rx/tx tcp ack param*/
    struct     sk_buff_head hcc_ack_queue[HCC_TCP_STREAM];
    int32      hcc_ack_count[HCC_TCP_STREAM];               /* ack num in each queue */
    int32      hcc_duplicate_ack_count[HCC_TCP_STREAM];
    uint32     hcc_tcp_ack_no[HCC_TCP_STREAM];
    spinlock_t hcc_tcp_ack_lock;
#ifdef HCC_DEBUG
    uint32 ack_count[HCC_TCP_STREAM];
    uint32 duplicate_ack_count[HCC_TCP_STREAM];
#endif
    /*TBD filter upline ack later*/
};

struct wlan_tcp_flow
{
    unsigned long src_ip;
    unsigned long dst_ip;
    uint16 src_port;
    uint16 dst_port;
    uint8  protocol;
};

struct tcp_list_node
{
    struct list_head        list;
    uint32                  used;       /* 0-free, 1-used */
    struct wlan_tcp_flow   *buf;
    int32                   index;
    uint32                  last_ts;  /*time of last frame*/
};

struct wlan_perform_tcp_list
{
    struct tcp_list_node    tcp_pool[HCC_TCP_STREAM];
    int32                   free_count;
    spinlock_t              tcp_lock;      /* lock for tcp pool */
    spinlock_t              tcp_list_lock; /* lock for tcp list */
    struct list_head        tcp_list;
};
#endif

typedef struct _hcc_trans_queues_
{
    /*used to flow ctrl*/
    atomic_t                     wait_count;
    /*used to count rx/tx queues total count*/
    atomic_t                     count;
    spinlock_t                   flow_ctrl_lock;
    hcc_trans_flowctrl_handle_cb flow_ctrl_handle;
    wlan_perform_tcp_impls       filter_info;
    hcc_trans_queue              queues[HCC_QUEUE_COUNT];
    unsigned long long           hcc_action_count[HCC_ACTION_COUNT];
}hcc_trans_queues;

typedef struct _hcc_bt_opt_item_
{
    int enable;
    unsigned long timeout;
    hcc_chan_type dir_type;
    struct hcc_handler   *hcc;
    /*the time for bt opt detect, TBD.*/
    struct timer_list           bt_opt_timer;    
}hcc_bt_opt_item;

typedef struct _hcc_bt_opt_
{
    /*The stat indicate the wid stat, 0x11 or 0x10, 
	 0x10 means enable opt, 0x11 means disable*/
    int stat;
    spinlock_t          lock;
    hcc_bt_opt_item item[HCC_DIR_COUNT];
}hcc_bt_opt;

typedef struct _hcc_debug_{
    int      peak_debug;
    atomic_t hcc_tx_count;
    atomic_t hcc_single_count;
    atomic_t hcc_assem_count;
}hcc_debug;

/*when dev flow ctrl locked over 11s, we just give you a warning.*/
#define HCC_DEV_FLOW_INFO_MAX_TRACE_SECONDS (5)
typedef struct _hcc_dev_flow_info_
{
    /*Just For Debug!!*/
    /*how many times the enter this time period*/
    unsigned long long  count;
    /*the newest flow ctrl update time, we did not update the time=0's cpu time,
    because it's too many*/
    unsigned long long cpu_time;
}hcc_dev_flow_info;

#define HCC_DEV_FLOW_OFF_TIMEOUT        (10*HZ)
typedef struct _hcc_dev_flow_ctrl_
{
    unsigned char force_ctrl_on;
    /*the max dev flow ctrl off time,
        over this we try to reopen the dev flowctrl*/
    unsigned long timeout;

    /*The last cpu time flow ctrl timeout*/
    unsigned long long cpu_timeout_time;

    unsigned long flow_ctrl_off_time;
    unsigned long flow_ctrl_on_time;
    /*time for flow ctrl lock too long, device flow ctrl*/
    struct timer_list   flow_timer;
    /*Trace 0s ~ 10s, when over 10s, we increase the 10s's count,
     flow_info's array index the second,
     sample, flow_info[0] indicate  the dev lock the flow ctrl 0~1 s
             flow_info[1] indicate  the dev lock the flow ctrl 1~2 s*/
    hcc_dev_flow_info flow_info[HCC_DEV_FLOW_INFO_MAX_TRACE_SECONDS + 1];
}hcc_dev_flow_ctrl;

#ifdef WMM_OPT_FOR_AUTH
struct wlan_wmm_info
{
    uint32  ac_last_ts[AC_COUNT];       /*维护各优先级帧的最后传输时间*/
    uint32  wmm_ac_drop_flag[AC_COUNT]; /*标识本次是否做丢弃操作*/
    uint8   ac_bitmap;                  /*标识各优先级数据是否存在*/
    uint32  wmm_high_prio_ac_drop_switch; /*高优先级队列丢包开关*/
    uint8   current_ac_cnt;
    struct timer_list   wmm_auth_timer;
};
#endif


/*used to process suspend/resume & exception handler queue*/
typedef struct _hcc_common_queue_
{
    spinlock_t                  lock;
    struct workqueue_struct*     wq;
}hcc_common_queue;

struct hcc_handler
{
    struct hi110x_device       *hi110x_dev;
    struct iodevice            *io_dev;

#ifdef WLAN_PERFORM_OPT
    struct wlan_perform_tcp      hcc_tcp_ack[HCC_DIR_COUNT];

    struct wlan_perform_tcp_list hcc_tcp_ack_list[HCC_DIR_COUNT];

    struct task_struct         *hcc_transfer_task;
    wait_queue_head_t          transfer_wait_queue;

#endif
    /*used to process suspend/resume & exception handler queue,eq.*/
    hcc_common_queue            hcc_comm_wq;

    void*                       excp_first_func;
    void*                       excp_last_func;
    unsigned long long          excp_cpu_time_first;
    unsigned long long          excp_cpu_time_last;
    uint32                      excp_count;
    spinlock_t                  excp_lock;
    struct work_struct          excp_worker;

    /*leacy source code*/
    /*struct workqueue_struct    *tx_workqueue;
    struct work_struct          tx_worker;*/

    struct hcc_rx_action        rx_actions[HCC_TYPE_MAX];
    wait_queue_head_t           wait_queue; /* extern data wait queue */

    hcc_trans_queues            hcc_queues[HCC_DIR_COUNT];

    hcc_bt_opt                  bt_opt;

    uint32                      tcp_down_quotiety;
    uint32                      tcp_up_quotiety;

    struct sk_buff_head         tx_queue;

    uint8                      *tx_buf;
    uint32                      tx_buf_size;

    uint8                       assem_info[HCC_MAX_ASSEMBLE_NUM];
    uint32                      assembled_cnt;

    /*used to protect hcc tx transfer.*/
    struct mutex                tx_transfer_lock;

    /*This lock used to protect the rx skb list split*/
    struct mutex                rx_split_lock;

    /*used to protect skb add queue when hcc is closed*/
    spinlock_t               tx_skb_wake_lock;

    uint32                      assem_tx_count[HCC_MAX_ASSEMBLE_NUM + 1];
    uint32                      assem_rx_count[HCC_MAX_ASSEMBLE_NUM + 1];
    spinlock_t                  assem_c_lock;

#ifdef WMM_OPT_FOR_AUTH
    struct wlan_wmm_info        wmm_info;
    uint32                      sys_wmm_ac_opt_enable;
#endif
    atomic_t                    tx_seq;
    atomic_t                    rx_seq;

    /* statics sync string hcc_chan_stat_str same time.*/
    atomic_t                    hcc_chan_status;
   // uint32                    net_queue_flowcontrol;
    uint32                      dev_flowctrl_on_count;
    uint32                      dev_flowctrl_off_count;
    uint32                      dev_assemble_reset_count;

    uint32                      tx_arp_count;
    uint32                      tx_eapol_count;
    uint32                      tx_dhcp_count;

    uint32                      sys_hcc_dyn_pri_enable;
    uint32                      sys_hcc_dyn_pri_mask;

    uint32                      sys_tcp_rx_ack_opt_enable;
    uint32                      sys_tcp_tx_ack_opt_enable;

    uint32                      sys_hcc_rx_opt_enable;
    uint32                      sys_hcc_tx_assem_opt_enable;
#ifdef HCC_DEBUG
    hcc_debug            debug;
#endif

    hcc_dev_flow_ctrl   dev_flow_ctrl;

    /*used to control the special net stream by z00262551*/
    hwifi_net_stream net_stream;

    /*detect net work state*/
    hcc_net_work_detect net_work_detect;
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
 int hcc_queues_flow_ctrl_len(struct hcc_handler* hcc,
                                        hcc_chan_type dir);
int hcc_queue_flow_ctrl_len(struct hcc_handler* hcc,
                                        hcc_queue_type type,
                                        hcc_chan_type dir);
struct hcc_handler* hcc_alloc(struct hi110x_device *hi110x_dev, struct iodevice *io_dev);
void  hcc_release(struct hcc_handler* hcc);

int32 hcc_rx(struct hcc_handler *hcc, struct sk_buff *skb);
int32 hcc_tx(struct hcc_handler *hcc, uint8 main_type, uint8 sub_type, struct sk_buff *skb);

int32 hcc_rx_register(struct hcc_handler *hcc, uint8 mtype, hcc_rx_cb_func rx_cb, void *context);
void  hcc_rx_unregister(struct hcc_handler *hcc, uint8 mtype);

void hcc_tx_queue_all_reset(struct hcc_handler* hcc, uint8 locked);

void hcc_tx_wake_queue(struct hcc_handler* hcc, uint8 locked);
void hcc_set_tx_queue_status(struct hcc_handler* hcc, hcc_state state);

void hcc_rx_queue_all_reset(struct hcc_handler* hcc, uint8 locked);

 void hcc_queues_reset(struct hcc_handler* hcc, uint8 locked);

void hcc_sched_transfer(struct hcc_handler* hcc);

extern int hwifi_send_dfx_wid_char(struct cfg_struct *cfg, uint16 wid, uint8 val);

static inline void hcc_tx_transfer_lock(struct hcc_handler *hcc)
{
    HWIFI_VERBOSE("+++++++tx transfer lock+++r==>[%pF]", __builtin_return_address(0));
    mutex_lock(&hcc->tx_transfer_lock);
    HWIFI_VERBOSE("-------tx transfer lock---r==>[%pF]", __builtin_return_address(0));
}

static inline void hcc_tx_transfer_unlock(struct hcc_handler *hcc)
{
    HWIFI_VERBOSE("+++++++tx transfer unlock++r==>[%pF]", __builtin_return_address(0));
    mutex_unlock(&hcc->tx_transfer_lock);
    HWIFI_VERBOSE("-------tx transfer unlock--r==>[%pF]", __builtin_return_address(0));
}

static inline void hcc_rx_split_lock(struct hcc_handler *hcc)
{
    HWIFI_VERBOSE("+++++++rx split lock+++r==>[%pF]", __builtin_return_address(0));
    mutex_lock(&hcc->rx_split_lock);
    HWIFI_VERBOSE("-------rx split lock---r==>[%pF]", __builtin_return_address(0));
}

static inline void hcc_rx_split_unlock(struct hcc_handler *hcc)
{
    HWIFI_VERBOSE("+++++++rx split unlock++r==>[%pF]", __builtin_return_address(0));
    mutex_unlock(&hcc->rx_split_lock);
    HWIFI_VERBOSE("-------rx split unlock--r==>[%pF]", __builtin_return_address(0));
}

static inline void hcc_transfer_lock(struct hcc_handler *hcc)
{
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!hcc->io_dev);
    hsdio_rx_transfer_lock(bus_to_sdio(hcc->io_dev));
    hcc_tx_transfer_lock(hcc);
}

static inline void hcc_transfer_unlock(struct hcc_handler *hcc)
{
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!hcc->io_dev);
    hcc_tx_transfer_unlock(hcc);
    hsdio_rx_transfer_unlock(bus_to_sdio(hcc->io_dev));
}

/*Make sure this is no tx data transfer when you call this function.TRANS_LOCK/TRANS_UNLOCK*/
static  inline int32 hcc_tx_asseminfo_reset(struct hcc_handler *hcc, uint8 locked)
{
    HWIFI_BUG_ON(!hcc);
    if(locked)
        hcc_tx_transfer_lock(hcc);
    hcc->assembled_cnt = 0;
    memset(hcc->assem_info, 0, sizeof(hcc->assem_info));
    if(locked)
        hcc_tx_transfer_unlock(hcc);
    return SUCC;
}

static  void hcc_dev_flowctr_timer_del(struct hcc_handler *hcc)
{
    if(in_interrupt())
    {
        del_timer(&hcc->dev_flow_ctrl.flow_timer);
    }
    else
    {
        del_timer_sync(&hcc->dev_flow_ctrl.flow_timer);
    }
}

static inline void hcc_dev_flowctrl_on(struct hcc_handler *hcc, int32 need_notify_dev)
{
    if(FLOWCTRL_OFF == g_flowctrl_flag)
    {
        long flow_ctrl_period;
        hcc_dev_flowctr_timer_del(hcc);
        hcc->dev_flow_ctrl.flow_ctrl_on_time = jiffies;
        HWIFI_DEBUG("Dev flow ctrl status change from off=>on");
        flow_ctrl_period = (long)hcc->dev_flow_ctrl.flow_ctrl_on_time -
                           (long)hcc->dev_flow_ctrl.flow_ctrl_off_time;
        if(likely(flow_ctrl_period >= 0))
        {
            uint32 second = jiffies_to_msecs(flow_ctrl_period)/1000;
            second = MIN(second,(uint32)HCC_DEV_FLOW_INFO_MAX_TRACE_SECONDS);
            hcc->dev_flow_ctrl.flow_info[second].count++;
            if(unlikely(second))
            {
                hcc->dev_flow_ctrl.flow_info[second].cpu_time =
                    cpu_clock(UINT_MAX);
                if(second >= HCC_DEV_FLOW_INFO_MAX_TRACE_SECONDS)
                {
                    HWIFI_WARNING("dev flow ctrl locked %u seconds,too long... ", second);
                }
            }
        }else
        {
            HWIFI_WARNING("Flow ctrl off time[%lu] is after on time[%lu]?",
                            hcc->dev_flow_ctrl.flow_ctrl_off_time,
                            hcc->dev_flow_ctrl.flow_ctrl_on_time);
        }
    }

    if (need_notify_dev)
    {
        HWIFI_DEBUG("Host turn on dev flow ctrl...");
        bus_reset_flowctrl(hcc->io_dev);
    }
        if(FLOWCTRL_OFF == g_flowctrl_flag)
        {
            g_flowctrl_flag = FLOWCTRL_ON;
            /*If hcc tx flow ctrl change from off ==> on we should flush the hcc_trans_thread*/
            hcc_sched_transfer(hcc);
        }
    smp_wmb();
    hcc->dev_flowctrl_on_count++;

    HWIFI_DEBUG("Start tranferring to device");
}
static inline void hcc_dev_flowctrl_off(struct hcc_handler *hcc, hwifi_trans_flow_dir flow_dir)
{
    if(FLOWCTRL_ON == g_flowctrl_flag)
    {
        HWIFI_DEBUG("Dev flow ctrl status change from on=>off");
        if(likely(HWIFI_DEV2HOST == flow_dir))
        {
            mod_timer(&hcc->dev_flow_ctrl.flow_timer, jiffies + hcc->dev_flow_ctrl.timeout);
        }else if(HWIFI_HOST2DEV == flow_dir)
        {
            hcc_dev_flowctr_timer_del(hcc);
        }
        hcc->dev_flow_ctrl.flow_ctrl_off_time = jiffies;
    }
    g_flowctrl_flag = FLOWCTRL_OFF;
    smp_wmb();
    hcc->dev_flowctrl_off_count++;

    HWIFI_DEBUG("stop tranferring to device");
}

static inline uint32 hcc_get_dev_flowctrl(void)
{
    return g_flowctrl_flag;
}

static inline void hcc_dev_assemble_reset(struct hcc_handler *hcc)
{
    g_assemble_reset_flag = ASSEMBLE_RESET_ENABLE;
    smp_wmb();
    hcc->dev_assemble_reset_count ++;
    HWIFI_WARNING("DEV pkt CHECKSUM error");
}

/*FIXME? For Debug assem count*/
STATIC inline void assem_debug_count_reset(struct hcc_handler *hcc)
{
    //spin_lock(&hcc->assem_c_lock);
    memset(hcc->assem_rx_count, 0, sizeof(hcc->assem_rx_count));
    memset(hcc->assem_tx_count, 0, sizeof(hcc->assem_tx_count));
    //spin_unlock(&hcc->assem_c_lock);
}

STATIC inline void assem_debug_tx_count_incr(struct hcc_handler *hcc, int assem_count)
{
    //spin_lock(&hcc->assem_c_lock);
    hcc->assem_tx_count[assem_count]++;
    //spin_unlock(&hcc->assem_c_lock);
}

STATIC inline void assem_debug_rx_count_incr(struct hcc_handler *hcc, int assem_count)
{
    //spin_lock(&hcc->assem_c_lock);
    hcc->assem_rx_count[assem_count]++;
    //spin_unlock(&hcc->assem_c_lock);
}

STATIC inline int hcc_cpu_time_to_str(char* tbuf, int buff_len,unsigned long long t)
{
    unsigned tlen;
    unsigned long nanosec_rem;
    HWIFI_BUG_ON(!tbuf);
    nanosec_rem = do_div(t, 1000000000);
    tlen = sprintf(tbuf, "[%5lu.%06lu]",
                        (unsigned long) t,
                        nanosec_rem / 1000);
    if(HWIFI_WARN(buff_len <= tlen, "buff len[%d] must bigger than tlen[%d]",
                                    buff_len, tlen))
    {
        return -EFAIL;
    }
    return SUCC;
}

STATIC inline int hi110x_print_cpu_time_str(unsigned long long t, char* loglevel,
                                         char* output)
{
    int ret = 0;
    char buf[50]={0};
    ret = hcc_cpu_time_to_str(buf, 50, t);
    printk("%s %s %s \n", loglevel, output, buf);
    return ret;
}

int32 hcc_dispose_pkts(struct hcc_handler *hcc, uint8 *buf, uint32 len);
uint16 hcc_get_nextpkt_len(uint8 *buf, uint32 len);

#ifdef WLAN_PERFORM_OPT
int32 hcc_build_header(struct hcc_handler *hcc, uint8 mtype, uint8 stype, struct sk_buff *skb);
#endif

extern void hcc_netif_txflowcontrol(struct hcc_handler* hcc,  int32 queue_mapping, uint32 state);

int hcc_get_nextpkt_skb_info(struct hi110x_sdio *hi_sdio,
                                    struct sk_buff_head *pkt_sk_list_head);
int32 hcc_dispose_skb_sg(struct hcc_handler *hcc, struct sk_buff_head * head);
void hcc_transfer_reset(struct hcc_handler* hcc, uint8 flag);
int32 hcc_drop_skb_sg(struct hi110x_sdio *hi_sdio, struct sk_buff_head * head);

uint32 check_txqueue_status(struct hcc_handler* hcc);

#ifdef WMM_OPT_FOR_AUTH
hcc_queue_type hcc_filter_spec_type(uint8* data, struct hcc_handler *hcc, hcc_chan_type dir);
#else
hcc_queue_type hcc_filter_tcp_type(uint8* data);
#endif

char* hcc_get_queue_str(hcc_queue_type q_type);
char* hcc_get_dir_str(hcc_chan_type dir);

void hcc_trans_limit_default_config(struct hcc_handler* , enum hi110x_network_mode);

void hcc_set_trans_limit(struct hcc_handler* hcc, hcc_chan_type chan_type,
                          hcc_queue_type   queue_type, uint32 limit);

void hcc_trans_dyn_pri_control(struct hcc_handler* hcc, int enable);
hcc_state hcc_get_tx_queue_status(struct hcc_handler* hcc);
void hcc_debug_peak_inc(struct hcc_handler   *hcc, hcc_chan_type dir,
                        hcc_queue_type type,unsigned long count);
void hcc_debug_peak_reset(struct hcc_handler   *hcc);
void hcc_debug_peak_show(struct hcc_handler   *hcc, hwifi_union_log* log);
char* hcc_get_action_str(hcc_action_type action);
char* hcc_get_chan_stat_str(hcc_state state);

char* hcc_get_net_stream_sm_str(hwifi_net_stream_sm sm_stat);

void hcc_queues_flow_ctrl_handle(struct hcc_handler* hcc,
                                        hcc_queue_type type,
                                        hcc_chan_type dir,
                                        bool reduce,int i);
uint32 hcc_get_tx_net_queue_mapping(struct hcc_handler* hcc, struct sk_buff *skb);

int  hcc_net_stream_try_connect(struct hcc_handler    *hcc,
                                                                                  hwifi_net_stream_type stream_t);
int hcc_net_streams_connected(struct hcc_handler    *hcc);

hwifi_net_stream_type hcc_find_stream_by_name(struct hcc_handler* hcc,
                                                                                                                        const char* name);

/*net work exception detect*/                                                                                                                        
 void hcc_net_work_state_detect(struct hcc_handler    *hcc);
 void hcc_net_work_state_detect_cancel(struct hcc_handler    *hcc);
 void hcc_net_work_detect_deinit(struct hcc_handler    *hcc);

/*For hcc rx/tx queues total count, for wait event condition*/
STATIC inline int hcc_queues_len(struct hcc_handler* hcc,
                                        hcc_chan_type dir)
{
    return atomic_read(&hcc->hcc_queues[dir].count);
}
STATIC inline void hcc_queues_len_add(struct hcc_handler* hcc,
                                        hcc_chan_type dir, int i)
{
    atomic_add(i, &hcc->hcc_queues[dir].count);
}
STATIC inline void hcc_queues_len_sub(struct hcc_handler* hcc,
                                        hcc_chan_type dir, int i)
{
    atomic_sub(i, &hcc->hcc_queues[dir].count);
    if(unlikely(atomic_read(&hcc->hcc_queues[dir].count)<0))
    {
        HWIFI_ERROR("Error %s wait count:%d",
                    hcc_get_dir_str(dir),atomic_read(&hcc->hcc_queues[dir].count));
    }
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_hcc.h */
