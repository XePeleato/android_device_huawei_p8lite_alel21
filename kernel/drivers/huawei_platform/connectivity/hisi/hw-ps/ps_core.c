/******************************************************************************

          Copyright (C), 2001-2011, huawei technology limited company

 ******************************************************************************
  Filename   : ps_core.c
  Version    : first draft
  Author     : zwx144390
  Create date: 2012-10-11
  Modify     :
  Func descr : Translition from BT/GNSS/FM to uart,and Recives data from
               uart tty to BT/GNSS/FM
  Func list  :
               ps_skb_dequeue
               ps_skb_enqueue
               ps_core_exit
               ps_core_init
               ps_core_recv
               ps_core_tx_attemper
               ps_uart_ioctl
               ps_write_tty
  Modi histoy:
  1.Data     : 2012-10-11
    Author   : zwx144390
    Modify
    content  : Create File

******************************************************************************/

/*****************************************************************************
  Include Head file
*****************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/seq_file.h>
#include <linux/skbuff.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/jiffies.h>
#include <linux/firmware.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/gpio.h>
#include <linux/debugfs.h>
#include <linux/sysfs.h>
#include <linux/tty.h>
#include <linux/miscdevice.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/rtc.h>

#include "hw_bfg_ps.h"

/* function pointer for rx data */
int32 (*tty_recv)(void *, const uint8 *, int32);

/*****************************************************************************
  Function implement
*****************************************************************************/
/**
 * Prototype    : ps_write_tty
 * Description  : can be called when write data to uart
 *                  This is the internal write function - a wrapper
 *                  to tty->ops->write
 * input        : data -> the ptr of send data buf
 *                count -> the size of send data buf
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
int32 ps_write_tty(struct ps_core_s *ps_core_d,const uint8 *data, int32 count)
{
    struct tty_struct *tty;

    PS_PRINT_FUNCTION_NAME;
    if (unlikely((NULL == ps_core_d) || (NULL == ps_core_d->tty)))
    {
        PS_PRINT_ERR("tty unavailable to perform write");
        return -EINVAL;
    }

    PS_PRINT_DBG("TX:data[0] = %x, data[1] = %x, data[2] = %x, data[3] = %x, data[%d] = %x",
                  data[0],data[1],data[2],data[3], count, data[count-1]);
    tty = ps_core_d->tty;
    /* write data to uart tx buf */
    return tty->ops->write(tty, data, count);
}

#ifdef FEATURE_BT_HCI_LINE_LDISC
/**
 * Prototype    : ps_push_bt_proto_work
 * Description  : push rx bt skb data to bt proto.
 * input        : ps_core_d
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
void ps_push_bt_proto_work(struct work_struct *work)
{
    struct ps_core_s *ps_core_d;
    struct sk_buff *skb;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if ((NULL == ps_core_d) || (false == ps_core_d->bt_registered) ||
        (NULL == ps_core_d->bt_proto)||(NULL == ps_core_d->bt_proto->recv))
    {
        PS_PRINT_ERR(" %s is err", __func__);
        return;
    }

    skb = NULL;
    while((skb = ps_skb_dequeue(ps_core_d, RX_BT_QUEUE)))
    {
        if ((ps_core_d->bt_proto->recv(ps_core_d->bt_proto->priv_data,
                skb->data, skb->len)) != 0)
        {
            PS_PRINT_ERR(" %s-err", __func__);
            kfree_skb(skb);
            return;
        }
        kfree_skb(skb);
        if (ps_core_d->rx_bt_seq.qlen < (RX_BT_QUE_MAX_NUM/2) &&
            (atomic_read(&ps_core_d->throttle_flag)))
        {
            tty_unthrottle(ps_core_d->tty);
            atomic_set(&ps_core_d->throttle_flag, 0);
        }
    }
    return;
}

/**
 * Prototype    : ps_uart_ioctl
 * Description  : to call registration complete callbacks of all protocol
 *                  stack drivers
 * input        : ps_core_d
 *                err : err =0 -> register is ok
 *                      err!=0 -> register is false
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
int32 ps_uart_ioctl(struct ps_core_s *ps_core_d, int8 cmd, int8 coment)
{
    PS_PRINT_INFO(" %s ", __func__);
    if (likely((ps_core_d != NULL) && (true == ps_core_d->bt_registered)
            && (ps_core_d->bt_proto->uart_ioctl != NULL)))
    {
        ps_core_d->bt_proto->uart_ioctl(ps_core_d->bt_proto->priv_data, cmd, coment);
    }
    return 0;
}
#endif

/**
 * Prototype    : ps_change_uart_baud_rate
 * Description  : change arm platform uart baud rate to secend
 *                baud rate for high baud rate when download patch
 * input        : ps_core_d
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
int32 ps_change_uart_baud_rate(int64 baud_rate)
{
    struct ps_plat_s *ps_plat_d;
    uint64 timeleft = 0;

    ps_plat_d = NULL;

    PS_PRINT_INFO("%s", __func__);
    ps_get_plat_reference(&ps_plat_d);
    if (unlikely(NULL == ps_plat_d))
    {
        PS_PRINT_ERR("ps_plat_d is NULL");
        return -EINVAL;
    }

    INIT_COMPLETION(ps_plat_d->change_baud_rate);
    /* set changed baud rate */
    ps_plat_d->flow_cntrl = 1;  /* enable flow_cntrl */
    ps_plat_d->baud_rate = baud_rate;
    ps_plat_d->ldisc_install = 1;   /* open uart */
    /* notify to octty; octty change baud rate */
    sysfs_notify(g_sysfs_hi110x_ps, NULL, "install");
    /* wait change baud rate is over */
    timeleft = wait_for_completion_timeout(&ps_plat_d->change_baud_rate,
            msecs_to_jiffies(HISI_LDISC_TIME));
    if (!timeleft)
    {
        PS_PRINT_ERR("change baud_rate timeout");
        return -EINVAL;
    }
    else
    {
        /* ldisc installed now */
        PS_PRINT_SUC("change baud_rate installed");
        return 0;
    }
}
/**
 * Prototype    : ps_patch_to_nomal
 * Description  : from download patch state to normal state
 * input        : ps_core_d
 * output       : not
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/12/06
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
void ps_patch_to_nomal(void)
{
    struct ps_core_s *ps_core_d;
    PS_PRINT_INFO("%s", __func__);
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if ((NULL == ps_core_d))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return;
    }
    ps_core_d->in_download_patch_state = false;
    /* init complation */
    INIT_COMPLETION(ps_core_d->dev_3in1_opened);
    /* change function pointer, pointer to ps_core_recv */
    tty_recv = ps_core_recv;
    /* init variable for rx data */
    ps_core_d->rx_pkt_total_len = 0;
    ps_core_d->rx_have_recv_pkt_len = 0;
    ps_core_d->rx_have_del_public_len = 0;
    ps_core_d->rx_decode_public_buf_ptr = ps_core_d->rx_public_buf_org_ptr;
    return;
}
uint8 check_bfg_not_booting(struct ps_core_s *ps_core_d)
{
    if ((0 == atomic_read(&ps_core_d->bt_state_booting)) &&
        (0 == atomic_read(&ps_core_d->fm_state_booting))&&
        (0 == atomic_read(&ps_core_d->gnss_state_booting)))
    {
        return true;
    }
    return false;
}
void host_allow_bfg_sleep(struct ps_core_s *ps_core_d)
{
    PS_PRINT_SUC("host allow bfg sleep and pull down gpio!\n");
    ps_core_d->ps_pm->bfg_gpio_state_set(0);
    ps_core_d->ps_pm->bfg_wake_lock(0);
    if (timer_pending(&ps_core_d->beat_timer))
    {
        atomic_set(&ps_core_d->bfg_beat_flag, 0);
    }
    mod_timer(&ps_core_d->beat_timer, jiffies + BEAT_TIME_SLEEP * HZ);
}

/**
 * Prototype    : mod_bt_timer
 * Description  :
 * input        : uint8
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/05/09
 *     Author       : wx145522
 *     Modification : Created function
 *
 */
int32 mod_bt_timer(uint8 on)
{
    struct ps_core_s *ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);

    if (BFG_TIMER_ON == on)
    {
        /*mod timer*/
        mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
    }
    else if (BFG_TIMER_OFF == on)
    {
        /*del timer*/
        del_timer_sync(&ps_core_d->bfg_timer);
    }
    else
    {
        /* mod bfg_timer to n 100 ms */
        mod_timer(&ps_core_d->bfg_timer, jiffies + on * HZ /10);
        PS_PRINT_INFO("mod bfg_timer to %u * 100 ms", on);
    }

    return 0;
}

/**
 * Prototype    : mod_beat_timer
 * Description  :
 * input        : uint8
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/05/09
 *     Author       : wx145522
 *     Modification : Created function
 *
 */
int32 mod_beat_timer(uint8 on)
{
    struct ps_core_s *ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);

    if (on)
    {
        /*mod timer for active*/
        if (timer_pending(&ps_core_d->beat_timer))
        {
            atomic_set(&ps_core_d->bfg_beat_flag, 0);
        }
        mod_timer(&ps_core_d->beat_timer, jiffies + BEAT_TIME_ACTIVE * HZ);
    }
    else
    {
        /*del timer*/
        del_timer_sync(&ps_core_d->beat_timer);
    }

    return 0;
}


/**
 * Prototype    : check_device_state
 * Description  : check bfg device state for transfer data
 * input        : ps_core_d
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/05/09
 *     Author       : wx145522
 *     Modification : Created function
 *
 */
