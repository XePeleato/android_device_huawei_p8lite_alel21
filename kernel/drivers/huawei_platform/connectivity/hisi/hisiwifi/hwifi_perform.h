

#ifndef __HWIFI_PERFORM_H__
#define __HWIFI_PERFORM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include <linux/skbuff.h>
#include "common.h"
#include "hwifi_hcc.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#ifdef WLAN_PERFORM_OPT
#define WLAN_PERFORM_GET_BE16(a) ((uint16) (((a)[0] << 8) | (a)[1]))
#define WLAN_PERFORM_GET_BE32(a) ((((uint32) (a)[0]) << 24) | (((uint32) (a)[1]) << 16) | \
                                 (((uint32) (a)[2]) << 8) | ((uint32) (a)[3]))
#define HIGH_HALF_BYTE(a) (((a) & 0xF0) >> 4)
#define LOW_HALF_BYTE(a)  ((a) & 0x0F)
#define IP_TYPE          0x0800

#define TCP_PROTOCOL      0x06
#define UDP_PROTOCOL      0x11

#define FIN_FLAG_BIT    (1 << 0)
#define SYN_FLAG_BIT    (1 << 1)
#define RESET_FLAG_BIT  (1 << 2)
#define URGENT_FLAG_BIT (1 << 5)
#define FILTER_FLAG_MASK (FIN_FLAG_BIT | SYN_FLAG_BIT | RESET_FLAG_BIT | URGENT_FLAG_BIT)
#define MAX_TIME_BY_TWO (1 << 16)
#endif

#define PERFORM_TOTAL_SDIO_DEFAULT_NUM  1000

#define UNDEFINED_STREAM_INDEX  (0xFFFFFFFF)
#define WIFI_TCP_STREAM_GET(skb) (((struct skb_tcp_cb  *)skb->cb)->index)
#define WIFI_TCP_NO_GET(skb) (((struct skb_tcp_cb  *)skb->cb)->tcp_ack_no)

#ifdef WMM_OPT_FOR_AUTH
#define DEFAULT_WMM_DROP_INTERVAL  10
#define DEFAULT_QUEUE_LEN_FOR_DROP 40
#define DEFAULT_DROP_CNT_PER_TIME  100
#endif
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum
{
    TCP_ACK_FILTER_TYPE  = 1,
    DUP_TCP_ACK_TYPE,
    TCP_TYPE_COUNT
};

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
#ifdef WMM_OPT_FOR_AUTH
extern  uint32 g_wmm_drop_interval;
extern  uint32 g_wmm_que_len_for_drop;
extern  uint32 g_wmm_drop_cnt_per_time;
#endif

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
struct hcc_handler;
struct hi110x_device;
struct hi110x_sdio;
struct rxbuf_node;

#ifdef WLAN_PERFORM_OPT
struct wlan_ethmac_header
{
   uint8   mac_ra[MAC_LEN];
   uint8   mac_ta[MAC_LEN];
   uint8   mac_protocol[2];
};

struct wlan_ip_header
{
  uint8   ip_version;
  uint8   ip_diffservice;
  uint8   ip_length[2];
  uint8   ip_indentifier[2];
  uint8   ip_fragflag[2];
  uint8   ip_live;
  uint8   ip_protocol;
  uint8   ip_check[2];
  uint8   ip_sip[4];
  uint8   ip_dip[4];
};

struct wlan_udp_header{
   uint8  udp_sport[2];
   uint8  udp_dport[2];
   uint8  udp_length[2];
   uint8  udp_check[2];
};

struct wlan_tcp_header
{
   uint8  tcp_sport[2];
   uint8  tcp_dport[2];
   uint8  tcp_seqnum[4];
   uint8  tcp_acknum[4];
   uint8  tcp_offset;
   uint8  tcp_flags;
   uint8  tcp_window[2];
   uint8  tcp_check[2];
   uint8  tcp_urgent[2];
};

#endif



