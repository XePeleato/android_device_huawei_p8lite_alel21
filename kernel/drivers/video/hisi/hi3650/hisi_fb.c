 /* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
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
 */

#include "hisi_fb.h"
#include "hisi_overlay_utils.h"
#include <huawei_platform/log/log_jank.h>


#if 0
/dev/graphics/fb0
/sys/class/graphics/fb0
/sys/devices/platform/
#endif

static int hisi_fb_resource_initialized;
static struct platform_device *pdev_list[HISI_FB_MAX_DEV_LIST] = {0};

static int pdev_list_cnt;
struct fb_info *fbi_list[HISI_FB_MAX_FBI_LIST] = {0};
static int fbi_list_index;

struct hisi_fb_data_type *hisifd_list[HISI_FB_MAX_FBI_LIST] = {0};
static int hisifd_list_index;

#define HISI_FB_ION_CLIENT_NAME	"hisi_fb_ion"

uint32_t g_dts_resouce_ready = 0;

static char __iomem *hisifd_dss_base;
static char __iomem *hisifd_crgperi_base;

static uint32_t hisifd_irq_pdp;
static uint32_t hisifd_irq_sdp;
static uint32_t hisifd_irq_adp;
static uint32_t hisifd_irq_dsi0;
static uint32_t hisifd_irq_dsi1;

#define MAX_DPE_NUM	(3)
static struct regulator_bulk_data g_dpe_regulator[MAX_DPE_NUM] =
	{{0}, {0}, {0}};

static const char *g_dss_clk_name;
static const char *g_dss_pxl0_clk_name;
static const char *g_dss_pxl1_clk_name;
static const char *g_dss_dphy0_clk_name;
static const char *g_dss_dphy1_clk_name;

int g_primary_lcd_xres = 0;
int g_primary_lcd_yres = 0;


/*
** for debug, S_IRUGO
** /sys/module/hisifb/parameters
*/
unsigned hisi_fb_msg_level = 7;
module_param_named(debug_msg_level, hisi_fb_msg_level, int, 0644);
MODULE_PARM_DESC(debug_msg_level, "hisi fb msg level");

int g_debug_ldi_underflow = 0;
module_param_named(debug_ldi_underflow, g_debug_ldi_underflow, int, 0644);
MODULE_PARM_DESC(debug_ldi_underflow, "hisi ldi_underflow debug");

int g_debug_mmu_error = 0;
module_param_named(debug_mmu_error, g_debug_mmu_error, int, 0644);
MODULE_PARM_DESC(debug_mmu_error, "hisi mmu error");

int g_debug_ovl_osd = 0;
module_param_named(debug_ovl_osd, g_debug_ovl_osd, int, 0644);
MODULE_PARM_DESC(debug_ovl_osd, "hisi overlay osd debug");

int g_debug_online_vsync = 0;
module_param_named(debug_online_vsync, g_debug_online_vsync, int, 0644);
MODULE_PARM_DESC(debug_online_vsync, "hisi online vsync debug");

int g_debug_ovl_online_composer = 0;
module_param_named(debug_ovl_online_composer, g_debug_ovl_online_composer, int, 0644);
MODULE_PARM_DESC(debug_ovl_online_composer, "hisi overlay online composer debug");

int g_debug_ovl_offline_composer = 0;
module_param_named(debug_ovl_offline_composer, g_debug_ovl_offline_composer, int, 0644);
MODULE_PARM_DESC(debug_ovl_offline_composer, "hisi overlay offline composer debug");

int g_debug_ovl_optimized = 0;
module_param_named(debug_ovl_optimized, g_debug_ovl_optimized, int, 0644);
MODULE_PARM_DESC(debug_ovl_optimized, "hisi overlay optimized debug");

int g_enable_ovl_optimized = 0;
module_param_named(enable_ovl_optimized, g_enable_ovl_optimized, int, 0644);
MODULE_PARM_DESC(enable_ovl_optimized, "hisi overlay optimized enable");

int g_scf_stretch_threshold = SCF_STRETCH_THRESHOLD;
module_param_named(scf_stretch_threshold, g_scf_stretch_threshold, int, 0644);
MODULE_PARM_DESC(scf_stretch_threshold, "hisi scf stretch threshold");

int g_enable_dirty_region_updt = 1;
module_param_named(enable_dirty_region_updt, g_enable_dirty_region_updt, int, 0644);
MODULE_PARM_DESC(enable_dirty_region_updt, "hisi dss dirty_region_updt enable");

int g_debug_dirty_region_updt = 0;
module_param_named(debug_dirty_region_updt, g_debug_dirty_region_updt, int, 0644);
MODULE_PARM_DESC(debug_dirty_region_updt, "hisi dss dirty_region_updt debug");

int g_debug_mipi_dphy_lp = 1;
module_param_named(debug_mipi_dphy_lp, g_debug_mipi_dphy_lp, int, 0644);
MODULE_PARM_DESC(debug_mipi_dphy_lp, "hisi mipi dphy lowpower debug");

int g_debug_dss_clk_lp = 1;
module_param_named(debug_dss_clk_lp, g_debug_dss_clk_lp, int, 0644);
MODULE_PARM_DESC(debug_dss_clk_lp, "hisi dss clock lowpower debug");


/******************************************************************************
** FUNCTIONS PROTOTYPES
*/
static int hisi_fb_register(struct hisi_fb_data_type *hisifd);
static int hisi_fb_blank_sub(int blank_mode, struct fb_info *info);

static int hisi_fb_open(struct fb_info *info, int user);
static int hisi_fb_release(struct fb_info *info, int user);
static int hisi_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
static int hisi_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info);
static int hisi_fb_set_par(struct fb_info *info);
static int hisi_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg);
static int hisi_fb_mmap(struct fb_info *info, struct vm_area_struct * vma);

static int hisi_fb_suspend_sub(struct hisi_fb_data_type *hisifd);
static int hisi_fb_resume_sub(struct hisi_fb_data_type *hisifd);
#ifdef CONFIG_HAS_EARLYSUSPEND
static void hisifb_early_suspend(struct early_suspend *h);
static void hisifb_early_resume(struct early_suspend *h);
#endif

#ifdef CONFIG_PM_RUNTIME
static void hisifb_pm_runtime_get(struct hisi_fb_data_type *hisifd);
static void hisifb_pm_runtime_put(struct hisi_fb_data_type *hisifd);
static void hisifb_pm_runtime_register(struct platform_device *pdev);
static void hisifb_pm_runtime_unregister(struct platform_device *pdev);
#endif

static unsigned long hisifb_alloc_fb_buffer(struct hisi_fb_data_type *hisifd);
static void hisifb_free_fb_buffer(struct hisi_fb_data_type *hisifd);


/*******************************************************************************
**
*/

void hisi_fb1_power_ctrl(bool power_full)
{
	struct fb_info *info = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	info = fbi_list[1];
	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, power_full(%d) +.\n", hisifd->index, power_full);

	down(&hisifd->blank_sem);
	if (hisifd->is_hdmi_power_full == power_full){
		up(&hisifd->blank_sem);
		return;
	}

	if (power_full == true) {
		hisifd->is_hdmi_power_full = true;
	} else {
		hisifd->is_hdmi_power_full = false;

		if(hisifd->buf_sync_suspend)
			hisifd->buf_sync_suspend(hisifd);
	}
	up(&hisifd->blank_sem);
}
EXPORT_SYMBOL(hisi_fb1_power_ctrl);

