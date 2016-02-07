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
#include <huawei_platform/touthscreen/huawei_touchscreen.h>
//#include "mipi_jdi_NT36750.h"
#include <linux/hisi/hw_cmdline_parse.h>
#include <huawei_platform/log/log_jank.h>

#define DTS_COMP_JDI_NT36750 "hisilicon,mipi_jdi_NT36750"
#define LCD_VDDIO_TYPE_NAME	"lcd-vddio-type"
#define CABC_OFF 0
#define CABC_UI_MODE 1
#define CABC_STILL_MODE 2
#define CABC_MOVING_MODE 3
#define CHECKSUM_SIZE 8
static struct hisi_fb_panel_data jdi_panel_data;

static int hkadc_buf = 0;
static bool checksum_enable_ctl = false;
static bool g_debug_enable = false;
static int g_cabc_mode = 1;

extern int fastboot_set_needed;
extern bool gesture_func;
extern void ts_power_gpio_enable(void);
extern void ts_power_gpio_disable(void);


/*******************************************************************************
** Power ON Sequence(sleep mode to Normal mode)
*/
static char page_2_select[] = {
	0XFF,
	0x20,
};

static char vgho_hiZ[] = {
	0X30,
	0x20,
};

static char non_reload[] = {
	0XFB,
	0x01,
};

static char page_1_select[] = {
	0XFF,
	0x10,
};

static char tear_on[] = {
	0x35,
	0x00,
};

static char display_on[] = {
	0x29,
};

static char exit_sleep[] = {
	0x11,
};

static char bl_enable[] = {
	0x53,
	0x2C,
};

static char te_line[] = {
	0x44,
	0x07, 0x7D,
};

/*******************************************************************************
** setting PWM frequency to 58KHZ
*/
//static char set_page3[] = {
//	0xFF,
//	0x23,
//};

//static char set_pwm_freq[] = {
//	0x08,
//	0x03,
//};

//static char set_page0[] = {
//	0xFF,
//	0x10,
//};

/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/
static char display_off[] = {
	0x28,
};

static char enter_sleep[] = {
	0x10,
};

static struct dsi_cmd_desc jdi_display_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(tear_on), tear_on},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(page_2_select), page_2_select},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(vgho_hiZ), vgho_hiZ},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(non_reload), non_reload},
	{DTYPE_DCS_LWRITE, 0,200, WAIT_TYPE_US,
		sizeof(page_1_select), page_1_select},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(te_line), te_line},
	{DTYPE_DCS_WRITE, 0, 115, WAIT_TYPE_MS,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 0, 50, WAIT_TYPE_MS,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc jdi_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 0, 60, WAIT_TYPE_MS,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(enter_sleep), enter_sleep}
};
static char non_reload_0xFB[] = {
	0xFB,
	0x01,
};
static char cmd1_0xFF[] = {
	0xFF,
	0x10,
};


/******************************************************************************
*
** Display Effect Sequence(smart color, edge enhancement, smart contrast, cabc)
*/


static char page_selection_0xFF_0x22[] = {
	0xFF,
	0x22,
};

static char color_enhancement_0x00[] = {
	0x00,
	0x20,
};

static char color_enhancement_0x01[] = {
	0x01,
	0x21,
};

static char color_enhancement_0x02[] = {
	0x02,
	0x22,
};

static char color_enhancement_0x03[] = {
	0x03,
	0x23,
};

static char color_enhancement_0x04[] = {
	0x04,
	0x24,
};

static char color_enhancement_0x05[] = {
	0x05,
	0x25,
};

static char color_enhancement_0x06[] = {
	0x06,
	0x26,
};

static char color_enhancement_0x07[] = {
	0x07,
	0x27,
};

static char color_enhancement_0x08[] = {
	0x08,
	0x28,
};

static char color_enhancement_0x09[] = {
	0x09,
	0x29,
};

static char color_enhancement_0x0A[] = {
	0x0A,
	0x2A,
};

static char color_enhancement_0x0B[] = {
	0x0B,
	0x2B,
};

static char color_enhancement_0x0C[] = {
	0x0C,
	0x2C,
};

static char color_enhancement_0x0D[] = {
	0x0D,
	0x2D,
};

