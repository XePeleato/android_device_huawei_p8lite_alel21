/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
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
#include "hisi_overlay_utils.h"

extern int g_enable_dirty_region_updt;

void set_reg(char __iomem *addr, uint32_t val, uint8_t bw, uint8_t bs)
{
	uint32_t mask = (1UL << bw) - 1UL;
	uint32_t tmp = 0;

	tmp = inp32(addr);
	tmp &= ~(mask << bs);

	outp32(addr, tmp | ((val & mask) << bs));
}

uint32_t set_bits32(uint32_t old_val, uint32_t val, uint8_t bw, uint8_t bs)
{
	uint32_t mask = (1UL << bw) - 1UL;
	uint32_t tmp = 0;

	tmp = old_val;
	tmp &= ~(mask << bs);

	return (tmp | ((val & mask) << bs));
}

void hisifb_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *addr, uint32_t val, uint8_t bw, uint8_t bs)
{
	set_reg(addr, val, bw, bs);
}

uint32_t hisifb_line_length(int index, uint32_t xres, int bpp)
{
	return ALIGN_UP(xres * bpp, DMA_STRIDE_ALIGN);
}

 void hisifb_get_timestamp(struct timeval *tv)
{
	struct timespec ts;

	ktime_get_ts(&ts);
	tv->tv_sec = ts.tv_sec;
	tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;

	//struct timeval timestamp;
	//do_gettimeofday(&timestamp);
	//timestamp = ktime_to_timeval(ktime_get());
}

int hisifb_sbl_pow_i(int base,int exp)
{
	int result = 1;
	int i = 0;

	for (i = 1; i <= exp; ++i) {
		result *= base;
	}

	return result;
}

 uint32_t hisifb_timestamp_diff(struct timeval *lasttime, struct timeval *curtime)
 {
	 return (uint32_t)((curtime->tv_usec >= lasttime->tv_usec) ?
		 curtime->tv_usec - lasttime->tv_usec :
		 1000000 - (lasttime->tv_usec - curtime->tv_usec));

	//return (curtime->tv_sec - lasttime->tv_sec) * 1000 +
	//	(curtime->tv_usec - lasttime->tv_usec) /1000;
 }

int hisifb_ctrl_fastboot(struct hisi_fb_data_type *hisifd)
{
	struct hisi_fb_panel_data *pdata = NULL;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (pdata->set_fastboot) {
		ret = pdata->set_fastboot(hisifd->pdev);
	}

	if (is_mipi_video_panel(hisifd)) {
		hisi_overlay_set_fastboot(hisifd);
		hisi_overlay_on(hisifd, false);
	} else {
		hisi_overlay_on(hisifd, true);
	}

	hisifb_vsync_resume(hisifd);

	return ret;
}

int hisifb_ctrl_on(struct hisi_fb_data_type *hisifd)
{
	struct hisi_fb_panel_data *pdata = NULL;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (pdata->on) {
		ret = pdata->on(hisifd->pdev);
	}

	hisi_overlay_on(hisifd, true);

	hisifb_vsync_resume(hisifd);

	if (hisifd->panel_info.esd_enable) {
		if (is_mipi_cmd_panel(hisifd)) {
			hisifd->esd_hrtimer_enable = false;
			hrtimer_restart(&hisifd->esd_hrtimer);
		}
	}

	return ret;
}

int hisifb_ctrl_off(struct hisi_fb_data_type *hisifd)
{
	struct hisi_fb_panel_data *pdata = NULL;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (hisifd->panel_info.esd_enable) {
		if (is_mipi_cmd_panel(hisifd)) {
			hisifd->esd_hrtimer_enable = false;
			hrtimer_cancel(&hisifd->esd_hrtimer);
		}
	}

	hisifb_vsync_suspend(hisifd);

	hisi_overlay_off(hisifd);

	if (pdata->off) {
		ret = pdata->off(hisifd->pdev);
	}

	return ret;
}

