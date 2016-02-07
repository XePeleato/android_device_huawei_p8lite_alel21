
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <linux/time.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/netdevice.h>

#include "hsdio.h"
#include "hi110x.h"
#include "hwifi_bus.h"
#include "hwifi_event.h"
#include "hwifi_cfg80211.h"
#include "hwifi_hcc.h"
#include "hwifi_perform.h"

struct wlan_perform_cfg_impl g_perform_cfg_info_object;
struct wlan_perform_cfg_impl* g_perform_cfg_info = &g_perform_cfg_info_object;

struct wlan_perform_statistic_impl g_perform_statistic_object;
struct wlan_perform_statistic_impl *g_perform_statistic = &g_perform_statistic_object;

#ifdef WMM_OPT_FOR_AUTH
uint32 g_wmm_drop_interval = DEFAULT_WMM_DROP_INTERVAL;
uint32 g_wmm_que_len_for_drop = DEFAULT_QUEUE_LEN_FOR_DROP;
uint32 g_wmm_drop_cnt_per_time = DEFAULT_DROP_CNT_PER_TIME;

#endif
#ifdef WLAN_PERFORM_DEBUG

void hwifi_perform_init_rx_statistic(void)
{
    atomic_set(&g_perform_statistic->rx_network_perform_pktcnt, 0);
    g_perform_statistic->rx_sdio_sum_buffer_len= 0;
    g_perform_statistic->rx_sdio_usecs= 0;

    g_perform_statistic->rx_net_sum_mpdu_len= 0;
    g_perform_statistic->rx_net_usecs= 0;
}


static void hwifi_perform_calc_rx_throughput(void)
{
    unsigned long         cur_jiffies;
    unsigned long         cur_usecs;
    unsigned long         sdio_rate;
    unsigned long         network_rate;
    unsigned long         total_bits;
    unsigned long         sdio_dur_us;
    unsigned long         net_dur_us;


    cur_jiffies = (unsigned long)get_jiffies_64();
    cur_usecs = (unsigned long)(jiffies_to_usecs(cur_jiffies));


    if (g_perform_cfg_info->sdio_statistic)
    {
        total_bits = (g_perform_statistic->rx_sdio_sum_buffer_len * 8);
        sdio_dur_us = (cur_usecs - g_perform_statistic->rx_sdio_usecs);
        sdio_rate = total_bits/sdio_dur_us;
        HWIFI_PERFORM("\n\n SDIO perform rx statistic: sdio_rate = %lu Mbps, [use time] = %lu us, sumlen = %lu B\n",sdio_rate, sdio_dur_us, g_perform_statistic->rx_sdio_sum_buffer_len);
    }

    if (g_perform_cfg_info->network_statistic)
    {
        total_bits = (g_perform_statistic->rx_net_sum_mpdu_len * 8);
        net_dur_us = (cur_usecs - g_perform_statistic->rx_net_usecs);
        network_rate = total_bits/net_dur_us;
        HWIFI_PERFORM("\n\n network perform rx statistic: network_rate = %lu Mbps, [use time] = %lu us, sumlen = %lu B\n",
                    network_rate, net_dur_us, g_perform_statistic->rx_net_sum_mpdu_len);
    }

    hwifi_perform_init_rx_statistic();
}



void hwifi_perform_rx_sdio_statistic(struct hi110x_device *hi110x_dev, uint32 len)
{
    unsigned long         jiffies;
    uint32                conn_status;

    HWIFI_ASSERT(NULL != hi110x_dev);
    if( g_perform_cfg_info->mpdu_pkt_num <= atomic_read(&g_perform_statistic->rx_network_perform_pktcnt))
    {
        if(g_perform_cfg_info->sdio_bypass)
        {
            hwifi_perform_calc_rx_throughput();
        }
    }

    conn_status = hi110x_dev->cfg->conn.status;
    if(HWIFI_CONNECTED == conn_status)
    {
        if(!g_perform_statistic->rx_sdio_sum_buffer_len)
        {
            jiffies = (unsigned long)get_jiffies_64();
            g_perform_statistic->rx_sdio_usecs = (unsigned long)(jiffies_to_usecs(jiffies));
            HWIFI_PERFORM("\n\nSDIO perform rx statistic start time: %lu\n\n", g_perform_statistic->rx_sdio_usecs);
        }
        /* SDIO ByPass情况下，统计计数加1，此时为SDIO计数 */
        if(g_perform_cfg_info->sdio_bypass)
        {
           atomic_inc(&g_perform_statistic->rx_network_perform_pktcnt);
        }
        g_perform_statistic->rx_sdio_sum_buffer_len += len;
    }
}
void hwifi_perform_rx_network_statistic(struct hi110x_device *hi110x_dev, uint32 len)
{
    unsigned long         jiffies;
    uint32                conn_status;

    if(g_perform_cfg_info->mpdu_pkt_num <= atomic_read(&g_perform_statistic->rx_network_perform_pktcnt))
    {
        hwifi_perform_calc_rx_throughput();
    }

    conn_status = hi110x_dev->cfg->conn.status;
    if(HWIFI_CONNECTED == conn_status)
    {
        if(!g_perform_statistic->rx_net_sum_mpdu_len)
        {
            jiffies = (unsigned long)get_jiffies_64();
            g_perform_statistic->rx_net_usecs   = (unsigned long)(jiffies_to_usecs(jiffies));
            HWIFI_PERFORM("\n\nnetwork perform rx statistic start time: %lu\n\n", g_perform_statistic->rx_net_usecs);
        }
        atomic_inc(&g_perform_statistic->rx_network_perform_pktcnt);
        g_perform_statistic->rx_net_sum_mpdu_len += len;
    }
}



