/*
 *
 * Copyright (C) 2013 HUAWEI, Inc.
 * Author: wangliang <bill.wangliang@huawei.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/device.h>
#include <linux/sysfs.h>

#include <asm/io.h>
#include <linux/wakelock.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>
#include <huawei_platform/log/hw_log.h>
#include<linux/of.h>

#ifndef VOS_ZOS
#define VOS_ZOS                        2
#endif

#ifndef VOS_OS_VER
#define VOS_OS_VER                      VOS_ZOS
#endif

#ifndef VOS_WIN32
#define VOS_WIN32                      1
#endif

#include <RnicConfigInterface.h>

#undef HWLOG_TAG
#define HWLOG_TAG rnic_adapter
HWLOG_REGIST();

#define DRIVER_VERSION        "1.0.0"

#define DTS_COMP_RNIC_ADAPTER_NAME "rnic_adapter_interface_dev"

/* The struct of the Config, the lenth of this Struct is 8.
typedef struct
{
    RNIC_MODEM_TYPE_ENUM_UINT8          enModemType;
    unsigned char                       ucRmNetId;
    unsigned short                      usModemId;
    unsigned char                       ucPdnId;
    unsigned char                       ucRabId;
    RNIC_RMNET_STATUS_ENUM_UINT8        enRmnetStatus;
    RNIC_IP_TYPE_ENUM_UINT8             enIpType;
}RNIC_RMNET_CONFIG_STRU;
*/
#define RNIC_CONFIG_LENTH       (8)

static RNIC_RMNET_CONFIG_STRU RnicConfig = {0};

static int rnic_adapter_pf_probe(struct platform_device *pdev);
static int rnic_adapter_pf_remove(struct platform_device *dev);

int rnic_adapter_pf_remove(struct platform_device *dev)
{
    hwlog_info("called remove %s\n", __func__);
    return 0;
}

static ssize_t store_config(struct device *dev,
                struct device_attribute *attr, const char *buf, size_t count)
{

    if (count != RNIC_CONFIG_LENTH) {
        hwlog_info("RNIC__ %s count invalible count = %d\n", __func__, count);
        if (count < RNIC_CONFIG_LENTH) {
            return count;
        }
    }

    memcpy(&RnicConfig, buf, RNIC_CONFIG_LENTH);

    hwlog_info("RNIC__ %s enable value %s \n", __func__, buf);
    RNIC_ConfigRmnetStatus(&RnicConfig);
    return count;
}

static ssize_t show_config(struct device *pdev, struct device_attribute *attr, char *buf)
{
    hwlog_info("called show %s\n", __func__);
    memcpy(buf, &RnicConfig, RNIC_CONFIG_LENTH);
    return RNIC_CONFIG_LENTH;
}

static DEVICE_ATTR(rnic_adapter, S_IWUSR, show_config, store_config);

static struct attribute *rnic_adapter_attributes[] = {
        &dev_attr_rnic_adapter.attr,
        NULL
};

static const struct attribute_group rnic_adapter_attr_group = {
        .attrs = rnic_adapter_attributes,
};

static struct platform_device rnic_adapter_dev = {
    .name = "rnic_adapter_interface",
};

int rnic_adapter_pf_probe(struct platform_device *pdev)
{
    int ret = 0;

    hwlog_info("RNIC__called probe %s\n", __func__);

    ret = platform_device_register(&rnic_adapter_dev);
    if (ret) {
        hwlog_err( "%s: platform_device_register failed, ret:%d.\n",
                __func__, ret);
        goto err_register_platform_device;
    }

    ret =  sysfs_create_group(&rnic_adapter_dev.dev.kobj, &rnic_adapter_attr_group);
    if (ret){
        hwlog_err("RNIC__ %s rnic_adapter_pf_probe. \n", __func__);
        platform_device_unregister(&rnic_adapter_dev);
    }

err_register_platform_device:
    return ret;
}

static const struct of_device_id rnic_adapter_match_table[] = {
    {
        .compatible = DTS_COMP_RNIC_ADAPTER_NAME,
        .data = NULL,
    },
    {
    },
};
MODULE_DEVICE_TABLE(of, rnic_adapter_match_table);

static struct platform_driver rnic_adapter_driver = {
    .probe = rnic_adapter_pf_probe,
    .remove = rnic_adapter_pf_remove,

    .driver = {
        .name = "rnic_adapter_interface_dev",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(rnic_adapter_match_table),
    },

};

static int rnic_adapter_init(void)
{
    int err = 0;

    err = platform_driver_register(&rnic_adapter_driver);
    if (err){
        hwlog_err("RNIC__ %s rnic_adapter_init.\n", __func__);
    }

    return err;
}

static void __exit rnic_adapter_exit(void)
{
    platform_driver_unregister(&rnic_adapter_driver);
}

module_init(rnic_adapter_init);
module_exit(rnic_adapter_exit);
