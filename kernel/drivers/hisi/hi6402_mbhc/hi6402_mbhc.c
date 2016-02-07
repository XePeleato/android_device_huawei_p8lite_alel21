/*
 * hi6402_mbhc.c -- hi6402 mbhc driver
 *
 * Copyright (c) 2014 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/miscdevice.h>
#include <sound/soc.h>
#include <sound/jack.h>
#include <linux/switch.h>
#include <linux/wakelock.h>
#include <huawei_platform/dsm/dsm_pub.h>
#include <linux/input.h>
#include <hi6402_algo_interface.h>

#include "hi6402_mbhc.h"
#include "huawei_platform/audio/anc_hs.h"

#define LOG_TAG "HI6402_MBHC"

#define PRINT_INFO  0
#define PRINT_WARN  0
#define PRINT_DEBUG 0
#define PRINT_ERR   1

#if PRINT_INFO
#define logi(fmt, ...) printk("[" LOG_TAG "][I]" fmt "\n", ##__VA_ARGS__)
#else
#define logi(fmt, ...) ((void)0)
#endif

#if PRINT_WARN
#define logw(fmt, ...) printk("[" LOG_TAG "][W]" fmt "\n", ##__VA_ARGS__)
#else
#define logw(fmt, ...) ((void)0)
#endif

#if PRINT_DEBUG
#define logd(fmt, ...) printk("[" LOG_TAG "][D]" fmt "\n", ##__VA_ARGS__)
#else
#define logd(fmt, ...) ((void)0)
#endif

#if PRINT_ERR
#define loge(fmt, ...) printk("[" LOG_TAG "][E]" fmt "\n", ##__VA_ARGS__)
#else
#define loge(fmt, ...) ((void)0)
#endif

#define HI6402_CFG_BASE_ADDR	0x20007000

#define HI6402_REG_IRQ_0	(HI6402_CFG_BASE_ADDR + 0x14)
#define HI6402_HS_PLUGIN_BIT		6

#define HI6402_DSP_VAD_CMD	(HI6402_CFG_BASE_ADDR + 0x73)

#define HI6402_REG_IRQ_1	(HI6402_CFG_BASE_ADDR + 0x15)
#define HI6402_VAD_BIT			4
#define HI6402_REG_HSDET_CTRL	(HI6402_CFG_BASE_ADDR + 0xDE)

#define HI6402_MBHC_VREF_REG		(HI6402_CFG_BASE_ADDR + 0xE0)
#define HI6402_MBHC_VREF_BIT			7

#define HI6402_MICBIAS_ECO_REG		(HI6402_CFG_BASE_ADDR + 0xE1)
#define HI6402_MICBIAS_ECO_ON_BIT		0

#define HI6402_VAD_INT_SET	0x20003000

#define HI6402_IRQ_SOURCE_REG	(HI6402_CFG_BASE_ADDR + 0x1C)
#define HI6402_PLUGIN_IRQ_BIT			3

#define HI6402_REG_IRQ_MBHC_1		(HI6402_CFG_BASE_ADDR + 0x15)
#define HI6402_SARADC_CMP_BIT			7

#define HI6402_REG_IRQ_MBHC_2		(HI6402_CFG_BASE_ADDR + 0x16)
#define HI6402_SARADC_RD_BIT			2

#define HI6402_MASK_IRQ_REG_2		(HI6402_CFG_BASE_ADDR + 0x19)
#define HI6402_MASK_PLL_UNLOCK_BIT		4

#define HI6402_PLL_CFG_REG		(HI6402_CFG_BASE_ADDR + 0xE6)
#define HI6402_PLL_RST_BIT				2

#define HI6402_ANA_REG60			(HI6402_CFG_BASE_ADDR + 0xDD)
#define HI6402_SARADC_PD_BIT			6
#define HI6402_SAR_START_BIT			5
#define HI6402_MBHC_ON_BIT				4

#define HI6402_PLL_LOCK_FLAG_REG	(HI6402_CFG_BASE_ADDR + 0xF6)
#define PLL_LOCK_VALUE					0x02

#define HI6402_ANA_ROREG7			(HI6402_CFG_BASE_ADDR + 0xF8)
#define HI6402_SARADC_RDY_BIT			0

/* SARADC */
#define HI6402_SAR_EP_CFG0_REG		(HI6402_CFG_BASE_ADDR + 0x006)
#define HI6402_SAR_AUTO_EN_BIT			0

#define HI6402_SAR_EP_CFG1_REG		(HI6402_CFG_BASE_ADDR + 0x007)
#define HI6402_SAR_CFG_REG			(HI6402_CFG_BASE_ADDR + 0x0DF)

#define HI6402_BTN_VALUE_REG0		(HI6402_CFG_BASE_ADDR + 0x0B)
#define HI6402_BTN_VALUE_REG1		(HI6402_CFG_BASE_ADDR + 0x0C)
#define HI6402_BTN_VALUE_REG2		(HI6402_CFG_BASE_ADDR + 0x0D)

