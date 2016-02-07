/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : LNvCommon.h
  Description     : LNvCommon.h header file
  History           :
     1.Name+ID      yyyy-mm-dd  Draft Enact

******************************************************************************/

#ifndef __LNVCOMMON_H__
#define __LNVCOMMON_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#if 0
#include  "vos.h"
#include  "PsTypeDef.h"
#endif

/* this file is included by drv, submit by zhouluojun, reviewed by hujianbo */
/* #pragma pack(4) */

/*****************************************************************************
  2 macro
*****************************************************************************/

#if 0
/* 非标准频段 begin */
#define LTE_COMM_NONSTANDARD_BAND_BEGIN                     101
#define LTE_COMM_NONSTANDARD_BAND_END                       116

#define LTE_COMM_NONSTANDARD_BAND_MAX_COUNT                 16
/* 非标准频段 end */
#endif
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


enum NV_ITEM_ID_ENUM
{
    EN_NV_ID_BEGIN                                  = 0,

    /* PS begin */
    EN_NV_ID_PS_BEGIN                               = 0xD200,/*Modem begin D000*/
    EN_NV_ID_LTEAPT_TOTAL_FLG                       = 0xD201,
    EN_NV_ID_IMS_SIP_CONFIG                         = 0xD202,
    EN_NV_ID_IMS_CONFIG                             = 0xD203,
    EN_NV_ID_UE_NET_CAPABILITY                      = 0xD204,
    EN_NV_ID_UE_CENTER                              = 0xD205,/* modified by lihong00150010 2012-07-31 cs+ps1 */
    EN_NV_ID_NAS_RELEASE                            = 0xD206,
    EN_NV_ID_IMS_RAT_SUPPORT                        = 0xD207,/*modified by sunbing 49683 2013-06-27 VoLTE*/
    EN_NV_ID_SEC_CONTEXT                            = 0xD208,
    EN_NV_ID_IMSA_CONFIG                            = 0xD209,/*modified by sunbing 49683 2013-06-27 VoLTE*/
    EN_NV_ID_EPS_LOC                                = 0xD20a,
    EN_NV_ID_IMS_CAPABILITY                         = 0xD20b,/*modified by sunbing 49683 2013-06-27 VoLTE*/
    EN_NV_ID_SIP_PORT_CONFIG                        = 0xD20c,/*modified by sunbing 49683 2013-06-27 VoLTE*/
    EN_NV_ID_IMSI                                   = 0xD20d,
    EN_NV_ID_UE_VOICE_DOMAIN                        = 0xD20e,
    EN_NV_ID_IMS_VOIP_CONFIG                        = 0xD20f,
    EN_NV_ID_IMS_CODE_CONFIG                        = 0xD210,
    EN_NV_ID_IMS_SS_CONF_CONFIG                     = 0xD211,
    EN_NV_ID_ATTACH_BEARER_RE_ESTABLISH             = 0xD212,
    EN_NV_ID_IMS_SECURITY_CONFIG                    = 0xD213,
    EN_NV_ID_IMS_MEDIA_PARM_CONFIG                  = 0xD214,
    EN_NV_ID_PS_BEARER_CNTXT_PARA2                  = 0xD215,
    EN_NV_ID_PS_BEARER_CNTXT_PARA3                  = 0xD216,
    EN_NV_ID_PS_BEARER_CNTXT_PARA4                  = 0xD217,
    EN_NV_ID_PS_BEARER_CNTXT_PARA5                  = 0xD218,
    EN_NV_ID_PS_BEARER_CNTXT_PARA6                  = 0xD219,
    EN_NV_ID_CONFIG_NWCAUSE                         = 0xD21a,/* modify by leili 2014-02-15 for self-adaption NW cause*/
    EN_NV_ID_PS_BEARER_CNTXT_PARA8                  = 0xD21b,
    EN_NV_ID_LNAS_COMM_CONFIG_PARA                  = 0xD21c,
    EN_NV_ID_PCSCF_DISCOVERY_POLICY                 = 0xD21d,
    EN_NV_ID_UE_USIM_FEATURE                        = 0xD21e,

    EN_NV_ID_CELL_MEAS_THREDHOLD                    = 0xD220,
    EN_NV_ID_GET_SIB_THRESHOLD                      = 0xD221,
    /* R10 NV修改 begin */
    EN_NV_ID_UE_CAP_V9a0                            = 0xD222,
    EN_NV_ID_UE_CAP_V1020                           = 0xD223,
    EN_NV_ID_UE_CAP_V1060                           = 0xD224,
    EN_NV_ID_UE_CAP_RF_PARA_V1060                   = 0xD225,
    /* R10 NV修改 end */
    /* CMCC-BEGIN */
    EN_NV_ID_IRAT_REDIR_SWITCH                      = 0xD226,
    /* CMCC-END */
    /* MTC add by k0228722 begin */
    EN_NV_ID_LTE_SPAC_BAND_INFO                     = 0xD227,
    EN_NV_ID_LTE_ABANDON_BAND_INFO                  = 0xD228,
    /* MTC add by k0228722 end */
    /* MTC NOTCH add by begin */
    EN_NV_ID_LTE_MTC_NOTCH_FEATURE_INFO             = 0xD229,
    /* MTC NOTCH add by end */
    /* adapt filter begin */
    EN_NV_ID_RRC_THRESHOLD                          = 0XD22b,
    /* adapt filter end */
    /*Delete FDD Band */
    EN_NV_ID_LTE_CUSTOM_MCC_INFO                     = 0xD22a,
    /*Delete FDD Band */
    EN_NV_ID_UE_CAPABILITY                          = 0xD22c,
    EN_NV_ID_SUPPORT_FREQ_BAND                      = 0xD22d,
    NV_ID_RRC_NV_CALIBRATION_BAND_LIST              = 0xD22e,
    EN_NV_ID_SUPPORT_SUPPORT_RESEL                  = 0xD22f,
    EN_NV_ID_UE_MAX_TX_POWER                        = 0xD230,
    EN_NV_ID_CGI_INFO                               = 0xD231,
    EN_NV_ID_RRC_LOCK_FREQ_SWITCH                   = 0xD232,
    EN_NV_ID_RRC_RESELECT_3DB_SWITCH                = 0xD233,
    EN_NV_ID_UE_RFRD_TYPE                           = 0xD234,
    EN_NV_ID_LTE_IRAT_TDS_CAPABILITY_INFO           = 0xD235,
    EN_NV_ID_UE_EXTBAND_INFO                        = 0xD236,
    EN_NV_ID_EXTBAND_INFO_WITH_BANDWIDTH		    = 0xD237,

    EN_NV_ID_DSDS_CFG_INFO                          = 0xD238,



    EN_NV_ID_RF_HW                                  = 0xD242,
    EN_NV_ID_USIM_BEGIN                             = 0xD250,
    EN_NV_ID_UST_FILE                               = 0xD251,
    EN_NV_ID_MNC_LEN_FILE                           = 0xD252,
    EN_NV_ID_FORBIDDEN_PLMN_LIST_FILE               = 0xD253,
    EN_NV_ID_UPLMN_SEL_FILE                         = 0xD254,
    EN_NV_ID_OPLMN_SEL_FILE                         = 0xD255,
    EN_NV_ID_HPLMN_PERI_FILE                        = 0xD256,
    EN_NV_ID_SEC_CONTEXT_FILE                       = 0xD257,
    EN_NV_ID_CS_LOC_FILE                            = 0xD258,
    EN_NV_ID_PS_LOC_FILE                            = 0xD259,
    EN_NV_ID_EPS_LOC_FILE                           = 0xD25a,
    EN_NV_ID_IMSI_FILE                              = 0xD25b,
    EN_NV_ID_CS_CKIK_FILE                           = 0xD25c,
    EN_NV_ID_PS_KEY_FILE                            = 0xD25d,
    EN_NV_ID_KC_FILE                                = 0xD25e,
    EN_NV_ID_UE_NAS_SECURITY_ALGORITHMS             = 0xD25f,
    EN_NV_ID_USIM_VISION                            = 0xD260,
    EN_NV_ID_ACC_CLASSMASK_FILE                     = 0xD261,
    EN_NV_ID_HPLMN_ACT_FILE                         = 0xD262,/* 未使用 */
    EN_NV_ID_SEQ_INFO                               = 0xD263,
    EN_NV_ID_LRPLMNSI_FILE                          = 0xD264,

    EN_NV_ID_USIM_END                               = 0xD269,

    EN_NV_ID_CAND_CELL                              = 0xD26a,
    /* modify by lishangfeng begin */
    EN_NV_CSQ_RPT_INFO                              = 0xD26b,
    /* modify by lishangfeng end */
    EN_NV_ID_TPS_SWITCH_PARA                        = 0xD26c,
    EN_NV_ID_TDS_CERSSI_REPROT_PARA                 = 0xD301,
    EN_NV_ID_SWITCH_PARA                            = 0xD275,
    EN_NV_ID_LTE_CONFIG                             = 0xD276,
    EN_NV_ID_LTE_GATE_WAY_IP                        = 0xD277,
    EN_NV_ID_SIB_SUPPORT                            = 0xD280,
    EN_NV_ID_GATE_CLOCK_SWITCH_CONFIG               = 0xD281,
    EN_NV_ID_POWER_CONFIG                           = 0xD282,
    EN_NV_ID_LTE_COMP_SWITCH                        = 0xD283,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND7                 = 0xD284,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND20                = 0xD285,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND38                = 0xD286,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND40                = 0xD287,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND64                = 0xD288,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND1                 = 0xD29f,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND2                 = 0xD26c,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND3                 = 0xD26d,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND4                 = 0xD26e,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND5                 = 0xD26f,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND6                 = 0xD270,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND8                 = 0xD271,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND9                 = 0xD272,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND10                = 0xD273,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND11                = 0xD274,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND12                = 0xD278,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND13                = 0xD279,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND14                = 0xD27a,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND15                = 0xD27b,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND16                = 0xD27c,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND17                = 0xD27d,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND18                = 0xD27e,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND19                = 0xD27f,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND21                = 0xD289,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND22                = 0xD28a,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND23                = 0xD28b,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND24                = 0xD28c,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND25                = 0xD28d,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND26                = 0xD28e,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND27                = 0xD290,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND28                = 0xD291,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND29                = 0xD292,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND30                = 0xD293,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND31                = 0xD294,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND32                = 0xD295,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND33                = 0xD296,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND34                = 0xD297,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND35                = 0xD298,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND36                = 0xD299,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND37                = 0xD29a,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND39                = 0xD29b,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND41                = 0xD29c,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND42                = 0xD29d,
    EN_NV_ID_UE_CAP_MEAS_PARA_BAND43                = 0xD29e,
    /* mul Band */
    EN_NV_ID_BAND1_CAND_INFO                        = 0xD2a0,
    EN_NV_ID_BAND2_CAND_INFO                        = 0xD2a1,
    /*EN_NV_ID_BAND3_CAND_INFO                      = 0xD2a2,*/
    EN_NV_ID_BAND4_CAND_INFO                        = 0xD2a3,
    EN_NV_ID_BAND5_CAND_INFO                        = 0xD2a4,
    EN_NV_ID_BAND6_CAND_INFO                        = 0xD2a5,
    /*EN_NV_ID_BAND7_CAND_INFO                      = 0xD2a6,*/
    EN_NV_ID_BAND8_CAND_INFO                        = 0xD2a7,
    EN_NV_ID_BAND9_CAND_INFO                        = 0xD2a8,
    EN_NV_ID_BAND10_CAND_INFO                       = 0xD2a9,
    EN_NV_ID_BAND11_CAND_INFO                       = 0xD2aa,
    EN_NV_ID_BAND12_CAND_INFO                       = 0xD2ab,
    EN_NV_ID_BAND13_CAND_INFO                       = 0xD2ac,
    EN_NV_ID_BAND14_CAND_INFO                       = 0xD2ad,
    EN_NV_ID_BAND15_CAND_INFO                       = 0xD2ae,
    EN_NV_ID_BAND16_CAND_INFO                       = 0xD2af,
    EN_NV_ID_BAND17_CAND_INFO                       = 0xD2b0,
    EN_NV_ID_BAND18_CAND_INFO                       = 0xD2b1,
    EN_NV_ID_BAND19_CAND_INFO                       = 0xD2b2,
    /*EN_NV_ID_BAND20_CAND_INFO                     = 0xD2b3,*/
    EN_NV_ID_BAND21_CAND_INFO                       = 0xD2b4,
    EN_NV_ID_BAND22_CAND_INFO                       = 0xD2b5,
    EN_NV_ID_BAND23_CAND_INFO                       = 0xD2b6,
    EN_NV_ID_BAND24_CAND_INFO                       = 0xD2b7,
    EN_NV_ID_BAND25_CAND_INFO                       = 0xD2b8,
    EN_NV_ID_BAND26_CAND_INFO                       = 0xD2b9,
    EN_NV_ID_BAND27_CAND_INFO                       = 0xD2ba,
    EN_NV_ID_BAND28_CAND_INFO                       = 0xD2bb,
    EN_NV_ID_BAND29_CAND_INFO                       = 0xD2bc,
    EN_NV_ID_BAND30_CAND_INFO                       = 0xD2bd,
    EN_NV_ID_BAND31_CAND_INFO                       = 0xD2be,
    EN_NV_ID_BAND32_CAND_INFO                       = 0xD2bf,
    EN_NV_ID_BAND33_CAND_INFO                       = 0xD2c0,
    EN_NV_ID_BAND34_CAND_INFO                       = 0xD2c1,
    EN_NV_ID_BAND35_CAND_INFO                       = 0xD2c2,
    EN_NV_ID_BAND36_CAND_INFO                       = 0xD2c3,
    EN_NV_ID_BAND37_CAND_INFO                       = 0xD2c4,
    /*EN_NV_ID_BAND38_CAND_INFO                     = 0xD2c5,*/
    /*EN_NV_ID_BAND39_CAND_INFO                     = 0xD2c6,*/
    /*EN_NV_ID_BAND40_CAND_INFO                     = 0xD2c7,*/
    /*EN_NV_ID_BAND41_CAND_INFO                     = 0xD2c8,*/
    EN_NV_ID_BAND42_CAND_INFO                       = 0xD2c9,
    EN_NV_ID_BAND43_CAND_INFO                       = 0xD2ca,
    EN_NV_ID_BAND3_CAND_INFO                        = 0xD2d1,
    EN_NV_ID_BAND7_CAND_INFO                        = 0xD2d2,
    EN_NV_ID_BAND20_CAND_INFO                       = 0xD2d3,
    EN_NV_ID_BAND38_CAND_INFO                       = 0xD2d4,
    EN_NV_ID_BAND39_CAND_INFO                       = 0xD2d5,
    EN_NV_ID_BAND40_CAND_INFO                       = 0xD2d6,
    EN_NV_ID_BAND41_CAND_INFO                       = 0xD2d7,
    EN_NV_ID_BAND64_CAND_INFO                       = 0xD2d8,
    EN_NV_ID_FLOWCTRL_CONFIG                        = 0xD2d9,

    /* gaojishan-nv-bands-cfg-begin */
#if 0
    /* gaojishan-SYSCFG-SAVENV-Begin */
    EN_NV_ID_GUL_SUPPORT_FREQ_BAND                  = 0xD2da, /*NV_UE_SUPPORT_FREQ_BAND_STRU,NV_UE_EUTRA_SUPPORT_FREQ_BAND_LIST_STRU
 */
    /* gaojishan-SYSCFG-SAVENV-End */
#endif
    EN_NV_ID_BANDXX_CAND_INFO                       = 0xD2da,
    /* gaojishan-nv-bands-cfg-end */
    /* MTC AREA LOST Begin */
    EN_NV_ID_LTE_AREA_LOST_THSD                     = 0xD2c7,
    /* MTC AREA LOST End */
    /* wangrui 20120817 begin */
    EN_NV_LWCLASH_RANGE_INFO                        = 0xD2c8,
    /* wangrui 20120817 end */

    /* 非标频段的候补信息begin */

    EN_NV_ID_BANDNon1_CAND_INFO                     = 0xD2EA,
    EN_NV_ID_BANDNon2_CAND_INFO                     = 0xD2DB,
    EN_NV_ID_BANDNon3_CAND_INFO                     = 0xD2DC,
    EN_NV_ID_BANDNon4_CAND_INFO                     = 0xD2DD,
    EN_NV_ID_BANDNon5_CAND_INFO                     = 0xD2DE,
    EN_NV_ID_BANDNon6_CAND_INFO                     = 0xD2DF,
    EN_NV_ID_BANDNon7_CAND_INFO                     = 0xD2E0,
    EN_NV_ID_BANDNon8_CAND_INFO                     = 0xD2E1,
    EN_NV_ID_BANDNon9_CAND_INFO                     = 0xD2E2,
    EN_NV_ID_BANDNon10_CAND_INFO                    = 0xD2E3,
    EN_NV_ID_BANDNon11_CAND_INFO                    = 0xD2E4,
    EN_NV_ID_BANDNon12_CAND_INFO                    = 0xD2E5,
    EN_NV_ID_BANDNon13_CAND_INFO                    = 0xD2E6,
    EN_NV_ID_BANDNon14_CAND_INFO                    = 0xD2E7,
    EN_NV_ID_BANDNon15_CAND_INFO                    = 0xD2E8,
    EN_NV_ID_BANDNon16_CAND_INFO                    = 0xD2E9,
    /* 非标频段的候补信息end */

    /* 非标频段的频段信息begin */
    EN_NV_ID_BANDNon1_BAND_INFO                     = 0xD2ED,
    EN_NV_ID_BANDNon2_BAND_INFO                     = 0xD2EE,
    EN_NV_ID_BANDNon3_BAND_INFO                     = 0xD2EF,
    EN_NV_ID_BANDNon4_BAND_INFO                     = 0xD2F0,
    EN_NV_ID_BANDNon5_BAND_INFO                     = 0xD2F1,
    EN_NV_ID_BANDNon6_BAND_INFO                     = 0xD2F2,
    EN_NV_ID_BANDNon7_BAND_INFO                     = 0xD2F3,
    EN_NV_ID_BANDNon8_BAND_INFO                     = 0xD2F4,
    EN_NV_ID_BANDNon9_BAND_INFO                     = 0xD2F5,
    EN_NV_ID_BANDNon10_BAND_INFO                    = 0xD2F6,
    EN_NV_ID_BANDNon11_BAND_INFO                    = 0xD2F7,
    EN_NV_ID_BANDNon12_BAND_INFO                    = 0xD2F8,
    EN_NV_ID_BANDNon13_BAND_INFO                    = 0xD2F9,
    EN_NV_ID_BANDNon14_BAND_INFO                    = 0xD2FA,
    EN_NV_ID_BANDNon15_BAND_INFO                    = 0xD2FB,
    EN_NV_ID_BANDNon16_BAND_INFO                    = 0xD2FC,


    /* 非标频段的频段信息end */

    EN_NV_ID_FLOWCTRL_LIMIT_CONFIG                  = 0xD2FD,
    EN_NV_ID_PS_END                                 = 0xD2ff,

    /*TDS begin*/
    /*l00280875 freq/cell lock begin*/
    EN_NV_ID_TDS_FREQ_CELL_LOCK_SWITCH         = 0xD2FE,
    /*l00280875 freq/cell lock end*/

    EN_NV_ID_TDS_UTRANCAPABILITY                    = 0xD300,
    EN_NV_ID_TDS_ENG_VERSION,
    EN_NV_ID_TDS_MAC_HSPA_CTRPARA,
    EN_NV_ID_TDS_RRC_LOCK_FREQ_SWITCH,
    EN_NV_ID_TDS_SUPPORT_FREQ_BAND,
    EN_NV_ID_TDS_FLOWCTRPARA                        = 0xD305,
    EN_NV_ID_TDS_ENG_NULLTIMER_CONTROL_TESTFLAG_EQPOUT,
    /*EN_NV_ID_TDS_ENG_RRM_TESTFLAG,
 */
    /*EN_NV_ID_TDS_EQPOUT,
 */
    EN_NV_ID_TDS_NET_SELECT_MODE,
    EN_NV_ID_TDS_PLMN_SELECT_MODE,
    EN_NV_ID_TDS_POWER_ON_ATTACH_MODE,
    EN_NV_ID_TDS_MOBILESTATION_CLASS,
    EN_NV_ID_TDS_UTRAN_CFG_DSP,
    /* TRRC CHANGE FOR BA begin */
    EN_NV_ID_TDS_BA_LIST                           = 0xD30c,
    /* TRRC CHANGE FOR BA end */
    /*dongyuxi nv modify begin
 */
    EN_NV_ID_TDS_CUSTOMIZE_CS_SERVICE               = 0xD30d,
    /*dongyuxi nv modify end
 */
    /*TDS end*/

    /* gaojishan-fast-dorm-cfg-3 */
    EN_NV_ID_CTRL_PARA                              = 0xD30e,

/* filter the serve cell begin*/
    EN_NV_ID_TDS_ALFA_FILTER                        = 0xD30f,
/* filter the serve cell end*/
    /*dongyuxi nv modify end
 */
    /*TDS end*/


    /* R10 NV修改 begin */
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND1        = 0xD310,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND2        = 0xD311,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND3        = 0xD312,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND4        = 0xD313,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND5        = 0xD314,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND6        = 0xD315,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND7        = 0xD316,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND8        = 0xD317,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND9        = 0xD318,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND10       = 0xD319,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND11       = 0xD31a,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND12       = 0xD31b,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND13       = 0xD31c,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND14       = 0xD31d,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND15       = 0xD31e,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND16       = 0xD31f,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND17       = 0xD320,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND18       = 0xD321,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND19       = 0xD322,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND20       = 0xD323,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND21       = 0xD324,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND22       = 0xD325,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND23       = 0xD326,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND24       = 0xD327,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND25       = 0xD328,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND26       = 0xD329,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND27       = 0xD32a,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND28       = 0xD32b,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND29       = 0xD32c,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND30       = 0xD32d,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND31       = 0xD32e,
    EN_NV_ID_UE_CAP_V1020_RF_MEAS_PARA_BAND32       = 0xD32f,
    /* R10 NV修改 end */

    /* add for SGLTE Begin */
    EN_NV_ID_TDS_AREA_LOST_TIMER                    = 0xD330,
    /* add for SGLTE End */

    EN_NV_ID_TDS_FEATHURE_PARA                      = 0xD331,


    /* MTC AREA LOST Begin */
    EN_NV_ID_TDS_AREA_LOST_THSD                     = 0xD332,
    /* MTC AREA LOST End */
    /* MTC RESEL Begin */
    EN_NV_ID_TDS_MTC_RESEL_PARA                     = 0xD333,
    /* MTC RESEL End */

    /* TL L2 Begin */
    EN_NV_ID_TL_L2_PARA                     = 0xD334,
    /* TL L2 End */
	
    EN_NV_ID_BSIC_FRAME_PARA                     = 0xD335,


    /* R11 NV修改 begin */
     EN_NV_ID_UE_CAP_V1090                          = 0xD340,
     EN_NV_ID_UE_CAP_V1130                          = 0xD341,
     /* R11 NV修改 end */

    EN_NV_ID_BG_LTE_LIMIT_THREHHOLD                 = 0xD38d,
    EN_NV_ID_BG_LTE_LIMIT_THREHHOLD_TYPE            = 0xD38e,
    EN_NV_ID_DPDT_SWITCH_PARA                            = 0xD38f,
    /* PS end */
    EN_NV_ID_AGC_PARA                               = 0xD3CD,

    EN_NV_ID_RFE_CONFIG_TIME                        = 0xD3cf,

    EN_NV_ID_ADC_OPEN_TIME                          = 0xD400,
    EN_NV_ID_RFIC_T1                                = 0xD401,
    EN_NV_ID_RFIC_T2                                = 0xD402,
    EN_NV_ID_RFIC_T3                                = 0xD403,
    EN_NV_ID_RFIC_T4                                = 0xD404,
    EN_NV_ID_RFIC_T7                                = 0xD405,
    EN_NV_ID_RFIC_T8                                = 0xD406,
    EN_NV_ID_RFIC_T9                                = 0xD407,
    EN_NV_ID_RFIC_T10                               = 0xD408,
    EN_NV_ID_PA_OPEN_TIME                           = 0xD409,


    EN_NV_ID_FTM_CAND_CELL_LIST_B19                 = 0xD900,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B19            = 0xD901,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B19            = 0xD902,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B19          = 0xD903,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B19          = 0xD904,
    EN_NV_ID_TEMP_SENSOR_TABLE_B19                  = 0xD90f,
    EN_NV_ID_HI6360_AGC_PARA_B19                    = 0xD918,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B19        = 0xD91b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B19        = 0xD91c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B19                  = 0xD91d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B19                   = 0xD91e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B19              = 0xD91f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B19         = 0xD920,
    EN_NV_ID_LTE_TX_ATTEN_B19                       = 0xD921,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B19          = 0xD922,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B19          = 0xD923,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B19           = 0xD924,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B19           = 0xD925,

    EN_NV_ID_LTE_TX_APT_PARA_B19                    = 0xD926,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B19            = 0xD927,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B19             = 0xD928,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B19             = 0xD929,
    EN_NV_ID_LTE_TX_MPR_B19                         = 0xD92a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B19                     = 0xD92b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B19                        = 0xD92c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B19         = 0xD92d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B19                    = 0xD92e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B19                    = 0xD92f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B19                       = 0xD930,
    EN_NV_ID_TX_RF_BIAS_B19                         = 0xD931,
    EN_NV_ID_TX_PA_TEMP_COMP_B19                    = 0xD932,
    EN_NV_ID_TX_ATTEN_TABLE_B19                     = 0xD933,
    EN_NV_ID_POWERDET_VOLTAGE_B19                   = 0xD934,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B19                = 0xD935,
    EN_NV_ID_LTE_TX_PD_PARA_B19                      = 0xD936,
    EN_NV_ID_TX_ET_BAND_PARA_B19                      = 0xD937,

    EN_NV_ID_FTM_CAND_CELL_LIST_B21                 = 0xD940,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B21            = 0xD941,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B21            = 0xD942,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B21          = 0xD943,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B21          = 0xD944,
    EN_NV_ID_TEMP_SENSOR_TABLE_B21                  = 0xD94f,
    EN_NV_ID_HI6360_AGC_PARA_B21                    = 0xD958,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B21        = 0xD95b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B21        = 0xD95c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B21                  = 0xD95d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B21                   = 0xD95e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B21              = 0xD95f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B21         = 0xD960,
    EN_NV_ID_LTE_TX_ATTEN_B21                       = 0xD961,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B21          = 0xD962,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B21          = 0xD963,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B21           = 0xD964,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B21           = 0xD965,