void  hwifi_perform_rx_sdio_bypass(struct hi110x_sdio *hi_sdio, struct rxbuf_node  *rxbuf_node)
{
    uint32                 conn_status;

    HWIFI_ASSERT(NULL != hi_sdio);
    HWIFI_ASSERT(NULL != rxbuf_node);

    conn_status = hi_sdio->hi110x_dev->cfg->conn.status;
    if(HWIFI_CONNECTED == conn_status)
    {
        HWIFI_DEBUG("sdio_bypass: when connected sdio by pass");
        hi110x_sdio_put_rxbuf(hi_sdio, rxbuf_node);
    }
    else
    {
        /* Start of zhangwei 64406 2013-03-28 B295 同步优化 */
        #if 0
        if (test_and_set_bit(0, &hi_sdio->isr_work_state))
        {
            HWIFI_DEBUG("sdio_bypass: conn is not connected, work running");
        }
        else
        #endif
        {
            HWIFI_DEBUG("sdio_bypass: conn is not connected, start work");
            queue_work(hi_sdio->rx_process_wq, &hi_sdio->isr_work);
        }
        /* End of zhangwei 64406 2013-03-28 B295 同步优化 */
    }
}


void  hwifi_perform_rx_network_bypass(struct hi110x_device *hi110x_dev, struct sk_buff *skb)
{
    HWIFI_ASSERT(NULL != skb);
    dev_kfree_skb_any(skb);
}

void hwifi_perform_init_tx_statistic(void)
{
    atomic_set(&g_perform_statistic->tx_network_perform_pktcnt, 0);
    g_perform_statistic->tx_sdio_sum_buffer_len= 0;
    g_perform_statistic->tx_sdio_usecs= 0;

    g_perform_statistic->tx_work_sched_num = 0;
    g_perform_statistic->tx_sdio_sum_aggr_num = 0;

    g_perform_statistic->tx_net_sum_mpdu_len= 0;
    g_perform_statistic->tx_net_usecs= 0;
}

static void hwifi_perform_calc_tx_throughput(void)
{
    unsigned long         cur_jiffies;
    unsigned long         cur_usecs;
    unsigned long         sdio_rate;
    unsigned long         network_rate;
    unsigned long         total_bits;
    unsigned long         sdio_dur_us;
    unsigned long         net_dur_us;
    unsigned long         sdio_aggr_num;

    if (g_perform_cfg_info->network_statistic)
    {
        cur_jiffies = (unsigned long)get_jiffies_64();
        cur_usecs   = (unsigned long)(jiffies_to_usecs(cur_jiffies));
        total_bits = (g_perform_statistic->tx_net_sum_mpdu_len*8);
        net_dur_us = (cur_usecs - g_perform_statistic->tx_net_usecs);

        network_rate = total_bits/net_dur_us;
        HWIFI_PERFORM("\n\nnetwork perform tx statistic: network_rate = %lu Mbps, [use time] = %lu us, sumlen = %lu B\n",
                    network_rate, net_dur_us, g_perform_statistic->tx_net_sum_mpdu_len);
    }

    if ((g_perform_cfg_info->sdio_statistic)&&(g_perform_statistic->tx_sdio_sum_aggr_num))
    {
        cur_jiffies = (unsigned long)get_jiffies_64();
        cur_usecs   = (unsigned long)(jiffies_to_usecs(cur_jiffies));
        total_bits = (g_perform_statistic->tx_sdio_sum_buffer_len*8);
        sdio_dur_us = (cur_usecs - g_perform_statistic->tx_sdio_usecs);
        sdio_rate    = (total_bits/sdio_dur_us);
        sdio_aggr_num = (atomic_read(&g_perform_statistic->tx_network_perform_pktcnt)*10)/(g_perform_statistic->tx_sdio_sum_aggr_num);
        HWIFI_PERFORM("\n\nSDIO perform tx statistic: sdio_rate = %lu Mbps, 10*sdio_aggr is %lu, [use time] = %lu us, sumlen = %lu B\n",sdio_rate, sdio_aggr_num, sdio_dur_us, g_perform_statistic->tx_sdio_sum_buffer_len);
        sdio_aggr_num = (atomic_read(&g_perform_statistic->tx_network_perform_pktcnt)*10)/g_perform_statistic->tx_work_sched_num;
        HWIFI_PERFORM("sdio aggr number = %lu, tx_work_sched_num is %lu, data frame num is %d,10*sdio_aggr per work is %lu\n",g_perform_statistic->tx_sdio_sum_aggr_num,g_perform_statistic->tx_work_sched_num, atomic_read(&g_perform_statistic->tx_network_perform_pktcnt), sdio_aggr_num);
    }
    hwifi_perform_init_tx_statistic();
}

void  hwifi_perform_tx_network_bypass(struct sk_buff * skb)
{
    HWIFI_DEBUG("network_bypass: when connected network by pass");
    dev_kfree_skb_any(skb);
}


