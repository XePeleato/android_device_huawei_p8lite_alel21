/*
 *
 * Copyright (C), 2012-2021, Huawei Tech. Co., Ltd.
 *
 * File Name     : hwifi_wl_config_ioctl.h
 * Version         : Initial Draft
 * Created         : 2012/12/12
 * Last Modified :
 * Description   : hwifi_wl_config_ioctl.h header file
 * Function List :
 * History          :
 * 1.Date         : 2014/1/20
 *   Modification: Created file
 *
 */

#ifndef __HWIFI_WL_CONFIG_IOCTL_H__
#define __HWIFI_WL_CONFIG_IOCTL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include <linux/ieee80211.h>  /* IEEE80211_MAX_SSID_LEN */
#include "hwifi_event.h"
#include "hwifi_aggre.h"
#include "hsdio_test.h"
#include "hwifi_cfg80211.h"


/*
 * 2 Macro Definition
 */
#define PK_MODE_ADDR         0x000807df

#define IS_GET_PMF_CMD(cmd) ((cmd) > 0x80)


/*
 * 3 Enum Type Definition
 */
enum HWIFI_WL_CONFIG_IOCTL_CMD
{
    /* 此结构体必须与上层应用wl_config_lib.h中的保持一致 */
    HWIFI_IOCTL_CMD_P2P_FLAG_SET = 0,         /* set p2p on/off switch */
    HWIFI_IOCTL_CMD_P2P_FLAG_GET,             /* get p2p on/off switch */

    HWIFI_IOCTL_CMD_GET_CURR_CHANNEL,         /* get current channel */

    HWIFI_IOCTL_CMD_SET_AUTO_CHANNEL,         /* set auto channel flag */
    HWIFI_IOCTL_CMD_GET_AUTO_CHANNEL,         /* get auto channel flag */

    HWIFI_IOCTL_CMD_STA_WMM_UAPSD_SET,        /* set STA UAPSD WMM POWER SAVE */
    HWIFI_IOCTL_CMD_AMSDU_SET,                /* set amsdu param */
    HWIFI_IOCTL_CMD_BA_SET,                   /* set ba param */
    HWIFI_IOCTL_CMD_BA_GET,                   /* get ba param */
    HWIFI_IOCTL_CMD_AMPDU_SET,                /* set ampdu param */

    HWIFI_IOCTL_CMD_AGGRE_MODE_SET,           /* set device aggreagtion mode */
    HWIFI_IOCTL_CMD_AGGRE_MODE_GET,
    HWIFI_IOCTL_CMD_AGGRE_CFG_SET,            /* host ba session config: max_tx, max_rx, max_retry */
    HWIFI_IOCTL_CMD_AGGRE_CFG_GET,

    HWIFI_IOCTL_CMD_LINK_STATIS_SET,      /* set link statistic */
    HWIFI_IOCTL_CMD_LINK_STATIS_GET,      /* get link statistic */

    HWIFI_IOCTL_CMD_RATE_SET,                 /*set rate*/
    HWIFI_IOCTL_CMD_RATE_GET,                 /*get rate*/

    HWIFI_IOCTL_CMD_RUNTC_SET,                /*set run test case*/
    HWIFI_IOCTL_CMD_RUNTC_GET,                /*get run test case*/

    HWIFI_IOCTL_CMD_FC_SET,                   /*set fc param*/
    HWIFI_IOCTL_CMD_FC_GET,                   /*get fc param*/

    HWIFI_IOCTL_CMD_GMODE_SET,                /*set gmode parma*/
    HWIFI_IOCTL_CMD_GMODE_GET,                /*get gmode param*/
    HWIFI_IOCTL_CMD_NMODE_SET,                /*set nmode param*/
    HWIFI_IOCTL_CMD_NMODE_GET,                /*get nmode param*/

    HWIFI_IOCTL_CMD_LINT_SET,                 /*set listen interval */
    HWIFI_IOCTL_CMD_LINT_GET,                 /*get listen interval*/

    HWIFI_IOCTL_CMD_PREAM_SET,                /*set preamble type*/
    HWIFI_IOCTL_CMD_PREAM_GET,                /*get preamble type*/

    HWIFI_IOCTL_CMD_HWAR_SET,                 /*set hwar param*/
    HWIFI_IOCTL_CMD_HWAR_GET,                 /*get hwar param*/