    EN_NV_ID_LTE_TX_APT_PARA_B21                    = 0xD966,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B21            = 0xD967,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B21             = 0xD968,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B21             = 0xD969,
    EN_NV_ID_LTE_TX_MPR_B21                         = 0xD96a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B21                     = 0xD96b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B21                        = 0xD96c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B21         = 0xD96d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B21                   = 0xD96e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B21                   = 0xD96f,
    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B21                       = 0xD970,
    EN_NV_ID_TX_RF_BIAS_B21                         = 0xD971,
    EN_NV_ID_TX_PA_TEMP_COMP_B21                   = 0xD972,
    EN_NV_ID_TX_ATTEN_TABLE_B21                        = 0xD973,
    EN_NV_ID_POWERDET_VOLTAGE_B21                        = 0xD974,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B21               = 0xD975,
    EN_NV_ID_LTE_TX_PD_PARA_B21                     = 0xD976,
    EN_NV_ID_TX_ET_BAND_PARA_B21                     = 0xD977,

    EN_NV_ID_FTM_CAND_CELL_LIST_B2                  = 0xD980,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B2             = 0xD981,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B2             = 0xD982,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B2           = 0xD983,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B2           = 0xD984,
    EN_NV_ID_TEMP_SENSOR_TABLE_B2                   = 0xD98f,
    EN_NV_ID_HI6360_AGC_PARA_B2                     = 0xD998,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B2         = 0xD99b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B2         = 0xD99c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B2                   = 0xD99d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B2                    = 0xD99e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B2               = 0xD99f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B2          = 0xD9a0,
    EN_NV_ID_LTE_TX_ATTEN_B2                        = 0xD9a1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B2           = 0xD9a2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B2           = 0xD9a3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B2            = 0xD9a4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B2            = 0xD9a5,

    EN_NV_ID_LTE_TX_APT_PARA_B2                     = 0xD9a6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B2             = 0xD9a7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B2              = 0xD9a8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B2              = 0xD9a9,
    EN_NV_ID_LTE_TX_MPR_B2                          = 0xD9aa,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B2                      = 0xD9ab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B2                         = 0xD9ac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B2         = 0xD9ad,

    EN_NV_ID_LTE_TX_AMPR_NS05_B2                   = 0xD9ae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B2                   = 0xD9af,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B2                       = 0xD9b0,
    EN_NV_ID_TX_RF_BIAS_B2                         = 0xD9b1,
    EN_NV_ID_TX_PA_TEMP_COMP_B2                   = 0xD9b2,
    EN_NV_ID_TX_ATTEN_TABLE_B2                        = 0xD9b3,
    EN_NV_ID_POWERDET_VOLTAGE_B2                        = 0xD9b4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B2                = 0xD9b5,
    EN_NV_ID_LTE_TX_PD_PARA_B2                      = 0xD9b6,
    EN_NV_ID_TX_ET_BAND_PARA_B2                     = 0xD9b7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B4                  = 0xD9c0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B4             = 0xD9c1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B4             = 0xD9c2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B4           = 0xD9c3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT4_B4           = 0xD9c4,
    EN_NV_ID_TEMP_SENSOR_TABLE_B4                   = 0xD9cf,
    EN_NV_ID_HI6360_AGC_PARA_B4                     = 0xD9d8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B4         = 0xD9db,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT4_B4         = 0xD9dc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B4                   = 0xD9dd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B4                    = 0xD9de,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B4               = 0xD9df,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B4          = 0xD9e0,
    EN_NV_ID_LTE_TX_ATTEN_B4                        = 0xD9e1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B4           = 0xD9e2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B4           = 0xD9e3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B4            = 0xD9e4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B4            = 0xD9e5,

    EN_NV_ID_LTE_TX_APT_PARA_B4                     = 0xD9e6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B4             = 0xD9e7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B4              = 0xD9e8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B4              = 0xD9e9,
    EN_NV_ID_LTE_TX_MPR_B4                          = 0xD9ea,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B4                      = 0xD9eb,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B4                         = 0xD9ec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B4         = 0xD9ed,

    EN_NV_ID_LTE_TX_AMPR_NS05_B4                   = 0xD9ee,
    EN_NV_ID_LTE_TX_AMPR_NS09_B4                   = 0xD9ef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B4                       = 0xD9f0,
    EN_NV_ID_TX_RF_BIAS_B4                         = 0xD9f1,
    EN_NV_ID_TX_PA_TEMP_COMP_B4                   = 0xD9f2,
    EN_NV_ID_TX_ATTEN_TABLE_B4                        = 0xD9f3,
    EN_NV_ID_POWERDET_VOLTAGE_B4                        = 0xD9f4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B4                = 0xD9f5,
    EN_NV_ID_LTE_TX_PD_PARA_B4                      = 0xD9f6,
    EN_NV_ID_TX_ET_BAND_PARA_B4                     = 0xD9f7,


    EN_NV_ID_FTM_CAND_CELL_LIST_B6                  = 0xDa00,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B6             = 0xDa01,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B6             = 0xDa02,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B6           = 0xDa03,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT6_B6           = 0xDa04,
    EN_NV_ID_TEMP_SENSOR_TABLE_B6                   = 0xDa0f,
    EN_NV_ID_HI6360_AGC_PARA_B6                     = 0xDa18,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B6         = 0xDa1b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT6_B6         = 0xDa1c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B6                   = 0xDa1d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B6                    = 0xDa1e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B6               = 0xDa1f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B6          = 0xDa20,
    EN_NV_ID_LTE_TX_ATTEN_B6                        = 0xDa21,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B6           = 0xDa22,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B6           = 0xDa23,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B6            = 0xDa24,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B6            = 0xDa25,

    EN_NV_ID_LTE_TX_APT_PARA_B6                     = 0xDa26,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B6             = 0xDa27,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B6              = 0xDa28,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B6              = 0xDa29,
    EN_NV_ID_LTE_TX_MPR_B6                          = 0xDa2a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B6                      = 0xDa2b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B6                         = 0xDa2c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B6         = 0xDa2d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B6                   = 0xDa2e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B6                   = 0xDa2f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B6                       = 0xDa30,
    EN_NV_ID_TX_RF_BIAS_B6                         = 0xDa31,
    EN_NV_ID_TX_PA_TEMP_COMP_B6                   = 0xDa32,
    EN_NV_ID_TX_ATTEN_TABLE_B6                        = 0xDa33,
    EN_NV_ID_POWERDET_VOLTAGE_B6                        = 0xDa34,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B6                = 0xDa35,
    EN_NV_ID_LTE_TX_PD_PARA_B6                      = 0xDa36,
    EN_NV_ID_TX_ET_BAND_PARA_B6                     = 0xDa37,


    EN_NV_ID_FTM_CAND_CELL_LIST_B9                  = 0xDa40,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B9             = 0xDa41,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B9             = 0xDa42,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B9           = 0xDa43,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT9_B9           = 0xDa44,
    EN_NV_ID_TEMP_SENSOR_TABLE_B9                   = 0xDa4f,
    EN_NV_ID_HI6360_AGC_PARA_B9                     = 0xDa58,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B9         = 0xDa5b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT9_B9         = 0xDa5c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B9                   = 0xDa5d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B9                    = 0xDa5e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B9               = 0xDa5f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B9          = 0xDa60,
    EN_NV_ID_LTE_TX_ATTEN_B9                        = 0xDa61,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B9           = 0xDa62,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B9           = 0xDa63,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B9            = 0xDa64,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B9            = 0xDa65,

    EN_NV_ID_LTE_TX_APT_PARA_B9                     = 0xDa66,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B9             = 0xDa67,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B9              = 0xDa68,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B9              = 0xDa69,
    EN_NV_ID_LTE_TX_MPR_B9                          = 0xDa6a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B9                      = 0xDa6b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B9                         = 0xDa6c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B9         = 0xDa6d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B9                   = 0xDa6e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B9                   = 0xDa6f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B9                       = 0xDa70,
    EN_NV_ID_TX_RF_BIAS_B9                         = 0xDa71,
    EN_NV_ID_TX_PA_TEMP_COMP_B9                   = 0xDa72,
    EN_NV_ID_TX_ATTEN_TABLE_B9                        = 0xDa73,
    EN_NV_ID_POWERDET_VOLTAGE_B9                        = 0xDa74,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B9                = 0xDa75,
    EN_NV_ID_LTE_TX_PD_PARA_B9                      = 0xDa76,
    EN_NV_ID_TX_ET_BAND_PARA_B9                     = 0xDa77,

    EN_NV_ID_FTM_CAND_CELL_LIST_B10                 = 0xDa80,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B10            = 0xDa81,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B10            = 0xDa82,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B10          = 0xDa83,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT10_B10         = 0xDa84,
    EN_NV_ID_TEMP_SENSOR_TABLE_B10                  = 0xDa8f,
    EN_NV_ID_HI6360_AGC_PARA_B10                    = 0xDa98,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B10        = 0xDa9b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT10_B10       = 0xDa9c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B10                  = 0xDa9d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B10                   = 0xDa9e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B10              = 0xDa9f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B10         = 0xDaa0,
    EN_NV_ID_LTE_TX_ATTEN_B10                       = 0xDaa1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B10          = 0xDaa2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B10          = 0xDaa3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B10           = 0xDaa4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B10           = 0xDaa5,

    EN_NV_ID_LTE_TX_APT_PARA_B10                    = 0xDaa6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B10            = 0xDaa7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B10             = 0xDaa8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B10             = 0xDaa9,
    EN_NV_ID_LTE_TX_MPR_B10                         = 0xDaaa,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B10                     = 0xDaab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B10                        = 0xDaac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B10         = 0xDaad,

    EN_NV_ID_LTE_TX_AMPR_NS05_B10                   = 0xDaae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B10                   = 0xDaaf,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B10                       = 0xDab0,
    EN_NV_ID_TX_RF_BIAS_B10                         = 0xDab1,
    EN_NV_ID_TX_PA_TEMP_COMP_B10                   = 0xDab2,
    EN_NV_ID_TX_ATTEN_TABLE_B10                        = 0xDab3,
    EN_NV_ID_POWERDET_VOLTAGE_B10                        = 0xDab4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B10               = 0xDab5,
    EN_NV_ID_LTE_TX_PD_PARA_B10                     = 0xDab6,
    EN_NV_ID_TX_ET_BAND_PARA_B10                    = 0xDab7,


    EN_NV_ID_FTM_CAND_CELL_LIST_B11                 = 0xDac0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B11            = 0xDac1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B11            = 0xDac2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B11          = 0xDac3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT11_B11         = 0xDac4,
    EN_NV_ID_TEMP_SENSOR_TABLE_B11                  = 0xDacf,
    EN_NV_ID_HI6360_AGC_PARA_B11                    = 0xDad8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B11        = 0xDadb,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT11_B11       = 0xDadc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B11                  = 0xDadd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B11                   = 0xDade,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B11              = 0xDadf,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B11         = 0xDae0,
    EN_NV_ID_LTE_TX_ATTEN_B11                       = 0xDae1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B11          = 0xDae2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B11          = 0xDae3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B11           = 0xDae4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B11           = 0xDae5,

    EN_NV_ID_LTE_TX_APT_PARA_B11                    = 0xDae6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B11            = 0xDae7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B11             = 0xDae8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B11             = 0xDae9,
    EN_NV_ID_LTE_TX_MPR_B11                         = 0xDaea,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B11                     = 0xDaeb,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B11                        = 0xDaec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B11         = 0xDaed,

    EN_NV_ID_LTE_TX_AMPR_NS05_B11                   = 0xDaee,
    EN_NV_ID_LTE_TX_AMPR_NS09_B11                   = 0xDaef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B11                       = 0xDaf0,
    EN_NV_ID_TX_RF_BIAS_B11                         = 0xDaf1,
    EN_NV_ID_TX_PA_TEMP_COMP_B11                   = 0xDaf2,
    EN_NV_ID_TX_ATTEN_TABLE_B11                        = 0xDaf3,
    EN_NV_ID_POWERDET_VOLTAGE_B11                        = 0xDaf4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B11               = 0xDaf5,
    EN_NV_ID_LTE_TX_PD_PARA_B11                     = 0xDaf6,
    EN_NV_ID_TX_ET_BAND_PARA_B11                    = 0xDaf7,


    EN_NV_ID_FTM_CAND_CELL_LIST_B12                 = 0xDb00,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B12            = 0xDb01,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B12            = 0xDb02,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B12          = 0xDb03,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT12_B12         = 0xDb04,
    EN_NV_ID_TEMP_SENSOR_TABLE_B12                  = 0xDb0f,
    EN_NV_ID_HI6360_AGC_PARA_B12                    = 0xDb18,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B12        = 0xDb1b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT12_B12       = 0xDb1c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B12                  = 0xDb1d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B12                   = 0xDb1e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B12              = 0xDb1f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B12         = 0xDb20,
    EN_NV_ID_LTE_TX_ATTEN_B12                       = 0xDb21,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B12          = 0xDb22,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B12          = 0xDb23,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B12           = 0xDb24,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B12           = 0xDb25,

    EN_NV_ID_LTE_TX_APT_PARA_B12                    = 0xDb26,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B12            = 0xDb27,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B12             = 0xDb28,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B12             = 0xDb29,
    EN_NV_ID_LTE_TX_MPR_B12                         = 0xDb2a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B12                     = 0xDb2b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B12                        = 0xDb2c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B12         = 0xDb2d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B12                   = 0xDb2e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B12                   = 0xDb2f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B12                       = 0xDb30,
    EN_NV_ID_TX_RF_BIAS_B12                         = 0xDb31,
    EN_NV_ID_TX_PA_TEMP_COMP_B12                   = 0xDb32,
    EN_NV_ID_TX_ATTEN_TABLE_B12                        = 0xDb33,
    EN_NV_ID_POWERDET_VOLTAGE_B12                        = 0xDb34,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B12               = 0xDb35,
    EN_NV_ID_LTE_TX_PD_PARA_B12                     = 0xDb36,
    EN_NV_ID_TX_ET_BAND_PARA_B12                    = 0xDb37,

    EN_NV_ID_FTM_CAND_CELL_LIST_B13                 = 0xDb40,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B13            = 0xDb41,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B13            = 0xDb42,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B13          = 0xDb43,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT13_B13         = 0xDb44,
    EN_NV_ID_TEMP_SENSOR_TABLE_B13                  = 0xDb4f,
    EN_NV_ID_HI6360_AGC_PARA_B13                    = 0xDb58,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B13        = 0xDb5b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT13_B13       = 0xDb5c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B13                  = 0xDb5d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B13                  = 0xDb5e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B13             = 0xDb5f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B13         = 0xDb60,
    EN_NV_ID_LTE_TX_ATTEN_B13                       = 0xDb61,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B13          = 0xDb62,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B13          = 0xDb63,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B13           = 0xDb64,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B13           = 0xDb65,

    EN_NV_ID_LTE_TX_APT_PARA_B13                    = 0xDb66,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B13            = 0xDb67,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B13             = 0xDb68,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B13             = 0xDb69,
    EN_NV_ID_LTE_TX_MPR_B13                         = 0xDb6a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B13                     = 0xDb6b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B13                        = 0xDb6c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B13         = 0xDb6d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B13                   = 0xDb6e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B13                   = 0xDb6f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B13                       = 0xDb70,
    EN_NV_ID_TX_RF_BIAS_B13                         = 0xDb71,
    EN_NV_ID_TX_PA_TEMP_COMP_B13                   = 0xDb72,
    EN_NV_ID_TX_ATTEN_TABLE_B13                        = 0xDb73,
    EN_NV_ID_POWERDET_VOLTAGE_B13                        = 0xDb74,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B13               = 0xDb75,
    EN_NV_ID_LTE_TX_PD_PARA_B13                     = 0xDb76,
    EN_NV_ID_TX_ET_BAND_PARA_B13                    = 0xDb77,

    EN_NV_ID_FTM_CAND_CELL_LIST_B14                 = 0xDb80,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B14            = 0xDb81,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B14            = 0xDb82,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B14          = 0xDb83,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT14_B14         = 0xDb84,
    EN_NV_ID_TEMP_SENSOR_TABLE_B14                  = 0xDb8f,
    EN_NV_ID_HI6360_AGC_PARA_B14                    = 0xDb98,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B14        = 0xDb9b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT14_B14       = 0xDb9c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B14                  = 0xDb9d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B14                  = 0xDb9e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B14             = 0xDb9f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B14         = 0xDba0,
    EN_NV_ID_LTE_TX_ATTEN_B14                       = 0xDba1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B14          = 0xDba2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B14          = 0xDba3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B14           = 0xDba4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B14           = 0xDba5,

    EN_NV_ID_LTE_TX_APT_PARA_B14                    = 0xDba6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B14            = 0xDba7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B14             = 0xDba8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B14             = 0xDba9,
    EN_NV_ID_LTE_TX_MPR_B14                         = 0xDbaa,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B14                     = 0xDbab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B14                        = 0xDbac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B14         = 0xDbad,

    EN_NV_ID_LTE_TX_AMPR_NS05_B14                   = 0xDbae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B14                   = 0xDbaf,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B14                       = 0xDbb0,
    EN_NV_ID_TX_RF_BIAS_B14                         = 0xDbb1,
    EN_NV_ID_TX_PA_TEMP_COMP_B14                   = 0xDbb2,
    EN_NV_ID_TX_ATTEN_TABLE_B14                        = 0xDbb3,
    EN_NV_ID_POWERDET_VOLTAGE_B14                        = 0xDbb4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B14               = 0xDbb5,
    EN_NV_ID_LTE_TX_PD_PARA_B14                     = 0xDbb6,
    EN_NV_ID_TX_ET_BAND_PARA_B14                    = 0xDbb7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B17                 = 0xDbc0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B17            = 0xDbc1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B17            = 0xDbc2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B17          = 0xDbc3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT17_B17         = 0xDbc4,
    EN_NV_ID_TEMP_SENSOR_TABLE_B17                  = 0xDbcf,
    EN_NV_ID_HI6360_AGC_PARA_B17                    = 0xDbd8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B17        = 0xDbdb,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT17_B17       = 0xDbdc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B17                  = 0xDbdd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B17                  = 0xDbde,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B17             = 0xDbdf,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B17         = 0xDbe0,
    EN_NV_ID_LTE_TX_ATTEN_B17                       = 0xDbe1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B17          = 0xDbe2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B17          = 0xDbe3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B17           = 0xDbe4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B17           = 0xDbe5,

    EN_NV_ID_LTE_TX_APT_PARA_B17                    = 0xDbe6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B17            = 0xDbe7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B17             = 0xDbe8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B17             = 0xDbe9,
    EN_NV_ID_LTE_TX_MPR_B17                         = 0xDbea,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B17                     = 0xDbeb,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B17                        = 0xDbec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B17         = 0xDbed,

    EN_NV_ID_LTE_TX_AMPR_NS05_B17                   = 0xDbee,
    EN_NV_ID_LTE_TX_AMPR_NS09_B17                   = 0xDbef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B17                       = 0xDbf0,
    EN_NV_ID_TX_RF_BIAS_B17                         = 0xDbf1,
    EN_NV_ID_TX_PA_TEMP_COMP_B17                   = 0xDbf2,
    EN_NV_ID_TX_ATTEN_TABLE_B17                        = 0xDbf3,
    EN_NV_ID_POWERDET_VOLTAGE_B17                        = 0xDbf4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B17               = 0xDbf5,
    EN_NV_ID_LTE_TX_PD_PARA_B17                     = 0xDbf6,
    EN_NV_ID_TX_ET_BAND_PARA_B17                    = 0xDbf7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B18                 = 0xDc00,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B18            = 0xDc01,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B18            = 0xDc02,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B18          = 0xDc03,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT18_B18         = 0xDc04,
    EN_NV_ID_TEMP_SENSOR_TABLE_B18                  = 0xDc0f,
    EN_NV_ID_HI6360_AGC_PARA_B18                    = 0xDc18,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B18        = 0xDc1b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT18_B18       = 0xDc1c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B18                  = 0xDc1d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B18                  = 0xDc1e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B18             = 0xDc1f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B18         = 0xDc20,
    EN_NV_ID_LTE_TX_ATTEN_B18                       = 0xDc21,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B18          = 0xDc22,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B18          = 0xDc23,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B18           = 0xDc24,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B18           = 0xDc25,

    EN_NV_ID_LTE_TX_APT_PARA_B18                    = 0xDc26,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B18            = 0xDc27,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B18             = 0xDc28,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B18             = 0xDc29,
    EN_NV_ID_LTE_TX_MPR_B18                         = 0xDc2a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B18                     = 0xDc2b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B18                        = 0xDc2c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B18         = 0xDc2d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B18                   = 0xDc2e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B18                   = 0xDc2f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B18                       = 0xDc30,
    EN_NV_ID_TX_RF_BIAS_B18                         = 0xDc31,
    EN_NV_ID_TX_PA_TEMP_COMP_B18                   = 0xDc32,
    EN_NV_ID_TX_ATTEN_TABLE_B18                        = 0xDc33,
    EN_NV_ID_POWERDET_VOLTAGE_B18                        = 0xDc34,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B18               = 0xDc35,
    EN_NV_ID_LTE_TX_PD_PARA_B18                     = 0xDc36,
    EN_NV_ID_TX_ET_BAND_PARA_B18                    = 0xDc37,

    EN_NV_ID_FTM_CAND_CELL_LIST_B22                 = 0xDc40,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B22            = 0xDc41,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B22            = 0xDc42,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B22          = 0xDc43,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT22_B22         = 0xDc44,
    EN_NV_ID_TEMP_SENSOR_TABLE_B22                  = 0xDc4f,
    EN_NV_ID_HI6360_AGC_PARA_B22                    = 0xDc58,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B22        = 0xDc5b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT22_B22       = 0xDc5c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B22                  = 0xDc5d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B22                  = 0xDc5e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B22             = 0xDc5f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B22         = 0xDc60,
    EN_NV_ID_LTE_TX_ATTEN_B22                       = 0xDc61,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B22          = 0xDc62,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B22          = 0xDc63,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B22           = 0xDc64,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B22           = 0xDc65,

    EN_NV_ID_LTE_TX_APT_PARA_B22                    = 0xDc66,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B22            = 0xDc67,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B22             = 0xDc68,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B22             = 0xDc69,
    EN_NV_ID_LTE_TX_MPR_B22                         = 0xDc6a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B22                     = 0xDc6b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B22                        = 0xDc6c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B22         = 0xDc6d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B22                   = 0xDc6e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B22                   = 0xDc6f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B22                       = 0xDc70,
    EN_NV_ID_TX_RF_BIAS_B22                         = 0xDc71,
    EN_NV_ID_TX_PA_TEMP_COMP_B22                   = 0xDc72,
    EN_NV_ID_TX_ATTEN_TABLE_B22                        = 0xDc73,
    EN_NV_ID_POWERDET_VOLTAGE_B22                        = 0xDc74,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B22               = 0xDc75,
    EN_NV_ID_LTE_TX_PD_PARA_B22                     = 0xDC36,
    EN_NV_ID_TX_ET_BAND_PARA_B22                    = 0xDc77,

    EN_NV_ID_FTM_CAND_CELL_LIST_B23                 = 0xDc80,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B23            = 0xDc81,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B23            = 0xDc82,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B23          = 0xDc83,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT23_B23         = 0xDc84,
    EN_NV_ID_TEMP_SENSOR_TABLE_B23                  = 0xDc8f,
    EN_NV_ID_HI6360_AGC_PARA_B23                    = 0xDc98,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B23        = 0xDc9b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT23_B23       = 0xDc9c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B23                  = 0xDc9d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B23                   = 0xDc9e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B23              = 0xDc9f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B23         = 0xDca0,
    EN_NV_ID_LTE_TX_ATTEN_B23                       = 0xDca1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B23          = 0xDca2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B23          = 0xDca3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B23           = 0xDca4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B23           = 0xDca5,

    EN_NV_ID_LTE_TX_APT_PARA_B23                    = 0xDca6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B23            = 0xDca7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B23             = 0xDca8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B23             = 0xDca9,
    EN_NV_ID_LTE_TX_MPR_B23                         = 0xDcaa,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B23                     = 0xDcab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B23                        = 0xDcac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B23         = 0xDcad,

    EN_NV_ID_LTE_TX_AMPR_NS05_B23                   = 0xDcae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B23                   = 0xDcaf,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B23                       = 0xDcb0,
    EN_NV_ID_TX_RF_BIAS_B23                         = 0xDcb1,
    EN_NV_ID_TX_PA_TEMP_COMP_B23                   = 0xDcb2,
    EN_NV_ID_TX_ATTEN_TABLE_B23                        = 0xDcb3,
    EN_NV_ID_POWERDET_VOLTAGE_B23                        = 0xDcb4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B23               = 0xDcb5,
    EN_NV_ID_LTE_TX_PD_PARA_B23                     = 0xDcb6,
    EN_NV_ID_TX_ET_BAND_PARA_B23                    = 0xDcb7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B24                 = 0xDcc0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B24            = 0xDcc1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B24            = 0xDcc2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B24          = 0xDcc3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT24_B24         = 0xDcc4,
    EN_NV_ID_TEMP_SENSOR_TABLE_B24                  = 0xDccf,
    EN_NV_ID_HI6360_AGC_PARA_B24                    = 0xDcd8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B24        = 0xDcdb,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT24_B24       = 0xDcdc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B24                  = 0xDcdd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B24                   = 0xDcde,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B24              = 0xDcdf,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B24         = 0xDce0,
    EN_NV_ID_LTE_TX_ATTEN_B24                       = 0xDce1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B24          = 0xDce2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B24          = 0xDce3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B24           = 0xDce4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B24           = 0xDce5,

    EN_NV_ID_LTE_TX_APT_PARA_B24                    = 0xDce6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B24            = 0xDce7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B24             = 0xDce8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B24             = 0xDce9,
    EN_NV_ID_LTE_TX_MPR_B24                         = 0xDcea,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B24                     = 0xDceb,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B24                        = 0xDcec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B24         = 0xDced,

    EN_NV_ID_LTE_TX_AMPR_NS05_B24                   = 0xDcee,
    EN_NV_ID_LTE_TX_AMPR_NS09_B24                   = 0xDcef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B24                       = 0xDcf0,
    EN_NV_ID_TX_RF_BIAS_B24                         = 0xDcf1,
    EN_NV_ID_TX_PA_TEMP_COMP_B24                   = 0xDcf2,
    EN_NV_ID_TX_ATTEN_TABLE_B24                        = 0xDcf3,
    EN_NV_ID_POWERDET_VOLTAGE_B24                        = 0xDcf4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B24               = 0xDcf5,
    EN_NV_ID_LTE_TX_PD_PARA_B24                     = 0xDcf6,
    EN_NV_ID_TX_ET_BAND_PARA_B24                    = 0xDcf7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B25                 = 0xDd00,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B25            = 0xDd01,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B25            = 0xDd02,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B25          = 0xDd03,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT25_B25         = 0xDd04,
    EN_NV_ID_TEMP_SENSOR_TABLE_B25                  = 0xDd0f,
    EN_NV_ID_HI6360_AGC_PARA_B25                    = 0xDd18,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B25        = 0xDd1b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT25_B25       = 0xDd1c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B25                  = 0xDd1d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B25                   = 0xDd1e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B25              = 0xDd1f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B25         = 0xDd20,
    EN_NV_ID_LTE_TX_ATTEN_B25                       = 0xDd21,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B25          = 0xDd22,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B25          = 0xDd23,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B25           = 0xDd24,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B25           = 0xDd25,