static char color_enhancement_0x0E[] = {
	0x0E,
	0x2E,
};

static char color_enhancement_0x0F[] = {
	0x0F,
	0x2F,
};

static char color_enhancement_0x10[] = {
	0x10,
	0x30,
};

static char color_enhancement_0x11[] = {
	0x11,
	0x30,
};

static char color_enhancement_0x12[] = {
	0x12,
	0x30,
};

static char color_enhancement_0x13[] = {
	0x13,
	0x30,
};

static char color_enhancement_0x1B[] = {
	0x1B,
	0x4C,
};

static char color_enhancement_0x1C[] = {
	0x1C,
	0x48,
};

static char color_enhancement_0x1D[] = {
	0x1D,
	0x44,
};

static char color_enhancement_0x1E[] = {
	0x1E,
	0x40,
};

static char color_enhancement_0x1F[] = {
	0x1F,
	0x3C,
};

static char color_enhancement_0x20[] = {
	0x20,
	0x38,
};

static char color_enhancement_0x21[] = {
	0x21,
	0x38,
};

static char color_enhancement_0x22[] = {
	0x22,
	0x38,
};

static char color_enhancement_0x23[] = {
	0x23,
	0x38,
};

static char color_enhancement_0x24[] = {
	0x24,
	0x38,
};

static char color_enhancement_0x25[] = {
	0x25,
	0x38,
};

static char color_enhancement_0x26[] = {
	0x26,
	0x38,
};

static char color_enhancement_0x27[] = {
	0x27,
	0x38,
};

static char color_enhancement_0x28[] = {
	0x28,
	0x30,
};

static char color_enhancement_0x29[] = {
	0x29,
	0x28,
};

static char color_enhancement_0x2A[] = {
	0x2A,
	0x20,
};

static char color_enhancement_0x2B[] = {
	0x2B,
	0x10,
};

static char color_enhancement_0x2F[] = {
	0x2F,
	0x00,
};

static char color_enhancement_0x30[] = {
	0x30,
	0x00,
};

static char color_enhancement_0x31[] = {
	0x31,
	0x00,
};

static char color_enhancement_0x32[] = {
	0x32,
	0x04,
};

static char color_enhancement_0x33[] = {
	0x33,
	0x00,
};

static char color_enhancement_0x34[] = {
	0x34,
	0x00,
};

static char color_enhancement_0x35[] = {
	0x35,
	0x04,
};

static char color_enhancement_0x36[] = {
	0x36,
	0x08,
};

static char color_enhancement_0x37[] = {
	0x37,
	0x0c,
};

static char color_enhancement_0x38[] = {
	0x38,
	0x10,
};

static char color_enhancement_0x39[] = {
	0x39,
	0x14,
};

static char color_enhancement_0x3A[] = {
	0x3A,
	0x18,
};

static char color_enhancement_0x3B[] = {
	0x3B,
	0x1c,
};

static char color_enhancement_0x3F[] = {
	0x3F,
	0x20,
};

static char color_enhancement_0x40[] = {
	0x40,
	0x24,
};

static char color_enhancement_0x41[] = {
	0x41,
	0x25,
};

static char color_enhancement_0x42[] = {
	0x42,
	0x26,
};

static char color_enhancement_0x43[] = {
	0x43,
	0x26,
};

static char color_enhancement_0x44[] = {
	0x44,
	0x26,
};

static char color_enhancement_0x45[] = {
	0x45,
	0x24,
};

static char color_enhancement_0x46[] = {
	0x46,
	0x20,
};

static char color_enhancement_0x47[] = {
	0x47,
	0x1c,
};

static char color_enhancement_0x48[] = {
	0x48,
	0x18,
};

static char color_enhancement_0x49[] = {
	0x49,
	0x14,
};

static char color_enhancement_0x4A[] = {
	0x4A,
	0x10,
};

static char color_enhancement_0x4B[] = {
	0x4B,
	0x0c,
};

static char color_enhancement_0x4C[] = {
	0x4C,
	0x08,
};

static char smart_color_ratio_0x4D[] = {
	0x4D,
	0x00,
};

static char smart_color_ratio_0x4E[] = {
	0x4E,
	0x03,
};

static char smart_color_ratio_0x4F[] = {
	0x4F,
	0x04,
};

