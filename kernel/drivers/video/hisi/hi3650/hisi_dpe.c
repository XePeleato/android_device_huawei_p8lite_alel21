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
#include "hisi_dpe_utils.h"
#include "hisi_overlay_utils.h"


/*******************************************************************************
**
*/
static int dpe_init(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		init_dbuf(hisifd);
		init_dpp(hisifd);
		init_sbl(hisifd);
		init_ifbc(hisifd);
		init_ldi(hisifd);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		init_dbuf(hisifd);
		init_ldi(hisifd);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		;
	} else {
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
	}

	return 0;
}

static int dpe_deinit(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		deinit_ldi(hisifd);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		deinit_ldi(hisifd);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		;
	} else {
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
	}

	return 0;
}

static void dpe_interrupt_unmask(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;
	uint32_t unmask = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		unmask = ~0;
		unmask &= ~(BIT_BACKLIGHT | BIT_VSYNC | BIT_VACTIVE0_START);
		outp32(dss_base + GLB_CPU_PDP_INT_MSK, unmask);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		unmask = ~0;
		unmask &= ~(BIT_VSYNC | BIT_VACTIVE0_START);
		outp32(dss_base + GLB_CPU_SDP_INT_MSK, unmask);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		;
	} else {
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
	}
}

static void dpe_interrupt_mask(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;
	uint32_t mask = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		mask = ~0;
		outp32(dss_base + GLB_CPU_PDP_INT_MSK, mask);

	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		mask = ~0;
		outp32(dss_base + GLB_CPU_SDP_INT_MSK, mask);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		;
	} else {
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
	}
}

static int dpe_irq_enable(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	if (hisifd->dpe_irq)
		enable_irq(hisifd->dpe_irq);

	return 0;
}

static int dpe_irq_disable(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	if (hisifd->dpe_irq)
		disable_irq(hisifd->dpe_irq);

	/*disable_irq_nosync(hisifd->dpe_irq);*/

	return 0;
}

static int dpe_irq_disable_nosync(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	if (hisifd->dpe_irq)
		disable_irq_nosync(hisifd->dpe_irq);

	return 0;
}

int dpe_regulator_enable(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

#if !defined(CONFIG_ARCH_HI3630FPGA) && !defined(CONFIG_HISI_3635_FPGA) \
	&& !defined(CONFIG_HISI_3650_FPGA)
	ret = regulator_bulk_enable(1, hisifd->dpe_regulator);
	if (ret) {
		HISI_FB_ERR("fb%d regulator_enable failed, error=%d!\n", hisifd->index, ret);
		return ret;
	}
#endif

	return ret;
}

int dpe_regulator_disable(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

#if !defined(CONFIG_ARCH_HI3630FPGA) && !defined(CONFIG_HISI_3635_FPGA) \
	&& !defined(CONFIG_HISI_3650_FPGA)
	ret = regulator_bulk_disable(1, hisifd->dpe_regulator);
	if (ret != 0) {
		HISI_FB_ERR("fb%d regulator_disable failed, error=%d!\n", hisifd->index, ret);
		return ret;
	}
#endif

	return ret;
}

int dpe_clk_enable(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	return ret;
}

int dpe_clk_disable(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	return ret;
}


