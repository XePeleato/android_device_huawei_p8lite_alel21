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
#include <linux/huawei/hisi_adc.h>
#include <linux/hisi/hw_cmdline_parse.h>
#include "mipi_auo_NT51021.h"

#define DTS_COMP_AUO_NT51021 "hisilicon,mipi_auo_NT51021"
#define LCD_VDDIO_TYPE_NAME	"lcd-vddio-type"

#define CABC_OFF 0
#define CABC_UI_MODE 1
#define CABC_STILL_MODE 2
#define CABC_MOVING_MODE 3
#define CHECKSUM_SIZE 8
static struct hisi_fb_panel_data auo_panel_data;

static int g_lcd_type = 2; //g_lcd_type = 1(AUO);  g_lcd_type = 0(truly)  else unkown;

static int g_cabc_mode = 2;//0:off  2:still   3:moving

static int hkadc_buf = 0;
static bool checksum_enable_ctl = false;

extern int fastboot_set_needed;


/*******************************************************************************
** Power ON Sequence(sleep mode to Normal mode)
*/

/*******************************************************************************
** setting PWM frequency to 58KHZ
*/

/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/

/*******************************************************************************
** LCD VCC
*/
#define VCC_LCDIO_NAME		"lcdio-vcc"
#define VCC_LCDANALOG_NAME	"lcdanalog-vcc"

//static struct regulator *vcc_lcdio;
//static struct regulator *vcc_lcdanalog;

static struct vcc_desc auo_lcd_vcc_init_cmds[] = {
	/* vcc get */
	/* vcc set voltage */
};

static struct vcc_desc auo_lcd_vcc_finit_cmds[] = {
	/* vcc put */
};

static struct vcc_desc auo_lcd_vcc_enable_cmds[] = {
	/* vcc enable */
};

static struct vcc_desc auo_lcd_vcc_disable_cmds[] = {
	/* vcc disable */
};

/*******************************************************************************
** LCD IOMUX
*/
static struct pinctrl_data pctrl;

static struct pinctrl_cmd_desc auo_lcd_pinctrl_init_cmds[] = {
	{DTYPE_PINCTRL_GET, &pctrl, 0},
	{DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
	{DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc auo_lcd_pinctrl_normal_cmds[] = {
	{DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};

static struct pinctrl_cmd_desc auo_lcd_pinctrl_lowpower_cmds[] = {
	{DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc auo_lcd_pinctrl_finit_cmds[] = {
	{DTYPE_PINCTRL_PUT, &pctrl, 0},
};

/*******************************************************************************
** LCD GPIO
*/
#define GPIO_LCD_BL_ENABLE_NAME	"gpio_lcd_bl_enable"
#define GPIO_LCD_VCC_NAME	"gpio_lcd_vcc"
#define GPIO_LCD_ID0_NAME	"gpio_lcd_id0"
#define GPIO_LCD_ID1_NAME    "gpio_lcd_id1"
#define GPIO_LCD_BIAS_ENABLE_NAME	"gpio_lcd_bias_enable"

static uint32_t gpio_lcd_bl_enable;  /*gpio_4_3, gpio_035*/
static uint32_t gpio_lcd_vcc;  /*gpio_19_5, gpio_157*/
static uint32_t gpio_lcd_id0;  /*gpio_4_6, gpio_038*/
static uint32_t gpio_lcd_id1;  /*gpio_4_7, gpio_039*/
static uint32_t gpio_lcd_bias_enable;  /*gpio_5_1, gpio_041*/

static uint32_t g_vddio_type = 0;


static struct gpio_desc auo_lcd_gpio_request_cmds[] = {
	/* VCC */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_VCC_NAME, &gpio_lcd_vcc, 0},
	/* backlight enable */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* lcd id */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID0_NAME, &gpio_lcd_id0, 0},
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID1_NAME, &gpio_lcd_id1, 0},
	/* AVDD  */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_BIAS_ENABLE_NAME, &gpio_lcd_bias_enable, 0},
};

static struct gpio_desc auo_lcd_gpio_free_cmds[] = {
	/* VCC */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_VCC_NAME, &gpio_lcd_vcc, 0},
	/* backlight enable */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* lcd id */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID0_NAME, &gpio_lcd_id0, 0},
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID1_NAME, &gpio_lcd_id1, 0},
	/* AVDD */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_BIAS_ENABLE_NAME, &gpio_lcd_bias_enable, 0},
};

