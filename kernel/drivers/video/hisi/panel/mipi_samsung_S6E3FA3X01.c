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
#include <linux/hisi/hw_cmdline_parse.h>
#include <huawei_platform/touthscreen/huawei_touchscreen.h>
#include <huawei_platform/log/log_jank.h>

#define DTS_COMP_SAMSUNG_S6E3FA3X01 "hisilicon,mipi_samsung_S6E3FA3X01"
#define AMOLED_CHECK_INT

static struct hisi_fb_panel_data samsung_s6e3fa3x01_panel_data;
static bool g_debug_enable = false;
extern int fastboot_set_needed;

/*******************************************************************************
** Power ON Sequence(sleep mode to Normal mode)
*/
static char caset_data[] = {
	0x2A,
	0x00,0x00,0x04, 0x37,
};

static char paset_data[] = {
	0x2B,
	0x00,0x00,0x07,0x7f,
};

static char bl_enable[] = {
	0x53,
	0x24,
};

static char tear_on[] = {
	0x35,
	0x00,
};

static char te_line[] = {
	0x44,
	0x03, 0x80,
};

static char exit_sleep[] = {
	0x11,
};

static char display_on[] = {
	0x29,
};

/*pcd setting*/
static char open_pcd_setting[] = {
	0xF0,
	0x5A, 0x5A,
};

/*normal H, when pcd happened,trigle to L, and always L*/
static char setting_pcd[] = {
	0xCC,
	0x5C, 0x51,
};

static char setting_errflag_mipi_err[] = {
	0xED,
	0x44,
};

static char close_pcd_setting[] = {
	0xF0,
	0xA5, 0xA5,
};
/*errflag_setting*/
/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/
static char display_off[] = {
	0x28,
};

static char enter_sleep[] = {
	0x10,
};

static struct dsi_cmd_desc display_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(tear_on), tear_on},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(caset_data), caset_data},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(paset_data), paset_data},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(te_line), te_line},
	{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(open_pcd_setting), open_pcd_setting},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(setting_pcd), setting_pcd},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(setting_errflag_mipi_err), setting_errflag_mipi_err},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(close_pcd_setting), close_pcd_setting},
	{DTYPE_DCS_WRITE, 0, 150, WAIT_TYPE_MS,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 0, 60, WAIT_TYPE_MS,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 0, 180, WAIT_TYPE_MS,
		sizeof(enter_sleep), enter_sleep}
};

/*******************************************************************************
** LCD VCC
*/
#define VCC_LCDIO_NAME		"lcdio-vcc"
#define VCC_LCDANALOG_NAME	"lcdanalog-vcc"

static struct regulator *vcc_io;
static struct regulator *vcc_analog;

static struct vcc_desc samsung_s6e3fa3x01_vcc_init_cmds[] = {
	/* vcc get */
	{DTYPE_VCC_GET, VCC_LCDIO_NAME, &vcc_io, 0, 0, WAIT_TYPE_MS, 0},
	{DTYPE_VCC_GET, VCC_LCDANALOG_NAME, &vcc_analog, 0, 0, WAIT_TYPE_MS, 0},

	/* vcc set voltage */
	{DTYPE_VCC_SET_VOLTAGE, VCC_LCDANALOG_NAME, &vcc_analog, 3300000, 3300000, WAIT_TYPE_MS, 0},
};

static struct vcc_desc samsung_s6e3fa3x01_vcc_finit_cmds[] = {
	/* vcc put */
	{DTYPE_VCC_PUT, VCC_LCDIO_NAME, &vcc_io, 0, 0, WAIT_TYPE_MS, 0},
	{DTYPE_VCC_PUT, VCC_LCDANALOG_NAME, &vcc_analog, 0, 0, WAIT_TYPE_MS, 0},
};

