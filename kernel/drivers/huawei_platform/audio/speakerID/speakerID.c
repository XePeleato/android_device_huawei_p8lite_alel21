/*
 * speakerID.c -- speakerID driver
 *
 * Copyright (c) 2014 Huawei Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 #include <linux/init.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/regulator/consumer.h>
#include <linux/string.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/huawei/hisi_adc.h>

#define LOG_TAG "speakerID"

#define SPEAKER_ID_NAME    "speakerID"
#define ADC_READ_COUNT     3
#define SPEAKER_ID_LIMIT   500

#define SPEAKER_ID_HIGH    1
#define SPEAKER_ID_LOW   2

struct speaker_id{
    int adc_channel;
    int spk_id_limit;
};

struct speaker_id spk_id;

static int adc_read()
{
    int value;
    int loop = ADC_READ_COUNT;

    while(loop) {
        loop--;
        value = hisi_adc_get_value(spk_id.adc_channel);
        pr_info("adc_read adc:%d\n", value);
        if(value < 0) {
            pr_err("%s(%u) : get hkadc error err = %d\n", __FUNCTION__, __LINE__, value);
            continue;
        }
        break;
    }
    // read err default as SPEAKER_ID_LOW
    if(value < spk_id.spk_id_limit)
        return SPEAKER_ID_LOW;
    else
        return SPEAKER_ID_HIGH;
}


static ssize_t speakerID_read(struct file *file, char __user *user_buf,
                            size_t count, loff_t *ppos)
{
    char buf[32] = {0};

    int id = adc_read();
    pr_info("speakerID_read id:%d\n", id);

    snprintf(buf, sizeof(buf), "%d", id);
    return simple_read_from_buffer(user_buf, count, ppos, buf, strlen(buf));
}

static const struct of_device_id speaker_id_match[] = {
    { .compatible = "huawei,speaker_id", },
    {},
};
MODULE_DEVICE_TABLE(of, speaker_id_match);

static const struct file_operations speakerID_fops = {
    .owner          = THIS_MODULE,
    .open           = simple_open,
    .read           = speakerID_read,
};

static struct miscdevice speakerID_device = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = SPEAKER_ID_NAME,
    .fops   = &speakerID_fops,
};


static int speakerID_probe(struct platform_device *pdev)
{
    int ret = -ENODEV;
    const struct of_device_id *match;
    struct device_node *node;
    int temp;

    match = of_match_device(speaker_id_match, &pdev->dev);
    if (!match) {
        pr_err("get speaker_id device info err\n");
        return -ENOENT;
    }
    node = pdev->dev.of_node;

    /* get channel id */
    if (!of_property_read_u32(node, "channel", &temp)) {
        spk_id.adc_channel= temp;
    } else {
        pr_err("get adc channel failed, set default value\n");
        spk_id.adc_channel = 5;
    }
    /*get speakerID limit value*/
    if (!of_property_read_u32(node, "spk_id_limit", &temp)) {
        spk_id.spk_id_limit= temp;
        pr_info("get spk_id_limit from dts:%d\n", temp);
    } else {
        pr_err("get spk_id_limit failed, set default value\n");
        spk_id.spk_id_limit = SPEAKER_ID_LIMIT;
    }

    ret = misc_register(&speakerID_device);
    if (ret) {
        pr_err("%s: speakerID_device register failed", __FUNCTION__);
        return ret;
    }

    return ret;
}

static int speakerID_remove(struct platform_device *pdev)
{
    return 0;
}


static struct platform_driver speakerID_driver = {
    .driver = {
        .name  = SPEAKER_ID_NAME,
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(speaker_id_match),
    },
    .probe  = speakerID_probe,
    .remove = speakerID_remove,
};

static int __init speakerID_init(void)
{
    return platform_driver_register(&speakerID_driver);
}

static void __exit speakerID_exit(void)
{
    platform_driver_unregister(&speakerID_driver);
}

module_init(speakerID_init);
module_exit(speakerID_exit);

MODULE_DESCRIPTION("speakerID driver");
MODULE_LICENSE("GPL");