    EN_NV_ID_LTE_TX_APT_PARA_B25                    = 0xDd26,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B25            = 0xDd27,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B25             = 0xDd28,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B25             = 0xDd29,
    EN_NV_ID_LTE_TX_MPR_B25                         = 0xDd2a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B25                     = 0xDd2b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B25                        = 0xDd2c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B25         = 0xDd2d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B25                   = 0xDd2e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B25                   = 0xDd2f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B25                       = 0xDd30,
    EN_NV_ID_TX_RF_BIAS_B25                         = 0xDd31,
    EN_NV_ID_TX_PA_TEMP_COMP_B25                   = 0xDd32,
    EN_NV_ID_TX_ATTEN_TABLE_B25                        = 0xDd33,
    EN_NV_ID_POWERDET_VOLTAGE_B25                        = 0xDd34,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B25               = 0xDd35,
    EN_NV_ID_LTE_TX_PD_PARA_B25                     = 0xDd36,
    EN_NV_ID_TX_ET_BAND_PARA_B25                    = 0xDd37,

    EN_NV_ID_FTM_CAND_CELL_LIST_B33                 = 0xDd40,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B33            = 0xDd41,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B33            = 0xDd42,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B33          = 0xDd43,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT33_B33         = 0xDd44,
    EN_NV_ID_TEMP_SENSOR_TABLE_B33                  = 0xDd4f,
    EN_NV_ID_HI6360_AGC_PARA_B33                    = 0xDd58,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B33        = 0xDd5b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT33_B33       = 0xDd5c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B33                  = 0xDd5d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B33                   = 0xDd5e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B33              = 0xDd5f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B33         = 0xDd60,
    EN_NV_ID_LTE_TX_ATTEN_B33                       = 0xDd61,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B33          = 0xDd62,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B33          = 0xDd63,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B33           = 0xDd64,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B33           = 0xDd65,

    EN_NV_ID_LTE_TX_APT_PARA_B33                    = 0xDd66,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B33            = 0xDd67,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B33             = 0xDd68,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B33             = 0xDd69,
    EN_NV_ID_LTE_TX_MPR_B33                         = 0xDd6a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B33                     = 0xDd6b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B33                        = 0xDd6c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B33         = 0xDd6d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B33                   = 0xDd6e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B33                   = 0xDd6f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B33                       = 0xDd70,
    EN_NV_ID_TX_RF_BIAS_B33                         = 0xDd71,
    EN_NV_ID_TX_PA_TEMP_COMP_B33                   = 0xDd72,
    EN_NV_ID_TX_ATTEN_TABLE_B33                        = 0xDd73,
    EN_NV_ID_POWERDET_VOLTAGE_B33                        = 0xDd74,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B33               = 0xDd75,
    EN_NV_ID_LTE_TX_PD_PARA_B33                     = 0xDd76,
    EN_NV_ID_TX_ET_BAND_PARA_B33                    = 0xDd77,

    EN_NV_ID_FTM_CAND_CELL_LIST_B34                 = 0xDd80,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B34            = 0xDd81,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B34            = 0xDd82,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B34          = 0xDd83,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT34_B34         = 0xDd84,
    EN_NV_ID_TEMP_SENSOR_TABLE_B34                  = 0xDd8f,
    EN_NV_ID_HI6360_AGC_PARA_B34                    = 0xDd98,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B34        = 0xDd9b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT34_B34       = 0xDd9c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B34                  = 0xDd9d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B34                   = 0xDd9e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B34              = 0xDd9f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B34         = 0xDda0,
    EN_NV_ID_LTE_TX_ATTEN_B34                       = 0xDda1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B34          = 0xDda2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B34          = 0xDda3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B34           = 0xDda4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B34           = 0xDda5,

    EN_NV_ID_LTE_TX_APT_PARA_B34                    = 0xDda6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B34            = 0xDda7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B34             = 0xDda8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B34             = 0xDda9,
    EN_NV_ID_LTE_TX_MPR_B34                         = 0xDdaa,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B34                     = 0xDdab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B34                        = 0xDdac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B34         = 0xDdad,

    EN_NV_ID_LTE_TX_AMPR_NS05_B34                   = 0xDdae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B34                   = 0xDdaf,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B34                       = 0xDdb0,
    EN_NV_ID_TX_RF_BIAS_B34                         = 0xDdb1,
    EN_NV_ID_TX_PA_TEMP_COMP_B34                   = 0xDdb2,
    EN_NV_ID_TX_ATTEN_TABLE_B34                        = 0xDdb3,
    EN_NV_ID_POWERDET_VOLTAGE_B34                        = 0xDdb4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B34               = 0xDdb5,
    EN_NV_ID_LTE_TX_PD_PARA_B34                     = 0xDdb6,
    EN_NV_ID_TX_ET_BAND_PARA_B34                    = 0xDdb7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B35                 = 0xDdc0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B35            = 0xDdc1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B35            = 0xDdc2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B35          = 0xDdc3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT35_B35         = 0xDdc4,
    EN_NV_ID_TEMP_SENSOR_TABLE_B35                  = 0xDdcf,
    EN_NV_ID_HI6360_AGC_PARA_B35                    = 0xDdd8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B35        = 0xDddb,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT35_B35       = 0xDddc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B35                  = 0xDddd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B35                   = 0xDdde,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B35              = 0xDddf,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B35         = 0xDde0,
    EN_NV_ID_LTE_TX_ATTEN_B35                       = 0xDde1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B35          = 0xDde2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B35          = 0xDde3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B35           = 0xDde4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B35           = 0xDde5,

    EN_NV_ID_LTE_TX_APT_PARA_B35                    = 0xDde6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B35            = 0xDde7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B35             = 0xDde8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B35             = 0xDde9,
    EN_NV_ID_LTE_TX_MPR_B35                         = 0xDdea,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B35                     = 0xDdeb,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B35                        = 0xDdec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B35         = 0xDded,

    EN_NV_ID_LTE_TX_AMPR_NS05_B35                   = 0xDdee,
    EN_NV_ID_LTE_TX_AMPR_NS09_B35                   = 0xDdef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B35                       = 0xDdf0,
    EN_NV_ID_TX_RF_BIAS_B35                         = 0xDdf1,
    EN_NV_ID_TX_PA_TEMP_COMP_B35                   = 0xDdf2,
    EN_NV_ID_TX_ATTEN_TABLE_B35                        = 0xDdf3,
    EN_NV_ID_POWERDET_VOLTAGE_B35                        = 0xDdf4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B35               = 0xDdf5,
    EN_NV_ID_LTE_TX_PD_PARA_B35                     = 0xDdf6,
    EN_NV_ID_TX_ET_BAND_PARA_B35                    = 0xDdf7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B36                 = 0xDe00,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B36            = 0xDe01,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B36            = 0xDe02,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B36          = 0xDe03,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT36_B36         = 0xDe04,
    EN_NV_ID_TEMP_SENSOR_TABLE_B36                  = 0xDe0f,
    EN_NV_ID_HI6360_AGC_PARA_B36                    = 0xDe18,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B36        = 0xDe1b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT36_B36       = 0xDe1c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B36                  = 0xDe1d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B36                   = 0xDe1e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B36              = 0xDe1f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B36         = 0xDe20,
    EN_NV_ID_LTE_TX_ATTEN_B36                       = 0xDe21,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B36          = 0xDe22,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B36          = 0xDe23,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B36           = 0xDe24,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B36           = 0xDe25,

    EN_NV_ID_LTE_TX_APT_PARA_B36                    = 0xDe26,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B36            = 0xDe27,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B36             = 0xDe28,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B36             = 0xDe29,
    EN_NV_ID_LTE_TX_MPR_B36                         = 0xDe2a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B36                     = 0xDe2b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B36                        = 0xDe2c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B36         = 0xDe2d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B36                   = 0xDe2e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B36                   = 0xDe2f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B36                       = 0xDe30,
    EN_NV_ID_TX_RF_BIAS_B36                         = 0xDe31,
    EN_NV_ID_TX_PA_TEMP_COMP_B36                   = 0xDe32,
    EN_NV_ID_TX_ATTEN_TABLE_B36                        = 0xDe33,
    EN_NV_ID_POWERDET_VOLTAGE_B36                        = 0xDe34,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B36               = 0xDe35,
    EN_NV_ID_LTE_TX_PD_PARA_B36                     = 0xDe36,
    EN_NV_ID_TX_ET_BAND_PARA_B36                    = 0xDe37,

    EN_NV_ID_FTM_CAND_CELL_LIST_B37                 = 0xDe40,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B37            = 0xDe41,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B37            = 0xDe42,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B37          = 0xDe43,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT37_B37         = 0xDe44,
    EN_NV_ID_TEMP_SENSOR_TABLE_B37                  = 0xDe4f,
    EN_NV_ID_HI6360_AGC_PARA_B37                    = 0xDe58,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B37        = 0xDe5b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT37_B37       = 0xDe5c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B37                  = 0xDe5d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B37                   = 0xDe5e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B37              = 0xDe5f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B37         = 0xDe60,
    EN_NV_ID_LTE_TX_ATTEN_B37                       = 0xDe61,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B37          = 0xDe62,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B37          = 0xDe63,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B37           = 0xDe64,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B37           = 0xDe65,

    EN_NV_ID_LTE_TX_APT_PARA_B37                    = 0xDe66,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B37            = 0xDe67,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B37             = 0xDe68,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B37             = 0xDe69,
    EN_NV_ID_LTE_TX_MPR_B37                         = 0xDe6a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B37                     = 0xDe6b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B37                        = 0xDe6c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B37         = 0xDe6d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B37                   = 0xDe6e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B37                   = 0xDe6f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B37                       = 0xDe70,
    EN_NV_ID_TX_RF_BIAS_B37                         = 0xDe71,
    EN_NV_ID_TX_PA_TEMP_COMP_B37                   = 0xDe72,
    EN_NV_ID_TX_ATTEN_TABLE_B37                        = 0xDe73,
    EN_NV_ID_POWERDET_VOLTAGE_B37                        = 0xDe74,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B37               = 0xDe75,
    EN_NV_ID_LTE_TX_PD_PARA_B37                     = 0xDe76,
    EN_NV_ID_TX_ET_BAND_PARA_B37                    = 0xDe77,

    EN_NV_ID_FTM_CAND_CELL_LIST_B42                 = 0xDe80,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B42            = 0xDe81,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B42            = 0xDe82,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B42          = 0xDe83,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT42_B42         = 0xDe84,
    EN_NV_ID_TEMP_SENSOR_TABLE_B42                  = 0xDe8f,
    EN_NV_ID_HI6360_AGC_PARA_B42                    = 0xDe98,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B42        = 0xDe9b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT42_B42       = 0xDe9c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B42                  = 0xDe9d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B42                   = 0xDe9e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B42              = 0xDe9f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B42         = 0xDea0,
    EN_NV_ID_LTE_TX_ATTEN_B42                       = 0xDea1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B42          = 0xDea2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B42          = 0xDea3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B42           = 0xDea4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B42           = 0xDea5,

    EN_NV_ID_LTE_TX_APT_PARA_B42                    = 0xDea6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B42            = 0xDea7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B42             = 0xDea8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B42             = 0xDea9,
    EN_NV_ID_LTE_TX_MPR_B42                         = 0xDeaa,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B42                     = 0xDeab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B42                        = 0xDeac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B42         = 0xDead,

    EN_NV_ID_LTE_TX_AMPR_NS05_B42                   = 0xDeae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B42                   = 0xDeaf,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B42                       = 0xDeb0,
    EN_NV_ID_TX_RF_BIAS_B42                         = 0xDeb1,
    EN_NV_ID_TX_PA_TEMP_COMP_B42                   = 0xDeb2,
    EN_NV_ID_TX_ATTEN_TABLE_B42                        = 0xDeb3,
    EN_NV_ID_POWERDET_VOLTAGE_B42                        = 0xDeb4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B42               = 0xDeb5,
    EN_NV_ID_LTE_TX_PD_PARA_B42						=0xDeb6,
    EN_NV_ID_TX_ET_BAND_PARA_B42                    = 0xDeb7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B43                 = 0xDec0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B43            = 0xDec1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B43            = 0xDec2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B43          = 0xDec3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT43_B43         = 0xDec4,
    EN_NV_ID_TEMP_SENSOR_TABLE_B43                  = 0xDecf,
    EN_NV_ID_HI6360_AGC_PARA_B43                    = 0xDed8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B43        = 0xDedb,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT43_B43       = 0xDedc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B43                  = 0xDedd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B43                   = 0xDede,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B43              = 0xDedf,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B43         = 0xDee0,
    EN_NV_ID_LTE_TX_ATTEN_B43                       = 0xDee1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B43          = 0xDee2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B43          = 0xDee3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B43           = 0xDee4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B43           = 0xDee5,

    EN_NV_ID_LTE_TX_APT_PARA_B43                    = 0xDee6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B43            = 0xDee7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B43             = 0xDee8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B43             = 0xDee9,
    EN_NV_ID_LTE_TX_MPR_B43                         = 0xDeea,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B43                     = 0xDeeb,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B43                        = 0xDeec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B43         = 0xDeed,

    EN_NV_ID_LTE_TX_AMPR_NS05_B43                   = 0xDeee,
    EN_NV_ID_LTE_TX_AMPR_NS09_B43                   = 0xDeef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B43                       = 0xDef0,
    EN_NV_ID_TX_RF_BIAS_B43                         = 0xDef1,
    EN_NV_ID_TX_PA_TEMP_COMP_B43                   = 0xDef2,
    EN_NV_ID_TX_ATTEN_TABLE_B43                        = 0xDef3,
    EN_NV_ID_POWERDET_VOLTAGE_B43                        = 0xDef4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B43               = 0xDef5,
    EN_NV_ID_LTE_TX_PD_PARA_B43						= 0xDef6,
    EN_NV_ID_TX_ET_BAND_PARA_B43                     = 0xDef7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B39                 = 0xDf00,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B39            = 0xDf01,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B39            = 0xDf02,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B39          = 0xDf03,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT36_B39         = 0xDf04,
    EN_NV_ID_TEMP_SENSOR_TABLE_B39                  = 0xDf0f,
    EN_NV_ID_HI6360_AGC_PARA_B39                    = 0xDf18,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B39        = 0xDf1b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT36_B39       = 0xDf1c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B39                  = 0xDf1d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B39                   = 0xDf1e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B39              = 0xDf1f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B39         = 0xDf20,
    EN_NV_ID_LTE_TX_ATTEN_B39                       = 0xDf21,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B39          = 0xDf22,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B39          = 0xDf23,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B39           = 0xDf24,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B39           = 0xDf25,

    EN_NV_ID_LTE_TX_APT_PARA_B39                    = 0xDf26,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B39            = 0xDf27,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B39             = 0xDf28,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B39             = 0xDf29,
    EN_NV_ID_LTE_TX_MPR_B39                         = 0xDf2a,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B39                     = 0xDf2b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B39                        = 0xDf2c,

    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B39         = 0xDf2d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B39                   = 0xDf2e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B39                   = 0xDf2f,
    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B39                       = 0xDf30,
    EN_NV_ID_TX_RF_BIAS_B39                         = 0xDf31,
    EN_NV_ID_TX_PA_TEMP_COMP_B39                   = 0xDf32,
    EN_NV_ID_TX_ATTEN_TABLE_B39                        = 0xDf33,
    EN_NV_ID_POWERDET_VOLTAGE_B39                        = 0xDf34,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B39               = 0xDf35,
    EN_NV_ID_LTE_TX_PD_PARA_B39                     = 0xDf36,
    EN_NV_ID_TX_ET_BAND_PARA_B39                    = 0xDf37,



    EN_NV_ID_FTM_CAND_CELL_LIST_B20                 = 0xD600,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B20            = 0xD601,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B20            = 0xD602,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B20          = 0xD603,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B20          = 0xD604,
    EN_NV_ID_TEMP_SENSOR_TABLE_B20                  = 0xD60f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B20        = 0xD61b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B20        = 0xD61c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B20                  = 0xD61d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B20                   = 0xD61e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B20              = 0xD61f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B20         = 0xD620,
    EN_NV_ID_LTE_TX_ATTEN_B20                       = 0xD621,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B20          = 0xD622,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B20          = 0xD623,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B20           = 0xD624,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B20           = 0xD625,
    EN_NV_ID_LTE_TX_APT_PARA_B20                    = 0xD626,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B20            = 0xD627,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B20             = 0xD628,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B20             = 0xD629,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B20                         = 0xD62a,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B20                     = 0xD62b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B20                        = 0xD62c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B20         = 0xD62d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B20                   = 0xD62e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B20                   = 0xD62f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B20                       = 0xD630,
    EN_NV_ID_TX_RF_BIAS_B20                         = 0xD631,
    EN_NV_ID_TX_PA_TEMP_COMP_B20                   = 0xD632,
    EN_NV_ID_TX_ATTEN_TABLE_B20                        = 0xD633,
    EN_NV_ID_POWERDET_VOLTAGE_B20                        = 0xD634,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B20               = 0xD635,
    EN_NV_ID_LTE_TX_PD_PARA_B20                     = 0xD636,
    EN_NV_ID_TX_ET_BAND_PARA_B20                    = 0xD637,

    EN_NV_ID_FTM_CAND_CELL_LIST_B40                 = 0xD640,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B40            = 0xD641,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B40            = 0xD642,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B40          = 0xD643,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B40          = 0xD644,
    EN_NV_ID_TEMP_SENSOR_TABLE_B40                  = 0xD64f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B40        = 0xD65b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B40        = 0xD65c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B40                  = 0xD65d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B40                   = 0xD65e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B40              = 0xD65f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B40         = 0xD660,
    EN_NV_ID_LTE_TX_ATTEN_B40                       = 0xD661,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B40          = 0xD662,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B40          = 0xD663,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B40           = 0xD664,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B40           = 0xD665,
    EN_NV_ID_LTE_TX_APT_PARA_B40                    = 0xD666,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B40            = 0xD667,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B40             = 0xD668,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B40             = 0xD669,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B40                         = 0xD66a,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B40                     = 0xD66b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B40                        = 0xD66c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B40         = 0xD66d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B40                   = 0xD66e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B40                   = 0xD66f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B40                       = 0xD670,
    EN_NV_ID_TX_RF_BIAS_B40                         = 0xD671,
    EN_NV_ID_TX_PA_TEMP_COMP_B40                   = 0xD672,
    EN_NV_ID_TX_ATTEN_TABLE_B40                        = 0xD673,
    EN_NV_ID_POWERDET_VOLTAGE_B40                        = 0xD674,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B40               = 0xD675,
    EN_NV_ID_LTE_TX_PD_PARA_B40                     = 0xD676,
    EN_NV_ID_TX_ET_BAND_PARA_B40                    = 0xD677,

    EN_NV_ID_FTM_CAND_CELL_LIST_B38                 = 0xD680,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B38            = 0xD681,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B38            = 0xD682,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B38          = 0xD683,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B38          = 0xD684,
    EN_NV_ID_TEMP_SENSOR_TABLE_B38                  = 0xD68f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B38        = 0xD69b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B38        = 0xD69c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B38                  = 0xD69d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B38                   = 0xD69e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B38              = 0xD69f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B38         = 0xD6a0,
    EN_NV_ID_LTE_TX_ATTEN_B38                       = 0xD6a1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B38          = 0xD6a2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B38          = 0xD6a3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B38           = 0xD6a4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B38           = 0xD6a5,
    EN_NV_ID_LTE_TX_APT_PARA_B38                    = 0xD6a6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B38            = 0xD6a7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B38             = 0xD6a8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B38             = 0xD6a9,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B38                         = 0xD6aa,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B38                     = 0xD6ab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B38                        = 0xD6ac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B38         = 0xD6ad,

    EN_NV_ID_LTE_TX_AMPR_NS05_B38                   = 0xD6ae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B38                   = 0xD6af,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B38                       = 0xD6b0,
    EN_NV_ID_TX_RF_BIAS_B38                         = 0xD6b1,
    EN_NV_ID_TX_PA_TEMP_COMP_B38                   = 0xD6b2,
    EN_NV_ID_TX_ATTEN_TABLE_B38                        = 0xD6b3,
    EN_NV_ID_POWERDET_VOLTAGE_B38                        = 0xD6b4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B38               = 0xD6b5,
    EN_NV_ID_LTE_TX_PD_PARA_B38                     = 0xD6b6,
    EN_NV_ID_TX_ET_BAND_PARA_B38                    = 0xD6b7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B41                 = 0xD6c0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B41            = 0xD6c1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B41            = 0xD6c2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B41          = 0xD6c3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B41          = 0xD6c4,
    EN_NV_ID_TEMP_SENSOR_TABLE_B41                  = 0xD6cf,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B41        = 0xD6Db,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B41        = 0xD6Dc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B41                  = 0xD6Dd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B41                   = 0xD6De,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B41              = 0xD6Df,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B41         = 0xD6E0,
    EN_NV_ID_LTE_TX_ATTEN_B41                       = 0xD6E1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B41          = 0xD6E2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B41          = 0xD6E3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B41           = 0xD6E4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B41           = 0xD6E5,
    EN_NV_ID_LTE_TX_APT_PARA_B41                    = 0xD6E6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B41            = 0xD6E7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B41             = 0xD6E8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B41             = 0xD6E9,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B41                         = 0xD6ea,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B41                     = 0xD6eb,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B41                        = 0xD6ec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B41         = 0xD6ed,

    EN_NV_ID_LTE_TX_AMPR_NS05_B41                   = 0xD6ee,
    EN_NV_ID_LTE_TX_AMPR_NS09_B41                   = 0xD6ef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B41                       = 0xD6f0,
    EN_NV_ID_TX_RF_BIAS_B41                         = 0xD6f1,
    EN_NV_ID_TX_PA_TEMP_COMP_B41                   = 0xD6f2,
    EN_NV_ID_TX_ATTEN_TABLE_B41                    = 0xD6f3,
    EN_NV_ID_POWERDET_VOLTAGE_B41                   = 0xD6f4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B41               = 0xD6f5,
    EN_NV_ID_LTE_TX_PD_PARA_B41                     = 0xD6f6,
    EN_NV_ID_TX_ET_BAND_PARA_B41                    = 0xD6f7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B7                  = 0xD700,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B7             = 0xD701,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B7             = 0xD702,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B7           = 0xD703,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B7           = 0xD704,
    EN_NV_ID_TEMP_SENSOR_TABLE_B7                   = 0xD70f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B7         = 0xD71b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B7         = 0xD71c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B7                   = 0xD71d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B7                    = 0xD71e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B7               = 0xD71f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B7          = 0xD720,
    EN_NV_ID_LTE_TX_ATTEN_B7                        = 0xD721,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B7           = 0xD722,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B7           = 0xD723,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B7            = 0xD724,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B7            = 0xD725,
    EN_NV_ID_LTE_TX_APT_PARA_B7                     = 0xD726,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B7             = 0xD727,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B7              = 0xD728,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B7              = 0xD729,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B7                          = 0xD72a,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B7                      = 0xD72b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B7                         = 0xD72c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B7         = 0xD72d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B7                   = 0xD72e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B7                   = 0xD72f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B7                       = 0xD730,
    EN_NV_ID_TX_RF_BIAS_B7                         = 0xD731,
    EN_NV_ID_TX_PA_TEMP_COMP_B7                   = 0xD732,
    EN_NV_ID_TX_ATTEN_TABLE_B7                        = 0xD733,
    EN_NV_ID_POWERDET_VOLTAGE_B7                        = 0xD734,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B7               = 0xD735,
    EN_NV_ID_LTE_TX_PD_PARA_B7                      = 0xD736,
    EN_NV_ID_TX_ET_BAND_PARA_B7                    = 0xD737,

    EN_NV_ID_FTM_CAND_CELL_LIST_B3                  = 0xD800,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B3             = 0xD801,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B3             = 0xD802,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B3           = 0xD803,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B3           = 0xD804,
    EN_NV_ID_TEMP_SENSOR_TABLE_B3                   = 0xD80f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B3         = 0xD81b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B3         = 0xD81c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B3                   = 0xD81d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B3                    = 0xD81e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B3               = 0xD81f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B3          = 0xD820,
    EN_NV_ID_LTE_TX_ATTEN_B3                        = 0xD821,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B3           = 0xD822,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B3           = 0xD823,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B3            = 0xD824,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B3            = 0xD825,
    EN_NV_ID_LTE_TX_APT_PARA_B3                     = 0xD826,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B3             = 0xD827,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B3              = 0xD828,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B3              = 0xD829,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B3                          = 0xD82a,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B3                      = 0xD82b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B3                         = 0xD82c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B3         = 0xD82d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B3                   = 0xD82e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B3                   = 0xD82f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B3                       = 0xD830,
    EN_NV_ID_TX_RF_BIAS_B3                         = 0xD831,
    EN_NV_ID_TX_PA_TEMP_COMP_B3                   = 0xD832,
    EN_NV_ID_TX_ATTEN_TABLE_B3                        = 0xD833,
    EN_NV_ID_POWERDET_VOLTAGE_B3                        = 0xD834,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B3               = 0xD835,
    EN_NV_ID_LTE_TX_PD_PARA_B3                      = 0xD836,
    EN_NV_ID_TX_ET_BAND_PARA_B3                     = 0xD837,

    EN_NV_ID_FTM_CAND_CELL_LIST_B1                  = 0xD840,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B1             = 0xD841,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B1             = 0xD842,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B1           = 0xD843,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B1           = 0xD844,
    EN_NV_ID_TEMP_SENSOR_TABLE_B1                   = 0xD84f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B1         = 0xD85b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B1         = 0xD85c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B1                   = 0xD85d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B1                    = 0xD85e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B1               = 0xD85f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B1          = 0xD860,
    EN_NV_ID_LTE_TX_ATTEN_B1                        = 0xD861,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B1           = 0xD862,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B1           = 0xD863,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B1            = 0xD864,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B1            = 0xD865,
    EN_NV_ID_LTE_TX_APT_PARA_B1                     = 0xD866,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B1             = 0xD867,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B1              = 0xD868,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B1              = 0xD869,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B1                          = 0xD86a,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B1                      = 0xD86b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B1                         = 0xD86c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B1         = 0xD86d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B1                   = 0xD86e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B1                   = 0xD86f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B1                       = 0xD870,
    EN_NV_ID_TX_RF_BIAS_B1                         = 0xD871,
    EN_NV_ID_TX_PA_TEMP_COMP_B1                   = 0xD872,
    EN_NV_ID_TX_ATTEN_TABLE_B1                        = 0xD873,
    EN_NV_ID_POWERDET_VOLTAGE_B1                        = 0xD874,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B1                = 0xD875,
    EN_NV_ID_LTE_TX_PD_PARA_B1                      = 0xD876,
    EN_NV_ID_TX_ET_BAND_PARA_B1                     = 0xD877,

