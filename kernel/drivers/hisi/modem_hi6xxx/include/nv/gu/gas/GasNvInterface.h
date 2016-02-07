/******************************************************************************

  Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : GasNvInterface.h
  Description     : GasNvInterface.h header file
  History         :

******************************************************************************/

#ifndef __GASNVINTERFACE_H__
#define __GASNVINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(2)
#else
#pragma pack(push, 2)
#endif

#include "vos.h"

/*****************************************************************************
  2 Macro
*****************************************************************************/
#define NVIM_ULTRA_CLASSMARK_LEN                            (24)
#define NVIM_BAND_PWR_LEN                                   (8)
#define NVIM_CLASSMARK1_LEN                                 (2)
#define NVIM_CLASSMARK2_LEN                                 (4)
#define NVIM_CLASSMARK3_LEN                                 (16)
#define NVIM_CLASSMARK3_R8_LEN                              (36)
#define NVIM_GCF_ITEM_LEN                                   (80)
#define NVIM_GSM_BA_MAX_SIZE                                (33)
#define NVIM_EGPRS_RA_CAPABILITY_DATA_LEN                   (53)
#define NVIM_PREFER_GSM_PLMN_LIST_LEN                       (976)
#define NVIM_GSM_DEC_FAIL_ARFCN_LIST_LEN                    (120)
#define NVIM_EUTRA_MAX_SUPPORT_BANDS_NUM                    (64)
#define NVIM_TDS_MAX_SUPPORT_BANDS_NUM                      (8)
#define NVIM_EUTRA_CAPA_COMM_INFO_SIZE                      (260)
#define NVIM_CBS_MID_LIST_LEN                               (2004)
#define NVIM_CBS_MID_RANGE_LIST_LEN                         (2004)
/* Added by yangsicong for L2G REDIR C1 CUSTUME, 2015-1-26, begin */
#define NVIM_GAS_C1_CALC_OPT_PLMN_WHITE_LIST_CNT_MAX        (20)
/* Added by yangsicong for L2G REDIR C1 CUSTUME, 2015-1-26, end */

#define NVIM_GAS_GSM_BAND_CUSTOMIZE_LIST_MAX_CNT            (80)

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
enum NVIM_TDS_FREQ_BAND_LIST_ENUM
{
    ID_NVIM_TDS_FREQ_BAND_A         = 0x01,         /* 频点范围: 9504~9596  10054~10121 */
    ID_NVIM_TDS_FREQ_BAND_B         = 0x02,         /* 频点范围: 9254~9546  9654~9946 */
    ID_NVIM_TDS_FREQ_BAND_C         = 0x04,         /* 频点范围: 9554~9646 */
    ID_NVIM_TDS_FREQ_BAND_D         = 0x08,         /* 频点范围: 12854~13096 */
    ID_NVIM_TDS_FREQ_BAND_E         = 0x10,         /* 频点范围: 11504~11996 */
    ID_NVIM_TDS_FREQ_BAND_F         = 0x20,         /* 频点范围: 9404~9596 */
    ID_NVIM_TDS_FREQ_BAND_BUTT
};
typedef VOS_UINT8  NVIM_TDS_FREQ_BAND_LIST_ENUM_UINT8;

/*****************************************************************************
  5 STRUCT
*****************************************************************************/
/*****************************************************************************
*                                                                            *
*                           参数设置消息结构                                 *
*                                                                            *
******************************************************************************/

