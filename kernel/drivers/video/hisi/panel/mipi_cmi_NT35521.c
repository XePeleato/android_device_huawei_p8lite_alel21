/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above
 *	   copyright notice, this list of conditions and the following
 *	   disclaimer in the documentation and/or other materials provided
 *	   with the distribution.
 *	 * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *	   contributors may be used to endorse or promote products derived
 *	   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/pwm.h>
#include <linux/gpio.h>

#include <linux/hisi/hi6xxx-lcd_type.h>

#include "balong_fb.h"
#include "balong_fb_def.h"
#include "mipi_dsi.h"
#include "mipi_reg.h"
#include <linux/lcd_tuning.h>

#if defined (CONFIG_HUAWEI_DSM)
#include <huawei_platform/dsm/dsm_pub.h>
#endif

#define PWM_LEVEL 100

#define CMI_CABC_ENABLED  1
//As the reg0x55 used for CABC and IE/SRE, so add two global variables to mark respective status. 
static int g_IE_SRE_level = 0x80;


/*--------cmi power on initial code -----------*/
static char cmi_power_on_param1[] =
{
    0xff,
    0xAA, 0x55, 0xA5, 0x80,
};

static char cmi_power_on_param2[] =
{
    0x6F,
    0x11, 0x00,
};

static char cmi_power_on_param3[] =
{
    0xF7,
    0x20, 0x00,
};

static char cmi_power_on_param4[] =
{
    0x6F,
    0x06,
};

static char cmi_power_on_param5[] =
{
    0xF7,
    0xA0,
};

static char cmi_power_on_param6[] =
{
    0x6F,
    0x19,
};

static char cmi_power_on_param7[] =
{
    0xF7,
    0x12,
};
// updated for VGL precharge function enable by Novatek 0722
static char cmi_power_on_paramF4[] =
{
    0xF4,
    0x03,
};
static char cmi_power_on_param8[] =
{
    0xF0,
    0x55, 0xAA, 0x52, 0x08, 0x00,
};
//change for esd bug
static char cmi_power_on_param9[] =
{
    0xC8,
    /*resolve esd result in lcd blank screen */
    0x80,//0x00,
};
static char cmi_power_on_param10[] =
{
    0xB1,
    0x68, 0x21,
};

static char cmi_power_on_param11[] =
{
    0xB6,
    0x08,
};

static char cmi_power_on_param12[] =
{
    0x6F,
    0x02,
};

static char cmi_power_on_param13[] =
{
    0xB8,
    0x08,
};

static char cmi_power_on_param14[] =
{
    0xBB,
    0x74, 0x44,
};

static char cmi_power_on_param15[] =
{
    0xBC,
    0x00, 0x00,
};

static char cmi_power_on_param16[] =
{
    0xBD,
    0x02, 0xB0, 0x1E, 0x1E, 0x00,
};

//optimize mipi clk for 2 Lane application
static char cmi_power_on_paramC5[] =
{
    0xC5,
    0x01, 0x07,
};

static char cmi_power_on_param17[] =
{
    0xF0,
    0x55, 0xAA, 0x52, 0x08, 0x01,
};

static char cmi_power_on_param18[] =
{
    0xB0,
    0x05, 0x05,
};

static char cmi_power_on_param19[] =
{
    0xB1,
    0x05, 0x05,
};

static char cmi_power_on_param20[] =
{
    0xBC,
    0xA0, 0x01,
};

static char cmi_power_on_param21[] =
{
    0xBD,
    0xA0, 0x01,
};

static char cmi_power_on_param22[] =
{
    0xCA,
    0x00,
};

static char cmi_power_on_param23[] =
{
    0xC0,
    0x04,
};
/* VCOM
static char cmi_power_on_param23[] = {
	0xBE,
    0x51,
};
*/

static char cmi_power_on_param24[] =
{
    0xB3,
    0x37, 0x37,
};

static char cmi_power_on_param25[] =
{
    0xB4,
    0x0F, 0x0F,
};
// updated for VDDA pump clock by Novatek  0722
static char cmi_power_on_paramB6[] =
{
    0xB6,
    0x05, 0x05,
};
static char cmi_power_on_param26[] =
{
    0xB9,
    /*resolve esd result in lcd blank screen */
    0x36, 0x36,//0x46, 0x46,
};

//modify pump clock to 0x25
static char cmi_power_on_param27[] =
{
    0xBA,
    0x25, 0x25,  //0x24,0x24
};

static char cmi_power_on_param28[] =
{
    0xF0,
    0x55, 0xAA, 0x52, 0x08, 0x02,
};
// Use 1 gamma code and enable CABC function
static char cmi_power_on_param29[] =
{
	0xEE,
	0x01,
};
static char cmi_power_on_param30[] =
{
    0xB0,
    0x00,0x00,0x00,0x42,0x00,0x88,0x00,0xA6,0x00,0xBF,0x00,0xE6,0x01,0x05,0x01,0x32,
};
static char cmi_power_on_param31[] =
{
    0xB1,
    0x01,0x58,0x01,0x94,0x01,0xC1,0x02,0x08,0x02,0x40,0x02,0x42,0x02,0x78,0x02,0xB4,
};
static char cmi_power_on_param32[] =
{
    0xB2,
    0x02,0xD9,0x03,0x0C,0x03,0x30,0x03,0x5F,0x03,0x7F,0x03,0xA5,0x03,0xC0,0x03,0xE0,
};
static char cmi_power_on_param33[] =
{
    0xB3,
    0x03,0xF6,0x03,0xFF,
};
static char cmi_power_on_param46[] =
{
    0xF0,
    0x55, 0xAA, 0x52, 0x08, 0x06,
};

static char cmi_power_on_param47[] =
{
    0xB0,
    0x29, 0x2A,
};

static char cmi_power_on_param48[] =
{
    0xB1,
    0x10, 0x12,
};

static char cmi_power_on_param49[] =
{
    0xB2,
    0x14, 0x16,
};

static char cmi_power_on_param50[] =
{
    0xB3,
    0x18, 0x1A,
};

static char cmi_power_on_param51[] =
{
    0xB4,
    0x08, 0x0A,
};

static char cmi_power_on_param52[] =
{
    0xB5,
    0x2E, 0x2E,
};

static char cmi_power_on_param53[] =
{
    0xB6,
    0x2E, 0x2E,
};

static char cmi_power_on_param54[] =
{
    0xB7,
    0x2E, 0x2E,
};

static char cmi_power_on_param55[] =
{
    0xB8,
    0x2E, 0x00,
};

static char cmi_power_on_param56[] =
{
    0xB9,
    0x2E, 0x2E,
};

static char cmi_power_on_param57[] =
{
    0xBA,
    0x2E, 0x2E,
};

