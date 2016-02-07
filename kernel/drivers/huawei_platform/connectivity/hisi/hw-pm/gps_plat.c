#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/consumer.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/stat.h>

#define DTS_COMP_GPS_POWER_NAME "hisilicon,hi1101_gps"
#define HI_GPS_REF_CLK_FREQ    49152000

typedef struct hi_gps_info {
    struct clk *refclk_sel;
    struct clk *refclk;
    struct pinctrl *pctrl;
    struct pinctrl_state *pins_normal;
    struct pinctrl_state *pins_idle;
} HI_GPS_INFO;

static struct clk *gps_ref_clk = NULL;
static HI_GPS_INFO *hi_gps_info_t = NULL;

static int hi_gps_probe(struct platform_device *pdev)
{
    HI_GPS_INFO *hi_gps_info = NULL;
    struct device *gps_power_dev = &pdev->dev;
    struct device_node *np = gps_power_dev->of_node;
    int ret = 0;

    printk(KERN_INFO "[GPS] start find gps_power\n");
    hi_gps_info = kzalloc(sizeof(HI_GPS_INFO), GFP_KERNEL);
    if (!hi_gps_info) {
        printk(KERN_ERR "[GPS] Alloc memory failed\n");
        return -ENOMEM;
    }

    hi_gps_info->pctrl = devm_pinctrl_get(gps_power_dev);
    if (IS_ERR(hi_gps_info->pctrl)) {
        printk(KERN_ERR "[GPS] pinctrl get error! %p\n", hi_gps_info->pctrl);
        ret = -1;
        goto err_pinctrl_get;
    }

    hi_gps_info->pins_normal = pinctrl_lookup_state(hi_gps_info->pctrl, "default");
    if (IS_ERR(hi_gps_info->pins_normal)) {
        printk(KERN_ERR "[GPS] hi_gps_info->pins_normal lookup error! %p\n", hi_gps_info->pins_normal);
        ret = -1;
        goto err_pins_normal;
    }

    hi_gps_info->pins_idle = pinctrl_lookup_state(hi_gps_info->pctrl, "idle");
    if (IS_ERR(hi_gps_info->pins_idle)) {
        printk(KERN_ERR "[GPS] hi_gps_info->pins_idle lookup error! %p\n", hi_gps_info->pins_idle);
        ret = -1;
        goto err_pins_idle;
    }

    ret = pinctrl_select_state(hi_gps_info->pctrl, hi_gps_info->pins_normal);
    if (ret) {
        printk(KERN_ERR "[GPS] pinctrl_select_state error! %d\n", ret);
        goto err_select_state;
    }
    printk(KERN_INFO "[GPS] pinctrl is finish\n");

    hi_gps_info->refclk_sel = of_clk_get_by_name(np, "ref_clk_sel");
    if (IS_ERR(hi_gps_info->refclk_sel)) {
        printk(KERN_ERR "[GPS] ref_clk_sel get failed!\n");
        ret = -1;
        goto err_refclk_sel_get;
    }
    ret = clk_set_rate(hi_gps_info->refclk_sel, HI_GPS_REF_CLK_FREQ);
    if (ret < 0) {
        printk(KERN_ERR "[GPS] ref_clk_sel set rate failed\n");
        ret = -1;
        goto err_refclk_sel_set_rate;
    }

    hi_gps_info->refclk = of_clk_get_by_name(np, "ref_clk");
    if (IS_ERR(hi_gps_info->refclk)) {
        printk(KERN_ERR "[GPS] ref_clk get failed!\n");
        ret = -1;
        goto err_refclk_get;
    }
    gps_ref_clk = hi_gps_info->refclk;
    printk(KERN_INFO "[GPS] ref clk is finished!\n");

    platform_set_drvdata(pdev, hi_gps_info);
    hi_gps_info_t = hi_gps_info;
    return 0;

err_refclk_get:    
err_refclk_sel_set_rate:
    clk_put(hi_gps_info->refclk_sel);
err_refclk_sel_get:    
err_select_state:    
err_pins_idle:    
err_pins_normal:
    devm_pinctrl_put(hi_gps_info->pctrl);  
err_pinctrl_get:
    kfree(hi_gps_info);
    hi_gps_info = NULL;
    hi_gps_info_t = NULL;
    return ret;
}

static void hi_gps_shutdown(struct platform_device *pdev)
{
    HI_GPS_INFO *hi_gps_info = platform_get_drvdata(pdev);
    printk(KERN_INFO "[GPS] hi_gps_shutdown!\n");

    if (!hi_gps_info) {
        printk(KERN_ERR "[GPS] hi_gps_info is NULL,just return.\n");
        return;
    }
    
    platform_set_drvdata(pdev, NULL);
    kfree(hi_gps_info);
    hi_gps_info = NULL;
    hi_gps_info_t = NULL;
    return;
}

static const struct of_device_id gps_power_match_table[] = {
    {
        .compatible = DTS_COMP_GPS_POWER_NAME,   // compatible must match with which defined in dts
        .data = NULL,
    },
    {
    },
};

MODULE_DEVICE_TABLE(of, gps_power_match_table);

static struct platform_driver hi_gps_plat_driver = {
    .probe          = hi_gps_probe,
    .suspend        = NULL,
    .remove         = NULL,
    .shutdown       = hi_gps_shutdown,
    .driver = {
        .name = "hi1101_gps",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(gps_power_match_table), // dts required code
    },
};

static int __init hi_gps_plat_init(void)
{
    return platform_driver_register(&hi_gps_plat_driver);
}

static void __exit hi_gps_plat_exit(void)
{
    platform_driver_unregister(&hi_gps_plat_driver);
}

int set_gps_ref_clk_enable_hi110x(bool enable)
{
    int ret = 0;

    printk(KERN_INFO "[GPS] set_gps_ref_clk_enable(%d) \n",enable);
    if (IS_ERR_OR_NULL(gps_ref_clk)) {
        printk(KERN_ERR "[GPS] ERROR: refclk is invalid! \n");
        return -1;
    }

    if (enable) {
        ret = clk_prepare_enable(gps_ref_clk);
        if (ret < 0) {
            printk(KERN_ERR "[GPS] ERROR: refclk enable failed! \n");
            return -1;
        }
    } else {
        clk_disable_unprepare(gps_ref_clk);
    }
    printk(KERN_INFO "[GPS] set_gps_ref_clk_enable finish \n");

    return 0;
}
EXPORT_SYMBOL(set_gps_ref_clk_enable_hi110x);

module_init(hi_gps_plat_init);
module_exit(hi_gps_plat_exit);

MODULE_AUTHOR("DRIVER_AUTHOR");
MODULE_DESCRIPTION("GPS Hi110X Platfrom driver");
MODULE_LICENSE("GPL");
