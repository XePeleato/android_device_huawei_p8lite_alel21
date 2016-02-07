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

#include "hisi_dpe_utils.h"


struct dss_clk_rate * get_dss_clk_rate(struct hisi_fb_data_type *hisifd)
{
	struct hisi_panel_info *pinfo = NULL;
	struct dss_clk_rate *pdss_clk_rate = NULL;

	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);
	pdss_clk_rate = &(hisifd->dss_clk_rate);

	/* FIXME: TBD  */
	if (pdss_clk_rate->dss_pclk_clk_rate == 0) {
		if ((pinfo->width * pinfo->height) >= (2560 * 1600)) {
			pdss_clk_rate->dss_pri_clk_rate = 300 * 1000000UL;
			pdss_clk_rate->dss_aux_clk_rate = 300 * 1000000UL;
			pdss_clk_rate->dss_pclk_clk_rate = 120 * 1000000UL;
		} else if ((pinfo->width * pinfo->height) >= (1920 * 1080)) {
			pdss_clk_rate->dss_pri_clk_rate = 300 * 1000000UL;
			pdss_clk_rate->dss_aux_clk_rate = 300 * 1000000UL;
			pdss_clk_rate->dss_pclk_clk_rate = 120 * 1000000UL;
		} else if ((pinfo->width * pinfo->height) >= (1280 * 720)) {
			pdss_clk_rate->dss_pri_clk_rate = 80 * 1000000UL;
			pdss_clk_rate->dss_aux_clk_rate = 80 * 1000000UL;
			pdss_clk_rate->dss_pclk_clk_rate = 120 * 1000000UL;
		} else {
			pdss_clk_rate->dss_pri_clk_rate = 300 * 1000000UL;
			pdss_clk_rate->dss_aux_clk_rate = 300 * 1000000UL;
			pdss_clk_rate->dss_pclk_clk_rate = 120 * 1000000UL;
		}
	}

	return pdss_clk_rate;
}

int set_dss_clk_rate(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);


	return ret;
}

void init_dpp(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dpp_base = NULL;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		dpp_base = hisifd->dss_base + DSS_DPP_OFFSET;
		hisifd->dss_flush_bits |= BIT_DPP0_FLUSH_EN;
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	set_reg(dpp_base + DPP_IMG_HRZ, DSS_WIDTH(hisifd->panel_info.xres), 13, 0);
	set_reg(dpp_base + DPP_IMG_VRT, DSS_HEIGHT(hisifd->panel_info.yres), 13, 0);
}

