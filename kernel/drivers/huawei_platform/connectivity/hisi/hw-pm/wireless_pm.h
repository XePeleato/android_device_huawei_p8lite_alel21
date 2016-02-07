/******************************************************************************

          Copyright (C), 2001-2011, huawei technology limited company

 ******************************************************************************
  Filename   : wireless_pm.h
  Version    : first draft
  Author     : cwx145522
  Create date: 2012-12-10
  Modify     :
  Func descr : inclue file
  Func list  :
  Modi histoy:
  1.Data     : 2012-12-10
    Author   : cwx145522
    Modify
    content  : Create File

******************************************************************************/

#ifndef __WIRELESS_PM_H__
#define __WIRELESS_PM_H__

/*****************************************************************************
  1 Include other Head file
*****************************************************************************/

#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/wakelock.h>
#include "../hw-ps/hw_bfg_ps.h"
/*****************************************************************************
  2 Define macro
*****************************************************************************/

/*#define ENABLE_BFG_LOWPOWER_FEATURE*/
#define BFG_LOCK_NAME                   "bfg_wake_lock"

#define UART_INIT_OK                    0x01
#define SDIO_INIT_OK                    0x02

#define BT_STATE_SHUTDOWN               0x10
#define BT_STATE_BOOTUP                 0x11
#define GNSS_STATE_SHUTDOWN             0x20
#define GNSS_STATE_BOOTUP               0x21
#define FM_STATE_SHUTDOWN               0x30
#define FM_STATE_BOOTUP                 0x31
#define WLAN_STATE_SHUTDOWN             0x40
#define WLAN_STATE_BOOTUP               0x41

#define SUCC                            0
#define FAIL                            1
#define LOWLEVEL                        0
#define HIGHLEVEL                       1

#define WLAN_PINDISABLE                 1
#define BFG_PINDISABLE                  2
#define WLAN_PINSTATEENABLE             4
#define BFG_PINSTATEENABLE              8
#define PINENABLE                       0
#define PINSTATE_NUM                    2
#define PINSTATE_SWITCH_MASK            ((1 << PINSTATE_NUM) - 1)
#define PINSTATE_ENABLE_MASK            (PINSTATE_SWITCH_MASK << PINSTATE_NUM)
#define PINSTATE_MASK                   (PINSTATE_SWITCH_MASK | PINSTATE_ENABLE_MASK)

#define BT_STATE_FLAG                   0
#define GNSS_STATE_FLAG                 1
#define FM_STATE_FLAG                   2
#define WLAN_STATE_FLAG                 3
#define BT_AND_FM_STATE_FLAG            4
#define BT_AND_GNSS_STATE_FLAG          5
#define FM_AND_GNSS_STATE_FLAG          6
#define BT_FM_AND_GNSS_STATE_FLAG       7

#define EXCEPTION_RETRY_COUNTER         3

#define K3_32K_B_CLOCK_ADDR             0xFCC00140
#define K3_32K_B_CLOCK_ENABLE           0x2
#define GPIO_4_CONFIG_ADDR              0xFC80382C
#define GPIO_4_DISABLE_UP_PULLED        0xFFFE
#define GPIO_4_ENABLE_DOWN_PULLED       0x2
#define ADDR_LEN                        32

#define DTS_COMP_HI1101_POWER_NAME      "hisilicon,hi1101-power"

/* for huawei's BT, FM, GNSS, WLAN chip */

/*****************************************************************************
  3 STRUCT DEFINE
*****************************************************************************/


/*interface for wlan*/
struct wlan_pm_s
{
    int32 (*wlan_patch_download)(void);
    void (*check_wlan_state)(void);
    int32(*check_bfg_state)(void);
    int32 (*check_bt_state) (void);
    int32 (*wlan_gpio_state_get)(void);
    void (*wlan_gpio_state_set)(uint8);
    int32 (*sdio_reinit)(void);
    
	int32 (*dev_wakeup)(void);

    int32 (*wlan_patch_write)(uint8 *data, uint32 len);
    int32 (*wlan_patch_read)(uint8 *data, uint32 len);
    uint32 wlan_irq;
};

/*private data for pm driver*/
struct pm_drv_data
{
    /*3 in 1 interface pointer*/
    struct ps_pm_s                  *ps_pm_interface;

    /*wlan interface pointer*/
    struct wlan_pm_s                *wlan_pm_info;

    /*timer for bfg uart suspend*/
    //struct timer_list               bfg_timer;

    /*wake lock for bfg,be used to prevent host form suspend*/
    struct wake_lock                bfg_wake_lock;

    /*mutex for sync*/
    struct mutex                    host_mutex;

    /*Tasklet to respond to change in hostwake line */
    struct tasklet_struct           hostwake_task;

    /*bfg tx spinlock*/
    spinlock_t                      tx_lock;

    /*flag for gnss going to sleep*/
    //atomic_t                        gnss_sleep_flag;

    /*wlan state*/
    uint8 wlan_state;

    /*bt state*/
    uint8 bt_state;

    /*gnss state*/
    uint8 gnss_state;

    /*fm state*/
    uint8 fm_state;

    /*flag for patch init*/
    uint64 patch_init_flag;

    /*wlan irq num*/
    uint32 wlan_irq;

    /*bfg irq num*/
    uint32 bfg_irq;

};

enum PATCH_INFO_TYPE_ENUM
{
    ENUM_INFO_SDIO    =  0,        /*download patch by sdio*/
    ENUM_INFO_UART,                /*download patch by uart*/
    ENUM_INFO_TOTAL                /*interface total*/
};

#endif



