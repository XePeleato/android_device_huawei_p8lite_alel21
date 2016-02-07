#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/irq.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/huawei/usb/hisi_susb.h>
#include <soc_ao_sctrl_interface.h>
#include <soc_baseaddr_interface.h>
#include <soc_peri_sctrl_interface.h>
#include <soc_usbotg_interface.h>
#include <soc_smart_interface.h>
#include <soc_irqs.h>
#include "lm.h"
#include <huawei_platform/usb/switch/switch_usb.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/of_gpio.h>
#include <linux/module.h>
#include "dwc_otg_hi6xxx.h"
#include <linux/pinctrl/consumer.h>
#include <linux/mfd/hisi_hi6xxx_pmic.h>
#include <linux/hisi/hi6xxx-iomap.h>
#include <linux/hisi/hi6xxx-platform.h>
#include <linux/pwrctrl_power_state_manager.h>
#include <linux/init.h>



struct hiusb_info *g_hiusb_info;
static unsigned int release_wakelock;

#define RUN_MODE_LEN 32
static char run_mode[RUN_MODE_LEN];
static int oem_otg_enable = 0;
int g_otg_enable = 0;

#ifdef CONFIG_SWITCH_USB_CLS
static struct platform_device usb_switch_device = {
    .name   = "switch-usb",
    .dev    = {
        .init_name = "switch-usb",
    },
};

static struct platform_device *plat_usb_dev[] __initdata = {
    &usb_switch_device,
};
#endif

#ifdef CONFIG_SWITCH_FSA9685
extern int fsa9685_manual_detach(void);
#endif

static char* usb_ct_str[] = {
    "CHARGER_TYPE_USB",
    "CHARGER_TYPE_BC_USB",
    "CHARGER_TYPE_NON_STANDARD",
    "CHARGER_TYPE_STANDARD",
    "CHARGER_REMOVED",
    "USB_EVENT_OTG_ID",
    "unknown",
};
#define HIUSB_MAX_STATUS 3
static char* usb_status[] = {
    "HIUSB_OFF",
    "HIUSB_DEVICE",
    "HIUSB_HOST",
    "HIUSB_SUSPEND"
};
/* ---------------------- usb common -------------------------------------*/
#define HI6xxx_USB_NAME    "hi6xxx-usb-otg"

/* usb rst registers *//*w00140341 B030 modify*/
struct hisi_rst hiusb_rst[] = {

    {0, \
     (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_bus_START) |
     (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_por_picophy_START) |
     (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_START) |
     (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_32k_START) , 10, 0},/* OTG AND PHY in alwayson sctrl */
    {0,0,0},
};


/* usb nrst registers *//*w00140341 B030 modify*/
struct hisi_rst hiusb_nrst[] = {
    {0, \
     (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_bus_START) |
     (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_por_picophy_START) |
     (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_START) |
     (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_32k_START) , 10, 0},/* OTG AND PHY in alwayson sctrl */
    {0,0,0},
};
/* --------------------- sysfs file --------------------------------------*/
/**
 * Show the switch gpio status.
 */
irqreturn_t hiusb_vbus_intr(int irq, void *dev);
int use_switch_driver=0;
int id_no_bypass=0;

#ifdef CONFIG_CHARGER_TYPE_RECHECK
// 插入充电器被识别为USB标志位，当需要进一步确认标志位为0，已确认类型则为1
int g_charger_usb_checked = 0;

int getChargerUsbState(void)
{
    return g_charger_usb_checked;
}
EXPORT_SYMBOL_GPL(getChargerUsbState);

int setChargerUsbState(int state)
{
    if(state > 1 || state < 0)
    {
        return -1;
    }
	g_charger_usb_checked = state;
	return g_charger_usb_checked;
}
#endif

int get_resource(struct hiusb_info *hiusb_info)
{
    int ret = 0;
    struct device_node *np = NULL;
	enum of_gpio_flags flags;

    /*
     * get registers base address
     */
    hiusb_info->aosctrlcrg_base = (void __iomem *)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);

    hiusb_info->pericrg_base = (void __iomem *)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);

    if (!hiusb_info->aosctrlcrg_base || !hiusb_info->pericrg_base) {
        usb_err("get registers base address failed!\n");
        return -ENODEV;
    }

    hiusb_rst[0].addr = SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_ADDR(hiusb_info->pericrg_base);
    hiusb_nrst[0].addr = SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_ADDR(hiusb_info->pericrg_base);

    usb_dbg("%s nrst addr:%p value:0x%8.8x udelay:%d\n",
                __func__,
                hiusb_nrst[0].addr,
                hiusb_nrst[0].value,
                hiusb_nrst[0].udelay);

    usb_dbg("%s rst addr:%p value:0x%8.8x udelay:%d\n",
                __func__,
                hiusb_rst[0].addr,
                hiusb_rst[0].value,
                hiusb_rst[0].udelay);

    np = of_find_compatible_node(NULL, NULL, "hisilicon,hi6xxx-usb-otg");
    if (np) {
        if (of_property_read_u32(np, "huawei,use_switch_driver",&use_switch_driver))
            usb_dbg("%s:huawei,use_switch_driver is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,use_switch_driver %d\n",__FUNCTION__,use_switch_driver);

        if (of_property_read_u32(np, "huawei,id_no_bypass",&id_no_bypass))
            usb_dbg("%s:huawei,id_no_bypass is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,id_no_bypass %d\n",__FUNCTION__,id_no_bypass);

        if (of_property_read_u32(np, "huawei,otg_without_mhl",&hiusb_info->otg_without_mhl))
            usb_dbg("%s:huawei,otg_without_mhl is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,otg_without_mhl %d\n",__FUNCTION__,hiusb_info->otg_without_mhl);

        usb_err("[USB_DEBUG]g_otg_enable is %d\n",g_otg_enable);
        hiusb_info->otg_int_gpio = of_get_gpio_by_prop(np, "huawei,otg_int_gpio", 0, 0, &flags);
        if (!gpio_is_valid(hiusb_info->otg_int_gpio)){
	        hiusb_info->otg_int_gpio = 0;
			g_otg_enable = 0;
            usb_err("%s:huawei,otg_int_gpio is not config\n",__FUNCTION__);
        }
		else if(!g_otg_enable)
		{
            hiusb_info->otg_int_gpio = 0;
            usb_err("[USB_DEBUG]%s:g_otg_enable is 0 ,set otg_int_gpio to 0! \n",__FUNCTION__);
		}
        usb_dbg("%s:huawei,otg_int_gpio %d\n",__FUNCTION__,hiusb_info->otg_int_gpio);

        if (of_property_read_u32(np, "huawei,phy_reset_pin",&hiusb_info->phy_reset_pin))
            usb_dbg("%s:huawei,phy_reset_pin is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,phy_reset_pin %d\n",__FUNCTION__,hiusb_info->phy_reset_pin);

        if (of_property_read_u32(np, "huawei,switch_pin1",&hiusb_info->switch_pin1))
            usb_dbg("%s:huawei,switch_pin1 is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,switch_pin1 %d\n",__FUNCTION__,hiusb_info->switch_pin1);

        if (of_property_read_u32(np, "huawei,switch_pin2",&hiusb_info->switch_pin2))
            usb_dbg("%s:huawei,switch_pin2 is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,switch_pin2 %d\n",__FUNCTION__,hiusb_info->switch_pin2);

        if (of_property_read_u32(np, "huawei,switch_id_pin",&hiusb_info->switch_id_pin))
            usb_dbg("%s:huawei,switch_id_pin is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,switch_id_pin %d\n",__FUNCTION__,hiusb_info->switch_id_pin);

        if (of_property_read_u32(np, "huawei,cdma_usb_enable",&hiusb_info->cdma_usb_enable))
            usb_dbg("%s:huawei,cdma_usb_enable is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,cdma_usb_enable %d\n",__FUNCTION__,hiusb_info->cdma_usb_enable);

        if (of_property_read_u32(np, "huawei,eye_pattern",&hiusb_info->eye_pattern))
            usb_dbg("%s:huawei,eye_pattern is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,eye_pattern 0x%x\n",__FUNCTION__,hiusb_info->eye_pattern);

        if (of_property_read_u32(np, "huawei,otg_eye_pattern",&hiusb_info->otg_eye_pattern))
            usb_dbg("%s:huawei,otg_eye_pattern is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,otg_eye_pattern 0x%x\n",__FUNCTION__,hiusb_info->otg_eye_pattern);

        if (of_property_read_u32(np, "huawei,usb_bus_clk_rate",&hiusb_info->clk_rate))
            usb_dbg("%s:huawei,usb_bus_clk_rate is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,usb_bus_clk_rate 0x%x\n",__FUNCTION__,hiusb_info->clk_rate);

        if (of_property_read_u32(np, "huawei,quirks",&hiusb_info->quirks))
            usb_dbg("%s:huawei,quirks is not config\n",__FUNCTION__);
        usb_dbg("%s:huawei,quirks 0x%x\n",__FUNCTION__,hiusb_info->quirks);
        hiusb_info->clk_usbotg_off = of_clk_get(np, 0);
        ret = IS_ERR(hiusb_info->clk_usbotg_off);
        if (ret) {
            usb_err("get clk_usbotg_off failed!\n");
            goto get_clk_usbotg_off_fail;
        }
    }

    usb_dbg("get resource %s!\n", ret ? "error" : "done");
    return 0;
