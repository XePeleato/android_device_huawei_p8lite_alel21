/*
 * hi6402es_mbhc.c -- hi6402es mbhc driver
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

#include "hi6402es_mbhc.h"
#include "huawei_platform/audio/anc_hs.h"

#define LOG_TAG "HI6402ES_MBHC"

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

#define HI6402ES_CFG_BASE_ADDR		0x20007000

#define HI6402ES_REG_IRQ_0		(HI6402ES_CFG_BASE_ADDR + 0x14)
#define HI6402ES_HS_PLUGIN_BIT			6

#define HI6402ES_REG_IRQ_1		(HI6402ES_CFG_BASE_ADDR + 0x15)
#define HI6402ES_VAD_BIT					4
#define HI6402ES_REG_HSDET_CTRL	(HI6402ES_CFG_BASE_ADDR + 0xDE)

#define HI6402ES_MBHC_VREF_REG	(HI6402ES_CFG_BASE_ADDR + 0xE0)

#define HI6402ES_MICBIAS_ECO_REG	(HI6402ES_CFG_BASE_ADDR + 0xE1)

#define HI6402ES_VAD_INT_SET			0x20003000

static const struct of_device_id hi6402es_mbhc_of_match[] = {
	{
		.compatible = "hisilicon,hi6402es-mbhc",
	},
	{ },
};
MODULE_DEVICE_TABLE(of, hi6402es_mbhc_of_match);

#define HI6402ES_BTN_MASK	(SND_JACK_BTN_0 | SND_JACK_BTN_1 | SND_JACK_BTN_2)

extern void hi6402es_soc_jack_report(int report, int mask);

void hi6402es_jack_report(struct hi6402es_mbhc_platform_data *pdata)
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
	/* report invert 4-pole headset as 3-pole headphone */
		jack_status = HISI_JACK_HEADPHONE;
	/* force through */
	case HISI_JACK_HEADPHONE:
		jack_report = SND_JACK_HEADPHONE;
		pr_info("%s : 3-pole headphone plug in\n", __FUNCTION__);
		break;
	default:
		pr_err("%s : error hs_status(%d)\n", __FUNCTION__, pdata->hs_status);
		break;
	}

	/* clear btn event */
	hi6402es_soc_jack_report(0, HI6402ES_BTN_MASK);
	/* btn_report jack status */
	hi6402es_soc_jack_report(jack_report, SND_JACK_HEADSET);
#ifdef CONFIG_SWITCH
	switch_set_state(&pdata->sdev, jack_status);
#endif
}

static inline bool check_headset_pluged_in(struct hi6402es_mbhc_platform_data *pdata)
{
	/*
	 * 0 : means headset is pluged out
	 * 1 : means headset is pluged in
	 */
	hi6402es_irq_write(pdata->p_irq, HI6402ES_REG_IRQ_0, 0x40);
	mdelay(2);
	return (0 != (hi6402es_irq_read(pdata->p_irq, HI6402ES_REG_IRQ_0) & (1 << HI6402ES_HS_PLUGIN_BIT)));
}

static void mask_irq(int id, bool mask)
{
	struct irq_desc *irq_desc = NULL;

	irq_desc = irq_to_desc(id);
	if (irq_desc) {
		if (mask)
			irq_desc->irq_data.chip->irq_mask(&irq_desc->irq_data);
		else
			irq_desc->irq_data.chip->irq_unmask(&irq_desc->irq_data);
	} else {
		pr_warn("%s : irq to desc return NULL\n", __FUNCTION__);
	}
}

void hi6402es_plug_in_detect(struct hi6402es_mbhc_platform_data *pdata)
{
	if (!check_headset_pluged_in(pdata))
		return;

	wake_lock(&pdata->wake_lock);
	mutex_lock(&pdata->plug_mutex);

	anc_hs_start_charge();

	mutex_lock(&pdata->status_mutex);
	/* todo : btn_report 4-pole headset only now */
	pdata->hs_status = HISI_JACK_HEADSET;
	pdata->btn_report = SND_JACK_HEADSET;
	mutex_unlock(&pdata->status_mutex);

	anc_hs_charge_detect(100, ANC_HS_4POLE);

	hi6402es_jack_report(pdata);

	/* todo */
	hi6402es_reg_clr_bit(pdata->p_irq, HI6402ES_MBHC_VREF_REG, 7);
	hi6402es_reg_set_bit(pdata->p_irq, HI6402ES_MICBIAS_ECO_REG, 0);

	/* unmask btn down irq */
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], false);
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO], false);

	mutex_unlock(&pdata->plug_mutex);
	wake_unlock(&pdata->wake_lock);
}

