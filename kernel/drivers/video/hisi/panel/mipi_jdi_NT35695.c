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
#include "mipi_jdi_NT35695.h"
#include <linux/hisi/hw_cmdline_parse.h>
#include <huawei_platform/log/log_jank.h>

#define DTS_COMP_JDI_NT35695 "hisilicon,mipi_jdi_NT35695"
#define LCD_VDDIO_TYPE_NAME	"lcd-vddio-type"
#define PLK_RF_REG_FLAG		"rf-workaround-flag"
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
static uint32_t g_rf_flag = 0;

extern int fastboot_set_needed;
extern bool gesture_func;
extern void ts_power_gpio_enable(void);
extern void ts_power_gpio_disable(void);

/*******************************************************************************
** Reduce radio interfere Sequence
*/
static char cmd2_page_0_select[] = {
	0XFF,
	0x20,
};

static char cmd2_5c_reg[] = {
	0X5C,
	0x82,
};

static char cmd2_5e_reg[] = {
	0X5E,
	0x02,
};

static char cmd2_page_4_select[] = {
	0XFF,
	0x24,
};

static char cmd2_7f_reg[] = {
	0X7F,
	0x13,
};

static char cmd2_81_reg[] = {
	0X81,
	0x04,
};

static char cmd2_fb_reg[] = {
	0XFB,
	0x01,
};

static struct dsi_cmd_desc jdi_reduce_rf_interfere_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(cmd2_page_0_select), cmd2_page_0_select},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(cmd2_5c_reg), cmd2_5c_reg},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(cmd2_5e_reg), cmd2_5e_reg},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(cmd2_page_4_select), cmd2_page_4_select},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(cmd2_7f_reg), cmd2_7f_reg},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(cmd2_81_reg), cmd2_81_reg},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(cmd2_fb_reg), cmd2_fb_reg},
};

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

static char page_E_select[] = {
	0XFF,
	0xE0,
};

static char sram_solution_TOPT[] = {
	0xB5,
	0x86,
};

static char sram_solution_WESOPT[] = {
	0xB6,
	0x77,
};

static char sram_solution_pre_VDD[] = {
	0XB8,
	0xAD,
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
	0x24,
};

static char te_line[] = {
	0x44,
	0x03, 0x80,
};

/*******************************************************************************
** setting PWM frequency to 58KHZ
*/
static char set_page3[] = {
	0xFF,
	0x23,
};

static char set_pwm_freq[] = {
	0x08,
	0x03,
};

static char set_page0[] = {
	0xFF,
	0x10,
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

static struct dsi_cmd_desc jdi_display_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(tear_on), tear_on},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(caset_data), caset_data},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(paset_data), paset_data},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(set_page3), set_page3},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(non_reload), non_reload},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(set_pwm_freq), set_pwm_freq},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(set_page0), set_page0},
	{DTYPE_DCS_LWRITE, 0,200, WAIT_TYPE_US,
		sizeof(page_E_select), page_E_select},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(sram_solution_TOPT), sram_solution_TOPT},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(sram_solution_WESOPT), sram_solution_WESOPT},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(sram_solution_pre_VDD), sram_solution_pre_VDD},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
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
static char page_selection_0xFF_0x20[] = {
	0xFF,
	0x20,
};

static char gamma_dither_0x68[] = {
	0x68,
	0x03,
};

//R(+) MCR cmd
static char gamma_r_positive_0x75[] = {
	0x75,
	0x00,
};

static char gamma_r_positive_0x76[] = {
	0x76,
	0x08,
};

static char gamma_r_positive_0x77[] = {
	0x77,
	0x00,
};

static char gamma_r_positive_0x78[] = {
	0x78,
	0x1F,
};

static char gamma_r_positive_0x79[] = {
	0x79,
	0x00,
};

static char gamma_r_positive_0x7A[] = {
	0x7A,
	0x46,
};

static char gamma_r_positive_0x7B[] = {
	0x7B,
	0x00,
};

static char gamma_r_positive_0x7C[] = {
	0x7C,
	0x61,
};

static char gamma_r_positive_0x7D[] = {
	0x7D,
	0x00,
};

static char gamma_r_positive_0x7E[] = {
	0x7E,
	0x7A,
};

static char gamma_r_positive_0x7F[] = {
	0x7F,
	0x00,
};

static char gamma_r_positive_0x80[] = {
	0x80,
	0x96,
};

static char gamma_r_positive_0x81[] = {
	0x81,
	0x00,
};

static char gamma_r_positive_0x82[] = {
	0x82,
	0xAC,
};

static char gamma_r_positive_0x83[] = {
	0x83,
	0x00,
};

static char gamma_r_positive_0x84[] = {
	0x84,
	0xC3,
};

static char gamma_r_positive_0x85[] = {
	0x85,
	0x00,
};

static char gamma_r_positive_0x86[] = {
	0x86,
	0xD4,
};

static char gamma_r_positive_0x87[] = {
	0x87,
	0x01,
};

static char gamma_r_positive_0x88[] = {
	0x88,
	0x0E,
};

static char gamma_r_positive_0x89[] = {
	0x89,
	0x01,
};

static char gamma_r_positive_0x8A[] = {
	0x8A,
	0x3A,
};

static char gamma_r_positive_0x8B[] = {
	0x8B,
	0x01,
};

static char gamma_r_positive_0x8C[] = {
	0x8C,
	0x83,
};

static char gamma_r_positive_0x8D[] = {
	0x8D,
	0x01,
};

static char gamma_r_positive_0x8E[] = {
	0x8E,
	0xB8,
};

static char gamma_r_positive_0x8F[] = {
	0x8F,
	0x02,
};

static char gamma_r_positive_0x90[] = {
	0x90,
	0x07,
};

static char gamma_r_positive_0x91[] = {
	0x91,
	0x02,
};

static char gamma_r_positive_0x92[] = {
	0x92,
	0x48,
};

static char gamma_r_positive_0x93[] = {
	0x93,
	0x02,
};

static char gamma_r_positive_0x94[] = {
	0x94,
	0x49,
};

static char gamma_r_positive_0x95[] = {
	0x95,
	0x02,
};

static char gamma_r_positive_0x96[] = {
	0x96,
	0x88,
};

static char gamma_r_positive_0x97[] = {
	0x97,
	0x02,
};

static char gamma_r_positive_0x98[] = {
	0x98,
	0xCA,
};

static char gamma_r_positive_0x99[] = {
	0x99,
	0x02,
};

static char gamma_r_positive_0x9A[] = {
	0x9A,
	0xF1,
};

static char gamma_r_positive_0x9B[] = {
	0x9B,
	0x03,
};

static char gamma_r_positive_0x9C[] = {
	0x9C,
	0x23,
};

static char gamma_r_positive_0x9D[] = {
	0x9D,
	0x03,
};

static char gamma_r_positive_0x9E[] = {
	0x9E,
	0x41,
};

static char gamma_r_positive_0x9F[] = {
	0x9F,
	0x03,
};

static char gamma_r_positive_0xA0[] = {
	0xA0,
	0x67,
};

static char gamma_r_positive_0xA2[] = {
	0xA2,
	0x03,
};

static char gamma_r_positive_0xA3[] = {
	0xA3,
	0x72,
};

static char gamma_r_positive_0xA4[] = {
	0xA4,
	0x03,
};

static char gamma_r_positive_0xA5[] = {
	0xA5,
	0x7F,
};

static char gamma_r_positive_0xA6[] = {
	0xA6,
	0x03,
};

static char gamma_r_positive_0xA7[] = {
	0xA7,
	0x8D,
};

static char gamma_r_positive_0xA9[] = {
	0xA9,
	0x03,
};

static char gamma_r_positive_0xAA[] = {
	0xAA,
	0x9D,
};

static char gamma_r_positive_0xAB[] = {
	0xAB,
	0x03,
};

static char gamma_r_positive_0xAC[] = {
	0xAC,
	0xAF,
};

static char gamma_r_positive_0xAD[] = {
	0xAD,
	0x03,
};

static char gamma_r_positive_0xAE[] = {
	0xAE,
	0xC8,
};

static char gamma_r_positive_0xAF[] = {
	0xAF,
	0x03,
};

static char gamma_r_positive_0xB0[] = {
	0xB0,
	0xF2,
};

static char gamma_r_positive_0xB1[] = {
	0xB1,
	0x03,
};

static char gamma_r_positive_0xB2[] = {
	0xB2,
	0xF3,
};

//R(-) MCR cmd
static char gamma_r_negative_0xB3[] = {
	0xB3,
	0x00,
};

static char gamma_r_negative_0xB4[] = {
	0xB4,
	0x08,
};

static char gamma_r_negative_0xB5[] = {
	0xB5,
	0x00,
};

static char gamma_r_negative_0xB6[] = {
	0xB6,
	0x1F,
};

static char gamma_r_negative_0xB7[] = {
	0xB7,
	0x00,
};

static char gamma_r_negative_0xB8[] = {
	0xB8,
	0x46,
};

static char gamma_r_negative_0xB9[] = {
	0xB9,
	0x00,
};

static char gamma_r_negative_0xBA[] = {
	0xBA,
	0x61,
};

static char gamma_r_negative_0xBB[] = {
	0xBB,
	0x00,
};

static char gamma_r_negative_0xBC[] = {
	0xBC,
	0x7A,
};

static char gamma_r_negative_0xBD[] = {
	0xBD,
	0x00,
};

static char gamma_r_negative_0xBE[] = {
	0xBE,
	0x96,
};

static char gamma_r_negative_0xBF[] = {
	0xBF,
	0x00,
};

static char gamma_r_negative_0xC0[] = {
	0xC0,
	0xAC,
};

static char gamma_r_negative_0xC1[] = {
	0xC1,
	0x00,
};

static char gamma_r_negative_0xC2[] = {
	0xC2,
	0xC3,
};

static char gamma_r_negative_0xC3[] = {
	0xC3,
	0x00,
};

static char gamma_r_negative_0xC4[] = {
	0xC4,
	0xD4,
};

static char gamma_r_negative_0xC5[] = {
	0xC5,
	0x01,
};

static char gamma_r_negative_0xC6[] = {
	0xC6,
	0x0E,
};

static char gamma_r_negative_0xC7[] = {
	0xC7,
	0x01,
};

static char gamma_r_negative_0xC8[] = {
	0xC8,
	0x3A,
};

