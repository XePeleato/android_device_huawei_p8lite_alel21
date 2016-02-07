/******************************************************************************

          Copyright (C), 2001-2011, huawei technology limited company

 ******************************************************************************
  Filename   : ps_inode.c
  Version    : first draft
  Author     : zwx144390
  Create date: 2012-10-11
  Modify     :
  Func descr : Create file system inode for GNSS/FM, and register driver.
  Func list  :
              hw_fm_open
              hw_fm_release
              hw_gnss_open
              hw_gnss_release
              hw_gnss_write
              open_tty_drv
              ps_bt_register
              ps_bt_unregister
              ps_tty_complete
              ps_get_core_reference
              ps_probe
              ps_remove
              ps_resume
              ps_suspend
              ps_tx_gnssbuf
              ps_bt_write
              release_tty_drv
              show_baud_rate
              show_dev_name
              show_flow_cntrl
              show_install
              store_baud_rate
              store_dev_name
  Modi histoy:
  1.Data     : 2012-10-11
    Author   : zwx144390
    Modify
    content  : Create File

******************************************************************************/

/*****************************************************************************
  1 Include Head file
*****************************************************************************/
#include <linux/platform_device.h>
#include <linux/jiffies.h>
#include <linux/firmware.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/gpio.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sysfs.h>
#include <linux/tty.h>
#include <linux/poll.h>
#include <linux/timer.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#endif

#include "hw_bfg_ps.h"

#if 0
#include <linux/clk.h>
#include <mach/clock.h>
#include <mach/clk_name_interface.h>
#endif

/*****************************************************************************
  Define global variable
*****************************************************************************/
/**
 * This references is the per-PS platform device in the arch/arm/
 * board-xx.c file.
 */
STATIC struct platform_device *hw_ps_device;
STATIC int g_debug_cnt = 0;
struct kobject *g_sysfs_hi110x_ps = NULL;
/*****************************************************************************
  Function implement
*****************************************************************************/

/**********************************************************************/
/* internal functions */
/**
 * Prototype    : ps_get_plat_reference
 * Description  : reference the plat's dat,This references the per-PS
 *                  platform device in the arch/arm/board-xx.c file.
 * input        : use *hw_ps_device
 * output       : the have registered platform device struct
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_get_plat_reference(struct ps_plat_s **plat_data)
{
    struct platform_device  *pdev;
    struct ps_plat_s    *ps_plat_d;

    PS_PRINT_FUNCTION_NAME;
    pdev = hw_ps_device;
    if (!pdev)
    {
        *plat_data = NULL;
        PS_PRINT_ERR("pdev is NULL");
        return 0;
    }
    /* find ps_plat_d base hw_ps_device */
    ps_plat_d = dev_get_drvdata(&pdev->dev);
    *plat_data = ps_plat_d;
    return 0;
}

/**
 * Prototype    : ps_get_core_reference
 * Description  : reference the core's data,This references the per-PS
 *                  platform device in the arch/xx/board-xx.c file..
 * input        : use *hw_ps_device
 * output       : the have registered ps_core_d struct
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_get_core_reference(struct ps_core_s **core_data)
{
    struct platform_device *pdev;
    struct ps_plat_s *ps_plat_d;

    /* get ps_plat_d reference from platform device */

    PS_PRINT_FUNCTION_NAME;
    pdev = hw_ps_device;
    if (!pdev)
    {
        *core_data = NULL;
        PS_PRINT_ERR("pdev is NULL");
        return 0;
    }
    ps_plat_d = dev_get_drvdata(&pdev->dev);
    *core_data = ps_plat_d->core_data;
    return 0;
}

/**
 * Prototype    : ps_tty_complete
 * Description  : to signal completion of line discipline installation
 *                  called from PS Core, upon tty_open.
 * input        : ps_plat_d
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_tty_complete(void *pm_data)
{
    struct ps_plat_s *ps_plat_d = (struct ps_plat_s *)pm_data;

    PS_PRINT_FUNCTION_NAME;
    complete(&ps_plat_d->ldisc_installed);
    return 0;
}

/**
 * Prototype    : ps_change_baud_rate_complete
 * Description  : to signal completion of change baud rate function
 *                  called from PS Core, upon tty_open.
 * input        : ps_plat_d
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_change_baud_rate_complete(void *pm_data)
{
    struct ps_plat_s *ps_plat_d = (struct ps_plat_s *)pm_data;

    PS_PRINT_FUNCTION_NAME;
    complete(&ps_plat_d->change_baud_rate);
    return 0;
}

/**
 * Prototype    : ps_chk_bfg_active
 * Description  : to chk wether or not bfg active
 *
 * input        : ps_plat_d
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_chk_bfg_active(struct ps_core_s *ps_core_d)
{

    PS_PRINT_FUNCTION_NAME;
    if ((true == atomic_read(&ps_core_d->bt_func_has_open)) ||
        (true == atomic_read(&ps_core_d->gnss_func_has_open))||
        (true == atomic_read(&ps_core_d->fm_func_has_open)))
    {
        return true;
    }
    return false;
}

/**
 * Prototype    : ps_alloc_skb
 * Description  : allocate mem for new skb
 *
 * input        : ps_plat_d
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
struct sk_buff *ps_alloc_skb(uint16 len)
{
    struct sk_buff *skb;

    PS_PRINT_FUNCTION_NAME;
    skb = NULL;
    skb = alloc_skb(len, GFP_ATOMIC);
    if (NULL == skb) {
        PS_PRINT_WARNING("can't allocate mem for new skb");
        return NULL;
    }
    skb_put(skb, len);
    return skb;
}

/**
 * Prototype    : open_tty_drv
 * Description  : called from PS Core when BT protocol stack drivers
 *                  registration,or FM/GNSS hal stack open FM/GNSS inode
 * input        : ps_plat_d
 * output       : return 0--> open tty uart is ok
 *                return !0-> open tty uart is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 open_tty_drv(void *pm_data)
{
    struct ps_plat_s *ps_plat_d = NULL;
    struct ps_core_s *ps_core_d;
#if 0
    struct clk *uart_clock;
    struct clk *uart_pclock;
    int32 ret = 0;
#endif
    uint8 retry = OPEN_TTY_RETRY_COUNT;
    uint64 timeleft = 0;

    PS_PRINT_INFO("%s", __func__);
    if (unlikely(NULL == pm_data))
    {
        PS_PRINT_ERR("pm_data is NULL");
        return -EINVAL;
    }
    ps_plat_d = (struct ps_plat_s *)pm_data;
    ps_core_d = ps_plat_d->core_data;
    if (true == ps_core_d->tty_have_open)
    {
        PS_PRINT_SUC("line discipline have installed");
        return 0;
    }
#if 0
    uart_clock = k3v2_get_clock_by_name(CLK_UART4);
    if (!uart_clock)
    {
        PS_PRINT_ERR("Unable to initialize clock");
        return -EINVAL;
    }

    uart_pclock = k3v2_get_clock_by_name(CLKIN_SYS);
    if (!uart_pclock)
    {
        PS_PRINT_ERR("Unable to find parent clock");
        return -EINVAL;
    }

    ret = clk_set_parent(uart_clock, uart_pclock);
    if(ret)
    {
        PS_PRINT_ERR("Unable to set parent of clock");
        return -EINVAL;
    }
#endif

    do {
        /* re-initialize the completion */
        INIT_COMPLETION(ps_plat_d->ldisc_installed);
        /* send notification to octty */
        ps_plat_d->ldisc_install = 1;
        PS_PRINT_INFO("ldisc_install = 1\n");
        /* notify a info to octty */
        sysfs_notify(g_sysfs_hi110x_ps, NULL, "install");
        /* wait for ldisc to be installed */
        timeleft = wait_for_completion_timeout(&ps_plat_d->ldisc_installed,
                msecs_to_jiffies(HISI_LDISC_TIME));
        if (!timeleft)
        {
            /* ldisc installation timeout */
            PS_PRINT_ERR("ldisc installation timeout");
            continue;
        }
        else
        {
            /* ldisc installed now */
            PS_PRINT_SUC("line discipline installed");
            ps_core_d->tty_have_open = true;
            return 0;
        }
    } while (retry--);
    return -EPERM;
}

/**
 * Prototype    : release_tty_drv
 * Description  : called from PS Core when BT protocol stack drivers
 *                  unregistration,or FM/GNSS hal stack close FM/GNSS inode
 * input        : ps_plat_d
 * output       : return 0--> open tty uart is ok
 *                return !0-> open tty uart is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 release_tty_drv(void *pm_data)
{
    struct ps_plat_s *ps_plat_d = NULL;
    struct tty_struct *tty = NULL;
    struct ps_core_s *ps_core_d;
    uint64 timeleft = 0;
    uint8 delay_times = RELEASE_DELAT_TIMES;

    PS_PRINT_INFO("%s", __func__);
    if (unlikely(NULL == pm_data))
    {
        PS_PRINT_ERR("pm_data is NULL");
        return -EINVAL;
    }

    ps_plat_d = (struct ps_plat_s *)pm_data;
    ps_core_d = ps_plat_d->core_data;
    tty = ps_core_d->tty;

    if (false == ps_core_d->tty_have_open)
    {
        PS_PRINT_INFO("line discipline have uninstalled");
        return 0;
    }
    /* clean all tx sk_buff */
    while(((ps_core_d->tx_high_seq.qlen)||(ps_core_d->tx_low_seq.qlen))&&(delay_times))
    {
        msleep(100);
        delay_times --;
    }
    msleep(200);

    INIT_COMPLETION(ps_plat_d->ldisc_installed);

    if (tty)
    {   /* can be called before ldisc is installed */
        /* Flush any pending characters in the driver and discipline. */
        PS_PRINT_INFO(" %s--> into flush_buffer\n", __func__);
        tty_ldisc_flush(tty);
        tty_driver_flush_buffer(tty);
        tty->ops->flush_buffer(tty);
    }

    /* send uninstall notification to octty */
    PS_PRINT_INFO("ldisc_install = 0");
    ps_plat_d->ldisc_install = 0;
    /* notify info to octty,and close uart */
    sysfs_notify(g_sysfs_hi110x_ps, NULL, "install");

    /* wait for ldisc to be un-installed */
    timeleft = wait_for_completion_timeout(&ps_plat_d->ldisc_installed,
            msecs_to_jiffies(HISI_LDISC_TIME));
    if (!timeleft)
    {   /* timeout */
        PS_PRINT_ERR(" timed out waiting for ldisc to be un-installed");
        return -ETIMEDOUT;
    }

    ps_core_d->tty_have_open = false;
    ps_plat_d->flow_cntrl = 1;
    /* baud rate change to default */
    ps_plat_d->baud_rate = DEFAULT_BAUD_RATE;

    return 0;
}