static char cmi_power_on_param58[] =
{
    0xBB,
    0x01, 0x2E,
};

static char cmi_power_on_param59[] =
{
    0xBC,
    0x2E, 0x2E,
};

static char cmi_power_on_param60[] =
{
    0xBD,
    0x2E, 0x2E,
};

static char cmi_power_on_param61[] =
{
    0xBE,
    0x2E, 0x2E,
};

static char cmi_power_on_param62[] =
{
    0xBF,
    0x0B, 0x09,
};

static char cmi_power_on_param63[] =
{
    0xC0,
    0x1B, 0x19,
};

static char cmi_power_on_param64[] =
{
    0xC1,
    0x17, 0x15,
};

static char cmi_power_on_param65[] =
{
    0xC2,
    0x13, 0x11,
};

static char cmi_power_on_param66[] =
{
    0xC3,
    0x2A, 0x29,
};

static char cmi_power_on_param67[] =
{
    0xE5,
    0x2E, 0x2E,
};

static char cmi_power_on_param68[] =
{
    0xC4,
    0x29, 0x2A,
};

static char cmi_power_on_param69[] =
{
    0xC5,
    0x1B, 0x19,
};

static char cmi_power_on_param70[] =
{
    0xC6,
    0x17, 0x15,
};

static char cmi_power_on_param71[] =
{
    0xC7,
    0x13, 0x11,
};

static char cmi_power_on_param72[] =
{
    0xC8,
    0x01, 0x0B,
};

static char cmi_power_on_param73[] =
{
    0xC9,
    0x2E, 0x2E,
};

static char cmi_power_on_param74[] =
{
    0xCA,
    0x2E, 0x2E,
};
static char cmi_power_on_param75[] =
{
    0xCB,
    0x2E, 0x2E,
};


static char cmi_power_on_param76[] =
{
    0xCC,
    0x2E, 0x09,
};
static char cmi_power_on_param77[] =
{
    0xCD,
    0x2E, 0x2E,
};
static char cmi_power_on_param78[] =
{
    0xCE,
    0x2E, 0x2E,
};

static char cmi_power_on_param79[] =
{
    0xCF,
    0x08, 0x2E,
};

static char cmi_power_on_param80[] =
{
    0xD0,
    0x2E, 0x2E,
};

static char cmi_power_on_param81[] =
{
    0xD1,
    0x2E, 0x2E,
};

static char cmi_power_on_param82[] =
{
    0xD2,
    0x2E, 0x2E,
};

static char cmi_power_on_param83[] =
{
    0xD3,
    0x0A, 0x00,
};

static char cmi_power_on_param84[] =
{
    0xD4,
    0x10, 0x12,
};

static char cmi_power_on_param85[] =
{
    0xD5,
    0x14, 0x16,
};

static char cmi_power_on_param86[] =
{
    0xD6,
    0x18, 0x1A,
};

static char cmi_power_on_param87[] =
{
    0xD7,
    0x2A, 0x29,
};

static char cmi_power_on_param88[] =
{
    0xE6,
    0x2E, 0x2E,
};

