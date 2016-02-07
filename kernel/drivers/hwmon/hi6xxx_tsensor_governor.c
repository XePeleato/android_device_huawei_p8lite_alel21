/*
 * Copyright (c) 2011 Hisilicon Technologies Co., Ltd. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/err.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/stddef.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/reboot.h>
#include <linux/slab.h>
#include <linux/timer.h>
//#include <linux/pm_qos_params.h>
#include <linux/pm_qos.h>
#include <asm/irq.h>
#include <linux/of.h>
#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#endif

#include "hi6xxx_tsensor_driver.h"
#include "soc_baseaddr_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_ao_sctrl_interface.h"

#define  DRIVER_NAME	"hi6xxx-tsensor"

struct tsensor_governor *tsensor_gov = NULL;

/*
 * sysfs hook functions
 */
static ssize_t show_acpu_c0_temp(struct device *dev,
                    struct device_attribute *devattr, char *buf)
{
    int temperature = 0;
    int ret = 0;
    ret = tsensor_temp_get(TSENSOR_TYPE_ACPU_CLUSTER0,&temperature);
    if(TSENSOR_OK != ret)
    {
        return sprintf(buf, "Error\n");
    }
    return snprintf(buf, sizeof(int), "%d\n", temperature);
}

static ssize_t show_acpu_c1_temp(struct device *dev,
                    struct device_attribute *devattr, char *buf)
{
    int temperature = 0;
    int ret = 0;
    ret = tsensor_temp_get(TSENSOR_TYPE_ACPU_CLUSTER1,&temperature);
    if(TSENSOR_OK != ret)
    {
        return sprintf(buf, "Error\n");
    }
    return snprintf(buf, sizeof(int), "%d\n", temperature);
}

static ssize_t show_gpu_temp(struct device *dev,
                    struct device_attribute *devattr, char *buf)
{
    int temperature = 0;
    int ret = 0;
    ret = tsensor_temp_get(TSENSOR_TYPE_GPU,&temperature);
    if(TSENSOR_OK != ret)
    {
        return sprintf(buf, "Error\n");
    }
    return snprintf(buf, sizeof(int), "%d\n", temperature);
}

static ssize_t show_local_temp(struct device *dev,
                    struct device_attribute *devattr, char *buf)
{
    int temperature = 0;
    int ret = 0;
    ret = tsensor_temp_get(TSENSOR_TYPE_LOCAL,&temperature);
    if(TSENSOR_OK != ret)
    {
        return sprintf(buf, "Error\n");
    }
    return snprintf(buf, sizeof(int), "%d\n", temperature);
}

static ssize_t tsensor_debug_switch(struct device *dev, struct device_attribute *attr,const char *buf, size_t n)
{
    long val;
    if ((strict_strtol(buf, 10, &val) < 0))
        return -EINVAL;

    if((val == 0) || (val == 1))
    {
        tsensor_gov->debug_switch = val;
    }

    return n;
}

static ssize_t show_warning_cnt(struct device *dev,
                    struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "Warning count : %d\n", tsensor_gov->warning_count);
}

static ssize_t tsensor_onoff(struct device *dev, struct device_attribute *attr,const char *buf, size_t n)
{
    long val;
    if ((strict_strtol(buf, 10, &val) < 0))
        return -EINVAL;

    tsensor_switch(val);

    return n;
}

static ssize_t show_onoff(struct device *dev,
                    struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "Tsensor Status : %d\n", tsensor_status());
}

/*注册sysfs接口*/
static DEVICE_ATTR(acpu_c0_temp, S_IRUSR|S_IRGRP, show_acpu_c0_temp, NULL);
static DEVICE_ATTR(acpu_c1_temp, S_IRUSR|S_IRGRP, show_acpu_c1_temp, NULL);
static DEVICE_ATTR(gpu_temp, S_IRUSR|S_IRGRP, show_gpu_temp, NULL);
static DEVICE_ATTR(local_temp, S_IRUSR|S_IRGRP, show_local_temp, NULL);
static DEVICE_ATTR(debug_sw, S_IWUSR|S_IWGRP, NULL, tsensor_debug_switch);
static DEVICE_ATTR(warning_cnt, S_IRUSR|S_IRGRP, show_warning_cnt, NULL);
static DEVICE_ATTR(onoff, S_IRWXU|S_IRGRP, show_onoff,tsensor_onoff);

