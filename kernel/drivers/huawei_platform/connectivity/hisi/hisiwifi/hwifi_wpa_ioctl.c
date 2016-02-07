


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

#include "hi110x.h"
#include "hwifi_wpa_ioctl.h"
#include "hwifi_cfgapi.h"
#include "hwifi_msg.h"
#include "hi110x_pm.h"
#include "cfg80211_stru.h"
#include "hwifi_utils.h"
#include "hwifi_cfg80211.h"
#include "hwifi_netdev.h"
#include "hwifi_dev_err.h"

#ifdef __ROAM__ 
#include "hwifi_roam_main.h"  
#include "hwifi_roam_fsm.h"
#endif

/*
 * 2 Global Variable Definition
 */

/*
 * 3 Function Definition
 */

static  int  ie_data_total_len( wifi_wpa_ioctl_data_struct* ioctl_data)
{
    int i;
    int ret = 0;
    int len_t =0;
    for(i = 0; i < P2P_WPS_IE_NUM; i++)
    {
        len_t = ioctl_data->pri_data.p2p_wps_ie.ie_len[i];
        if(likely(len_t >= 0))
        {
            ret += len_t;
        }            
        else
        {
            HWIFI_WARNING("invaild ie len:%d, ie type:%d", len_t, i);
            ret = 0;
            break;
        }
    }
	if(!ret)
		HWIFI_INFO("empty ie");
    
    return ret;
}


STATIC int32 set_p2p_wps_ie(wifi_wpa_ioctl_data_struct *ioctl_data, struct ifreq *ifr, struct hi110x_device *hi110x_dev)
{
    int32  ret;
    int32 buf_offset = 0;
    int32 ie_buff_len;
    uint8 *ie_buff;
    void * __user ie_offset_user;
    uint8 *beacon_ie;
    int32  beacon_ie_len;
    uint8 *proberesp_ie;
    int32  proberesp_ie_len;
    uint8 *assocresp_ie;
    int32  assocresp_ie_len;

    beacon_ie_len    = ioctl_data->pri_data.p2p_wps_ie.ie_len[BEACON_IE];
    proberesp_ie_len = ioctl_data->pri_data.p2p_wps_ie.ie_len[PROB_RSP_IE];
    assocresp_ie_len = ioctl_data->pri_data.p2p_wps_ie.ie_len[ASSOC_RSP_IE];
#if 0
    HWIFI_INFO("beacon len:%d, probe len:%d, assoc len:%d",beacon_ie_len,
                        proberesp_ie_len,assocresp_ie_len);
#endif                        
    ie_buff_len = ie_data_total_len(ioctl_data);
#if 0    
    if(0 == ie_buff_len)
    {
        return -EFAIL;
    }
#endif    

    ie_buff = OS_MALLOC_GFP(ie_buff_len);
    if(NULL == ie_buff)
    {
        HWIFI_WARNING("alloc  buff  failed  buff_len:%d", ie_buff_len);
        return -EFAIL;
    }

    ie_offset_user = ((wifi_wpa_ioctl_data_struct*)(ifr->ifr_data))->pri_data.p2p_wps_ie.ie_buff;

    ret = copy_from_user(ie_buff,  ie_offset_user, ie_buff_len);
    if(ret)
    {
        HWIFI_WARNING("copy ie buff from user space failed=%d, buff_len:%d", ret, ie_buff_len);
        ret = -EFAIL;
        goto fail_copy_ie_buff;
    }

    beacon_ie = ie_buff+ buf_offset;
    buf_offset += beacon_ie_len;

    proberesp_ie = ie_buff+ buf_offset;
    buf_offset += proberesp_ie_len;

    assocresp_ie = ie_buff+ buf_offset;
    buf_offset += assocresp_ie_len;    

    /* 设置 IE信息 到device侧 */
    ret = wl_set_p2p_ie(hi110x_dev->cfg, beacon_ie, (uint16)beacon_ie_len,
                        proberesp_ie, (uint16)proberesp_ie_len,
                        assocresp_ie, (uint16)assocresp_ie_len);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to set p2p ie to device!");
    }
    
    OS_MEM_FREE(ie_buff);
    return ret;
    