    EN_NV_ID_FTM_CAND_CELL_LIST_B5                  = 0xD880,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B5             = 0xD881,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B5             = 0xD882,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B5           = 0xD883,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B5           = 0xD884,
    EN_NV_ID_TEMP_SENSOR_TABLE_B5                   = 0xD88f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B5         = 0xD89b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B5         = 0xD89c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B5                   = 0xD89d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B5                    = 0xD89e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B5               = 0xD89f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B5          = 0xD8a0,
    EN_NV_ID_LTE_TX_ATTEN_B5                        = 0xD8a1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B5           = 0xD8a2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B5           = 0xD8a3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B5            = 0xD8a4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B5            = 0xD8a5,
    EN_NV_ID_LTE_TX_APT_PARA_B5                     = 0xD8a6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B5             = 0xD8a7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B5              = 0xD8a8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B5              = 0xD8a9,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B5                          = 0xD8aa,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B5                      = 0xD8ab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B5                         = 0xD8ac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B5         = 0xD8ad,

    EN_NV_ID_LTE_TX_AMPR_NS05_B5                   = 0xD8ae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B5                   = 0xD8af,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B5                       = 0xD8b0,
    EN_NV_ID_TX_RF_BIAS_B5                         = 0xD8b1,
    EN_NV_ID_TX_PA_TEMP_COMP_B5                   = 0xD8b2,
    EN_NV_ID_TX_ATTEN_TABLE_B5                        = 0xD8b3,
    EN_NV_ID_POWERDET_VOLTAGE_B5                        = 0xD8b4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B5               = 0xD8b5,
    EN_NV_ID_LTE_TX_PD_PARA_B5                      = 0xD8b6,
    EN_NV_ID_TX_ET_BAND_PARA_B5                     = 0xD8b7,

    EN_NV_ID_FTM_CAND_CELL_LIST_B8                  = 0xD8c0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B8             = 0xD8c1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B8             = 0xD8c2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B8           = 0xD8c3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B8           = 0xD8c4,
    EN_NV_ID_TEMP_SENSOR_TABLE_B8                   = 0xD8cf,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B8         = 0xD8db,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B8         = 0xD8dc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B8                   = 0xD8dd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B8                    = 0xD8de,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B8               = 0xD8df,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B8          = 0xD8e0,
    EN_NV_ID_LTE_TX_ATTEN_B8                        = 0xD8e1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B8           = 0xD8e2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B8           = 0xD8e3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B8            = 0xD8e4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B8            = 0xD8e5,
    EN_NV_ID_LTE_TX_APT_PARA_B8                     = 0xD8e6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B8             = 0xD8e7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B8              = 0xD8e8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B8              = 0xD8e9,

    /* modify by wangrui mpr begin */
    EN_NV_ID_LTE_TX_MPR_B8                          = 0xD8ea,
    /* modify by wangrui mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B8                      = 0xD8eb,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B8                         = 0xD8ec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B8         = 0xD8ed,

    EN_NV_ID_LTE_TX_AMPR_NS05_B8                   = 0xD8ee,
    EN_NV_ID_LTE_TX_AMPR_NS09_B8                   = 0xD8ef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B8                       = 0xD8f0,
    EN_NV_ID_TX_RF_BIAS_B8                         = 0xD8f1,
    EN_NV_ID_TX_PA_TEMP_COMP_B8                   = 0xD8f2,
    EN_NV_ID_TX_ATTEN_TABLE_B8                        = 0xD8f3,
    EN_NV_ID_POWERDET_VOLTAGE_B8                        = 0xD8f4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B8               = 0xD8f5,
    EN_NV_ID_LTE_TX_PD_PARA_B8                      = 0xD8f6,
    EN_NV_ID_TX_ET_BAND_PARA_B8                     = 0xD8f7,

/*Band28 相关NV 项*/
	EN_NV_ID_FTM_CAND_CELL_LIST_B28                  = 0xDf40,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B28             = 0xDf41,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B28             = 0xDf42,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B28           = 0xDf43,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B28           = 0xDf44,
    EN_NV_ID_TEMP_SENSOR_TABLE_B28                   = 0xDf4f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B28         = 0xDf5b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B28         = 0xDf5c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B28                   = 0xDf5d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B28                    = 0xDf5e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B28               = 0xDf5f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B28          = 0xDf60,
    EN_NV_ID_LTE_TX_ATTEN_B28                        = 0xDf61,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B28           = 0xDf62,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B28           = 0xDf63,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B28            = 0xDf64,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B28            = 0xDf65,
    EN_NV_ID_LTE_TX_APT_PARA_B28                     = 0xDf66,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B28             = 0xDf67,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B28             = 0xDf68,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B28             = 0xDf69,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B28                          = 0xDf6a,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B28                      = 0xDf6b,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B28                         = 0xDf6c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B28         = 0xDf6d,

    EN_NV_ID_LTE_TX_AMPR_NS05_B28                   = 0xDf6e,
    EN_NV_ID_LTE_TX_AMPR_NS09_B28                   = 0xDf6f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B28                      = 0xDf70,
    EN_NV_ID_TX_RF_BIAS_B28                         = 0xDf71,
    EN_NV_ID_TX_PA_TEMP_COMP_B28                   = 0xDf72,
    EN_NV_ID_TX_ATTEN_TABLE_B28                        = 0xDf73,
    EN_NV_ID_POWERDET_VOLTAGE_B28                        = 0xDf74,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B28               = 0xDf75,
    EN_NV_ID_LTE_TX_PD_PARA_B28                     = 0xDf76,
    EN_NV_ID_TX_ET_BAND_PARA_B28                    = 0xDf77,
/* 非标频段begin */
       /* EN_NV_ID_FTM_CAND_CELL_LIST_BNon1                 = 0xDf40,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon1            = 0xDf41,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon1            = 0xDf42,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon1          = 0xDf43,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT37_BNon1         = 0xDf44,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon1                  = 0xDf4f,
    EN_NV_ID_HI6360_AGC_PARA_BNon1                    = 0xDf58,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon1        = 0xDf5b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT37_BNon1       = 0xDf5c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon1                  = 0xDf5d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon1                   = 0xDf5e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon1              = 0xDf5f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon1         = 0xDf60,
    EN_NV_ID_LTE_TX_ATTEN_BNon1                       = 0xDf61,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon1          = 0xDf62,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon1          = 0xDf63,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon1           = 0xDf64,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon1           = 0xDf65,
    EN_NV_ID_LTE_TX_APT_PARA_BNon1                    = 0xDf66,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon1            = 0xDf67,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon1             = 0xDf68,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon1             = 0xDf69,
    EN_NV_ID_LTE_TX_MPR_BNon1                         = 0xDf6a,
    EN_NV_ID_LTE_ANT_SAR_BNon1                     = 0xDf6b,
*/
  //  EN_NV_ID_LTE_TX_AMPR_BNon1                        = 0xDf6c,
  //  EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon1         = 0xDf6d,

  //  EN_NV_ID_LTE_TX_AMPR_NS05_BNon1                   = 0xDf6e,
  //  EN_NV_ID_LTE_TX_AMPR_NS09_BNon1                   = 0xDf6f,

    /*add for V9R1_6361 Begin*/

   // EN_NV_ID_TX_RF_BB_ATT_BNon1                       = 0xDf70,
   // EN_NV_ID_TX_RF_BIAS_BNon1                         = 0xDf71,
   // EN_NV_ID_TX_PA_TEMP_COMP_BNon1                   = 0xDf72,
   // EN_NV_ID_TX_ATTEN_TABLE_BNon1                        = 0xDf73,
    //EN_NV_ID_POWERDET_VOLTAGE_BNon1                        = 0xDf74,
    /*add for V9R1_6361 End*/
   // EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon1               = 0xDf75,
/*Band128 相关NV项*/
	EN_NV_ID_FTM_CAND_CELL_LIST_B128                  = 0xDf80,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_B128             = 0xDf81,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_B128             = 0xDf82,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_B128           = 0xDf83,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT2_B128           = 0xDf84,
    EN_NV_ID_TEMP_SENSOR_TABLE_B128                    = 0xDf8f,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_B128         = 0xDf9b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT2_B128         = 0xDf9c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_B128                    = 0xDf9d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_B128                     = 0xDf9e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_B128                = 0xDf9f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_B128          = 0xDfa0,
    EN_NV_ID_LTE_TX_ATTEN_B128                          = 0xDfa1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_B128           = 0xDfa2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_B128           = 0xDfa3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_B128            = 0xDfa4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_B128            = 0xDfa5,
    EN_NV_ID_LTE_TX_APT_PARA_B128                      = 0xDfa6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_B128             = 0xDfa7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_B128              = 0xDfa8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_B128              = 0xDfa9,

    /* modify by lishangfneg mpr begin */
    EN_NV_ID_LTE_TX_MPR_B128                           = 0xDfaa,
    /* modify by lishangfneg mpr end */

    /* add by wangrui tx begin */
    EN_NV_ID_LTE_ANT_SAR_B128                          = 0xDfab,
    /* add by wangrui tx end */

    EN_NV_ID_LTE_TX_AMPR_B128                         = 0xDfac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_B128         = 0xDfad,

    EN_NV_ID_LTE_TX_AMPR_NS05_B128                    = 0xDfae,
    EN_NV_ID_LTE_TX_AMPR_NS09_B128                    = 0xDfaf,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_B128                         = 0xDfb0,
    EN_NV_ID_TX_RF_BIAS_B128                           = 0xDfb1,
    EN_NV_ID_TX_PA_TEMP_COMP_B128                      = 0xDfb2,
    EN_NV_ID_TX_ATTEN_TABLE_B128                       = 0xDfb3,
    EN_NV_ID_POWERDET_VOLTAGE_B128                     = 0xDfb4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_B128                = 0xDfb5,
    EN_NV_ID_LTE_TX_PD_PARA_B128                       = 0xDfb6,
    EN_NV_ID_TX_ET_BAND_PARA_B128                      = 0xDfb7,

#if 0
    EN_NV_ID_FTM_CAND_CELL_LIST_BNon2                 = 0xDf80,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon2            = 0xDf81,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon2            = 0xDf82,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon2          = 0xDf83,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT42_BNon2         = 0xDf84,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon2                  = 0xDf8f,
    EN_NV_ID_HI6360_AGC_PARA_BNon2                    = 0xDf98,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon2        = 0xDf9b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT42_BNon2       = 0xDf9c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon2                  = 0xDf9d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon2                   = 0xDf9e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon2              = 0xDf9f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon2         = 0xDfa0,
    EN_NV_ID_LTE_TX_ATTEN_BNon2                       = 0xDfa1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon2          = 0xDfa2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon2          = 0xDfa3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon2           = 0xDfa4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon2           = 0xDfa5,
    EN_NV_ID_LTE_TX_APT_PARA_BNon2                    = 0xDfa6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon2            = 0xDfa7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon2             = 0xDfa8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon2             = 0xDfa9,
    EN_NV_ID_LTE_TX_MPR_BNon2                         = 0xDfaa,
    EN_NV_ID_LTE_ANT_SAR_BNon2                     = 0xDfab,

    EN_NV_ID_LTE_TX_AMPR_BNon2                        = 0xDfac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon2         = 0xDfad,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon2                   = 0xDfae,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon2                   = 0xDfaf,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon2                       = 0xDfb0,
    EN_NV_ID_TX_RF_BIAS_BNon2                         = 0xDfb1,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon2                   = 0xDfb2,
    EN_NV_ID_TX_ATTEN_TABLE_BNon2                        = 0xDfb3,
    EN_NV_ID_POWERDET_VOLTAGE_BNon2                        = 0xDfb4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon2               = 0xDfb5,
#endif

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon3                 = 0xDfc0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon3            = 0xDfc1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon3            = 0xDfc2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon3          = 0xDfc3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT43_BNon3         = 0xDfc4,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon3                  = 0xDfcf,
    EN_NV_ID_HI6360_AGC_PARA_BNon3                    = 0xDfd8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon3        = 0xDfdb,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT43_BNon3       = 0xDfdc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon3                  = 0xDfdd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon3                   = 0xDfde,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon3              = 0xDfdf,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon3         = 0xDfe0,
    EN_NV_ID_LTE_TX_ATTEN_BNon3                       = 0xDfe1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon3          = 0xDfe2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon3          = 0xDfe3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon3           = 0xDfe4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon3           = 0xDfe5,
    EN_NV_ID_LTE_TX_APT_PARA_BNon3                    = 0xDfe6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon3            = 0xDfe7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon3             = 0xDfe8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon3             = 0xDfe9,
    EN_NV_ID_LTE_TX_MPR_BNon3                         = 0xDfea,
    EN_NV_ID_LTE_ANT_SAR_BNon3                     = 0xDfeb,

    EN_NV_ID_LTE_TX_AMPR_BNon3                        = 0xDfec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon3         = 0xDfed,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon3                   = 0xDfee,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon3                   = 0xDfef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon3                       = 0xDff0,
    EN_NV_ID_TX_RF_BIAS_BNon3                         = 0xDff1,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon3                   = 0xDff2,
    EN_NV_ID_TX_ATTEN_TABLE_BNon3                        = 0xDff3,
    EN_NV_ID_POWERDET_VOLTAGE_BNon3                        = 0xDff4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon3               = 0xDff5,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon4                 = 0xe000,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon4            = 0xe001,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon4            = 0xe002,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon4          = 0xe003,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT36_BNon4         = 0xe004,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon4                  = 0xe00f,
    EN_NV_ID_HI6360_AGC_PARA_BNon4                    = 0xe018,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon4        = 0xe01b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT36_BNon4       = 0xe01c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon4                  = 0xe01d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon4                   = 0xe01e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon4              = 0xe01f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon4         = 0xe020,
    EN_NV_ID_LTE_TX_ATTEN_BNon4                       = 0xe021,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon4          = 0xe022,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon4          = 0xe023,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon4           = 0xe024,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon4           = 0xe025,

    EN_NV_ID_LTE_TX_APT_PARA_BNon4                    = 0xe026,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon4            = 0xe027,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon4             = 0xe028,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon4             = 0xe029,
    EN_NV_ID_LTE_TX_MPR_BNon4                         = 0xe02a,
    EN_NV_ID_LTE_ANT_SAR_BNon4                     = 0xe02b,

    EN_NV_ID_LTE_TX_AMPR_BNon4                        = 0xe02c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon4         = 0xe02d,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon4                   = 0xe02e,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon4                   = 0xe02f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon4                       = 0xe030,
    EN_NV_ID_TX_RF_BIAS_BNon4                         = 0xe031,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon4                   = 0xe032,
    EN_NV_ID_TX_ATTEN_TABLE_BNon4                        = 0xe033,
    EN_NV_ID_POWERDET_VOLTAGE_BNon4                        = 0xe034,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon4               = 0xe035,

    /* 非标频段begin */
    EN_NV_ID_FTM_CAND_CELL_LIST_BNon5                 = 0xe040,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon5            = 0xe041,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon5            = 0xe042,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon5          = 0xe043,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT37_BNon5         = 0xe044,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon5                  = 0xe04f,
    EN_NV_ID_HI6360_AGC_PARA_BNon5                    = 0xe058,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon5        = 0xe05b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT37_BNon5       = 0xe05c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon5                  = 0xe05d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon5                   = 0xe05e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon5              = 0xe05f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon5         = 0xe060,
    EN_NV_ID_LTE_TX_ATTEN_BNon5                       = 0xe061,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon5          = 0xe062,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon5          = 0xe063,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon5           = 0xe064,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon5           = 0xe065,
    EN_NV_ID_LTE_TX_APT_PARA_BNon5                    = 0xe066,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon5            = 0xe067,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon5             = 0xe068,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon5             = 0xe069,
    EN_NV_ID_LTE_TX_MPR_BNon5                         = 0xe06a,
    EN_NV_ID_LTE_ANT_SAR_BNon5                     = 0xe06b,

    EN_NV_ID_LTE_TX_AMPR_BNon5                        = 0xe06c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon5         = 0xe06d,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon5                   = 0xe06e,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon5                   = 0xe06f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon5                       = 0xe070,
    EN_NV_ID_TX_RF_BIAS_BNon5                         = 0xe071,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon5                   = 0xe072,
    EN_NV_ID_TX_ATTEN_TABLE_BNon5                        = 0xe073,
    EN_NV_ID_POWERDET_VOLTAGE_BNon5                        = 0xe074,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon5               = 0xe075,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon6                 = 0xe080,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon6            = 0xe081,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon6            = 0xe082,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon6          = 0xe083,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT42_BNon6         = 0xe084,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon6                  = 0xe08f,
    EN_NV_ID_HI6360_AGC_PARA_BNon6                    = 0xe098,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon6        = 0xe09b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT42_BNon6       = 0xe09c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon6                  = 0xe09d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon6                   = 0xe09e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon6              = 0xe09f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon6         = 0xe0a0,
    EN_NV_ID_LTE_TX_ATTEN_BNon6                       = 0xe0a1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon6          = 0xe0a2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon6          = 0xe0a3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon6           = 0xe0a4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon6           = 0xe0a5,
    EN_NV_ID_LTE_TX_APT_PARA_BNon6                    = 0xe0a6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon6            = 0xe0a7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon6             = 0xe0a8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon6             = 0xe0a9,
    EN_NV_ID_LTE_TX_MPR_BNon6                         = 0xe0aa,
    EN_NV_ID_LTE_ANT_SAR_BNon6                     = 0xe0ab,

    EN_NV_ID_LTE_TX_AMPR_BNon6                        = 0xe0ac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon6         = 0xe0ad,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon6                   = 0xe0ae,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon6                   = 0xe0af,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon6                       = 0xe0b0,
    EN_NV_ID_TX_RF_BIAS_BNon6                         = 0xe0b1,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon6                   = 0xe0b2,
    EN_NV_ID_TX_ATTEN_TABLE_BNon6                        = 0xe0b3,
    EN_NV_ID_POWERDET_VOLTAGE_BNon6                        = 0xe0b4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon6               = 0xe0b5,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon7                 = 0xe0c0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon7            = 0xe0c1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon7            = 0xe0c2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon7          = 0xe0c3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT43_BNon7         = 0xe0c4,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon7                  = 0xe0cf,
    EN_NV_ID_HI6360_AGC_PARA_BNon7                    = 0xe0d8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon7        = 0xe0db,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT43_BNon7       = 0xe0dc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon7                  = 0xe0dd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon7                   = 0xe0de,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon7              = 0xe0df,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon7         = 0xe0e0,
    EN_NV_ID_LTE_TX_ATTEN_BNon7                       = 0xe0e1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon7          = 0xe0e2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon7          = 0xe0e3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon7           = 0xe0e4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon7           = 0xe0e5,
    EN_NV_ID_LTE_TX_APT_PARA_BNon7                    = 0xe0e6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon7            = 0xe0e7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon7             = 0xe0e8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon7             = 0xe0e9,
    EN_NV_ID_LTE_TX_MPR_BNon7                         = 0xe0ea,
    EN_NV_ID_LTE_ANT_SAR_BNon7                     = 0xe0eb,

    EN_NV_ID_LTE_TX_AMPR_BNon7                        = 0xe0ec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon7         = 0xe0ed,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon7                   = 0xe0ee,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon7                   = 0xe0ef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon7                       = 0xe0f0,
    EN_NV_ID_TX_RF_BIAS_BNon7                         = 0xe0f1,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon7                   = 0xe0f2,
    EN_NV_ID_TX_ATTEN_TABLE_BNon7                        = 0xe0f3,
    EN_NV_ID_POWERDET_VOLTAGE_BNon7                        = 0xe0f4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon7               = 0xe0f5,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon8                 = 0xe100,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon8            = 0xe101,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon8            = 0xe102,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon8          = 0xe103,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT36_BNon8         = 0xe104,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon8                  = 0xe10f,
    EN_NV_ID_HI6360_AGC_PARA_BNon8                    = 0xe118,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon8        = 0xe11b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT36_BNon8       = 0xe11c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon8                  = 0xe11d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon8                   = 0xe11e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon8              = 0xe11f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon8         = 0xe120,
    EN_NV_ID_LTE_TX_ATTEN_BNon8                       = 0xe121,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon8          = 0xe122,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon8          = 0xe123,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon8           = 0xe124,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon8           = 0xe125,

    EN_NV_ID_LTE_TX_APT_PARA_BNon8                    = 0xe126,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon8            = 0xe127,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon8             = 0xe128,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon8             = 0xe129,
    EN_NV_ID_LTE_TX_MPR_BNon8                         = 0xe12a,
    EN_NV_ID_LTE_ANT_SAR_BNon8                     = 0xe12b,

    EN_NV_ID_LTE_TX_AMPR_BNon8                        = 0xe12c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon8         = 0xe12d,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon8                   = 0xe12e,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon8                   = 0xe12f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon8                       = 0xe130,
    EN_NV_ID_TX_RF_BIAS_BNon8                         = 0xe131,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon8                   = 0xe132,
    EN_NV_ID_TX_ATTEN_TABLE_BNon8                        = 0xe133,
    EN_NV_ID_POWERDET_VOLTAGE_BNon8                        = 0xe134,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon8               = 0xe135,

    /* 非标频段begin */
    EN_NV_ID_FTM_CAND_CELL_LIST_BNon9                 = 0xe140,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon9            = 0xe141,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon9            = 0xe142,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon9          = 0xe143,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT37_BNon9         = 0xe144,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon9                  = 0xe14f,
    EN_NV_ID_HI6360_AGC_PARA_BNon9                    = 0xe158,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon9        = 0xe15b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT37_BNon9       = 0xe15c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon9                  = 0xe15d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon9                   = 0xe15e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon9              = 0xe15f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon9         = 0xe160,
    EN_NV_ID_LTE_TX_ATTEN_BNon9                       = 0xe161,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon9          = 0xe162,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon9          = 0xe163,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon9           = 0xe164,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon9           = 0xe165,
    EN_NV_ID_LTE_TX_APT_PARA_BNon9                    = 0xe166,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon9            = 0xe167,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon9             = 0xe168,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon9             = 0xe169,
    EN_NV_ID_LTE_TX_MPR_BNon9                         = 0xe16a,
    EN_NV_ID_LTE_ANT_SAR_BNon9                        = 0xe16b,

    EN_NV_ID_LTE_TX_AMPR_BNon9                        = 0xe16c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon9         = 0xe16d,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon9                   = 0xe16e,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon9                   = 0xe16f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon9                       = 0xe170,
    EN_NV_ID_TX_RF_BIAS_BNon9                         = 0xe171,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon9                   = 0xe172,
    EN_NV_ID_TX_ATTEN_TABLE_BNon9                        = 0xe173,
    EN_NV_ID_POWERDET_VOLTAGE_BNon9                        = 0xe174,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon9               = 0xe175,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon10                 = 0xe180,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon10            = 0xe181,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon10            = 0xe182,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon10          = 0xe183,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT42_BNon10         = 0xe184,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon10                  = 0xe18f,
    EN_NV_ID_HI6360_AGC_PARA_BNon10                    = 0xe198,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon10        = 0xe19b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT42_BNon10       = 0xe19c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon10                  = 0xe19d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon10                   = 0xe19e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon10              = 0xe19f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon10         = 0xe1a0,
    EN_NV_ID_LTE_TX_ATTEN_BNon10                       = 0xe1a1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon10          = 0xe1a2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon10          = 0xe1a3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon10           = 0xe1a4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon10           = 0xe1a5,
    EN_NV_ID_LTE_TX_APT_PARA_BNon10                    = 0xe1a6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon10            = 0xe1a7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon10             = 0xe1a8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon10             = 0xe1a9,
    EN_NV_ID_LTE_TX_MPR_BNon10                         = 0xe1aa,
    EN_NV_ID_LTE_ANT_SAR_BNon10                     = 0xe1ab,

    EN_NV_ID_LTE_TX_AMPR_BNon10                        = 0xe1ac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon10         = 0xe1ad,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon10                   = 0xe1ae,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon10                   = 0xe1af,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon10                       = 0xe1b0,
    EN_NV_ID_TX_RF_BIAS_BNon10                         = 0xe1b1,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon10                   = 0xe1b2,
    EN_NV_ID_TX_ATTEN_TABLE_BNon10                        = 0xe1b3,
    EN_NV_ID_POWERDET_VOLTAGE_BNon10                        = 0xe1b4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon10               = 0xe1b5,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon11                 = 0xe1c0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon11            = 0xe1c1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon11            = 0xe1c2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon11          = 0xe1c3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT43_BNon11         = 0xe1c4,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon11                  = 0xe1cf,
    EN_NV_ID_HI6360_AGC_PARA_BNon11                    = 0xe1d8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon11        = 0xe1db,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT43_BNon11       = 0xe1dc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon11                  = 0xe1dd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon11                   = 0xe1de,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon11              = 0xe1df,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon11         = 0xe1e0,
    EN_NV_ID_LTE_TX_ATTEN_BNon11                       = 0xe1e1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon11          = 0xe1e2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon11          = 0xe1e3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon11           = 0xe1e4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon11           = 0xe1e5,
    EN_NV_ID_LTE_TX_APT_PARA_BNon11                    = 0xe1e6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon11            = 0xe1e7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon11             = 0xe1e8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon11             = 0xe1e9,
    EN_NV_ID_LTE_TX_MPR_BNon11                         = 0xe1ea,
    EN_NV_ID_LTE_ANT_SAR_BNon11                     = 0xe1eb,

    EN_NV_ID_LTE_TX_AMPR_BNon11                        = 0xe1ec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon11         = 0xe1ed,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon11                   = 0xe1ee,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon11                   = 0xe1ef,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon11                       = 0xe1f0,
    EN_NV_ID_TX_RF_BIAS_BNon11                         = 0xe1f1,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon11                   = 0xe1f2,
    EN_NV_ID_TX_ATTEN_TABLE_BNon11                        = 0xe1f3,
    EN_NV_ID_POWERDET_VOLTAGE_BNon11                        = 0xe1f4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon11               = 0xe1f5,
    EN_NV_ID_FTM_CAND_CELL_LIST_BNon12                 = 0xe200,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon12            = 0xe201,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon12            = 0xe202,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon12          = 0xe203,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT36_BNon12         = 0xe204,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon12                  = 0xe20f,
    EN_NV_ID_HI6360_AGC_PARA_BNon12                    = 0xe218,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon12        = 0xe21b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT36_BNon12       = 0xe21c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon12                  = 0xe21d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon12                   = 0xe21e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon12              = 0xe21f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon12         = 0xe220,
    EN_NV_ID_LTE_TX_ATTEN_BNon12                       = 0xe221,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon12          = 0xe222,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon12          = 0xe223,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon12           = 0xe224,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon12           = 0xe225,