static struct vcc_desc samsung_s6e3fa3x01_vcc_enable_cmds[] = {
	/* vcc enable */
	{DTYPE_VCC_ENABLE, VCC_LCDANALOG_NAME, &vcc_analog, 0, 0, WAIT_TYPE_MS, 3},
	{DTYPE_VCC_ENABLE, VCC_LCDIO_NAME, &vcc_io, 0, 0, WAIT_TYPE_MS, 10},
};

static struct vcc_desc samsung_s6e3fa3x01_vcc_disable_cmds[] = {
	/* vcc disable */
	{DTYPE_VCC_DISABLE, VCC_LCDIO_NAME, &vcc_io, 0, 0, WAIT_TYPE_MS, 3},
	{DTYPE_VCC_DISABLE, VCC_LCDANALOG_NAME, &vcc_analog, 0, 0, WAIT_TYPE_MS, 3},
};

/*******************************************************************************
** LCD IOMUX
*/
static struct pinctrl_data pctrl;

static struct pinctrl_cmd_desc samsung_s6e3fa3x01_pinctrl_init_cmds[] = {
	{DTYPE_PINCTRL_GET, &pctrl, 0},
	{DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
	{DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc samsung_s6e3fa3x01_pinctrl_normal_cmds[] = {
	{DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};

static struct pinctrl_cmd_desc samsung_s6e3fa3x01_pinctrl_lowpower_cmds[] = {
	{DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc samsung_s6e3fa3x01_pinctrl_finit_cmds[] = {
	{DTYPE_PINCTRL_PUT, &pctrl, 0},
};

/*******************************************************************************
** GPIO
*/
#define GPIO_RESET_NAME	"gpio_amoled_reset"
#define GPIO_ID_NAME	"gpio_amoled_id"
#define GPIO_VDDIO_ENABLE_NAME "gpio_amoled_vddio_enable"
#define GPIO_PCD_NAME  "gpio_amoled_pcd"
#define GPIO_ERR_FLAG  "gpio_amoled_err_flag"

static uint32_t gpio_reset;  /*gpio_4_5, gpio_037*/
static uint32_t gpio_id;  /*gpio_4_6, gpio_038*/
static uint32_t gpio_vddio_enable; /*gpio_15_4, gpio_124*/
static uint32_t gpio_pcd; /*UL:gpio_039;CL:codec_gpio_024*/
static uint32_t gpio_err_flag; /*gpio_5_1, gpio_041*/

/***panel on**/
static struct gpio_desc samsung_s6e3fa3x01_gpio_request_cmds[] = {
	/*pcd*/
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_PCD_NAME, &gpio_pcd, 0},
	/*err flag*/
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_ERR_FLAG, &gpio_err_flag, 0},
	/* reset */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_RESET_NAME, &gpio_reset, 0},
	/* id */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_ID_NAME, &gpio_id, 0},
	/*vddio_enable*/
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_VDDIO_ENABLE_NAME, &gpio_vddio_enable, 0},
};

static struct gpio_desc samsung_s6e3fa3x01_gpio_normal_cmds[] = {
	/*vddio enable*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_VDDIO_ENABLE_NAME, &gpio_vddio_enable, 1},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_RESET_NAME, &gpio_reset, 1},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_RESET_NAME, &gpio_reset, 0},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_RESET_NAME, &gpio_reset, 1},
	/* id */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 1,
		GPIO_ID_NAME, &gpio_id, 0},
	/*pcd*/
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 1,
		GPIO_PCD_NAME, &gpio_pcd, 0},
	/*err flag*/
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 1,
		GPIO_ERR_FLAG, &gpio_err_flag, 0},
};

/***panel off***/
static struct gpio_desc samsung_s6e3fa3x01_gpio_lowpower_cmds[] = {
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_RESET_NAME, &gpio_reset, 0},
	/*vddio disable*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_VDDIO_ENABLE_NAME, &gpio_vddio_enable, 0},
	/* reset input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_RESET_NAME, &gpio_reset, 0},
	/*vddio_enable input*/
	{DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_VDDIO_ENABLE_NAME, &gpio_vddio_enable, 0},
};