get_clk_usbotg_off_fail:
    return ret;
}

void hiusb_system_info(void)
{
    uint32_t reg;

    reg = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));
    printk("SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR:0x%8.8x\n",reg & 0x428);

    reg = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR(g_hiusb_info->pericrg_base));
    printk("SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR:0x%8.8x\n",reg & 0xD40);

    reg = readl(SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_ADDR(g_hiusb_info->pericrg_base));
    printk("SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_ADDR:0x%8.8x\n",reg & 0xF0);

    reg = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR(g_hiusb_info->pericrg_base));
    printk("SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR:0x%8.8x\n",reg);

}

STATIC ssize_t swstate_store(struct device *_dev,
                 struct device_attribute *attr,
                 const char *buf, size_t count)
{
    uint32_t val = simple_strtoul(buf, NULL, 16);

    if (val == 0) {
        if (g_hiusb_info->cdma_usb_enable == 0) {
            pr_err("%s: no cdma_usb_enable gpio!\n", __func__);
        } else {
            gpio_direction_output(g_hiusb_info->cdma_usb_enable, 0);
        }
        switch_off();
        msleep(1000);
        switch_reset();
    } else if(val == 1) {
        if (g_hiusb_info->cdma_usb_enable == 0) {
            pr_err("%s: no cdma_usb_enable gpio!\n", __func__);
        } else {
            gpio_direction_output(g_hiusb_info->cdma_usb_enable, 1);
        }
        switch_off();
        msleep(1000);
        switch_cdma();
    } else {
        if (g_hiusb_info->cdma_usb_enable == 0) {
            pr_err("%s: no cdma_usb_enable gpio!\n", __func__);
        } else {
            gpio_direction_output(g_hiusb_info->cdma_usb_enable, 0);
        }
        switch_off();
    }

    return count;
}
STATIC ssize_t swstate_show(struct device *_dev,
              struct device_attribute *attr, char *buf)
{
    int cdma_usb_enable=0;

    if (g_hiusb_info->cdma_usb_enable)
        cdma_usb_enable = gpio_get_value_cansleep(g_hiusb_info->cdma_usb_enable);

    return sprintf(buf, "%d", cdma_usb_enable);
}
DEVICE_ATTR(swstate, (S_IRUGO|S_IWUSR), swstate_show, swstate_store);

/**
 * Show the charger status.
 */
STATIC ssize_t charger_show(struct device *_dev,
              struct device_attribute *attr, char *buf)
{
    struct lm_device *lm_dev = container_of(_dev, struct lm_device, dev);
    struct hiusb_info *hiusb_info;
    int ret;
    char charger_type[10];

    hiusb_info = lm_dev->hiusb_info;
    ret = get_charger_name();
    switch (ret) {
        case CHARGER_TYPE_USB:
            strncpy(charger_type, "SDP", strlen("SDP")+1);
            break;
        case CHARGER_TYPE_BC_USB:
            strncpy(charger_type, "CDP", strlen("CDP")+1);
            break;
        case CHARGER_TYPE_NON_STANDARD:
            strncpy(charger_type, "SDP", strlen("SDP")+1);
            break;
        case CHARGER_TYPE_STANDARD:
            strncpy(charger_type, "DCP", strlen("DCP")+1);
            break;
        case CHARGER_REMOVED:
            strncpy(charger_type, "REMOVED", strlen("DCP")+1);
            break;
        default:
            return sprintf(buf, "usb not init\n");
    }

    return sprintf(buf, "charger_type:%d(%s)\n", ret, charger_type);
}
DEVICE_ATTR(charger, S_IRUGO, charger_show, NULL);

/**
 * Show the hiusb_status.
 */
STATIC ssize_t hiusb_status_show(struct device *_dev,
              struct device_attribute *attr, char *buf)
{
    struct lm_device *lm_dev = container_of(_dev, struct lm_device, dev);
    struct hiusb_info *hiusb_info;
    int ret;
    char hiusb_status[20];

    hiusb_info = lm_dev->hiusb_info;

    ret = down_interruptible(&hiusb_info->hiusb_info_sema);
    if (ret) {
        pr_err("%s: down hiusb_info_sema failed!\n", __func__);
        return ret;
    }
    ret = hiusb_info->hiusb_status;
    up(&hiusb_info->hiusb_info_sema);

    switch (ret) {
        case HIUSB_OFF:
            strncpy(hiusb_status, "HIUSB_OFF", strlen("HIUSB_OFF")+1);
            break;
        case HIUSB_DEVICE:
            strncpy(hiusb_status, "HIUSB_DEVICE", strlen("HIUSB_DEVICE")+1);
            break;
        case HIUSB_HOST:
            strncpy(hiusb_status, "HIUSB_HOST", strlen("HIUSB_HOST")+1);
            break;
        case HIUSB_SUSPEND:
            strncpy(hiusb_status, "HIUSB_SUSPEND", strlen("HIUSB_SUSPEND")+1);
            break;
        default:
            strncpy(hiusb_status, "UNKNOWN", strlen("UNKNOWN")+1);
            break;
    }

    return sprintf(buf, "hiusb_status:%d(%s)\n", hiusb_info->hiusb_status, hiusb_status);
}
DEVICE_ATTR(hiusb_status, S_IRUGO, hiusb_status_show, NULL);

/**
 * Store the test_vbus_store attribure.
 */
STATIC ssize_t test_vbus_store(struct device *_dev,
                 struct device_attribute *attr,
                 const char *buf, size_t count)
{
    struct lm_device *lm_dev = container_of(_dev, struct lm_device, dev);
    struct hiusb_info *hiusb_info = lm_dev->hiusb_info;
    uint32_t val = simple_strtoul(buf, NULL, 16);

    if (hiusb_info->insert_irq == 0 || hiusb_info->draw_irq == 0) {
        dev_info(&lm_dev->dev, "No insert/draw irq.\n");
        return 0;
    }

    if (val == 0) {
        hiusb_vbus_intr(hiusb_info->draw_irq, lm_dev);
    } else {
        hiusb_vbus_intr(hiusb_info->insert_irq, lm_dev);
    }

    return count;
}
DEVICE_ATTR(test_vbus, S_IWUSR, NULL, test_vbus_store);

/**
 * Store the test_id_store attribure.
 */
STATIC ssize_t test_id_store(struct device *_dev,
                 struct device_attribute *attr,
                 const char *buf, size_t count)
{
    struct lm_device *lm_dev = container_of(_dev, struct lm_device, dev);
    struct hiusb_info *hiusb_info = lm_dev->hiusb_info;
    uint32_t val = simple_strtoul(buf, NULL, 16);

    if (hiusb_info->insert_irq == 0 || hiusb_info->draw_irq == 0) {
        dev_info(&lm_dev->dev, "No insert/draw irq.\n");
        return 0;
    }

    if (val == 0) {
        hisi_usb_id_change(ID_RISE_EVENT);
        msleep(3000);
        hisi_usb_id_change(ID_FALL_EVENT);
    } else {
        hiusb_vbus_intr(hiusb_info->draw_irq, lm_dev);
        msleep(3000);
        hiusb_vbus_intr(hiusb_info->insert_irq, lm_dev);
    }

    return count;
}
DEVICE_ATTR(test_id, S_IWUSR, NULL, test_id_store);

/**
 * Store the test_hibernation_store attribure.
 */
STATIC ssize_t test_hibernation_store(struct device *_dev,
                 struct device_attribute *attr,
                 const char *buf, size_t count)
{
    struct lm_device *lm_dev = container_of(_dev, struct lm_device, dev);
    struct hiusb_info *hiusb_info = lm_dev->hiusb_info;
    uint32_t val = simple_strtoul(buf, NULL, 16);

    if (hiusb_info->insert_irq == 0 || hiusb_info->draw_irq == 0) {
        dev_info(&lm_dev->dev, "No insert/draw irq.\n");
        return 0;
    }

    dwc_otg_set_hibernation(lm_dev, val);

    return count;
}
DEVICE_ATTR(test_hibernation, S_IWUSR, NULL, test_hibernation_store);

void t_vb(int count)
{
    struct lm_device *lm_dev = g_hiusb_info->lm_dev;
    if (count == 0) {
        hiusb_vbus_intr(g_hiusb_info->draw_irq, lm_dev);
    } else {
        hiusb_vbus_intr(g_hiusb_info->insert_irq, lm_dev);
    }

    return;
}

/**
 * release_wakelock include chrg_lock && hiusb_info->dev_wakelock,
 * so that system can enter deep sleep.
 */
static ssize_t release_wakelock_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", release_wakelock);
}
static void release_wakelock_do_disconnect(struct lm_device *lm_dev);
static ssize_t release_wakelock_store(struct device *pdev,
		struct device_attribute *attr, const char *buf, size_t size)
{
    long val = 0;
    struct lm_device *lm_dev = container_of(pdev, struct lm_device, dev);
    struct hiusb_info *hiusb_info = lm_dev->hiusb_info;

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    dev_info(&lm_dev->dev, "%s store val:%ld.\n", __func__, val);
    release_wakelock = val;
    if (release_wakelock)
    {
        release_wakelock_do_disconnect(lm_dev);
    }
    else
    {
        if (!wake_lock_active(&hiusb_info->dev_wakelock))
        {
            dev_info(&lm_dev->dev, "usb dev_wakelock lock.\n");
            wake_lock(&hiusb_info->dev_wakelock);
        }
    }
    return size;
}
DEVICE_ATTR(release_wakelock, (S_IRUGO | S_IWUSR), release_wakelock_show, release_wakelock_store);

