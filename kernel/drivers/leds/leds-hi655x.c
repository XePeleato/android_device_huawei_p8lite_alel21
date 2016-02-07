/*
 * LEDs driver for k3
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/of_device.h>
#include <linux/hisi/hisi-leds.h>

int light_red = 0;
int light_green = 0;
int light_blue = 0;
int light_red_bright = 0;
int light_green_bright = 0;
int light_blue_bright = 0;
unsigned int rising_and_falling = 0x33; /*default Value, means 1s to rising and falling*/
unsigned char start_time = DR_START_DEL_512;
unsigned long light_red_delay_on = 0;
unsigned long light_red_delay_off = 0;
unsigned long light_green_delay_on = 0;
unsigned long light_green_delay_off = 0;
unsigned long light_blue_delay_on = 0;
unsigned long light_blue_delay_off = 0;
static struct hisi_led_drv_data *hisi_led_pdata;

extern unsigned char hi6xxx_pmic_reg_read (unsigned int reg_addr);
extern void hi6xxx_pmic_reg_write (unsigned int reg_addr, unsigned char regval);

/* read register  */
static unsigned char hisi_led_reg_read(u32 led_address)
{
    /* return readl(hisi_led_pdata->hisi_led_base + led_address);
    */
    /* + 2013-06-24; use the new API fanjicun*/
    return hi6xxx_pmic_reg_read(led_address);
    /* - 2013-06-24; use the new API fanjicun*/
}
/* write register  */
static void hisi_led_reg_write(u8 led_set, u32 led_address)
{
    /* + 2013-06-24; use the new API fanjicun*/
    /*
    writel(led_set, hisi_led_pdata->hisi_led_base + led_address);
    */
    hi6xxx_pmic_reg_write(led_address,led_set);
    /* - 2013-06-24; use the new API fanjicun*/
}
/* set led off  */
static void hisi_led_set_disable(u8 id)
{
    unsigned char hisi_led_dr_ctl;

    hisi_led_dr_ctl = hisi_led_reg_read(DR_LED_CTRL);
    printk(KERN_INFO "BEFORE Disable: Led_id=[%d], register value=[0x%x]\n", id, hisi_led_dr_ctl);

    switch (id) {

    case HISI_LED0:
        hisi_led_dr_ctl &= DR3_DISABLE;
                light_red_bright = 0;
        break;

    case HISI_LED1:
        hisi_led_dr_ctl &= DR4_DISABLE;
                light_green_bright = 0;
        break;

    case HISI_LED2:
        hisi_led_dr_ctl &= DR5_DISABLE;
                light_blue_bright = 0;
        break;

    default:
        break;
    }

    hisi_led_reg_write(hisi_led_dr_ctl, DR_LED_CTRL);
	
    hisi_led_dr_ctl = hisi_led_reg_read(DR_LED_CTRL);
    printk(KERN_INFO "AFTER Disable: Led_id=[%d], register value=[0x%x]\n", id, hisi_led_dr_ctl);
}
static void hisi_led_set_reg_write(struct led_set_config *bightness_config)
{
     /* config current  */
    hisi_led_reg_write(bightness_config->brightness_set, bightness_config->hisi_led_iset_address);
    /* start_delay  */
    hisi_led_reg_write(start_time, bightness_config->hisi_led_start_address);
    /* delay_on  */
    hisi_led_reg_write(DR_DELAY_ON, bightness_config->hisi_led_tim_address);
    /* enable */
    hisi_led_reg_write(bightness_config->hisi_led_dr_ctl, DR_LED_CTRL);
    /* output enable */
    hisi_led_reg_write(bightness_config->hisi_led_dr_out_ctl, DR_OUT_CTRL);
    /*set rising and falling*/
    hisi_led_reg_write(rising_and_falling,bightness_config->hisi_led_tim1_address );
}

