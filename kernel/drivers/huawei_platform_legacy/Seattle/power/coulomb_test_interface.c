/*****************************************************************************************
* Filename:    coulomb_interface.c
*
* Discription:  coulometer interface functions.
* Copyright:    (C) 2014 huawei.
*
* revision history:
* 
*
******************************************************************************************/
#include "coulomb_test_interface.h"

struct coulomb_test_interface_info *g_coulomb_test_interface_info = NULL;
static struct hi6421_pmic *pmic = NULL;

unsigned char coulomb_reg_read(unsigned int regaddr)
{
    unsigned char val = hi6421_pmic_read(pmic,regaddr);
    return val;
}
void coulomb_reg_write(unsigned int regaddr,unsigned char val)
{
    hi6421_pmic_write(pmic,regaddr,val);
}

void coulomb_regs_read(unsigned int regaddr,unsigned char* buf,int size)
{
    int i = 0;
    unsigned int addr = regaddr;
    while(i < size)
    {
        *buf++ = hi6421_pmic_read(pmic,addr++);
        ++i;
    }
}
void coulomb_regs_write(unsigned int regaddr,unsigned char* buf,int size)
{
    int i = 0;
    unsigned int addr = regaddr;
    while(i < size)
    {
        hi6421_pmic_write(pmic,addr++,*buf++);
        ++i;
    }
}

static ssize_t coulomb_test_show_version(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;

    val = COULOMB_REG_READ(COULOMB_COUL_VERSION_ADDR);
    return sprintf(buf, "0x%x\n", val);
}

static ssize_t coulomb_test_show_debug_reg(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;

    val = COULOMB_REG_READ(COULOMB_DEBUG_REG);
    return sprintf(buf, "0x%x\n", val);
}

static ssize_t coulomb_test_show_vol_offset_mod(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;

    COULOMB_REGS_READ(COULOMB_VOL_OFFSET_MOD, &val, 2);
    return sprintf(buf, "0x%x\n", val);
}

static ssize_t coulomb_test_show_cur_offset_mod(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;

    COULOMB_REGS_READ(COULOMB_CUR_OFFSET_MOD, &val, 2);
    return sprintf(buf, "0x%x\n", val);
}

static ssize_t coulomb_test_show_new_battery(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;
    unsigned int result = 1;

    val = COULOMB_REG_READ(COULOMB_BATTERY_MOVE_ADDR);

    if(val == BATTERY_MOVE_MAGIC_NUM)
    {
        result = 0;
    }

    return sprintf(buf, "%u\n", result);
}

static ssize_t coulomb_test_show_coul_ctrl(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;

    val = COULOMB_REG_READ(COULOMB_COUL_CTRL_REG);
    return sprintf(buf, "0x%x\n", val);
}

static ssize_t coulomb_test_show_low_int(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;
    struct coulomb_test_interface_info *di = dev_get_drvdata(dev);

    val = di->low_int;
    return sprintf(buf, "0x%x\n", val);
}

static ssize_t coulomb_test_show_calib_a(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;
    struct coulomb_test_interface_info *di = dev_get_drvdata(dev);

    val = di->v_offset_a;
    return sprintf(buf, "0x%x\n", val);
}

static ssize_t coulomb_test_show_calib_b(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val = 0;
    struct coulomb_test_interface_info *di = dev_get_drvdata(dev);

    val = di->v_offset_b;
    return sprintf(buf, "0x%x\n", val);
}

static DEVICE_ATTR(version, S_IWUSR | S_IRUGO,
                  coulomb_test_show_version,
                  NULL);
static DEVICE_ATTR(debug_reg, S_IWUSR | S_IRUGO,
                  coulomb_test_show_debug_reg,
                  NULL);
static DEVICE_ATTR(vol_offset_mod, S_IWUSR | S_IRUGO,
                  coulomb_test_show_vol_offset_mod,
                  NULL);
static DEVICE_ATTR(cur_offset_mod, S_IWUSR | S_IRUGO,
                  coulomb_test_show_cur_offset_mod,
                  NULL);
static DEVICE_ATTR(new_battery, S_IWUSR | S_IRUGO,
                  coulomb_test_show_new_battery,
                  NULL);
