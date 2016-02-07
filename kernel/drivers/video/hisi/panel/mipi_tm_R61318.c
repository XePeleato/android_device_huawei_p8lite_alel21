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

#define TM_CABC_ENABLED  0
static int g_IE_SRE_level = 0x80;

/*--------TM power on initial code -----------*/
static char tianma_power_on_param1[] =
{
    0xB0,
    0x00,
};

static char tianma_power_on_param2[] =
{
    0xE3,
    0x01,
};

static char tianma_power_on_param3[] =
{
    0xB6,
    0x32,
};

static char tianma_power_on_param4[] =
{
    0xE5,
    0x03,
};

static char tianma_power_on_param5[] =
{
    0xC0,
    0x23, 0xB2, 0x1D, 0x08, 0x22, 0x7F,
};

static char tianma_power_on_param6[] =
{
    0xC1,
    0x05, 0x78, 0x05, 0x78, 0x00, 0x00, 0x00,
};

static char tianma_power_on_param7[] =
{
    0xC4,
    0xC3, 0xDD, 0x00,
};

static char tianma_power_on_param8[] =
{
    0xC5,
    0X0B, 0x10, 0X13,
};

static char tianma_power_on_param9[] =
{
    0xC6,
    0x29,
};

static char tianma_power_on_param10[] =
{
    0xC8,
    0x31, 0x82, 0x00, 0xA4, 0x98, 0x83, 0x92, 0x5A, 0xAC, 0xB9, 0x07, 0xA3, 0x9C, 0x95, 0xD2, 0x3A, 0x65, 0x84, 0xCF, 0x35, 0xB6, 0x54, 0x42, 0xC7, 0x14, 0x02, 0x44, 0x18,
};

static char tianma_power_on_param11[] =
{
    0xCA,
    0x00, 0x10, 0x15, 0x1B, 0x22, 0x27, 0x29, 0x26, 0x1D, 0x1D, 0x19, 0x13, 0x0C, 0x08, 0x04, 0x00, 0x10, 0x15, 0x1B, 0x22, 0x27, 0x29, 0x26, 0x1D, 0x1D, 0x19, 0x13, 0x0C, 0x08, 0x04,
};

static char tianma_power_on_param12[] =
{
    0xD0,
    0x05, 0x8B, 0x42,
};

static char tianma_power_on_param13[] =
{
    0xD1,
    0x0A,
};

static char tianma_power_on_param14[] =
{
    0xD5,
    0x44, 0x44,
};

static char tianma_power_on_param15[] =
{
    0xB7,
    0x82, 0x3C, 0x10, 0x3C, 0xAB,
};

static char tianma_power_on_param16[] =
{
    0xB8,
    0x32, 0x40, 0x43, 0x49, 0x55, 0x62, 0x71, 0x82, 0x84, 0xA8, 0xB9, 0x89, 0xDB, 0xCA, 0xE5, 0xFC, 0xFF,
};

static char tianma_power_on_param17[] =
{
    0xBB,
    0x2E,
};

/*set PWM to 40 kHz*/
static char tianma_power_on_param18[] =
{
    0xBC,
    0x00, 0x5F, 0x04, 0x04, 0x00,
};
/*****************/

static char tianma_power_on_param19[] =
{
    0xB7,
    0x50, 0x02,
};

static char tianma_power_on_param20[] =
{
    0xF3,
    0x01, 0x00, 0x00, 0x00, 0x00,
};

static char tianma_power_on_param21[] =
{
    0x35,
    0x00,
};

static char tianma_power_on_param22[] =
{
    0x51,
    0x00,
};

static char tianma_power_on_param23[] =
{
    0x53,
    0x24,
};

static char tianma_power_on_param24[] =
{
    0x55,
    0x00,
};

static char tianma_power_on_param25[] =
{
    0x11,
};

static char tianma_power_on_param26[] =
{
    0x29,
};


/*----------------Power ON Sequence(sleep mode to Normal mode)---------------------*/
static char bl_level_0[] =
{
    0x51,
    0x00,
};

