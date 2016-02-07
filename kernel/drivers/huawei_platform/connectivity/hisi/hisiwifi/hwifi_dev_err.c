/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 *
 * File Name     : hwifi_dev_err.c
 * Version       : Initial Draft
 * Author        : xudayong
 * Created       : 2013/9/17
 * Last Modified :
 * Description   : netlink for dev error
 * Function List :
 * History       :
 * 1.Date        : 2013/9/17
 *   Author      : xudayong
 *   Modification: Created file
 *
 */


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <net/netlink.h>

#include "common.h"
#include "hi110x.h"
#include "hwifi_utils.h"
#include "hwifi_hcc.h"
#include "hwifi_cfg80211.h"
#include "hwifi_dev_err.h"
#include "hwifi_cfgapi.h"
#include "hi110x_pm.h"


/*V8R1 B180 platform path change  by zhouxinfeng 2014/7/14*/
#define HWIFI_POWERIN_STATE_STR "/sys/devices/hi110x_power/powerpin_state"
/*BIT0:1 means wifi ctrl poweron, 0 means not ctrl*/
/*BIT2:1 means power on ctrl by bit0, 0 means ctrl by hw-pm module*/
#define PM_POWER_PIN_WIFI     (0)
#define PM_POWER_PIN_CTR_WIFI (2)
/*
 * 2 Global Variable Definition
 */
struct dev_excp_globals dev_excp_handler_data;
extern int32  exception_mask;
extern int32 hwifi_report_diconnect_res(struct cfg_struct *cfg, uint16 reason);
extern int32 hwifi_connectting_fail_report_conn_res(struct cfg_struct *cfg, int32 connect_status, uint16 status_code);

/*
 * 3 Function Definition
 */
/*
 * Prototype    : dev_netlink_rev
 * Description  : receive netlink date from app
 * Input        : void
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/8
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void dev_netlink_rev(struct sk_buff *skb)
{
    struct sk_buff                 *pst_skb;
    struct nlmsghdr                *pst_nlh;
    struct dev_netlink_msg_hdr_stru st_msg_hdr;
    int32                           l_len;

    if (NULL == skb)
    {
        HWIFI_ERROR("para fail\n");
        return;
    }

    HWIFI_DEBUG("dev_kernel_netlink_recv.\n");

    pst_skb = NULL;
    pst_nlh = NULL;

    memset(dev_excp_handler_data.data, 0, DATA_BUF_LEN);

    pst_skb = skb_get(skb);
    if (pst_skb->len >= NLMSG_SPACE(0))
    {
        pst_nlh = nlmsg_hdr(pst_skb);
        l_len   = NLMSG_PAYLOAD(pst_nlh, 0);
        OS_MEM_CPY(dev_excp_handler_data.data, NLMSG_DATA(pst_nlh), l_len);
        OS_MEM_CPY((void *)&st_msg_hdr, dev_excp_handler_data.data, sizeof(st_msg_hdr));

        if (0 == st_msg_hdr.cmd)
        {
            dev_excp_handler_data.usepid = pst_nlh->nlmsg_pid;   /*pid of sending process */

            HWIFI_DEBUG("pid is [%d], msg is [%s]\n", dev_excp_handler_data.usepid , &dev_excp_handler_data.data[sizeof(st_msg_hdr)]);
        }
    }

        kfree_skb(pst_skb);

    return;

}