/*****************************************************************************
 结构名    : NVIM_ULTRA_CLASSMARK_STRU
 结构说明  : en_NV_Item_Ultra_Classmark 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucUltraClassmark[NVIM_ULTRA_CLASSMARK_LEN];
}NVIM_ULTRA_CLASSMARK_STRU;

/*****************************************************************************
 结构名    : NVIM_GAS_MULTIRATE_FLAG_STRU
 结构说明  : en_NV_Item_Gas_MultiRateFlag 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usMultiRateFlag;
}NVIM_GAS_MULTIRATE_FLAG_STRU;

/*****************************************************************************
 结构名    : NVIM_BAND_PWR_STRU
 结构说明  : en_NV_Item_Band_Pwr 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucBandPwr[NVIM_BAND_PWR_LEN];
}NVIM_BAND_PWR_STRU;

/*****************************************************************************
 结构名    : NVIM_VGCS_FLAG_STRU
 结构说明  : en_NV_Item_Vgcs_Flag 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usVgcsFlag;
}NVIM_VGCS_FLAG_STRU;

/*****************************************************************************
 结构名    : NVIM_EGPRS_MULTI_SLOT_CLASS_STRU
 结构说明  : en_NV_Item_Egprs_Multi_Slot_Class 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usEgprsMultiSlotClass;                  /* Range: [0,12] */
}NVIM_EGPRS_MULTI_SLOT_CLASS_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_CLASSMARK1_STRU
 结构说明  : en_NV_Item_Gsm_Classmark1 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucGsmClassmark1[NVIM_CLASSMARK1_LEN];
}NVIM_GSM_CLASSMARK1_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_CLASSMARK2_STRU
 结构说明  : en_NV_Item_Gsm_Classmark2 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucGsmClassmark2[NVIM_CLASSMARK2_LEN];
}NVIM_GSM_CLASSMARK2_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_CLASSMARK3_STRU
 结构说明  : en_NV_Item_Gsm_Classmark3 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucGsmClassmark3[NVIM_CLASSMARK3_LEN];
}NVIM_GSM_CLASSMARK3_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_IND_FREQ_STRU
 结构说明  : en_NV_Item_Gsm_Ind_Freq 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usGsmIndFreq;
}NVIM_GSM_IND_FREQ_STRU;

/*****************************************************************************
 结构名    : NVIM_GCF_ITEM_STRU
 结构说明  : en_NV_Item_GCF_Item 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucGcfItem[NVIM_GCF_ITEM_LEN];
}NVIM_GCF_ITEM_STRU;

/*****************************************************************************
 结构名    : NVIM_G2W_RSSI_RSCP_OFFSET_STRU
 结构说明  : en_NV_Item_G2W_RSSI_RSCP_OFFSET 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usG2WRssiRscpOffset;
}NVIM_G2W_RSSI_RSCP_OFFSET_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_BA_COUNT_STRU
 结构说明  : en_NV_Item_GSM_Ba_Count 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usGsmBaCount;
}NVIM_GSM_BA_COUNT_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_BA_LIST_STRU
 结构说明  : en_NV_Item_GSM_Ba_List 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulMcc;
    VOS_UINT32                          ulMnc;
    VOS_UINT16                          usArfcn[NVIM_GSM_BA_MAX_SIZE];
}NVIM_GSM_BA_LIST_STRU;

/*****************************************************************************
 结构名    : NVIM_EGPRS_FLAG_STRU
 结构说明  : en_NV_Item_Egprs_Flag 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usEgprsFlag;
}NVIM_EGPRS_FLAG_STRU;

/*****************************************************************************
 结构名    : NVIM_EGPRS_RA_CAPABILITY_STRU
 结构说明  : en_NV_Item_EgprsRaCapability 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucLength;
    VOS_UINT8                           aucEgprsRaCapabilityData[NVIM_EGPRS_RA_CAPABILITY_DATA_LEN];
}NVIM_EGPRS_RA_CAPABILITY_STRU;

/*****************************************************************************
 结构名    : NVIM_PREFER_GSM_PLMN_COUNT_STRU
 结构说明  : en_NV_Item_Prefer_GSM_PLMN_Count 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usPreferGsmPlmnCount;
}NVIM_PREFER_GSM_PLMN_COUNT_STRU;

/*****************************************************************************
 结构名    : NVIM_PREFER_GSM_PLMN_LIST_STRU
 结构说明  : en_NV_Item_Prefer_GSM_PLMN_List 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          ausPreferGsmPlmnListData[NVIM_PREFER_GSM_PLMN_LIST_LEN];
}NVIM_PREFER_GSM_PLMN_LIST_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_DEC_FAIL_ARFCN_COUNT_STRU
 结构说明  : en_NV_Item_GSM_DEC_FAIL_ARFCN_Count 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usGsmDecFailArfcnCount;
}NVIM_GSM_DEC_FAIL_ARFCN_COUNT_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_DEC_FAIL_ARFCN_LIST_STRU
 结构说明  : en_NV_Item_GSM_DEC_FAIL_ARFCN_List 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usGsmDecFailArfcnList[NVIM_GSM_DEC_FAIL_ARFCN_LIST_LEN];
}NVIM_GSM_DEC_FAIL_ARFCN_LIST_STRU;

/*****************************************************************************
 结构名    : NVIM_PREFER_GSM_PLMN_SWITCH_STRU
 结构说明  : en_NV_Item_Prefer_GSM_PLMN_Switch 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usPreferGsmPlmnSwitch;
}NVIM_PREFER_GSM_PLMN_SWITCH_STRU;

/*****************************************************************************
 结构名    : NVIM_GERAN_FEATURE_PACKAGE1_STRU
 结构说明  : en_NV_Item_Geran_Feature_Package1 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usGeranFeaturePackage1;                 /* Range: [0,1] */
}NVIM_GERAN_FEATURE_PACKAGE1_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_A5_STRU
 结构说明  : en_NV_Item_Gsm_A5 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usGsmA5;                                /* Range: [0,7] */
}NVIM_GSM_A5_STRU;

