/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 *
 * File Name     : wireless_pm.c
 * Version       : Hi1101
 * Author        : cwx145522
 * Created       : 2012/12/10
 * Last Modified :
 * Description   :
 * Function List :
              bfg_dev_power_control
              bfg_dev_power_off
              bfg_gpio_get
              bfg_gpio_set
              bfg_hostwake_task
              bfg_patch_download_function
              bfg_patch_recv
              bfg_sleep_state_show
              bfg_sleep_wakeup
              bfg_spin_lock
              bfg_wake_host_isr
              bfg_wake_lock
              bt_power_state_show
              bt_power_state_store
              check_bfg_state
              check_dev_is_shutdown
              check_wlan_state
              coex_mode_store
              fm_power_state_show
              fm_power_state_store
              gnss_power_state_show
              gnss_power_state_store
              gpio_request_and_init
              hi110x_power_off
              hi110x_power_on
              host_wake_wlan
              low_power_exit
              low_power_init
              low_power_probe
              low_power_remove
              low_power_resume
              low_power_suspend
              pm_data_recv
              pm_sdio_send
              pm_uart_send
              pm_uart_set_baudrate
              ps_pm_register
              ps_pm_unregister
              release_mutex_lock
              set_bfg_state
              wlan_gpio_get
              wlan_gpio_set
              wlan_patch_download_function
              wlan_patch_recv
              wlan_pm_register
              wlan_pm_unregister
              wlan_power_set
              wlan_sleep_state_show

 * History       :
 * 1.Date        : 2012/12/10
 *   Author      : cwx145522
 *   Modification: Created file
 *
 */

/*****************************************************************************
  1 Header File Including
*****************************************************************************/

#include <linux/module.h>   /* kernel module definitions */
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/kobject.h>
#include <linux/irq.h>
#include <linux/wakelock.h>
#ifndef UT_TEST
#include <linux/slab.h>
#endif
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/gpio.h>
//#include <mach/gpio.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#endif
#include "../hw-ps/hw_bfg_ps.h"
#include "wireless_dev.h"
#include "wireless_pm.h"
#include "wireless_board.h"


/*****************************************************************************
  2 Global Variable Definition
*****************************************************************************/

struct platform_device *g_pm_device;
STATIC struct kobject *g_sysfs_hi110x_power = NULL;

uint8  powerpin_state = PINENABLE;

/*bfg download patch function*/
int32 bfg_patch_download_function(void);

/*judge whether the wake lock is active or not*/
extern int wake_lock_active(struct wake_lock *lock);

/*function for download patch(sdio or uart)*/
extern int32 patch_download_patch(int32 type);

/*function for patch init(sdio or uart)*/
extern int32 patch_init(int32 type);
extern void reset_uart_rx_buf(void);

/*function for patch uninstall*/
extern int32 patch_exit(void);

/*function for patch receive by uart*/
extern int32 uart_recv_data(const uint8 *data, int32 len);

#ifdef FEATURE_BTCOEX_TEST
/*wifi-bt coexistence mode set function, definition by BT driver*/
extern int btcoex_set_dhcp_mode(int mode);
#endif

/*****************************************************************************
  3 Function Definition
*****************************************************************************/

/*****************************************************************************
 * Prototype    : pm_uart_set_baudrate
 * Description  : set baudrate of uart0 when download patch
 * Input        : int64 baudrate: the baudrate want to set
 * Output       : no
 * Return       : 0 means succeed,-1 means failed.
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 pm_uart_set_baudrate(int64 baudrate)
{
    /*get platform driver data*/
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    PS_PRINT_DBG("set uart baudrate to %ld\n", baudrate);

    /*call interface supplied by 3 in 1*/
    if (NULL != pm_data->ps_pm_interface->change_baud_rate)
    {
        /*set uart baudrate*/
        return pm_data->ps_pm_interface->change_baud_rate(baudrate);
    }

    PS_PRINT_ERR("change_baud_rate is NULL!\n");

    return -FAIL;
}

/*****************************************************************************
 * Prototype    : pm_uart_send
 * Description  : uart patch transmit function
 * Input        : uint8 *date: address of data
                  int32 len: length of data
 * Output       : no
 * Return       : length which has been sent
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 pm_uart_send(uint8 *data, int32 len)
{
    uint16  count   = 0;

    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*call interface supplied by 3 in 1*/
    if (NULL == pm_data->ps_pm_interface->write_patch)
    {
        PS_PRINT_ERR("bfg_write_patch is NULL!\n");
        return -FAIL;
    }

    while(1)
    {
        /*this function return the length has been sent*/
        count = pm_data->ps_pm_interface->write_patch(data, len);
        /*data has been sent over and return*/
        if(count == len)
        {
            return len;
        }

        /*data has not been sent over, we will send again*/
        data = data + count;
        len  = len  - count;
        msleep(1);
    }
}

/*****************************************************************************
 * Prototype    : pm_change_patch_state
 * Description  :
 * Input        :

 * Output       : no
 * Return       :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/12/06
 *     Author       : zWX144390
 *     Modification : Created function

*****************************************************************************/
int32 pm_change_patch_state(void)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*call interface supplied by 3 in 1*/
    if (NULL == pm_data->ps_pm_interface->change_patch_to_nomal)
    {
        PS_PRINT_ERR("change_patch_to_nomal is NULL!\n");
        return -FAIL;
    }
    pm_data->ps_pm_interface->change_patch_to_nomal();
    return 0;
}
EXPORT_SYMBOL(pm_change_patch_state);

/*****************************************************************************
 * Prototype    : pm_sdio_send
 * Description  : sdio patch transmit function
 * Input        : uint8 *date: address of data
                  int32 len: length of data
 * Output       : no
 * Return       : length which has been sent
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 pm_sdio_send(uint8 *date, int32 len)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*call interface supplied wlan driver to send wlan patch data*/
    if (NULL != pm_data->wlan_pm_info->wlan_patch_write)
    {
        return pm_data->wlan_pm_info->wlan_patch_write(date, len);
    }

    PS_PRINT_ERR("wlan_patch_write is NULL!\n");
    return -FAIL;
}

