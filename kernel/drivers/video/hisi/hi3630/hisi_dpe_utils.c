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


DEFINE_SEMAPHORE(hisi_fb_dss_common_clk_sem);
DEFINE_SEMAPHORE(hisi_fb_dss_panel_clk_sem);
DEFINE_SEMAPHORE(hisi_fb_dss_dpe2_clk_sem);
DEFINE_SEMAPHORE(hisi_fb_dss_rot1_clk_sem);
DEFINE_SEMAPHORE(hisi_fb_dss_axi_clk_sem);

static int common_refcount;
static int panel_refcount;
static int dpe2_refcount;
static int rot1_refcount;
static int axi_refcount;
static unsigned int g_csc_value[9];
static unsigned int g_is_csc_set;
static unsigned int g_led_rg_csc_value[9];
static unsigned int g_is_led_rg_csc_set;
static unsigned  int g_comform_value = 0;
static unsigned  int g_starlight_value = 0;

struct dss_clk_rate * get_dss_clk_rate(struct hisi_fb_data_type *hisifd)
{
	struct hisi_panel_info *pinfo = NULL;
	struct dss_clk_rate *pdss_clk_rate = NULL;

	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);
	pdss_clk_rate = &(hisifd->dss_clk_rate);

	/* FIXME: TBD  */
	if (g_fpga_flag == 1) {
		if (pdss_clk_rate->dss_pclk_clk_rate == 0) {
			pdss_clk_rate->dss_pri_clk_rate = 40 * 1000000UL;
			pdss_clk_rate->dss_aux_clk_rate = 40 * 1000000UL;
			pdss_clk_rate->dss_pclk_clk_rate = 20 * 1000000UL;
		}
	} else {
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
	}

	return pdss_clk_rate;
}

int set_dss_clk_rate(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	if (hisifd->dss_pri_clk && (hisifd->dss_clk_rate.dss_pri_clk_rate > 0)) {
		if (hisifd->dss_clk_rate.dss_pri_clk_rate > DSS_PRI_CLK_RATE_MAX)
			hisifd->dss_clk_rate.dss_pri_clk_rate = DSS_PRI_CLK_RATE_MAX;

		ret = clk_set_rate(hisifd->dss_pri_clk, hisifd->dss_clk_rate.dss_pri_clk_rate);
		if (ret < 0) {
			HISI_FB_ERR("fb%d dss_pri_clk clk_set_rate(%llu) failed, error=%d!\n",
				hisifd->index, hisifd->dss_clk_rate.dss_pri_clk_rate, ret);
			return -EINVAL;
		}

		//HISI_FB_INFO("dss_pri_clk:[%llu]->[%llu].\n",
		//	hisifd->dss_clk_rate.dss_pri_clk_rate, clk_get_rate(hisifd->dss_pri_clk));
	}

	if (hisifd->dss_aux_clk && (hisifd->dss_clk_rate.dss_aux_clk_rate > 0)) {
		if (hisifd->dss_clk_rate.dss_aux_clk_rate > DSS_AUX_CLK_RATE_MAX)
			hisifd->dss_clk_rate.dss_aux_clk_rate = DSS_AUX_CLK_RATE_MAX;

		ret = clk_set_rate(hisifd->dss_aux_clk, hisifd->dss_clk_rate.dss_aux_clk_rate);
		if (ret < 0) {
			HISI_FB_ERR("fb%d dss_aux_clk_rate clk_set_rate(%llu) failed, error=%d!\n",
				hisifd->index, hisifd->dss_clk_rate.dss_aux_clk_rate, ret);
			return -EINVAL;
		}

		//HISI_FB_INFO("dss_aux_clk:[%lu]->[%lu].\n",
		//	hisifd->dss_clk_rate.dss_aux_clk_rate, clk_get_rate(hisifd->dss_aux_clk));
	}

	if (hisifd->dss_pclk_clk && (hisifd->dss_clk_rate.dss_pclk_clk_rate > 0)) {
		if (hisifd->dss_clk_rate.dss_pclk_clk_rate > DSS_PCLK_CLK_RATE_MAX)
			hisifd->dss_clk_rate.dss_pclk_clk_rate = DSS_PCLK_CLK_RATE_MAX;

		ret = clk_set_rate(hisifd->dss_pclk_clk, hisifd->dss_clk_rate.dss_pclk_clk_rate);
		if (ret < 0) {
			HISI_FB_ERR("fb%d dss_pclk_clk clk_set_rate(%llu) failed, error=%d!\n",
				hisifd->index, hisifd->dss_clk_rate.dss_pclk_clk_rate, ret);
			return -EINVAL;
		}

		//HISI_FB_INFO("dss_pclk_clk:[%llu]->[%llu].\n",
		//	hisifd->dss_clk_rate.dss_pclk_clk_rate, clk_get_rate(hisifd->dss_pclk_clk));
	}

	return ret;
}

void init_dpp(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dpp_base = NULL;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		dpp_base = hisifd->dss_base + DSS_DPP0_CTRL_OFFSET;
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	set_reg(dpp_base + DPP_IMG_HRZ, DSS_WIDTH(hisifd->panel_info.xres), 13, 0);
	set_reg(dpp_base + DPP_IMG_VRT, DSS_HEIGHT(hisifd->panel_info.yres), 13, 0);

	if (hisifd->panel_info.dirty_region_updt_support) {
		if (is_mipi_cmd_panel(hisifd)) {
			outp32(dpp_base + DPP_CLK_GT, 0xA);
		}
	}
}

void init_sbl(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dpp_base = NULL;
	char __iomem *sbl_base = NULL;
	uint32_t tmp = 0;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		dpp_base = hisifd->dss_base + DSS_DPP0_CTRL_OFFSET;
		sbl_base = hisifd->dss_base + DSS_DPP_SBL_OFFSET;
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
		outp32(dpp_base + DPP_SBL, 0x1);
	} else {
		outp32(dpp_base + DPP_SBL, 0x0);
	}
}

