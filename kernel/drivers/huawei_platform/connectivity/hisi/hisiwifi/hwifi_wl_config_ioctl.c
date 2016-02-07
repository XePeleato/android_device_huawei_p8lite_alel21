


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

#include "hi110x.h"
#include "hwifi_wl_config_ioctl.h"
#include "hwifi_cfgapi.h"
#include "hwifi_msg.h"
#include "hi110x_pm.h"


#include <linux/skbuff.h>
#include "cfg80211_stru.h"
#include "hwifi_utils.h"
#include "hwifi_cfg80211.h"
#include "hwifi_sfw_antijam.h"
#include "hwifi_netdev.h"
#include "hwifi_dev_err.h"
#ifdef WLAN_ARRG_DYNAMIC_CONTROL
#include "hwifi_aggre.h"
#endif
/*
 * 2 Global Variable Definition
 */
extern uint32 g_powermgmt_switch;


uint8 g_use_dync_16vs8 = 1;


uint8 g_method         = 0;


/*
 * 3 Function Definition
 */


/*
 * Prototype    : wl_rate_set
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
int32  wl_rate_set(struct cfg_struct *cfg, uint8  rate)
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
        HWIFI_WARNING("Failed to  send rate set msg!");
        return -EFAIL;
    }

    cfg->sta_info.rate = rate;

    HWIFI_INFO("Succeed to send rate set msg");

    return SUCC;
}


int32  wl_runtc_set(struct cfg_struct *cfg, uint8 test_case)
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
    hwifi_fill_char_wid(rate_set, WID_DFX_RUNTC, test_case);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to  send rate set msg!");
        return -EFAIL;
    }

    cfg->sta_info.test_case = test_case;

    HWIFI_INFO("Succeed to send rate set msg");

    return SUCC;
}


/*
 * Prototype    : wl_mcs_set
 * Description  : set mcs rate
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
int32 wl_mcs_set(struct cfg_struct *cfg,uint8 mcs)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *mcs_set;
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
    mcs_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(mcs_set, WID_11N_CURRENT_TX_MCS, mcs);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to  send mcs rate set msg!");
        return -EFAIL;
    }

    cfg->sta_info.mcs = mcs;

    HWIFI_INFO("Succeed to send mcs rate set msg");

    return SUCC;


}

/*
 * Prototype    : wl_qos_set
 * Description  : set qos ap enabled
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
int32 wl_qos_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *qos;
    uint16                   msg_size;
    int32                    ret;

    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support qos set.");
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
    qos = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(qos, WID_QOS_ENABLE, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to  send qos ap enabled msg!");
        return -EFAIL;
    }

    cfg->sta_info.qos_enabled = enabled;

    HWIFI_INFO("Succeed to send qos ap enabled msg");

    return SUCC;


}

/*
 * Prototype    : wl_rifs_prot_set
 * Description  : set rifs prot enabled
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
int32 wl_rifs_prot_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *rifs;
    uint16                   msg_size;
    int32                    ret;

    if (IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support rifs prot mode set.");
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
    rifs = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(rifs, WID_RIFS_MODE, enabled);
    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to  send rifs prot enable msg!");
        return -EFAIL;
    }
    cfg->sta_info.rifs_prot_enabled = enabled;

    HWIFI_INFO("Succeed to send rifs prot enable msg");

    return SUCC;


}
/*
 * Prototype    : wl_ack_policy_set
 * Description  : set ack policy enabled
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
int32 wl_ack_policy_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *ack;
    uint16                   msg_size;
    int32                    ret;

    if (IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support ack policy set.");
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
    ack = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));
    hwifi_fill_char_wid(ack, WID_ACK_POLICY, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send ack policy enable msg!");
        return -EFAIL;
    }
    cfg->sta_info.ack_policy_enabled = enabled;

    HWIFI_INFO("Succeed to send ack policy enable msg");

    return SUCC;


}
/*
 * Prototype    : wl_txop_prot_set
 * Description  : set txop prot enabled
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
int32 wl_txop_prot_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *txop;
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
    txop = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(txop, WID_11N_TXOP_PROT_DISABLE, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send txop prot enable msg!");
        return -EFAIL;
    }
    cfg->sta_info.txop_prot_enabled = enabled;

    HWIFI_INFO("Succeed to send txop prot enable msg");

    return SUCC;
}
/*
 * Prototype    : wl_erp_prot_set
 * Description  : set erp prot enabled
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
int32 wl_erp_prot_set(struct cfg_struct *cfg,uint8 erp)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *erp_set;
    uint16                   msg_size;
    int32                    ret;

    if (IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support erp prot mode set.");
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
    erp_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(erp_set, WID_11N_ERP_PROT_TYPE, erp);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to  send erp prot enable msg!");
        return -EFAIL;
    }
    cfg->sta_info.erp_prot = erp;

    HWIFI_INFO("Succeed to send erp prot enable msg");

    return SUCC;
}

/*
 * Prototype    : wl_htprot_set
 * Description  : set ht prot enabled
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
int32 wl_htprot_set(struct cfg_struct *cfg,uint8 htprot)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *htprot_set;
    uint16                   msg_size;
    int32                    ret;

    if (IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support ht prot mode set.");
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
    htprot_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(htprot_set, WID_11N_HT_PROT_TYPE, htprot);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to  send ht prot enable msg!");
        return -EFAIL;
    }
    cfg->sta_info.htprot = htprot;

    HWIFI_INFO("Succeed to send ht prot enable msg");

    return SUCC;
}
/*
 * Prototype    : wl_gi_set
 * Description  : set gi enabled
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
int32 wl_gi_set(struct cfg_struct *cfg,uint8 enabled)
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
        return -EFAIL;
    }

    /* fill the msg header */
    msghdr = (struct hwifi_msg_header*)skb_put(skb, sizeof(struct hwifi_msg_header));
    hwifi_fill_msg_hdr(msghdr, HOST_CMD_CONFIG, msg_size);

    /* fill ps mode */
    gi = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(gi, WID_11N_SHORT_GI_ENABLE, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to  send GI  enable msg!");
        return -EFAIL;
    }
    cfg->sta_info.gi_enabled = enabled;

    HWIFI_INFO("Succeed to send GI enable msg");

    return SUCC;
}
/*
 * Prototype    : wl_ht_set
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
int32 wl_ht_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *ht;
    uint16                   msg_size;
    int32                    ret;
    if (IS_CONNECTED(cfg) || IS_AP(cfg))
    {
        HWIFI_WARNING("Connected mode can not support ht capability set.");
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

    hwifi_fill_char_wid(ht, WID_11N_ENABLE, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to  send ht capability enable msg!");
        return -EFAIL;
    }
    cfg->sta_info.ht_capability_enabled = enabled;

    HWIFI_INFO("Succeed to send ht capability enable msg");

    return SUCC;
}

/*
 * Prototype    : wl_pwrm_set
 * Description  : set power management
 * Input        : struct cfg_struct *cfg,uint8 enabled
 *
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
int32 wl_pwrm_set(struct cfg_struct *cfg,uint8 pwrm)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *pwrm_set;
    uint16                   msg_size;
    int32                    ret;

    if(IS_AP(cfg))
    {
         HWIFI_WARNING("in ap mode can not support pwrm mode set.");
         return -EFAIL;
    }

    if(!g_powermgmt_switch)
    {
        HWIFI_WARNING("g_powermgmt_switch(%d) is closed,ignore power save command.", g_powermgmt_switch);
        return SUCC;
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
    pwrm_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(pwrm_set, WID_PS_MODE, pwrm);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to  send power management set msg!");
       return -EFAIL;
    }
    cfg->sta_info.pwrm = pwrm;

    HWIFI_INFO("Succeed to set power management mode %u", pwrm);

    /* 当device已经enable ps时，新设置的值必须重新enable*/
    if (cfg->sta_info.ps_enabled)
    {
        ret = wl_pwrm_enable(cfg, POWER_SAVE_ENABLE);
        if(SUCC != ret)
        {
            HWIFI_WARNING("wl_pwrm_enable return failed,ret=%d",ret);
            return -EFAIL;
        }
    }

    return SUCC;
}
/*
 * Prototype    : wl_pwrm_enable
 * Description  : enable power save
 * Input        : struct cfg_struct *cfg,uint8 enabled
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/8
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  wl_pwrm_enable(struct cfg_struct *cfg,uint8 enabled)
{

    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *ps_enabled;
    uint16                   msg_size;
    int32                    ret;

    if(IS_AP(cfg))
    {
         HWIFI_WARNING("Power save not supported in ap mode.");
         return -EFAIL;
    }

    if(!g_powermgmt_switch)
    {
        HWIFI_WARNING("g_powermgmt_switch(%d) is closed,ignore power save command.", g_powermgmt_switch);
        return SUCC;
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
    ps_enabled = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(ps_enabled, WID_PS_ENABLE, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to  send power save enable msg!");
       return -EFAIL;
    }

    cfg->sta_info.ps_enabled = enabled;

    HWIFI_INFO("Succeed to set power save enable:%d",enabled);

    return SUCC;
}

/*
 * Prototype    : wl_prot_set
 * Description  : set prot mode
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
int32 wl_prot_set(struct cfg_struct *cfg,uint8 prot)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *prot_set;
    uint16                   msg_size;
    int32                    ret;

    if (IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support prot mode set.");
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
    prot_set = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(prot_set, WID_11N_PROT_MECH, prot);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send prot mode set msg!");
       return -EFAIL;
    }
    cfg->sta_info.prot = prot;

    HWIFI_INFO("Succeed to send prot mode set msg");

    return SUCC;
}

/*
 * Prototype    : wl_2040_coex_set
 * Description  : 20/40 coex mode set
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
int32 wl_2040_coex_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *coex;
    uint16                   msg_size;
    int32                    ret;
    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support 20/40 coex enable set.");
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
    coex = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(coex, WID_2040_COEXISTENCE, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send 20/40 coex mode set msg!");
       return -EFAIL;
    }

    if(IS_AP(cfg))
    {
        cfg->ap_info.coex_2040_enabled = enabled;
    }
    else
    {
        cfg->sta_info.coex_2040_enabled = enabled;
    }

    HWIFI_INFO("Succeed to send 20/40 coex mode set %d msg", enabled);

    return SUCC;
}


int32 wl_amsdu_certify_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *coex;
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
    coex = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(coex, WID_AMSDU_LEN_FIX, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send amsdu certify mode set msg!");
       return -EFAIL;
    }

    cfg->ap_info.amsdu_certify_enabled = enabled;

    HWIFI_INFO("Succeed to send amsdu certify mode set %d msg", enabled);

    return SUCC;
}


int32 wl_psm_certify_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *coex;
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
    coex = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(coex, WID_CHAR_RESERVE02, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send psm certify mode set msg!");
       return -EFAIL;
    }

    cfg->ap_info.psm_certify_enabled = enabled;

    HWIFI_INFO("Succeed to send psm certify mode set %d msg", enabled);

    return SUCC;
}


int32 wl_wmm_ac_opt_set(struct hcc_handler *hcc, uint8 enabled)
{
    HWIFI_ASSERT(NULL != hcc);

    hcc->sys_wmm_ac_opt_enable = enabled;

    HWIFI_INFO("Succeed to set wmm ac opt:%d", enabled);

    return SUCC;
}


/*
 * Prototype    : wl_fc_set
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
int32  wl_fc_set(struct cfg_struct *cfg,uint8 enabled)
{
    int32                    ret;

    ret = hwifi_sta_2040_enable_ctrl_set(cfg,enabled);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send fc param set msg!");
       return -EFAIL;
    }

    HWIFI_INFO("Succeed to set fc:%d", enabled);

    return SUCC;
}
/*
 * Prototype    : wl_hwar_set
 * Description  : set hardware auto rate
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
int32  wl_hwar_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *hwar;
    uint16                   msg_size;
    int32                    ret;

    if(!IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support hwar param set.");
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
    hwar = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(hwar, WID_DFX_HWAR, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send hwar param set msg!");
       return -EFAIL;
    }
    cfg->sta_info.hwar= enabled;

    HWIFI_INFO("Succeed to set hwar:%d", enabled);

    return SUCC;
}
/*
 * Prototype    : wl_pk_mode_addr_set
 * Description  : pk mode addr set
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
int32  wl_pk_mode_addr_set(struct cfg_struct *cfg,int32 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct int_wid          *pk_mode_addr;
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

    /* fill hwar */
    pk_mode_addr = (struct int_wid *)skb_put(skb, sizeof(struct int_wid));

    hwifi_fill_int_wid(pk_mode_addr, WID_MEMORY_ADDRESS, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send pk mode addr set msg!");
       return -EFAIL;
    }

    return SUCC;
}