    EN_NV_ID_LTE_TX_APT_PARA_BNon12                    = 0xe226,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon12            = 0xe227,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon12             = 0xe228,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon12             = 0xe229,
    EN_NV_ID_LTE_TX_MPR_BNon12                         = 0xe22a,
    EN_NV_ID_LTE_ANT_SAR_BNon12                     = 0xe22b,

    EN_NV_ID_LTE_TX_AMPR_BNon12                        = 0xe22c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon12         = 0xe22d,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon12                   = 0xe22e,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon12                   = 0xe22f,


    EN_NV_ID_TX_RF_BB_ATT_BNon12                       = 0xe230,
    EN_NV_ID_TX_RF_BIAS_BNon12                         = 0xe231,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon12                   = 0xe232,
    EN_NV_ID_TX_ATTEN_TABLE_BNon12                        = 0xe233,
    EN_NV_ID_POWERDET_VOLTAGE_BNon12                        = 0xe234,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon12               = 0xe235,
    /* 非标频段begin */
    EN_NV_ID_FTM_CAND_CELL_LIST_BNon13                 = 0xe240,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon13            = 0xe241,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon13            = 0xe242,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon13          = 0xe243,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT37_BNon13         = 0xe244,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon13                  = 0xe24f,
    EN_NV_ID_HI6360_AGC_PARA_BNon13                    = 0xe258,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon13        = 0xe25b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT37_BNon13       = 0xe25c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon13                  = 0xe25d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon13                   = 0xe25e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon13              = 0xe25f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon13         = 0xe260,
    EN_NV_ID_LTE_TX_ATTEN_BNon13                       = 0xe261,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon13          = 0xe262,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon13          = 0xe263,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon13           = 0xe264,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon13           = 0xe265,
    EN_NV_ID_LTE_TX_APT_PARA_BNon13                    = 0xe266,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon13            = 0xe267,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon13             = 0xe268,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon13             = 0xe269,
    EN_NV_ID_LTE_TX_MPR_BNon13                         = 0xe26a,
    EN_NV_ID_LTE_ANT_SAR_BNon13                     = 0xe26b,

    EN_NV_ID_LTE_TX_AMPR_BNon13                        = 0xe26c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon13         = 0xe26d,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon13                   = 0xe26e,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon13                   = 0xe26f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon13                       = 0xe270,
    EN_NV_ID_TX_RF_BIAS_BNon13                         = 0xe271,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon13                   = 0xe272,
    EN_NV_ID_TX_ATTEN_TABLE_BNon13                        = 0xe273,
    EN_NV_ID_POWERDET_VOLTAGE_BNon13                        = 0xe274,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon13               = 0xe275,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon14                 = 0xe280,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon14            = 0xe281,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon14            = 0xe282,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon14          = 0xe283,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT42_BNon14         = 0xe284,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon14                  = 0xe28f,
    EN_NV_ID_HI6360_AGC_PARA_BNon14                    = 0xe298,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon14        = 0xe29b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT42_BNon14       = 0xe29c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon14                  = 0xe29d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon14                   = 0xe29e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon14              = 0xe29f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon14         = 0xe2a0,
    EN_NV_ID_LTE_TX_ATTEN_BNon14                       = 0xe2a1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon14          = 0xe2a2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon14          = 0xe2a3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon14           = 0xe2a4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon14           = 0xe2a5,
    EN_NV_ID_LTE_TX_APT_PARA_BNon14                    = 0xe2a6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon14            = 0xe2a7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon14             = 0xe2a8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon14             = 0xe2a9,
    EN_NV_ID_LTE_TX_MPR_BNon14                         = 0xe2aa,
    EN_NV_ID_LTE_ANT_SAR_BNon14                     = 0xe2ab,

    EN_NV_ID_LTE_TX_AMPR_BNon14                        = 0xe2ac,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon14         = 0xe2ad,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon14                   = 0xe2ae,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon14                   = 0xe2af,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon14                       = 0xe2b0,
    EN_NV_ID_TX_RF_BIAS_BNon14                         = 0xe2b1,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon14                   = 0xe2b2,
    EN_NV_ID_TX_ATTEN_TABLE_BNon14                        = 0xe2b3,
    EN_NV_ID_POWERDET_VOLTAGE_BNon14                        = 0xe2b4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon14               = 0xe2b5,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon15                 = 0xe2c0,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon15            = 0xe2c1,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon15            = 0xe2c2,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon15          = 0xe2c3,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT43_BNon15         = 0xe2c4,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon15                  = 0xe2cf,
    EN_NV_ID_HI6360_AGC_PARA_BNon15                    = 0xe2d8,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon15        = 0xe2db,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT43_BNon15       = 0xe2dc,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon15                  = 0xe2dd,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon15                   = 0xe2de,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon15              = 0xe2df,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon15         = 0xe2e0,
    EN_NV_ID_LTE_TX_ATTEN_BNon15                       = 0xe2e1,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon15          = 0xe2e2,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon15          = 0xe2e3,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon15           = 0xe2e4,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon15           = 0xe2e5,
    EN_NV_ID_LTE_TX_APT_PARA_BNon15                    = 0xe2e6,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon15            = 0xe2e7,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon15             = 0xe2e8,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon15             = 0xe2e9,
    EN_NV_ID_LTE_TX_MPR_BNon15                         = 0xe2ea,
    EN_NV_ID_LTE_ANT_SAR_BNon15                     = 0xe2eb,

    EN_NV_ID_LTE_TX_AMPR_BNon15                        = 0xe2ec,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon15         = 0xe2ed,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon15                   = 0xe2ee,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon15                   = 0xe2ef,

    EN_NV_ID_TX_RF_BB_ATT_BNon15                       = 0xe2f0,
    EN_NV_ID_TX_RF_BIAS_BNon15                         = 0xe2f1,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon15                   = 0xe2f2,
    EN_NV_ID_TX_ATTEN_TABLE_BNon15                        = 0xe2f3,
    EN_NV_ID_POWERDET_VOLTAGE_BNon15                        = 0xe2f4,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon15               = 0xe2f5,

    EN_NV_ID_FTM_CAND_CELL_LIST_BNon16                 = 0xe300,
    EN_NV_ID_LTE_RX_CALIBRATION_FREQ_BNon16            = 0xe301,
    EN_NV_ID_LTE_TX_CALIBRATION_FREQ_BNon16            = 0xe302,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT1_BNon16          = 0xe303,
    EN_NV_ID_LTE_RX_AGC_BLK_TABLE_ANT36_BNon16         = 0xe304,
    EN_NV_ID_TEMP_SENSOR_TABLE_BNon16                  = 0xe30f,
    EN_NV_ID_HI6360_AGC_PARA_BNon16                    = 0xe318,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT1_BNon16        = 0xe31b,
    EN_NV_ID_LTE_RX_AGC_NOBLK_TABLE_ANT36_BNon16       = 0xe31c,
    EN_NV_ID_LTE_AGC_TEMP_COMP_BNon16                  = 0xe31d,
    EN_NV_ID_LTE_IP2_CAL_CHAN_BNon16                   = 0xe31e,
    EN_NV_ID_LTE_IP2_CAL_THRESHOLD_BNon16              = 0xe31f,
    EN_NV_ID_LTE_TX_APC_GAIN_THRESHHOLD_BNon16         = 0xe320,
    EN_NV_ID_LTE_TX_ATTEN_BNon16                       = 0xe321,
    EN_NV_ID_LTE_TX_APC_TEMP_COMP_STRU_BNon16          = 0xe322,
    EN_NV_ID_LTE_TX_CAL_HIGHGAIN_POWER_BNon16          = 0xe323,
    EN_NV_ID_LTE_TX_CAL_MIDGAIN_POWER_BNon16           = 0xe324,
    EN_NV_ID_LTE_TX_CAL_LOWGAIN_POWER_BNon16           = 0xe325,

    EN_NV_ID_LTE_TX_APT_PARA_BNon16                    = 0xe326,
    EN_NV_ID_LTE_TX_APT_PDM_HIGH_TBL_BNon16            = 0xe327,
    EN_NV_ID_LTE_TX_APT_PDM_MID_TBL_BNon16             = 0xe328,
    EN_NV_ID_LTE_TX_APT_PDM_LOW_TBL_BNon16             = 0xe329,
    EN_NV_ID_LTE_TX_MPR_BNon16                         = 0xe32a,
    EN_NV_ID_LTE_ANT_SAR_BNon16                     = 0xe32b,

    EN_NV_ID_LTE_TX_AMPR_BNon16                        = 0xe32c,
    EN_NV_ID_LTE_TX_DIVERSITY_LOSS_COMP_BNon16         = 0xe32d,

    EN_NV_ID_LTE_TX_AMPR_NS05_BNon16                   = 0xe32e,
    EN_NV_ID_LTE_TX_AMPR_NS09_BNon16                   = 0xe32f,

    /*add for V9R1_6361 Begin*/

    EN_NV_ID_TX_RF_BB_ATT_BNon16                       = 0xe330,
    EN_NV_ID_TX_RF_BIAS_BNon16                         = 0xe331,
    EN_NV_ID_TX_PA_TEMP_COMP_BNon16                   = 0xe332,
    EN_NV_ID_TX_ATTEN_TABLE_BNon16                        = 0xe333,
    EN_NV_ID_POWERDET_VOLTAGE_BNon16                        = 0xe334,
    /*add for V9R1_6361 End*/
    EN_NV_ID_LTE_TX_UL_ONE_RB_MPR_BNon16               = 0xe335,

    /* 非标频段end */
    EN_NV_ID_MODEM_END                              = 0xE4ff,
    /*Modem end 0xE4FF00*/

    EN_NV_ID_TCXO_DYN_CONFIG                        = 0x1401,

	#if 1
    EN_NV_ID_DSP2RF_CFG                             = 0xe400,
    EN_NV_ID_RF_ADVANCE_TIME                        = 0xe401,
    EN_NV_ID_RF_AGC_PARA_CODE                       = 0xe402,
    EN_NV_ID_RF_AGC_PARA_UPTHRGAIN                  = 0xe403,
    EN_NV_ID_RF_AGC_PARA_DOWNTHRGAIN                = 0xe404,
    EN_NV_ID_RF_AGC_PARA_TOTALGAIN                  = 0xe405,
    EN_NV_ID_RF_ACS_PARA                            = 0xe406,
    EN_NV_ID_AGC_CAL_PARA                           = 0xe407,
    EN_NV_ID_RF_APC_PARA                            = 0xe408,
    EN_NV_ID_MAXPOWER                               = 0xE4e0,
    EN_NV_ID_TX_APCOFF_REDUCE                       = 0xE4e1,
    EN_NV_ID_TX_CAL_FREQ_LIST_0                     = 0xE4e3,
    EN_NV_ID_TX_CAL_FREQ_LIST_1                     = 0xE4e9,
    EN_NV_ID_TX_CAL_FREQ_LIST_2                     = 0xE4ef,
    EN_NV_ID_RX_CAL_FREQ_LIST_0                     = 0xE4e4,
    EN_NV_ID_RX_CAL_FREQ_LIST_1                     = 0xE4ea,
    EN_NV_ID_RX_CAL_FREQ_LIST_2                     = 0xE4f0,
    EN_NV_ID_TEMP_SEMSOR_0                          = 0xE4e5,
    EN_NV_ID_TEMP_SEMSOR_1                          = 0xE4eb,
    EN_NV_ID_TEMP_SEMSOR_2                          = 0xE4f1,
    EN_NV_ID_TX_TEMP_COMP_0                         = 0xE4e6,
    EN_NV_ID_TX_TEMP_COMP_1                         = 0xE4ec,
    EN_NV_ID_TX_TEMP_COMP_2                         = 0xE4f2,
    EN_NV_ID_RX_TEMP_COMP_0                         = 0xE4e7,
    EN_NV_ID_RX_TEMP_COMP_1                         = 0xE4ed,
    EN_NV_ID_RX_TEMP_COMP_2                         = 0xE4f3,
    EN_NV_ID_PA_POWER_0                             = 0xF8f0,
    EN_NV_ID_PA_POWER_1                             = 0xF8f5,
    EN_NV_ID_PA_POWER_2                             = 0xF8fa,
    EN_NV_ID_TX_APC_COMP_0                          = 0xF8f1,
    EN_NV_ID_TX_APC_COMP_1                          = 0xF8f6,
    EN_NV_ID_TX_APC_COMP_2                          = 0xF8fb,
    EN_NV_ID_TX_APC_FREQ_COMP_0                     = 0xF8f2,
    EN_NV_ID_TX_APC_FREQ_COMP_1                     = 0xF8f7,
    EN_NV_ID_TX_APC_FREQ_COMP_2                     = 0xF8fc,
    EN_NV_ID_RX_AGC_COMP_0                          = 0xF8f3,
    EN_NV_ID_RX_AGC_COMP_1                          = 0xF8f8,
    EN_NV_ID_RX_AGC_COMP_2                          = 0xF8fd,
    EN_NV_ID_RX_AGC_FREQ_COMP_0                     = 0xF8f4,
    EN_NV_ID_RX_AGC_FREQ_COMP_1                     = 0xF8f9,
    EN_NV_ID_RX_AGC_FREQ_COMP_2                     = 0xF8fe,
    EN_NV_ID_TX_PA_LEVEL_THRE                     = 0xE409,
    EN_NV_ID_US_TCXO_INIT						  = 0xE900,
    EN_NV_ID_DRX_PARA						      = 0xE40a,
    #endif

	#if 0
	/*公共NV，预留*/
	/*modem nv*/
	EN_NV_ID_TDS_TEMPERATURE_SENSOR 				= 0xe490,
	EN_NV_ID_TDS_TX_CAL_FREQ 						= 0xe491,
	EN_NV_ID_TDS_TX_TEMP_COMP 						= 0xe492,
	EN_NV_ID_TDS_TX_RF_BIAS 						= 0xe493,
	EN_NV_ID_TDS_TX_RF_BB_ATT 						= 0xe494,
	EN_NV_ID_TDS_PA_LEVEL_THRE 						= 0xe495,
	EN_NV_ID_TDS_RX_CAL_FREQ 						= 0xe496,
	EN_NV_ID_TDS_AGC_BAND_PARA 						= 0xe497,
	EN_NV_ID_TDS_AGC_TEMP_COMP 						= 0xe498,
	/*工厂nv*/
	EN_NV_ID_TDS_TX_FREQ_COMP 						= 0xf890,
	EN_NV_ID_TDS_PA_POWER_DIFFERENCE 				= 0xf891,
	EN_NV_ID_TDS_TX_APC_GAIN 						= 0xf892,
	EN_NV_ID_TDS_AGC_FREQ_COMP 						= 0xf893,
	EN_NV_ID_TDS_DCOC_CAL 							= 0xf894,
	EN_NV_ID_TDS_RF_TXIQ_CAL 						= 0xf895,
	#endif

	EN_NV_ID_TDS_RFIC_CFG_STRU_DEFAULT				= 0xe499,
	EN_NV_ID_TDS_LINECTRL_ALLOT_BY_HARDWARE_STRU_DEFAULT = 0xe49a,
	EN_NV_ID_TDS_RF_ADVANCE_TIME_STU_DEFAULT		= 0xe49b,
    EN_NV_ID_TDS_MIPI_CTRL_DEFAULT              = 0xe49c,

    /*l00253980+tas 20140915 begin*/
    EN_NV_ID_TDS_TAS_STRU_DEFAULT               = 0xe49d,
    EN_NV_ID_TDS_TAS_RF_STRU_DEFAULT               = 0xe49e,
    /*l00253980+tas 20140915 end*/

	/*Band34*/
	/*modem nv*/
	EN_NV_ID_TDS_TEMPERATURE_SENSOR_B34				= 0xe4a0,
	EN_NV_ID_TDS_TX_CAL_FREQ_B34 					= 0xe4a1,
	EN_NV_ID_TDS_TX_TEMP_COMP_B34 					= 0xe4a2,
	EN_NV_ID_TDS_TX_RF_BIAS_B34 					= 0xe4a3,
	EN_NV_ID_TDS_TX_RF_BB_ATT_B34 					= 0xe4a4,
	EN_NV_ID_TDS_PA_LEVEL_THRE_B34					= 0xe4a5,
	EN_NV_ID_TDS_RX_CAL_FREQ_B34 					= 0xe4a6,
	EN_NV_ID_TDS_AGC_BAND_PARA_B34 					= 0xe4a7,
	EN_NV_ID_TDS_AGC_TEMP_COMP_B34 					= 0xe4a8,
	EN_NV_ID_TDS_TX_RF_BB_MAX_ATT_B34 				= 0xe4a9,
	EN_NV_ID_TDS_TX_CAL_BB_ATT_B34					= 0xe4aa,
	EN_NV_ID_TDS_TX_PA_CAL_FREQ_B34					= 0xe4ab,
	/*工厂nv*/
	EN_NV_ID_TDS_TX_FREQ_COMP_B34 					= 0xf8a0,
	EN_NV_ID_TDS_PA_POWER_DIFFERENCE_B34 			= 0xf8a1,
	EN_NV_ID_TDS_TX_APC_GAIN_ITEM_B34 				= 0xf8a2,
	EN_NV_ID_TDS_AGC_FREQ_COMP_B34 					= 0xf8a3,
	EN_NV_ID_TDS_DCOC_CAL_B34 						= 0xf8a4,
	EN_NV_ID_TDS_RF_TXIQ_CAL_B34 					= 0xf8a5,
	EN_NV_ID_TDS_TX_APC_GAIN_B34 					= 0xf8a6,
	EN_NV_ID_TDS_TX_CAL_PA_GAIN_BB_B34				= 0xf8a7,
	EN_NV_ID_TDS_PA_MID_TX_FREQ_COMP_B34				= 0xf8a8,
	EN_NV_ID_TDS_PA_LOW_TX_FREQ_COMP_B34				= 0xf8a9,
	/*Band39*/
	/*modem nv*/
	EN_NV_ID_TDS_TEMPERATURE_SENSOR_B39				= 0xe4b0,
	EN_NV_ID_TDS_TX_CAL_FREQ_B39 					= 0xe4b1,
	EN_NV_ID_TDS_TX_TEMP_COMP_B39 					= 0xe4b2,
	EN_NV_ID_TDS_TX_RF_BIAS_B39 					= 0xe4b3,
	EN_NV_ID_TDS_TX_RF_BB_ATT_B39 					= 0xe4b4,
	EN_NV_ID_TDS_PA_LEVEL_THRE_B39					= 0xe4b5,
	EN_NV_ID_TDS_RX_CAL_FREQ_B39 					= 0xe4b6,
	EN_NV_ID_TDS_AGC_BAND_PARA_B39 					= 0xe4b7,
	EN_NV_ID_TDS_AGC_TEMP_COMP_B39 					= 0xe4b8,
	EN_NV_ID_TDS_TX_RF_BB_MAX_ATT_B39 				= 0xe4b9,
	EN_NV_ID_TDS_TX_CAL_BB_ATT_B39					= 0xe4ba,
	EN_NV_ID_TDS_TX_PA_CAL_FREQ_B39					= 0xe4bb,
	/*工厂nv*/
	EN_NV_ID_TDS_TX_FREQ_COMP_B39 					= 0xf8b0,
	EN_NV_ID_TDS_PA_POWER_DIFFERENCE_B39 			= 0xf8b1,
	EN_NV_ID_TDS_TX_APC_GAIN_ITEM_B39 				= 0xf8b2,
	EN_NV_ID_TDS_AGC_FREQ_COMP_B39 					= 0xf8b3,
	EN_NV_ID_TDS_DCOC_CAL_B39 						= 0xf8b4,
	EN_NV_ID_TDS_RF_TXIQ_CAL_B39 					= 0xf8b5,
	EN_NV_ID_TDS_TX_APC_GAIN_B39 					= 0xf8b6,
	EN_NV_ID_TDS_TX_CAL_PA_GAIN_BB_B39				= 0xf8b7,
	EN_NV_ID_TDS_PA_MID_TX_FREQ_COMP_B39				= 0xf8b8,
	EN_NV_ID_TDS_PA_LOW_TX_FREQ_COMP_B39				= 0xf8b9,

	/*Band40*/
	/*modem nv*/
	EN_NV_ID_TDS_TEMPERATURE_SENSOR_B40				= 0xe4c0,
	EN_NV_ID_TDS_TX_CAL_FREQ_B40 					= 0xe4c1,
	EN_NV_ID_TDS_TX_TEMP_COMP_B40 					= 0xe4c2,
	EN_NV_ID_TDS_TX_RF_BIAS_B40 					= 0xe4c3,
	EN_NV_ID_TDS_TX_RF_BB_ATT_B40 					= 0xe4c4,
	EN_NV_ID_TDS_PA_LEVEL_THRE_B40					= 0xe4c5,
	EN_NV_ID_TDS_RX_CAL_FREQ_B40 					= 0xe4c6,
	EN_NV_ID_TDS_AGC_BAND_PARA_B40 					= 0xe4c7,
	EN_NV_ID_TDS_AGC_TEMP_COMP_B40 					= 0xe4c8,
	EN_NV_ID_TDS_TX_RF_BB_MAX_ATT_B40 				= 0xe4c9,
	EN_NV_ID_TDS_TX_CAL_BB_ATT_B40					= 0xe4ca,
	EN_NV_ID_TDS_TX_PA_CAL_FREQ_B40					= 0xe4cb,
	/*工厂nv*/
	EN_NV_ID_TDS_TX_FREQ_COMP_B40 					= 0xf8c0,
	EN_NV_ID_TDS_PA_POWER_DIFFERENCE_B40 			= 0xf8c1,
	EN_NV_ID_TDS_TX_APC_GAIN_ITEM_B40 				= 0xf8c2,
	EN_NV_ID_TDS_AGC_FREQ_COMP_B40 					= 0xf8c3,
	EN_NV_ID_TDS_DCOC_CAL_B40 						= 0xf8c4,
	EN_NV_ID_TDS_RF_TXIQ_CAL_B40 					= 0xf8c5,
	EN_NV_ID_TDS_TX_APC_GAIN_B40 					= 0xf8c6,
	EN_NV_ID_TDS_TX_CAL_PA_GAIN_BB_B40				= 0xf8c7,
	EN_NV_ID_TDS_PA_MID_TX_FREQ_COMP_B40				= 0xf8c8,
	EN_NV_ID_TDS_PA_LOW_TX_FREQ_COMP_B40				= 0xf8c9,

	/*Band_Reserved*/
	/*modem nv*/
	EN_NV_ID_TDS_TEMPERATURE_SENSOR_BRESERVED		= 0xe4d0,
	EN_NV_ID_TDS_TX_CAL_FREQ_BRESERVED 				= 0xe4d1,
	EN_NV_ID_TDS_TX_TEMP_COMP_BRESERVED 			= 0xe4d2,
	EN_NV_ID_TDS_TX_RF_BIAS_BRESERVED 				= 0xe4d3,
	EN_NV_ID_TDS_TX_RF_BB_ATT_BRESERVED 			= 0xe4d4,
	EN_NV_ID_TDS_PA_LEVEL_THRE_BRESERVED			= 0xe4d5,
	EN_NV_ID_TDS_RX_CAL_FREQ_BRESERVED 				= 0xe4d6,
	EN_NV_ID_TDS_AGC_BAND_PARA_BRESERVED 			= 0xe4d7,
	EN_NV_ID_TDS_AGC_TEMP_COMP_BRESERVED 			= 0xe4d8,
	EN_NV_ID_TDS_TX_RF_BB_MAX_ATT_BRESERVED 		= 0xe4d9,
	EN_NV_ID_TDS_TX_CAL_BB_ATT_BRESERVED			= 0xe4da,
	EN_NV_ID_TDS_TX_PA_CAL_FREQ_BRESERVED			= 0xe4db,
	/*工厂nv*/
	EN_NV_ID_TDS_TX_FREQ_COMP_BRESERVED 			= 0xf8d0,
	EN_NV_ID_TDS_PA_POWER_DIFFERENCE_BRESERVED 		= 0xf8d1,
	EN_NV_ID_TDS_TX_APC_GAIN_ITEM_BRESERVED 		= 0xf8d2,
	EN_NV_ID_TDS_AGC_FREQ_COMP_BRESERVED 			= 0xf8d3,
	EN_NV_ID_TDS_DCOC_CAL_BRESERVED 				= 0xf8d4,
	EN_NV_ID_TDS_RF_TXIQ_CAL_BRESERVED 				= 0xf8d5,
	EN_NV_ID_TDS_TX_APC_GAIN_BRESERVED 				= 0xf8d6,
	EN_NV_ID_TDS_TX_CAL_PA_GAIN_BB_BRESERVED		= 0xf8d7,
	EN_NV_ID_TDS_PA_MID_TX_FREQ_COMP_BRESERVED		= 0xf8a8,
	EN_NV_ID_TDS_PA_LOW_TX_FREQ_COMP_BRESERVED		= 0xf8a9,


    EN_NV_ID_LTE_TCXO_INIT_FREQ                     = 0xe900,
    EN_NV_ID_TDS_NV_TUNER_PARA                      = 0xe901,
    EN_NV_ID_TDS_DRX_PARA						    = 0xe902,
    EN_NV_ID_TDS_VRAMP_PARA                         = 0xe903,

    EN_NV_ID_ANT_MODEM_LOSS_B20                     = 0xeb00,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B20         = 0xeb0d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B20         = 0xeb0e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B20       = 0xeb0f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B20       = 0xeb10,
    EN_NV_ID_LTE_IP2_CAL_B20                        = 0xeb11,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B20      = 0xeb12,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B20       = 0xeb13,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B20       = 0xeb14,
    EN_NV_ID_LTE_PA_POWER_B20                       = 0xeb15,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B20        = 0xeb16,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B20         = 0xeb17,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B20         = 0xeb18,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B20                    = 0xeb19,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B20                  = 0xeb1a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B20                  = 0xeb1b,
    EN_NV_ID_TX_APC_GAIN_B20                       = 0xeb1c,
    EN_NV_ID_RF_TXIQ_CAL_B20                       = 0xeb1d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B20               = 0xeb1e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B20              = 0xeb1f,



	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B20				  = 0xeb25,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B20 			  = 0xeb26,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B20			  = 0xeb27,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B20 		  = 0xeb28,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B20	  = 0xeb29,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B20	  = 0xeb2a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B20  = 0xeb2b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B20  = 0xeb2c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B20                  = 0xeb2d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B20                  = 0xeb2e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B20                 = 0xeb2f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B20                   = 0xeb30,
    EN_NV_ID_ET_FINEDLY_PARA_B20                    = 0xeb31,
    EN_NV_ID_ET_VOFFSET_GAIN_B20                    = 0xeb32,
    EN_NV_ID_ET_EVDELY_B20                           = 0xeb33,
    EN_NV_ID_RF_CA_RCCODE_B20                        = 0xeb34,


    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B40                     = 0xeb40,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B40         = 0xeb4D,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B40         = 0xeb4E,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B40       = 0xeb4F,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B40       = 0xeb50,
    EN_NV_ID_LTE_IP2_CAL_B40                        = 0xeb51,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B40      = 0xeb52,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B40       = 0xeb53,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B40       = 0xeb54,
    EN_NV_ID_LTE_PA_POWER_B40                       = 0xeb55,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B40        = 0xeb56,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B40         = 0xeb57,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B40         = 0xeb58,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B40                    = 0xeb59,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B40                  = 0xeb5a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B40                  = 0xeb5b,
    EN_NV_ID_TX_APC_GAIN_B40                       = 0xeb5c,
    EN_NV_ID_RF_TXIQ_CAL_B40                       = 0xeb5d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B40               = 0xeb5e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B40              = 0xeb5f,
    /*add for V9R1_6361 End*/

	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B40				  = 0xeb65,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B40 			  = 0xeb66,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B40			  = 0xeb67,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B40 		  = 0xeb68,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B40	  = 0xeb69,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B40	  = 0xeb6a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B40  = 0xeb6b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B40  = 0xeb6c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B40                  = 0xeb6d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B40                  = 0xeb6e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B40                 = 0xeb6f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B40                   = 0xeb70,
    EN_NV_ID_ET_FINEDLY_PARA_B40                    = 0xeb71,
    EN_NV_ID_ET_VOFFSET_GAIN_B40                    = 0xeb72,
    EN_NV_ID_ET_EVDELY_B40                           = 0xeb73,
    EN_NV_ID_RF_CA_RCCODE_B40                        = 0xeb74,