/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
#ifdef WLAN_PERFORM_DEBUG
extern void hwifi_perform_rx_network_bypass(struct hi110x_device *hi110x_dev, struct sk_buff *skb);
extern void hwifi_perform_rx_sdio_bypass(struct hi110x_sdio *hi_sdio, struct rxbuf_node  *rxbuf_node);
extern void hwifi_perform_rx_network_statistic(struct hi110x_device *hi110x_dev, uint32 len);
extern void hwifi_perform_rx_sdio_statistic(struct hi110x_device *hi110x_dev, uint32 len);
extern void hwifi_perform_init_rx_statistic(void);

extern void hwifi_perform_init_tx_statistic(void);
extern void hwifi_perform_tx_network_bypass(struct sk_buff * skb);
extern void hwifi_perform_tx_sdio_bypass(unsigned char *buf, uint32 count);
extern void hwifi_perform_tx_network_statistic(uint32 len);
extern void hwifi_perform_tx_sdio_statistic(uint32 len);
#endif

extern void  hwifi_perform_rwtotal_sdio_statistic(uint32 len, uint32 pkt_cnt);

extern void hwifi_perform_init_rwtotal_statistic(void);
extern void hwifi_perform_deinit_rwtotal_statistic(void);
extern void hwifi_perform_calc_rwtotal_throughput(uint32 len);
#ifdef WLAN_PERFORM_OPT
extern void hwifi_perform_deinit_list(struct hcc_handler *hcc);
extern int32 hwifi_perform_tcp_ack_filter(struct sk_buff *skb, struct hcc_handler *hcc, hcc_chan_type dir);
extern void hwifi_perform_tcp_ack_tx(struct hcc_handler *hcc, hcc_queue_type type, hcc_chan_type dir);
extern int32 hwifi_perform_init_filter_tcp_ack_pool(struct hcc_handler* hcc);
extern int32 hwifi_perform_init_tcp_ack_info(void);
extern void hiwifi_perform_para_init(struct hi110x_sdio* hi_sdio);

extern change_sched_mode hiwifi_perform_change_sched_mode(struct sk_buff *skb, struct hcc_handler *hcc);
int32  hwifi_perform_get_tcp_ack_info( struct hcc_handler *hcc,struct sk_buff *skb, uint8 dir);
int hwifi_perform_rx_tcp_ack_filter(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data);
int hwifi_perform_tx_tcp_ack_filter(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data);

static inline unsigned long long hwifi_perform_get_tcp_info_dup_count(struct hcc_handler* hcc, hcc_chan_type dir)
{
    int i;
    unsigned long long total_count = 0;
    HWIFI_BUG_ON(!hcc);
    for(i = 0; i < HCC_TCP_STREAM; i++)
    {
        total_count += hcc->hcc_queues[dir].filter_info.tcp_info[i].dup_ack_count;
    }
    return total_count;
}

static inline unsigned long long hwifi_perform_get_tcp_info_drop_count(struct hcc_handler* hcc, hcc_chan_type dir)
{
    int i;
    unsigned long long total_count = 0;
    HWIFI_BUG_ON(!hcc);
    for(i = 0; i < HCC_TCP_STREAM; i++)
    {
        total_count += hcc->hcc_queues[dir].filter_info.tcp_info[i].drop_count;
    }
    return total_count;
}

static inline unsigned long long hwifi_perform_get_tcp_info_send_count(struct hcc_handler* hcc, hcc_chan_type dir)
{
    int i;
    unsigned long long total_count = 0;
    HWIFI_BUG_ON(!hcc);
    for(i = 0; i < HCC_TCP_STREAM; i++)
    {
        total_count += hcc->hcc_queues[dir].filter_info.tcp_info[i].send_count;
    }
    return total_count + hcc->hcc_queues[dir].filter_info.ignored_count;
}

#endif
#ifdef WMM_OPT_FOR_AUTH
extern int hwifi_perform_wmm_ac_filter_reg(struct hcc_handler* hcc, hcc_queue_type type,hcc_chan_type dir, void* data);

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
