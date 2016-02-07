/*
 *
 * Copyright (C), 2012-2021, Huawei Tech. Co., Ltd.
 *
 * File Name     : hwifi_android.h
 * Version       : Initial Draft
 * Created       : 2012/11/23
 * Last Modified :
 * Description   : hwifi_android.h header file
 * Function List :
 * History       :
 * 1.Date        : 2012/11/23
 *   Modification: Created file
 *
 */

#ifndef __HWIFI_MAIN_H__
#define __HWIFI_MAIN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include <linux/netdevice.h>

/*
 * 2 Macro Definition
 */


/*
 * 3 Enum Type Definition
 */
enum ANDROID_WIFI_PRIV_CMD {
    ANDROID_WIFI_CMD_START = 0,/* turn on wifi hardware */
    ANDROID_WIFI_CMD_STOP,/* turn off wifi hardware */
    ANDROID_WIFI_CMD_SCAN_ACTIVE,/* set scan type to active */
    ANDROID_WIFI_CMD_SCAN_PASSIVE,/* set scan type to passive */
    ANDROID_WIFI_CMD_SETBAND,/* set band */
    ANDROID_WIFI_CMD_GETBAND,/* get band */
    ANDROID_WIFI_CMD_SETCOUNTRY,/* set country code */
    ANDROID_WIFI_CMD_GETCOUNTRY,/* set country code */
#ifdef CONFIG_BT_COEX
    ANDROID_WIFI_CMD_BTCOEXMODE,/*Android Set wifi-bt coexistence mode*/
    ANDROID_WIFI_CMD_WIBTCOEX,  /* coexistence for WIFI operation */
    ANDROID_WIFI_CMD_BTWIFICOEX,/* coexistence for wifi connect */
#endif
    ANDROID_WIFI_CMD_P2P_FLAG_SET,

    ANDROID_WIFI_CMD_LTE_COEX_PARAS,

    ANDROID_WIFI_CMD_MAX
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

/*z00262551 arm64*/
typedef struct __android_wifi_priv_cmd_ {
        int used_len;
        int total_len;
        char buf_offset[0];
} android_wifi_priv_cmd;
#define HWIFI_PRIV_CMD_HDR_LEN()    (sizeof(android_wifi_priv_cmd))
#define HWIFI_PRIV_CMD_DATA(buf)  (((android_wifi_priv_cmd*)(buf))->buf_offset)
#define HWIFI_PRIV_CMD_HDR(buf)  ((android_wifi_priv_cmd*)(buf))

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
extern int32 hwifi_android_priv_cmd(struct net_device *net, struct ifreq *ifr, int cmd);
extern int32 hwifi_regdom_update(struct cfg_struct *cfg, const uint8 *country_code);

#ifdef CONFIG_BT_COEX
/*wifi-bt coexistence mode set function, definition by BT driver*/
extern int btcoex_set_dhcp_mode(int mode, uint32 oppo_win_time, uint32 force_timeout);
extern void set_coex_param(int oper_state, int wifi_state, int oper_delay);
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_main.h */