inline void acm_set_lut(char __iomem *address, uint32_t table[], uint32_t size)
{
	uint32_t data = 0;
	uint32_t index = 0;
	uint32_t i;
	size /= 2;
	for (i = 0; i < size; i++) {
		index = i << 1;
		data = table[index] + (table[index + 1] << 8);
		outp32(address + (i << 2), data);
	}
}

inline void acm_set_lut_hue(char __iomem *address, uint32_t table[], uint32_t size)
{
	uint32_t data = 0;
	uint32_t index = 0;
	uint32_t i;
	size /= 2;
	for (i = 0; i < size; i++) {
		index = i << 1;
		data = table[index] + (table[index + 1] << 16);
		outp32(address + (i << 2), data);
	}
}

void init_acm(struct hisi_fb_data_type *hisifd)
{
	char __iomem *acm_base = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);
	if(pinfo->acm_support != 1){
		HISI_FB_DEBUG("fb%d, not support acm!\n", hisifd->index);
		return;
	}

	acm_base = hisifd->dss_base + DSS_DPP_ACM_OFFSET;

	set_reg(acm_base + ACM_EN, 0x0, 1, 0);
	set_reg(acm_base + ACM_R0_H, ((pinfo->r0_hh&0x3ff)<<16)|(pinfo->r0_lh&0x3ff), 32, 0);
	set_reg(acm_base + ACM_R1_H, ((pinfo->r1_hh&0x3ff)<<16)|(pinfo->r1_lh&0x3ff), 32, 0);
	set_reg(acm_base + ACM_R2_H, ((pinfo->r2_hh&0x3ff)<<16)|(pinfo->r2_lh&0x3ff), 32, 0);
	set_reg(acm_base + ACM_R3_H, ((pinfo->r3_hh&0x3ff)<<16)|(pinfo->r3_lh&0x3ff), 32, 0);
	set_reg(acm_base + ACM_R4_H, ((pinfo->r4_hh&0x3ff)<<16)|(pinfo->r4_lh&0x3ff), 32, 0);
	set_reg(acm_base + ACM_R5_H, ((pinfo->r5_hh&0x3ff)<<16)|(pinfo->r5_lh&0x3ff), 32, 0);
	set_reg(acm_base + ACM_R6_H, ((pinfo->r6_hh&0x3ff)<<16)|(pinfo->r6_lh&0x3ff), 32, 0);

	if (pinfo->acm_lut_hue_table && pinfo->acm_lut_hue_table_len > 0) {
		acm_set_lut_hue(acm_base + ACM_LUT_HUE, pinfo->acm_lut_hue_table, pinfo->acm_lut_hue_table_len);
	} else {
		HISI_FB_ERR("fb%d, acm_lut_hue_table is NULL or acm_lut_hue_table_len less than 0!\n", hisifd->index);
		return;
	}
	if (pinfo->acm_lut_value_table && pinfo->acm_lut_value_table_len > 0) {
		acm_set_lut(acm_base + ACM_LUT_VALUE, pinfo->acm_lut_value_table, pinfo->acm_lut_value_table_len);
	} else {
		HISI_FB_ERR("fb%d, acm_lut_value_table is NULL or acm_lut_value_table_len less than 0!\n", hisifd->index);
		return;
	}
	if (pinfo->acm_lut_sata_table && pinfo->acm_lut_sata_table_len > 0) {
		acm_set_lut(acm_base + ACM_LUT_SATA, pinfo->acm_lut_sata_table, pinfo->acm_lut_sata_table_len);
	} else {
		HISI_FB_ERR("fb%d, acm_lut_sata_table is NULL or acm_lut_sata_table_len less than 0!\n", hisifd->index);
		return;
	}
	if (pinfo->acm_lut_satr_table && pinfo->acm_lut_satr_table_len > 0) {
		acm_set_lut(acm_base + ACM_LUT_SATR, pinfo->acm_lut_satr_table, pinfo->acm_lut_satr_table_len);
	} else {
		HISI_FB_ERR("fb%d, acm_lut_satr_table is NULL or acm_lut_satr_table_len less than 0!\n", hisifd->index);
		return;
	}

	set_reg(acm_base + ACM_EN, 0x1, 1, 0);
	set_reg(acm_base + ACM_EN, pinfo->acm_valid_num & 0x7, 3, 4);
}

void init_xcc_and_gama(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;
	struct hisi_panel_info *pinfo = NULL;
	unsigned int i;

	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);
	if(pinfo->color_temperature_support != 1){
		HISI_FB_INFO("The panel do not need AP's color temperature.\n");
		return;
	}
	HISI_FB_INFO("start init xcc and gamma.\n");
	dss_base = hisifd->dss_base;

	set_reg(dss_base + DSS_DPP_GAMMA_OFFSET + GAMMA_BYPASS_EN, 0x0, 1, 0);//gamma enable
	set_reg(dss_base + DSS_DPP_GAMMA_OFFSET + GAMMA_CORE_GT, 0x2, 32, 0);//lut clock enable

	set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_BYPASS_EN, 0x0, 1, 0);//xcc enable
	set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_IGM_GT, 0x2, 32, 0);//xcc clock enable

	for(i = 0; i < 256; i++)
	{
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + (LCP_IGM_R_LUT + i * 4), i * 16, 32, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + (LCP_IGM_G_LUT + i * 4), i * 16, 32, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + (LCP_IGM_B_LUT + i * 4), i * 16, 32, 0);

		set_reg(dss_base + DSS_DPP_GAMMA_OFFSET + (GAMMA_R_LUT + i * 4), i, 32, 0);
		set_reg(dss_base + DSS_DPP_GAMMA_OFFSET + (GAMMA_G_LUT + i * 4), i, 32, 0);
		set_reg(dss_base + DSS_DPP_GAMMA_OFFSET + (GAMMA_B_LUT + i * 4), i, 32, 0);
	}
}

