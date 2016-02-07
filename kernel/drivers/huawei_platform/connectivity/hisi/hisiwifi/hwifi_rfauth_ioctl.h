/*
 *
 * Copyright (C), 2012-2021, Huawei Tech. Co., Ltd.
 *
 * File Name     : hwifi_android_ioctl.h
 * Version         : Initial Draft
 * Created         : 2012/12/12
 * Last Modified :
 * Description   : hwifi_android_ioctl.h header file
 * Function List :
 * History          :
 * 1.Date         : 2014/5/14
 *   Modification: Created file
 *
 */

#ifndef __HWIFI_ANDROID_IOCTL_H__
#define __HWIFI_ANDROID_IOCTL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include "common.h"
#include "hi110x.h"
 /*
 * 2 Macro Definition
 */

#define CW_MAC_ADDR         0x30002550

 /*
  * 3 Enum Type Definition
  */
enum HWIFI_ANDROID_IOCTL_CMD
{
    /* 此结构体必须与射频和协议认证测试APP中的保持一致 */
    HWIFI_ANDROID_IOCTL_CMD_MODE_SET =0,              /* set burst tx/rx mode */
    HWIFI_ANDROID_IOCTL_CMD_MODE_GET,                 /* get burst tx/rx mode */
    HWIFI_ANDROID_IOCTL_CMD_CHAN_SET,                 /* set chan in 20M mode*/
    HWIFI_ANDROID_IOCTL_CMD_CHAN_GET,                 /* get chan in 20M mode*/
    HWIFI_ANDROID_IOCTL_CMD_UCHAN_SET,                /* set uchan in 40M mode*/
    HWIFI_ANDROID_IOCTL_CMD_UCHAN_GET,                /* get uchan in 40M mode*/
    HWIFI_ANDROID_IOCTL_CMD_CURRENTTXPOW_SET,         /* set current tx power */
    HWIFI_ANDROID_IOCTL_CMD_CURRENTTXPOW_GET,         /* get current tx power */
    HWIFI_ANDROID_IOCTL_CMD_RATE_SET,                 /* set rate */
    HWIFI_ANDROID_IOCTL_CMD_RATE_GET,                 /* get rate */
    HWIFI_ANDROID_IOCTL_CMD_MCS_SET,                  /* set mcs rate */
    HWIFI_ANDROID_IOCTL_CMD_MCS_GET,                  /* get mcs rate */
    HWIFI_ANDROID_IOCTL_CMD_FC_SET,                   /* set fc param */
    HWIFI_ANDROID_IOCTL_CMD_FC_GET,                   /* get fc param */
    HWIFI_ANDROID_IOCTL_CMD_POWER_MANAGEMENT_SET,     /* power management set*/
    HWIFI_ANDROID_IOCTL_CMD_POWER_MANAGEMENT_GET,     /* power management get*/
    HWIFI_ANDROID_IOCTL_CMD_PS_SWITCH_SET,            /* set lowpower_ps_switch */
    HWIFI_ANDROID_IOCTL_CMD_PS_SWITCH_GET,            /* get lowpower_ps_switch */
    HWIFI_ANDROID_IOCTL_OLTPC_ACTIVE_SET,             /* set oltpc active*/
    HWIFI_ANDROID_IOCTL_OLTPC_ACTIVE_GET,             /* get oltpc active*/
    HWIFI_ANDROID_IOCTL_RFUPC_SET,                    /* set rfupc */
    HWIFI_ANDROID_IOCTL_RFUPC_GET,                    /* get rfupc */
    HWIFI_ANDROID_IOCTL_SCALEA_SET,                   /* set scalea */
    HWIFI_ANDROID_IOCTL_SCALEA_GET,                   /* get scalea */
    HWIFI_ANDROID_IOCTL_SCALEB_SET,                   /* set scaleb */
    HWIFI_ANDROID_IOCTL_SCALEB_GET,                   /* get scaleb */
    HWIFI_ANDROID_IOCTL_MAC_ADDRESS_SET,              /* set mac address */
    HWIFI_ANDROID_IOCTL_MAC_ADDRESS_GET,              /* get mac address */
    HWIFI_ANDROID_IOCTL_RF_REG_SET,                   /* set rf reg value */
    HWIFI_ANDROID_IOCTL_RF_REG_GET,                   /* get rf reg value */
    HWIFI_ANDROID_IOCTL_CMD_CTRL_NETWORK,             /* control network */
    HWIFI_ANDROID_IOCTL_CMD_GI_SET,                   /* Guard Interval */
    HWIFI_ANDROID_IOCTL_CMD_GI_GET,                   /* Guard Interval */
    HWIFI_ANDROID_IOCTL_CMD_RUNTC_SET,                /* runtc */
    HWIFI_ANDROID_IOCTL_CMD_MAC_REG_SET,              /* msc reg */
    HWIFI_ANDROID_IOCTL_CMD_PHY_REG_SET,              /* phy reg */
    HWIFI_ANDROID_IOCTL_CMD_FEM_CLOSE_5G,             /* close 5g,used by the requirement of checking fem flag */
    HWIFI_ANDROID_IOCTL_CMD_FEM_FLAG_GET,             /* get fem flag,used by the requirement of checking the disabled fem */
    HWIFI_ANDROID_IOCTL_CMD_RX_COUNT_CLEAN,           /* clean rx count */
    HWIFI_ANDROID_IOCTL_CMD_RX_COUNT_GET,             /* get rx count */
    HWIFI_ANDROID_IOCTL_CMD_NUM
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
struct device;

typedef struct wifi_android_ioctl_data_struct
{
    /* 此结构体必须与上层应用中的结构体保持一致 */
    int32                        cmd;                 /* 命令号 */
    union
    {
        uint8             mode;
        uint8             channel;                  /* set channel */
        uint16            uchannel;                 /* set uchannel */
        int32             currenttxpow;
        uint8             rate;                     /* set rate */
        uint8             mcs;                      /* set mcs rate */
        uint8             fc_enabled;               /* fc param enabled */
        uint8             pwrm;                     /* manual set pwrm */
        uint32            ps_switch;                /* ps switch */
        int32             oltpc_active;             /* oltpc active */
        uint16            rfupc_info;               /* set rfupc */
        uint8             scalea_info;              /* set scalea */
        uint8             scaleb_info;              /* set scaleb */
        int32             rf_reg_info;              /* set RF REG */
        int8              mac_addr[ETH_ALEN];
        int32             network_flag;
        uint8             gi_enable;                /* set gi */
        uint8             test_case;                /* test case */
        uint8             mac_reg;                  /* mac reg */
        uint8             phy_reg;                  /* phy reg */
        int32             maddr_val;                /* maddr value,used by the requirement of checking the disabled fem */
        uint8             fem_flag;                 /* fem flag,used by the requirement of checking the disabled fem */
        int32             network_ctrl_flag;        /* control network flag */
        uint32            rx_count;                 /* rx package count */
    }pri_data;
 }wifi_android_ioctl_data_struct;

typedef struct wifi_android_ioctl_data_struct_pri
{
    /* 用于记录配置信息 */
    int32                        cmd;                 /* 命令号 */
    union
    {
        uint8             mode;
        uint8             channel;                  /* get channel */
        uint16            uchannel;                 /* get uchannel */
        int32             currenttxpow;
        uint8             rate;                     /* get rate */
        uint8             mcs;                      /* get mcs rate */
        uint8             fc_enabled;               /* fc param enabled */
        uint8             pwrm;                     /* manual set pwrm */
        uint32            ps_switch;                /* ps switch */
        int32             oltpc_active;             /* oltpc active */
        uint16            rfupc_info;               /* get rfupc */
        uint8             scalea_info;              /* get scalea */
        uint8             scaleb_info;              /* get scaleb */
        int32             rf_reg_info;              /* get RF REG */
        int8              mac_addr[ETH_ALEN];
        int32             network_flag;
        uint8             gi_enable;                /* get gi */
        uint8             test_case;                /* test case */
        uint8             mac_reg;                  /* mac reg */
        uint8             phy_reg;                  /* phy reg */
        int32             maddr_val;                /* maddr value,used by the requirement of checking the disabled fem */
        uint8             fem_flag;                 /* fem flag,used by the requirement of checking the disabled fem */
        int32             network_ctrl_flag;        /* control network flag */
    }pri_data;

    uint32              rx_count;                   /* rx package recv count */
    int32               rx_count_read_flag;         /* rx package recv read flag*/
 }wifi_android_ioctl_data_struct_pri;

