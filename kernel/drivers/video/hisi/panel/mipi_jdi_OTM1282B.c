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
#include "../../../huawei/touchscreen/huawei_touchscreen_chips.h"
#include <huawei_platform/log/log_jank.h>


#define PWM_LEVEL 100
static bool g_debug_enable = false;

static int cnt = 0;

/*----------------Power ON Sequence(sleep mode to Normal mode)---------------------*/

/*static char soft_reset[] = {
	0x01,
};*/

static char bl_level_0[] = {
	0x51,
	0x00,
};

static char bl_level[] = {
	0x51,
	0x00 //0xd0, //0xd0
};

static char bl_enable[] = {
	0x53,
	0x24,
};

/*static char bl_mode[] = {
	0x55,
	0x01,
};*/

static char te_enable[] = {
	0x35,
	0x00,
};


static char exit_sleep[] = {
	0x11,
};

/*static char normal_display_on[] = {
	0x13,
};

static char all_pixels_off[] = {
	0x22,
};*/

static char display_on[] = {
	0x29,
};

/*static char get_chip_id[] = {
	0xDA,
};

static char lcd_disp_x[] = {
	0x2A,
	0x00, 0x00,0x02,0xCF
};

static char lcd_disp_y[] = {
	0x2B,
	0x00, 0x00,0x04,0xFF
};*/

/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/
static char display_off[] = {
	0x28,
};

static char enter_sleep[] = {
	0x10,
};

#if 0
//debug begin
static char cmd2_enable[] = {
	0xFF,
	0x12, 0x82, 0x01,
};

static char orise_enable0[] = {
	0x00,
	0x80,
};

static char orise_enable1[] = {
	0xFF,
	0x12, 0X82,
};

static char video_mode_0[] = {
	0x00,
	0x00,
};

static char video_mode_1[] = {
	0x1C,
	0x32,
};

static char video_mode_2[] = {
	0x00,
	0xa0,
};

static char video_mode_3[] = {
	0xc1,
	0xe0,
};

static char sw_pin_lane_sel_00[] ={
	0x00,
	0x90,
};

static char sw_pin_lane_sel_11[] ={
	0xB3,
	0x00, 0x0c, 0x10,
};
//debug end

/*static struct dsi_cmd_desc jdi_get_chip_id_cmds[] = {
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
	sizeof(get_chip_id), get_chip_id}
};*/

static struct dsi_cmd_desc jdi_backlight_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(bl_level), bl_level},
	{DTYPE_DCS_WRITE1, 0, 20, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
};
#endif

static struct dsi_cmd_desc jdi_display_on_cmds[] = {

	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(bl_level), bl_level},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
	{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
		sizeof(te_enable), te_enable},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(display_on), display_on},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(bl_level), bl_level},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},

#if 0

	{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(cmd2_enable), cmd2_enable},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(orise_enable0), orise_enable0},
	{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(orise_enable1), orise_enable1},

        {DTYPE_DCS_WRITE1,0,200,WAIT_TYPE_US,
                sizeof(video_mode_0),video_mode_0},
         {DTYPE_DCS_WRITE1,0,200,WAIT_TYPE_US,
                sizeof(video_mode_1),video_mode_1},
          {DTYPE_DCS_WRITE1,0,200,WAIT_TYPE_US,
                sizeof(video_mode_2),video_mode_2},
         {DTYPE_DCS_WRITE1,0,200,WAIT_TYPE_US,
                sizeof(video_mode_3),video_mode_3},
      {DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(sw_pin_lane_sel_00), sw_pin_lane_sel_00},
	{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(sw_pin_lane_sel_11), sw_pin_lane_sel_11},





	{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_US,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
		sizeof(display_on), display_on},
#endif
};

static struct dsi_cmd_desc jdi_display_off_cmds[] = {
 {DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,

		sizeof(bl_level_0), bl_level_0},
	{DTYPE_DCS_WRITE, 0, 30, WAIT_TYPE_US,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(enter_sleep), enter_sleep}
};




static volatile bool g_display_on;
static struct balong_fb_panel_data jdi_panel_data;


/******************************************************************************/
static struct lcd_tuning_dev *p_tuning_dev = NULL;
static int cabc_mode = 1; /* allow application to set cabc mode to ui mode */

/*y=pow(x,0.6),x=[0,255]*/
/*static u32 square_point_six(u32 x)
{
	unsigned long t = x * x * x;
	int i = 0, j = 255, k = 0;
	unsigned long t0 = 0;
	while (j - i > 1) {
		k = (i + j) / 2;
			t0 = k * k * k * k * k;
		if(t0 < t)
			i = k;
		else if (t0 > t)
			j = k;
		else
			return k;
	}
	return k;
}*/

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

	/* Fix me: Implement it */

	return ret;
}

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

	/* Fix me: Implement it */

	return ret;
}

struct lcd_tuning_ops sp_tuning_ops = {
	.set_gamma = jdi_set_gamma,
	.set_cabc = jdi_set_cabc,
};