void hi6402es_plug_in_workfunc(struct work_struct *work)
{
	struct hi6402es_mbhc_platform_data *pdata = container_of(work,
			struct hi6402es_mbhc_platform_data,
			headset_plug_in_delay_work.work);

	BUG_ON(NULL == pdata);

	wake_lock(&pdata->wake_lock);
	hi6402es_plug_in_detect(pdata);
	wake_unlock(&pdata->wake_lock);
}

void hi6402es_plug_out_workfunc(struct work_struct *work)
{
	struct hi6402es_mbhc_platform_data *pdata = container_of(work,
			struct hi6402es_mbhc_platform_data,
			headset_plug_out_delay_work.work);

	pr_info("%s : hs pluged out\n", __FUNCTION__);

	BUG_ON(NULL == pdata);

	wake_lock(&pdata->wake_lock);
	mutex_lock(&pdata->plug_mutex);

	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], true);
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_COMP1], true);
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO], true);
	/* todo */
	hi6402es_reg_clr_bit(pdata->p_irq, HI6402ES_MICBIAS_ECO_REG, 0);
	hi6402es_reg_set_bit(pdata->p_irq, HI6402ES_MBHC_VREF_REG, 7);

	mutex_lock(&pdata->status_mutex);
	pdata->hs_status = HISI_JACK_NONE;
	pdata->btn_report = 0;
	mutex_unlock(&pdata->status_mutex);

	anc_hs_stop_charge();

	hi6402es_jack_report(pdata);

	mutex_unlock(&pdata->plug_mutex);
	wake_unlock(&pdata->wake_lock);

	/* unmask plugin interrupt */
	mask_irq(pdata->irq[HI6402ES_IRQ_PLUGIN], false);
}

void hi6402es_btnup_workfunc(struct work_struct *work)
{
	struct hi6402es_mbhc_platform_data *pdata = container_of(work,
			struct hi6402es_mbhc_platform_data,
			headset_btn_up_delay_work.work);

	BUG_ON(NULL == pdata);

	pr_info("%s(%u) : btn up !\n", __FUNCTION__, __LINE__);

	pdata->btn_report = 0;

	hi6402es_soc_jack_report(pdata->btn_report, HI6402ES_BTN_MASK);

	/* mask btn up interrupt*/
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_COMP1], true);
	/* unmask btn down interrupt*/
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], false);

	return;
}

void hi6402es_sound_triger_workfunc(struct work_struct *work)
{
	struct hi6402es_mbhc_platform_data *pdata = container_of(work,
			struct hi6402es_mbhc_platform_data,
			headset_sound_triger_delay_work.work);

	BUG_ON(NULL == pdata);

	wake_lock(&pdata->wake_lock);

	/* clr VAD INTR */
	hi6402es_irq_write(pdata->p_irq, HI6402ES_VAD_INT_SET, 0);
	hi6402es_irq_write(pdata->p_irq, HI6402ES_REG_IRQ_1, 1<<HI6402ES_VAD_BIT);

	mask_irq(pdata->irq[HI6402ES_IRQ_SOUND_TRIGER], false);

	hi6402es_soc_jack_report(SND_JACK_BTN_5, SND_JACK_BTN_5);

	hi6402es_soc_jack_report(0, SND_JACK_BTN_5);

	pr_info("%s(%u): sound_triger = 0x%x\n",
			__FUNCTION__, __LINE__, SND_JACK_BTN_5);

	wake_unlock(&pdata->wake_lock);

	return;
}