static char cmi_power_on_param89[] =
{
    0xD8,
    0x00, 0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param90[] =
{
    0xD9,
    0x00, 0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param91[] =
{
    0xE7,
    0x00,
};

static char cmi_power_on_param92[] =
{
    0xF0,
    0x55, 0xAA, 0x52, 0x08, 0x03,
};

static char cmi_power_on_param93[] =
{
    0xB0,
    0x00, 0x00,
};

static char cmi_power_on_param94[] =
{
    0xB1,
    0x00, 0x00,
};

static char cmi_power_on_param95[] =
{
    0xB2,
    0x05, 0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param96[] =
{
    0xB6,
    0x05, 0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param97[] =
{
    0xB7,
    0x05, 0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param98[] =
{
    0xBA,
    0x57, 0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param99[] =
{
    0xBB,
    0x57, 0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param100[] =
{
    0xC0,
    0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param101[] =
{
    0xC1,
    0x00, 0x00, 0x00, 0x00,
};

static char cmi_power_on_param102[] =
{
    0xC4,
    0x60,
};

static char cmi_power_on_param103[] =
{
    0xC5,
    0x40,
};

static char cmi_power_on_param104[] =
{
    0xF0,
    0x55, 0xAA, 0x52, 0x08, 0x05,
};

static char cmi_power_on_param105[] =
{
    0xBD,
    0x03, 0x01, 0x03, 0x03, 0x03,
};

static char cmi_power_on_param106[] =
{
    0xB0,
    0x17, 0x06,
};

static char cmi_power_on_param107[] =
{
    0xB1,
    0x17, 0x06,
};

static char cmi_power_on_param108[] =
{
    0xB2,
    0x17, 0x06,
};

static char cmi_power_on_param109[] =
{
    0xB3,
    0x17, 0x06,
};

static char cmi_power_on_param110[] =
{
    0xB4,
    0x17, 0x06,
};

static char cmi_power_on_param111[] =
{
    0xB5,
    0x17, 0x06,
};

static char cmi_power_on_param112[] =
{
    0xB8,
    0x00,
};

static char cmi_power_on_param113[] =
{
    0xB9,
    0x00,
};

static char cmi_power_on_param114[] =
{
    0xBA,
    0x00,
};

static char cmi_power_on_param115[] =
{
    0xBB,
    0x02,
};

static char cmi_power_on_param116[] =
{
    0xBC,
    0x00,
};

static char cmi_power_on_param117[] =
{
    0xC0,
    0x07,
};

static char cmi_power_on_param118[] =
{
    0xC4,
    0x80,
};

static char cmi_power_on_param119[] =
{
    0xC5,
    0xA4,
};

static char cmi_power_on_param120[] =
{
    0xC8,
    0x05, 0x30,
};

static char cmi_power_on_param121[] =
{
    0xC9,
    0x01, 0x31,
};

static char cmi_power_on_param122[] =
{
    0xCC,
    0x00, 0x00, 0x3C,
};

static char cmi_power_on_param123[] =
{
    0xCD,
    0x00, 0x00, 0x3C,
};

static char cmi_power_on_param124[] =
{
    0xD1,
    0x00, 0x05, 0x09, 0x07, 0x10,
};

static char cmi_power_on_param125[] =
{
    0xD2,
    0x00, 0x05, 0x0E, 0x07, 0x10,
};

static char cmi_power_on_param126[] =
{
    0xE5,
    0x06,
};

static char cmi_power_on_param127[] =
{
    0xE6,
    0x06,
};

static char cmi_power_on_param128[] =
{
    0xE7,
    0x06,
};

static char cmi_power_on_param129[] =
{
    0xE8,
    0x06,
};

static char cmi_power_on_param130[] =
{
    0xE9,
    0x06,
};

static char cmi_power_on_param131[] =
{
    0xEA,
    0x06,
};

static char cmi_power_on_param132[] =
{
    0xED,
    0x30,
};

static char cmi_power_on_param133[] =
{
    0x6F,
    0x11,
};

static char cmi_power_on_param134[] =
{
    0xF3,
    0x01,
};

/*set pwm freq to 39.06KHz*/
static char cmi_power_on_param135[] =
{
    0xF0,
    0x55, 0xAA, 0x52, 0x08, 0x00,
};

static char cmi_power_on_param136[] =
{
    0xD9,
    0x01, 0x01, //0x02, 0x04,
};
// Vivid Color parameters
static char cmi_power_on_paramD1[] = {
    0xD1,
    0x00,0x02,0x06,0x0A,0x0E,0x12,0x15,0x18,0x18,0x18,0x16, 0x14,0x12,0x0A,0x04,0x00,
};
// Smart Color parameters
static char cmi_power_on_param137[] = {
    0xD7,
    0x24,0x23,0x22,0x21,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,
};
static char cmi_power_on_param138[] = {
    0xD8,
    0x00,0x00,0x00,0x00,0x00,0x00, 0x00, 0x00,0x00, 0x00, 0x23,0x22,0x00,
};
//Disable edge enhancement and close CE dimming function
static char cmi_power_on_param139[] =
{
	0xCC,
	0x40,0x36,0xBA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0xFF,0x00,
};
//set cabc PWM level in UI mode
static char cmi_power_on_param140[] = {
    0xE6,
    0xFF, 0xFF, 0xFA, 0xFA,
};
//write cabc minimum brightness
static char cmi_power_on_param141[] = {
    0x5E,
    0x28,
};
/*----------------Power ON Sequence(sleep mode to Normal mode)---------------------*/

/*static char soft_reset[] = {
	0x01,
};*/

static char bl_level_0[] =
{
    0x51,
    0x00,
};

static char bl_level[] =
{
    0x51,
    0x00, //0xd0, //0xd0
};

static char bl_enable[] =
{
    0x53,
    0x24,
};

//CABC
#if  CMI_CABC_ENABLED
//Init cabc to UI mode when panel on
static char cabc_mode[] =
{
    0x55,
    0x81,
};
#endif

//TE
/*
static char te_enable[] = {
	0x35,
	0x00,
};
*/

static char exit_sleep[] =
{
    0x11,
};

static char display_on[] =
{
    0x29,
};

/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/
static char display_off[] =
{
    0x28,
};

static char enter_sleep[] =
{
    0x10,
};

static struct dsi_cmd_desc cmi_display_on_cmds[] =
{
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param1), cmi_power_on_param1
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param2), cmi_power_on_param2
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param3), cmi_power_on_param3
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param4), cmi_power_on_param4
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param5), cmi_power_on_param5
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param6), cmi_power_on_param6
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param7), cmi_power_on_param7
    },
	{
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_paramF4), cmi_power_on_paramF4
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param8), cmi_power_on_param8
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param9), cmi_power_on_param9
    },

    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param10), cmi_power_on_param10
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param11), cmi_power_on_param11
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param12), cmi_power_on_param12
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param13), cmi_power_on_param13
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param14), cmi_power_on_param14
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param15), cmi_power_on_param15
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param16), cmi_power_on_param16
    },

    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_paramC5), cmi_power_on_paramC5
    },

    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param17), cmi_power_on_param17
    },

    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param18), cmi_power_on_param18
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param19), cmi_power_on_param19
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param20), cmi_power_on_param20
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param21), cmi_power_on_param21
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param22), cmi_power_on_param22
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param23), cmi_power_on_param23
    },

    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param24), cmi_power_on_param24
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param25), cmi_power_on_param25
    },
	{
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_paramB6), cmi_power_on_paramB6
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param26), cmi_power_on_param26
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param27), cmi_power_on_param27
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param28), cmi_power_on_param28
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param29), cmi_power_on_param29
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param30), cmi_power_on_param30
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param31), cmi_power_on_param31
    },

    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param32), cmi_power_on_param32
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param33), cmi_power_on_param33
    },	
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param46), cmi_power_on_param46
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param47), cmi_power_on_param47
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param48), cmi_power_on_param48
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param49), cmi_power_on_param49
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param50), cmi_power_on_param50
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param51), cmi_power_on_param51
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param52), cmi_power_on_param52
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param53), cmi_power_on_param53
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param54), cmi_power_on_param54
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param55), cmi_power_on_param55
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param56), cmi_power_on_param56
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param57), cmi_power_on_param57
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param58), cmi_power_on_param58
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param59), cmi_power_on_param59
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param60), cmi_power_on_param60
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param61), cmi_power_on_param61
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param62), cmi_power_on_param62
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param63), cmi_power_on_param63
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param64), cmi_power_on_param64
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param65), cmi_power_on_param65
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param66), cmi_power_on_param66
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param67), cmi_power_on_param67
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param68), cmi_power_on_param68
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param69), cmi_power_on_param69
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param70), cmi_power_on_param70
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param71), cmi_power_on_param71
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param72), cmi_power_on_param72
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param73), cmi_power_on_param73
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param74), cmi_power_on_param74
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param75), cmi_power_on_param75
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param76), cmi_power_on_param76
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param77), cmi_power_on_param77
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param78), cmi_power_on_param78
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param79), cmi_power_on_param79
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param80), cmi_power_on_param80
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param81), cmi_power_on_param81
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param82), cmi_power_on_param82
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param83), cmi_power_on_param83
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param84), cmi_power_on_param84
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param85), cmi_power_on_param85
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param86), cmi_power_on_param86
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param87), cmi_power_on_param87
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param88), cmi_power_on_param88
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param89), cmi_power_on_param89
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param90), cmi_power_on_param90
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param91), cmi_power_on_param91
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param92), cmi_power_on_param92
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param93), cmi_power_on_param93
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param94), cmi_power_on_param94
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param95), cmi_power_on_param95
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param96), cmi_power_on_param96
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param97), cmi_power_on_param97
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param98), cmi_power_on_param98
    },
    {
        DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US,
        sizeof(cmi_power_on_param99), cmi_power_on_param99
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param100), cmi_power_on_param100
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param101), cmi_power_on_param101
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param102), cmi_power_on_param102
    },

    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param103), cmi_power_on_param103
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param104), cmi_power_on_param104
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param105), cmi_power_on_param105
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param106), cmi_power_on_param106
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param107), cmi_power_on_param107
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param108), cmi_power_on_param108
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param109), cmi_power_on_param109
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param110), cmi_power_on_param110
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param111), cmi_power_on_param111
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param112), cmi_power_on_param112
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param113), cmi_power_on_param113
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param114), cmi_power_on_param114
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param115), cmi_power_on_param115
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param116), cmi_power_on_param116
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param117), cmi_power_on_param117
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param118), cmi_power_on_param118
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param119), cmi_power_on_param119
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param120), cmi_power_on_param120
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param121), cmi_power_on_param121
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param122), cmi_power_on_param122
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param123), cmi_power_on_param123
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param124), cmi_power_on_param124
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param125), cmi_power_on_param125
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param126), cmi_power_on_param126
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param127), cmi_power_on_param127
    },

    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param128), cmi_power_on_param128
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param129), cmi_power_on_param129
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param130), cmi_power_on_param130
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param131), cmi_power_on_param131
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param132), cmi_power_on_param132
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param133), cmi_power_on_param133
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param134), cmi_power_on_param134
    },

    /*set pwm freq to 39.06KHz*/
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param135), cmi_power_on_param135
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param136), cmi_power_on_param136
    },
	
	{
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_paramD1), cmi_power_on_paramD1 
    },
	{
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param137), cmi_power_on_param137
    },
	{
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param138), cmi_power_on_param138
    },
	//Disable edge enhancement
	{
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param139), cmi_power_on_param139
    },
	{
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param140), cmi_power_on_param140
    },
	{
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(cmi_power_on_param141), cmi_power_on_param141
    },
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(bl_level), bl_level
    },
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(bl_enable), bl_enable
    },
    #if  CMI_CABC_ENABLED
    {
    	DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
	 	sizeof(cabc_mode), cabc_mode
    },	
    #endif
    //	{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
    //		sizeof(te_enable), te_enable},
    {
        DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
        sizeof(exit_sleep), exit_sleep
    },
    {
        DTYPE_DCS_WRITE, 0, 20, WAIT_TYPE_MS,
        sizeof(display_on), display_on
    },
    //	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
    //		sizeof(bl_level), bl_level},
    //	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
    //		sizeof(bl_enable), bl_enable},


};
static struct dsi_cmd_desc cmi_display_off_cmds[] =
{
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(bl_level_0), bl_level_0
    },
    {
        DTYPE_DCS_WRITE, 0, 20, WAIT_TYPE_MS,
        sizeof(display_off), display_off
    },
    {
        DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
        sizeof(enter_sleep), enter_sleep
    }
};

