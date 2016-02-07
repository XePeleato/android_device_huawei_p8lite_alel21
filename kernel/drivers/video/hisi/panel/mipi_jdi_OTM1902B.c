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
#include <huawei_platform/log/log_jank.h>

#define DTS_COMP_JDI_OTM1902B "hisilicon,mipi_jdi_OTM1902B"
#define LCD_VDDIO_TYPE_NAME	"lcd-vddio-type"
static struct hisi_fb_panel_data jdi_panel_data;

static int hkadc_buf = 0;

static bool g_debug_enable = false;

extern int fastboot_set_needed;

/*******************************************************************************
** Power ON Sequence(sleep mode to Normal mode)
*/
static char adrsft1[] = {
	0x00,
	0x00,
};

static char cmd2_ena1[] = {
	0xff,
	0x19, 0x02, 0x01, 0x00,
};

static char adrsft2[] = {
	0x00,
	0x80,
};

static char cmd2_ena2[] = {
	0xff,
	0x19, 0x02,
};

static char adrsft3[] = {
	0x00,
	0x83,
};

static char adrsft4[] = {
	0xf3,
	0xca,
};

static char adrsft5[] = {
	0x00,
	0x90,
};

static char adrsft6[] = {
	0xc4,
	0x00,
};

static char adrsft7[] = {
	0x00,
	0xb4,
};

static char adrsft8[] = {
	0xc0,
	0xc0,
};

static char adrsft9[] = {
	0x00,
	0x87,
};

static char pixel_eyes_setting[] = {
	0xa4,
	0x15,
};

static char adrsft10[] = {
	0x00,
	0x00,
};
/*
static char caset_data[] = {
	0x2A,
	0x00,0x00,0x04, 0x37,
};

static char paset_data[] = {
	0x2B,
	0x00,0x00,0x07,0x7f,
};
*/
static char tear_on[] = {
	0x35,
	0x00,
};

static char bl_enable[] = {
	0x53,
	0x24,
};
/*******************************************************************************
**display effect
*/
//ce
//0x00 0x00
static char orise_clever_mode[] = {
	0x59,
	0x03,
};

static char addr_shift_a0[] = {
	0x00,
	0xa0,
};

static char ce_d6_1[] = {
	0xD6,
	0x03, 0x01, 0x00, 0x00, 0x00,
	0x00, 0xfd, 0x00, 0x03, 0x06,
	0x06, 0x02,
};

static char addr_shift_b0[] = {
	0x00,
	0xb0,
};

static char ce_d6_2[] = {
	0xD6,
	0x00, 0x00, 0x66, 0xb3, 0xcd,
	0xb3, 0xcd, 0xb3, 0xa6, 0xb3,
	0xcd, 0xb3,
};

static char addr_shift_c0[] = {
	0x00,
	0xc0,
};

static char ce_d6_3[] = {
	0xD6,
	0x37, 0x00, 0x89, 0x77, 0x89,
	0x77, 0x89, 0x77, 0x6f, 0x77,
	0x89, 0x77,
};

static char addr_shift_d0[] = {
	0x00,
	0xd0,
};

static char ce_d6_4[] = {
	0xD6,
	0x37, 0x3c, 0x44, 0x3c, 0x44,
	0x3c, 0x44, 0x3c, 0x37, 0x3c,
	0x44, 0x3c,
};

//0x00 0x80
static char ce_cmd[] = {
	0xD6,
	0x3A,
};

//sharpness
//0x00 0x00
static char sp_cmd[] = {
	0x59,
	0x03,
};

static char sp_shift_0x90[] = {
	0x00,
	0x90,
};

static char sp_D7_1[] = {
	0xD7,
	0x83,
};

static char sp_shift_0x92[] = {
	0x00,
	0x92,
};

static char sp_D7_2[] = {
	0xD7,
	0xff,
};

static char sp_shift_0x93[] = {
	0x00,
	0x93,
};

static char sp_D7_3[] = {
	0xD7,
	0x00,
};

//CABC
//0x00 0x00
//0x59 0x03
//0x00 0x80
static char cabc_ca[] = {
	0xCA,
	0x80,0x88,0x90,0x98,0xa0,
	0xa8,0xb0,0xb8,0xc0,0xc7,
	0xcf,0xd7,0xdf,0xe7,0xef,
	0xf7,0xcc,0xff,0xa5,0xff,
	0x80,0xff,0x05,0x03,0x05,
	0x03,0x05,0x03,
};
//0x00 0x00
static char cabc_c6_G1[] = {
	0xc6,
	0x10,
};
static char cabc_c7_G1[] = {
	0xC7,
	0xf0,0x8e,0xbc,0x9d,0xac,
	0x9c,0xac,0x9b,0xab,0x8c,
	0x67,0x55,0x45,0x44,0x44,
	0x44,0x44,0x44
};

