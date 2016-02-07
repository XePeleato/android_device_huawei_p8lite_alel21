/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 *
 * File Name     : hwifi_command.c
 * Version       : Initial Draft
 * Author        : Hash
 * Created       : 2012/9/21
 * Last Modified :
 * Description   : build and send cmd to device
 * Function List :
 * History       :
 * 1.Date        : 2012/9/21
 *   Author      : Hash
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
#include <linux/skbuff.h>
#include <net/cfg80211.h>
#include <linux/sched.h>
#include <net/netlink.h>


#include "common.h"
#include "hwifi_event.h"
#include "cfg80211_stru.h"
#include "hwifi_utils.h"
#include "hwifi_msg.h"
#include "cfg_event_rx.h"
#include "hi110x.h"
#include "hwifi_cfg80211.h"
#include "hwifi_hcc.h"
#include "hwifi_cfgapi.h"
#include "hwifi_regdomain.h"
#include "hwifi_regdb.h"
#ifdef __ROAM__
#include "hwifi_roam_main.h"
#include "hwifi_roam_alg.h"
#endif
#include "hwifi_wpa_ioctl.h" /* for wl_pwrm_set */
#include "hwifi_wl_config_ioctl.h"
#include "hi110x_pm.h"

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
#include "hwifi_aggre.h"
#endif


#include "hwifi_config.h"
#include "hwifi_dev_err.h"


/*
 * 2 Global Variable Definition
 */
uint8  g_ba_max_tx    = 0x2;
uint8  g_ba_max_rx    = 0x2;
uint8  g_ba_max_retry = 0x10;
uint8  g_ec212_count  = 0;
extern int32 g_dev_err_subcnt;
/* wid value */
#define FILTER_VALUE         0x10

#define WIDHDR_LEN           3
#define SHUTDOWN_RF_VALUE    0


/* set device 2.4g or 5g params  */
#define DEVICE_2G_BAND 0x0000
#define DEVICE_5G_BAND 0x8000

#define ASSOC_REQ_IE_OFFSET        28
#define ASSOC_RESP_IE_OFFSET       30

/* cipher suite selectors */
#define HWIFI_CIPHER_SUITE_WEP40   0x000FAC01
#define HWIFI_CIPHER_SUITE_TKIP    0x000FAC02
#define HWIFI_CIPHER_SUITE_WEP104  0x000FAC05

#define HWIFI_PAIRWISE   1
#define HWIFI_GROUP      0
#define HWIFI_WAPI_KEY   32

#define MAX_DEVICE_NUM   8

#define MISS_ID         -1

#define AUTH_TYPE_ERROR  13

#define LTE_COEX_SET_DISABLE            0
#define LTE_COEX_SET_ENABLE             1
#define LTE_COEX_WLAN_PRIORITY_LOW      2
#define LTE_COEX_WLAN_PRIORITY_HIGH     3

TX_RATE_STRU stTxRate[] =
{
    { 0x80, 6},
    { 0x81, 13},
    { 0x82, 19},
    { 0x83, 26},
    { 0x84, 39},
    { 0x85, 52},
    { 0x86, 58},
    { 0x87, 65},
};
/*
 * 3 Function Definition
 */

int g_device_tx_status = 0;
uint8 g_ec212_unicast = 0;


int g_rssi_compensate = 0;
module_param(g_rssi_compensate, int, S_IRUGO|S_IWUSR);

int8 g_lte_wifi_coex = LTE_COEX_SET_DISABLE;

#ifdef __ROAM__
int32 hwifi_recv_roam_report(struct cfg_struct *cfg, uint8 *buf);
int32 hwifi_recv_roam_scan_report(struct cfg_struct *cfg, uint8 *buf);
int32 hwifi_recv_roam_report_trigger(struct cfg_struct *cfg, uint8 *buf);
int32 hwifi_recv_roam_new_ap_assoc_cmpl(struct cfg_struct *cfg, uint8 *buf);
int32 hwifi_recv_roam_return_to_old(struct cfg_struct *cfg, uint8 *buf);
void hwifi_roam_calc_tx_failed_scale(struct cfg_struct *cfg, uint32 tx_success_pkt, uint32 tx_failed_pkt, int32 curr_rate);
#endif
void hwifi_get_rate_info(DEVICE_STATS_STRU stStatInfo, struct rate_info *rate_info);
static int32 smooth_signal_rssi(struct cfg_struct *cfg, int32 rssi);
extern uint32 get_wifi_rssi(void);

int32  hwifi_sta_2040_enable_ctrl_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff                      *skb;
    struct set_sta_2040_enable_msg      *wid_2040_enabled_msg;
    uint16                               msg_size;
    int32                                ret;


    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support WID_2040_ENABLE set.");
        return -EFAIL;
    }

    msg_size = sizeof(struct set_sta_2040_enable_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
       HWIFI_WARNING("Failed to alloc skb for 2040 enable ctrl message!");
       return -EFAIL;
    }

    wid_2040_enabled_msg = (struct set_sta_2040_enable_msg *)skb_put(skb, sizeof(struct set_sta_2040_enable_msg));

    /* set the msg header */
    hwifi_fill_msg_hdr(&wid_2040_enabled_msg->msg_hdr, HOST_CMD_CONFIG, msg_size);
    /*control 40M mode ; enabled:1-->open 40M mode ; enabled:0-->close 40M mode*/
    hwifi_fill_char_wid(&wid_2040_enabled_msg->wid_2040_enable, WID_2040_ENABLE, enabled);
    /*control 40M short gi; enabled:1-->open 40M short gi; enabled:0-->close 40M short gi*/
    hwifi_fill_char_wid(&wid_2040_enabled_msg->wid_char_reserve, WID_CHAR_RESERVE, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send WID_2040_ENABLE param set msg!");
       /*dev_kfree_skb_any(skb);*/
       return -EFAIL;
    }

    if (IS_AP(cfg))
    {
        cfg->ap_info.fc_enabled  =  enabled;
    }
    else
    {
        cfg->sta_info.fc_enabled  =  enabled;
    }

    HWIFI_INFO("Succeed to set WID_2040_ENABLE:%d", enabled);

    return SUCC;
}






int32 connect_init_11b_params(struct cfg_struct *cfg)
{
    struct sk_buff *skb;
    struct hwifi_11b_connect_msg *msg;
    int32                         ret;
    uint16                        msg_size;

    HWIFI_ASSERT((NULL != cfg));

    msg_size = sizeof(struct hwifi_11b_connect_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct hwifi_11b_connect_msg *)skb_put(skb, msg_size);

    /* set the msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    hwifi_fill_char_wid(&msg->phy_active_reg_1, WID_PHY_ACTIVE_REG, WID_11b_connect_PHY_ACTIVE_REG_1);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_1, WID_11N_PHY_ACTIVE_REG_VAL, WID_11b_connect_ACTIVE_REG_VAL_1);
    hwifi_fill_char_wid(&msg->phy_active_reg_2, WID_PHY_ACTIVE_REG, WID_11b_connect_PHY_ACTIVE_REG_2);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_2,WID_11N_PHY_ACTIVE_REG_VAL,WID_11b_connect_ACTIVE_REG_VAL_2);
    hwifi_fill_char_wid(&msg->phy_active_reg_3, WID_PHY_ACTIVE_REG, WID_11b_connect_PHY_ACTIVE_REG_3);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_3,WID_11N_PHY_ACTIVE_REG_VAL,WID_11b_connect_ACTIVE_REG_VAL_3);

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("connect status, 11b param init success", ret);

    return ret;


}
int32 disconnect_init_11b_params(struct cfg_struct *cfg)
{
    struct sk_buff *skb;
    struct hwifi_11b_disconnect_msg  *msg;
    int32                             ret;
    uint16                            msg_size;

    HWIFI_ASSERT((NULL != cfg));

    msg_size = sizeof(struct hwifi_11b_disconnect_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct hwifi_11b_disconnect_msg *)skb_put(skb, msg_size);

    /* set the msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    hwifi_fill_char_wid(&msg->phy_active_reg_1, WID_PHY_ACTIVE_REG, WID_11b_disconnect_PHY_ACTIVE_REG_1);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_1, WID_11N_PHY_ACTIVE_REG_VAL, WID_11b_disconnect_ACTIVE_REG_VAL_1);
    hwifi_fill_char_wid(&msg->phy_active_reg_2, WID_PHY_ACTIVE_REG, WID_11b_disconnect_PHY_ACTIVE_REG_2);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_2,WID_11N_PHY_ACTIVE_REG_VAL,WID_11b_disconnect_ACTIVE_REG_VAL_2);
    hwifi_fill_char_wid(&msg->phy_active_reg_3, WID_PHY_ACTIVE_REG, WID_11b_disconnect_PHY_ACTIVE_REG_3);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_3,WID_11N_PHY_ACTIVE_REG_VAL,WID_11b_disconnect_ACTIVE_REG_VAL_3);

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("disconnect status, 11b param init success", ret);

    return ret;
}
uint16 hwifi_channel_index_to_freq(uint8 index)
{
    HWIFI_DEBUG("channel=%d",index);
    if (index >= HWIFI_MIN_CHANNEL_2G && index < HWIFI_MAX_CHANNEL_2G)
    {
        return (uint16)(HWIFI_BASE_FREQ_2G + 5 *(index - 1));
    }
    else if (HWIFI_MAX_CHANNEL_2G == index)
    {
        return (uint16)HWIFI_MAX_FREQ_2G;
    }
    else if (index >= HWIFI_MIN_CHANNEL_5G && index <= HWIFI_MAX_CHANNEL_5G)
    {
        return (uint16)(HWIFI_BASE_FREQ_5G + (5 * index));
    }
    /* handle 4.9G */
    else if (index >= HWIFI_MIN_CHANNEL_4_9G && index <= HWIFI_MAX_CHANNEL_4_9G)
    {
        return (uint16)(HWIFI_BASE_FREQ_4_9G + (5 * index));
    }
    else
    {
        return 0;
    }
}
uint8 bytes_check_sum(uint8* buf, uint32 len)
{
    uint8 checksum = 0;

    while(len--)
    {
        checksum += *buf++;
    }

    return checksum;
}

/*
 * Prototype    : hwifi_alloc_skb_for_cmd
 * Description  :
 * Input        :
 * Output       : None
 * Return Value : struct sk_buff *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/14
 *     Author       : kf74588
 *     Modification : Created function
 *
 */

struct sk_buff * hwifi_alloc_skb_for_cmd(uint32 len)
{
    struct sk_buff  *skb;

    skb = dev_alloc_skb(HCC_MAX_HEAD_SIZE + len + HCC_SKB_TAIL_ALGIN_LEN);
    if (NULL == skb)
    {
        HWIFI_WARNING("Failed to alloc skb!");
        return NULL;
    }

    skb_reserve(skb, HCC_MAX_HEAD_SIZE);

    return skb;
}

void hwifi_fill_msg_hdr(struct hwifi_msg_header *header, uint8 cmd_type, uint16 msg_len)
{
    header->msg_type = MSG_TYPE_WRITE;
    header->msg_len  = OS_CPU_TO_LE16(msg_len);

    header->host_cmd.wid = OS_CPU_TO_LE16(WID_HOST_CMD);
    header->host_cmd.len = CHAR_WID_LEN;
    header->host_cmd.val = cmd_type;
}

STATIC inline int32 _hwifi_send_cmd_(struct cfg_struct *cfg,
                                                                                            struct hwifi_msg_header *header,
                                                                                            struct sk_buff  *skb)
{
    int ret = SUCC;
    static atomic_t seq_num;
    HWIFI_BUG_ON(!cfg);
    HWIFI_BUG_ON(!header);
    HWIFI_BUG_ON(!skb);
    /* fill msg seq */
    header->msg_seq = 0xFF & atomic_inc_return(&seq_num);

    ret = hcc_tx(cfg->hi110x_dev->hcc, HCC_TYPE_WIFI_CTRL, HCC_SUBTYPE_ANY, skb);
    if (unlikely(ret != SUCC))
    {
        HWIFI_WARNING("Failed to send hcc data!");
        dev_kfree_skb_any(skb);

        return ret;
    }
    return ret;
}

/*
 * Prototype    : hwifi_send_cmd
 * Description  : send wid format cmd to device
 * Input        :
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/20
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
int32 hwifi_send_cmd(struct cfg_struct *cfg, struct sk_buff  *skb)
{
    struct hwifi_msg_header *header;
    uint8 cmd_val;
    int32 ret = SUCC;

    header = (struct hwifi_msg_header *)(skb->data);

    if (header->msg_len != skb->len)
    {
        HWIFI_WARNING("Invalid header [msg_len:%d] not equal [skb->len:%d]",header->msg_len,skb->len);
        dev_kfree_skb_any(skb);
        return -EFAIL;
    }

    /* drop other cmd until init complete */
    cmd_val = header->host_cmd.val;
    if (TRUE != cfg->init_flag && HOST_CMD_INIT != cmd_val)
    {
        HWIFI_INFO(" WID command[%d] is dropped",cmd_val);
        dev_kfree_skb_any(skb);

        return -EFAIL;
    }

    ret = _hwifi_send_cmd_(cfg, header, skb);

    return ret;
}
int hwifi_send_dfx_wid_char(struct cfg_struct *cfg, uint16 wid, uint8 val)
{
    int ret = SUCC;
    uint16                            msg_size;
    struct sk_buff                   *skb;
    struct hwifi_msg_header  *msg;
    if(HWIFI_WARN_ON((wid & 0xF000) >> 12 != WID_CHAR))
    {
        return -EFAIL;
    }

    msg_size = sizeof(struct hwifi_msg_header);
    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    msg = (struct hwifi_msg_header *)skb_put(skb, msg_size);
    msg->msg_type = MSG_TYPE_WRITE;
    msg->msg_len  = OS_CPU_TO_LE16(msg_size);

    msg->host_cmd.wid = OS_CPU_TO_LE16(wid);
    msg->host_cmd.len = CHAR_WID_LEN;
    msg->host_cmd.val = val;

    ret = _hwifi_send_cmd_(cfg, msg, skb);

    return ret;
}

/*
 * Prototype    : wl_set_wmm_ap_uapsd
 * Description  : set ap to support wmm
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/10
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 wl_set_wmm_ap_uapsd(struct cfg_struct *cfg, uint8 enabled)
{
    struct set_ap_uapsd_msg *msg;
    struct sk_buff          *skb;
    uint16                   msg_size;
    int32                    ret;

    HWIFI_ASSERT(NULL != cfg);

    msg_size = sizeof(struct set_ap_uapsd_msg);
    if(msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    msg = (struct set_ap_uapsd_msg *)skb_put(skb, msg_size);

    /* add msg header */
    hwifi_fill_msg_hdr(&msg->msg_header, HOST_CMD_CONFIG, msg_size);
    /* add ap uapsd wid */
    hwifi_fill_char_wid(&msg->ap_uapsd, WID_UAPSD_SUPPORT_AP, enabled);

    ret = hwifi_send_cmd(cfg, skb);

    return ret;
}



int32  hwifi_set_wmm_uapsd(struct cfg_struct *cfg, struct wmm_uapsd_param *param)
{
    struct set_uapsd_msg  *msg;
    struct sk_buff        *skb;
    uint8                 *checksum;
    uint16                 msg_size;
    int32                  ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != param));

    msg_size = sizeof(struct set_uapsd_msg) + WID_UAPSD_CONFIG_BUF_LEN + 1;/* 1 for checksum*/
    if(msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    msg = (struct set_uapsd_msg *)skb_put(skb, msg_size);

    /* add msg header */
    hwifi_fill_msg_hdr(&msg->msg_header, HOST_CMD_CONFIG, msg_size);
    /*add uapsd wid */
    hwifi_fill_binary_wid(&msg->wmm_uapsd_buf, WID_UAPSD_CONFIG, WID_UAPSD_CONFIG_BUF_LEN, (uint8 *)param);

    /* checksum */
    checksum = (uint8 *)(msg + 1) + WID_UAPSD_CONFIG_BUF_LEN;
   *checksum = bytes_check_sum(msg->wmm_uapsd_buf.val, WID_UAPSD_CONFIG_BUF_LEN);

    ret = hwifi_send_cmd(cfg, skb);

    return ret;
}



inline void hwifi_fill_query_msg_hdr(struct hwifi_query_msg_header *header, uint16 msg_len)
{
    header->msg_type = MSG_TYPE_QUERY;
    header->msg_len  = OS_CPU_TO_LE16(msg_len);
}



/*
 * Prototype    : hwifi_fill_scan_chan
 * Description  : insert scan channel to scan msg
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/22
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
uint8 *hwifi_fill_scan_chan(uint8 *buf, struct hwifi_scan_param_stru *param)
{
    struct channel_list_wid *channel_list;
    struct char_wid         *filter;
    uint8   *checksum;                  /* pointer to channel list checksum */

    HWIFI_DEBUG("Add channel list in SCAN msg");
    channel_list             = (struct channel_list_wid*)buf;
    channel_list->wid        = OS_CPU_TO_LE16(WID_BIN_CHANNEL_LIST);
    channel_list->n_channels = param->n_channels;
    channel_list->len        = channel_list->n_channels + 1;
    OS_MEM_CPY(channel_list->channels, param->channels, channel_list->n_channels);

    checksum    = (uint8*)channel_list->channels + channel_list->n_channels;
    (*checksum) = bytes_check_sum(&channel_list->n_channels, channel_list->n_channels + 1);

    buf = checksum + 1;

    /* insert filter ssid */
    filter      = (struct char_wid *)buf;
    hwifi_fill_char_wid(filter, WID_SCAN_FILTER, FILTER_VALUE);

    buf = (uint8 *)(filter + 1);

    return buf;
}


static int32 hwifi_del_p2p_ie(uint8 *ie, int32 ie_len, uint8 *ie_out, int32 *ie_out_len)
{
    uint8   *pos;
    uint8   *pos_out;
    uint32   oui;
    uint32   left = ie_len;
    uint8    id;
    uint8    elen;
    uint8    p2p_ie_found_flag;

    if ((NULL == ie) || (0 == ie_len))
    {
        return SUCC;
    }
    else if(NULL == ie_out)
    {
        HWIFI_WARNING("Invalid NULL param ie_out!");
        return -EFAIL;
    }

    p2p_ie_found_flag = FALSE;
    pos = ie;
    pos_out = ie_out;
    *ie_out_len = 0;


    HWIFI_DEBUG("ie_len is %d.", ie_len);

    while (left >= 2)
    {
        id = *pos++;
        elen = *pos++;
        left -= 2;

        HWIFI_DEBUG("Element id is 0x%x, elen is %d, left is %d", id, elen, left);

        if (elen > left)
        {
            HWIFI_WARNING("Element id is 0x%x, element len(%d) big than left len(%d),parse failed.", id, elen, left);
            return -EFAIL;
        }

        if (WLAN_EID_VENDOR_SPECIFIC == id)
        {
            oui = WPA_GET_BE32(pos);
            if (P2P_IE_VENDOR_TYPE == oui)
            {
                HWIFI_DEBUG("Find p2p ie,ie len is %d", elen + 2);
                p2p_ie_found_flag = TRUE;
            }
        }

        if (TRUE == p2p_ie_found_flag)
        {
            /* 找到P2P IE，跳过此IE ,并将后面剩下的IE拷贝到输出参数ie_out中*/
            left -= elen;
            pos += elen;

            memcpy(pos_out, pos, left);
            pos_out += left;
            *ie_out_len += left;

            break;
        }
        else
        {
            /* 不是P2P IE,则拷贝到输出参数中 */
            memcpy(pos_out, pos - 2, elen + 2);
            pos_out += (elen + 2);
            *ie_out_len += (elen + 2);

            left -= elen;
            pos += elen;
        }
    }

    return SUCC;
}




