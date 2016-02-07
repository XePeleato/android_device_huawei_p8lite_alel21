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
		enable_clk_pdp(hisifd);

		hisi_dss_config_ok_begin(hisifd);
		init_dpp(hisifd);
		init_dbuf(hisifd);
		init_sbl(hisifd);
		init_ifbc(hisifd);
		init_acm(hisifd);
		init_xcc_and_gama(hisifd);
		init_ldi(hisifd);
		hisi_dss_config_ok_end(hisifd);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		enable_clk_sdp(hisifd);
		hisi_dss_config_ok_begin(hisifd);
		init_dbuf(hisifd);
		init_ldi(hisifd);
		hisi_dss_config_ok_end(hisifd);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		enable_clk_adp(hisifd);
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

		disable_clk_pdp(hisifd);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		deinit_ldi(hisifd);
		disable_clk_sdp(hisifd);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		disable_clk_adp(hisifd);
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
		/* Stage 1  interrupts */
		//unmask = ~0;
		//unmask &= ~(BIT_PDP_LDI0_IRQ_CPU | BIT_WBE0_MMU_IRQ_CPU);
		unmask = 0x0;
		outp32(dss_base + DSS_GLB_PDP_CPU_IRQ_MSK, unmask);

		/*
		** Stage 2 interrupts
		** BIT_LDI_UNFLOW_INT in hisi_baselayer_init
		*/
		unmask = ~0;

		if (is_mipi_cmd_panel(hisifd))
			unmask &= ~(BIT_VACTIVE0_START_INT | BIT_VACTIVE0_END_INT | BIT_LDI_TE0_PIN_INT);
		else
			unmask &= ~(BIT_VSYNC_INT | BIT_VACTIVE0_START_INT | BIT_VACTIVE0_END_INT);
		outp32(dss_base + PDP_LDI_CPU_IRQ_MSK, unmask);

		unmask = ~0;
		unmask &= ~(BIT_WBE0_FRAME_END_CPU_CH1 | BIT_WBE0_FRAME_END_CPU_CH0);
		outp32(dss_base + DSS_DPE0_OFFSET +
			DSS_DP_CTRL_OFFSET + PDP_WBE0_CPU_IRQ_MSK, unmask);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		/* Stage 1  interrupts */
		//unmask = ~0;
		//unmask &= ~(BIT_SDP_LDI1_IRQ_MCU | BIT_WBE0_MMU_IRQ_CPU);
		unmask = 0x0;
		outp32(dss_base + DSS_GLB_SDP_CPU_IRQ_MSK, unmask);

		/* Stage 2 interrupts */
		unmask = ~0;
		unmask &= ~(BIT_VSYNC_INT | BIT_VACTIVE0_START_INT);
		outp32(dss_base + SDP_LDI_CPU_IRQ_MSK, unmask);

		unmask = ~0;
		unmask &= ~(BIT_WBE0_FRAME_END_CPU_CH1 | BIT_WBE0_FRAME_END_CPU_CH0);
		outp32(dss_base + DSS_DPE1_OFFSET +
			DSS_DP_CTRL_OFFSET + SDP_WBE0_CPU_IRQ_MSK, unmask);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		/* Stage 1  interrupts */
		//unmask = ~0;
		//unmask &= ~(BIT_OFFLINE_S2_IRQ_CPU_OFF |
		//	BIT_WBE1_MMU_IRQ_CPU_OFF | BIT_WBE0_MMU_IRQ_CPU_OFF |
		//	BIT_CMDLIST_IRQ_CPU_OFF);
		unmask = 0x0;
		outp32(dss_base + DSS_GLB_OFFLINE_CPU_IRQ_MSK, unmask);

		/* Stage 2 interrupts */
		unmask = ~0;
		unmask &= ~(BIT_OFFLINE_WBE1_CH0_FRM_END_CPU |
			BIT_OFFLINE_WBE1_CH1_FRM_END_CPU);
		outp32(dss_base + DSS_DPE3_OFFSET +
			DSS_DP_CTRL_OFFSET + OFFLINE_WBE1_CPU_IRQ_MSK, unmask);

		/* enable adp irq */
		//outp32(dss_base + DSS_GLB_OFFLINE_S2_CPU_IRQ_MSK, 0);
		/* enable cmdlist irq */
		//set_reg(dss_base + DSS_CMD_LIST_OFFSET + CMDLIST_CH4_INTE, 0x3F, 6, 0);
		//set_reg(dss_base + DSS_CMD_LIST_OFFSET + CMDLIST_CH5_INTE, 0x3F, 6, 0);
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
		/* Stage 1  interrupts */
		mask = ~0;
		outp32(dss_base + DSS_GLB_PDP_CPU_IRQ_MSK, mask);

		/* Stage 2 interrupts */
		mask = ~0;
		outp32(dss_base + DSS_DPE0_OFFSET +
			DSS_DP_CTRL_OFFSET + PDP_WBE0_CPU_IRQ_MSK, mask);

		mask = ~0;
		outp32(dss_base + PDP_LDI_CPU_IRQ_MSK, mask);
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		/* Stage 1  interrupts */
		mask = ~0;
		outp32(dss_base + DSS_GLB_SDP_CPU_IRQ_MSK, mask);

		/* Stage 2 interrupts */
		mask = ~0;
		outp32(dss_base + DSS_DPE1_OFFSET +
			DSS_DP_CTRL_OFFSET + SDP_WBE0_CPU_IRQ_MSK, mask);

		mask = ~0;
		outp32(dss_base + SDP_LDI_CPU_IRQ_MSK, mask);
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		/* Stage 1  interrupts */
		mask = ~0;
		outp32(dss_base + DSS_GLB_OFFLINE_CPU_IRQ_MSK, mask);

		/* Stage 2 interrupts */
		mask = ~0;
		outp32(dss_base + DSS_DPE3_OFFSET +
			DSS_DP_CTRL_OFFSET + OFFLINE_WBE1_CPU_IRQ_MSK, mask);
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

	ret = regulator_bulk_enable(1, hisifd->dpe_regulator);
	if (ret) {
		HISI_FB_ERR("fb%d regulator_enable failed, error=%d!\n", hisifd->index, ret);
		return ret;
	}

	return ret;
}