fail_copy_ie_buff:
    OS_MEM_FREE(ie_buff);
    return ret;
}
STATIC int32 get_sta_assoc_ie(struct hi110x_device *hi110x_dev, wifi_wpa_ioctl_data_struct *ioctl_data, struct ifreq *ifr)
{
    uint8 *mac;
    uint32 buf_size;
    void __user *user_buf;

    struct {
        uint32 len;
        uint8  val[0];
    } *assoc_req;

    mac = (uint8 *)ioctl_data->pri_data.assoc_req_ie.mac;
    buf_size = ioctl_data->pri_data.assoc_req_ie.buf_size;
    
    user_buf = ((wifi_wpa_ioctl_data_struct*)(ifr->ifr_data))->pri_data.assoc_req_ie.buf;

    assoc_req = hwifi_get_sta_req_ie(hi110x_dev->cfg, mac);
    if (NULL == assoc_req)
    {
        HWIFI_WARNING("Failed to get sta assoc request ie!");
        return -EFAIL;
    }

    if (assoc_req->len > buf_size - sizeof(assoc_req->len))
    {
        assoc_req->len = buf_size - sizeof(assoc_req->len);
    }

    if(copy_to_user(user_buf, assoc_req, assoc_req->len + sizeof(assoc_req->len)))
    {
        HWIFI_WARNING("Failed to copy to user!");
        return -EFAIL;
    }

    return SUCC;
}

/*
 * Prototype    : get_p2p_noa
 * Description  : get p2p noa attr
 * Input        : None
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/12/15
 *     Author       : xudayong
 *     Modification : Created function
 *
 */
int32 get_p2p_noa(struct hi110x_device *hi110x_dev, wifi_wpa_ioctl_data_struct *ioctl_data, struct ifreq *ifr)
{
    uint32       buf_len;
    uint32       used_len;
    void __user *noa_buf;
    uint8       *buf;

    buf_len  = ioctl_data->pri_data.buf.buf_len;
    noa_buf = ((wifi_wpa_ioctl_data_struct*)(ifr->ifr_data))->pri_data.buf.noa_buf;

    buf = OS_ZMALLOC_GFP(buf_len);

    used_len = wl_get_p2p_noa(hi110x_dev->cfg, buf, (uint8)buf_len);
    ioctl_data->pri_data.buf.used_len = used_len;

    if(copy_to_user(ifr->ifr_data, ioctl_data, sizeof(wifi_wpa_ioctl_data_struct)))
    {
        HWIFI_WARNING("Failed to copy used len!");
        OS_MEM_FREE(buf);
        return -EFAIL;        
    }

    if(copy_to_user(noa_buf, buf, used_len))
    {
        HWIFI_WARNING("Failed to copy noa buf!");
        OS_MEM_FREE(buf);
        return -EFAIL;
    }
    OS_MEM_FREE(buf);
    return SUCC;
}