/*****************************************************************************
 * Prototype    : wlan_patch_recv
 * Description  : function for patch receive by sdio
 * Input        : uint8 *data: address of data
                  int32 count: length of data
 * Output       : no
 * Return       : length which has been sent
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 wlan_patch_recv(uint8 *data, int32 count)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*call interface supplied wlan driver to recv wlan patch data*/
    if (NULL != pm_data->wlan_pm_info->wlan_patch_read)
    {
        return pm_data->wlan_pm_info->wlan_patch_read(data, count);
    }

    PS_PRINT_ERR("wlan_patch_read is NULL!\n");
    return -FAIL;
}

/*****************************************************************************
 * Prototype    : bfg_patch_recv
 * Description  : function for bfg patch receive
 * Input        : uint8 *data: address of data
                  int32 count: length of data
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 bfg_patch_recv(const uint8 *data, int32 count)
{
    int32 ret;

    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*this function should be called after patch_init(uart),
      otherwise maybe null pointer*/
    if (test_bit(UART_INIT_OK, &pm_data->patch_init_flag))
    {
        ret = uart_recv_data(data, count);
        return ret;
    }

    return SUCC;
}

void bfg_mutex_lock(uint8 on)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return ;
    }

    if (on)
    {
        mutex_lock(&pm_data->host_mutex);
    }
    else
    {
        mutex_unlock(&pm_data->host_mutex);
    }
}

/*****************************************************************************
 * Prototype    : bfg_gpio_set
 * Description  : bfg sleep control function
 * Input        : uint8 on: 0 means agree sleep ,1 means disagree.
 * Output       : none
 * Return       : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
void bfg_gpio_set(uint8 on)
{
    if (on)
    {
        /*disagree bfg to sleep*/
        reset_uart_rx_buf();
        gpio_direction_output(g_wire_dev->host_wakeup_bfg,HIGHLEVEL);
        PS_PRINT_SUC("host wakeup bfg by pull up gpio\n");
    }
    else
    {
        /*agree bfg to sleep*/
        gpio_direction_output(g_wire_dev->host_wakeup_bfg,LOWLEVEL);
    }
}

/*****************************************************************************
 * Prototype    : bfg_gpio_get
 * Description  : judge whether bfg is sleep or not via gpio
 * Input        : uint8 on: 0 means sleep ,1 means active.
 * Output       : none
 * Return       : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 bfg_gpio_get(void)
{
    /*check bfg state via gpio*/
    return gpio_get_value(g_wire_dev->host_wakeup_bfg);
}

/*****************************************************************************
 * Prototype    : bfg_wake_lock
 * Description  : control bfg wake lock
 * Input        : uint8 on: 0 means wake unlock ,1 means wake lock.
 * Output       : none
 * Return       : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
void bfg_wake_lock(uint8 on)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return;
    }

    if (on)
    {
        /*wake lock*/
        wake_lock(&pm_data->bfg_wake_lock);
    }
    else
    {
        /*wake unlock*/
        wake_unlock(&pm_data->bfg_wake_lock);
    }
}

/*****************************************************************************
 * Prototype    : bfg_spin_lock
 * Description  : control bfg spin lock
 * Input        : uint8 on: 0 means spin unlock ,1 means spin lock.
 * Output       : none
 * Return       : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
void bfg_spin_lock(uint8 on)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return;
    }

    if (on)
    {
        /*spin lock*/
        spin_lock_bh(&pm_data->tx_lock);
    }
    else
    {
        /*spin unlock*/
        spin_unlock_bh(&pm_data->tx_lock);
    }
}

/*****************************************************************************
 * Prototype    : wlan_gpio_set
 * Description  : wlan sleep control function
 * Input        : uint8 on: 0 means agree sleep ,1 means disagree.
 * Output       : none
 * Return       : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
void wlan_gpio_set(uint8 on)
{
    if (on)
    {
        /*disagree wlan to sleep*/
        gpio_direction_output(g_wire_dev->host_wakeup_wlan, HIGHLEVEL);
    }
    else
    {
        /*agree wlan to sleep*/
        gpio_direction_output(g_wire_dev->host_wakeup_wlan, LOWLEVEL);
    }

}

/*****************************************************************************
 * Prototype    : wlan_gpio_get
 * Description  : judge whether wlan is sleep or not via gpio
 * Input        : uint8 on: 0 means sleep ,1 means active.
 * Output       : none
 * Return       : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 wlan_gpio_get(void)
{
    /*check wlan state via gpio*/
    return gpio_get_value(g_wire_dev->host_wakeup_wlan);
}

/*****************************************************************************
 * Prototype    : host_wake_wlan
 * Description  : host wake up wlan device by gpio
 * Input        : none
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 host_wake_wlan(void)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*wake up wlan via gpio*/
    gpio_direction_output(g_wire_dev->host_wakeup_wlan, HIGHLEVEL);

    /*wait for device hardware init complete*/
    mdelay(10);

    /*reinit SDIO after wake up wlan*/
    if (NULL != pm_data->wlan_pm_info->dev_wakeup)
    {
        return pm_data->wlan_pm_info->dev_wakeup();
    }


    return -FAIL;

}

/*****************************************************************************
 * Prototype    : bfg_sleep_wakeup
 * Description  : function for bfg device wake up
 * Input        : none
 * Output       : none
 * Return       : none
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
void bfg_sleep_wakeup(void)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return ;
    }

    PS_PRINT_INFO("bfg sleep wakeup!\n");
    /*wake up bfg via gpio*/
    reset_uart_rx_buf();
    gpio_direction_output(g_wire_dev->host_wakeup_bfg, HIGHLEVEL);
    /*mod beat timer to 3s*/
    pm_data->ps_pm_interface->reset_beat_timer(1);
    /*wake lock host after wakeup bfg*/
    wake_lock(&pm_data->bfg_wake_lock);
}

/*****************************************************************************
 * Prototype    : set_bfg_state
 * Description  : functions called after bt/gnss/fm power on to set the state
 * Input        : uint8 type: means bt/gnss/fm
 * Output       : no
 * Return       : 0 means succeed,-1 means failed.
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 set_bfg_state(uint8 type)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*set bt/fm/gnss state to bootup*/
    if (BT_STATE_FLAG == type)
    {
        pm_data->bt_state = BT_STATE_BOOTUP;
    }
    else if (GNSS_STATE_FLAG == type)
    {
        pm_data->gnss_state = GNSS_STATE_BOOTUP;
    }
    else if (FM_STATE_FLAG == type)
    {
        pm_data->fm_state = FM_STATE_BOOTUP;
    }

    return SUCC;
}