int hisifb_ctrl_frc(struct hisi_fb_data_type *hisifd, int fps)
{
	struct hisi_fb_panel_data *pdata = NULL;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (pdata->frc_handle) {
		ret = pdata->frc_handle(hisifd->pdev, fps);
	}

	return ret;
}

int hisifb_ctrl_esd(struct hisi_fb_data_type *hisifd)
{
	struct hisi_fb_panel_data *pdata = NULL;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_DEBUG("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata->esd_handle) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->esd_handle(hisifd->pdev);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

int hisifb_ctrl_sbl(struct fb_info *info, int value)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	int val = 0;
	int sbl_lsensor_value = 0;
	int sbl_level = 0;
	int sbl_enable = 0;
	dss_sbl_t sbl;

	int assertiveness = 0;
	int assertiveness_base = 0;
	int assertiveness_exp = 0;
	int calib_a = 60;
	int calib_b = 95;
	int calib_c = 5;
	int calib_d = 1;
	int tmp = 0;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_DEBUG("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (!hisifd->panel_info.sbl_support) {
		HISI_FB_ERR("fb%d, SBL not supported!", hisifd->index);
		goto err_out;
	}

	memset(&sbl, 0 , sizeof(dss_sbl_t));
	val = value;
	sbl_lsensor_value = val & 0xffff;
	sbl_level = (val >> 16) & 0xff;
	sbl_enable = (val >> 24) & 0x1;
	hisifd->sbl_lsensor_value = sbl_lsensor_value;
	hisifd->sbl_level = sbl_level;
	hisifd->sbl_enable = sbl_enable;

	assertiveness_base = 2;
	assertiveness_exp = 8 * (sbl_level - 128) / 255;

	if (assertiveness_exp < 0) {
		assertiveness_exp *= (-1);
		assertiveness = hisifb_sbl_pow_i(assertiveness_base, assertiveness_exp);
		calib_a = calib_a / assertiveness;
		calib_b = calib_b;
		calib_c = calib_c / assertiveness;
		calib_d = calib_d / assertiveness;
	} else {
		assertiveness = hisifb_sbl_pow_i(assertiveness_base, assertiveness_exp);
		calib_a = calib_a * assertiveness;
		calib_b = calib_b;
		calib_c = calib_c * assertiveness;
		calib_d = calib_d * assertiveness;
	}

	tmp = hisifd->bl_level & 0xff;
	sbl.sbl_backlight_l = set_bits32(sbl.sbl_backlight_l, tmp, 8, 0);
	tmp = (hisifd->bl_level >> 8) & 0xff;
	sbl.sbl_backlight_h= set_bits32(sbl.sbl_backlight_h, tmp, 8, 0);
	tmp = sbl_lsensor_value & 0xff;
	sbl.sbl_ambient_light_l = set_bits32(sbl.sbl_ambient_light_l, tmp, 8, 0);
	tmp= (sbl_lsensor_value >> 8) & 0xff;
	sbl.sbl_ambient_light_h= set_bits32(sbl.sbl_ambient_light_h, tmp, 8, 0);
	tmp = calib_a & 0xff;
	sbl.sbl_calibration_a_l = set_bits32(sbl.sbl_calibration_a_l, tmp, 8, 0);
	tmp= (calib_a >> 8) & 0xff;
	sbl.sbl_calibration_a_h= set_bits32(sbl.sbl_calibration_a_h, tmp, 8, 0);
	tmp = calib_b & 0xff;
	sbl.sbl_calibration_b_l = set_bits32(sbl.sbl_calibration_b_l, tmp, 8, 0);
	tmp= (calib_b >> 8) & 0xff;
	sbl.sbl_calibration_b_h= set_bits32(sbl.sbl_calibration_b_h, tmp, 8, 0);
	tmp = calib_c & 0xff;
	sbl.sbl_calibration_c_l = set_bits32(sbl.sbl_calibration_c_l, tmp, 8, 0);
	tmp= (calib_c >> 8) & 0xff;
	sbl.sbl_calibration_c_h= set_bits32(sbl.sbl_calibration_c_h, tmp, 8, 0);
	tmp = calib_d & 0xff;
	sbl.sbl_calibration_d_l = set_bits32(sbl.sbl_calibration_d_l, tmp, 8, 0);
	tmp= (calib_d >> 8) & 0xff;
	sbl.sbl_calibration_d_h = set_bits32(sbl.sbl_calibration_d_h, tmp, 8, 0);
	tmp = sbl_enable & 0x1;
	sbl.sbl_enable = set_bits32(sbl.sbl_enable, tmp, 1, 0);

	memcpy(&(hisifd->sbl), &sbl, sizeof(dss_sbl_t));


	if (is_mipi_cmd_panel(hisifd) && pdata->sbl_ctrl) {
		hisifb_activate_vsync(hisifd);
		hisifd->sbl_enable = (sbl_enable > 0) ? 1 : 0;
		pdata->sbl_ctrl(hisifd->pdev, hisifd->sbl_enable);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

int hisifb_line_length_get(struct fb_info *info, void __user *argp)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	hisi_fb_line_length_t line_length;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	ret = copy_from_user(&line_length, argp, sizeof(hisi_fb_line_length_t));
	if (ret) {
		HISI_FB_ERR("copy_from_user failed!ret=%d.", ret);
		return ret;
	}

	line_length.line_length_out = hisifb_line_length(line_length.index, line_length.xres, line_length.bpp);

	ret = copy_to_user(argp, &line_length, sizeof(hisi_fb_line_length_t));
	if (ret) {
		HISI_FB_ERR("copy_to_user failed!ret=%d.\n", ret);
		return -EINVAL;
	}

	return 0;
}

int hisifb_ctrl_dss_clk_rate(struct fb_info *info, void __user *argp)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	dss_clk_rate_t dss_clk_rate;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	ret = copy_from_user(&dss_clk_rate, argp, sizeof(dss_clk_rate_t));
	if (ret) {
		HISI_FB_ERR("copy_from_user failed!ret=%d.", ret);
		return ret;
	}

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		ret = -EPERM;
		goto err_out;
	}


	if (dss_clk_rate.dss_pri_clk_rate > 0) {
		hisifd->dss_clk_rate.dss_pri_clk_rate = dss_clk_rate.dss_pri_clk_rate;
	}

	if (dss_clk_rate.dss_aux_clk_rate > 0) {
		hisifd->dss_clk_rate.dss_aux_clk_rate = dss_clk_rate.dss_aux_clk_rate;
	}

	if (dss_clk_rate.dss_pclk_clk_rate > 0) {
		hisifd->dss_clk_rate.dss_pclk_clk_rate = dss_clk_rate.dss_pclk_clk_rate;
	}

	ret = set_dss_clk_rate(hisifd);

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

int hisifb_ctrl_pixclk_rate(struct fb_info *info, void __user *argp)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	struct fb_var_screeninfo var;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	pdata = dev_get_platdata(&hisifd->pdev->dev);
	if ((!pdata) || (!pdata->set_pixclk_rate)) {
		HISI_FB_ERR("no panel operation detected!\n");
		return -ENODEV;
	}

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		ret = -EPERM;
		goto err_out;
	}

	ret = copy_from_user(&var, argp, sizeof(var));
	if (ret) {
		HISI_FB_ERR("copy from user failed!\n");
		return ret;
	}

	HISI_FB_INFO("\t fb%d set pixclk_rate:\n", hisifd->index);
	HISI_FB_INFO("\t  xres  |  yres  | right_margin | left_margin | lower_margin | upper_margin | hsync_len | vsync_len | pixclock \n");
	HISI_FB_INFO("\t--------+--------+--------------+-------------+--------------+"
		"--------------+-----------+-----------+----------\n");
	HISI_FB_INFO("\t %6d | %6d | %12d | %11d | %12d | %12d | %9d | %9d | %8d \n",
		var.xres, var.yres, var.right_margin, var.left_margin, var.lower_margin, var.upper_margin,
		var.hsync_len, var.vsync_len, var.pixclock);

	memcpy(&info->var, &var, sizeof(var));

	hisifd->panel_info.xres = var.xres;
	hisifd->panel_info.yres = var.yres;
	hisifd->panel_info.pxl_clk_rate = (var.pixclock == 0) ? hisifd->panel_info.pxl_clk_rate : var.pixclock;
	hisifd->panel_info.ldi.h_front_porch = var.right_margin;
	hisifd->panel_info.ldi.h_back_porch = var.left_margin;
	hisifd->panel_info.ldi.h_pulse_width = var.hsync_len;
	hisifd->panel_info.ldi.v_front_porch = var.lower_margin;
	hisifd->panel_info.ldi.v_back_porch = var.upper_margin;
	hisifd->panel_info.ldi.v_pulse_width = var.vsync_len;

	hisifb_activate_vsync(hisifd);
	ret = pdata->set_pixclk_rate(hisifd->pdev);
	if (ret != 0) {
		HISI_FB_ERR("failed to set_pixclk_rate!\n");
	}
	hisifb_deactivate_vsync(hisifd);

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_model_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (pdata->lcd_model_show) {
		ret = pdata->lcd_model_show(hisifd->pdev, buf);
	}

	return ret;
}

