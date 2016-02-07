

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * 1 Header File Including
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_func.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/skbuff.h>


#include "hi110x.h"
#include "hwifi_extern.h"
#include "hsdio_test.h"
extern struct kobject *g_sysfs_hi110x;
/*
 * 2 Global Variable Definition
 */
struct hsdio_test_event  *g_hsdio_test_event = NULL;

struct hsdio_test_mode hsdio_test_mode[HSDIO_TEST_CASE_COUNT] = { 
    [HSDIO_TEST_CASE_TX] = {"tx", HCC_SUBTYPE_TX, "Hi110x Tx mode only"}, 
    [HSDIO_TEST_CASE_LOOP] = {"loop", HCC_SUBTYPE_LOOP, "Hi110x Loop mode"}
};

/*
 * 3 Function Declare
 */


uint64 hsdio_test_get_trans_time(ktime_t start_time, ktime_t stop_time)
{
    ktime_t trans_time;
    uint64  trans_us;

    trans_time = ktime_sub(stop_time, start_time);

    trans_us = (uint64)ktime_to_us(trans_time);

    if (trans_us == 0)
    {
        trans_us = 1;
    }

    return trans_us;
}

void hsdio_test_result_report(uint32 throughput, uint64 seconds)
{
     const char *mode_desc;
     int32  idx;

     idx = g_hsdio_test_event->test_data.mode_idx; /* 0 or 1 */

     mode_desc = hsdio_test_mode[idx].mode_desc;
     printk(KERN_ERR"[WIFI]########%s##SDIO Test %d pkts sent##########", mode_desc, g_hsdio_test_event->test_data.pkt_sent);
     printk(KERN_ERR"[WIFI]########%s##SDIO Test %d pkts rcvd##########", mode_desc, g_hsdio_test_event->test_data.pkt_rcvd);
     printk(KERN_ERR"[WIFI]########%s##SDIO Test %d pkts generate##########", mode_desc, g_hsdio_test_event->test_data.pkt_gen);
     printk(KERN_ERR"[WIFI]########%s##SDIO Test pkt lengh %d##########", mode_desc, g_hsdio_test_event->test_data.pkt_len);
     printk(KERN_ERR"[WIFI]########%s##SDIO Test Throughput %u Mbps##########", mode_desc, throughput);
     printk(KERN_ERR"[WIFI]########Hi110x SDIO Test## Used  %llu seconds##########", seconds);
}
 void hsdio_test_throughput_cac(uint32 pkt_len, uint32 pkts_sent, 
                                                                                          uint32 pkt_rcvd, ktime_t start_time, ktime_t stop_time)
 {
     uint64 trans_us;
     uint64 temp;
     uint64 us_to_s; /* converted  usecond to second */

     trans_us = hsdio_test_get_trans_time(start_time, stop_time);

     temp = (pkts_sent + pkt_rcvd);

     temp = temp * pkt_len;

     temp = temp * 1000u;

     temp = temp * 1000u;

     temp = temp * 8u;

     do_div(temp, trans_us); /* temp = temp / trans_us */

     do_div(temp, 1024u); /* Converted to KBit */

     do_div(temp, 1024u); /* Converted to MBit */

     us_to_s = trans_us;

     do_div(us_to_s, 1000000u); /* us_to_s = us_to_s / 1000000 */
     g_hsdio_test_event->test_data.throughput = temp;
     g_hsdio_test_event->test_data.trans_time = us_to_s;
 }
void hsdio_test_tx_throughput_statistic(ktime_t start_time, ktime_t stop_time)
{
    hsdio_test_throughput_cac(
                 g_hsdio_test_event->test_data.pkt_len,
                  g_hsdio_test_event->test_data.pkt_sent,
                   0,
                   start_time,
                   stop_time);
}

void hsdio_test_loop_throughput_statistic(ktime_t start_time, ktime_t stop_time)
{
    hsdio_test_throughput_cac(
                 g_hsdio_test_event->test_data.pkt_len,
                  g_hsdio_test_event->test_data.pkt_sent,
                   g_hsdio_test_event->test_data.pkt_rcvd,
                   start_time,
                   stop_time);
}

STATIC  void hsdio_test_throughput_gen(void)
{
    if(HSDIO_TEST_CASE_TX == g_hsdio_test_event->test_data.mode_idx)
    {
        hsdio_test_tx_throughput_statistic(g_hsdio_test_event->start_time, g_hsdio_test_event->last_time);
    }else if(HSDIO_TEST_CASE_LOOP == g_hsdio_test_event->test_data.mode_idx )
    {
        hsdio_test_loop_throughput_statistic(g_hsdio_test_event->start_time, g_hsdio_test_event->last_time);
    }        
}