int32 hwifi_start_scan(struct cfg_struct *cfg, struct hwifi_scan_param_stru *param)
{
    struct sk_buff  *skb;
    struct scan_msg *msg;                       /* pointer to scan cmd message struct */
    uint16           msg_size;                  /* scan cmd message size */
    uint16       band;          /* scan band */
    int32            ie_len;
    uint8           *ie;
    int32            ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != param));
    ie_len = 0;
    ie     = NULL;

    msg_size = sizeof(struct scan_msg);

    if (param->n_channels > 0)
    {
        msg_size += sizeof(struct channel_list_wid);
        msg_size += param->n_channels + 1;
        msg_size += sizeof(struct char_wid);
    }

    if (param->ssid_num > 0)
    {
        /* GO模式判断，防止GO模式扫描时覆盖之前设置的SSID */
        if ((!IS_P2P_ON(cfg)) || (!IS_AP(cfg)))
        {
            msg_size += sizeof(struct string_wid) + param->ssids[0].ssid_len;
        }
    }

    msg_size += sizeof(struct char_wid);

    if (param->ie != NULL)
    {
        /* 1:usered to set checksum */
        if (!IS_P2P_ON(cfg))
        {
            /* P2P 关闭的情况下扫描报文不带P2P IE，需要从IE信息中删除 */
            ie = kmalloc(param->ie_len, GFP_KERNEL);
            if (NULL == ie)
            {
                HWIFI_ERROR("Failed to alloc for scan param IE!");
                return -EFAIL;
            }

            memset(ie, 0, param->ie_len);
            ret = hwifi_del_p2p_ie((uint8 *)param->ie, param->ie_len, ie, &ie_len);
            if(SUCC != ret)
            {
                kfree(ie);
                ie = NULL;

                HWIFI_ERROR("Failed to del p2p ie!");
                return -EFAIL;
            }

            msg_size += sizeof(struct binary_wid) + ie_len + 1;
        }
        else
        {
            msg_size += sizeof(struct binary_wid) + param->ie_len + 1;
        }

    }
    else
    {
        msg_size += sizeof(struct binary_wid) + 1;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        if (!IS_P2P_ON(cfg))
        {
            kfree(ie);
            ie = NULL;
        }
        return -EFAIL;
    }

    msg = (struct scan_msg *)skb_put(skb, sizeof(struct scan_msg));

    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_SCAN, msg_size);

    /* fill scan type */
    hwifi_fill_char_wid(&msg->scan_type_wid, WID_SCAN_TYPE, (HWIFI_ACTIVE_SCAN == param->type));

    /* fill scan band */
    band = (param->band & BAND_2G) ? DEVICE_2G_BAND : DEVICE_5G_BAND;
    if(IS_AP(cfg) && IS_P2P_ON(cfg))
    {
        /* GO mode scan,add channel info */
        band |= (cfg->ap_info.channel_info & (~DEVICE_5G_BAND));
    }
    hwifi_fill_short_wid(&msg->scan_band_wid, WID_USER_PREF_CHANNEL, band);

    /* fill scan channel list wid */
    if (param->n_channels > 0)
    {
        uint8 * old;
        uint8 * new;

        old = skb_tail_pointer(skb);
        new = hwifi_fill_scan_chan(old, param);

        skb_put(skb, new - old);
    }

    /* fill scan ssid wid */
    if (param->ssid_num > 0)
    {
        /* GO模式判断，防止GO模式扫描时覆盖之前设置的SSID */
        if (!IS_P2P_ON(cfg) || !IS_AP(cfg))
        {
            struct string_wid       *ssid;       /* pointer to wid of ssid */

            HWIFI_DEBUG("Add ssid in SCAN msg");
            ssid      = (struct string_wid *)skb_tail_pointer(skb);
            hwifi_fill_string_wid(ssid, WID_SSID, param->ssids[0].ssid_len, param->ssids[0].ssid);

            skb_put(skb, sizeof(struct string_wid) + ssid->len);
        }
    }

    {
        struct char_wid         *bcast_ssid; /* pointer to wid of bcast_ssid */
        int bcast;

        HWIFI_DEBUG("Add bcast_ssid in SCAN msg");
        bcast_ssid = (struct char_wid *)skb_tail_pointer(skb);

        bcast = (param->ssid_num > 0)? FALSE:TRUE;
        hwifi_fill_char_wid(bcast_ssid, WID_BCAST_SSID, bcast);

        skb_put(skb, sizeof(struct char_wid));
    }

    /* fill scan ie */
    if (param->ie != NULL)
    {
        struct binary_wid  *scan_ie;
        uint8           *checksum;

        HWIFI_DEBUG("Add ie [len=%d] in SCAN msg", param->ie_len);
        scan_ie = (struct binary_wid *)skb_tail_pointer(skb);


        if (!IS_P2P_ON(cfg))
        {
            /* p2p 未开启，则删除P2P IE */
            hwifi_fill_binary_wid(scan_ie, WID_IE, ie_len, (uint8 *)ie);
        }
        else
        {
            hwifi_fill_binary_wid(scan_ie, WID_IE, param->ie_len, (uint8 *)param->ie);
        }


        skb_put(skb, sizeof(struct binary_wid) + scan_ie->len);

        checksum = (uint8 *)skb_tail_pointer(skb);
        *checksum = bytes_check_sum(scan_ie->val, scan_ie->len);

        skb_put(skb, sizeof(*checksum));
    }
    else
    {
        struct binary_wid  *scan_ie;
        uint8           *checksum;

        HWIFI_DEBUG("Add ie param->ie is NULL -> WID_IE NULL in SCAN msg");
        scan_ie = (struct binary_wid *)skb_tail_pointer(skb);


        hwifi_fill_binary_wid(scan_ie, WID_IE, 0, NULL);

        skb_put(skb, sizeof(struct binary_wid) + scan_ie->len);

        checksum = (uint8 *)skb_tail_pointer(skb);
        *checksum = bytes_check_sum(scan_ie->val, scan_ie->len);

        skb_put(skb, sizeof(*checksum));

    }

    /* send scan cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("SCAN", ret);
    if (!IS_P2P_ON(cfg))
    {
        /* return之前 需要是释放ie内存 */
        kfree(ie);
        ie = NULL;
    }

    return ret;
}

/*
 * Prototype    : hwifi_fill_wepkey
 * Description  : fill wep key info to connect msg
 * Input        : None
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/22
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void hwifi_fill_wepkey(struct wep_key *wep_key, uint8 key_len, uint8 key_idx, uint8 *key, uint8 set_flag)
{
    wep_key->wepkey_wid       = OS_CPU_TO_LE16(WID_ADD_WEP_KEY);
    wep_key->wepkey_wid_len   = sizeof(struct wep_key) + key_len - WIDHDR_LEN;
    wep_key->set_flag         = set_flag;
    wep_key->key_index        = key_idx;
    wep_key->key_len          = key_len;
    OS_MEM_CPY(wep_key->key_val, key, key_len);
}


STATIC void hwifi_fill_listeninterval(struct char_wid *listen_interval, int32 value)
{
    HWIFI_ASSERT(listen_interval != NULL);

    listen_interval->wid        = WID_LISTEN_INTERVAL;
    listen_interval->len        = sizeof(struct char_wid);
    listen_interval->val        = value;
}


int32 hwifi_set_listeninterval(struct cfg_struct *cfg, int32 value)
{
    struct set_listeninterval_msg    *msg;
    struct sk_buff                   *skb;
    uint16                            msg_size;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    msg_size = sizeof(struct set_listeninterval_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    msg = (struct set_listeninterval_msg *)skb_put(skb, sizeof(struct set_listeninterval_msg));

    /* fill listeninterval wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_header, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_listeninterval(&msg->listeninterval, value);

    /* send listeninterval wid to Device */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set listeninterval wid", ret);

    return SUCC;
}


int32 hwifi_set_pktfilter(struct cfg_struct *cfg, int32 value)
{
    struct set_pktfilter_msg         *msg;
    struct sk_buff                   *skb;
    uint16                            msg_size;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    msg_size = sizeof(struct set_pktfilter_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long pktfilter msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    msg = (struct set_pktfilter_msg *)skb_put(skb, sizeof(struct set_pktfilter_msg));

    /* fill pktfilter wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_char_wid(&msg->pktfilter, WID_FILTER_GROUP_DATA, value);

    /* send pktfilter wid to Device */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set pktfilter wid", ret);

    return SUCC;
}


int32 hwifi_keepalive_enable(struct cfg_struct *cfg, int32 value)
{
    struct set_keepalive_msg         *msg;
    struct sk_buff                   *skb;
    uint16                            msg_size;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    msg_size = sizeof(struct set_keepalive_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long exterTSF msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    msg = (struct set_keepalive_msg *)skb_put(skb, sizeof(struct set_keepalive_msg));

    /* fill keepalive wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_char_wid(&msg->keepalive, WID_KEEP_ALIVE_ENABLE, value);

    /* send keepalive wid to Device */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set keepalive wid", ret);

    return SUCC;
}


int32 hwifi_set_powermode(struct cfg_struct *cfg, int32 value)
{
    struct set_powermode_msg         *msg;
    struct sk_buff                   *skb;
    uint16                            msg_size;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    HWIFI_INFO("set powermode = %d", value);
    msg_size = sizeof(struct set_powermode_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long powermode msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    msg = (struct set_powermode_msg *)skb_put(skb, sizeof(struct set_powermode_msg));

    /* fill set_powermode wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_char_wid(&msg->power_mode, WID_POWER_MANAGEMENT, value);

    /* set_powermode wid to Device */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set_powermode wid", ret);

    return SUCC;
}


int32 hwifi_set_hips_switch(struct cfg_struct *cfg, int32 value)
{
    struct set_hips_switch_msg       *msg;
    struct sk_buff                   *skb;
    uint16                            msg_size;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    HWIFI_INFO("set hips switch = %d", value);
    msg_size = sizeof(struct set_hips_switch_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("set hips switch : the leng of the set hips msg is too long");
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        HWIFI_WARNING("can not malloc for set hips msg");
        return -ENOMEM;
    }

    msg = (struct set_hips_switch_msg *)skb_put(skb, sizeof(struct set_hips_switch_msg));

    /* fill set hips switch wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_char_wid(&msg->hips_switch, WID_HI1101_PS_SWITCH, value);

    /* set hips switch wid to Device */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set hips switch wid", ret);

    return SUCC;
}



#define WID_FIT_CPUFREQ_LEN 1
int32 hwifi_fit_cpufreq(struct cfg_struct *cfg, uint8 value)
{
    struct fit_freq_msg              *msg;
    struct sk_buff                   *skb;
    uint8                            *checksum;
    uint16                            msg_size;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    HWIFI_DEBUG("fit frequecy = %d....", value);
    msg_size = sizeof(struct fit_freq_msg) + sizeof(*checksum) + WID_FIT_CPUFREQ_LEN;
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("fit freq : the leng of the fit_freq_msg is too long");
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        HWIFI_WARNING("can not malloc for fit_freq_msg");
        return -ENOMEM;
    }

    msg = (struct fit_freq_msg *)skb_put(skb, msg_size);

    /* fill fit_freq wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    /**
     *设置fit_freq bin WID,wid长度为1时，为降频wid
     */
    hwifi_fill_binary_wid(&msg->fit_freq, WID_FREQ_PLAN, WID_FIT_CPUFREQ_LEN, &value);

    /* checksum */
     checksum = (uint8 *)(msg + 1) + WID_FIT_CPUFREQ_LEN;
    *checksum = bytes_check_sum(msg->fit_freq.val, WID_FIT_CPUFREQ_LEN);

    /* send fit freq wid to Device */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("fit freq wid", ret);

    if (SUCC == ret)
    {
        /* 保存cpu频率 */
        g_pm_info->last_cpufreq = value;
    }

    return SUCC;
}
 #define WID_SHUTDOWN_RF_LEN 1
int32 hwifi_shutdown_RF(struct cfg_struct *cfg)
{
    struct shutdown_RF_msg           *msg;
    struct sk_buff                   *skb;
    uint8                            *checksum;
    uint16                            msg_size;
    uint8                             val;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    val = SHUTDOWN_RF_VALUE;

    HWIFI_DEBUG("shut down RF");
    msg_size = sizeof(struct shutdown_RF_msg) + sizeof(*checksum) + WID_SHUTDOWN_RF_LEN;
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("shutdown_RF : the leng of the shutdown_RF_msg is too long");
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        HWIFI_WARNING("Failed to malloc for shutdown_RF_msg");
        return -ENOMEM;
    }

    msg = (struct shutdown_RF_msg *)skb_put(skb, msg_size);

    /* fill shutdown wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    /**
     *设置WID_FREQ_PLAN bin WID,wid长度为1并且值为0时，为shutdown RF wid
     */
    hwifi_fill_binary_wid(&msg->RF_shutdown, WID_FREQ_PLAN, WID_SHUTDOWN_RF_LEN, &val);

    /* checksum */
     checksum = (uint8 *)(msg + 1) + WID_SHUTDOWN_RF_LEN;
    *checksum = bytes_check_sum(msg->RF_shutdown.val, WID_SHUTDOWN_RF_LEN);

    /* send shutdown RF wid to Device */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("shutdown RF wid", ret);

    return SUCC;
}


 #define WID_SHUTDOWN_POWERSAVE_LEN 1
 #define SHUTDOWN_POWERSAVE_VALUE   0xFE
int32 hwifi_shutdown_powersave(struct cfg_struct *cfg)
{
    struct shutdown_powersave_msg    *msg;
    struct sk_buff                   *skb;
    uint8                            *checksum;
    uint16                            msg_size;
    uint8                             val;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    val = SHUTDOWN_POWERSAVE_VALUE;

    HWIFI_INFO("shut down Powersave...");
    msg_size = sizeof(struct shutdown_powersave_msg) + sizeof(*checksum) + WID_SHUTDOWN_POWERSAVE_LEN;
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("shutdown powersave : the leng of the shutdown powersave msg is too long");
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        HWIFI_WARNING("can not malloc for shutdown powersave msg");
        return -ENOMEM;
    }

    msg = (struct shutdown_powersave_msg *)skb_put(skb, msg_size);

    /* fill shutdown wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    /**
     *设置WID_FREQ_PLAN bin WID,wid长度为1并且值为0xFE时，为shutdown powersave wid
     */
    hwifi_fill_binary_wid(&msg->shutdown_powersave, WID_FREQ_PLAN, WID_SHUTDOWN_POWERSAVE_LEN, &val);

    /* checksum */
     checksum = (uint8 *)(msg + 1) + WID_SHUTDOWN_POWERSAVE_LEN;
    *checksum = bytes_check_sum(msg->shutdown_powersave.val, WID_SHUTDOWN_POWERSAVE_LEN);

    /* send shutdown powersave wid to Device */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("shutdown powersave wid", ret);

    return SUCC;
}




int32 hwifi_set_bt_opt(struct cfg_struct *cfg, int value)
{
    struct bt_opt_msg    *msg;
    struct sk_buff                   *skb;
    uint16                            msg_size;
    uint8                             val;
    int32                             ret;

    HWIFI_ASSERT(NULL != cfg);

    val = value;

    msg_size = sizeof(struct bt_opt_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("shutdown powersave : the leng of the shutdown powersave msg is too long");
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        HWIFI_WARNING("can not malloc for shutdown powersave msg");
        return -ENOMEM;
    }

    msg = (struct bt_opt_msg *)skb_put(skb, msg_size);

    /* fill shutdown wid ,msg header and wid information */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    /**
     *设置WID_FREQ_PLAN bin WID,wid长度为1
     */
    hwifi_fill_char_wid(&msg->bt_opt, WID_CHAR_RESERVE, val);

    /* send bt opt wid to device */
    ret = hwifi_send_cmd(cfg, skb);
	if (SUCC != ret)
	{
		HWIFI_WARNING("Failed to send BT OPT MSG!");
		return -EFAIL;
	}
    PRINT_SEND_CMD_RET("BT OPT WID", ret);

    return SUCC;
}

#ifdef __ROAM__
/*
 * Prototype    : save_conn_ie
 * Description  : save connect ie,include wpa/rsn ie.
 * Input        : struct cfg_struct *cfg
                  int32 ie_len
 * Output       : None
 * Return Value : SUCC or -EFAIL
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/6/17
 *     Author       : dengwenhua
 *     Modification : Created function
 *
 */
STATIC void save_conn_ie(struct cfg_struct *cfg, uint8 *ie, int32 ie_len)
{
    HWIFI_ASSERT(NULL != cfg);

    if(cfg->conn.bss.assoc_ie_len && (NULL != cfg->conn.bss.assoc_ie))
    {
        /* 删除旧的IE */
        kfree(cfg->conn.bss.assoc_ie);
        cfg->conn.bss.assoc_ie = NULL;
        cfg->conn.bss.assoc_ie_len = 0;
    }

    if(0 < ie_len)
    {
        HWIFI_ASSERT(NULL != ie);

        /* 保存关联AP的加密IE信息 */
        cfg->conn.bss.assoc_ie = (int8 *)kzalloc(ie_len, GFP_KERNEL);
        if (NULL == cfg->conn.bss.assoc_ie)
        {
            HWIFI_WARNING("Failed to alloc memory for conn.bss.assoc_ie,but continue to connect.");
        }
        else
        {
            memcpy(cfg->conn.bss.assoc_ie, ie, ie_len);
            cfg->conn.bss.assoc_ie_len = ie_len;
        }
    }
}
#endif

/*
 * Prototype    : hwifi_start_conn
 * Description  : build connect msg with wid format and send msg
 * Input        : struct cfg_struct *cfg
                  hwifi_conn_param_stru *connect_param
 * Output       : None
 * Return Value : SUCC or -EFAIL
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/26
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_start_conn(struct cfg_struct *cfg, struct hwifi_conn_param_stru *connect_param)
{
    struct sk_buff        *skb,*skb_reconn = NULL;
    struct connect_msg    *msg;            /* msg body */
    uint16                 msg_size;
    int32                  no_p2p_ie_len;
    uint8                 *no_p2p_ie;
    int32                  ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != connect_param) && (0 != connect_param->ssid_len));


    /* get msg size */
    msg_size = sizeof(struct connect_msg) + connect_param->ssid_len;

    /* size for bssid if exist */
    if (TRUE == connect_param->bssid_flag)
    {
        msg_size += sizeof(struct string_wid) + MAC_LEN;
    }

    /* size for wep key if exist */
    if (0 != connect_param->wep_key_len)
    {
        msg_size += sizeof(struct wep_key) + connect_param->wep_key_len;
    }

    /* size for crypto info if exist */
    if (HWIFI_AUTHTYPE_SHARED_KEY != connect_param->auth_type)
    {
        msg_size += sizeof(struct crypto_info);
    }


    no_p2p_ie_len = 0;
    no_p2p_ie     = NULL;

    if (!IS_P2P_ON(cfg))
    {
        /* P2P 关闭的情况下关联请求报文不带P2P IE，需要从IE信息中删除 */
        if ((NULL != connect_param->ie) && (0 != connect_param->ie_len))
        {
            no_p2p_ie = kmalloc(connect_param->ie_len, GFP_KERNEL);
            if (NULL == no_p2p_ie)
            {
                HWIFI_ERROR("Failed to alloc no_p2p_ie!");
                return -EFAIL;
            }

            memset(no_p2p_ie, 0, connect_param->ie_len);

            ret = hwifi_del_p2p_ie((uint8 *)connect_param->ie, connect_param->ie_len, no_p2p_ie, &no_p2p_ie_len);
            if(SUCC != ret)
            {
                kfree(no_p2p_ie);
                no_p2p_ie = NULL;

                HWIFI_ERROR("Failed to del p2p ie");
                return -EFAIL;
            }
        }

        msg_size += sizeof(struct binary_wid) + no_p2p_ie_len + 1;
    }
    else
    {
        msg_size += sizeof(struct binary_wid) + connect_param->ie_len + 1;
    }


    /* beacon frame */
    msg_size += sizeof(struct short_wid);
    msg_size += sizeof(struct binary_wid) + connect_param->bss_info->mgmt_len + 2;

    if (msg_size > MAX_MSG_LEN)
    {
        if (!IS_P2P_ON(cfg))
        {
            kfree(no_p2p_ie);
            no_p2p_ie = NULL;
        }
        HWIFI_WARNING("Invalid too long conn msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        if (!IS_P2P_ON(cfg))
        {
            kfree(no_p2p_ie);
            no_p2p_ie = NULL;
        }
        return -EFAIL;
    }

    msg = (struct connect_msg *)skb_put(skb, sizeof(struct connect_msg) + connect_param->ssid_len);

    /* set the msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONNECT, msg_size);

    /* set auth type */
    hwifi_fill_char_wid(&msg->auth_type, WID_AUTH_TYPE, (uint8)(connect_param->auth_type | connect_param->auth_mode));

    /* set crypto type */
    hwifi_fill_char_wid(&msg->_11i_mode, WID_11I_ENCYPT_MODE, connect_param->crypto_type);
    HWIFI_DEBUG("START_CONN : 11i_mode is %u", connect_param->crypto_type);

    /* set ssid */
    hwifi_fill_string_wid(&msg->ssid, WID_SSID, (uint8)connect_param->ssid_len, connect_param->ssid);

    /* set bssid */
    if (TRUE == connect_param->bssid_flag)
    {
        struct string_wid     *bssid;

        HWIFI_DEBUG("START_CONN : add bssid in CONNECT msg");
        bssid = (struct string_wid *)skb_tail_pointer(skb);
        hwifi_fill_string_wid(bssid, WID_BSSID, MAC_LEN, connect_param->bssid);

        skb_put(skb, sizeof(struct string_wid) + MAC_LEN);
    }

    /* set wep key and key index */
    if (0 != connect_param->wep_key_len)
    {
        struct wep_key        *wep_key;

        HWIFI_DEBUG("START_CONN : add wep_key in CONNECT msg");
        wep_key = (struct wep_key *)skb_tail_pointer(skb);
        hwifi_fill_wepkey(wep_key, connect_param->wep_key_len, connect_param->wep_key_index, connect_param->wep_key, TRUE);

        skb_put(skb, sizeof(struct wep_key) + wep_key->key_len);
    }

    /* set crypto info */
    if (HWIFI_AUTHTYPE_SHARED_KEY != connect_param->auth_type)
    {
        struct crypto_info    *crypto_info;    /* crypto info:pairwise key and group key */

        HWIFI_DEBUG("START_CONN : add crypto_info in CONNECT msg");
        crypto_info = (struct crypto_info *)skb_tail_pointer(skb);

        /* set group key */
        hwifi_fill_char_wid(&crypto_info->group_key, WID_11I_GROUP_CIPHER_MODE, connect_param->group_crypto);

        /* set pairwise key */
        hwifi_fill_char_wid(&crypto_info->pairwise, WID_11I_PAIRWISE_CIPHER_MODE, connect_param->pairwise_crypto);

        skb_put(skb, sizeof(struct crypto_info));
    }

     /* p2p added set connect ie */

    {
        struct binary_wid     *ie;
        uint8                 *checksum;

        HWIFI_DEBUG("START_CONN : add connect_ie in CONNECT msg");

        ie  = (struct binary_wid *)skb_tail_pointer(skb);
        if (!IS_P2P_ON(cfg))
        {
            hwifi_fill_binary_wid(ie, WID_IE, no_p2p_ie_len, no_p2p_ie);
        }
        else
        {
            hwifi_fill_binary_wid(ie, WID_IE, connect_param->ie_len, connect_param->ie);
        }
        skb_put(skb, sizeof(struct binary_wid) + ie->len);

#ifdef __ROAM__
        save_conn_ie(cfg, ie->val, ie->len);
#endif

        checksum= (uint8 *)skb_tail_pointer(skb);
        *checksum = bytes_check_sum(ie->val, ie->len);
        skb_put(skb, sizeof(*checksum));
    }
    /* p2p added */
    {
        uint16 freq_band;
        struct binary_wid *beacon;
        struct short_wid  *band;
        uint8             *checksum,*pos;

        /* fill scan band */
        band = (struct short_wid *)skb_put(skb, sizeof(struct short_wid));
        freq_band = (connect_param->bss_info->freq < HWIFI_BASE_FREQ_4_9G) ? DEVICE_2G_BAND : DEVICE_5G_BAND;
        hwifi_fill_short_wid(band, WID_USER_PREF_CHANNEL, freq_band);

        HWIFI_INFO("START_CONN : add beacon frame in CONNECT msg");
        beacon = (struct binary_wid *)skb_tail_pointer(skb);

        /* add channel and beacon frame */
        hwifi_fill_binary_wid(beacon, WID_BIN_FRAME, 1, &connect_param->channel);
        pos = beacon->val + beacon->len;
        OS_MEM_CPY(pos, connect_param->bss_info->mgmt, connect_param->bss_info->mgmt_len);
        beacon->len += connect_param->bss_info->mgmt_len;
        skb_put(skb, sizeof(struct binary_wid) + beacon->len);

        checksum= (uint8 *)skb_tail_pointer(skb);
       *checksum = bytes_check_sum(beacon->val, beacon->len);
        skb_put(skb, sizeof(*checksum));
    }

    if (TRUE == cfg->conn.auto_authtype)
    {
        HWIFI_DEBUG("Save msg for share key reconnect");
        skb_reconn = skb_copy(skb, GFP_KERNEL);
        if(NULL != skb_reconn)
        {
            msg = (struct connect_msg *)(skb_reconn->data);
            hwifi_fill_char_wid(&msg->auth_type, WID_AUTH_TYPE, (uint8)(HWIFI_AUTHTYPE_SHARED_KEY | connect_param->auth_mode));
        }
        if (NULL != cfg->conn.reconn_msg)
        {
            HWIFI_DEBUG("Invalid empty reconn msg!");
            dev_kfree_skb_any(cfg->conn.reconn_msg);
        }
        cfg->conn.reconn_msg = skb_reconn;
    }

    /* send connect msg */
    ret = hwifi_send_cmd(cfg, skb);

    if (ret != SUCC)
    {
        dev_kfree_skb_any(cfg->conn.reconn_msg);
        cfg->conn.reconn_msg = NULL;
    }

    PRINT_SEND_CMD_RET("CONN", ret);
    if (!IS_P2P_ON(cfg))
    {
        /* return之前 需要是释放no_p2p_ie内存 */
        kfree(no_p2p_ie);
        no_p2p_ie = NULL;
    }

    return ret;
}