/*
 * Prototype    : wl_pk_mode_set
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
int32  wl_pk_mode_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid          *pk_mode;
    uint16                   msg_size;
    int32                    ret;

    ret = wl_pk_mode_addr_set(cfg,PK_MODE_ADDR);
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

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send pk mode set msg!");
       return -EFAIL;
    }
    cfg->sta_info.pk_mode = enabled;

    HWIFI_INFO("Succeed to set pk_mode:%d", enabled);

    return SUCC;
}

/*
 * Prototype    : wl_gmode_set
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
int32  wl_gmode_set(struct cfg_struct *cfg,uint8 mode)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *gmode;
    uint16                   msg_size;
    int32                    ret;

    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support gmode enable set.");
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
       HWIFI_WARNING("Failed to send gmode param set msg!");
       return -EFAIL;
    }
    cfg->sta_info.gmode= mode;

    HWIFI_INFO("Succeed to set gmode:%d", mode);

    return SUCC;
}
/*
 * Prototype    : wl_nmode_set
 * Description  : set 11n operating mode
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
int32  wl_nmode_set(struct cfg_struct *cfg,uint8 mode)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *nmode;
    uint16                   msg_size;
    int32                    ret;

    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support nmode param set.");
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
    nmode = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(nmode, WID_11N_OPERATING_TYPE, mode);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send nmode param set msg!");
       return -EFAIL;
    }
    cfg->sta_info.nmode= mode;

    HWIFI_INFO("Succeed to set nmode:%d", mode);

    return SUCC;
}
/*
 * Prototype    : wl_lint_set
 * Description  : set listen interval
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
int32  wl_lint_set(struct cfg_struct *cfg,int32 interval)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *lint;
    uint16                   msg_size;
    int32                    ret;

    if(IS_CONNECTED(cfg) || IS_AP(cfg))
    {
        HWIFI_WARNING("Connected mode can not support lint set.");
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

    /* fill lint */
    lint = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(lint, WID_LISTEN_INTERVAL, interval);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send lint param set msg!");
       return -EFAIL;
    }
    cfg->sta_info.lint= interval;

    HWIFI_INFO("Succeed to set lint:%d", interval);

    return SUCC;
}
/*
 * Prototype    : wl_pream_set
 * Description  : set preamble type
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
int32  wl_pream_set(struct cfg_struct *cfg,uint8 mode)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *pream;
    uint16                   msg_size;
    int32                    ret;

    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support pream param set.");
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

    /* fill lint */
    pream = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(pream, WID_PREAMBLE, mode);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send pream param set msg!");
       return -EFAIL;
    }
    cfg->sta_info.pream= mode;

    HWIFI_INFO("Succeed to set pream:%d", mode);

    return SUCC;
}
/*
 * Prototype    : wl_ctxbw_set
 * Description  : set tx channel bandwidth
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
int32  wl_ctxbw_set(struct cfg_struct *cfg,uint8 bandwidth)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *ctxbw;
    uint16                   msg_size;
    int32                    ret;


    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support ctxbw set.");
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

    /* fill lint */
    ctxbw = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(ctxbw, WID_11N_CURRENT_TX_BW, bandwidth);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send ctxbw param set msg!");
       return -EFAIL;
    }
    cfg->sta_info.ctxbw= bandwidth;

    HWIFI_INFO("Succeed to set ctxbw:%d", bandwidth);

    return SUCC;
}
/*
 * Prototype    : wl_intol_set
 * Description  : set 40MHz Intolerance mode
 * Input        : struct cfg_struct *cfg,uint8 enabled
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/5/6
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  wl_intol_set(struct cfg_struct *cfg,uint8 enabled)
{
    struct sk_buff          *skb;
    struct hwifi_msg_header *msghdr;
    struct char_wid         *intol;
    uint16                   msg_size;
    int32                    ret;


    if(IS_CONNECTED(cfg))
    {
        HWIFI_WARNING("Connected mode can not support intol param set.");
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

    /* fill lint */
    intol = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

    hwifi_fill_char_wid(intol, WID_2040_40MHZ_INTOLERANT, enabled);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
       HWIFI_WARNING("Failed to send intol param set msg!");
       return -EFAIL;
    }
    cfg->sta_info.ctxbw= enabled;

    HWIFI_INFO("Succeed to set intol:%d", enabled);

    return SUCC;
}

