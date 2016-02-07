/******************************************************************************
 *
 * Copyright (C), 2001-2012, Huawei Tech. Co., Ltd.
 *
 *
 * File Name     : hwifi_aggre.c
 * Version       : Initial Draft
 * Author        : yWX164715
 * Created       : 2013/3/19
 * Last Modified :
 * Description   : aggregation
 * Function List :
 * History       :
 * 1.Date        : 2013/3/19
 *   Author      : yWX164715
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

#include "hwifi_aggre.h"
#include "cfg80211_stru.h"
#include "cfg_event_rx.h"
#include "hwifi_cfg80211.h"
#include "hwifi_hcc.h"


/*
 * 2 Global Variable Definition
 */
#ifdef WLAN_ARRG_DYNAMIC_CONTROL
#define AGREE_DATE_TIMOUT       (1)
#define AGGR_RATE_DOWN          (625000)/* 5Mb/s*/
#define AGGR_RATE_UP            (1125000)/* 9Mb/s*/

#define WLAN_SHORT_PACKET_LEN   500

unsigned long g_data_rate_pps_down  = 50;
unsigned long g_data_rate_pps_up    = 100;

unsigned long g_data_rate_down    = AGGR_RATE_DOWN*AGREE_DATE_TIMOUT;
unsigned long g_data_rate_up      = AGGR_RATE_UP*AGREE_DATE_TIMOUT;
unsigned long g_ap_data_rate_down = AGGR_RATE_DOWN*AGREE_DATE_TIMOUT;
unsigned long g_ap_data_rate_up   = AGGR_RATE_UP*AGREE_DATE_TIMOUT;

#define M40M_AGGR_NUM_RATE_UP              (50)
#define M40M_AGGR_NUM_RATE_DOWN            (30)
#define M20M_AGGR_NUM_RATE_UP              (25)
#define M20M_AGGR_NUM_RATE_DOWN            (20)
#define MAX_AGGR_NUM_16                    (32)
#define MAX_AGGR_NUM_8                     (16)

#define WID_FREQ_PLAN_FOR_AGGRE_NUM_LEN    (3)

unsigned long g_aggr_num_rate_up    =  (M20M_AGGR_NUM_RATE_UP * 1000000 / 8);
unsigned long g_aggr_num_rate_down  =  (M20M_AGGR_NUM_RATE_DOWN * 1000000 / 8);
unsigned long g_rate_sum            =  0;
unsigned char g_count_aggre         =  0;

extern uint8 g_use_dync_16vs8;
extern uint8 g_method;


module_param(g_data_rate_up, ulong, S_IRUGO|S_IWUSR);
module_param(g_data_rate_down, ulong, S_IRUGO|S_IWUSR);
#endif

/*
 * 3 Function Definition
 */


void hwifi_fill_teardown(struct set_teardown_wid *set_teardown)
{
    HWIFI_ASSERT(set_teardown != NULL);

    set_teardown->wid           = WID_11N_P_ACTION_REQ;
    set_teardown->wid_len       = sizeof(struct set_teardown_wid) - sizeof(struct string_wid);
    set_teardown->context_len   = sizeof(struct set_teardown_wid) - sizeof(struct string_wid);
    set_teardown->msg_type      = N_P_ACTION_REQ;
    set_teardown->flags         = 0xFF;
}



int32 hwifi_set_teardown(struct cfg_struct *cfg)
{
    struct set_teardown_msg *msg;
    struct sk_buff          *skb;
    uint16                   msg_size;
    int32                    ret;

    HWIFI_ASSERT(NULL != cfg);

    msg_size = sizeof(struct set_teardown_msg);
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

    msg = (struct set_teardown_msg *)skb_put(skb, sizeof(struct set_teardown_msg));

    hwifi_fill_msg_hdr(&msg->msg_header, HOST_CMD_CONFIG, msg_size);

    /* add teardown wid */
    hwifi_fill_teardown(&msg->set_teardown);

    /* send teardown wid to clear AMPDU infomation */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set teardown", ret);

    return SUCC;
}



STATIC uint8 convert_ac_to_tid(uint8 traffic_class)
{
    if (AGGRE_AC_BE == traffic_class || AGGRE_AC_BK == traffic_class)
    {
        return traffic_class;
    }
    else if (AGGRE_AC_VI == traffic_class || AGGRE_AC_VO == traffic_class)
    {
        return (uint8)((traffic_class << 1) + 1);
    }
    else
    {
        HWIFI_WARNING("TID value is error");
        return AGGRE_AC_BE;
    }
}


/*
 * Prototype    : hwifi_fill_addba
 * Description  : fill addba wid
 * Input        : struct set_ba_wid *set_ba, struct aggre_cfg_param *param
 * Output       : NONE
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : ywx164715
 *     Modification : Created function
 */
void hwifi_fill_addba(struct set_addba_wid *set_ba, struct aggre_cfg_param *param)
{
    HWIFI_ASSERT((set_ba != NULL) && (param != NULL));

    set_ba->ba_wid.wid              = WID_11E_P_ACTION_REQ;
    set_ba->ba_wid.wid_len          = 0x14;
    set_ba->ba_wid.context_len      = 0x14;
    set_ba->ba_wid.msg_type         = BA_CATEGORY;
    set_ba->ba_wid.action_type      = param->ba_action_type;
    set_ba->ba_wid.TID              = convert_ac_to_tid(param->TID);
    set_ba->ba_policy               = 0x01;
    set_ba->buf_size                = 0x20;
    set_ba->ba_timeout              = 0x00;
    set_ba->addba_timeout           = 0x1F4;
    set_ba->max_frame               = param->max_num;
    set_ba->buf_timeout             = 0x64;

}


/*
 * Prototype    : hwifi_fill_delba
 * Description  : fill set ba wid
 * Input        : struct set_ba_wid *set_ba, struct aggre_cfg_param *param
 * Output       : NONE
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : ywx164715
 *     Modification : Created function
 */
