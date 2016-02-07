/*
 *
 * Copyright (c) 2011-2013 Hisilicon Technologies CO., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/hrtimer.h>
#include <linux/err.h>
#include <linux/irq.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/workqueue.h>

#include "../../staging/android/timed_output.h"
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/clk.h>
#include <linux/of_device.h>
#include <linux/huawei/hi6421_vibrator.h>

struct k3_vibrator_data {
	int work_mode;
	u8 lm_dr2;
	u8 reg_dr2_vset;
	u8 lra_ocp_en;
	u8 lra_ocp_sel;
	u8 lra_freq;
	struct timed_output_dev dev;
	struct hrtimer timer;
	struct mutex lock;
	spinlock_t sp_lock;
	void __iomem *k3_vibrator_base;
};

// Begin Immersion changes
static struct k3_vibrator_data *p_data = NULL;
// End Immersion changes

static void k3_vibrator_on(struct k3_vibrator_data  *pdata)
{
	u32 k3_vibrator_dr2_ctrl = 0;
	u32 k3_vibrator_dr2_reserved = 0;
	unsigned long flags;

	if (!pdata) {
		pr_err("error:k3_vibrator_on data is null\n");
		return;
	}

	spin_lock_irqsave(&pdata->sp_lock, flags);

	k3_vibrator_dr2_ctrl = K3_VIB_REG_R(pdata->k3_vibrator_base, DR2_CTRL);
	k3_vibrator_dr2_ctrl = (k3_vibrator_dr2_ctrl & DR2_TEST_MODE_CLR) | \
				LM_FIX_MODE | REG_DR2_EN | pdata->lm_dr2;
	K3_VIB_REG_W(pdata->reg_dr2_vset, pdata->k3_vibrator_base, DR2_ISET);
	if (pdata->work_mode) {
		k3_vibrator_dr2_reserved = (K3_VIB_REG_R(pdata->k3_vibrator_base, DR2_RESERVED) & \
					DR2_OCP_HIGHZ_CLR) | DR2_HIGHZ_DIS | pdata->lra_ocp_sel;
		K3_VIB_REG_W(pdata->lra_freq, pdata->k3_vibrator_base, DR2_FREQ);
		K3_VIB_REG_W(k3_vibrator_dr2_reserved, pdata->k3_vibrator_base, DR2_RESERVED);
		K3_VIB_REG_W(k3_vibrator_dr2_ctrl | DRIVE_MODE | pdata->lra_ocp_en,
					pdata->k3_vibrator_base, DR2_CTRL);
	} else {
		K3_VIB_REG_W(k3_vibrator_dr2_ctrl, pdata->k3_vibrator_base, DR2_CTRL);
	}

	spin_unlock_irqrestore(&pdata->sp_lock, flags);
	return ;
}

static void k3_vibrator_off(struct k3_vibrator_data  *pdata)
{
	u32 k3_vibrator_dr2_ctrl = 0;
	u32 k3_vibrator_dr2_reserved = 0;
	unsigned long flags;

	if (!pdata) {
		pr_err("error:k3_vibrator_off data is null\n");
		return;
	}
	spin_lock_irqsave(&pdata->sp_lock, flags);

	if (pdata->work_mode) {
		k3_vibrator_dr2_reserved = K3_VIB_REG_R(pdata->k3_vibrator_base, DR2_RESERVED) | DR2_HIGHZ_EN;
		K3_VIB_REG_W(k3_vibrator_dr2_reserved, pdata->k3_vibrator_base, DR2_RESERVED);
	}
	k3_vibrator_dr2_ctrl = K3_VIB_REG_R(pdata->k3_vibrator_base, DR2_CTRL);
	K3_VIB_REG_W(k3_vibrator_dr2_ctrl & DR2_DISABLE, pdata->k3_vibrator_base, DR2_CTRL);

	spin_unlock_irqrestore(&pdata->sp_lock, flags);
	return ;
}

static enum hrtimer_restart k3_vibrator_timer_func(struct hrtimer *timer)
{
	struct k3_vibrator_data *pdata = container_of(timer, struct k3_vibrator_data, timer);
	if (!pdata) {
		pr_err("error: get k3_vibrator_data is null!\n");
	}
	k3_vibrator_off(pdata);
	return HRTIMER_NORESTART;
}

static int k3_vibrator_get_time(struct timed_output_dev *dev)
{
	struct k3_vibrator_data *pdata =
			container_of(dev, struct k3_vibrator_data, dev);
	if (!pdata) {
		pr_err("error:k3_vibrator_get_time pdata is null\n");
		return -1;
	}
	if (hrtimer_active(&pdata->timer)) {
		ktime_t r = hrtimer_get_remaining(&pdata->timer);
		struct timeval t = ktime_to_timeval(r);
		return t.tv_sec * 1000 + t.tv_usec / 1000;
	} else {
		return 0;
	}
}
#if 0
static u32 k3_vibrator_get_value(int value)
{
	u32 set_value = 0;

	if (value <= DR2_OT_SEL_10)
		set_value = DR2_OT_SEL00;
	else if (value <= DR2_OT_SEL_20)
		set_value = DR2_OT_SEL01;
	else if (value <= DR2_OT_SEL_40)
		set_value = DR2_OT_SEL02;
	else if (value <= DR2_OT_SEL_50)
		set_value = DR2_OT_SEL03;
	else if (value <= DR2_OT_SEL_60)
		set_value = DR2_OT_SEL04;
	else if (value <= DR2_OT_SEL_80)
		set_value = DR2_OT_SEL05;
	else if (value <= DR2_OT_SEL_100)
		set_value = DR2_OT_SEL06;
	else if (value <= DR2_OT_SEL_200)
		set_value = DR2_OT_SEL07;
	else if (value <= DR2_OT_SEL_300)
		set_value = DR2_OT_SEL08;
	else if (value <= DR2_OT_SEL_400)
		set_value = DR2_OT_SEL09;
	else if (value <= DR2_OT_SEL_500)
		set_value = DR2_OT_SEL10;
	else if (value <= DR2_OT_SEL_600)
		set_value = DR2_OT_SEL11;
	else if (value <= DR2_OT_SEL_700)
		set_value = DR2_OT_SEL12;
	else if (value <= DR2_OT_SEL_800)
		set_value = DR2_OT_SEL13;
	else if (value <= DR2_OT_SEL_900)
		set_value = DR2_OT_SEL14;
	else
		set_value = DR2_OT_SEL15;

	return set_value;
}
#endif

static void k3_vibrator_enable(struct timed_output_dev *dev, int value)
{
	struct k3_vibrator_data *pdata = container_of(dev, struct k3_vibrator_data, dev);

	if (value < 0 ||!pdata ) {
		pr_err("error:vibrator_enable value:%d is negative\n", value);
		return;
	}
	/* cancel previous timer */
	if (hrtimer_active(&pdata->timer))
		hrtimer_cancel(&pdata->timer);
	#if 0
	if (value > 0 && value <= DR2_OT_SEL_1000) {
		u32 set_value = 0;
		u32 k3_vibrator_dr2_ctrl = 0;

		mutex_lock(&pdata->lock);
		K3_VIB_REG_W(pdata->reg_dr2_vset, pdata->k3_vibrator_base, DR2_ISET);
		k3_vibrator_dr2_ctrl = K3_VIB_REG_R(pdata->k3_vibrator_base, DR2_CTRL);
		if (pdata->work_mode)
			K3_VIB_REG_W((k3_vibrator_dr2_ctrl | LM_FIX_MODE | DRIVE_MODE | \
					pdata->lm_dr2) & DR2_DISABLE, pdata->k3_vibrator_base, DR2_CTRL);
		else
			K3_VIB_REG_W((k3_vibrator_dr2_ctrl | LM_FIX_MODE | pdata->lm_dr2) & DR2_DISABLE, \
					pdata->k3_vibrator_base, DR2_CTRL);

		set_value = k3_vibrator_get_value(value);
		K3_VIB_REG_W(set_value | DR2_OT_EN, pdata->k3_vibrator_base, DR2_CTRL1);
		mutex_unlock(&pdata->lock);
	} else if (value > DR2_OT_SEL_1000) {
	#endif
	if (value > 0) {
		k3_vibrator_on(pdata);
		hrtimer_start(&pdata->timer,
			ktime_set(value / 1000, (value % 1000) * 1000000),
			HRTIMER_MODE_REL);
	} else {
		k3_vibrator_off(pdata);
	}
}