static ssize_t jdi_lcd_info_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int ret = 0;
	struct balong_panel_info *pinfo = NULL;

	pinfo = jdi_panel_data.panel_info;
	sprintf(buf, "jdi_OTM1282B 4.7' CMD TFT %d x %d\n",
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


static void jdi_disp_on(struct balong_fb_data_type *balongfd)
{
	struct balong_panel_info *pinfo = NULL;
	//int retval;

	BUG_ON(balongfd == NULL);
	pinfo = &(balongfd->panel_info);
/*
	retval = ts_power_control_notify(TS_RESUME_DEVICE);
	if (retval < 0) {
		balongfb_loge("Failed to send TS_RESUME_DEVICE. Code: %d.\n", retval);
	}
*/
    set_pinctrl_normal(pinfo);
	LCD_GPIO_REQUEST(pinfo);

	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_reset, 1);
	mdelay(10);

	LCD_VCC_ENABLE(pinfo->lcd_bias_vcc);  //vsn vsp enable
	mdelay(12);
	LCD_VCC_ENABLE(pinfo->lcd_vsp_vcc);
	mdelay(10);
	LCD_VCC_ENABLE(pinfo->lcd_vsn_vcc);

	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_power, 1); //vsn vsp enable
	mdelay(10);

	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_reset, 0);
	mdelay(10);

	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_reset, 1);
	mdelay(120);

    LCD_VCC_ENABLE(pinfo->lcd_bl_vcc); //backlight
	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_drv_en, 1);	//backlight
	mdelay(10);

	/* lcd display on sequence */
	mipi_dsi_cmds_tx(jdi_display_on_cmds, ARRAY_SIZE(jdi_display_on_cmds));

	balongfb_logi("exit\n");
}

static void jdi_disp_off(struct balong_fb_data_type *balongfd)
{
	struct balong_panel_info *pinfo = NULL;
	//int retval;

	BUG_ON(balongfd == NULL);
	pinfo = &(balongfd->panel_info);

	mipi_dsi_cmds_tx(jdi_display_off_cmds, ARRAY_SIZE(jdi_display_off_cmds));

	gpio_direction_output(pinfo->gpio_reset, 0);
	mdelay(1);

	LCD_VCC_DISABLE(pinfo->lcd_bias_vcc); //vsn vsp
	LCD_VCC_DISABLE(pinfo->lcd_vsn_vcc);
	LCD_VCC_DISABLE(pinfo->lcd_vsp_vcc);
	gpio_direction_output(pinfo->gpio_power, 0);
	mdelay(1);

	LCD_GPIO_FREE(pinfo);
	set_pinctrl_lowpower(pinfo);

	LCD_VCC_DISABLE(pinfo->lcd_bl_vcc); //backlight
/*
	retval = ts_power_control_notify(TS_SUSPEND_DEVICE);
	if (retval < 0) {
		balongfb_loge("Failed to send TS_SUSPEND_DEVICE. Code: %d.\n", retval);
	}
*/
}

