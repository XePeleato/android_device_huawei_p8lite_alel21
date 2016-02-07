

#ifndef __LPSNVINTERFACE_H__
#define __LPSNVINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#include  "PsTypeDef.h"
#include  "LNvCommon.h"
#include  "RrcNvInterface.h"
#include  "LNasNvInterface.h"
#include  "msp_nvim.h"
#if (VOS_OS_VER != VOS_WIN32)
#include  "msp.h"
#endif
#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
/* NV???????? */
/*
ERR_MSP_NO_INITILIZATION   3              ģ????ʼ??ʧ?ܣ??޷?ִ?нӿڲ???
ERR_MSP_NO_INITILIZATION   502            ????????
ERR_MSP_NV_NOT_SUPPORT_ID  520            ??ID??????
ERR_MSP_NV_ERROR_READ      521            ??flashʧ??
ERR_MSP_NV_ITEM_IS_EMPTY   1100           ???????ݳ???Ϊ??
*/
#if (VOS_OS_VER != VOS_WIN32)
#define LPs_NvimItem_Read( ulId, pItemData, usItemDataLen ) \
        NVM_Read( ulId, (VOS_VOID *)pItemData, usItemDataLen)

/*
ERR_MSP_NO_INITILIZATION             3              ģ????ʼ??ʧ?ܣ??޷?ִ?нӿڲ???
ERR_MSP_NO_INITILIZATION             502            ????????
ERR_MSP_NV_NOT_SUPPORT_ID            520            ??ID?????ڣ?????Ҫ???ýӿ??в?֧??????ID?Ĳ?????
ERR_MSP_NV_NOT_SUPPORT_LENTH         522            ???Ȳ?֧??
ERR_MSP_NV_ERROR_WRITE               524            дflashʧ??
ERR_MSP_NV_FLASH_FULL                1103           flash?ռ䱻д??
ERR_MSP_NVIM_NOT_SUPPORT_WRITE       2007           ϵͳģʽ???????޸ĸ???
*/
#define LPs_NvimItem_Write( ulId, pItemData, usItemDataLen)\
       NVM_Write( ulId, (VOS_VOID *)pItemData, usItemDataLen)
#else
#define LPs_NvimItem_Read( ulId, pItemData, usItemDataLen ) \
        NVM_Read_Stub( ulId, (VOS_VOID *)pItemData, usItemDataLen)

#define LPs_NvimItem_Write( ulId, pItemData, usItemDataLen)\
       NVM_Write_Stub( ulId, (VOS_VOID *)pItemData, usItemDataLen)
#endif

#define LPs_FactNvim_Write(ulId, pItemData, usDataLen)\
        NVIM_WriteFactoryForce(ulId, (VOS_VOID *)pItemData, usDataLen)

/* GCF??????,λ?????? */
#define LPS_NV_GET_TAU_CIPHERED_BIT         ( 0x00000001 )
#define LPS_NV_LOCK_BAND_FREQ_CELL_IN_TEST_MODE_FUNC_BIT     (0x00000001)

/*h00159435 TdsDataInfoNotify Begin*/
#define LPS_NV_DL_CTRL_PDU_NOTIFY_BIT          (0x00000002)
/*h00159435 TdsDataInfoNotify End*/

/*added by lishangfeng 20130320 begin*/
#define LPS_NV_SEARCHING_OPTIMIZE_FUN_BIT    (0x00000004)
/*added by lishangfeng 20130320 end*/

#define LPS_NV_GET_MSG4_OLDCR_BIT           ( 0x00000002 )

/* GCF?Ƿ?֧??24.301 Table D.1.1?н?��ԭ?????ã?λ?򿪹? */
#define LPS_NV_GET_EST_CAUSE_BIT             ( 0x00000004 )

/* ָʾ??ȫ?????Ƿ?֧?ֲ???ģʽ,λ?????? */
#define LPS_NV_GET_SUPPORT_SMC_TEST_MODE_BIT  ( 0x00000008 )

/* GCF??׮ͨ??CDRX??????????ʵ?ʳ???Ϊ?˽????Ĳ??򿪸ÿ??أ?λ?򿪹? */
#define LPS_NV_GET_CDRX_L2_TIMER_BIT         ( 0x00000010 )

/* PS???ܿ?????,λ?????? */
#define LPS_NV_GET_SUPPORT_BAND64_BIT       ( 0x00000001 )

/* PS???ܿ?????,ɾ??SDF?Ƿ?Я??Qos λ?? */
/* ɾ??SDFʱ?Ƿ?Я??Qos??Ϣ???ܿ??أ?0Ϊ?رգ?1Ϊ?򿪡?Ĭ??Ϊ?ر? */
#define LPS_NV_GET_DEL_SDF_WITH_QOS_BIT       ( 0x00000002 )

/* ?Ƿ?????��??̬?յ???ue??ҵ??Ѱ?????????ͷ?, ȱʡ0 ??ʾ?????ͷ?,  4??ʾ???????ͷ? */
#define LPS_NV_GET_CONN_RCV_PAGING_TO_RELASE_BIT       ( 0x00000004 )

/* PS???ܿ?????,?Ƿ?֧?ְ?ȫ??��?Ƚ?λ?? */
#define LPS_NV_GET_SECU_CAP_MACTCH_BIT         ( 0x00000008 )

/* PS???ܿ?????,?Ƿ?֧?ְ?ȫ??????´??????ľܾ?NAS??Ϣ??ʶ */
#define LPS_NV_GET_NAS_PLAIN_REJ_MSG_FLAG_BIT  (0x00001000)

/* PS???ܿ?????,????��??CSFB???˺?????BUG??NV???ر?ʶ */
#define LPS_NV_GET_NAS_CSFB_TAU_TYPE2_FLAG_BIT  (0x00004000)

/* PS???ܿ?????,?Ƿ?֧??????KDF?㷨λ?? */
#define LPS_NV_GET_HARD_KDF_BIT         ( 0x00000010 )
#define LPS_NV_GET_HARD_KDF_HYBRI_BIT   ( 0x00000020 )
#define LPS_NV_GET_ULDRB_DISCARD_BIT    ( 0x00000040 )

/* ??????ʱ???Ż????أ? 0x00000080??ʾ?򿪣?0??ʾ?ر? */
#define PS_NV_TIME_DELAY_SWITCH_BIT     ( 0x00000080 )

/* ??????ʱ???Ż????أ? 0x00000100??ʾ?򿪣?0??ʾ?ر? */
#define PS_NV_TIME_DELAY_HOOK_SWITCH_BIT     ( 0x00000100 )

/* MBMS?????? */
#define LPS_NV_GET_MBMS_SUPPORT_BIT         ( 0x00000200 )

/* ????Bandwidth???󿪹? */
#define LPS_NV_GET_BANDWIDTH_SUPPORT_BIT         ( 0x00000400 )

/* modify by lishangfeng freq all begin */
#define LPS_NV_GET_FREQ_IN_ALL_BANDWITH_SUPPORT_BIT ( 0x00000800 )
/* modify by lishangfeng freq all end */
/* ?ؽ????? */
#define LPS_NV_GET_REST_STUB_SUPPORT_BIT         ( 0x00001000 )

/* sunjitan DCM???????? GU??L???л????жϱ???TA???? */
#define LPS_NV_GET_GU2L_HO_IGNORE_FORBID_TA_BIT     ( 0x00002000 )

/* gaojishan-ETWS-Begin */
#define LPS_NV_ETWS_SUPPORT_BIT         ( 0x00004000 )
#define LPS_NV_CMAS_SUPPORT_BIT         ( 0x00008000 )
/* gaojishan-ETWS-End */
/* gaojishan-SYSCFG-SAVENV-Begin */
#define LPS_NV_GET_JP_UBand1_Record_SUPPORT_BIT         ( 0x00010000 )
/* gaojishan-SYSCFG-SAVENV-End */

/* L?ؽ?UTRA-FDD???????? */
#define LPS_NV_GET_L_REEST_CELL_SRCH_UTRA_FDD_BIT         ( 0x00020000 )

/* L?ؽ?GSM???????? */
#define LPS_NV_GET_L_REEST_CELL_SRCH_GSM_BIT         ( 0x00040000 )

/* С????????С?�����???޷????ջ??߲??ֽ??յ?ϵͳ??Ϣʱ,?????жϷ???ҪbarС?? */
#define LPS_NV_GET_CELL_SRCH_CELL_BAR_BIT         ( 0x00080000 )

/* gaojishan-nccpermit */
#define LPS_NV_NCC_PERMIT_BIT         ( 0x00100000 )

/* bgs begin */
/* L?????ѹ????Ƿ?֧?ֿ??? */
#define LPS_NV_GET_L_BGS_SUPPORT_BIT                 ( 0x00200000 )
/* bgs end */

/*y00151394,tcp????*/
#define LPS_NV_TCP_OPT_BIT            ( 0x00800000 )


/* gaojishan-reest-measGap*/
#define LPS_NV_GET_REEST_MEAS_GAP_SETUP_BIT            ( 0x00400000 )

/* gaojishan-SoftBank-???Ʊ?־:true,set Band41 range */
#define LPS_NV_SOFT_BANK_FLAG_BIT            ( 0x01000000 )

/* gaojishan-nccpermit-2 */
#define LPS_NV_NO_IDLE_NCC_PERMIT_BIT        ( 0x02000000 )

/* gaojishan-dcom-resel-cfg */
#define LPS_NV_JP_DCOM_USE_R8_BIT        ( 0x04000000 )

/* gaojishan-dcom-resel-cfg */
#define LPS_NV_JP_DCOM_CON_TO_IDLE_BIT        ( 0x08000000 )

/* gaojishan-dcom-resel-cfg */
#define LPS_NV_JP_DCOM_REL_OFFSET_BIT        ( 0x10000000 )
#define LPS_NV_JP_DCOM_REL_OFFSET_DEBUG_BIT  ( 0x20000000 )
/* gaojishan-fast-dorm-cfg */
#define LPS_NV_JP_DCOM_FAST_DORM_BIT  ( 0x40000000 )

/* add by lishangfeng for RRM 5.2.1 test begin */
#define LPS_NV_RRM_LTE_521_BIT  ( 0x80000000 )
/* add by lishangfeng for RRM 5.2.1 test end */

/* gaojishan-CMAS-Begin */
#define LPS_NV_CMAS_REPT_BEFORE_CAMP_SUCC_BIT    (0x00000008 )
/* gaojishan-CMAS-End */

/* v7r2 ca begin */
/* CA?????Ƿ??򿪣?1Ϊ?򿪣?0Ϊ?ر? */
#define LPS_NV_CA_SUPPORT_BIT  ( 0x00000001 )
/* v7r2 ca end */

/* add for LTE NetScan begin */
/* stPsFunFlag02????????Ϊ1 */
/* NetScan?????Ƿ??򿪣?1Ϊ?򿪣?0Ϊ?ر? */
#define LPS_NV_NETSCAN_SUPPORT_BIT    (0x00000002)
/* add for LTE NetScan end */

/* lwurc Ƶ??????ָʾ???? */
#define LPS_NV_LWURC_BAND_LIMIT_FLG_BIT            (0x00000020)

/* ?����????ѣ?????ֻ????L??ģ?????????Ϻ??ظ?LMM???????�����?????? GU ģ */
#define LPS_NV_GET_L_BGS_L_SUPPORT_BIT            (0x00000040)

/* modify by lishangfeng 20131212 begin */
#define LPS_NV_SND_ETWS_IMIDIEATELY_BIT           (0x00000080)                  /* 0x80??ʾΪ��?????ͣ?0x00??ʾ???????ٷ??? */
/* modify by lishangfeng 20131212 end */

#define LPS_NV_L2_TLEVENT_REPORT_BIT           (0x00000100)                  /* TL L2?¼??ϱ?*/
#define LPS_NV_L2_LTEPRINT_REPORT_BIT           (0x00000200)                  /* LTE L2 ??ӡ??Ϣ?ϱ?*/
#define LPS_NV_RRC_LIST_SEARCH_TWICE_BIT           (0x00000400)                  /* LTE ????LIST????????Ƶ??????��?ο???*/
#define LPS_NV_RRC_BACKTOLTE_REDRECTFAIL_BIT        (0x00000800)
#define LPS_NV_L2_CACELLINFO_REPORT_BIT          (0x00002000)                  /*CA SCELL״̬??Ϣ?ϱ?*/

#define LPS_NV_GET_SSC_SUPPORT_BIT                (0x00000001)
#define LPS_NV_GET_ADRX_SUPPORT_BIT               (0x00000002)

/*TDS DSP ?Լ???ַ*/
#define ADDR_SELF_ADJUST_PARA       (0x1300F840)


/* GCF?????? */
#define  LPS_NV_GET_GCF_FLAG(ucIndex, ulFlag) \
         (g_aulGcfFlg[ucIndex]&(ulFlag))

/* PS???ܿ????? */
#define  LPS_NV_GET_PS_FUN_FLAG(ucIndex, ulFlag) \
         (g_aulPsFunFlg[ucIndex]&(ulFlag))

#define  LPS_NV_SUPPORT_BAND64                1     /* ֧??Band64Ƶ?? */
#define  LPS_NV_NOT_SUPPORT_BAND64            0     /* ??֧??Band64Ƶ?? */
#define LPS_NV_CONN_RCV_PAGING_RELASE  0          /* ?յ?ҵ??Ѱ???????ͷ? */
#define LPS_NV_CONN_RCV_PAGING_NO_RELASE  LPS_NV_GET_CONN_RCV_PAGING_TO_RELASE_BIT   /* ?յ?ҵ??Ѱ?����????ͷ? */

