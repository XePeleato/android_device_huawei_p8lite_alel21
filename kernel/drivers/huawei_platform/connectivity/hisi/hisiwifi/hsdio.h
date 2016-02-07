

#ifndef __HWIFI_SDIO_H__
#define __HWIFI_SDIO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/host.h>
#include <linux/skbuff.h>
#include "user_ctrl.h"

#include "hwifi_bus.h"

#include <linux/of_platform.h>
#include <linux/of_device.h>

/*
 * 2 Macro Definition
 */
#define VENDOR_ID_HISI                      0x22D1      /* VENDOR ID */
#define PRODUCT_ID_HISI_1101                0x1101      /* Product 1101 */

#define SDIO_BLOCK_SIZE                     512         /* one size of data transfer block size,
                                                         * 64, 128, 256, 512, 1024 */

#define SDIO_MAX_CONTINUS_RX_COUNT          64          /* sdio max continus rx cnt */

#define SDIO_DEFAULT_RXBUFS_COUNT           512
#define SDIO_MAX_RXBUFS_COUNT               1024

#define SDIO_RXBUF_MAX_SIZE                 (32*1024)

#define REG_FUNC1_FIFO                      0x00        /* Read Write FIFO */
#define REG_FUNC1_INT_STATUS                0x08        /* interrupt mask and clear reg */
#define REG_FUNC1_INT_ENABLE                0x09        /* interrupt */
#define REG_FUNC1_XFER_COUNT                0x0c        /* notify number of bytes to be read */
#define REG_FUNC1_WRITE_MSG                 0x24        /* write msg to device */
#define REG_FUNC1_MSG_FROM_DEV              0x28        /* notify Host that device has got the msg */
#define REG_FUNC1_MSG_HIGH_FROM_DEV         0x2b        /* notify Host that device has got the msg, Host receive the msg ack */

#define HI110X_FUNC1_INT_DREADY      (1 << 0)           /* data ready interrupt */
#define HI110X_FUNC1_INT_RERROR      (1 << 1)           /* data read error interrupt */
#define HI110X_FUNC1_INT_MFARM       (1 << 2)           /* ARM Msg interrupt */
#define HI110X_FUNC1_INT_ACK         (1 << 3)           /* ACK interrupt */

#define HI110X_FUNC1_INT_MASK        (HI110X_FUNC1_INT_DREADY | HI110X_FUNC1_INT_RERROR | HI110X_FUNC1_INT_MFARM | HI110X_FUNC1_INT_ACK)

/* Device to Host Msg */
#define HI110X_MSG_FLOW_ON           (1 << 2)    /* can transfer */
#define HI110X_MSG_FLOW_OFF          (1 << 1)    /* can't transfer */
#define HI110X_MSG_VERIFY_ERROR      (1 << 3)

#define HI110X_MSG_DEVICE_ERROR      (1 << 7)   /*device is abort*/
#define HI110X_MSG_DEVICE_NO_RX      (1 << 8)   /*device is norx */

#define HI110X_MSG_RESET_FLOWCTRL    (1 << 0)    /* reset device flowctrl */
#define HI110X_MSG_ALLOWSLEEP        (1 << 5)   /* allow Device to sleep */
#define HI110X_MSG_FORBIDSLEEP       (1 << 6)   /* forbid Device sleep */

#define HI110X_MSG_DISCONN           (1 << 0)

#define HI110X_MSG_DEV_PRE_SHUTDOWN_REPLY       ((1<<7)|(1<<30))

/* Host to Device Msg */
#define HOST2HI110X_TRYSLEEP_MSG     (1 << 0)   /* Host allow Device to sleep msg */

#define HOST2HI110X_PRE_SHUTDOWN_MSG     (1 << 1)   /* Host allow Device to sleep msg */

#define SDIO_ALIGN_4_OR_BLK(len)     ((len) < SDIO_BLOCK_SIZE ? ALIGN((len), 4) : ALIGN((len), SDIO_BLOCK_SIZE))

#define SDIO_READ                               0x01
#define SDIO_WRITE                              0x02
#define HWIFI_SLEEP_MSG_WAIT_TIMEOUT            (2000)
#define HWIFI_PRE_SHUTDOWN_MSG_TIMEOUT            (500)
/* SDIO status flag */
typedef enum {
    HI110X_SDIO_UNAVAILABLE = 0,
    HI110X_SDIO_AVAILABLE,
}HI110X_SDIO_STATUS;
#define HCC_MAX_ASSEMBLE_NUM            16
#define HCC_DEFAULT_ASSEMBLE_NUM        8
#define HCC_ASSEMBLE_INFO_SIZE          16

#define HWIFI_SCHEDULE_PERIOD   (1)

/*add one sg item for align sdio block transfer*/
#define HCC_SDIO_SG_ASSEMBLE_NUM      (HCC_MAX_ASSEMBLE_NUM + 1)

#define ACTIVE_HEARTBEAT_TIME           3
#define DEEPSLEEP_HEARTBEAT_TIME        300