// Begin Immersion changes
void imm_vibrator_en(bool en)
{
	if (!p_data ) {
		pr_err("error:p_data is null\n");
		return;
	}

	if(en){
		k3_vibrator_on(p_data);
	}
	else{
		k3_vibrator_off(p_data);
	}
}
EXPORT_SYMBOL(imm_vibrator_en);

void imm_vibrator_pwm(int force)
{
	if(force > 0)
		p_data->reg_dr2_vset = (u8)(32*force/128+31);
	else if(force == 0)
		p_data->reg_dr2_vset = 0x20;
	else if(force < 0)
		p_data->reg_dr2_vset = 0x0;
	else
		return;

	if(p_data->reg_dr2_vset >= 0x0 || p_data->reg_dr2_vset <= 0x3F)
	{
		K3_VIB_REG_W(p_data->reg_dr2_vset, p_data->k3_vibrator_base, DR2_ISET);
	}
}
EXPORT_SYMBOL(imm_vibrator_pwm);
// End Immersion changes

#ifdef CONFIG_OF
static const struct of_device_id hsk3_vibrator_match[] = {
	{ .compatible = "hisilicon,hi6421-vibrator",},
	{},
};
MODULE_DEVICE_TABLE(of, hsk3_vibrator_match);
#endif

static int k3_vibrator_probe(struct platform_device *pdev)
{
	// Begin Immersion changes
	// struct k3_vibrator_data *p_data;
	// End Immersion changes

	struct resource *res;
	int ret = 0;
	u32 config_val;
	if (!of_match_node(hsk3_vibrator_match, pdev->dev.of_node)) {
		dev_err(&pdev->dev, "dev node is not match. exiting.\n");
		return -ENODEV;
	}

	p_data = devm_kzalloc(&pdev->dev, sizeof(struct k3_vibrator_data), GFP_KERNEL);
	if (p_data == NULL) {
		dev_err(&pdev->dev, "failed to allocate vibrator_device\n");
		return -ENOMEM;
	}

	/* get base_address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to find registers\n");
		ret = -ENXIO;
		goto err;
	}

	p_data->k3_vibrator_base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!p_data->k3_vibrator_base) {
		dev_err(&pdev->dev, "failed to map registers\n");
		ret = -ENXIO;
		goto err;
	}

	/* init timer */
	hrtimer_init(&p_data->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	p_data->timer.function = k3_vibrator_timer_func;

	/* init lock */
	mutex_init(&p_data->lock);
	spin_lock_init(&p_data->sp_lock);

	p_data->dev.name = K3_VIBRATOR;
	p_data->dev.get_time = k3_vibrator_get_time;
	p_data->dev.enable = k3_vibrator_enable;

	if(of_property_read_u32(pdev->dev.of_node, "hisi,erm_lra", &config_val))
		p_data->work_mode = WORK_ERM;
	else
		p_data->work_mode = config_val;
	if (p_data->work_mode) {
		if (!of_property_read_u32(pdev->dev.of_node, "hisi,ocp_en", &config_val))
			p_data->lra_ocp_en = config_val >> 7;
		else
			p_data->lra_ocp_en = 0;
		if (!of_property_read_u32(pdev->dev.of_node, "hisi,ocp_sel", &config_val))
			p_data->lra_ocp_sel = config_val >> 5;
		else
			p_data->lra_ocp_sel = 0;
		if (!of_property_read_u32(pdev->dev.of_node, "hisi,freq", &config_val))
			p_data->lra_freq = config_val;
		else
			p_data->lra_freq = 0x20;
	}

	if(of_property_read_u32(pdev->dev.of_node, "hisi,dr2_test", &config_val))
		p_data->lm_dr2 = MAX_LM_DR2;
	else
		p_data->lm_dr2  =  (config_val << 5) & MAX_LM_DR2;

	if(of_property_read_u32(pdev->dev.of_node, "hisi,dr2_vset", &config_val))
		p_data->reg_dr2_vset = ISET_POWER;
	else
		p_data->reg_dr2_vset = config_val & ISET_POWER;

	ret = timed_output_dev_register(&p_data->dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to regist dev\n");
		goto err_remap;
	}

	platform_set_drvdata(pdev, p_data);

	return 0;
err_remap:
	devm_iounmap(&pdev->dev, p_data->k3_vibrator_base);
err:
	kfree(p_data);
	p_data = NULL;
	return ret;
}

static int k3_vibrator_remove(struct platform_device *pdev)
{
	struct k3_vibrator_data *pdata = platform_get_drvdata(pdev);

	if (pdata == NULL) {
		dev_err(&pdev->dev, "%s:pdata is NULL\n", __func__);
		return -ENODEV;
	}

	if (hrtimer_active(&pdata->timer))
		hrtimer_cancel(&pdata->timer);

	timed_output_dev_unregister(&pdata->dev);

	devm_iounmap(&pdev->dev, pdata->k3_vibrator_base);
	platform_set_drvdata(pdev, NULL);
	kfree(pdata);
	pdata = NULL;

	return 0;
}

static void k3_vibrator_shutdown(struct platform_device *pdev)
{
	u32 k3_vibrator_dr2_ctrl = 0;
	struct k3_vibrator_data *pdata = platform_get_drvdata(pdev);
	if (pdata == NULL) {
		dev_err(&pdev->dev, "%s:pdata is NULL\n", __func__);
		return;
	}

	if (hrtimer_active(&pdata->timer)) {
		hrtimer_cancel(&pdata->timer);
		k3_vibrator_dr2_ctrl = K3_VIB_REG_R(pdata->k3_vibrator_base, DR2_CTRL);
		K3_VIB_REG_W(k3_vibrator_dr2_ctrl & DR2_DISABLE, pdata->k3_vibrator_base, DR2_CTRL);
	}

	return ;
}

#ifdef CONFIG_PM
static int  k3_vibrator_suspend(struct platform_device *pdev, pm_message_t state)
{
	u32 k3_vibrator_dr2_ctrl = 0;
	struct k3_vibrator_data *pdata = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s: suspend +\n", __func__);
	if (pdata == NULL) {
		dev_err(&pdev->dev, "%s:pdata is NULL\n", __func__);
		return -ENODEV;
	}

	if (!mutex_trylock(&pdata->lock)) {
		dev_err(&pdev->dev, "%s: mutex_trylock.\n", __func__);
		return -EAGAIN;
	}
	if (hrtimer_active(&pdata->timer)) {
		hrtimer_cancel(&pdata->timer);
		k3_vibrator_dr2_ctrl = K3_VIB_REG_R(pdata->k3_vibrator_base, DR2_CTRL);
		K3_VIB_REG_W(k3_vibrator_dr2_ctrl & DR2_DISABLE, pdata->k3_vibrator_base, DR2_CTRL);
	}

	dev_info(&pdev->dev, "%s: suspend -\n", __func__);
	return 0;
}

static int k3_vibrator_resume(struct platform_device *pdev)
{
	struct k3_vibrator_data *pdata = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s: resume +\n", __func__);
	if (pdata == NULL) {
		dev_err(&pdev->dev, "%s:pdata is NULL\n", __func__);
		return -ENODEV;
	}

	mutex_unlock(&pdata->lock);

	dev_info(&pdev->dev, "%s: resume -\n", __func__);
	return 0;
}
#endif

static struct platform_driver k3_vibrator_driver = {
	.probe  = k3_vibrator_probe,
	.remove = k3_vibrator_remove,
	.shutdown	= k3_vibrator_shutdown,
#ifdef CONFIG_PM
	.suspend	= k3_vibrator_suspend,
	.resume		= k3_vibrator_resume,
#endif
	.driver = {
		.name   = K3_VIBRATOR,
		.owner  = THIS_MODULE,
		.of_match_table = of_match_ptr(hsk3_vibrator_match),
	},
};

module_platform_driver(k3_vibrator_driver);

MODULE_DESCRIPTION(" k3 vibrator driver");
MODULE_AUTHOR("chenxiaoping<chenxiaoping@huawei.com>");
MODULE_LICENSE("GPL");