    EN_NV_ID_ANT_MODEM_LOSS_B38                     = 0xeb80,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B38         = 0xeb8d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B38         = 0xeb8e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B38       = 0xeb8f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B38       = 0xeb90,
    EN_NV_ID_LTE_IP2_CAL_B38                        = 0xeb91,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B38      = 0xeb92,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B38       = 0xeb93,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B38       = 0xeb94,
    EN_NV_ID_LTE_PA_POWER_B38                       = 0xeb95,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B38        = 0xeb96,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B38         = 0xeb97,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B38         = 0xeb98,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B38                    = 0xeb99,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B38                  = 0xeb9a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B38                  = 0xeb9b,
    EN_NV_ID_TX_APC_GAIN_B38                       = 0xeb9c,
    EN_NV_ID_RF_TXIQ_CAL_B38                       = 0xeb9d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B38               = 0xeb9e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B38              = 0xeb9f,
    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B38                = 0xeba5,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B38               = 0xeba6,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B38              = 0xeba7,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B38           = 0xeba8,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B38         = 0xeba9,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B38         = 0xebaa,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B38       = 0xebab,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B38       = 0xebac,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B38                  = 0xebad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B38                  = 0xebae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B38                 = 0xebaf,
    EN_NV_ID_LTE_ET_GAIN_COMP_B38                   = 0xebb0,
    EN_NV_ID_ET_FINEDLY_PARA_B38                    = 0xebb1,
    EN_NV_ID_ET_VOFFSET_GAIN_B38                    = 0xebb2,
    EN_NV_ID_ET_EVDELY_B38                           = 0xebb3,
    EN_NV_ID_RF_CA_RCCODE_B38                        = 0xebb4,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B41                     = 0xebc0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B41         = 0xebcd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B41         = 0xebce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B41       = 0xebcf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B41       = 0xebd0,
    EN_NV_ID_LTE_IP2_CAL_B41                        = 0xebd1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B41      = 0xebd2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B41       = 0xebd3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B41       = 0xebd4,
    EN_NV_ID_LTE_PA_POWER_B41                       = 0xebd5,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B41        = 0xebd6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B41         = 0xebd7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B41         = 0xebd8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B41                    = 0xebd9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B41                  = 0xebda,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B41                  = 0xebdb,
    EN_NV_ID_TX_APC_GAIN_B41                       = 0xebdc,
    EN_NV_ID_RF_TXIQ_CAL_B41                       = 0xebdd,
    EN_NV_ID_PA_POWER_DIFFERENCE_B41               = 0xebde,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B41              = 0xebdf,
    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B41                = 0xebe5,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B41               = 0xebe6,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B41              = 0xebe7,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B41           = 0xebe8,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B41    = 0xebe9,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B41    = 0xebea,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B41  = 0xebeb,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B41  = 0xebec,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B41                  = 0xebed,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B41                  = 0xebee,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B41                 = 0xebef,
    EN_NV_ID_LTE_ET_GAIN_COMP_B41                   = 0xebf0,
    EN_NV_ID_ET_FINEDLY_PARA_B41                    = 0xebf1,
    EN_NV_ID_ET_VOFFSET_GAIN_B41                    = 0xebf2,
    EN_NV_ID_ET_EVDELY_B41                           = 0xebf3,
    EN_NV_ID_RF_CA_RCCODE_B41                        = 0xebf4,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B7                      = 0xec00,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B7          = 0xec0D,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B7          = 0xec0E,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B7        = 0xec0F,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B7        = 0xec10,
    EN_NV_ID_LTE_IP2_CAL_B7                         = 0xec11,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B7       = 0xec12,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B7        = 0xec13,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B7        = 0xec14,
    EN_NV_ID_LTE_PA_POWER_B7                        = 0xec15,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B7         = 0xec16,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B7          = 0xec17,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B7          = 0xec18,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B7                    = 0xec19,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B7                  = 0xec1a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B7                  = 0xec1b,
    EN_NV_ID_TX_APC_GAIN_B7                       = 0xec1c,
    EN_NV_ID_RF_TXIQ_CAL_B7                       = 0xec1d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B7               = 0xec1e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B7              = 0xec1f,
    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B7                = 0xec25,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B7               = 0xec26,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B7              = 0xec27,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B7           = 0xec28,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B7    = 0xec29,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B7    = 0xec2a,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B7  = 0xec2b,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B7  = 0xec2c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B7                  = 0xec2d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B7                  = 0xec2e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B7                 = 0xec2f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B7                   = 0xec30,
    EN_NV_ID_ET_FINEDLY_PARA_B7                    = 0xec31,
    EN_NV_ID_ET_VOFFSET_GAIN_B7                    = 0xec32,
    EN_NV_ID_ET_EVDELY_B7                           = 0xec33,
    EN_NV_ID_RF_CA_RCCODE_B7                        = 0xec34,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B3                      = 0xed00,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B3          = 0xed0D,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B3          = 0xed0E,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B3        = 0xed0F,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B3        = 0xed10,
    EN_NV_ID_LTE_IP2_CAL_B3                         = 0xed11,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B3       = 0xed12,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B3        = 0xed13,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B3        = 0xed14,
    EN_NV_ID_LTE_PA_POWER_B3                        = 0xed15,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B3         = 0xed16,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B3          = 0xed17,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B3          = 0xed18,
     /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B3                    = 0xed19,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B3                  = 0xed1a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B3                  = 0xed1b,
    EN_NV_ID_TX_APC_GAIN_B3                       = 0xed1c,
    EN_NV_ID_RF_TXIQ_CAL_B3                       = 0xed1d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B3               = 0xed1e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B3              = 0xed1f,
    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B3                = 0xed25,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B3               = 0xed26,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B3              = 0xed27,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B3           = 0xed28,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B3    = 0xed29,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B3    = 0xed2a,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B3  = 0xed2b,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B3  = 0xed2c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B3                  = 0xed2d,
	EN_NV_ID_ET_LUT_TBL_MIDCH_B3                  = 0xed2e,
	EN_NV_ID_ET_LUT_TBL_HIGHCH_B3                 = 0xed2f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B3                    = 0xed30,
    EN_NV_ID_ET_FINEDLY_PARA_B3                     = 0xed31,
    EN_NV_ID_ET_VOFFSET_GAIN_B3                     = 0xed32,
    EN_NV_ID_ET_EVDELY_B3                           = 0xed33,
    EN_NV_ID_RF_CA_RCCODE_B3                        = 0xed34,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B1                      = 0xed40,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B1          = 0xed4D,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B1          = 0xed4E,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B1        = 0xed4F,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B1        = 0xed50,
    EN_NV_ID_LTE_IP2_CAL_B1                         = 0xed51,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B1       = 0xed52,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B1        = 0xed53,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B1        = 0xed54,
    EN_NV_ID_LTE_PA_POWER_B1                        = 0xed55,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B1         = 0xed56,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B1          = 0xed57,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B1          = 0xed58,
     /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B1                    = 0xed59,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B1                  = 0xed5a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B1                  = 0xed5b,
    EN_NV_ID_TX_APC_GAIN_B1                       = 0xed5c,
    EN_NV_ID_RF_TXIQ_CAL_B1                       = 0xed5d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B1               = 0xed5e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B1              = 0xed5f,
    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B1                = 0xed65,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B1               = 0xed66,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B1              = 0xed67,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B1           = 0xed68,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B1    = 0xed69,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B1    = 0xed6a,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B1  = 0xed6b,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B1  = 0xed6c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B1                  = 0xed6d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B1                  = 0xed6e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B1                 = 0xed6f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B1                   = 0xed70,
    EN_NV_ID_ET_FINEDLY_PARA_B1                    = 0xed71,
    EN_NV_ID_ET_VOFFSET_GAIN_B1                    = 0xed72,
    EN_NV_ID_ET_EVDELY_B1                           = 0xed73,
    EN_NV_ID_RF_CA_RCCODE_B1                        = 0xed74,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B5                      = 0xed80,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B5          = 0xed8D,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B5          = 0xed8E,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B5        = 0xed8F,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B5        = 0xed90,
    EN_NV_ID_LTE_IP2_CAL_B5                         = 0xed91,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B5       = 0xed92,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B5        = 0xed93,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B5        = 0xed94,
    EN_NV_ID_LTE_PA_POWER_B5                        = 0xed95,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B5         = 0xed96,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B5          = 0xed97,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B5          = 0xed98,
     /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B5                    = 0xed99,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B5                  = 0xed9a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B5                  = 0xed9b,
    EN_NV_ID_TX_APC_GAIN_B5                       = 0xed9c,
    EN_NV_ID_RF_TXIQ_CAL_B5                       = 0xed9d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B5               = 0xed9e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B5              = 0xed9f,
    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B5                = 0xedA5,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B5               = 0xedA6,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B5              = 0xedA7,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B5           = 0xedA8,


	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B5	  = 0xedA9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B5	  = 0xedAa,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B5  = 0xedAb,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B5  = 0xedAc,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B5                  = 0xedad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B5                  = 0xedae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B5                 = 0xedaf,
    EN_NV_ID_LTE_ET_GAIN_COMP_B5                   = 0xedb0,
    EN_NV_ID_ET_FINEDLY_PARA_B5                    = 0xedb1,
    EN_NV_ID_ET_VOFFSET_GAIN_B5                    = 0xedb2,
    EN_NV_ID_ET_EVDELY_B5                           = 0xedb3,
    EN_NV_ID_RF_CA_RCCODE_B5                        = 0xedb4,

    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B8                      = 0xedc0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B8          = 0xedcD,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B8          = 0xedcE,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B8        = 0xedcF,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B8        = 0xedd0,
    EN_NV_ID_LTE_IP2_CAL_B8                         = 0xedd1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B8       = 0xedd2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B8        = 0xedd3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B8        = 0xedd4,
    EN_NV_ID_LTE_PA_POWER_B8                        = 0xedd5,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B8         = 0xedd6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B8          = 0xedd7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B8          = 0xedd8,
     /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B8                    = 0xedd9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B8                  = 0xedda,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B8                  = 0xeddb,
    EN_NV_ID_TX_APC_GAIN_B8                       = 0xeddc,
    EN_NV_ID_RF_TXIQ_CAL_B8                       = 0xeddd,
    EN_NV_ID_PA_POWER_DIFFERENCE_B8               = 0xedde,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B8              = 0xeddf,
	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B8                = 0xede5,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B8               = 0xede6,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B8              = 0xede7,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B8           = 0xede8,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B8    = 0xede9,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B8    = 0xedea,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B8  = 0xedeb,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B8  = 0xedec,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B8                  = 0xeded,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B8                  = 0xedee,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B8                 = 0xedef,
    EN_NV_ID_LTE_ET_GAIN_COMP_B8                   = 0xedf0,
    EN_NV_ID_ET_FINEDLY_PARA_B8                    = 0xedf1,
    EN_NV_ID_ET_VOFFSET_GAIN_B8                    = 0xedf2,
    EN_NV_ID_ET_EVDELY_B8                           = 0xedf3,
    EN_NV_ID_RF_CA_RCCODE_B8                        = 0xedf4,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B19                     = 0xEE00,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B19         = 0xEE0d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B19         = 0xEE0e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B19       = 0xEE0f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B19       = 0xEE10,
    EN_NV_ID_LTE_IP2_CAL_B19                        = 0xEE11,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B19      = 0xEE12,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B19       = 0xEE13,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B19       = 0xEE14,
    EN_NV_ID_LTE_PA_POWER_B19                       = 0xEE15,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B19        = 0xEE16,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B19         = 0xEE17,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B19         = 0xEE18,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B19                     = 0xEE19,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B19                   = 0xEE1a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B19                   = 0xEE1b,
    EN_NV_ID_TX_APC_GAIN_B19                        = 0xEE1c,
    EN_NV_ID_RF_TXIQ_CAL_B19                        = 0xEE1d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B19                = 0xEE1e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B19               = 0xEE1f,
    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B19                  = 0xee25,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B19                 = 0xee26,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B19                = 0xee27,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B19             = 0xee28,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B19      = 0xee29,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B19      = 0xee2a,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B19    = 0xee2b,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B19    = 0xee2c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B19                  = 0xee2d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B19                  = 0xee2e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B19                 = 0xee2f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B19                   = 0xee30,
    EN_NV_ID_ET_FINEDLY_PARA_B19                    = 0xee31,
    EN_NV_ID_ET_VOFFSET_GAIN_B19                    = 0xee32,
    EN_NV_ID_ET_EVDELY_B19                           = 0xee33,
    EN_NV_ID_RF_CA_RCCODE_B19                        = 0xee34,

    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B21                     = 0xEE40,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B21         = 0xEE4d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B21         = 0xEE4e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B21       = 0xEE4f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B21       = 0xEE50,
    EN_NV_ID_LTE_IP2_CAL_B21                        = 0xEE51,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B21      = 0xEE52,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B21       = 0xEE53,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B21       = 0xEE54,
    EN_NV_ID_LTE_PA_POWER_B21                       = 0xEE55,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B21        = 0xEE56,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B21         = 0xEE57,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B21         = 0xEE58,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B21                     = 0xEE59,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B21                   = 0xEE5a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B21                   = 0xEE5b,
    EN_NV_ID_TX_APC_GAIN_B21                        = 0xEE5c,
    EN_NV_ID_RF_TXIQ_CAL_B21                        = 0xEE5d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B21                = 0xEE5e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B21               = 0xEE5f,
    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B21                  = 0xee65,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B21                 = 0xee66,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B21                = 0xee67,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B21             = 0xee68,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B21      = 0xee69,
    EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B21      = 0xee6a,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B21    = 0xee6b,
    EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B21    = 0xee6c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B21                  = 0xee6d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B21                  = 0xee6e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B21                 = 0xee6f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B21                   = 0xee70,
    EN_NV_ID_ET_FINEDLY_PARA_B21                    = 0xee71,
    EN_NV_ID_ET_VOFFSET_GAIN_B21                    = 0xee72,
    EN_NV_ID_ET_EVDELY_B21                           = 0xee73,
    EN_NV_ID_RF_CA_RCCODE_B21                        = 0xee74,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_B2                      = 0xEE80,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B2          = 0xEE8d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B2          = 0xEE8e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B2        = 0xEE8f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B2        = 0xEE90,
    EN_NV_ID_LTE_IP2_CAL_B2                         = 0xEE91,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B2       = 0xEE92,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B2        = 0xEE93,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B2        = 0xEE94,
    EN_NV_ID_LTE_PA_POWER_B2                        = 0xEE95,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B2         = 0xEE96,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B2          = 0xEE97,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B2          = 0xEE98,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B2                     = 0xEE99,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B2                   = 0xEE9a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B2                   = 0xEE9b,
    EN_NV_ID_TX_APC_GAIN_B2                        = 0xEE9c,
    EN_NV_ID_RF_TXIQ_CAL_B2                        = 0xEE9d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B2                = 0xEE9e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B2               = 0xEE9f,
    /*add for V9R1_6361 End*/



    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B2                = 0xEEa5,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B2               = 0xEEa6,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B2              = 0xEEa7,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B2           = 0xEEa8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B2	  = 0xEEa9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B2	  = 0xEEaa,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B2  = 0xEEab,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B2  = 0xEEac,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B2                  = 0xEEad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B2                  = 0xEEae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B2                 = 0xEEaf,
    EN_NV_ID_LTE_ET_GAIN_COMP_B2                   = 0xEEb0,
    EN_NV_ID_ET_FINEDLY_PARA_B2                    = 0xEEb1,
    EN_NV_ID_ET_VOFFSET_GAIN_B2                    = 0xEEb2,
    EN_NV_ID_ET_EVDELY_B2                           = 0xEEb3,
    EN_NV_ID_RF_CA_RCCODE_B2                        = 0xEEb4,

    EN_NV_ID_ANT_MODEM_LOSS_B4                      = 0xEEc0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B4          = 0xEEcd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B4          = 0xEEce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B4        = 0xEEcf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B4        = 0xEEd0,
    EN_NV_ID_LTE_IP2_CAL_B4                         = 0xEEd1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B4       = 0xEEd2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B4        = 0xEEd3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B4        = 0xEEd4,
    EN_NV_ID_LTE_PA_POWER_B4                        = 0xEEd5,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B4         = 0xEEd6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B4          = 0xEEd7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B4          = 0xEEd8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B4                     = 0xEEd9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B4                   = 0xEEda,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B4                   = 0xEEdb,
    EN_NV_ID_TX_APC_GAIN_B4                        = 0xEEdc,
    EN_NV_ID_RF_TXIQ_CAL_B4                        = 0xEEdd,
    EN_NV_ID_PA_POWER_DIFFERENCE_B4                = 0xEEde,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B4               = 0xEEdf,
    /*add for V9R1_6361 End*/

    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B4                = 0xEEe5,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B4               = 0xEEe6,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B4              = 0xEEe7,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B4           = 0xEEe8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B4	  = 0xEEe9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B4	  = 0xEEea,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B4  = 0xEEeb,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B4  = 0xEEec,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B4                  = 0xEEed,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B4                  = 0xEEee,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B4                 = 0xEEef,
    EN_NV_ID_LTE_ET_GAIN_COMP_B4                   = 0xEEf0,
    EN_NV_ID_ET_FINEDLY_PARA_B4                    = 0xEEf1,
    EN_NV_ID_ET_VOFFSET_GAIN_B4                    = 0xEEf2,
    EN_NV_ID_ET_EVDELY_B4                           = 0xEEf3,
    EN_NV_ID_RF_CA_RCCODE_B4                        = 0xEEf4,

    EN_NV_ID_ANT_MODEM_LOSS_B6                      = 0xEf00,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B6          = 0xEf0d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B6          = 0xEf0e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B6        = 0xEf0f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B6        = 0xEf10,
    EN_NV_ID_LTE_IP2_CAL_B6                         = 0xEf11,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B6       = 0xEf12,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B6        = 0xEf13,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B6        = 0xEf14,
    EN_NV_ID_LTE_PA_POWER_B6                        = 0xEf15,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B6         = 0xEf16,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B6          = 0xEf17,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B6          = 0xEf18,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B6                     = 0xEf19,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B6                   = 0xEf1a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B6                   = 0xEf1b,
    EN_NV_ID_TX_APC_GAIN_B6                        = 0xEf1c,
    EN_NV_ID_RF_TXIQ_CAL_B6                        = 0xEf1d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B6                = 0xEf1e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B6               = 0xEf1f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B6                = 0xEf25,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B6               = 0xEf26,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B6              = 0xEf27,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B6           = 0xEf28,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B6	  = 0xEf29,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B6	  = 0xEf2a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B6  = 0xEf2b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B6  = 0xEf2c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B6                  = 0xEf2d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B6                  = 0xEf2e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B6                 = 0xEf2f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B6                   = 0xEf30,
    EN_NV_ID_ET_FINEDLY_PARA_B6                    = 0xEf31,
    EN_NV_ID_ET_VOFFSET_GAIN_B6                    = 0xEf32,
    EN_NV_ID_ET_EVDELY_B6                           = 0xEf33,
    EN_NV_ID_RF_CA_RCCODE_B6                        = 0xEf34,

    EN_NV_ID_ANT_MODEM_LOSS_B9                      = 0xEf40,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B9          = 0xEf4d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B9          = 0xEf4e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B9        = 0xEf4f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B9        = 0xEf50,
    EN_NV_ID_LTE_IP2_CAL_B9                         = 0xEf51,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B9       = 0xEf52,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B9        = 0xEf53,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B9        = 0xEf54,
    EN_NV_ID_LTE_PA_POWER_B9                        = 0xEf55,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B9         = 0xEf56,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B9          = 0xEf57,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B9          = 0xEf58,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B9                     = 0xEf59,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B9                   = 0xEf5a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B9                   = 0xEf5b,
    EN_NV_ID_TX_APC_GAIN_B9                        = 0xEf5c,
    EN_NV_ID_RF_TXIQ_CAL_B9                        = 0xEf5d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B9                = 0xEf5e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B9               = 0xEf5f,
    /*add for V9R1_6361 End*/


    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B9                = 0xEf65,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B9               = 0xEf66,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B9              = 0xEf67,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B9           = 0xEf68,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B9	  = 0xEf69,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B9	  = 0xEf6a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B9  = 0xEf6b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B9  = 0xEf6c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B9                  = 0xEf6d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B9                  = 0xEf6e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B9                 = 0xEf6f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B9                   = 0xEf70,
    EN_NV_ID_ET_FINEDLY_PARA_B9                    = 0xEf71,
    EN_NV_ID_ET_VOFFSET_GAIN_B9                    = 0xEf72,
    EN_NV_ID_ET_EVDELY_B9                           = 0xEf73,
    EN_NV_ID_RF_CA_RCCODE_B9                        = 0xEf74,

    EN_NV_ID_ANT_MODEM_LOSS_B10                     = 0xEf80,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B10         = 0xEf8d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B10         = 0xEf8e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B10       = 0xEf8f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B10       = 0xEf90,
    EN_NV_ID_LTE_IP2_CAL_B10                        = 0xEf91,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B10      = 0xEf92,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B10       = 0xEf93,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B10       = 0xEf94,
    EN_NV_ID_LTE_PA_POWER_B10                       = 0xEf95,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B10        = 0xEf96,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B10         = 0xEf97,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B10         = 0xEf98,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B10                    = 0xEf99,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B10                  = 0xEf9a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B10                  = 0xEf9b,
    EN_NV_ID_TX_APC_GAIN_B10                       = 0xEf9c,
    EN_NV_ID_RF_TXIQ_CAL_B10                       = 0xEf9d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B10               = 0xEf9e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B10              = 0xEf9f,
    /*add for V9R1_6361 End*/


    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B10                = 0xEfa5,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B10               = 0xEfa6,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B10              = 0xEfa7,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B10           = 0xEfa8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B10	  = 0xEfa9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B10	  = 0xEfaa,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B10  = 0xEfab,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B10  = 0xEfac,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B10                  = 0xEfad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B10                  = 0xEfae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B10                 = 0xEfaf,
    EN_NV_ID_LTE_ET_GAIN_COMP_B10                   = 0xEfb0,
    EN_NV_ID_ET_FINEDLY_PARA_B10                    = 0xEfb1,
    EN_NV_ID_ET_VOFFSET_GAIN_B10                    = 0xEfb2,
    EN_NV_ID_ET_EVDELY_B10                           = 0xEfb3,
    EN_NV_ID_RF_CA_RCCODE_B10                        = 0xEfb4,





    EN_NV_ID_ANT_MODEM_LOSS_B11                     = 0xEfc0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B11         = 0xEfcd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B11         = 0xEfce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B11       = 0xEfcf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B11       = 0xEfd0,
    EN_NV_ID_LTE_IP2_CAL_B11                        = 0xEfd1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B11      = 0xEfd2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B11       = 0xEfd3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B11       = 0xEfd4,
    EN_NV_ID_LTE_PA_POWER_B11                       = 0xEfd5,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B11        = 0xEfd6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B11         = 0xEfd7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B11         = 0xEfd8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B11                    = 0xEfd9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B11                  = 0xEfda,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B11                  = 0xEfdb,
    EN_NV_ID_TX_APC_GAIN_B11                       = 0xEfdc,
    EN_NV_ID_RF_TXIQ_CAL_B11                       = 0xEfdd,
    EN_NV_ID_PA_POWER_DIFFERENCE_B11               = 0xEfde,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B11              = 0xEfdf,
    /*add for V9R1_6361 End*/


    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B11                = 0xEfe5,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B11               = 0xEfe6,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B11              = 0xEfe7,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B11           = 0xEfe8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B11	  = 0xEfe9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B11	  = 0xEfea,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B11  = 0xEfeb,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B11  = 0xEfec,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B11                  = 0xEfed,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B11                  = 0xEfee,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B11                 = 0xEfef,
    EN_NV_ID_LTE_ET_GAIN_COMP_B11                   = 0xEff0,
    EN_NV_ID_ET_FINEDLY_PARA_B11                    = 0xEff1,
    EN_NV_ID_ET_VOFFSET_GAIN_B11                    = 0xEff2,
    EN_NV_ID_ET_EVDELY_B11                           = 0xEff3,
    EN_NV_ID_RF_CA_RCCODE_B11                        = 0xEff4,

    EN_NV_ID_ANT_MODEM_LOSS_B12                     = 0xf000,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B12         = 0xf00d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B12         = 0xf00e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B12       = 0xf00f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B12       = 0xf010,
    EN_NV_ID_LTE_IP2_CAL_B12                        = 0xf011,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B12      = 0xf012,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B12       = 0xf013,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B12       = 0xf014,
    EN_NV_ID_LTE_PA_POWER_B12                       = 0xf015,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B12        = 0xf016,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B12         = 0xf017,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B12         = 0xf018,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B12                    = 0xf019,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B12                  = 0xf01a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B12                  = 0xf01b,
    EN_NV_ID_TX_APC_GAIN_B12                       = 0xf01c,
    EN_NV_ID_RF_TXIQ_CAL_B12                       = 0xf01d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B12               = 0xf01e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B12              = 0xf01f,
    /*add for V9R1_6361 End*/


    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B12                = 0xf025,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B12               = 0xf026,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B12              = 0xf027,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B12           = 0xf028,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B12	  = 0xf029,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B12	  = 0xf02a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B12  = 0xf02b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B12  = 0xf02c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B12                  = 0xf02d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B12                  = 0xf02e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B12                 = 0xf02f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B12                   = 0xf030,
    EN_NV_ID_ET_FINEDLY_PARA_B12                    = 0xf031,
    EN_NV_ID_ET_VOFFSET_GAIN_B12                    = 0xf032,
    EN_NV_ID_ET_EVDELY_B12                           = 0xf033,
    EN_NV_ID_RF_CA_RCCODE_B12                        = 0xf034,