void hi6402es_btndown_workfunc(struct work_struct *work)
{
	int saradc_value = 0;
	struct hi6402es_mbhc_platform_data *pdata = container_of(work,
			struct hi6402es_mbhc_platform_data,
			headset_btn_down_delay_work.work);

	BUG_ON(NULL == pdata);

	if (!check_headset_pluged_in(pdata)) {
		pr_info("%s(%u) : hs pluged out \n", __FUNCTION__, __LINE__);
		return;
	}

	pr_info("%s(%u) : btn down !\n", __FUNCTION__, __LINE__);

	wake_lock(&pdata->wake_lock);

	if (HISI_JACK_HEADSET == pdata->hs_status) {
		/* power on hs-micbias */
		//hi6401_hs_micbias_saradc_enable(codec, true);
		//saradc_value = hi6401_read_saradc_value(codec);
		//pr_info("%s(%u) :saradc_value: %d \n", __FUNCTION__, __LINE__, saradc_value);
		//hi6401_hs_micbias_saradc_enable(codec, false);

		if (!check_headset_pluged_in(pdata)) {
			pr_info("%s(%u) : hs pluged out \n", __FUNCTION__, __LINE__);

			/* mask all btn interrupt*/
			mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_COMP1], true);
			mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], true);
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
			hi6402es_plug_in_detect(pdata);
			goto end;
		}

		if (!check_headset_pluged_in(pdata)) {
			pr_info("%s(%u) : hs pluged out \n", __FUNCTION__, __LINE__);

			/* mask all btn interrupt*/
			mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_COMP1], true);
			mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], true);
			goto end;
		}

		/*btn_report key event*/
		pr_info("%s(%u): btn_report type = 0x%x, status=0x%x\n",
				__FUNCTION__, __LINE__, pdata->btn_report, pdata->hs_status);
		hi6402es_soc_jack_report(pdata->btn_report, HI6402ES_BTN_MASK);
	}

	/* mask btn down interrupt*/
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], true);
	/* unmask btn up interrupt*/
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_COMP1], false);
end:
	wake_unlock(&pdata->wake_lock);

	return;
}

static irqreturn_t hi6402es_plugin_handler(int irq, void *data)
{
	struct hi6402es_mbhc_platform_data *pdata =
			(struct hi6402es_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	/* mask plugin interrupt*/
	mask_irq(pdata->irq[HI6402ES_IRQ_PLUGIN], true);

	wake_lock_timeout(&pdata->wake_lock, 900);

	queue_delayed_work(pdata->headset_plug_in_delay_wq,
			&pdata->headset_plug_in_delay_work,
			msecs_to_jiffies(800));

	return IRQ_HANDLED;
}

static irqreturn_t hi6402es_plugout_handler(int irq, void *data)
{
	struct hi6402es_mbhc_platform_data *pdata =
			(struct hi6402es_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	wake_lock_timeout(&pdata->wake_lock, 30);

	queue_delayed_work(pdata->headset_plug_out_delay_wq,
			&pdata->headset_plug_out_delay_work,
			msecs_to_jiffies(0));

	return IRQ_HANDLED;
}

static irqreturn_t hi6402es_btnup_handler(int irq, void *data)
{
	struct hi6402es_mbhc_platform_data *pdata =
			(struct hi6402es_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	pr_err("----cg---- %s\n", __FUNCTION__);

	if (!check_headset_pluged_in(pdata))
		return IRQ_HANDLED;

	wake_lock_timeout(&pdata->wake_lock, 100);

	if (HISI_JACK_INVERT == pdata->hs_status) {
		pr_info("%s: further detect\n", __FUNCTION__);
		/* further detect */
		queue_delayed_work(pdata->headset_plug_in_delay_wq,
				&pdata->headset_plug_in_delay_work,
				msecs_to_jiffies(50));
	} else if (0 == pdata->btn_report){
		return IRQ_HANDLED;
	} else {
		queue_delayed_work(pdata->headset_btn_up_delay_wq,
				&pdata->headset_btn_up_delay_work,
				msecs_to_jiffies(60));
		pdata->btn_report = 0;
	}

	return IRQ_HANDLED;
}

static irqreturn_t hi6402es_btndown_handler(int irq, void *data)
{
	struct hi6402es_mbhc_platform_data *pdata =
			(struct hi6402es_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	pr_err("----cg---- %s\n", __FUNCTION__);

	if (!check_headset_pluged_in(pdata))
		return IRQ_HANDLED;

	/* mask btn down interrupt*/
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], true);
	/* unmask btn up interrupt*/
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_COMP1], false);

	wake_lock_timeout(&pdata->wake_lock, 50);

	queue_delayed_work(pdata->headset_btn_down_delay_wq,
				&pdata->headset_btn_down_delay_work,
				msecs_to_jiffies(30));

	return IRQ_HANDLED;
}

static irqreturn_t hi6402es_btnup_eco_handler(int irq, void *data)
{
	struct hi6402es_mbhc_platform_data *pdata =
			(struct hi6402es_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	pr_err("----cg---- %s\n", __FUNCTION__);

	mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_ECO], true);
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO], false);

	if (!check_headset_pluged_in(pdata))
		return IRQ_HANDLED;

	wake_lock_timeout(&pdata->wake_lock, 100);

	if (HISI_JACK_INVERT == pdata->hs_status) {
		pr_err("%s: further detect\n", __FUNCTION__);
		/* further detect */
		queue_delayed_work(pdata->headset_plug_in_delay_wq,
				&pdata->headset_plug_in_delay_work,
				msecs_to_jiffies(50));
	} else if (0 == pdata->btn_report){
		return IRQ_HANDLED;
	} else {
		queue_delayed_work(pdata->headset_btn_up_delay_wq,
				&pdata->headset_btn_up_delay_work,
				msecs_to_jiffies(50));
	}

	return IRQ_HANDLED;
}