/*****************************************************************************
 * Prototype    : check_dev_is_shutdown
 * Description  : check device(wlan/bt/fm/gnss) is shutdown or not
 * Input        : uint8 type: means one or more system to check
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/07/29
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 check_dev_is_shutdown(uint8 type)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    switch(type)
    {
        case BT_STATE_FLAG:
            return ((BT_STATE_SHUTDOWN == pm_data->bt_state)
            ? true : false);
        case GNSS_STATE_FLAG:
            return ((GNSS_STATE_SHUTDOWN == pm_data->gnss_state)
            ? true : false);
        case FM_STATE_FLAG:
            return ((FM_STATE_SHUTDOWN == pm_data->fm_state)
            ? true : false);
        case WLAN_STATE_FLAG:
            return ((WLAN_STATE_SHUTDOWN == pm_data->wlan_state)
            ? true : false);
        case BT_AND_FM_STATE_FLAG:
            return (((BT_STATE_SHUTDOWN == pm_data->bt_state)
            && (FM_STATE_SHUTDOWN == pm_data->fm_state))
            ? true : false);
        case BT_AND_GNSS_STATE_FLAG:
            return (((BT_STATE_SHUTDOWN == pm_data->bt_state)
            && (GNSS_STATE_SHUTDOWN == pm_data->gnss_state))
            ? true : false);
        case FM_AND_GNSS_STATE_FLAG:
            return (((FM_STATE_SHUTDOWN == pm_data->fm_state)
            && (GNSS_STATE_SHUTDOWN == pm_data->gnss_state))
            ? true : false);
        case BT_FM_AND_GNSS_STATE_FLAG:
            return (((BT_STATE_SHUTDOWN == pm_data->bt_state)
            && (FM_STATE_SHUTDOWN == pm_data->fm_state)
            && (GNSS_STATE_SHUTDOWN == pm_data->gnss_state))
            ? true : false);
        default:
            PS_PRINT_ERR("invalid type input!\n");
            break;
    }

    return -FAIL;
}

void power_reset(void)
{

    /*pull up power on gpio*/
    mdelay(1);
    gpio_direction_output(g_wire_dev->power_on, LOWLEVEL);
    mdelay(10);
    gpio_direction_output(g_wire_dev->power_on, HIGHLEVEL);
    mdelay(1);
    /* make sure xldo vet is 1.8v.
     * If open bfg,this gpio should be pull up after en is up.
     */
    gpio_direction_output(g_wire_dev->host_wakeup_bfg, LOWLEVEL);
}

int32 bfg_dev_power_on(void)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*if other two systems are both off, we should wake lock host*/
    wake_lock(&pm_data->bfg_wake_lock);

    /*pull down bt_en for reset*/
    gpio_direction_output(g_wire_dev->bfg_power_on, LOWLEVEL);

    /*if wlan is off, we must pull up power on gpio*/
    if (check_dev_is_shutdown(WLAN_STATE_FLAG))
    {
        /*pull up power on gpio*/
        if (PINENABLE == (powerpin_state & (powerpin_state >> PINSTATE_NUM)))
        {
            gpio_direction_output(g_wire_dev->power_on, LOWLEVEL);
        }
        mdelay(10);
        gpio_direction_output(g_wire_dev->power_on, HIGHLEVEL);

        /*xldo vet*/
        gpio_direction_output(g_wire_dev->host_wakeup_bfg, LOWLEVEL);
    }

    mdelay(10);
    /*pull up bt_en*/
    gpio_direction_output(g_wire_dev->bfg_power_on, HIGHLEVEL);

    mdelay(1);

    /* pull up gpio to disagree bfg public section sleep*/
    gpio_direction_output(g_wire_dev->host_wakeup_bfg, HIGHLEVEL);

#ifndef Hi110X_VERSION_ROM
    /*wait 8s for device count down*/
    msleep(8000);
#else
    /*wait 10ms for device software bootup*/
    mdelay(10);
#endif
    return SUCC;
}

/*****************************************************************************
 * Prototype    : hi110x_power_on
 * Description  : hi110x device power on function
 * Input        : uint8 type: means one of wlan/bt/fm/gnss
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/07/29
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 hi110x_power_on(uint8 type)
{
    int32 state                 = 0;

    /*get pm driver data*/
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    if (WLAN_STATE_FLAG == type)
    {
        mutex_lock(&pm_data->host_mutex);

        PS_PRINT_INFO("wlan power on!\n");

        /*pull down wlan_en for reset*/
        gpio_direction_output(g_wire_dev->wlan_power_on, LOWLEVEL);

        /*if bfg are both off, we must pull up power on gpio*/
        if (check_dev_is_shutdown(BT_FM_AND_GNSS_STATE_FLAG))
        {
            /*pull up power on gpio*/
            if (PINENABLE == (powerpin_state & (powerpin_state >> PINSTATE_NUM)))
            {
                gpio_direction_output(g_wire_dev->power_on, LOWLEVEL);
            }
            mdelay(10);
            gpio_direction_output(g_wire_dev->power_on, HIGHLEVEL);

            /*xldo set*/
            gpio_direction_output(g_wire_dev->host_wakeup_bfg, LOWLEVEL);
        }
        else
        {
            PS_PRINT_INFO("will wakeup bfg before download wifi patch!\n");
            spin_lock_bh(&pm_data->tx_lock);
            /*check device state via gpio, if sleep ,we should wake up*/
            if (LOWLEVEL == gpio_get_value(g_wire_dev->host_wakeup_bfg))
            {
                bfg_sleep_wakeup();
                /* bt,fm,gnss all reset timer, filter gnss in bfg_timer_expire */
                /* mod bt_timer to 800ms */
                pm_data->ps_pm_interface->reset_bt_timer(8);
                spin_unlock_bh(&pm_data->tx_lock);
                /*wait for device wake up and init*/
                mdelay(10);
            }
            else
            {
                spin_unlock_bh(&pm_data->tx_lock);
            }

        }
        mdelay(10);
        /*pull up gpio to disagree wlan sleep*/
        gpio_direction_output(g_wire_dev->host_wakeup_wlan, HIGHLEVEL);
        mdelay(1);
        /*pull up wlan_en gpio*/
        gpio_direction_output(g_wire_dev->wlan_power_on, HIGHLEVEL);

#ifndef Hi110X_VERSION_ROM
        /*wait 8s for device count down*/
        msleep(8000);
#else
        /*wait for device software bootup*/
        msleep(100);
#endif
    }
    else
    {
        if (BT_STATE_FLAG == type)
        {
            PS_PRINT_INFO("bt power on!\n");
            state = check_dev_is_shutdown(FM_AND_GNSS_STATE_FLAG);
        }
        else if (FM_STATE_FLAG == type)
        {
            PS_PRINT_INFO("fm power on!\n");
            state = check_dev_is_shutdown(BT_AND_GNSS_STATE_FLAG);
        }
        else
        {
            PS_PRINT_INFO("gnss power on!\n");
            state = check_dev_is_shutdown(BT_AND_FM_STATE_FLAG);
        }

        /*if other two systems are both off, we must pull up bt_en*/
        if (state)
        {
            bfg_dev_power_on();
        }
        else
        {
        /*if there is a system(bt/fm/gnss) on, we just need to judge
          whether is deep-sleep or not*/
            spin_lock_bh(&pm_data->tx_lock);
            if (LOWLEVEL == gpio_get_value(g_wire_dev->host_wakeup_bfg))
            {
                PS_PRINT_SUC("host wakeup bfg for power on one of bfg\n");
                bfg_sleep_wakeup();
                if (GNSS_STATE_FLAG != type)
                {
                    pm_data->ps_pm_interface->reset_bt_timer(1);
                }
                spin_unlock_bh(&pm_data->tx_lock);

                /*wait for device wake up and init*/
                mdelay(10);
            }
            else
            {
                spin_unlock_bh(&pm_data->tx_lock);
            }
        }
    }

    return SUCC;
}