#define HI6402_MBHC_MASK_PLUGOUT_BIT		7
#define HI6402_MBHC_MASK_PLUGIN_BIT			6
#define HI6402_MBHC_MASK_ECODOWN_BIT		5
#define HI6402_MBHC_MASK_ECOUP_BIT			4
#define HI6402_MBHC_MASK_DOWN2_BIT			3
#define HI6402_MBHC_MASK_UP2_BIT			2
#define HI6402_MBHC_MASK_DOWN_BIT			1
#define HI6402_MBHC_MASK_UP_BIT				0

#define HI6402_SARADC_VALUE_REG	(HI6402_CFG_BASE_ADDR + 0xF1)

#define HI6402_PLL_PD_FLAG_REG	(HI6402_CFG_BASE_ADDR + 0xD0)
#define PLL_PD_VALUE					0x02

#define PLL_UNLOCK_REG_START	(HI6402_CFG_BASE_ADDR + 0xA0)
#define PLL_UNLOCK_REG_END		(HI6402_CFG_BASE_ADDR + 0xF8)

#define GET_VOLTAGE(value)	(value * 2800 / 0xFF)	/* saradc range 0 ~ 2800mV */

#define SOUNDTRIGGER_EVENT_BASE				KEY_F14
#define SOUNDTRIGGER_EVENT_NUM				(5)

static struct dsm_dev dsm_audio = {
    .name = "dsm_audio",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
    .fops = NULL,
    .buff_size = 4096,
};
struct dsm_client *dsm_audio_client = NULL;

static const struct of_device_id hi6402_mbhc_of_match[] = {
	{
		.compatible = "hisilicon,hi6402-mbhc",
	},
	{ },
};
MODULE_DEVICE_TABLE(of, hi6402_mbhc_of_match);

#define HI6402_BTN_MASK	(SND_JACK_BTN_0 | SND_JACK_BTN_1 | SND_JACK_BTN_2)

extern void hi6402_soc_jack_report(int report, int mask);

static void hi6402_mbhc_enable_ldo8(struct hi6402_mbhc_platform_data *pdata, bool enable)
{
	BUG_ON(NULL == pdata->p_irq);
	if (enable) {
		if (!pdata->ldo8_status) {
			hi6402_irq_enable_ldo8(pdata->p_irq, true);
			pdata->ldo8_status = 1;
		}
	} else {
		if (pdata->ldo8_status) {
			hi6402_irq_enable_ldo8(pdata->p_irq, false);
			pdata->ldo8_status = 0;
		}
	}
}

void hi6402_jack_report(struct hi6402_mbhc_platform_data *pdata)
{
	enum hisi_jack_states jack_status = pdata->hs_status;
	int jack_report = 0;

	switch(pdata->hs_status) {
	case HISI_JACK_NONE:
		jack_report = 0;
		pr_info("%s : plug out\n", __FUNCTION__);
		break;
	case HISI_JACK_HEADSET:
		jack_report = SND_JACK_HEADSET;
		pr_info("%s : 4-pole headset plug in\n", __FUNCTION__);
		break;
	case HISI_JACK_INVERT:
		jack_report = SND_JACK_HEADPHONE;
		pr_info("%s : invert headset plug in\n", __FUNCTION__);
		break;
	case HISI_JACK_HEADPHONE:
		jack_report = SND_JACK_HEADPHONE;
		pr_info("%s : 3-pole headphone plug in\n", __FUNCTION__);
		break;
	default:
		pr_err("%s : error hs_status(%d)\n", __FUNCTION__, pdata->hs_status);
		break;
	}

	/* clear btn event */
	hi6402_soc_jack_report(0, HI6402_BTN_MASK);
	/* btn_report jack status */
	hi6402_soc_jack_report(jack_report, SND_JACK_HEADSET);
#ifdef CONFIG_SWITCH
	switch_set_state(&pdata->sdev, jack_status);
#endif
}

static inline bool check_headset_pluged_in(struct hi6402_mbhc_platform_data *pdata)
{
	/*
	 * 0 : means headset is pluged out
	 * 1 : means headset is pluged in
	 */
	return (0 != (hi6402_irq_read(pdata->p_irq, HI6402_IRQ_SOURCE_REG) & (1 << HI6402_PLUGIN_IRQ_BIT)));
}

static inline bool check_saradc_value_ready_detect(struct hi6402_mbhc_platform_data *pdata)
{
	/* read codec status */
	int value = hi6402_irq_read(pdata->p_irq, HI6402_REG_IRQ_MBHC_2) & (1 << HI6402_SARADC_RD_BIT);

	/*clr irq*/
	hi6402_irq_write(pdata->p_irq, HI6402_REG_IRQ_MBHC_2, 0x04);

	if (0 == value)
		return false;

	return true;
}

