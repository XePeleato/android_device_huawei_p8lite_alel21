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
#include <huawei_platform/log/log_jank.h>

#if defined (CONFIG_HUAWEI_DSM)
#include <huawei_platform/dsm/dsm_pub.h>
#endif

#define PWM_LEVEL 100
static bool g_debug_enable = false;

#define YASSY_CABC_ENABLED  1
static int g_SRE_level = 0x00;
static int g_SRE_enable = 0x00;
/*--------yassy power on initial code -----------*/
static char yassy_power_on_param1[] =
{
    0xB9,
    0xFF, 0x83, 0x94,
};
// BTA Read and BTA error report. 20141025
static char yassy_power_on_param2[] =
{
    0xBA,
    0x73, 0x83,
};
// VGL as Himax latest spec. 20141025
static char yassy_power_on_param3[] =
{
    0xB1,
    0x6C, 0x10, 0x10, 0x24, 0x04, 0x11, 0xF1, 0x80, 0xEA, 0x55, 0x23, 0x80, 0xC0, 0xD2, 0x58,
};

static char yassy_power_on_param4[] =
{
    0xB2,
    0x00, 0x64, 0x10, 0x07, 0x22, 0x1C, 0x08, 0x08, 0x1C, 0x4D, 0x00,
};
// as CTP spec. GIP timing  20141025
static char yassy_power_on_param5[] =
{
    0xB4,
    0x00, 0xFF, 0x01, 0x50, 0x01, 0x50, 0x01, 0x50, 0x07, 0x6A, 0x11, 0x6A,
};

static char yassy_power_on_param6[] =
{
    0xBF,
    0x41, 0x0E, 0x01,
};
// as CTP spec. GIP timing  20141025
static char yassy_power_on_param7[] =
{
    0xD3,
    0x00, 0x00, 0x00, 0x40, 0x07, 0x00, 0x00, 0x32, 0x10, 0x09, 0x00, 0x09, 0x52, 0x15, 0x11,
    0x05, 0x11, 0x32, 0x10, 0x00, 0x00, 0x00, 0x37, 0x33, 0x0D, 0x0D, 0x37, 0x0C, 0x0C, 0x47,
};

static char yassy_power_on_param8[] =
{
    0xD5,
    0x20, 0x21, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x18, 0x18, 0x24, 0x25,
};

static char yassy_power_on_param9[] =
{
    0xD6,
    0x24, 0x25, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x58, 0x58, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x20, 0x21,
};

static char yassy_power_on_param10[] =
{
    0xE0,
    0x00, 0x06, 0x0B, 0x33, 0x39, 0x3f, 0x1A, 0x3F, 0x06, 0x09, 0x0B, 0x16, 0x0E, 0x12, 0x15,
    0x13, 0x14, 0x07, 0x11, 0x13, 0x17, 0x00, 0x06, 0x0B, 0x33, 0x39, 0x3F, 0x19, 0x3F, 0x06,
    0x09, 0x0B, 0x17, 0x0F, 0x12, 0x15, 0x13, 0x14, 0x07, 0x12, 0x13, 0x17,
};
// set Gamma code
static char yassy_power_on_param11[] =
{
    0xBD,
    0x00,
};
static char yassy_power_on_param12[] =
{
    0xC1,
    0x01, 0x00, 0x0A, 0x14, 0x1D, 0x27, 0x30, 0x3A, 0x44, 0x4C, 0x55, 0x5E, 0x66, 0x6F, 0x78,
    0x80, 0x87, 0x8E, 0x95, 0x9D, 0xA4, 0xAB, 0xB3, 0xB9, 0xC0, 0xC7, 0xCE, 0xD5, 0xDB, 0xE2,
    0xE9, 0xF1, 0xF8, 0xFF, 0x13, 0x3C, 0xA7, 0x80, 0x75, 0x89, 0x57, 0xE1, 0xC0,
};
static char yassy_power_on_param13[] =
{
    0xBD,
    0x01,
};

static char yassy_power_on_param14[] =
{
    0xC1,
    0x00, 0x0A, 0x14, 0x1D, 0x27, 0x30, 0x3A, 0x44, 0x4C, 0x55, 0x5E, 0x66, 0x6F, 0x78, 0x80,
    0x87, 0x8E, 0x95, 0x9D, 0xA4, 0xAB, 0xB3, 0xB9, 0xC0, 0xC7, 0xCE, 0xD5, 0xDB, 0xE2, 0xE9,
    0xF1, 0xF8, 0xFF, 0x13, 0x3C, 0xA7, 0x80, 0x75, 0x89, 0x57, 0xE1, 0xC0,
};