static char vivid_color_enable_0x1A[] = {
	0x1A,
	0x17,
};

static char smart_color_enable_0x53[] = {
	0x53,
	0x77,
};

static char skin_protection_enable_0x54[] = {
	0x54,
	0x03,
};

static char v_constrain_enable_0x55[] = {
	0x55,
	0x77,
};

static char smart_contrast_enable_0x56[] = {
	0x56,
	0x00,
};

static char sharpness_enable_0x68[] = {
	0x68,
	0x00,
};

static char sharpness_0x65[] = {
	0x65,
	0x82,
};

static char sharpness_0x66[] = {
	0x66,
	0xA6,
};

static char sharpness_0x67[] = {
	0x67,
	0xC8,
};

static char sharpness_0x69[] = {
	0x69,
	0xF2,
};

static char sharpness_0x97[] = {
	0x97,
	0xFF,
};

static char sharpness_0x98[] = {
	0x98,
	0x1C,
};

static char dither_enable_0xA2[] = {
	0xA2,
	0x20,
};
static char page_selection_0xFF_0x23[] = {
	0xFF,
	0x23,
};

static char cabc_endimming_0x00[] = {
	0x00,
	0x02,
};

static char cabc_0x32[] = {
	0x32,
	0x03,
};

static char cabc_off[] = {
	0x55,
	0xB0,
};

static char cabc_set_mode_UI[] = {
	0x55,
	0xB1,
};

static char cabc_set_mode_STILL[] = {
	0x55,
	0xB2,
};

static char cabc_set_mode_MOVING[] = {
	0x55,
	0xB3,
};

static struct dsi_cmd_desc jdi_display_effect_on_cmds[] = {
	//diplay effect

	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(page_selection_0xFF_0x22), page_selection_0xFF_0x22},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(non_reload_0xFB), non_reload_0xFB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x00), color_enhancement_0x00},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x01), color_enhancement_0x01},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x02), color_enhancement_0x02},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x03), color_enhancement_0x03},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x04), color_enhancement_0x04},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x05), color_enhancement_0x05},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x06), color_enhancement_0x06},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x07), color_enhancement_0x07},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x08), color_enhancement_0x08},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x09), color_enhancement_0x09},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x0A), color_enhancement_0x0A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x0B), color_enhancement_0x0B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x0C), color_enhancement_0x0C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x0D), color_enhancement_0x0D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x0E), color_enhancement_0x0E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x0F), color_enhancement_0x0F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x10), color_enhancement_0x10},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x11), color_enhancement_0x11},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x12), color_enhancement_0x12},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x13), color_enhancement_0x13},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x1B), color_enhancement_0x1B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x1C), color_enhancement_0x1C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x1D), color_enhancement_0x1D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x1E), color_enhancement_0x1E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x1F), color_enhancement_0x1F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x20), color_enhancement_0x20},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x21), color_enhancement_0x21},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x22), color_enhancement_0x22},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x23), color_enhancement_0x23},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x24), color_enhancement_0x24},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x25), color_enhancement_0x25},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x26), color_enhancement_0x26},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x27), color_enhancement_0x27},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x28), color_enhancement_0x28},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x29), color_enhancement_0x29},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x2A), color_enhancement_0x2A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x2B), color_enhancement_0x2B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x2F), color_enhancement_0x2F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x30), color_enhancement_0x30},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x31), color_enhancement_0x31},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x32), color_enhancement_0x32},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x33), color_enhancement_0x33},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x34), color_enhancement_0x34},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x35), color_enhancement_0x35},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x36), color_enhancement_0x36},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x37), color_enhancement_0x37},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x38), color_enhancement_0x38},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x39), color_enhancement_0x39},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x3A), color_enhancement_0x3A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x3B), color_enhancement_0x3B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x3F), color_enhancement_0x3F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x40), color_enhancement_0x40},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x41), color_enhancement_0x41},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x42), color_enhancement_0x42},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x43), color_enhancement_0x43},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x44), color_enhancement_0x44},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x45), color_enhancement_0x45},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x46), color_enhancement_0x46},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x47), color_enhancement_0x47},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x48), color_enhancement_0x48},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x49), color_enhancement_0x49},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x4A), color_enhancement_0x4A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x4B), color_enhancement_0x4B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(color_enhancement_0x4C), color_enhancement_0x4C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(smart_color_ratio_0x4D), smart_color_ratio_0x4D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(smart_color_ratio_0x4E), smart_color_ratio_0x4E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(smart_color_ratio_0x4F), smart_color_ratio_0x4F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(vivid_color_enable_0x1A), vivid_color_enable_0x1A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(smart_color_enable_0x53), smart_color_enable_0x53},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(skin_protection_enable_0x54), skin_protection_enable_0x54},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(v_constrain_enable_0x55), v_constrain_enable_0x55},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(smart_contrast_enable_0x56), smart_contrast_enable_0x56},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(sharpness_enable_0x68), sharpness_enable_0x68},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(sharpness_0x65), sharpness_0x65},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(sharpness_0x66), sharpness_0x66},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(sharpness_0x67), sharpness_0x67},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(sharpness_0x69), sharpness_0x69},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(sharpness_0x97), sharpness_0x97},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(sharpness_0x98), sharpness_0x98},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(dither_enable_0xA2), dither_enable_0xA2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(page_selection_0xFF_0x23), page_selection_0xFF_0x23},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(non_reload_0xFB), non_reload_0xFB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_endimming_0x00), cabc_endimming_0x00},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_0x32), cabc_0x32},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(cmd1_0xFF), cmd1_0xFF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_set_mode_UI), cabc_set_mode_UI},
};