int hisi_fb1_blank(int blank_mode)
{
	int ret = 0;
	struct fb_info *info = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	info = fbi_list[1];
	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, blank_mode(%d) +.\n", hisifd->index, blank_mode);

	ret = hisi_fb_blank_sub(blank_mode, info);
	if (ret != 0) {
		HISI_FB_ERR("fb%d, blank_mode(%d) failed!\n", hisifd->index, blank_mode);
	}

	HISI_FB_INFO("fb%d, blank_mode(%d) -.\n", hisifd->index, blank_mode);

	return ret;
}
EXPORT_SYMBOL(hisi_fb1_blank);

struct platform_device *hisi_fb_add_device(struct platform_device *pdev)
{
	struct hisi_fb_panel_data *pdata = NULL;
	struct platform_device *this_dev = NULL;
	struct fb_info *fbi = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	uint32_t type = 0;
	uint32_t id = 0;

	BUG_ON(pdev == NULL);
	pdata = dev_get_platdata(&pdev->dev);
	BUG_ON(pdata == NULL);

	if (fbi_list_index >= HISI_FB_MAX_FBI_LIST) {
		HISI_FB_ERR("no more framebuffer info list!\n");
		return NULL;
	}

	id = pdev->id;
	type = pdata->panel_info->type;

	/* alloc panel device data */
	this_dev = hisi_fb_device_alloc(pdata, type, id);
	if (!this_dev) {
		HISI_FB_ERR("failed to hisi_fb_device_alloc!\n");
		return NULL;
	}

	/* alloc framebuffer info + par data */
	fbi = framebuffer_alloc(sizeof(struct hisi_fb_data_type), NULL);
	if (fbi == NULL) {
		HISI_FB_ERR("can't alloc framebuffer info data!\n");
		platform_device_put(this_dev);
		return NULL;
	}

	hisifd = (struct hisi_fb_data_type *)fbi->par;
	memset(hisifd, 0, sizeof(struct hisi_fb_data_type));
	hisifd->fbi = fbi;

	hisifd->fb_imgType = HISI_FB_PIXEL_FORMAT_BGRA_8888;
	hisifd->index = fbi_list_index;
	hisifd->dss_base = hisifd_dss_base;
	hisifd->crgperi_base = hisifd_crgperi_base;

	hisifd->dss_clk_name = g_dss_clk_name;
	hisifd->dss_pxl0_clk_name = g_dss_pxl0_clk_name;
	hisifd->dss_pxl1_clk_name = g_dss_pxl1_clk_name;
	hisifd->dss_dphy0_clk_name = g_dss_dphy0_clk_name;
	hisifd->dss_dphy1_clk_name = g_dss_dphy1_clk_name;

	hisifd->dsi0_irq = hisifd_irq_dsi0;
	hisifd->dsi1_irq = hisifd_irq_dsi1;
	if (hisifd->index == PRIMARY_PANEL_IDX) {
		hisifd->fb_num = HISI_FB0_NUM;
		hisifd->dpe_irq = hisifd_irq_pdp;
		hisifd->dpe_regulator = &(g_dpe_regulator[0]);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		hisifd->fb_num = HISI_FB1_NUM;
		hisifd->dpe_irq = hisifd_irq_sdp;
		hisifd->dpe_regulator = &(g_dpe_regulator[1]);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		hisifd->fb_num = HISI_FB2_NUM;
		hisifd->dpe_irq = hisifd_irq_adp;
		hisifd->dpe_regulator = &(g_dpe_regulator[2]);
	} else {
		HISI_FB_ERR("fb%d not support now!\n", hisifd->index);
		return NULL;
	}

	/* link to the latest pdev */
	hisifd->pdev = this_dev;

	hisifd_list[hisifd_list_index++] = hisifd;
	fbi_list[fbi_list_index++] = fbi;

	 /* get/set panel info */
	memcpy(&hisifd->panel_info, pdata->panel_info, sizeof(struct hisi_panel_info));

	/* set driver data */
	platform_set_drvdata(this_dev, hisifd);

	if (platform_device_add(this_dev)) {
		HISI_FB_ERR("failed to platform_device_add!\n");
		platform_device_put(this_dev);
		framebuffer_release(fbi);
		fbi_list_index--;
		return NULL;
	}

	return this_dev;
}

static int hisifb_lcd_check_reg(struct fb_info *info, void __user *argp)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	int lcd_check = LCD_CHECK_REG_FAIL;

	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata && pdata->lcd_check_reg) {
		hisifb_activate_vsync(hisifd);
		lcd_check = pdata->lcd_check_reg(hisifd->pdev);
		hisifb_deactivate_vsync(hisifd);
	} else {
		HISI_FB_ERR("lcd_check_reg is NULL\n");
	}

err_out:
	if (copy_to_user(argp, &lcd_check, sizeof(lcd_check))) {
		HISI_FB_ERR("copy to user fail");
		return -EFAULT;
	}

	return 0;
}

static int hisifb_lcd_mipi_detect(struct fb_info *info, void __user *argp)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	int mipi_detect = LCD_MIPI_DETECT_FAIL;

	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel power off!\n", hisifd->index);
		goto err_out;
	}

	if (pdata && pdata->lcd_mipi_detect) {
		hisifb_activate_vsync(hisifd);
		mipi_detect = pdata->lcd_mipi_detect(hisifd->pdev);
		hisifb_deactivate_vsync(hisifd);
	} else {
		HISI_FB_ERR("lcd_mipi_detect is NULL\n");
	}

err_out:
	if (copy_to_user(argp, &mipi_detect, sizeof(mipi_detect))) {
		HISI_FB_ERR("copy to user fail");
		return -EFAULT;
	}

	return 0;
}

static int hisi_fb_blank_sub(int blank_mode, struct fb_info *info)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;
	int curr_pwr_state = 0;

	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	down(&hisifd->blank_sem);
	switch (blank_mode) {
	case FB_BLANK_UNBLANK:
		if (!hisifd->panel_power_on) {
			ret = hisifd->on_fnc(hisifd);
			if (ret == 0) {
				hisifd->panel_power_on = true;
			}
		}
		break;

	case FB_BLANK_VSYNC_SUSPEND:
	case FB_BLANK_HSYNC_SUSPEND:
	case FB_BLANK_NORMAL:
	case FB_BLANK_POWERDOWN:
	default:
		if (hisifd->panel_power_on) {
			curr_pwr_state = hisifd->panel_power_on;
			hisifd->panel_power_on = false;

			if (hisifd->bl_cancel) {
				hisifd->bl_cancel(hisifd);
			}

			ret = hisifd->off_fnc(hisifd);
			if (ret)
				hisifd->panel_power_on = curr_pwr_state;

			if(hisifd->buf_sync_suspend)
				hisifd->buf_sync_suspend(hisifd);
		}
		break;
	}
	up(&hisifd->blank_sem);

	return ret;
}

#ifdef CONFIG_FASTBOOT_DISP_ENABLE
static int fastboot_set_needed;

static bool hisi_fb_set_fastboot_needed(struct fb_info *info)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	if (fastboot_set_needed) {
		hisifb_ctrl_fastboot(hisifd);

		hisifd->panel_power_on = true;
		if (info->screen_base && (info->fix.smem_len > 0))
			memset(info->screen_base, 0x0, info->fix.smem_len);
		fastboot_set_needed = 0;
		return true;
	}

	return false;
}
#endif

static int hisi_fb_open_sub(struct fb_info *info)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;
	bool needed = false;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	if (hisifd->set_fastboot_fnc) {
		needed = hisifd->set_fastboot_fnc(info);
	}

	if (!needed) {
		ret = hisi_fb_blank_sub(FB_BLANK_UNBLANK, info);
		if (ret != 0) {
			HISI_FB_ERR("can't turn on display!\n");
			return ret;
		}
	}

	return 0;
}

