/*
 * Device driver for regulators in Hi6421 IC
 *
 * Copyright (c) 2013 Linaro Ltd.
 * Copyright (c) 2011 Hisilicon.
 *
 * Guodong Xu <guodong.xu@linaro.org>
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
 */

#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/mfd/hi3xxx_hi6421v300.h>
#include <linux/irq.h>
#include <huawei_platform/dsm/dsm_pub.h>
#ifdef CONFIG_HISI_BALONG_MODEM
#include "bsp_icc.h"
#endif

#ifndef NO_IRQ
#define NO_IRQ       0
#endif

static struct dsm_dev dsm_pmu_irq = {
	.name = "dsm_pmu_irq",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 1024,
};
static struct dsm_client *pmu_irq_dclient = NULL;

/* 8-bit register offset in PMIC */
#define HI6421_REG_IRQ1			0x120
#define HI6421_REG_IRQ2			0x121
#define HI6421_REG_IRQ3			0x122
#define HI6421_REG_IRQ4			0x123
#define HI6421_REG_IRQ5			0x124

#define HI6421_REG_IRQM1			0x102
#define HI6421_REG_IRQM2			0x103
#define HI6421_REG_IRQM3			0x104
#define HI6421_REG_IRQM4			0x105
#define HI6421_REG_IRQM5			0x106

#define HI6421_REG_OCPIRQ2		0x126
#define HI6421_LDO2_OCP_CLEAR	0x40

#define HI6421_COUL_OCP_BIT		0x03
#define HI6421_SIM_HPD_BIT			0xcc
#define HI6421_PMU_VERSION_NUM	0x30

#define HI6421_MASK_STATE			0xff
#define HI6421_MASK_RTC_BIT			0x01
#define HI6421_MASK_NO_RTC			0xfe
#define HI6421_MASK_STATE1			0xff
#define HI6421_IRQ_ARRAY			5
#define HI6421_IRQ_KEY_NUM			0
#define HI6421_IRQ_KEY_VALUE		0xc0
#define HI6421_IRQ_KEY_DOWN		7
#define HI6421_IRQ_KEY_UP			6
#define HI6421_IRQ_RTC_NUM			0
#define HI6421_IRQ_RTC_VALUE		0x01 // 0x80

#define HI6421_IRQ_HPD_NUM 3
#define HI6421_IRQ_HPD0_VALUE 0x03
#define HI6421_IRQ_HPD1_VALUE 0x30
#define HI6421_IRQ_LDO11_ADDR 0x8a
#define HI6421_IRQ_LDO12_ADDR 0x8b
#define HI6421_IRQ_LDO11_VALUE 0x05
#define HI6421_IRQ_LDO12_VALUE 0x05

#define SIM0                (0)
#define SIM1                (1)
#define SIM_HPD_LEAVE       (3)

#ifdef CONFIG_HISI_BALONG_MODEM
#define SIM0_CHANNEL_ID ((ICC_CHN_IFC<< 16) | IFC_RECV_FUNC_SIM0)
#define SIM1_CHANNEL_ID ((ICC_CHN_IFC<< 16) | IFC_RECV_FUNC_SIM1)
#endif

static struct of_device_id of_hi6421_pmic_child_match_tbl[] = {
	/* regulators */
	{
		.compatible = "hisilicon,hi6421-ldo",
	},
	{
		.compatible = "hisilicon,hi6421-lsw",
	},
	{
		.compatible = "hisilicon,hi6421-buck012",
	},
	{
		.compatible = "hisilicon,hi6421-buck345",
	},
	{
		.compatible = "hisilicon,hi6421-rtc",
	},
	/* coul */
	{
		.compatible = "hisilicon,hi6421-coul",
	},
	{ /* end */ }
};

static struct of_device_id of_hi6421_pmic_match_tbl[] = {
	{
		.compatible = "hisilicon,hi6421-pmic",
	},
	{ /* end */ }
};

