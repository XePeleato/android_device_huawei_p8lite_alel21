/*
 * linux/driver/usb/host/ehci-hisik3.c
 *
 * Copyright (c) 2011 Hisi technology corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation;version 2 of the License.
 *
 */
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/usb_phy.h>
#include <linux/regulator/consumer.h>

#define HISIK3_USB_DMA_ALIGN 32

struct hisik3_ehci_hcd *hiusb_ehci_private;
#define CONFIG_USB_EHCI_ONOFF_FEATURE

static void hisik3_ehci_power_up(struct usb_hcd *hcd);
static void hisik3_ehci_power_down(struct usb_hcd *hcd);

#ifdef CONFIG_PM
static int hisik3_ehci_bus_suspend(struct usb_hcd *hcd);
static int hisik3_ehci_bus_resume(struct usb_hcd *hcd);
static int hisik3_usb_suspend(struct usb_hcd *hcd);
static int hisik3_usb_resume(struct usb_hcd *hcd);
#endif

#ifdef CONFIG_USB_EHCI_ONOFF_FEATURE
/* Stored ehci handle for hsic insatnce */
struct usb_hcd *ehci_handle;
/*
static struct hc_driver __read_mostly hisi_hc_driver;
*/
int connect_status = 1;

static DEFINE_MUTEX(ehci_power_lock);

#define PERI_CRG_BASE	0xFFF35000
#define REG_PEREN4	0x40
#define REG_PERDIS4	0x44

void __iomem		*base;

struct hisik3_ehci_platform_data hisik3_usbh_data = {
	.operating_mode = HISIK3_USB_HOST,
	.phy_config = NULL,
	.power_down_on_bus_suspend = 0,
	.post_suspend = NULL,
	.pre_resume = NULL,
} ;

static u64 hisik3_usb_ehci_dmamask = (0xFFFFFFFFUL);

static int hisi_ehci_clk_disable(struct hisik3_ehci_hcd *hiusb_ehci)
{
	/*writel(0x1c ,base + REG_PERDIS4);*/

	clk_disable_unprepare(hiusb_ehci->hclk_usb2host);
	clk_disable_unprepare(hiusb_ehci->clk_usbhost48);
	clk_disable_unprepare(hiusb_ehci->clk_usbhost12);

	return 0;
}

static int hisi_ehci_clk_enable(struct hisik3_ehci_hcd *hiusb_ehci)
{
	int ret = 0;

	ret = clk_prepare_enable(hiusb_ehci->hclk_usb2host);
	if (ret) {
		printk("hiusb_ehci->hclk_usb2host prepare clock enable error!\n");
		return ret;
	}

	ret = clk_prepare_enable(hiusb_ehci->clk_usbhost48);
	if (ret) {
		printk("hiusb_ehci->clk_usbhost48 prepare clock enable error!\n");
		return ret;
	}

	ret = clk_prepare_enable(hiusb_ehci->clk_usbhost12);
	if (ret) {
		printk("hiusb_ehci->clk_usbhost12 prepare clock enable error!\n");
		return ret;
	}

	/*writel(0x1c ,base + REG_PEREN4);*/

	return 0;
}

static int hisik3_enable_usb_regulator(struct hisik3_ehci_hcd *hiusb_ehci)
{
	int ret = 0;
#if 1
	ret = regulator_bulk_enable(1, &(hiusb_ehci->pdata->regu_ldo8));
	if (0 != ret) {
		pr_err("%s : couldn't enable regulators(ldo8) %d\n", __FUNCTION__, ret);
		return ret;
	}
#endif
	return ret;

}


static void hisik3_disable_usb_regulator(struct hisik3_ehci_hcd *hiusb_ehci)
{
#if 1
	regulator_bulk_disable(1, &(hiusb_ehci->pdata->regu_ldo8));
#endif
}

static ssize_t show_ehci_status(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", connect_status);
}