void hwifi_perform_tx_network_statistic(uint32 len)
{
    unsigned long         jiffies;

    if(g_perform_cfg_info->mpdu_pkt_num <= atomic_read(&g_perform_statistic->tx_network_perform_pktcnt))
    {
        if(g_perform_cfg_info->network_bypass)
        {
            hwifi_perform_calc_tx_throughput();
        }
    }

    if(!g_perform_statistic->tx_net_sum_mpdu_len)
    {
        jiffies = (unsigned long)get_jiffies_64();
        g_perform_statistic->tx_net_usecs   = (unsigned long)(jiffies_to_usecs(jiffies));
        HWIFI_PERFORM("\n\nnetwork perform tx statistic start time: %lu\n\n", g_perform_statistic->tx_net_usecs);
    }

    atomic_inc(&g_perform_statistic->tx_network_perform_pktcnt);
    g_perform_statistic->tx_net_sum_mpdu_len += len;
}


void  hwifi_perform_tx_sdio_bypass(unsigned char *buf, uint32 count)
{

}



void  hwifi_perform_tx_sdio_statistic(uint32 len)
{
    unsigned long        jiffies;

    if(g_perform_cfg_info->mpdu_pkt_num <= atomic_read(&g_perform_statistic->tx_network_perform_pktcnt))
    {
        hwifi_perform_calc_tx_throughput();
    }

    if(!g_perform_statistic->tx_sdio_sum_buffer_len)
    {
        jiffies = (unsigned long)get_jiffies_64();
        g_perform_statistic->tx_sdio_usecs   = (unsigned long)(jiffies_to_usecs(jiffies));
        HWIFI_DEBUG("\n\nSDIO perform tx statistic start time: %lu\n\n", g_perform_statistic->tx_sdio_usecs);
    }
    g_perform_statistic->tx_sdio_sum_buffer_len += len;
}

#endif


void hwifi_perform_init_rwtotal_param(void)
{
    atomic_set(&g_perform_statistic->rwtotal_perform_pktcnt, 0);
    g_perform_statistic->rwtotal_sdio_sum_buffer_len = 0;
    g_perform_statistic->rwtotal_sdio_usecs = 0;

    /* 添加一个2S的timer */
    mod_timer(&g_perform_cfg_info->statis_timer, jiffies + 2 * HZ);
}

void hwifi_perform_init_rwtotal_statistic(void)
{
    /**
     * 统计收发总的吞吐量
     * 1、按照500包流量统计一次
     * 2、起一个timer，如果没有500包，timer超时就开始统计
          timer的超时时间为2s
     */
    g_perform_cfg_info->total_sdio_pkt_num = PERFORM_TOTAL_SDIO_DEFAULT_NUM;
    init_timer(&g_perform_cfg_info->statis_timer);
    g_perform_cfg_info->statis_timer.function   = (void *)hwifi_perform_calc_rwtotal_throughput;

    g_perform_statistic->total_sdio_rate    = 0;
    g_perform_statistic->total_pps_rate     = 0;
    hwifi_perform_init_rwtotal_param();
}

void hwifi_perform_deinit_rwtotal_statistic(void)
{
    atomic_set(&g_perform_statistic->rwtotal_perform_pktcnt, 0);
    g_perform_statistic->rwtotal_sdio_sum_buffer_len = 0;
    g_perform_statistic->rwtotal_sdio_usecs = 0;
    g_perform_statistic->total_sdio_rate    = 0;
    g_perform_statistic->total_pps_rate     = 0;
    del_timer_sync(&g_perform_cfg_info->statis_timer);
}


void  hwifi_perform_rwtotal_sdio_statistic(uint32 len, uint32 pkt_cnt)
{
    unsigned long        jiffies;

    if(g_perform_cfg_info->total_sdio_pkt_num <= atomic_read(&g_perform_statistic->rwtotal_perform_pktcnt))
    {
        hwifi_perform_calc_rwtotal_throughput(len);
    }

    if(!g_perform_statistic->rwtotal_sdio_sum_buffer_len)
    {
        jiffies = (unsigned long)get_jiffies_64();
        g_perform_statistic->rwtotal_sdio_usecs= (unsigned long)(jiffies_to_usecs(jiffies));
        HWIFI_DEBUG("\n\nSDIO perform rw total statistic start time: %lu\n\n", g_perform_statistic->rwtotal_sdio_usecs);
    }

    atomic_add(pkt_cnt, &g_perform_statistic->rwtotal_perform_pktcnt);
    g_perform_statistic->rwtotal_sdio_sum_buffer_len += len;
}


void hwifi_perform_calc_rwtotal_throughput(uint32 len)
{
    unsigned long         cur_jiffies;
    unsigned long         cur_usecs;
    unsigned long         sdio_rate;
    unsigned long         pps_rate;
    unsigned long         total_bits;
    unsigned long         sdio_dur_us;
    unsigned int          pps_cal_cnt;

    pps_cal_cnt = atomic_read(&g_perform_statistic->rwtotal_perform_pktcnt);
    cur_jiffies = (unsigned long)get_jiffies_64();
    cur_usecs   = (unsigned long)(jiffies_to_usecs(cur_jiffies));
    total_bits  = ((g_perform_statistic->rwtotal_sdio_sum_buffer_len) * 8);
    sdio_dur_us = (cur_usecs - (g_perform_statistic->rwtotal_sdio_usecs));
    sdio_rate   = total_bits/(sdio_dur_us?:1);
    pps_rate    = (pps_cal_cnt*1000000)/(sdio_dur_us?:1);

    g_perform_statistic->total_sdio_rate = sdio_rate;
    g_perform_statistic->total_pps_rate  = pps_rate;
    HWIFI_PERFORM("\n\nSDIO perform tx statistic:\
                    sdio_rate = %lu Mbps,\
                    [use time] = %lu us,\
                    sumlen = %lu B,\
                    pps_rate = %lu p/s\n",
    sdio_rate, sdio_dur_us, g_perform_statistic->rwtotal_sdio_sum_buffer_len, g_perform_statistic->total_pps_rate);

    hwifi_perform_init_rwtotal_param();
}