void init_sbl(struct hisi_fb_data_type *hisifd)
{
	char __iomem *sbl_base = NULL;
	uint32_t tmp = 0;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		sbl_base = hisifd->dss_base + DSS_DPP_SBL_OFFSET;
		hisifd->dss_flush_bits |= BIT_DPP0_FLUSH_EN;
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	if (is_mipi_cmd_panel(hisifd)) {
		outp32(sbl_base + SBL_CONFIG_BUFFER_MODE, 0x2);
	} else {
		outp32(sbl_base + SBL_CONFIG_BUFFER_MODE, 0x0);
	}

	outp32(sbl_base + SBL_AUTOMATIC_START_CALC_STRENGTH_DRC_BACKLIGHT_SEL, 0x80);
	tmp = hisifd->panel_info.xres;
	outp32(sbl_base + SBL_FRAME_WIDTH_L, (tmp & 0xff));
	outp32(sbl_base + SBL_FRAME_WIDTH_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.yres;
	outp32(sbl_base + SBL_FRAME_HEIGHT_L, (tmp & 0xff));
	outp32(sbl_base + SBL_FRAME_HEIGHT_H, ((tmp >> 8) & 0xff));
	outp32(sbl_base + SBL_APICAL_DITHER, 0x0);
	tmp = hisifd->bl_level;
	outp32(sbl_base + SBL_BACKLIGHT_L, (tmp & 0xff));
	outp32(sbl_base + SBL_BACKLIGHT_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.backlight_scale;
	outp32(sbl_base + SBL_BACKLIGHT_SCALE_L, (tmp & 0xff));
	outp32(sbl_base + SBL_BACKLIGHT_SCALE_H, (tmp >> 8) & 0xff);
	tmp = hisifd->panel_info.smart_bl.strength_limit;
	outp32(sbl_base + SBL_STRENGTH_LIMIT, tmp);
	tmp = hisifd->panel_info.smart_bl.calibration_a;
	outp32(sbl_base + SBL_CALIBRATION_A_L, (tmp & 0xff));
	outp32(sbl_base + SBL_CALIBRATION_A_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.calibration_b;
	outp32(sbl_base + SBL_CALIBRATION_B_L, (tmp & 0xff));
	outp32(sbl_base + SBL_CALIBRATION_B_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.calibration_c;
	outp32(sbl_base + SBL_CALIBRATION_C_L, (tmp & 0xff));
	outp32(sbl_base + SBL_CALIBRATION_C_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.calibration_d;
	outp32(sbl_base + SBL_CALIBRATION_D_L, (tmp & 0xff));
	outp32(sbl_base + SBL_CALIBRATION_D_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.t_filter_control;
	outp32(sbl_base + SBL_T_FILTER_CONTROL, tmp);
	tmp = hisifd->panel_info.smart_bl.backlight_min;
	outp32(sbl_base + SBL_BACKLIGHT_MIN_L, (tmp & 0xff));
	outp32(sbl_base + SBL_BACKLIGHT_MIN_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.backlight_max;
	outp32(sbl_base + SBL_BACKLIGHT_MAX_L, (tmp & 0xff));
	outp32(sbl_base + SBL_BACKLIGHT_MAX_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.ambient_light_min;
	outp32(sbl_base + SBL_AMBIENT_LIGHT_MIN_L, (tmp & 0xff));
	outp32(sbl_base + SBL_AMBIENT_LIGHT_MIN_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.filter_a;
	outp32(sbl_base + SBL_FILTER_A_L, (tmp & 0xff));
	outp32(sbl_base + SBL_FILTER_A_H, ((tmp >> 8) & 0xff));
	tmp = hisifd->panel_info.smart_bl.filter_b;
	outp32(sbl_base + SBL_FILTER_B, tmp);
	tmp = hisifd->panel_info.smart_bl.logo_left;
	outp32(sbl_base + SBL_LOGO_LEFT, tmp);
	tmp = hisifd->panel_info.smart_bl.logo_top;
	outp32(sbl_base + SBL_LOGO_TOP, tmp);
	outp32(sbl_base + SBL_ENABLE, 0x0);

	if (hisifd->panel_info.sbl_support == 1) {
		outp32(sbl_base + SBL_ENABLE, 0x1);
	} else {
		outp32(sbl_base + SBL_ENABLE, 0x0);
	}
}

void init_ifbc(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ifbc_base = NULL;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		ifbc_base = hisifd->dss_base + DSS_IFBC_OFFSET;
		hisifd->dss_flush_bits |= BIT_DPP0_FLUSH_EN;
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	set_reg(ifbc_base + IFBC_SIZE,
		((DSS_WIDTH(hisifd->panel_info.xres) << 16) |
		DSS_HEIGHT(hisifd->panel_info.yres)), 32, 0);
	set_reg(ifbc_base + IFBC_CTRL, hisifd->panel_info.ifbc_type, 10, 0);
	if (hisifd->panel_info.ifbc_type != IFBC_TYPE_NON) {
		set_reg(ifbc_base + IFBC_EN, 0x3, 2, 0);
	} else {
		set_reg(ifbc_base + IFBC_EN, 0x0, 2, 0);
	}
}

static int get_dfs_sram_valid_num(struct hisi_fb_data_type *hisifd)
{
	int sram_valid_num = 0;
	int frame_size = 0;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);
	frame_size = pinfo->xres * pinfo->yres;

	sram_valid_num =  (frame_size <= RES_720P) ? 0
		: (frame_size <= RES_1080P) ? 1
		: 2;

	return sram_valid_num;
}

void init_dbuf(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dbuf_base = NULL;
	struct hisi_panel_info *pinfo = NULL;
	int sram_valid_num = 0;
	int sram_max_mem_depth = 0;

	uint32_t thd_rqos_in = 0;
	uint32_t thd_rqos_out = 0;
	uint32_t thd_wqos_in = 0;
	uint32_t thd_wqos_out = 0;
	uint32_t thd_cg_in = 0;
	uint32_t thd_cg_out = 0;
	uint32_t thd_wr_wait = 0;
	uint32_t thd_cg_hold = 0;
	uint32_t thd_fill_lev1 = 0;
	uint32_t thd_fill_lev2 = 0;
	uint32_t thd_fill_lev3 = 0;
	uint32_t thd_fill_lev4 = 0;

	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);

	sram_valid_num = get_dfs_sram_valid_num(hisifd);
	sram_max_mem_depth = (sram_valid_num + 1) * DBUF_SRAM_ONE_MEM_DEPTH;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		dbuf_base = hisifd->dss_base + DSS_DBUF0_OFFSET;
		hisifd->dss_flush_bits |= BIT_DBUF0_FLUSH_EN;

		if (sram_valid_num == 0) {
			thd_rqos_in = 0x30C;
			thd_rqos_out = 0x352;
			thd_wqos_in = 0x420;
			thd_wqos_out = 0x160;
			thd_cg_in = 0x57F;
			thd_cg_out = 0x352;
			thd_cg_hold = 0x20;
			thd_wr_wait = 0x352;
			thd_fill_lev1 = 0x11A;
			thd_fill_lev2 = 0x234;
			thd_fill_lev3 = 0x34D;
			thd_fill_lev4 = 0x467;
		} else if (sram_valid_num == 1) {
			thd_rqos_in = 0x898;
			thd_rqos_out = 0x9C0;
			thd_wqos_in = 0x840;
			thd_wqos_out = 0x2C0;
			thd_cg_in = 0xAFF;
			thd_cg_out = 0x9C0;
			thd_cg_hold = 0x20;
			thd_wr_wait = 0x8DA;
			thd_fill_lev1 = 0x234;
			thd_fill_lev2 = 0x467;
			thd_fill_lev3 = 0x69A;
			thd_fill_lev4 = 0x8CD;
		} else {
			thd_rqos_in = 0xD9F;
			thd_rqos_out = 0xF23;
			thd_wqos_in = 0xC7D;
			thd_wqos_out = 0x42A;
			thd_cg_in = 0x107F;
			thd_cg_out = 0xFFF;
			thd_cg_hold = 0x20;
			thd_wr_wait = 0xF23;
			thd_fill_lev1 = 0x34D;
			thd_fill_lev2 = 0x69A;
			thd_fill_lev3 = 0x9E7;
			thd_fill_lev4 = 0xD34;
		}
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		dbuf_base = hisifd->dss_base + DSS_DBUF1_OFFSET;
		hisifd->dss_flush_bits |= BIT_DBUF1_FLUSH_EN;

		thd_rqos_in = 0x1B4;
		thd_rqos_out = 0x1E7;
		thd_wqos_in = 0x180;
		thd_wqos_out = 0x80;
		thd_cg_in = 0x3FF;
		thd_cg_out = 0x3FF;
		thd_wr_wait = 0x19A;
		thd_cg_hold = 0x20;
		thd_fill_lev1 = 0x67;
		thd_fill_lev2 = 0xCD;
		thd_fill_lev3 = 0x134;
		thd_fill_lev4 = 0x19A;
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	outp32(dbuf_base + DBUF_FRM_SIZE, pinfo->xres * pinfo->yres);
	outp32(dbuf_base + DBUF_FRM_HSIZE, DSS_WIDTH(pinfo->xres));
	outp32(dbuf_base + DBUF_SRAM_VALID_NUM, sram_valid_num);

	outp32(dbuf_base + DBUF_THD_RQOS, (thd_rqos_out<< 16) | thd_rqos_in);
	outp32(dbuf_base + DBUF_THD_WQOS, (thd_wqos_out << 16) | thd_wqos_in);
	outp32(dbuf_base + DBUF_THD_CG, (thd_cg_out << 16) | thd_cg_in);
	outp32(dbuf_base + DBUF_THD_OTHER, (thd_cg_hold << 16) | thd_wr_wait);
	outp32(dbuf_base + DBUF_THD_FILL_LEV0, (thd_fill_lev2 << 16) | thd_fill_lev1);
	outp32(dbuf_base + DBUF_FILL_LEV1_CNT, (thd_fill_lev4 << 16) | thd_fill_lev3);

	/* dfs_core_gt: use default value */
	/*set_reg(dbuf_base + DBUF_DFS_LP_CTRL, 0, 2, 26);*/
	/* ret_aft_prefetch: use default value */
	/*set_reg(dbuf_base + DBUF_DFS_LP_CTRL, 1, 1, 25);*/
	/* pd_upon_frm_end: use default value */
	/*set_reg(dbuf_base + DBUF_DFS_LP_CTRL, 1, 1, 24);*/
}

void init_ldi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ldi_base = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		ldi_base = hisifd->dss_base + DSS_LDI0_OFFSET;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		ldi_base = hisifd->dss_base + DSS_LDI1_OFFSET;

	#ifdef CONFIG_FPGA_SDP_TEST
		set_reg(hisifd->dss_base + GLB_TP_SEL, 0x2, 2, 0);
	#endif
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	if (is_dual_mipi_panel(hisifd)) {
		outp32(ldi_base + LDI_DPI0_HRZ_CTRL0,
			pinfo->ldi.h_front_porch | (pinfo->ldi.h_back_porch << 16));
		outp32(ldi_base + LDI_DPI0_HRZ_CTRL1, DSS_WIDTH(pinfo->ldi.h_pulse_width));
		outp32(ldi_base + LDI_DPI0_HRZ_CTRL2, DSS_WIDTH(pinfo->xres / 2));

		outp32(ldi_base + LDI_DPI1_HRZ_CTRL0,
			pinfo->ldi.h_back_porch << 16);
		outp32(ldi_base + LDI_DPI1_HRZ_CTRL1, DSS_WIDTH(pinfo->ldi.h_pulse_width));
		outp32(ldi_base + LDI_DPI1_HRZ_CTRL2, DSS_WIDTH(pinfo->xres / 2));

		outp32(ldi_base + LDI_OVERLAP_SIZE,
			pinfo->ldi.dpi0_overlap_size | (pinfo->ldi.dpi1_overlap_size << 16));

		/* dual_mode_en */
		set_reg(ldi_base + LDI_CTRL, 1, 1, 5);

		/* split mode */
		set_reg(ldi_base + LDI_CTRL, 0, 1, 16);
	} else {
		outp32(ldi_base + LDI_DPI0_HRZ_CTRL0,
			pinfo->ldi.h_front_porch | (pinfo->ldi.h_back_porch << 16));
		outp32(ldi_base + LDI_DPI0_HRZ_CTRL1, DSS_WIDTH(pinfo->ldi.h_pulse_width));
		outp32(ldi_base + LDI_DPI0_HRZ_CTRL1, DSS_WIDTH(pinfo->xres));
	}
	outp32(ldi_base + LDI_VRT_CTRL0,
		pinfo->ldi.v_front_porch | (pinfo->ldi.v_back_porch << 16));
	outp32(ldi_base + LDI_VRT_CTRL1, DSS_HEIGHT(pinfo->ldi.v_pulse_width));
	outp32(ldi_base + LDI_VRT_CTRL2, DSS_HEIGHT(pinfo->yres));

	outp32(ldi_base + LDI_PLR_CTRL,
		pinfo->ldi.vsync_plr | (pinfo->ldi.hsync_plr << 1) |
		(pinfo->ldi.pixelclk_plr << 2) | (pinfo->ldi.data_en_plr << 3));

	/* ldi disable */
	set_reg(ldi_base + LDI_CTRL, 0, 1, 0);
	/* data_gate_en: mipi command LCD open */
	if (is_mipi_cmd_panel(hisifd)) {
		//set_reg(ldi_base + LDI_CTRL, 0x1, 1, 2);
		set_reg(ldi_base + LDI_FRM_MSK, 0x1, 1, 0);
	}
	/* bpp */
	set_reg(ldi_base + LDI_CTRL, pinfo->bpp, 2, 3);
	/* bgr */
	set_reg(ldi_base + LDI_CTRL, pinfo->bgr_fmt, 1, 13);
	/* color mode: for mipi dsi */
	/*set_reg(dss_base + LDI_CTRL, 0, 1, 14);*/
	/* shutdown: for mipi dsi */
	/*set_reg(dss_base + LDI_CTRL, 0, 1, 15);*/

#ifdef CONFIG_HISI_FB_COLORBAR_USED
	/* colorbar width */
	set_reg(ldi_base + LDI_CTRL, DSS_WIDTH(0x3c), 7, 6);
	/* colorbar ort */
	set_reg(ldi_base + LDI_WORK_MODE, 0, 1, 1);
	/* colorbar enable */
	set_reg(ldi_base + LDI_WORK_MODE, 0, 1, 0);
#else
	/* normal */
	outp32(ldi_base + LDI_WORK_MODE, 0x1);
#endif
	/* for ddr pmqos */
	outp32(ldi_base + LDI_VINACT_MSK_LEN,
		pinfo->ldi.v_front_porch);
	outp32(ldi_base + LDI_CG_CTRL, 0x0);
	/* use default value */
	/*outp32(ldi_base + LDI_SRAM_LP_CTRL, );*/

	/* for 3D frame by frame */
	/*outp32(ldi_base + LDI_3D_CTRL, );
	outp32(ldi_base + LDI_DE_SPACE_LOW, );
	set_reg(ldi_base + LDI_CTRL, 1, 1, 1);*/

	/* for 1Hz LCD and mipi command LCD */
	/*outp32(ldi_base + LDI_FRM_MSK, );*/
	if (is_mipi_cmd_panel(hisifd))
	{
		set_reg(ldi_base + LDI_DSI_CMD_MOD_CTRL, 0x1, 1, 0);
		set_reg(ldi_base + LDI_DSI_TE_CTRL, 0x4001, 32, 0);
		set_reg(ldi_base + LDI_DSI_TE_HS_NUM, 0x0, 32, 0);
		set_reg(ldi_base + LDI_DSI_TE_HS_WD, 0xF0F, 32, 0);
		set_reg(ldi_base + LDI_DSI_TE_VS_WD, 0x3FC0FF, 32, 0);
	}
}

void deinit_ldi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ldi_base = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		ldi_base = hisifd->dss_base + DSS_LDI0_OFFSET;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		ldi_base = hisifd->dss_base + DSS_LDI1_OFFSET;
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	set_reg(ldi_base + LDI_CTRL, 0, 1, 0);
}

void enable_ldi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ldi_base = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		ldi_base = hisifd->dss_base + DSS_LDI0_OFFSET;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		ldi_base = hisifd->dss_base + DSS_LDI1_OFFSET;
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	/* ldi enable */
	set_reg(ldi_base + LDI_CTRL, 1, 1, 0);
}

void single_frame_update(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ldi_base = NULL;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		ldi_base = hisifd->dss_base + DSS_LDI0_OFFSET;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		ldi_base = hisifd->dss_base + DSS_LDI1_OFFSET;
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	if (is_mipi_cmd_panel(hisifd)) {
		set_reg(ldi_base + LDI_FRM_MSK, 1, 1, 24);
	}

	if (hisifd->vsync_ctrl.vsync_report_fnc) {
		atomic_inc(&(hisifd->vsync_ctrl.buffer_updated));
	}
}
