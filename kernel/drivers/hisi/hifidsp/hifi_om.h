/*
 * hifi om.
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __HIFI_OM_H__
#define __HIFI_OM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef PLATFORM_HI3XXX
#define HIFI_LOG_PATH_PARENT "/data/hisi_logs/"
#define HIFI_LOG_PATH "/data/hisi_logs/hifi_log/"
#define FILE_NAME_DUMP_DSP_LOG		 HIFI_LOG_PATH"hifi.log"
#define FILE_NAME_DUMP_DSP_BIN		 HIFI_LOG_PATH"hifi.bin"
#define FILE_NAME_DUMP_DSP_PANIC_LOG HIFI_LOG_PATH"hifi_panic.log"
#define FILE_NAME_DUMP_DSP_PANIC_BIN HIFI_LOG_PATH"hifi_panic.bin"
#define FILE_NAME_DUMP_DSP_OCRAM_BIN HIFI_LOG_PATH"hifi_ocram.bin"
#define FILE_NAME_DUMP_DSP_TCM_BIN   HIFI_LOG_PATH"hifi_tcm.bin"
#endif
#define HIFIDEBUG_PATH                                "hifidebug"
#define HIFIDEBUG_LEVEL_PROC_FILE                     "debuglevel"
#define HIFIDEBUG_DSPDUMPLOG_PROC_FILE                "dspdumplog"
#ifdef PLATFORM_HI3XXX
#define HIFIDEGUB_DSPDEBUGLEVEL_PROC_FILE             "dspdebuglevel"
#endif

#ifndef LOG_TAG
#define LOG_TAG "hifi_misc "
#endif

typedef enum {
	DUMP_DSP_LOG,
	DUMP_DSP_BIN
}DUMP_DSP_TYPE;

typedef enum {
	DSP_NORMAL,
	DSP_PANIC
}DSP_ERROR_TYPE;

typedef enum {
	NORMAL_LOG = 0,
	NORMAL_BIN,
	PANIC_LOG,
	PANIC_BIN,
	OCRAM_BIN,
	TCM_BIN
}DUMP_DSP_INDEX;

#ifdef PLATFORM_HI6XXX
typedef struct
{
	unsigned int		work_id;
	struct work_struct	work_stru;
} hifi_mntn_work_stru;
#endif

struct hifi_om_s {
	struct task_struct* kdumpdsp_task;
	struct semaphore	dsp_dump_sema;

	unsigned int	debug_level;
	unsigned int	dsp_debug_level;
	unsigned int*	dsp_debug_level_addr;

	unsigned int	pre_dump_timestamp;
	unsigned int*	dsp_time_stamp;
	unsigned int	pre_exception_no;
	unsigned int*	dsp_exception_no;

	unsigned int*	dsp_panic_mark;

	unsigned int*	dsp_log_cur_addr;
	char*			dsp_log_addr;
	char*			dsp_bin_addr;
	bool			first_dump_log;
	bool			force_dump_log;
	bool			dsp_hifidebug_show_tag;
	bool			dsp_loaded;
	unsigned int	dsp_loaded_sign;

	unsigned int*	dsp_debug_kill_addr;

#ifdef PLATFORM_HI6XXX
	hifi_mntn_work_stru 	 hifi_mntn_work;
	struct workqueue_struct *hifi_mntn_wq;
#endif
};

struct hifi_dsp_dump_info{
	DSP_ERROR_TYPE error_type;
	DUMP_DSP_TYPE dump_type;
	char* file_name;
	char* data_addr;
	unsigned int data_len;
};

extern struct hifi_om_s g_om_data;

#ifdef PLATFORM_HI3XXX
#define HIFI_STAMP (unsigned int)readl(g_om_data.dsp_time_stamp)
#endif

#ifdef PLATFORM_HI6XXX
#define HIFI_STAMP 0
#endif

#define logd(fmt, ...) \
do {\
	if (g_om_data.debug_level >= 3) {\
		printk(LOG_TAG"[D][%u]:%s:%d: "fmt, HIFI_STAMP, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
	}\
} while(0);

#define logi(fmt, ...) \
do {\
	if (g_om_data.debug_level >= 2) {\
		printk(LOG_TAG"[I][%u]:%s:%d: "fmt, HIFI_STAMP, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
	}\
} while(0);


#define logw(fmt, ...) \
do {\
	if (g_om_data.debug_level >= 1) {\
		printk(LOG_TAG"[W][%u]:%s:%d: "fmt, HIFI_STAMP, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
	}\
} while(0);

#define loge(fmt, ...) \
do {\
	if (g_om_data.debug_level >= 0) {\
		printk(LOG_TAG"[E][%u]:%s:%d: "fmt, HIFI_STAMP, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
	}\
} while(0);

#define IN_FUNCTION   logd("begin.\n");
#define OUT_FUNCTION  logd("end.\n");

void hifi_om_init(struct platform_device *dev, unsigned char* hifi_priv_base_virt, unsigned char* hifi_priv_base_phy);
void hifi_om_deinit(struct platform_device *dev);

int hifi_dsp_dump_hifi(unsigned long arg);
void hifi_dump_panic_log(void);

bool hifi_is_loaded(void);

#ifdef PLATFORM_HI3XXX
int hifi_get_dmesg(unsigned long arg);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

