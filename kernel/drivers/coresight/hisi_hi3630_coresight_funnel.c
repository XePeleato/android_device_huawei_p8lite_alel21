/* Copyright (c) 2013, hisi semiconductor co,ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/hisi_hi3630_of_coresight.h>
#include <linux/hisi_hi3630_coresight.h>

#include "hisi_hi3630_coresight_priv.h"

#define funnel_writel(drvdata, val, off)	\
			__raw_writel((val), drvdata->base + off)
#define funnel_readl(drvdata, off)		\
			__raw_readl(drvdata->base + off)

#define FUNNEL_LOCK(drvdata)						\
do {									\
	mb();								\
	funnel_writel(drvdata, 0x0, CORESIGHT_LAR);			\
} while (0)
#define FUNNEL_UNLOCK(drvdata)						\
do {									\
	funnel_writel(drvdata, CORESIGHT_UNLOCK, CORESIGHT_LAR);	\
	mb();								\
} while (0)

#define FUNNEL_FUNCTL		(0x000)
#define FUNNEL_PRICTL		(0x004)
#define FUNNEL_ITATBDATA0	(0xEEC)
#define FUNNEL_ITATBCTR2	(0xEF0)
#define FUNNEL_ITATBCTR1	(0xEF4)
#define FUNNEL_ITATBCTR0	(0xEF8)

#define FUNNEL_HOLDTIME_MASK	(0xF00)
#define FUNNEL_HOLDTIME_SHFT	(0x8)
#define FUNNEL_HOLDTIME		(0x7 << FUNNEL_HOLDTIME_SHFT)

struct funnel_drvdata {
	void __iomem		*base;
	struct device		*dev;
	struct coresight_device	*csdev;
	struct clk		*clk_at;
	struct clk		*clk_dbg;
	uint32_t		priority;
	bool			status;
};

static int funnel_enable_clock(struct funnel_drvdata *drvdata)
{
	int ret = 0;

	ret = clk_prepare_enable(drvdata->clk_at);
	if (ret) {
		dev_err(drvdata->dev, "coresight prepare clock enable error!\n");
		return ret;
	}

	ret = clk_prepare_enable(drvdata->clk_dbg);
	if (ret) {
		clk_disable_unprepare(drvdata->clk_at);
		return ret;
	}

	return ret;
}

static void funnel_disable_clock(struct funnel_drvdata *drvdata)
{
	clk_disable_unprepare(drvdata->clk_at);
	clk_disable_unprepare(drvdata->clk_dbg);
}

static void __funnel_enable(struct funnel_drvdata *drvdata, int port)
{
	uint32_t functl;

	FUNNEL_UNLOCK(drvdata);

	functl = funnel_readl(drvdata, FUNNEL_FUNCTL);
	functl &= ~FUNNEL_HOLDTIME_MASK;
	functl |= FUNNEL_HOLDTIME;
	functl |= (1 << port);
	/*funnel_writel(drvdata, functl, FUNNEL_FUNCTL);*/
	funnel_writel(drvdata, 0x31f, FUNNEL_FUNCTL);

	funnel_writel(drvdata, drvdata->priority, FUNNEL_PRICTL);

	FUNNEL_LOCK(drvdata);

#ifdef	FUNNEL_DEBUG
	dev_dbg(drvdata->dev, "FUNNEL_FUNCTL = 0x%02x\n", funnel_readl(drvdata, FUNNEL_FUNCTL));
	dev_dbg(drvdata->dev, "FUNNEL_PRICTL = 0x%02x\n", funnel_readl(drvdata, FUNNEL_PRICTL));
	dev_dbg(drvdata->dev, "CORESIGHT_LAR = 0x%02x\n", funnel_readl(drvdata, CORESIGHT_LAR));
#endif
}

static int funnel_enable(struct coresight_device *csdev, int inport,
			 int outport)
{
	struct funnel_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	int ret;

	ret = funnel_enable_clock(drvdata);
	if (ret)
		return ret;

	__funnel_enable(drvdata, inport);

	dev_info(drvdata->dev, "FUNNEL inport %d enabled\n", inport);
	return 0;
}

static void __funnel_disable(struct funnel_drvdata *drvdata, int inport)
{
	uint32_t functl;

	FUNNEL_UNLOCK(drvdata);

	functl = funnel_readl(drvdata, FUNNEL_FUNCTL);
	functl &= ~(1 << inport);
	funnel_writel(drvdata, functl, FUNNEL_FUNCTL);

	FUNNEL_LOCK(drvdata);
}

static void funnel_disable(struct coresight_device *csdev, int inport,
			   int outport)
{
	struct funnel_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	__funnel_disable(drvdata, inport);

	funnel_disable_clock(drvdata);

	dev_info(drvdata->dev, "FUNNEL inport %d disabled\n", inport);
}

static const struct coresight_ops_link funnel_link_ops = {
	.enable		= funnel_enable,
	.disable	= funnel_disable,
};

static const struct coresight_ops funnel_cs_ops = {
	.link_ops	= &funnel_link_ops,
};

static ssize_t funnel_show_priority(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct funnel_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val = drvdata->priority;

	return scnprintf(buf, PAGE_SIZE, "%#lx\n", val);
}

