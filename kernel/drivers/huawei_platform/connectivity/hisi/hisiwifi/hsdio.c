

/*
 * 1 Header File Including
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/wlan_plat.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/stat.h>   /* for S_IRUGO|S_IWUSR */
#include <linux/mmc/sdio.h>
#include <linux/mmc/host.h>

#include "hwifi_utils.h"
#include "hsdio.h"
#include "common.h"
#include "hi110x.h"
#include "hwifi_hcc.h"
#include "user_ctrl.h"
#include "hi110x_pm.h"
#include "hwifi_cfg80211.h"
#include "hwifi_msg.h"
#include "hwifi_perform.h"
#include "hi110x_pm.h"
#include "hwifi_dev_err.h"

#include "hwifi_config.h"
#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#endif
#define DTS_COMP_WIFI_POWER_NAME "hisilicon,hi1101_wifi"
#define SDIO_CFG_CHECK_FILE      "/system/vendor/firmware/sdio_recover_cfg"
/* SDIO 接口下载patch */
#define INFO_SDIO_DOWN_PATCH     0

/*
 * 2 Global Variable Definition
 */
int32 g_lCpuId = 0;
int32 g_dev_err_subcnt = 0;
STATIC  struct completion  sdio_driver_complete;
#ifdef CONFIG_USE_OF
STATIC  struct completion  wifi_dev_complete;
#endif
/*int32 g_sdio_status = FALSE;*/
/*
 * 3 Function Definition
 */

/* coming from arch/arm/mach-k3v2/common.c */
extern int hi110x_sdio_detectcard_to_core(int val);
#ifdef WLAN_POWER_MANAGEMENT
extern int wlan_power_set(uint8 on);
extern int wlan_power_off(void);
#endif
//STATIC void hi110x_sdio_destroy_rxbuf_pool(struct hi110x_sdio *hi_sdio);
void hi110x_sdio_dispose_data(struct hi110x_sdio  *hi_sdio);
int32 hi110x_sdio_data_sg_irq(struct hi110x_sdio *hi_sdio);

int intr_mode = 1;  /* 0 -sdio 1-gpio*/
module_param(intr_mode, int, S_IRUGO);

struct semaphore sdio_wake_sema;
/*1000ms*/
#define DEFAULT_ERR_WAIT_TIME (1000)
int err_wait_time = DEFAULT_ERR_WAIT_TIME;
module_param(err_wait_time, int, S_IRUGO|S_IWUSR);

static uint32 rxbuf_count = SDIO_DEFAULT_RXBUFS_COUNT;
module_param(rxbuf_count, uint, S_IRUGO);

extern uint32 sdio_max_dtr;
module_param(sdio_max_dtr, uint, S_IRUGO);
int32  exception_mask=TRUE;
int32  exception_enable = FALSE;
module_param(exception_enable, int, S_IRUGO|S_IWUSR);

/*Try to force load wifi.ko*/
static uint32 wifi_force_load = 0;
module_param(wifi_force_load, uint, S_IRUGO|S_IWUSR);

struct platform_device * hi110x_of_wifi_dev=NULL;

STATIC hsdio_io_cfg io_cfg_t;

#ifdef WLAN_PERFORM_DEBUG
extern struct wlan_perform_cfg_impl *g_perform_cfg_info;
#endif

STATIC void hi110x_sdio_set_state(struct iodevice *io_dev, uint8 state);
STATIC uint8 hi110x_sdio_get_state(struct iodevice *io_dev);
STATIC int32 hi110x_bus_resume(struct iodevice *io_dev);

STATIC int32 hi110x_bus_transfer_sg(struct iodevice *, struct scatterlist *,
                                    uint32 , uint32);
STATIC int32 hi110x_sdio_rw_sg(struct hi110x_sdio *, int32 ,
                        uint32 , struct scatterlist *,
                        uint32 , uint32 );

void hi110x_sdio_int_stat_dump(struct hi110x_sdio *hi_sdio, hwifi_union_log* log)
{
    int ret;
    uint8 int_mask;
    uint8 msg_mask;
    struct sdio_func  *func;
    if(!hi_sdio || in_interrupt() || in_atomic())
    {
        return;
    }
    func = hi_sdio->func;
    sdio_claim_host(func);
    if(!hi110x_sdio_get_state(&hi_sdio->io_dev))
    {
        HWIFI_UNION_PRINT(log,"Sdio channel stopped!Can not read sdio int stat.");
        sdio_release_host(func);
        return ;
    }
    int_mask = sdio_readb(func, REG_FUNC1_INT_STATUS, &ret);
    if(ret)
    {
        HWIFI_UNION_PRINT(log,"Failed to read sdio func1 interrupt status!ret=%d\n", ret);
        sdio_release_host(func);
        return;
    }
    if(!int_mask)
    {
        HWIFI_UNION_PRINT(log,"No sdio interrupt detect. \n");
        sdio_release_host(func);
        return;
    }
    HWIFI_UNION_PRINT(log,"\n");
    HWIFI_UNION_PRINT(log,"Sdio interrupt stat: 0x%2X \n",int_mask);
    if(int_mask & HI110X_FUNC1_INT_MFARM)
    {
        msg_mask = sdio_readb(func, REG_FUNC1_MSG_FROM_DEV, &ret);
        if(ret)
        {
            HWIFI_UNION_PRINT(log,"Failed to read sdio msg interrupt status!ret=%d\n", ret);
            sdio_release_host(func);
            return;
        }
        HWIFI_UNION_PRINT(log,"Sdio message stat: 0x%2X \n", msg_mask);
    }
    HWIFI_UNION_PRINT(log,"\n");
    sdio_release_host(func);

}


STATIC void hi110x_heartbeat_timeout_isr(struct hi110x_sdio *hi_sdio)
{
    hcc_state state;

    state = hcc_get_tx_queue_status(hi_sdio->hi110x_dev->hcc);
    HWIFI_ERROR("Heartbeat timeout for %u seconds when wifi %s, export a exception",
                hi_sdio->heartbeat.cur_timeout,
                hcc_get_chan_stat_str(state)
                );
    /*reboot wlan device*/
    hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);
}
STATIC int32 check_pktype_is_beatheart(struct hi110x_sdio *hi_sdio, uint8 *buf)
{
    uint8            *ptr;
    uint8             type;
    uint8             aggr_info;

    /* get pkt type , get pkt aggr information*/
    ptr  = buf;
    type = *(ptr + 1);
    aggr_info = *(ptr + 2);

    /*hi110x_heartbeat_wdt_timer(hi_sdio, aggr_info, type);*/
    if (!HCC_HAVE_AGGRINFO(aggr_info) && (HCC_MAIN_TYPE(type) == HCC_TYPE_BEATHEART))
    {
        HWIFI_DEBUG("Host receive heartbeat pkt");
        return TRUE;
    }

    return FALSE;
}


STATIC int32 hi110x_sdio_rw_buf(struct hi110x_sdio *hi_sdio, int32 rw, uint32 addr, uint8 *buf, uint32 rw_sz)
{
    struct sdio_func *func = hi_sdio->func;
    int32             ret  = SUCC;

    /* padding len of buf has been assure when alloc */
    rw_sz  = SDIO_ALIGN_4_OR_BLK(rw_sz);

    sdio_claim_host(func);
    if (SDIO_READ == rw)
    {
       ret = sdio_readsb(func, buf, addr, rw_sz);
    }
    else if(SDIO_WRITE == rw)
    {
        ret = sdio_writesb(func, addr, buf, rw_sz);
    }

    sdio_release_host(func);

    /* 统计吞吐量，根据吞吐量适配CPU工作频率 */
    ret = hi110x_fitcpu_for_throughput(hi_sdio, rw_sz, 1);
    return ret;
}

STATIC void hisdio_reset_skb(struct sk_buff* skb)
{
    HWIFI_BUG_ON(!skb);
    skb->len = 0;
    skb_reset_tail_pointer(skb);
}

struct sk_buff* hisdio_reserve_skb_alloc(struct hi110x_sdio *hi_sdio, uint32 len)
{
    struct  sk_buff* skb;
    HWIFI_BUG_ON(!hi_sdio);
    skb = skb_dequeue(&hi_sdio->rx_reserve_skb_list);
    if(likely(skb))
    {
        HWIFI_BUG_ON(skb_tailroom(skb) < len);
        hisdio_reset_skb(skb);
    }
    HWIFI_DEBUG("alloc reserved skb [qlen==>%u]",hi_sdio->rx_reserve_skb_list.qlen);
    return skb;
}

void hisdio_reserve_skb_free(struct hi110x_sdio *hi_sdio, struct sk_buff* skb)
{
    HWIFI_BUG_ON(!hi_sdio);
    if(HWIFI_WARN_ON(!skb))
    {
        return;
    }
    hisdio_reset_skb(skb);
    skb_queue_tail(&hi_sdio->rx_reserve_skb_list, skb);
    HWIFI_DEBUG("after free reserved skb [qlen==>%u]",hi_sdio->rx_reserve_skb_list.qlen);
}

void hisdio_reserve_skb_free_list_init(struct hi110x_sdio *hi_sdio, struct sk_buff_head* list)
{
    struct  sk_buff* skb;
    HWIFI_BUG_ON(!hi_sdio);
    HWIFI_BUG_ON(!list);
    if(HWIFI_WARN(skb_queue_empty(list),"free empty list"))
    {
        return;
    }
    while(NULL != (skb = skb_dequeue(list)))
    {
        hisdio_reserve_skb_free(hi_sdio, skb);
    }
    skb_queue_head_init(list);
    HWIFI_DEBUG("after free reserved skb [qlen==>%u]",hi_sdio->rx_reserve_skb_list.qlen);
}

STATIC int hisdio_skb_reserve_mem_init(struct hi110x_sdio *hi_sdio)
{
    int i;
    struct sk_buff* skb;
    struct sk_buff_head  *list;
    HWIFI_BUG_ON(!hi_sdio);
    list = &hi_sdio->rx_reserve_skb_list;
    skb_queue_head_init(list);

    for(i = 0; i < HCC_MAX_ASSEMBLE_NUM*2 ; i++)
    {
        skb = dev_alloc_skb(ALIGN(SINGLE_PACKET_MAX_SIZE, SDIO_BLOCK_SIZE));
        if(skb)
        {
            skb_queue_tail(list,skb);
        }
        else
        {
            goto failed;
        }
    }

    return 0;
failed:
    skb_queue_purge(list);
    return -1;
}

STATIC void hisdio_skb_reserve_mem_exit(struct hi110x_sdio *hi_sdio)
{
    HWIFI_BUG_ON(!hi_sdio);
    skb_queue_purge(&hi_sdio->rx_reserve_skb_list);
    skb_queue_head_init(&hi_sdio->rx_reserve_skb_list);
}


STATIC void hi110x_sdio_build_sg_from_sklist(
            struct hi110x_sdio *hi_sdio,
            uint32* rw_sz
            )
{
    int idx = 0;
    uint32 sum_len = 0;
    struct sk_buff         *skb = NULL;
    struct scatterlist *rx_sg = hi_sdio->rx_sg;
    struct sk_buff_head *next_pkt_sk_list = &hi_sdio->next_pkt_sk_list;
    /*internal call function, needn't add assert.*/

    HWIFI_BUG_ON(skb_queue_empty(next_pkt_sk_list));

    sg_init_table(rx_sg, (HCC_SDIO_SG_ASSEMBLE_NUM));

    skb_queue_walk(next_pkt_sk_list, skb)
    {
        sg_set_buf(&rx_sg[idx], skb->data, skb->len);
        sum_len += skb->len;
        idx++;
    }
    hi_sdio->rx_sg_len = idx;
    HWIFI_BUG_ON(0 == idx);
    HWIFI_BUG_ON(idx > (HCC_MAX_ASSEMBLE_NUM));

    *rw_sz = sum_len;
    HWIFI_DEBUG("Build Sdio Rx scatter list [list num:%u], total bytes:%u", hi_sdio->rx_sg_len, sum_len);
    return;
}
#ifdef WLAN_PERFORM_DEBUG
void  hi110x_sdio_put_rxbuf(struct hi110x_sdio *hi_sdio, struct rxbuf_node *node)
#else
STATIC inline void  hi110x_sdio_put_rxbuf(struct hi110x_sdio *hi_sdio, struct rxbuf_node *node)
#endif
{
    spin_lock(&hi_sdio->rxbuf_pool_lock);

    node->used = 0;
    node->data_len = 0;
    hi_sdio->free_rxbuf_count++;

    spin_unlock(&hi_sdio->rxbuf_pool_lock);

    wake_up(&hi_sdio->rxbuf_waitq);
}



STATIC inline int32 hi110x_sdio_check_rx_len(struct hi110x_sdio  *hi_sdio, uint32 len, uint32 check0)
{
    if ((len > hi_sdio->rxbuf_size)
        || ((len > SDIO_BLOCK_SIZE) && (len & (SDIO_BLOCK_SIZE-1)))
        || (check0 && (0 == len)))
    {
        HWIFI_ERROR("Sdio rx len exceed max size or not align. xfer_cnt = %d", len);
        return -EFAIL;
    }

    return SUCC;
}