/*
 * Prototype    : hwifi_start_disconn
 * Description  : build disconnect msg with wid format and send msg
 * Input        : struct cfg_struct *cfg
                  struct hwifi_disconn_param_stru *disconn_param
 * Output       : SUCC or -EFAIL
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/27
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_start_disconn(struct cfg_struct *cfg, struct hwifi_disconn_param_stru *disconn_param)
{
    struct sk_buff  *skb;
    int32                ret;
    struct disconn_msg  *msg;              /* msg body */
    uint16               msg_size;

    HWIFI_ASSERT((NULL != cfg)&&(NULL != disconn_param));

    msg_size = sizeof(struct disconn_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long disconn msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct disconn_msg *)skb_put(skb, sizeof(struct disconn_msg));

    /* set the msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_DISCONNECT, msg_size);

    /* set disconnect wid */
    hwifi_fill_int_wid(&msg->disconnect, WID_DISCONNECT, ((((uint32)disconn_param->reason_code) << 16) | disconn_param->aid));

    hwifi_fill_char_wid(&msg->type, WID_DEL_TYPE, disconn_param->type);

    /* send msg */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("DISCONN", ret);

    return ret;
}


int32 hwifi_start_change_mode(struct cfg_struct *cfg, struct hwifi_mode_param_stru *mode_param)
{
    struct sk_buff  *skb;
    struct mode_msg *msg;            /* msg body */
    uint16           msg_size;
    int32            ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != mode_param));

    msg_size = sizeof(struct mode_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long change mode size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct mode_msg *)skb_put(skb, sizeof(struct mode_msg));

    /* set msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_MODE, msg_size);

    /* set the mode that change to */
    hwifi_fill_char_wid(&msg->mode, WID_MODE_CHANGE, mode_param->next_mode);

    /* send msg */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("CHANGE_MODE", ret);

    return ret;
}


int32 hwifi_set_short_gi(struct cfg_struct *cfg, uint8 short_gi_enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *gi;
    uint16                   msg_size;
    int32                    ret;

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct char_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        HWIFI_ERROR("Failed to alloc for beacon short GI!");
        return -EFAIL;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    gi = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(gi, WID_11N_SHORT_GI_ENABLE, short_gi_enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send GI enable msg,short_gi_enabled=%d", short_gi_enabled);
        return -EFAIL;
    }

    HWIFI_DEBUG("succeed to send GI enable msg,short_gi_enabled=%d", short_gi_enabled);

    return SUCC;
}
int32 hwifi_start_addset_beacon(struct cfg_struct *cfg, struct hwifi_beacon_param_stru  *beacon_param)
{
    struct sk_buff    *skb;
    struct beacon_msg *beacon_msg_info;
    struct binary_wid *p2p_ie;
    uint8             *msg;
    uint8             *pos,*checksum;
    int32              msg_size;
    int32              ret;

    if ((NULL == cfg) || (NULL == beacon_param))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    msg_size = sizeof(struct beacon_msg)
             + beacon_param->ssid_len
             + sizeof(struct binary_wid)
             + beacon_param->p2p_ie_len
             + beacon_param->wps_ie_len
             + 1; /* checksum */

    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long addset beacon msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, sizeof(struct beacon_msg) + beacon_param->ssid_len);

    /* fill the msg header */
    beacon_msg_info = (struct beacon_msg *)msg;
    hwifi_fill_msg_hdr(&beacon_msg_info->msg_hdr, HOST_CMD_CONFIG, msg_size);
    /* ap network,默认值为1 */
    hwifi_fill_char_wid(&beacon_msg_info->ap_network, WID_START_AP_NETWORK, 1);
    hwifi_fill_short_wid(&beacon_msg_info->interval, WID_BEACON_INTERVAL, beacon_param->interval);
    hwifi_fill_char_wid(&beacon_msg_info->dtim,WID_DTIM_PERIOD, beacon_param->dtim_period);
    /* WID_11I_MODE,接口文档为WID_11I_ENCYPT_MODE?? */
    hwifi_fill_char_wid(&beacon_msg_info->encrypt_mode, WID_11I_MODE, beacon_param->crypto_mode);
    hwifi_fill_char_wid(&beacon_msg_info->pair_encrypt,WID_11I_PAIRWISE_CIPHER_MODE, beacon_param->pairwise_crypto);
    hwifi_fill_char_wid(&beacon_msg_info->group_encrypt,WID_11I_GROUP_CIPHER_MODE, beacon_param->group_crypto);
    hwifi_fill_char_wid(&beacon_msg_info->auth_type, WID_AUTH_TYPE, beacon_param->auth_type);
    hwifi_fill_short_wid(&beacon_msg_info->channel_info, WID_USER_PREF_CHANNEL, beacon_param->channel);
    hwifi_fill_char_wid(&beacon_msg_info->enable_20M_40M, WID_2040_ENABLE, beacon_param->enable_20M_40M);
    hwifi_fill_char_wid(&beacon_msg_info->coexist_20M_40M, WID_2040_COEXISTENCE, beacon_param->coexist_20M_40M);
    hwifi_fill_char_wid(&beacon_msg_info->ht_enabled, WID_11N_ENABLE, beacon_param->ht_enabled);
    hwifi_fill_char_wid(&beacon_msg_info->broad_ssid,WID_BCAST_SSID,beacon_param->ssid_is_hidden);
    hwifi_fill_string_wid(&beacon_msg_info->ssid, WID_SSID, (uint8)beacon_param->ssid_len, (uint8 *)beacon_param->ssid);

    /* add p2p/wps ie */
    p2p_ie = (struct binary_wid *)skb_tail_pointer(skb);
    hwifi_fill_binary_wid(p2p_ie, WID_IE_BEACON, beacon_param->wps_ie_len, beacon_param->wps_ie);
    pos = p2p_ie->val + p2p_ie->len;
    OS_MEM_CPY(pos, beacon_param->p2p_ie, beacon_param->p2p_ie_len);
    p2p_ie->len += beacon_param->p2p_ie_len;
    checksum = (uint8 *)p2p_ie->val + p2p_ie->len;
   *checksum = bytes_check_sum(p2p_ie->val, p2p_ie->len);
    skb_put(skb, sizeof(struct binary_wid) + p2p_ie->len + 1);

    /* send msg */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("ADDSET BEACON",ret);
    return ret;
}



int32  hwifi_start_set_bss(struct cfg_struct *cfg, struct hwifi_bss_param_stru *bss_param)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msg_hdr;
    uint16                   msg_size;
    int32                    ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != bss_param));

    msg_size = sizeof(struct hwifi_msg_header);

    /* size for preamble if exist */
    if (BSS_PARAM_NO_CHANGE != bss_param->short_preamble)
    {
        msg_size += sizeof(struct char_wid);
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg_hdr = (struct hwifi_msg_header *)skb_put(skb, sizeof(struct hwifi_msg_header));

    /* fill the msg */
    hwifi_fill_msg_hdr(msg_hdr, HOST_CMD_CONFIG, msg_size);

    /* set short preamble */
    if (BSS_PARAM_NO_CHANGE != bss_param->short_preamble)
    {
        #define AUTO_PREAMBLE 0
        #define LONG_PREAMBLE 1
        struct char_wid        *preamble;
        const char *preamble_name = (BSS_PARAM_DISABLE == bss_param->short_preamble)? "long premble" : "auto premble";
        uint16 preamble_value     = (BSS_PARAM_DISABLE == bss_param->short_preamble)? LONG_PREAMBLE : AUTO_PREAMBLE;

        HWIFI_DEBUG("START_SET_BSS : %s", preamble_name);
        preamble = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
        hwifi_fill_char_wid(preamble, WID_PREAMBLE, preamble_value);
    }

    /* send msg */
    HWIFI_DEBUG("Ready send bss param cmd to device");
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("SET_BSS", ret);

    return ret;
}


int32  hwifi_start_set_hwmode(struct cfg_struct *cfg, struct hwifi_hwmode_param_stru *hwmode_param)
{
    struct sk_buff  *skb;
    struct hwifi_msg_header *msg_hdr;
    uint16                   msg_size;
    int32                    ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != hwmode_param));

    msg_size = sizeof(struct hwifi_msg_header);

    if (hwmode_param->hw_mode < HW_MODE_NUM)
    {
        msg_size += sizeof(struct char_wid);    /* for hw mode */
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg_hdr = (struct hwifi_msg_header *)skb_put(skb, sizeof(struct hwifi_msg_header));

    /* fill the msg */
    hwifi_fill_msg_hdr(msg_hdr, HOST_CMD_CONFIG, msg_size);

    /* set hw mode */
    if (hwmode_param->hw_mode < HW_MODE_NUM)
    {
        struct char_wid *hw_mode;
        uint8            hw_mode_val;

        hw_mode = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
        switch(hwmode_param->hw_mode)
        {
            /* 0:b,1:g,2:a */
            case 0:
                hw_mode_val = HWIFI_MODE_B_ONLY;
                break;

            case 1:
                if(IS_P2P_ON(cfg))
                {
                    /* if p2p is on,change to g only mode */
                    hw_mode_val = HWIFI_MODE_G_ONLY;
                }
                else
                {
                    hw_mode_val = HWIFI_MODE_BG_1;
                }
                break;

            case 2:
                hw_mode_val = HWIFI_MODE_A_ONLY;
                break;

            default:
                hw_mode_val = HWIFI_MODE_BG_1; /* default: HWIFI_MODE_BG_1 */
                break;
        }

        hwifi_fill_char_wid(hw_mode, WID_11G_OPERATING_MODE, hw_mode_val);
    }

    /* send msg */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("HW MODE", ret);

    return ret;
}
int32  hwifi_start_set_pmksa(struct cfg_struct *cfg, struct hwifi_pmksa_stru *pmksa_param)
{
    struct sk_buff  *skb;
    struct pmksa_msg       *msg;        /* msg body */
    uint16                  msg_size;
    int32                   ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != pmksa_param));

    msg_size = sizeof(struct pmksa_msg);
    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("Invalid too long set pmksa msg size:%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct pmksa_msg *)skb_put(skb, sizeof(struct pmksa_msg));

    /* set the msg */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    /* set pmkid wid */
    msg->pmksa_wid               = OS_CPU_TO_LE16(WID_PMKID_INFO);
    msg->pmksa_wid_len           = 1 + MAC_LEN + PMKID_LEN;
    msg->pmksa_wid_value.n_pmksa = 1;
    OS_MEM_CPY(msg->pmksa_wid_value.bssid, pmksa_param->bssid, MAC_LEN);
    OS_MEM_CPY(msg->pmksa_wid_value.pmkid, pmksa_param->pmkid, PMKID_LEN);

    /* send msg */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("SET_PMKSA", ret);

    return ret;
}

/*
 * Prototype    : hwifi_query_stats
 * Description  :
 * Input        : struct cfg_struct *cfg
                  struct hwifi_stats_param_stru   *stats_param
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/13
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
int32 hwifi_query_stats(struct cfg_struct *cfg, struct hwifi_stats_param_stru *param)
{
    struct sk_buff   *skb;
    struct stats_msg *msg;
    int32            ret;
    uint16           msg_size;

    HWIFI_ASSERT((NULL != cfg) && (NULL != param));

    msg_size = sizeof(struct stats_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct stats_msg *)skb_put(skb, sizeof(struct stats_msg));

    /* fill msg */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_STATS, msg_size);
    hwifi_fill_char_wid(&msg->stats_wid, WID_CHAR_STATS, param->aid);

    /* send msg */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("STATS", ret);

    return ret;
}

/*
 * Prototype    : hwifi_set_mac
 * Description  :
 * Input        : struct cfg_struct *cfg
                  uint8 *mac
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/3
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
int32 wl_set_mac(struct cfg_struct *cfg, uint8* mac)
{
    struct sk_buff   *skb;
    struct set_mac_msg *msg;
    int32               ret;
    uint16              msg_size;

    HWIFI_ASSERT((NULL != cfg) && (NULL != mac));

    msg_size = sizeof(struct set_mac_msg) + MAC_LEN;

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct set_mac_msg *)skb_put(skb, sizeof(struct set_mac_msg) + MAC_LEN);

    /* set the msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    hwifi_fill_string_wid(&msg->mac_wid, WID_MAC_ADDR, MAC_LEN, mac);

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("SET MAC", ret);

    return ret;
}

void hwifi_get_rate_info(DEVICE_STATS_STRU stStatInfo, struct rate_info *rate_info)
{

    if(stStatInfo.ucTxRateIdx >> 4 == 0x8)/*查找到11n的速率索引*/
    {
        int index;
        for(index = 0; index < sizeof(stTxRate)/sizeof(TX_RATE_STRU); index++) /*查表匹配对应的速率值*/
        {
            if( stTxRate[index].ucTxRateIdx == stStatInfo.ucTxRateIdx)
            {
                rate_info->legacy = 10*(stTxRate[index].ucTxRate);
                rate_info->flags = stStatInfo.ucTxRateFlg ;
                if( 0 == (rate_info->flags & RATE_INFO_FLAGS_MCS))/*if flag bit0 is 0,change it to 1*/
                {
                    HWIFI_WARNING("hwifi_recv_stats_result:receive 11n rate with wrong mcs flag");
                    rate_info->flags |= RATE_INFO_FLAGS_MCS;
                }
                    rate_info->mcs =0x0f &(stStatInfo.ucTxRateIdx);
                break;
                }
        }
        if ((sizeof(stTxRate)/sizeof(TX_RATE_STRU)) == index)
        {
            HWIFI_WARNING("hwifi_recv_stats_result:receive wrong ucTxRateIdx");
            }
        }
    else /*非11n的速率索引即为其速率值*/
    {
        rate_info->legacy = 10*(stStatInfo.ucTxRateIdx);
        rate_info->mcs = 0;
        rate_info->flags = 0;
    }
    return;

}

#define RELATIVE_VALUE_POS_H 10
#define RELATIVE_VALUE_POS_M 6
#define RELATIVE_VALUE_POS_L 2
#define RELATIVE_VALUE_NEG_H -10
#define RELATIVE_VALUE_NEG_M -6
#define RELATIVE_VALUE_NEG_L -2
#define FACTOR_H 5
#define FACTOR_M 3
#define FACTOR_L 2
#define ABS(a,b)  ((a) >= 0 ? (b) : (-(b)))

static int32 smooth_signal_rssi(struct cfg_struct *cfg, int32 rssi)
{
    int32 factor;
    int32 delta;
    /*规避芯片bug，有时device会上报0值，此处将0值屏蔽*/
    if( 0 == rssi)
    {
        return cfg->latest_signal;
    }
    /*取当前值和历史值的差值，差值在-10~10范围外，取-10或10*/
    delta = rssi - cfg->latest_signal;
    if( delta > RELATIVE_VALUE_POS_H)
    {
        delta = RELATIVE_VALUE_POS_H;
    }
    else if( delta < RELATIVE_VALUE_NEG_H)
    {
        delta = RELATIVE_VALUE_NEG_H;
    }
    /*获取差值比重因子，1/5,1/3或1/2*/
    if( delta > RELATIVE_VALUE_POS_M || delta < RELATIVE_VALUE_NEG_M )
    {
        factor = FACTOR_H;
    }
    else if( delta > RELATIVE_VALUE_NEG_L && delta < RELATIVE_VALUE_POS_L)
    {
        factor = FACTOR_L;
    }
    else
    {
        factor = FACTOR_M;
    }
    /*计算平滑后的rssi*/
    cfg->latest_signal = cfg->latest_signal + (delta*2 + ABS(delta,factor))/(factor*2);
    HWIFI_DEBUG("rssi=%d, cfg->latest_signal%d",rssi, cfg->latest_signal);
    return cfg->latest_signal;

}
/*
 * Prototype    : hwifi_recv_stats_result
 * Description  :
 * Input        : struct cfg_struct *cfg
                  uint8             *buf
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/13
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
int32 hwifi_recv_stats_result(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_STAT_QUERY_RESULT_STRU *msg;
    struct hwifi_stats_result_stru  stats_result;
    int32                           ret;
    UWORD8                          flag;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    OS_MEM_SET(&stats_result, 0, sizeof(struct hwifi_stats_result_stru));

    /* TODO fill statistic information into st_stats_result*/
    msg = (CFG_MSG_STAT_QUERY_RESULT_STRU*)buf;

    stats_result.mac             = msg->aucApMacAddr;
    stats_result.rx_packets      = OS_LE32_TO_CPU(msg->stStatInfo.ulRxPkt);
    stats_result.tx_packets      = OS_LE32_TO_CPU(msg->stStatInfo.ulTxPkt);
    stats_result.rx_bytes        = (uint32)OS_LE32_TO_CPU(msg->stStatInfo.stRxPktBit.ulLow);
    stats_result.tx_bytes        = (uint32)OS_LE32_TO_CPU(msg->stStatInfo.stTxPktBit.ulLow);

    stats_result.tx_retries      = OS_LE32_TO_CPU(msg->stStatInfo.ulTxRetry);
    stats_result.tx_failed       = OS_LE32_TO_CPU(msg->stStatInfo.ulTxFailed);
    stats_result.rx_dropped_misc = OS_LE32_TO_CPU(msg->stStatInfo.ulRxDropped);

    stats_result.signal          = smooth_signal_rssi(cfg, msg->stStatInfo.cRssi);
    /*如果获取的rssi为正值，需修改为-1上报*/
    if(stats_result.signal > 0)
    {
        stats_result.signal = -1;
    }

	stats_result.signal -= get_wifi_rssi();
	HWIFI_DEBUG("======eanble rssi fix, rssi=%d===========\n", get_wifi_rssi());
#ifdef __ROAM__
    cfg->roam.roam_fsm.roam_scan_interval_s = calc_scan_interval(&cfg->roam, cfg->latest_signal);
#endif

    flag = msg->stStatInfo.ucTxRateFlg;
    msg->stStatInfo.ucTxRateFlg = 0;
    if (flag&BIT(0))
    {
        msg->stStatInfo.ucTxRateFlg |= RATE_INFO_FLAGS_MCS;
    }
    if (flag&BIT(1))
    {
        msg->stStatInfo.ucTxRateFlg |= RATE_INFO_FLAGS_40_MHZ_WIDTH;
    }
    if (flag&BIT(2))
    {
        msg->stStatInfo.ucTxRateFlg |= RATE_INFO_FLAGS_SHORT_GI;
    }

    hwifi_get_rate_info(msg->stStatInfo, &stats_result.txrate);
    HWIFI_DEBUG("device report value: rssi=%d, stats_result.signal=%d,ucTxRateIdx = %02X,ucTxRateFlg = %02X;mstats_result.txrate.legacy =%d,txrate.mcs=%d,txrate.flags=%02X",
                 msg->stStatInfo.cRssi,
                 stats_result.signal,
                 msg->stStatInfo.ucTxRateIdx,
                 msg->stStatInfo.ucTxRateFlg,
                 stats_result.txrate.legacy,
                 stats_result.txrate.mcs,
                 stats_result.txrate.flags);

    if (g_device_tx_status++ > 3)
    {
        g_device_tx_status = 0;

        /* monitor for device tx status */
        HWIFI_INFO("device report value: rssi = %d, signal = %d, tx_succ = %d, tx_failed = %d, tx_retry = %d, rate_idx = %d",
          msg->stStatInfo.cRssi,
          stats_result.signal,
          msg->stStatInfo.ulTxPkt,
          msg->stStatInfo.ulTxFailed,
          msg->stStatInfo.ulTxRetry,
          msg->stStatInfo.ucTxRateIdx);
    }

    ret = report_stats_result(cfg, &stats_result);

    #ifdef __ROAM__
    hwifi_roam_calc_tx_failed_scale(cfg, stats_result.tx_packets, stats_result.tx_failed, stats_result.txrate.legacy/10);
    #endif

    PRINT_REPORT_EVT_RET("stats", ret);

    return ret;
}
int32 hwifi_recv_scan_inform(struct cfg_struct *cfg, uint8 *buf)
{

    CFG_MSG_NETWORK_INFO_STRU *msg;
    struct scanned_bss_event   scan_result;
    int32                      ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    memset(&scan_result, 0, sizeof(struct scanned_bss_event));

    /* get scan result form WID msg */
    msg = (CFG_MSG_NETWORK_INFO_STRU *)buf;

    scan_result.rssi      = (int32)(msg->cRssi);
    scan_result.freq      = hwifi_channel_index_to_freq(msg->ucChan);
    scan_result.mgmt_len  = msg->usNetWorkMsgLength;
    scan_result.mgmt      = (uint8*)(msg + 1);

    if (0 == scan_result.freq)
    {
        HWIFI_WARNING("Invalid unsupported channel num: %u", msg->ucChan);
        return -EFAIL;
    }

    /* report scan result */
    ret = report_scanned_bss(cfg, &scan_result);
    PRINT_REPORT_EVT_RET("Scanned bss", ret);

    return ret;
}