uint8 check_device_state(struct ps_core_s *ps_core_d, uint8 type)
{
    ps_core_d->ps_pm->bfg_spin_lock(1);
    if (0 == ps_core_d->ps_pm->bfg_gpio_state_get())
    {
        if (GNSS_WRITE_FLAG != type)
        {
            /*add by cwx145522 for pm begin*/
            mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
            /*add by cwx145522 for pm end*/
        }
        else
        {
            atomic_set(&ps_core_d->gnss_sleep_flag, 0);
        }
        /*wake up bfg via gpio*/
        ps_core_d->ps_pm->bfg_gpio_state_set(1);

        /*mod beat timer to 3s*/
        mod_timer(&ps_core_d->beat_timer, jiffies + BEAT_TIME_ACTIVE * HZ);
        if (timer_pending(&ps_core_d->beat_timer))
        {
            atomic_set(&ps_core_d->bfg_beat_flag, 0);
        }
        /*wake lock host after wakeup bfg*/
        ps_core_d->ps_pm->bfg_wake_lock(1);
        /*wait for device wake up and init*/
        ps_core_d->ps_pm->bfg_spin_lock(0);
		PS_PRINT_INFO("host has wakeup bfg\n");
        mdelay(10);
        return 0;
    }
    else
    {
        if (GNSS_WRITE_FLAG == type)
        {
            atomic_set(&ps_core_d->gnss_sleep_flag, 0);
        }
        ps_core_d->ps_pm->bfg_spin_lock(0);
        return 1;
    }
}

/**
 * Prototype    : bfg_timer_expire
 * Description  : bfg timer expired function
 * input        : uint64
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/05/09
 *     Author       : wx145522
 *     Modification : Created function
 *
 */
void bfg_timer_expire(uint64 data)
{
    struct ps_core_s *ps_core_d = (struct ps_core_s *)data;

    PS_PRINT_DBG("enter\n");

    ps_core_d->ps_pm->bfg_spin_lock(1);

    if ((POWER_OFF_STATE == ps_core_d->bt_state) && (POWER_OFF_STATE == ps_core_d->fm_state) && \
       (POWER_ON_STATE == ps_core_d->gnss_state))
    {
        if (ps_core_d->bfg_lowpower_enable && (1 == atomic_read(&ps_core_d->gnss_sleep_flag)))
        {
            PS_PRINT_INFO("only gnss in beat timer and in sleep state\n");
            host_allow_bfg_sleep(ps_core_d);
        }
        ps_core_d->ps_pm->bfg_spin_lock(0);
        return;
    }

    PS_PRINT_INFO("bt_fm_data_flag=%d\n", atomic_read(&ps_core_d->bt_fm_data_flag));
    if (0 == atomic_read(&ps_core_d->bt_fm_data_flag))
    {
        /*if bt and fm are idle, we should judge the state of gnss*/
        if (((POWER_OFF_STATE == ps_core_d->gnss_state) ||
            (1 == atomic_read(&ps_core_d->gnss_sleep_flag))) &&
             check_bfg_not_booting(ps_core_d))
        {
            if (ps_core_d->bfg_lowpower_enable)
            {
                host_allow_bfg_sleep(ps_core_d);
            }
            else
            {
                mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
            }
        }
        /*if gnss disagree sleep, we should mod timer*/
        else
        {
            /*add by cwx145522 for pm begin*/
            mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
            /*add by cwx145522 for pm end*/
        }
    }
    /*if bt or fm is busy ,we should mod timer and disagree sleep(gpio stay highlevel)*/
    else
    {
        /*add by cwx145522 for pm begin*/
        mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
        /*add by cwx145522 for pm end*/
    }

    atomic_set(&ps_core_d->bt_fm_data_flag, 0);

    ps_core_d->ps_pm->bfg_spin_lock(0);

    return;
}

/**
 * Prototype    : beat_timer_expire
 * Description  : beat timer expired function
 * input        : uint64
 * output       : no
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/05/17
 *     Author       : wx145522
 *     Modification : Created function
 *
 */
void beat_timer_expire(uint64 data)
{
    struct ps_core_s *ps_core_d = (struct ps_core_s *)data;

    PS_PRINT_DBG("enter\n");

    PS_PRINT_INFO("bfg_beat_flag=%d\n", atomic_read(&ps_core_d->bfg_beat_flag));
    if (0 == atomic_read(&ps_core_d->bfg_beat_flag))
    {
        PS_PRINT_ERR("host can not recvive beat information\n");
        if (true == ps_core_d->allow_reset_device)
        {
            if (0 == ps_core_d->in_reset_device_process)
            {
                PS_PRINT_ERR("beat timer bring to reset work!");
                ps_core_d->in_reset_device_process = 1;
                if ((0 == ps_core_d->ps_pm->bfg_gpio_state_get()) && \
                    (false == atomic_read(&ps_core_d->bt_func_has_open)))
                {
                    PS_PRINT_WARNING("gnss device in deep sleep, not reset device");
                    ps_core_d->in_reset_device_process = 0;
                }
                else
                {
                    PS_PRINT_WARNING("bt open state:%d", atomic_read(&ps_core_d->bt_func_has_open));
                    PS_PRINT_WARNING("gnss open state:%d", atomic_read(&ps_core_d->gnss_func_has_open) );
                    if ((false == atomic_read(&ps_core_d->bt_func_has_open)) && (false == atomic_read(&ps_core_d->gnss_func_has_open)))
                    {
                        PS_PRINT_WARNING("only fm device in deep sleep, not reset device");
                    }
                    else
                    {
                        PS_PRINT_WARNING("add rest_device_work to queue_work");
                        queue_work(ps_core_d->ps_rst_device_workqueue, &ps_core_d->rst_device_work);
                    }
                }
            }
        }
    }

    atomic_set(&ps_core_d->bfg_beat_flag, 0);
    /* yes or no deep sleep state */
    ps_core_d->ps_pm->bfg_spin_lock(1);
    if (1 == ps_core_d->ps_pm->bfg_gpio_state_get())
    {   /* set heat time for normal state */
        mod_timer(&ps_core_d->beat_timer, jiffies + BEAT_TIME_ACTIVE * HZ);
    }
    else
    {   /* set heat time for deep sleep state */
        mod_timer(&ps_core_d->beat_timer, jiffies + BEAT_TIME_SLEEP * HZ);
    }
    ps_core_d->ps_pm->bfg_spin_lock(0);

    return;
}
/*add by cwx145522 for pm end*/

/**
 * Prototype    : ps_exe_sys_func
 * Description  : called by core when recive sys data event from device
 *              : decode rx sys packet function
 * input        : ps_core_d
 * output       : return 0 -> have finish
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_exe_sys_func(struct ps_core_s *ps_core_d)
{
    PS_PRINT_DBG(" %s ", __func__);
    if (NULL == ps_core_d)
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }
    switch(ps_core_d->rx_sys_content)
    {
    case SYS_INF_PL_INIT:
#ifdef FEATURE_PM_DOWNLOAD_PATCH
        /* have inited platform */
        PS_PRINT_INFO("Device plat has open");
        if (BT_DOWNLOAD_PATCH == ps_core_d->down_load_patch_proto)
        {
            PS_PRINT_INFO("Send open BT CMD");
            ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_BT);
        }
        else if (GNSS_DOWNLOAD_PATCH == ps_core_d->down_load_patch_proto)
        {
            PS_PRINT_INFO("Send open GNSS CMD");
            ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_GNSS);
        }
        else if (FM_DOWNLOAD_PATCH == ps_core_d->down_load_patch_proto)
        {
            PS_PRINT_INFO("Send open FM CMD");
            ps_tx_sys_cmd(ps_core_d, PACKET_TX_FUNC_SYS, SYS_CFG_OPEN_FM);
        }
#endif
        break;
    case SYS_INF_BT_INIT:
    case SYS_INF_GNSS_INIT:
    case SYS_INF_FM_INIT:
#ifdef FEATURE_PM_DOWNLOAD_PATCH
        /* if function has inited, rx sys info */
        PS_PRINT_INFO("Function has opened");
        complete(&ps_core_d->dev_3in1_opened);
#endif
        break;
    case SYS_INF_BT_DISABLE:
        break;
    case SYS_INF_GNSS_DISABLE:
        break;
    case SYS_INF_FM_DISABLE:
        break;
    case SYS_INF_BT_EXIT:
        break;
    case SYS_INF_GNSS_EXIT:
        break;
    case SYS_INF_FM_EXIT:
        break;
    case SYS_INF_GNSS_WAIT_DOWNLOAD:
        break;
    case SYS_INF_GNSS_DOWNLOAD_COMPLETE:
        break;
    case SYS_INF_BFG_HEART_BEAT:
        PS_PRINT_INFO("receive heart beat from bfg\n");
        break;
    default:
        break;
    }
    return 0;
}

/**
 * Prototype    : ps_store_gnss_mem_buf
 * Description  : called by core when recive gnss data from device,
 *                  memcpy recive data to mem buf
 * input        : ps_core_d
 *                buf_ptr -> ptr of recived data buf
 * output       : return 0 -> have finish
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_store_gnss_mem_buf(struct ps_core_s *ps_core_d,uint8 *buf_ptr)
{
    PS_PRINT_DBG(" %s ", __func__);
    if ((NULL == ps_core_d) || (NULL == buf_ptr))
    {
        PS_PRINT_ERR("ps_core_d or buf_ptr is NULL");
        return -EINVAL;
    }
    ps_core_d->rx_gnss_pkt_len = ps_core_d->rx_pkt_total_len - sizeof(struct ps_packet_head);
    ps_core_d->rx_gnss_pkt_len = ps_core_d->rx_gnss_pkt_len - sizeof(struct ps_packet_end);
    ps_core_d->rx_gnss_buf_all_len = ps_core_d->rx_gnss_buf_all_len + ps_core_d->rx_gnss_pkt_len;
    if (ps_core_d->rx_gnss_buf_all_len < GNSS_RX_MAX_FRAME)
    {   /* copy gnss data to rx gnss buf */
        memcpy(ps_core_d->rx_gnss_buf_ptr, buf_ptr, ps_core_d->rx_gnss_pkt_len);
        ps_core_d->rx_gnss_buf_ptr = ps_core_d->rx_gnss_buf_ptr + ps_core_d->rx_gnss_pkt_len;
        return 0;
    }
    else
    {
        return RX_GNSS_ERR;
    }
}