static struct attribute *tsensor_attributes[] = {
    &dev_attr_acpu_c0_temp.attr,
    &dev_attr_acpu_c1_temp.attr,
    &dev_attr_gpu_temp.attr,
    &dev_attr_local_temp.attr,
    &dev_attr_debug_sw.attr,
    &dev_attr_warning_cnt.attr,
    &dev_attr_onoff.attr,
    NULL
};

static const struct attribute_group tsensor_group = {
    .attrs = tsensor_attributes,
};

/*****************************************************************************
 函 数 名  : hi6xxx_tsensor_probe
 功能描述  : tsensor driver probe函数
 输入参数  :
             pdev:设备句柄
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月6日
    作    者   : 刘龙 00217270
    修改内容   : 新生成函数

*****************************************************************************/

static int hi6xxx_tsensor_probe(struct platform_device *pdev)
{
    struct tsensor_devinfo *devinfo = NULL;
	struct class * charge_class = NULL;
	struct device * new_dev = NULL;
    int ret = 0;

    printk(KERN_INFO "hi6xxx_tsensor_probe\n");

    /*全局变量初始化 */
    tsensor_gov = kzalloc(sizeof(struct tsensor_governor), GFP_KERNEL);
    if (!tsensor_gov) {
        pr_err("%s:hi6xxx_tsensor_probe kzalloc failed\n", __func__);
        return -ENOMEM;
    }

    devinfo = kzalloc(sizeof(struct tsensor_devinfo), GFP_KERNEL);
    if (!devinfo) {
        pr_err("%s:hi6xxx_tsensor_probe kzalloc failed\n", __func__);
        goto probe_err;
    }
    devinfo->pdev = pdev;
    devinfo->dev = &pdev->dev;

    kobject_uevent(&pdev->dev.kobj, KOBJ_ADD);
    platform_set_drvdata(pdev, devinfo);

    /*创建sysfs节点 */
    ret = sysfs_create_group(&pdev->dev.kobj, &tsensor_group);
    if (ret) {
        dev_err(&pdev->dev, "hi6xxx_tsensor_probe sysfs_create_group failed\n");
        goto probe_err;
    }

	charge_class = class_create(THIS_MODULE, "hw_tsensor");
	new_dev = device_create(charge_class, NULL, 0, "%s",
                            "tsensor");
	ret = sysfs_create_link(&new_dev->kobj, &pdev->dev.kobj, "cpu_temperature");

    INIT_DELAYED_WORK(&tsensor_gov->tsensor_monitor_work,
             tsensor_monitor_work_fn);
    tsensor_gov->average_period = TSENSOR_NORMAL_MONITORING_RATE;
    tsensor_gov->debug_switch = TSENSOR_DEBUG_OFF;
    tsensor_gov->warning_count = 0;
    mutex_init(&(tsensor_gov->lock));

    /*tsensor初始化 */
    if (pdev->dev.of_node)
    {
		ret = tsensor_init(pdev);
		if (ret != 0)
		{
			dev_err(&pdev->dev, "tsensor_init\n");
			goto probe_err;
		}
        
		tsensor_late_init();
    }
    else
    {
    	ret = -ENODEV;
		goto probe_err;
    }

    printk(KERN_INFO "hi6xxx_tsensor_probe probe success\n");

    return ret;

probe_err:

    platform_set_drvdata(pdev, NULL);
    if (devinfo) {
        kfree(devinfo);
        devinfo = NULL;
    }
    if (tsensor_gov) {
        kfree(tsensor_gov);
        tsensor_gov = NULL;
    }
    return ret;
}

#ifdef CONFIG_PM
/*****************************************************************************
 函 数 名  : hi6xxx_tsensor_suspend
 功能描述  : tsensor driver suspend函数
 输入参数  :
             pdev:设备句柄
             state:pm状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月6日
    作    者   : 刘龙 00217270
    修改内容   : 新生成函数

*****************************************************************************/
static int  hi6xxx_tsensor_suspend(struct platform_device *pdev, pm_message_t state)
{
    return tsensor_suspend();
}

/*****************************************************************************
 函 数 名  : hi6xxx_tsensor_resume
 功能描述  : tsensor driver resume函数
 输入参数  :
             pdev:设备句柄
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月6日
    作    者   : 刘龙 00217270
    修改内容   : 新生成函数

*****************************************************************************/

