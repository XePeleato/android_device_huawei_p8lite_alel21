/*
 * axierr irq support.
 *
 * Copyright (c) 2013- Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/cpu.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/platform_device.h>

#define A53_cluster0_cpu0		(0)
#define A53_cluster1_cpu4		(4)

#define AXI_ERROR_CLUSTER0_EN_MASK        (1 << 14)
#define AXI_ERROR_CLUSTER1_EN_MASK        (1 << 15)

static void __iomem *acpu_sctrl_base_virt;
static void *axi_err_irq_en_addr;

static irqreturn_t irq_handler_axierr_A53(int irq, void *dev_id)
{
	pr_err("AXI or CHI asynchronous error has occurred!!\n");
#ifdef CONFIG_ARM64
	/**
	 *write register L2ECTLR_EL1,clear irq
	 */
	asm volatile(
		"	mrs x0, S3_1_C11_C0_3	\n"
		"	mov x1, #(0x2 << 28)	\n"
		"	bic x0, x0, x1		\n"
		"	msr S3_1_C11_C0_3, x0	\n"
	);

	pr_err("64AXI or CHI asynchronous error end!\n");
#else
	/**
	 *write register L2ECTLR_EL1,clear irq
	 */
	asm volatile(
		"	mov r0, #0x0	\n"
		"	mcr p15, 1, r0, c9, c0, 3	\n"
	);
	pr_err("32AXI or CHI asynchronous error end!\n");
#endif
	return IRQ_HANDLED;
}

static int hisi_axi_suspend(struct platform_device *pdev, pm_message_t state)
{
	pr_info("hisi axi suspend!\n");
	return 0;
}

static int hisi_axi_resume(struct platform_device *pdev)
{
	int ret = 0;
	u32 axi_enable_offset = 0x0;
	u32 axi_cluster0_irq_num;
	u32 axi_cluster1_irq_num;
	u32 axi_err_irq_en    = 0x0;
	struct device_node *node = pdev->dev.of_node;

	pr_info("hisi axi resume start!\n");

	/**
	 * affinity cluster0's axi err interrupt
	 */
	axi_cluster0_irq_num = irq_of_parse_and_map(node, 0);
	ret = irq_set_affinity(axi_cluster0_irq_num, cpumask_of(A53_cluster0_cpu0));
	if (ret < 0) {
		pr_err("axi_cluster0_irq_num %d: A53 irqaffinity register failed\n", axi_cluster0_irq_num);
		return ret;
	}

	/**
	 * affinity cluster1's axi err interrupt
	 */
	axi_cluster1_irq_num = irq_of_parse_and_map(node, 1);
	ret = irq_set_affinity(axi_cluster1_irq_num, cpumask_of(A53_cluster1_cpu4));
	if (ret < 0) {
		pr_err("axi_cluster1_irq_num %d: A53 irqaffinity register failed\n", axi_cluster1_irq_num);
		return ret;
	}

	/**
	 * hi6xxx need enable axi error irq, hi3xxx no need
	 */
	if (of_property_read_bool(node, "need-enable-axi-irq")){
		if (acpu_sctrl_base_virt) {
			ret = of_property_read_u32_index(node, "offset", 0, &axi_enable_offset);
			if (ret) {
				pr_err("read axi_enable_offset is failed!\n");
				return ret;
			}

			axi_err_irq_en_addr = (char *)acpu_sctrl_base_virt + axi_enable_offset;
			axi_err_irq_en = readl_relaxed(axi_err_irq_en_addr);
			axi_err_irq_en |= AXI_ERROR_CLUSTER0_EN_MASK + AXI_ERROR_CLUSTER1_EN_MASK;
			writel_relaxed(axi_err_irq_en, axi_err_irq_en_addr);
		}
	}

	pr_info("hisi axi resume finished.\n");
	return ret;
}