#ifdef WLAN_PERFORM_OPT
char* hwifi_perform_get_tcp_type_str(struct sk_buff *skb)
{
    struct skb_tcp_cb  *tcp_cb       = NULL;
    HWIFI_BUG_ON(!skb);
    tcp_cb = (struct skb_tcp_cb  *)skb->cb;
    if(TCP_ACK_FILTER_TYPE == tcp_cb->tcp_type)
    {
        return "tcp ack";
    }
    else if(DUP_TCP_ACK_TYPE == tcp_cb->tcp_type)
    {
        return "tcp dumplicate ack";
    }
    else
    {
        return "unkown tcp type";
    }
}

void hiwifi_perform_ack_count_reset(struct hcc_handler *hcc,hcc_chan_type dir, int stream)
{
    HWIFI_BUG_ON(!hcc);
    spin_lock_bh(&hcc->hcc_tcp_ack[dir].hcc_tcp_ack_lock);
    hcc->hcc_tcp_ack[dir].hcc_ack_count[stream] = 0;
    hcc->hcc_tcp_ack[dir].hcc_duplicate_ack_count[stream] = 0;
    spin_unlock_bh(&hcc->hcc_tcp_ack[dir].hcc_tcp_ack_lock);
}

struct tcp_list_node *hwifi_perform_find_oldest_node(struct hcc_handler *hcc, hcc_chan_type dir)
{
    struct tcp_list_node *node,*next;
    struct tcp_list_node *oldest_node = NULL;
    unsigned long oldest_time = jiffies;  /*init current time*/
    struct wlan_perform_tcp_list *tmp_list;
    tmp_list = &hcc->hcc_tcp_ack_list[dir];
    list_for_each_entry_safe(node, next, &tmp_list->tcp_list, list)
    {
        if (TRUE == time_before_eq((unsigned long)node->last_ts, (unsigned long)oldest_time))
        {
            oldest_time = node->last_ts;
            oldest_node = node;
            HWIFI_PR_DEBUG("%s find last_ts %lu   oldest_time %lu", dir?"down":"up",
                            (unsigned long)node->last_ts,
                            (unsigned long)oldest_time);
        }
    }
    if (NULL != oldest_node)
    list_del_init(&oldest_node->list);
    else
    HWIFI_ERROR("can't find oldest node xx");
    return oldest_node;
}
/*
 * Prototype    : hwifi_perform_get_buf
 * Description  : get buf for tcp ack
 * Input        : None
 * Output       : None
 * Return Value : struct
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/2
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
struct tcp_list_node *hwifi_perform_get_buf(struct hcc_handler *hcc, hcc_chan_type dir)
{
    struct tcp_list_node *node = NULL;
    int                   i;
    struct wlan_perform_tcp_list *tmp_ack_list = &hcc->hcc_tcp_ack_list[dir];

    if (0 == tmp_ack_list->free_count)
    {
        node = hwifi_perform_find_oldest_node(hcc,dir);
        return node;
    }

    for (i = 0; i < HCC_TCP_STREAM; i++)
    {
        if ( 0 == tmp_ack_list->tcp_pool[i].used)
        {
            tmp_ack_list->tcp_pool[i].used = 1;
            tmp_ack_list->free_count--;
            HWIFI_PR_DEBUG("%s get buf %d free:%d", hcc_get_dir_str(dir), i, tmp_ack_list->free_count);
            node = &tmp_ack_list->tcp_pool[i];
            break;
        }
    }
    return node;
}

/*
 * Prototype    : hwifi_perform_free_buf
 * Description  : free buf to tcp buf pool
 * Input        : struct hcc_handler *hcc
                  struct tcp_list_node *node
                  uint8 dir
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/2
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void hwifi_perform_free_buf(struct hcc_handler *hcc, struct tcp_list_node *node, hcc_chan_type dir)
{
    hcc->hcc_tcp_ack[dir].hcc_tcp_ack_no[node->index] = 0;
    node->used = 0;

    hcc->hcc_tcp_ack_list[dir].free_count++;

    HWIFI_PR_DEBUG("%s perform free:%d", hcc_get_dir_str(dir),
                                         hcc->hcc_tcp_ack_list[dir].free_count);
}

/*
 * Prototype    : hwifi_perform_add_node
 * Description  : add node to ack list
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/26
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_perform_add_node(struct hcc_handler *hcc, struct wlan_tcp_flow *tcp_info, hcc_chan_type dir)
{
    struct tcp_list_node *node;

    if ((NULL == hcc) || (NULL == tcp_info))
    {
        return -EFAIL;
    }

    node = hwifi_perform_get_buf(hcc,dir);

    if (NULL == node)
    {
        HWIFI_ERROR("Invalid NULL node!");
        return -EFAIL;
    }

    node->buf->dst_ip   = tcp_info->dst_ip;
    node->buf->src_ip   = tcp_info->src_ip;
    node->buf->src_port = tcp_info->src_port;
    node->buf->dst_port = tcp_info->dst_port;
    node->buf->protocol = tcp_info->protocol;
    node->last_ts       = jiffies;

    list_add_tail(&node->list, &hcc->hcc_tcp_ack_list[dir].tcp_list);

    HWIFI_PR_DEBUG("%s:add node succ", hcc_get_dir_str(dir));

    return node->index;
}


int32 hwifi_perform_init_filter_tcp_ack_pool(struct hcc_handler* hcc)
{
    int     i = 0;
    int      tcp_index;

    HWIFI_ASSERT(NULL != hcc);

    /*init downline tcp ack pool*/

    /*init tx_worker_state*/

    for(i = 0; i < HCC_DIR_COUNT; i++)
    {
        for (tcp_index = 0; tcp_index < HCC_TCP_STREAM; tcp_index++)
        {
            spin_lock_init(&hcc->hcc_tcp_ack[i].hcc_tcp_ack_lock);
            skb_queue_head_init(&hcc->hcc_tcp_ack[i].hcc_ack_queue[tcp_index]);
            hcc->hcc_tcp_ack_list[i].tcp_pool[tcp_index].buf =
                        (struct wlan_tcp_flow *)kzalloc(sizeof(struct wlan_tcp_flow), GFP_KERNEL);
            if(!hcc->hcc_tcp_ack_list[i].tcp_pool[tcp_index].buf)
            {
                HWIFI_ERROR("alloc %s failed strem :%d", hcc_get_dir_str(i), tcp_index);
                hwifi_perform_deinit_list(hcc);
                return -EFAIL;
            }
            hcc->hcc_tcp_ack_list[i].tcp_pool[tcp_index].used = 0;
            hcc->hcc_tcp_ack_list[i].tcp_pool[tcp_index].index = tcp_index;
            hcc->hcc_tcp_ack_list[i].tcp_pool[tcp_index].last_ts = jiffies;
        }
        INIT_LIST_HEAD(&hcc->hcc_tcp_ack_list[i].tcp_list);
        hcc->hcc_tcp_ack_list[i].free_count= HCC_TCP_STREAM;
        HWIFI_INFO("wifi tcp perform %s init done.", hcc_get_dir_str(i));
    }

    return SUCC;
}