static char gamma_r_negative_0xC9[] = {
	0xC9,
	0x01,
};

static char gamma_r_negative_0xCA[] = {
	0xCA,
	0x83,
};

static char gamma_r_negative_0xCB[] = {
	0xCB,
	0x01,
};

static char gamma_r_negative_0xCC[] = {
	0xCC,
	0xB8,
};

static char gamma_r_negative_0xCD[] = {
	0xCD,
	0x02,
};

static char gamma_r_negative_0xCE[] = {
	0xCE,
	0x07,
};

static char gamma_r_negative_0xCF[] = {
	0xCF,
	0x02,
};

static char gamma_r_negative_0xD0[] = {
	0xD0,
	0x48,
};

static char gamma_r_negative_0xD1[] = {
	0xD1,
	0x02,
};

static char gamma_r_negative_0xD2[] = {
	0xD2,
	0x49,
};

static char gamma_r_negative_0xD3[] = {
	0xD3,
	0x02,
};

static char gamma_r_negative_0xD4[] = {
	0xD4,
	0x88,
};

static char gamma_r_negative_0xD5[] = {
	0xD5,
	0x02,
};

static char gamma_r_negative_0xD6[] = {
	0xD6,
	0xCA,
};

static char gamma_r_negative_0xD7[] = {
	0xD7,
	0x02,
};

static char gamma_r_negative_0xD8[] = {
	0xD8,
	0xF1,
};

static char gamma_r_negative_0xD9[] = {
	0xD9,
	0x03,
};

static char gamma_r_negative_0xDA[] = {
	0xDA,
	0x23,
};

static char gamma_r_negative_0xDB[] = {
	0xDB,
	0x03,
};

static char gamma_r_negative_0xDC[] = {
	0xDC,
	0x41,
};

static char gamma_r_negative_0xDD[] = {
	0xDD,
	0x03,
};

static char gamma_r_negative_0xDE[] = {
	0xDE,
	0x67,
};

static char gamma_r_negative_0xDF[] = {
	0xDF,
	0x03,
};

static char gamma_r_negative_0xE0[] = {
	0xE0,
	0x72,
};

static char gamma_r_negative_0xE1[] = {
	0xE1,
	0x03,
};

static char gamma_r_negative_0xE2[] = {
	0xE2,
	0x7F,
};

static char gamma_r_negative_0xE3[] = {
	0xE3,
	0x03,
};

static char gamma_r_negative_0xE4[] = {
	0xE4,
	0x8D,
};

static char gamma_r_negative_0xE5[] = {
	0xE5,
	0x03,
};

static char gamma_r_negative_0xE6[] = {
	0xE6,
	0x9D,
};

static char gamma_r_negative_0xE7[] = {
	0xE7,
	0x03,
};

static char gamma_r_negative_0xE8[] = {
	0xE8,
	0xAF,
};

static char gamma_r_negative_0xE9[] = {
	0xE9,
	0x03,
};

static char gamma_r_negative_0xEA[] = {
	0xEA,
	0xC8,
};

static char gamma_r_negative_0xEB[] = {
	0xEB,
	0x03,
};

static char gamma_r_negative_0xEC[] = {
	0xEC,
	0xF2,
};

static char gamma_r_negative_0xED[] = {
	0xED,
	0x03,
};

static char gamma_r_negative_0xEE[] = {
	0xEE,
	0xF3,
};

//G(+) MCR cmd
static char gamma_g_positive_0xEF[] = {
	0xEF,
	0x00,
};

static char gamma_g_positive_0xF0[] = {
	0xF0,
	0x45,
};

static char gamma_g_positive_0xF1[] = {
	0xF1,
	0x00,
};

static char gamma_g_positive_0xF2[] = {
	0xF2,
	0x56,
};

static char gamma_g_positive_0xF3[] = {
	0xF3,
	0x00,
};

static char gamma_g_positive_0xF4[] = {
	0xF4,
	0x75,
};

static char gamma_g_positive_0xF5[] = {
	0xF5,
	0x00,
};

static char gamma_g_positive_0xF6[] = {
	0xF6,
	0x90,
};

static char gamma_g_positive_0xF7[] = {
	0xF7,
	0x00,
};

static char gamma_g_positive_0xF8[] = {
	0xF8,
	0xA7,
};

static char gamma_g_positive_0xF9[] = {
	0xF9,
	0x00,
};

static char gamma_g_positive_0xFA[] = {
	0xFA,
	0xBC,
};

//page selection cmd start
static char page_selection_0xFF_0x21[] = {
	0xFF,
	0x21,
};

//G(+) MCR cmd
static char gamma_g_positive_0x00[] = {
	0x00,
	0x00,
};

static char gamma_g_positive_0x01[] = {
	0x01,
	0xCF,
};

static char gamma_g_positive_0x02[] = {
	0x02,
	0x00,
};

static char gamma_g_positive_0x03[] = {
	0x03,
	0xDF,
};

static char gamma_g_positive_0x04[] = {
	0x04,
	0x00,
};

static char gamma_g_positive_0x05[] = {
	0x05,
	0xEF,
};

static char gamma_g_positive_0x06[] = {
	0x06,
	0x01,
};

static char gamma_g_positive_0x07[] = {
	0x07,
	0x24,
};

static char gamma_g_positive_0x08[] = {
	0x08,
	0x01,
};

static char gamma_g_positive_0x09[] = {
	0x09,
	0x4E,
};

static char gamma_g_positive_0x0A[] = {
	0x0A,
	0x01,
};

static char gamma_g_positive_0x0B[] = {
	0x0B,
	0x8E,
};

static char gamma_g_positive_0x0C[] = {
	0x0C,
	0x01,
};

static char gamma_g_positive_0x0D[] = {
	0x0D,
	0xC0,
};

static char gamma_g_positive_0x0E[] = {
	0x0E,
	0x02,
};

static char gamma_g_positive_0x0F[] = {
	0x0F,
	0x0D,
};

static char gamma_g_positive_0x10[] = {
	0x10,
	0x02,
};

static char gamma_g_positive_0x11[] = {
	0x11,
	0x4C,
};

static char gamma_g_positive_0x12[] = {
	0x12,
	0x02,
};

static char gamma_g_positive_0x13[] = {
	0x13,
	0x4E,
};

static char gamma_g_positive_0x14[] = {
	0x14,
	0x02,
};

static char gamma_g_positive_0x15[] = {
	0x15,
	0x89,
};

static char gamma_g_positive_0x16[] = {
	0x16,
	0x02,
};

static char gamma_g_positive_0x17[] = {
	0x17,
	0xCB,
};

static char gamma_g_positive_0x18[] = {
	0x18,
	0x02,
};

static char gamma_g_positive_0x19[] = {
	0x19,
	0xF3,
};

static char gamma_g_positive_0x1A[] = {
	0x1A,
	0x03,
};

static char gamma_g_positive_0x1B[] = {
	0x1B,
	0x24,
};

static char gamma_g_positive_0x1C[] = {
	0x1C,
	0x03,
};

static char gamma_g_positive_0x1D[] = {
	0x1D,
	0x42,
};

static char gamma_g_positive_0x1E[] = {
	0x1E,
	0x03,
};

static char gamma_g_positive_0x1F[] = {
	0x1F,
	0x68,
};

static char gamma_g_positive_0x20[] = {
	0x20,
	0x03,
};

static char gamma_g_positive_0x21[] = {
	0x21,
	0x73,
};

static char gamma_g_positive_0x22[] = {
	0x22,
	0x03,
};

static char gamma_g_positive_0x23[] = {
	0x23,
	0x7F,
};

static char gamma_g_positive_0x24[] = {
	0x24,
	0x03,
};

static char gamma_g_positive_0x25[] = {
	0x25,
	0x8D,
};

static char gamma_g_positive_0x26[] = {
	0x26,
	0x03,
};

static char gamma_g_positive_0x27[] = {
	0x27,
	0x9D,
};

static char gamma_g_positive_0x28[] = {
	0x28,
	0x03,
};

static char gamma_g_positive_0x29[] = {
	0x29,
	0xAF,
};

static char gamma_g_positive_0x2A[] = {
	0x2A,
	0x03,
};

static char gamma_g_positive_0x2B[] = {
	0x2B,
	0xC4,
};

static char gamma_g_positive_0x2D[] = {
	0x2D,
	0x03,
};

static char gamma_g_positive_0x2F[] = {
	0x2F,
	0xED,
};

static char gamma_g_positive_0x30[] = {
	0x30,
	0x03,
};

static char gamma_g_positive_0x31[] = {
	0x31,
	0xEE,
};

//G(-) MCR cmd
static char gamma_g_negative_0x32[] = {
	0x32,
	0x00,
};

static char gamma_g_negative_0x33[] = {
	0x33,
	0x45,
};

static char gamma_g_negative_0x34[] = {
	0x34,
	0x00,
};

static char gamma_g_negative_0x35[] = {
	0x35,
	0x56,
};

static char gamma_g_negative_0x36[] = {
	0x36,
	0x00,
};

static char gamma_g_negative_0x37[] = {
	0x37,
	0x75,
};

static char gamma_g_negative_0x38[] = {
	0x38,
	0x00,
};

static char gamma_g_negative_0x39[] = {
	0x39,
	0x90,
};

static char gamma_g_negative_0x3A[] = {
	0x3A,
	0x00,
};

static char gamma_g_negative_0x3B[] = {
	0x3B,
	0xA7,
};

static char gamma_g_negative_0x3D[] = {
	0x3D,
	0x00,
};

static char gamma_g_negative_0x3F[] = {
	0x3F,
	0xBC,
};

static char gamma_g_negative_0x40[] = {
	0x40,
	0x00,
};

static char gamma_g_negative_0x41[] = {
	0x41,
	0xCF,
};

static char gamma_g_negative_0x42[] = {
	0x42,
	0x00,
};

static char gamma_g_negative_0x43[] = {
	0x43,
	0xDF,
};

static char gamma_g_negative_0x44[] = {
	0x44,
	0x00,
};

static char gamma_g_negative_0x45[] = {
	0x45,
	0xEF,
};

static char gamma_g_negative_0x46[] = {
	0x46,
	0x01,
};

static char gamma_g_negative_0x47[] = {
	0x47,
	0x24,
};

static char gamma_g_negative_0x48[] = {
	0x48,
	0x01,
};

static char gamma_g_negative_0x49[] = {
	0x49,
	0x4E,
};

