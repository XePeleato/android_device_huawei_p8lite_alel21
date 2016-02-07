/* Copyright (c) 2008-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/
#include "hisi_fb.h"

#include <linux/huawei/hisi_adc.h>
#include <huawei_platform/log/log_jank.h>

#define DTS_COMP_JDI_R69429 "hisilicon,mipi_jdi_R69429"
#define LCD_VDDIO_TYPE_NAME "lcd-vddio-type"
#define CHECKSUM_SIZE 8

static volatile bool backlight_log_once = true;

static struct platform_device *hisifb_pdev  = NULL;

//cabc Macro, K3V3+ change these values
#define CABC_OFF 0
#define CABC_UI_MODE 1
#define CABC_STILL_MODE 2
#define CABC_MOVING_MODE 3

static int cabc_wanted = CABC_OFF;



static int g_IE_SRE_level = 0x80;
static int g_CABC_mode = 0x01;

static struct hisi_fb_panel_data jdi_panel_data;

static int hkadc_buf = 0;
static bool checksum_enable_ctl = false;
static bool g_debug_enable = false;

extern int fastboot_set_needed;

enum {  
	LCD_BIST_CHECK_FAIL = -1,
	LCD_BIST_CHECK_TIMEOUT = -2,
	LCD_BIST_CHECK_PASS = 0,
}; 

/*******************************************************************************
** Power ON Sequence(sleep mode to Normal mode)
*/
static char bl_level[] = {
    0x51,
    0x00,
};

static char bl_enable[] = {
    0x53,
    0x24,
};

static char te_enable[] = {
    0x35,
    0x00,
};

static char exit_sleep[] = {
    0x11,
};

static char display_on[] = {
    0x29,
};

static char bl_enable_dimming[] = {
    0x53,
    0x2C,
};
//CABC related cmds
static char back_light_control1[] = {
    0xB8,
    0x07,0x90,0x1e,0x00,0x1E,0x32,
};

static char back_light_control2[] = {
    0xB9,
    0x07,0x82,0x3c,0x00,0x3C,0x87,
};

static char back_light_control3[] = {
    0xBA,
    0x07,0x78,0x78,0x0A,0x64,0xB4,
};

static char SRE_control1_cmd[] = {
    0xBB,
    0x01,0x1e,0x14,
};

static char SRE_control2_cmd[] = {
    0xBC,
    0x01,0x50,0x32,
};

static char SRE_control3_cmd[] = {
    0xBD,
    0x01,0xB4,0xA0,
};

static char back_light_control4[] = {
    0xCE,
    0x7d,0x40,0x43,0x49,0x55,0x62,0x71,0x82,
    0x94,0xa8,0xb9,0xcb,0xdb,0xE9,0xf5,0xfc,
    0xff,0x01,0x38,0x04,0x04,0x44,0x20,         //PWM_DIV = 0x01, PWM_CYCLE=0x38  -->45khz
 };


static char basic_ce_cmd[] = {
    0xCA,
    0x01,0x80,0xa0,0xa0,0xff,0x90,0x80,0x28,
    0x30,0x3f,0x10,0x80,0x1A,0x4A,0x37,0x80,
    0x30,0xc8,0x0C,0x0C,0x09,0x07,0x08,0x08,
    0x30,0xa8,0x0e,0x12,0x20,0x18,0x20,0x18,
};


static char china_back_light_control1[] = {
    0xB8,
    0x07,0x91,0x1e,0x30,0x1E,0x32,
};

static char china_ce_cmd[] = {
    0xCA,
    0x01,0x80,0xC0,0xC0,0xC9,0xC0,0xA0,0xC0,
    0x13,0x20,0x10,0x73,0x1A,0x4A,0x37,0x80,
    0x30,0xC8,0x0C,0x0C,0x09,0x07,0x08,0x08,
    0x00,0xA8,0x0E,0x12,0x20,0x18,0x20,0x18,
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

static char lcd_disp_x[] = {
    0x2A,
    0x00, 0x00, 0x04, 0xAF
};

static char lcd_disp_y[] = {
    0x2B,
    0x00, 0x00, 0x07, 0x7F
};

/* Delay TE*/
static char Delay_TE[] = {
    0x44,
    0x07, 0x7F,
};

static char soft_reset_cmd[] = {
    0X01,
};

static char LTPS_timing_setting_cmd[] = {
    0XC6,
    0x78, 0X01, 0X45, 0X05, 0X67, 0X67, 0X0A, 0X01,
    0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01,
    0X01, 0X0A, 0X19, 0X05
};

static char  MCAP_cmd[] = {
    0xB0,
    0x00,
};

static char  interface_setting[] = {
    0xB3,
    0x04,0x08,0x00,0x22,0x00,
};

static char  interface_ID_set_cmd[] = {
    0xB4,
    0x0C,
};
static char  DSI_CTRL_cmd[] = {
    0xB6,
    0x3A,0xD3,
};
static char  set_pixsel_cmd[] = {
    0x3A,
    0x77,
};
static char  set_col_addr_cmd[] = {
    0x2A,
    0x00, 0x00, 0x04, 0xAF,
};

static char  set_page_addr_cmd[] = {
    0x2B,
    0x00,0x00,0x07,0x7F,
};

static char  send_image_cmd[] = {
    0x2C,
};
static char sequencer_test_control_cmd[] = {
    0xD6,
    0x01,
};

static struct dsi_cmd_desc jdi_display_on_cmds[] = {
    {DTYPE_DCS_WRITE, 0, 5, WAIT_TYPE_MS,
        sizeof(soft_reset_cmd), soft_reset_cmd},
    {DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(MCAP_cmd), MCAP_cmd},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(interface_setting), interface_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(interface_ID_set_cmd), interface_ID_set_cmd},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(DSI_CTRL_cmd), DSI_CTRL_cmd},
    {DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(sequencer_test_control_cmd), sequencer_test_control_cmd},
    {DTYPE_GEN_LWRITE, 0, 10, WAIT_TYPE_US,
        sizeof(basic_ce_cmd), basic_ce_cmd},
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(LTPS_timing_setting_cmd), LTPS_timing_setting_cmd},
    {DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(set_pixsel_cmd), set_pixsel_cmd},
    {DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(set_col_addr_cmd), set_col_addr_cmd},
    {DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(set_page_addr_cmd), set_page_addr_cmd},
    {DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(send_image_cmd), send_image_cmd},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(bl_level), bl_level},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(bl_enable), bl_enable},
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(te_enable), te_enable},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(Delay_TE), Delay_TE},
    {DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
        sizeof(exit_sleep), exit_sleep},
    {DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
        sizeof(display_on), display_on},
};

