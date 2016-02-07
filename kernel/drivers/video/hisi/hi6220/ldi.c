/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/err.h>

#include "balong_fb.h"
#include "ldi_reg.h"
#include "mipi_reg.h"

int ldi_init(struct balong_fb_data_type *balongfd)
{
	u8* ade_base = 0;
	u32 ldi_mask = 0;

	BUG_ON(balongfd == NULL);

	ade_base = balongfd->ade_base;

	set_LDI_HRZ_CTRL0(ade_base, balongfd->panel_info.ldi.h_front_porch,
	                    balongfd->panel_info.ldi.h_back_porch);
	set_LDI_HRZ_CTRL1_hsw(ade_base, balongfd->panel_info.ldi.h_pulse_width);
    set_LDI_VRT_CTRL0(ade_base, balongfd->panel_info.ldi.v_front_porch,
	                balongfd->panel_info.ldi.v_back_porch);

	if (balongfd->panel_info.ldi.v_pulse_width > 15)
		balongfd->panel_info.ldi.v_pulse_width = 15;
	set_LDI_VRT_CTRL1_vsw(ade_base, balongfd->panel_info.ldi.v_pulse_width);
	set_LDI_PLR_CTRL_hsync(ade_base, balongfd->panel_info.ldi.hsync_plr);
	set_LDI_PLR_CTRL_vsync(ade_base, balongfd->panel_info.ldi.vsync_plr);
	set_LDI_PLR_CTRL_pixel_clk(ade_base, balongfd->panel_info.ldi.pixelclk_plr);
	set_LDI_PLR_CTRL_data_en(ade_base, balongfd->panel_info.ldi.data_en_plr);

	set_LDI_DSP_SIZE_size(ade_base, balongfd->panel_info.xres, balongfd->panel_info.yres);

    set_LDI_WORK_MODE_work_mode(ade_base, LDI_WORK);
	set_LDI_WORK_MODE_colorbar_en(ade_base, ADE_DISABLE);
    ldi_mask = LDI_ISR_FRAME_END_INT | LDI_ISR_UNDER_FLOW_INT;

#if ADE_DEBUG_LOG_ENABLE
    if (g_fb_colorbar_mod == 1) {
        set_LDI_WORK_MODE_work_mode(ade_base, LDI_TEST);
	    set_LDI_WORK_MODE_colorbar_en(ade_base, ADE_ENABLE);
    }

    if (g_ade_isr_disable) {
        ldi_mask = 0;
    }
#endif
    set_LDI_INT_MASK(ade_base, ldi_mask);

	set_LDI_CTRL_bgr(ade_base, balongfd->panel_info.bgr_fmt);
	set_LDI_CTRL_bpp(ade_base, balongfd->panel_info.bpp);
	set_LDI_CTRL_disp_mode(ade_base, balongfd->panel_info.ldi.disp_mode);
	set_LDI_CTRL_corlorbar_width(ade_base, 0x3C);
	//|| (PANEL_LDI == balongfd->panel_info.type)
	if (PANEL_MIPI_CMD == balongfd->panel_info.type) {
	    set_LDI_CTRL_data_gate_en(ade_base, ADE_ENABLE);
	    set_LDI_CTRL_self_clr(ade_base, ADE_TRUE);

		/* if panel type is MIPI_CMD,we should enable ldi after pannel init,so it will implement in lid_on() */
		set_LDI_CTRL_ldi_en(ade_base, ADE_DISABLE);
	} else {
		//set_LDI_CTRL_ldi_en(ade_base, ADE_ENABLE);
	}
	set_LDI_INT_CLR(ade_base, 0xFFFFFFFF);

	if (!(balongfd->panel_info.bl_set_type & BL_SET_BY_PWM)) {
        set_reg(ade_base + LDI_DE_SPACE_LOW_REG, 0x1, 1, 1);
	}

	return 0;
}

int ldi_on(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;

	BUG_ON(pdev == NULL);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

    balongfb_logi_display_debugfs("ldi_on: balongfd->panel_info.clk_rate = %d \n", balongfd->panel_info.clk_rate);
	if (clk_set_rate(balongfd->ldi_clk, balongfd->panel_info.clk_rate) != 0) {
        balongfb_loge("set ade_pixel_clk_rate fail \n");
        return -EINVAL;
	}

	/* ldi init */
	ldi_init(balongfd);

	ret = panel_next_on(pdev);

	return ret;
}

int ldi_off(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;
	u8* ade_base = 0;

	BUG_ON(pdev == NULL);

	balongfd = platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

    balongfb_logi_display_debugfs(" enter succ!\n");

	ade_base = balongfd->ade_base;

	balongfd->ade_ldi_on = false;

	ret = panel_next_off(pdev);
	set_LDI_CTRL_ldi_en(ade_base, ADE_DISABLE);

    /* dsi pixel off */
    set_reg(ade_base + LDI_HDMI_DSI_GT_REG, 0x1, 1, 0);

    balongfb_logi_display_debugfs("exit succ!\n");
	return ret;
}

STATIC int ldi_remove(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;

	BUG_ON(pdev == NULL);
	balongfd = platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	balongfb_logi("index=%d, enter!\n", balongfd->index);

	if (!IS_ERR(balongfd->ldi_clk)) {
		clk_put(balongfd->ldi_clk);
	}

	ret = panel_next_remove(pdev);

	balongfb_logi("index=%d, exit!\n", balongfd->index);

	return ret;
}

int ldi_set_backlight(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;

	BUG_ON(pdev == NULL);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	ret = panel_next_set_backlight(pdev);

	return ret;
}