static char gamma_g_negative_0x4A[] = {
	0x4A,
	0x01,
};

static char gamma_g_negative_0x4B[] = {
	0x4B,
	0x8E,
};

static char gamma_g_negative_0x4C[] = {
	0x4C,
	0x01,
};

static char gamma_g_negative_0x4D[] = {
	0x4D,
	0xC0,
};

static char gamma_g_negative_0x4E[] = {
	0x4E,
	0x02,
};

static char gamma_g_negative_0x4F[] = {
	0x4F,
	0x0D,
};

static char gamma_g_negative_0x50[] = {
	0x50,
	0x02,
};

static char gamma_g_negative_0x51[] = {
	0x51,
	0x4C,
};

static char gamma_g_negative_0x52[] = {
	0x52,
	0x02,
};

static char gamma_g_negative_0x53[] = {
	0x53,
	0x4E,
};

static char gamma_g_negative_0x54[] = {
	0x54,
	0x02,
};

static char gamma_g_negative_0x55[] = {
	0x55,
	0x89,
};

static char gamma_g_negative_0x56[] = {
	0x56,
	0x02,
};

static char gamma_g_negative_0x58[] = {
	0x58,
	0xCB,
};

static char gamma_g_negative_0x59[] = {
	0x59,
	0x02,
};

static char gamma_g_negative_0x5A[] = {
	0x5A,
	0xF3,
};

static char gamma_g_negative_0x5B[] = {
	0x5B,
	0x03,
};

static char gamma_g_negative_0x5C[] = {
	0x5C,
	0x24,
};

static char gamma_g_negative_0x5D[] = {
	0x5D,
	0x03,
};

static char gamma_g_negative_0x5E[] = {
	0x5E,
	0x42,
};

static char gamma_g_negative_0x5F[] = {
	0x5F,
	0x03,
};

static char gamma_g_negative_0x60[] = {
	0x60,
	0x68,
};

static char gamma_g_negative_0x61[] = {
	0x61,
	0x03,
};

static char gamma_g_negative_0x62[] = {
	0x62,
	0x73,
};

static char gamma_g_negative_0x63[] = {
	0x63,
	0x03,
};

static char gamma_g_negative_0x64[] = {
	0x64,
	0x7F,
};

static char gamma_g_negative_0x65[] = {
	0x65,
	0x03,
};

static char gamma_g_negative_0x66[] = {
	0x66,
	0x8D,
};

static char gamma_g_negative_0x67[] = {
	0x67,
	0x03,
};

static char gamma_g_negative_0x68[] = {
	0x68,
	0x9D,
};

static char gamma_g_negative_0x69[] = {
	0x69,
	0x03,
};

static char gamma_g_negative_0x6A[] = {
	0x6A,
	0xAF,
};

static char gamma_g_negative_0x6B[] = {
	0x6B,
	0x03,
};

static char gamma_g_negative_0x6C[] = {
	0x6C,
	0xC4,
};

static char gamma_g_negative_0x6D[] = {
	0x6D,
	0x03,
};

static char gamma_g_negative_0x6E[] = {
	0x6E,
	0xED,
};

static char gamma_g_negative_0x6F[] = {
	0x6F,
	0x03,
};

static char gamma_g_negative_0x70[] = {
	0x70,
	0xEE,
};

//B(+) MCR cmd
static char gamma_b_positive_0x71[] = {
	0x71,
	0x01,
};

static char gamma_b_positive_0x72[] = {
	0x72,
	0x19,
};

static char gamma_b_positive_0x73[] = {
	0x73,
	0x01,
};

static char gamma_b_positive_0x74[] = {
	0x74,
	0x1F,
};

static char gamma_b_positive_0x75[] = {
	0x75,
	0x01,
};

static char gamma_b_positive_0x76[] = {
	0x76,
	0x2B,
};

static char gamma_b_positive_0x77[] = {
	0x77,
	0x01,
};

static char gamma_b_positive_0x78[] = {
	0x78,
	0x35,
};

static char gamma_b_positive_0x79[] = {
	0x79,
	0x01,
};

static char gamma_b_positive_0x7A[] = {
	0x7A,
	0x3F,
};

static char gamma_b_positive_0x7B[] = {
	0x7B,
	0x01,
};

static char gamma_b_positive_0x7C[] = {
	0x7C,
	0x48,
};

static char gamma_b_positive_0x7D[] = {
	0x7D,
	0x01,
};

static char gamma_b_positive_0x7E[] = {
	0x7E,
	0x52,
};

static char gamma_b_positive_0x7F[] = {
	0x7F,
	0x01,
};

static char gamma_b_positive_0x80[] = {
	0x80,
	0x5B,
};

static char gamma_b_positive_0x81[] = {
	0x81,
	0x01,
};

static char gamma_b_positive_0x82[] = {
	0x82,
	0x63,
};

static char gamma_b_positive_0x83[] = {
	0x83,
	0x01,
};

static char gamma_b_positive_0x84[] = {
	0x84,
	0x81,
};

static char gamma_b_positive_0x85[] = {
	0x85,
	0x01,
};

static char gamma_b_positive_0x86[] = {
	0x86,
	0x9C,
};

static char gamma_b_positive_0x87[] = {
	0x87,
	0x01,
};

static char gamma_b_positive_0x88[] = {
	0x88,
	0xC9,
};

static char gamma_b_positive_0x89[] = {
	0x89,
	0x01,
};

static char gamma_b_positive_0x8A[] = {
	0x8A,
	0xEE,
};

static char gamma_b_positive_0x8B[] = {
	0x8B,
	0x02,
};

static char gamma_b_positive_0x8C[] = {
	0x8C,
	0x2D,
};

static char gamma_b_positive_0x8D[] = {
	0x8D,
	0x02,
};

static char gamma_b_positive_0x8E[] = {
	0x8E,
	0x63,
};

static char gamma_b_positive_0x8F[] = {
	0x8F,
	0x02,
};

static char gamma_b_positive_0x90[] = {
	0x90,
	0x65,
};

static char gamma_b_positive_0x91[] = {
	0x91,
	0x02,
};

static char gamma_b_positive_0x92[] = {
	0x92,
	0x9C,
};

static char gamma_b_positive_0x93[] = {
	0x93,
	0x02,
};

static char gamma_b_positive_0x94[] = {
	0x94,
	0xDB,
};

static char gamma_b_positive_0x95[] = {
	0x95,
	0x03,
};

static char gamma_b_positive_0x96[] = {
	0x96,
	0x01,
};

static char gamma_b_positive_0x97[] = {
	0x97,
	0x03,
};

static char gamma_b_positive_0x98[] = {
	0x98,
	0x32,
};

static char gamma_b_positive_0x99[] = {
	0x99,
	0x03,
};

static char gamma_b_positive_0x9A[] = {
	0x9A,
	0x51,
};

static char gamma_b_positive_0x9B[] = {
	0x9B,
	0x03,
};

static char gamma_b_positive_0x9C[] = {
	0x9C,
	0x7E,
};

static char gamma_b_positive_0x9D[] = {
	0x9D,
	0x03,
};

static char gamma_b_positive_0x9E[] = {
	0x9E,
	0x8E,
};

static char gamma_b_positive_0x9F[] = {
	0x9F,
	0x03,
};

static char gamma_b_positive_0xA0[] = {
	0xA0,
	0xA3,
};

static char gamma_b_positive_0xA2[] = {
	0xA2,
	0x03,
};

static char gamma_b_positive_0xA3[] = {
	0xA3,
	0xB2,
};

static char gamma_b_positive_0xA4[] = {
	0xA4,
	0x03,
};

static char gamma_b_positive_0xA5[] = {
	0xA5,
	0xBE,
};

static char gamma_b_positive_0xA6[] = {
	0xA6,
	0x03,
};

static char gamma_b_positive_0xA7[] = {
	0xA7,
	0xC3,
};

static char gamma_b_positive_0xA9[] = {
	0xA9,
	0x03,
};

static char gamma_b_positive_0xAA[] = {
	0xAA,
	0xD1,
};

static char gamma_b_positive_0xAB[] = {
	0xAB,
	0x03,
};

static char gamma_b_positive_0xAC[] = {
	0xAC,
	0xD9,
};

static char gamma_b_positive_0xAD[] = {
	0xAD,
	0x03,
};

static char gamma_b_positive_0xAE[] = {
	0xAE,
	0xDA,
};

//B(-) MCR cmd
static char gamma_b_negative_0xAF[] = {
	0xAF,
	0x01,
};

static char gamma_b_negative_0xB0[] = {
	0xB0,
	0x19,
};

static char gamma_b_negative_0xB1[] = {
	0xB1,
	0x01,
};

static char gamma_b_negative_0xB2[] = {
	0xB2,
	0x1F,
};

static char gamma_b_negative_0xB3[] = {
	0xB3,
	0x01,
};

static char gamma_b_negative_0xB4[] = {
	0xB4,
	0x2B,
};

static char gamma_b_negative_0xB5[] = {
	0xB5,
	0x01,
};

static char gamma_b_negative_0xB6[] = {
	0xB6,
	0x35,
};

static char gamma_b_negative_0xB7[] = {
	0xB7,
	0x01,
};

static char gamma_b_negative_0xB8[] = {
	0xB8,
	0x3F,
};

static char gamma_b_negative_0xB9[] = {
	0xB9,
	0x01,
};

static char gamma_b_negative_0xBA[] = {
	0xBA,
	0x48,
};

static char gamma_b_negative_0xBB[] = {
	0xBB,
	0x01,
};

static char gamma_b_negative_0xBC[] = {
	0xBC,
	0x52,
};

static char gamma_b_negative_0xBD[] = {
	0xBD,
	0x01,
};

static char gamma_b_negative_0xBE[] = {
	0xBE,
	0x5B,
};

static char gamma_b_negative_0xBF[] = {
	0xBF,
	0x01,
};

static char gamma_b_negative_0xC0[] = {
	0xC0,
	0x63,
};

static char gamma_b_negative_0xC1[] = {
	0xC1,
	0x01,
};

static char gamma_b_negative_0xC2[] = {
	0xC2,
	0x81,
};

static char gamma_b_negative_0xC3[] = {
	0xC3,
	0x01,
};

static char gamma_b_negative_0xC4[] = {
	0xC4,
	0x9C,
};

static char gamma_b_negative_0xC5[] = {
	0xC5,
	0x01,
};

