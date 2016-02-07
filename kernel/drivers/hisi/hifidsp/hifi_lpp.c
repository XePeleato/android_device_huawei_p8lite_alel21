/*
 * hifi misc driver.
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/wakelock.h>
#include <linux/errno.h>
#include <linux/of_address.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/completion.h>
#include <linux/suspend.h>
#include <linux/reboot.h>
#include <linux/delay.h>

#include <asm/memory.h>
#include <asm/types.h>
#include <asm/io.h>

#ifdef CONFIG_ARM64
#include <linux/compat.h>
#endif

#ifdef PLATFORM_HI6XXX
#include <linux/hisi/util.h>
#include <linux/of.h>
#include <trace/trace_kernel.h>
#include <../mailbox/hi6xxx_mailbox/bsp_mailbox.h>
#include <../mailbox/hi6xxx_mailbox/drv_mailbox_cfg.h>

#endif


#include "hifi_lpp.h"
#include "audio_hifi.h"

#ifdef PLATFORM_HI3XXX
#include "drv_mailbox_msg.h"
#include "bsp_drv_ipc.h"
#endif
#include "hifi_om.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef PLATFORM_HI3XXX
#define DTS_COMP_HIFIDSP_NAME "hisilicon,k3hifidsp"
#define SEND_MSG_TO_HIFI mailbox_send_msg
#endif

#ifdef PLATFORM_HI6XXX
#define DTS_COMP_HIFIDSP_NAME "hifi_dsp_misc"
#define SEND_MSG_TO_HIFI DRV_MAILBOX_SENDMAIL
#endif

static DEFINE_SEMAPHORE(s_misc_sem);

LIST_HEAD(recv_sync_work_queue_head);
LIST_HEAD(recv_proc_work_queue_head);

struct hifi_misc_priv {
	spinlock_t	recv_sync_lock;
	spinlock_t	recv_proc_lock;

	struct completion completion;
	wait_queue_head_t proc_waitq;

	int			wait_flag;
	unsigned int	sn;

	struct wake_lock hifi_misc_wakelock;

	unsigned char*	hifi_priv_base_virt;
	unsigned char*	hifi_priv_base_phy;
	struct device	*dev;
};
static struct hifi_misc_priv s_misc_data;

#ifdef PLATFORM_HI3XXX
static struct notifier_block s_hifi_sr_nb;
static struct notifier_block s_hifi_reboot_nb;
static atomic_t volatile s_hifi_in_suspend = ATOMIC_INIT(0);
static atomic_t volatile s_hifi_in_saving = ATOMIC_INIT(0);
#endif

void sochifi_watchdog_send_event(void)
{
	hifi_dump_panic_log();
	logi("soc hifi watchdog coming, now reset mediaserver \n");
	char *envp[2] = {"hifi_watchdog", NULL};
	kobject_uevent_env(&s_misc_data.dev->kobj, KOBJ_CHANGE, envp);
}

static void hifi_misc_msg_info(unsigned short _msg_id)
{
	HIFI_MSG_ID msg_id =  (HIFI_MSG_ID)_msg_id;

	switch(msg_id) {
		case ID_AP_AUDIO_PLAY_START_REQ:
			logi("MSG: ID_AP_AUDIO_PLAY_START_REQ.\n");
			break;
		case ID_AUDIO_AP_PLAY_START_CNF:
			logi("MSG: ID_AUDIO_AP_PLAY_START_CNF.\n");
			break;
		case ID_AP_AUDIO_PLAY_PAUSE_REQ:
			logi("MSG: ID_AP_AUDIO_PLAY_PAUSE_REQ.\n");
			break;
		case ID_AUDIO_AP_PLAY_PAUSE_CNF:
			logi("MSG: ID_AUDIO_AP_PLAY_PAUSE_CNF.\n");
			break;
		case ID_AUDIO_AP_PLAY_DONE_IND:
			logi("MSG: ID_AUDIO_AP_PLAY_DONE_IND.\n");
			break;
		case ID_AP_AUDIO_PLAY_UPDATE_BUF_CMD:
			logi("MSG: ID_AP_AUDIO_PLAY_UPDATE_BUF_CMD.\n");
			break;
		case ID_AP_AUDIO_PLAY_WAKEUPTHREAD_REQ:
			logi("MSG: ID_AP_AUDIO_PLAY_WAKEUPTHREAD_REQ.\n");
			break;
		case ID_AP_AUDIO_PLAY_QUERY_TIME_REQ:
			logd("MSG: ID_AP_AUDIO_PLAY_QUERY_TIME_REQ.\n");
			break;
		case ID_AUDIO_AP_PLAY_QUERY_TIME_CNF:
			logd("MSG: ID_AUDIO_AP_PLAY_QUERY_TIME_CNF.\n");
			break;
		case ID_AP_AUDIO_PLAY_QUERY_STATUS_REQ:
			logd("MSG: ID_AP_AUDIO_PLAY_QUERY_STATUS_REQ.\n");
			break;
		case ID_AUDIO_AP_PLAY_QUERY_STATUS_CNF:
			logd("MSG: ID_AUDIO_AP_PLAY_QUERY_STATUS_CNF.\n");
			break;
		case ID_AP_AUDIO_PLAY_SEEK_REQ:
			logi("MSG: ID_AP_AUDIO_PLAY_SEEK_REQ.\n");
			break;
		case ID_AUDIO_AP_PLAY_SEEK_CNF:
			logi("MSG: ID_AUDIO_AP_PLAY_SEEK_CNF.\n");
			break;
		case ID_AP_AUDIO_PLAY_SET_VOL_CMD:
			logi("MSG: ID_AP_AUDIO_PLAY_SET_VOL_CMD.\n");
			break;
		case ID_AP_HIFI_ENHANCE_START_REQ:
			logi("MSG: ID_AP_HIFI_ENHANCE_START_REQ.\n");
			break;
		case ID_HIFI_AP_ENHANCE_START_CNF:
			logi("MSG: ID_HIFI_AP_ENHANCE_START_CNF.\n");
			break;
		case ID_AP_HIFI_ENHANCE_STOP_REQ:
			logi("MSG: ID_AP_HIFI_ENHANCE_STOP_REQ.\n");
			break;
		case ID_HIFI_AP_ENHANCE_STOP_CNF:
			logi("MSG: ID_HIFI_AP_ENHANCE_STOP_CNF.\n");
			break;
		case ID_AP_HIFI_ENHANCE_SET_DEVICE_REQ:
			logi("MSG: ID_AP_HIFI_ENHANCE_SET_DEVICE_REQ.\n");
			break;
		case ID_HIFI_AP_ENHANCE_SET_DEVICE_CNF:
			logi("MSG: ID_HIFI_AP_ENHANCE_SET_DEVICE_CNF.\n");
			break;
		case ID_AP_AUDIO_ENHANCE_SET_DEVICE_IND:
			logi("MSG: ID_AP_AUDIO_ENHANCE_SET_DEVICE_IND.\n");
			break;
		case ID_AP_AUDIO_MLIB_SET_PARA_IND:
			logi("MSG: ID_AP_AUDIO_MLIB_SET_PARA_IND.\n");
			break;
		case ID_AP_HIFI_VOICE_RECORD_START_CMD:
			logi("MSG: ID_AP_HIFI_VOICE_RECORD_START_CMD.\n");
			break;
		case ID_AP_HIFI_VOICE_RECORD_STOP_CMD:
			logi("MSG: ID_AP_HIFI_VOICE_RECORD_STOP_CMD.\n");
			break;
		case ID_AP_VOICEPP_START_REQ:
			logi("MSG: ID_AP_VOICEPP_START_REQ .\n");
			break;
		case ID_VOICEPP_AP_START_CNF:
			logi("MSG: ID_VOICEPP_AP_START_CNF .\n");
			break;
		case ID_AP_VOICEPP_STOP_REQ:
			logi("MSG: ID_AP_VOICEPP_STOP_REQ .\n");
			break;
		case ID_VOICEPP_AP_STOP_CNF:
			logi("MSG: ID_VOICEPP_AP_STOP_CNF .\n");
			break;
		case ID_AP_VOICEPP_SET_DEVICE_REQ:
			logi("MSG: ID_AP_VOICEPP_SET_DEVICE_REQ .\n");
			break;
		case ID_VOICEPP_AP_SET_DEVICE_CNF:
			logi("MSG: ID_VOICEPP_AP_SET_DEVICE_CNF .\n");
			break;
		case ID_AP_VOICEPP_SET_WB_REQ:
			logi("MSG: ID_AP_VOICEPP_SET_WB_REQ .\n");
			break;
		case ID_VOICEPP_AP_SET_WB_CNF:
			logi("MSG: ID_VOICEPP_AP_SET_WB_CNF .\n");
			break;
		case ID_AP_AUDIO_SET_DTS_ENABLE_CMD:
			logi("MSG: ID_AP_AUDIO_SET_DTS_ENABLE_CMD.\n");
			break;
		case ID_AP_AUDIO_SET_DTS_DEV_CMD:
			logi("MSG: ID_AP_AUDIO_SET_DTS_DEV_CMD.\n");
			break;
		case ID_AP_AUDIO_SET_DTS_GEQ_CMD:
			logi("MSG: ID_AP_AUDIO_SET_DTS_GEQ_CMD.\n");
			break;
		case ID_AP_AUDIO_SET_DTS_GEQ_ENABLE_CMD:
			logi("MSG: ID_AP_AUDIO_SET_DTS_GEQ_ENABLE_CMD.\n");
			break;
		case ID_AP_AUDIO_RECORD_PCM_HOOK_CMD:
			logi("MSG: ID_AP_AUDIO_RECORD_PCM_HOOK_CMD.\n");
			break;
		case ID_AP_AUDIO_CMD_SET_SOURCE_CMD:
			logi("MSG: ID_AP_AUDIO_CMD_SET_SOURCE_CMD.\n");
			break;
		case ID_AP_AUDIO_CMD_SET_DEVICE_CMD:
			logi("MSG: ID_AP_AUDIO_CMD_SET_DEVICE_CMD.\n");
			break;
		case ID_AP_AUDIO_CMD_SET_MODE_CMD:
			logi("MSG: ID_AP_AUDIO_CMD_SET_MODE_CMD.\n");
			break;
		default:
			logw("MSG: Not defined msg id: 0x%x.\n", msg_id);
			break;
	}
	return;
}

/*****************************************************************************
 函 数 名  : hifi_misc_async_write
 功能描述  : Hifi MISC 设备异步发送接口，将异步消息发给Hifi，非阻塞接口
 输入参数  :
			 unsigned char *arg  :需要下发的数据buff指针
			 unsigned int len	 :下发的数据长度
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_misc_async_write(unsigned char *arg, unsigned int len)
{
	int ret = OK;

	IN_FUNCTION;

	if (NULL == arg) {
		loge("input arg is NULL.\n");
		ret = ERROR;
		goto END;
	}

	/*调用核间通信接口发送数据*/
	ret = SEND_MSG_TO_HIFI(MAILBOX_MAILCODE_ACPU_TO_HIFI_MISC, arg, len);
	if (OK != ret) {
		loge("msg send to hifi fail,ret is %d.\n", ret);
		ret = ERROR;
		goto END;
	}