void hiusb_attr_create(struct lm_device *lm_dev)
{
    int error;
    struct class *switch_usb_class;
    struct device *switch_usb_dev;
    struct class* hisi_usb_class;
    struct device* hisi_usb_dev;

    if(!use_switch_driver){
        switch_usb_class = class_create(THIS_MODULE, "usbswitch");
        switch_usb_dev = device_create(switch_usb_class, NULL, MKDEV(0,0), NULL, "usbsw");
        error = device_create_file(switch_usb_dev, &dev_attr_swstate);
    }

    hisi_usb_class = class_create(THIS_MODULE,"hisi_usb_class");
    if (NULL == hisi_usb_class)
    {
        usb_err("create hisi_usb_class error!\n");
    }
    else
    {
        hisi_usb_dev= device_create(hisi_usb_class, NULL, 0, NULL, "hisi_usb_dev");
        if (NULL == hisi_usb_dev)
        {
            usb_err("create hisi_usb_dev error!\n");
        }
        else
        {
            error = sysfs_create_link(&hisi_usb_dev->kobj, &lm_dev->dev.kobj, "interface");
            if (error)
                usb_err("sysfs_create_link error!\n");
        }
    }

    error = device_create_file(&lm_dev->dev, &dev_attr_charger);
    error = device_create_file(&lm_dev->dev, &dev_attr_hiusb_status);
    error = device_create_file(&lm_dev->dev, &dev_attr_test_vbus);
    error = device_create_file(&lm_dev->dev, &dev_attr_test_id);
    error = device_create_file(&lm_dev->dev, &dev_attr_test_hibernation);
    error = device_create_file(&lm_dev->dev, &dev_attr_release_wakelock);
}

void hiusb_attr_remove(struct lm_device *lm_dev)
{
    device_remove_file(&lm_dev->dev, &dev_attr_charger);
    device_remove_file(&lm_dev->dev, &dev_attr_hiusb_status);
    device_remove_file(&lm_dev->dev, &dev_attr_test_vbus);
    device_remove_file(&lm_dev->dev, &dev_attr_test_id);
    device_remove_file(&lm_dev->dev, &dev_attr_test_hibernation);
    device_remove_file(&lm_dev->dev, &dev_attr_release_wakelock);
}

/* ---------------------- usb charger ------------------------------------*/
/* pmu read */
extern unsigned int hi6xxx_pmic_get_version(void);
STATIC unsigned int pmu_version(void)
{
    return hi6xxx_pmic_get_version();
}

unsigned char vbus_status(void)
{
    unsigned char vbus_status = 0;
    vbus_status = hi6xxx_pmic_reg_read(SOC_SMART_STATUS2_ADDR(0));
    vbus_status &= (1<<SOC_SMART_STATUS2_vbus_det_3p6_d20r_START);
    return vbus_status;
}

int get_charger_name(void)
{
    int ret = -1;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return ret;
    }

    ret = down_interruptible(&g_hiusb_info->hiusb_info_sema);
    if (ret) {
        pr_err("%s: down hiusb_info_sema failed!\n", __func__);
        return ret;
    }

    ret = g_hiusb_info->charger_type;

    up(&g_hiusb_info->hiusb_info_sema);

    return ret;
}
EXPORT_SYMBOL_GPL(get_charger_name);


int hiusb_charger_registe_notifier(struct notifier_block *nb)
{
    int ret = -1;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return ret;
    }

    ret = atomic_notifier_chain_register(
            &g_hiusb_info->charger_type_notifier_head, nb);

    return ret;

}
EXPORT_SYMBOL_GPL(hiusb_charger_registe_notifier);


int hiusb_charger_unregiste_notifier(struct notifier_block *nb)
{
    int ret = -1;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return ret;
    }

    ret = atomic_notifier_chain_unregister(
            &g_hiusb_info->charger_type_notifier_head, nb);

    return ret;
}
EXPORT_SYMBOL_GPL(hiusb_charger_unregiste_notifier);

STATIC void notify_charger_type(void)
{
    atomic_notifier_call_chain(&g_hiusb_info->charger_type_notifier_head,
        g_hiusb_info->charger_type, g_hiusb_info);
    pr_info("%s: Notify charger type: %s\n", __func__, g_hiusb_info->charger_type < CHARGER_TYPE_MAX ? usb_ct_str[g_hiusb_info->charger_type]:"error type");
    return;
}

STATIC int detect_charger_type(void)
{
    int type;

    /* todo: wait huangyonggang */
    SOC_PERI_SCTRL_SC_PERIPH_CTRL5_UNION peri_sctrl_ctrl5;
    SOC_PERI_SCTRL_SC_PERIPH_STAT2_UNION peri_sctrl_stat2;

    peri_sctrl_ctrl5.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));
    peri_sctrl_ctrl5.reg.picophy_bc_mode = 1;
    peri_sctrl_ctrl5.reg.picophy_dcdenb = 1;
    peri_sctrl_ctrl5.reg.picophy_chrgsel = 0;
    peri_sctrl_ctrl5.reg.picophy_vdatsrcend = 0;
    peri_sctrl_ctrl5.reg.picophy_vdatdetenb = 0;  
    writel(peri_sctrl_ctrl5.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));

    msleep(900);

    peri_sctrl_stat2.value = readl(SOC_PERI_SCTRL_SC_PERIPH_STAT2_ADDR(g_hiusb_info->pericrg_base));
    if (peri_sctrl_stat2.reg.datcondet_sts == 0) {
#ifdef CONFIG_CHARGER_TYPE_RECHECK
        pr_info("slowinsertusb, 900ms overtime!\n");
        type = CHARGER_TYPE_NON_STANDARD;
#else
        type = CHARGER_TYPE_USB;
        printk(KERN_WARNING "%s:datcondet_sts is 0\n",__func__);
#endif
        goto out;
    }

    msleep(20);
    peri_sctrl_ctrl5.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));
    peri_sctrl_ctrl5.reg.picophy_chrgsel = 0;
    peri_sctrl_ctrl5.reg.picophy_vdatsrcend = 1;
    peri_sctrl_ctrl5.reg.picophy_vdatdetenb = 1;
    writel(peri_sctrl_ctrl5.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));
    msleep(20);
    peri_sctrl_stat2.value = readl(SOC_PERI_SCTRL_SC_PERIPH_STAT2_ADDR(g_hiusb_info->pericrg_base));
    if (peri_sctrl_stat2.reg.chgdet_sts == 0) {
#ifdef CONFIG_CHARGER_TYPE_RECHECK
        pr_info("slowinsertusb, pd detect sdp!\n");
#endif
        type = CHARGER_TYPE_USB;  /*SDP*/
        goto out;
    } else {
        type = CHARGER_TYPE_STANDARD;     /*CDP OR DCP*/
    }

    peri_sctrl_ctrl5.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));
    peri_sctrl_ctrl5.reg.picophy_chrgsel = 1;
    peri_sctrl_ctrl5.reg.picophy_vdatsrcend = 1;
    peri_sctrl_ctrl5.reg.picophy_vdatdetenb = 1;
    writel(peri_sctrl_ctrl5.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));
    msleep(20);
    peri_sctrl_stat2.value = readl(SOC_PERI_SCTRL_SC_PERIPH_STAT2_ADDR(g_hiusb_info->pericrg_base));
    if (peri_sctrl_stat2.reg.chgdet_sts == 0) {
        type = CHARGER_TYPE_BC_USB;  /*CDP*/
    } else {
        type = CHARGER_TYPE_STANDARD;     /*DCP*/
    }

out:
    peri_sctrl_ctrl5.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));
    if (CHARGER_TYPE_STANDARD == type){
        peri_sctrl_ctrl5.reg.picophy_chrgsel = 0;
        peri_sctrl_ctrl5.reg.picophy_vdatsrcend = 1;
        peri_sctrl_ctrl5.reg.picophy_vdatdetenb = 1;
        peri_sctrl_ctrl5.reg.picophy_bc_mode = 1;
    }
    else{
        peri_sctrl_ctrl5.reg.picophy_bc_mode = 0;
    }
    peri_sctrl_ctrl5.reg.picophy_dcdenb = 0;
    writel(peri_sctrl_ctrl5.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));

    return type;
}

#ifdef CONFIG_CHARGER_TYPE_RECHECK
int hiusb_charger_detect_charger_type(void)
{
    return detect_charger_type();
}
EXPORT_SYMBOL_GPL(hiusb_charger_detect_charger_type);
#endif