static struct dsi_cmd_desc jdi_display_off_cmds[] = {
    {DTYPE_DCS_WRITE, 0, 20, WAIT_TYPE_MS,
        sizeof(display_off), display_off},
    {DTYPE_DCS_WRITE, 0, 80, WAIT_TYPE_MS,
        sizeof(enter_sleep), enter_sleep},
};
 static struct dsi_cmd_desc set_display_address[] = {
    {DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(lcd_disp_x), lcd_disp_x},
    {DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(lcd_disp_y), lcd_disp_y},
    {DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(send_image_cmd), send_image_cmd},
};
static struct dsi_cmd_desc jdi_cabc_cmds[] = {
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(MCAP_cmd), MCAP_cmd},
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(back_light_control1), back_light_control1},
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(back_light_control2), back_light_control2},
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(back_light_control3), back_light_control3},
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(SRE_control1_cmd), SRE_control1_cmd},
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(SRE_control2_cmd), SRE_control2_cmd},
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(SRE_control3_cmd), SRE_control3_cmd},
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(back_light_control4), back_light_control4},
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(bl_enable), bl_enable},
};

static struct dsi_cmd_desc jdi_china_ce_cmds[] = {
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(china_back_light_control1), china_back_light_control1},
    {DTYPE_GEN_LWRITE, 0, 10, WAIT_TYPE_US,
        sizeof(china_ce_cmd), china_ce_cmd},    
};
static struct dsi_cmd_desc jdi_basic_ce_cmds[] = {
    {DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(back_light_control1), back_light_control1},
    {DTYPE_GEN_LWRITE, 0, 10, WAIT_TYPE_US,
        sizeof(basic_ce_cmd), basic_ce_cmd},    
};

/************************************************************
 *
 * LCD VCC
 *
 ************************************************************/
// ToDo leave these code for comparing with other lcd driver
//#define VCC_LCDIO_NAME      "lcdio-vcc"
//#define VCC_LCDANALOG_NAME    "lcdanalog-vcc"

 /* scharg regulator */
//static struct regulator *vcc_lcdio;
//static struct regulator *vcc_lcdanalog;

static struct vcc_desc jdi_lcd_vcc_init_cmds[] = {
    /* vcc get */
//    {DTYPE_VCC_GET, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 0},
//  {DTYPE_VCC_GET, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 0},

    /* vcc set voltage */
//  {DTYPE_VCC_SET_VOLTAGE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 3100000, 3100000, WAIT_TYPE_MS, 0},
};
static struct vcc_desc jdi_lcd_vcc_finit_cmds[] = {
    /* vcc put */
//    {DTYPE_VCC_PUT, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 0},
//  {DTYPE_VCC_PUT, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 0},
};

static struct vcc_desc jdi_lcd_vcc_enable_cmds[] = {
    /* vcc enable */
//  {DTYPE_VCC_ENABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 3},
//    {DTYPE_VCC_ENABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 3},
};

static struct vcc_desc jdi_lcd_vcc_disable_cmds[] = {
    /* vcc disable */
//    {DTYPE_VCC_DISABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 3},
//  {DTYPE_VCC_DISABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 3},
};

/*******************************************************************************
** LCD IOMUX
*/
static struct pinctrl_data pctrl;

static struct pinctrl_cmd_desc jdi_lcd_pinctrl_init_cmds[] = {
    {DTYPE_PINCTRL_GET,       &pctrl, 0},
    {DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
    {DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};
static struct pinctrl_cmd_desc jdi_lcd_pinctrl_normal_cmds[] = {
    {DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};
static struct pinctrl_cmd_desc jdi_lcd_pinctrl_lowpower_cmds[] = {
    {DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc jdi_lcd_pinctrl_finit_cmds[] = {
    {DTYPE_PINCTRL_PUT, &pctrl, 0},
};
/*******************************************************************************
** LCD GPIO
*/
#define GPIO_LCD_BL_ENABLE_NAME "gpio_lcd_bl_enable"
#define GPIO_LCD_RESET_NAME "gpio_lcd_reset"
#define GPIO_LCD_ID_NAME    "gpio_lcd_id"
#define GPIO_LCD_P5V5_ENABLE_NAME   "gpio_lcd_p5v5_enable"
#define GPIO_LCD_N5V5_ENABLE_NAME "gpio_lcd_n5v5_enable"

static uint32_t gpio_lcd_bl_enable;  /*gpio_4_3, gpio_035*/
static uint32_t gpio_lcd_reset;  /*gpio_4_5, gpio_037*/
static uint32_t gpio_lcd_id;  /*gpio_4_6, gpio_038*/
static uint32_t gpio_lcd_p5v5_enable;  /*gpio_5_1, gpio_041*/
static uint32_t gpio_lcd_n5v5_enable;  /*gpio_5_2, gpio_042*/

static uint32_t g_vddio_type = 0;
static struct gpio_desc jdi_lcd_gpio_request_cmds[] = {
    /* backlight enable */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
        GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
    /* reset */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
        GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    /* lcd id */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
        GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
    /* AVDD_5.5V */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
        GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
    /* AVEE_-5.5V */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
        GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 0},
};

static struct gpio_desc jdi_lcd_gpio_free_cmds[] = {
    /* backlight enable */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
        GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
    /* reset */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
        GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    /* lcd id */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
        GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
    /* AVDD_5.5V */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
        GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
    /* AVEE_-5.5V */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
        GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 0},

};


static struct gpio_desc jdi_lcd_gpio_normal_cmds[] = {
    /* AVDD_5.5V*/
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 0,
        GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 1},
    /* AVEE_-5.5V */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
        GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 1},
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
        GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
        GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
        GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
    /* backlight enable */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
        GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 1},
    /* lcd id */
    {DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 1,
        GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
};

static struct gpio_desc jdi_lcd_gpio_lowpower_cmds[] = {
    /* backlight enable */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
        GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
    /* AVEE_-5.5V */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
        GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 0},
    /* AVDD_5.5V*/
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
        GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
        GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    /* backlight enable input */
    {DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
        GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
    /* AVEE_-5.5V input */
    {DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
        GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 0},
    /* AVDD_5.5V input */
    {DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
        GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
    /* reset input */
    {DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
        GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},

};
/*******************************************************************************
** LCD IOMUX
*/

/*******************************************************************************
** LCD GPIO
*/


static void jdi_power_on(struct platform_device* pdev)
{
    HISI_FB_INFO("%s suc!\n", __func__);

    vcc_cmds_tx(pdev, jdi_lcd_vcc_enable_cmds,
        ARRAY_SIZE(jdi_lcd_vcc_enable_cmds));

    /* lcd pinctrl normal */
    pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds, \
        ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));