//0x00 0x00
static char cabc_c6_G2[] = {
	0xc6,
	0x11,
};
static char cabc_c7_G2[] = {
	0xC7,
	0xf0,0xac,0xab,0xbc,0xba,
	0x9b,0xab,0xba,0xb8,0xab,
	0x78,0x56,0x55,0x44,0x44,
	0x44,0x44,0x44
};

//0x00 0x00
static char cabc_c6_G3[] = {
	0xc6,
	0x12,
};
static char cabc_c7_G3[] = {
	0xC7,
	0xf0,0xab,0xaa,0xab,0xab,
	0xab,0xaa,0xaa,0xa9,0x9b,
	0x8a,0x67,0x55,0x45,0x44,
	0x44,0x44,0x44
};

//0x00 0x00
static char cabc_c6_G4[] = {
	0xc6,
	0x13,
};
static char cabc_c7_G4[] = {
	0xC7,
	0xf0,0xaa,0xaa,0xab,0x9b,
	0x9b,0xaa,0xa9,0xa9,0xa9,
	0x9a,0x78,0x56,0x55,0x44,
	0x44,0x44,0x44
};

//0x00 0x00
static char cabc_c6_G5[] = {
	0xc6,
	0x14,
};
static char cabc_c7_G5[] = {
	0xC7,
	0xf0,0xa9,0xaa,0xab,0x9a,
	0xaa,0xa9,0xa9,0x8a,0xa9,
	0x99,0x8a,0x67,0x55,0x55,
	0x44,0x44,0x33
};

//0x00 0x00
static char cabc_c6_G6[] = {
	0xc6,
	0x15,
};
static char cabc_c7_G6[] = {
	0xC7,
	0xf0,0xa8,0xaa,0xaa,0x7b,
	0xab,0x99,0x9a,0x99,0x99,
	0x8b,0xa9,0x55,0x55,0x55,
	0x55,0x45,0x44
};

//0x00 0x00
static char cabc_c6_G7[] = {
	0xc6,
	0x16,
};
static char cabc_c7_G7[] = {
	0xC7,
	0xf0,0xa7,0xaa,0xaa,0x8a,
	0xaa,0x99,0x99,0x99,0xa8,
	0x89,0xa9,0x8a,0x67,0x54,
	0x44,0x44,0x33
};

//0x00 0x00
static char cabc_c6_G8[] = {
	0xc6,
	0x17,
};
static char cabc_c7_G8[] = {
	0xC7,
	0xf0,0x97,0xaa,0xaa,0x89,
	0xaa,0xa8,0x88,0x9a,0xa7,
	0xa8,0xa7,0x66,0x66,0x66,
	0x56,0x55,0x55
};

//0x00 0x00
static char cabc_c6_G9[] = {
	0xc6,
	0x18,
};
static char cabc_c7_G9[] = {
	0xC7,
	0xf0,0x87,0x9a,0x9b,0x8a,
	0xa9,0xa8,0x88,0xa9,0x87,
	0x9a,0x88,0x89,0x67,0x56,
	0x55,0x55,0x55
};

//0x00 0x00
static char cabc_c6_G10[] = {
	0xc6,
	0x19,
};
static char cabc_c7_G10[] = {
	0xC7,
	0xe0,0x97,0x8a,0x9b,0x8a,
	0x99,0x99,0x98,0xa8,0x87,
	0x8a,0x79,0x8a,0x67,0x66,
	0x56,0x55,0x55
};

//0x00 0x00
static char cabc_c6_G11[] = {
	0xc6,
	0x1a,
};
static char cabc_c7_G11[] = {
	0xC7,
	0xe0,0xa6,0x89,0xaa,0xa9,
	0x98,0x8a,0x88,0x89,0x79,
	0x7a,0x7a,0x98,0x78,0x66,
	0x66,0x56,0x45
};