#define LPS_NV_GET_HARD_KDF_OFF        0                        /* ??֧??Ӳ??KDF?㷨 */
#define LPS_NV_GET_HARD_KDF_ON         LPS_NV_GET_HARD_KDF_BIT  /* ֧??Ӳ??KDF?㷨 */


#define LPS_NV_GET_HARD_KDF_HYBRI_OFF        0                              /* ??ʹ??��?λ??ϼ?????KDF?㷨 */
#define LPS_NV_GET_HARD_KDF_HYBRI_ON         LPS_NV_GET_HARD_KDF_HYBRI_BIT  /* ʹ??��?λ??ϼ?????KDF?㷨 */


/* PS???ܿ??غ궨?? */
#define LPS_FUNC_SW_ON                       (1)
#define LPS_FUNC_SW_OFF                      (0)

/*??ӡȫ?ֱ?��??????????*/
#define MAX_GLOBAL_PRANT_NUM   5

/* TPS ?????? */
#define  TPS_NV_GET_FLAG(uVlaue, ulFlag) \
         (uVlaue&(ulFlag))
#define TPS_NV_OP_HL1_SCHEDULE_IND_FLITER_BIT            ( 0x00000001 )
#define TPS_NV_OP_T_RRC_trafficMeasPeriod_FLITER_BIT            ( 0x00000002 )
#define TPS_NV_Print_FLITER_BIT            ( 0x00000004 )
/*CELLUPDATE MODEFIED BEGIN*/
#define TPS_NV_OP_Cellupdate_stub_BIT            ( 0x00000001 )
/*CELLUPDATE MODEFIED END*/
#define TPS_NV_OP_ADRX_GET_BIT            ( 0x00000100 )

/*lilong00280875 DMO begin*/
/*bitFlag10*/
#define  TPS_NV_OP_DMO_Message_BIT           ( 0x00000200)
/*lilong00280875 DMO end*/

#define TPS_NV_SMC_STUB_BIT                 ( 0x00000400 )

#define TPS_NV_OP_NEWKEY_GET_BIT            ( 0x00000008 )

#define TPS_NV_OP_TCP_ACK_DIS_GET_BIT       ( 0x00000010 )


/*DETECT CELL MODIFIY BEGIN*/
#define TPS_NV_OP_DETECT_CELL_GET_BIT       (0x00000020)
/*DETECT CELL MODIFIY END*/

#define TPS_NV_OP_MEASCONTROL_GET_BIT       (0x00000040)

#define TPS_NV_OP_RlcUmDataInd_GET_BIT       (0x00000080)


#define TPS_NV_OP_Debug_Info_BIT            ( 0x00000002 )

#define TPS_NV_OP_Debug_State_BIT            ( 0x00000004 )


#define TPS_NV_GCF_CS_LOOP_BIT            ( 0x00000001 )

#define TPS_NV_GCF_DRXEXTRA_CONTROL_BIT            ( 0x00000002 )

/*׼FR????begin*/
#define TPS_NV_OP_GSM_FR_BIT           ( 0x00000004 )
/*׼FR????end*/
/*TG ??ѡ?Ż?begin*/
#define TPS_NV_GCF_SIRAT_BIT           ( 0x00000008 )
#define TPS_NV_GCF_QRXLEVMIN_BIT        (0x00000010)
#define TPS_NV_GCF_S_UNSATISFIED_BIT    (0x00000020)
#define TPS_NV_T2G_IRAT_REPORT_BIT      (0x00000040)
/*TG ??ѡ?Ż?end*/

#define TPS_NV_GCF_RRM_STUB_BIT         (0x00000080)

/* LTE PS Transfer begin */
#define LRRC_MTC_THRESHOLD_MIN_VALUE      (0)
#define LRRC_MTC_THRESHOLD_MAX_VALUE      (31 * 2 * 8)
/* LTE PS Transfer end */
/* niuxiufan taiwan modify begin */
#define LRRC_IRAT_REDIR_NOT_BANDSCAN_BIT (0x00000008)
/* niuxiufan taiwan modify end */

#define TPS_NV_OP_OCCUPY_GET_BIT            ( 0x00000001 )
#define TPS_NV_OP_SIGNAL_REL_GET_BIT        ( 0x00000002 )


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
/*????ö??*/
enum RRC_NV_SWITCH_ENUM
{
    RRC_SWITCH_OFF = 0,
    RRC_SWITCH_ON  = 1
};

typedef VOS_UINT32   RRC_NV_SWITCH_ENUM_UINT32;

/* sib??ʱ??ѡ?񿪹?ö??*/
enum RRC_NV_TIMER_CHOICE_ENUM
{
    RRC_TIMER_CHOICE_DEFAULT = 0,           /* Ĭ??ѡ?? */
    RRC_TIMER_CHOICE_DT  = 1,               /* ?µ?????ѡ?? */
    RRC_TIMER_CHOICE_BUTT
};
typedef VOS_UINT32   RRC_NV_TIMER_CHOICE_ENUM_UINT32;

enum RRC_PHY_LTE_RF_HW_ENUM
{
    RRC_PHY_LTE_RF_HW_BAND7_38_40         = 0,  /* Band7/38/40??Ƶ???۰?*/
    RRC_PHY_LTE_RF_HW_BAND20,                   /* Band20??Ƶ???۰?*/
    RRC_PHY_LTE_RF_HW_USB_STICK_VB,             /* USB STICK??vbӲ?????? */
    RRC_PHY_LTE_RF_HW_SOFTBANK_BAND64,          /* ????ʹ?õ?Band64?۰? */
    RRC_PHY_LTE_RF_HW_USB_STICK_VC,             /* USB STICK??vCӲ?????? */
    RRC_PHY_LTE_RF_HW_USB_STICK_VD,             /* USB STICK??vDӲ?????? */
    RRC_PHY_LTE_RF_HW_USB_STICK_VE,             /* USB STICK??vEӲ?????? */
    RRC_PHY_LTE_RF_HW_USB_STICK_VF,             /* USB STICK??vFӲ?????? */
    RRC_PHY_LTE_RF_HW_USB_STICK_VG,             /* USB STICK??vGӲ?????? */
    RRC_PHY_LTE_RF_HW_USB_STICK_VH,             /* USB STICK??vHӲ?????? */
    RRC_PHY_LTE_RF_HW_BUTT
};
typedef VOS_UINT8 RRC_PHY_LTE_RF_HW_ENUM_UINT8;
/*NV??ö??*/
/* gaojishan-PLMN_EXACTLY_COMPARE_FLG */
#ifdef PS_ITT_PC_TEST2
#define en_NV_Item_PLMN_EXACTLY_COMPARE_FLG   (9106)
#endif
/*****************************************************************************
 ö????    : PS_NV_INDEX_ENUM
 ö??˵??  : ??????????ͬband?µ???ͬ??NV??
*****************************************************************************/
enum PS_NV_INDEX_ENUM
{
    NV_IDX_AGC_PARA                     = 0 ,

    NV_IDX_MODEM_LOSS,
    NV_IDX_TEMP_SENSOR,
    NV_IDX_RX_CAL_FREQ_LIST,
    NV_IDX_RX_NOBLK_AGC_FREQ_COMP_ANT1,
    NV_IDX_RX_NOBLK_AGC_FREQ_COMP_ANT2,
    NV_IDX_RX_BLK_AGC_FREQ_COMP_ANT1,
    NV_IDX_RX_BLK_AGC_FREQ_COMP_ANT2,
    NV_IDX_RX_TEMP_COMP,
    NV_IDX_RX_IP2_CAL_FREQ_LIST,
    NV_IDX_RX_IP2_CAL_THRESHOLD,
    /*NV_IDX_RX_IP2_CAL,*/
    NV_IDX_TX_APC_GAIN_THRESHOLD,
    /*NV_IDX_APC_ATT_TABLE_STRU,*/
    NV_IDX_TX_CAL_FREQ_LIST,
    NV_IDX_TX_PA_POWER,
    NV_IDX_TX_CAL_HIGHGAIN_POWER,
    NV_IDX_TX_CAL_MIDGAIN_POWER,
    NV_IDX_TX_CAL_LOWGAIN_POWER,
    NV_IDX_TX_APC_HIGHGAIN_FREQ_COMP,
    NV_IDX_TX_APC_MIDGAIN_FREQ_COMP,
    NV_IDX_TX_APC_LOWGAIN_FREQ_COMP,
    NV_IDX_TX_APC_TEMP_COMP,
    NV_IDX_TX_APT_PARA,
    NV_IDX_TX_APT_PDM_HIGH_TBL,
    NV_IDX_TX_APT_PDM_MID_TBL,
    NV_IDX_TX_APT_PDM_LOW_TBL,
    NV_IDX_TX_APT_PDM_MC_COMP_HIGH_TBL,
    NV_IDX_TX_APT_PDM_MC_COMP_MID_TBL,
    NV_IDX_TX_APT_PDM_MC_COMP_LOW_TBL,
    /* add by lishangfneg mpr begin */
    NV_IDX_TX_MPR,
    /* add by lishangfneg mpr end */
    NV_IDX_ANT_SAR,
    /*add by chenhongqiang begin*/
    NV_IDX_LTE_TX_ATTEN,
    /*add by chenhongqiang end*/

    NV_IDX_TX_AMPR,

    NV_IDX_LTE_TX_DIVERSITY_LOSS_COMP,

    EN_NV_ID_LTE_TX_AMPR_NS05,
    EN_NV_ID_LTE_TX_AMPR_NS09,
    /*add for V9R1_6361 Begin*/
    NV_IDX_IIP2_CAL_TABLE,
    NV_IDX_RF_DCOC_CAL_ANT1,
    NV_IDX_RF_DCOC_CAL_ANT2,
    NV_IDX_TX_APC_GAIN,
    NV_IDX_RF_TXIQ_CAL,
    NV_IDX_TX_RF_BB_ATT,
    NV_IDX_TX_RF_BIAS,
    NV_IDX_PA_POWER_DIFFERENCE,
    NV_IDX_TX_RF_FREQ_COMP_STRU,
    NV_IDX_TX_PA_TEMP_COMP,
    NV_IDX_TX_ATTEN_TABLE,
    NV_IDX_POWERDET_VOLTAGE,
    /*add for V9R1_6361 End*/
    NV_IDX_TX_ET_BAND_PARA,
    NV_IDX_ET_LUT_TBL_LOWCH,
    NV_IDX_ET_LUT_TBL_MIDCH,
    NV_IDX_ET_LUT_TBL_HIGHCH,
    NV_IDX_ET_GAIN_COMP,
    NV_IDX_TX_ET_FINEDLY_PARA,
    NV_IDX_TX_ET_VOFFSET_GAIN,
    NV_IDX_TX_ET_EVDELY,
    NV_IDX_RF_CA_RCCODE_STRU,
    NV_IDX_LTE_TX_UL_ONE_RB_MPR,

    NV_IDX_LTE_TX_PD_PARA,
    NV_IDX_LTE_TX_PD_AUTO_CAL,
    NV_IDX_LTE_TX_PD_PWR_TABLE,
    NV_IDX_LTE_TX_PD_VOLT_TABLE,
    NV_IDX_LTE_TX_PD_TEMPCMP_TABLE,

    NV_IDX_RX_NOBLK_AGC_SCC_FREQ_COMP_ANT1,
    NV_IDX_RX_NOBLK_AGC_SCC_FREQ_COMP_ANT2,
    NV_IDX_RX_BLK_AGC_SCC_FREQ_COMP_ANT1,
    NV_IDX_RX_BLK_AGC_SCC_FREQ_COMP_ANT2,
    EN_IDX_NV_ID_FTM_CAND_CELL_LIST,

    PS_NV_IDX_BUTT
};
typedef VOS_UINT32 PS_NV_INDEX_ENUM_UINT32;

/*****************************************************************************
 ö????    : PS_BAND_ENUM
 ö??˵??  : ??????????ͬband??NV?б?
*****************************************************************************/
enum PS_BAND_ENUM
{
    PS_BAND_7,
    PS_BAND_38,
    PS_BAND_20,
    PS_BAND_40,
    PS_BAND_41,
    PS_BAND_3,
    PS_BAND_1,
    PS_BAND_5,
    PS_BAND_8,
    PS_BAND_39,

    PS_BAND_19,
    PS_BAND_21,

    PS_BAND_2,
    PS_BAND_4,
    PS_BAND_6,
    PS_BAND_9,
    PS_BAND_10,
    PS_BAND_11,
    PS_BAND_12,
    PS_BAND_13,
    PS_BAND_14,
    PS_BAND_17,

    PS_BAND_18,
    PS_BAND_22,
    PS_BAND_23,
    PS_BAND_24,
    PS_BAND_25,
    PS_BAND_33,
    PS_BAND_34,
    PS_BAND_35,
    PS_BAND_36,
    PS_BAND_37,

    PS_BAND_42,
    PS_BAND_43,
	PS_BAND_28,
    /* ?Ǳ?Ƶ??begin */
    //PS_BAND_101,
    PS_BAND_128,
    PS_BAND_103,
    PS_BAND_104,
    PS_BAND_105,
    PS_BAND_106,
    PS_BAND_107,
    PS_BAND_108,
    PS_BAND_109,
    PS_BAND_110,
    PS_BAND_111,
    PS_BAND_112,
    PS_BAND_113,
    PS_BAND_114,
    PS_BAND_115,
    PS_BAND_116,
    /* ?Ǳ?Ƶ??end */