void init_ifbc(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ifbc_base = NULL;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		ifbc_base = hisifd->dss_base + DSS_DPP_IFBC_OFFSET;
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

#if 0
	set_reg(ifbc_base + IFBC_Himax_CTRL0, 32, 0);
	set_reg(ifbc_base + IFBC_Himax_CTRL1, 32, 0);
	set_reg(ifbc_base + IFBC_Himax_CTRL2, 32, 0);
	set_reg(ifbc_base + IFBC_Himax_CTRL3, 32, 0);
	set_reg(ifbc_base + IFBC_PM_CTRL, 32, 0);
	set_reg(ifbc_base + IFBC_MEM_CTRL, 32, 0);
	set_reg(ifbc_base + IFBC_HIMAX_TEST_MODE, 32, 0);
	set_reg(ifbc_base + IFBC_CORE_GT, 32, 0);
#endif
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
	sram_max_mem_depth = (sram_valid_num + 1) * DFS_SRAM_ONE_MEM_DEPTH;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		dbuf_base = hisifd->dss_base + DSS_DPE0_OFFSET;

		if (g_fpga_flag == 1) {
			thd_rqos_in = GET_THD_RQOS_IN(sram_max_mem_depth);
			thd_rqos_out = GET_THD_RQOS_OUT(sram_max_mem_depth);
			thd_wqos_in = GET_THD_WQOS_IN(sram_max_mem_depth);
			thd_wqos_out = GET_THD_WQOS_OUT(sram_max_mem_depth);
			#if 1
			thd_cg_in = GET_THD_CG_IN(sram_max_mem_depth);
			thd_cg_out = GET_THD_CG_OUT(sram_max_mem_depth);
			#else
			thd_cg_in = 0x3fff;
			thd_cg_out = 0x3fff;
			#endif
			thd_wr_wait = GET_THD_OTHER_WR_WAIT(sram_max_mem_depth);
			thd_cg_hold = GET_THD_OTHER_DFS_CG_HOLD(sram_max_mem_depth);
		} else {
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
		}
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		dbuf_base = hisifd->dss_base + DSS_DPE1_OFFSET;

		if (g_fpga_flag == 1) {
			thd_rqos_in = GET_THD_RQOS_IN(sram_max_mem_depth);
			thd_rqos_out = GET_THD_RQOS_OUT(sram_max_mem_depth);
			thd_wqos_in = GET_THD_WQOS_IN(sram_max_mem_depth);
			thd_wqos_out = GET_THD_WQOS_OUT(sram_max_mem_depth);
			#if 1
			thd_cg_in = GET_THD_CG_IN(sram_max_mem_depth);
			thd_cg_out = GET_THD_CG_OUT(sram_max_mem_depth);
			#else
			thd_cg_in = 0x3fff;
			thd_cg_out = 0x3fff;
			#endif
			thd_wr_wait = GET_THD_OTHER_WR_WAIT(sram_max_mem_depth);
			thd_cg_hold = GET_THD_OTHER_DFS_CG_HOLD(sram_max_mem_depth);
		} else {
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
		}
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	outp32(dbuf_base + DFS_FRM_SIZE, pinfo->xres * pinfo->yres);
	outp32(dbuf_base + DFS_FRM_HSIZE, DSS_WIDTH(pinfo->xres));
	outp32(dbuf_base + DFS_SRAM_VALID_NUM, sram_valid_num);

	outp32(dbuf_base + DFS_THD_RQOS, (thd_rqos_out<< 16) | thd_rqos_in);
	outp32(dbuf_base + DFS_THD_WQOS, (thd_wqos_out << 16) | thd_wqos_in);
	outp32(dbuf_base + DFS_THD_CG, (thd_cg_out << 16) | thd_cg_in);
	outp32(dbuf_base + DFS_THD_OTHER, (thd_cg_hold << 16) | thd_wr_wait);
	outp32(dbuf_base + DFS_THD_FILL_LEV0, (thd_fill_lev2 << 16) | thd_fill_lev1);
	outp32(dbuf_base + DFS_FILL_LEV1_CNT, (thd_fill_lev4 << 16) | thd_fill_lev3);

	/* dfs_core_gt: use default value */
	/*set_reg(dbuf_base + DFS_DFS_LP_CTRL, 0, 2, 26);*/
	/* ret_aft_prefetch: use default value */
	/*set_reg(dbuf_base + DFS_DFS_LP_CTRL, 1, 1, 25);*/
	/* pd_upon_frm_end: use default value */
	/*set_reg(dbuf_base + DFS_DFS_LP_CTRL, 1, 1, 24);*/
}