static struct dsi_cmd_desc jdi_cabc_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_off), cabc_off},
};

static struct dsi_cmd_desc jdi_cabc_ui_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_set_mode_UI), cabc_set_mode_UI},
};

static struct dsi_cmd_desc jdi_cabc_still_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_set_mode_STILL), cabc_set_mode_STILL},
};

static struct dsi_cmd_desc jdi_cabc_moving_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_set_mode_MOVING), cabc_set_mode_MOVING},
};
/*******************************************************************************
** LCD VCC
*/
#define VCC_LCDIO_NAME		"lcdio-vcc"
#define VCC_LCDANALOG_NAME	"lcdanalog-vcc"

static struct regulator *vcc_lcdio;
static struct regulator *vcc_lcdanalog;

static struct vcc_desc jdi_lcd_vcc_init_cmds[] = {
	/* vcc get */
	{DTYPE_VCC_GET, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 0},
	{DTYPE_VCC_GET, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 0},

	/* vcc set voltage */
	{DTYPE_VCC_SET_VOLTAGE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 3100000, 3100000, WAIT_TYPE_MS, 0},
};

static struct vcc_desc jdi_lcd_vcc_finit_cmds[] = {
	/* vcc put */
	{DTYPE_VCC_PUT, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 0},
	{DTYPE_VCC_PUT, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 0},
};

static struct vcc_desc jdi_lcd_vcc_enable_cmds[] = {
	/* vcc enable */
	{DTYPE_VCC_ENABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 3},
	{DTYPE_VCC_ENABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 3},
};

static struct vcc_desc jdi_lcd_vcc_disable_cmds[] = {
	/* vcc disable */
	{DTYPE_VCC_DISABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 3},
	{DTYPE_VCC_DISABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 3},
};

/*******************************************************************************
** LCD IOMUX
*/
static struct pinctrl_data pctrl;

static struct pinctrl_cmd_desc jdi_lcd_pinctrl_init_cmds[] = {
	{DTYPE_PINCTRL_GET, &pctrl, 0},
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
#define GPIO_LCD_BL_ENABLE_NAME	"gpio_lcd_bl_enable"
#define GPIO_LCD_RESET_NAME	"gpio_lcd_reset"
#define GPIO_LCD_ID_NAME	"gpio_lcd_id"
#define GPIO_LCD_P5V5_ENABLE_NAME	"gpio_lcd_p5v5_enable"
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

static struct gpio_desc jdi_lcd_gpio_sleep_free_cmds[] = {
	/* reset */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* backlight enable */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* lcd id */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
};

static struct gpio_desc jdi_lcd_gpio_sleep_request_cmds[] = {
	/* reset */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* backlight enable */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* lcd id */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
};

static struct gpio_desc jdi_lcd_gpio_sleep_lp_cmds[] = {
	/* backlight disable */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
};

static struct gpio_desc jdi_lcd_gpio_sleep_normal_cmds[] = {
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	/* backlight enable */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 1},
	/* lcd id */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
};