STATIC inline void hi110x_sdio_flow_ctrl_sched(struct hi110x_sdio *hi_sdio)
{
    /*need't pass the  hcc_queue_type when rx, useless*/
    hcc_queues_flow_ctrl_handle(hi_sdio->hi110x_dev->hcc, HCC_QUEUE_COUNT, HCC_RX, 0,0);
}
int32 hi110x_sdio_data_sg_irq(struct hi110x_sdio *hi_sdio)
{
    uint32 rw_sz  = 0;
    struct sdio_func   *func;
#ifdef WLAN_PERFORM_DEBUG
    struct rxbuf_node  *rxbuf_node;
#endif
    uint32              xfer_count;
    int32               ret;
    struct sk_buff     *skb;

    func = hi_sdio->func;

    hi110x_sdio_flow_ctrl_sched(hi_sdio);

    /*
     * 1. read xfer bytes(if next_pkt_len>0, bypass this )
     * 2. read data from device
     * 3. schedule work to process receive data
     *
     * */

    xfer_count = hi_sdio->next_pkt_len;
    if (0 == xfer_count)
    {
        xfer_count = sdio_readl(func, REG_FUNC1_XFER_COUNT, &ret);
        if (unlikely(ret < 0))
        {
            hi_sdio->next_pkt_len = 0;
            HWIFI_ERROR("Sdio Read Single Package len failed ret=%d", ret);
            hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);
            return -EFAIL;
        }
        if (hi110x_sdio_check_rx_len(hi_sdio, xfer_count, 1) != SUCC)
        {
            HWIFI_ERROR("Sdio Rx Single Transfer len[%u] invalid", xfer_count);
            return -EFAIL;
        }
        HWIFI_DEBUG("Sdio Rx Single Package Received,len:%u", xfer_count);
        skb = dev_alloc_skb(ALIGN(xfer_count, HCC_PACKET_ALIGN_LEN));
        if(unlikely(!skb))
        {
            HWIFI_INFO("Sdio Rx Single skb alloc failed! Try to use reserved skb.");
            skb = hisdio_reserve_skb_alloc(hi_sdio, ALIGN(xfer_count, HCC_PACKET_ALIGN_LEN));
            if(!skb)
            {
                /*reserved skb mem should can get for ever*/
                HWIFI_BUG_ON(1);
            }
            /*dev skb alloc failed, single pkt used reserved mem, we should drop it.*/
            set_bit(HSDIO_RX_ERROR_TRANS_BUSY, &hi_sdio->rx_skb_err);
        }

        skb_put(skb, ALIGN(xfer_count, HCC_PACKET_ALIGN_LEN));
        if(unlikely(!skb_queue_empty(&hi_sdio->next_pkt_sk_list)))
        {
            HWIFI_ERROR("next_pkt_sk_list  should be empty, but now[remain_len:%u]!",
                         hi_sdio->next_pkt_sk_list.qlen);
            skb_queue_purge(&hi_sdio->next_pkt_sk_list);
        }
        __skb_queue_tail(&hi_sdio->next_pkt_sk_list, skb);

    }
    else
    {
        if(unlikely(test_bit(HSDIO_RX_ERROR_NEXT_PKT_BUSY,&hi_sdio->rx_skb_err)))
        {
            /*This assem transfer skb used reserved mem we should drop it.*/
            set_bit(HSDIO_RX_ERROR_TRANS_BUSY, &hi_sdio->rx_skb_err);
        }
        HWIFI_DEBUG("Sdio Rx Assemable Package Received,len:%u", xfer_count);
        if(HWIFI_DEBUG_CONDTION())
        {
            /*if we enable wifi debug mode,
              read length from sdio controller every time
              and check with assem info.*/
            uint32 sdio_count = sdio_readl(func, REG_FUNC1_XFER_COUNT, &ret);
            if (unlikely(ret < 0))
            {
              hi_sdio->next_pkt_len = 0;
              HWIFI_ERROR("Sdio Read xfercount reg,ret=%d,trans len should be %u"
                         ,ret,SDIO_ALIGN_4_OR_BLK(hi_sdio->next_pkt_len));
              hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);
              return -EFAIL;
            }

            HWIFI_WARN((SDIO_ALIGN_4_OR_BLK(hi_sdio->next_pkt_len) != sdio_count),
                    "Device request transfer %u bytes,but last assem info request %u"
                   ,sdio_count,SDIO_ALIGN_4_OR_BLK(hi_sdio->next_pkt_len));
            HWIFI_DEBUG("Device request transfer %u bytes, last assem info request %u"
                        ,sdio_count,SDIO_ALIGN_4_OR_BLK(hi_sdio->next_pkt_len));
        }
    }

    /* beacuse get buf may cost lot of time, so release bus first */
    sdio_release_host(func);

    hi110x_sdio_build_sg_from_sklist(hi_sdio, &rw_sz);

    ret = hi110x_sdio_rw_sg(hi_sdio,SDIO_READ, REG_FUNC1_FIFO,
                       hi_sdio->rx_sg, hi_sdio->rx_sg_len, rw_sz);
    if(unlikely(ret))
    {
        HWIFI_WARNING("Failed to read sdio![rw_sz=%u][ret=%d]\n", rw_sz, ret);
        sdio_claim_host(func);
        hi_sdio->next_pkt_len = 0;
        skb_queue_purge(&hi_sdio->next_pkt_sk_list);
        return -EFAIL;
    }
    else
    {
        /*If we have rx data transfer we thought device alive*/
        hisdio_heartbeat_timer_update(hi_sdio, hi_sdio->heartbeat.work_timeout);
    }

    ret = check_pktype_is_beatheart(hi_sdio, sg_virt(&hi_sdio->rx_sg[0]));
    if(g_powermgmt_switch && (ret != TRUE))
    {
        HWIFI_DEBUG("Sdio Rx request feed watchdog...");
        hi110x_pm_feed_wdg(hi_sdio->hi110x_dev->pm_info);
    }

    hi_sdio->data_rx_count++;

    /*build next package skb buffer list. and dispose the skb data.*/
    ret = hcc_get_nextpkt_skb_info(hi_sdio, &hi_sdio->next_pkt_sk_list);
    if(ret)
    {
        HWIFI_WARNING("Failed to get next package info!next_pkt_len=%u len=%u", hi_sdio->next_pkt_len, xfer_count);
        sdio_claim_host(func);
        return -EFAIL;
    }

    HWIFI_DEBUG("sdio rx. next_pkt_len=%u len=%u", hi_sdio->next_pkt_len, xfer_count);