static irqreturn_t hi6402es_btndown_eco_handler(int irq, void *data)
{
	struct hi6402es_mbhc_platform_data *pdata =
			(struct hi6402es_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);

	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO], true);
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_ECO], false);

	pr_err("----cg---- %s\n", __FUNCTION__);

	if (!check_headset_pluged_in(pdata))
		return IRQ_HANDLED;

	wake_lock_timeout(&pdata->wake_lock, 50);

	queue_delayed_work(pdata->headset_btn_down_delay_wq,
				&pdata->headset_btn_down_delay_work,
				msecs_to_jiffies(20));

	return IRQ_HANDLED;
}

static irqreturn_t hi6402es_sound_triger_handler(int irq, void *data)
{
	struct hi6402es_mbhc_platform_data *pdata =
			(struct hi6402es_mbhc_platform_data *)data;

	BUG_ON(NULL == pdata);
	/* mask sound_triger interrupt */
	mask_irq(pdata->irq[HI6402ES_IRQ_SOUND_TRIGER], true);

	pr_info("----sound triger---- %s\n", __FUNCTION__);

	wake_lock_timeout(&pdata->wake_lock, 100);

	queue_delayed_work(pdata->headset_sound_triger_delay_wq,
				&pdata->headset_sound_triger_delay_work,
				msecs_to_jiffies(0));

	return IRQ_HANDLED;
}

static bool hi6402_check_headset_in(void *pdata)
{
	return check_headset_pluged_in((struct hi6402es_mbhc_platform_data *)pdata);
}

static void hi6402_resume_lock(void *pdata, bool lock)
{
	struct hi6402es_mbhc_platform_data * p = (struct hi6402es_mbhc_platform_data *)pdata;

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
		.btn_report = hi6402es_soc_jack_report,
		.codec_resume_lock = hi6402_resume_lock,
	},
};