END:
	OUT_FUNCTION;
	return ret;
}

/*****************************************************************************
 函 数 名  : hifi_misc_sync_write
 功能描述  : Hifi MISC 设备同步发送接口，将同步消息发给Hifi，阻塞接口
 输入参数  :
			 unsigned char	*buff  :需要下发的数据buff指针
			 unsigned int len	  :下发的数据长度
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_misc_sync_write(unsigned char  *buff, unsigned int len)
{
	int ret = OK;

	IN_FUNCTION;

	if (NULL == buff) {
		loge("input arg is NULL.\n");
		ret = ERROR;
		goto END;
	}

	INIT_COMPLETION(s_misc_data.completion);

	/*调用核间通信接口发送数据，得到返回值ret*/
	ret = SEND_MSG_TO_HIFI(MAILBOX_MAILCODE_ACPU_TO_HIFI_MISC, buff, len);
	if (OK != ret) {
		loge("msg send to hifi fail,ret is %d.\n", ret);
		ret = ERROR;
		goto END;
	}

	ret = wait_for_completion_timeout(&s_misc_data.completion, msecs_to_jiffies(2000));
	s_misc_data.sn++;
	if (unlikely(s_misc_data.sn & 0x10000000)){
		s_misc_data.sn = 0;
	}

	if (!ret) {
		loge("wait completion timeout.\n");
		hifi_dump_panic_log();
		ret = ERROR;
		goto END;
	} else {
		ret = OK;
	}

END:
	OUT_FUNCTION;
	return ret;
}

/*****************************************************************************
 函 数 名  : hifi_misc_handle_mail
 功能描述  : Hifi MISC 设备双核通信接收中断处理函数
			约定收到HIIF邮箱消息的首4个字节是MSGID
 输入参数  : void *usr_para			: 注册时传递的参数
			 void *mail_handle			: 邮箱数据参数
			 unsigned int mail_len		: 邮箱数据长度
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static void hifi_misc_handle_mail(void *usr_para, void *mail_handle, unsigned int mail_len)
{
	unsigned int ret_mail			= 0;
	struct recv_request *recv = NULL;
	HIFI_CHN_CMD *cmd_para = NULL;

	IN_FUNCTION;

	if (NULL == mail_handle) {
		loge("mail_handle is NULL.\n");
		goto END;
	}

	if (mail_len <= SIZE_CMD_ID) {
		loge("mail_len is less than SIZE_CMD_ID(%d).\n", mail_len);
		goto END;
	}

	recv = (struct recv_request *)kmalloc(sizeof(struct recv_request), GFP_ATOMIC);
	if (NULL == recv)
	{
		loge("recv kmalloc failed.\n");
		goto ERR;
	}
	memset(recv, 0, sizeof(struct recv_request));

	/* 设定SIZE */
	recv->rev_msg.mail_buff_len = mail_len;
	/* 分配总的空间 */
	recv->rev_msg.mail_buff = (unsigned char *)kmalloc(SIZE_LIMIT_PARAM, GFP_ATOMIC);
	if (NULL == recv->rev_msg.mail_buff)
	{
		loge("recv->rev_msg.mail_buff kmalloc failed.\n");
		goto ERR;
	}
	memset(recv->rev_msg.mail_buff, 0, SIZE_LIMIT_PARAM);

	/* 将剩余内容copy透传到buff中 */