/*
 * Prototype    : wl_currenttxpow_set
 * Description  : set current tx power
 * Input        : struct cfg_struct *cfg,uint8 currt_tx_power
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/3/22
 *     Author       : zhanglu/z00217036
 *     Modification : Created function
 *
 */
int32 wl_currenttxpow_set(struct cfg_struct *cfg,int32 currt_tx_power)
{
       struct sk_buff          *skb;
       struct hwifi_msg_header *msghdr;
       struct char_wid         *intol;
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

       /* fill lint */
       intol = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

       hwifi_fill_char_wid(intol, WID_CURRENT_TX_POW, currt_tx_power);

       ret = hwifi_send_cmd(cfg, skb);
       if (SUCC != ret)
       {
          HWIFI_WARNING("Failed to send intol param set msg!");
          return -EFAIL;
       }
       cfg->sta_info.currenttxpow = currt_tx_power;

       HWIFI_INFO("Succeed to set currt_tx_power:%d", currt_tx_power);

       return SUCC;

}

/*
 * Prototype    : wl_device_log_set
 * Description  : set control device log
 * Input        : struct cfg_struct *cfg,uint8 device_log_module_id,uint8 log_control
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/3/22
 *     Author       : zhanglu/z00217036
 *     Modification : Created function
 *
 */
int32 wl_device_log_set(struct cfg_struct *cfg,uint8 device_log_module_id,uint8 log_control)
{
       struct sk_buff          *skb;
       struct hwifi_msg_header *msghdr;
       struct char_wid         *intol;
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

       /* fill lint */
       intol = (struct char_wid *)skb_put(skb, sizeof(struct char_wid));

       if( WIFI_DEVICE_LOG_ON == log_control )
       {
            hwifi_fill_char_wid(intol, WID_DFX_LOG_ON, device_log_module_id);
       }else if( WIFI_DEVICE_LOG_OFF == log_control)
       {
            hwifi_fill_char_wid(intol, WID_DFX_LOG_OFF, device_log_module_id);
       }else{
			dev_kfree_skb_any(skb);
            return -EFAIL;
       }

       ret = hwifi_send_cmd(cfg, skb);
       if (SUCC != ret)
       {
          HWIFI_WARNING("Failed to send device log level param set msg!");
          return -EFAIL;
       }

       cfg->device_log.device_log_flag[device_log_module_id]= log_control;

       HWIFI_ERROR("Succeed to set device log level param,device_log_module_id:%d, \
          device_log_module_id:%d(1: on,0:off)", device_log_module_id,log_control);

       return SUCC;

}