/*******************************************************************************
**
*/
static int dpe_set_fastboot(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_set_fastboot(pdev);

	dpe_irq_enable(hisifd);
	dpe_interrupt_unmask(hisifd);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int dpe_on(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	dpe_regulator_enable(hisifd);
	dpe_clk_enable(hisifd);

	dpe_init(hisifd);

	if (is_ldi_panel(hisifd)) {
		hisifd->panel_info.lcd_init_step = LCD_INIT_POWER_ON;
		ret = panel_next_on(pdev);
	}

	ret = panel_next_on(pdev);

	if (hisifd->panel_info.vsync_ctrl_type == VSYNC_CTRL_NONE) {
		dpe_irq_enable(hisifd);
		dpe_interrupt_unmask(hisifd);
	}

#if 0
	if (hisifd->index == PRIMARY_PANEL_IDX) {
		enable_ldi(hisifd);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		enable_ldi(hisifd);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		;
	} else {
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
	}
#endif

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int dpe_off(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (hisifd->panel_info.vsync_ctrl_type == VSYNC_CTRL_NONE) {
		dpe_interrupt_mask(hisifd);
		dpe_irq_disable(hisifd);
	} else {
		if (hisifd->vsync_ctrl.vsync_ctrl_enabled == 1) {
			if (hisifd->panel_info.vsync_ctrl_type & VSYNC_CTRL_ISR_OFF) {
				dpe_interrupt_mask(hisifd);
				dpe_irq_disable(hisifd);
				HISI_FB_INFO("fb%d, need to disable dpe irq! vsync_ctrl_enabled=%d.\n",
					hisifd->index, hisifd->vsync_ctrl.vsync_ctrl_enabled);
			}
		}
	}

	ret = panel_next_off(pdev);

	dpe_deinit(hisifd);

	dpe_clk_disable(hisifd);
	dpe_regulator_disable(hisifd);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int dpe_remove(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_remove(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int dpe_set_backlight(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_set_backlight(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int dpe_sbl_ctrl(struct platform_device *pdev, int enable)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *sbl_base = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	sbl_base = hisifd->dss_base + DSS_DPP_SBL_OFFSET;

	if (hisifd->panel_info.sbl_support == 0) {
		HISI_FB_ERR("fb%d not support SBL!\n", hisifd->index);
		return 0;
	}

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	set_reg(sbl_base + SBL_BACKLIGHT_L, hisifd->sbl.sbl_backlight_l, 8, 0);
	set_reg(sbl_base + SBL_BACKLIGHT_H, hisifd->sbl.sbl_backlight_h, 8, 0);
	set_reg(sbl_base + SBL_AMBIENT_LIGHT_L, hisifd->sbl.sbl_ambient_light_l, 8, 0);
	set_reg(sbl_base + SBL_AMBIENT_LIGHT_H, hisifd->sbl.sbl_ambient_light_h, 8, 0);
	set_reg(sbl_base + SBL_CALIBRATION_A_L, hisifd->sbl.sbl_calibration_a_l, 8, 0);
	set_reg(sbl_base + SBL_CALIBRATION_A_H, hisifd->sbl.sbl_calibration_a_h, 8, 0);
	set_reg(sbl_base + SBL_CALIBRATION_B_L, hisifd->sbl.sbl_calibration_b_l, 8, 0);
	set_reg(sbl_base + SBL_CALIBRATION_B_H, hisifd->sbl.sbl_calibration_b_h, 8, 0);
	set_reg(sbl_base + SBL_CALIBRATION_C_L, hisifd->sbl.sbl_calibration_c_l, 8, 0);
	set_reg(sbl_base + SBL_CALIBRATION_C_H, hisifd->sbl.sbl_calibration_c_h, 8, 0);
	set_reg(sbl_base + SBL_CALIBRATION_D_L, hisifd->sbl.sbl_calibration_d_l, 8, 0);
	set_reg(sbl_base + SBL_CALIBRATION_D_H, hisifd->sbl.sbl_calibration_d_h, 8, 0);
	set_reg(sbl_base + SBL_ENABLE, hisifd->sbl.sbl_enable, 1, 0);

	ret = panel_next_sbl_ctrl(pdev, enable);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int dpe_vsync_ctrl(struct platform_device *pdev, int enable)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (enable) {
		ret = panel_next_vsync_ctrl(pdev, enable);
		if (hisifd->panel_info.vsync_ctrl_type & VSYNC_CTRL_ISR_OFF) {
			dpe_interrupt_unmask(hisifd);
			dpe_irq_enable(hisifd);
		}
	} else {
		ret = panel_next_vsync_ctrl(pdev, enable);
		if (hisifd->panel_info.vsync_ctrl_type & VSYNC_CTRL_ISR_OFF) {
			dpe_interrupt_mask(hisifd);
			dpe_irq_disable_nosync(hisifd);
		}
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int dpe_frc_handle(struct platform_device *pdev, int fps)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_frc_handle(pdev, fps);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int dpe_esd_handle(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_esd_handle(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int dpe_set_display_region(struct platform_device *pdev,
	struct dss_rect *dirty)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *dbuf_base = NULL;
	char __iomem *ifbc_base = NULL;
	char __iomem *ldi_base = NULL;

	BUG_ON(pdev == NULL || dirty == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("index=%d, enter!\n", hisifd->index);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		dbuf_base = hisifd->dss_base + DSS_DBUF0_OFFSET;
		ifbc_base = hisifd->dss_base + DSS_IFBC_OFFSET;
		ldi_base = hisifd->dss_base+ DSS_LDI0_OFFSET;
	} else {
		WARN_ON(1);
	}

	outp32(dbuf_base + DBUF_FRM_SIZE, dirty->w * dirty->h);
	outp32(dbuf_base + DBUF_FRM_HSIZE, DSS_WIDTH(dirty->w));

	outp32(ifbc_base + IFBC_SIZE, ((DSS_WIDTH(dirty->w) << 16) | DSS_HEIGHT(dirty->h)));

	outp32(ldi_base + LDI_DPI0_HRZ_CTRL1, DSS_WIDTH(dirty->w) | (DSS_WIDTH(hisifd->panel_info.ldi.h_pulse_width) << 16));
	outp32(ldi_base + LDI_VRT_CTRL1, DSS_HEIGHT(dirty->h) | (DSS_HEIGHT(hisifd->panel_info.ldi.v_pulse_width) << 16));

	ret = panel_next_set_display_region(pdev, dirty);

	HISI_FB_DEBUG("index=%d, exit!\n", hisifd->index);

	return ret;
}

static int dpe_lcd_check_reg(struct platform_device *pdev)
{
	BUG_ON(pdev == NULL);

	return panel_next_lcd_check_reg(pdev);
}

static int dpe_lcd_mipi_detect(struct platform_device *pdev)
{
	BUG_ON(pdev == NULL);

	return panel_next_lcd_mipi_detect(pdev);
}

static int dpe_set_pixclk_rate(struct platform_device *pdev)
{
	char __iomem *ldi_base = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	struct dss_clk_rate *pdss_clk_rate = NULL;
	int ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);
	pdss_clk_rate = get_dss_clk_rate(hisifd);
	BUG_ON(pdss_clk_rate == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		ldi_base = hisifd->dss_base+ DSS_LDI0_OFFSET;

		if (IS_ERR(hisifd->dss_pxl0_clk)) {
			ret = PTR_ERR(hisifd->dss_pxl0_clk);
			return ret;
		}

		ret = clk_set_rate(hisifd->dss_pxl0_clk, pinfo->pxl_clk_rate);
		if (ret < 0) {
			HISI_FB_ERR("fb%d dss_pxl1_clk clk_set_rate(%llu) failed, error=%d!\n",
				hisifd->index, pinfo->pxl_clk_rate, ret);
			return -EINVAL;
		}

		HISI_FB_INFO("dss_pxl1_clk:[%llu]->[%llu].\n",
			pinfo->pxl_clk_rate, (uint64_t)clk_get_rate(hisifd->dss_pxl0_clk));
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		ldi_base = hisifd->dss_base+ DSS_LDI1_OFFSET;

		if (IS_ERR(hisifd->dss_pxl1_clk)) {
			ret = PTR_ERR(hisifd->dss_pxl1_clk);
			return ret;
		}

		ret = clk_set_rate(hisifd->dss_pxl1_clk, pinfo->pxl_clk_rate);
		if (ret < 0) {
			HISI_FB_ERR("fb%d dss_pxl1_clk clk_set_rate(%llu) failed, error=%d!\n",
				hisifd->index, pinfo->pxl_clk_rate, ret);
			return -EINVAL;
		}

		HISI_FB_INFO("dss_pxl1_clk:[%llu]->[%llu].\n",
			pinfo->pxl_clk_rate, (uint64_t)clk_get_rate(hisifd->dss_pxl1_clk));
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return -EINVAL;
	}

	init_dbuf(hisifd);

	outp32(ldi_base + LDI_DPI0_HRZ_CTRL0,
		pinfo->ldi.h_front_porch | (pinfo->ldi.h_back_porch << 16));
	outp32(ldi_base + LDI_DPI0_HRZ_CTRL1, DSS_WIDTH(pinfo->ldi.h_pulse_width));
	outp32(ldi_base + LDI_DPI0_HRZ_CTRL1, DSS_WIDTH(pinfo->xres));

	outp32(ldi_base + LDI_VRT_CTRL0,
		pinfo->ldi.v_front_porch | (pinfo->ldi.v_back_porch << 16));
	outp32(ldi_base + LDI_VRT_CTRL1, DSS_HEIGHT(pinfo->ldi.v_pulse_width));
	outp32(ldi_base + LDI_VRT_CTRL2, DSS_HEIGHT(pinfo->yres));

	outp32(ldi_base + LDI_PLR_CTRL,
		pinfo->ldi.vsync_plr | (pinfo->ldi.hsync_plr << 1) |
		(pinfo->ldi.pixelclk_plr << 2) | (pinfo->ldi.data_en_plr << 3));

	return 0;
}

/*******************************************************************************
**
*/
static int dpe_regulator_clk_irq_setup(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	const char *regulator_name = NULL;
	const char *irq_name = NULL;
	irqreturn_t (*isr_fnc)(int irq, void *ptr);
	int ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		regulator_name = REGULATOR_PDP_NAME;
		irq_name = IRQ_PDP_NAME;
		isr_fnc = dss_pdp_isr;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		regulator_name = REGULATOR_SDP_NAME;
		irq_name = IRQ_SDP_NAME;
		isr_fnc = dss_sdp_isr;
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		regulator_name = REGULATOR_ADP_NAME;
		irq_name = IRQ_ADP_NAME;
		isr_fnc = dss_adp_isr;
	} else {
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
		return -EINVAL;
	}

	ret = request_irq(hisifd->dpe_irq, isr_fnc, IRQF_DISABLED, irq_name, (void *)hisifd);
	if (ret != 0) {
		HISI_FB_ERR("fb%d request_irq failed, irq_no=%d error=%d!\n",
			hisifd->index, hisifd->dpe_irq, ret);
		return ret;
	} else {
		if (hisifd->dpe_irq)
			disable_irq(hisifd->dpe_irq);
	}

	//hisi_irqaffinity_register(hisifd->dpe_irq, 1);

	return 0;
}

static int dpe_probe(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct platform_device *hisi_fb_dev = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	struct fb_info *fbi = NULL;
	int ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = dpe_regulator_clk_irq_setup(pdev);
	if (ret) {
		HISI_FB_ERR("fb%d dpe_irq_clk_setup failed, error=%d!\n", hisifd->index, ret);
		goto err;
	}

	/* alloc device */
	hisi_fb_dev = platform_device_alloc(DEV_NAME_FB, pdev->id);
	if (!hisi_fb_dev) {
		HISI_FB_ERR("fb%d platform_device_alloc failed, error=%d!\n", hisifd->index, ret);
		ret = -ENOMEM;
		goto err_device_alloc;
	}

	/* link to the latest pdev */
	hisifd->pdev = hisi_fb_dev;

	/* alloc panel device data */
	ret = platform_device_add_data(hisi_fb_dev, dev_get_platdata(&pdev->dev),
		sizeof(struct hisi_fb_panel_data));
	if (ret) {
		HISI_FB_ERR("fb%d platform_device_add_data failed, error=%d!\n", hisifd->index, ret);
		goto err_device_put;
	}

	/* data chain */
	pdata = dev_get_platdata(&hisi_fb_dev->dev);
	pdata->set_fastboot = dpe_set_fastboot;
	pdata->on = dpe_on;
	pdata->off = dpe_off;
	pdata->remove = dpe_remove;
	pdata->set_backlight = dpe_set_backlight;
	pdata->sbl_ctrl = dpe_sbl_ctrl;
	pdata->vsync_ctrl = dpe_vsync_ctrl;
	pdata->frc_handle = dpe_frc_handle;
	pdata->esd_handle = dpe_esd_handle;
	pdata->set_display_region = dpe_set_display_region;
	pdata->set_pixclk_rate = dpe_set_pixclk_rate;
	pdata->lcd_check_reg = dpe_lcd_check_reg;
	pdata->lcd_mipi_detect = dpe_lcd_mipi_detect;
	pdata->next = pdev;

	/* get/set panel info */
	memcpy(&hisifd->panel_info, pdata->panel_info, sizeof(struct hisi_panel_info));

	fbi = hisifd->fbi;
	fbi->var.pixclock = hisifd->panel_info.pxl_clk_rate;
	/*fbi->var.pixclock = clk_round_rate(hisifd->dpe_clk, hisifd->panel_info.pxl_clk_rate);*/
	fbi->var.left_margin = hisifd->panel_info.ldi.h_back_porch;
	fbi->var.right_margin = hisifd->panel_info.ldi.h_front_porch;
	fbi->var.upper_margin = hisifd->panel_info.ldi.v_back_porch;
	fbi->var.lower_margin = hisifd->panel_info.ldi.v_front_porch;
	fbi->var.hsync_len = hisifd->panel_info.ldi.h_pulse_width;
	fbi->var.vsync_len = hisifd->panel_info.ldi.v_pulse_width;

	/* set driver data */
	platform_set_drvdata(hisi_fb_dev, hisifd);
	ret = platform_device_add(hisi_fb_dev);
	if (ret) {
		HISI_FB_ERR("fb%d platform_device_add failed, error=%d!\n", hisifd->index, ret);
		goto err_device_put;
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;

err_device_put:
	platform_device_put(hisi_fb_dev);
err_device_alloc:
err:
	return ret;
}

static struct platform_driver this_driver = {
	.probe = dpe_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = DEV_NAME_DSS_DPE,
	},
};

static int __init dpe_driver_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(dpe_driver_init);