/************************************************************
 *
 * LCD VCC
 *
 ************************************************************/

#define VCC_BACKLIGHT_NAME		"lcd_backlight"
#define VCC_LCDBIAS_NAME		"vcc_lcdbias"
#define VCC_LCD_VSN_NAME		"lcd_vsn"
#define VCC_LCD_VSP_NAME		"lcd_vsp"

 /* scharg regulator */
static struct regulator *lcd_bl_vcc;
static struct regulator *lcd_bias_vcc;
static struct regulator *lcd_vsn_vcc;
static struct regulator *lcd_vsp_vcc;

static struct vcc_desc cmi_lcd_vcc_get_cmds[] = {
    /* vcc get */
    {DTYPE_VCC_GET, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, 0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  0, 0, 0, 0, 0, 0},
};

static struct vcc_desc cmi_lcd_vcc_set_cmds[] = {
    /* vcc set voltage */
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCDBIAS_NAME, &lcd_bias_vcc,  0, 0, 5400000, 5400000, 0, 0},
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCD_VSP_NAME, &lcd_vsp_vcc,   0, 0, 5400000, 5400000, 0, 0},
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCD_VSN_NAME, &lcd_vsn_vcc,   0, 0, 5400000, 5400000, 0, 0},
};

static struct vcc_desc cmi_lcd_vcc_put_cmds[] = {
	/* vcc put */
    {DTYPE_VCC_PUT, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, 0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  0, 0, 0, 0, 0, 0},
};

static struct vcc_desc cmi_lcd_vcc_enable_cmds[] = {
	/* vcc enable */
    {DTYPE_VCC_ENABLE, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, WAIT_TYPE_MS, 12, 0, 0, 0, 0},
    {DTYPE_VCC_ENABLE, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  WAIT_TYPE_MS, 10, 0, 0, 0, 0},
    {DTYPE_VCC_ENABLE, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  WAIT_TYPE_MS, 50, 0, 0, 0, 0},
};

static struct vcc_desc cmi_lcd_vcc_disable_cmds[] = {
	/* vcc disable */
    {DTYPE_VCC_DISABLE, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  WAIT_TYPE_MS, 5, 0, 0, 0, 0},
    {DTYPE_VCC_DISABLE, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  WAIT_TYPE_MS, 5, 0, 0, 0, 0},
    {DTYPE_VCC_DISABLE, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, WAIT_TYPE_MS, 1, 0, 0, 0, 0},
};

static struct vcc_desc cmi_lcd_bl_enable_cmds[] = {
    /* backlight enable */
    {DTYPE_VCC_ENABLE, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   WAIT_TYPE_MS, 10, 0, 0, 0, 0},
};

static struct vcc_desc cmi_lcd_bl_disable_cmds[] = {
    /* backlight disable */
    {DTYPE_VCC_DISABLE, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,  WAIT_TYPE_MS, 1, 0, 0, 0, 0},
};

/************************************************************
 *
 * LCD GPIO PINCTRL
 *
 ************************************************************/
static struct pinctrl_data pctrl;