    EN_NV_ID_ANT_MODEM_LOSS_B13                     = 0xf040,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B13         = 0xf04d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B13         = 0xf04e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B13       = 0xf04f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B13       = 0xf050,
    EN_NV_ID_LTE_IP2_CAL_B13                        = 0xf051,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B13      = 0xf052,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B13       = 0xf053,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B13       = 0xf054,
    EN_NV_ID_LTE_PA_POWER_B13                       = 0xf055,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B13        = 0xf056,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B13         = 0xf057,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B13         = 0xf058,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B13                    = 0xf059,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B13                  = 0xf05a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B13                  = 0xf05b,
    EN_NV_ID_TX_APC_GAIN_B13                       = 0xf05c,
    EN_NV_ID_RF_TXIQ_CAL_B13                       = 0xf05d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B13               = 0xf05e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B13              = 0xf05f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B13 			   = 0xf065,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B13			   = 0xf066,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B13			   = 0xf067,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B13		   = 0xf068,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B13   = 0xf069,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B13   = 0xf06a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B13  = 0xf06b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B13  = 0xf06c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B13                  = 0xf06d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B13                  = 0xf06e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B13                 = 0xf06f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B13                   = 0xf070,
    EN_NV_ID_ET_FINEDLY_PARA_B13                    = 0xf071,
    EN_NV_ID_ET_VOFFSET_GAIN_B13                    = 0xf072,
    EN_NV_ID_ET_EVDELY_B13                           = 0xf073,
    EN_NV_ID_RF_CA_RCCODE_B13                        = 0xf074,


    EN_NV_ID_ANT_MODEM_LOSS_B14                     = 0xf080,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B14         = 0xf08d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B14         = 0xf08e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B14       = 0xf08f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B14       = 0xf090,
    EN_NV_ID_LTE_IP2_CAL_B14                        = 0xf091,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B14      = 0xf092,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B14       = 0xf093,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B14       = 0xf094,
    EN_NV_ID_LTE_PA_POWER_B14                       = 0xf095,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B14        = 0xf096,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B14         = 0xf097,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B14         = 0xf098,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B14                    = 0xf099,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B14                  = 0xf09a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B14                  = 0xf09b,
    EN_NV_ID_TX_APC_GAIN_B14                       = 0xf09c,
    EN_NV_ID_RF_TXIQ_CAL_B14                       = 0xf09d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B14               = 0xf09e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B14              = 0xf09f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B14 			   = 0xf0a5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B14			   = 0xf0a6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B14			   = 0xf0a7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B14		   = 0xf0a8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B14   = 0xf0a9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B14   = 0xf0aa,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B14  = 0xf0ab,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B14  = 0xf0ac,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B14                  = 0xf0ad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B14                  = 0xf0ae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B14                 = 0xf0af,
    EN_NV_ID_LTE_ET_GAIN_COMP_B14                   = 0xf0b0,
    EN_NV_ID_ET_FINEDLY_PARA_B14                    = 0xf0b1,
    EN_NV_ID_ET_VOFFSET_GAIN_B14                    = 0xf0b2,
    EN_NV_ID_ET_EVDELY_B14                           = 0xf0b3,
    EN_NV_ID_RF_CA_RCCODE_B14                        = 0xf0b4,






    EN_NV_ID_ANT_MODEM_LOSS_B17                     = 0xf0c0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B17         = 0xf0cd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B17         = 0xf0ce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B17       = 0xf0cf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B17       = 0xf0d0,
    EN_NV_ID_LTE_IP2_CAL_B17                        = 0xf0d1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B17      = 0xf0d2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B17       = 0xf0d3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B17       = 0xf0d4,
    EN_NV_ID_LTE_PA_POWER_B17                       = 0xf0d5,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B17        = 0xf0d6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B17         = 0xf0d7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B17         = 0xf0d8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B17                    = 0xf0d9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B17                  = 0xf0da,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B17                  = 0xf0db,
    EN_NV_ID_TX_APC_GAIN_B17                       = 0xf0dc,
    EN_NV_ID_RF_TXIQ_CAL_B17                       = 0xf0dd,
    EN_NV_ID_PA_POWER_DIFFERENCE_B17               = 0xf0de,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B17              = 0xf0df,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B17 			   = 0xf0e5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B17			   = 0xf0e6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B17			   = 0xf0e7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B17		   = 0xf0e8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B17   = 0xf0e9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B17   = 0xf0ea,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B17  = 0xf0eb,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B17  = 0xf0ec,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B17                  = 0xf0ed,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B17                  = 0xf0ee,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B17                 = 0xf0ef,
    EN_NV_ID_LTE_ET_GAIN_COMP_B17                   = 0xf0f0,
    EN_NV_ID_ET_FINEDLY_PARA_B17                    = 0xf0f1,
    EN_NV_ID_ET_VOFFSET_GAIN_B17                    = 0xf0f2,
    EN_NV_ID_ET_EVDELY_B17                           = 0xf0f3,
    EN_NV_ID_RF_CA_RCCODE_B17                        = 0xf0f4,





    EN_NV_ID_ANT_MODEM_LOSS_B18                     = 0xf100,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B18         = 0xf10d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B18         = 0xf10e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B18       = 0xf10f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B18       = 0xf110,
    EN_NV_ID_LTE_IP2_CAL_B18                        = 0xf111,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B18      = 0xf112,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B18       = 0xf113,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B18       = 0xf114,
    EN_NV_ID_LTE_PA_POWER_B18                       = 0xf115,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B18        = 0xf116,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B18         = 0xf117,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B18         = 0xf118,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B18                    = 0xf119,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B18                  = 0xf11a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B18                  = 0xf11b,
    EN_NV_ID_TX_APC_GAIN_B18                       = 0xf11c,
    EN_NV_ID_RF_TXIQ_CAL_B18                       = 0xf11d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B18               = 0xf11e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B18              = 0xf11f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B18 			   = 0xf125,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B18			   = 0xf126,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B18			   = 0xf127,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B18		   = 0xf128,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B18   = 0xf129,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B18   = 0xf12a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B18  = 0xf12b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B18  = 0xf12c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B18                  = 0xf12d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B18                  = 0xf12e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B18                 = 0xf12f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B18                   = 0xf130,
    EN_NV_ID_ET_FINEDLY_PARA_B18                    = 0xf131,
    EN_NV_ID_ET_VOFFSET_GAIN_B18                    = 0xf132,
    EN_NV_ID_ET_EVDELY_B18                           = 0xf133,
    EN_NV_ID_RF_CA_RCCODE_B18                        = 0xf134,





    EN_NV_ID_ANT_MODEM_LOSS_B22                     = 0xf140,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B22         = 0xf14d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B22         = 0xf14e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B22       = 0xf14f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B22       = 0xf150,
    EN_NV_ID_LTE_IP2_CAL_B22                        = 0xf151,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B22      = 0xf152,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B22       = 0xf153,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B22       = 0xf154,
    EN_NV_ID_LTE_PA_POWER_B22                       = 0xf155,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B22        = 0xf156,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B22         = 0xf157,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B22         = 0xf158,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B22                    = 0xf159,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B22                  = 0xf15a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B22                  = 0xf15b,
    EN_NV_ID_TX_APC_GAIN_B22                       = 0xf15c,
    EN_NV_ID_RF_TXIQ_CAL_B22                       = 0xf15d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B22               = 0xf15e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B22              = 0xf15f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B22 			   = 0xf165,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B22			   = 0xf166,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B22			   = 0xf167,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B22		   = 0xf168,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B22   = 0xf169,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B22   = 0xf16a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B22  = 0xf16b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B22  = 0xf16c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B22                  = 0xf16d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B22                  = 0xf16e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B22                 = 0xf16f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B22                   = 0xf170,
    EN_NV_ID_ET_FINEDLY_PARA_B22                    = 0xf171,
    EN_NV_ID_ET_VOFFSET_GAIN_B22                    = 0xf172,
    EN_NV_ID_ET_EVDELY_B22                           = 0xf173,
    EN_NV_ID_RF_CA_RCCODE_B22                        = 0xf174,





    EN_NV_ID_ANT_MODEM_LOSS_B23                     = 0xf180,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B23         = 0xf18d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B23         = 0xf18e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B23       = 0xf18f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B23       = 0xf190,
    EN_NV_ID_LTE_IP2_CAL_B23                        = 0xf191,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B23      = 0xf192,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B23       = 0xf193,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B23       = 0xf194,
    EN_NV_ID_LTE_PA_POWER_B23                       = 0xf195,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B23        = 0xf196,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B23         = 0xf197,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B23         = 0xf198,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B23                    = 0xf199,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B23                  = 0xf19a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B23                  = 0xf19b,
    EN_NV_ID_TX_APC_GAIN_B23                       = 0xf19c,
    EN_NV_ID_RF_TXIQ_CAL_B23                       = 0xf19d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B23               = 0xf19e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B23              = 0xf19f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B23 			   = 0xf1a5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B23			   = 0xf1a6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B23			   = 0xf1a7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B23		   = 0xf1a8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B23   = 0xf1a9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B23   = 0xf1aa,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B23  = 0xf1ab,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B23  = 0xf1ac,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B23                  = 0xf1ad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B23                  = 0xf1ae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B23                 = 0xf1af,
    EN_NV_ID_LTE_ET_GAIN_COMP_B23                   = 0xf1b0,
    EN_NV_ID_ET_FINEDLY_PARA_B23                    = 0xf1b1,
    EN_NV_ID_ET_VOFFSET_GAIN_B23                    = 0xf1b2,
    EN_NV_ID_ET_EVDELY_B23                           = 0xf1b3,
    EN_NV_ID_RF_CA_RCCODE_B23                        = 0xf1b4,

    EN_NV_ID_ANT_MODEM_LOSS_B24                     = 0xf1c0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B24         = 0xf1cd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B24         = 0xf1ce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B24       = 0xf1cf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B24       = 0xf1d0,
    EN_NV_ID_LTE_IP2_CAL_B24                        = 0xf1d1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B24      = 0xf1d2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B24       = 0xf1d3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B24       = 0xf1d4,
    EN_NV_ID_LTE_PA_POWER_B24                       = 0xf1d5,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B24        = 0xf1d6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B24         = 0xf1d7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B24         = 0xf1d8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B24                    = 0xf1d9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B24                  = 0xf1da,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B24                  = 0xf1db,
    EN_NV_ID_TX_APC_GAIN_B24                       = 0xf1dc,
    EN_NV_ID_RF_TXIQ_CAL_B24                       = 0xf1dd,
    EN_NV_ID_PA_POWER_DIFFERENCE_B24               = 0xf1de,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B24              = 0xf1df,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B24 			   = 0xf1e5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B24			   = 0xf1e6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B24			   = 0xf1e7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B24		   = 0xf1e8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B24   = 0xf1e9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B24   = 0xf1ea,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B24  = 0xf1eb,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B24  = 0xf1ec,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B24                  = 0xf1ed,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B24                  = 0xf1ee,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B24                 = 0xf1ef,
    EN_NV_ID_LTE_ET_GAIN_COMP_B24                   = 0xf1f0,
    EN_NV_ID_ET_FINEDLY_PARA_B24                    = 0xf1f1,
    EN_NV_ID_ET_VOFFSET_GAIN_B24                    = 0xf1f2,
    EN_NV_ID_ET_EVDELY_B24                           = 0xf1f3,
    EN_NV_ID_RF_CA_RCCODE_B24                        = 0xf1f4,

    EN_NV_ID_ANT_MODEM_LOSS_B25                     = 0xf200,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B25         = 0xf20d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B25         = 0xf20e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B25       = 0xf20f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B25       = 0xf210,
    EN_NV_ID_LTE_IP2_CAL_B25                        = 0xf211,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B25      = 0xf212,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B25       = 0xf213,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B25       = 0xf214,
    EN_NV_ID_LTE_PA_POWER_B25                       = 0xf215,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B25        = 0xf216,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B25         = 0xf217,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B25         = 0xf218,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B25                    = 0xf219,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B25                  = 0xf21a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B25                  = 0xf21b,
    EN_NV_ID_TX_APC_GAIN_B25                       = 0xf21c,
    EN_NV_ID_RF_TXIQ_CAL_B25                       = 0xf21d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B25               = 0xf21e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B25              = 0xf21f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B25 			   = 0xf225,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B25			   = 0xf226,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B25			   = 0xf227,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B25		   = 0xf228,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B25   = 0xf229,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B25   = 0xf22a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B25  = 0xf22b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B25  = 0xf22c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B25                  = 0xf22d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B25                  = 0xf22e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B25                 = 0xf22f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B25                   = 0xf230,
    EN_NV_ID_ET_FINEDLY_PARA_B25                    = 0xf231,
    EN_NV_ID_ET_VOFFSET_GAIN_B25                    = 0xf232,
    EN_NV_ID_ET_EVDELY_B25                           = 0xf233,
    EN_NV_ID_RF_CA_RCCODE_B25                        = 0xf234,

    EN_NV_ID_ANT_MODEM_LOSS_B33                     = 0xf240,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B33         = 0xf24d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B33         = 0xf24e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B33       = 0xf24f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B33       = 0xf250,
    EN_NV_ID_LTE_IP2_CAL_B33                        = 0xf251,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B33      = 0xf252,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B33       = 0xf253,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B33       = 0xf254,
    EN_NV_ID_LTE_PA_POWER_B33                       = 0xf255,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B33        = 0xf256,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B33         = 0xf257,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B33         = 0xf258,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B33                    = 0xf259,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B33                  = 0xf25a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B33                  = 0xf25b,
    EN_NV_ID_TX_APC_GAIN_B33                       = 0xf25c,
    EN_NV_ID_RF_TXIQ_CAL_B33                       = 0xf25d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B33               = 0xf25e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B33              = 0xf25f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B33 			   = 0xf265,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B33			   = 0xf266,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B33			   = 0xf267,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B33		   = 0xf268,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B33   = 0xf269,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B33   = 0xf26a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B33  = 0xf26b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B33  = 0xf26c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B33                  = 0xf26d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B33                  = 0xf26e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B33                 = 0xf26f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B33                   = 0xf270,
    EN_NV_ID_ET_FINEDLY_PARA_B33                    = 0xf271,
    EN_NV_ID_ET_VOFFSET_GAIN_B33                    = 0xf272,
    EN_NV_ID_ET_EVDELY_B33                           = 0xf273,
    EN_NV_ID_RF_CA_RCCODE_B33                        = 0xf274,





    EN_NV_ID_ANT_MODEM_LOSS_B34                     = 0xf280,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B34         = 0xf28d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B34         = 0xf28e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B34       = 0xf28f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B34       = 0xf290,
    EN_NV_ID_LTE_IP2_CAL_B34                        = 0xf291,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B34      = 0xf292,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B34       = 0xf293,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B34       = 0xf294,
    EN_NV_ID_LTE_PA_POWER_B34                       = 0xf295,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B34        = 0xf296,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B34         = 0xf297,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B34         = 0xf298,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B34                    = 0xf299,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B34                  = 0xf29a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B34                  = 0xf29b,
    EN_NV_ID_TX_APC_GAIN_B34                       = 0xf29c,
    EN_NV_ID_RF_TXIQ_CAL_B34                       = 0xf29d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B34               = 0xf29e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B34              = 0xf29f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B34 			   = 0xf2a5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B34			   = 0xf2a6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B34			   = 0xf2a7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B34		   = 0xf2a8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B34   = 0xf2a9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B34   = 0xf2aa,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B34  = 0xf2ab,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B34  = 0xf2ac,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B34                  = 0xf2ad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B34                  = 0xf2ae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B34                 = 0xf2af,
    EN_NV_ID_LTE_ET_GAIN_COMP_B34                   = 0xf2b0,
    EN_NV_ID_ET_FINEDLY_PARA_B34                    = 0xf2b1,
    EN_NV_ID_ET_VOFFSET_GAIN_B34                    = 0xf2b2,
    EN_NV_ID_ET_EVDELY_B34                           = 0xf2b3,
    EN_NV_ID_RF_CA_RCCODE_B34                        = 0xf2b4,

    EN_NV_ID_ANT_MODEM_LOSS_B35                     = 0xf2c0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B35         = 0xf2cd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B35         = 0xf2ce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B35       = 0xf2cf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B35       = 0xf2d0,
    EN_NV_ID_LTE_IP2_CAL_B35                        = 0xf2d1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B35      = 0xf2d2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B35       = 0xf2d3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B35       = 0xf2d4,
    EN_NV_ID_LTE_PA_POWER_B35                       = 0xf2d5,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B35        = 0xf2d6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B35         = 0xf2d7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B35         = 0xf2d8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B35                    = 0xf2d9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B35                  = 0xf2da,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B35                  = 0xf2db,
    EN_NV_ID_TX_APC_GAIN_B35                       = 0xf2dc,
    EN_NV_ID_RF_TXIQ_CAL_B35                       = 0xf2dd,
    EN_NV_ID_PA_POWER_DIFFERENCE_B35               = 0xf2de,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B35              = 0xf2df,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B35 			   = 0xf2e5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B35			   = 0xf2e6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B35			   = 0xf2e7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B35		   = 0xf2e8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B35   = 0xf2e9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B35   = 0xf2ea,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B35  = 0xf2eb,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B35  = 0xf2ec,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B35                  = 0xf2ed,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B35                  = 0xf2ee,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B35                 = 0xf2ef,
    EN_NV_ID_LTE_ET_GAIN_COMP_B35                   = 0xf2f0,
    EN_NV_ID_ET_FINEDLY_PARA_B35                    = 0xf2f1,
    EN_NV_ID_ET_VOFFSET_GAIN_B35                    = 0xf2f2,
    EN_NV_ID_ET_EVDELY_B35                           = 0xf2f3,
    EN_NV_ID_RF_CA_RCCODE_B35                        = 0xf2f4,

    EN_NV_ID_ANT_MODEM_LOSS_B36                     = 0xf300,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B36         = 0xf30d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B36         = 0xf30e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B36       = 0xf30f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B36       = 0xf310,
    EN_NV_ID_LTE_IP2_CAL_B36                        = 0xf311,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B36      = 0xf312,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B36       = 0xf313,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B36       = 0xf314,
    EN_NV_ID_LTE_PA_POWER_B36                       = 0xf315,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B36        = 0xf316,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B36         = 0xf317,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B36         = 0xf318,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B36                    = 0xf319,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B36                  = 0xf31a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B36                  = 0xf31b,
    EN_NV_ID_TX_APC_GAIN_B36                       = 0xf31c,
    EN_NV_ID_RF_TXIQ_CAL_B36                       = 0xf31d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B36               = 0xf31e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B36              = 0xf31f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B36 			   = 0xf325,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B36			   = 0xf326,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B36			   = 0xf327,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B36		   = 0xf328,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B36   = 0xf329,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B36   = 0xf32a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B36  = 0xf32b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B36  = 0xf32c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B36                  = 0xf32d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B36                  = 0xf32e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B36                 = 0xf32f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B36                   = 0xf330,
    EN_NV_ID_ET_FINEDLY_PARA_B36                    = 0xf331,
    EN_NV_ID_ET_VOFFSET_GAIN_B36                    = 0xf332,
    EN_NV_ID_ET_EVDELY_B36                           = 0xf333,
    EN_NV_ID_RF_CA_RCCODE_B36                        = 0xf334,

    EN_NV_ID_ANT_MODEM_LOSS_B37                     = 0xf340,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B37         = 0xf34d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B37         = 0xf34e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B37       = 0xf34f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B37       = 0xf350,
    EN_NV_ID_LTE_IP2_CAL_B37                        = 0xf351,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B37      = 0xf352,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B37       = 0xf353,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B37       = 0xf354,
    EN_NV_ID_LTE_PA_POWER_B37                       = 0xf355,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B37        = 0xf356,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B37         = 0xf357,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B37         = 0xf358,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B37                    = 0xf359,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B37                  = 0xf35a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B37                  = 0xf35b,
    EN_NV_ID_TX_APC_GAIN_B37                       = 0xf35c,
    EN_NV_ID_RF_TXIQ_CAL_B37                       = 0xf35d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B37               = 0xf35e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B37              = 0xf35f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B37 			   = 0xf365,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B37			   = 0xf366,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B37			   = 0xf367,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B37		   = 0xf368,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B37   = 0xf369,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B37   = 0xf36a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B37  = 0xf36b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B37  = 0xf36c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B37                  = 0xf36d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B37                  = 0xf36e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B37                 = 0xf36f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B37                   = 0xf370,
    EN_NV_ID_ET_FINEDLY_PARA_B37                    = 0xf371,
    EN_NV_ID_ET_VOFFSET_GAIN_B37                    = 0xf372,
    EN_NV_ID_ET_EVDELY_B37                           = 0xf373,
    EN_NV_ID_RF_CA_RCCODE_B37                        = 0xf374,

    EN_NV_ID_ANT_MODEM_LOSS_B42                     = 0xf380,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B42         = 0xf38d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B42         = 0xf38e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B42       = 0xf38f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B42       = 0xf390,
    EN_NV_ID_LTE_IP2_CAL_B42                        = 0xf391,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B42      = 0xf392,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B42       = 0xf393,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B42       = 0xf394,
    EN_NV_ID_LTE_PA_POWER_B42                       = 0xf395,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B42        = 0xf396,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B42         = 0xf397,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B42         = 0xf398,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B42                    = 0xf399,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B42                  = 0xf39a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B42                  = 0xf39b,
    EN_NV_ID_TX_APC_GAIN_B42                       = 0xf39c,
    EN_NV_ID_RF_TXIQ_CAL_B42                       = 0xf39d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B42               = 0xf39e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B42              = 0xf39f,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B42 			   = 0xf3a5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B42			   = 0xf3a6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B42			   = 0xf3a7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B42		   = 0xf3a8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B42   = 0xf3a9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B42   = 0xf3aa,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B42  = 0xf3ab,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B42  = 0xf3ac,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B42                  = 0xf3ad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B42                  = 0xf3ae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B42                 = 0xf3af,
    EN_NV_ID_LTE_ET_GAIN_COMP_B42                   = 0xf3b0,
    EN_NV_ID_ET_FINEDLY_PARA_B42                    = 0xf3b1,
    EN_NV_ID_ET_VOFFSET_GAIN_B42                    = 0xf3b2,
    EN_NV_ID_ET_EVDELY_B42                           = 0xf3b3,
    EN_NV_ID_RF_CA_RCCODE_B42                        = 0xf3b4,

    EN_NV_ID_ANT_MODEM_LOSS_B43                     = 0xf3c0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B43         = 0xf3cd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B43         = 0xf3ce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B43       = 0xf3cf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B43       = 0xf3d0,
    EN_NV_ID_LTE_IP2_CAL_B43                        = 0xf3d1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B43      = 0xf3d2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B43       = 0xf3d3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B43       = 0xf3d4,
    EN_NV_ID_LTE_PA_POWER_B43                       = 0xf3d5,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B43        = 0xf3d6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B43         = 0xf3d7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B43         = 0xf3d8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B43                    = 0xf3d9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B43                  = 0xf3da,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B43                  = 0xf3db,
    EN_NV_ID_TX_APC_GAIN_B43                       = 0xf3dc,
    EN_NV_ID_RF_TXIQ_CAL_B43                       = 0xf3dd,
    EN_NV_ID_PA_POWER_DIFFERENCE_B43               = 0xf3de,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B43              = 0xf3df,
    /*add for V9R1_6361 End*/


	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B43 			   = 0xf3e5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B43			   = 0xf3e6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B43			   = 0xf3e7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B43		   = 0xf3e8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B43   = 0xf3e9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B43   = 0xf3ea,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B43  = 0xf3eb,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B43  = 0xf3ec,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B43                  = 0xf3ed,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B43                  = 0xf3ee,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B43                 = 0xf3ef,
    EN_NV_ID_LTE_ET_GAIN_COMP_B43                   = 0xf3f0,
    EN_NV_ID_ET_FINEDLY_PARA_B43                    = 0xf3f1,
    EN_NV_ID_ET_VOFFSET_GAIN_B43                    = 0xf3f2,
    EN_NV_ID_ET_EVDELY_B43                           = 0xf3f3,
    EN_NV_ID_RF_CA_RCCODE_B43                        = 0xf3f4,