static struct gpio_desc samsung_s6e3fa3x01_gpio_free_cmds[] = {
	/* reset */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_RESET_NAME, &gpio_reset, 0},
	/* id */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_ID_NAME, &gpio_id, 0},
	/*pcd*/
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_PCD_NAME, &gpio_pcd, 0},
	/*err flag*/
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_ERR_FLAG, &gpio_err_flag, 0},
	/*vddio_enable*/
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_VDDIO_ENABLE_NAME, &gpio_vddio_enable, 0},
};

#ifdef AMOLED_CHECK_INT
static irqreturn_t pcd_irq_isr_func(int irq, void *handle)
{
	if(gpio_get_value_cansleep(gpio_pcd) == 0)
		HISI_FB_INFO("pcd irq!\n");
	else
		HISI_FB_DEBUG("bad pcd irq!\n");

	return IRQ_HANDLED;
}

static irqreturn_t errflag_irq_isr_func(int irq, void *handle)
{
	if(gpio_get_value_cansleep(gpio_err_flag) == 1)
		HISI_FB_INFO("err_flag irq!\n");
	else
		HISI_FB_DEBUG("bad err_flag irq!\n");

	return IRQ_HANDLED;
}

static void amoled_irq_enable(void)
{
	enable_irq(gpio_to_irq(gpio_pcd));
	enable_irq(gpio_to_irq(gpio_err_flag));
}

