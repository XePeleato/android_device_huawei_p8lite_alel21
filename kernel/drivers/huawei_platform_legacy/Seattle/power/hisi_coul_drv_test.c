/*****************************************************************************************
* Filename:	hisi_coul_drv_test.c
*
* Discription:  this file provide sys  interface to set handle  battery state such as capacity, voltage,current,
*		        temperature.
* Copyright:	(C) 2014 huawei.
*
* revision history:
* 04/28/14 yuanqinshun -v1.0
*
******************************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <hisi_coul_drv_test.h>
#include <linux/platform_device.h>
#include <linux/device.h> 
#include <linux/of.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

struct hisi_coul_test_info *g_hisi_coul_test_info = NULL;

static ssize_t hisi_coul_set_test_start_flag(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 16, &val) < 0) || (val < 0) )
        return -EINVAL;
    di->test_start_flag = val;
    return status;
}

static ssize_t hisi_coul_show_test_start_flag(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->test_start_flag;
    return sprintf(buf, "%lu\n", val);
}

static ssize_t hisi_coul_set_input_batt_exist(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;
    di->input_batt_exist = val;
    return status;
}

static ssize_t hisi_coul_show_input_batt_exist(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->input_batt_exist;
    return sprintf(buf, "%lu\n", val);
}

static ssize_t hisi_coul_set_input_batt_capacity(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 100))
        return -EINVAL;
    di->input_batt_capacity = val;
    return status;
}

static ssize_t hisi_coul_show_input_batt_capacity(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->input_batt_capacity;
    return sprintf(buf, "%lu\n", val);
}

static ssize_t hisi_coul_set_input_batt_temp(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < -40) || (val > 80))
        return -EINVAL;
    di->input_batt_temp = val;
    return status;
}

static ssize_t hisi_coul_show_input_batt_temp(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->input_batt_temp;
    return sprintf(buf, "%lu\n", val);
}

static ssize_t hisi_coul_set_input_batt_full(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;
    di->input_batt_full = val;
    return status;
}

static ssize_t hisi_coul_show_input_batt_full(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->input_batt_full;
    return sprintf(buf, "%lu\n", val);
}

static ssize_t hisi_coul_set_input_batt_volt(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 2000) || (val > 5000))
        return -EINVAL;
    di->input_batt_volt = val;
    return status;
}

static ssize_t hisi_coul_show_input_batt_volt(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->input_batt_volt;
    return sprintf(buf, "%lu\n", val);
}

static ssize_t hisi_coul_set_input_batt_cur(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < -10000) || (val > 10000))
        return -EINVAL;
    di->input_batt_cur = val;
    return status;
}

static ssize_t hisi_coul_show_input_batt_cur(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->input_batt_cur;
    return sprintf(buf, "%lu\n", val);
}

static ssize_t hisi_coul_set_input_batt_fcc(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 10000))
        return -EINVAL;
    di->input_batt_fcc = val;
    return status;
}

static ssize_t hisi_coul_show_input_batt_fcc(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->input_batt_fcc;
    return sprintf(buf, "%lu\n", val);
}


static ssize_t hisi_coul_set_input_event(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 16, &val) < 0) || (val < 0) || (val > 10000))
        return -EINVAL;
    di->input_event = val;
    return status;
}

static ssize_t hisi_coul_show_input_event(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hisi_coul_test_info *di = dev_get_drvdata(dev);

    val = di->input_event;
    return sprintf(buf, "%lu\n", val);
}

static DEVICE_ATTR(test_start_flag, S_IWUSR | S_IRUGO,
                  hisi_coul_show_test_start_flag,
                  hisi_coul_set_test_start_flag);
static DEVICE_ATTR(input_batt_exist, S_IWUSR | S_IRUGO,
                  hisi_coul_show_input_batt_exist,
                  hisi_coul_set_input_batt_exist);
static DEVICE_ATTR(input_batt_capacity, S_IWUSR | S_IRUGO,
                  hisi_coul_show_input_batt_capacity,
                  hisi_coul_set_input_batt_capacity);
static DEVICE_ATTR(input_batt_temp, S_IWUSR | S_IRUGO,
                  hisi_coul_show_input_batt_temp,
                  hisi_coul_set_input_batt_temp);
static DEVICE_ATTR(input_batt_full, S_IWUSR | S_IRUGO,
                  hisi_coul_show_input_batt_full,
                  hisi_coul_set_input_batt_full);
static DEVICE_ATTR(input_batt_volt, S_IWUSR | S_IRUGO,
                  hisi_coul_show_input_batt_volt,
                  hisi_coul_set_input_batt_volt);
static DEVICE_ATTR(input_batt_cur, S_IWUSR | S_IRUGO,
                  hisi_coul_show_input_batt_cur,
                  hisi_coul_set_input_batt_cur);
static DEVICE_ATTR(input_batt_fcc, S_IWUSR | S_IRUGO,
                  hisi_coul_show_input_batt_fcc,
                  hisi_coul_set_input_batt_fcc);
static DEVICE_ATTR(input_event, S_IWUSR | S_IRUGO,
                  hisi_coul_show_input_event,
                  hisi_coul_set_input_event);

static struct attribute *hisi_coul_attributes[] = {
    &dev_attr_test_start_flag.attr,
    &dev_attr_input_batt_exist.attr,
    &dev_attr_input_batt_capacity.attr,
    &dev_attr_input_batt_temp.attr,
    &dev_attr_input_batt_full.attr,
    &dev_attr_input_batt_volt.attr,
    &dev_attr_input_batt_cur.attr,
    &dev_attr_input_batt_fcc.attr,
    &dev_attr_input_event.attr,
    NULL,
};

static const struct attribute_group hisi_coul_attr_group = {
    .attrs = hisi_coul_attributes,
};

static int hisi_coul_drv_test_probe(struct platform_device *pdev)
{
    struct hisi_coul_test_info *di = NULL;
    struct class * hisi_coul_class;
    struct device * new_dev = NULL;
    int ret = 0;

    di = (struct hisi_coul_test_info *)kzalloc(sizeof(struct hisi_coul_test_info), GFP_KERNEL);
    if (!di)
    {
        return -ENOMEM;
    }
    g_hisi_coul_test_info = di;
    dev_set_drvdata(&pdev->dev,di);
    di->test_start_flag = 0;
    di->input_batt_capacity = 50;
    di->input_batt_cur = 500;
    di->input_batt_exist = 1;
    di->input_batt_fcc = 2000;
    di->input_batt_full = 0;
    di->input_batt_temp = 25;
    di->input_batt_volt = 3800;

    ret = sysfs_create_group(&pdev->dev.kobj, &hisi_coul_attr_group);
    if (ret) {
        printk(KERN_DEBUG"could not create sysfs files\n");
        goto err_sysfs;
    }

    hisi_coul_class = class_create(THIS_MODULE, "hisi_coul_drv_test");
    new_dev = device_create(hisi_coul_class, NULL, 0, "%s",
                            "hisi_coul");
    ret = sysfs_create_link(&new_dev->kobj, &pdev->dev.kobj, "hisi_coul_test_info");
    if(ret < 0){
        printk(KERN_DEBUG"create link to charge data fail.\n");
    }
    printk(KERN_DEBUG"hisi_coul_drv_test_probe ok!");
    return 0;

err_sysfs:
    kfree(di);
    return -1;
}

static int hisi_coul_drv_test_remove(struct platform_device *pdev)
{
    struct hisi_coul_test_info *di = dev_get_drvdata(&pdev->dev);
    sysfs_remove_group(&pdev->dev.kobj, &hisi_coul_attr_group);
    if(di == NULL)
        return -ENOMEM;

    kfree(di);
    return 0;
}

static void hisi_coul_drv_test_shutdown(struct platform_device *pdev)
{
    struct hisi_coul_test_info *di = dev_get_drvdata(&pdev->dev);

    printk(KERN_DEBUG"%s ++\n",__func__);
    if (di == NULL) {
        printk(KERN_ERR"di is NULL!\n");
        return;
    }
    printk(KERN_DEBUG"%s --\n",__func__);
    return;
}

static struct of_device_id hisi_coul_drv_test_match_table[] =
{
    {
    .compatible = "hisi_coul_drv_test",
    .data = NULL,
    },
    {
    },
};

static struct platform_driver hisi_coul_drv_test_driver = {
    .probe = hisi_coul_drv_test_probe,
    .remove = hisi_coul_drv_test_remove,
    .shutdown = hisi_coul_drv_test_shutdown,
    .driver = {
       .name = "hisi_coul_drv_test",
	.owner = THIS_MODULE,
	.of_match_table = of_match_ptr(hisi_coul_drv_test_match_table),
    },
};

int __init hisi_coul_test_init(void)
{
    printk(KERN_INFO"hisi_coul_test_init\n");
    return (platform_driver_register(&hisi_coul_drv_test_driver));
}
module_init(hisi_coul_test_init);


void __exit hisi_coul_test_exit(void)
{
    platform_driver_unregister(&hisi_coul_drv_test_driver);
}

module_exit(hisi_coul_test_exit);

MODULE_AUTHOR("HUAWEI");
MODULE_DESCRIPTION("hisi coul test module driver");
MODULE_LICENSE("GPL");