/*
 * Prototype    : wl_rate_get
 * Description  : get rate
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_rate_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.rate;
}


uint8  wl_runtc_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.test_case;
}

/*
 * Prototype    : wl_mcs_get
 * Description  : get mcs rate
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_mcs_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.mcs;
}


/*
 * Prototype    : wl_qos_get
 * Description  : get qos
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_qos_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.qos_enabled;
}

/*
 * Prototype    : wl_ackp_get
 * Description  : get ackp
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_ackp_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.ack_policy_enabled;
}
/*
 * Prototype    : wl_rifs_prot_get
 * Description  : get rifs prot mode
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_rifs_prot_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.rifs_prot_enabled;
}

/*
 * Prototype    : wl_pk_mode_get
 * Description  : get pk mode
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_pk_mode_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.pk_mode;
}
/*
 * Prototype    : wl_txop_get
 * Description  : get txop
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_txop_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.txop_prot_enabled;
}

/*
 * Prototype    : wl_erp_get
 * Description  : get erp prot
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_erp_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.erp_prot;
}

/*
 * Prototype    : wl_htprot_get
 * Description  : get htprot
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_htprot_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.htprot;
}

/*
 * Prototype    : wl_gi_get
 * Description  : get gi
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_gi_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.gi_enabled;
}

/*
 * Prototype    : wl_ht_get
 * Description  : get ht
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_ht_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.ht_capability_enabled;
}

/*
 * Prototype    : wl_pwrm_get
 * Description  : get pwrm
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_pwrm_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.pwrm;
}

/*
 * Prototype    : wl_prot_get
 * Description  : get prot
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/2/19
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_prot_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.prot;
}
/*
 * Prototype    : wl_coex_2040_get
 * Description  : get 20/40 coex mode
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : uint8
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/5
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_coex_2040_get(struct cfg_struct *cfg)
{
    if (IS_AP(cfg))
    {
        return cfg->ap_info.coex_2040_enabled;
    }
    else
    {
        return cfg->sta_info.coex_2040_enabled;
    }
}


uint8  wl_amsdu_certify_get(struct cfg_struct *cfg)
{

    return cfg->ap_info.amsdu_certify_enabled;

}


uint8  wl_psm_certify_get(struct cfg_struct *cfg)
{

    return cfg->ap_info.psm_certify_enabled;

}


uint8  wl_wmm_ac_opt_get(struct hcc_handler *hcc)
{

    return hcc->sys_wmm_ac_opt_enable;

}

/*
 * Prototype    : wl_fc_get
 * Description  : get the setting of fc param
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/28
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_fc_get(struct cfg_struct *cfg)
{
    if (IS_AP(cfg))
    {
        return cfg->ap_info.fc_enabled;
    }
    else
    {
        return cfg->sta_info.fc_enabled;
    }
}
/*
 * Prototype    : wl_hwar_get
 * Description  : get the setting of hwar param
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/28
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_hwar_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.hwar;
}
/*
 * Prototype    : wl_gmode_get
 * Description  : get the setting of gmode param
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/28
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_gmode_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.gmode;
}
/*
 * Prototype    : wl_nmode_get
 * Description  : get the setting of nmode param
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/28
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_nmode_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.nmode;
}
/*
 * Prototype    : wl_lint_get
 * Description  : get the setting of lint param
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/28
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
int32  wl_lint_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.lint;
}
/*
 * Prototype    : wl_pream_get
 * Description  : get the setting of pream param
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/28
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_pream_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.pream;
}
/*
 * Prototype    : wl_ctxbw_get
 * Description  : get the setting of ctxbw param
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/4/28
 *     Author       : hWX160629
 *     Modification : Created function
 *
 */
uint8  wl_ctxbw_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.ctxbw;
}

/*
 * Prototype    : wl_currenttxpow_get
 * Description  : get current tx power
 * Input        : struct cfg_struct *cfg
 *
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/3/21
 *     Author       : zhanglu/z00217036
 *     Modification : Created function
 *
 */
int32  wl_currenttxpow_get(struct cfg_struct *cfg)
{
    return cfg->sta_info.currenttxpow;
}


int32 wl_device_log_get(struct cfg_struct *cfg,uint8 device_log_module_id)
{
    if( WIFI_DEVICE_LOG_INVALID != cfg->device_log.device_log_flag[device_log_module_id] )
        return cfg->device_log.device_log_flag[device_log_module_id];
    else
        return WIFI_DEVICE_LOG_INVALID;

}


 int32 wl_link_statis_cfg_get(struct cfg_struct *cfg, struct stats_struct *statis)
{
    HWIFI_ASSERT(cfg != NULL);
    memcpy(statis, &cfg->conn.bss.stats, sizeof(struct stats_struct));

    return SUCC;
}