/*****************************************************************************
 结构名    : NVIM_LOW_COST_EDGE_FLAG_STRU
 结构说明  : en_NV_Item_LowCostEdge_Flag 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usLowCostEdgeFlag;                      /* Range: [0,1] */
}NVIM_LOW_COST_EDGE_FLAG_STRU;

/*****************************************************************************
 结构名    : NVIM_GPRS_ACTIVE_TIMER_LENGTH_STRU
 结构说明  : en_NV_Item_GPRS_ActiveTimerLength 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulGprsActiveTimerLength;
}NVIM_GPRS_ACTIVE_TIMER_LENGTH_STRU;

/*****************************************************************************
 结构名    : NVIM_GPRS_MULTI_SLOT_CLASS_STRU
 结构说明  : en_Nv_Item_Gprs_Multi_Slot_Class 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usGprsMultiSlotClass;                   /* Range: [0,12] */
}NVIM_GPRS_MULTI_SLOT_CLASS_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_PLMN_SEARCH_ARFCN_MAX_NUM_STRU
 结构说明  : en_NV_Item_GSM_PLMN_SEARCH_ARFCN_MAX_NUM 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usGsmPlmnSearchArfcmMaxNum;             /* Range: [0x1,0x8C] */
}NVIM_GSM_PLMN_SEARCH_ARFCN_MAX_NUM_STRU;

/*****************************************************************************
 结构名    : NVIM_GCBS_CONF_STRU
 结构说明  : en_Nv_Item_GCBS_Conf 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usECBCHSwitch;              /* Range: [0,1] */
    VOS_UINT16                                      usDrxSwitch;                /* Range: [0,1] */
    VOS_UINT32                                      ulGCBSActiveTimerLength;
}NVIM_GCBS_CONF_STRU;

/*****************************************************************************
 结构名    : NVIM_REPEATED_ACCH_CAPABILITY_STRU
 结构说明  : en_NV_Item_Repeated_Acch_Capability 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usRepeatedAcchCapability;
}NVIM_REPEATED_ACCH_CAPABILITY_STRU;

/*****************************************************************************
 结构名    : NVIM_ES_IND_STRU
 结构说明  : en_NV_Item_ES_IND 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usEsInd;                    /* Range: [0,1] */
}NVIM_ES_IND_STRU;

/*****************************************************************************
 结构名    : NVIM_GPRS_EXTENDED_DYNAMIC_ALLOCATION_CAPABILITY_STRU
 结构说明  : en_NV_Item_GPRS_Extended_Dynamic_Allocation_Capability 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usGprsExtDynAllocCap;       /* Range: [0,1] */
}NVIM_GPRS_EXTENDED_DYNAMIC_ALLOCATION_CAPABILITY_STRU;

/*****************************************************************************
 结构名    : NVIM_EGPRS_EXTENDED_DYNAMIC_ALLOCATION_CAPABILITY_STRU
 结构说明  : en_NV_Item_EGPRS_Extended_Dynamic_Allocation_Capability 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usEGprsExtDynAllocCap;      /* Range: [0,1] */
}NVIM_EGPRS_EXTENDED_DYNAMIC_ALLOCATION_CAPABILITY_STRU;