static void amoled_irq_disable(void)
{
	disable_irq(gpio_to_irq(gpio_pcd));
	disable_irq(gpio_to_irq(gpio_err_flag));
}
#endif
/*******************************************************************************
**
*/
static int mipi_samsung_s6e3fa3x01_set_fastboot(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* pinctrl normal */
	pinctrl_cmds_tx(pdev, samsung_s6e3fa3x01_pinctrl_normal_cmds,
		ARRAY_SIZE(samsung_s6e3fa3x01_pinctrl_normal_cmds));

	/* gpio request */
	gpio_cmds_tx(samsung_s6e3fa3x01_gpio_request_cmds,
		ARRAY_SIZE(samsung_s6e3fa3x01_gpio_request_cmds));

	/* backlight on */	
	hisi_lcd_backlight_on(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int mipi_samsung_s6e3fa3x01_on(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	char __iomem *mipi_dsi0_base = NULL;

#if defined (CONFIG_HUAWEI_DSM)
	static struct lcd_reg_read_t lcd_status_reg[] = {
		{0x0A, 0x9C, 0xFF, "lcd power state"},
	};
#endif

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	pinfo = &(hisifd->panel_info);
	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
		LOG_JANK_D(JLID_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
		/* vcc enable */
		vcc_cmds_tx(pdev, samsung_s6e3fa3x01_vcc_enable_cmds,
			ARRAY_SIZE(samsung_s6e3fa3x01_vcc_enable_cmds));

		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE) {
		/* pinctrl normal */
		pinctrl_cmds_tx(pdev, samsung_s6e3fa3x01_pinctrl_normal_cmds,
			ARRAY_SIZE(samsung_s6e3fa3x01_pinctrl_normal_cmds));
		/* gpio request */
		gpio_cmds_tx(samsung_s6e3fa3x01_gpio_request_cmds, \
			ARRAY_SIZE(samsung_s6e3fa3x01_gpio_request_cmds));

		/* gpio normal */
		gpio_cmds_tx(samsung_s6e3fa3x01_gpio_normal_cmds, \
			ARRAY_SIZE(samsung_s6e3fa3x01_gpio_normal_cmds));

		mipi_dsi_cmds_tx(display_on_cmds, \
			ARRAY_SIZE(display_on_cmds), mipi_dsi0_base);

#if defined (CONFIG_HUAWEI_DSM)
		panel_check_status_and_report_by_dsm(lcd_status_reg, \
			ARRAY_SIZE(lcd_status_reg), mipi_dsi0_base);
#endif
		pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_HS_SEND_SEQUENCE) {
		/* backlight on */
		hisi_lcd_backlight_on(pdev);
		g_debug_enable = true;
#ifdef AMOLED_CHECK_INT
		amoled_irq_enable();
#endif
	} else {
		HISI_FB_ERR("failed to init!\n");
	}

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_samsung_s6e3fa3x01_off(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	LOG_JANK_D(JLID_KERNEL_LCD_POWER_OFF, "%s", "JL_KERNEL_LCD_POWER_OFF");

#ifdef AMOLED_CHECK_INT
	amoled_irq_disable();
#endif
	/* display off sequence */
	mipi_dsi_cmds_tx(display_off_cmds, \
		ARRAY_SIZE(display_off_cmds), hisifd->mipi_dsi0_base);

	/* gpio lowpower */
	gpio_cmds_tx(samsung_s6e3fa3x01_gpio_lowpower_cmds, \
		ARRAY_SIZE(samsung_s6e3fa3x01_gpio_lowpower_cmds));
	/* gpio free */
	gpio_cmds_tx(samsung_s6e3fa3x01_gpio_free_cmds, \
		ARRAY_SIZE(samsung_s6e3fa3x01_gpio_free_cmds));

	/* pinctrl lowpower */
	pinctrl_cmds_tx(pdev, samsung_s6e3fa3x01_pinctrl_lowpower_cmds,
		ARRAY_SIZE(samsung_s6e3fa3x01_pinctrl_lowpower_cmds));
	/* vcc disable */
	vcc_cmds_tx(pdev, samsung_s6e3fa3x01_vcc_disable_cmds,
		ARRAY_SIZE(samsung_s6e3fa3x01_vcc_disable_cmds));

	if (hisifd->hisi_fb_shutdown) {
		ts_thread_stop_notify();
	}

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_samsung_s6e3fa3x01_remove(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);

	if (!hisifd) {
		return 0;
	}

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* vcc finit */
	vcc_cmds_tx(pdev, samsung_s6e3fa3x01_vcc_finit_cmds,
		ARRAY_SIZE(samsung_s6e3fa3x01_vcc_finit_cmds));

	/* pinctrl finit */
	pinctrl_cmds_tx(pdev, samsung_s6e3fa3x01_pinctrl_finit_cmds,
		ARRAY_SIZE(samsung_s6e3fa3x01_pinctrl_finit_cmds));

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int mipi_samsung_s6e3fa3x01_set_brightness(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;

	char payload[2] = {0, 0};
	struct dsi_cmd_desc bl_cmd[] = {
		{DTYPE_DCS_WRITE1, 0, 500, WAIT_TYPE_US,
			sizeof(payload), payload},
	};

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (unlikely(g_debug_enable)) {
		HISI_FB_INFO("Set brightness to %d\n", hisifd->bl_level);
		LOG_JANK_D(JLID_KERNEL_LCD_BACKLIGHT_ON, "JL_KERNEL_LCD_BACKLIGHT_ON,%u", hisifd->bl_level);
		g_debug_enable = false;
	}

	if (hisifd->panel_info.bl_set_type & BL_SET_BY_PWM) {
		ret = hisi_pwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_BLPWM) {
		ret = hisi_blpwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_MIPI) {
		HISI_FB_DEBUG("set brightness by mipi\n");
		bl_cmd[0].payload[0] = 0x51;
		bl_cmd[0].payload[1] = hisifd->bl_level;

		mipi_dsi_cmds_tx(bl_cmd, ARRAY_SIZE(bl_cmd), hisifd->mipi_dsi0_base);
	} else {
		HISI_FB_ERR("No such bl_set_type!\n");
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

/******************************************************************************/
static ssize_t mipi_samsung_s6e3fa3x01_model_show(struct platform_device *pdev,
	char *buf)
{
	ssize_t ret = 0;

	ret = snprintf(buf, PAGE_SIZE, "Amoled-s6e3fa3x01\n");
	return ret;
}

static ssize_t mipi_samsung_pcd_errflag_check(struct platform_device *pdev,
	char *buf)
{
	int pcd_gpio = 0;
	int errflag_gpio = 0;
	ssize_t ret = 0;
	u8 result_value = 0;

	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);

	if (!hisifd) {
		HISI_FB_ERR("hisifd is null\n");
		ret = snprintf(buf, PAGE_SIZE, "%d\n", result_value);
		return ret;
	}

	if (!hisifd->panel_power_on){
		HISI_FB_INFO("panel is poweroff\n");
		ret = snprintf(buf, PAGE_SIZE, "%d\n", result_value);
		return ret;
	}

	pcd_gpio = gpio_get_value_cansleep(gpio_pcd);
	errflag_gpio = gpio_get_value_cansleep(gpio_err_flag);

	HISI_FB_INFO("pcd:%d, errflag:%d\n", pcd_gpio, errflag_gpio);

	if ((pcd_gpio == 1) && (errflag_gpio == 0)) {
		result_value = 0; // PCD_ERR_FLAG_SUCCESS
	} else if ((pcd_gpio == 0) && (errflag_gpio == 0)) {
		result_value = 1; //only  PCD_FAIL
	} else if ((pcd_gpio == 1) && (errflag_gpio == 1)) {
		result_value = 2; //only ERRFLAG FAIL
	} else if ((pcd_gpio == 0) && (errflag_gpio == 1)) {
		result_value = 3; //PCD_ERR_FLAG_FAIL
	} else
		result_value = 0;

	ret = snprintf(buf, PAGE_SIZE, "%d\n", result_value);
	return ret;
}

static int mipi_samsung_s6e3fa3x01_set_display_region(struct platform_device *pdev,
	struct dss_rect *dirty)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL || dirty == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	return 0;
}

#if 0
static ssize_t mipi_samsung_s6e3fa3x01_check_reg_show(struct platform_device *pdev, char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	char lcd_reg_CC[] = {0xCC};
	int read_value = 0;
	ssize_t ret = 0;
	char __iomem *mipi_dsi0_base = NULL;

	struct dsi_cmd_desc lcd_check_reg[] = {
		{DTYPE_DCS_READ, 0, 10, WAIT_TYPE_US,
			sizeof(lcd_reg_CC), lcd_reg_CC},
	};

	BUG_ON(pdev == NULL || dirty == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	ret = mipi_dsi_cmds_rx(&read_value, lcd_check_reg, ARRAY_SIZE(lcd_check_reg), mipi_dsi0_base);

	HISI_FB_ERR("lcd_reg_CC: %d", read_value);
	ret = snprintf(buf, PAGE_SIZE, "lcd_reg_CC: %d", read_value);

	return ret ;
}
#endif

static ssize_t mipi_samsung_s6e3fa3x01_amoled_acl_show(struct platform_device *pdev,
	char *buf)
{
	return snprintf(buf, PAGE_SIZE, "ACL show supported\n");
}

static ssize_t mipi_samsung_s6e3fa3x01_amoled_acl_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	mipi_dsi0_base =hisifd->mipi_dsi0_base;
	HISI_FB_INFO("ACL store supported\n");
	return count;
}

static ssize_t mipi_samsung_s6e3fa3x01_amoled_hbm_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	int val = 0;
	int sbl_lsensor_value = 0;
	int sbl_level = 0;
	int sbl_enable = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	mipi_dsi0_base =hisifd->mipi_dsi0_base;

	val = (int)simple_strtoul(buf, NULL, 0);
	sbl_lsensor_value = val & 0xffff;
	sbl_level = (val >> 16) & 0xff;
	sbl_enable = (val >> 24) & 0x1;

	HISI_FB_INFO("HBM store supported. sbl_lsensor_value=%d, sbl_level=%d, sbl_enable=%d\n",
		sbl_lsensor_value, sbl_level, sbl_enable);
	return count;
}

static struct hisi_panel_info samsung_s6e3fa3x01_panel_info = {0};
static struct hisi_fb_panel_data samsung_s6e3fa3x01_panel_data = {
	.panel_info = &samsung_s6e3fa3x01_panel_info,
	.set_fastboot = mipi_samsung_s6e3fa3x01_set_fastboot,
	.on = mipi_samsung_s6e3fa3x01_on,
	.off = mipi_samsung_s6e3fa3x01_off,
	.remove = mipi_samsung_s6e3fa3x01_remove,
	.set_backlight = mipi_samsung_s6e3fa3x01_set_brightness,

	.lcd_model_show = mipi_samsung_s6e3fa3x01_model_show,
	.amoled_pcd_errflag_check = mipi_samsung_pcd_errflag_check,
	.set_display_region = mipi_samsung_s6e3fa3x01_set_display_region,
//	.lcd_check_reg = mipi_samsung_s6e3fa3x01_check_reg_show,
	.amoled_acl_show = mipi_samsung_s6e3fa3x01_amoled_acl_show,
	.amoled_acl_store = mipi_samsung_s6e3fa3x01_amoled_acl_store,
	.amoled_hbm_store = mipi_samsung_s6e3fa3x01_amoled_hbm_store,
	.set_display_resolution = NULL,
};

/*******************************************************************************
**
*/
static int mipi_samsung_s6e3fa3x01_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_panel_info *pinfo = NULL;
	struct device_node *np = NULL;

	if (hisi_fb_device_probe_defer(PANEL_MIPI_CMD))
		goto err_probe_defer;

	HISI_FB_INFO("+.\n");

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_SAMSUNG_S6E3FA3X01);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_SAMSUNG_S6E3FA3X01);
		goto err_return;
	}

	gpio_reset = of_get_named_gpio(np, "gpios", 0);  /*gpio_4_5, gpio_037*/
	gpio_id = of_get_named_gpio(np, "gpios", 1);  /*gpio_3_3, gpio_027*/
	gpio_vddio_enable = of_get_named_gpio(np, "gpios", 2);  /*gpio_15_4, gpio_124*/
	gpio_pcd = of_get_named_gpio(np, "gpios", 3);	/*UL:gpio_039;CL:codec_gpio_024*/
	gpio_err_flag = of_get_named_gpio(np, "gpios", 4); /*gpio_5_1, gpio_041*/

	pdev->id = 1;
	/* init panel info */
	pinfo = samsung_s6e3fa3x01_panel_data.panel_info;
	memset(pinfo, 0, sizeof(struct hisi_panel_info));
	pinfo->xres = 1080;
	pinfo->yres = 1920;
	pinfo->width  = 65;  //mm
	pinfo->height = 115; //mm
	pinfo->type = PANEL_MIPI_CMD;
	pinfo->orientation = LCD_PORTRAIT;
	pinfo->bpp = LCD_RGB888;
	pinfo->bgr_fmt = LCD_RGB;
	pinfo->bl_set_type = BL_SET_BY_MIPI;

	pinfo->bl_min = 1;
	pinfo->bl_max = 255;
	pinfo->vsync_ctrl_type = (VSYNC_CTRL_ISR_OFF |
		VSYNC_CTRL_MIPI_ULPS);

	pinfo->frc_enable = 0;
	pinfo->esd_enable = 0;
	pinfo->dirty_region_updt_support = 0;

	if(runmode_is_factory()) {
		pinfo->sbl_support = 0;
	} else {
		pinfo->sbl_support = 1;
	}

	pinfo->color_temperature_support = 1;
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

	pinfo->ifbc_type = IFBC_TYPE_NON;

	pinfo->ldi.h_back_porch = 23;
	pinfo->ldi.h_front_porch = 50;
	pinfo->ldi.h_pulse_width = 20;
	pinfo->ldi.v_back_porch = 12;
	pinfo->ldi.v_front_porch = 14;
	pinfo->ldi.v_pulse_width = 4;

	pinfo->mipi.lane_nums = DSI_4_LANES;
	pinfo->mipi.color_mode = DSI_24BITS_1;
	pinfo->mipi.vc = 0;
	pinfo->mipi.dsi_bit_clk = 480;
	pinfo->dsi_bit_clk_upt_support = 0;
	pinfo->mipi.dsi_bit_clk_upt = pinfo->mipi.dsi_bit_clk;

	pinfo->pxl_clk_rate = 150*1000000UL;

	/* vcc init */
	ret = vcc_cmds_tx(pdev, samsung_s6e3fa3x01_vcc_init_cmds,
		ARRAY_SIZE(samsung_s6e3fa3x01_vcc_init_cmds));
	if (ret != 0) {
		HISI_FB_ERR("vcc init failed!\n");
		goto err_return;
	}