static char gamma_b_negative_0xC6[] = {
	0xC6,
	0xC9,
};

static char gamma_b_negative_0xC7[] = {
	0xC7,
	0x01,
};

static char gamma_b_negative_0xC8[] = {
	0xC8,
	0xEE,
};

static char gamma_b_negative_0xC9[] = {
	0xC9,
	0x02,
};

static char gamma_b_negative_0xCA[] = {
	0xCA,
	0x2D,
};

static char gamma_b_negative_0xCB[] = {
	0xCB,
	0x02,
};

static char gamma_b_negative_0xCC[] = {
	0xCC,
	0x63,
};

static char gamma_b_negative_0xCD[] = {
	0xCD,
	0x02,
};

static char gamma_b_negative_0xCE[] = {
	0xCE,
	0x65,
};

static char gamma_b_negative_0xCF[] = {
	0xCF,
	0x02,
};

static char gamma_b_negative_0xD0[] = {
	0xD0,
	0x9C,
};

static char gamma_b_negative_0xD1[] = {
	0xD1,
	0x02,
};

static char gamma_b_negative_0xD2[] = {
	0xD2,
	0xDB,
};

static char gamma_b_negative_0xD3[] = {
	0xD3,
	0x03,
};

static char gamma_b_negative_0xD4[] = {
	0xD4,
	0x01,
};

static char gamma_b_negative_0xD5[] = {
	0xD5,
	0x03,
};

static char gamma_b_negative_0xD6[] = {
	0xD6,
	0x32,
};

static char gamma_b_negative_0xD7[] = {
	0xD7,
	0x03,
};

static char gamma_b_negative_0xD8[] = {
	0xD8,
	0x51,
};

static char gamma_b_negative_0xD9[] = {
	0xD9,
	0x03,
};

static char gamma_b_negative_0xDA[] = {
	0xDA,
	0x7E,
};

static char gamma_b_negative_0xDB[] = {
	0xDB,
	0x03,
};

static char gamma_b_negative_0xDC[] = {
	0xDC,
	0x8E,
};

static char gamma_b_negative_0xDD[] = {
	0xDD,
	0x03,
};

static char gamma_b_negative_0xDE[] = {
	0xDE,
	0xA3,
};

static char gamma_b_negative_0xDF[] = {
	0xDF,
	0x03,
};

static char gamma_b_negative_0xE0[] = {
	0xE0,
	0xB2,
};

static char gamma_b_negative_0xE1[] = {
	0xE1,
	0x03,
};

static char gamma_b_negative_0xE2[] = {
	0xE2,
	0xBE,
};

static char gamma_b_negative_0xE3[] = {
	0xE3,
	0x03,
};

static char gamma_b_negative_0xE4[] = {
	0xE4,
	0xC3,
};

static char gamma_b_negative_0xE5[] = {
	0xE5,
	0x03,
};

static char gamma_b_negative_0xE6[] = {
	0xE6,
	0xD1,
};

static char gamma_b_negative_0xE7[] = {
	0xE7,
	0x03,
};

static char gamma_b_negative_0xE8[] = {
	0xE8,
	0xD9,
};

static char gamma_b_negative_0xE9[] = {
	0xE9,
	0x03,
};

static char gamma_b_negative_0xEA[] = {
	0xEA,
	0xDA,
};

static char page_selection_0xFF_0x22[] = {
	0xFF,
	0x22,
};

static char color_enhancement_0x00[] = {
	0x00,
	0x50,
};

static char color_enhancement_0x01[] = {
	0x01,
	0x50,
};

static char color_enhancement_0x02[] = {
	0x02,
	0x50,
};

static char color_enhancement_0x03[] = {
	0x03,
	0x50,
};

static char color_enhancement_0x04[] = {
	0x04,
	0x50,
};

static char color_enhancement_0x05[] = {
	0x05,
	0x50,
};

static char color_enhancement_0x06[] = {
	0x06,
	0x4C,
};

static char color_enhancement_0x07[] = {
	0x07,
	0x48,
};

static char color_enhancement_0x08[] = {
	0x08,
	0x44,
};

static char color_enhancement_0x09[] = {
	0x09,
	0x40,
};

static char color_enhancement_0x0A[] = {
	0x0A,
	0x3C,
};

static char color_enhancement_0x0B[] = {
	0x0B,
	0x38,
};

static char color_enhancement_0x0C[] = {
	0x0C,
	0x38,
};

static char color_enhancement_0x0D[] = {
	0x0D,
	0x30,
};

static char color_enhancement_0x0E[] = {
	0x0E,
	0x28,
};

static char color_enhancement_0x0F[] = {
	0x0F,
	0x20,
};

static char color_enhancement_0x10[] = {
	0x10,
	0x10,
};

static char color_enhancement_0x11[] = {
	0x11,
	0x00,
};

static char color_enhancement_0x12[] = {
	0x12,
	0x00,
};

static char color_enhancement_0x13[] = {
	0x13,
	0x00,
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
	0x80,
};

static char color_enhancement_0x33[] = {
	0x33,
	0x80,
};

static char color_enhancement_0x34[] = {
	0x34,
	0x80,
};

static char color_enhancement_0x35[] = {
	0x35,
	0x83,
};

static char color_enhancement_0x36[] = {
	0x36,
	0x85,
};

static char color_enhancement_0x37[] = {
	0x37,
	0x88,
};

static char color_enhancement_0x38[] = {
	0x38,
	0x8A,
};

static char color_enhancement_0x39[] = {
	0x39,
	0x8B,
};

static char color_enhancement_0x3A[] = {
	0x3A,
	0x8B,
};

static char color_enhancement_0x3B[] = {
	0x3B,
	0x8D,
};

static char color_enhancement_0x3F[] = {
	0x3F,
	0x90,
};

static char color_enhancement_0x40[] = {
	0x40,
	0x8D,
};

static char color_enhancement_0x41[] = {
	0x41,
	0x8A,
};

static char color_enhancement_0x42[] = {
	0x42,
	0x87,
};

static char color_enhancement_0x43[] = {
	0x43,
	0x8A,
};

static char color_enhancement_0x44[] = {
	0x44,
	0x8D,
};

static char color_enhancement_0x45[] = {
	0x45,
	0x90,
};

static char color_enhancement_0x46[] = {
	0x46,
	0x92,
};

static char color_enhancement_0x47[] = {
	0x47,
	0x8F,
};

static char color_enhancement_0x48[] = {
	0x48,
	0x8C,
};

static char color_enhancement_0x49[] = {
	0x49,
	0x89,
};

static char color_enhancement_0x4A[] = {
	0x4A,
	0x86,
};

static char color_enhancement_0x4B[] = {
	0x4B,
	0x83,
};

static char color_enhancement_0x4C[] = {
	0x4C,
	0x80,
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
	0x0a,
};

static char vivid_color_enable_0x1A[] = {
	0x1A,
	0x00,
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
	0x90,
};

static char cabc_set_mode_UI[] = {
	0x55,
	0x91,
};

static char cabc_set_mode_STILL[] = {
	0x55,
	0x92,
};

static char cabc_set_mode_MOVING[] = {
	0x55,
	0x93,
};

#ifdef CONFIG_BACKLIGHT_10000
static char PWM_OUT_0x51[] = {
	0x51,
	0xFE,
};
#endif

