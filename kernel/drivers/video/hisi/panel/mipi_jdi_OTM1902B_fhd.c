/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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
#include <huawei_platform/touthscreen/huawei_touchscreen.h>
#include <huawei_platform/log/log_jank.h>

#define PWM_LEVEL 100

static int cnt = 0;

static bool g_debug_enable = false;

/*******************************************************************************
 ** Power ON Sequence(sleep mode to Normal mode)
 */
static char addr_shift_00[] = {
    0x00,
    0x00,
};

static char cmd2_ena1[] = {
    0xff,
    0x19, 0x02, 0x01, 0x00,
};

static char addr_shift_80[] = {
    0x00,
    0x80,
};

static char cmd2_ena2[] = {
    0xff,
    0x19, 0x02,
};

/*
   static char sw_pin_lane_sel_00[] = {
   0x00,
   0x92,
   };
   static char sw_pin_3lane_sel_11[] = {
   0xff,
   0x00, 0x02,
   };
   */



static char addr_shift_83[] = {
    0x00,
    0x83,
};

static char adjustment_IC_1[] = {
    0xf3,
    0xca,
};

static char addr_shift_90[] = {
    0x00,
    0x90,
};

static char adjustment_IC_2[] = {
    0xc4,
    0x00,
};

static char addr_shift_b4[] = {
    0x00,
    0xb4,
};

static char adjustment_IC_3[] = {
    0xc0,
    0xc0,
};
static char addr_shift_87[] = {
    0x00,
    0x87,
};

static char pixel_eyes_set[] = {
    0xa4,
    0x15,
};

static char ca_set[] = {
    0x2a,
    0x00, 0x00, 0x04, 0x37,
};

static char pa_set[] = {
    0x2b,
    0x00, 0x00, 0x07, 0x7f,
};

static char cabc_mode_set[] = {
    0x55,
    0x00,
};
static char cabc_min_brightness[] = {
    0x5e,
    0x00,
};

static char bl_level_0[] = {
    0x51,
    0x00,
};

static char bl_level[] = {
    0x51,
    0x00,
};

static char te_enable[] = {
    0x35,
    0x00,
};

/*
   static char orise_shift_0x00[] = {
   0x00,
   0x00,
   };

   static char orise_sheift_0x80[] = {
   0x00,
   0x80,
   };
   */



/*Enable Orise Mode*/
/*
   static char enable_orise_command1[] = {
   0xFF,
   0x12, 0x82, 0x01,
   };

   static char enable_orise_command2[] = {
   0xFF,
   0x12, 0x82,
   };
   */

static char exit_sleep[] = {
    0x11,
};

#if 0
static char normal_display_on[] = {
    0x13,
};
#endif

static char all_pixels_off[] = {
    0x22,
};

static char display_on[] = {
    0x29,
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

#if PARTIAL_UPDATE
static char lcd_disp_x[] = {
    0x2A,
    0x00, 0x00, 0x02, 0xCF
};

static char lcd_disp_y[] = {
    0x2B,
    0x00, 0x00, 0x04, 0xFF
};
#endif

#if 0
static char get_chip_id[] = {
    0xDA,
};

/*enable orise mode*/
static char enable_orise_mode1[] = {
    0xFF,
    0x12, 0x82,0x01,
};

static char enable_orise_mode2[] = {
    0x00,
    0x80,
};

static char enable_orise_mode3[] = {
    0xFF,
    0x12, 0x82,
};

static char enable_orise_mode4[] = {
    0x00,
    0x80,
};

/*Disable per-charge*/
static char disable_per_charge [] = {
    0xA5,
    0x0C, 0x04, 0x01,
};


/* Set VGL*/
static char set_vgl1[] = {
    0x00,
    0xB0,
};

static char set_vgl2[] = {
    0xC5,
    0x92, 0xD6,0xAF,0xAF,0x82,0x88,0x44,0x44,0x40,0x88,
};
#endif
/* Delay TE*/
static char Delay_TE[] = {
    0x44,
    0x07, 0x7F,
};

#if 0
static char  P_DRV_M1[] = {
    0x00,
    0xB3,
};

static char P_DRV_M2[] = {
    0xC0,
    0x33,
};

static char bl_PWM_CTRL1[] = {
    0x00,
    0xB0,
};

static char bl_PWM_CTRL3[] = {
    0xCA,
    0xE3,0xE3,0x5F,0x50,
};
#endif
static char bl_enable[] = {
    0x53,
    0x24,
};


#if 0
static char orise_shift_0xb4[] = {
    0x00,
    0xb4,
};

static char display_address[] = {
    0xc0,
    0x10,
};
#endif
//#define RGV_INV_ENABLE

#ifdef RGV_INV_ENABLE

static char Gen_write_RGB_INV_ON1[] = {
    0xff,
    0x12,
    0x82,
    0x01,
};

static char Gen_write_RGB_INV_ON2[] = {
    0x00,
    0x80,
};

static char Gen_write_RGB_INV_ON3[] = {
    0xff,
    0x12,
    0x82,
};

static char RGB_INV0[] = {
    0x00,
    0xB3,
};

static char RGB_INV1[] = {
    0xC0,
    0x66,
};

static char Gen_write_RGB_INV_OFF1[] = {
    0x00,
    0x80,
};

static char Gen_write_RGB_INV_OFF2[] = {
    0xff,
    0x00,
    0x00,
};

static char Gen_write_RGB_INV_OFF3[] = {
    0x00,
    0x00,
};

static char Gen_write_RGB_INV_OFF4[] = {
    0xff,
    0x00,
    0x00,
    0x00,
};


#endif

/*----------------CABC Base Sequence---------------------*/
#if 0
static char enable_orise_mode5[] = {
    0x00,
    0x00,
};

static char CABC_UI_MODE[] = {
    0x55,
    0x91,
};

static char CABC_UI_curve[] = {
    0xc6,
    0x10,
};

static char CABC_STILL_MODE[] = {
    0x55,
    0x92,
};

static char CABC_STILL_curve[] = {
    0xc6,
    0x11,
};


static char CABC_VID_MODE[] = {
    0x55,
    0x93,
};

static char CABC_VID_curve[] = {
    0xc6,
    0x12,
};

static char CABC_VID_curve_setting[] = {
    0xc7,
    0xb0, 0xab, 0xaa, 0x99, 0x99,
    0x99, 0x89, 0x88, 0x88, 0x88,
    0x77, 0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x56,
};
/*----------------CE Sequence---------------------*/
static char CE_medium_on[] = {
    0x55,
    0x90,
};


static char enable_orise_mode8[] = {
    0x00,
    0xb0,
};

static char CE_param2[] = {
    0xd6,
    0x01,0x26,0x01,0x5a,0x01,
    0x80,0x01,0x5a,0x01,0x26,
    0x01,0x4d,
};

static char enable_orise_mode9[] = {
    0x00,
    0xc0,
};

static char CE_param3[] = {
    0xd6,
    0x55,0x11,0x00,0x22,0x11,
    0x3c,0x55,0x11,0x3c,0x1a,
    0x11,0x3c,
};

static char enable_orise_mode10[] = {
    0x00,
    0xd0,
};

static char CE_param4[] = {
    0xd6,
    0x55,0x11,0x3c,0x1a,0x11,
    0x33,
};

static char enable_orise_mode11[] = {
    0x00,
    0xe0,
};

static char CE_param5[] = {
    0xd6,
    0x2b,0x11,0x1a,0x11,0x11,
    0x1e,0x2b,0x11,0x1e,0x0d,
    0x11,0x1e,
};

static char enable_orise_mode12[] = {
    0x00,
    0xf0,
};

static char CE_param6[] = {
    0xd6,
    0x2b,0x11,0x1e,0x0d,0x11,
    0x1a,
};


static char enable_orise_mode14[] = {
    0x00, 0x00,
};

static char GVDD_setting[] = {
    0xd8,
    0x38, 0x38,
};

static char gamma22_Rp_setting[] = {
    0xe1,
    0x01,0x24,0x2d,0x3b,0x45,
    0x4c,0x5a,0x6c,0x78,0x88,
    0x93,0x9a,0x60,0x5b,0x56,
    0x4c,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};

static char gamma22_Rn_setting[] = {
    0xe2,
    0x01,0x24,0x2d,0x3b,0x45,
    0x4c,0x5a,0x6c,0x78,0x88,
    0x93,0x9a,0x60,0x5b,0x56,
    0x4c,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};

static char gamma22_Gp_setting[] = {
    0xe3,
    0x01,0x24,0x2d,0x3b,0x45,
    0x4c,0x5a,0x6c,0x78,0x88,
    0x93,0x9a,0x60,0x5b,0x56,
    0x4c,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};

static char gamma22_Gn_setting[] = {
    0xe4,
    0x01,0x24,0x2d,0x3b,0x45,
    0x4c,0x5a,0x6c,0x78,0x88,
    0x93,0x9a,0x60,0x5b,0x56,
    0x4c,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};

static char gamma22_Bp_setting[] = {
    0xe5,
    0x01,0x24,0x2d,0x3b,0x45,
    0x4c,0x5a,0x6c,0x78,0x88,
    0x93,0x9a,0x60,0x5b,0x56,
    0x4c,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};

static char gamma22_Bn_setting[] = {
    0xe6,
    0x01,0x24,0x2d,0x3b,0x45,
    0x4c,0x5a,0x6c,0x78,0x88,
    0x93,0x9a,0x60,0x5b,0x56,
    0x4c,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};
static char gamma23_Rp_setting[] = {
    0xe1,
    0x78,0x79,0x7b,0x7d,0x7f,
    0x81,0x86,0x8f,0x92,0x9a,
    0x9f,0xa3,0x5a,0x56,0x53,
    0x4a,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};

static char gamma23_Rn_setting[] = {
    0xe2,
    0x78,0x79,0x7b,0x7d,0x7f,
    0x81,0x86,0x8f,0x92,0x9a,
    0x9f,0xa3,0x5a,0x56,0x53,
    0x4a,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};

static char gamma23_Gp_setting[] = {
    0xe3,
    0x64,0x66,0x68,0x6a,0x6f,
    0x74,0x7a,0x85,0x8a,0x95,
    0x9c,0xa1,0x5c,0x59,0x56,
    0x4c,0x3c,0x33,0x2a,0x25,
    0x22,0x16,0x13,0x0F,
};

static char gamma23_Gn_setting[] = {
    0xe4,
    0x64,0x66,0x68,0x6a,0x6f,
    0x74,0x7a,0x85,0x8a,0x95,
    0x9c,0xa1,0x5c,0x59,0x56,
    0x4c,0x3c,0x33,0x2a,0x25,
    0x22,0x16,0x13,0x0F,
};

static char gamma23_Bp_setting[] = {
    0xe5,
    0x01,0x24,0x2d,0x3b,0x45,
    0x4c,0x5a,0x6c,0x78,0x88,
    0x93,0x9a,0x60,0x5b,0x56,
    0x4c,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};

static char gamma23_Bn_setting[] = {
    0xe6,
    0x01,0x24,0x2d,0x3b,0x45,
    0x4c,0x5a,0x6c,0x78,0x88,
    0x93,0x9a,0x60,0x5b,0x56,
    0x4c,0x3c,0x2f,0x25,0x22,
    0x19,0x16,0x13,0x0F,
};


static struct dsi_cmd_desc write_gamma23_cmds[] = {
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma23_Rp_setting), gamma23_Rp_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma23_Rn_setting), gamma23_Rn_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma23_Gp_setting), gamma23_Gp_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma23_Gn_setting), gamma23_Gn_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma23_Bp_setting), gamma23_Bp_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma23_Bn_setting), gamma23_Bn_setting},
};