int32 hwifi_recv_scan_complete(struct cfg_struct *cfg, uint8 *buf)
{

    CFG_MSG_SCAN_COMP_STRU     *msg;
    struct scan_complete_stru   scan_complete;   /* scan complete reason */
    int32                       ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    OS_MEM_SET(&scan_complete, 0, sizeof(struct scan_complete_stru));

    /* get scan complete reason form WID msg */
    msg = (CFG_MSG_SCAN_COMP_STRU *)buf;
    scan_complete.reason = msg->stMsgHdr.usMsgSubStatus;

    /* report scan complete */
    ret = report_scan_done(cfg, &scan_complete);
    PRINT_REPORT_EVT_RET("Scan complete", ret);

    return ret;
}

/*
 * Prototype    : hwifi_get_conn_status
 * Description  : change connect status from Device level result to  mid
                  level  result
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/26
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
STATIC int32 hwifi_get_conn_status(enum ASOC_STATUS_TYPE_ENUM dev_status)
{
    switch(dev_status)
    {
        case ASOC_STATUS_SUCC:
            return CONN_SUCC;
        case ASOC_STATUS_AUTH_FAILED:
            return AUTH_FAILED;
        case ASOC_STATUS_ASOC_FAILED:
            return ASOC_FAILED;
        default:
            return UNSPECIFIED_FAILED;
    }
}

/*
 * Prototype    : hwifi_recv_conn_res
 * Description  : get connect result
 * Input        : struct cfg_struct *cfg
                  uint8             *buf
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/28
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_recv_conn_res(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_CONNECT_RESULT_STRU    *pst_msg;
    struct hwifi_conn_result_stru   connect_res;        /* connect result stru */
    struct mgmt_buf                *assoc_req_frame;    /* assoc request frame */
    struct mgmt_buf                *assoc_resp_frame;   /* assoc response frame */
    uint8                          *pos;                /* pos for operation */
    int32                           ret;
    struct hwifi_disconn_param_stru disconn_param;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    OS_MEM_SET(&connect_res, 0, sizeof(connect_res));

    /* get connect result */
    pst_msg                    = (CFG_MSG_CONNECT_RESULT_STRU *)buf;
    connect_res.connect_status = hwifi_get_conn_status(pst_msg->stMsgHdr.usMsgSubStatus);
    connect_res.status_code    = OS_LE16_TO_CPU(pst_msg->usCode);

    if ((TRUE == cfg->conn.auto_authtype)
    /*   && (AUTH_TYPE_ERROR == connect_res.status_code) */
       && (AUTH_FAILED == connect_res.connect_status)
       && (cfg->conn.reconn_msg != NULL))
    {
        HWIFI_INFO("Send connect msg when connect fail");
        cfg->conn.auto_authtype = FALSE;
        ret = hwifi_send_cmd(cfg, cfg->conn.reconn_msg);
        cfg->conn.reconn_msg = NULL;
        return ret;
    }
    else if (TRUE == cfg->conn.auto_authtype)
    {
        HWIFI_INFO("Release skb buf in connect result[%s]", connect_res.connect_status == CONN_SUCC ? "SUCC" : "FAILED");
        dev_kfree_skb_any(cfg->conn.reconn_msg);
        cfg->conn.auto_authtype = FALSE;
        cfg->conn.reconn_msg    = NULL;
    }

    /* get bssid and mgmt frame */
    if (ASOC_STATUS_SUCC == pst_msg->stMsgHdr.usMsgSubStatus)
    {
        OS_MEM_CPY(connect_res.bssid, pst_msg->aucApMacAddr, MAC_LEN);
        pos = (uint8 *)(pst_msg + 1);

        assoc_req_frame           = (struct mgmt_buf *)pos;
        pos  += sizeof(struct mgmt_buf) + assoc_req_frame->mgmt_len;
        assoc_resp_frame          = (struct mgmt_buf *)pos;

        /* get assoc req ie */
        connect_res.req_ie_len = OS_LE16_TO_CPU(assoc_req_frame->mgmt_len - ASSOC_REQ_IE_OFFSET);
        connect_res.req_ie = assoc_req_frame->mgmt_val + ASSOC_REQ_IE_OFFSET;


        /* get assoc resp ie */
        connect_res.resp_ie_len = OS_LE16_TO_CPU(assoc_resp_frame->mgmt_len - ASSOC_RESP_IE_OFFSET);
        connect_res.resp_ie    = assoc_resp_frame->mgmt_val + ASSOC_RESP_IE_OFFSET;
        g_dev_err_subcnt = 0;
        HWIFI_INFO("RECV_CONN_RES : assoc_req_frame->mgmt_len = %u,assoc_resp_frame->mgmt_len = %u",
                    connect_res.req_ie_len, connect_res.resp_ie_len);
    }

    /* report connect result */
    ret = report_connect_event(cfg, &connect_res);
    PRINT_REPORT_EVT_RET("connect result", ret);

    if ((-EFAIL == ret) && (HWIFI_CONNECTING != cfg->conn.status))
    {
        /* the state of host and device is not the same, synchronize it. */
        HWIFI_WARNING("current status is %d,set host and device state to disconnect.", cfg->conn.status);

        memset(&disconn_param, 0, sizeof(disconn_param));
        disconn_param.reason_code = 1; /* unknow reason */
        hwifi_start_disconn(cfg, &disconn_param);

        cfg->conn.status = HWIFI_DISCONNECTED;
    }

    connect_init_11b_params(cfg);

    if (ASOC_STATUS_SUCC != pst_msg->stMsgHdr.usMsgSubStatus)
    {
        strncpy(cfg->beacon_ie_country_code,INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN);
        ret = hwifi_regdomain_update(cfg);
    }

    return ret;
}

int32 hwifi_connectting_fail_report_conn_res(struct cfg_struct *cfg, int32 connect_status, uint16 status_code)
{
    int32 ret;
    struct hwifi_conn_result_stru   connect_res;
    OS_MEM_SET(&connect_res, 0, sizeof(connect_res));
    connect_res.connect_status = connect_status;
    connect_res.status_code    = status_code;

    ret = report_connect_event(cfg, &connect_res);
    PRINT_REPORT_EVT_RET("connecting fail, connect result ", ret);

    connect_init_11b_params(cfg);

    strncpy(cfg->beacon_ie_country_code,INVALID_COUNTRY_CODE, COUNTRY_CODE_LEN);
    ret = hwifi_regdomain_update(cfg);

    return ret;
}

/*
 * Prototype    : hwifi_sta_leave_res
 * Description  : process sta leave result in AP mode
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/25
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_report_sta_leave_res(struct cfg_struct *cfg, uint8 aid)
{
    struct hwifi_sta_leave_stru event;
    int32                       ret;

    OS_MEM_SET(&event, 0, sizeof(event));

    event.aid = aid;

    ret = report_sta_leave_event(cfg, &event);
    PRINT_REPORT_EVT_RET("sta leave", ret);

    return ret;
}

/*
 * Prototype    : hwifi_diconnect_res
 * Description  : process sta disconnect from ap in STA mode
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/9/25
 *     Author       : Hash
 *     Modification : Created function
 *
 */
int32 hwifi_report_diconnect_res(struct cfg_struct *cfg, uint16 reason)
{
    struct hwifi_disconn_result_stru  disconn_res;
    int32                             ret;

    memset(&disconn_res, 0, sizeof(disconn_res));

    disconn_res.reason_code = reason;

    /* device上报去关联后,立即删除扫描列表信息 */
    clean_scanned_bss_immediately(&cfg->scan.bss_mgmt);

#ifdef WLAN_POWER_MANAGEMENT
    /* 去关联时，将watchdog timer删除 */
    if (g_powermgmt_switch)
    {
        if (cfg->hi110x_dev->pm_info != NULL)
        {
            ret = hi110x_pm_stop_wdg(cfg->hi110x_dev->pm_info);
            if (ret < 0)
            {
                HWIFI_WARNING("stop watchdog timer Failed");
            }
        }
        else
        {
            HWIFI_WARNING("pm_info ptr is NULL...");
        }
    }
#endif

    /* let stop_wdg before report event, avoid wdg 100ms timeout */
    ret = report_disconnect_event(cfg, &disconn_res);
    PRINT_REPORT_EVT_RET("disconnect", ret);

    return ret;
}


/*
 * Prototype    : hwifi_recx_disconn_res
 * Description  : get disconnect result
 * Input        : struct cfg_struct *cfg,
                  uint8 *buf,
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/6/28
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_recv_disconn_res(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_DISCONNECT_RESULT_STRU *msg;
    int32                           ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    msg = (CFG_MSG_DISCONNECT_RESULT_STRU *)buf;

    /* if disconnect started by host, don't report it */
    if (0 == msg->stMsgHdr.usMsgSubStatus)
    {
        HWIFI_INFO("Sync disconnect");

        /*
         * if disconnect actively, restore previous regulatory domain rules before
         * connect.
         */
        ret = hwifi_regdomain_update(cfg);
        if (ret != SUCC)
        {
            HWIFI_WARNING("disconnect result active:hwifi regdom restore by default fail.");
        }


        #ifdef WLAN_ARRG_DYNAMIC_CONTROL
        hwifi_stop_timer(cfg);
        #endif

        disconnect_init_11b_params(cfg);

        return SUCC;
    }

    if (msg->ucAid > 0)
    {
        /* AP Mode */
        ret = hwifi_report_sta_leave_res(cfg, msg->ucAid);
    }
    else
    {
        /* STA Mode */
        ret = hwifi_report_diconnect_res(cfg, msg->stMsgHdr.usMsgSubStatus);
    }

    disconnect_init_11b_params(cfg);

    return ret;
}

/*
 * Prototype    : hwifi_recv_sta_join_info
 * Description  :
 * Input        : struct cfg_struct *cfg
                  uint8             *buf
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/16
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
int32 hwifi_recv_sta_join_info(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_ASSOC_STA_STRU *msg;
    int32                   ret;
    struct hwifi_sta_join_stru event;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    msg = (CFG_MSG_ASSOC_STA_STRU *)buf;

    OS_MEM_SET(&event, 0, sizeof(event));
    event.aid = msg->ucAid;
    event.mac = msg->aucApMacAddr;
    event.success = (0 == msg->ucStaType) ? FALSE : TRUE;

    ret = report_sta_join_event(cfg, &event);
    PRINT_REPORT_EVT_RET("sta join", ret);

    return ret;
}


int32 hwifi_recv_chang_mode_res(struct cfg_struct *cfg, uint8 *buf)
{
    struct mode_resp           *msg;
    struct hwifi_set_mode_stru  change_mode_res;
    int32                       ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    /* get change result */
    msg                             = (struct mode_resp *)buf;
    change_mode_res.hwifi_recv_mode = msg->dev_mode;
    change_mode_res.set_mode_res    = (uint8)(OS_LE16_TO_CPU(msg->sub_status));
    ret = report_change_mode_resp(cfg, &change_mode_res);
    PRINT_REPORT_EVT_RET("change mode", ret);

    return ret;
}
int32 hwifi_recv_config_result(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_SET_RESULT_STRU     *msg;
    struct hwifi_change_res_stru set_param_res;
    int32                        ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    /* get set result */
    msg                         = (CFG_MSG_SET_RESULT_STRU *)buf;
    set_param_res.set_param_res = (uint8)(OS_LE16_TO_CPU(msg->stMsgHdr.usMsgSubStatus));

    ret = report_set_param_resp(cfg, &set_param_res);
    PRINT_REPORT_EVT_RET("config result", ret);

    return ret;
}

/*
 * Prototype    : hwifi_recv_mgmt_frame
 * Description  : receive mgmt frame and report it
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/16
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_recv_mgmt_frame(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_MGMT_FRAME_STRU       *msg;
    struct mgmt_report_event_stru  mgmt;
    int32                          ret;

    if ((NULL == cfg) || (NULL == buf))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    memset(&mgmt, 0, sizeof(struct mgmt_report_event_stru));

    /* get scan result form WID msg */
    msg = (CFG_MSG_MGMT_FRAME_STRU *)buf;

    mgmt.freq      = hwifi_channel_index_to_freq(msg->ucChan);
    mgmt.type      = OS_LE16_TO_CPU(msg->stMsgHdr.usMsgSubStatus);
    mgmt.mgmt_len  = OS_LE16_TO_CPU(msg->usMgmtFrmLength);
    mgmt.mgmt      = (uint8*)(msg + 1);
    mgmt.u.value     = OS_LE16_TO_CPU(msg->usAid);

    if (0 == mgmt.freq)
    {
        HWIFI_WARNING("Unsupported channel num: %u", msg->ucChan);
        return -EFAIL;
    }

    /* report scan result */
    ret = report_mgmt_frame(cfg, &mgmt);
    PRINT_REPORT_EVT_RET("mgmt frame", ret);

    return ret;
}


int32 hwifi_recv_init_result(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_SET_RESULT_STRU         *msg;
    struct hwifi_init_res_stru      init_param_res;
    int32                           ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    /* get set result */
    msg                           = (CFG_MSG_SET_RESULT_STRU *)buf;
    init_param_res.init_param_res = (uint8)(OS_LE16_TO_CPU(msg->stMsgHdr.usMsgSubStatus));

    ret = report_init_param_resp(cfg, &init_param_res);
    PRINT_REPORT_EVT_RET(" init_param_result", ret);

    return ret;
}
int32 hwifi_recv_status_ready(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_MODE_RESULT_STRU        *msg;
    struct hwifi_status_ready_stru  set_param_res;
    int32                           ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    /* get set result */
    msg                                 = (CFG_MSG_MODE_RESULT_STRU *)buf;
    set_param_res.ready_status_res      = (uint8)(OS_LE16_TO_CPU(msg->stMsgHdr.usMsgSubStatus));
    cfg->hi110x_dev->mode.current_mode  = (uint8)(OS_LE16_TO_CPU(msg->ucMode)); /* Device模式 */
    HWIFI_BUG_ON(!cfg->hi110x_dev);
    hcc_trans_limit_default_config(cfg->hi110x_dev->hcc, cfg->hi110x_dev->mode.current_mode);


    ret = hwifi_dev_ready_event(cfg, &set_param_res);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to report status ready result");
        return -EFAIL;
    }

    HWIFI_DEBUG("Current_mode=%d", cfg->hi110x_dev->mode.current_mode);

    return SUCC;
}

/************************************************************
 * Prototype    : hwifi_recv_ba_session
 * Description  : get mac address ba sessiones
 * Input        : struct cfg_struct *cfg, uint8 *buf
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    : hwifi_proc_asyn_msg
 *
 *   History        :
 *   1.Date         : 2013/03/22
 *     Author       : ywx164715
 *     Modification : Created function
 *
 ***********************************************************/
int32 hwifi_recv_ba_session(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_BA_SESSION_STRU        *msg;
    uint8                           i;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));
    /* get query result */
    msg = (CFG_MSG_BA_SESSION_STRU *)buf;

    if(msg->stMsgHdr.usMsgSubStatus == 2)
    {
        HWIFI_WARNING("Failed to find the mac address");

        cfg->aggre_info.ba_param_res.conn_num                   = 0;
        cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].id = -EFAIL;
        wake_up_interruptible(&cfg->wait_queue);

        return -EFAIL;
    }else if (msg->stMsgHdr.usMsgSubStatus == 3)
    {
        HWIFI_WARNING("No ba sessions of the mac address");

        cfg->aggre_info.ba_param_res.conn_num                   = 0;
        cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].id = -EFAIL;
        wake_up_interruptible(&cfg->wait_queue);

        return SUCC;
    }

    memcpy(cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].mac_addr, msg->ba_sess_data.aucDestMacAddr, MAC_LEN);

    cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].tx_ba_num = msg->ba_sess_data.ucTxBaSessionCount;
    cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].rx_ba_num = msg->ba_sess_data.ucRxBaSessionCount;

    for(i = 0; i < cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].tx_ba_num; i++)
    {
        cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].tx_tid[i]    = msg->ba_sess_data.aucTxBaTid[i];
    }

    for(i = 0; i < cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].rx_ba_num; i++)
    {
        cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].rx_tid[i]    = msg->ba_sess_data.aucRxBaTid[i];
    }

    cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].id        = msg->ba_sess_data.ucAid;
    cfg->aggre_info.ba_param_res.conn_num                          = 1;
    wake_up_interruptible(&cfg->wait_queue);


    return SUCC;
}


/************************************************************
 * Prototype    : hwifi_recv_all_ba_session
 * Description  : get all mac address ba sessiones
 * Input        : struct cfg_struct *cfg, uint8 *buf
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    : hwifi_proc_asyn_msg
 *
 *   History        :
 *   1.Date         : 2013/03/23
 *     Author       : ywx164715
 *     Modification : Created function
 *
 ***********************************************************/
int32 hwifi_recv_all_ba_session(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_ALL_BA_SESSION_STRU     *msg;
    int32                           conn_num;
    uint8                           i, j, k, n;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    /* get query result */
    msg = (CFG_MSG_ALL_BA_SESSION_STRU *)buf;

    if(msg->stMsgHdr.usMsgSubStatus == 0)
    {
        HWIFI_WARNING("BA_SESSION_RES : no any connected mac address");

        cfg->aggre_info.ba_param_res.conn_num                   = 0;
        cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].id = 0xFF;
        wake_up_interruptible(&cfg->wait_queue);

        return SUCC;
    }

    conn_num = msg->stMsgHdr.usMsgSubStatus;

    for(i = 0; i < conn_num; i++)
    {
        k = 0;
        n = 0;

        cfg->aggre_info.ba_param_res.ba_sess_res[i].id = i;
        memcpy(cfg->aggre_info.ba_param_res.ba_sess_res[i].mac_addr, msg->ba_sess_data[i].aucDestMacAddr, MAC_LEN);

        for(j = 0; j < 8; j++)
        {
            if(msg->ba_sess_data[i].ucTxBaTidsMap & (0x01 << j))
            {
                cfg->aggre_info.ba_param_res.ba_sess_res[i].tx_tid[k++] = j;
            }

            if(msg->ba_sess_data[i].ucRxBaTidsMap & (0x01 << j))
            {
                cfg->aggre_info.ba_param_res.ba_sess_res[i].rx_tid[n++] = j;
            }
        }

        cfg->aggre_info.ba_param_res.ba_sess_res[i].tx_ba_num = k;
        cfg->aggre_info.ba_param_res.ba_sess_res[i].rx_ba_num = n;
    }

    cfg->aggre_info.ba_param_res.conn_num       = conn_num;

    wake_up_interruptible(&cfg->wait_queue);

    HWIFI_DEBUG("BA_SESSION_RES : leaving %s", __FUNCTION__);

    return SUCC;
}