static void hi6421_pmic_sim_hpd_msg_to_ccore(int sim_id)
{
#ifdef CONFIG_HISI_BALONG_MODEM
	u8 sim_state = SIM_HPD_LEAVE;
	u32 channel_id = 0;

	pr_info("[%s:%d]: sim_id=%d, sim_state=%d \n", __func__, __LINE__, sim_id, sim_state);

	if (SIM0 == sim_id) {
		channel_id = SIM0_CHANNEL_ID;
	} else if (SIM1 == sim_id) {
		channel_id = SIM1_CHANNEL_ID;
	} else {
		pr_info("[%s:%d]: sim_id=%d, sim_state=%d \n", __func__, __LINE__, sim_id, sim_state);
		return;
	}

	bsp_icc_send(ICC_CPU_MODEM, channel_id, &sim_state, sizeof(sim_state));
#else
	pr_info("%s, CONFIG_HISI_BALONG_MODEM not defined \n", __func__);
#endif
}

/*
 * The PMIC register is only 8-bit.
 * Hisilicon SoC use hardware to map PMIC register into SoC mapping.
 * At here, we are accessing SoC register with 32-bit.
 */
u32 hi6421_pmic_read(struct hi6421_pmic *pmic, int reg)
{
	u32 ret;

	ret = readl(pmic->regs + (reg << 2));

	return ret;
}
EXPORT_SYMBOL(hi6421_pmic_read);

void hi6421_pmic_write(struct hi6421_pmic *pmic, int reg, u32 val)
{
	unsigned long flags;
	spin_lock_irqsave(&pmic->lock, flags);
	writel(val, pmic->regs + (reg << 2));
	spin_unlock_irqrestore(&pmic->lock, flags);
}
EXPORT_SYMBOL(hi6421_pmic_write);

void hi6421_pmic_rmw(struct hi6421_pmic *pmic, int reg,
		     u32 mask, u32 bits)
{
	u32 data;
	unsigned long flags;

	spin_lock_irqsave(&pmic->lock, flags);
	data = readl(pmic->regs + (reg << 2)) & ~mask;
	data |= mask & bits;
	writel(data, pmic->regs + (reg << 2));
	spin_unlock_irqrestore(&pmic->lock, flags);
}
EXPORT_SYMBOL(hi6421_pmic_rmw);

extern irqreturn_t pmu_rtc_handler(int irq, void *data);

