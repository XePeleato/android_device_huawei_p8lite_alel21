/* Copyright (c) 2008-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
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
#include <huawei_platform/log/log_jank.h>


#define DTS_COMP_TOSHIBA_MDY90 "hisilicon,mipi_toshiba_MDY90"

static struct hisi_fb_panel_data toshiba_panel_data;
static bool g_debug_enable = false;


/*******************************************************************************
** Power ON Sequence(sleep mode to Normal mode)
*/
static char MCAP[] = {
	0xB0,
	0x00,
};

static char set_address[] = {
	0x36,
	0xC0,
};

static char bl_level[] = {
	0x51,
	0xff, 0xff,
};

static char bl_enable[] = {
	0x53,
	0x24,
};

static char exit_sleep[] = {
	0x11,
};

static char display_on[] = {
	0x29,
};
static char soft_reset[] = {
	0x01,
};

/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/
static char display_off[] = {
	0x28,
};

static char enter_sleep[] = {
	0x10,
};

/* BASE Sequence */
static char MCAP_param1[] = {
	0xb0,
	0x04,
};

static char MCAP_param2[] = {
	0xb0,
	0x03,
};

/* CE Sequence */
static char ce_param[] = {
	0xca,
	0x01, 0x80, 0xdc, 0xf0, 0xec,
	0xf0, 0x90, 0xa0, 0x08, 0x10,
	0x14, 0xab, 0x0a, 0x4a, 0x37,
	0xa0, 0x55, 0xf8, 0x0c, 0x10,
	0x20, 0x10, 0x3f, 0x3f, 0x19,
	0xd6, 0x10, 0x10, 0x3f, 0x3f,
	0x3f, 0x3f,
};

/* Sharpness Sequence */
static char sharpness_param[] = {
	0xdd,
	0x11, 0x93, /*ADST 150%->50%, AVST 100%->50%*/
};


static struct dsi_cmd_desc toshiba_display_on_cmd[] = {
	{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
		sizeof(soft_reset), soft_reset},
	{DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
		sizeof(MCAP), MCAP},
	{DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(bl_level), bl_level},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(set_address), set_address},

	{DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
		sizeof(MCAP_param1), MCAP_param1},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(sharpness_param), sharpness_param},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(ce_param), ce_param},
	{DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
		sizeof(MCAP_param2), MCAP_param2},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(display_on), display_on},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(exit_sleep), exit_sleep},
};

static struct dsi_cmd_desc toshiba_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 0, 20, WAIT_TYPE_MS,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 0, 80, WAIT_TYPE_MS,
		sizeof(enter_sleep), enter_sleep}
};


/*******************************************************************************
** LCD VCC
*/
#define VCC_LCDIO_NAME		"lcdio-vcc"
#define VCC_LCDANALOG_NAME	"lcdanalog-vcc"

static struct regulator *vcc_lcdio;
static struct regulator *vcc_lcdanalog;

static struct vcc_desc toshiba_lcd_vcc_init_cmds[] = {
	/* vcc get */
	{DTYPE_VCC_GET, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 0},
	{DTYPE_VCC_GET, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 0},

	/* vcc set voltage */
	{DTYPE_VCC_SET_VOLTAGE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 2800000, 2800000, WAIT_TYPE_MS, 0},
};

static struct vcc_desc toshiba_lcd_vcc_finit_cmds[] = {
	/* vcc put */
	{DTYPE_VCC_PUT, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 0},
	{DTYPE_VCC_PUT, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 0},
};

static struct vcc_desc toshiba_lcd_vcc_enable_cmds[] = {
	/* vcc enable */
	{DTYPE_VCC_ENABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 3},
	{DTYPE_VCC_ENABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 3},
};

static struct vcc_desc toshiba_lcd_vcc_disable_cmds[] = {
	/* vcc disable */
	{DTYPE_VCC_DISABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 3},
	{DTYPE_VCC_DISABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 3},
};


/*******************************************************************************
** LCD PINCTRL
*/
static struct pinctrl_data pctrl;