/**
 * Prototype    : ps_push_skb_queue
 * Description  : alloc skb buf,and memcopy data to skb buff
 * input        : ps_core_d
 *                buf_ptr -> ptr of recived data buf
 * output       : return 0 -> have finish
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_push_skb_queue(struct ps_core_s *ps_core_d, uint8 *buf_ptr,
                                uint16 pkt_len, uint8 type)
{
    struct sk_buff *skb;

    PS_PRINT_FUNCTION_NAME;
    if ((NULL == ps_core_d) || (NULL == buf_ptr))
    {
        PS_PRINT_ERR("ps_core_d or buf_ptr is NULL");
        return -EINVAL;
    }

    skb = NULL;
    /* alloc sk_buff */
    skb = alloc_skb(pkt_len, GFP_ATOMIC);
    if (NULL == skb) {
        PS_PRINT_ERR("can't allocate mem for new skb");
        return -EINVAL;
    }
    skb_put(skb, pkt_len);
    memcpy(skb->data, buf_ptr, pkt_len);
    /* push alloc skb to skb queue */
    ps_skb_enqueue(ps_core_d, skb, type);
    return 0;
}

/**
 * Prototype    : ps_decode_packet_func
 * Description  : called by core when recive data from device is
 *                  a complete packet
 *              : decode packet function,and push sk_buff head queue
 * input        : ps_core_d
 *                buf_ptr -> ptr of recived data buf
 * output       : return 0 -> have finish
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_decode_packet_func(struct ps_core_s *ps_core_d, uint8 *buf_ptr)
{
    struct sk_buff *skb = NULL;
    uint16 rx_pkt_total_len = 0;
    uint16 dbg_pkt_lenth;
    uint8 *ptr;
    int32  ret = 0;

    PS_PRINT_FUNCTION_NAME;
    if ((NULL == ps_core_d) || (NULL == buf_ptr))
    {
        PS_PRINT_ERR("ps_core_d or buf_ptr is NULL");
        return -EINVAL;
    }

    atomic_set(&ps_core_d->bfg_beat_flag, 1);
    ptr = buf_ptr;
    ptr = ptr + sizeof(struct ps_packet_head);
    switch (ps_core_d->rx_pkt_type)
    {
    case PACKET_RX_FUNC_SYS:
        /* this is a sys packet */
        ps_core_d->rx_sys_content = *ptr;
        ps_exe_sys_func(ps_core_d);
		if ((SYS_INF_BFG_HEART_BEAT == ps_core_d->rx_sys_content) && 
			atomic_read(&ps_core_d->ps_pm->bfg_wake_host_flag))
		{
			if (atomic_read(&ps_core_d->bt_func_has_open) ||
				atomic_read(&ps_core_d->fm_func_has_open))
			{
				PS_PRINT_INFO("beat data, mod bfg_timer 300 ms!\n");
				atomic_set(&ps_core_d->ps_pm->bfg_wake_host_flag, 0);
				mod_timer(&ps_core_d->bfg_timer, jiffies + 3 * HZ /10);
			}

		}

        break;
    case PACKET_RX_FUNC_BT:
        /* this is a bluetooth packet */
        if (atomic_read(&ps_core_d->bt_func_has_open))
        {   /* if bt function is open,and push data to rx bt sk_buff queue */
            if (atomic_read(&ps_core_d->ps_pm->bfg_wake_host_flag))
            {
                PS_PRINT_SUC("receive first bt data after bfg wake host!\n");
                atomic_set(&ps_core_d->ps_pm->bfg_wake_host_flag, 0);
                mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
            }
            else
            {
                atomic_set(&ps_core_d->bt_fm_data_flag, 1);
            }

            /* this is a bluetooth packet */
            PS_PRINT_DBG("rx_bt_seq.qlen = %d", ps_core_d->rx_bt_seq.qlen);
            rx_pkt_total_len = ps_core_d->rx_pkt_total_len - sizeof(struct ps_packet_head);
            rx_pkt_total_len = rx_pkt_total_len - sizeof(struct ps_packet_end);
            /* push this skb to bt read queue */
            ret = ps_push_skb_queue(ps_core_d, ptr, rx_pkt_total_len, RX_BT_QUEUE);
            if (ret < 0)
            {
                return -EINVAL;
            }
            #ifdef FEATURE_BT_HCI_LINE_LDISC
            queue_work(ps_core_d->ps_rx_workqueue, &ps_core_d->rx_bt_work);
            #else
            /* wake up bt read wait_event_interupt */
            wake_up_interruptible(&ps_core_d->rx_bt_wait);
            #endif
            if (ps_core_d->rx_bt_seq.qlen > RX_BT_QUE_MAX_NUM)
            {   /* if rx bt queue is too large, must set RTS busy */
                tty_throttle(ps_core_d->tty);
                atomic_set(&ps_core_d->throttle_flag, 1);
                PS_PRINT_WARNING(" rx bt queue too large!");
            }
        }
        break;
    case PACKET_RX_FUNC_FM:
        /* this is a fm packet */
        if (atomic_read(&ps_core_d->fm_func_has_open))
        {   /* if fm function is open,and push data to rx fm sk_buff queue */
            if (atomic_read(&ps_core_d->ps_pm->bfg_wake_host_flag))
            {
                PS_PRINT_SUC("receive first fm data after bfg wake host!\n");
                atomic_set(&ps_core_d->ps_pm->bfg_wake_host_flag, 0);
                mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
            }
            else
            {
                atomic_set(&ps_core_d->bt_fm_data_flag, 1);
            }

            PS_PRINT_DBG("rx_fm_seq.qlen = %d", ps_core_d->rx_fm_seq.qlen);
            /* this is a fm packet */
            rx_pkt_total_len = ps_core_d->rx_pkt_total_len - sizeof(struct ps_packet_head);
            rx_pkt_total_len = rx_pkt_total_len - sizeof(struct ps_packet_end);
            if (rx_pkt_total_len <= FM_MAX_FRAME)
            {   /* push the skb to fm read queue */
                ret = ps_push_skb_queue(ps_core_d, ptr, rx_pkt_total_len, RX_FM_QUEUE);
                if (ret < 0)
                {
                    return -EINVAL;
                }
                /* wake up fm read wait_event_interupt */
                wake_up_interruptible(&ps_core_d->rx_fm_wait);
                if (ps_core_d->rx_fm_seq.qlen > RX_FM_QUE_MAX_NUM)
                {   /* if rx fm queue is too large, must set RTS busy */
                    tty_throttle(ps_core_d->tty);
                    atomic_set(&ps_core_d->throttle_flag, 1);
                    PS_PRINT_WARNING(" rx fm queue too large!");
                }
            }
        }
        break;
    case PACKET_RX_FUNC_GNSS_START:
        /* this is a gnss start packet */
        if (atomic_read(&ps_core_d->gnss_func_has_open))
        {
            /*add by cwx145522 for pm begin*/
            atomic_set(&ps_core_d->gnss_sleep_flag, 0);
            /*add by cwx145522 for pm end*/

            PS_PRINT_DBG("into ->gnss-START");
            /* check yes or no:this is a gnss start packet */
            if ((RX_GNSS_SEQ_NULL == ps_core_d->rx_gnss_prev_seq)||
               (RX_GNSS_SEQ_LAST == ps_core_d->rx_gnss_prev_seq))
            {   /* copy recv data to rx gnss buf */
                ps_core_d->rx_gnss_prev_seq = RX_GNSS_SEQ_START;
                ret = ps_store_gnss_mem_buf(ps_core_d, ptr);
                if (RX_GNSS_ERR == ret)
                {
                    PS_PRINT_ERR("rx gnss all data lenth is err!");
                }
            }
            else
            {   /* sequence is not correct */
                PS_PRINT_ERR("rx gnss seq is err!");
            }
        }
        break;
    case PACKET_RX_FUNC_GNSS_INT:
        /* this is a gnss internal packet */
        if (atomic_read(&ps_core_d->gnss_func_has_open))
        {
            /*add by cwx145522 for pm begin*/
            atomic_set(&ps_core_d->gnss_sleep_flag, 0);
            /*add by cwx145522 for pm end*/

            PS_PRINT_DBG("into ->gnss-INT");
            /* check yes or no this is a gnss int packet */
            if ((RX_GNSS_SEQ_START == ps_core_d->rx_gnss_prev_seq)||
               (RX_GNSS_SEQ_INT == ps_core_d->rx_gnss_prev_seq))
            {   /* copy recv data to rx gnss buf */
                ps_core_d->rx_gnss_prev_seq = RX_GNSS_SEQ_INT;
                ret = ps_store_gnss_mem_buf(ps_core_d, ptr);
                if (RX_GNSS_ERR == ret)
                {
                    PS_PRINT_ERR("rx gnss all data lenth is err!");
                }
            }
            else
            {   /* sequence is not correct */
                PS_PRINT_ERR("rx gnss seq is err!");
            }
        }
        break;
    case PACKET_RX_FUNC_GNSS_LAST:
        /* this is a gnss last packet */
        if (atomic_read(&ps_core_d->gnss_func_has_open))
        {
            /*add by cwx145522 for pm begin*/
            atomic_set(&ps_core_d->gnss_sleep_flag, 0);
            /*add by cwx145522 for pm end*/

            PS_PRINT_DBG("into ->gnss-LAST");
            /* this is a gnss last packet */
            PS_PRINT_DBG("rx_gnss_seq.qlen = %d", ps_core_d->rx_gnss_seq.qlen);
            ps_core_d->rx_gnss_prev_seq = RX_GNSS_SEQ_LAST;
            ret = ps_store_gnss_mem_buf(ps_core_d, ptr);
            if (RX_GNSS_ERR == ret)
            {
                PS_PRINT_ERR("rx gnss all data lenth is err!");
            }
            /* push the skb to gnss read queue */
            ret = ps_push_skb_queue(ps_core_d, ps_core_d->rx_gnss_buf_org_ptr,
                ps_core_d->rx_gnss_buf_all_len, RX_GNSS_QUEUE);
            if (ret < 0)
            {
                return -EINVAL;
            }
            ps_core_d->rx_gnss_buf_ptr = ps_core_d->rx_gnss_buf_org_ptr;
            ps_core_d->rx_gnss_buf_all_len = 0;
            /* wake up gnss read wait_event_interupt */
            wake_up_interruptible(&ps_core_d->rx_gnss_wait);
            if (ps_core_d->rx_gnss_seq.qlen > RX_GNSS_QUE_MAX_NUM)
            {   /* if rx gnss queue is too large, must set RTS busy */
                tty_throttle(ps_core_d->tty);
                atomic_set(&ps_core_d->throttle_flag, 1);
                PS_PRINT_WARNING(" rx gnss queue too large!");
            }
        }
        break;
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    case PACKET_RX_FUNC_PM_BFG:
        break;