/* ---------------------- usb switch ------------------------------------*/
STATIC int switch_init(struct lm_device *dev)
{
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;
    int ret = 0;

    lm_dev = dev;
    hiusb_info = lm_dev->hiusb_info;


    dev_info(&lm_dev->dev, "%s +.\n", __func__);

    if (hiusb_info->switch_pin1) {
        ret = gpio_request(hiusb_info->switch_pin1, "switch_pin1");
        if (ret < 0) {
            dev_err(&lm_dev->dev, "%s request switch_pin1 failed.\n", __func__);
            goto request_pin1_fail;
        }
        gpio_direction_output(hiusb_info->switch_pin1, 0);
    }

    if (hiusb_info->switch_pin2) {
        ret = gpio_request(hiusb_info->switch_pin2, "switch_pin2");
        if (ret < 0) {
            dev_err(&lm_dev->dev, "%s request switch_pin2 failed.\n", __func__);
            goto request_pin2_fail;
        }
        gpio_direction_output(hiusb_info->switch_pin2, 0);
    }

    if (hiusb_info->switch_id_pin) {
        ret = gpio_request(hiusb_info->switch_id_pin, "switch_id_pin");
        if (ret < 0) {
            dev_err(&lm_dev->dev, "%s request switch_id_pin failed.\n", __func__);
            goto request_id_pin_fail;
        }
        gpio_direction_output(hiusb_info->switch_id_pin, 1);
    }

    if (hiusb_info->cdma_usb_enable) {
        ret = gpio_request(hiusb_info->cdma_usb_enable, "cdma_usb_enable");
        if (ret < 0) {
            dev_err(&lm_dev->dev, "%s request cdma_usb_enable failed.\n", __func__);
            goto request_id_pin_fail;
        }
        gpio_direction_output(hiusb_info->cdma_usb_enable, 0);
    }

    pr_info("%s:switch_pin1:%d, switch_pin2:%d, switch_id_pin:%d, cdma_usb_enable:%d\n", __func__, hiusb_info->switch_pin1, hiusb_info->switch_pin2, hiusb_info->switch_id_pin, hiusb_info->cdma_usb_enable);

    return 0;

request_id_pin_fail:
    gpio_free(hiusb_info->switch_pin2);
request_pin2_fail:
    gpio_free(hiusb_info->switch_pin1);
request_pin1_fail:
    return ret;
}

int switch_reset(void)
{

    unsigned long flags;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return -1;
    }

    pr_info("%s", __func__);
    spin_lock_irqsave(&g_hiusb_info->switch_lock, flags);
    /*todo : D+/D- -- usb | ID -- MHL*/
    if(!use_switch_driver){
	 if (g_hiusb_info->switch_pin1) {
            gpio_direction_output(g_hiusb_info->switch_pin1, 0);
	 }
	 if ( g_hiusb_info->switch_pin2 ) {
            gpio_direction_output(g_hiusb_info->switch_pin2, 0);
	 }
	 if (g_hiusb_info->switch_id_pin) {
            gpio_direction_output(g_hiusb_info->switch_id_pin, 1);
	 }
    }else{
        //add switch default code here
#ifdef CONFIG_SWITCH_FSA9685
        fsa9685_manual_detach();
#endif
    }
    spin_unlock_irqrestore(&g_hiusb_info->switch_lock, flags);

    return 0;
}

int __switch_usb_id(void)
{

    unsigned long flags;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return -1;
    }

    pr_info("%s", __func__);
    /*todo : D+/D- -- usb | ID -- MHL*/
    spin_lock_irqsave(&g_hiusb_info->switch_lock, flags);
    if(!use_switch_driver){
	if (g_hiusb_info->switch_id_pin) {
           gpio_direction_output(g_hiusb_info->switch_id_pin, 0);
	}
    }else{
    }
    spin_unlock_irqrestore(&g_hiusb_info->switch_lock, flags);

    return 0;
}

int __switch_usb(void)
{

    unsigned long flags;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return -1;
    }

    pr_info("%s", __func__);
    /*todo : D+/D- -- usb | ID -- MHL*/
    spin_lock_irqsave(&g_hiusb_info->switch_lock, flags);
    if(!use_switch_driver){
	 if (g_hiusb_info->switch_pin1) {
            gpio_direction_output(g_hiusb_info->switch_pin1, 0);
	 }
	 if ( g_hiusb_info->switch_pin2 ) {
            gpio_direction_output(g_hiusb_info->switch_pin2, 0);
	  }
	  if (g_hiusb_info->switch_id_pin) {
            gpio_direction_output(g_hiusb_info->switch_id_pin, 0);
	 }
    }else{
    }
    spin_unlock_irqrestore(&g_hiusb_info->switch_lock, flags);

    return 0;
}

int switch_usb(void)
{
    int ret = -1;
    if(!use_switch_driver){
        ret = __switch_usb();

        hisi_usb_id_change(ID_FALL_EVENT);
        return ret;
    }else{
        hisi_usb_id_change(ID_FALL_EVENT);
        //just return
        return 0;
    }
}

int __switch_mhl_id(void)
{

    unsigned long flags;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return -1;
    }

    pr_info("%s", __func__);
    /*todo : D+/D- -- usb | ID -- MHL*/
    spin_lock_irqsave(&g_hiusb_info->switch_lock, flags);
    if(!use_switch_driver){
	 if (g_hiusb_info->switch_id_pin) {
            gpio_direction_output(g_hiusb_info->switch_id_pin, 1);
	 }
    }else{
    }
    spin_unlock_irqrestore(&g_hiusb_info->switch_lock, flags);

    return 0;
}

int switch_mhl(void)
{

    unsigned long flags;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return -1;
    }

    pr_info("%s", __func__);
    /*todo : D+/D- -- usb | ID -- MHL*/
    spin_lock_irqsave(&g_hiusb_info->switch_lock, flags);
    if(!use_switch_driver){
	 if (g_hiusb_info->switch_pin1) {
            gpio_direction_output(g_hiusb_info->switch_pin1, 0);
	 }
	 if (g_hiusb_info->switch_pin2) {
            gpio_direction_output(g_hiusb_info->switch_pin2, 1);
	 }
	 if (g_hiusb_info->switch_id_pin) {
            gpio_direction_output(g_hiusb_info->switch_id_pin, 1);
	 }
    }else{
        //wait 30ms for fsa9685 switch
        msleep(30);
    }
    spin_unlock_irqrestore(&g_hiusb_info->switch_lock, flags);

    return 0;
}

int switch_cdma(void)
{

    unsigned long flags;
    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return -1;
    }

    pr_info("%s", __func__);
    /*todo : D+/D- -- usb | ID -- MHL*/
    spin_lock_irqsave(&g_hiusb_info->switch_lock, flags);
    if(!use_switch_driver){
	 if (g_hiusb_info->switch_pin1) {
            gpio_direction_output(g_hiusb_info->switch_pin1, 1);
	 }
	 if (g_hiusb_info->switch_pin2) {
            gpio_direction_output(g_hiusb_info->switch_pin2, 0);
	 }
	 if (g_hiusb_info->switch_id_pin) {
            gpio_direction_output(g_hiusb_info->switch_id_pin, 1);
	 }
    }else{
    }
    spin_unlock_irqrestore(&g_hiusb_info->switch_lock, flags);

    return 0;
}

int switch_off(void)
{

    unsigned long flags;
    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return -1;
    }

    pr_info("%s", __func__);
    /*todo : D+/D- -- usb | ID -- MHL*/
    spin_lock_irqsave(&g_hiusb_info->switch_lock, flags);
    if(!use_switch_driver){
	 if (g_hiusb_info->switch_pin1) {
            gpio_direction_output(g_hiusb_info->switch_pin1, 1);
	 }
	 if ( g_hiusb_info->switch_pin2) {
            gpio_direction_output(g_hiusb_info->switch_pin2, 1);
	 }
	 if (g_hiusb_info->switch_id_pin) {
            gpio_direction_output(g_hiusb_info->switch_id_pin, 0);
	 }
    }else{
    }
    spin_unlock_irqrestore(&g_hiusb_info->switch_lock, flags);

    return 0;
}

int id_state(void)
{
    int id = 0;
    if(!use_switch_driver){
        if (g_hiusb_info->switch_id_pin) {
            id = gpio_get_value_cansleep(g_hiusb_info->switch_id_pin);
        }
    }else{
        id = 1;
    }
    return id;
}

/*sel_state(D+/D-):
 *0 == usb
 *1 == mhl
 *2 == cdma
 *other == off
 */
int sel_state(void)
{
    int sel_state = 0;
    int sel0 = 0;
    int sel1 = 0;

    if (g_hiusb_info->switch_pin1) {
        sel0 = gpio_get_value_cansleep(g_hiusb_info->switch_pin1);
    }
    if (g_hiusb_info->switch_pin2) {
        sel1 = gpio_get_value_cansleep(g_hiusb_info->switch_pin2);
    }

    if (sel0 == 0 && sel1 == 0) {
        sel_state = 0;
    } else if (sel0 == 0 && sel1 == 1) {
        sel_state = 1;
    } else if (sel0 == 1 && sel1 == 0) {
        sel_state = 2;
    } else {
        sel_state = 3;
    }

    return sel_state;
}