void hwifi_fill_delba(struct set_delba_wid *set_ba, struct aggre_cfg_param *param)
{
    HWIFI_ASSERT((set_ba != NULL) && (param != NULL));

    set_ba->ba_wid.wid              = WID_11E_P_ACTION_REQ;
    set_ba->ba_wid.wid_len          = 0x0C;
    set_ba->ba_wid.context_len      = 0x0C;
    set_ba->ba_wid.msg_type         = BA_CATEGORY;
    set_ba->ba_wid.action_type      = param->ba_action_type;
    set_ba->ba_wid.TID              = convert_ac_to_tid(param->TID);
    set_ba->direction               = 0x01;
    set_ba->result_code             = 0x20;
}


/*
 * Prototype    : hwifi_fill_start_amsdu
 * Description  : fill set amsdu wid
 * Input        : struct set_amsdu_wid *set_amsdu, struct aggre_cfg_param *param
 * Output       : NONE
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : ywx164715
 *     Modification : Created function
 */
void hwifi_fill_start_amsdu(struct set_start_amsdu_wid *set_amsdu, struct aggre_cfg_param *param)
{
    HWIFI_ASSERT((set_amsdu != NULL) && (param != NULL));

    set_amsdu->amsdu_wid.wid                = WID_11N_P_ACTION_REQ;
    set_amsdu->amsdu_wid.wid_len            = 0x0F;
    set_amsdu->amsdu_wid.context_len        = 0x0F;
    set_amsdu->amsdu_wid.msg_type           = N_P_ACTION_REQ;

    /* IOCTL input 1 means start AMSDU; input 0 means end AMSDU*/
    set_amsdu->amsdu_wid.action_type        = param->amsdu_action_type;
    set_amsdu->amsdu_wid.TID                = convert_ac_to_tid(param->TID);
    set_amsdu->max_num                      = param->max_num;
    set_amsdu->amsdu_max_size               = OS_CPU_TO_LE16(MAX_AMSDU_SIZE);
    set_amsdu->timeout                      = MAX_AMSDU_TIMEOUT;
}


/*
 * Prototype    : hwifi_fill_end_amsdu
 * Description  : fill set amsdu wid
 * Input        : struct set_amsdu_wid *set_amsdu, struct aggre_cfg_param *param
 * Output       : NONE
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : ywx164715
 *     Modification : Created function
 */
void hwifi_fill_end_amsdu(struct set_amsdu_wid *set_amsdu, struct aggre_cfg_param *param)
{
    HWIFI_ASSERT((set_amsdu != NULL) && (param != NULL));

    set_amsdu->wid             = WID_11N_P_ACTION_REQ;
    set_amsdu->wid_len         = 0x0A;
    set_amsdu->context_len     = 0x0A;
    set_amsdu->msg_type        = N_P_ACTION_REQ;

    /* IOCTL input 1 means start AMSDU; input 0 means end AMSDU*/
    set_amsdu->action_type     = param->amsdu_action_type;
    set_amsdu->TID             = convert_ac_to_tid(param->TID);
}


/*
 * Prototype    : is_zero_mac_addr
 * Description  : check if mac address is zero
 * Input        : uint8 *mac_addr
 * Output       : NONE
 * Return Value : int8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/04/9
 *     Author       : ywx164715
 *     Modification :
 */
int8 is_zero_mac_addr(uint8 *mac_addr)
{
    uint8 i;

    HWIFI_ASSERT(mac_addr != NULL);

    /* check if mac address is zero */
    for(i = 0; i < MAC_LEN; i++)
    {
        if(mac_addr[i] != 0)
        {
            /* not zero mac address */
            return 0;
        }
    }

    /* zero mac address */
    return 1;
}
int32 hwifi_set_amsdu(struct cfg_struct *cfg, struct aggre_cfg_param *param)
{
    struct set_start_amsdu_msg    *start_msg;
    struct set_end_amsdu_msg      *end_msg;
    struct sk_buff          *skb;
    uint16                   msg_size;
    int32                    ret;

    HWIFI_ASSERT(NULL != cfg);

    if(param->amsdu_action_type == A_MSDU_START)
    {
        msg_size = sizeof(struct set_start_amsdu_msg);
        if (msg_size > MAX_MSG_LEN)
        {
            HWIFI_WARNING("Invalid too long start amsdu msg size:%d",msg_size);
            return -EFAIL;
        }

        skb = hwifi_alloc_skb_for_cmd(msg_size);
        if (NULL == skb)
        {
            return -ENOMEM;
        }

        start_msg = (struct set_start_amsdu_msg *)skb_put(skb, sizeof(struct set_start_amsdu_msg));

        hwifi_fill_msg_hdr(&start_msg->msg_header, HOST_CMD_CONFIG, msg_size);

        /* add amsdu wid */
        hwifi_fill_start_amsdu(&start_msg->set_amsdu, param);

        if (is_zero_mac_addr(param->mac_addr))
        {    /* use user data */
            memcpy(start_msg->set_amsdu.amsdu_wid.bssid, param->mac_addr, MAC_LEN);
        }
        else if (IS_AP(cfg))
        {
            /* AP mode */
            memcpy(start_msg->set_amsdu.amsdu_wid.bssid, cfg->ap_info.sta_mgmt.stations[CONNECTED_STA_PRE].mac, MAC_LEN);
        }
        else if (IS_STA(cfg))
        {
            /* STA mode copy mac address fill ba and ampdu struct*/
            memcpy(start_msg->set_amsdu.amsdu_wid.bssid, cfg->conn.bss.bssid, MAC_LEN);
        }
    }
    else
    {
        msg_size = sizeof(struct set_end_amsdu_msg);
        if (msg_size > MAX_MSG_LEN)
        {
            HWIFI_WARNING("Invalid too long end amsdu msg size:%d",msg_size);
            return -EFAIL;
        }

        skb = hwifi_alloc_skb_for_cmd(msg_size);
        if (NULL == skb)
        {
            return -ENOMEM;
        }

        end_msg = (struct set_end_amsdu_msg *)skb_put(skb, sizeof(struct set_end_amsdu_msg));

        hwifi_fill_msg_hdr(&end_msg->msg_header, HOST_CMD_CONFIG, msg_size);

        /* add amsdu wid */
        hwifi_fill_end_amsdu(&end_msg->set_amsdu, param);

        if (is_zero_mac_addr(param->mac_addr))
        {    /* use user data */
            memcpy(end_msg->set_amsdu.bssid, param->mac_addr, MAC_LEN);
        }
        else if (IS_AP(cfg))
        {
            /* AP mode */
            memcpy(end_msg->set_amsdu.bssid, cfg->ap_info.sta_mgmt.stations[CONNECTED_STA_PRE].mac, MAC_LEN);
        }
        else if (IS_STA(cfg))
        {
            /* STA mode copy mac address fill ba and ampdu struct*/
            memcpy(end_msg->set_amsdu.bssid, cfg->conn.bss.bssid, MAC_LEN);
        }
    }

    /* send amsdu wid to Device for test */
    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set amsdu", ret);

    return ret;
}
void hwifi_fill_start_ampdu(struct set_start_ampdu_wid *set_ampdu, struct aggre_cfg_param *param)
{
    HWIFI_ASSERT((set_ampdu != NULL) && (param != NULL));

    set_ampdu->ampdu_wid.wid          = WID_11N_P_ACTION_REQ;
    set_ampdu->ampdu_wid.wid_len      = 0x0E;
    set_ampdu->ampdu_wid.context_len  = 0x0E;
    set_ampdu->ampdu_wid.msg_type     = N_P_ACTION_REQ;
    set_ampdu->ampdu_wid.action_type  = param->ampdu_action_type;
    set_ampdu->ampdu_wid.TID          = convert_ac_to_tid(param->TID);
    set_ampdu->max_mpdu               = param->max_num;
    set_ampdu->timeout                = MAX_AMPDU_TIMEOUT;
    set_ampdu->ack_policy             = 0;
}



