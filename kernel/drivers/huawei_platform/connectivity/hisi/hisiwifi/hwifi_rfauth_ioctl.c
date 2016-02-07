


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
#include <linux/skbuff.h>

#include "hwifi_tps.h"
#include "hwifi_wl_config_ioctl.h"
#include "hwifi_msg.h"
#include "hwifi_cfg80211.h"
#include "hi110x_pm.h"
#include "hwifi_rfauth_ioctl.h"

/*
 * 2 Global Variable Definition
 */


/*
 * 3 Function Definition
 */

/*
 * Prototype    : wl_cw_mac_addr_set
 * Description  : pk mode set
 * Input        : struct cfg_struct *cfg,uint8 enabled
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/27
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  wl_cw_mac_addr_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid          *pk_mode;
    uint16                   msg_size;
    int32                    ret;

    ret = wl_pk_mode_addr_set(cfg, CW_MAC_ADDR);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send pk mode addr set msg!");
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

    /* fill hwar */
    pk_mode = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(pk_mode, WID_MEMORY_ACCESS_8BIT, enabled);
    msleep(1000);
    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send pk mode set msg!");
       return -EFAIL;
    }
    //cfg->sta_info.pk_mode = enabled;

    HWIFI_INFO("Succeed to set pk_mode:%d", enabled);

    return SUCC;
}


int32 wl_phy_stat_set(struct cfg_struct *cfg,uint8 channel_info)
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
     hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_1, WID_11N_PHY_ACTIVE_REG_VAL, 4);

     hwifi_fill_char_wid(&msg->phy_active_reg_2, WID_PHY_ACTIVE_REG, WID_11b_connect_PHY_ACTIVE_REG_4 );
     hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_2, WID_11N_PHY_ACTIVE_REG_VAL, 1 );
     HWIFI_WARNING("0x66 send down yo device");

     hwifi_fill_char_wid(&msg->phy_active_reg_3, WID_PHY_ACTIVE_REG,WID_11b_connect_PHY_ACTIVE_REG_5 );
     hwifi_fill_int_wid(&msg->hw_11n_phy_active_reg_val_3,WID_11N_PHY_ACTIVE_REG_VAL, 1 );
     msleep(1000);
     ret = hwifi_send_cmd(cfg, skb);

     PRINT_SEND_CMD_RET("connect status, 11b param init success", ret);

     return ret;

}
int32 wl_channel_set(struct cfg_struct *cfg,uint8 channel_info)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *channel_set;
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
    channel_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(channel_set, WID_PRIMARY_CHANNEL, channel_info);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
      HWIFI_WARNING("Failed to send channel mode set msg!");
      return -EFAIL;
    }
    cfg->android_ioctl.pri_data.channel = channel_info;

    HWIFI_INFO("Succeed to send channel mode set msg");

    return SUCC;
}


uint8 wl_channel_get(struct cfg_struct *cfg)
{
    return cfg->android_ioctl.pri_data.channel;
}


int32 wl_uchannel_set(struct cfg_struct *cfg,uint16 uchannel_info)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct short_wid         *uchannel_set;
    uint16                   msg_size;
    int32                    ret;

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct short_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    /* fill ps mode */
    uchannel_set = (struct short_wid *)skb_put(skb, sizeof(struct short_wid));

    hwifi_fill_short_wid(uchannel_set, WID_USER_PREF_CHANNEL, uchannel_info);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send uchannel mode set msg!");
        return -EFAIL;
    }
    cfg->android_ioctl.pri_data.uchannel = uchannel_info;

    HWIFI_INFO("Succeed to send uchannel mode set msg");

    return SUCC;
}



uint16  wl_uchannel_get(struct cfg_struct *cfg)
{
    return cfg->android_ioctl.pri_data.uchannel;
}




int32 wl_rfupc_set(struct cfg_struct *cfg,uint16 rfupc_info)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct short_wid        *rfupc_set;
    uint16                   msg_size;
    int32                    ret;

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct short_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    /* fill ps mode */
    rfupc_set = (struct short_wid *)skb_put(skb, sizeof(struct short_wid));

    hwifi_fill_short_wid(rfupc_set, WID_UPC_GAIN, rfupc_info);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send rfupc set msg!");
        return -EFAIL;
    }
    cfg->android_ioctl.pri_data.rfupc_info = rfupc_info;

    HWIFI_INFO("Succeed to send rfupc set msg");

    return SUCC;
}



uint16  wl_rfupc_get(struct cfg_struct *cfg)
{
    return cfg->android_ioctl.pri_data.rfupc_info;
}