    /* lcd gpio request */
    gpio_cmds_tx(jdi_lcd_gpio_request_cmds, \
        ARRAY_SIZE(jdi_lcd_gpio_request_cmds));
        
    
    /* lcd gpio normal */
    gpio_cmds_tx(jdi_lcd_gpio_normal_cmds, \
        ARRAY_SIZE(jdi_lcd_gpio_normal_cmds));
}

static void jdi_power_off(struct platform_device* pdev)
{
    struct hisi_fb_data_type *hisifd = NULL;

    BUG_ON(pdev == NULL);
    hisifd = platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL);

    HISI_FB_INFO("fb%d, +!\n", hisifd->index);

    /* backlight off */
    hisi_lcd_backlight_off(pdev);

    /* lcd display off sequence */
    mipi_dsi_cmds_tx(jdi_display_off_cmds, \
        ARRAY_SIZE(jdi_display_off_cmds), hisifd->mipi_dsi0_base);

    /* lcd gpio lowpower */
    gpio_cmds_tx(jdi_lcd_gpio_lowpower_cmds, \
        ARRAY_SIZE(jdi_lcd_gpio_lowpower_cmds));
    /* lcd gpio free */
    gpio_cmds_tx(jdi_lcd_gpio_free_cmds, \
        ARRAY_SIZE(jdi_lcd_gpio_free_cmds));

    /* lcd pinctrl lowpower */
    pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_lowpower_cmds, \
        ARRAY_SIZE(jdi_lcd_pinctrl_lowpower_cmds));

    /* lcd vcc disable */
    vcc_cmds_tx(pdev, jdi_lcd_vcc_disable_cmds, \
        ARRAY_SIZE(jdi_lcd_vcc_disable_cmds));

}


/******************************************************************************/

static int cabc_mode = 1; /* allow application to set cabc mode to ui mode */
static int ce_mode = 1;
static ssize_t mipi_jdi_lcd_info_show(struct platform_device *dev, char *buf)
{
    int ret = 0;
    struct hisi_panel_info *pinfo = NULL;

    pinfo = jdi_panel_data.panel_info;
    snprintf(buf, PAGE_SIZE,"jdi_R69429 7.02' CMD TFT %d x %d\n",pinfo->xres, pinfo->yres);
    ret = strlen(buf) + 1;

    return ret;
}


static ssize_t get_ce_mode(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    return snprintf(buf, 16,"%d\n", ce_mode);
}

static ssize_t set_ce_mode(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int ret = 0;
    unsigned long val = 0;
    struct hisi_fb_data_type *hisifd = NULL;	

	BUG_ON(hisifb_pdev == NULL);
	hisifd = (struct hisi_fb_data_type *)platform_get_drvdata(hisifb_pdev);
	BUG_ON(hisifd == NULL);

    HISI_FB_ERR("fb%d, +!\n", hisifd->index);

    ret = strict_strtoul(buf, 0, &val);
    if (ret)
        return ret;

    ce_mode = val;

	if (hisifd->panel_power_on == false)
	{
		return snprintf((char *)buf, 16,"%d\n", ce_mode);
	}

	if(1 == ce_mode) {
		mipi_dsi_cmds_tx(jdi_china_ce_cmds, ARRAY_SIZE(jdi_china_ce_cmds), hisifd->mipi_dsi0_base);
		mdelay(5);
	}
	else {
		mipi_dsi_cmds_tx(jdi_basic_ce_cmds, ARRAY_SIZE(jdi_basic_ce_cmds), hisifd->mipi_dsi0_base);
		mdelay(5);
	}

	return snprintf((char *)buf, 16,"%d\n", ce_mode);
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

static int mipi_jdi_panel_on(struct platform_device *pdev)
{
    struct hisi_fb_data_type *hisifd = NULL;
    struct hisi_panel_info *pinfo = NULL;

    char __iomem *mipi_dsi0_base = NULL;
    uint32_t status = 0;
    uint32_t try_times = 0;

#if defined (CONFIG_HUAWEI_DSM)
	static struct lcd_reg_read_t lcd_status_reg[] = {
		{0x0A, 0x1C, 0x5C, "lcd power state"},
		{0x0E, 0x80, 0xC1, "lcd signal mode"},
		{0x05, 0x00, 0xFF, "mipi dsi error number"},
//		{0xDA, 0x00, 0x00, "RDID1"},
	};
#endif


    BUG_ON(pdev == NULL);
    
    HISI_FB_INFO("%s enter succ!\n",__func__);
    hisifd = (struct hisi_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL);

    HISI_FB_INFO("fb%d, +!\n", hisifd->index);

    pinfo = &(hisifd->panel_info);
    mipi_dsi0_base = hisifd->mipi_dsi0_base;
    HISI_FB_INFO("%s,pinfo->lcd_init_step = %d!\n",__func__,pinfo->lcd_init_step);
    if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
        LOG_JANK_D(JLID_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
        g_debug_enable = true;

        
        jdi_power_on(pdev);
        pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
  
    } else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE) {

        /* lcd display on sequence */ 
        mipi_dsi_cmds_tx(jdi_display_on_cmds, ARRAY_SIZE(jdi_display_on_cmds),hisifd->mipi_dsi0_base);
        mipi_dsi_cmds_tx(jdi_cabc_cmds, ARRAY_SIZE(jdi_cabc_cmds),hisifd->mipi_dsi0_base);

    	if(1 == ce_mode) {
        	mipi_dsi_cmds_tx(jdi_china_ce_cmds, ARRAY_SIZE(jdi_china_ce_cmds),hisifd->mipi_dsi0_base);
       		mdelay(5);
		}
    	else
    	{
      		mipi_dsi_cmds_tx(jdi_basic_ce_cmds, ARRAY_SIZE(jdi_basic_ce_cmds),hisifd->mipi_dsi0_base);
       		mdelay(5);
	}

        backlight_log_once = true;

#if defined (CONFIG_HUAWEI_DSM)
		panel_check_status_and_report_by_dsm(lcd_status_reg, \
			ARRAY_SIZE(lcd_status_reg), mipi_dsi0_base);
#endif

        pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
    } else if (pinfo->lcd_init_step == LCD_INIT_MIPI_HS_SEND_SEQUENCE) {
        /* backlight on */
        hisi_lcd_backlight_on(pdev);
    } else {
        HISI_FB_ERR("failed to init lcd!\n");
    }
    /* backlight on */
    hisi_lcd_backlight_on(pdev);

    HISI_FB_INFO("fb%d, -!\n", hisifd->index);
    return 0;
}

