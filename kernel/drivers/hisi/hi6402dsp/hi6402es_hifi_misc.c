/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/delay.h>

#include <linux/firmware.h>

#include <hi6402_hifi_interface.h>
#include <linux/mfd/hi6402es_irq.h>
#include "hi6402es_hifi_regs.h"
#include "hi6402es_hifi_debug.h"

#include "hi6402es_hifi_misc.h"

#define DSP_IF_NUM 8
#define MAX_MSG_SIZE 128
#define MAX_PARA_SIZE 4096
#define RESULT_SIZE 4
#define MAX_OUT_PARA_SIZE ((RESULT_SIZE) + (MAX_PARA_SIZE))

#define HI6402ES_FREQ_12P288M 0
#define HI6402ES_FREQ_32P576K 1

/*XXX: Some bugs will be fixed in CS version.*/
#define HI6402ES_VERSION_ES

/*XXX: change to 4 to enbale debug print*/
unsigned long hi6402es_dsp_debug_level = 3;

static const unsigned int DSP_MSG_START_ADDR = HI6402ES_OCRAM1_BASE;
static const unsigned int DSP_MLIB_PARA_ADDR =
				HI6402ES_OCRAM1_BASE + 2 * MAX_MSG_SIZE;

struct reg_rw_struct {
	unsigned int	reg;
	unsigned int	val;
};

struct hi6402es_dsp_priv {
	struct hi6402es_irq *p_irq;
	struct device *dev;

	unsigned int irq_cmd_valid;
	struct mutex msg_mutex;

	unsigned int msg_send_done;
	wait_queue_head_t send_msg_wq;

	unsigned int sync_msg_ret;
	wait_queue_head_t sync_msg_wq;

	unsigned int power_ref;
	struct mutex power_ref_mutex;

	unsigned int runstall_ref;
	struct mutex runstall_mutex;

	unsigned int clk_ref;
	struct mutex clk_mutex;

	unsigned int wakeup_state;
	unsigned int datahook_state;
	unsigned int smartpa_state;
	unsigned int madtest_state;
};

static struct hi6402es_dsp_priv priv;


/* ************************************************************************************
 * use reg access supplied in hi6402es_irq
 * */
static inline void hi6402es_hifi_write_reg(unsigned int reg, unsigned int val)
{
	BUG_ON(priv.p_irq == NULL);
	hi6402es_irq_write(priv.p_irq, reg, val);
}

static inline unsigned int hi6402es_hifi_read_reg(unsigned int reg)
{
	BUG_ON(priv.p_irq == NULL);
	return hi6402es_irq_read(priv.p_irq, reg);
}

static inline void hi6402es_hifi_reg_set_bit(unsigned int reg,
					   unsigned int offset)
{
	BUG_ON(priv.p_irq == NULL);
	hi6402es_reg_set_bit(priv.p_irq, reg, offset);
}

static inline void hi6402es_hifi_reg_clr_bit(unsigned int reg,
					   unsigned int offset)
{
	BUG_ON(priv.p_irq == NULL);
	hi6402es_reg_clr_bit(priv.p_irq, reg, offset);
}

static inline void hi6402es_hifi_reg_write_bits(unsigned int reg,
					      unsigned int value,
					      unsigned int mask)
{
	BUG_ON(priv.p_irq == NULL);
	hi6402es_reg_write_bits(priv.p_irq, reg, value, mask);
}

static void hi6402es_notify_dsp_to_receive(void)
{
	hi6402es_hifi_write_reg(HI6402ES_GPIO0_DIR, 0x20);
	hi6402es_hifi_write_reg(HI6402ES_GPIO0_IS , 0x0);
	hi6402es_hifi_write_reg(HI6402ES_GPIO0_IBE, 0x0);
	hi6402es_hifi_write_reg(HI6402ES_GPIO0_IEV, 0x20);
	hi6402es_hifi_write_reg(HI6402ES_GPIO0_IE2, 0x20);
	hi6402es_hifi_write_reg(HI6402ES_GPIO0_DATA_ch5, 0x0);
	hi6402es_hifi_write_reg(HI6402ES_GPIO0_DATA_ch5, 0x20);
}

static irqreturn_t hi6402es_msg_irq_handler(int irq, void *data)
{
	IN_FUNCTION;

	if (!hi6402es_hifi_read_reg(HI6402ES_DSP_CMD_STATUS_VLD)) {
		HI6402ES_DSP_ERROR("CMD invalid\n");
		return IRQ_HANDLED;
	}

	if (hi6402es_hifi_read_reg(HI6402ES_DSP_CMD_STATUS) & (0x1 << 0)) {
		HI6402ES_DSP_INFO("bit 0\n");
		hi6402es_hifi_reg_clr_bit(HIFI_CMD_STAT, 0);
		priv.msg_send_done = 1;
		wake_up_interruptible_all(&priv.send_msg_wq);
	}

	if (hi6402es_hifi_read_reg(HI6402ES_DSP_CMD_STATUS) & (0x1 << 1)) {
		HI6402ES_DSP_INFO("bit 1\n");
		hi6402es_hifi_reg_clr_bit(HIFI_CMD_STAT, 1);
		priv.sync_msg_ret = 1;
		wake_up_interruptible_all(&priv.sync_msg_wq);
	}

	//clr irq
#ifdef HI6402ES_VERSION_ES
	if ((hi6402es_hifi_read_reg(HI6402ES_DSP_CMD_STAT_VLD) & 0x1) == 0) {
		hi6402es_hifi_reg_set_bit(HI6402ES_DSP_CMD_STAT_VLD, 0);
	}
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_CMD_STAT_VLD, 0);
#endif
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_CMD_STAT_VLD_IRQ, 0);

	OUT_FUNCTION;
	return IRQ_HANDLED;
}

static void hi6402es_write(const unsigned int start_addr,
			 const unsigned char *arg,
			 const unsigned int len)
{
	int i = 0;
	unsigned int *to_send = NULL;

	BUG_ON(arg == NULL);
	BUG_ON(len == 0);

	to_send = (unsigned int *)arg;
	for (i=0; i<len; i+=4) {
		hi6402es_hifi_write_reg(start_addr + i, *to_send++);
	}
}

static void hi6402es_read(const unsigned int start_addr,
			unsigned char *arg,
			const unsigned int len)
{
	int i = 0;
	unsigned int *to_receive = NULL;

	BUG_ON(arg == NULL);
	BUG_ON(len == 0);

	to_receive = (unsigned int *)arg;
	for (i=0; i<len; i+=4) {
		*to_receive++ = hi6402es_hifi_read_reg(start_addr + i);
	}
}

static int hi6402es_write_mlib_para(const unsigned char *arg,
				  const unsigned int len)
{
	if (len > MAX_PARA_SIZE) {
		HI6402ES_DSP_ERROR("msg length:%u exceed limit!\n", len);
		return ERROR;
	}

	if ((len & 0x3) != 0) {
		HI6402ES_DSP_ERROR("msg length:%u is not 4 byte aligned\n", len);
		return ERROR;
	}

	hi6402es_write(DSP_MLIB_PARA_ADDR, arg, len);

	return OK;
}

static int hi6402es_read_mlib_para(unsigned char *arg, const unsigned int len)
{
	BUG_ON(arg == NULL);
	BUG_ON(len == 0);

	if (len > MAX_PARA_SIZE) {
		HI6402ES_DSP_ERROR("msg length:%u exceed limit!\n", len);
		return ERROR;
	}

	if ((len & 0x3) != 0) {
		HI6402ES_DSP_ERROR("msg length:%u is not 4 byte aligned\n", len);
		return ERROR;
	}

	hi6402es_read(DSP_MLIB_PARA_ADDR, arg, len);

	return OK;
}