/*
 * Prototype    : hwifi_recv_mic_error
 * Description  : process mic error event
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/11
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_recv_mic_error(struct cfg_struct *cfg, uint8 *buf)
{
    struct mic_error_report_stru mic_error;
    CFG_MSG_TKIP_MIC_ERROR_STRU *msg;
    int  ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    msg = (CFG_MSG_TKIP_MIC_ERROR_STRU *)buf;
    if (msg->stMsgHdr.usMsgStatus == FALSE)
    {
        HWIFI_INFO("No mic error");
        return SUCC;
    }

    mic_error.addr     = msg->SourceAddr;
    mic_error.key_type = msg->ucKeyType ? HWIFI_KEYTYPE_GROUP : HWIFI_KEYTYPE_PAIRWISE;
    mic_error.key_id   = MISS_ID;

    HWIFI_INFO("Receive mic error event from device,addr:"MACFMT ",key_type:%d,key_id:%d",
                MAC2STR(mic_error.addr), (int32)mic_error.key_type, mic_error.key_id);

    ret = report_mic_error_event(cfg, &mic_error);
    return ret;
}



int32 hwifi_cmd_add_key(struct cfg_struct *cfg, struct priv_key_param *key, int8 *mac_addr, bool pairwise, int8 index)
{
    struct sk_buff *skb;
    struct _add_key_info    *add_key_data;
    int32                   ret;

    /* if key is wep ,we should handle individually */
    HWIFI_DEBUG("START_ADD_KEY : key->cipher = 0x%8X", key->cipher);
    if ((HWIFI_CIPHER_SUITE_WEP40 == key->cipher) || (HWIFI_CIPHER_SUITE_WEP104 == key->cipher))
    {
        return SUCC;
    }

    skb = hwifi_alloc_skb_for_cmd(sizeof(struct _add_key_info));
    if (NULL == skb)
    {
        return -EFAIL;
    }

    add_key_data = (struct _add_key_info *)skb_put(skb, sizeof(struct hwifi_msg_header));

    hwifi_fill_msg_hdr(&add_key_data->header, HOST_CMD_CONFIG, sizeof(add_key_data->header));

    if (pairwise)
    {
        add_key_data->wid_hdr.wid    = WID_ADD_PTK;
        add_key_data->wid_hdr.len    = sizeof(add_key_data->ptk_dat);

        OS_MEM_CPY(add_key_data->ptk_dat.des_addr, mac_addr, MAC_LEN);
        add_key_data->ptk_dat.key_len = key->key_len;
        if (HWIFI_CIPHER_SUITE_TKIP == key->cipher)
        {
            HWIFI_INFO("START_ADD_KEY : install tkip key");
            OS_MEM_CPY(add_key_data->ptk_dat.tk1, key->key, 16);
            if (HI_MODE_AP == cfg->hi110x_dev->mode.current_mode)
            {
                //end
                OS_MEM_CPY(add_key_data->ptk_dat.mic.michel_rx, key->key + MIC_TX_OFFSET, MIC_TX_LEN);
                OS_MEM_CPY(add_key_data->ptk_dat.mic.michel_tx, key->key + MIC_RX_OFFSET, MIC_RX_LEN);
            }
            else
            {
                OS_MEM_CPY(add_key_data->ptk_dat.mic.michel_rx, key->key + MIC_RX_OFFSET, MIC_RX_LEN);
                OS_MEM_CPY(add_key_data->ptk_dat.mic.michel_tx, key->key + MIC_TX_OFFSET, MIC_TX_LEN);
            }
        }
        else
        {
            HWIFI_INFO("START_ADD_KEY : install ccmp key");
            OS_MEM_CPY((char *)&add_key_data->ptk_dat.tk1[0], key->key, key->key_len);
        }

        add_key_data->header.msg_len        += sizeof(add_key_data->wid_hdr);
        add_key_data->header.msg_len        += sizeof(add_key_data->ptk_dat);

    skb_put(skb, sizeof(add_key_data->wid_hdr) + sizeof(add_key_data->ptk_dat));

        HWIFI_DEBUG( "START_ADD_KEY : addkey: ptk_dat len=%zu",sizeof(add_key_data->ptk_dat));
    }
    else
    {
        HWIFI_DEBUG("START_ADD_KEY : add gtk");
        add_key_data->wid_hdr.wid    = WID_ADD_RX_GTK;
        add_key_data->wid_hdr.len    = sizeof(add_key_data->gtk_dat);

        OS_MEM_CPY(add_key_data->gtk_dat.des_addr, mac_addr, MAC_LEN);
        OS_MEM_CPY(add_key_data->gtk_dat.key_rsc, key->seq, key->seq_len);
        add_key_data->gtk_dat.key_index      = index;
        add_key_data->gtk_dat.key_len        = key->key_len;
        if (HWIFI_CIPHER_SUITE_TKIP == key->cipher)
        {
            HWIFI_INFO("START_ADD_KEY : GTK:install tkip key");
            OS_MEM_CPY(add_key_data->gtk_dat.tk1, key->key, TK1_LEN);
            if (HI_MODE_AP == cfg->hi110x_dev->mode.current_mode)
            {
                OS_MEM_CPY(add_key_data->gtk_dat.mic.michel_rx, key->key + MIC_TX_OFFSET, MIC_TX_LEN);
                OS_MEM_CPY(add_key_data->gtk_dat.mic.michel_tx, key->key + MIC_RX_OFFSET, MIC_RX_LEN);
            }
            else
            {
                OS_MEM_CPY(add_key_data->gtk_dat.mic.michel_rx, key->key + MIC_RX_OFFSET, MIC_RX_LEN);
                OS_MEM_CPY(add_key_data->gtk_dat.mic.michel_tx, key->key + MIC_TX_OFFSET, MIC_TX_LEN);
            }
        }
        else
        {
            HWIFI_INFO("START_ADD_KEY : GTK:install ccmp key");
            OS_MEM_CPY((char *)&add_key_data->gtk_dat.tk1[0], key->key, key->key_len);
        }

        add_key_data->header.msg_len        += sizeof(add_key_data->wid_hdr);
        add_key_data->header.msg_len        += sizeof(add_key_data->gtk_dat);

    skb_put(skb, sizeof(add_key_data->wid_hdr) + sizeof(add_key_data->gtk_dat));
    }


    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("ADD KEY", ret);

    if (((ret == SUCC) && (!pairwise)) || (HI_MODE_AP == cfg->hi110x_dev->mode.current_mode))
    {
        if(HI_MODE_AP == cfg->hi110x_dev->mode.current_mode)
        {
            if(1 == pairwise)
            {
                /* 单播添加完key需要调用此函数生效，组播由hostapd上层调用 */
                hwifi_cmd_set_defkey(cfg, key, mac_addr, index, pairwise);
            }
        }
        else
        {
            hwifi_cmd_set_defkey(cfg, key, mac_addr, index, 0);
        }
    }

    return ret;
}


int32 hwifi_cmd_set_defkey(struct cfg_struct *cfg, struct priv_key_param *key, int8 *mac_addr, uint8 index, bool unicast)
{
    struct sk_buff *skb;
    struct _set_defkey_info *defkey_dat;
    int32                     ret;

    skb = hwifi_alloc_skb_for_cmd(sizeof(struct _set_defkey_info));
    if (NULL == skb)
    {
        return -EFAIL;
    }

		/* save the ec212 params */
		g_ec212_unicast = unicast;
		g_ec212_count   = 0;

    defkey_dat = (struct _set_defkey_info *)skb_put(skb, sizeof(struct hwifi_msg_header));

    hwifi_fill_msg_hdr(&defkey_dat->header, HOST_CMD_CONFIG, sizeof(defkey_dat->header));

    defkey_dat->wid_hdr.wid  = WID_SET_DEFAULT_KEY;
    defkey_dat->wid_hdr.len  = sizeof(defkey_dat->gtk_ptk);

    memcpy(defkey_dat->gtk_ptk.mac_addr, mac_addr, MAC_LEN);
    defkey_dat->gtk_ptk.key_index  = index;
    defkey_dat->gtk_ptk.type       = ((unicast) ? 1 : 2 );

    defkey_dat->header.msg_len  += sizeof(defkey_dat->wid_hdr);
    defkey_dat->header.msg_len  += sizeof(defkey_dat->gtk_ptk);
    skb_put(skb, sizeof(defkey_dat->wid_hdr) + sizeof(defkey_dat->gtk_ptk));

    HWIFI_WARNING("START_SET_DEFKEY : set default key index = %d , type = %s \n", index, unicast ? "unicast" : "group");
    HWIFI_WARNING("START_SET_DEFKEY : set default: %s len=%zu\n",unicast ? "ptk" : "gtk", sizeof(defkey_dat->gtk_ptk));
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("SET DEFAULT KEY", ret);

    return ret;
}
int32 hwifi_cmd_remove_key(struct cfg_struct  *cfg,
                                    int8               *mac_addr,
                                    uint8               index,
                                    bool                unicast,
                                    uint16              cipher)
{
    struct sk_buff *skb;
    int32     ret;
    struct _remove_key_info *remove_key_dat;

    skb = hwifi_alloc_skb_for_cmd(sizeof(struct _remove_key_info));
    if (NULL == skb)
    {
        return -EFAIL;
    }

    remove_key_dat = (struct _remove_key_info *)skb_put(skb, sizeof(struct hwifi_msg_header));

    hwifi_fill_msg_hdr(&remove_key_dat->header, HOST_CMD_CONFIG, sizeof(remove_key_dat->header));

    if ((HWIFI_CIPHER_SUITE_WEP40 == cipher) || (HWIFI_CIPHER_SUITE_WEP104 == cipher))   /* WEP40 or WEP104 */
    {
        HWIFI_DEBUG("START_REMOVE_KEY : remove wep key\n");

        remove_key_dat->wid_hdr.wid      = WID_REMOVE_WEP_KEY;
        remove_key_dat->wid_hdr.len      = sizeof(remove_key_dat->wep);

        memcpy(remove_key_dat->wep.mac_addr, mac_addr, MAC_LEN);
        remove_key_dat->wep.key_index    = index;

        remove_key_dat->header.msg_len  += sizeof(remove_key_dat->wid_hdr);
        remove_key_dat->header.msg_len  += sizeof(remove_key_dat->wep);

    skb_put(skb, sizeof(remove_key_dat->wid_hdr) + sizeof(remove_key_dat->wep));
    }
    else
    {
        HWIFI_DEBUG("START_REMOVE_KEY : remove ptk or gtk key\n");

        remove_key_dat->wid_hdr.wid      = WID_REMOVE_KEY;
        remove_key_dat->wid_hdr.len      = sizeof(remove_key_dat->ptk_gtk);

        memcpy(remove_key_dat->ptk_gtk.mac_addr, mac_addr, MAC_LEN);
        remove_key_dat->ptk_gtk.key_index = index;
        remove_key_dat->ptk_gtk.type      = unicast ? 1 : 2;

        remove_key_dat->header.msg_len   += sizeof(remove_key_dat->wid_hdr);
        remove_key_dat->header.msg_len   += sizeof(remove_key_dat->ptk_gtk);

    skb_put(skb, sizeof(remove_key_dat->wid_hdr) + sizeof(remove_key_dat->ptk_gtk));
    }

    HWIFI_DEBUG("START_REMOVE_KEY : send remove key msg\n");

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("REMOVE KEY", ret);

    return ret;
}
int32 hwifi_recv_channel_info(struct cfg_struct *cfg, uint8 *buf, int32 len)
{
    return report_recv_channel_info(cfg, buf, len);
}

/*
 * Prototype    : hwifi_cmd_add_wapi_key
 * Description  : add key for wapi
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/29
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_cmd_add_wapi_key(struct cfg_struct *cfg, bool pairwise, int8 index, int8 *mac_addr, struct priv_key_param *key)
{
    struct sk_buff *skb;
    struct wapi_key *add_key_msg;
    int32     ret;
/*  uint8     PN[]={0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,
                    0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36};
*/
    uint8     PN[]={0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,
                    0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c};
    if((NULL == cfg) || (NULL == mac_addr) || (NULL == key))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    if (HWIFI_WAPI_KEY != key->key_len)
    {
        HWIFI_WARNING("Invalid wapi key len:%d!",key->key_len);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(sizeof(struct wapi_key));
    if (NULL == skb)
    {
        return -EFAIL;
    }

    add_key_msg = (struct wapi_key *)skb_put(skb, sizeof(struct wapi_key));

    hwifi_fill_msg_hdr(&add_key_msg->header, HOST_CMD_CONFIG, sizeof(struct wapi_key));

    add_key_msg->wid_hdr.wid = WID_ADD_WPIKEY;
    add_key_msg->wid_hdr.len = sizeof(struct wapi_key) - sizeof(struct hwifi_msg_header) - sizeof(struct _hwifi_wid_header);
    if(TRUE == pairwise)
    {
        add_key_msg->keytype = HWIFI_PAIRWISE;
        memcpy(add_key_msg->pn, PN, WAPI_PN_LEN);
    }
    else
    {
        add_key_msg->keytype = HWIFI_GROUP;
        memset(add_key_msg->pn, 0, WAPI_PN_LEN);
    }
    add_key_msg->keyid = index;

    /* only for sta mode, go mode need to do */
    memcpy(add_key_msg->ap_addr, mac_addr, MAC_LEN);
    memset(add_key_msg->sta_addr, 0, MAC_LEN);
    add_key_msg->wpi_ek_len = WAPI_KEY_LEN;
    memcpy(add_key_msg->wpi_ek, key->key, WAPI_KEY_LEN);
    add_key_msg->wpi_ck_len = WAPI_KEY_LEN;
    memcpy(add_key_msg->wpi_ck, &key->key[WAPI_KEY_LEN], WAPI_KEY_LEN);

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("add wapi key",ret);
    return ret;
}

/*
 * Prototype    : hwifi_cmd_remove_wapi_key
 * Description  : remove wapi key
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/29
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_cmd_remove_wapi_key(struct cfg_struct *cfg, bool pairwise, int8 index, int8 *mac_addr)
{
    struct sk_buff  *skb;
    struct wapi_key *del_key_msg;
    int32     ret;

    if ((NULL == cfg) || (NULL == mac_addr))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(sizeof(struct wapi_key));
    if (NULL == skb)
    {
        return -EFAIL;
    }

    del_key_msg = (struct wapi_key *)skb_put(skb, sizeof(struct wapi_key));
    memset(del_key_msg, 0, sizeof(struct wapi_key));
    hwifi_fill_msg_hdr(&del_key_msg->header, HOST_CMD_CONFIG, sizeof(struct wapi_key));
    del_key_msg->wid_hdr.wid = WID_REMOVE_WPIKEY;
    del_key_msg->wid_hdr.len = sizeof(struct wapi_key) - sizeof(struct hwifi_msg_header) - sizeof(struct _hwifi_wid_header);
    if(TRUE == pairwise)
    {
        del_key_msg->keytype = HWIFI_PAIRWISE;
    }
    else
    {
        del_key_msg->keytype = HWIFI_GROUP;
    }
    del_key_msg->keyid = index;
    memcpy(del_key_msg->ap_addr, mac_addr, MAC_LEN);

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("remove wpai key",ret);

    return ret;
}

/*
 * Prototype    : wl_get_dbb_info
 * Description  : get dbb number info
 * Input        : struct cfg_struct *cfg,
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/11/9
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32 wl_get_dbb_info(struct cfg_struct  *cfg)
{
    struct sk_buff                *skb;
    struct hwifi_query_msg_header *msghdr;
    uint8                         *msg;
    uint16                         msg_size;
    int32                          ret;

    msg_size = sizeof(struct hwifi_query_msg_header) + HISI_WIFI_DBB_LEN * sizeof(int8);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    msghdr = (struct hwifi_query_msg_header *)msg;

    hwifi_fill_query_msg_hdr(msghdr,msg_size);
    msghdr->wid_list[0] = WID_HARDWARE_VERSION;


    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("get DBB number",ret);
    return ret;


}


/*
 * Prototype    : wl_get_upc_info
 * Description  : get upc info
 * Input        : struct cfg_struct *cfg,
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/4/25
 *     Author       : zhanglu/z00217036
 *     Modification : Created function
 *
 */
int32 wl_get_upc_info(struct cfg_struct  *cfg)
{
    struct sk_buff                *skb;
    struct hwifi_query_msg_header *msghdr;
    uint8                         *msg;
    uint16                         msg_size;
    int32                          ret;

    /*查询WID upc */
    msg_size = sizeof(struct hwifi_query_msg_header) + sizeof(uint16);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    msghdr = (struct hwifi_query_msg_header *)msg;

    hwifi_fill_query_msg_hdr(msghdr,msg_size);
    msghdr->wid_list[0] = WID_UPC_STATUS;


    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("get UPC info",ret);
    return ret;


}
/*
 * Prototype    : hwifi_recv_dbb_info
 * Description  : Recive dbb info from device
 * Input        : struct cfg_struct *cfg, int8 *buf, int32 len
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/11/22
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32 hwifi_recv_dbb_info(struct cfg_struct *cfg, int8 *buf, int32 len)
{
    if ((NULL == cfg)||(NULL == buf))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    strncpy( cfg->hi110x_dev->tps->dbb,buf,HISI_WIFI_DBB_LEN );

    cfg->hi110x_dev->tps->dbb_ver_got = 0;

    HWIFI_DEBUG("DBB numbers of WIFI :%s",buf);
    wake_up_interruptible(&cfg->wait_queue);

    return SUCC;
}


int32 hwifi_recv_upc_info(struct cfg_struct *cfg, uint8 *buf, int32 len)
{
    if ((NULL == cfg)||(NULL == buf))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }
    if(len < 2)
    {
        HWIFI_WARNING("recv upc info length is %d less than 2 !",len);
        return -EFAIL;
    }
    cfg->hi110x_dev->tps->check_upc_flag = buf[0] | (buf[1] << 8) ;
    cfg->hi110x_dev->tps->check_upc_ctrl = SUCC;

    HWIFI_DEBUG("upc flag of WIFI :%d",cfg->hi110x_dev->tps->check_upc_flag);
    wake_up_interruptible(&cfg->wait_queue);

    return SUCC;
}

/*
 * Prototype    : hwifi_recv_rx_info
 * Description  : Recive rx info from device
 * Input        : struct cfg_struct *cfg, int8 *buf, int32 len
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         :
 *     Author       :
 *     Modification : Created function
 *
 */
int32 hwifi_recv_rx_info(struct cfg_struct *cfg, uint8 *buf, int32 len)
{
    if ((NULL == cfg)||(NULL == buf))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    if (len < 4)
    {
        HWIFI_WARNING("recv rx info length is %d less than 4 !",len);
        return -EFAIL;
    }

    cfg->android_ioctl.rx_count = buf[0] | (buf[1] << 8)
        | (buf[2] << 16) | (buf[3] << 24);

    cfg->android_ioctl.rx_count_read_flag = SUCC;

    HWIFI_DEBUG("rx_count_read_flag :%d", cfg->android_ioctl.rx_count_read_flag);
    wake_up_interruptible(&cfg->wait_queue);

    return SUCC;
}

/*
 * Prototype    : hwifi_recv_fem_flag_info
 * Description  : Recive fem flag info from device
 * Input        : struct cfg_struct *cfg, int8 *buf, int32 len
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         :
 *     Author       :
 *     Modification : Created function
 *
 */
int32 hwifi_recv_fem_flag_info(struct cfg_struct *cfg, uint8 *buf, int32 len)
{
    if ((NULL == cfg)||(NULL == buf))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    cfg->android_ioctl.pri_data.fem_flag = buf[0];

    HWIFI_DEBUG("fem flag of WIFI :%d()",cfg->android_ioctl.pri_data.fem_flag);
    wake_up_interruptible(&cfg->wait_queue);

    return SUCC;
}

/*
 * Prototype    : HWIFI_PROC_RES_MSG
 * Description  : process the msg that its type is MSG_TYPE_RESPONSE
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/7/6
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_proc_res_msg(struct cfg_struct *cfg, uint8 *buf)
{
    struct hwifi_query_msg_header *msghdr;
    int32                          msg_len;
    uint8                         *pos;
    int32                          wid;
    uint8                          wid_value_len;
    int32                          ret;

    msghdr = (struct hwifi_query_msg_header *)buf;
    msg_len = msghdr->msg_len - sizeof(struct hwifi_query_msg_header);

    if (0 > msg_len)
    {
        HWIFI_WARNING("Invalid too short respose msg len:%d", msghdr->msg_len);
        return -EFAIL;
    }

    ret = SUCC;

    /* 最短的wid是char类型的，1次查询可上报多个wid查询结果 */
    pos = (uint8 *)(msghdr->wid_list);
    while(msg_len >= sizeof(struct char_wid))
    {
        /* 解析TLV 格式的查询结果,type:2字节，len:1字节，Value根据len值计算 */
        wid = pos[0] | (pos[1] << 8);
        wid_value_len = pos[2];

        HWIFI_DEBUG("Wid is 0x%x,value_len = %d", wid,wid_value_len);

        switch(wid)
        {
            case WID_CURR_OPER_CHANNEL:
                ret = hwifi_recv_channel_info(cfg, pos + 3,wid_value_len);
                break;
            case WID_HARDWARE_VERSION:
                ret = hwifi_recv_dbb_info(cfg, pos + 3,wid_value_len);
                break;
            case WID_UPC_STATUS:
                ret = hwifi_recv_upc_info(cfg, pos + 3,wid_value_len);
                break;
            case WID_INT_RESERVE:
                ret = hwifi_recv_rx_info(cfg, pos + 3, wid_value_len);
                break;
            default:
                break;
        }

        if (SUCC != ret)
        {
            HWIFI_WARNING("Failed to recv channel info [ret(%d)][wid=%d]", ret, wid);
            break;
        }

        msg_len = msg_len - wid_value_len - 3; /* TLV 格式，TYPE 和 len 为3字节 */
        pos = pos + 3 + wid_value_len;

    }

    if(0 != msg_len)
    {
        HWIFI_WARNING("Parse error msg len:(%d)", msg_len);
        ret = -EFAIL;
    }

    return ret;
}
int32 hwifi_cmd_set_defkey_force(struct cfg_struct *cfg, int8 *mac_addr, uint8 index, bool unicast)
{
		struct sk_buff *skb;
		struct _set_defkey_info *defkey_dat;
		int32                     ret;

		skb = hwifi_alloc_skb_for_cmd(sizeof(struct _set_defkey_info));
		if (NULL == skb)
		{
			return -EFAIL;
	  }

		defkey_dat = (struct _set_defkey_info *)skb_put(skb, sizeof(struct hwifi_msg_header));

		hwifi_fill_msg_hdr(&defkey_dat->header, HOST_CMD_CONFIG, sizeof(defkey_dat->header));

		defkey_dat->wid_hdr.wid  = WID_SET_DEFAULT_KEY;
		defkey_dat->wid_hdr.len  = sizeof(defkey_dat->gtk_ptk);

		memcpy(defkey_dat->gtk_ptk.mac_addr, mac_addr, MAC_LEN);
		defkey_dat->gtk_ptk.key_index  = index;
		defkey_dat->gtk_ptk.type       = ((unicast) ? 1 : 2 );

		defkey_dat->header.msg_len  += sizeof(defkey_dat->wid_hdr);
	  defkey_dat->header.msg_len  += sizeof(defkey_dat->gtk_ptk);
		skb_put(skb, sizeof(defkey_dat->wid_hdr) + sizeof(defkey_dat->gtk_ptk));

		HWIFI_INFO("EC212: set default key index = %u , type = %s \n", index, unicast ? "unicast" : "group");
		HWIFI_INFO("EC212: set default: %s len=%zu",unicast ? "ptk" : "gtk", sizeof(defkey_dat->gtk_ptk));

	  ret = hwifi_send_cmd(cfg, skb);

		PRINT_SEND_CMD_RET("EC212: SET DEFAULT KEY", ret);

		return ret;
}
/*
 * Prototype    : hwifi_proc_asyn_msg
 * Description  : process the msg that its type is MSG_TYPE_REPORT_STATUS
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/7/6
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
 int32 hwifi_recv_ec212_report(struct cfg_struct *cfg, uint8 *buf)
 {
	 	CFG_MSG_EC212_STRU *ec212_msg;
		int ret = 0;

		HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

		ec212_msg = (CFG_MSG_EC212_STRU *)buf;

		/* first process ec212 */
		if(0 == g_ec212_count)
		{
				g_ec212_count++;

				hwifi_cmd_set_defkey_force(cfg, ec212_msg->sta_addr, ec212_msg->ucKeyId, g_ec212_unicast);
		}
		else
		{
			 g_ec212_count = 0;

			 hwifi_exception_submit(cfg->hi110x_dev->hcc);

		}

		return ret;

 }