static struct dsi_cmd_desc jdi_display_effect_on_cmds[] = {
	//diplay effect
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(page_selection_0xFF_0x20), page_selection_0xFF_0x20},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof( non_reload_0xFB),  non_reload_0xFB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_dither_0x68), gamma_dither_0x68},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x75), gamma_r_positive_0x75},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x76), gamma_r_positive_0x76},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x77), gamma_r_positive_0x77},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x78), gamma_r_positive_0x78},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x79), gamma_r_positive_0x79},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x7A), gamma_r_positive_0x7A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x7B), gamma_r_positive_0x7B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x7C), gamma_r_positive_0x7C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x7D), gamma_r_positive_0x7D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x7E), gamma_r_positive_0x7E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x7F), gamma_r_positive_0x7F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x80), gamma_r_positive_0x80},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x81), gamma_r_positive_0x81},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x82), gamma_r_positive_0x82},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x83), gamma_r_positive_0x83},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x84), gamma_r_positive_0x84},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x85), gamma_r_positive_0x85},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x86), gamma_r_positive_0x86},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x87), gamma_r_positive_0x87},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x88), gamma_r_positive_0x88},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x89), gamma_r_positive_0x89},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x8A), gamma_r_positive_0x8A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x8B), gamma_r_positive_0x8B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x8C), gamma_r_positive_0x8C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x8D), gamma_r_positive_0x8D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x8E), gamma_r_positive_0x8E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x8F), gamma_r_positive_0x8F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x90), gamma_r_positive_0x90},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x91), gamma_r_positive_0x91},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x92), gamma_r_positive_0x92},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x93), gamma_r_positive_0x93},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x94), gamma_r_positive_0x94},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x95), gamma_r_positive_0x95},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x96), gamma_r_positive_0x96},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x97), gamma_r_positive_0x97},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x98), gamma_r_positive_0x98},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x99), gamma_r_positive_0x99},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x9A), gamma_r_positive_0x9A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x9B), gamma_r_positive_0x9B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x9C), gamma_r_positive_0x9C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x9D), gamma_r_positive_0x9D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x9E), gamma_r_positive_0x9E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0x9F), gamma_r_positive_0x9F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xA0), gamma_r_positive_0xA0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xA2), gamma_r_positive_0xA2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xA3), gamma_r_positive_0xA3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xA4), gamma_r_positive_0xA4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xA5), gamma_r_positive_0xA5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xA6), gamma_r_positive_0xA6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xA7), gamma_r_positive_0xA7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xA9), gamma_r_positive_0xA9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xAA), gamma_r_positive_0xAA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xAB), gamma_r_positive_0xAB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xAC), gamma_r_positive_0xAC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xAD), gamma_r_positive_0xAD},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xAE), gamma_r_positive_0xAE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xAF), gamma_r_positive_0xAF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xB0), gamma_r_positive_0xB0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xB1), gamma_r_positive_0xB1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_positive_0xB2), gamma_r_positive_0xB2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xB3), gamma_r_negative_0xB3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xB4), gamma_r_negative_0xB4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xB5), gamma_r_negative_0xB5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xB6), gamma_r_negative_0xB6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xB7), gamma_r_negative_0xB7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xB8), gamma_r_negative_0xB8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xB9), gamma_r_negative_0xB9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xBA), gamma_r_negative_0xBA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xBB), gamma_r_negative_0xBB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xBC), gamma_r_negative_0xBC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xBD), gamma_r_negative_0xBD},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xBE), gamma_r_negative_0xBE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xBF), gamma_r_negative_0xBF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC0), gamma_r_negative_0xC0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC1), gamma_r_negative_0xC1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC2), gamma_r_negative_0xC2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC3), gamma_r_negative_0xC3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC4), gamma_r_negative_0xC4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC5), gamma_r_negative_0xC5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC6), gamma_r_negative_0xC6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC7), gamma_r_negative_0xC7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC8), gamma_r_negative_0xC8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xC9), gamma_r_negative_0xC9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xCA), gamma_r_negative_0xCA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xCB), gamma_r_negative_0xCB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xCC), gamma_r_negative_0xCC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xCD), gamma_r_negative_0xCD},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xCE), gamma_r_negative_0xCE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xCF), gamma_r_negative_0xCF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD0), gamma_r_negative_0xD0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD1), gamma_r_negative_0xD1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD2), gamma_r_negative_0xD2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD3), gamma_r_negative_0xD3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD4), gamma_r_negative_0xD4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD5), gamma_r_negative_0xD5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD6), gamma_r_negative_0xD6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD7), gamma_r_negative_0xD7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD8), gamma_r_negative_0xD8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xD9), gamma_r_negative_0xD9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xDA), gamma_r_negative_0xDA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xDB), gamma_r_negative_0xDB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xDC), gamma_r_negative_0xDC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xDD), gamma_r_negative_0xDD},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xDE), gamma_r_negative_0xDE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xDF), gamma_r_negative_0xDF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE0), gamma_r_negative_0xE0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE1), gamma_r_negative_0xE1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE2), gamma_r_negative_0xE2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE3), gamma_r_negative_0xE3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE4), gamma_r_negative_0xE4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE5), gamma_r_negative_0xE5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE6), gamma_r_negative_0xE6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE7), gamma_r_negative_0xE7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE8), gamma_r_negative_0xE8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xE9), gamma_r_negative_0xE9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xEA), gamma_r_negative_0xEA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xEB), gamma_r_negative_0xEB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xEC), gamma_r_negative_0xEC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xED), gamma_r_negative_0xED},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_r_negative_0xEE), gamma_r_negative_0xEE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xEF), gamma_g_positive_0xEF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF0), gamma_g_positive_0xF0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF1), gamma_g_positive_0xF1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF2), gamma_g_positive_0xF2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF3), gamma_g_positive_0xF3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF4), gamma_g_positive_0xF4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF5), gamma_g_positive_0xF5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF6), gamma_g_positive_0xF6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF7), gamma_g_positive_0xF7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF8), gamma_g_positive_0xF8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xF9), gamma_g_positive_0xF9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0xFA), gamma_g_positive_0xFA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(page_selection_0xFF_0x21), page_selection_0xFF_0x21},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof( non_reload_0xFB),  non_reload_0xFB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x00), gamma_g_positive_0x00},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x01), gamma_g_positive_0x01},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x02), gamma_g_positive_0x02},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x03), gamma_g_positive_0x03},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x04), gamma_g_positive_0x04},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x05), gamma_g_positive_0x05},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x06), gamma_g_positive_0x06},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x07), gamma_g_positive_0x07},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x08), gamma_g_positive_0x08},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x09), gamma_g_positive_0x09},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x0A), gamma_g_positive_0x0A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x0B), gamma_g_positive_0x0B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x0C), gamma_g_positive_0x0C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x0D), gamma_g_positive_0x0D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x0E), gamma_g_positive_0x0E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x0F), gamma_g_positive_0x0F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x10), gamma_g_positive_0x10},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x11), gamma_g_positive_0x11},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x12), gamma_g_positive_0x12},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x13), gamma_g_positive_0x13},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x14), gamma_g_positive_0x14},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x15), gamma_g_positive_0x15},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x16), gamma_g_positive_0x16},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x17), gamma_g_positive_0x17},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x18), gamma_g_positive_0x18},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x19), gamma_g_positive_0x19},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x1A), gamma_g_positive_0x1A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x1B), gamma_g_positive_0x1B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x1C), gamma_g_positive_0x1C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x1D), gamma_g_positive_0x1D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x1E), gamma_g_positive_0x1E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x1F), gamma_g_positive_0x1F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x20), gamma_g_positive_0x20},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x21), gamma_g_positive_0x21},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x22), gamma_g_positive_0x22},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x23), gamma_g_positive_0x23},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x24), gamma_g_positive_0x24},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x25), gamma_g_positive_0x25},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x26), gamma_g_positive_0x26},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x27), gamma_g_positive_0x27},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x28), gamma_g_positive_0x28},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x29), gamma_g_positive_0x29},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x2A), gamma_g_positive_0x2A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x2B), gamma_g_positive_0x2B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x2D), gamma_g_positive_0x2D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x2F), gamma_g_positive_0x2F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x30), gamma_g_positive_0x30},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_positive_0x31), gamma_g_positive_0x31},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x32), gamma_g_negative_0x32},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x33), gamma_g_negative_0x33},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x34), gamma_g_negative_0x34},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x35), gamma_g_negative_0x35},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x36), gamma_g_negative_0x36},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x37), gamma_g_negative_0x37},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x38), gamma_g_negative_0x38},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x39), gamma_g_negative_0x39},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x3A), gamma_g_negative_0x3A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x3B), gamma_g_negative_0x3B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x3D), gamma_g_negative_0x3D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x3F), gamma_g_negative_0x3F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x40), gamma_g_negative_0x40},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x41), gamma_g_negative_0x41},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x42), gamma_g_negative_0x42},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x43), gamma_g_negative_0x43},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x44), gamma_g_negative_0x44},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x45), gamma_g_negative_0x45},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x46), gamma_g_negative_0x46},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x47), gamma_g_negative_0x47},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x48), gamma_g_negative_0x48},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x49), gamma_g_negative_0x49},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x4A), gamma_g_negative_0x4A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x4B), gamma_g_negative_0x4B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x4C), gamma_g_negative_0x4C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x4D), gamma_g_negative_0x4D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x4E), gamma_g_negative_0x4E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x4F), gamma_g_negative_0x4F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x50), gamma_g_negative_0x50},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x51), gamma_g_negative_0x51},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x52), gamma_g_negative_0x52},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x53), gamma_g_negative_0x53},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x54), gamma_g_negative_0x54},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x55), gamma_g_negative_0x55},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x56), gamma_g_negative_0x56},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x58), gamma_g_negative_0x58},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x59), gamma_g_negative_0x59},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x5A), gamma_g_negative_0x5A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x5B), gamma_g_negative_0x5B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x5C), gamma_g_negative_0x5C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x5D), gamma_g_negative_0x5D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x5E), gamma_g_negative_0x5E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x5F), gamma_g_negative_0x5F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x60), gamma_g_negative_0x60},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x61), gamma_g_negative_0x61},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x62), gamma_g_negative_0x62},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x63), gamma_g_negative_0x63},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x64), gamma_g_negative_0x64},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x65), gamma_g_negative_0x65},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x66), gamma_g_negative_0x66},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x67), gamma_g_negative_0x67},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x68), gamma_g_negative_0x68},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x69), gamma_g_negative_0x69},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x6A), gamma_g_negative_0x6A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x6B), gamma_g_negative_0x6B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x6C), gamma_g_negative_0x6C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x6D), gamma_g_negative_0x6D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x6E), gamma_g_negative_0x6E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x6F), gamma_g_negative_0x6F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_g_negative_0x70), gamma_g_negative_0x70},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x71), gamma_b_positive_0x71},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x72), gamma_b_positive_0x72},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x73), gamma_b_positive_0x73},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x74), gamma_b_positive_0x74},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x75), gamma_b_positive_0x75},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x76), gamma_b_positive_0x76},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x77), gamma_b_positive_0x77},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x78), gamma_b_positive_0x78},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x79), gamma_b_positive_0x79},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x7A), gamma_b_positive_0x7A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x7B), gamma_b_positive_0x7B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x7C), gamma_b_positive_0x7C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x7D), gamma_b_positive_0x7D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x7E), gamma_b_positive_0x7E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x7F), gamma_b_positive_0x7F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x80), gamma_b_positive_0x80},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x81), gamma_b_positive_0x81},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x82), gamma_b_positive_0x82},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x83), gamma_b_positive_0x83},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x84), gamma_b_positive_0x84},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x85), gamma_b_positive_0x85},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x86), gamma_b_positive_0x86},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x87), gamma_b_positive_0x87},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x88), gamma_b_positive_0x88},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x89), gamma_b_positive_0x89},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x8A), gamma_b_positive_0x8A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x8B), gamma_b_positive_0x8B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x8C), gamma_b_positive_0x8C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x8D), gamma_b_positive_0x8D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x8E), gamma_b_positive_0x8E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x8F), gamma_b_positive_0x8F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x90), gamma_b_positive_0x90},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x91), gamma_b_positive_0x91},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x92), gamma_b_positive_0x92},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x93), gamma_b_positive_0x93},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x94), gamma_b_positive_0x94},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x95), gamma_b_positive_0x95},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x96), gamma_b_positive_0x96},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x97), gamma_b_positive_0x97},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x98), gamma_b_positive_0x98},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x99), gamma_b_positive_0x99},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x9A), gamma_b_positive_0x9A},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x9B), gamma_b_positive_0x9B},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x9C), gamma_b_positive_0x9C},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x9D), gamma_b_positive_0x9D},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x9E), gamma_b_positive_0x9E},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0x9F), gamma_b_positive_0x9F},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xA0), gamma_b_positive_0xA0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xA2), gamma_b_positive_0xA2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xA3), gamma_b_positive_0xA3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xA4), gamma_b_positive_0xA4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xA5), gamma_b_positive_0xA5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xA6), gamma_b_positive_0xA6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xA7), gamma_b_positive_0xA7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xA9), gamma_b_positive_0xA9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xAA), gamma_b_positive_0xAA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xAB), gamma_b_positive_0xAB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xAC), gamma_b_positive_0xAC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xAD), gamma_b_positive_0xAD},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_positive_0xAE), gamma_b_positive_0xAE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xAF), gamma_b_negative_0xAF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB0), gamma_b_negative_0xB0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB1), gamma_b_negative_0xB1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB2), gamma_b_negative_0xB2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB3), gamma_b_negative_0xB3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB4), gamma_b_negative_0xB4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB5), gamma_b_negative_0xB5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB6), gamma_b_negative_0xB6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB7), gamma_b_negative_0xB7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB8), gamma_b_negative_0xB8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xB9), gamma_b_negative_0xB9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xBA), gamma_b_negative_0xBA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xBB), gamma_b_negative_0xBB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xBC), gamma_b_negative_0xBC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xBD), gamma_b_negative_0xBD},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xBE), gamma_b_negative_0xBE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xBF), gamma_b_negative_0xBF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC0), gamma_b_negative_0xC0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC1), gamma_b_negative_0xC1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC2), gamma_b_negative_0xC2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC3), gamma_b_negative_0xC3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC4), gamma_b_negative_0xC4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC5), gamma_b_negative_0xC5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC6), gamma_b_negative_0xC6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC7), gamma_b_negative_0xC7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC8), gamma_b_negative_0xC8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xC9), gamma_b_negative_0xC9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xCA), gamma_b_negative_0xCA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xCB), gamma_b_negative_0xCB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xCC), gamma_b_negative_0xCC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xCD), gamma_b_negative_0xCD},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xCE), gamma_b_negative_0xCE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xCF), gamma_b_negative_0xCF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD0), gamma_b_negative_0xD0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD1), gamma_b_negative_0xD1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD2), gamma_b_negative_0xD2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD3), gamma_b_negative_0xD3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD4), gamma_b_negative_0xD4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD5), gamma_b_negative_0xD5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD6), gamma_b_negative_0xD6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD7), gamma_b_negative_0xD7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD8), gamma_b_negative_0xD8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xD9), gamma_b_negative_0xD9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xDA), gamma_b_negative_0xDA},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xDB), gamma_b_negative_0xDB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xDC), gamma_b_negative_0xDC},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xDD), gamma_b_negative_0xDD},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xDE), gamma_b_negative_0xDE},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xDF), gamma_b_negative_0xDF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE0), gamma_b_negative_0xE0},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE1), gamma_b_negative_0xE1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE2), gamma_b_negative_0xE2},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE3), gamma_b_negative_0xE3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE4), gamma_b_negative_0xE4},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE5), gamma_b_negative_0xE5},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE6), gamma_b_negative_0xE6},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE7), gamma_b_negative_0xE7},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE8), gamma_b_negative_0xE8},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xE9), gamma_b_negative_0xE9},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(gamma_b_negative_0xEA), gamma_b_negative_0xEA},
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
		sizeof(cabc_0x32), cabc_0x32},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(cmd1_0xFF), cmd1_0xFF},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_set_mode_UI), cabc_set_mode_UI},
};