/*XXX for now our msg is 4byte aligned*/
static int hi6402es_write_msg(const unsigned char *arg, const unsigned int len)
{
	int ret = OK;

	IN_FUNCTION;

	if (len > MAX_MSG_SIZE) {
		HI6402ES_DSP_ERROR("msg length exceed limit!\n");
		return ERROR;
	}

	if ((len & 0x3) != 0) {
		HI6402ES_DSP_ERROR("msg length:%u is not 4 byte aligned\n", len);
		return ERROR;
	}

	hi6402es_write(DSP_MSG_START_ADDR, arg, len);

	priv.msg_send_done = 0;
	hi6402es_notify_dsp_to_receive();

	/*XXX: wait 200ms(600ms in total) for cmd write. could change in the future */
	if (wait_event_interruptible_timeout(priv.send_msg_wq,
                       (priv.msg_send_done == 1), HZ/5) == 0) {
		HI6402ES_DSP_ERROR("wait for send msg done timeout, ret=%d\n", ret);
		ret = BUSY;
	}

	OUT_FUNCTION;

	return ret;
}

static int hi6402es_get_input_param(unsigned int usr_para_size,
				  void *usr_para_addr,
				  unsigned int *krn_para_size,
				  void **krn_para_addr)
{
	void *para_in = NULL;
	unsigned int para_size_in = 0;

	IN_FUNCTION;

	para_size_in = roundup(usr_para_size, 4);
	para_in = kzalloc(para_size_in, GFP_KERNEL);
	if (para_in == NULL) {
		HI6402ES_DSP_ERROR("kzalloc fail\n");
		goto ERR;
	}

	if (usr_para_addr != NULL) {
		if (copy_from_user(para_in , usr_para_addr, usr_para_size)) {
			HI6402ES_DSP_ERROR("copy_from_user fail\n");
			goto ERR;
		}
	} else {
		HI6402ES_DSP_ERROR("usr_para_addr is null no user data\n");
		goto ERR;
	}

	*krn_para_size = para_size_in;
	*krn_para_addr = para_in;

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

static void hi6402es_param_free(void **krn_para_addr)
{
	IN_FUNCTION;

	if (*krn_para_addr != NULL) {
		kfree(*krn_para_addr);
		*krn_para_addr = NULL;
	} else {
		HI6402ES_DSP_ERROR("krn_para_addr to free is NULL\n");
	}

	OUT_FUNCTION;

	return;
}

static int hi6402es_async_write(const unsigned char *arg, const unsigned int len)
{
	int ret = OK;
	unsigned int retry_times = 0;

	IN_FUNCTION;

	BUG_ON(arg == NULL);
	BUG_ON(len == 0);

	/*TODO: hi6402es_write need to check whether data had been changed via ssi.
		1. use Check digit to verify data
		2. check regs to confirm */
	do {
		ret = hi6402es_write_msg(arg, len);
		retry_times++;
		if (retry_times > 1) {
			HI6402ES_DSP_INFO("msg send timeout, retry %u\n", retry_times);
		}
	} while (ret == BUSY && retry_times < MSG_SEND_RETRIES);

	if (ret != OK) {
		HI6402ES_DSP_ERROR("msg send to hifi fail,ret is %d\n", ret);
		ret = ERROR;
	}

	OUT_FUNCTION;

	return ret;
}

static int hi6402es_alloc_output_param_buffer(unsigned int usr_para_size,
					    void *usr_para_addr,
					    unsigned int *krn_para_size,
					    void **krn_para_addr)
{
	BUG_ON(*krn_para_addr != NULL);

	HI6402ES_DSP_INFO("malloc size: %u\n", usr_para_size);
	if (usr_para_size == 0 || usr_para_size > MAX_OUT_PARA_SIZE) {
		HI6402ES_DSP_ERROR("usr space size invalid\n");
		return ERROR;
	}

	if (usr_para_addr == NULL) {
		HI6402ES_DSP_ERROR("usr_para_addr is NULL\n");
		return ERROR;
	}

	*krn_para_addr = kzalloc(usr_para_size, GFP_KERNEL);
	if (*krn_para_addr == NULL) {
		HI6402ES_DSP_ERROR("kzalloc fail\n");
		return ERROR;
	}

	*krn_para_size = usr_para_size;

	return OK;
}

static int hi6402es_put_output_param(unsigned int usr_para_size,
				   void *usr_para_addr,
				   unsigned int krn_para_size,
				   void *krn_para_addr)
{
	int ret = OK;

	BUG_ON(usr_para_size == 0);
	BUG_ON(usr_para_addr == NULL);
	BUG_ON(krn_para_size == 0);
	BUG_ON(krn_para_addr == NULL);

	IN_FUNCTION;

	if (krn_para_size != usr_para_size) {
		HI6402ES_DSP_ERROR("krn para size:%d != usr para size%d\n",
				 krn_para_size, usr_para_size);
		return ERROR;
	}

	HI6402ES_DSP_INFO("user_para_size:%d\n", usr_para_size);
	ret = copy_to_user(usr_para_addr, krn_para_addr, usr_para_size);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("copy_to_user fail, ret is %d\n", ret);
		return ERROR;
	}

	OUT_FUNCTION;

	return OK;
}

static int hi6402es_sync_write(const unsigned char *arg, const unsigned int len)
{
	int ret = OK;

	IN_FUNCTION;

	priv.sync_msg_ret = 0;

	ret = hi6402es_async_write(arg, len);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("send msg failed\n");
		goto end;
	}

	if (wait_event_interruptible_timeout(priv.sync_msg_wq,
                       (priv.sync_msg_ret == 1), HZ) == 0) {
		HI6402ES_DSP_ERROR("wait for sync cmd done timeout, ret=%d\n", ret);
		ret = BUSY;
	}

end:
	OUT_FUNCTION;

	return ret;
}

/* dsp_if bypass config bit 6,7 */
static unsigned int hi6402es_sc_src_lr_ctrls_m[] = {
	HI6402ES_SC_S1_SRC_LR_CTRL_M,
	HI6402ES_SC_S2_SRC_LR_CTRL_M,
	HI6402ES_SC_S3_SRC_LR_CTRL_M,
	HI6402ES_SC_S4_SRC_LR_CTRL_M,
};

/* dsp_if io sample rate config */
static unsigned int hi6402es_sc_fs_ctrls_h[] = {
	HI6402ES_SC_FS_S1_CTRL_H,
	HI6402ES_SC_FS_S2_CTRL_H,
	HI6402ES_SC_FS_S3_CTRL_H,
	HI6402ES_SC_FS_S4_CTRL_H,
};

/* check for parameters used by misc only */
static int hi6402es_dsp_if_para_check(const char *arg)
{
	unsigned int i = 0;

	PCM_IF_MSG_STRU *pcm_if_msg = NULL;
	DSP_IF_OPEN_REQ_STRU *dsp_if_open_req = (DSP_IF_OPEN_REQ_STRU *)arg;
	PCM_PROCESS_DMA_MSG_STRU *dma_msg_stru = &dsp_if_open_req->stProcessDMA;

	for (i=0; i<dma_msg_stru->uwIFCount; i++) {
		pcm_if_msg = &dma_msg_stru->stIFCfgList[i];

		if (pcm_if_msg->uwIFId >= DSP_IF_NUM) {
			HI6402ES_DSP_ERROR("dsp if ID %d is out of range\n",
					pcm_if_msg->uwIFId);
			return ERROR;
		}

		switch (pcm_if_msg->uwSampleRateIn) {
		case 0:
			HI6402ES_DSP_INFO("DATA_HOOK_PROCESS, sample_rate=0\n");
			break;
		case 8000:
		case 16000:
		case 32000:
		case 48000:
		case 96000:
		case 192000:
			break;
		default:
			HI6402ES_DSP_ERROR("unsupport sample_rate %d \n",
					pcm_if_msg->uwSampleRateIn);
			return ERROR;
		}
	}

	return OK;
}

