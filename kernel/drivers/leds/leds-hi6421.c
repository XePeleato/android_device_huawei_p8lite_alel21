/*
 * LEDs driver for k3
 *
 * Copyright (c) 2011-2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/gpio.h>
#include <linux/huawei/hi6421-leds.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/notifier.h>
#include <linux/nmi.h>
#include <huawei_platform/dsm/dsm_pub.h>
#include <linux/of_device.h>

extern struct atomic_notifier_head panic_notifier_list;

static struct k3_led_drv_data *k3_led_pdata;
unsigned long k3_leds_status = 0;
static struct dsm_dev dsm_leds = {
	.name = "dsm_leds",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 512,
};
static struct dsm_client *leds_client = NULL;

static void k3_led_set_disable(u8 id)
{
	u32 k3_led_dr_ctl;

	k3_led_dr_ctl = K3_LED_REG_R(DR_LED_CTRL);

	if (id == K3_LED0) {
		k3_led_dr_ctl &= DR3_DISABLE;
	} else if (id == K3_LED1) {
		k3_led_dr_ctl &= DR4_DISABLE;
	} else if (id == K3_LED2) {
		k3_led_dr_ctl &= DR5_DISABLE;
	} else {
		return;
	}

	K3_LED_REG_W(k3_led_dr_ctl, DR_LED_CTRL);
	clear_bit(id, &k3_leds_status);
}

static void k3_led_panic_handler(void)
{
    int index;

    pr_info("Apanic: %s\n", __func__);
    for(index = 0; index < K3_LEDS_MAX; index++)
    {
        k3_led_set_disable(index);
	}
}

static struct notifier_block panic_led = {
	.notifier_call	= k3_led_panic_handler,
};

static void k3_led_set_reg_write(struct led_set_config *brightness_config)
{
	/* config current */
	K3_LED_REG_W(brightness_config->k3_led_dr_iset, DR_LED_ISET);
	/* start_delay  */
	K3_LED_REG_W(DR_START_DEL_512, brightness_config->k3_led_start_address);
	/* set_on  */
	K3_LED_REG_W(DR_DELAY_ON, brightness_config->k3_led_tim_address);
	/* enable */
	K3_LED_REG_W(brightness_config->k3_led_dr_ctl, DR_LED_CTRL);
	/* output enable */
	K3_LED_REG_W(brightness_config->k3_led_dr_out_ctl, DR_OUT_CTRL);
}

/* set led half brightness or full brightness  */
static void k3_led_set_enbale(u8 brightness, u8 id)
{
	u32 k3_led_dr_ctl;
	u32 k3_led_dr_iset;
	u32 k3_led_dr_out_ctl;
	struct led_set_config brightness_config;

	k3_led_dr_ctl = K3_LED_REG_R(DR_LED_CTRL);
	k3_led_dr_iset = K3_LED_REG_R(DR_LED_ISET);
	k3_led_dr_out_ctl = K3_LED_REG_R(DR_OUT_CTRL);

	switch (id) {

	case K3_LED0:
		brightness_config.k3_led_start_address = DR3_START_DEL;
		brightness_config.k3_led_tim_address = DR3_TIM_CONF0;
		brightness_config.k3_led_dr_iset = k3_led_dr_iset;
		brightness_config.k3_led_dr_out_ctl = k3_led_dr_out_ctl & DR3_OUT_ENABLE;
		if (brightness == LED_FULL)
			brightness_config.k3_led_dr_ctl = (k3_led_dr_ctl & DR35_BRIGHTNESS_CLR) |
								DR3_ENABLE | DR35_BRIGHTNESS_FULL;
		else
			brightness_config.k3_led_dr_ctl = ((k3_led_dr_ctl & DR35_BRIGHTNESS_CLR) |
								DR3_ENABLE) | DR35_BRIGHTNESS_HALF;
		break;

	case K3_LED1:
		brightness_config.k3_led_start_address = DR4_START_DEL;
		brightness_config.k3_led_tim_address = DR4_TIM_CONF0;
		brightness_config.k3_led_dr_ctl = k3_led_dr_ctl | DR4_ENABLE;
		brightness_config.k3_led_dr_out_ctl = k3_led_dr_out_ctl & DR4_OUT_ENABLE;
		if (brightness == LED_FULL)
			brightness_config.k3_led_dr_iset = (k3_led_dr_iset & DR4_BRIGHTNESS_CLR) | DR4_BRIGHTNESS_FULL;
		else
			brightness_config.k3_led_dr_iset = (k3_led_dr_iset & DR4_BRIGHTNESS_CLR) | DR4_BRIGHTNESS_HALF;
		break;

	case K3_LED2:
		brightness_config.k3_led_start_address = DR5_START_DEL;
		brightness_config.k3_led_tim_address = DR5_TIM_CONF0;
		brightness_config.k3_led_dr_ctl = k3_led_dr_ctl | DR5_ENABLE;
		brightness_config.k3_led_dr_out_ctl = k3_led_dr_out_ctl & DR5_OUT_ENABLE;
		if (brightness == LED_FULL)
			brightness_config.k3_led_dr_iset = (k3_led_dr_iset & DR35_BRIGHTNESS_CLR) | DR35_BRIGHTNESS_FULL;
		else
			brightness_config.k3_led_dr_iset = (k3_led_dr_iset & DR35_BRIGHTNESS_CLR) | DR35_BRIGHTNESS_HALF;

		break;

	default:
		break;
	}

	k3_led_set_reg_write(&brightness_config);

	set_bit(id, &k3_leds_status);
	smp_wmb();
	if (test_bit(K3_LED0, &k3_leds_status) && test_bit(K3_LED1, &k3_leds_status)
			&& test_bit(K3_LED2, &k3_leds_status) && !dsm_client_ocuppy(leds_client)) {
		dsm_client_record(leds_client, "LED0 LED1 LED2 are all turn on, so the sensor leds "\
				"display white.\n");
		dsm_client_notify(leds_client, DSM_LEDS_ERROR_NO);
	}
}