static irqreturn_t hi6421_irq_handler(int irq, void *data)
{
	struct hi6421_pmic *pmic = (struct hi6421_pmic *)data;
	unsigned long pending;
	int i, offset,error_num = 0 , dsm_error_found = -1;
	unsigned char IRQ_REG[HI6421_IRQ_ARRAY];
	struct error_mask {
		unsigned int error_type;
		int reg_num;
	};

	struct error_mask error_masks[] = {
		{PMU_IRQ_REG0_ERROR_OTMP_R, 0},
		{PMU_IRQ_REG0_ERROR_HRESETN_F, 0},
		{PMU_IRQ_REG1_ERROR_BOI_R, 1},
		{PMU_IRQ_REG1_ERROR_VBUS_DET_4P3_F, 1},
		{PMU_IRQ_REG1_ERROR_VBUS_DET_5P5_R, 1},
		{PMU_IRQ_REG2_ERROR_VSYS_OV, 2},
		{PMU_IRQ_REG2_ERROR_VSYS_UV, 2},
		{PMU_IRQ_REG2_ERROR_BST_UVP, 2},
		{PMU_IRQ_REG2_ERROR_BST_OVP, 2},
		{PMU_IRQ_REG4_ERROR_LED_SHORT_R, 4},
		{PMU_IRQ_REG4_ERROR_LED_OPEN_R, 4},
		{PMU_IRQ_REG4_ERROR_VBAT2P3_F, 4},
		{PMU_IRQ_REG4_ERROR_VBAT2P8_F, 4},
		{PMU_IRQ_REG4_ERROR_OTMP150_R, 4},
	};

	if (!dsm_client_ocuppy(pmu_irq_dclient)) {
		dsm_error_found++;
	}

	for (i = 0; i < HI6421_IRQ_ARRAY; i++) {
		pending = readl(pmic->regs + ((i + HI6421_REG_IRQ1) << 2));
		pending &= HI6421_MASK_FIELD;
		if (pending != 0) {
			pr_debug("pending[%d]=0x%lx\n\r", i, pending);
		}
		IRQ_REG[i] = (unsigned char) pending;
		writel(pending, pmic->regs + ((i + HI6421_REG_IRQ1) << 2));

		/*solve powerkey order*/
		if ((HI6421_IRQ_KEY_NUM == i) && ((pending & HI6421_IRQ_KEY_VALUE) == HI6421_IRQ_KEY_VALUE)) {
			generic_handle_irq(pmic->irqs[HI6421_IRQ_KEY_DOWN]);
			generic_handle_irq(pmic->irqs[HI6421_IRQ_KEY_UP]);
			pending &= (~HI6421_IRQ_KEY_VALUE);
		}

		/*After HPD,software close LDO*/
		if (HI6421_IRQ_HPD_NUM == i) {
			pr_info("[%s:%d]:sim pending value is 0x%x!\n", __func__, __LINE__, pending);
			if((pending & HI6421_IRQ_HPD0_VALUE)) {
				pr_info("[%s:%d]:IRQ HPD0 happend,software close ldo11!\n", __func__, __LINE__);
				/*close ldo11*/
				hi6421_pmic_write(pmic, HI6421_IRQ_LDO11_ADDR, HI6421_IRQ_LDO11_VALUE);
				hi6421_pmic_sim_hpd_msg_to_ccore(SIM0);
			}

			if ((pending & HI6421_IRQ_HPD1_VALUE)) {
				pr_info("[%s:%d]:IRQ HPD1 happend,software close ldo12!\n", __func__, __LINE__);
				/*close ldo12*/
				hi6421_pmic_write(pmic, HI6421_IRQ_LDO12_ADDR, HI6421_IRQ_LDO12_VALUE);
				hi6421_pmic_sim_hpd_msg_to_ccore(SIM1);
			}
		}

		if ((HI6421_IRQ_RTC_NUM == i) && ((pending & HI6421_IRQ_RTC_VALUE) == HI6421_IRQ_RTC_VALUE)) {
			pmu_rtc_handler( irq, data);
			pending &= (~HI6421_IRQ_RTC_VALUE);
		}

		if (pending) {
			for_each_set_bit(offset, &pending, HI6421_BITS)
				generic_handle_irq(pmic->irqs[offset + i * HI6421_BITS]);
		}
	}
	if (dsm_error_found >= 0)
	{
		for (i = 0; i < (sizeof(error_masks)/sizeof(struct error_mask)); i++)
		{
			if ((error_masks[i].error_type & IRQ_REG[error_masks[i].reg_num]) == error_masks[i].error_type)
			{
				dsm_error_found ++;
				error_num = i;
				break;
			}
		}
		if (dsm_error_found > 0)
		{
			dsm_client_record(pmu_irq_dclient, "IRQ_REG[0-4]:0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", IRQ_REG[0],IRQ_REG[1],IRQ_REG[2],IRQ_REG[3],IRQ_REG[4]);
			dsm_client_notify(pmu_irq_dclient, DSM_PMU_IRQ_ERROR_NO + error_num );
		}
		else
		{
			dsm_client_unocuppy(pmu_irq_dclient);
		}
	}
	else {
		/* dsm irq_dclient ocuppy failed */
	}

	return IRQ_HANDLED;
}

static void hi6421_irq_mask(struct irq_data *d)
{
	struct hi6421_pmic *pmic = irq_data_get_irq_chip_data(d);
	u32 data, offset;
	unsigned long flags;

	offset = ((irqd_to_hwirq(d) >> 3) + HI6421_REG_IRQM1) << 2;
	spin_lock_irqsave(&pmic->lock, flags);
	data = readl(pmic->regs + offset);
	data |= (1 << (irqd_to_hwirq(d) & 0x07));
	writel(data, pmic->regs + offset);
	spin_unlock_irqrestore(&pmic->lock, flags);
}