STATIC int32 hsdio_test_rcvd(uint8 main_type, uint8 test_type, struct sk_buff *skb, void *context)
{
     struct hsdio_test_event    *sdio_test_event = (struct hsdio_test_event *)context;

     uint32 tmp_test_type;

     HWIFI_ASSERT(NULL != skb);

     sdio_test_event->test_data.pkt_rcvd++;
     g_hsdio_test_event->last_time= ktime_get();
     /* parse skb data in assemble and none assemble packet */
     if ( ( HSDIO_TEST_DATA_STOP == skb->data[16] ) || ( HSDIO_TEST_DATA_STOP == skb->data[0] ))
     {

        tmp_test_type = hsdio_test_mode[g_hsdio_test_event->test_data.mode_idx].test_type;

        sdio_test_event->test_data.pkt_rcvd--;/* exclude stop pkt */

        if (HCC_SUBTYPE_LOOP == tmp_test_type)
        {
            complete(&g_hsdio_test_event->test_done);
        }
        else if (HCC_SUBTYPE_TX == tmp_test_type)
        {
            complete(&g_hsdio_test_event->test_done);
        }
     }

     hwifi_free_skb(skb);

     return SUCC;
}


STATIC int32 hsdio_test_sent(uint8 test_type, uint8 gen_type)
{
     struct sk_buff     *skb;
     int32               ret;
     /*
      * 1) alloc memory for skb,
      * 2) skb free when send after dequeue from tx queue
      * */
     skb  = hwifi_alloc_skb(g_hsdio_test_event->test_data.pkt_len);
     if (NULL == skb)
     {
         HWIFI_WARNING("hwifi alloc skb fail.");
         return -EFAIL;
     }

     if (HSDIO_TEST_GEN_START == gen_type)
     {
        /* fill test data */
        memset(skb_put(skb, g_hsdio_test_event->test_data.pkt_len), HSDIO_TEST_DATA_GEN, g_hsdio_test_event->test_data.pkt_len);
     }
     else if (HSDIO_TEST_GEN_STOP == gen_type)
     {
        /* fill stop data */
        memset(skb_put(skb, g_hsdio_test_event->test_data.pkt_len), HSDIO_TEST_DATA_STOP, g_hsdio_test_event->test_data.pkt_len);
     }

     ret = hwifi_send(HCC_TYPE_TEST, test_type, skb);
     if(SUCC != ret)
     {
         HWIFI_WARNING("hwifi send test pkt error.");

         return ret;
     }

     return ret;
}


STATIC int32 hsdio_test_start(uint8 test_type)
{
    int32             ret;
    uint32            i;

    /* statatics of test */
    g_hsdio_test_event->test_data.pkt_rcvd = 0;
    g_hsdio_test_event->test_data.pkt_sent = 0;
    g_hsdio_test_event->test_data.throughput = 0;
    g_hsdio_test_event->test_data.trans_time = 0;
    g_hsdio_test_event->last_time= g_hsdio_test_event->start_time = ktime_get();

    for (i = 0; i < g_hsdio_test_event->test_data.pkt_gen; i++)
    {
        ret = hsdio_test_sent(test_type, HSDIO_TEST_GEN_START);
        if(-EFAIL == ret)
        {
            HWIFI_WARNING("hsdio test gen pkt send fail.");
            return ret;
        }

        g_hsdio_test_event->test_data.pkt_sent++ ;
        g_hsdio_test_event->last_time= ktime_get();
        if(unlikely(FALSE == g_hsdio_test_event->started))
        {
            ret = -EFAIL;
            return ret;
        }
    }

    return SUCC;
}
STATIC int32 hsdio_test_stop(void)
{
    int32   ret;

    ret = hsdio_test_sent(HCC_SUBTYPE_LOOP, HSDIO_TEST_GEN_STOP);
    if (-EFAIL == ret)
    {
        HWIFI_WARNING("hsdio test stop pkt send fail.");
        return -EFAIL;
    }

    return ret;
}


void hsdio_test_work(struct work_struct *work)
{
    uint8       test_type;
    int32       ret;

    test_type = hsdio_test_mode[g_hsdio_test_event->test_data.mode_idx].test_type;

    /* hsdio test start */
    ret = hsdio_test_start(test_type);
    if ( -EFAIL == ret)
    {
        complete(&g_hsdio_test_event->test_done);
        HWIFI_WARNING("hsdio test work start test pkt send fail.");
        return;
    }

    /* no matter in TX only or Loop only, send Loop packet so that host can know that firmware has terminate */
    ret = hsdio_test_stop();
    if ( -EFAIL == ret)
    {
        complete(&g_hsdio_test_event->test_done);
        HWIFI_WARNING("hsdio test work stop test pkt send fail.");
        return;
    }
}


