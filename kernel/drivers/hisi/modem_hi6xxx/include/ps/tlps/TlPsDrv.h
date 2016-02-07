

#ifndef __TLPS_DRV_H__
#define __TLPS_DRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
/*k00279574 add for VC build pass*/
#if (VOS_OS_VER != VOS_WIN32)
#include "mdrv.h"
#endif

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*DRV_SYSTEM_ERROR*/
#define LDRV_OM_SYSTEM_ERROR mdrv_om_system_error

#define LDRV_OM_REGISTER_CALLBACK mdrv_om_register_callback

/*BSP_BBPGetCurTime*/
#define LDRV_TIMER_GET_ACC_TS mdrv_timer_get_accuracy_timestamp

/*DRV_GET_SLICE*/
#define LDRV_TIMER_GET_NORMAL_TIMESTAMP mdrv_timer_get_normal_timestamp

/*BSP_OM_GetBoardType*/
#define LDRV_VER_GET_INFO mdrv_ver_get_info

/*BSP_BBPIntDlTbRegCb*/
#define LDRV_BBP_REG_DLTB_INTR_CB mdrv_bbp_register_dltb_intr_cb

/*mdrv_bbp_get_lte_sysfrm*/
#define LDRV_BBP_GET_SYSFRM mdrv_bbp_get_lte_sysfrm

/*mdrv_bbp_get_lte_subfrm*/
#define LDRV_BBP_GET_SUBFRM mdrv_bbp_get_lte_subfrm

/*mdrv_bbp_init_intr*/
#define LDRV_BBP_INIT_INTR mdrv_bbp_init_intr

/*mdrv_bbp_get_lte_ta*/
#define LDRV_BBP_GET_TA mdrv_bbp_get_lte_ta

/*mdrv_dsp_load_bbe*/
#define LDRV_DSP_LOAD_BBE mdrv_dsp_load_bbe

/*mdrv_dsp_run_bbe*/
#define LDRV_DSP_RUN_BBE mdrv_dsp_run_bbe

/*mdrv_pm_pwrup*/
#define LDRV_PM_PWRUP mdrv_pm_pwrup

/*mdrv_pm_wake_lock*/
#define LDRV_PM_WAKE_LOCK mdrv_pm_wake_lock

/*mdrv_pm_wake_unlock*/
#define LDRV_PM_WAKE_UNLOCK mdrv_pm_wake_unlock

#define LDRV_PM_GET_DFS_PROFILE_LIMIT mdrv_pm_get_dfs_profile_limit

/*BSP_IPF_Init*/
#define LDRV_IPF_INIT mdrv_ipf_init

/*BSP_IPF_ConfigUlChannel*/
#define LDRV_IPF_CONFIG_ULCHAN mdrv_ipf_config_ulchan

/*BSP_IPF_ConfigDlChannel*/
#define LDRV_IPF_CONFIG_DLCHAN mdrv_ipf_config_dlchan

/*BSP_IPF_ChannelEnable*/
#define LDRV_IPF_ENABLE_CHAN mdrv_ipf_enable_chan

/*BSP_IPF_GetDlBDNum*/
#define LDRV_IPF_GET_DLBD_NUM mdrv_ipf_get_dlbd_num

/*BSP_IPF_GetFilter*/
#define LDRV_IPF_GET_FILTER mdrv_ipf_get_filter

#define LDRV_IPF_REG_OPS mdrv_ipf_register_ops

/*BSP_IPF_GetUlRd*/
#define LDRV_IPF_GET_ULRD mdrv_ipf_get_ulrd

/*BSP_IPF_ConfigUlAd*/
#define LDRV_IPF_CONFIG_ULAD mdrv_ipf_config_ulad

/*BSP_IPF_GetUlAdNum*/
#define LDRV_IPF_GET_ULAD_NUM mdrv_ipf_get_ulad_num

/*BSP_IPF_SetFilter*/
#define LDRV_IPF_SET_FILTER mdrv_ipf_set_filter

/*BSP_IPF_ConfigDownFilter*/
#define LDRV_IPF_CONFIG_DLBD mdrv_ipf_config_dlbd

/*BSP_IPF_ConfigTimeout*/
#define LDRV_IPF_CONFIG_TIMEOUT mdrv_ipf_config_timeout

/*BSP_IPF_ConfigADThr*/
#define LDRV_IPF_CONFIG_ADTHRED mdrv_ipf_config_adthred

#define LDRV_IPF_SET_DBUF_PARA mdrv_ipf_set_dbuf_para

/*DRV_RT_MEMCPY*/
#define LDRV_MEM_MEMCPY mdrv_memcpy

/*BSP_GetMemAddrSize*/
#define LDRV_MEM_GETMEMINFO mdrv_getmeminfo

/*DIAG_LogReport*/
#define LDRV_DIAG_LOGREPORT DIAG_PrintfV

/*DIAG_EventReport*/
#define LDRV_DIAG_EVENTREPORT DIAG_ReportEventLog

/*DIAG_AirMsgReport*/
#define LDRV_DIAG_AIRMSGREPORT DIAG_ReportAirMessageLog

/*DIAG_TraceReport*/
#define LDRV_DIAG_TRACEREPORT DIAG_ReportLayerMessageLog

/*DIAG_UserPlaneReport*/
#define LDRV_DIAG_USERPLANEREPORT DIAG_ReportUserPlaneMessageLog

/*DIAG_CommonReport*/
#define LDRV_DIAG_COMMONREPORT DIAG_ReportCommand

/*DIAG_GtrRcvCallBackReg*/
#define LDRV_DIAG_GTRRCVCALLBACKREG DIAG_GtrRcvCallBackReg