/*****************************************************************************
 结构名    : NVIM_REVISION_LEVEL_INDICATOR_STRU
 结构说明  : en_NV_Item_Revision_Level_Indicator 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usRevLvlInd;                /* Range: [0,1] */
}NVIM_REVISION_LEVEL_INDICATOR_STRU;

/*****************************************************************************
 结构名    : NVIM_DOWNLINK_ADVANCED_RECEIVER_PERFORMANCE_STRU
 结构说明  : en_NV_Item_Downlink_Advanced_Receiver_Performance 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usDlAdvRcvPer;              /* Range: [0,1] */
}NVIM_DOWNLINK_ADVANCED_RECEIVER_PERFORMANCE_STRU;

/*****************************************************************************
 结构名    : NVIM_EXT_RLC_MAC_CTRL_MSG_SEGMENT_CAPABILITY_STRU
 结构说明  : en_NV_Item_Ext_RLC_MAC_Ctrl_Msg_Segment_Capability 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usExtRlcMacCtrlMsgSegCap;   /* Range: [0,1] */
}NVIM_EXT_RLC_MAC_CTRL_MSG_SEGMENT_CAPABILITY_STRU;

/*****************************************************************************
 结构名    : NVIM_PS_HANDOVER_CAPABILITY_STRU
 结构说明  : en_NV_Item_PS_Handover_Capability 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usPsHandoverCapability;     /* Range: [0,1] */
}NVIM_PS_HANDOVER_CAPABILITY_STRU;

/*****************************************************************************
 结构名    : NVIM_GAS_WEAK_SIGNAL_THREHOLD_STRU
 结构说明  : en_NV_Item_GAS_Errorlog_Energy_Threshold 结构
*****************************************************************************/
typedef struct
{
    VOS_INT16                                       sWeakSignalThreshold;
    VOS_UINT8                                       aucReserve[2];
}NVIM_GAS_WEAK_SIGNAL_THREHOLD_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_MULTIRATE_CAP_STRU
 结构说明  : en_NV_Item_GSM_Multirate_Capability 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                       ucGsmMultirateCap;
    VOS_UINT8                                       aucRsv[3];
}NVIM_GSM_MULTIRATE_CAP_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_CLASSMARK3_R8_STRU
 结构说明  : en_NV_Item_Gsm_Classmark3_R8 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                       aucGsmClassMark3Data[NVIM_CLASSMARK3_R8_LEN];
}NVIM_GSM_CLASSMARK3_R8_STRU;

/*****************************************************************************
 结构名    : NVIM_EUTRA_MEAS_AND_REPORTING_SUPPORT_FLG_STRU
 结构说明  : en_NV_Item_EUTRA_MEAS_AND_REPORTING_SUPPORT_FLG 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usLteMeasSupportedFlg;      /* Range: [0,1] */
}NVIM_EUTRA_MEAS_AND_REPORTING_SUPPORT_FLG_STRU;

/*****************************************************************************
 结构名    : NVIM_PRI_BASED_RESEL_SUPPORT_FLG_STRU
 结构说明  : en_NV_Item_PRI_BASED_RESEL_SUPPORT_FLG 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usPriBasedReselSupportFlg;  /* Range: [0,1] */
}NVIM_PRI_BASED_RESEL_SUPPORT_FLG_STRU;

/*****************************************************************************
 结构名    : NVIM_GERAN_TO_EUTRA_SUPPORT_IN_TRANSFER_MODE_STRU
 结构说明  : en_NV_Item_GERAN_TO_EUTRA_SUPPORT_IN_TRANSFER_MODE 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                      usLteSupportInTransferMode; /* Range: [0,3] */
}NVIM_GERAN_TO_EUTRA_SUPPORT_IN_TRANSFER_MODE_STRU;

