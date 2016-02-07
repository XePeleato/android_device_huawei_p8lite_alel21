#include <linux/pm_runtime.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include "dwc_otg_regs.h"
#include "dwc_otg_cil.h"
#include "dwc_otg_driver.h"
#include "dwc_otg_pcd.h"
#include "dwc_otg_hcd.h"
#include "dwc_otg_os_dep.h"
#include "lm.h"
#include <linux/huawei/usb/hisi_susb.h>
#include <linux/interrupt.h>
#include <linux/module.h>


static struct device *dwc_get_hcd_device(struct lm_device *lm_dev);
extern struct hiusb_info *g_hiusb_info;
extern irqreturn_t hiusb_vbus_intr(int irq, void *dev);
extern unsigned char vbus_status(void);

/* get the device handler from the hcd which the usb device acts as */
static struct device *dwc_get_hcd_device(struct lm_device *lm_dev)
{
    dwc_otg_device_t *dwc_otg_dev;
    struct dwc_otg_hcd *otg_hcd = NULL;
    struct usb_hcd *hcd = NULL;
    struct usb_device *udev = NULL;
    struct device *dev = NULL;

    dwc_otg_dev = lm_get_drvdata(lm_dev);
    if (dwc_otg_dev) {
        otg_hcd = dwc_otg_dev->hcd;
        if (otg_hcd) {
            hcd = (struct usb_hcd *)(otg_hcd->priv);
            if (hcd) {
                udev = hcd->self.root_hub;
                if (udev) {
                    dev = &(udev->dev);
                    return dev;
                }
            }
        }
    }
    dev_err(&lm_dev->dev, "%s: fail.\n", __func__);
    return 0;
}

/* Enable USB pm runtime suspend */
int dwc_pm_runtime_enable(struct lm_device *lm_dev)
{
    struct device *dev = NULL;

    dev = dwc_get_hcd_device(lm_dev);
    if (!dev) {
        dev_err(&lm_dev->dev, "%s: get device fail.\n", __func__);
        return -1;
    }

    if (!pm_runtime_enabled(dev)) {
        pm_runtime_enable(dev);
    }
#ifdef CONFIG_PM_RUNTIME
    dev_info(&lm_dev->dev, "%s: enable runtime suspend %d\n",
            __func__, dev->power.disable_depth);
#endif
    return 0;
}

/* Disable USB pm runtime suspend */
int dwc_pm_runtime_disable(struct lm_device *lm_dev)
{
    struct device *dev = NULL;

    dev = dwc_get_hcd_device(lm_dev);
    if (!dev) {
        dev_err(&lm_dev->dev, "%s: get device fail.\n", __func__);
        return -1;
    }

    if (pm_runtime_enabled(dev)) {
        pm_runtime_disable(dev);
    }
#ifdef CONFIG_PM_RUNTIME
    dev_info(&lm_dev->dev, "%s: disable runtime suspend %d\n",
            __func__, dev->power.disable_depth);
#endif
    return 0;
}

int dwc_dev_insert_init(struct lm_device *lm_dev)
{
    dwc_otg_device_t *dwc_otg_dev;
    dwc_otg_core_if_t *core_if;
    struct hiusb_info *hiusb_info;
    int count = 0;
    
    hiusb_info = lm_dev->hiusb_info;
    dwc_otg_dev = lm_get_drvdata(lm_dev);
    core_if = dwc_otg_dev->core_if;
    
    dwc_otg_disable_global_interrupts(core_if);
    core_if->op_state = B_PERIPHERAL;
    dwc_otg_core_init(core_if);

    while (!dwc_otg_is_device_mode(core_if)) {
        dev_info(&lm_dev->dev, "%s, wait for device mode +.\n", __func__);
        dwc_msleep(10);
        if (++count > 50)
            return -1;
    }

    core_if->op_state = B_PERIPHERAL;	
    cil_pcd_start(core_if);
    dwc_pm_runtime_disable(lm_dev);
    dwc_otg_enable_global_interrupts(core_if);  
    
    dwc_otg_pcd_disconnect_us(dwc_otg_dev->pcd, 50);
    if (hiusb_info->gadget_enable) {
        dwc_otg_pcd_connect_us(dwc_otg_dev->pcd, 50);
    }
    dev_info(&lm_dev->dev, "%s .\n", __func__);
    return 0;
}

int dwc_host_insert_init(struct lm_device *lm_dev)
{
    dwc_otg_device_t *dwc_otg_dev;
    dwc_otg_core_if_t *core_if;
    struct hiusb_info *hiusb_info;
    int count = 0;
    
    hiusb_info = lm_dev->hiusb_info;
    dwc_otg_dev = lm_get_drvdata(lm_dev);
    core_if = dwc_otg_dev->core_if;
    
    dwc_otg_disable_global_interrupts(core_if);
    core_if->op_state = A_HOST;
    if (dwc_otg_core_init(core_if)) {
        dev_info(&lm_dev->dev, "%s, dwc_otg_core_init error.\n", __func__);
        return -1;
    }

    while (!dwc_otg_is_host_mode(core_if)) {
        dev_info(&lm_dev->dev, "%s, wait for host mode +.\n", __func__);
        dwc_msleep(10);
        if (++count > 50)
            return -1;
    }
    core_if->op_state = A_HOST;
    cil_hcd_start(core_if);
    dwc_otg_enable_global_interrupts(core_if); 
    
    dev_info(&lm_dev->dev, "%s .\n", __func__);
    return 0;
}