/*
 * Prototype    : dev_netlink_create
 * Description  : create netlink for device exception
 * Input        : void
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/8
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 dev_netlink_create( void )
{
    struct netlink_kernel_cfg netlink_cfg = {
         .input = dev_netlink_rev,
    };
    dev_excp_handler_data.nlsk = netlink_kernel_create(&init_net, NETLINK_DEV_ERROR, &netlink_cfg);
    if (NULL == dev_excp_handler_data.nlsk)
    {
        HWIFI_ERROR("fail to create netlink socket");
        return -EFAIL;
    }

    HWIFI_DEBUG("suceed to create netlink socket");
    return SUCC;
}


/*
 * Prototype    : dev_netlink_send
 * Description  : send netlink data
 * Input        : void
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/8
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 dev_netlink_send (unsigned char *data, int data_len)
{
    struct sk_buff         *skb;
    struct nlmsghdr        *nlh;
    int32                   ret;
    int32                   len;

    len = NLMSG_SPACE(data_len);
    skb = alloc_skb(len, GFP_KERNEL);
    if (NULL == skb)
    {
        HWIFI_ERROR("dev error: allocate failed.\n");
        return -EFAIL;
    }
    nlh = nlmsg_put(skb, 0, 0, 0, data_len, 0);

    memcpy(NLMSG_DATA(nlh), data, data_len);

    NETLINK_CB(skb).portid = 0;                 /* from kernel */

    ret = netlink_unicast(dev_excp_handler_data.nlsk, skb, dev_excp_handler_data.usepid, MSG_DONTWAIT);

    HWIFI_INFO("send dev error netlink msg, ret = %d", ret);

    return ret;
}
int32 dev_netlink_send_atomic(unsigned char *data, int data_len)
{
    struct sk_buff         *skb;
    struct nlmsghdr        *nlh;
    int32                   ret;
    int32                   len;

    len = NLMSG_SPACE(data_len);
    skb = alloc_skb(len, GFP_ATOMIC);
    if (NULL == skb)
    {
        HWIFI_ERROR("dev error: allocate failed.\n");
        return -EFAIL;
    }
    nlh = nlmsg_put(skb, 0, 0, 0, data_len, 0);

    memcpy(NLMSG_DATA(nlh), data, data_len);

    NETLINK_CB(skb).portid = 0;                 /* from kernel */

    ret = netlink_unicast(dev_excp_handler_data.nlsk, skb, dev_excp_handler_data.usepid, MSG_DONTWAIT);

    HWIFI_INFO("send netlink msg, ret = %d", ret);

    return ret;
}
void hwifi_reduce_bt_priority(void)
{
    int32 ret;
    dev_excp_handler_data.mode = HWIFI_REDUCE_BT_PRI;
    ret = dev_netlink_send_atomic(&dev_excp_handler_data.mode, sizeof(unsigned char));
    HWIFI_DEBUG("Send msg mode:%d,ret=%d", HWIFI_REDUCE_BT_PRI, ret);
}

/*
 * Prototype    : hwifi_process_sta_excp
 * Description  :
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/8/7
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_process_sta_excp(struct cfg_struct *cfg)
{
    struct hi110x_device             *hi110x_dev;
    int32                             ret = -EFAIL;
    uint16                            reason = 1;
    HWIFI_ASSERT(NULL != cfg);

    hi110x_dev = cfg->hi110x_dev;
    HWIFI_WARNING("sta mode,start to process dev exception");

    dev_excp_handler_data.mode = HWIFI_STA_MODE;

    hwifi_set_init_flag(cfg, FALSE);

    if (HWIFI_CONNECTING == cfg->conn.status)
    {
        HWIFI_WARNING("Connecting, Connecting fail report conn res");
        hwifi_connectting_fail_report_conn_res(cfg, AUTH_FAILED, reason);
    }

    /* disconnect */
    hwifi_report_diconnect_res(cfg, reason);

    /* stop scanning before restart */
    hwifi_scan_done(cfg);
    //wlan_opera_lock(hi110x_dev->pm_info);
    /* power down device */
    hwifi_wlan_close(hi110x_dev->netdev);
    /* power on device */
    ret = hwifi_wlan_open(hi110x_dev->netdev);
    if(ret != SUCC)
    {
        HWIFI_ERROR("Failed to restart device");
        //wlan_opera_unlock(hi110x_dev->pm_info);
        return ret;
    }

    //wlan_opera_unlock(hi110x_dev->pm_info);

    ret = dev_netlink_send(&dev_excp_handler_data.mode, sizeof(unsigned char));

    return ret;
}