static int hi6402es_dsp_if_set_sample_rate(unsigned int dsp_if_id,
					    unsigned int sample_rate)
{
	unsigned int addr = 0;
	unsigned char mask = 0;
	unsigned char sample_rate_index = 0;

	unsigned int i2s_id = dsp_if_id / 2;
	unsigned int direct =
		(dsp_if_id & 0x1) ? HI6402ES_HIFI_PCM_OUT : HI6402ES_HIFI_PCM_IN;

	IN_FUNCTION;

	BUG_ON(i2s_id >= ARRAY_SIZE(hi6402es_sc_fs_ctrls_h));

	switch (sample_rate) {

	case 0:
		HI6402ES_DSP_INFO("DATA_HOOK_PROCESS, sample_rate=0\n");
		break;
	case 8000:
		sample_rate_index = HI6402ES_HIFI_PCM_SAMPLE_RATE_8K;
		break;
	case 16000:
		sample_rate_index = HI6402ES_HIFI_PCM_SAMPLE_RATE_16K;
		break;
	case 32000:
		sample_rate_index = HI6402ES_HIFI_PCM_SAMPLE_RATE_32K;
		break;
	case 48000:
		sample_rate_index = HI6402ES_HIFI_PCM_SAMPLE_RATE_48K;
		break;
	case 96000:
		sample_rate_index = HI6402ES_HIFI_PCM_SAMPLE_RATE_96K;
		break;
	case 192000:
		sample_rate_index = HI6402ES_HIFI_PCM_SAMPLE_RATE_192K;
		break;
	default:
		/* shouldn't be here */
		HI6402ES_DSP_ERROR("unsupport sample_rate %d!! \n", sample_rate);
	}

	addr = hi6402es_sc_fs_ctrls_h[i2s_id];
	mask = (direct == HI6402ES_HIFI_PCM_IN) ? 0xf : 0xf0;
	sample_rate_index = (direct == HI6402ES_HIFI_PCM_IN)
		                ? sample_rate_index : sample_rate_index << 4;

	hi6402es_hifi_reg_write_bits(addr, sample_rate_index, mask);

	OUT_FUNCTION;

	return 0;
}

static void hi6402es_dsp_if_set_bypass(unsigned int dsp_if_id, bool enable)
{
	unsigned int addr = 0;
	unsigned int bit = 0;

	unsigned int i2s_id = dsp_if_id / 2;
	unsigned int direct =
		(dsp_if_id & 0x1) ? HI6402ES_HIFI_PCM_OUT : HI6402ES_HIFI_PCM_IN;

	IN_FUNCTION;

	BUG_ON(i2s_id >= ARRAY_SIZE(hi6402es_sc_src_lr_ctrls_m));

	bit = (direct == HI6402ES_HIFI_PCM_IN) ? 6 : 7;
	addr = hi6402es_sc_src_lr_ctrls_m[i2s_id];

	if (enable) {
		hi6402es_hifi_reg_set_bit(addr, bit);
	} else {
		hi6402es_hifi_reg_clr_bit(addr, bit);
	}

	OUT_FUNCTION;
}

/* now we'v alread check the para, so don't do it again */
static void hi6402es_dsp_if_config(const char *arg, bool enable)
{
	unsigned int i = 0;
	unsigned int clock_change_ifs = 0;

	PCM_IF_MSG_STRU *pcm_if_msg = NULL;
	DSP_IF_OPEN_REQ_STRU *dsp_if_open_req = (DSP_IF_OPEN_REQ_STRU *)arg;
	PCM_PROCESS_DMA_MSG_STRU *dma_msg_stru = &dsp_if_open_req->stProcessDMA;

	IN_FUNCTION;

	for (i=0; i<dma_msg_stru->uwIFCount; i++) {
		pcm_if_msg = &dma_msg_stru->stIFCfgList[i];

		if (enable) {
			hi6402es_dsp_if_set_sample_rate(pcm_if_msg->uwIFId,
						pcm_if_msg->uwSampleRateIn);
		}
		clock_change_ifs |= 1 << (pcm_if_msg->uwIFId);
	}

	HI6402ES_DSP_INFO("clock_change_ifs: 0x%x\n", clock_change_ifs);
	hi6402es_hifi_reg_write_bits(HI6402ES_DSP_I2S_DSPIF_CLK_EN,
				   enable?(~0):0,
				   clock_change_ifs/* mask */);

	for (i=0; i<DSP_IF_NUM; i++) {
		if(clock_change_ifs & (1<<i)) {
			hi6402es_dsp_if_set_bypass(i, !enable);
		}
	}

	OUT_FUNCTION;
}

/* to do: should use define uart type */
int hi6402es_uart_config[][4] = {{0xBA,0xE8,0x22,0x14},/* hi6402es req :12.288M */
						{0xAB,0xAA,0x2A,0x4}};/* hi6402es req :32K */
static int hi6402es_func_uart_div_switch(int hi6402es_req)
{
	int i = 0;
	u32 ret = 0;
	/* set uart clk div(CFG nManager) */
	for(i = 0; i < 3; i++) {
		hi6402es_hifi_write_reg(HI6402ES_DSP_UART_DIV_0 + i,hi6402es_uart_config[hi6402es_req][i]);
		ret = hi6402es_hifi_read_reg(HI6402ES_DSP_UART_DIV_0 + i);
		if(ret != hi6402es_uart_config[hi6402es_req][i]) {
			HI6402ES_DSP_ERROR("set uart div fail\n");
			return -1;
		}
	}

	/* set dsp uart DHL(DSP nManager) */
	hi6402es_hifi_reg_set_bit(0x2000400c,7);
	hi6402es_hifi_write_reg(0x20004000,hi6402es_uart_config[hi6402es_req][3]);
	hi6402es_hifi_reg_clr_bit(0x2000400c,7);

	return 0;
}

/*
 * cmd_process_functions
 * */

static void start_wakeup(void);
static void stop_wakeup(void);
static void start_hifi(void);
static void stop_hifi(void);