static char yassy_power_on_param15[] =
{
    0xBD,
    0x02,
};

static char yassy_power_on_param16[] =
{
    0xC1,
    0x00, 0x0A, 0x14, 0x1D, 0x27, 0x30, 0x3A, 0x44, 0x4C, 0x55, 0x5E, 0x66, 0x6F, 0x78, 0x80,
    0x87, 0x8E, 0x95, 0x9D, 0xA4, 0xAB, 0xB3, 0xB9, 0xC0, 0xC7, 0xCE, 0xD5, 0xDB, 0xE2, 0xE9,
    0xF1, 0xF8, 0xFF, 0x13, 0x3C, 0xA7, 0x80, 0x75, 0x89, 0x57, 0xE1, 0xC0,
};

static char yassy_power_on_param17[] =
{
    0xC0,
    0x30, 0x14,
};

static char yassy_power_on_param18[] =
{
    0xC7,
    0x00, 0xC0, 0x40, 0xC0,
};

static char yassy_power_on_param19[] =
{
    0xCC,
    0x09,
};

static char yassy_power_on_param20[] =
{
    0xBC,
    0x07,
};

static char yassy_power_on_param21[] =
{
    0xC9,
    0x1F, 0x00, 0x12, 0x1E, 0x00, 0x1E, 0x00,
};
// The reg E4h/E5h/E6h is used for CE function.
static char yassy_power_on_param22[] =
{
    0xE4,
    0x41, 0x00,
};
static char yassy_power_on_param23[] =
{
    0xE5,
    0x00, 0x04, 0x08, 0x0A, 0x0C, 0x0C, 0x02, 0x20, 0x50, 0x10, 0xF3,
};
static char yassy_power_on_param24[] =
{
    0xE6,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x1D, 0x20, 0x1A, 0x20, 0x20, 0x20, 0x16, 0x12, 0x16, 0x20, 0x20, 0x20,
};
static char bl_level_0[] =
{
    0x51,
    0x00,
};

static char bl_level[] =
{
    0x51,
    0x00,
};

static char bl_enable[] =
{
    0x53,
    0x24,
};

static char yassy_power_on_param5E[] =
{
    0x5E,
    0x0A,
};

static char exit_sleep[] =
{
    0x11,
};

static char display_on[] =
{
    0x29,
};
#if  YASSY_CABC_ENABLED
//Init cabc to UI mode when panel on
// Set the sequence of CABC and CE. 0x01 -> 0x11
static char cabc_mode[] =
{
    0x55,
    0x11,
};

//set cabc mode according to application situation
static char  yassy_cabc_ui_on[] =
{
    0x55,
    0x11,
};

static char  yassy_cabc_vid_on[] =
{
    0x55,
    0x13,
};

static char  yassy_cabc_off[] =
{
    0x55,
    0x10,
};

static char yassy_dimming_off_ui[] =
{
    0x53,
    0x24,
};

static char yassy_dimming_on_video[] =
{
    0x53,
    0x2C,
};

static char yassy_cabc_dimming_off_ui[] =
{
    0xC9,
    0x1F, 0x00, 0x12, 0x1E, 0x00, 0x1E, 0x00,
};

static char yassy_cabc_dimming_on_video[] =
{
    0xC9,
    0x1F, 0x00, 0x12, 0x1E, 0x81, 0x1E, 0x00,
};

static struct dsi_cmd_desc yassy_cabc_ui_cmds[] =
{
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(yassy_cabc_ui_on), yassy_cabc_ui_on
    },
};

static struct dsi_cmd_desc yassy_cabc_vid_cmds[] =
{
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(yassy_cabc_vid_on), yassy_cabc_vid_on
    },
};

static struct dsi_cmd_desc yassy_cabc_off_cmds[] =
{
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(yassy_cabc_off), yassy_cabc_off
    },
};

static struct dsi_cmd_desc yassy_dimming_off_ui_cmds[] =
{
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(yassy_dimming_off_ui), yassy_dimming_off_ui
    },
};

static struct dsi_cmd_desc yassy_cabc_dimming_off_ui_cmds[] =
{
    {
        DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(yassy_cabc_dimming_off_ui), yassy_cabc_dimming_off_ui
    },
};