static int hi6402_read_saradc_value_detect(struct hi6402_mbhc_platform_data *pdata)
{
	int retry = 3;
	int reg_value = 0;

	BUG_ON(NULL == pdata->p_irq);

	mutex_lock(&pdata->saradc_mutex);

	/* saradc on */
	hi6402_reg_clr_bit(pdata->p_irq, HI6402_ANA_REG60, HI6402_SARADC_PD_BIT);
	/* start saradc */
	hi6402_reg_set_bit(pdata->p_irq, HI6402_ANA_REG60, HI6402_SAR_START_BIT);

	while(retry--) {
		usleep_range(1000, 1100);
		if (check_saradc_value_ready_detect(pdata)) {
			reg_value = hi6402_irq_read(pdata->p_irq, HI6402_SARADC_VALUE_REG);
			pr_info("%s : saradc value is %#x\n", __FUNCTION__, reg_value);
			break;
		}
	}

	if (0 > retry)
		pr_err("%s : get saradc value err, set as 0\n", __FUNCTION__);

	/* end saradc */
	hi6402_reg_clr_bit(pdata->p_irq, HI6402_ANA_REG60, HI6402_SAR_START_BIT);
	/* saradc pd */
	hi6402_reg_set_bit(pdata->p_irq, HI6402_ANA_REG60, HI6402_SARADC_PD_BIT);

	mutex_unlock(&pdata->saradc_mutex);

	return GET_VOLTAGE(reg_value);
}

void hi6402_mbhc_on(struct hi6402_mbhc_platform_data *pdata)
{
	/* open mbhc */
	hi6402_reg_clr_bit(pdata->p_irq, HI6402_MBHC_VREF_REG, HI6402_MBHC_VREF_BIT);
	/* saradc cfg */
	hi6402_irq_write(pdata->p_irq, HI6402_SAR_CFG_REG, 0x7C);
	/* mbhc on */
	hi6402_reg_clr_bit(pdata->p_irq, HI6402_ANA_REG60, HI6402_MBHC_ON_BIT);
	msleep(150);

	return;
}

void hi6402_plug_in_detect(struct hi6402_mbhc_platform_data *pdata)
{
	int saradc_value = 0;
	int anc_type = ANC_HS_REVERT_4POLE;

	if (!check_headset_pluged_in(pdata))
		return;

	wake_lock(&pdata->wake_lock);
	mutex_lock(&pdata->plug_mutex);

	if(check_anc_hs_support()) {
		//mask btn irqs while control boost
		hi6402_irq_mask_btn_irqs(pdata->p_irq);
		anc_hs_start_charge();
	}

	/* micbias on */
	hi6402_irq_micbias_mbhc_enable(pdata->p_irq, true);

	/* mbhc on (normal not auto) */
	hi6402_mbhc_on(pdata);
	/* read hs value */
	saradc_value = hi6402_read_saradc_value_detect(pdata);

	mutex_lock(&pdata->status_mutex);

	if (pdata->hs_3_pole_max_voltage >= saradc_value) {
		/* 3-pole headphone */
		pr_info("%s : 3 pole is pluged in\n", __FUNCTION__);
		pdata->hs_status = HISI_JACK_HEADPHONE;
		anc_type = ANC_HS_3POLE;
	} else if (pdata->hs_4_pole_min_voltage <= saradc_value &&
			pdata->hs_4_pole_max_voltage >= saradc_value) {
		/* 4-pole headset */
		pr_info("%s : 4 pole is pluged in\n", __FUNCTION__);
		pdata->hs_status = HISI_JACK_HEADSET;
		anc_type = ANC_HS_4POLE;
	} else {
		/* invert 4-pole headset */
		pr_info("%s : need further detect, report as 3-pole headphone\n", __FUNCTION__);
		pdata->hs_status = HISI_JACK_INVERT;
		anc_type = ANC_HS_REVERT_4POLE;
	}

	mutex_unlock(&pdata->status_mutex);

	if(check_anc_hs_support()) {
		//mask btn irqs while control boost
		hi6402_irq_mask_btn_irqs(pdata->p_irq);
		anc_hs_charge_detect(saradc_value, anc_type);
	}

	hi6402_jack_report(pdata);

	hi6402_mbhc_enable_ldo8(pdata, true);

	/* micbias off */
	hi6402_irq_micbias_mbhc_enable(pdata->p_irq, false);

	mutex_unlock(&pdata->plug_mutex);
	wake_unlock(&pdata->wake_lock);
}

