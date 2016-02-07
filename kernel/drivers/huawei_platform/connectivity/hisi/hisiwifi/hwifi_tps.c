



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include <linux/module.h>
#include <linux/netdevice.h>

#include "hwifi_tps.h"
#include "cfg80211_stru.h"
#include "hwifi_cfg80211.h"
#include "hwifi_wpa_ioctl.h" /* for wl_pwrm_set */
#include "hwifi_wl_config_ioctl.h"


#include <net/cfg80211.h>      /* wdev_priv */
#include <linux/etherdevice.h> /* eth_type_trans */
#include "hwifi_utils.h"
#include "hwifi_hcc.h"
#include "hwifi_netdev.h"
#include "hwifi_cfgapi.h"






/*
 * 2 Global Variable Definition
 */

/*
 * 3 Function Definition
 */


/*****************************************************************************
 函 数 名  : hwifi_test_freq_set
 功能描述  : 设置wifi频段
 输入参数  : struct cfg_struct *cfg, uint8  enabled
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWX160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_test_freq_set(struct cfg_struct *cfg, int32  freq)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *wid_freq;
    uint16                   msg_size;
    int32                    ret;

    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Current connected status not support wifreq param setting.");
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
    wid_freq = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(wid_freq, WID_PRIMARY_CHANNEL , freq);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send wifi freq set msg!");
        return -EFAIL;
    }

    cfg->ap_info.curr_channel=freq;

    HWIFI_DEBUG("Succeed to set wifreq: %d", freq);

    return SUCC;
}

/*****************************************************************************
 函 数 名  : hwifi_test_userpow_set
 功能描述  : 设置WIFI功率
 输入参数  : struct cfg_struct *cfg, uint8  enabled
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWX160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_test_userpow_set(struct cfg_struct *cfg, int32  userpow)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *wid_userpow;
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
    wid_userpow = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(wid_userpow, WID_USER_CONTROL_ON_TX_POWER , (uint8)userpow);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send wifi userpow set msg!");
        return -EFAIL;
    }

    cfg->hi110x_dev->tps->userpow=userpow;

    HWIFI_DEBUG("succeed to set wiuserpow: %d", userpow);

    return SUCC;
}
/*****************************************************************************
 函 数 名  : hwifi_test_userpow_get
 功能描述  : 获取WIFI功率
 输入参数  : struct cfg_struct *cfg
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWS160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_test_userpow_get(struct cfg_struct *cfg)
{
    return cfg->hi110x_dev->tps->userpow;
}
/*****************************************************************************
 函 数 名  : hwifi_test_pow_set
 功能描述  : 设定WIFI指定功率发送
 输入参数  : struct cfg_struct *cfg, uint8  enabled
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWX160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_test_pow_set(struct cfg_struct *cfg, int32  pow)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *wid_pow;
    uint16                   msg_size;
    int32                    ret;
    int32  n_channels_2G = 1;
    int32  n_channels_5G = 0;

    if (HWIFI_CONNECTED == cfg->conn.status)
    {
        if (cfg->conn.bss.freq <= HWIFI_AT_TEST_MAX_FREQ_2G)
        {
            HWIFI_INFO("scan: 2.4G connected scan, only scan 2.4G");
            n_channels_5G = 0;
            n_channels_2G = 1;
        }
        else
        {
            HWIFI_INFO("scan: 5G connected scan, only scan 5G");
            n_channels_5G = 1;
            n_channels_2G = 0;
        }
    }
    else if(IS_AP(cfg))
    {
        if(cfg->ap_info.channel_info & HWIFI_AT_TEST_5G_BAND)
        {
            HWIFI_INFO("ap operation on 5G, only scan 5G");
            n_channels_5G = 1;
            n_channels_2G = 0;
        }
        else
        {
            HWIFI_INFO("ap operation on 2.4G, only scan 2.4G");
            n_channels_5G = 0;
            n_channels_2G = 1;
        }
    }

    if(n_channels_5G > 0)
    {
       if(pow > 180 || pow <0)
       {
           HWIFI_WARNING("can not set the pow value %d",pow);
           return -EFAIL;
       }
    }
    else if(n_channels_2G > 0)
    {
        if(pow > 200 || pow <0)
        {
           HWIFI_WARNING("can not set the pow value %d",pow);
           return -EFAIL;
        }
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
    wid_pow = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(wid_pow, WID_CURRENT_TX_POW , (uint16)pow);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("fail to send wifi pow set msg");
        return -EFAIL;
    }

    cfg->hi110x_dev->tps->pow=pow;

    HWIFI_INFO("succeed to send wifi pow set msg %d",pow);

    return SUCC;
}
/*****************************************************************************
 函 数 名  : hwifi_test_pow_get
 功能描述  : 获取WIFI指定功率
 输入参数  :struct cfg_struct *cfg
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWX160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_test_pow_get(struct cfg_struct *cfg)
{
    return cfg->hi110x_dev->tps->pow;
}

/*****************************************************************************
 函 数 名  : hwifi_oltpc_active_set
 功能描述  : 设置WiFi 当前的功率调整信息读取指令
 输入参数  : struct cfg_struct *cfg, uint8  enabled
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWX160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_oltpc_active_set(struct cfg_struct *cfg, int32  enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *wid_oltpc;
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
    wid_oltpc = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(wid_oltpc, WID_OLTPC_ACTIVE , (uint8)enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("fail to send wifi oltpc active set msg");
        return -EFAIL;
    }

    cfg->hi110x_dev->tps->oltpc_active=enabled;

    HWIFI_DEBUG("succeed to send wifi oltpc active set msg");

    return SUCC;
}
/*****************************************************************************
 函 数 名  : hwifi_oltpc_active_get
 功能描述  : 获取WiFi 当前的功率调整信息读取指令
 输入参数  :struct cfg_struct *cfg
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWX160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_oltpc_active_get(struct cfg_struct *cfg)
{
    return cfg->hi110x_dev->tps->oltpc_active;
}


/*****************************************************************************
 函 数 名  : hwifi_oltpc_switch_set
 功能描述  : 设置WiFi 当前的功率调整信息读取指令
 输入参数  : struct cfg_struct *cfg, uint8  enabled
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWX160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_oltpc_switch_set(struct cfg_struct *cfg, int32  enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *wid_oltpc;
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
    wid_oltpc = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(wid_oltpc, WID_OLTPC_SWITCH , (uint8)enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send wifi olptc active set msg!");
        return -EFAIL;
    }

    cfg->hi110x_dev->tps->oltpc_switch=enabled;

    HWIFI_DEBUG("succeed to send wifi oltpc switch set msg");

    return SUCC;
}
/*****************************************************************************
 函 数 名  : hwifi_oltpc_switch_get
 功能描述  : 获取WiFi 当前的功率调整信息读取指令
 输入参数  :struct cfg_struct *cfg
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月12日
    作    者   : hWX160629
    修改内容   : 新生成函数

*****************************************************************************/
int32  hwifi_oltpc_switch_get(struct cfg_struct *cfg)
{
    return cfg->hi110x_dev->tps->oltpc_switch;
}