    HWIFI_IOCTL_CMD_CTXBW_SET,                /*set tx channel bandwidth*/
    HWIFI_IOCTL_CMD_CTXBW_GET,                /*get tx channel bandwidth*/

    HWIFI_IOCTL_CMD_MCS_SET,                  /*set mcs rate*/
    HWIFI_IOCTL_CMD_MCS_GET,                  /*get mcs rate*/

    HWIFI_IOCTL_CMD_INTOL_SET,                /*set 40MHz Intolerance mode*/
    HWIFI_IOCTL_CMD_INTOL_GET,                /*get 40MHz Intolerance mode*/

    HWIFI_IOCTL_CMD_QOS_SET,                  /*set QOS enabled*/
    HWIFI_IOCTL_CMD_QOS_GET,                  /*get QOS enabled*/

    HWIFI_IOCTL_CMD_RIFS_PROT_SET,            /*set RIFS PROT enabled*/
    HWIFI_IOCTL_CMD_RIFS_PROT_GET,

    HWIFI_IOCTL_CMD_ACK_POLICY_ENABLE_SET,    /*set ack policy enabled*/
    HWIFI_IOCTL_CMD_ACK_POLICY_ENABLE_GET,    /*get ack policy*/

    HWIFI_IOCTL_CMD_TXOP_PROT_ENABLE_SET,     /*set txop prot enabled*/
    HWIFI_IOCTL_CMD_TXOP_PROT_ENABLE_GET,     /*get txop prot enabled*/

    HWIFI_IOCTL_CMD_ERP_PROT_ENABLE_SET,      /*set erp prot enabled*/
    HWIFI_IOCTL_CMD_ERP_PROT_ENABLE_GET,      /*get erp prot enabled*/

    HWIFI_IOCTL_CMD_HTPROT_ENABLE_SET,        /*set htprot enabled*/
    HWIFI_IOCTL_CMD_HTPROT_ENABLE_GET,        /*get htprot enabled*/

    HWIFI_IOCTL_CMD_GI_ENABLE_SET,            /*Enable Short Guard Interval Length */
    HWIFI_IOCTL_CMD_GI_ENABLE_GET,            /*get Guard Interval Length*/
    HWIFI_IOCTL_CMD_HT_ENABLE_SET,            /*set ht capability enabled*/
    HWIFI_IOCTL_CMD_HT_ENABLE_GET,            /*get ht capability enabled*/

    HWIFI_IOCTL_CMD_POWER_MANAGEMENT_SET,     /*power management set*/
    HWIFI_IOCTL_CMD_POWER_MANAGEMENT_GET,     /*power management get*/

    HWIFI_IOCTL_CMD_PK_MODE_SET,
    HWIFI_IOCTL_CMD_PK_MODE_GET,

    HWIFI_IOCTL_CMD_PROT_MODE_SET,             /*protection mode set*/
    HWIFI_IOCTL_CMD_PROT_MODE_GET,             /*protection mode get*/

    HWIFI_IOCTL_CMD_2040_COEX_SET,             /*20/40 COEX MODE SET*/
    HWIFI_IOCTL_CMD_2040_COEX_GET,             /*20/40 COEX MODE GET*/

    HWIFI_IOCTL_CMD_AMSDU_CERTIFY_SET,         /*amsdu certify mode set*/
    HWIFI_IOCTL_CMD_AMSDU_CERTIFY_GET,         /*amsdu certify mode get*/

    HWIFI_IOCTL_CMD_PSM_CERTIFY_SET,           /*psm certify mode set*/
    HWIFI_IOCTL_CMD_PSM_CERTIFY_GET,           /*psm certify mode get*/

    HWIFI_IOCTL_CMD_WMM_AC_OPT_SET,             /*wmm ac opt certify mode set*/
    HWIFI_IOCTL_CMD_WMM_AC_OPT_GET,             /*wmm ac opt certify mode get*/

    HWIFI_IOCTL_CMD_ROAM_PARAM,
    HWIFI_IOCTL_CMD_PMF_PARAM,

	HWIFI_IOCTL_CMD_DC_OFFSET_SET,			   /* dc_offset configs set */
	HWIFI_IOCTL_CMD_DC_OFFSET_GET,

    HWIFI_IOCTL_CMD_AP_WMM_UAPSD_SET,           /* set ap wmm */

