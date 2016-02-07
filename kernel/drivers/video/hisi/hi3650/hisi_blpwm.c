/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/

#include "hisi_fb.h"


/* default pwm clk */
#define DEFAULT_PWM_CLK_RATE	(120 * 1000000L)

static char __iomem *hisifd_blpwm_base;
static struct clk *g_dss_blpwm_clk;

static struct pinctrl_data blpwmpctrl;

#if !defined(CONFIG_ARCH_HI3630FPGA) && !defined(CONFIG_HISI_3635_FPGA) \
	&& !defined(CONFIG_HISI_3650_FPGA)
static struct pinctrl_cmd_desc blpwm_pinctrl_init_cmds[] = {
	{DTYPE_PINCTRL_GET, &blpwmpctrl, 0},
	{DTYPE_PINCTRL_STATE_GET, &blpwmpctrl, DTYPE_PINCTRL_STATE_DEFAULT},
	{DTYPE_PINCTRL_STATE_GET, &blpwmpctrl, DTYPE_PINCTRL_STATE_IDLE},
};
#endif

static struct pinctrl_cmd_desc blpwm_pinctrl_normal_cmds[] = {
	{DTYPE_PINCTRL_SET, &blpwmpctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};

static struct pinctrl_cmd_desc blpwm_pinctrl_lowpower_cmds[] = {
	{DTYPE_PINCTRL_SET, &blpwmpctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc blpwm_pinctrl_finit_cmds[] = {
	{DTYPE_PINCTRL_PUT, &blpwmpctrl, 0},
};


#define BLPWM_OUT_CTRL	(0x100)
#define BLPWM_OUT_DIV	(0x104)
#define BLPWM_OUT_CFG	(0x108)

#define BLPWM_BL_LEVEL_MIN	(9)
#define BLPWM_OUT_PRECISION	(800)


int hisi_blpwm_set_backlight(struct hisi_fb_data_type *hisifd)
{
	char __iomem *blpwm_base = NULL;
	uint32_t bl_level = 0;
	uint32_t brightness = 0;

	BUG_ON(hisifd == NULL);

	blpwm_base = hisifd_blpwm_base;
	BUG_ON(blpwm_base == NULL);

	bl_level = hisifd->bl_level;
	if (bl_level > hisifd->panel_info.bl_max) {
		bl_level = hisifd->panel_info.bl_max;
	}

	if ((bl_level > 0) && (bl_level < BLPWM_BL_LEVEL_MIN)) {
		bl_level = BLPWM_BL_LEVEL_MIN;
	}

	bl_level = (bl_level * BLPWM_OUT_PRECISION) / hisifd->panel_info.bl_max;
	brightness = (bl_level << 16) | (BLPWM_OUT_PRECISION - bl_level);

	outp32(blpwm_base + BLPWM_OUT_DIV, 0x2);
	outp32(blpwm_base + BLPWM_OUT_CFG, brightness);

	return 0;
}

int hisi_blpwm_on(struct platform_device *pdev)
{
	int ret = 0;
	struct clk *clk_tmp = NULL;
	char __iomem *blpwm_base = NULL;

	BUG_ON(pdev == NULL);
	blpwm_base = hisifd_blpwm_base;
	BUG_ON(blpwm_base == NULL);

	clk_tmp = g_dss_blpwm_clk;
	if (clk_tmp) {
		ret = clk_prepare(clk_tmp);
		if (ret) {
			HISI_FB_ERR("dss_blpwm_clk clk_prepare failed, error=%d!\n", ret);
			return -EINVAL;
		}

		ret = clk_enable(clk_tmp);
		if (ret) {
			HISI_FB_ERR("dss_blpwm_clk clk_enable failed, error=%d!\n", ret);
			return -EINVAL;
		}
	}

	ret = pinctrl_cmds_tx(pdev, blpwm_pinctrl_normal_cmds,
		ARRAY_SIZE(blpwm_pinctrl_normal_cmds));

	outp32(blpwm_base + BLPWM_OUT_CTRL, 0x1);

	return ret;
}

int hisi_blpwm_off(struct platform_device *pdev)
{
	int ret = 0;
	struct clk *clk_tmp = NULL;
	char __iomem *blpwm_base = NULL;

	BUG_ON(pdev == NULL);
	blpwm_base = hisifd_blpwm_base;
	BUG_ON(blpwm_base == NULL);

	outp32(blpwm_base + BLPWM_OUT_CTRL, 0x0);

	ret = pinctrl_cmds_tx(pdev, blpwm_pinctrl_lowpower_cmds,
		ARRAY_SIZE(blpwm_pinctrl_lowpower_cmds));

	clk_tmp = g_dss_blpwm_clk;
	if (clk_tmp) {
		clk_disable(clk_tmp);
		clk_unprepare(clk_tmp);
	}

	return ret;
}

static int hisi_blpwm_probe(struct platform_device *pdev)
{
	struct device_node *np = NULL;
	int ret = 0;

	HISI_FB_DEBUG("+.\n");

	BUG_ON(pdev == NULL);

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_BLPWM_NAME);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_BLPWM_NAME);
		ret = -ENXIO;
		goto err_return;
	}