void hwifi_fill_end_ampdu(struct set_ampdu_wid *set_ampdu, struct aggre_cfg_param *param)
{
    HWIFI_ASSERT((set_ampdu != NULL) && (param != NULL));

    set_ampdu->wid          = WID_11N_P_ACTION_REQ;
    set_ampdu->wid_len      = 0x0A;
    set_ampdu->context_len  = 0x0A;
    set_ampdu->msg_type     = N_P_ACTION_REQ;
    set_ampdu->action_type  = param->ampdu_action_type;
    set_ampdu->TID          = convert_ac_to_tid(param->TID);
}


/*
 * Prototype    : hwifi_fill_aggre_cfg_wid
 * Description  : fill ba ampdu config wid
 * Input        : struct set_aggre_cfg_wid *set_aggre_cfg,
                  struct host_ba_cfg_param *param
 * Output       : NONE
 * Return Value : int32
 * Calls        :
 * Called By    : wl_aggre_cfg_set()
 *
 *   History        :
 *   1.Date         : 2013/03/19
 *     Author       : ywx164715
 *     Modification : Created function
 */
void hwifi_fill_aggre_cfg_wid(struct set_aggre_cfg_wid *set_aggre_cfg,
                                     struct host_ba_cfg_param *param)
{
    HWIFI_ASSERT((set_aggre_cfg != NULL) && (param != NULL));

    set_aggre_cfg->wid          = WID_11N_BA_AMPDU_PARAM;
    set_aggre_cfg->wid_len      = 0x04;
    set_aggre_cfg->max_tx       = param->max_tx;
    set_aggre_cfg->max_rx       = param->max_rx;
    set_aggre_cfg->max_retry    = param->max_retry;
}


/*
 * Prototype    : hwifi_fill_query_ba_wid
 * Description  : fill query ba wid
 * Input        : struct set_query_ba_wid *set_query_ba,
 * Output       : NONE
 * Return Value : int32
 * Calls        :
 * Called By    : wl_query_ba_set()
 *
 *   History        :
 *   1.Date         : 2013/03/19
 *     Author       : ywx164715
 *     Modification : Created function
 */
void hwifi_fill_query_ba_wid(struct set_query_ba_wid *set_query_ba,
                                    struct query_ba_param *param)
{
    HWIFI_ASSERT((set_query_ba != NULL) && (param != NULL));

    set_query_ba->wid        = WID_BA_INFO_QUERY;
    set_query_ba->wid_len    = 0x07;
    set_query_ba->flag       = param->flag;
}