void hi6402_btn_down(struct hi6402_mbhc_platform_data *pdata)
{
	int saradc_value = 0;

	if (!check_headset_pluged_in(pdata)) {
		pr_info("%s(%u) : hs pluged out \n", __FUNCTION__, __LINE__);
		return;
	}

	wake_lock(&pdata->wake_lock);

	if (HISI_JACK_HEADSET == pdata->hs_status) {
		/* micbias on */
		hi6402_irq_micbias_mbhc_enable(pdata->p_irq, true);

		/* auto read */
		saradc_value = hi6402_read_saradc_value_detect(pdata);

		/* micbias off */
		hi6402_irq_micbias_mbhc_enable(pdata->p_irq, false);

		msleep(30);

		if (!check_headset_pluged_in(pdata)) {
			pr_info("%s(%u) : hs pluged out \n", __FUNCTION__, __LINE__);
			goto end;
		}

		if ((saradc_value >= pdata->hs_4_pole_min_voltage) && (saradc_value <= pdata->hs_4_pole_max_voltage)) {
			pr_info("%s(%u) : process as btn up! \n", __FUNCTION__, __LINE__);
			mutex_lock(&pdata->status_mutex);
			pdata->btn_report = 0;
			mutex_unlock(&pdata->status_mutex);
		} else if ((saradc_value >= pdata->btn_play_min_voltage) && (saradc_value <= pdata->btn_play_max_voltage)) {
			mutex_lock(&pdata->status_mutex);
			pdata->btn_report = SND_JACK_BTN_0;
			mutex_unlock(&pdata->status_mutex);
		} else if (pdata->btn_volume_up_min_voltage < saradc_value && saradc_value <= pdata->btn_volume_up_max_voltage) {
			mutex_lock(&pdata->status_mutex);
			pdata->btn_report = SND_JACK_BTN_1;
			mutex_unlock(&pdata->status_mutex);
		} else if (pdata->btn_volume_down_min_voltage < saradc_value && saradc_value <= pdata->btn_volume_down_max_voltage) {
			mutex_lock(&pdata->status_mutex);
			pdata->btn_report = SND_JACK_BTN_2;
			mutex_unlock(&pdata->status_mutex);
		} else {
			msleep(30);
			hi6402_plug_in_detect(pdata);
			goto end;
		}

		if (!check_headset_pluged_in(pdata)) {
			pr_info("%s(%u) : hs pluged out \n", __FUNCTION__, __LINE__);
			goto end;
		}

		/*btn_report key event*/
		pr_info("%s(%u): btn_report type = 0x%x, status=0x%x\n",
				__FUNCTION__, __LINE__, pdata->btn_report, pdata->hs_status);
		hi6402_soc_jack_report(pdata->btn_report, HI6402_BTN_MASK);
	}

end:
	wake_unlock(&pdata->wake_lock);

	return;
}