/*****************************************************************************
 * Prototype    : bfg_dev_power_off
 * Description  : bfg device power off function
 * Input        : void
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/07/29
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 bfg_dev_power_off(void)
{
    /*get pm driver data*/
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*delete bfg beat timer*/
    pm_data->ps_pm_interface->reset_beat_timer(0);

    /*pull down the gpio used by bfg low power*/
    gpio_direction_output(g_wire_dev->host_wakeup_bfg, LOWLEVEL);

    /*pull down bt_en*/
    gpio_direction_output(g_wire_dev->bfg_power_on, LOWLEVEL);

    /*if wlan is shutdown, we should pull down power_on gpio*/
    if (check_dev_is_shutdown(WLAN_STATE_FLAG))
    {
        if (PINENABLE == (powerpin_state & (powerpin_state >> PINSTATE_NUM)))
        {
            gpio_direction_output(g_wire_dev->power_on, LOWLEVEL);
        }
    }

    /*if wakelock is active, we should wake unlock*/
    if (wake_lock_active(&pm_data->bfg_wake_lock))
    {
        wake_unlock(&pm_data->bfg_wake_lock);
    }

    return SUCC;
}

int32 bfg_exception_power_control(uint8 on)
{
    /*get pm driver data*/
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    if (on)
    {
        bfg_dev_power_on();
    }
    else
    {
        /*delete bfg low-power timer*/
        pm_data->ps_pm_interface->reset_bt_timer(0);
        bfg_dev_power_off();
    }
	return 0;
}

/*****************************************************************************
 * Prototype    : hi110x_power_off
 * Description  : hi110x device power off function
 * Input        : uint8 type: means one of wlan/bt/fm/gnss
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/07/29
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 hi110x_power_off(uint8 type)
{
    int32 state                 = 0;

    /*get pm driver data*/
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    if (WLAN_STATE_FLAG == type)
    {
        mutex_lock(&pm_data->host_mutex);
        PS_PRINT_INFO("wlan power off!\n");

        /*pull down wlan_en*/
        gpio_direction_output(g_wire_dev->wlan_power_on, LOWLEVEL);

        gpio_direction_output(g_wire_dev->host_wakeup_wlan, LOWLEVEL);

        /*if bfg are both off, we must pull down power on*/
        if (check_dev_is_shutdown(BT_FM_AND_GNSS_STATE_FLAG))
        {
            /*pull down power on*/
            if (PINENABLE == (powerpin_state & (powerpin_state >> PINSTATE_NUM)))
            {
                gpio_direction_output(g_wire_dev->power_on, LOWLEVEL);
            }
        }

        /*set wlan state to shutdown*/
        pm_data->wlan_state = WLAN_STATE_SHUTDOWN;

        mutex_unlock(&pm_data->host_mutex);
    }
    /*when power off gnss, we don't care bfg low power timer*/
    else if (GNSS_STATE_FLAG == type)
    {
        PS_PRINT_INFO("gnss power off!\n");

        if (check_dev_is_shutdown(BT_AND_FM_STATE_FLAG))
        {
            bfg_dev_power_off();
        }

        /*set gnss state to shutdown*/
        pm_data->gnss_state = GNSS_STATE_SHUTDOWN;
    }
    /*when power off bt or fm, we should judge the state of the other*/
    else
    {
        if (BT_STATE_FLAG == type)
        {
            PS_PRINT_INFO("bt power off!\n");
            state = check_dev_is_shutdown(FM_STATE_FLAG);
        }
        else if (FM_STATE_FLAG == type)
        {
            PS_PRINT_INFO("fm power off!\n");
            state = check_dev_is_shutdown(BT_STATE_FLAG);
        }

        if (state)
        {
            /*delete bfg low-power timer*/
            pm_data->ps_pm_interface->reset_bt_timer(0);

            if (check_dev_is_shutdown(GNSS_STATE_FLAG))
            {
                bfg_dev_power_off();
            }
        }

        if (BT_STATE_FLAG == type)
        {
            /*set bt state to shutdown*/
            pm_data->bt_state = BT_STATE_SHUTDOWN;
        }
        else if (FM_STATE_FLAG == type)
        {
            /*set fm state to shutdown*/
            pm_data->fm_state = FM_STATE_SHUTDOWN;
        }
    }

    return SUCC;
}