static int mipi_jdi_panel_off(struct platform_device *pdev)
{
    struct hisi_fb_data_type *hisifd = NULL;
    struct hisi_panel_info *pinfo = NULL;

    BUG_ON(pdev == NULL);
    HISI_FB_INFO("%s enter succ!\n",__func__);
    LOG_JANK_D(JLID_KERNEL_LCD_POWER_OFF, "%s", "JL_KERNEL_LCD_POWER_OFF");

    hisifd = (struct hisi_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL);
    pinfo = &(hisifd->panel_info); 

        
    mipi_dsi_cmds_tx(jdi_display_off_cmds, ARRAY_SIZE(jdi_display_off_cmds),hisifd->mipi_dsi0_base);

    jdi_power_off(pdev);
        
    return 0;
}

static int mipi_jdi_panel_remove(struct platform_device *pdev)
{
    struct hisi_fb_data_type *hisifd = NULL;

    HISI_FB_INFO("%s enter succ!\n",__func__);
    BUG_ON(pdev == NULL);

    hisifd = (struct hisi_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL); 

    HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
    /* lcd vcc finit */
    vcc_cmds_tx(pdev, jdi_lcd_vcc_finit_cmds,
        ARRAY_SIZE(jdi_lcd_vcc_finit_cmds));

    /* lcd pinctrl finit */
    pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_finit_cmds,
        ARRAY_SIZE(jdi_lcd_pinctrl_finit_cmds));

    HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

    return 0;
}

static int mipi_jdi_panel_set_backlight(struct platform_device *pdev)
{
    struct hisi_fb_data_type *hisifd = NULL;
    u32 level = 0;

    char bl_level_adjust[2] = {
        0x51,
        0x00,
    };
	
    struct dsi_cmd_desc  jdi_bl_level_adjust[] = {
        {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
            sizeof(bl_level_adjust), bl_level_adjust},      
    };

    struct dsi_cmd_desc  jdi_bl_enable_dimming[] = {
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(bl_enable_dimming), bl_enable_dimming},      
    };

    BUG_ON(pdev == NULL);
    hisifd = (struct hisi_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL); 

    /*Our eyes are more sensitive to small brightness.
    So we adjust the brightness of lcd following iphone4 */
    level = hisifd->bl_level;

    HISI_FB_INFO("%s enter succ!, level = %d\n",__func__, level);	

    if (level > 255) {
        level = 255;
    }
    //backlight may turn off when bl_level is below 6.
    /*Our eyes are more sensitive to small brightness.
    So we adjust the brightness of lcd following iphone4
    ---the original description for >248*/
    if (level < 6 && level != 0)
    {
        level = 6;
    } 

    bl_level_adjust[1] = level;
	
    mipi_dsi_cmds_tx(jdi_bl_level_adjust, \
        ARRAY_SIZE(jdi_bl_level_adjust),hisifd->mipi_dsi0_base);

    if (backlight_log_once) {
        backlight_log_once = false;
        HISI_FB_DEBUG("----hisifd->bl_level=%d,set backlight to level = %d\n",hisifd->bl_level, level);
        mdelay(20);
        mipi_dsi_cmds_tx(jdi_bl_enable_dimming, \
            ARRAY_SIZE(jdi_bl_enable_dimming),hisifd->mipi_dsi0_base);
    }
    
    HISI_FB_DEBUG(" set backlight succ level = %d \n",level);

	if (unlikely(g_debug_enable)) {
		LOG_JANK_D(JLID_KERNEL_LCD_BACKLIGHT_ON, "JL_KERNEL_LCD_BACKLIGHT_ON,%u", level);
		g_debug_enable = false;
	}

    return 0;
}