    PS_BAND_BUTT
};
typedef VOS_UINT8 PS_BAND_ENUM_UINT8;

/*****************************************************************************
 ?ṹ??    : LPS_NV_FLOW_CONTROL
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : Э??ջ???ؽṹ??
*****************************************************************************/
typedef struct
{
    /* ?????ܿ??? */
    VOS_UINT32                          ulFlowCtrlFlag;
    /*ulFcInitFlag???س?ʼ??ѡ?? 0-??RRC??̬ѡ????1-??????NV?????п???*/
    VOS_UINT32                          ulFcInitFlag;
    VOS_UINT32                          ulFcPdcpTarget;
    VOS_UINT32                          ulTargetIPMax;
    VOS_UINT32                          ulSendSduMax;
    VOS_UINT32                          ulDlSduMax;
    VOS_UINT32                          ulULIpMax;
    VOS_UINT32                          ulMeasGap;
    VOS_UINT32                          ulSduGap;
}LPS_NV_FLOW_CONTROL;

/*****************************************************************************
 ?ṹ??    : LPS_NV_GLOBAL_PRINT
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : Э??ջ??ӡȫ?ֱ?��?ṹ??
*****************************************************************************/
typedef struct
{
    /* ??ӡ?ܿ??? 0:?رմ?ӡȫ?ֱ?��??1: ??ӡһ????2:??ӡ��?????��? 3 4 5 */
    VOS_UINT32                          ulGlobalPrintFlag;
    VOS_UINT32                          aulPara[MAX_GLOBAL_PRANT_NUM];
}LPS_NV_GLOBAL_PRINT_STRU;

/*****************************************************************************
 ?ṹ??    : LPS_NV_MEAS_THRESHOLD_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8 ucSynsSwitch;     /*?Ƿ??����?��ֵ???׷????ؽ????? */
    VOS_UINT8 ucRsrpLowCount;   /* PHY?ϱ??Ĳ?��ֵС??sRsrpHoldThreshold?ﵽ��?Ρ??????ؽ????? */
    VOS_INT16 sRsrpHoldThreshold;  /* PHY?ϱ??Ĳ?��ֵС??ĳֵ ucRsrpLowCount++*/
}LPS_NV_MEAS_THRESHOLD_STRU;