/**
 * Prototype    : open_tty_download_patch
 * Description  : called from PS Core when BT protocol stack drivers
 *                  registration,or FM/GNSS hal stack open FM/GNSS inode
 * input        : ps_plat_d
 * output       : return 0--> open tty uart is ok
 *                return !0-> open tty uart is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 open_tty_download_patch(struct ps_core_s *ps_core_d, uint8 proto)
{
    uint64 timeleft;
    int32 err = 0;

    PS_PRINT_INFO("%s", __func__);
    if (unlikely((NULL == ps_core_d)||(NULL == ps_core_d->ps_pm)||
        (NULL == ps_core_d->ps_pm->download_patch)))
    {
        PS_PRINT_ERR("ps_core_d or ps_core_d->ps_pm or ps_core_d->ps_pm->download_patch is NULL");
        return -EINVAL;
    }
    /* check yes or no have download patch */
    if (false == ps_core_d->tty_have_open)
    {   /* open uart */
        err = open_tty_drv(ps_core_d->pm_data);
        if (err != 0)
        {
            PS_PRINT_ERR(" open tty is err!");
            /* if current user is bt, when failed, need to tell bt the result */
            #ifdef FEATURE_BT_HCI_LINE_LDISC
            if (BT_DOWNLOAD_PATCH == proto)
            {
                ps_uart_ioctl(ps_core_d, UART_IOCTL_CMD, CLOSE_UART_COMENT);
            }
            #endif
            return -EPERM;
        }
        ps_core_d->down_load_patch_proto = proto;
        /* into download patch state */
        ps_core_d->in_download_patch_state = true;
        tty_recv = ps_recv_patch;
        err = ps_core_d->ps_pm->download_patch();
        if (err)
        {   /* if download patch err,and close uart */
            ps_core_d->in_download_patch_state = false;
            err = release_tty_drv(ps_core_d->pm_data);
            PS_PRINT_ERR(" download_patch is failed!\n");
            return -EPERM;
        }
        PS_PRINT_SUC(" download_patch is successfully!\n");
    }
    else
    {
        /* initialize the completion */
        /* if not needs download patch,and direct tx init proto cmd */
        INIT_COMPLETION(ps_core_d->dev_3in1_opened);
        if (BT_DOWNLOAD_PATCH == proto)
        {
            PS_PRINT_INFO("no need open uart: Send open BT CMD");
            ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_BT);
        }
        else if (GNSS_DOWNLOAD_PATCH == proto)
        {
            PS_PRINT_INFO("no need open uart: Send open GNSS CMD");
            ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_GNSS);
        }
        else if (FM_DOWNLOAD_PATCH == proto)
        {
            PS_PRINT_INFO("no need open uart: Send open FM CMD");
            ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_FM);
        }
    }

    timeleft = wait_for_completion_timeout(&ps_core_d->dev_3in1_opened,
            msecs_to_jiffies(DEV_3IN1_OPEN_TIME));
    if (!timeleft)
    {
        PS_PRINT_ERR("wait bfg device 3_in_1 init ack timeout\n");
        /* if timeout, if not proto opened and close uart */
        if (false == ps_chk_bfg_active(ps_core_d))
        {
            PS_PRINT_ERR("need close uart!\n");
            /* close uart */
            err = release_tty_drv(ps_core_d->pm_data);
        }
        return -ETIMEDOUT;
    }
    else
    {
        /* ldisc installed now */
        PS_PRINT_SUC("wait bfg device 3_in_1 init ack OK\n");
        mod_timer(&ps_core_d->beat_timer, jiffies + BEAT_TIME_ACTIVE * HZ);
    }
    return 0;
}

/********************************************************************/
/**
 * Prototype    : ps_patch_write
 * Description  : functions called from pm drivers,used download patch data
 * input        : type, content
 * output       : no
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/12
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_patch_write(uint8 *data, int32 count)
{
    struct ps_core_s *ps_core_d;
    int32 len;

    PS_PRINT_FUNCTION_NAME;

    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d) || (NULL == ps_core_d->tty)))
    {
        PS_PRINT_ERR(" tty unavailable to perform write");
        return -EINVAL;
    }
    /* write to uart */
    len = ps_write_tty(ps_core_d, data, count);
    return len;
}

/**
 * Prototype    : ps_recv_patch
 * Description  : PS's pm receive function.this function is called when download patch.
 * input        : data -> recive data ptr   from UART TTY
 *                count -> recive data count from UART TTY
 * output       : not
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_recv_patch(void *disc_data, const uint8 *data, int32 count)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_FUNCTION_NAME;

    ps_core_d = (struct ps_core_s *)disc_data;
    if (unlikely((NULL == data) || (NULL == disc_data) || (NULL == ps_core_d->ps_pm)||
        (NULL == ps_core_d->ps_pm->recv_patch)))
    {
        PS_PRINT_ERR(" received null from TTY ");
        return -EINVAL;
    }

#ifdef DEBUG_USE
    vfs_write(ps_core_d->rx_data_fp_patch, data, count, &ps_core_d->rx_data_fp_patch->f_pos);
#endif

    /* tx data to hw-pm */
    if (ps_core_d->ps_pm->recv_patch(data, count) < 0)
    {
        PS_PRINT_ERR(" %s-err", __func__);
        return -EPERM;
    }
    return 0;
}

/**
 * Prototype    : ps_kfree_skb
 * Description  : when close a function, kfree skb
 * input        : ps_core_d, skb type
 * output       : no
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/26
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
void ps_kfree_skb(struct ps_core_s *ps_core_d, uint8 type)
{
    struct sk_buff *skb = NULL;

    PS_PRINT_FUNCTION_NAME;
    if (NULL == ps_core_d)
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return;
    }

    while ((skb = ps_skb_dequeue(ps_core_d, type)))
    {
        kfree_skb(skb);
    }

    switch (type)
    {
    case TX_HIGH_QUEUE:
        skb_queue_purge(&ps_core_d->tx_high_seq);
        break;
    case TX_LOW_QUEUE:
        skb_queue_purge(&ps_core_d->tx_low_seq);
        break;
    case RX_GNSS_QUEUE:
        skb_queue_purge(&ps_core_d->rx_gnss_seq);
        break;
    case RX_FM_QUEUE:
        skb_queue_purge(&ps_core_d->rx_fm_seq);
        break;
    case RX_BT_QUEUE:
        skb_queue_purge(&ps_core_d->rx_bt_seq);
        break;
    case RX_DBG_QUEUE:
        skb_queue_purge(&ps_core_d->rx_dbg_seq);
        break;
    default:break;
    }
    return;
}

/**
 * Prototype    : ps_restore_skbqueue
 * Description  : when err and restore skb to seq function.
 * input        : ps_core_d, skb
 * output       : not
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_restore_skbqueue(struct ps_core_s *ps_core_d, struct sk_buff *skb, uint8 type)
{
    PS_PRINT_FUNCTION_NAME;

    if (unlikely((NULL == skb) || (NULL == ps_core_d)))
    {
        PS_PRINT_ERR(" skb or ps_core_d is NULL");
        return -EINVAL;
    }

    switch (type)
    {
    case RX_GNSS_QUEUE:
        skb_queue_head(&ps_core_d->rx_gnss_seq, skb);
        break;
    case RX_FM_QUEUE:
        skb_queue_head(&ps_core_d->rx_fm_seq, skb);
        break;
    case RX_BT_QUEUE:
        skb_queue_head(&ps_core_d->rx_bt_seq, skb);
        break;
    case RX_DBG_QUEUE:
        skb_queue_head(&ps_core_d->rx_dbg_seq, skb);
        break;
    default:
        break;
    }
    return 0;
}

#ifdef FEATURE_BT_HCI_LINE_LDISC
/********************************************************************/
/**
 * Prototype    : ps_bt_register
 * Description  : functions called from bt protocol stack drivers
 *                  when need register to ps driver; it is to be EXPORT-ed
 * input        : new_proto
 * output       : return 0--> register is ok
 *                return !0-> register is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_bt_register(struct ps_proto_s *new_proto)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if ((NULL == ps_core_d) || (NULL == new_proto) ||
        (NULL == new_proto->recv) || (NULL == new_proto->uart_ioctl))
    {
        PS_PRINT_ERR("gdata/new_proto/recv or uart_ioctl not ready");
        return -EINVAL;
    }

    if (true == ps_core_d->bt_registered)
    {
        PS_PRINT_ERR("bt have been registered!");
        return -EALREADY;
    }

    ps_core_d->bt_proto = new_proto;
    ps_core_d->bt_registered = true;
    ps_core_d->bt_max_frame_size = new_proto->max_frame_size;
    new_proto->write = ps_bt_write;
    PS_PRINT_SUC("bt register successful!");
    return 0;
}
EXPORT_SYMBOL_GPL(ps_bt_register);

/**
 * Prototype    : ps_bt_unregister
 * Description  : functions called from bt protocol stack drivers
 *                  when need unregister to ps driver; it is to be EXPORT-ed
 * input        : proto
 * output       : return 0--> register is ok
 *                return !0-> register is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_bt_unregister(struct ps_proto_s *proto)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if ((NULL == ps_core_d))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    if (false == ps_core_d->bt_registered)
    {
        PS_PRINT_WARNING("bt have already unregister!");
        return 0;
    }

    ps_core_d->bt_proto = NULL;
    ps_core_d->bt_registered = false;
    PS_PRINT_SUC(" bt proto unregistered over!");
    return 0;
}
/* for protocols making use of shared transport */
EXPORT_SYMBOL_GPL(ps_bt_unregister);

/**
 * Prototype    : ps_bt_open_uart
 * Description  : functions called from bt protocol stack drivers
 *                  when user open bluetooth
 * input        : void
 * output       : return 0--> open is ok
 *                return !0-> open is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_bt_open_uart(void)
{
    struct ps_core_s *ps_core_d;
    int32 ret = 0;

    PS_PRINT_INFO("%s", __func__);

    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    if (unlikely((NULL == ps_core_d)||(NULL == ps_core_d->bt_proto)||
        (NULL == ps_core_d->bt_proto->uart_ioctl)||
        (NULL == ps_core_d->ps_pm)||(NULL == ps_core_d->ps_pm->set_pm_state)||
        (NULL == ps_core_d->ps_pm->bfg_power_set)||
        (NULL == ps_core_d->ps_pm->bfg_mutex_lock)))
#else
    if (unlikely((NULL == ps_core_d)||(NULL == ps_core_d->bt_proto)||
        (NULL == ps_core_d->bt_proto->uart_ioctl)))
#endif
    {
        PS_PRINT_ERR("bt_proto/recv or uart_ioctl not ready");
        return -EINVAL;
    }

    if (false == ps_core_d->bt_registered)
    {
        PS_PRINT_ERR("bt has not registered!");
        return -EALREADY;
    }

    ps_core_d->ps_pm->bfg_mutex_lock(1);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /* first power on bfg gpio pin */
    ret = ps_core_d->ps_pm->bfg_power_set(BT_DOWNLOAD_PATCH, BFG_POWER_GPIO_UP);
    if (ret)
    {
        PS_PRINT_ERR("set bt power on err!ret = %d", ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        return -EPERM;
    }

    /* open uart driver and download patch */
    ret = open_tty_download_patch(ps_core_d, BT_DOWNLOAD_PATCH);
    if (ret < 0)
    {
        PS_PRINT_ERR("open tty or download patch or init bt err! ret = %d", ret);
        ret = ps_core_d->ps_pm->bfg_power_set(BT_DOWNLOAD_PATCH, BFG_POWER_GPIO_DOWN);
        if (ret)
        {
            PS_PRINT_ERR("set bt power off err!ret = %d", ret);
            ps_core_d->ps_pm->bfg_mutex_lock(0);
            return -EPERM;
        }
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        return -EINVAL;
    }
    ps_core_d->ps_pm->set_pm_state(BT_DOWNLOAD_PATCH);
    /*add by cwx145522 for pm begin*/
    ps_core_d->bt_state = POWER_ON_STATE;
    /*add by cwx145522 for pm end*/
#else
    ret = open_tty_drv(ps_core_d->pm_data);
    if (ret != 0)
    {
        PS_PRINT_ERR("open tty is err! ret = %d",ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        return -EINVAL;
    }
    /* tx sys bt open info */
    ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_BT);
#endif
    atomic_set(&ps_core_d->bt_func_has_open, 1);

    /*add by cwx145522 for pm begin*/
    mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
    /*add by cwx145522 for pm end*/

    ps_core_d->ps_pm->bfg_mutex_lock(0);

    PS_PRINT_SUC(" open bt has ok!");
    ps_uart_ioctl(ps_core_d, UART_IOCTL_CMD, OPEN_UART_COMENT);
    return ret;
}