static struct pinctrl_cmd_desc cmi_lcd_pinctrl_init_cmds[] = {
    {DTYPE_PINCTRL_GET,       &pctrl, 0},
    {DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
    {DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};
static struct pinctrl_cmd_desc cmi_lcd_pinctrl_normal_cmds[] = {
    {DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};
static struct pinctrl_cmd_desc cmi_lcd_pinctrl_lowpower_cmds[] = {
    {DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

#define GPIO_LCD_RESET_NAME      "lcd_reset_gpio"

static u32 gpio_lcd_reset;

static struct gpio_desc cmi_lcd_gpio_get_cmds[] = {
    /* reset */
    {DTYPE_GPIO_GET, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};
static struct gpio_desc cmi_lcd_gpio_request_cmds[] = {
    /* reset */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc cmi_lcd_gpio_free_cmds[] = {
    /* reset */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};
static struct gpio_desc cmi_lcd_gpio_normal_cmds[] = {
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 120,GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},

};

static struct gpio_desc cmi_lcd_gpio_lowpower_cmds[] = {
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static volatile bool g_display_on;
static struct balong_fb_panel_data cmi_panel_data;


/******************************************************************************/
static struct lcd_tuning_dev *p_tuning_dev = NULL;

//show lcd_info
static ssize_t cmi_lcd_info_show(struct device* dev,
                                 struct device_attribute* attr, char* buf)
{
    int ret = 0;
    struct balong_panel_info* pinfo = NULL;

    pinfo = cmi_panel_data.panel_info;
    snprintf(buf, PAGE_SIZE,"%s %d x %d\n",g_lcd_name_buf,pinfo->xres, pinfo->yres);
    ret = strlen(buf) + 1;

    return ret;
}

#define CMI_ESD_DEBUG	0
#if CMI_ESD_DEBUG
/*for esd test*/
static int esd_flag = 0;
static ssize_t cmi_show_esd_test(struct device* dev,
                                 struct device_attribute* attr, char* buf)
{
    return sprintf(buf, "%d\n", esd_flag);
}
static ssize_t cmi_store_esd_test(struct device* dev,
                                  struct device_attribute* attr, const char* buf, size_t count)
{
    int ret = 0;
    unsigned long val = 0;

    ret = strict_strtoul(buf, 0, &val);
    if (ret)
    { return ret; }

    esd_flag = val;
    return sprintf((char*)buf, "%d\n", esd_flag);
}
static DEVICE_ATTR(esd_test, 0644, cmi_show_esd_test, cmi_store_esd_test);
#endif

static DEVICE_ATTR(lcd_info, S_IRUGO, cmi_lcd_info_show, NULL);


static struct attribute* cmi_attrs[] =
{
    &dev_attr_lcd_info.attr,
#if CMI_ESD_DEBUG
    /*for esd test*/
    &dev_attr_esd_test.attr,
#endif
    NULL,
};

static struct attribute_group cmi_attr_group =
{
    .attrs = cmi_attrs,
};

static int cmi_sysfs_init(struct platform_device* pdev)
{
    int ret = 0;
    ret = sysfs_create_group(&pdev->dev.kobj, &cmi_attr_group);
    if (ret)
    {
        balongfb_loge("create sysfs file failed!\n");
        return ret;
    }
    return 0;
}

static void cmi_sysfs_deinit(struct platform_device* pdev)
{
    sysfs_remove_group(&pdev->dev.kobj, &cmi_attr_group);
}

static int mipi_cmi_panel_on(struct platform_device *pdev)
{
    struct balong_fb_data_type* balongfd = NULL;
    struct balong_panel_info* pinfo = NULL;
	struct balong_fb_panel_data *pdata = NULL;

#if defined (CONFIG_HUAWEI_DSM)
    u8* ade_base = NULL;
    u32 int_st0 = 0, int_st1 = 0;
#endif


    BUG_ON(pdev == NULL);

    pr_info("%s enter succ!\n", __func__);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

#if defined (CONFIG_HUAWEI_DSM)
    ade_base = balongfd->ade_base;
#endif

    pinfo = &(balongfd->panel_info);
    pr_info("%s,pinfo->lcd_init_step = %d!\n", __func__, pinfo->lcd_init_step);

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON)
	{
		/* lcd set voltage */
		vcc_cmds_tx(NULL, cmi_lcd_vcc_set_cmds, \
			ARRAY_SIZE(cmi_lcd_vcc_set_cmds));

		pinfo->lcd_init_step = LCD_INIT_SEND_SEQUENCE;
	}
	else if (pinfo->lcd_init_step == LCD_INIT_SEND_SEQUENCE)
	{
		if (!g_display_on) {
            /* lcd pinctrl normal */
            pinctrl_cmds_tx(pdev, cmi_lcd_pinctrl_normal_cmds, \
                ARRAY_SIZE(cmi_lcd_pinctrl_normal_cmds));

            /* lcd gpio request */
            gpio_cmds_tx(NULL, cmi_lcd_gpio_request_cmds, \
                ARRAY_SIZE(cmi_lcd_gpio_request_cmds));

            /* lcd vcc enable */
            vcc_cmds_tx(NULL, cmi_lcd_vcc_enable_cmds, \
                ARRAY_SIZE(cmi_lcd_vcc_enable_cmds));

            /* lcd gpio normal */
            gpio_cmds_tx(NULL, cmi_lcd_gpio_normal_cmds, \
                ARRAY_SIZE(cmi_lcd_gpio_normal_cmds));
            /* remove lcd backlight enable */

            /* lcd display on sequence */
            mipi_dsi_cmds_tx(cmi_display_on_cmds, \
                ARRAY_SIZE(cmi_display_on_cmds), balongfd->dsi_base);

			g_display_on = true;
			/*first set sbl to panel*/
			if(balongfd->panel_info.sbl_enable)
			{
				pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;
				if(pdata->set_sre != NULL)
				{
					pdata->set_sre(balongfd->pdev, balongfd->sbl_enable, balongfd->sbl_lsensor_value);
				}
			}
        }
    }
    else
    {
        balongfb_loge("failed to init lcd!\n");
    }

    pr_info("%s exit succ!\n", __func__);

#if defined (CONFIG_HUAWEI_DSM)
    int_st0 = inp32(ade_base +  MIPIDSI_INT_ST0_ADDR);
    int_st1 = inp32(ade_base +  MIPIDSI_INT_ST1_ADDR);
    if ((0x00 != int_st0 || 0x00 != int_st1) && (!dsm_client_ocuppy(lcd_dclient)))
    {
        dsm_client_record(lcd_dclient, "LCD dsi status. int_st0:0x%x,\
		int_st1:0x%x.\n", int_st0, int_st1);
        dsm_client_notify(lcd_dclient, DSM_LCD_STATUS_ERROR_NO);
    }
#endif

    return 0;
}

static int mipi_cmi_panel_off(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);
    pr_info("%s enter succ!\n", __func__);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

	if (g_display_on)
	{

        /* lcd display off sequence */
        mipi_dsi_cmds_tx(cmi_display_off_cmds, \
            ARRAY_SIZE(cmi_display_off_cmds), balongfd->dsi_base);

        /* shutdown d_phy */
        set_MIPIDSI_PHY_RSTZ(0);

        /* lcd gpio lowpower */
        gpio_cmds_tx(NULL, cmi_lcd_gpio_lowpower_cmds, \
            ARRAY_SIZE(cmi_lcd_gpio_lowpower_cmds));
        /* lcd gpio free */
        gpio_cmds_tx(NULL, cmi_lcd_gpio_free_cmds, \
            ARRAY_SIZE(cmi_lcd_gpio_free_cmds));

        /* lcd pinctrl lowpower */
        pinctrl_cmds_tx(pdev, cmi_lcd_pinctrl_lowpower_cmds, \
            ARRAY_SIZE(cmi_lcd_pinctrl_lowpower_cmds));

        /* lcd vcc disable */
        vcc_cmds_tx(NULL, cmi_lcd_vcc_disable_cmds, \
            ARRAY_SIZE(cmi_lcd_vcc_disable_cmds));
        /* remove lcd backlight disable */
    	g_display_on = false;

    }

    pr_info("%s exit succ!\n",__func__);
    return 0;
}

static int mipi_cmi_panel_remove(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    if (balongfd->panel_info.bl_set_type & BL_SET_BY_PWM)
    {
		PWM_CLK_PUT(&(balongfd->panel_info));
	}
	/* lcd vcc finit */
	vcc_cmds_tx(pdev, cmi_lcd_vcc_put_cmds, \
		ARRAY_SIZE(cmi_lcd_vcc_put_cmds));

    /* lcd pinctrl lowpower */
    pinctrl_cmds_tx(pdev, cmi_lcd_pinctrl_lowpower_cmds, \
        ARRAY_SIZE(cmi_lcd_pinctrl_lowpower_cmds));

	cmi_sysfs_deinit(pdev);

    return 0;
}
static int cmi_set_gamma(struct lcd_tuning_dev* ltd, enum lcd_gamma gamma)
{
    int ret = 0;
    struct platform_device* pdev = NULL;
    struct balong_fb_data_type* balongfd = NULL;
    u8* ade_base = 0;

    BUG_ON(ltd == NULL);
    pdev = (struct platform_device*)(ltd->data);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    ade_base = balongfd->ade_base;

    /* Fix me: Implement it */
    return ret;
}

static int cmi_set_cabc(struct platform_device *pdev, int cabc)
{
    int ret = 0;
    struct balong_fb_data_type* balongfd = NULL;  
    //set cabc mode according to application situation
	//dimming off in UI mode
	char  cmi_dimming_off_ui[] =    
	{
          0x53,
          0x24,
    };
	//dimming on in Video mode
	char  cmi_dimming_on_video[] = 
	{
          0x53,
          0x2C,
    };
    char  cmi_cabc_ui_on[] = {
          0x55,
          0x01 | g_IE_SRE_level,
    };
    char  cmi_cabc_vid_on[] = 
	{
          0x55,
          0x03 | g_IE_SRE_level,
    };
    char  cmi_cabc_off[] = 
	{
          0x55,
          0x00 | g_IE_SRE_level,
    };
    
	struct dsi_cmd_desc cmi_dimming_off_ui_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(cmi_dimming_off_ui), cmi_dimming_off_ui
        },
    };
	
	struct dsi_cmd_desc cmi_dimming_on_video_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(cmi_dimming_on_video), cmi_dimming_on_video
        },
    };
	
    struct dsi_cmd_desc cmi_cabc_ui_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(cmi_cabc_ui_on), cmi_cabc_ui_on
        },
    };
   
    struct dsi_cmd_desc cmi_cabc_vid_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(cmi_cabc_vid_on), cmi_cabc_vid_on
        },
    };
    
    struct dsi_cmd_desc cmi_cabc_off_cmds[] = 
    {
       {
    	 DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
    	 sizeof(cmi_cabc_off), cmi_cabc_off
       },
    };

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);
    pr_info("%s CABC_MODE =  %d\n", __func__, cabc);

    #if CMI_CABC_ENABLED
    switch (cabc)
    {
		case CABC_UI:                       // set CABC as UI Mode
		{
			balongfd->g_CABC_mode = 0x01;
			mipi_dsi_cmds_tx(cmi_cabc_ui_cmds, ARRAY_SIZE(cmi_cabc_ui_cmds), balongfd->dsi_base);
			mipi_dsi_cmds_tx(cmi_dimming_off_ui_cmds, ARRAY_SIZE(cmi_dimming_off_ui_cmds), balongfd->dsi_base);  //dimming off in UI mode
        }
        break;

        case CABC_VID:                      // set CABC as Video Mode
		{
			balongfd->g_CABC_mode = 0x03;
	        mipi_dsi_cmds_tx(cmi_cabc_vid_cmds, ARRAY_SIZE(cmi_cabc_vid_cmds), balongfd->dsi_base);
			mipi_dsi_cmds_tx(cmi_dimming_on_video_cmds, ARRAY_SIZE(cmi_dimming_on_video_cmds), balongfd->dsi_base); //dimming on in Video mode
        }
		break;

        case CABC_OFF:                      // close CABC
		{
			balongfd->g_CABC_mode = 0x00;
	        mipi_dsi_cmds_tx(cmi_cabc_off_cmds, ARRAY_SIZE(cmi_cabc_off_cmds), balongfd->dsi_base);
        }	
        break;
		
        default:
        	ret = -1;
        break;
    }
    #endif
    return ret;
}