static struct dsi_cmd_desc cabc_dimming_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(page_selection_0xFF_0x23), page_selection_0xFF_0x23},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(non_reload_0xFB), non_reload_0xFB},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_endimming_0x00), cabc_endimming_0x00},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(cmd1_0xFF), cmd1_0xFF},
};

#ifdef CONFIG_BACKLIGHT_10000
static struct dsi_cmd_desc pwm_out_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(PWM_OUT_0x51), PWM_OUT_0x51},
};
#endif

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
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
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
 
/*******************************************************************************
** ACM LUT Tables
*/
static u32 acm_lut_hue_table[] = {
	0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f, 0x0010, 0x0011, 0x0012,
	0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024,
	0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
	0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044,
	0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053,
	0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005c, 0x005d, 0x005e, 0x005f, 0x0060, 0x0061, 0x0062,
	0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 0x0070,
	0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
	0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, 0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ad, 0x00ae,
	0x00af, 0x00b0, 0x00b1, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b6, 0x00b7, 0x00b8, 0x00b9, 0x00ba, 0x00ba, 0x00bb,
	0x00bc, 0x00bd, 0x00be, 0x00bf, 0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c6, 0x00c7, 0x00c8, 0x00c9, 0x00ca,
	0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, 0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 0x00d8, 0x00d9, 0x00da,
	0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, 0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 0x00e8, 0x00e9,
	0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 0x00ef, 0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 0x00f8,
	0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff, 0x0100, 0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108,
	0x010a, 0x010b, 0x010c, 0x010d, 0x010e, 0x010f, 0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117, 0x0118, 0x0119,
	0x011a, 0x011b, 0x011c, 0x011d, 0x011e, 0x011f, 0x0120, 0x0121, 0x0122, 0x0123, 0x0124, 0x0125, 0x0126, 0x0127, 0x0128, 0x0129,
	0x012a, 0x012c, 0x012d, 0x012e, 0x012f, 0x0130, 0x0131, 0x0132, 0x0133, 0x0134, 0x0135, 0x0136, 0x0137, 0x0138, 0x0139, 0x013a,
	0x013b, 0x013c, 0x013d, 0x013e, 0x013f, 0x0140, 0x0141, 0x0142, 0x0143, 0x0144, 0x0145, 0x0147, 0x0148, 0x0149, 0x014a, 0x014b,
	0x014c, 0x014d, 0x014e, 0x014f, 0x0150, 0x0151, 0x0152, 0x0153, 0x0154, 0x0155, 0x0156, 0x0157, 0x0158, 0x0159, 0x015a, 0x015b,
	0x015c, 0x015e, 0x015f, 0x0160, 0x0161, 0x0162, 0x0163, 0x0164, 0x0165, 0x0166, 0x0167, 0x0168, 0x0169, 0x016a, 0x016b, 0x016c,
	0x016d, 0x016e, 0x016f, 0x0170, 0x0171, 0x0172, 0x0174, 0x0175, 0x0176, 0x0177, 0x0178, 0x0179, 0x017a, 0x017b, 0x017c, 0x017d,
	0x017e, 0x017f, 0x0180, 0x0181, 0x0182, 0x0183, 0x0184, 0x0185, 0x0186, 0x0187, 0x0188, 0x0189, 0x018a, 0x018b, 0x018c, 0x018d,
	0x018e, 0x0190, 0x0191, 0x0192, 0x0193, 0x0194, 0x0195, 0x0196, 0x0197, 0x0198, 0x0199, 0x019a, 0x019b, 0x019c, 0x019d, 0x019e,
	0x019f, 0x01a0, 0x01a1, 0x01a2, 0x01a3, 0x01a4, 0x01a5, 0x01a6, 0x01a7, 0x01a8, 0x01a9, 0x01aa, 0x01ab, 0x01ac, 0x01ad, 0x01ae,
	0x01b0, 0x01b1, 0x01b2, 0x01b3, 0x01b4, 0x01b5, 0x01b6, 0x01b7, 0x01b8, 0x01b9, 0x01ba, 0x01bb, 0x01bc, 0x01bd, 0x01be, 0x01bf,
	0x01c0, 0x01c1, 0x01c2, 0x01c3, 0x01c4, 0x01c5, 0x01c6, 0x01c7, 0x01c8, 0x01c9, 0x01ca, 0x01cb, 0x01cc, 0x01cd, 0x01ce, 0x01cf,
	0x01d0, 0x01d1, 0x01d2, 0x01d3, 0x01d4, 0x01d5, 0x01d6, 0x01d7, 0x01d8, 0x01d9, 0x01da, 0x01db, 0x01dc, 0x01dd, 0x01de, 0x01df,
	0x01e0, 0x01e1, 0x01e2, 0x01e3, 0x01e4, 0x01e5, 0x01e6, 0x01e7, 0x01e8, 0x01e9, 0x01ea, 0x01eb, 0x01ec, 0x01ed, 0x01ee, 0x01ef,
	0x01f0, 0x01f1, 0x01f2, 0x01f3, 0x01f4, 0x01f5, 0x01f6, 0x01f7, 0x01f8, 0x01f9, 0x01fa, 0x01fb, 0x01fc, 0x01fd, 0x01fe, 0x01ff,
	0x0200, 0x0201, 0x0202, 0x0203, 0x0204, 0x0205, 0x0206, 0x0207, 0x0208, 0x0209, 0x020a, 0x020b, 0x020c, 0x020d, 0x020e, 0x020f,
	0x0210, 0x0211, 0x0212, 0x0213, 0x0214, 0x0215, 0x0216, 0x0217, 0x0218, 0x0219, 0x021a, 0x021b, 0x021c, 0x021d, 0x021e, 0x021f,
	0x0220, 0x0221, 0x0222, 0x0223, 0x0224, 0x0225, 0x0226, 0x0227, 0x0228, 0x0229, 0x022a, 0x022b, 0x022c, 0x022d, 0x022e, 0x022f,
	0x0230, 0x0231, 0x0232, 0x0233, 0x0234, 0x0235, 0x0236, 0x0237, 0x0238, 0x0239, 0x023a, 0x023b, 0x023c, 0x023d, 0x023e, 0x023f,
	0x0240, 0x0241, 0x0242, 0x0243, 0x0244, 0x0245, 0x0246, 0x0247, 0x0248, 0x0249, 0x024a, 0x024b, 0x024c, 0x024d, 0x024e, 0x024f,
	0x0250, 0x0251, 0x0252, 0x0253, 0x0254, 0x0255, 0x0256, 0x0257, 0x0258, 0x0259, 0x025a, 0x025b, 0x025c, 0x025d, 0x025e, 0x025f,
	0x0260, 0x0261, 0x0262, 0x0263, 0x0264, 0x0265, 0x0266, 0x0267, 0x0268, 0x0269, 0x026a, 0x026b, 0x026c, 0x026d, 0x026e, 0x026f,
	0x0270, 0x0271, 0x0272, 0x0273, 0x0274, 0x0275, 0x0276, 0x0277, 0x0278, 0x0279, 0x027a, 0x027b, 0x027c, 0x027d, 0x027e, 0x027f,
	0x0280, 0x0281, 0x0282, 0x0283, 0x0284, 0x0285, 0x0286, 0x0287, 0x0288, 0x0289, 0x028a, 0x028b, 0x028c, 0x028d, 0x028e, 0x028f,
	0x0290, 0x0291, 0x0292, 0x0293, 0x0294, 0x0295, 0x0296, 0x0297, 0x0298, 0x0299, 0x029a, 0x029b, 0x029c, 0x029d, 0x029e, 0x029f,
	0x02a0, 0x02a1, 0x02a2, 0x02a3, 0x02a4, 0x02a5, 0x02a6, 0x02a7, 0x02a8, 0x02a9, 0x02aa, 0x02ab, 0x02ac, 0x02ad, 0x02ae, 0x02af,
	0x02b0, 0x02b1, 0x02b2, 0x02b3, 0x02b4, 0x02b5, 0x02b6, 0x02b7, 0x02b8, 0x02b9, 0x02ba, 0x02bb, 0x02bc, 0x02bd, 0x02be, 0x02bf,
	0x02c0, 0x02c1, 0x02c2, 0x02c3, 0x02c4, 0x02c5, 0x02c6, 0x02c7, 0x02c8, 0x02c9, 0x02ca, 0x02cb, 0x02cc, 0x02cd, 0x02ce, 0x02cf,
	0x02d0, 0x02d1, 0x02d2, 0x02d3, 0x02d4, 0x02d5, 0x02d6, 0x02d7, 0x02d8, 0x02d9, 0x02da, 0x02db, 0x02dc, 0x02dd, 0x02de, 0x02df,
	0x02e0, 0x02e1, 0x02e2, 0x02e3, 0x02e4, 0x02e5, 0x02e6, 0x02e7, 0x02e8, 0x02e9, 0x02ea, 0x02eb, 0x02ec, 0x02ed, 0x02ee, 0x02ef,
	0x02f0, 0x02f1, 0x02f2, 0x02f3, 0x02f4, 0x02f5, 0x02f6, 0x02f7, 0x02f8, 0x02f9, 0x02fa, 0x02fb, 0x02fc, 0x02fd, 0x02fe, 0x02ff,
	0x0300, 0x0301, 0x0302, 0x0303, 0x0304, 0x0305, 0x0306, 0x0307, 0x0308, 0x0309, 0x030a, 0x030b, 0x030c, 0x030d, 0x030e, 0x030f,
	0x0310, 0x0311, 0x0312, 0x0313, 0x0314, 0x0315, 0x0316, 0x0317, 0x0318, 0x0319, 0x031a, 0x031b, 0x031c, 0x031d, 0x031e, 0x031f,
	0x0320, 0x0321, 0x0322, 0x0323, 0x0324, 0x0325, 0x0326, 0x0327, 0x0328, 0x0329, 0x032a, 0x032b, 0x032c, 0x032d, 0x032e, 0x032f,
	0x0330, 0x0331, 0x0332, 0x0333, 0x0334, 0x0335, 0x0336, 0x0337, 0x0338, 0x0339, 0x033a, 0x033b, 0x033c, 0x033d, 0x033e, 0x033f,
	0x0340, 0x0341, 0x0342, 0x0343, 0x0344, 0x0345, 0x0346, 0x0347, 0x0348, 0x0349, 0x034a, 0x034b, 0x034c, 0x034d, 0x034e, 0x034f,
	0x0350, 0x0351, 0x0352, 0x0353, 0x0354, 0x0355, 0x0356, 0x0357, 0x0358, 0x0359, 0x035a, 0x035b, 0x035c, 0x035d, 0x035e, 0x035f,
	0x0360, 0x0361, 0x0362, 0x0363, 0x0364, 0x0365, 0x0366, 0x0367, 0x0368, 0x0369, 0x036a, 0x036b, 0x036c, 0x036d, 0x036e, 0x036f,
	0x0370, 0x0371, 0x0372, 0x0373, 0x0374, 0x0375, 0x0376, 0x0377, 0x0378, 0x0379, 0x037a, 0x037b, 0x037c, 0x037d, 0x037e, 0x037f,
	0x0380, 0x0381, 0x0382, 0x0383, 0x0384, 0x0385, 0x0386, 0x0387, 0x0388, 0x0389, 0x038a, 0x038b, 0x038c, 0x038d, 0x038e, 0x038f,
	0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f,
	0x03a0, 0x03a2, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7, 0x03a8, 0x03a9, 0x03aa, 0x03ab, 0x03ac, 0x03ad, 0x03ae, 0x03af, 0x03b0,
	0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf, 0x03c0,
	0x03c1, 0x03c2, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7, 0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03cc, 0x03cd, 0x03ce, 0x03cf, 0x03d0,
	0x03d1, 0x03d2, 0x03d3, 0x03d4, 0x03d5, 0x03d6, 0x03d7, 0x03d8, 0x03d9, 0x03da, 0x03db, 0x03dc, 0x03dd, 0x03de, 0x03df, 0x03e0,
	0x03e2, 0x03e3, 0x03e4, 0x03e5, 0x03e6, 0x03e7, 0x03e8, 0x03e9, 0x03ea, 0x03eb, 0x03ec, 0x03ed, 0x03ee, 0x03ef, 0x03f0, 0x03f1,
	0x03f2, 0x03f3, 0x03f4, 0x03f5, 0x03f6, 0x03f7, 0x03f8, 0x03f9, 0x03fa, 0x03fb, 0x03fc, 0x03fd, 0x03fe, 0x03ff, 0x0000, 0x0001,
};