int32  hwifi_set_ba(struct cfg_struct *cfg, struct aggre_cfg_param *param)
{
    struct set_addba_msg      *adb_msg;
    struct set_delba_msg      *del_msg;

    struct sk_buff            *skb;
    uint16                    msg_size;
    int32                     ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != param));

    if(param->ba_action_type == MLME_ADDBA_REQ_TYPE)
    {
        msg_size = sizeof(struct set_addba_msg);

        if (msg_size > MAX_MSG_LEN)
        {
            HWIFI_WARNING("Invalid too long set addba msg size:%d!",msg_size);
            return -EFAIL;
        }

        skb = hwifi_alloc_skb_for_cmd(msg_size);
        if (NULL == skb)
        {
            return -ENOMEM;
        }

        adb_msg = (struct set_addba_msg *)skb_put(skb, sizeof(struct set_addba_msg));

        /* add msg header */
        hwifi_fill_msg_hdr(&adb_msg->msg_header, HOST_CMD_CONFIG, msg_size);

        /* add ba wid */
        hwifi_fill_addba(&adb_msg->set_ba, param);

        if (is_zero_mac_addr(param->mac_addr))
        {    /* use user data */
            memcpy(adb_msg->set_ba.ba_wid.bssid, param->mac_addr, MAC_LEN);
        }
        else if (IS_AP(cfg))
        {
            /* AP mode */
            memcpy(adb_msg->set_ba.ba_wid.bssid,
                   cfg->ap_info.sta_mgmt.stations[CONNECTED_STA_PRE].mac,
                   MAC_LEN);
        }
        else if (IS_STA(cfg))
        {
            /* STA mode copy mac address fill ba and ampdu struct*/
            memcpy(adb_msg->set_ba.ba_wid.bssid, cfg->conn.bss.bssid, MAC_LEN);
        }
    }
    else
    {
        msg_size = sizeof(struct set_delba_msg);

        if (msg_size > MAX_MSG_LEN)
        {
            HWIFI_WARNING("Invalid too long delba msg size:%d",msg_size);
            return -EFAIL;
        }

        skb = hwifi_alloc_skb_for_cmd(msg_size);
        if (NULL == skb)
        {
            return -ENOMEM;
        }

        del_msg = (struct set_delba_msg *)skb_put(skb, sizeof(struct set_delba_msg));

        /* add msg header */
        hwifi_fill_msg_hdr(&del_msg->msg_header, HOST_CMD_CONFIG, msg_size);

        /* add ba wid */
        hwifi_fill_delba(&del_msg->set_ba, param);

        if (is_zero_mac_addr(param->mac_addr))
        {    /* use user data */
            memcpy(del_msg->set_ba.ba_wid.bssid, param->mac_addr, MAC_LEN);
        }
        else if (IS_AP(cfg))
        {
            /* AP mode */
            memcpy(del_msg->set_ba.ba_wid.bssid, cfg->ap_info.sta_mgmt.stations[CONNECTED_STA_PRE].mac, MAC_LEN);
        }
        else if (IS_STA(cfg))
        {
            /* STA mode copy mac address fill ba and ampdu struct*/
            memcpy(del_msg->set_ba.ba_wid.bssid, cfg->conn.bss.bssid, MAC_LEN);
        }
    }

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set ba", ret);

    return ret;
}



int32  hwifi_set_ampdu(struct cfg_struct *cfg, struct aggre_cfg_param *param)
{
    struct set_start_ampdu_msg  *start_msg;
    struct set_end_ampdu_msg    *end_msg;
    struct sk_buff        *skb;
    uint16                 msg_size;
    int32                  ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != param));

    if(param->ampdu_action_type == A_MPDU_START)
    {
        msg_size = sizeof(struct set_start_ampdu_msg);
        if (msg_size > MAX_MSG_LEN)
        {
            HWIFI_WARNING("Invalid too long start ampdu msg size:%d",msg_size);
            return -EFAIL;
        }

        skb = hwifi_alloc_skb_for_cmd(msg_size);
        if (NULL == skb)
        {
            return -ENOMEM;
        }

        start_msg = (struct set_start_ampdu_msg *)skb_put(skb, sizeof(struct set_start_ampdu_msg));

        /* add msg header */
        hwifi_fill_msg_hdr(&start_msg->msg_header, HOST_CMD_CONFIG, msg_size);
        /*add ampdu wid */
        hwifi_fill_start_ampdu(&start_msg->set_ampdu, param);

        if (is_zero_mac_addr(param->mac_addr))
        {    /* use user data */
            memcpy(start_msg->set_ampdu.ampdu_wid.bssid, param->mac_addr, MAC_LEN);
        }
        else if (IS_AP(cfg))
        {
            /* AP mode */
            memcpy(start_msg->set_ampdu.ampdu_wid.bssid, cfg->ap_info.sta_mgmt.stations[CONNECTED_STA_PRE].mac, MAC_LEN);
        }
        else if (IS_STA(cfg))
        {
            /* STA mode copy mac address fill ba and ampdu struct*/
            memcpy(start_msg->set_ampdu.ampdu_wid.bssid, cfg->conn.bss.bssid, MAC_LEN);
        }
    }
    else
    {
        msg_size = sizeof(struct set_end_ampdu_msg);
        if (msg_size > MAX_MSG_LEN)
        {
            HWIFI_WARNING("Invalid too long end ampdu msg size:%d",msg_size);
            return -EFAIL;
        }

        skb = hwifi_alloc_skb_for_cmd(msg_size);
        if (NULL == skb)
        {
            return -ENOMEM;
        }

        end_msg = (struct set_end_ampdu_msg *)skb_put(skb, sizeof(struct set_end_ampdu_msg));

        /* add msg header */
        hwifi_fill_msg_hdr(&end_msg->msg_header, HOST_CMD_CONFIG, msg_size);
        /*add ampdu wid */
        hwifi_fill_end_ampdu(&end_msg->set_ampdu, param);

        if (is_zero_mac_addr(param->mac_addr))
        {    /* use user data */
            memcpy(end_msg->set_ampdu.bssid, param->mac_addr, MAC_LEN);
        }
        else if (IS_AP(cfg))
        {
            /* AP mode */
            memcpy(end_msg->set_ampdu.bssid, cfg->ap_info.sta_mgmt.stations[CONNECTED_STA_PRE].mac, MAC_LEN);
        }
        else if (IS_STA(cfg))
        {
            /* STA mode copy mac address fill ba and ampdu struct*/
            memcpy(end_msg->set_ampdu.bssid, cfg->conn.bss.bssid, MAC_LEN);
        }
    }

    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("set ampdu", ret);

    return ret;
}