int dpe_regulator_disable(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	ret = regulator_bulk_disable(1, hisifd->dpe_regulator);
	if (ret != 0) {
		HISI_FB_ERR("fb%d regulator_disable failed, error=%d!\n", hisifd->index, ret);
		return ret;
	}

	return ret;
}

int dpe_clk_enable(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;
	struct clk *clk_tmp = NULL;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		clk_tmp = hisifd->dss_axi_clk;
		if (clk_tmp) {
			ret = clk_prepare(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_axi_clk clk_prepare failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}

			ret = clk_enable(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_axi_clk clk_enable failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}
		}

		clk_tmp = hisifd->dss_pclk_clk;
		if (clk_tmp) {
			ret = clk_prepare(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_pclk_clk clk_prepare failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}

			ret = clk_enable(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_pclk_clk clk_enable failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}
		}

		clk_tmp = hisifd->dss_pri_clk;
		if (clk_tmp) {
			ret = clk_prepare(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_pri_clk clk_prepare failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}

			ret = clk_enable(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_pri_clk clk_enable failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}
		}

		clk_tmp = hisifd->dss_pxl0_clk;
		if (clk_tmp) {
			ret = clk_prepare(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_pxl0_clk clk_prepare failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}

			ret = clk_enable(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_pxl0_clk clk_enable failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}
		}
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		clk_tmp = hisifd->dss_pxl1_clk;
		if (clk_tmp) {
			ret = clk_prepare(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_pxl1_clk clk_prepare failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}

			ret = clk_enable(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_pxl1_clk clk_enable failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}
		}
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		clk_tmp = hisifd->dss_aux_clk;
		if (clk_tmp) {
			ret = clk_prepare(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_aux_clk clk_prepare failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}

			ret = clk_enable(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_aux_clk clk_enable failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}
		}
	} else {
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
		return -EINVAL;
	}

	return 0;
}