static struct dsi_cmd_desc yassy_dimming_on_video_cmds[] =
{
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(yassy_dimming_on_video), yassy_dimming_on_video
    },
};

static struct dsi_cmd_desc yassy_cabc_dimming_on_video_cmds[] =
{
    {
        DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(yassy_cabc_dimming_on_video), yassy_cabc_dimming_on_video
    },
};

#endif
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
static struct dsi_cmd_desc yassy_display_on_cmds[] =
{
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param1), yassy_power_on_param1
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param2), yassy_power_on_param2
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param3), yassy_power_on_param3
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param4), yassy_power_on_param4
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param5), yassy_power_on_param5
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param6), yassy_power_on_param6
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param7), yassy_power_on_param7
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param8), yassy_power_on_param8
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param9), yassy_power_on_param9
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param10), yassy_power_on_param10
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param11), yassy_power_on_param11
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param12), yassy_power_on_param12
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param13), yassy_power_on_param13
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param14), yassy_power_on_param14
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param15), yassy_power_on_param15
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param16), yassy_power_on_param16
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param17), yassy_power_on_param17
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param18), yassy_power_on_param18
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param19), yassy_power_on_param19
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param20), yassy_power_on_param20
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param21), yassy_power_on_param21
    },
    {
        DTYPE_DCS_LWRITE, 0, 5, WAIT_TYPE_MS,
        sizeof(yassy_power_on_param22), yassy_power_on_param22
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param23), yassy_power_on_param23
    },
    {
        DTYPE_DCS_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(yassy_power_on_param24), yassy_power_on_param24
    },
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(bl_level), bl_level
    },
#if  YASSY_CABC_ENABLED
    {
        DTYPE_DCS_WRITE1, 0, 5, WAIT_TYPE_MS,
        sizeof(cabc_mode), cabc_mode
    },
#endif
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(bl_enable), bl_enable
    },
    {
        DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
        sizeof(yassy_power_on_param5E), yassy_power_on_param5E
    },

    {
        DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
        sizeof(exit_sleep), exit_sleep
    },
    {
        DTYPE_DCS_WRITE, 0, 20, WAIT_TYPE_MS,
        sizeof(display_on), display_on
    },
};

static struct dsi_cmd_desc yassy_display_off_cmds[] =
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
static struct regulator* lcd_bl_vcc;
static struct regulator* lcd_bias_vcc;
static struct regulator* lcd_vsn_vcc;
static struct regulator* lcd_vsp_vcc;

static struct vcc_desc yassy_lcd_vcc_get_cmds[] =
{
    /* vcc get */
    {DTYPE_VCC_GET, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, 0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  0, 0, 0, 0, 0, 0},
};

static struct vcc_desc yassy_lcd_vcc_set_cmds[] =
{
    /* vcc set voltage */
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCDBIAS_NAME, &lcd_bias_vcc,  0, 0, 5400000, 5400000, 0, 0},
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCD_VSP_NAME, &lcd_vsp_vcc,   0, 0, 5400000, 5400000, 0, 0},
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCD_VSN_NAME, &lcd_vsn_vcc,   0, 0, 5400000, 5400000, 0, 0},
};

static struct vcc_desc yassy_lcd_vcc_put_cmds[] =
{
    /* vcc put */
    {DTYPE_VCC_PUT, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, 0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  0, 0, 0, 0, 0, 0},
};

static struct vcc_desc yassy_lcd_vcc_enable_cmds[] =
{
    /* vcc enable */
    {DTYPE_VCC_ENABLE, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, WAIT_TYPE_MS, 12, 0, 0, 0, 0},
    {DTYPE_VCC_ENABLE, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  WAIT_TYPE_MS, 10, 0, 0, 0, 0},
    {DTYPE_VCC_ENABLE, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  WAIT_TYPE_MS, 50, 0, 0, 0, 0},
};

static struct vcc_desc yassy_lcd_vcc_disable_cmds[] =
{
    /* vcc disable */
    {DTYPE_VCC_DISABLE, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  WAIT_TYPE_MS, 5, 0, 0, 0, 0},
    {DTYPE_VCC_DISABLE, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  WAIT_TYPE_MS, 5, 0, 0, 0, 0},
    {DTYPE_VCC_DISABLE, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, WAIT_TYPE_MS, 1, 0, 0, 0, 0},
};

static struct vcc_desc yassy_lcd_bl_enable_cmds[] =
{
    /* backlight enable */
    {DTYPE_VCC_ENABLE, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   WAIT_TYPE_MS, 10, 0, 0, 0, 0},
};

