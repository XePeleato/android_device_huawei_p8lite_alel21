/*
 * head file of mntn_save_logdata.c
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef	__MNTN_SAVE_LOGDATA_H__
#define __MNTN_SAVE_LOGDATA_H__

#include <linux/types.h>


/*
 ***************************************************************************
 * Macro define here
 ***************************************************************************
*/
#define MNTN_AP_LOGDIR        "/data/hisi_logs/ap_log/coredump/"
#define MNTN_CP_LOGDIR        "/data/hisi_logs/cp_log/coredump/"
#define MNTN_HIFI_LOGDIR        "/data/hisi_logs/hifi_log/coredump/"
#define MNTN_MCU_LOGDIR        MNTN_AP_LOGDIR


#define MNTN_SRAME_ACPU_FILE    "sram_acpu_memory.bin"
#define MNTN_SRAME_MCU_FILE    "sram_mcu_memory.bin"
#define MNTN_BBE16_SDR_FILE    "bbe16_sdr_memory.bin"
#define MNTN_BBE16_TDSTABLE_FILE    "bbe16_tds_table_memory.bin"
#define MNTN_BBE16_IMGDDR_FILE    "bbe16_img_ddr_memory.bin"
#define MNTN_BBE16_DTCM_FILE    "bbe16_dtcm_memory.bin"
#define MNTN_TELEMNTN_FILE    "telemntn_memory.bin"
#define MNTN_PERI_SC_FILE    "peri_sc.bin"
#define MNTN_MODEM_SC_FILE    "modem_sc.bin"
#define MNTN_MODEM_LOG_FILE    "modem_log.bin"
#define MNTN_MODEM_MEMORY_FILE    "modem_memory.bin"

#define MNTN_A_EXC_MEMORY_FILE    "a_mntn.bin"
#define MNTN_HIFI_MEMORY_FILE    "hifi_memory.bin"
#define MNTN_HIFI_RH_FILE    "rh.bin"
#define MNTN_MCU_MEMORY_FILE    "mcu_memory.bin"
#define MNTN_MCU_LOG_FILE    "mcu_log.bin"
#define MNTN_IPC_SHARE_DATA_FILE    "ipc_share.bin"

#define MNTN_HISTORY_LOG	"history.log"

/*data/hisi_logs property value*/
#define MNTN_HISILOG_DIR_PRO_VALUE    0774
/*data/hisi_logs/ap_log property value*/
#define MNTN_AP_DIR_PRO_VALUE    0774
/*data/hisi_logs/cp_log property value*/
#define MNTN_CP_DIR_PRO_VALUE    0774
/*data/hisi_logs/hifi_log property value*/
#define MNTN_HIFI_DIR_PRO_VALUE    0774

/*data/hisi_logs/../coredump property value*/
#define MNTN_COREDUMP_DIR_PRO_VALUE    0774

/*data/hisi_logs/../coredump/20150303-113101 property value*/
#define MNTN_COREDUMP_TIMENAME_DIR_PRO_VALUE    0774

#define MNTN_MCU_DIR_PRO_VALUE    MNTN_AP_DIR_PRO_VALUE
#define MNTN_HISTORY_LOG_PRO_VALUE    0664
#define MNTN_LOG_FILE_PRO_VALUE    0664

#define MNTN_LOG_DIR_OWNER_UID    0		/*root*/
#define MNTN_LOG_DIR_OWNER_GID    1000		/*system*/

#define MNTN_LOG_FILE_OWNER_UID    MNTN_LOG_DIR_OWNER_UID
#define MNTN_LOG_FILE_OWNER_GID    MNTN_LOG_DIR_OWNER_GID

#define MNTN_CP_LOG_MAX_NUMBER		5
#define MNTN_MCU_LOG_MAX_NUMBER		5
#define MNTN_HIFI_LOG_MAX_NUMBER		5
/*
 ***************************************************************************
 * Struct define here
 ***************************************************************************
*/

/*
 ***************************************************************************
 *extern functions here
 ***************************************************************************
*/
extern int mntn_hifi_reset_save_log(const char *preason);
extern int mntn_mcu_reset_save_log(const char *preason);
#endif	/* End #define __MNTN_SAVE_LOGDATA_H__ */