#endif
    case PACKET_RX_FUNC_BFG_DBG:
        /* this is a dbg packet */
        PS_PRINT_DBG("rx_dbg_seq.qlen = %d", ps_core_d->rx_dbg_seq.qlen);
        rx_pkt_total_len = ps_core_d->rx_pkt_total_len - sizeof(struct ps_packet_head);
        rx_pkt_total_len = rx_pkt_total_len - sizeof(struct ps_packet_end);

        /* check yes or not is last words */
        if ((PACKET_START_SIGNAL == ptr[0])&&(PACKET_RX_FUNC_LAST_WORDS == ptr[1]))
        {
            dbg_pkt_lenth = ptr[5]*0x100;
            dbg_pkt_lenth = dbg_pkt_lenth + ptr[4];
            if (dbg_pkt_lenth == rx_pkt_total_len)
            {   /* after check this is last words */
                PS_PRINT_ERR("recv the last words from device!");
                /* check yes or not allow reset device */
                if (true == ps_core_d->allow_reset_device)
                {
                    if (0 == ps_core_d->in_reset_device_process)
                    {
                        PS_PRINT_ERR("last words bring to reset work!");
                        ps_core_d->in_reset_device_process = 1;
                        /* trigger reset device work */
                        queue_work(ps_core_d->ps_rst_device_workqueue, &ps_core_d->rst_device_work);
                    }
                }
            }
        }

        if (atomic_read(&ps_core_d->dbg_func_has_open))
        {
            if (rx_pkt_total_len <= DBG_MAX_FRAME)
            {   /* push the skb to dbg read queue */
                ret = ps_push_skb_queue(ps_core_d, ptr, rx_pkt_total_len, RX_DBG_QUEUE);
                if (ret < 0)
                {
                    PS_PRINT_ERR("push dbg skb queue is err");
                    return -EINVAL;
                }
                /* wake up dbg read wait_event_interupt */
                wake_up_interruptible(&ps_core_d->rx_dbg_wait);
                if (ps_core_d->rx_dbg_seq.qlen > RX_DBG_QUE_MAX_NUM)
                {   /* if rx dbg queue is too large, must remove some skb buf data */
                    PS_PRINT_DBG(" rx dbg queue too large!");
                    /* if sdt data is large£¬remove the head skb data */
                    skb = ps_skb_dequeue(ps_core_d, RX_DBG_QUEUE);
                    kfree_skb(skb);
                }
            }
        }
        break;
    default:
        break;
    };
    return 0;
}

/**
 * Prototype    : ps_check_packet_head
 * Description  : check recived curr packet is or not a complete packet
 * input        : ps_core_d
 *                buf_ptr -> ptr of recived data buf
 *                count   -> size of recived data buf
 * output       : return 0 -> this packet is a complete packet
 *                return 1 -> this packet has not a complete packet,need continue recv
 *                return -1-> this packst is a err packet,del it
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_check_packet_head(struct ps_core_s *ps_core_d, uint8 *buf_ptr, int32 count)
{
    uint8 *ptr;
    uint16 len;
    uint16 lenbak;

    PS_PRINT_FUNCTION_NAME;
    if ((NULL == ps_core_d) || (NULL == buf_ptr))
    {
        PS_PRINT_ERR("ps_core_d or buf_ptr is NULL");
        return -EINVAL;
    }
    ptr = buf_ptr;
    /* check number one byte yes or no 7e */
    if (PACKET_START_SIGNAL == *ptr)
    {   /* if count less then sys packet lenth and continue */
        if (count < SYS_TOTAL_PACKET_LENTH)
        {
            return RX_PACKET_CONTINUE;
        }
        ptr++;
        ps_core_d->rx_pkt_type = *ptr;
        /* check packet type */
        if (PACKET_RX_FUNC_BFG_DBG >= ps_core_d->rx_pkt_type)
        {
            ptr++;
            len = *ptr;
            ptr++;
            lenbak = *ptr;
            lenbak = lenbak * 0x100;
            len = len + lenbak;
            ps_core_d->rx_pkt_total_len = len;
            /* check packet lenth less then sys packet lenth */
            if (ps_core_d->rx_pkt_total_len < SYS_TOTAL_PACKET_LENTH)
            {   /* the packet lenth is err */
                PS_PRINT_ERR("the pkt len is ERR: %x\n", ps_core_d->rx_pkt_total_len);
                return RX_PACKET_ERR;
            }

	        switch (ps_core_d->rx_pkt_type)
	        {
                case PACKET_RX_FUNC_BT:
                    if (BT_FRAME_MAX_LEN < ps_core_d->rx_pkt_total_len)
                    {
                        PS_PRINT_ERR("type=%d, the pkt len is ERR: 0x%x\n", ps_core_d->rx_pkt_type, ps_core_d->rx_pkt_total_len);
                        return RX_PACKET_ERR;
                    }
	                break;
                case PACKET_RX_FUNC_GNSS_START:
                case PACKET_RX_FUNC_GNSS_INT:
                case PACKET_RX_FUNC_GNSS_LAST:
                    if (GNSS_FRAME_MAX_LEN < ps_core_d->rx_pkt_total_len)
                    {
                        PS_PRINT_ERR("type=%d, the pkt len is ERR: 0x%x\n", ps_core_d->rx_pkt_type, ps_core_d->rx_pkt_total_len);
                        return RX_PACKET_ERR;
                    }
	                break;
                case PACKET_RX_FUNC_FM:
                    if (FM_FRAME_MAX_LEN < ps_core_d->rx_pkt_total_len)
                    {
                        PS_PRINT_ERR("type=%d, the pkt len is ERR: 0x%x\n", ps_core_d->rx_pkt_type, ps_core_d->rx_pkt_total_len);
                        return RX_PACKET_ERR;
                    }
	                break;
                case PACKET_RX_FUNC_BFG_DBG:
                    if (DBG_FRAME_MAX_LEN < ps_core_d->rx_pkt_total_len)
                    {
                        PS_PRINT_ERR("type=%d, the pkt len is ERR: 0x%x\n", ps_core_d->rx_pkt_type, ps_core_d->rx_pkt_total_len);
                        return RX_PACKET_ERR;
                    }
	                break;
                default:
                    break;
	        }

            /* check packet lenth large than buf total lenth */
            if (ps_core_d->rx_pkt_total_len > PUBLIC_BUF_MAX)
            {   /* recv packet lenth is err, delete it */
                PS_PRINT_ERR("the pkt len is too large: %x\n", ps_core_d->rx_pkt_total_len);
                ps_core_d->rx_pkt_total_len = 0;
                return RX_PACKET_ERR;
            }
            /* if type is sys packet,and packet lenth large sys pkt lenth,is err packet */
            if ((PACKET_RX_FUNC_SYS == ps_core_d->rx_pkt_type)&&
                (ps_core_d->rx_pkt_total_len > SYS_TOTAL_PACKET_LENTH))
            {
                PS_PRINT_ERR("the pkt type and len err: %x, %x\n", ps_core_d->rx_pkt_type,
                    ps_core_d->rx_pkt_total_len);
                return RX_PACKET_ERR;
            }
            /* if pkt lenth less then sys packet lenth and continue */
            if (ps_core_d->rx_pkt_total_len > count)
            {   /* the packet is breaked, need continue rx */
                return RX_PACKET_CONTINUE;
            }
            ptr = buf_ptr + len - sizeof(struct ps_packet_end);
            /* check the last byte yes or not 7e */
            if (PACKET_END_SIGNAL == *ptr)
            {   /* the packet is a good packet */
                return RX_PACKET_CORRECT;
            }
            else
            {   /* the packet is a err packet */
                PS_PRINT_ERR("the pkt end is ERR: %x\n", *ptr);
                return RX_PACKET_ERR;
            }
        }
        else
        {
            PS_PRINT_ERR("the pkt type is ERR: %x\n", ps_core_d->rx_pkt_type);
            return RX_PACKET_ERR;
        }
    }
    PS_PRINT_ERR("the pkt head is ERR: %x\n", *ptr);
    return RX_PACKET_ERR;
}