static int hi6402es_mbhc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hi6402es_mbhc_platform_data *pdata = NULL;
	const struct of_device_id *match = NULL;
	int ret = 0;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (NULL == pdata){
		dev_err(dev, "cannot allocate hisi 6421 spk platform data\n");
		return -ENOMEM;
	}

	match = of_match_device(hi6402es_mbhc_of_match, dev);
	if (!match) {
		pr_err("get device info err\n");
		return -ENOENT;
	} else {
		struct device_node *node = dev->of_node;
		int temp;

		/* get board defination */
		if (!of_property_read_u32(node, "hisilicon,hi6402es_hs_det_inv", &temp))
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

	wake_lock_init(&pdata->wake_lock, WAKE_LOCK_SUSPEND, "hisi-6402-mbhc");
	mutex_init(&pdata->plug_mutex);
	mutex_init(&pdata->status_mutex);

	/* get irqs */
	pdata->irq[HI6402ES_IRQ_SOUND_TRIGER] = platform_get_irq_byname(pdev, "sound_triger");
	if (0 > pdata->irq[HI6402ES_IRQ_SOUND_TRIGER])
		return -ENOENT;

	pdata->irq[HI6402ES_IRQ_PLUGOUT] = platform_get_irq_byname(pdev, "plugout");
	if (0 > pdata->irq[HI6402ES_IRQ_PLUGOUT])
		return -ENOENT;

	pdata->irq[HI6402ES_IRQ_PLUGIN] = platform_get_irq_byname(pdev, "plugin");
	if (0 > pdata->irq[HI6402ES_IRQ_PLUGIN])
		return -ENOENT;

	pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO] = platform_get_irq_byname(pdev, "btndown_eco");
	if (0 > pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO])
		return -ENOENT;

	pdata->irq[HI6402ES_IRQ_BTNUP_ECO] = platform_get_irq_byname(pdev, "btnup_eco");
	if (0 > pdata->irq[HI6402ES_IRQ_BTNUP_ECO])
		return -ENOENT;

	pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1] = platform_get_irq_byname(pdev, "btndown");
	if (0 > pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1])
		return -ENOENT;

	pdata->irq[HI6402ES_IRQ_BTNUP_COMP1] = platform_get_irq_byname(pdev, "btnup");
	if (0 > pdata->irq[HI6402ES_IRQ_BTNUP_COMP1])
		return -ENOENT;


	pdata->headset_plug_in_delay_wq =
			create_singlethread_workqueue("headset_plug_in_delay_wq");
	if (!(pdata->headset_plug_in_delay_wq)) {
		pr_err("%s : headset_plug_in_delay_wq create failed", __FUNCTION__);
		ret = -ENOMEM;
		goto headset_plug_in_wq_err;
	}
	INIT_DELAYED_WORK(&pdata->headset_plug_in_delay_work, hi6402es_plug_in_workfunc);

	pdata->headset_plug_out_delay_wq =
			create_singlethread_workqueue("headset_plug_out_delay_wq");
	if (!(pdata->headset_plug_out_delay_wq)) {
		pr_err("%s : headset_plug_out_delay_wq create failed", __FUNCTION__);
		ret = -ENOMEM;
		goto headset_plug_out_wq_err;
	}
	INIT_DELAYED_WORK(&pdata->headset_plug_out_delay_work, hi6402es_plug_out_workfunc);

	pdata->headset_btn_up_delay_wq =
			create_singlethread_workqueue("headset_btn_up_delay_wq");
	if (!(pdata->headset_btn_up_delay_wq)) {
		pr_err("%s : headset_btn_up_delay_wq create failed", __FUNCTION__);
		ret = -ENOMEM;
		goto headset_btn_up_wq_err;
	}
	INIT_DELAYED_WORK(&pdata->headset_btn_up_delay_work, hi6402es_btnup_workfunc);

	pdata->headset_btn_down_delay_wq =
			create_singlethread_workqueue("headset_btn_down_delay_wq");
	if (!(pdata->headset_btn_down_delay_wq)) {
		pr_err("%s : headset_btn_down_delay_wq create failed", __FUNCTION__);
		ret = -ENOMEM;
		goto headset_btn_down_wq_err;
	}
	INIT_DELAYED_WORK(&pdata->headset_btn_down_delay_work, hi6402es_btndown_workfunc);

	pdata->headset_sound_triger_delay_wq =
			create_singlethread_workqueue("headset_sound_triger_delay_wq");
	if (!(pdata->headset_sound_triger_delay_wq)) {
		pr_err("%s : headset_sound_triger_delay_wq create failed", __FUNCTION__);
		ret = -ENOMEM;
		goto headset_sound_triger_wq_err;
	}
	INIT_DELAYED_WORK(&pdata->headset_sound_triger_delay_work, hi6402es_sound_triger_workfunc);

	/* irq request : sound triger */
	ret = request_irq(pdata->irq[HI6402ES_IRQ_SOUND_TRIGER],
			  hi6402es_sound_triger_handler,
			  IRQF_DISABLED | IRQF_NO_SUSPEND, "sound_triger", pdata);
	if (0 > ret)
		goto sound_triger_err;

	/* irq request : plugout */
	ret = request_irq(pdata->irq[HI6402ES_IRQ_PLUGOUT],
			  hi6402es_plugout_handler,
			  IRQF_DISABLED | IRQF_NO_SUSPEND, "plugout", pdata);
	if (0 > ret)
		goto plugout_err;

	/* irq request : plugin */
	ret = request_irq(pdata->irq[HI6402ES_IRQ_PLUGIN],
			  hi6402es_plugin_handler,
			  IRQF_DISABLED | IRQF_NO_SUSPEND, "plugin", pdata);
	if (0 > ret)
		goto plugin_err;

	/* irq request : button up(eco mode) */
	ret = request_irq(pdata->irq[HI6402ES_IRQ_BTNUP_ECO],
			  hi6402es_btnup_eco_handler,
			  IRQF_DISABLED | IRQF_NO_SUSPEND, "btnup_eco", pdata);
	if (0 > ret)
		goto btnup_eco_err;

	/* irq request : button down(eco mode) */
	ret = request_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO],
			  hi6402es_btndown_eco_handler,
			  IRQF_DISABLED | IRQF_NO_SUSPEND, "btndown_eco", pdata);
	if (0 > ret)
		goto btndown_eco_err;

	/* irq request : button down */
	ret = request_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1],
			  hi6402es_btndown_handler,
			  IRQF_DISABLED | IRQF_NO_SUSPEND, "btndown_comp1", pdata);
	if (0 > ret)
		goto btndown_comp1_err;

	/* irq request : button up */
	ret = request_irq(pdata->irq[HI6402ES_IRQ_BTNUP_COMP1],
			  hi6402es_btnup_handler,
			  IRQF_DISABLED | IRQF_NO_SUSPEND, "btnup_comp1", pdata);
	if (0 > ret)
		goto btnup_comp1_err;

	/* mask all btn interrupt*/
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO], true);
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_ECO], true);
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], true);
	mask_irq(pdata->irq[HI6402ES_IRQ_BTNUP_COMP1], true);

	//register anc hs first
	anc_hs_dev_register(&anc_dev, pdata);