void dwc_dev_draw_init(struct lm_device *lm_dev)
{
    dwc_otg_device_t *dwc_otg_dev;
    dwc_otg_core_if_t *core_if;

    dev_info(&lm_dev->dev, "%s +.\n", __func__);
    dwc_otg_dev = lm_get_drvdata(lm_dev);
    core_if = dwc_otg_dev->core_if;
    dwc_otg_disable_global_interrupts(core_if);

    dwc_otg_dev->pcd->fops->disconnect(dwc_otg_dev->pcd);

    dwc_pm_runtime_enable(lm_dev);
    core_if->hibernation_suspend = 0;
    dev_info(&lm_dev->dev, "%s -.\n", __func__);
    return;
}

void hisi_usb_soft_disconnect(void)
{
    dwc_otg_device_t *dwc_otg_dev;
    
    printk(KERN_INFO "%s:+\n",__func__);
    if(0 != vbus_status()){
        dwc_otg_dev = lm_get_drvdata(g_hiusb_info->lm_dev);
        mdelay(100);
        dwc_otg_pcd_disconnect_us(dwc_otg_dev->pcd,0);
        mdelay(200);
    }
    else{
       printk(KERN_INFO "%s:no vbus\n",__func__); 
    }
    printk(KERN_INFO "%s:-\n",__func__);
}

void dwc_host_draw_init(struct lm_device *lm_dev)
{
    dwc_otg_device_t *dwc_otg_dev;
    dwc_otg_core_if_t *core_if;

    dev_info(&lm_dev->dev, "%s +.\n", __func__);
    dwc_otg_dev = lm_get_drvdata(lm_dev);
    core_if = dwc_otg_dev->core_if;
    dwc_otg_disable_global_interrupts(core_if);

    core_if->hibernation_suspend = 0;
    dev_info(&lm_dev->dev, "%s -.\n", __func__);
    return;
}

void dwc_otg_set_hibernation(struct lm_device *lm_dev, int val)
{
    dwc_otg_device_t *dwc_otg_dev;
    dwc_otg_core_if_t *core_if;
    
    dev_info(&lm_dev->dev, "%s +.\n", __func__);
    dwc_otg_dev = lm_get_drvdata(lm_dev);
    core_if = dwc_otg_dev->core_if;
    
    core_if->power_down = val;    

    return;
}

int hiusb_is_host_mode(struct lm_device *lm_dev)
{
    dwc_otg_device_t *dwc_otg_dev;
    dwc_otg_core_if_t *core_if;
    
    dev_info(&lm_dev->dev, "%s +.\n", __func__);
    dwc_otg_dev = lm_get_drvdata(lm_dev);
    core_if = dwc_otg_dev->core_if;
    return dwc_otg_is_host_mode(core_if);
}

static int hisi_usb_vbus_probe(struct platform_device *pdev)
{
    int ret = 0;
    pr_info("[%s]+\n", __func__);

    g_hiusb_info->insert_irq = platform_get_irq_byname(pdev, "connect");
    if (g_hiusb_info->insert_irq < 0) {
        printk(KERN_ERR "failed to get connect irq\n");
        return -ENOENT;
    }
    g_hiusb_info->draw_irq = platform_get_irq_byname(pdev, "disconnect");
    if (g_hiusb_info->draw_irq < 0) {
        printk(KERN_ERR "failed to get disconnect irq\n");
        return -ENOENT;
    }

    pr_info("g_hiusb_info->insert_irq: %d, g_hiusb_info->draw_irq: %d\n",
            g_hiusb_info->insert_irq, g_hiusb_info->draw_irq);

    /* request insert irq */
    ret = request_irq(g_hiusb_info->insert_irq, hiusb_vbus_intr,
            IRQF_NO_SUSPEND, "hiusb_insert_intr", g_hiusb_info->lm_dev);
    if (ret) {
        printk(KERN_ERR "request insert irq failed.\n");
        return ret;
    }

    /* request draw irq */
    ret = request_irq(g_hiusb_info->draw_irq, hiusb_vbus_intr,
            IRQF_NO_SUSPEND, "hiusb_draw_intr", g_hiusb_info->lm_dev);
    if (ret) {
        printk(KERN_ERR "request draw irq failed.\n");
        return ret;
    }
    
    pr_info("[%s]-\n", __func__);

    return ret;
}

static int hisi_usb_vbus_remove(struct platform_device *pdev)
{
    free_irq(g_hiusb_info->insert_irq, pdev);
    free_irq(g_hiusb_info->draw_irq, pdev);
    return 0;
}

static struct of_device_id hisi_usb_vbus_of_match[] = {
    { .compatible = "hisilicon,hi6xxx-usbvbus", },
    { },
};

static struct platform_driver hisi_usb_vbus_driver = {
    .probe      = hisi_usb_vbus_probe,
    .remove     = hisi_usb_vbus_remove,
    .driver     = {
        .owner      = THIS_MODULE,
        .name       = "hi6xxx-usbvbus",
        .of_match_table = hisi_usb_vbus_of_match,
    },
};
module_platform_driver(hisi_usb_vbus_driver);

MODULE_AUTHOR("wangbinghui<wangbinghui@huawei.com>");
MODULE_DESCRIPTION("HI6xxx usb vbus driver");
MODULE_LICENSE("GPL V2");