static ssize_t store_ehci_power(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int power_on;
	int ret;
	struct hisik3_ehci_hcd *hiusb_ehci = dev_get_drvdata(dev);
	struct usb_hcd *hcd = ehci_to_hcd(hiusb_ehci->ehci);

	if (sscanf(buf, "%d", &power_on) != 1) {
		printk("%s::return EINVAL.\n", __func__);
		return -EINVAL;
	}

	mutex_lock(&ehci_power_lock);

	if (power_on == 0 && ehci_handle != NULL) {
		/*enable hisc phy cnf clk,12MHz and 480MHz.*/
		hisik3_usb_phy_clk_enable(hiusb_ehci->phy);
		ret = hisi_ehci_clk_enable(hiusb_ehci);
		hiusb_ehci->clock_state = 1;
		if (ret) {
			printk(KERN_ERR "%s.clk_enable core_clk failed\n", __func__);
		}
#if 0
		connect_status = 0;
#endif
		clear_bit(HCD_FLAG_DEAD, &hcd->flags);
		usb_remove_hcd(hcd);
		hisik3_ehci_power_down(hcd);
		ehci_handle = NULL;
	} else if (power_on == 1) {
		if (ehci_handle) {
			clear_bit(HCD_FLAG_DEAD, &hcd->flags);
			usb_remove_hcd(hcd);
			hisik3_ehci_power_down(hcd);
		}
		hisik3_ehci_power_up(hcd);

		hiusb_ehci->ehci->caps = hcd->regs;

		hiusb_ehci->ehci->regs = hcd->regs +
			 HC_LENGTH(hiusb_ehci->ehci, ehci_readl(hiusb_ehci->ehci, &hiusb_ehci->ehci->caps->hc_capbase));
		hiusb_ehci->ehci->hcs_params = ehci_readl(hiusb_ehci->ehci, &hiusb_ehci->ehci->caps->hcs_params);
		hiusb_ehci->ehci->sbrn = 0x20;

		if (usb_add_hcd(hiusb_ehci->hcd, hiusb_ehci->irq, IRQF_SHARED)) {
			printk("Failed to add USB HCD\n");
			mutex_unlock(&ehci_power_lock);
			return -1;
		}

		ehci_writel(hiusb_ehci->ehci, 1, &hiusb_ehci->ehci->regs->configured_flag);

		ehci_handle = hcd;

		hiusb_ehci->ehci_init = 1;
	/* For debug use */
	} else if (power_on == 2) {
		hisik3_ehci_bus_suspend(hcd);
	} else if (power_on == 3) {
		hisik3_ehci_bus_resume(hcd);
	} else if (power_on == 4) {
		hisik3_usb_suspend(hcd);
	} else if (power_on == 5) {
		hisik3_usb_resume(hcd);
	}

	mutex_unlock(&ehci_power_lock);
	return count;
}

static DEVICE_ATTR(ehci_power, S_IRUGO | S_IWUSR | S_IWGRP,
		show_ehci_status, store_ehci_power);

#endif

static void hisik3_ehci_power_up(struct usb_hcd *hcd)
{
	int ret = 0;
	struct hisik3_ehci_hcd *hiusb_ehci = dev_get_drvdata(hcd->self.controller);

	if (!hiusb_ehci->clock_state) {
		/*enable hisc phy cnf clk,12MHz and 480MHz.*/
		hisik3_usb_phy_clk_enable(hiusb_ehci->phy);
		ret = hisi_ehci_clk_enable(hiusb_ehci);
		if (ret) {
			printk(KERN_ERR "%s.clk_enable core_clk failed\n", __func__);
		}

		hiusb_ehci->clock_state = 1;
	}

	hisik3_enable_usb_regulator(hiusb_ehci);
	hisik3_usb_phy_power_on(hiusb_ehci, hiusb_ehci->hcd->regs);
	hiusb_ehci->host_resumed = 1;
	return;
}

static void hisik3_ehci_power_down(struct usb_hcd *hcd)
{
	struct hisik3_ehci_hcd *hiusb_ehci = dev_get_drvdata(hcd->self.controller);
	hiusb_ehci->host_resumed = 0;
	hisik3_usb_phy_power_off(hiusb_ehci);

	hisik3_disable_usb_regulator(hiusb_ehci);

	if (hiusb_ehci->clock_state) {
		/*disable hisc phy cnf clk,12MHz and 480MHz.*/
		hisi_ehci_clk_disable(hiusb_ehci);
		hisik3_usb_phy_clk_disable(hiusb_ehci->phy);
		hiusb_ehci->clock_state = 0;
	}

	return;
}