int32 hwifi_init_bampdu(struct cfg_struct *cfg)
{
    struct aggre_cfg_param *param;
    int32                ret;

    param = (struct aggre_cfg_param *)kzalloc(sizeof(struct aggre_cfg_param), GFP_KERNEL);
    if (NULL == param)
    {
        HWIFI_WARNING("Failed to alloc aggre cfg param!");
        return -ENOMEM;
    }

    /* fill ba ampdu param struct */
    param->ba_action_type    = MLME_ADDBA_REQ_TYPE;
    param->ampdu_action_type = A_MPDU_START;
    param->TID               = 0;
    param->max_num           = AMPDU_AGGRE_MAX_NUM;

    ret = hwifi_set_ba(cfg, param);
    if (ret < 0)
    {
        HWIFI_WARNING("Failed to set ba!");
        kfree(param);
        return -EFAIL;
    }

    ret = hwifi_set_ampdu(cfg, param);
    if (ret < 0)
    {
        HWIFI_WARNING("Failed to set ampdu!");
        kfree(param);
        return -EFAIL;
    }

    kfree(param);

    return SUCC;
}
int32 wl_set_amsdu(struct cfg_struct *cfg, uint8 *buf)
{
    struct aggre_cfg_param   *param;
    int32                 ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    param = (struct aggre_cfg_param *)buf;

    HWIFI_DEBUG("\naggre_cfg_param->amsdu_action_type = %d\n"
                "\naggre_cfg_param->TID             = %d\n"
                "\naggre_cfg_param->max_num         = %d\n",
                param->amsdu_action_type, param->TID, param->max_num);

    HWIFI_DEBUG("Mac address:" MACFMT, MAC2STR(param->mac_addr));

    if(IS_STA(cfg))
    {
        if (!IS_CONNECTED(cfg))
        {
            HWIFI_WARNING("STATUS is not Connected, ba info is not supported");
            return -EFAIL;
        }
    }

    ret = hwifi_set_amsdu(cfg, param);
    if (SUCC != ret)
    {
        HWIFI_ERROR("Failed to set amsdu!");
        return ret;
    }


    return SUCC;
}



int32 wl_set_ba(struct cfg_struct *cfg, uint8 *buf)
{
    struct aggre_cfg_param      *param;
    int32                    ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    param = (struct aggre_cfg_param *)buf;

    HWIFI_DEBUG("\naggre_cfg_param->ba_action_type = %d\n"
                "\naggre_cfg_param->TID              = %d\n"
                "\naggre_cfg_param->max_num        = %d\n",
                param->ba_action_type, param->TID, param->max_num);

    HWIFI_DEBUG("Mac address:" MACFMT, MAC2STR(param->mac_addr));

    if(IS_STA(cfg))
    {
        if (!IS_CONNECTED(cfg))
        {
            HWIFI_WARNING("STATUS is not Connected, ba info is not supported");
            return -EFAIL;
        }
    }

    ret = hwifi_set_ba(cfg, param);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to set ba!");
        return ret;
    }

    return SUCC;
}



int32 wl_set_ampdu(struct cfg_struct *cfg, uint8 *buf)
{
    struct aggre_cfg_param      *param;
    int32                    ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != buf));

    param = (struct aggre_cfg_param *)buf;

    HWIFI_DEBUG("\naggre_cfg_param->ampdu_action_type = %d\n"
                "\naggre_cfg_param->TID                 = %d\n"
                "\naggre_cfg_param->max_num         = %d\n",
                param->ampdu_action_type, param->TID, param->max_num);

    HWIFI_DEBUG("Mac address:" MACFMT, MAC2STR(param->mac_addr));


    if(IS_STA(cfg))
    {
        if (!IS_CONNECTED(cfg))
        {
            HWIFI_WARNING("STATUS is not Connected, ba info is not supported");
            return -EFAIL;
        }
    }

    ret = hwifi_set_ampdu(cfg, param);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to set ampdu!");
        return ret;
    }


    return SUCC;
}


/*
 * Prototype    : wl_aggre_mode_set
 * Description  : auto aggreagtion mode set
 * Input        : struct cfg_struct *cfg,uint8 enabled
 * Output       : NONE
 * Return Value : int32
 * Calls        :
 * Called By    : hwifi_ioctl_cmd()
 *
 *   History        :
 *   1.Date         : 2013/03/19
 *     Author       : ywx164715
 *     Modification : Created function
 *
 */
int32 wl_aggre_mode_set(struct cfg_struct *cfg, uint8 enabled)
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
        return -ENOMEM;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    /* fill ps mode */
    ps = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(ps, WID_BA_CONTROL_SET, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send aggreagtion mode set msg!");
        return ret;
    }
    cfg->aggre_info.aggre_mode_enabled = enabled;

    HWIFI_DEBUG("Succeed to set aggreagtion mode msg!");

    return SUCC;
}


int32 wl_aggre_mode_lock_set(struct cfg_struct *cfg, uint8 enabled)
{
    int32 ret = FALSE;
    int32 ret_t = SUCC;
    spin_lock_bh(&cfg->aggre_info.lock);
    do
    {
        if(!enabled == cfg->aggre_info.aggr_start)
        {
            HWIFI_DEBUG("agree already %s", !enabled?"start":"stop");
            ret = TRUE;
            break;
        }
        ret_t = wl_aggre_mode_set(cfg, enabled);
        if (SUCC != ret_t)
        {
            ret = FALSE;
            HWIFI_WARNING("Failed to %s aggre!", !enabled?"start":"stop");
        }else{
            HWIFI_INFO("Sucuess to %s aggre", !enabled?"start":"stop");
            cfg->aggre_info.aggr_start = !enabled;
            ret = TRUE;
        }
    }while(0);
    spin_unlock_bh(&cfg->aggre_info.lock);
    return ret;
}