/*****************************************************************************
 * Prototype    : bfg_dev_power_control
 * Description  : bfg power control function
 * Input        : int32 flag: 1 means on, 0 means off
                  uint8 type: means one of bfg
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/07/29
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 bfg_dev_power_control(uint8 type, uint8 flag)
{
    int32 ret                   = 0;

    if (1 == flag)
    {
        /*bt/fm/gnss power on*/
        ret = hi110x_power_on(type);
        if (ret)
        {
            PS_PRINT_ERR("bfg power on is error!\n");
        }
    }
    else if (0 == flag)
    {
        /*bt/fm/gnss power off*/
        ret = hi110x_power_off(type);
        if (ret)
        {
            PS_PRINT_ERR("bfg power off is error!\n");
        }
    }
    else
    {
        PS_PRINT_ERR("invalid input data!\n");
        ret = -FAIL;
    }

    return ret;
}

/*****************************************************************************
 * Prototype    : wlan_power_set
 * Description  : wlan power control function(on or off)
 * Input        : uint8 on: 1 means on, 0 means off
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 wlan_power_set(uint8 on)
{
    int ret;

    if (on)
    {
        /*wlan power on*/
        ret = hi110x_power_on(WLAN_STATE_FLAG);
        if (ret)
        {
            PS_PRINT_ERR("wlan power on is error\n");
        }
    }
    else
    {
        /*wlan power off*/
        ret = hi110x_power_off(WLAN_STATE_FLAG);
        if (ret)
        {
            PS_PRINT_ERR("wlan power off is error\n");
        }
    }

    return ret;
}
EXPORT_SYMBOL(wlan_power_set);

int32 wlan_power_off(void)
{
	/*get pm driver data*/
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

	PS_PRINT_INFO("wlan power off!\n");

	/*pull down wlan_en*/
	gpio_direction_output(g_wire_dev->wlan_power_on, LOWLEVEL);

	gpio_direction_output(g_wire_dev->host_wakeup_wlan, LOWLEVEL);

	/*if bfg are both off, we must pull down power on*/
	if (check_dev_is_shutdown(BT_FM_AND_GNSS_STATE_FLAG))
	{
		/*pull down power on*/
	    /* this functio is called only by insmod wifi.ko */
        /* so no need to judge powerpin_state */
		gpio_direction_output(g_wire_dev->power_on, LOWLEVEL);
	}

	/*set wlan state to shutdown*/
	pm_data->wlan_state = WLAN_STATE_SHUTDOWN;

	mutex_unlock(&pm_data->host_mutex);

	return SUCC;
}

EXPORT_SYMBOL(wlan_power_off);


/*****************************************************************************
 * Prototype    : wlan_patch_download_function
 * Description  : download wlan patch
 * Input        : void
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int wlan_patch_download_function(void)
{
    int32 ret                   = 0;
    int32 counter               = 0;

    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        mutex_unlock(&pm_data->host_mutex);
        return -FAIL;
    }

    PS_PRINT_DBG("enter\n");

    /*patch_init(sdio) function should just be called once*/
    if (!test_bit(SDIO_INIT_OK, &pm_data->patch_init_flag))
    {
        ret = patch_init(ENUM_INFO_SDIO);
        if (ret)
        {
            PS_PRINT_ERR("patch modem init failed, ret:%d!\n", ret);
            mutex_unlock(&pm_data->host_mutex);
            return ret;
        }

        set_bit(SDIO_INIT_OK, &pm_data->patch_init_flag);
    }

    /*wlan patch download, three times for fail*/
    for (counter = 0; counter < EXCEPTION_RETRY_COUNTER; counter++)
    {
        ret = patch_download_patch(ENUM_INFO_SDIO);
        if (ret)
        {
            PS_PRINT_ERR("wlan patch download fail, and reset power!\n");
            gpio_direction_output(g_wire_dev->wlan_power_on, LOWLEVEL);

            /* if bfg are both off, we must pull up power on gpio */
            if (check_dev_is_shutdown(BT_FM_AND_GNSS_STATE_FLAG))
            {
                PS_PRINT_SUC("wifi reset poweron!\n");
                power_reset();
            }
            mdelay(10);
            gpio_direction_output(g_wire_dev->wlan_power_on, HIGHLEVEL);
            mdelay(1);

            /*reinit SDIO after power on*/
            if (NULL != pm_data->wlan_pm_info->sdio_reinit)
            {
                pm_data->wlan_pm_info->sdio_reinit();
            }
        }
        else
        {
            /*patch download successfully and set wlan state to bootup*/
            pm_data->wlan_state = WLAN_STATE_BOOTUP;
            mutex_unlock(&pm_data->host_mutex);
			msleep(100);

			return ret;
        }
    }

    /*going to exception*/
    PS_PRINT_ERR("wlan patch download failed finally!\n");
    mutex_unlock(&pm_data->host_mutex);
    return -FAIL;

}

/*****************************************************************************
 * Prototype    : check_wlan_state
 * Description  : check whether wlan is sleep or not by gpio
 * Input        : void
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
void check_wlan_state(void)
{
    int ret = 0;

    /*check wlan state via gpio, low means sleep*/
    if(LOWLEVEL == gpio_get_value(g_wire_dev->host_wakeup_wlan))
    {
        ret = host_wake_wlan();
        if (ret)
        {
            PS_PRINT_ERR("host wake wlan is failed!\n");
            return;
        }
    }

}