static DEVICE_ATTR(coul_ctrl, S_IWUSR | S_IRUGO,
                  coulomb_test_show_coul_ctrl,
                  NULL);
static DEVICE_ATTR(low_int, S_IWUSR | S_IRUGO,
                  coulomb_test_show_low_int,
                  NULL);
static DEVICE_ATTR(calib_a, S_IWUSR | S_IRUGO,
                  coulomb_test_show_calib_a,
                  NULL);
static DEVICE_ATTR(calib_b, S_IWUSR | S_IRUGO,
                  coulomb_test_show_calib_b,
                  NULL);

static struct attribute *coulomb_test_interface_attributes[] = {
    &dev_attr_version.attr,
    &dev_attr_debug_reg.attr,
    &dev_attr_vol_offset_mod.attr,
    &dev_attr_cur_offset_mod.attr,
    &dev_attr_new_battery.attr,
    &dev_attr_coul_ctrl.attr,
    &dev_attr_low_int.attr,
    &dev_attr_calib_a.attr,
    &dev_attr_calib_b.attr,
    NULL,
};

static const struct attribute_group coulomb_test_interface_attr_group = {
    .attrs = coulomb_test_interface_attributes,
};

static int coulomb_test_interface_probe(struct platform_device *pdev)
{
    struct coulomb_test_interface_info *di = NULL;
    struct class * coulomb_test_interface_class;
    struct device * new_dev = NULL;
    int ret = 0;

    pmic = dev_get_drvdata(pdev->dev.parent);
    if(NULL == pmic)
    {
        return -ENOENT;
    }

    di = (struct coulomb_test_interface_info *)kzalloc(sizeof(struct coulomb_test_interface_info), GFP_KERNEL);
    if (!di)
    {
        return -ENOMEM;
    }

    g_coulomb_test_interface_info = di;

    platform_set_drvdata(pdev, di);

    ret = sysfs_create_group(&pdev->dev.kobj, &coulomb_test_interface_attr_group);
    if (ret) {
        printk(KERN_DEBUG"could not create sysfs files\n");
        goto err_sysfs;
    }

    coulomb_test_interface_class = class_create(THIS_MODULE, "coulomb_interface");
    new_dev = device_create(coulomb_test_interface_class, NULL, 0, "%s",
                            "coulomb");
    ret = sysfs_create_link(&new_dev->kobj, &pdev->dev.kobj, "coulomb_interface_test_info");
    if(ret < 0){
        printk(KERN_DEBUG"create link to charge data fail.\n");
    }
    printk(KERN_DEBUG"hisi_coul_drv_test_probe ok!");
    return 0;

err_sysfs:
    kfree(di);
    return -1;
}


static int coulomb_test_interface_drv_remove(struct platform_device *pdev)
{
    struct coulomb_test_interface_info *di = dev_get_drvdata(&pdev->dev);

    if(di == NULL)
        return -ENOMEM;

    kfree(di);
    return 0;
}

static void coulomb_test_interface_drv_shutdown(struct platform_device *pdev)
{
    struct coulomb_test_interface_info *di = dev_get_drvdata(&pdev->dev);
    if (di == NULL) {
        printk(KERN_ERR"di is NULL!\n");
        return;
    }
    kfree(di);
    return;
}


static struct of_device_id coulomb_test_interface_match_table[] =
{
    {
    .compatible = "hisilicon,coulomb_interface",
    .data = NULL,
    },
    {
    },
};

static struct platform_driver coulomb_test_interface_driver = {
    .probe = coulomb_test_interface_probe,
    .remove = coulomb_test_interface_drv_remove,
    .shutdown = coulomb_test_interface_drv_shutdown,
    .driver = {
        .name = "hisilicon,coulomb_interface",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(coulomb_test_interface_match_table),
    },
};

int __init coulomb_test_interface_init(void)
{
    return (platform_driver_register(&coulomb_test_interface_driver));
}
fs_initcall_sync(coulomb_test_interface_init);

void __exit coulomb_interface_exit(void)
{
    platform_driver_unregister(&coulomb_test_interface_driver);
}

module_exit(coulomb_interface_exit);

MODULE_AUTHOR("HUAWEI");
MODULE_DESCRIPTION("coulomb interface module driver");
MODULE_LICENSE("GPL");
