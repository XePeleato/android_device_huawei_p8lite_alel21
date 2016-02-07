/*
 * hifi misc driver.
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __HIFI_LPP_H__
#define __HIFI_LPP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PAGE_MAX_SIZE		 0x1000

#ifndef OK
#define OK			 0
#endif

#define ERROR		(-1)
#define BUSY		(-2)
#define NOMEM		(-3)
#define INVAILD 	(-4)

/* IOCTL入参和出参的SIZE限制 */
#define SIZE_LIMIT_PARAM		(256)

#ifdef PLATFORM_HI3XXX
/* HIFI专用区 */
/* |0x37700000~~~~~~~~|0x37c00000~~~~~|0x37c02000~~~~~|0x37c20000~~~~~~|0x37c60000~~~|0x37c75000~~~~~|0x37c75400~~~~~|0x37c75410~~~~~|0x37c80000~~~~~~~~~| */
/* |~~~~~Code run 5M~~|~~~socp 8K~~~~~|~~reserv1 120K~|~~~~~OCRAM 256K~|~~~~TCM 84K~~|~~~SEC HEAD 1K~|~WTD FLAG 16B~~|~~reserv2 43K~~|~~Code backup 3.5M~| */
/* |~~~~~~~~no sec~~~~|~~~~~~~no sec~~|~~~~~~~no sec~~|~~~~~~~nosec~~~~|~~~~~~nosec~~|~~~~~~~~nosec~~|~~~~~~~~nosec~~|~~~~~~~no sec~~|~~~~~~~~~sec~~~~~~~| */
/* |0x37bfffff~~~~~~~~|0x37c01fff~~~~~|0x37c1ffff~~~~~|0x37c5ffff~~~~~~|0x37c74fff~~~|0x37c753ff~~~~~|0x37c7540f~~~~~|0x37c7ffff~~~~~|0x37ffffff~~~~~~~~~| */

/* |~~0x2D600000 ~ 0x2DBFFFFF~~| */
/* |~~~Music data 6M~~~~~~~~~~~| */
/* |~~~~~no sec~~~~~~~~~~~~~~~~| */

#define HIFI_RUN_SIZE					(0x500000)
#define HIFI_MUSIC_DATA_SIZE			(0x600000)
#define HIFI_SOCP_SIZE					(0x2000)
#define HIFI_RESERVE1_SIZE				(0x1E000)
#define HIFI_IMAGE_OCRAMBAK_SIZE		(0x40000)
#define HIFI_IMAGE_TCMBAK_SIZE			(0x15000)
#define HIFI_RESERVE2_SIZE				(0xABF0)
#define HIFI_IMAGE_SIZE 				(0x380000)
#define HIFI_SIZE						(0x900000)
#define HIFI_SEC_HEAD_SIZE				(0x400)
#define HIFI_WTD_FLAG_SIZE				(0x10)

#define HIFI_BASE_ADDR					(0x37700000)
#define HIFI_RUN_LOCATION				(HIFI_BASE_ADDR)											/*Code run*/
#define HIFI_SYS_MEM_ADDR               (HIFI_BASE_ADDR)
#define HIFI_MUSIC_DATA_LOCATION		(0x2D600000)												/*Music Data*/
#define HIFI_SOCP_LOCATION				(HIFI_RUN_LOCATION + HIFI_RUN_SIZE) 						/*SOCP*/
#define HIFI_RESERVE1_LOCATION			(HIFI_SOCP_LOCATION + HIFI_SOCP_SIZE)						/*reserv1*/
#define HIFI_IMAGE_OCRAMBAK_LOCATION	(HIFI_RESERVE1_LOCATION + HIFI_RESERVE1_SIZE)				/*OCRAM*/
#define HIFI_IMAGE_TCMBAK_LOCATION		(HIFI_IMAGE_OCRAMBAK_LOCATION + HIFI_IMAGE_OCRAMBAK_SIZE)	/*TCM*/
#define HIFI_SEC_HEAD_BACKUP            (HIFI_IMAGE_TCMBAK_LOCATION + HIFI_IMAGE_TCMBAK_SIZE)		/*SEC HEAD backup*/
#define HIFI_WTD_FLAG_BASE				(HIFI_SEC_HEAD_BACKUP + HIFI_SEC_HEAD_SIZE)					/*Watchdog flag*/
#define HIFI_RESERVE2_LOCATION			(HIFI_WTD_FLAG_BASE + HIFI_WTD_FLAG_SIZE)                   /*reserv2*/
#define HIFI_IMAGE_LOCATION 			(HIFI_RESERVE2_LOCATION + HIFI_RESERVE2_SIZE)               /*Code backup*/

#define HIFI_WTD_FLAG_NMI              (HIFI_WTD_FLAG_BASE + 0x8)