/*****************************************************************************
 * Prototype    : check_bfg_state
 * Description  : check whether bfg is power on or not by gpio
 * Input        : void
 * Output       : int32
 * Return       : 1 means power on,0 means power off
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/05/08
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 check_bfg_state(void)
{
    return gpio_get_value(g_wire_dev->bfg_power_on);
}



int32 check_bt_state(void)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    PS_PRINT_DBG("enter\n");
    if (BT_STATE_SHUTDOWN == pm_data->bt_state)
    {
        return 0;
    }
    else if (BT_STATE_BOOTUP == pm_data->bt_state)
    {
        return 1;
    }
    else
    {
        return -FAIL;
    }
}

/*****************************************************************************
 * Prototype    : wlan_pm_register
 * Description  : register interface for wlan
 * Input        : struct wlan_pm_s *new_pm: interface want to register
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 wlan_pm_register(struct wlan_pm_s *new_pm)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    PS_PRINT_DBG("enter\n");

    if (NULL == new_pm)
    {
        PS_PRINT_ERR("new_pm is NULL!\n");
        return -FAIL;
    }

    pm_data->wlan_pm_info                          = new_pm;
    pm_data->wlan_pm_info->wlan_irq                = pm_data->wlan_irq;
    pm_data->wlan_pm_info->wlan_patch_download     = wlan_patch_download_function;
    pm_data->wlan_pm_info->check_wlan_state        = check_wlan_state;
    pm_data->wlan_pm_info->check_bfg_state         = check_bfg_state;
    pm_data->wlan_pm_info->check_bt_state          = check_bt_state;
    pm_data->wlan_pm_info->wlan_gpio_state_set     = wlan_gpio_set;
    pm_data->wlan_pm_info->wlan_gpio_state_get     = wlan_gpio_get;

    return SUCC;
}

EXPORT_SYMBOL(wlan_pm_register);

/*****************************************************************************
 * Prototype    : wlan_pm_unregister
 * Description  : unregister interface for wlan
 * Input        : struct wlan_pm_s *new_pm: interface want to unregister
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
void wlan_pm_unregister(struct wlan_pm_s *new_pm)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return ;
    }

    PS_PRINT_DBG("enter\n");

    if (NULL == new_pm)
    {
        PS_PRINT_ERR("new_pm is NULL!\n");
        return ;
    }

    pm_data->wlan_pm_info           = NULL;
    new_pm->wlan_patch_download     = NULL;
    new_pm->check_wlan_state        = NULL;
    new_pm->wlan_gpio_state_set     = NULL;
    new_pm->wlan_gpio_state_get     = NULL;

}

EXPORT_SYMBOL(wlan_pm_unregister);

void powerpin_state_set(uint8 value)
{
    powerpin_state |= value & PINSTATE_MASK;
}

/*****************************************************************************
 * Prototype    : ps_pm_register
 * Description  : register interface for 3 in 1
 * Input        : struct ps_pm_s *new_pm: interface want to register
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 ps_pm_register(struct ps_pm_s *new_pm)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL\n");
        return -FAIL;
    }

    PS_PRINT_DBG("enter\n");

    if (NULL == new_pm)
    {
        PS_PRINT_ERR("new_pm is null!\n");
        return -FAIL;
    }

    pm_data->ps_pm_interface                        = new_pm;
    pm_data->ps_pm_interface->download_patch        = bfg_patch_download_function;
    pm_data->ps_pm_interface->recv_patch            = bfg_patch_recv;
    pm_data->ps_pm_interface->set_pm_state          = set_bfg_state;
    pm_data->ps_pm_interface->bfg_mutex_lock        = bfg_mutex_lock;
    pm_data->ps_pm_interface->bfg_wake_lock         = bfg_wake_lock;
    pm_data->ps_pm_interface->bfg_spin_lock         = bfg_spin_lock;
    pm_data->ps_pm_interface->bfg_gpio_state_get    = bfg_gpio_get;
    pm_data->ps_pm_interface->bfg_gpio_state_set    = bfg_gpio_set;
    pm_data->ps_pm_interface->bfg_power_set         = bfg_dev_power_control;
    pm_data->ps_pm_interface->reset_bfg_power_gpio  = bfg_exception_power_control;
    pm_data->ps_pm_interface->disable_bfg_powerpin_state  = powerpin_state_set;

    PS_PRINT_SUC("pm registered over!");

    return SUCC;
}
EXPORT_SYMBOL_GPL(ps_pm_register);

/*****************************************************************************
 * Prototype    : ps_pm_unregister
 * Description  : unregister interface for 3 in 1
 * Input        : struct ps_pm_s *new_pm: interface want to unregister
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int32 ps_pm_unregister(struct ps_pm_s *new_pm)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL\n");
        return -FAIL;
    }

    PS_PRINT_DBG("enter\n");

    if (NULL == new_pm)
    {
        PS_PRINT_ERR("new_pm is null!\n");
        return -FAIL;
    }

    pm_data->ps_pm_interface        = NULL;
    new_pm->download_patch          = NULL;
    new_pm->recv_patch              = NULL;
    new_pm->set_pm_state            = NULL;
    new_pm->bfg_mutex_lock          = NULL;
    new_pm->bfg_wake_lock           = NULL;
    new_pm->bfg_spin_lock           = NULL;
    new_pm->bfg_gpio_state_get      = NULL;
    new_pm->bfg_gpio_state_set      = NULL;
    new_pm->reset_bfg_power_gpio    = NULL;
    PS_PRINT_SUC("pm unregistered over!");

    return SUCC;
}
EXPORT_SYMBOL_GPL(ps_pm_unregister);

/*****************************************************************************
 * Prototype    : bfg_patch_download_function
 * Description  : download bfg patch
 * Input        : void
 * Output       : none
 * Return       : 0 means succeed,-1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/01/22
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
int bfg_patch_download_function(void)
{
    int32 ret       = 0;
    int32 err       = 0;
    int32 counter   = 0;

    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    PS_PRINT_DBG("enter\n");

    /*patch_init(uart) function should just be called once*/
    if (!test_bit(UART_INIT_OK, &pm_data->patch_init_flag))
    {
        ret = patch_init(ENUM_INFO_UART);
        if (ret)
        {
            PS_PRINT_ERR("patch modem init failed, ret:%d!\n", ret);
            return ret;
        }

        set_bit(UART_INIT_OK, &pm_data->patch_init_flag);
    }

    /*bfg patch download, three times for fail*/
    for (counter = 0; counter < EXCEPTION_RETRY_COUNTER; counter++)
    {
        ret = patch_download_patch(ENUM_INFO_UART);
        if (ret)
        {
            PS_PRINT_ERR("bfg patch download fail, and reset power!\n");

            err = pm_uart_set_baudrate(DEFAULT_BAUD_RATE);
            if (err)
            {
                PS_PRINT_ERR(" modify baudrate fail!!\n");
                return err;
            }

            gpio_direction_output(g_wire_dev->bfg_power_on, LOWLEVEL);

            /*if wlan is off, we must pull up power on gpio*/
            if (check_dev_is_shutdown(WLAN_STATE_FLAG))
            {
                PS_PRINT_SUC("bfg reset poweron!\n");
                power_reset();
            }
            mdelay(10);
            gpio_direction_output(g_wire_dev->bfg_power_on, HIGHLEVEL);
            mdelay(1);
            /* pull up gpio to disagree bfg public section sleep*/
            gpio_direction_output(g_wire_dev->host_wakeup_bfg, HIGHLEVEL);
        }
        else
        {
            /*download patch successfully*/
            return ret;
        }
    }

    /*going to exception*/
    PS_PRINT_ERR("bfg patch download has failed finally!\n");
    return ret;

}