#ifdef AMOLED_CHECK_INT
	ret = request_threaded_irq(gpio_to_irq(gpio_pcd), NULL, pcd_irq_isr_func,
			IRQF_ONESHOT | IRQF_TRIGGER_FALLING,
			"pcd_irq", (void *)pdev);
	if (ret != 0) {
		HISI_FB_ERR("request_irq failed, irq_no=%d error=%d!\n", gpio_to_irq(gpio_pcd), ret);
	}

	ret = request_threaded_irq(gpio_to_irq(gpio_err_flag), NULL, errflag_irq_isr_func,
			IRQF_ONESHOT | IRQF_TRIGGER_RISING,
			"errflag_irq", (void *)pdev);
	if (ret != 0) {
		HISI_FB_ERR("request_irq failed, irq_no=%d error=%d!\n", gpio_to_irq(gpio_err_flag), ret);
	}
#endif

	if (fastboot_set_needed) {
		vcc_cmds_tx(pdev, samsung_s6e3fa3x01_vcc_enable_cmds,
			ARRAY_SIZE(samsung_s6e3fa3x01_vcc_enable_cmds));
	}

	/* pinctrl init */
	ret = pinctrl_cmds_tx(pdev, samsung_s6e3fa3x01_pinctrl_init_cmds,
			ARRAY_SIZE(samsung_s6e3fa3x01_pinctrl_init_cmds));
	if (ret != 0) {
	        HISI_FB_ERR("Init pinctrl failed, defer\n");
		goto err_return;
	}

	/* alloc panel device data */
	ret = platform_device_add_data(pdev, &samsung_s6e3fa3x01_panel_data,
		sizeof(struct hisi_fb_panel_data));
	if (ret) {
		HISI_FB_ERR("platform_device_add_data failed!\n");
		goto err_device_put;
	}

	hisi_fb_add_device(pdev);

	HISI_FB_INFO("-.\n");

	return 0;

err_device_put:
	platform_device_put(pdev);
err_return:
	return ret;
err_probe_defer:
	return -EPROBE_DEFER;

	return ret;
}

static const struct of_device_id hisi_panel_match_table[] = {
    {
        .compatible = DTS_COMP_SAMSUNG_S6E3FA3X01,
        .data = NULL,
    },
    {},
};
MODULE_DEVICE_TABLE(of, hisi_panel_match_table);

static struct platform_driver this_driver = {
	.probe = mipi_samsung_s6e3fa3x01_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = "mipi_samsung_S6E3FA3X01",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hisi_panel_match_table),
	},
};

static int __init mipi_samsung_s6e3fa3x01_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(mipi_samsung_s6e3fa3x01_init);