    EN_NV_ID_ANT_MODEM_LOSS_B39                     = 0xf400,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B39         = 0xf40d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B39         = 0xf40e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B39       = 0xf40f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B39       = 0xf410,
    EN_NV_ID_LTE_IP2_CAL_B39                        = 0xf411,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B39      = 0xf412,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B39       = 0xf413,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B39       = 0xf414,
    EN_NV_ID_LTE_PA_POWER_B39                       = 0xf415,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B39        = 0xf416,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B39         = 0xf417,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B39         = 0xf418,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_B39                    = 0xf419,
    EN_NV_ID_RF_DCOC_CAL_ANT1_B39                  = 0xf41a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_B39                  = 0xf41b,
    EN_NV_ID_TX_APC_GAIN_B39                       = 0xf41c,
    EN_NV_ID_RF_TXIQ_CAL_B39                       = 0xf41d,
    EN_NV_ID_PA_POWER_DIFFERENCE_B39               = 0xf41e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_B39              = 0xf41f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_LTE_TX_PD_AUTO_CAL_B39                = 0xf425,
    EN_NV_ID_LTE_TX_PD_PWR_TABLE_B39               = 0xf426,
    EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B39              = 0xf427,
    EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B39           = 0xf428,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B39	  = 0xf429,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B39	  = 0xf42a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B39  = 0xf42b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B39  = 0xf42c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B39                  = 0xf42d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B39                  = 0xf42e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B39                 = 0xf42f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B39                   = 0xf430,
    EN_NV_ID_ET_FINEDLY_PARA_B39                    = 0xf431,
    EN_NV_ID_ET_VOFFSET_GAIN_B39                    = 0xf432,
    EN_NV_ID_ET_EVDELY_B39                           = 0xf433,
    EN_NV_ID_RF_CA_RCCODE_B39                        = 0xf434,

/*BAND28 生产NV*/
	EN_NV_ID_ANT_MODEM_LOSS_B28						= 0xf440,
	EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B28			= 0xf44D,
	EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B28			= 0xf44E,
	EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B28		= 0xf44F,
	EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B28		= 0xf450,
	EN_NV_ID_LTE_IP2_CAL_B28 						= 0xf451,
	EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B28		= 0xf452,
	EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B28		= 0xf453,
	EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B28		= 0xf454,
	EN_NV_ID_LTE_PA_POWER_B28						= 0xf455,
	EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B28 		= 0xf456,
	EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B28			= 0xf457,
	EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B28			= 0xf458,
	/*add for V9R1_6361 Begin*/
	EN_NV_ID_IIP2_CAL_TABLE_B28					  = 0xf459,
	EN_NV_ID_RF_DCOC_CAL_ANT1_B28				  = 0xf45a,
	EN_NV_ID_RF_DCOC_CAL_ANT2_B28				  = 0xf45b,
	EN_NV_ID_TX_APC_GAIN_B28 					  = 0xf45c,
	EN_NV_ID_RF_TXIQ_CAL_B28 					  = 0xf45d,
	EN_NV_ID_PA_POWER_DIFFERENCE_B28 			  = 0xf45e,
	EN_NV_ID_TX_RF_FREQ_COMP_STRU_B28			  = 0xf45f,
	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B28				  = 0xf465,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B28 			  = 0xf466,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B28			  = 0xf467,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B28		  = 0xf468,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B28	  = 0xf469,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B28	  = 0xf46a,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B28  = 0xf46b,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B28  = 0xf46c,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B28                  = 0xf46d,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B28                  = 0xf46e,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B28                 = 0xf46f,
    EN_NV_ID_LTE_ET_GAIN_COMP_B28                   = 0xf470,
    EN_NV_ID_ET_FINEDLY_PARA_B28                    = 0xf471,
    EN_NV_ID_ET_VOFFSET_GAIN_B28                    = 0xf472,
    EN_NV_ID_ET_EVDELY_B28                           = 0xf473,
    EN_NV_ID_RF_CA_RCCODE_B28                        = 0xf474,

/*
    EN_NV_ID_ANT_MODEM_LOSS_BNon1                     = 0xf440,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon1         = 0xf44d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon1         = 0xf44e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon1       = 0xf44f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon1       = 0xf450,
    EN_NV_ID_LTE_IP2_CAL_BNon1                        = 0xf451,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon1      = 0xf452,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon1       = 0xf453,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon1       = 0xf454,
    EN_NV_ID_LTE_PA_POWER_BNon1                       = 0xf455,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon1        = 0xf456,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon1         = 0xf457,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon1         = 0xf458,
    */
    /*add for V9R1_6361 Begin*/
/*    EN_NV_ID_IIP2_CAL_TABLE_BNon1                    = 0xf459,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon1                 = 0xf45a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon1                  = 0xf45b,
    EN_NV_ID_TX_APC_GAIN_BNon1                       = 0xf45c,
    EN_NV_ID_RF_TXIQ_CAL_BNon1                       = 0xf45d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon1               = 0xf45e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon1              = 0xf45f,
    */
    /*add for V9R1_6361 End*/
   /*BAND128 生产NV*/
	EN_NV_ID_ANT_MODEM_LOSS_B128						= 0xf480,
	EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_B128		= 0xf48D,
	EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_B128		= 0xf48E,
	EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_B128		= 0xf48F,
	EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_B128		= 0xf490,
	EN_NV_ID_LTE_IP2_CAL_B128 						= 0xf491,
	EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_B128		= 0xf492,
	EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_B128		= 0xf493,
	EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_B128		= 0xf494,
	EN_NV_ID_LTE_PA_POWER_B128						= 0xf495,
	EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_B128 		= 0xf496,
	EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_B128		= 0xf497,
	EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_B128		= 0xf498,
	/*add for V9R1_6361 Begin*/
	EN_NV_ID_IIP2_CAL_TABLE_B128					    = 0xf499,
	EN_NV_ID_RF_DCOC_CAL_ANT1_B128				    = 0xf49a,
	EN_NV_ID_RF_DCOC_CAL_ANT2_B128				    = 0xf49b,
	EN_NV_ID_TX_APC_GAIN_B128 					    = 0xf49c,
	EN_NV_ID_RF_TXIQ_CAL_B128 					    = 0xf49d,
	EN_NV_ID_PA_POWER_DIFFERENCE_B128 			    = 0xf49e,
	EN_NV_ID_TX_RF_FREQ_COMP_STRU_B128			    = 0xf49f,
	EN_NV_ID_LTE_TX_PD_AUTO_CAL_B128				    = 0xf4a5,
	EN_NV_ID_LTE_TX_PD_PWR_TABLE_B128 			    = 0xf4a6,
	EN_NV_ID_LTE_TX_PD_VOLT_TABLE_B128			    = 0xf4a7,
	EN_NV_ID_LTE_TX_PD_TEMPCMP_TABLE_B128		    = 0xf4a8,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT1_B128	= 0xf4a9,
	EN_NV_ID_LTE_AGC_SCC_BLK_FREQ_COMP_ANT2_B128	= 0xf4aa,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT1_B128 = 0xf4ab,
	EN_NV_ID_LTE_AGC_SCC_NOBLK_FREQ_COMP_ANT2_B128 = 0xf4ac,
    EN_NV_ID_ET_LUT_TBL_LOWCH_B128                   = 0xf4ad,
    EN_NV_ID_ET_LUT_TBL_MIDCH_B128                   = 0xf4ae,
    EN_NV_ID_ET_LUT_TBL_HIGHCH_B128                  = 0xf4af,
    EN_NV_ID_LTE_ET_GAIN_COMP_B128                   = 0xf4b0,
    EN_NV_ID_ET_FINEDLY_PARA_B128                    = 0xf4b1,
    EN_NV_ID_ET_VOFFSET_GAIN_B128                    = 0xf4b2,
    EN_NV_ID_ET_EVDELY_B128                           = 0xf4b3,
    EN_NV_ID_RF_CA_RCCODE_B128                        = 0xf4b4,
#if 0
    EN_NV_ID_ANT_MODEM_LOSS_BNon2                     = 0xf480,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon2         = 0xf48d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon2         = 0xf48e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon2       = 0xf48f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon2       = 0xf490,
    EN_NV_ID_LTE_IP2_CAL_BNon2                        = 0xf491,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon2      = 0xf492,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon2       = 0xf493,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon2       = 0xf494,
    EN_NV_ID_LTE_PA_POWER_BNon2                       = 0xf495,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon2        = 0xf496,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon2         = 0xf497,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon2         = 0xf498,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon2                    = 0xf499,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon2                 = 0xf49a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon2                  = 0xf49b,
    EN_NV_ID_TX_APC_GAIN_BNon2                       = 0xf49c,
    EN_NV_ID_RF_TXIQ_CAL_BNon2                       = 0xf49d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon2               = 0xf49e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon2              = 0xf49f,
    /*add for V9R1_6361 End*/
#endif

    EN_NV_ID_ANT_MODEM_LOSS_BNon3                     = 0xf4c0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon3         = 0xf4cd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon3         = 0xf4ce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon3       = 0xf4cf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon3       = 0xf4d0,
    EN_NV_ID_LTE_IP2_CAL_BNon3                        = 0xf4d1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon3      = 0xf4d2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon3       = 0xf4d3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon3       = 0xf4d4,
    EN_NV_ID_LTE_PA_POWER_BNon3                       = 0xf4d5,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon3        = 0xf4d6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon3         = 0xf4d7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon3         = 0xf4d8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon3                    = 0xf4d9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon3                 = 0xf4da,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon3                  = 0xf4db,
    EN_NV_ID_TX_APC_GAIN_BNon3                       = 0xf4dc,
    EN_NV_ID_RF_TXIQ_CAL_BNon3                       = 0xf4dd,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon3               = 0xf4de,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon3              = 0xf4df,
    /*add for V9R1_6361 End*/
    EN_NV_ID_ANT_MODEM_LOSS_BNon4                     = 0xf500,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon4         = 0xf50d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon4         = 0xf50e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon4       = 0xf50f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon4       = 0xf510,
    EN_NV_ID_LTE_IP2_CAL_BNon4                        = 0xf511,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon4      = 0xf512,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon4       = 0xf513,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon4       = 0xf514,
    EN_NV_ID_LTE_PA_POWER_BNon4                       = 0xf515,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon4        = 0xf516,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon4         = 0xf517,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon4         = 0xf518,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon4                    = 0xf519,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon4                 = 0xf51a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon4                  = 0xf51b,
    EN_NV_ID_TX_APC_GAIN_BNon4                       = 0xf51c,
    EN_NV_ID_RF_TXIQ_CAL_BNon4                       = 0xf51d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon4               = 0xf51e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon4              = 0xf51f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon5                     = 0xf540,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon5         = 0xf54d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon5         = 0xf54e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon5       = 0xf54f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon5       = 0xf550,
    EN_NV_ID_LTE_IP2_CAL_BNon5                        = 0xf551,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon5      = 0xf552,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon5       = 0xf553,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon5       = 0xf554,
    EN_NV_ID_LTE_PA_POWER_BNon5                       = 0xf555,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon5        = 0xf556,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon5         = 0xf557,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon5         = 0xf558,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon5                    = 0xf559,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon5                 = 0xf55a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon5                  = 0xf55b,
    EN_NV_ID_TX_APC_GAIN_BNon5                       = 0xf55c,
    EN_NV_ID_RF_TXIQ_CAL_BNon5                       = 0xf55d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon5               = 0xf55e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon5              = 0xf55f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon6                     = 0xf580,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon6         = 0xf58d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon6         = 0xf58e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon6       = 0xf58f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon6       = 0xf590,
    EN_NV_ID_LTE_IP2_CAL_BNon6                        = 0xf591,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon6      = 0xf592,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon6       = 0xf593,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon6       = 0xf594,
    EN_NV_ID_LTE_PA_POWER_BNon6                       = 0xf595,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon6        = 0xf596,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon6         = 0xf597,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon6         = 0xf598,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon6                    = 0xf599,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon6                 = 0xf59a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon6                  = 0xf59b,
    EN_NV_ID_TX_APC_GAIN_BNon6                       = 0xf59c,
    EN_NV_ID_RF_TXIQ_CAL_BNon6                       = 0xf59d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon6               = 0xf59e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon6              = 0xf59f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon7                     = 0xf5c0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon7         = 0xf5cd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon7         = 0xf5ce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon7       = 0xf5cf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon7       = 0xf5d0,
    EN_NV_ID_LTE_IP2_CAL_BNon7                        = 0xf5d1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon7      = 0xf5d2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon7       = 0xf5d3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon7       = 0xf5d4,
    EN_NV_ID_LTE_PA_POWER_BNon7                       = 0xf5d5,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon7        = 0xf5d6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon7         = 0xf5d7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon7         = 0xf5d8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon7                    = 0xf5d9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon7                 = 0xf5da,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon7                  = 0xf5db,
    EN_NV_ID_TX_APC_GAIN_BNon7                       = 0xf5dc,
    EN_NV_ID_RF_TXIQ_CAL_BNon7                       = 0xf5dd,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon7               = 0xf5de,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon7              = 0xf5df,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon8                     = 0xf600,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon8         = 0xf60d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon8         = 0xf60e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon8       = 0xf60f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon8       = 0xf610,
    EN_NV_ID_LTE_IP2_CAL_BNon8                        = 0xf611,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon8      = 0xf612,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon8       = 0xf613,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon8       = 0xf614,
    EN_NV_ID_LTE_PA_POWER_BNon8                       = 0xf615,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon8        = 0xf616,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon8         = 0xf617,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon8         = 0xf618,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon8                    = 0xf619,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon8                 = 0xf61a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon8                  = 0xf61b,
    EN_NV_ID_TX_APC_GAIN_BNon8                       = 0xf61c,
    EN_NV_ID_RF_TXIQ_CAL_BNon8                       = 0xf61d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon8               = 0xf61e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon8              = 0xf61f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon9                     = 0xf640,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon9         = 0xf64d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon9         = 0xf64e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon9       = 0xf64f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon9       = 0xf650,
    EN_NV_ID_LTE_IP2_CAL_BNon9                        = 0xf651,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon9      = 0xf652,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon9       = 0xf653,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon9       = 0xf654,
    EN_NV_ID_LTE_PA_POWER_BNon9                       = 0xf655,

    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon9        = 0xf656,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon9         = 0xf657,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon9         = 0xf658,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon9                    = 0xf659,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon9                 = 0xf65a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon9                  = 0xf65b,
    EN_NV_ID_TX_APC_GAIN_BNon9                       = 0xf65c,
    EN_NV_ID_RF_TXIQ_CAL_BNon9                       = 0xf65d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon9               = 0xf65e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon9              = 0xf65f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon10                     = 0xf680,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon10         = 0xf68d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon10         = 0xf68e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon10       = 0xf68f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon10       = 0xf690,
    EN_NV_ID_LTE_IP2_CAL_BNon10                        = 0xf691,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon10      = 0xf692,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon10       = 0xf693,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon10       = 0xf694,
    EN_NV_ID_LTE_PA_POWER_BNon10                       = 0xf695,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon10        = 0xf696,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon10         = 0xf697,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon10         = 0xf698,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon10                   = 0xf699,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon10                = 0xf69a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon10                 = 0xf69b,
    EN_NV_ID_TX_APC_GAIN_BNon10                      = 0xf69c,
    EN_NV_ID_RF_TXIQ_CAL_BNon10                      = 0xf69d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon10              = 0xf69e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon10             = 0xf69f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon11                     = 0xf6c0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon11         = 0xf6cd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon11         = 0xf6ce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon11       = 0xf6cf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon11       = 0xf6d0,
    EN_NV_ID_LTE_IP2_CAL_BNon11                        = 0xf6d1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon11      = 0xf6d2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon11       = 0xf6d3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon11       = 0xf6d4,
    EN_NV_ID_LTE_PA_POWER_BNon11                       = 0xf6d5,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon11        = 0xf6d6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon11         = 0xf6d7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon11         = 0xf6d8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon11                   = 0xf6d9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon11                = 0xf6da,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon11                 = 0xf6db,
    EN_NV_ID_TX_APC_GAIN_BNon11                      = 0xf6dc,
    EN_NV_ID_RF_TXIQ_CAL_BNon11                      = 0xf6dd,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon11              = 0xf6de,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon11             = 0xf6df,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon12                     = 0xf700,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon12         = 0xf70d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon12         = 0xf70e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon12       = 0xf70f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon12       = 0xf710,
    EN_NV_ID_LTE_IP2_CAL_BNon12                        = 0xf711,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon12      = 0xf712,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon12       = 0xf713,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon12       = 0xf714,
    EN_NV_ID_LTE_PA_POWER_BNon12                       = 0xf715,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon12        = 0xf716,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon12         = 0xf717,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon12         = 0xf718,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon12                   = 0xf719,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon12                = 0xf71a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon12                 = 0xf71b,
    EN_NV_ID_TX_APC_GAIN_BNon12                      = 0xf71c,
    EN_NV_ID_RF_TXIQ_CAL_BNon12                      = 0xf71d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon12              = 0xf71e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon12             = 0xf71f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon13                     = 0xf740,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon13         = 0xf74d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon13         = 0xf74e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon13       = 0xf74f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon13       = 0xf750,
    EN_NV_ID_LTE_IP2_CAL_BNon13                        = 0xf751,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon13      = 0xf752,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon13       = 0xf753,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon13       = 0xf754,
    EN_NV_ID_LTE_PA_POWER_BNon13                       = 0xf755,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon13        = 0xf756,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon13         = 0xf757,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon13         = 0xf758,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon13                   = 0xf759,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon13                = 0xf75a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon13                 = 0xf75b,
    EN_NV_ID_TX_APC_GAIN_BNon13                      = 0xf75c,
    EN_NV_ID_RF_TXIQ_CAL_BNon13                      = 0xf75d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon13              = 0xf75e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon13             = 0xf75f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon14                     = 0xf780,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon14         = 0xf78d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon14         = 0xf78e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon14       = 0xf78f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon14       = 0xf790,
    EN_NV_ID_LTE_IP2_CAL_BNon14                        = 0xf791,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon14      = 0xf792,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon14       = 0xf793,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon14       = 0xf794,
    EN_NV_ID_LTE_PA_POWER_BNon14                       = 0xf795,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon14        = 0xf796,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon14         = 0xf797,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon14         = 0xf798,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon14                   = 0xf799,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon14                = 0xf79a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon14                 = 0xf79b,
    EN_NV_ID_TX_APC_GAIN_BNon14                      = 0xf79c,
    EN_NV_ID_RF_TXIQ_CAL_BNon14                      = 0xf79d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon14              = 0xf79e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon14             = 0xf79f,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon15                     = 0xf7c0,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon15         = 0xf7cd,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon15         = 0xf7ce,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon15       = 0xf7cf,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon15       = 0xf7d0,
    EN_NV_ID_LTE_IP2_CAL_BNon15                        = 0xf7d1,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon15      = 0xf7d2,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon15       = 0xf7d3,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon15       = 0xf7d4,
    EN_NV_ID_LTE_PA_POWER_BNon15                       = 0xf7d5,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon15        = 0xf7d6,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon15         = 0xf7d7,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon15         = 0xf7d8,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon15                   = 0xf7d9,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon15                = 0xf7da,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon15                 = 0xf7db,
    EN_NV_ID_TX_APC_GAIN_BNon15                      = 0xf7dc,
    EN_NV_ID_RF_TXIQ_CAL_BNon15                      = 0xf7dd,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon15              = 0xf7de,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon15             = 0xf7df,
    /*add for V9R1_6361 End*/

    EN_NV_ID_ANT_MODEM_LOSS_BNon16                     = 0xf800,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT1_BNon16         = 0xf80d,
    EN_NV_ID_LTE_AGC_BLK_FREQ_COMP_ANT2_BNon16         = 0xf80e,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT1_BNon16       = 0xf80f,
    EN_NV_ID_LTE_AGC_NOBLK_FREQ_COMP_ANT2_BNon16       = 0xf810,
    EN_NV_ID_LTE_IP2_CAL_BNon16                        = 0xf811,
    EN_NV_ID_LTE_TX_APC_HIGHGAIN_FREQ_COMP_BNon16      = 0xf812,
    EN_NV_ID_LTE_TX_APC_MIDGAIN_FREQ_COMP_BNon16       = 0xf813,
    EN_NV_ID_LTE_TX_APC_LOWGAIN_FREQ_COMP_BNon16       = 0xf814,
    EN_NV_ID_LTE_PA_POWER_BNon16                       = 0xf815,
    EN_NV_ID_LTE_TX_APT_COMP_MC_HIGH_TBL_BNon16        = 0xf816,
    EN_NV_ID_LTE_TX_APT_COMP_MC_MID_TBL_BNon16         = 0xf817,
    EN_NV_ID_LTE_TX_APT_COMP_MC_LOW_TBL_BNon16         = 0xf818,
    /*add for V9R1_6361 Begin*/
    EN_NV_ID_IIP2_CAL_TABLE_BNon16                   = 0xf819,
    EN_NV_ID_RF_DCOC_CAL_ANT1_BNon16                = 0xf81a,
    EN_NV_ID_RF_DCOC_CAL_ANT2_BNon16                 = 0xf81b,
    EN_NV_ID_TX_APC_GAIN_BNon16                      = 0xf81c,
    EN_NV_ID_RF_TXIQ_CAL_BNon16                      = 0xf81d,
    EN_NV_ID_PA_POWER_DIFFERENCE_BNon16              = 0xf81e,
    EN_NV_ID_TX_RF_FREQ_COMP_STRU_BNon16             = 0xf81f,
    /*add for V9R1_6361 End*/



    EN_NV_ID_TIMING_PARA                            = 0xD3C0,
    EN_NV_ID_EMU_FAKECELL_PARA                      = 0xD3C1,
    EN_NV_ID_CQI_PARA                               = 0xD3C2,
    EN_NV_ID_ANTCORR_PARA                           = 0xD3C3,
    EN_NV_ID_RLM_PARA                               = 0xD3C4,
    EN_NV_ID_AFC_PARA                               = 0xD3C5,
    EN_NV_ID_IRC_PUB_PARA                           = 0xD3C6,
    EN_NV_ID_CHE_PARA                               = 0xD3C7,
    EN_NV_ID_VITERBI_PARA                           = 0xD3C8,
    EN_NV_ID_TURBO_PARA                             = 0xD3C9,
    EN_NV_ID_DEM_LIST_PARA                          = 0xD3CA,
    EN_NV_ID_AD9361_UL_PARA                         = 0xD3CB,
    EN_NV_ID_HI6360_UL_PARA                         = 0xD3CC,

    /* modify by lisahgnfeng begin */
    EN_NV_ID_PHY_FUNC_VERIFY_SWITCH_PARA            = 0xD3E0,
    /* modify by lisahgnfeng end */

    /*tcx0*/
    EN_NV_ID_TCXO_DYNAMIC_CONFIG_PARA               = 0xD3E2,

    EN_NV_ID_TX_FILTER_CMP                          = 0xD3E3,

    EN_NV_ID_LPHY_PD_COMM_PARA                      = 0xD3E5,

    /* BEGIN: Added by Dongyuxi 00209188, 2013/3/22 */
    EN_NV_ID_LVRAMP_PARA                            = 0xD4FC,
    EN_NV_ID_FE_NOTCH_INFO                          = 0xD4FF, /*notch*/
    EN_NV_ID_FE_BASIC_INFO                          = 0xD500,
    EN_NV_ID_FE_RFIC_INIT                           = 0xD501,
    EN_NV_ID_FE_COMM_CONFIG                         = 0xD502,
    EN_NV_ID_PBAND_INFO                             = 0xD503,
    EN_NV_ID_SBAND_INFO                             = 0xD504,
    EN_NV_ID_PBAND_MIPI_INFO                        = 0xD505,
    /* END:   Added by Dongyuxi 00209188, 2013/3/22 */
    EN_NV_ID_CA_TUNER_INFO                          = 0xD506,

    EN_NV_ID_APC_GAIN_DEFALUT                       = 0xD507,
    EN_NV_ID_PA_POWER_DIFFERENCE_DEFALUT            = 0xD508,
    EN_NV_ID_DSP_NV_PARA_SIZE                       = 0xD509,
    EN_NV_ID_MIPIDEV_INIT                           = 0xD50A,
    EN_NV_ID_LPHY_ET_COMM_PARA                   = 0xD50B,
    EN_NV_ID_LPHY_ET_VENDOR_PARA_TI              = 0xD50C,
    EN_NV_ID_LPHY_ET_VENDOR_PARA_R2              = 0xD50D,
    EN_NV_ID_LPHY_ET_VENDOR_PARA_QU              = 0xD50E,
    EN_NV_ID_RF_CA_RCCAL_CFG                      = 0xD50F,

    /*begin added by yubo00271905 2014/06/23*/
	EN_NV_ID_LPHY_LWCOEX_INIT_PARA				 	= 0xD510,
	/*end added by yubo00271905 2014/06/23*/


    EN_NV_ID_ASU_PARA                           = 0xD511,
    EN_NV_SINGLE_XO_DEFINE                      = 0xD51d,
#ifdef FEATURE_TLPHY_SINGLE_XO
    EN_NV_ID_TL_COMM_NV_PARA_SIZE                = 0xD512,
    EN_NV_ID_DCXO_C_TRIM_DEFAULT                 = 0xD513,
    EN_NV_ID_DCXO_C2_FIX_DEFAULT                 = 0xD514,
    EN_NV_ID_XO_INIT_FREQUENCY                   = 0xD515,
    EN_NV_ID_DCXO_C_TRIM                         = 0xD516,
    EN_NV_ID_DCXO_C2_FIX                         = 0xD517,
    EN_NV_ID_DCXO_TEMP_COMP_POLY_COEF            = 0xD518,
    EN_NV_ID_DCXO_TEMP_COMP_THRESHOLD            = 0xD519,
    EN_NV_ID_DCXO_FREQ_VS_TEMP_ARRAY             = 0xD51a,
    EN_NV_ID_DCXO_TEMP_READ_PERIOD               = 0xD51b,
    EN_NV_ID_DCXO_TEMP_COMP_POLY_ALL             = 0xD51c,
#endif

#if 0
    EN_NV_ID_V9R1RFBOARD_FE_BASIC_INFO              = 0xD5A0,
    EN_NV_ID_RF_BOARD_TYPE                          = 0xD5A1,
    EN_NV_ID_V9R1RFBOARD_FE_COMM_CONFIG           = 0xD5A2,
    EN_NV_ID_V9R1RFBOARD_PBAND_INFO               = 0xD5A3,
    EN_NV_ID_V9R1RFBOARD_SBAND_INFO              = 0xD5A4,
    EN_NV_ID_V9R1RFBOARD_PBAND_MIPI_INFO         = 0xD5A5,
    EN_NV_ID_V9R1RFBOARD_CA_TUNER_INFO          = 0xD5A6,
#endif
    EN_NV_ID_HI6360_AGC_PARA_B20                    = 0xD618,
    EN_NV_ID_AD9361_AGC_PARA_B20                    = 0xD619,
    EN_NV_ID_LPHY_AGC_BASE_TABLE_B20                = 0xD61A,

    EN_NV_ID_HI6360_AGC_PARA_B41                    = 0xD6d8,
    EN_NV_ID_AD9361_AGC_PARA_B41                    = 0xD6d9,
    EN_NV_ID_LPHY_AGC_BASE_TABLE_B41                = 0xD6da,

    EN_NV_ID_HI6360_AGC_PARA_B40                    = 0xD658,
    EN_NV_ID_AD9361_AGC_PARA_B40                    = 0xD659,
    EN_NV_ID_LPHY_AGC_BASE_TABLE_B40                = 0xD65A,

    EN_NV_ID_HI6360_AGC_PARA_B38                    = 0xD698,
    EN_NV_ID_AD9361_AGC_PARA_B38                    = 0xD699,
    EN_NV_ID_LPHY_AGC_BASE_TABLE_B38                = 0xD69A,

    EN_NV_ID_HI6360_AGC_PARA_B7                     = 0xD718,
    EN_NV_ID_AD9361_AGC_PARA_B7                     = 0xD719,
    EN_NV_ID_LPHY_AGC_BASE_TABLE_B7                 = 0xD71A,

    EN_NV_ID_HI6360_AGC_PARA_B3                     = 0xD817,
    EN_NV_ID_HI6360_AGC_PARA_B1                     = 0xD858,
    EN_NV_ID_HI6360_AGC_PARA_B5                     = 0xD898,
    EN_NV_ID_HI6360_AGC_PARA_B8                     = 0xD8d8,
    EN_NV_ID_HI6360_AGC_PARA_B28                    = 0xDf58,/**/
    EN_NV_ID_HI6360_AGC_PARA_B128                   = 0xDf98,

    EN_NV_ID_LPHY_DSP_VERSION_INFO                  = 0xD818,
    EN_NV_ID_LPHY_DSP_CONFIG_INFO                   = 0xD819,
    EN_NV_ID_MULTIMODE_DSP_COMMON_CONFIG_INFO       = 0xD81A,

    EN_NV_RX_BT_LEVEL_MAP_TABLE                     = 0xD3e1,








    EN_NV_ID_FACTORY_END                            = 0x2fff
};
typedef unsigned long NV_ITEM_ID_ENUM_UINT32;





/*****************************************************************************
   5 STRUCT
*****************************************************************************/




/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/









/* this file is included by drv, submit by zhouluojun, reviewed by hujianbo */
/*
#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif
*/



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of PsNvInterface.h */