int32 hwifi_wl_config_ioctl_cmd(struct hi110x_device* hi110x_dev, struct ifreq *ifr, int32 cmd)
{
    wifi_wl_config_ioctl_data_struct ioctl_data;
    int32 ret = SUCC;

    if ((NULL == hi110x_dev) || (NULL == ifr) || (NULL == ifr->ifr_data))
    {
        HWIFI_WARNING("Invalid empty value arguments!");
        return -EFAIL;
    }

    HWIFI_PRINT_ONCE(INFO, "sizeof wifi_wl_config_ioctl_data_struct is %zu", sizeof(wifi_wl_config_ioctl_data_struct));

    if(copy_from_user(&ioctl_data,ifr->ifr_data,sizeof(wifi_wl_config_ioctl_data_struct)))
    {
        HWIFI_WARNING("Failed to copy ifr->ifr_data from user!");
        return -EFAIL;
    }

    if(ioctl_data.verify != VERIFY_CODE)
    {
        HWIFI_WARNING("ioctl verify failed,verify code is:%d(not equal %d)", ioctl_data.verify, VERIFY_CODE);
        return -EFAIL;
    }

    HWIFI_INFO("CMD:%d",ioctl_data.cmd);
    switch(ioctl_data.cmd)
    {
        case HWIFI_IOCTL_CMD_P2P_FLAG_SET:
             ret = hwifi_p2p_flag_set(hi110x_dev->cfg,ioctl_data.pri_data.p2p_on);
             break;

        case HWIFI_IOCTL_CMD_P2P_FLAG_GET:
             ioctl_data.pri_data.p2p_on = hwifi_p2p_flag_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ifr->ifr_data to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_GET_CURR_CHANNEL:
             ioctl_data.pri_data.cur_channel = hwifi_get_cur_channel(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ifr->ifr_data to user!");
                ret = -EFAIL;
             }

             break;

        case HWIFI_IOCTL_CMD_SET_AUTO_CHANNEL:
             ret = hwifi_set_auto_channel(hi110x_dev->cfg, ioctl_data.pri_data.auto_channel);
             break;

        case HWIFI_IOCTL_CMD_GET_AUTO_CHANNEL:
             ioctl_data.pri_data.auto_channel = hwifi_get_auto_channel(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }

             break;

        case HWIFI_IOCTL_CMD_RATE_SET:
             ret = wl_rate_set(hi110x_dev->cfg, ioctl_data.pri_data.rate);
             break;
        case HWIFI_IOCTL_CMD_RATE_GET:
             ioctl_data.pri_data.rate = wl_rate_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_RUNTC_SET:
             ret = wl_runtc_set(hi110x_dev->cfg, ioctl_data.pri_data.test_case);
             break;
        case HWIFI_IOCTL_CMD_RUNTC_GET:
             ioctl_data.pri_data.test_case = wl_runtc_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_MCS_SET:
             ret = wl_mcs_set(hi110x_dev->cfg, ioctl_data.pri_data.mcs);
             break;

        case HWIFI_IOCTL_CMD_MCS_GET:
             ioctl_data.pri_data.mcs = wl_mcs_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_QOS_SET:
             ret = wl_qos_set(hi110x_dev->cfg, ioctl_data.pri_data.qos_enabled);
             break;
        case HWIFI_IOCTL_CMD_QOS_GET:
             ioctl_data.pri_data.qos_enabled = wl_qos_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_RIFS_PROT_SET:
             ret = wl_rifs_prot_set(hi110x_dev->cfg, ioctl_data.pri_data.rifs_prot_enabled);
             break;

        case HWIFI_IOCTL_CMD_RIFS_PROT_GET:
             ioctl_data.pri_data.rifs_prot_enabled = wl_rifs_prot_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_ACK_POLICY_ENABLE_SET:
             ret = wl_ack_policy_set(hi110x_dev->cfg, ioctl_data.pri_data.ack_policy_enabled);
             break;

        case HWIFI_IOCTL_CMD_ACK_POLICY_ENABLE_GET:
             ioctl_data.pri_data.ack_policy_enabled = wl_ackp_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_TXOP_PROT_ENABLE_SET:
             ret = wl_txop_prot_set(hi110x_dev->cfg, ioctl_data.pri_data.txop_prot_enabled);
             break;

        case HWIFI_IOCTL_CMD_TXOP_PROT_ENABLE_GET:
             ioctl_data.pri_data.txop_prot_enabled = wl_txop_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;
        case HWIFI_IOCTL_CMD_ERP_PROT_ENABLE_SET:
             ret = wl_erp_prot_set(hi110x_dev->cfg, ioctl_data.pri_data.erp_prot);
             break;

        case HWIFI_IOCTL_CMD_ERP_PROT_ENABLE_GET:
             ioctl_data.pri_data.erp_prot = wl_erp_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_PK_MODE_SET:
             ret = wl_pk_mode_set(hi110x_dev->cfg,ioctl_data.pri_data.pk_mode);
             break;

        case HWIFI_IOCTL_CMD_PK_MODE_GET:
             ioctl_data.pri_data.pk_mode = wl_pk_mode_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_HTPROT_ENABLE_SET:
             ret = wl_htprot_set(hi110x_dev->cfg, ioctl_data.pri_data.htprot);
             break;

        case HWIFI_IOCTL_CMD_HTPROT_ENABLE_GET:
             ioctl_data.pri_data.htprot = wl_htprot_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_GI_ENABLE_SET:
             ret = wl_gi_set(hi110x_dev->cfg, ioctl_data.pri_data.gi_enabled);
             break;

        case HWIFI_IOCTL_CMD_GI_ENABLE_GET:
             ioctl_data.pri_data.gi_enabled = wl_gi_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_HT_ENABLE_SET:
             ret = wl_ht_set(hi110x_dev->cfg, ioctl_data.pri_data.ht_capability_enabled);
             break;

        case HWIFI_IOCTL_CMD_HT_ENABLE_GET:
             ioctl_data.pri_data.ht_capability_enabled = wl_ht_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_POWER_MANAGEMENT_SET:
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

        case HWIFI_IOCTL_CMD_POWER_MANAGEMENT_GET:
             ioctl_data.pri_data.pwrm = wl_pwrm_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_PROT_MODE_SET:
             ret = wl_prot_set(hi110x_dev->cfg, ioctl_data.pri_data.prot);
             break;

        case HWIFI_IOCTL_CMD_PROT_MODE_GET:
             ioctl_data.pri_data.prot = wl_prot_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_2040_COEX_SET:
             ret = wl_2040_coex_set(hi110x_dev->cfg, ioctl_data.pri_data.coex_2040_enabled);
             break;

        case HWIFI_IOCTL_CMD_2040_COEX_GET:
             ioctl_data.pri_data.coex_2040_enabled = wl_coex_2040_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_AMSDU_CERTIFY_SET:
               ret = wl_amsdu_certify_set(hi110x_dev->cfg, ioctl_data.pri_data.amsdu_certify_enabled);
               break;

        case HWIFI_IOCTL_CMD_AMSDU_CERTIFY_GET:
             ioctl_data.pri_data.amsdu_certify_enabled = wl_amsdu_certify_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_PSM_CERTIFY_SET:
               ret = wl_psm_certify_set(hi110x_dev->cfg, ioctl_data.pri_data.psm_certify_enabled);
               break;

        case HWIFI_IOCTL_CMD_WMM_AC_OPT_SET:
               ret = wl_wmm_ac_opt_set(hi110x_dev->hcc, ioctl_data.pri_data.wmm_ac_opt_enabled);
               break;

        case HWIFI_IOCTL_CMD_PSM_CERTIFY_GET:
             ioctl_data.pri_data.psm_certify_enabled = wl_psm_certify_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_WMM_AC_OPT_GET:
             ioctl_data.pri_data.wmm_ac_opt_enabled = wl_wmm_ac_opt_get(hi110x_dev->hcc);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_FC_SET:
             ret = wl_fc_set(hi110x_dev->cfg, ioctl_data.pri_data.fc_enabled);
             break;

        case HWIFI_IOCTL_CMD_FC_GET:
             ioctl_data.pri_data.fc_enabled = wl_fc_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

         case HWIFI_IOCTL_CMD_HWAR_SET:
             ret = wl_hwar_set(hi110x_dev->cfg, ioctl_data.pri_data.hwar);
             break;

         case HWIFI_IOCTL_CMD_HWAR_GET:
             ioctl_data.pri_data.hwar = wl_hwar_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

         case HWIFI_IOCTL_CMD_GMODE_SET:
             ret = wl_gmode_set(hi110x_dev->cfg, ioctl_data.pri_data.gmode);
             break;

         case HWIFI_IOCTL_CMD_GMODE_GET:
             ioctl_data.pri_data.gmode = wl_gmode_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

         case HWIFI_IOCTL_CMD_NMODE_SET:
             ret = wl_nmode_set(hi110x_dev->cfg, ioctl_data.pri_data.nmode);
             break;

         case HWIFI_IOCTL_CMD_NMODE_GET:
             ioctl_data.pri_data.nmode = wl_nmode_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_LINT_SET:
             ret = wl_lint_set(hi110x_dev->cfg, ioctl_data.pri_data.lint);
             break;

        case HWIFI_IOCTL_CMD_LINT_GET:
             ioctl_data.pri_data.lint = wl_lint_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_PREAM_SET:
             ret = wl_pream_set(hi110x_dev->cfg, ioctl_data.pri_data.pream);
             break;

        case HWIFI_IOCTL_CMD_PREAM_GET:
             ioctl_data.pri_data.pream = wl_pream_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_CTXBW_SET:
             ret = wl_ctxbw_set(hi110x_dev->cfg, ioctl_data.pri_data.ctxbw);
             break;

        case HWIFI_IOCTL_CMD_CTXBW_GET:
             ioctl_data.pri_data.ctxbw = wl_ctxbw_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_INTOL_SET:
             ret = wl_intol_set(hi110x_dev->cfg, ioctl_data.pri_data.intol);
             break;

        case HWIFI_IOCTL_CMD_INTOL_GET:
             ioctl_data.pri_data.ctxbw = wl_ctxbw_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("Failed to copy to user!");
                 ret = -EFAIL;
             }
             break;

        case HWIFI_IOCTL_CMD_STA_WMM_UAPSD_SET:
             ret = wl_set_wmm_uapsd(hi110x_dev->cfg, (uint8 *)&ioctl_data.pri_data.wmm_uapsd_cfg_param);
             break;

        case HWIFI_IOCTL_CMD_AMSDU_SET:
			HWIFI_INFO("start set amsdu");
            ret = wl_set_amsdu(hi110x_dev->cfg, (uint8 *)&ioctl_data.pri_data.aggre_param);
            break;

        case HWIFI_IOCTL_CMD_BA_SET:
            HWIFI_INFO("start set ba");
            ret = wl_set_ba(hi110x_dev->cfg, (uint8 *)&ioctl_data.pri_data.aggre_param);
            break;

        case HWIFI_IOCTL_CMD_BA_GET:
            HWIFI_INFO("query ba session set");

            ret = wl_query_ba_set(hi110x_dev->cfg, &ioctl_data.pri_data.ba_query_param);
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(ioctl_data)))
            {
                HWIFI_WARNING("copy_to_user failed");
                ret = -EFAIL;
            }
            break;

        case HWIFI_IOCTL_CMD_AMPDU_SET:
            HWIFI_INFO("start set ampdu");
            ret = wl_set_ampdu(hi110x_dev->cfg, (uint8 *)&ioctl_data.pri_data.aggre_param);
            break;

        case HWIFI_IOCTL_CMD_AGGRE_MODE_SET:
	        HWIFI_INFO("auto aggreagtion mode set.");
            ret = wl_aggre_mode_set(hi110x_dev->cfg, ioctl_data.pri_data.aggre_mode_enabled);
            break;

        case HWIFI_IOCTL_CMD_AGGRE_MODE_GET:
	        HWIFI_INFO("Auto aggreagtion mode get.");
            ret = wl_aggre_mode_get(hi110x_dev->cfg, &ioctl_data.pri_data.aggre_mode_enabled);
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;

        case HWIFI_IOCTL_CMD_AGGRE_CFG_SET:
            HWIFI_INFO("config ba session set.max_retry:%u", ioctl_data.pri_data.ba_cfg_param.max_retry );
            if(ioctl_data.pri_data.ba_cfg_param.max_retry == 255)
            {
                g_method = 1;
                g_use_dync_16vs8 = 1;
                ret = SUCC;
            }
            else if(ioctl_data.pri_data.ba_cfg_param.max_retry == 254)
            {
                g_method = 0;
                g_use_dync_16vs8 = 1;
                ret = SUCC;
            }
            else if(ioctl_data.pri_data.ba_cfg_param.max_retry == 253)
            {
                g_use_dync_16vs8 = 0;
                ret = SUCC;
            }
            else
            {
                ret = wl_aggre_cfg_set(hi110x_dev->cfg, &ioctl_data.pri_data.ba_cfg_param);
            }
            break;

        case HWIFI_IOCTL_CMD_AGGRE_CFG_GET:
            HWIFI_INFO("Config ba session get.");
            ret = wl_aggre_cfg_get(hi110x_dev->cfg, &ioctl_data.pri_data.ba_cfg_param);
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;

        case HWIFI_IOCTL_CMD_LINK_STATIS_GET:
            HWIFI_INFO("Link statis get.");
            ret = wl_link_statis_cfg_get(hi110x_dev->cfg, &ioctl_data.pri_data.link_statis);
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;

        case HWIFI_IOCTL_CMD_DC_OFFSET_SET:
            HWIFI_INFO("hwifi dc offset configs set.");
            ret = wl_dc_offset_set(hi110x_dev->cfg, &ioctl_data.pri_data.dc_param);
            break;

        case HWIFI_IOCTL_CMD_DC_OFFSET_GET:
            HWIFI_INFO("Hwifi dc offset configs get.");

            ret = wl_dc_offset_get(hi110x_dev->cfg, &ioctl_data.pri_data.dc_param);
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;