/*
 * Prototype    : hwifi_process_ap_excp
 * Description  : process device exception when AP mode
 * Input        : void
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/8
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_process_ap_excp(struct cfg_struct *cfg)
{
    int32 ret;

    HWIFI_WARNING("AP mode,start to process dev exception");

    hwifi_ap_deinit(cfg);
    dev_excp_handler_data.mode = HWIFI_AP_MODE;
    ret = dev_netlink_send(&dev_excp_handler_data.mode, sizeof(unsigned char));
    return ret;
}

/*
 * Prototype    : hwifi_process_p2p_excp
 * Description  : process device exception when p2p is on
 * Input        : void
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/5
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_process_p2p_excp(struct cfg_struct *cfg)
{
    int32                 ret;
    struct hi110x_device *hi110x_dev;
    hi110x_dev = cfg->hi110x_dev;
    HWIFI_WARNING("start to process p2p dev exception");

    hwifi_set_init_flag(cfg, FALSE);

    if (IS_AP(cfg))
    {
        hwifi_ap_deinit(cfg);
    }
    else
    {
        /* stop scanning before restart */
        hwifi_scan_done(cfg);
    }

    dev_excp_handler_data.mode = HWIFI_P2P_MODE;
    //wlan_opera_lock(hi110x_dev->pm_info);

    /* power down device */
    hwifi_wlan_close(hi110x_dev->netdev);

    /* power on device */
    ret = hwifi_wlan_open(hi110x_dev->netdev);
    if(ret != SUCC)
    {
        HWIFI_ERROR("Failed to restart device");
        //wlan_opera_unlock(hi110x_dev->pm_info);
        return ret;
    }

    //wlan_opera_unlock(hi110x_dev->pm_info);

    ret = dev_netlink_send(&dev_excp_handler_data.mode, sizeof(unsigned char));

    return ret;
}

/*
 * Prototype    : hwifi_dev_excp_recv_data
 * Description  : process device exception
 * Input        : void
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/8
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_dev_excp_recv_data(uint8 main_type, uint8 sub_type, struct sk_buff *skb, void *param)
{
    int32   ret = SUCC;
    struct cfg_struct *cfg;
    struct hcc_handler *hcc = (struct hcc_handler *)param;

    cfg = hcc->hi110x_dev->cfg;

    HWIFI_INFO("recv date in dev error netlink");

    if (IS_P2P_ON(cfg))
    {

        ret = hwifi_process_p2p_excp(cfg);
    }
    else if (IS_STA(cfg))
    {

        ret = hwifi_process_sta_excp(cfg);
    }
    else if (IS_AP(cfg))
    {

        ret = hwifi_process_ap_excp(cfg);
    }
    return ret;
}


int32  hwifi_exception_enable_set(struct hcc_handler *hcc, int32 enable)
{
    int32 ret;
    unsigned long flags;
    HWIFI_BUG_ON(!hcc);
    spin_lock_irqsave(&hcc->excp_lock, flags);
    ret = exception_mask;
    exception_mask = enable;
    spin_unlock_irqrestore(&hcc->excp_lock, flags);
    return ret;
}

int hwifi_exception_submit(struct hcc_handler *hcc)
{
    int ret = SUCC;
    unsigned long flags;
    struct hi110x_sdio *hi_sdio;
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!hcc->io_dev);
    hi_sdio = bus_to_sdio(hcc->io_dev);
    /*sdio transfer  error, stop heartbeat timer andpm watchdog,
      we need to close sdio & hcc channel*/
    spin_lock_irqsave(&hcc->excp_lock, flags);
    if(!hcc->excp_count)
    {
        /*record first exception occur time.*/
        hcc->excp_first_func   = (void*)_RET_IP_;
        hcc->excp_cpu_time_first = cpu_clock(UINT_MAX);
    }else{
        /*record last exception occur time.*/
        hcc->excp_last_func   = (void*)_RET_IP_;
        hcc->excp_cpu_time_last = cpu_clock(UINT_MAX);
    }
    hcc->excp_count++;
    if(exception_enable && exception_mask)
    {
        hcc_set_tx_queue_status(hcc, hcc_abort);
        bus_set_state(&hi_sdio->io_dev, FALSE);
        ret = hi110x_comm_work_submit(hi_sdio, &hcc->excp_worker);
    }else{
        ret = 0;
        HWIFI_WARNING("Exception disabled, exception mask :%d, Force wifi continue working,Just dump wifi log",
                                         exception_mask);
        hsdio_union_log_submit(hi_sdio);
    }
    spin_unlock_irqrestore(&hcc->excp_lock, flags);
    return ret;
}
int32 hwifi_exception_force_submit(struct hcc_handler *hcc, char* type)
{
    int32  exception_t, ret;
    exception_t =  exception_enable;
    exception_enable = TRUE;
    HWIFI_WARNING("Submit a %s exception!", type);
    ret = hwifi_exception_submit(hcc);
    exception_enable = exception_t;
    return ret;
}