/* 2655 begin */
 /*****************************************************************************
 ?ṹ??    : LPS_NV_LONG_BIT_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : Э??ջ?ĺ?Э?鹦?????صĿ??صĽṹ??
*****************************************************************************/
typedef struct
{
    VOS_UINT32  bitFlag01                   :1;
    VOS_UINT32  bitFlag02                   :1;
    VOS_UINT32  bitFlag03                   :1;
    VOS_UINT32  bitFlag04                   :1;
    VOS_UINT32  bitFlag05                   :1;
    VOS_UINT32  bitFlag06                   :1;
    VOS_UINT32  bitFlag07                   :1;
    VOS_UINT32  bitFlag08                   :1;
    VOS_UINT32  bitFlag09                   :1;
    VOS_UINT32  bitFlag10                   :1;
    VOS_UINT32  bitFlag11                   :1;
    VOS_UINT32  bitFlag12                   :1;
    VOS_UINT32  bitFlag13                   :1;
    VOS_UINT32  bitFlag14                   :1;
    VOS_UINT32  bitFlag15                   :1;
    VOS_UINT32  bitFlag16                   :1;
    VOS_UINT32  bitFlag17                   :1;
    VOS_UINT32  bitFlag18                   :1;
    VOS_UINT32  bitFlag19                   :1;
    VOS_UINT32  bitFlag20                   :1;
    VOS_UINT32  bitFlag21                   :1;
    VOS_UINT32  bitFlag22                   :1;
    VOS_UINT32  bitFlag23                   :1;
    VOS_UINT32  bitFlag24                   :1;
    VOS_UINT32  bitFlag25                   :1;
    VOS_UINT32  bitFlag26                   :1;
    VOS_UINT32  bitFlag27                   :1;
    VOS_UINT32  bitFlag28                   :1;
    VOS_UINT32  bitFlag29                   :1;
    VOS_UINT32  bitFlag30                   :1;
    VOS_UINT32  bitFlag31                   :1;
    VOS_UINT32  bitFlag32                   :1;
}LPS_NV_LONG_BIT_STRU;
/*****************************************************************************
 ?ṹ??    : LPS_NV_TL_L2_PARA_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : L2???????صĿ??صĽṹ??
*****************************************************************************/
typedef struct
{
    /* ????RLC ???????????????? ??NVĬ??Ϊ45*/
    VOS_UINT32                          ulRlcUlSduLimit;

    LPS_NV_LONG_BIT_STRU                stL2Flag;

    /************************stL2Flag????BIT????***************************
     bitFlag01: ????ץȡL2 MAC PDU?еĲ??? g_ulIfForwardToPc, Ĭ??Ϊ??
     bitFlag02: ????ץȡL2 MAC PDU?еĲ??? g_ulCapPusch, Ĭ??Ϊ??
     bitFlag03: ????ץȡL2 MAC PDU?еĲ??? g_ulCapPdsch, Ĭ??Ϊ??
     bitFlag04: ????ץȡL2 MAC PDU?еĲ??? g_ulCapPdcp, Ĭ??Ϊ??
     bitFlag05: ????ץȡL2 MAC PDU?еĲ??? g_ulTcpDebugFlg, Ĭ??Ϊ??
     bitFlag06: ????ץȡL2 MAC PDU?еĲ??? g_ulTcpDlFlg, Ĭ??Ϊ??
     bitFlag07: ????ץȡL2 MAC PDU?еĲ??? g_g_ulTcpUlFlg, Ĭ??Ϊ??
     bitFlag08: ????ץȡL2 MAC PDU?еĲ??? g_ulTcpCtrlPduFlg, Ĭ??Ϊ??
     bitFlag09:
     bitFlag10:
     bitFlag11:
     bitFlag12:
     bitFlag13:
     bitFlag14:
     bitFlag15:
     bitFlag16:
     bitFlag17:
     bitFlag18:
     bitFlag19:
     bitFlag20:
     bitFlag21:
     bitFlag22:
     bitFlag23:
     bitFlag24:
     bitFlag25:
     bitFlag26:
     bitFlag27:
     bitFlag28:
     bitFlag29:
     bitFlag30:
     bitFlag31:
     bitFlag32:
    ****************************************************************************/

    LPS_NV_LONG_BIT_STRU                stL2Flag1;

    /*Ԥ??*/
    /*?????еĵ?һ??Ԫ??Ϊ?????޵?????????ӦIPV6ǰ׺???⣬ʹ??NV???Ƶ?ȫ?ֱ?��??IPV6ǰ׺Ĭ??UEֻ??1??*/
    VOS_UINT32                          ulRev[32];

}LPS_NV_TL_L2_PARA_STRU;
/*****************************************************************************
 ?ṹ??    : LPS_SWITCH_PARA_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : Э??ջ?ĺ?Э?鹦?????صĿ??صĽṹ??
*****************************************************************************/
typedef struct
{
    /* ????TA Timer??ʱSRS??PUCCH?ͷſ??? 0:?رգ?1:?򿪣?Ĭ?Ϲر? */
    VOS_UINT32                          ulCloseTaTimer;

    /* ????SR?????????? 0:?ر?  1:?򿪣?Ĭ?Ϲر? */
    VOS_UINT32                          ulSrTrigFlag;

    /* ????SR???????????뿪?? 0:?رգ?1:?򿪣?Ĭ?Ϲر? */
    VOS_UINT32                          ulCloseSrRandFlag;

    /*ͬʧ?????أ?0Ϊ?رգ?1Ϊ?򿪡?Ĭ??Ϊ?رգ?????Ϊ????*/
    /*VOS_UINT32                          ulSyncCtrlFlag;*/

    /*��??̬??��???أ?0Ϊ?رգ?1Ϊ?򿪡?Ĭ??Ϊ?رգ?????Ϊ????*/
    VOS_UINT32                          ulConnMeasFlg;

    /*???ò?��IND??ϢƵ?ʣ???????ʾ?ϱ?????: ȡֵ????Ҫ???ڵ???1*/
    VOS_UINT32                          ulMeasReportMaxNum;

    /*0:?ر?ץ?????? 1:????ץ?????? Ĭ???Ǵ???*/
    VOS_UINT32                          ulIfForwardToPc;

    /*?򿪰?ȫ???ܿ??أ???????Ҫʱ???øñ?־*/
    VOS_UINT32                          ulSmcControl;

    /*????eNB??TDD?Ľ???????*/
    /*VOS_UINT32                          ulMsg4OnlyPadding;*/

    /*?ж??Ƿ?BARС??????*/
    VOS_UINT32                          ulCloseBarCell;

    /*DRX???ܿ??? 0:?ر?  1:????*/
    VOS_UINT32                          ulDrxControlFlag;

   /*?ⳡ????׮???뿪?? 0:?ر?  1:????.Ĭ??Ϊ?ر?*/
    VOS_UINT32                          ulFieldTestSwitch;

    /* PCO???ܿ??أ?0Ϊ?رգ?1Ϊ?򿪡?Ĭ??Ϊ???? */
    VOS_UINT32                          ulPcoFlag;

    /*???ؽṹ*/
    LPS_NV_FLOW_CONTROL                  stPsNvFlowCtl;

    /************************stPsGcfFlag01????BIT????***************************
     bitFlag01:??ʾGCF???Գ???TAU??Ϣ?Ƿ????ܣ?ƽ̨ȱʡֵΪ1??
               0: ???ܣ?1:??????
     bitFlag02:??ʾGCF???Գ???msg4???����أ?ƽ̨ȱʡֵΪ1??
               0: ??ʾ??Э??ʵ??; 1:??ʾ?????ܷ???ʵ??
     bitFlag03:??ʾGCF???Գ????Ƿ?֧??24.301 Table D.1.1?н?��ԭ?????ã?ƽ̨ȱʡֵΪ1??
               0: ֧??; 1:??֧??
     bitFlag04:??ʾGCF???Գ???ָʾ??ȫ?????Ƿ?֧?ֲ???ģʽ??????ģʽ֧?ֿ????????㷨??ƽ̨ȱʡֵΪ1??
               0: ֧??; 1:??֧??
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stPsGcfFlag01;

    /************************stPsGcfFlag02????BIT????***************************
     stPsGcfFlag01????BIT????:
     bitFlag01:
     bitFlag02:
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stPsGcfFlag02;

    /************************stPsFunFlag01????BIT????***************************
     bitFlag01: Band64?Ƿ?֧?֣?ȱʡֵ:0; 0:??֧??;1:֧??
     bitFlag02: ɾ??SDFʱ?Ƿ?Я??Qos??Ϣ,ȱʡֵ:0; 0:Я??;1:??Я??
     bitFlag03: ?Ƿ?????��??̬?յ???ue??ҵ??Ѱ?????????ͷ?,ȱʡֵ:0; 0:?ͷ?;1:???ͷ?
     bitFlag04: У?鰲ȫ???ܿ???,ȱʡֵ:0; 0:????;1:?ر?
     bitFlag05:
     bitFlag06:
     bitFlag07: ???Ź??ܺ???֮ǰ??װ??????ʱ????DRB??????PDCP???ڶ???,0Ϊ?رգ?1Ϊ?򿪡?Ĭ??Ϊ?ر?
     bitFlag08: Printʱ???Ż????أ?ȱʡֵ:0; 0:??;1:??;????RRM????ʱʱ?Զ???Ч??
     bitFlag09: ??????Ϣʱ???Ż????أ?ȱʡֵ:0; 0:??;1:??;
     bitFlag10: MBMS??֧?ֿ??أ?ȱʡֵ:0; 0:??;1:??;
     bitFlag11: SBM???????󿪹أ??ض?band֧???ض??Ĵ???.ȱʡֵ:0; 0:??;1:??;
     bitFlag12: ????????С???Ĵ????Ƕ???Э??Ƶ???ڣ?ȱʡֵ:1; 0:??;1:??;
     bitFlag13: ???Բ?????,ģ???л?ʧ????????ȱʡֵ:1; 0:??;1:??;
     bitFlag14: DCM???????? GU??L???л?NAS???жϱ???TA;ȱʡֵ:0; 0:??;1:??;
     bitFlag15: ?Ƿ?֧??ETWS????;ȱʡֵ:1; 0:??;1:??;
     bitFlag16: ?Ƿ?֧??CMAS????;ȱʡֵ:1; 0:??;1:??;
     bitFlag17:
     bitFlag18: L?ؽ?ʱ?Ƿ?????UTRAN;ȱʡֵ:1; 0:??;1:??;
     bitFlag19: L?ؽ?ʱ?Ƿ?????GSM;ȱʡֵ:1; 0:??;1:??;
     bitFlag20: ???ֽ??յ?ϵͳ??Ϣʱ,????С??RSRP????С???趨ֵ???????жϷ???ҪbarС????;ȱʡֵ:0; 0:??;1:??;
     bitFlag21: NCC???????һ??Ǵ?????????Ŀǰ????Э??ʵ?֣?ȱʡֵ:0; 0??ʾ????Э?飬1??ʾ??????Э??
     bitFlag22: ?Ƿ?֧?ֱ???????,ȱʡֵ:0;1:?أ?0:????
     bitFlag23: ?ؽ?ʱ??Ҫ????GAP?Ƿ???????????????һ???????㣬ȱʡֵ:1; 1:????0:??
     bitFlag24:
     bitFlag25:
     bitFlag26: IDLE????ѡǰ???ж?NCC??ȱʡֵ:0; 0??ʾ??Ҫ?жϣ?1??ʾ???жϡ?
     bitFlag27: V7R1?ձ?DCOM??????ȱʡֵ:0; 0:??;1:??;
     bitFlag28: V7R1?ձ?DCOM??????ȱʡֵ:0; 0:??;1:??;
     bitFlag29: V7R1?ձ?DCOM??????ȱʡֵ:0; 0:??;1:??;
     bitFlag30: V7R1?ձ?DCOM??????ȱʡֵ:0; 0:??;1:??;
     bitFlag31: V7R1?ձ?DCOM??????ȱʡֵ:0; 0:??;1:??;
     bitFlag32: RRM????ʱ??ȱʡ?رմ?ӡ?Ƿ???Ч??ȱʡֵ:0; 1:????Ч??0:??Ч??
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stPsFunFlag01;

    /************************stPsFunFlag02????BIT????***************************
     bitFlag01: CA???Կ???:ȱʡֵ:0; 0:??;1:??:;
     bitFlag02:
     bitFlag03:
     bitFlag04: CMAS??פ??֮ǰ????פ??֮???ϱ???ȱʡֵ:0;0: פ??֮???ϱ?; 1:פ??ǰ?󶼿????ϱ?
     bitFlag05:
     bitFlag06:
     bitFlag07: ?????ѣ?ֻ??L??ģ.????GUģ????.ȱʡֵ:0; 0:??;1:??;
     bitFlag08: ETWS????ʱ??; 1:��?????ͣ?0:???????ٷ???
     bitFlag09: TL L2?ؼ??¼??ϱ?-ȱʡֵ:0;0:??;1:??
     bitFlag10: LTE L2 ??????Ϣ?ϱ?-ȱʡֵ:0;0:??;1:??
     bitFlag11: LIST????????Ƶ???Ƿ?????2?Ρ?0:??;1:??
     bitFlag12: ????ָ??ģʽ?ض??????????Ƿ?????????????ģʽ???ض???????; 0:????????ָ??ģʽ???ض???֮??????????????ģʽ???ض???;1:ֻ????????ָ??ģʽ???ض???
     bitFlag13:
     bitFlag14: LTE CAС??״̬??Ϣ?ϱ?-ȱʡֵ:0;0:??;1:??
     bitFlag15:
     bitFlag16:
     bitFlag17:
     bitFlag18:
     bitFlag19:
     bitFlag20:
     bitFlag21:
     bitFlag22:
     bitFlag23:
     bitFlag24:
     bitFlag25:
     bitFlag26:
     bitFlag27:
     bitFlag28:
     bitFlag29:
     bitFlag30:
     bitFlag31:
     bitFlag32:
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stPsFunFlag02;


    LPS_NV_LONG_BIT_STRU                 stPsFunFlag03;

   /************************stPsFunFlag04????BIT????***************************
     bitFlag01:
     bitFlag02: ?޿??????򿪻??��?ʧ?ܣ??Ƿ?ɾ????NV?еİ?ȫ??????,ȱʡֵ:1; 0:ɾ??; 1:??ɾ??
     bitFlag03:
     bitFlag04:
     bitFlag05:
     bitFlag06:
     bitFlag07:
     bitFlag08:
     bitFlag09:
     bitFlag10:
     bitFlag11:
     bitFlag12:
     bitFlag13:
     bitFlag14:
     bitFlag15:
     bitFlag16:
     bitFlag17:
     bitFlag18:
     bitFlag19:
     bitFlag20:
     bitFlag21:
     bitFlag22:
     bitFlag23:
     bitFlag24:
     bitFlag25:
     bitFlag26:
     bitFlag27:
     bitFlag28:
     bitFlag29:
     bitFlag30:
     bitFlag31:
     bitFlag32:
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stPsFunFlag04;

    LPS_NV_GLOBAL_PRINT_STRU             stPsNvGlobalPrint;
}LPS_SWITCH_PARA_STRU;

/*****************************************************************************
 ?ṹ??    : TPS_SWITCH_PARA_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : Э??ջ?ĺ?Э?鹦?????صĿ??صĽṹ??
*****************************************************************************/
typedef struct
{
    /************************stTpsLayFliterFlag01????BIT????********************
     bitFlag01:??????ϢOP_HL1_SCHEDULE_IND???˿??ء?ƽ̨ȱʡֵΪ0
               0:???? 1:??????
     bitFlag02:??????ϢOP_T_RRC_trafficMeasPeriod???˿??ء?ƽ̨ȱʡֵΪ0
               0:???? 1:??????
     bitFlag03:???ֶ?̬??ӡ?رգ?һ???????顢ѭ???ȴ?Ҫ??Ϣ?رա?ƽ̨ȱʡֵΪ0
               0:?رմ?ӡ 1:???رմ?ӡ
     bitFlag04:
     bitFlag05:
     bitFlag06:
     bitFlag07:
     bitFlag08:
     bitFlag09:
     bitFlag10:
     bitFlag11:
     bitFlag12:
     bitFlag13:
     bitFlag14:
     bitFlag15:
     bitFlag16:
     bitFlag17:
     bitFlag18:
     bitFlag19:
     bitFlag20:
     bitFlag21:
     bitFlag22:
     bitFlag23:
     bitFlag24:
     bitFlag25:
     bitFlag26:
     bitFlag27:
     bitFlag28:
     bitFlag29:
     bitFlag30:
     bitFlag31:
     bitFlag32:

    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stTpsLayFliterFlag01;

    /************************stTpsLayFliterFlag01????BIT????********************
     bitFlag01:
     bitFlag02:
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stTpsLayFliterFlag02;

    /************************stTPsGCFFlag01????BIT????**************************
     bitFlag01: GCF?Ƿ?????cs loop ???ܡ?
                ƽ̨ȱʡֵΪ0
                0:?ر?CsLoop????  1:?򿪹???
                ?˹????Ѿ??Զ?ʶ?????????򿪡?

     bitFlag02: TDS????????????,?Ǳ깦?ܡ????ܴ???ʱTRRC??????S׼????ֱ?ӷ??????????̡?
                ƽ̨ȱʡֵΪ0
                0:???ܹر? 1:???ܴ???

     bitFlag03: ׼FR???Կ???
                ƽ̨ȱʡֵΪ1
                0:???ܹر? 1:???ܴ???

     bitFlag04: TG????????sOfIratsearch?????Ż????Ѳ?ʹ?á?
                ƽ̨ȱʡֵΪ0
                0:?رչ???  1:?򿪹???

     bitFlag05: TG????????QrxLev?????Ż????Ѳ?ʹ?á?
                ƽ̨ȱʡֵΪ0
                0:?رչ???  1:?򿪹???

     bitFlag06: ??T??SֵС??0ʱ??????GSM??Sֵ????0??С????ֱ????ѡ???Ѳ?ʹ?á?
                ƽ̨ȱʡֵΪ0
                0:?رչ???  1:?򿪹???

     bitFlag07: ??ѡ?Ż?,?Ǳ깦?ܡ?????????��????????Χ?ڣ?????��??6db????֤??ѡ??tdsʱ??t??��?㹻ǿ
                ƽ̨ȱʡֵΪ0
                0:?رչ???  1:?򿪹???

     bitFlag08: RRM׮??????ֻ?е??˹??ܴ򿪣?RRM???Բ???ͨ????
                ƽ̨ȱʡֵΪ0
                0:???ܹر? 1:???ܴ???

     bitFlag09:
     bitFlag10:
     bitFlag11:
     bitFlag12:
     bitFlag13:
     bitFlag14:
     bitFlag15:
     bitFlag16:
     bitFlag17:
     bitFlag18:
     bitFlag19:
     bitFlag20:
     bitFlag21:
     bitFlag22:
     bitFlag23:
     bitFlag24:
     bitFlag25:
     bitFlag26:
     bitFlag27:
     bitFlag28:
     bitFlag29:
     bitFlag30:
     bitFlag31:
     bitFlag32:
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stTPsGCFFlag01;

    /************************stTPsGCFFlag02????BIT????**************************
     bitFlag01:
     bitFlag02:
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stTPsGCFFlag02;

    /************************stTPsFunFlag01????BIT????**************************
     bitFlag01: CellUpdate?????Ż?????T313??ʱ??????ֻ??PS??ʱ??????CellUpdateֱ??,????IDL״̬
                ƽ̨ȱʡֵΪ1??
                0:???ܴ??? 1:???ܹر?

     bitFlag02: debug??Ϣ???���??????״̬?µ?С????Ϣ??״̬?????ݽ??б??棬
                ƽ̨ȱʡֵΪ0.
                0:???ܴ??? 1:???ܹر?

     bitFlag03: debug??Ϣ?ϱ????ܣ???Debug??Ϣ?Բ?????ʽ?ϱ???
                ƽ̨ȱʡֵΪ0??
                0:???ܴ??? 1:???ܹر?

     bitFlag04: SMCʹ??ͬ???????ӿڵĲ???
                ƽ̨ȱʡֵΪ0
                0:???ܹر? 1:???ܴ???

     bitFlag05: TCP ACK???????ܡ?
                ƽ̨ȱʡֵΪ0
                0:???ܹر? 1:???ܴ???

     bitFlag06: DetectCell???ܡ?
                ƽ̨ȱʡֵΪ0
                0:???ܹر? 1:???ܴ???

     bitFlag07: ????????????ЧMeasCtrl?????ض????绷????ʹ??
                ƽ̨ȱʡֵΪ0
                0:???ܹر? 1:???ܴ???

     bitFlag08: CCCH??Ϣ?ϱ????��ε???????UE?Ŀտ?
                ƽ̨ȱʡֵΪ1
                0:???ܹر? 1:???ܴ???

     bitFlag09:ADRX???????? ,????Ѱ???ط???С???ڣ???Ч???ӳ???DRX???????ڣ???????Ч???ʹ???????
               ƽ̨??ȱʡֵΪ0
               0:???ܹر? 1:???ܴ???

     bitFlag10:
     bitFlag11:
     bitFlag12:
     bitFlag13:
     bitFlag14:
     bitFlag15:
     bitFlag16:
     bitFlag17:
     bitFlag18:
     bitFlag19:
     bitFlag20:
     bitFlag21:
     bitFlag22:
     bitFlag23:
     bitFlag24:
     bitFlag25:
     bitFlag26:
     bitFlag27:
     bitFlag28:
     bitFlag29:
     bitFlag30:
     bitFlag31:
     bitFlag32:

    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stTPsFunFlag01;

    /************************stTPsFunFlag02????BIT????**************************
     bitFlag01: TDS DSDS ?????Ƿ???DCH_CONFIG???ò???.Ĭ??Ϊ0
                0:???ܴ???  1:???ܹر?
     bitFlag02: TDS DSDS ?????Ƿ???500ms????SignalRel??Ĭ??Ϊ0.
                0:???ܴ???  1:???ܹر?
    ****************************************************************************/
    LPS_NV_LONG_BIT_STRU                 stTPsFunFlag02;
}TPS_SWITCH_PARA_STRU;

/*****************************************************************************
 ?ṹ??    : TDS_CERSSI_REPORT_PARA_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : Э??ջ?ĺ?Э?鹦?????صĿ??صĽṹ??
*****************************************************************************/
typedef struct
{
    VOS_UINT32                           ulThreshold;
    VOS_UINT32                           ulInterval;
    VOS_UINT8                            aucRsv[4];
}TDS_CERSSI_REPORT_PARA_STRU;
/*****************************************************************************
 ?ṹ??    : LPS_LTE_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : Э??ջ?ĺ?LTE???ص?????
*****************************************************************************/
typedef struct
{
    /* LTE???߸??? */
    VOS_UINT32                          ulPsCfgDspAntCnt;

    /*VOS_UINT32                          ulPsCfgDspRatMod;*/
    /*Ƶ???ţ?ȡֵ??Χ??1??40???ֱ???Ӧ36.101??bandȡֵ,Ĭ??Ϊ20*/
    VOS_UINT8                          ucEutraBand;

    VOS_UINT8                          aucRsv[3];

    /* ????????֡???ȣ?Ĭ??Ϊ1 */
    VOS_UINT32                          ulCfgSubFrameAssign;

    /* ??????֡???ã?Ĭ??Ϊ7 */
    VOS_UINT32                          ulSubFramePattern;
}LPS_LTE_CONFIG_STRU;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_CAND_CELL_LIST_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ????С??NV?ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usFreqCount;                            /* ??Ч??Ƶ????Ŀ */
    VOS_UINT8                           aucReserved1[2];
    RRC_NV_CSEL_CAND_FREQ_STRU          ausCandFreqs[RRC_NV_EACH_BAND_FREQ_COUNT];/* ????Ƶ???б? */
}LRRC_NV_CAND_CELL_LIST_STRU;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_SUPPORT_BAND_LIST_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : UE֧??Ƶ??NV?ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usBandCount;                            /* ??Ч??Ƶ????Ŀ */
    VOS_UINT8                           aucReserved1[2];
    RRC_NV_CSEL_FREQ_BAND_INFO_STRU     astCandBands[RRC_NV_EACH_BAND_BANDS_COUNT];
}LRRC_NV_SUPPORT_BAND_LIST_STRU;