static int hisi_fb_release_sub(struct fb_info *info)
{
	int ret = 0;

	BUG_ON(info == NULL);

	ret = hisi_fb_blank_sub(FB_BLANK_POWERDOWN, info);
	if (ret != 0) {
		HISI_FB_ERR("can't turn off display!\n");
		return ret;
	}

	return 0;
}


/*******************************************************************************
**
*/
static int hisi_fb_blank(int blank_mode, struct fb_info *info)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, blank_mode(%d) +.\n", hisifd->index, blank_mode);

#if 0
	if (blank_mode == FB_BLANK_POWERDOWN) {
		struct fb_event event;
		event.info = info;
		event.data = &blank_mode;
		fb_notifier_call_chain(FB_EVENT_BLANK, &event);
	}
#endif

	ret = hisi_fb_blank_sub(blank_mode, info);
	if (ret != 0) {
		HISI_FB_ERR("fb%d, blank_mode(%d) failed!\n", hisifd->index, blank_mode);
		return ret;
	}

	HISI_FB_INFO("fb%d, blank_mode(%d) -.\n", hisifd->index, blank_mode);

	return 0;
}

static int hisi_fb_open(struct fb_info *info, int user)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

//	if (hisifd->pm_runtime_get)
//		hisifd->pm_runtime_get(hisifd);

	if (!hisifd->ref_cnt) {
		HISI_FB_DEBUG("fb%d, +!\n", hisifd->index);
		if (hisifd->open_sub_fnc) {
            LOG_JANK_D(JLID_KERNEL_LCD_OPEN, "%s", "JL_KERNEL_LCD_OPEN 3650");
			ret = hisifd->open_sub_fnc(info);
		}
		HISI_FB_DEBUG("fb%d, -!\n", hisifd->index);
	}

	hisifd->ref_cnt++;

	return ret;
}

static int hisi_fb_release(struct fb_info *info, int user)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	if (!hisifd->ref_cnt) {
		HISI_FB_INFO("try to close unopened fb%d!\n", hisifd->index);
		return -EINVAL;
	}

	hisifd->ref_cnt--;

	if (!hisifd->ref_cnt) {
		HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
		if (hisifd->release_sub_fnc) {
			ret = hisifd->release_sub_fnc(info);
		}
		HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
	}

//	if (hisifd->pm_runtime_put)
//		hisifd->pm_runtime_put(hisifd);

	return ret;
}

static int hisi_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	if (var->rotate != FB_ROTATE_UR) {
		HISI_FB_DEBUG("error rotate %d!\n", var->rotate);
		return -EINVAL;
	}

	if (var->grayscale != info->var.grayscale) {
		HISI_FB_DEBUG("error grayscale %d!\n", var->grayscale);
		return -EINVAL;
	}

	if ((var->xres_virtual <= 0) || (var->yres_virtual <= 0)) {
		HISI_FB_ERR("xres_virtual=%d yres_virtual=%d out of range!",
			var->xres_virtual, var->yres_virtual);
		return -EINVAL;
	}

#if 0
	if (info->fix.smem_len <
		(hisifb_line_length(hisifd->index, var->xres_virtual, (var->bits_per_pixel >> 3)) *
		var->yres_virtual)) {
		HISI_FB_ERR("fb%d smem_len=%d is out of range!\n", hisifd->index, info->fix.smem_len);
		return -EINVAL;
	}
#endif

	if ((var->xres == 0) || (var->yres == 0)) {
		HISI_FB_ERR("xres=%d, yres=%d is invalid!\n", var->xres, var->yres);
		return -EINVAL;
	}

	if (var->xoffset > (var->xres_virtual - var->xres)) {
		HISI_FB_ERR("xoffset=%d(xres_virtual=%d, xres=%d) out of range!\n",
			var->xoffset, var->xres_virtual, var->xres);
		return -EINVAL;
	}

	if (var->yoffset > (var->yres_virtual - var->yres)) {
		HISI_FB_ERR("yoffset=%d(yres_virtual=%d, yres=%d) out of range!\n",
			var->yoffset, var->yres_virtual, var->yres);
		return -EINVAL;
	}

	return 0;
}

static int hisi_fb_set_par(struct fb_info *info)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct fb_var_screeninfo *var = NULL;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	var = &info->var;

	hisifd->fbi->fix.line_length = hisifb_line_length(hisifd->index, var->xres_virtual,
		var->bits_per_pixel >> 3);

	return 0;
}

static int hisi_fb_pan_display(struct fb_var_screeninfo *var,
	struct fb_info *info)
{
	struct hisi_fb_data_type *hisifd = NULL;

	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	if (!hisifd->panel_power_on)
		return -EPERM;

	if (var->xoffset > (info->var.xres_virtual - info->var.xres))
		return -EINVAL;

	if (var->yoffset > (info->var.yres_virtual - info->var.yres))
		return -EINVAL;

	if (info->fix.xpanstep)
		info->var.xoffset =
		(var->xoffset / info->fix.xpanstep) * info->fix.xpanstep;

	if (info->fix.ypanstep)
		info->var.yoffset =
		(var->yoffset / info->fix.ypanstep) * info->fix.ypanstep;

	if (hisifd->pan_display_fnc)
		hisifd->pan_display_fnc(hisifd);
	else
		HISI_FB_ERR("fb%d pan_display_fnc not set!\n", hisifd->index);

	if (hisifd->bl_update) {
		hisifd->bl_update(hisifd);
	}

	return 0;
}

static int hisifb_dirty_region_updt_set(struct fb_info *info, void __user *argp)
{
	int enable = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	if (g_enable_dirty_region_updt
		&& hisifd->panel_info.dirty_region_updt_support
		&& !hisifd->sbl_enable) {
		enable = 1;
	}

	hisifd->dirty_region_updt_enable = enable;

	if (copy_to_user(argp, &enable, sizeof(enable))) {
		HISI_FB_ERR("copy to user fail");
		return -EFAULT;
	}

	return 0;
}

static int hisi_fb_switch_resolution_process(struct hisi_fb_data_type * hisifd)
{
	struct hisi_panel_info * pInfo = NULL;
	struct fb_info *fbi = NULL;
	struct fb_var_screeninfo *var = NULL;
	struct fb_fix_screeninfo *fix = NULL;
	struct hisi_fb_panel_data *pdata = NULL;

	if (NULL == hisifd) {
		HISI_FB_ERR("hisi_fb_switch_resolution_process:NULL pointer!\n");
		return -1;
	}

	fbi = hisifd->fbi;
	fix = &fbi->fix;
	var = &fbi->var;

	pInfo = &hisifd->panel_info;

	pdata = (struct hisi_fb_panel_data *)hisifd->pdev->dev.platform_data;
	if ((!pdata) || (!pdata->set_disp_resolution)) {
		HISI_FB_ERR("no panel operation detected!\n");
		return 0;
	}
	if (pdata->set_disp_resolution(hisifd->pdev) != 0) {
		HISI_FB_ERR("set_disp_resolution error!\n");
		return 0;
	}

	fbi->var.pixclock = hisifd->panel_info.pxl_clk_rate;
	/*fbi->var.pixclock = clk_round_rate(hisifd->dpe_clk, hisifd->panel_info.pxl_clk_rate);*/
	fbi->var.left_margin = hisifd->panel_info.ldi.h_back_porch;
	fbi->var.right_margin = hisifd->panel_info.ldi.h_front_porch;
	fbi->var.upper_margin = hisifd->panel_info.ldi.v_back_porch;
	fbi->var.lower_margin = hisifd->panel_info.ldi.v_front_porch;
	fbi->var.hsync_len = hisifd->panel_info.ldi.h_pulse_width;
	fbi->var.vsync_len = hisifd->panel_info.ldi.v_pulse_width;

	var->height = hisifd->panel_info.height;  /* height of picture in mm */
	var->width = hisifd->panel_info.width;  /* width of picture in mm */
	var->xres = hisifd->panel_info.xres;
	var->yres = hisifd->panel_info.yres;
	var->xres_virtual = hisifd->panel_info.xres;
	var->yres_virtual = hisifd->panel_info.yres * hisifd->fb_num;

	fix->line_length = hisifb_line_length(hisifd->index, var->xres_virtual, var->bits_per_pixel >> 3);
	fix->smem_len = roundup(fix->line_length * var->yres_virtual, PAGE_SIZE);
	if (hisifd->index == PRIMARY_PANEL_IDX) {
		g_primary_lcd_xres = hisifd->panel_info.xres;
		g_primary_lcd_yres = hisifd->panel_info.yres;
	}

	hisifb_free_fb_buffer(hisifd);
	if (fix->smem_len > 0) {
		if (!hisifb_alloc_fb_buffer(hisifd)) {
			HISI_FB_ERR("hisifb_alloc_buffer failed!\n");
			return -1;
		}
	}

	return 0;
}