/*****************************************************************************
 * Prototype    : bfg_hostwake_task
 * Description  : A tasklet function that runs  in tasklet context and
                  further defer the work.
 * Input        : param data Not used
 * Output       : no
 * Return       : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/26
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
STATIC void bfg_hostwake_task(unsigned long data)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return ;
    }

    spin_lock_bh(&pm_data->tx_lock);

    /*check device state via gpio, if sleep ,we should wake up*/
    if (LOWLEVEL == gpio_get_value(g_wire_dev->host_wakeup_bfg))
    {
        bfg_sleep_wakeup();
        if ((BT_STATE_BOOTUP == pm_data->bt_state) || (FM_STATE_BOOTUP == pm_data->fm_state))
        {
            pm_data->ps_pm_interface->reset_bt_timer(1);
        }
        spin_unlock_bh(&pm_data->tx_lock);

        /*wait for device wake up and init*/
        mdelay(10);
    }
    else
    {
        spin_unlock_bh(&pm_data->tx_lock);
    }

}


/*****************************************************************************
 * Prototype    : bfg_wake_host_isr
 * Description  : functions called when bt wake host via GPIO
 * Input        : irq, dev_id
 * Output       : no
 * Return       :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/26
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
irqreturn_t bfg_wake_host_isr(int irq, void *dev_id)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return IRQ_NONE;
    }

    if ((BT_STATE_BOOTUP == pm_data->bt_state) || (GNSS_STATE_BOOTUP == pm_data->gnss_state)
        || (FM_STATE_BOOTUP == pm_data->fm_state))
    {
        PS_PRINT_SUC("bfg wakeup host irq is trigged!\n");
    }
    else
    {
        PS_PRINT_ERR("bfg irq is burr and ignore!\n");
        return IRQ_NONE;
    }
    atomic_set(&pm_data->ps_pm_interface->bfg_wake_host_flag, 1);
    tasklet_schedule(&pm_data->hostwake_task);

    return IRQ_HANDLED;
}

STATIC int low_power_suspend(struct platform_device *pdev, pm_message_t state)
{
    return SUCC;
}

STATIC int low_power_resume(struct platform_device *pdev)
{
    return SUCC;
}

STATIC ssize_t bt_power_state_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*return the state of bt(on or off)*/
    if (BT_STATE_BOOTUP == pm_data->bt_state)
    {
        return snprintf(buf, 3, "%s\n", "1");
    }
    else
    {
        return snprintf(buf, 3, "%s\n", "0");
    }
}