/*
 * Prototype    : hwifi_proc_asyn_msg
 * Description  : process the msg that its type is MSG_TYPE_REPORT_STATUS
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/7/6
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_proc_asyn_msg(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_HEADER_STRU     *msghdr;
    uint16                   dev_status;
    int32                    ret = -EFAIL;

    msghdr     = (CFG_MSG_HEADER_STRU *)buf;
    dev_status =  OS_LE16_TO_CPU(msghdr->usMsgStatus);

    HWIFI_DEBUG("dev_status = 0x%X, index=%u, len=%u", dev_status, msghdr->ucMsgIndex, msghdr->usMsgLength);

    switch(dev_status)
    {
        case DEVICE_STATUS_NET_INFO:
            ret = hwifi_recv_scan_inform(cfg, buf);
            break;
        case DEVICE_STATUS_SCAN_COMP:
            ret = hwifi_recv_scan_complete(cfg, buf);
            break;
        case DEVICE_STATUS_ASOC_COMP:
            ret = hwifi_recv_conn_res(cfg, buf);
            break;
        case DEVICE_STATUS_DISCONNECT:
            ret = hwifi_recv_disconn_res(cfg, buf);
            break;
        case DEVICE_STATUS_STATS:
            ret = hwifi_recv_stats_result(cfg, buf);
            break;
        case DEVICE_STATUS_CONNECT:
            /* AP mode, STA connect AP */
            ret = hwifi_recv_sta_join_info(cfg, buf);
            break;
        case DEVICE_STATUS_MODE:
            ret = hwifi_recv_chang_mode_res(cfg, buf);
            break;
        case DEVICE_STATUS_CONFIG:
            ret = hwifi_recv_config_result(cfg, buf);
            break;
        case DEVICE_STATUS_FRAME:
            ret = hwifi_recv_mgmt_frame(cfg, buf);
            break;
        case DEVICE_STATUS_INIT:
            /* init device protocol */
            ret = hwifi_recv_init_result(cfg, buf);
            break;
        /* 启动加载策略修改 */
        case DEVICE_STATUS_READY:
            /* device ready  */
            ret = hwifi_recv_status_ready(cfg, buf);
            break;
    #ifdef MAC_802_11W
        case DEVICE_STATUS_PMF:
            ret = hwifi_recv_pmf_report(cfg, buf);
        break;
    #endif

    #ifdef __ROAM__
        case DEVICE_STATUS_ROAM:
            /* init device protocol */
            ret = hwifi_recv_roam_report(cfg, buf);
            break;
    #endif

        case DEVICE_STATUS_SPEC_BA_SESSION:
            ret = hwifi_recv_ba_session(cfg, buf);
            break;
        case DEVICE_STATUS_ALL_BA_SESSION:
            ret = hwifi_recv_all_ba_session(cfg, buf);
            break;

        case DEVICE_STATUS_TKIP_MIC_FAILURE:
            ret = hwifi_recv_mic_error(cfg, buf);
            break;

        case DEVICE_EC212_REPORT:
            ret = hwifi_recv_ec212_report(cfg, buf);
            break;

        default:
            ret = -EFAIL;
            break;
    }
    return ret;
}
int32 wl_recv_cmd(struct cfg_struct *cfg, uint8 *buf, uint32 size)
{
    CFG_MSG_HEADER_STRU    *event_msghdr;
    uint8                   type;
    uint16                  msg_len;
    int32                   ret;

    event_msghdr = (CFG_MSG_HEADER_STRU *)buf;
    type         = event_msghdr->ucMsgType;
    msg_len      = OS_LE16_TO_CPU(event_msghdr->usMsgLength);
    if (msg_len > size)
    {
        HWIFI_WARNING("Invalid too short receive msg size, ul_size=%d, msg_len=%d", size, msg_len);
        return -EFAIL;
    }
    HWIFI_DEBUG("CMD type:%d",type);
    switch (type)
    {
        case MSG_TYPE_RESPONSE:
            /* ack response */
            ret = hwifi_proc_res_msg(cfg, buf);
            break;
        case MSG_TYPE_REPORT_STATUS:
            ret = hwifi_proc_asyn_msg(cfg, buf);
            break;
        default:
            ret = -EFAIL;
            HWIFI_WARNING("Invalid unknow msg type:%d",type);
            break;
    }

    return ret;
}

STATIC int32 wl_rx_cb(unsigned char main_type,unsigned char sub_type, struct sk_buff *skb, void *param)
{
    int32 ret;
    struct cfg_struct *cfg;
    HWIFI_ASSERT(NULL != param);
    HWIFI_ASSERT(NULL != skb);
    cfg = (struct cfg_struct *)param;
    HWIFI_ASSERT(NULL != cfg->hi110x_dev);
    HWIFI_ASSERT(NULL != cfg->hi110x_dev->hcc);
    HWIFI_ASSERT(NULL != cfg->hi110x_dev->hcc->io_dev);

    if(HCC_TYPE_BEATHEART == main_type)
    {
        if(cfg->hi110x_dev && cfg->hi110x_dev->hcc && cfg->hi110x_dev->hcc->io_dev)
        {
            bus_to_sdio(cfg->hi110x_dev->hcc->io_dev)->heartbeat.last_update_time
                                                        = cpu_clock(UINT_MAX);
        }
        if(!IS_STA(cfg))
        {
            /* 只有STA模式采用心跳上报sta统计信息 */
            HWIFI_DEBUG("HCC_TYPE_BEATHEART packet,but not sta mode,ignore the info.");
            dev_kfree_skb_any(skb);
            return SUCC;
        }
    }

    ret = wl_recv_cmd(cfg, skb->data, skb->len);

    HWIFI_DEBUG("Hwifi_recv_cmd return %d", ret);

    dev_kfree_skb_any(skb);

    return ret;
}
int32 cfg_event_rx_init(struct cfg_struct *cfg)
{
    hcc_rx_register(cfg->hi110x_dev->hcc,
                HCC_TYPE_BEATHEART,
                wl_rx_cb,
                cfg);
    return hcc_rx_register(cfg->hi110x_dev->hcc,
                HCC_TYPE_WIFI_CTRL,
                wl_rx_cb,
                cfg);
}



void cfg_event_rx_exit(struct cfg_struct *cfg)
{
    hcc_rx_unregister(cfg->hi110x_dev->hcc, HCC_TYPE_BEATHEART);
    hcc_rx_unregister(cfg->hi110x_dev->hcc, HCC_TYPE_WIFI_CTRL);
}


int32 hwifi_send_mgmt_frame(struct cfg_struct *cfg, uint32 channel, const uint8* buf, int32 len)
{
    struct sk_buff *skb;
    struct mgmt_tx_msg *msg;
    uint16              msg_size;
    int32               ret;
    uint8              *checksum;

    msg_size = sizeof(struct mgmt_tx_msg) + len + 1;  /* 1:usered to set checksum */
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct mgmt_tx_msg *)skb_put(skb, msg_size);

    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_MGMT_FRAME, msg_size);
    hwifi_fill_int_wid(&msg->channel, WID_SEND_CHANNEL, channel);
    hwifi_fill_binary_wid(&msg->frame_buf, WID_FRAME, len, buf);

    checksum  = (uint8 *)(msg + 1) + len;
    *checksum = bytes_check_sum(msg->frame_buf.val, msg->frame_buf.len);

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("send mgmt frame",ret);
    return ret;
}
int32 hwifi_send_p2p_flag(struct cfg_struct *cfg, int32 flag)
{
    struct sk_buff *skb;
    struct p2p_flag_msg *msg;
    uint16              msg_size;
    int32               ret;

    HWIFI_INFO("flag = %d,cfg->sta_info.gmode =%d",flag,cfg->sta_info.gmode);
    msg_size = sizeof(struct p2p_flag_msg);
    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct p2p_flag_msg *)skb_put(skb, msg_size);

    /* set the msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);
    if( flag == 0)/*p2p关闭*/
    {
        hwifi_fill_char_wid(&msg->p2p_gmode, WID_11G_OPERATING_MODE,cfg->sta_info.gmode);
    }
    else/*p2p打开*/
    {
        hwifi_fill_char_wid(&msg->p2p_gmode, WID_11G_OPERATING_MODE,HWIFI_MODE_G_ONLY);
    }

    hwifi_fill_char_wid(&msg->p2p_flag, WID_P2P_ENABLE, flag);

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("p2p_flag_set", ret);

    msleep(500);

    return ret;
}


/*
 * Prototype    : hwifi_p2p_discover_listen
 * Description  :
 * Input        :struct cfg_struct *cfg
                :int32 channel
                :uint32 duration_ms

 * Output       : None
 * Return Value : SUCC:success,-EFAIL:failed.
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/10/15
 *     Author       : liuyong lwx144625
 *     Modification : Created function
 *
 */

int32 hwifi_send_p2p_listen(struct cfg_struct *cfg, int32 channel)
{
    struct sk_buff *skb;
    struct p2p_listen_msg *msg;
    int32                 ret;
    uint16                msg_size;

    msg_size = sizeof(struct p2p_listen_msg);
    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct p2p_listen_msg *)skb_put(skb, msg_size);

    /* set the msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_LISTEN, msg_size);
    hwifi_fill_int_wid(&msg->channel_wid, WID_CHANNEL, channel);

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("send p2p listen",ret);
    return ret;
}

int32 hwifi_cancel_p2p_listen(struct cfg_struct *cfg)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msg;
    int32                    ret;
    uint16                   msg_size;

    msg_size = sizeof(struct hwifi_msg_header);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct hwifi_msg_header *)skb_put(skb, msg_size);

    /* set the msg header */
    hwifi_fill_msg_hdr(msg, HOST_CMD_CANCEL_LISTEN, msg_size);


    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("cancel p2p listen",ret);
    return ret;
}
int32 wl_set_p2p_ie(struct cfg_struct *cfg,
                         uint8             *beacon_ie,
                         uint16             beacon_ie_len,
                         uint8             *probe_rsp_ie,
                         uint16             probe_rsp_ie_len,
                         uint8             *asoc_rsp_ie,
                         uint16             asoc_rsp_ie_len)
{
    struct sk_buff *skb;
    struct hwifi_msg_header *msghdr;
    struct binary_wid       *beacon;
    struct binary_wid       *probe_rsp;
    struct binary_wid       *asoc_rsp;
    uint8                   *msg;
    uint8                   *pos;
    uint16                   msg_size;
    int32                    ret;
    uint8                   *checksum;
    int8                     update_all_ie;

    if ((0 == beacon_ie_len) && (0 == asoc_rsp_ie_len))
    {
        /* STA处于P2P 扫描状态，只会更新probe_rsp_ie信息。
         * 如果所有IE为空，则清空P2P 监听状态设置的WPS/P2P IE ，
         * 如果只有probe response不为空，则下发监听WPS/P2P IE,用于device回应probe response携带此IE
         */
         msg_size = sizeof(struct hwifi_msg_header)
             + (sizeof(struct binary_wid) + 1)
             + probe_rsp_ie_len;

         update_all_ie = FALSE;

         if (0 != probe_rsp_ie_len)
         {
            cfg->p2p_info.find_listen_state = TRUE;
         }
         else
         {
            cfg->p2p_info.find_listen_state = FALSE;
         }
    }
    else
    {
        /* GO更新所有的P2P/WPS IE
         * 虽然GO在初始化和去初始化时也会清空所有的IE信息,但由device侧在模式切换过程中负责主动清空
         */
        msg_size = sizeof(struct hwifi_msg_header)
             + (3 * (sizeof(struct binary_wid) + 1))
             + beacon_ie_len
             + probe_rsp_ie_len
             + asoc_rsp_ie_len;

        update_all_ie = TRUE;
    }


    HWIFI_INFO("beacon_ie_len is %d,probe_rsp_ie_len=%d,asoc_rsp_ie_len=%d,update_all_ie=%d",
                beacon_ie_len, probe_rsp_ie_len, asoc_rsp_ie_len, update_all_ie);


    if (msg_size > MAX_MSG_LEN)
    {
        HWIFI_WARNING("the length of the set p2p ie msg is too long,msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    /* add msg header */
    msghdr = (struct hwifi_msg_header *)msg;
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);
    pos    = (uint8 *)(msghdr + 1);


    if (TRUE != update_all_ie)
    {
        /* add probe response ie */
        probe_rsp = (struct binary_wid *)pos;
        HWIFI_INFO("probe_rsp_ie_len is %d", probe_rsp_ie_len);
        hwifi_fill_binary_wid(probe_rsp, WID_IE_PROBERSP, probe_rsp_ie_len, probe_rsp_ie);
        checksum = (uint8 *)probe_rsp->val + probe_rsp->len;
        *checksum = bytes_check_sum(probe_rsp->val, probe_rsp->len);
    }
    else
    {
        /* add beacon ie */
        beacon = (struct binary_wid *)pos;
        HWIFI_INFO("beacon ie len is %d", beacon_ie_len);
        hwifi_fill_binary_wid(beacon, WID_IE_BEACON, beacon_ie_len, beacon_ie);
        checksum = (uint8 *)beacon->val + beacon->len;
        *checksum = bytes_check_sum(beacon->val, beacon->len);
        pos = checksum + 1;

        /* add probe response ie */
        probe_rsp = (struct binary_wid *)pos;
        HWIFI_INFO("probe_rsp_ie_len is %d", probe_rsp_ie_len);
        hwifi_fill_binary_wid(probe_rsp, WID_IE_PROBERSP_GO, probe_rsp_ie_len, probe_rsp_ie);
        checksum = (uint8 *)probe_rsp->val + probe_rsp->len;
        *checksum = bytes_check_sum(probe_rsp->val, probe_rsp->len);
        pos = checksum + 1;

        /* add assoc response ie */
        asoc_rsp = (struct binary_wid *)pos;
        HWIFI_INFO("asoc_rsp_ie_len is %d", asoc_rsp_ie_len);
        hwifi_fill_binary_wid(asoc_rsp, WID_IE_ASOCRSP, asoc_rsp_ie_len, asoc_rsp_ie);
        checksum = (uint8 *)asoc_rsp->val + asoc_rsp->len;
        *checksum = bytes_check_sum(asoc_rsp->val, asoc_rsp->len);
    }


    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("set_p2p_ie", ret);

    return ret;
}


int32 hwifi_start_set_wiphy(struct cfg_struct *cfg, struct hwifi_wiphy_param *param)
{
    struct sk_buff *skb;
    struct hwifi_msg_header *msghdr;
    uint16                   msg_size;
    int32 ret;

    msg_size = sizeof(struct hwifi_msg_header);

    if (TRUE == param->frag_threshold_changed)
    {
        msg_size += sizeof(struct short_wid);
    }

    if (TRUE == param->rts_threshold_changed)
    {
        msg_size += sizeof(struct short_wid);
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    if (TRUE == param->frag_threshold_changed)
    {
        struct short_wid *frag_threshold;

        frag_threshold = (struct short_wid *)skb_put(skb, sizeof(struct short_wid));
        hwifi_fill_short_wid(frag_threshold, WID_FRAG_THRESHOLD, param->frag_threshold);
    }

    if (TRUE == param->rts_threshold_changed)
    {
        struct short_wid *rts_threshold;

        rts_threshold = (struct short_wid *)skb_put(skb, sizeof(struct short_wid));
        hwifi_fill_short_wid(rts_threshold, WID_RTS_THRESHOLD, param->rts_threshold);
    }

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("SET WIPHY", ret);

    return ret;
}


int32 hwifi_set_static_wep_key(struct cfg_struct *cfg, struct hwifi_wep_key_param* param)
{
    struct sk_buff *skb;
    struct hwifi_msg_header *msghdr;
    uint16                   msg_size;
    int32 ret;
    int i;

    msg_size = sizeof(struct hwifi_msg_header);
    for (i = 0; i < MAX_WEP_KEY_COUNT; i++)
    {
        if (param->keys[i].len > 0)
        {
            msg_size += (sizeof(struct wep_key) + param->keys[i].len);
        }
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    for (i = 0; i < MAX_WEP_KEY_COUNT; i++)
    {
        if (param->keys[i].len > 0)
        {
            struct wep_key *wep_key;
            uint8 set_flag;

            set_flag = (param->keys[i].index == param->default_index)? TRUE: FALSE;
            wep_key = (struct wep_key *)skb_put(skb, sizeof(struct wep_key) + param->keys[i].len);
            hwifi_fill_wepkey(wep_key, param->keys[i].len,
                    param->keys[i].index, param->keys[i].key, set_flag);
        }
    }

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("SET WEP KEY", ret);

    return ret;
}
#if 0

 int32  hwifi_cfg80211_set_power(struct cfg_struct *cfg, uint32  ps_mode)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *ps;
    uint16                   msg_size;
    int32                    ret;

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct char_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    /* fill ps mode */
    ps = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(ps, WID_POWER_MANAGEMENT, ps_mode);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("fail to send the set_power msg");
        return -EFAIL;
    }

    HWIFI_INFO("succeed to send the power save msg");

    return SUCC;
}
#endif

int32 wl_send_channel(struct cfg_struct  *cfg,
                             uint16       channel,
                             uint8        enable_20M_40M,
                             uint8        coexist_20M_40M,
                             uint8        ht_enabled)
{
    struct channel_msg     *channel_info;
    struct                  sk_buff *skb;
    uint8                  *msg;
    uint16                  msg_size;
    int32                   ret;

    msg_size = sizeof(struct channel_msg);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    /* add msg header */
    channel_info = (struct channel_msg *)msg;
    hwifi_fill_msg_hdr(&channel_info->msg_hdr, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_short_wid(&channel_info->channel_info, WID_USER_PREF_CHANNEL, channel);
    hwifi_fill_char_wid(&channel_info->enable_20M_40M, WID_2040_ENABLE, enable_20M_40M);
    hwifi_fill_char_wid(&channel_info->coexist_20M_40M, WID_2040_COEXISTENCE, coexist_20M_40M);
    hwifi_fill_char_wid(&channel_info->ht_enabled, WID_11N_ENABLE, ht_enabled);

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("send channel",ret);

    return ret;

}


int32 wl_get_channel_info(struct cfg_struct  *cfg)
{
    struct sk_buff                *skb;
    struct hwifi_query_msg_header *msghdr;
    uint8                         *msg;
    uint16                         msg_size;
    int32                          ret;

    /* 查询当前信道，1个查询WID号 */
    msg_size = sizeof(struct hwifi_query_msg_header) + sizeof(uint16);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    msghdr = (struct hwifi_query_msg_header *)msg;

    hwifi_fill_query_msg_hdr(msghdr,msg_size);
    msghdr->wid_list[0] = WID_CURR_OPER_CHANNEL;


    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("get channel",ret);
    return ret;


}

/*
 * Prototype    : hwifi_cmd_set_noa
 * Description  : start or stop p2p noa
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/4
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_cmd_set_noa(struct cfg_struct *cfg, struct hwifi_p2p_noa_param *noa_param)
{
    struct noa_msg  *set_noa;
    struct           sk_buff *skb;
    uint8           *msg;
    uint16           msg_size;
    int32            ret;

    msg_size = sizeof(struct noa_msg);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long set noa msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    set_noa = (struct noa_msg *)msg;
    hwifi_fill_msg_hdr(&set_noa->msg_hdr, HOST_CMD_CONFIG, msg_size);
    set_noa->noa.wid        = WID_P2P_NOA_SCHEDULE;
    set_noa->noa.len        = sizeof(struct noa_wid) - WIDHDR_LEN;

    if (0 == noa_param->count)
    {
        set_noa->noa.noa_num = 0;
    }
    else
    {
        set_noa->noa.noa_num = 1;
    }

    set_noa->noa.noa_len    = set_noa->noa.noa_num * 13 + 1;
    set_noa->noa.noa1_cnt   = noa_param->count;
    set_noa->noa.noa1_dur   = OS_CPU_TO_LE32(noa_param->duration);
    set_noa->noa.noa1_int   = OS_CPU_TO_LE32(noa_param->interval);
    set_noa->noa.noa1_start = OS_CPU_TO_LE32(noa_param->starttime);

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set noa",ret);
    return ret;
}

/*
 * Prototype    : hwifi_cmd_set_p2p_ps
 * Description  : set p2p power save
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/4
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_cmd_set_p2p_ps(struct cfg_struct *cfg, uint8 ctwindow)
{
    struct p2p_ps_msg   *set_p2p_ps;
    struct               sk_buff *skb;
    uint8               *msg;
    uint16               msg_size;
    int32                ret;

    msg_size = sizeof(struct ps_msg);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    set_p2p_ps = (struct p2p_ps_msg *)msg;
    hwifi_fill_msg_hdr(&set_p2p_ps->msg_hdr, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_char_wid(&set_p2p_ps->ctwindow, WID_P2P_CT_WINDOW, ctwindow);

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set p2p ps",ret);
    return ret;
}

#if 0
/*
 * Prototype    : hwifi_cmd_set_ps
 * Description  : set legacy ps
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/4
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_cmd_set_ps(struct cfg_struct *cfg, uint8 ps)
{
    struct ps_msg   *set_ps;
    struct           sk_buff *skb;
    uint8           *msg;
    uint16           msg_size;
    int32            ret;

    msg_size = sizeof(struct ps_msg);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("the length of the set p2p ps msg is too long,msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    set_ps = (struct ps_msg *)msg;
    hwifi_fill_msg_hdr(&set_ps->msg_hdr, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_char_wid(&set_ps->powersave, WID_POWER_MANAGEMENT, ps);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("fail to send the set_ps msg");
        return ret;
    }

    HWIFI_INFO("succeed to set set_ps msg.");

    return ret;
}
#endif

#ifdef __ROAM__
/*
 * Prototype    : hwifi_recv_roam_report
 * Description  : handle the msg reported by device
 * Input        : cfg
                : buf : buf of msg
                : channel
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_proc_asyn_msg
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_recv_roam_report(struct cfg_struct *cfg, uint8 *buf)
{
    struct roam_roaming_result_msg     *msg;
    int32                        ret;
    uint16                       roam_substatus;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    ret = -EFAIL;
    msg            = (struct roam_roaming_result_msg *)buf;
    roam_substatus = (uint8)(OS_LE16_TO_CPU(msg->msg_header.enMsgSubStatus));

    HWIFI_INFO("Roam subtatus:%d",roam_substatus);
    switch (roam_substatus)
    {
        case ROAM_SUB_STATUS_CFG_RESULT:
            /* 暂时不做处理 */
            break;

        case ROAM_SUB_STATUS_TRIGGER:
            HWIFI_DEBUG("Recv roam trigger.\n");
            ret = hwifi_recv_roam_report_trigger(cfg, buf);
            break;

        case ROAM_SUB_STATUS_TRIGGER_FORCE:
            HWIFI_INFO("Recv ROAM_SUB_STATUS_TRIGGER_FORCE.\n");
            cfg->roam.roam_fsm.force_trigger_scan_flag = 1;
            ret = hwifi_recv_roam_report_trigger(cfg, buf);
            break;

        case ROAM_SUB_STATUS_SCAN_RESULT:
            HWIFI_DEBUG("Recv scan result.\n");
            ret = hwifi_recv_roam_scan_report(cfg, buf);
            break;

       case ROAM_SUB_STATUS_NEW_AP_ASSOC:
           HWIFI_INFO("Recv new ap assoc succ.\n");
           ret = hwifi_recv_roam_new_ap_assoc_cmpl(cfg, buf);
           break;

       case ROAM_SUB_STATUS_RETURN_TO_OLD:
           HWIFI_INFO("Recv return to old ap.\n");
           ret = hwifi_recv_roam_return_to_old(cfg, buf);
           break;

       default:
           break;
    }

    return ret;
}