/* set enable or disable led of dr3,4,5 */
static int k3_led_set(struct k3_led_data *led, u8 brightness)
{
	int ret = 0;
	u8 id = led->id;
	struct k3_led_drv_data *data = k3_led_pdata;

	if (brightness != LED_OFF && brightness != LED_HALF && brightness != LED_FULL) {
		pr_err("k3_led_set brightness:%d is error\n", brightness);
		ret = -EINVAL;
		return ret;
	}

	mutex_lock(&data->lock);

	switch (id) {

	case K3_LED0:
	case K3_LED1:
	case K3_LED2:
		if (brightness == LED_OFF) {
			/* set led off */
			k3_led_set_disable(id);
		} else {
			/* set led brightness */
			k3_led_set_enbale(brightness, id);
		}
		break;
	default:
		pr_err("k3_led_set id:%d is error\n", id);
		ret = -EINVAL;
		break;
	}

	mutex_unlock(&data->lock);
	pr_info("led id:%d display brightness: %d.\n", id, brightness);

	return ret;
}
static void k3_led_set_blink_reg_write(u8 id, u8 set_time)
{
	u32 led_address = DR3_TIM_CONF0;

	if (id == K3_LED0) {
		led_address = DR3_TIM_CONF0;
	} else if (id == K3_LED1) {
		led_address = DR4_TIM_CONF0;
	} else if (id == K3_LED2) {
		led_address = DR5_TIM_CONF0;
	} else {
		return ;
	}

	K3_LED_REG_W(set_time, led_address);
}
/* get the set time in area */
static u8 k3_led_get_time(unsigned long delay, u8 flag)
{
	u8 set_time = 0;

	if (delay == DEL_0)
		set_time = DR_DEL00;
	else if (delay <= DEL_125)
		set_time = DR_DEL01;
	else if (delay <= DEL_250)
		set_time = DR_DEL02;
	else if (delay <= DEL_500)
		set_time = DR_DEL03;
	else if (delay <= DEL_1000)
		set_time = DR_DEL04;
	else if (delay <= DEL_2000)
		set_time = DR_DEL05;
	else if (delay <= DEL_2850)
		set_time = DR_DEL06;
	else if (delay <= DEL_3000)
		set_time = DR_DEL07;
	else if (delay <= DEL_4000)
		set_time = DR_DEL08;
	else if (delay <= DEL_6000)
		set_time = DR_DEL09;
	else if (delay <= DEL_8000)
		set_time = DR_DEL10;
	else if (delay <= DEL_12000)
		set_time = DR_DEL11;
	else if (delay <= DEL_14000)
		set_time = DR_DEL12;
	else
		set_time = DR_DEL13;

	if (flag)
		return set_time << 4;
	else
		return set_time;
}