static ssize_t hisifb_lcd_cabc_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (pdata->lcd_cabc_mode_show) {
		ret = pdata->lcd_cabc_mode_show(hisifd->pdev, buf);
	}

	return ret;
}

static ssize_t hisifb_lcd_cabc_mode_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata->lcd_cabc_mode_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_cabc_mode_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_check_reg_show(struct device *dev,
          struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata && pdata->lcd_check_reg) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_check_reg(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	} else {
		HISI_FB_ERR("lcd_check_reg is NULL\n");
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_frame_count_show(struct device *dev,
          struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%u\n", g_frame_count);
}

static ssize_t hisifb_lcd_mipi_detect_show(struct device *dev,
          struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata && pdata->lcd_mipi_detect) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_mipi_detect(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	} else {
		HISI_FB_ERR("lcd_mipi_detect is NULL\n");
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_mipi_dsi_bit_clk_upt_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata->mipi_dsi_bit_clk_upt_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->mipi_dsi_bit_clk_upt_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_mipi_dsi_bit_clk_upt_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata->mipi_dsi_bit_clk_upt_show) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->mipi_dsi_bit_clk_upt_show(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_hkadc_debug_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (pdata->lcd_hkadc_debug_show) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_hkadc_debug_show(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	}

	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_hkadc_debug_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (pdata->lcd_hkadc_debug_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_hkadc_debug_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_gram_check_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata->lcd_gram_check_show) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_gram_check_show(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_gram_check_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		ret = -EINVAL;
		goto err_out;
	}

	if (pdata->lcd_gram_check_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_gram_check_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_color_temperature_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		ret = -EINVAL;
		goto err_out;
	}

	if (pdata->lcd_color_temperature_show) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_color_temperature_show(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_color_temperature_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		ret = -EINVAL;
		goto err_out;
	}

	if (pdata->lcd_color_temperature_store) {
		if (pdata->panel_info->dirty_region_updt_support)
		{			
			g_enable_dirty_region_updt = 0;
		}
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_color_temperature_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return count;
}

static ssize_t hisifb_led_rg_lcd_color_temperature_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		ret = -EINVAL;
		goto err_out;
	}

	if (pdata->led_rg_lcd_color_temperature_show) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->led_rg_lcd_color_temperature_show(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_led_rg_lcd_color_temperature_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		ret = -EINVAL;
		goto err_out;
	}

	if (pdata->led_rg_lcd_color_temperature_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->led_rg_lcd_color_temperature_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return count;
}

static ssize_t hisifb_lcd_comform_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (pdata->lcd_comform_mode_show) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_comform_mode_show(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	}

	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_comform_mode_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (pdata->lcd_comform_mode_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_comform_mode_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

	up(&hisifd->blank_sem);
	return count;
}

static ssize_t hisifb_lcd_starlight_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (pdata->lcd_starlight_mode_show) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_starlight_mode_show(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	}

	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_starlight_mode_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (pdata->lcd_starlight_mode_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_starlight_mode_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

	up(&hisifd->blank_sem);
	return count;
}
static ssize_t hisifb_lcd_voltage_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (pdata->lcd_voltage_enable_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_voltage_enable_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_sbl_ctrl_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	ret = snprintf(buf, PAGE_SIZE, "sbl_lsensor_value=%d, sbl_level=%d, sbl_enable=%d\n",
		hisifd->sbl_lsensor_value, hisifd->sbl_level, hisifd->sbl_enable);

	return ret;
}

static ssize_t hisifb_sbl_ctrl_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	int val = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	val = (int)simple_strtoul(buf, NULL, 0);
	if (hisifd->sbl_ctrl_fnc) {
		ret = hisifd->sbl_ctrl_fnc(fbi, val);
	}

	down(&hisifd->blank_sem);

	if (pdata->amoled_hbm_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->amoled_hbm_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

	up(&hisifd->blank_sem);

	return count;
}

static ssize_t hisifb_lcd_bist_check(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	char lcd_bist_check_result[512] = {0};

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata->lcd_bist_check) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->lcd_bist_check(hisifd->pdev, lcd_bist_check_result);
		hisifb_deactivate_vsync(hisifd);
	}

	ret = snprintf(buf, PAGE_SIZE, "%s", lcd_bist_check_result);
	HISI_FB_INFO("LCD bist check result : %s\n", lcd_bist_check_result);

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_lcd_func_switch_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	pinfo = &(hisifd->panel_info);

	ret = snprintf(buf, PAGE_SIZE,
		"sbl=%d\ncolor_temperature=%d\ndsi_bit_clk_upt=%d\ndirty_region_updt=%d\n",
		pinfo->sbl_support, pinfo->color_temperature_support,
		pinfo->dsi_bit_clk_upt_support, pinfo->dirty_region_updt_support);

	return ret;
}