static struct lcd_tuning_ops sp_tuning_ops =
{
    .set_gamma = cmi_set_gamma,
    .set_cabc = cmi_set_cabc,
};
static int mipi_cmi_panel_set_backlight(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;
    u32 level = 0;
    static u32 last_level = 255;

    /* Our eyes are more sensitive to small brightness.
       So we adjust the brightness of lcd following iphone4 */
    char bl_level_adjust[2] =
    {
        0x51,
        0x00,
    };

    struct dsi_cmd_desc cmi_bl_level_adjust[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 0, WAIT_TYPE_US,
            sizeof(bl_level_adjust), bl_level_adjust
        },
    };

    BUG_ON(pdev == NULL);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);


    level = balongfd->bl_level;

    if (level > 255)
    { level = 255; }

    //backlight may turn off when bl_level is below 4.
    if (level < 4 && level != 0)
    {
        level = 4;
    }

    bl_level_adjust[1] = level;

    mipi_dsi_cmds_tx(cmi_bl_level_adjust, ARRAY_SIZE(cmi_bl_level_adjust), balongfd->dsi_base);
    if(level == 0)
    {
        vcc_cmds_tx(NULL, cmi_lcd_bl_disable_cmds, \
            ARRAY_SIZE(cmi_lcd_bl_disable_cmds));
    }
    else if(last_level == 0 && level !=0)
    {
        vcc_cmds_tx(NULL, cmi_lcd_bl_enable_cmds, \
            ARRAY_SIZE(cmi_lcd_bl_enable_cmds));
    }
    last_level = level;
    //modified for beta test, it will be modified after beta test.
    balongfb_loge(" set backlight succ ,balongfd->bl_level = %d, level = %d \n",balongfd->bl_level,level);

    return 0;
}
#if LCD_CHECK_MIPI_SUPPORT
static int mipi_cmi_check_mipi_tr(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;
    u32 read_value = 0;
    int err = 0;
    BUG_ON(pdev == NULL);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);
    balongfd->lcd_readvalue = 0;
    //read 0x0a
    err = mipi_dsi_read_reg(balongfd, 0x0a, &read_value,DTYPE_GEN_READ1);
    if (err || read_value != 0x9c)
    {
        err = -1;
        goto check_mipi_tr_error;
    }
    balongfd->lcd_readvalue |= read_value << 24;
    //read 0x0b
    err = mipi_dsi_read_reg(balongfd, 0x0b, &read_value,DTYPE_GEN_READ1);
    if (err || read_value != 0x00)
    {
        err = -1;
        goto check_mipi_tr_error;
    }
    balongfd->lcd_readvalue |= read_value << 16;
    //read 0x0c
    err = mipi_dsi_read_reg(balongfd, 0x0c, &read_value,DTYPE_GEN_READ1);
    if (err || read_value != 0x70)
    {
        err = -1;
        goto check_mipi_tr_error;
    }
    balongfd->lcd_readvalue |= read_value << 8;
    //read 0x0d
    err = mipi_dsi_read_reg(balongfd, 0x0d, &read_value,DTYPE_GEN_READ1);
    if (err || read_value != 0x00)
    {
        err = -1;
        goto check_mipi_tr_error;
    }
    balongfd->lcd_readvalue |= read_value;