static void hi6421_irq_unmask(struct irq_data *d)
{
	struct hi6421_pmic *pmic = irq_data_get_irq_chip_data(d);
	u32 data, offset;
	unsigned long flags;

	offset = ((irqd_to_hwirq(d) >> 3) + HI6421_REG_IRQM1) << 2;
	spin_lock_irqsave(&pmic->lock, flags);
	data = readl(pmic->regs + offset);
	data &= ~(1 << (irqd_to_hwirq(d) & 0x07));
	writel(data, pmic->regs + offset);
	spin_unlock_irqrestore(&pmic->lock, flags);
}

static struct irq_chip hi6421_irqchip = {
	.name		= "hi6421v300-irq",
	.irq_mask	= hi6421_irq_mask,
	.irq_unmask	= hi6421_irq_unmask,
	.irq_disable	= hi6421_irq_mask,
	.irq_enable	= hi6421_irq_unmask,
};

static int hi6421_irq_map(struct irq_domain *d, unsigned int virq,
			  irq_hw_number_t hw)
{
	struct hi6421_pmic *pmic = d->host_data;

	irq_set_chip_and_handler_name(virq, &hi6421_irqchip,
				      handle_simple_irq, "hi6421");
	irq_set_chip_data(virq, pmic);
	irq_set_irq_type(virq, IRQ_TYPE_NONE);

	return 0;
}

static struct irq_domain_ops hi6421_domain_ops = {
	.map	= hi6421_irq_map,
	.xlate	= irq_domain_xlate_twocell,
};

static int hi6421_pmic_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct hi6421_pmic *pmic = NULL;
	enum of_gpio_flags flags;
	unsigned int register_info[3], ocp_ctrl_reg, ocp_buck_delay, ocp_ldo_delay;
	int ret, i, virq;
	int fpga_flag = 0;
	u32 value;

	pmic = devm_kzalloc(dev, sizeof(*pmic), GFP_KERNEL);
	if (!pmic) {
		dev_err(dev, "cannot allocate hi6421_pmic device info\n");
		return -ENOMEM;
	}

	/* get resources */
	pmic->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!pmic->res) {
		dev_err(dev, "platform_get_resource err\n");
		return -ENOENT;
	}

	if (!devm_request_mem_region(dev, pmic->res->start,
				     resource_size(pmic->res),
				     pdev->name)) {
		dev_err(dev, "cannot claim register memory\n");
		return -ENOMEM;
	}

	pmic->regs = devm_ioremap(dev, pmic->res->start,
				  resource_size(pmic->res));
	if (!pmic->regs) {
		dev_err(dev, "cannot map register memory\n");
		return -ENOMEM;
	}

	/* TODO: get and enable clk request */
	spin_lock_init(&pmic->lock);

	of_property_read_u32_array(np, "hisilicon,pmic_fpga_flag", &fpga_flag, 1);

	if (1 == fpga_flag) {
		goto after_irq_register;
	}

	pmic->gpio = of_get_gpio_flags(np, 0, &flags);
	if (pmic->gpio < 0)
		return pmic->gpio;

	if (!gpio_is_valid(pmic->gpio))
		return -EINVAL;

	ret = gpio_request_one(pmic->gpio, GPIOF_IN, "pmic");
	if (ret < 0) {
		dev_err(dev, "failed to request gpio%d\n", pmic->gpio);
		return ret;
	}

	pmic->irq = gpio_to_irq(pmic->gpio);
	/* clear IRQ status */
	/* Don't change the RTC IRQ mask state */
	value = hi6421_pmic_read(pmic, HI6421_REG_IRQM1);
	hi6421_pmic_write(pmic, HI6421_REG_IRQM1, ((value & HI6421_MASK_RTC_BIT)|HI6421_MASK_NO_RTC));
	hi6421_pmic_write(pmic, HI6421_REG_IRQM2, HI6421_MASK_STATE);
	hi6421_pmic_write(pmic, HI6421_REG_IRQM3, HI6421_MASK_STATE);
	hi6421_pmic_write(pmic, HI6421_REG_IRQM4, HI6421_MASK_STATE);
	hi6421_pmic_write(pmic, HI6421_REG_IRQM5, HI6421_MASK_STATE);
	hi6421_pmic_write(pmic, HI6421_REG_IRQ1, HI6421_MASK_STATE);
	hi6421_pmic_write(pmic, HI6421_REG_IRQ2, HI6421_MASK_STATE);
	hi6421_pmic_write(pmic, HI6421_REG_IRQ3, HI6421_MASK_STATE1);
	hi6421_pmic_write(pmic, HI6421_REG_IRQ4, HI6421_MASK_STATE);
	hi6421_pmic_write(pmic, HI6421_REG_IRQ5, HI6421_MASK_STATE);
	/*Clear LDO2 OCP*/
	hi6421_pmic_write(pmic, HI6421_REG_OCPIRQ2, HI6421_LDO2_OCP_CLEAR);

	pmic->domain = irq_domain_add_simple(np, HI6421_NR_IRQ, 0,
					     &hi6421_domain_ops, pmic);
	if (!pmic->domain) {
		dev_err(dev, "failed irq domain add simple!\n");
		ret = -ENODEV;
		goto irq_domain;
	}

	for (i = 0; i < HI6421_NR_IRQ; i++) {
		virq = irq_create_mapping(pmic->domain, i);
		if (virq == NO_IRQ) {
			pr_debug("Failed mapping hwirq\n");
			ret = -ENOSPC;
			goto irq_create_mapping;
		}
		pmic->irqs[i] = virq;
	}

	ret = request_threaded_irq(pmic->irq, hi6421_irq_handler, NULL,
				IRQF_TRIGGER_LOW | IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND,
				   "pmic", pmic);
	if (ret < 0) {
		dev_err(dev, "could not claim pmic %d\n", ret);
		ret = -ENODEV;
		goto request_theaded_irq;
	}

	/*if there is a ldo current over,support ocp qudou*/
	ret = of_property_read_u32_array(np, "hisilicon,hi6421-ocp-deb-ctrl",
						register_info, 3);
	if (ret) {
		dev_err(dev, "no hisilicon,hi6421-ldo-ocp-deb-ctrl property set\n");
		ret = -ENODEV;
		goto read_u32_array;
	}
	ocp_ctrl_reg = register_info[0];
	ocp_buck_delay = register_info[1];
	ocp_ldo_delay = register_info[2];
	hi6421_pmic_write(pmic, ocp_ctrl_reg, (ocp_buck_delay | ocp_ldo_delay));