void hiusb_hibernation_init(void)
{

    SOC_PERI_SCTRL_SC_PERIPH_CTRL4_UNION peri_sctrl_ctrl4;

    peri_sctrl_ctrl4.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR(g_hiusb_info->pericrg_base));
    peri_sctrl_ctrl4.reg.bc11_c = 0;
    peri_sctrl_ctrl4.reg.bc11_b = 0;
    peri_sctrl_ctrl4.reg.bc11_float = 0;
    writel(peri_sctrl_ctrl4.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR(g_hiusb_info->pericrg_base));
    udelay(10);
    pr_info("%s .\n", __func__);

    return;
}
#if 0 /* no need control pin in v8 */
int hiusb_pinctrl_set(const char *name)
{
    struct pinctrl *p;
    struct pinctrl_state *pinctrl_state;
    int ret = 0;

    /*switch iomux start */
    p = pinctrl_get(&(g_hiusb_info->pdev->dev));
    if (IS_ERR(p)) {
        usb_err("%s :could not get pinctrl\n",__FUNCTION__);
    }

    pinctrl_state = pinctrl_lookup_state(p, name);
    if (IS_ERR(pinctrl_state)) {
        usb_err("%s : could not get defstate (%li)\n",
                __FUNCTION__ , PTR_ERR(pinctrl_state));
    }
    ret = pinctrl_select_state(p, pinctrl_state);
    if (ret) {
        usb_err("%s : could not set pins to default state\n", __FUNCTION__);
    }

    return ret;
    /* switch iomux end */
}
#endif
void hiusb_otg_and_phy_setup(int mode)
{
    int i;
    SOC_PERI_SCTRL_SC_PERIPH_CTRL4_UNION peri_sctrl_ctrl4;
    SOC_PERI_SCTRL_SC_PERIPH_CTRL5_UNION peri_sctrl_ctrl5;
    int ret;
    unsigned int eye_pattern_val = 0x7053348c;
    unsigned int otg_eye_pattern_val = 0x7053348c;
#if 0 /* no need config pin in v8 */
    ret = hiusb_pinctrl_set(PINCTRL_STATE_DEFAULT);
    if(ret){
        usb_err("%s : hiusb_pinctrl_set:%d\n", __FUNCTION__,ret);
    }
#endif
    /* nrst */
    for (i=0;hiusb_nrst[i].addr != NULL; i++) {
        writel(hiusb_nrst[i].value, hiusb_nrst[i].addr);
        udelay(hiusb_nrst[i].udelay);
        usb_dbg("%s nrst addr:%p value:0x%8.8x udelay:%d\n",
                __func__,
                hiusb_nrst[i].addr,
                hiusb_nrst[i].value,
                hiusb_nrst[i].udelay);
    }

    /* ctrl 5 */
    peri_sctrl_ctrl5.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));
    peri_sctrl_ctrl5.reg.picophy_bc_mode = 0;
    peri_sctrl_ctrl5.reg.usbotg_res_sel = 1;
    peri_sctrl_ctrl5.reg.picophy_acaenb = 1;

    writel(peri_sctrl_ctrl5.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));

    /* ctrl 4 */
    peri_sctrl_ctrl4.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR(g_hiusb_info->pericrg_base));
    /*w00140341 B030 modify*/
    peri_sctrl_ctrl4.reg.pico_siddq = 0;
    peri_sctrl_ctrl4.reg.pico_ogdisable = 0;
    peri_sctrl_ctrl4.reg.otg_phy_sel = 1;
    peri_sctrl_ctrl4.reg.pico_vbusvldextsel = 1;
    peri_sctrl_ctrl4.reg.pico_vbusvldext = 1;
    writel(peri_sctrl_ctrl4.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR(g_hiusb_info->pericrg_base));
    udelay(60);

    if(0 == mode){
        if(0 == g_hiusb_info->eye_pattern)
            writel(eye_pattern_val, SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR(g_hiusb_info->pericrg_base));
        else
            writel(g_hiusb_info->eye_pattern, SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR(g_hiusb_info->pericrg_base));
    }
    else{
        if(0 == g_hiusb_info->otg_eye_pattern)
            writel(otg_eye_pattern_val, SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR(g_hiusb_info->pericrg_base));
        else
            writel(g_hiusb_info->otg_eye_pattern, SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR(g_hiusb_info->pericrg_base));
    }

    // clk_enable();
    if(g_hiusb_info->bus_low_add_request)
    {
        g_hiusb_info->bus_low_add_request(g_hiusb_info->lm_dev);
    }

    ret = clk_prepare_enable(g_hiusb_info->clk_usbotg_off);
    if (ret) {
        printk("%s:failed to clk_prepare_enable clk_usbotg_off\n", __func__);
    }

    ret = pwrctrl_request_power_state(PWRCTRL_SLEEP_USB,PWRCTRL_SYS_STAT_S1,&(g_hiusb_info->power_qos_id));
    if (ret) {
        printk("%s:failed to pwrctrl_request_power_state\n", __func__);
    }

    hiusb_system_info();

    pr_info("%s .\n", __func__);

    return;
}

void hiusb_otg_and_phy_cleanup(void)
{
    int i;
    SOC_PERI_SCTRL_SC_PERIPH_CTRL4_UNION peri_sctrl_ctrl4;

    int ret;

    ret = pwrctrl_release_power_state(g_hiusb_info->power_qos_id);
    if (ret) {
        printk("%s:failed to pwrctrl_release_power_state\n", __func__);
    }

    if(g_hiusb_info->bus_low_remove_request)
    {
        g_hiusb_info->bus_low_remove_request(g_hiusb_info->lm_dev);
    }

    /* clk_disable(); */
    clk_disable_unprepare(g_hiusb_info->clk_usbotg_off);

    /* ctrl 4 */
    peri_sctrl_ctrl4.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR(g_hiusb_info->pericrg_base));
    peri_sctrl_ctrl4.reg.pico_siddq = 1;
    writel(peri_sctrl_ctrl4.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR(g_hiusb_info->pericrg_base));

    /* rst */
    for (i=0;hiusb_rst[i].addr != NULL; i++) {
        writel(hiusb_rst[i].value, hiusb_rst[i].addr);
        udelay(hiusb_rst[i].udelay);
        usb_dbg("%s rst addr:%p value:0x%8.8x udelay:%d\n",
                __func__,
                hiusb_rst[i].addr,
                hiusb_rst[i].value,
                hiusb_rst[i].udelay);
    }
#if 0 /* no need config pin in v8 */
    ret = hiusb_pinctrl_set(PINCTRL_STATE_DEFAULT);
    if(ret){
        usb_err("%s : hiusb_pinctrl_set:%d\n", __FUNCTION__,ret);
    }
#endif
    hiusb_system_info();
    pr_info("%s -.\n", __func__);
    return;
}

void test_wake_unlock_usb(void)
{
    dev_info(&g_hiusb_info->lm_dev->dev, "%s +.\n", __func__);
    wake_unlock(&g_hiusb_info->dev_wakelock);
    dev_info(&g_hiusb_info->lm_dev->dev, "%s -.\n", __func__);
}

static void release_wakelock_do_disconnect(struct lm_device *lm_dev)
{
    struct hiusb_info *hiusb_info = lm_dev->hiusb_info;
    SOC_PERI_SCTRL_SC_PERIPH_CTRL5_UNION peri_sctrl_ctrl5 = {0};

    if (down_interruptible(&hiusb_info->hiusb_info_sema)) {
        dev_err(&lm_dev->dev, "%s down fail.\n", __func__);
        return;
    }

    if (hiusb_info->hiusb_status != HIUSB_DEVICE) {
        dev_err(&lm_dev->dev, "%s charge DRAW wrong status error:(%d)%s.\n",
            __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error");

        goto FuncEnd;
    }

    dev_dbg(&lm_dev->dev, "%s it needs disable bc mode\n",__func__);
    peri_sctrl_ctrl5.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(hiusb_info->pericrg_base));
    peri_sctrl_ctrl5.reg.picophy_bc_mode = 0;
    writel(peri_sctrl_ctrl5.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));

    if (CHARGER_TYPE_STANDARD != hiusb_info->charger_type){
        dwc_dev_draw_init(lm_dev);
    }

    if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
        hiusb_info->charger_type = CHARGER_REMOVED;
        notify_charger_type();
    }
    hiusb_otg_and_phy_cleanup();
    __switch_mhl_id();
    wake_unlock(&hiusb_info->dev_wakelock);

    dev_dbg(&lm_dev->dev, "%s status:(%d)%s -> %s .\n",
        __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error", usb_status[HIUSB_OFF]);
    hiusb_info->hiusb_status = HIUSB_OFF;
    if(hiusb_info->otg_int_gpio)
        enable_irq(gpio_to_irq(hiusb_info->otg_int_gpio));

#ifdef CONFIG_CHARGER_TYPE_RECHECK
    g_charger_usb_checked = 0;
#endif

FuncEnd:
    up(&hiusb_info->hiusb_info_sema);
    return;
}