/*
 * Prototype    : hwifi_perform_deinit_list
 * Description  : deinit tcp ack list
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/26
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void hwifi_perform_deinit_list(struct hcc_handler *hcc)
{
    int i;
    int dir;
    for(dir = 0; dir < HCC_DIR_COUNT; dir++)
    {
        for(i = 0; i < HCC_TCP_STREAM; i++)
        {
            if (NULL != hcc->hcc_tcp_ack_list[dir].tcp_pool[i].buf)
            {
                kfree(hcc->hcc_tcp_ack_list[dir].tcp_pool[i].buf);
                hcc->hcc_tcp_ack_list[dir].tcp_pool[i].buf = NULL;
            }
        }
    }
}

/*
 * Prototype    : hwifi_perform_get_flow_index
 * Description  : get follow index for each tcp ack
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/25
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_perform_get_flow_index(struct hcc_handler     *hcc,
                                             struct wlan_ip_header  *ip_hdr,
                                             struct wlan_tcp_header *tcp_hdr,
                                             hcc_chan_type dir)
{
    struct wlan_tcp_flow   tcp_flow_info;
    struct tcp_list_node  *node;
    struct list_head *tmp_tcp_list;
    int index = -EFAIL;

    tcp_flow_info.src_ip   = WLAN_PERFORM_GET_BE32(ip_hdr->ip_sip);
    tcp_flow_info.dst_ip   = WLAN_PERFORM_GET_BE32(ip_hdr->ip_dip);
    tcp_flow_info.src_port = WLAN_PERFORM_GET_BE16(tcp_hdr->tcp_sport);
    tcp_flow_info.dst_port = WLAN_PERFORM_GET_BE16(tcp_hdr->tcp_dport);
    tcp_flow_info.protocol = ip_hdr->ip_protocol;

    tmp_tcp_list = &hcc->hcc_tcp_ack_list[dir].tcp_list;

    /* get the queue index of tcp ack */
    list_for_each_entry(node, tmp_tcp_list, list)
    {
        if ((node->buf->src_ip    == tcp_flow_info.src_ip)
           && (node->buf->dst_ip   == tcp_flow_info.dst_ip)
           && (node->buf->src_port == tcp_flow_info.src_port)
           && (node->buf->dst_port == tcp_flow_info.dst_port)
           && (node->buf->protocol == tcp_flow_info.protocol)
           && (node->used == 1))
        {
            HWIFI_PR_DEBUG("%s:find the same tcp flow info [index:%d]",
                            hcc_get_dir_str(dir),node->index);
            node->last_ts = jiffies; /*renew the last pkt time*/
            return node->index;
        }
    }

    /* new follow, save in new node */
    index = hwifi_perform_add_node(hcc, &tcp_flow_info, dir);

    return index;

}