#ifdef __ROAM__
        case HWIFI_IOCTL_CMD_ROAM_PARAM:
        {
            int32 get = FALSE;
            HWIFI_INFO("Roam param.");

            ret = roam_alg_param(hi110x_dev->cfg,
                        ioctl_data.pri_data.roam_param.type,
                        &ioctl_data.pri_data.roam_param.value,
                        &get);
            if (ret != SUCC)
            {
                ret = -EFAIL;
                break;
            }

            if (TRUE == get)
            {
                if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
                {
                    HWIFI_WARNING("Failed to copy to user!");
                    ret = -EFAIL;
                }
            }
        }
            break;
#endif

    #ifdef MAC_802_11W
        case HWIFI_IOCTL_CMD_PMF_PARAM:
        {
            int32 get = FALSE;

            HWIFI_INFO("Start to set pmf params.");

            ret = pmf_param(hi110x_dev->cfg,
                        ioctl_data.pri_data.pmf_param.type,
                        &ioctl_data.pri_data.pmf_param.value,
                        &get);
            if (ret != SUCC)
            {
                ret = -EFAIL;
                HWIFI_WARNING("Failed to pmf param!");
                break;
            }

            if (TRUE == get)
            {
                if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
                {
                    HWIFI_WARNING("Failed to copy to user!");
                    ret = -EFAIL;
                }
            }
        }
            break;
    #endif

        case HWIFI_IOCTL_CMD_AP_WMM_UAPSD_SET:
            HWIFI_INFO("set ap wmm uapsd");
            ret = wl_set_wmm_ap_uapsd(hi110x_dev->cfg, ioctl_data.pri_data.ap_wmm_enable);
            break;

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
        case HWIFI_IOCTL_CMD_AGGRE_FUNC_SET:
            HWIFI_INFO("Set aggre function");
            ret = wl_set_aggre_func(hi110x_dev->cfg, ioctl_data.pri_data.aggre_func_enabled);
            break;

        case HWIFI_IOCTL_CMD_AGGRE_FUNC_GET:
            HWIFI_INFO("Get aggre function");
            ret = wl_get_aggre_func(hi110x_dev->cfg, &ioctl_data.pri_data.aggre_func_enabled);
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;

        case HWIFI_IOCTL_CMD_AGGRE_MAX_RATE:
            HWIFI_INFO("Aggre max rate set to %d", ioctl_data.pri_data.max_rate);
            if ((ioctl_data.pri_data.max_rate * 1000) < g_data_rate_down)
            {
                HWIFI_WARNING("Too small aggre max rate, please reset!");
                ret = -EFAIL;
            }
            else
            {
                g_data_rate_up = ioctl_data.pri_data.max_rate * 1000;
            }
            break;

        case HWIFI_IOCTL_CMD_AGGRE_MIN_RATE:
            HWIFI_INFO("Aggre min rate set to %d", ioctl_data.pri_data.min_rate);
            if ((ioctl_data.pri_data.min_rate * 1000) > g_data_rate_up)
            {
                HWIFI_WARNING("Too big aggre min rate, please reset!");
                ret = -EFAIL;
            }
            else
            {
                g_data_rate_down = ioctl_data.pri_data.min_rate * 1000;
            }
            break;