static ssize_t hisifb_lcd_func_switch_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	struct hisi_panel_info *pinfo = NULL;
	char command[60] = {0};

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	pinfo = &(hisifd->panel_info);

	if (!sscanf(buf, "%s", command)) {/* [false alarm] */
		HISI_FB_INFO("bad command(%s)\n", command);
		return count;
	}

	down(&hisifd->blank_sem);

	hisifb_activate_vsync(hisifd);

	if (!strncmp("sbl:", command, strlen("sbl:"))) {
		if('0' == command[strlen("sbl:")]) {
			pinfo->sbl_support = 0;
			HISI_FB_INFO("sbl disable\n");
		} else {
			pinfo->sbl_support = 1;
			HISI_FB_INFO("sbl enable\n");
		}
	}

	if (!strncmp("color_temperature:", command, strlen("color_temperature:"))) {
		if('0' == command[strlen("color_temperature:")]) {
			pinfo->color_temperature_support = 0;
			dpe_init_ct_cscValue();
			HISI_FB_INFO("color_temperature disable\n");
		} else {
			pinfo->color_temperature_support = 1;
			HISI_FB_INFO("color_temperature enable\n");
		}
	}

	if (!strncmp("dsi_bit_clk_upt:", command, strlen("dsi_bit_clk_upt:"))) {
		if('0' == command[strlen("dsi_bit_clk_upt:")]) {
			pinfo->dsi_bit_clk_upt_support = 0;
			HISI_FB_INFO("dsi_bit_clk_upt disable\n");
		} else {
			pinfo->dsi_bit_clk_upt_support = 1;
			HISI_FB_INFO("dsi_bit_clk_upt enable\n");
		}
	}

	if (!strncmp("dirty_region_upt:", command, strlen("dirty_region_upt:"))) {
		if('0' == command[strlen("dirty_region_upt:")]) {
			pinfo->dirty_region_updt_support = 0;
			HISI_FB_INFO("dirty_region_upt disable\n");
		} else {
			pinfo->dirty_region_updt_support = 1;
			HISI_FB_INFO("dirty_region_upt enable\n");
		}
	}

	hisifb_deactivate_vsync(hisifd);

	up(&hisifd->blank_sem);

	return count;
}

