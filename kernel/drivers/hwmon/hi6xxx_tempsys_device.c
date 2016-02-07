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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#include "hi6xxx_hkadc_value.h"
#include "hi6xxx_hkadc_ctrl.h"

/*
遗留问题
1. 模块及函数命名
2. sfs get val的 形式，字符串?
3. 以log通道还是物理通道显示给用户?
4, 在DRV_CHANNEL_CONFIG加入通道名字符
*/

#define THERMAL_ERRO        1

#define THERMAL_CLASS_NAME   "thermal"
#define THERMAL_DEVICE_NAME  "temp"

#define TEMPERATOR_DEV_MAX        HKADC_TEMP_BUTT

typedef enum t_TEMP_CONSUMER_SYSID_E
{
    TEMP_CONSUMER_AP_SYSID,
    TEMP_CONSUMER_PA_SYSID,
    TEMP_CONSUMER_CHARGER_SYSID,
    TEMP_CONSUMER_BUTT_SYSID
}TEMP_CONSUMER_SYSID_E;

struct hkadc_consumer_map
{
    unsigned int logic_chan;
    unsigned int sys_id;
    char*  typename;
};

typedef struct drv_hkadc_map tempsys_dev;

extern struct drv_hkadc_map g_hkadc_chan_map[];

static ssize_t tempsys_val_show(struct device * dev,
                    struct device_attribute * attr,
                    char * buf);

static  ssize_t tempsys_val_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count);


static ssize_t tempsys_vol_show(struct device * dev,
                    struct device_attribute * attr,
                    char * buf);

static  ssize_t tempsys_vol_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count);
/*定义: dev_attr_val*/
DEVICE_ATTR(val, S_IRUGO | S_IWUSR, tempsys_val_show, tempsys_val_store);
DEVICE_ATTR(vol, S_IRUGO | S_IWUSR, tempsys_vol_show, tempsys_vol_store);
/*****************************************************************************
 函 数 名  : tempsys_val_show
 功能描述  : 显示某个温度节点的温度值
 输入参数  : dev 标准设备描述符

 输出参数  : 显示出来的字符串长度
 返 回 值  :
*****************************************************************************/
static ssize_t tempsys_val_show(struct device * dev,
                    struct device_attribute * attr,
                    char * buf)
{
    unsigned short volt;
    short temp;
    tempsys_dev* tem_dev = (tempsys_dev*)dev_get_drvdata(dev);

    DRV_HKADC_GET_TEMP(tem_dev->logic_chan, &volt, &temp,0);

    /*返回字符形式的设备温度值*/
    return snprintf(buf, PAGE_SIZE, "%d\n",temp);
}

static ssize_t tempsys_val_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
    return 0;
}
static ssize_t tempsys_vol_show(struct device * dev,
                    struct device_attribute * attr,
                    char * buf)
{   
    unsigned short volt;
    short temp;
    tempsys_dev* tem_dev = (tempsys_dev*)dev_get_drvdata(dev);

    DRV_HKADC_GET_TEMP(tem_dev->logic_chan, &volt, &temp, 0);
    
    /*返回字符形式的设备温度值*/
    return snprintf(buf, PAGE_SIZE, "%d\n",volt);
}

static ssize_t tempsys_vol_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
    return 0;
}

static ssize_t tempsys_type_show(struct device * dev,
                    struct device_attribute * attr,
                    char * buf)
{
    struct hkadc_consumer_map*  hkadc_consumer_chan = (struct hkadc_consumer_map*)dev_get_drvdata(dev);
    return snprintf(buf, PAGE_SIZE, "%s\n",hkadc_consumer_chan->typename);
}

static ssize_t tempsys_temp_show(struct device * dev,
                    struct device_attribute * attr,
                    char * buf)
{
    unsigned short volt;
    short temp;
    struct hkadc_consumer_map*  hkadc_consumer_chan = (struct hkadc_consumer_map*)dev_get_drvdata(dev);
    DRV_HKADC_GET_TEMP(hkadc_consumer_chan->logic_chan, &volt, &temp,0);
    temp /=10;
    return snprintf(buf, PAGE_SIZE, "%d\n",temp);
}

DEVICE_ATTR(type, S_IRUGO, tempsys_type_show, NULL);
DEVICE_ATTR(temp, S_IRUGO, tempsys_temp_show, NULL);

void tempsys_put_devdata(tempsys_dev* tem_dev, int num)
{
    /*mutex_init(&tem_dev->mutex);
    tem_dev->logic_chan = num;*/
}