int32 wl_scalea_set(struct cfg_struct *cfg,uint8 scalea_info)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *scalea_set;
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
    scalea_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(scalea_set, WID_11A_SCALE, scalea_info);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send scalea set msg!");
        return -EFAIL;
    }
    cfg->android_ioctl.pri_data.scalea_info = scalea_info;

    HWIFI_INFO("Succeed to send scalea set msg");

    return SUCC;
}



uint16  wl_scalea_get(struct cfg_struct *cfg)
{
    return cfg->android_ioctl.pri_data.scalea_info;
}





int32 wl_scaleb_set(struct cfg_struct *cfg,uint8 scaleb_info)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *scaleb_set;
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
    scaleb_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(scaleb_set, WID_11B_SCALE, scaleb_info);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send scalea set msg!");
        return -EFAIL;
    }
    cfg->android_ioctl.pri_data.scaleb_info = scaleb_info;

    HWIFI_INFO("Succeed to send scalea set msg");

    return SUCC;
}



uint16  wl_scaleb_get(struct cfg_struct *cfg)
{
    return cfg->android_ioctl.pri_data.scaleb_info;
}





int32 wl_mac_addr_set(struct cfg_struct *cfg,uint8 *mac)
{
    int32 ret;

    ret = wl_set_mac(cfg, mac);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to set mac!");
        return -EFAIL;
    }

    memcpy(cfg->ndev->dev_addr, mac, ETH_ALEN);
    return SUCC;
}



int32 wl_mac_addr_get(struct cfg_struct *cfg,uint8 *mac)
{
    memcpy(mac,cfg->ndev->dev_addr,ETH_ALEN);
    return SUCC;
}



int32 wl_rf_reg_set(struct cfg_struct *cfg,int32 rf_reg_info)
{
    struct sk_buff           *skb;
    struct hwifi_msg_header  *msghdr;
    struct int_wid           *rf_reg_set;
    uint16                    msg_size;
    int32                     ret;

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct int_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    /* fill ps mode */
    rf_reg_set = (struct int_wid *)skb_put(skb, sizeof(struct int_wid));

    hwifi_fill_int_wid(rf_reg_set, WID_RF_REG_VAL, rf_reg_info);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send rf_reg set msg!");
        return -EFAIL;
    }
    cfg->android_ioctl.pri_data.rf_reg_info = rf_reg_info;

    HWIFI_INFO("Succeed to send rf_reg set msg,%d",rf_reg_info);

    return SUCC;
}



int32  wl_rf_reg_get(struct cfg_struct *cfg)
{
    return cfg->android_ioctl.pri_data.rf_reg_info;
}


 int32 hwifi_ctrl_network(struct net_device *netdev,int32 flag)
 {
    int ret;
    if( TRUE == flag )
    {
        //ret = hwifi_android_ioctl_network(netdev,IFACE_ON);
    }else if( FALSE == flag )
    {
        //ret = hwifi_android_ioctl_network(netdev,IFACE_OFF);
    }else{
        HWIFI_INFO("the flag of control network is invalid");
        return -EFAIL;
    }

    return ret;
 }


int32 wl_android_set_close5g(struct cfg_struct *cfg,int32 maddr_value)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *maddr_set;
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

    /* close 5g  */
    maddr_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(maddr_set, WID_CALI_MASK_CTRL, maddr_value);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to close 5g!");
        return -EFAIL;
    }
    cfg->android_ioctl.pri_data.maddr_val = maddr_value;

    msleep(500);
    HWIFI_INFO("Succeed to close 5g");

    return SUCC;

}


int32 wl_rx_count_clean(struct cfg_struct *cfg, uint32 rx_count)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct int_wid          *rx_count_wid;
    uint16                   msg_size;
    int32                    ret;

    msg_size = sizeof(struct hwifi_msg_header) + sizeof(struct int_wid);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
       return -EFAIL;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    /* fill msg */
    rx_count_wid = (struct int_wid *)skb_put(skb, sizeof(struct int_wid));

    hwifi_fill_int_wid(rx_count_wid, WID_INT_RESERVE, rx_count);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send wl_rx_count_set msg!");
       return -EFAIL;
    }

    return SUCC;
}