static ssize_t hisifb_lcd_region_limit_show(struct device* dev,
        struct device_attribute* attr, char* buf)
{
    struct fb_info *fbi = NULL;
    struct hisi_fb_data_type *hisifd = NULL;

    fbi = dev_get_drvdata(dev);
    BUG_ON(fbi == NULL);
    hisifd = (struct hisi_fb_data_type *)fbi->par;
    BUG_ON(hisifd == NULL);

    snprintf(buf, PAGE_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                hisifd->panel_info.dirty_region_info.left_align, hisifd->panel_info.dirty_region_info.right_align,
                hisifd->panel_info.dirty_region_info.top_align,hisifd->panel_info.dirty_region_info.bottom_align,
                hisifd->panel_info.dirty_region_info.w_align, hisifd->panel_info.dirty_region_info.h_align,
                hisifd->panel_info.dirty_region_info.w_min, hisifd->panel_info.dirty_region_info.h_min,
                hisifd->panel_info.dirty_region_info.top_start,hisifd->panel_info.dirty_region_info.bottom_start);

    return strlen(buf) + 1;
}

static ssize_t hisifb_lcd_region_limit_store(struct device* dev,
        struct device_attribute* attr, const char* buf, size_t count)
{
    int i = 0;
    int limit_value[11];
    char *token, *cur;
    struct fb_info *fbi = NULL;
    struct hisi_fb_data_type *hisifd = NULL;

