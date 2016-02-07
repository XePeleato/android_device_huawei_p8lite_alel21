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
#include <linux/timer.h>
#include <linux/delay.h>

/* default pwm clk */
#define DEFAULT_PWM_CLK_RATE	(120 * 1000000L)

static char __iomem *hisifd_pwm_base;
static struct clk *g_dss_pwm_clk;
static struct platform_device *g_pwm_pdev;

static struct pinctrl_data pwmpctrl;


static struct pinctrl_cmd_desc pwm_pinctrl_init_cmds[] = {
	{DTYPE_PINCTRL_GET, &pwmpctrl, 0},
	{DTYPE_PINCTRL_STATE_GET, &pwmpctrl, DTYPE_PINCTRL_STATE_DEFAULT},
	{DTYPE_PINCTRL_STATE_GET, &pwmpctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc pwm_pinctrl_normal_cmds[] = {
	{DTYPE_PINCTRL_SET, &pwmpctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};

static struct pinctrl_cmd_desc pwm_pinctrl_lowpower_cmds[] = {
	{DTYPE_PINCTRL_SET, &pwmpctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc pwm_pinctrl_finit_cmds[] = {
	{DTYPE_PINCTRL_PUT, &pwmpctrl, 0},
};


#define PWM_LOCK_OFFSET	(0x0000)
#define PWM_CTL_OFFSET	(0X0004)
#define PWM_CFG_OFFSET	(0x0008)
#define PWM_PR0_OFFSET	(0x0100)
#define PWM_PR1_OFFSET	(0x0104)
#define PWM_C0_MR_OFFSET	(0x0300)
#define PWM_C0_MR0_OFFSET	(0x0304)

#define PWM_BL_LEVEL_MIN	(4)
#define PWM_OUT_PRECISION	(800)

#ifdef CONFIG_BACKLIGHT_10000
#define CABC_DIMMING_STEP_TOTAL_NUM    32
#define CABC_PWM_DUTY_MAX_LEVEL	        254
static struct task_struct *cabc_pwm_task;

struct bl_info{
	int32_t bl_max;
	int32_t ap_brightness;
	int32_t index_cabc_dimming;
	int32_t cabc_pwm;
	int32_t prev_cabc_pwm;
	int32_t current_cabc_pwm;
	int32_t cabc_pwm_in;
};

static struct bl_info g_bl_info;

static void init_bl_info(void)
{
	g_bl_info.bl_max = 254;
	g_bl_info.ap_brightness = 0;
	g_bl_info.index_cabc_dimming = 0;
	g_bl_info.cabc_pwm = 254;
	g_bl_info.prev_cabc_pwm =254;
	g_bl_info.current_cabc_pwm =254;
	g_bl_info.cabc_pwm_in =254;
	return ;
}

static void update_backlight(uint32_t backlight)
{
	uint32_t bl_level = (backlight * PWM_OUT_PRECISION) / g_bl_info.bl_max;
	HISI_FB_INFO("update_backlight: backlight = %d!\n", backlight);
	BUG_ON(hisifd_pwm_base == NULL);
	outp32(hisifd_pwm_base + PWM_LOCK_OFFSET, 0x1acce551);
	outp32(hisifd_pwm_base + PWM_CTL_OFFSET, 0x0);
	outp32(hisifd_pwm_base + PWM_CFG_OFFSET, 0x2);
	outp32(hisifd_pwm_base + PWM_PR0_OFFSET, 0x1);
	outp32(hisifd_pwm_base + PWM_PR1_OFFSET, 0x2);
	outp32(hisifd_pwm_base + PWM_CTL_OFFSET, 0x1);
	outp32(hisifd_pwm_base + PWM_C0_MR_OFFSET, (PWM_OUT_PRECISION - 1));
	outp32(hisifd_pwm_base + PWM_C0_MR0_OFFSET, bl_level);
}

static int cabc_pwm_thread(void *p)
{
	while(!kthread_should_stop())
	{
		if(g_bl_info.index_cabc_dimming > CABC_DIMMING_STEP_TOTAL_NUM)
		{
			set_current_state(TASK_INTERRUPTIBLE);
			schedule();
			g_bl_info.index_cabc_dimming =1 ;
		}
		else
		{
			if(g_bl_info.cabc_pwm_in != 0)
			{
			    g_bl_info.cabc_pwm = g_bl_info.cabc_pwm_in;
			    g_bl_info.cabc_pwm_in = 0;
			    g_bl_info.index_cabc_dimming = 1;
			    g_bl_info.prev_cabc_pwm = g_bl_info.current_cabc_pwm;
			}
			int32_t delta_cabc_pwm = g_bl_info.cabc_pwm - g_bl_info.prev_cabc_pwm;
			int32_t pwm_duty=delta_cabc_pwm*g_bl_info.index_cabc_dimming/32 + delta_cabc_pwm *g_bl_info.index_cabc_dimming % 32 /16;
			g_bl_info.current_cabc_pwm = g_bl_info.prev_cabc_pwm +   pwm_duty;
			HISI_FB_INFO("g_bl_info.current_cabc_pwm = %d,g_bl_info.index_cabc_dimming= %d,pwm_duty= %d,g_bl_info.index_cabc_dimming= %d!\n",g_bl_info.current_cabc_pwm ,g_bl_info.index_cabc_dimming,pwm_duty,g_bl_info.index_cabc_dimming);
			int32_t backlight = g_bl_info.current_cabc_pwm * g_bl_info.ap_brightness / CABC_PWM_DUTY_MAX_LEVEL;
			if (backlight > 0 && backlight < PWM_BL_LEVEL_MIN) {
			    backlight = PWM_BL_LEVEL_MIN;
			}
			update_backlight(backlight);
			g_bl_info.index_cabc_dimming++;
			msleep(16);
		}
	}
	return 0;
}

int hisi_cabc_set_backlight(uint32_t cabc_pwm_in)
{
	if(g_bl_info.ap_brightness == 0 || cabc_pwm_task == NULL)
		return 0;
	g_bl_info.cabc_pwm_in = cabc_pwm_in;
	wake_up_process(cabc_pwm_task);
	return 0;
}
#endif

int hisi_pwm_set_backlight(struct hisi_fb_data_type *hisifd)
{
	char __iomem *pwm_base = NULL;
	uint32_t bl_max = 0;
	uint32_t bl_level = 0;

	BUG_ON(hisifd == NULL);

	pwm_base = hisifd_pwm_base;
	BUG_ON(pwm_base == NULL);

	#ifdef CONFIG_BACKLIGHT_10000
	    g_bl_info.bl_max = hisifd->panel_info.bl_max;
	#endif
	bl_level = hisifd->bl_level;
	bl_max = hisifd->panel_info.bl_max;
	if (bl_level > hisifd->panel_info.bl_max) {
		bl_level = hisifd->panel_info.bl_max;
	}

	if (bl_max < 1) {
		HISI_FB_ERR("bl_max=%d is invalid!\n", bl_max);
		return -EINVAL;
	}

	if ((bl_level > 0) && (bl_level < PWM_BL_LEVEL_MIN)) {
		bl_level = PWM_BL_LEVEL_MIN;
	}

	#ifdef CONFIG_BACKLIGHT_10000
	      if(bl_level > 0){
		    bl_level= bl_level * g_bl_info.current_cabc_pwm / CABC_PWM_DUTY_MAX_LEVEL;
		    bl_level =  bl_level < PWM_BL_LEVEL_MIN ? PWM_BL_LEVEL_MIN : bl_level ;
	      }
	      g_bl_info.ap_brightness = bl_level;
	      HISI_FB_INFO("g_bl_info.ap_brightness= %d\n",bl_level);
	      if(bl_level > 0 && g_bl_info.index_cabc_dimming > 0 &&  g_bl_info.index_cabc_dimming < 33 )
	      {
		    HISI_FB_INFO("cabc is dimming and g_bl_info.index_cabc_dimming = %d\n",g_bl_info.index_cabc_dimming);
		    return;
	      }
	#endif

	bl_level = (bl_level * PWM_OUT_PRECISION) / bl_max;

	outp32(pwm_base + PWM_LOCK_OFFSET, 0x1acce551);
	outp32(pwm_base + PWM_CTL_OFFSET, 0x0);
	outp32(pwm_base + PWM_CFG_OFFSET, 0x2);
	outp32(pwm_base + PWM_PR0_OFFSET, 0x1);
	outp32(pwm_base + PWM_PR1_OFFSET, 0x2);
	outp32(pwm_base + PWM_CTL_OFFSET, 0x1);
	outp32(pwm_base + PWM_C0_MR_OFFSET, (PWM_OUT_PRECISION - 1));
	outp32(pwm_base + PWM_C0_MR0_OFFSET, bl_level);

	return 0;
}

int hisi_pwm_on(struct platform_device *pdev)
{
	int ret = 0;
	struct clk *clk_tmp = NULL;

	BUG_ON(pdev == NULL);

	clk_tmp = g_dss_pwm_clk;
	if (clk_tmp) {
		ret = clk_prepare(clk_tmp);
		if (ret) {
			HISI_FB_ERR("dss_pwm_clk clk_prepare failed, error=%d!\n", ret);
			return -EINVAL;
		}

		ret = clk_enable(clk_tmp);
		if (ret) {
			HISI_FB_ERR("dss_pwm_clk clk_enable failed, error=%d!\n", ret);
			return -EINVAL;
		}
	}

	ret = pinctrl_cmds_tx(g_pwm_pdev, pwm_pinctrl_normal_cmds,
		ARRAY_SIZE(pwm_pinctrl_normal_cmds));

	#ifdef CONFIG_BACKLIGHT_10000
		init_bl_info();
		cabc_pwm_task = kthread_create(cabc_pwm_thread, NULL, "cabc_pwm_task");
		if(IS_ERR(cabc_pwm_task))
		{
			HISI_FB_ERR("Unable to start kernel cabc_pwm_task./n");
			cabc_pwm_task = NULL;
			return -EINVAL;
		}
	#endif
	return ret;
}

int hisi_pwm_off(struct platform_device *pdev)
{
	int ret = 0;
	#ifdef CONFIG_BACKLIGHT_10000
	    if(cabc_pwm_task)
	    {
		    kthread_stop(cabc_pwm_task);
		    cabc_pwm_task = NULL;
	    }
	#endif

	struct clk *clk_tmp = NULL;

	BUG_ON(pdev == NULL);

	ret = pinctrl_cmds_tx(g_pwm_pdev, pwm_pinctrl_lowpower_cmds,
		ARRAY_SIZE(pwm_pinctrl_lowpower_cmds));

	clk_tmp = g_dss_pwm_clk;
	if (clk_tmp) {
		clk_disable(clk_tmp);
		clk_unprepare(clk_tmp);
	}

	return ret;
}

static int hisi_pwm_probe(struct platform_device *pdev)
{
	struct device_node *np = NULL;
	int ret = 0;

	HISI_FB_DEBUG("+.\n");

	BUG_ON(pdev == NULL);

	g_pwm_pdev = pdev;

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_PWM_NAME);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_PWM_NAME);
		ret = -ENXIO;
		goto err_return;
	}

	/* get pwm reg base */
	hisifd_pwm_base = of_iomap(np, 0);

	/* pwm pinctrl init */
	ret = pinctrl_cmds_tx(pdev, pwm_pinctrl_init_cmds,
		ARRAY_SIZE(pwm_pinctrl_init_cmds));
	if (ret != 0) {
		HISI_FB_ERR("Init pwm pinctrl failed! ret=%d.\n", ret);
		goto err_return;
	}

	/* get blpwm clk resource */
	g_dss_pwm_clk = of_clk_get(np, 0);
	if (IS_ERR(g_dss_pwm_clk)) {
		HISI_FB_ERR("%s clock not found: %d!\n",
			np->name, (int)PTR_ERR(g_dss_pwm_clk));
		ret = -ENXIO;
		goto err_return;
	}

	ret = clk_set_rate(g_dss_pwm_clk, DEFAULT_PWM_CLK_RATE);
	if (ret != 0) {
		HISI_FB_ERR("dss_pwm_clk clk_set_rate(%lu) failed, error=%d!\n",
			DEFAULT_PWM_CLK_RATE, ret);
		ret = -EINVAL;
		goto err_return;
	}

	HISI_FB_INFO("dss_pwm_clk:[%lu]->[%lu].\n",
		DEFAULT_PWM_CLK_RATE, clk_get_rate(g_dss_pwm_clk));

	hisi_fb_device_set_status0(DTS_PWM_READY);

	HISI_FB_DEBUG("-.\n");

	return 0;

err_return:
	return ret;
}

static int hisi_pwm_remove(struct platform_device *pdev)
{
	struct clk *clk_tmp = NULL;
	int ret = 0;

	ret = pinctrl_cmds_tx(pdev, pwm_pinctrl_finit_cmds,
		ARRAY_SIZE(pwm_pinctrl_finit_cmds));

	clk_tmp = g_dss_pwm_clk;
	if (clk_tmp) {
		clk_put(clk_tmp);
		clk_tmp = NULL;
	}

	return ret;
}

static const struct of_device_id hisi_pwm_match_table[] = {
	{
		.compatible = DTS_COMP_PWM_NAME,
		.data = NULL,
	},
	{},
};
MODULE_DEVICE_TABLE(of, hisi_pwm_match_table);

static struct platform_driver this_driver = {
	.probe = hisi_pwm_probe,
	.remove = hisi_pwm_remove,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = DEV_NAME_PWM,
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hisi_pwm_match_table),
	},
};

static int __init hisi_pwm_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(hisi_pwm_init);