int32  hwifi_dyn_aggre_num_set(struct cfg_struct *cfg, struct host_ba_aggre_num_param *param)
{
    struct aggre_num_msg  *msg;
    struct sk_buff        *skb;
    uint8                 *checksum;
    uint16                 msg_size;
    int32                  ret;

    HWIFI_ASSERT((NULL != cfg) && (NULL != param));

    msg_size = sizeof(struct aggre_num_msg) + WID_FREQ_PLAN_FOR_AGGRE_NUM_LEN + 1;/* 1 for checksum*/
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

    msg = (struct aggre_num_msg *)skb_put(skb, msg_size);

    /* add msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);
    /*add uapsd wid */
    hwifi_fill_binary_wid(&msg->aggre_num, WID_FREQ_PLAN, WID_FREQ_PLAN_FOR_AGGRE_NUM_LEN, (uint8 *)param);

    /* checksum */
    checksum = (uint8 *)(msg + 1) + WID_FREQ_PLAN_FOR_AGGRE_NUM_LEN;
   *checksum = bytes_check_sum(msg->aggre_num.val, WID_FREQ_PLAN_FOR_AGGRE_NUM_LEN);

    ret = hwifi_send_cmd(cfg, skb);

    return ret;
}


/*
 * Prototype    : wl_aggre_cfg_set
 * Description  : config ba session
 * Input        : struct cfg_struct *cfg,uint8 *buf
 * Output       : NONE
 * Return Value : int32
 * Calls        :
 * Called By    : hwifi_ioctl_cmd()
 *
 *   History        :
 *   1.Date         : 2013/03/19
 *     Author       : ywx164715
 *     Modification : Created function
 *
 */
int32 wl_aggre_cfg_set(struct cfg_struct *cfg, struct host_ba_cfg_param *host_ba_cfg)
{
    struct sk_buff          *skb;
    struct set_aggre_cfg_msg *aggre_cfg_msg;
    uint16                   msg_size;
    int32                    ret;

    HWIFI_DEBUG("max_tx: %d\n"
                "max_rx: %d\n"
                "max_retry: %d\n",
                host_ba_cfg->max_tx,
                host_ba_cfg->max_rx,
                host_ba_cfg->max_retry);

    msg_size = sizeof(struct set_aggre_cfg_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    /* fill the msg header */
    aggre_cfg_msg = (struct set_aggre_cfg_msg*)skb_put(skb, sizeof(struct set_aggre_cfg_msg));
    hwifi_fill_msg_hdr(&aggre_cfg_msg->msg_header, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_aggre_cfg_wid(&aggre_cfg_msg->set_aggre_cfg, host_ba_cfg);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send ba ampdu set msg!");
        return -EFAIL;
    }

    memcpy(&cfg->aggre_info.host_ba_cfg, host_ba_cfg, sizeof(struct host_ba_cfg_param));

    HWIFI_DEBUG("Succeed to ba ampdu set msg");

    return SUCC;
}


/*
 * Prototype    : wl_query_ba_set
 * Description  : query ba session info
 * Input        : struct cfg_struct *cfg,struct query_ba_param *param
 * Output       : NONE
 * Return Value : int32
 * Calls        :
 * Called By    : hwifi_ioctl_cmd()
 *
 *   History        :
 *   1.Date         : 2013/03/19
 *     Author       : ywx164715
 *     Modification : Created function
 *
 */
int32 wl_query_ba_set(struct cfg_struct *cfg, struct query_ba_param *param)
{
    struct sk_buff          *skb;
    struct set_query_ba_msg *query_ba_msg;
    uint16                   msg_size;
    int32                    ret;

    HWIFI_DEBUG("Mac address:" MACFMT, MAC2STR(param->mac_addr));

    msg_size = sizeof(struct set_query_ba_msg);
    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -ENOMEM;
    }

    /* fill the msg header */
    query_ba_msg = (struct set_query_ba_msg*)skb_put(skb, sizeof(struct set_query_ba_msg));
    hwifi_fill_msg_hdr(&query_ba_msg->msg_header, HOST_CMD_CONFIG, msg_size);
    hwifi_fill_query_ba_wid(&query_ba_msg->set_query_ba, param);

    if (is_zero_mac_addr(param->mac_addr))
    {    /* use user data */
        memcpy(query_ba_msg->set_query_ba.bssid, param->mac_addr, MAC_LEN);
    }else if (IS_AP(cfg) && param->flag == 0){      /* AP mode */
        memcpy(query_ba_msg->set_query_ba.bssid, cfg->ap_info.sta_mgmt.stations[CONNECTED_STA_PRE].mac, MAC_LEN);
    }else if (IS_STA(cfg)){     /* STA mode copy mac address fill ba and ampdu struct*/
        memcpy(query_ba_msg->set_query_ba.bssid, cfg->conn.bss.bssid, MAC_LEN);
    }

    /* init for wait event */
    cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].id = 0xFE;

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send aggreagtion mode set msg!");
        return -EFAIL;
    }



    ret = wait_event_interruptible_timeout(cfg->wait_queue, (0xFE != cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].id), 5 * HZ);

    if (0 == ret)
    {
        HWIFI_WARNING("Wait_event_queue time out!");
        return -EFAIL;
    }
    else if (ret < 0)
    {
        HWIFI_WARNING("Wait_event_queue internal error,ret=%d", ret);
        return -EFAIL;
    }
    else if (cfg->aggre_info.ba_param_res.ba_sess_res[ZERO_INDEX].id == 0xFF)
    {
        HWIFI_WARNING("Cann't find the mac address or no any connected!");
        return -EFAIL;
    }

    memcpy((uint8 *)param, (uint8 *)&cfg->aggre_info.ba_param_res, sizeof(*param));

    HWIFI_DEBUG("Succeed to query ba session!");

    return SUCC;
}


/*
 * Prototype    : wl_aggre_cfg_get
 * Description  : get ba sessiones config info
 * Input        : struct cfg_struct *cfg, struct wifi_ioctl_data_struct *ioctl_data
 * Output       : NONE
 * Return Value : struct host_ba_cfg_param *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/23
 *     Author       : ywx164715
 *     Modification : Created function
 *
 */
 int32 wl_aggre_cfg_get(struct cfg_struct *cfg, struct host_ba_cfg_param *host_ba_param)
{
    HWIFI_ASSERT(cfg != NULL);

    host_ba_param->max_tx    = cfg->aggre_info.host_ba_cfg.max_tx;
    host_ba_param->max_rx    = cfg->aggre_info.host_ba_cfg.max_rx;
    host_ba_param->max_retry = cfg->aggre_info.host_ba_cfg.max_retry;

    return SUCC;
}


