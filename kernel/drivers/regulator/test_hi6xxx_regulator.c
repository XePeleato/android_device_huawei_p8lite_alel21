/*
 * Device driver for PMU DRIVER	 in HI6xxx SOC
 *
 * Copyright (c) 2011 Hisilicon Co. Ltd
 *
 * Dongbin Yu <yudongbin@huawei.com> : 2014-2-7
 *
 * In V8R1 the mtcmos is same with V9R1's driver,so it is multiplex.
 * That the reason why name of the c document is "hi6xxx".
 *
 * I only code the mtcmos arch,there are some thing which not finished because
 * of the litter time.In process of iterative development you must add some
 * function into the module.
 *
 *
 */

#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regulator/consumer.h>

#include <linux/delay.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of_platform.h>
#include <linux/io.h>
#include <linux/of.h>


struct hi6xxx_regulator_st {
	int status;                             /*this property in child  node*/
	unsigned int off_on_delay;              /*this property in parent node*/
	int type;        /*this property in child  node*/

	/*this property must be unify which is in child node*/
    int   ctrl_regs[3];
    int   ctrl_data[2];

    int   vset_regs;
    int   vset_data[2];
    int                        vol_numb;
    int                        *vset_table;
	struct regulator_desc rdesc;
};

typedef struct test_pmic_power_name{
    char name[24];
}test_pmic_power;

static test_pmic_power power_array[] = {
    {"ldo2"},
    {"ldo7"},
    {"ldo10"},
    {"ldo13"},
    {"ldo14"},
    {"ldo15"},
    {"ldo17"},
    {"ldo19"},
    {"ldo22"},
    {"schg_boost1"},
    {"schg_boost2"},
    {"schg_boost3"},
    {"schg_source1"},
    {"schg_source2"},
    {"schg_boost4"},
    {"schg_ldo3"},
    {"schg_cpn1"},
    {"schg_ldo1"},
    {"schg_ldo2"},
    {"MTCMOS-G3D"},
    {"MTCMOS-CODECISP"},
    {"MTCMOS-HIFI"},
    {"SD CARD IO"},
    {"SD Card"}
};

static struct regulator *test_hi6xxx_reg[sizeof(power_array)/sizeof(test_pmic_power)] = {0};

void show_regulator(void)
{
    int index = 0;
    printk("++ id ++ power_name\n");
    for(index = 0; index < sizeof(power_array)/sizeof(test_pmic_power); index++)
    {
        printk("++ %d  :  %s\n",index,power_array[index].name);
    }
}
EXPORT_SYMBOL(show_regulator);

static ssize_t regulator_on_test(struct device *dev, struct device_attribute *attr,const char *buf, size_t n)
{
    long val;
    int ret = 0;
    if ((strict_strtol(buf, 10, &val) < 0))
        return -EINVAL;

    ret = regulator_enable(test_hi6xxx_reg[val]);
    printk("**********%s regulator_enable ret %d**********\n",power_array[val].name,ret);

    return n;
}

static ssize_t regulator_off_test(struct device *dev, struct device_attribute *attr,const char *buf, size_t n)
{
    long val;
    int ret = 0;
    if ((strict_strtol(buf, 10, &val) < 0))
        return -EINVAL;

    ret = regulator_disable(test_hi6xxx_reg[val]);
    printk("**********%s regulator_disable ret %d**********\n",power_array[val].name,ret);

    return n;
}

static ssize_t regulator_status_test(struct device *dev, struct device_attribute *attr,const char *buf, size_t n)
{
    long val;
    int ret = 0;
    if ((strict_strtol(buf, 10, &val) < 0))
        return -EINVAL;

    ret = regulator_is_enabled(test_hi6xxx_reg[val]);
    printk("**********%s regulator_status ret %d**********\n",power_array[val].name,ret);

    return n;
}

static ssize_t regulator_show(struct device *dev,struct device_attribute *devattr, char *buf)
{
    show_regulator();
    return 0;
}

/*lint -e665*/
static DEVICE_ATTR(regulator_open, S_IWUSR | S_IRUGO, NULL, regulator_on_test);
static DEVICE_ATTR(regulator_close, S_IWUSR | S_IRUGO, NULL, regulator_off_test);
static DEVICE_ATTR(regulator_status, S_IWUSR | S_IRUGO, NULL, regulator_status_test);
static DEVICE_ATTR(show_regulator_node, S_IWUSR | S_IRUGO, regulator_show, NULL);
/*lint +e665*/

static struct attribute *regulator_attributes[] = {
    &dev_attr_regulator_open.attr,
    &dev_attr_regulator_close.attr,
    &dev_attr_regulator_status.attr,
    &dev_attr_show_regulator_node.attr,
    NULL
};

static const struct attribute_group regulator_group = {
    .attrs = regulator_attributes,
};

static void show_power_status(void)
{
   int index = 0;
   int ret = 0;

   for(index = 0; index < sizeof(power_array)/sizeof(test_pmic_power) ; index++)
   {
       ret = regulator_is_enabled(test_hi6xxx_reg[index]);
       printk("%s status is %d\n",power_array[index].name,ret);
   }

   return;
}
EXPORT_SYMBOL(show_power_status);