 /*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
int32   hwifi_android_ioctl_cmd(struct hi110x_device* hi110x_dev, struct ifreq *ifr, int32 cmd);
int32   wl_channel_set(struct cfg_struct *cfg,uint8 channel_info);
uint8   wl_channel_get(struct cfg_struct *cfg);
uint8   wl_channel_get(struct cfg_struct *cfg);
uint16  wl_uchannel_get(struct cfg_struct *cfg);
int32   wl_rfupc_set(struct cfg_struct *cfg,uint16 rfupc_info);
uint16  wl_rfupc_get(struct cfg_struct *cfg);
int32   wl_scalea_set(struct cfg_struct *cfg,uint8 scalea_info);
uint16  wl_scalea_get(struct cfg_struct *cfg);
int32   wl_scaleb_set(struct cfg_struct *cfg,uint8 scaleb_info);
uint16  wl_scaleb_get(struct cfg_struct *cfg);
int32   wl_mac_addr_set(struct cfg_struct *cfg,uint8 *mac);
int32   wl_mac_addr_get(struct cfg_struct *cfg,uint8 *mac);
int32   wl_rf_reg_set(struct cfg_struct *cfg,int32 rf_reg_info);
int32   wl_rf_reg_get(struct cfg_struct *cfg);


 #ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_android_ioctl.h */