    HWIFI_IOCTL_CMD_AGGRE_FUNC_SET,             /* ioctl cmd for aggre func */
    HWIFI_IOCTL_CMD_AGGRE_FUNC_GET,
    HWIFI_IOCTL_CMD_AGGRE_MAX_RATE,
    HWIFI_IOCTL_CMD_AGGRE_MIN_RATE,

    HWIFI_IOCTL_CMD_DEV_ERROR_SET,              /* ioctl cmd for dev error */
    HWIFI_IOCTL_CMD_DEV_ERROR_GET,

    HWIFI_IOCTL_DATA_PERFORM_SET,
    HWIFI_IOCTL_DATA_PERFORM_SWITCH_SET,

    HWIFI_IOCTL_CMD_SDIOTEST_SET,
    HWIFI_IOCTL_CMD_SDIOTEST_GET,
    HWIFI_IOCTL_CMD_LOGLEVEL_SET,
    HWIFI_IOCTL_CMD_LOGLEVEL_GET,
    HWIFI_IOCTL_CMD_RATE_40M_SET,
    HWIFI_IOCTL_CMD_RATE_40M_GET,
    HWIFI_IOCTL_CMD_RATE_80M_SET,
    HWIFI_IOCTL_CMD_RATE_80M_GET,
    HWIFI_IOCTL_CMD_RATE_160M_SET,
    HWIFI_IOCTL_CMD_RATE_160M_GET,
    HWIFI_IOCTL_CMD_ASSEMBLE_COUNT_SET,
    HWIFI_IOCTL_CMD_ASSEMBLE_COUNT_GET,
    HWIFI_IOCTL_CMD_PANIC_SET,
    HWIFI_IOCTL_CMD_PANIC_GET,
    HWIFI_IOCTL_CMD_PS_SWITCH_SET,
    HWIFI_IOCTL_CMD_PS_SWITCH_GET,
    HWIFI_IOCTL_CMD_WATCHDOG_TIMEOUT_SET,
    HWIFI_IOCTL_CMD_WATCHDOG_TIMEOUT_GET,
    HWIFI_IOCTL_CMD_DUMP_SET,
    HWIFI_IOCTL_CMD_DUMP_GET,
    HWIFI_IOCTL_CMD_CPUFREQ_SET,
    HWIFI_IOCTL_CMD_CPUFREQ_GET,
    HWIFI_IOCTL_CMD_CURRENTTXPOW_SET,
    HWIFI_IOCTL_CMD_CURRENTTXPOW_GET,
    HWIFI_IOCTL_CMD_DEVICE_LOG_SET,
    HWIFI_IOCTL_CMD_DEVICE_LOG_GET,

	HWIFI_IOCTL_CMD_NUM
};


enum pmf_param_cmd_enum
{
    PMF_PARAM_CMD_COMMIT = 0x00,

    /****************************set****************************/
    PMF_PARAM_CMD_SET_ACTIVATED = 0x01,
    PMF_PARAM_CMD_SET_ALLOWED,

    /****************************get****************************/
    PMF_PARAM_CMD_GET_ACTIVATED = 0x81,
    PMF_PARAM_CMD_GET_ALLOWED
};


/*
 * 4 Global Variable Declaring
 */

extern int log_dump_flag;


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
struct device_log_param
{
    int32 log_control; /* 1:on  0:off*/
    uint8 device_log_module_id;
};

