/******************************************************************************

          Copyright (C), 2001-2011, huawei technology limited company

 ******************************************************************************
  Filename   : hw_bfg_ps.h
  Version    : first draft
  Author     : zwx144390
  Create date: 2012-10-11
  Modify     :
  Func descr : inclue file
  Func list  :
  Modi histoy:
  1.Data     : 2012-10-11
    Author   : zwx144390
    Modify
    content  : Create File

******************************************************************************/
#ifndef __HW_BFG_PS_H__
#define __HW_BFG_PS_H__

#ifdef UT_TEST
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#endif
/*****************************************************************************
  Include else Head file
*****************************************************************************/

#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/debugfs.h>

#ifdef CONFIG_HW_CHR
#include "../hw-chr/chr_user.h"
#else
#define CHR_LOG(prio, tag, fmt...)
#endif

/*****************************************************************************
  Define macro
*****************************************************************************/
typedef unsigned char               uint8;
typedef char                        int8;
typedef unsigned short              uint16;
typedef short                       int16;
typedef unsigned int                uint32;
typedef int                         int32;
typedef unsigned long               uint64;
typedef long                        int64;

#define FEATURE_PM_DOWNLOAD_PATCH

/* #define FEATURE_BT_HCI_LINE_LDISC  */

/* when needs recv uart data to a data file, open this option */
/* when open this option, must mkdir /data/bfg_data before start bfg */
/* #define DEBUG_USE  */

#define DBG_FILE_TIME               (600)

#ifndef UT_TEST
//#define HW_DEBUG