int32 hwifi_wpa_ioctl_cmd(struct hi110x_device* hi110x_dev, struct ifreq *ifr, int32 cmd)
{
    wifi_wpa_ioctl_data_struct ioctl_data;
#ifdef __ROAM__ 
    struct roam_all_stru *roam_info;
#endif
    int32 excp_stat;
    int32 ret = SUCC;

    if ((NULL == hi110x_dev) || (NULL == ifr) || (NULL == ifr->ifr_data))
    {
        HWIFI_WARNING("Invalid empty value arguments!");
        return -EFAIL;
    }
    
    HWIFI_PRINT_ONCE(INFO, "sizeof wifi_wpa_ioctl_data_struct is %zu", sizeof(wifi_wpa_ioctl_data_struct));
    
    if(copy_from_user(&ioctl_data,ifr->ifr_data,sizeof(wifi_wpa_ioctl_data_struct)))
    {
        HWIFI_WARNING("Failed to copy ifr->ifr_data from user!");
        return -EFAIL;
    }

    if(ioctl_data.verify != VERIFY_CODE)
    {
        HWIFI_WARNING("ioctl verify failed,verify code is:%d(not equal %d)", ioctl_data.verify, VERIFY_CODE);
        return -EFAIL;    
    }

    HWIFI_DEBUG("CMD:%d",ioctl_data.cmd);

    switch(ioctl_data.cmd)
    {
        case HWIFI_WPA_IOCTL_CMD_P2P_FLAG_SET:
             HWIFI_INFO("set p2p flag:%d.", ioctl_data.pri_data.p2p_on);
             ret = hwifi_p2p_flag_set(hi110x_dev->cfg,ioctl_data.pri_data.p2p_on);
             break;

        case HWIFI_WPA_IOCTL_CMD_P2P_FLAG_GET:
             ioctl_data.pri_data.p2p_on = hwifi_p2p_flag_get(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wpa_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ifr->ifr_data to user!");
                ret = -EFAIL;
             }
             break;

        case HWIFI_WPA_IOCTL_CMD_SET_WPS_P2P_IE:
             ret = set_p2p_wps_ie(&ioctl_data, ifr, hi110x_dev);
             break;

        case HWIFI_WPA_IOCTL_CMD_GET_STA_ASSOC_REQ_IE:
             ret = get_sta_assoc_ie(hi110x_dev, &ioctl_data, ifr);
             break;

        case HWIFI_WPA_IOCTL_CMD_SET_AUTH_ALG:
             ret = hwifi_set_auth_alg(hi110x_dev->cfg, ioctl_data.pri_data.auth_algs);
             break;

        case HWIFI_WPA_IOCTL_CMD_SET_HW_MODE:
             ret = hwifi_set_hw_mode(hi110x_dev->cfg, ioctl_data.pri_data.hw_mode);
             break;

        case HWIFI_WPA_IOCTL_CMD_SET_HT_ENABLE:
             ret = hwifi_set_ht_enabled(hi110x_dev->cfg, ioctl_data.pri_data.ht_enabled);
             break;

        case HWIFI_WPA_IOCTL_CMD_SET_SSID:
             ret = hwifi_set_ssid(hi110x_dev->cfg, ioctl_data.pri_data.ssid);
             break;

        case HWIFI_WPA_IOCTL_CMD_GET_CURR_CHANNEL:
             ioctl_data.pri_data.cur_channel = hwifi_get_cur_channel(hi110x_dev->cfg);
             if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wpa_ioctl_data_struct)))
             {
                HWIFI_WARNING("Failed to copy ifr->ifr_data to user!");
                ret = -EFAIL;
             }

             break;

        case HWIFI_WPA_IOCTL_CMD_P2P_CTWINDOW:
             ret = wl_set_p2p_ps(hi110x_dev->cfg,
                                ioctl_data.pri_data.ct.ctwindow,
                                ioctl_data.pri_data.ct.opps,
                                ioctl_data.pri_data.ct.legacy_ps);
             break;

        case HWIFI_WPA_IOCTL_CMD_P2P_NOA:
             ret = wl_set_p2p_noa(hi110x_dev->cfg,
                                 ioctl_data.pri_data.noa.count,
                                 ioctl_data.pri_data.noa.start,
                                 ioctl_data.pri_data.noa.duration);
             break;

        case HWIFI_WPA_IOCTL_CMD_GET_NOA:
             ret = get_p2p_noa(hi110x_dev, &ioctl_data, ifr);
             break;

        case HIWFI_WPA_IOCTL_CMD_SET_STA_NUM:
            ret = wl_set_max_sta_num(hi110x_dev->cfg, ioctl_data.pri_data.max_num);
            break;

        case HWIFI_WPA_IOCTL_CMD_TKIP_MEASURE:
            HWIFI_INFO("Set tkip measure flags");
            ret = hwifi_set_tkip_measure(hi110x_dev->cfg, ioctl_data.pri_data.tkip_measure_enable);
            break;