/*****************************************************************************
 结构名    : NVIM_UE_EUTRA_FREQ_BAND_INFO_STRU
 结构说明  : 用于描述一个 LTE 频段
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucBandNo;
    VOS_UINT8                           ucDuplexModeFlg;
    VOS_UINT8                           aucReserve[2];
}NVIM_UE_EUTRA_FREQ_BAND_INFO_STRU;

/*****************************************************************************
 结构名    : NVIM_UE_EUTRA_SUPPORT_FREQ_BAND_LIST_STRU
 结构说明  : en_NV_Item_EUTRA_CAPA_COMM_INFO 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usBandCnt;
    VOS_UINT8                               aucReserved1[2];
    NVIM_UE_EUTRA_FREQ_BAND_INFO_STRU       astCandBands[NVIM_EUTRA_MAX_SUPPORT_BANDS_NUM];
}NVIM_UE_EUTRA_SUPPORT_FREQ_BAND_LIST_STRU;

/*****************************************************************************
 结构名    : NVIM_GAS_HIGH_MULTISLOT_CLASS_STRU
 结构说明  : en_NV_Item_GAS_High_Multislot_Class 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usHighMultislotClassFlg;
    VOS_UINT16                              usHighMultislotClass;
}NVIM_GAS_HIGH_MULTISLOT_CLASS_STRU;

/*****************************************************************************
 结构名    : NVIM_GPRS_NON_DRX_TIMER_LENGTH_STRU
 结构说明  : en_NV_Item_GPRS_Non_Drx_Timer_Length 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usNonDrxTimerLen;                   /* Range: [0,7] */
}NVIM_GPRS_NON_DRX_TIMER_LENGTH_STRU;

/*****************************************************************************
 结构名    : NVIM_UE_TDS_SUPPORT_FREQ_BAND_LIST_STRU
 结构说明  : en_NV_Item_UTRAN_TDD_FREQ_BAND 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               ucBandCnt;                          /* Range: [0,6] */
    VOS_UINT8                               aucReserved[3];
    VOS_UINT8                               aucBandNo[NVIM_TDS_MAX_SUPPORT_BANDS_NUM];
}NVIM_UE_TDS_SUPPORT_FREQ_BAND_LIST_STRU;

/*****************************************************************************
 结构名    : NVIM_QSEARCH_CUSTOMIZATION_STRU
 结构说明  : en_NV_Item_QSearch_Customization 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usQSearchCustomMode;
}NVIM_QSEARCH_CUSTOMIZATION_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_POOR_RXQUAL_THRESHOLD_STRU
 结构说明  : en_NV_Item_Gsm_Poor_RxQual_ThresHold 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usRrPoorRxQualThresHold;
}NVIM_GSM_POOR_RXQUAL_THRESHOLD_STRU;

/*****************************************************************************
 结构名    : NVIM_CSFB_CUSTOMIZATION_STRU
 结构说明  : en_NV_Item_Csfb_Customization 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usCsfbCustomization;
}NVIM_CSFB_CUSTOMIZATION_STRU;

/*****************************************************************************
 结构名    : NVIM_CBS_MID_LIST_STRU
 结构说明  : en_NV_Item_CBS_MID_List 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               aucCbsMidList[NVIM_CBS_MID_LIST_LEN];
}NVIM_CBS_MID_LIST_STRU;

/*****************************************************************************
 结构名    : NVIM_CBS_MID_RANGE_LIST_STRU
 结构说明  : en_NV_Item_CBS_MID_Range_List 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               aucCbsMidRangeList[NVIM_CBS_MID_RANGE_LIST_LEN];
}NVIM_CBS_MID_RANGE_LIST_STRU;


typedef struct
{
   VOS_UINT8                            ucFrLteMeasFlag;                        /* 是否开启GSM自主重定向到LTE的LTE频点测量，0: 关闭, 1: 开启，默认值为1 */       
   VOS_UINT8                            ucMaxSavedMeasTimes;                    /* 最大保存的测量次数，默认值为8，范围:[0,8] */  

   VOS_UINT8                            ucFrInvalidMeasFlag;                    /* 是否开启用测量结果判断GSM自主重定向到LTE是否有效，0: 关闭, 1: 开启，默认值为1 */ 
   VOS_UINT8                            ucMaxInvalidMeasTimes;                  /* 最大无效测量次数，用来判定频点是否有效，不大于最大保存的测量次数，默认值为8，范围:[0,8] */
}NVIM_GSM_AUTO_FR_LTE_MEAS_CONFIG_STRU;


typedef struct
{
   VOS_UINT8                            ucC1CustomizeSwitchFlag;          /* 是否开启路损C1定制，0: 关闭, 1: 开启，默认值为0 */       
   VOS_UINT8                            ucRxlevAccessMin;                 /* 开启路损C1定制后，定制的最小接入电平等级，默认等级为8，范围:[0,63] */  
   VOS_UINT8                            ucC1ValueThreshold;               /* C1阈值 */
   VOS_UINT8                            aucRsv[1];
}NVIM_GSM_C1_CUSTOMIZE_CFG_STRU;