STATIC void hiusb_otg_intr_work(struct work_struct *work)
{
    struct hiusb_info *hiusb_info;
    struct lm_device *lm_dev;
    int intr_flag;
    unsigned long flags;
    SOC_PERI_SCTRL_SC_PERIPH_CTRL5_UNION peri_sctrl_ctrl5;

    hiusb_info = container_of(to_delayed_work(work),
            struct hiusb_info, otg_intr_work);
    lm_dev = hiusb_info->lm_dev;
    dev_info(&lm_dev->dev, "%s +.\n", __func__);

    if (down_interruptible(&hiusb_info->hiusb_info_sema)) {
        dev_err(&lm_dev->dev, "%s down fail.\n", __func__);
        return;
    }

    spin_lock_irqsave(&hiusb_info->intr_flag_lock, flags);
    intr_flag = hiusb_info->intr_flag;
    spin_unlock_irqrestore(&hiusb_info->intr_flag_lock, flags);

    switch (intr_flag) {
        case CHARGER_CONNECT_EVENT:
            if ((0 != pmu_version()) && (vbus_status() == 0)) {
                dev_err(&lm_dev->dev, "%s charge INSERT no vbus error.\n", __func__);
                spin_lock(&hiusb_info->intr_flag_lock);
                hiusb_info->intr_flag = CHARGER_DISCONNECT_EVENT;
                spin_unlock(&hiusb_info->intr_flag_lock);
                schedule_delayed_work(&hiusb_info->otg_intr_work, 0);
                break;
            }
            if (hiusb_info->hiusb_status != HIUSB_OFF ) {
                dev_err(&lm_dev->dev, "%s charge INSERT wrong status error:(%d)%s.\n",
                    __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error");
                break;
            }

            wake_lock(&hiusb_info->dev_wakelock);
            /* usb sc init */
            hiusb_otg_and_phy_setup(0);
            /* detect charger */
            if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                hiusb_info->charger_type = detect_charger_type();
                notify_charger_type();
            }

            if (CHARGER_TYPE_STANDARD == hiusb_info->charger_type){
                dev_dbg(&lm_dev->dev, "%s hiusb_info->charger_type is CHARGER_TYPE_STANDARD,it don't need init usb ip\n",__func__);
            }
            else{
                /* usb ip init */
                if(0 == dwc_dev_insert_init(lm_dev)){
                    dev_dbg(&lm_dev->dev, "%s status: (%d)%s -> %s .\n",
                        __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error", usb_status[HIUSB_DEVICE]);
                }
                else{
                    dev_err(&lm_dev->dev, "%s dwc_dev_insert_init error .\n", __func__);
                    if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                        hiusb_info->charger_type = CHARGER_REMOVED;
                        notify_charger_type();
                     }
                     hiusb_otg_and_phy_cleanup();
                     wake_unlock(&hiusb_info->dev_wakelock);
                     break;
                }
            }

            hiusb_info->hiusb_status = HIUSB_DEVICE;
            if(hiusb_info->otg_int_gpio)
                disable_irq_nosync(gpio_to_irq(hiusb_info->otg_int_gpio));
            break;
        case CHARGER_DISCONNECT_EVENT:
            if ((0 != pmu_version()) && (vbus_status() != 0)) {
                dev_err(&lm_dev->dev, "%s charge DRAW have vbus error.\n", __func__);
                spin_lock(&hiusb_info->intr_flag_lock);
                hiusb_info->intr_flag = CHARGER_CONNECT_EVENT;
                spin_unlock(&hiusb_info->intr_flag_lock);
                schedule_delayed_work(&hiusb_info->otg_intr_work, 0);
                break;
            }
            if (hiusb_info->hiusb_status != HIUSB_DEVICE) {
                dev_err(&lm_dev->dev, "%s charge DRAW wrong status error:(%d)%s.\n",
                    __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error");
                break;
            }

            dev_dbg(&lm_dev->dev, "%s it need disable bc mode\n",__func__);
            peri_sctrl_ctrl5.value = readl(SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(hiusb_info->pericrg_base));
            peri_sctrl_ctrl5.reg.picophy_bc_mode = 0;
            writel(peri_sctrl_ctrl5.value , SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(g_hiusb_info->pericrg_base));

            if (CHARGER_TYPE_STANDARD != hiusb_info->charger_type){
                dwc_dev_draw_init(lm_dev);
            }

            if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                hiusb_info->charger_type = CHARGER_REMOVED;
                notify_charger_type();
            }
            hiusb_otg_and_phy_cleanup();
            __switch_mhl_id();
            wake_unlock(&hiusb_info->dev_wakelock);

            dev_dbg(&lm_dev->dev, "%s status:(%d)%s -> %s .\n",
                    __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error", usb_status[HIUSB_OFF]);
            hiusb_info->hiusb_status = HIUSB_OFF;
            if(hiusb_info->otg_int_gpio)
                enable_irq(gpio_to_irq(hiusb_info->otg_int_gpio));
#ifdef CONFIG_CHARGER_TYPE_RECHECK
            g_charger_usb_checked = 0;
#endif
            break;
        case ID_FALL_EVENT:
            if (hiusb_info->hiusb_status != HIUSB_OFF) {
                dev_err(&lm_dev->dev, "%s ID_FALL in wrong status error:(%d)%s.\n",
                    __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error");
                break;
            }
            if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                hiusb_info->charger_type = USB_EVENT_OTG_ID;
                notify_charger_type();
            }

            wake_lock(&hiusb_info->host_wakelock);
            if (hiusb_info->vbus_pin) {
                gpio_direction_output(hiusb_info->vbus_pin, 1);
            }
            hiusb_otg_and_phy_setup(1);
            if(0 == dwc_host_insert_init(lm_dev)){
                dev_dbg(&lm_dev->dev, "%s status:(%d)%s -> %s .\n",
                        __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error", usb_status[HIUSB_HOST]);
                hiusb_info->hiusb_status = HIUSB_HOST;
            } else {
                dev_err(&lm_dev->dev, "%s dwc_host_insert_init error .\n", __func__);
                if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                    hiusb_info->charger_type = CHARGER_REMOVED;
                    notify_charger_type();
                }
                hiusb_otg_and_phy_cleanup();
                wake_unlock(&hiusb_info->host_wakelock);
            }
            break;
        case ID_RISE_EVENT:
            if (hiusb_info->hiusb_status != HIUSB_HOST) {
                dev_err(&lm_dev->dev, "%s ID_RISE in wrong status error:(%d)%s.\n",
                    __func__, hiusb_info->hiusb_status,hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error");
#ifdef CONFIG_SWITCH_FSA9685
                fsa9685_manual_detach();
#endif
                break;
            }
            dwc_host_draw_init(lm_dev);
            if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                hiusb_info->charger_type = CHARGER_REMOVED;
                notify_charger_type();
            }
            hiusb_otg_and_phy_cleanup();
            if (hiusb_info->vbus_pin) {
                gpio_direction_output(hiusb_info->vbus_pin, 0);
            }
            switch_reset();
            wake_unlock(&hiusb_info->host_wakelock);

            dev_dbg(&lm_dev->dev, "%s status:(%d)%s -> %s .\n",
                    __func__, hiusb_info->hiusb_status, hiusb_info->hiusb_status <= HIUSB_MAX_STATUS ? usb_status[hiusb_info->hiusb_status]:"error", usb_status[HIUSB_OFF]);
            hiusb_info->hiusb_status = HIUSB_OFF;
#ifdef CONFIG_CHARGER_TYPE_RECHECK
            g_charger_usb_checked = 0;
#endif
            break;
        default:
            dev_err(&lm_dev->dev, "%s error flag.\n", __func__);
            break;
    }
    up(&hiusb_info->hiusb_info_sema);
    dev_info(&lm_dev->dev, "%s done.\n", __func__);
    return;
}

int hisi_usb_id_change(enum otg_dev_event_type flag)
{
    struct hiusb_info *hiusb_info;

    if (NULL ==  g_hiusb_info) {
       pr_err("%s: g_hiusb_info not init!\n", __func__);
       return -1;
    }

    if (flag != ID_FALL_EVENT && flag != ID_RISE_EVENT)
    {
        return -EINVAL;
    }
    pr_debug("%s +.\n", __func__);
    hiusb_info = g_hiusb_info;
    spin_lock(&hiusb_info->intr_flag_lock);
    hiusb_info->intr_flag = flag;
    spin_unlock(&hiusb_info->intr_flag_lock);

    schedule_delayed_work(&hiusb_info->otg_intr_work, 0);
    pr_debug("%s %d -.\n", __func__, flag);

    return 0;
}

irqreturn_t hiusb_vbus_intr(int irq, void *dev)
{
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = lm_dev->hiusb_info;
    dev_dbg(&lm_dev->dev, "%s +.\n", __func__);

    spin_lock(&hiusb_info->intr_flag_lock);
    if (irq == hiusb_info->insert_irq) {
        hiusb_info->intr_flag = CHARGER_CONNECT_EVENT;
    } else {
        hiusb_info->intr_flag = CHARGER_DISCONNECT_EVENT;
    }
    spin_unlock(&hiusb_info->intr_flag_lock);

    if( use_switch_driver && irq == hiusb_info->insert_irq )
        schedule_delayed_work(&hiusb_info->otg_intr_work, msecs_to_jiffies(30) );
    else
    schedule_delayed_work(&hiusb_info->otg_intr_work, 0 );

    dev_dbg(&lm_dev->dev, "%s irq(%d) %s %s -.\n", __func__, irq, "Vbus", (irq==hiusb_info->insert_irq) ? "insert":"remove");
    return IRQ_HANDLED;
}

irqreturn_t hiusb_otggpio_intr(int irq, void *dev)
{
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = lm_dev->hiusb_info;
    dev_dbg(&lm_dev->dev, "%s +.\n", __func__);

    if(1==hiusb_info->otg_without_mhl) {
        if ((irq == gpio_to_irq(hiusb_info->otg_int_gpio))  &&
            (0 == gpio_get_value_cansleep(hiusb_info->otg_int_gpio))) {
            switch_usb();
        }
    }

    dev_dbg(&lm_dev->dev, "%s irq %d -.\n", __func__, irq);
    return IRQ_HANDLED;
}

STATIC int hiusb_init_resource(struct lm_device *dev)
{
    int ret = 0;
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = dev->hiusb_info;

    ATOMIC_INIT_NOTIFIER_HEAD(&hiusb_info->charger_type_notifier_head);
    spin_lock_init(&hiusb_info->intr_flag_lock);
    wake_lock_init(&hiusb_info->dev_wakelock, WAKE_LOCK_SUSPEND, "hiusb_dev_wakelock");
    wake_lock_init(&hiusb_info->host_wakelock, WAKE_LOCK_SUSPEND, "hiusb_host_wakelock");
    sema_init(&hiusb_info->hiusb_info_sema, 0);
    spin_lock_init(&hiusb_info->switch_lock);
    hiusb_info->charger_type = CHARGER_REMOVED;
    hiusb_info->hiusb_status = HIUSB_OFF;
    hiusb_info->gadget_enable = 0;
    hiusb_info->pm_qos_req.pm_qos_class = 0;

    ret = get_resource(hiusb_info);
    if (ret) {
        usb_err("get resource failed!\n");
        return ret;
    }
#if 0 /* no need config pin in v8 */
    ret = hiusb_pinctrl_set(PINCTRL_STATE_DEFAULT);
    if(ret){
        usb_err("%s : hiusb_pinctrl_set:%d\n", __FUNCTION__,ret);
        goto request_vbus_pin_fail;
    }
#endif

    switch_init(lm_dev);

    if (hiusb_info->vbus_pin) {
        ret = gpio_request(hiusb_info->vbus_pin, "USB_VBUS");
        if (ret < 0) {
            dev_err(&dev->dev, "%s usb vbus gpio error", __func__);
            goto request_vbus_pin_fail;
        }
        gpio_direction_output(hiusb_info->vbus_pin, 0);
    }

    if (hiusb_info->phy_reset_pin) {
        ret = gpio_request(hiusb_info->phy_reset_pin, "USBPHY_REST");
        if (ret < 0) {
            dev_err(&dev->dev, "%s usb phy reset gpio error", __func__);
            goto request_phy_reset_pin_fail;
        }
        gpio_direction_output(hiusb_info->phy_reset_pin, 0);
        mdelay(100);
        gpio_direction_output(hiusb_info->phy_reset_pin, 1);
    }

    if (hiusb_info->otg_int_gpio) {
        ret = gpio_request(hiusb_info->otg_int_gpio, "OTG_INT_GPIO");
        if (ret < 0) {
            dev_err(&dev->dev, "%s usb otg int gpio error", __func__);
            goto request_otg_int_gpio_fail;
        }
        gpio_direction_input(hiusb_info->otg_int_gpio);
    }

    /* init workqueue for usb insert/draw : only for debug */
    INIT_DELAYED_WORK(&hiusb_info->otg_intr_work, hiusb_otg_intr_work);
    dev_info(&lm_dev->dev, "%s.\n", __func__);

    return 0;

request_otg_int_gpio_fail:
    if (hiusb_info->phy_reset_pin) {
        gpio_free(hiusb_info->phy_reset_pin);
    }
request_phy_reset_pin_fail:
    if (hiusb_info->vbus_pin) {
        gpio_free(hiusb_info->vbus_pin);
    }
request_vbus_pin_fail:
    clk_put(hiusb_info->clk_usbotg_off);
    return ret;

}

STATIC int hiusb_free_resource(struct lm_device *dev)
{
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = dev->hiusb_info;

    clk_put(hiusb_info->clk_usbotg_off);
    if (hiusb_info->vbus_pin) {
        gpio_free(hiusb_info->vbus_pin);
    }
    if (hiusb_info->phy_reset_pin) {
        gpio_free(hiusb_info->phy_reset_pin);
    }
    if (hiusb_info->otg_int_gpio) {
        gpio_free(hiusb_info->otg_int_gpio);
    }
    if (hiusb_info->switch_pin1) {
        gpio_free(hiusb_info->switch_pin1);
    }
    if (hiusb_info->switch_pin2) {
        gpio_free(hiusb_info->switch_pin2);
    }
    if (hiusb_info->cdma_usb_enable) {
        gpio_free(hiusb_info->cdma_usb_enable);
    }
    if (hiusb_info->switch_id_pin) {
        gpio_free(hiusb_info->switch_id_pin);
    }
    dev_info(&lm_dev->dev, "%s.\n", __func__);
    return 0;
}

STATIC int hiusb_request_irq(struct lm_device *dev)
{
    int ret = 0;
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = dev->hiusb_info;

    /* request otg gpio int irq */
    if(hiusb_info->otg_int_gpio) {
        gpio_direction_input(hiusb_info->otg_int_gpio);
        ret = request_irq(gpio_to_irq(hiusb_info->otg_int_gpio), hiusb_otggpio_intr,
            IRQF_NO_SUSPEND|IRQF_TRIGGER_FALLING, "hiusb_otggpio_intr", lm_dev);
        if (ret) {
            dev_err(&dev->dev, "request gpio irq failed.\n");
            return ret;
        }

    }

    dev_info(&dev->dev, "request gpio: %d irq\n", hiusb_info->otg_int_gpio);
    return 0;
}

STATIC int hiusb_free_irq(struct lm_device *dev)
{
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = dev->hiusb_info;

    if (hiusb_info->insert_irq == 0 || hiusb_info->draw_irq == 0
            || (hiusb_info->quirks & HIUSB_QUIRKS_PMUIRQ) == 0) {
        dev_info(&lm_dev->dev, "No insert/draw irq.\n");
    } else {
        //free_irq(hiusb_info->insert_irq, dev);
        //free_irq(hiusb_info->draw_irq, dev);
    }
    dev_info(&lm_dev->dev, "%s.\n", __func__);
    return 0;
}

STATIC int hiusb_pullup(struct lm_device *dev,int is_on)
{
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = dev->hiusb_info;

    if (is_on) {
        hiusb_info->gadget_enable = 1;
    } else {
        hiusb_info->gadget_enable = 0;
    }

    dev_dbg(&lm_dev->dev, "%s !\n", __func__);

    return 0;
}

STATIC int hiusb_suspend(struct lm_device *dev)
{
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = dev->hiusb_info;
    __switch_usb_id();

    dev_dbg(&lm_dev->dev, "%s !\n", __func__);

    return 0;
}

STATIC int hiusb_resume(struct lm_device *dev)
{
    struct lm_device *lm_dev;

    struct hiusb_info *hiusb_info;
    int state;

    lm_dev = dev;
    hiusb_info = dev->hiusb_info;
    /* nothing to do */
    state = sel_state();
    if (state == 1) {
        switch_reset();
    } else {
        __switch_mhl_id();
    }

    if (release_wakelock)
    {
        hiusb_info->hiusb_status = HIUSB_OFF;
        spin_lock(&hiusb_info->intr_flag_lock);
        hiusb_info->intr_flag = CHARGER_CONNECT_EVENT;
        spin_unlock(&hiusb_info->intr_flag_lock);
        
        release_wakelock = 0;
        schedule_delayed_work(&hiusb_info->otg_intr_work, 0);
    }

    dev_dbg(&lm_dev->dev, "%s !\n", __func__);

    return 0;
}


STATIC int hiusb_init_phase2(struct lm_device *dev)
{
    int ret;
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;
    unsigned int otg_eye_pattern_val = 0x7053348c;
    
    lm_dev = dev;
    hiusb_info = lm_dev->hiusb_info;

    ret = hiusb_request_irq(lm_dev);
    if (ret < 0) {
        dev_err(&dev->dev, "hiusb_request_irq failed!\n");
        goto request_irq_fail;
    }

    /* init finish */


    if (hiusb_is_host_mode(lm_dev)) {
        if(hiusb_info->otg_int_gpio)
        {
            dev_info(&dev->dev, "host wakelock!\n");
            wake_lock(&hiusb_info->host_wakelock);
            if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                hiusb_info->charger_type = USB_EVENT_OTG_ID;
                notify_charger_type();
            }
            if(0 == hiusb_info->otg_eye_pattern)
                writel(otg_eye_pattern_val, SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR(hiusb_info->pericrg_base));
            else
                writel(hiusb_info->otg_eye_pattern, SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR(hiusb_info->pericrg_base));
            hiusb_info->hiusb_status = HIUSB_HOST;
        }
        else
        {
            dev_info(&dev->dev, "can't support OTG, usb disconnect!\n");
            hiusb_otg_and_phy_cleanup();
            wake_unlock(&hiusb_info->dev_wakelock);
            if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                hiusb_info->charger_type = CHARGER_REMOVED;
                notify_charger_type();
            }
            hiusb_info->hiusb_status = HIUSB_OFF;
        }
    } else if ((pmu_version() != 0) && (vbus_status() == 0)) {
        dev_info(&dev->dev, "usb disconnect!\n");
        hiusb_otg_and_phy_cleanup();
        wake_unlock(&hiusb_info->dev_wakelock);
        if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
            hiusb_info->charger_type = CHARGER_REMOVED;
            notify_charger_type();
        }
        hiusb_info->hiusb_status = HIUSB_OFF;
    } else {
        hiusb_info->hiusb_status = HIUSB_DEVICE;
        dwc_pm_runtime_disable(lm_dev);
        if(hiusb_info->otg_int_gpio)
            disable_irq_nosync(gpio_to_irq(hiusb_info->otg_int_gpio));
        if (hiusb_info->quirks & HIUSB_QUIRKS_CHARGER) {
                hiusb_info->charger_type = detect_charger_type();
                notify_charger_type();
        }
    }

    up(&hiusb_info->hiusb_info_sema);

    dev_info(&lm_dev->dev, "%s done.\n", __func__);

    return 0;