static struct gpio_desc auo_lcd_gpio_normal_cmds[] = {
	/* vcc */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20,
		GPIO_LCD_VCC_NAME, &gpio_lcd_vcc, 1},
	/* AVDD*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_BIAS_ENABLE_NAME, &gpio_lcd_bias_enable, 1},
	/* backlight enable */
    //{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
    //	GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 1},
	/* lcd id */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_ID0_NAME, &gpio_lcd_id0, 0},
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_ID1_NAME, &gpio_lcd_id1, 0},
};

static struct gpio_desc auo_lcd_enable_bl_cmds[] = {
	/* backlight enable */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 1},
};

static struct gpio_desc auo_lcd_disable_bl_cmds[] = {
	/* backlight enable */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
};

static struct gpio_desc auo_lcd_gpio_lowpower_cmds[] = {
	/* backlight enable */
    //{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
    //	GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* AVDDV*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_BIAS_ENABLE_NAME, &gpio_lcd_bias_enable, 0},
	/* vcc */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 100,
		GPIO_LCD_VCC_NAME, &gpio_lcd_vcc, 0},
	/* backlight enable input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* AVDD input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_BIAS_ENABLE_NAME, &gpio_lcd_bias_enable, 0},
	/* vcc input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_LCD_VCC_NAME, &gpio_lcd_vcc, 0},
};

#define AUO_ESD_TEST 0
#define AUO_SBL_TEST 0

#define AUO_CE_TEST 1
#if AUO_CE_TEST
static int g_ce_mode = 0;
static struct hisi_fb_data_type *hisifd_test = NULL;
static ssize_t get_ce_mode(struct device *dev,
    struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 16,"%d\n", g_ce_mode);
}

static ssize_t set_ce_mode(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;
	unsigned long val = 0;

	struct dsi_cmd_desc ce_enable_cmd[] = {
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step0), select_page3_step0},
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step1), select_page3_step1},
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(ce_enable), ce_enable},
	};

	struct dsi_cmd_desc ce_disable_cmd[] = {
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step0), select_page3_step0},
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step1), select_page3_step1},
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(ce_disable), ce_disable},
	};

	ret = strict_strtoul(buf, 0, &val);
	if (ret)
	    return ret;

	if(0 == val) {
		HISI_FB_INFO("fb%d, CE -> disable !\n", hisifd_test->index);
		mipi_dsi_cmds_tx(ce_disable_cmd, ARRAY_SIZE(ce_disable_cmd),hisifd_test->mipi_dsi0_base);
		mdelay(5);
	}else if (1 == val){
		HISI_FB_INFO("fb%d, CE -> enable !\n", hisifd_test->index);
		mipi_dsi_cmds_tx(ce_enable_cmd, ARRAY_SIZE(ce_enable_cmd),hisifd_test->mipi_dsi0_base);
		mdelay(5);
	}else{
		val = g_ce_mode;
		HISI_FB_INFO("fb%d, CE -> no this mode !\n", hisifd_test->index);
	}

	g_ce_mode = val;

	return snprintf((char *)buf, 16,"%d\n", g_ce_mode);
}


static DEVICE_ATTR(color_enhance_mode, 0664, get_ce_mode, set_ce_mode);

static struct attribute *jdi_attrs[] = {
	&dev_attr_color_enhance_mode.attr,
	NULL,
};

static struct attribute_group jdi_attr_group = {
	.attrs = jdi_attrs,
};

static int jdi_sysfs_init(struct platform_device *pdev)
{
	int ret = 0;
	ret = sysfs_create_group(&pdev->dev.kobj, &jdi_attr_group);
	if (ret) {
		HISI_FB_ERR("create sysfs file failed!\n");
		return ret;
	}
	return 0;
}

static void jdi_sysfs_deinit(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &jdi_attr_group);
}
#endif

/*******************************************************************************
**
*/
static int mipi_auo_panel_set_fastboot(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* lcd pinctrl normal */
	pinctrl_cmds_tx(pdev, auo_lcd_pinctrl_normal_cmds,
		ARRAY_SIZE(auo_lcd_pinctrl_normal_cmds));

	/* lcd gpio request */
	gpio_cmds_tx(auo_lcd_gpio_request_cmds,
		ARRAY_SIZE(auo_lcd_gpio_request_cmds));

	/* backlight on */
	hisi_lcd_backlight_on(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int mipi_auo_panel_on(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	//char __iomem *mipi_dsi0_base = NULL;
#if defined (CONFIG_HUAWEI_DSM)
//	static struct lcd_reg_read_t lcd_status_reg[] = {
//		{0x0A, 0x9C, 0xFF, "lcd power state"},
//		{0x0E, 0x80, 0xC1, "lcd signal mode"},
//		{0x05, 0x00, 0xFF, "mipi dsi error number"},
//		{0xDA, 0x00, 0x00, "RDID1"},
//	};
#endif

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	pinfo = &(hisifd->panel_info);
//	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
		/* lcd vcc enable */
		/* lcd pinctrl normal */
		pinctrl_cmds_tx(pdev, auo_lcd_pinctrl_normal_cmds,
			ARRAY_SIZE(auo_lcd_pinctrl_normal_cmds));

		/* lcd gpio request */
		gpio_cmds_tx(auo_lcd_gpio_request_cmds, \
			ARRAY_SIZE(auo_lcd_gpio_request_cmds));

		/* lcd gpio normal */
		gpio_cmds_tx(auo_lcd_gpio_normal_cmds, \
			ARRAY_SIZE(auo_lcd_gpio_normal_cmds));

		vcc_cmds_tx(pdev, auo_lcd_vcc_enable_cmds,
			ARRAY_SIZE(auo_lcd_vcc_enable_cmds));
		//if (g_vddio_type)
		//	ts_power_gpio_enable();

		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE) {
		//MIPI LP mode, IC noise for MIPI
		mipi_dsi_cmds_tx(shift_to_mipi_cmd, \
        			ARRAY_SIZE(shift_to_mipi_cmd), hisifd->mipi_dsi0_base);
		mipi_dsi_cmds_tx(elusion_high_bl, \
				ARRAY_SIZE(elusion_high_bl), hisifd->mipi_dsi0_base);
		
		/*g_lcd_type = 01(AUO);  g_lcd_type = 00(truly);*/
		if (1 == g_lcd_type){
			mipi_dsi_cmds_tx(auo_cabc_initial_cmd, \
				ARRAY_SIZE(auo_cabc_initial_cmd), hisifd->mipi_dsi0_base);
			mipi_dsi_cmds_tx(auo_ce_initial_cmd, \
				ARRAY_SIZE(auo_ce_initial_cmd), hisifd->mipi_dsi0_base);
			mipi_dsi_cmds_tx(auo_display_on_cmds, \
				ARRAY_SIZE(auo_display_on_cmds), hisifd->mipi_dsi0_base);
			g_ce_mode = 1;
			
		}else{
			mipi_dsi_cmds_tx(truly_cabc_initial_cmd, \
				ARRAY_SIZE(truly_cabc_initial_cmd), hisifd->mipi_dsi0_base);
			mipi_dsi_cmds_tx(truly_ce_initial_cmd, \
				ARRAY_SIZE(truly_ce_initial_cmd), hisifd->mipi_dsi0_base);
			mipi_dsi_cmds_tx(truly_gamma_initial_cmd, \
				ARRAY_SIZE(truly_gamma_initial_cmd), hisifd->mipi_dsi0_base);
			mdelay(100);
			mipi_dsi_cmds_tx(truly_display_on_cmds, \
				ARRAY_SIZE(truly_display_on_cmds), hisifd->mipi_dsi0_base);
			g_ce_mode = 0;
		}
		
		g_cabc_mode = 2;

#if defined (CONFIG_HUAWEI_DSM)
/*		panel_check_status_and_report_by_dsm(lcd_status_reg, \
			ARRAY_SIZE(lcd_status_reg), mipi_dsi0_base);*/
#endif
		pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_HS_SEND_SEQUENCE) {
		/* backlight on */
		hisi_lcd_backlight_on(pdev);
		
	} else {
		HISI_FB_ERR("failed to init lcd!\n");
	}

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_auo_panel_off(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	/* backlight off */
	hisi_lcd_backlight_off(pdev);
	gpio_cmds_tx(auo_lcd_disable_bl_cmds, \
			ARRAY_SIZE(auo_lcd_disable_bl_cmds));
	mdelay(200);

	/* lcd display off sequence */
       mipi_dsi_cmds_tx(sleep_in_cmds, \
           	ARRAY_SIZE(sleep_in_cmds), hisifd->mipi_dsi0_base);
	/* lcd gpio lowpower */
	gpio_cmds_tx(auo_lcd_gpio_lowpower_cmds, \
		ARRAY_SIZE(auo_lcd_gpio_lowpower_cmds));
	/* lcd gpio free */
	gpio_cmds_tx(auo_lcd_gpio_free_cmds, \
		ARRAY_SIZE(auo_lcd_gpio_free_cmds));

	/* lcd pinctrl lowpower */
	pinctrl_cmds_tx(pdev, auo_lcd_pinctrl_lowpower_cmds,
		ARRAY_SIZE(auo_lcd_pinctrl_lowpower_cmds));

	//if (g_vddio_type)
		//ts_power_gpio_disable();
	mdelay(500);
	/* lcd vcc disable */
	vcc_cmds_tx(pdev, auo_lcd_vcc_disable_cmds,
		ARRAY_SIZE(auo_lcd_vcc_disable_cmds));

	if (hisifd->hisi_fb_shutdown) {
		//ts_power_control_notify(TS_IC_SHUT_DOWN, SHORT_SYNC_TIMEOUT);
	}
	checksum_enable_ctl = false;

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_auo_panel_remove(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);

	if (!hisifd) {
		return 0;
	}

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* lcd vcc finit */
	vcc_cmds_tx(pdev, auo_lcd_vcc_finit_cmds,
		ARRAY_SIZE(auo_lcd_vcc_finit_cmds));

	/* lcd pinctrl finit */
	pinctrl_cmds_tx(pdev, auo_lcd_pinctrl_finit_cmds,
		ARRAY_SIZE(auo_lcd_pinctrl_finit_cmds));

#if AUO_CE_TEST
	jdi_sysfs_deinit(pdev);
#endif

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int mipi_auo_panel_set_backlight(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	u32 level = 0;
	int ret = 0;
	int bl_gpio_status = 0;

       char led_page0[2] = {0x83, 0x00};       /* DTYPE_DCS_WRITE1 */
       char led_page1[2] = {0x84, 0x00};       /* DTYPE_DCS_WRITE1 */
       char bl_level_adjust[2] = {0x9f, 0x00}; /* DTYPE_DCS_WRITE1 */
	struct dsi_cmd_desc bl_cmd[] = {
		{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
			sizeof(led_page0), led_page0},
		{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
			sizeof(led_page1), led_page1},		
		{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
			sizeof(bl_level_adjust), bl_level_adjust},		
	};

	char still_dimming[2] = {0x95,0x60};
	char moving_dimming[2] = {0x94, 0x78};
	struct dsi_cmd_desc dimming_initial_cmd[] = {
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(still_dimming), still_dimming},
		{DTYPE_DCS_WRITE1, 0,1, WAIT_TYPE_MS, sizeof(moving_dimming), moving_dimming},
	};
	
	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	level = hisifd->bl_level;
	bl_level_adjust[1] = level;

	if (hisifd->panel_info.bl_set_type & BL_SET_BY_PWM) {
		ret = hisi_pwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_BLPWM) {
		ret = hisi_blpwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_MIPI) {
		bl_level_adjust[1] = hisifd->bl_level;
		if(hisifd->backlight.bl_level_old != bl_level_adjust[1] ){
			HISI_FB_INFO("set bl = %d !\n", bl_level_adjust[1]);

			bl_gpio_status = gpio_get_value(gpio_lcd_bl_enable);
			if (!bl_gpio_status){
				mdelay(70);
				//reset dimming
				mipi_dsi_cmds_tx(dimming_initial_cmd, ARRAY_SIZE(dimming_initial_cmd), hisifd->mipi_dsi0_base);
				mipi_dsi_cmds_tx(bl_cmd, ARRAY_SIZE(bl_cmd), hisifd->mipi_dsi0_base);
				gpio_cmds_tx(auo_lcd_enable_bl_cmds, ARRAY_SIZE(auo_lcd_enable_bl_cmds));
			}else{
				mipi_dsi_cmds_tx(bl_cmd, ARRAY_SIZE(bl_cmd), hisifd->mipi_dsi0_base);
			}
		}
		
	} else {
		HISI_FB_ERR("No such bl_set_type!\n");
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}


/******************************************************************************/
static ssize_t mipi_auo_panel_lcd_model_show(struct platform_device *pdev,
	char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (1 == g_lcd_type){
		ret = snprintf(buf, PAGE_SIZE, "auo_NT51021 8.0' VIDEO TFT 1200 x 1920\n");
	}else if (0 == g_lcd_type){
		ret = snprintf(buf, PAGE_SIZE, "truly_NT51021 8.0' VIDEO TFT 1200 x 1920\n");
	}else{
		ret = snprintf(buf, PAGE_SIZE, "error, no this panel, check the panel !\n");
	}
	
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_auo_panel_lcd_cabc_mode_show(struct platform_device *pdev,
	char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = snprintf(buf, PAGE_SIZE, "%d\n", g_cabc_mode);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_auo_panel_lcd_cabc_mode_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	int ret = 0;
	unsigned long val = 0;
	int flag=-1;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	mipi_dsi0_base =hisifd->mipi_dsi0_base;

	ret = strict_strtoul(buf, 0, &val);
	if (ret)
               return ret;

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	flag=(int)val;
	if (flag==CABC_OFF ){
              g_cabc_mode=0;
		if (1 == g_lcd_type){
			mipi_dsi_cmds_tx(auo_cabc_off_cmds,  ARRAY_SIZE(auo_cabc_off_cmds), mipi_dsi0_base);
			
		}else{
			mipi_dsi_cmds_tx(truly_cabc_off_cmds,  ARRAY_SIZE(truly_cabc_off_cmds), mipi_dsi0_base);
		}
	} else if (flag==CABC_UI_MODE ){
              g_cabc_mode=CABC_UI_MODE;
              if (1 == g_lcd_type){
			mipi_dsi_cmds_tx(auo_cabc_still_on_cmds,  ARRAY_SIZE(auo_cabc_still_on_cmds), mipi_dsi0_base);
			
		}else{
			mipi_dsi_cmds_tx(truly_cabc_still_on_cmds,  ARRAY_SIZE(truly_cabc_still_on_cmds), mipi_dsi0_base);
		}
	}else if (flag==CABC_STILL_MODE ){
              g_cabc_mode=CABC_STILL_MODE;
              if (1 == g_lcd_type){
			mipi_dsi_cmds_tx(auo_cabc_still_on_cmds,  ARRAY_SIZE(auo_cabc_still_on_cmds), mipi_dsi0_base);
			
		}else{
			mipi_dsi_cmds_tx(truly_cabc_still_on_cmds,  ARRAY_SIZE(truly_cabc_still_on_cmds), mipi_dsi0_base);
		}
	}else if (flag==CABC_MOVING_MODE ){
              g_cabc_mode=CABC_MOVING_MODE;
              if (1 == g_lcd_type){
			mipi_dsi_cmds_tx(auo_cabc_moving_on_cmds,  ARRAY_SIZE(auo_cabc_moving_on_cmds), mipi_dsi0_base);
			
		}else{
			mipi_dsi_cmds_tx(truly_cabc_moving_on_cmds,  ARRAY_SIZE(truly_cabc_moving_on_cmds), mipi_dsi0_base);
		}
	}else{
		HISI_FB_DEBUG("cabc no this mode!\n");
	};
	
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
	
	return snprintf((char *)buf, count, "%d\n", g_cabc_mode);
}

static ssize_t mipi_auo_panel_lcd_check_reg_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
#if 0
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	u32 read_value[4] = {0};
	u32 expected_value[4] = {0x9c, 0x00, 0x77, 0x00};
	u32 read_mask[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	char* reg_name[4] = {"power mode", "MADCTR", "pixel format", "image mode"};
	char lcd_reg_0a[] = {0x0a};
	char lcd_reg_0b[] = {0x0b};
	char lcd_reg_0c[] = {0x0c};
	char lcd_reg_0d[] = {0x0d};

	struct dsi_cmd_desc lcd_check_reg[] = {
		{DTYPE_DCS_READ, 0, 10, WAIT_TYPE_US,
			sizeof(lcd_reg_0a), lcd_reg_0a},
		{DTYPE_DCS_READ, 0, 10, WAIT_TYPE_US,
			sizeof(lcd_reg_0b), lcd_reg_0b},
		{DTYPE_DCS_READ, 0, 10, WAIT_TYPE_US,
			sizeof(lcd_reg_0c), lcd_reg_0c},
		{DTYPE_DCS_READ, 0, 10, WAIT_TYPE_US,
			sizeof(lcd_reg_0d), lcd_reg_0d},
	};

	struct mipi_dsi_read_compare_data data = {
		.read_value = read_value,
		.expected_value = expected_value,
		.read_mask = read_mask,
		.reg_name = reg_name,
		.log_on = 1,
		.cmds = lcd_check_reg,
		.cnt = ARRAY_SIZE(lcd_check_reg),
	};

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	if (!mipi_dsi_read_compare(&data, mipi_dsi0_base)) {
		ret = snprintf(buf, PAGE_SIZE, "OK\n");
	} else {
		ret = snprintf(buf, PAGE_SIZE, "ERROR\n");
	}
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
#endif
	return ret;
}

static ssize_t mipi_auo_panel_lcd_mipi_detect_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	u32 read_value[2] = {0};
	u32 expected_value[2] = {0x80, 0x00};
	u32 read_mask[2] = {0xFF, 0xFF};
	char* reg_name[2] = {"signal mode", "dsi error number"};
	char lcd_reg_0e[] = {0x0e};
	char lcd_reg_05[] = {0x05};

	struct dsi_cmd_desc lcd_check_reg[] = {
		{DTYPE_DCS_READ, 0, 10, WAIT_TYPE_US,
			sizeof(lcd_reg_0e), lcd_reg_0e},
		{DTYPE_DCS_READ, 0, 10, WAIT_TYPE_US,
			sizeof(lcd_reg_05), lcd_reg_05},
	};

	struct mipi_dsi_read_compare_data data = {
		.read_value = read_value,
		.expected_value = expected_value,
		.read_mask = read_mask,
		.reg_name = reg_name,
		.log_on = 1,
		.cmds = lcd_check_reg,
		.cnt = ARRAY_SIZE(lcd_check_reg),
	};

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
    //if (!mipi_dsi_read_compare(&data, mipi_dsi0_base)) {
    //	ret = snprintf(buf, PAGE_SIZE, "OK\n");
    //} else {
    //	ret = snprintf(buf, PAGE_SIZE, "ERROR\n");
    //}
	ret = snprintf(buf, PAGE_SIZE, "OK\n");
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_auo_panel_lcd_hkadc_debug_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = snprintf(buf, PAGE_SIZE, "%d\n", hkadc_buf*4);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_auo_panel_lcd_hkadc_debug_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	int ret = 0;
	int channel = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = sscanf(buf, "%u", &channel);
	if(ret <= 0) {
		HISI_FB_ERR("Sscanf return invalid, ret = %d\n", ret);
		return count;
	}

	hkadc_buf = hisi_adc_get_value(channel);
	HISI_FB_INFO("channel[%d] value is %d\n", channel, hkadc_buf);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return count;
}

static struct hisi_panel_info auo_panel_info = {0};
static struct hisi_fb_panel_data auo_panel_data = {
	.panel_info = &auo_panel_info,
	.set_fastboot = mipi_auo_panel_set_fastboot,
	.on = mipi_auo_panel_on,
	.off = mipi_auo_panel_off,
	.remove = mipi_auo_panel_remove,
	.set_backlight = mipi_auo_panel_set_backlight,

	.lcd_model_show = mipi_auo_panel_lcd_model_show,
	.lcd_cabc_mode_show = mipi_auo_panel_lcd_cabc_mode_show,
	.lcd_cabc_mode_store = mipi_auo_panel_lcd_cabc_mode_store,
	.lcd_check_reg = mipi_auo_panel_lcd_check_reg_show,
	.lcd_mipi_detect = mipi_auo_panel_lcd_mipi_detect_show,
	.lcd_hkadc_debug_show = mipi_auo_panel_lcd_hkadc_debug_show,
	.lcd_hkadc_debug_store = mipi_auo_panel_lcd_hkadc_debug_store,
	.lcd_gram_check_show = NULL,
	.lcd_gram_check_store = NULL,
	.lcd_bist_check = NULL,
	.set_display_region = NULL,
	.set_display_resolution = NULL,
};


/*******************************************************************************
**
*/
static int mipi_auo_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_panel_info *pinfo = NULL;
	struct device_node *np = NULL;
	uint32_t bl_type = 0;
#if AUO_CE_TEST
	struct platform_device *this_dev = NULL;
#endif
	int id0 = 0;
	int id1 = 0;
	int nflag0 = 0;
	int nflag1 = 0;

	if (hisi_fb_device_probe_defer(PANEL_MIPI_CMD))
		goto err_probe_defer;

	HISI_FB_DEBUG("+.\n");

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_AUO_NT51021);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_AUO_NT51021);
		goto err_return;
	}
	
	gpio_lcd_vcc = of_get_named_gpio(np, "gpios", 0);  /*gpio_19_5, gpio_157*/
	gpio_lcd_bl_enable = of_get_named_gpio(np, "gpios", 1);  /*gpio_4_3, gpio_035*/
	gpio_lcd_bias_enable = of_get_named_gpio(np, "gpios", 2);  /*gpio_5_1, gpio_041*/
	gpio_lcd_id0 = of_get_named_gpio(np, "gpios", 3);  /*gpio_4_6, gpio_038*/
	gpio_lcd_id1 = of_get_named_gpio(np, "gpios", 4);  /*gpio_4_7, gpio_039*/
	
	HISI_FB_INFO("gpio bl_enable-vcc--bias_enable--id0--id1: %d--%d--%d-- %d-- %d!\n",\
		               gpio_lcd_bl_enable,gpio_lcd_vcc,gpio_lcd_bias_enable, gpio_lcd_id0, gpio_lcd_id1);

	ret = of_property_read_u32(np, LCD_BL_TYPE_NAME, &bl_type);
	if (ret) {
		HISI_FB_ERR("get lcd_bl_type failed!\n");
		bl_type = BL_SET_BY_MIPI;
	}

	ret = of_property_read_u32(np, LCD_VDDIO_TYPE_NAME, &g_vddio_type);
	if (ret) {
		HISI_FB_ERR("get lcd_vddio_type failed!\n");
		g_vddio_type = 0;
	}
	HISI_FB_INFO("get lcd_vddio_type: %d\n", g_vddio_type);

	pdev->id = 1;
	/* init lcd panel info */
	pinfo = auo_panel_data.panel_info;
	memset(pinfo, 0, sizeof(struct hisi_panel_info));
	pinfo->xres = 1200;
	pinfo->yres = 1920;
	pinfo->width  = 107;  //mm
	pinfo->height = 172; //mm
	pinfo->type = PANEL_MIPI_VIDEO;
	pinfo->orientation = LCD_PORTRAIT;
	pinfo->bpp = LCD_RGB888;
	pinfo->bgr_fmt = LCD_RGB;
	pinfo->bl_set_type = bl_type;

	pinfo->bl_min = 1;
	pinfo->bl_max = 255;
	pinfo->vsync_ctrl_type = 0;

	pinfo->frc_enable = 0;
	pinfo->esd_enable = 0;
	pinfo->dirty_region_updt_support = 0;

	if(runmode_is_factory()) {
		HISI_FB_INFO("sbl_support = 0\n");
		pinfo->sbl_support = 0;
	} else {
		HISI_FB_INFO("sbl_support = 1\n");
		pinfo->sbl_support = 1;
	}

	pinfo->color_temperature_support = 1;
	pinfo->comform_mode_support = 1;
	pinfo->starlight_mode_support = 1;
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

	pinfo->ldi.h_back_porch = 62;
	pinfo->ldi.h_front_porch = 80;
	pinfo->ldi.h_pulse_width = 2;
	pinfo->ldi.v_back_porch = 24;
	pinfo->ldi.v_front_porch = 38;
	pinfo->ldi.v_pulse_width = 2;
	
	pinfo->mipi.lane_nums = DSI_4_LANES;
	pinfo->mipi.color_mode = DSI_24BITS_1;
	pinfo->mipi.vc = 0;
	pinfo->mipi.dsi_bit_clk = 481;
	pinfo->pxl_clk_rate = 160*1000000UL;

	/* lcd vcc init */
	ret = vcc_cmds_tx(pdev, auo_lcd_vcc_init_cmds,
		ARRAY_SIZE(auo_lcd_vcc_init_cmds));
	if (ret != 0) {
		HISI_FB_ERR("LCD vcc init failed!\n");
		goto err_return;
	}

	if (fastboot_set_needed) {
		vcc_cmds_tx(pdev, auo_lcd_vcc_enable_cmds,
			ARRAY_SIZE(auo_lcd_vcc_enable_cmds));
	}

	/* lcd pinctrl init */
	ret = pinctrl_cmds_tx(pdev, auo_lcd_pinctrl_init_cmds,
		ARRAY_SIZE(auo_lcd_pinctrl_init_cmds));
	if (ret != 0) {
	        HISI_FB_ERR("Init pinctrl failed, defer\n");
		goto err_return;
	}

	/* alloc panel device data */
	ret = platform_device_add_data(pdev, &auo_panel_data,
		sizeof(struct hisi_fb_panel_data));
	if (ret) {
		HISI_FB_ERR("platform_device_add_data failed!\n");
		goto err_device_put;
	}

	