/*
 * 3 Enum Type Definition
 */


/*
 * 4 Global Variable Declaring
 */
extern uint32           loglevel;
extern int              panic_debug;
extern uint             band_5g_enabled;
extern unsigned long    g_data_rate_down;
extern unsigned long    g_data_rate_up;
extern uint32           hcc_assemble_count;
extern uint8            g_ba_max_tx;
extern uint8            g_ba_max_rx;
extern uint8            g_ba_max_retry;
extern uint32           g_cur_cpufreq;
extern uint32           g_sdio_rate_40M;
extern uint32           g_sdio_rate_80M;
extern uint32           g_sdio_rate_160M;
extern uint32           g_sdio_rate_240M;
extern uint32           g_powermgmt_switch;
extern uint32           g_watchdog_timeout;
extern uint32           g_wmm_drop_interval;
extern uint32           g_wmm_que_len_for_drop;
extern uint32           g_wmm_drop_cnt_per_time;
extern unsigned long    g_ap_data_rate_down;
extern unsigned long    g_ap_data_rate_up;
extern int              countrycode_change_by_ie_enabled;
extern int32            g_roam_switch;
extern uint8            g_ap_coex_2040m_enabled;
extern uint8            g_sta_2040m_enabled;
extern uint8            g_ap_2040m_enabled;
extern int              g_trigger_rssi_threshold;
extern uint32           g_trigger_count_threshold;

/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
struct hi110x_device;

struct rxbuf_node
{
    struct list_head    list;
    uint32              used;       /* 0-free, 1-used */
    uint8              *rxbuf;
    uint32              data_len;
};

typedef struct _rx_skb_node_
{
    struct list_head    list;
    struct sk_buff_head rx_head;
}rx_skb_node;

#define HSDIO_RX_ERROR_TRANS_BUSY     (0)
/*indicate next pkt alloc failed!*/
#define HSDIO_RX_ERROR_NEXT_PKT_BUSY    (1)

typedef struct _hsdio_heartbeat_{
    /*when working, 30s(this maybe change in file hisi_cfg.ini) heartbeat timeout
      when sleeping,22 minutes heartbeat timeout,
      FIXME? device use rtc time report heartbeat pkt, host use soft time to monitor
    */
    uint32 cur_timeout;
    uint32 work_timeout;
    uint32 sleep_timeout;;

    /*The last heartbeat pkt came cpu time.*/
    unsigned long long last_update_time;
    /*heart beat timer*/
    struct timer_list           heartbeat_timer;
}hsdio_heartbeat;

struct hi110x_sdio
{
    struct iodevice             io_dev;
    struct hi110x_device       *hi110x_dev;
    struct sdio_func           *func;

    uint32                      next_pkt_len;
    /*used to protect rx transfer.*/
    struct mutex                rx_transfer_lock;

    struct sk_buff_head         next_pkt_sk_list;
    uint32                      rx_sg_len;
    struct scatterlist          rx_sg[HCC_SDIO_SG_ASSEMBLE_NUM];

    struct  list_head           rx_skb_pool_list;
    spinlock_t                  rx_skb_pool_lock;
    atomic_t                    rx_skb_pool_count;

    /*used to sg list sdio block align*/
    uint8                       *sdio_align_buff;


    struct sk_buff_head         rx_reserve_skb_list;
    /*rx trans err stat*/
    unsigned long                      rx_skb_err;

    struct rxbuf_node          *rxbuf_pool;
    struct list_head            rxbuf_list;
    int32                       free_rxbuf_count;
    uint32                      rxbuf_size;
    spinlock_t                  rxbuf_pool_lock;
    spinlock_t                  rxbuf_list_lock;
    wait_queue_head_t           rxbuf_waitq;

    uint32                      data_rx_count;


#ifdef WLAN_PERFORM_OPT
    struct task_struct         *sdio_rx_dispose_tsk;
    struct semaphore            sdio_rx_dispose_sema;
    atomic_t                    sdio_tx_on_flag;      /*表示tx thread 是否在操作SDIO总线*/
    atomic_t                    sdio_rx_wait_flag;    /*表示是否有rx在等待*/
#endif

    hsdio_heartbeat             heartbeat;

    struct workqueue_struct    *rx_process_wq;
    struct work_struct          isr_work;
    atomic_t                    intr_pending;

    struct task_struct         *gpio_rx_tsk;
    struct semaphore            gpio_rx_sema;

    struct work_struct          wakeup_work;
    struct work_struct          sleep_work;


    /*sdio interrupt via gpio*/
    unsigned long long sdio_int_count;
    unsigned long long gpio_int_count;

    uint32                hsdio_tx_err_count;
    uint32                hsdio_rx_err_count;
    uint32                dev_err_cnt;
    uint32                dev_err_subcnt;

};

typedef struct _hsdio_io_cfg_
{
#ifdef CONFIG_PINCTRL
    char* current_pinctrl_state;
    struct pinctrl *pctrl;
    struct pinctrl_state *pins_normal;
    struct pinctrl_state *pins_idle;
#else
#define PINCTRL_STATE_DEFAULT "unsupport"
#define PINCTRL_STATE_IDLE "unsupport"
#endif
}hsdio_io_cfg;