static void hisik3_ehci_restart(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);

	ehci_reset(ehci);

	/* setup the frame list and Async q heads */
	ehci_writel(ehci, ehci->periodic_dma, &ehci->regs->frame_list);
	ehci_writel(ehci, (u32)ehci->async->qh_dma, &ehci->regs->async_next);
	/* setup the command register and set the controller in RUN mode */
	ehci->command &= ~(CMD_LRESET|CMD_IAAD|CMD_PSE|CMD_ASE|CMD_RESET);
	ehci->command |= CMD_RUN;
	ehci_writel(ehci, ehci->command, &ehci->regs->command);

	down_write(&ehci_cf_port_reset_rwsem);
	ehci_writel(ehci, FLAG_CF, &ehci->regs->configured_flag);
	/* flush posted writes */
	ehci_readl(ehci, &ehci->regs->command);
	up_write(&ehci_cf_port_reset_rwsem);
}

static int hisik3_usb_suspend(struct usb_hcd *hcd)
{
	struct hisik3_ehci_hcd *hiusb_ehci = dev_get_drvdata(hcd->self.controller);

	struct ehci_regs __iomem *hw = hiusb_ehci->ehci->regs;
	unsigned long flags;
	printk("hisik3_usb_suspend.\n");

	spin_lock_irqsave(&hiusb_ehci->ehci->lock, flags);

	hiusb_ehci->port_speed = (readl(&hw->port_status[0]) >> 26) & 0x3;
	ehci_halt(hiusb_ehci->ehci);
	clear_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);

	spin_unlock_irqrestore(&hiusb_ehci->ehci->lock, flags);

	hisik3_ehci_power_down(hcd);
	return 0;
}

static int hisik3_usb_resume(struct usb_hcd *hcd)
{
	struct hisik3_ehci_hcd *hiusb_ehci = dev_get_drvdata(hcd->self.controller);
	struct ehci_hcd	*ehci = hcd_to_ehci(hcd);
	struct ehci_regs __iomem *hw = ehci->regs;
	unsigned long val;
	printk("hisik3_usb_resume.\n");

	set_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);
	hisik3_ehci_power_up(hcd);

	if (hiusb_ehci->port_speed > HISIK3_USB_PHY_PORT_SPEED_HIGH) {
		/* Wait for the phy to detect new devices
		 * before we restart the controller */
		msleep(10);
		goto restart;
	}

	/* Force the phy to keep data lines in suspend state */
	hisik3_ehci_phy_restore_start(hiusb_ehci->phy, hiusb_ehci->port_speed);

	/* Enable host mode */
	tdi_reset(ehci);

	/* Enable Port Power */
	val = readl(&hw->port_status[0]);
	val |= PORT_POWER;
	writel(val, &hw->port_status[0]);
	udelay(10);

	/* Check if the phy resume from LP0. When the phy resume from LP0
	 * USB register will be reset. */
	if (!readl(&hw->async_next)) {
		/* Program the field PTC based on the saved speed mode */
		val = readl(&hw->port_status[0]);
		val &= ~PORT_TEST(~0);
		if (hiusb_ehci->port_speed == HISIK3_USB_PHY_PORT_SPEED_HIGH)
			val |= PORT_TEST_FORCE;
		else if (hiusb_ehci->port_speed == HISIK3_USB_PHY_PORT_SPEED_FULL)
			val |= PORT_TEST(6);
		else if (hiusb_ehci->port_speed == HISIK3_USB_PHY_PORT_SPEED_LOW)
			val |= PORT_TEST(7);
		writel(val, &hw->port_status[0]);
		udelay(10);

		/* Disable test mode by setting PTC field to NORMAL_OP */
		val = readl(&hw->port_status[0]);
		val &= ~PORT_TEST(~0);
		writel(val, &hw->port_status[0]);
		udelay(10);
	}

	/* Poll until CCS is enabled */
	if (handshake(ehci, &hw->port_status[0], PORT_CONNECT,
						 PORT_CONNECT, 2000)) {
		pr_err("%s: timeout waiting for PORT_CONNECT\n", __func__);
		goto restart;
	}

	/* Poll until PE is enabled */
	if (handshake(ehci, &hw->port_status[0], PORT_PE,
						 PORT_PE, 2000)) {
		pr_err("%s: timeout waiting for USB_PORTSC1_PE\n", __func__);
		goto restart;
	}

	/* Clear the PCI status, to avoid an interrupt taken upon resume */
	val = readl(&hw->status);
	val |= STS_PCD;
	writel(val, &hw->status);

	/* Put controller in suspend mode by writing 1 to SUSP bit of PORTSC */
	val = readl(&hw->port_status[0]);
	if ((val & PORT_POWER) && (val & PORT_PE)) {
		val |= PORT_SUSPEND;
		writel(val, &hw->port_status[0]);

		/* Wait until port suspend completes */
		if (handshake(ehci, &hw->port_status[0], PORT_SUSPEND,
							 PORT_SUSPEND, 1000)) {
			pr_err("%s: timeout waiting for PORT_SUSPEND\n",
								__func__);
			goto restart;
		}
	}

	hisik3_ehci_phy_restore_end(hiusb_ehci->phy);
	return 0;

