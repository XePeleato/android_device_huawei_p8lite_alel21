/*
 * drivers/power/smpl/smpl.c
 * Copyright (C) 2012-2015 HUAWEI, Inc.
 * Author: HUAWEI, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <huawei_platform/log/hw_log.h>
#include <huawei_platform/dsm/dsm_pub.h>

#define ERROR_NO_SMPL 10420
#define HWLOG_TAG smpl
HWLOG_REGIST();
static struct dsm_dev dsm_smpl = {
    .name = "dsm_smpl",
    .fops = NULL,
    .buff_size = 1024,
};
static struct dsm_client *smpl_client = NULL;

struct smpl_device_info {
    struct device   *dev;
    struct delayed_work smpl_delayed_work;
};

static unsigned int smpl_happened = 0;
static int __init early_parse_normal_reset_type_cmdline(char * p)
{
    if (p)
    {
        if (!strncmp(p,"SMPL", 5))
        {
            smpl_happened = 1;
        }
    }
    hwlog_info("smpl happened = %d\n", smpl_happened);
    return 0;
}
early_param("normal_reset_type", early_parse_normal_reset_type_cmdline);

static void smpl_work(struct work_struct *work)
{
    if (smpl_happened)
    {
        if (!dsm_client_ocuppy(smpl_client))
        {
            hwlog_info("smpl record and notify\n");
            dsm_client_record(smpl_client, "smpl happened!\n");
            dsm_client_notify(smpl_client, ERROR_NO_SMPL);
        }
    }
}
static int smpl_probe(struct platform_device *pdev)
{
    struct device_node* np;
    struct smpl_device_info* di;

    np = pdev->dev.of_node;
    if(NULL == np)
    {
        hwlog_err("np is NULL\n");
        return -1;
    }
    di = kzalloc(sizeof(*di), GFP_KERNEL);
    if (!di)
    {
        hwlog_err("di is NULL\n");
        return -ENOMEM;
    }
    if (!smpl_client)
    {
        smpl_client = dsm_register_client(&dsm_smpl);
    }
    if (NULL == smpl_client)
    {
        hwlog_err("smpl register dsm fail\n");
        return -1;
    }
    INIT_DELAYED_WORK(&di->smpl_delayed_work, smpl_work);
    schedule_delayed_work(&di->smpl_delayed_work, 10);
    hwlog_info("smpl probe ok!\n");
    return 0;
}

static struct of_device_id smpl_match_table[] =
{
    {
        .compatible = "huawei,smpl",
        .data = NULL,
    },
    {
    },
};
static struct platform_driver smpl_driver = {
    .probe = smpl_probe,
    .driver = {
        .name = "huawei,smpl",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(smpl_match_table),
    },
};

static int __init smpl_init(void)
{
    return platform_driver_register(&smpl_driver);
}

module_init(smpl_init);

static void __exit smpl_exit(void)
{
    platform_driver_unregister(&smpl_driver);
}

module_exit(smpl_exit);

MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:smpl");
MODULE_AUTHOR("HUAWEI Inc");