void init_ldi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);

	pinfo = &(hisifd->panel_info);
	dss_base = hisifd->dss_base;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		if (is_dual_mipi_panel(hisifd)) {
			outp32(dss_base + PDP_LDI_DPI0_HRZ_CTRL0,
				pinfo->ldi.h_front_porch | (pinfo->ldi.h_back_porch << 16));
			outp32(dss_base + PDP_LDI_DPI0_HRZ_CTRL1,
				DSS_WIDTH(pinfo->xres / 2) | (DSS_WIDTH(pinfo->ldi.h_pulse_width) << 16));

			outp32(dss_base + PDP_LDI_DPI1_HRZ_CTRL0,
				pinfo->ldi.h_back_porch << 16);
			outp32(dss_base + PDP_LDI_DPI1_HRZ_CTRL1,
				DSS_WIDTH(pinfo->xres / 2) | (DSS_WIDTH(pinfo->ldi.h_pulse_width) << 16));

			outp32(dss_base + PDP_LDI_OVERLAP_SIZE,
				pinfo->ldi.dpi0_overlap_size | (pinfo->ldi.dpi1_overlap_size << 16));

			/* dual_mode_en */
			set_reg(dss_base + PDP_LDI_CTRL, 1, 1, 5);
			/* split mode */
			set_reg(dss_base + PDP_LDI_CTRL, 0, 1, 16);
		} else {
			outp32(dss_base + PDP_LDI_DPI0_HRZ_CTRL0,
				pinfo->ldi.h_front_porch | (pinfo->ldi.h_back_porch << 16));
			outp32(dss_base + PDP_LDI_DPI0_HRZ_CTRL1,
				DSS_WIDTH(pinfo->xres) | (DSS_WIDTH(pinfo->ldi.h_pulse_width) << 16));
		}
		outp32(dss_base + PDP_LDI_VRT_CTRL0,
			pinfo->ldi.v_front_porch | (pinfo->ldi.v_back_porch << 16));
		outp32(dss_base + PDP_LDI_VRT_CTRL1,
			DSS_HEIGHT(pinfo->yres) | (DSS_HEIGHT(pinfo->ldi.v_pulse_width) << 16));
		outp32(dss_base + PDP_LDI_PLR_CTRL,
			pinfo->ldi.vsync_plr | (pinfo->ldi.hsync_plr << 1) |
			(pinfo->ldi.pixelclk_plr << 2) | (pinfo->ldi.data_en_plr << 3));

		/* ldi disable */
		set_reg(dss_base + PDP_LDI_CTRL, 0, 1, 0);
		/* data_gate_en: mipi command LCD open */
		if (is_mipi_cmd_panel(hisifd)) {
			//set_reg(dss_base + PDP_LDI_CTRL, 0x1, 1, 2);
			hisifd->ldi_data_gate_en = 1;

			set_reg(dss_base + PDP_LDI_FRM_MSK, 0x1, 1, 0);
		}
		/* bpp */
		set_reg(dss_base + PDP_LDI_CTRL, pinfo->bpp, 2, 3);
		/* bgr */
		set_reg(dss_base + PDP_LDI_CTRL, pinfo->bgr_fmt, 1, 13);
		/* color mode: for mipi dsi */
		/*set_reg(dss_base + PDP_LDI_CTRL, 0, 1, 14);*/
		/* shutdown: for mipi dsi */
		/*set_reg(dss_base + PDP_LDI_CTRL, 0, 1, 15);*/

	#ifdef CONFIG_HISI_FB_COLORBAR_USED
		/* colorbar width */
		set_reg(dss_base + PDP_LDI_CTRL, DSS_WIDTH(0x3c), 7, 6);
		/* colorbar ort */
		set_reg(dss_base + PDP_LDI_WORK_MODE, 0, 1, 1);
		/* colorbar enable */
		set_reg(dss_base + PDP_LDI_WORK_MODE, 0, 1, 0);
	#else
		/* normal */
		outp32(dss_base + PDP_LDI_WORK_MODE, 0x1);
	#endif
		/* for ddr pmqos */
		outp32(dss_base + PDP_LDI_VINACT_MSK_LEN,
		pinfo->ldi.v_front_porch);
		outp32(dss_base + PDP_LDI_CG_CTRL, 0x0);
		/* use default value */
		/*outp32(dss_base + PDP_SRAM_LP_CTRL, );*/

		/* for 3D frame by frame */
		/*outp32(dss_base + PDP_LDI_3D_CTRL, );
		outp32(dss_base + PDP_LDI_DE_SPACE_LOW, );
		set_reg(dss_base + PDP_LDI_CTRL, 1, 1, 1);*/

		/* for 1Hz LCD and mipi command LCD */
		/*outp32(dss_base + PDP_LDI_FRM_MSK, );*/
		if (is_mipi_cmd_panel(hisifd)) {
			set_reg(dss_base + PDP_DSI_CMD_MOD_CTRL, 0x1, 1, 0);
			set_reg(dss_base + PDP_DSI_TE_CTRL, 0x4001, 32, 0);
			set_reg(dss_base + PDP_DSI_TE_HS_NUM, 0x0, 32, 0);
			set_reg(dss_base + PDP_DSI_TE_HS_WD, 0xF0F, 32, 0);
			set_reg(dss_base + PDP_DSI_TE_VS_WD, 0x3FC0FF, 32, 0);
		}

		/*
		** for test
		** 2'b00: reserved
		** 2'b01: LDI0->DSI0
		** 2'b10: LDI0->DSI1
		** 2'b11: LDI1
		*/
		if (g_fpga_flag == 1) {
			set_reg(dss_base + DSS_GLB_TEST_SEL, 0x1, 2, 0);
		}
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
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

		/* ldi disable */
		set_reg(dss_base + SDP_LDI_CTRL, 0, 1, 0);
		/* data_gate_en */
		/*set_reg(dss_base + PDP_LDI_CTRL, 1, 1, 2);*/
		/* bpp */
		set_reg(dss_base + SDP_LDI_CTRL, pinfo->bpp, 2, 3);
		/* bgr */
		set_reg(dss_base + SDP_LDI_CTRL, pinfo->bgr_fmt, 1, 13);
		/* color mode */
		/*set_reg(dss_base + SDP_LDI_CTRL, 0, 1, 14);*/
		/* shutdown */
		/*set_reg(dss_base + SDP_LDI_CTRL, 0, 1, 15);*/

	#ifdef CONFIG_HISI_FB_COLORBAR_USED
		/* colorbar width */
		set_reg(dss_base + SDP_LDI_CTRL, DSS_WIDTH(0x3c), 7, 6);
		/* colorbar ort */
		set_reg(dss_base + SDP_LDI_WORK_MODE, 0, 1, 1);
		/* colorbar enable */
		set_reg(dss_base + SDP_LDI_WORK_MODE, 0, 1, 0);
	#else
		/* normal */
		outp32(dss_base + SDP_LDI_WORK_MODE, 0x1);
	#endif
		outp32(dss_base + SDP_LDI_VINACT_MSK_LEN,
			pinfo->ldi.v_front_porch);
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}
}