void hwifi_powerpin_state_save(int* flag)
{
    int state;
    char buf[20] = {0};
    struct file* fp;
    mm_segment_t fs;
    HWIFI_BUG_ON(!flag);
    fs = get_fs();
    set_fs(KERNEL_DS);
    fp = filp_open(HWIFI_POWERIN_STATE_STR, O_RDWR, 0777);
    if (IS_ERR(fp))
    {
        HWIFI_ERROR("Open %s failed ret=%ld",HWIFI_POWERIN_STATE_STR, PTR_ERR(fp));
        set_fs(fs);
        return;
    }
    if(vfs_read(fp, (char*)buf, sizeof(int), &fp->f_pos) <=0)
    {
        HWIFI_ERROR("Read %s failed!", HWIFI_POWERIN_STATE_STR);
        goto failed;
    }
    if (sscanf(buf, "%d", flag) != 1) {
        HWIFI_ERROR("Invalid result get from %s\n", HWIFI_POWERIN_STATE_STR);
        goto failed;
    }
    memset(buf, 0, sizeof(buf));
    HWIFI_INFO("WiFi powerpin state 0x%4x", *flag);
    state = (*flag) | (1<<PM_POWER_PIN_WIFI) | (1<<PM_POWER_PIN_CTR_WIFI);
    snprintf(buf, sizeof(buf), "%d", state);
    if(vfs_write(fp, buf, sizeof(int), &fp->f_pos) <=0 )
    {
        HWIFI_ERROR("Write %s failed",  HWIFI_POWERIN_STATE_STR);
        goto failed;
    }
    HWIFI_INFO("Write %s to  %s", buf,HWIFI_POWERIN_STATE_STR);

    set_fs(fs);
    filp_close(fp, NULL);
    return;
failed:
    *flag = -EFAIL;
    set_fs(fs);
    filp_close(fp, NULL);
    return;

}