restart:
	if (hiusb_ehci->port_speed <= HISIK3_USB_PHY_PORT_SPEED_HIGH)
		hisik3_ehci_phy_restore_end(hiusb_ehci->phy);

	hisik3_ehci_restart(hcd);
	return 0;
}

static void hisik3_ehci_shutdown(struct usb_hcd *hcd)
{
	struct hisik3_ehci_hcd *hiusb_ehci = dev_get_drvdata(hcd->self.controller);

	ehci_dbg(hiusb_ehci->ehci, "hisik3_ehci_shutdown.\n");

	/* ehci_shutdown touches the USB controller registers, make sure
	 * controller has clocks to it */
	if (!hiusb_ehci->host_resumed)
		hisik3_ehci_power_up(hcd);

	if (hiusb_ehci->ehci_init)
		ehci_shutdown(hcd);
}

#ifdef CONFIG_PM
static int hisik3_ehci_bus_suspend(struct usb_hcd *hcd)
{
	struct hisik3_ehci_hcd *hiusb_ehci = dev_get_drvdata(hcd->self.controller);
	int error_status = 0;

	printk("hisik3_ehci_bus_suspend.\n");

	error_status = ehci_bus_suspend(hcd);
	if (!error_status && hiusb_ehci->power_down_on_bus_suspend) {
		hisik3_usb_suspend(hcd);
		hiusb_ehci->bus_suspended = 1;
	}

	if (hiusb_ehci->pdata && hiusb_ehci->pdata->post_suspend)
		hiusb_ehci->pdata->post_suspend();

	if (hiusb_ehci->clock_state) {
		/*disable hisc phy cnf clk,12MHz and 480MHz.*/
		//hisi_ehci_clk_disable(hiusb_ehci);
		//hisik3_usb_phy_clk_disable(hiusb_ehci->phy);
		//hiusb_ehci->clock_state = 0;
	}

	return error_status;
}

static int hisik3_ehci_bus_resume(struct usb_hcd *hcd)
{
	//int ret = 0;
	struct hisik3_ehci_hcd *hiusb_ehci = dev_get_drvdata(hcd->self.controller);
	printk("hisik3_ehci_bus_resume.\n");

	if (hiusb_ehci->pdata && hiusb_ehci->pdata->pre_resume)
		hiusb_ehci->pdata->pre_resume();

	if (!hiusb_ehci->clock_state) {
		/*enable hisc phy cnf clk,12MHz and 480MHz.*/
		//hisik3_usb_phy_clk_enable(hiusb_ehci->phy);
		//ret = hisi_ehci_clk_enable(hiusb_ehci);
		//if (ret) {
		//	printk(KERN_ERR "%s.clk_enable core_clk failed\n", __func__);
		//}

		//hiusb_ehci->clock_state = 1;
	}

	if (hiusb_ehci->bus_suspended && hiusb_ehci->power_down_on_bus_suspend) {
		hisik3_usb_resume(hcd);
		hiusb_ehci->bus_suspended = 0;
	}

	hisik3_usb_phy_preresume(hiusb_ehci->phy);
	hiusb_ehci->port_resuming = 1;
	return ehci_bus_resume(hcd);
}
#endif

static struct hc_driver hisik3_ehci_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "Hisilicon K3 EHCI Host Controller",
	.hcd_priv_size		= sizeof(struct ehci_hcd),

	.irq			= ehci_irq,
	.flags			= HCD_USB2 | HCD_MEMORY,
	.reset                  = ehci_setup,
	.start			= ehci_run,

	.stop			= ehci_stop,
	.shutdown		= hisik3_ehci_shutdown,

	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,

	.get_frame_number	= ehci_get_frame,

	.endpoint_reset		= ehci_endpoint_reset,
	.hub_status_data	= ehci_hub_status_data,

	.hub_control		= ehci_hub_control,
#ifdef CONFIG_PM
	.bus_suspend		= hisik3_ehci_bus_suspend,
	.bus_resume		= hisik3_ehci_bus_resume,
