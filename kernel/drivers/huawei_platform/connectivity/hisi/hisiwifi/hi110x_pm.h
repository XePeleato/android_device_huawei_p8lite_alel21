/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name     : hi110x_pm.h
 * Version       : Initial Draft
 * Created       : 2013/1/10
 * Last Modified :
 * Description   : hi110x_pm.c header file
 * Function List :
 * History       :
 * 1.Date        : 2013/1/10
 *   Modification: Created file
 *
 */

#ifndef __HI110X_PM_H__
#define __HI110X_PM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <linux/mmc/sdio_func.h>
#include <linux/fb.h>
#include <linux/wakelock.h>
#include "common.h"
#include "hwifi_utils.h"
#include "hwifi_bus.h"
#include "hwifi_hcc.h"
#include "hwifi_wpa_ioctl.h" /* for wl_pwrm_set */
#include "hwifi_wl_config_ioctl.h"

/* Start of zhangwei 64406 2013-03-26 B295 增加预编译宏 */
#ifdef WLAN_POWER_MANAGEMENT
/* End of zhangwei 64406 2013-03-26 B295 增加预编译宏 */
/*
 * 2 Macro Definition
 */

/*
 * 3 Enum Type Definition
 */

#define DEFAULT_WATCHDOG_TIMEOUT                (3 * 1000)

#define MIN_WATCHDOG_TIMEOUT                    0
#define HI110X_VER_MAX_TIMEOUT                  20


#define PM_FORBID_SLEEP         (-EBUSY)

/* 省电模式配置 */
enum {
    HI110X_POWERMODE_OFF = 0,
    HI110X_POWERMODE_MIN,
    HI110X_POWERMODE_MAX,
};

/* 省电模式使能配置 */
enum {
    POWER_SAVE_DISABLE = 0,
    POWER_SAVE_ENABLE
};

/* Host屏状态 */
enum SCREEN_STATE
{
    SCREEN_OFF = 0,
    SCREEN_ON
};

enum IFACE_STATE {
    IFACE_OFF = 0,
    IFACE_ON
};

enum WLAN_PM_CPU_FREQ_ENUM
{
    WLCPU_11MHZ     =   0x17UL,                 //Wlan Cpu Freq
    WLCPU_13MHZ     =   0x18UL,
    WLCPU_16MHZ     =   0x19UL,
    WLCPU_20MHZ     =   0x1AUL,
    WLCPU_26MHZ     =   0x1BUL,
    WLCPU_40MHZ     =   0x1CUL,
    WLCPU_80MHZ     =   0x1DUL,
    WLCPU_160MHZ    =   0x1EUL,
    WLCPU_240MHZ    =   0x1FUL,
};

enum SDIO_MAXCAP_RATE_ENUM
{
    MAXCAP_11MHZ_RATE  = 0,
    MAXCAP_13MHZ_RATE  = 0,
    MAXCAP_16MHZ_RATE  = 0,
    MAXCAP_20MHZ_RATE  = 0,
    MAXCAP_26MHZ_RATE  = 0,
    MAXCAP_40MHZ_RATE  = 0,
    MAXCAP_80MHZ_RATE  = 0,
    MAXCAP_160MHZ_RATE = 0,
    MAXCAP_240MHZ_RATE = 0,
};

enum CAP_DELTA_ENUM
{
    ENTER_THRESHOLD_DELTA = 4,
    EXIT_THRESHOLD_DELTA  = 2,
};

enum GET_DEEPSLEEP_STATE_ENUM
{
    NODEEPSLEEP_STATE = 0,
    DEEPSLEEP_STATE,
};

enum
{
    HI110X_TPC_2G_MAXPOWER_GOMODE = 30,
    HI110X_TPC_5G_MAXPOWER_GOMODE = 287,
};

enum
{
    KEEPCONNECT_DISABLE = 0,
    KEEPCONNECT_ENABLE_WHENPLUGGED,
    KEEPCONNECT_ENABLE_ALWAYS
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
extern struct hi110x_pm_info         *g_pm_info;
extern struct wlan_pm_s              *g_wlan_info;

extern uint32 g_watchdog_timeout;
extern uint32 g_powermgmt_switch;
extern uint32 g_cur_cpufreq;
extern uint32 g_sdio_rate_40M;
extern uint32 g_sdio_rate_80M;
extern uint32 g_sdio_rate_160M;
extern uint32 g_sdio_rate_240M;
extern uint32 g_keepconnect_switch;

struct hi110x_device;


struct wlan_pm_s
{
    /* interface from platform */
    int32 (*wlan_patch_download)(void);
    void  (*check_wlan_state)(void);
    int32 (*check_bfg_state)(void); /* 1 means bfg power on, 0 means power off */
	int32 (*check_bt_state)(void);
    int32 (*wlan_gpio_state_get)(void); /* return value 0: GPIO down; 1: GPIO up */
    void (*wlan_gpio_state_set)(uint8 value); /* param 0: GPIO down; 1: GPIO up */

    /* provide interface for platform */
    int32 (*sdio_reinit)(void);

    int32 (*dev_wakeup)(void);