/**
 * Prototype    : ps_core_recv
 * Description  : PS's core receive function.Decodes received RAW data and
 *                  forwards to corresponding client drivers (Bluetooth)
 *                  or hal stack(FM,GNSS).
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
int32 ps_core_recv(void *disc_data, const uint8 *data, int32 count)
{
    struct ps_core_s *ps_core_d;
    uint8 *ptr;
    int32 count1 = 0;
    int32 ret = 0;

    PS_PRINT_FUNCTION_NAME;
    if (unlikely(NULL == data) || (NULL == disc_data))
    {
        PS_PRINT_ERR(" received null from TTY ");
        return 0;
    }

    if (count < 0)
    {
        PS_PRINT_ERR(" received count from TTY err");
        return 0;
    }

    ps_core_d = (struct ps_core_s *)disc_data;

#ifdef DEBUG_USE
    vfs_write(ps_core_d->rx_data_fp, data, count, &ps_core_d->rx_data_fp->f_pos);
#endif

    ps_core_d->rx_decode_tty_ptr = (int8 *)data;
    while (count)
    {   /* if curr packet is breaked packet,and first memcpy */
        if (ps_core_d->rx_have_recv_pkt_len)
        {   /* this is a breaked packet */
            ptr = ps_core_d->rx_public_buf_org_ptr;
            ptr = ptr + ps_core_d->rx_have_recv_pkt_len; /* find public buf ptr */
            ps_core_d->rx_have_recv_pkt_len = ps_core_d->rx_have_recv_pkt_len + count;
            /* if recv data len > public buf len,this is err */
            if (ps_core_d->rx_have_recv_pkt_len > PUBLIC_BUF_MAX)
            {
                PS_PRINT_WARNING("received count too large!");
                ps_core_d->rx_have_del_public_len = 0;
                ps_core_d->rx_have_recv_pkt_len = 0;
                ps_core_d->rx_decode_public_buf_ptr = ps_core_d->rx_public_buf_org_ptr;
                continue;
            }
            /* copy all uart buf data to public buf */
            memcpy(ptr, ps_core_d->rx_decode_tty_ptr, count);
            count = 0;
            /* if count less then sys packet lenth and continue */
            if (ps_core_d->rx_have_recv_pkt_len < SYS_TOTAL_PACKET_LENTH)
            {   /* not decode,and continue rxd data */
                continue;
            }
            /* decode public buf data */
            while(ps_core_d->rx_have_del_public_len < ps_core_d->rx_have_recv_pkt_len)
            {   /* find needs decode data in buf */
                count1 = ps_core_d->rx_have_recv_pkt_len - ps_core_d->rx_have_del_public_len;
                /* check curr data */
                ret = ps_check_packet_head(ps_core_d, ps_core_d->rx_decode_public_buf_ptr, count1);
                if (RX_PACKET_CORRECT == ret)
                {   /* this packet is correct, and push it to proto buf */
                    ps_decode_packet_func(ps_core_d,ps_core_d->rx_decode_public_buf_ptr);
                    ps_core_d->rx_have_del_public_len = ps_core_d->rx_have_del_public_len + ps_core_d->rx_pkt_total_len;
                    ps_core_d->rx_decode_public_buf_ptr = ps_core_d->rx_decode_public_buf_ptr + ps_core_d->rx_pkt_total_len;
                    ps_core_d->rx_pkt_total_len = 0;
                    continue;
                }
                else if (RX_PACKET_CONTINUE == ret)
                {   /* this packet is not completed,need continue rx */
                    count1 = ps_core_d->rx_have_recv_pkt_len - ps_core_d->rx_have_del_public_len;
                    /* copy breaked pkt to buf from start */
                    memcpy(ps_core_d->rx_public_buf_org_ptr, ps_core_d->rx_decode_public_buf_ptr, count1);
                    ps_core_d->rx_have_recv_pkt_len = count1;
                    ps_core_d->rx_decode_public_buf_ptr = ps_core_d->rx_public_buf_org_ptr;
                    ps_core_d->rx_have_del_public_len = 0;
                    break;
                }
                else
                {   /* this packet is err! remove a byte and continue decode */
                    ps_core_d->rx_decode_public_buf_ptr ++;
                    ps_core_d->rx_have_del_public_len ++;
                }
            }
            if (ps_core_d->rx_have_del_public_len == ps_core_d->rx_have_recv_pkt_len)
            {   /* have decode all public buf data, reset ptr and lenth */
                ps_core_d->rx_have_del_public_len = 0;
                ps_core_d->rx_have_recv_pkt_len = 0;
                ps_core_d->rx_decode_public_buf_ptr = ps_core_d->rx_public_buf_org_ptr;
            }
        }
        else
        {   /* if not have recv data, direct decode data in uart buf */
            ret = ps_check_packet_head(ps_core_d, ps_core_d->rx_decode_tty_ptr, count);
            if (RX_PACKET_CORRECT == ret)
            {   /* this packet is correct, and push it to proto buf */
                ps_decode_packet_func(ps_core_d,ps_core_d->rx_decode_tty_ptr);
                ps_core_d->rx_decode_tty_ptr = ps_core_d->rx_decode_tty_ptr + ps_core_d->rx_pkt_total_len;
                count = count - ps_core_d->rx_pkt_total_len;
                ps_core_d->rx_pkt_total_len = 0;
                continue;
            }
            else if (RX_PACKET_CONTINUE == ret)
            {   /* this packet is not completed,need continue rx */
                ptr = ps_core_d->rx_public_buf_org_ptr;
                /* copy data to public buf,and continue rx */
                memcpy(ptr, ps_core_d->rx_decode_tty_ptr, count);
                ps_core_d->rx_have_recv_pkt_len = count;
                count = 0;
                continue;
            }
            else
            {   /* this packet is err! remove a byte and continue decode */
                ps_core_d->rx_decode_tty_ptr++;
                count--;
            }
        }
    }
    return 0;
}
static unsigned int g_reset_cnt = 0;
void reset_uart_rx_buf(void)
{
	uint32 i = 0;
	struct ps_core_s *ps_core_d = NULL;
	ps_get_core_reference(&ps_core_d);

    if (0 < ps_core_d->rx_have_recv_pkt_len)
    {   /* have decode all public buf data, reset ptr and lenth */
        PS_PRINT_WARNING("uart rx buf has data, rx_have_recv_pkt_len=%d\n", ps_core_d->rx_have_recv_pkt_len);
	    PS_PRINT_WARNING("uart rx buf has data, rx_have_del_public_len=%d\n", ps_core_d->rx_have_del_public_len);
	    PS_PRINT_WARNING("uart rx buf has data, rx_decode_public_buf_ptr=%p\n", ps_core_d->rx_decode_public_buf_ptr);
	    PS_PRINT_WARNING("uart rx buf has data, rx_public_buf_org_ptr=%p\n", ps_core_d->rx_public_buf_org_ptr);
	    for(i = 0;i < ps_core_d->rx_have_recv_pkt_len; i++)
	    {
			PS_PRINT_DBG("uart rx buf has data, data[%d]=0x%x\n", i, *(ps_core_d->rx_decode_public_buf_ptr + i));
	    }
		g_reset_cnt++;
	    PS_PRINT_WARNING("reset uart rx buf, reset cnt=%d\n", g_reset_cnt);
        ps_core_d->rx_have_del_public_len = 0;
        ps_core_d->rx_have_recv_pkt_len = 0;
        ps_core_d->rx_decode_public_buf_ptr = ps_core_d->rx_public_buf_org_ptr;

    }
}
/**
 * Prototype    : ps_bt_enqueue
 * Description  : push skb data to skb head queue from tail
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
int32 ps_skb_enqueue(struct ps_core_s *ps_core_d, struct sk_buff *skb, uint8 type)
{
    PS_PRINT_FUNCTION_NAME;
    if (unlikely((NULL == skb) || (NULL == ps_core_d)))
    {
        PS_PRINT_ERR(" skb or ps_core_d is NULL");
        return -EINVAL;
    }

    switch (type)
    {
    case TX_HIGH_QUEUE:
        skb_queue_tail(&ps_core_d->tx_high_seq, skb);
        break;
    case TX_LOW_QUEUE:
        skb_queue_tail(&ps_core_d->tx_low_seq, skb);
        break;
    case RX_GNSS_QUEUE:
        skb_queue_tail(&ps_core_d->rx_gnss_seq, skb);
        break;
    case RX_FM_QUEUE:
        skb_queue_tail(&ps_core_d->rx_fm_seq, skb);
        break;
    case RX_BT_QUEUE:
        skb_queue_tail(&ps_core_d->rx_bt_seq, skb);
        break;
    case RX_DBG_QUEUE:
        skb_queue_tail(&ps_core_d->rx_dbg_seq, skb);
        break;
    default:
        break;
    }
    return 0;
}

/**
 * Prototype    : ps_skb_dequeue
 * Description  : internal de sk_buff seq function. return top of txq.
 * input        : ps_core_d
 * output       : sk_buff
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
struct sk_buff *ps_skb_dequeue(struct ps_core_s *ps_core_d, uint8 type)
{
    struct sk_buff *curr_skb;

    PS_PRINT_FUNCTION_NAME;
    if (NULL == ps_core_d)
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return NULL;
    }

    curr_skb = NULL;
    if (TX_HIGH_QUEUE == type)
    {
        curr_skb = skb_dequeue(&ps_core_d->tx_high_seq);
    }
    else if (TX_LOW_QUEUE == type)
    {
        curr_skb = skb_dequeue(&ps_core_d->tx_low_seq);
    }
    else if (RX_GNSS_QUEUE == type)
    {
        curr_skb = skb_dequeue(&ps_core_d->rx_gnss_seq);
    }
    else if (RX_FM_QUEUE == type)
    {
        curr_skb = skb_dequeue(&ps_core_d->rx_fm_seq);
    }
    else if (RX_BT_QUEUE == type)
    {
        curr_skb = skb_dequeue(&ps_core_d->rx_bt_seq);
    }
    else if (RX_DBG_QUEUE == type)
    {
        curr_skb = skb_dequeue(&ps_core_d->rx_dbg_seq);
    }
    return curr_skb;
}

/**
 * Prototype    : ps_core_tx_attemper
 * Description  : adj tx skb buf data to tty, seq BT and FM or GNSS.
 * input        : ps_core_d
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
int32 ps_core_tx_attemper(struct ps_core_s *ps_core_d)
{
    struct sk_buff *skb;
    uint8 tx_high_times = 0;
    int32 len = 0;

    PS_PRINT_FUNCTION_NAME;
    if (NULL == ps_core_d)
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    skb = NULL;
    while(1)
    {
        if (tx_high_times < BT_TX_TIMES)
        {   /* first scan high queue, if not and read low queue skb */
            if ((skb = ps_skb_dequeue(ps_core_d, TX_HIGH_QUEUE)))
            {
                tx_high_times ++;
            }
            else
            {   /* high queue no skb, direct read low queue skb */
                skb = ps_skb_dequeue(ps_core_d, TX_LOW_QUEUE);
                tx_high_times  = 0;
            }
        }
        else
        {   /* first scan low queue, if not and read high queue skb */
            if ((skb = ps_skb_dequeue(ps_core_d, TX_LOW_QUEUE)))
            {
                tx_high_times  = 0;
            }
            else
            {   /* low queue no skb, direct read high queue skb */
                skb = ps_skb_dequeue(ps_core_d, TX_HIGH_QUEUE);
                tx_high_times ++;
            }
        }

        if (NULL == skb)
        {   /* no skb and exit */
            break;
        }

        if ((NULL != ps_core_d->tty))
        {
            /* enable wake-up from TTY */
            set_bit(TTY_DO_WRITE_WAKEUP, &ps_core_d->tty->flags);
        }
        /* tx skb data to uart driver until the skb is NULL */
        while(skb->len)
        {
            if (false == ps_core_d->in_download_patch_state)
            {
                /* len is have tx to uart byte number */
                len = ps_write_tty(ps_core_d, skb->data, skb->len);
                if (len < 0)
                {
                    PS_PRINT_ERR("tty have not opened!");
                    kfree_skb(skb);
                    return 0;
                }
            }
            else
            {   /* curr is download patch state,remove this skb data */
                len = skb->len;
            }
            skb_pull(skb, len);
            /* if skb->len = len as expected, skb->len=0 */
            if (skb->len) {
                msleep(1);
            }
        }
        kfree_skb(skb);
    }
    return 0;
}