//0x00 0x00
static char cabc_c6_G12[] = {
	0xc6,
	0x1b,
};
static char cabc_c7_G12[] = {
	0xC7,
	0xb0,0x99,0x99,0x99,0x9a,
	0x98,0x88,0x88,0x89,0x88,
	0x98,0x79,0x88,0x8a,0x67,
	0x66,0x66,0x55
};

//0x00 0x00
static char cabc_c6_G13[] = {
	0xc6,
	0x1c,
};
static char cabc_c7_G13[] = {
	0xC7,
	0xe0,0x96,0x89,0x9a,0x89,
	0xb7,0x88,0x88,0x88,0x88,
	0x88,0x89,0x87,0xa8,0x98,
	0x78,0x56,0x34
};

//0x00 0x00
static char cabc_c6_G14[] = {
	0xc6,
	0x1d,
};
static char cabc_c7_G14[] = {
	0xC7,
	0xb0,0x89,0x89,0xa9,0x98,
	0x88,0x7a,0x89,0x97,0x88,
	0x78,0x78,0x77,0x89,0x79,
	0x88,0x67,0x56
};

//0x00 0x00
static char cabc_c6_G15[] = {
	0xc6,
	0x1e,
};
static char cabc_c7_G15[] = {
	0xC7,
	0xb0,0x98,0xa8,0xa7,0x89,
	0x88,0x98,0x88,0x78,0x88,
	0x88,0x88,0x77,0x77,0x77,
	0x56,0x98,0x9A
};

//0x00 0x00
static char cabc_c6_G16[] = {
	0xc6,
	0x1f,
};
static char cabc_c7_G16[] = {
	0xC7,
	0x90,0x8a,0x89,0x99,0x98,
	0x88,0x88,0x88,0x88,0x88,
	0x77,0x88,0x78,0x88,0x77,
	0x77,0x89,0x78
};

static char cabc_shift_UI[] = {
	0x00,
	0x90,
};

static char cabc_UI_mode[] = {
	0xCA,
	0xCC, 0xFF,
};

static char cabc_shift_STILL[] = {
	0x00,
	0x92,
};

static char cabc_STILL_mode[] = {
	0xCA,
	0xA5, 0xFF,
};

static char cabc_shift_moving[] = {
	0x00,
	0x94,
};

static char cabc_moving_mode[] = {
	0xCA,
	0x80, 0xFF,
};

//0x00 0x00
static char cabc_disable_curve[] = {
	0xc6,
	0x00,
};

//0x00 0x00
static char cabc_disable_setting[] = {
	0x59,
	0x00,
};

static char cabc_53[] = {
	0x53,
	0x2c,
};
/*static char cabc_set_mode_UI[] = {
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
};*/


/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/
static char exit_sleep[] = {
	0x11,
};

static char display_on[] = {
	0x29,
};

static char display_off[] = {
	0x28,
};

static char enter_sleep[] = {
	0x10,
};

static char Delay_TE[] = {
	0x44,
	0x07, 0x80,
};

/*static char soft_reset[] = {
	0x01,
};*/

static struct dsi_cmd_desc jdi_display_effect_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(cmd2_ena1), cmd2_ena1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft2), adrsft2},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(cmd2_ena2), cmd2_ena2},
	//CE
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(orise_clever_mode), orise_clever_mode},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(addr_shift_a0), addr_shift_a0},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(ce_d6_1), ce_d6_1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(addr_shift_b0), addr_shift_b0},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(ce_d6_2), ce_d6_2},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(addr_shift_c0), addr_shift_c0},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(ce_d6_3), ce_d6_3},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(addr_shift_d0), addr_shift_d0},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(ce_d6_4), ce_d6_4},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft2), adrsft2},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(ce_cmd), ce_cmd},
	//sharpness
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_cmd), sp_cmd},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_shift_0x90), sp_shift_0x90},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_D7_1), sp_D7_1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_shift_0x92), sp_shift_0x92},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_D7_2), sp_D7_2},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_shift_0x93), sp_shift_0x93},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_D7_3), sp_D7_3},
	//cabc
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft2), adrsft2},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_ca), cabc_ca},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G1), cabc_c6_G1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G1), cabc_c7_G1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G2), cabc_c6_G2},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G2), cabc_c7_G2},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G3), cabc_c6_G3},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G3), cabc_c7_G3},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G4), cabc_c6_G4},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G4), cabc_c7_G4},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G5), cabc_c6_G5},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G5), cabc_c7_G5},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G6), cabc_c6_G6},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G6), cabc_c7_G6},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G7), cabc_c6_G7},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G7), cabc_c7_G7},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G8), cabc_c6_G8},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G8), cabc_c7_G8},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G9), cabc_c6_G9},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G9), cabc_c7_G9},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G10), cabc_c6_G10},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G10), cabc_c7_G10},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G11), cabc_c6_G11},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G11), cabc_c7_G11},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G12), cabc_c6_G12},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G12), cabc_c7_G12},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G13), cabc_c6_G13},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G13), cabc_c7_G13},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G14), cabc_c6_G14},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G14), cabc_c7_G14},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G15), cabc_c6_G15},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G15), cabc_c7_G15},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c6_G16), cabc_c6_G16},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_c7_G16), cabc_c7_G16},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_shift_UI), cabc_shift_UI},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_UI_mode), cabc_UI_mode},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_shift_STILL), cabc_shift_STILL},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_STILL_mode), cabc_STILL_mode},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_shift_moving), cabc_shift_moving},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_moving_mode), cabc_moving_mode},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_disable_curve), cabc_disable_curve},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(adrsft1), adrsft1},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_disable_setting), cabc_disable_setting},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_53), cabc_53},
	{DTYPE_DCS_WRITE1, 0,1, WAIT_TYPE_MS,
		sizeof(adrsft3), adrsft3},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft4), adrsft4},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft5), adrsft5},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft6), adrsft6},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft7), adrsft7},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft8), adrsft8},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft9), adrsft9},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(pixel_eyes_setting), pixel_eyes_setting},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(adrsft10), adrsft10},
};