/* config of dr3,4,5 delay_on and delay_off registers */
static int k3_led_set_blink(struct led_classdev *led_ldev,
	unsigned long *delay_on, unsigned long *delay_off)
{
	struct k3_led_data *led_dat =
		container_of(led_ldev, struct k3_led_data, ldev);

	struct k3_led_drv_data *data = k3_led_pdata;

	u8 id = 0;
	int ret = 0;
	u8 set_time_on = 0;
	u8 set_time_off = 0;
	if (!led_dat) {
		pr_err("led set blink error\n");
		return -EINVAL;
	}
	id = led_dat->id;
	if ((*delay_on == 0) && (*delay_off == 0))
		return ret;

	mutex_lock(&data->lock);

	if ((id == K3_LED0) || (id == K3_LED1) || (id == K3_LED2)) {
		led_ldev->blink_delay_on =  *delay_on;
		led_ldev->blink_delay_off = *delay_off;

		set_time_on = k3_led_get_time(*delay_on, DELAY_ON);
		set_time_off = k3_led_get_time(*delay_off, DELAY_OFF);
		k3_led_set_blink_reg_write(id, set_time_on | set_time_off);
	} else {
		pr_err("k3_led_set_blink id:%d is error\n", id);
		ret = -EINVAL;
	}

	mutex_unlock(&data->lock);
	return ret;
}

/* set brightness of dr3,4,5 lights*/
static void k3_led_set_brightness(struct led_classdev *led_ldev,
				      enum led_brightness brightness)
{
	struct k3_led_data *led =
		container_of(led_ldev, struct k3_led_data, ldev);
	if (!led) {
		pr_err("led set btrightnss error!\n");
	}
	led->status.brightness = brightness;
	k3_led_set(led, led->status.brightness);
}

/* config lights */
static int k3_led_configure(struct platform_device *pdev,
				struct k3_led_drv_data *data,
				struct k3_led_platform_data *pdata)
{
	int i;
	int ret = 0;

	for (i = 0; i < pdata->leds_size; i++) {
		struct k3_led *pled = &pdata->leds[i];
		struct k3_led_data *led = &data->leds[i];
		led->id = i;

		led->status.brightness = pled->brightness;
		led->status.delay_on = pled->delay_on;
		led->status.delay_off = pled->delay_off;
		led->ldev.name = pled->name;
		led->ldev.default_trigger = pled->default_trigger;
		led->ldev.max_brightness = LED_FULL;
		led->ldev.blink_set = k3_led_set_blink;
		led->ldev.brightness_set = k3_led_set_brightness;
		//led->ldev.flags = LED_CORE_SUSPENDRESUME;

		ret = led_classdev_register(&pdev->dev, &led->ldev);
		if (ret < 0) {
			dev_err(&pdev->dev,
					"couldn't register LED %s\n",
					led->ldev.name);
			goto exit;
		}
	}
	return 0;

exit:
	if (i > 0) {
		for (i = i - 1; i >= 0; i--) {
			led_classdev_unregister(&data->leds[i].ldev);
		}
	}
	return ret;
}

static struct k3_led_platform_data hi6421_leds = {
	.leds_size = K3_LEDS_MAX,
	.leds = {
		[0] = {
			.name = "red",
			.brightness = LED_OFF,
			.delay_on = 0,
			.delay_off = 0,
			.default_trigger = "timer",
		},
		[1] = {
			.name = "green",
			.brightness = LED_OFF,
			.delay_on = 0,
			.delay_off = 0,
			.default_trigger = "timer",
		},
		[2] {
			.name = "blue",
			.brightness = LED_OFF,
			.delay_on = 0,
			.delay_off = 0,
			.default_trigger = "timer",
		},
	},
};