/*****************************************************************************
 结构名    : NVIM_GSM_NFREQ_THRESHOLD_STRU
 结构说明  : en_NV_Item_Gsm_NFreq_Threshold 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                            ucFirstNFreqThreshold;                 /* 邻频干扰过滤门限 */
    VOS_UINT8                            aucRsv[3];
}NVIM_GSM_NFREQ_THRESHOLD_STRU;

/*****************************************************************************
 结构名    : NVIM_GAS_W_NON_NCELL_MEAS_CTRL_STRU
 结构说明  : en_NV_Item_Gas_W_Non_NCell_Meas_Ctrl 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                            ucIdleNonNCellMeasEnable;              /* 空闲态下是否启动非邻区测量 */
    VOS_UINT8                            ucTransferNonNCellMeasEnable;          /* 传输态下是否启动非邻区测量 */
    VOS_INT16                            sRscpThreshold;                        /* 启动非邻区测量的 RSCP 门限 */
    VOS_INT16                            sEcn0Threshold;                        /* 启动非邻区测量的 ECN0 门限 */
    VOS_INT16                            sRssiThreshold;                        /* 启动非邻区测量的 RSSI 门限 */
}NVIM_GAS_W_NON_NCELL_MEAS_CTRL_STRU;
/* Added by l67237 for Gas Dsds Feature, 2014-3-4, begin */
/*****************************************************************************
 结构名    : NV_GSM_RF_UNAVAILABLE_CFG_STRU
 结构说明  : en_NV_Item_GSM_RF_UNAVAILABLE_CFG 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucStartSearchFlag;                      /* 是否启动搜网:,1:是; 0:否 */
    VOS_UINT8                           ucSearchTimerLen;                       /* 多长时间启动搜网，单位:s */
    VOS_UINT8                           aucRsv[2];
}NV_GSM_RF_UNAVAILABLE_CFG_STRU;
/* Added by l67237 for Gas Dsds Feature, 2014-3-4, end */

/*****************************************************************************
 结构名    : NVIM_GSM_CELL_INFO_RPT_CFG_STRU
 结构说明  : en_NV_Item_Gsm_Cell_Info_Rpt_Cfg 结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                            ucCellInfoRptFlg;      /* 是否启动上报: 0:不启动; 1:启动 */
    VOS_UINT8                            aucRsv[1];             /* 保留位 */
    VOS_UINT16                           usRptInterval;         /* 上报周期长度, 单位: ms */
}NVIM_GSM_CELL_INFO_RPT_CFG_STRU;

/*****************************************************************************
 结构名    : NVIM_GAS_INDIVIDUAL_CUSTOMIZE_CFG_STRU
 结构说明  : en_NV_Item_Gas_Individual_Customize_Cfg 结构,用于设置非标的特殊定制相关的配置
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucAccFailNoPunishCfg;                   /* 是否惩罚随即接入失败的小区:
                                                                                   bit0: 1: PS失败不惩罚; 0: PS失败惩罚
                                                                                   bit1: 1: CS失败不惩罚; 0: CS失败惩罚 */
    VOS_INT8                            cFreqRxlevThreshold;                    /* 有效频点能量门限,主要用于搜网流程
                                                                                   被动重选 目标小区选择时也会用到 */

    VOS_UINT8                           ucAutoFrNonChanRelCase;                 /* 没有收到 Channel Release 的异常场景下是否启动 FR:
                                                                                   bit0: 1: 主动挂断时启动 FR; 
                                                                                         0: 主动挂断时不启动FR
                                                                                   bit1: 1: 语音链路失败启动FR; 
                                                                                         0: 语音链路失败不启动FR */

    VOS_UINT8                           aucRsv[37];
}NVIM_GAS_INDIVIDUAL_CUSTOMIZE_CFG_STRU;


typedef struct
{
   VOS_UINT16                           usCsHoTimeAlarmThreshold;                   /* CS切换时间长度门限，超过该切换时间，GAS主动上报告警信息 */
   
   VOS_UINT8                            aucRsv[18];                                /* 预留位，为以后KWKC预留 */
}NVIM_GAS_MNTN_CONFIG_STRU;