/* set led half brightness or full brightness  */
static void hisi_led_set_select_led(u8 brightness_set, u8 id)
{
    unsigned char hisi_led_dr_ctl;
    unsigned char hisi_led_dr_out_ctl;
    struct led_set_config bightness_config;

    hisi_led_dr_ctl = hisi_led_reg_read(DR_LED_CTRL);
    hisi_led_dr_out_ctl = hisi_led_reg_read(DR_OUT_CTRL);

    switch (id) {
		
    case HISI_LED0:
        bightness_config.brightness_set = brightness_set;
        bightness_config.hisi_led_iset_address = DR3_ISET;
        bightness_config.hisi_led_start_address = DR3_START_DEL;
        bightness_config.hisi_led_tim_address = DR3_TIM_CONF0;
        bightness_config.hisi_led_dr_ctl = hisi_led_dr_ctl | DR3_ENABLE;
        bightness_config.hisi_led_dr_out_ctl = hisi_led_dr_out_ctl & DR3_OUT_ENABLE;
        bightness_config.hisi_led_tim1_address = DR3_TIM_CONF1;
        light_red = id;
        light_red_bright = brightness_set;
        break;
		
    case HISI_LED1:
        bightness_config.brightness_set = brightness_set;
        bightness_config.hisi_led_iset_address = DR4_ISET;
        bightness_config.hisi_led_start_address = DR4_START_DEL;
        bightness_config.hisi_led_tim_address = DR4_TIM_CONF0;
        bightness_config.hisi_led_dr_ctl = hisi_led_dr_ctl | DR4_ENABLE;
        bightness_config.hisi_led_dr_out_ctl = hisi_led_dr_out_ctl & DR4_OUT_ENABLE;
        bightness_config.hisi_led_tim1_address = DR4_TIM_CONF1;
        light_green = id;
        light_green_bright = brightness_set;
        break;
		
    case HISI_LED2:
        bightness_config.brightness_set = brightness_set;
        bightness_config.hisi_led_iset_address = DR5_ISET;
        bightness_config.hisi_led_start_address = DR5_START_DEL;
        bightness_config.hisi_led_tim_address = DR5_TIM_CONF0;
        bightness_config.hisi_led_dr_ctl = hisi_led_dr_ctl | DR5_ENABLE;
        bightness_config.hisi_led_dr_out_ctl = hisi_led_dr_out_ctl & DR5_OUT_ENABLE;
        bightness_config.hisi_led_tim1_address = DR5_TIM_CONF1;
        light_blue = id;
        light_blue_bright = brightness_set;
        break;
    default:
        break;
    }
    printk(KERN_INFO "hisi_led_set_select_led, Led_id=[%d], brightness_set=%d,\n", id, brightness_set);
    hisi_led_set_reg_write(&bightness_config);
}

/* set registers of dr3,4,5 */
static int hisi_led_set(struct hisi_led_data *led, u8 brightness)
{
    int ret = 0;
    u8 id = led->id;
    struct hisi_led_drv_data *data = hisi_led_pdata;

    if (brightness != LED_OFF && brightness != LED_HALF && brightness != LED_FULL) {
        pr_err("hisi_led_set brightness:%d is error\n", brightness);
        ret = -EINVAL;
        return ret;
    }

    mutex_lock(&data->lock);
    if (ret) {
        pr_err("pmu clock enable failed,ret:%d\n", ret);
        mutex_unlock(&data->lock);
        return ret;
    }

    switch (id) {

    case HISI_LED0:
    case HISI_LED1:
    case HISI_LED2:
        if (brightness == LED_OFF) {
            /* set led off */
            hisi_led_set_disable(id);
            printk(KERN_INFO "[%s] off id is %d\n", __FUNCTION__, id);
        } else if (brightness == LED_HALF) {
            /* set led half brightness */
            hisi_led_set_select_led(DR_BRIGHTNESS_HALF, id);
            printk(KERN_INFO "[%s] half id is %d\n",  __FUNCTION__, id);
        } else {
            /* set led full brightness */
            hisi_led_set_select_led(DR_BRIGHTNESS_FULL, id);
                printk(KERN_INFO "[%s] full id is %d\n", __FUNCTION__, id);
        }
        break;
    default:
        pr_err("hisi_led_set id:%d is error\n", id);
        ret = -EINVAL;
        break;
    }
    mutex_unlock(&data->lock);
    return ret;
}
static void hisi_led_set_blink_reg_write(u8 id, u8 set_time)
{
    u32 led_address = DR3_TIM_CONF0;
    u32 led_change_address = DR3_TIM_CONF1;
    u8 set_chang_time = 0x00;

    switch (id) {

    case HISI_LED0:
        led_address = DR3_TIM_CONF0;
        led_change_address = DR3_TIM_CONF1;
        break;

    case HISI_LED1:
        led_address = DR4_TIM_CONF0;
        led_change_address = DR4_TIM_CONF1;
        break;

    case HISI_LED2:
        led_address = DR5_TIM_CONF0;
        led_change_address = DR5_TIM_CONF1;
        break;

    default:
        break;
    }
    hisi_led_reg_write(set_time, led_address);
    hisi_led_reg_write(set_chang_time, led_change_address);
    printk(KERN_INFO "[%s] Led[%d] on_off time is [%u]\n", __FUNCTION__, id, set_time);

}
/* get the set time in area */
static u8 hisi_led_get_time(unsigned long delay, u8 flag)
{
    u8 set_time = 0;
    if (delay <= DEL_500)
        set_time = DR_DEL01;
    else if (delay <= DEL_1000)
        set_time = DR_DEL02;
    else if (delay <= DEL_2000)
        set_time = DR_DEL03;
    else if (delay <= DEL_4000)
        set_time = DR_DEL04;
    else if (delay <= DEL_6000)
        set_time = DR_DEL05;
    else if (delay <= DEL_8000)
        set_time = DR_DEL06;
    else if (delay <= DEL_12000)
        set_time = DR_DEL07;
    else if (delay <= DEL_14000)
        set_time = DR_DEL08;
    else
        set_time = DR_DEL09;

    if (flag)
        return set_time << 4;
    else
        return set_time;
}

