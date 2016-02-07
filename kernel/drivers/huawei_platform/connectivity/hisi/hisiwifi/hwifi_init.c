


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */

#ifdef __ROAM__
#include <linux/kernel.h>

#include "hwifi_roam_alg.h"
#endif

#include "hwifi_msg.h"
#include "hwifi_cfg80211.h"
#include "hwifi_regdomain.h"
#include "hwifi_regdb.h"

#include "hwifi_init.h"
#include "hwifi_config.h"

/*
 * 2 Global Variable Definition
 */
#ifdef __ROAM__
extern struct hwifi_freq_map FREQ_2G_MAP[];
extern struct hwifi_freq_map FREQ_5G_MAP[];
extern uint32 SIZE_FREQ_2G_MAP;
extern uint32 SIZE_FREQ_5G_MAP;
#endif

extern int8 g_lte_wifi_coex;

extern uint8 g_sta_2040m_enabled;
extern uint8 g_ap_2040m_enabled;

/*
 * 3 Function Definition
 */


 int32 hwifi_init_get_log_module_number(struct cfg_struct *cfg)
 {
     int log_number=0;
     int i;

     for(i=0;i < WIFI_DEVICE_LOG_MAX;i++)
     {
         if(cfg->device_log.device_log_flag[i] != WIFI_DEVICE_LOG_INVALID )
         {
             log_number++;
         }
     }

     return log_number;
 }


 int32 hwifi_init_fill_device_log_msg(struct cfg_struct *cfg, struct sk_buff  *skb)
 {
     struct char_wid     *log_char_wid;
     int i;
     int32                device_log_number;

     device_log_number = hwifi_init_get_log_module_number(cfg);
     if( skb_tailroom(skb) < ( device_log_number *sizeof(struct char_wid) ) )
     {
          HWIFI_WARNING("init device log msg fail, left size:%d,need size=%zu",\
                       skb_tailroom(skb),device_log_number *sizeof(struct char_wid));
          return -EFAIL;
     }

     for(i=0; i< WIFI_DEVICE_LOG_MAX;i++)
     {
         if( WIFI_DEVICE_LOG_ON == cfg->device_log.device_log_flag[i] )
         {
             log_char_wid = (struct char_wid *)skb_tail_pointer(skb);
             hwifi_fill_char_wid(log_char_wid, WID_DFX_LOG_ON,i);
             skb_put(skb, sizeof(struct char_wid));
         }else if( WIFI_DEVICE_LOG_OFF == cfg->device_log.device_log_flag[i] )
         {
             log_char_wid = (struct char_wid *)skb_tail_pointer(skb);
             hwifi_fill_char_wid(log_char_wid,WID_DFX_LOG_OFF, i);
             skb_put(skb, sizeof(struct char_wid));
         }

     }

     return (device_log_number *sizeof(struct char_wid));
 }




 int32 hwifi_init_get_currttxpow_size(struct cfg_struct *cfg)
 {
     int32 number = 0;

     if( WIFI_CURRT_TX_POW_INVALID !=  cfg->sta_info.currenttxpow )
     {
         number += sizeof(struct char_wid);
     }

     return number;
 }


 int32 hwifi_init_fill_currttxpow_msg(struct cfg_struct *cfg, struct sk_buff  *skb)
 {
     struct char_wid     *char_wid_params;
     int32                currttxpow_sizes;

     currttxpow_sizes     = hwifi_init_get_currttxpow_size(cfg);
     if( skb_tailroom(skb) < currttxpow_sizes )
     {
          HWIFI_DEBUG("init currttxpow msg fail,left size:%d,need size=%d",\
                       skb_tailroom(skb),currttxpow_sizes);
          return -EFAIL;
     }

     if( WIFI_CURRT_TX_POW_INVALID !=  cfg->sta_info.currenttxpow )
     {

         char_wid_params = (struct char_wid *)skb_tail_pointer(skb);
         hwifi_fill_char_wid(char_wid_params, WID_CURRENT_TX_POW,cfg->sta_info.currenttxpow);
         skb_put(skb, sizeof(struct char_wid));
     }

     return currttxpow_sizes;
 }