/*
 * Prototype    : wl_aggre_mode_get
 * Description  : get device aggregation mode
 * Input        : struct cfg_struct *cfg, struct wifi_ioctl_data_struct *ioctl_data
 * Output       : NONE
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/23
 *     Author       : ywx164715
 *     Modification : Created function
 *
 */
int32 wl_aggre_mode_get(struct cfg_struct *cfg, uint8 *aggre_mode)
{
    HWIFI_ASSERT(cfg != NULL);

    *aggre_mode = cfg->aggre_info.aggre_mode_enabled;

    return SUCC;
}

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
/*
 * Prototype    : wl_set_aggre_func
 * Description  :
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/25
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 wl_set_aggre_func(struct cfg_struct *cfg, uint8 enabled)
{
    int32 ret;
    HWIFI_ASSERT(NULL != cfg);

    if (FALSE == enabled)
    {
        cfg->aggre_info.aggre_func = FALSE;
        del_timer_sync(&cfg->aggre_info.data_rate_timer);

        /* stop aggre */
        ret = wl_aggre_mode_set(cfg, TRUE);
        if (ret != SUCC)
        {
            HWIFI_WARNING("Failed to stop aggre function!");
        }
        cfg->aggre_info.aggr_start = FALSE;
    }
    else
    {
        cfg->aggre_info.aggre_func = TRUE;
        mod_timer(&cfg->aggre_info.data_rate_timer, (jiffies + AGREE_DATE_TIMOUT*HZ));
    }

    cfg->aggre_info.rate = 0;
    cfg->aggre_info.short_frame_pps = 0;
    return SUCC;
}

/*
 * Prototype    : wl_get_aggre_func
 * Description  :
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/25
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 wl_get_aggre_func(struct cfg_struct *cfg, uint8 *aggre_func)
{
    HWIFI_ASSERT((cfg != NULL) && (aggre_func != NULL));
    *aggre_func = cfg->aggre_info.aggre_func;
    return SUCC;
}

/*
 * Prototype    : hwifi_start_timer
 * Description  :
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/25
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_start_timer(struct cfg_struct *cfg)
{
    HWIFI_ASSERT(NULL != cfg);
    if (TRUE == cfg->aggre_info.aggre_func)
    {
        cfg->aggre_info.aggr_start = FALSE;
        cfg->aggre_info.rate       = 0;
        cfg->aggre_info.short_frame_pps   = 0;
        mod_timer(&cfg->aggre_info.data_rate_timer, (jiffies + AGREE_DATE_TIMOUT*HZ));
    }
    else
    {
        HWIFI_INFO("Aggre fuction isn't started");
    }

    return SUCC;
}

/*
 * Prototype    : hwifi_stop_timer
 * Description  :
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/25
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_stop_timer(struct cfg_struct *cfg)
{
    int32 ret = SUCC;

    HWIFI_ASSERT(NULL != cfg);
    if (FALSE == cfg->aggre_info.aggre_func)
    {
        HWIFI_INFO("Aggre function isn't started");
        return SUCC;
    }

    del_timer_sync(&cfg->aggre_info.data_rate_timer);

    /* stop aggre */
    if (TRUE == cfg->aggre_info.aggr_start)
    {
        ret = wl_aggre_mode_set(cfg, TRUE);
    }

    if (-EFAIL == ret)
    {
        HWIFI_WARNING("Failed to stop aggre!");
    }

    cfg->aggre_info.aggr_start = FALSE;
    cfg->aggre_info.rate       = 0;
    cfg->aggre_info.short_frame_pps   = 0;

    return ret;
}


/*
 * Prototype    : hwifi_save_flow_info
 * Description  : struct skbuff *skb
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/13
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 hwifi_save_flow_info(struct sk_buff *skb, struct cfg_struct *cfg)
{
    HWIFI_ASSERT((NULL != cfg) && (NULL != skb));

    if ((TRUE == cfg->aggre_info.aggre_func)
        && (PACKET_BROADCAST != skb->pkt_type)
        && (PACKET_MULTICAST != skb->pkt_type))
    {
        cfg->aggre_info.rate += skb->len;
        if (skb->len < WLAN_SHORT_PACKET_LEN)
        {
			cfg->aggre_info.short_frame_pps++;
        }
    }
    return SUCC;
}

void hwifi_prepare_aggr_parm(struct host_ba_aggre_num_param *ba_param, int32 max_aggr_num, uint8 method)
{
    //ba_param->max_tx    = 2;
    //ba_param->max_rx    = 2;
    //ba_param->max_retry = max_aggr_num;

    /* 0xFD is used for aggre number 16 vs 8 */
    ba_param->aggre_set_tag = 0xFD;
    ba_param->max_aggre_num = max_aggr_num / 2;
    ba_param->method        = method;
}