static int hi6402es_func_if_open(struct krn_param_io_buf *param)
{
	int ret = 0;
	DSP_IF_OPEN_REQ_STRU *dsp_if_open_req =
		(DSP_IF_OPEN_REQ_STRU *)(param->buf_in);
	PCM_PROCESS_DMA_MSG_STRU *dma_msg_stru = &dsp_if_open_req->stProcessDMA;

	IN_FUNCTION;

	ret = hi6402es_dsp_if_para_check(param->buf_in);
	if(ret != OK) {
		HI6402ES_DSP_ERROR("dsp if parameter invalid\n");
		goto end;
	}

	/* DATA HOOK should first open dspif then start dma */
	if (dma_msg_stru->uwProcessId == MLIB_PATH_DATA_HOOK) {
		if(1 == priv.datahook_state) {
			HI6402ES_DSP_ERROR("datahook has been opened\n");
			goto end;
		}
		priv.datahook_state = 1;
		start_hifi();
		hi6402es_dsp_if_config(param->buf_in, true);
		ret = hi6402es_sync_write(param->buf_in, param->buf_size_in);
		if (ret != OK) {
			HI6402ES_DSP_ERROR("async_write ret=%d\n", ret);
			/* disable dsp if clk */
			hi6402es_dsp_if_config(param->buf_in, false);
			goto end;
		}
	} else {
		if (dma_msg_stru->uwProcessId == MLIB_PATH_WAKEUP) {
			if(1 == priv.wakeup_state) {
				HI6402ES_DSP_ERROR("wakeup has been opened\n");
				goto end;
			}
			start_wakeup();
			ret = hi6402es_func_uart_div_switch(HI6402ES_FREQ_32P576K);
			if (OK != ret)
				goto end;
		} else if(dma_msg_stru->uwProcessId == MLIB_PATH_SMARTPA) {
			if(1 == priv.smartpa_state) {
				HI6402ES_DSP_ERROR("smartpa has been opened\n");
				goto end;
			}
			priv.smartpa_state = 1;
			start_hifi();
		}

		ret = hi6402es_sync_write(param->buf_in, param->buf_size_in);
		if (ret != OK) {
			HI6402ES_DSP_ERROR("async_write ret=%d\n", ret);
			goto end;
		}

		hi6402es_dsp_if_config(param->buf_in, true);
	}

end:
	OUT_FUNCTION;
	return ret;
}

static int hi6402es_func_if_close(struct krn_param_io_buf *param)
{
	int ret = 0;
	DSP_IF_OPEN_REQ_STRU *dsp_if_open_req =
						(DSP_IF_OPEN_REQ_STRU *)(param->buf_in);
	PCM_PROCESS_DMA_MSG_STRU *dma_msg_stru = &dsp_if_open_req->stProcessDMA;

	IN_FUNCTION;

	ret = hi6402es_dsp_if_para_check(param->buf_in);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("dsp if parameter invalid\n");
		goto end;
	}

	ret = hi6402es_async_write(param->buf_in, param->buf_size_in);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("async_write ret=%d\n", ret);
		goto end;
	}

	/* FIXME:should use sync write */
	msleep(100);

	hi6402es_dsp_if_config(param->buf_in, false);

	msleep(100);

	if(dma_msg_stru->uwProcessId == MLIB_PATH_WAKEUP) {
		ret = hi6402es_func_uart_div_switch(HI6402ES_FREQ_12P288M);

		if (OK != ret)
			goto end;

		if(1 == priv.wakeup_state){
			stop_wakeup();
		}
	}

	if(dma_msg_stru->uwProcessId == MLIB_PATH_DATA_HOOK && 1 == priv.datahook_state) {
		stop_hifi();
		priv.datahook_state = 0;
	}

	if(dma_msg_stru->uwProcessId == MLIB_PATH_SMARTPA && 1 == priv.smartpa_state) {
		stop_hifi();
		priv.smartpa_state = 0;
	}
end:
	OUT_FUNCTION;
	return ret;
}

static int hi6402es_func_para_set(struct krn_param_io_buf *param)
{
	int ret = OK;
	MLIB_PARA_SET_REQ_STRU *mlib_para = NULL;

	IN_FUNCTION;

	BUG_ON(param == NULL);
	BUG_ON(param->buf_in == NULL);
	BUG_ON(param->buf_size_in == 0);

	mlib_para = (MLIB_PARA_SET_REQ_STRU *)param->buf_in;

	ret = hi6402es_write_mlib_para(mlib_para->aucData, mlib_para->uwSize);

	if(ret != OK) {
		HI6402ES_DSP_ERROR("write mlib para failed\n");
		goto end;
	}

	ret = hi6402es_sync_write(param->buf_in, sizeof(MLIB_PARA_SET_REQ_STRU));
	if (ret != OK) {
		HI6402ES_DSP_ERROR("async write failed\n");
		goto end;
	}

end:
	OUT_FUNCTION;
	return ret;
}

static int hi6402es_func_para_get(struct krn_param_io_buf *param)
{
	int ret = OK;

	IN_FUNCTION;

	BUG_ON(param == NULL);
	BUG_ON(param->buf_in == NULL);
	BUG_ON(param->buf_size_in == 0);
	BUG_ON(param->buf_out == NULL);
	BUG_ON(param->buf_size_out == 0);

	/* send request */
	ret = hi6402es_sync_write(param->buf_in, param->buf_size_in);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("sync write failed\n");
		goto end;
	}

	ret = hi6402es_read_mlib_para(param->buf_out + RESULT_SIZE,
				param->buf_size_out - RESULT_SIZE);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("read para failed\n");
		goto end;
	}

end:
	OUT_FUNCTION;
	return ret;
}

static int hi6402es_func_om(struct krn_param_io_buf *param)
{
	int ret = OK;

	IN_FUNCTION;

	BUG_ON(param == NULL);
	BUG_ON(param->buf_in == NULL);
	BUG_ON(param->buf_size_in == 0);

	ret = hi6402es_async_write(param->buf_in, param->buf_size_in);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("async write failed\n");
		goto end;
	}

end:
	OUT_FUNCTION;
	return ret;
}

/* compatible with CS */
static int hi6402es_func_fw_download(struct krn_param_io_buf *param)
{
	IN_FUNCTION;
	OUT_FUNCTION;
	return OK;
}

static int hi6402es_func_mad_test_start(struct krn_param_io_buf *param)
{
	int ret = OK;
	IN_FUNCTION;

	BUG_ON(param == NULL);
	BUG_ON(param->buf_in == NULL);
	BUG_ON(param->buf_size_in == 0);

	if(1 == priv.madtest_state){
			HI6402ES_DSP_ERROR("mad test has been opend!\n");
			return ERROR;
	}
	start_hifi();
	ret = hi6402es_async_write(param->buf_in, param->buf_size_in);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("async write failed\n");
		goto end;
	}
	priv.madtest_state = 1;
end:
	OUT_FUNCTION;
	if(ret != OK){
			stop_hifi();
	}
	return ret;
}

static int hi6402es_func_mad_test_stop(struct krn_param_io_buf *param)
{
	int ret = OK;
	IN_FUNCTION;

	BUG_ON(param == NULL);
	BUG_ON(param->buf_in == NULL);
	BUG_ON(param->buf_size_in == 0);

	if(0 == priv.madtest_state){
			HI6402ES_DSP_ERROR("mad test has been stoped!\n");
			return ERROR;
	}
	ret = hi6402es_async_write(param->buf_in, param->buf_size_in);
	if (ret != OK) {
			HI6402ES_DSP_ERROR("async write failed\n");
			goto end;
	}
	priv.madtest_state = 0;
end:
	OUT_FUNCTION;
	stop_hifi();
	return ret;
}

static cmd_process_func hi6402es_select_func(const unsigned char *arg,
					   const struct cmd_func_pair *func_map,
					   const unsigned int func_map_size)
{
	unsigned int i = 0;
	unsigned short msg_id;

	BUG_ON(arg == NULL);

	msg_id = *(unsigned short*)arg;

	IN_FUNCTION;
	for (i = 0; i < func_map_size; i++) {
		if (func_map[i].cmd_id == msg_id) {
			return func_map[i].func;
		}
	}

	HI6402ES_DSP_ERROR("cmd_process_func for id:%d not found!\n", msg_id);

	OUT_FUNCTION;
	return (cmd_process_func)NULL;
}