/*
 * Prototype    : hwifi_test_mode_set
 * Description  : set burst rx/tx mode
 * Input        : struct cfg_struct *cfg, uint8  enabled
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/5/3
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  hwifi_test_mode_set(struct cfg_struct *cfg, uint8  mode)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *test_mode;
    uint16                   msg_size;
    int32                    ret;

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct char_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    if(!IS_STA(cfg) && !IS_AP(cfg))
    {
        HWIFI_WARNING("Current status can not support burst tx/rx mode set.");
        dev_kfree_skb_any(skb);
        return -EFAIL;
    }
    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_MODE, msg_size);

    /* fill ps mode */
    test_mode = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(test_mode, WID_MODE_CHANGE, mode);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send mode set msg!");
        return -EFAIL;
    }

    cfg->hi110x_dev->tps->mode=mode;

    HWIFI_DEBUG("Succeed to set mode param :%d", mode);

    return SUCC;
}
/*
 * Prototype    : hwifi_test_mode_get
 * Description  : get the setting of mode param
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/5/3
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  hwifi_test_mode_get(struct cfg_struct *cfg)
{
    return cfg->hi110x_dev->tps->mode;
}

/*
 * Prototype    : hwifi_test_datarate_set
 * Description  : set rate
 * Input        : struct cfg_struct *cfg,
 *                uint8 enabled
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  hwifi_test_datarate_set(struct cfg_struct *cfg, uint8  rate)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *rate_set;
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
    rate_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(rate_set, WID_CURRENT_TX_RATE, rate);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("fail to send rate set msg");
        return -EFAIL;
    }

    HWIFI_INFO("succeed to send rate set msg %d",rate);

    return SUCC;
}
/*
 * Prototype    : hwifi_band_set
 * Description  : enable/disable support for 40MHz operation
 * Input        : struct cfg_struct *cfg,uint8 enabled
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/26
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  hwifi_band_set(struct cfg_struct *cfg,uint8 band)
{
    int32                    ret;

    ret = hwifi_sta_2040_enable_ctrl_set(cfg,band);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send band param set msg!");
       return -EFAIL;
    }

    HWIFI_INFO("Succeed to set band param:%d", band);

    return SUCC;
}

/*
 * Prototype    : wifitest_protocol_gmode_set
 * Description  : set 11g operating mode
 * Input        : struct cfg_struct *cfg,uint8 enabled
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/26
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  wifitest_protocol_gmode_set(struct cfg_struct *cfg,uint8 mode)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *gmode;
    uint16                   msg_size;
    int32                    ret;

    if(IS_CONNECTED(cfg)||(IS_P2P_ON(cfg)))
    {
        HWIFI_WARNING("current status can not support protocol gmode set.");
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

    /* fill gmode */
    gmode = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(gmode, WID_11G_OPERATING_MODE, mode);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("fail to send protocol gmode set msg");
       return -EFAIL;
    }
    cfg->sta_info.gmode= mode;

    HWIFI_INFO("succeed to set protocol gmode:%d", mode);

    return SUCC;
}
/*
 * Prototype    : wifitest_protocol_nmode_set
 * Description  : set ht capability enabled
 * Input        : struct cfg_struct *cfg,uint8 enabled
 *                uint8 enabled
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32 wifitest_protocol_nmode_set(struct cfg_struct *cfg,uint8 mode)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *ht;
    uint16                   msg_size;
    int32                    ret;
    if (IS_CONNECTED(cfg) || IS_AP(cfg))
    {
        HWIFI_WARNING("current status can not support 11n mode set.");
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
    ht = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(ht, WID_11N_ENABLE, mode);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("fail to send 11n mode set msg");
        return -EFAIL;
    }

    HWIFI_INFO("succeed to send 11n mode set msg %d",mode);

    return SUCC;
}

/*
 * Prototype    : hwifi_dbb_get
 * Description  : get dbb of wifi
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/11/9
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32 hwifi_dbb_get(struct cfg_struct *cfg,int8 *dbb)
{
    int32 ret;

    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return -EFAIL;
    }

    cfg->hi110x_dev->tps->dbb_ver_got = 0xFF;

    ret = wl_get_dbb_info(cfg);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to get DBB number!");
        return -EFAIL;
    }

    ret = wait_event_interruptible_timeout(cfg->wait_queue, (0xFF != cfg->hi110x_dev->tps->dbb_ver_got), 5 * HZ);
    if (0 == ret)
    {
            HWIFI_WARNING("wait for dbb version message time out(5s)!");
            return -EFAIL;
    }
    else if (ret < 0)
    {
            HWIFI_WARNING("wait for dbb version message error!");
            return -EFAIL;
    }

    strncpy(dbb,cfg->hi110x_dev->tps->dbb,HISI_WIFI_DBB_LEN);

    HWIFI_DEBUG("DBB number is %s",cfg->hi110x_dev->tps->dbb);

    return SUCC;

}



/*
 * Prototype    : hwifi_upc_get
 * Description  : get upc of wifi
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/4/25
 *     Author       : zhanglu/z00217036
 *     Modification : Created function
 *
 */