/**
 * Prototype    : ps_bt_close_uart
 * Description  : functions called from bt protocol stack drivers
 *                  when user open bluetooth
 * input        : void
 * output       : return 0--> close is ok
 *                return !0-> close is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_bt_close_uart(void)
{
    struct ps_core_s *ps_core_d;
    int32 ret = 0;

    PS_PRINT_INFO("%s", __func__);

    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely(NULL == ps_core_d))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    if (false == ps_core_d->bt_registered)
    {
        PS_PRINT_WARNING("bt has unregister!");
        return -EALREADY;
    }

    ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_CLOSE_BT);

    ps_core_d->ps_pm->bfg_mutex_lock(1);
    atomic_set(&ps_core_d->bt_func_has_open, 0);
    if (false == ps_chk_bfg_active(ps_core_d))
    {
        ret = release_tty_drv(ps_core_d->pm_data);
        if (ret != 0)
        {
            PS_PRINT_ERR("bt close tty is err! ret = %d",ret);
            ps_core_d->ps_pm->bfg_mutex_lock(0);
            return -EINVAL;
        }

        /*add by cwx145522 begin*/
        del_timer_sync(&ps_core_d->beat_timer);
        /*add by cwx145522 end*/
    }
    ps_core_d->bt_state = POWER_OFF_STATE;

    /* lastest power off bfg gpio pin */
    ret = ps_core_d->ps_pm->bfg_power_set(BT_DOWNLOAD_PATCH, BFG_POWER_GPIO_DOWN);
    if (ret)
    {
        PS_PRINT_ERR("set bt power off err!ret = %d", ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        return -EPERM;
    }
    ps_core_d->ps_pm->bfg_mutex_lock(0);
    ps_uart_ioctl(ps_core_d, UART_IOCTL_CMD, CLOSE_UART_COMENT);
    /* kfree have rx skb */
    ps_kfree_skb(ps_core_d, RX_BT_QUEUE);
    PS_PRINT_SUC(" bt has closed!");
    return 0;
}

/**
 * Prototype    : ps_bt_write
 * Description  : called in bt protocol stack drivers
 *                  via the write function pointer
 * input        : skb_buff
 * output       : len(have write to uart byte count)
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_bt_write(struct sk_buff *skb)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skbuff;
    int32 len = 0;

    PS_PRINT_FUNCTION_NAME;

    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /*add by cwx145522 for pm begin*/
    if (unlikely((NULL == ps_core_d)||(NULL == ps_core_d->tty)||
        (NULL == skb)||(NULL == ps_core_d->ps_pm)))
    /*add by cwx145522 for pm end*/
#else
    if (unlikely((NULL == skb) || (NULL == ps_core_d)
            || (NULL == ps_core_d->tty)))
#endif
    {
        PS_PRINT_ERR("bt skb or tty is NULL");
        return -EINVAL;
    }

#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /* call PM module check curr PM state, if deep sleep and resume it*/
    if (check_device_state(ps_core_d, BT_WRITE_FLAG))
    {
        atomic_set(&ps_core_d->bt_fm_data_flag, 1);
    }
#endif

    if (skb->len > ps_core_d->bt_max_frame_size)
    {
        PS_PRINT_ERR("bt skb len is too large!");
        return -EINVAL;
    }

    if (ps_core_d->tx_high_seq.qlen > TX_HIGH_QUE_MAX_NUM)
    {
        return 0;
    }

    skbuff = NULL;
    skbuff = skb_copy_expand(skb, sizeof(struct ps_packet_head),
                    sizeof(struct ps_packet_end), GFP_ATOMIC);
    if (NULL == skbuff)
    {
        PS_PRINT_ERR("can't allocate mem for new bt tx skb");
        return -EINVAL;
    }
    ps_tx_bt_add_head(ps_core_d, skbuff);
    ps_skb_enqueue(ps_core_d, skbuff, TX_HIGH_QUEUE);
    queue_work(ps_core_d->ps_tx_workqueue, &ps_core_d->tx_skb_work);
    /* return number of bytes curr skb */
    len = skb->len;
    return len;
}
#else

/**********************************************************************/
/**
 * Prototype    : hw_bt_open
 * Description  : functions called from above bt hal,when open bt file
 *                  input : "/dev/hwbt"
 * input        : return 0 --> open is ok
 * output       : return !0--> open is false
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/26
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_bt_open(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;
    int32 ret = 0;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    if (unlikely((NULL == ps_core_d)||(NULL == inode)||(NULL == filp)||
        (NULL == ps_core_d->ps_pm)||(NULL == ps_core_d->ps_pm->set_pm_state)||
        (NULL == ps_core_d->ps_pm->bfg_power_set)||
        (NULL == ps_core_d->ps_pm->bfg_mutex_lock)))
#else
    if (unlikely((NULL == ps_core_d)||(NULL == inode)||
        (NULL == filp)))
#endif
    {
        PS_PRINT_ERR("ps_core_d is err");
        return -EINVAL;
    }

    ps_core_d->ps_pm->bfg_mutex_lock(1);
    atomic_set(&ps_core_d->bt_state_booting, 1);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /* first power on bfg gpio pin */
    ret = ps_core_d->ps_pm->bfg_power_set(BT_DOWNLOAD_PATCH, BFG_POWER_GPIO_UP);
    if (ret)
    {
        PS_PRINT_ERR("set bt power on err!ret = %d", ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->bt_state_booting, 0);
        return -EPERM;
    }

    /* open uart driver and download patch */
    ret = open_tty_download_patch(ps_core_d, BT_DOWNLOAD_PATCH);
    if (ret < 0)
    {
        PS_PRINT_ERR("open tty or download patch or init bt err! ret = %d", ret);
        ret = ps_core_d->ps_pm->bfg_power_set(BT_DOWNLOAD_PATCH, BFG_POWER_GPIO_DOWN);
        if (ret)
        {
            PS_PRINT_ERR("set bt power off err!ret = %d", ret);
            ps_core_d->ps_pm->bfg_mutex_lock(0);
            atomic_set(&ps_core_d->bt_state_booting, 0);
            return -EPERM;
        }
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->bt_state_booting, 0);
        return -EINVAL;
    }
    ps_core_d->ps_pm->set_pm_state(BT_DOWNLOAD_PATCH);
    /*add by cwx145522 for pm begin*/
    ps_core_d->bt_state = POWER_ON_STATE;
    /*add by cwx145522 for pm end*/
#else
    ret = open_tty_drv(ps_core_d->pm_data);
    if (ret != 0)
    {
        PS_PRINT_ERR("open tty is err! ret = %d",ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->bt_state_booting, 0);
        return -EINVAL;
    }
    /* tx sys bt open info */
    ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_BT);
#endif
    /* info bt proto have opened */
    atomic_set(&ps_core_d->bt_func_has_open, 1);

    /*add by cwx145522 for pm begin*/
    mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
    /*add by cwx145522 for pm end*/
	
    atomic_set(&ps_core_d->bt_state_booting, 0);
    ps_core_d->ps_pm->bfg_mutex_lock(0);
    return 0;
}

/**
 * Prototype    : hw_bt_read
 * Description  : functions called from above bt hal,read count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual read byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/26
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_bt_read(struct file *filp, int8 __user *buf,
                                size_t count,loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb = NULL;
    uint16 count1;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    /* pull skb data from skb queue */
    if (NULL == (skb = ps_skb_dequeue(ps_core_d, RX_BT_QUEUE)))
    {   /* no skb data */
        PS_PRINT_WARNING("bt read skb queue is null!");
        return 0;
    }

    /* read min value from skb->len or count */
    count1 = min_t(size_t, skb->len, count);
    if (copy_to_user(buf, skb->data, count1))
    {
        PS_PRINT_ERR("copy_to_user is err!");
        ps_restore_skbqueue(ps_core_d, skb, RX_BT_QUEUE);
        return -EFAULT;
    }
    /* have read count1 byte */
    skb_pull(skb, count1);
    /* if skb->len = 0: read is over */
    if (0 == skb->len)
    {   /* curr skb data have read to user */
        kfree_skb(skb);
    }
    else
    {   /* if don,t read over; restore to skb queue */
        ps_restore_skbqueue(ps_core_d, skb, RX_BT_QUEUE);
    }
    if (ps_core_d->rx_bt_seq.qlen < (RX_BT_QUE_MAX_NUM/2) &&
        (atomic_read(&ps_core_d->throttle_flag)))
    {   /* open RTS info, allow uart rx data */
        tty_unthrottle(ps_core_d->tty);
        atomic_set(&ps_core_d->throttle_flag, 0);
    }
    return count1;
}

/**
 * Prototype    : hw_bt_write
 * Description  : functions called from above bt hal,write count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual write byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/26
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_bt_write(struct file *filp, const int8 __user *buf,
                                size_t count,loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb;
    uint16 total_len;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /*add by cwx145522 for pm begin*/
    if (unlikely((NULL == ps_core_d)||(NULL == buf)||(NULL == filp)||
        (NULL == ps_core_d->tty)||(NULL == ps_core_d->ps_pm)))
    /*add by cwx145522 for pm end*/
#else
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
#endif
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    /* if count is too large;and don,t tx */
    if (count > BT_MAX_FRAME)
    {
        PS_PRINT_ERR("bt skb len is too large!");
        return -EINVAL;
    }

#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /* call PM module check curr PM state, if deep sleep and resume it*/
    if (check_device_state(ps_core_d, BT_WRITE_FLAG))
    {
        atomic_set(&ps_core_d->bt_fm_data_flag, 1);
    }
#endif

    /* if high queue num > MAX_NUM and don't write */
    if (ps_core_d->tx_high_seq.qlen > TX_HIGH_QUE_MAX_NUM)
    {
        return 0;
    }
    /* curr skb packet total lenth */
    total_len = count + sizeof(struct ps_packet_head) + sizeof(struct ps_packet_end);

    skb  = ps_alloc_skb(total_len);
    if (NULL == skb)
    {
        PS_PRINT_ERR("ps alloc skb mem fail");
        return -EFAULT;
    }

    /* copy data from user to kernel */
    if (copy_from_user(&skb->data[sizeof(struct ps_packet_head)],
                        buf, count))
    {
        PS_PRINT_ERR("copy_from_user from bt is err");
        kfree_skb(skb);
        return -EFAULT;
    }
    /* add packet head to bt packet */
    ps_add_packet_head(skb->data, PACKET_TX_FUNC_BT, total_len);
    /* push curr skb to high queue */
    ps_skb_enqueue(ps_core_d, skb, TX_HIGH_QUEUE);
    queue_work(ps_core_d->ps_tx_workqueue, &ps_core_d->tx_skb_work);
    return count;
}