static struct cmd_func_pair async_cmd_func_map[] = {
};

static int hi6402es_hifi_async_cmd(unsigned long arg)
{
	int ret = OK;
	cmd_process_func func = NULL;
	struct misc_io_async_param param;
	struct krn_param_io_buf krn_param;

	memset(&param, 0, sizeof(param));
	memset(&krn_param, 0, sizeof(krn_param));

	IN_FUNCTION;

	//FIXME: should check arg not be NULL?
	if (copy_from_user(&param, (void*)arg,
			sizeof(struct misc_io_async_param))) {
		HI6402ES_DSP_ERROR("copy_from_user fail.\n");
		ret = ERROR;
		goto end;
	}

	ret = hi6402es_get_input_param(param.para_size_in,
                     INT_TO_ADDR(param.para_in_l, param.para_in_h),
				     &krn_param.buf_size_in,
				     (void **)&krn_param.buf_in);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("get_input_param ret=%d\n", ret);
		goto end;
	}

	func = hi6402es_select_func(krn_param.buf_in, async_cmd_func_map,
				  ARRAY_SIZE(async_cmd_func_map));
	if (func == NULL) {
		HI6402ES_DSP_ERROR("select_func error.\n");
		ret = ERROR;
		goto end;
	}

	ret = func(&krn_param);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("func process error.\n");
		goto end;
	}

end:
	hi6402es_param_free((void **)&krn_param.buf_in);

	OUT_FUNCTION;

	return ret;
}

static struct cmd_func_pair sync_cmd_func_map[] = {
	{ ID_AP_DSP_IF_OPEN,         hi6402es_func_if_open       },
	{ ID_AP_DSP_IF_CLOSE,        hi6402es_func_if_close      },
	{ ID_AP_DSP_PARAMETER_SET,   hi6402es_func_para_set      },
	{ ID_AP_DSP_PARAMETER_GET,   hi6402es_func_para_get      },
	{ ID_AP_DSP_OM,              hi6402es_func_om            },
	{ ID_AP_DSP_MADTEST_START,   hi6402es_func_mad_test_start},
	{ ID_AP_DSP_MADTEST_STOP,    hi6402es_func_mad_test_stop },
	{ ID_AP_IMGAE_DOWNLOAD,      hi6402es_func_fw_download   },
};

static int hi6402es_hifi_sync_cmd(unsigned long arg)
{
	int ret = OK;
	cmd_process_func func = NULL;
	struct misc_io_sync_param param;
	struct krn_param_io_buf krn_param;

	IN_FUNCTION;

	memset(&param, 0, sizeof(param));
	memset(&krn_param, 0, sizeof(krn_param));

	//FIXME: should check arg not be NULL?
	if (copy_from_user(&param, (void*)arg,
			   sizeof(struct misc_io_sync_param))) {
		HI6402ES_DSP_ERROR("copy_from_user fail.\n");
		ret = ERROR;
		goto end;
	}

	ret = hi6402es_alloc_output_param_buffer(param.para_size_out,
					       INT_TO_ADDR(param.para_out_l, param.para_out_h),
					      &krn_param.buf_size_out,
				     (void **)&krn_param.buf_out);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("alloc output buffer failed.\n");
		goto end;
	}

	ret = hi6402es_get_input_param(param.para_size_in,
	                 INT_TO_ADDR(param.para_in_l, param.para_in_h),
				     &krn_param.buf_size_in,
				     (void **)&krn_param.buf_in);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("get_input_param ret=%d\n", ret);
		goto end;
	}

	func = hi6402es_select_func(krn_param.buf_in, sync_cmd_func_map,
				  ARRAY_SIZE(sync_cmd_func_map));
	if (func == NULL) {
		HI6402ES_DSP_ERROR("select_func error.\n");
		ret = ERROR;
		goto end;
	}

	ret = func(&krn_param);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("func process error.\n");
		goto end;
	}

	/* write result to out buf */
	BUG_ON(krn_param.buf_out == NULL);
	if (krn_param.buf_size_out >= sizeof(int)) {
		*(int *)krn_param.buf_out = ret;
	} else {
		HI6402ES_DSP_ERROR("not enough space to save result\n");
		goto end;
	}

	/* copy result to user space */
	ret = hi6402es_put_output_param(param.para_size_out,
			          INT_TO_ADDR(param.para_out_l, param.para_out_h),
				      krn_param.buf_size_out,
			      (void *)krn_param.buf_out);
	if (ret != OK) {
		HI6402ES_DSP_ERROR("copy result to user failed\n");
		goto end;
	}

end:
	hi6402es_param_free((void **)&krn_param.buf_in);
	hi6402es_param_free((void **)&krn_param.buf_out);

	OUT_FUNCTION;

	return ret;
}

/*****************************************************************************
 * misc driver
 * */
static int hi6402es_hifi_misc_open(struct inode *finode, struct file *fd)
{
	return 0;
}

static int hi6402es_hifi_misc_release(struct inode *finode, struct file *fd)
{
	return 0;
}

static long hi6402es_hifi_misc_ioctl(struct file *fd,
                                   unsigned int cmd,
                                   unsigned long arg)
{
	int ret = 0;

	IN_FUNCTION;

	switch(cmd) {
		case HI6402ES_HIFI_MISC_IOCTL_ASYNCMSG:
			HI6402ES_DSP_DEBUG("ioctl: HIFI_MISC_IOCTL_ASYNCMSG\n");
			mutex_lock(&priv.msg_mutex);
			ret = hi6402es_hifi_async_cmd(arg);
			mutex_unlock(&priv.msg_mutex);
			break;
		case HI6402ES_HIFI_MISC_IOCTL_SYNCMSG:
			HI6402ES_DSP_DEBUG("ioctl: HIFI_MISC_IOCTL_SYNCMSG\n");
			mutex_lock(&priv.msg_mutex);
			ret = hi6402es_hifi_sync_cmd(arg);
			mutex_unlock(&priv.msg_mutex);
			break;
		default:
			HI6402ES_DSP_ERROR("ioctl: Invalid CMD =0x%x\n", cmd);
			//TODO: should return a meaningful value
			ret = -1;
			break;
	}

	OUT_FUNCTION;

	return (long)ret;
}

static ssize_t hi6402es_hifi_misc_read(struct file *file, char __user *buf,
                                     size_t nbytes, loff_t *pos)
{
	struct reg_rw_struct kern_buf;
	if (copy_from_user(&kern_buf, (void  __user *)buf, nbytes)) {
		HI6402ES_DSP_ERROR("copy_from_user fail.\n");
		return -EFAULT;
	}

	kern_buf.val = hi6402es_hifi_read_reg(kern_buf.reg);
	if (copy_to_user((void  __user *)buf, &kern_buf, nbytes)) {
		HI6402ES_DSP_ERROR("copy_to_user fail.\n");
		return -EFAULT;
	}

	return 0;
}

static ssize_t hi6402es_hifi_misc_write(struct file *file,
				      const char __user *buf,
				      size_t nbytes, loff_t *pos)
{
	struct reg_rw_struct kern_buf;
	if (copy_from_user(&kern_buf, (void  __user *)buf, nbytes)) {
		HI6402ES_DSP_ERROR("copy_from_user fail.\n");
		return -EFAULT;
	}

	hi6402es_hifi_write_reg(kern_buf.reg, kern_buf.val);

	return 0;
}