#ifdef WLAN_POWER_MANAGEMENT
        case HWIFI_WPA_IOCTL_CMD_POWERMGMT_ON:
            if (hi110x_dev->pm_info)
            {
                HWIFI_INFO("set power management flags");
                hi110x_dev->pm_info->powermgmt_state = ioctl_data.pri_data.powermgmt_on;
            }
            else
            {
                HWIFI_WARNING("set powermgmt cmd failed, hi110x_dev->pm_info is NULL");
            }

			break;

        case HWIFI_WPA_IOCTL_CMD_AP_POWER_OFF:
            HWIFI_INFO("ap mode power off");
            wlan_opera_lock(hi110x_dev->pm_info);
            /*before close wifi we must disable exception work.*/
            excp_stat = hwifi_exception_enable_set(hi110x_dev->hcc, FALSE);
            cancel_work_sync(&hi110x_dev->hcc->excp_worker);
            hwifi_exception_enable_set(hi110x_dev->hcc, excp_stat);
            ret = hwifi_wlan_close(hi110x_dev->netdev);
            wlan_opera_unlock(hi110x_dev->pm_info);
            hi110x_wake_unlock(hi110x_dev->pm_info);

            hi110x_wakelocks_release_detect(hi110x_dev->pm_info);

            break;

	    case HWIFI_WPA_IOCTL_CMD_AP_POWER_ON:
                HWIFI_INFO("ap mode power on");
                hi110x_wake_lock(hi110x_dev->pm_info);
                wlan_opera_lock(hi110x_dev->pm_info);
                ret = hwifi_wlan_open(hi110x_dev->netdev);
                wlan_opera_unlock(hi110x_dev->pm_info);

            break;

		case HWIFI_WPA_IOCTL_CMD_STA_IFACE_STATE_SET:
	        HWIFI_INFO("process STA mode iface state from wpa_supplciant");
	        ret = hwifi_sm_power_handle(hi110x_dev, ioctl_data.pri_data.iface_state, false);

	        break;

	    case HWIFI_WPA_IOCTL_CMD_P2P_IFACE_STATE_SET:
	        HWIFI_INFO("process P2P mode iface state from wpa_supplciant");
	        ret = hwifi_sm_power_handle(hi110x_dev, ioctl_data.pri_data.iface_state, true);

	        break;

	    case HWIFI_WPA_IOCTL_CMD_SCREEN_STATE_GET:
	        HWIFI_DEBUG("transfer screen state to wpa_supplicant");

			HWIFI_ASSERT(hi110x_dev->pm_info != NULL);
            ioctl_data.pri_data.screen_state = hi110x_get_screen_state(hi110x_dev->pm_info);
            if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wpa_ioctl_data_struct)))
            {
                HWIFI_WARNING("screen state copy_to_user failed");
                ret = -EFAIL;
            }

            break;

        case HWIFI_WPA_IOCTL_CMD_CUR_MODE_GET:
            HWIFI_DEBUG("transfer cur mode to wpa_supplicant");

            HWIFI_ASSERT(hi110x_dev->pm_info != NULL);
            ioctl_data.pri_data.cur_mode = hwifi_get_cur_mode(hi110x_dev->cfg);
            if(copy_to_user(ifr->ifr_data,&ioctl_data,sizeof(wifi_wpa_ioctl_data_struct)))
            {
                HWIFI_WARNING("current mode copy_to_user failed");
                ret = -EFAIL;
            }

            break;
        
#ifdef __ROAM__         
        case HWIFI_WPA_IOCTL_CMD_ROAM_COMPLETE_SET:
        
            HWIFI_ASSERT(hi110x_dev->cfg != NULL);
            
            roam_info = &hi110x_dev->cfg->roam;
            
            /* wpa_supplicant认为漫游成功，此时发送缓存数据 */
            roam_process_addkey(roam_info);

            /* 漫游成功后,最新信号为当前AP的信号值 */
            hi110x_dev->cfg->latest_signal = roam_info->roam_fsm.target_ap_rssi;
            hi110x_dev->cfg->conn.bss.stats.signal = roam_info->roam_fsm.target_ap_rssi;
            HWIFI_DEBUG("roam complete update latest_signal=%d", hi110x_dev->cfg->latest_signal);

#ifdef WLAN_POWER_MANAGEMENT
            HWIFI_INFO("roam complete flag=%d,power_manager_flag=%d",
                        ioctl_data.pri_data.roam_complete_flag, roam_info->power_manager_flag);

            if(roam_info->power_manager_flag)
            {
                HWIFI_INFO("roam end,roam success.");
                roam_info->power_manager_flag = FALSE;
                hi110x_dev->pm_info->set_roam_state(hi110x_dev, FALSE);
                ret = hi110x_start_powermgmt(hi110x_dev, ioctl_data.pri_data.roam_complete_flag);
            }
            else
            {
                HWIFI_INFO("power_manager_flag(%d) may be error,Never start roam to a AP.", roam_info->power_manager_flag);
            }
#endif            
            break;  
#endif            

#endif
        case HWIFI_WPA_IOCTL_CMD_WIFI_WORK_STATUS_GET:
            HWIFI_DEBUG("get wifi work status for coex");
            ioctl_data.pri_data.wifi_work_status = hwifi_get_active_status();
            if(copy_to_user(ifr->ifr_data, &ioctl_data, sizeof(wifi_wpa_ioctl_data_struct)))
            {
                HWIFI_WARNING("copy_to_user failed");
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