/**
 * Prototype    : hw_bt_poll
 * Description  : called by bt func from hal;
 *                  check whether or not allow read and write
 * input        : file handle
 * output       : no
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/26
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC uint32 hw_bt_poll(struct file *filp, poll_table *wait)
{
    struct ps_core_s *ps_core_d;
    uint32 mask = 0;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    /* push curr wait event to wait queue */
    poll_wait(filp, &ps_core_d->rx_bt_wait, wait);

    if (ps_core_d->rx_bt_seq.qlen)
    {   /* have data to read */
        mask |= POLLIN | POLLRDNORM;
    }

    return mask;
}

/**
 * Prototype    : hw_bt_ioctl
 * Description  : called by bt func from hal; default not use
 * input        : file handle, cmd, arg
 * output       : no
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/26
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int64 hw_bt_ioctl(struct file *file, uint32 cmd, uint64 arg)
{
    PS_PRINT_FUNCTION_NAME;
    return 0;
}

/**
 * Prototype    : hw_bt_release
 * Description  : called by bt func from hal when close bt inode
 * input        : have opened file handle
 * output       : return 0 --> close is ok
 *                return !0--> close is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/26
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_bt_release(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;
    int32 ret = 0;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == inode)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    /* wake up bt wait queue */
    wake_up_interruptible(&ps_core_d->rx_bt_wait);
    if (check_device_state(ps_core_d, BT_WRITE_FLAG))
    {
        atomic_set(&ps_core_d->bt_fm_data_flag, 1);
    }
	/* tx close bt cmd to device */
    ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_CLOSE_BT);

    ps_core_d->ps_pm->bfg_mutex_lock(1);
    atomic_set(&ps_core_d->bt_func_has_open, 0);
    if (false == ps_chk_bfg_active(ps_core_d))
    {   /* close uart driver */
        ret = release_tty_drv(ps_core_d->pm_data);
        if (ret != 0)
        {
            PS_PRINT_ERR("close tty is err! ret = %d",ret);
            ps_core_d->ps_pm->bfg_mutex_lock(0);
            return -EINVAL;
        }

        /*add by cwx145522 begin*/
        del_timer_sync(&ps_core_d->beat_timer);
        /*add by cwx145522 end*/
		
    }
    else
    {
        if ((POWER_OFF_STATE == ps_core_d->fm_state) &&
            (1 == atomic_read(&ps_core_d->gnss_sleep_flag)) &&
            check_bfg_not_booting(ps_core_d))
        {
            PS_PRINT_SUC("bt power off request sleep!\n");
            host_allow_bfg_sleep(ps_core_d);
        }
    }
    ps_core_d->bt_state = POWER_OFF_STATE;

    /* lastest power off bfg gpio pin */
    ret = ps_core_d->ps_pm->bfg_power_set(BT_DOWNLOAD_PATCH, BFG_POWER_GPIO_DOWN);
    if (ret)
    {
        PS_PRINT_ERR("set bt power off err!ret = %d", ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        return -EPERM;
    }

    ps_core_d->ps_pm->bfg_mutex_lock(0);
    /* kfree have rx bt skb */
    ps_kfree_skb(ps_core_d, RX_BT_QUEUE);
    return 0;
}
#endif

/**********************************************************************/
/**
 * Prototype    : hw_fm_open
 * Description  : functions called from above fm hal,when open fm file
 *                  input : "/dev/hwfm"
 * input        : return 0 --> open is ok
 * output       : return !0--> open is false
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_fm_open(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;
    int32 ret = 0;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    if (unlikely((NULL == ps_core_d)||(NULL == inode)||(NULL == filp)||
        (NULL == ps_core_d->ps_pm)||(NULL == ps_core_d->ps_pm->set_pm_state)||
        (NULL == ps_core_d->ps_pm->bfg_power_set)||
        (NULL == ps_core_d->ps_pm->bfg_mutex_lock)))
#else
    if (unlikely((NULL == ps_core_d)||(NULL == inode)||
        (NULL == filp)))
#endif
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    ps_core_d->ps_pm->bfg_mutex_lock(1);
	
    atomic_set(&ps_core_d->fm_state_booting, 1);

#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /* first power on bfg gpio pin */
    ret = ps_core_d->ps_pm->bfg_power_set(FM_DOWNLOAD_PATCH, BFG_POWER_GPIO_UP);
    if (ret)
    {
        PS_PRINT_ERR("set fm power on err!ret = %d", ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->fm_state_booting, 0);
        return -EPERM;
    }
    /* open uart driver and download patch */
    ret = open_tty_download_patch(ps_core_d, FM_DOWNLOAD_PATCH);
    if (ret < 0)
    {
        PS_PRINT_ERR("open tty or download patch or init FM err! ret = %d", ret);
        ret = ps_core_d->ps_pm->bfg_power_set(FM_DOWNLOAD_PATCH, BFG_POWER_GPIO_DOWN);
        if (ret)
        {
            PS_PRINT_ERR("set fm power off err!ret = %d", ret);
            ps_core_d->ps_pm->bfg_mutex_lock(0);
            atomic_set(&ps_core_d->fm_state_booting, 0);
            return -EPERM;
        }
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->fm_state_booting, 0);
        return -EINVAL;
    }
    ps_core_d->ps_pm->set_pm_state(FM_DOWNLOAD_PATCH);
    /*add by cwx145522 for pm begin*/
    ps_core_d->fm_state = POWER_ON_STATE;
    /*add by cwx145522 for pm end*/
#else
    ret = open_tty_drv(ps_core_d->pm_data);
    if (ret != 0)
    {
        PS_PRINT_ERR("open tty is err!");
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->fm_state_booting, 0);
        return -EINVAL;
    }
    ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_FM);
#endif
    /* info fm proto have opened */
    atomic_set(&ps_core_d->fm_func_has_open, 1);

    /*add by cwx145522 for pm begin*/
    mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
    /*add by cwx145522 for pm end*/
	
    atomic_set(&ps_core_d->fm_state_booting, 0);

    ps_core_d->ps_pm->bfg_mutex_lock(0);

    /* set fm read timeout default value */
    ps_core_d->fm_read_delay = FM_READ_DEFAULT_TIME;
    return 0;
}

/**
 * Prototype    : hw_fm_read
 * Description  : functions called from above fm hal,read count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual read byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_fm_read(struct file *filp, int8 __user *buf,
                                size_t count,loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb = NULL;
    uint16 count1;
    int64 timeout;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    if (0 == ps_core_d->rx_fm_seq.qlen)
    {   /* if don,t data, and wait timeout function */
        if (filp->f_flags & O_NONBLOCK)
        {   /* if O_NONBLOCK read and return */
            return -EAGAIN;
        }
        /* timeout function;when have data,can interrupt */
        timeout = wait_event_interruptible_timeout(ps_core_d->rx_fm_wait,
            (ps_core_d->rx_fm_seq.qlen > 0), msecs_to_jiffies(ps_core_d->fm_read_delay));
        if (!timeout)
        {
            PS_PRINT_DBG("fm read time out!");
            return -ETIMEDOUT;
        }
    }
    /* pull skb data from skb queue */
    if (NULL == (skb = ps_skb_dequeue(ps_core_d, RX_FM_QUEUE)))
    {
        PS_PRINT_ERR("fm read err!");
        return -ETIMEDOUT;
    }
    /* read min value from skb->len or count */
    count1 = min_t(size_t, skb->len, count);
    if (copy_to_user(buf, skb->data, count1))
    {
        PS_PRINT_ERR("copy_to_user is err!");
        ps_restore_skbqueue(ps_core_d, skb, RX_FM_QUEUE);
        return -EFAULT;
    }
    /* have read count1 byte */
    skb_pull(skb, count1);
    /* if skb->len == 0: curr skb read is over */
    if (0 == skb->len)
    {   /* curr skb data have read to user */
        kfree_skb(skb);
    }
    else
    {   /* if don,t read over; restore to skb queue */
        ps_restore_skbqueue(ps_core_d, skb, RX_FM_QUEUE);
    }

    if (ps_core_d->rx_fm_seq.qlen < (RX_FM_QUE_MAX_NUM/2) &&
            (atomic_read(&ps_core_d->throttle_flag)))
    {   /* open RTS info, allow uart rx data */
        tty_unthrottle(ps_core_d->tty);
        atomic_set(&ps_core_d->throttle_flag, 0);
    }
    return count1;
}

/**
 * Prototype    : hw_fm_write
 * Description  : functions called from above fm hal,write count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual write byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_fm_write(struct file *filp, const int8 __user *buf,
                                size_t count,loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb;
    uint16 total_len;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /*add by cwx145522 for pm begin*/
    if (unlikely((NULL == ps_core_d)||(NULL == buf)||(NULL == filp)||
        (NULL == ps_core_d->tty)||(NULL == ps_core_d->ps_pm)))
    /*add by cwx145522 for pm end*/
#else
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
#endif
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    /* if count is too large;and don,t tx */
    if (count > (FM_MAX_FRAME - sizeof(struct ps_packet_head) - sizeof(struct ps_packet_end)))
    {
        PS_PRINT_ERR("err:fm packet is too large!");
        return -EINVAL;
    }

    /* if low queue num > MAX_NUM and don't write */
    if (ps_core_d->tx_low_seq.qlen > TX_LOW_QUE_MAX_NUM)
    {
        return 0;
    }

    if (check_device_state(ps_core_d, FM_WRITE_FLAG))
    {
        atomic_set(&ps_core_d->bt_fm_data_flag, 1);
    }


    total_len = count + sizeof(struct ps_packet_head) + sizeof(struct ps_packet_end);

    skb  = ps_alloc_skb(total_len);
    if (NULL == skb)
    {
        PS_PRINT_ERR("ps alloc skb mem fail");
        return -EFAULT;
    }
    /* copy data from user to kernel */
    if (copy_from_user(&skb->data[sizeof(struct ps_packet_head)],
                        buf, count))
    {
        PS_PRINT_ERR("copy_from_user from fm is err");
        kfree_skb(skb);
        return -EFAULT;
    }
    /* add packet head to fm packet */
    ps_add_packet_head(skb->data, PACKET_TX_FUNC_FM, total_len);
    /* push curr skb to low queue */
    ps_skb_enqueue(ps_core_d, skb, TX_LOW_QUEUE);
    queue_work(ps_core_d->ps_tx_workqueue, &ps_core_d->tx_skb_work);
    return count;
}

/**
 * Prototype    : hw_fm_ioctl
 * Description  : called by hw func from hal when open power gpio or close power gpio
 * input        : file handle, cmd, arg
 * output       : no
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int64 hw_fm_ioctl(struct file *file, uint32 cmd, uint64 arg)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||(NULL == file)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    if (FM_SET_READ_TIME == cmd)
    {
        if (arg < FM_MAX_READ_TIME)
        {   /* set timeout for fm read function */
            ps_core_d->fm_read_delay = arg;
        }
        else
        {
            PS_PRINT_ERR("arg is too large!");
            return -EINVAL;
        }
    }
    return 0;
}