static char enable_orise_mode16[] = {
    0x00,
    0x00,
};

static char enable_orise_mode17[] = {
    0x00,
    0x04,
};


static struct dsi_cmd_desc read_IC_version_cmds1[] = {
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode1), enable_orise_mode1},
    {DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode2), enable_orise_mode2},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode3), enable_orise_mode3},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode16), enable_orise_mode16},
};

static struct dsi_cmd_desc set_scan_mode[]= {
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(orise_shift_0xb4), orise_shift_0xb4},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(display_address), display_address},
};

static struct dsi_cmd_desc read_IC_version_cmds2[] = {
    {DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode17), enable_orise_mode17},
};
#endif
#if 0

static char ce_init_param1[] = {
    0xD4,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x41, 0x00, 0x40, 0x00, 0x42,
    0x00, 0x40, 0x00, 0x43, 0x00, 0x40, 0x00, 0x44, 0x00, 0x40,
    0x00, 0x45, 0x00, 0x40, 0x00, 0x45, 0x00, 0x40, 0x00, 0x46,
    0x00, 0x40, 0x00, 0x47, 0x00, 0x40, 0x00, 0x48, 0x00, 0x40,
    0x00, 0x49, 0x00, 0x40, 0x00, 0x4a, 0x00, 0x40, 0x00, 0x4b,
    0x00, 0x40, 0x00, 0x4b, 0x00, 0x40, 0x00, 0x4c, 0x00, 0x40,
    0x00, 0x4d, 0x00, 0x40, 0x00, 0x4e, 0x00, 0x40, 0x00, 0x4e,
    0x00, 0x40, 0x00, 0x4f, 0x00, 0x40, 0x00, 0x50, 0x00, 0x40,
    0x00, 0x50, 0x00, 0x40, 0x00, 0x51, 0x00, 0x40, 0x00, 0x52,
    0x00, 0x40, 0x00, 0x52, 0x00, 0x40, 0x00, 0x53, 0x00, 0x40,
    0x00, 0x54, 0x00, 0x40, 0x00, 0x54, 0x00, 0x40, 0x00, 0x55,
    0x00, 0x40, 0x00, 0x55, 0x00, 0x40, 0x00, 0x56, 0x00, 0x40,
};