int dpe_clk_disable(struct hisi_fb_data_type *hisifd)
{
	struct clk *clk_tmp = NULL;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		clk_tmp = hisifd->dss_pclk_clk;
		if (clk_tmp) {
			clk_disable(clk_tmp);
			clk_unprepare(clk_tmp);
		}

		clk_tmp = hisifd->dss_axi_clk;
		if (clk_tmp) {
			clk_disable(clk_tmp);
			clk_unprepare(clk_tmp);
		}

		clk_tmp = hisifd->dss_pri_clk;
		if (clk_tmp) {
			clk_disable(clk_tmp);
			clk_unprepare(clk_tmp);
		}

		clk_tmp = hisifd->dss_pxl0_clk;
		if (clk_tmp) {
			clk_disable(clk_tmp);
			clk_unprepare(clk_tmp);
		}
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		clk_tmp = hisifd->dss_pxl1_clk;
		if (clk_tmp) {
			clk_disable(clk_tmp);
			clk_unprepare(clk_tmp);
		}
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		clk_tmp = hisifd->dss_aux_clk;
		if (clk_tmp) {
			clk_disable(clk_tmp);
			clk_unprepare(clk_tmp);
		}
	} else {
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
		return -EINVAL;
	}

	return 0;
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

	dpe_regulator_enable(hisifd);
	enable_clk_pdp(hisifd);

	/* data_gate_en: mipi command LCD open */
	if (is_mipi_cmd_panel(hisifd)) {
		hisifd->ldi_data_gate_en = 1;
	}

	ret = panel_next_set_fastboot(pdev);

	if (hisifd->panel_info.vsync_ctrl_type == VSYNC_CTRL_NONE) {
		dpe_irq_enable(hisifd);
		dpe_interrupt_unmask(hisifd);
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int dpe_on(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* dis reset DSI */
	if (is_mipi_panel(hisifd)) {
		if (is_dual_mipi_panel(hisifd)) {
			outp32(hisifd->crgperi_base + PERRSTDIS3_OFFSET, 0x00030000);
		} else {
			outp32(hisifd->crgperi_base + PERRSTDIS3_OFFSET, 0x00020000);
		}
	}

	dpe_regulator_enable(hisifd);
	dpe_init(hisifd);

	dpe_set_ct_cscValue(hisifd);

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
	dpe_regulator_disable(hisifd);

	/* reset DSI */
	if (is_mipi_panel(hisifd)) {
		if (is_dual_mipi_panel(hisifd)) {
			outp32(hisifd->crgperi_base + PERRSTEN3_OFFSET, 0x00030000);
		} else {
			outp32(hisifd->crgperi_base + PERRSTEN3_OFFSET, 0x00020000);
		}
	}

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

	if (hisifd->dss_axi_clk) {
		clk_put(hisifd->dss_axi_clk);
		hisifd->dss_axi_clk = NULL;
	}

	if (hisifd->dss_pclk_clk) {
		clk_put(hisifd->dss_pclk_clk);
		hisifd->dss_pclk_clk = NULL;
	}

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		if (hisifd->dss_pri_clk) {
			clk_put(hisifd->dss_pri_clk);
			hisifd->dss_pri_clk = NULL;
		}

		if (hisifd->dss_pxl0_clk) {
			clk_put(hisifd->dss_pxl0_clk);
			hisifd->dss_pxl0_clk = NULL;
		}
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		if (hisifd->dss_pxl1_clk) {
			clk_put(hisifd->dss_pxl1_clk);
			hisifd->dss_pxl1_clk = NULL;
		}
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		if (hisifd->dss_aux_clk) {
			clk_put(hisifd->dss_aux_clk);
			hisifd->dss_aux_clk = NULL;
		}
	} else {
		ret = -1;
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
	}

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

	HISI_FB_INFO("sbl_lsensor_value=%d, sbl_level=%d, sbl_enable=%d\n",
							hisifd->sbl_lsensor_value, hisifd->sbl_level, hisifd->sbl_enable);

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
	char __iomem *addr = NULL;

	BUG_ON(pdev == NULL || dirty == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("index=%d, enter!\n", hisifd->index);

	addr = hisifd->dss_base + DSS_DPE0_OFFSET + DFS_FRM_SIZE;
	outp32(addr, dirty->w * dirty->h);
	addr = hisifd->dss_base + DSS_DPE0_OFFSET + DFS_FRM_HSIZE;
	outp32(addr, DSS_WIDTH(dirty->w));

	addr = hisifd->dss_base + DSS_DPP_IFBC_OFFSET + IFBC_SIZE;
	outp32(addr, ((DSS_WIDTH(dirty->w) << 16) | DSS_HEIGHT(dirty->h)));

	addr = hisifd->dss_base + PDP_LDI_DPI0_HRZ_CTRL1;
	outp32(addr, DSS_WIDTH(dirty->w) | (DSS_WIDTH(hisifd->panel_info.ldi.h_pulse_width) << 16));
	addr = hisifd->dss_base + PDP_LDI_VRT_CTRL1;
	outp32(addr, DSS_HEIGHT(dirty->h) | (DSS_HEIGHT(hisifd->panel_info.ldi.v_pulse_width) << 16));

	ret = panel_next_set_display_region(pdev, dirty);

	HISI_FB_DEBUG("index=%d, exit!\n", hisifd->index);

	return ret;
}

static int dpe_set_pixclk_rate(struct platform_device *pdev)
{
	char __iomem *dss_base = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	struct dss_clk_rate *pdss_clk_rate = NULL;
	int ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;
	BUG_ON(dss_base == NULL);

	pinfo = &(hisifd->panel_info);
	pdss_clk_rate = get_dss_clk_rate(hisifd);
	BUG_ON(pdss_clk_rate == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		if (IS_ERR(hisifd->dss_pxl0_clk)) {
			ret = PTR_ERR(hisifd->dss_pxl0_clk);
			return ret;
		}

		ret = clk_set_rate(hisifd->dss_pxl0_clk, pinfo->pxl_clk_rate);
		if (ret < 0) {
			HISI_FB_ERR("fb%d dss_pxl0_clk clk_set_rate(%llu) failed, error=%d!\n",
				hisifd->index, pinfo->pxl_clk_rate, ret);
			return -EINVAL;
		}

		HISI_FB_INFO("dss_pxl0_clk:[%llu]->[%llu].\n",
			pinfo->pxl_clk_rate, (uint64_t)clk_get_rate(hisifd->dss_pxl0_clk));

		init_dbuf(hisifd);

		outp32(dss_base + PDP_LDI_DPI0_HRZ_CTRL0,
			pinfo->ldi.h_front_porch | (pinfo->ldi.h_back_porch << 16));
		outp32(dss_base + PDP_LDI_DPI0_HRZ_CTRL1,
			DSS_WIDTH(pinfo->xres) | (DSS_WIDTH(pinfo->ldi.h_pulse_width) << 16));
		outp32(dss_base + PDP_LDI_VRT_CTRL0,
			pinfo->ldi.v_front_porch | (pinfo->ldi.v_back_porch << 16));
		outp32(dss_base + PDP_LDI_VRT_CTRL1,
			DSS_HEIGHT(pinfo->yres) | (DSS_HEIGHT(pinfo->ldi.v_pulse_width) << 16));
		outp32(dss_base + PDP_LDI_PLR_CTRL,
			pinfo->ldi.vsync_plr | (pinfo->ldi.hsync_plr << 1) |
			(pinfo->ldi.pixelclk_plr << 2) | (pinfo->ldi.data_en_plr << 3));
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
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

		init_dbuf(hisifd);

		outp32(dss_base + SDP_LDI_HRZ_CTRL0,
			pinfo->ldi.h_front_porch | (pinfo->ldi.h_back_porch << 16));
		outp32(dss_base + SDP_LDI_HRZ_CTRL1,
			DSS_WIDTH(pinfo->xres) | (DSS_WIDTH(pinfo->ldi.h_pulse_width) << 16));
		outp32(dss_base + SDP_LDI_VRT_CTRL0,
			pinfo->ldi.v_front_porch | (pinfo->ldi.v_back_porch << 16));
		outp32(dss_base + SDP_LDI_VRT_CTRL1,
			DSS_HEIGHT(pinfo->yres) | (DSS_HEIGHT(pinfo->ldi.v_pulse_width) << 16));
		outp32(dss_base + SDP_LDI_PLR_CTRL,
			pinfo->ldi.vsync_plr | (pinfo->ldi.hsync_plr << 1) |
			(pinfo->ldi.pixelclk_plr << 2) | (pinfo->ldi.data_en_plr << 3));
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return -EINVAL;
	}

	return 0;
}

static ssize_t dpe_lcd_model_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_model_show(pdev, buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_lcd_check_reg_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_check_reg(pdev, buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_lcd_mipi_detect_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_mipi_detect(pdev, buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_lcd_hkadc_debug_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_hkadc_debug_show(pdev, buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_lcd_hkadc_debug_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_hkadc_debug_store(pdev, buf, count);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_lcd_gram_check_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_gram_check_show(pdev, buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_lcd_gram_check_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_gram_check_store(pdev, buf, count);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_lcd_color_temperature_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = dpe_show_ct_cscValue(buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
	return ret;
}

static ssize_t dpe_lcd_color_temperature_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t flag = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	unsigned int csc_value[100];
	char *token, *cur;
	int i = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	cur = buf;
	while (token = strsep(&cur, ",")) {
		csc_value[i++] = simple_strtol(token, NULL, 0);
	}

	if (pinfo->color_temperature_support) {
		dpe_store_ct_cscValue(csc_value);
		flag = dpe_set_ct_cscValue(hisifd);
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return count;
}

static ssize_t dpe_led_rg_lcd_color_temperature_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = dpe_show_led_rg_ct_cscValue(buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
	return ret;
}

static ssize_t dpe_led_rg_lcd_color_temperature_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t flag = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	unsigned int csc_value[100];
	char *token, *cur;
	int i = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	cur = buf;
	while (token = strsep(&cur, ",")) {
		csc_value[i++] = simple_strtol(token, NULL, 0);
	}

	if (pinfo->color_temperature_support) {
		dpe_store_led_rg_ct_cscValue(csc_value);
		flag = dpe_set_led_rg_ct_cscValue(hisifd);
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return count;
}

static ssize_t dpe_lcd_comform_mode_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = dpe_show_comform_ct_cscValue(buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
	return ret;
}

static ssize_t dpe_lcd_comform_mode_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t flag = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	unsigned int val = 0;
	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
    
	if (pinfo->comform_mode_support) {
		val = simple_strtoul(buf, NULL, 0);           
		if(val <= COMFORM_MAX){       
		    HISI_FB_INFO("comform_mode = %d !\n", val);   
		    dpe_update_g_comform_discount(val);
		    flag = dpe_set_comform_ct_cscValue(hisifd);
		}else{
		    HISI_FB_ERR("comform_mode = %d, not support this level!\n", val);  
		}
	}	

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return count;
}

static ssize_t dpe_lcd_starlight_mode_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = dpe_show_starlight_ct_cscValue(buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
	return ret;
}

static ssize_t dpe_lcd_starlight_mode_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t flag = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	unsigned int val = 0;
	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (pinfo->starlight_mode_support) {  
		val = simple_strtoul(buf, NULL, 0);        
		if(val <= STARLIGHT_MAX){
		    HISI_FB_INFO("g_starlight_discount = %d !\n", val); 
		    dpe_update_g_starlight_discount(val);
		    flag = dpe_set_starlight_ct_cscValue(hisifd);
		}else{
		    HISI_FB_ERR("starlight_mode = %d, not support this level!\n", val); 
		}
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return count;
}
static ssize_t dpe_lcd_voltage_enable_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_voltage_enable_store(pdev, buf, count);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_lcd_bist_check(struct platform_device *pdev,
	char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_lcd_bist_check(pdev, buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_amoled_pcd_errflag_check(struct platform_device *pdev,
	char *buf)
{
	ssize_t ret = 0;

	ret = panel_next_amoled_pcd_errflag_check(pdev, buf);
	return ret;
}

static ssize_t dpe_amoled_acl_show(struct platform_device *pdev, char *buf)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_amoled_acl_show(pdev, buf);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_amoled_acl_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_amoled_acl_store(pdev, buf, count);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static ssize_t dpe_amoled_hbm_store(struct platform_device *pdev,
	const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);
	ret = panel_next_amoled_hbm_store(pdev, buf, count);
	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

/*******************************************************************************
**
*/
static int dpe_regulator_clk_irq_setup(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	struct dss_clk_rate *pdss_clk_rate = NULL;
	const char *regulator_name = NULL;
	const char *irq_name = NULL;
	irqreturn_t (*isr_fnc)(int irq, void *ptr);
	int ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);
	pdss_clk_rate = get_dss_clk_rate(hisifd);
	BUG_ON(pdss_clk_rate == NULL);

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

	hisifd->dss_axi_clk = devm_clk_get(&pdev->dev, hisifd->dss_axi_clk_name);
	if (IS_ERR(hisifd->dss_axi_clk)) {
		ret = PTR_ERR(hisifd->dss_axi_clk);
		return ret;
	}

	hisifd->dss_pclk_clk = devm_clk_get(&pdev->dev, hisifd->dss_pclk_clk_name);
	if (IS_ERR(hisifd->dss_pclk_clk)) {
		ret = PTR_ERR(hisifd->dss_pclk_clk);
		return ret;
	} else {
		ret = clk_set_rate(hisifd->dss_pclk_clk, pdss_clk_rate->dss_pclk_clk_rate);
		if (ret < 0) {
			HISI_FB_ERR("fb%d dss_pclk_clk clk_set_rate(%llu) failed, error=%d!\n",
				hisifd->index, pdss_clk_rate->dss_pclk_clk_rate, ret);
			return -EINVAL;
		}

		HISI_FB_INFO("dss_pclk_clk:[%llu]->[%llu].\n",
			pdss_clk_rate->dss_pclk_clk_rate, (uint64_t)clk_get_rate(hisifd->dss_pclk_clk));
	}

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		hisifd->dss_pri_clk = devm_clk_get(&pdev->dev, hisifd->dss_pri_clk_name);
		if (IS_ERR(hisifd->dss_pri_clk)) {
			ret = PTR_ERR(hisifd->dss_pri_clk);
			return ret;
		} else {
			ret = clk_set_rate(hisifd->dss_pri_clk, pdss_clk_rate->dss_pri_clk_rate);
			if (ret < 0) {
				HISI_FB_ERR("fb%d dss_pri_clk clk_set_rate(%llu) failed, error=%d!\n",
					hisifd->index, pdss_clk_rate->dss_pri_clk_rate, ret);
				return -EINVAL;
			}

			HISI_FB_INFO("dss_pri_clk:[%llu]->[%llu].\n",
				pdss_clk_rate->dss_pri_clk_rate, (uint64_t)clk_get_rate(hisifd->dss_pri_clk));
		}

		hisifd->dss_pxl0_clk = devm_clk_get(&pdev->dev, hisifd->dss_pxl0_clk_name);
		if (IS_ERR(hisifd->dss_pxl0_clk)) {
			ret = PTR_ERR(hisifd->dss_pxl0_clk);
			return ret;
		} else {
			ret = clk_set_rate(hisifd->dss_pxl0_clk, pinfo->pxl_clk_rate);
			if (ret < 0) {
				HISI_FB_ERR("fb%d dss_pxl0_clk clk_set_rate(%llu) failed, error=%d!\n",
					hisifd->index, pinfo->pxl_clk_rate, ret);
				return -EINVAL;
			}

			HISI_FB_INFO("dss_pxl0_clk:[%llu]->[%llu].\n",
				pinfo->pxl_clk_rate, (uint64_t)clk_get_rate(hisifd->dss_pxl0_clk));
		}
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		hisifd->dss_pxl1_clk = devm_clk_get(&pdev->dev, hisifd->dss_pxl1_clk_name);
		if (IS_ERR(hisifd->dss_pxl1_clk)) {
			ret = PTR_ERR(hisifd->dss_pxl1_clk);
			return ret;
		} else {
			ret = clk_set_rate(hisifd->dss_pxl1_clk, pinfo->pxl_clk_rate);
			if (ret < 0) {
				HISI_FB_ERR("fb%d dss_pxl1_clk clk_set_rate(%llu) failed, error=%d!\n",
					hisifd->index, pinfo->pxl_clk_rate, ret);
				return -EINVAL;
			}

			HISI_FB_INFO("dss_pxl1_clk:[%llu]->[%llu].\n",
				pinfo->pxl_clk_rate, (uint64_t)clk_get_rate(hisifd->dss_pxl1_clk));
		}
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		hisifd->dss_aux_clk = devm_clk_get(&pdev->dev, hisifd->dss_aux_clk_name);
		if (IS_ERR(hisifd->dss_aux_clk)) {
			ret = PTR_ERR(hisifd->dss_aux_clk);
			return ret;
		} else {
			ret = clk_set_rate(hisifd->dss_aux_clk, pdss_clk_rate->dss_aux_clk_rate);
			if (ret < 0) {
				HISI_FB_ERR("fb%d dss_aux_clk clk_set_rate(%llu) failed, error=%d!\n",
					hisifd->index, pdss_clk_rate->dss_aux_clk_rate, ret);
				return -EINVAL;
			}

			HISI_FB_INFO("dss_aux_clk:[%llu]->[%llu].\n",
				pdss_clk_rate->dss_aux_clk_rate, (uint64_t)clk_get_rate(hisifd->dss_aux_clk));
		}
	} else {
		ret = -1;
		HISI_FB_ERR("fb%d, not support this device!\n", hisifd->index);
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
	pdata->lcd_model_show = dpe_lcd_model_show;
	pdata->lcd_check_reg = dpe_lcd_check_reg_show;
	pdata->lcd_mipi_detect = dpe_lcd_mipi_detect_show;
	pdata->lcd_hkadc_debug_show = dpe_lcd_hkadc_debug_show;
	pdata->lcd_hkadc_debug_store = dpe_lcd_hkadc_debug_store;
	pdata->lcd_gram_check_show = dpe_lcd_gram_check_show;
	pdata->lcd_gram_check_store = dpe_lcd_gram_check_store;
	pdata->lcd_color_temperature_show = dpe_lcd_color_temperature_show;
	pdata->lcd_color_temperature_store = dpe_lcd_color_temperature_store;
	pdata->led_rg_lcd_color_temperature_show = dpe_led_rg_lcd_color_temperature_show;
	pdata->led_rg_lcd_color_temperature_store = dpe_led_rg_lcd_color_temperature_store;
	pdata->lcd_comform_mode_show = dpe_lcd_comform_mode_show;
	pdata->lcd_comform_mode_store = dpe_lcd_comform_mode_store;
	pdata->lcd_starlight_mode_show = dpe_lcd_starlight_mode_show;
	pdata->lcd_starlight_mode_store = dpe_lcd_starlight_mode_store;
	pdata->lcd_voltage_enable_store = dpe_lcd_voltage_enable_store;
	pdata->lcd_bist_check = dpe_lcd_bist_check;
	pdata->amoled_pcd_errflag_check = dpe_amoled_pcd_errflag_check;
	pdata->amoled_acl_show = dpe_amoled_acl_show;
	pdata->amoled_acl_store = dpe_amoled_acl_store;
	pdata->amoled_hbm_store = dpe_amoled_hbm_store;
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
	dpe_init_ct_cscValue();
	dpe_init_led_rg_ct_cscValue();

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