/**
 * Prototype    : hw_fm_release
 * Description  : called by fm func from hal when close fm inode
 * input        : have opened file handle
 * output       : return 0 --> close is ok
 *                return !0--> close is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_fm_release(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;
    int32 ret = 0;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == inode)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    /* wake up fm wait queue */
    wake_up_interruptible(&ps_core_d->rx_fm_wait);
	
    if (check_device_state(ps_core_d, FM_WRITE_FLAG))
    {
        atomic_set(&ps_core_d->bt_fm_data_flag, 1);
    }
	/* tx close fm cmd to device */
    ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_CLOSE_FM);

    ps_core_d->ps_pm->bfg_mutex_lock(1);
    atomic_set(&ps_core_d->fm_func_has_open, 0);
    if (false == ps_chk_bfg_active(ps_core_d))
    {   /* close uart driver */
        ret = release_tty_drv(ps_core_d->pm_data);
        if (ret != 0)
        {
            PS_PRINT_ERR("close tty is err!");
            ps_core_d->ps_pm->bfg_mutex_lock(0);
            return -EINVAL;
        }

        /*add by cwx145522 begin*/
        del_timer_sync(&ps_core_d->beat_timer);
        /*add by cwx145522 end*/
    }
    else
    {
        if ((POWER_OFF_STATE == ps_core_d->bt_state) &&
            (1 == atomic_read(&ps_core_d->gnss_sleep_flag)) &&
            check_bfg_not_booting(ps_core_d))
        {
            PS_PRINT_SUC("fm power off request sleep!\n");
            host_allow_bfg_sleep(ps_core_d);
        }
    }
    ps_core_d->fm_state = POWER_OFF_STATE;
    /* lastest power off bfg gpio pin */
    ret = ps_core_d->ps_pm->bfg_power_set(FM_DOWNLOAD_PATCH, BFG_POWER_GPIO_DOWN);
    if (ret)
    {
        PS_PRINT_ERR("set fm power off err!ret = %d", ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        return -EPERM;
    }

    ps_core_d->ps_pm->bfg_mutex_lock(0);
    /* kfree have rx fm skb */
    ps_kfree_skb(ps_core_d, RX_FM_QUEUE);
    return 0;
}

/**
 * Prototype    : hw_gnss_open
 * Description  : functions called from above gnss hal,when open gnss file
 * input        : "/dev/hwgnss"
 * output       : return 0 --> open is ok
 *                return !0--> open is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_gnss_open(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;
    uint8 * gnss_rx_buf;
    int32 ret = 0;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    if (unlikely((NULL == ps_core_d)||(NULL == inode)||(NULL == filp)||
        (NULL == ps_core_d->ps_pm)||(NULL == ps_core_d->ps_pm->set_pm_state)||
        (NULL == ps_core_d->ps_pm->bfg_power_set)||
        (NULL == ps_core_d->ps_pm->bfg_mutex_lock)))
#else
    if (unlikely((NULL == ps_core_d)||(NULL == inode)||
        (NULL == filp)))
#endif
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    gnss_rx_buf = kzalloc(GNSS_RX_MAX_FRAME, GFP_ATOMIC);
    if (!gnss_rx_buf)
    {
        PS_PRINT_ERR("no mem to allocate to read gnss!");
        return -ENOMEM;
    }
    memset(gnss_rx_buf,0,GNSS_RX_MAX_FRAME);
    ps_core_d->rx_gnss_buf_ptr = gnss_rx_buf;
    ps_core_d->rx_gnss_buf_org_ptr = gnss_rx_buf;

    ps_core_d->ps_pm->bfg_mutex_lock(1);
	
    atomic_set(&ps_core_d->gnss_state_booting, 1);

#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /* first power on bfg gpio pin */
    ret = ps_core_d->ps_pm->bfg_power_set(GNSS_DOWNLOAD_PATCH, BFG_POWER_GPIO_UP);
    if (ret)
    {
        PS_PRINT_ERR("set gnss power on err!ret = %d", ret);
        kfree(gnss_rx_buf);
        ps_core_d->rx_gnss_buf_ptr = 0;
        ps_core_d->rx_gnss_buf_org_ptr = 0;
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->gnss_state_booting, 0);
        return -EPERM;
    }
    /* open uart driver and download patch */
    ret = open_tty_download_patch(ps_core_d, GNSS_DOWNLOAD_PATCH);
    if (ret != 0)
    {
        PS_PRINT_ERR("open tty or download patch or init GNSS err! ret = %d", ret);
        kfree(gnss_rx_buf);
        ps_core_d->rx_gnss_buf_ptr = 0;
        ps_core_d->rx_gnss_buf_org_ptr = 0;
        ret = ps_core_d->ps_pm->bfg_power_set(GNSS_DOWNLOAD_PATCH, BFG_POWER_GPIO_DOWN);
        if (ret)
        {
            PS_PRINT_ERR("set gnss power off err!ret = %d", ret);
            ps_core_d->ps_pm->bfg_mutex_lock(0);
            atomic_set(&ps_core_d->gnss_state_booting, 0);
            return -EPERM;
        }
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->gnss_state_booting, 0);
        return -EINVAL;
    }
    ps_core_d->ps_pm->set_pm_state(GNSS_DOWNLOAD_PATCH);
    /*add by cwx145522 for pm begin*/
    ps_core_d->gnss_state = POWER_ON_STATE;
    /*add by cwx145522 for pm end*/
#else
    ret = open_tty_drv(ps_core_d->pm_data);
    if (ret != 0)
    {
        PS_PRINT_ERR("open tty is err!");
        kfree(gnss_rx_buf);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        atomic_set(&ps_core_d->gnss_state_booting, 0);
        return -EINVAL;
    }
    ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_GNSS);
#endif
    /* info gnss proto have opened */
    atomic_set(&ps_core_d->gnss_func_has_open, 1);
	
    atomic_set(&ps_core_d->gnss_state_booting, 0);
    ps_core_d->ps_pm->bfg_mutex_lock(0);
    /* set gnss read timeout default value */
    ps_core_d->gnss_read_delay = GNSS_READ_DEFAULT_TIME;
    return 0;
}

/**
 * Prototype    : hw_gnss_read
 * Description  : functions called from above gnss hal,read count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual read byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_gnss_read(struct file *filp, int8 __user *buf,
                                    size_t count,loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb = NULL;
    int32 count1;
    int64 timeout;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    if (0 == ps_core_d->rx_gnss_seq.qlen)
    {   /* if no data, and wait timeout function */
        if (filp->f_flags & O_NONBLOCK)
        {   /* if O_NONBLOCK read and return */
            return -EAGAIN;
        }
        /* timeout function;when have data,can interrupt */
        timeout = wait_event_interruptible_timeout(ps_core_d->rx_gnss_wait,
            (ps_core_d->rx_gnss_seq.qlen > 0), msecs_to_jiffies(ps_core_d->gnss_read_delay));
        if (!timeout)
        {
            PS_PRINT_DBG("gnss read time out!");
            return -ETIMEDOUT;
        }
    }
    /* pull skb data from skb queue */
    if (NULL == (skb = ps_skb_dequeue(ps_core_d, RX_GNSS_QUEUE)))
    {
        PS_PRINT_ERR("gnss read time out!");
        return -ETIMEDOUT;
    }
    /* read min value from skb->len or count */
    count1 = min_t(size_t, skb->len, count);
    if (copy_to_user(buf, skb->data, count1))
    {
        PS_PRINT_ERR("copy_to_user err!");
        ps_restore_skbqueue(ps_core_d, skb, RX_GNSS_QUEUE);
        return -EFAULT;
    }
    /* have read count1 byte */
    skb_pull(skb, count1);
    /* if skb->len = 0: read is over */
    if (0 == skb->len)
    {   /* curr skb data have read to user */
        kfree_skb(skb);
    }
    else
    {   /* if don,t read over; restore to skb queue */
        ps_restore_skbqueue(ps_core_d, skb, RX_GNSS_QUEUE);
    }

    if (ps_core_d->rx_gnss_seq.qlen < (RX_GNSS_QUE_MAX_NUM/2) &&
            (atomic_read(&ps_core_d->throttle_flag)))
    {   /* open RTS info, allow uart rx data */
        tty_unthrottle(ps_core_d->tty);
        atomic_set(&ps_core_d->throttle_flag, 0);
    }
    return count1;
}

/**
 * Prototype    : hw_gnss_write
 * Description  : functions called from above gnss hal,write count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual write byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_gnss_write(struct file *filp, const int8 __user *buf,
                                    size_t count, loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    uint16 count1;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /*add by cwx145522 for pm begin*/
    if (unlikely((NULL == ps_core_d)||(NULL == buf)||(NULL == filp)||
        (NULL == ps_core_d->tty)||(NULL == ps_core_d->ps_pm)))
    /*add by cwx145522 for pm end*/
#else
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
#endif
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    /* if count is too large;and don,t tx */
    if (count > GNSS_TX_MAX_FRAME)
    {
        PS_PRINT_ERR("err:gnss packet is too large!");
        return -EINVAL;
    }

#ifdef FEATURE_PM_DOWNLOAD_PATCH
    /* call PM module check curr PM state, if deep sleep and resume it*/
	
    check_device_state(ps_core_d, GNSS_WRITE_FLAG);

#endif

    /* if low queue num > MAX_NUM and don't write */
    if (ps_core_d->tx_low_seq.qlen > TX_LOW_QUE_MAX_NUM)
    {
        return 0;
    }

    count1 = count;
    /* to divide up packet function and tx to tty work */
    if (ps_tx_gnssbuf(ps_core_d, buf, count) < 0)
    {
        PS_PRINT_ERR("hw_gnss_write is err");
        return -EFAULT;
    }
    return count1;
}

/**
 * Prototype    : hw_gnss_ioctl
 * Description  : called by gnss func from hal when open power gpio or close power gpio
 * input        : file handle, cmd, arg
 * output       : no
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int64 hw_gnss_ioctl(struct file *file, uint32 cmd, uint64 arg)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||(NULL == file)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    if (GNSS_SET_READ_TIME == cmd)
    {
        if (arg < GNSS_MAX_READ_TIME)
        {   /* set timeout for gnss read function */
            ps_core_d->gnss_read_delay = arg;
        }
        else
        {
            PS_PRINT_ERR("arg is too large!");
            return -EINVAL;
        }
    }
    return 0;
}