static char ce_init_param2[] = {
    0xD4,
    0x00, 0x57, 0x00, 0x40, 0x00, 0x57, 0x00, 0x40, 0x00, 0x58,
    0x00, 0x40, 0x00, 0x59, 0x00, 0x40, 0x00, 0x59, 0x00, 0x40,
    0x00, 0x5a, 0x00, 0x40, 0x00, 0x5b, 0x00, 0x40, 0x00, 0x5b,
    0x00, 0x40, 0x00, 0x5c, 0x00, 0x40, 0x00, 0x5c, 0x00, 0x40,
    0x00, 0x5d, 0x00, 0x40, 0x00, 0x5e, 0x00, 0x40, 0x00, 0x5e,
    0x00, 0x40, 0x00, 0x5f, 0x00, 0x40, 0x00, 0x60, 0x00, 0x40,
    0x00, 0x60, 0x00, 0x40, 0x00, 0x5f, 0x00, 0x40, 0x00, 0x5f,
    0x00, 0x40, 0x00, 0x5e, 0x00, 0x40, 0x00, 0x5d, 0x00, 0x40,
    0x00, 0x5d, 0x00, 0x40, 0x00, 0x5c, 0x00, 0x40, 0x00, 0x5c,
    0x00, 0x40, 0x00, 0x5b, 0x00, 0x40, 0x00, 0x5a, 0x00, 0x40,
    0x00, 0x5a, 0x00, 0x40, 0x00, 0x59, 0x00, 0x40, 0x00, 0x58,
    0x00, 0x40, 0x00, 0x58, 0x00, 0x40, 0x00, 0x57, 0x00, 0x40,
    0x00, 0x56, 0x00, 0x40, 0x00, 0x56, 0x00, 0x40, 0x00, 0x55,
    0x00, 0x40, 0x00, 0x54, 0x00, 0x40, 0x00, 0x53, 0x00, 0x40,
    0x00, 0x52, 0x00, 0x40, 0x00, 0x51, 0x00, 0x40, 0x00, 0x50,
    0x00, 0x40, 0x00, 0x4f, 0x00, 0x40, 0x00, 0x4f, 0x00, 0x40,
    0x00, 0x4e, 0x00, 0x40, 0x00, 0x4d, 0x00, 0x40, 0x00, 0x4c,
    0x00, 0x40, 0x00, 0x4b, 0x00, 0x40, 0x00, 0x4a, 0x00, 0x40,
};

static char ce_init_param3[] = {
    0xD4,
    0x00, 0x4a, 0x00, 0x40, 0x00, 0x4b, 0x00, 0x40, 0x00, 0x4c,
    0x00, 0x40, 0x00, 0x4c, 0x00, 0x40, 0x00, 0x4d, 0x00, 0x40,
    0x00, 0x4e, 0x00, 0x40, 0x00, 0x4f, 0x00, 0x40, 0x00, 0x50,
    0x00, 0x40, 0x00, 0x51, 0x00, 0x40, 0x00, 0x52, 0x00, 0x40,
    0x00, 0x53, 0x00, 0x40, 0x00, 0x53, 0x00, 0x40, 0x00, 0x54,
    0x00, 0x40, 0x00, 0x55, 0x00, 0x40, 0x00, 0x56, 0x00, 0x40,
    0x00, 0x57, 0x00, 0x40, 0x00, 0x57, 0x00, 0x40, 0x00, 0x58,
    0x00, 0x40, 0x00, 0x59, 0x00, 0x40, 0x00, 0x59, 0x00, 0x40,
    0x00, 0x5a, 0x00, 0x40, 0x00, 0x5b, 0x00, 0x40, 0x00, 0x5b,
    0x00, 0x40, 0x00, 0x5c, 0x00, 0x40, 0x00, 0x5c, 0x00, 0x40,
    0x00, 0x5d, 0x00, 0x40, 0x00, 0x5e, 0x00, 0x40, 0x00, 0x5e,
    0x00, 0x40, 0x00, 0x5f, 0x00, 0x40, 0x00, 0x60, 0x00, 0x40,
    0x00, 0x60, 0x00, 0x40, 0x00, 0x5f, 0x00, 0x40, 0x00, 0x5f,
    0x00, 0x40, 0x00, 0x5e, 0x00, 0x40, 0x00, 0x5d, 0x00, 0x40,
    0x00, 0x5d, 0x00, 0x40, 0x00, 0x5c, 0x00, 0x40, 0x00, 0x5c,
    0x00, 0x40, 0x00, 0x5b, 0x00, 0x40, 0x00, 0x5a, 0x00, 0x40,
    0x00, 0x5a, 0x00, 0x40, 0x00, 0x59, 0x00, 0x40, 0x00, 0x58,
    0x00, 0x40, 0x00, 0x58, 0x00, 0x40, 0x00, 0x57, 0x00, 0x40,
};

static char ce_init_param4[] = {
    0xD4,
    0x00, 0x56, 0x00, 0x40, 0x00, 0x56, 0x00, 0x40, 0x00, 0x55,
    0x00, 0x40, 0x00, 0x54, 0x00, 0x40, 0x00, 0x53, 0x00, 0x40,
    0x00, 0x52, 0x00, 0x40, 0x00, 0x51, 0x00, 0x40, 0x00, 0x50,
    0x00, 0x40, 0x00, 0x4f, 0x00, 0x40, 0x00, 0x4f, 0x00, 0x40,
    0x00, 0x4e, 0x00, 0x40, 0x00, 0x4d, 0x00, 0x40, 0x00, 0x4c,
    0x00, 0x40, 0x00, 0x4b, 0x00, 0x40, 0x00, 0x4a, 0x00, 0x40,
    0x00, 0x4a, 0x00, 0x40, 0x00, 0x4a, 0x00, 0x40, 0x00, 0x4b,
    0x00, 0x40, 0x00, 0x4c, 0x00, 0x40, 0x00, 0x4c, 0x00, 0x40,
    0x00, 0x4d, 0x00, 0x40, 0x00, 0x4e, 0x00, 0x40, 0x00, 0x4e,
    0x00, 0x40, 0x00, 0x4f, 0x00, 0x40, 0x00, 0x50, 0x00, 0x40,
    0x00, 0x50, 0x00, 0x40, 0x00, 0x51, 0x00, 0x40, 0x00, 0x52,
    0x00, 0x40, 0x00, 0x52, 0x00, 0x40, 0x00, 0x53, 0x00, 0x40,
    0x00, 0x53, 0x00, 0x40, 0x00, 0x52, 0x00, 0x40, 0x00, 0x51,
    0x00, 0x40, 0x00, 0x4f, 0x00, 0x40, 0x00, 0x4e, 0x00, 0x40,
    0x00, 0x4d, 0x00, 0x40, 0x00, 0x4b, 0x00, 0x40, 0x00, 0x4a,
    0x00, 0x40, 0x00, 0x49, 0x00, 0x40, 0x00, 0x47, 0x00, 0x40,
    0x00, 0x46, 0x00, 0x40, 0x00, 0x45, 0x00, 0x40, 0x00, 0x44,
    0x00, 0x40, 0x00, 0x42, 0x00, 0x40, 0x00, 0x41, 0x00, 0x40,
};

static char ce_init_param5[] = {
    0xD5,
    0x00, 0x55, 0x00, 0x4b, 0x00, 0x54, 0x00, 0x4b, 0x00, 0x52,
    0x00, 0x4a, 0x00, 0x51, 0x00, 0x4a, 0x00, 0x4f, 0x00, 0x4a,
    0x00, 0x4e, 0x00, 0x49, 0x00, 0x4c, 0x00, 0x49, 0x00, 0x4b,
    0x00, 0x49, 0x00, 0x4a, 0x00, 0x49, 0x00, 0x48, 0x00, 0x48,
    0x00, 0x47, 0x00, 0x48, 0x00, 0x45, 0x00, 0x48, 0x00, 0x44,
    0x00, 0x47, 0x00, 0x43, 0x00, 0x47, 0x00, 0x41, 0x00, 0x47,
    0x00, 0x40, 0x00, 0x47, 0x00, 0x41, 0x00, 0x46, 0x00, 0x41,
    0x00, 0x46, 0x00, 0x42, 0x00, 0x46, 0x00, 0x43, 0x00, 0x46,
    0x00, 0x43, 0x00, 0x46, 0x00, 0x44, 0x00, 0x46, 0x00, 0x44,
    0x00, 0x46, 0x00, 0x45, 0x00, 0x45, 0x00, 0x45, 0x00, 0x45,
    0x00, 0x46, 0x00, 0x45, 0x00, 0x47, 0x00, 0x45, 0x00, 0x47,
    0x00, 0x45, 0x00, 0x48, 0x00, 0x45, 0x00, 0x48, 0x00, 0x44,
    0x00, 0x49, 0x00, 0x44, 0x00, 0x49, 0x00, 0x45, 0x00, 0x4a,
    0x00, 0x45, 0x00, 0x4a, 0x00, 0x45, 0x00, 0x4a, 0x00, 0x45,
    0x00, 0x4b, 0x00, 0x46, 0x00, 0x4b, 0x00, 0x46, 0x00, 0x4c,
    0x00, 0x46, 0x00, 0x4c, 0x00, 0x46, 0x00, 0x4d, 0x00, 0x46,
    0x00, 0x4d, 0x00, 0x47, 0x00, 0x4e, 0x00, 0x47, 0x00, 0x4e,
    0x00, 0x47, 0x00, 0x4e, 0x00, 0x47, 0x00, 0x4f, 0x00, 0x47,
};