#ifdef CONFIG_OF
static const struct of_device_id hsk3_led_match[] = {
	{ .compatible = "hisilicon,hi6421-led",
		.data = &hi6421_leds,},
	{},
};
MODULE_DEVICE_TABLE(of, hsk3_led_match);
#endif
static int k3_led_probe(struct platform_device *pdev)
{
	struct k3_led_platform_data *pdata;
	struct k3_led_drv_data *data;
	struct resource *res;
	const struct of_device_id *match;
	int ret = 0 ;

	if ((match = of_match_node(hsk3_led_match, pdev->dev.of_node)) == NULL) {
		dev_err(&pdev->dev, "dev node is not match. exiting.\n");
		return -ENODEV;
	}

	pdata = (struct k3_led_platform_data *)match->data;
	if (pdata == NULL) {
		dev_err(&pdev->dev, "no platform data\n");
		return -EINVAL;
	}

	data = devm_kzalloc(&pdev->dev, sizeof(struct k3_led_drv_data), GFP_KERNEL);
	if (!data) {
		dev_err(&pdev->dev, "failed to allocate led_device\n");
		return -ENOMEM;
	}

	/* get base_address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to find registers\n");
		ret = -ENXIO;
		goto err;
	}
	data->k3_led_base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!data->k3_led_base) {
		dev_err(&pdev->dev, "failed to map registers\n");
		ret = -ENXIO;
		goto err;
	}

	mutex_init(&data->lock);

	platform_set_drvdata(pdev, data);
	k3_led_pdata = data;

	/*config lights*/
	ret = of_property_read_string(pdev->dev.of_node, "hi6421,dr3ctrl", &hi6421_leds.leds[0].name);
	if (ret < 0) {
		dev_err(&pdev->dev, "config dr3's colour failure!\n");
		goto err_remap;
	}

	ret = of_property_read_string(pdev->dev.of_node, "hi6421,dr4ctrl", &hi6421_leds.leds[1].name);
	if (ret < 0) {
		dev_err(&pdev->dev, "config dr4's colour failure!\n");
		goto err_remap;
	}

	ret = of_property_read_string(pdev->dev.of_node, "hi6421,dr5ctrl", &hi6421_leds.leds[2].name);
	if (ret < 0) {
		dev_err(&pdev->dev, "config dr5's colour failure!\n");
		goto err_remap;
	}

	ret = k3_led_configure(pdev, data, pdata);
	if (ret < 0)
		goto err_remap;

	atomic_notifier_chain_register(&panic_notifier_list, &panic_led);

	if (!leds_client) {
		leds_client = dsm_register_client(&dsm_leds);
	}
	return 0;

err_remap:
	devm_iounmap(&pdev->dev, data->k3_led_base);
err:
	kfree(data);
	data = NULL;
	return ret;
}

static int k3_led_remove(struct platform_device *pdev)
{
	int i;
	struct k3_led_platform_data *pdata;
	const struct of_device_id *match = NULL;
	struct k3_led_drv_data *data = platform_get_drvdata(pdev);

	if (data == NULL) {
		dev_err(&pdev->dev, "%s:data is NULL\n", __func__);
		return -ENODEV;
	}

	match = of_match_node(hsk3_led_match, pdev->dev.of_node);
	if (match == NULL) {
		dev_err(&pdev->dev, "%s:Device id is NULL\n", __func__);
		return -ENODEV;
	}
	pdata = (struct k3_led_platform_data *)match->data;
	for (i = 0; i < pdata->leds_size; i++) {
		led_classdev_unregister(&data->leds[i].ldev);
	}

	devm_iounmap(&pdev->dev, data->k3_led_base);
	kfree(data);
	data = NULL;
	platform_set_drvdata(pdev, NULL);
	return 0;
}

static void k3_led_shutdown(struct platform_device *pdev)
{
	u32 k3_led_dr_ctl = 0;
	struct k3_led_drv_data *data = platform_get_drvdata(pdev);
	if (data == NULL) {
		dev_err(&pdev->dev, "%s:data is NULL\n", __func__);
		return;
	}

	k3_led_dr_ctl = K3_LED_REG_R(DR_LED_CTRL);
	K3_LED_REG_W(k3_led_dr_ctl & DR_CONTR_DISABLE, DR_LED_CTRL);

	return ;
}

#ifdef CONFIG_PM
static int k3_led_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct k3_led_drv_data *data = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s: suspend +\n", __func__);
	if (data == NULL) {
		dev_err(&pdev->dev, "%s:data is NULL\n", __func__);
		return -ENODEV;
	}

	if (!mutex_trylock(&data->lock)) {
		dev_err(&pdev->dev, "%s: mutex_trylock.\n", __func__);
		return -EAGAIN;
	}

	dev_info(&pdev->dev, "%s: suspend -\n", __func__);
	return 0;
}
static int k3_led_resume(struct platform_device *pdev)
{
	struct k3_led_drv_data *data = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s: resume +\n", __func__);
	if (data == NULL) {
		dev_err(&pdev->dev, "%s:data is NULL\n", __func__);
		return -ENODEV;
	}

	mutex_unlock(&data->lock);

	dev_info(&pdev->dev, "%s: resume -\n", __func__);
	return 0;
}
#endif

static struct platform_driver k3_led_driver = {
	.probe		= k3_led_probe,
	.remove		= k3_led_remove,
	.shutdown	= k3_led_shutdown,
#ifdef CONFIG_PM
	.suspend	= k3_led_suspend,
	.resume		= k3_led_resume,
#endif
	.driver		= {
		.name	= K3_LEDS,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(hsk3_led_match),
	},
};

module_platform_driver(k3_led_driver);

MODULE_ALIAS("platform:k3-leds");
MODULE_DESCRIPTION("k3 LED driver");
MODULE_AUTHOR(""songyantao<songyantao@huawei.com>"");
MODULE_LICENSE("GPL");