/* config of  dr3,4,5 delay_on and delay_off registers */
static int hisi_led_set_blink(struct led_classdev *led_ldev,
    unsigned long *delay_on, unsigned long *delay_off)
{
    struct hisi_led_data *led_dat =
        container_of(led_ldev, struct hisi_led_data, ldev);

    struct hisi_led_drv_data *data = hisi_led_pdata;
    u8 id = led_dat->id;
    int ret = 0;
    u8 set_time_on = 0;
    u8 set_time_off = 0;

    if ((*delay_on == 0) && (*delay_off == 0))
        return ret;
        if (id == 0){
            light_red_delay_on = *delay_on;
            light_red_delay_off = *delay_off;
        }
        if (id == 1){
            light_green_delay_on = *delay_on;
            light_green_delay_off = *delay_off;
        }
        if (id == 2){
            light_blue_delay_on = *delay_on;
            light_blue_delay_off = *delay_off;
        }
    mutex_lock(&data->lock);

    switch (id) {

    case HISI_LED0:
    case HISI_LED1:
    case HISI_LED2:
        led_ldev->blink_delay_on =  *delay_on;
        led_ldev->blink_delay_off = *delay_off;

        set_time_on = hisi_led_get_time(*delay_on, DELAY_ON);
        set_time_off = hisi_led_get_time(*delay_off, DELAY_OFF);
        hisi_led_set_blink_reg_write(id, set_time_on | set_time_off);
        break;
    default:
        pr_err("hisi_led_set_blink id:%d is error\n", id);
        ret = -EINVAL;
        break;
    }
    mutex_unlock(&data->lock);
    return ret;
}

/* set brightness of dr3,4,5 lights*/
static void  hisi_led_set_brightness(struct led_classdev *led_ldev,
                      enum led_brightness brightness)
{
    struct hisi_led_data *led =
        container_of(led_ldev, struct hisi_led_data, ldev);

    led->status.brightness = brightness;
    hisi_led_set(led, led->status.brightness);
}

/* config lights */
static int hisi_led_configure(struct platform_device *pdev,
                struct hisi_led_drv_data *data,
                struct hisi_led_platform_data *pdata)
{
    int i;
    int ret = 0;