static struct dsi_cmd_desc jdi_display_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
		sizeof(tear_on), tear_on},
	//{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
	//	sizeof(caset_data), caset_data},
	//{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
	//	sizeof(paset_data), paset_data},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(Delay_TE), Delay_TE},
	{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 0, 100, WAIT_TYPE_MS,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc jdi_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 0, 60, WAIT_TYPE_MS,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(enter_sleep), enter_sleep}
};

static char command_2_enable[] = {
	0x00,
	0x00,
};

static char command_2_enable_1_para[] = {
	0xFF,
	0x19, 0x02, 0x01,
};

static char command_2_enable_2[] = {
	0x00,
	0x80,
};

static char command_2_enable_2_para[] = {
	0xFF,
	0x19, 0x02,
};

static char HD720_setting_1_para[] = {
	0x2A,
	0x00, 0x00, 0x02, 0xCF,
};

static char HD1080_setting_1_para[] = {
	0x2a,
	0x00, 0x00, 0x04, 0x37,
};

static char HD720_setting_2_para[] = {
	0x2B,
	0x00, 0x00, 0x04, 0xFF,
};

static char HD1080_setting_2_para[] = {
	0x2b,
	0x00, 0x00, 0x07, 0x7f,
};

static char cleveredge_1_5x_para[] = {
	0x1C,
	0x05,
};

static char cleveredge_disable[] = {
	0x1C,
	0x00,
};

static char cleveredge_P1[] = {
	0x00,
	0x91,
};

static char cleveredge_P1_para[] = {
	0xD7,
	0xC8,
};

static char cleveredge_P2[] = {
	0x00,
	0x93,
};

static char cleveredge_P2_para[] = {
	0xD7,
	0x08,
};

static char cleveredge_use_setting[] = {
	0x00,
	0xAC,
};

static char cleveredge_use_setting_para[] = {
	0xC0,
	0x04,
};

static char command_2_disable_para[] = {
	0xFF,
	0xFF, 0xFF, 0xFF, 0xFF,
};

static char command_clevermode[] = {
	0x59,
	0x03,
};

static struct dsi_cmd_desc cleveredge_inital_720P_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable_1_para), command_2_enable_1_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable_2), command_2_enable_2},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable_2_para), command_2_enable_2_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_clevermode), command_clevermode},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(HD720_setting_1_para), HD720_setting_1_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(HD720_setting_2_para), HD720_setting_2_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(cleveredge_1_5x_para), cleveredge_1_5x_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(cleveredge_P1), cleveredge_P1},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(cleveredge_P1_para), cleveredge_P1_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(cleveredge_P2), cleveredge_P2},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(cleveredge_P2_para), cleveredge_P2_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(cleveredge_use_setting), cleveredge_use_setting},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(cleveredge_use_setting_para), cleveredge_use_setting_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_disable_para), command_2_disable_para}
};