STATIC struct kobj_attribute bt_power_attr = {
    .attr = {
        .name = "bt_power_state",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show   = bt_power_state_show,
};

STATIC ssize_t ini_file_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{

#ifdef HISI_GET_BOARD_ID
	return snprintf(buf, INI_NAME_LEN, "%s", g_st_boardid.ini_file_name);
#else
	*buf = '\0';
	return 1;
#endif
}

STATIC struct kobj_attribute ini_file_attr = {
    .attr = {
        .name = "ini_file_name",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show   = ini_file_show,
};


STATIC ssize_t fm_power_state_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*return the state of fm(on or off)*/
    if (FM_STATE_BOOTUP == pm_data->fm_state)
    {
        return snprintf(buf, 3, "%s\n", "1");
    }
    else
    {
        return snprintf(buf, 3, "%s\n", "0");
    }
}

STATIC struct kobj_attribute fm_power_attr = {
    .attr = {
        .name = "fm_power_state",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show   = fm_power_state_show,
};

STATIC ssize_t gnss_power_state_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*return the state of gnss(on or off)*/
    if (GNSS_STATE_BOOTUP == pm_data->gnss_state)
    {
        return snprintf(buf, 3, "%s\n", "1");
    }
    else
    {
        return snprintf(buf, 3, "%s\n", "0");
    }
}

STATIC struct kobj_attribute gnss_power_attr = {
    .attr = {
        .name = "gnss_power_state",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show   = gnss_power_state_show,
};

#ifdef FEATURE_BTCOEX_TEST
STATIC ssize_t coex_mode_store(struct kobject *kobj,
        struct kobj_attribute *attr, const char *buf, size_t count)
{
	int mode;

	mode = simple_strtol(buf, NULL, 10);

	btcoex_set_dhcp_mode(mode);

	return count;
}

STATIC struct kobj_attribute coex_mode_attr = {
    .attr = {
        .name = "coex_mode",
        .mode = S_IRUGO | S_IWUSR,
    },
    .store = coex_mode_store,
};
#endif

STATIC ssize_t bfg_sleep_state_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*return the state of bfg(sleep or active)*/
    if (HIGHLEVEL == gpio_get_value(g_wire_dev->host_wakeup_bfg))
    {
        return snprintf(buf, 3, "%s\n", "1");
    }
    else
    {
        return snprintf(buf, 3, "%s\n", "0");
    }
}

STATIC struct kobj_attribute bfg_sleep_attr = {
    .attr = {
        .name = "bfg_sleep_state",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show = bfg_sleep_state_show,
};

STATIC ssize_t wlan_sleep_state_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{
    struct pm_drv_data *pm_data = platform_get_drvdata(g_pm_device);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    /*return the state of wlan(sleep or active)*/
    if (HIGHLEVEL == gpio_get_value(g_wire_dev->host_wakeup_wlan))
    {
        return snprintf(buf, 3, "%s\n", "1");
    }
    else
    {
        return snprintf(buf, 3, "%s\n", "0");
    }
}

STATIC struct kobj_attribute wlan_sleep_attr = {
    .attr = {
        .name = "wlan_sleep_state",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show = wlan_sleep_state_show,
};

STATIC ssize_t powerpin_state_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{
    /*return the state of wlan(sleep or active)*/
    return snprintf(buf, 8, "%hu\n", powerpin_state);
}
STATIC ssize_t powerpin_state_store(struct kobject *kobj,
        struct kobj_attribute *attr, const char *buf, size_t count)
{
	int mode;
    mode = simple_strtol(buf, NULL, 10);
    powerpin_state |= mode & PINSTATE_MASK;
	return count;
}

STATIC struct kobj_attribute powerpin_attr = {
    .attr = {
        .name = "powerpin_state",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show = powerpin_state_show,
    .store = powerpin_state_store,
};

STATIC struct attribute *bfg_power_attrs[] = {
    &bt_power_attr.attr,
    &fm_power_attr.attr,
    &gnss_power_attr.attr,
#ifdef FEATURE_BTCOEX_TEST
    &coex_mode_attr.attr,
#endif
    &bfg_sleep_attr.attr,
    &wlan_sleep_attr.attr,
    &powerpin_attr.attr,
    &ini_file_attr,
    NULL
};

STATIC struct attribute_group power_state_group = {
    .attrs = bfg_power_attrs,
};

STATIC int low_power_remove(struct platform_device *pdev)
{
    int ret = 0;

    struct pm_drv_data *pm_data = platform_get_drvdata(pdev);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -FAIL;
    }

    PS_PRINT_DBG("enter\n");

    /*free gpio and irq resource*/
    gpio_free(g_wire_dev->host_wakeup_wlan);
    gpio_free(g_wire_dev->wlan_wakeup_host);
    gpio_free(g_wire_dev->bfg_wakeup_host);
    gpio_free(g_wire_dev->host_wakeup_bfg);
    gpio_free(g_wire_dev->power_on);
    gpio_free(g_wire_dev->wlan_power_on);
    gpio_free(g_wire_dev->bfg_power_on);
    free_irq(pm_data->bfg_irq, NULL);

    /*remove kobject*/

	kobject_put(g_sysfs_hi110x_power);
	g_sysfs_hi110x_power = NULL;
    sysfs_remove_group(g_sysfs_hi110x_power , &power_state_group);

    /*delete timer*/
    pm_data->ps_pm_interface->reset_bt_timer(0);

    /*destory wake lock*/
    wake_lock_destroy(&pm_data->bfg_wake_lock);

    /*free platform driver data struct*/
    kfree(pm_data);
    pm_data = NULL;

    platform_set_drvdata(pdev, NULL);

    return ret;
}

/*****************************************************************************
 * Prototype    : low_power_probe
 * Description  : paltform driver init function
 * Input        : void
 * Output       : no
 * Return       : 0 means succeed, -1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/26
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
STATIC int low_power_probe(struct platform_device *pdev)
{
    int ret             = 0;
    struct pm_drv_data  *pm_data;

	struct device_node *np;

    PS_PRINT_SUC("enter\n");
	np = of_find_compatible_node(NULL, NULL,DTS_COMP_HI1101_POWER_NAME);	// should be the same as dts node compatible property
	if (NULL == np) {
		PS_PRINT_ERR("Unable to find %s\n",DTS_COMP_HI1101_POWER_NAME);
		return -ENOENT;
	}

	PS_PRINT_INFO("have find %s\n",DTS_COMP_HI1101_POWER_NAME);

    /*save device pointer*/
    g_pm_device = pdev;

    /*malloc memory for struct*/
    pm_data = kzalloc(sizeof(struct pm_drv_data), GFP_KERNEL);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("no mem to allocate pm_data\n");
        return -ENOMEM;
    }

    memset(pm_data, 0, sizeof(struct pm_drv_data));

    /*init some members of struct pm_data*/
    pm_data->wlan_state             = WLAN_STATE_SHUTDOWN;
    pm_data->bt_state               = BT_STATE_SHUTDOWN;
    pm_data->gnss_state             = GNSS_STATE_SHUTDOWN;
    pm_data->fm_state               = FM_STATE_SHUTDOWN;

    /*set driver data*/
    platform_set_drvdata(pdev, pm_data);

    /*request and init gpio*/
    ret = gpio_request_and_init(pdev);
    if (ret)
    {
        PS_PRINT_ERR("gpio init has failed!\n");
		kfree(pm_data);
        return ret;
    }

    /*creat file sysfs to control the power of fm and gnss*/
    g_sysfs_hi110x_power = kobject_create_and_add("hi110x_power", &devices_kset->kobj);
    if (NULL == g_sysfs_hi110x_power)
    {
        PS_PRINT_ERR("Failed to creat g_sysfs_hi110x_power !!!\n ");
		return -ENOMEM;
    }

    ret = sysfs_create_group(g_sysfs_hi110x_power , &power_state_group);
    if (ret)
    {
        kfree(pm_data);
        PS_PRINT_ERR("failed to create power properties!\n");
        return ret;
    }

#ifdef HISI_GET_BOARD_ID
	memset(&g_st_boardid, 0, sizeof(BOARD_INFO_STRU));
	get_board_info(&g_st_boardid);
#endif
    /*init bfg wake lock */
    wake_lock_init(&pm_data->bfg_wake_lock, WAKE_LOCK_SUSPEND, BFG_LOCK_NAME);

    /*init mutex*/
    mutex_init(&pm_data->host_mutex);

    /*init spinlock*/
    spin_lock_init(&pm_data->tx_lock);

    /*initialize host wake tasklet*/
    tasklet_init(&pm_data->hostwake_task, bfg_hostwake_task, (uint64)NULL);

    return ret;
}

static struct of_device_id hi1101_power_match_table[] = {
	{ 
		.compatible = DTS_COMP_HI1101_POWER_NAME, 
		.data = NULL,
    },
	{ },
};

STATIC struct platform_driver low_power_driver =
{
    .probe          = low_power_probe,
    .remove         = low_power_remove,
    .suspend        = low_power_suspend,
    .resume         = low_power_resume,
    .driver         = {
        .name   = "wireless_low_power",
        .owner  = THIS_MODULE,
		.of_match_table	= hi1101_power_match_table,
    },
};

STATIC int __init low_power_init(void)
{
    PS_PRINT_DBG("k3v2 wireless low power init!\n");
#ifdef CONFIG_HWCONNECTIVITY
    if (!isMyConnectivityChip(CHIP_TYPE_HI110X )) {
        PS_PRINT_ERR("cfg lower power chip type is not match, skip driver init");
        return -EINVAL;
    } else {
        PS_PRINT_INFO("cfg lower power type is matched with hi110x, continue");
    }
#endif

    return platform_driver_register(&low_power_driver);
}

STATIC void __exit low_power_exit(void)
{
    PS_PRINT_DBG("k3v2 wireless low power exit!\n");

    platform_driver_unregister(&low_power_driver);

    patch_exit();

}

module_init(low_power_init);
module_exit(low_power_exit);
MODULE_AUTHOR("Hisilicon wireless low power Driver Group");
MODULE_DESCRIPTION("K3v2 wireless low power platform driver");
MODULE_LICENSE("GPL");