    for (i = 0; i < pdata->leds_size; i++) {
        struct hisi_led *pled = &pdata->leds[i];
        struct hisi_led_data *led = &data->leds[i];
        led->id = i;

        led->status.brightness = pled->brightness;
        led->status.delay_on = pled->delay_on;
        led->status.delay_off = pled->delay_off;
        led->ldev.name = pled->name;
        led->ldev.default_trigger = pled->default_trigger;
        led->ldev.max_brightness = LED_FULL;
        led->ldev.blink_set = hisi_led_set_blink;
        led->ldev.brightness_set = hisi_led_set_brightness;
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

static struct hisi_led_platform_data hi6552_leds = {
    .leds_size = HISI_LEDS_MAX,
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
static const struct of_device_id hi6552_led_match[] = {
    { .compatible = "hisilicon,hi6552-led",
        .data = &hi6552_leds,},
    {},
};
MODULE_DEVICE_TABLE(of, hi6552_led_match);
#endif

static int hisi_led_probe(struct platform_device *pdev)
{
    struct hisi_led_platform_data *pdata;
    struct hisi_led_drv_data *data;
    struct resource *res;
    const struct of_device_id *match;
    int ret = 0 ;

    if ((match = of_match_node(hi6552_led_match, pdev->dev.of_node)) == NULL) {
        dev_err(&pdev->dev, "dev node is not match. exiting.\n");
        return -ENODEV;
    }

    pdata = (struct hisi_led_platform_data *)match->data;
    if (pdata == NULL) {
        dev_err(&pdev->dev, "no platform data\n");
        return -EINVAL;
    }

    data = devm_kzalloc(&pdev->dev, sizeof(struct hisi_led_drv_data), GFP_KERNEL);
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
    data->hisi_led_base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
    if (!data->hisi_led_base) {
        dev_err(&pdev->dev, "failed to map registers\n");
        ret = -ENXIO;
        goto err;
    }

    mutex_init(&data->lock);

    platform_set_drvdata(pdev, data);
    hisi_led_pdata = data;

    /*config lights*/
    ret = of_property_read_string(pdev->dev.of_node, "hi6552,dr3ctrl", &hi6552_leds.leds[0].name);
    if (ret < 0) {
        dev_err(&pdev->dev, "config dr3's colour failure!\n");
        goto err_remap;
    }

    ret = of_property_read_string(pdev->dev.of_node, "hi6552,dr4ctrl", &hi6552_leds.leds[1].name);
    if (ret < 0) {
        dev_err(&pdev->dev, "config dr4's colour failure!\n");
        goto err_remap;
    }

    ret = of_property_read_string(pdev->dev.of_node, "hi6552,dr5ctrl", &hi6552_leds.leds[2].name);
    if (ret < 0) {
        dev_err(&pdev->dev, "config dr5's colour failure!\n");
        goto err_remap;
    }

    /*config lights*/
    ret = hisi_led_configure(pdev, data, pdata);
    if (ret < 0) {
        goto err_remap;
    }
  
	ret = of_property_read_u32(pdev->dev.of_node, "rising_and_falling", &rising_and_falling);
	if (ret < 0) 
		dev_err(&pdev->dev, "config rising_and_falling value failure ret = %d\n",ret);

	if(0 == rising_and_falling)
        start_time = 0x00;

    return 0;

err_remap:
    devm_iounmap(&pdev->dev,data->hisi_led_base);
err:
    kfree(data);
    data = NULL;
    return ret;
}

static int hisi_led_remove(struct platform_device *pdev)
{
    struct hisi_led_platform_data *pdata = pdev->dev.platform_data;
    struct hisi_led_drv_data *data = platform_get_drvdata(pdev);
    int i;

    if (data == NULL) {
        dev_err(&pdev->dev, "%s:data is NULL\n", __func__);
        return -ENODEV;
    }

    for (i = 0; i < pdata->leds_size; i++) {
        led_classdev_unregister(&data->leds[i].ldev);
    }

    devm_iounmap(&pdev->dev,data->hisi_led_base);
    kfree(data);
    data = NULL;
    platform_set_drvdata(pdev, NULL);
    return 0;
}
static void hisi_led_shutdown(struct platform_device *pdev)
{
    struct hisi_led_drv_data *data = platform_get_drvdata(pdev);

    if (data == NULL) {
        dev_err(&pdev->dev, "%s:data is NULL\n", __func__);
        return;
    }
    printk("[%s] +\n", __func__);

    hisi_led_reg_write(DR_CONTR_DISABLE, DR_LED_CTRL);

    printk("[%s] -\n", __func__);

    return ;
}

#ifdef CONFIG_PM
static int  hisi_led_suspend(struct platform_device *pdev, pm_message_t state)
{
    struct hisi_led_drv_data *data = platform_get_drvdata(pdev);
    if (data == NULL) {
        dev_err(&pdev->dev, "%s:data is NULL\n", __func__);
        return -ENODEV;
    }
    printk("[%s] +\n", __func__);

    if (!mutex_trylock(&data->lock)) {
        dev_err(&pdev->dev, "%s: mutex_trylock.\n", __func__);
        return -EAGAIN;
    }
    printk("[%s] -\n", __func__);
    return 0;
}
static int hisi_led_resume(struct platform_device *pdev)
{

    struct hisi_led_drv_data *data = platform_get_drvdata(pdev);
    if (data == NULL) {
        dev_err(&pdev->dev, "%s:data is NULL\n", __func__);
        return -ENODEV;
    }
    printk("[%s] +\n", __func__);
    mutex_unlock(&data->lock);
    printk("[%s] -\n", __func__);
    return 0;
}
#endif

static struct platform_driver hisi_led_driver = {
    .probe        = hisi_led_probe,
    .remove        = hisi_led_remove,
    .shutdown    = hisi_led_shutdown,
#ifdef CONFIG_PM
    .suspend    = hisi_led_suspend,
    .resume        = hisi_led_resume,
#endif
    .driver        = {
        .name    = HISI_LEDS,
        .owner    = THIS_MODULE,
        .of_match_table = of_match_ptr(hi6552_led_match),
    },
};

MODULE_ALIAS("platform:k3-leds");
static int __init hisi_led_init(void)
{
    return platform_driver_register(&hisi_led_driver);
}

static void __exit hisi_led_exit(void)
{
    platform_driver_unregister(&hisi_led_driver);
}

module_init(hisi_led_init);
module_exit(hisi_led_exit);

MODULE_AUTHOR("skf57909");
MODULE_DESCRIPTION("k3 LED driver");
MODULE_LICENSE("GPL");