check_mipi_tr_error:
    return err;

}

static char Enter_NT_page0[] =
{
    0xF0,
    0x55, 0xAA, 0x52, 0x08, 0x00,
};

static char inversion_mode_1dot[] =
{
    0xBC,
    0x01, 0x01,
};

static char inversion_mode_column[] =
{
    0xBC,
    0x00, 0x00,
};

static struct dsi_cmd_desc  nt35521_lcd_inversion_type_1dot[] =
{
    {
        DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(Enter_NT_page0), Enter_NT_page0
    },
    {
        DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(inversion_mode_1dot), inversion_mode_1dot
    },
};

static struct dsi_cmd_desc  nt35521_lcd_inversion_type_column[] =
{
    {
        DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(Enter_NT_page0), Enter_NT_page0
    },
    {
        DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(inversion_mode_column), inversion_mode_column
    },
};

static int mipi_cmi_set_inversion_type(struct platform_device* pdev, unsigned int inversion_mode)
{
    int ret = 0;
    struct balong_fb_data_type* balongfd = NULL;
    BUG_ON(pdev == NULL);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);
    balongfd->lcd_inversion_mode = inversion_mode;
    switch (inversion_mode)
    {
        case INVERSION_TYPE_1DOT:
            mipi_dsi_cmds_tx(nt35521_lcd_inversion_type_1dot, \
                             ARRAY_SIZE(nt35521_lcd_inversion_type_1dot),balongfd->dsi_base);
            balongfb_loge("INVERSION_TYPE_1DOT\n");
            ret = 0;
            break;
        case INVERSION_TYPE_COLUMN:
            mipi_dsi_cmds_tx(nt35521_lcd_inversion_type_column, \
                             ARRAY_SIZE(nt35521_lcd_inversion_type_column),balongfd->dsi_base);
            balongfb_loge("INVERSION_TYPE_COLUMN\n");
            ret = 0;
            break;
        default:
            mipi_dsi_cmds_tx(nt35521_lcd_inversion_type_column, \
                             ARRAY_SIZE(nt35521_lcd_inversion_type_column),balongfd->dsi_base);
            balongfb_loge("default LCD inversion type, INVERSION_TYPE_COLUMN.\n");
            ret = -EINVAL;
            break;
    }
    return ret;
}
#endif
static int mipi_cmi_panel_set_fastboot(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    /* lcd gpio request */
    gpio_cmds_tx(NULL, cmi_lcd_gpio_request_cmds, \
        ARRAY_SIZE(cmi_lcd_gpio_request_cmds));

    /* lcd vcc enable */
    vcc_cmds_tx(NULL, cmi_lcd_vcc_enable_cmds, \
        ARRAY_SIZE(cmi_lcd_vcc_enable_cmds));

    /* lcd backlight enable */
    vcc_cmds_tx(NULL, cmi_lcd_bl_enable_cmds, \
        ARRAY_SIZE(cmi_lcd_bl_enable_cmds));
    g_display_on = true;

    return 0;
}
static int mipi_cmi_panel_set_sre(struct platform_device* pdev, int enable, int value)
{
    struct balong_fb_data_type* balongfd = NULL;

    /* As the reg 0x55 is used by CABC and SRE /IE, so use local data to enable each funtion seperately */
    char sbl_level_setting[2] =
    {
        0x55,
        0x00,
    };

    struct dsi_cmd_desc cmi_sbl_level_setting[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(sbl_level_setting), sbl_level_setting
        },
    };

    BUG_ON(pdev == NULL);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    //update flag depends on status get from lightsensor
	if(balongfd->sbl_enable == 1)	                          // Outdoor mode (check the SRE select box)
	{
		if( 0x50 != g_IE_SRE_level )          //  set reg 0x55 only when the value changed 
		{
        	g_IE_SRE_level = 0x50;            //SRE Medium LEVEL  0x5x,
			sbl_level_setting[1]  = balongfd->g_CABC_mode | g_IE_SRE_level;		   
		    mipi_dsi_cmds_tx(cmi_sbl_level_setting, ARRAY_SIZE(cmi_sbl_level_setting), balongfd->dsi_base); //send to IC reg
		}
	}
	else                 // 1.Indoor mode (check the SRE select box).  2.Uncheck the SRE select box.
    {
    	if( 0x50 == g_IE_SRE_level )         //  set reg 0x55 only when the value changed
		{
        	g_IE_SRE_level = 0x80;           //IE LOW LEVEL 0x8x,
			sbl_level_setting[1]  = balongfd->g_CABC_mode | g_IE_SRE_level;	   
		    mipi_dsi_cmds_tx(cmi_sbl_level_setting, ARRAY_SIZE(cmi_sbl_level_setting), balongfd->dsi_base);//send to IC reg
		}	
	}
	pr_info("%s enter succ!SRE sbl_enable = %d reg0x55_value = 0x%x\n",__func__, balongfd->sbl_enable, balongfd->g_CABC_mode | g_IE_SRE_level);
	return 0;
}

#if LCD_ESD_CHECK_SUPPORT
/*for esd check*/
static int mipi_cmi_panel_check_esd(struct platform_device* pdev)
{
    int ret = 0;
    int err = 0;
    unsigned int val = 0;
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);
    ret = mipi_dsi_read_reg(balongfd, 0x0a, &val,DTYPE_GEN_READ1);
    if ((ret == 0) && ((val&0xff) != 0x9c))
    {
        err = 1;
        balongfb_loge("check esd reg 0x0a is:0x%x.\n", val);
    }
#if CMI_ESD_DEBUG
    if (esd_flag != 0)
    {
        err = 1;
    }
#endif
    return err;
}
#endif //LCD_ESD_CHECK_SUPPORT