/*
 * Prototype    : hwifi_perform_get_tcp_ack
 * Description  : get tcp ack from the skb data
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/23
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32  hwifi_perform_get_tcp_ack(struct sk_buff *skb, struct hcc_handler *hcc, int *index, uint8 dir)
{
    struct wlan_ip_header  *ip_hdr;
    struct wlan_tcp_header *tcp_hdr;
    struct skb_tcp_cb  *tcp_cb  = NULL;
    int32  ip_pkt_len;
    int32  ip_hdr_len;
    int32  tcp_hdr_len;
    uint32 tcp_ack_no;
    uint8 *pos;
    struct wlan_ethmac_header *eth_hdr;
    uint16 ip_type;
    uint32 *tmp_tcp_ack_no;
    struct hcc_header  *header;

    header = (struct hcc_header*)skb->data;

    /* remove hcc header */
    pos = skb->data + sizeof(struct hcc_header) +  header->pad + (header->option ? HCC_ASSEMBLE_INFO_SIZE : 0);

    tcp_cb = (struct skb_tcp_cb*)skb->cb;

    eth_hdr = (struct wlan_ethmac_header *)pos;
    ip_type = ((eth_hdr->mac_protocol[0] << 8) | (eth_hdr->mac_protocol[1]));
    if (IP_TYPE != ip_type)
    {
        HWIFI_WARNING("**not ip packet return direct**");
        return -EFAIL;
    }

    pos    += sizeof(struct wlan_ethmac_header);
    ip_hdr  = (struct wlan_ip_header *)pos;

    /*option1:非tcp帧时不过滤*/
    if (TCP_PROTOCOL != ip_hdr->ip_protocol)
    {
        /* not tcp data */
        HWIFI_WARNING("**not tcp packet return direct**");
        return -EFAIL;
    }

    pos       += sizeof(struct wlan_ip_header);
    tcp_hdr    = (struct wlan_tcp_header *)pos;
    ip_pkt_len  = WLAN_PERFORM_GET_BE16(ip_hdr->ip_length);
    ip_hdr_len     = LOW_HALF_BYTE(ip_hdr->ip_version) * 4;
    tcp_hdr_len    = HIGH_HALF_BYTE(tcp_hdr->tcp_offset)* 4;
    tcp_ack_no = WLAN_PERFORM_GET_BE32(tcp_hdr->tcp_acknum);

    /*option2:Tcp帧头后携带数据时不过滤*/
    if ((ip_pkt_len - tcp_hdr_len) != ip_hdr_len)
    {
        /* ack with data */
        HWIFI_WARNING("**tcp ack packet with data, return direct**");
        return -EFAIL;
    }

    /*option3:SYN FIN RST URG有为1的时候不过滤*/
    if ((tcp_hdr->tcp_flags) & FILTER_FLAG_MASK)
    {
        HWIFI_PR_DEBUG("**specific tcp pkt, can't be filter**");
        return -EFAIL;
    }

    /*option4:flow index取不到时不过滤*/
   *index = hwifi_perform_get_flow_index(hcc, ip_hdr, tcp_hdr, dir);
    if(-EFAIL == *index)
    {
        return -EFAIL;
    }
    HWIFI_WARN_ON(*index >= HCC_TCP_STREAM);
    tcp_cb->index    = *index;
    tcp_cb->tcp_ack_no = tcp_ack_no;

    /*检测duplicat ack是否存在，如果存在则累计ack流最大成员数*/
    tmp_tcp_ack_no = hcc->hcc_tcp_ack[dir].hcc_tcp_ack_no;
    HWIFI_PR_DEBUG("stream:%d ack no:%u  tcp ack no:%u", *index, tcp_ack_no, tmp_tcp_ack_no[*index]);
    if (tcp_ack_no == tmp_tcp_ack_no[*index])
    {
        HWIFI_DEBUG("%s:**duplicate ack is coming**", hcc_get_dir_str(dir));
        tcp_cb->tcp_type  = DUP_TCP_ACK_TYPE;
        hcc->hcc_queues[dir].filter_info.tcp_info[*index].dup_ack_count++;
        return SUCC;
    }

    tmp_tcp_ack_no[*index] = WLAN_PERFORM_GET_BE32(tcp_hdr->tcp_acknum);

    /*赋值tcp_cb*/
    tcp_cb->tcp_type = TCP_ACK_FILTER_TYPE;

    return SUCC;
}

