/*
 * Device driver for SCHARGER POWER DRIVER	 in HI6xxx SOC
 *
 * Copyright (c) 2011 Hisilicon Co. Ltd
 *
 * Yongbo Sun <sunyongbo@huawei.com> : 2014-3-25
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *
 */
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of_platform.h>
#include <linux/io.h>
#include <linux/of.h>
static struct of_device_id of_hi6xxx_scharger_power_child_match_tbl[] = {
	/* regulators */
	{
		.compatible = "hisilicon,hi6xxx-regulator-scharger",
	},
	{ /* end */ }
};

static struct of_device_id of_hi6xxx_scharger_power_match_tbl[] = {
	{
		.compatible = "hisilicon,hi6xxx-scharger-driver",
	},
	{ /* end */ }
};
static int hi6xxx_scharger_power_probe(struct platform_device *pdev)
{
 	struct device *dev = NULL;
	struct device_node *np = NULL;

    if(pdev == NULL){
		printk(KERN_ERR"[%s] Pdev is null!\n",__FUNCTION__);
		return -1;
    }
 	dev = &pdev->dev;

	if(dev == NULL){
		printk(KERN_ERR"[%s] dev is null!\n",__FUNCTION__);
		return -1;
    }
	np = dev->of_node;

	/* 
	 * populate sub nodes,system init only adapt father node.
	 */
	of_platform_populate(np, of_hi6xxx_scharger_power_child_match_tbl, NULL, dev);

	return 0;
}

static struct platform_driver scharger_power_driver = {
	.driver = {
		.name	= "hisi_hi6xxx_charger_power",
		.owner  = THIS_MODULE,
		.of_match_table = of_hi6xxx_scharger_power_match_tbl,
	},
	.probe	= hi6xxx_scharger_power_probe,
};

static int __init hi6xxx_charger_power_init(void)
{
    return platform_driver_register(&scharger_power_driver);
}

static void __exit hi6xxx_charger_power_exit(void)
{
	platform_driver_unregister(&scharger_power_driver);
}

fs_initcall(hi6xxx_charger_power_init);
module_exit(hi6xxx_charger_power_exit);

MODULE_AUTHOR("Yongbo Sun<sunyongbo@huawei.com>");
MODULE_DESCRIPTION("HI6251 scharger power interface driver");
MODULE_LICENSE("GPL V2");