STATIC ssize_t  hsdio_test_get_para(struct device *dev, struct device_attribute *attr, char*buf)
{
    int ret = 0;
    const char *mode_str;
    int32       tmp_mode_idx;

    HWIFI_ASSERT((NULL != dev) && (NULL != attr) && (NULL != buf));

    tmp_mode_idx = g_hsdio_test_event->test_data.mode_idx;

    mode_str = "unknown";

    if ((tmp_mode_idx >= 0) && (tmp_mode_idx < ARRAY_SIZE(hsdio_test_mode)))
    {
        mode_str = hsdio_test_mode[tmp_mode_idx].mode;
    }
    
    hsdio_test_throughput_gen();  

     ret +=  snprintf(buf + ret , PAGE_SIZE-1,"########%s##SDIO Test %d pkts sent##########\n", mode_str, g_hsdio_test_event->test_data.pkt_sent);
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1,"########%s##SDIO Test %d pkts rcvd##########\n", mode_str, g_hsdio_test_event->test_data.pkt_rcvd);
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1,"########%s##SDIO Test %d pkts generate##########\n", mode_str, g_hsdio_test_event->test_data.pkt_gen);
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1, "########%s##SDIO Test pkt lengh %d##########\n", mode_str, g_hsdio_test_event->test_data.pkt_len);
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1, "########%s##SDIO Test Throughput %u Mbps##########\n", mode_str, (uint32)g_hsdio_test_event->test_data.throughput);
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1, "########Hi110x SDIO Test## Used  %llu seconds##########\n", g_hsdio_test_event->test_data.trans_time);    

    return ret;
}


STATIC ssize_t  hsdio_test_set_para(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    hsdio_test_data  data = {0};
    uint32              tmp_pkt_len;
    uint32              tmp_pkt_gen;
    int8                mode[128] = {0};
    uint32              i;

    HWIFI_ASSERT((NULL != dev) && (NULL != attr) && (NULL != buf));


    if ((sscanf(buf, "%4s %d %d", mode, &tmp_pkt_len, &tmp_pkt_gen) != 3))
    {
        HWIFI_WARNING("input format is incorrect, example:loop 1536 10000 or tx 1536 10000 ");

        return -EINVAL;
    }

    for (i = 0; i < ARRAY_SIZE(hsdio_test_mode); i++)
    {
            /* find mode if match */
            if (sysfs_streq(hsdio_test_mode[i].mode, mode))
            {
                break;
            }
    }

    if (ARRAY_SIZE(hsdio_test_mode) == i)
    {
        HWIFI_WARNING("unknown test mode.");

        return -EINVAL;
    }


    data.pkt_len = tmp_pkt_len;
    data.pkt_gen = tmp_pkt_gen;
    data.mode_idx = i;

    if(hsdio_test_set_case(&data))
    {
        return -EINVAL;
    }

    hsdio_test_result_report(g_hsdio_test_event->test_data.throughput, g_hsdio_test_event->test_data.trans_time);
    return count;
}

STATIC DEVICE_ATTR(hsdio_test, S_IRUGO | S_IWUSR, hsdio_test_get_para, hsdio_test_set_para);

STATIC ssize_t  hsdio_test_get_readme(struct device *dev, struct device_attribute *attr, char*buf)
{
    int ret = 0;

    HWIFI_ASSERT((NULL != dev) && (NULL != attr) && (NULL != buf));

     ret +=  snprintf(buf + ret , PAGE_SIZE-1,"hsdio test usage:\n");
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1,"     tx test: echo 'tx 1500 10' > hsdio_test #\n");
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1,"         send 10 packages   1500 bytes length\n");
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1, "    loop test: echo 'loop 1500 10' > hsdio_test\n");
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1, "        loop send 10 pkts 1500 bytes length\n");
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1, "    check results:\n");    
     ret +=  snprintf(buf + ret,  PAGE_SIZE-1, "        cat hsdio_test\n");    

    return ret;
}
STATIC DEVICE_ATTR(Readme, S_IRUGO, hsdio_test_get_readme, NULL);
STATIC struct attribute *hsdio_test_sysfs_entries[] = {
        &dev_attr_hsdio_test.attr,    
        &dev_attr_Readme.attr,            
        NULL
};

STATIC struct attribute_group hsdio_test_attribute_group = {
        .name = "test_case",
        .attrs = hsdio_test_sysfs_entries,
};