    fbi = dev_get_drvdata(dev);
    BUG_ON(fbi == NULL);
    hisifd = (struct hisi_fb_data_type *)fbi->par;
    BUG_ON(hisifd == NULL);

    cur = buf;
    while (token = strsep(&cur, ",")) {
        limit_value[i++] = (int)simple_strtol(token, NULL, 0);
    }

    hisifd->panel_info.dirty_region_info.left_align   = limit_value[0];
    hisifd->panel_info.dirty_region_info.right_align  = limit_value[1];
    hisifd->panel_info.dirty_region_info.top_align    = limit_value[2];
    hisifd->panel_info.dirty_region_info.bottom_align = limit_value[3];
    hisifd->panel_info.dirty_region_info.w_align      = limit_value[4];
    hisifd->panel_info.dirty_region_info.h_align      = limit_value[5];
    hisifd->panel_info.dirty_region_info.w_min        = limit_value[6];
    hisifd->panel_info.dirty_region_info.h_min        = limit_value[7];
    hisifd->panel_info.dirty_region_info.top_start    = limit_value[8];
    hisifd->panel_info.dirty_region_info.bottom_start = limit_value[9];

    return count;
}

static ssize_t hisifb_amoled_pcd_errflag_check(struct device* dev,
        struct device_attribute* attr, char* buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (pdata->amoled_pcd_errflag_check) {
		ret = pdata->amoled_pcd_errflag_check(hisifd->pdev, buf);
	}

	return ret;
}

static ssize_t hisifb_amoled_acl_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata->amoled_acl_show) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->amoled_acl_show(hisifd->pdev, buf);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t hisifb_amoled_acl_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	BUG_ON(dev == NULL);
	fbi = dev_get_drvdata(dev);
	BUG_ON(fbi == NULL);
	hisifd = (struct hisi_fb_data_type *)fbi->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		ret = -EINVAL;
		goto err_out;
	}

	if (pdata->lcd_gram_check_store) {
		hisifb_activate_vsync(hisifd);
		ret = pdata->amoled_acl_store(hisifd->pdev, buf, count);
		hisifb_deactivate_vsync(hisifd);
	}

err_out:
	up(&hisifd->blank_sem);

	return ret;
}

static ssize_t lcd_status_check_show(struct device* dev,
        struct device_attribute* attr, char* buf)
{
	int return_value = 1;
	return snprintf(buf, PAGE_SIZE, "%d\n", return_value);
}