static struct gpio_desc jdi_lcd_gpio_normal_cmds[] = {
	/* AVDD_5.5V*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 0,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 1},
	/* AVEE_-5.5V */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 1},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
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
	
	char ce_disable[] = {0x55, 0x00};
	
	char ce_reg_medium[] = {0x55, 0xB0};
	char cabc_mode_reg[] = {0x00,0x01,0x02,0x03};//off,UI,still,moving
	struct dsi_cmd_desc ce_medium_cmd[] = {
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(ce_reg_medium), ce_reg_medium},
	};
	struct dsi_cmd_desc ce_disable_cmd[] = {
		{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(ce_disable), ce_disable},
	};

	ret = strict_strtoul(buf, 0, &val);
	if (ret)
	    return ret;

	if(0 == val) {
		HISI_FB_INFO("fb%d, CE -> disable !\n", hisifd_test->index);
		ce_disable[1] = (cabc_mode_reg[g_cabc_mode]&0x0f)|ce_disable[1];
		mipi_dsi_cmds_tx(ce_disable_cmd, ARRAY_SIZE(ce_disable_cmd),hisifd_test->mipi_dsi0_base);
		mdelay(5);
	}else if (1 == val){
		HISI_FB_INFO("fb%d, CE -> Weak level !\n", hisifd_test->index);
		//mipi_dsi_cmds_tx(ce_weak_cmd, ARRAY_SIZE(ce_weak_cmd),hisifd_test->mipi_dsi0_base);
		mdelay(5);
	}else if (2 == val){
		HISI_FB_INFO("fb%d, CE -> Medium level !\n", hisifd_test->index);
		ce_reg_medium[1] = (cabc_mode_reg[g_cabc_mode]&0x0f)|ce_reg_medium[1];
		mipi_dsi_cmds_tx(ce_medium_cmd, ARRAY_SIZE(ce_medium_cmd),hisifd_test->mipi_dsi0_base);
		mdelay(5);
	}else if (3 == val){
		HISI_FB_INFO("fb%d, CE -> Strong level !\n", hisifd_test->index);
		//mipi_dsi_cmds_tx(ce_strong_cmd, ARRAY_SIZE(ce_strong_cmd),hisifd_test->mipi_dsi0_base);
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
/*******************************************************************************
**
*/
static int mipi_jdi_panel_set_fastboot(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* lcd pinctrl normal */
	pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds,
		ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));

	/* lcd gpio request */
	gpio_cmds_tx(jdi_lcd_gpio_request_cmds,
		ARRAY_SIZE(jdi_lcd_gpio_request_cmds));

	/* backlight on */
	hisi_lcd_backlight_on(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int mipi_jdi_panel_on(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	char __iomem *mipi_dsi0_base = NULL;
#if defined (CONFIG_HUAWEI_DSM)
	static struct lcd_reg_read_t lcd_status_reg[] = {
		{0x0A, 0x9C, 0xFF, "lcd power state"},
		{0x0E, 0x80, 0xC1, "lcd signal mode"},
		{0x05, 0x00, 0xFF, "mipi dsi error number"},
		{0xDA, 0x00, 0x00, "RDID1"},
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
		if (false == gesture_func) {
			/* lcd vcc enable */
			vcc_cmds_tx(pdev, jdi_lcd_vcc_enable_cmds,
				ARRAY_SIZE(jdi_lcd_vcc_enable_cmds));
		//if (g_vddio_type)
		//	ts_power_gpio_enable();
		} else {
			HISI_FB_INFO("power on (gesture_func:%d)\n", gesture_func);
		}

		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE) {
		/* lcd pinctrl normal */
		pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds,
			ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));
		if (false == gesture_func) {
			/* lcd gpio request */
			gpio_cmds_tx(jdi_lcd_gpio_request_cmds, \
				ARRAY_SIZE(jdi_lcd_gpio_request_cmds));

			/* lcd gpio normal */
			gpio_cmds_tx(jdi_lcd_gpio_normal_cmds, \
				ARRAY_SIZE(jdi_lcd_gpio_normal_cmds));
		} else {
			gpio_cmds_tx(jdi_lcd_gpio_sleep_request_cmds, \
					ARRAY_SIZE(jdi_lcd_gpio_sleep_request_cmds));
			msleep(50);
			gpio_cmds_tx(jdi_lcd_gpio_sleep_normal_cmds, \
					ARRAY_SIZE(jdi_lcd_gpio_sleep_normal_cmds));
			HISI_FB_INFO("lp send sequence (gesture_func:%d)\n", gesture_func);
		}

		mipi_dsi_cmds_tx(jdi_display_effect_on_cmds, \
			ARRAY_SIZE(jdi_display_effect_on_cmds), mipi_dsi0_base);

		mipi_dsi_cmds_tx(jdi_display_on_cmds, \
			ARRAY_SIZE(jdi_display_on_cmds), mipi_dsi0_base);
			
		g_cabc_mode = 1;
		g_ce_mode = 2;
#if defined (CONFIG_HUAWEI_DSM)
		panel_check_status_and_report_by_dsm(lcd_status_reg, \
			ARRAY_SIZE(lcd_status_reg), mipi_dsi0_base);
#endif
		pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_HS_SEND_SEQUENCE) {
		/* backlight on */
		hisi_lcd_backlight_on(pdev);
		g_debug_enable = true;
	} else {
		HISI_FB_ERR("failed to init lcd!\n");
	}

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_jdi_panel_off(struct platform_device *pdev)
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
	mipi_dsi_cmds_tx(jdi_display_off_cmds, \
		ARRAY_SIZE(jdi_display_off_cmds), hisifd->mipi_dsi0_base);
	if (false == gesture_func) {
		/* lcd gpio lowpower */
		gpio_cmds_tx(jdi_lcd_gpio_lowpower_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_lowpower_cmds));
		/* lcd gpio free */
		gpio_cmds_tx(jdi_lcd_gpio_free_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_free_cmds));

		/* lcd pinctrl lowpower */
		pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_lowpower_cmds,
			ARRAY_SIZE(jdi_lcd_pinctrl_lowpower_cmds));

		//if (g_vddio_type)
		//	ts_power_gpio_disable();
		mdelay(3);
		/* lcd vcc disable */
		vcc_cmds_tx(pdev, jdi_lcd_vcc_disable_cmds,
			ARRAY_SIZE(jdi_lcd_vcc_disable_cmds));
	} else {
		HISI_FB_INFO("display_off (gesture_func:%d)\n", gesture_func);
		/*backlights disable*/
		gpio_cmds_tx(jdi_lcd_gpio_sleep_lp_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_sleep_lp_cmds));

		/* lcd gpio free */
		gpio_cmds_tx(jdi_lcd_gpio_sleep_free_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_sleep_free_cmds));

		/* lcd pinctrl normal */
		pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds,
			ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));
	}
	//if (hisifd->hisi_fb_shutdown) {
	//	ts_power_control_notify(TS_SUSPEND_DEVICE, SHORT_SYNC_TIMEOUT);
	//}
	checksum_enable_ctl = false;

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_jdi_panel_remove(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);

	if (!hisifd) {
		return 0;
	}

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* lcd vcc finit */
	vcc_cmds_tx(pdev, jdi_lcd_vcc_finit_cmds,
		ARRAY_SIZE(jdi_lcd_vcc_finit_cmds));

	/* lcd pinctrl finit */
	pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_finit_cmds,
		ARRAY_SIZE(jdi_lcd_pinctrl_finit_cmds));

	jdi_sysfs_deinit(pdev);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int mipi_jdi_panel_set_backlight(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;

	char payload[2] = {0, 0};
	struct dsi_cmd_desc bl_cmd[] = {
		{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
			sizeof(payload), payload},
	};

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (unlikely(g_debug_enable)) {
		HISI_FB_INFO("Set backlight to %d\n", hisifd->bl_level);
		LOG_JANK_D(JLID_KERNEL_LCD_BACKLIGHT_ON, "JL_KERNEL_LCD_BACKLIGHT_ON,%u", hisifd->bl_level);
		g_debug_enable = false;
	}

	if (hisifd->panel_info.bl_set_type & BL_SET_BY_PWM) {
		ret = hisi_pwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_BLPWM) {
		ret = hisi_blpwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_MIPI) {
		bl_cmd[0].payload[0] = 0x51;
		bl_cmd[0].payload[1] = hisifd->bl_level;

		mipi_dsi_cmds_tx(bl_cmd, ARRAY_SIZE(bl_cmd), hisifd->mipi_dsi0_base);
	} else {
		HISI_FB_ERR("No such bl_set_type!\n");
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	HISI_FB_ERR("mipi_jdi_panel_set_backlight wxh exit %d.\n",bl_cmd[0].payload[1]);

	return ret;
}


/******************************************************************************/
static ssize_t mipi_jdi_panel_lcd_model_show(struct platform_device *pdev,
	char *buf)
{
	struct hisi_fb_data_type *hisifd = NULL;
	ssize_t ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = snprintf(buf, PAGE_SIZE, "jdi_NT36750 6.8' CMD TFT 1080 x 1920\n");

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_jdi_panel_lcd_cabc_mode_show(struct platform_device *pdev,
	char *buf)
{    
	return snprintf(buf, PAGE_SIZE, "%d\n", g_cabc_mode);
}

static ssize_t mipi_jdi_panel_lcd_cabc_mode_store(struct platform_device *pdev,
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

	flag=(int)val;
	if (flag==CABC_OFF ){
              g_cabc_mode=0;
              mipi_dsi_cmds_tx(jdi_cabc_off_cmds, \
                               ARRAY_SIZE(jdi_cabc_off_cmds),\
                               mipi_dsi0_base);
	}else  if (flag==CABC_UI_MODE) {
              g_cabc_mode=1;
              mipi_dsi_cmds_tx(jdi_cabc_ui_on_cmds, \
                               ARRAY_SIZE(jdi_cabc_ui_on_cmds),\
                               mipi_dsi0_base);
	} else if (flag==CABC_STILL_MODE ){
              g_cabc_mode=2;
              mipi_dsi_cmds_tx(jdi_cabc_still_on_cmds, \
                               ARRAY_SIZE(jdi_cabc_still_on_cmds),\
                               mipi_dsi0_base);
	}else if (flag==CABC_MOVING_MODE ){
              g_cabc_mode=3;
              mipi_dsi_cmds_tx(jdi_cabc_moving_on_cmds, \
                               ARRAY_SIZE(jdi_cabc_moving_on_cmds),\
                               mipi_dsi0_base);
	}
	return snprintf((char *)buf, count, "%d\n", g_cabc_mode);
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
#if 0
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
	int error;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	hisifd->lcd_self_testing = true;
	error = ts_power_control_notify(TS_BEFORE_SUSPEND, SHORT_SYNC_TIMEOUT);

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

	//if (!error)
	//	error = ts_power_control_notify(TS_AFTER_RESUME, SHORT_SYNC_TIMEOUT);
	hisifd->lcd_self_testing = false;

	return final_ret;
}
#endif

static char lcd_disp_x[] = {
	0x2A,
	0x00, 0x00,0x04,0x37
};

static char lcd_disp_y[] = {
	0x2B,
	0x00, 0x00,0x07,0x7F
};

static struct dsi_cmd_desc set_display_address[] = {
	{DTYPE_DCS_LWRITE, 0, 5, WAIT_TYPE_US,
		sizeof(lcd_disp_x), lcd_disp_x},
	{DTYPE_DCS_LWRITE, 0, 5, WAIT_TYPE_US,
		sizeof(lcd_disp_y), lcd_disp_y},
};

static int mipi_jdi_panel_set_display_region(struct platform_device *pdev,
	struct dss_rect *dirty)
{
	struct hisi_fb_data_type *hisifd = NULL;
    HISI_FB_ERR("%s enter succ!dss_rect = {%d, %d, %d, %d}\n", __func__, 
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


static struct hisi_panel_info jdi_panel_info = {0};
static struct hisi_fb_panel_data jdi_panel_data = {
	.panel_info = &jdi_panel_info,
	.set_fastboot = mipi_jdi_panel_set_fastboot,
	.on = mipi_jdi_panel_on,
	.off = mipi_jdi_panel_off,
	.remove = mipi_jdi_panel_remove,
	.set_backlight = mipi_jdi_panel_set_backlight,

	.lcd_model_show = mipi_jdi_panel_lcd_model_show,
	.lcd_cabc_mode_show = mipi_jdi_panel_lcd_cabc_mode_show,
	.lcd_cabc_mode_store = mipi_jdi_panel_lcd_cabc_mode_store,
	.lcd_check_reg = mipi_jdi_panel_lcd_check_reg_show,
	.lcd_mipi_detect = mipi_jdi_panel_lcd_mipi_detect_show,
	.lcd_hkadc_debug_show = mipi_jdi_panel_lcd_hkadc_debug_show,
	.lcd_hkadc_debug_store = mipi_jdi_panel_lcd_hkadc_debug_store,
	.lcd_gram_check_show = mipi_jdi_panel_lcd_gram_check_show,
	.lcd_gram_check_store = mipi_jdi_panel_lcd_gram_check_store,
	.lcd_bist_check = NULL,//mipi_jdi_panel_lcd_bist_check,
	.set_display_region = mipi_jdi_panel_set_display_region,
	.set_display_resolution = NULL,
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
	struct platform_device *this_dev = NULL;

	if (hisi_fb_device_probe_defer(PANEL_MIPI_CMD))
		goto err_probe_defer;

	HISI_FB_DEBUG("+.\n");


	HISI_FB_INFO("mipi_jdi_probe 36750 !\n");

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_JDI_NT36750);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_JDI_NT36750);
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
	pinfo = jdi_panel_data.panel_info;
	memset(pinfo, 0, sizeof(struct hisi_panel_info));
	pinfo->xres = 1080;
	pinfo->yres = 1920;
	pinfo->width  = 87;  //mm
	pinfo->height = 157; //mm
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
	pinfo->esd_enable = 0;
	pinfo->dirty_region_updt_support = 1;

    pinfo->dirty_region_info.left_align = -1;
    pinfo->dirty_region_info.right_align = -1;
    pinfo->dirty_region_info.top_align = 8;
    pinfo->dirty_region_info.bottom_align = 8;
    pinfo->dirty_region_info.w_align = -1;
    pinfo->dirty_region_info.h_align = -1;
    pinfo->dirty_region_info.w_min = 1080;
    pinfo->dirty_region_info.h_min = -1;
    pinfo->dirty_region_info.top_start = -1;
    pinfo->dirty_region_info.bottom_start = -1;

	if(runmode_is_factory()) {
		pinfo->sbl_support = 0;
		pinfo->dsi_bit_clk_upt_support = 0;
		pinfo->acm_support = 0;
		pinfo->prefix_sharpness_support = 0;
	} else {
		pinfo->sbl_support = 1;
		pinfo->dsi_bit_clk_upt_support = 1;
		pinfo->acm_support = 0;
		pinfo->prefix_sharpness_support = 0;
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
	pinfo->mipi.dsi_bit_clk_val1 = 471; //470.4
	pinfo->mipi.dsi_bit_clk_val2 = 480;
	pinfo->mipi.dsi_bit_clk_val3 = 490; //489.6
	pinfo->mipi.dsi_bit_clk_val4 = 500; //499.2
	//pinfo->mipi.dsi_bit_clk_val5 = ;
	pinfo->mipi.dsi_bit_clk_upt = pinfo->mipi.dsi_bit_clk;

	pinfo->pxl_clk_rate = 150*1000000UL;

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

	this_dev = hisi_fb_add_device(pdev);
	BUG_ON(this_dev == NULL);
	hisifd_test = platform_get_drvdata(this_dev);
	BUG_ON(hisifd_test == NULL);
	//HISI_FB_INFO("fb%d, dss-dsi0 = 0x%x.\n", hisifd_test->index,hisifd_test->mipi_dsi0_base);
	jdi_sysfs_init(pdev);

	HISI_FB_DEBUG("-.\n");
	
    sysfs_create_link(NULL,&pdev->dev.kobj,"lcd");                                                                                                      
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
        .compatible = DTS_COMP_JDI_NT36750,
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
		.name = "mipi_jdi_NT36750",
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