int hsdio_test_set_case(hsdio_test_data *data)
{
    int ret;
    HWIFI_BUG_ON(!data);
    if(unlikely(!g_hsdio_test_event->test_workqueue))
    {
        HWIFI_ERROR("wifi probe failed, please retry.");
        return -EBUSY;
    }
    /* filter and update test_time */
    if (data->pkt_len > HSDIO_TEST_MAX_PKTLEN)
    {
        HWIFI_WARNING("invalid test pkt len. %u", data->pkt_len);

        return -EINVAL;
    }

    if (data->pkt_gen  > HSDIO_TEST_MAX_PKTGEN)
    {
        HWIFI_WARNING("invalid test pkt which need to gen. %u", data->pkt_gen);

        return -EINVAL;
    }
    
    mutex_lock(&g_hsdio_test_event->mutex_lock);
    if (TRUE == g_hsdio_test_event->started)
    {
        HWIFI_WARNING("sdio test task is processing, wait for end and reinput.");
        mutex_unlock(&g_hsdio_test_event->mutex_lock);
        return -EINVAL;
    }

    g_hsdio_test_event->started = TRUE;
    mutex_unlock(&g_hsdio_test_event->mutex_lock);
    
    memcpy(&g_hsdio_test_event->test_data, data, sizeof(hsdio_test_data));

    INIT_COMPLETION(g_hsdio_test_event->test_done);     
    
    queue_work(g_hsdio_test_event->test_workqueue, &g_hsdio_test_event->test_work);
    ret = wait_for_completion_interruptible(&g_hsdio_test_event->test_done);
    if(ret < 0)
    {
        HWIFI_WARNING("Test Event  terminated ret=%d", ret);
    }
    HWIFI_INFO("Test Done.");
    
    hsdio_test_throughput_gen();
    
    memcpy(data, &g_hsdio_test_event->test_data, sizeof(hsdio_test_data));
    g_hsdio_test_event->started = FALSE;
    return SUCC;
}

void hsdio_test_get_case(hsdio_test_data *data)
{
    HWIFI_BUG_ON(!data);
    hsdio_test_throughput_gen();
    memcpy(data, &g_hsdio_test_event->test_data, sizeof(hsdio_test_data));
}


int32  hsdio_test_init_module(void)
{
    int32       ret ;

    /* create the files associated with this kobject */
    ret = sysfs_create_group(g_sysfs_hi110x, &hsdio_test_attribute_group);
    if (0 != ret)
    {
        HWIFI_WARNING("sysfs create group fail.");
        return ret;
    }

    /* alloc memory for perf_action pointer */
    g_hsdio_test_event = kzalloc(sizeof(*g_hsdio_test_event), GFP_KERNEL);
    if (!g_hsdio_test_event)
    {
        HWIFI_WARNING("error kzalloc g_hsdio_test_event mem.");
        ret = -ENOMEM;
        goto fail_g_hsdio_test_event;
    }

    /* register callback for rx */
    ret = hwifi_rx_extern_register(HCC_TYPE_TEST, hsdio_test_rcvd, g_hsdio_test_event);
    if ( SUCC != ret)
    {
        HWIFI_WARNING("error %d register callback for rx.", ret);
        ret = -EFAIL;
        goto fail_rx_cb_register;
    }

    /* mutex lock init */
    mutex_init(&g_hsdio_test_event->mutex_lock);

    init_completion(&g_hsdio_test_event->test_done);

    /* init hsdio_test param */
    g_hsdio_test_event->test_data.mode_idx = -1;
    g_hsdio_test_event->test_data.pkt_len  = 0;
    g_hsdio_test_event->test_data.pkt_sent = 0;
    g_hsdio_test_event->test_data.pkt_gen  = 0;
    g_hsdio_test_event->started  = FALSE;

    /* create workqueue */
    g_hsdio_test_event->test_workqueue = create_singlethread_workqueue("sdio_test");
    if (NULL == g_hsdio_test_event)
    {
    HWIFI_WARNING("work queue create fail.");
    ret =  -ENOMEM;
    goto fail_sdio_test_workqueue;
    }
    INIT_WORK(&g_hsdio_test_event->test_work, hsdio_test_work);

    return ret;
fail_sdio_test_workqueue:
    hwifi_rx_extern_unregister(HCC_TYPE_TEST);    
fail_rx_cb_register:
     kfree(g_hsdio_test_event);    
fail_g_hsdio_test_event:
     sysfs_remove_group(g_sysfs_hi110x, &hsdio_test_attribute_group);
    return ret;
}

void  hsdio_test_exit_module(void)
{
    if(g_hsdio_test_event->test_workqueue)
    {
        destroy_workqueue(g_hsdio_test_event->test_workqueue);
        g_hsdio_test_event->test_workqueue = NULL;
    }        
    sysfs_remove_group(g_sysfs_hi110x, &hsdio_test_attribute_group);
    hwifi_rx_extern_unregister(HCC_TYPE_TEST);
    kfree(g_hsdio_test_event);
    g_hsdio_test_event = NULL;
}
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