/**
 * Prototype    : ps_core_tx_work
 * Description  : adj tx buf data to tty seq, BT and FM or GNSS.
 * input        : ps_core_d
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
void ps_core_tx_work(struct work_struct *work)
{
    struct ps_core_s *ps_core_d;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely(NULL == ps_core_d))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return;
    }
    /* into tx skb buff queue */
    ps_core_tx_attemper(ps_core_d);
    return;
}

/**
 * Prototype    : ps_rst_device_work
 * Description  : this work is used to reset device.
 * input        : ps_core_d
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
void ps_rst_device_work(struct work_struct *work)
{
    struct ps_plat_s *ps_plat_d;
    struct ps_core_s *ps_core_d;
    int32   ret;
    int32   sleep_state;
    STATIC uint8 reset_fm_cmd[] = {0xfb};
    STATIC uint8 reset_gnss_cmd[] = {0xa1,0xb4,0xc7,0x51};

    PS_PRINT_INFO("%s", __func__);
    ps_plat_d = NULL;
    ps_get_plat_reference(&ps_plat_d);
    if (unlikely(NULL == ps_plat_d))
    {
        PS_PRINT_ERR("ps_plat_d is NULL");
        return;
    }
    ps_core_d = ps_plat_d->core_data;

    ps_core_d->ps_pm->bfg_mutex_lock(1);
    if(NULL != ps_core_d->ps_pm->disable_bfg_powerpin_state)
    {
        ps_core_d->ps_pm->disable_bfg_powerpin_state(2);
    }
    do
    {
        PS_PRINT_INFO("info to proto start down device gpio!");
        ps_core_d->reset_device_msg = 0;
        /* tx sys notify info to bt function */
        sysfs_notify(g_sysfs_hi110x_ps, NULL, "bfg_reset_1101");
        msleep(50);
        ps_core_d->reset_device_msg = 1;

        ps_core_d->bfg_exception_cnt += 1;
#ifdef FEATURE_BT_HCI_LINE_LDISC
        if ((NULL != ps_core_d->bt_proto)&&(NULL != ps_core_d->bt_proto->uart_ioctl))
        {
            ps_uart_ioctl(ps_core_d, UART_RST_DEVICE, SHOW_RST_DEVICE_START);
        }
#endif

        if (false == ps_chk_bfg_active(ps_core_d))
        {   /* no proto function is opened,and direct return */
            PS_PRINT_ERR("no proto func is opened");
            break;
        }
        sleep_state = ps_core_d->ps_pm->bfg_gpio_state_get();

        if ((true == atomic_read(&ps_core_d->gnss_func_has_open))||
           (true == atomic_read(&ps_core_d->fm_func_has_open)))
        {
            /* first close uart */
            ret = release_tty_drv(ps_core_d->pm_data);
            if (ret != 0)
            {
                PS_PRINT_ERR("close tty is err!");
                break;
            }
            /* close power gpio,and then open power gpio */
            if ((NULL != ps_core_d->ps_pm)&&(NULL != ps_core_d->ps_pm->reset_bfg_power_gpio))
            {
                PS_PRINT_INFO("start reset device gpio");
                /*  down bfg power gpio */
                if (0 != ps_core_d->ps_pm->reset_bfg_power_gpio(BFG_POWER_GPIO_DOWN))
                {
                    PS_PRINT_ERR("down bfg power gpio is err");
                    break;
                }

                /*  up bfg power gpio */
                if (0 != ps_core_d->ps_pm->reset_bfg_power_gpio(BFG_POWER_GPIO_UP))
                {
                    PS_PRINT_ERR("up bfg power gpio is err");
                    break;
                }
                PS_PRINT_INFO("have reset device gpio");
                ps_core_d->ps_pm->bfg_gpio_state_set(1);
                PS_PRINT_INFO("have reset host wakeup device gpio");
            }

            if (true == atomic_read(&ps_core_d->gnss_func_has_open))
            {
                ret = open_tty_download_patch(ps_core_d, GNSS_DOWNLOAD_PATCH);
                if (ret < 0)
                {
                    PS_PRINT_ERR("open tty or download patch or init GNSS err! ret = %d", ret);
                    break;
                }
                /* push reset commed skb data to queue */
                ret = ps_push_skb_queue(ps_core_d, reset_gnss_cmd,
                    sizeof(reset_gnss_cmd), RX_GNSS_QUEUE);
                if (ret < 0)
                {
                    break;
                }
                /* wake up gnss read wait_event_interupt */
                wake_up_interruptible(&ps_core_d->rx_gnss_wait);
                if ((0 == sleep_state)&& \
                    (false == atomic_read(&ps_core_d->bt_func_has_open)))
                {
                    host_allow_bfg_sleep(ps_core_d);
                    PS_PRINT_WARNING("gnss device in deep sleep, pull down gpio again");
                }
            }
            if (true == atomic_read(&ps_core_d->fm_func_has_open))
            {
                ret = open_tty_download_patch(ps_core_d, FM_DOWNLOAD_PATCH);
                if (ret < 0)
                {
                    PS_PRINT_ERR("open tty or download patch or init FM err! ret = %d", ret);
                    break;
                }
                /* push reset commed skb data to read queue */
                ret = ps_push_skb_queue(ps_core_d, reset_fm_cmd,
                    sizeof(reset_fm_cmd), RX_FM_QUEUE);
                if (ret < 0)
                {
                    break;
                }
                /* wake up fm read wait_event_interupt */
                wake_up_interruptible(&ps_core_d->rx_fm_wait);
                /*add by cwx145522 for pm begin*/
                mod_timer(&ps_core_d->bfg_timer, jiffies + BT_SLEEP_TIME * HZ);
                /*add by cwx145522 for pm end*/
            }
        }
    }while(0);

    ps_core_d->ps_pm->bfg_mutex_lock(0);
#ifdef FEATURE_BT_HCI_LINE_LDISC
    if ((NULL != ps_core_d->bt_proto)&&(NULL != ps_core_d->bt_proto->uart_ioctl))
    {
        ps_uart_ioctl(ps_core_d, UART_RST_DEVICE, SHOW_RST_DEVICE_OVER);
    }
#endif
    ps_core_d->in_reset_device_process = 0;
    PS_PRINT_INFO("reset device work is over!");
    return;
}

/**
 * Prototype    : ps_add_packet_head
 * Description  : add packet head to recv buf from hal or bt driver.
 * input        : buf  -> ptr of buf
 *                type -> packet type£¬example bt,fm,or gnss
 *                lenth-> packet length
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
int32 ps_add_packet_head(uint8 *buf, uint8 type, uint16 lenth)
{
    int8 *ptr;
    uint16 len;

    PS_PRINT_FUNCTION_NAME;
    if (NULL == buf)
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    ptr  = buf;
    *ptr = PACKET_START_SIGNAL;
    ptr++;
    *ptr = type;
    len  = lenth;
    ptr++;
    *ptr = LEN_LOW_BYTE(len);
    ptr++;
    *ptr = LEN_HIGH_BYTE(len);

    ptr = buf;
    ptr = ptr + lenth;
    ptr = ptr - sizeof(struct ps_packet_end);
    *ptr = PACKET_END_SIGNAL;
    return 0;
}

/**
 * Prototype    : ps_set_sys_packet
 * Description  : set sys packet head to buf
 * input        : buf  -> ptr of buf
 *                type -> packet type£¬example bt,fm,or gnss
 *                lenth-> packet length
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
int32 ps_set_sys_packet(uint8 *buf, uint8 type, uint8 content)
{
    int8 *ptr;

    PS_PRINT_FUNCTION_NAME;
    if (NULL == buf)
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    ptr  = buf;
    *ptr = PACKET_START_SIGNAL;
    ptr++;
    *ptr = type;
    ptr++;
    *ptr = SYS_TOTAL_PACKET_LENTH;
    ptr++;
    *ptr = 0;
    ptr++;
    *ptr = content;
    ptr++;
    *ptr = PACKET_END_SIGNAL;
    return 0;
}

/**
 * Prototype    : ps_tx_sys_cmd
 * Description  : tx sys commend to uart tty
 * input        : ps_core_d
 * output       : not
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
int32 ps_tx_sys_cmd(struct ps_core_s *ps_core_d, uint8 type, uint8 content)
{
    struct sk_buff *skb;

    PS_PRINT_FUNCTION_NAME;
    if (NULL == ps_core_d)
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    skb = NULL;
    skb = alloc_skb(SYS_TOTAL_PACKET_LENTH, GFP_ATOMIC);
    if (NULL == skb) {
        PS_PRINT_ERR("can't allocate mem for new sys skb");
        return -EINVAL;
    }
    skb_put(skb, SYS_TOTAL_PACKET_LENTH);
    /* set sys packet head to skb */
    ps_set_sys_packet(skb->data, type, content);
    ps_skb_enqueue(ps_core_d, skb, TX_HIGH_QUEUE);
    queue_work(ps_core_d->ps_tx_workqueue, &ps_core_d->tx_skb_work);
    return 0;
}

