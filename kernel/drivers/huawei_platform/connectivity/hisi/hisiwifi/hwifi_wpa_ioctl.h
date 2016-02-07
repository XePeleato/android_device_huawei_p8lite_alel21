/*
 *
 * Copyright (C), 2012-2021, Huawei Tech. Co., Ltd.
 *
 * File Name     : hwifi_wpa_ioctl.h
 * Version         : Initial Draft
 * Created         : 2012/12/12
 * Last Modified :
 * Description   : hwifi_wpa_ioctl.h header file
 * Function List :
 * History          :
 * 1.Date         : 2014/1/20
 *   Modification: Created file
 *
 */

#ifndef __HWIFI_WPA_IOCTL_H__
#define __HWIFI_WPA_IOCTL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include <linux/ieee80211.h>  /* IEEE80211_MAX_SSID_LEN */


/*
 * 2 Macro Definition
 */

/*
 * 3 Enum Type Definition
 */
enum HWIFI_WPA_IOCTL_CMD
{
    /* 此结构体必须与上层应用hisi_connectivity_patch.h中的保持一致 */
    HWIFI_WPA_IOCTL_CMD_P2P_FLAG_SET = 0,         /* set p2p on/off switch */
    HWIFI_WPA_IOCTL_CMD_P2P_FLAG_GET,             /* get p2p on/off switch */
    HWIFI_WPA_IOCTL_CMD_GET_CURR_CHANNEL,         /* get current channel */
    HWIFI_WPA_IOCTL_CMD_SET_WPS_P2P_IE,           /* set p2p wps ie information to device */
    HWIFI_WPA_IOCTL_CMD_SET_AUTH_ALG,             /* set open/shared mode */
    HWIFI_WPA_IOCTL_CMD_SET_HW_MODE,              /* set hardware mode a,b,g */
    HWIFI_WPA_IOCTL_CMD_SET_HT_ENABLE,            /* set 11N mode enable/disable */
    HWIFI_WPA_IOCTL_CMD_SET_SSID,                 /* set ssid */
    HWIFI_WPA_IOCTL_CMD_GET_STA_ASSOC_REQ_IE,     /* get sta assocate request ie */
    HWIFI_WPA_IOCTL_CMD_P2P_CTWINDOW,             /* set ctwindow value */
    HWIFI_WPA_IOCTL_CMD_P2P_NOA,                  /* set p2p noa */
    HWIFI_WPA_IOCTL_CMD_GET_NOA,                  /* get p2p noa */
    HIWFI_WPA_IOCTL_CMD_SET_STA_NUM,              /* set max sta num of sta */
    HWIFI_WPA_IOCTL_CMD_TKIP_MEASURE,
    HWIFI_WPA_IOCTL_CMD_POWERMGMT_ON,             /* 使能电源管理，低功耗功能 */
    HWIFI_WPA_IOCTL_CMD_STA_IFACE_STATE_SET,      /* STA模式关闭打开网卡命令 */
    HWIFI_WPA_IOCTL_CMD_P2P_IFACE_STATE_SET,      /* P2P模式关闭打开网卡命令 */
    HWIFI_WPA_IOCTL_CMD_SCREEN_STATE_GET,         /* 获取屏幕状态 */
    HWIFI_WPA_IOCTL_CMD_CUR_MODE_GET,             /* 获取当前工作模式 */
    HWIFI_WPA_IOCTL_CMD_AP_POWER_OFF,             /*ap mode power off*/
    HWIFI_WPA_IOCTL_CMD_AP_POWER_ON,              /*ap mode power on*/
    HWIFI_WPA_IOCTL_CMD_WIFI_WORK_STATUS_GET,     /* ioctl cmd for wifi work status */
    HWIFI_WPA_IOCTL_CMD_ROAM_COMPLETE_SET,
    HWIFI_WPA_IOCTL_CMD_NUM
};


enum IE_TYPE
{
    BEACON_IE = 0,
    PROB_RSP_IE,
    ASSOC_RSP_IE,
    P2P_WPS_IE_NUM
};


/*
 * 4 Global Variable Declaring
 */



/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
/* ioctl 通用数据结构 */
typedef struct _wifi_wpa_ioctl_data_struct_
{
    /* 此结构体必须与上层应用hisi_connectivity_patch.h中的保持一致 */
    /*To adapter LP64 & LLP32, we never should use pointer type or long in the struct*/
    /* 校验位,取值1101,与其他平台区别开来 */
    int32 verify;
    int32 cmd;                                /* 命令号 */
    union
    {
        int32              p2p_on;            /* p2p 使能参数 */
        int32              cur_channel;

        /*ie len used to reference the ie buff hdr addr*/
        struct
        {
            int32 ie_len[P2P_WPS_IE_NUM];
            char ie_buff[0];
        }p2p_wps_ie;

        int8 auth_algs;                       /* 0:open,1:shared */
        int8 hw_mode;                         /* 0:b,1:g,2:a */
        int8 ht_enabled;                      /* 0:11n mode not enable,1:11n mode enabled */
        int8 ssid[IEEE80211_MAX_SSID_LEN + 1];

        struct
        {
            unsigned char mac[6];
            int           buf_size;          /* user space ie buffer size */
            char buf[0];               /* the assoc req ie buf offset, can not use pointer by arm64*/
        }assoc_req_ie;                       /* ap mode use,get sta assocate request ie */

        /* p2p ps */
        struct
        {
            int32 legacy_ps;
            int32 ctwindow;
            int32 opps;
        }ct;
        /* p2p noa */
        struct
        {
            uint8   count;
            int32   start;
            int32   duration;
        }noa;
        /* get p2p noa, keep it at the last */
        struct
        {
            uint8        used_len;
            uint8        buf_len;
            char         noa_buf[0];
        }buf;

        int32 max_num;
        int32 tkip_measure_enable;    /* 1: enable, 0: disable */
        int32 powermgmt_on;           /* 使能电源管理，低功耗功能 */
        int32 iface_state;            /* 控制网卡状态 */
        int32 screen_state;
        int32 cur_mode;
        uint8 wifi_work_status;
        int32 roam_complete_flag;     /* roam success or roam failed. */

    }pri_data;
}wifi_wpa_ioctl_data_struct;


/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */

int32 hwifi_wpa_ioctl_cmd(struct hi110x_device* hi110x_dev, struct ifreq *ifr, int32 cmd);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_wpa_ioctl.h */