static ssize_t funnel_store_priority(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t size)
{
	struct funnel_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val;

	if (sscanf(buf, "%lx", &val) != 1)
		return -EINVAL;

	drvdata->priority = val;
	return size;
}
static DEVICE_ATTR(priority, S_IRUGO | S_IWUSR, funnel_show_priority,
		   funnel_store_priority);

static struct attribute *funnel_attrs[] = {
	&dev_attr_priority.attr,
	NULL,
};

static struct attribute_group funnel_attr_grp = {
	.attrs = funnel_attrs,
};

static const struct attribute_group *funnel_attr_grps[] = {
	&funnel_attr_grp,
	NULL,
};

static int funnel_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct coresight_platform_data *pdata = NULL;
	struct funnel_drvdata *drvdata = NULL;
	struct resource *res = NULL;
	struct coresight_desc *desc = NULL;
	int ret = 0;

	if (pdev->dev.of_node) {
		pdata = of_get_coresight_platform_data(dev, pdev->dev.of_node);
		if (IS_ERR(pdata)) {
			dev_err(drvdata->dev, "of_get_coresight_platform_data error!\n");
			return PTR_ERR(pdata);
		}
		pdev->dev.platform_data = pdata;
	}

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata) {
		dev_err(drvdata->dev, "coresight kzalloc error!\n");
		return -ENOMEM;
	}
	drvdata->dev = &pdev->dev;
	platform_set_drvdata(pdev, drvdata);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(drvdata->dev, "coresight get resource error!\n");
		return -ENODEV;
	}

	drvdata->base = devm_ioremap(dev, res->start, resource_size(res));
	if (!drvdata->base) {
		dev_err(drvdata->dev, "coresight ioremap error!\n");
		return -ENOMEM;
	}

	drvdata->status = false;

	drvdata->clk_at= devm_clk_get(dev, pdata->clock_at);
	if (IS_ERR(drvdata->clk_at)) {
		dev_err(drvdata->dev, "coresight get clock error!\n");
		ret = PTR_ERR(drvdata->clk_at);
		return -ENODEV;
	}

	ret = clk_set_rate(drvdata->clk_at, 240000000);
	if (ret) {
		dev_err(drvdata->dev, "coresight set clock rate error!\n");
		return -ENODEV;
	}

	drvdata->clk_dbg= devm_clk_get(dev, pdata->clock_dbg);
	if (IS_ERR(drvdata->clk_dbg)) {
		dev_err(drvdata->dev, "coresight get clock error!\n");
		ret = PTR_ERR(drvdata->clk_dbg);
		return -ENODEV;
	}

	ret = clk_set_rate(drvdata->clk_dbg, 120000000);
	if (ret) {
		dev_err(drvdata->dev, "coresight set clock rate error!\n");
		return -ENODEV;
	}

	desc = devm_kzalloc(dev, sizeof(*desc), GFP_KERNEL);
	if (!desc) {
		dev_err(drvdata->dev, "coresight desc kzalloc error!\n");
		return -ENOMEM;
	}
	desc->type = CORESIGHT_DEV_TYPE_LINK;
	desc->subtype.link_subtype = CORESIGHT_DEV_SUBTYPE_LINK_MERG;
	desc->ops = &funnel_cs_ops;
	desc->pdata = pdev->dev.platform_data;
	desc->dev = &pdev->dev;
	desc->groups = funnel_attr_grps;
	desc->owner = THIS_MODULE;
	drvdata->csdev = coresight_register(desc);
	if (IS_ERR(drvdata->csdev)) {
		dev_err(drvdata->dev, "coresight_register error!\n");
		return PTR_ERR(drvdata->csdev);
	}

	dev_info(dev, "FUNNEL initialized\n");
	return 0;
}

static int  funnel_suspend(struct platform_device *pdev, pm_message_t message)
{

	struct funnel_drvdata *drvdata = platform_get_drvdata(pdev);

	dev_info(drvdata->dev, "funnel_suspend+++\n");

	dev_info(drvdata->dev, "funnel_suspend---\n");
	return 0;
}

static int  funnel_resume(struct platform_device *pdev)
{
	struct funnel_drvdata *drvdata = platform_get_drvdata(pdev);

	dev_info(drvdata->dev, "funnel_resume+++\n");

	dev_info(drvdata->dev, "funnel_resume---\n");
	return 0;
}

static int funnel_remove(struct platform_device *pdev)
{
	struct funnel_drvdata *drvdata = platform_get_drvdata(pdev);

	coresight_unregister(drvdata->csdev);
	return 0;
}

static struct of_device_id funnel_match[] = {
	{.compatible = "huawei,coresight-funnel"},
	{}
};

static struct platform_driver funnel_driver = {
	.probe          = funnel_probe,
	.remove         = funnel_remove,
	.suspend	= funnel_suspend,
	.resume		= funnel_resume,
	.driver         = {
		.name   = "coresight-funnel",
		.owner	= THIS_MODULE,
		.of_match_table = funnel_match,
	},
};

static int __init funnel_init(void)
{
	return platform_driver_register(&funnel_driver);
}
module_init(funnel_init);

static void __exit funnel_exit(void)
{
	platform_driver_unregister(&funnel_driver);
}
module_exit(funnel_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("CoreSight Funnel driver");