int32 wl_get_rx_info(struct cfg_struct *cfg)
{
    struct sk_buff                *skb;
    struct hwifi_query_msg_header *msghdr;
    uint8                         *msg;
    uint16                         msg_size;
    int32                          ret;

    msg_size = sizeof(struct hwifi_query_msg_header) + sizeof(uint16);
    if (MAX_MSG_LEN < msg_size)
    {
        HWIFI_WARNING("Invalid too long msg_size=%d", msg_size);
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
    msghdr->wid_list[0] = WID_INT_RESERVE;


    ret = hwifi_send_cmd(cfg, skb);
    PRINT_SEND_CMD_RET("get rx info",ret);
    return ret;
}
int32 wl_rx_count_get(struct cfg_struct *cfg)
{
    int32 ret;

    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        return -EFAIL;
    }

    cfg->android_ioctl.rx_count_read_flag = -EFAIL;

    ret = wl_get_rx_info(cfg);
    if(SUCC != ret)
    {
        HWIFI_WARNING("Failed to get rx count!");
        return -EFAIL;
    }

    ret = wait_event_interruptible_timeout(cfg->wait_queue, (-EFAIL != cfg->android_ioctl.rx_count_read_flag), 5 * HZ);
    if (0 == ret)
    {
        HWIFI_WARNING("wait for rx count info message time out(5s)!");
        return -EFAIL;
    }
    else if (ret < 0)
    {
        HWIFI_WARNING("wait for rx count info message error!");
        return -EFAIL;
    }

    HWIFI_DEBUG("report rx count info is %d", cfg->android_ioctl.rx_count);

    return cfg->android_ioctl.rx_count;
}


