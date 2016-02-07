/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_id.h                                                          */
/*                                                                           */
/* Author: Qin Peifeng                                                       */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2000-04-21                                                          */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2000-04-21                                                       */
/*    Author: Qin Peifeng                                                    */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/
#ifndef _V_ID_H
#define _V_ID_H

#include "v_iddef.h"
#include "VosPidDef.h"
#include "product_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/* To decide to use dynamic FID/PID or static FID/PID, if using static */
#define VOS_USE_DYNAMIC_FIDPID                 VOS_NO

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
extern VOS_UINT32 HPA_FIDInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 GAS_FidPidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 LLC_FID_Init( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 PBD_FID_Init( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 WTTF_MAC_RLC_UlFidInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 WTTF_MAC_RLC_DlFidInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 WuepsRabmFidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 NAS_MML_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 CNAS_FidInit(enum VOS_INIT_PHASE_DEFINE enInitPhase);
extern VOS_UINT32 WuepsCmFidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 USIMM_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 WuepsSIFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 WuepsTafFidInit( enum  VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 WuepsSleepFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 UE_FIDInit( enum  VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 GRM_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 WuepsOMFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 RRC_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 PS_OM_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 CM_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 LMAC_RLC_UlFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 LMAC_RLC_DlFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 LHPA_FidPidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 COMM_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 MSP_DiagFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 MSP_L4FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 MSP_FtmFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 OMCallBackFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 Spy_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 OM_AgentFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 CBPCA_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 CSIMA_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 OM_NoSigFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 FC_CCORE_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
/* Add begin by s00164817*/
extern VOS_UINT32 TTF_MemRbFreeFidInit ( enum VOS_INIT_PHASE_DEFINE ip );
/* Add end by s00164817*/
extern VOS_UINT32 CDS_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase);
#if (FEATURE_ON == FEATURE_BASTET)
extern VOS_UINT32 BST_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase);
#endif
extern VOS_UINT32 NDCLIENT_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase);
extern VOS_UINT32 WuepsSleepFidInit(enum VOS_INIT_PHASE_DEFINE ip);

extern VOS_UINT32  CAS_1X_FidPidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 CTTF_1X_RevFidInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 CTTF_1X_FwdFidInit( enum VOS_INIT_PHASE_DEFINE ip );

/* 1x cproc begin */
extern VOS_UINT32 DSP_CPROC_FidInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 CPROC_1X_FidInit( enum VOS_INIT_PHASE_DEFINE enInitPhrase );
extern VOS_UINT32 CPROC_EVDO_FidInit( enum VOS_INIT_PHASE_DEFINE enInitPhrase );
/* 1x cproc end */

extern VOS_UINT32 CBTFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 PAMOM_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)/* TDS begin */
extern VOS_UINT32 process_pdc_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
/*extern VOS_UINT32 process_rlc_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );*/
extern VOS_UINT32 process_mac_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 hl100_SndCmd_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
#endif/*(FEATURE_ON == FEATURE_UE_MODE_TDS) TDS end */

#elif (OSA_CPU_ACPU == VOS_OSA_CPU)
extern VOS_UINT32 APP_NDIS_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 DMS_DsFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 OM_AcpuFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 OM_AcpuCallBackFidInit(enum VOS_INIT_PHASE_DEFINE ip);
/* Linux add begin，w68271 */
extern VOS_UINT32 RNIC_FidInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 NFExt_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
/* Linux add end，w68271 */
extern VOS_UINT32 FC_ACORE_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase);
extern VOS_UINT32 CPULOAD_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase);
/* Add begin by s00164817*/
extern VOS_UINT32 IMM_RbMemFreeFidInit( enum VOS_INIT_PHASE_DEFINE ip );
/* Add end by s00164817*/

extern VOS_UINT32 MSP_AppDiagFidInit(enum VOS_INIT_PHASE_DEFINE ip);