/**
 * Prototype    : hw_gnss_release
 * Description  : called by gnss func from hal when close gnss inode
 * input        : have opened file handle
 * output       : return 0 --> close is ok
 *                return !0--> close is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_gnss_release(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;
    int32 ret = 0;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == inode)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    /* wake up gnss wait queue */
    wake_up_interruptible(&ps_core_d->rx_gnss_wait);
	
    check_device_state(ps_core_d, GNSS_WRITE_FLAG);
    atomic_set(&ps_core_d->gnss_sleep_flag, 0);

	/* tx close gnss cmd to device */
    ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_CLOSE_GNSS);

    ps_core_d->ps_pm->bfg_mutex_lock(1);
    atomic_set(&ps_core_d->gnss_func_has_open, 0);
    if (false == ps_chk_bfg_active(ps_core_d))
    {   /* close uart driver */
        ret = release_tty_drv(ps_core_d->pm_data);
        if (ret != 0)
        {
            PS_PRINT_ERR("close tty is err!");
            ps_core_d->ps_pm->bfg_mutex_lock(0);
            return -EINVAL;
        }

        /*add by cwx145522 begin*/
        del_timer_sync(&ps_core_d->beat_timer);
        /*add by cwx145522 end*/
    }
    else
    {
        if (!timer_pending(&ps_core_d->bfg_timer) && check_bfg_not_booting(ps_core_d))
        {
            PS_PRINT_SUC("gnss power off request sleep!\n");
            host_allow_bfg_sleep(ps_core_d);
        }
    }
    ps_core_d->gnss_state = POWER_OFF_STATE;

    /* lastest power off bfg gpio pin */
    ret = ps_core_d->ps_pm->bfg_power_set(GNSS_DOWNLOAD_PATCH, BFG_POWER_GPIO_DOWN);
    if (ret)
    {
        PS_PRINT_ERR("set gnss power off err!ret = %d", ret);
        ps_core_d->ps_pm->bfg_mutex_lock(0);
        return -EPERM;
    }

    ps_core_d->ps_pm->bfg_mutex_lock(0);

    if (NULL != ps_core_d->rx_gnss_buf_org_ptr)
    {
        kfree(ps_core_d->rx_gnss_buf_org_ptr);
    }
    ps_core_d->rx_gnss_buf_org_ptr = 0;
    ps_core_d->rx_gnss_prev_seq = RX_GNSS_SEQ_NULL;
    ps_core_d->rx_gnss_buf_all_len = 0;

    /* kfree have rx gnss skb */
    ps_kfree_skb(ps_core_d, RX_GNSS_QUEUE);
    return 0;
}

/**********************************************************************/
/**
 * Prototype    : hw_debug_open
 * Description  : functions called from above oam hal,when open debug file
 *                  input : "/dev/hwbfgdbg"
 * input        : return 0 --> open is ok
 * output       : return !0--> open is false
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_debug_open(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);

    if (unlikely((NULL == ps_core_d)||(NULL == inode)||
        (NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    /*add by cwx145522 for pm begin*/
/*  mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ); */
    /*add by cwx145522 for pm end*/
	g_debug_cnt++;
    PS_PRINT_INFO("%s g_debug_cnt=%d", __func__, g_debug_cnt);
    atomic_set(&ps_core_d->dbg_func_has_open, 1);
    /* set dbg read timeout default value */
    ps_core_d->dbg_read_delay = DBG_READ_DEFAULT_TIME;
    return 0;
}

/**
 * Prototype    : hw_debug_read
 * Description  : functions called from above oam hal,read count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual read byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_debug_read(struct file *filp, int8 __user *buf,
                                size_t count,loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb = NULL;
    uint16 count1;
    int64 timeout;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    if (0 == ps_core_d->rx_dbg_seq.qlen)
    {   /* if no data, and wait timeout function */
        if (filp->f_flags & O_NONBLOCK)
        {   /* if O_NONBLOCK read and return */
            return -EAGAIN;
        }

        /* timeout function;when have data,can interrupt */
        timeout = wait_event_interruptible_timeout(ps_core_d->rx_dbg_wait,
            (ps_core_d->rx_dbg_seq.qlen > 0), msecs_to_jiffies(ps_core_d->dbg_read_delay));
        if (!timeout)
        {
            PS_PRINT_DBG("debug read time out!");
            return -ETIMEDOUT;
        }
    }

    /* pull skb data from skb queue */
    if (NULL == (skb = ps_skb_dequeue(ps_core_d, RX_DBG_QUEUE)))
    {
        PS_PRINT_ERR("dbg read err!");
        return -ETIMEDOUT;
    }
    /* read min value from skb->len or count */
    count1 = min_t(size_t, skb->len, count);
    if (copy_to_user(buf, skb->data, count1))
    {
        PS_PRINT_ERR("debug copy_to_user is err!");
        ps_restore_skbqueue(ps_core_d, skb, RX_DBG_QUEUE);
        return -EFAULT;
    }
    /* have read count1 byte */
    skb_pull(skb, count1);
    /* if skb->len == 0: curr skb read is over */
    if (0 == skb->len)
    {   /* curr skb data have read to user */
        kfree_skb(skb);
    }
    else
    {   /* if don,t read over; restore to skb queue */
        ps_restore_skbqueue(ps_core_d, skb, RX_DBG_QUEUE);
    }

    return count1;
}

/**
 * Prototype    : hw_debug_write
 * Description  : functions called from above oam hal,write count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual write byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_debug_write(struct file *filp, const int8 __user *buf,
                                size_t count,loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb;
    uint16 total_len;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    if (false == ps_core_d->tty_have_open)
    {
        PS_PRINT_ERR("err: uart not opened!");
        return -EFAULT;
    }

    /* if count is too large;and don,t tx */
    if (count > (DBG_MAX_FRAME - sizeof(struct ps_packet_head) - sizeof(struct ps_packet_end)))
    {
        PS_PRINT_ERR("err: dbg packet is too large!");
        return -EINVAL;
    }

    /* if low queue num > MAX_NUM and don't write */
    if (ps_core_d->tx_low_seq.qlen > TX_LOW_QUE_MAX_NUM)
    {
        return 0;
    }

	if (POWER_OFF_STATE == ps_core_d->fm_state &&
		POWER_OFF_STATE == ps_core_d->gnss_state &&
		POWER_OFF_STATE == ps_core_d->bt_state)
	{
		
        PS_PRINT_ERR("bfg is closed, /dev/hwdebug cant't write!!!");
        return -EINVAL;
	}

    if (check_device_state(ps_core_d, DEBUG_WRITE_FLAG))
    {
        atomic_set(&ps_core_d->bt_fm_data_flag, 1);
    }


    total_len = count + sizeof(struct ps_packet_head) + sizeof(struct ps_packet_end);

    skb  = ps_alloc_skb(total_len);
    if (NULL == skb)
    {
        PS_PRINT_ERR("ps alloc skb mem fail");
        return -EFAULT;
    }

    /* copy data from user to kernel */
    if (copy_from_user(&skb->data[sizeof(struct ps_packet_head)],
                        buf, count))
    {
        PS_PRINT_ERR("copy_from_user from dbg is err");
        kfree_skb(skb);
        return -EFAULT;
    }

    /* add packet head to dbg packet */
    ps_add_packet_head(skb->data, PACKET_TX_FUNC_BFG_DBG, total_len);
    /* push curr skb to low queue */
    ps_skb_enqueue(ps_core_d, skb, TX_LOW_QUEUE);
    queue_work(ps_core_d->ps_tx_workqueue, &ps_core_d->tx_skb_work);
    return count;
}

/**
 * Prototype    : hw_debug_release
 * Description  : called by oam func from hal when close debug inode
 * input        : have opened file handle
 * output       : return 0 --> close is ok
 *                return !0--> close is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_debug_release(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == inode)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
	g_debug_cnt--;
    PS_PRINT_INFO("%s g_debug_cnt=%d", __func__, g_debug_cnt);
	if (0 == g_debug_cnt)
	{
		/* wake up bt dbg wait queue */
		wake_up_interruptible(&ps_core_d->rx_dbg_wait);
		atomic_set(&ps_core_d->dbg_func_has_open, 0);

		/* kfree have rx dbg skb */
		ps_kfree_skb(ps_core_d, RX_DBG_QUEUE);
	}
    return 0;
}

/**********************************************************************/
/**
 * Prototype    : hw_coexist_open
 * Description  : functions called from above hal function,
 *                  input : "/dev/hwcoexist"
 * input        : return 0 --> open is ok
 * output       : return !0--> open is false
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_coexist_open(struct inode *inode, struct file *filp)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);

    if (unlikely((NULL == ps_core_d)||(NULL == inode)||
        (NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    /*add by cwx145522 for pm begin*/
    mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
    /*add by cwx145522 for pm end*/

    return 0;
}

/**
 * Prototype    : hw_coexist_read
 * Description  : functions called from above hal,read count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual read byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_coexist_read(struct file *filp, int8 __user *buf,
                                size_t count,loff_t *f_pos)
{
    PS_PRINT_INFO("%s", __func__);
    return 0;
}

/**
 * Prototype    : hw_coexist_write
 * Description  : functions called from above hal funtion,write count data to buf
 * input        : file handle, buf, count
 * output       : return size --> actual write byte size
 *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC ssize_t hw_coexist_write(struct file *filp, const int8 __user *buf,
                                size_t count,loff_t *f_pos)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb;
    uint16 total_len;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely((NULL == ps_core_d)||
        (NULL == buf)||(NULL == filp)))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    if (false == ps_core_d->tty_have_open)
    {
        PS_PRINT_ERR("err: uart not opened!");
        return -EFAULT;
    }

    /* if count is too large;and don,t tx */
    if (count > COEXIST_MAX_FRAME)
    {
        PS_PRINT_ERR("err: coexist cmd is too large!");
        return -EINVAL;
    }

    /* if high queue num > MAX_NUM and don't write */
    if (ps_core_d->tx_low_seq.qlen > TX_HIGH_QUE_MAX_NUM)
    {
        return 0;
    }

    if (check_device_state(ps_core_d, COEX_WRITE_FLAG))
    {
        atomic_set(&ps_core_d->bt_fm_data_flag, 1);
    }

    total_len = count;

    skb  = ps_alloc_skb(total_len);
    if (NULL == skb)
    {
        PS_PRINT_ERR("ps alloc skb mem fail");
        return -EFAULT;
    }

    /* copy data from user to kernel */
    if (copy_from_user(skb->data, buf, count))
    {
        PS_PRINT_ERR("copy_from_user from coexist is err");
        kfree_skb(skb);
        return -EFAULT;
    }

    /* push curr skb to high queue */
    ps_skb_enqueue(ps_core_d, skb, TX_HIGH_QUEUE);
    queue_work(ps_core_d->ps_tx_workqueue, &ps_core_d->tx_skb_work);
    return count;
}

/**
 * Prototype    : hw_coexist_release
 * Description  : called by func from hal when needs close inode
 * input        : have opened file handle
 * output       : return 0 --> close is ok
 *                return !0--> close is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 hw_coexist_release(struct inode *inode, struct file *filp)
{
    PS_PRINT_INFO("%s", __func__);
    return 0;
}

/**********************************************************************/
#ifdef FEATURE_BT_HCI_LINE_LDISC
/* write by bt protol stack to decide whether or not open uart */
STATIC ssize_t store_open_uart(struct device *dev,
        struct device_attribute *attr, int8 *buf, size_t count)
{
    int32 ret;

    PS_PRINT_INFO("%s, *buf = %d\n", __func__, *buf);
    if (OPEN_UART_COMENT == *buf)
    {
        ret = ps_bt_open_uart();
    }
    else
    {
        ret = ps_bt_close_uart();
    }

    if (ret < 0)
    {
        return ret;
    }
    return count;
}

STATIC struct kobj_attribute bt_open_uart =
__ATTR(open_uart, 0644, NULL, (void *)store_open_uart);

#else

/* bt misc device ops struct */
STATIC const struct file_operations hw_bt_fops = {
        .owner          = THIS_MODULE,
        .open           = hw_bt_open,
        .write          = hw_bt_write,
        .read           = hw_bt_read,
        .poll           = hw_bt_poll,
        .unlocked_ioctl = hw_bt_ioctl,
        .release        = hw_bt_release,
};