/*****************************************************************************
 ?ṹ??    : RRC_NV_EACH_BAND_IND_INFO_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ÿ??Band?µĺ?????Ϣ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucBandInd;                              /* Ƶ??ָʾ */
    PS_BOOL_ENUM_UINT8                  enFirstSearchFlg;                       /* Ƶ????Ƶ???????׶Σ?????????????Ƶ?? */
    VOS_UINT8                           aucReserved1[2];
    LRRC_NV_CAND_CELL_LIST_STRU         stCandCellInfo;                         /* Band?ں???С????Ϣ */
    LRRC_NV_SUPPORT_BAND_LIST_STRU      stCandBandsInfo;                        /* Band?ں???Ƶ????Ϣ */
}LRRC_NV_EACH_BAND_CAND_INFO_STRU;


/*****************************************************************************
 ?ṹ??    : LRRC_NV_CALIBRATION_BAND_LIST_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : UE??ҪУ׼??band??Ϣ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucReserved[2];
    VOS_UINT16                          usCnt;
    VOS_UINT8                           aucEutraBand[RRC_MAX_NUM_OF_BANDS];
}LRRC_NV_CALIBRATION_BAND_LIST_STRU;

/*****************************************************************************
 ?ṹ??    : LRRC_UE_CAP_MEAS_EUTRA_BAND_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : UE EUTRA??��??MEAS->Band????
*****************************************************************************/
typedef struct
{
    RRC_INTER_FREQ_EUTRA_BAND_INFO_STRU stInterFreqEutraBandInfo;
    RRC_INTER_RAT_BAND_INFO_STRU        stInterRatBandInfo;
}LRRC_UE_CAP_MEAS_EUTRA_BAND_STRU;

/*****************************************************************************
 ?ṹ??    : RRC_UE_EUTRA_CAP_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : UE EUTRA??��??Ϣ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitFeatureGroupIndsPresent    : 1;
    VOS_UINT32                          bitUeCapV920IesPresent        : 1;
    VOS_UINT32                          bitSpare                      : 30;
    VOS_UINT8                           aucReserved[2];
    /* ??��????UE???????汾??1??֧?֣?0????֧?֣??ӵ͵???λ?ֱ???��ָʾRel8,Rel9,.... */
    VOS_UINT8                           ucAsRelBitmap;
    RRC_UE_CATEGORY_ENUM_UINT8          enUeCatg;
    RRC_UE_CAP_PDCP_PARA_STRU           stPdcpPara;
    RRC_UE_CAP_PHY_PARA_STRU            stPhyPara;
    RRC_UE_CAP_RF_PARA_STRU             stRfPara;
    RRC_UE_CAP_MEAS_PARA_STRU           stMeasPara;
    RRC_UE_CAP_FEATURE_GROUP_IND_STRU   stFeatrueGroupInds;
    RRC_UE_CAP_INTER_RAT_PARA_STRU      stInterRatPara;
    RRC_UE_EUTRA_CAP_V920_IES_STRU      stUeCapV920Ies;
} LRRC_NV_UE_EUTRA_CAP_STRU;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_RRC_CTRL_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : RRC???ƿ??ؽṹ
             ??ѡ3db???ƿ???,BARС????????????????,SIB???ն?ʱ??ѡ?񿪹?
*****************************************************************************/
typedef struct
{
    RRC_NV_SWITCH_ENUM_UINT32            en3dbCtlFlg;           /* Ĭ??ֵΪ?ر? 0 */
    VOS_UINT32                           ulMaxSeachCntOfBarFrq;
    RRC_NV_TIMER_CHOICE_ENUM_UINT32      enSibTimerChoiceFlg;   /* Ĭ??Ϊ0 */
    VOS_UINT32                           ulReserved;            /* Ԥ??1λ */
}LRRC_NV_RRC_CTRL_STRU;

enum LRRC_NV_LOCK_MODE
{
    LRRC_NV_LOCK_MODE_RoadTest                    = 0,/*·????Ƶ??????Ƶģʽ??,??List????????*/
    LRRC_NV_LOCK_MODE_POS                         = 1,/*POS?�����????Ƶ??????Ƶģʽ?£?Ҫ????List????????*/
    LRRC_NV_LOCK_MODE_BUTT
};
typedef VOS_UINT8   LRRC_NV_LOCK_MODE_UINT8;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_LOCK_INFO_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  :NV??????Ϣ???ݽṹ
*****************************************************************************/
typedef struct
{
    RRC_NV_LOCK_INFO_ENUM_UINT32         enLockInd;
    /*begin: add by wangmiao00272217 */
    PS_BOOL_ENUM_UINT8                   ucActiveFlag;                   /*??Ч??־λ???????µ綪ʧ,PS_TRUE?µ???Ч????????Ч*/
    LRRC_NV_LOCK_MODE_UINT8              ucLockMode;                     /*??Ƶģʽ,*/
    /*end: add by wangmiao00272217 ??Ч??־λ???????µ綪ʧ,PS_TRUE?µ???Ч????????Ч*/
    VOS_UINT8                            ucBandInd;                      /* Ƶ??ָʾ */
    VOS_UINT8                            ucForbidReselAndHO;             /* ??Ƶ???????Ƿ??????л?????ѡ */
    VOS_UINT16                           usCellId;                       /* С??ID */
    VOS_UINT16                           usFreqPoint;                    /* Ƶ?? */
    RRC_NV_CSEL_FREQ_BAND_INFO_STRU      stLockedBand;                   /* ????Band??Ϣ */
} LRRC_NV_LOCK_INFO_STRU;

/* modify by lishangfeng begin */

/*****************************************************************************
 ?ṹ??    : LRRC_NV_CSQ_RPT_INFO_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ??��?ϱ??????ں?????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                       ulRssiReportLen;
    VOS_UINT32                       ulDetaRssi;    /* ??��?ϱ????? */
    VOS_UINT16                       usMs;          /* ?ϱ?ʱ?????? */
    VOS_UINT16                       usDetaSINR;    /* ?????????? (0,50)db*/
} LRRC_NV_CSQ_RPT_INFO_STRU;

/* modify by lishangfeng end */

/* wangrui 20120817 begin */
/*****************************************************************************
 ?ṹ??    : LRRC_NV_LWCLASH_RANGE_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ??ͻ??Χ?????޺?????
*****************************************************************************/
typedef struct
{
    VOS_UINT16                        usFreqBegin;        /*??Χ????Ƶ?? ??λ:100Khz*/
    VOS_UINT16                        usFreqEnd;          /*??Χ????Ƶ?? ??λ:100Khz*/
} LRRC_NV_LWCLASH_RANGE_INFO_STRU;
/*****************************************************************************
 ?ṹ??    : LRRC_NV_LWCLASH_RANGE_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ??ͻ??Χ?????޺?????
*****************************************************************************/
typedef struct
{
    VOS_UINT16                        ulnum;                /*??Χ????Ƶ?? ??λ:100Khz*/
    LRRC_NV_LWCLASH_RANGE_INFO_STRU   stBandRng[RRC_NV_LWCLASH_RANGE_COUNT]; /*??Χ????Ƶ?? ??λ:100Khz*/
} LRRC_NV_LWCLASH_RANGE_STRU;

/* wangrui 20120817 end */
/*****************************************************************************
 ?ṹ??    : LRRC_NV_UE_RFRD_TYPE_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  :NV????Ƶ???ͽṹ
*****************************************************************************/
typedef struct
{
    RRC_NV_UE_RFRD_TYPE_ENUM_UINT8       enUeRfrdType;              /* ??Ƶ???? */
    VOS_UINT8                            aucReserved[3];

} LRRC_NV_UE_RFRD_TYPE_STRU;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_RF_HW_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  :9361RF????
*****************************************************************************/
typedef struct
{
    RRC_PHY_LTE_RF_HW_ENUM_UINT8       enLteRfHw;
    VOS_UINT8                          aucRsv[3];
} LRRC_NV_RF_HW_STRU;

/*****************************************************************************
 ?ṹ??    : LTE_COMPENSATE_SWITCH_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ????????, ??NV?��?????DSP
*****************************************************************************/
typedef struct
{
    VOS_UINT16 usFreqCmpnstSwitch;      /*0???ص?Ƶ?ʲ??? 1:????Ƶ?ʲ???*/
    VOS_UINT16 usTempCmpnstSwitch;      /*0???ص??¶Ȳ???  1:?????¶Ȳ???*/
    VOS_UINT16 usTxPwrCmpnstSwitch;     /*0???ص????书?ʲ??? 1:?򿪷??䲹?? ????ǰ?˿???Ԥ??*/
    VOS_UINT16 usReserved;
}LTE_COMPENSATE_SWITCH_STRU;

/*****************************************************************************
 ?ṹ??    : LUPLINK_POWER_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ????????, ??NV?��?????DSP
*****************************************************************************/
typedef struct
{
    VOS_UINT16 usPuschPwrCtrlSwitch;
    VOS_UINT16 usPuschClosedLoopSwitch;
    VOS_UINT16 usPucchPwrCtrlSwitch;
    VOS_UINT16 usPucchClosedLoopSwitch;
    VOS_UINT16 usSrsPwrCtrlSwitch;
    VOS_UINT16 usPrachPwrCtrlSwitch;

    VOS_INT16  sPuschTxPower;
    VOS_INT16  sPucchTxPower;
    VOS_INT16  sSrsTxPower;
    VOS_INT16  sPrachTxPower;
}LUPLINK_POWER_CONFIG_STRU;

/* LTE PS Transfer begin */
/* MTC AREA LOST Begins */
#if(FEATURE_MULTI_MODEM == FEATURE_ON)
typedef struct
{
    PS_BOOL_ENUM_UINT8      enValidFlag;  /*?????Ƿ???Ч  */
    VOS_UINT8               ucLrrcMTCLostThreshold; 	/* LTE??PSǨ??????,0~31 */
    VOS_UINT8               ucLrrcMTCAvailableDValue;    /* LTE??PS??Ǩ??????Ǩ?????޲?ֵ,0~(31-ucLrrcMTCLostThresh) */
    VOS_UINT8               ucMTCLostTimerLen;    /* LTE->CDMA??PSǨ?ƹ??̵Ķ?ʱ?����?,ȡֵ??λ:s */
    VOS_INT16               sRsv; /*????*/
    VOS_UINT16              usMTCAvailableTimerLen; /* CDMA->LTE??PSǨ?ƹ??̵Ķ?ʱ?����?,ȡֵ??λ:s */
}LTE_MTC_AREA_LOST_THRESHOLD_STRU;
#endif
/* MTC AREA LOST End */
/* LTE PS Transfer end */