/*lint -e665*/
static DEVICE_ATTR(lcd_model, 0644, hisifb_lcd_model_show, NULL);
static DEVICE_ATTR(lcd_cabc_mode, S_IRUGO|S_IWUSR, hisifb_lcd_cabc_mode_show, hisifb_lcd_cabc_mode_store);
static DEVICE_ATTR(lcd_check_reg, S_IRUGO, hisifb_lcd_check_reg_show, NULL);
static DEVICE_ATTR(lcd_mipi_detect, S_IRUGO, hisifb_lcd_mipi_detect_show, NULL);
static DEVICE_ATTR(frame_count, S_IRUGO, hisifb_frame_count_show, NULL);
static DEVICE_ATTR(mipi_dsi_bit_clk_upt, S_IRUGO|S_IWUSR, hisifb_mipi_dsi_bit_clk_upt_show, hisifb_mipi_dsi_bit_clk_upt_store);
static DEVICE_ATTR(lcd_hkadc, S_IRUGO|S_IWUSR, hisifb_lcd_hkadc_debug_show, hisifb_lcd_hkadc_debug_store);
static DEVICE_ATTR(lcd_checksum, S_IRUGO|S_IWUSR, hisifb_lcd_gram_check_show, hisifb_lcd_gram_check_store);
static DEVICE_ATTR(lcd_color_temperature, S_IRUGO|S_IWUSR, hisifb_lcd_color_temperature_show, hisifb_lcd_color_temperature_store);
static DEVICE_ATTR(led_rg_lcd_color_temperature, S_IRUGO|S_IWUSR, hisifb_led_rg_lcd_color_temperature_show, hisifb_led_rg_lcd_color_temperature_store);
static DEVICE_ATTR(lcd_comform_mode, S_IRUGO|S_IWUSR, hisifb_lcd_comform_mode_show, hisifb_lcd_comform_mode_store);
static DEVICE_ATTR(lcd_starlight_mode, S_IRUGO|S_IWUSR, hisifb_lcd_starlight_mode_show, hisifb_lcd_starlight_mode_store);
static DEVICE_ATTR(lcd_voltage_enable, S_IWUSR, NULL, hisifb_lcd_voltage_enable_store);
static DEVICE_ATTR(sbl_ctrl, S_IRUGO|S_IWUSR, hisifb_sbl_ctrl_show, hisifb_sbl_ctrl_store);
static DEVICE_ATTR(lcd_bist_check, S_IRUGO, hisifb_lcd_bist_check, NULL);
static DEVICE_ATTR(lcd_func_switch, S_IRUGO|S_IWUSR, hisifb_lcd_func_switch_show, hisifb_lcd_func_switch_store);
static DEVICE_ATTR(lcd_region_limit, S_IRUGO|S_IWUSR, hisifb_lcd_region_limit_show,hisifb_lcd_region_limit_store);
static DEVICE_ATTR(amoled_pcd_errflag_check, 0644, hisifb_amoled_pcd_errflag_check, NULL);
static DEVICE_ATTR(lcd_status, S_IRUGO, lcd_status_check_show, NULL);
static DEVICE_ATTR(amoled_acl, S_IRUGO|S_IWUSR, hisifb_amoled_acl_show, hisifb_amoled_acl_store);

/*lint +e665*/

void hisifb_sysfs_attrs_add(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (hisifd->sysfs_attrs_append_fnc) {
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_model.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_cabc_mode.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_check_reg.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_mipi_detect.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_frame_count.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_mipi_dsi_bit_clk_upt.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_hkadc.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_checksum.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_color_temperature.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_led_rg_lcd_color_temperature.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_comform_mode.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_starlight_mode.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_voltage_enable.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_sbl_ctrl.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_bist_check.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_func_switch.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_region_limit.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_amoled_pcd_errflag_check.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_lcd_status.attr);
		hisifd->sysfs_attrs_append_fnc(hisifd, &dev_attr_amoled_acl.attr);
    }

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
}