request_irq_fail:
    hiusb_free_resource(lm_dev);
    return ret;
}

STATIC int hiusb_remove(struct lm_device *dev)
{
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;

    lm_dev = dev;
    hiusb_info = lm_dev->hiusb_info;
    hiusb_attr_remove(lm_dev);
    hiusb_free_resource(lm_dev);
    hiusb_free_irq(lm_dev);
    return 0;
}


void hiusb_bus_low_add_request(struct lm_device *dev)
{
    struct hiusb_info *hiusb_info;
    hiusb_info = dev->hiusb_info;
    
    dev_info(&dev->dev, "add request!\n");
    


    if(pm_qos_request_active(&hiusb_info->pm_qos_req)){
        printk(KERN_ERR "hiusb_bus_low_add_request, already add request! \n");
        return;
    }

    pm_qos_add_request(&hiusb_info->pm_qos_req, PM_QOS_BUSLOW_MINFREQ, hiusb_info->clk_rate);
}

void hiusb_bus_low_remove_request(struct lm_device *dev)
{
    struct hiusb_info *hiusb_info;
    hiusb_info = dev->hiusb_info;

    dev_info(&dev->dev, "remove request!\n");


    if (!pm_qos_request_active(&hiusb_info->pm_qos_req)){
        printk(KERN_ERR "hiusb_bus_low_remove_request, remove request which hadn't been added!!! \n");
        return;
    }

    pm_qos_remove_request(&hiusb_info->pm_qos_req);
}