#define bus_to_sdio(iodev) container_of((iodev), struct hi110x_sdio, io_dev)

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */

/*protect rx data transfer*/
static inline void hsdio_rx_transfer_lock(struct hi110x_sdio *hi_sdio)
{
    HWIFI_VERBOSE("+++++++rx transfer lock++++r==>[%pF]", __builtin_return_address(0));
    mutex_lock(&hi_sdio->rx_transfer_lock);
    HWIFI_VERBOSE("-------rx transfer lock----r==>[%pF]", __builtin_return_address(0));
}

static inline void hsdio_rx_transfer_unlock(struct hi110x_sdio *hi_sdio)
{
    HWIFI_VERBOSE("+++++++rx transfer unlock+++++r==>[%pF]", __builtin_return_address(0));
    mutex_unlock(&hi_sdio->rx_transfer_lock);
    HWIFI_VERBOSE("-------rx transfer unlock-----r==>[%pF]", __builtin_return_address(0));
}

/*Debug Function author zwx165479*/
static inline void dump_sg_buffer(const char* level,struct scatterlist* sg, int sg_len)
{
    int i = 0;
    struct scatterlist* sg_t;
    BUG_ON(!sg);
    level = level?:"";
    printk("Sg Debug Dump: [sg:%p][sg_len:%d]\n", sg, sg_len);
    for_each_sg(sg, sg_t, sg_len, i)
    {
        printk("%s[sg:%d],[data:%p],[len:%u]\n",level,i,sg_virt(sg_t), sg_t->length);
        print_hex_dump_bytes("sg ", DUMP_PREFIX_ADDRESS, sg_virt(sg_t), sg_t->length);
    }
}

static inline void dump_skb_buffer(const char* level, struct sk_buff_head* skb_head)
{
    int i = 0;
    struct sk_buff    *skb;
    //return;
    level = level?:KERN_DEBUG;
    printk("%s=====skb Debug Dump=====\n", level);
    skb_queue_walk(skb_head, skb)
    {
        printk("%s[%d][data:%p],[len:%d]\n",level,++i, (void*)skb->data, skb->len);
        print_hex_dump_bytes("skb dump ", DUMP_PREFIX_ADDRESS, (void*)skb->data, skb->len);
    }
}

static inline void hisdio_heartbeat_timer_start(struct hi110x_sdio *hi_sdio, unsigned int timeout)
{
    /*start the heart beat */
    HWIFI_INFO("Sdio start %u seconds timeout heartbeat timer", timeout);
    hi_sdio->heartbeat.cur_timeout = timeout;
    mod_timer(&hi_sdio->heartbeat.heartbeat_timer,
              jiffies + timeout * HZ);
}

static inline void hisdio_heartbeat_timer_update(struct hi110x_sdio *hi_sdio, unsigned int timeout)
{
    /*start the heart beat */
    HWIFI_DEBUG("Sdio update %u seconds timeout timer", timeout);
    hi_sdio->heartbeat.cur_timeout = timeout;
    mod_timer(&hi_sdio->heartbeat.heartbeat_timer,
              jiffies + timeout * HZ);
}

static inline void hisdio_heartbeat_timer_stop(struct hi110x_sdio *hi_sdio)
{
    HWIFI_INFO("Sdio hearbeat timer stop");
    if(in_interrupt())
    {
        del_timer(&hi_sdio->heartbeat.heartbeat_timer);
    }else
    {
        del_timer_sync(&hi_sdio->heartbeat.heartbeat_timer);
    }
}
extern int32 patch_get_cfg(uint8 *cfg, int32 type);
struct sk_buff* hisdio_reserve_skb_alloc(struct hi110x_sdio *hi_sdio, uint32 len);
void hisdio_reserve_skb_free(struct hi110x_sdio *hi_sdio,struct sk_buff* skb);
void hisdio_reserve_skb_free_list_init(struct hi110x_sdio *hi_sdio, struct sk_buff_head* list);

int hi110x_comm_work_submit(struct hi110x_sdio *hi_sdio, struct work_struct* worker);
int hsdio_union_log_submit(struct hi110x_sdio *hi_sdio);
/*Sdio Wake Up event Process*/
int hsdio_wakeup_submit(struct hi110x_sdio *hi_sdio, struct work_struct* worker);
/*Sdio Sleep event process */
int hsdio_sleep_submit(struct hi110x_sdio *hi_sdio, struct work_struct* worker);
void hi110x_gpio_intr_enable(int8 enable);
void hi110x_sdio_int_stat_dump(struct hi110x_sdio *hi_sdio, hwifi_union_log* log);
#ifndef CONFIG_PINCTRL
static inline int32 hsdio_sdio_iocfg(char* state)
{
}
#else
int32 hsdio_sdio_iocfg(char* state);
#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_sdio.h */