static struct balong_panel_info cmi_panel_info = {0};
static struct balong_fb_panel_data cmi_panel_data =
{
    .panel_info = &cmi_panel_info,
    .on = mipi_cmi_panel_on,
    .off = mipi_cmi_panel_off,
    .remove = mipi_cmi_panel_remove,
    .set_backlight = mipi_cmi_panel_set_backlight,
    .set_fastboot = mipi_cmi_panel_set_fastboot,
    .set_sre = mipi_cmi_panel_set_sre,
#if LCD_ESD_CHECK_SUPPORT
    /*for esd check*/
    .check_esd = mipi_cmi_panel_check_esd,
#endif //LCD_ESD_CHECK_SUPPORT

#if LCD_CHECK_MIPI_SUPPORT
    .check_mipi_tr = mipi_cmi_check_mipi_tr,
    .set_inversion_type = mipi_cmi_set_inversion_type,
#endif
    .lcd_cabc_mode_store  =  cmi_set_cabc,
    .lcd_gram_check_show  =  mipi_cmi_check_mipi_tr,
    .lcd_inversion_mode_store = mipi_cmi_set_inversion_type,
};

static int __init cmi_probe(struct platform_device* pdev)
{
    struct balong_panel_info* pinfo = NULL;
    struct platform_device* reg_pdev = NULL;
    struct lcd_properities lcd_props;
    /*add lcd info*/
    char lcd_name[] = "cmi_NT35521 5.5' VIDEO TFT";
    int len;
    pr_info("%s enter succ!!!!\n", __func__);
    /*add lcd info*/
    len = strlen(lcd_name);
    memset(g_lcd_name_buf, 0, sizeof(g_lcd_name_buf));
    strncpy(g_lcd_name_buf, lcd_name, (len > 100 ? 100 : len));
    g_display_on = false;
    pinfo = cmi_panel_data.panel_info;

    /* init lcd panel info */
    pinfo->xres = 720;       // HD
    pinfo->yres = 1280;
    pinfo->width  = 68;  //mm
    pinfo->height = 121; //mm
    pinfo->type = PANEL_MIPI_VIDEO;
    pinfo->orientation = LCD_PORTRAIT;
    pinfo->bpp = ADE_OUT_RGB_888;
    pinfo->s3d_frm = ADE_FRM_FMT_2D;
    pinfo->bgr_fmt = ADE_RGB;
    pinfo->bl_set_type = BL_SET_BY_MIPI;
    pinfo->bl_max = PWM_LEVEL;
    pinfo->bl_min = 1;

    pinfo->frc_enable = 0;    //close FRC
    //enable 1, disable 0
    pinfo->sbl_enable = 1;    //enable sbl

#if LCD_ESD_CHECK_SUPPORT
    /*for esd check enable 1,disable 0*/
    if (NULL != strstr(saved_command_line, "androidboot.swtype=factory"))
	{
		pinfo->esd_enable = 0;
	}
	else
	{
		pinfo->esd_enable = 1;
	}
#endif //LCD_ESD_CHECK_SUPPORT

    pinfo->sbl.bl_max = 0xff;
    pinfo->sbl.cal_a = 0x0f;
    pinfo->sbl.cal_b = 0xd8;
    pinfo->sbl.str_limit = 0x40;

    // porch
    pinfo->ldi.h_back_porch = 28;//63;
    pinfo->ldi.h_front_porch = 200;//164;//155
    pinfo->ldi.h_pulse_width = 2;//10;
    pinfo->ldi.v_back_porch = 25;
    pinfo->ldi.v_front_porch = 25;
    pinfo->ldi.v_pulse_width = 2;
    // video mode
    pinfo->ldi.hsync_plr = 1;
    pinfo->ldi.vsync_plr = 1;
    pinfo->ldi.pixelclk_plr = 0;
    pinfo->ldi.data_en_plr = 0;

    pinfo->ldi.disp_mode = LDI_DISP_MODE_NOT_3D_FBF;

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

    /* init at DTS */
    pinfo->mipi.lane_nums = DSI_4_LANES;
    pinfo->mipi.color_mode = DSI_24BITS_1;
    pinfo->mipi.vc = 0;
    pinfo->frame_rate = 60;
    pdev->id = 1;

	/* lcd resource */
	get_resource_from_dts(pdev, pinfo);

    /* lcd gpio init */
    gpio_cmds_tx(pdev, cmi_lcd_gpio_get_cmds, \
            ARRAY_SIZE(cmi_lcd_gpio_get_cmds));

	/* lcd vcc init */
	vcc_cmds_tx(pdev, cmi_lcd_vcc_get_cmds,
		ARRAY_SIZE(cmi_lcd_vcc_get_cmds));

	/* lcd pinctrl init */
	pinctrl_cmds_tx(pdev, cmi_lcd_pinctrl_init_cmds,
		ARRAY_SIZE(cmi_lcd_pinctrl_init_cmds));

    /* alloc panel device data */
    if (platform_device_add_data(pdev, &cmi_panel_data,
                                 sizeof(struct balong_fb_panel_data)))
    {
        balongfb_loge("platform_device_add_data failed!\n");
        platform_device_put(pdev);
        return -ENOMEM;
    }

    reg_pdev = balong_fb_add_device(pdev);
    if (NULL == reg_pdev)
    {
        balongfb_loge("add device failed!\n");
        return -1;
    }

    /* for cabc */
    lcd_props.type = TFT;
    lcd_props.default_gamma = GAMMA22;
    p_tuning_dev = lcd_tuning_dev_register(&lcd_props, &sp_tuning_ops, (void*)reg_pdev);
    if (IS_ERR(p_tuning_dev))
    {
        balongfb_loge("lcd_tuning_dev_register failed!\n");
        return -1;
    }
    cmi_sysfs_init(pdev);

    pr_info("%s exit succ!\n", __func__);
    return 0;
}
static const struct of_device_id mipi_cmi_nt35521_match_table[] =
{
    {
        .compatible = "hisilicon,mipi-cmi-nt35521",
        .data = NULL,
    },
    {},
};

static struct platform_driver this_driver =
{
    .probe = cmi_probe,
    .remove = NULL,
    .suspend = NULL,
    .resume = NULL,
    .shutdown = NULL,
    .driver = {
        .name = "mipi_cmi_NT35521",
        .of_match_table = mipi_cmi_nt35521_match_table,
    },
};

static int __init mipi_cmi_panel_init(void)
{
    int ret = 0;
#ifndef CONFIG_ARM64
    if ( get_lcd_type() != CMI_NT35521_LCD)
    {
        balongfb_loge("lcd type is not CMI_NT35521_LCD.\n");
        return ret;
    }
#endif
    ret = platform_driver_register(&this_driver);
    if (ret)
    {
        balongfb_loge("not able to register the driver\n");
        return ret;
    }

    return ret;
}

module_init(mipi_cmi_panel_init);