#if AUO_CE_TEST
	this_dev = hisi_fb_add_device(pdev);
	BUG_ON(this_dev == NULL);
	hisifd_test = platform_get_drvdata(this_dev);
	BUG_ON(hisifd_test == NULL);
	//HISI_FB_INFO("fb%d, dss-dsi0 = 0x%x.\n", hisifd_test->index,hisifd_test->mipi_dsi0_base);
	jdi_sysfs_init(pdev);
#else
	hisi_fb_add_device(pdev);
#endif

	if (gpio_request(gpio_lcd_id1, GPIO_LCD_ID1_NAME) != 0) {
		HISI_FB_ERR("failed to gpio_request, lable=%s, gpio=%d!\n",
					GPIO_LCD_ID1_NAME, gpio_lcd_id1);
        	/*failed to gpio request, so need not to free*/
		nflag1 = 1;
	}
	if (gpio_request(gpio_lcd_id0, GPIO_LCD_ID0_NAME) != 0) {
		HISI_FB_ERR("failed to gpio_request, lable=%s, gpio=%d!\n",
					GPIO_LCD_ID0_NAME, gpio_lcd_id0);
        	/*failed to gpio request, so need not to free*/
		nflag0 = 1;
	}
	
	id1 = gpio_get_value(gpio_lcd_id1);
	id0 = gpio_get_value(gpio_lcd_id0);

	if (!nflag1){
		gpio_free(gpio_lcd_id1);
	}
	if (!nflag0){
		gpio_free(gpio_lcd_id0);
	}

	/*id0:id1 = 01(AUO);  id0:id1 = 00(truly);*/
	if (0 == id0 && 1 == id1){
		g_lcd_type  = 1; //AUO
		g_ce_mode = 1;
		HISI_FB_INFO("auo_NT51021 8.0' VIDEO TFT 1200 x 1920\n");
	}else if (0 == id0 && 0 == id1){
		g_lcd_type  = 0; //truly
		HISI_FB_INFO("truly_NT51021 8.0' VIDEO TFT 1200 x 1920\n");
	}else{
		g_lcd_type = 2;
		HISI_FB_INFO("error, no this panel, check the panel !\n");
	}

	HISI_FB_DEBUG("-.\n");


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
        .compatible = DTS_COMP_AUO_NT51021,
        .data = NULL,
    },
    {},
};
MODULE_DEVICE_TABLE(of, hisi_panel_match_table);

static struct platform_driver this_driver = {
	.probe = mipi_auo_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = "mipi_auo_NT51021",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hisi_panel_match_table),
	},
};

static int __init mipi_auo_panel_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(mipi_auo_panel_init);