static char ce_init_param6[] = {
    0xD5,
    0x00, 0x4f, 0x00, 0x48, 0x00, 0x50, 0x00, 0x48, 0x00, 0x50,
    0x00, 0x48, 0x00, 0x50, 0x00, 0x48, 0x00, 0x51, 0x00, 0x49,
    0x00, 0x51, 0x00, 0x49, 0x00, 0x52, 0x00, 0x49, 0x00, 0x52,
    0x00, 0x49, 0x00, 0x53, 0x00, 0x49, 0x00, 0x53, 0x00, 0x4a,
    0x00, 0x53, 0x00, 0x4a, 0x00, 0x54, 0x00, 0x4a, 0x00, 0x54,
    0x00, 0x4a, 0x00, 0x55, 0x00, 0x4a, 0x00, 0x55, 0x00, 0x4b,
    0x00, 0x55, 0x00, 0x4b, 0x00, 0x55, 0x00, 0x4b, 0x00, 0x54,
    0x00, 0x4a, 0x00, 0x54, 0x00, 0x4a, 0x00, 0x54, 0x00, 0x4a,
    0x00, 0x53, 0x00, 0x4a, 0x00, 0x53, 0x00, 0x4a, 0x00, 0x52,
    0x00, 0x49, 0x00, 0x52, 0x00, 0x49, 0x00, 0x52, 0x00, 0x49,
    0x00, 0x51, 0x00, 0x49, 0x00, 0x51, 0x00, 0x48, 0x00, 0x50,
    0x00, 0x48, 0x00, 0x50, 0x00, 0x48, 0x00, 0x4f, 0x00, 0x48,
    0x00, 0x4f, 0x00, 0x48, 0x00, 0x4e, 0x00, 0x47, 0x00, 0x4e,
    0x00, 0x47, 0x00, 0x4d, 0x00, 0x47, 0x00, 0x4d, 0x00, 0x46,
    0x00, 0x4c, 0x00, 0x46, 0x00, 0x4c, 0x00, 0x46, 0x00, 0x4b,
    0x00, 0x46, 0x00, 0x4a, 0x00, 0x45, 0x00, 0x4a, 0x00, 0x45,
    0x00, 0x49, 0x00, 0x45, 0x00, 0x49, 0x00, 0x44, 0x00, 0x48,
    0x00, 0x44, 0x00, 0x48, 0x00, 0x44, 0x00, 0x47, 0x00, 0x44,
};

static char ce_init_param7[] = {
    0xD5,
    0x00, 0x47, 0x00, 0x43, 0x00, 0x47, 0x00, 0x44, 0x00, 0x48,
    0x00, 0x44, 0x00, 0x48, 0x00, 0x44, 0x00, 0x49, 0x00, 0x45,
    0x00, 0x4a, 0x00, 0x45, 0x00, 0x4a, 0x00, 0x45, 0x00, 0x4b,
    0x00, 0x45, 0x00, 0x4b, 0x00, 0x46, 0x00, 0x4c, 0x00, 0x46,
    0x00, 0x4c, 0x00, 0x46, 0x00, 0x4d, 0x00, 0x47, 0x00, 0x4e,
    0x00, 0x47, 0x00, 0x4e, 0x00, 0x47, 0x00, 0x4f, 0x00, 0x47,
    0x00, 0x4f, 0x00, 0x48, 0x00, 0x50, 0x00, 0x48, 0x00, 0x50,
    0x00, 0x48, 0x00, 0x50, 0x00, 0x48, 0x00, 0x51, 0x00, 0x49,
    0x00, 0x51, 0x00, 0x49, 0x00, 0x52, 0x00, 0x49, 0x00, 0x52,
    0x00, 0x49, 0x00, 0x53, 0x00, 0x49, 0x00, 0x53, 0x00, 0x4a,
    0x00, 0x53, 0x00, 0x4a, 0x00, 0x54, 0x00, 0x4a, 0x00, 0x54,
    0x00, 0x4a, 0x00, 0x55, 0x00, 0x4a, 0x00, 0x55, 0x00, 0x4b,
    0x00, 0x55, 0x00, 0x4b, 0x00, 0x55, 0x00, 0x4b, 0x00, 0x54,
    0x00, 0x4a, 0x00, 0x54, 0x00, 0x4a, 0x00, 0x54, 0x00, 0x4a,
    0x00, 0x53, 0x00, 0x4a, 0x00, 0x53, 0x00, 0x4a, 0x00, 0x52,
    0x00, 0x49, 0x00, 0x52, 0x00, 0x49, 0x00, 0x52, 0x00, 0x49,
    0x00, 0x51, 0x00, 0x49, 0x00, 0x51, 0x00, 0x48, 0x00, 0x50,
    0x00, 0x48, 0x00, 0x50, 0x00, 0x48, 0x00, 0x4f, 0x00, 0x48,
};

static char ce_init_param8[] = {
    0xD5,
    0x00, 0x4f, 0x00, 0x48, 0x00, 0x4e, 0x00, 0x47, 0x00, 0x4e,
    0x00, 0x47, 0x00, 0x4d, 0x00, 0x47, 0x00, 0x4d, 0x00, 0x46,
    0x00, 0x4c, 0x00, 0x46, 0x00, 0x4c, 0x00, 0x46, 0x00, 0x4b,
    0x00, 0x46, 0x00, 0x4a, 0x00, 0x45, 0x00, 0x4a, 0x00, 0x45,
    0x00, 0x49, 0x00, 0x45, 0x00, 0x49, 0x00, 0x44, 0x00, 0x48,
    0x00, 0x44, 0x00, 0x48, 0x00, 0x44, 0x00, 0x47, 0x00, 0x44,
    0x00, 0x47, 0x00, 0x43, 0x00, 0x47, 0x00, 0x44, 0x00, 0x48,
    0x00, 0x44, 0x00, 0x48, 0x00, 0x44, 0x00, 0x48, 0x00, 0x44,
    0x00, 0x49, 0x00, 0x45, 0x00, 0x49, 0x00, 0x45, 0x00, 0x4a,
    0x00, 0x45, 0x00, 0x4a, 0x00, 0x45, 0x00, 0x4a, 0x00, 0x45,
    0x00, 0x4b, 0x00, 0x46, 0x00, 0x4b, 0x00, 0x46, 0x00, 0x4c,
    0x00, 0x46, 0x00, 0x4c, 0x00, 0x46, 0x00, 0x4d, 0x00, 0x46,
    0x00, 0x4d, 0x00, 0x47, 0x00, 0x4e, 0x00, 0x47, 0x00, 0x4e,
    0x00, 0x47, 0x00, 0x4f, 0x00, 0x48, 0x00, 0x4f, 0x00, 0x48,
    0x00, 0x50, 0x00, 0x48, 0x00, 0x50, 0x00, 0x48, 0x00, 0x51,
    0x00, 0x49, 0x00, 0x52, 0x00, 0x49, 0x00, 0x52, 0x00, 0x49,
    0x00, 0x53, 0x00, 0x4a, 0x00, 0x53, 0x00, 0x4a, 0x00, 0x54,
    0x00, 0x4a, 0x00, 0x54, 0x00, 0x4a, 0x00, 0x55, 0x00, 0x4b,
};
#endif
#if 0
static struct dsi_cmd_desc jdi_backlight_cmds[] = {
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(bl_level), bl_level},
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(bl_enable), bl_enable},
};

