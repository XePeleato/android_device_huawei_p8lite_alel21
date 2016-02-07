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
	 return (curtime->tv_usec >= lasttime->tv_usec) ?
		 curtime->tv_usec - lasttime->tv_usec :
		 1000000 - (lasttime->tv_usec - curtime->tv_usec);

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

	hisi_overlay_on(hisifd);

	hisifb_vsync_resume(hisifd);

	return ret;
}

int hisifb_ctrl_off(struct hisi_fb_data_type *hisifd)
{
	struct hisi_fb_panel_data *pdata = NULL;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

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

	if (pdata->esd_handle) {
		ret = pdata->esd_handle(hisifd->pdev);
	}

	return ret;
}

int hisifb_ctrl_sbl(struct fb_info *info, void __user *argp)
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

	if (!hisifd->panel_info.sbl_support) {
		HISI_FB_ERR("fb%d, SBL not supported!", hisifd->index);
		return 0;
	}

	ret = copy_from_user(&val, argp, sizeof(val));
	if (ret) {
		HISI_FB_ERR("hisifb_ctrl_sbl ioctl failed!ret=%d.", ret);
		return ret;
	}

	sbl_lsensor_value = val & 0xffff;
	sbl_level = (val >> 16) & 0xff;
	sbl_enable = (val >> 24) & 0x1;

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

	if (!is_mipi_cmd_panel(hisifd) && pdata->sbl_ctrl) {
		hisifb_activate_vsync(hisifd);
		hisifd->sbl_enable = (sbl_enable > 0) ? 1 : 0;
		pdata->sbl_ctrl(hisifd->pdev, hisifd->sbl_enable);
		hisifb_deactivate_vsync(hisifd);
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
		HISI_FB_ERR("hisifb_ctrl_sbl ioctl failed!ret=%d.", ret);
		return ret;
	}

	ret = set_dss_clk_rate(hisifd);

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

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("panel power off!\n");
		return -EPERM;
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

	return ret;
}
