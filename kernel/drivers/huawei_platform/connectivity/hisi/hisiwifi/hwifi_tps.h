

#ifndef __HWIFI_TPS_H__
#define __HWIFI_TPS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hwifi_msg.h"
#include "cfg80211_stru.h"


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define HWIFI_AT_TEST_5G_BAND           0x8000
#define HWIFI_AT_TEST_MAX_FREQ_2G       (2484)

#define WID_SIGNAL_TONE_PHY_ACTIVE_REG_1       0xff
#define WID_SIGNAL_TONE_ACTIVE_REG_VAL_1   5
#define WID_SIGNAL_TONE_PHY_ACTIVE_REG_2       0x50
#define WID_SIGNAL_TONE_ACTIVE_REG_VAL_2   3
#define WID_SIGNAL_TONE_PHY_ACTIVE_REG_3       0xff
#define WID_SIGNAL_TONE_ACTIVE_REG_VAL_3   4
#define WID_SIGNAL_TONE_PHY_ACTIVE_REG_4       0x62
#define WID_SIGNAL_TONE_ACTIVE_REG_VAL_4   1
#define WID_SIGNAL_TONE_PHY_ACTIVE_REG_5       0x64
#define WID_SIGNAL_TONE_ACTIVE_REG_VAL_5   3
#define WID_SIGNAL_TONE_PHY_ACTIVE_REG_6       0xD5
#define WID_SIGNAL_TONE_ACTIVE_REG_VAL_6   0x25
#define WID_SIGNAL_TONE_PHY_ACTIVE_REG_7       0x66
#define WID_SIGNAL_TONE_ACTIVE_REG_VAL_7   0x1
#define WID_SIGNAL_TONE_PHY_ACTIVE_REG_8       0x65
#define WID_SIGNAL_TONE_ACTIVE_REG_VAL_8   0x1
#define WID_SIGNAL_TONE_RF_REG_INFO            0xFE000101


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum HWIFI_TEST_IOCTL_CMD
{

       HWIFI_IOCTL_CMD_WI_FREQ_SET=0,                /* set wifi freq */
       HWIFI_IOCTL_CMD_WI_USERPOW_SET,             /* set user power */
       HWIFI_IOCTL_CMD_WI_USERPOW_GET,             /* get user power */
       HWIFI_IOCTL_CMD_WI_POWER_SET,               /* set  power */
       HWIFI_IOCTL_CMD_WI_POWER_GET,               /* get  power */
       HWIFI_IOCTL_CMD_MODE_SET,                   /* set burst tx/rx mode */
       HWIFI_IOCTL_CMD_MODE_GET,                   /* get burst tx/rx mode */
       HWIFI_IOCTL_OLTPC_ACTIVE_SET,               /* set oltpc active */
       HWIFI_IOCTL_OLTPC_ACTIVE_GET,               /* get oltpc active*/
       HWIFI_IOCTL_OLTPC_SWITCH_SET,               /* set oltpc switch*/
       HWIFI_IOCTL_OLTPC_SWITCH_GET,               /* get oltpc switch*/
       HWIFI_IOCTL_CMD_DATARATE_SET,
       HWIFI_IOCTL_CMD_BAND_SET,
       HWIFI_IOCTL_CMD_PROTOCOL_GMODE_SET,
       HWIFI_IOCTL_CMD_PROTOCOL_NMODE_SET,
       HWIFI_IOCTL_CMD_DBB_GET,

       HWIFI_IOCTL_CMD_UPC_GET,
       HWIFI_IOCTL_CMD_GEN_CW_SINGLE_TONE_SET,

       HWIFI_IOCTL_CMD_TEST_BUTT

};
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
struct hwifi_gen_cw_single_tone_msg {
    struct hwifi_msg_header msg_hdr;
    struct char_wid  phy_active_reg_1;
    struct int_wid   hw_11n_phy_active_reg_val_1;
    struct char_wid  phy_active_reg_2;
    struct int_wid   hw_11n_phy_active_reg_val_2;
    struct char_wid  phy_active_reg_3;
    struct int_wid   hw_11n_phy_active_reg_val_3;
    struct char_wid  phy_active_reg_4;
    struct int_wid   hw_11n_phy_active_reg_val_4;
    struct char_wid  phy_active_reg_5;
    struct int_wid   hw_11n_phy_active_reg_val_5;
    struct char_wid  phy_active_reg_6;
    struct int_wid   hw_11n_phy_active_reg_val_6;
    struct char_wid  phy_active_reg_7;
    struct int_wid   hw_11n_phy_active_reg_val_7;
    struct char_wid  phy_active_reg_8;
    struct int_wid   hw_11n_phy_active_reg_val_8;
    struct int_wid   rf_reg_info;
};
            /* ioctl 通用数据结构 */
typedef struct wifi_ioctl_test_data_struct
{
    /* 校验位,取值1101,与其他平台区别开来 */
    int32 verify;
    int cmd;                                /* 命令号 */
    union
    {
        int32 freq;
        int32 userpow;
        int32 pow;
        int32 oltpc_active;
        int32 oltpc_switch;
        uint8 mode;
        int32 datarate;
        int32 band;
        int32 protocol_gmode;
        int32 protocol_nmode;
        /* TBD:dbb[HISI_WIFI_DBB_LEN+1], need to keep consistency with Android wlconfig definition */
        int8  dbb[HISI_WIFI_DBB_LEN];

        int32 check_upc_flag;
    }pri_data;
}wifi_ioctl_test_data_struct;


struct tps_struct
{
        /*装备测试*/
        int32 userpow;
        int32 pow;
        int32 oltpc_active;
        int32 oltpc_switch;
        uint8 mode;
        uint32 dbb_ver_got;
        int8  dbb[HISI_WIFI_DBB_LEN+1];

        int32 check_upc_ctrl;
        int32 check_upc_flag;
};

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


extern int32 hwifi_tps_ioctl_cmd(struct hi110x_device* hi110x_dev, struct ifreq *ifr, int32 cmd);
extern int32 hwifi_test_mode_set(struct cfg_struct *cfg, uint8  mode);
extern int32 hwifi_test_mode_get(struct cfg_struct *cfg);
extern int32 hwifi_oltpc_active_set(struct cfg_struct *cfg, int32  enabled);
extern int32 hwifi_oltpc_active_get(struct cfg_struct *cfg);

extern int32 hwifi_upc_get(struct cfg_struct *cfg);







#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_tps.h */