/* gaojishan-PLMN_EXACTLY_COMPARE_FLG */
#ifdef PS_ITT_PC_TEST2
typedef struct
{
    VOS_UINT8   ucPlmnExactlyCompareFlag;
    VOS_UINT8   aucRsv[3];
}NVIM_PLMN_EXACTLY_COMPARE_FLAG_STRU;
#endif
/* TCXo */
#if (VOS_OS_VER == VOS_WIN32)
/*****************************************************************************
 ?ṹ??    : LTE_TCXO_DYNAMIC_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : TCXO??̬??Ϣ
*****************************************************************************/
typedef struct
{
    VOS_UINT16 usVctcxoThdHigh;             /* ??̬TCXO?????? */
    VOS_UINT16 usVctcxoDynamic;             /* ??̬TCXOֵ, UE?ϵ?ʱ??HPA??ȡNV??????DSP,
                                               UE?ϵ翪??????��??̬????DSP??ȡ??Чֵд??NV */
    VOS_UINT16 usVctcxoFlag;                /* ??̬TCXOȡֵ?Ƿ???Ч??ʶ, DSP?ɲ???,
                                               0: ??Ч????????Ч????ЧʱusDynamicTcxoֵ?????? */
    VOS_UINT16 usVctcxoFuncSwitch;          /* ??̬TCXO?????Ƿ?ʹ?ÿ???, DSP?ɲ???,
                                               ?޸ĺ?, ??Ҫ?µ????ϵ??Ż???Ч
                                               0: ?ر?, ?ر?ʱusVctcxoDynamicȡֵusVctcxoInit
                                               1: ???? */
}LTE_TCXO_DYNAMIC_CONFIG_STRU;
#endif
#if 0
/*****************************************************************************
 ?ṹ??    : LRRC_NV_UE_AD9361_INFO_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  :NV????Ƶ9361??Ϣ?ṹ
*****************************************************************************/
typedef struct
{
    /*Common NV List*/
    VOS_UINT16 usVctcxoInit;                /*Vco??ֵ*/
    VOS_UINT16 usRsvd;
    VOS_UINT16 usAntModemLoss[16];          /*Modem?????????߼?????*/
    VOS_UINT16 usTempSensorTable[20];       /*PA???????????⵽??ƽ̨?¶?ת???ɵ?ѹ??????HKADC??????????ֵ*/
    /*Rx NV List*/
    VOS_UINT16 usRxCalibrFreqList[16];      /*RxƵ??У׼??*/
    VOS_UINT32 ulRxAgcTableAnt1[128];       /*??Ӧ????ad9361??agc tabel*/
    VOS_UINT32 ulRxAgcTableAnt2[128];
    VOS_UINT8  ucRxAagcGainIdxTblAnt1[128]; /*aagc?߼?????????????*/
    VOS_UINT8  ucRxAagcGainIdxTblAnt2[128]; /*aagc?߼?????????????*/
    VOS_UINT16 usRxAgcTableAnt1Err[128];      /*??Ӧlna???ĸ???λ*/
    VOS_UINT16 usRxAgcTableAnt2Err[128];
    VOS_UINT16 usLnaGain0FreqComp[2][16];   /*Lna?ڵ?λ0ʱ??Ӧ?Ĳ?ͬ???ߺ?Ƶ???µĲ???ֵ*/
    VOS_UINT16 usLnaGain1FreqComp[2][16];   /*Lna?ڵ?λ1ʱ??Ӧ?Ĳ?ͬ???ߺ?Ƶ???µĲ???ֵ*/
    VOS_UINT16 usLnaGain2FreqComp[2][16];   /*Lna?ڵ?λ2ʱ??Ӧ?Ĳ?ͬ???ߺ?Ƶ???µĲ???ֵ*/
    VOS_UINT16 usLnaGain3FreqComp[2][16];   /*Lna?ڵ?λ3ʱ??Ӧ?Ĳ?ͬ???ߺ?Ƶ???µĲ???ֵ*/
    VOS_INT16 sLnaGain0TmpComp[20];        /*Lna?ڵ?λ0ʱ??Ӧ?Ĳ?ͬ?¶?ʱ?ĵĲ???ֵ*/
    VOS_INT16 sLnaGain1TmpComp[20];        /*Lna?ڵ?λ1ʱ??Ӧ?Ĳ?ͬ?¶?ʱ?ĵĲ???ֵ*/
    VOS_INT16 sLnaGain2TmpComp[20];        /*Lna?ڵ?λ2ʱ??Ӧ?Ĳ?ͬ?¶?ʱ?ĵĲ???ֵ*/
    VOS_INT16 sLnaGain3TmpComp[20];        /*Lna?ڵ?λ3ʱ??Ӧ?Ĳ?ͬ?¶?ʱ?ĵĲ???ֵ*/
    /*Tx NV List*/
    VOS_UINT16 usTxCalibrFreqList[16];      /*TxƵ??У׼??*/
    VOS_UINT16 usTxApcHighGainTable[32];    /*apc????????*/
    VOS_UINT16 usTxApcLowGainTable[60];     /*apc????????*/
    VOS_UINT16 usTxApcHighThrd;             /*apc?????浽??????????*/
    VOS_UINT16 usTxApcLowThrd;              /*apc?????浽??????????*/
    VOS_UINT16 ausTxApcHighPwr[4];         /* ?????湦?ʵ? */
    VOS_UINT16 ausTxApcLowPwr[4];          /* ?????湦?ʵ? */
    VOS_UINT16 usTxApcFreqCompHighGain[4][16]; /*paƵ?ʲ?????0.25db????*/
    VOS_UINT16 usTxApcFreqCompLowGain[4][16];
    VOS_INT16 sTxApcTempCompHighGain[20];  /*???ڲ?????ͬ?¶??µ?PA??????????*/
    VOS_INT16 sTxApcTempCompLowGain[20];   /*???ڲ?????ͬ?¶??µ?PA??????????*/
    VOS_UINT16 usTxPowerDetVoltage[16];     /*PA???ʼ???ģ???????ĵ?ѹ??ʵ?ʷ??͹??ʵĹ?ϵ*/
    LTE_COMPENSATE_SWITCH_STRU   stCmpnstSwitch;
    LUPLINK_POWER_CONFIG_STRU stUlPwrCfg;
    /*Timer NV List*/
    VOS_UINT16 ulAdcOpenTime;
    VOS_UINT16 usRficT1;
    VOS_UINT16 usRficT2;
    VOS_UINT16 usRficT3;
    VOS_UINT16 usRficT4;
    VOS_UINT16 usRficT5;
    VOS_UINT16 usRficT6;
    VOS_UINT16 usRficT7;
    VOS_UINT16 usRficT8;
    VOS_UINT16 usRficT9;
    VOS_UINT16 usRficT10;
    VOS_UINT16 usPaOpenTime;
    VOS_UINT32 ulAagcLNATH[7];
    VOS_UINT32 ulAagcLNAOffAddr[4];
    VOS_UINT16 usDbmToGainTbl_normal[120];
    VOS_UINT16 usDbmToGainTbl_AcsBlk[128];
    LTE_TCXO_DYNAMIC_CONFIG_STRU stTcxoDynamicCfg;
}LRRC_NV_UE_AD9361_INFO_STRU;

#endif
/*****************************************************************************
 ?ṹ??    : LTE_GATE_CLOCK_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  :ʱ???ſ?
*****************************************************************************/
typedef struct
{
    VOS_UINT16 usGateClockEnableFlag;       /* 0:invalid, 1:Valid, Ĭ?? 0 */
    VOS_UINT16 usRsvd;
} LTE_GATE_CLOCK_CONFIG_STRU;

/*****************************************************************************
 ?ṹ??    : FC_LTE_CPU_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ????LTE CPU???????ýṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT16          usFirstDelay;           /* ?ӳٽ???????״̬??��?????????ڸ?????ֵ?Ž??????? */
    VOS_UINT16          usUlDownRate;           /* ???н??ٲ????ٷֱ? */
    VOS_UINT16          usUlUpRate;             /* ???????ٲ????ٷֱ? */
    VOS_UINT16          usUlMinThr;             /* ???н??????? kbps */
    VOS_UINT16          usHqDownRate;           /* HARQ???ٲ????ٷֱ? */
    VOS_UINT16          usHqUpRate;             /* HARQ???ٲ????ٷֱ? */
    VOS_UINT16          usHqMaxDiscardRate;     /* HARQ???????? */
    VOS_UINT16          usDowngradeThres;       /* ??????ֵ??CPUռ???ʸ??ڸ?ֵʱ????ʼ???? */
    VOS_UINT16          usUpgradeThres;         /* ??????ֵ??CPUռ???ʵ??ڸ?ֵʱ????ʼ???? */
	VOS_UINT16          usResv;
} FC_LTE_CPU_CONFIG_STRU;

/*****************************************************************************
 ?ṹ??    : FC_LTE_TEMPERATURE_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ????LTE ?¿????ýṹ
*****************************************************************************/
#define FC_UL_THROUGHPUT_THRES_CNT_NV          (12)
typedef struct
{
    VOS_UINT8           ucInitialPos;           /* ???ٿ?ʼλ??0~11, Ĭ??4 */
    VOS_UINT8           ucZeroSupport;          /* ?Ƿ?֧?????ʽ???0,0??ʾ??֧?֣?Ĭ??0 */
    VOS_UINT8           ucResv[2];              /* ????λ */
    VOS_UINT32          ulTemperSteps[FC_UL_THROUGHPUT_THRES_CNT_NV];      /* ?¿????ʵ?λkbps????ֵ[50000, 40000, 30000, 25000, 20000, 15000, 10000, 5000, 2000, 0, 0, 0] */
    VOS_UINT32          ulMinBsrThr;            /* BSR?ܹ????Ƶ???С????kbps??Ĭ??120 */
} FC_LTE_TEMPERATURE_CONFIG_STRU;

/*****************************************************************************
 ?ṹ??    : FLOWCTRL_LTE_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ????LTE???????ýṹ
*****************************************************************************/
typedef struct
{
    FC_LTE_TEMPERATURE_CONFIG_STRU stFcTemperatureConfig;
    FC_LTE_CPU_CONFIG_STRU  stFcCpuConfig;
} FLOWCTRL_LTE_CONFIG_STRU;


/* gaojishan-SYSCFG-SAVENV-Begin */
#define NV_ITEM_EUTRA_MAX_SUPPORT_BANDS_NUM         (64)
#define NV_MAX_FDD_FREQ_BANDS_NUM                 (32)

/*
NVIM_UE_SUPPORT_FREQ_BAND_STRU?ṹ˵??:
usWcdmaBand??usGsmBand??Bitλ??ʾ?û????õ?Ƶ?Σ?bit1????Ƶ??I,bit2????Ƶ??II,
?��?????,????λΪ1,??ʾ֧?ָ?Ƶ??.?±??Ǳ???λ??Ƶ?ζ?Ӧ??ϵ:
-------------------------------------------------------------------------------
        bit8       bit7      bit6     bit5    bit4     bit3      bit2     bit1
-------------------------------------------------------------------------------
WCDMA   900(VIII)  2600(VII) 800(VI)  850(V)  1700(IV) 1800(III) 1900(II) 2100(I) oct1
        spare      spare     spare    spare   spare    spare     spare   J1700(IX)oct2
-------------------------------------------------------------------------------
GSMƵ?? 1900(VIII) 1800(VII) E900(VI) R900(V) P900(IV) 850(III)  480(II)  450(I)  oct3
        spare      spare     spare    spare   spare    spare     spare    700(IX) oct4
-------------------------------------------------------------------------------
aucUeSupportWcdmaBand??aucUeSupportGsmBand????????ʾUE֧?ֵ?Ƶ??,???Դ洢˳????
?Ⱥ???ʾƵ??????˳??,??0xff??ʾ??Ч.

????:
oct1-oct4?ֱ??ǣ?0x03,0x00,0x7B,0x00
   ???????û?????Ƶ??Ϊ??W??WCDMA-I-2100, WCDMA-II-1900
                         G??850(III),P900(IV),R900(V),E900(VI),1800(VII)
oct5-oct16?ֱ???:2,5,1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
   ??????UE֧??WƵ??I,II,V,????˳????:II,V,I.
oct17-oct28?ֱ???:4,5,8,7,6,3,0xff,0xff,0xff,0xff,0xff,0xff
   ??????UE֧??GƵ??III,IV,V,VI,VII,VIII,????˳????:IV,V,VIII,VII,VI,III.
*/
typedef struct
{
    VOS_UINT32                  ulWcdmaBand;
    VOS_UINT16                  usGsmBand;
    VOS_UINT8                   aucReserve[2];
    VOS_UINT8                   aucUeSupportWcdmaBand[NV_MAX_FDD_FREQ_BANDS_NUM];
    VOS_UINT8                   aucUeSupportGsmBand[NV_MAX_FDD_FREQ_BANDS_NUM];
}NV_UE_SUPPORT_FREQ_BAND_STRU;

/*****************************************************************************
 ?ṹ??    : NV_UE_EUTRA_FREQ_BAND_INFO_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : LTE֧?ֵ?һ??Ƶ?ε?Ƶ????Ϣ?ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucBandNo;                               /* band?ţ?ֵΪ(1-64) */
    VOS_UINT8                           ucDuplexModeFlg;                        /* ˫??ģʽָʾ??1Ϊ??˫????0Ϊȫ˫????Ĭ??Ϊ0 */
    VOS_UINT8                           aucReserve[2];
}NV_UE_EUTRA_FREQ_BAND_INFO_STRU;
/*******************************************************************************
 ?ṹ??    : NV_UE_EUTRA_SUPPORT_FREQ_BAND_LIST_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : LTE֧?ֵ???��?ṹ,??LTE,WAS GAS??ͬ??ȡ????��FDD,TDD,?Լ?LTE֧?ֵ?Ƶ????Ϣ

LTEÿ??BAND????ʼƵ????
1  0     ?C 599
2  600   -  1199
3  1200  ?C 1949
4  1950  ?C 2399
5  2400  ?C 2649
6  2650  ?C 2749
7  2750  ?C 3449
83 450   ?C 3799
93 800   ?C 4149
10 4150  ?C 4749
11 4750  ?C 4949
12 5010  -  5179
13 5180  ?C 5279
14 5280  ?C 5379
...
17 5730  ?C 5849
18 5850  ?C 5999
19 6000  ?C 6149
20 6150  ?C 6449
21 6450  ?C 6599
...
33 36000 ?C 36199
34 36200 ?C 36349
35 36350 ?C 36949
36 36950 ?C 37549
37 37550 ?C 37749
38 37750 ?C 38249
39 38250 -  38649
40 38650 -  39649
64 25450 -  25749
*******************************************************************************/
typedef struct
{
    VOS_UINT16                              usBandCnt;                          /* LTE band????  Ĭ??ֵ4 */
    VOS_UINT8                               aucReserved1[2];                    /* ?????ֽڣ??Ա??? */

    /* LTE֧?ֵ?band??????????Ϊ64 ??Ĭ??֧??7??20??38 40?????a
        stCandBands[0].ucBandNo  =  7
        astCandBands[1].ucBandNo = 20
        astCandBands[2].ucBandNo = 38
        astCandBands[3].ucBandNo = 40  */
    NV_UE_EUTRA_FREQ_BAND_INFO_STRU       astCandBands[NV_ITEM_EUTRA_MAX_SUPPORT_BANDS_NUM];
}NV_UE_EUTRA_SUPPORT_FREQ_BAND_LIST_STRU;