void hwifi_powerpin_state_restore(int flag)
{
    char buf[20] = {0};
    struct file* fp;
    mm_segment_t fs;
    if(flag < 0)
    {
        HWIFI_ERROR("Save power pin state failed, need't restore");
        return;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    fp = filp_open(HWIFI_POWERIN_STATE_STR, O_RDWR, 0777);
    if (IS_ERR(fp))
    {
        HWIFI_ERROR("Open %s failed ret=%ld",HWIFI_POWERIN_STATE_STR, PTR_ERR(fp));
        goto failed;
    }
    snprintf(buf, sizeof(buf), "%d", flag);
    if(vfs_write(fp, buf, sizeof(int), &fp->f_pos) <=0 )
    {
        HWIFI_ERROR("Write %s failed",  HWIFI_POWERIN_STATE_STR);
        goto failed;
    }
    HWIFI_INFO("Restore powerpin state 0x%4x", flag);
    set_fs(fs);
    filp_close(fp, NULL);
    return;
failed:
    set_fs(fs);
    filp_close(fp, NULL);
    return;
}
/*
 * Prototype    : hwifi_dev_excp_rx
 * Description  : report device exception
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/8/28
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void hwifi_dev_excp_rx(struct work_struct *excp_worker)
{
    int         wlan_stat = 0;
    int32                    ret;
    struct hcc_rx_action    *rx_action;
    struct hcc_handler      *hcc;
    hcc_state state;

    hcc = container_of(excp_worker, struct hcc_handler, excp_worker);

    HWIFI_ASSERT(hcc != NULL);
    if(hcc_abort != hcc_get_tx_queue_status(hcc))
    {
        state = hcc_get_tx_queue_status(hcc);
        HWIFI_WARNING("Hcc now %s, need't process exception.",
                       hcc_get_chan_stat_str(state));

        return;
    }
    /*hcc->excp_count++;*/
    HWIFI_WARNING("Fatal wifi error, Enter wifi exception process");
    hwifi_union_log_dump(KERN_DEBUG);

    HWIFI_WARNING("We try to reopen wlan device");

    /*set hw-pm to reset wlan en, not power off, export the device exception file*/


    rx_action   = &hcc->rx_actions[HCC_TYPE_DEV_ERROR];

    if (unlikely(NULL == rx_action->rx_cb))
    {
        HWIFI_WARNING("rx_cb is not ready");
        return;
    }
    hwifi_powerpin_state_save(&wlan_stat);
    mutex_lock(&rx_action->mutex_lock);
    ret = rx_action->rx_cb(HCC_TYPE_DEV_ERROR, HCC_SUBTYPE_ANY, NULL, rx_action->context);
    if (unlikely(ret < 0))
    {
        HWIFI_WARNING("process rx handle failed, main/sub(%u/%u).", HCC_TYPE_WIFI_CTRL, HCC_SUBTYPE_ANY);
        mutex_unlock(&rx_action->mutex_lock);
        hwifi_powerpin_state_restore(wlan_stat);
        return;
    }
    mutex_unlock(&rx_action->mutex_lock);
    hwifi_powerpin_state_restore(wlan_stat);
    HWIFI_INFO("Wifi exception process Done.");
    return;
}

/*
 * Prototype    : init_dev_excp_handler
 * Description  : init dev exception handler
 * Input        : void
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/8
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 init_dev_excp_handler(struct hcc_handler *hcc)
{
    int32   ret;

    HWIFI_DEBUG("into init_exception_enable_handler\n");

    INIT_WORK(&hcc->excp_worker, hwifi_dev_excp_rx);

    hcc_rx_register(hcc, HCC_TYPE_DEV_ERROR, hwifi_dev_excp_recv_data, (void *)hcc);

    memset((void *)&dev_excp_handler_data, 0, sizeof(dev_excp_handler_data));

    dev_excp_handler_data.data = kmalloc(DATA_BUF_LEN, GFP_KERNEL);
    if (NULL == dev_excp_handler_data.data)
    {
        HWIFI_ERROR("malloc dev_excp_handler_data.puc_data fail, len = %d.\n", DATA_BUF_LEN);
        return -EFAIL;
    }
    OS_MEM_SET(dev_excp_handler_data.data, 0, DATA_BUF_LEN);

    ret = dev_netlink_create();
    if (0 > ret)
    {
        kfree(dev_excp_handler_data.data);
        HWIFI_ERROR("init_dev_err_kernel init is ERR!");
        return -EFAIL;
    }

    HWIFI_DEBUG("init_exception_enable_handler init ok.\n");

    return SUCC;
}

/*
 * Prototype    : deinit_dev_excp_handler
 * Description  : deinit dev exception handler
 * Input        : void
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/8
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void deinit_dev_excp_handler(struct hcc_handler *hcc)
{
    cancel_work_sync(&hcc->excp_worker);
    hcc_rx_unregister(hcc, HCC_TYPE_DEV_ERROR);

    if (dev_excp_handler_data.nlsk != NULL)
    {
        netlink_kernel_release(dev_excp_handler_data.nlsk);
        dev_excp_handler_data.usepid = 0;
    }

    if (!dev_excp_handler_data.data)
    {
        kfree(dev_excp_handler_data.data);
    }

    HWIFI_DEBUG("oamkernel remove ok.\n");

    return;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