#if (FEATURE_ON == FEATURE_BASTET)
extern VOS_UINT32 BASTET_CommFidInit(enum VOS_INIT_PHASE_DEFINE ip);
#endif

extern VOS_UINT32 CBTAppFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 USIMM_APP_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 PCV_AcpuFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 PAMOM_APP_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);

#endif  /*(OSA_CPU_CCPU == VOS_OSA_CPU)*/

#ifdef HPA_ITT
extern VOS_UINT32 MAPS_Stub_Fid_InitFun(enum VOS_INIT_PHASE_DEFINE InitPhase);
#endif  /*HPA_ITT*/

#ifdef USP_2_0
extern VOS_UINT32 WHPA_FidPidInit( enum VOS_INIT_PHASE_DEFINE ip );
#endif  /*USP_2_0*/

#if (VOS_WIN32 == VOS_OS_VER)
extern VOS_UINT32 WuepsATFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 RNIC_FidInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 IMM_RbMemFreeFidInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 OM_AcpuFidInit( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 CBTAppFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 USIMM_APP_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 PCV_AcpuFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 PAMOM_APP_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
#endif  /*(VOS_WIN32 == VOS_OS_VER)*/

extern VOS_UINT32 ADS_UL_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 ADS_DL_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 CSD_FidInit(enum VOS_INIT_PHASE_DEFINE enInitPhase);

extern VOS_UINT32 TAF_AGENT_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase);

/* DSP 上移  */
extern VOS_UINT32 UPHY_APM_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 UPHY_UCOM_WphyFidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 UPHY_UCOM_GphyFidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 UPHY_DRX_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 UPHY_IDLE_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 UPHY_UCOM_UpaFidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 UPHY_UCOM_WmeasFidInit(enum VOS_INIT_PHASE_DEFINE InitPhrase);
extern VOS_UINT32 UPHY_INIT_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 RCM_DRV_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );

/* DSP 上移 end */

/* Voice in ACPU start */
extern VOS_UINT32 VOICE_NormalFidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase);
extern VOS_UINT32 VOICE_RtFidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase);
/* Voice in ACPU end */

extern VOS_UINT32 IMSA_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 IMSVA_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 IPS_MNTN_CCORE_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 MTC_FidInit (enum VOS_INIT_PHASE_DEFINE ip);
/************************* MODEM ID DEFINITION ******************************/

#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
extern VOS_UINT32 I1_USIMM_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_WuepsSIFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_WuepsSleepFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_GAS_FidPidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_NAS_MML_FidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_WuepsCmFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_WuepsRabmFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_WuepsTafFidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_PBD_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_LLC_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_GRM_FID_Init(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_UINT32 I1_UPHY_UCOM_GphyFidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 I1_UPHY_APM_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 I1_UPHY_DRX_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 I1_UPHY_IDLE_FidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 RRM_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase);
#endif  /*( FEATURE_MULTI_MODEM == FEATURE_ON )*/

extern VOS_UINT32 CMMCA_FidInit (enum VOS_INIT_PHASE_DEFINE ip);