int32 hwifi_upc_get(struct cfg_struct *cfg)
{
    int32 ret;

    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return -EFAIL;
    }

    cfg->hi110x_dev->tps->check_upc_ctrl = -EFAIL;

    ret = wl_get_upc_info(cfg);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to get upc!");
        return -EFAIL;
    }

    ret = wait_event_interruptible_timeout(cfg->wait_queue, (-EFAIL != cfg->hi110x_dev->tps->check_upc_ctrl), 5 * HZ);
    if (0 == ret)
    {
            HWIFI_WARNING("wait for upc info message time out(5s)!");
            return -EFAIL;
    }
    else if (ret < 0)
    {
            HWIFI_WARNING("wait for upc info message error!");
            return -EFAIL;
    }

    HWIFI_DEBUG("report upc info is %d",cfg->hi110x_dev->tps->check_upc_flag);

    return cfg->hi110x_dev->tps->check_upc_flag;


}

/*
 * Prototype    : hwifi_gen_cw_single_tone_set
 * Description  : set gen_cw_single_tone
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/10/11
 *     Author       : zhanglu/z00217036
 *     Modification : Created function
 *
 */
int32 hwifi_gen_cw_single_tone_set(struct cfg_struct *cfg)
{
    int32                                ret;
    uint16                               msg_size;
    struct sk_buff                      *skb;
    struct hwifi_gen_cw_single_tone_msg *msg;


    HWIFI_ASSERT((NULL != cfg));

    msg_size = sizeof(struct hwifi_gen_cw_single_tone_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    msg = (struct hwifi_gen_cw_single_tone_msg *)skb_put(skb, msg_size);

    /* set the msg header */
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    hwifi_fill_char_wid(&msg->phy_active_reg_1, WID_PHY_ACTIVE_REG, WID_SIGNAL_TONE_PHY_ACTIVE_REG_1);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_1, WID_11N_PHY_ACTIVE_REG_VAL, WID_SIGNAL_TONE_ACTIVE_REG_VAL_1);
    hwifi_fill_char_wid(&msg->phy_active_reg_2, WID_PHY_ACTIVE_REG, WID_SIGNAL_TONE_PHY_ACTIVE_REG_2);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_2,WID_11N_PHY_ACTIVE_REG_VAL,WID_SIGNAL_TONE_ACTIVE_REG_VAL_2);
    hwifi_fill_char_wid(&msg->phy_active_reg_3, WID_PHY_ACTIVE_REG, WID_SIGNAL_TONE_PHY_ACTIVE_REG_3);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_3,WID_11N_PHY_ACTIVE_REG_VAL,WID_SIGNAL_TONE_ACTIVE_REG_VAL_3);
    hwifi_fill_char_wid(&msg->phy_active_reg_4, WID_PHY_ACTIVE_REG, WID_SIGNAL_TONE_PHY_ACTIVE_REG_4);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_4,WID_11N_PHY_ACTIVE_REG_VAL,WID_SIGNAL_TONE_ACTIVE_REG_VAL_4);

    hwifi_fill_char_wid(&msg->phy_active_reg_5, WID_PHY_ACTIVE_REG, WID_SIGNAL_TONE_PHY_ACTIVE_REG_5);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_5,WID_11N_PHY_ACTIVE_REG_VAL,WID_SIGNAL_TONE_ACTIVE_REG_VAL_5);
    hwifi_fill_char_wid(&msg->phy_active_reg_6, WID_PHY_ACTIVE_REG, WID_SIGNAL_TONE_PHY_ACTIVE_REG_6);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_6,WID_11N_PHY_ACTIVE_REG_VAL,WID_SIGNAL_TONE_ACTIVE_REG_VAL_6);
    hwifi_fill_char_wid(&msg->phy_active_reg_7, WID_PHY_ACTIVE_REG, WID_SIGNAL_TONE_PHY_ACTIVE_REG_7);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_7,WID_11N_PHY_ACTIVE_REG_VAL,WID_SIGNAL_TONE_ACTIVE_REG_VAL_7);
    hwifi_fill_char_wid(&msg->phy_active_reg_8, WID_PHY_ACTIVE_REG, WID_SIGNAL_TONE_PHY_ACTIVE_REG_8);
    hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_8,WID_11N_PHY_ACTIVE_REG_VAL,WID_SIGNAL_TONE_ACTIVE_REG_VAL_8);

    hwifi_fill_int_wid(&msg->rf_reg_info, WID_RF_REG_VAL, WID_SIGNAL_TONE_RF_REG_INFO);

    ret = hwifi_send_cmd(cfg, skb);

    PRINT_SEND_CMD_RET("connect status,gen_cw_single_tone param set success", ret);

    return ret;

}