static int mipi_jdi_panel_set_fastboot(struct platform_device *pdev)
{
    struct hisi_fb_data_type *hisifd = NULL;

    BUG_ON(pdev == NULL);

    hisifd = platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL);

    HISI_FB_INFO("%s enter succ!\n",__func__);	
    HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

    /* lcd pinctrl normal */
    pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds,
        ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));

    /* lcd gpio request */
    gpio_cmds_tx(jdi_lcd_gpio_request_cmds,
        ARRAY_SIZE(jdi_lcd_gpio_request_cmds));

     if(1 == ce_mode) {
    	       mipi_dsi_cmds_tx(jdi_china_ce_cmds, ARRAY_SIZE(jdi_china_ce_cmds), hisifd->mipi_dsi0_base);
		mdelay(5);
     }
     else{
		mipi_dsi_cmds_tx(jdi_basic_ce_cmds, ARRAY_SIZE(jdi_basic_ce_cmds), hisifd->mipi_dsi0_base);
		mdelay(5);
     }

    /* backlight on */
    hisi_lcd_backlight_on(pdev);
    HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
    
    return 0;
}


static int mipi_jdi_panel_lcd_cabc_mode_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", cabc_mode);
}

static ssize_t mipi_jdi_panel_lcd_cabc_mode_store(struct platform_device *pdev,
    const char *buf, size_t count)
{
    struct hisi_fb_data_type *hisifd = NULL;
    ssize_t ret = 0;
    unsigned long val = 0;

    char  cabc_ui_on[] =
    {
        0x55,
        0x01 | g_IE_SRE_level,
    };

    char  cabc_vid_on[] =
    {
        0x55,
        0x03 | g_IE_SRE_level,
    };

    char  cabc_off[] =
    {
        0x55,
        0x00 | g_IE_SRE_level,
    };

    struct dsi_cmd_desc cabc_ui_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(cabc_ui_on), cabc_ui_on
        },
    };

    struct dsi_cmd_desc cabc_vid_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(cabc_vid_on), cabc_vid_on
        },
    };
    struct dsi_cmd_desc cabc_off_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(cabc_off), cabc_off
        },
    };

    HISI_FB_ERR("%s buf = %s, count = %d\n", __func__, buf, (int)count);
        
    ret = strict_strtoul(buf, 0, &val);
    if (ret)
        return ret;
        
    BUG_ON(pdev == NULL);
    hisifd = platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL);

    HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

    //now check backlight
    if ((hisifd->bl_level <= 15)&&(CABC_OFF != val))
    {
        cabc_wanted = val;
        HISI_FB_ERR("could not set cabc to %d, because backlight %u <= 15!\n", val, hisifd->bl_level);
        ret = -1;
        return ret;
    }

    /* Fix me: Implement it */
    switch (val)
    {
        case CABC_UI_MODE:
            mipi_dsi_cmds_tx(cabc_ui_cmds, \
                            ARRAY_SIZE(cabc_ui_cmds), hisifd->mipi_dsi0_base);
            cabc_mode = val;
            cabc_wanted = CABC_OFF;
            HISI_FB_ERR("set to CABC_UI_MODE mode !\n");
            break;
        case CABC_MOVING_MODE:
            mipi_dsi_cmds_tx(cabc_vid_cmds, \
                            ARRAY_SIZE(cabc_vid_cmds), hisifd->mipi_dsi0_base);
            cabc_mode = val;
            cabc_wanted = CABC_OFF;
            HISI_FB_ERR("set to CABC_MOVING_MODE mode !\n");
            break;
        case CABC_OFF:
            mipi_dsi_cmds_tx(cabc_off_cmds, \
                            ARRAY_SIZE(cabc_off_cmds), hisifd->mipi_dsi0_base);
            cabc_mode = val;
            cabc_wanted = CABC_OFF;
            HISI_FB_ERR("set to CABC_OFF mode !\n");
            break;
        default:
            ret = -1;
            break;
    }

    return snprintf((char *)buf, count, "%d\n", cabc_mode);
}


static int mipi_jdi_panel_set_sre(struct platform_device* pdev, int enable)
{
    struct hisi_fb_data_type* hisifd = NULL;

    HISI_FB_INFO("%s enter succ!SRE enable = %d reg0x55_value = 0x%x\n", __func__, enable, g_CABC_mode | g_IE_SRE_level);

    /* As the reg 0x55 is used by CABC and SRE /IE, so use local data to enable each funtion seperately */
    char sbl_level_setting[2] =
    {
        0x55,
        0x00,
    };

    struct dsi_cmd_desc sbl_level_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(sbl_level_setting), sbl_level_setting
        },
    };

    BUG_ON(pdev == NULL);
    hisifd = (struct hisi_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL);

    if (enable == 1)                            // Enable SRE settings
    {
            g_IE_SRE_level = 0x70;           //SRE Strong LEVEL  ,SRE_ON = 1, SRE[1:0] = 11b
            sbl_level_setting[1]  = g_CABC_mode | g_IE_SRE_level;
            mipi_dsi_cmds_tx(sbl_level_cmds, ARRAY_SIZE(sbl_level_cmds), hisifd->mipi_dsi0_base); 
    }
    else  if (enable == 0)                 // Disable SRE settings
    {
            g_IE_SRE_level = 0x80;          //SRE_ON = 0, SRE[1:0] = 00b
            sbl_level_setting[1]  = g_CABC_mode | g_IE_SRE_level;
            mipi_dsi_cmds_tx(sbl_level_cmds, ARRAY_SIZE(sbl_level_cmds), hisifd->mipi_dsi0_base);

    }
    else
    {
           HISI_FB_ERR("%s input error enable = %d\n", __func__, enable); 
           return -1;
    }

    HISI_FB_INFO("%s exit succ!SRE enable = %d reg0x55_value = 0x%x\n", __func__, enable,  g_CABC_mode | g_IE_SRE_level);
    return 0;
}


static ssize_t mipi_jdi_panel_lcd_check_reg_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
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

       HISI_FB_INFO("%s enter succ!\n",__func__);	
	   
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

	return ret;
}

static ssize_t mipi_jdi_panel_lcd_mipi_detect_show(struct platform_device *pdev, char *buf)
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

       HISI_FB_INFO("%s enter succ!\n",__func__);	

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

	return ret;
}