#ifdef FEATURE_BT_HCI_LINE_LDISC
/**
 * Prototype    : ps_tx_bt_add_head
 * Description  : add packet dead to recv sk_buff from bt driver.
 * input        : ps_core_d; skb
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
int32 ps_tx_bt_add_head(struct ps_core_s *ps_core_d, struct sk_buff *skb)
{

    PS_PRINT_FUNCTION_NAME;
    if (unlikely((NULL == skb) || (NULL == ps_core_d)))
    {
        PS_PRINT_ERR("bt skb or ps_core_d is NULL");
        return -EINVAL;
    }

    skb_push(skb,sizeof(struct ps_packet_head));
    skb_put(skb,sizeof(struct ps_packet_end));
    ps_add_packet_head(skb->data,PACKET_TX_FUNC_BT,skb->len);
    return 0;
}
#endif

/**
 * Prototype    : ps_tx_gnssbuf
 * Description  : add packet head to recv gnss data buff from hal,and tx to tty uart
 * input        : ps_core_d
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
int32 ps_tx_gnssbuf(struct ps_core_s *ps_core_d, const int8 __user *buf, size_t count)
{
    struct sk_buff *skb;
    uint16 tx_skb_len;
    uint16 tx_gnss_len;
    uint8  start = 0;

    PS_PRINT_FUNCTION_NAME;
    if (unlikely(NULL == ps_core_d))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    while(count > 0)
    {
        if (count > GNSS_PACKET_LIMIT)
        {
            tx_gnss_len = GNSS_PACKET_LIMIT;
        }
        else
        {
            tx_gnss_len = count;
        }
        /* curr tx skb total lenth */
        tx_skb_len = tx_gnss_len + sizeof(struct ps_packet_head);
        tx_skb_len = tx_skb_len + sizeof(struct ps_packet_end);

        skb  = ps_alloc_skb(tx_skb_len);
        if (NULL == skb)
        {
            PS_PRINT_ERR("ps alloc skb mem fail");
            return -EFAULT;
        }

        if (count > GNSS_PACKET_LIMIT)
        {
            if (false == start)
            {   /* this is a start gnss packet */
                ps_add_packet_head(skb->data, PACKET_TX_FUNC_GNSS_START, tx_skb_len);
                start = true;
            }
            else
            {   /* this is a int gnss packet */
                ps_add_packet_head(skb->data, PACKET_TX_FUNC_GNSS_INT, tx_skb_len);
            }
        }
        else
        {   /* this is the last gnss packet */
            ps_add_packet_head(skb->data, PACKET_TX_FUNC_GNSS_LAST, tx_skb_len);
        }

        if (copy_from_user(&skb->data[sizeof(struct ps_packet_head)], buf, tx_gnss_len))
        {
            PS_PRINT_ERR("can't copy_from_user for gnss");
            kfree_skb(skb);
            return -EFAULT;
        }

        /* push the skb to skb queue */
        ps_skb_enqueue(ps_core_d, skb, TX_LOW_QUEUE);
        /* triger tx data to uart work */
        queue_work(ps_core_d->ps_tx_workqueue, &ps_core_d->tx_skb_work);
        buf = buf + tx_gnss_len;
        count = count - tx_gnss_len;
    }
    return 0;
}

/********************************************************************/
/**
 * Prototype    : ps_tty_open
 * Description  : called by tty uart itself when open tty uart from octty
 * input        : tty -> have opened tty
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
STATIC int32 ps_tty_open(struct tty_struct *tty)
{
    int32 err = 0;
    struct ps_core_s *ps_core_d;

    PS_PRINT_FUNCTION_NAME;
    ps_core_d = NULL;
    ps_get_core_reference(&ps_core_d);
    if (unlikely(NULL == ps_core_d))
    {
        PS_PRINT_ERR("ps_core_d is NULL");
        return -EINVAL;
    }

    ps_core_d->tty = tty;
    tty->disc_data = ps_core_d;

    /* don't do an wakeup for now */
    clear_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);

    /* set mem already allocated */
    tty->receive_room = 65536;
    /* Flush any pending characters in the driver and discipline. */
    tty_ldisc_flush(tty);
    tty_driver_flush_buffer(tty);
    /* signal to octty that installation of N_HW_BFG ldisc is complete   */
    ps_tty_complete(ps_core_d->pm_data);
    ps_change_baud_rate_complete(ps_core_d->pm_data);
    PS_PRINT_INFO("%s have done !!!", __func__);
    return err;
}

/**
 * Prototype    : ps_tty_close
 * Description  : called by tty uart when close tty uart from octty
 * input        : tty -> have opened tty
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
STATIC void ps_tty_close(struct tty_struct *tty)
{
    struct  ps_core_s *ps_core_d = NULL;

    PS_PRINT_FUNCTION_NAME;
    if ((NULL == tty)||(NULL == tty->disc_data))
    {
        PS_PRINT_ERR("tty or tty->disc_data is NULL");
        return;
    }
    ps_core_d = tty->disc_data;

    /* Flush any pending characters in the driver and discipline. */
    tty_ldisc_flush(tty);
    tty_driver_flush_buffer(tty);
    ps_core_d->tty = NULL;

    /* signal to complate that N_HW_BFG ldisc is un-installed */
    ps_tty_complete(ps_core_d->pm_data);

    ps_kfree_skb(ps_core_d, TX_HIGH_QUEUE);
    ps_kfree_skb(ps_core_d, TX_LOW_QUEUE);

    PS_PRINT_INFO("%s: have done!!!", __func__);
}

/**
 * Prototype    : ps_tty_receive
 * Description  : called by tty uart when recive data from tty uart
 * input        : tty   -> have opened tty
 *                data -> recive data ptr
 *                count-> recive data count
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
STATIC void ps_tty_receive(struct tty_struct *tty, const uint8 *data,
               int8 *tty_flags, int32 count)
{
    struct  ps_core_s *ps_core_d = NULL;

#ifdef DEBUG_USE
    struct timeval tv;
    struct rtc_time tm;
    uint64  tmp;
    char filename[50] = {0};
    char filename_patch[50] = {0};
#endif

    PS_PRINT_FUNCTION_NAME;
    if (unlikely((NULL == tty)||(NULL == tty->disc_data)||(NULL == tty_recv)))
    {
        PS_PRINT_ERR("tty or tty->disc_data or tty_recv is NULL");
        return;
    }
    ps_core_d = tty->disc_data;

#ifdef DEBUG_USE
    ps_core_d->curr_time = jiffies;
    tmp = ps_core_d->curr_time - ps_core_d->pre_time;
    if ((tmp > DBG_FILE_TIME * HZ)||(0 == ps_core_d->pre_time))
    {
        if (NULL != ps_core_d->rx_data_fp)
        {
            filp_close(ps_core_d->rx_data_fp, NULL);
        }
        do_gettimeofday(&tv);
        rtc_time_to_tm(tv.tv_sec, &tm);

        snprintf(filename_patch, sizeof(filename_patch) - 1, "/data/bfg_data/bfg_patch-%04d%02d%02d%02d%02d%02d",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
        PS_PRINT_INFO("filename = %s",filename_patch);
        snprintf(filename, sizeof(filename) - 1, "/data/bfg_data/bfg_rx-%04d%02d%02d%02d%02d%02d",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
        PS_PRINT_INFO("filename = %s",filename);

        ps_core_d->rx_data_fp_patch = filp_open(filename_patch, O_RDWR | O_CREAT, 0777);
        ps_core_d->rx_data_fp = filp_open(filename, O_RDWR | O_CREAT, 0777);
        ps_core_d->pre_time = ps_core_d->curr_time;
    }
#endif

    spin_lock(&ps_core_d->rx_lock);
#ifdef FEATURE_PM_DOWNLOAD_PATCH
    PS_PRINT_DBG("RX:data[0] = %x, data[1] = %x, data[2] = %x, data[3] = %x, data[count-1] = %x",
                  data[0],data[1],data[2],data[3],data[count-1]);
    tty_recv(tty->disc_data, data, count);
#else
    ps_core_recv(tty->disc_data, data, count);
#endif
    spin_unlock(&ps_core_d->rx_lock);
}

/**
 * Prototype    : ps_tty_wakeup
 * Description  : called by tty uart when wakeup from suspend
 * input        : tty   -> have opened tty
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
STATIC void ps_tty_wakeup(struct tty_struct *tty)
{
    struct  ps_core_s *ps_core_d = NULL;

    PS_PRINT_FUNCTION_NAME;
    if ((NULL == tty)||(NULL == tty->disc_data))
    {
        PS_PRINT_ERR("tty or tty->disc_data is NULL");
        return;
    }
    ps_core_d = tty->disc_data;
    /* don't do an wakeup for now */
    clear_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);

    /* call our internal wakeup */
    queue_work(ps_core_d->ps_tx_workqueue, &ps_core_d->tx_skb_work);
}