STATIC struct miscdevice hw_bt_device = {
        .minor  = MISC_DYNAMIC_MINOR,
        .name   = "hwbt",
        .fops   = &hw_bt_fops,
};
#endif

/* fm misc device ops struct */
STATIC const struct file_operations hw_fm_fops = {
        .owner          = THIS_MODULE,
        .open           = hw_fm_open,
        .write          = hw_fm_write,
        .read           = hw_fm_read,
        .unlocked_ioctl = hw_fm_ioctl,
        .release        = hw_fm_release,
};

/* gnss misc device ops struct */
STATIC const struct file_operations hw_gnss_fops = {
        .owner          = THIS_MODULE,
        .open           = hw_gnss_open,
        .write          = hw_gnss_write,
        .read           = hw_gnss_read,
        .unlocked_ioctl = hw_gnss_ioctl,
        .release        = hw_gnss_release,
};

/* debug misc device ops struct */
STATIC const struct file_operations hw_debug_fops = {
        .owner          = THIS_MODULE,
        .open           = hw_debug_open,
        .write          = hw_debug_write,
        .read           = hw_debug_read,
        .release        = hw_debug_release,
};

/* coexist misc device ops struct */
STATIC const struct file_operations hw_coexist_fops = {
        .owner          = THIS_MODULE,
        .open           = hw_coexist_open,
        .write          = hw_coexist_write,
        .read           = hw_coexist_read,
        .release        = hw_coexist_release,
};

STATIC struct miscdevice hw_fm_device = {
        .minor  = MISC_DYNAMIC_MINOR,
        .name   = "hwfm",
        .fops   = &hw_fm_fops,
};

STATIC struct miscdevice hw_gnss_device = {
        .minor  = MISC_DYNAMIC_MINOR,
        .name   = "hwgnss",
        .fops   = &hw_gnss_fops,
};

STATIC struct miscdevice hw_debug_device = {
        .minor  = MISC_DYNAMIC_MINOR,
        .name   = "hwbfgdbg",
        .fops   = &hw_debug_fops,
};

STATIC struct miscdevice hw_coexist_cmd = {
        .minor  = MISC_DYNAMIC_MINOR,
        .name   = "hwcoexist",
        .fops   = &hw_coexist_fops,
};

/**********************************************************************/
/* functions called from subsystems */
/* called by octty from hal to decide open or close uart */
STATIC ssize_t show_install(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_FUNCTION_NAME;
    return snprintf(buf, 3, "%d\n", pm_data->ldisc_install);
}

/* read by octty from hal to decide open which uart */
STATIC ssize_t show_dev_name(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_FUNCTION_NAME;
    return snprintf(buf, 32, "%s\n", pm_data->dev_name);
}

/* read by octty from hal to decide what baud rate to use */
STATIC ssize_t show_baud_rate(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_FUNCTION_NAME;
    return snprintf(buf, 10, "%lu\n", pm_data->baud_rate);
}

/* read by octty from hal to decide whether or not use flow cntrl */
STATIC ssize_t show_flow_cntrl(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_FUNCTION_NAME;
    return snprintf(buf, 10, "%hu\n", pm_data->flow_cntrl);
}

STATIC ssize_t gnss_lowpower_state_store(struct device *dev,
        struct kobj_attribute *attr, const char *buf, size_t count)
{
    int flag    = 0;

    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    flag = simple_strtol(buf, NULL, 10);
    /*gnss write the flag to request sleep*/
    if (1 == flag)
    {
        /*if bt and fm are both shutdown ,we will pull down gpio directly*/
        PS_PRINT_SUC("gnss request sleep, will check state\n");
        pm_data->core_data->ps_pm->bfg_spin_lock(1);
        if (!timer_pending(&pm_data->core_data->bfg_timer) &&
             check_bfg_not_booting(pm_data->core_data))
        {
            PS_PRINT_SUC("gnss low power request sleep!\n");
            host_allow_bfg_sleep(pm_data->core_data);
        }

        /*set the flag to 1 means gnss request sleep*/
        atomic_set(&pm_data->core_data->gnss_sleep_flag, 1);
        pm_data->core_data->ps_pm->bfg_spin_lock(0);
    }
    else
    {
        PS_PRINT_DBG("invalid gnss lowpower data!\n");
        return -EFAULT;
    }

    return count;
}

STATIC ssize_t show_bfg_lowpower(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_FUNCTION_NAME;
    return snprintf(buf, 10, "%hu\n", pm_data->core_data->bfg_lowpower_enable);
}

STATIC ssize_t store_bfg_lowpower(struct device *dev,
        struct kobj_attribute *attr, const char *buf, size_t count)
{
    int flag    = 0;

    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    flag = simple_strtol(buf, NULL, 10);
    /*gnss write the flag to request sleep*/
    if (1 == flag)
    {
        pm_data->core_data->bfg_lowpower_enable = 1;
    }
    else if (0 == flag)
    {
        pm_data->core_data->bfg_lowpower_enable = 0;
    }
    else
    {
        PS_PRINT_DBG("invalid bfg lowpower data!\n");
        return -EFAULT;
    }

    return count;
}

/* info to bt proto,mean device yes or not have reset device */
STATIC ssize_t show_reset_device(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_INFO("%s: msg == %x", __func__, pm_data->core_data->reset_device_msg);
    return snprintf(buf, 10, "%hu\n", pm_data->core_data->reset_device_msg);
}

/* hal proto use curr inode to force reset device */
STATIC ssize_t store_reset_device(struct device *dev,
        struct kobj_attribute *attr, const char *buf, size_t count)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_INFO("%s", __func__);
    if (STORE_RST_DEVICE_OVER == *buf)
    {
        if (0 == pm_data->core_data->in_reset_device_process)
        {
            PS_PRINT_ERR("proto function bring to reset work!");
            pm_data->core_data->in_reset_device_process = 1;
            queue_work(pm_data->core_data->ps_rst_device_workqueue,
                &pm_data->core_data->rst_device_work);
        }
    }
    return count;
}

/* show curr reset function yes or not open */
STATIC ssize_t show_allow_reset_device(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_INFO("%s", __func__);
    return snprintf(buf, 10, "%hu\n", pm_data->core_data->allow_reset_device);
}

/* set curr reset function yes or not allow open */
STATIC ssize_t store_allow_reset_device(struct device *dev,
        struct kobj_attribute *attr, const char *buf, size_t count)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    PS_PRINT_INFO("%s", __func__);
    if (ALLOW_RST_DEVICE == *buf)
    {
        PS_PRINT_INFO("allow reset device!");
        /* reset func opened */
        pm_data->core_data->allow_reset_device = true;
    }
    else if (MASK_RST_DEVICE == *buf)
    {
        PS_PRINT_INFO("have mask reset device!");
        /* reset func closed */
        pm_data->core_data->allow_reset_device = false;
    }
    return count;
}

/* show curr reset function yes or not open */
STATIC ssize_t show_bfg_exception_num(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    return snprintf(buf, 10, "%hu\n", pm_data->core_data->bfg_exception_cnt);
}

/* set curr reset function yes or not allow open */
STATIC ssize_t store_bfg_exception_num(struct device *dev,
        struct kobj_attribute *attr, const char *buf, size_t count)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);

    if (CLEAR_EXCEPTION_CNT == *buf)
    {
        /* reset func opened */
        PS_PRINT_INFO("set bfg exception count to 0 \n");
        pm_data->core_data->bfg_exception_cnt = 0;
    }
    else     
    {
         PS_PRINT_INFO("only 0 to clear exception count!\n");
    }
    return count;
}


/* show curr bfg proto yes or not opened state */
STATIC ssize_t show_bfg_active_state(struct device *dev,
        struct device_attribute *attr, int8 *buf)
{
    struct ps_plat_s *pm_data = dev_get_drvdata(&hw_ps_device->dev);
    uint8 bt_state = 0;
    uint8 gnss_state = 0;
    uint8 fm_state = 0;

    PS_PRINT_INFO("%s", __func__);
    if (true == atomic_read(&pm_data->core_data->bt_func_has_open))
    {
        bt_state = 1;
    }
    if (true == atomic_read(&pm_data->core_data->gnss_func_has_open))
    {
        gnss_state = 1;
    }
    if (true == atomic_read(&pm_data->core_data->fm_func_has_open))
    {
        fm_state = 1;
    }

    return snprintf(buf, 32, "bt:%hu; gnss:%hu; fm:%hu;\n", bt_state, gnss_state, fm_state);
}

/* structures specific for sysfs entries */
STATIC struct kobj_attribute ldisc_install =
__ATTR(install, 0444, (void *)show_install, NULL);

STATIC struct kobj_attribute uart_dev_name =
__ATTR(dev_name, 0444, (void *)show_dev_name, NULL);

STATIC struct kobj_attribute uart_baud_rate =
__ATTR(baud_rate, 0444, (void *)show_baud_rate, NULL);

STATIC struct kobj_attribute uart_flow_cntrl =
__ATTR(flow_cntrl, 0444, (void *)show_flow_cntrl, NULL);

STATIC struct kobj_attribute gnss_lowpower_cntrl =
__ATTR(gnss_lowpower_state, 0644, NULL, (void *)gnss_lowpower_state_store);

STATIC struct kobj_attribute bfg_lowpower_cntrl =
__ATTR(bfg_lowpower_enable, 0644, (void *)show_bfg_lowpower, (void *)store_bfg_lowpower);

STATIC struct kobj_attribute bfg_reset_device =
__ATTR(bfg_reset_1101, 0664, (void *)show_reset_device, (void *)store_reset_device);

STATIC struct kobj_attribute bfg_allow_reset_device =
__ATTR(allow_reset_1101, 0644, (void *)show_allow_reset_device, (void *)store_allow_reset_device);


STATIC struct kobj_attribute bfg_exception_cnt =
__ATTR(bfg_exception_num, 0644, (void *)show_bfg_exception_num, (void *)store_bfg_exception_num);

STATIC struct kobj_attribute bfg_active_state =
__ATTR(bfg_state, 0444, (void *)show_bfg_active_state, NULL);

STATIC struct attribute *ps_attrs[] = {
        &ldisc_install.attr,
        &uart_dev_name.attr,
        &uart_baud_rate.attr,
        &uart_flow_cntrl.attr,
        #ifdef FEATURE_BT_HCI_LINE_LDISC
        &bt_open_uart.attr,
        #endif
        &gnss_lowpower_cntrl.attr,
        &bfg_lowpower_cntrl.attr,
        &bfg_reset_device.attr,
        &bfg_allow_reset_device.attr,
        &bfg_exception_cnt.attr,
        &bfg_active_state.attr,
        NULL,
};

STATIC struct attribute_group ps_attr_grp = {
    .attrs = ps_attrs,
};

static struct  hw_ps_plat_data   hisi_platform_data = {
    //.dev_name           = "/dev/ttyAMA4",
    //.dev_name           = "/dev/ttyAMA3",
    .dev_name           = "/dev/ttyAMA4",
    .flow_cntrl         = 1,
    .baud_rate          = 921600,
    .suspend            = NULL,
    .resume             = NULL,
    .set_bt_power       = NULL,
    .set_fm_power       = NULL,
    .set_gnss_power     = NULL,
    .clear_bt_power     = NULL,
    .clear_fm_power     = NULL,
    .clear_gnss_power   = NULL,                                                                                                                                      
};