/*****************************************************************************
 函 数 名  : tempsys_create_device
 功能描述  : 根据hkadc的逻辑通道号列表，创建thermal class下面的sysfs文件节点
 输入参数  : 温保设备类描述

 输出参数  :
 返 回 值  : 成功返回0，失败返回错误号
*****************************************************************************/
int tempsys_create_device(struct class* thermal_class)
{
    struct device * new_dev = NULL;
    int i;
    int ret;
    dev_t devno = 0;
    tempsys_dev * temp_dev;
    char dev_name[256];

    /*创建所有tempsys 温度设备文件*/
    for (i = 0; i < TEMPERATOR_DEV_MAX; i++) {
        temp_dev = bsp_hkadc_get_chan_map(i);
        if (temp_dev) {
           memset(dev_name, 0, sizeof(dev_name));
           snprintf(dev_name,256,"%s:%s", THERMAL_DEVICE_NAME, temp_dev->name);

            /*在设备类别目录创建char型文件节点: /sys/class/thermal/temp:i*/
            new_dev = device_create(thermal_class, NULL, devno, "%s", dev_name);
            if (IS_ERR(new_dev)) {
                ret = PTR_ERR(new_dev);
                printk(KERN_ALERT"failed to create tempsys device.\n");
                return ret;
            }

            /*在下面创建属性文件/sys/class/thermal/temp:i/val */
            ret = device_create_file(new_dev, &dev_attr_val);
            if (ret < 0) {
                printk(KERN_ALERT"failed to create tempsys file.\n");
                return ret;
            }

            /*tempsys_put_devdata(&g_hkadc_chan_map[i], i);*/
            dev_set_drvdata(new_dev,temp_dev);
        }else {
           printk(KERN_ALERT"temp dev %d not find.\n", i);
        }
    }

    return 0;
}

struct hkadc_consumer_map g_hkadc_consumer_map[] = 
{
    {HKADC_TEMP_SYSTEMH,TEMP_CONSUMER_AP_SYSID,"system_h"},
    {HKADC_TEMP_PA0,    TEMP_CONSUMER_PA_SYSID,"pa_0"},
    {HKADC_TEMP_CHARGER_ID,TEMP_CONSUMER_CHARGER_SYSID,"charger"}
};

struct hkadc_consumer_map*  get_hkadc_consumer_by_sysid(TEMP_CONSUMER_SYSID_E temp_sysid)
{
    int i = 0;
    int map_size = sizeof(g_hkadc_consumer_map)/sizeof(g_hkadc_consumer_map[0]);
    
    for(i = 0;i< map_size;i++)
    {
        if(i == temp_sysid)
        {
            return &g_hkadc_consumer_map[i];
        }
    }

    return NULL;   
}

void tempsys_create_consumer_device(struct class* thermal_class)
{
    int i = 0;
    char dev_name[256] ={0};
    struct device * new_dev = NULL;
    dev_t devno = 0;
    int ret = 0;
    struct hkadc_consumer_map*  hkadc_consumer_chan = NULL;

    for(i = 0;i<TEMP_CONSUMER_BUTT_SYSID;i++)
    {
        hkadc_consumer_chan = get_hkadc_consumer_by_sysid(i);
        if(NULL == hkadc_consumer_chan)
        {
            pr_err("%s:sysid is %d get_hkadc_consumer_by_sysid is NULL\n",__func__,i);
            continue; 
        }

        memset(dev_name, 0, sizeof(dev_name));
        snprintf(dev_name,256,"thermal_zone%d", hkadc_consumer_chan->sys_id);
        new_dev = device_create(thermal_class, NULL, devno, "%s", dev_name);
        if (IS_ERR(new_dev)) 
        {
            pr_err("%s:device_create failed \n",__func__);
            continue; 
        }

        dev_set_drvdata(new_dev,(void*)hkadc_consumer_chan);

        ret = device_create_file(new_dev, &dev_attr_type);
        if (ret < 0) 
        {
            pr_err("%s:device_create_file dev_attr_type \n",__func__);
            continue; 
        }

        ret = device_create_file(new_dev, &dev_attr_temp);
        if (ret < 0) 
        {
            pr_err("%s:device_create_file dev_attr_temp \n",__func__);
            continue; 
        }
    }
}
/*****************************************************************************
 函 数 名  : tempsys_device_init
 功能描述  : 温度sysfs节点设备创建初始化
 输入参数  :

 输出参数  :
 返 回 值  :
*****************************************************************************/
static int __init tempsys_device_init(void)
{
    int ret = THERMAL_ERRO;

    struct class * thermal_class;

    /*在 /sys/class/目录下创建温保设备类别目录: thermal*/
    thermal_class = class_create(THIS_MODULE, THERMAL_CLASS_NAME);
    if (IS_ERR(thermal_class)) {
        ret = PTR_ERR(thermal_class);
        printk(KERN_ERR "failed to create thermal class.\n");
        goto destory_class;
    }

    printk(KERN_INFO " initialize thermal device.\n");

    /*在新创建类别里面，创建所有温度显示设备*/
    if (tempsys_create_device(thermal_class)) {
        goto destory_device;
    }

    tempsys_create_consumer_device(thermal_class);

    thermal_data_init();

    printk(KERN_INFO "Succeedded to initialize thermal device.\n");

    return 0;

destory_device:

destory_class:

    return ret;
}

static void __exit tempsys_device_exit(void)
{
    
}

module_init(tempsys_device_init);
module_exit(tempsys_device_exit);

MODULE_AUTHOR("  <@huawei.com>");
MODULE_DESCRIPTION("hi6xxx tempator sysfs interface");
MODULE_LICENSE("GPL");