static int hisifb_resolution_switch(struct fb_info *info, void __user *argp)
{
	int ret  = 0;
	int flag = 0;
	struct hisi_fb_data_type * hisifd = NULL;

	BUG_ON(NULL == info);

	if (copy_from_user(&flag, argp, sizeof(flag))){
		HISI_FB_ERR("copy from user failed\n");
		return -1;
	}

	hisifd = (struct hisi_fb_data_type*)info->par;

	BUG_ON(NULL == hisifd);

	if (flag >= DISPLAY_LOW_POWER_LEVEL_MAX ) {
		HISI_FB_ERR("Invalid level of Multi Resolution\n");
		return -1;
	}

	if (hisifd->index != 0){
		HISI_FB_ERR("Invalid FB device\n");
		return -1;
	}

	if (hisifd->switch_res_flag == flag) {
		return 1;
	}
	hisifd->switch_res_flag = flag;
	ret = hisi_fb_blank_sub(FB_BLANK_POWERDOWN, info);

	if (ret != 0) {
		HISI_FB_ERR("can't turn off display!\n");
		return -1;
	}

	ret = hisi_fb_switch_resolution_process(hisifd);

	return ret;
}

static int hisifb_resolution_update(struct fb_info *info, void __user *argp)
{
	int flag = 0;
	int ret = 0;
	struct hisi_fb_data_type * hisifd;
	BUG_ON(NULL == info);
	if (copy_from_user(&flag, argp, sizeof(flag))){
		HISI_FB_ERR("copy from user failed\n");
		return -EFAULT;
	}
	hisifd = (struct hisi_fb_data_type*)info->par;
	BUG_ON(hisifd == NULL);

	if (hisifd->switch_res_flag == flag) {
		ret = hisi_fb_blank_sub(FB_BLANK_UNBLANK, info);
	}

	if (ret != 0) {
		HISI_FB_ERR("can't turn on display!\n");
	}
	return ret;
}

static int hisi_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
	int ret = -ENOSYS;
	struct hisi_fb_data_type *hisifd = NULL;
	void __user *argp = (void __user *)arg;

	BUG_ON(info == NULL);
	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);

	switch (cmd) {
	case HISIFB_VSYNC_CTRL:
		if (hisifd->vsync_ctrl_fnc) {
			ret = hisifd->vsync_ctrl_fnc(info, argp);
		}
		break;

	case HISIFB_SBL_CTRL:
		if (hisifd->sbl_ctrl_fnc) {
			ret = hisifd->sbl_ctrl_fnc(info, argp);
		}
		break;

	case HISIFB_DSS_CLK_RATE_SET:
		ret = hisifb_ctrl_dss_clk_rate(info, argp);
		break;

	case HISIFB_PIXCLK_RATE_SET:
		down(&hisifd->blank_sem);
		ret = hisifb_ctrl_pixclk_rate(info, argp);
		up(&hisifd->blank_sem);
		break;

	case HISIFB_QUERY_UPDT_STATE:
		ret = hisifb_dirty_region_updt_set(info, argp);
		break;
	case HISIFB_CHECK_MIPI_TR:
		ret = hisifb_lcd_check_reg(info, argp);
		break;
	case HISIFB_MIPI_DETECT:
		ret = hisifb_lcd_mipi_detect(info, argp);
		break;
	case HISIFB_RESOLUTION_SWITCH:
		ret = hisifb_resolution_switch(info, argp);
		break;
	case HISIFB_RESOLUTION_UPDATE:
		ret = hisifb_resolution_update(info, argp);
		break;

	default:
		if (hisifd->ov_ioctl_handler)
			ret = hisifd->ov_ioctl_handler(hisifd, cmd, argp);
		break;
	}

	if (ret == -ENOSYS)
		HISI_FB_ERR("unsupported ioctl (%x)\n", cmd);

	return ret;
}

static int hisi_fb_mmap(struct fb_info *info, struct vm_area_struct * vma)
{
#ifndef CONFIG_HISI_FB_HEAP_CARVEOUT_USED
	struct hisi_fb_data_type *hisifd = NULL;
	struct sg_table *table = NULL;
	struct scatterlist *sg = NULL;
	struct page *page = NULL;
	unsigned long remainder = 0;
	unsigned long len = 0;
	unsigned long addr = 0;
	unsigned long offset = 0;
	int i = 0;
	int ret = 0;

	hisifd = (struct hisi_fb_data_type *)info->par;
	BUG_ON(hisifd == NULL);
	table = ion_sg_table(hisifd->ion_client, hisifd->ion_handle);
	BUG_ON(table == NULL);

	addr = vma->vm_start;
	offset = vma->vm_pgoff * PAGE_SIZE;

	for_each_sg(table->sgl, sg, table->nents, i) {
		page = sg_page(sg);
		remainder = vma->vm_end - addr;
		len = sg_dma_len(sg);

		if (offset >= sg_dma_len(sg)) {
			offset -= sg_dma_len(sg);
			continue;
		} else if (offset) {
			page += offset / PAGE_SIZE;
			len = sg_dma_len(sg) - offset;
			offset = 0;
		}
		len = min(len, remainder);
		ret = remap_pfn_range(vma, addr, page_to_pfn(page), len,
			vma->vm_page_prot);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, failed to remap_pfn_range! ret=%d\n", hisifd->index, ret);
		}

		addr += len;
		if (addr >= vma->vm_end)
			return 0;
	}
#endif

	return 0;
}