#endif

        case HWIFI_IOCTL_CMD_DEV_ERROR_SET:
            HWIFI_INFO("dev error set to %d", ioctl_data.pri_data.dev_error_enabled);
            exception_enable = ioctl_data.pri_data.dev_error_enabled;
            break;

        case HWIFI_IOCTL_CMD_DEV_ERROR_GET:
            HWIFI_INFO("get dev error function status");
            ioctl_data.pri_data.dev_error_enabled = exception_enable;
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                HWIFI_WARNING("copy_to_user failed");
                ret = -EFAIL;
            }
            break;

#ifdef WLAN_PERFORM_DEBUG
        case HWIFI_IOCTL_DATA_PERFORM_SET:
            printk("version is %d.%d, date is 2013 04 26, time is 10:46 clock", g_wlan_host_version/100, g_wlan_host_version%100);
            ret = SUCC;
            wlan_set_and_enable_perform_debugging(hi110x_dev, (uint8 *)&ioctl_data.pri_data.wlan_perform_cfg_interface);
            break;

        case HWIFI_IOCTL_DATA_PERFORM_SWITCH_SET:
            printk("version is %d.%d, date is 2013 04 01, time is 10:46 clock", g_wlan_host_version/100, g_wlan_host_version%100);
            ret = SUCC;
            wlan_disable_perform_debugging(hi110x_dev,(uint8 *)&ioctl_data.pri_data.wlan_perform_cfg_interface);
            break;