void regulator_switch_test(int id, int op)
{
    int ret= 0;

    if(0 == test_hi6xxx_reg[id])
    {
        printk("dev is not exist,%s\n",power_array[id].name);
        return;
    }
    if((id >= sizeof(power_array)/sizeof(test_pmic_power)))
    {
	 	printk("dev is not exist\n");
        return;
    }

    switch (op){
	    case 1:
	        ret = regulator_is_enabled(test_hi6xxx_reg[id]);
	        printk("**********%s 1 regulator_is_enabled ret %d*************\n",power_array[id].name,ret);

	        ret = regulator_enable(test_hi6xxx_reg[id]);
	        printk("**********%s regulator_enable ret %d**********\n",power_array[id].name,ret);

	        ret = regulator_is_enabled(test_hi6xxx_reg[id]);
	        printk("**********%s 2 regulator_is_enabled ret %d**********\n",power_array[id].name,ret);

	        ret = regulator_disable(test_hi6xxx_reg[id]);
	        printk("**********%s regulator_disable ret %d**********\n",power_array[id].name,ret);

	        ret = regulator_is_enabled(test_hi6xxx_reg[id]);
	        printk("**********%s 3 regulator_is_enabled ret %d**********\n",power_array[id].name,ret);
	    	break;
	    case 2:
	        ret = regulator_is_enabled(test_hi6xxx_reg[id]);
	        printk("**********%s [regulator_is_enabled] ret %d**********\n",power_array[id].name,ret);
	    	break;
	    case 3:
	        ret = regulator_enable(test_hi6xxx_reg[id]);
	        printk("**********%s [regulator_enable] ret %d**********\n",power_array[id].name,ret);
	    	break;
	    case 4:
	        ret = regulator_disable(test_hi6xxx_reg[id]);
	        printk("**********%s [regulator_disable] ret %d**********\n",power_array[id].name,ret);
	    	break;
	    default:
	        printk("**********operation is null**********\n");
	    	break;
    }
}
EXPORT_SYMBOL(regulator_switch_test);

void regulator_voltage_test(int id, int op, int min_uV, int max_uV)
{
    int ret = 0;
    unsigned int selector = 0;

    if(0 == test_hi6xxx_reg[id])
    {
        printk("dev is not exist,%s\n",power_array[id].name);
        return;
    }
    if((id >= sizeof(power_array)/sizeof(test_pmic_power)))
    {
	 	printk("dev is not exist\n");
        return;
    }
	
    switch(op)
    {
        case 1:
            ret = regulator_list_voltage(test_hi6xxx_reg[id], selector);
            break;
        case 2:
            ret = regulator_get_voltage(test_hi6xxx_reg[id]);
            printk("**********%s [regulator_get_voltage] ret %duV**********\n",power_array[id].name,ret);
            break;
        case 3:
            ret = regulator_set_voltage(test_hi6xxx_reg[id], min_uV, max_uV);
            printk("**********%s [regulator_set_voltage] ret %d**********\n",power_array[id].name,ret);
            break;
        case 4:
            ret = regulator_set_current_limit(test_hi6xxx_reg[id], min_uV, max_uV);
            printk("**********%s [regulator_set_current] ret %d**********\n",power_array[id].name,ret);
            break;
        default:
            printk("**********operation is null**********\n");
            break;
    }
}
EXPORT_SYMBOL(regulator_voltage_test);

static int test_hi6xxx_regulator_probe(struct platform_device *pdev)
{
    int index = 0;
    int ret = 0;
	struct regulator *test_regulator = NULL;
 	struct device *dev = &pdev->dev;
    pr_info("^^^^^^^^^^^^^^^[%s 1]^^^^^^^^^^^^^^^\n", __FUNCTION__);

    pr_info("^^^^^^^^^^^^^^^[%s 2]^^^^^^^^^^^^^^^\n", __FUNCTION__);

    for(index = 0; index < sizeof(power_array)/sizeof(test_pmic_power); index++)
    {
    	test_regulator = regulator_get(dev,power_array[index].name);
        test_hi6xxx_reg[index] = test_regulator;
        pr_info("***************name  %s,%p*************\n",
            power_array[index].name, test_regulator);
    }

    pr_info("^^^^^^^^^^^^^^^[%s 3]^^^^^^^^^^^^^^^\n", __FUNCTION__);

    ret = sysfs_create_group(&pdev->dev.kobj, &regulator_group);
    if (ret) {
        printk("could not create regulator sysfs files\n");
    }

    pr_info("^^^^^^^^^^^^^^^[%s 4]^^^^^^^^^^^^^^^\n", __FUNCTION__);

	return 0;
}

static struct of_device_id of_test_hi6xxx_regulator_match_tbl[] = {
	{
		.compatible = "hisilicon,test-hi6xxx-regulator-driver",
	},
	{ /* end */ }
};


static struct platform_driver test_hi6xxx_regulator_driver = {
	.driver = {
		.name	= "test_hisi_hi6xxx_regulator",
		.owner  = THIS_MODULE,
		.of_match_table = of_test_hi6xxx_regulator_match_tbl,
	},
	.probe	= test_hi6xxx_regulator_probe,
};


static int __init test_hi6xxx_regulator_init(void)
{
    pr_info("^^^^^^^^^^^^^^^[%s]^^^^^^^^^^^^^^^\n", __FUNCTION__);
    return platform_driver_register(&test_hi6xxx_regulator_driver);
}

static void __exit test_hi6xxx_regulator_exit(void)
{
	platform_driver_unregister(&test_hi6xxx_regulator_driver);
}


late_initcall(test_hi6xxx_regulator_init);
module_exit(test_hi6xxx_regulator_exit);

MODULE_AUTHOR("Dongbin Yu<yudongbin@huawei.com>");
MODULE_DESCRIPTION("HI6xxx MTCMOS interface driver");
MODULE_LICENSE("GPL V2");