#define HIFI_OFFSET_MUSIC_DATA			(HIFI_RUN_SIZE)
#define HIFI_OFFSET_IMG 				(HIFI_SIZE - HIFI_IMAGE_SIZE)
#define HIFI_OFFSET_RUN 				(0x0)

#if 1 /*hifi define*/
#define DRV_HIFI_DDR_BASE_ADDR			(HIFI_BASE_ADDR)
#define DRV_HIFI_SOCP_DDR_BASE_ADDR 	(HIFI_SOCP_LOCATION)
#define DRV_DSP_PANIC_MARK				(HIFI_RESERVE1_LOCATION)
#define DRV_DSP_UART_LOG_LEVEL			(HIFI_RESERVE1_LOCATION + 4)
#define DRV_DSP_UART_TO_MEM_CUR_ADDR	(DRV_DSP_UART_LOG_LEVEL + 4)
#define DRV_DSP_EXCEPTION_NO			(DRV_DSP_UART_TO_MEM_CUR_ADDR + 4)
#define DRV_DSP_IDLE_COUNT_ADDR			(DRV_DSP_EXCEPTION_NO + 4)
#define DRV_DSP_LOADED_INDICATE			(DRV_DSP_IDLE_COUNT_ADDR + 4)
#define DRV_DSP_KILLME_ADDR 			(DRV_DSP_LOADED_INDICATE + 4) /*only for debug, 0x37d82018*/
#define DRV_DSP_WRITE_MEM_PRINT_ADDR	(DRV_DSP_KILLME_ADDR + 4)
#define DRV_DSP_POWER_STATUS_ADDR		(DRV_DSP_WRITE_MEM_PRINT_ADDR + 4)			/*0x37c02020*/
#define DRV_DSP_POWER_ON				(0x55AA55AA)
#define DRV_DSP_POWER_OFF				(0x55FF55FF)
#define DRV_DSP_KILLME_VALUE			(0xA5A55A5A)
#endif

#define HIFI_OCRAM_BASE_ADDR			(0xE8000000)
#define HIFI_TCM_BASE_ADDR				(0xE8058000)

#define DRV_DSP_STACK_TO_MEM_SIZE       (0x1000)
#define DRV_DSP_STACK_TO_MEM            (HIFI_BASE_ADDR+HIFI_RUN_SIZE-DRV_DSP_STACK_TO_MEM_SIZE)

#define DRV_DSP_UART_TO_MEM 			(0x3FE80000)
#define DRV_DSP_UART_TO_MEM_SIZE		(512*1024)
#define DRV_DSP_UART_TO_MEM_RESERVE_SIZE (10*1024)

#define HIFI_SIZE_MUSIC_DATA			(HIFI_MUSIC_DATA_SIZE)
#define SIZE_PARAM_PRIV 				(200 * 1024)

#define SYS_TIME_STAMP_REG				(0xFFF0A534)
#endif

#ifdef PLATFORM_HI6XXX
#define OFFSET_HIFI_PRIV		(0x00300000)
#define HIFI_MUSIC_DATA_SIZE	(0x100000) /* LowPowerPlayer.cpp:MEM_BUFF_SIZE */
#define HIFI_PRIV_ADDR			(HIFI_SYS_MEM_ADDR + OFFSET_HIFI_PRIV)
#define SIZE_PARAM_PRIV 		(200 * 1024)
#endif

/* 接收HIFI消息，前部cmd_id占用的字节数 */
#define SIZE_CMD_ID 	   (8)

/* notice主动上报一次缓冲 */
#define REV_MSG_NOTICE_ID_MAX		2

#define ACPU_TO_HIFI_ASYNC_CMD	  0xFFFFFFFF

#define BUFFER_NUM	8

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