static char bl_enable[] =
{
    0x53,
    0x24,
};

//CABC
#if  TM_CABC_ENABLED
//Init cabc to UI mode when panel on
static char cabc_mode[] =
{
    0x55,
    0x81,
};
//set cabc mode according to application situation
static char  tm_cabc_ui_on[] =
{
    0x55,
    0x01,
};

static char  tm_cabc_vid_on[] =
{
    0x55,
    0x03,
};

static struct dsi_cmd_desc tm_cabc_ui_cmds[] =
{
    {
        DTYPE_GEN_LWRITE, 0, 30, WAIT_TYPE_US,
        sizeof(tm_cabc_ui_on), tm_cabc_ui_on
    },
};

static struct dsi_cmd_desc tm_cabc_vid_cmds[] =
{
    {
        DTYPE_GEN_LWRITE, 0, 30, WAIT_TYPE_US,
        sizeof(tm_cabc_vid_on), tm_cabc_vid_on
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

static char mft_cmd_access_enable[] =
{
    0xB0,
    0x00,
};
static char enter_lowpower_mode[] =
{
    0xB1,
    0x01,
};

static struct dsi_cmd_desc tm_display_on_cmds[] =
{
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param1), tianma_power_on_param1
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param2), tianma_power_on_param2
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param3), tianma_power_on_param3
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param4), tianma_power_on_param4
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param5), tianma_power_on_param5
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param6), tianma_power_on_param6
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param7), tianma_power_on_param7
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param8), tianma_power_on_param8
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param9), tianma_power_on_param9
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param10), tianma_power_on_param10
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param11), tianma_power_on_param11
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param12), tianma_power_on_param12
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param13), tianma_power_on_param13
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param14), tianma_power_on_param14
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param15), tianma_power_on_param15
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param16), tianma_power_on_param16
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param17), tianma_power_on_param17
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param18), tianma_power_on_param18
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param19), tianma_power_on_param19
    },
    {
        DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param20), tianma_power_on_param20
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param21), tianma_power_on_param21
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param22), tianma_power_on_param22
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param23), tianma_power_on_param23
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(tianma_power_on_param24), tianma_power_on_param24
    },
    {
        DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
        sizeof(tianma_power_on_param25), tianma_power_on_param25
    },
    {
        DTYPE_DCS_WRITE, 0, 20, WAIT_TYPE_MS,
        sizeof(tianma_power_on_param26), tianma_power_on_param26
    },
};

static struct dsi_cmd_desc tm_display_off_cmds[] =
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
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(mft_cmd_access_enable), mft_cmd_access_enable
    },
    {
        DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
        sizeof(enter_lowpower_mode), enter_lowpower_mode
    },
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

static struct vcc_desc tianma_lcd_vcc_get_cmds[] =
{
    /* vcc get */
    {DTYPE_VCC_GET, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, 0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_GET, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  0, 0, 0, 0, 0, 0},
};

static struct vcc_desc tianma_lcd_vcc_set_cmds[] =
{
    /* vcc set voltage */
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCDBIAS_NAME, &lcd_bias_vcc,  0, 0, 5400000, 5400000, 0, 0},
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCD_VSP_NAME, &lcd_vsp_vcc,   0, 0, 5400000, 5400000, 0, 0},
    {DTYPE_VCC_SET_VOLTAGE, VCC_LCD_VSN_NAME, &lcd_vsn_vcc,   0, 0, 5400000, 5400000, 0, 0},
};

static struct vcc_desc tianma_lcd_vcc_put_cmds[] =
{
    /* vcc put */
    {DTYPE_VCC_PUT, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, 0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  0, 0, 0, 0, 0, 0},
    {DTYPE_VCC_PUT, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  0, 0, 0, 0, 0, 0},
};