int32 hwifi_tps_ioctl_cmd(struct hi110x_device* hi110x_dev, struct ifreq *ifr, int32 cmd)
{
    wifi_ioctl_test_data_struct ioctl_data;
    int32                       ret = SUCC;

    if ((NULL == hi110x_dev) || (NULL == ifr) || (NULL == ifr->ifr_data))
    {
        HWIFI_WARNING("Invalid NULL params!");
        return -EFAIL;
    }

    HWIFI_PRINT_ONCE(INFO, "sizeof wifi_ioctl_test_data_struct is %zu", sizeof(wifi_ioctl_test_data_struct));

    if(copy_from_user(&ioctl_data,ifr->ifr_data,sizeof(wifi_ioctl_test_data_struct)))
    {
        HWIFI_WARNING("Failed to copy ifr->ifr_data from user!");
        return -EFAIL;
    }

    if(ioctl_data.verify != VERIFY_CODE)
    {
        HWIFI_WARNING("ioctl verify failed,verify code is:%d(not equal %d)", ioctl_data.verify, VERIFY_CODE);
        return -EFAIL;
    }

    switch(ioctl_data.cmd)
    {
        case HWIFI_IOCTL_CMD_WI_FREQ_SET:
             ret = hwifi_test_freq_set(hi110x_dev->cfg,ioctl_data.pri_data.freq);
             break;
        case HWIFI_IOCTL_CMD_WI_USERPOW_SET:
             ret = hwifi_test_userpow_set(hi110x_dev->cfg,ioctl_data.pri_data.userpow);
             break;
        case HWIFI_IOCTL_CMD_WI_USERPOW_GET:
             ioctl_data.pri_data.userpow = hwifi_test_userpow_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_ioctl_test_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ioctl_data to user !");
                ret = -EFAIL;
             }
             break;
        case HWIFI_IOCTL_CMD_WI_POWER_SET:
             ret = hwifi_test_pow_set(hi110x_dev->cfg,ioctl_data.pri_data.pow);
             break;
        case HWIFI_IOCTL_CMD_WI_POWER_GET:
             ioctl_data.pri_data.pow = hwifi_test_pow_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_ioctl_test_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ioctl_data to user !");
                ret = -EFAIL;
             }
             break;
        case  HWIFI_IOCTL_OLTPC_ACTIVE_SET:
              ret = hwifi_oltpc_active_set(hi110x_dev->cfg,ioctl_data.pri_data.oltpc_active);
              break;
        case  HWIFI_IOCTL_OLTPC_SWITCH_SET:
              ret = hwifi_oltpc_switch_set(hi110x_dev->cfg,ioctl_data.pri_data.oltpc_switch);
              break;
        case  HWIFI_IOCTL_OLTPC_ACTIVE_GET:
              ioctl_data.pri_data.oltpc_active=hwifi_oltpc_active_get(hi110x_dev->cfg);
              if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_ioctl_test_data_struct)))
              {
                 HWIFI_WARNING("Failed to copy ioctl_data to user !");
                 ret = -EFAIL;
              }
              break;
        case  HWIFI_IOCTL_OLTPC_SWITCH_GET:
              ioctl_data.pri_data.oltpc_switch=hwifi_oltpc_switch_get(hi110x_dev->cfg);
              if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_ioctl_test_data_struct)))
              {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
              }
              break;
        case HWIFI_IOCTL_CMD_MODE_SET:
             ret = hwifi_test_mode_set(hi110x_dev->cfg,ioctl_data.pri_data.mode);
             break;
        case HWIFI_IOCTL_CMD_MODE_GET:
             ioctl_data.pri_data.mode=hwifi_test_mode_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_ioctl_test_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy ioctl_data to user !");
                 ret = -EFAIL;
             }
             break;
        case HWIFI_IOCTL_CMD_DATARATE_SET:
             ret = hwifi_test_datarate_set(hi110x_dev->cfg,(uint8)ioctl_data.pri_data.datarate);
             break;
        case HWIFI_IOCTL_CMD_BAND_SET:
             ret = hwifi_band_set(hi110x_dev->cfg,ioctl_data.pri_data.band);
             break;
        case HWIFI_IOCTL_CMD_PROTOCOL_GMODE_SET:
             ret = wifitest_protocol_gmode_set(hi110x_dev->cfg,ioctl_data.pri_data.protocol_gmode);
             break;
        case HWIFI_IOCTL_CMD_PROTOCOL_NMODE_SET:
             ret = wifitest_protocol_nmode_set(hi110x_dev->cfg,ioctl_data.pri_data.protocol_nmode);
             break;
        case HWIFI_IOCTL_CMD_DBB_GET:
             ret = hwifi_dbb_get(hi110x_dev->cfg,ioctl_data.pri_data.dbb);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_ioctl_test_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy ioctl_data to user !");
                 ret = -EFAIL;
             }
             break;
        case HWIFI_IOCTL_CMD_UPC_GET:
             ioctl_data.pri_data.check_upc_flag = hwifi_upc_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_ioctl_test_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy ioctl_data to user !");
                 ret = -EFAIL;
             }
             break;
        case HWIFI_IOCTL_CMD_GEN_CW_SINGLE_TONE_SET:
             ret = hwifi_gen_cw_single_tone_set(hi110x_dev->cfg);
             break;
        default:
            HWIFI_WARNING("Invalid not support ioctl_data.cmd(%d)",ioctl_data.cmd);
            ret = -EFAIL;
            break;
    }

    return ret;

}













#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