#endif

        case HWIFI_IOCTL_CMD_SDIOTEST_SET:
             /*Support sdio tx/loop test*/
            if(hsdio_test_set_case(&ioctl_data.pri_data.sdiotest))
            {
                HWIFI_ERROR("sdio test failed!");
                ret = -EFAIL;
            }
            else
            {
                if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
                {
                    HWIFI_WARNING("copy_to_user failed");
                    ret = -EFAIL;
                }
                HWIFI_INFO("sdio test done.");
            }
            break;

        case HWIFI_IOCTL_CMD_SDIOTEST_GET:
            /*Return test result*/
            hsdio_test_get_case(&ioctl_data.pri_data.sdiotest);
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                HWIFI_WARNING("copy_to_user failed");
                ret = -EFAIL;
            }
            break;
        case HWIFI_IOCTL_CMD_LOGLEVEL_SET:
             HWIFI_INFO("Set current loglevel : %d", ioctl_data.pri_data.loglevel);
             loglevel = ioctl_data.pri_data.loglevel;
             break;
        case HWIFI_IOCTL_CMD_LOGLEVEL_GET:
             HWIFI_INFO("Get current loglevel : %d",loglevel);
             ioctl_data.pri_data.loglevel = loglevel;
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
             }
            break;
        case HWIFI_IOCTL_CMD_RATE_40M_SET:
             HWIFI_INFO("Set g_sdio_rate_40M : %d", ioctl_data.pri_data.rate_40M);
             g_sdio_rate_40M = ioctl_data.pri_data.rate_40M;
             break;
        case HWIFI_IOCTL_CMD_RATE_40M_GET:
             HWIFI_INFO("Get g_sdio_rate_40M : %d",g_sdio_rate_40M);
             ioctl_data.pri_data.rate_40M = g_sdio_rate_40M;
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
             }
            break;
        case HWIFI_IOCTL_CMD_RATE_80M_SET:
             HWIFI_INFO("Set g_sdio_rate_80M : %d", ioctl_data.pri_data.rate_80M);
             g_sdio_rate_80M = ioctl_data.pri_data.rate_80M;
             break;
        case HWIFI_IOCTL_CMD_RATE_80M_GET:
             HWIFI_INFO("Get g_sdio_rate_80M : %d",g_sdio_rate_80M);
             ioctl_data.pri_data.rate_80M = g_sdio_rate_80M;
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
             }
            break;
        case HWIFI_IOCTL_CMD_RATE_160M_SET:
             HWIFI_INFO("Set g_sdio_rate_160M : %d", ioctl_data.pri_data.rate_160M);
             g_sdio_rate_160M = ioctl_data.pri_data.rate_160M;
             break;
        case HWIFI_IOCTL_CMD_RATE_160M_GET:
             HWIFI_INFO("Get g_sdio_rate_160M : %d",g_sdio_rate_160M);
             ioctl_data.pri_data.rate_160M = g_sdio_rate_160M;
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
             }
            break;

       case HWIFI_IOCTL_CMD_ASSEMBLE_COUNT_SET:
            HWIFI_INFO("Set hcc_assemble_count : %d", ioctl_data.pri_data.assemble_count);
            hcc_assemble_count = ioctl_data.pri_data.assemble_count;
            break;
       case HWIFI_IOCTL_CMD_ASSEMBLE_COUNT_GET:
            HWIFI_INFO("Get hcc_assemble_count : %d",hcc_assemble_count);
             ioctl_data.pri_data.assemble_count = hcc_assemble_count;
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
             }
            break;
       case HWIFI_IOCTL_CMD_PANIC_SET:
            HWIFI_INFO("Set panic_debug : %d", ioctl_data.pri_data.panic);
            panic_debug = ioctl_data.pri_data.panic;
            break;
       case HWIFI_IOCTL_CMD_PANIC_GET:
            HWIFI_INFO("Get panic_debug : %d",panic_debug);
             ioctl_data.pri_data.panic = panic_debug;
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
             {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
             }
            break;

       case HWIFI_IOCTL_CMD_PS_SWITCH_SET:
            HWIFI_INFO("Set g_powermgmt_switch : %d", ioctl_data.pri_data.ps_switch);
            g_powermgmt_switch = ioctl_data.pri_data.ps_switch;
            break;
       case HWIFI_IOCTL_CMD_PS_SWITCH_GET:
            HWIFI_INFO("Get g_powermgmt_switch : %d",g_powermgmt_switch);
            ioctl_data.pri_data.ps_switch = g_powermgmt_switch;
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
            }
            break;
       case HWIFI_IOCTL_CMD_WATCHDOG_TIMEOUT_SET:
            HWIFI_INFO("Set g_watchdog_timeout : %d", ioctl_data.pri_data.watchdog_timeout);
            g_watchdog_timeout = ioctl_data.pri_data.watchdog_timeout;
            break;
       case HWIFI_IOCTL_CMD_WATCHDOG_TIMEOUT_GET:
            HWIFI_INFO("Get g_watchdog_timeout : %d",g_watchdog_timeout);
            ioctl_data.pri_data.watchdog_timeout = g_watchdog_timeout;
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
            }
            break;
       case HWIFI_IOCTL_CMD_DUMP_SET:
            HWIFI_INFO("Set log_dump_flag : %d", ioctl_data.pri_data.dump);
            log_dump_flag = ioctl_data.pri_data.dump;
            break;
       case HWIFI_IOCTL_CMD_DUMP_GET:
            HWIFI_INFO("Get log_dump_flag : %d",log_dump_flag);
            ioctl_data.pri_data.dump = log_dump_flag;
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
            }
            break;
       case HWIFI_IOCTL_CMD_CPUFREQ_SET:
            HWIFI_INFO("Set g_cur_cpufreq : %d", ioctl_data.pri_data.cpufreq);
            ret = hwifi_fit_cpufreq(g_hi110x_dev->cfg, ioctl_data.pri_data.cpufreq);
            g_cur_cpufreq = ioctl_data.pri_data.cpufreq;
            break;
       case HWIFI_IOCTL_CMD_CPUFREQ_GET:
            HWIFI_INFO("Get g_cur_cpufreq : %d",g_cur_cpufreq);
            ioctl_data.pri_data.cpufreq = g_cur_cpufreq;
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                 HWIFI_WARNING("copy_to_user failed");
                 ret = -EFAIL;
            }
            break;
        case HWIFI_IOCTL_CMD_CURRENTTXPOW_SET:
            HWIFI_INFO("Set current_tx_pow : %d", ioctl_data.pri_data.currenttxpow);
            ret = wl_currenttxpow_set(hi110x_dev->cfg, ioctl_data.pri_data.currenttxpow);
            break;
        case HWIFI_IOCTL_CMD_CURRENTTXPOW_GET:
            ioctl_data.pri_data.currenttxpow = wl_currenttxpow_get(hi110x_dev->cfg);
            if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
            {
                HWIFI_WARNING("Failed to copy to user!");
                ret = -EFAIL;
            }
            break;
        case HWIFI_IOCTL_CMD_DEVICE_LOG_SET:
            HWIFI_INFO("Set device log level,device_log_module_id: %d,log_control:%d", ioctl_data.pri_data.device_log.device_log_module_id,ioctl_data.pri_data.device_log.log_control);
            ret = wl_device_log_set(hi110x_dev->cfg,ioctl_data.pri_data.device_log.device_log_module_id,ioctl_data.pri_data.device_log.log_control);
            break;
        case HWIFI_IOCTL_CMD_DEVICE_LOG_GET:
            if( (ioctl_data.pri_data.device_log.device_log_module_id < 0) || (ioctl_data.pri_data.device_log.device_log_module_id > 62))
            {
                HWIFI_WARNING("device_log_module_id is out of range!");
                ret = -EFAIL;
            }
            ioctl_data.pri_data.device_log.log_control= wl_device_log_get(hi110x_dev->cfg,ioctl_data.pri_data.device_log.device_log_module_id);
            if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wl_config_ioctl_data_struct)))
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