static int hi6xxx_tsensor_resume(struct platform_device *pdev)
{
    return tsensor_resume();
}
#endif

/*****************************************************************************
 函 数 名  : hi6xxx_tsensor_remove
 功能描述  : tsensor driver remote函数
 输入参数  :
             pdev:设备句柄
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月6日
    作    者   : 刘龙 00217270
    修改内容   : 新生成函数

*****************************************************************************/
static int hi6xxx_tsensor_remove(struct platform_device *pdev)
{
    struct tsensor_devinfo *devinfo = platform_get_drvdata(pdev);

    printk(KERN_INFO"hi6xxx_tsensor_remove\n");

    if (devinfo == NULL) {
        dev_err(&pdev->dev, "devinfo is null\n");
        return -ENODEV;
    }

    /*取消定时监控 */
    cancel_delayed_work_sync(&tsensor_gov->tsensor_monitor_work);

    /*tsensor挂起*/
    tsensor_suspend();

    kfree(devinfo);

    kfree(tsensor_gov);
    tsensor_gov = NULL;

    platform_set_drvdata(pdev, NULL);

    return 0;
}

/*****************************************************************************
 函 数 名  : hi6xxx_tsensor_shutdown
 功能描述  : tsensor driver shutdown函数
 输入参数  :
             pdev:设备句柄
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月6日
    作    者   : 刘龙 00217270
    修改内容   : 新生成函数

*****************************************************************************/
static void hi6xxx_tsensor_shutdown(struct platform_device *pdev)
{
    struct tsensor_devinfo *devinfo = platform_get_drvdata(pdev);

    printk(KERN_INFO"hi6xxx_tsensor_shutdown\n");

    if (devinfo == NULL) {
        dev_err(&pdev->dev, "devinfo is null\n");
        return ;
    }

    cancel_delayed_work_sync(&tsensor_gov->tsensor_late_init_work);

    /*取消tsensor温度监控*/
    cancel_delayed_work_sync(&tsensor_gov->tsensor_monitor_work);

    /*tsensor挂起 */
    tsensor_suspend();

    kfree(devinfo);
    devinfo = NULL;

    platform_set_drvdata(pdev, NULL);

    printk("hi6xxx_tsensor_shutdown\n");

    return;
}

static struct of_device_id tsensors_match[] = {
	{
        .compatible = "hisilicon,hi6xxx-tsensor-driver",
	},
	{}
};


/*tsensor platform driver信息 */
static struct platform_driver hi6xxx_tsensor_driver = {
    .probe     = hi6xxx_tsensor_probe,
#ifdef CONFIG_PM
    .suspend   = hi6xxx_tsensor_suspend,
    .resume    = hi6xxx_tsensor_resume,
#endif
    .remove    = hi6xxx_tsensor_remove,
    .shutdown  = hi6xxx_tsensor_shutdown,
    .driver  = {
    	.owner   = THIS_MODULE,
    	.name    = DRIVER_NAME,
    	.of_match_table = tsensors_match,
    },
};

/*****************************************************************************
 函 数 名  : hi6xxx_tsensor_init
 功能描述  : tsensor driver初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月6日
    作    者   : 刘龙 00217270
    修改内容   : 新生成函数

*****************************************************************************/
static int __init hi6xxx_tsensor_init(void)
{
    int ret;

    ret = platform_driver_register(&hi6xxx_tsensor_driver);
    if (ret < 0)
    {
        printk(KERN_ERR"hi6xxx platform_driver_register ret %d\n", ret);

      	 return ret;
    }

    printk(KERN_INFO"hi6xxx_tsensor_init ret %d\n", ret);

    return ret;
}

/*****************************************************************************
 函 数 名  : hi6xxx_tsensor_exit
 功能描述  : tsensor driver退出函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月6日
    作    者   : 刘龙 00217270
    修改内容   : 新生成函数

*****************************************************************************/
static void __exit hi6xxx_tsensor_exit(void)
{
    printk(KERN_INFO"hi6xxx_tsensor_exit.\n");
    platform_driver_unregister(&hi6xxx_tsensor_driver);
}

late_initcall(hi6xxx_tsensor_init);
module_exit(hi6xxx_tsensor_exit);

MODULE_DESCRIPTION("Hi6xxx Tsensor Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("<liulong@huawei.com>");