/*
 * Prototype    : hwifi_data_rate_timeout
 * Description  : count data rate
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/14
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
STATIC void hwifi_data_rate_timeout(unsigned long data)
{
    struct cfg_struct *cfg;
    struct timer_list *_timer;
    struct host_ba_aggre_num_param ba_param;
    /*struct scanned_bss_info     *bss;
    uint8                       *probe_resp;
    uint32                       probe_resp_len;
    uint8                       *htc_ie;*/
    uint8                       ht_40M_flag;

    cfg = (struct cfg_struct *)data;

    if(IS_STA(cfg))
    {
        if ((cfg->aggre_info.rate <= g_data_rate_down) && (cfg->aggre_info.short_frame_pps) <= g_data_rate_pps_down)
        {
            HWIFI_DEBUG("Rate Slow Down,Try to Stop aggre");
             /* stop aggre by device */
            wl_aggre_mode_lock_set(cfg, TRUE);
        }
        else if ((cfg->aggre_info.rate > g_data_rate_up) || (cfg->aggre_info.short_frame_pps) >= g_data_rate_pps_up)
        {
            if(hcc_net_streams_connected(cfg->hi110x_dev->hcc))
            {
                HWIFI_DEBUG("Can not start agree yet, some special stream connected.");
            } else{
            	HWIFI_DEBUG("Rate Go Up,Try to Start aggre");
            	/* start aggre by device */
            	wl_aggre_mode_lock_set(cfg, FALSE);
            }
        }
    }
    else if(IS_AP(cfg))
    {
        if (cfg->aggre_info.rate <= g_ap_data_rate_down)
        {
            HWIFI_DEBUG("Rate Slow Down,Try to Stop aggre");

            /* stop aggre by device */
            wl_aggre_mode_lock_set(cfg, TRUE);
        }
        else if(cfg->aggre_info.rate > g_ap_data_rate_up)
        {
            if(hcc_net_streams_connected(cfg->hi110x_dev->hcc))
            {
                HWIFI_DEBUG("Can not start agree yet, some special stream connected.");
            } else{
                HWIFI_DEBUG("Rate Go Up,Try to Start aggre");
                /* start aggre by device */
                wl_aggre_mode_lock_set(cfg, FALSE);
            }
        }
    }

    HWIFI_DEBUG("g_use_dync_16vs8:%u",g_use_dync_16vs8);
    if((cfg->aggre_info.rate > g_data_rate_up) && (IS_STA(cfg)) && (g_use_dync_16vs8))
    {
        ht_40M_flag = cfg->rate.flags & RATE_INFO_FLAGS_40_MHZ_WIDTH;

        /* 说明支持40M信道 */
        if (ht_40M_flag && cfg->sta_info.fc_enabled)
        {
            g_aggr_num_rate_up    = (M40M_AGGR_NUM_RATE_UP * 1000000 / 8)*10;
            g_aggr_num_rate_down  = (M40M_AGGR_NUM_RATE_DOWN * 1000000 / 8)*10;
        }
        else
        {
            g_aggr_num_rate_up    = (M20M_AGGR_NUM_RATE_UP * 1000000 / 8)*10;
            g_aggr_num_rate_down  = (M20M_AGGR_NUM_RATE_DOWN * 1000000 / 8)*10;
        }

        /* 按10s统计 */
        if(g_count_aggre < 10)
        {
            g_count_aggre++;
            g_rate_sum += cfg->aggre_info.rate;

            cfg->aggre_info.rate = 0;
            cfg->aggre_info.short_frame_pps = 0;
            _timer = &cfg->aggre_info.data_rate_timer;
            mod_timer(_timer, (jiffies + AGREE_DATE_TIMOUT*HZ));

            return;
        }

        // HWIFI_INFO("rate:%lu, max_aggre_num:%u",cfg->aggre_info.rate,cfg->aggre_info.host_ba_cfg.max_retry);
        HWIFI_DEBUG("rate:%lu, max_aggre_num:%u",g_rate_sum,cfg->aggre_info.host_ba_cfg.max_retry);
        HWIFI_DEBUG("g_aggr_num_rate_down:%lu, g_aggr_num_rate_up:%lu",g_aggr_num_rate_down,g_aggr_num_rate_up);
        if ((g_rate_sum < g_aggr_num_rate_down) && (MAX_AGGR_NUM_16 != cfg->aggre_info.host_ba_cfg.max_retry))
        {
            // set aggr num 16
            hwifi_prepare_aggr_parm(&ba_param, MAX_AGGR_NUM_16, g_method);
            // wl_aggre_cfg_set(cfg, &ba_param);
            hwifi_dyn_aggre_num_set(cfg, &ba_param);
            cfg->aggre_info.host_ba_cfg.max_retry = MAX_AGGR_NUM_16;

            HWIFI_DEBUG("send max aggre num:%d",MAX_AGGR_NUM_16);
        }
        else if ((g_rate_sum > g_aggr_num_rate_up) && (MAX_AGGR_NUM_8 != cfg->aggre_info.host_ba_cfg.max_retry))
        {
            // set aggr num 8
            hwifi_prepare_aggr_parm(&ba_param, MAX_AGGR_NUM_8, g_method);
            hwifi_dyn_aggre_num_set(cfg, &ba_param);
            cfg->aggre_info.host_ba_cfg.max_retry = MAX_AGGR_NUM_8;

            HWIFI_DEBUG("send max aggre num:%d",MAX_AGGR_NUM_8);
        }

        g_rate_sum    = 0;
        g_count_aggre = 0;

    }

    cfg->aggre_info.rate = 0;
    cfg->aggre_info.short_frame_pps = 0;
    _timer = &cfg->aggre_info.data_rate_timer;

    mod_timer(_timer, (jiffies + AGREE_DATE_TIMOUT*HZ));
}

/*
 * Prototype    : hwifi_aggr_init
 * Description  : init aggr param
 * Input        : None
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/14
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
void hwifi_aggr_init(struct cfg_struct *cfg)
{
    spin_lock_init(&cfg->aggre_info.lock);
    init_timer(&cfg->aggre_info.data_rate_timer);

    cfg->aggre_info.rate = 0;
    cfg->aggre_info.short_frame_pps = 0;

    cfg->aggre_info.data_rate_timer.function = (void *)hwifi_data_rate_timeout;
    cfg->aggre_info.data_rate_timer.data     = (unsigned long) cfg;

    cfg->aggre_info.aggr_start = FALSE;
    cfg->aggre_info.aggre_func = TRUE;

    cfg->aggre_info.host_ba_cfg.max_rx    = g_ba_max_rx;
    cfg->aggre_info.host_ba_cfg.max_tx    = g_ba_max_tx;
    cfg->aggre_info.host_ba_cfg.max_retry = g_ba_max_retry;
}


#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