/************************* MACRO & TYPE DEFINITION ***************************/
/* VOS编程模式下的用户模块划分信息 */
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
BEGIN_FID_DEFINITION()
/* add your FID here, e.g.
  * DEFINE_FID(VOS_FID_APP0)
  * DEFINE_FID(VOS_FID_APPx) */
    DEFINE_FID(WUEPS_FID_OM)
    DEFINE_FID(I0_MAPS_USIMM_FID)
    DEFINE_FID(I0_MAPS_SI_FID)
    DEFINE_FID(I0_UEPS_FID_SL)
    DEFINE_FID(UEPS_FID_RM)
    DEFINE_FID(I0_UEPS_FID_RR)
    DEFINE_FID(I0_UEPS_FID_GRM)
    DEFINE_FID(WUEPS_FID_HPA)
    DEFINE_FID(I0_WUEPS_FID_MM)
    DEFINE_FID(I0_WUEPS_FID_CM)
    DEFINE_FID(I0_WUEPS_FID_RABMSM)
    DEFINE_FID(I0_WUEPS_FID_TAF)
    DEFINE_FID(WUEPS_FID_OM_CALLBACK)
    DEFINE_FID(WUEPS_FID_SPY)
    DEFINE_FID(AGENT_FID_OM)
    DEFINE_FID(UEPS_FID_FLOWCTRL_C)
    DEFINE_FID(UEPS_FID_RM_DL)
    DEFINE_FID(WUEPS_FID_NOSIG)
    DEFINE_FID(UEPS_FID_TTF_MEM_RB_FREE)
    DEFINE_FID(UEPS_FID_CDS)
    DEFINE_FID(UEPS_FID_NDCLIENT)
    DEFINE_FID(I0_UEPS_FID_PBD)
    DEFINE_FID(WUEPS_FID_NVIM_FLUSH)
    DEFINE_FID(I0_WUEPS_FID_SLEEP)
    DEFINE_FID(I0_DSP_FID_APM)
    DEFINE_FID(DSP_FID_UPA)
    DEFINE_FID(DSP_FID_WPHY)
    DEFINE_FID(I0_DSP_FID_GPHY)
    DEFINE_FID(I0_DSP_FID_DRX)
    DEFINE_FID(I0_DSP_FID_IDLE)
    DEFINE_FID(DSP_FID_WMEAS)
    DEFINE_FID(DSP_FID_RCM)
    DEFINE_FID(DSP_FID_STARTUP)
    DEFINE_FID(UEPS_FID_IPS_CCORE)
    DEFINE_FID(WUEPS_FID_CBPCA)
    DEFINE_FID(WUEPS_FID_CSIMA)
    DEFINE_FID(WUEPS_FID_CMMCA)
    DEFINE_FID(CCPU_FID_CBT)
    DEFINE_FID(CCPU_FID_PAM_OM)
    DEFINE_FID(UEPS_FID_MTC)

#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
    DEFINE_FID(I1_MAPS_USIMM_FID)
    DEFINE_FID(I1_MAPS_SI_FID)
    DEFINE_FID(I1_WUEPS_FID_SLEEP)
    DEFINE_FID(I1_UEPS_FID_RR)
    DEFINE_FID(I1_WUEPS_FID_MM)
    DEFINE_FID(I1_WUEPS_FID_CM)
    DEFINE_FID(I1_WUEPS_FID_RABMSM)
    DEFINE_FID(I1_WUEPS_FID_TAF)
    DEFINE_FID(I1_UEPS_FID_SL)
    DEFINE_FID(I1_UEPS_FID_PBD)
    DEFINE_FID(I1_UEPS_FID_GRM)
    DEFINE_FID(I1_DSP_FID_APM)
    DEFINE_FID(I1_DSP_FID_GPHY)
    DEFINE_FID(I1_DSP_FID_DRX)
    DEFINE_FID(I1_DSP_FID_IDLE)
    DEFINE_FID(UEPS_FID_RRM)
#endif  /*( FEATURE_MULTI_MODEM == FEATURE_ON )*/

#if (RAT_MODE != RAT_GU)
    DEFINE_FID(MSP_FID_DIAG_CCPU)
    DEFINE_FID(MSP_L4_FID)
    DEFINE_FID(MSP_FTM_FID)
    DEFINE_FID(PS_FID_RRC)
    DEFINE_FID(PS_FID_CM)
    DEFINE_FID(PS_FID_OM)
    DEFINE_FID(PS_FID_RM_UL)
    DEFINE_FID(PS_FID_RM_DL)
    DEFINE_FID(PS_FID_HPA)
    DEFINE_FID(PS_FID_IMSA)
    DEFINE_FID(PS_FID_IMSVA)