#ifdef PLATFORM_HI3XXX
	ret_mail = mailbox_read_msg_data(mail_handle, (char*)(recv->rev_msg.mail_buff), (unsigned int *)(&(recv->rev_msg.mail_buff_len)));
#endif
#ifdef PLATFORM_HI6XXX
	ret_mail = DRV_MAILBOX_READMAILDATA(mail_handle, (char*)recv->rev_msg.mail_buff, (unsigned int *)&recv->rev_msg.mail_buff_len);
#endif
	if ((ret_mail != MAILBOX_OK) || (recv->rev_msg.mail_buff_len <= 0)) {
		loge("Empty point or data length error! ret=0x%x, mail_size: %d.\n", (unsigned int)ret_mail, recv->rev_msg.mail_buff_len);
		goto ERR;
	}

	logd("ret_mail=%d, mail_buff_len=%d, msgID=0x%x.\n", ret_mail, recv->rev_msg.mail_buff_len,
		 *((unsigned int *)(recv->rev_msg.mail_buff + mail_len - SIZE_CMD_ID)));

	if (recv->rev_msg.mail_buff_len > mail_len) {
		loge("ReadMailData mail_size(%d) > mail_len(%d).\n", recv->rev_msg.mail_buff_len, mail_len);
		goto ERR;
	}

	/* 约定，前4个字节是cmd_id */
	cmd_para   = (HIFI_CHN_CMD *)(recv->rev_msg.mail_buff + mail_len - SIZE_CMD_ID);
	/* 赋予不同的接收指针，由接收者释放分配空间 */
	if (HIFI_CHN_SYNC_CMD == cmd_para->cmd_type) {
		if (s_misc_data.sn == cmd_para->sn) {
#ifdef PLATFORM_HI6XXX
			wake_lock_timeout(&s_misc_data.hifi_misc_wakelock, HZ / 2);
#endif
			spin_lock_bh(&s_misc_data.recv_sync_lock);
			list_add_tail(&recv->recv_node, &recv_sync_work_queue_head);
			spin_unlock_bh(&s_misc_data.recv_sync_lock);
			complete(&s_misc_data.completion);
		} else {
			loge("s_misc_data.sn !== cmd_para->sn: %d, %d.\n", s_misc_data.sn, cmd_para->sn);
		}
		goto END;
	} else if ((HIFI_CHN_READNOTICE_CMD == cmd_para->cmd_type) && (ACPU_TO_HIFI_ASYNC_CMD == cmd_para->sn)) {
#ifdef PLATFORM_HI3XXX
		if (HIFI_CHN_READNOTICE_CMD == cmd_para->cmd_type) {
			wake_lock_timeout(&s_misc_data.hifi_misc_wakelock, 5*HZ);
		}
#endif
#ifdef PLATFORM_HI6XXX
		wake_lock_timeout(&s_misc_data.hifi_misc_wakelock, HZ);
#endif
		spin_lock_bh(&s_misc_data.recv_proc_lock);
		list_add_tail(&recv->recv_node, &recv_proc_work_queue_head);
		s_misc_data.wait_flag++;
		spin_unlock_bh(&s_misc_data.recv_proc_lock);
		wake_up(&s_misc_data.proc_waitq);
		goto END;
	} else {
		loge("unknown msg comed from hifi .\n");
	}

ERR:
	if (recv) {
		if (recv->rev_msg.mail_buff) {
			kfree(recv->rev_msg.mail_buff);
		}
		kfree(recv);
	}