/*
 * Prototype    : hwifi_start_roam_set_param
 * Description  : send the msg which set the parameters of roam to device
 * Input        : roam : all info of roam
                : cfg_param : parameter of roam alg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_update_scan_channels
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_start_roam_set_param(struct cfg_struct *cfg, struct roam_cfg_stru *cfg_param)
{
    struct sk_buff      *skb;
    struct roam_cfg_msg *msg;       /* pointer to cfg cmd message struct */
    uint16               msg_size;  /* cfg message size */
    int32                ret;
#ifdef __CHECKSUM__
    uint8               *checksum;
#endif

    HWIFI_ASSERT((NULL != cfg) && (NULL != cfg_param));

    msg_size = sizeof(struct roam_cfg_msg);
#ifdef __CHECKSUM__
    msg_size += sizeof(*checksum);
#endif

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct roam_cfg_msg *)skb_put(skb, sizeof(struct roam_cfg_msg));

    /* TBD HOST_CMD_ROAM需要定义 */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_ROAM, msg_size);

    /* TBD fill cfg param */
    hwifi_fill_binary_wid(&msg->roam_wid, WID_ROAM, sizeof(struct roam_cfg_stru), (uint8 *)cfg_param);
#ifdef __CHECKSUM__
    checksum= (uint8 *)skb_tail_pointer(skb);
    *checksum = bytes_check_sum(msg->roam_wid.val, msg->roam_wid.len);
    skb_put(skb, sizeof(*checksum));
#endif
    /* send cfg cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("roam cfg", ret);

    return ret;
}

/*
 * Prototype    : hwifi_start_roam_scan
 * Description  : send the msg which start-up scan process to device
 * Input        : roam : all info of roam
                : scan_param: parameter of roam scan
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_go_foreign_channel
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_start_roam_scan(struct cfg_struct *cfg, struct roam_scan_cfg_ch_stru *scan_param)
{
    struct sk_buff              *skb;
    struct roam_scan_cfg_ch_msg *msg;       /* pointer to scan cmd message struct */
    uint16                       msg_size;  /* scan cmd message size */
    int32                        ret;
#ifdef __CHECKSUM__
    uint8               *checksum;
#endif

    struct string_wid *ssid;

    HWIFI_ASSERT((NULL != cfg) && (NULL != scan_param));

    msg_size = sizeof(struct roam_scan_cfg_ch_msg)
                + sizeof(struct string_wid) + cfg->conn.bss.ssid_len;


#ifdef __CHECKSUM__
        msg_size += sizeof(*checksum);
#endif

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct roam_scan_cfg_ch_msg *)skb_put(skb, sizeof(struct roam_scan_cfg_ch_msg));

    /* TBD HOST_CMD_ROAM需要定义 */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_ROAM, msg_size);

    /* TBD fill roam param */
    hwifi_fill_binary_wid(&msg->roam_wid, WID_ROAM, sizeof(struct roam_scan_cfg_ch_stru), (uint8 *)scan_param);
#ifdef __CHECKSUM__
    checksum= (uint8 *)skb_tail_pointer(skb);
    *checksum = bytes_check_sum(msg->roam_wid.val, msg->roam_wid.len);
    skb_put(skb, sizeof(*checksum));
#endif

    ssid = (struct string_wid *)skb_tail_pointer(skb);
    hwifi_fill_string_wid(ssid, WID_SSID, cfg->conn.bss.ssid_len, cfg->conn.bss.ssid);
    skb_put(skb, sizeof(struct string_wid) + ssid->len);

    /* send roam scan cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("roam scan", ret);

    return ret;
}

/*
 * Prototype    : hwifi_start_roam_send_probe_req
 * Description  : send the msg which send a probe request
 * Input        : roam : all info of roam
                : scan_param: parameter of send probe req
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_send_probe_req
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_start_roam_send_probe_req(struct cfg_struct *cfg, struct roam_scan_probe_req_stru *scan_param)
{
    struct sk_buff  *skb;
    struct roam_scan_probe_req_msg *msg;                       /* pointer to scan cmd message struct */
    uint16           msg_size;                  /* scan cmd message size */
    int32            ret;
#ifdef __CHECKSUM__
    uint8           *checksum;
#endif

    HWIFI_ASSERT((NULL != cfg) && (NULL != scan_param));

    msg_size = sizeof(struct roam_scan_probe_req_msg);
#ifdef __CHECKSUM__
    msg_size += sizeof(*checksum);
#endif

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct roam_scan_probe_req_msg *)skb_put(skb, sizeof(struct roam_scan_probe_req_msg));

    /* TBD HOST_CMD_ROAM需要定义 */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_ROAM, msg_size);

    /* TBD fill send probe req param */
    hwifi_fill_binary_wid(&msg->roam_wid, WID_ROAM, sizeof(struct roam_scan_probe_req_stru), (uint8 *)scan_param);
#ifdef __CHECKSUM__
    checksum= (uint8 *)skb_tail_pointer(skb);
    *checksum = bytes_check_sum(msg->roam_wid.val, msg->roam_wid.len);
    skb_put(skb, sizeof(*checksum));
#endif

    /* send roam send probe req cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("roam send probe req", ret);

    return ret;
}

STATIC int32 hwifi_set_roam_ie(struct cfg_struct *cfg)
{
    uint8           *skb_data;
    struct sk_buff  *skb;
    uint16           msg_size;
    int32            ret;
#ifdef __CHECKSUM__
    uint8           *checksum;
#endif

    HWIFI_ASSERT(NULL != cfg);

    msg_size = sizeof(struct hwifi_msg_header) +  sizeof(struct binary_wid) + cfg->conn.bss.assoc_ie_len;

#ifdef __CHECKSUM__
    msg_size += sizeof(*checksum);
#endif

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    skb_data = (uint8 *)skb_put(skb, (sizeof(struct hwifi_msg_header) + sizeof(struct binary_wid) + cfg->conn.bss.assoc_ie_len));

    hwifi_fill_msg_hdr((struct hwifi_msg_header *)skb_data, HOST_CMD_CONFIG, msg_size);

    skb_data = skb_data + sizeof(struct hwifi_msg_header);
    hwifi_fill_binary_wid((struct binary_wid *)(skb_data), WID_IE, cfg->conn.bss.assoc_ie_len, cfg->conn.bss.assoc_ie);

#ifdef __CHECKSUM__
    checksum= (uint8 *)skb_tail_pointer(skb);
    *checksum = bytes_check_sum(((struct binary_wid *)skb_data)->val, ((struct binary_wid *)skb_data)->len);
    skb_put(skb, sizeof(*checksum));
#endif

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("set roam ie to device", ret);
    return ret;

}


/*
 * Prototype    : hwifi_start_roaming
 * Description  : build roam cmd with wid format and send msg
 * Input        : struct cfg_struct *cfg
                  struct roam_roaming_stru  *cmd
 * Output       : None
 * Return Value : int32
 * Calls        : hwifi_send_cmd
 * Called By    : hwifi_start_roam
 *
 *   History        :
 *   1.Date         : 2013/1/10
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_start_roaming(struct cfg_struct *cfg, struct roam_roaming_stru *param, uint8 *frame)
{
    struct sk_buff  *skb;
    struct roam_roaming_msg *msg;                       /* pointer to scan cmd message struct */
    uint16           msg_size;                  /* scan cmd message size */
    int32            ret;
#ifdef __CHECKSUM__
    uint8           *checksum;
#endif

    HWIFI_ASSERT((NULL != cfg) && (NULL != param) && (NULL != frame));

    ret = hwifi_set_roam_ie(cfg);
    if(SUCC != ret)
    {
        HWIFI_WARNING("hwifi_set_roam_ie failed,start roaming failed.");
        return -EFAIL;
    }


    /* 需要beacon长度 */
    msg_size = sizeof(struct roam_roaming_msg) + param->length;
#ifdef __CHECKSUM__
    msg_size += sizeof(*checksum);
#endif

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct roam_roaming_msg *)skb_put(skb, sizeof(struct roam_roaming_msg) + param->length);

    /* TBD HOST_CMD_ROAM需要定义 */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_ROAM, msg_size);

    /* TBD fill roam param */
    //hwifi_fill_binary_wid(&msg->roam_wid, WID_ROAM, sizeof(struct roam_roaming_stru) + param->length, (uint8 *)param);
    msg->roam_wid.wid = OS_CPU_TO_LE16(WID_ROAM);
    msg->roam_wid.len = OS_CPU_TO_LE16(sizeof(struct roam_roaming_stru) + param->length);

    OS_MEM_CPY(&msg->roaming_param, param, sizeof(struct roam_roaming_stru));
    OS_MEM_CPY(msg->roaming_param.frame, frame, param->length);
#ifdef __CHECKSUM__
    checksum= (uint8 *)skb_tail_pointer(skb);
    *checksum = bytes_check_sum(msg->roam_wid.val, msg->roam_wid.len);
    skb_put(skb, sizeof(*checksum));
#endif

    /* send roam roaming cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("roam roaming start", ret);

    return ret;
}


/*
 * Prototype    : hwifi_start_roaming_timeout
 * Description  : build roam cmd with wid format and send msg
 * Input        : struct cfg_struct *cfg
                  struct roam_roaming_stru  *cmd
 * Output       : None
 * Return Value : int32
 * Calls        : hwifi_send_cmd
 * Called By    : hwifi_start_roam
 *
 *   History        :
 *   1.Date         : 2013/1/10
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_start_roaming_timeout(struct cfg_struct *cfg, struct roam_roaming_timeout_stru *param)
{
    struct sk_buff  *skb;
    struct roam_roaming_timeout_msg *msg;                       /* pointer to scan cmd message struct */
    uint16           msg_size;                  /* scan cmd message size */
    int32            ret;
#ifdef __CHECKSUM__
    uint8           *checksum;
#endif

    HWIFI_ASSERT((NULL != cfg) && (NULL != param));

    msg_size = sizeof(struct roam_roaming_timeout_msg);
#ifdef __CHECKSUM__
    msg_size += sizeof(*checksum);
#endif

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct roam_roaming_timeout_msg *)skb_put(skb, sizeof(struct roam_roaming_timeout_msg));

    /* TBD HOST_CMD_ROAM需要定义 */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_ROAM, msg_size);

    /* TBD fill roam timeout param */
    hwifi_fill_binary_wid(&msg->roam_wid, WID_ROAM, sizeof(struct roam_roaming_timeout_stru), (uint8 *)param);
#ifdef __CHECKSUM__
    checksum= (uint8 *)skb_tail_pointer(skb);
    *checksum = bytes_check_sum(msg->roam_wid.val, msg->roam_wid.len);
    skb_put(skb, sizeof(*checksum));
#endif

    /* send roam roaming timeout cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("roam roaming timeout", ret);

    return ret;
}

/*
 * Prototype    : hwifi_start_stay_home
 * Description  : send the msg which command device to stay home channel
 * Input        : roam : all info of roam
                : home_param: parameter of stay home channel
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : roam_go_home_channel
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_start_stay_home(struct cfg_struct *cfg, struct roam_stay_home_stru *home_param)
{
    struct sk_buff  *skb;
    struct roam_stay_home_channel_msg *msg;                       /* pointer to scan cmd message struct */
    uint16           msg_size;                  /* scan cmd message size */
    int32            ret;
#ifdef __CHECKSUM__
    uint8           *checksum;
#endif

    HWIFI_ASSERT(NULL != cfg);

    msg_size = sizeof(struct roam_stay_home_channel_msg);
#ifdef __CHECKSUM__
    msg_size += sizeof(*checksum);
#endif

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct roam_stay_home_channel_msg *)skb_put(skb, sizeof(struct roam_stay_home_channel_msg));

    /* TBD HOST_CMD_ROAM需要定义 */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_ROAM, msg_size);

    /* TBD fill roam param */
    hwifi_fill_binary_wid(&msg->roam_wid, WID_ROAM, sizeof(struct roam_stay_home_stru), (uint8 *)home_param);
#ifdef __CHECKSUM__
    checksum= (uint8 *)skb_tail_pointer(skb);
    *checksum = bytes_check_sum(msg->roam_wid.val, msg->roam_wid.len);
    skb_put(skb, sizeof(*checksum));
#endif

    /* send roam scan cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("roam stay home", ret);

    return ret;
}


/*
 * Prototype    : hwifi_recv_roam_scan_report
 * Description  : handle the msg reported by device
 * Input        : cfg
                : buf : buf of msg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_recv_roam_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_recv_roam_scan_report(struct cfg_struct *cfg, uint8 *buf)
{
    struct roam_scan_report_msg *msg;
    uint8                   *beacon_ie;
    uint32                   beacon_ie_len;
    uint8                   *ssid_ie;
    struct ieee80211_mgmt   *new_ap_beacon;
    struct roam_all_stru *roam_info;
    struct roam_candidate_ap_stru *candidate_ap;
    struct roam_scan_candidate_ap_node *new_ap;
    struct scanned_bss_event bss_event;
    enum ieee80211_band band;
    int32 ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    /* get scan result form WID msg */
    msg = (struct roam_scan_report_msg *)buf;

    roam_info = &cfg->roam;
    HWIFI_ASSERT(NULL != roam_info);

    HWIFI_DEBUG("Msg content: band = %d, channel = %d, rssi = %d\n", msg->scan_report.band, msg->scan_report.channel, msg->scan_report.rssi);

    /* 如果信道非法，直接返回，不处理 */
    if (msg->scan_report.band >= ROAM_PARAM_BAND_BUTT)
    {
        HWIFI_ERROR("Band is invalid. band = %d\n", msg->scan_report.band);
        return -EFAIL;
    }

    new_ap_beacon = (struct ieee80211_mgmt *)msg->scan_report.frame;
    HWIFI_DEBUG("BSSID:"MACFMT, MAC2STR(new_ap_beacon->bssid));

    /* get channel utilizationg in BSS LOAD IE */
    beacon_ie = new_ap_beacon->u.beacon.variable;
    beacon_ie_len = msg->scan_report.length - offsetof(struct ieee80211_mgmt, u.beacon.variable);

    /* filter the ap if ssid diamatch */
    ssid_ie = (uint8 *)cfg80211_find_ie(WLAN_EID_SSID, beacon_ie, beacon_ie_len);

    if (NULL == ssid_ie)
    {
        HWIFI_DEBUG("hwifi_recv_roam_scan_report: ssid is null\n");
        return SUCC;
    }

    if ((NULL != ssid_ie) && (hwifi_roam_ssid_is_dismatch(cfg, (*(ssid_ie + 1)), (ssid_ie + 2))))
    {
        HWIFI_DEBUG("hwifi_recv_roam_scan_report: ssid is dismatch.\n");
        return SUCC;
    }

    /* 加密RSN/WPA IE匹配判断,如果加密RSN/WPA IE不匹配则返回 */
    if(hwifi_roam_ie_is_mismatch(cfg, msg->scan_report.frame, msg->scan_report.length))
    {
        return SUCC;
    }

    HWIFI_INFO("Ssid match, band=%d, channel = %d, rssi = %d",
            msg->scan_report.band, msg->scan_report.channel, msg->scan_report.rssi - 255);

    /* update scan list */
    band = (ROAM_PARAM_BAND_2G == msg->scan_report.band)?
                   IEEE80211_BAND_2GHZ : IEEE80211_BAND_5GHZ;

        memset(&bss_event, 0, sizeof(bss_event));

        bss_event.freq = ieee80211_channel_to_frequency(msg->scan_report.channel, band);
        bss_event.rssi = msg->scan_report.rssi;
        bss_event.mgmt = msg->scan_report.frame;
        bss_event.mgmt_len = msg->scan_report.length;

    report_scanned_bss(cfg, &bss_event);

    candidate_ap = hwifi_roam_find_candidate_ap_by_bssid(&roam_info->ap_list, new_ap_beacon->bssid);
    if (NULL == candidate_ap)
    {
        HWIFI_INFO("new ap ["MACFMT"]", MAC2STR(new_ap_beacon->bssid));

        new_ap = hwifi_roam_candidate_ap_alloc();
        if (NULL == new_ap)
        {
            HWIFI_ERROR("hwifi_recv_roam_scan_report: hwifi_roam_candidate_ap_alloc fail.\n");
            return -EFAIL;
        }

        memset(new_ap, 0, sizeof(struct roam_scan_candidate_ap_node));

        spin_lock_init(&new_ap->ap_node_lock);

        ret = hwifi_roam_candidate_ap_add(roam_info, buf, new_ap);
        if (SUCC != ret)
        {
            HWIFI_ERROR("hwifi_recv_roam_scan_report: hwifi_roam_candidate_ap_add fail. ret = %d \n", ret);

            hwifi_roam_candidate_ap_free(new_ap);
            return ret;
        }

    }
    else
    {
        HWIFI_INFO("Exsited ap ["MACFMT"]", MAC2STR(new_ap_beacon->bssid));

        ret = hwifi_roam_candidate_ap_update(roam_info, buf, candidate_ap);

        hwifi_roam_candidate_ap_put(candidate_ap);

        return ret;
    }

    return SUCC;
}

/*
 * Prototype    : hwifi_recv_roam_report_trigger
 * Description  : handle the msg reported by device
 * Input        : cfg
                : buf : buf of msg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_recv_roam_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_recv_roam_report_trigger(struct cfg_struct *cfg, uint8 *buf)
{
    struct roam_all_stru* roam;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    roam = &cfg->roam;
    HWIFI_ASSERT(NULL != roam);

    roam_process_trigger(roam);

    return SUCC;
}

/*
 * Prototype    : hwifi_recv_roam_return_to_old
 * Description  : handle the msg reported by device
 * Input        : cfg
                : buf : buf of msg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_recv_roam_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_recv_roam_return_to_old(struct cfg_struct *cfg, uint8 *buf)
{
    struct roam_all_stru* roam;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    roam = &cfg->roam;
    HWIFI_ASSERT(NULL != roam);

    roam_process_return(roam);

    return SUCC;
}

/*
 * Prototype    : hwifi_recv_roam_new_ap_assoc_cmpl
 * Description  : handle the msg reported by device
 * Input        : cfg
                : buf : buf of msg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_recv_roam_report
 *
 *   History        :
 *   1.Date         : 2013/1/20
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_recv_roam_new_ap_assoc_cmpl(struct cfg_struct *cfg, uint8 *buf)
{
    struct roam_roaming_result_msg *msg;
    struct hwifi_roamed_event roamed_event;

    struct ieee80211_mgmt *mgmt_req = NULL;
    struct ieee80211_mgmt *mgmt_resp = NULL;
    uint8           *req_ie = NULL;
    uint8            req_ie_len = 0;
    uint8           *resp_ie = NULL;
    uint8            resp_ie_len = 0;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    msg = (struct roam_roaming_result_msg *)buf;

    HWIFI_DEBUG("Msg content: band = %d, channel = %d\n", msg->new_ap_assoc_comp.band, msg->new_ap_assoc_comp.channel);

    mgmt_req = (struct ieee80211_mgmt *)msg->new_ap_assoc_comp.frame;
    mgmt_resp = (struct ieee80211_mgmt *)(msg->new_ap_assoc_comp.frame + msg->new_ap_assoc_comp.assoc_req_length);

    /* get assoc request ie */
    if (ieee80211_is_assoc_req(mgmt_req->frame_control))
    {
        req_ie = mgmt_req->u.assoc_req.variable;
        req_ie_len = msg->new_ap_assoc_comp.assoc_req_length - offsetof(struct ieee80211_mgmt, u.assoc_req.variable);
    }
    else if (ieee80211_is_reassoc_req(mgmt_req->frame_control))
    {
        req_ie = mgmt_req->u.reassoc_req.variable;
        req_ie_len = msg->new_ap_assoc_comp.assoc_req_length - offsetof(struct ieee80211_mgmt, u.reassoc_req.variable);
    }

     /* get assoc response ie */
    if (ieee80211_is_assoc_resp(mgmt_resp->frame_control))
    {
        resp_ie = mgmt_resp->u.assoc_resp.variable;
        resp_ie_len = msg->new_ap_assoc_comp.assoc_rsp_length - offsetof(struct ieee80211_mgmt, u.assoc_resp.variable);
    }
    else if (ieee80211_is_reassoc_resp(mgmt_resp->frame_control))
    {
        resp_ie = mgmt_resp->u.reassoc_resp.variable;
        resp_ie_len = msg->new_ap_assoc_comp.assoc_rsp_length - offsetof(struct ieee80211_mgmt, u.reassoc_resp.variable);
    }

    memset(&roamed_event, 0, sizeof(roamed_event));
    roamed_event.bssid = mgmt_req->bssid;
    roamed_event.band = msg->new_ap_assoc_comp.band;
    roamed_event.channel = msg->new_ap_assoc_comp.channel;
    roamed_event.req_ie_len = req_ie_len;
    roamed_event.req_ie = req_ie;
    roamed_event.resp_ie_len = resp_ie_len;
    roamed_event.resp_ie = resp_ie;

    report_roamed_event(cfg, &roamed_event);

    return SUCC;
}