static const struct file_operations hi6402es_hifi_misc_fops = {
	.owner			= THIS_MODULE,
	.open			= hi6402es_hifi_misc_open,
	.release		= hi6402es_hifi_misc_release,
	.read			= hi6402es_hifi_misc_read,
	.write			= hi6402es_hifi_misc_write,
	.unlocked_ioctl = hi6402es_hifi_misc_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = hi6402es_hifi_misc_ioctl,
#endif
};

static struct miscdevice hi6402es_hifi_misc_device = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "hi6402_hifi_misc",
	.fops	= &hi6402es_hifi_misc_fops,
};


/*
 *  dsp img download
 */
#define HI6402ES_HIFI_FW "hifi_6402/hifi_6402.bin"

static int hi6402es_hifi_fw_section_head_check(struct drv_hifi_image_head *img_head,
					     struct drv_hifi_image_sec *img_sec)
{
	/* BSS section do not need check offset and size,
	 * beacuse BSS section only record address and length, the content is 0 */
	if (img_sec->type == DRV_HIFI_IMAGE_SEC_TYPE_BSS
		&& (img_sec->sn < img_head->sections_num)
		&& (img_sec->load_attib < (unsigned char)DRV_HIFI_IMAGE_SEC_LOAD_BUTT))
		return 0;

	/* check section number and section size  */
	if ((img_sec->sn >= img_head->sections_num)
		|| (img_sec->src_offset + img_sec->size > img_head->image_size)
		|| (img_sec->type >= (unsigned char)DRV_HIFI_IMAGE_SEC_TYPE_BUTT)
		|| (img_sec->load_attib >= (unsigned char)DRV_HIFI_IMAGE_SEC_LOAD_BUTT))
	{
		HI6402ES_DSP_INFO("hifi: drv_hifi_check_sections ERROR.\n");
		return -1;
	}

	return 0;
}

static int hi6402es_hifi_fw_head_check(struct drv_hifi_image_head *head)
{
	int i = 0;
	int ret = 0;

	for (i = 0; i < head->sections_num; i++) {
		/* check the sections */
		ret = hi6402es_hifi_fw_section_head_check(head, &(head->sections[i]));

		if (ret != 0) {
			HI6402ES_DSP_INFO("hifi: Invalid hifi section\n");
			goto exit;
		}
	}

exit:
	return ret;
}

static void hi6402es_hifi_download(const struct firmware *fw)
{
	struct drv_hifi_image_head	  *head = NULL;
	int i = 0;
	int ret = 0;

	/* load firmware */
	HI6402ES_DSP_INFO("size [%zu]\n", fw->size);
	head = (struct drv_hifi_image_head *)fw->data;

	ret = hi6402es_hifi_fw_head_check(head);

	for (i = 0; i < head->sections_num; i++) {

		unsigned int des_addr;
		unsigned int *src_addr;
		int j = 0;

		src_addr = (unsigned int *)((char *)head + head->sections[i].src_offset);
		des_addr = head->sections[i].des_addr;

		HI6402ES_DSP_DEBUG("hifi: sections_num = %d,des_addr = 0x%x, load_attib = %d, size = 0x%x,"
				 " sn = %d, src_offset = 0x%x, type = %d\n", \
				 head->sections_num,\
				 head->sections[i].des_addr,\
				 head->sections[i].load_attib,\
				 head->sections[i].size,\
				 head->sections[i].sn,\
				 head->sections[i].src_offset,\
				 head->sections[i].type);

		HI6402ES_DSP_INFO("[0x%p]->[0x%x] [0x%x]", src_addr, des_addr, *src_addr);
		if(head->sections[i].type == DRV_HIFI_IMAGE_SEC_TYPE_BSS) {
			for(j = 0; j < head->sections[i].size; j = j + 4) {
				/* copy the sections */
				hi6402es_hifi_write_reg(des_addr, 0x0);
				des_addr += 4;
			}
		} else {
			for(j = 0; j < head->sections[i].size; j = j + 4) {
				/* copy the sections */
				hi6402es_hifi_write_reg(des_addr, *(src_addr));
				src_addr++;
				des_addr += 4;
			}
		}
	}

}

static void hi6402es_hifi_pwron_work(void)
{
	/* Soft reset HIFI */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_SW_RST_REQ, 1);

	/* Soft reset APB of power-off area */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_CLK_CTRL, 4);

	/* Close DSPIF clocks, and soft reset DSPIF */
	hi6402es_hifi_write_reg(HI6402ES_DSP_I2S_DSPIF_CLK_EN, 0x0);
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_DAC_DP_CLK_EN_1, 4);

	/* turn on power-off area */
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_LP_CTRL, 0);

	/* Disable isolation cell */
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_LP_CTRL, 1);

	/* Open HIFI clocks */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_SC_DSP_CTRL0, 0);
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_SC_DSP_CTRL0, 3);

	/* Open APB clock of power-off area */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_APB_CLK_CFG, 6);

	/* open DSPIF clocks */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_DAC_DP_CLK_EN_1, 4);

	/* Release HIFI reset */
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_SW_RST_REQ, 1);
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_CLK_CTRL, 4);

	/* enable axi */
	hi6402es_hifi_write_reg(HI6402ES_DSP_RAM2AXI_CTRL, 0x01);

}

static void hi6402es_hifi_pwron(void)
{
	mutex_lock(&priv.power_ref_mutex);
	if(0 == priv.power_ref)
		hi6402es_hifi_pwron_work();
	priv.power_ref++;
	mutex_unlock(&priv.power_ref_mutex);
	return;
}

static void hi6402es_hifi_pwroff_work(void)
{
	/* Close HIFI clock */
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_SC_DSP_CTRL0, 0);
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_SC_DSP_CTRL0, 3);

	/* Close APB clock */
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_APB_CLK_CFG, 6);

	/* Close DSPIF clocks, and soft reset DSPIF */
	hi6402es_hifi_write_reg(HI6402ES_DSP_I2S_DSPIF_CLK_EN, 0x0);
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_DAC_DP_CLK_EN_1, 4);

	/* Enable isolation cell */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_LP_CTRL, 1);

	/* Soft reset HIFI */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_SW_RST_REQ, 1);

	/* Soft reset APB */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_CLK_CTRL, 4);

	/* Turn off power of power-off area */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_LP_CTRL, 0);

	/* Pull up runstall of HIFI */
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_SC_DSP_CTRL0, 2);

	/* disable axi */
	hi6402es_hifi_write_reg(HI6402ES_DSP_RAM2AXI_CTRL, 0x05);
}

static void hi6402es_hifi_pwroff(void)
{
	mutex_lock(&priv.power_ref_mutex);
	if(0 == priv.power_ref){
		mutex_unlock(&priv.power_ref_mutex);
		return;
	}
	priv.power_ref--;
	if(0 == priv.power_ref)
		hi6402es_hifi_pwroff_work();
	mutex_unlock(&priv.power_ref_mutex);
	return;
}

static void hi6402es_hifi_enable_pll_work(void)
{
	/* set pll */
	BUG_ON(priv.p_irq == NULL);
	hi6402es_irq_pll_enable(priv.p_irq, true);

}

static void hi6402es_hifi_enable_pll(void)
{
	mutex_lock(&priv.clk_mutex);
	if(0 == priv.clk_ref)
		hi6402es_hifi_enable_pll_work();
	priv.clk_ref++;
	mutex_unlock(&priv.clk_mutex);
}

static void hi6402es_hifi_disable_pll_work(void)
{
	/* set pll */
	BUG_ON(priv.p_irq == NULL);
	hi6402es_irq_pll_enable(priv.p_irq, false);
}