void deinit_ldi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		set_reg(dss_base + PDP_LDI_CTRL, 0, 1, 0);
	} else  if (hisifd->index == EXTERNAL_PANEL_IDX) {
		set_reg(dss_base + SDP_LDI_CTRL, 0, 1, 0);
	} else {
		;
	}
}

void enable_ldi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		set_reg(dss_base + PDP_LDI_CTRL, 1, 1, 0);
	} else  if (hisifd->index == EXTERNAL_PANEL_IDX) {
		set_reg(dss_base + SDP_LDI_CTRL, 1, 1, 0);
	} else {
		;
	}
}

void disable_ldi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		set_reg(dss_base + PDP_LDI_CTRL, 0, 1, 0);
	} else  if (hisifd->index == EXTERNAL_PANEL_IDX) {
		set_reg(dss_base + SDP_LDI_CTRL, 0, 1, 0);
	} else {
		;
	}
}

void single_frame_update(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		if (is_mipi_cmd_panel(hisifd)) {
			set_reg(dss_base + PDP_LDI_FRM_MSK, 1, 1, 24);
		}
		set_reg(dss_base + PDP_LDI_CTRL, 1, 1, 0);
		#ifdef CONFIG_BACKLIGHT_10000
		        updateCabcPwm();
		#endif
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		if (is_mipi_cmd_panel(hisifd)) {
			set_reg(dss_base + SDP_LDI_FRM_MSK, 1, 1, 24);
		}
		set_reg(dss_base + SDP_LDI_CTRL, 1, 1, 0);
	} else {
		;
	}

	if (hisifd->vsync_ctrl.vsync_report_fnc) {
		atomic_inc(&(hisifd->vsync_ctrl.buffer_updated));
	}
}


/******************************************************************************/
#define PDP_EN_CLK_CFG	(BIT_CLK_PDPAXI | BIT_CLK_PDPPRI | \
	BIT_CLK_PXL0DIV | BIT_CLK_PDPPXL0)
/*#define SDP_EN_CLK_CFG	(BIT_CLK_SDPAXI | \
	BIT_CLK_SDPPRI | BIT_CLK_SDPPXL1 | BIT_CLK_HDMIPXL1)
#define ADP_EN_CLK_CFG	(BIT_CLK_ADPAXI | BIT_CLK_ADPDPE3)*/
#define DPE2_EN_CLK_CFG	(BIT_CLK_DPE2AXI | BIT_CLK_ADPDPE2)

#define PDP_DSI0_EN_CLK_CFG	(BIT_CLK_PDPDSI0 | BIT_DSI0_PCLK)
#define PDP_DSI1_EN_CLK_CFG	(BIT_CLK_PDPDSI1 | BIT_DSI1_PCLK)
#define PDP_IFBC_EN_CLK_CFG	(BIT_CLK_PXL0DIV2 | BIT_DIV2_GT)

#define PDP_DIS_CLK_CFG	(PDP_EN_CLK_CFG)
//#define SDP_DIS_CLK_CFG	(SDP_EN_CLK_CFG)
//#define ADP_DIS_CLK_CFG	(ADP_EN_CLK_CFG)
#define DPE2_DIS_CLK_CFG	(DPE2_EN_CLK_CFG)

#define PDP_DSI0_DIS_CLK_CFG	(PDP_DSI0_EN_CLK_CFG)
#define PDP_DSI1_DIS_CLK_CFG	(PDP_DSI1_EN_CLK_CFG)
#define PDP_IFBC_DIS_CLK_CFG	(PDP_IFBC_EN_CLK_CFG)


/******************************************************************************/
#define PDP_EN_RST_CFG	(BIT_PDP_AXI_RST | \
	BIT_PDP_PRI_RST | BIT_PDP_PXL0_RST | BIT_PDP_AFW_RST | \
	BIT_PDP_AFWCLR_MASK | BIT_PDP_AFR_RST | BIT_PDP_AFRCLR_MASK)
/*#define SDP_EN_RST_CFG	(BIT_SDP_PRI_RST | BIT_SDP_AXI_RST | \
	BIT_SDP_PXL1_RST | BIT_SDP_AFW_RST | BIT_SDP_AFR_RST)
#define ADP_EN_RST_CFG	(BIT_ADP_AXI_RST | BIT_ADP_DPE3_RST)*/
#define DPE2_EN_RST_CFG	(BIT_DPE2_AXI_RST | BIT_ADP_DPE2_RST)

#define PDP_DSI0_EN_RST_CFG	(BIT_PDP_DSI0_RST | BIT_DSI0_PRST)
#define PDP_DSI1_EN_RST_CFG	(BIT_PDP_DSI1_RST | BIT_DSI1_PRST)

#define PDP_DIS_RST_CFG	(PDP_EN_RST_CFG)
//#define SDP_DIS_RST_CFG	(SDP_EN_RST_CFG)
//#define ADP_DIS_RST_CFG	(ADP_EN_RST_CFG)
#define DPE2_DIS_RST_CFG	(DPE2_EN_RST_CFG)

#define PDP_DSI0_DIS_RST_CFG	(PDP_DSI0_EN_RST_CFG)
#define PDP_DSI1_DIS_RST_CFG	(PDP_DSI1_EN_RST_CFG)


#define ROT1_EN_RST_CFG	(BIT_ROT_RST)
#define ROT1_DIS_RST_CFG	(ROT1_EN_RST_CFG)


#define AXI_EN_CLK_CFG	(BIT_CLK_AXI)
#define AXI_DIS_CLK_CFG	(AXI_EN_CLK_CFG)
#define AXI_EN_RST_CFG	(BIT_DSS_AXI_RST)
#define AXI_DIS_RST_CFG	(AXI_EN_RST_CFG)