static struct dsi_cmd_desc jdi_video_on_v3_cmds[] = {
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(bl_level), bl_level},

    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(bl_enable_noDimming), bl_enable_noDimming},

    {DTYPE_DCS_WRITE, 0, 100, WAIT_TYPE_US,
        sizeof(all_pixels_off), all_pixels_off},

    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(te_enable), te_enable},

    {DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
        sizeof(normal_display_on), normal_display_on},

    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode1), enable_orise_mode1},
    {DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode2), enable_orise_mode2},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode3), enable_orise_mode3},

    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(ce_init_param1), ce_init_param1},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(ce_init_param2), ce_init_param2},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(ce_init_param3), ce_init_param3},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(ce_init_param4), ce_init_param4},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(ce_init_param5), ce_init_param5},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(ce_init_param6), ce_init_param6},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(ce_init_param7), ce_init_param7},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(ce_init_param8), ce_init_param8},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(CE_medium_on), CE_medium_on},

    {DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode4), enable_orise_mode4},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(disable_per_charge), disable_per_charge},
    {DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
        sizeof(set_vgl1), set_vgl1},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(set_vgl2), set_vgl2},

    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(P_DRV_M1), P_DRV_M1},

    {DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(P_DRV_M2), P_DRV_M2},
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(bl_PWM_CTRL1), bl_PWM_CTRL1},
    {DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(bl_PWM_CTRL3), bl_PWM_CTRL3},

    {DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
        sizeof(exit_sleep), exit_sleep},

    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(GVDD_setting), GVDD_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma22_Rp_setting), gamma22_Rp_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma22_Rn_setting), gamma22_Rn_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma22_Gp_setting), gamma22_Gp_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma22_Gn_setting), gamma22_Gn_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma22_Bp_setting), gamma22_Bp_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(enable_orise_mode14), enable_orise_mode14},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(gamma22_Bn_setting), gamma22_Bn_setting},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(Delay_TE), Delay_TE},

    {DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
        sizeof(display_on), display_on},
};
#endif

static struct dsi_cmd_desc jdi_display_on_cmds[] = {
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(bl_level), bl_level},
    {DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_ena1), cmd2_ena1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(addr_shift_80), addr_shift_80},
    {DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_ena2), cmd2_ena2},
    //{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
    //sizeof(sw_pin_lane_sel_00), sw_pin_lane_sel_00},
    //{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
    //sizeof(sw_pin_3lane_sel_11), sw_pin_3lane_sel_11},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(addr_shift_83), addr_shift_83},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(adjustment_IC_1), adjustment_IC_1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(addr_shift_90), addr_shift_90},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(adjustment_IC_2), adjustment_IC_2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(addr_shift_b4), addr_shift_b4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(adjustment_IC_3), adjustment_IC_3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(addr_shift_87), addr_shift_87},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(pixel_eyes_set), pixel_eyes_set},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(addr_shift_00), addr_shift_00},


    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(te_enable), te_enable},
    {DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
        sizeof(ca_set), ca_set},
    {DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
        sizeof(pa_set), pa_set},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cabc_mode_set), cabc_mode_set},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cabc_min_brightness), cabc_min_brightness},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(Delay_TE), Delay_TE},
    {DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
        sizeof(exit_sleep), exit_sleep},
    {DTYPE_DCS_WRITE, 0, 100, WAIT_TYPE_MS,
        sizeof(display_on), display_on},
};


/*
   {

   {DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
   sizeof(bl_level), bl_level},
   {DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
   sizeof(bl_enable), bl_enable},
   {DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
   sizeof(te_enable), te_enable},



   {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
   sizeof(enable_orise_command1), enable_orise_command1},
   {DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
   sizeof(orise_sheift_0x80), orise_sheift_0x80},
   {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
   sizeof(enable_orise_command2), enable_orise_command2},

//{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
//sizeof(addr_shift_00), addr_shift_00},
//{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
//sizeof(cmd2_ena1), cmd2_ena1},
//{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
//sizeof(addr_shift_80), addr_shift_80},
//{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
//sizeof(cmd2_ena2), cmd2_ena2},
{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
sizeof(sw_pin_lane_sel_00), sw_pin_lane_sel_00},
{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
sizeof(sw_pin_3lane_sel_11), sw_pin_3lane_sel_11},

{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
sizeof(Delay_TE), Delay_TE},
{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_US,
sizeof(exit_sleep), exit_sleep},
{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
sizeof(display_on), display_on},

};
*/


static struct dsi_cmd_desc jdi_display_off_cmds[] = {
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(bl_level_0), bl_level_0},
    {DTYPE_DCS_WRITE, 0, 100, WAIT_TYPE_US,
        sizeof(all_pixels_off), all_pixels_off},
    {DTYPE_DCS_WRITE, 0, 70, WAIT_TYPE_MS,
        sizeof(display_off), display_off},
    {DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
        sizeof(enter_sleep), enter_sleep},
};

#if PARTIAL_UPDATE
static struct dsi_cmd_desc set_display_address[] = {
    {DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US,
        sizeof(lcd_disp_x), lcd_disp_x},
    {DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US,
        sizeof(lcd_disp_y), lcd_disp_y},
};
#endif

#if 0
static struct dsi_cmd_desc jdi_get_chip_id_cmds[] = {
    {DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
        sizeof(get_chip_id), get_chip_id}
};

static struct dsi_cmd_desc jdi_cabc_cmds[] = {
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_enable_setting), CABC_enable_setting},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode6), enable_orise_mode6},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(CABC_func_setting), CABC_func_setting},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_UI_curve), CABC_UI_curve},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(CABC_UI_curve_setting), CABC_UI_curve_setting},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_STILL_curve), CABC_STILL_curve},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(CABC_STILL_curve_setting), CABC_STILL_curve_setting},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_VID_curve), CABC_VID_curve},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(CABC_VID_curve_setting), CABC_VID_curve_setting},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_disable_curve), CABC_disable_curve},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_disable_setting), CABC_disable_setting},
};


static struct dsi_cmd_desc jdi_cabc_ui_on_cmds[] = {
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_UI_MODE), CABC_UI_MODE},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_UI_curve), CABC_UI_curve},
};

static struct dsi_cmd_desc jdi_cabc_still_on_cmds[] = {
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_STILL_MODE), CABC_STILL_MODE},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_STILL_curve), CABC_STILL_curve},
};