after_irq_register:

	platform_set_drvdata(pdev, pmic);

	/* populate sub nodes */
	of_platform_populate(np, of_hi6421_pmic_child_match_tbl, NULL, dev);

	return 0;

read_u32_array:
	free_irq(pmic->irq, pmic);
request_theaded_irq:
irq_create_mapping:
irq_domain:
	gpio_free(pmic->gpio);
	return ret;
}

static int hi6421_pmic_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hi6421_pmic *pmic = platform_get_drvdata(pdev);

	free_irq(pmic->irq, pmic);
	gpio_free(pmic->gpio);
	devm_iounmap(dev, pmic->regs);
	devm_release_mem_region(dev, pmic->res->start,
				resource_size(pmic->res));
	devm_kfree(dev, pmic);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver hi6421_pmic_driver = {
	.driver = {
		.name	= "hi6421_pmic",
		.owner  = THIS_MODULE,
		.of_match_table = of_hi6421_pmic_match_tbl,
	},
	.probe	= hi6421_pmic_probe,
	.remove	= hi6421_pmic_remove,
};
/*module_platform_driver(hi6421_pmic_driver);*/
static int __init hi6421_pmic_init(void)
{
	return platform_driver_register(&hi6421_pmic_driver);
}

static void __exit hi6421_pmic_exit(void)
{
	platform_driver_unregister(&hi6421_pmic_driver);
}

fs_initcall(hi6421_pmic_init);
module_exit(hi6421_pmic_exit);

MODULE_AUTHOR("Guodong Xu <guodong.xu@linaro.org>");
MODULE_DESCRIPTION("Hi6421 PMIC driver");
MODULE_LICENSE("GPL v2");