#endif
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,
};

static int hisik3_ehci_of_get_paltform_data(struct device *dev, struct device_node *node, struct hisik3_ehci_platform_data *pdata)
{
	int ret = 0;

	ret = of_property_read_string_index(node, "clock-names", 0, &pdata->hclk_usb2host);
	if (ret)
		return ret;

	ret = of_property_read_string_index(node, "clock-names", 1, &pdata->clk_usbhost48);
	if (ret)
		return ret;

	ret = of_property_read_string_index(node, "clock-names", 2, &pdata->clk_usbhost12);
	if (ret)
		return ret;

	ret = of_property_read_string_index(node, "clock-names", 3, &pdata->clk_usb2host_ref);
	if (ret)
		return ret;

	ret = of_property_read_string_index(node, "clock-names", 4, &pdata->clk_hsic);
	if (ret)
		return ret;

	return ret;
}

static int hisik3_ehci_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct usb_hcd *hcd;
	struct hisik3_ehci_hcd *hiusb_ehci;
	struct hisik3_ehci_platform_data *pdata;
	int err = 0;
	int irq;

	if (usb_disabled())
		return -ENODEV;

	pdata = &hisik3_usbh_data;

	pdev->dev.dma_mask = &hisik3_usb_ehci_dmamask;
	pdev->dev.coherent_dma_mask = (0xFFFFFFFFUL);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (unlikely(!res)) {
		return -ENXIO;
	}

	irq = platform_get_irq(pdev, 0);
	if (unlikely(irq < 0)) {
		return -EINTR;
	}

	hcd = usb_create_hcd(&hisik3_ehci_hc_driver, &pdev->dev, hcd_name);
	if (!hcd) {
		pr_err( "Unable to create HCD\n");
		return -ENODEV;
	}

	hiusb_ehci = kzalloc(sizeof(struct hisik3_ehci_hcd), GFP_KERNEL);
	if (!hiusb_ehci) {
		goto fail_alloc;
	}

	platform_set_drvdata(pdev, hiusb_ehci);

	hcd->regs = (void __iomem *)res->start;
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		err = -EBUSY;
		goto fail_mregion;
	}

	hcd->regs = devm_ioremap(&pdev->dev, hcd->rsrc_start, hcd->rsrc_len);
	if (unlikely(hcd->regs == NULL)) {
		err = -EFAULT;
		goto fail_io;
	}

	hiusb_ehci->host_resumed = 1;
	hiusb_ehci->power_down_on_bus_suspend = pdata->power_down_on_bus_suspend;
	printk("%s:: hiusb_ehci->power_down_on_bus_suspend = %d.\n", __func__, hiusb_ehci->power_down_on_bus_suspend);
	hiusb_ehci->ehci = hcd_to_ehci(hcd);

	/*save private infos*/
	hiusb_ehci->hcd = hcd;
	hiusb_ehci->irq = irq;
	hiusb_ehci->instance = pdev->id;
	hiusb_ehci->pdata = pdata;
	hiusb_ehci->ehci_init = 0;
	hiusb_ehci->clock_state = 1;
	hiusb_ehci_private = hiusb_ehci;

	if (pdev->dev.of_node) {
		err = hisik3_ehci_of_get_paltform_data(&pdev->dev, pdev->dev.of_node, pdata);
		if (err) {
			dev_err(&pdev->dev, "hisik3_ehci_of_get_paltform_data error!\n");
			goto fail_io;
		}
		pdev->dev.platform_data = pdata;
	}

	hiusb_ehci->hclk_usb2host = devm_clk_get(&pdev->dev, pdata->hclk_usb2host);
	if (IS_ERR(hiusb_ehci->hclk_usb2host)) {
		dev_err(&pdev->dev, "hsic get clock error!\n");
		err = PTR_ERR(hiusb_ehci->hclk_usb2host);
		goto fail_io;
	}

	hiusb_ehci->clk_usbhost48 = devm_clk_get(&pdev->dev, pdata->clk_usbhost48);
	if (IS_ERR(hiusb_ehci->clk_usbhost48)) {
		dev_err(&pdev->dev, "hsic get clock error!\n");
		err = PTR_ERR(hiusb_ehci->clk_usbhost48);
		goto fail_io;
	}

	hiusb_ehci->clk_usbhost12 = devm_clk_get(&pdev->dev, pdata->clk_usbhost12);
	if (IS_ERR(hiusb_ehci->clk_usbhost12)) {
		dev_err(&pdev->dev, "hsic get clock error!\n");
		err = PTR_ERR(hiusb_ehci->clk_usbhost12);
		goto fail_io;
	}

	hiusb_ehci->clk_usb2host_ref = devm_clk_get(&pdev->dev, pdata->clk_usb2host_ref);
	if (IS_ERR(hiusb_ehci->clk_usb2host_ref)) {
		dev_err(&pdev->dev, "hsic get clock error!\n");
		err = PTR_ERR(hiusb_ehci->clk_usb2host_ref);
		goto fail_io;
	}

	hiusb_ehci->clk_hsic = devm_clk_get(&pdev->dev, pdata->clk_hsic);
	if (IS_ERR(hiusb_ehci->clk_hsic)) {
		dev_err(&pdev->dev, "hsic get clock error!\n");
		err = PTR_ERR(hiusb_ehci->clk_hsic);
		goto fail_io;
	}