static struct vcc_desc tianma_lcd_vcc_enable_cmds[] =
{
    /* vcc enable */
    {DTYPE_VCC_ENABLE, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, WAIT_TYPE_MS, 12, 0, 0, 0, 0},
    {DTYPE_VCC_ENABLE, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  WAIT_TYPE_MS, 10, 0, 0, 0, 0},
    {DTYPE_VCC_ENABLE, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  WAIT_TYPE_MS, 50, 0, 0, 0, 0},
};

static struct vcc_desc tianma_lcd_vcc_disable_cmds[] =
{
    /* vcc disable */
    {DTYPE_VCC_DISABLE, VCC_LCD_VSN_NAME,   &lcd_vsn_vcc,  WAIT_TYPE_MS, 5, 0, 0, 0, 0},
    {DTYPE_VCC_DISABLE, VCC_LCD_VSP_NAME,   &lcd_vsp_vcc,  WAIT_TYPE_MS, 5, 0, 0, 0, 0},
    {DTYPE_VCC_DISABLE, VCC_LCDBIAS_NAME,   &lcd_bias_vcc, WAIT_TYPE_MS, 1, 0, 0, 0, 0},
};

static struct vcc_desc tianma_lcd_bl_enable_cmds[] =
{
    /* backlight enable */
    {DTYPE_VCC_ENABLE, VCC_BACKLIGHT_NAME, &lcd_bl_vcc,   WAIT_TYPE_MS, 10, 0, 0, 0, 0},
};

static struct vcc_desc tianma_lcd_bl_disable_cmds[] =
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