static void hi6402es_hifi_disable_pll(void)
{
	mutex_lock(&priv.clk_mutex);
	if(0 == priv.clk_ref){
		mutex_unlock(&priv.clk_mutex);
		return;
	}
	priv.clk_ref--;
	if(0 == priv.clk_ref)
		hi6402es_hifi_disable_pll_work();
	mutex_unlock(&priv.clk_mutex);
}

static void hi6402es_hifi_enable_mad_pll(void)
{
#if 0
	/* set pll */
	BUG_ON(priv.p_irq == NULL);
	hi6402es_irq_mad_pll_enable(priv.p_irq, true);
#endif
}

static void hi6402es_hifi_disable_mad_pll(void)
{
#if 0
	/* set pll */
	BUG_ON(priv.p_irq == NULL);
	hi6402es_irq_mad_pll_enable(priv.p_irq, false);
#endif
}

static void hi6402es_hifi_enable_jtag(void)
{
	hi6402es_hifi_write_reg(HI6402ES_DSP_IOS_IOM_I2S2_SDO, 0x1c4);
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_IOS_AF_CTRL0, 0);
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_IOSHARE_BASE, 5);
}

static void hi6402es_hifi_enable_uart(void)
{
	/* enable jtag and uart should set in dsp just for test*/
	hi6402es_hifi_write_reg(HI6402ES_DSP_IOS_IOM_UART_TXD, 0x1c4);
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_IOSHARE_BASE, 12);
}


static void hi6402es_hifi_runstall_pull_down_work(void)
{
	/* Pull down runstall of HIFI*/
	hi6402es_hifi_reg_clr_bit(HI6402ES_DSP_SC_DSP_CTRL0, 2);
}

static void hi6402es_hifi_runstall_pull_down(void)
{
	mutex_lock(&priv.runstall_mutex);
	if(0 == priv.runstall_ref)
		hi6402es_hifi_runstall_pull_down_work();
	priv.runstall_ref++;
	mutex_unlock(&priv.runstall_mutex);
}

static void hi6402es_hifi_runstall_pull_up_work(void)
{
	/* Pull up runstall of HIFI*/
	hi6402es_hifi_reg_set_bit(HI6402ES_DSP_SC_DSP_CTRL0, 2);
}

static void hi6402es_hifi_runstall_pull_up(void)
{
	mutex_lock(&priv.runstall_mutex);
	if(0 == priv.runstall_ref){
		mutex_unlock(&priv.runstall_mutex);
		return;
	}
	priv.runstall_ref--;
	if(0 == priv.runstall_ref)
		hi6402es_hifi_runstall_pull_up_work();
	mutex_unlock(&priv.runstall_mutex);
}

static void hi6402es_hifi_fw_redownload(void)
{
	int ret = 0;
	const struct firmware *fw;

	/* download dsp image */
	ret = request_firmware(&fw, HI6402ES_HIFI_FW, priv.dev);

	if (ret != 0) {
		dev_err(priv.dev, "Failed to request application(%s): %d\n",
			HI6402ES_HIFI_FW, ret);
		return;
	}

	BUG_ON(fw == NULL);

	hi6402es_hifi_download(fw);
	release_firmware(fw);

	return;
}

static void hi6402es_hifi_fw_download(const struct firmware *fw, void *context)
{
	BUG_ON(fw == NULL);
	hi6402es_hifi_enable_pll();
	hi6402es_hifi_pwron();
	hi6402es_hifi_download(fw);
	release_firmware(fw);
	hi6402es_hifi_pwroff();
	hi6402es_hifi_disable_pll();
	//hi6402es_hifi_runstall_pull_down();
}

static void start_wakeup(void)
{
	/* enable mad clk */
	hi6402es_hifi_enable_mad_pll();
	/* hi6402es hifi power on */
	hi6402es_hifi_pwron();
	/* hi6402es hifi runstall pull down */
	hi6402es_hifi_runstall_pull_down();

	priv.wakeup_state = 1;
}

static void stop_wakeup(void)
{
	priv.wakeup_state = 0;
	/* hi6402es hifi runstall pull up */
	hi6402es_hifi_runstall_pull_up();
	/* hi6402es hifi power off */
	hi6402es_hifi_pwroff();
	/* enable mad clk */
	hi6402es_hifi_disable_mad_pll();
}

static void start_hifi(void)
{
	/* enable mad clk */
	hi6402es_hifi_enable_pll();
	/* hi6402es hifi power on */
	hi6402es_hifi_pwron();
	/* hi6402es hifi runstall pull down */
	hi6402es_hifi_runstall_pull_down();
}

static void stop_hifi(void)
{
	/* hi6402es hifi runstall pull up */
	hi6402es_hifi_runstall_pull_up();
	/* hi6402es hifi power off */
	hi6402es_hifi_pwroff();
	/* enable mad clk */
	hi6402es_hifi_disable_pll();
}

static void hi6402es_mad_set_param(void)
{
	//ana_stable time 5ms
	hi6402es_hifi_write_reg(HI6402ES_MAD_ANA_TIME_L, 0xAA);
	//pll stable time
	hi6402es_hifi_write_reg(HI6402ES_MAD_PLL_TIME_L, 0x01);
	//adc stable time 100us
	hi6402es_hifi_write_reg(HI6402ES_MAD_ADC_TIME_L, 0xA5);
	//mad_omit_samp
	hi6402es_hifi_write_reg(HI6402ES_MAD_OMIT_SAMP, 0x10);
	//rms_number
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_NUM, 0x01);
	//mad_auto_act_number
	hi6402es_hifi_write_reg(HI6402ES_MAD_AUTO_ACT_NUMBER, 0x0);
	//mad_rms_mode
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_MODE, 0xC3);
	//mad_sleep_time0
	hi6402es_hifi_write_reg(HI6402ES_MAD_SLEEP_TIME0, 0xA0);
	//mad_sleep_time1
	hi6402es_hifi_write_reg(HI6402ES_MAD_SLEEP_TIME1, 0x0);
	//mad_rms_int_en
	hi6402es_hifi_reg_set_bit(HI6402ES_MAD_RMS_INT_EN, HI6402ES_MAD_RMS_INT_EN_BIT);
	//mad_rms_thre
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_THRE0, 0x00);
	//mad_rms_thre
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_THRE1, 0xA8);
	//mad_rms_thre
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_THRE2, 0x61);
	//mad_rms_thre
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_THRE3, 0x00);
	//mad_rms_time
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_TIME0, 0xA0);
	//mad_rms_time
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_TIME1, 0x00);
	//mad_rms_diff_thre
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_DIFF_THRE0, 0x49);
	//mad_rms_diff_thre
	hi6402es_hifi_write_reg(HI6402ES_MAD_RMS_DIFF_THRE1, 0x28);
	//mad_filter_id
	hi6402es_hifi_reg_write_bits(HI6402ES_MAD_FILTER_ID,
		0x3<<HI6402ES_MAD_FILTER_ID_BIT, 0x7<<HI6402ES_MAD_FILTER_ID_BIT);
	//mad_filter_id1
	hi6402es_hifi_reg_write_bits(HI6402ES_MAD_FILTER_ID1,
		0x0<<HI6402ES_MAD_FILTER_ID1_BIT, 0x7<<HI6402ES_MAD_FILTER_ID1_BIT);
	return;
}
/* ********************************************************************
 * sysfs device, for debugging
 * ********************************************************************/