static irqreturn_t hi6402_plugin_handler(int irq, void *data)
{
	struct hi6402_mbhc_platform_data *pdata =
			(struct hi6402_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	msleep(800);

	hi6402_plug_in_detect(pdata);

	return IRQ_HANDLED;
}

static irqreturn_t hi6402_plugout_handler(int irq, void *data)
{
	struct hi6402_mbhc_platform_data *pdata =
			(struct hi6402_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	if (check_headset_pluged_in(pdata)) {
		pr_info("%s : hs still plugin \n", __FUNCTION__);
		return IRQ_HANDLED;
	}

	mutex_lock(&pdata->plug_mutex);

	hi6402_irq_cancel_delay_work(pdata->p_irq);

	hi6402_irq_mask_btn_irqs(pdata->p_irq);
	//stop charge first
	anc_hs_stop_charge();

	/* eco off */
	hi6402_reg_clr_bit(pdata->p_irq, HI6402_MICBIAS_ECO_REG, HI6402_MICBIAS_ECO_ON_BIT);

	mutex_lock(&pdata->p_irq->hs_micbias_mutex);
	/* hs micbias off */
	pdata->p_irq->mbhc_micbias_work = false;
	pdata->p_irq->dapm_micbias_work = false;
	mutex_unlock(&pdata->p_irq->hs_micbias_mutex);
	hi6402_irq_hs_micbias_enable(pdata->p_irq, false);

	hi6402_irq_clr_btn_irqs(pdata->p_irq);

	hi6402_irq_mask_btn_irqs(pdata->p_irq);

	/* eco off */
	hi6402_reg_clr_bit(pdata->p_irq, HI6402_MICBIAS_ECO_REG, HI6402_MICBIAS_ECO_ON_BIT);
	pr_info("%s : eco disable \n", __FUNCTION__);
	/* vref off */
	hi6402_reg_set_bit(pdata->p_irq, HI6402_MBHC_VREF_REG, HI6402_MBHC_VREF_BIT);
	/* mbhc cmp off */
	hi6402_reg_set_bit(pdata->p_irq, HI6402_ANA_REG60, HI6402_MBHC_ON_BIT);

	mutex_lock(&pdata->status_mutex);
	pdata->hs_status = HISI_JACK_NONE;
	pdata->btn_report = 0;
	mutex_unlock(&pdata->status_mutex);

	hi6402_jack_report(pdata);

	hi6402_mbhc_enable_ldo8(pdata, false);

	mutex_unlock(&pdata->plug_mutex);

	return IRQ_HANDLED;
}

static irqreturn_t hi6402_btnup_handler(int irq, void *data)
{
	struct hi6402_mbhc_platform_data *pdata =
			(struct hi6402_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	if (!check_headset_pluged_in(pdata))
		return IRQ_HANDLED;

	if (HISI_JACK_INVERT == pdata->hs_status) {
		pr_info("%s: further detect\n", __FUNCTION__);
		/* further detect */
		hi6402_plug_in_detect(pdata);
	} else if (0 == pdata->btn_report) {
		if (HISI_JACK_HEADSET != pdata->hs_status) {
			/* further detect */
			hi6402_plug_in_detect(pdata);
		}
		return IRQ_HANDLED;
	} else {
		mutex_lock(&pdata->status_mutex);
		pdata->btn_report = 0;
		hi6402_soc_jack_report(pdata->btn_report, HI6402_BTN_MASK);
		mutex_unlock(&pdata->status_mutex);
		pr_info("%s(%u) : btn up !\n", __FUNCTION__, __LINE__);
	}

	return IRQ_HANDLED;
}

static irqreturn_t hi6402_btndown_handler(int irq, void *data)
{
	struct hi6402_mbhc_platform_data *pdata =
			(struct hi6402_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	pr_err("%s: btn down \n", __FUNCTION__);

	hi6402_btn_down(pdata);

	return IRQ_HANDLED;
}

static irqreturn_t hi6402_btnup_eco_handler(int irq, void *data)
{
	struct hi6402_mbhc_platform_data *pdata =
			(struct hi6402_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	if (!check_headset_pluged_in(pdata))
		return IRQ_HANDLED;

	wake_lock_timeout(&pdata->wake_lock, 100);

	if (HISI_JACK_INVERT == pdata->hs_status) {
		pr_err("%s: further detect\n", __FUNCTION__);
		/* further detect */
		hi6402_plug_in_detect(pdata);
	} else if (0 == pdata->btn_report){
		if (HISI_JACK_HEADSET != pdata->hs_status) {
			/* further detect */
			hi6402_plug_in_detect(pdata);
		}
		return IRQ_HANDLED;
	} else {
		mutex_lock(&pdata->status_mutex);
		pdata->btn_report = 0;
		hi6402_soc_jack_report(pdata->btn_report, HI6402_BTN_MASK);
		mutex_unlock(&pdata->status_mutex);
		pr_info("%s(%u) : btn up !\n", __FUNCTION__, __LINE__);
	}

	return IRQ_HANDLED;
}

static irqreturn_t hi6402_btndown_eco_handler(int irq, void *data)
{
	struct hi6402_mbhc_platform_data *pdata =
			(struct hi6402_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	pr_err("%s: btn down \n", __FUNCTION__);

	hi6402_btn_down(pdata);

	return IRQ_HANDLED;
}

static irqreturn_t hi6402_pll_unlock_handler(int irq, void *data)
{
	struct hi6402_mbhc_platform_data *pdata =
			(struct hi6402_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	unsigned int i = 0;

	mutex_lock(&pdata->p_irq->pll_mutex);

	if (0 != (hi6402_irq_read(pdata->p_irq, HI6402_PLL_PD_FLAG_REG) & PLL_PD_VALUE) || 0 != (hi6402_irq_read(pdata->p_irq, HI6402_PLL_LOCK_FLAG_REG) & PLL_LOCK_VALUE)){
		pr_info("%s(%u): pll pd, ignore pll unlock irq! \n", __FUNCTION__, __LINE__);
		/* mask pll unlock irq */
		mutex_lock(&pdata->p_irq->irq_lock);
		hi6402_reg_set_bit(pdata->p_irq, HI6402_MASK_IRQ_REG_2, HI6402_MASK_PLL_UNLOCK_BIT);
		pdata->p_irq->mask2 |= 0x10;
		mutex_unlock(&pdata->p_irq->irq_lock);
	} else {
		pr_info("%s(%u): pll unlock irq! \n", __FUNCTION__, __LINE__);

		if (!dsm_client_ocuppy(dsm_audio_client)) {
			dsm_client_record(dsm_audio_client, "DSM_HI6402_PLL_UNLOCK\n");
			dsm_client_notify(dsm_audio_client, DSM_HI6402_PLL_UNLOCK);
		}

		for (i = PLL_UNLOCK_REG_START; i<= PLL_UNLOCK_REG_END; i++) {
			pr_err("%s(%u): %x is %x \n", __FUNCTION__, __LINE__, i, hi6402_irq_read(pdata->p_irq, i));
		}

		pdata->p_irq->pll_status = HI6402_PLL_RST;
		hi6402_irq_set_pll_mode(pdata->p_irq);
	}

	mutex_unlock(&pdata->p_irq->pll_mutex);

	return IRQ_HANDLED;
}

static bool hi6402_check_headset_in(void *pdata)
{
	return check_headset_pluged_in((struct hi6402_mbhc_platform_data *)pdata);
}

static void hi6402_resume_lock(void *pdata, bool lock)
{
	struct hi6402_mbhc_platform_data * p = (struct hi6402_mbhc_platform_data *)pdata;

	if(lock) {
		mutex_lock(&p->p_irq->sr_mutex);
	} else {
		mutex_unlock(&p->p_irq->sr_mutex);
	}
}

static struct anc_dev anc_dev = {
	.name = "anc_hs",
	.ops = {
		.check_headset_in = hi6402_check_headset_in,
		.btn_report = hi6402_soc_jack_report,
		.codec_resume_lock = hi6402_resume_lock,
	},
};

struct input_dev *soundtrigger_input_dev;

static irqreturn_t hi6402_sound_triger_handler(int irq, void *data)
{
	struct hi6402_mbhc_platform_data *pdata =
			(struct hi6402_mbhc_platform_data *)data;
	unsigned int soundtrigger_event = 0;

	BUG_ON(NULL == pdata);

	wake_lock_timeout(&pdata->soundtrigger_wake_lock, 2000);

	/* clr VAD INTR */
	hi6402_irq_write(pdata->p_irq, HI6402_VAD_INT_SET, 0);
	hi6402_irq_write(pdata->p_irq, HI6402_REG_IRQ_1, 1<<HI6402_VAD_BIT);

	soundtrigger_event = hi6402_irq_read(pdata->p_irq, HI6402_DSP_VAD_CMD);

	pr_info("%s:----sound trigger----%d\n", __FUNCTION__, soundtrigger_event);

	if(soundtrigger_event < ST_EVENT_MAX) {
		input_report_key(soundtrigger_input_dev,
			SOUNDTRIGGER_EVENT_BASE + soundtrigger_event, 1);
		input_sync(soundtrigger_input_dev);

		input_report_key(soundtrigger_input_dev,
			SOUNDTRIGGER_EVENT_BASE + soundtrigger_event, 0);
		input_sync(soundtrigger_input_dev);
	}

	return IRQ_HANDLED;
}

static int soundtrigger_input_init(struct device *dev)
{
	int error = 0;

	soundtrigger_input_dev = input_allocate_device();
	if (!soundtrigger_input_dev) {
		pr_err("failed to allocate memory for input dev\n");
		error = -ENOMEM;
		goto err_dev;
	}

	soundtrigger_input_dev->name = "soundtrigger_input_dev";

	soundtrigger_input_dev->evbit[0] = BIT_MASK(EV_KEY);
	soundtrigger_input_dev->keybit[BIT_WORD(KEY_F14)] |= BIT_MASK(KEY_F14);
	soundtrigger_input_dev->keybit[BIT_WORD(KEY_F15)] |= BIT_MASK(KEY_F15);
	soundtrigger_input_dev->keybit[BIT_WORD(KEY_F16)] |= BIT_MASK(KEY_F16);
	soundtrigger_input_dev->keybit[BIT_WORD(KEY_F17)] |= BIT_MASK(KEY_F17);
	soundtrigger_input_dev->keybit[BIT_WORD(KEY_F18)] |= BIT_MASK(KEY_F18);
	error = input_register_device(soundtrigger_input_dev);
	if(error < 0) {
		pr_err("input register device failed, error_no is %d.\n", error);
		goto err_free_dev;
	}
	pr_info("input register device successful.\n");
	return 0;

err_free_dev:
	input_free_device(soundtrigger_input_dev);

err_dev:
	pr_err("error_no is %d.\n", error);

	return error;
}

static int hi6402_mbhc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hi6402_mbhc_platform_data *pdata = NULL;
	const struct of_device_id *match = NULL;
	int ret = 0;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (NULL == pdata) {
		dev_err(dev, "cannot allocate hisi 6421 spk platform data\n");
		return -ENOMEM;
	}

	match = of_match_device(hi6402_mbhc_of_match, dev);
	if (!match) {
		pr_err("get device info err\n");
		return -ENOENT;
	} else {
		struct device_node *node = dev->of_node;
		int temp;

		/* get board defination */
		if (!of_property_read_u32(node, "hisilicon,hi6402_hs_det_inv", &temp))
			pdata->hs_det_inv = temp;
		else
			pdata->hs_det_inv = 0;

		if (!of_property_read_u32(node, "hisilicon,hs_3_pole_min_voltage", &temp))
			pdata->hs_3_pole_min_voltage = temp;
		else
			pdata->hs_3_pole_min_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,hs_3_pole_max_voltage", &temp))
			pdata->hs_3_pole_max_voltage = temp;
		else
			pdata->hs_3_pole_max_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,hs_4_pole_min_voltage", &temp))
			pdata->hs_4_pole_min_voltage = temp;
		else
			pdata->hs_4_pole_min_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,hs_4_pole_max_voltage", &temp))
			pdata->hs_4_pole_max_voltage = temp;
		else
			pdata->hs_4_pole_max_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,btn_play_min_voltage", &temp))
			pdata->btn_play_min_voltage = temp;
		else
			pdata->btn_play_min_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,btn_play_max_voltage", &temp))
			pdata->btn_play_max_voltage = temp;
		else
			pdata->btn_play_max_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,btn_volume_up_min_voltage", &temp))
			pdata->btn_volume_up_min_voltage = temp;
		else
			pdata->btn_volume_up_min_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,btn_volume_up_max_voltage", &temp))
			pdata->btn_volume_up_max_voltage = temp;
		else
			pdata->btn_volume_up_max_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,btn_volume_down_min_voltage", &temp))
			pdata->btn_volume_down_min_voltage = temp;
		else
			pdata->btn_volume_down_min_voltage = 0;

		if (!of_property_read_u32(node, "hisilicon,btn_volume_down_max_voltage", &temp))
			pdata->btn_volume_down_max_voltage = temp;
		else
			pdata->btn_volume_down_max_voltage = 0;
	}

	pdata->p_irq = dev_get_drvdata(pdev->dev.parent);
	if (!pdata->p_irq) {
		dev_err(dev, "get parend device error\n");
		return -ENOENT;
	}

#ifdef CONFIG_SWITCH
	pdata->sdev.name = "h2w";
	ret = switch_dev_register(&pdata->sdev);
	if (ret) {
		pr_err("%s : error registering switch device %d\n", __FUNCTION__, ret);
		return ret;
	}
#endif

	/* get irqs */
	pdata->irq[HI6402_IRQ_PLL_UNLOCK] = platform_get_irq_byname(pdev, "pll_unlock");
	if (0 > pdata->irq[HI6402_IRQ_PLL_UNLOCK]) {
		pr_err("get pll unlock error");
		return -ENOENT;
	}

	pdata->irq[HI6402_IRQ_SOUND_TRIGER] = platform_get_irq_byname(pdev, "sound_triger");
	if (0 > pdata->irq[HI6402_IRQ_SOUND_TRIGER]) {
		pr_err("get sound triger error");
		return -ENOENT;
	}

	pdata->irq[HI6402_IRQ_PLUGOUT] = platform_get_irq_byname(pdev, "plugout");
	if (0 > pdata->irq[HI6402_IRQ_PLUGOUT]) {
		pr_err("get plug out irq num error");
		return -ENOENT;
	}

	pdata->irq[HI6402_IRQ_PLUGIN] = platform_get_irq_byname(pdev, "plugin");
	if (0 > pdata->irq[HI6402_IRQ_PLUGIN]) {
		pr_err("get plug in irq num error");
		return -ENOENT;
	}

	pdata->irq[HI6402_IRQ_BTNDOWN_ECO] = platform_get_irq_byname(pdev, "btndown_eco");
	if (0 > pdata->irq[HI6402_IRQ_BTNDOWN_ECO]) {
		pr_err("get btn down eco irq num error");
		return -ENOENT;
	}

	pdata->irq[HI6402_IRQ_BTNUP_ECO] = platform_get_irq_byname(pdev, "btnup_eco");
	if (0 > pdata->irq[HI6402_IRQ_BTNUP_ECO]) {
		pr_err("get btn up eco irq num error");
		return -ENOENT;
	}

	pdata->irq[HI6402_IRQ_BTNDOWN_COMP1] = platform_get_irq_byname(pdev, "btndown");
	if (0 > pdata->irq[HI6402_IRQ_BTNDOWN_COMP1]) {
		pr_err("get btn down irq num error");
		return -ENOENT;
	}

	pdata->irq[HI6402_IRQ_BTNUP_COMP1] = platform_get_irq_byname(pdev, "btnup");
	if (0 > pdata->irq[HI6402_IRQ_BTNUP_COMP1]) {
		pr_err("get btn up irq num error");
		return -ENOENT;
	}

	wake_lock_init(&pdata->wake_lock, WAKE_LOCK_SUSPEND, "hisi-6402-mbhc");
	wake_lock_init(&pdata->soundtrigger_wake_lock, WAKE_LOCK_SUSPEND, "hisi-6402-soundtrigger");
	mutex_init(&pdata->plug_mutex);
	mutex_init(&pdata->status_mutex);
	mutex_init(&pdata->saradc_mutex);

	/* irq request : pll unlock */
	ret = devm_request_threaded_irq(dev, pdata->irq[HI6402_IRQ_PLL_UNLOCK], NULL,
					hi6402_pll_unlock_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"pll_unlock", pdata);
	if (0 > ret) {
		pr_err("request irq for pll unlock err\n");
		goto pll_unlock_err;
	}
	mutex_lock(&pdata->p_irq->irq_lock);
	hi6402_reg_set_bit(pdata->p_irq, HI6402_MASK_IRQ_REG_2, HI6402_MASK_PLL_UNLOCK_BIT);
	pdata->p_irq->mask2 |= 0x10;
	mutex_unlock(&pdata->p_irq->irq_lock);

	/* irq request : sound triger */
	ret = devm_request_threaded_irq(dev, pdata->irq[HI6402_IRQ_SOUND_TRIGER], NULL,
					hi6402_sound_triger_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"sound_triger", pdata);
	if (0 > ret) {
		pr_err("request irq for sound triger err\n");
		goto sound_triger_err;
	}

	/* irq request : plugout */
	ret = devm_request_threaded_irq(dev, pdata->irq[HI6402_IRQ_PLUGOUT], NULL,
					hi6402_plugout_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"plugout", pdata);
	if (0 > ret) {
		pr_err("request irq for plugout err\n");
		goto plugout_err;
	}

	/* irq request : plugin */
	ret = devm_request_threaded_irq(dev, pdata->irq[HI6402_IRQ_PLUGIN], NULL,
					hi6402_plugin_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"plugin", pdata);
	if (0 > ret) {
		pr_err("request irq for plugin err\n");
		goto plugin_err;
	}

	/* irq request : button up(eco mode) */
	ret = devm_request_threaded_irq(dev, pdata->irq[HI6402_IRQ_BTNUP_ECO], NULL,
					hi6402_btnup_eco_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"btnup_eco", pdata);
	if (0 > ret) {
		pr_err("request irq for btnup eco err\n");
		goto btnup_eco_err;
	}

	/* irq request : button down(eco mode) */
	ret = devm_request_threaded_irq(dev, pdata->irq[HI6402_IRQ_BTNDOWN_ECO], NULL,
					hi6402_btndown_eco_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"btndown_eco", pdata);
	if (0 > ret) {
		pr_err("request irq for btndown eco err\n");
		goto btndown_eco_err;
	}

	/* irq request : button down */
	ret = devm_request_threaded_irq(dev, pdata->irq[HI6402_IRQ_BTNDOWN_COMP1], NULL,
					hi6402_btndown_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"btndown_comp1", pdata);
	if (0 > ret) {
		pr_err("request irq for btndown comp1 err\n");
		goto btndown_comp1_err;
	}

	/* irq request : button up */
	ret = devm_request_threaded_irq(dev, pdata->irq[HI6402_IRQ_BTNUP_COMP1], NULL,
					hi6402_btnup_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"btnup_comp1", pdata);
	if (0 > ret) {
		pr_err("request irq for btnup comp1 err\n");
		goto btnup_comp1_err;
	}

	hi6402_irq_mask_btn_irqs(pdata->p_irq);
	hi6402_irq_clr_btn_irqs(pdata->p_irq);
	
	/* enable hsdet */
	hi6402_irq_write(pdata->p_irq, HI6402_REG_HSDET_CTRL, 0x19);
	hi6402_irq_write(pdata->p_irq, HI6402_MBHC_VREF_REG, 0x8E);

	//register anc hs first
	anc_hs_dev_register(&anc_dev, pdata);

	//register soundtrigger input device.
	ret = soundtrigger_input_init(dev);
	if(ret)
		pr_err("input registor failed: %d\n", ret);

#if 0
	/* check jack at first time */
	if (check_headset_pluged_in(pdata))
		hi6402_plug_in_detect(pdata);
#endif

	pdata->miscdev.minor = MISC_DYNAMIC_MINOR;
	pdata->miscdev.name = "hi6402_mbhc";

	ret = misc_register(&pdata->miscdev);
	if (ret) {
		loge("%s : hisi 6421 spk_device register failed", __FUNCTION__);
		goto btnup_comp1_err;
	}

	if (!dsm_audio_client) {
		dsm_audio_client = dsm_register_client(&dsm_audio);
	}

	pr_info("%s : hi6402 mbhc probe ok \n", __FUNCTION__);

	return ret;

btnup_comp1_err:
	free_irq(pdata->irq[HI6402_IRQ_BTNDOWN_COMP1], pdata);
btndown_comp1_err:
	free_irq(pdata->irq[HI6402_IRQ_BTNDOWN_ECO], pdata);
btndown_eco_err:
	free_irq(pdata->irq[HI6402_IRQ_BTNUP_ECO], pdata);
btnup_eco_err:
	free_irq(pdata->irq[HI6402_IRQ_PLUGIN], pdata);
plugin_err:
	free_irq(pdata->irq[HI6402_IRQ_PLUGOUT], pdata);
plugout_err:
	free_irq(pdata->irq[HI6402_IRQ_SOUND_TRIGER], pdata);
sound_triger_err:
	free_irq(pdata->irq[HI6402_IRQ_PLL_UNLOCK], pdata);
pll_unlock_err:
	wake_lock_destroy(&pdata->soundtrigger_wake_lock);
	wake_lock_destroy(&pdata->wake_lock);
	mutex_destroy(&pdata->plug_mutex);
	mutex_destroy(&pdata->status_mutex);

	return ret;
}

static int hi6402_mbhc_remove(struct platform_device *pdev)
{
	struct hi6402_mbhc_platform_data *pdata = platform_get_drvdata(pdev);

	wake_lock_destroy(&pdata->soundtrigger_wake_lock);
	wake_lock_destroy(&pdata->wake_lock);
	mutex_destroy(&pdata->plug_mutex);
	mutex_destroy(&pdata->status_mutex);
	input_unregister_device(soundtrigger_input_dev);

	return 0;
}

static struct platform_driver hi6402_mbhc_driver = {
	.driver = {
		.name	= "hi6402_mbhc",
		.owner	= THIS_MODULE,
		.of_match_table = hi6402_mbhc_of_match,
	},
	.probe	= hi6402_mbhc_probe,
	.remove	= hi6402_mbhc_remove,
};

static int __init hi6402_mbhc_init(void)
{
	return platform_driver_register(&hi6402_mbhc_driver);
}

static void __exit hi6402_mbhc_exit(void)
{
	platform_driver_unregister(&hi6402_mbhc_driver);
}

late_initcall(hi6402_mbhc_init);
module_exit(hi6402_mbhc_exit);

MODULE_DESCRIPTION("hi6402_mbhc driver");
MODULE_LICENSE("GPL");