int hwifi_perform_tcp_ack_list_filter(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, struct sk_buff_head  *head)
{
    struct tcp_list_node *node,*next;
    struct sk_buff * skb;
    struct sk_buff_head  head_t;
    spinlock_t*  data_queue_lock;
    HWIFI_BUG_ON(!hcc);
    if(!skb_queue_len(head))
    {
        return 0;
    }
    skb_queue_head_init(&head_t);

    HWIFI_PR_DEBUG("%s:filter queue qlen %u", hcc_get_dir_str(dir), skb_queue_len(head));
    while(!!(skb = skb_dequeue(head)))
    {
        if(hwifi_perform_tcp_ack_filter(skb, hcc, dir))
        {
            HWIFI_PR_DEBUG("not found tcp ack...");
            skb_queue_tail(&head_t, skb);
        }
        else
        {
            HWIFI_PR_DEBUG("found tcp ack...");
        }
    }
    HWIFI_WARN_ON(!skb_queue_empty(head));
    skb_queue_splice_init(&head_t, head);
    hcc->hcc_queues[dir].filter_info.ignored_count +=
                skb_queue_len(head);
    data_queue_lock = &hcc->hcc_queues[dir].queues[type].data_queue_lock;
    list_for_each_entry_safe(node, next, &hcc->hcc_tcp_ack_list[dir].tcp_list, list)
    {
        struct     sk_buff_head* hcc_ack_queue;
        HWIFI_PR_DEBUG("%s:--begin to recv packet--", hcc_get_dir_str(dir));
        if (node->used == 0)
        {
            continue;
        }
        hcc_ack_queue = &hcc->hcc_tcp_ack[dir].hcc_ack_queue[node->index];

        spin_lock_bh(data_queue_lock);
        HWIFI_PR_DEBUG("%s: ------recv packet------[qlen:%u]", hcc_get_dir_str(dir),
                    skb_queue_len(hcc_ack_queue));
        hiwifi_perform_ack_count_reset(hcc, dir, node->index);
        hcc->hcc_queues[dir].filter_info.tcp_info[node->index].send_count +=
                skb_queue_len(hcc_ack_queue);

        skb_queue_splice_tail_init(hcc_ack_queue, head);
        spin_unlock_bh(data_queue_lock);

        list_del_init(&node->list);

        hwifi_perform_free_buf(hcc, node, dir);

    }
    return 0;
}

int hwifi_perform_rx_tcp_ack_filter(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data)
{
    struct sk_buff_head  *head = (struct sk_buff_head  *)data;
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!data);
    HWIFI_BUG_ON(DATA_TCP_ACK_QUEUE != type);
    HWIFI_BUG_ON(HCC_RX != dir);

    if(!hcc->sys_tcp_rx_ack_opt_enable)
    {
        return 0;
    }
    return hwifi_perform_tcp_ack_list_filter(hcc, type, dir, head);
}

int hwifi_perform_tx_tcp_ack_filter(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data)
{
    struct sk_buff_head  *head = (struct sk_buff_head  *)data;
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!data);
    HWIFI_BUG_ON(DATA_TCP_ACK_QUEUE != type);
    HWIFI_BUG_ON(HCC_TX != dir);

    if(!hcc->sys_tcp_tx_ack_opt_enable)
    {
        return 0;
    }
    return hwifi_perform_tcp_ack_list_filter(hcc, type, dir, head);
}


/*
 * Prototype    : hwifi_perform_tcp_ack_filter
 * Description  : save it to tcp ack queue
 * Input        : uint8 *buf
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/23
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_perform_tcp_ack_filter(struct sk_buff *skb, struct hcc_handler *hcc, hcc_chan_type dir)
{

    int ret;
    int index;
    int32 ack_limit;
    struct     sk_buff_head* hcc_ack_queue;
    struct sk_buff     *ack          = NULL;
    struct skb_tcp_cb  *tcp_cb       = NULL;
    struct skb_tcp_cb  *temp_tcp_cb  = NULL;

    ret = hwifi_perform_get_tcp_ack(skb, hcc, &index, dir);
    if (ret != SUCC)
    {
        return -EFAIL;
    }

    tcp_cb = (struct skb_tcp_cb*)skb->cb;

    HWIFI_PR_DEBUG("%s:filter ack[%s]-----------",
        hcc_get_dir_str(dir), hwifi_perform_get_tcp_type_str(skb));


    spin_lock_bh(&hcc->hcc_tcp_ack[dir].hcc_tcp_ack_lock);
    ack_limit = hcc->hcc_queues[dir].filter_info.ack_limit;
    hcc_ack_queue = &hcc->hcc_tcp_ack[dir].hcc_ack_queue[index];
    #if 0
    /*当收到正常ack时，清除所有dup ack*/
    if (TCP_ACK_FILTER_TYPE == tcp_cb->tcp_type)
    {
        hcc->hcc_queues[dir].filter_info.tcp_info[index].drop_count += skb_queue_len(hcc_ack_queue);
        HWIFI_PR_DEBUG("%s:------drop dup packet------qlen:%u",
                        hcc_get_dir_str(dir),skb_queue_len(hcc_ack_queue));
        /*hcc_trans_pkt_count_sub(hcc, dir, skb_queue_len(hcc_ack_queue));*/
        skb_queue_purge(hcc_ack_queue);
        hcc->hcc_tcp_ack[dir].hcc_ack_count[index] = 0;
        hcc->hcc_tcp_ack[dir].hcc_duplicate_ack_count[index] = 0;
    }
    /*FIXEME? TBD:warning ack_limit did not effect*/
    /* if more than g_tx_tcp_count, drop the earlist one, then add new one; else add new one directly */
    #endif

    /*if normal ack received, del until ack_limit ack left*/
    if (TCP_ACK_FILTER_TYPE == tcp_cb->tcp_type)
    {
        while (hcc->hcc_tcp_ack[dir].hcc_ack_count[index] >= ack_limit)
        {
            HWIFI_PR_DEBUG("%s:stream:%d : ack count:%u, qlen:%u", hcc_get_dir_str(dir), index,
                hcc->hcc_tcp_ack[dir].hcc_ack_count[index],
                skb_queue_len(hcc_ack_queue));
            ack = skb_dequeue(hcc_ack_queue);
            if (NULL == ack)
            {
                break;
            }
            HWIFI_DEBUG("%s ------drop packet------", hcc_get_dir_str(dir));
            hcc->hcc_queues[dir].filter_info.tcp_info[index].drop_count++;
            temp_tcp_cb = (struct skb_tcp_cb*)ack->cb;
            if (TCP_ACK_FILTER_TYPE == temp_tcp_cb->tcp_type)
            {
                hcc->hcc_tcp_ack[dir].hcc_ack_count[index]--;
            }
            else if (DUP_TCP_ACK_TYPE == temp_tcp_cb->tcp_type)
            {
                hcc->hcc_tcp_ack[dir].hcc_duplicate_ack_count[index]--;
            }
            /*hcc_trans_pkt_count_dec(hcc,dir);*/   /*如果TCP ACK被丢弃，则计数器做自减*/
            HWIFI_PR_DEBUG("%s: ack count:%d , dupcount:%d ", hcc_get_dir_str(dir),
                            hcc->hcc_tcp_ack[dir].hcc_ack_count[index],
                            hcc->hcc_tcp_ack[dir].hcc_duplicate_ack_count[index]);
            dev_kfree_skb_any(ack);
        }
    }
    HWIFI_PR_DEBUG("%s: stream:%d qlen:%u", hcc_get_dir_str(dir),
                index, skb_queue_len(hcc_ack_queue));

    skb_queue_tail(hcc_ack_queue, skb);

    if (TCP_ACK_FILTER_TYPE == tcp_cb->tcp_type)
    {
        hcc->hcc_tcp_ack[dir].hcc_ack_count[index]++;
    }
    else if (DUP_TCP_ACK_TYPE == tcp_cb->tcp_type)
    {
        hcc->hcc_tcp_ack[dir].hcc_duplicate_ack_count[index]++;
    }
    spin_unlock_bh(&hcc->hcc_tcp_ack[dir].hcc_tcp_ack_lock);

    return SUCC;
}