static int32 hwifi_init_fill_ba_session_msg(struct sk_buff *skb)
{
     int32                               ba_param;
     struct host_ba_cfg_param            ba_cfg_param;
     struct int_wid                     *ba_session_param;

     if( skb_tailroom(skb) < sizeof(struct int_wid) )
     {
         HWIFI_DEBUG("init ba session msg fail,left size:%d,need size=%zu",\
                       skb_tailroom(skb),sizeof(struct int_wid));
         return -EFAIL;
     }

     ba_session_param =(struct int_wid *)skb_tail_pointer(skb);
     /*WID_11N_BA_AMPDU_PARAM*/
     ba_cfg_param.max_tx     = g_ba_max_tx;
     ba_cfg_param.max_rx     = g_ba_max_rx;
     ba_cfg_param.max_retry  = g_ba_max_retry;
     ba_param                = ba_cfg_param.max_tx;
     ba_param               |= (ba_cfg_param.max_rx << 8);
     ba_param               |= (ba_cfg_param.max_retry << 16);

     hwifi_fill_int_wid(ba_session_param, WID_11N_BA_AMPDU_PARAM, ba_param);
     skb_put(skb, sizeof(struct int_wid));
     return  sizeof(struct int_wid);

}

static int32 hwifi_init_fill_char_wid(struct sk_buff *skb,uint16 wid_value,uint8 value)
{
    struct char_wid     *param_char_wid;

    if( skb_tailroom(skb) < sizeof(struct char_wid))
    {
         HWIFI_WARNING("init ba session msg fail,no more space, left size:%d,need size=%zu",\
                       skb_tailroom(skb), sizeof(struct char_wid));

         return -EFAIL;
    }

    param_char_wid = (struct char_wid *)skb_tail_pointer(skb);
    hwifi_fill_char_wid(param_char_wid, wid_value,value);
    skb_put(skb, sizeof(struct char_wid));

    return sizeof(struct char_wid);
}

static int32 hwifi_init_fill_short_wid(struct sk_buff *skb,uint16 wid_value,uint16 value)
{
    struct short_wid     *param_short_wid;

    if( skb_tailroom(skb) < sizeof(struct short_wid))
    {
         HWIFI_WARNING("init ba session msg fail,no more space, left size:%d,need size=%zu",\
                       skb_tailroom(skb), sizeof(struct short_wid));

         return -EFAIL;
    }

    param_short_wid = (struct short_wid *)skb_tail_pointer(skb);
    hwifi_fill_short_wid(param_short_wid, wid_value,value);
    skb_put(skb, sizeof(struct short_wid));

    return sizeof(struct short_wid);
}

static int32 hwifi_init_fill_mac_msg(struct cfg_struct *cfg, struct sk_buff *skb)
{
    struct string_wid  *mac_addr_wid;
    int8                mac_addr[ETH_ALEN];
#ifdef INI_CONFIG
    int32               mac_read_flag;
#endif

    if( skb_tailroom(skb) < (sizeof(struct string_wid) + MAC_LEN))
    {
         HWIFI_WARNING("init mac msg fail,left size:%d,need size=%zu",\
                       skb_tailroom(skb),sizeof(struct string_wid)+MAC_LEN);;
         return -EFAIL;
    }
#ifdef INI_CONFIG
    mac_read_flag = hwifi_get_mac_addr(mac_addr);
    if (SUCC != mac_read_flag)
    {
        HWIFI_WARNING("failed to get mac address");
    }
#endif

    mac_addr_wid = (struct string_wid*)skb_tail_pointer(skb);
    hwifi_fill_string_wid(mac_addr_wid, WID_MAC_ADDR, MAC_LEN, cfg->ndev->dev_addr);
    skb_put(skb, sizeof(struct string_wid) + MAC_LEN);

    return (sizeof(struct string_wid) + MAC_LEN);
}