#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
    DEFINE_FID(HL1_FID_SNDCMD)
    DEFINE_FID(TPS_FID_MAC)
    DEFINE_FID(TPS_FID_PDC)
#endif

#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    DEFINE_FID(UEPS_FID_CAS1X)
    DEFINE_FID(MSPS_FID_CTTF_1X_REV)
    DEFINE_FID(MSPS_FID_CTTF_1X_FWD)
    DEFINE_FID(DSP_FID_CPROC)
    DEFINE_FID(CPROC_FID_1X)
    DEFINE_FID(CPROC_FID_EVDO)
    DEFINE_FID(UEPS_FID_CNAS)
#endif

#ifdef HPA_ITT
    DEFINE_FID(MAPS_STUB_FID)
#endif  /*HPA_ITT*/
#if (FEATURE_ON == FEATURE_BASTET)
    DEFINE_FID(UEPS_FID_BASTET)
#endif
#if (VOS_WIN32 == VOS_OS_VER)
    DEFINE_FID(WUEPS_FID_AT)
    DEFINE_FID(ACPU_FID_OM)
    DEFINE_FID(WUEPS_FID_ITTSTUB)
    DEFINE_FID(ACPU_FID_ADS_UL)
    DEFINE_FID(ACPU_FID_ADS_DL)
    DEFINE_FID(ACPU_FID_CSD)
    DEFINE_FID(ACPU_FID_RNIC)
    DEFINE_FID(ACPU_FID_TAFAGENT)
    DEFINE_FID(UEPS_FID_IMM_RB_FREE)
    DEFINE_FID(MSP_APP_DS_FID)
    DEFINE_FID(ACPU_FID_CBT)
    DEFINE_FID(ACPU_FID_USIMM)
    DEFINE_FID(ACPU_FID_PCVOICE)
    DEFINE_FID(ACPU_FID_PAM_OM)
#endif

END_FID_DEFINITION()
#endif  /*(OSA_CPU_CCPU == VOS_OSA_CPU)*/

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
BEGIN_FID_DEFINITION()
/* add your FID here, e.g.
 * DEFINE_FID(VOS_FID_APP0)
 * DEFINE_FID(VOS_FID_APPx) */
    DEFINE_FID(UEPS_FID_IMM_RB_FREE)
    DEFINE_FID(MSP_APP_DS_FID)
    DEFINE_FID(PS_FID_APP_NDIS_PPP_DIPC)
    DEFINE_FID(ACPU_FID_OM_CALLBACK)
    DEFINE_FID(ACPU_FID_OM)
    DEFINE_FID(ACPU_FID_RNIC)
    DEFINE_FID(ACPU_FID_NFEXT)
    DEFINE_FID(ACPU_FID_ADS_UL)
    DEFINE_FID(ACPU_FID_ADS_DL)
    DEFINE_FID(ACPU_FID_CSD)
    DEFINE_FID(ACPU_FID_TAFAGENT)
    DEFINE_FID(ACPU_FID_CPULOAD)
    DEFINE_FID(UEPS_FID_FLOWCTRL_A)
    DEFINE_FID(ACPU_FID_CBT)
    DEFINE_FID(ACPU_FID_USIMM)
    DEFINE_FID(ACPU_FID_PCVOICE)
    DEFINE_FID(ACPU_FID_PAM_OM)

#if (RAT_MODE != RAT_GU)
    DEFINE_FID(MSP_FID_DIAG_ACPU)
#endif  /*(RAT_MODE != RAT_GU)*/

#if (FEATURE_VOICE_UP == FEATURE_ON)
    DEFINE_FID(ACPU_FID_MED_RT)
    DEFINE_FID(ACPU_FID_MED)
#endif

#if (FEATURE_ON == FEATURE_BASTET)
    DEFINE_FID(ACPU_FID_BASTET_COMM)
#endif