void enable_clk_pdp(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	enable_clk_axi(hisifd);
	enable_clk_common(hisifd);
	enable_clk_panel(hisifd);
	enable_clk_dpe2(hisifd);
	enable_clk_rot1(hisifd);
}

void disable_clk_pdp(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	disable_clk_rot1(hisifd);
	disable_clk_dpe2(hisifd);
	disable_clk_panel(hisifd);
	disable_clk_common(hisifd);
	disable_clk_axi(hisifd);
}

void enable_clk_sdp(struct hisi_fb_data_type *hisifd)
{
	enable_clk_axi(hisifd);
	enable_clk_common(hisifd);
	enable_clk_dpe2(hisifd);
	enable_clk_rot1(hisifd);
}

void disable_clk_sdp(struct hisi_fb_data_type *hisifd)
{
	disable_clk_rot1(hisifd);
	disable_clk_dpe2(hisifd);
	disable_clk_common(hisifd);
	disable_clk_axi(hisifd);
}

void enable_clk_adp(struct hisi_fb_data_type *hisifd)
{
	enable_clk_dpe2(hisifd);
	enable_clk_rot1(hisifd);
	enable_clk_axi(hisifd);
}

void disable_clk_adp(struct hisi_fb_data_type *hisifd)
{
	disable_clk_axi(hisifd);
	disable_clk_rot1(hisifd);
	disable_clk_dpe2(hisifd);

	set_reg(hisifd->dss_base + DSS_GLB_DPE2_CLK_SW, 0x0, 1, 0);
	set_reg(hisifd->dss_base + DSS_GLB_ROT1_CLK_SW, 0x0, 1, 0);
}

void enable_clk_common(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int prev_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_common_clk_sem);
	prev_refcount = common_refcount++;
	if (!prev_refcount) {
		/* de-reset status */
		val =0x0;
		val |= PDP_DIS_RST_CFG;
		outp32(dss_base + DSS_GLB_DIS_RST_CFG, val);

		/* enable clk */
		val = 0x0;
		val |= PDP_EN_CLK_CFG;
		outp32(dss_base + DSS_GLB_EN_CLK_CFG, val);
	}
	up(&hisi_fb_dss_common_clk_sem);
}

void disable_clk_common(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int new_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_common_clk_sem);
	new_refcount = --common_refcount;
	WARN_ON(new_refcount < 0);
	if (!new_refcount) {
		/* disable clk */
		val = 0x0;
		val |= PDP_DIS_CLK_CFG;
		outp32(dss_base + DSS_GLB_DIS_CLK_CFG, val);

		/* reset status */
		val = 0x0;
		val |= PDP_EN_RST_CFG;
		outp32(dss_base + DSS_GLB_EN_RST_CFG, val);
	}
	up(&hisi_fb_dss_common_clk_sem);
}

void enable_clk_panel(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int prev_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_panel_clk_sem);
	prev_refcount = panel_refcount++;
	if (!prev_refcount) {
		/* de-reset status */
		val =0x0;
		if (is_mipi_panel(hisifd)) {
			val |= PDP_DSI0_DIS_RST_CFG;
		}
		if (is_dual_mipi_panel(hisifd)) {
			val |= PDP_DSI1_DIS_RST_CFG;
		}
		outp32(dss_base + DSS_GLB_DIS_RST_CFG, val);

		/* enable clk */
		val = 0x0;
		if (is_mipi_panel(hisifd)) {
			val |= PDP_DSI0_EN_CLK_CFG;
		}
		if (is_dual_mipi_panel(hisifd)) {
			val |= PDP_DSI1_EN_CLK_CFG;
			if (is_ifbc_panel(hisifd)) {
				set_reg(dss_base + DSS_GLB_PXL0_DIV, 0xb, 4, 0);
			} else {
				set_reg(dss_base + DSS_GLB_PXL0_DIV, 0x9, 4, 0);
			}
		}
		if (is_ifbc_panel(hisifd)) {
			val |= PDP_IFBC_EN_CLK_CFG;
		}
		outp32(dss_base + DSS_GLB_EN_CLK_CFG, val);
	}
	up(&hisi_fb_dss_panel_clk_sem);
}

void disable_clk_panel(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int new_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_panel_clk_sem);
	new_refcount = --panel_refcount;
	WARN_ON(new_refcount < 0);
	if (!new_refcount) {
		/* disable clk */
		val = 0x0;
		if (is_mipi_panel(hisifd)) {
			val |= PDP_DSI0_DIS_CLK_CFG;
		}
		if (is_dual_mipi_panel(hisifd)) {
			val |= PDP_DSI1_DIS_CLK_CFG;
		}
		if (is_ifbc_panel(hisifd)) {
			val |= PDP_IFBC_DIS_CLK_CFG;
		}
		outp32(dss_base + DSS_GLB_DIS_CLK_CFG, val);

		/* reset status */
		val = 0x0;
		if (is_mipi_panel(hisifd)) {
			val |= PDP_DSI0_EN_RST_CFG;
		}
		if (is_dual_mipi_panel(hisifd)) {
			val |= PDP_DSI1_EN_RST_CFG;
		}
		outp32(dss_base + DSS_GLB_EN_RST_CFG, val);
	}
	up(&hisi_fb_dss_panel_clk_sem);
}

void enable_clk_dpe2(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int prev_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_dpe2_clk_sem);
	prev_refcount = dpe2_refcount++;
	if (!prev_refcount) {
		/* enable clk */
		val = 0x0;
		val |= (DPE2_EN_CLK_CFG);
		outp32(dss_base + DSS_GLB_EN_CLK_CFG, val);

		/* de-reset status */
		val = 0x0;
		val |= (DPE2_DIS_RST_CFG);
		outp32(dss_base + DSS_GLB_DIS_RST_CFG, val);
	}
	up(&hisi_fb_dss_dpe2_clk_sem);
}