static unsigned long hisifb_alloc_fb_buffer(struct hisi_fb_data_type *hisifd)
{
	struct fb_info *fbi = NULL;
	struct ion_client *client = NULL;
	struct ion_handle *handle = NULL;
	size_t buf_len = 0;
	unsigned long buf_addr = 0;

	BUG_ON(hisifd == NULL);
	fbi = hisifd->fbi;
	BUG_ON(fbi == NULL);

	client = hisifd->ion_client;
	if (IS_ERR_OR_NULL(client)) {
		HISI_FB_ERR("failed to create ion client!\n");
		goto err_return;
	}

	buf_len = fbi->fix.smem_len;

#ifdef CONFIG_HISI_FB_HEAP_CARVEOUT_USED
	handle = ion_alloc(client, buf_len, PAGE_SIZE, ION_HEAP(ION_GRALLOC_HEAP_ID), 0);
#else
	handle = ion_alloc(client, buf_len, PAGE_SIZE, ION_HEAP(ION_SYSTEM_HEAP_ID), 0);
#endif
	if (IS_ERR_OR_NULL(handle)) {
		HISI_FB_ERR("failed to ion_alloc!\n");
		goto err_return;
	}

	fbi->screen_base = ion_map_kernel(client, handle);
	if (!fbi->screen_base) {
		HISI_FB_ERR("failed to ion_map_kernel!\n");
		goto err_ion_map;
	}

#ifdef CONFIG_HISI_FB_HEAP_CARVEOUT_USED
	if (ion_phys(client, handle, &buf_addr, &buf_len) < 0) {
		HISI_FB_ERR("failed to get ion phys!\n");
		goto err_ion_get_addr;
	}
#else
	if (ion_map_iommu(client, handle, &(hisifd->iommu_format))) {
		HISI_FB_ERR("failed to ion_map_iommu!\n");
		goto err_ion_get_addr;
	}

	buf_addr = hisifd->iommu_format.iova_start;
#endif

	fbi->fix.smem_start = buf_addr;
	fbi->screen_size = fbi->fix.smem_len;
	//memset(fbi->screen_base, 0x0, fbi->screen_size);

	hisifd->ion_handle = handle;

	return buf_addr;

err_ion_get_addr:
	ion_unmap_kernel(hisifd->ion_client, hisifd->ion_handle);
err_ion_map:
	ion_free(hisifd->ion_client, hisifd->ion_handle);
err_return:
	return 0;
}

static void hisifb_free_fb_buffer(struct hisi_fb_data_type *hisifd)
{
	if (hisifd->ion_client != NULL &&
		hisifd->ion_handle != NULL) {
	#ifndef CONFIG_HISI_FB_HEAP_CARVEOUT_USED
		ion_unmap_iommu(hisifd->ion_client, hisifd->ion_handle);
	#endif
		ion_unmap_kernel(hisifd->ion_client, hisifd->ion_handle);
		ion_free(hisifd->ion_client, hisifd->ion_handle);
		hisifd->ion_handle = NULL;
	}
}


/*******************************************************************************
**
*/
static struct fb_ops hisi_fb_ops = {
	.owner = THIS_MODULE,
	.fb_open = hisi_fb_open,
	.fb_release = hisi_fb_release,
	.fb_read = NULL,
	.fb_write = NULL,
	.fb_cursor = NULL,
	.fb_check_var = hisi_fb_check_var,
	.fb_set_par = hisi_fb_set_par,
	.fb_setcolreg = NULL,
	.fb_blank = hisi_fb_blank,
	.fb_pan_display = hisi_fb_pan_display,
	.fb_fillrect = NULL,
	.fb_copyarea = NULL,
	.fb_imageblit = NULL,
	.fb_rotate = NULL,
	.fb_sync = NULL,
	.fb_ioctl = hisi_fb_ioctl,
	.fb_compat_ioctl = hisi_fb_ioctl,
	.fb_mmap = hisi_fb_mmap,
};