STATIC int ldi_set_timing(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;
	u8* ade_base = 0;

	BUG_ON(pdev == NULL);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	ade_base = balongfd->ade_base;

    set_LDI_HRZ_CTRL0(ade_base, balongfd->panel_info.ldi.h_front_porch, balongfd->panel_info.ldi.h_back_porch);
	set_LDI_HRZ_CTRL1_hsw(ade_base, balongfd->panel_info.ldi.h_pulse_width);
	set_LDI_VRT_CTRL0(ade_base, balongfd->panel_info.ldi.v_front_porch, balongfd->panel_info.ldi.v_back_porch);
	if (balongfd->panel_info.ldi.v_pulse_width > 15)
		balongfd->panel_info.ldi.v_pulse_width = 15;
	set_LDI_VRT_CTRL1_vsw(ade_base, balongfd->panel_info.ldi.v_pulse_width);

	set_LDI_DSP_SIZE_size(ade_base, balongfd->panel_info.xres, balongfd->panel_info.yres);

	set_LDI_PLR_CTRL_hsync(ade_base, balongfd->panel_info.ldi.hsync_plr);
	set_LDI_PLR_CTRL_vsync(ade_base, balongfd->panel_info.ldi.vsync_plr);

	if (clk_set_rate(balongfd->ldi_clk, balongfd->panel_info.clk_rate) != 0) {
        balongfb_loge("set ade_pixel_clk_rate fail \n");
        return -EINVAL;
	}

	ret = panel_next_set_timing(pdev);

    balongfb_logi("%s exit succ!\n",__func__);
	return ret;
}

STATIC int ldi_set_frc(struct platform_device *pdev, int target_fps)
{
	int ret = 0;
	BUG_ON(pdev == NULL);

	ret = panel_next_set_frc(pdev, target_fps);

	return 0;
}

STATIC int ldi_check_esd(struct platform_device *pdev)
{
	BUG_ON(pdev == NULL);

	return panel_next_check_esd(pdev);
}

#if LCD_CHECK_MIPI_SUPPORT
static int ldi_check_mipi_tr(struct platform_device *pdev)
{
    BUG_ON(pdev == NULL);
    return panel_next_check_mipi_tr(pdev);
}

static int ldi_set_inversion_type(struct platform_device *pdev, unsigned int inversion_mode)
{
    BUG_ON(pdev == NULL);
    return panel_next_set_inversion_type(pdev, inversion_mode);
}
#endif

STATIC int ldi_probe(struct platform_device *pdev)
{
	struct balong_fb_data_type *balongfd = NULL;
	struct fb_info *fbi = NULL;
	struct platform_device *balong_fb_dev = NULL;
	struct balong_fb_panel_data *pdata = NULL;
	int ret = 0;

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

    balongfb_logi("%s enter succ!\n",__func__);

	/* balong_fb device */
	balong_fb_dev = platform_device_alloc(DEV_NAME_FB, pdev->id);
	if (!balong_fb_dev) {
		balongfb_loge("failed to balong_fb platform_device_alloc!\n");
		return -ENOMEM;
	}

	/* link to the latest pdev */
	balongfd->pdev = balong_fb_dev;

	/* alloc panel device data */
	if (platform_device_add_data(balong_fb_dev, pdev->dev.platform_data,
		sizeof(struct balong_fb_panel_data))) {
		balongfb_loge("failed to platform_device_add_data!\n");
		platform_device_put(balong_fb_dev);
		return -ENOMEM;
	}

	/* data chain */
	pdata = (struct balong_fb_panel_data *)balong_fb_dev->dev.platform_data;
	pdata->on = ldi_on;
	pdata->off = ldi_off;
	pdata->remove = ldi_remove;
	pdata->set_backlight = ldi_set_backlight;
	pdata->set_timing = ldi_set_timing;
	pdata->set_frc = ldi_set_frc;
	pdata->check_esd = ldi_check_esd;
	#if LCD_CHECK_MIPI_SUPPORT
	pdata->check_mipi_tr = ldi_check_mipi_tr;
	pdata->set_inversion_type = ldi_set_inversion_type;
	#endif
	pdata->next = pdev;

	/* get/set panel info */
	memcpy(&balongfd->panel_info, pdata->panel_info, sizeof(struct balong_panel_info));

	fbi = balongfd->fbi;
	/*fbi->var.pixclock = clk_round_rate(pixel_edc_clk, balongfd->panel_info.clk_rate);*/
	fbi->var.pixclock = balongfd->panel_info.clk_rate;
	fbi->var.left_margin = balongfd->panel_info.ldi.h_back_porch;
	fbi->var.right_margin = balongfd->panel_info.ldi.h_front_porch;
	fbi->var.upper_margin = balongfd->panel_info.ldi.v_back_porch;
	fbi->var.lower_margin = balongfd->panel_info.ldi.v_front_porch;
	fbi->var.hsync_len = balongfd->panel_info.ldi.h_pulse_width;
	fbi->var.vsync_len = balongfd->panel_info.ldi.v_pulse_width;

	/* set driver data */
	platform_set_drvdata(balong_fb_dev, balongfd);
	/* register in balong_fb driver */
	ret = platform_device_add(balong_fb_dev);
	if (ret) {
		platform_device_put(balong_fb_dev);
		balongfb_loge("failed to platform_device_add!\n");
		return ret;
	}

    balongfb_logi("%s exit succ!\n",__func__);
	return ret;
}

static struct platform_driver this_driver = {
	.probe = ldi_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = DEV_NAME_LDI,
		},
};

STATIC int __init ldi_driver_init(void)
{
	int ret = 0;

    balongfb_logi("%s enter succ!\n",__func__);

	ret = platform_driver_register(&this_driver);
	if (ret) {
		balongfb_loge("not able to register the driver, error=%d!\n", ret);
		return ret;
	}

    balongfb_logi("%s exit succ!\n",__func__);

	return ret;
}

module_init(ldi_driver_init);