/*****************************************************************************
 ?ṹ??    : NV_GUL_SUPPORT_FREQ_BAND_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ??¼SYSCFG????GUL????Ч??band?ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                       ucValidFlag;
    VOS_UINT8                                       aucReserved1[3];
    NV_UE_SUPPORT_FREQ_BAND_STRU                  stGuCfgBand;
    NV_UE_EUTRA_SUPPORT_FREQ_BAND_LIST_STRU       stLteCfgBand;
}NV_GUL_SUPPORT_FREQ_BAND_STRU;
/* gaojishan-SYSCFG-SAVENV-End */
/* gaojishan-nv-bands-cfg-begin */
#define NV_ITEM_EUTRA_MAX_BANDXX_CAND_BANDS_NUM    (12)
#define NV_ITEM_EUTRA_MAX_CUSTOM_BAND_WIDTH_NUM    (6)
#define NV_LTE_NV_MAX_SUPPORT_BAND                 (43)
#define NV_BAND_IND_64                             (64)
enum RRC_NV_D2DA_BANDXX_FUNC_FLAG_ENUM
{
    RRC_BANDXX_FUNC_INVALIED = 0,
    RRC_BANDXX_FUNC_JAPAN_CUSTOMER,
    RRC_BANDXX_FUNC_CUSTOMIZE_BAND_RANG
};
typedef VOS_UINT16 RRC_NV_D2DA_BANDXX_FUNC_FLAG_ENUM_UINT16;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_CUSTOM_BAND_INFO_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : band??????Ϣ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                         ucBandWidth;/* 0-5?ֱ???Ӧ:1.4m??3m??5m??10m??15m??20m */
    VOS_UINT8                         aucReserved1[3];
    RRC_NV_CSEL_FREQ_BAND_INFO_STRU   stCandBands;
}LRRC_NV_CUSTOM_BAND_INFO_STRU;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_BANDXX_CAND_INFO_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : band??????Ϣ?б?
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                       ucBandInds;
    VOS_UINT8                                       ucCustomBandInfoNum;
    VOS_UINT8                                       aucReserved1[2];
    LRRC_NV_CUSTOM_BAND_INFO_STRU                   astCustomBandInfoList[NV_ITEM_EUTRA_MAX_CUSTOM_BAND_WIDTH_NUM];
}LRRC_NV_BANDXX_CAND_INFO_STRU;

/*****************************************************************************
 ?ṹ??    : NV_ID_BANDXX_CAND_INFO_GROUP_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ????band???ƽṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                       ucValidFlag;
    VOS_UINT8                                       ucCandBandNum;
    VOS_UINT8                                       aucReserved1[2];
    LRRC_NV_BANDXX_CAND_INFO_STRU                   astCandBandInfoList[NV_ITEM_EUTRA_MAX_BANDXX_CAND_BANDS_NUM];
}NV_ID_BANDXX_CAND_INFO_GROUP_STRU;
/* gaojishan-nv-bands-cfg-end */

typedef struct
{
    VOS_UINT16                                      usMinRsrpThreshold;
    VOS_UINT16                                      usRsv;
}LRRC_NV_CSEL_GET_SIB_THRESHOLD_STRU;
typedef struct
{
    PS_BOOL_ENUM_UINT8    ucOffSetEnableFlg; /* offset NVʹ?ܱ???*/
    PS_BOOL_ENUM_UINT8    enR8BasedCellReselFlg;  /* ?Ƿ??ǻ???R8????ѡ׼??*/
    VOS_UINT8              aucReservel[2];         /* ????λ*/
    VOS_UINT16             usWRscpThresh;        /* W????С????RSCP???ޣ????ڴ????޲???ʹ??offset??ȡ????ֵ*/
    VOS_UINT16             usWEcnoThresh;        /* W????С????ECNO???ޣ????ڴ????޲???ʹ??offset??ȡ????ֵ*/
    VOS_UINT16             usEutranCellRsrpOffset;  /* EUTRANС????RSRP OFFSET*/
    VOS_UINT16             usEutranCellRsrqOffset;  /* EUTRANС????RSRQ OFFSET*/
    VOS_UINT16             usLteServRsrpOffSet;
    VOS_UINT16             usLteServRsrqOffSet;
    VOS_UINT16             usUtranRscpOffSet;
    VOS_UINT16             usUtranEcnoOffSet;
}NV_ID_APP_LRRC_RESEL_OFFSET_CFG_STRU;

/* gaojishan-fast-dorm-cfg-3 */
#define LRRC_NV_CTRL_PARA_U8_MAX_LEN                   (16)
#define LRRC_NV_CTRL_PARA_U16_MAX_LEN                  (8)
#define LRRC_NV_CTRL_PARA_U32_MAX_LEN                  (8)

/* gaojishan-fast-dorm-cfg-3 */
#define LRRC_CTRL_PARA_FAST_DORM_DELAY_BIT             (16)
#define LRRC_GET_CTRL_PARA_U8_INDEX( usIndex )         ( (usIndex))
#define LRRC_GET_CTRL_PARA_U16_INDEX( usIndex )        ( (usIndex)-LRRC_NV_CTRL_PARA_U8_MAX_LEN)
#define LRRC_GET_CTRL_PARA_U32_INDEX( usIndex )        ( (usIndex)-LRRC_NV_CTRL_PARA_U8_MAX_LEN - LRRC_NV_CTRL_PARA_U16_MAX_LEN )
#define LRRC_CTRL_PARA_IS_BIT_PRESENT( ulBitMap, ulBit )         ( (ulBitMap)&( 1<< (ulBit) ) )

typedef struct
{
    VOS_UINT32          bitValidMap ; /*BitMap(high--->low) U32,U16,U8,U8 */
    VOS_UINT8           astucCtrlPara[LRRC_NV_CTRL_PARA_U8_MAX_LEN];
    VOS_UINT16          astusCtrlPara[LRRC_NV_CTRL_PARA_U16_MAX_LEN];
    VOS_UINT32          astulCtrlPara[LRRC_NV_CTRL_PARA_U32_MAX_LEN];
}NV_ID_CTRL_PARA_STRU;

/* CMCC-BEGIN */
/*****************************************************************************
 ?ṹ??    : LTE_IRAT_REDIR_SWITCH_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ??ϵͳ?ض?????CMCC G2L?ض???4s???????ƿ??غ?G2L?????ض???2s???????ƿ???
            ɳ?? W2L?ض????????ض???2s???????ƿ??????ݽṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                   ucG2LRedirFlag;                 /* G2L?ض???4s???????ƿ??? */
    VOS_UINT8                                   ucG2LAutoRedirFlag;             /* G2L?????ض???2s???????ƿ??? */
    VOS_UINT8                                   ucW2LAutoRedirFlag;             /* W2L?????ض???2s???????ƿ??? */
    VOS_UINT8                                   aucReservd1[5];
    VOS_UINT16                                  usG2LRedirTimeLen;              /* G2L?ض???ʱ????Ĭ??ֵΪ4s????λΪms */
    VOS_UINT16                                  usG2LAutoRedirTimeLen;          /* G2L?????ض???ʱ????Ĭ??ֵΪ2s????λΪms */
    VOS_UINT16                                  usW2LAutoRedirTimeLen;          /* W2L?????ض???ʱ????Ĭ??ֵΪ2s????λΪms */
    VOS_UINT8                                   aucReservd2[6];
}LTE_IRAT_REDIR_SWITCH_STRU;
/* CMCC-END */
/*****************************************************************************
 ?ṹ??    : FC_LTE_TEMPERATURE_LIMIT_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ????LTE ?¿????ýṹ
*****************************************************************************/
#define FC_UL_THROUGHPUT_MIN_COUNT_NV          (0x03)
typedef struct
{
    VOS_UINT32          ulZeroSupported;
    VOS_UINT32          ulMinThroughput[FC_UL_THROUGHPUT_MIN_COUNT_NV];
} FC_LTE_TEMPERATURE_LIMIT_CONFIG_STRU;

/*****************************************************************************
 ?ṹ??    : LTE_APT_TOTAL_SWITCH
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : LTE NV?ܿ???
*****************************************************************************/
typedef struct
{
    VOS_UINT32          ulLteAptTotalSwitch;
    VOS_UINT32          ulRsv;
}LTE_APT_COMMONCTRL_STRU;

#define     LTE_MTC_ABAN_BAND_NUM_MAX   (8)
#define     LTE_MTC_ABAN_PLMN_NUM_MAX   (8)
#define     LTE_MTC_SPAC_BAND_NUM_MAX   (8)

typedef struct
{
    VOS_UINT32                          ulMcc;              /* MCC,3 bytes      */
    VOS_UINT32                          ulMnc;              /* MNC,2 or 3 bytes */
} LRRC_PLMN_ID_STRU;

/* ɨƵ?ų? band NV??Ϣ */
typedef struct
{
    LRRC_PLMN_ID_STRU                 stPlmnID;
    VOS_UINT32                       ulBandNum;                                    /* ???õ?Band???? */
    VOS_UINT8                        ucBand[LTE_MTC_ABAN_BAND_NUM_MAX];            /* ??¼????ҪɨƵ??Band */
} LTE_MTC_PLMN_ABAN_BAND_LIST;

/*Delete FDD Band */
typedef struct
{
    VOS_UINT8                     ucSwitchFlag;                                   /* ?????ܿؿ???,0 close; 1 open */
    VOS_UINT8                     ucReserved;
    VOS_UINT16                    usMcc;                                          /* Ĭ????д 460  */
}LTE_CUSTOM_MCC_INFO_STRU;

extern  LTE_CUSTOM_MCC_INFO_STRU g_stLRrcDelFddBand;
/*Delete FDD Band */

typedef struct
{
    VOS_UINT32                        ulPlmnNum;
    LTE_MTC_PLMN_ABAN_BAND_LIST       astAbandPlmnList[LTE_MTC_ABAN_PLMN_NUM_MAX]; /* Plmn??BandList */
} LTE_MTC_ABANDON_EUTRA_BAND_SCAN_STRU;


/* ɨƵ????band ??????Ϣ */
typedef struct
{
     VOS_UINT8                      ucBand;                                        /* ָʾBand??Ϣ */
     VOS_UINT8                      aucRsv[3];                                     /*????λ*/
     VOS_UINT16                     usFreqBegin;
     VOS_UINT16                     usFreqEnd;
}LTE_MTC_SPAC_BAND_CONFIG_LIST;

typedef struct
{
     VOS_UINT32                     ulBandNum;
     VOS_UINT32                     ulSpacBandSwitch;                               /* 0 close; 1 open*/
     LTE_MTC_SPAC_BAND_CONFIG_LIST  astSpacBandList[LTE_MTC_SPAC_BAND_NUM_MAX];     /* ??¼??ҪɨƵ???????õ?Band */
} LTE_MTC_SPAC_BAND_SCAN_CONFIG_STRU;
extern LTE_MTC_ABANDON_EUTRA_BAND_SCAN_STRU g_stAbandEutraBandInfo;
extern LTE_MTC_SPAC_BAND_SCAN_CONFIG_STRU   g_stSpacBandInfo;

/* MTC NOTCH add begin  */
typedef struct
{
     VOS_UINT16                     usSwitchFlag;                                   /* ?????ܿؿ??? */
     VOS_UINT8                      aucRcve[2];                                     /* ????λ */
} LTE_MTC_NOTCH_FEATURE_STRU;

/* MTC NOTCH add end  */
/*****************************************************************************
 ?ṹ??    : LTE_RRC_THRESHOLD_STRU
 Э??????  :
 ASN.1???? :
        ulCdrxPeriod:ʹ??Balong????ֵ
        lGeranBasiclag: ʹ??Balong????ֵ
        lGeranBasiStartThres:ʹ??Balong????ֵ
        ulCdrxFilterSwitch:ʹ??Balong????ֵ
        ulAbandinFristItraMeasFlg:??????һ????ϵͳ??��??????0:????????1:??????Balong????Ĭ??ֵ:0.
        ulTdsT315Len:  T315??ʱ?����ȣ???λ?롣Balong????Ĭ??ֵΪ0????????Ҫ???????ã?????ֵ 5.
        ulHrpdIdleMeasIndInterTime:hrpd?ϱ?????̬??��Ind??ʱ?????����?λ????.Balong????Ĭ??ֵ:200.
        ulHrpdConnMeasIndInterTime:hrpd?ϱ?��??̬??��Ind??ʱ?????����?λ????.Balong????Ĭ??ֵ:200.
        ultimerLrrcWaitCdmaCnfLen:?ȴ?hrpd??ѡ???ض???????IND??ʱ??ʱ??,??λ????.Balong????Ĭ??ֵ:30000.
        ulTReselectUtran: ?Զ???3G??ѡ??ʱ?����?.??λ:?롣Balong????Ĭ??ֵΪ0????????Ҫ???????ã?????ֵ 60.
        ulTReselectGsm: ?Զ???2G??ѡ??ʱ?����?.??λ:?롣Balong????Ĭ??ֵΪ0????????Ҫ???????ã?????ֵ 50.
 ?ṹ˵??  : LTE RRC ??ֵ
*****************************************************************************/
/* begin:Abandon Frist Itra Meas Result */
typedef struct
{
    VOS_UINT32          ulCdrxPeriod;
    VOS_INT32           lGeranBasiclag;
    VOS_INT32           lGeranBasiStartThres;
    VOS_UINT32          ulCdrxFilterSwitch;
    VOS_UINT32          ulAbandinFristItraMeasFlg;
    VOS_UINT32          ulTdsT315Len;
    VOS_UINT32          ulHrpdIdleMeasIndInterTime;
    VOS_UINT32          ulHrpdConnMeasIndInterTime;
    VOS_UINT32          ultimerLrrcWaitCdmaCnfLen;
    VOS_UINT32          ulTReselectUtran;
    VOS_UINT32          ulTReselectGsm;
    /* mod for FreqSearchEnhance begin */
    VOS_UINT32          ulFreqSearchEnhanceFlag;
    VOS_UINT32          ulFreqSearchEnhanceSrchCnt;
    VOS_UINT32          ulFreqSearchEnhanceSrchThres;
    /* mod for FreqSearchEnhance end */
    VOS_UINT32          ulCampFailRetryNum;
    VOS_UINT32          ulBgsL2GuSearchIndThreshFlag;
    VOS_INT32           lBgsL2GuSearchIndThresh;
    VOS_UINT32          aulRsv1;
    VOS_UINT32          aulRsv2;
    VOS_UINT32          aulRsv3;
    VOS_UINT32          aulRsv4;
    VOS_UINT32          aulRsv5;
    VOS_UINT32          aulRsv6;
    VOS_UINT32          aulRsv7;
    VOS_UINT32          aulRsv8;
    VOS_UINT32          aulRsv9;
    VOS_UINT32          aulRsv10;
    VOS_UINT32          aulRsv11;
    VOS_UINT32          aulRsv12;
    VOS_UINT32          aulRsv13;
    VOS_UINT32          aulRsv14;
    VOS_UINT32          aulRsv15;
}LTE_RRC_THRESHOLD_STRU;
extern VOS_UINT32   g_ulAbandinFristItraMeasFlg;
/* end:Abandon Frist Itra Meas Result */