static int hisi_fb_register(struct hisi_fb_data_type *hisifd)
{
	int bpp = 0;
	struct hisi_panel_info *panel_info = NULL;
	struct fb_info *fbi = NULL;
	struct fb_fix_screeninfo *fix = NULL;
	struct fb_var_screeninfo *var = NULL;

	BUG_ON(hisifd == NULL);
	panel_info = &hisifd->panel_info;
	BUG_ON(panel_info == NULL);

	/*
	 * fb info initialization
	 */
	fbi = hisifd->fbi;
	fix = &fbi->fix;
	var = &fbi->var;

	fix->type_aux = 0;
	fix->visual = FB_VISUAL_TRUECOLOR;
	fix->ywrapstep = 0;
	fix->mmio_start = 0;
	fix->mmio_len = 0;
	fix->accel = FB_ACCEL_NONE;

	var->xoffset = 0;
	var->yoffset = 0;
	var->grayscale = 0;
	var->nonstd = 0;
	var->activate = FB_ACTIVATE_VBL;
	var->height = panel_info->height;
	var->width = panel_info->width;
	var->accel_flags = 0;
	var->sync = 0;
	var->rotate = 0;

	switch (hisifd->fb_imgType) {
	case HISI_FB_PIXEL_FORMAT_BGR_565:
		fix->type = FB_TYPE_PACKED_PIXELS;
		fix->xpanstep = 1;
		fix->ypanstep = 1;
		var->vmode = FB_VMODE_NONINTERLACED;

		var->blue.offset = 0;
		var->green.offset = 5;
		var->red.offset = 11;
		var->transp.offset = 0;

		var->blue.length = 5;
		var->green.length = 6;
		var->red.length = 5;
		var->transp.length = 0;

		var->blue.msb_right = 0;
		var->green.msb_right = 0;
		var->red.msb_right = 0;
		var->transp.msb_right = 0;
		bpp = 2;
		break;

	case HISI_FB_PIXEL_FORMAT_BGRX_4444:
		fix->type = FB_TYPE_PACKED_PIXELS;
		fix->xpanstep = 1;
		fix->ypanstep = 1;
		var->vmode = FB_VMODE_NONINTERLACED;

		var->blue.offset = 0;
		var->green.offset = 4;
		var->red.offset = 8;
		var->transp.offset = 0;

		var->blue.length = 4;
		var->green.length = 4;
		var->red.length = 4;
		var->transp.length = 0;

		var->blue.msb_right = 0;
		var->green.msb_right = 0;
		var->red.msb_right = 0;
		var->transp.msb_right = 0;
		bpp = 2;
		break;

	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
		fix->type = FB_TYPE_PACKED_PIXELS;
		fix->xpanstep = 1;
		fix->ypanstep = 1;
		var->vmode = FB_VMODE_NONINTERLACED;

		var->blue.offset = 0;
		var->green.offset = 4;
		var->red.offset = 8;
		var->transp.offset = 12;

		var->blue.length = 4;
		var->green.length = 4;
		var->red.length = 4;
		var->transp.length = 4;

		var->blue.msb_right = 0;
		var->green.msb_right = 0;
		var->red.msb_right = 0;
		var->transp.msb_right = 0;
		bpp = 2;
		break;

	case HISI_FB_PIXEL_FORMAT_BGRX_5551:
		fix->type = FB_TYPE_PACKED_PIXELS;
		fix->xpanstep = 1;
		fix->ypanstep = 1;
		var->vmode = FB_VMODE_NONINTERLACED;

		var->blue.offset = 0;
		var->green.offset = 5;
		var->red.offset = 10;
		var->transp.offset = 0;

		var->blue.length = 5;
		var->green.length = 5;
		var->red.length = 5;
		var->transp.length = 0;

		var->blue.msb_right = 0;
		var->green.msb_right = 0;
		var->red.msb_right = 0;
		var->transp.msb_right = 0;
		bpp = 2;
		break;

	case HISI_FB_PIXEL_FORMAT_BGRA_5551:
		fix->type = FB_TYPE_PACKED_PIXELS;
		fix->xpanstep = 1;
		fix->ypanstep = 1;
		var->vmode = FB_VMODE_NONINTERLACED;

		var->blue.offset = 0;
		var->green.offset = 5;
		var->red.offset = 10;
		var->transp.offset = 15;

		var->blue.length = 5;
		var->green.length = 5;
		var->red.length = 5;
		var->transp.length = 1;

		var->blue.msb_right = 0;
		var->green.msb_right = 0;
		var->red.msb_right = 0;
		var->transp.msb_right = 0;
		bpp = 2;
		break;

	case HISI_FB_PIXEL_FORMAT_BGRX_8888:
		fix->type = FB_TYPE_PACKED_PIXELS;
		fix->xpanstep = 1;
		fix->ypanstep = 1;
		var->vmode = FB_VMODE_NONINTERLACED;

		var->blue.offset = 0;
		var->green.offset = 8;
		var->red.offset = 16;
		var->transp.offset = 0;

		var->blue.length = 8;
		var->green.length = 8;
		var->red.length = 8;
		var->transp.length = 0;

		var->blue.msb_right = 0;
		var->green.msb_right = 0;
		var->red.msb_right = 0;
		var->transp.msb_right = 0;

		bpp = 3;
		break;

	case HISI_FB_PIXEL_FORMAT_BGRA_8888:
		fix->type = FB_TYPE_PACKED_PIXELS;
		fix->xpanstep = 1;
		fix->ypanstep = 1;
		var->vmode = FB_VMODE_NONINTERLACED;

		var->blue.offset = 0;
		var->green.offset = 8;
		var->red.offset = 16;
		var->transp.offset = 24;

		var->blue.length = 8;
		var->green.length = 8;
		var->red.length = 8;
		var->transp.length = 8;

		var->blue.msb_right = 0;
		var->green.msb_right = 0;
		var->red.msb_right = 0;
		var->transp.msb_right = 0;

		bpp = 4;
		break;

	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
		fix->type = FB_TYPE_INTERLEAVED_PLANES;
		fix->xpanstep = 2;
		fix->ypanstep = 1;
		var->vmode = FB_VMODE_NONINTERLACED;

		/* FIXME: R/G/B offset? */
		var->blue.offset = 0;
		var->green.offset = 5;
		var->red.offset = 11;
		var->transp.offset = 0;

		var->blue.length = 5;
		var->green.length = 6;
		var->red.length = 5;
		var->transp.length = 0;

		var->blue.msb_right = 0;
		var->green.msb_right = 0;
		var->red.msb_right = 0;
		var->transp.msb_right = 0;

		bpp = 2;
		break;

	default:
		HISI_FB_ERR("fb%d, unkown image type!\n", hisifd->index);
		return -EINVAL;
	}

	var->xres = panel_info->xres;
	var->yres = panel_info->yres;
	var->xres_virtual = panel_info->xres;
	var->yres_virtual = panel_info->yres * hisifd->fb_num;
	var->bits_per_pixel = bpp * 8;

	snprintf(fix->id, sizeof(fix->id), "hisifb%d", hisifd->index);
	fix->line_length = hisifb_line_length(hisifd->index, var->xres_virtual, bpp);
	fix->smem_len = roundup(fix->line_length * var->yres_virtual, PAGE_SIZE);
	fix->smem_start = 0;

	fbi->screen_base = 0;
	fbi->fbops = &hisi_fb_ops;
	fbi->flags = FBINFO_FLAG_DEFAULT;
	fbi->pseudo_palette = NULL;

	hisifd->ion_client = hisi_ion_client_create(HISI_FB_ION_CLIENT_NAME);
	if (IS_ERR_OR_NULL(hisifd->ion_client)) {
		HISI_FB_ERR("failed to create ion client!\n");
		return -ENOMEM;
	}
	hisifd->ion_handle = NULL;
	memset(&hisifd->iommu_format, 0, sizeof(struct iommu_map_format));

	if (fix->smem_len > 0) {
		if (!hisifb_alloc_fb_buffer(hisifd)) {
			HISI_FB_ERR("hisifb_alloc_buffer failed!\n");
			return -ENOMEM;
		}
	}

	hisifd->ref_cnt = 0;
	hisifd->panel_power_on = false;
	sema_init(&hisifd->blank_sem, 1);

	memset(&hisifd->dss_clk_rate, 0, sizeof(struct dss_clk_rate));

	hisifd->on_fnc = hisifb_ctrl_on;
	hisifd->off_fnc = hisifb_ctrl_off;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		g_primary_lcd_xres = panel_info->xres;
		g_primary_lcd_yres = panel_info->yres;

	#ifdef CONFIG_FASTBOOT_DISP_ENABLE
		fastboot_set_needed = 1;
		hisifd->set_fastboot_fnc = hisi_fb_set_fastboot_needed;
	#else
		hisifd->set_fastboot_fnc = NULL;
	#endif
		hisifd->open_sub_fnc = hisi_fb_open_sub;
		hisifd->release_sub_fnc = hisi_fb_release_sub;
		hisifd->frc_fnc = hisifb_ctrl_frc;
		hisifd->esd_fnc = hisifb_ctrl_esd;
		hisifd->sbl_ctrl_fnc = hisifb_ctrl_sbl;

	#ifdef CONFIG_PM_RUNTIME
		hisifd->pm_runtime_register = hisifb_pm_runtime_register;
		hisifd->pm_runtime_unregister = hisifb_pm_runtime_unregister;
		hisifd->pm_runtime_get = hisifb_pm_runtime_get;
		hisifd->pm_runtime_put = hisifb_pm_runtime_put;
	#else
		hisifd->pm_runtime_register = NULL;
		hisifd->pm_runtime_unregister = NULL;
		hisifd->pm_runtime_get = NULL;
		hisifd->pm_runtime_put = NULL;
	#endif
		hisifd->bl_register = hisifb_backlight_register;
		hisifd->bl_unregister = hisifb_backlight_unregister;
		hisifd->bl_update = hisifb_backlight_update;
		hisifd->bl_cancel = hisifb_backlight_cancel;
		hisifd->vsync_register = hisifb_vsync_register;
		hisifd->vsync_unregister = hisifb_vsync_unregister;
		hisifd->vsync_ctrl_fnc = hisifb_vsync_ctrl;
		hisifd->vsync_isr_handler = hisifb_vsync_isr_handler;
		hisifd->buf_sync_register = hisifb_buf_sync_register;
		hisifd->buf_sync_unregister = hisifb_buf_sync_unregister;
		hisifd->buf_sync_signal = hisifb_buf_sync_signal;
		hisifd->buf_sync_suspend = hisifb_buf_sync_suspend;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		hisifd->set_fastboot_fnc = NULL;
		hisifd->open_sub_fnc = NULL;
		hisifd->release_sub_fnc = NULL;
		hisifd->frc_fnc = NULL;
		hisifd->esd_fnc = NULL;
		hisifd->sbl_ctrl_fnc = NULL;

	#ifdef CONFIG_PM_RUNTIME
		hisifd->pm_runtime_register = hisifb_pm_runtime_register;
		hisifd->pm_runtime_unregister = hisifb_pm_runtime_unregister;
		hisifd->pm_runtime_get = hisifb_pm_runtime_get;
		hisifd->pm_runtime_put = hisifb_pm_runtime_put;
	#else
		hisifd->pm_runtime_register = NULL;
		hisifd->pm_runtime_unregister = NULL;
		hisifd->pm_runtime_get = NULL;
		hisifd->pm_runtime_put = NULL;
	#endif
		hisifd->bl_register = hisifb_backlight_register;
		hisifd->bl_unregister = hisifb_backlight_unregister;
		hisifd->bl_update = hisifb_backlight_update;
		hisifd->bl_cancel = hisifb_backlight_cancel;
		hisifd->vsync_register = hisifb_vsync_register;
		hisifd->vsync_unregister = hisifb_vsync_unregister;
		hisifd->vsync_ctrl_fnc = hisifb_vsync_ctrl;
		hisifd->vsync_isr_handler = hisifb_vsync_isr_handler;
		hisifd->buf_sync_register = hisifb_buf_sync_register;
		hisifd->buf_sync_unregister = hisifb_buf_sync_unregister;
		hisifd->buf_sync_signal = hisifb_buf_sync_signal;
		hisifd->buf_sync_suspend = hisifb_buf_sync_suspend;
	} else {
		hisifd->set_fastboot_fnc = NULL;
		hisifd->open_sub_fnc = NULL;
		hisifd->release_sub_fnc = NULL;
		hisifd->frc_fnc = NULL;
		hisifd->esd_fnc = NULL;
		hisifd->sbl_ctrl_fnc = NULL;

		hisifd->pm_runtime_register = NULL;
		hisifd->pm_runtime_unregister = NULL;
		hisifd->pm_runtime_get = NULL;
		hisifd->pm_runtime_put = NULL;
		hisifd->bl_register = NULL;
		hisifd->bl_unregister = NULL;
		hisifd->bl_update = NULL;
		hisifd->bl_cancel = NULL;
		hisifd->vsync_register = NULL;
		hisifd->vsync_unregister = NULL;
		hisifd->vsync_ctrl_fnc = NULL;
		hisifd->vsync_isr_handler = NULL;
		hisifd->buf_sync_register = NULL;
		hisifd->buf_sync_unregister = NULL;
		hisifd->buf_sync_signal = NULL;
		hisifd->buf_sync_suspend = NULL;
	}

	if (hisi_overlay_init(hisifd)) {
		HISI_FB_ERR("unable to init overlay!\n");
		return -EPERM;
	}

	if (register_framebuffer(fbi) < 0) {
		HISI_FB_ERR("fb%d failed to register_framebuffer!", hisifd->index);
		return -EPERM;
	}

	/* backlight register */
	if (hisifd->bl_register)
		hisifd->bl_register(hisifd->pdev);
	/* vsync register */
	if (hisifd->vsync_register)
		hisifd->vsync_register(hisifd->pdev);
	/* buf_sync register */
	if (hisifd->buf_sync_register)
		hisifd->buf_sync_register(hisifd->pdev);
	/* pm runtime register */
	if (hisifd->pm_runtime_register)
		hisifd->pm_runtime_register(hisifd->pdev);

	HISI_FB_INFO("FrameBuffer[%d] %dx%d size=%d bytes phy_addr=%lu virt_addr=%p "
		"is registered successfully!\n",
		hisifd->index, var->xres, var->yres, fbi->fix.smem_len,
		fix->smem_start, fbi->screen_base);

	return 0;
}