/*DIAG_SocpPowerOn*/
#define LDRV_DIAG_SOCPPOWERON DIAG_SocpPowerOn

/*DIAG_DspInitFinished*/
#define LDRV_DIAG_DSPINITFINISHED DIAG_DspInitFinished

#define LDRV_TLSLEEP_ACTIVATEHW TLSLEEP_ActivateHw

#define LDRV_FTM_GETMODE FTM_GetMode

/*BSP_Accelerator_DMA*/
#define LDRV_ACC_DMA mdrv_acc_dma

/*BSP_Accelerator_Cipher*/
#define LDRV_ACC_CIPHER mdrv_acc_cipher

/*BSP_Accelerator_GetBdFifoAddr*/
#define LDRV_ACC_GET_BDQ_ADDR mdrv_acc_get_bdq_addr

/*BSP_Accelerator_GetStatus*/
#define LDRV_ACC_GET_STATUS mdrv_acc_get_status

/*BSP_Accelerator_Enable*/
#define LDRV_ACC_ENABLE mdrv_acc_enable

/*BSP_CIPHER_RegistFreeMemCB*/
#define LDRV_CIPHER_REG_MEMFREE_CB mdrv_cipher_register_memfree_cb

/*BSP_CIPHER_RegistNotifyCB*/
#define LDRV_CIPHER_REG_NOTIFY_CB mdrv_cipher_register_notify_cb

/*BSP_CIPHER_Purge*/
#define LDRV_CIPHER_PURGE_CHAN mdrv_cipher_purge_chan

/*BSP_CIPHER_SingleSubmitTask*/
#define LDRV_CIPHER_SINGLE_SUBMIT_TASK mdrv_cipher_single_submit_task

/*BSP_CIPHER_RelaSubmitTask*/
#define LDRV_CIPHER_RELA_SUBMIT_TASK mdrv_cipher_rela_submit_task

/*BSP_CIPHER_SetKey*/
#define LDRV_CIPHER_SET_KEY mdrv_cipher_set_key

/*BSP_CIPHER_GetKey*/
#define LDRV_CIPHER_GET_KEY mdrv_cipher_get_key

/*BSP_CIPHER_SetDataBufPara*/
#define LDRV_CIPHER_SET_DBUF_PARA mdrv_cipher_set_dbuf_para

/*BSP_CIPHER_GetCmplSrcBuff*/
#define LDRV_CIPHER_GET_CMPLT_DATA mdrv_cipher_get_cmplt_data

/*BSP_CIPHER_Disable*/
#define LDRV_CIPHER_DISABLE mdrv_cipher_disable


/*BSP_KDF_KeyMake*/
#define LDRV_CIPHER_GEN_KEY mdrv_cipher_gen_key

/*BSP_CIPHER_GetRdInfo*/
#define LDRV_CIPHER_GET_RDINFO mdrv_cipher_get_rdinfo

/*DRV_DUMP_GET_BUFFER*/
#define LDRV_OM_REGISTER_FIELD mdrv_om_register_field

/*ddmPhaseScoreBoot*/
#define LDRV_SYSBOOT_STAMPADD mdrv_sysboot_stampadd


/*DRV_MODEM_MODE_CONFIG*/
#define LDRV_PMU_SET_MODEM_MODE mdrv_pmu_set_modem_mode

/*DRV_MODEM_APT_DISABLE*/
#define LDRV_PMU_DISABLE_APT mdrv_pmu_disable_apt

/*DRV_MODEM_APT_ENABLE*/
#define LDRV_PMU_ENABLE_APT mdrv_pmu_enable_apt

/*BSP_BBPGetCurTime*/
#define TDRV_TIMER_GET_ACC_TS mdrv_timer_get_accuracy_timestamp

/*mdrv_timer_get_normal_timestamp*/
#define TDRV_TIMER_GET_NORMAL_TS mdrv_timer_get_normal_timestamp

/*mdrv_bbp_register_tdstf_intr_cb*/
#define TDRV_BBP_REG_TDSTF_INTR_CB mdrv_bbp_register_tdstf_intr_cb

/*mdrv_bbp_enable_tdstf_intr*/
#define TDRV_BBP_ENABLE_TDSTF_INTR mdrv_bbp_enable_tdstf_intr

#define TDRV_BBP_DISABLE_TDSTF_INTR mdrv_bbp_disable_tdstf_intr

/*mdrv_bbp_set_tds_subfrm_offset*/
#define TDRV_BBP_SET_SUBFRM_OFFSET mdrv_bbp_set_tds_subfrm_offset

/*mdrv_pm_pwrup*/
#define TDRV_PM_PWRUP mdrv_pm_pwrup

/*DRV_RT_MEMCPY*/
#define TDRV_MEM_MEMCPY mdrv_memcpy

/*mdrv_nv_read*/
#define TDRV_NV_READ NV_Read

/*DIAG_LogReport*/
#define TDRV_DIAG_LOGREPORT DIAG_PrintfV

/*DIAG_EventReport*/
#define TDRV_DIAG_EVENTREPORT DIAG_ReportEventLog

/*DIAG_TraceReport*/
#define TDRV_DIAG_TRACEREPORT DIAG_ReportLayerMessageLog

/*BSP_GetIPBaseAddr*/
#define TDRV_MISC_GET_IP_BASEADDR mdrv_misc_get_ip_baseaddr

/*DRV_MODEM_MODE_CONFIG*/
#define TDRV_PMU_SET_MODEM_MODE mdrv_pmu_set_modem_mode

/*DRV_MODEM_APT_DISABLE*/
#define TDRV_PMU_DISABLE_APT mdrv_pmu_disable_apt

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

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

/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
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

#endif