/* ---------------------- usb oem -------------------------------------*/
STATIC struct hiusb_info hiusb_oem_info = {
    .phy_reset_pin              = 0,
    .switch_pin1                = 0,
    .switch_pin2                = 0,
    .switch_id_pin              = 0,
    .cdma_usb_enable            = 0,
    .otg_int_gpio               = 0,
    .otg_without_mhl            = 0,
    .eye_pattern                = 0,
    .insert_irq                 = 0,
    .draw_irq                   = 0,
    .power_qos_id          = PWRCTRL_POWER_STAT_INVALID_ID,
    .quirks                     = HIUSB_QUIRKS_CHARGER | HIUSB_QUIRKS_PMUIRQ,
    .init_phase1                = NULL,
    .init_phase2                = hiusb_init_phase2,
    .pullup                     = hiusb_pullup,
    .remove                     = hiusb_remove,
    .suspend                    = hiusb_suspend,
    .resume                     = hiusb_resume,
    .bus_low_add_request      = hiusb_bus_low_add_request,
    .bus_low_remove_request   = hiusb_bus_low_remove_request,
};

STATIC int dwc_otg_hi6xxx_probe(struct platform_device *pdev)
{
    int ret = 0;
    int irq = 0;
    struct lm_device *lm_dev;
    struct hiusb_info *hiusb_info;
    struct resource *res;
    int powerdown_charge_type = 0;

    usb_dbg("[dwc_otg_hi6xxx_probe enter\n");
    if(!strcmp(run_mode,"factory"))
    {
         usb_err("%s factory version enable otg. \n", __func__);
		 g_otg_enable = 1;
    }
	else
    {
         powerdown_charge_type = get_pd_charge_flag();
         usb_err("%s normal boot---pd_charge_type=%d,%d \n", __func__,powerdown_charge_type,oem_otg_enable);
         if(oem_otg_enable && (1 != powerdown_charge_type))
         {
             usb_err("%s otg enable\n", __func__);
             g_otg_enable = 1;
         }
		 else
		 {
		     usb_err("%s otg disable\n", __func__);
			 g_otg_enable = 0;
		 }
    }
    /*
     * register the dwc otg device
     */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        usb_err("get resource failed!\n");
        ret = -EBUSY;
        goto error;
    }
#ifdef CONFIG_ARM64
    usb_dbg("res start: 0x%llx, res end: 0x%llx, res flags: %ld\n",
        res->start, res->end, res->flags);
#else
    usb_dbg("res start: 0x%x, res end: 0x%x, res flags: %ld\n",
        res->start, res->end, res->flags);
#endif

    irq = platform_get_irq(pdev, 0);
    if (!irq) {
        usb_err("get irq failed!\n");
        ret = -EBUSY;
        goto error;
    }

    usb_dbg("res irq: %d\n", irq);

    lm_dev = devm_kzalloc(&pdev->dev, sizeof(*lm_dev), GFP_KERNEL);
    if (!lm_dev) {
        usb_err("alloc lm_dev failed! not enough memory\n");
        ret = -ENOMEM;
        goto error;
    }

    lm_dev->hiusb_info = &hiusb_oem_info;
    hiusb_info = lm_dev->hiusb_info;
    hiusb_info->lm_dev = lm_dev;
    platform_set_drvdata(pdev, hiusb_info);
    hiusb_info->pdev = pdev;
    /* only for charger */
    g_hiusb_info =  lm_dev->hiusb_info;

    lm_dev->resource.start = res->start;
    lm_dev->resource.end = res->end;
    lm_dev->resource.flags = res->flags;
    lm_dev->irq = irq;
    lm_dev->id = -1;
    lm_dev->dev.init_name = HI6xxx_USB_NAME;

    /*init resource*/
    ret = hiusb_init_resource(lm_dev);
    if (ret < 0) {
        dev_err(&lm_dev->dev, "hiusb_init_resource failed!\n");
        goto error;
    }

    hiusb_otg_and_phy_setup(0);

    if (pmu_version() == 0) {
        dev_info(&lm_dev->dev, "pmu ec before!\n");
        dev_info(&lm_dev->dev, "dev wakelock!\n");
        wake_lock(&hiusb_info->dev_wakelock);
    } else {
        dev_info(&lm_dev->dev, "pmu ec after!\n");
        if (vbus_status() != 0) {
            dev_info(&lm_dev->dev, "dev wakelock!\n");
            wake_lock(&hiusb_info->dev_wakelock);
        }
    }

    ret = lm_device_register(lm_dev);
    if (ret) {
        usb_err("register dwc otg device failed\n");
        goto lm_device_register_error;
    }
    usb_dbg("register dwc otg device done\n");
    hiusb_attr_create(lm_dev);
    dev_info(&lm_dev->dev, "%s done.\n", __func__);

    return 0;
lm_device_register_error:
    hiusb_free_resource(lm_dev);
error:
    return ret;
}



#ifdef CONFIG_SWITCH_USB_CLS
static int __init plat_usb_dev_init(void)
{
    int ret = 0;

    ret = platform_add_devices(plat_usb_dev, ARRAY_SIZE(plat_usb_dev));

    return ret;
};

device_initcall(plat_usb_dev_init);
#endif

#ifdef CONFIG_OF
static const struct of_device_id dwc_otg_hi6xxx_mach[] = {
    { .compatible = "hisilicon,hi6xxx-usb-otg" },
    {},
};
MODULE_DEVICE_TABLE(of, dwc_otg_hi6xxx_mach);
#else
#define dwc_otg_hi6xxx_mach NULL
#endif

static struct platform_driver dwc_otg_hi6xxx_driver = {
    .probe        = dwc_otg_hi6xxx_probe,
    /*.remove        = hiusb_remove,*/
    .driver        = {
        .name    = "hi6xxx_otg",
        .of_match_table = of_match_ptr(dwc_otg_hi6xxx_mach),
    },
};

module_platform_driver(dwc_otg_hi6xxx_driver);

/*w00140341 wait for solution */
int hisi_charger_type_notifier_register(struct notifier_block *nb)
{
    int ret = -1;
    return ret;
}

int hisi_charger_type_notifier_unregister(struct notifier_block *nb)
{
    int ret = -1;
    return ret;
}

enum hisi_charger_type hisi_get_charger_type(void)
{
    return 0;
}

int hisi_usb_otg_event(enum otg_dev_event_type event_type)
{
    return 0;
}
static int __init early_parse_run_mode_cmdline(char * p)
{
    if(!p)
    {
        printk(KERN_INFO "[USB] get run mode fail!");
        return 0;
    }
    strncpy(run_mode, p, RUN_MODE_LEN);
	printk(KERN_INFO "otg status p:%s, run_mode :%s\n", p, run_mode);
    return 0;
}
early_param("androidboot.swtype", early_parse_run_mode_cmdline);

static int __init early_parse_otg_status_cmdline(char * p)
{
    if(p)
    {
        if(!strcmp(p,"enable"))
        {
            oem_otg_enable = 1;
        }
        else
        {
            oem_otg_enable = 0;
        }

        printk(KERN_INFO "otg status p:%s, oem_otg_enable :%u\n", p, oem_otg_enable);
    }

    return 0;

}
early_param("otg.status", early_parse_otg_status_cmdline);

MODULE_AUTHOR("wangbinghui<wangbinghui@huawei.com>");
MODULE_DESCRIPTION("HI6xxx usb otg driver");
MODULE_LICENSE("GPL V2");