static struct vcc_desc yassy_lcd_bl_disable_cmds[] =
{
    /* backlight disable */
    {DTYPE_VCC_DISABLE, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,  WAIT_TYPE_MS, 1, 0, 0, 0, 0},
};

/************************************************************
 *
 * LCD GPIO PINCTRL
 *
 ************************************************************/
static struct pinctrl_data pctrl;

static struct pinctrl_cmd_desc yassy_lcd_pinctrl_init_cmds[] =
{
    {DTYPE_PINCTRL_GET,       &pctrl, 0},
    {DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
    {DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};
static struct pinctrl_cmd_desc yassy_lcd_pinctrl_normal_cmds[] =
{
    {DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};
static struct pinctrl_cmd_desc yassy_lcd_pinctrl_lowpower_cmds[] =
{
    {DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

#define GPIO_LCD_RESET_NAME      "lcd_reset_gpio"
static u32 gpio_lcd_reset;
static struct gpio_desc yassy_lcd_gpio_get_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_GET, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc yassy_lcd_gpio_request_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc yassy_lcd_gpio_free_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};
static struct gpio_desc yassy_lcd_gpio_normal_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 120, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
};

static struct gpio_desc yassy_lcd_gpio_lowpower_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static volatile bool g_display_on;
static struct balong_fb_panel_data yassy_panel_data;

/******************************************************************************/
static struct lcd_tuning_dev* p_tuning_dev = NULL;
//show lcd_info
static ssize_t yassy_lcd_info_show(struct device* dev,
                                   struct device_attribute* attr, char* buf)
{
    int ret = 0;
    struct balong_panel_info* pinfo = NULL;

    pinfo = yassy_panel_data.panel_info;
    snprintf(buf, PAGE_SIZE, "%s %d x %d\n", g_lcd_name_buf, pinfo->xres, pinfo->yres);
    ret = strlen(buf) + 1;

    return ret;
}

#define YASSY_ESD_DEBUG	0
#if YASSY_ESD_DEBUG
/*for esd test*/
static int esd_flag = 0;
static ssize_t yassy_show_esd_test(struct device* dev,
                                   struct device_attribute* attr, char* buf)
{
    return sprintf(buf, "%d\n", esd_flag);
}
static ssize_t yassy_store_esd_test(struct device* dev,
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
static DEVICE_ATTR(esd_test, 0644, yassy_show_esd_test, yassy_store_esd_test);
#endif

static DEVICE_ATTR(lcd_info, S_IRUGO, yassy_lcd_info_show, NULL);


static struct attribute* yassy_attrs[] =
{
    &dev_attr_lcd_info.attr,
#if YASSY_ESD_DEBUG
    /*for esd test*/
    &dev_attr_esd_test.attr,
#endif
    NULL,
};

static struct attribute_group yassy_attr_group =
{
    .attrs = yassy_attrs,
};

static int yassy_sysfs_init(struct platform_device* pdev)
{
    int ret = 0;
    ret = sysfs_create_group(&pdev->dev.kobj, &yassy_attr_group);
    if (ret)
    {
        balongfb_loge("create sysfs file failed!\n");
        return ret;
    }
    return 0;
}

static void yassy_sysfs_deinit(struct platform_device* pdev)
{
    sysfs_remove_group(&pdev->dev.kobj, &yassy_attr_group);
}

static int mipi_yassy_panel_on(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;
    struct balong_panel_info* pinfo = NULL;
    struct balong_fb_panel_data* pdata = NULL;

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
        vcc_cmds_tx(NULL, yassy_lcd_vcc_set_cmds, \
                    ARRAY_SIZE(yassy_lcd_vcc_set_cmds));

        pinfo->lcd_init_step = LCD_INIT_SEND_SEQUENCE;
    }
    else if (pinfo->lcd_init_step == LCD_INIT_SEND_SEQUENCE)
    {
        if (!g_display_on)
        {
            LOG_JANK_D(JLID_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
            g_debug_enable = true;

            /* lcd pinctrl normal */
            pinctrl_cmds_tx(pdev, yassy_lcd_pinctrl_normal_cmds, \
                            ARRAY_SIZE(yassy_lcd_pinctrl_normal_cmds));

            /* lcd gpio request */
            gpio_cmds_tx(NULL, yassy_lcd_gpio_request_cmds, \
                         ARRAY_SIZE(yassy_lcd_gpio_request_cmds));

            /* lcd vcc enable */
            vcc_cmds_tx(NULL, yassy_lcd_vcc_enable_cmds, \
                        ARRAY_SIZE(yassy_lcd_vcc_enable_cmds));

            /* lcd gpio normal */
            gpio_cmds_tx(NULL, yassy_lcd_gpio_normal_cmds, \
                         ARRAY_SIZE(yassy_lcd_gpio_normal_cmds));
            /* remove lcd backlight enable */

            /* lcd display on sequence */
            mipi_dsi_cmds_tx(yassy_display_on_cmds, \
                             ARRAY_SIZE(yassy_display_on_cmds), balongfd->dsi_base);

            g_display_on = true;
            /*first set sbl to panel*/
            if (balongfd->panel_info.sbl_enable)
            {
                pdata = (struct balong_fb_panel_data*)balongfd->pdev->dev.platform_data;
                if (pdata->set_sre != NULL)
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

static int mipi_yassy_panel_off(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);
    pr_info("%s enter succ!\n", __func__);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    if (g_display_on)
    {
        LOG_JANK_D(JLID_KERNEL_LCD_POWER_OFF, "%s", "JL_KERNEL_LCD_POWER_OFF");

        /* lcd display off sequence */
        mipi_dsi_cmds_tx(yassy_display_off_cmds, \
                         ARRAY_SIZE(yassy_display_off_cmds), balongfd->dsi_base);

        /* shutdown d_phy */
        set_MIPIDSI_PHY_RSTZ(0);

        /* lcd gpio lowpower */
        gpio_cmds_tx(NULL, yassy_lcd_gpio_lowpower_cmds, \
                     ARRAY_SIZE(yassy_lcd_gpio_lowpower_cmds));
        /* lcd gpio free */
        gpio_cmds_tx(NULL, yassy_lcd_gpio_free_cmds, \
                     ARRAY_SIZE(yassy_lcd_gpio_free_cmds));

        /* lcd pinctrl lowpower */
        pinctrl_cmds_tx(pdev, yassy_lcd_pinctrl_lowpower_cmds, \
                        ARRAY_SIZE(yassy_lcd_pinctrl_lowpower_cmds));

        /* lcd vcc disable */
        vcc_cmds_tx(NULL, yassy_lcd_vcc_disable_cmds, \
                    ARRAY_SIZE(yassy_lcd_vcc_disable_cmds));
        /* remove lcd backlight disable */
        g_display_on = false;
    }

    pr_info("%s exit succ!\n", __func__);
    return 0;
}

static int mipi_yassy_panel_remove(struct platform_device* pdev)
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
    vcc_cmds_tx(pdev, yassy_lcd_vcc_put_cmds, \
                ARRAY_SIZE(yassy_lcd_vcc_put_cmds));

    /* lcd pinctrl lowpower */
    pinctrl_cmds_tx(pdev, yassy_lcd_pinctrl_lowpower_cmds, \
                    ARRAY_SIZE(yassy_lcd_pinctrl_lowpower_cmds));
    yassy_sysfs_deinit(pdev);

    return 0;
}
static int yassy_set_gamma(struct lcd_tuning_dev* ltd, enum lcd_gamma gamma)
{
    int ret = 0;
    struct platform_device* pdev = NULL;
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(ltd == NULL);
    pdev = (struct platform_device*)(ltd->data);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    /* Fix me: Implement it */
    return ret;
}
static int yassy_set_cabc(struct platform_device* pdev, int cabc)
{
    int ret = 0;
    struct balong_fb_data_type* balongfd = NULL;

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

#if YASSY_CABC_ENABLED
    switch (cabc)
    {
        case CABC_UI:              // set CABC as UI Mode
        {
            balongfd->g_CABC_mode = 0x01;
            mipi_dsi_cmds_tx(yassy_cabc_ui_cmds, ARRAY_SIZE(yassy_cabc_ui_cmds), balongfd->dsi_base);
            mipi_dsi_cmds_tx(yassy_dimming_off_ui_cmds, ARRAY_SIZE(yassy_dimming_off_ui_cmds), balongfd->dsi_base);
            mipi_dsi_cmds_tx(yassy_cabc_dimming_off_ui_cmds, ARRAY_SIZE(yassy_cabc_dimming_off_ui_cmds), balongfd->dsi_base);
        }
        break;

        case CABC_VID:			   // set CABC as Video Mode
        {
            balongfd->g_CABC_mode = 0x03;
            mipi_dsi_cmds_tx(yassy_cabc_vid_cmds, ARRAY_SIZE(yassy_cabc_vid_cmds), balongfd->dsi_base);
            mipi_dsi_cmds_tx(yassy_dimming_on_video_cmds, ARRAY_SIZE(yassy_dimming_on_video_cmds), balongfd->dsi_base);
            mipi_dsi_cmds_tx(yassy_cabc_dimming_on_video_cmds, ARRAY_SIZE(yassy_cabc_dimming_on_video_cmds), balongfd->dsi_base);
        }
        break;

        case CABC_OFF:			   // close CABC
            balongfd->g_CABC_mode = 0x00;
            mipi_dsi_cmds_tx(yassy_cabc_off_cmds, ARRAY_SIZE(yassy_cabc_off_cmds), balongfd->dsi_base);
            break;
        default:
            ret = -1;
            break;
    }
    pr_info("%s CABC_MODE =  %d\n", __func__, cabc);
#endif
    return ret;
}

static struct lcd_tuning_ops sp_tuning_ops =
{
    .set_gamma = yassy_set_gamma,
    .set_cabc = yassy_set_cabc,
};
static int mipi_yassy_panel_set_backlight(struct platform_device* pdev)
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

    struct dsi_cmd_desc yassy_bl_level_adjust[] =
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

    mipi_dsi_cmds_tx(yassy_bl_level_adjust, ARRAY_SIZE(yassy_bl_level_adjust), balongfd->dsi_base);
    if (level == 0)
    {
        vcc_cmds_tx(NULL, yassy_lcd_bl_disable_cmds, \
                    ARRAY_SIZE(yassy_lcd_bl_disable_cmds));
    }
    else if (last_level == 0 && level != 0)
    {
        vcc_cmds_tx(NULL, yassy_lcd_bl_enable_cmds, \
                    ARRAY_SIZE(yassy_lcd_bl_enable_cmds));
    }
    last_level = level;
    //modified for beta test, it will be modified after beta test.
    balongfb_loge(" set backlight succ ,balongfd->bl_level = %d, level = %d \n", balongfd->bl_level, level);

    if (unlikely(g_debug_enable))
    {
        LOG_JANK_D(JLID_KERNEL_LCD_BACKLIGHT_ON, "JL_KERNEL_LCD_BACKLIGHT_ON,%u", level);
        g_debug_enable = false;
    }
    return 0;
}
#if LCD_CHECK_MIPI_SUPPORT
static int mipi_yassy_check_mipi_tr(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;
    u32 read_value = 0;
    int err = 0;
    BUG_ON(pdev == NULL);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);
    balongfd->lcd_readvalue = 0;
    //read 0x0a
    err = mipi_dsi_read_reg(balongfd, 0x0a, &read_value, DTYPE_GEN_READ1);
    if (err ||  (read_value & 0xff) != 0x1c)
    {
        err = -1;
        goto check_mipi_tr_error;
    }
    balongfd->lcd_readvalue |= (read_value & 0xff) << 24;
    //read 0x0b
    err = mipi_dsi_read_reg(balongfd, 0x0b, &read_value, DTYPE_GEN_READ1);
    if (err || (read_value & 0xff) != 0x00)
    {
        err = -1;
        goto check_mipi_tr_error;
    }
    balongfd->lcd_readvalue |= (read_value & 0xff) << 16;
    //read 0x0c
    err = mipi_dsi_read_reg(balongfd, 0x0c, &read_value, DTYPE_GEN_READ1);
    if (err ||  (read_value & 0xff) != 0x70)
    {
        err = -1;
        goto check_mipi_tr_error;
    }
    balongfd->lcd_readvalue |= (read_value & 0xff) << 8;
    //read 0x0d
    err = mipi_dsi_read_reg(balongfd, 0x0d, &read_value, DTYPE_GEN_READ1);
    if (err ||  (read_value & 0xff) != 0x00)
    {
        err = -1;
        goto check_mipi_tr_error;
    }
    balongfd->lcd_readvalue |= (read_value & 0xff);
check_mipi_tr_error:
    return err;

}

static char inversion_mode_1dot[] =
{
    0xB2,
    0x00, 0x64, 0x10, 0x07, 0x32, 0x1C, 0x08, 0x08, 0x1C, 0x4D, 0x00,
};

static char inversion_mode_column[] =
{
    0xB2,
    0x00, 0x64, 0x10, 0x07, 0x32, 0x1C, 0x08, 0x08, 0x1C, 0x4D, 0x00,
};

static struct dsi_cmd_desc  hx8394_lcd_inversion_type_1dot[] =
{
    {
        DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(inversion_mode_1dot), inversion_mode_1dot
    },
};

static struct dsi_cmd_desc  hx8394_lcd_inversion_type_column[] =
{
    {
        DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
        sizeof(inversion_mode_column), inversion_mode_column
    },
};

static int mipi_yassy_set_inversion_type(struct platform_device* pdev, unsigned int inversion_mode)
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
            mipi_dsi_cmds_tx(hx8394_lcd_inversion_type_1dot, \
                             ARRAY_SIZE(hx8394_lcd_inversion_type_1dot), balongfd->dsi_base);
            balongfb_loge("INVERSION_TYPE_1DOT\n");
            ret = 0;
            break;
        case INVERSION_TYPE_COLUMN:
            mipi_dsi_cmds_tx(hx8394_lcd_inversion_type_column, \
                             ARRAY_SIZE(hx8394_lcd_inversion_type_column), balongfd->dsi_base);
            balongfb_loge("INVERSION_TYPE_COLUMN\n");
            ret = 0;
            break;
        default:
            mipi_dsi_cmds_tx(hx8394_lcd_inversion_type_column, \
                             ARRAY_SIZE(hx8394_lcd_inversion_type_column), balongfd->dsi_base);
            balongfb_loge("default LCD inversion type, INVERSION_TYPE_COLUMN.\n");
            ret = -EINVAL;
            break;
    }
    return ret;
}

#endif
static int mipi_yassy_panel_set_fastboot(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    /* lcd gpio request */
    gpio_cmds_tx(NULL, yassy_lcd_gpio_request_cmds, \
                 ARRAY_SIZE(yassy_lcd_gpio_request_cmds));

    /* lcd vcc enable */
    vcc_cmds_tx(NULL, yassy_lcd_vcc_enable_cmds, \
                ARRAY_SIZE(yassy_lcd_vcc_enable_cmds));

    /* lcd backlight enable */
    vcc_cmds_tx(NULL, yassy_lcd_bl_enable_cmds, \
                ARRAY_SIZE(yassy_lcd_bl_enable_cmds));
    g_display_on = true;

    return 0;
}

static int mipi_yassy_panel_set_sre(struct platform_device* pdev, int enable, int value)
{
    struct balong_fb_data_type* balongfd = NULL;
    /* The reg D0h is used for SBL function */
    char slr_setting[4] =
    {
        0xD0,
        0x80, 0x00, 0x00,
    };
    struct dsi_cmd_desc yassy_slr_level_setting[] =
    {
        {
            DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
            sizeof(slr_setting), slr_setting
        },
    };

    BUG_ON(pdev == NULL);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    //update flag depends on status get from light-sensor.
    if (balongfd->sbl_enable == 1)	                                          // Outdoor mode (check the SRE select box)
    {
        if ( 0x40 != g_SRE_level )                             //  set reg D0h only when the sbl_level  changed
        {
            g_SRE_level = 0x40;                                //SBL Normal LEVEL
            g_SRE_enable = 0x01;                                // enable SLR function
            slr_setting[2] = g_SRE_level;
            slr_setting[3] = g_SRE_enable;
            mipi_dsi_cmds_tx(yassy_slr_level_setting, ARRAY_SIZE(yassy_slr_level_setting), balongfd->dsi_base); //send to IC reg
        }
    }
    else                                    // 1.Indoor mode (check the SRE select box).  2.Uncheck the SRE select box.
    {
        if ( 0x40 == g_SRE_level )                         //  set reg 0xD0 only when the value changed
        {
            g_SRE_level = 0x00;                                //zero enhancement,
            g_SRE_enable = 0x00;                               // disable SLR function
            slr_setting[2] = g_SRE_level;
            slr_setting[3] = g_SRE_enable;
            mipi_dsi_cmds_tx(yassy_slr_level_setting, ARRAY_SIZE(yassy_slr_level_setting), balongfd->dsi_base);//send to IC reg
        }
    }
    pr_info("%s enter succ! sbl_enable= %d,  g_SRE_level= 0x%x, g_SRE_enable= 0x%x\n", __func__, balongfd->sbl_enable, slr_setting[2] , slr_setting[3]);
    return 0;
}
#if LCD_ESD_CHECK_SUPPORT
/*for esd check*/
static int mipi_yassy_panel_check_esd(struct platform_device* pdev)
{
    int ret = 0;
    int err = 0;
    unsigned int val = 0;
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    ret = mipi_dsi_read_reg(balongfd, 0xD9, &val, DTYPE_GEN_READ1);
    if ((ret == 0) && ((val & 0x80) == 0x0))
    {
        err = 1;
        balongfb_loge("check esd reg 0xd9 is:0x%x.\n", val);
    }
#if YASSY_ESD_DEBUG
    if (esd_flag != 0)
    {
        err = 1;
    }
#endif
    return err;
}
#endif //LCD_ESD_CHECK_SUPPORT
static struct balong_panel_info yassy_panel_info = {0};
static struct balong_fb_panel_data yassy_panel_data =
{
    .panel_info = &yassy_panel_info,
    .on = mipi_yassy_panel_on,
    .off = mipi_yassy_panel_off,
    .remove = mipi_yassy_panel_remove,
    .set_backlight = mipi_yassy_panel_set_backlight,
    .set_fastboot = mipi_yassy_panel_set_fastboot,
    .set_sre = mipi_yassy_panel_set_sre,
#if LCD_ESD_CHECK_SUPPORT
    /*for esd check*/
    .check_esd = mipi_yassy_panel_check_esd,
#endif //LCD_ESD_CHECK_SUPPORT
#if  LCD_CHECK_MIPI_SUPPORT
    .check_mipi_tr = mipi_yassy_check_mipi_tr,
    .set_inversion_type = mipi_yassy_set_inversion_type,
#endif

    .lcd_cabc_mode_store  =  yassy_set_cabc,
    .lcd_gram_check_show  =  mipi_yassy_check_mipi_tr,
    .lcd_inversion_mode_store = mipi_yassy_set_inversion_type,
};

static int __init yassy_probe(struct platform_device* pdev)
{
    struct balong_panel_info* pinfo = NULL;
    struct platform_device* reg_pdev = NULL;
    struct lcd_properities lcd_props;
    /*add lcd info*/
    char lcd_name[] = "yassy_hx8394 5.5' VIDEO TFT";
    int len;
    pr_info("%s enter succ!!!!\n", __func__);
    /*add lcd info*/
    len = strlen(lcd_name);
    memset(g_lcd_name_buf, 0, sizeof(g_lcd_name_buf));
    strncpy(g_lcd_name_buf, lcd_name, (len > 100 ? 100 : len));
    g_display_on = false;
    pinfo = yassy_panel_data.panel_info;

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
    pinfo->ldi.h_back_porch = 32;//63;
    pinfo->ldi.h_front_porch = 169;//162;
    pinfo->ldi.h_pulse_width = 2;//10;
    pinfo->ldi.v_back_porch = 14;
    pinfo->ldi.v_front_porch = 36;
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
    gpio_cmds_tx(pdev, yassy_lcd_gpio_get_cmds, \
                 ARRAY_SIZE(yassy_lcd_gpio_get_cmds));

    /* lcd vcc init */
    vcc_cmds_tx(pdev, yassy_lcd_vcc_get_cmds,
                ARRAY_SIZE(yassy_lcd_vcc_get_cmds));

    /* lcd pinctrl init */
    pinctrl_cmds_tx(pdev, yassy_lcd_pinctrl_init_cmds,
                    ARRAY_SIZE(yassy_lcd_pinctrl_init_cmds));

    /* alloc panel device data */
    if (platform_device_add_data(pdev, &yassy_panel_data,
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
    yassy_sysfs_init(pdev);

    pr_info("%s exit succ!\n", __func__);
    return 0;
}
static const struct of_device_id mipi_yassy_hx8394_match_table[] =
{
    {
        .compatible = "hisilicon,mipi-yassy-hx8394",
        .data = NULL,
    },
    {},
};

static struct platform_driver this_driver =
{
    .probe = yassy_probe,
    .remove = NULL,
    .suspend = NULL,
    .resume = NULL,
    .shutdown = NULL,
    .driver = {
        .name = "mipi_yassy_hx8394",
        .of_match_table = mipi_yassy_hx8394_match_table,
    },
};

static int __init mipi_yassy_panel_init(void)
{
    int ret = 0;
#ifndef CONFIG_ARM64
    if ( get_lcd_type() != YASSY_HX8394_LCD)
    {
        balongfb_loge("lcd type is not yassy_hx8394_LCD.\n");
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

module_init(mipi_yassy_panel_init);