#ifdef WLAN_PERFORM_DEBUG
    if ((g_perform_cfg_info->trigger_mode) && ((Hi1101_RX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
    {
        hwifi_perform_rx_sdio_statistic(hi_sdio->hi110x_dev, rxbuf_node->data_len);
    }
    if (g_perform_cfg_info->trigger_mode && g_perform_cfg_info->sdio_bypass && ((Hi1101_RX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
    {
        hwifi_perform_rx_sdio_bypass(hi_sdio, rxbuf_node);
        return SUCC;
    }
    else
#endif
    {
#ifdef WLAN_PERFORM_OPT
        if (intr_mode)
        {
            /*激活线程*/
            if(!hi_sdio->hi110x_dev->hcc->sys_hcc_rx_opt_enable)
                up(&hi_sdio->sdio_rx_dispose_sema);
        }
        else
#endif
        {
            if(!HWIFI_DEBUG_CONDTION())
                queue_work(hi_sdio->rx_process_wq, &hi_sdio->isr_work);
        }
        sdio_claim_host(func);
        return SUCC;
    }
}

/*Submit the work who effect the hcc & sdio channel transfer*/
int hi110x_comm_work_submit(struct hi110x_sdio *hi_sdio, struct work_struct* worker)
{
    int ret = 0;
    unsigned long flags = 0;
    struct hcc_handler     *hcc;
    HWIFI_BUG_ON(!hi_sdio);
    if(HWIFI_WARN(!hi_sdio->hi110x_dev,"hi110x_dev null\n")
      || HWIFI_WARN(!hi_sdio->hi110x_dev->hcc, "hcc null\n"))
    {
        HWIFI_WARNING("Submit %pF worker Failed!", (void*)worker->func);
        return -EBUSY;
    }
    hcc   = hi_sdio->hi110x_dev->hcc;
    spin_lock_irqsave(&hcc->hcc_comm_wq.lock, flags);

    /*FIXME? maybe we should move these two functions  upon*/
    hisdio_heartbeat_timer_stop(hi_sdio);
    hi110x_pm_stop_wdg(hi_sdio->hi110x_dev->pm_info);

    if(work_busy(worker))
    {
        /*If comm worker is processing,
          we need't submit again*/
        HWIFI_WARNING("WiFi %pF Worker is Processing...need't submit",
                       (void*)worker->func);
        ret = -EBUSY;
        goto done;
    }
    else
    {
        HWIFI_INFO("WiFi %pF Worker Submit",(void*)worker->func);
        queue_work(hcc->hcc_comm_wq.wq, worker);
    }
done:
    spin_unlock_irqrestore(&hcc->hcc_comm_wq.lock, flags);
    return ret;
}

int hsdio_union_log_submit(struct hi110x_sdio *hi_sdio)
{
    return hi110x_comm_work_submit(hi_sdio, hwifi_union_get_worker());
}

/*Sdio Wake Up event Process*/
int hsdio_wakeup_submit(struct hi110x_sdio *hi_sdio, struct work_struct* worker)
{
    return hi110x_comm_work_submit(hi_sdio, worker);
}
/*Sdio Sleep event process */
int hsdio_sleep_submit(struct hi110x_sdio *hi_sdio, struct work_struct* worker)
{
    return hi110x_comm_work_submit(hi_sdio, worker);
}

int32 hi110x_sdio_msg_irq(struct hi110x_sdio *hi_sdio)
{
    struct sdio_func       *func;
    struct hi110x_device   *hi110x_dev;
    uint8                   msg;
    uint8                   sbmsg;
    int32                   ret;

    hi110x_dev = hi_sdio->hi110x_dev;
    func       = hi_sdio->func;

    /* reading interrupt form ARM Gerneral Purpose Register(0x28)  */
    msg = sdio_readb(func, REG_FUNC1_MSG_FROM_DEV, &ret);
    if (ret < 0)
    {
        HWIFI_ERROR("Sdio readb error![ret=%d]",ret);
        hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);
        return -EFAIL;
    }
    HWIFI_DEBUG("Sdio Message:%d",msg);

    if (msg & HI110X_MSG_FLOW_OFF)
    {
        hcc_dev_flowctrl_off(hi110x_dev->hcc, HWIFI_DEV2HOST);
    }
    /* open flow control:sdio stops sending data to device */
    else if (msg & HI110X_MSG_FLOW_ON)
    {
        hcc_dev_flowctrl_on(hi110x_dev->hcc, 0);
    }
    else if (msg & HI110X_MSG_VERIFY_ERROR)
    {
        /* TBD device calibration error */
        hcc_dev_assemble_reset(hi110x_dev->hcc);
    }
    else if (msg & HI110X_MSG_DEVICE_ERROR)
    {
        /* wlan device is error */
        sbmsg = sdio_readb(func, REG_FUNC1_MSG_FROM_DEV+1, &ret);

        if(sbmsg & 1)
        {
            g_dev_err_subcnt ++;
            hi_sdio->dev_err_subcnt ++;
            HWIFI_WARNING("Wlan dev normal exception, %d times,sub type:%u, global times: %d",
                          hi_sdio->dev_err_subcnt,sbmsg,g_dev_err_subcnt);

            if(g_dev_err_subcnt > 1)
            {
                ret = patch_get_cfg(SDIO_CFG_CHECK_FILE,INFO_SDIO_DOWN_PATCH);
                if(ret == -EFAIL)
                {
                    HWIFI_WARNING("Wlan dev normal exception,fail to read sdio_recover_cfg");
                } else {
                    HWIFI_WARNING("Wlan dev normal exception,succ to read sdio_recover_cfg");
                    g_dev_err_subcnt = 0;
                }
            }
            hwifi_exception_force_submit(hi110x_dev->hcc,
                                        "Normal");
        }else{
            hi_sdio->dev_err_cnt++;
            HWIFI_ERROR("Wlan dev error exception, %d times,sub type:%u",
                            hi_sdio->dev_err_cnt,sbmsg);
            hwifi_exception_submit(hi110x_dev->hcc);
        }
    }
    else if (msg & HI110X_MSG_ALLOWSLEEP)
    {
        HWIFI_INFO("Sdio Allow Sleep Message Process Had Moved To Sleep Work");
    }
    else if (msg & HI110X_MSG_FORBIDSLEEP)
    {
        HWIFI_INFO("Sdio Forbid Message Process Had Moved To Sleep Work");
    }
    else if (msg & HI110X_MSG_DISCONN)
    {
        HWIFI_WARNING("Received disconnect event message");
        hwifi_process_disconn_event(hi110x_dev->cfg);
    }
    if ((!(msg & (HI110X_MSG_FLOW_ON | HI110X_MSG_FLOW_OFF))) && (FLOWCTRL_OFF == g_flowctrl_flag))
    {
        HWIFI_WARNING("Force open flowctrl on");
        hcc_dev_flowctrl_on(hi110x_dev->hcc, 0);
    }

    return SUCC;
}


STATIC int32 hi110x_sdio_do_isr(struct hi110x_sdio *hi_sdio)
{
    uint8                   int_mask;
    int32                   ret;
    struct sdio_func       *func;
    struct hi110x_device   *hi110x_dev;

    hi110x_dev = hi_sdio->hi110x_dev;
    func       = hi_sdio->func;
    if(!hi110x_sdio_get_state(&hi_sdio->io_dev))
    {
        if(printk_ratelimit())
            HWIFI_INFO("Sdio channel stopped!");
        schedule();
        return -EFAIL;
    }
    /* read interrupt indicator register */
    int_mask = sdio_readb(func, REG_FUNC1_INT_STATUS, &ret);
    if (unlikely(ret < 0))
    {
        HWIFI_ERROR("Failed to read sdio func1 interrupt status!ret=%d",ret);
        hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);
        return -EFAIL;
    }
    HWIFI_DEBUG("Handle Sdio Interrupt Mask:%d", int_mask);
    if (unlikely(0 == (int_mask & HI110X_FUNC1_INT_MASK)))
    {
        HWIFI_DEBUG("No sdio interrupt occur, unavailable %s interrupt",
                    intr_mode?"gpio":"sdio");
        return -EFAIL;
    }
    hi_sdio->sdio_int_count++;
    /* clear interrupt mask */
    sdio_writeb(func, int_mask, REG_FUNC1_INT_STATUS, &ret);
    if (unlikely(ret < 0))
    {
        HWIFI_ERROR("Faild to clear sdio func1 interrupt!ret=%d",ret);
        hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);
        return -EFAIL;
    }

    if (int_mask & HI110X_FUNC1_INT_RERROR)
    {
        /* TBD:try to read the data again */
        HWIFI_DEBUG("Sdio func1 interrupt Error, try to read the data anyway");
    }

    /* message interrupt, flow control */
    if (int_mask & HI110X_FUNC1_INT_MFARM)
    {
        if (hi110x_sdio_msg_irq(hi_sdio) != SUCC)
        {
            return -EFAIL;
        }
    }

    if (int_mask & HI110X_FUNC1_INT_DREADY)
    {
        return hi110x_sdio_data_sg_irq(hi_sdio);
    }
    HWIFI_DEBUG("Succeed To Handle Sdio Irq\n");
    return SUCC;
}




void hi110x_sdio_rx_skb_pool_list_clear(struct hi110x_sdio  *hi_sdio)
{
    unsigned long flags;
    struct hcc_handler   *hcc;
    rx_skb_node    *node = NULL;

    HWIFI_ASSERT(NULL != hi_sdio);
    HWIFI_ASSERT(NULL != hi_sdio->hi110x_dev);
    HWIFI_ASSERT(NULL != hi_sdio->hi110x_dev->hcc);

    hcc     = hi_sdio->hi110x_dev->hcc;

    spin_lock_irqsave(&hi_sdio->rx_skb_pool_lock, flags);
    while(!list_empty(&hi_sdio->rx_skb_pool_list))
    {
        struct list_head * wait_send = hi_sdio->rx_skb_pool_list.next;
        list_del(wait_send);
        node = container_of(wait_send,rx_skb_node,list);
        skb_queue_purge(&node->rx_head);
        kfree(node);
    }
    spin_unlock_irqrestore(&hi_sdio->rx_skb_pool_lock, flags);
}

#ifdef WLAN_PERFORM_OPT



void hi110x_sdio_dispose_data(struct hi110x_sdio  *hi_sdio)
{
    unsigned long flags;
    struct hcc_handler   *hcc;
    rx_skb_node    *node = NULL;

    HWIFI_ASSERT(NULL != hi_sdio);
    HWIFI_ASSERT(NULL != hi_sdio->hi110x_dev);
    HWIFI_ASSERT(NULL != hi_sdio->hi110x_dev->hcc);

    hcc     = hi_sdio->hi110x_dev->hcc;

    spin_lock_irqsave(&hi_sdio->rx_skb_pool_lock, flags);
    if(likely(!list_empty(&hi_sdio->rx_skb_pool_list)))
    {
        struct list_head * wait_send = hi_sdio->rx_skb_pool_list.next;
        list_del(wait_send);
        spin_unlock_irqrestore(&hi_sdio->rx_skb_pool_lock, flags);
        node = container_of(wait_send,rx_skb_node,list);
        hcc_dispose_skb_sg(hcc, &node->rx_head);
        kfree(node);
        spin_lock_irqsave(&hi_sdio->rx_skb_pool_lock, flags);
    }
    else
    {
        HWIFI_WARNING("nothing to dispose!!");
    }
    spin_unlock_irqrestore(&hi_sdio->rx_skb_pool_lock, flags);
}


STATIC int32 hi110x_sdio_dispose_thread(void *data)
{
    struct hi110x_sdio *hi_sdio;
    hi_sdio = (struct hi110x_sdio *)data;

    HWIFI_ASSERT(hi_sdio != NULL);

    /*if (g_lCpuId != smp_processor_id())
    {
        HWIFI_ERROR("sdio dispose thread is in progress of cpu %d, not %d--------",smp_processor_id(),g_lCpuId);
        return SUCC;
    }*/

    allow_signal(SIGTERM);
    while (likely(!down_interruptible(&hi_sdio->sdio_rx_dispose_sema)))
    {

        if (kthread_should_stop())
        {
            break;
        }
        hi110x_sdio_dispose_data(hi_sdio);

    }

    HWIFI_INFO("Dispose sdio data thread terminated!");
    return SUCC;

}
#endif


void hi110x_sdio_isr_work(struct work_struct *work)
{
    struct hi110x_sdio   *hi_sdio = container_of(work, struct hi110x_sdio, isr_work);
    unsigned long flags;
    struct hcc_handler   *hcc;
    rx_skb_node    *node = NULL;
    struct list_head      rxbuf_list_temp;

    HWIFI_ASSERT(NULL != hi_sdio);
    HWIFI_ASSERT(NULL != hi_sdio->hi110x_dev);
    HWIFI_ASSERT(NULL != hi_sdio->hi110x_dev->hcc);

    hcc     = hi_sdio->hi110x_dev->hcc;

    INIT_LIST_HEAD(&rxbuf_list_temp);

    spin_lock_irqsave(&hi_sdio->rx_skb_pool_lock, flags);
    while(!list_empty(&hi_sdio->rx_skb_pool_list))
    {
        struct list_head * wait_send = hi_sdio->rx_skb_pool_list.next;
        list_del(wait_send);
        spin_unlock_irqrestore(&hi_sdio->rx_skb_pool_lock, flags);
        node = container_of(wait_send,rx_skb_node,list);
        hcc_dispose_skb_sg(hcc, &node->rx_head);
        kfree(node);
        spin_lock_irqsave(&hi_sdio->rx_skb_pool_lock, flags);
    }
    spin_unlock_irqrestore(&hi_sdio->rx_skb_pool_lock, flags);
}


void hi110x_sdio_isr(struct sdio_func *func)
{
    struct hi110x_sdio     *hi_sdio;
    int                     ret;
    uint32                  weight = SDIO_MAX_CONTINUS_RX_COUNT;

    HWIFI_ASSERT(NULL != func);

    hi_sdio = sdio_get_drvdata(func);
    if (NULL == hi_sdio)
    {
        HWIFI_ERROR("hi110x_sdio is NULL");
        return;
    }

    do
    {
        sdio_release_host(hi_sdio->func);
        hsdio_rx_transfer_lock(hi_sdio);
        sdio_claim_host(hi_sdio->func);
        ret = hi110x_sdio_do_isr(hi_sdio);
        sdio_release_host(hi_sdio->func);
        hsdio_rx_transfer_unlock(hi_sdio);
        sdio_claim_host(hi_sdio->func);
    }while ((SUCC == ret) && (--weight > 0));
}


void hi110x_sdio_wakeup_work(struct work_struct *worker)
{

    int ret = 0;
    struct hcc_handler     *hcc;
    struct hi110x_sdio* hi_sdio;
    HWIFI_BUG_ON(!worker);
    hi_sdio = container_of(worker, struct hi110x_sdio, wakeup_work);
    if(HWIFI_WARN(!hi_sdio->hi110x_dev,"hi110x_dev null\n")
       || HWIFI_WARN(!hi_sdio->hi110x_dev->hcc, "hcc null\n"))
    {
        hi110x_wake_unlock(hi_sdio->hi110x_dev->pm_info);
        return;
    }
    hcc = hi_sdio->hi110x_dev->hcc;
    hcc_transfer_lock(hcc);
    if(hcc_running == hcc_get_tx_queue_status(hcc))
    {
        HWIFI_INFO("WiFi Sdio Had Already Wake up by Hcc Tx, need't Wake Again!");
        hcc_transfer_unlock(hcc);

        hi110x_wake_unlock(hi_sdio->hi110x_dev->pm_info);

        return;
    }
    HWIFI_INFO("Wifi device  wake up host...");
    ret = hi110x_pm_dev_wakeup();
    if(ret)
    {
        HWIFI_ERROR("Gpio interrupt sdio reinit failed,Try to Transfer Anyway ret=%d", ret);
    }else{
            hi_sdio->hi110x_dev->pm_info->pm_stat.dev_host_wakeup++;
    }

    if (NULL != hi_sdio->hi110x_dev->pm_info->get_deepsleep_state)
    {
        /* 设置网卡状态为唤醒状态 */
        HWIFI_INFO("Set Wlan To Wakeup State");
        hi_sdio->hi110x_dev->pm_info->get_deepsleep_state(NODEEPSLEEP_STATE);
    }
    hcc_transfer_unlock(hcc);
}
STATIC int hi110x_wait_sleepmsg(struct hi110x_sdio  *hi_sdio)
{
    int ret;
    uint8  int_mask, msg_mask;
    struct sdio_func  *func;
    unsigned long timeout_jiffies;
    HWIFI_BUG_ON(!hi_sdio);
    func = hi_sdio->func;
    sdio_claim_host(func);
    /*1 second timeout,*/
    timeout_jiffies = jiffies + msecs_to_jiffies(HWIFI_SLEEP_MSG_WAIT_TIMEOUT);
    while(1)
    {
        if(time_after(jiffies, timeout_jiffies))
        {
            /*timeout*/
            HWIFI_ERROR("Device did not reply the sleep respone msg for %d ms!",
                        HWIFI_SLEEP_MSG_WAIT_TIMEOUT);
            /*hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);*/
            ret = -ETIMEDOUT;
            break;
        }
        ret = SUCC;
        /*Wait for sdio msg*/
        msg_mask = 0;
        int_mask = sdio_readb(func, REG_FUNC1_INT_STATUS, &ret);
        if (unlikely(ret < 0))
        {
            HWIFI_ERROR("Failed to read sdio func1 interrupt status!ret=%d",ret);
            ret = -EFAIL;
            break;
        }
        if(int_mask)
        {
            /*We Just Only Clear message interrupt, never should clear data interrupt!!
            FIXME? There is a risk when other such as flow message came! z00262551*/
            sdio_writeb(func, int_mask &~(HI110X_FUNC1_INT_DREADY | HI110X_FUNC1_INT_RERROR),
                        REG_FUNC1_INT_STATUS, &ret);
            if (unlikely(ret < 0))
            {
                HWIFI_ERROR("Faild to clear sdio func1 interrupt!ret=%d",ret);
                ret = -EFAIL;
                break;
            }

            if (int_mask & HI110X_FUNC1_INT_MFARM)
            {
                /*msg came*/
                /*read interrupt indicator register */
                msg_mask = sdio_readb(func, REG_FUNC1_MSG_FROM_DEV, &ret);
                HWIFI_INFO("int mask 0x%2X, msg mask 0x%2X", int_mask, msg_mask);
                if(msg_mask & HI110X_MSG_ALLOWSLEEP)
                {
                    HWIFI_INFO("Device allow host sleep msg come, check whether we can pull down gpio");
                    if(int_mask & HI110X_FUNC1_INT_DREADY)
                    {
                        /*Device send allow sleep msg, but request sdio read transfer at the same time,
                          this is a error,
                          Device g_ucMutexTx should be ON, forbid msg should came,
                          Host can not sleep now, this maybe cause device error*/
                        ret = -EFAIL;
                        HWIFI_ERROR("Fatal Device Error, Deivce should never allow host sleep when request sdio transfer.");
                    }
                    else
                    {
                        HWIFI_INFO("Check done, Sleep Process over, prepare to pull down device gpio");
                        ret = SUCC;
                    }

                    break;
                }
                else if(msg_mask & HI110X_MSG_FORBIDSLEEP)
                {
                    /*Deivce forbid Host sleep, maybe device request sdio read transfer,
                      we can't clear the data interrupt*/
                    HWIFI_INFO("Deivce can't sleep now, WiFi keep working..");
                    ret = PM_FORBID_SLEEP;
                    break;
                }
            }
        }
        schedule();
    }
    sdio_release_host(func);
    return ret;
}

void hi110x_sdio_sleep_work(struct work_struct *worker)
{
    int ret = SUCC;
    int sdio_state = TRUE;
    hcc_state state;
    struct hcc_handler     *hcc;
    struct hi110x_sdio* hi_sdio;
    HWIFI_BUG_ON(!worker);
    hi_sdio = container_of(worker, struct hi110x_sdio, sleep_work);

    if(HWIFI_WARN(!hi_sdio->hi110x_dev,"hi110x_dev null\n")
       || HWIFI_WARN(!hi_sdio->hi110x_dev->hcc, "hcc null\n"))
    {
        goto restart_wdt_timer;
    }

    hcc = hi_sdio->hi110x_dev->hcc;

    /* allow Device to sleep, First check tx queue status,
    * if it has data, forbid Device to sleep,
    * else pull down GPIO, allow Device to sleep */
    hcc_transfer_lock(hi_sdio->hi110x_dev->hcc);
    state = hcc_get_tx_queue_status(hcc);
    sdio_state = hi110x_sdio_get_state(hcc->io_dev);
    if(hcc_running != state || TRUE != sdio_state )
    {
        HWIFI_WARNING("Hcc %s, sdio %s, can't sleep.",
                       hcc_get_chan_stat_str(state),
                       sdio_state?"enable":"disabled");
        goto failed_check_state;
    }
    /* modefy timer period for deep sleep */
    /*stop heartbeat timer here.*/
    hisdio_heartbeat_timer_stop(hi_sdio);
    ret = hi110x_sleep_request(hi_sdio->func);
    if (PM_FORBID_SLEEP == ret)
    {
        HWIFI_INFO("Did not sleep, continue work.");
        goto failed_send_msg;
    }
    else if(ret != SUCC){
        HWIFI_ERROR("Sdio error occur!");
        goto failed_exception;
    }

    /*waiting for allow or forbid msg*/
    ret = hi110x_wait_sleepmsg(hi_sdio);
    if (PM_FORBID_SLEEP == ret)
    {
        HWIFI_INFO("Did not sleep, continue work.");
        goto failed_wait_sleepmsg;
    }
    else if(ret != SUCC){
        HWIFI_ERROR("Sdio error occur!");
        goto failed_exception;
    }

    hi110x_wake_unlock(hi_sdio->hi110x_dev->pm_info);

    HWIFI_INFO("Hcc Tx queue empty,Allow Sleep.");
    hi110x_pm_allow_sleep(hi_sdio->hi110x_dev->pm_info);
    g_pm_info->allowsleep_flag = FALSE;
    hcc_transfer_unlock(hi_sdio->hi110x_dev->hcc);

    /*If sleep actually, we should start deepsleep hb timer.*/
    hisdio_heartbeat_timer_start(hi_sdio, hi_sdio->heartbeat.sleep_timeout);

    return;

failed_exception:
    g_pm_info->allowsleep_flag = FALSE;
    hcc_transfer_unlock(hi_sdio->hi110x_dev->hcc);
    hi110x_wake_unlock(hi_sdio->hi110x_dev->pm_info);
    if(!(hi_sdio->hi110x_dev->pm_info->pm_stat.sleep_fail_count++))
        hi_sdio->hi110x_dev->pm_info->pm_stat.sleep_fail_cpu_time =
        cpu_clock(UINT_MAX);
    hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);
    return;

failed_wait_sleepmsg:
failed_send_msg:
    g_pm_info->allowsleep_flag = FALSE;
    hcc_sched_transfer(hcc);
    /*when send msg failed, wifi is still working, so restart heartbeat and wdt timer*/
    hisdio_heartbeat_timer_start(hi_sdio, hi_sdio->heartbeat.work_timeout);
failed_check_state:
    hcc_transfer_unlock(hi_sdio->hi110x_dev->hcc);
restart_wdt_timer:
    /*restart wdt timer,when failed*/
    hi110x_pm_start_wdg(hi_sdio->hi110x_dev->pm_info);
    hi110x_wake_unlock(hi_sdio->hi110x_dev->pm_info);

    return;
}


STATIC int32 hi110x_gpio_rxdata_thread(void *data)
{
    struct hcc_handler             *hcc =NULL;
    struct hi110x_sdio      *hi_sdio;
    struct sdio_func   *func;
    struct sched_param       param;

    hi_sdio = (struct hi110x_sdio *)data;
    HWIFI_ASSERT(hi_sdio != NULL);

    func        = hi_sdio->func;
    HWIFI_BUG_ON(!hi_sdio->hi110x_dev);
    HWIFI_BUG_ON(!hi_sdio->hi110x_dev->hcc);
    hcc = hi_sdio->hi110x_dev->hcc;

    printk(KERN_DEBUG"Hisi Wifi sched ctrl rx thread high speed\n");
    param.sched_priority = 0;
    hi110x_set_thread_property(current,SCHED_NORMAL,
                                &param,
                                 HI110X_SDIO_RX_THREAD_NICE);


    allow_signal(SIGTERM);
    while (likely(!down_interruptible(&hi_sdio->gpio_rx_sema)))
    {

        /* smp_read_barrier_depends(); */
        if (kthread_should_stop())
        {
            /* HWIFI_DEBUG("exit thread");*/
            break;
        }

        /* start to read GPIO interrupt */
        hsdio_rx_transfer_lock(hi_sdio);
        sdio_claim_host(func);
        hi110x_sdio_do_isr(hi_sdio);
        sdio_release_host(hi_sdio->func);
        hsdio_rx_transfer_unlock(hi_sdio);
    }

    HWIFI_DEBUG("gpio rx data is terminated");
    return SUCC;
}
void hi110x_gpio_intr_enable(int8 enable)
{
    unsigned long flags;

    spin_lock_irqsave(&g_pm_info->irq_lock, flags);
    if (enable)
    {
        enable_irq(g_wlan_info->wlan_irq);
    }
    else
    {
        disable_irq_nosync(g_wlan_info->wlan_irq);
    }
    spin_unlock_irqrestore(&g_pm_info->irq_lock, flags);
}



STATIC irqreturn_t wlan_gpio_irq(int irq, void *dev_id)
{
    struct hi110x_sdio *hi_sdio;
    uint8               state;

    hi_sdio = (struct hi110x_sdio *)dev_id;
    if(NULL == hi_sdio)
    {
        HWIFI_WARNING("GPIO interrupt function param is NULL");
        return IRQ_HANDLED;
    }

    g_pm_info->allowsleep_flag = FALSE;
    state = g_wlan_info->wlan_gpio_state_get();
    if(TRUE == state)
    {
        hi_sdio->gpio_int_count++;
        HWIFI_DEBUG("Sdio Rx Data Interrupt.");
        up(&hi_sdio->gpio_rx_sema);

    }
    else
    {
        HWIFI_INFO("Sdio Wakeup Interrrupt");

        /* hold wake lock when wakeup by device */
        hi110x_wake_lock(hi_sdio->hi110x_dev->pm_info);
        if(hsdio_wakeup_submit(hi_sdio, &hi_sdio->wakeup_work))
        {
             hi110x_wake_unlock(hi_sdio->hi110x_dev->pm_info);
        }
    }

    return IRQ_HANDLED;
}
int32 hi110x_register_gpio_intr(struct hi110x_sdio *hi_sdio)
{
    int32 ret = SUCC;

    HWIFI_INFO("Wlan GPIO irq = %u", g_wlan_info->wlan_irq);

    /* determine irq is wakeup interrupt or data irq */
    if(NULL == g_wlan_info->wlan_gpio_state_get)
    {
        HWIFI_WARNING("gpio get state function is NULL");
        return -EFAIL;
    }

    /* create thread for gpio rx data in interrupt handler*/
    hi_sdio->gpio_rx_tsk = hi110x_thread_create(hi110x_gpio_rxdata_thread,
                            (struct hi110x_sdio *)hi_sdio,
                            &hi_sdio->gpio_rx_sema,
                            "hi110x_gpio_rx_data",
                            HI110X_SDIO_RX_THREAD_POLICY,
                            HI110X_GPIO_RXDATA_THREAD_PRIORITY,g_lCpuId);
    if (NULL == hi_sdio->gpio_rx_tsk)
    {
        HWIFI_ERROR("Failed to create rx data thread");
        return -EFAIL;
    }


   ret = request_irq(g_wlan_info->wlan_irq, wlan_gpio_irq, IRQF_NO_SUSPEND | IRQF_TRIGGER_RISING | IRQF_DISABLED, "hi110x_gpio_intr", hi_sdio);
   if (ret < 0)
   {
       HWIFI_ERROR("Failed to request wlan irq");
       hi110x_thread_stop(hi_sdio->gpio_rx_tsk, &hi_sdio->gpio_rx_sema);
       hi_sdio->gpio_rx_tsk = NULL;

       return ret;
   }

   HWIFI_INFO("Success to register sdio gpio intr");

   return SUCC;

}


void hi110x_unregister_gpio_intr(struct hi110x_sdio *hi_sdio)
{
    /* disable wlan irq */
    hi110x_gpio_intr_enable(FALSE);
    /* free irq when sdio driver deinit */
    free_irq(g_wlan_info->wlan_irq, hi_sdio);
    hi110x_thread_stop(hi_sdio->gpio_rx_tsk, &hi_sdio->gpio_rx_sema);
    hi_sdio->gpio_rx_tsk = NULL;

}



int32 hi110x_sdio_interrupt_register(struct hi110x_sdio *hi_sdio)
{
    int32 ret;

    if (intr_mode)
    {
        /* use GPIO interrupt for sdio data interrupt */
        ret = hi110x_register_gpio_intr(hi_sdio);
        if (ret < 0)
        {
            HWIFI_ERROR("Failed to register gpio interrupt");
            return ret;
        }
     }
     else
     {
        sdio_claim_host(hi_sdio->func);
        /* use sdio bus line data1 for sdio data interrupt */
        ret = sdio_claim_irq(hi_sdio->func, hi110x_sdio_isr);
        HWIFI_INFO("Notice Hi1101 register sdio interrupt");
        if (ret < 0)
        {
            HWIFI_ERROR("Failed to register sdio interrupt");
            sdio_release_host(hi_sdio->func);

            return -EFAIL;
        }
        sdio_release_host(hi_sdio->func);
      }

    return SUCC;

}


void hi110x_sdio_interrupt_unregister(struct hi110x_sdio *hi_sdio)
{
    if (intr_mode)
    {
        /* use GPIO interrupt for sdio data interrupt */
        hi110x_unregister_gpio_intr(hi_sdio);
    }
    else
    {
        /* use sdio bus line data1 for sdio data interrupt */
        sdio_release_irq(hi_sdio->func);
    }
}

static void hwifi_init_status(struct hi110x_sdio *hi_sdio)
{
    /* 初始化完成后,下电wlan,同时释放所有wlan相关的wake_lock锁 */
    hi110x_gpio_intr_enable(0);
    hi110x_sdio_set_state(&hi_sdio->io_dev,FALSE);
    hsdio_sdio_iocfg(PINCTRL_STATE_IDLE);
    wlan_power_off();
    hi110x_wake_unlock(hi_sdio->hi110x_dev->pm_info);
    hi110x_wakelocks_release_detect(hi_sdio->hi110x_dev->pm_info);
}
int32 hi110x_sdio_dev_init(struct hi110x_sdio *hi_sdio)
{
    struct sdio_func   *func;
    int32               ret;

    /* Start of zhangwei 64406 2013-03-25 B295 编程规范美化 */
    HWIFI_ASSERT(NULL != hi_sdio);
    HWIFI_ASSERT(NULL != hi_sdio->hi110x_dev);
    /* End of zhangwei 64406 2013-03-25 B295 编程规范美化 */

    func = hi_sdio->func;
    sdio_claim_host(func);

    ret = sdio_enable_func(func);
    if (ret < 0)
    {
        HWIFI_ERROR("Failed to enable sdio function!");
        sdio_release_host(func);

        return -EFAIL;
    }

    ret = sdio_set_block_size(func, SDIO_BLOCK_SIZE);
    if (ret < 0)
    {
        HWIFI_ERROR("Failed to set sdio blk size!");
        sdio_disable_func(func);
        sdio_release_host(func);

        return -EFAIL;
    }

#ifdef WLAN_POWER_MANAGEMENT
    /*store sdio func1 */
    g_pm_info->sdio_func = func;
#endif

    /* init heartbeat timer, when can not receive heartbeat pkt, TBD: reset HI1101 */
    init_timer(&hi_sdio->heartbeat.heartbeat_timer);
    hi_sdio->heartbeat.heartbeat_timer.data     = (unsigned long)hi_sdio;
    hi_sdio->heartbeat.heartbeat_timer.function = (void *)hi110x_heartbeat_timeout_isr;

    /* before enable sdio function 1, clear its interrupt flag, no matter it exist or not */
    sdio_writeb(func, HI110X_FUNC1_INT_MASK, REG_FUNC1_INT_STATUS, &ret);
    if (ret < 0)
    {
        HWIFI_ERROR("Failed to clear sdio interrupt!");
        del_timer_sync(&hi_sdio->heartbeat.heartbeat_timer);
        sdio_disable_func(func);
        sdio_release_host(func);
        return -EFAIL;
    }

    /*
     * enable four interrupt sources in function 1:
     *      data ready for host to read
     *      read data error
     *      message from arm is available
     *      device has receive message from host
     * */
    sdio_writeb(func, HI110X_FUNC1_INT_MASK, REG_FUNC1_INT_ENABLE, &ret);
    if (ret < 0)
    {
        HWIFI_ERROR("Failed to enable sdio interrupt!");
    del_timer_sync(&hi_sdio->heartbeat.heartbeat_timer);
    sdio_disable_func(func);
        sdio_release_host(func);

        return -EFAIL;
    }

    hi110x_sdio_set_state(&hi_sdio->io_dev,TRUE);
    sdio_release_host(func);

    /* 初始化SDIO总吞吐量统计的相关参数 */
    hwifi_perform_init_rwtotal_statistic();

    HWIFI_INFO("sdio function %d enabled.", func->num);

    if (!hi_sdio->func->card->cccr.multi_block)
    {
        HWIFI_ERROR("Not support muti block transfer! ");
        hwifi_perform_deinit_rwtotal_statistic();
        return -EIO;
    }

    hwifi_init_status(hi_sdio);

    return SUCC;
}
STATIC int hi110x_sdio_dev_deinit(struct hi110x_sdio *hi_sdio)
{
    struct sdio_func   *func;
    int32               ret;

    HWIFI_ASSERT(NULL != hi_sdio && NULL != hi_sdio->hi110x_dev);

    func  = hi_sdio->func;
    HWIFI_ASSERT(NULL != func);

    sdio_claim_host(func);
    hisdio_heartbeat_timer_stop(hi_sdio);
    sdio_writeb(func, 0, REG_FUNC1_INT_ENABLE, &ret);
    hi110x_sdio_interrupt_unregister(hi_sdio);
    ret |= sdio_disable_func(func);
    hi110x_sdio_set_state(&hi_sdio->io_dev, FALSE);
    sdio_release_host(func);

    hwifi_perform_deinit_rwtotal_statistic();
    HWIFI_INFO("sdio device has been disabled");

    return ret;
}


STATIC uint8 hi110x_sdio_get_state(struct iodevice *io_dev)
{
    /*io_dev assert had been done in hwifi_bus.h*/
    struct hi110x_sdio *hi_sdio = bus_to_sdio(io_dev);
    HWIFI_BUG_ON(!hi_sdio);
    HWIFI_BUG_ON(!hi_sdio->hi110x_dev);
    return (uint8)test_bit(HI_STATUS_BUS_READY, &hi_sdio->hi110x_dev->status);
}
STATIC void hi110x_sdio_set_state(struct iodevice *io_dev, uint8 state)
{
    struct hi110x_sdio *hi_sdio;
    HWIFI_BUG_ON(!io_dev);
    hi_sdio = bus_to_sdio(io_dev);
    HWIFI_BUG_ON(!hi_sdio);
    HWIFI_BUG_ON(!hi_sdio->hi110x_dev);

    /*Debug dump a warning when the state is the same as before*/
    if(unlikely(!!state == test_bit(HI_STATUS_BUS_READY, &hi_sdio->hi110x_dev->status)))
    {
        HWIFI_INFO("you set a same state:[%s ==> %s]",
                test_bit(HI_STATUS_BUS_READY, &hi_sdio->hi110x_dev->status)
                ? "bus online" : "bus offline",
                state ? "bus online" : "bus offline");
    }
    HWIFI_INFO("sdio bus state change from %s ==> %s",
            test_bit(HI_STATUS_BUS_READY, &hi_sdio->hi110x_dev->status)
            ? "bus online" : "bus offline",
            state ? "bus online" : "bus offline"
            );
    if(state)
    {
        set_bit(HI_STATUS_BUS_READY, &hi_sdio->hi110x_dev->status);
    }
    else
    {
        clear_bit(HI_STATUS_BUS_READY, &hi_sdio->hi110x_dev->status);
    }
}
static void hi110x_sdio_rx_transfer_info_reset(struct iodevice *io_dev, uint8 locked)
{
    /*Please make sure you had synchronized with wifi device,
    This is every important,make sure sdio slave reset
    the assem info at the same time!!*/
    struct hi110x_sdio *hi_sdio = bus_to_sdio(io_dev);
    HWIFI_BUG_ON(!hi_sdio);
    HWIFI_DEBUG("=========rx transfer info reset==========");

    if(locked)
        hsdio_rx_transfer_lock(hi_sdio);

    hi_sdio->next_pkt_len = 0;
    skb_queue_purge(&hi_sdio->next_pkt_sk_list);
    hi_sdio->rx_skb_err = 0;

    if(locked)
        hsdio_rx_transfer_unlock(hi_sdio);

}


STATIC int32 hi110x_bus_enable(struct iodevice *io_dev)
{
    int32 ret = SUCC;
    struct hi110x_sdio *hi_sdio;
    HWIFI_BUG_ON(!io_dev);
    hi_sdio = bus_to_sdio(io_dev);
    HWIFI_INFO("=======hsdio bus enable=======");
    /*when  enable bus, the probe had enable the bus
      so we must set the bus top first*/
    /*hcc_set_tx_queue_status(hi_sdio->hi110x_dev->hcc, hcc_stop);*/
    /*we should set hcc channel to running when receive the cfg ready msg,
    not here.*/
    hi110x_sdio_powerset_time_record(TRUE);
    ret = hi110x_pm_poweron_reinit();
    HWIFI_WARN(ret, "pm poweron reinit failed ret = %d ", ret);
    return ret;
}


STATIC int32 hwifi_send_shutdown_msg(struct sdio_func *func)
{
    int32       ret  = SUCC;

    HWIFI_INFO("Send msg to notice device to prepare shutdown.");
    sdio_claim_host(func);
    /* set msg to Device for shutdown clear work, when work done, device should enter deadloop wait poweroff */
    sdio_writel(func, HOST2HI110X_PRE_SHUTDOWN_MSG, REG_FUNC1_WRITE_MSG, &ret);
    if (ret < 0)
    {
        HWIFI_WARNING("Failed to send prepare shutdown msg!ret=%d", ret);
        ret = -EFAIL;
    }
    sdio_release_host(func);
    return ret;
}

STATIC int hwifi_wait_dev_shutdown_msg(struct hi110x_sdio  *hi_sdio)
{
    int ret;
    uint8  int_mask;
    u32 msg_mask;
    struct sdio_func  *func;
    unsigned long timeout_jiffies;
    HWIFI_BUG_ON(!hi_sdio);
    func = hi_sdio->func;
    sdio_claim_host(func);
    /*1 second timeout,*/
    timeout_jiffies = jiffies + msecs_to_jiffies(HWIFI_PRE_SHUTDOWN_MSG_TIMEOUT);
    while(1)
    {
        if(time_after(jiffies, timeout_jiffies))
        {
            /*timeout*/
            HWIFI_WARNING("Device did not reply shutdown prepare msg for %d ms!",
                        HWIFI_PRE_SHUTDOWN_MSG_TIMEOUT);
            ret = -ETIMEDOUT;
            break;
        }
        ret = SUCC;
        /*Wait for sdio msg*/
        msg_mask = 0;

        int_mask = sdio_readb(func, REG_FUNC1_INT_STATUS, &ret);
        if (unlikely(ret < 0))
        {
            HWIFI_ERROR("Failed to read sdio func1 interrupt status!ret=%d",ret);
            ret = -EFAIL;
            break;
        }

         sdio_writeb(func, int_mask, REG_FUNC1_INT_STATUS, &ret);
        if (unlikely(ret < 0))
        {
            HWIFI_ERROR("Failed to write sdio func1 interrupt status!ret=%d",ret);
            ret = -EFAIL;
            break;
        }

        if(int_mask)
        {
            sdio_writeb(func, int_mask &~(HI110X_FUNC1_INT_DREADY | HI110X_FUNC1_INT_RERROR),
                        REG_FUNC1_INT_STATUS, &ret);
            if (unlikely(ret < 0))
            {
                HWIFI_ERROR("Faild to clear sdio func1 interrupt!ret=%d",ret);
                ret = -EFAIL;
                break;
            }

            if (int_mask & HI110X_FUNC1_INT_MFARM)
            {
                /*msg came*/
                /*read interrupt indicator register */
                msg_mask = sdio_readl(func, REG_FUNC1_MSG_FROM_DEV, &ret);
                HWIFI_INFO("int mask 0x%2X, msg mask 0x%4X", int_mask, msg_mask);
                if(msg_mask & (HI110X_MSG_DEV_PRE_SHUTDOWN_REPLY))
                {
                    HWIFI_INFO("Device prepare shutdown ok msg come!");
                    break;
                }
            }
        }
        schedule();
    }
    sdio_release_host(func);
    return ret;
}


void  hwifi_notice_device_shutdown(struct hi110x_sdio *  hi_sdio)
{
    HWIFI_BUG_ON(!hi_sdio);
    if(hwifi_send_shutdown_msg(hi_sdio->func))
        return;
    if(hwifi_wait_dev_shutdown_msg(hi_sdio))
    {
        return;
    }
    HWIFI_INFO("Wlan prepare shutdown ok.");
}


STATIC int32 hi110x_bus_disable(struct iodevice *io_dev)
{
    int32 ret = SUCC;
    struct hi110x_sdio *hi_sdio;
    HWIFI_BUG_ON(!io_dev);
    hi_sdio = bus_to_sdio(io_dev);
    HWIFI_BUG_ON(!hi_sdio->func);

    /*disable heartbeat fisrt*/
    hisdio_heartbeat_timer_stop(hi_sdio);

    /*Import: Before we pull down wlan power,
        we should notice device to shutdown clear work*/
    hwifi_notice_device_shutdown(hi_sdio);

    HWIFI_INFO("=======hsdio bus disable=======");
    ret = hi110x_pm_dev_deinit(hi_sdio->func);
    if(ret)
    {
        HWIFI_INFO("pm sdio deinit failed ret = %d \n", ret);
    }
    hi110x_sdio_rx_transfer_info_reset(io_dev, TRANS_UNLOCK);
    hi110x_sdio_powerset_time_record(FALSE);

    hi110x_wlan_power_set(0);
    /*clear rx dipose list*/
    hi110x_sdio_rx_skb_pool_list_clear(hi_sdio);
    hi110x_pm_stat_clear();
    return ret;
}
STATIC int32 hi110x_bus_suspend(struct iodevice *io_dev)
{
    HWIFI_INFO("========hsdio bus suspend==========");
    return SUCC;
}


STATIC int32 hi110x_bus_resume(struct iodevice *io_dev)
{
    HWIFI_INFO("========hsdio bus resume==========");
    return SUCC;
}


STATIC int32 hi110x_bus_transfer(struct iodevice *io_dev, uint8 *buf, uint32 len)
{
    struct hi110x_sdio *hi_sdio;
    struct sdio_func   *func;
    int32               ret;

    HWIFI_ASSERT(NULL != io_dev);

    hi_sdio = bus_to_sdio(io_dev);
    func    = hi_sdio->func;

    /*FIXME? buf covert must same long with the pointer*/
    if (unlikely((0 == len) || (NULL == buf) || ((unsigned long)buf & 0x03)))
    {
        HWIFI_WARNING("Invalid input parameter. buf = 0x%p, len = %u", buf, len);
        return -EFAIL;
    }
    /* check wlan state; if gpio is down, it will reinit sdio; if gpio is up , send data */
    if (!g_wlan_info->wlan_gpio_state_get())
    {
        /* 低电平唤醒，持有wakelock锁 */
        hi110x_wake_lock(hi_sdio->hi110x_dev->pm_info);
    }

    g_wlan_info->check_wlan_state();

#ifdef WLAN_PERFORM_DEBUG
    if ((g_perform_cfg_info->trigger_mode) && ((Hi1101_TX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
    {
        hwifi_perform_tx_sdio_statistic(len);
    }
    if(g_perform_cfg_info->trigger_mode && g_perform_cfg_info->sdio_bypass && ((Hi1101_TX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
    {
        hwifi_perform_tx_sdio_bypass(buf, len);
        ret = SUCC;
    }
    else
#endif
    {
        ret = hi110x_sdio_rw_buf(hi_sdio, SDIO_WRITE, REG_FUNC1_FIFO, buf, len);
        /* when open power manager, start feed watchdog */
        if(g_powermgmt_switch && (ret == SUCC))
        {
            hi110x_pm_feed_wdg(hi_sdio->hi110x_dev->pm_info);
        }
    }

    HWIFI_DEBUG("sdio transfered data, len = %u, ret = %d", len, ret);

    return ret;
}


int32 hi110x_bus_reset_flowctrl(struct iodevice* io_dev)
{
    struct hi110x_sdio *hi_sdio;
    struct sdio_func   *func;
    int32               ret;

    HWIFI_ASSERT(io_dev != NULL);

    hi_sdio = bus_to_sdio(io_dev);
    func    = hi_sdio->func;

    sdio_claim_host(func);
    sdio_writeb(func, HI110X_MSG_RESET_FLOWCTRL, REG_FUNC1_WRITE_MSG, &ret);
    if (ret < 0)
    {
        HWIFI_ERROR("sdio write msg reg fail.");
        sdio_release_host(func);

        return -EFAIL;
    }

    sdio_release_host(func);

    HWIFI_INFO("reset sdio flowctrl.");

    return SUCC;
}


STATIC int32 hi110x_bus_interrupt_pending(struct iodevice *io_dev)
{
    HWIFI_ASSERT(io_dev != NULL);
    return atomic_read(&(bus_to_sdio(io_dev)->intr_pending));
}


STATIC uint32 hi110x_bus_get_max_transfer_size(struct iodevice *io_dev)
{
    struct hi110x_sdio *hi_sdio;
    struct sdio_func   *func;
    uint32              max_blocks;

    HWIFI_ASSERT(NULL != io_dev);

    hi_sdio = bus_to_sdio(io_dev);
    func    = hi_sdio->func;

    /*
     * max transfer size limited by host,
     * because sdio use only 1 seg,
     * so transfer size can't exceed max seg size
     */
    max_blocks = MIN(func->card->host->max_blk_count,
                func->card->host->max_seg_size / func->cur_blksize);
    max_blocks = MIN(max_blocks, 511u);

    return MIN((max_blocks * func->cur_blksize), func->card->host->max_req_size) ;
}

STATIC int hsdio_heartbeat_log_dump(hwifi_union_log* log)
{
    struct hi110x_sdio *hi_sdio = (struct hi110x_sdio *)log->data;
    if(!hi_sdio)
    {
        HWIFI_UNION_PRINT(log, "hi_sdio is null ptr");
        return 0;
    }
    HWIFI_UNION_PRINT(log, "heartbeat cur timer timeout:%u(s)\n", hi_sdio->heartbeat.cur_timeout);
    HWIFI_UNION_PRINT(log, "heartbeat work timeout:%u(s),sleep timeout:%u(s)\n",
                    hi_sdio->heartbeat.work_timeout,hi_sdio->heartbeat.sleep_timeout);
    hi110x_print_cpu_time_str(hi_sdio->heartbeat.last_update_time, log->loglevel,
                                "heartbeat last package came at ");
    return 0;
}

STATIC hwifi_union_log heart_beat_log =
{
    .module = "WiFi heartbeat",
    .cb   = hsdio_heartbeat_log_dump,
};

STATIC int hsdio_sdio_log_dump(hwifi_union_log* log)
{
    struct hi110x_sdio *hi_sdio = (struct hi110x_sdio *)log->data;
    if(!hi_sdio)
    {
        HWIFI_UNION_PRINT(log, "hi_sdio is null ptr");
        return 0;
    }
    HWIFI_UNION_PRINT(log, "sdio interrupt count:%llu\n", hi_sdio->sdio_int_count);
    HWIFI_UNION_PRINT(log, "gpio interrupt count:%llu\n", hi_sdio->gpio_int_count);
    HWIFI_UNION_PRINT(log, "hsdio dev_err_cnt:%u, dev sub cnt:%u ,rx thread policy[%d]\n",
                                                hi_sdio->dev_err_cnt,
                                                hi_sdio->dev_err_subcnt,
                                                hi_sdio->gpio_rx_tsk->policy);
    hi110x_sdio_int_stat_dump(hi_sdio, log);
#ifdef CONFIG_PINCTRL
    HWIFI_UNION_PRINT(log, "sdio io cfg state:%s\n", io_cfg_t.current_pinctrl_state);
#endif
    return 0;
}

STATIC hwifi_union_log hsdio_sdio_log =
{
    .module = "WiFi sdio",
    .cb   = hsdio_sdio_log_dump,
};

void hi110x_sdio_hb_init(struct hi110x_sdio  *hi_sdio)
{
    hi_sdio->heartbeat.work_timeout =
                    hwifi_get_param_int(HISI_CONFIG_WIFI_HB_WORK_TIMEOUT);

    hi_sdio->heartbeat.sleep_timeout =
                    hwifi_get_param_int(HISI_CONFIG_WIFI_HB_SLEEP_TIMEOUT);
}


struct hi110x_sdio* hi110x_sdio_alloc(struct sdio_func *func)
{
    struct hi110x_sdio     *hi_sdio;

    HWIFI_ASSERT(NULL != func);

    /* alloce sdio control struct */
    hi_sdio = kzalloc(sizeof(*hi_sdio), GFP_KERNEL);
    if (NULL == hi_sdio)
    {
        HWIFI_ERROR("Failed to alloc hi_sdio!");
        return NULL;
    }

    hi_sdio->func           = func;
    hi_sdio->io_dev.disable = hi110x_bus_disable;
    hi_sdio->io_dev.enable  = hi110x_bus_enable;
    hi_sdio->io_dev.transfer= hi110x_bus_transfer;
    hi_sdio->io_dev.get_state       = hi110x_sdio_get_state;
    hi_sdio->io_dev.set_state       = hi110x_sdio_set_state;
    hi_sdio->io_dev.rx_info_reset   = hi110x_sdio_rx_transfer_info_reset;
    hi_sdio->io_dev.resume          = hi110x_bus_resume;
    hi_sdio->io_dev.suspend         = hi110x_bus_suspend;
    hi_sdio->io_dev.transfer_sg = hi110x_bus_transfer_sg;
    hi_sdio->io_dev.reset_flowctrl = hi110x_bus_reset_flowctrl;
    hi_sdio->io_dev.interrupt_pending = hi110x_bus_interrupt_pending;
    hi_sdio->io_dev.get_max_transfer_size = hi110x_bus_get_max_transfer_size;

    //atomic_set(&hi_sdio->intr_pending, 0);

    hi_sdio->next_pkt_len = 0;
    hi_sdio->rxbuf_size   = MIN((uint32)SDIO_RXBUF_MAX_SIZE,
                              hi110x_bus_get_max_transfer_size(&hi_sdio->io_dev));

    HWIFI_INFO("bus max transfer size %u", hi_sdio->rxbuf_size);
    mutex_init(&hi_sdio->rx_transfer_lock);
    skb_queue_head_init(&hi_sdio->next_pkt_sk_list);
    skb_queue_head_init(&hi_sdio->rx_reserve_skb_list);
    INIT_LIST_HEAD(&hi_sdio->rx_skb_pool_list);
    spin_lock_init(&hi_sdio->rx_skb_pool_lock);

    exception_enable = hwifi_get_param_int(HISI_CONFIG_WIFI_EXCEPTION_ENABLE);

    hi110x_sdio_hb_init(hi_sdio);

    hi_sdio->sdio_align_buff = kzalloc(SDIO_BLOCK_SIZE, GFP_KERNEL);
    if(!hi_sdio->sdio_align_buff)
    {
        kfree(hi_sdio);
        return NULL;
    }
    if(hisdio_skb_reserve_mem_init(hi_sdio))
    {
        kfree(hi_sdio->sdio_align_buff);
        mutex_destroy(&hi_sdio->rx_transfer_lock);
        kfree(hi_sdio);
        return NULL;
    }
#ifdef WLAN_PERFORM_OPT
    if (intr_mode)
    {
        /*创建帧解析线程*/
        hi_sdio->sdio_rx_dispose_tsk = hi110x_thread_create(hi110x_sdio_dispose_thread,
                            (struct hi110x_sdio *)hi_sdio,
                            &hi_sdio->sdio_rx_dispose_sema,
                            "hi110x_sdio_dispose_data", SCHED_FIFO, HI110X_SDIO_DISPOSE_THREAD_PRIORITY,g_lCpuId);
        if (NULL == hi_sdio->sdio_rx_dispose_tsk)
        {
            HWIFI_ERROR("Failed to create sdio dispose thread!");
            hisdio_skb_reserve_mem_exit(hi_sdio);
            kfree(hi_sdio->sdio_align_buff);
            mutex_destroy(&hi_sdio->rx_transfer_lock);
            kfree(hi_sdio);
            return NULL;
        }
    }
    else
#endif
    {
        /* create hi110x work queue thread for rx */
        hi_sdio->rx_process_wq = create_singlethread_workqueue("hi110x_sdio_rx");
        if (NULL == hi_sdio->rx_process_wq)
        {
            HWIFI_ERROR("Failed to create sdio rx workqueue!");
            /*hi110x_sdio_destroy_rxbuf_pool(hi_sdio);*/
            hisdio_skb_reserve_mem_exit(hi_sdio);
            kfree(hi_sdio->sdio_align_buff);
            mutex_destroy(&hi_sdio->rx_transfer_lock);
            kfree(hi_sdio);
            return NULL;
        }
        INIT_WORK(&hi_sdio->isr_work, hi110x_sdio_isr_work);
    }
    INIT_WORK(&hi_sdio->wakeup_work, hi110x_sdio_wakeup_work);
    INIT_WORK(&hi_sdio->sleep_work,  hi110x_sdio_sleep_work);
    hwifi_union_log_register(&hsdio_sdio_log, (void*)hi_sdio);
    hwifi_union_log_register(&heart_beat_log, (void*)hi_sdio);

    /* func keep a pointer to hi110x_sdio */
    sdio_set_drvdata(func, hi_sdio);
    return hi_sdio;
}
STATIC void hi110x_sdio_free(struct hi110x_sdio *hi_sdio)
{
    if (NULL == hi_sdio)
    {
        HWIFI_WARNING("Invalid NULL hi_sdio!");
        return;
    }
#ifdef WLAN_PERFORM_OPT
    if (intr_mode)
    {
        if (hi_sdio->sdio_rx_dispose_tsk != NULL)
        {
            hi110x_thread_stop(hi_sdio->sdio_rx_dispose_tsk, &hi_sdio->sdio_rx_dispose_sema);
            hi_sdio->sdio_rx_dispose_tsk = NULL;
        }
    }
    else
#endif
    {
        if (hi_sdio->rx_process_wq != NULL)
        {
            destroy_workqueue(hi_sdio->rx_process_wq);
            hi_sdio->rx_process_wq = NULL;
        }
    }
    hisdio_skb_reserve_mem_exit(hi_sdio);
    kfree(hi_sdio->sdio_align_buff);
    mutex_destroy(&hi_sdio->rx_transfer_lock);
    kfree(hi_sdio);
}


STATIC int32 hi110x_sdio_probe(struct sdio_func *func, const struct sdio_device_id *ids)
{
    struct hi110x_sdio     *hi_sdio;
    struct hi110x_device   *hi110x_dev;
    int32                   ret;

    HWIFI_ASSERT((NULL != func) && (NULL != ids));
    HWIFI_INFO("sdio function[%d] match,vendor id = %x;product id = %x",func->num, ids->vendor, ids->device);
    /*获取当前CPU号*/

    g_lCpuId = get_cpu();
    put_cpu();

    /* alloce sdio control struct */
    hi_sdio = hi110x_sdio_alloc(func);
    if (NULL == hi_sdio)
    {
        HWIFI_ERROR("Failed to alloc hi_sdio!");

        return -ENOMEM;
    }

    /* alloc and register hi110x */
    hi110x_dev = hi110x_alloc(&func->dev, &hi_sdio->io_dev);
    if (NULL == hi110x_dev)
    {
        HWIFI_ERROR("Failed to alloc hi110x_dev!");
        hi110x_sdio_free(hi_sdio);

        return -ENOMEM;
    }
    /* important */
    hi_sdio->hi110x_dev = hi110x_dev;

#ifdef WLAN_PERFORM_OPT

    /*初始化性能相关参数*/
    hiwifi_perform_para_init(hi_sdio);

#endif

    /* register interrupt process function */
    ret = hi110x_sdio_interrupt_register(hi_sdio);
    if(ret < 0)
    {
        HWIFI_ERROR("Failed to register sdio interrupt!");
        hi110x_release(hi110x_dev);
        hi110x_sdio_free(hi_sdio);

        return -EFAIL;
    }

    if (hi110x_sdio_dev_init(hi_sdio) != SUCC)
    {
        hi110x_sdio_interrupt_unregister(hi_sdio);
        hi110x_release(hi110x_dev);
        hi110x_sdio_free(hi_sdio);

        return -EFAIL;
    }

    if(hsdio_test_init_module() != SUCC)
    {
        hi110x_sdio_dev_deinit(hi_sdio);
        hi110x_sdio_interrupt_unregister(hi_sdio);
        hi110x_release(hi110x_dev);
        hi110x_sdio_free(hi_sdio);
        return -EFAIL;
    }

    HWIFI_INFO("hi110x sdio probe ok");
    sema_init(&sdio_wake_sema, 1);

    complete(&sdio_driver_complete);
    return SUCC;
}

STATIC void check_sg_format(struct scatterlist *sg,
                            uint32 sg_len)
{
    int i;
    struct scatterlist *sg_t;
    for_each_sg(sg, sg_t, sg_len, i)
    {
        HWIFI_WARN(((unsigned long)sg_virt(sg_t) & 0x03)||(sg_t->length & 0x03),
                       "[i:%d][addr:%p][len:%u] ", i, sg_virt(sg_t), sg_t->length);
    }
}

STATIC int mmc_io_rw_scat_extended(struct mmc_card *card, int write, unsigned fn,
                            unsigned addr, int incr_addr,
                            struct scatterlist *sg, uint32 sg_len,
                            unsigned blocks, unsigned blksz)
{
    struct mmc_request mrq = {0};
    struct mmc_command cmd = {0};
    struct mmc_data data = {0};

    HWIFI_BUG_ON(!card);
    HWIFI_BUG_ON(!sg);
    HWIFI_BUG_ON(sg_len == 0);
    HWIFI_BUG_ON(fn > 7);

    if(HWIFI_WARN_ON(blksz == 0))
        return -EINVAL;

    /* sanity check */
    if (unlikely(addr & ~0x1FFFF))
        return -EINVAL;

    /*sg format*/
    if(HWIFI_DEBUG_CONDTION())
        check_sg_format(sg, sg_len);

    mrq.cmd = &cmd;
    mrq.data = &data;

    cmd.opcode = SD_IO_RW_EXTENDED;
    cmd.arg = write ? 0x80000000 : 0x00000000;
    cmd.arg |= fn << 28;
    cmd.arg |= incr_addr ? 0x04000000 : 0x00000000;
    cmd.arg |= addr << 9;
    if (blocks == 1 && blksz <= 512)
        cmd.arg |= (blksz == 512) ? 0 : blksz;  /* byte mode */
    else
        cmd.arg |= 0x08000000 | blocks;     /* block mode */
    cmd.flags = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_ADTC;

    data.blksz = blksz;
    data.blocks = blocks;
    data.flags = write ? MMC_DATA_WRITE : MMC_DATA_READ;
    data.sg = sg;
    data.sg_len = sg_len;
    HWIFI_PR_DEBUG("[blksz:%u][blocks:%u][sg_len:%u][mode:%s]", blksz,
                blocks,sg_len,write?"write":"read");
    HWIFI_PR_DEBUG("%s : [cmd opcode:%d][cmd arg:0x%8x][cmd flags: 0x%8x]", mmc_hostname(card->host),
                cmd.opcode, cmd.arg, cmd.flags);
    HWIFI_DEBUG("Sdio %s data transfer start", write?"write":"read");
    mmc_set_data_timeout(&data, card);
    mmc_wait_for_req(card->host, &mrq);
    HWIFI_DEBUG("wait for %s tranfer over.", write?"write":"read");
    if (cmd.error)
        return cmd.error;
    if (data.error)
        return data.error;
    if(HWIFI_WARN(mmc_host_is_spi(card->host),
                  "HiSi WiFi  driver do not support spi sg transfer!"))
    {
        return -EIO;
    }
    if (cmd.resp[0] & R5_ERROR)
        return -EIO;
    if (cmd.resp[0] & R5_FUNCTION_NUMBER)
        return -EINVAL;
    if (cmd.resp[0] & R5_OUT_OF_RANGE)
        return -ERANGE;
    HWIFI_DEBUG("Transfer done. %s sucuess!", write?"write":"read");
    return 0;
}


STATIC int32 hi110x_sdio_rw_sg(struct hi110x_sdio *hi_sdio, int32 rw,
                        uint32 addr, struct scatterlist *sg,
                        uint32 sg_len, uint32 rw_sz)
{
    int ret = SUCC;
    uint32 align_len = 0;
    uint32 align_t = 0;
    int write = (rw == SDIO_READ) ? 0 : 1;
    struct sdio_func *func = hi_sdio->func;

    HWIFI_BUG_ON(!hi_sdio);
    HWIFI_BUG_ON(!rw_sz);

    if(HWIFI_WARN(!sg_len,"Sdio %s Scatter list num should never be zero, total request len: %u",
                   rw == SDIO_READ ? "SDIO READ" : "SDIO WRITE",
                   rw_sz))
    {
        return -EINVAL;
    }
    align_t = SDIO_ALIGN_4_OR_BLK(rw_sz);
    align_len = align_t - rw_sz;
    if(likely(align_len))
    {
        sg_set_buf(&sg[sg_len], hi_sdio->sdio_align_buff, align_len);
        sg_len++;
    }
    sg_mark_end(&sg[sg_len - 1]);

    HWIFI_DEBUG("Sdio %s request %u bytes transfer, scatter list num %u, used %u bytes to align",
                 write ?"write":"read",rw_sz, sg_len, align_len);

    rw_sz = align_t;
    /*sdio scatter list driver ,when letter than 512 bytes bytes mode, other blockmode*/
    HWIFI_WARN_ON((rw_sz >= SDIO_BLOCK_SIZE) && (rw_sz & (SDIO_BLOCK_SIZE - 1)));
    HWIFI_WARN_ON((rw_sz < SDIO_BLOCK_SIZE)  && (rw_sz & (4 - 1)));
    HWIFI_WARN((align_len & 0x3),"%u",align_len);

    sdio_claim_host(func);
    /*sdio bus state access lock by sdio bus claim locked.*/
    if(!hi110x_sdio_get_state(&hi_sdio->io_dev))
    {
        if(printk_ratelimit())
            HWIFI_WARNING("Sdio channel stopped!");
        schedule();
        sdio_release_host(func);
        return -EFAIL;
    }
    ret = mmc_io_rw_scat_extended(hi_sdio->func->card, write,
                                   hi_sdio->func->num, addr,
                                   0, sg,
                                   sg_len, (rw_sz/SDIO_BLOCK_SIZE)?:1,
                                   MIN(rw_sz, (uint32)SDIO_BLOCK_SIZE));
    if(unlikely(ret))
    {
        /*sdio transfer  error, stop heartbeat timer andpm watchdog,
          we need to close sdio channel*/
        if(write)
            hi_sdio->hsdio_tx_err_count++;
        else
            hi_sdio->hsdio_rx_err_count++;
        HWIFI_ERROR("Hsdio %s Failed, We export a exception.", write ?"Write":"Read");
        hwifi_exception_submit(hi_sdio->hi110x_dev->hcc);
        /*Wait for host controller to dump register.*/
        msleep(max(DEFAULT_ERR_WAIT_TIME,err_wait_time));
    }
    sdio_release_host(func);

    /* 统计吞吐量，根据吞吐量适配CPU工作频率 */
    ret = hi110x_fitcpu_for_throughput(hi_sdio, rw_sz, sg_len);

    return ret;
}


STATIC int32 hi110x_bus_transfer_sg(struct iodevice *io_dev, struct scatterlist *sg,
                                    uint32 sg_len, uint32 rw_sz)
{
    struct hi110x_sdio *hi_sdio = NULL;
    struct sdio_func   *func = NULL;
    int32               ret = 0;

    HWIFI_BUG_ON(!io_dev);

    hi_sdio = bus_to_sdio(io_dev);
    func    = hi_sdio->func;


    /* check wlan state; if gpio is down, it will reinit sdio; if gpio is up , send data */
    if (!g_wlan_info->wlan_gpio_state_get())
    {
        hi110x_wake_lock(hi_sdio->hi110x_dev->pm_info);
        hi_sdio->hi110x_dev->pm_info->pm_stat.host_dev_wakeup++;
        HWIFI_INFO("Wifi host wake up device ...");
    }

    g_wlan_info->check_wlan_state();

#ifdef WLAN_PERFORM_DEBUG
    if ((g_perform_cfg_info->trigger_mode) && ((Hi1101_TX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
    {
        hwifi_perform_tx_sdio_statistic(rw_sz);
    }
    if(g_perform_cfg_info->trigger_mode && g_perform_cfg_info->sdio_bypass && ((Hi1101_TX == g_perform_cfg_info->trigger_direction)||(Hi1101_ALL == g_perform_cfg_info->trigger_direction)))
    {
        hwifi_perform_tx_sdio_bypass(sg, rw_sz);
        ret = SUCC;
    }
    else
#endif
    {
        ret = hi110x_sdio_rw_sg(hi_sdio, SDIO_WRITE, REG_FUNC1_FIFO, sg, sg_len, rw_sz);
        /* when open power manager, start feed watchdog */
        if(g_powermgmt_switch && (ret == SUCC))
        {
            HWIFI_DEBUG("Sdio write,feed wifi soft watch dog...");
            hi110x_pm_feed_wdg(hi_sdio->hi110x_dev->pm_info);
        }
    }

    HWIFI_DEBUG("sdio transfer data, sg = %p, sg_num = %u, total_len=%u, ret = %d", sg, sg_len,rw_sz, ret);

    return ret;
}


STATIC void hi110x_sdio_remove(struct sdio_func *func)
{
    struct hi110x_sdio *hi_sdio;

    HWIFI_ASSERT(NULL != func);

    hi_sdio = (struct hi110x_sdio *)sdio_get_drvdata(func);
    if (NULL == hi_sdio)
    {
        HWIFI_WARNING("Invalid NULL hi_sdio!");
        return;
    }
    hsdio_test_exit_module();
    /* TBD: should move to netdev down */
    hi110x_sdio_dev_deinit(hi_sdio);
    hi110x_release(hi_sdio->hi110x_dev);

    hi110x_sdio_free(hi_sdio);
    sdio_set_drvdata(func, NULL);

    HWIFI_INFO("hisilicon connectivity driver has been removed.");
}
STATIC int hi110x_sdio_suspend(struct device *dev)
{
    struct sdio_func *func;
    struct hi110x_sdio *hi_sdio;
    if (down_interruptible(&sdio_wake_sema))
    {
        HWIFI_ERROR("sdio_wake_sema down failed.");
    }
    HWIFI_INFO("+++++++sdio suspend+++++++++++++");
    if (NULL == dev)
    {
        HWIFI_WARNING("dev is null");
        return SUCC;
    }
    func = dev_to_sdio_func(dev);
    if (NULL == func)
    {
        HWIFI_WARNING("fun is null");
        return SUCC;
    }
    hi_sdio = sdio_get_drvdata(func);
    if ((NULL == hi_sdio) || (NULL == hi_sdio->hi110x_dev)
     || (NULL == hi_sdio->hi110x_dev->pm_info))
    {
        HWIFI_WARNING("can not get pm_info");
        return SUCC;
    }
    if (wake_lock_active(&(hi_sdio->hi110x_dev->pm_info->wklock_wifi)))
    {
        /* has wake lock so stop controller's suspend,
         * otherwise controller maybe error while sdio reinit*/
        HWIFI_INFO("Already wake up");
        up(&sdio_wake_sema);
        return -EFAIL;
    }
    return SUCC;
}


STATIC int hi110x_sdio_resume(struct device *dev)
{
    up(&sdio_wake_sema);
    HWIFI_INFO("--------sdio resume-----------");
    return SUCC;
}


STATIC struct sdio_device_id const hi110x_sdio_ids[] = {
    { SDIO_DEVICE(VENDOR_ID_HISI, PRODUCT_ID_HISI_1101) },
    {},
};
MODULE_DEVICE_TABLE(sdio, hi110x_sdio_ids);

STATIC const struct dev_pm_ops hi110x_sdio_pm_ops = {
     .suspend = hi110x_sdio_suspend,
     .resume = hi110x_sdio_resume,
};

STATIC  struct sdio_driver hi110x_sdio_driver = {
    .name       = "hi110x_sdio",
    .id_table   = hi110x_sdio_ids,
    .probe      = hi110x_sdio_probe,
    .remove     = hi110x_sdio_remove,
    .drv        = {
        .owner  = THIS_MODULE,
        .pm     = &hi110x_sdio_pm_ops,
    }
};


STATIC int32  hi110x_init_global_params(void)
{
    loglevel                = hwifi_get_param_int(HISI_CONFIG_WIFI_LOGLEVEL);
    panic_debug             = hwifi_get_param_int(HISI_CONFIG_WIFI_PANIC_DEBUG);
    band_5g_enabled         = hwifi_get_param_int(HISI_CONFIG_WIFI_BAND_5G_ENABLEED);
    g_data_rate_down        = hwifi_get_param_int(HISI_CONFIG_WIFI_DATA_RATE_DOWN);
    g_data_rate_up          = hwifi_get_param_int(HISI_CONFIG_WIFI_DATA_RATE_UP);
    hcc_assemble_count      = hwifi_get_param_int(HISI_CONFIG_WIFI_HCC_ASSEMBLE_COUNT);
    g_ba_max_tx             = hwifi_get_param_int(HISI_CONFIG_WIFI_BA_MAX_TX);
    g_ba_max_rx             = hwifi_get_param_int(HISI_CONFIG_WIFI_BA_MAX_RX);
    g_ba_max_retry          = hwifi_get_param_int(HISI_CONFIG_WIFI_BA_MAX_BUFFERSIZE);
    g_cur_cpufreq           = hwifi_get_param_int(HISI_CONFIG_WIFI_CUR_CPUFREQ);
    g_sdio_rate_40M         = hwifi_get_param_int(HISI_CONFIG_WIFI_SDIO_RATE_40M);
    g_sdio_rate_80M         = hwifi_get_param_int(HISI_CONFIG_WIFI_SDIO_RATE_80M);
    g_sdio_rate_160M        = hwifi_get_param_int(HISI_CONFIG_WIFI_SDIO_RATE_160M);
    g_powermgmt_switch      = hwifi_get_param_int(HISI_CONFIG_WIFI_POWERMGMT_SWITCH);
    g_watchdog_timeout      = hwifi_get_param_int(HISI_CONFIG_WIFI_WATCHDOG_TIMEOUT);
    g_wmm_drop_interval     = hwifi_get_param_int(HISI_CONFIG_WIFI_WMM_DROP_INTERVAL);
    g_wmm_que_len_for_drop  = hwifi_get_param_int(HISI_CONFIG_WIFI_QUE_LEN_FOR_DROP);
    g_wmm_drop_cnt_per_time = hwifi_get_param_int(HISI_CONFIG_WIFI_WMM_DROP_CNT_PER_TIME);

    /*leacy sched_ctrl              = hwifi_get_param_int(HISI_CONFIG_WIFI_TXRX_THREAD_TPYE);*/
    g_ap_data_rate_down     = hwifi_get_param_int(HISI_CONFIG_WIFI_AP_DATA_RATE_DOWN);
    g_ap_data_rate_up       = hwifi_get_param_int(HISI_CONFIG_WIFI_AP_DATA_RATE_UP);
    countrycode_change_by_ie_enabled = hwifi_get_param_int(HISI_CONFIG_WIFI_COUNTRYCODE_CHANGE_BY_IE);

#ifdef __ROAM__
    g_roam_switch          =  hwifi_get_param_int(HISI_CONFIG_WIFI_ROAM_SWITCH);
#endif

    g_ap_coex_2040m_enabled = hwifi_get_param_int(HISI_CONFIG_WIFI_AP_COEX_2040M_ENABLED);

    g_sta_2040m_enabled = hwifi_get_param_int(HISI_CONFIG_WIFI_STA_2040M_ENABLED);
    g_ap_2040m_enabled = hwifi_get_param_int(HISI_CONFIG_WIFI_AP_2040M_ENABLED);

#ifdef __ROAM__
    g_trigger_rssi_threshold = hwifi_get_param_int(HISI_CONFIG_WIFI_ROAM_TRIGGER_RSSI_THRESHOLD);
    g_trigger_count_threshold = hwifi_get_param_int(HISI_CONFIG_WIFI_ROAM_TRIGGER_COUNT_THRESHOLD);
#endif

    return SUCC;
}


STATIC int32 hsdio_io_cfg_init(void)
{
    int32 ret = SUCC;
    if(!hi110x_of_wifi_dev)
    {
        HWIFI_INFO("WiFi Boot From Sdio drvier.");
        return ret;
    }
#ifdef CONFIG_PINCTRL
    io_cfg_t.pctrl = devm_pinctrl_get(&hi110x_of_wifi_dev->dev);
    if (HWIFI_WARN_ON(IS_ERR_OR_NULL(io_cfg_t.pctrl)))
    {
        HWIFI_WARNING("iomux_lookup_block failed, and the value of pctrl is %p",io_cfg_t.pctrl);
        ret = -EFAIL;
        goto err_pinctrl_get;
    }
    io_cfg_t.pins_normal = pinctrl_lookup_state(io_cfg_t.pctrl, PINCTRL_STATE_DEFAULT);
    if (HWIFI_WARN_ON(IS_ERR_OR_NULL(io_cfg_t.pins_normal)))
    {
        HWIFI_WARNING("pinctrl_lookup_state default failed, and the value of pins_normal is%p", io_cfg_t.pins_normal);
        ret = -EFAIL;
        goto err_pinctrl_normal;
    }

    io_cfg_t.pins_idle = pinctrl_lookup_state(io_cfg_t.pctrl, PINCTRL_STATE_IDLE);
    if (HWIFI_WARN_ON(IS_ERR_OR_NULL(io_cfg_t.pins_idle)))
    {
        HWIFI_WARNING("pinctrl_lookup_state idle failed, and the value of pins_idle is %p", io_cfg_t.pins_idle);
        ret = -EFAIL;
        goto err_pinctrl_idle;
    }
    io_cfg_t.current_pinctrl_state = "unselected";
    HWIFI_INFO("Sdio io cfg init ok.");
    return ret;
err_pinctrl_idle:
    io_cfg_t.pins_idle = NULL;
err_pinctrl_normal:
    io_cfg_t.pins_normal = NULL;
err_pinctrl_get:
    devm_pinctrl_put(io_cfg_t.pctrl);
    io_cfg_t.pctrl = NULL;
#endif
    return ret;
}

STATIC void hsdio_io_cfg_exit(void)
{
#ifdef CONFIG_PINCTRL
    if(!IS_ERR_OR_NULL(io_cfg_t.pctrl))
    {
        devm_pinctrl_put(io_cfg_t.pctrl);
        io_cfg_t.pctrl = NULL;
    }
    io_cfg_t.pins_normal = NULL;
    io_cfg_t.pins_idle = NULL;
    io_cfg_t.current_pinctrl_state = "unselected";
#endif
}

#ifdef CONFIG_PINCTRL
int32 hsdio_sdio_iocfg(char* state)
{
    int ret = -EFAIL;
    if(HWIFI_WARN_ON(!state))
        return ret;

    if(IS_ERR_OR_NULL(io_cfg_t.pctrl))
    {
        HWIFI_PRINT_ONCE(INFO, "pctrl hadn't got!");
        return ret;
    }
    HWIFI_INFO("Try to set the pinctrl state from %s to %s",
            io_cfg_t.current_pinctrl_state?:"",
            state);
    if(!strcmp(state, PINCTRL_STATE_DEFAULT))
    {
        /*After power on,wakeup,  or before power off ,
          we should make the sdio io to work state*/
        if(!IS_ERR_OR_NULL(io_cfg_t.pins_normal))
            ret = pinctrl_select_state(io_cfg_t.pctrl, io_cfg_t.pins_normal);
        else
            HWIFI_PRINT_ONCE(WARNING, "pin normal hadn't got");
    }
    else if(!strcmp(state, PINCTRL_STATE_IDLE))
    {
        /*Before power on,sleep,  or after power off ,
          we should make the sdio io to idle*/
        if(!IS_ERR_OR_NULL(io_cfg_t.pins_idle))
            ret = pinctrl_select_state(io_cfg_t.pctrl, io_cfg_t.pins_idle);
        else
            HWIFI_PRINT_ONCE(WARNING, "pin idle hadn't got");
    }
    else if(!strcmp(state, "unsupport"))
    {
        ret = -EFAIL;
    }
    else
    {
        HWIFI_WARN(1, "unkown state:%s\n", state);
        return ret;
    }
    if(!ret)
    {
        HWIFI_INFO("Set sdio io done.");
        io_cfg_t.current_pinctrl_state = state;
    }
    return ret;

}
#endif

STATIC  int32 hi110x_sdio_init_module(void)
{
    int ret;

    /* read data from ini file and init host params */
     hwifi_config_init();
     hi110x_init_global_params();

    HWIFI_DEBUG("*********Hi1101 WiFi Driver Installing*********\n");

    if (userctrl_init() != SUCC)
    {
        HWIFI_WARNING("log level ctrl init failed");
        return -EFAIL;
    }
    init_completion(&sdio_driver_complete);
    HWIFI_DEBUG("Notify mmc to detect sdio device");

    ret = hsdio_io_cfg_init();
    if(ret && wifi_force_load)
        goto failed_io_cfg_init;

    hsdio_sdio_iocfg(PINCTRL_STATE_IDLE);
    /*
     * this should be done in mpw1
     * it depends on the gpio used to power up and down 1101 chip
     *
     * */
#ifdef WLAN_POWER_MANAGEMENT
    wlan_power_set(1);
#endif
    hsdio_sdio_iocfg(PINCTRL_STATE_DEFAULT);
    /*Notify to enum sdio device*/
    hi110x_sdio_detectcard_to_core(1);
    HWIFI_DEBUG("Start to register sdio module");

#ifdef WLAN_PERFORM_DEBUG
    hwifi_perform_init_rx_statistic();
    hwifi_perform_init_tx_statistic();
#endif
    INIT_COMPLETION(sdio_driver_complete);
    ret = sdio_register_driver(&hi110x_sdio_driver);
    if(ret)
    {
        HWIFI_ERROR("Register sdio driver Failed ret=%d", ret);
        goto failed_sdio_reg;
    }
    if(wait_for_completion_timeout(&sdio_driver_complete, 10*HZ))
    {
        HWIFI_INFO("WiFi Load Sucuess!Sdio enum Done.");
        ret = SUCC;
    }
    else
    {
        HWIFI_ERROR("Sdio enum timeout.");
        ret =  -ENODEV;
        goto failed_sdio_enum;
    }
    return ret;
failed_sdio_enum:
    sdio_unregister_driver(&hi110x_sdio_driver);
failed_sdio_reg:
    hi110x_sdio_detectcard_to_core(0);
#ifdef WLAN_POWER_MANAGEMENT
    wlan_power_off();
#endif
    hsdio_io_cfg_exit();
failed_io_cfg_init:
    userctrl_exit();
    return ret;
}

STATIC  void __exit hi110x_sdio_exit_module(void)
{
    HWIFI_DEBUG("Sdio module unregistered");
    sdio_unregister_driver(&hi110x_sdio_driver);
    hi110x_sdio_detectcard_to_core(0);
#ifdef WLAN_POWER_MANAGEMENT
    wlan_power_set(0);
#endif
    hsdio_io_cfg_exit();
    userctrl_exit();
}

#ifdef CONFIG_USE_OF
STATIC int  hi110x_wifi_power_probe(struct platform_device *pdev)
{
    int ret = 0;
    HWIFI_BUG_ON(!pdev);
    hi110x_of_wifi_dev = pdev;
    memset(&io_cfg_t,0,sizeof(hsdio_io_cfg));
    ret = hi110x_sdio_init_module();
    complete(&wifi_dev_complete);
    return ret;
}

STATIC int  hi110x_wifi_power_remove(struct platform_device *pdev)
{
    hi110x_of_wifi_dev = NULL;
    hi110x_sdio_exit_module();
    return SUCC;
}


STATIC struct of_device_id hi110x_wifi_power_match_table[] = {
    { .compatible = DTS_COMP_WIFI_POWER_NAME,
        .data = NULL,
            },
    { },
};

STATIC struct platform_driver hi110x_wifi_power_driver = {
    .driver        = {
        .name        = "hi110x_power",
        .owner = THIS_MODULE,
               .of_match_table    = hi110x_wifi_power_match_table,
    },
    .probe          = hi110x_wifi_power_probe,
    .remove     = hi110x_wifi_power_remove,
};
#endif
#define WIFI_IS_START 1
uint32          g_wifi_start = 0;
struct mutex    wifi_enable_write_mutex;

int32 wifi_start(void)
{
    int ret;
#ifdef CONFIG_USE_OF
    init_completion(&wifi_dev_complete);
    ret = platform_driver_register(&hi110x_wifi_power_driver);
    if (ret)
    {
        HWIFI_ERROR("hi1101 platform_driver_register failed, ret:%d.\n", ret);
        return ret;
    }
    if(wait_for_completion_timeout(&wifi_dev_complete, 2*HZ))
    {
        HWIFI_INFO("Of platform device probe done.");
        ret = SUCC;
    }
    else
    {
        HWIFI_ERROR("Of platform device probe failed, please check the dts file!");
        ret =  -ENODEV;
        platform_driver_unregister(&hi110x_wifi_power_driver);
        return ret;
    }
#else
    ret = hi110x_sdio_init_module();
#endif

    return ret;
}
static ssize_t wifi_start_write(struct file *filp, const char __user *buffer, size_t len, loff_t *off)
{
    HWIFI_DEBUG("wifi_start_write enter!");
    mutex_lock(&wifi_enable_write_mutex);
    if (WIFI_IS_START == g_wifi_start) {
        HWIFI_ERROR("wifi has started!");
        mutex_unlock(&wifi_enable_write_mutex);
        return -EINVAL;
    }

    g_wifi_start = WIFI_IS_START;
    mutex_unlock(&wifi_enable_write_mutex);

    HWIFI_INFO("wifi start begin\n");
    if (0 != wifi_start())
    {
        HWIFI_ERROR("wifi start fail!");
        return -EINVAL;
    }

	HWIFI_INFO("wifi start ok\n");
    return len;
}

static const struct file_operations wifi_proc_start = {
	.owner = THIS_MODULE,
	.write = wifi_start_write,
};


#define WIFI_START_PROC_DIR "wifi_enable"
#define WIFI_START_PROC_FILE "wifi_start"
STATIC  int32 __init hi110x_wifi_init_module(void)
{
    int ret = 0;
    struct proc_dir_entry *wifi_start_dir = NULL;
    struct proc_dir_entry *wifi_start_file = NULL;

#ifdef CONFIG_HWCONNECTIVITY
    if (!isMyConnectivityChip(CHIP_TYPE_HI110X)) {
        HWIFI_ERROR("cfg wifi chip type is not match, skip driver init");
        return -EINVAL;
    } else {
        HWIFI_INFO("cfg wifi type is matched with hi110x, continue");
    }
#endif

    wifi_start_dir = proc_mkdir(WIFI_START_PROC_DIR, NULL);
    if (!wifi_start_dir) {
		HWIFI_ERROR("wifi start create proc dir failed.\n");	
		ret = -ENOMEM;
        return ret;
    }

	wifi_start_file = proc_create(WIFI_START_PROC_FILE, S_IWUSR|S_IWGRP, wifi_start_dir, &wifi_proc_start);
    if (!wifi_start_file) {
		HWIFI_ERROR("wifi start create proc file failed.\n");	
		ret = -ENOMEM;
        return ret;
    }

    mutex_init(&wifi_enable_write_mutex);

    HWIFI_INFO("wifi start create proc file ok.\n");	
	return ret;
}

STATIC  void __exit hi110x_wifi_exit_module(void)
{
	if (WIFI_IS_START == g_wifi_start)
	{
#ifdef CONFIG_USE_OF
		platform_driver_unregister(&hi110x_wifi_power_driver);
#else
		hi110x_sdio_exit_module();
#endif
	}
}
module_init(hi110x_wifi_init_module);
module_exit(hi110x_wifi_exit_module);

MODULE_LICENSE("GPL");