/**********************************************************************/
/**
 * Prototype    : ps_probe
 * Description  : functions called from platform device driver subsystem
 *                  need to have a relevant platform device entry in the
 *                  platform's board-*.c file
 * input        : pdev
 * output       : return 0  is ok
 *                return !0 is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 ps_probe(struct platform_device *pdev)
{
    struct hw_ps_plat_data *pdata = NULL;
    struct ps_plat_s *ps_plat_d;
    int32 status;
    int32 err;
	struct device_node *np;
	const char * pdev_name;

    PS_PRINT_FUNCTION_NAME;

	np = of_find_compatible_node(NULL, NULL,DTS_COMP_HI1101_PS_NAME);	// should be the same as dts node compatible property
	if (NULL == np) {
		PS_PRINT_ERR("Unable to find %s\n",DTS_COMP_HI1101_PS_NAME);
		return -ENOENT;
	}

	if (of_property_read_string(np, "uart-dev", &pdev_name)) 
	{
		PS_PRINT_ERR("%s node doesn't have uart-dev property!\n", np->name);
		return -ENOENT;
	}   

	strncpy(hisi_platform_data.dev_name, pdev_name, strlen(pdev_name));
	PS_PRINT_INFO("%s node have uart-dev property = %s \n", np->name, hisi_platform_data.dev_name);

	pdev->dev.platform_data = &hisi_platform_data;
	pdata = &hisi_platform_data;

    hw_ps_device = pdev;

    ps_plat_d = kzalloc(sizeof(struct ps_plat_s), GFP_ATOMIC);
    if (!ps_plat_d)
    {
        PS_PRINT_ERR("no mem to allocate");
        return -ENOMEM;
    }
    dev_set_drvdata(&pdev->dev, ps_plat_d);

    status = ps_core_init(&ps_plat_d->core_data);
    if (status != 0)
    {
        PS_PRINT_ERR(" PS core init failed");
        kfree(ps_plat_d);
        return -EIO;
    }

    /* refer to itself */
    ps_plat_d->core_data->pm_data = ps_plat_d;
    /* get reference of pdev */
    ps_plat_d->pm_pdev = pdev;

    init_completion(&ps_plat_d->ldisc_installed);
    init_completion(&ps_plat_d->change_baud_rate);

    g_sysfs_hi110x_ps = kobject_create_and_add("hi110x_ps", &devices_kset->kobj);
    if (NULL == g_sysfs_hi110x_ps)
    {
        PS_PRINT_ERR("Failed to creat g_sysfs_hi110x_ps !!!\n ");
		return -ENOMEM;
    }

    status = sysfs_create_group(g_sysfs_hi110x_ps, &ps_attr_grp);
    if (status)
    {
		PS_PRINT_ERR("failed to create sysfs entries");
		kobject_put(g_sysfs_hi110x_ps);
		g_sysfs_hi110x_ps = NULL;
        ps_core_exit(ps_plat_d->core_data);
        /* free the global data pointer */
        kfree(ps_plat_d);
        return -EFAULT;
    }

    /* copying platform data */
    strncpy(ps_plat_d->dev_name, pdata->dev_name, HISI_UART_DEV_NAME_LEN);
	ps_plat_d->dev_name[HISI_UART_DEV_NAME_LEN] = '\0';
    ps_plat_d->flow_cntrl = pdata->flow_cntrl;
    ps_plat_d->baud_rate = pdata->baud_rate;
    PS_PRINT_INFO("sysfs entries created\n");

    tty_recv = ps_core_recv;

#ifndef FEATURE_BT_HCI_LINE_LDISC
    /* register inode file for bt func use */
    err = misc_register(&hw_bt_device);
    if (0 != err)
    {
        ps_core_exit(ps_plat_d->core_data);
        kfree(ps_plat_d);

		kobject_put(g_sysfs_hi110x_ps);
		g_sysfs_hi110x_ps = NULL;
        sysfs_remove_group(g_sysfs_hi110x_ps, &ps_attr_grp);
        PS_PRINT_ERR("Failed to register bt inode using misc_register.\n ");
        return -ENODEV;
    }
#endif

    /* register inode file for fm func use */
    err = misc_register(&hw_fm_device);
    if (0 != err)
    {
        ps_core_exit(ps_plat_d->core_data);
        kfree(ps_plat_d);
		kobject_put(g_sysfs_hi110x_ps);
		g_sysfs_hi110x_ps = NULL;
        sysfs_remove_group(g_sysfs_hi110x_ps, &ps_attr_grp);
        #ifndef FEATURE_BT_HCI_LINE_LDISC
        misc_deregister(&hw_bt_device);
        #endif
        PS_PRINT_ERR("Failed to register fm inode using misc_register.\n ");
        return -ENODEV;
    }

    /* register inode file for gnss func use */
    err = misc_register(&hw_gnss_device);
    if (0 != err)
    {
        ps_core_exit(ps_plat_d->core_data);
        kfree(ps_plat_d);
		kobject_put(g_sysfs_hi110x_ps);
		g_sysfs_hi110x_ps = NULL;
        sysfs_remove_group(g_sysfs_hi110x_ps, &ps_attr_grp);
        #ifndef FEATURE_BT_HCI_LINE_LDISC
        misc_deregister(&hw_bt_device);
        #endif
        misc_deregister(&hw_fm_device);
        PS_PRINT_ERR("Failed to register gnss inode using misc_register.\n ");
        return -ENODEV;
    }

    /* register inode file for bfg device debug use */
    err = misc_register(&hw_debug_device);
    if (0 != err)
    {
        ps_core_exit(ps_plat_d->core_data);
        kfree(ps_plat_d);
		kobject_put(g_sysfs_hi110x_ps);
		g_sysfs_hi110x_ps = NULL;
        sysfs_remove_group(g_sysfs_hi110x_ps, &ps_attr_grp);
    #ifndef FEATURE_BT_HCI_LINE_LDISC
        misc_deregister(&hw_bt_device);
    #endif
        misc_deregister(&hw_fm_device);
        misc_deregister(&hw_gnss_device);
        PS_PRINT_ERR("Failed to register debug inode using misc_register.\n ");
        return -ENODEV;
    }

    /* register inode file for coexist cmd use */
    err = misc_register(&hw_coexist_cmd);
    if (0 != err)
    {
        ps_core_exit(ps_plat_d->core_data);
        kfree(ps_plat_d);
		kobject_put(g_sysfs_hi110x_ps);
		g_sysfs_hi110x_ps = NULL;
        sysfs_remove_group(g_sysfs_hi110x_ps, &ps_attr_grp);
#ifndef FEATURE_BT_HCI_LINE_LDISC
        misc_deregister(&hw_bt_device);
#endif
        misc_deregister(&hw_fm_device);
        misc_deregister(&hw_gnss_device);
        misc_deregister(&hw_debug_device);
        PS_PRINT_ERR("Failed to register coexist cmd inode using misc_register.\n ");
        return -ENODEV;
    }

    PS_PRINT_SUC("%s is over!", __func__);
    return 0;
}

/**
 * Prototype    : ps_suspend
 * Description  : called by kernel when kernel goto suspend
 * input        : pdev, state
 * output       : return 0 --> ps_suspend is ok
 *                return !0--> ps_suspend is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_suspend(struct platform_device *pdev, pm_message_t state)
{
#if 0
    struct hw_ps_plat_data  *pdata = pdev->dev.platform_data;
    if (pdata->suspend)
    {
        return pdata->suspend(pdev, state);
    }

    return -EOPNOTSUPP;
#else
    return 0;
#endif
}

/**
 * Prototype    : ps_resume
 * Description  : called by kernel when kernel resume from suspend
 * input        : pdev
 * output       : return 0 --> ps_suspend is ok
 *                return !0--> ps_suspend is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_resume(struct platform_device *pdev)
{
#if 0
    struct hw_ps_plat_data  *pdata = pdev->dev.platform_data;
    if (pdata->resume)
    {
        return pdata->resume(pdev);
    }

    return -EOPNOTSUPP;
#else
    return 0;
#endif
}

/**
 * Prototype    : ps_remove
 * Description  : called when user applation rmmod driver
 * input        : pdev
 * output       : return 0 --> ps_suspend is ok
 *                return !0--> ps_suspend is false
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
STATIC int32 ps_remove(struct platform_device *pdev)
{
    struct ps_plat_s *ps_plat_d;
    struct hw_ps_plat_data *pdata;

    PS_PRINT_FUNCTION_NAME;

    pdata = pdev->dev.platform_data;
    ps_plat_d = dev_get_drvdata(&pdev->dev);

	kobject_put(g_sysfs_hi110x_ps);
	sysfs_remove_group(g_sysfs_hi110x_ps, &ps_attr_grp);
	g_sysfs_hi110x_ps = NULL;
    PS_PRINT_INFO("sysfs entries removed");

    ps_plat_d->pm_pdev = NULL;
    ps_core_exit(ps_plat_d->core_data);

#ifndef FEATURE_BT_HCI_LINE_LDISC
    misc_deregister(&hw_bt_device);
#endif
    misc_deregister(&hw_fm_device);
    PS_PRINT_INFO("misc fm device have removed");
    misc_deregister(&hw_gnss_device);
    PS_PRINT_INFO("misc gnss device have removed");
    misc_deregister(&hw_debug_device);
    PS_PRINT_INFO("misc debug device have removed");
    misc_deregister(&hw_coexist_cmd);
    PS_PRINT_INFO("misc coexist cmd have removed");

    kfree(ps_plat_d);
    ps_plat_d = NULL;
    return 0;
}

static struct of_device_id hi1101_ps_match_table[] = {
	{ 
		.compatible = DTS_COMP_HI1101_PS_NAME, 
		.data = NULL,
    },
	{ },
};
/**********************************************************/
/*  platform_driver struct for PS module */
STATIC struct platform_driver ps_platform_driver = {
        .probe      = ps_probe,
        .remove     = ps_remove,
        .suspend    = ps_suspend,
        .resume     = ps_resume,
        .driver     = {
            .name   = "hw-ps",
            .owner  = THIS_MODULE,
			.of_match_table	= hi1101_ps_match_table,
        },
};
/* entry point for PS module when insmod ps_drv.ko */
STATIC int32 __init hw_ps_init(void)
{
    int32 ret;
#ifdef CONFIG_HWCONNECTIVITY
    if (!isMyConnectivityChip(CHIP_TYPE_HI110X )) {
        PS_PRINT_ERR("cfg ps chip type is not match, skip driver init");
        return -EINVAL;
    } else {
        PS_PRINT_INFO("cfg ps type is matched with hi110x, continue");
    }
#endif

    PS_PRINT_FUNCTION_NAME;

    ret = platform_driver_register(&ps_platform_driver);
    if (ret)
    {
        PS_PRINT_ERR("Unable to register platform driver.\n");
    }
    return ret;
}

/* exit point for PS module when rmmod ps_drv.ko */
STATIC void __exit hw_ps_exit(void)
{
    platform_driver_unregister(&ps_platform_driver);
}

module_init(hw_ps_init);
module_exit(hw_ps_exit);
MODULE_DESCRIPTION("Public serial Driver for huawei BT/FM/GNSS chips");
MODULE_LICENSE("GPL");