#if 0 /* todo in cs version */
	/* check jack at first time */
	if (check_headset_pluged_in(pdata))
		hi6402es_plug_in_detect(pdata);
#endif

	/* enable hsdet */
	hi6402es_irq_write(pdata->p_irq, HI6402ES_REG_HSDET_CTRL, 0x1);
	hi6402es_irq_write(pdata->p_irq, HI6402ES_MBHC_VREF_REG, 0x8c);

	pdata->miscdev.minor = MISC_DYNAMIC_MINOR;
	pdata->miscdev.name = "hi6402es_mbhc";

	ret = misc_register(&pdata->miscdev);
	if (ret)
		loge("%s : hisi 6421 spk_device register failed", __FUNCTION__);

	return ret;

btnup_comp1_err:
	free_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_COMP1], pdata);
btndown_comp1_err:
	free_irq(pdata->irq[HI6402ES_IRQ_BTNDOWN_ECO], pdata);
btndown_eco_err:
	free_irq(pdata->irq[HI6402ES_IRQ_BTNUP_ECO], pdata);
btnup_eco_err:
	free_irq(pdata->irq[HI6402ES_IRQ_PLUGIN], pdata);
plugin_err:
	free_irq(pdata->irq[HI6402ES_IRQ_PLUGOUT], pdata);
plugout_err:
	free_irq(pdata->irq[HI6402ES_IRQ_SOUND_TRIGER], pdata);
sound_triger_err:
	if(pdata->headset_sound_triger_delay_wq) {
		cancel_delayed_work(&pdata->headset_sound_triger_delay_work);
		flush_workqueue(pdata->headset_sound_triger_delay_wq);
		destroy_workqueue(pdata->headset_sound_triger_delay_wq);
	}
headset_sound_triger_wq_err:
	if(pdata->headset_btn_down_delay_wq) {
		cancel_delayed_work(&pdata->headset_btn_down_delay_work);
		flush_workqueue(pdata->headset_btn_down_delay_wq);
		destroy_workqueue(pdata->headset_btn_down_delay_wq);
	}
headset_btn_down_wq_err:
	if(pdata->headset_btn_up_delay_wq) {
		cancel_delayed_work(&pdata->headset_btn_up_delay_work);
		flush_workqueue(pdata->headset_btn_up_delay_wq);
		destroy_workqueue(pdata->headset_btn_up_delay_wq);
	}
headset_btn_up_wq_err:
	if(pdata->headset_plug_out_delay_wq) {
		cancel_delayed_work(&pdata->headset_plug_out_delay_work);
		flush_workqueue(pdata->headset_plug_out_delay_wq);
		destroy_workqueue(pdata->headset_plug_out_delay_wq);
	}
headset_plug_out_wq_err:
	if(pdata->headset_plug_in_delay_wq) {
		cancel_delayed_work(&pdata->headset_plug_in_delay_work);
		flush_workqueue(pdata->headset_plug_in_delay_wq);
		destroy_workqueue(pdata->headset_plug_in_delay_wq);
	}
headset_plug_in_wq_err:
	return ret;
}

static int hi6402es_mbhc_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver hi6402es_mbhc_driver = {
	.driver = {
		.name	= "hi6402es_mbhc",
		.owner	= THIS_MODULE,
		.of_match_table = hi6402es_mbhc_of_match,
	},
	.probe	= hi6402es_mbhc_probe,
	.remove	= hi6402es_mbhc_remove,
};

static int __init hi6402es_mbhc_init(void)
{
	return platform_driver_register(&hi6402es_mbhc_driver);
}

static void __exit hi6402es_mbhc_exit(void)
{
	platform_driver_unregister(&hi6402es_mbhc_driver);
}

late_initcall(hi6402es_mbhc_init);
module_exit(hi6402es_mbhc_exit);

MODULE_DESCRIPTION("hi6402es_mbhc driver");
MODULE_LICENSE("GPL");