static struct dsi_cmd_desc jdi_cabc_vid_on_cmds[] = {
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_VID_MODE), CABC_VID_MODE},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(enable_orise_mode5), enable_orise_mode5},
    {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
        sizeof(CABC_VID_curve), CABC_VID_curve},
};
#endif
/*
   static struct dsi_cmd_desc jdi_ce_cmds[] = {
   {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
   sizeof(enable_orise_mode7), enable_orise_mode7},
   {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
   sizeof(CE_param1), CE_param1},
   {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
   sizeof(enable_orise_mode8), enable_orise_mode8},
   {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
   sizeof(CE_param2), CE_param2},
   {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
   sizeof(enable_orise_mode9), enable_orise_mode9},
   {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
   sizeof(CE_param3), CE_param3},
   {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
   sizeof(enable_orise_mode10), enable_orise_mode10},
   {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
   sizeof(CE_param4), CE_param4},
   {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
   sizeof(enable_orise_mode11), enable_orise_mode11},
   {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
   sizeof(CE_param5), CE_param5},
   {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
   sizeof(enable_orise_mode12), enable_orise_mode12},
   {DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
   sizeof(CE_param6), CE_param6},
   {DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
   sizeof(enable_orise_mode5), enable_orise_mode5},
   };

   static struct dsi_cmd_desc jdi_ce_on_cmds[] = {
   {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
   sizeof(CE_medium_on), CE_medium_on},
   };
   */

/************************************************************
 *
 * LCD VCC
 *
 ************************************************************/

#define VCC_BACKLIGHT_NAME      "lcd_backlight"
#define VCC_LCDBIAS_NAME        "vcc_lcdbias"
#define VCC_LCD_VSN_NAME        "lcd_vsn"
#define VCC_LCD_VSP_NAME        "lcd_vsp"
#define VCC_TP_ANALOG_NAME      "tp_analog"

/* scharg regulator */
static struct regulator *lcd_bl_vcc;
static struct regulator *lcd_bias_vcc;
static struct regulator *lcd_vsn_vcc;
static struct regulator *lcd_vsp_vcc;
static struct regulator *tp_analog_vcc;

static struct vcc_desc jdi_lcd_vcc_get_cmds[] = {
    /* vcc get */
    {DTYPE_VCC_GET, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, 0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  0, 0, 0, 0, 0, 0},

    {DTYPE_VCC_GET, VCC_TP_ANALOG_NAME, &tp_analog_vcc,0, 0, 0, 0, 0, 0},
};

static struct vcc_desc jdi_lcd_vcc_set_cmds[] = {
    /* vcc set voltage */
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCDBIAS_NAME, &lcd_bias_vcc,  0, 0, 5400000, 5400000, 0, 0},
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCD_VSP_NAME, &lcd_vsp_vcc,   0, 0, 5400000, 5400000, 0, 0},
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCD_VSN_NAME, &lcd_vsn_vcc,   0, 0, 5400000, 5400000, 0, 0},
};

static struct vcc_desc jdi_lcd_vcc_put_cmds[] = {
    /* vcc put */
    {DTYPE_VCC_PUT, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, 0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  0, 0, 0, 0, 0, 0},
};

static struct vcc_desc jdi_lcd_vcc_enable_cmds[] = {
    /* vcc enable */
    {DTYPE_VCC_ENABLE, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, WAIT_TYPE_MS, 12, 0, 0, 0, 0},
    {DTYPE_VCC_ENABLE, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  WAIT_TYPE_MS, 10, 0, 0, 0, 0},
    {DTYPE_VCC_ENABLE, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  WAIT_TYPE_MS, 50, 0, 0, 0, 0},
};

/* tp vcc enable for fastboot continue display */
static struct vcc_desc jdi_tp_vcc_enable_cmds[] = {
    {DTYPE_VCC_ENABLE, VCC_TP_ANALOG_NAME, &tp_analog_vcc, WAIT_TYPE_MS, 10, 0, 0, 0, 0},
};

static struct vcc_desc jdi_lcd_vcc_disable_cmds[] = {
    /* vcc disable */
    {DTYPE_VCC_DISABLE, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  WAIT_TYPE_MS, 5, 0, 0, 0, 0},
    {DTYPE_VCC_DISABLE, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  WAIT_TYPE_MS, 5, 0, 0, 0, 0},
    {DTYPE_VCC_DISABLE, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, WAIT_TYPE_MS, 1, 0, 0, 0, 0},
};

static struct vcc_desc jdi_lcd_bl_enable_cmds[] = {
    /* backlight enable */
    {DTYPE_VCC_ENABLE, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   WAIT_TYPE_MS, 10, 0, 0, 0, 0},
};

static struct vcc_desc jdi_lcd_bl_disable_cmds[] = {
    /* backlight disable */
    {DTYPE_VCC_DISABLE, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,  WAIT_TYPE_MS, 1, 0, 0, 0, 0},
};

/************************************************************
 *
 * LCD GPIO PINCTRL
 *
 ************************************************************/
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

#define GPIO_LCD_RESET_NAME     "lcd_reset_gpio"
static u32 gpio_lcd_reset;