static struct pinctrl_cmd_desc toshiba_lcd_pinctrl_init_cmds[] = {
	{DTYPE_PINCTRL_GET, &pctrl, 0},
	{DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
	{DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc toshiba_lcd_pinctrl_normal_cmds[] = {
	{DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};

static struct pinctrl_cmd_desc toshiba_lcd_pinctrl_lowpower_cmds[] = {
	{DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc toshiba_lcd_pinctrl_finit_cmds[] = {
	{DTYPE_PINCTRL_PUT, &pctrl, 0},
};


/*******************************************************************************
** GPIO
*/
#define GPIO_LCD_RESET_NAME	"gpio_lcd_reset"
#define GPIO_LCD_P5V5_ENABLE_NAME	"gpio_lcd_p5v5_enable"

static uint32_t	gpio_lcd_reset;  /*gpio_4_5, gpio_037*/
static uint32_t	gpio_lcd_p5v5_enable;  /*gpio_5_1, gpio_041*/

static struct gpio_desc toshiba_lcd_gpio_request_cmds[] = {
	/* reset */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* AVDD_5.5V */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
};

static struct gpio_desc toshiba_lcd_gpio_free_cmds[] = {
	/* reset */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* AVDD_5.5V */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
};

static struct gpio_desc toshiba_lcd_gpio_normal_cmds[] = {
	/* AVDD_5.5V*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 1},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
};

static struct gpio_desc toshiba_lcd_gpio_lowpower_cmds[] = {
	/* AVDD_5.5V*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* AVDD_5.5V input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
	/* reset input*/
	{DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};


/*******************************************************************************
**
*/
static int mipi_toshiba_panel_set_fastboot(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* lcd vcc enable */
	vcc_cmds_tx(pdev, toshiba_lcd_vcc_enable_cmds, \
		ARRAY_SIZE(toshiba_lcd_vcc_enable_cmds));

	/* lcd pinctrl normal */
	pinctrl_cmds_tx(pdev,toshiba_lcd_pinctrl_normal_cmds,
		ARRAY_SIZE(toshiba_lcd_pinctrl_normal_cmds));

	/* lcd gpio request */
	gpio_cmds_tx(toshiba_lcd_gpio_request_cmds,
		ARRAY_SIZE(toshiba_lcd_gpio_request_cmds));

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int mipi_toshiba_panel_on(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	pinfo = &(hisifd->panel_info);

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
		LOG_JANK_D(JLID_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
		g_debug_enable = true;

		/* lcd vcc enable */
		vcc_cmds_tx(NULL, toshiba_lcd_vcc_enable_cmds,
			ARRAY_SIZE(toshiba_lcd_vcc_enable_cmds));

		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE) {
		/* lcd pinctrl normal */
		pinctrl_cmds_tx(pdev, toshiba_lcd_pinctrl_normal_cmds,
			ARRAY_SIZE(toshiba_lcd_pinctrl_normal_cmds));

		/* lcd gpio request */
		gpio_cmds_tx(toshiba_lcd_gpio_request_cmds, \
			ARRAY_SIZE(toshiba_lcd_gpio_request_cmds));
		/* lcd gpio normal */
		gpio_cmds_tx(toshiba_lcd_gpio_normal_cmds, \
			ARRAY_SIZE(toshiba_lcd_gpio_normal_cmds));

		/* lcd display on sequence */
		mipi_dsi_cmds_tx(toshiba_display_on_cmd,
			ARRAY_SIZE(toshiba_display_on_cmd), hisifd->mipi_dsi0_base);

		pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_HS_SEND_SEQUENCE) {
		;
	} else {
		HISI_FB_ERR("failed to init lcd!\n");
	}

	/* backlight on */
	hisi_lcd_backlight_on(pdev);

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_toshiba_panel_off(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	LOG_JANK_D(JLID_KERNEL_LCD_POWER_OFF, "%s", "JL_KERNEL_LCD_POWER_OFF");

	/* backlight off */
	hisi_lcd_backlight_off(pdev);

	/* lcd display off sequence */
	mipi_dsi_cmds_tx(toshiba_display_off_cmds, \
		ARRAY_SIZE(toshiba_display_off_cmds), hisifd->mipi_dsi0_base);

	/* lcd gpio lowpower */
	gpio_cmds_tx(toshiba_lcd_gpio_lowpower_cmds, \
		ARRAY_SIZE(toshiba_lcd_gpio_lowpower_cmds));
	/* lcd gpio free */
	gpio_cmds_tx(toshiba_lcd_gpio_free_cmds, \
		ARRAY_SIZE(toshiba_lcd_gpio_free_cmds));

	/* lcd pinctrl lowpower */
	pinctrl_cmds_tx(pdev, toshiba_lcd_pinctrl_lowpower_cmds,
		ARRAY_SIZE(toshiba_lcd_pinctrl_lowpower_cmds));

	/* lcd vcc disable */
	vcc_cmds_tx(NULL, toshiba_lcd_vcc_disable_cmds, \
		ARRAY_SIZE(toshiba_lcd_vcc_disable_cmds));

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_toshiba_panel_remove(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	/*BUG_ON(hisifd == NULL);*/

	if (!hisifd) {
		return 0;
	}

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* lcd vcc finit */
	vcc_cmds_tx(pdev, toshiba_lcd_vcc_finit_cmds, \
		ARRAY_SIZE(toshiba_lcd_vcc_finit_cmds));

	/* lcd pinctrl finit */
	pinctrl_cmds_tx(pdev, toshiba_lcd_pinctrl_finit_cmds,
		ARRAY_SIZE(toshiba_lcd_pinctrl_finit_cmds));

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int mipi_toshiba_panel_set_backlight(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;

	char payload[3] = {0, 0, 0};
	struct dsi_cmd_desc bl_cmd[] = {
		{DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
			sizeof(payload), payload},
	};

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (hisifd->panel_info.bl_set_type & BL_SET_BY_PWM) {
		ret = hisi_pwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_BLPWM) {
		ret = hisi_blpwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_MIPI) {
		bl_cmd[0].payload[0] = 0x51;
		bl_cmd[0].payload[1] = (hisifd->bl_level >> 4) & 0xf;
		bl_cmd[0].payload[2] = hisifd->bl_level & 0xf;

		mipi_dsi_cmds_tx(bl_cmd, ARRAY_SIZE(bl_cmd), hisifd->mipi_dsi0_base);
	} else {
		;
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	if (unlikely(g_debug_enable)) {
		LOG_JANK_D(JLID_KERNEL_LCD_BACKLIGHT_ON, "JL_KERNEL_LCD_BACKLIGHT_ON,%u", hisifd->bl_level);
		g_debug_enable = false;
	}

	return ret;
}


/******************************************************************************/
static ssize_t mipi_toshiba_panel_lcd_model_show(struct platform_device *pdev,
	char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = snprintf(buf, PAGE_SIZE, "mipi_toshiba_MDY90\n");

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_toshiba_panel_lcd_cabc_mode_show(struct platform_device *pdev,
	char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_toshiba_panel_lcd_cabc_mode_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_toshiba_panel_lcd_check_reg(struct platform_device *pdev,
	char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_toshiba_panel_lcd_mipi_detect(struct platform_device *pdev,
	char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}


static struct hisi_panel_info toshiba_panel_info = {0};
static struct hisi_fb_panel_data toshiba_panel_data = {
	.panel_info = &toshiba_panel_info,
	.set_fastboot = mipi_toshiba_panel_set_fastboot,
	.on = mipi_toshiba_panel_on,
	.off = mipi_toshiba_panel_off,
	.remove = mipi_toshiba_panel_remove,
	.set_backlight = mipi_toshiba_panel_set_backlight,

	.lcd_model_show = mipi_toshiba_panel_lcd_model_show,
	.lcd_cabc_mode_show = mipi_toshiba_panel_lcd_cabc_mode_show,
	.lcd_cabc_mode_store = mipi_toshiba_panel_lcd_cabc_mode_store,
	.lcd_check_reg = mipi_toshiba_panel_lcd_check_reg,
	.lcd_mipi_detect = mipi_toshiba_panel_lcd_mipi_detect,
	.set_display_region = NULL,
	.set_display_resolution = NULL,
};


/*******************************************************************************
**
*/
static int mipi_toshiba_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_panel_info *pinfo = NULL;
	struct device_node *np = NULL;
	uint32_t bl_type = 0;

	if (hisi_fb_device_probe_defer(PANEL_MIPI_VIDEO)) {
		goto err_probe_defer;
	}

	HISI_FB_DEBUG("+.\n");

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_TOSHIBA_MDY90);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_TOSHIBA_MDY90);
		goto err_return;
	}

	gpio_lcd_reset = of_get_named_gpio(np, "gpios", 0);  /*gpio_4_5, gpio_037*/
	gpio_lcd_p5v5_enable = of_get_named_gpio(np, "gpios", 1);  /*gpio_5_1, gpio_041*/

	ret = of_property_read_u32(np, LCD_BL_TYPE_NAME, &bl_type);
	if (ret) {
		HISI_FB_ERR("get lcd_bl_type failed!\n");
		bl_type = BL_SET_BY_PWM;
	}

	pdev->id = 1;
	/* init lcd panel info */
	pinfo = toshiba_panel_data.panel_info;
	memset(pinfo, 0, sizeof(struct hisi_panel_info));
	pinfo->xres = 1080;
	pinfo->yres = 1920;
	pinfo->width = 62;
	pinfo->height = 110;
	pinfo->type = PANEL_MIPI_VIDEO;
	pinfo->orientation = LCD_PORTRAIT;
	pinfo->bpp = LCD_RGB888;
	pinfo->bgr_fmt = LCD_RGB;
	pinfo->bl_set_type = bl_type;
	pinfo->bl_min = 1;
	pinfo->bl_max = 255;

	pinfo->frc_enable = 0;
	pinfo->esd_enable = 0;

	pinfo->sbl_support = 0;
	pinfo->smart_bl.strength_limit = 128;
	pinfo->smart_bl.calibration_a = 60;
	pinfo->smart_bl.calibration_b = 95;
	pinfo->smart_bl.calibration_c = 5;
	pinfo->smart_bl.calibration_d = 1;
	pinfo->smart_bl.t_filter_control = 5;
	pinfo->smart_bl.backlight_min = 480;
	pinfo->smart_bl.backlight_max = 4096;
	pinfo->smart_bl.backlight_scale = 0xff;
	pinfo->smart_bl.ambient_light_min = 14;
	pinfo->smart_bl.filter_a = 1738;
	pinfo->smart_bl.filter_b = 6;
	pinfo->smart_bl.logo_left = 0;
	pinfo->smart_bl.logo_top = 0;

	pinfo->ldi.h_back_porch = 40;
	pinfo->ldi.h_front_porch = 90;
	pinfo->ldi.h_pulse_width = 20;
	pinfo->ldi.v_back_porch = 12;
	pinfo->ldi.v_front_porch = 14;
	pinfo->ldi.v_pulse_width = 4;

	pinfo->mipi.lane_nums = DSI_4_LANES;
	pinfo->mipi.color_mode = DSI_24BITS_1;
	pinfo->mipi.vc = 0;
	pinfo->mipi.dsi_bit_clk = 480;

	pinfo->pxl_clk_rate = 144 * 1000000UL;

    pinfo->dirty_region_info.left_align = -1;
    pinfo->dirty_region_info.right_align = -1;
    pinfo->dirty_region_info.top_align = -1;
    pinfo->dirty_region_info.bottom_align = -1;
    pinfo->dirty_region_info.w_align = -1;
    pinfo->dirty_region_info.h_align = -1;
    pinfo->dirty_region_info.w_min = -1;
    pinfo->dirty_region_info.h_min = -1;
    pinfo->dirty_region_info.top_start = -1;
    pinfo->dirty_region_info.bottom_start = -1;

	/* lcd vcc init */
	ret = vcc_cmds_tx(pdev, toshiba_lcd_vcc_init_cmds,
		ARRAY_SIZE(toshiba_lcd_vcc_init_cmds));
	if (ret != 0) {
		HISI_FB_ERR("LCD vcc init failed!\n");
		goto err_return;
	}

	/* lcd pinctrl init */
	ret = pinctrl_cmds_tx(pdev, toshiba_lcd_pinctrl_init_cmds,
		ARRAY_SIZE(toshiba_lcd_pinctrl_init_cmds));
	if (ret != 0) {
		HISI_FB_ERR("lcd iomux init failed!\n");
		goto err_return;
	}

	/* alloc panel device data */
	ret = platform_device_add_data(pdev, &toshiba_panel_data,
		sizeof(struct hisi_fb_panel_data));
	if (ret) {
		HISI_FB_ERR("platform_device_add_data failed!\n");
		goto err_device_put;
	}

	hisi_fb_add_device(pdev);

	HISI_FB_DEBUG("-.\n");

	return 0;

err_device_put:
	platform_device_put(pdev);
err_return:
	return ret;
err_probe_defer:
	return -EPROBE_DEFER;
}

static const struct of_device_id hisi_panel_match_table[] = {
    {
        .compatible = DTS_COMP_TOSHIBA_MDY90,
        .data = NULL,
    },
    {},
};

MODULE_DEVICE_TABLE(of, hisi_panel_match_table);

static struct platform_driver this_driver = {
	.probe = mipi_toshiba_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = "mipi_toshiba_MDY90",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hisi_panel_match_table),
	},
};

static int __init mipi_toshiba_panel_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(mipi_toshiba_panel_init);