static ssize_t mipi_jdi_panel_lcd_hkadc_debug_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

       HISI_FB_INFO("%s enter succ!\n",__func__);	

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = snprintf(buf, PAGE_SIZE, "%d\n", hkadc_buf*4);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_jdi_panel_lcd_hkadc_debug_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	int ret = 0;
	int channel = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

    HISI_FB_INFO("%s enter succ!\n",__func__);	
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

static ssize_t mipi_jdi_panel_lcd_gram_check_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	u32 rd[CHECKSUM_SIZE] = {0};
	int i = 0;
	char cmdF_page0_select[] = {0xFF, 0xF0};
	char cmd1_page0_select[] = {0xFF, 0x10};
	char checksum_read[] = {0x73};

	struct dsi_cmd_desc packet_size_set_cmd = {DTYPE_MAX_PKTSIZE, 0, 10, WAIT_TYPE_US, 1, NULL};

	struct dsi_cmd_desc lcd_checksum_select_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(cmdF_page0_select), cmdF_page0_select},
	};

	struct dsi_cmd_desc lcd_checksum_dis_select_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(cmd1_page0_select), cmd1_page0_select},
	};

	struct dsi_cmd_desc lcd_check_reg[] = {
		{DTYPE_DCS_READ, 0, 20, WAIT_TYPE_US,
			sizeof(checksum_read), checksum_read},
	};

       HISI_FB_INFO("%s enter succ!\n",__func__);	

	if (!checksum_enable_ctl) {
		HISI_FB_INFO("Checksum disabled\n");
		return ret;
	}

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	mipi_dsi_max_return_packet_size(&packet_size_set_cmd, mipi_dsi0_base);

	mipi_dsi_cmds_tx(lcd_checksum_select_cmds, \
		ARRAY_SIZE(lcd_checksum_select_cmds), mipi_dsi0_base);
	for (i = 0; i < CHECKSUM_SIZE; i++) {
		char *data = lcd_check_reg[0].payload;
		*data = 0x73 + i;
		mipi_dsi_cmds_rx((rd + i), lcd_check_reg, \
			ARRAY_SIZE(lcd_check_reg), mipi_dsi0_base);
	}

	ret = snprintf(buf, PAGE_SIZE, "%d %d %d %d %d %d %d %d\n", \
			rd[0], rd[1], rd[2], rd[3], rd[4], rd[5], rd[6], rd[7]);
	HISI_FB_INFO("%d %d %d %d %d %d %d %d\n", \
			rd[0], rd[1], rd[2], rd[3], rd[4], rd[5], rd[6], rd[7]);

	mipi_dsi_cmds_tx(lcd_checksum_dis_select_cmds, \
		ARRAY_SIZE(lcd_checksum_dis_select_cmds), mipi_dsi0_base);

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_jdi_panel_lcd_gram_check_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	char cmdF_page0_select[] = {0xFF, 0xF0};
	char checksum_init[] = {0x7B, 0x00};
	char checksum_ena[] = {0x92, 0x01};
	char checksum_dis[] = {0x92, 0x00};
	char cmd1_page0_select[] = {0xFF, 0x10};

	struct dsi_cmd_desc lcd_checksum_enable_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(cmdF_page0_select), cmdF_page0_select},
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(checksum_init), checksum_init},
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(checksum_ena), checksum_ena},
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(cmd1_page0_select), cmd1_page0_select},
	};

	struct dsi_cmd_desc lcd_checksum_disable_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(cmdF_page0_select), cmdF_page0_select},
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(checksum_dis), checksum_dis},
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(cmd1_page0_select), cmd1_page0_select},
	};

       HISI_FB_INFO("%s enter succ!\n",__func__);	

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if ((!*buf) ||(!strchr("yY1nN0", *buf))) {
		HISI_FB_ERR("Input param is error(valid: yY1nN0): %s. \n",buf);
		return -EINVAL;
	}

	checksum_enable_ctl = !!strchr("yY1", *buf);
	if(checksum_enable_ctl == true) {
		mipi_dsi_cmds_tx(lcd_checksum_enable_cmds, \
			ARRAY_SIZE(lcd_checksum_enable_cmds), mipi_dsi0_base);
		HISI_FB_INFO("Enable checksum\n");
	} else {
		mipi_dsi_cmds_tx(lcd_checksum_disable_cmds, \
			ARRAY_SIZE(lcd_checksum_disable_cmds), mipi_dsi0_base);
		HISI_FB_INFO("Disable checksum\n");
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return count;
}

static int bist_read_and_check(struct platform_device *pdev, char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	u32 D8_value = 0xFF, D9_value = 0xFF, BC_value = 0xFF;
	int ret = LCD_BIST_CHECK_PASS;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;

       HISI_FB_INFO("%s enter succ!\n",__func__);	
	   
	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0xD806);
	if (!mipi_dsi_read(&D8_value, mipi_dsi0_base)) {
		HISI_FB_ERR("LCD bist check read register D8 timeout\n");
		ret = LCD_BIST_CHECK_TIMEOUT;
		goto read_reg_failed;
	}

	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0xD906);
	if (!mipi_dsi_read(&D9_value, mipi_dsi0_base)) {
		HISI_FB_ERR("LCD bist check read register D9 timeout\n");
		ret = LCD_BIST_CHECK_TIMEOUT;
		goto read_reg_failed;
	}

	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0xBC06);
	if (!mipi_dsi_read(&BC_value, mipi_dsi0_base)) {
		HISI_FB_ERR("LCD bist check read register BC timeout\n");
		ret = LCD_BIST_CHECK_TIMEOUT;
		goto read_reg_failed;
	}

	D8_value &= 0xFF;
	D9_value &= 0xFF;
	BC_value &= 0x60;
	ret = ((D8_value == 0xFF) && (D9_value == 0x3F) && (BC_value == 0x60)) ? \
				LCD_BIST_CHECK_PASS : LCD_BIST_CHECK_FAIL;
	HISI_FB_INFO("D8:%x, D9:%x, BC:%x\n", D8_value, D9_value, BC_value);