END_FID_DEFINITION()
#endif  /*(OSA_CPU_ACPU == VOS_OSA_CPU)*/

#ifdef VOS_OSA_SINGLE_ARM
#define VOS_PID_DOPRAEND                (VOS_PID_CPU_ID_0_DOPRAEND)
#define VOS_PID_BUTT                    (VOS_CPU_ID_1_PID_BUTT)

#else

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#define VOS_PID_DOPRAEND                (VOS_PID_CPU_ID_0_DOPRAEND)
#define VOS_PID_BUTT                    (VOS_CPU_ID_0_PID_BUTT)
#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#define VOS_PID_DOPRAEND                (VOS_PID_CPU_ID_1_DOPRAEND)
#define VOS_PID_BUTT                    (VOS_CPU_ID_1_PID_BUTT)
#endif

#endif


/* 为了和MAPS2000兼容, 暂时转定义下述PID */
#define WUEPS_PID_PHY                   WUEPS_PID_HPA
#define WUEPS_PID_RRCF                  WUEPS_PID_WRR

/* GU NAS PID转定义 */
#define WUEPS_PID_LMM                   PS_PID_MM
#define WUEPS_PID_ESM                   PS_PID_ESM

/*for new HPA */
#define UEPS_PID_WHPA                   WUEPS_PID_HPA
#define UEPS_PID_GHPA_PP                UEPS_PID_GHPA

#define PS_PID_APP                      PS_PID_OM

#if defined(INSTANCE_1)
#define MAPS_USIMM_FID   I1_MAPS_USIMM_FID
#define MAPS_SI_FID      I1_MAPS_SI_FID
#define WUEPS_FID_SLEEP  I1_WUEPS_FID_SLEEP
#define UEPS_FID_RR      I1_UEPS_FID_RR
#define WUEPS_FID_MM     I1_WUEPS_FID_MM
#define WUEPS_FID_CM     I1_WUEPS_FID_CM
#define WUEPS_FID_RABMSM I1_WUEPS_FID_RABMSM
#define WUEPS_FID_TAF    I1_WUEPS_FID_TAF
#define UEPS_FID_SL      I1_UEPS_FID_SL
#define UEPS_FID_PBD     I1_UEPS_FID_PBD
#define UEPS_FID_GRM     I1_UEPS_FID_GRM
#define DSP_FID_APM      I1_DSP_FID_APM
#define DSP_FID_GPHY     I1_DSP_FID_GPHY
#define DSP_FID_DRX      I1_DSP_FID_DRX
#define DSP_FID_IDLE     I1_DSP_FID_IDLE
#else
#define MAPS_USIMM_FID   I0_MAPS_USIMM_FID
#define MAPS_SI_FID      I0_MAPS_SI_FID
#define WUEPS_FID_SLEEP  I0_WUEPS_FID_SLEEP
#define UEPS_FID_RR      I0_UEPS_FID_RR
#define WUEPS_FID_MM     I0_WUEPS_FID_MM
#define WUEPS_FID_CM     I0_WUEPS_FID_CM
#define WUEPS_FID_RABMSM I0_WUEPS_FID_RABMSM
#define WUEPS_FID_TAF    I0_WUEPS_FID_TAF
#define UEPS_FID_SL      I0_UEPS_FID_SL
#define UEPS_FID_PBD     I0_UEPS_FID_PBD
#define UEPS_FID_GRM     I0_UEPS_FID_GRM
#define DSP_FID_APM      I0_DSP_FID_APM
#define DSP_FID_GPHY     I0_DSP_FID_GPHY
#define DSP_FID_DRX      I0_DSP_FID_DRX
#define DSP_FID_IDLE     I0_DSP_FID_IDLE
#endif

#if (VOS_WIN32 == VOS_OS_VER)
extern VOS_UINT32 WuepsFidITTStubInit( enum VOS_INIT_PHASE_DEFINE ip ) ;
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_ID_H */