void disable_clk_dpe2(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int new_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_dpe2_clk_sem);
	new_refcount = --dpe2_refcount;
	WARN_ON(new_refcount < 0);
	if (!new_refcount) {
		/* reset status */
		val = 0x0;
		val |= (DPE2_EN_RST_CFG);
		outp32(dss_base + DSS_GLB_EN_RST_CFG, val);

		/* disable clk */
		val = 0x0;
		val |= DPE2_DIS_CLK_CFG;
		outp32(dss_base + DSS_GLB_DIS_CLK_CFG, val);
	}
	up(&hisi_fb_dss_dpe2_clk_sem);
}

void enable_clk_rot1(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int prev_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_rot1_clk_sem);
	prev_refcount = rot1_refcount++;
	if (!prev_refcount) {
		/* de-reset status */
		val = 0x0;
		val |= (ROT1_DIS_RST_CFG);
		outp32(dss_base + DSS_GLB_DIS_RST_CFG, val);
	}
	up(&hisi_fb_dss_rot1_clk_sem);
}

void disable_clk_rot1(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int new_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_rot1_clk_sem);
	new_refcount = --rot1_refcount;
	WARN_ON(new_refcount < 0);
	if (!new_refcount) {
		/* reset status */
		val = 0x0;
		val |= (ROT1_EN_RST_CFG);
		outp32(dss_base + DSS_GLB_EN_RST_CFG, val);
	}
	up(&hisi_fb_dss_rot1_clk_sem);
}

void enable_clk_axi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int prev_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_axi_clk_sem);
	prev_refcount = axi_refcount++;
	if (!prev_refcount) {
		/* enable clk */
		val = 0x0;
		val |= (AXI_EN_CLK_CFG);
		outp32(dss_base + DSS_GLB_EN_CLK_CFG, val);

		/* disable reset status */
		val = 0x0;
		val |= (AXI_DIS_RST_CFG);
		outp32(dss_base + DSS_GLB_DIS_RST_CFG, val);
	}
	up(&hisi_fb_dss_axi_clk_sem);
}

void disable_clk_axi(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = NULL;
	uint32_t val = 0;
	int new_refcount = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	down(&hisi_fb_dss_axi_clk_sem);
	new_refcount = --axi_refcount;
	WARN_ON(new_refcount < 0);
	if (!new_refcount) {
		/* enable reset status */
		val = 0x0;
		val |= (AXI_EN_RST_CFG);
		outp32(dss_base + DSS_GLB_EN_RST_CFG, val);

		/* disable clk */
		val = 0x0;
		val |= AXI_DIS_CLK_CFG;
		outp32(dss_base + DSS_GLB_DIS_CLK_CFG, val);
	}
	up(&hisi_fb_dss_axi_clk_sem);
}

void dpe_init_ct_cscValue(void)
{
	g_csc_value[0] = 32768;
	g_csc_value[1] = 0;
	g_csc_value[2] = 0;
	g_csc_value[3] = 0;
	g_csc_value[4] = 32768;
	g_csc_value[5] = 0;
	g_csc_value[6] = 0;
	g_csc_value[7] = 0;
	g_csc_value[8] = 32768;
	g_is_csc_set = 0;

	return;
}

void dpe_store_ct_cscValue(unsigned int csc_value[])
{
	g_csc_value [0] = csc_value[0];
	g_csc_value [1] = csc_value[1];
	g_csc_value [2] = csc_value[2];
	g_csc_value [3] = csc_value[3];
	g_csc_value [4] = csc_value[4];
	g_csc_value [5] = csc_value[5];
	g_csc_value [6] = csc_value[6];
	g_csc_value [7] = csc_value[7];
	g_csc_value [8] = csc_value[8];
	g_is_csc_set = 1;

	return;
}

void dpe_update_g_starlight_discount(unsigned int value)
{
    g_starlight_value = value;
    HISI_FB_INFO(" g_starlight_value = %d" , g_starlight_value);       
}

void dpe_update_g_comform_discount(unsigned int value)
{
    g_comform_value = value;
    HISI_FB_INFO(" g_comform_value = %d" , g_comform_value);       
}

int dpe_set_ct_cscValue(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;
	struct hisi_panel_info *pinfo = NULL;

	dss_base = hisifd->dss_base;
	pinfo = &(hisifd->panel_info);

	if (1 == g_is_csc_set && pinfo->color_temperature_support) {
		HISI_FB_DEBUG("real set color temperature: g_is_csc_set = %d, R = 0x%x, G = 0x%x, B = 0x%x .\n",
				g_is_csc_set, g_csc_value[0], g_csc_value[4], g_csc_value[8]);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_01, g_csc_value[0]*g_led_rg_csc_value[0]/32768*DISCOUNT_COEFFICIENT(g_starlight_value), 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_02, g_csc_value[1], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_03, g_csc_value[2], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_11, g_csc_value[3], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_12, g_csc_value[4]*g_led_rg_csc_value[4]/32768*DISCOUNT_COEFFICIENT(g_starlight_value), 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_13, g_csc_value[5], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_21, g_csc_value[6], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_22, g_csc_value[7], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_23, g_csc_value[8]*g_led_rg_csc_value[8]/32768*DISCOUNT_COEFFICIENT(g_starlight_value)*DISCOUNT_COEFFICIENT(g_comform_value), 17, 0);
	}

	return 0;
}

ssize_t dpe_show_ct_cscValue(char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
							g_csc_value [0], g_csc_value [1], g_csc_value [2],
							g_csc_value [3], g_csc_value [4], g_csc_value [5],
							g_csc_value [6], g_csc_value [7], g_csc_value [8]);
}

void dpe_init_led_rg_ct_cscValue(void)
{
	g_led_rg_csc_value[0] = 32768;
	g_led_rg_csc_value[1] = 0;
	g_led_rg_csc_value[2] = 0;
	g_led_rg_csc_value[3] = 0;
	g_led_rg_csc_value[4] = 32768;
	g_led_rg_csc_value[5] = 0;
	g_led_rg_csc_value[6] = 0;
	g_led_rg_csc_value[7] = 0;
	g_led_rg_csc_value[8] = 32768;
	g_is_led_rg_csc_set = 0;

	return;
}