static u32 acm_lut_value_table[] = {
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
	0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, 0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
	0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, 0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
	0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, 0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
	0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
	0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
	0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff,
};

static u32 acm_lut_sata_table[] = {
	0x0005, 0x0006, 0x0007, 0x0008, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000e, 0x000e, 0x000d, 0x000d, 0x000d, 0x000c, 0x000c,
	0x000c, 0x000b, 0x000a, 0x0009, 0x0008, 0x0007, 0x0006, 0x0005, 0x0004, 0x0004, 0x0003, 0x0002, 0x0002, 0x0002, 0x0001, 0x0000,
	0x0000, 0x0010, 0x0020, 0x002f, 0x003f, 0x004f, 0x005e, 0x006e, 0x007e, 0x0089, 0x0094, 0x009f, 0x00aa, 0x00b5, 0x00c0, 0x00cb,
	0x00d6, 0x00d8, 0x00da, 0x00dc, 0x00de, 0x00df, 0x00e1, 0x00e3, 0x00e5, 0x00e6, 0x00e6, 0x00e6, 0x00e7, 0x00e8, 0x00e8, 0x00e8,
	0x00e9, 0x00e9, 0x00e8, 0x00e8, 0x00e8, 0x00e8, 0x00e8, 0x00e7, 0x00e7, 0x00e5, 0x00e3, 0x00e1, 0x00de, 0x00dc, 0x00da, 0x00d8,
	0x00d6, 0x00cd, 0x00c4, 0x00bb, 0x00b2, 0x00a9, 0x00a0, 0x0097, 0x008e, 0x0086, 0x007e, 0x0076, 0x006e, 0x0065, 0x005d, 0x0055,
	0x004d, 0x0047, 0x0041, 0x003b, 0x0036, 0x0030, 0x002a, 0x0024, 0x001e, 0x001a, 0x0017, 0x0014, 0x0010, 0x000c, 0x0009, 0x0006,
	0x0002, 0x0002, 0x0002, 0x0001, 0x0001, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0001, 0x0002, 0x0002, 0x0003, 0x0004, 0x0004,
};

static u32 acm_lut_satr_table[] = {
	0x0000, 0x00ff, 0x00fd, 0x00fc, 0x00fa, 0x00f9, 0x00f8, 0x00f6, 0x00f5, 0x00f3, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2,
	0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f2,
	0x00f2, 0x00f2, 0x00f2, 0x00f2, 0x00f3, 0x00f3, 0x00f3, 0x00f3, 0x00f4, 0x00f4, 0x00f4, 0x00f4, 0x00f5, 0x00f5, 0x00f5, 0x00f6,
	0x00f6, 0x00f6, 0x00f6, 0x00f7, 0x00f7, 0x00f7, 0x00f7, 0x00f8, 0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fd, 0x00fe, 0x00ff, 0x0000,
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x0009, 0x000a, 0x000a, 0x000a, 0x000b,
	0x000b, 0x000c, 0x000c, 0x000c, 0x000d, 0x000d, 0x000d, 0x000e, 0x000e, 0x000f, 0x000f, 0x000f, 0x0010, 0x0010, 0x0010, 0x0011,
	0x0011, 0x0012, 0x0012, 0x0012, 0x0011, 0x0011, 0x0010, 0x0010, 0x0010, 0x000f, 0x000f, 0x000e, 0x000e, 0x000d, 0x000d, 0x000d,
	0x000c, 0x000c, 0x000b, 0x000b, 0x000b, 0x000a, 0x000a, 0x0009, 0x0009, 0x0008, 0x0006, 0x0005, 0x0004, 0x0003, 0x0001, 0x0000,
	0x0000, 0x0002, 0x0003, 0x0005, 0x0006, 0x0008, 0x000a, 0x000b, 0x000d, 0x000e, 0x0010, 0x0010, 0x0011, 0x0011, 0x0011, 0x0011,
	0x0012, 0x0012, 0x0012, 0x0013, 0x0013, 0x0013, 0x0014, 0x0014, 0x0014, 0x0014, 0x0015, 0x0015, 0x0015, 0x0016, 0x0016, 0x0016,
	0x0016, 0x0017, 0x0017, 0x0016, 0x0016, 0x0015, 0x0015, 0x0014, 0x0014, 0x0013, 0x0013, 0x0012, 0x0012, 0x0011, 0x0010, 0x0010,
	0x000f, 0x000f, 0x000e, 0x000e, 0x000d, 0x000d, 0x000c, 0x000c, 0x000b, 0x0009, 0x0008, 0x0006, 0x0005, 0x0003, 0x0002, 0x0000,
	0x0000, 0x0001, 0x0003, 0x0004, 0x0006, 0x0007, 0x0008, 0x000a, 0x000b, 0x000d, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e,
	0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000c, 0x000c, 0x000c,
	0x000c, 0x000c, 0x000c, 0x000c, 0x000c, 0x000b, 0x000b, 0x000b, 0x000b, 0x000a, 0x000a, 0x000a, 0x000a, 0x000a, 0x0009, 0x0009,
	0x0009, 0x0009, 0x0008, 0x0008, 0x0008, 0x0008, 0x0007, 0x0007, 0x0007, 0x0006, 0x0005, 0x0004, 0x0003, 0x0002, 0x0001, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};


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
			if (g_vddio_type)
				ts_power_gpio_enable();
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

		if (g_rf_flag)
		{
			mipi_dsi_cmds_tx(jdi_reduce_rf_interfere_cmds,\
				ARRAY_SIZE(jdi_reduce_rf_interfere_cmds),mipi_dsi0_base);
		}

		mipi_dsi_cmds_tx(jdi_display_effect_on_cmds, \
			ARRAY_SIZE(jdi_display_effect_on_cmds), mipi_dsi0_base);