static int32 hwifi_init_fill_regdomain_msg(struct cfg_struct *cfg, struct sk_buff *skb)
{
    const struct ieee80211_regdomain   *regdom;
    struct hwifi_regdom_param          *regdom_param;
    uint32                              db_index = 0;

    regdom = hwifi_find_countrycode_regdom(cfg);

    if( skb_tailroom(skb) < (sizeof(struct hwifi_regdom_param)+regdom->n_reg_rules * sizeof(struct hwifi_regdom_rc_info_wid)))
    {
         HWIFI_WARNING("init regdomain msg fail,left size:%d,need size=%zu",\
                       skb_tailroom(skb),sizeof(struct hwifi_regdom_param)+regdom->n_reg_rules * sizeof(struct hwifi_regdom_rc_info_wid));
         return -EFAIL;
    }

    regdom_param = (struct hwifi_regdom_param *)skb_tail_pointer(skb);
    /* fill country code wid */
    hwifi_regdomain_fill_country_code(&regdom_param->country_code_wid, cfg->current_country_code);
    /* fill radar stadard code */
    hwifi_regdomain_fill_radar_code(&regdom_param->country_code_wid, &regdom_param->radar_code_wid, db_index);
    /* fill regulatory class bmap */
    hwifi_regdomain_fill_rc_bmap(regdom, &regdom_param->rc_bmap_wid);
    /* fill regulatory class general info */
    hwifi_regdomain_fill_rc_info(regdom, regdom_param->rc_info_wid);

#ifdef __ROAM__
    hwifi_roam_update_channels_regdomain_flag(&cfg->roam, regdom, regdom_param->rc_info_wid, FREQ_2G_MAP, SIZE_FREQ_2G_MAP, FREQ_5G_MAP, SIZE_FREQ_5G_MAP);
#endif

    skb_put(skb, sizeof(struct hwifi_regdom_param)
            + regdom->n_reg_rules * sizeof(struct hwifi_regdom_rc_info_wid));

    return (sizeof(struct hwifi_regdom_param)+regdom->n_reg_rules * sizeof(struct hwifi_regdom_rc_info_wid));

}
 int32 hwifi_init_hw_params(struct cfg_struct *cfg)
 {
     struct sk_buff                     *skb;
     int32                               ret;
     uint16                              msg_size = 0;
     int32                               len;
     struct hwifi_msg_header            *msg_header;


     HWIFI_ASSERT((NULL != cfg));

     skb = hwifi_alloc_skb_for_cmd(MAX_MSG_LEN);

     if (NULL == skb)
     {
         HWIFI_WARNING("hwifi_alloc_skb_for_cmd fail");
         return -EFAIL;
     }

     /* init msg:protol begin */
     msg_header = (struct hwifi_msg_header *)skb_put(skb, sizeof(struct hwifi_msg_header));
     msg_size += sizeof(struct hwifi_msg_header);

     /* init_msg,WID_11G_OPERATING_MODE begin*/
     len = hwifi_init_fill_char_wid(skb, WID_11G_OPERATING_MODE, cfg->sta_info.gmode); //  WID_G_MIXED_11B_2_MODE modify by luolingzhi
     if(len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_11G_OPERATING_MODE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_11G_OPERATING_MODE end*/

     /* init_msg,WID_CURRENT_TX_RATE begin*/
     len = hwifi_init_fill_char_wid(skb, WID_CURRENT_TX_RATE, WID_AUTO_RATE);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_CURRENT_TX_RATE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_CURRENT_TX_RATE end*/

     /* init_msg,WID_QOS_ENABLE begin */
     len = hwifi_init_fill_char_wid(skb, WID_QOS_ENABLE, WID_QOS);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_QOS_ENABLE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_QOS_ENABLE end */

     /* init_msg,WID_POWER_MANAGEMENT begin*/
     len = hwifi_init_fill_char_wid(skb, WID_POWER_MANAGEMENT, WID_SAVE_POWER);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_POWER_MANAGEMENT msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_POWER_MANAGEMENT end*/

     /* init_msg,WID_11I_MODE begin*/
     len = hwifi_init_fill_char_wid(skb, WID_11I_MODE , WID_NO_ENCRYP_MODEL);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_11I_MODE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_11I_MODE end*/

     /* init_msg,WID_AUTH_TYPE begin*/
     len = hwifi_init_fill_char_wid(skb, WID_AUTH_TYPE , WID_OPEN_AUTHENTICATION);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_AUTH_TYPE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_AUTH_TYPE end*/

     /* init_msg,WID_11N_ENABLE begin*/
     len = hwifi_init_fill_char_wid(skb, WID_11N_ENABLE , cfg->sta_info.ht_capability_enabled);    // WID_OPEN_HT modify by luolingzhi
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_11N_ENABLE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_11N_ENABLE end*/

     /* init_msg,WID_2040_ENABLE begin*/
     /*control 40M mode ; enabled:1-->open 40M mode ; enabled:0-->close 40M mode*/
     len = hwifi_init_fill_char_wid(skb, WID_2040_ENABLE , cfg->sta_info.fc_enabled);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_2040_ENABLE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_2040_ENABLE end*/

     /* init_msg,WID_SHORT_GI begin*/
     /*control 40M short gi ; enabled:1-->open 40M short gi ; enabled:0-->close 40M short gi*/
     len = hwifi_init_fill_char_wid(skb, WID_CHAR_RESERVE , cfg->sta_info.fc_enabled);

     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill SHORT_GI msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_SHORT_GI end*/

     /* init_msg,WID_2040_COEXISTENCE begin*/
     len = hwifi_init_fill_char_wid(skb, WID_2040_COEXISTENCE , WID_2040_COEXISTENCE_OPEN);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_2040_COEXISTENCE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init_msg,WID_2040_COEXISTENCE end*/

     /* init_msg,WID_LTE_COEX_ENABLE begin*/
     len = hwifi_init_fill_short_wid(skb, WID_LTE_COEX_ENABLE,
                        ((LTECOEX_CONFIG_CMD << 8) | g_lte_wifi_coex));
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill WID_LTE_COEX_ENABLE msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }
     msg_size += len;

     /*init msg:ba session begin*/
     len = hwifi_init_fill_ba_session_msg(skb);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill ba session msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /*init msg:ba session end*/

     /*init msg:mac begin*/
     len = hwifi_init_fill_mac_msg(cfg,skb);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill mac msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init msg:mac end */

     /* init msg:regdomain begin */
     len = hwifi_init_fill_regdomain_msg(cfg,skb);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill regdomain msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init msg:regdomain end */

     /* init msg:device log begin */
     len = hwifi_init_fill_device_log_msg(cfg,skb);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill device log msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init msg:device log end */

     /* init msg:currttxpow begin */
     len = hwifi_init_fill_currttxpow_msg(cfg,skb);
     if( len < 0 )
     {
        HWIFI_WARNING("init fail when fill currttxpow msg");
        dev_kfree_skb_any(skb);
        return -EFAIL;
     }

     msg_size += len;
     /* init msg:currttxpow end */

     if(msg_size != skb->len)
     {
          HWIFI_WARNING("init fail,Invalid message length,msg_size:%d skb->len:%d",msg_size,skb->len);
          dev_kfree_skb_any(skb);
          return -EFAIL;
     }

     /* set the msg header */
     hwifi_fill_msg_hdr(msg_header, HOST_CMD_INIT, msg_size);
     ret = hwifi_send_cmd(cfg, skb);

     PRINT_SEND_CMD_RET("INIT DEVICE", ret);

     return ret;
 }


 #ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