    int32 (*wlan_patch_write)(uint8 *data, uint32 len);
    int32 (*wlan_patch_read)(uint8 *data, uint32 len);
    uint32 wlan_irq;
};

typedef void (* get_deepsleep_state_cb)(int32 state);
typedef void (* set_roam_state_cb)(struct hi110x_device *hi110x_dev, uint32 state);
typedef struct _hi110x_pm_stat_
{
    uint32      sleep_count;
    uint32      wakeup_count;
    uint32      host_dev_wakeup:16;
    uint32      dev_host_wakeup:16;
    uint32      sleep_fail_count;
    uint32      wakeup_fail_count;
    unsigned long long sleep_fail_cpu_time;
    unsigned long long wakeup_fail_cpu_time;
    unsigned long long power_on_time;
    unsigned long long power_off_time;
}hi110x_pm_stat;
struct hi110x_pm_info
{
    struct hi110x_device   *hi110x_dev;
    struct timer_list       watchdog_timer;
    spinlock_t              suspend_lock;
    spinlock_t              irq_lock;
    struct sdio_func       *sdio_func;
    struct mutex            wlan_lock;
    atomic_t                wlanopen_cnt;

    int32                   powermgmt_state;    /* 是否设置powersave标志 */
    int32                   screen_state;       /* 设置屏幕状态 */
    int32                   allowsleep_flag;    /* 设置允许睡眠标志 */
    int32                   in_suspend;
    int32                   connect_finished;
    int32                   iface_state;        /* wpa_supplicant设置wlan状态 */
    int32                   roam_trigger;
    uint8                   last_cpufreq;       /* 保存上一次的cpu频率 */

#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend    early_suspend;
#else
    struct notifier_block   pm_notifier;
#endif

#ifdef CONFIG_HAS_WAKELOCK
    struct wake_lock        wklock_wifi;
    spinlock_t              wklock_spinlock;
    int32                   wklock_cnt;
    void*                   hold_func;
    int32                   hold_wklock_cnt;
#endif
    hi110x_pm_stat          pm_stat;
    get_deepsleep_state_cb  get_deepsleep_state;
    set_roam_state_cb       set_roam_state;
};

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */
#define IS_GET_IPADDR(pm_info) (TRUE == pm_info->connect_finished)
#define BPS_TO_PPS(value)   ((value)*1024*1024/(1500*8))
static inline void wlan_opera_lock(struct hi110x_pm_info *pm_info)
{
    HWIFI_BUG_ON(!pm_info);
    mutex_lock(&pm_info->wlan_lock);
}

static inline void wlan_opera_unlock(struct hi110x_pm_info *pm_info)
{
    HWIFI_BUG_ON(!pm_info);
    mutex_unlock(&pm_info->wlan_lock);
}

/*
 * 10 Function Declare
 */
 void hi110x_pm_stat_clear(void);
struct hi110x_pm_info * hi110x_pm_alloc(struct hi110x_device *hi110x_dev, struct device *dev);
void hi110x_pm_free(struct hi110x_pm_info *pm);
void hi110x_sdio_powerset_time_record(int32 poweron);
int32 hi110x_pm_dev_wakeup(void);
int32 hi110x_pm_poweron_reinit(void);
int32 hi110x_pm_dev_deinit(struct sdio_func *func);

void hi110x_wake_lock(struct hi110x_pm_info *pm_info);
void hi110x_wake_unlock(struct hi110x_pm_info *pm_info);
int  hi110x_wake_release_lock(struct hi110x_pm_info *pm_info, int locks);
void hi110x_wlan_power_set(uint8 value);
int32 hi110x_pm_powermode_set(struct cfg_struct *cfg, int32 ps_mode);

void hi110x_pm_feed_wdg(struct hi110x_pm_info *pm_info);
int32 hi110x_set_suspend(struct hi110x_pm_info *pm_info, int32 value);
int32 hi110x_get_screen_state(struct hi110x_pm_info *pm_info);
int32 hi110x_pm_start_wdg(struct hi110x_pm_info *pm_info);
int32 hi110x_pm_stop_wdg(struct hi110x_pm_info *pm_info);
int32 hi110x_fitcpu_for_throughput(struct hi110x_sdio *hi_sdio, uint32 len, uint32 pkt_cnt);
void hi110x_pm_allow_sleep(struct hi110x_pm_info *pm_info);
void hi110x_suspend_action(struct hi110x_pm_info *pm_info);
int32 hi110x_stop_powermgmt(struct hi110x_device *hi110x_dev);
int32 hi110x_start_powermgmt(struct hi110x_device *hi110x_dev, uint32 roam_results);
int32 hi110x_keepconnect_enable (struct hi110x_device *hi110x_dev);
void hi110x_keepconnect_disable (struct hi110x_device *hi110x_dev);
int32 hi110x_sleep_request(struct sdio_func *func);
void hi110x_wakelocks_release_detect(struct hi110x_pm_info *pm_info);

/* Start of zhangwei 64406 2013-03-26 B295 增加预编译宏 */
#endif
/* End of zhangwei 64406 2013-03-26 B295 增加预编译宏 */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hi110x_pm.h */