	/* get blpwm reg base */
	hisifd_blpwm_base = of_iomap(np, 0);

#if !defined(CONFIG_ARCH_HI3630FPGA) && !defined(CONFIG_HISI_3635_FPGA) \
	&& !defined(CONFIG_HISI_3650_FPGA)
	/* blpwm pinctrl init */
	ret = pinctrl_cmds_tx(pdev, blpwm_pinctrl_init_cmds,
		ARRAY_SIZE(blpwm_pinctrl_init_cmds));
	if (ret != 0) {
		HISI_FB_ERR("Init blpwm pinctrl failed! ret=%d.\n", ret);
		goto err_return;
	}

	/* get blpwm clk resource */
	g_dss_blpwm_clk = of_clk_get(np, 0);
	if (IS_ERR(g_dss_blpwm_clk)) {
		HISI_FB_ERR("%s clock not found: %d!\n",
			np->name, (int)PTR_ERR(g_dss_blpwm_clk));
		ret = -ENXIO;
		goto err_return;
	}

	ret = clk_set_rate(g_dss_blpwm_clk, DEFAULT_PWM_CLK_RATE);
	if (ret < 0) {
		HISI_FB_ERR("dss_blpwm_clk clk_set_rate(%lu) failed, error=%d!\n",
			DEFAULT_PWM_CLK_RATE, ret);
		ret = -EINVAL;
		goto err_return;
	}

	HISI_FB_INFO("dss_blpwm_clk:[%lu]->[%lu].\n",
		DEFAULT_PWM_CLK_RATE, clk_get_rate(g_dss_blpwm_clk));
#endif

	hisi_fb_device_set_status0(DTS_PWM_READY);

	HISI_FB_DEBUG("-.\n");

	return 0;

err_return:
	return ret;
}

static int hisi_blpwm_remove(struct platform_device *pdev)
{
	struct clk *clk_tmp = NULL;
	int ret = 0;

	ret = pinctrl_cmds_tx(pdev, blpwm_pinctrl_finit_cmds,
		ARRAY_SIZE(blpwm_pinctrl_finit_cmds));

	clk_tmp = g_dss_blpwm_clk;
	if (clk_tmp) {
		clk_put(clk_tmp);
		clk_tmp = NULL;
	}

	return ret;
}

static const struct of_device_id hisi_blpwm_match_table[] = {
	{
		.compatible = DTS_COMP_BLPWM_NAME,
		.data = NULL,
	}
};
MODULE_DEVICE_TABLE(of, hisi_blpwm_match_table);

static struct platform_driver this_driver = {
	.probe = hisi_blpwm_probe,
	.remove = hisi_blpwm_remove,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = DEV_NAME_BLPWM,
		.of_match_table = of_match_ptr(hisi_blpwm_match_table),
	},
};

static int __init hisi_blpwm_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(hisi_blpwm_init);