read_reg_failed:
	return ret;
}

static ssize_t mipi_jdi_panel_lcd_bist_check(struct platform_device *pdev,
	char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	int ret = LCD_BIST_CHECK_PASS;
	int final_ret = LCD_BIST_CHECK_PASS;
	int i, j;
#if 0
    HISI_FB_INFO("%s enter succ!\n",__func__);	
	   
	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	hisifd->lcd_self_testing = true;

#ifdef LAST_4_STEPS_ONLY_FOR_BIST_CHECK
	j = 0;
#else
	j = 5;
#endif
	for (i = j; i<ARRAY_SIZE(jdi_display_bist_check_cmds); i++) {
		HISI_FB_INFO("TEST %d\n", i+1);
		mipi_dsi_cmds_tx(jdi_display_bist_check_cmds[i], \
			bist_check_cmds_size[i], mipi_dsi0_base);
		ret = bist_read_and_check(pdev, buf);
		if (ret != LCD_BIST_CHECK_PASS) {
			HISI_FB_ERR("check failed\n");
			final_ret = ret;
		}
	}

	if (final_ret == LCD_BIST_CHECK_PASS)
		snprintf(buf, PAGE_SIZE, "pass\n");
	else
		snprintf(buf, PAGE_SIZE, "fail\n");

	mipi_dsi_cmds_tx(jdi_display_bist_check_end, \
		ARRAY_SIZE(jdi_display_bist_check_end), mipi_dsi0_base);

	hisifd->lcd_self_testing = false;
#endif
	return final_ret;
}
static int mipi_jdi_panel_set_display_region(struct platform_device *pdev,
    struct dss_rect *dirty)
{
    struct hisi_fb_data_type *hisifd = NULL;

    HISI_FB_DEBUG("%s enter succ!dss_rect = {%d, %d, %d, %d}\n", __func__, 
	dirty->x, dirty->y, dirty->w, dirty->h);

    BUG_ON(pdev == NULL || dirty == NULL);
    hisifd = platform_get_drvdata(pdev);
    BUG_ON(hisifd == NULL);

    lcd_disp_x[1] = (dirty->x >> 8) & 0xff;
    lcd_disp_x[2] = dirty->x & 0xff;
    lcd_disp_x[3] = ((dirty->x + dirty->w - 1) >> 8) & 0xff;
    lcd_disp_x[4] = (dirty->x + dirty->w - 1) & 0xff;
    lcd_disp_y[1] = (dirty->y >> 8) & 0xff;
    lcd_disp_y[2] = dirty->y & 0xff;
    lcd_disp_y[3] = ((dirty->y + dirty->h - 1) >> 8) & 0xff;
    lcd_disp_y[4] = (dirty->y + dirty->h - 1) & 0xff;

    mipi_dsi_cmds_tx(set_display_address, \
        ARRAY_SIZE(set_display_address), hisifd->mipi_dsi0_base);

    return 0;
}