/*******************************************************************************
**
*/
static int hisi_fb_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct device_node *np = NULL;

	if (!hisi_fb_resource_initialized) {
		HISI_FB_DEBUG("initialized=%d, +.\n", hisi_fb_resource_initialized);

		pdev->id = 0;

		np = of_find_compatible_node(NULL, NULL, DTS_COMP_FB_NAME);
		if (!np) {
			HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_FB_NAME);
			return -ENXIO;
		}

		/* get irq no */
		hisifd_irq_pdp = irq_of_parse_and_map(np, 0);
		if (!hisifd_irq_pdp) {
			HISI_FB_ERR("failed to get hisifd_irq_pdp resource.\n");
			return -ENXIO;
		}

		hisifd_irq_sdp = irq_of_parse_and_map(np, 1);
		if (!hisifd_irq_sdp) {
			HISI_FB_ERR("failed to get hisifd_irq_sdp resource.\n");
			return -ENXIO;
		}

		hisifd_irq_adp = irq_of_parse_and_map(np, 2);
		if (!hisifd_irq_sdp) {
			HISI_FB_ERR("failed to get hisifd_irq_sdp resource.\n");
			return -ENXIO;
		}

		hisifd_irq_dsi0 = irq_of_parse_and_map(np, 3);
		if (!hisifd_irq_dsi0) {
			HISI_FB_ERR("failed to get hisifd_irq_dsi0 resource.\n");
			return -ENXIO;
		}

		hisifd_irq_dsi1 = irq_of_parse_and_map(np, 4);
		if (!hisifd_irq_dsi1) {
			HISI_FB_ERR("failed to get hisifd_irq_dsi1 resource.\n");
			return -ENXIO;
		}

		/* get dss reg base */
		hisifd_dss_base = of_iomap(np, 0);
		if (!hisifd_dss_base) {
			HISI_FB_ERR("failed to get hisifd_dss_base resource.\n");
			return -ENXIO;
		}
		hisifd_crgperi_base = of_iomap(np, 1);
		if (!hisifd_crgperi_base) {
			HISI_FB_ERR("failed to get hisifd_crgperi_base resource.\n");
			return -ENXIO;
		}

#if !defined(CONFIG_ARCH_HI3630FPGA) && !defined(CONFIG_HISI_3635_FPGA) \
	&& !defined(CONFIG_HISI_3650_FPGA)
		/* get regulator resource */
		g_dpe_regulator[0].supply = REGULATOR_PDP_NAME;
		g_dpe_regulator[1].supply = REGULATOR_SDP_NAME;
		g_dpe_regulator[2].supply = REGULATOR_ADP_NAME;
		ret = devm_regulator_bulk_get(&(pdev->dev),
			ARRAY_SIZE(g_dpe_regulator), g_dpe_regulator);
		if (ret) {
			HISI_FB_ERR("failed to get regulator resource! ret=%d.\n", ret);
			return -ENXIO;
		}

		/* get dss clk resource */
		ret = of_property_read_string_index(np, "clock-names", 1, &g_dss_clk_name);
		if (ret != 0) {
			HISI_FB_ERR("failed to get pri_clk resource! ret=%d.\n", ret);
		ret = of_property_read_string_index(np, "clock-names", 3, &g_dss_pxl0_clk_name);
		if (ret != 0) {
			HISI_FB_ERR("failed to get pxl0_clk resource! ret=%d.\n", ret);
			return -ENXIO;
		}
		ret = of_property_read_string_index(np, "clock-names", 4, &g_dss_pxl1_clk_name);
		if (ret != 0) {
			HISI_FB_ERR("failed to get pxl1_clk resource! ret=%d.\n", ret);
			return -ENXIO;
		}
			return -ENXIO;
		}
		ret = of_property_read_string_index(np, "clock-names", 6, &g_dss_dphy0_clk_name);
				if (ret != 0) {
			HISI_FB_ERR("failed to get dphy0_clk resource! ret=%d.\n", ret);
			return -ENXIO;
		}
		ret = of_property_read_string_index(np, "clock-names", 7, &g_dss_dphy1_clk_name);
				if (ret != 0) {
			HISI_FB_ERR("failed to get dphy1_clk resource! ret=%d.\n", ret);
			return -ENXIO;
		}
#endif

		hisi_fb_resource_initialized = 1;

		hisi_fb_device_set_status0(DTS_FB_RESOURCE_INIT_READY);

		HISI_FB_DEBUG("initialized=%d, -.\n", hisi_fb_resource_initialized);
		return 0;
	}

	if (pdev->id < 0) {
		HISI_FB_ERR("WARNING: id=%d, name=%s!\n", pdev->id, pdev->name);
		return 0;
	}

	if (!hisi_fb_resource_initialized) {
		HISI_FB_ERR("fb resource not initialized!\n");
		return -EPERM;
	}

	if (pdev_list_cnt >= HISI_FB_MAX_DEV_LIST) {
		HISI_FB_ERR("too many fb devices, num=%d!\n", pdev_list_cnt);
		return -ENOMEM;
	}

	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = hisi_fb_register(hisifd);
	if (ret) {
		HISI_FB_ERR("fb%d hisi_fb_register failed, error=%d!\n", hisifd->index, ret);
		return ret;
	}

	/* config earlysuspend */