static ssize_t hi6402es_debug_show(struct device *dev,
				 struct device_attribute *attr,
				 char *buf)
{
	char *help_info = "1 : pwron \n2 : pwroff  \n3 : run \n4 : stop \n"
			  "5 : jtag \n6 : uart\n7 : enable pll\n"
			  "8 : disable pll\n0 : download\n";
	return snprintf(buf, PAGE_SIZE, "%s\n", help_info);
}

static ssize_t hi6402es_debug_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t size)
{
	unsigned long cmd = 0;

	if (buf == NULL) {
		HI6402ES_DSP_ERROR("buf is NULL\n");
		return -EINVAL;
	}

	if (strict_strtoul(buf, 0, &cmd) != 0) {
		HI6402ES_DSP_ERROR("input para is error\n");
		return -EINVAL;
	}

	switch (cmd) {
	case 0:
		hi6402es_hifi_fw_redownload();
		break;
	case 1:
		hi6402es_hifi_pwron();
		break;
	case 2:
		hi6402es_hifi_pwroff();
		break;
	case 3:
		hi6402es_hifi_runstall_pull_down();
		break;
	case 4:
		hi6402es_hifi_runstall_pull_up();
		break;
	case 5:
		hi6402es_hifi_enable_jtag();
		break;
	case 6:
		hi6402es_hifi_enable_uart();
		break;
	case 7:
		hi6402es_hifi_enable_pll();
		break;
	case 8:
		hi6402es_hifi_disable_pll();
		break;
	default:
		break;
	}

	return size;
}

static ssize_t hi6402es_debuglevel_show(struct device *dev,
                                      struct device_attribute *attr,
                                      char *buf)
{
	IN_FUNCTION;

	return snprintf(buf, PAGE_SIZE, "%ld\n", hi6402es_dsp_debug_level);
}



static ssize_t hi6402es_debuglevel_store(struct device *dev,
                                       struct device_attribute *attr,
                                       const char *buf, size_t size)
{
	IN_FUNCTION;

	if (buf == NULL) {
		HI6402ES_DSP_ERROR("buf is NULL\n");
		return -EINVAL;
	}

	if (strict_strtoul(buf, 0, &hi6402es_dsp_debug_level) != 0) {
		HI6402ES_DSP_ERROR("input para is error\n");
		return -EINVAL;
	}

	OUT_FUNCTION;

	return size;
}

static DEVICE_ATTR(hi6402esdebug, S_IRUGO | S_IWUSR|S_IWGRP,
		   hi6402es_debug_show, hi6402es_debug_store);
static DEVICE_ATTR(hi6402esdebuglevel, S_IRUGO | S_IWUSR|S_IWGRP,
		   hi6402es_debuglevel_show, hi6402es_debuglevel_store);

static int hi6402es_create_sysfs_file(struct device *dev,
                                    const struct device_attribute *attr)
{
	BUG_ON(dev == NULL);

	return sysfs_create_file(&dev->kobj, &attr->attr);
}

static void hi6402es_remove_sysfs_file(struct device *dev,
                                     const struct device_attribute *attr)
{
	BUG_ON(dev == NULL);

	sysfs_remove_file(&dev->kobj, &attr->attr);

	return;
}

static int hi6402es_create_sysfs(struct platform_device *pdev)
{
	BUG_ON(pdev == NULL);

	hi6402es_create_sysfs_file(&pdev->dev, &dev_attr_hi6402esdebuglevel);
	hi6402es_create_sysfs_file(&pdev->dev, &dev_attr_hi6402esdebug);

	return 0;
}

static void hi6402es_remove_sysfs(struct platform_device *pdev)
{
	BUG_ON(pdev == NULL);

	hi6402es_remove_sysfs_file(&pdev->dev, &dev_attr_hi6402esdebuglevel);
	hi6402es_remove_sysfs_file(&pdev->dev, &dev_attr_hi6402esdebug);

	return;
}

/*****************************************************************************
 * platfrom driver
 * */
#define DTS_COMP_HI6402ES_DSP_NAME "hisilicon,hi6402es-dsp"

static int hi6402es_hifi_misc_probe (struct platform_device *pdev)
{
	int ret = 0;
	struct device *dev = &pdev->dev;

	IN_FUNCTION;

	BUG_ON(dev == NULL);
	priv.dev = &pdev->dev;
	/* get clk & regulator */
	priv.p_irq = dev_get_drvdata(pdev->dev.parent);
	if (!priv.p_irq) {
		HI6402ES_DSP_ERROR("get parend device error\n");
		return -ENOENT;
	}

	mutex_init(&priv.msg_mutex);
	mutex_init(&priv.power_ref_mutex);
	mutex_init(&priv.runstall_mutex);
	mutex_init(&priv.clk_mutex);

	priv.irq_cmd_valid = platform_get_irq_byname(pdev, "cmd_valid");
	if (0 > priv.irq_cmd_valid) {
		HI6402ES_DSP_ERROR("get cmd_valid irq failed! \n");
		return -ENOENT;
	}

	/* irq request : cmd_valid */
	ret = request_irq(priv.irq_cmd_valid,
			  hi6402es_msg_irq_handler,
			  IRQF_DISABLED | IRQF_NO_SUSPEND, "cmd_valid", &priv);
	if (0 > ret) {
		HI6402ES_DSP_ERROR("request_irq failed! \n");
		return -1;
	}

	misc_register(&hi6402es_hifi_misc_device);
	hi6402es_create_sysfs(pdev);

	priv.msg_send_done = 0;
	init_waitqueue_head(&priv.send_msg_wq);

	priv.sync_msg_ret = 0;
	init_waitqueue_head(&priv.sync_msg_wq);

	priv.power_ref = 0;
	priv.runstall_ref = 0;
	priv.clk_ref = 0;

	priv.wakeup_state = 0;
	priv.datahook_state = 0;
	priv.smartpa_state = 0;
	priv.madtest_state = 0;

	/* down load dsp image */
	request_firmware_nowait(THIS_MODULE, FW_ACTION_NOHOTPLUG,
		HI6402ES_HIFI_FW,  &(pdev->dev), GFP_KERNEL,
		NULL, hi6402es_hifi_fw_download);

	/* MAD set param */
	hi6402es_mad_set_param();

	OUT_FUNCTION;

	return 0;
}


static int hi6402es_hifi_misc_remove(struct platform_device *pdev)
{
	IN_FUNCTION;

	(void)misc_deregister(&hi6402es_hifi_misc_device);

	hi6402es_remove_sysfs(pdev);

	OUT_FUNCTION;

	return 0;
}


static const struct of_device_id hi6402es_dsp_match_table[] = {
	{
		.compatible = DTS_COMP_HI6402ES_DSP_NAME,
	},
	{},
};
MODULE_DEVICE_TABLE(of, hi6402es_dsp_match_table);


static struct platform_driver hi6402es_hifi_misc_driver = {
	.driver =
	{
		.name           = "hi6402es hifi misc",
		.owner          = THIS_MODULE,
		.of_match_table = of_match_ptr(hi6402es_dsp_match_table),
	},
	.probe  = hi6402es_hifi_misc_probe,
	.remove = hi6402es_hifi_misc_remove,
};

static int __init hi6402es_hifi_misc_init(void)
{
	return platform_driver_register(&hi6402es_hifi_misc_driver);
}

static void __exit hi6402es_hifi_misc_exit(void)
{
	platform_driver_unregister(&hi6402es_hifi_misc_driver);
}

late_initcall(hi6402es_hifi_misc_init);
module_exit(hi6402es_hifi_misc_exit);

MODULE_DESCRIPTION("hi6402es hifi misc driver");
MODULE_LICENSE("GPL");