static int hisi_axi_probe(struct platform_device *pdev)
{
	int ret = 0;
	u32 axi_cluster0_irq_num;
	u32 axi_cluster1_irq_num;
	u32 axi_enable_offset = 0x0;
	u32 axi_err_irq_en    = 0x0;
	struct device_node *node = pdev->dev.of_node;

	pr_info("axi init begin!\n");

	/**
	 * affinity cluster0's axi err interrupt
	 */
	axi_cluster0_irq_num = irq_of_parse_and_map(node, 0);
	ret = request_irq(axi_cluster0_irq_num, irq_handler_axierr_A53,
			IRQF_TRIGGER_RISING, "axierr_A53_cluster0", NULL);
	if (ret) {
		pr_err("%s: cannot register IRQ %d for A53. Line: %d\n",
				__func__, axi_cluster0_irq_num, __LINE__);
		goto err;
	}
	ret = irq_set_affinity(axi_cluster0_irq_num, cpumask_of(A53_cluster0_cpu0));
	if (ret < 0) {
		pr_err("hisi axi init failed: A53 irqaffinity register failed\n");
		goto err_a53_cluster0_irqaffinity_register;
	}

	/**
	 * affinity cluster1's axi err interrupt
	 */
	axi_cluster1_irq_num = irq_of_parse_and_map(node, 1);
	ret = request_irq(axi_cluster1_irq_num, irq_handler_axierr_A53,
			IRQF_TRIGGER_RISING, "axierr_A53_cluster1", NULL);
	if (ret) {
		pr_err("%s: cannot register IRQ %d for A53. Line: %d\n",
				__func__, axi_cluster1_irq_num, __LINE__);
		goto err_a53_cluster0_irqaffinity_register;
	}
	ret = irq_set_affinity(axi_cluster1_irq_num, cpumask_of(A53_cluster1_cpu4));
	if (ret < 0) {
		pr_err("hisi axi init failed: 53 irqaffinity register failed\n");
		goto err_a53_cluster1_irqaffinity_register;
	}

	/**
	 * hi6xxx need enable axi error irq, hi3xxx no need
	 */
	if (of_property_read_bool(node, "need-enable-axi-irq")){
		acpu_sctrl_base_virt = of_iomap(node, 0);
		if (acpu_sctrl_base_virt) {
			ret = of_property_read_u32_index(node, "offset", 0, &axi_enable_offset);
			if (ret) {
				pr_err("read axi_enable_offset is failed!\n");
				goto err_a53_cluster1_irqaffinity_register;
			}

			axi_err_irq_en_addr = (char *)acpu_sctrl_base_virt + axi_enable_offset;
			axi_err_irq_en = readl_relaxed(axi_err_irq_en_addr);
			axi_err_irq_en |= AXI_ERROR_CLUSTER0_EN_MASK + AXI_ERROR_CLUSTER1_EN_MASK;
			writel_relaxed(axi_err_irq_en, axi_err_irq_en_addr);
		}
	}

	pr_info("hisi axi init finished.\n");

	return ret;

err_a53_cluster1_irqaffinity_register:
	axi_cluster1_irq_num = irq_of_parse_and_map(node, 1);
	free_irq(axi_cluster1_irq_num, NULL);

err_a53_cluster0_irqaffinity_register:
	axi_cluster0_irq_num = irq_of_parse_and_map(node, 0);
	free_irq(axi_cluster0_irq_num, NULL);

err:
	pr_err("hisi axi init failed!\n");
	return ret;
}

static struct of_device_id of_axi_match_tbl[] = {
	{
		.compatible = "hisi,hi6xxx-axi",
	},
	{
		.compatible = "hisi,hi3xxx-axi",
	},
	{ /* end */ }
};

static struct platform_driver hisi_axi_driver = {
	.driver  = {
		.owner = THIS_MODULE,
		.name = "hisi-axi",
		.of_match_table = of_axi_match_tbl,
	},
	.probe  =  hisi_axi_probe,
#ifdef CONFIG_PM
	.suspend = hisi_axi_suspend,
	.resume  = hisi_axi_resume,
#endif
};

static int __init hisi_axi_init(void)
{
	return(platform_driver_register(&hisi_axi_driver));
}

module_init(hisi_axi_init);