/**
 * Prototype    : ps_tty_flush_buffer
 * Description  : called by tty uart when flush buffer
 * input        : tty   -> have opened tty
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
STATIC void ps_tty_flush_buffer(struct tty_struct *tty)
{
#if 0
    struct  ps_core_s *ps_core_d = NULL;

    PS_PRINT_FUNCTION_NAME;
    if ((NULL == tty)||(NULL == tty->disc_data))
    {
        PS_PRINT_ERR("tty or tty->disc_data is NULL");
        return;
    }
    ps_core_d = tty->disc_data;
    tty->ops->flush_buffer(tty);
#endif

    return;
}

STATIC struct tty_ldisc_ops ps_ldisc_ops = {
    .magic          = TTY_LDISC_MAGIC,
    .name           = "n_ps",
    .open           = ps_tty_open,
    .close          = ps_tty_close,
    .receive_buf    = ps_tty_receive,
    .write_wakeup   = ps_tty_wakeup,
    .flush_buffer   = ps_tty_flush_buffer,
    .owner          = THIS_MODULE
};

/********************************************************************/
/**
 * Prototype    : ps_core_init
 * Description  : init ps_core_d struct and kzalloc memery
 * input        : no
 * output       : ps_core_d
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/05
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
int32 ps_core_init(struct ps_core_s **core_data)
{
    struct ps_core_s *ps_core_d;
    struct ps_pm_s *ps_pm_d;
    uint8 *ptr;
    int32  err;

    PS_PRINT_FUNCTION_NAME;

    ps_core_d = kzalloc(sizeof(struct ps_core_s), GFP_KERNEL);
    if (!ps_core_d)
    {
        PS_PRINT_ERR("memory allocation failed");
        return -ENOMEM;
    }

    ptr = kzalloc(PUBLIC_BUF_MAX, GFP_KERNEL );
    if (!ptr)
    {
        kfree(ps_core_d);
        PS_PRINT_ERR("no mem to allocate to public buf!");
        return -ENOMEM;
    }
    memset(ptr, 0, PUBLIC_BUF_MAX);
    ps_core_d->rx_public_buf_org_ptr = ptr;
    ps_core_d->rx_decode_public_buf_ptr = ptr;

    ps_pm_d = kzalloc(sizeof(struct ps_pm_s), GFP_KERNEL);
    if (!ps_pm_d)
    {
        PS_PRINT_ERR("ps_pm_d memory allocation failed");
        kfree(ps_core_d->rx_public_buf_org_ptr);
        kfree(ps_core_d);
        return -ENOMEM;
    }

    err = ps_pm_register(ps_pm_d);
    if (err)
    {
        kfree(ps_core_d->rx_public_buf_org_ptr);
        kfree(ps_core_d->ps_pm);
        kfree(ps_core_d);
        PS_PRINT_ERR("error registering ps_pm err = %d", err);
        return -EFAULT;
    }
    ps_core_d->ps_pm = ps_pm_d;
    ps_pm_d->write_patch = ps_patch_write;
    ps_pm_d->change_baud_rate = ps_change_uart_baud_rate;
    ps_pm_d->reset_bt_timer = mod_bt_timer;
    ps_pm_d->reset_beat_timer = mod_beat_timer;
    ps_pm_d->change_patch_to_nomal  = ps_patch_to_nomal;

    err = tty_register_ldisc(N_HW_BFG, &ps_ldisc_ops);
    if (err)
    {
        ps_pm_unregister(ps_pm_d);
        kfree(ps_core_d->rx_public_buf_org_ptr);
        kfree(ps_core_d->ps_pm);
        kfree(ps_core_d);
        PS_PRINT_ERR("error registering %d line discipline %d",
               N_HW_BFG, err);
        return -EFAULT;
    }

    /* Initialize PS tx and rx queue head. BT module skb's
     * will be pushed in this queue for actual transmission.
     */
    skb_queue_head_init(&ps_core_d->tx_high_seq);
    skb_queue_head_init(&ps_core_d->tx_low_seq);
    skb_queue_head_init(&ps_core_d->rx_gnss_seq);
    skb_queue_head_init(&ps_core_d->rx_fm_seq);
    skb_queue_head_init(&ps_core_d->rx_bt_seq);
    skb_queue_head_init(&ps_core_d->rx_dbg_seq);

    spin_lock_init(&ps_core_d->rx_lock);
    init_completion(&ps_core_d->dev_3in1_opened);
    /* create a singlethread work queue */
    ps_core_d->ps_tx_workqueue = create_singlethread_workqueue("ps_tx_queue");
    /* init tx work */
    INIT_WORK(&ps_core_d->tx_skb_work, ps_core_tx_work);

#ifdef FEATURE_BT_HCI_LINE_LDISC
    ps_core_d->ps_rx_workqueue = create_singlethread_workqueue("ps_rx_queue");
    INIT_WORK(&ps_core_d->rx_bt_work, ps_push_bt_proto_work);
    ps_core_d->bt_registered = false;
#endif

    /* create a singlethread work queue for reset device */
    ps_core_d->ps_rst_device_workqueue = create_singlethread_workqueue("ps_rst_device_queue");
    /* init reset device work */
    INIT_WORK(&ps_core_d->rst_device_work, ps_rst_device_work);

    atomic_set(&ps_core_d->throttle_flag, 0);
    atomic_set(&ps_core_d->bt_func_has_open, 0);
    atomic_set(&ps_core_d->fm_func_has_open, 0);
    atomic_set(&ps_core_d->gnss_func_has_open, 0);
    atomic_set(&ps_core_d->dbg_func_has_open, 0);

    /* add by cwx145522 for pm begin */
    /* init bfg data timer */
    init_timer(&ps_core_d->bfg_timer);
    ps_core_d->bfg_timer.function = bfg_timer_expire;
    ps_core_d->bfg_timer.expires  = jiffies + BT_SLEEP_TIME*HZ;
    ps_core_d->bfg_timer.data     = (unsigned long)ps_core_d;

    /* init heat time */
    init_timer(&ps_core_d->beat_timer);
    ps_core_d->beat_timer.function = beat_timer_expire;
    ps_core_d->beat_timer.expires  = jiffies + BEAT_TIME_ACTIVE*HZ;
    ps_core_d->beat_timer.data     = (unsigned long)ps_core_d;

    atomic_set(&ps_core_d->bt_fm_data_flag, 1);
    atomic_set(&ps_core_d->gnss_sleep_flag, 0);
    atomic_set(&ps_core_d->bfg_beat_flag, 1);

    ps_core_d->bt_state     = POWER_OFF_STATE;
    ps_core_d->fm_state     = POWER_OFF_STATE;
    ps_core_d->gnss_state   = POWER_OFF_STATE;
    ps_core_d->bfg_lowpower_enable = 0;
    /* add by cwx145522 for pm end */

#ifndef FEATURE_BT_HCI_LINE_LDISC
    init_waitqueue_head(&ps_core_d->rx_bt_wait);
#endif
    init_waitqueue_head(&ps_core_d->rx_fm_wait);
    init_waitqueue_head(&ps_core_d->rx_gnss_wait);
    init_waitqueue_head(&ps_core_d->rx_dbg_wait);

    ps_core_d->rx_gnss_prev_seq = RX_GNSS_SEQ_NULL;
    ps_core_d->rx_gnss_buf_all_len = 0;
    ps_core_d->rx_gnss_buf_org_ptr = 0;

    ps_core_d->rx_pkt_total_len = 0;
    ps_core_d->rx_have_recv_pkt_len = 0;
    ps_core_d->rx_have_del_public_len = 0;

    ps_core_d->tty_have_open = false;
    /* when reset_device_msg == 0; then start reset process */
    ps_core_d->reset_device_msg = 1;
    ps_core_d->in_reset_device_process = 0;
    ps_core_d->in_download_patch_state = false;
    ps_core_d->allow_reset_device = true;
    ps_core_d->bfg_exception_cnt = 0;

#ifdef DEBUG_USE
    ps_core_d->curr_time = 0;
    ps_core_d->pre_time = 0;
    ps_core_d->rx_data_fp = 0;
#endif

    *core_data = ps_core_d;
    return 0;
}

/**
 * Prototype    : ps_core_exit
 * Description  : release have init ps_core_d struct and kfree memery
 * input        : ps_core_d
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
int32 ps_core_exit(struct ps_core_s *ps_core_d)
{
    int32 err;

    PS_PRINT_FUNCTION_NAME;

    if (ps_core_d != NULL)
    {
        /* Free PS tx and rx queue */
        ps_kfree_skb(ps_core_d, TX_HIGH_QUEUE);
        ps_kfree_skb(ps_core_d, TX_LOW_QUEUE);
        ps_kfree_skb(ps_core_d, RX_GNSS_QUEUE);
        ps_kfree_skb(ps_core_d, RX_FM_QUEUE);
        ps_kfree_skb(ps_core_d, RX_BT_QUEUE);
        ps_kfree_skb(ps_core_d, RX_DBG_QUEUE);

        /* free tx work queue */
        destroy_workqueue(ps_core_d->ps_tx_workqueue);

        #ifdef FEATURE_BT_HCI_LINE_LDISC
        destroy_workqueue(ps_core_d->ps_rx_workqueue);
        #endif

        destroy_workqueue(ps_core_d->ps_rst_device_workqueue);

        /* del time */
        del_timer_sync(&ps_core_d->bfg_timer);

        del_timer_sync(&ps_core_d->beat_timer);

        /* TTY ldisc cleanup */
        err = tty_unregister_ldisc(N_HW_BFG);
        if (err)
        {
            PS_PRINT_ERR("unable to un-register ldisc %d", err);
        }
        /* unregister pm */
        ps_pm_unregister(ps_core_d->ps_pm);
        /* free the global data pointer */
        kfree(ps_core_d->rx_public_buf_org_ptr);
        kfree(ps_core_d->ps_pm);
        kfree(ps_core_d);
    }
    return 0;
}