#if 1
	pdata->regu_ldo8.supply = "hsic";
	err = devm_regulator_bulk_get(&pdev->dev, 1, &(pdata->regu_ldo8));
	if (0 != err) {
		dev_err(&pdev->dev, "couldn't get regulators(ldo8) %d\n", err);
		goto fail_io;
	}
#endif

	err = hisi_ehci_clk_enable(hiusb_ehci);
	if (err) {
		dev_err(&pdev->dev, "hisi_ehci_clk_enable error!\n");
		goto fail_io;
	}

	hiusb_ehci->phy = hisik3_usb_phy_open(0, hiusb_ehci->hcd->regs, hiusb_ehci->pdata->phy_config,
						HISIK3_USB_PHY_MODE_HOST, hiusb_ehci);
	if (IS_ERR(hiusb_ehci->phy)) {
		printk("Failed to open USB phy\n");
		err =  -ENXIO;
		goto fail_clk;
	}

	hisik3_ehci_power_up(hcd);

	err = device_create_file(&(pdev->dev), &dev_attr_ehci_power);
	if (err) {
		dev_err(&pdev->dev, "Failed to create sysfs entry\n");
		goto fail_attr;
	}

	hisik3_ehci_power_down(hcd);

	return err;

	hisik3_ehci_power_down(hcd);
	usb_remove_hcd(hcd);
fail_attr:
	hisik3_usb_phy_close(hiusb_ehci->phy);
fail_clk:
	hisi_ehci_clk_disable(hiusb_ehci);
fail_io:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
fail_mregion:
	kfree(hiusb_ehci);
fail_alloc:
	usb_put_hcd(hcd);
	return err;
}

static int hisik3_ehci_remove(struct platform_device *pdev)
{
	struct hisik3_ehci_hcd *hiusb_ehci = platform_get_drvdata(pdev);
	struct usb_hcd *hcd = NULL;

	if (!hiusb_ehci)
		return -EINVAL;

	hcd = ehci_to_hcd(hiusb_ehci->ehci);

	if (!hcd)
		return -EINVAL;

	usb_remove_hcd(hcd);
	usb_put_hcd(hcd);

	hisik3_ehci_power_down(hcd);
	hisik3_usb_phy_close(hiusb_ehci->phy);

	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);

	kfree(hiusb_ehci);

	return 0;
}

#if 0
static void hisik3_ehci_hcd_shutdown(struct platform_device *pdev)
{
	struct hisik3_ehci_hcd *hiusb_ehci = platform_get_drvdata(pdev);
	struct usb_hcd *hcd = NULL;

	if (!hiusb_ehci) {
		return;
	}

	hcd = ehci_to_hcd(hiusb_ehci->ehci);

	if (!hcd) {
		return;
	}

	if (hcd->driver->shutdown)
		hcd->driver->shutdown(hcd);
}
#endif

static const struct ehci_driver_overrides hisi_overrides __initdata = {
	.reset = ehci_setup,
};

#ifdef CONFIG_OF
static const struct of_device_id hs_ehci_of_match[] = {
	{ .compatible = "hisilicon,hi3630-hsic", },
	{},
};
MODULE_DEVICE_TABLE(of, hs_ehci_of_match);
#endif

static struct platform_driver ehci_k3v3_driver = {
	.probe		= hisik3_ehci_probe,
	.remove		= hisik3_ehci_remove,
	.driver		= {
		.name	= "hi3630-hsic",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(hs_ehci_of_match),
	},
};