static struct gpio_desc jdi_lcd_gpio_get_cmds[] = {
    /* reset */
    {DTYPE_GPIO_GET, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc jdi_lcd_gpio_request_cmds[] = {
    /* reset */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc jdi_lcd_gpio_free_cmds[] = {
    /* reset */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc jdi_lcd_gpio_normal_cmds[] = {
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 120,GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
};

static struct gpio_desc jdi_lcd_gpio_lowpower_cmds[] = {
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static volatile bool g_display_on;
static struct balong_fb_panel_data jdi_panel_data;


/******************************************************************************/
static struct lcd_tuning_dev *p_tuning_dev = NULL;
static int cabc_mode = 1; /* allow application to set cabc mode to ui mode */


/*
   static int jdi_set_gamma(struct lcd_tuning_dev *ltd, enum lcd_gamma gamma)
   {
   int ret = 0;
   struct platform_device *pdev = NULL;
   struct balong_fb_data_type *balongfd = NULL;
//u32 ade_base = 0;

BUG_ON(ltd == NULL);
pdev = (struct platform_device *)(ltd->data);
balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
BUG_ON(balongfd == NULL);

//ade_base = balongfd->ade_base;



return ret;
}
*/

static int jdi_set_cabc(struct lcd_tuning_dev *ltd, enum  tft_cabc cabc)
{
    int ret = 0;
    struct platform_device *pdev = NULL;
    struct balong_fb_data_type *balongfd = NULL;
    u8* ade_base = 0;

    BUG_ON(ltd == NULL);
    pdev = (struct platform_device *)(ltd->data);
    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    ade_base = balongfd->ade_base;



    return ret;
}

/*
   static struct lcd_tuning_ops jdi_sp_tuning_ops = {
   .set_gamma = jdi_set_gamma,
   .set_cabc = jdi_set_cabc,
   };
   */

static ssize_t jdi_lcd_info_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    int ret = 0;
    struct balong_panel_info *pinfo = NULL;

    pinfo = jdi_panel_data.panel_info;
    sprintf(buf, "jdi_otm1902b 6.1' CMD TFT %d x %d\n",
            pinfo->xres, pinfo->yres);
    ret = strlen(buf) + 1;

    return ret;
}

static ssize_t show_cabc_mode(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", cabc_mode);
}

static ssize_t store_cabc_mode(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
    int ret = 0;
    unsigned long val = 0;

    ret = strict_strtoul(buf, 0, &val);
    if (ret)
        return ret;

    if(val == 1) {
        /* allow application to set cabc mode to ui mode */
        cabc_mode =1;
        jdi_set_cabc(p_tuning_dev, CABC_UI);
    } else if (val == 2) {
        /* force cabc mode to video mode */
        cabc_mode =2;
        jdi_set_cabc(p_tuning_dev, CABC_VID);
    }

    return sprintf((char *)buf, "%d\n", cabc_mode);
}

static DEVICE_ATTR(lcd_info, S_IRUGO, jdi_lcd_info_show, NULL);
static DEVICE_ATTR(cabc_mode, 0644, show_cabc_mode, store_cabc_mode);

static struct attribute *jdi_attrs[] = {
    &dev_attr_lcd_info.attr,
    &dev_attr_cabc_mode.attr,
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
        balongfb_loge("create sysfs file failed!\n");
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
    struct balong_fb_data_type *balongfd = NULL;
    struct balong_panel_info *pinfo = NULL;
    int retval = 0;

    BUG_ON(pdev == NULL);

    pr_info("%s enter succ\n",__func__);
    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    pinfo = &(balongfd->panel_info);
    pr_info("%s,pinfo->lcd_init_step = %d!\n",__func__,pinfo->lcd_init_step);

    if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
        /* lcd set voltage */
        vcc_cmds_tx(NULL, jdi_lcd_vcc_set_cmds, \
                ARRAY_SIZE(jdi_lcd_vcc_set_cmds));

        pinfo->lcd_init_step = LCD_INIT_SEND_SEQUENCE;
    } else if (pinfo->lcd_init_step == LCD_INIT_SEND_SEQUENCE) {
        if (!g_display_on) {
            retval = ts_power_control_notify(TS_RESUME_DEVICE, 0);
            if (retval < 0) {
                balongfb_loge("Failed to send TS_RESUME_DEVICE. Code: %d.\n", retval);
            }

            /* lcd pinctrl normal */
            pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds, \
                    ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));

            /* lcd gpio request */
            gpio_cmds_tx(NULL, jdi_lcd_gpio_request_cmds, \
                    ARRAY_SIZE(jdi_lcd_gpio_request_cmds));

            /* lcd vcc enable */
            vcc_cmds_tx(NULL, jdi_lcd_vcc_enable_cmds, \
                    ARRAY_SIZE(jdi_lcd_vcc_enable_cmds));

            /* lcd gpio normal */
            gpio_cmds_tx(NULL, jdi_lcd_gpio_normal_cmds, \
                    ARRAY_SIZE(jdi_lcd_gpio_normal_cmds));

            /* lcd backlight enable */
            vcc_cmds_tx(NULL, jdi_lcd_bl_enable_cmds, \
                    ARRAY_SIZE(jdi_lcd_bl_enable_cmds));

            /* lcd display on sequence */
            mipi_dsi_cmds_tx(jdi_display_on_cmds, \
                    ARRAY_SIZE(jdi_display_on_cmds), balongfd->dsi_base);

            g_display_on = true;

            LOG_JANK_D(JLID_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
            g_debug_enable = true;
        }


    } else {
        balongfb_loge("failed to init lcd!\n");
    }

    pr_info("%s exit succ!\n",__func__);

    return 0;
}

static int mipi_jdi_panel_off(struct platform_device *pdev)
{
    struct balong_fb_data_type *balongfd = NULL;
    int retval;

    BUG_ON(pdev == NULL);
    pr_info("%s enter succ!\n",__func__);

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    if (g_display_on) {

        /* lcd display off sequence */
        mipi_dsi_cmds_tx(jdi_display_off_cmds, \
                ARRAY_SIZE(jdi_display_off_cmds), balongfd->dsi_base);

        /* shutdown d_phy */
        set_MIPIDSI_PHY_RSTZ(0);

        /* lcd gpio lowpower */
        gpio_cmds_tx(NULL, jdi_lcd_gpio_lowpower_cmds, \
                ARRAY_SIZE(jdi_lcd_gpio_lowpower_cmds));
        /* lcd gpio free */
        gpio_cmds_tx(NULL, jdi_lcd_gpio_free_cmds, \
                ARRAY_SIZE(jdi_lcd_gpio_free_cmds));

        /* lcd pinctrl lowpower */
        pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_lowpower_cmds, \
                ARRAY_SIZE(jdi_lcd_pinctrl_lowpower_cmds));

        /* lcd vcc disable */
        vcc_cmds_tx(NULL, jdi_lcd_vcc_disable_cmds, \
                ARRAY_SIZE(jdi_lcd_vcc_disable_cmds));

        /* lcd backlight disable */
        vcc_cmds_tx(NULL, jdi_lcd_bl_disable_cmds, \
                ARRAY_SIZE(jdi_lcd_bl_disable_cmds));

        retval = ts_power_control_notify(TS_SUSPEND_DEVICE, 100);
        if (retval < 0) {
            balongfb_loge("Failed to send TS_SUSPEND_DEVICE. Code: %d.\n", retval);
        }

        g_display_on = false;
        LOG_JANK_D(JLID_KERNEL_LCD_POWER_OFF, "%s", "JL_KERNEL_LCD_POWER_OFF");
    }

    cnt = 0;
    return 0;
}

static int mipi_jdi_panel_remove(struct platform_device *pdev)
{
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    if (balongfd->panel_info.bl_set_type & BL_SET_BY_PWM) {
        PWM_CLK_PUT(&(balongfd->panel_info));
    }
    /* lcd vcc finit */
    vcc_cmds_tx(pdev, jdi_lcd_vcc_put_cmds, \
            ARRAY_SIZE(jdi_lcd_vcc_put_cmds));

    /* lcd pinctrl lowpower */
    pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_lowpower_cmds, \
            ARRAY_SIZE(jdi_lcd_pinctrl_lowpower_cmds));
    jdi_sysfs_deinit(pdev);

    return 0;
}

static int mipi_jdi_panel_set_backlight(struct platform_device *pdev)
{
    struct balong_fb_data_type *balongfd = NULL;
    u32 level = 0;

    /* Our eyes are more sensitive to small brightness.
       So we adjust the brightness of lcd following iphone4 */
    char bl_level_adjust[2] = {
        0x51,
        0x00,
    };

    struct dsi_cmd_desc jdi_bl_level_adjust[] = {
        {DTYPE_DCS_WRITE1, 0, 0, WAIT_TYPE_US,
            sizeof(bl_level_adjust), bl_level_adjust},
        {DTYPE_DCS_WRITE1, 0, 0, WAIT_TYPE_US,
            sizeof(bl_enable), bl_enable},
    };

    BUG_ON(pdev == NULL);
    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);


    level = balongfd->bl_level;

    if (level > 255)
        level = 255;

    //backlight may turn off when bl_level is below 6.
    if (level < 6 && level != 0)
    {
        level = 6;
    }

    bl_level_adjust[1] = level;



    mipi_dsi_cmds_tx(jdi_bl_level_adjust, ARRAY_SIZE(jdi_bl_level_adjust), balongfd->dsi_base);

    balongfb_logi_backlight_debugfs(" set backlight succ level = %d \n",level);

    if (unlikely(g_debug_enable)) {
        LOG_JANK_D(JLID_KERNEL_LCD_BACKLIGHT_ON, "JL_KERNEL_LCD_BACKLIGHT_ON,%u", level);
        g_debug_enable = false;
    }

    return 0;

}