#ifdef CONFIG_BACKLIGHT_10000
		if (pinfo->bl_set_type == BL_SET_BY_MIPI) {
			mipi_dsi_cmds_tx(cabc_dimming_on_cmds, \
			      ARRAY_SIZE(	cabc_dimming_on_cmds), mipi_dsi0_base);
		}else{
			mipi_dsi_cmds_tx(pwm_out_on_cmds, \
			      ARRAY_SIZE(	pwm_out_on_cmds), mipi_dsi0_base);
		}
#endif
		mipi_dsi_cmds_tx(jdi_display_on_cmds, \
			ARRAY_SIZE(jdi_display_on_cmds), mipi_dsi0_base);
		g_cabc_mode = 1;
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

		if (g_vddio_type)
			ts_power_gpio_disable();
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


	if (hisifd->hisi_fb_shutdown) {
		ts_thread_stop_notify();
	}
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

	ret = snprintf(buf, PAGE_SIZE, "jdi_NT35695 5.2' CMD TFT\n");

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

	if (!error)
		error = ts_power_control_notify(TS_AFTER_RESUME, SHORT_SYNC_TIMEOUT);
	hisifd->lcd_self_testing = false;

	return final_ret;
}

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
	.lcd_bist_check = mipi_jdi_panel_lcd_bist_check,
	.set_display_region = mipi_jdi_panel_set_display_region,
	.set_display_resolution = NULL,
};

static void set_scp_param(struct hisi_panel_info *pinfo)
{
	int dls_bound = 0x08, f3 = 0x40,  f2 = 0x40,  f1 = 0x20;
	int dls_clk_cfg = 2, shaps_clk_cfg = 2,  block_sync = 1, cfg_gate = 0;
	int module_gate = 0, darkkratio = 0xc, module_en = 1, darkkmax2 = 0xc0;
	int userk = 0x84, inv_kb = 0xed, dynakb = 0x14, dynakd = 0x14;
	int darkkmax1 = 0x180,yb = 0, brightc = 0xc, darkt = 0x2;
	int yavgmax = 0x78, inv_yavgbase = 0x6, yavgbase = 0x28, shaps_bound = 0x20;
	int yavg = 0xa0, darkkup = 0xcd, ydarkpixelt = 0x28, inv_ymaxmbase = 0x3;
	int hsize = 0x9ff, brightc_cfg = 0x38, dls_bypass = 1, pa_sel = 0, dark_en = 1;
	int fh3 = 0x64, fh2 = 0x18,fh1 = 0xb7, fh0 = 0x93;
	int ypkn = 0x13, ypkp = 0x13, tx = 0x8, kh = 0x1c;
	int ktmp_max = 1, sel = 0, shaps_bypass = 1, ypb = 0x3;
	int b = 0x8, dltikn = 0x4, dltikp = 0x4, largescaleup = 0, xs = 0;
	int error_clear = 0, flag_clear = 0;
	int yminp_in = 0x20,  xminp_in = 0x2a, ymaxp_in = 0xdc, xmaxp_in = 0xd6;
	int kxy_in = 0x117, darkk_in = 0xc9;
	int head_clip = 0, tail_clip = 0, kmax_in = 0xda;


	pinfo->scp_reg0 = ( ((dls_bound & 0xff) << 24) | ((f3 & 0xff) << 16) | ((f2 & 0xff) << 8) | (f1 & 0xff) );
	pinfo->scp_reg1 = ( ((dls_clk_cfg & 0x3) << 25) | ((shaps_clk_cfg & 0x3) << 23) | ((block_sync & 0x1) << 22)
		| ((cfg_gate & 0x1) << 21) | ((module_gate & 0x1) << 20) | ((darkkratio & 0x1ff) << 11)
		| ((module_en & 0x1) << 10) | (darkkmax2 & 0x3ff));
	pinfo->scp_reg2 = (((userk & 0xff) << 24) |((inv_kb & 0xff) << 16) | ((dynakb & 0xff) << 8) | (dynakd & 0xff));
	pinfo->scp_reg3 = (((darkkmax1 & 0x3ff) << 20) | ((yb & 0xff) << 12)  | ((brightc & 0xf) << 8) | (darkt & 0xff)) ;
	pinfo->scp_reg4 = (((yavgmax & 0xff) << 24) |((inv_yavgbase & 0xff) << 16) | ((yavgbase & 0xff) << 8) | (shaps_bound & 0xff));
	pinfo->scp_reg5 = (((yavg & 0xff) << 24) |((darkkup & 0xff) << 16) | ((ydarkpixelt & 0xff) << 8) | (inv_ymaxmbase & 0xff));
	pinfo->scp_reg6 = (((hsize&0xffff) << 9) | ((brightc_cfg&0x3f) << 3) | ((dls_bypass&0x1) << 2) | ((pa_sel&0x1) << 1) | (dark_en&0x1));
	pinfo->scp_reg7 = (((fh3  & 0xff) << 24) |((fh2 & 0xff) << 16) | ((fh1 & 0xff) << 8) | (fh0 & 0xff));
	pinfo->scp_reg8 = (((ypkn  & 0xff) << 24) |((ypkp & 0xff) << 16) | ((tx & 0xff) << 8) | (kh & 0xff));
	pinfo->scp_reg9 = (((ktmp_max&0x1)<<23) | ((sel&0x1)<<22) | ((shaps_bypass&0x1)<<21) | ((ypb &0xf)<<17) | ((b &0xf)<<13)
		| ((dltikn&0xf)<<9) | ((dltikp&0xf)<<5) | ((largescaleup&0x1)<<4) | (xs&0xf));
	pinfo->scp_reg10 = (((error_clear&0x1)<<1) | (flag_clear&0x1));
	pinfo->scp_reg11 = (((yminp_in  & 0xff) << 24) |((xminp_in & 0xff) << 16) | ((ymaxp_in & 0xff) << 8) | (xmaxp_in & 0xff));
	pinfo->scp_reg12 = ( ((kxy_in & 0xffff) << 16) | (darkk_in & 0xffff));
	pinfo->scp_reg13 = ( ((head_clip & 0xff) << 24) | ((tail_clip & 0xff) << 16) | (kmax_in & 0xffff));

	return;
}

/*******************************************************************************
**
*/
static int mipi_jdi_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_panel_info *pinfo = NULL;
	struct device_node *np = NULL;
	uint32_t bl_type = 0;

	if (hisi_fb_device_probe_defer(PANEL_MIPI_CMD))
		goto err_probe_defer;

	HISI_FB_DEBUG("+.\n");

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_JDI_NT35695);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_JDI_NT35695);
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

	ret = of_property_read_u32(np,PLK_RF_REG_FLAG , &g_rf_flag);
	if (ret) {
		HISI_FB_ERR("get lcd_vddio_type failed!\n");
		g_rf_flag = 0;
	}
	HISI_FB_INFO("get g_rf_flag: %d\n", g_rf_flag);

	pdev->id = 1;
	/* init lcd panel info */
	pinfo = jdi_panel_data.panel_info;
	memset(pinfo, 0, sizeof(struct hisi_panel_info));
	pinfo->xres = 1080;
	pinfo->yres = 1920;
	pinfo->width  = 65;  //mm
	pinfo->height = 115; //mm
	pinfo->type = PANEL_MIPI_CMD;
	pinfo->orientation = LCD_PORTRAIT;
	pinfo->bpp = LCD_RGB888;
	pinfo->bgr_fmt = LCD_RGB;
	pinfo->bl_set_type = bl_type;

	pinfo->bl_min = 4;
	pinfo->bl_max = 254;
	pinfo->vsync_ctrl_type = (VSYNC_CTRL_ISR_OFF |
		VSYNC_CTRL_MIPI_ULPS);

	pinfo->frc_enable = 0;
	pinfo->esd_enable = 0;
	pinfo->dirty_region_updt_support = 0;
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

	if(runmode_is_factory()) {
		pinfo->sbl_support = 0;
		pinfo->dsi_bit_clk_upt_support = 0;
		pinfo->acm_support = 0;
		pinfo->prefix_sharpness_support = 0;
	} else {
		pinfo->sbl_support = 1;
		pinfo->dsi_bit_clk_upt_support = 1;
		pinfo->acm_support = 1;
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

	if (pinfo->acm_support == 1) {
		pinfo->acm_lut_hue_table = acm_lut_hue_table;
		pinfo->acm_lut_hue_table_len = sizeof(acm_lut_hue_table) / sizeof(acm_lut_hue_table[0]);
		pinfo->acm_lut_value_table = acm_lut_value_table;
		pinfo->acm_lut_value_table_len = sizeof(acm_lut_value_table) / sizeof(acm_lut_value_table[0]);
		pinfo->acm_lut_sata_table = acm_lut_sata_table;
		pinfo->acm_lut_sata_table_len = sizeof(acm_lut_sata_table) / sizeof(acm_lut_sata_table[0]);
		pinfo->acm_lut_satr_table = acm_lut_satr_table;
		pinfo->acm_lut_satr_table_len = sizeof(acm_lut_satr_table) / sizeof(acm_lut_satr_table[0]);
		pinfo->acm_valid_num = 7;
		pinfo->r0_hh = 0x6c;
		pinfo->r0_lh = 0x3c0;
		pinfo->r1_hh = 0xff;
		pinfo->r1_lh = 0x6d;
		pinfo->r2_hh = 0x17f;
		pinfo->r2_lh = 0x100;
		pinfo->r3_hh = 0x1ff;
		pinfo->r3_lh = 0x180;
		pinfo->r4_hh = 0x27f;
		pinfo->r4_lh = 0x200;
		pinfo->r5_hh = 0x2ff;
		pinfo->r5_lh = 0x280;
		pinfo->r6_hh = 0x37f;
		pinfo->r6_lh = 0x300;
	}

	if (pinfo->prefix_sharpness_support == 1) {
		set_scp_param(pinfo);
	}

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

	hisi_fb_add_device(pdev);

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
        .compatible = DTS_COMP_JDI_NT35695,
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
		.name = "mipi_jdi_NT35695",
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