#ifdef HW_DEBUG
#define PS_PRINT_DBG(s, args...)            do{ \
		printk(KERN_DEBUG KBUILD_MODNAME ":[%s:DEBUG]: " s,__func__, ## args); \
		CHR_LOG(CHR_LOG_DEBUG, CHR_LOG_TAG_PLAT, s, ##args); \
	}while(0)
#define PS_PRINT_FUNCTION_NAME                                                                       \
        do {                                                                                         \
            printk(KERN_DEBUG KBUILD_MODNAME ":[The function name is : %s:NAME]" ,__func__);         \
        }while(0)
#else
#define PS_PRINT_DBG(s, args...)
#define PS_PRINT_FUNCTION_NAME
#endif
#define PS_PRINT_ALERT(s, args...)          do{ \
		printk(KERN_ALERT KBUILD_MODNAME ":[%s:ALERT]: " s,__func__, ## args); \
		CHR_LOG(CHR_LOG_ERROR, CHR_LOG_TAG_PLAT, s, ##args); \
	}while(0)

#define PS_PRINT_ERR(s, args...)            do{ \
		printk(KERN_ERR KBUILD_MODNAME ":[%s:ERR]: " s,__func__, ## args); \
		CHR_LOG(CHR_LOG_ERROR, CHR_LOG_TAG_PLAT, s, ##args); \
	}while(0)

#define PS_PRINT_SUC(s, args...)            do{ \
		printk(KERN_ERR KBUILD_MODNAME ":[%s:SUC]: " s,__func__, ## args); \
		CHR_LOG(CHR_LOG_INFO, CHR_LOG_TAG_PLAT, s, ##args); \
	}while(0)

#define PS_PRINT_WARNING(s, args...)        do{ \
		printk(KERN_WARNING KBUILD_MODNAME ":[%s:WARNING]: " s,__func__, ## args);\
		CHR_LOG(CHR_LOG_WARN, CHR_LOG_TAG_PLAT, s, ##args); \
	}while(0)
#define PS_PRINT_INFO(s, args...)           printk(KERN_INFO KBUILD_MODNAME ":[%s:INFO]: " s,__func__, ## args)

#define STATIC  static
#else

#define STATIC

#endif

#define DTS_COMP_HI1101_PS_NAME		"hisilicon,hi1101-ps"

#define BT_MAX_FRAME                (1500)      /* tx bt data max lenth */

#define PUBLIC_BUF_MAX              (4500)      /* public buf total lenth */
#define GNSS_TX_MAX_FRAME           (3000)      /* gnss send buf max lenth */
#define GNSS_RX_MAX_FRAME           (8001)      /* gnss recv buf max lenth */
#define GNSS_PACKET_LIMIT           (200)

#define BT_TX_TIMES                 (5)         /* times continue tx bt data */

#define FM_MAX_FRAME                (28)        /* fm cmd data max lenth */

#define DBG_MAX_FRAME               (10000)     /* dbg cmd data max lenth */

#define COEXIST_MAX_FRAME           (10)        /* coexist data max lenth */

#define BT_FRAME_MAX_LEN		(1032) /* 1026 + beat */
#define GNSS_FRAME_MAX_LEN	    (58)  /* 52 + beat*/
#define FM_FRAME_MAX_LEN		(75)  /* 69 + beat */
#define DBG_FRAME_MAX_LEN		(156) /* 150 + beat */


#define PACKET_START_SIGNAL         (0x7e)
#define PACKET_END_SIGNAL           (0x7e)

/* below is sys type define for 4 byte packet head */
#define PACKET_TX_FUNC_SYS          (0x00)
#define PACKET_RX_FUNC_SYS          (0x01)
#define PACKET_TX_FUNC_BT           (0x02)
#define PACKET_RX_FUNC_BT           (0x03)

#define PACKET_TX_FUNC_GNSS_START   (0x04)
#define PACKET_TX_FUNC_GNSS_INT     (0x05)
#define PACKET_TX_FUNC_GNSS_LAST    (0x06)

#define PACKET_RX_FUNC_GNSS_START   (0x07)
#define PACKET_RX_FUNC_GNSS_INT     (0x08)
#define PACKET_RX_FUNC_GNSS_LAST    (0x09)

#define PACKET_TX_FUNC_FM           (0x0a)
#define PACKET_RX_FUNC_FM           (0x0b)

#define PACKET_RX_FUNC_PM_BFG       (0x0c)

#define PACKET_TX_FUNC_BFG_DBG      (0x0d)      /* tx dbg level to device */
#define PACKET_RX_FUNC_BFG_DBG      (0x0e)      /* rx dbg from device */

/* end sys type define */

/* last words type in 8 byte packet head */
#define PACKET_RX_FUNC_LAST_WORDS   (0x10)      /* rx last words from device */

/* fixed lenth for sys packet */
#define SYS_TOTAL_PACKET_LENTH      (0x06)      /* sys packet total lenth */

/* below is tx sys content define */
#define SYS_CFG_OPEN_BT             (0x00)      /* open BT */
#define SYS_CFG_OPEN_GNSS           (0x01)      /* open GNSS */
#define SYS_CFG_OPEN_FM             (0x02)      /* open FM */
#define SYS_CFG_CLOSE_BT            (0x03)      /* close BT */
#define SYS_CFG_CLOSE_GNSS          (0x04)      /* close GNSS */
#define SYS_CFG_CLOSE_FM            (0x05)      /* close FM */
#define SYS_REQ_STATIC_INFO         (0x06)      /* ask statistic */

/* below is rx sys content define from device */
#define SYS_INF_PL_INIT             (0x00)      /* platform init over */
#define SYS_INF_BT_INIT             (0x01)      /* BT soft init over */
#define SYS_INF_GNSS_INIT           (0x02)      /* GNSS soft init over */
#define SYS_INF_FM_INIT             (0x03)      /* FM soft init over */
#define SYS_INF_BT_DISABLE          (0x04)      /* BT disable */
#define SYS_INF_GNSS_DISABLE        (0x05)      /* FM disable */
#define SYS_INF_FM_DISABLE          (0x06)      /* GNSS disable */
#define SYS_INF_BT_EXIT             (0x07)      /* BT release */
#define SYS_INF_GNSS_EXIT           (0x08)      /* GNSS release */
#define SYS_INF_FM_EXIT             (0x09)      /* FM release */
#define SYS_INF_GNSS_WAIT_DOWNLOAD      (0x0A)  /* wait download GNSS */
#define SYS_INF_GNSS_DOWNLOAD_COMPLETE  (0x0B)  /* GNSS download over */
#define SYS_INF_BFG_HEART_BEAT      (0x0C)      /* BFG heart-beat information */

/* time in msec to wait for
 * line discipline to be installed
 */
#define HISI_LDISC_TIME             (1000)

#define RX_PACKET_CORRECT           (0)
#define RX_PACKET_CONTINUE          (1)
#define RX_PACKET_ERR               (-1)

#define RX_GNSS_ERR                 (-1)

#define OPEN_TTY_RETRY_COUNT        (5)

#define HISI_UART_DEV_NAME_LEN      (32)

/* separated packet sequence control byte define for gnss */
#define RX_GNSS_SEQ_NULL            (0)
#define RX_GNSS_SEQ_START           (1)
#define RX_GNSS_SEQ_INT             (2)
#define RX_GNSS_SEQ_LAST            (3)

#define UART_IOCTL_CMD              (1)
#define OPEN_UART_COMENT            ('1')
#define CLOSE_UART_COMENT           ('0')

#define UART_RST_DEVICE             (2)
#define SHOW_RST_DEVICE_START       ('0')
#define SHOW_RST_DEVICE_OVER        ('1')
#define STORE_RST_DEVICE_OVER       ('1')

/* yes or not allow reset device */
#define ALLOW_RST_DEVICE            ('1')
#define MASK_RST_DEVICE             ('0')
#define CLEAR_EXCEPTION_CNT         ('0')

#define DEFAULT_BAUD_RATE           (921600)

/* timeout for gnss read */
#define GNSS_SET_READ_TIME          (1)
#define GNSS_READ_DEFAULT_TIME      (1000)
#define GNSS_MAX_READ_TIME          (10000)

/* timeout for fm read */
#define FM_SET_READ_TIME            (1)
#define FM_READ_DEFAULT_TIME        (1000)
#define FM_MAX_READ_TIME            (10000)

/* timeout for dbg read */
#define DBG_READ_DEFAULT_TIME       (500)

#define LEN_LOW_BYTE(len)           ((uint8)(len & 0xff))
#define LEN_HIGH_BYTE(len)          ((uint8)((len & 0xff00)>>8))

/* define for tx and rx packet queue */
#define TX_HIGH_QUEUE               (1)
#define TX_LOW_QUEUE                (2)
#define RX_GNSS_QUEUE               (3)
#define RX_FM_QUEUE                 (4)
#define RX_BT_QUEUE                 (5)
#define RX_DBG_QUEUE                (6)

/* max number for tx and rx packet queue */
#define TX_HIGH_QUE_MAX_NUM         (50)
#define TX_LOW_QUE_MAX_NUM          (250)
#define RX_GNSS_QUE_MAX_NUM         (50)
#define RX_FM_QUE_MAX_NUM           (50)
#define RX_BT_QUE_MAX_NUM           (50)
#define RX_DBG_QUE_MAX_NUM          (50)

/* proto define for bt,gnss,fm when download patch */
#define BT_DOWNLOAD_PATCH           (0)
#define GNSS_DOWNLOAD_PATCH         (1)
#define FM_DOWNLOAD_PATCH           (2)

/* timeout when after download patch */
#define DEV_3IN1_OPEN_TIME          (5000)

#define RELEASE_DELAT_TIMES         (5)

/*add by cwx145522 for pm begin*/
#define BT_SLEEP_TIME               (5)
#define BEAT_TIME_ACTIVE            (3)
#define BEAT_TIME_SLEEP             (360)
#define POWER_ON_STATE              (1)
#define POWER_OFF_STATE             (0)
#define BFG_TIMER_ON                (1)
#define BFG_TIMER_OFF               (0)
/*add by cwx145522 for pm end*/

#define BFG_POWER_GPIO_DOWN         (0)
#define BFG_POWER_GPIO_UP           (1)
#define BT_WRITE_FLAG               (0)
#define FM_WRITE_FLAG               (1)
#define GNSS_WRITE_FLAG             (2)
#define COEX_WRITE_FLAG             (3)
#define DEBUG_WRITE_FLAG            (4)
/*****************************************************************************
  3 STRUCT define
*****************************************************************************/
/**
 * struct ps_proto_s - Per Protocol structure from BT/FM/GPS to ps
 * @recv: the receiver callback pointing to a function in the
 *  protocol drivers called by the ps driver upon receiving
 *  relevant data.
 * @uart_ioctl: callback handler pointing to a function in protocol
 *  handler called by ps when the pending registrations are complete.
 *  The registrations are marked pending, in situations when fw
 *  download is in progress.
 * @write: pointer to function in ps provided to protocol drivers from ps,
 *  to be made use when protocol drivers have data to send to TTY.
 * @priv_data: privdate data holder for the protocol drivers, sent
 *  from the protocol drivers during registration, and sent back on
 *  uart_ioctl and recv.
 * @max_frame_size: size of the largest frame the protocol can receive.
 */
struct ps_proto_s {
    int64 (*recv) (void *, uint8 *,int32 count);
    void  (*uart_ioctl) (void *,int8 cmd,int8 coment);
    int32 (*write) (struct sk_buff *skb);
    void  *priv_data;

    uint16 max_frame_size;
    uint16 fill_byte1;
};

struct ps_pm_s {
    atomic_t bfg_wake_host_flag;
    int32 (*download_patch) (void);
    int32 (*recv_patch) (const uint8 *,int32 count);
    int32 (*write_patch) (uint8 *,int32 count);
    int32 (*change_baud_rate) (int64 baud_rate);
    int32 (*set_pm_state) (uint8 type);
    int32 (*reset_bt_timer) (uint8);
    void  (*bfg_mutex_lock)(uint8);
    int32 (*bfg_power_set)(uint8, uint8);
    /*add by cwx145522 for pm begin*/
    void  (*bfg_wake_lock)(uint8);
    int32 (*bfg_gpio_state_get)(void);
    void  (*bfg_gpio_state_set)(uint8);
    int32 (*reset_beat_timer)(uint8);
    void  (*bfg_spin_lock)(uint8);
    /*add by cwx145522 for pm end*/
    int32 (*reset_bfg_power_gpio)(uint8);
    void  (*disable_bfg_powerpin_state)(uint8);
    void  (*change_patch_to_nomal)(void);
};

struct ps_packet_head {
    uint8  packet_start;
    uint8  func_type;
    uint16 packet_len;
};

struct ps_packet_end {
    uint8 packet_end;
};

/**
 * struct ps_core_s - PS core internal structure
 * @tty: tty provided by the TTY core for line disciplines.
 * @txq: the list of skbs which needs to be sent onto the TTY.
 * @pm_data: reference to the parent encapsulating structure.
 *
 */

struct ps_core_s {
    struct ps_proto_s *bt_proto;
    struct ps_pm_s *ps_pm;
    struct tty_struct *tty;
    /* queue head define for all sk_buff */
    struct sk_buff_head tx_high_seq;
    struct sk_buff_head tx_low_seq;
    struct sk_buff_head rx_gnss_seq;
    struct sk_buff_head rx_fm_seq;
    struct sk_buff_head rx_bt_seq;
    struct sk_buff_head rx_dbg_seq;
    /* define for new workqueue */
    struct workqueue_struct *ps_tx_workqueue;
    struct workqueue_struct *ps_rx_workqueue;
    struct workqueue_struct *ps_rst_device_workqueue;
    /* define new work struct */
    struct work_struct tx_skb_work;
    struct work_struct rx_bt_work;
    struct work_struct rst_device_work;
    /* define completion for 3in1 */
    struct completion dev_3in1_opened;

#ifdef  DEBUG_USE
    struct file * rx_data_fp;
    struct file * rx_data_fp_patch;
    uint64 pre_time;
    uint64 curr_time;
#endif

    /*add by cwx145522 for pm begin*/
    /*timer for bfg uart suspend*/
    struct timer_list bfg_timer;
    struct timer_list beat_timer;
    /*add by cwx145522 for pm end*/
    spinlock_t rx_lock;

    /* wait queue for rx packet */
    wait_queue_head_t rx_gnss_wait;
    wait_queue_head_t rx_bt_wait;
    wait_queue_head_t rx_fm_wait;
    wait_queue_head_t rx_dbg_wait;
    atomic_t throttle_flag;
    atomic_t bt_func_has_open;
    atomic_t fm_func_has_open;
    atomic_t gnss_func_has_open;
    atomic_t dbg_func_has_open;
    /*add by cwx145522 for pm begin*/
    /*bt and fm uart data flag*/
    atomic_t bt_fm_data_flag;
    /*flag for gnss going to sleep*/
    atomic_t gnss_sleep_flag;
    /*flag for heart beat*/
    atomic_t bfg_beat_flag;
    atomic_t bt_state_booting;
    atomic_t fm_state_booting;
    atomic_t gnss_state_booting;

    uint8  bt_state;
    uint8  fm_state;
    uint8  gnss_state;

    uint8  bfg_lowpower_enable;
    /*add by cwx145522 for pm end*/
    void  *pm_data;

    uint8  down_load_patch_proto;

    uint16 bt_max_frame_size;
    uint8  bt_registered;

    uint8  rx_sys_content;

    /* define variable for decode rx data from uart */
    uint8  rx_pkt_type;
    uint16 rx_pkt_total_len;
    uint16 rx_have_recv_pkt_len;
    uint16 rx_have_del_public_len;
    uint8 *rx_decode_tty_ptr;
    uint8 *rx_public_buf_org_ptr;
    uint8 *rx_decode_public_buf_ptr;

    /* define variable for rx gnss data */
    uint8 *rx_gnss_buf_ptr;
    uint8 *rx_gnss_buf_org_ptr;
    uint16 rx_gnss_pkt_len;
    uint16 rx_gnss_buf_all_len;
    uint8  rx_gnss_prev_seq;

    uint8  tty_have_open;
    uint16 gnss_read_delay;
    uint16 fm_read_delay;
    uint16 dbg_read_delay;
    uint8  reset_device_msg;
    uint8  in_reset_device_process;
    uint8  in_download_patch_state;
    uint8  allow_reset_device;
    uint32 bfg_exception_cnt;
};

/**
 * struct ps_plat_s - the PM internal data, embedded as the
 *  platform's drv data. One for each PS device in the system.
 * @pm_pdev: the platform device added in one of the board-XX.c file
 *  in arch/XX/ directory, 1 for each PS device.
 * @ldisc_installed: completion handler to notify that the OCTTY accepted
 *  the request to install ldisc, notify from tty_open which suggests
 *  the ldisc was properly installed.
 * @core_data: PS core's data, which mainly is the tty's disc_data
 *
 */
struct ps_plat_s {
    struct platform_device *pm_pdev;
    struct completion ldisc_installed;
    struct completion change_baud_rate;
    struct ps_core_s *core_data;
    uint8  dev_name[HISI_UART_DEV_NAME_LEN + 1];
    uint64 baud_rate;
    uint8  ldisc_install;
    uint8  flow_cntrl;
    uint16 fill_byte1;
};

/**
 * struct hw_ps_plat_data - platform data shared between PS driver and
 *  platform specific board file which adds the PS device.
 * @dev_name: The UART/TTY name to which chip is interfaced. (eg: /dev/ttyS1)
 * @flow_cntrl: Should always be 1, since UART's CTS/RTS is used for PM
 *  purposes.
 * @baud_rate: The baud rate supported by the Host UART controller, this will
 *  be shared across with the chip via a HCI VS command from User-Space Init
 *  Mgr application.
 * @suspend:
 * @resume: legacy PM routines hooked to platform specific board file, so as
 *  to take chip-host interface specific action.
 * @set_bt_power: set gpio BT_EN to high.
 */
struct hw_ps_plat_data {
    int32 (*suspend)(struct platform_device *, pm_message_t);
    int32 (*resume)(struct platform_device *);
    void  (*set_bt_power)(void);
    void  (*set_fm_power)(void);
    void  (*set_gnss_power)(void);
    void  (*clear_bt_power)(void);
    void  (*clear_fm_power)(void);
    void  (*clear_gnss_power)(void);
    uint8  dev_name[HISI_UART_DEV_NAME_LEN]; /* uart name */
    uint64 baud_rate;
    uint8  flow_cntrl; /* flow control flag */
    uint16 fill_byte1;
    uint8  fill_byte2;
};

extern int32 (*tty_recv)(void *, const uint8 *, int32);

/*****************************************************************************
  Function declare
*****************************************************************************/

extern int32 ps_pm_register(struct ps_pm_s *new_pm);

extern int32 ps_pm_unregister(struct ps_pm_s *del_pm);

int32 ps_get_plat_reference(struct ps_plat_s **plat_data);

int32 ps_bt_register(struct ps_proto_s *);

int32 ps_bt_unregister(struct ps_proto_s *);

/**
 * ps_write_tty -
 * point this to tty->driver->write or tty->ops->write
 * depending upon the kernel version
 */
int32 ps_write_tty(struct ps_core_s*, const uint8*, int32);

/**
 * ps_bt_write -
 * internal write function, passed onto protocol drivers
 * via the write function ptr of protocol struct
 */
int32 ps_bt_write(struct sk_buff *);

int32 ps_core_tx_attemper(struct ps_core_s *ps_core_d);

/* init, exit entry funcs called from PM */
int32 ps_core_init(struct ps_core_s **);
int32 ps_core_exit(struct ps_core_s *);

/**
 * functions called when 1 of the protocol drivers gets
 * registered, these need to communicate with OCTTY to request
 * ldisc installed
 */
int32 open_tty_drv(void *);

int32 release_tty_drv(void *);

int32 ps_tty_complete(void *);

int32 ps_get_core_reference(struct ps_core_s **core_data);

int32 ps_core_recv(void *disc_data, const uint8 *data, int32 count);

int32 ps_tx_sys_cmd(struct ps_core_s *ps_core_d, uint8 type, uint8 content);

int32 ps_tx_gnssbuf(struct ps_core_s *ps_core_d, const int8 __user *buf, size_t count);

int32 ps_uart_ioctl(struct ps_core_s *ps_core_d, int8 cmd, int8 coment);

int32 ps_tx_bt_add_head(struct ps_core_s *ps_core_d, struct sk_buff *skb);

int32 ps_skb_enqueue(struct ps_core_s *ps_core_d, struct sk_buff *skb, uint8 type);

int32 ps_add_packet_head(uint8 *buf, uint8 type, uint16 lenth);

int32 ps_change_baud_rate_complete(void *pm_data);

void  ps_kfree_skb(struct ps_core_s *ps_core_d, uint8 type);

struct sk_buff *ps_alloc_skb(uint16 len);

struct sk_buff *ps_skb_dequeue(struct ps_core_s *ps_core_d, uint8 type);

int32 ps_patch_write(uint8 *data, int32 count);

int32 ps_recv_patch(void *disc_data, const uint8 *data, int32 count);

int32 ps_set_sys_packet(uint8 *buf, uint8 type, uint8 content);

int32 ps_change_uart_baud_rate(int64 baud_rate);

uint8 check_bfg_not_booting(struct ps_core_s *ps_core_d);
void host_allow_bfg_sleep(struct ps_core_s *ps_core_d);
uint8  check_device_state(struct ps_core_s *ps_core_d, uint8 type);

int32 ps_check_packet_head(struct ps_core_s *ps_core_d, uint8 *buf_ptr, int32 count);

int32 open_tty_download_patch(struct ps_core_s *ps_core_d, uint8 proto);

int32 ps_chk_bfg_active(struct ps_core_s *ps_core_d);

extern struct kobject *g_sysfs_hi110x_ps;
extern struct kset	  *devices_kset;
#ifdef UT_TEST
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
#endif
#endif /* HW_BFG_PS_H */