static struct dsi_cmd_desc cleveredge_inital_1080P_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable_1_para), command_2_enable_1_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable_2), command_2_enable_2},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable_2_para), command_2_enable_2_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(HD1080_setting_1_para), HD1080_setting_1_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(HD1080_setting_2_para), HD1080_setting_2_para},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(command_2_enable), command_2_enable},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(cleveredge_disable), cleveredge_disable},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_shift_0x93), sp_shift_0x93},
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(sp_D7_3), sp_D7_3},
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

static uint32_t	gpio_lcd_bl_enable;  /*gpio_4_3, gpio_035*/
static uint32_t	gpio_lcd_reset;  /*gpio_4_5, gpio_037*/
static uint32_t	gpio_lcd_id;  /*gpio_4_6, gpio_038*/
static uint32_t	gpio_lcd_p5v5_enable;  /*gpio_5_1, gpio_041*/
static uint32_t	gpio_lcd_n5v5_enable;  /*gpio_5_2, gpio_042*/

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
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 1},
	/* AVEE_-5.5V */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 1},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_US, 50,
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

extern void ts_power_gpio_enable(void);
extern void ts_power_gpio_disable(void);
static int mipi_jdi_panel_on(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	uint32_t status = 0;
	uint32_t try_times = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	pinfo = &(hisifd->panel_info);
	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
		LOG_JANK_D(JLID_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
		g_debug_enable = true;

		/* lcd vcc enable */
		vcc_cmds_tx(pdev, jdi_lcd_vcc_enable_cmds,
			ARRAY_SIZE(jdi_lcd_vcc_enable_cmds));
		if (g_vddio_type)
			ts_power_gpio_enable();

		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE) {
		/* lcd pinctrl normal */
		pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds,
			ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));

		/* lcd gpio request */
		gpio_cmds_tx(jdi_lcd_gpio_request_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_request_cmds));

		/* lcd gpio normal */
		gpio_cmds_tx(jdi_lcd_gpio_normal_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_normal_cmds));

		if (DISPLAY_LOW_POWER_LEVEL_HD == hisifd->switch_res_flag) {
			mipi_dsi_cmds_tx(cleveredge_inital_720P_cmds,
				ARRAY_SIZE(cleveredge_inital_720P_cmds), mipi_dsi0_base);
		} else {
			mipi_dsi_cmds_tx(cleveredge_inital_1080P_cmds,
				ARRAY_SIZE(cleveredge_inital_1080P_cmds), mipi_dsi0_base);
		}

		/* lcd color management sequence */
		mipi_dsi_cmds_tx(jdi_display_effect_cmds, \
			ARRAY_SIZE(jdi_display_effect_cmds), mipi_dsi0_base);

		mipi_dsi_cmds_tx(jdi_display_on_cmds, \
			ARRAY_SIZE(jdi_display_on_cmds), mipi_dsi0_base);

		/*Read LCD power state*/
		outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0x0A06);
		status = inp32(mipi_dsi0_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
		while (status & 0x10) {
			udelay(50);
			if (++try_times > 100) {
				try_times = 0;
				HISI_FB_ERR("Read lcd power status timeout!\n");
				break;
			}

			status = inp32(mipi_dsi0_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
		}
		status = inp32(mipi_dsi0_base + MIPIDSI_GEN_PLD_DATA_OFFSET);
		HISI_FB_INFO("LCD Power State = 0x%x.\n", status);

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

	/* lcd gpio lowpower */
	gpio_cmds_tx(jdi_lcd_gpio_lowpower_cmds, \
		ARRAY_SIZE(jdi_lcd_gpio_lowpower_cmds));
	/* lcd gpio free */
	gpio_cmds_tx(jdi_lcd_gpio_free_cmds, \
		ARRAY_SIZE(jdi_lcd_gpio_free_cmds));

	/* lcd pinctrl lowpower */
	pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_lowpower_cmds,
		ARRAY_SIZE(jdi_lcd_pinctrl_lowpower_cmds));

	/* lcd vcc disable */
	vcc_cmds_tx(pdev, jdi_lcd_vcc_disable_cmds,
		ARRAY_SIZE(jdi_lcd_vcc_disable_cmds));

	if (g_vddio_type)
		ts_power_gpio_disable();

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_jdi_panel_remove(struct platform_device *pdev)
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
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

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
		;
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

	ret = snprintf(buf, PAGE_SIZE, "jdi_OTM1902B 5.0' CMD TFT 1080 x 1920\n");

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t mipi_jdi_panel_lcd_cabc_mode_show(struct platform_device *pdev,
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

static ssize_t mipi_jdi_panel_lcd_cabc_mode_store(struct platform_device *pdev,
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

static ssize_t mipi_jdi_panel_lcd_check_reg_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	u32 read_value[4] = {0};
	u32 expected_value[4] = {0x9c, 0x00, 0x07, 0x00};
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
	u32 read_mask[4] = {0xFF, 0xFF};
	char* reg_name[4] = {"signal mode", "dsi error number"};
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
	u32 read_value = 0xFF;
	char packet_size[] = {0x01};
	char lcd_reg_aa[] = {0xaa};

	struct dsi_cmd_desc set_packet_size[] = {
		{DTYPE_MAX_PKTSIZE, 0, 10, WAIT_TYPE_US,
			sizeof(packet_size), packet_size},
	};

	struct dsi_cmd_desc lcd_check_reg[] = {
		{DTYPE_DCS_READ, 0, 20, WAIT_TYPE_US,
			sizeof(lcd_reg_aa), lcd_reg_aa},
	};

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	mipi_dsi_cmds_tx(set_packet_size, \
		ARRAY_SIZE(set_packet_size), mipi_dsi0_base);

	mipi_dsi_cmds_rx(&read_value, lcd_check_reg, \
		ARRAY_SIZE(lcd_check_reg), mipi_dsi0_base);

	ret = snprintf(buf, PAGE_SIZE, "%d 0 0 0 0 0 0 0\n", read_value);
	HISI_FB_INFO("%d\n", read_value);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
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

static int mipi_jdi_panel_set_display_resolution(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	int retval = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);

	switch (hisifd->switch_res_flag) {
	case DISPLAY_LOW_POWER_LEVEL_HD:
		pinfo->xres = 720;
		pinfo->yres = 1280;
		pinfo->pxl_clk_rate = 76 * 1000000;
		pinfo->mipi.dsi_bit_clk = 241;
		pinfo->dirty_region_updt_support = 0;
		break;
	case DISPLAY_LOW_POWER_LEVEL_FHD:
		pinfo->xres = 1080;
		pinfo->yres = 1920;
		pinfo->pxl_clk_rate = 150 * 1000000;
		pinfo->mipi.dsi_bit_clk = 480;
		pinfo->dirty_region_updt_support = 1;
		break;
	default:
		retval =  -1;
	}

	return retval;
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
	.set_display_region = mipi_jdi_panel_set_display_region,
	.set_display_resolution = mipi_jdi_panel_set_display_resolution,
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

	if (hisi_fb_device_probe_defer(PANEL_MIPI_CMD)) {
		goto err_probe_defer;
	}

	HISI_FB_DEBUG("+.\n");

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_JDI_OTM1902B);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_JDI_OTM1902B);
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
	pinfo->xres = 1080;
	pinfo->yres = 1920;
	pinfo->width  = 61;  //mm
	pinfo->height = 109; //mm
	pinfo->type = PANEL_MIPI_CMD;
	pinfo->orientation = LCD_PORTRAIT;
	pinfo->bpp = LCD_RGB888;
	pinfo->bgr_fmt = LCD_RGB;
	pinfo->bl_set_type = bl_type;

	pinfo->bl_min = 1;
	pinfo->bl_max = 255;

	pinfo->vsync_ctrl_type = (VSYNC_CTRL_ISR_OFF |
		VSYNC_CTRL_MIPI_ULPS | VSYNC_CTRL_CLK_OFF);
	pinfo->frc_enable = 0;
	pinfo->esd_enable = 0;
	pinfo->dirty_region_updt_support = 1;
    pinfo->dirty_region_info.left_align = 2;
    pinfo->dirty_region_info.right_align = 2;
    pinfo->dirty_region_info.top_align = 2;
    pinfo->dirty_region_info.bottom_align = 2;
    pinfo->dirty_region_info.w_align = -1;
    pinfo->dirty_region_info.h_align = -1;
    pinfo->dirty_region_info.w_min = -1;
    pinfo->dirty_region_info.h_min = -1;
    pinfo->dirty_region_info.top_start = -1;
    pinfo->dirty_region_info.bottom_start = -1;

	pinfo->sbl_support = 0;
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
        .compatible = DTS_COMP_JDI_OTM1902B,
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
		.name = "mipi_jdi_OTM1902B",
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