int32 hwifi_set_dev_roam_report_enabled(struct cfg_struct *cfg, uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *scan_trigger_wid;
    uint16                   msg_size;
    int32                    ret;

    if(NULL == cfg)
    {
        HWIFI_WARNING("argument cfg is null.\n");
        return -EFAIL;
    }

    if(enabled != ROAM_DEVICE_SCAN_TRIGGER_REPORT_DISABLE &&
        enabled != ROAM_DEVICE_SCAN_TRIGGER_REPORT_ENABLE)
    {
        HWIFI_WARNING("argument enabled(%d) is not right.\n", enabled);
        return -EFAIL;
    }

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct char_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
       return -EFAIL;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    /* fill ps mode */
    scan_trigger_wid = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(scan_trigger_wid, WID_CHAR_RESERVE02, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send device_roam_report_enabled flag.");
       return -EFAIL;
    }

    HWIFI_DEBUG("Success to send device_roam_report_enabled flag(%d).", enabled);
    return SUCC;
}


void hwifi_roam_calc_tx_failed_scale(struct cfg_struct *cfg, uint32 tx_success_pkt, uint32 tx_failed_pkt, int32 curr_rate)
{
    uint32 tx_failed_scale;
    uint32 tx_failed_sum;
    uint32 tx_success_sum;
    struct roam_all_stru *pst_roam_info;

    if(NULL == cfg)
    {
        HWIFI_WARNING("pst_roam_info is null!");
        return;
    }
    pst_roam_info = &cfg->roam;

    if(!cfg->hi110x_dev->pm_info->connect_finished)
    {
        /* 未关联情况下不记录报文发送统计值 */
        pst_roam_info->alg_param.tx_pkt_param.record_count = 0;
        pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 0;
        return;
    }

    if(0 == pst_roam_info->alg_param.tx_pkt_param.record_count)
    {
        memset(pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt, 0 ,sizeof(pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt));
        memset(pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt, 0 ,sizeof(pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt));

        pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt[0] = tx_success_pkt;
        pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt[0] = tx_failed_pkt;
        pst_roam_info->alg_param.tx_pkt_param.record_count++;
    }
    else if((pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt[0] > tx_success_pkt)
            ||(pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt[0] > tx_failed_pkt)
            ||(pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt[1] > tx_success_pkt)
            ||(pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt[1] > tx_failed_pkt))
    {
        /* 如果device上报最新发送报文统计低于上1秒的记录值，则说明重新关联过新的AP,这里重新开始记录 */
        HWIFI_INFO("reset tx packet record,record_count=%d,tx_success_pkt[0]=%u,tx_failed_pkt[0]=%u,tx_success_pkt[1]=%u,tx_failed_pkt[1]=%u,tx_succss_pkt=%u,tx_failed_pkt=%u",
                    pst_roam_info->alg_param.tx_pkt_param.record_count,
                    pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt[0],
                    pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt[0],
                    pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt[1],
                    pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt[1],
                    tx_success_pkt, tx_failed_pkt);
        pst_roam_info->alg_param.tx_pkt_param.record_count = 0;
        pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 0;
    }
    else
    {
        pst_roam_info->alg_param.tx_pkt_param.record_count++;
        pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt[1] = tx_success_pkt;
        pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt[1] = tx_failed_pkt;

        if(pst_roam_info->alg_param.tx_pkt_param.record_count >= pst_roam_info->alg_param.tx_pkt_param.time_interval)
        {
            pst_roam_info->alg_param.tx_pkt_param.record_count= 0;

            tx_failed_sum = tx_failed_pkt - pst_roam_info->alg_param.tx_pkt_param.tx_failed_pkt[0];
            tx_success_sum = tx_success_pkt - pst_roam_info->alg_param.tx_pkt_param.tx_success_pkt[0];
            if(tx_failed_sum > 0)
            {
                tx_failed_scale = (100 * tx_failed_sum)  / (tx_failed_sum + tx_success_sum);
            }
            else
            {
                tx_failed_scale = 0;
            }

            HWIFI_INFO("tx_failed_sum=%u,tx_success_sum=%u,tx_failed_scale=%u,current_rssi=%d,tx_failed_trigger_flag=%d,curr_rate=%d",
                        tx_failed_sum, tx_success_sum, tx_failed_scale,
                        cfg->latest_signal, pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag, curr_rate);

            if((tx_failed_scale >= pst_roam_info->alg_param.tx_pkt_param.tx_failed_scale_threshold)
               &&(cfg->latest_signal > pst_roam_info->alg_param.tx_pkt_param.rssi_threshold)
               &&((tx_failed_sum + tx_success_sum) > pst_roam_info->alg_param.tx_pkt_param.tx_packet_num_threshold)
               &&(curr_rate <= pst_roam_info->alg_param.tx_pkt_param.tx_rate_threshold))
            {
                HWIFI_INFO("roam_process_trigger due to high tx packet scale failed.");
                pst_roam_info->alg_param.tx_pkt_param.tx_failed_trigger_flag = 1;
                roam_process_trigger(pst_roam_info);
            }
        }
    }


}

#endif

#ifdef MAC_802_11W
/*
 * Prototype    : hwifi_pmf_send_nl_msg
 * Description  : send netlink msg to host app
 * Input        : skb
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_recv_pmf_report
 *
 *   History        :
 *   1.Date         : 2013/2/1
 *     Author       : k53369
 *     Modification : Created function
 *
 */
int32 hwifi_pmf_send_nl_msg(struct cfg_struct *cfg, uint8 *buf, uint32 msg_len)

{
    uint32           size;
    struct sk_buff  *skb;
    struct nlmsghdr *nlh;
    int retval;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    size = NLMSG_SPACE(msg_len);

    skb = alloc_skb(size, GFP_ATOMIC);
    if (NULL == skb)
    {
        HWIFI_ERROR("Failed to alloc skb!");
        return -EFAIL;
    }

    /* 初始化一个netlink消息首部，TBD */
    nlh = nlmsg_put(skb, 0, 0, 0, msg_len, 0);

    /* 填充数据区 */
    memcpy(NLMSG_DATA(nlh), buf, msg_len);

    /* 设置消息长度 */
    nlh->nlmsg_len = NLMSG_LENGTH(msg_len);

    /* 设置控制字段 */
    NETLINK_CB(skb).portid = 0;
    NETLINK_CB(skb).dst_group = 0;

    HWIFI_INFO("[kernel space] skb->data:%s\n", (char *)NLMSG_DATA((struct nlmsghdr *)skb->data));

    //发送数据
    retval = netlink_unicast(cfg->pmf_info.nl_sk, skb, cfg->pmf_info.user_pid, MSG_DONTWAIT);

    HWIFI_INFO("[kernel space] netlink_unicast return: %d\n", retval);

    return retval;
}

/*
 * Prototype    : hwifi_pmf_resv_nl_msg
 * Description  : receive netlink msg from host app
 * Input        : skb
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : null
 *
 *   History        :
 *   1.Date         : 2013/2/1
 *     Author       : k53369
 *     Modification : Created function
 *
 */
void hwifi_pmf_resv_nl_msg(struct sk_buff *skb)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nl_hdr;
    uint8 *nl_data;
    uint32 nl_data_len;

    HWIFI_ASSERT((NULL != g_hi110x_dev)|| (NULL != g_hi110x_dev->cfg) || (NULL != skb));

    HWIFI_INFO("[Host][Driver], receive netlink msg from host app\n.");

    /* TBD 需要考虑skb在什么地方释放 */
    nl_skb = skb_get(skb);
    nl_hdr = (struct nlmsghdr *)skb->data;

    HWIFI_INFO("[Host][Driver], nl_hdr->nlmsg_len = %d, sizeof(struct nlmsghdr) = %zu, \
               nl_skb->len = %d, nl_hdr->nlmsg_len = %d", nl_hdr->nlmsg_len, sizeof(struct nlmsghdr),
               nl_skb->len, nl_hdr->nlmsg_len);

    if((nl_hdr->nlmsg_len >= sizeof(struct nlmsghdr))&& (nl_skb->len >= nl_hdr->nlmsg_len))
    {
        g_hi110x_dev->cfg->pmf_info.user_pid = nl_hdr->nlmsg_pid;

        HWIFI_INFO("Data receive from user are:%s\n", (char *)NLMSG_DATA(nl_hdr));
        HWIFI_INFO("user_pid:%d\n", nl_hdr->nlmsg_pid);

        nl_data = (uint8 *)nlmsg_data(nl_hdr);
        nl_data_len = nlmsg_len(nl_hdr);
        HWIFI_INFO("msg_type=%d, nl_data_len = %d\n", nl_data[0], nl_data_len);

        if (nl_data[0] != 0xFF)
        {
            hwifi_send_pmf_cmd(g_hi110x_dev->cfg, nl_data, nl_data_len);
        }
    }

    kfree_skb(skb);

    return;
}

/*
 * Prototype    : hwifi_pmf_init
 * Description  : create netlink in kernel
 * Input        : cfg
                : buf : buf of msg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_init_cfg
 *
 *   History        :
 *   1.Date         : 2013/1/28
 *     Author       :
 *     Modification : Created function
 *
 */
int32 hwifi_pmf_init(struct cfg_struct *cfg)
{

    struct netlink_kernel_cfg netlink_cfg = {
         .input = hwifi_pmf_resv_nl_msg,
    };
    HWIFI_ASSERT(NULL != cfg);

    cfg->pmf_info.dot11RSNAProtectedManagementFramesActivated = 0;
    cfg->pmf_info.dot11RSNAUnprotectedManagementFramesAllowed = 0;

//    cfg->pmf_info.nl_sk = netlink_kernel_create(&init_net, NETLINK_PMF, 0, hwifi_pmf_resv_nl_msg, NULL, THIS_MODULE);
    cfg->pmf_info.nl_sk = netlink_kernel_create(&init_net, NETLINK_PMF, &netlink_cfg);
    if(NULL == cfg->pmf_info.nl_sk)
    {
        HWIFI_ERROR("Failed to create kernel netlink!");
        return -EFAIL;
    }

    HWIFI_INFO("Succeed  to create kernel netlink");

    return SUCC;
}

/*
 * Prototype    : hwifi_send_pmf_cmd
 * Description  : send the cmd msg to device
 * Input        : cfg
                : buf : buf of msg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_pmf_resv_nl_msg
 *
 *   History        :
 *   1.Date         : 2013/1/28
 *     Author       :
 *     Modification : Created function
 *
 */
int32 hwifi_send_pmf_cmd(struct cfg_struct *cfg, uint8 *buf, uint32 buf_len)
{
    uint8           *skb_data;
    struct sk_buff  *skb;
    uint16           msg_size;
    int32            ret;
#ifdef __CHECKSUM__
    uint8           *checksum;
#endif

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct binary_wid) + buf_len;
#ifdef __CHECKSUM__
    msg_size += sizeof(*checksum);
#endif

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    skb_data = (uint8 *)skb_put(skb, (sizeof(struct hwifi_msg_header) + sizeof(struct binary_wid) + buf_len));

    hwifi_fill_msg_hdr((struct hwifi_msg_header *)skb_data, HOST_CMD_PMF, msg_size);

    skb_data = skb_data + sizeof(struct hwifi_msg_header);
    hwifi_fill_binary_wid((struct binary_wid *)(skb_data), WID_PMF, buf_len, buf);

#ifdef __CHECKSUM__
    checksum= (uint8 *)skb_tail_pointer(skb);
    *checksum = bytes_check_sum(((struct binary_wid *)skb_data)->val, ((struct binary_wid *)skb_data)->len);
    skb_put(skb, sizeof(*checksum));
#endif

    {
        uint32 msg_length;
        uint8 *data;
        data = skb->data;
        for (msg_length = 0; msg_length < msg_size; msg_length++)
        {
            if (0 == (msg_length % 16) && (msg_length > 0))
                printk("\n");

            printk(KERN_ERR"[WIFI][INFO][%02X]",data[msg_length]);
        }
    }

    /* send pmf cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("pmf msg to device", ret);
    return ret;
}

/*
 * Prototype    : hwifi_recv_pmf_report
 * Description  : handle the msg reported by device
 * Input        : cfg
                : buf : buf of msg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_proc_asyn_msg
 *
 *   History        :
 *   1.Date         : 2013/1/28
 *     Author       :
 *     Modification : Created function
 *
 */
int32 hwifi_recv_pmf_report(struct cfg_struct *cfg, uint8 *buf)
{
    CFG_MSG_PROTECTED_MGMT_STRU   *msg;
    uint32                         msg_len;
    int32                          ret = -EFAIL;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    HWIFI_INFO("[Host][Driver]Receive from device.\n");

    msg = (CFG_MSG_PROTECTED_MGMT_STRU *)buf;
    msg_len = OS_LE16_TO_CPU(msg->stMsgHdr.usMsgLength);

    /* set the ptk  key index and key value */
    msg->ptk_index = cfg->key_mgmt.ptk_key_index;
    if (msg->ptk_index >= 4)
    {
        msg->ptk_type = 0xFFFFFFFF;
    }
    else
    {
        msg->ptk_type = cfg->key_mgmt.ptk[cfg->key_mgmt.ptk_key_index].cipher;
        memcpy(msg->ptk_key, cfg->key_mgmt.ptk[cfg->key_mgmt.ptk_key_index].key, 16);
    }

    if (0 == msg->ptk_type)
    {
        msg->igtk_type = 0xFFFFFFFF;
    }

    /* set the igtk key index and key value */
    msg->igtk_index = cfg->key_mgmt.igtk_key_index;
    if (msg->igtk_index >= 6)
    {
        msg->igtk_type = 0xFFFFFFFF;
    }
    else
    {
        msg->igtk_type = cfg->key_mgmt.igtk[cfg->key_mgmt.igtk_key_index].cipher;
        memcpy(msg->igtk_key, cfg->key_mgmt.igtk[cfg->key_mgmt.igtk_key_index].key, 16);
    }

    if (WLAN_CIPHER_SUITE_AES_CMAC != msg->igtk_type)
    {
        msg->igtk_type = 0xFFFFFFFF;
    }

    ret = hwifi_pmf_send_nl_msg(cfg, buf, msg_len);

    return ret;
}

/*
 * Prototype    : hwifi_pmf_exit
 * Description  : release netlink in kernel
 * Input        : cfg
                : buf : buf of msg
 * Output       : None
 * Return Value : SUCC or failure code
 * Calls        : null
 * Called By    : hwifi_exit_cfg
 *
 *   History        :
 *   1.Date         : 2013/1/28
 *     Author       :
 *     Modification : Created function
 *
 */
void hwifi_pmf_exit(struct cfg_struct *cfg)
{
    HWIFI_ASSERT(NULL != cfg);

    netlink_kernel_release(cfg->pmf_info.nl_sk);

    cfg->pmf_info.user_pid = 0;

    HWIFI_INFO("Succeed to release netlink from kernel!");

    return;
}

/*****************************************************************************
 函 数 名  : pmf_set_params
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年2月27日
    作    者   : mayuan
    修改内容   : 新生成函数

*****************************************************************************/
int32  pmf_set_params(struct cfg_struct *cfg)
{
    struct sk_buff      *skb;
    struct pmf_msg      *msg;       /* pointer to cfg cmd message struct */
    uint16               msg_size;  /* cfg message size */
    int32                ret = SUCC;

    HWIFI_ASSERT(NULL != cfg);

    msg_size = sizeof(struct pmf_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct pmf_msg *)skb_put(skb, sizeof(struct pmf_msg));

    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_PMF, msg_size);

    HWIFI_INFO("pmf_set_params: allowed = %d, activated = %d.\n", cfg->pmf_info.dot11RSNAUnprotectedManagementFramesAllowed, cfg->pmf_info.dot11RSNAProtectedManagementFramesActivated);

    hwifi_fill_char_wid(&msg->pmf_activated, WID_PMF_ACTIVATED, cfg->pmf_info.dot11RSNAProtectedManagementFramesActivated);
    hwifi_fill_char_wid(&msg->pmf_allowed, WID_UNPMF_ALLOWED, cfg->pmf_info.dot11RSNAUnprotectedManagementFramesAllowed);

    /* send pmf cmd to device */
    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("PMF", ret);

    return ret;
}


/*****************************************************************************
 函 数 名  : pmf_param
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年2月26日
    作    者   : mayuan
    修改内容   : 新生成函数

*****************************************************************************/
int32 pmf_param(struct cfg_struct* cfg, int param_cmd , int32 *param_value, int32 *get)
{
    struct pmf_info_stru *pmf_params;
    int32                 ret = SUCC;

    HWIFI_ASSERT((NULL != cfg) && (NULL != param_value) && (NULL != get));

    pmf_params = &cfg->pmf_info;
    HWIFI_ASSERT(NULL != pmf_params);

    *get = FALSE;

    if (IS_GET_PMF_CMD(param_cmd))
    {
        *get = TRUE;
    }

    switch(param_cmd)
    {
        /* set pmf params to device */
        case PMF_PARAM_CMD_COMMIT:
            ret = pmf_set_params(cfg);
        break;

        /* set */
        case PMF_PARAM_CMD_SET_ACTIVATED:
            pmf_params->dot11RSNAProtectedManagementFramesActivated = (uint8)(*param_value);
        break;

        case PMF_PARAM_CMD_SET_ALLOWED:
            pmf_params->dot11RSNAUnprotectedManagementFramesAllowed = (uint8)(*param_value);
        break;

        /* get */
        case PMF_PARAM_CMD_GET_ACTIVATED:
            *param_value =(int32)pmf_params->dot11RSNAProtectedManagementFramesActivated;
        break;

        case PMF_PARAM_CMD_GET_ALLOWED:
            *param_value = (int32)pmf_params->dot11RSNAUnprotectedManagementFramesAllowed;
        break;

        default:
            HWIFI_INFO("pmf_param, invalid param cmd = %d\n", param_cmd);
        break;
    }

    return ret;
}


#endif


/*
 * Prototype    : wl_set_max_sta_num
 * Description  : set max sta num to device
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/13
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 wl_set_max_sta_num(struct cfg_struct *cfg, int32 num)
{
    struct sta_num_msg *set_num;
    struct              sk_buff *skb;
    uint8              *msg;
    uint16              msg_size;
    int32               ret;

    if ((num < 0) || (num > MAX_DEVICE_NUM))
    {
        HWIFI_WARNING("max_sta_num[%d] is too big, set default max num:%d", num, MAX_DEVICE_NUM);
        num = MAX_DEVICE_NUM;
    }

    msg_size = sizeof(struct sta_num_msg);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    set_num = (struct sta_num_msg *)msg;
    hwifi_fill_msg_hdr(&set_num->msg_hdr, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_char_wid(&set_num->sta_num, WID_MAX_NUM_STA, (uint8)num);
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set max sta num", ret);
    return ret;
}

/*
 * Prototype    : hwifi_process_disconn_event
 * Description  : process disconnect msg irq from device
 * Input        : None
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/10/15
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_process_disconn_event(struct cfg_struct *cfg)
{
    int32  ret;
    uint16 reason = 1;
    ret = hwifi_report_diconnect_res(cfg, reason);
    if (ret != SUCC)
    {
        HWIFI_WARNING("fail to report disconn result");
    }
    return ret;
}

/*
 * Prototype    : hwifi_set_lte_coex_enable
 * Description  : set lte_coex status to device
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/3/6
 *     Author       : huangtao/h00240371
 *     Modification : Created function
 *
 */

int32 hwifi_set_lte_coex_paras(struct cfg_struct *cfg, int8 lte_paras)
{
    struct lte_coex_paras_msg *lte_coex_set;
    struct              sk_buff *skb;
    uint8   *msg;
    uint16              msg_size;
    int32               ret;

    HWIFI_WARNING("hwifi_set_lte_coex_enable lte_enable=%d",lte_paras);

    msg_size = sizeof(struct lte_coex_paras_msg);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d",msg_size);
        return -EFAIL;
    }

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = skb_put(skb, msg_size);

    /* init msg */
    memset(msg, 0, msg_size);

    lte_coex_set = (struct lte_coex_paras_msg *)msg;
    hwifi_fill_msg_hdr(&lte_coex_set->msg_hdr, HOST_CMD_CONFIG, msg_size);
    //hwifi_fill_short_wid(&lte_coex_set->lte_coex_paras, WID_LTE_COEX_ENABLE,
    //                    ((LTECOEX_CONFIG_CMD << 8) | lte_paras));
    if (LTE_COEX_SET_DISABLE == lte_paras || LTE_COEX_SET_ENABLE == lte_paras)
    {
        hwifi_fill_short_wid(&lte_coex_set->lte_coex_paras, WID_LTE_COEX_ENABLE,
                        ((LTECOEX_CONFIG_CMD << 8) | lte_paras));
        HWIFI_WARNING("hwifi_set_lte_coex_paras, short_wid = %x", ((LTECOEX_CONFIG_CMD << 8) | lte_paras));
    }
    else if (LTE_COEX_WLAN_PRIORITY_LOW == lte_paras || LTE_COEX_WLAN_PRIORITY_HIGH == lte_paras)
    {
        hwifi_fill_short_wid(&lte_coex_set->lte_coex_paras, WID_LTE_COEX_ENABLE,
                        ((LTECOEX_PRIORITY_CMD << 8) | (lte_paras - LTE_COEX_WLAN_PRIORITY_LOW)));
        HWIFI_WARNING("hwifi_set_lte_coex_paras, short_wid = %x",
                    ((LTECOEX_PRIORITY_CMD << 8) | (lte_paras - LTE_COEX_WLAN_PRIORITY_LOW)));
    }
    else
    {
        //It should not be here
        HWIFI_WARNING("hwifi_set_lte_coex_paras, wrong lte_paras = %d", lte_paras);
    }
    ret = hwifi_send_cmd(cfg, skb);

    //Save coex status
    if (SUCC == ret &&
        (LTE_COEX_SET_DISABLE == lte_paras || LTE_COEX_SET_ENABLE == lte_paras))
    {
        g_lte_wifi_coex = lte_paras;
    }

    HWIFI_WARNING("hwifi_set_lte_coex_paras, ret = %d", ret);
    //PRINT_SEND_CMD_RET("hwifi_set_lte_coex_enable", ret);
    return ret;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