void dpe_store_led_rg_ct_cscValue(unsigned int csc_value[])
{
	g_led_rg_csc_value [0] = csc_value[0];
	g_led_rg_csc_value [1] = csc_value[1];
	g_led_rg_csc_value [2] = csc_value[2];
	g_led_rg_csc_value [3] = csc_value[3];
	g_led_rg_csc_value [4] = csc_value[4];
	g_led_rg_csc_value [5] = csc_value[5];
	g_led_rg_csc_value [6] = csc_value[6];
	g_led_rg_csc_value [7] = csc_value[7];
	g_led_rg_csc_value [8] = csc_value[8];
	g_is_led_rg_csc_set = 1;

	return;
}

int dpe_set_led_rg_ct_cscValue(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;
	struct hisi_panel_info *pinfo = NULL;

	dss_base = hisifd->dss_base;
	pinfo = &(hisifd->panel_info);

	if (1 == g_is_led_rg_csc_set && pinfo->color_temperature_support) {
		HISI_FB_DEBUG("real set color temperature: g_is_led_rg_csc_set = %d, R = 0x%x, G = 0x%x, B = 0x%x .\n",
				g_is_led_rg_csc_set, g_led_rg_csc_value[0], g_led_rg_csc_value[4], g_led_rg_csc_value[8]);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_01, g_led_rg_csc_value[0]*g_csc_value[0]/32768*DISCOUNT_COEFFICIENT(g_starlight_value), 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_02, g_led_rg_csc_value[1], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_03, g_led_rg_csc_value[2], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_11, g_led_rg_csc_value[3], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_12, g_led_rg_csc_value[4]*g_csc_value[4]/32768*DISCOUNT_COEFFICIENT(g_starlight_value), 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_13, g_led_rg_csc_value[5], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_21, g_led_rg_csc_value[6], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_22, g_led_rg_csc_value[7], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_23, g_led_rg_csc_value[8]*g_csc_value[8]/32768*DISCOUNT_COEFFICIENT(g_starlight_value)*DISCOUNT_COEFFICIENT(g_comform_value), 17, 0);
	}

	return 0;
}

ssize_t dpe_show_led_rg_ct_cscValue(char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
							g_led_rg_csc_value [0], g_led_rg_csc_value [1], g_led_rg_csc_value [2],
							g_led_rg_csc_value [3], g_led_rg_csc_value [4], g_led_rg_csc_value [5],
							g_led_rg_csc_value [6], g_led_rg_csc_value [7], g_led_rg_csc_value [8]);
}


int dpe_set_comform_ct_cscValue(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;
	struct hisi_panel_info *pinfo = NULL;

	dss_base = hisifd->dss_base;
	pinfo = &(hisifd->panel_info);

	if (pinfo->color_temperature_support) {
		HISI_FB_DEBUG("real set comform: g_comform_value = %d, R = 0x%x, G = 0x%x, B = 0x%x .\n",
				g_comform_value, g_csc_value[0], g_csc_value[4], g_csc_value[8]);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_01, g_csc_value[0]*g_led_rg_csc_value[0]/32768*DISCOUNT_COEFFICIENT(g_starlight_value), 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_02, g_csc_value[1], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_03, g_csc_value[2], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_11, g_csc_value[3], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_12, g_csc_value[4]*g_led_rg_csc_value[4]/32768*DISCOUNT_COEFFICIENT(g_starlight_value), 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_13, g_csc_value[5], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_21, g_csc_value[6], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_22, g_csc_value[7], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_23, g_csc_value[8]*g_led_rg_csc_value[8]/32768*DISCOUNT_COEFFICIENT(g_starlight_value)*DISCOUNT_COEFFICIENT(g_comform_value), 17, 0);
	}

	return 0;
}

ssize_t dpe_show_comform_ct_cscValue(char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d,g_comform_value = %d\n",
							g_csc_value [0], g_csc_value [1], g_csc_value [2],
							g_csc_value [3], g_csc_value [4], g_csc_value [5],
							g_csc_value [6], g_csc_value [7], g_csc_value [8],
							g_comform_value);
}

int dpe_set_starlight_ct_cscValue(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dss_base = 0;
	struct hisi_panel_info *pinfo = NULL;

	dss_base = hisifd->dss_base;
	pinfo = &(hisifd->panel_info);

	if (pinfo->color_temperature_support) {
		HISI_FB_DEBUG("real set g_starlight_value: g_starlight_value = %d, R = 0x%x, G = 0x%x, B = 0x%x .\n",
				g_starlight_value, g_csc_value[0], g_csc_value[4], g_csc_value[8]);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_01, g_csc_value[0]*g_led_rg_csc_value[0]/32768*DISCOUNT_COEFFICIENT(g_starlight_value), 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_02, g_csc_value[1], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_03, g_csc_value[2], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_11, g_csc_value[3], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_12, g_csc_value[4]*g_led_rg_csc_value[4]/32768*DISCOUNT_COEFFICIENT(g_starlight_value), 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_13, g_csc_value[5], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_21, g_csc_value[6], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_22, g_csc_value[7], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_23, g_csc_value[8]*g_led_rg_csc_value[8]/32768*DISCOUNT_COEFFICIENT(g_starlight_value)*DISCOUNT_COEFFICIENT(g_comform_value), 17, 0);
	}

	return 0;
}

ssize_t dpe_show_starlight_ct_cscValue(char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d,g_starlight_value = %d\n",
							g_csc_value [0], g_csc_value [1], g_csc_value [2],
							g_csc_value [3], g_csc_value [4], g_csc_value [5],
							g_csc_value [6], g_csc_value [7], g_csc_value [8],
							g_starlight_value);
}