typedef enum HIFI_MSG_ID_ {

	/*DTS command id from ap*/
	ID_AP_AUDIO_SET_DTS_ENABLE_CMD		= 0xDD36,
	ID_AP_AUDIO_SET_DTS_DEV_CMD			= 0xDD38,
	ID_AP_AUDIO_SET_DTS_GEQ_CMD			= 0xDD39,
	ID_AP_AUDIO_SET_DTS_GEQ_ENABLE_CMD	= 0xDD3B,

	/* Voice Record */
	ID_AP_HIFI_VOICE_RECORD_START_CMD	= 0xDD40,
	ID_AP_HIFI_VOICE_RECORD_STOP_CMD	= 0xDD41,

	/* voicePP MSG_ID */
	ID_AP_VOICEPP_START_REQ				= 0xDD42,						/* 启动VOICEPP通话 */
	ID_VOICEPP_MSG_START				= ID_AP_VOICEPP_START_REQ,
	ID_VOICEPP_AP_START_CNF				= 0xDD43,
	ID_AP_VOICEPP_STOP_REQ				= 0xDD44,						/* 终止VOICEPP通话 */
	ID_VOICEPP_AP_STOP_CNF				= 0xDD45,
	ID_AP_VOICEPP_SET_DEVICE_REQ		= 0xDD46,						/* 配置输入输出设备 */
	ID_VOICEPP_AP_SET_DEVICE_CNF		= 0xDD47,
	ID_AP_VOICEPP_SET_WB_REQ			= 0xDD48,						/* 设置宽窄带 */
	ID_VOICEPP_AP_SET_WB_CNF			= 0xDD49,
	ID_VOICEPP_MSG_END					= 0xDD4A,

	ID_AP_AUDIO_PLAY_START_REQ			= 0xDD51,/* AP启动Hifi audio player request命令 */
	ID_AUDIO_AP_PLAY_START_CNF			= 0xDD52,/* Hifi启动audio player后回复AP confirm命令 */
	ID_AP_AUDIO_PLAY_PAUSE_REQ			= 0xDD53,/* AP停止Hifi audio player request命令 */
	ID_AUDIO_AP_PLAY_PAUSE_CNF			= 0xDD54,/* Hifi停止audio player后回复AP confirm命令 */
	ID_AUDIO_AP_PLAY_DONE_IND			= 0xDD56,/* Hifi通知AP audio player一块数据播放完毕或者播放中断indication */
	ID_AP_AUDIO_PLAY_UPDATE_BUF_CMD 	= 0xDD57,/* AP通知Hifi新数据块更新command */
	ID_AP_AUDIO_PLAY_QUERY_TIME_REQ 	= 0xDD59,/* AP查询Hifi audio player播放进度request命令 */
	ID_AP_AUDIO_PLAY_WAKEUPTHREAD_REQ	= 0xDD5A,
	ID_AUDIO_AP_PLAY_QUERY_TIME_CNF 	= 0xDD60,/* Hifi回复AP audio player播放进度confirm命令 */
	ID_AP_AUDIO_PLAY_QUERY_STATUS_REQ	= 0xDD61,/* AP查询Hifi audio player播放状态request命令 */
	ID_AUDIO_AP_PLAY_QUERY_STATUS_CNF	= 0xDD62,/* Hifi回复AP audio player播放状态confirm命令 */
	ID_AP_AUDIO_PLAY_SEEK_REQ			= 0xDD63,/* AP seek Hifi audio player到某一位置request命令 */
	ID_AUDIO_AP_PLAY_SEEK_CNF			= 0xDD64,/* Hifi回复AP seek结果confirm命令 */
	ID_AP_AUDIO_PLAY_SET_VOL_CMD		= 0xDD70,/* AP设置音量命令 */
	ID_AP_AUDIO_RECORD_PCM_HOOK_CMD		= 0xDD7A,/* AP 通知HIFI开始抓取PCM数据 */
	/* enhance msgid between ap and hifi */
	ID_AP_HIFI_ENHANCE_START_REQ		= 0xDD81,
	ID_HIFI_AP_ENHANCE_START_CNF		= 0xDD82,
	ID_AP_HIFI_ENHANCE_STOP_REQ 		= 0xDD83,
	ID_HIFI_AP_ENHANCE_STOP_CNF 		= 0xDD84,
	ID_AP_HIFI_ENHANCE_SET_DEVICE_REQ	= 0xDD85,
	ID_HIFI_AP_ENHANCE_SET_DEVICE_CNF	= 0xDD86,

	/* audio enhance msgid between ap and hifi */
	ID_AP_AUDIO_ENHANCE_SET_DEVICE_IND	= 0xDD91,
	ID_AP_AUDIO_MLIB_SET_PARA_IND		= 0xDD92,
	ID_AP_AUDIO_CMD_SET_SOURCE_CMD		= 0xDD95,
	ID_AP_AUDIO_CMD_SET_DEVICE_CMD		= 0xDD96,
	ID_AP_AUDIO_CMD_SET_MODE_CMD		= 0xDD97,
} HIFI_MSG_ID;

/*处理hifi回复消息，记录cmd_id和数据*/
typedef struct {
	unsigned char *mail_buff;		/* 邮箱数据接收的buff */
	unsigned int mail_buff_len;
	unsigned int cmd_id;			/* 邮箱接收数据前4个字节是cmd_id */
	unsigned char *out_buff_ptr;	/* 指向mail_buff cmd_id后的位置 */
	unsigned int out_buff_len;
} rev_msg_buff;

struct recv_request {
	struct list_head recv_node;
	rev_msg_buff rev_msg;
};

struct misc_recmsg_param {
	unsigned short	msgID;
	unsigned short	playStatus;
};

struct temp_hifi_cmd{
	unsigned short	msgID;
	unsigned short	value;
};

#ifdef PLATFORM_HI3XXX
int hifi_send_msg(unsigned int mailcode, void *data, unsigned int length);
void hifi_get_log_signal(void);
void hifi_release_log_signal(void);
void sochifi_watchdog_send_event(void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hifi_lpp.h */