#ifdef CONFIG_HAS_EARLYSUSPEND
	hisifd->early_suspend.suspend = hisifb_early_suspend;
	hisifd->early_suspend.resume = hisifb_early_resume;
	hisifd->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB - 2;
	register_early_suspend(&hisifd->early_suspend);
#endif

	pdev_list[pdev_list_cnt++] = pdev;

	/* set device probe status */
	hisi_fb_device_set_status1(hisifd);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int hisi_fb_remove(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* pm_runtime unregister */
	if (hisifd->pm_runtime_unregister)
		hisifd->pm_runtime_unregister(pdev);

	/* stop the device */
	if (hisi_fb_suspend_sub(hisifd) != 0)
		HISI_FB_ERR("fb%d hisi_fb_suspend_sub failed!\n", hisifd->index);

	/* overlay destroy */
	hisi_overlay_deinit(hisifd);

	/* free framebuffer */
	hisifb_free_fb_buffer(hisifd);
	if (hisifd->ion_client) {
		ion_client_destroy(hisifd->ion_client);
		hisifd->ion_client = NULL;
	}

	/* remove /dev/fb* */
	unregister_framebuffer(hisifd->fbi);

	/* unregister buf_sync */
	if (hisifd->buf_sync_unregister)
		hisifd->buf_sync_unregister(pdev);
	/* unregister vsync */
	if (hisifd->vsync_unregister)
		hisifd->vsync_unregister(pdev);
	/* unregister backlight */
	if (hisifd->bl_unregister)
		hisifd->bl_unregister(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int hisi_fb_suspend_sub(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	ret = hisi_fb_blank_sub(FB_BLANK_POWERDOWN, hisifd->fbi);
	if (ret) {
		HISI_FB_ERR("fb%d can't turn off display, error=%d!\n", hisifd->index, ret);
		return ret;
	}

	return 0;
}

static int hisi_fb_resume_sub(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	ret = hisi_fb_blank_sub(FB_BLANK_UNBLANK, hisifd->fbi);
	if (ret) {
		HISI_FB_ERR("fb%d can't turn on display, error=%d!\n", hisifd->index, ret);
	}

	return ret;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void hisifb_early_suspend(struct early_suspend *h)
{
	struct hisi_fb_data_type *hisifd = NULL;

	hisifd = container_of(h, struct hisi_fb_data_type, early_suspend);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	hisi_fb_suspend_sub(hisifd);

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);
}

static void hisifb_early_resume(struct early_suspend *h)
{
	struct hisi_fb_data_type *hisifd = NULL;

	hisifd = container_of(h, struct hisi_fb_data_type, early_suspend);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	hisi_fb_resume_sub(hisifd);

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);
}
#endif

#if defined(CONFIG_PM) && !defined(CONFIG_HAS_EARLYSUSPEND)
static int hisi_fb_suspend(struct platform_device *pdev, pm_message_t state)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	console_lock();
	fb_set_suspend(hisifd->fbi, FBINFO_STATE_SUSPENDED);
	ret = hisi_fb_suspend_sub(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("fb%d hisi_fb_suspend_sub failed, error=%d!\n", hisifd->index, ret);
		fb_set_suspend(hisifd->fbi, FBINFO_STATE_RUNNING);
	} else {
		pdev->dev.power.power_state = state;
	}
	console_unlock();

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);

	return ret;
}

static int hisi_fb_resume(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	console_lock();
	ret = hisi_fb_resume_sub(hisifd);
	pdev->dev.power.power_state = PMSG_ON;
	fb_set_suspend(hisifd->fbi, FBINFO_STATE_RUNNING);
	console_unlock();

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);

	return ret;
}
#else
#define hisi_fb_suspend NULL
#define hisi_fb_resume NULL
#endif


/*******************************************************************************
** pm_runtime
*/
#ifdef CONFIG_PM_RUNTIME
static int hisi_fb_runtime_suspend(struct device *dev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;

	hisifd = dev_get_drvdata(dev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	ret = hisi_fb_suspend_sub(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("fb%d, failed to hisi_fb_suspend_sub! ret=%d\n", hisifd->index, ret);
	}

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);

	return 0;
}

static int hisi_fb_runtime_resume(struct device *dev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;

	hisifd = dev_get_drvdata(dev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	ret = hisi_fb_resume_sub(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("fb%d, failed to hisi_fb_resume_sub! ret=%d\n", hisifd->index, ret);
	}

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);

	return 0;
}

static int hisi_fb_runtime_idle(struct device *dev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	hisifd = dev_get_drvdata(dev);
	BUG_ON(hisifd == NULL);

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);

	return 0;
}

static void hisifb_pm_runtime_get(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	ret = pm_runtime_get_sync(hisifd->fbi->dev);
	if (ret < 0) {
		HISI_FB_ERR("fb%d, failed to pm_runtime_get_sync! ret=%d.", hisifd->index, ret);
	}
}

static void hisifb_pm_runtime_put(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	ret = pm_runtime_put(hisifd->fbi->dev);
	if (ret < 0) {
		HISI_FB_ERR("fb%d, failed to pm_runtime_put! ret=%d.", hisifd->index, ret);
	}
}

static void hisifb_pm_runtime_register(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	ret = pm_runtime_set_active(hisifd->fbi->dev);
	if (ret < 0)
		HISI_FB_ERR("fb%d failed to pm_runtime_set_active.\n", hisifd->index);
	pm_runtime_enable(hisifd->fbi->dev);
}

static void hisifb_pm_runtime_unregister(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pm_runtime_disable(hisifd->fbi->dev);
}
#endif

#ifdef CONFIG_PM_SLEEP
static int hisi_fb_pm_suspend(struct device *dev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;

	hisifd = dev_get_drvdata(dev);
	if (!hisifd)
		return 0;

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	ret = hisi_fb_suspend_sub(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("fb%d, failed to hisi_fb_suspend_sub! ret=%d\n", hisifd->index, ret);
	}

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);

	return 0;
}

static int hisi_fb_pm_resume(struct device *dev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;

	hisifd = dev_get_drvdata(dev);
	if (!hisifd)
		return 0;

	HISI_FB_INFO("fb%d, +.\n", hisifd->index);

	ret = hisi_fb_resume_sub(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("fb%d, failed to hisi_fb_resume_sub! ret=%d\n", hisifd->index, ret);
	}

	HISI_FB_INFO("fb%d, -.\n", hisifd->index);

	return 0;
}
#endif


/*******************************************************************************
**
*/
static struct dev_pm_ops hisi_fb_dev_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(hisi_fb_pm_suspend, hisi_fb_pm_resume)
	SET_RUNTIME_PM_OPS(hisi_fb_runtime_suspend,
		hisi_fb_runtime_resume, hisi_fb_runtime_idle)
};

static const struct of_device_id hisi_fb_match_table[] = {
	{
		.compatible = DTS_COMP_FB_NAME,
		.data = NULL,
	},
};
MODULE_DEVICE_TABLE(of, hisi_fb_match_table);

static struct platform_driver hisi_fb_driver = {
	.probe = hisi_fb_probe,
	.remove = hisi_fb_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend = hisi_fb_suspend,
	.resume = hisi_fb_resume,
#endif
	.shutdown = NULL,
	.driver = {
		.name = DEV_NAME_FB,
		.of_match_table = of_match_ptr(hisi_fb_match_table),
		.pm = &hisi_fb_dev_pm_ops,
	},
};

static int __init hisi_fb_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&hisi_fb_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(hisi_fb_init);