static struct pinctrl_cmd_desc tianma_lcd_pinctrl_init_cmds[] =
{
    {DTYPE_PINCTRL_GET,       &pctrl, 0},
    {DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
    {DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};
static struct pinctrl_cmd_desc tianma_lcd_pinctrl_normal_cmds[] =
{
    {DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};
static struct pinctrl_cmd_desc tianma_lcd_pinctrl_lowpower_cmds[] =
{
    {DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

#define GPIO_LCD_RESET_NAME      "lcd_reset_gpio"
static u32 gpio_lcd_reset;

static struct gpio_desc tianma_lcd_gpio_get_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_GET, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc tianma_lcd_gpio_request_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc tianma_lcd_gpio_free_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static struct gpio_desc tianma_lcd_gpio_normal_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 120, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
};

static struct gpio_desc tianma_lcd_gpio_lowpower_cmds[] =
{
    /* reset */
    {DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1, GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

static volatile bool g_display_on;
static struct balong_fb_panel_data tm_panel_data;
static struct lcd_tuning_dev* p_tuning_dev = NULL;

static ssize_t tm_lcd_info_show(struct device* dev,
                                struct device_attribute* attr, char* buf)
{
    int ret = 0;
    struct balong_panel_info* pinfo = NULL;

    pinfo = tm_panel_data.panel_info;
    snprintf(buf, PAGE_SIZE, "%s %d x %d\n", g_lcd_name_buf, pinfo->xres, pinfo->yres);
    ret = strlen(buf) + 1;

    return ret;
}

#define TM_ESD_DEBUG	0
#if TM_ESD_DEBUG
/*for esd test*/
static int esd_flag = 0;
static ssize_t tm_show_esd_test(struct device* dev,
                                struct device_attribute* attr, char* buf)
{
    return sprintf(buf, "%d\n", esd_flag);
}
static ssize_t tm_store_esd_test(struct device* dev,
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
static DEVICE_ATTR(esd_test, 0644, tm_show_esd_test, tm_store_esd_test);
#endif
static DEVICE_ATTR(lcd_info, S_IRUGO, tm_lcd_info_show, NULL);

static struct attribute* tm_attrs[] =
{
    &dev_attr_lcd_info.attr,
#if TM_ESD_DEBUG
    /*for esd test*/
    &dev_attr_esd_test.attr,
#endif
    NULL,
};

static struct attribute_group tm_attr_group =
{
    .attrs = tm_attrs,
};

static int tm_sysfs_init(struct platform_device* pdev)
{
    int ret = 0;
    ret = sysfs_create_group(&pdev->dev.kobj, &tm_attr_group);
    if (ret)
    {
        balongfb_loge("create sysfs file failed!\n");
        return ret;
    }
    return 0;
}

static void tm_sysfs_deinit(struct platform_device* pdev)
{
    sysfs_remove_group(&pdev->dev.kobj, &tm_attr_group);
}

static int mipi_tm_panel_on(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;
    struct balong_panel_info* pinfo = NULL;
    struct balong_fb_panel_data* pdata = NULL;

#if defined (CONFIG_HUAWEI_DSM)
    u8* ade_base = NULL;
    u32 int_st0 = 0, int_st1 = 0;
#endif

    BUG_ON(pdev == NULL);

    pr_info("r61318: %s enter succ!\n", __func__);
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
        vcc_cmds_tx(NULL, tianma_lcd_vcc_set_cmds, \
                    ARRAY_SIZE(tianma_lcd_vcc_set_cmds));

        pinfo->lcd_init_step = LCD_INIT_SEND_SEQUENCE;
    }
    else if (pinfo->lcd_init_step == LCD_INIT_SEND_SEQUENCE)
    {
        if (!g_display_on)
        {
			LOG_JANK_D(JLID_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
			g_debug_enable = true;

            /* lcd pinctrl normal */
            pinctrl_cmds_tx(pdev, tianma_lcd_pinctrl_normal_cmds, \
                            ARRAY_SIZE(tianma_lcd_pinctrl_normal_cmds));

            /* lcd gpio request */
            gpio_cmds_tx(NULL, tianma_lcd_gpio_request_cmds, \
                         ARRAY_SIZE(tianma_lcd_gpio_request_cmds));

            /* lcd vcc enable */
            vcc_cmds_tx(NULL, tianma_lcd_vcc_enable_cmds, \
                        ARRAY_SIZE(tianma_lcd_vcc_enable_cmds));

            /* lcd gpio normal */
            gpio_cmds_tx(NULL, tianma_lcd_gpio_normal_cmds, \
                         ARRAY_SIZE(tianma_lcd_gpio_normal_cmds));

            /* lcd display on sequence */
            mipi_dsi_cmds_tx(tm_display_on_cmds, \
                             ARRAY_SIZE(tm_display_on_cmds), balongfd->dsi_base);

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

static int mipi_tm_panel_off(struct platform_device* pdev)
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
        mipi_dsi_cmds_tx(tm_display_off_cmds, \
                         ARRAY_SIZE(tm_display_off_cmds), balongfd->dsi_base);

        /* shutdown d_phy */
        set_MIPIDSI_PHY_RSTZ(0);

        /* lcd gpio lowpower */
        gpio_cmds_tx(NULL, tianma_lcd_gpio_lowpower_cmds, \
                     ARRAY_SIZE(tianma_lcd_gpio_lowpower_cmds));
        /* lcd gpio free */
        gpio_cmds_tx(NULL, tianma_lcd_gpio_free_cmds, \
                     ARRAY_SIZE(tianma_lcd_gpio_free_cmds));

        /* lcd pinctrl lowpower */
        pinctrl_cmds_tx(pdev, tianma_lcd_pinctrl_lowpower_cmds, \
                        ARRAY_SIZE(tianma_lcd_pinctrl_lowpower_cmds));

        /* lcd vcc disable */
        vcc_cmds_tx(NULL, tianma_lcd_vcc_disable_cmds, \
                    ARRAY_SIZE(tianma_lcd_vcc_disable_cmds));

        g_display_on = false;
    }

    pr_info("%s exit succ!\n", __func__);
    return 0;
}

static int mipi_tm_panel_remove(struct platform_device* pdev)
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
    vcc_cmds_tx(pdev, tianma_lcd_vcc_put_cmds, \
                ARRAY_SIZE(tianma_lcd_vcc_put_cmds));

    /* lcd pinctrl lowpower */
    pinctrl_cmds_tx(pdev, tianma_lcd_pinctrl_lowpower_cmds, \
                    ARRAY_SIZE(tianma_lcd_pinctrl_lowpower_cmds));

    tm_sysfs_deinit(pdev);

    return 0;
}

static int tm_set_gamma(struct lcd_tuning_dev* ltd, enum lcd_gamma gamma)
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

static int tm_set_cabc(struct platform_device *pdev, int cabc)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;
    //set cabc mode according to application situation
    char  tm_cabc_ui_on[] =
    {
        0x55,
        0x01 | g_IE_SRE_level,
    };

    char  tm_cabc_vid_on[] =
    {
        0x55,
        0x03 | g_IE_SRE_level,
    };

    char  tm_cabc_off[] =
    {
        0x55,
        0x00 | g_IE_SRE_level,
    };

    struct dsi_cmd_desc tm_cabc_ui_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(tm_cabc_ui_on), tm_cabc_ui_on
        },
    };

    struct dsi_cmd_desc tm_cabc_vid_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(tm_cabc_vid_on), tm_cabc_vid_on
        },
    };
    struct dsi_cmd_desc tm_cabc_off_cmds[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
            sizeof(tm_cabc_off), tm_cabc_off
        },
    };

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    pr_info("%s CABC_MODE =  %d\n", __func__, cabc);

#if TM_CABC_ENABLED
    switch (cabc)
    {
        case CABC_UI:                     // set CABC as UI Mode
        {
            balongfd->g_CABC_mode = 0x01;
            mipi_dsi_cmds_tx(tm_cabc_ui_cmds, ARRAY_SIZE(tm_cabc_ui_cmds), balongfd->dsi_base);
        }
        break;

        case CABC_VID:                   // set CABC as Video Mode
        {
            balongfd->g_CABC_mode = 0x03;
            mipi_dsi_cmds_tx(tm_cabc_vid_cmds, ARRAY_SIZE(tm_cabc_vid_cmds), balongfd->dsi_base);
        }
        break;

        case CABC_OFF:                   // close CABC
        {
            g_CABC_mode = 0x00;
            mipi_dsi_cmds_tx(tm_cabc_off_cmds, ARRAY_SIZE(tm_cabc_off_cmds), balongfd->dsi_base);
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
    .set_gamma = tm_set_gamma,
    .set_cabc = tm_set_cabc,
};

static int mipi_tm_panel_set_backlight(struct platform_device* pdev)
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

    struct dsi_cmd_desc tm_bl_level_adjust[] =
    {
        {
            DTYPE_DCS_WRITE1, 0, 0, WAIT_TYPE_US,
            sizeof(bl_level_adjust), bl_level_adjust
        },
        {
            DTYPE_DCS_WRITE1, 0, 0, WAIT_TYPE_US,
            sizeof(bl_enable), bl_enable
        },
    };

    BUG_ON(pdev == NULL);
    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    level = balongfd->bl_level;

    if (level > 255)
    { level = 255; }

    //backlight may turn off when bl_level is below 6.
    if (level < 6 && level != 0)
    {
        level = 6;
    }

    bl_level_adjust[1] = level;

    mipi_dsi_cmds_tx(tm_bl_level_adjust, ARRAY_SIZE(tm_bl_level_adjust), balongfd->dsi_base);
    if(level == 0)
    {
        vcc_cmds_tx(NULL, tianma_lcd_bl_disable_cmds, \
            ARRAY_SIZE(tianma_lcd_bl_disable_cmds));
    }
    else if(last_level == 0 && level !=0)
    {
        vcc_cmds_tx(NULL, tianma_lcd_bl_enable_cmds, \
            ARRAY_SIZE(tianma_lcd_bl_enable_cmds));
    }
    last_level = level;
    //modified for beta test, it will be modified after beta test.
    balongfb_loge(" set backlight succ ,balongfd->bl_level = %d, level = %d \n", balongfd->bl_level, level);

	if (unlikely(g_debug_enable)) {
		LOG_JANK_D(JLID_KERNEL_LCD_BACKLIGHT_ON, "JL_KERNEL_LCD_BACKLIGHT_ON,%u", level);
		g_debug_enable = false;
	}
    return 0;
}

static int mipi_tm_panel_set_fastboot(struct platform_device* pdev)
{
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    /* lcd gpio request */
    gpio_cmds_tx(NULL, tianma_lcd_gpio_request_cmds, \
                 ARRAY_SIZE(tianma_lcd_gpio_request_cmds));

    /* lcd vcc enable */
    vcc_cmds_tx(NULL, tianma_lcd_vcc_enable_cmds, \
                ARRAY_SIZE(tianma_lcd_vcc_enable_cmds));

    /* lcd backlight enable */
    vcc_cmds_tx(NULL, tianma_lcd_bl_enable_cmds, \
                ARRAY_SIZE(tianma_lcd_bl_enable_cmds));
    g_display_on = true;

    return 0;
}

static int mipi_tm_panel_set_sre(struct platform_device* pdev, int enable, int value)
{
    struct balong_fb_data_type* balongfd = NULL;

    /* As the reg 0x55 is used by CABC and SRE /IE, so use local data to enable each funtion seperately */
    char sbl_level_setting[2] =
    {
        0x55,
        0x00,
    };

    struct dsi_cmd_desc tm_sbl_level_setting[] =
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
    if (enable == 5)	                        // Outdoor mode (check the SRE select box)
    {
        if (0x50 != g_IE_SRE_level )         //  set reg 0x55 only when the value changed
        {
            g_IE_SRE_level = 0x50;           //SRE Medium LEVEL  0x5x,
            sbl_level_setting[1]  = balongfd->g_CABC_mode | g_IE_SRE_level;
            mipi_dsi_cmds_tx(tm_sbl_level_setting, ARRAY_SIZE(tm_sbl_level_setting), balongfd->dsi_base); //send to IC reg
        }
    }
    else                   // 1.Indoor mode (check the SRE select box).  2.Uncheck the SRE select box.
    {
        if (0x50 == g_IE_SRE_level )        //  set reg 0x55 only when the value changed
        {
            g_IE_SRE_level = 0x80;          //IE LOW LEVEL 0x8x,
            sbl_level_setting[1]  = balongfd->g_CABC_mode | g_IE_SRE_level;
            mipi_dsi_cmds_tx(tm_sbl_level_setting, ARRAY_SIZE(tm_sbl_level_setting), balongfd->dsi_base);//send to IC reg
        }
    }
    pr_info("%s enter succ!SRE enable = %d reg0x55_value = 0x%x\n", __func__, enable, balongfd->g_CABC_mode | g_IE_SRE_level);
    return 0;
}

#if LCD_ESD_CHECK_SUPPORT
/*for esd check*/
static int mipi_tm_panel_check_esd(struct platform_device* pdev)
{
    int ret = 0;
    int err = 0;
    unsigned int val = 0;
    struct balong_fb_data_type* balongfd = NULL;

    BUG_ON(pdev == NULL);

    balongfd = (struct balong_fb_data_type*)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

    ret = mipi_dsi_read_reg(balongfd, 0x0a, &val, DTYPE_DCS_READ);
    if ((ret == 0) && ((val & 0xff) != 0x9c))
    {
        err = 1;
        balongfb_loge("check esd reg 0x0a is:0x%x.\n", val);
    }
#if TM_ESD_DEBUG
    if (esd_flag != 0)
    {
        err = 1;
    }
#endif

    return err;
}
#endif //LCD_ESD_CHECK_SUPPORT

static struct balong_panel_info tm_panel_info = {0};
static struct balong_fb_panel_data tm_panel_data =
{
    .panel_info = &tm_panel_info,
    .on = mipi_tm_panel_on,
    .off = mipi_tm_panel_off,
    .remove = mipi_tm_panel_remove,
    .set_backlight = mipi_tm_panel_set_backlight,
    .set_fastboot = mipi_tm_panel_set_fastboot,
    /*Sunlight Readability Enhancement function*/
    .set_sre = mipi_tm_panel_set_sre,
#if LCD_ESD_CHECK_SUPPORT
    /*for esd check*/
    .check_esd = mipi_tm_panel_check_esd,
#endif //LCD_ESD_CHECK_SUPPORT
    .lcd_cabc_mode_store  =  tm_set_cabc,
};

static int __init tm_probe(struct platform_device* pdev)
{
    struct balong_panel_info* pinfo = NULL;
    struct platform_device* reg_pdev = NULL;
    struct lcd_properities lcd_props;
    static int test_flag = 0;
    /*add lcd info*/
    char lcd_name[] = "tm_R61318 5.5' VIDEO TFT";
    int len;
    pr_info("R61318: %s enter succ!!!!\n", __func__);
    /*add lcd info*/
    len = strlen(lcd_name);
    memset(g_lcd_name_buf, 0, sizeof(g_lcd_name_buf));
    strncpy(g_lcd_name_buf, lcd_name, (len > 100 ? 100 : len));
    g_display_on = false;
    pinfo = tm_panel_data.panel_info;

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
    /*Sunlight Readability Enhancement function, enable 1, disable 0*/
    pinfo->sbl_enable = 1;//    //enable sbl
#if LCD_ESD_CHECK_SUPPORT
    /*for esd check enable 1,disable 0*/
    if (NULL != strstr(saved_command_line, "androidboot.swtype=factory"))
    {
        pinfo->esd_enable = 0;
    }
    else
    {
        pinfo->esd_enable = 0;
    }
#endif //LCD_ESD_CHECK_SUPPORT

    pinfo->sbl.bl_max = 0xff;
    pinfo->sbl.cal_a = 0x0f;
    pinfo->sbl.cal_b = 0xd8;
    pinfo->sbl.str_limit = 0x40;

    // porch
    pinfo->ldi.h_back_porch = 32;
    pinfo->ldi.h_front_porch = 169;
    pinfo->ldi.h_pulse_width = 2;
    pinfo->ldi.v_back_porch = 25;
    pinfo->ldi.v_front_porch = 25;
    pinfo->ldi.v_pulse_width = 2;

    // video mode
    pinfo->ldi.hsync_plr = 1;
    pinfo->ldi.vsync_plr = 1;
    pinfo->ldi.pixelclk_plr = 0;
    pinfo->ldi.data_en_plr = 0;

    pinfo->ldi.disp_mode = LDI_DISP_MODE_NOT_3D_FBF;

    // video panel doesn't support partial update,init -1 invalid parameter
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
    gpio_cmds_tx(pdev, tianma_lcd_gpio_get_cmds, \
                 ARRAY_SIZE(tianma_lcd_gpio_get_cmds));

    /* lcd vcc init */
    vcc_cmds_tx(pdev, tianma_lcd_vcc_get_cmds, \
                ARRAY_SIZE(tianma_lcd_vcc_get_cmds));

    /* lcd pinctrl init */
    pinctrl_cmds_tx(pdev, tianma_lcd_pinctrl_init_cmds, \
                    ARRAY_SIZE(tianma_lcd_pinctrl_init_cmds));

    /* alloc panel device data */
    if (platform_device_add_data(pdev, &tm_panel_data,
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

    tm_sysfs_init(pdev);

    pr_info("%s exit succ!\n", __func__);
    return 0;
}
static const struct of_device_id mipi_tm_r61318_match_table[] =
{
    {
        .compatible = "hisilicon,mipi-tm-r61318",
        .data = NULL,
    },
    {},
};

static struct platform_driver this_driver =
{
    .probe = tm_probe,
    .remove = NULL,
    .suspend = NULL,
    .resume = NULL,
    .shutdown = NULL,
    .driver = {
        .name = "mipi_tm_R61318",
        .of_match_table = mipi_tm_r61318_match_table,
    },
};

static int __init mipi_tm_panel_init(void)
{
    int ret = 0;
#ifndef CONFIG_ARM64
    if ( get_lcd_type() != TM_R61318_LCD)
    {
        balongfb_loge("lcd type is not TM_R61318_LCD.\n");
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

module_init(mipi_tm_panel_init);