END:
	OUT_FUNCTION;
	return;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_get_input_param
 功能描述  : 获取用户空间入参，并转换为内核空间入参
 输入参数  : usr_para_size，用户空间入参SIZE
			usr_para_addr，用户空间入参地址
 输出参数  : krn_para_size，转换后的内核空间入参SIZE
			krn_para_addr，转换后的内核空间入参地址
 返 回 值  : OK / ERROR
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年3月26日
	作	  者   : s00212991
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_get_input_param(unsigned int usr_para_size, void *usr_para_addr,
									unsigned int *krn_para_size, void **krn_para_addr)
{
	void *para_in = NULL;
	unsigned int para_size_in = 0;

	IN_FUNCTION;

	/*获取arg入参*/
	para_size_in = usr_para_size + SIZE_CMD_ID;

	/* 限制分配空间 */
	if (para_size_in > SIZE_LIMIT_PARAM) {
		loge("para_size_in exceed LIMIT(%u/%u).\n", para_size_in, SIZE_LIMIT_PARAM);
		goto ERR;
	}

	para_in = kzalloc(para_size_in, GFP_KERNEL);
	if (NULL == para_in) {
		loge("kzalloc fail.\n");
		goto ERR;
	}

	if (NULL != usr_para_addr) {
		if (copy_from_user(para_in , usr_para_addr, usr_para_size)) {
			loge("copy_from_user fail.\n");
			goto ERR;
		}
	} else {
		loge("usr_para_addr is null no user data.\n");
		goto ERR;
	}

	/* 设置出参 */
	*krn_para_size = para_size_in;
	*krn_para_addr = para_in;

	hifi_misc_msg_info(*(unsigned short*)para_in);

	OUT_FUNCTION;
	return OK;

ERR:
	if (para_in != NULL) {
		kfree(para_in);
		para_in = NULL;
	}

	OUT_FUNCTION;
	return ERROR;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_get_input_param_free
 功能描述  : 释放分配的内核空间
 输入参数  : krn_para_addr，待释放的内核空间地址
 输出参数  : 无
 返 回 值  : OK / ERROR
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年3月26日
	作	  者   : s00212991
	修改内容   : 新生成函数

*****************************************************************************/
static void hifi_dsp_get_input_param_free(void **krn_para_addr)
{
	IN_FUNCTION;

	if (*krn_para_addr != NULL) {
		kfree(*krn_para_addr);
		*krn_para_addr = NULL;
	} else {
		loge("krn_para_addr to free is NULL.\n");
	}

	OUT_FUNCTION;
	return;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_get_output_param
 功能描述  : 内核空间出参转换为用户空间出参
 输入参数  : krn_para_size，转换后的内核空间出参SIZE
			krn_para_addr，转换后的内核空间出参地址
 输出参数  : usr_para_size，用户空间出参SIZE
			usr_para_addr，用户空间出参地址
 返 回 值  : OK / ERROR
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年3月26日
	作	  者   : s00212991
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_get_output_param(unsigned int krn_para_size, void *krn_para_addr,
									 unsigned int *usr_para_size, void *usr_para_addr)
{
	int ret			= OK;
	void *para_to = NULL;
	unsigned int para_n = 0;

	IN_FUNCTION;

	/* 入参判定 */
	if (NULL == krn_para_addr) {
		loge("krn_para_addr is NULL.\n");
		ret = -EINVAL;
		goto END;
	}

	/* 入参判定 */
	if ((NULL == usr_para_addr) || (NULL == usr_para_size)) {
		loge("usr_size_p=0x%p, usr_addr=0x%p.\n", usr_para_size, usr_para_addr);
		ret = -EINVAL;
		goto END;
	}

	para_to = usr_para_addr;
	para_n = krn_para_size;
	if (para_n > SIZE_LIMIT_PARAM) {
		loge("para_n exceed limit (%d / %d).\n", para_n, SIZE_LIMIT_PARAM);
		ret = -EINVAL;
		goto END;
	}

	if (para_n > *usr_para_size) {
		loge("para_n exceed usr_size(%d / %d).\n", para_n, *usr_para_size);
		ret = -EINVAL;
		goto END;
	}

	/* Copy data from kernel space to user space
		to, from, n */
	ret = copy_to_user(para_to, krn_para_addr, para_n);
	if (OK != ret) {
		loge("copy_to_user fail, ret is %d.\n", ret);
		ret = ERROR;
		goto END;
	}

	*usr_para_size = para_n;
	hifi_misc_msg_info(*(unsigned short*)para_to);

END:
	OUT_FUNCTION;
	return ret;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_async_cmd
 功能描述  : Hifi MISC IOCTL异步命令处理函数
 输入参数  : unsigned long arg : ioctl的入参
 输出参数  : 无
 返 回 值  : 成功:OK 失败:BUSY
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2013年3月18日
	作	  者   : 石旺来 00212991
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_async_cmd(unsigned long arg)
{
	int ret = OK;
	struct misc_io_async_param param;
	void *para_krn_in = NULL;
	unsigned int para_krn_size_in = 0;
	HIFI_CHN_CMD *cmd_para = NULL;
	void* para_addr_in = NULL;

	IN_FUNCTION;

	if (copy_from_user(&param,(void*) arg, sizeof(struct misc_io_async_param))) {
		loge("copy_from_user fail.\n");
		ret = ERROR;
		goto END;
	}

	para_addr_in = INT_TO_ADDR(param.para_in_l,param.para_in_h);
	/*获取arg入参*/
	ret = hifi_dsp_get_input_param(param.para_size_in, para_addr_in,
								   &para_krn_size_in, &para_krn_in);
	if (OK != ret) {
		loge("get ret=%d.\n", ret);
		goto END;
	}
	/* add cmd id and sn  */
	cmd_para = (HIFI_CHN_CMD *)(para_krn_in+para_krn_size_in-SIZE_CMD_ID);
	cmd_para->cmd_type = HIFI_CHN_SYNC_CMD;
	cmd_para->sn = ACPU_TO_HIFI_ASYNC_CMD;

	/*邮箱发送至HIFI, 异步*/
	ret = hifi_misc_async_write(para_krn_in, para_krn_size_in);
	if (OK != ret) {
		loge("async_write ret=%d.\n", ret);
		goto END;
	}

END:
	hifi_dsp_get_input_param_free(&para_krn_in);
	OUT_FUNCTION;
	return ret;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_sync_cmd
 功能描述  : Hifi MISC IOCTL同步命令处理函数
 输入参数  : unsigned long arg : ioctl的入参
 输出参数  : 无
 返 回 值  : 成功:OK 失败:BUSY
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2013年3月18日
	作	  者   : 石旺来 00212991
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_sync_cmd(unsigned long arg)
{
	int ret = OK;
	struct misc_io_sync_param param;
	void *para_krn_in = NULL;
	unsigned int para_krn_size_in = 0;
	HIFI_CHN_CMD *cmd_para = NULL;
	void* para_addr_in	= NULL;
	void* para_addr_out = NULL;
	struct recv_request *recv = NULL;

	IN_FUNCTION;

	if (copy_from_user(&param,(void*) arg, sizeof(struct misc_io_sync_param))) {
		loge("copy_from_user fail.\n");
		ret = ERROR;
		goto END;
	}
	logd("para_size_in=%d.\n", param.para_size_in);

	para_addr_in   = INT_TO_ADDR(param.para_in_l ,param.para_in_h);
	para_addr_out  = INT_TO_ADDR(param.para_out_l,param.para_out_h);
	/*获取arg入参*/
	ret = hifi_dsp_get_input_param(param.para_size_in, para_addr_in,
						&para_krn_size_in, &para_krn_in);
	if (OK != ret) {
		loge("hifi_dsp_get_input_param fail: ret=%d.\n", ret);
		goto END;
	}

	/* add cmd id and sn  */
	cmd_para = (HIFI_CHN_CMD *)(para_krn_in+para_krn_size_in-SIZE_CMD_ID);
	cmd_para->cmd_type = HIFI_CHN_SYNC_CMD;

	cmd_para->sn = s_misc_data.sn;

	/*邮箱发送至HIFI, 同步*/
	ret = hifi_misc_sync_write(para_krn_in, para_krn_size_in);
	if (OK != ret) {
		loge("hifi_misc_sync_write ret=%d.\n", ret);
		goto END;
	}

	/*将获得的rev_msg信息填充到出参arg*/
	spin_lock_bh(&s_misc_data.recv_sync_lock);

	if (!list_empty(&recv_sync_work_queue_head)) {
		recv = list_entry(recv_sync_work_queue_head.next, struct recv_request, recv_node);
		ret = hifi_dsp_get_output_param(recv->rev_msg.mail_buff_len- SIZE_CMD_ID, recv->rev_msg.mail_buff,
						&param.para_size_out, para_addr_out);
		if (OK != ret) {
			loge("get_out ret=%d.\n", ret);
		}

		list_del(&recv->recv_node);
		kfree(recv->rev_msg.mail_buff);
		kfree(recv);
		recv = NULL;
	}
	spin_unlock_bh(&s_misc_data.recv_sync_lock);

	if (copy_to_user((void*)arg, &param, sizeof(struct misc_io_sync_param))) {
		loge("copy_to_user fail.\n");
		ret = ERROR;
		goto END;
	}

END:
	/*释放krn入参*/
	hifi_dsp_get_input_param_free(&para_krn_in);

	OUT_FUNCTION;
	return ret;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_get_phys_cmd
 功能描述  : Hifi MISC IOCTL获取物理地址
 输入参数  : unsigned long arg : ioctl的入参
 输出参数  : 无
 返 回 值  : 成功:OK 失败:BUSY
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2013年3月18日
	作	  者   : 石旺来 00212991
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_get_phys_cmd(unsigned long arg)
{
	int ret  =	OK;
	struct misc_io_get_phys_param param;
	unsigned long para_addr_in = 0;

	IN_FUNCTION;

	if (copy_from_user(&param,(void*) arg, sizeof(struct misc_io_get_phys_param))) {
		loge("copy_from_user fail.\n");
		OUT_FUNCTION;
		return ERROR;
	}

	switch (param.flag)
	{
		case 0:
			para_addr_in = (unsigned long)s_misc_data.hifi_priv_base_phy;
			param.phys_addr_l = GET_LOW32(para_addr_in);
			param.phys_addr_h = GET_HIG32(para_addr_in);
			logd("para_addr_in = 0x%ld.\n", para_addr_in);
			break;

		default:
			ret = ERROR;
			loge("invalid flag=%d.\n", param.flag);
			break;
	}

	if (copy_to_user((void*)arg, &param, sizeof(struct misc_io_get_phys_param))) {
		loge("copy_to_user fail.\n");
		ret = ERROR;
	}

	OUT_FUNCTION;
	return ret;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_senddata_sync_cmd
 功能描述  : Hifi MISC IOCTL发送数据同步命令处理函数
 输入参数  : unsigned long arg : ioctl的入参
 输出参数  : 无
 返 回 值  : 成功:OK 失败:BUSY
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2013年3月18日
	作	  者   : 石旺来 00212991
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_senddata_sync_cmd(unsigned long arg)
{
	loge("this cmd is not supported by now .\n");
	return ERROR;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_test_cmd
 功能描述  : 测试函数
 输入参数  : unsigned long arg : ioctl的入参
 输出参数  : 无
 返 回 值  : 成功:OK 失败:BUSY
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2013年3月18日
	作	  者   : 石旺来 00212991
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_test_cmd(unsigned long arg)
{
	loge("this cmd is not supported by now .\n");
	return ERROR;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_wakeup_read_thread
 功能描述  : 唤醒read线程, 返回RESET消息
 输入参数  : unsigned long arg : ioctl的入参
 输出参数  : 无
 返 回 值  : 成功:OK 失败:BUSY
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2014年4月24日
	作	  者   : 张恩忠 00222844
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_wakeup_read_thread(unsigned long arg)
{
	struct recv_request *recv = NULL;
	struct misc_recmsg_param *recmsg = NULL;
	unsigned int wake_cmd = (unsigned int)arg;

	recv = (struct recv_request *)kmalloc(sizeof(struct recv_request), GFP_ATOMIC);
	if (NULL == recv)
	{
		loge("recv kmalloc failed.\n");
		return -ENOMEM;
	}

    wake_lock_timeout(&s_misc_data.hifi_misc_wakelock, HZ);

	memset(recv, 0, sizeof(struct recv_request));

	/* 分配总的空间 */
	recv->rev_msg.mail_buff = (unsigned char *)kmalloc(SIZE_LIMIT_PARAM, GFP_ATOMIC);
	if (NULL == recv->rev_msg.mail_buff)
	{
		kfree(recv);
		loge("recv->rev_msg.mail_buff kmalloc failed.\n");
		return -ENOMEM;
	}
	memset(recv->rev_msg.mail_buff, 0, SIZE_LIMIT_PARAM);

	recmsg = (struct misc_recmsg_param *)recv->rev_msg.mail_buff;
	recmsg->msgID = ID_AUDIO_AP_PLAY_DONE_IND;
	recmsg->playStatus = (unsigned short)wake_cmd;

	/* 设定SIZE */
	recv->rev_msg.mail_buff_len = sizeof(struct misc_recmsg_param) + SIZE_CMD_ID;

	spin_lock_bh(&s_misc_data.recv_proc_lock);
	list_add_tail(&recv->recv_node, &recv_proc_work_queue_head);
	s_misc_data.wait_flag++;
	spin_unlock_bh(&s_misc_data.recv_proc_lock);

	wake_up(&s_misc_data.proc_waitq);

	return OK;
}

/*****************************************************************************
 函 数 名  : hifi_dsp_write_param
 功能描述  : 将用户态算法参数拷贝到HIFI中
 输入参数  : unsigned long arg : ioctl的入参
 输出参数  : 无
 返 回 值  : 成功:OK 失败:BUSY
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2013年10月24日
	作	  者   : 侯良军 00215385
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_dsp_write_param(unsigned long arg)
{
    int ret = OK;
    phys_addr_t hifi_param_phy_addr = 0;
    void*       hifi_param_vir_addr = NULL;
    void*       para_addr_in        = NULL;
    void*       para_addr_out       = NULL;
    struct misc_io_sync_param para;

    IN_FUNCTION;

    if (copy_from_user(&para, (void*)arg, sizeof(struct misc_io_sync_param))) {
        loge("copy_from_user fail.\n");
        ret = ERROR;
        goto error1;
    }

    para_addr_in  = INT_TO_ADDR(para.para_in_l ,para.para_in_h);
    para_addr_out = INT_TO_ADDR(para.para_out_l,para.para_out_h);


    logi("Size of CARM_HIFI_DYN_ADDR_SHARE_STRU = %ld\n",sizeof(CARM_HIFI_DYN_ADDR_SHARE_STRU));
    hifi_param_phy_addr = (phys_addr_t)(HIFI_SYS_MEM_ADDR + sizeof(CARM_HIFI_DYN_ADDR_SHARE_STRU));
    logd("hifi_param_phy_addr = 0x%p\n", (void*)hifi_param_phy_addr);


    hifi_param_vir_addr = (unsigned char*)ioremap_wc(hifi_param_phy_addr, SIZE_PARAM_PRIV);
    if (NULL == hifi_param_vir_addr) {
        loge("hifi_param_vir_addr ioremap_wc fail.\n");
        ret = ERROR;
        goto error2;
    }
    logd("hifi_param_vir_addr = 0x%p. (*hifi_param_vir_addr) = 0x%x\n",
            hifi_param_vir_addr, (*(int *)hifi_param_vir_addr));

    logd("user addr = 0x%p, size = %d \n", para_addr_in, para.para_size_in);
    ret = copy_from_user(hifi_param_vir_addr, (void __user *)para_addr_in, para.para_size_in);

    if ( ret != 0) {
        loge("copy data to hifi error! ret = %d.\n", ret);
    }

error2:
    if (hifi_param_vir_addr != NULL) {
        iounmap(hifi_param_vir_addr);
    }

    put_user(ret, (int __user *)para_addr_out);

error1:
    OUT_FUNCTION;
    return ret;
}

/*****************************************************************************
 函 数 名  : hifi_misc_open
 功能描述  : Hifi MISC 设备打开操作
 输入参数  : struct inode *finode  :设备节点信息
			 struct file *fd	:对应设备fd
 输出参数  : 无
 返 回 值  : 成功:OK 失败:BUSY
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_misc_open(struct inode *finode, struct file *fd)
{
    logi("open device.\n");
	return OK;
}

/*****************************************************************************
 函 数 名  : hifi_misc_release
 功能描述  : Hifi MISC 设备不再使用时释放函数
 输入参数  : struct inode *finode  :设备节点信息
			 struct file *fd	:对应设备fd
 输出参数  : 无
 返 回 值  : OK
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_misc_release(struct inode *finode, struct file *fd)
{
    logi("close device.\n");
	return OK;
}

/*****************************************************************************
 函 数 名  : hifi_misc_ioctl
 功能描述  : Hifi MISC 设备提供给上层与设备交互的控制通道接口
 输入参数  : struct file *fd  :对应fd
			 unsigned int cmd	:cmd类型
			 unsigned long arg	:上层传下来的buff地址
 输出参数  : 无
 返 回 值  : 成功:OK 失败:ERROR
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static long hifi_misc_ioctl(struct file *fd,
							unsigned int cmd,
							unsigned long arg)
{
	int ret = OK;
	void __user *data32 = (void __user *)arg;

	IN_FUNCTION;

	if (!(void __user *)arg) {
		loge("Input buff is NULL.\n");
		OUT_FUNCTION;
		return (long)ERROR;
	}

	if (!hifi_is_loaded()) {
		loge("hifi isn't loaded.\n");
		return (long)ERROR;
	}

#ifdef CONFIG_ARM64
	data32 = (void __user *)compat_ptr(arg);
#endif

	/*cmd命令处理*/
	switch(cmd) {
		case HIFI_MISC_IOCTL_ASYNCMSG/*异步命令*/:
			logd("ioctl: HIFI_MISC_IOCTL_ASYNCMSG.\n");
			ret = hifi_dsp_async_cmd((unsigned long)data32);
			break;

		case HIFI_MISC_IOCTL_SYNCMSG/*同步命令*/:
			logd("ioctl: HIFI_MISC_IOCTL_SYNCMSG.\n");
			ret = down_interruptible(&s_misc_sem);
			if (ret != 0)
			{
				loge("SYNCMSG wake up by other irq err:%d.\n",ret);
				goto out;
			}
			ret = hifi_dsp_sync_cmd((unsigned long)data32);
			up(&s_misc_sem);
			break;

		case HIFI_MISC_IOCTL_SENDDATA_SYNC/*发送接收数据*/:
			logd("ioctl: HIFI_MISC_IOCTL_SENDDATA_SYNC.\n");
			ret = down_interruptible(&s_misc_sem);
			if (ret != 0)
			{
				loge("SENDDATA_SYNC wake up by other irq err:%d.\n",ret);
				goto out;
			}
			ret = hifi_dsp_senddata_sync_cmd((unsigned long)data32); /*not used by now*/
			up(&s_misc_sem);
			break;

		case HIFI_MISC_IOCTL_GET_PHYS/*获取*/:
			logd("ioctl: HIFI_MISC_IOCTL_GET_PHYS.\n");
			ret = hifi_dsp_get_phys_cmd((unsigned long)data32);
			break;

		case HIFI_MISC_IOCTL_TEST/*测试接口*/:
			logd("ioctl: HIFI_MISC_IOCTL_TEST.\n");
			ret = hifi_dsp_test_cmd((unsigned long)data32);
			break;

		case HIFI_MISC_IOCTL_WRITE_PARAMS : /* write algo param to hifi*/
			ret = hifi_dsp_write_param((unsigned long)data32);
			break;

		case HIFI_MISC_IOCTL_DUMP_HIFI:
			logi("ioctl: HIFI_MISC_IOCTL_DUMP_HIFI.\n");
			ret = hifi_dsp_dump_hifi((unsigned long)data32);
			break;
#ifdef PLATFORM_HI3XXX
		case HIFI_MISC_IOCTL_DISPLAY_MSG:
			logi("ioctl: HIFI_MISC_IOCTL_DISPLAY_MSG.\n");
			ret = hifi_get_dmesg(arg);
			break;
#endif
		case HIFI_MISC_IOCTL_WAKEUP_THREAD:
			logi("ioctl: HIFI_MISC_IOCTL_WAKEUP_THREAD.\n");
			ret = hifi_dsp_wakeup_read_thread((unsigned long)data32);
			break;
		default:
			/*打印无该CMD类型*/
			ret = (long)ERROR;
			loge("ioctl: Invalid CMD =0x%x.\n", (unsigned int)cmd);
			break;
	}
out:
	OUT_FUNCTION;
	return (long)ret;
}

static int hifi_misc_mmap(struct file *file, struct vm_area_struct *vma)
{
	int ret = OK;
	unsigned long phys_page_addr = 0;
	unsigned long size = 0;
	IN_FUNCTION;

	phys_page_addr = (u64)s_misc_data.hifi_priv_base_phy >> PAGE_SHIFT;
	size = ((unsigned long)vma->vm_end - (unsigned long)vma->vm_start);
	logd("vma=0x%p.\n", vma);
	logd("size=%ld, vma->vm_start=%ld, end=%ld.\n", ((unsigned long)vma->vm_end - (unsigned long)vma->vm_start),
		 (unsigned long)vma->vm_start, (unsigned long)vma->vm_end);
	logd("phys_page_addr=0x%ld.\n", (unsigned long)phys_page_addr);

	vma->vm_page_prot = PAGE_SHARED;
	vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

	if(size > HIFI_MUSIC_DATA_SIZE) {
		size = HIFI_MUSIC_DATA_SIZE;
	}

    ret = remap_pfn_range(vma,
                    vma->vm_start,
                    phys_page_addr,
	                size,
                    vma->vm_page_prot);
    if(ret != 0)
    {
        loge("remap_pfn_range ret=%d\n", ret);
        return ERROR;
    }

	OUT_FUNCTION;
	return ret;
}

/*****************************************************************************
 函 数 名  : hifi_misc_proc_read
 功能描述  : 用于将Hifi MISC 设备接收Hifi的消息/数据通过文件的方式递交给用户
			 态
 输入参数  : char *pg	:系统自动填充的用以填充数据的buff，为系统申请的一页
			 char**start  :指示读文件的起始位置，page的偏移量
			 off_t off	  :读文件时page的偏移，当*start存在时，
						   off会被系统忽略而认为*start就是off
			 int count	  :表示读多少字节
			 int *eof	  :读动作是否停止下发标志
			 void *data   :保留为驱动内部使用
 输出参数  : 无
 返 回 值  : 尚未读取部分的长度,如果长度大于4K则返回ERROR
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static ssize_t hifi_misc_proc_read(struct file *file, char __user *buf,
								   size_t count, loff_t *ppos)
{
	int len = 0, ret = OK;
	struct recv_request *recv = NULL;
	struct misc_recmsg_param *recmsg = NULL;

	IN_FUNCTION;

	if (!hifi_is_loaded()) {
		loge("hifi isn't loaded.\n");
		return 0;
	}

	/*等待读信号量*/
	logi("go to wait_event_interruptible.\n");
	ret = wait_event_interruptible(s_misc_data.proc_waitq, s_misc_data.wait_flag!=0);
	if (ret) {
		loge("wait event interruptible fail, 0x%x.\n", ret);
		OUT_FUNCTION;
		return -EBUSY;
	}

	logi("wait_event_interruptible success.\n");

	spin_lock_bh(&s_misc_data.recv_proc_lock);

	if (likely(s_misc_data.wait_flag > 0)) {
		s_misc_data.wait_flag--;
	}

	if (!list_empty(&recv_proc_work_queue_head)) {
		logi("queue is not null.\n");
		recv = list_entry(recv_proc_work_queue_head.next, struct recv_request, recv_node);
		if (recv) {
			len = recv->rev_msg.mail_buff_len;

			if (unlikely(len >= PAGE_MAX_SIZE)) {
				loge("buff size is invalid: %d(>= 4K or <=8).\n", len);
				ret = (int)ERROR;
			} else {
				recmsg = (struct misc_recmsg_param*)recv->rev_msg.mail_buff;
				logi("msgid: 0x%x, play status(0 - done normal, 1 - done complete, 2 -- done abnormal, 3 -- reset): %d.\n", recmsg->msgID, recmsg->playStatus);
				/*将数据写到page中*/
				len = copy_to_user(buf, recv->rev_msg.mail_buff, (recv->rev_msg.mail_buff_len - SIZE_CMD_ID));
				ret = len;
			}

			list_del(&recv->recv_node);
			kfree(recv->rev_msg.mail_buff);
			kfree(recv);
			recv = NULL;
		} else {
			loge("recv is null.\n");
		}
	} else {
		loge("queue is null.\n");
	}

	spin_unlock_bh(&s_misc_data.recv_proc_lock);

	OUT_FUNCTION;
	return ret;
}

static const struct file_operations hifi_misc_fops = {
	.owner			= THIS_MODULE,
	.open			= hifi_misc_open,
	.release		= hifi_misc_release,
	.unlocked_ioctl = hifi_misc_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = hifi_misc_ioctl,
#endif
	.mmap			= hifi_misc_mmap,
};

static struct miscdevice hifi_misc_device = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "hifi_misc",
	.fops	= &hifi_misc_fops,
};

static const struct file_operations hifi_proc_fops = {
	.owner = THIS_MODULE,
	.read = hifi_misc_proc_read,
};

static void hifi_misc_proc_init( void )
{
	struct proc_dir_entry * entry ;
	struct proc_dir_entry * hifi_misc_dir;

	hifi_misc_dir = proc_mkdir("hifidsp", NULL);
	if (hifi_misc_dir == NULL) {
		loge("Unable to create /proc/hifidsp directory.\n");
		return ;
	}

	/* Creating read/write "status" entry */
	entry = proc_create("hifi", S_IRUSR|S_IRGRP|S_IROTH, hifi_misc_dir, &hifi_proc_fops);
	if (entry == NULL) {
		remove_proc_entry("dsp", 0);
		loge("Unable to create /proc/hifidsp/hifi entry.\n");
		return ;
	}
}

#ifdef PLATFORM_HI3XXX
#ifdef CONFIG_PM
static int hifi_sr_event(struct notifier_block *this,
		unsigned long event, void *ptr) {
	switch (event) {
	case PM_POST_HIBERNATION:
	case PM_POST_SUSPEND:
		logi("resume +.\n");
		atomic_set(&s_hifi_in_suspend, 0);
		logi("resume -.\n");
		break;

	case PM_HIBERNATION_PREPARE:
	case PM_SUSPEND_PREPARE:
		logi("suspend +.\n");
		atomic_set(&s_hifi_in_suspend, 1);
		while (true) {
			if (atomic_read(&s_hifi_in_saving)) {
				msleep(100);
			} else {
				break;
			}
		}
		logi("suspend -.\n");
		break;
	default:
		return NOTIFY_DONE;
	}
	return NOTIFY_OK;
}
#endif

static int hifi_reboot_notifier(struct notifier_block *nb,
		unsigned long foo, void *bar)
{
	logi("reboot +.\n");
	atomic_set(&s_hifi_in_suspend, 1);
	while (true) {
		if (atomic_read(&s_hifi_in_saving)) {
			msleep(100);
		} else {
			break;
		}
	}
	logi("reboot -.\n");

	return 0;
}

void hifi_get_log_signal(void)
{
	while (true) {
		if (atomic_read(&s_hifi_in_suspend)) {
			msleep(100);
		} else {
			atomic_set(&s_hifi_in_saving, 1);
			break;
		}
	}
	return;
}

void hifi_release_log_signal(void)
{
	atomic_set(&s_hifi_in_saving, 0);
}

int hifi_send_msg(unsigned int mailcode, void *data, unsigned int length)
{
	if (hifi_is_loaded()) {
		return (unsigned int)mailbox_send_msg(mailcode, data, length);
	}
	return -1;
}
EXPORT_SYMBOL(hifi_send_msg);
#endif

/*****************************************************************************
 函 数 名  : hifi_misc_probe
 功能描述  : Hifi MISC设备探测注册函数
 输入参数  : struct platform_device *pdev
 输出参数  : 无
 返 回 值  : 成功:OK 失败:ERROR
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_misc_probe (struct platform_device *pdev)
{
	int ret = OK;

#ifdef PLATFORM_HI3XXX
	struct temp_hifi_cmd tmp_msg;
	struct misc_io_async_param tmp_async_msg;
#endif

	IN_FUNCTION;

	memset(&s_misc_data, 0, sizeof(struct hifi_misc_priv));

	s_misc_data.dev = &pdev->dev;

#ifdef PLATFORM_HI3XXX
#ifdef CONFIG_PM
	/* Register to get PM events */
	s_hifi_sr_nb.notifier_call = hifi_sr_event;
	s_hifi_sr_nb.priority = -1;
	(void)register_pm_notifier(&s_hifi_sr_nb);
#endif

	s_hifi_reboot_nb.notifier_call = hifi_reboot_notifier;
	s_hifi_reboot_nb.priority = -1;
	(void)register_reboot_notifier(&s_hifi_reboot_nb);
#endif

#ifdef PLATFORM_HI3XXX
	s_misc_data.hifi_priv_base_virt = (unsigned char*)ioremap_wc(HIFI_BASE_ADDR, (HIFI_SIZE - HIFI_IMAGE_SIZE));
	if (NULL == s_misc_data.hifi_priv_base_virt) {
		printk("hifi ioremap_wc error.\n");//can't use logx
		ret = ERROR;
		goto ERR;
	}
	s_misc_data.hifi_priv_base_phy = (unsigned char*)HIFI_MUSIC_DATA_LOCATION;
#endif

#ifdef PLATFORM_HI6XXX
	s_misc_data.hifi_priv_base_phy = (unsigned char *)HIFI_PRIV_ADDR;
#endif

	hifi_om_init(pdev, s_misc_data.hifi_priv_base_virt, s_misc_data.hifi_priv_base_phy);

	printk("hifi pdev name[%s].\n", pdev->name);//can't use logx

	ret = misc_register(&hifi_misc_device);
	if (OK != ret) {
		loge("hifi misc device register fail,ERROR is %d.\n", ret);
		ret = ERROR;
		goto ERR;
	}

	hifi_misc_proc_init();

	/*初始化接受信号量*/
	spin_lock_init(&s_misc_data.recv_sync_lock);
	spin_lock_init(&s_misc_data.recv_proc_lock);

	/*初始化同步信号量*/
	init_completion(&s_misc_data.completion);

	/*初始化读文件信号量*/
	init_waitqueue_head(&s_misc_data.proc_waitq);
	s_misc_data.wait_flag = 0;
	s_misc_data.sn = 0;

	wake_lock_init(&s_misc_data.hifi_misc_wakelock,WAKE_LOCK_SUSPEND, "hifi_wakelock");

#ifdef PLATFORM_HI3XXX
	ret = DRV_IPCIntInit();
	if (OK != ret) {
		loge("hifi ipc init fail.\n");
		goto ERR;
	}
	ret = (int)mailbox_init();
	if (OK != ret) {
		loge("hifi mailbox init fail.\n");
		goto ERR;
	}
#endif
	/*注册双核通信处理函数*/
#ifdef PLATFORM_HI3XXX
	ret = mailbox_reg_msg_cb(MAILBOX_MAILCODE_HIFI_TO_ACPU_MISC, (mb_msg_cb)hifi_misc_handle_mail, NULL);
#endif
#ifdef PLATFORM_HI6XXX
	ret = DRV_MAILBOX_REGISTERRECVFUNC( MAILBOX_MAILCODE_HIFI_TO_ACPU_MISC, ( mb_msg_cb)hifi_misc_handle_mail, NULL );
#endif
	if (OK != ret) {
		loge("hifi mailbox handle func register fail.\n");
		goto ERR;
	}

#ifdef PLATFORM_HI3XXX
	tmp_msg.msgID = ID_AP_AUDIO_PLAY_WAKEUPTHREAD_REQ;
	tmp_msg.value = 1;
	tmp_async_msg.para_in_l = GET_LOW32((void*)&tmp_msg);
	tmp_async_msg.para_in_h = GET_HIG32((void*)&tmp_msg);
	tmp_async_msg.para_size_in = sizeof(struct temp_hifi_cmd);

	/*系统初始化时，需要先启动一次hifi，确保modem与hifi通讯正常*/
	if (hifi_is_loaded()) {
		ret = (unsigned int)mailbox_send_msg(MAILBOX_MAILCODE_ACPU_TO_HIFI_MISC, &tmp_async_msg, tmp_async_msg.para_size_in);
		if (OK != ret) {
			loge("msg send to hifi fail, ret is %d.\n", ret);
			goto ERR;
		}
	}
#endif

	OUT_FUNCTION;
	return OK;

ERR:
	hifi_om_deinit(pdev);

	if (NULL != s_misc_data.hifi_priv_base_virt) {
		iounmap(s_misc_data.hifi_priv_base_virt);
		s_misc_data.hifi_priv_base_virt = NULL;
	}

#ifdef PLATFORM_HI3XXX
#ifdef CONFIG_PM
	unregister_pm_notifier(&s_hifi_sr_nb);
#endif
	unregister_reboot_notifier(&s_hifi_reboot_nb);
#endif

	(void)misc_deregister(&hifi_misc_device);

	OUT_FUNCTION;
	return ERROR;
}

/*****************************************************************************
 函 数 名  : hifi_misc_remove
 功能描述  : Hifi MISC 设备移除
 输入参数  : struct platform_device *pdev
 输出参数  : 无
 返 回 值  : OK
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static int hifi_misc_remove(struct platform_device *pdev)
{
	IN_FUNCTION;

	hifi_om_deinit(pdev);

	if (NULL != s_misc_data.hifi_priv_base_virt) {
		iounmap(s_misc_data.hifi_priv_base_virt);
		s_misc_data.hifi_priv_base_virt = NULL;
	}

#ifdef PLATFORM_HI3XXX
#ifdef CONFIG_PM
	/* Unregister for PM events */
	unregister_pm_notifier(&s_hifi_sr_nb);
#endif
	unregister_reboot_notifier(&s_hifi_reboot_nb);
#endif

	/* misc deregister*/
	(void)misc_deregister(&hifi_misc_device);

	OUT_FUNCTION;
	return OK;
}

static const struct of_device_id hifi_match_table[] = {
	{
		.compatible = DTS_COMP_HIFIDSP_NAME,
		.data = NULL,
	},
	{}
};
MODULE_DEVICE_TABLE(of, hifi_match_table);

static struct platform_driver hifi_misc_driver = {
	.driver =
	{
		.name  = "hifi_dsp_misc",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hifi_match_table),
	},
	.probe	= hifi_misc_probe,
	.remove = hifi_misc_remove,
};

#ifdef PLATFORM_HI3XXX
module_platform_driver(hifi_misc_driver);
#endif

#ifdef PLATFORM_HI6XXX
/*****************************************************************************
 函 数 名  : hifi_misc_init
 功能描述  : Hifi MISC driver注册到platform driver上
 输入参数  : void
 输出参数  : 无
 返 回 值  : 成功:OK 失败:ERROR
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static int __init hifi_misc_init( void )
{
	int ret = 0;

	IN_FUNCTION;
	printk("Audio:hifi_misc_init \n");

	ret = platform_driver_register(&hifi_misc_driver);
	if(OK != ret)
	{
		loge("hifi driver register fail,ERROR is %d\n", ret);
		return ERROR;
	}

	OUT_FUNCTION;
	return OK;
}

/*****************************************************************************
 函 数 名  : hifi_misc_exit
 功能描述  : Hifi MISC driver从platform driver上去注册
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2012年8月1日
	作	  者   : 夏青 00195127
	修改内容   : 新生成函数

*****************************************************************************/
static void __exit hifi_misc_exit( void )
{
	IN_FUNCTION;

	platform_driver_unregister(&hifi_misc_driver);

	OUT_FUNCTION;
}

module_init(hifi_misc_init);
module_exit(hifi_misc_exit);
#endif

MODULE_DESCRIPTION("hifi driver");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

