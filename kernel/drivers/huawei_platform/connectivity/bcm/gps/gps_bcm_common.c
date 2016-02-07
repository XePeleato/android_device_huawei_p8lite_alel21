#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/printk.h>
#include <linux/huawei/gps/huawei_gps.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/unistd.h>
#include <linux/bug.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#define DTS_COMP_GPS_POWER_NAME "huawei,gps_power"
#include <linux/huawei/hw_connectivity.h>

#define STR_BCM_TYPE_4752 "bcm4752"
#define STR_BCM_TYPE_47531 "bcm47531"

#define GPS_IC_TYPE_4752 4752
#define GPS_IC_TYPE_47531 47531

#define GPS_REFCLK_SEL_ENABLE 1
#define GPS_REFCLK_SEL_DISABLE 0

static int gps_ic_type = -1;
static int gps_ref_sel_enable = -1;

int get_gps_ic_type()
{
    return gps_ic_type;
}

int get_gps_ref_sel_enable()
{
    return gps_ref_sel_enable;
}
static int __init gps_bcm_common_init(void)
{
    struct device_node* np = NULL;
    int ret = 0;
    char *gps_ic_type_str = NULL;
    char *gps_ref_clk_enable = NULL;

    np = of_find_compatible_node(NULL, NULL, DTS_COMP_GPS_POWER_NAME);
    if (!np) {
        printk(KERN_ERR "%s, can't find node \n", __func__, DTS_COMP_GPS_POWER_NAME);
        return -1;
    }

    ret=of_property_read_string(np, "broadcom_config,ic_type", (const char **)&gps_ic_type_str);
    if(ret)
    {
        printk(KERN_ERR "get broadcom_config,ic_type fail ret=%d\n",ret);
        gps_ic_type = GPS_IC_TYPE_4752;
    }
    else if(0 == strncmp(STR_BCM_TYPE_4752, gps_ic_type_str, sizeof(STR_BCM_TYPE_4752)))
    {
        gps_ic_type = GPS_IC_TYPE_4752;
    }
    else if(0 == strncmp(STR_BCM_TYPE_47531, gps_ic_type_str, sizeof(STR_BCM_TYPE_47531)))
    {
        gps_ic_type = GPS_IC_TYPE_47531;
    }
    else
    {
        printk(KERN_ERR "get broadcom_config,get ic type error\n");
        gps_ic_type = -1;
    }

    ret = of_property_read_string(np, "broadcom_config,ref_clk_sel_enable", (const char **)&gps_ref_clk_enable);
    if(ret)
    {
        gps_ref_sel_enable = GPS_REFCLK_SEL_ENABLE;
    }
    else if(0 == strncmp("no",gps_ref_clk_enable,sizeof("no")))
    {
        gps_ref_sel_enable = GPS_REFCLK_SEL_DISABLE;
    }
    else  if(0 == strncmp("yes",gps_ref_clk_enable,sizeof("yes")))
    {
        gps_ref_sel_enable = GPS_REFCLK_SEL_ENABLE;
    }
    else
    {
        gps_ref_sel_enable = -1;
    }

    return 0;
}

static void __exit gps_bcm_common_exit(void)
{
   printk(KERN_ERR "%s, exit\n", __func__);
   return;
}


int set_gps_ref_clk_enable_bcm(bool enable)
{
    int ret = 0;

    printk(KERN_INFO "[GPS] set_gps_ref_clk_enable(%d) \n", enable);
    if(GPS_IC_TYPE_4752 == get_gps_ic_type())
    {
        set_gps_ref_clk_enable_bcm4752(enable);
    }
    else if(GPS_IC_TYPE_47531 == get_gps_ic_type())
    {
        set_gps_ref_clk_enable_bcm47531(enable);
    }
    else
    {
        printk(KERN_INFO "[GPS] can't find IC \n");
    }

    return 0;
}

fs_initcall(gps_bcm_common_init);
module_exit(gps_bcm_common_exit);

MODULE_AUTHOR("DRIVER_AUTHOR");
MODULE_DESCRIPTION("GPS Boardcom gps common driver");
MODULE_LICENSE("GPL");