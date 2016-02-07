


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
#include <linux/ieee80211.h>
#include <net/cfg80211.h>

#include "hi110x.h"
#include "hwifi_event.h"
#include "hwifi_cfg80211.h"
#include "hwifi_regdomain.h"
#include "hwifi_android.h"
#include "hi110x_pm.h"
#include "hwifi_netdev.h"

/*
 * 2 Global Variable Definition
 */
const int8 *g_android_hwifi_priv_cmd[ANDROID_WIFI_CMD_MAX] = {
    "START",
    "STOP",
    "SCAN-ACTIVE",
    "SCAN-PASSIVE",
    "SETBAND",
    "GETBAND",
    "COUNTRY",/* android use "COUNTRY" cmd */
    "GETCOUNTRY",/* new defined, for wifi regulatory domain utils */
#ifdef CONFIG_BT_COEX
    "BTCOEXMODE",/*Android Set wifi-bt coexistence mode*/
    "WIBTCOEX",/* coexistence for WIFI operation */
    "BTWIFICOEX",/* coexistence for WIFI connect */
#endif
    "P2P_SET",
    "LTECOEX",
};

/*
 * 3 Function Definition
 */

int32 hwifi_android_set_country_cmd(struct cfg_struct *cfg, const uint8 *command)
{
    const uint8        *country_code;
    int32               ret;

    HWIFI_ASSERT(NULL != cfg);
    HWIFI_ASSERT(NULL != cfg->wiphy);
    HWIFI_ASSERT(NULL != command);

    country_code = command + strlen("COUNTRY") + 1;
    HWIFI_INFO("country_code=%c%c, curr_mode=%d", country_code[0], country_code[1], cfg->hi110x_dev->mode.current_mode);
    strncpy(cfg->sim_country_code, country_code, COUNTRY_CODE_LEN);

    /* check if connected or connecting,scanning */
    if ((HWIFI_CONNECTED == cfg->conn.status)
        || (HWIFI_CONNECTING == cfg->conn.status)
        || (TRUE == cfg->scan.is_scanning)
        || (TRUE == cfg->beacon_set))
    {
        HWIFI_WARNING("Cfg80211 work at one of scanning, connected,connecting,ap mode,changing interface mode.");
        return -EFAIL;
    }

    /* check current country code */
    ret = hwifi_regdomain_update(cfg);
    if (ret != SUCC)
    {
        HWIFI_WARNING("Failed to set country code!");
        return -EFAIL;
    }

    return SUCC;
}