extern VOS_UINT8    g_ucTReselectUtran;
extern VOS_UINT8    g_ucTReselectGsm;

#define         MAX_IRAT_TDS_UE_CAPABILITY_LENGHT           (100)
typedef struct
{
     VOS_UINT16                     usSwitchFlag;                                   /* ?????ܿؿ???: Ĭ?ϴ???ʹ??NV???õ?ue??��????????????????ʹ???Լ?????????*/
     VOS_UINT16                     usLength;                                       /* UE??��???����? */
     VOS_UINT8                      aucCapInfo[MAX_IRAT_TDS_UE_CAPABILITY_LENGHT];                                /* UE??��???? */
} LTE_IRAT_TDS_UE_CAPABILITY_STRU;
/*****************************************************************************
 ?ṹ??    : LTE_RRC_EXTBANDINFO_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : Band28ȫƵ??????????NV
             ucflag:1??ʾ��չBAND???ܴ򿪣?0??ʾ?ر?
             ucBandnum:??ʾ?м???BAND??Ҫ?õ?��չBAND??
             aucRsv[]:Ԥ??
             aucBandInd[]:??Ҫ��չBAND?ŵ?BAND?б?
*****************************************************************************/
typedef struct
{
    VOS_UINT8          ucflag;
    VOS_UINT8          ucBandnum;
    VOS_UINT8          aucRsv[2];
    VOS_UINT8          aucBandInd[8];
}LTE_RRC_EXTBANDINFO_STRU;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_EXT_BAND_LIST_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : ?ڲ?ͬ?????µķֶ?Ƶ??NV?ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usBandInd;      /* ??????BAND ??*/
    VOS_UINT16                          usScellBandInd;      /* Scell??BAND ??*/
    
	/* Band?ں򲹴???Ƶ????Ϣ */
    LRRC_NV_SUPPORT_BAND_LIST_STRU      astBandInfoWithBandWidth[6];/*RRC_BAND_WIDTH_BUTT*/
}LRRC_NV_EXT_BAND_LIST_STRU;

/*****************************************************************************
 ?ṹ??    : LTE_RRC_EXTBANDINFO_WITH_BANDWIDTH_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : UE֧?ֵ?Ƶ???ڲ?ͬ?????µķֶ?Ƶ??NV?ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8          ucActive;
    VOS_UINT8          ucScellEnable;
    VOS_UINT16         usBandCount;
    LRRC_NV_EXT_BAND_LIST_STRU          stCandBandsInfo[8]; /*֧??????8??Ƶ???Ʒ?Ƶ*/
}LTE_RRC_EXTBANDINFO_WITH_BANDWIDTH_STRU;

/*****************************************************************************
 ?ṹ??    : LRRC_NV_DSDS_CONFIG_STRU
 Э??????  :
 ASN.1???? :
 ?ṹ˵??  : DSDS????????NV
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usRsv0;
    VOS_UINT16                          usDsdsPagingTimeLen;
    VOS_UINT16                          usDsdsShrtOccpRfRcverTimeLen;
    VOS_UINT16                          usRsv;
    VOS_UINT32                          ulRsv0;
    VOS_UINT32                          ulRsv1;
    VOS_UINT32                          ulRsv2;
    VOS_UINT32                          ulRsv3;
}LRRC_NV_DSDS_CONFIG_STRU;
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
extern PS_BAND_ENUM_UINT8 g_enBandInd;
extern VOS_UINT8 g_ucBandInd;
extern  VOS_UINT32  g_enBandHI6360NvId[PS_BAND_BUTT][PS_NV_IDX_BUTT];
#if 0
extern VOS_UINT32  g_enBandNvId[PS_BAND_BUTT][PS_NV_IDX_BUTT];
#endif
#if (VOS_OS_VER != VOS_WIN32)
/*?ر?TA Timer?Ŀ??أ?0:???عرգ???TA Timer??Ч 1:???ش򿪣???TA Timer????Ч*/
extern VOS_UINT32  g_ulCloseTaTimer;

/*?ر?SR?????????????Ŀ??أ?0:???عرգ???SR??????????????Ч 1:???ش򿪣???SR???????????벻??Ч*/
extern VOS_UINT32  g_ulCloseSrRandFlag;

/*?ر?SR???????ƿ??أ?0:???عرգ???SR???Թ???BSR????Ϊǰ?? 1:???ش򿪣???SR?Թ???BSR????Ϊǰ??*/
extern VOS_UINT32  g_ulSrTrigFlag;

/*?????Ƿ񽫰?????Ϣת??һ????PC?Խ???????*/
extern VOS_UINT32  g_ulIfForwardToPc;

/* ???ܵĿ???:0-?򿪼??ܣ?1-?رռ???*/
extern VOS_UINT32  g_ulSmcControl;

/* DCM??????????GU??L???л??????Ǳ???TA?Ŀ??? */
extern VOS_UINT32  g_ulHoIgnoreForbidTaFlag;

/*?ⳡ????׮???뿪?? 0:?ر?  1:????.Ĭ??Ϊ?ر?*/
extern VOS_UINT32 g_ulFieldTestSwitch;

/*????eNB??TDD?Ľ???????*/

/*?ж??Ƿ?BARС??*/
extern VOS_UINT32 g_ulCloseBarCell;

/* DRX Control Flag  0: Close 1: Open */
extern VOS_UINT32 g_ulDrxControlFlag;

/* ????????֡???ȣ?Ĭ??Ϊ1 */
extern VOS_UINT32 g_ulCfgSubFrameAssign;

/* ??????֡???ã?Ĭ??Ϊ7 */
extern VOS_UINT32 g_ulSubFramePattern;

/* ???߸??? */
extern VOS_UINT32 g_ulPsCfgDspAntCnt;

#if 0
extern VOS_UINT32 g_ulPsCfgDspRatMod;
#endif

/* PCO???ܿ??أ?0Ϊ?رգ?1Ϊ?򿪡?Ĭ??Ϊ???? */
extern VOS_UINT32 g_ulPcoFlag;

/* ɾ??SDFʱ?Ƿ?Я??Qos??Ϣ???ܿ??أ?0Ϊ?رգ?1Ϊ?򿪡?Ĭ??Ϊ?ر? */
extern VOS_UINT32 g_ulDelSdfWithQosFlag;

/* UE??ȫ??��У?鿪?أ?0Ϊ?򿪣?8Ϊ?رա?Ĭ??Ϊ???? */
extern VOS_UINT32 g_ulUeSecuCapMatch;

/* TAU??Ϣ???ܿ??أ?0Ϊ???ܣ?1Ϊ?????ܡ?Ĭ??Ϊ?????? */
extern VOS_UINT32 g_ulTauCipheredFlag;

/*?????㷨????*/
extern VOS_UINT32  g_ulFlowCtrlFlag;

/*???ص???CPUʣ??Ŀ??ֵ*/
extern VOS_UINT32  g_ulFcPdcpTarget;

/*©Ͱ?㷨??????ֵ*/
extern VOS_UINT32   g_ulTargetIPMax;
extern VOS_UINT32   g_ulSendSduMax;

/*????©Ͱ?㷨??ֵ*/
extern VOS_UINT32  g_ulDlSduMax;
extern VOS_UINT32  g_ulULIpMax;

/*©Ͱ?㷨?Ĳ?��???ڣ???λms*/
extern VOS_UINT32  g_ulMeasGap;
extern VOS_UINT32  g_ulSduGap;
/*???س?ʼ??ѡ??*/
extern VOS_UINT32  g_ulFcInitFlag ;

/*???ڶ?Msg4 6??ͷ??ʽ??CR?޸?ǰ?Ľ??????���1??ʾ???Խ??????????ص?Msg4ͷ??
??ʾֻ?ܽ???????Э???涨??6?ָ?ʽ??Ĭ????0*/
extern VOS_UINT32  g_ulMsg4ForOldCrFlag;

/* GCF?Ƿ?֧??24.301 Table D.1.1?н?��ԭ?????? 0x00000004,֧?֣?0??֧?? */
extern VOS_UINT32 g_ulGcfEstCauseFlag;

/* SMC?????Ƿ?֧?ֲ???ģʽ??ȱʡ֧?֣?ȡֵΪ0x00000008 */
extern VOS_UINT32  g_ulSmcFuncTestMode;

/* GCF??׮ͨ??CDRX??????????ʵ?ʳ???Ϊ?˽????Ĳ??򿪸ÿ??أ?λ?򿪹?,0??ʾĬ?ϲ??򿪶?Ӧ?ⳡ????
1??ʾ?򿪶?ӦGCF???ԣ?Ĭ??ֵ??0
extern VOS_UINT32  g_ulGcfConnDrxStubFlag;*/

#endif
/* ??Ʒѡ?񿪹? */
extern VOS_UINT32  g_ulPsSupportBand64Flg;
extern VOS_UINT32  g_ulPsPagingConnRelFlg;
extern VOS_UINT32  g_ulPsUlDrbDiscardFlag;
extern VOS_UINT32  g_ulMbmsSupportSwitch;
extern VOS_UINT32  g_ulPsBandwidthSupportFlg;
/* modify by lishangfeng freq all begin */
extern VOS_UINT32  g_ulPsFreqInAllBandWidthFlg;

extern VOS_UINT32  g_ulHoFailFlag;
extern VOS_UINT16  g_usHoFailT304;
/* gaojishan-SYSCFG-SAVENV-Begin */
extern VOS_UINT32  gulJpSbmDualImsiSwitch;
extern VOS_UINT32  gulLteDelWcdmaBandInJapan;           /*NV?????У???Ҫɾ????*/

/*Begin: for sbm delete wcdma band9  */
extern VOS_UINT8       gucJpUband1MncCnt;
extern VOS_UINT8       gaucJpUband1Mnc[3];
extern VOS_UINT8       gaucJpUband1Mcc[3];
extern VOS_UINT8       gaucJpUband2Mcc[3];
/*End: for sbm delete wcdma band9  */
/* gaojishan-SYSCFG-SAVENV-End */
extern VOS_UINT32  g_ulLReestIratCellSrchFlg;
/* gaojishan-nccpermit */
extern VOS_UINT32 g_ulNccPermitFlag;
extern VOS_UINT32  g_ulLBgsSupportFlg;
/* gaojishan-reest-measGap*/
extern VOS_UINT32  g_ulLBgsLSupportFlg;

extern VOS_UINT32 g_ulLTEReestMeasGapSetupFlag;
extern VOS_UINT32 g_ulLTEConnToIdleFlag;


/* gaojishan-PLMN_EXACTLY_COMPARE_FLG */
extern VOS_UINT32 g_ulPlmnExactCmpFlg;
/* gaojishan-SoftBank-???Ʊ?־:true,set Band41 range */
extern VOS_UINT32 gulSoftBankFlag ;
/* gaojishan-nccpermit-2 */
extern VOS_UINT32 g_ulNoIdleNccPermitFlag;

/* gaojishan-dcom-resel-cfg */
extern VOS_UINT32 g_ulLteJpDcomUseR8Flag;
extern VOS_UINT32 g_ulLteJpDccomRelOffSetFlag;
extern VOS_UINT32 g_ulLteUtranEvaFlag;
/* gaojishan-fast-dorm-cfg */
extern VOS_UINT32 g_ulLteJpFastdormFlag;
/* gaojishan-fast-dorm-cfg-3 */
extern VOS_UINT32 g_usLteJpFastdormDelayValue;

/*h00159435 TdsDataInfoNotify Begin*/
extern VOS_UINT32 g_ulDlCtrlPduNotifyFlag;
/*h00159435 TdsDataInfoNotify End*/

/* gaojishan-CMAS-Begin */
/* >0: cmas report after rrc auth(after camp in fact); 0:cmas report even when cell searching*/
extern VOS_UINT32                              gul_CmasRptBeforeAuthSwitch;
/* gaojishan-CMAS-End */

extern VOS_UINT32 g_ulSscSupportFlag;
extern VOS_UINT32 g_ulAdrxSupportFlag;
/* niuxiufan taiwan modify begin */
extern VOS_UINT32 g_ulRedirNotBandScanFlg;
/* niuxiufan taiwan modify end */
/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/










#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of PsNvInterface.h */