static int mipi_jdi_panel_set_fastboot(struct platform_device *pdev)
{
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    /* tp vcc enable for fastboot continue display */
    vcc_cmds_tx(NULL, jdi_tp_vcc_enable_cmds, \
            ARRAY_SIZE(jdi_tp_vcc_enable_cmds));

    /* lcd gpio request */
    gpio_cmds_tx(NULL, jdi_lcd_gpio_request_cmds, \
            ARRAY_SIZE(jdi_lcd_gpio_request_cmds));

    /* lcd vcc enable */
    vcc_cmds_tx(NULL, jdi_lcd_vcc_enable_cmds, \
            ARRAY_SIZE(jdi_lcd_vcc_enable_cmds));

    /* lcd backlight enable */
    vcc_cmds_tx(NULL, jdi_lcd_bl_enable_cmds, \
            ARRAY_SIZE(jdi_lcd_bl_enable_cmds));

    g_display_on = true;

    return 0;
}


static int mipi_jdi_panel_set_cabc(struct platform_device *pdev, int value)
{
    return 0;
}

#if PARTIAL_UPDATE
static int mipi_jdi_panel_set_display_region(struct platform_device *pdev, int x, int y, int width, int height)
{

    lcd_disp_x[1] = (x >> 8) & 0xff;
    lcd_disp_x[2] = x & 0xff;
    lcd_disp_x[3] = ((x + width -1) >> 8) & 0xff;
    lcd_disp_x[4] = (x + width -1) & 0xff;

    lcd_disp_y[1] = (y >> 8) & 0xff;
    lcd_disp_y[2] = y & 0xff;
    lcd_disp_y[3] = ((y + height -1) >> 8) & 0xff;
    lcd_disp_y[4] = (y + height -1) & 0xff;

    mipi_dsi_cmds_tx(set_display_address, ARRAY_SIZE(set_display_address), NULL);

    return 0;
}
#endif

static struct balong_panel_info jdi_panel_info = {0};
static struct balong_fb_panel_data jdi_panel_data = {
    .panel_info = &jdi_panel_info,
    .on = mipi_jdi_panel_on,
    .off = mipi_jdi_panel_off,
    .remove = mipi_jdi_panel_remove,
    .set_backlight = mipi_jdi_panel_set_backlight,
    .set_fastboot = mipi_jdi_panel_set_fastboot,
    .set_cabc = mipi_jdi_panel_set_cabc,
#if PARTIAL_UPDATE
    .set_display_region = mipi_jdi_panel_set_display_region,
#endif
};

static int __init jdi_probe(struct platform_device *pdev)
{
    struct balong_panel_info *pinfo = NULL;
    struct platform_device *reg_pdev = NULL;
    struct lcd_properities lcd_props;
    char lcd_name[] = "jdi_1902b 6.1' command TFT";
    int len;
    pr_info("%s 1902b enter succ !zxl\n",__func__);
    len = strlen(lcd_name);
    memset(g_lcd_name_buf,0,sizeof(g_lcd_name_buf));
    strncpy(g_lcd_name_buf,lcd_name, (len >100 ? 100 : len));
    g_display_on = false;
    pinfo = jdi_panel_data.panel_info;

    /* init lcd panel info */
    pinfo->xres = 1080;
    pinfo->yres = 1920;
    pinfo->width  = 61;  //mm
    pinfo->height = 109; //mm
    pinfo->type = PANEL_MIPI_CMD;
    pinfo->orientation = LCD_PORTRAIT;
    pinfo->bpp = ADE_OUT_RGB_888;
    pinfo->s3d_frm = ADE_FRM_FMT_2D;
    pinfo->bgr_fmt = ADE_RGB;
    pinfo->bl_set_type = BL_SET_BY_MIPI;
    pinfo->bl_max = PWM_LEVEL;
    pinfo->bl_min = 1;

    pinfo->frc_enable = 0;
    pinfo->esd_enable = 0;
    pinfo->sbl_enable = 0;

    pinfo->sbl.bl_max = 0xff;
    pinfo->sbl.cal_a = 0x0f;
    pinfo->sbl.cal_b = 0xd8;
    pinfo->sbl.str_limit = 0x40;

    //init support partial update,the limit region should set to correct value
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

    pinfo->ldi.h_back_porch = 23;
    pinfo->ldi.h_front_porch = 50;
    pinfo->ldi.h_pulse_width = 20;
    pinfo->ldi.v_back_porch = 12;
    pinfo->ldi.v_front_porch = 14;
    pinfo->ldi.v_pulse_width = 2;

    pinfo->ldi.hsync_plr = 1;
    pinfo->ldi.vsync_plr = 0;
    pinfo->ldi.pixelclk_plr = 1;
    pinfo->ldi.data_en_plr = 0;

    pinfo->ldi.disp_mode = LDI_DISP_MODE_NOT_3D_FBF;

    /* Note: must init here */
    pinfo->mipi.lane_nums = DSI_4_LANES;
    pinfo->mipi.color_mode = DSI_24BITS_1;
    pinfo->mipi.vc = 0;
    pinfo->frame_rate = 60;
    pdev->id = 1;

    /* lcd resource */
    get_resource_from_dts(pdev, pinfo);

    /* lcd gpio init */
    gpio_cmds_tx(pdev, jdi_lcd_gpio_get_cmds, \
            ARRAY_SIZE(jdi_lcd_gpio_get_cmds));

    /* lcd vcc init */
    vcc_cmds_tx(pdev, jdi_lcd_vcc_get_cmds, \
            ARRAY_SIZE(jdi_lcd_vcc_get_cmds));

    /* lcd pinctrl init */
    pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_init_cmds, \
            ARRAY_SIZE(jdi_lcd_pinctrl_init_cmds));

    /* alloc panel device data */
    if (platform_device_add_data(pdev, &jdi_panel_data,
                sizeof(struct balong_fb_panel_data))) {
        balongfb_loge("platform_device_add_data failed!\n");
        platform_device_put(pdev);
        return -ENOMEM;
    }

    reg_pdev = balong_fb_add_device(pdev);
    if(NULL == reg_pdev){
        balongfb_loge("balong_fb_add_device failed!\n");
        return -1;
    }

    lcd_props.type = TFT;
    lcd_props.default_gamma = GAMMA22;

#if 0
    p_tuning_dev = lcd_tuning_dev_register(&lcd_props, &sp_tuning_ops, (void *)reg_pdev);
    if (IS_ERR(p_tuning_dev)) {
        balongfb_loge("lcd_tuning_dev_register failed!\n");
        return -1;
    }
#endif

    jdi_sysfs_init(pdev);

    pr_info("%s exit succ!\n",__func__);
    return 0;
}

static const struct of_device_id mipi_jdi_otm1902b_match_table[] = {
    {
        .compatible ="hisilicon,mipi-jdi-otm1902b",
        .data = NULL,
    },
    {},
};

static struct platform_driver this_driver = {
    .probe = jdi_probe,
    .remove = NULL,
    .suspend = NULL,
    .resume = NULL,
    .shutdown = NULL,
    .driver = {
        .name = "mipi_jdi_OTM1902B",
        .of_match_table = mipi_jdi_otm1902b_match_table,
    },
};

static int __init mipi_jdi_panel_init(void)
{
    int ret = 0;
#ifndef CONFIG_ARM64
    if( get_lcd_type() != JDI_1902B_LCD)
    {
        return ret;
    }
#endif
    ret = platform_driver_register(&this_driver);
    if (ret) {
        balongfb_loge("not able to register the driver\n");
        return ret;
    }

    return ret;
}


module_init(mipi_jdi_panel_init);