int32 hwifi_android_priv_cmd(struct net_device *ndev, struct ifreq *ifr, int cmd)
{
    void __user * command_buf = NULL;
    struct cfg_struct          *cfg;
    struct wiphy               *wiphy;
    uint8                      *command;
    android_wifi_priv_cmd      priv_cmd;
    int32                       written_number = 0;
    uint32                      i;
    int32                       ret = SUCC;
#ifdef CONFIG_BT_COEX
    uint                        mode = 0;
#endif
    int32                       p2p_flag;
    int8                        lte_paras;


    HWIFI_ASSERT(NULL != ndev);
    HWIFI_ASSERT(NULL != ifr);
    HWIFI_ASSERT(NULL != ifr->ifr_data);

    HWIFI_PRINT_ONCE(INFO, "sizeof android_wifi_priv_cmd is %zu\n", HWIFI_PRIV_CMD_HDR_LEN());

    if (copy_from_user(&priv_cmd, ifr->ifr_data, HWIFI_PRIV_CMD_HDR_LEN()))
    {
        HWIFI_WARNING("Failed to copy ifr_data from user to priv_cmd!");
        return -EFAULT;
    }

    /*Android Private Command had adpter to arm64, so check the caller.*/
    if(unlikely(priv_cmd.total_len <= 0 || priv_cmd.total_len > PAGE_SIZE))
    {
        HWIFI_ERROR("Error private command length:%d", priv_cmd.total_len);
        return -EFAULT;
    }

    command = kmalloc(priv_cmd.total_len, GFP_KERNEL);
    if (NULL == command)
    {
        HWIFI_WARNING("Failed to alloc command!");
        return -ENOMEM;
    }

    command_buf =HWIFI_PRIV_CMD_DATA(ifr->ifr_data);
    if (copy_from_user(command, command_buf, priv_cmd.total_len))
    {
        HWIFI_WARNING("Failed to copy priv_cmd.buf from user to command!");
        kfree(command);

        return -EFAULT;
    }

    wiphy = ndev->ieee80211_ptr->wiphy;
    if (NULL == wiphy)
    {
        HWIFI_WARNING("Invalid NULL wiphy!");
        kfree(command);

        return -EFAIL;
    }

    /* get cfg struct handle */
    cfg = wiphy_priv(wiphy);
    if (NULL == cfg)
    {
        HWIFI_WARNING("Invalid NULL cfg!");
        kfree(command);

        return -EFAIL;
    }

    HWIFI_INFO("Android private cmd \"%s\" on %s", command, ifr->ifr_name);

    for (i = 0; i < ANDROID_WIFI_CMD_MAX; i++)
    {
        if (0 == strncmp(command, g_android_hwifi_priv_cmd[i], strlen(g_android_hwifi_priv_cmd[i])))
        {
            break;
        }
    }

    switch (i)
    {
        /* TBD:android priv cmd */
        case ANDROID_WIFI_CMD_START:

            break;

        case ANDROID_WIFI_CMD_STOP:

            break;

        case ANDROID_WIFI_CMD_SCAN_ACTIVE:
        case ANDROID_WIFI_CMD_SCAN_PASSIVE:
        case ANDROID_WIFI_CMD_SETBAND:
        case ANDROID_WIFI_CMD_GETBAND:
            break;
        case ANDROID_WIFI_CMD_SETCOUNTRY:
            written_number = hwifi_android_set_country_cmd(cfg, command);
            break;
        case ANDROID_WIFI_CMD_GETCOUNTRY:
            command[0] = cfg->current_country_code[0];
            command[1] = cfg->current_country_code[1];
            command[2] = '\0';
            written_number = strlen(command);
            break;
#ifdef CONFIG_BT_COEX
        case ANDROID_WIFI_CMD_BTCOEXMODE:
            mode = *(command + strlen(g_android_hwifi_priv_cmd[i]) + 1) - '0';
            if(btcoex_set_dhcp_mode(mode, 2500, 5500))
            {
                HWIFI_WARNING("Failed to set btcoex dhcp mode!");
            }
            HWIFI_DEBUG("CMD_BTCOEXMODE[%u]", mode);
            snprintf(command, 3, "OK");
            written_number = strlen("OK");
            break;

        case ANDROID_WIFI_CMD_WIBTCOEX:
            mode = *(command + strlen(g_android_hwifi_priv_cmd[i]) + 1) - '0';
            if(btcoex_set_dhcp_mode(mode, 0, 5000))
            {
                HWIFI_WARNING("Failed to set bt coex mode[%u] instantly !", mode);
            }
            HWIFI_DEBUG("CMD_WIBTCOEX[%u]", mode);
            snprintf(command, 3, "OK");
            written_number = strlen("OK");
            break;

        case ANDROID_WIFI_CMD_BTWIFICOEX:
            mode = *(command + strlen(g_android_hwifi_priv_cmd[i]) + 1) - '0';
            HWIFI_DEBUG("set bt_wifi_coex to %d", mode);
            set_coex_param(mode, 1, 10000);
            snprintf(command, 3, "OK");
            written_number = strlen("OK");
            break;
#endif
        case ANDROID_WIFI_CMD_P2P_FLAG_SET:
            p2p_flag = *(command + strlen(g_android_hwifi_priv_cmd[i]) + 1) - '0';
            HWIFI_INFO("p2p flag is %d", p2p_flag);
            ret = hwifi_p2p_flag_set(cfg, p2p_flag);
            break;

        case ANDROID_WIFI_CMD_LTE_COEX_PARAS:
            lte_paras = *(command + strlen(g_android_hwifi_priv_cmd[i]) + 1) - '0';
            HWIFI_INFO("lte_coex flag is %d", lte_paras);
            ret = hwifi_set_lte_coex_paras(cfg, lte_paras);
            break;

        default:
            HWIFI_INFO("Unknown android priv cmd: %s ignored", command);
            snprintf(command, 3, "OK");
            written_number = strlen("OK");
            break;
    }

    if (written_number >= 0)
    {
        if (written_number == 0)
        {
            command[0] = '\0';
        }

        if (written_number > priv_cmd.total_len)
        {
            written_number = priv_cmd.total_len;
        }
        else
        {
            written_number++;
        }

        priv_cmd.used_len = written_number;
        if (copy_to_user(command_buf, command, written_number))
        {
            HWIFI_WARNING("Failed to copy priv_cmd.buf to user");
            kfree(command);

            return -EFAULT;
        }
    }
    else
    {
        ret = written_number;
    }

    if (command)
    {
        kfree(command);
    }

    return ret;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