static int mipi_jdi_panel_on(struct platform_device *pdev)
{
	struct balong_fb_data_type *balongfd = NULL;
	struct balong_panel_info *pinfo = NULL;

	BUG_ON(pdev == NULL);
	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	pr_info("%s enter succ!\n",__func__);

	pinfo = &(balongfd->panel_info);
	pr_info("%s,pinfo->lcd_init_step = %d!\n",__func__,pinfo->lcd_init_step);

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
	    /* set vcc current and voltage */
	    //LCD_VCC_SET_CURRENT(pinfo->lcd_bl_vcc, 20000, 20000); /* bl_vcc must be 20mA */
	    LCD_VCC_SET_VOLTAGE(pinfo->lcd_bias_vcc, 5000000, 5000000); /* 5.0V */
	    LCD_VCC_SET_VOLTAGE(pinfo->lcd_vsn_vcc, 5000000, 5000000); /* 5.0V */
	    LCD_VCC_SET_VOLTAGE(pinfo->lcd_vsp_vcc, 5000000, 5000000); /* 5.0V */
		pinfo->lcd_init_step = LCD_INIT_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_SEND_SEQUENCE) {
		if (!g_display_on) {
			LOG_JANK_D(JLID_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
			g_debug_enable = true;
			/* lcd display on */
			jdi_disp_on(balongfd);
			g_display_on = true;
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

	BUG_ON(pdev == NULL);
    pr_info("%s enter succ!\n",__func__);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	if (g_display_on) {
		LOG_JANK_D(JLID_KERNEL_LCD_POWER_OFF, "%s", "JL_KERNEL_LCD_POWER_OFF");

		g_display_on = false;
		/* lcd display off */
		jdi_disp_off(balongfd);
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

	LCD_VCC_PUT(&(balongfd->panel_info));
    set_pinctrl_lowpower(&(balongfd->panel_info));
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
	if (level > 255) {
		level = 255;
    }
    //backlight may turn off when bl_level is below 6.
	if (level < 6 && level != 0) {
		level = 6;
	}
	bl_level_adjust[1] = level;
	mipi_dsi_cmds_tx(jdi_bl_level_adjust, ARRAY_SIZE(jdi_bl_level_adjust));

	balongfb_logi_backlight_debugfs(" set backlight succ level = %d \n",level);

	if (unlikely(g_debug_enable)) {
		LOG_JANK_D(JLID_KERNEL_LCD_BACKLIGHT_ON, "JL_KERNEL_LCD_BACKLIGHT_ON,%u", level);
		g_debug_enable = false;
	}

	return 0;

}

#if 0
static int mipi_jdi_panel_set_fastboot(struct platform_device *pdev)
{
	struct balong_fb_data_type *balongfd = NULL;
	struct balong_panel_info *pinfo;

	BUG_ON(pdev == NULL);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	pinfo = &(balongfd->panel_info);

	LCD_VCC_ENABLE(pinfo->lcd_bl_vcc);
	LCD_VCC_ENABLE(pinfo->lcd_bias_vcc);
	LCD_VCC_ENABLE(pinfo->lcd_vsn_vcc);
	LCD_VCC_ENABLE(pinfo->lcd_vsp_vcc);
    set_pinctrl_normal(pinfo);
	LCD_GPIO_REQUEST(pinfo);

	return 0;
}
#endif

static int mipi_jdi_panel_set_cabc(struct platform_device *pdev, int value)
{
	return 0;
}

static struct balong_panel_info jdi_panel_info = {0};
static struct balong_fb_panel_data jdi_panel_data = {
	.panel_info = &jdi_panel_info,
	.on = mipi_jdi_panel_on,
	.off = mipi_jdi_panel_off,
	.remove = mipi_jdi_panel_remove,
	.set_backlight = mipi_jdi_panel_set_backlight,
	.set_fastboot = NULL,
	.set_cabc = mipi_jdi_panel_set_cabc,
};

static int __init jdi_probe(struct platform_device *pdev)
{
	struct balong_panel_info *pinfo = NULL;
	struct platform_device *reg_pdev = NULL;
	struct lcd_properities lcd_props;

	pr_info("%s enter succ!!!!\n",__func__);

	g_display_on = false;
	pinfo = jdi_panel_data.panel_info;

	/* init lcd panel info */
	pinfo->xres = 720;
	pinfo->yres = 1280;
	pinfo->width  = 58;  //mm
	pinfo->height = 103; //mm
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

	pinfo->ldi.h_back_porch = 43;//16;
	pinfo->ldi.h_front_porch = 80;//16;
	pinfo->ldi.h_pulse_width = 57;//2;
	pinfo->ldi.v_back_porch = 12;//2;
	pinfo->ldi.v_front_porch = 14;//2;
	pinfo->ldi.v_pulse_width = 2;

	pinfo->ldi.hsync_plr = 1;
	pinfo->ldi.vsync_plr = 0;
	pinfo->ldi.pixelclk_plr = 1;
	pinfo->ldi.data_en_plr = 0;

	pinfo->ldi.disp_mode = LDI_DISP_MODE_NOT_3D_FBF;

	/* init at DTS */
	pinfo->mipi.lane_nums = DSI_4_LANES;
	pinfo->mipi.color_mode = DSI_24BITS_1;
	pinfo->mipi.vc = 0;
	pinfo->frame_rate = 60;

	/* lcd vcc */
	LCD_VCC_GET(pdev, pinfo);

	/* lcd iomux */
	set_pinctrl_init(pdev, pinfo);

	/* lcd resource */
	LCD_RESOURCE(pdev, pinfo);

    pdev->id = 1;

	/* alloc panel device data */
	if (platform_device_add_data(pdev, &jdi_panel_data,
		sizeof(struct balong_fb_panel_data))) {
		balongfb_loge("platform_device_add_data failed!\n");
		platform_device_put(pdev);
		return -ENOMEM;
	}

	reg_pdev = balong_fb_add_device(pdev);
    if(NULL == reg_pdev){
        return -1;
    }

	/* for cabc */
	lcd_props.type = TFT;
	lcd_props.default_gamma = GAMMA25;

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
static const struct of_device_id mipi_jdi_otm1282b_match_table[] = {
    {
        .compatible ="hisilicon,mipi-jdi-otm1282b",
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
		.name = "mipi_jdi_OTM1282B",
                .of_match_table = mipi_jdi_otm1282b_match_table,
	},
};

static int __init mipi_jdi_panel_init(void)
{
	int ret = 0;

    if( get_lcd_type() != JDI_1282B_LCD)
    {
        return ret;
    }
	ret = platform_driver_register(&this_driver);
	if (ret) {
		balongfb_loge("not able to register the driver\n");
		return ret;
	}

	return ret;
}

module_init(mipi_jdi_panel_init);