int32 hwifi_android_ioctl_cmd(struct hi110x_device* hi110x_dev, struct ifreq *ifr, int32 cmd)
{
    wifi_android_ioctl_data_struct ioctl_data;
    int32 ret = SUCC;

    if ((NULL == hi110x_dev) || (NULL == ifr) || (NULL == ifr->ifr_data))
    {
        HWIFI_WARNING("Invalid empty value arguments!");
        return -EFAIL;
    }

    if(copy_from_user(&ioctl_data,ifr->ifr_data,sizeof(wifi_android_ioctl_data_struct)))
    {
        HWIFI_WARNING("Failed to copy ifr->ifr_data from user!");
        return -EFAIL;
    }

    HWIFI_INFO("CMD:%d",ioctl_data.cmd);
    switch(ioctl_data.cmd)
    {
        /*wmode set*/
        case HWIFI_ANDROID_IOCTL_CMD_MODE_SET:
             ret = hwifi_test_mode_set(hi110x_dev->cfg,ioctl_data.pri_data.mode);
             break;
        /*wmode get*/
        case HWIFI_ANDROID_IOCTL_CMD_MODE_GET:
             ioctl_data.pri_data.mode=hwifi_test_mode_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy ioctl_data to user !");
                 ret = -EFAIL;
             }
             break;

        /*信道设置 new*/
        /* 设置20M带宽下的信道*/
        case HWIFI_ANDROID_IOCTL_CMD_CHAN_SET:
            HWIFI_INFO("Set chan : %d", ioctl_data.pri_data.channel);
            ret = wl_channel_set(hi110x_dev->cfg, ioctl_data.pri_data.channel);
            break;

        case HWIFI_ANDROID_IOCTL_CMD_CHAN_GET:
            ioctl_data.pri_data.channel = wl_channel_get(hi110x_dev->cfg);
            if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;

        /* 设置40M带宽下的信道*/
        case HWIFI_ANDROID_IOCTL_CMD_UCHAN_SET:
            HWIFI_INFO("Set uchan : %d", ioctl_data.pri_data.uchannel);
            ret = wl_uchannel_set(hi110x_dev->cfg, ioctl_data.pri_data.uchannel);
            break;

        case HWIFI_ANDROID_IOCTL_CMD_UCHAN_GET:
            ioctl_data.pri_data.uchannel = wl_uchannel_get(hi110x_dev->cfg);
            if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;

        /*currenttxpow set */
        case HWIFI_ANDROID_IOCTL_CMD_CURRENTTXPOW_SET:
            HWIFI_INFO("Set current_tx_pow : %d", ioctl_data.pri_data.currenttxpow);
            ret = wl_currenttxpow_set(hi110x_dev->cfg, ioctl_data.pri_data.currenttxpow);
            break;
        case HWIFI_ANDROID_IOCTL_CMD_CURRENTTXPOW_GET:
            ioctl_data.pri_data.currenttxpow = wl_currenttxpow_get(hi110x_dev->cfg);
            if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;

        /*配置速率 */
        case HWIFI_ANDROID_IOCTL_CMD_RATE_SET:
             ret = wl_rate_set(hi110x_dev->cfg, ioctl_data.pri_data.rate);
             break;
        case HWIFI_ANDROID_IOCTL_CMD_RATE_GET:
             ioctl_data.pri_data.rate = wl_rate_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_ANDROID_IOCTL_CMD_MCS_SET:
             ret = wl_mcs_set(hi110x_dev->cfg, ioctl_data.pri_data.mcs);
             break;

        case HWIFI_ANDROID_IOCTL_CMD_MCS_GET:
             ioctl_data.pri_data.mcs = wl_mcs_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }
             break;

        /* fc */
        case HWIFI_ANDROID_IOCTL_CMD_FC_SET:
             ret = wl_fc_set(hi110x_dev->cfg, ioctl_data.pri_data.fc_enabled);
             break;

        case HWIFI_ANDROID_IOCTL_CMD_FC_GET:
             ioctl_data.pri_data.fc_enabled = wl_fc_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        /*关闭低功耗 pwrm*/
        case HWIFI_ANDROID_IOCTL_CMD_POWER_MANAGEMENT_SET:
             ret = wl_pwrm_set(hi110x_dev->cfg, ioctl_data.pri_data.pwrm);
             if (ret !=-EFAIL)
             {
                hi110x_dev->cfg->sta_info.manual_pwrm = ioctl_data.pri_data.pwrm;
             }

             /* 如果配置省电模式0，配置Device相应的模式 */
             if (0 == ioctl_data.pri_data.pwrm)
             {
                 ret = hwifi_shutdown_powersave(hi110x_dev->cfg);
                 HWIFI_WARNING("shutdown powersave now...");
             }
             break;

        case HWIFI_ANDROID_IOCTL_CMD_POWER_MANAGEMENT_GET:
             ioctl_data.pri_data.pwrm = wl_pwrm_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;
        /*lowpower_ps_switch*/
        case HWIFI_ANDROID_IOCTL_CMD_PS_SWITCH_SET:
             HWIFI_INFO("Set g_powermgmt_switch : %d", ioctl_data.pri_data.ps_switch);
             g_powermgmt_switch = ioctl_data.pri_data.ps_switch;
             break;
        case HWIFI_ANDROID_IOCTL_CMD_PS_SWITCH_GET:
             HWIFI_INFO("Get g_powermgmt_switch : %d",g_powermgmt_switch);
             ioctl_data.pri_data.ps_switch = g_powermgmt_switch;
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_android_ioctl_data_struct)))
             {
                  HWIFI_WARNING("copy_to_user failed");
                  ret = -EFAIL;
             }
             break;

        /*tpc*/
        case HWIFI_ANDROID_IOCTL_OLTPC_ACTIVE_SET:
              ret = hwifi_oltpc_active_set(hi110x_dev->cfg,ioctl_data.pri_data.oltpc_active);
              break;

        case HWIFI_ANDROID_IOCTL_OLTPC_ACTIVE_GET:
             ioctl_data.pri_data.oltpc_active=hwifi_oltpc_active_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ioctl_data to user !");
                ret = -EFAIL;
             }
             break;

        /*rfupc new finish*/
        case HWIFI_ANDROID_IOCTL_RFUPC_SET:
             HWIFI_INFO("Set rfupc : %d", ioctl_data.pri_data.rfupc_info);
             ret = wl_rfupc_set(hi110x_dev->cfg, ioctl_data.pri_data.rfupc_info);
             break;

        case HWIFI_ANDROID_IOCTL_RFUPC_GET:
             ioctl_data.pri_data.rfupc_info = wl_rfupc_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ioctl_data to user !");
                ret = -EFAIL;
             }
             break;

        /*scalea new finish */
        case HWIFI_ANDROID_IOCTL_SCALEA_SET:
             HWIFI_INFO("Set scalea : %d", ioctl_data.pri_data.scalea_info);
             ret = wl_scalea_set(hi110x_dev->cfg, ioctl_data.pri_data.scalea_info);
             break;

        case HWIFI_ANDROID_IOCTL_SCALEA_GET:
             ioctl_data.pri_data.scalea_info= wl_scalea_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ioctl_data to user !");
                ret = -EFAIL;
             }
             break;

        /*scaleb  new finish*/
        case HWIFI_ANDROID_IOCTL_SCALEB_SET:
             HWIFI_INFO("Set scaleb : %d", ioctl_data.pri_data.scaleb_info);
             ret = wl_scaleb_set(hi110x_dev->cfg, ioctl_data.pri_data.scaleb_info);
             break;

        case HWIFI_ANDROID_IOCTL_SCALEB_GET:
             ioctl_data.pri_data.scaleb_info= wl_scaleb_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ioctl_data to user !");
                ret = -EFAIL;
             }
             break;

        /*mac address new finish*/
        case HWIFI_ANDROID_IOCTL_MAC_ADDRESS_SET:
             ret = wl_mac_addr_set(hi110x_dev->cfg, ioctl_data.pri_data.mac_addr);
             break;

        case HWIFI_ANDROID_IOCTL_MAC_ADDRESS_GET:
             ret = wl_mac_addr_get(hi110x_dev->cfg,ioctl_data.pri_data.mac_addr);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy ioctl_data to user !");
                 ret = -EFAIL;
             }
             break;

        /*rf reg value new finish*/
        case HWIFI_ANDROID_IOCTL_RF_REG_SET:
             HWIFI_INFO("Set reg value : %d", ioctl_data.pri_data.rf_reg_info);
             ret = wl_rf_reg_set(hi110x_dev->cfg, ioctl_data.pri_data.rf_reg_info);
             break;

        case HWIFI_ANDROID_IOCTL_RF_REG_GET:
             ioctl_data.pri_data.rf_reg_info= wl_rf_reg_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ioctl_data to user !");
                ret = -EFAIL;
             }
             break;

        case HWIFI_ANDROID_IOCTL_CMD_CTRL_NETWORK:// 与上层结构体保持一致
             HWIFI_INFO("control network : %d", ioctl_data.pri_data.network_flag);
             //不会执行到此处，此命令预留
             break;
        case HWIFI_ANDROID_IOCTL_CMD_GI_SET:
             HWIFI_INFO("gi set : %d", ioctl_data.pri_data.gi_enable);
             ret = wl_gi_set(hi110x_dev->cfg, ioctl_data.pri_data.gi_enable);
             break;
        case HWIFI_ANDROID_IOCTL_CMD_GI_GET:
            HWIFI_INFO("gi get : %d", ioctl_data.pri_data.gi_enable);
            ioctl_data.pri_data.gi_enable = wl_gi_get(hi110x_dev->cfg);
            if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
            {
               HWIFI_WARNING("Failed to copy to user!");
               ret = -EFAIL;
            }
             break;
        case HWIFI_ANDROID_IOCTL_CMD_RUNTC_SET:
             HWIFI_INFO("runtc test case set : %d", ioctl_data.pri_data.test_case);
             ret = wl_runtc_set(hi110x_dev->cfg, ioctl_data.pri_data.test_case);
             break;
        case HWIFI_ANDROID_IOCTL_CMD_MAC_REG_SET:
             HWIFI_INFO("mac reg set : %d", ioctl_data.pri_data.mac_reg);
             ret =  wl_cw_mac_addr_set(hi110x_dev->cfg, ioctl_data.pri_data.mac_reg);
             break;
        case HWIFI_ANDROID_IOCTL_CMD_PHY_REG_SET:
             HWIFI_INFO("phy_reg set : %d", ioctl_data.pri_data.phy_reg);
             ret = wl_phy_stat_set(hi110x_dev->cfg, ioctl_data.pri_data.phy_reg);
             break;
        case HWIFI_ANDROID_IOCTL_CMD_FEM_CLOSE_5G:
             HWIFI_INFO("fem close 5g : %0X", ioctl_data.pri_data.maddr_val);
             ret = wl_android_set_close5g(hi110x_dev->cfg, ioctl_data.pri_data.maddr_val);
             break;
        case HWIFI_ANDROID_IOCTL_CMD_FEM_FLAG_GET:
             ioctl_data.pri_data.fem_flag = hwifi_upc_get(hi110x_dev->cfg);
             HWIFI_INFO("fem flag get : %d", ioctl_data.pri_data.fem_flag);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_android_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy ioctl_data to user !");
                 ret = -EFAIL;
             }
             break;
        case HWIFI_ANDROID_IOCTL_CMD_RX_COUNT_CLEAN:
             HWIFI_INFO("rx count clean! ");
             ret = wl_rx_count_clean(hi110x_dev->cfg, ioctl_data.pri_data.rx_count);
             break;
        case HWIFI_ANDROID_IOCTL_CMD_RX_COUNT_GET:
            ioctl_data.pri_data.rx_count = wl_rx_count_get(hi110x_dev->cfg);
            HWIFI_INFO("rx count get : %d", ioctl_data.pri_data.rx_count);
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_android_ioctl_data_struct)))
            {
               HWIFI_WARNING("Failed to copy to user!");
               ret = -EFAIL;
            }
            break;
        default:
            HWIFI_WARNING("Invalid ioctl_data.cmd(%d)!",ioctl_data.cmd);
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