/*
 * Prototype    : hiwifi_perform_para_init
 * Description  : init perform param
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/5/8
 *     Author       : houyin
 *     Modification : Created function
 *
 */
void hiwifi_perform_para_init(struct hi110x_sdio* hi_sdio)
{
    /*默认打开下行TCP ACK过滤*/
    g_perform_cfg_info->tx_ack_num    = 1;
}

#endif

#ifdef WMM_OPT_FOR_AUTH
int hiwifi_perform_wmm_ac_filter(struct hcc_handler *hcc, hcc_queue_type type, hcc_chan_type dir, struct sk_buff_head  *head)
{
    struct sk_buff *skb;
    spinlock_t*  lock;
    uint8 drop_ratio = 0;
    uint8 i;
    uint32 drop_len;
    uint8 current_ac_idx = type - DATA_UDP_BK_QUEUE;

    /*if flow ctl now, do not drop*/
    if (g_flowctrl_flag != FLOWCTRL_ON)
    {
        return 0;
    }

    /*if the cnt of tx ac is only one, then not drop*/
    if (1 == hcc->wmm_info.current_ac_cnt)
    {
        return 0;
    }

    for (i = 0; i < RX_AC_BK; i++)
    {
        if (((((hcc->wmm_info.ac_bitmap) >> i) & 1 ) || (((hcc->wmm_info.ac_bitmap) >> (i + RX_AC_BK)) & 1))
                && (i > current_ac_idx))
        {
            drop_ratio++;
        }
    }

    /*don't drop lower prior ac in the first 10S */
    if ((FALSE == hcc->wmm_info.wmm_high_prio_ac_drop_switch) && (0 != drop_ratio))
        return 0;

    /*don't drop higher prior ac in the first 10S */
    if ((TRUE == hcc->wmm_info.wmm_high_prio_ac_drop_switch) && (!drop_ratio))
        return 0;

    lock = &hcc->hcc_queues[dir].queues[type].data_queue_lock;

    spin_lock_bh(lock);
    drop_len = skb_queue_len(head);

    if (drop_len > g_wmm_que_len_for_drop)
    {
        if (g_wmm_drop_interval == hcc->wmm_info.wmm_ac_drop_flag[current_ac_idx])
        {
            drop_len = MIN(g_wmm_drop_cnt_per_time, drop_len);
            hcc->wmm_info.wmm_ac_drop_flag[current_ac_idx] = 0;
        }
        else
        {
            hcc->wmm_info.wmm_ac_drop_flag[current_ac_idx]++;   /*make sure drop one in g_wmm_drop_interval*/
            spin_unlock_bh(lock);
            return 0;
        }
    }
    else
    {
        spin_unlock_bh(lock);
        return 0;
    }

    /*drop pkt*/
    for(i = 0; i < drop_len; i++)
    {
        skb = skb_dequeue_tail(head);

        if (NULL != skb)
        {
            dev_kfree_skb(skb);
        }
    }
    /*hcc_trans_pkt_count_sub(hcc, dir, drop_len);*/
    spin_unlock_bh(lock);

    return 0;
}

int hwifi_perform_wmm_ac_filter_reg(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data)
{
    struct sk_buff_head  *head = (struct sk_buff_head  *)data;
    HWIFI_BUG_ON(!hcc);
    HWIFI_BUG_ON(!data);

    if(!hcc->sys_wmm_ac_opt_enable)
    {
        return 0;
    }

    if ((type > DATA_UDP_VO_QUEUE) || (type < DATA_UDP_BK_QUEUE))
        return 0;

    return hiwifi_perform_wmm_ac_filter(hcc, type, dir, head);
}

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