/*for esd check*/
static int mipi_jdi_panel_check_esd(struct platform_device* pdev)
{
	int ret = 0;
	int err = 0;
	unsigned int val = 0;
	struct hisi_fb_data_type* hisifd = NULL;

	u32 read_value[1] = {0};
	u32 expected_value[1] = {0x1c};
	u32 read_mask[1] = {0x5C};
	char* reg_name[1] = {"power mode"};
	char lcd_reg_0a[] = {0x0a};

	struct dsi_cmd_desc lcd_check_reg[] = {
		{DTYPE_DCS_READ, 0, 10, WAIT_TYPE_US,
			sizeof(lcd_reg_0a), lcd_reg_0a},
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
	hisifd = (struct hisi_fb_data_type*)platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_ERR("fb%d, +.\n", hisifd->index);
	if (!mipi_dsi_read_compare(&data, hisifd->mipi_dsi0_base)) 
	{
		err = 0;
	} 
	else 
	{
		err = 1;
	}

	HISI_FB_ERR("fb%d, -.\n", hisifd->index);

    return err;
}
static struct hisi_panel_info jdi_panel_info = {0};
static struct hisi_fb_panel_data jdi_panel_data = {
    .panel_info = &jdi_panel_info,
    .on = mipi_jdi_panel_on,
    .off = mipi_jdi_panel_off,
    .remove = mipi_jdi_panel_remove,
    .set_backlight = mipi_jdi_panel_set_backlight,
    .lcd_model_show = mipi_jdi_lcd_info_show,
    .set_fastboot = mipi_jdi_panel_set_fastboot,
    .lcd_cabc_mode_store = mipi_jdi_panel_lcd_cabc_mode_store,
    .lcd_cabc_mode_show = mipi_jdi_panel_lcd_cabc_mode_show,
	.lcd_check_reg = mipi_jdi_panel_lcd_check_reg_show,
	.lcd_mipi_detect = mipi_jdi_panel_lcd_mipi_detect_show,
	.lcd_hkadc_debug_show = mipi_jdi_panel_lcd_hkadc_debug_show,
	.lcd_hkadc_debug_store = mipi_jdi_panel_lcd_hkadc_debug_store,
	.lcd_gram_check_show = mipi_jdi_panel_lcd_gram_check_show,
	.lcd_gram_check_store = mipi_jdi_panel_lcd_gram_check_store,
	//	.lcd_bist_check = mipi_jdi_panel_lcd_bist_check,
    .esd_handle = mipi_jdi_panel_check_esd,
//    .sbl_ctrl = mipi_jdi_panel_set_sre,
    .set_display_region = mipi_jdi_panel_set_display_region,
};


/*******************************************************************************
**
*/
static int mipi_jdi_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct hisi_panel_info *pinfo = NULL;

    
    struct device_node *np = NULL;
    uint32_t bl_type = 0;

    HISI_FB_INFO("%s enter succ!\n",__func__);
	
    if (hisi_fb_device_probe_defer(PANEL_MIPI_CMD)) {
        goto err_probe_defer;
    }

    HISI_FB_DEBUG("+.\n");

    np = of_find_compatible_node(NULL, NULL, DTS_COMP_JDI_R69429);
    if (!np) {
        HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_JDI_R69429);
        goto err_return;
    }

    gpio_lcd_bl_enable = of_get_named_gpio(np, "gpios", 0);  /*gpio_4_3, gpio_035*/
    gpio_lcd_reset = of_get_named_gpio(np, "gpios", 1);  /*gpio_4_5, gpio_037*/
    gpio_lcd_id = of_get_named_gpio(np, "gpios", 2);  /*gpio_4_6, gpio_038*/
    gpio_lcd_p5v5_enable = of_get_named_gpio(np, "gpios", 3);  /*gpio_5_1, gpio_041*/
    gpio_lcd_n5v5_enable = of_get_named_gpio(np, "gpios", 4);  /*gpio_5_2, gpio_042*/

    ret = of_property_read_u32(np, LCD_BL_TYPE_NAME, &bl_type);
    if (ret) {
        HISI_FB_ERR("get lcd_bl_type failed!\n");
        bl_type = BL_SET_BY_PWM;
    }

    ret = of_property_read_u32(np, LCD_VDDIO_TYPE_NAME, &g_vddio_type);
    if (ret) {
        HISI_FB_ERR("get lcd_vddio_type failed!\n");
        g_vddio_type = 0;
    }

    pdev->id = 1;
    /* init lcd panel info */
    pinfo = jdi_panel_data.panel_info;
    memset(pinfo, 0, sizeof(struct hisi_panel_info));
    /* init lcd panel info */
    pinfo->xres = 1200;
    pinfo->yres = 1920;
    pinfo->width  = 94;  //mm
    pinfo->height = 151; //mm
    pinfo->type = PANEL_MIPI_CMD;
    pinfo->orientation = LCD_PORTRAIT;
    pinfo->bpp = LCD_RGB888;
    pinfo->bgr_fmt = LCD_RGB;
    pinfo->bl_set_type = bl_type;

    pinfo->bl_min = 1;
    pinfo->bl_max = 255;
    pinfo->vsync_ctrl_type = (VSYNC_CTRL_ISR_OFF |
        VSYNC_CTRL_MIPI_ULPS);
    pinfo->frc_enable = 0;



    if (NULL != strstr(saved_command_line, "androidboot.swtype=factory"))
    {
        pinfo->sbl_support = 0;
        pinfo->esd_enable = 0;
    }
    else
    {
        pinfo->sbl_support = 1;	
        pinfo->esd_enable = 0;
    }

    pinfo->dirty_region_updt_support = 1;
	pinfo->dirty_region_info.left_align = 4;
	pinfo->dirty_region_info.right_align = 4;
	pinfo->dirty_region_info.top_align = 2;
	pinfo->dirty_region_info.bottom_align = 2;
	pinfo->dirty_region_info.w_align = -1;
	pinfo->dirty_region_info.h_align = -1;
	pinfo->dirty_region_info.w_min = 1200;
	pinfo->dirty_region_info.h_min = 2;
    pinfo->dirty_region_info.top_start = -1;
    pinfo->dirty_region_info.bottom_start = -1;

    pinfo->color_temperature_support = 1;
    pinfo->smart_bl.strength_limit = 100;
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
    pinfo->ldi.h_back_porch = 10;
    pinfo->ldi.h_front_porch = 90;
    pinfo->ldi.h_pulse_width = 10;
    pinfo->ldi.v_back_porch = 6;
    pinfo->ldi.v_front_porch = 8;
    pinfo->ldi.v_pulse_width = 2;

    /* Note: must init here */  
    pinfo->mipi.lane_nums = DSI_4_LANES;
    pinfo->mipi.color_mode = DSI_24BITS_1;
    pinfo->mipi.vc = 0;
    pinfo->mipi.dsi_bit_clk = 500;
    pinfo->pxl_clk_rate = 160*1000000UL;

    /* lcd vcc init */
    ret = vcc_cmds_tx(pdev, jdi_lcd_vcc_init_cmds,
        ARRAY_SIZE(jdi_lcd_vcc_init_cmds));
    if (ret != 0) {
        HISI_FB_ERR("LCD vcc init failed!\n");
        goto err_return;
    }

    if (fastboot_set_needed) {
        vcc_cmds_tx(pdev, jdi_lcd_vcc_enable_cmds,
            ARRAY_SIZE(jdi_lcd_vcc_enable_cmds));
    }

    /* lcd pinctrl init */
    ret = pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_init_cmds,
        ARRAY_SIZE(jdi_lcd_pinctrl_init_cmds));
    if (ret != 0) {
            HISI_FB_ERR("Init pinctrl failed, defer\n");
        goto err_return;
    }

    /* alloc panel device data */
    ret = platform_device_add_data(pdev, &jdi_panel_data,
        sizeof(struct hisi_fb_panel_data));
    if (ret) {
        HISI_FB_ERR("platform_device_add_data failed!\n");
        goto err_device_put;
    }

    hisifb_pdev = hisi_fb_add_device(pdev);

    jdi_sysfs_init(pdev);
//	sysfs_merge_group(hisifb_pdev, &jdi_attr_group);
//    jdi_sysfs_init(pdev);
//  pr_info("%s exit succ!\n",__func__);
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
        .compatible = DTS_COMP_JDI_R69429,
        .data = NULL,
    },
    {},
};
MODULE_DEVICE_TABLE(of, hisi_panel_match_table);

static struct platform_driver this_driver = {
    .probe = mipi_jdi_probe,
    .remove = NULL,
    .suspend = NULL,
    .resume = NULL,
    .shutdown = NULL,
    .driver = {
        .name = "mipi_jdi_R69429",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(hisi_panel_match_table),
    },
};

static int __init mipi_jdi_panel_init(void)
{
    int ret = 0;

    ret = platform_driver_register(&this_driver);
    if (ret) {
        HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
        return ret;
    }

    return ret;
}

module_init(mipi_jdi_panel_init);