typedef struct
{
   VOS_UINT8                            ucTdsDefaultQRxlMin;            /* TDS 默认重选门限, 单位: -1dB */

   VOS_UINT8                            aucRsv[3];                      /* 保留位 */
}NVIM_GAS_UTRAN_TDD_DEFAULT_Q_RXLMIN;

/* Added by yangsicong for L2G REDIR C1 CUSTUME, 2015-1-26, begin */

typedef struct
{
    VOS_UINT32                          ulMcc;
    VOS_UINT32                          ulMnc;
}NVIM_GAS_PLMN_ID_STRU;

typedef struct
{
   VOS_UINT16                           usPlmnCnt;                                                  /* 白名单中PLMN个数 */  
   VOS_UINT16                           usReserve;
   NVIM_GAS_PLMN_ID_STRU                astPlmn[NVIM_GAS_C1_CALC_OPT_PLMN_WHITE_LIST_CNT_MAX];       /* 预留位，为以后KWKC预留 */
}NVIM_GAS_C1_CALC_OPT_PLMN_WHITE_LIST_STRU;
/* Added by yangsicong for L2G REDIR C1 CUSTUME, 2015-1-26, end */

/*****************************************************************************
 结构名    : NVIM_GSM_RAPID_HO_CUSTOMIZE_CFG_STRU
 结构说明  : en_NV_Item_GSM_RAPID_HO_CUSTOMIZE_CFG 结构,GSM快速切换定制特性相关配置
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSwitchFlag;                   /* 特性开关 */
    VOS_UINT8                           ucBadQualityThreshold;          /* 信号质量判定门限.误码率.单位: 百分之一 */
    VOS_UINT16                          usAlpha;                        /* Alpha因子. 单位: 0.001 */
    VOS_UINT8                           ucBadQualityCntThreshold;       /* 信号质量差统计门限 */
    VOS_UINT8                           ucNCellRptAddValue;             /* 邻区上报增加值 */
    VOS_UINT8                           aucRsv[2];                      /* 保留位 */
}NVIM_GSM_RAPID_HO_CUSTOMIZE_CFG_STRU;


typedef struct
{
    NVIM_GAS_PLMN_ID_STRU               stPlmn;
    VOS_UINT32                          ulBand;
}NVIM_GSM_BAND_CUSTOMIZE_STRU;


typedef struct
{
   VOS_UINT16                           usItemCnt;                  /* 定制项个数 */
   VOS_UINT16                           usRestoreEnableMask;        /* 启用被丢弃的超出协议范围的频点生效场景 */
   VOS_UINT32                           ulCustomizeBandEnableMask;  /* 定制 Band 生效场景 */
   NVIM_GSM_BAND_CUSTOMIZE_STRU         astCustomizeBand[NVIM_GAS_GSM_BAND_CUSTOMIZE_LIST_MAX_CNT];
}NVIM_GAS_GSM_BAND_CUSTOMIZE_CFG_STRU;
typedef struct
{
   VOS_UINT8                            ucRrRaFailEnableFlg;        /* 生效标志 */

   VOS_UINT8                            ucRsv1;                     /* 保留位 */
   VOS_UINT8                            ucRsv2;                     /* 保留位 */
   VOS_UINT8                            ucRsv3;                     /* 保留位 */
}NVIM_GAS_FAST_AREA_LOST_CFG_STRU;


typedef struct
{
    VOS_UINT8                            ucAutoFrInvalidMask;   /* 生效标志。各bit值 0 表示可以返回, 1 表示无效不可以返回 */
                                                                /* bit0: CSFB 主叫, NAS仍未反馈业务建立结果的情况下是否可以返回 */
                                                                /* bit1: CSFB 被叫, NAS仍未反馈业务建立结果的情况下是否可以返回 */
                                                                /* bit2~bit8: 保留位 */

    VOS_UINT8                            ucRsv1;                /* 保留位 */
    VOS_UINT8                            ucRsv2;                /* 保留位 */
    VOS_UINT8                            ucRsv3;                /* 保留位 */
}NVIM_GAS_AUTO_FR_CFG_STRU;

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










#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif





#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of NasNvInterface.h */