typedef struct wifi_wl_config_ioctl_data_struct
{
    /* 此结构体必须与上层应用wl_config_lib.h中的保持一致 */
    /* 校验位,取值1101,与其他平台区别开来 */
    int32 verify;
    int32                        cmd;                 /* 命令号 */
    union
    {
        int32                    p2p_on;              /* p2p 使能参数 */
        int32                    cur_channel;
        int8                     auto_channel;

        struct wmm_uapsd_param   wmm_uapsd_cfg_param;
        struct aggre_cfg_param   aggre_param;
        struct query_ba_param    ba_query_param;

        uint8                    aggre_mode_enabled;  /* auto aggreagtion mode set*/
        struct host_ba_cfg_param ba_cfg_param;
        struct stats_struct      link_statis;         /* link statis info */
        uint8                    rate;                /* set rate */
        uint8                    test_case;           /* test case */
        uint8                    fc_enabled;          /* fc param enabled */
        uint8                    gmode;               /* set 11g operating mode */
        uint8                    nmode;               /* set 11n operating mode */
        int32                    lint;                /* set listen interval */
        uint8                    pream;               /* set preamble type */
        uint8                    hwar;                /* set hardware auto rate */
        uint8                    ctxbw;               /* set tx channel bandwidth */
        uint8                    mcs;                 /* set mcs rate */
        uint8                    intol;               /* set 40MHz Intolerance mode  */
        uint8                    qos_enabled;         /* qos set enabled */
        uint8                    rifs_prot_enabled;   /* rifs prot enabled */
        uint8                    ack_policy_enabled;  /* ack policy enabled */
        uint8                    txop_prot_enabled;   /* txop prot enabled */
        uint8                    erp_prot;            /* erp prot enabled */
        uint8                    htprot;              /* htprot enabled */
        uint8                    gi_enabled;          /* gi enabled */

        uint8                    ht_capability_enabled;  /* ht enabled */
        uint8                    pwrm;                   /* power management */
        uint8                    pk_mode;
        uint8                    prot;                   /* prot mode set */
        uint8                    coex_2040_enabled;      /* 2040 coex mode set */
        uint8                    amsdu_certify_enabled;  /* amsdu certify mode set */
        uint8                    psm_certify_enabled;    /* psm certify mode set */
        uint8                    wmm_ac_opt_enabled;     /* wmm ac opt certify mode set */

        uint8                    loglevel;
        uint8                    rate_40M;
        uint8                    rate_80M;
        uint8                    rate_160M;
        uint8                    assemble_count;
        uint8                    panic;
        uint8                    ps_switch;
        uint8                    watchdog_timeout;
        uint8                    dump;
        uint8                    cpufreq;
        int32                    currenttxpow;
        struct device_log_param  device_log;

#ifdef __ROAM__
        struct
        {
            int32 type;
            int32 value;
        }roam_param;
#endif

#ifdef MAC_802_11W
        struct
        {
            int type;
            int value;
        }pmf_param;
#endif

        struct dc_offset_param    dc_param;               /* software anti-jam */
        uint8                     ap_wmm_enable;

#ifdef WLAN_ARRG_DYNAMIC_CONTROL
        uint8 aggre_func_enabled;
        int32 max_rate;
        int32 min_rate;
#endif
        uint8 dev_error_enabled;

#ifdef WLAN_PERFORM_DEBUG
        struct
        {
            uint8   sdio_bypass;
            uint8   network_bypass;
            uint8   sdio_statis;
            uint8   network_statis;
            uint32  mpdu_pkt_num;
            uint8   trigger_mode;
        }wlan_perform_cfg_interface;
#endif

        hsdio_test_data sdiotest;

    }pri_data;
}wifi_wl_config_ioctl_data_struct;


/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */

int32 hwifi_wl_config_ioctl_cmd(struct hi110x_device* hi110x_dev, struct ifreq *ifr, int32 cmd);
int32 wl_pwrm_set(struct cfg_struct *cfg,uint8 pwrm);
int32 wl_ht_set(struct cfg_struct *cfg,uint8 enabled);
uint8 wl_ht_get(struct cfg_struct *cfg);
int32 wl_rate_set(struct cfg_struct *cfg, uint8  rate);
uint8 wl_rate_get(struct cfg_struct *cfg);
int32 wl_gi_set(struct cfg_struct *cfg,uint8 enabled);
uint8 wl_gi_get(struct cfg_struct *cfg);
int32 wl_pwrm_enable(struct cfg_struct *cfg,uint8 enabled);
int32 wl_currenttxpow_set(struct cfg_struct *cfg,int32 currt_tx_power);
int32 wl_currenttxpow_get(struct cfg_struct *cfg);
uint8 wl_mcs_get(struct cfg_struct *cfg);
int32 wl_mcs_set(struct cfg_struct *cfg,uint8 mcs);
int32 wl_fc_set(struct cfg_struct *cfg,uint8 enabled);
uint8 wl_fc_get(struct cfg_struct *cfg);
int32 wl_pwrm_set(struct cfg_struct *cfg,uint8 pwrm);
uint8 wl_pwrm_get(struct cfg_struct *cfg);
extern int32  wl_pk_mode_addr_set(struct cfg_struct *cfg,int32 enabled);
extern int32  wl_runtc_set(struct cfg_struct *cfg, uint8 test_case);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_wl_config_ioctl.h */


