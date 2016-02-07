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

#include "hisi_overlay_utils.h"

/* rptb gen pool */
static struct gen_pool * g_rptb_gen_pool;

DEFINE_SEMAPHORE(hisi_fb_dss_scf1_sem);
DEFINE_SEMAPHORE(hisi_fb_dss_scf2_sem);
static int g_scf1_coef_load_refcount;
static int g_scf2_coef_load_refcount;

uint32_t g_frame_count = 0;

uint32_t g_dss_module_base[DSS_CHN_MAX][MODULE_CHN_MAX] = {
	/******************DPE0*******************/
	/* DPE0 CH0 */
	{DSS_DPE0_OFFSET + DSS_DPE_RDMA0_OFFSET,
	  DSS_DPE0_OFFSET + DSS_DPE_RDMA1_OFFSET,
	  0,
	  0,
	  DSS_DPE0_OFFSET + DSS_DPE_RDFC0_OFFSET,
	  DSS_DPE0_OFFSET + DSS_SCF0_OFFSET,
	  DSS_DPE0_OFFSET + DSS_DPE_SCP_OFFSET,
	  DSS_DPE0_OFFSET + DSS_DPE_CSC0_OFFSET,
	  DSS_DPE0_OFFSET + DSS_MMU_RDMA0_OFFSET,
	  DSS_DPE0_OFFSET + DSS_MMU_RDMA1_OFFSET,
	  DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET,
	  DSS_GLB_DPE0_CH0_CTL,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET + DPE0_CH1_MUX,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET + ROT0_PM_CTRL,
	  0,
	  0,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET + DPE0_SWITCH},
	/* DPE0 CH1 */
	{DSS_DPE0_OFFSET + DSS_DPE_RDMA2_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE0_OFFSET + DSS_DPE_RDFC1_OFFSET,
	  DSS_DPE0_OFFSET + DSS_SCF0_OFFSET,
	  DSS_DPE0_OFFSET + DSS_DPE_SCP_OFFSET,
	  DSS_DPE0_OFFSET + DSS_DPE_CSC1_OFFSET,
	  DSS_DPE0_OFFSET + DSS_MMU_RDMA2_OFFSET,
	  0,
	  0,
	  DSS_GLB_DPE0_CH1_CTL,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET + DPE0_CH1_MUX,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET + DPE0_SWITCH},
	/* DPE0 CH2 */
	{DSS_DPE0_OFFSET + DSS_DPE_RDMA3_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE0_OFFSET + DSS_DPE_RDFC2_OFFSET,
	  0,
	  0,
	  DSS_DPE0_OFFSET + DSS_DPE_CSC2_OFFSET,
	  DSS_DPE0_OFFSET + DSS_MMU_RDMA3_OFFSET,
	  0,
	  0,
	  DSS_GLB_DPE0_CH2_CTL,
	  0,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},
	/* DPE0 CH3 */
	{DSS_DPE0_OFFSET + DSS_DPE_RDMA4_OFFSET,
	  0,
	  DSS_DPE0_OFFSET + DSS_DPE_FBDC_OFFSET,
	  0,
	  DSS_DPE0_OFFSET + DSS_DPE_RDFC3_OFFSET,
	  0,
	  0,
	  DSS_DPE0_OFFSET + DSS_DPE_CSC3_OFFSET,
	  DSS_DPE0_OFFSET + DSS_MMU_RDMA4_OFFSET,
	  0,
	  0,
	  DSS_GLB_DPE0_CH3_CTL,
	  0,
	  DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},
	/* DPE0 CH4 */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},


	/******************DPE1*******************/
	/* DPE1 CH0 */
	{DSS_DPE1_OFFSET + DSS_DPE_RDMA0_OFFSET,
	  DSS_DPE1_OFFSET + DSS_DPE_RDMA1_OFFSET,
	  0,
	  0,
	  DSS_DPE1_OFFSET + DSS_DPE_RDFC0_OFFSET,
	  DSS_WBE0_OFFSET + DSS_SCF1_OFFSET,
	  DSS_DPE1_OFFSET + DSS_DPE_SCP_OFFSET,
	  DSS_DPE1_OFFSET + DSS_DPE_CSC0_OFFSET,
	  DSS_DPE1_OFFSET + DSS_MMU_RDMA0_OFFSET,
	  DSS_DPE1_OFFSET + DSS_MMU_RDMA1_OFFSET,
	  0,
	  DSS_GLB_DPE1_CH0_CTL,
	  DSS_DPE1_OFFSET + DSS_DP_CTRL_OFFSET + DPE1_CH0_MUX,
	  DSS_DPE1_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  DSS_WBE0_OFFSET + DSS_DP_CTRL_OFFSET + WBE0_SCF1_SEL,
	  0},
	/* DPE1 CH1 */
	{DSS_DPE1_OFFSET + DSS_DPE_RDMA4_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE1_OFFSET + DSS_DPE_RDFC3_OFFSET,
	  0,
	  0,
	  DSS_DPE1_OFFSET + DSS_DPE_CSC3_OFFSET,
	  DSS_DPE1_OFFSET + DSS_MMU_RDMA2_OFFSET,
	  0,
	  0,
	  DSS_GLB_DPE1_CH3_CTL,
	  0,
	  DSS_DPE1_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},
	/* DPE1 CH2 */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},


	/******************DPE2*******************/
	/* DPE2 CH0 */
	{DSS_DPE2_OFFSET + DSS_DPE_RDMA0_OFFSET,
	  DSS_DPE2_OFFSET + DSS_DPE_RDMA1_OFFSET,
	  0,
	  0,
	  DSS_DPE2_OFFSET + DSS_DPE_RDFC0_OFFSET,
	  DSS_DPE2_OFFSET + DSS_SCF2_OFFSET,
	  DSS_DPE2_OFFSET + DSS_DPE_SCP_OFFSET,
	  DSS_DPE2_OFFSET + DSS_DPE_CSC0_OFFSET,
	  DSS_DPE2_OFFSET + DSS_MMU_RDMA0_OFFSET,
	  DSS_DPE2_OFFSET + DSS_MMU_RDMA1_OFFSET,
	  DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET,
	  DSS_GLB_DPE2_CH0_CTL,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET + DPE2_CH0_MUX,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET,
	  DSS_GLB_ROT1_PM_CTRL,
	  DSS_GLB_ROT1_SEL,
	  0,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET + DPE2_SWITCH},
	/* DPE2 CH1 */
	{DSS_DPE2_OFFSET + DSS_DPE_RDMA2_OFFSET,
	  0,
	  DSS_DPE2_OFFSET + DSS_DPE_FBDC_OFFSET,
	  0,
	  DSS_DPE2_OFFSET + DSS_DPE_RDFC1_OFFSET,
	  DSS_DPE2_OFFSET + DSS_SCF2_OFFSET,
	  DSS_DPE2_OFFSET + DSS_DPE_SCP_OFFSET,
	  DSS_DPE2_OFFSET + DSS_DPE_CSC1_OFFSET,
	  DSS_DPE2_OFFSET + DSS_MMU_RDMA2_OFFSET,
	  0,
	  0,
	  DSS_GLB_DPE2_CH1_CTL,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET + DPE2_CH1_MUX,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET + DPE2_SWITCH},
	/* DPE2 CH2 */
	{DSS_DPE2_OFFSET + DSS_DPE_RDMA3_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE2_OFFSET + DSS_DPE_RDFC2_OFFSET,
	  0,
	  0,
	  DSS_DPE2_OFFSET + DSS_DPE_CSC2_OFFSET,
	  DSS_DPE2_OFFSET + DSS_MMU_RDMA3_OFFSET,
	  0,
	  0,
	  DSS_GLB_DPE2_CH2_CTL,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET + DPE2_CH2_MUX,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},
	/* DPE2 CH3 */
	{DSS_DPE2_OFFSET + DSS_DPE_RDMA4_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE2_OFFSET + DSS_DPE_RDFC3_OFFSET,
	  0,
	  0,
	  DSS_DPE2_OFFSET + DSS_DPE_CSC3_OFFSET,
	  DSS_DPE2_OFFSET + DSS_MMU_RDMA4_OFFSET,
	  0,
	  0,
	  DSS_GLB_DPE2_CH3_CTL,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET + DPE2_CH3_MUX,
	  DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},


	/******************DPE3*******************/
	/* DPE3_CHN0 */
	{DSS_DPE3_OFFSET + DSS_DPE_RDMA0_OFFSET,
	  DSS_DPE3_OFFSET + DSS_DPE_RDMA1_OFFSET,
	  0,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_RDFC0_OFFSET,
	  DSS_DPE3_OFFSET + DSS_SCF4_OFFSET,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_CSC0_OFFSET,
	  DSS_DPE3_OFFSET + DSS_MMU_RDMA0_OFFSET,
	  DSS_DPE3_OFFSET + DSS_MMU_RDMA1_OFFSET,
	  DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET,
	  DSS_GLB_DPE3_CH0_CTL,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET + DPE3_CH0_MUX,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},
	/* DPE3_CHN1 */
	{DSS_DPE3_OFFSET + DSS_DPE_RDMA2_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_RDFC1_OFFSET,
	  DSS_DPE3_OFFSET + DSS_SCF3_OFFSET,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_CSC1_OFFSET,
	  DSS_DPE3_OFFSET + DSS_MMU_RDMA2_OFFSET,
	  0,
	  DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET,
	  DSS_GLB_DPE3_CH1_CTL,
	  0,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET + ADP_ROT2_SEL,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET + ADP_SCF3_SEL,
	  0},
	/* DPE3_CHN2 */
	{DSS_DPE3_OFFSET + DSS_DPE_RDMA3_OFFSET,
	  0,
	  0,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_RDFC2_OFFSET,
	  0,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_CSC2_OFFSET,
	  DSS_DPE3_OFFSET + DSS_MMU_RDMA3_OFFSET,
	  0,
	  DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET,
	  DSS_GLB_DPE3_CH2_CTL,
	  0,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},
	/* DPE3_CHN3 */
	{DSS_DPE3_OFFSET + DSS_DPE_RDMA4_OFFSET,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_FBDC_OFFSET,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_RDFC3_OFFSET,
	  0,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_CSC3_OFFSET,
	  DSS_DPE3_OFFSET + DSS_MMU_RDMA4_OFFSET,
	  0,
	  0,
	  DSS_GLB_DPE3_CH3_CTL,
	  0,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},


	/******************WBE0*******************/
	/* WBE0_CHN0 */
	{DSS_WBE0_OFFSET + DSS_WBE_WDMA0_OFFSET,
	  0,
	  0,
	  0,
	  DSS_WBE0_OFFSET + DSS_WBE_WDFC0_OFFSET,
	  0,
	  0,
	  0,
	  DSS_WBE0_OFFSET + DSS_MMU_WDMA0_OFFSET,
	  0,
	  0,
	  DSS_GLB_WBE0_CH0_CTL,
	  0,
	  DSS_WBE0_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},
	/* WBE0_CHN1 */
	{DSS_WBE0_OFFSET + DSS_WBE_WDMA1_OFFSET,
	  0,
	  0,
	  DSS_WBE0_OFFSET + DSS_WBE_FBC0_OFFSET,
	  DSS_WBE0_OFFSET + DSS_WBE_WDFC1_OFFSET,
	  DSS_WBE0_OFFSET + DSS_SCF1_OFFSET,
	  0,
	  DSS_WBE0_OFFSET + DSS_WBE_CSC1_OFFSET,
	  DSS_WBE0_OFFSET + DSS_MMU_WDMA1_OFFSET,
	  0,
	  DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET,
	  DSS_GLB_WBE0_CH1_CTL,
	  DSS_WBE0_OFFSET + DSS_DP_CTRL_OFFSET + WBE0_MUX,
	  DSS_WBE0_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  DSS_GLB_ROT1_SEL,
	  DSS_WBE0_OFFSET + DSS_DP_CTRL_OFFSET + WBE0_SCF1_SEL,
	  0},


	/******************WBE1*******************/
	/* WBE1_CHN0 */
	{DSS_WBE1_OFFSET + DSS_WBE_WDMA0_OFFSET,
	  0,
	  0,
	  0,
	  DSS_WBE1_OFFSET + DSS_WBE_WDFC0_OFFSET,
	  0,
	  0,
	  DSS_DPE3_OFFSET + DSS_DPE_CSC0_OFFSET,
	  DSS_WBE1_OFFSET + DSS_MMU_WDMA0_OFFSET,
	  0,
	  0,
	  DSS_GLB_WBE1_CH0_CTL,
	  0,
	  DSS_WBE1_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  0,
	  0,
	  0},
	/* WBE1_CHN1 */
	{DSS_WBE1_OFFSET + DSS_WBE_WDMA1_OFFSET,
	  0,
	  0,
	  DSS_WBE1_OFFSET + DSS_WBE_FBC0_OFFSET,
	  DSS_WBE1_OFFSET + DSS_WBE_WDFC1_OFFSET,
	  DSS_WBE1_OFFSET + DSS_SCF3_OFFSET,
	  0,
	  DSS_WBE1_OFFSET + DSS_WBE_CSC1_OFFSET,
	  DSS_WBE1_OFFSET + DSS_MMU_WDMA1_OFFSET,
	  0,
	  DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET,
	  DSS_GLB_WBE1_CH1_CTL,
	  0,
	  DSS_WBE1_OFFSET + DSS_DP_CTRL_OFFSET,
	  0,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET + ADP_ROT2_SEL,
	  DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET + ADP_SCF3_SEL,
	  0},
};

uint32_t g_dss_module_ovl_base[DSS_OVL_MAX][MODULE_OVL_MAX] = {
	/* OVL PDP */
	{DSS_OVL0_OFFSET,
	DSS_GLB_OV0_SCENE,
	DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET + OV0_MUX,
	DSS_GLB_PDP_ONLINE_CFG_OK,
	DSS_DPE0_OFFSET + DFS_WBE_EN},
	/* OVL SDP */
	{DSS_OVL1_OFFSET,
	DSS_GLB_OV1_SCENE,
	DSS_DPE1_OFFSET + DSS_DP_CTRL_OFFSET + OV1_MUX,
	DSS_GLB_SDP_ONLINE_CFG_OK,
	DSS_DPE1_OFFSET + DFS_WBE_EN},
	/* OVL ADP */
	{DSS_OVL2_OFFSET,
	DSS_GLB_OV2_SCENE,
	DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET + OV2_MUX,
	0,
	0},
};

uint32_t g_dss_module_eng_base[DSS_ENG_MAX][MODULE_ENG_MAX] = {
	/* DPE0 */
	{DSS_DPE0_OFFSET,
	DSS_DPE0_OFFSET + DSS_DPE_RDMA_BRG_OFFSET,
	DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET},
	/* DPE1 */
	{DSS_DPE1_OFFSET,
	DSS_DPE1_OFFSET + DSS_DPE_RDMA_BRG_OFFSET,
	DSS_DPE1_OFFSET + DSS_DP_CTRL_OFFSET},
	/* DPE2 */
	{DSS_DPE2_OFFSET,
	DSS_DPE2_OFFSET + DSS_DPE_RDMA_BRG_OFFSET,
	DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET},
	/* DPE3 */
	{DSS_DPE3_OFFSET,
	DSS_DPE3_OFFSET + DSS_DPE_RDMA_BRG_OFFSET,
	DSS_DPE3_OFFSET + DSS_DP_CTRL_OFFSET},

	/* WBE0 */
	{DSS_WBE0_OFFSET,
	DSS_WBE_WDMA_BRG0_OFFSET,
	DSS_WBE0_OFFSET + DSS_DP_CTRL_OFFSET},
	/* WBE1 */
	{DSS_WBE1_OFFSET,
	DSS_WBE_WDMA_BRG1_OFFSET,
	DSS_WBE1_OFFSET + DSS_DP_CTRL_OFFSET},
};


/*******************************************************************************
**
*/
void dumpDssOverlay(dss_overlay_t *ov_data)
{
	uint32_t j = 0;
	dss_layer_t const *layer = NULL;
	dss_wb_layer_t const *wb_layer = NULL;

	BUG_ON(ov_data == NULL);

	printk("\n");
	printk("layer_nums = %d\n", ov_data->layer_nums);

	for (j = 0; j < ov_data->layer_nums; j++) {
		layer = &ov_data->layer_infos[j];

		printk("\n");
		printk("    LayerInfo[%d]:\n", j);

		printk("\t  flags | layer_idx | chn_idx | transform | blending | alpha |   color  | need_cap \n");
		printk("\t--------+-----------+---------+-----------+----------+-------+------------+----------\n");
		printk("\t %6u | %9d | %7d | %9d | %8u | %5d | 0x%8x | %8u \n",
			layer->flags, layer->layer_idx, layer->chn_idx,
			layer->transform, layer->blending, layer->glb_alpha,
			layer->color, layer->need_cap);

		printk("\n");
		printk("\t format |  width | height |  bpp   | stride | uStride | vStride |  phy_addr  |  vir_addr  | uOffset | vOffset \n");
		printk("\t--------+--------+--------+--------+--------+---------+---------+------------+------------+------------+------------\n");
		printk("\t %6u | %6u | %6u | %6u | %6u | %7u | %7u | %llx | %llx | %llx | %llu \n",
			layer->src.format, layer->src.width, layer->src.height, layer->src.bpp,
			layer->src.stride, layer->src.uStride, layer->src.vStride, layer->src.phy_addr,
			layer->src.vir_addr, layer->src.uOffset, layer->src.vOffset);

		printk("\n");
		printk("\t         src_rect         |          dst_rect         |       src_rect_mask       \n");
		printk("\t--------------------------+---------------------------+---------------------------\n");
		printk("\t[%5d,%5d,%5d,%5d] | [%5d,%5d,%5d,%5d] | [%5d,%5d,%5d,%5d] \n",
			layer->src_rect.x, layer->src_rect.y,
			layer->src_rect.w, layer->src_rect.h,
			layer->dst_rect.x, layer->dst_rect.y,
			layer->dst_rect.w, layer->dst_rect.h,
			layer->src_rect_mask.x, layer->src_rect_mask.y,
			layer->src_rect_mask.w, layer->src_rect_mask.h);
	}

	printk("\n");

	if (!ov_data->wb_enable)
		return;

	wb_layer = &(ov_data->wb_layer_info);
	printk("    wb info:\n");

	printk("\n");
	printk("\t format |  width | height |  bpp   | stride | uStride | vStride |  phy_addr  |  vir_addr  | uOffset | vOffset \n");
	printk("\t--------+--------+--------+--------+--------+---------+---------+------------+------------+------------+------------\n");
	printk("\t %6u | %6u | %6u | %6u | %6u | %7u | %7u | %llx | %llx | %llx | %llx \n",
			wb_layer->dst.format, wb_layer->dst.width, wb_layer->dst.height, wb_layer->dst.bpp,
			wb_layer->dst.stride, wb_layer->dst.uStride, wb_layer->dst.vStride, wb_layer->dst.phy_addr,
			wb_layer->dst.vir_addr, wb_layer->dst.uOffset, wb_layer->dst.vOffset);

	printk("\n");
	printk("\t         src_rect         |          dst_rect         | chn_idx | transform \n");
	printk("\t--------------------------+---------------------------|---------|---------|\n");
	printk("\t[%5d,%5d,%5d,%5d] | [%5d,%5d,%5d,%5d] | %7d | %9d \n",
		wb_layer->src_rect.x, wb_layer->src_rect.y,
		wb_layer->src_rect.w, wb_layer->src_rect.h,
		wb_layer->dst_rect.x, wb_layer->dst_rect.y,
		wb_layer->dst_rect.w, wb_layer->dst_rect.h,
		wb_layer->chn_idx, wb_layer->transform);

	printk("\n");
}

int hisi_dss_check_layer_par(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	if (layer->need_cap & (CAP_BASE | CAP_DIM))
		return 0;

	if (layer->src_rect.x < 0 || layer->src_rect.y < 0 ||
		layer->src_rect.w <= 0 || layer->src_rect.h <= 0) {
		HISI_FB_ERR("src_rect(%d, %d, %d, %d) is out of range!\n",
			layer->src_rect.x, layer->src_rect.y,
			layer->src_rect.w, layer->src_rect.h);
		return -EINVAL;
	}

	if (layer->dst_rect.x < 0 || layer->dst_rect.y < 0 ||
		layer->dst_rect.w <= 0 || layer->dst_rect.h <= 0) {
		HISI_FB_ERR("dst_rect(%d, %d, %d, %d) is out of range!\n",
			layer->dst_rect.x, layer->dst_rect.y,
			layer->dst_rect.w, layer->dst_rect.h);
		return -EINVAL;
	}

	return 0;
}


/*******************************************************************************
**
*/
static bool isChannelBelongtoDPE(int dpe_type, int chn_idx)
{
	if (dpe_type == DSS_ENG_DPE0) {
		if (chn_idx >= DPE0_CHN0 && chn_idx <= DPE0_CHN3)
			return true;
	} else if (dpe_type == DSS_ENG_DPE1) {
		if (chn_idx >= DPE1_CHN0 && chn_idx <= DPE1_CHN1)
			return true;
	} else if (dpe_type == DSS_ENG_DPE2) {
		if (chn_idx >= DPE2_CHN0 && chn_idx <= DPE2_CHN3)
			return true;
	} else if (dpe_type == DSS_ENG_DPE3) {
		if (chn_idx >= DPE3_CHN0 && chn_idx <= DPE3_CHN3)
			return true;
	} else if (dpe_type == DSS_ENG_WBE0) {
		if (chn_idx >= WBE0_CHN0 && chn_idx <= WBE0_CHN1)
			return true;
	} else if (dpe_type ==	DSS_ENG_WBE1) {
		if (chn_idx >= WBE1_CHN0 && chn_idx <= WBE1_CHN1)
			return true;
	} else {
		HISI_FB_ERR("not support this dpe_type(%d)!\n", dpe_type);
	}

	return false;
}

static bool hasChannelBelongtoDPE(int dpe_type, struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req)
{
	dss_layer_t *layer = NULL;
	int chn_idx = 0;
	int i = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	for (i = 0; i < pov_req->layer_nums; i++) {
		layer = &(pov_req->layer_infos[i]);
		chn_idx = layer->chn_idx;

		if (dpe_type == DSS_ENG_DPE0) {
			if (chn_idx >= DPE0_CHN0 && chn_idx <= DPE0_CHN3)
				return true;
		} else if (dpe_type == DSS_ENG_DPE1) {
			if (chn_idx >= DPE1_CHN0 && chn_idx <= DPE1_CHN1)
				return true;
		} else if (dpe_type == DSS_ENG_DPE2) {
			if (chn_idx >= DPE2_CHN0 && chn_idx <= DPE2_CHN3)
				return true;
		} else if (dpe_type == DSS_ENG_DPE3) {
			if (chn_idx >= DPE3_CHN0 && chn_idx <= DPE3_CHN3)
				return true;
		} else {
			HISI_FB_ERR("not support this dpe_type(%d)!\n", dpe_type);
			break;
		}
	}

	return false;
}

int hisi_get_hal_format(struct fb_info *info)
{
	struct fb_var_screeninfo *var = NULL;
	int hal_format = 0;
	int rgb_order = 0;

	BUG_ON(info == NULL);
	var = &info->var;

	switch (var->bits_per_pixel) {
	case 16:
		if (var->blue.offset == 0) {
			rgb_order = 0;
			if (var->red.offset == 8) {
				hal_format = (var->transp.offset == 12) ?
					HISI_FB_PIXEL_FORMAT_BGRA_4444 : HISI_FB_PIXEL_FORMAT_BGRX_4444;
			} else if (var->red.offset == 10) {
				hal_format = (var->transp.offset == 12) ?
					HISI_FB_PIXEL_FORMAT_BGRA_5551 : HISI_FB_PIXEL_FORMAT_BGRX_5551;
			} else if (var->red.offset == 11) {
				hal_format = HISI_FB_PIXEL_FORMAT_RGB_565;
			} else {
				goto err_return;
			}
		} else {
			rgb_order = 1;
			if (var->blue.offset == 8) {
				hal_format = (var->transp.offset == 12) ?
					HISI_FB_PIXEL_FORMAT_RGBA_4444 : HISI_FB_PIXEL_FORMAT_RGBX_4444;
			} else if (var->blue.offset == 10) {
				hal_format = (var->transp.offset == 12) ?
					HISI_FB_PIXEL_FORMAT_RGBA_5551 : HISI_FB_PIXEL_FORMAT_RGBX_5551;
			} else if (var->blue.offset == 11) {
				hal_format = HISI_FB_PIXEL_FORMAT_BGR_565;
			} else {
				goto err_return;
			}
		}
		break;

	case 32:
		if (var->blue.offset == 0) {
			rgb_order = 0;
			hal_format = (var->transp.length == 8) ?
				HISI_FB_PIXEL_FORMAT_BGRA_8888 : HISI_FB_PIXEL_FORMAT_BGRX_8888;
		} else {
			rgb_order = 1;
			hal_format = (var->transp.length == 8) ?
				HISI_FB_PIXEL_FORMAT_RGBA_8888 : HISI_FB_PIXEL_FORMAT_RGBA_8888;
		}
		break;

	default:
		goto err_return;
		break;
	}

	return hal_format;

err_return:
	HISI_FB_ERR("not support this bits_per_pixel(%d)!\n", var->bits_per_pixel);
	return -1;
}

static bool hal_format_has_alpha(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_RGBA_4444:
	case HISI_FB_PIXEL_FORMAT_RGBA_5551:
	case HISI_FB_PIXEL_FORMAT_RGBA_8888:

	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
	case HISI_FB_PIXEL_FORMAT_BGRA_5551:
	case HISI_FB_PIXEL_FORMAT_BGRA_8888:
		return true;

	default:
		return false;
	}
}

bool isYUVPackage(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
		return true;

	default:
		return false;
	}
}

bool isYUVSemiPlanar(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
		return true;

	default:
		return false;
	}
}

bool isYUVPlanar(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		return true;

	default:
		return false;
	}
}

bool isYUV(int format)
{
	return isYUVPackage(format) ||
		isYUVSemiPlanar(format) ||
		isYUVPlanar(format);
}

static bool is_YUV_SP_420(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
		return true;

	default:
		return false;
	}
}

#if 0
static bool is_YUV_SP_422(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
		return true;

	default:
		return false;
	}
}
#endif

static bool is_YUV_P_420(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		return true;

	default:
		return false;
	}
}

static bool is_YUV_P_422(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
		return true;

	default:
		return false;
	}
}

bool isNeedDither(int fmt)
{
	return (fmt == DFC_PIXEL_FORMAT_RGB_565) ||
		(fmt == DFC_PIXEL_FORMAT_BGR_565);
}

static bool isNeedRectClip(dss_rect_ltrb_t clip_rect)
{
	return ((clip_rect.left > 0) || (clip_rect.top > 0) ||
		(clip_rect.right > 0) || (clip_rect.bottom > 0));
}

static bool isSrcRectMasked(dss_layer_t *dss_layer, int aligned_pixel)
{
	BUG_ON(dss_layer == NULL);

	return ((dss_layer->src_rect_mask.w != 0) &&
		(dss_layer->src_rect_mask.h != 0) &&
		(ALIGN_DOWN(dss_layer->src_rect_mask.x + dss_layer->src_rect_mask.w, aligned_pixel) > 1));
}

static uint32_t isNeedRdmaStretchBlt(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	uint32_t v_stretch_ratio_threshold = 0;
	uint32_t v_stretch_ratio = 0;

	BUG_ON(layer == NULL);

	//FIXME:
	if (hisifd->ovl_type == DSS_OVL_ADP)
		return 0;

	if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
		if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
			v_stretch_ratio_threshold = ((layer->src_rect.h + layer->dst_rect.w - 1) / layer->dst_rect.w);
			//v_stretch_ratio = ((layer->src_rect.h / layer->dst_rect.w) / 2) * 2;
			v_stretch_ratio = (((layer->src_rect.h / layer->dst_rect.w) + 1) / 2) * 2;
		} else {
			v_stretch_ratio_threshold = ((layer->src_rect.h + layer->dst_rect.h - 1) / layer->dst_rect.h);
			//v_stretch_ratio = ((layer->src_rect.h / layer->dst_rect.h) / 2) * 2;
			v_stretch_ratio = (((layer->src_rect.h / layer->dst_rect.h) + 1) / 2) * 2;
		}
	} else {
		if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
			v_stretch_ratio_threshold = ((layer->src_rect.h + layer->dst_rect.w - 1) / layer->dst_rect.w);
			v_stretch_ratio = (layer->src_rect.h / layer->dst_rect.w);
		} else {
			v_stretch_ratio_threshold = ((layer->src_rect.h + layer->dst_rect.h - 1) / layer->dst_rect.h);
			v_stretch_ratio = (layer->src_rect.h / layer->dst_rect.h);
		}
	}

	if (v_stretch_ratio_threshold <= g_scf_stretch_threshold)
		v_stretch_ratio = 0;

	return v_stretch_ratio;
}

static bool is_need_rdma_rot_burst4(dss_layer_t *layer)
{
	BUG_ON(layer == NULL);

	if ((layer->chn_idx >= DPE3_CHN0) && (layer->chn_idx <= DPE3_CHN3) &&
		(layer->transform & HISI_FB_TRANSFORM_ROT_90) &&
		(!layer->src.is_tile) && (layer->src.bpp == 4)) {
		return true;
	}

	return false;
}

static void hisi_adjust_clip_rect(dss_layer_t *layer, dss_rect_ltrb_t *clip_rect)
{
	uint32_t temp = 0;

	switch (layer->transform) {
	case HISI_FB_TRANSFORM_NOP:
		// do nothing
		break;
	case HISI_FB_TRANSFORM_FLIP_V:
		{
			temp = clip_rect->top;
			clip_rect->top = clip_rect->bottom;
			clip_rect->bottom = temp;
		}
		break;
	case HISI_FB_TRANSFORM_FLIP_H:
		{
			temp = clip_rect->left;
			clip_rect->left = clip_rect->right;
			clip_rect->right = temp;
		}
		break;
	case HISI_FB_TRANSFORM_ROT_90:
		{
			temp = clip_rect->left;
			clip_rect->left = clip_rect->bottom;
			clip_rect->bottom = clip_rect->right;
			clip_rect->right = clip_rect->top;
			clip_rect->top = temp;
		}
		break;
	case HISI_FB_TRANSFORM_ROT_180:
		{
			temp = clip_rect->left;
			clip_rect->left =  clip_rect->right;
			clip_rect->right = temp ;

			temp = clip_rect->top;
			clip_rect->top =  clip_rect->bottom;
			clip_rect->bottom = temp;
		}
		break;
	case HISI_FB_TRANSFORM_ROT_270:
		{
			temp = clip_rect->left;
			clip_rect->left = clip_rect->top;
			clip_rect->top = clip_rect->right;
			clip_rect->right = clip_rect->bottom;
			clip_rect->bottom = temp;
		}
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H):
		{
			temp = clip_rect->left;
			clip_rect->left = clip_rect->bottom;
			clip_rect->bottom = clip_rect->right;
			clip_rect->right = clip_rect->top;
			clip_rect->top = temp;

			temp = clip_rect->top;
			clip_rect->top = clip_rect->bottom;
			clip_rect->bottom = temp;
		}
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V):
		{
			temp = clip_rect->left;
			clip_rect->left = clip_rect->bottom;
			clip_rect->bottom = clip_rect->right;
			clip_rect->right = clip_rect->top;
			clip_rect->top = temp;

			temp = clip_rect->left;
			clip_rect->left = clip_rect->right;
			clip_rect->right = temp;
		}
		break;
	default:
		HISI_FB_ERR("not supported this transform(%d)!", layer->transform);
		break;
	}
}

static uint32_t hisi_calculate_display_addr(bool mmu_enable, dss_layer_t *layer,
	dss_rect_ltrb_t *aligned_rect, int add_type)
{
	uint32_t addr = 0;
	uint32_t src_addr = 0;
	uint32_t stride = 0;
	int bpp = 0;
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;

	left = aligned_rect->left;
	right = aligned_rect->right;
	top = aligned_rect->top;
	bottom = aligned_rect->bottom;

	if (add_type == DSS_ADDR_PLANE0) {
		stride = layer->src.stride;
		src_addr = mmu_enable ? layer->src.vir_addr : layer->src.phy_addr;
		bpp = layer->src.bpp;
	} else if (add_type == DSS_ADDR_PLANE1) {
		stride = layer->src.uStride;
		src_addr = mmu_enable ? (layer->src.vir_addr + layer->src.uOffset) :
			(layer->src.phy_addr + layer->src.uOffset);
		bpp = 1;

		if (is_YUV_P_420(layer->src.format) || is_YUV_P_422(layer->src.format)) {
			left /= 2;
			right /= 2;
		}

		if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
			top /= 2;
			bottom /= 2;
		}
	} else if (add_type == DSS_ADDR_PLANE2) {
		stride = layer->src.vStride;
		src_addr = mmu_enable ? (layer->src.vir_addr + layer->src.vOffset) :
			(layer->src.phy_addr + layer->src.vOffset);
		bpp = 1;

		if (is_YUV_P_420(layer->src.format) || is_YUV_P_422(layer->src.format)) {
			left /= 2;
			right /= 2;
		}

		if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
			top /= 2;
			bottom /= 2;
		}
	} else {
		HISI_FB_ERR("NOT SUPPORT this add_type(%d).\n", add_type);
		BUG_ON(1);
	}

	switch(layer->transform) {
	case HISI_FB_TRANSFORM_NOP:
		addr = src_addr + top * stride + left * bpp;
		break;
	case HISI_FB_TRANSFORM_FLIP_H:
		addr = src_addr + top * stride + right * bpp;
		break;
	case HISI_FB_TRANSFORM_FLIP_V:
		addr = src_addr + bottom * stride + left * bpp;
		break;
	case HISI_FB_TRANSFORM_ROT_90:
		addr = src_addr + bottom * stride + left * bpp;
		break;
	case HISI_FB_TRANSFORM_ROT_180:
		addr = src_addr + bottom * stride + right * bpp;
		break;
	case HISI_FB_TRANSFORM_ROT_270:
		addr = src_addr + top * stride + right * bpp;
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H):
		addr = src_addr + bottom * stride + right * bpp;
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V):
		addr = src_addr + top * stride + left * bpp;
		break;
	default:
		HISI_FB_ERR("not supported this transform(%d)!", layer->transform);
		break;
	}

	return addr;
}

static int hisi_get_chnIndex4crossSwitch(uint32_t need_cap, int32_t chn_idx)
{
	int ret = -1;

	if (need_cap & CAP_CROSS_SWITCH) {
		if (chn_idx == DPE0_CHN0)
			ret = DPE0_CHN1;
		else if (chn_idx == DPE0_CHN1)
			ret = DPE0_CHN0;
		else if (chn_idx == DPE2_CHN0)
			ret = DPE2_CHN1;
		else if (chn_idx== DPE2_CHN1)
			ret = DPE2_CHN0;
		else
			HISI_FB_ERR("CAP_CROSS_SWITCH not support this chn_idx(%d)!", chn_idx);
	} else {
		ret = chn_idx;
	}

	return  ret;
}

int hisi_get_rot_index(int chn_idx)
{
	int ret = 0;

	switch(chn_idx) {
	case DPE0_CHN0:
		ret = 0;
		break;

	case DPE2_CHN0:
	case WBE0_CHN1:
		ret = 1;
		break;

	case DPE3_CHN0:
		ret = 4;
		break;
	case DPE3_CHN1:
	case WBE1_CHN1:
		ret = 2;
		break;
	case DPE3_CHN2:
		ret = 3;
		break;

	default:
		ret = -1;
		//HISI_FB_ERR("not support this chn_idx(%d)!\n", chn_idx);
		break;
	}

	return ret;
}

int hisi_get_scf_index(int chn_idx)
{
	int ret = 0;

	switch(chn_idx) {
	case DPE0_CHN0:
	case DPE0_CHN1:
		ret = 0;
		break;

	case DPE1_CHN0:
	case WBE0_CHN1:
		ret = 1;
		break;

	case DPE2_CHN0:
	case DPE2_CHN1:
		ret = 2;
		break;

	case DPE3_CHN1:
	case WBE1_CHN1:
		ret = 3;
		break;

	case DPE3_CHN0:
		ret = 4;
		break;

	default:
		ret = -1;
		//HISI_FB_ERR("not support this chn_idx(%d)!\n", chn_idx);
		break;
	}

	return ret;
}

static int hisi_pixel_format_hal2mmu(int format)
{
	int ret = 0;

	switch(format) {
	case HISI_FB_PIXEL_FORMAT_RGB_565:
	case HISI_FB_PIXEL_FORMAT_RGBX_4444:
	case HISI_FB_PIXEL_FORMAT_RGBA_4444:
	case HISI_FB_PIXEL_FORMAT_RGBX_5551:
	case HISI_FB_PIXEL_FORMAT_RGBA_5551:

	case HISI_FB_PIXEL_FORMAT_BGR_565:
	case HISI_FB_PIXEL_FORMAT_BGRX_4444:
	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
	case HISI_FB_PIXEL_FORMAT_BGRX_5551:
	case HISI_FB_PIXEL_FORMAT_BGRA_5551:

	case HISI_FB_PIXEL_FORMAT_RGBX_8888:
	case HISI_FB_PIXEL_FORMAT_RGBA_8888:
	case HISI_FB_PIXEL_FORMAT_BGRX_8888:
	case HISI_FB_PIXEL_FORMAT_BGRA_8888:

	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
		ret = MMU_PIXEL_FORMAT_PACKAGE;
		break;

	/* Planar */
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:

	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		ret = MMU_PIXEL_FORMAT_YUV_PLANAR;
		break;

	/* Semi-Planar */
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:

	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
		ret = MMU_PIXEL_FORMAT_YUV_SEMI_PLANAR;
		break;

	default:
		HISI_FB_ERR("not support format(%d)!\n", format);
		ret = -1;
		break;
	}

	return ret;
}

static int hisi_pixel_format_hal2dma(int format)
{
	int ret = 0;

	switch(format) {
	case HISI_FB_PIXEL_FORMAT_RGB_565:
	case HISI_FB_PIXEL_FORMAT_RGBX_4444:
	case HISI_FB_PIXEL_FORMAT_RGBA_4444:
	case HISI_FB_PIXEL_FORMAT_RGBX_5551:
	case HISI_FB_PIXEL_FORMAT_RGBA_5551:

	case HISI_FB_PIXEL_FORMAT_BGR_565:
	case HISI_FB_PIXEL_FORMAT_BGRX_4444:
	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
	case HISI_FB_PIXEL_FORMAT_BGRX_5551:
	case HISI_FB_PIXEL_FORMAT_BGRA_5551:
		ret = DMA_PIXEL_FORMAT_RGB16BIT;
		break;

	case HISI_FB_PIXEL_FORMAT_RGBX_8888:
	case HISI_FB_PIXEL_FORMAT_RGBA_8888:
	case HISI_FB_PIXEL_FORMAT_BGRX_8888:
	case HISI_FB_PIXEL_FORMAT_BGRA_8888:
		ret = DMA_PIXEL_FORMAT_RGB32BIT;
		break;

	/* Packet */
	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
		ret = DMA_PIXEL_FORMAT_YUV_422_I;
		break;
	/* Planar */
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
		ret = DMA_PIXEL_FORMAT_YUV_422_P_HP_Y;
		break;
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		ret = DMA_PIXEL_FORMAT_YUV_420_P_Y;
		break;
	/* Semi-Planar */
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
		ret = DMA_PIXEL_FORMAT_YUV_422_SP_HP_Y;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
		ret = DMA_PIXEL_FORMAT_YUV_420_SP_Y;
		break;

	default:
		HISI_FB_ERR("not support format(%d)!\n", format);
		ret = -1;
		break;
	}

	return ret;
}

static int hisi_transform_hal2dma(int transform, int chn_index)
{
	int ret = 0;

	switch(transform) {
	case HISI_FB_TRANSFORM_NOP:
		ret = DSS_TRANSFORM_NOP;
		break;
	case HISI_FB_TRANSFORM_FLIP_H:
		ret = DSS_TRANSFORM_FLIP_H;
		break;
	case HISI_FB_TRANSFORM_FLIP_V:
		ret = DSS_TRANSFORM_FLIP_V;
		break;
	case HISI_FB_TRANSFORM_ROT_90:
		if ((chn_index >= WBE0_CHN0) && (chn_index <= WBE1_CHN1))
			ret = DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_H;
		else
			ret = DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_V;
		break;
	case HISI_FB_TRANSFORM_ROT_180:
		ret = DSS_TRANSFORM_FLIP_V |DSS_TRANSFORM_FLIP_H;
		break;
	case HISI_FB_TRANSFORM_ROT_270:
		if ((chn_index >= WBE0_CHN0) && (chn_index <= WBE1_CHN1))
			ret = DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_V;
		else
			ret = DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_H;
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H):
		if ((chn_index >= WBE0_CHN0) && (chn_index <= WBE1_CHN1))
			ret = DSS_TRANSFORM_ROT;
		else
			ret =  DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_V | DSS_TRANSFORM_FLIP_H;
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V):
		if ((chn_index >= WBE0_CHN0) && (chn_index <= WBE1_CHN1))
			ret = DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_H | DSS_TRANSFORM_FLIP_V;
		else
			ret = DSS_TRANSFORM_ROT;
		break;
	default:
		ret = -1;
		HISI_FB_ERR("Transform %d is not supported", transform);
		break;
	}

	return ret;
}

static int hisi_chnIndex2dmaIndex(int chn_idx)
{
	int ret = 0;

	switch(chn_idx) {
	case DPE0_CHN0:
	case DPE1_CHN0:
	case DPE2_CHN0:
	case DPE3_CHN0:
		ret = 0;
		break;
	case DPE0_CHN1:
	case DPE2_CHN1:
	case DPE3_CHN1:
		ret = 2;
		break;
	case DPE0_CHN2:
	case DPE2_CHN2:
	case DPE3_CHN2:
		ret = 3;
		break;
	case DPE0_CHN3:
	case DPE1_CHN1:
	case DPE2_CHN3:
	case DPE3_CHN3:
		ret = 4;
		break;
	default:
		ret = -1;
		HISI_FB_ERR("not support this chn_idx(%d)!\n", chn_idx);
		break;
	}

	return ret;
}

static int hisi_pixel_format_hal2dfc(int format, int transform)
{
	int ret = 0;

	switch (format) {
	case HISI_FB_PIXEL_FORMAT_RGB_565:
		ret = DFC_PIXEL_FORMAT_RGB_565;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_4444:
		ret = DFC_PIXEL_FORMAT_XBGR_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_4444:
		ret = DFC_PIXEL_FORMAT_ABGR_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_5551:
		ret = DFC_PIXEL_FORMAT_XBGR_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_5551:
		ret = DFC_PIXEL_FORMAT_ABGR_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_8888:
		ret = DFC_PIXEL_FORMAT_XBGR_8888;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_8888:
		ret = DFC_PIXEL_FORMAT_ABGR_8888;
		break;

	case HISI_FB_PIXEL_FORMAT_BGR_565:
		ret = DFC_PIXEL_FORMAT_BGR_565;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRX_4444:
		ret = DFC_PIXEL_FORMAT_XRGB_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
		ret = DFC_PIXEL_FORMAT_ARGB_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRX_5551:
		ret = DFC_PIXEL_FORMAT_XRGB_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRA_5551:
		ret = DFC_PIXEL_FORMAT_ARGB_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRX_8888:
		ret = DFC_PIXEL_FORMAT_XRGB_8888;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRA_8888:
		ret = DFC_PIXEL_FORMAT_ARGB_8888;
		break;

	/* YUV */
	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
		ret = (transform & HISI_FB_TRANSFORM_ROT_90) ?
			DFC_PIXEL_FORMAT_YUV444 : DFC_PIXEL_FORMAT_YUYV422;
		break;

	/* YUV Semi-planar */
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP: /* NV16 */
		ret = (transform & HISI_FB_TRANSFORM_ROT_90) ?
			DFC_PIXEL_FORMAT_YUV444 : DFC_PIXEL_FORMAT_YUYV422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
		ret = (transform & HISI_FB_TRANSFORM_ROT_90) ?
			DFC_PIXEL_FORMAT_YVU444 : DFC_PIXEL_FORMAT_YVYU422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
		ret = DFC_PIXEL_FORMAT_YVYU422;
		break;

	/* YUV Planar */
	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P: /* HISI_FB_PIXEL_FORMAT_YV12 */
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;

	default:
		HISI_FB_ERR("not support format(%d)!\n", format);
		ret = -1;
		break;
	}

	return ret;
}

static int hisi_dfc_get_bpp(int dfc_format)
{
	int ret = 0;

	switch (dfc_format) {
	case DFC_PIXEL_FORMAT_RGB_565:
	case DFC_PIXEL_FORMAT_XRGB_4444:
	case DFC_PIXEL_FORMAT_ARGB_4444:
	case DFC_PIXEL_FORMAT_XRGB_5551:
	case DFC_PIXEL_FORMAT_ARGB_5551:

	case DFC_PIXEL_FORMAT_BGR_565:
	case DFC_PIXEL_FORMAT_XBGR_4444:
	case DFC_PIXEL_FORMAT_ABGR_4444:
	case DFC_PIXEL_FORMAT_XBGR_5551:
	case DFC_PIXEL_FORMAT_ABGR_5551:
		ret = 2;
		break;

	case DFC_PIXEL_FORMAT_XRGB_8888:
	case DFC_PIXEL_FORMAT_ARGB_8888:
	case DFC_PIXEL_FORMAT_XBGR_8888:
	case DFC_PIXEL_FORMAT_ABGR_8888:
		ret = 4;
		break;

	case DFC_PIXEL_FORMAT_YUV444:
	case DFC_PIXEL_FORMAT_YVU444:
		ret = 3;
		break;

	case DFC_PIXEL_FORMAT_YUYV422:
	case DFC_PIXEL_FORMAT_YVYU422:
	case DFC_PIXEL_FORMAT_VYUY422:
	case DFC_PIXEL_FORMAT_UYVY422:
		ret = 2;
		break;

	default:
		HISI_FB_ERR("not support format(%d)!\n", dfc_format);
		ret = -1;
		break;
	}

	return ret;
}


/******************************************************************************/
void hisi_dss_unflow_handler(struct hisi_fb_data_type *hisifd, bool unmask)
{
	uint32_t tmp = 0;

	BUG_ON(hisifd == NULL);

	if (hisifd->ovl_type == DSS_OVL_PDP) {
		tmp = inp32(hisifd->dss_base + PDP_LDI_CPU_IRQ_MSK);
		if (unmask) {
			tmp &= ~BIT_LDI_UNFLOW_INT;
		} else {
			tmp |= BIT_LDI_UNFLOW_INT;
		}
		outp32(hisifd->dss_base + PDP_LDI_CPU_IRQ_MSK, tmp);

		if (g_debug_mmu_error) {
			outp32(hisifd->dss_base + DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET +
				PDP_DPE0_MMU_CPU_IRQ_MSK, 0xFFFFF);
			outp32(hisifd->dss_base + DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET +
				PDP_DPE2_MMU_CPU_IRQ_MSK, 0xFFFFF);
		}
	} else if (hisifd->ovl_type == DSS_OVL_SDP) {
		tmp = inp32(hisifd->dss_base + SDP_LDI_CPU_IRQ_MSK);
		if (unmask) {
			tmp &= ~BIT_LDI_UNFLOW_INT;
		} else {
			tmp |= BIT_LDI_UNFLOW_INT;
		}
		outp32(hisifd->dss_base + SDP_LDI_CPU_IRQ_MSK, tmp);
	} else {
		; /* do nothing */
	}

	//hisifd->dss_exception.underflow_exception = 1;
}

void hisi_dss_config_ok_begin(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ovl_cfg_ok_base = NULL;
	uint32_t module_base = 0;

	BUG_ON(hisifd == NULL);

	module_base = g_dss_module_ovl_base[hisifd->ovl_type][MODULE_OVL_CFG_OK];
	ovl_cfg_ok_base = hisifd->dss_base + module_base;
	BUG_ON(module_base == 0);

	hisifd->set_reg(hisifd, ovl_cfg_ok_base, 0x0, 1, 0);
}

void hisi_dss_config_ok_end(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ovl_cfg_ok_base = NULL;
	uint32_t module_base = 0;

	BUG_ON(hisifd == NULL);

	module_base = g_dss_module_ovl_base[hisifd->ovl_type][MODULE_OVL_CFG_OK];
	ovl_cfg_ok_base = hisifd->dss_base + module_base;
	BUG_ON(module_base == 0);

	hisifd->set_reg(hisifd, ovl_cfg_ok_base, 0x1, 1, 0);
}

void hisi_dfs_wbe_enable(struct hisi_fb_data_type *hisifd)
{
	char __iomem *wbe_en_base = NULL;
	uint32_t module_base = 0;

	BUG_ON(hisifd == NULL);

	module_base = g_dss_module_ovl_base[hisifd->ovl_type][MODULE_OVL_WBE_EN];
	wbe_en_base = hisifd->dss_base + module_base;
	BUG_ON(module_base == 0);

	hisifd->set_reg(hisifd, wbe_en_base, 0x1, 1, 0);
}

void hisi_dfs_wbe_disable(struct hisi_fb_data_type *hisifd)
{
	char __iomem *wbe_en_base = NULL;
	uint32_t module_base = 0;

	BUG_ON(hisifd == NULL);

	module_base = g_dss_module_ovl_base[hisifd->ovl_type][MODULE_OVL_WBE_EN];
	wbe_en_base = hisifd->dss_base + module_base;
	BUG_ON(module_base == 0);

	hisifd->set_reg(hisifd, wbe_en_base, 0x0, 1, 0);
}

void hisi_adp_offline_start_enable(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req)
{
	dss_wb_layer_t *wb_layer = NULL;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	wb_layer = &(pov_req->wb_layer_info);
	BUG_ON(wb_layer == NULL);

	if (wb_layer->chn_idx == WBE1_CHN0) {
		hisifd->set_reg(hisifd, hisifd->dss_base + DSS_GLB_ADP_OFFLINE_START0, 0x1, 32, 0);
	} else if (wb_layer->chn_idx == WBE1_CHN1) {
		hisifd->set_reg(hisifd, hisifd->dss_base + DSS_GLB_ADP_OFFLINE_START1, 0x1, 32, 0);
	} else {
		HISI_FB_ERR("not support this chn_idx(%d)!\n", wb_layer->chn_idx);
	}
}

void hisi_adp_offline_start_disable(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req)
{
	dss_wb_layer_t *wb_layer = NULL;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	wb_layer = &(pov_req->wb_layer_info);
	BUG_ON(wb_layer == NULL);

	if (wb_layer->chn_idx == WBE1_CHN0) {
		hisifd->set_reg(hisifd, hisifd->dss_base + DSS_GLB_ADP_OFFLINE_START0, 0x0, 32, 0);
	} else if (wb_layer->chn_idx == WBE1_CHN1) {
		hisifd->set_reg(hisifd, hisifd->dss_base + DSS_GLB_ADP_OFFLINE_START1, 0x0, 32, 0);
	} else {
		HISI_FB_ERR("not support this chn_idx(%d)!\n", wb_layer->chn_idx);
	}
}

int hisi_dss_rptb_handler(struct hisi_fb_data_type *hisifd, bool is_wb, int rptb_info_idx)
{
	int i = 0;
	dss_rptb_info_t *rptb_info_prev = NULL;
	dss_rptb_info_t *rptb_info_cur = NULL;

	if (is_wb) {
		rptb_info_prev = &(hisifd->dss_wb_rptb_info_prev[rptb_info_idx]);
		rptb_info_cur = &(hisifd->dss_wb_rptb_info_cur[rptb_info_idx]);
	} else {
		rptb_info_prev = &(hisifd->dss_rptb_info_prev[rptb_info_idx]);
		rptb_info_cur = &(hisifd->dss_rptb_info_cur[rptb_info_idx]);
	}

	if (hisifd->ovl_type == DSS_OVL_ADP) {
		for (i = 0; i < DSS_ROT_MAX; i++) {
			if (rptb_info_cur->rptb_size[i] > 0) {
				hisi_dss_rptb_free(g_rptb_gen_pool,
					rptb_info_cur->rptb_offset[i], rptb_info_cur->rptb_size[i]);
				rptb_info_cur->rptb_size[i] = 0;
				rptb_info_cur->rptb_offset[i] = 0;
			}
		}

		memset(rptb_info_prev, 0, sizeof(dss_rptb_info_t));
		memset(rptb_info_cur, 0, sizeof(dss_rptb_info_t));
	} else {
	#if 0
		for (i = 0; i < DSS_ROT_MAX; i++) {
			if (rptb_info_cur->rptb_size[i] > 0) {
				hisi_dss_rptb_free(g_rptb_gen_pool,
					rptb_info_cur->rptb_offset[i], rptb_info_cur->rptb_size[i]);
				rptb_info_cur->rptb_size[i] = 0;
				rptb_info_cur->rptb_offset[i] = 0;
			}
		}

		memset(rptb_info_prev, 0, sizeof(dss_rptb_info_t));
		memset(rptb_info_cur, 0, sizeof(dss_rptb_info_t));
	#else
		for (i = 0; i < DSS_ROT_MAX; i++) {
			if (rptb_info_prev->rptb_size[i] > 0) {
				hisi_dss_rptb_free(g_rptb_gen_pool,
					rptb_info_prev->rptb_offset[i], rptb_info_prev->rptb_size[i]);
				rptb_info_prev->rptb_size[i] = 0;
				rptb_info_prev->rptb_offset[i] = 0;
			}
		}
		memcpy(rptb_info_prev, rptb_info_cur, sizeof(dss_rptb_info_t));
		memset(rptb_info_cur, 0, sizeof(dss_rptb_info_t));
	#endif
	}

	return 0;
}

int hisi_dss_handle_prev_ovl_req(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req)
{
	dss_layer_t *layer = NULL;
	int chn_idx = 0;
	int chn_idx_tmp = 0;
	int i = 0;
	int k = 0;
	int idx_tmp = 0;
	uint32_t format = 0;
	uint32_t need_cap = 0;
	uint32_t tmp = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	if (hisifd->dss_exception.underflow_handler == 1) {
		for (k = 0; k < DSS_CHN_MAX; k++) {
			if (hisifd->dss_exception.chn_used[k] == 1) {
				hisifd->dss_glb.chn_ctl_used[k] = 1;
				if (isYUVPlanar(hisifd->dss_exception.chn_format_used[k])
					&& (k == DPE3_CHN0)) {
					hisifd->dss_glb.chn_ctl_used[k + 1] = 1;
				}
			}
		}

		hisifd->dss_exception.underflow_handler = 0;
	}

	for (i = 0; i < pov_req->layer_nums; i++) {
		layer = &pov_req->layer_infos[i];
		chn_idx = layer->chn_idx;
		format = layer->src.format;
		need_cap = layer->need_cap;

		/* CHN CTRL */
		if (hisifd->dss_exception.underflow_exception == 1) {
			hisifd->dss_exception.chn_used[chn_idx] = 1;
			hisifd->dss_exception.chn_format_used[chn_idx] = format;
		} else {
			hisifd->dss_glb.chn_ctl_used[chn_idx] = 1;
			/*if (isYUVSemiPlanar(format) || isYUVPlanar(format)) {
				hisifd->dss_glb.chn_ctl_used[chn_idx] = 1;
			}*/
			if (isYUVPlanar(format) && (chn_idx == DPE3_CHN0)) {
				chn_idx_tmp = chn_idx + 1;
				hisifd->dss_glb.chn_ctl_used[chn_idx_tmp] = 1;
			}
		}

		/* RDMA */
		hisifd->dss_module.rdma0_used[chn_idx] = 1;
		if (isYUVSemiPlanar(format) || isYUVPlanar(format)) {
			hisifd->dss_module.rdma1_used[chn_idx] = 1;
		}
		if (isYUVPlanar(format) && (chn_idx == DPE3_CHN0)) {
			chn_idx_tmp = chn_idx + 1;
			hisifd->dss_module.rdma0_used[chn_idx_tmp] = 1;
		}

		if (layer->src.mmu_enable) {
			/* MMU_DMA */
			hisifd->dss_module.mmu_dma0_used[chn_idx] = 1;

			if (isYUVSemiPlanar(format) || isYUVPlanar(format)) {
				hisifd->dss_module.mmu_dma1_used[chn_idx] = 1;
			}

			if (isYUVPlanar(format) && (chn_idx == DPE3_CHN0)) {
				chn_idx_tmp = chn_idx + 1;
				hisifd->dss_module.mmu_dma0_used[chn_idx_tmp] = 1;
			}

			/* MMU_RPTB */
			if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
				idx_tmp = hisi_get_rot_index(chn_idx);
				if (idx_tmp < 0) {
					HISI_FB_ERR("hisi_get_rot_index failed! chn_idx=%d.\n", chn_idx);
				} else {
					if (pov_req->wb_enable && (pov_req->wb_layer_info.chn_idx == WBE1_CHN1))
						tmp = hisifd->dss_rptb_info_cur[1].rptb_size[idx_tmp];
					else
						tmp = hisifd->dss_rptb_info_cur[0].rptb_size[idx_tmp];

					if (tmp > 0) {
						hisifd->dss_module.mmu_rptb_used = 1;
						hisifd->dss_module.mmu_rptb.mmu_rptb_idx_used[idx_tmp] = 1;
						hisifd->dss_module.mmu_rptb.mmu_rptb_ctl =
							set_bits32(hisifd->dss_module.mmu_rptb.mmu_rptb_ctl,
								(0 << idx_tmp) | (1 << (26 + idx_tmp)), 32, 0);

						hisifd->dss_glb.rot_tlb_scene_used[idx_tmp] = 1;
					}
				}
			}
		}

		/* FBDC */
		if (need_cap & CAP_FBDC) {
			hisifd->dss_module.fbdc_used[chn_idx] = 1;
		}

		/* DFC */
		hisifd->dss_module.dfc_used[chn_idx] = 1;

		if (need_cap & CAP_SCL) {
			/* SCF */
			idx_tmp = hisi_get_scf_index(chn_idx);
			if (idx_tmp < 0) {
				HISI_FB_ERR("hisi_get_scf_index failed! chn_idx=%d.\n", chn_idx);
			} else {
				hisifd->dss_module.scf_used[idx_tmp] = 1;
			}
		}

		/* SCP */
		if ((hisifd->panel_info.prefix_sharpness_support == 1) && isYUV(format)) {
			if (((chn_idx == DPE0_CHN0) && (need_cap & CAP_CROSS_SWITCH)) ||
				((chn_idx == DPE0_CHN1) && !(need_cap & CAP_CROSS_SWITCH)) ||
				((chn_idx == DPE2_CHN0) && (need_cap & CAP_CROSS_SWITCH)) ||
				((chn_idx == DPE2_CHN1) && !(need_cap & CAP_CROSS_SWITCH))) {
				hisifd->dss_module.scp_used[chn_idx] = 1;
			}
		}

		/* CSC */
		if (isYUV(format)) {
			chn_idx_tmp = hisi_get_chnIndex4crossSwitch(need_cap, chn_idx);
			if (chn_idx_tmp >= 0) {
				hisifd->dss_module.csc_used[chn_idx_tmp] = 1;
			}
		}

		/* OV */
		hisifd->dss_module.ov_used[hisifd->ovl_type] = 1;
	}

	if (hisifd->ovl_type != DSS_OVL_ADP)
		hisi_dss_rptb_handler(hisifd, false, 0);

	if (hisifd->dss_exception.underflow_exception == 1) {
		hisifd->dss_exception.underflow_exception = 0;
		hisifd->dss_exception.underflow_handler = 1;
	}

	return 0;
}

static int hisi_dss_lcd_refresh_direction_ctrl_layer(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, dss_layer_t *layer)
{
	struct hisi_panel_info *pinfo = NULL;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON(layer == NULL);

	pinfo = &(hisifd->panel_info);

	if ((pov_req->ovl_flags != DSS_SCENE_PDP_ONLINE) &&
		(pov_req->ovl_flags !=DSS_SCENE_SDP_ONLINE))
		return 0;

	if (pinfo->lcd_refresh_direction_ctrl == LCD_REFRESH_LEFT_TOP) {
		; //do nothing
	} else if (pinfo->lcd_refresh_direction_ctrl == LCD_REFRESH_RIGHT_TOP) {
		HISI_FB_INFO("fb%d, lcd_refresh_direction_ctrl(%d) not support!",
			hisifd->index, pinfo->lcd_refresh_direction_ctrl );
	} else if (pinfo->lcd_refresh_direction_ctrl == LCD_REFRESH_LEFT_BOTTOM) {
		HISI_FB_INFO("fb%d, lcd_refresh_direction_ctrl(%d) not support!",
			hisifd->index, pinfo->lcd_refresh_direction_ctrl );
	} else if (pinfo->lcd_refresh_direction_ctrl == LCD_REFRESH_RIGHT_BOTTOM) {
		switch (layer->transform) {
		case HISI_FB_TRANSFORM_NOP:
			layer->transform = HISI_FB_TRANSFORM_ROT_180;
			break;
		case HISI_FB_TRANSFORM_FLIP_H:
			layer->transform = HISI_FB_TRANSFORM_FLIP_V;
			break;
		case HISI_FB_TRANSFORM_FLIP_V:
			layer->transform = HISI_FB_TRANSFORM_FLIP_H;
			break;
		case HISI_FB_TRANSFORM_ROT_90:
			layer->transform = HISI_FB_TRANSFORM_ROT_270;
			break;
		case HISI_FB_TRANSFORM_ROT_180:
			layer->transform = HISI_FB_TRANSFORM_NOP;
			break;
		case HISI_FB_TRANSFORM_ROT_270:
			layer->transform = HISI_FB_TRANSFORM_ROT_90;
			break;

		case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H):
			layer->transform = (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V);
			break;
		case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V):
			layer->transform = (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H);
			break;

		default:
			HISI_FB_ERR("not support this transform(%d).\n", layer->transform);
			ret = -1;
			break;
		}

		if (ret == 0) {
			if ((pinfo->dirty_region_updt_support == 1) &&
				(pov_req->dirty_region_updt_info.dirty_rect.w > 0) &&
				(pov_req->dirty_region_updt_info.dirty_rect.h > 0)) {
				layer->dst_rect.x = (pov_req->dirty_region_updt_info.dirty_rect.w - (layer->dst_rect.x + layer->dst_rect.w));
				layer->dst_rect.y = (pov_req->dirty_region_updt_info.dirty_rect.h - (layer->dst_rect.y + layer->dst_rect.h));
			} else {
				layer->dst_rect.x = (pinfo->xres - (layer->dst_rect.x + layer->dst_rect.w));
				layer->dst_rect.y = (pinfo->yres - (layer->dst_rect.y + layer->dst_rect.h));
			}
		}
	} else {
		HISI_FB_ERR("fb%d, not support this lcd_refresh_direction_ctrl(%d)!\n",
			hisifd->index, pinfo->lcd_refresh_direction_ctrl);
		ret = -1;
	}

	return ret;
}

static int hisi_dss_lcd_refresh_direction_ctrl_ov(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req)
{
	struct hisi_panel_info *pinfo = NULL;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	pinfo = &(hisifd->panel_info);

	if ((pov_req->ovl_flags != DSS_SCENE_PDP_ONLINE) &&
		(pov_req->ovl_flags !=DSS_SCENE_SDP_ONLINE))
		return 0;

	if (pinfo->lcd_refresh_direction_ctrl == LCD_REFRESH_LEFT_TOP) {
		; //do nothing
	} else if (pinfo->lcd_refresh_direction_ctrl == LCD_REFRESH_RIGHT_TOP) {
		HISI_FB_INFO("fb%d, lcd_refresh_direction_ctrl(%d) not support!",
			hisifd->index, pinfo->lcd_refresh_direction_ctrl );
	} else if (pinfo->lcd_refresh_direction_ctrl == LCD_REFRESH_LEFT_BOTTOM) {
		HISI_FB_INFO("fb%d, lcd_refresh_direction_ctrl(%d) not support!",
			hisifd->index, pinfo->lcd_refresh_direction_ctrl );
	} else if (pinfo->lcd_refresh_direction_ctrl == LCD_REFRESH_RIGHT_BOTTOM) {
		if (pinfo->dirty_region_updt_support == 1) {
			pov_req->dirty_region_updt_info.dirty_rect.x =
				(pinfo->xres - (pov_req->dirty_region_updt_info.dirty_rect.x + pov_req->dirty_region_updt_info.dirty_rect.w));
			pov_req->dirty_region_updt_info.dirty_rect.y =
				(pinfo->yres - (pov_req->dirty_region_updt_info.dirty_rect.y + pov_req->dirty_region_updt_info.dirty_rect.h));
		}
	} else {
		HISI_FB_ERR("fb%d, not support this lcd_refresh_direction_ctrl(%d)!\n",
			hisifd->index, pinfo->lcd_refresh_direction_ctrl);
		ret = -1;
	}

	return 0;
}


int hisi_dss_handle_cur_ovl_req(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req)
{
	dss_layer_t *layer = NULL;
	int i = 0;
	int rot_idx = 0;
	int chn_idx = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	for (i = 0; i < pov_req->layer_nums; i++) {
		layer = &pov_req->layer_infos[i];
		chn_idx = layer->chn_idx;

		hisi_dss_lcd_refresh_direction_ctrl_layer(hisifd, pov_req, layer);

		if (hisifd->dss_exception.underflow_handler == 1) {
			hisifd->dss_exception.chn_used[chn_idx] = 0;
			hisifd->dss_exception.chn_format_used[chn_idx] = 0;
		}

		if (isChannelBelongtoDPE(DSS_ENG_DPE2, layer->chn_idx)) {
			hisifd->dss_module.dpe2_on_ref++;
		}

		if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
			rot_idx = hisi_get_rot_index(layer->chn_idx);
			if (rot_idx == 1) {
				hisifd->dss_module.rot1_on_ref++;
			}
		}
	}

	hisi_dss_lcd_refresh_direction_ctrl_ov(hisifd, pov_req);

	return 0;
}

int hisi_dss_handle_prev_ovl_req_wb(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req)
{
	dss_wb_layer_t *layer = NULL;
	int chn_idx = 0;
	int idx_tmp = 0;
	uint32_t format = 0;
	uint32_t need_cap = 0;
	uint32_t tmp = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	if (pov_req->wb_enable != 1)
		return 0;

	layer = &(pov_req->wb_layer_info);
	chn_idx = layer->chn_idx;
	format = layer->dst.format;
	need_cap = layer->need_cap;

	/* CSC */
	if (isYUV(format) && chn_idx != WBE1_CHN0) {
		hisifd->dss_module.csc_used[chn_idx] = 1;
	}

	if (need_cap & CAP_SCL) {
		/* SCF */
		idx_tmp = hisi_get_scf_index(chn_idx);
		if (idx_tmp < 0) {
			HISI_FB_ERR("hisi_get_scf_index failed! chn_idx=%d.\n", chn_idx);
		} else {
			hisifd->dss_module.scf_used[idx_tmp] = 1;
		}
	}

	/* DFC */
	hisifd->dss_module.dfc_used[chn_idx] = 1;

	/* FBC */
	if (need_cap & CAP_FBC) {
		hisifd->dss_module.fbc_used[chn_idx] = 1;
		hisifd->dss_module.fbc[chn_idx].fbc_pm_ctrl =
			set_bits32(hisifd->dss_module.fbc[chn_idx].fbc_pm_ctrl, 0x83AA, 17, 0);
	}

	/* CHN CTRL */
	if (hisifd->dss_wb_exception.underflow_exception == 1) {
		hisifd->dss_wb_exception.chn_used[chn_idx] = 1;
		hisifd->dss_wb_exception.chn_format_used[chn_idx] = format;
	} else {
		hisifd->dss_glb.chn_ctl_used[chn_idx] = 1;
	}

	/* WDMA */
	hisifd->dss_module.wdma_used[chn_idx] = 1;

	if (layer->dst.mmu_enable) {
		/* MMU WDMA */
		hisifd->dss_module.mmu_dma0_used[chn_idx] = 1;

		/* MMU RPTB */
		if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
			idx_tmp = hisi_get_rot_index(chn_idx);
			if (idx_tmp < 0) {
				HISI_FB_ERR("hisi_get_rot_index failed! chn_idx=%d.\n", chn_idx);
			} else {
				if (pov_req->wb_enable && (pov_req->wb_layer_info.chn_idx == WBE1_CHN1))
					tmp = hisifd->dss_wb_rptb_info_cur[1].rptb_size[idx_tmp];
				else
					tmp = hisifd->dss_wb_rptb_info_cur[0].rptb_size[idx_tmp];

				if (tmp > 0) {
					hisifd->dss_module.mmu_rptb_used = 1;
					hisifd->dss_module.mmu_rptb.mmu_rptb_idx_used[idx_tmp] = 1;
					hisifd->dss_module.mmu_rptb.mmu_rptb_ctl =
						set_bits32(hisifd->dss_module.mmu_rptb.mmu_rptb_ctl,
							(0 << idx_tmp) | (1 << (26 + idx_tmp)), 32, 0);

					hisifd->dss_glb.rot_tlb_scene_used[idx_tmp] = 1;
				}
			}
		}
	}

	if (hisifd->ovl_type != DSS_OVL_ADP)
		hisi_dss_rptb_handler(hisifd, true, 0);

	return 0;
}

int hisi_dss_handle_cur_ovl_req_wb(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req)
{
	dss_wb_layer_t *layer = NULL;
	int rot_idx = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	if (pov_req->wb_enable != 1)
		return 0;

	layer = &(pov_req->wb_layer_info);

	if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
		rot_idx = hisi_get_rot_index(layer->chn_idx);
		if (rot_idx == 1) {
			hisifd->dss_module.rot1_on_ref++;
		}
	}

	return 0;
}


/*******************************************************************************
** DSS RDMA Bridge
*/
static int g_rdma_bridge_mid[RDMA_MID_NUM] = {
	0, 1, 0, 1, 2,
	3, 4, 5, 6, 7,
	6, 7, 8, 9, 10,
	11, 12, 13
};

static int hisi_get_rdma_axi_port(int chn_idx, uint32_t flags)
{
	int axi_port = AXI_CH0;

	switch (flags) {
	case DSS_SCENE_PDP_ONLINE:
	case DSS_SCENE_SDP_ONLINE:
		axi_port = AXI_CH0;
		break;
	case DSS_SCENE_WBE0_WS_OFFLINE:
	case DSS_SCENE_WBE0_WO_OFFLINE:
	case DSS_SCENE_WBE1_WS_OFFLINE:
	case DSS_SCENE_WBE1_WO_OFFLINE:
		axi_port = AXI_CH1;
		break;
	default:
		axi_port = -1;
		HISI_FB_ERR("chn%d not support this flags(%d)!\n", chn_idx, flags);
		break;
	}

	return axi_port;
}

static int hisi_get_rdma_bridge_bpp(int format, int transform, int stretched_line_num)
{
	int bpp = 0;
	int rot_factor = 1;

	if (transform & HISI_FB_TRANSFORM_ROT_90)
		rot_factor = 2;

	switch (format) {
	case DMA_PIXEL_FORMAT_RGB32BIT:
		bpp = 8;
		break;
	case DMA_PIXEL_FORMAT_RGB16BIT:
	case DMA_PIXEL_FORMAT_YUV_422_I:
		bpp = 4;
		break;

	case DMA_PIXEL_FORMAT_YUV_422_SP_HP_Y:
	case DMA_PIXEL_FORMAT_YUV_422_SP_HP_UV:
		bpp = 2 * rot_factor;
		break;

	case DMA_PIXEL_FORMAT_YUV_420_SP_Y:
		bpp = 2 * rot_factor;
		break;
	case DMA_PIXEL_FORMAT_YUV_420_SP_UV:
		bpp = (stretched_line_num > 0) ? 2 * rot_factor : 2;
		break;

	case DMA_PIXEL_FORMAT_YUV_422_P_HP_Y:
		bpp = 2 * rot_factor;
		break;
	case DMA_PIXEL_FORMAT_YUV_422_P_HP_U:
	case DMA_PIXEL_FORMAT_YUV_422_P_HP_V:
		bpp = 1 * rot_factor;
		break;

	case DMA_PIXEL_FORMAT_YUV_420_P_Y:
		bpp = 2 * rot_factor;
		break;
	case DMA_PIXEL_FORMAT_YUV_420_P_U:
	case DMA_PIXEL_FORMAT_YUV_420_P_V:
		bpp = (stretched_line_num > 0) ? 1 * rot_factor : 1;
		break;

	case DMA_PIXEL_FORMAT_YUV_422_SP_VP_Y:
	case DMA_PIXEL_FORMAT_YUV_422_SP_VP_UV:
	case DMA_PIXEL_FORMAT_YUV_422_P_VP_Y:
	case DMA_PIXEL_FORMAT_YUV_422_P_VP_U:
	case DMA_PIXEL_FORMAT_YUV_422_P_VP_V:
	default:
		bpp = -1;
		HISI_FB_ERR("not support the format: %d\n", format);
		break;
	}

	return bpp;
}

static int hisi_get_rdma_bridge_m(int chn_idx, int transform)
{
	int m = 1;

	if (transform & HISI_FB_TRANSFORM_ROT_90) {
		switch (chn_idx) {
		case DPE0_CHN0:
		case DPE0_CHN1:
		case DPE0_CHN2:
		case DPE0_CHN3:
		case DPE1_CHN0:
		case DPE1_CHN1:
		case DPE2_CHN0:
		case DPE2_CHN1:
		case DPE2_CHN2:
		case DPE2_CHN3:
			m = 4;
			break;
		case DPE3_CHN0:
		case DPE3_CHN1:
		case DPE3_CHN2:
		case DPE3_CHN3:
			m = 2;
			break;
		default:
			m = -1;
			HISI_FB_ERR("not support this chn%d!\n", chn_idx);
			break;
		}
	}

	return m;
}

static int hisi_get_rdma_bridge_mid(dss_layer_t *layer, int dma_idx)
{
	int8_t chn_scene = 0;
	int8_t rotation = 0;
	int8_t uv_format = 0;
	int8_t scl = 0;
	int8_t index = 0;

	switch (layer->flags) {
	case DSS_SCENE_WBE0_WS_OFFLINE:
	case DSS_SCENE_WBE0_WO_OFFLINE:
		return g_rdma_bridge_mid[RDMA_MID_NUM - 2];

	case DSS_SCENE_WBE1_WS_OFFLINE:
	case DSS_SCENE_WBE1_WO_OFFLINE:
		return g_rdma_bridge_mid[RDMA_MID_NUM - 1];

	case DSS_SCENE_PDP_ONLINE:
		chn_scene = 0;
		break;
	case DSS_SCENE_SDP_ONLINE:
		chn_scene = 1;
		break;

	default:
		HISI_FB_ERR("layer_idx(%d), not support this scene(%d)!",
			layer->layer_idx, layer->flags);
		return -EINVAL;
	}

	rotation = (layer->transform & HISI_FB_TRANSFORM_ROT_90) ? 1 : 0;

	uv_format =((dma_idx == DSS_RDMA1) &&
		isYUVSemiPlanar(layer->src.format)) ? 1 : 0;

	scl = ((layer->src_rect.w != layer->dst_rect.w) ||
		(layer->src_rect.h != layer->dst_rect.h)) ? 1 : 0;

	index = (chn_scene << 3) | (rotation << 2) | (uv_format << 1) | scl;

	return g_rdma_bridge_mid[index];
}

static void hisi_dss_rdma_bridge_init(char __iomem *rdma_bridge_base,
	dss_rdma_bridge_t *s_rdma_bridge)
{
	BUG_ON(s_rdma_bridge == NULL);
	BUG_ON(rdma_bridge_base == NULL);

	memset(s_rdma_bridge, 0, sizeof(dss_rdma_bridge_t));

	s_rdma_bridge->rbrg_rdma_ctl[0] =
		inp32(rdma_bridge_base + RBRG_RDMA0_CTL);
	s_rdma_bridge->rbrg_rdma_ctl[1] =
		inp32(rdma_bridge_base + RBRG_RDMA1_CTL);
	s_rdma_bridge->rbrg_rdma_ctl[2] =
		inp32(rdma_bridge_base + RBRG_RDMA2_CTL);
	s_rdma_bridge->rbrg_rdma_ctl[3] =
		inp32(rdma_bridge_base + RBRG_RDMA3_CTL);
	s_rdma_bridge->rbrg_rdma_ctl[4] =
		inp32(rdma_bridge_base + RBRG_RDMA4_CTL);
	s_rdma_bridge->rbrg_outstanding_dep[0] =
		inp32(rdma_bridge_base + RBRG_OUTSTANDING_DEP0);
	s_rdma_bridge->rbrg_outstanding_dep[1] =
		inp32(rdma_bridge_base + RBRG_OUTSTANDING_DEP1);
	s_rdma_bridge->rbrg_outstanding_dep[2] =
		inp32(rdma_bridge_base + RBRG_OUTSTANDING_DEP2);
	s_rdma_bridge->rbrg_outstanding_dep[3] =
		inp32(rdma_bridge_base + RBRG_OUTSTANDING_DEP3);
	s_rdma_bridge->rbrg_outstanding_dep[4] =
		inp32(rdma_bridge_base + RBRG_OUTSTANDING_DEP4);
}

static void hisi_dss_rdma_bridge_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *rdma_bridge_base, dss_rdma_bridge_t *s_rdma_bridge)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(s_rdma_bridge == NULL);
	BUG_ON(rdma_bridge_base == NULL);

	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_RDMA0_CTL,
		s_rdma_bridge->rbrg_rdma_ctl[0], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_RDMA1_CTL,
		s_rdma_bridge->rbrg_rdma_ctl[1], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_RDMA2_CTL,
		s_rdma_bridge->rbrg_rdma_ctl[2], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_RDMA3_CTL,
		s_rdma_bridge->rbrg_rdma_ctl[3], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_RDMA4_CTL,
		s_rdma_bridge->rbrg_rdma_ctl[4], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_OUTSTANDING_DEP0,
		s_rdma_bridge->rbrg_outstanding_dep[0], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_OUTSTANDING_DEP1,
		s_rdma_bridge->rbrg_outstanding_dep[1], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_OUTSTANDING_DEP2,
		s_rdma_bridge->rbrg_outstanding_dep[2], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_OUTSTANDING_DEP3,
		s_rdma_bridge->rbrg_outstanding_dep[3], 32, 0);
	hisifd->set_reg(hisifd, rdma_bridge_base + RBRG_OUTSTANDING_DEP4,
		s_rdma_bridge->rbrg_outstanding_dep[4], 32, 0);
}

static int hisi_set_dpe_rdma_bridge(struct hisi_fb_data_type *hisifd, int dpe_type,
	dss_overlay_t *pov_req)
{
	dss_rdma_bridge_t *rdma_bridge = NULL;
	dss_layer_t *layer = NULL;

	int ret = 0;
	int i = 0;
	int k = 0;
	int dma_idx = 0;
	int dma_idx_tmp = 0;
	int dma_fomat = 0;

	int dma_axi_port[RDMA_NUM] = {0};
	int dma_bpp[RDMA_NUM] = {0};
	int dma_vscl[RDMA_NUM] = {0};
	int dma_hscl[RDMA_NUM] = {0};
	int dma_m[RDMA_NUM] = {0};
	int dma_osd[RDMA_NUM] = {0};
	int dma_mid[RDMA_NUM] = {0};
	int dma_rot[RDMA_NUM] = {0};
	int dma_osd_big[RDMA_NUM] = {0};
	int dma_rot_burst4[RDMA_NUM] = {0};

	int sum_axi0_bandwidth = 0;
	int sum_axi1_bandwidth = 0;
	int sum_axi0_osd = 0;
	int sum_axi1_osd = 0;
	int sum_axi0_osd_big = 0;
	int sum_axi1_osd_big = 0;

	int used_axi0_dma_num = 0;
	int used_axi1_dma_num = 0;
	int used_axi0_dma_rot_num = 0;
	int used_axi1_dma_rot_num = 0;

	int sum_axi0_remain_bandwidth = 0;
	int sum_axi1_remain_bandwidth = 0;
	uint32_t axi0_remain_bigid_num = 0;
	uint32_t axi1_remain_bigid_num = 0;

	uint32_t stretched_line_num = 0;
	dss_rect_t rdma_out_rect;
	bool need_dump = false;
	int tmp = 0;

	int req_min_cont = 0;
	int req_max_cont = 0;
	uint32_t rbrg_outstanding_dep = 0;
	uint32_t rbrg_rdma_ctl = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON((dpe_type < 0 || dpe_type >= DSS_ENG_MAX));

	if (!hasChannelBelongtoDPE(dpe_type, hisifd, pov_req))
		return 0;

	rdma_bridge = &(hisifd->dss_module.rdma_bridge[dpe_type]);
	hisifd->dss_module.rdma_bridge_used[dpe_type] = 1;

	for (k = 0; k < RDMA_NUM; k++) {
		dma_axi_port[k] = -1;
		dma_bpp[k] = 0;
		dma_vscl[k] = RDMA_OSD_MULTIPLE;
		dma_hscl[k] = RDMA_OSD_MULTIPLE;
		dma_m[k] = 1;
		dma_mid[k] = 0;
		dma_rot[k] = 0;
		dma_osd[k] = 0;
		dma_osd_big[k] = 0;
		dma_rot_burst4[k] = 0;
	}

	for (i = 0; i < pov_req->layer_nums; i++) {
		layer = &(pov_req->layer_infos[i]);

		if (layer->need_cap & (CAP_DIM |CAP_BASE))
			continue;

		if (!isChannelBelongtoDPE(dpe_type, layer->chn_idx))
			continue;

		ret = hisi_dss_check_layer_par(hisifd, layer);
		if (ret != 0) {
			HISI_FB_ERR("hisi_dss_check_layer_par failed! ret=%d\n", ret);
			return -EINVAL;
		}

		stretched_line_num = isNeedRdmaStretchBlt(hisifd, layer);
		rdma_out_rect = hisi_dss_rdma_out_rect(hisifd, layer);

		dma_idx = hisi_chnIndex2dmaIndex(layer->chn_idx);
		if ((dma_idx < 0) || (dma_idx >= RDMA_NUM)) {
			HISI_FB_ERR("chn_idx(%d) hisi_chnIndex2dmaIndex failed! dma_idx=%d.\n",
				layer->chn_idx, dma_idx);
			return -EINVAL;
		}

		dma_fomat = hisi_pixel_format_hal2dma(layer->src.format);
		if (dma_fomat < 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_pixel_format_hal2dma failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		dma_rot_burst4[dma_idx] = is_need_rdma_rot_burst4(layer) ? 1 : 0;
		dma_rot[dma_idx] = (layer->transform & HISI_FB_TRANSFORM_ROT_90) ? 1 : 0;

		dma_axi_port[dma_idx] = hisi_get_rdma_axi_port(layer->chn_idx, layer->flags);
		if (dma_axi_port[dma_idx] < 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_get_rdma_axi_port failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		dma_bpp[dma_idx] = hisi_get_rdma_bridge_bpp(dma_fomat, layer->transform, stretched_line_num);
		if (dma_bpp[dma_idx] <= 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_get_rdma_bridge_bpp failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		if (rdma_out_rect.h > layer->dst_rect.h)
			dma_vscl[dma_idx] = RDMA_OSD_MULTIPLE * rdma_out_rect.h / layer->dst_rect.h;

		if (rdma_out_rect.w > hisifd->panel_info.xres)
			dma_hscl[dma_idx] = RDMA_OSD_MULTIPLE * rdma_out_rect.w / hisifd->panel_info.xres;

		if ((dma_vscl[dma_idx] <= 0) || (dma_hscl[dma_idx] <= 0)) {
			HISI_FB_ERR("layer_idx%d, dma_idx%d, rot=%d, dma_vscl=%d, dma_hscl=%d, "
				"src_rect(%d,%d,%d,%d), dst_rect(%d,%d,%d,%d)!\n",
				layer->layer_idx, dma_idx, dma_rot[dma_idx], dma_vscl[dma_idx], dma_hscl[dma_idx],
				layer->src_rect.x, layer->src_rect.y, layer->src_rect.w, layer->src_rect.h,
				layer->dst_rect.x, layer->dst_rect.y, layer->dst_rect.w, layer->dst_rect.h);

			dma_vscl[dma_idx] = RDMA_OSD_MULTIPLE;
			dma_hscl[dma_idx] = RDMA_OSD_MULTIPLE;
		}

		dma_m[dma_idx] = hisi_get_rdma_bridge_m(layer->chn_idx, layer->transform);
		if (dma_m[dma_idx] <= 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_get_rdma_bridge_m failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		dma_mid[dma_idx] = hisi_get_rdma_bridge_mid(layer, dma_idx);
		if (dma_mid[dma_idx] < 0) {
			HISI_FB_ERR("chn_idx(%d) dma_idx(%d) hisi_get_rdma_bridge_mid failed!\n",
				layer->chn_idx, dma_idx);
			return -EINVAL;
		}

		if (dma_axi_port[dma_idx] == AXI_CH0) {
			used_axi0_dma_num++;
			if (dma_rot[dma_idx] == 1) {
				used_axi0_dma_rot_num++;
			}

			sum_axi0_bandwidth += (dma_bpp[dma_idx] * dma_vscl[dma_idx] * dma_hscl[dma_idx]);
		} else if (dma_axi_port[dma_idx] == AXI_CH1)  {
			used_axi1_dma_num++;
			if (dma_rot[dma_idx] == 1) {
				used_axi1_dma_rot_num++;
			}

			sum_axi1_bandwidth += (dma_bpp[dma_idx] * dma_vscl[dma_idx] * dma_hscl[dma_idx]);
		} else {
			WARN_ON(1);
		}

		if ((dma_idx == 0) && (isYUVSemiPlanar(layer->src.format) ||
			isYUVPlanar(layer->src.format))) {
			dma_idx_tmp = dma_idx + 1;
			dma_rot[dma_idx_tmp] = dma_rot[dma_idx];
			dma_rot_burst4[dma_idx_tmp] = dma_rot_burst4[dma_idx];
			dma_axi_port[dma_idx_tmp] = dma_axi_port[dma_idx];

			dma_bpp[dma_idx_tmp] = hisi_get_rdma_bridge_bpp(dma_fomat + 1, layer->transform, stretched_line_num);
			if (dma_bpp[dma_idx_tmp] <= 0) {
				HISI_FB_ERR("chn_idx(%d) dma_idx(%d) hisi_get_rdma_bridge_bpp failed!\n",
					layer->chn_idx, dma_idx_tmp);
				return -EINVAL;
			}

			dma_vscl[dma_idx_tmp] = dma_vscl[dma_idx];
			dma_hscl[dma_idx_tmp] = dma_hscl[dma_idx];

			dma_m[dma_idx_tmp] = dma_m[dma_idx];
			dma_mid[dma_idx_tmp] = hisi_get_rdma_bridge_mid(layer, dma_idx_tmp);
			if (dma_mid[dma_idx_tmp] < 0) {
				HISI_FB_ERR("chn_idx(%d) dma_idx(%d) hisi_get_rdma_bridge_mid failed!\n",
					layer->chn_idx, dma_idx_tmp);
				return -EINVAL;
			}

			if (dma_axi_port[dma_idx_tmp] == AXI_CH0) {
				used_axi0_dma_num++;
				if (dma_rot[dma_idx_tmp] == 1) {
					used_axi0_dma_rot_num++;
				}

				sum_axi0_bandwidth += (dma_bpp[dma_idx_tmp] * dma_vscl[dma_idx_tmp] * dma_hscl[dma_idx_tmp]);
			} else if (dma_axi_port[dma_idx_tmp] == AXI_CH1)  {
				used_axi1_dma_num++;
				if (dma_rot[dma_idx_tmp] == 1) {
					used_axi1_dma_rot_num++;
				}

				sum_axi1_bandwidth += (dma_bpp[dma_idx_tmp] * dma_vscl[dma_idx_tmp] * dma_hscl[dma_idx_tmp]);
			} else {
				WARN_ON(1);
			}
		}

		if ((dma_idx == 0) && isYUVPlanar(layer->src.format)) {
			dma_idx_tmp = dma_idx + 2;
			dma_rot[dma_idx_tmp] = dma_rot[dma_idx];
			dma_rot_burst4[dma_idx_tmp] = dma_rot_burst4[dma_idx];
			dma_axi_port[dma_idx_tmp] = dma_axi_port[dma_idx];

			dma_bpp[dma_idx_tmp] = hisi_get_rdma_bridge_bpp(dma_fomat + 2, layer->transform, stretched_line_num);
			if (dma_bpp[dma_idx_tmp] <= 0) {
				HISI_FB_ERR("chn_idx(%d) dma_idx(%d) hisi_get_rdma_bridge_bpp failed!\n",
					layer->chn_idx, dma_idx_tmp);
				return -EINVAL;
			}

			dma_vscl[dma_idx_tmp] = dma_vscl[dma_idx];
			dma_hscl[dma_idx_tmp] = dma_hscl[dma_idx];

			dma_m[dma_idx_tmp] = dma_m[dma_idx];
			dma_mid[dma_idx_tmp] = hisi_get_rdma_bridge_mid(layer, dma_idx_tmp);
			if (dma_mid[dma_idx_tmp] < 0) {
				HISI_FB_ERR("chn_idx(%d) dma_idx(%d) hisi_get_rdma_bridge_mid failed!\n",
					layer->chn_idx, dma_idx_tmp);
				return -EINVAL;
			}

			if (dma_axi_port[dma_idx_tmp] == AXI_CH0) {
				used_axi0_dma_num++;
				if (dma_rot[dma_idx_tmp] == 1) {
					used_axi0_dma_rot_num++;
				}

				sum_axi0_bandwidth += (dma_bpp[dma_idx_tmp] * dma_vscl[dma_idx_tmp] * dma_hscl[dma_idx_tmp]);
			} else if (dma_axi_port[dma_idx_tmp] == AXI_CH1)  {
				used_axi1_dma_num++;
				if (dma_rot[dma_idx_tmp] == 1) {
					used_axi1_dma_rot_num++;
				}

				sum_axi1_bandwidth += (dma_bpp[dma_idx_tmp] * dma_vscl[dma_idx_tmp] * dma_hscl[dma_idx_tmp]);
			} else {
				WARN_ON(1);
			}
		}
	}

	axi0_remain_bigid_num = AXI_OSD_BIG_SUM_MAX - 1 * used_axi0_dma_rot_num;
	axi1_remain_bigid_num = AXI_OSD_BIG_SUM_MAX - 1 * used_axi1_dma_rot_num;

	sum_axi0_remain_bandwidth = sum_axi0_bandwidth;
	sum_axi1_remain_bandwidth = sum_axi1_bandwidth;

	for (k = 0; k < RDMA_NUM; k++) {
		if (dma_axi_port[k] == AXI_CH0) {
			if (dma_rot[k] == 1) {
				dma_osd[k] = (RDMA_OSD_FORMULA - 1 * used_axi0_dma_rot_num) *
					dma_m[k] * (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]) / sum_axi0_bandwidth;
				dma_osd[k] = MIN(dma_osd[k], DMA_OSD_MAX_64);
				if (dma_osd[k] <= 0) {
					dma_osd[k] = 1;
				}

				sum_axi0_remain_bandwidth -= (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]);
				//axi0_remain_bigid_num -= dma_osd[k] /dma_m[k];
				axi0_remain_bigid_num -= (dma_osd[k] /dma_m[k] + (((dma_osd[k] % dma_m[k]) > 0) ? 1 : 0));
			}
		} else if (dma_axi_port[k] == AXI_CH1) {
			if (dma_rot[k] == 1) {
				dma_osd[k] = (RDMA_OSD_FORMULA - 1 * used_axi1_dma_rot_num) *
					dma_m[k] * (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]) / sum_axi1_bandwidth;
				dma_osd[k] = MIN(dma_osd[k], DMA_OSD_MAX_64);
				if (dma_osd[k] <= 0) {
					dma_osd[k] = 1;
				}

				sum_axi1_remain_bandwidth -= (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]);
				//axi1_remain_bigid_num -= dma_osd[k] /dma_m[k];
				axi1_remain_bigid_num -= (dma_osd[k] /dma_m[k] + (((dma_osd[k] % dma_m[k]) > 0) ? 1 : 0));
			}
		} else {
			; /* this channel is not used */
		}
	}

	for (k = 0; k < RDMA_NUM; k++) {
		if (dma_osd[k] != 0)
			continue;

		if (dma_axi_port[k] == AXI_CH0) {
			if ((axi0_remain_bigid_num <= 0) || (sum_axi0_remain_bandwidth <= 0)) {
				need_dump = true;
				goto err_adjust;
			}

			tmp = axi0_remain_bigid_num * (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]) / sum_axi0_remain_bandwidth;
			if (tmp <= 0) {
				dma_osd[k] = 1;

				sum_axi0_remain_bandwidth -= (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]);
				axi0_remain_bigid_num -= (dma_osd[k] /dma_m[k] + (((dma_osd[k] % dma_m[k]) > 0) ? 1 : 0));
			}
		} else if (dma_axi_port[k] == AXI_CH1) {
			if ((axi1_remain_bigid_num <= 0) || (sum_axi1_remain_bandwidth <= 0)) {
				need_dump = true;
				goto err_adjust;
			}

			tmp = axi1_remain_bigid_num * (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]) / sum_axi1_remain_bandwidth;
			if (tmp <= 0) {
				dma_osd[k] = 1;

				sum_axi1_remain_bandwidth -= (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]);
				axi1_remain_bigid_num -= (dma_osd[k] /dma_m[k] + (((dma_osd[k] % dma_m[k]) > 0) ? 1 : 0));
			}
		} else {
			; /* this channel is not used */
		}
	}

	for (k = 0; k < RDMA_NUM; k++) {
		if (dma_axi_port[k] == AXI_CH0) {
			if (dma_rot[k] == 0) {
				if (dma_osd[k] == 0)
					dma_osd[k] = axi0_remain_bigid_num * (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]) / sum_axi0_remain_bandwidth;

				if ((dma_osd[k] <= 0) || (dma_osd[k] > DMA_OSD_MAX_32))
					need_dump = true;
			} else {
				if ((dma_osd[k] <= 0) || (dma_osd[k] > DMA_OSD_MAX_64))
					need_dump = true;
			}

			dma_osd_big[k] = dma_osd[k] / dma_m[k];

			sum_axi0_osd += dma_osd[k];
			sum_axi0_osd_big += dma_osd_big[k];
		} else if (dma_axi_port[k] == AXI_CH1) {
			if (dma_rot[k] == 0) {
				if (dma_osd[k] == 0)
					dma_osd[k] = axi1_remain_bigid_num * (dma_bpp[k] * dma_vscl[k] * dma_hscl[k]) / sum_axi1_remain_bandwidth;

				if ((dma_osd[k] <= 0) || (dma_osd[k] > DMA_OSD_MAX_32))
					need_dump = true;
			} else {
				if ((dma_osd[k] <= 0) || (dma_osd[k] > DMA_OSD_MAX_64))
					need_dump = true;
			}

			dma_osd_big[k] = dma_osd[k] / dma_m[k];

			sum_axi1_osd += dma_osd[k];
			sum_axi1_osd_big += dma_osd_big[k];
		} else {
			; /* this channel is not used */
		}
	}

	if ((sum_axi0_osd_big > AXI_OSD_BIG_SUM_MAX) ||
		(sum_axi1_osd_big > AXI_OSD_BIG_SUM_MAX) ||
		(sum_axi0_osd > AXI_OSD_SUM_MAX) ||
		(sum_axi1_osd > AXI_OSD_SUM_MAX)) {
		need_dump = true;
	}

	if (g_debug_ovl_osd) {
		HISI_FB_WARNING("used_axi0_dma_num=%d, used_axi0_dma_rot_num=%d, "
			"sum_axi0_bandwidth=%d, axi0_remain_bigid_num=%d, sum_axi0_osd=%d, sum_axi0_osd_big=%d, !\n",
			used_axi0_dma_num, used_axi0_dma_rot_num, sum_axi0_bandwidth,
			axi0_remain_bigid_num, sum_axi0_osd, sum_axi0_osd_big);

		HISI_FB_WARNING("used_axi1_dma_num=%d, used_axi1_dma_rot_num=%d, "
			"sum_axi1_bandwidth=%d, axi1_remain_bigid_num=%d, sum_axi1_osd=%d, sum_axi1_osd_big=%d, !\n",
			used_axi1_dma_num, used_axi1_dma_rot_num, sum_axi1_bandwidth,
			axi1_remain_bigid_num, sum_axi1_osd, sum_axi1_osd_big);

		for (k = 0; k < RDMA_NUM; k++) {
			HISI_FB_WARNING("axi%d, dma%d, dma_osd=%d, dma_osd_big=%d, dma_rot=%d, dma_rot_burst4=%d, dma_mid=%d, "
				"(dma_m=%d, dma_bpp=%d, dma_vscl=%d, dma_hscl=%d)!\n",
				dma_axi_port[k], k, dma_osd[k],  dma_osd_big[k], dma_rot[k], dma_rot_burst4[k], dma_mid[k],
				dma_m[k], dma_bpp[k], dma_vscl[k], dma_hscl[k]);
		}
	}

err_adjust:
	if (need_dump) {
		HISI_FB_WARNING("used_axi0_dma_num=%d, used_axi0_dma_rot_num=%d, "
			"sum_axi0_bandwidth=%d, axi0_remain_bigid_num=%d, sum_axi0_osd=%d, sum_axi0_osd_big=%d, !\n",
			used_axi0_dma_num, used_axi0_dma_rot_num, sum_axi0_bandwidth,
			axi0_remain_bigid_num, sum_axi0_osd, sum_axi0_osd_big);

		HISI_FB_WARNING("used_axi1_dma_num=%d, used_axi1_dma_rot_num=%d, "
			"sum_axi1_bandwidth=%d, axi1_remain_bigid_num=%d, sum_axi1_osd=%d, sum_axi1_osd_big=%d, !\n",
			used_axi1_dma_num, used_axi1_dma_rot_num, sum_axi1_bandwidth,
			axi1_remain_bigid_num, sum_axi1_osd, sum_axi1_osd_big);

		for (k = 0; k < RDMA_NUM; k++) {
			HISI_FB_WARNING("axi%d, dma%d, dma_osd=%d, dma_osd_big=%d, dma_rot=%d, dma_rot_burst4=%d, dma_mid=%d, "
				"(dma_m=%d, dma_bpp=%d, dma_vscl=%d, dma_hscl=%d)!\n",
				dma_axi_port[k], k, dma_osd[k], dma_osd_big[k], dma_rot[k], dma_rot_burst4[k], dma_mid[k],
				dma_m[k], dma_bpp[k], dma_vscl[k], dma_hscl[k]);

			if (dma_axi_port[k] == AXI_CH0) {
				if (dma_rot[k] == 1) {
					dma_osd[k]= (AXI_OSD_BIG_SUM_MAX / used_axi0_dma_num) * 2;
				} else {
					dma_osd[k]= (AXI_OSD_BIG_SUM_MAX / used_axi0_dma_num);
				}
			} else if (dma_axi_port[k] == AXI_CH1) {
				if (dma_rot[k] == 1) {
					dma_osd[k]= (AXI_OSD_BIG_SUM_MAX / used_axi1_dma_num) * 2;
				} else {
					dma_osd[k]= (AXI_OSD_BIG_SUM_MAX / used_axi1_dma_num);
				}
			} else {
				;
			}
		}
	}

	for (k = 0; k < RDMA_NUM; k++) {
		if (dma_axi_port[k] < 0) {
			rbrg_outstanding_dep = 0x0;
			rbrg_rdma_ctl = 0x0;
		} else {
			req_max_cont = (dma_osd[k] > RDMA_REQ_CONT_MAX) ? RDMA_REQ_CONT_MAX : dma_osd[k];
			req_max_cont -= 1;
			req_min_cont = req_max_cont / 2;

			rbrg_outstanding_dep = (dma_osd[k] | (dma_osd[k] << 8));
			rbrg_rdma_ctl = (req_min_cont | (req_max_cont << 4) | (dma_mid[k] << 8) | (dma_axi_port[k] << 12));
		}

		rdma_bridge->rbrg_outstanding_dep[k] = set_bits32(rdma_bridge->rbrg_outstanding_dep[k],
			rbrg_outstanding_dep, 16, 0);
		rdma_bridge->rbrg_rdma_ctl[k] = set_bits32(rdma_bridge->rbrg_rdma_ctl[k],
			rbrg_rdma_ctl, 13, 0);

		if (g_debug_ovl_osd) {
			HISI_FB_INFO("dpe_type(%d), rdma%d, dma_axi_port(%d), dma_osd(%d), dma_mid(%d),"
				"rbrg_outstanding_dep(0x%x), rbrg_rdma_ctl(0x%x).\n",
				dpe_type, k, dma_axi_port[k], dma_osd[k], dma_mid[k],
				rbrg_outstanding_dep, rbrg_rdma_ctl);
		}
	}

	return 0;
}

int hisi_dss_rdma_bridge_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req)
{
	BUG_ON(hisifd == NULL);

	if (hisifd->ovl_type == DSS_OVL_PDP) {
		if (hisi_set_dpe_rdma_bridge(hisifd, DSS_ENG_DPE0, pov_req)) {
			HISI_FB_ERR("hisi_set_dpe_rdma_bridge for DSS_DPE0 failed!\n");
			return -EINVAL;
		}
	} else if (hisifd->ovl_type == DSS_OVL_SDP) {
		if (hisi_set_dpe_rdma_bridge(hisifd, DSS_ENG_DPE1, pov_req)) {
			HISI_FB_ERR("hisi_set_dpe_rdma_bridge for DSS_DPE0 failed!\n");
			return -EINVAL;
		}
	} else if (hisifd->ovl_type == DSS_OVL_ADP) {
		if (hisi_set_dpe_rdma_bridge(hisifd, DSS_ENG_DPE3, pov_req)) {
			HISI_FB_ERR("hisi_set_dpe_rdma_bridge for DSS_ENG_DPE3 failed!\n");
			return -EINVAL;
		}
	} else {
		HISI_FB_ERR("not support this ovl_type %d \n", hisifd->ovl_type);
		return -EINVAL;
	}

	if (hisi_set_dpe_rdma_bridge(hisifd, DSS_ENG_DPE2, pov_req)) {
		HISI_FB_ERR("hisi_set_dpe_rdma_bridge for DSS_ENG_DPE2 failed!\n");
		return -EINVAL;
	}

	return 0;
}


/*******************************************************************************
** DSS MMU
*/
void* hisi_dss_rptb_init(void)
{
	struct gen_pool *pool = NULL;
	int order = 3;
	size_t size = MMU_RPTB_SIZE_MAX;
	uint32_t addr = MMU_RPTB_SIZE_ALIGN;

	pool = gen_pool_create(order, 0);
	if (pool == NULL) {
		return NULL;
	}

	if (gen_pool_add(pool, addr, size, 0) != 0) {
		gen_pool_destroy(pool);
		return NULL;
	}

	return pool;
}

void hisi_dss_rptb_deinit(void *handle)
{
	if (handle) {
		gen_pool_destroy(handle);
		handle = NULL;
	}
}

uint32_t hisi_dss_rptb_alloc(void *handle, uint32_t size)
{
	return gen_pool_alloc(handle, size);
}

void hisi_dss_rptb_free(void *handle, uint32_t addr, uint32_t size)
{
	gen_pool_free(handle, addr, size);
}

static uint32_t hisi_get_mmu_tlb_tag_val(uint32_t format, uint32_t transform,
	uint8_t is_tile, bool rdma_stretch_enable)
{
	uint32_t mmu_ch_ctrl_val = 0;
	int mmu_format = 0;
	uint32_t mmu_tlb_tag_org = 0;
	int tlb_tag_size = 0;
	int tlb_tag_num = 0;
	int tlb_tag_ctl = 0;

	mmu_format= hisi_pixel_format_hal2mmu(format);
	if (mmu_format < 0) {
		HISI_FB_ERR("hisi_pixel_format_hal2mmu failed! mmu_format=%d.\n", mmu_format);
		return mmu_ch_ctrl_val;
	}

	if (((transform & 0x5) == 0x5) || ((transform & 0x6) == 0x6)) {
		transform = HISI_FB_TRANSFORM_ROT_90;
	}

	mmu_tlb_tag_org = (mmu_format & 0x3) | ((transform & 0x7) << 2) |
		((is_tile ? 1 : 0) << 5) | ((rdma_stretch_enable ? 1 : 0) << 6);

#if 0
	HISI_FB_ERR("mmu_format=%d, layer->transform=%d, layer->src.is_tile=%d,"
		" rdma_stretch_enable=%d, mmu_tlb_tag_org=0x%x\n",
		mmu_format, layer->transform, layer->src.is_tile,
		rdma_stretch_enable, mmu_tlb_tag_org);
#endif

	switch (mmu_tlb_tag_org) {
	case MMU_TLB_TAG_ORG_0x0:
	case MMU_TLB_TAG_ORG_0x4:
	case MMU_TLB_TAG_ORG_0x2:
	case MMU_TLB_TAG_ORG_0x6:
		tlb_tag_num = MMU_TAG_4;
		tlb_tag_size = MMU_TAG_128B;
		tlb_tag_ctl = MMU_TAG_RIGHT_ENDPOINT;
		break;
	case MMU_TLB_TAG_ORG_0x1:
	case MMU_TLB_TAG_ORG_0x5:
		tlb_tag_num = MMU_TAG_4;
		tlb_tag_size = MMU_TAG_64B;
		tlb_tag_ctl = MMU_TAG_RIGHT_ENDPOINT;
		break;
	case MMU_TLB_TAG_ORG_0x8:
	case MMU_TLB_TAG_ORG_0xC:
	case MMU_TLB_TAG_ORG_0xA:
	case MMU_TLB_TAG_ORG_0xE:
		tlb_tag_num = MMU_TAG_4;
		tlb_tag_size = MMU_TAG_128B;
		tlb_tag_ctl = MMU_TAG_LEFT_ENDPOINT;
		break;
	case MMU_TLB_TAG_ORG_0x9:
	case MMU_TLB_TAG_ORG_0xD:
		tlb_tag_num = MMU_TAG_4;
		tlb_tag_size = MMU_TAG_64B;
		tlb_tag_ctl = MMU_TAG_LEFT_ENDPOINT;
		break;
	case MMU_TLB_TAG_ORG_0x10:
	case MMU_TLB_TAG_ORG_0x1C:
	case MMU_TLB_TAG_ORG_0x12:
	case MMU_TLB_TAG_ORG_0x1E:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_64B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x11:
	case MMU_TLB_TAG_ORG_0x1D:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_32B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;

	case MMU_TLB_TAG_ORG_0x20:
	case MMU_TLB_TAG_ORG_0x24:
	case MMU_TLB_TAG_ORG_0x22:
	case MMU_TLB_TAG_ORG_0x26:
		tlb_tag_num = MMU_TAG_2;
		tlb_tag_size = MMU_TAG_256B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x21:
	case MMU_TLB_TAG_ORG_0x25:
		tlb_tag_num = MMU_TAG_2;
		tlb_tag_size = MMU_TAG_128B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x28:
	case MMU_TLB_TAG_ORG_0x2C:
	case MMU_TLB_TAG_ORG_0x2A:
	case MMU_TLB_TAG_ORG_0x2E:
		tlb_tag_num = MMU_TAG_2;
		tlb_tag_size = MMU_TAG_256B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x29:
	case MMU_TLB_TAG_ORG_0x2D:
		tlb_tag_num = MMU_TAG_2;
		tlb_tag_size = MMU_TAG_128B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x30:
	case MMU_TLB_TAG_ORG_0x3C:
	case MMU_TLB_TAG_ORG_0x32:
	case MMU_TLB_TAG_ORG_0x3E:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_16B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x31:
	case MMU_TLB_TAG_ORG_0x3D:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_16B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;

	case MMU_TLB_TAG_ORG_0x40:
	case MMU_TLB_TAG_ORG_0x44:
	case MMU_TLB_TAG_ORG_0x42:
	case MMU_TLB_TAG_ORG_0x46:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_32B;
		tlb_tag_ctl = MMU_TAG_RIGHT_ENDPOINT;
		break;
	case MMU_TLB_TAG_ORG_0x41:
	case MMU_TLB_TAG_ORG_0x45:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_32B;
		tlb_tag_ctl = MMU_TAG_RIGHT_ENDPOINT;
		break;
	case MMU_TLB_TAG_ORG_0x48:
	case MMU_TLB_TAG_ORG_0x4C:
	case MMU_TLB_TAG_ORG_0x4A:
	case MMU_TLB_TAG_ORG_0x4E:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_32B;
		tlb_tag_ctl = MMU_TAG_LEFT_ENDPOINT;
		break;
	case MMU_TLB_TAG_ORG_0x49:
	case MMU_TLB_TAG_ORG_0x4D:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_32B;
		tlb_tag_ctl = MMU_TAG_LEFT_ENDPOINT;
		break;
	case MMU_TLB_TAG_ORG_0x50:
	case MMU_TLB_TAG_ORG_0x5C:
	case MMU_TLB_TAG_ORG_0x52:
	case MMU_TLB_TAG_ORG_0x5E:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_32B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x51:
	case MMU_TLB_TAG_ORG_0x5D:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_32B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;

	case MMU_TLB_TAG_ORG_0x60:
	case MMU_TLB_TAG_ORG_0x64:
	case MMU_TLB_TAG_ORG_0x62:
	case MMU_TLB_TAG_ORG_0x66:
		tlb_tag_num = MMU_TAG_2;
		tlb_tag_size = MMU_TAG_256B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x61:
	case MMU_TLB_TAG_ORG_0x65:
		tlb_tag_num = MMU_TAG_2;
		tlb_tag_size = MMU_TAG_128B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x68:
	case MMU_TLB_TAG_ORG_0x6C:
	case MMU_TLB_TAG_ORG_0x6A:
	case MMU_TLB_TAG_ORG_0x6E:
		tlb_tag_num = MMU_TAG_2;
		tlb_tag_size = MMU_TAG_256B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x69:
	case MMU_TLB_TAG_ORG_0x6D:
		tlb_tag_num = MMU_TAG_2;
		tlb_tag_size = MMU_TAG_128B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x70:
	case MMU_TLB_TAG_ORG_0x7C:
	case MMU_TLB_TAG_ORG_0x72:
	case MMU_TLB_TAG_ORG_0x7E:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_16B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;
	case MMU_TLB_TAG_ORG_0x71:
	case MMU_TLB_TAG_ORG_0x7D:
		tlb_tag_num = MMU_TAG_8;
		tlb_tag_size = MMU_TAG_16B;
		tlb_tag_ctl = MMU_TAG_NEW;
		break;

	default:
		HISI_FB_ERR("not support this mmu_tlb_tag_org(0x%x)!\n", mmu_tlb_tag_org);
		break;
	}

	mmu_ch_ctrl_val = (tlb_tag_num << 4) | (tlb_tag_size << 6) | (tlb_tag_ctl << 14);

	return mmu_ch_ctrl_val;
}

static int hisi_rptb_pt_dma_sel(int chn_idx, int format)
{
	int ret = 0;
	bool is_yuv_semi_planar = false;
	bool is_yuv_planar = false;

	BUG_ON((chn_idx < 0) || (chn_idx >= DSS_CHN_MAX));

	is_yuv_semi_planar = isYUVSemiPlanar(format);
	is_yuv_planar = isYUVPlanar(format);

	switch (chn_idx) {
	case DPE0_CHN0:
		if (is_yuv_semi_planar) {
			ret = 0x1;
		} else {
			ret = 0x0;
		}
		break;
	case WBE0_CHN1:
		ret = 0x2;
		break;
	case DPE2_CHN0:
		if (is_yuv_semi_planar) {
			ret = 0x4;
		} else {
			ret = 0x3;
		}
		break;
	case WBE1_CHN1:
		ret = 0x5;
		break;
	case DPE3_CHN0:
		if (is_yuv_planar) {
			ret = 0x8;
		} else {
			ret = 0x7;
		}
		break;
	case DPE3_CHN1:
		ret = 0x6;
		break;
	case DPE3_CHN2:
		ret = 0x9;
		break;

	default:
		break;
	}

	return ret;
}

static void hisi_dss_mmu_dma_init(char __iomem *mmu_dma_base,
	dss_mmu_dma_t *s_mmu_dma)
{
	BUG_ON(mmu_dma_base == NULL);
	BUG_ON(s_mmu_dma == NULL);

	memset(s_mmu_dma, 0, sizeof(dss_mmu_dma_t));

	s_mmu_dma->mmu_ch_ctrl = inp32(mmu_dma_base + MMU_CH_CTRL);
	s_mmu_dma->mmu_ch_ptba = inp32(mmu_dma_base + MMU_CH_PTBA);
	s_mmu_dma->mmu_ch_ptva = inp32(mmu_dma_base + MMU_CH_PTVA);
	s_mmu_dma->mmu_ch_amsk = inp32(mmu_dma_base + MMU_CH_AMSK);
}

static void hisi_dss_mmu_dma_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *mmu_dma_base, dss_mmu_dma_t *s_mmu_dma)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(mmu_dma_base == NULL);
	BUG_ON(s_mmu_dma == NULL);

	hisifd->set_reg(hisifd, mmu_dma_base + MMU_CH_CTRL,
		s_mmu_dma->mmu_ch_ctrl , 32, 0);
	hisifd->set_reg(hisifd, mmu_dma_base + MMU_CH_PTBA,
		s_mmu_dma->mmu_ch_ptba, 32, 0);
	hisifd->set_reg(hisifd, mmu_dma_base + MMU_CH_PTVA,
		s_mmu_dma->mmu_ch_ptva, 32, 0);
	hisifd->set_reg(hisifd, mmu_dma_base + MMU_CH_AMSK,
		s_mmu_dma->mmu_ch_amsk, 32, 0);
	hisifd->set_reg(hisifd, mmu_dma_base + MMU_AXI_ARB,
		0xD51, 32, 0);
}

static void hisi_dss_mmu_rptb_init(char __iomem *mmu_rptb_base,
	dss_mmu_rptb_t *s_mmu_rptb)
{
	int i = 0;

	BUG_ON(mmu_rptb_base == NULL);
	BUG_ON(s_mmu_rptb == NULL);

	memset(s_mmu_rptb, 0, sizeof(dss_mmu_rptb_t));

	s_mmu_rptb->mmu_rptb_ctl = inp32(mmu_rptb_base + MMU_RPTB_CTL);
	for (i = 0; i < DSS_ROT_MAX; i++) {
		s_mmu_rptb->mmu_rptb_load[i] = inp32(mmu_rptb_base + MMU_RPTB_LOAD0 + i * 0x4);
		s_mmu_rptb->mmu_rptb_ba[i] = inp32(mmu_rptb_base + MMU_RPTB_BA0 + i * 0x4);
	}
	s_mmu_rptb->mmu_rtlb_sel = inp32(mmu_rptb_base + MMU_RTLB_SEL);
}

static void hisi_dss_mmu_rptb_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *mmu_rptb_base, dss_mmu_rptb_t *s_mmu_rptb)
{
	int i = 0 ;

	BUG_ON(hisifd == NULL);
	BUG_ON(mmu_rptb_base == NULL);
	BUG_ON(s_mmu_rptb == NULL);

	hisifd->set_reg(hisifd, mmu_rptb_base + MMU_RPTB_CTL,
			s_mmu_rptb->mmu_rptb_ctl, 32, 0);

	for (i = 0; i < DSS_ROT_MAX; i++) {
		if (s_mmu_rptb->mmu_rptb_idx_used[i] == 1) {
			hisifd->set_reg(hisifd, mmu_rptb_base + MMU_RPTB_LOAD0 + i * 0x04,
				s_mmu_rptb->mmu_rptb_load[i], 32, 0);
			hisifd->set_reg(hisifd, mmu_rptb_base + MMU_RPTB_BA0 + i * 0x04,
				s_mmu_rptb->mmu_rptb_ba[i], 32, 0);
		}
	}

	hisifd->set_reg(hisifd, mmu_rptb_base + MMU_RTLB_SEL,
		s_mmu_rptb->mmu_rtlb_sel, 32, 0);
}

int hisi_dss_mmu_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_layer_t *layer, dss_wb_layer_t *wb_layer, bool rdma_stretch_enable)
{
	dss_mmu_dma_t *mmu_dma0 = NULL;
	dss_mmu_dma_t *mmu_dma1 = NULL;
	dss_mmu_dma_t *mmu_dma2 = NULL;
	dss_mmu_rptb_t *mmu_rptb = NULL;
	bool is_yuv_semi_planar = false;
	bool is_yuv_planar = false;
	uint32_t ptba = 0;
	uint32_t ptva = 0;
	int rot_idx = 0;

	int axi_cmd_mid = 0;
	int tlb_pri_thr = 0;
	int tlb_pri_force = 0;

	int tlb_cmd_accu = 0;
	int tlb_rtlb_sel = 0;
	int pref_va_ctl = 0;
	int tlb_flush = 0;
	int tlb_rtlb_en = 0;
	int tlb_en = 0;

	uint32_t rptb_size = 0;
	uint32_t rptb_offset = 0;
	uint32_t rptb_ba = 0;
	uint32_t rptb_dma_sel = 0;
	uint32_t buf_size = 0;

	int chn_idx = 0;
	uint32_t transform = 0;
	uint32_t flags = 0;
	dss_img_t *img = 0;
	dss_rptb_info_t *rptb_info = NULL;
	uint32_t need_cap = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON((layer == NULL) && (wb_layer == NULL));
	BUG_ON(layer && wb_layer);

	if (wb_layer) {
		img = &(wb_layer->dst);
		chn_idx = wb_layer->chn_idx;
		transform = wb_layer->transform;
		flags = wb_layer->flags;
		need_cap = wb_layer->need_cap;
		if (chn_idx == WBE1_CHN1)
			rptb_info = &(hisifd->dss_wb_rptb_info_cur[1]);
		else
			rptb_info = &(hisifd->dss_wb_rptb_info_cur[0]);
	} else {
		img = &(layer->src);
		chn_idx = layer->chn_idx;
		transform = layer->transform;
		flags = layer->flags;
		need_cap = layer->need_cap;

		if (hisifd->ovl_type == DSS_OVL_ADP) {
			if (pov_req->wb_layer_info.chn_idx == WBE1_CHN1)
				rptb_info = &(hisifd->dss_rptb_info_cur[1]);
			else
				rptb_info = &(hisifd->dss_rptb_info_cur[0]);
		} else {
			rptb_info = &(hisifd->dss_rptb_info_cur[0]);
		}
	}

	is_yuv_semi_planar = isYUVSemiPlanar(img->format);
	is_yuv_planar = isYUVPlanar(img->format);

	mmu_dma0 = &(hisifd->dss_module.mmu_dma0[chn_idx]);
	hisifd->dss_module.mmu_dma0_used[chn_idx] = 1;
	if (is_yuv_semi_planar || is_yuv_planar) {
		mmu_dma1 = &(hisifd->dss_module.mmu_dma1[chn_idx]);
		hisifd->dss_module.mmu_dma1_used[chn_idx] = 1;
	}
	if (is_yuv_planar) {
		mmu_dma2 = &(hisifd->dss_module.mmu_dma0[chn_idx + 1]);
		hisifd->dss_module.mmu_dma0_used[chn_idx + 1] = 1;
	}

	if (img->ptba & (MMU_CH_PTBA_ALIGN - 1)) {
		HISI_FB_ERR("chn%d ptba(0x%x) is not %d bytes aligned.\n",
			chn_idx, img->ptba, MMU_CH_PTBA_ALIGN);
		return -EINVAL;
	}

	if (img->ptva & (MMU_CH_PTVA_ALIGN - 1)) {
		HISI_FB_ERR("chn%d ptva(0x%x) is not %d bytes aligned.\n",
			chn_idx, img->ptba, MMU_CH_PTVA_ALIGN);
		return -EINVAL;
	}

	tlb_en = 1;
	ptba = img->ptba;
	ptva = img->ptva;

	if (transform & HISI_FB_TRANSFORM_ROT_90) {
		rot_idx = hisi_get_rot_index(chn_idx);
		if (rot_idx < 0 || (need_cap & (CAP_BASE | CAP_DIM))) {
			HISI_FB_ERR("chn(%d), failed to hisi_get_rot_index! rot_idx=%d", chn_idx, rot_idx);
			goto err_rptb;
		}

		rptb_dma_sel = hisi_rptb_pt_dma_sel(chn_idx, img->format);

		if (rptb_info->rptb_size[rot_idx] <= 0) {
			buf_size = img->buf_size;
			rptb_size = ALIGN_UP(((((buf_size + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE) << 2) +
				(MMU_RPTB_WIDTH - 1)) / MMU_RPTB_WIDTH, MMU_RPTB_SIZE_ALIGN);

			rptb_offset = hisi_dss_rptb_alloc(g_rptb_gen_pool, rptb_size);
			if (rptb_offset < MMU_RPTB_SIZE_ALIGN) {
				HISI_FB_ERR("hisi_dss_rptb_alloc failed! rot_idx=%d, rptb_siz=%d, rptb_offset=%d.\n",
					rot_idx, rptb_size, rptb_offset);
				goto err_rptb;
			}

			rptb_info->rptb_offset[rot_idx] = rptb_offset;
			rptb_info->rptb_size[rot_idx] = rptb_size;
		} else {
			rptb_offset = rptb_info->rptb_offset[rot_idx];
			rptb_size = rptb_info->rptb_size[rot_idx];
			//rptb_size = 0x0;
		}

		tlb_rtlb_sel = rot_idx;
		tlb_rtlb_en = 1;
		pref_va_ctl = 1;
		rptb_ba = (img->ptba + ((img->vir_addr - img->ptva) >> 10)) & 0xFFFFFF80;
		rptb_offset -= MMU_RPTB_SIZE_ALIGN;

		ptba = rptb_offset << 4;
		ptva = img->vir_addr & 0xFFFE0000;

		mmu_rptb = &(hisifd->dss_module.mmu_rptb);
		mmu_rptb->mmu_rptb_idx_used[rot_idx] = 1;
		hisifd->dss_module.mmu_rptb_used = 1;

		mmu_rptb->mmu_rptb_ctl = set_bits32(mmu_rptb->mmu_rptb_ctl, 0x1, 1, rot_idx);
		mmu_rptb->mmu_rptb_ctl = set_bits32(mmu_rptb->mmu_rptb_ctl, 0x1, 1, (26 + rot_idx));
		mmu_rptb->mmu_rptb_ctl = set_bits32(mmu_rptb->mmu_rptb_ctl, 0x7, 3, 13);
		mmu_rptb->mmu_rptb_load[rot_idx] = set_bits32(mmu_rptb->mmu_rptb_load[rot_idx],
			(rptb_offset | (rptb_size << 12)), 24, 0);
		mmu_rptb->mmu_rptb_ba[rot_idx] = set_bits32(mmu_rptb->mmu_rptb_ba[rot_idx],
			rptb_ba, 32, 0);
		mmu_rptb->mmu_rtlb_sel = set_bits32(mmu_rptb->mmu_rtlb_sel, 0x1, 1, (27 + rot_idx));
		mmu_rptb->mmu_rtlb_sel = set_bits32(mmu_rptb->mmu_rtlb_sel, rptb_dma_sel, 4, (rot_idx * 4));

		if (hisifd->dss_glb.rot_tlb_scene_base[rot_idx]) {
			hisifd->dss_glb.rot_tlb_scene_used[rot_idx] = 1;
			hisifd->dss_glb.rot_tlb_scene_val[rot_idx] =
				set_bits32(hisifd->dss_glb.rot_tlb_scene_val[rot_idx], flags, 3, 0);
		} else {
			HISI_FB_ERR("rot_tlb_scene_base[%d] is invalid!\n", rot_idx);
		}
	}

err_rptb:
	mmu_dma0->mmu_ch_ctrl = set_bits32(mmu_dma0->mmu_ch_ctrl,
		hisi_get_mmu_tlb_tag_val(img->format, transform, img->is_tile, rdma_stretch_enable) |
		tlb_en | (tlb_rtlb_en << 1) | (tlb_flush << 2) |(pref_va_ctl << 3) | (tlb_rtlb_sel << 9) |
		(tlb_cmd_accu << 12) | (tlb_pri_force << 21) | (tlb_pri_thr << 22) |
		(axi_cmd_mid << 25), 29, 0);
	mmu_dma0->mmu_ch_ptba = set_bits32(mmu_dma0->mmu_ch_ptba, ptba, 32, 0);
	mmu_dma0->mmu_ch_ptva = set_bits32(mmu_dma0->mmu_ch_ptva, ptva, 32, 0);
	mmu_dma0->mmu_ch_amsk = set_bits32(mmu_dma0->mmu_ch_amsk,
		((img->stride + MMU_CH_AMSK_THRESHOLD - 1) /
		MMU_CH_AMSK_THRESHOLD + 1), 7, 12);

	if (is_yuv_semi_planar || is_yuv_planar) {
		memcpy(mmu_dma1, mmu_dma0, sizeof(dss_mmu_dma_t));
	}
	if (is_yuv_planar) {
		memcpy(mmu_dma2, mmu_dma0, sizeof(dss_mmu_dma_t));
	}

	return 0;
}


/*******************************************************************************
** DSS RDMA
*/
static int hisi_get_rdma_tile_interleave(uint32_t stride)
{
	int i = 0;
	uint32_t interleave[MAX_TILE_SURPORT_NUM] = {256, 512, 1024, 2048, 4096, 8192};

	for (i = 0; i < MAX_TILE_SURPORT_NUM; i++) {
		if (interleave[i] == stride)
			return MIN_INTERLEAVE + i;
	}

	return 0;
}

static int fix_scf_input(dss_layer_t *layer)
{
	if ((layer->need_cap & CAP_SCL) == 0)
		return 0;

	/* if layer has no h scf, do not pad 3 pix */
	if (((layer->block_info.h_ratio != SCF_CONST_FACTOR) && (layer->block_info.h_ratio != 0))
		|| ((layer->block_info.v_ratio != SCF_CONST_FACTOR) && (layer->block_info.v_ratio != 0))) {
		/* offline block case */
                if (layer->block_info.h_ratio == SCF_CONST_FACTOR)
                        return 0;
                else if (layer->src_rect.w >= SCF_MIN_INPUT
                        && (layer->transform & HISI_FB_TRANSFORM_ROT_90) == 0)
                        return 0;
                else if (layer->src_rect.h >= SCF_MIN_INPUT
                        && layer->transform & HISI_FB_TRANSFORM_ROT_90)
                        return 0;
	} else {
		/* online or offline no block case */
		if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
			if (layer->src_rect.w == layer->dst_rect.h)
				return 0;
		} else {
			if (layer->src_rect.w == layer->dst_rect.w)
				return 0;
		}
	}

	/* online and offline no block case */
	switch (layer->transform) {
		case HISI_FB_TRANSFORM_NOP:
		case HISI_FB_TRANSFORM_FLIP_V:
		case HISI_FB_TRANSFORM_FLIP_H:
		case HISI_FB_TRANSFORM_ROT_180:
			if (layer->src_rect.w >= SCF_MIN_INPUT)
				return 0;

			if (layer->block_info.first_tile && layer->block_info.last_tile) {
				HISI_FB_ERR("scf layer[%d] size is too small{%d,%d,%d,%d}\n",
					layer->chn_idx,
					layer->src_rect.x, layer->src_rect.y,
					layer->src_rect.w, layer->src_rect.h);
				return 1;
			} else if (layer->block_info.first_tile) {
				if (layer->transform == HISI_FB_TRANSFORM_FLIP_H
                                        || layer->transform == HISI_FB_TRANSFORM_ROT_180)
					layer->src_rect.x = layer->src_rect.x - SCF_MIN_OFFSET;
			} else {
                                /* if here,last_tile must to be 1, because middle block pix > 6 */
				if (layer->transform == 0 || layer->transform == HISI_FB_TRANSFORM_FLIP_V)
					layer->src_rect.x = layer->src_rect.x - SCF_MIN_OFFSET;

                                layer->block_info.acc_hscl += (SCF_MIN_OFFSET << 18);
			}
			layer->src_rect.w = layer->src_rect.w + SCF_MIN_OFFSET;

			break;
                case HISI_FB_TRANSFORM_FLIP_H | HISI_FB_TRANSFORM_ROT_90:
                case HISI_FB_TRANSFORM_FLIP_V | HISI_FB_TRANSFORM_ROT_90:
                case HISI_FB_TRANSFORM_ROT_90:
                case HISI_FB_TRANSFORM_ROT_270:
			if (layer->src_rect.h >= SCF_MIN_INPUT)
				return 0;

			if (layer->block_info.first_tile && layer->block_info.last_tile) {
				HISI_FB_ERR("scf layer[%d] size is too small{%d,%d,%d,%d}\n",
					layer->chn_idx,
					layer->src_rect.x, layer->src_rect.y,
					layer->src_rect.w, layer->src_rect.h);
				return 1;
			} else if (layer->block_info.first_tile) {
				if (layer->transform == HISI_FB_TRANSFORM_ROT_90
                                        || layer->transform == (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H))
					layer->src_rect.y = layer->src_rect.y - SCF_MIN_OFFSET;
			} else {
                                /* if here,last_tile must to be 1, because middle block pix > 6 */
				if (layer->transform == HISI_FB_TRANSFORM_ROT_270
                                        || layer->transform == (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V))
					layer->src_rect.y = layer->src_rect.y - SCF_MIN_OFFSET;

                                layer->block_info.acc_hscl += (SCF_MIN_OFFSET << 18);
			}
			layer->src_rect.h = layer->src_rect.h + SCF_MIN_OFFSET;
			break;
		default:
			HISI_FB_ERR("unknow dss_layer->transform: %d\n",layer->transform);
			return 1;
	}

	if (layer->src_rect.h < SCF_MIN_INPUT && layer->src_rect.w < SCF_MIN_INPUT) {
		HISI_FB_ERR("read more data,but scf[%d] still not satisfy 6*6 ! dump layer{%d,%d,%d,%d},transform[%d]\n",
			layer->chn_idx,
			layer->src_rect.x, layer->src_rect.y,
			layer->src_rect.w, layer->src_rect.h,
			layer->transform);
		return 1;
	}

	return 0;
}

static void hisi_dss_rdma_init(char __iomem *rdma_base, dss_rdma_t *s_rdma)
{
	BUG_ON(rdma_base == NULL);
	BUG_ON(s_rdma == NULL);

	s_rdma->rdma_data_addr = inp32(rdma_base + RDMA_DATA_ADDR);
	s_rdma->rdma_oft_x0 = inp32(rdma_base + RDMA_OFT_X0);
	s_rdma->rdma_oft_y0 = inp32(rdma_base + RDMA_OFT_Y0);
	s_rdma->rdma_rot_qos_lev = inp32(rdma_base + RDMA_ROT_QOS_LEV);
	s_rdma->rdma_stride = inp32(rdma_base + RDMA_STRIDE);
	s_rdma->rdma_oft_x1 = inp32(rdma_base + RDMA_OFT_X1);
	s_rdma->rdma_oft_y1 = inp32(rdma_base + RDMA_OFT_Y1);
	s_rdma->rdma_mask0 = inp32(rdma_base + RDMA_MASK0);
	s_rdma->rdma_mask1 = inp32(rdma_base + RDMA_MASK1);
	s_rdma->rdma_stretch_size_vrt = inp32(rdma_base + RDMA_STRETCH_SIZE_VRT);
	s_rdma->rdma_stretch_line_num = inp32(rdma_base + RDMA_STRETCH_LINE_NUM);
	//s_rdma->rdma_ctrl = inp32(rdma_base + RDMA_CTRL);
	s_rdma->rdma_ctrl = 0x0;
	s_rdma->rdma_data_num = inp32(rdma_base + RDMA_DATA_NUM);
	s_rdma->rdma_tile_scram = inp32(rdma_base + RDMA_TILE_SCRAM);
}

static void hisi_dss_rdma_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *rdma_base, dss_rdma_t *s_dma)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(rdma_base == NULL);
	BUG_ON(s_dma == NULL);

	hisifd->set_reg(hisifd, rdma_base + RDMA_DATA_ADDR, s_dma->rdma_data_addr, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_OFT_X0, s_dma->rdma_oft_x0, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_OFT_Y0, s_dma->rdma_oft_y0, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_ROT_QOS_LEV, s_dma->rdma_rot_qos_lev, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_STRIDE, s_dma->rdma_stride, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_OFT_X1, s_dma->rdma_oft_x1, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_OFT_Y1, s_dma->rdma_oft_y1, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_MASK0, s_dma->rdma_mask0, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_MASK1, s_dma->rdma_mask1, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_STRETCH_SIZE_VRT, s_dma->rdma_stretch_size_vrt, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_STRETCH_LINE_NUM, s_dma->rdma_stretch_line_num, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_CTRL, s_dma->rdma_ctrl, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_DATA_NUM, s_dma->rdma_data_num, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_TILE_SCRAM, s_dma->rdma_tile_scram, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + RDMA_BAK, 0x1, 32, 0);
}

static int hisi_dss_dfc_clip_check(dss_rect_ltrb_t *clip_rect)
{
	if ((clip_rect->left < 0 || clip_rect->left > DFC_MAX_CLIP_NUM) ||
		(clip_rect->right < 0 || clip_rect->right > DFC_MAX_CLIP_NUM) ||
		(clip_rect->top < 0 || clip_rect->top > DFC_MAX_CLIP_NUM) ||
		(clip_rect->bottom < 0 || clip_rect->bottom > DFC_MAX_CLIP_NUM)) {
		return 1;
	}

	return 0;
}

dss_rect_t hisi_dss_rdma_out_rect(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	dss_rect_t out_aligned_rect;
	dss_rect_ltrb_t aligned_rect;
	dss_rect_t new_src_rect;
	int aligned_pixel = 0;
	int bpp = 0;
	uint32_t stretched_line_num = 0;
	uint32_t temp = 0;

	int dfc_fmt = 0;
	int dfc_bpp = 0;
	int dfc_aligned = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	new_src_rect = layer->src_rect;
	stretched_line_num = isNeedRdmaStretchBlt(hisifd, layer);

	bpp = (isYUVSemiPlanar(layer->src.format) || isYUVPlanar(layer->src.format))
		? 1 : layer->src.bpp;
	aligned_pixel = DMA_ALIGN_BYTES / bpp;

	dfc_fmt = hisi_pixel_format_hal2dfc(layer->src.format, layer->transform);
	if (dfc_fmt < 0) {
		HISI_FB_ERR("layer format (%d) not support !\n", layer->src.format);
	}

	dfc_bpp = hisi_dfc_get_bpp(dfc_fmt);
	if (dfc_bpp <= 0) {
		HISI_FB_ERR("dfc_bpp(%d) not support !\n", dfc_bpp);
	}

	dfc_aligned = (dfc_bpp <= 2) ? 4 : 2;

	/* aligned_rect */
	if (is_YUV_P_420(layer->src.format) || is_YUV_P_422(layer->src.format)) {
		aligned_rect.left = ALIGN_DOWN(new_src_rect.x, 2 * aligned_pixel);
		aligned_rect.right = ALIGN_UP(new_src_rect.x + new_src_rect.w, 2 * aligned_pixel) - 1;
	} else {
		aligned_rect.left = ALIGN_DOWN(new_src_rect.x, aligned_pixel);
		aligned_rect.right = ALIGN_UP(new_src_rect.x + new_src_rect.w, aligned_pixel) - 1;
	}

	if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
		if (stretched_line_num > 0) {
			new_src_rect.h = ALIGN_DOWN(new_src_rect.h / stretched_line_num, dfc_aligned) *
				stretched_line_num;

			if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
				aligned_rect.top = ALIGN_DOWN(new_src_rect.y, 2);
				aligned_rect.bottom = DSS_HEIGHT(aligned_rect.top + new_src_rect.h);
			} else {
				aligned_rect.top = new_src_rect.y;
				aligned_rect.bottom = DSS_HEIGHT(new_src_rect.y + new_src_rect.h);
			}
		} else {
			aligned_rect.top = ALIGN_DOWN(new_src_rect.y, aligned_pixel);
			aligned_rect.bottom = ALIGN_UP(new_src_rect.y + new_src_rect.h, aligned_pixel) - 1;
		}
	} else {
		if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
			aligned_rect.top = ALIGN_DOWN(new_src_rect.y, 2);
			aligned_rect.bottom = ALIGN_UP(new_src_rect.y + new_src_rect.h, 2) - 1;
		} else {
			aligned_rect.top = new_src_rect.y;
			aligned_rect.bottom = DSS_HEIGHT(new_src_rect.y + new_src_rect.h);
		}
	}

	/* out_rect */
	out_aligned_rect.x = 0;
	out_aligned_rect.y = 0;
	out_aligned_rect.w = aligned_rect.right - aligned_rect.left + 1;
	out_aligned_rect.h = aligned_rect.bottom - aligned_rect.top + 1;
	if (stretched_line_num > 0) {
		temp = (out_aligned_rect.h / stretched_line_num) +
			((out_aligned_rect.h % stretched_line_num) ? 1 : 0) - 1;

		out_aligned_rect.h = temp + 1;
	}

	if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
		temp = out_aligned_rect.w;
		out_aligned_rect.w = out_aligned_rect.h;
		out_aligned_rect.h = temp;
	}

	return out_aligned_rect;
}

int hisi_dss_rdma_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_layer_t *layer, dss_rect_ltrb_t *clip_rect,
	dss_rect_t *out_aligned_rect, bool *rdma_stretch_enable)
{
	dss_rdma_t *dma0 = NULL;
	dss_rdma_t *dma1 = NULL;
	dss_rdma_t *dma2 = NULL;

	bool mmu_enable = false;
	bool is_yuv_semi_planar = false;
	bool is_yuv_planar = false;
	bool src_rect_mask_enable = false;

	uint32_t rdma_addr = 0;
	uint32_t rdma_stride = 0;
	int rdma_format = 0;
	int rdma_transform = 0;
	int rdma_data_num = 0;
	uint32_t stretch_size_vrt = 0;
	uint32_t stretched_line_num = 0;
	uint32_t stretched_stride = 0;
	uint32_t rdma_rot_burst4 = 0;

	int bpp = 0;
	int aligned_pixel = 0;
	int rdma_oft_x0 = 0;
	int rdma_oft_y0 = 0;
	int rdma_oft_x1 = 0;
	int rdma_oft_y1 = 0;
	int rdma_mask_x0 = 0;
	int rdma_mask_y0 = 0;
	int rdma_mask_x1 = 0;
	int rdma_mask_y1 = 0;

	int chn_idx = 0;
	uint32_t l2t_interleave_n = 0;

	dss_rect_ltrb_t aligned_rect;
	dss_rect_t new_src_rect;
	uint32_t temp = 0;
	int tmp = 0;

	int dfc_fmt = 0;
	int dfc_bpp = 0;
	int dfc_aligned = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);
	BUG_ON(pov_req == NULL);

	if (hisifd->ovl_type == DSS_OVL_ADP) {
		if (fix_scf_input(layer) != 0) {
			HISI_FB_ERR("fix_scf_input err\n");
			return -EINVAL;
		}
	}

	if (g_fpga_flag == 1) {
		if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
			char __iomem *rot_pm_ctrl_base = hisifd->dss_base +
				g_dss_module_base[layer->chn_idx][MODULE_ROT_PM_CTL];
			if (rot_pm_ctrl_base) {
				hisifd->set_reg(hisifd, rot_pm_ctrl_base, 1, 1, 21);
			}
		}
	}

	chn_idx = layer->chn_idx;
	new_src_rect = layer->src_rect;

	stretched_line_num = isNeedRdmaStretchBlt(hisifd, layer);
	*rdma_stretch_enable = (stretched_line_num > 0) ? true : false;

	mmu_enable = (layer->src.mmu_enable == 1) ? true : false;
	is_yuv_semi_planar = isYUVSemiPlanar(layer->src.format);
	is_yuv_planar = isYUVPlanar(layer->src.format);

#if 0
	if (is_yuv_semi_planar || is_yuv_planar) {
		if (!IS_EVEN(new_src_rect.h)) {
			HISI_FB_ERR("YUV semi_planar or planar format(%d), src_rect's height(%d) is invalid!\n",
				layer->src.format, new_src_rect.h);
		}
	}
#endif

	dma0 = &(hisifd->dss_module.rdma0[chn_idx]);
	hisifd->dss_module.rdma0_used[chn_idx] = 1;
	if (is_yuv_semi_planar || is_yuv_planar) {
		dma1 = &(hisifd->dss_module.rdma1[chn_idx]);
		hisifd->dss_module.rdma1_used[chn_idx] = 1;
	}
	if (is_yuv_planar) {
		dma2 = &(hisifd->dss_module.rdma0[chn_idx + 1]);
		hisifd->dss_module.rdma0_used[chn_idx + 1] = 1;
	}

	if ((chn_idx >= DPE3_CHN0) && (chn_idx <= DPE3_CHN3) &&
		(layer->transform & HISI_FB_TRANSFORM_ROT_90) &&
		(!layer->src.is_tile) && (layer->src.bpp == 4)) {
		rdma_rot_burst4 = 1;
	}

	rdma_addr = mmu_enable ? layer->src.vir_addr : layer->src.phy_addr;
	if (rdma_addr & (DMA_ADDR_ALIGN - 1)) {
		HISI_FB_ERR("layer%d rdma_addr(0x%x) is not %d bytes aligned.\n",
			layer->layer_idx, rdma_addr, DMA_ADDR_ALIGN);
		return -EINVAL;
	}

	if (layer->src.stride & (DMA_STRIDE_ALIGN - 1)) {
		HISI_FB_ERR("layer%d stride(0x%x) is not %d bytes aligned.\n",
			layer->layer_idx, layer->src.stride, DMA_STRIDE_ALIGN);
		return -EINVAL;
	}

	rdma_format = hisi_pixel_format_hal2dma(layer->src.format);
	if (rdma_format < 0) {
		HISI_FB_ERR("layer format(%d) not support !\n", layer->src.format);
		return -EINVAL;
	}

	rdma_transform = hisi_transform_hal2dma(layer->transform, chn_idx);
	if (rdma_transform < 0) {
		HISI_FB_ERR("layer transform(%d) not support!\n", layer->transform);
		return -EINVAL;
	}

	if (layer->src.is_tile) {
		l2t_interleave_n = hisi_get_rdma_tile_interleave(layer->src.stride);
		if (l2t_interleave_n < MIN_INTERLEAVE) {
			HISI_FB_ERR("tile stride should be 256*2^n, error stride:%d!\n", layer->src.stride);
			return -EINVAL;
		}
	}

	dfc_fmt = hisi_pixel_format_hal2dfc(layer->src.format, layer->transform);
	if (dfc_fmt < 0) {
		HISI_FB_ERR("layer format (%d) not support !\n", layer->src.format);
		return -EINVAL;
	}

	dfc_bpp = hisi_dfc_get_bpp(dfc_fmt);
	if (dfc_bpp <= 0) {
		HISI_FB_ERR("dfc_bpp(%d) not support !\n", dfc_bpp);
		return -EINVAL;
	}

	dfc_aligned = (dfc_bpp <= 2) ? 4 : 2;

	bpp = (is_yuv_semi_planar || is_yuv_planar) ? 1 : layer->src.bpp;
	aligned_pixel = DMA_ALIGN_BYTES / bpp;

	/* handle aligned_rect and clip_rect */
	{
		/* aligned_rect */
		if (is_YUV_P_420(layer->src.format) || is_YUV_P_422(layer->src.format)) {
			aligned_rect.left = ALIGN_DOWN(new_src_rect.x, 2 * aligned_pixel);
			aligned_rect.right = ALIGN_UP(new_src_rect.x + new_src_rect.w, 2 * aligned_pixel) - 1;
		} else {
			aligned_rect.left = ALIGN_DOWN(new_src_rect.x, aligned_pixel);
			aligned_rect.right = ALIGN_UP(new_src_rect.x + new_src_rect.w, aligned_pixel) - 1;
		}

		if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
			if (stretched_line_num > 0) {
				new_src_rect.h = ALIGN_DOWN(new_src_rect.h / stretched_line_num, dfc_aligned) *
					stretched_line_num;

				if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
					aligned_rect.top = ALIGN_DOWN(new_src_rect.y, 2);
					aligned_rect.bottom = DSS_HEIGHT(aligned_rect.top + new_src_rect.h);
				} else {
					aligned_rect.top = new_src_rect.y;
					aligned_rect.bottom = DSS_HEIGHT(new_src_rect.y + new_src_rect.h);
				}
			} else {
				aligned_rect.top = ALIGN_DOWN(new_src_rect.y, aligned_pixel);
				aligned_rect.bottom = ALIGN_UP(new_src_rect.y + new_src_rect.h, aligned_pixel) - 1;
			}
		} else {
			if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
				aligned_rect.top = ALIGN_DOWN(new_src_rect.y, 2);
				aligned_rect.bottom = ALIGN_UP(new_src_rect.y + new_src_rect.h, 2) - 1;
			} else {
				aligned_rect.top = new_src_rect.y;
				aligned_rect.bottom = DSS_HEIGHT(new_src_rect.y + new_src_rect.h);
			}
		}

		/* out_rect */
		out_aligned_rect->x = 0;
		out_aligned_rect->y = 0;
		out_aligned_rect->w = aligned_rect.right - aligned_rect.left + 1;
		out_aligned_rect->h = aligned_rect.bottom - aligned_rect.top + 1;
		if (stretched_line_num > 0) {
			stretch_size_vrt = (out_aligned_rect->h / stretched_line_num) +
				((out_aligned_rect->h % stretched_line_num) ? 1 : 0) - 1;

			out_aligned_rect->h = stretch_size_vrt + 1;
		} else {
			stretch_size_vrt = 0x0;
		}

		if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
			temp = out_aligned_rect->w;
			out_aligned_rect->w = out_aligned_rect->h;
			out_aligned_rect->h = temp;
		}

		/* clip_rect */
		tmp = new_src_rect.x - aligned_rect.left;
		clip_rect->left = (tmp > 0) ? tmp : 0;
		tmp = aligned_rect.right - DSS_WIDTH(new_src_rect.x + new_src_rect.w);
		clip_rect->right = (tmp > 0) ? tmp : 0;

		tmp = new_src_rect.y - aligned_rect.top;
		clip_rect->top = (tmp > 0) ? tmp : 0;
		tmp = aligned_rect.bottom - DSS_HEIGHT(new_src_rect.y + new_src_rect.h);
		clip_rect->bottom = (tmp > 0) ? tmp : 0;

		if (hisi_dss_dfc_clip_check(clip_rect)) {
			HISI_FB_ERR("clip rect unvalid => layer_idx=%d, chn_idx=%d, clip_rect(%d, %d, %d, %d).\n",
				layer->layer_idx, chn_idx, clip_rect->left, clip_rect->right,
				clip_rect->top, clip_rect->bottom);
			return -EINVAL;
		}

		hisi_adjust_clip_rect(layer, clip_rect);

		if (g_debug_ovl_online_composer) {
			HISI_FB_ERR("layer_idx%d, chn_idx=%d, format=%d, transform=%d, "
				"original_src_rect(%d,%d,%d,%d), aligned_rect(%d,%d,%d,%d), clip_rect(%d,%d,%d,%d), "
				"rdma_out_rect(%d,%d,%d,%d), dst_rect(%d,%d,%d,%d)!\n",
				layer->layer_idx, chn_idx, layer->src.format, layer->transform,
				layer->src_rect.x, layer->src_rect.y, layer->src_rect.w, layer->src_rect.h,
				aligned_rect.left, aligned_rect.top, aligned_rect.right, aligned_rect.bottom,
				clip_rect->left, clip_rect->top, clip_rect->right, clip_rect->bottom,
				out_aligned_rect->x, out_aligned_rect->y, out_aligned_rect->w, out_aligned_rect->h,
				layer->dst_rect.x, layer->dst_rect.y, layer->dst_rect.w, layer->dst_rect.h);
		}
	}

	rdma_oft_y0 = aligned_rect.top;
	rdma_oft_y1 = aligned_rect.bottom;
	rdma_oft_x0 = aligned_rect.left / aligned_pixel;
	rdma_oft_x1 = aligned_rect.right / aligned_pixel;

	if ((rdma_oft_x1 - rdma_oft_x0) < 0 ||
		(rdma_oft_x1 - rdma_oft_x0 + 1) > DMA_IN_WIDTH_MAX) {
		HISI_FB_ERR("out of range, rdma_oft_x0 = %d, rdma_oft_x1 = %d!\n",
			rdma_oft_x0, rdma_oft_x1);
		return -EINVAL;
	}

	if ((rdma_oft_y1 - rdma_oft_y0) < 0 ||
		(rdma_oft_y1 - rdma_oft_y0 + 1) > DMA_IN_HEIGHT_MAX) {
		HISI_FB_ERR("out of range, rdma_oft_y0 = %d, rdma_oft_y1 = %d\n",
			rdma_oft_y0, rdma_oft_y1);
		return -EINVAL;
	}

	rdma_addr = hisi_calculate_display_addr(mmu_enable, layer, &aligned_rect, DSS_ADDR_PLANE0);
	rdma_stride = layer->src.stride;
	rdma_data_num = (rdma_oft_x1 - rdma_oft_x0 + 1) * (rdma_oft_y1- rdma_oft_y0 + 1);

	src_rect_mask_enable = isSrcRectMasked(layer, aligned_pixel);

	if (src_rect_mask_enable) {
		rdma_mask_x0 = ALIGN_UP(layer->src_rect_mask.x, aligned_pixel) / aligned_pixel;
		//rdma_mask_x1 = DSS_WIDTH(layer->src_rect_mask.x + layer->src_rect_mask.w) / aligned_pixel -1;
		rdma_mask_x1 = (ALIGN_DOWN(layer->src_rect_mask.x + layer->src_rect_mask.w, aligned_pixel) - 1) / aligned_pixel;
		rdma_mask_y0 = layer->src_rect_mask.y;
		rdma_mask_y1 = DSS_HEIGHT(layer->src_rect_mask.y + layer->src_rect_mask.h);
	}


	if (stretched_line_num > 0) {
		stretched_stride = stretched_line_num * layer->src.stride / DMA_ALIGN_BYTES;
		rdma_data_num = (rdma_oft_x1 - rdma_oft_x0 + 1) * (stretch_size_vrt + 1);
	} else {
		stretch_size_vrt = 0x0;
		stretched_line_num = 0x0;
		stretched_stride = 0x0;
	}

	dma0->rdma_data_addr = set_bits32(dma0->rdma_data_addr, rdma_addr / DMA_ALIGN_BYTES, 28, 0);
	dma0->rdma_oft_x0 = set_bits32(dma0->rdma_oft_x0, rdma_oft_x0, 12, 0);
	dma0->rdma_oft_y0 = set_bits32(dma0->rdma_oft_y0, rdma_oft_y0, 16, 0);
	dma0->rdma_stride = set_bits32(dma0->rdma_stride, rdma_stride / DMA_ALIGN_BYTES, 13, 0);
	dma0->rdma_oft_x1 = set_bits32(dma0->rdma_oft_x1, rdma_oft_x1, 12, 0);
	dma0->rdma_oft_y1 = set_bits32(dma0->rdma_oft_y1, rdma_oft_y1, 16, 0);
	dma0->rdma_mask0 = set_bits32(dma0->rdma_mask0,
		(rdma_mask_y0 | (rdma_mask_x0 << 16)), 28, 0);
	dma0->rdma_mask1 = set_bits32(dma0->rdma_mask1,
		(rdma_mask_y1 | (rdma_mask_x1 << 16)), 28, 0);
	dma0->rdma_stretch_size_vrt = set_bits32(dma0->rdma_stretch_size_vrt,
		stretch_size_vrt, 16, 0);
	dma0->rdma_stretch_line_num = set_bits32(dma0->rdma_stretch_line_num,
		(stretched_line_num | (stretched_stride << 6)), 25, 0);
	/* FIXME: rdma_page_8k */
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, (layer->src.is_tile ? 0x1 : 0x0), 1, 1);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, ((layer->need_cap & CAP_FBDC) ? 0x1 : 0x0), 1, 2);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, rdma_format, 5, 3);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, (mmu_enable ? 0x1 : 0x0), 1, 8);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, rdma_transform, 3, 9);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, (*rdma_stretch_enable ? 0x1 : 0x0), 1, 12);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, l2t_interleave_n, 4, 13);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, (src_rect_mask_enable ? 0x1 : 0x0), 1, 17);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, rdma_rot_burst4, 1, 22);
	dma0->rdma_ctrl = set_bits32(dma0->rdma_ctrl, 0x1, 1, 25);
	dma0->rdma_data_num = set_bits32(dma0->rdma_data_num, rdma_data_num, 30, 0);
	dma0->rdma_tile_scram = set_bits32(dma0->rdma_tile_scram, (layer->src.is_tile ? 0x1 : 0x0), 1, 0);


	if (is_yuv_semi_planar || is_yuv_planar) {
		if (is_YUV_P_420(layer->src.format) || is_YUV_P_422(layer->src.format)) {
			rdma_oft_x0 /= 2;
			rdma_oft_x1 = (rdma_oft_x1 + 1) / 2 - 1;
		}

		if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
			rdma_oft_y0 /= 2;
			rdma_oft_y1 = (rdma_oft_y1 + 1) / 2 - 1;

			stretched_line_num /= 2;
		}

		rdma_addr = hisi_calculate_display_addr(mmu_enable, layer, &aligned_rect, DSS_ADDR_PLANE1);
		rdma_stride = layer->src.uStride;

		if (src_rect_mask_enable) {
			if (is_YUV_P_420(layer->src.format) || is_YUV_P_422(layer->src.format)) {
				rdma_mask_x0 /= 2;
				rdma_mask_x1 = (rdma_mask_x1 + 1) / 2 - 1;
			}

			if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {
				rdma_mask_y0 /= 2;
				rdma_mask_y1 = (rdma_mask_y1 + 1) / 2 - 1;
			}
		}

		if (*rdma_stretch_enable) {
			//stretch_size_vrt = stretch_size_vrt;
			stretched_stride = stretched_line_num * layer->src.uStride / DMA_ALIGN_BYTES;

			rdma_data_num = (rdma_oft_x1 - rdma_oft_x0 + 1) * (stretch_size_vrt + 1);
		} else {
			stretch_size_vrt = 0;
			stretched_line_num = 0;
			stretched_stride = 0;

			rdma_data_num = (rdma_oft_x1 - rdma_oft_x0 + 1) * (rdma_oft_y1- rdma_oft_y0 + 1);
		}

		if (is_YUV_SP_420(layer->src.format) || is_YUV_P_420(layer->src.format)) {/* [false alarm] */
			if (!(layer->transform & HISI_FB_TRANSFORM_ROT_90) && !(*rdma_stretch_enable)) {
				rdma_data_num *= 2;
			}
		}

		dma1->rdma_data_addr = set_bits32(dma1->rdma_data_addr, rdma_addr / DMA_ALIGN_BYTES, 28, 0);/* [false alarm] */
		dma1->rdma_oft_x0 = set_bits32(dma1->rdma_oft_x0, rdma_oft_x0, 12, 0);/* [false alarm] */
		dma1->rdma_oft_y0 = set_bits32(dma1->rdma_oft_y0, rdma_oft_y0, 16, 0);/* [false alarm] */
		dma1->rdma_stride = set_bits32(dma1->rdma_stride, rdma_stride / DMA_ALIGN_BYTES, 13, 0);/* [false alarm] */
		dma1->rdma_oft_x1 = set_bits32(dma1->rdma_oft_x1, rdma_oft_x1, 12, 0);/* [false alarm] */
		dma1->rdma_oft_y1 = set_bits32(dma1->rdma_oft_y1, rdma_oft_y1, 16, 0);/* [false alarm] */
		dma1->rdma_mask0 = set_bits32(dma1->rdma_mask0,
			(rdma_mask_y0 | (rdma_mask_x0 << 16)), 28, 0);/* [false alarm] */
		dma1->rdma_mask1 = set_bits32(dma1->rdma_mask1,
			(rdma_mask_y1 | (rdma_mask_x1 << 16)), 28, 0);/* [false alarm] */
		dma1->rdma_stretch_size_vrt = set_bits32(dma1->rdma_stretch_size_vrt,
			stretch_size_vrt, 16, 0);/* [false alarm] */
		dma1->rdma_stretch_line_num = set_bits32(dma1->rdma_stretch_line_num,
			(stretched_line_num | (stretched_stride << 6)), 25, 0);/* [false alarm] */
		/* FIXME: rdma_page_8k */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, (layer->src.is_tile ? 0x1 : 0x0), 1, 1);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, ((layer->need_cap & CAP_FBDC) ? 0x1 : 0x0), 1, 2);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, rdma_format + 1, 5, 3);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, (mmu_enable ? 0x1 : 0x0), 1, 8);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, rdma_transform, 3, 9);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, (*rdma_stretch_enable ? 0x1 : 0x0), 1, 12);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, l2t_interleave_n, 4, 13);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, (src_rect_mask_enable ? 0x1 : 0x0), 1, 17);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, rdma_rot_burst4, 1, 22);/* [false alarm] */
		dma1->rdma_ctrl = set_bits32(dma1->rdma_ctrl, 0x1, 1, 25);/* [false alarm] */
		dma1->rdma_data_num = set_bits32(dma1->rdma_data_num, rdma_data_num, 30, 0);/* [false alarm] */
		dma1->rdma_tile_scram = set_bits32(dma1->rdma_tile_scram, (layer->src.is_tile ? 0x1 : 0x0), 1, 0);/* [false alarm] */

		if (is_yuv_planar) {
			rdma_addr = hisi_calculate_display_addr(mmu_enable, layer, &aligned_rect, DSS_ADDR_PLANE2);
			rdma_stride = layer->src.vStride;/* [false alarm] */

			dma2->rdma_data_addr = set_bits32(dma2->rdma_data_addr, rdma_addr / DMA_ALIGN_BYTES, 28, 0);/* [false alarm] */
			dma2->rdma_oft_x0 = set_bits32(dma2->rdma_oft_x0, rdma_oft_x0, 12, 0);/* [false alarm] */
			dma2->rdma_oft_y0 = set_bits32(dma2->rdma_oft_y0, rdma_oft_y0, 16, 0);/* [false alarm] */
			dma2->rdma_stride = set_bits32(dma2->rdma_stride, rdma_stride / DMA_ALIGN_BYTES, 13, 0);/* [false alarm] */
			dma2->rdma_oft_x1 = set_bits32(dma2->rdma_oft_x1, rdma_oft_x1, 12, 0);/* [false alarm] */
			dma2->rdma_oft_y1 = set_bits32(dma2->rdma_oft_y1, rdma_oft_y1, 16, 0);/* [false alarm] */
			dma2->rdma_mask0 = set_bits32(dma2->rdma_mask0,
				(rdma_mask_y0 | (rdma_mask_x0 << 16)), 28, 0);/* [false alarm] */
			dma2->rdma_mask1 = set_bits32(dma2->rdma_mask1,
				(rdma_mask_y1 | (rdma_mask_x1 << 16)), 28, 0);/* [false alarm] */
			dma2->rdma_stretch_size_vrt = set_bits32(dma2->rdma_stretch_size_vrt,
				stretch_size_vrt, 16, 0);/* [false alarm] */
			dma2->rdma_stretch_line_num = set_bits32(dma2->rdma_stretch_line_num,
				(stretched_line_num | (stretched_stride << 6)), 25, 0);/* [false alarm] */
			/* FIXME: rdma_page_8k */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, (layer->src.is_tile ? 0x1 : 0x0), 1, 1);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, ((layer->need_cap & CAP_FBDC) ? 0x1 : 0x0), 1, 2);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, rdma_format + 2, 5, 3);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, (mmu_enable ? 0x1 : 0x0), 1, 8);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, rdma_transform, 3, 9);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, (*rdma_stretch_enable ? 0x1 : 0x0), 1, 12);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, l2t_interleave_n, 4, 13);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, (src_rect_mask_enable ? 0x1 : 0x0), 1, 17);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, rdma_rot_burst4, 1, 22);/* [false alarm] */
			dma2->rdma_ctrl = set_bits32(dma2->rdma_ctrl, 0x1, 1, 25);/* [false alarm] */
			dma2->rdma_data_num = set_bits32(dma2->rdma_data_num, rdma_data_num, 30, 0);/* [false alarm] */
			dma2->rdma_tile_scram = set_bits32(dma2->rdma_tile_scram, (layer->src.is_tile ? 0x1 : 0x0), 1, 0);/* [false alarm] */
		}
	}

	return 0;
}


/*******************************************************************************
** DSS FBC & FBDC
*/
static void hisi_dss_fbdc_init(char __iomem *fbdc_base, dss_fbdc_t *s_fbdc)
{
	BUG_ON(fbdc_base == NULL);
	BUG_ON(s_fbdc == NULL);

	memset(s_fbdc, 0, sizeof(dss_fbdc_t));

	s_fbdc->fbdc_data_addr = inp32(fbdc_base + FBDC_DATA_ADDR);
	s_fbdc->fbdc_data_stride = inp32(fbdc_base + FBDC_DATA_STRIDE);
	s_fbdc->fbdc_head_addr = inp32(fbdc_base + FBDC_HEAD_ADDR);
	s_fbdc->fbdc_head_stride = inp32(fbdc_base + FBDC_HEAD_STRIDE);
	s_fbdc->fbdc_size = inp32(fbdc_base + FBDC_SIZE);
	s_fbdc->fbdc_crop_s = inp32(fbdc_base + FBDC_CROP_S);
	s_fbdc->fbdc_ctrl= inp32(fbdc_base + FBDC_CTRL);
	s_fbdc->fbdc_pm_ctrl = inp32(fbdc_base + FBDC_PM_CTRL);
	s_fbdc->fbdc_src_size = inp32(fbdc_base + FBDC_SRC_SIZE);
}

static void hisi_dss_fbdc_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *fbdc_base, dss_fbdc_t *s_fbdc)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(fbdc_base == NULL);
	BUG_ON(s_fbdc == NULL);

	hisifd->set_reg(hisifd, fbdc_base + FBDC_DATA_ADDR, s_fbdc->fbdc_data_addr, 32, 0);
	hisifd->set_reg(hisifd, fbdc_base + FBDC_DATA_STRIDE, s_fbdc->fbdc_data_stride, 32, 0);
	hisifd->set_reg(hisifd, fbdc_base + FBDC_HEAD_ADDR, s_fbdc->fbdc_head_addr, 32, 0);
	hisifd->set_reg(hisifd, fbdc_base + FBDC_HEAD_STRIDE, s_fbdc->fbdc_head_stride, 32, 0);
	hisifd->set_reg(hisifd, fbdc_base + FBDC_SIZE, s_fbdc->fbdc_size, 32, 0);
	hisifd->set_reg(hisifd, fbdc_base + FBDC_CROP_S, s_fbdc->fbdc_crop_s, 32, 0);
	hisifd->set_reg(hisifd, fbdc_base + FBDC_CTRL, s_fbdc->fbdc_ctrl, 32, 0);
	hisifd->set_reg(hisifd, fbdc_base + FBDC_PM_CTRL, s_fbdc->fbdc_pm_ctrl, 32, 0);
	hisifd->set_reg(hisifd, fbdc_base + FBDC_SRC_SIZE, s_fbdc->fbdc_src_size, 32, 0);
}

int hisi_dss_fbdc_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_layer_t *layer, dss_rect_t aligned_rect)
{
	dss_fbdc_t *fbdc = NULL;
	int chn_idx = 0;
	uint32_t fbdc_src_width = 0;
	uint32_t fbdc_src_height = 0;
	uint32_t rdma_addr = 0;
	bool mmu_enable = false;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON(layer == NULL);

	if (!(layer->need_cap & CAP_FBDC))
		return 0;

	chn_idx = layer->chn_idx;
	mmu_enable = (layer->src.mmu_enable == 1) ? true : false;
	rdma_addr = mmu_enable ? layer->src.vir_addr : layer->src.phy_addr;

	fbdc = &(hisifd->dss_module.fbdc[chn_idx]);
	hisifd->dss_module.fbdc_used[chn_idx] = 1;

	if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
		fbdc_src_width = layer->src.height;
		fbdc_src_height = layer->src.width;
	} else {
		fbdc_src_width = layer->src.width;
		fbdc_src_height = layer->src.height;
	}

	fbdc->fbdc_data_addr = set_bits32(fbdc->fbdc_data_addr,
		(rdma_addr / DMA_ALIGN_BYTES), 28, 0);
	fbdc->fbdc_data_stride = set_bits32(fbdc->fbdc_data_stride,
		(layer->src.stride / DMA_ALIGN_BYTES), 18, 0);
	fbdc->fbdc_head_addr = set_bits32(fbdc->fbdc_head_addr,
		(pov_req->hdr_start_addr / DMA_ALIGN_BYTES), 28, 0);
	/* FIXME: head_stride */
	fbdc->fbdc_head_stride = set_bits32(fbdc->fbdc_head_stride,
		pov_req->hdr_stride, 18, 0);
	fbdc->fbdc_size = set_bits32(fbdc->fbdc_size, (DSS_HEIGHT(aligned_rect.h) |
		(DSS_WIDTH(aligned_rect.w) << 16)), 32, 0);
	fbdc->fbdc_crop_s = set_bits32(fbdc->fbdc_crop_s,
		(aligned_rect.y | (aligned_rect.x << 16)), 32, 0);
	fbdc->fbdc_ctrl = set_bits32(fbdc->fbdc_ctrl, layer->src.format, 5, 0);
	fbdc->fbdc_ctrl = set_bits32(fbdc->fbdc_ctrl, 0x1, 1, 5);
	fbdc->fbdc_ctrl = set_bits32(fbdc->fbdc_ctrl, 0x1, 1, 7);
	fbdc->fbdc_pm_ctrl = set_bits32(fbdc->fbdc_pm_ctrl, 0x601AA, 19, 0);
	fbdc->fbdc_src_size = set_bits32(fbdc->fbdc_src_size, ((DSS_HEIGHT(fbdc_src_height) |
		(DSS_WIDTH(fbdc_src_width) << 16))), 32, 0);

	return 0;
}

static void hisi_dss_fbc_init(char __iomem *fbc_base, dss_fbc_t *s_fbc)
{
	BUG_ON(fbc_base == NULL);
	BUG_ON(s_fbc == NULL);

	memset(s_fbc, 0, sizeof(dss_fbc_t));

	s_fbc->fbc_size = inp32(fbc_base + FBC_SIZE);
	s_fbc->fbc_ctrl = inp32(fbc_base + FBC_CTRL);
	s_fbc->fbc_en = inp32(fbc_base + FBC_EN);
	s_fbc->fbc_pm_ctrl = inp32(fbc_base + FBC_PM_CTRL);
	s_fbc->fbc_len = inp32(fbc_base + FBC_LEN);
}

void hisi_dss_fbc_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *fbc_base, dss_fbc_t *s_fbc)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(fbc_base == NULL);
	BUG_ON(s_fbc == NULL);

	hisifd->set_reg(hisifd, fbc_base + FBC_SIZE, s_fbc->fbc_size, 32, 0);
	hisifd->set_reg(hisifd, fbc_base + FBC_CTRL, s_fbc->fbc_ctrl, 32, 0);
	hisifd->set_reg(hisifd, fbc_base + FBC_EN, s_fbc->fbc_en, 32, 0);
	hisifd->set_reg(hisifd, fbc_base + FBC_PM_CTRL, s_fbc->fbc_pm_ctrl, 32, 0);
	hisifd->set_reg(hisifd, fbc_base + FBC_LEN, s_fbc->fbc_len, 32, 0);
}

int hisi_dss_fbc_config(struct hisi_fb_data_type *hisifd, dss_wb_layer_t *layer,
	dss_rect_t aligned_rect)
{
	dss_fbc_t *fbc = NULL;
	int chn_idx = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	if (!(layer->need_cap & CAP_FBC))
		return 0;

	chn_idx = layer->chn_idx;

	fbc = &(hisifd->dss_module.fbc[chn_idx]);
	hisifd->dss_module.fbc_used[chn_idx] = 1;

	fbc->fbc_size = set_bits32(fbc->fbc_size,
		(DSS_HEIGHT(aligned_rect.h) | (DSS_WIDTH(aligned_rect.w) << 16)), 32, 0);
	fbc->fbc_ctrl = set_bits32(fbc->fbc_ctrl, (layer->dst.format | (FBC_ARITH_SEL0 << 5) |
		(FBC_BLOCK_128_PIXELS << 7)), 23, 0);
	fbc->fbc_en = set_bits32(fbc->fbc_en, (0x1 | (0x1 << 1)), 2, 0);
	fbc->fbc_pm_ctrl = set_bits32(fbc->fbc_pm_ctrl, 0x81AA, 17, 0);
	fbc->fbc_len = set_bits32(fbc->fbc_len, (0x10 | (0x10 << 5)), 10, 0);

	return 0;
}


/*******************************************************************************
** DSS DFC
*/
static void hisi_dss_dfc_init(char __iomem *dfc_base, dss_dfc_t *s_dfc)
{
	BUG_ON(dfc_base == NULL);
	BUG_ON(s_dfc == NULL);

	memset(s_dfc, 0, sizeof(dss_dfc_t));

	s_dfc->disp_size = inp32(dfc_base + DFC_DISP_SIZE);
	s_dfc->pix_in_num = inp32(dfc_base + DFC_PIX_IN_NUM);
	s_dfc->disp_fmt = inp32(dfc_base + DFC_DISP_FMT);
	s_dfc->clip_ctl_hrz = inp32(dfc_base + DFC_CLIP_CTL_HRZ);
	s_dfc->clip_ctl_vrz = inp32(dfc_base + DFC_CLIP_CTL_VRZ);
	s_dfc->ctl_clip_en = inp32(dfc_base + DFC_CTL_CLIP_EN);
	s_dfc->icg_module = inp32(dfc_base + DFC_ICG_MODULE);
	s_dfc->dither_enable = inp32(dfc_base + DFC_DITHER_ENABLE);
	s_dfc->padding_ctl = inp32(dfc_base + DFC_PADDING_CTL);
}

static void hisi_dss_dfc_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *dfc_base, dss_dfc_t *s_dfc)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(dfc_base == NULL);
	BUG_ON(s_dfc == NULL);

	hisifd->set_reg(hisifd, dfc_base + DFC_DISP_SIZE, s_dfc->disp_size, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_PIX_IN_NUM, s_dfc->pix_in_num, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_DISP_FMT, s_dfc->disp_fmt, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_CLIP_CTL_HRZ, s_dfc->clip_ctl_hrz, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_CLIP_CTL_VRZ, s_dfc->clip_ctl_vrz, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_CTL_CLIP_EN, s_dfc->ctl_clip_en, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_ICG_MODULE, s_dfc->icg_module, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_DITHER_ENABLE, s_dfc->dither_enable, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_PADDING_CTL, s_dfc->padding_ctl, 32, 0);
}

int hisi_dss_rdfc_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer,
	dss_rect_t *aligned_rect, dss_rect_ltrb_t clip_rect)
{
	dss_dfc_t *dfc = NULL;
	int chn_idx = 0;
	int dfc_fmt = 0;
	int dfc_bpp = 0;
	int dfc_pix_in_num = 0;
	int dfc_aligned = 0;
	int size_hrz = 0;
	int size_vrt = 0;
	int dfc_hrz_clip = 0;
	bool need_clip = false;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	chn_idx = layer->chn_idx;

	dfc = &(hisifd->dss_module.dfc[chn_idx]);
	hisifd->dss_module.dfc_used[chn_idx] = 1;

	dfc_fmt = hisi_pixel_format_hal2dfc(layer->src.format, layer->transform);
	if (dfc_fmt < 0) {
		HISI_FB_ERR("layer format (%d) not support !\n", layer->src.format);
		return -EINVAL;
	}

	dfc_bpp = hisi_dfc_get_bpp(dfc_fmt);
	if (dfc_bpp <= 0) {
		HISI_FB_ERR("dfc_bpp(%d) not support !\n", dfc_bpp);
		return -EINVAL;
	}

	dfc_pix_in_num = (dfc_bpp <= 2) ? 0x1 : 0x0;
	dfc_aligned = (dfc_bpp <= 2) ? 4 : 2;

	need_clip = isNeedRectClip(clip_rect);

	size_hrz = DSS_WIDTH(aligned_rect->w);
	size_vrt = DSS_HEIGHT(aligned_rect->h);

	if (((size_hrz + 1) % dfc_aligned) != 0) {
		size_hrz -= 1;
		HISI_FB_ERR("SIZE_HRT=%d mismatch!bpp=%d\n", size_hrz, layer->src.bpp);

		HISI_FB_ERR("layer_idx%d, format=%d, transform=%d, "
			"original_src_rect(%d,%d,%d,%d), rdma_out_rect(%d,%d,%d,%d), dst_rect(%d,%d,%d,%d)!\n",
			layer->layer_idx, layer->src.format, layer->transform,
			layer->src_rect.x, layer->src_rect.y, layer->src_rect.w, layer->src_rect.h,
			aligned_rect->x, aligned_rect->y, aligned_rect->w, aligned_rect->h,
			layer->dst_rect.x, layer->dst_rect.y, layer->dst_rect.w, layer->dst_rect.h);
	}

	dfc_hrz_clip = (size_hrz + 1) % dfc_aligned;
	if (dfc_hrz_clip) {
		clip_rect.right += dfc_hrz_clip;
		size_hrz += dfc_hrz_clip;
		need_clip = true;
	}

	dfc->disp_size = set_bits32(dfc->disp_size, (size_vrt | (size_hrz << 13)), 26, 0);
	dfc->pix_in_num = set_bits32(dfc->pix_in_num, dfc_pix_in_num, 1, 0);
	dfc->disp_fmt = set_bits32(dfc->disp_fmt, dfc_fmt, 5, 1);
	if (need_clip) {
		dfc->clip_ctl_hrz = set_bits32(dfc->clip_ctl_hrz,
			(clip_rect.right | (clip_rect.left << 6)), 12, 0);
		dfc->clip_ctl_vrz = set_bits32(dfc->clip_ctl_vrz,
			(clip_rect.bottom | (clip_rect.top << 6)), 12, 0);
		dfc->ctl_clip_en = set_bits32(dfc->ctl_clip_en, 0x1, 1, 0);
	} else {
		dfc->clip_ctl_hrz = set_bits32(dfc->clip_ctl_hrz, 0x0, 12, 0);
		dfc->clip_ctl_vrz = set_bits32(dfc->clip_ctl_vrz, 0x0, 12, 0);
		dfc->ctl_clip_en = set_bits32(dfc->ctl_clip_en, 0x0, 1, 0);
	}
	dfc->icg_module = set_bits32(dfc->icg_module, 0x1, 1, 2);
	dfc->dither_enable = set_bits32(dfc->dither_enable, 0x0, 1, 0);
	dfc->padding_ctl = set_bits32(dfc->padding_ctl, 0x0, 17, 0);

	//update
	if (need_clip) {
		aligned_rect->w -= (clip_rect.left + clip_rect.right);
		aligned_rect->h -= (clip_rect.top + clip_rect.bottom);
	}

	return 0;
}

int hisi_dss_wdfc_config(struct hisi_fb_data_type *hisifd, dss_wb_layer_t *layer,
	dss_rect_t *aligned_rect, dss_rect_t *block_rect)
{
	dss_dfc_t *dfc = NULL;
	int chn_idx = 0;
	dss_rect_t in_rect;
	bool need_dither = false;

	int size_hrz = 0;
	int size_vrt = 0;
	int dfc_fmt = 0;
	int dfc_pix_in_num = 0;
	uint32_t dfc_w = 0;

	uint32_t left_pad = 0;
	uint32_t right_pad = 0;
	uint32_t top_pad = 0;
	uint32_t bottom_pad = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);
	BUG_ON(aligned_rect == NULL);

	chn_idx = layer->chn_idx;

	dfc = &(hisifd->dss_module.dfc[chn_idx]);
	hisifd->dss_module.dfc_used[chn_idx] = 1;

	dfc_fmt = hisi_pixel_format_hal2dfc(layer->dst.format, 0);
	if (dfc_fmt < 0) {
		HISI_FB_ERR("layer format (%d) not support !\n", layer->dst.format);
		return -EINVAL;
	}

	need_dither = isNeedDither(dfc_fmt);
	if (block_rect) {
		memcpy(&in_rect, block_rect, sizeof(dss_rect_t));
	} else {
		in_rect = layer->dst_rect;
	}

	size_hrz = DSS_WIDTH(in_rect.w);
	size_vrt = DSS_HEIGHT(in_rect.h);

	/* for wdfc */
	if ((size_hrz + 1) % 2 == 1) {
		size_hrz += 1;
		dfc_w = 1;
	}
	dfc_pix_in_num = 0x0;

	aligned_rect->x = ALIGN_DOWN(in_rect.x * layer->dst.bpp,
		DMA_ALIGN_BYTES) / layer->dst.bpp;
	aligned_rect->y = in_rect.y;
	aligned_rect->w = ALIGN_UP((in_rect.w + in_rect.x - aligned_rect->x + dfc_w) *
		layer->dst.bpp , DMA_ALIGN_BYTES) / layer->dst.bpp;
	aligned_rect->h = in_rect.h;

	left_pad = in_rect.x - aligned_rect->x;
	right_pad = aligned_rect->w - in_rect.w - left_pad - dfc_w;
	top_pad = 0;
	bottom_pad = 0;

	dfc->disp_size = set_bits32(dfc->disp_size, (size_vrt | (size_hrz << 13)), 26, 0);
	dfc->pix_in_num = set_bits32(dfc->pix_in_num, dfc_pix_in_num, 1, 0);
	dfc->disp_fmt = set_bits32(dfc->disp_fmt, dfc_fmt, 5, 1);
	dfc->clip_ctl_hrz = set_bits32(dfc->clip_ctl_hrz, 0x0, 12, 0);
	dfc->clip_ctl_vrz = set_bits32(dfc->clip_ctl_vrz, 0x0, 12, 0);
	dfc->ctl_clip_en = set_bits32(dfc->ctl_clip_en, 0x0, 1, 0);
	dfc->icg_module = set_bits32(dfc->icg_module, 0x1, 1, 2);
	if (need_dither) {
		dfc->dither_enable = set_bits32(dfc->dither_enable, 0x1, 1, 0);
	} else {
		dfc->dither_enable = set_bits32(dfc->dither_enable, 0x0, 1, 0);
	}

	if (left_pad || right_pad || top_pad || bottom_pad) {
		dfc->padding_ctl = set_bits32(dfc->padding_ctl, (0x1 | (left_pad << 1) |
			(right_pad << 5) | (top_pad << 9) | (bottom_pad << 13)), 17, 0);
	} else {
		dfc->padding_ctl = set_bits32(dfc->padding_ctl, 0x0, 17, 0);
	}

	return 0;
}


/*******************************************************************************
** DSS SCF
*/

/* Filter coefficients for SCF */
#define PHASE_NUM	(66)
#define TAP4	(4)
#define TAP5	(5)
#define TAP6	(6)

static const int H0_Y_COEF[PHASE_NUM][TAP6] = {
	/* Row 0~32: coefficients for scale down */
	{   2,  264,  500, 264,   2,  -8},
	{   2,  257,  499, 268,   6,  -8},
	{   1,  252,  498, 274,   8,  -9},
	{  -1,  246,  498, 281,   9,  -9},
	{  -2,  241,  497, 286,  12, -10},
	{  -3,  235,  497, 292,  13, -10},
	{  -5,  230,  496, 298,  15, -10},
	{  -6,  225,  495, 303,  18, -11},
	{  -7,  219,  494, 309,  20, -11},
	{  -7,  213,  493, 314,  23, -12},
	{  -9,  208,  491, 320,  26, -12},
	{ -10,  203,  490, 325,  28, -12},
	{ -10,  197,  488, 331,  31, -13},
	{ -10,  192,  486, 336,  33, -13},
	{ -12,  186,  485, 342,  36, -13},
	{ -12,  181,  482, 347,  39, -13},
	{ -13,  176,  480, 352,  42, -13},
	{ -14,  171,  478, 358,  45, -14},
	{ -14,  166,  476, 363,  48, -15},
	{ -14,  160,  473, 368,  52, -15},
	{ -14,  155,  470, 373,  55, -15},
	{ -15,  150,  467, 378,  59, -15},
	{ -15,  145,  464, 383,  62, -15},
	{ -16,  141,  461, 388,  65, -15},
	{ -16,  136,  458, 393,  68, -15},
	{ -16,  131,  455, 398,  72, -16},
	{ -16,  126,  451, 402,  77, -16},
	{ -16,  122,  448, 407,  79, -16},
	{ -16,  117,  444, 411,  84, -16},
	{ -17,  113,  441, 416,  87, -16},
	{ -17,  108,  437, 420,  92, -16},
	{ -17,  104,  433, 424,  96, -16},
	{ -17,  100,  429, 429, 100, -17},

	/* Row 33~65: coefficients for scale up */
	{-187,  105, 1186, 105, -187,   2},
	{-182,   86, 1186, 124, -192,   2},
	{-176,   67, 1185, 143, -197,   2},
	{-170,   49, 1182, 163, -202,   2},
	{-166,   32, 1180, 184, -207,   1},
	{-160,   15, 1176, 204, -212,   1},
	{-155,   -2, 1171, 225, -216,   1},
	{-149,  -18, 1166, 246, -221,   0},
	{-145,  -34, 1160, 268, -225,   0},
	{-139,  -49, 1153, 290, -230,  -1},
	{-134,  -63, 1145, 312, -234,  -2},
	{-129,  -78, 1137, 334, -238,  -2},
	{-124,  -91, 1128, 357, -241,  -5},
	{-119, -104, 1118, 379, -245,  -5},
	{-114, -117, 1107, 402, -248,  -6},
	{-109, -129, 1096, 425, -251,  -8},
	{-104, -141, 1083, 448, -254,  -8},
	{-100, -152, 1071, 471, -257,  -9},
	{ -95, -162, 1057, 494, -259, -11},
	{ -90, -172, 1043, 517, -261, -13},
	{ -86, -181, 1028, 540, -263, -14},
	{ -82, -190, 1013, 563, -264, -16},
	{ -77, -199,  997, 586, -265, -18},
	{ -73, -207,  980, 609, -266, -19},
	{ -69, -214,  963, 632, -266, -22},
	{ -65, -221,  945, 655, -266, -24},
	{ -62, -227,  927, 678, -266, -26},
	{ -58, -233,  908, 700, -265, -28},
	{ -54, -238,  889, 722, -264, -31},
	{ -51, -243,  870, 744, -262, -34},
	{ -48, -247,  850, 766, -260, -37},
	{ -45, -251,  829, 787, -257, -39},
	{ -42, -255,  809, 809, -255, -42}
};

static const int Y_COEF[PHASE_NUM][TAP5] = {
	/* Row 0~32: coefficients for scale down */
	{  98, 415, 415,  98, -2},
	{  95, 412, 418, 103, -4},
	{  91, 408, 422, 107, -4},
	{  87, 404, 426, 111, -4},
	{  84, 399, 430, 115, -4},
	{  80, 395, 434, 119, -4},
	{  76, 390, 438, 124, -4},
	{  73, 386, 440, 128, -3},
	{  70, 381, 444, 132, -3},
	{  66, 376, 448, 137, -3},
	{  63, 371, 451, 142, -3},
	{  60, 366, 455, 146, -3},
	{  57, 361, 457, 151, -2},
	{  54, 356, 460, 156, -2},
	{  51, 351, 463, 161, -2},
	{  49, 346, 465, 165, -1},
	{  46, 341, 468, 170, -1},
	{  43, 336, 470, 175, 0},
	{  41, 331, 472, 180, 0},
	{  38, 325, 474, 186, 1},
	{  36, 320, 476, 191, 1},
	{  34, 315, 477, 196, 2},
	{  32, 309, 479, 201, 3},
	{  29, 304, 481, 206, 4},
	{  27, 299, 481, 212, 5},
	{  26, 293, 482, 217, 6},
	{  24, 288, 484, 222, 6},
	{  22, 282, 484, 228, 8},
	{  20, 277, 485, 233, 9},
	{  19, 271, 485, 238, 11},
	{  17, 266, 485, 244, 12},
	{  16, 260, 485, 250, 13},
	{  14, 255, 486, 255, 14},
	/* Row 33~65: coefficients for scale up */
	{ -94, 608, 608, -94, -4},
	{ -94, 594, 619, -91, -4},
	{ -96, 579, 635, -89, -5},
	{ -96, 563, 650, -87, -6},
	{ -97, 548, 665, -85, -7},
	{ -97, 532, 678, -82, -7},
	{ -98, 516, 693, -79, -8},
	{ -97, 500, 705, -75, -9},
	{ -97, 484, 720, -72, -11},
	{ -97, 468, 733, -68, -12},
	{ -96, 452, 744, -63, -13},
	{ -95, 436, 755, -58, -14},
	{ -94, 419, 768, -53, -16},
	{ -93, 403, 779, -48, -17},
	{ -92, 387, 789, -42, -18},
	{ -90, 371, 799, -36, -20},
	{ -89, 355, 809, -29, -22},
	{ -87, 339, 817, -22, -23},
	{ -86, 324, 826, -15, -25},
	{ -84, 308, 835,  -8, -27},
	{ -82, 293, 842,   0, -29},
	{ -80, 277, 849,   9, -31},
	{ -78, 262, 855,  18, -33},
	{ -75, 247, 860,  27, -35},
	{ -73, 233, 865,  36, -37},
	{ -71, 218, 870,  46, -39},
	{ -69, 204, 874,  56, -41},
	{ -66, 190, 876,  67, -43},
	{ -64, 176, 879,  78, -45},
	{ -62, 163, 882,  89, -48},
	{ -59, 150, 883, 100, -50},
	{ -57, 137, 883, 112, -51},
	{ -55, 125, 884, 125, -55}
};

static const int UV_COEF[PHASE_NUM][TAP4] = {
	{214, 599, 214, -3},
	{207, 597, 223, -3},
	{200, 596, 231, -3},
	{193, 596, 238, -3},
	{186, 595, 246, -3},
	{178, 594, 255, -3},
	{171, 593, 263, -3},
	{165, 591, 271, -3},
	{158, 589, 279, -2},
	{151, 587, 288, -2},
	{145, 584, 296, -1},
	{139, 582, 304, -1},
	{133, 578, 312, 1},
	{127, 575, 321, 1},
	{121, 572, 329, 2},
	{115, 568, 337, 4},
	{109, 564, 346, 5},
	{104, 560, 354, 6},
	{ 98, 555, 362, 9},
	{ 94, 550, 370, 10},
	{ 88, 546, 379, 11},
	{ 84, 540, 387, 13},
	{ 79, 535, 395, 15},
	{ 74, 530, 403, 17},
	{ 70, 524, 411, 19},
	{ 66, 518, 419, 21},
	{ 62, 512, 427, 23},
	{ 57, 506, 435, 26},
	{ 54, 499, 443, 28},
	{ 50, 492, 451, 31},
	{ 47, 486, 457, 34},
	{ 43, 479, 465, 37},
	{ 40, 472, 472, 40},
	{214, 599, 214, -3},
	{207, 597, 223, -3},
	{200, 596, 231, -3},
	{193, 596, 238, -3},
	{186, 595, 246, -3},
	{178, 594, 255, -3},
	{171, 593, 263, -3},
	{165, 591, 271, -3},
	{158, 589, 279, -2},
	{151, 587, 288, -2},
	{145, 584, 296, -1},
	{139, 582, 304, -1},
	{133, 578, 312, 1},
	{127, 575, 321, 1},
	{121, 572, 329, 2},
	{115, 568, 337, 4},
	{109, 564, 346, 5},
	{104, 560, 354, 6},
	{ 98, 555, 362, 9},
	{ 94, 550, 370, 10},
	{ 88, 546, 379, 11},
	{ 84, 540, 387, 13},
	{ 79, 535, 395, 15},
	{ 74, 530, 403, 17},
	{ 70, 524, 411, 19},
	{ 66, 518, 419, 21},
	{ 62, 512, 427, 23},
	{ 57, 506, 435, 26},
	{ 54, 499, 443, 28},
	{ 50, 492, 451, 31},
	{ 47, 486, 457, 34},
	{ 43, 479, 465, 37},
	{ 40, 472, 472, 40}
};

static void hisi_dss_scf_init(char __iomem *scf_base, dss_scf_t *s_scf)
{
	BUG_ON(scf_base == NULL);
	BUG_ON(s_scf == NULL);

	memset(s_scf, 0, sizeof(dss_scf_t));

	s_scf->en_hscl_str = inp32(scf_base + SCF_EN_HSCL_STR);
	s_scf->en_vscl_str = inp32(scf_base + SCF_EN_VSCL_STR);
	s_scf->h_v_order = inp32(scf_base + SCF_H_V_ORDER);
	s_scf->input_width_height = inp32(scf_base + SCF_INPUT_WIDTH_HEIGHT);
	s_scf->output_width_heigth = inp32(scf_base + SCF_OUTPUT_WIDTH_HEIGTH);
	s_scf->en_hscl = inp32(scf_base + SCF_EN_HSCL);
	s_scf->en_vscl = inp32(scf_base + SCF_EN_VSCL);
	s_scf->acc_hscl = inp32(scf_base + SCF_ACC_HSCL);
	s_scf->inc_hscl = inp32(scf_base + SCF_INC_HSCL);
	s_scf->inc_vscl = inp32(scf_base + SCF_INC_VSCL);
}

static void hisi_dss_scf_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *scf_base, dss_scf_t *s_scf)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(scf_base == NULL);
	BUG_ON(s_scf == NULL);

	hisifd->set_reg(hisifd, scf_base + SCF_EN_HSCL_STR, s_scf->en_hscl_str, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_EN_VSCL_STR, s_scf->en_vscl_str, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_H_V_ORDER, s_scf->h_v_order, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_INPUT_WIDTH_HEIGHT, s_scf->input_width_height, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_OUTPUT_WIDTH_HEIGTH, s_scf->output_width_heigth, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_EN_HSCL, s_scf->en_hscl, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_EN_VSCL, s_scf->en_vscl, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_ACC_HSCL, s_scf->acc_hscl, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_INC_HSCL, s_scf->inc_hscl, 32, 0);
	hisifd->set_reg(hisifd, scf_base + SCF_INC_VSCL, s_scf->inc_vscl, 32, 0);
}

int hisi_dss_scf_write_coefs(struct hisi_fb_data_type *hisifd,
	char __iomem *addr, const int **p, int row, int col)
{
	int groups[3] = {0};
	int offset = 0;
	int valid_num = 0;
	int i= 0;
	int j = 0;
	int k = 0;

	BUG_ON(addr == NULL);

	if ((row != PHASE_NUM) || (col < TAP4 || col > TAP6)) {
		HISI_FB_ERR("SCF filter coefficients is err, phase_num = %d, tap_num = %d\n", row, col);
		return -EINVAL;
	}

	/*byte*/
	offset = (col == TAP4) ? 8 : 16;
	valid_num = (offset == 16) ? 3 : 2;

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j += 2) {
			if ((col % 2) && (j == col -1)) {
				groups[j / 2] = (*((int*)p + i * col + j) & 0xFFF) | (0 << 16);
			} else {
				groups[j / 2] = (*((int*)p + i * col + j) & 0xFFF) | (*((int*)p + i * col + j + 1) << 16);
			}
		}

		for (k = 0; k < valid_num; k++) {
			hisifd->set_reg(hisifd, addr + offset * i + k * sizeof(int), groups[k], 32, 0);
			groups[k] = 0;
		}
	}

	return 0;
}

int hisi_dss_scf_load_filter_coef(struct hisi_fb_data_type *hisifd, char __iomem *scf_base)
{
	char __iomem *h0_y_addr = NULL;
	char __iomem *y_addr = NULL;
	char __iomem *uv_addr = NULL;
	int ret = 0;

	BUG_ON(scf_base == NULL);

	h0_y_addr = scf_base + DSS_SCF_H0_Y_COEF_OFFSET;
	y_addr = scf_base + DSS_SCF_Y_COEF_OFFSET;
	uv_addr = scf_base + DSS_SCF_UV_COEF_OFFSET;

	ret = hisi_dss_scf_write_coefs(hisifd, h0_y_addr, (const int **)H0_Y_COEF, PHASE_NUM, TAP6);
	if (ret < 0) {
		HISI_FB_ERR("Error to write H0_Y_COEF coefficients.\n");
	}

	ret = hisi_dss_scf_write_coefs(hisifd, y_addr, (const int **)Y_COEF, PHASE_NUM, TAP5);
	if (ret < 0) {
		HISI_FB_ERR("Error to write Y_COEF coefficients.\n");
	}

	ret = hisi_dss_scf_write_coefs(hisifd, uv_addr, (const int **)UV_COEF, PHASE_NUM, TAP4);
	if (ret < 0) {
		HISI_FB_ERR("Error to write UV_COEF coefficients.\n");
	}

	return ret;
}

int hisi_dss_scf_coef_load(struct hisi_fb_data_type *hisifd)
{
	char __iomem *temp_base = NULL;
	int prev_refcount = 0;

	BUG_ON(hisifd == NULL);

	if (hisifd->ovl_type == DSS_OVL_ADP) {
		/*SCF4*/
		temp_base = hisifd->dss_base +
			g_dss_module_base[DPE3_CHN0][MODULE_SCF];
		hisi_dss_scf_load_filter_coef(hisifd, temp_base);
		/*SCF3*/
		temp_base = hisifd->dss_base +
			g_dss_module_base[DPE3_CHN1][MODULE_SCF];
		hisi_dss_scf_load_filter_coef(hisifd, temp_base);
	}

	if ((hisifd->ovl_type == DSS_OVL_PDP) ||(hisifd->ovl_type == DSS_OVL_SDP) ) {
		down(&hisi_fb_dss_scf1_sem);
		prev_refcount = g_scf1_coef_load_refcount++;
		if (!prev_refcount) {
			/*SCF0*/
			temp_base = hisifd->dss_base +
				g_dss_module_base[DPE0_CHN0][MODULE_SCF];
			hisi_dss_scf_load_filter_coef(hisifd, temp_base);

			/*SCF1*/
			temp_base = hisifd->dss_base +
				g_dss_module_base[DPE1_CHN0][MODULE_SCF];
			hisi_dss_scf_load_filter_coef(hisifd, temp_base);
		}
		up(&hisi_fb_dss_scf1_sem);
	}

	down(&hisi_fb_dss_scf2_sem);
	prev_refcount = g_scf2_coef_load_refcount++;
	if (!prev_refcount) {
		/*SCF2*/
		temp_base = hisifd->dss_base +
			g_dss_module_base[DPE2_CHN0][MODULE_SCF];
		hisi_dss_scf_load_filter_coef(hisifd, temp_base);
	}
	up(&hisi_fb_dss_scf2_sem);

	return 0;
}

int hisi_dss_scf_coef_unload(struct hisi_fb_data_type *hisifd)
{
	int new_refcount = 0;

	BUG_ON(hisifd == NULL);

	if ((hisifd->ovl_type == DSS_OVL_PDP) ||(hisifd->ovl_type == DSS_OVL_SDP) ) {
		down(&hisi_fb_dss_scf1_sem);
		new_refcount = --g_scf1_coef_load_refcount;
		if (new_refcount < 0) {
			HISI_FB_ERR("scf1_coef_load_refcount error!\n");
		}
		up(&hisi_fb_dss_scf1_sem);
	}

	down(&hisi_fb_dss_scf2_sem);
	new_refcount = --g_scf2_coef_load_refcount;
	if (new_refcount < 0) {
		HISI_FB_ERR("scf2_coef_load_refcount error!\n");
	}
	up(&hisi_fb_dss_scf2_sem);

	return 0;
}

int hisi_dss_scf_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer,
	dss_wb_layer_t *wb_layer, dss_rect_t *aligned_rect, bool rdma_stretch_enable)
{
	dss_scf_t *scf = NULL;
	int scf_idx = 0;
	dss_rect_t src_rect;
	dss_rect_t dst_rect;
	uint32_t need_cap = 0;
	int chn_idx = 0;
	uint32_t transform = 0;
	dss_block_info_t *pblock_info = NULL;

	bool en_hscl = false;
	bool en_vscl = false;
	uint32_t h_ratio = 0;
	uint32_t v_ratio = 0;
	uint32_t h_v_order = 0;
	uint32_t acc_hscl = 0;
	uint32_t acc_vscl = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON((layer == NULL) && (wb_layer == NULL));
	BUG_ON(layer && wb_layer);

	if (wb_layer) {
		need_cap = wb_layer->need_cap;
		chn_idx = wb_layer->chn_idx;
		transform = wb_layer->transform;
		if (aligned_rect)
			src_rect = *aligned_rect;
		else
			src_rect = wb_layer->src_rect;
		dst_rect = wb_layer->dst_rect;
	} else {
		need_cap = layer->need_cap;
		chn_idx = layer->chn_idx;
		transform = layer->transform;
		if (aligned_rect)
			src_rect = *aligned_rect;
		else
			src_rect = layer->src_rect;
		dst_rect = layer->dst_rect;
		pblock_info = &(layer->block_info);
	}

	if (!(need_cap & CAP_SCL))
		return 0;

	do {
		if (src_rect.w == dst_rect.w)
			break;

		en_hscl = true;

		if (pblock_info &&
			(pblock_info->h_ratio != 0) &&
			(pblock_info->h_ratio != SCF_CONST_FACTOR)) {
			h_ratio = pblock_info->h_ratio;
			break;
		}

		h_ratio = DSS_WIDTH(src_rect.w) * SCF_CONST_FACTOR / DSS_WIDTH(dst_rect.w);
		if ((dst_rect.w > (src_rect.w * SCF_UPSCALE_MAX))
			|| (src_rect.w > (dst_rect.w * SCF_DOWNSCALE_MAX))) {
			HISI_FB_ERR("width out of range, original_src_rec(%d, %d, %d, %d) "
				"new_src_rect(%d, %d, %d, %d), dst_rect(%d, %d, %d, %d), rdma_stretch_enable=%d\n",
				layer->src_rect.x, layer->src_rect.y, layer->src_rect.w, layer->src_rect.h,
				src_rect.x, src_rect.y, src_rect.w, src_rect.h,
				dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h, rdma_stretch_enable);

			return -EINVAL;
		}
	} while(0);

	do {
		if (src_rect.h == dst_rect.h)
			break;

		en_vscl = true;
		if (pblock_info &&
			(pblock_info->v_ratio != 0) &&
			(pblock_info->v_ratio != SCF_CONST_FACTOR)) {
			v_ratio = pblock_info->v_ratio;
			break;
		}

		v_ratio = (DSS_HEIGHT(src_rect.h) * SCF_CONST_FACTOR + SCF_CONST_FACTOR / 2 - acc_vscl) /
			DSS_HEIGHT(dst_rect.h);
		if ((dst_rect.h > (src_rect.h * SCF_UPSCALE_MAX))
			|| (src_rect.h > (dst_rect.h * SCF_DOWNSCALE_MAX))) {
			HISI_FB_ERR("height out of range, original_src_rec(%d, %d, %d, %d) "
				"new_src_rect(%d, %d, %d, %d), dst_rect(%d, %d, %d, %d), rdma_stretch_enable=%d.\n",
				layer->src_rect.x, layer->src_rect.y, layer->src_rect.w, layer->src_rect.h,
				src_rect.x, src_rect.y, src_rect.w, src_rect.h,
				dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h, rdma_stretch_enable);
			return -EINVAL;
		}
	} while(0);

	if (!en_hscl && !en_vscl)
		return 0;

	/* scale down, do hscl first; scale up, do vscl first*/
	if (pblock_info && ((pblock_info->h_ratio != 0) || (pblock_info->v_ratio != 0))) {
		h_v_order = pblock_info->h_v_order;
	} else {
		h_v_order = (src_rect.w > dst_rect.w) ? 0 : 1;
	}

	if (pblock_info && (pblock_info->acc_hscl != 0)) {
		acc_hscl = pblock_info->acc_hscl;
	}

	scf_idx = hisi_get_scf_index(chn_idx);
	if (scf_idx < 0) {
		HISI_FB_ERR("chn_idx(%d) hisi_get_scf_index failed!\n", chn_idx);
		return -EINVAL;
	}

	scf = &(hisifd->dss_module.scf[scf_idx]);
	hisifd->dss_module.scf_used[scf_idx] = 1;

	//if (DSS_WIDTH(src_rect.w) * 2 >= DSS_WIDTH(dst_rect.w)) {
	if (h_ratio >= 2 * SCF_CONST_FACTOR) {
		scf->en_hscl_str = set_bits32(scf->en_hscl_str, 0x1, 1, 0);
	} else {
		scf->en_hscl_str = set_bits32(scf->en_hscl_str, 0x0, 1, 0);
	}

	//if (DSS_HEIGHT(src_rect.h) * 2 >= DSS_HEIGHT(dst_rect.h)) {
	if (v_ratio >= 2 * SCF_CONST_FACTOR) {
		scf->en_vscl_str = set_bits32(scf->en_vscl_str, 0x1, 1, 0);
	} else {
		scf->en_vscl_str = set_bits32(scf->en_vscl_str, 0x0, 1, 0);
	}

	scf->h_v_order = set_bits32(scf->h_v_order, h_v_order, 1, 0);
	scf->input_width_height = set_bits32(scf->input_width_height,
		DSS_HEIGHT(src_rect.h), 13, 0);
	scf->input_width_height = set_bits32(scf->input_width_height,
		DSS_WIDTH(src_rect.w), 13, 16);
	scf->output_width_heigth = set_bits32(scf->output_width_heigth,
		DSS_HEIGHT(dst_rect.h), 13, 0);
	scf->output_width_heigth = set_bits32(scf->output_width_heigth,
		DSS_WIDTH(dst_rect.w), 13, 16);
	scf->en_hscl = set_bits32(scf->en_hscl, (en_hscl ? 0x1 : 0x0), 1, 0);
	scf->en_vscl = set_bits32(scf->en_vscl, (en_vscl ? 0x1 : 0x0), 1, 0);
	scf->acc_hscl = set_bits32(scf->acc_hscl, acc_hscl, 31, 0);
	scf->inc_hscl = set_bits32(scf->inc_hscl, h_ratio, 24, 0);
	scf->inc_vscl = set_bits32(scf->inc_vscl, v_ratio, 24, 0);

	return 0;
}

static void hisi_dss_scp_init(char __iomem *scp_base, dss_scp_t *s_scp)
{
	BUG_ON(scp_base == NULL);
	BUG_ON(s_scp == NULL);

	memset(s_scp, 0, sizeof(dss_scp_t));

	s_scp->scp_reg0 = inp32(scp_base + SCP_REG0);
	s_scp->scp_reg1 = inp32(scp_base + SCP_REG1);
	s_scp->scp_reg2 = inp32(scp_base + SCP_REG2);
	s_scp->scp_reg3 = inp32(scp_base + SCP_REG3);
	s_scp->scp_reg4 = inp32(scp_base + SCP_REG4);
	s_scp->scp_reg5 = inp32(scp_base + SCP_REG5);
	s_scp->scp_reg6 = inp32(scp_base + SCP_REG6);
	s_scp->scp_reg7 = inp32(scp_base + SCP_REG7);
	s_scp->scp_reg8 = inp32(scp_base + SCP_REG8);
	s_scp->scp_reg9 = inp32(scp_base + SCP_REG9);
	s_scp->scp_reg10 = inp32(scp_base + SCP_REG10);
	s_scp->scp_reg11 = inp32(scp_base + SCP_REG11);
	s_scp->scp_reg12 = inp32(scp_base + SCP_REG12);
	s_scp->scp_reg13 = inp32(scp_base + SCP_REG13);
}

static void hisi_dss_scp_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *scp_base, dss_scp_t *s_scp)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(scp_base == NULL);
	BUG_ON(s_scp == NULL);

	hisifd->set_reg(hisifd, scp_base + SCP_REG0, s_scp->scp_reg0, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG1, s_scp->scp_reg1, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG2, s_scp->scp_reg2, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG3, s_scp->scp_reg3, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG4, s_scp->scp_reg4, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG5, s_scp->scp_reg5, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG6, s_scp->scp_reg6, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG7, s_scp->scp_reg7, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG8, s_scp->scp_reg8, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG9, s_scp->scp_reg9, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG10, s_scp->scp_reg10, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG11, s_scp->scp_reg11, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG12, s_scp->scp_reg12, 32, 0);
	hisifd->set_reg(hisifd, scp_base + SCP_REG13, s_scp->scp_reg13, 32, 0);
}

int hisi_dss_scp_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	dss_scp_t *scp = NULL;
	bool need_scp = false;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	if ((hisifd->panel_info.prefix_sharpness_support == 1) && isYUV(layer->src.format)) {
		if (((layer->chn_idx == DPE0_CHN0) && (layer->need_cap & CAP_CROSS_SWITCH)) ||
			((layer->chn_idx == DPE0_CHN1) && !(layer->need_cap & CAP_CROSS_SWITCH)) ||
			((layer->chn_idx == DPE2_CHN0) && (layer->need_cap & CAP_CROSS_SWITCH)) ||
			((layer->chn_idx == DPE2_CHN1) && !(layer->need_cap & CAP_CROSS_SWITCH))) {
			need_scp = true;
		}
	}
	if (!need_scp) {
		return 0;
	}

	scp = &(hisifd->dss_module.scp[layer->chn_idx]);
	hisifd->dss_module.scp_used[layer->chn_idx] = 1;

	scp->scp_reg0 = set_bits32(scp->scp_reg0, hisifd->panel_info.scp_reg0, 32, 0);
	scp->scp_reg1 = set_bits32(scp->scp_reg1, hisifd->panel_info.scp_reg1, 32, 0);
	scp->scp_reg2 = set_bits32(scp->scp_reg2, hisifd->panel_info.scp_reg2, 32, 0);
	scp->scp_reg3 = set_bits32(scp->scp_reg3, hisifd->panel_info.scp_reg3, 32, 0);
	scp->scp_reg4 = set_bits32(scp->scp_reg4, hisifd->panel_info.scp_reg4, 32, 0);
	scp->scp_reg5 = set_bits32(scp->scp_reg5, hisifd->panel_info.scp_reg5, 32, 0);
	scp->scp_reg6 = set_bits32(scp->scp_reg6, hisifd->panel_info.scp_reg6, 32, 0);
	scp->scp_reg7 = set_bits32(scp->scp_reg7, hisifd->panel_info.scp_reg7, 32, 0);
	scp->scp_reg8 = set_bits32(scp->scp_reg8, hisifd->panel_info.scp_reg8, 32, 0);
	scp->scp_reg9 = set_bits32(scp->scp_reg9, hisifd->panel_info.scp_reg9, 32, 0);
	scp->scp_reg10 = set_bits32(scp->scp_reg10, hisifd->panel_info.scp_reg10, 32, 0);
	scp->scp_reg11 = set_bits32(scp->scp_reg11, hisifd->panel_info.scp_reg11, 32, 0);
	scp->scp_reg12 = set_bits32(scp->scp_reg12, hisifd->panel_info.scp_reg12, 32, 0);
	scp->scp_reg13 = set_bits32(scp->scp_reg13, hisifd->panel_info.scp_reg13, 32, 0);

	scp->scp_reg6 = set_bits32(scp->scp_reg6, DSS_WIDTH(layer->dst_rect.w), 16, 9);

	return 0;
}

/*******************************************************************************
** DSS CSC
*/
#define CSC_ROW	(3)
#define CSC_COL	(5)

/*	Rec.601 for Computer
*	 [ p00 p01 p02 cscidc2 cscodc2 ]
*	 [ p10 p11 p12 cscidc1 cscodc1 ]
*	 [ p20 p21 p22 cscidc0 cscodc0 ]
*/

static int CSC_COE_YUV2RGB601_NARROW[CSC_ROW][CSC_COL] = {
	{0x12a, 0x000, 0x199, 0x1f0, 0x0},
	{0x12a, 0x79d, 0x731, 0x180, 0x0},
	{0x12a, 0x205, 0x000, 0x180, 0x0}
};

static int CSC_COE_RGB2YUV601_NARROW[CSC_ROW][CSC_COL] = {
	{0x042, 0x081, 0x019, 0x0, 0x010},
	{0x7DB, 0x7B7, 0x070, 0x0, 0x080},
	{0x070, 0x7A3, 0x7EF, 0x0, 0x080}
};

static int CSC_COE_YUV2RGB709_NARROW[CSC_ROW][CSC_COL] = {
	{0x12A, 0x000, 0x1CB, 0x1F0, 0x0},
	{0x12A, 0x7CA, 0x778, 0x180, 0x0},
	{0x12A, 0x21D, 0x000, 0x180, 0x0}
};

static int CSC_COE_RGB2YUV709_NARROW[CSC_ROW][CSC_COL] = {
	{0x02F, 0x09D, 0x010, 0x0, 0x010},
	{0x7E7, 0x7AA, 0x070, 0x0, 0x080},
	{0x070, 0x79B, 0x7F7, 0x0, 0x080}
};


static int CSC_COE_YUV2RGB601_WIDE[CSC_ROW][CSC_COL] = {
	{0x100, 0x000, 0x15f, 0x000, 0x0},
	{0x100, 0x7ab, 0x74e, 0x180, 0x0},
	{0x100, 0x1bb, 0x000, 0x180, 0x0}
};

static int CSC_COE_RGB2YUV601_WIDE[CSC_ROW][CSC_COL] = {
	{0x04d, 0x096, 0x01d, 0x0, 0x000},
	{0x7d5, 0x79b, 0x083, 0x0, 0x080},
	{0x083, 0x793, 0x7ec, 0x0, 0x080},
};

static int CSC_COE_YUV2RGB709_WIDE[CSC_ROW][CSC_COL] = {
	{0x100, 0x000, 0x18a, 0x000, 0x0},
	{0x100, 0x7d2, 0x78b, 0x180, 0x0},
	{0x100, 0x1d1, 0x000, 0x180, 0x0},
};

static int CSC_COE_RGB2YUV709_WIDE[CSC_ROW][CSC_COL] = {
	{0x037, 0x0b7, 0x012, 0x0, 0x000},
	{0x7e3, 0x79c, 0x083, 0x0, 0x080},
	{0x083, 0x78a, 0x7f5, 0x0, 0x080},
};

static void hisi_dss_csc_init(char __iomem *csc_base, dss_csc_t *s_csc)
{
	BUG_ON(csc_base == NULL);
	BUG_ON(s_csc == NULL);

	memset(s_csc, 0, sizeof(dss_csc_t));

	s_csc->vhdcscidc = inp32(csc_base + CSC_VHDCSCIDC);
	s_csc->vhdcscodc = inp32(csc_base + CSC_VHDCSCODC);
	s_csc->vhdcscp0 = inp32(csc_base + CSC_VHDCSCP0);
	s_csc->vhdcscp1 = inp32(csc_base + CSC_VHDCSCP1);
	s_csc->vhdcscp2 = inp32(csc_base + CSC_VHDCSCP2);
	s_csc->vhdcscp3 = inp32(csc_base + CSC_VHDCSCP3);
	s_csc->vhdcscp4 = inp32(csc_base + CSC_VHDCSCP4);
	s_csc->icg_module = inp32(csc_base + CSC_ICG_MODULE);
}

static void hisi_dss_csc_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *csc_base, dss_csc_t *s_csc)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(csc_base == NULL);
	BUG_ON(s_csc == NULL);

	hisifd->set_reg(hisifd, csc_base + CSC_VHDCSCIDC, s_csc->vhdcscidc, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_VHDCSCODC, s_csc->vhdcscodc, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_VHDCSCP0, s_csc->vhdcscp0, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_VHDCSCP1, s_csc->vhdcscp1, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_VHDCSCP2, s_csc->vhdcscp2, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_VHDCSCP3, s_csc->vhdcscp3, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_VHDCSCP4, s_csc->vhdcscp4, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_ICG_MODULE, s_csc->icg_module, 32, 0);
}

int hisi_dss_csc_config(struct hisi_fb_data_type *hisifd,
	dss_layer_t *layer, dss_wb_layer_t *wb_layer)
{
	dss_csc_t *csc = NULL;
	int chn_idx = 0;
	uint32_t need_cap = 0;
	uint32_t format = 0;
	uint32_t csc_mode = 0;
	int (*csc_coe_yuv2rgb)[CSC_COL];
	int (*csc_coe_rgb2yuv)[CSC_COL];

	BUG_ON(hisifd == NULL);
	BUG_ON((layer == NULL) && (wb_layer == NULL));
	BUG_ON(layer && wb_layer);

	if (wb_layer) {
		chn_idx = wb_layer->chn_idx;
		need_cap = wb_layer->need_cap;
		format = wb_layer->dst.format;
		csc_mode = wb_layer->dst.csc_mode;
	} else {
		chn_idx = layer->chn_idx;
		need_cap = layer->need_cap;
		format = layer->src.format;
		csc_mode = layer->src.csc_mode;
	}

	if (!isYUV(format))
		return 0;

	if (csc_mode == DSS_CSC_601_WIDE) {
		csc_coe_yuv2rgb = CSC_COE_YUV2RGB601_WIDE;
		csc_coe_rgb2yuv = CSC_COE_RGB2YUV601_WIDE;
	} else if (csc_mode == DSS_CSC_601_NARROW) {
		csc_coe_yuv2rgb = CSC_COE_YUV2RGB601_NARROW;
		csc_coe_rgb2yuv = CSC_COE_RGB2YUV601_NARROW;
	} else if (csc_mode == DSS_CSC_709_WIDE) {
		csc_coe_yuv2rgb = CSC_COE_YUV2RGB709_WIDE;
		csc_coe_rgb2yuv = CSC_COE_RGB2YUV709_WIDE;
	} else if (csc_mode == DSS_CSC_709_NARROW) {
		csc_coe_yuv2rgb = CSC_COE_YUV2RGB709_NARROW;
		csc_coe_rgb2yuv = CSC_COE_RGB2YUV709_NARROW;
	} else {
		HISI_FB_ERR("not support this csc_mode(%d)!\n", csc_mode);
		csc_coe_yuv2rgb = CSC_COE_YUV2RGB601_WIDE;
		csc_coe_rgb2yuv = CSC_COE_RGB2YUV601_WIDE;
	}

	chn_idx = hisi_get_chnIndex4crossSwitch(need_cap, chn_idx);
	if (chn_idx < 0) {
		HISI_FB_ERR("hisi_get_chnIndex4crossSwitch failed!\n");
		return -EINVAL;
	}

	csc = &(hisifd->dss_module.csc[chn_idx]);
	hisifd->dss_module.csc_used[chn_idx] = 1;

	if (layer) {
		csc->vhdcscidc = set_bits32(csc->vhdcscidc, 0x1, 1, 27);
		csc->vhdcscidc = set_bits32(csc->vhdcscidc,
			(csc_coe_yuv2rgb[2][3] |
			(csc_coe_yuv2rgb[1][3] << 9) |
			(csc_coe_yuv2rgb[0][3] << 18)), 27, 0);

		csc->vhdcscodc = set_bits32(csc->vhdcscodc,
			(csc_coe_yuv2rgb[2][4] |
			(csc_coe_yuv2rgb[1][4] << 9) |
			(csc_coe_yuv2rgb[0][4] << 18)), 27, 0);

		csc->vhdcscp0 = set_bits32(csc->vhdcscp0, csc_coe_yuv2rgb[0][0], 11, 0);
		csc->vhdcscp0 = set_bits32(csc->vhdcscp0, csc_coe_yuv2rgb[0][1], 11, 16);

		csc->vhdcscp1 = set_bits32(csc->vhdcscp1, csc_coe_yuv2rgb[0][2], 11, 0);
		csc->vhdcscp1 = set_bits32(csc->vhdcscp1, csc_coe_yuv2rgb[1][0], 11, 16);

		csc->vhdcscp2 = set_bits32(csc->vhdcscp2, csc_coe_yuv2rgb[1][1], 11, 0);
		csc->vhdcscp2 = set_bits32(csc->vhdcscp2, csc_coe_yuv2rgb[1][2], 11, 16);

		csc->vhdcscp3 = set_bits32(csc->vhdcscp3, csc_coe_yuv2rgb[2][0], 11, 0);
		csc->vhdcscp3 = set_bits32(csc->vhdcscp3, csc_coe_yuv2rgb[2][1], 11, 16);

		csc->vhdcscp4 = set_bits32(csc->vhdcscp4, csc_coe_yuv2rgb[2][2], 11, 0);
	}

	if (wb_layer) {
		csc->vhdcscidc = set_bits32(csc->vhdcscidc, 0x1, 1, 27);
		csc->vhdcscidc = set_bits32(csc->vhdcscidc,
			(csc_coe_rgb2yuv[2][3] |
			(csc_coe_rgb2yuv[1][3] << 9) |
			(csc_coe_rgb2yuv[0][3] << 18)), 27, 0);

		csc->vhdcscodc = set_bits32(csc->vhdcscodc,
			(csc_coe_rgb2yuv[2][4] |
			(csc_coe_rgb2yuv[1][4] << 9) |
			(csc_coe_rgb2yuv[0][4] << 18)), 27, 0);

		csc->vhdcscp0 = set_bits32(csc->vhdcscp0, csc_coe_rgb2yuv[0][0], 11, 0);
		csc->vhdcscp0 = set_bits32(csc->vhdcscp0, csc_coe_rgb2yuv[0][1], 11, 16);

		csc->vhdcscp1 = set_bits32(csc->vhdcscp1, csc_coe_rgb2yuv[0][2], 11, 0);
		csc->vhdcscp1 = set_bits32(csc->vhdcscp1, csc_coe_rgb2yuv[1][0], 11, 16);

		csc->vhdcscp2 = set_bits32(csc->vhdcscp2, csc_coe_rgb2yuv[1][1], 11, 0);
		csc->vhdcscp2 = set_bits32(csc->vhdcscp2, csc_coe_rgb2yuv[1][2], 11, 16);

		csc->vhdcscp3 = set_bits32(csc->vhdcscp3, csc_coe_rgb2yuv[2][0], 11, 0);
		csc->vhdcscp3 = set_bits32(csc->vhdcscp3, csc_coe_rgb2yuv[2][1], 11, 16);

		csc->vhdcscp4 = set_bits32(csc->vhdcscp4, csc_coe_rgb2yuv[2][2], 27, 0);
	}

	csc->icg_module = set_bits32(csc->icg_module, 0x1, 1, 2);

	return 0;
}


/*******************************************************************************
** DSS OVL
*/
static int hisi_get_ovl_src_select(int chn_idx, int ovl_type)
{
	int ret = 0;

	if (ovl_type == DSS_OVL_PDP || ovl_type == DSS_OVL_ADP) {
		switch (chn_idx) {
		case DPE0_CHN0:
		case DPE3_CHN0:
			ret = 0;
			break;
		case DPE0_CHN1:
		case DPE3_CHN1:
			ret = 1;
			break;
		case DPE0_CHN2:
		case DPE3_CHN2:
			ret = 2;
			break;
		case DPE0_CHN3:
		case DPE3_CHN3:
			ret = 3;
			break;

		case DPE2_CHN0:
			ret = 4;
			break;
		case DPE2_CHN1:
			ret = 5;
			break;
		case DPE2_CHN2:
			ret = 6;
			break;
		case DPE2_CHN3:
			ret = 7;
			break;

		default:
			HISI_FB_ERR("not support, chn_idx=%d, ovl_type=%d!\n", chn_idx, ovl_type);
			ret = -1;
			break;
		}
	} else if (ovl_type == DSS_OVL_SDP) {
		switch (chn_idx) {
		case DPE1_CHN0:
			ret = 0;
			break;
		case DPE1_CHN1:
			ret = 1;
			break;

		case DPE2_CHN0:
			ret = 2;
			break;
		case DPE2_CHN1:
			ret = 3;
			break;
		default:
			HISI_FB_ERR("not support, chn_idx=%d, ovl_type=%d!\n", chn_idx, ovl_type);
			ret = -1;
			break;
		}
	} else {
		HISI_FB_ERR("not support this ovl_type(%d)!\n", ovl_type);
		ret = -1;
	}

	return ret;
}

static uint32_t get_ovl_blending_alpha_val(dss_layer_t *layer)
{
	int blend_mode = 0;

	int alpha_smode = 0;
	int src_pmode = 0;
	int src_gmode = 0;
	int src_amode = 0;
	int fix_mode = 0;
	int dst_pmode = 0;
	int dst_gmode = 0;
	int dst_amode = 0;
	int src_glb_alpha = 0;
	int dst_glb_alpha = 0;

	bool has_per_pixel_alpha = false;

	int x = 0;
	uint32_t alpha_val = 0;

	BUG_ON(layer == NULL);

	if (layer->glb_alpha < 0) {
		layer->glb_alpha = 0;
		HISI_FB_ERR("layer's glb_alpha(0x%x) is out of range!", layer->glb_alpha);
	} else if (layer->glb_alpha > 0xFF) {
		layer->glb_alpha = 0xFF;
		HISI_FB_ERR("layer's glb_alpha(0x%x) is out of range!", layer->glb_alpha);
	}

	has_per_pixel_alpha = hal_format_has_alpha(layer->src.format);

	src_glb_alpha = layer->glb_alpha;
	dst_glb_alpha = layer->glb_alpha;

	if (layer->layer_idx == 0) {
		blend_mode = DSS_BLEND_SRC;
	} else {
		if (layer->blending == HISI_FB_BLENDING_PREMULT) {
			if (has_per_pixel_alpha) {
				blend_mode = (layer->glb_alpha < 0xFF) ? DSS_BLEND_FIX_PER12 : DSS_BLEND_SRC_OVER_DST;
			} else {
				blend_mode = (layer->glb_alpha < 0xFF) ? DSS_BLEND_FIX_PER8 : DSS_BLEND_SRC;
			}
		} else if (layer->blending == HISI_FB_BLENDING_COVERAGE) {
			if (has_per_pixel_alpha) {
				blend_mode = (layer->glb_alpha < 0xFF) ? DSS_BLEND_FIX_PER13 : DSS_BLEND_FIX_OVER;
			} else {
				blend_mode = (layer->glb_alpha < 0xFF) ? DSS_BLEND_FIX_PER8 : DSS_BLEND_SRC;
			}
		} else {
			blend_mode = DSS_BLEND_SRC;
		}
	}

	if (g_debug_ovl_online_composer) {
		HISI_FB_INFO("layer_idx=%d, chn_idx=%d, has_per_pixel_alpha=%d, src_glb_alpha=%d, dst_glb_alpha=%d, blend_mode=%d.\n",
			layer->layer_idx, layer->chn_idx, has_per_pixel_alpha, src_glb_alpha, dst_glb_alpha, blend_mode);
	}

	switch (blend_mode) {
	case DSS_BLEND_CLEAR:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 0, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 0;
		break;
	case DSS_BLEND_SRC:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 0, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 1;
		break;
	case DSS_BLEND_DST:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 1, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 0;
		break;
	case DSS_BLEND_SRC_OVER_DST:
		alpha_smode = 0,
		src_pmode = 1, src_gmode = 0, src_amode = 3, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 1;
		break;
	case DSS_BLEND_DST_OVER_SRC:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 1, fix_mode = 0,
		dst_pmode = 1, dst_gmode = 0, dst_amode = 3;
		break;
	case DSS_BLEND_SRC_IN_DST:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 0, fix_mode = 0,
		dst_pmode = 0, dst_gmode = 0, dst_amode = 3;
		break;
	case DSS_BLEND_DST_IN_SRC:
		alpha_smode = 0,
		src_pmode = 0, src_gmode = 0, src_amode = 3, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 0;
		break;
	case DSS_BLEND_SRC_OUT_DST:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 0, fix_mode = 0,
		dst_pmode = 1, dst_gmode = 0, dst_amode = 3;
		break;
	case DSS_BLEND_DST_OUT_SRC:
		alpha_smode = 0,
		src_pmode = 1, src_gmode = 0, src_amode = 3, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 0;
		break;
	case DSS_BLEND_SRC_ATOP_DST:
		alpha_smode = 0,
		src_pmode = 1, src_gmode = 0, src_amode = 3, fix_mode = 0,
		dst_pmode = 0, dst_gmode = 0, dst_amode = 3;
		break;
	case DSS_BLEND_DST_ATOP_SRC:
		alpha_smode = 0,
		src_pmode = 0, src_gmode = 0, src_amode = 3, fix_mode = 0,
		dst_pmode = 1, dst_gmode = 0, dst_amode = 3;
		break;
	case DSS_BLEND_SRC_XOR_DST:
		alpha_smode = 0,
		src_pmode = 1, src_gmode = 0, src_amode = 3, fix_mode = 0,
		dst_pmode = 1, dst_gmode = 0, dst_amode = 3;
		break;
	case DSS_BLEND_SRC_ADD_DST:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 1, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 1;
		break;
	case DSS_BLEND_FIX_OVER:
		alpha_smode = 0,
		src_pmode = 1, src_gmode = 0, src_amode = 3, fix_mode = 1,
		dst_pmode = 0, dst_gmode = 0, dst_amode = 3;
		break;
	case DSS_BLEND_FIX_PER0:
		alpha_smode = 0,
		src_pmode = 1, src_gmode = 0, src_amode = 3, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 3;
		break;
	case DSS_BLEND_FIX_PER1:
		alpha_smode = 1,
		src_pmode = 1, src_gmode = 0, src_amode = 3, fix_mode = 1,
		dst_pmode = 0, dst_gmode = 1, dst_amode = 3;
		break;
	case DSS_BLEND_FIX_PER2:
		alpha_smode = 0,
		src_pmode = x, src_gmode = 2, src_amode = 2, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 1;
		break;
	case DSS_BLEND_FIX_PER3:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 1, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 2;
		break;
	case DSS_BLEND_FIX_PER4:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 0, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 3;
		break;
	case DSS_BLEND_FIX_PER5:
		alpha_smode = 0,
		src_pmode = x, src_gmode = 2, src_amode = 3, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 0;
		break;
	case DSS_BLEND_FIX_PER6:
		alpha_smode = 0,
		src_pmode = x, src_gmode = x, src_amode = 0, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 2;
		break;
	case DSS_BLEND_FIX_PER7:
		alpha_smode = 0,
		src_pmode = x, src_gmode = 2, src_amode = 2, fix_mode = x,
		dst_pmode = x, dst_gmode = x, dst_amode = 0;
		break;
	case DSS_BLEND_FIX_PER8:
		alpha_smode = 0,
		src_pmode = x, src_gmode = 2, src_amode = 2, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 3;
		break;
	case DSS_BLEND_FIX_PER9:
		alpha_smode = 0,
		src_pmode = x, src_gmode = 2, src_amode = 3, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 2;
		break;
	case DSS_BLEND_FIX_PER10:
		alpha_smode = 0,
		src_pmode = x, src_gmode = 2, src_amode = 2, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 2;
		break;
	case DSS_BLEND_FIX_PER11:
		alpha_smode = 0,
		src_pmode = x, src_gmode = 2, src_amode = 3, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 3;
		break;
	case DSS_BLEND_FIX_PER12:
		alpha_smode = 0,
		src_pmode = 0, src_gmode = 1, src_amode = 2, fix_mode = x,
		dst_pmode = x, dst_gmode = 2, dst_amode = 3;
		break;
	case DSS_BLEND_FIX_PER13:
		alpha_smode = 0,
		src_pmode = 0, src_gmode = 1, src_amode = 2, fix_mode = 1,
		dst_pmode = 0, dst_gmode = 1, dst_amode = 3;
		break;
	default:
		HISI_FB_ERR("not support this blend mode(%d)!", blend_mode);
		break;
	}

	alpha_val = (dst_glb_alpha << 0) | (fix_mode << 8) | (dst_pmode << 9) | (dst_gmode << 12) | (dst_amode << 14) |
		(src_glb_alpha << 16) | (alpha_smode << 24) | (src_pmode << 25) | (src_gmode << 28) | (src_amode << 30);

	return alpha_val;
}

static void hisi_dss_ovl_init(char __iomem *ovl_base, dss_ovl_t *s_ovl)
{
	int i = 0;

	BUG_ON(ovl_base == NULL);
	BUG_ON(s_ovl == NULL);

	memset(s_ovl, 0, sizeof(dss_ovl_t));

	s_ovl->ovl_ov_size = inp32(ovl_base + OVL_OV_SIZE);
	s_ovl->ovl_ov_bg_color = inp32(ovl_base + OVL_OV_BG_COLOR);
	s_ovl->ovl_dst_startpos = inp32(ovl_base + OVL_DST_STARTPOS);
	s_ovl->ovl_dst_endpos = inp32(ovl_base + OVL_DST_ENDPOS);
	s_ovl->ovl_ov_gcfg = inp32(ovl_base + OVL_OV_GCFG);

	for (i = 0; i < OVL_LAYER_NUM; i++) {
		s_ovl->ovl_layer[i].layer_pos =
			inp32(ovl_base + OVL_LAYER0_POS + i * 0x2C);
		s_ovl->ovl_layer[i].layer_size =
			inp32(ovl_base + OVL_LAYER0_SIZE + i * 0x2C);
		s_ovl->ovl_layer[i].layer_pattern =
			inp32(ovl_base + OVL_LAYER0_PATTERN + i * 0x2C);
		s_ovl->ovl_layer[i].layer_alpha =
			inp32(ovl_base + OVL_LAYER0_ALPHA + i * 0x2C);
		s_ovl->ovl_layer[i].layer_cfg =
			inp32(ovl_base + OVL_LAYER0_CFG + i * 0x2C);

		s_ovl->ovl_layer_position[i].ovl_layer_pspos =
			inp32(ovl_base + OVL_LAYER0_PSPOS + i * 0x8);
		s_ovl->ovl_layer_position[i].ovl_layer_pepos =
			inp32(ovl_base + OVL_LAYER0_PEPOS + i * 0x8);
	}

}

static void hisi_dss_ovl_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *ovl_base, dss_ovl_t *s_ovl)
{
	int i = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(ovl_base == NULL);
	BUG_ON(s_ovl == NULL);

	hisifd->set_reg(hisifd, ovl_base + OVL_OV_SIZE, s_ovl->ovl_ov_size, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OVL_OV_BG_COLOR, s_ovl->ovl_ov_bg_color, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OVL_DST_STARTPOS, s_ovl->ovl_dst_startpos, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OVL_DST_ENDPOS, s_ovl->ovl_dst_endpos, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OVL_OV_GCFG, s_ovl->ovl_ov_gcfg, 32, 0);
	if (g_fpga_flag == 1) {
		hisifd->set_reg(hisifd, ovl_base + OVL_GATE_CTRL, 0x2, 2, 0);
	}

	for (i = 0; i < OVL_LAYER_NUM; i++) {
		hisifd->set_reg(hisifd, ovl_base + OVL_LAYER0_POS + i * 0x2C,
			s_ovl->ovl_layer[i].layer_pos, 32, 0);
		hisifd->set_reg(hisifd, ovl_base + OVL_LAYER0_SIZE + i * 0x2C,
			s_ovl->ovl_layer[i].layer_size, 32, 0);
		hisifd->set_reg(hisifd, ovl_base + OVL_LAYER0_PATTERN + i * 0x2C,
			s_ovl->ovl_layer[i].layer_pattern, 32, 0);
		hisifd->set_reg(hisifd, ovl_base + OVL_LAYER0_ALPHA + i * 0x2C,
			s_ovl->ovl_layer[i].layer_alpha, 32, 0);
		hisifd->set_reg(hisifd, ovl_base + OVL_LAYER0_CFG + i * 0x2C,
			s_ovl->ovl_layer[i].layer_cfg, 32, 0);

		hisifd->set_reg(hisifd, ovl_base + OVL_LAYER0_PSPOS + i * 0x8,
			s_ovl->ovl_layer_position[i].ovl_layer_pspos, 32, 0);
		hisifd->set_reg(hisifd, ovl_base + OVL_LAYER0_PEPOS + i * 0x8,
			s_ovl->ovl_layer_position[i].ovl_layer_pepos, 32, 0);
	}
}

int hisi_dss_ovl_base_config(struct hisi_fb_data_type *hisifd,
	dss_rect_t *wb_block_rect, uint32_t ovl_flags)
{
	dss_ovl_t *ovl = NULL;
	uint8_t ovl_type = 0;
	int img_width = 0;
	int img_height = 0;

	BUG_ON(hisifd == NULL);

	ovl_type = hisifd->ovl_type;

	ovl = &(hisifd->dss_module.ov[ovl_type]);
	hisifd->dss_module.ov_used[ovl_type] = 1;

	if (wb_block_rect) {
		img_width = wb_block_rect->w;
		img_height = wb_block_rect->h;
	} else {
		img_width = hisifd->panel_info.xres;
		img_height = hisifd->panel_info.yres;
	}

	ovl->ovl_ov_size = set_bits32(ovl->ovl_ov_size, DSS_WIDTH(img_width), 15, 0);
	ovl->ovl_ov_size = set_bits32(ovl->ovl_ov_size, DSS_HEIGHT(img_height), 15, 16);
	ovl->ovl_ov_bg_color= set_bits32(ovl->ovl_ov_bg_color, 0xFF000000, 32, 0);
	ovl->ovl_dst_startpos = set_bits32(ovl->ovl_dst_startpos, 0x0, 32, 0);
	ovl->ovl_dst_endpos = set_bits32(ovl->ovl_dst_endpos, DSS_WIDTH(img_width), 15, 0);
	ovl->ovl_dst_endpos = set_bits32(ovl->ovl_dst_endpos, DSS_HEIGHT(img_height), 15, 16);
	ovl->ovl_ov_gcfg = set_bits32(ovl->ovl_ov_gcfg, 0x1, 1, 0);
	ovl->ovl_ov_gcfg = set_bits32(ovl->ovl_ov_gcfg, 0x1, 1, 16);

	/*
	** handle glb: OVL_MUX
	** OV0_MUX
	** OV1_MUX
	** OV2_MUX
	*/
	if (hisifd->dss_glb.ovl_mux_base) {
		if (ovl_type == DSS_OVL_PDP) {
			hisifd->dss_glb.ovl_mux_val =
				set_bits32(hisifd->dss_glb.ovl_mux_val, (0x1 | (0x1 << 2)), 4, 0);
			hisifd->dss_glb.ovl_mux_used = 1;
		} else if (ovl_type == DSS_OVL_SDP) {
			hisifd->dss_glb.ovl_mux_val =
				set_bits32(hisifd->dss_glb.ovl_mux_val, (0x1 | (0x1 << 2)), 3, 0);
			hisifd->dss_glb.ovl_mux_used = 1;
		} else if (ovl_type == DSS_OVL_ADP) {
			hisifd->dss_glb.ovl_mux_val =
				set_bits32(hisifd->dss_glb.ovl_mux_val, 0x1, 1, 3);
			hisifd->dss_glb.ovl_mux_used = 1;
		} else {
			HISI_FB_ERR("ovl_mux, ovl_type(%d) is invalid!\n", ovl_type);
		}
	} else {
		HISI_FB_ERR("ovl_type(%d), ovl_mux_base is invalid!\n", ovl_type);
	}

	/*
	** handle glb: OVL_SCENE
	** DSS_GLB_OV0_SCENE
	** DSS_GLB_OV1_SCENE
	** DSS_GLB_OV2_SCENE
	*/
	if (hisifd->dss_glb.ovl_scene_base) {
		hisifd->dss_glb.ovl_scene_val =
			set_bits32(hisifd->dss_glb.ovl_scene_val, ovl_flags, 3, 0);
		hisifd->dss_glb.ovl_scene_used = 1;
	} else {
		HISI_FB_ERR("ovl_type(%d), ovl_scene_base is invalid!\n", ovl_type);
	}

	return 0;
}

int hisi_dss_ovl_layer_config(struct hisi_fb_data_type *hisifd,
	dss_layer_t *layer, dss_rect_t *wb_block_rect)
{
	dss_ovl_t *ovl = NULL;
	uint8_t ovl_type = 0;
	int chn_idx = 0;
	int chn_idx_tmp = 0;
	int src_select = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	ovl_type = hisifd->ovl_type;
	chn_idx = layer->chn_idx;

	ovl = &(hisifd->dss_module.ov[ovl_type]);
	hisifd->dss_module.ov_used[ovl_type] = 1;

	if (layer->need_cap & CAP_BASE) {
		if ((layer->layer_idx == 0) &&
			(layer->dst_rect.x == 0) &&
			(layer->dst_rect.y == 0)  &&
			(layer->dst_rect.w == hisifd->panel_info.xres) &&
			(layer->dst_rect.h == hisifd->panel_info.yres)) {

			ovl->ovl_ov_bg_color = set_bits32(ovl->ovl_ov_bg_color, layer->color, 32, 0);
			ovl->ovl_ov_gcfg = set_bits32(ovl->ovl_ov_gcfg, 0x1, 1, 16);
		} else {
			HISI_FB_ERR("layer%d not a base layer!", layer->layer_idx);
		}

		return 0;
	}

	if (wb_block_rect) {
		ovl->ovl_layer[layer->layer_idx].layer_pos = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pos,
			(layer->dst_rect.x - wb_block_rect->x), 15, 0);
		ovl->ovl_layer[layer->layer_idx].layer_pos = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pos,
			(layer->dst_rect.y - wb_block_rect->y), 15, 16);

		ovl->ovl_layer[layer->layer_idx].layer_size = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_size,
			(layer->dst_rect.x - wb_block_rect->x + DSS_WIDTH(layer->dst_rect.w)), 15, 0);
		ovl->ovl_layer[layer->layer_idx].layer_size = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_size,
			(layer->dst_rect.y - wb_block_rect->y + DSS_HEIGHT(layer->dst_rect.h)), 15, 16);
	} else {
		ovl->ovl_layer[layer->layer_idx].layer_pos = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pos,
			layer->dst_rect.x, 15, 0);
		ovl->ovl_layer[layer->layer_idx].layer_pos = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pos,
			layer->dst_rect.y, 15, 16);

		ovl->ovl_layer[layer->layer_idx].layer_size = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_size,
			DSS_WIDTH(layer->dst_rect.x + layer->dst_rect.w), 15, 0);
		ovl->ovl_layer[layer->layer_idx].layer_size = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_size,
			DSS_HEIGHT(layer->dst_rect.y + layer->dst_rect.h), 15, 16);
	}

	ovl->ovl_layer[layer->layer_idx].layer_alpha = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_alpha,
		get_ovl_blending_alpha_val(layer), 32, 0);

	if (layer->need_cap & CAP_DIM) {
		ovl->ovl_layer[layer->layer_idx].layer_pattern =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pattern, layer->color, 32, 0);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, 0x1, 1, 0);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, 0x1, 1, 8);
	} else {
		chn_idx_tmp = hisi_get_chnIndex4crossSwitch(layer->need_cap, chn_idx);
		if (chn_idx_tmp < 0) {
			HISI_FB_ERR("layer%d chn%d hisi_get_chnIndex4crossSwitch failed!\n",
				layer->layer_idx, chn_idx);
			return -EINVAL;
		}

		src_select = hisi_get_ovl_src_select(chn_idx_tmp, ovl_type);
		if (src_select < 0) {
			HISI_FB_ERR("layer%d chn%d hisi_get_ovl_src_select failed!\n",
				layer->layer_idx, chn_idx);
			return -EINVAL;
		}

		ovl->ovl_layer[layer->layer_idx].layer_pattern =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pattern, 0x0, 32, 0);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, 0x1, 1, 0);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, 0x0, 1, 8);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, src_select, 3, 9);
	}

	ovl->ovl_layer_position[layer->layer_idx].ovl_layer_pspos =
		set_bits32(ovl->ovl_layer_position[layer->layer_idx].ovl_layer_pspos, layer->dst_rect.x, 15, 0);
	ovl->ovl_layer_position[layer->layer_idx].ovl_layer_pspos =
		set_bits32(ovl->ovl_layer_position[layer->layer_idx].ovl_layer_pspos, layer->dst_rect.y, 15, 16);
	ovl->ovl_layer_position[layer->layer_idx].ovl_layer_pepos =
		set_bits32(ovl->ovl_layer_position[layer->layer_idx].ovl_layer_pepos,
		DSS_WIDTH(layer->dst_rect.x + layer->dst_rect.w), 15, 0);
	ovl->ovl_layer_position[layer->layer_idx].ovl_layer_pepos =
		set_bits32(ovl->ovl_layer_position[layer->layer_idx].ovl_layer_pepos,
		DSS_HEIGHT(layer->dst_rect.y + layer->dst_rect.h), 15, 16);

	return 0;
}

int hisi_dss_ovl_optimized(struct hisi_fb_data_type *hisifd)
{
	char __iomem *ovl_base = NULL;
	int offset = 0;
	int i = 0;

	uint32_t layer_cfg_val = 0;
	uint32_t layer_info_alpha_val = 0;
	uint32_t layer_info_srccolor_val = 0;
	uint32_t layer_pattern = 0;

	BUG_ON(hisifd == NULL);

	ovl_base = hisifd->dss_base +
		g_dss_module_ovl_base[hisifd->ovl_type][MODULE_OVL_BASE];

	for (i = 0; i < OVL_LAYER_NUM; i++) {
		offset = i * 0x2C;

		layer_cfg_val = inp32(ovl_base + OVL_LAYER0_CFG + offset);
		layer_info_alpha_val = inp32(ovl_base + OVL_LAYER0_INFO_ALPHA + offset);
		layer_info_srccolor_val = inp32(ovl_base + OVL_LAYER0_INFO_SRCCOLOR + offset);

		if ((layer_cfg_val & BIT_OVL_LAYER_ENABLE) &&
			!(layer_cfg_val & BIT_OVL_LAYER_SRC_CFG) &&
			(layer_info_alpha_val & BIT_OVL_LAYER_SRCALPHA_FLAG) &&
			(layer_info_srccolor_val & BIT_OVL_LAYER_SRCCOLOR_FLAG)) {

			layer_pattern = ((layer_info_alpha_val << 8) & 0xFF000000) |
				((layer_info_srccolor_val >> 8) & 0x00FFFFFF);
			set_reg(ovl_base + OVL_LAYER0_PATTERN + offset, layer_pattern, 32, 0);

			layer_cfg_val |= BIT_OVL_LAYER_SRC_CFG;
			set_reg(ovl_base + OVL_LAYER0_CFG + offset, layer_cfg_val, 32, 0);

			if (g_debug_ovl_optimized) {
				HISI_FB_INFO("ovl src%d: layer_cfg_val(0x%x), layer_info_alpha(0x%x), "
					"layer_info_srccolor(0x%x), layer_pattern(0x%x).\n",
					i, layer_cfg_val, layer_info_alpha_val, layer_info_srccolor_val, layer_pattern);
			}
		}
	}

	return 0;
}

int hisi_dss_check_pure_layer(struct hisi_fb_data_type *hisifd, void __user *argp)
{
	int ret = 0;
	dss_pure_layer_check_t check_req;
	uint32_t layer_info_alpha_val = 0;
	uint32_t layer_info_srccolor_val = 0;
	char __iomem *ovl_base = NULL;
	int32_t layer_index = 0;
	int offset = 0;

	BUG_ON(hisifd == NULL);

	ret = copy_from_user(&check_req, argp, sizeof(check_req));
	if (ret) {
		HISI_FB_ERR("copy_from_user failed!\n");
		return -EINVAL;
	}

	ovl_base = hisifd->dss_base +
		g_dss_module_ovl_base[hisifd->ovl_type][MODULE_OVL_BASE];
	layer_index = check_req.layer_index;
	offset= layer_index * 0x2C;

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_DEBUG("fb%d panel is power off!", hisifd->index);
		ret =-EINVAL;
		goto err_out;
	}

	if (layer_index < 0 || layer_index >= OVL_LAYER_NUM) {
		HISI_FB_ERR("fb%d, invalid layer index: %d", hisifd->index, layer_index);
		ret = -EINVAL;
		goto err_out;
	}

	hisifb_activate_vsync(hisifd);

	layer_info_alpha_val = inp32(ovl_base + OVL_LAYER0_INFO_ALPHA + offset);
	layer_info_srccolor_val = inp32(ovl_base + OVL_LAYER0_INFO_SRCCOLOR + offset);

	check_req.alpha_flag = (layer_info_alpha_val & BIT_OVL_LAYER_SRCALPHA_FLAG) ? 1 : 0;
	check_req.color_flag = (layer_info_srccolor_val & BIT_OVL_LAYER_SRCCOLOR_FLAG) ? 1 : 0;
	check_req.color = ((layer_info_alpha_val << 8) & 0xFF000000) | ((layer_info_srccolor_val >> 8) & 0x00FFFFFF);

	hisifb_deactivate_vsync(hisifd);

	ret = copy_to_user(argp, &check_req, sizeof(check_req));
	if (ret) {
		HISI_FB_ERR("copy_to_user failed!\n");
		ret = -EINVAL;
		goto err_out;
	}

err_out:
	up(&hisifd->blank_sem);

	return 0;
}


/*******************************************************************************
** DSS MUX
*/
int hisi_dss_mux_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req, dss_layer_t *layer)
{
	int tmp_val = 0;
	bool is_yuv_semi_planar = false;
	bool is_yuv_planar = false;
	int tmp_chn_idx = 0;

	int layer_idx = 0;
	int chn_idx = 0;
	uint32_t need_cap = 0;
	uint32_t flags = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	layer_idx = layer->layer_idx;
	chn_idx = layer->chn_idx;
	need_cap = layer->need_cap;
	flags = layer->flags;
	is_yuv_semi_planar = isYUVSemiPlanar(layer->src.format);
	is_yuv_planar = isYUVPlanar(layer->src.format);

	/* handle channel ctrl and channel scene */
	if (hisifd->dss_glb.chn_ctl_base[chn_idx]) {
		if (is_yuv_semi_planar || is_yuv_planar) {
			hisifd->dss_glb.chn_ctl_val[chn_idx] =
				set_bits32(hisifd->dss_glb.chn_ctl_val[chn_idx], 0x3, 2, 0);
			hisifd->dss_glb.chn_ctl_used[chn_idx] = 1;
		} else {
			hisifd->dss_glb.chn_ctl_val[chn_idx] =
				set_bits32(hisifd->dss_glb.chn_ctl_val[chn_idx], 0x1, 1, 0);
			hisifd->dss_glb.chn_ctl_used[chn_idx] = 1;
		}
		hisifd->dss_glb.chn_ctl_val[chn_idx] =
			set_bits32(hisifd->dss_glb.chn_ctl_val[chn_idx], layer->flags, 3, 4);
		hisifd->dss_glb.chn_ctl_used[chn_idx] = 1;

		if (is_yuv_planar && (chn_idx == DPE3_CHN0)) {
			tmp_chn_idx = chn_idx + 1;
			if (hisifd->dss_glb.chn_ctl_base[tmp_chn_idx]) {
				hisifd->dss_glb.chn_ctl_val[tmp_chn_idx] =
					set_bits32(hisifd->dss_glb.chn_ctl_val[tmp_chn_idx], 0x1, 1, 0);
				hisifd->dss_glb.chn_ctl_val[tmp_chn_idx] =
					set_bits32(hisifd->dss_glb.chn_ctl_val[tmp_chn_idx], layer->flags, 3, 4);
				hisifd->dss_glb.chn_ctl_used[tmp_chn_idx] = 1;
			} else {
				HISI_FB_ERR("layer_idx=%d, chn_idx=%d, chn_ctl_base is null!\n",
					layer_idx, tmp_chn_idx);
			}
		}
	}

#if 0
	/* OVL MUX */
	if (need_cap & (CAP_FB_DMA | CAP_FB_POST)) {
		set_reg(ovl_mux_base, 0x0, 2, 0);
	} else {
		set_reg(ovl_mux_base, 0x5, 4, 0);
	}
#endif

	switch (chn_idx) {
	case DPE0_CHN0:
	case DPE0_CHN1:
		{
			/* CROSS SWITCH */
			if (hisifd->dss_glb.dpe0_cross_switch_base) {
				if (need_cap & CAP_CROSS_SWITCH) {
					hisifd->dss_glb.dpe0_cross_switch_val =
						set_bits32(hisifd->dss_glb.dpe0_cross_switch_val, 0x1, 1, 0);
					hisifd->dss_glb.dpe0_cross_switch_used = 1;
				} else {
					hisifd->dss_glb.dpe0_cross_switch_val =
						set_bits32(hisifd->dss_glb.dpe0_cross_switch_val, 0x0, 1, 0);
					hisifd->dss_glb.dpe0_cross_switch_used = 1;
				}
			} else {
				HISI_FB_ERR("chn_idx(%d), dpe0_cross_switch_base is invalid!\n", chn_idx);
			}

			/* CHN MUX */
			if (hisifd->dss_glb.chn_mux_base[chn_idx]) {
				/* 0:to ov0 1:to wbe0 dma0 */
				if (pov_req->wb_enable &&
					(pov_req->wb_layer_info.chn_idx == WBE0_CHN0)) {
					hisifd->dss_glb.chn_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.chn_mux_val[chn_idx], 0x1, 1, 0);
					hisifd->dss_glb.chn_mux_used[chn_idx] = 1;
				} else {
					hisifd->dss_glb.chn_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.chn_mux_val[chn_idx], 0x0, 1, 0);
					hisifd->dss_glb.chn_mux_used[chn_idx] = 1;
				}
			} else {
				HISI_FB_ERR("layer_idx(%d), chn_mux_base is NULL!\n", layer_idx);
			}
		}
		break;
	case DPE0_CHN2:
	case DPE0_CHN3:
		break;

	/* bg layer */
	case DPE0_CHN4:
		break;

	case DPE1_CHN0:
		{
			/* CHN MUX */
			if (hisifd->dss_glb.chn_mux_base[chn_idx]) {
				/* 0:to ov1 1:to wbe0 dma1 */
				if (pov_req->wb_enable &&
					(pov_req->wb_layer_info.chn_idx == WBE0_CHN1)) {
					hisifd->dss_glb.chn_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.chn_mux_val[chn_idx], 0x1, 1, 0);
					hisifd->dss_glb.chn_mux_used[chn_idx] = 1;
				} else {
					hisifd->dss_glb.chn_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.chn_mux_val[chn_idx], 0x0, 1, 0);
					hisifd->dss_glb.chn_mux_used[chn_idx] = 1;
				}
			} else {
				HISI_FB_ERR("layer_idx(%d), chn_mux_base is NULL!\n", layer_idx);
			}

			/* SCF MUX */
			if (need_cap & CAP_SCL) {
				if (hisifd->dss_glb.scf_mux_base[chn_idx]) {
					/* 1:WBE0, 2:DPE1 */
					hisifd->dss_glb.scf_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.scf_mux_val[chn_idx], 0x2, 2, 0);
					hisifd->dss_glb.scf_mux_used[chn_idx] = 1;
				} else {
					HISI_FB_ERR("layer_idx(%d), scf_mux_base is NULL!\n", layer_idx);
				}
			}
		}
		break;
	case DPE1_CHN1:
		break;

	/* bg layer */
	case DPE1_CHN2:
		break;

	case DPE2_CHN0:
		{
			/* ROT mux */
			if (need_cap & CAP_ROT) {
				if (hisifd->dss_glb.rot_mux_base[chn_idx]) {
					/* 0: DPE2; 1:WBE0 */
					hisifd->dss_glb.rot_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.rot_mux_val[chn_idx], 0x0, 1, 0);
					hisifd->dss_glb.rot_mux_used[chn_idx] = 1;
				} else {
					HISI_FB_ERR("layer_idx(%d), rot_mux_base is NULL!\n", layer_idx);
				}
			}
		}
		//break;
	case DPE2_CHN1:
		{
			/* CROSS SWITCH */
			if (hisifd->dss_glb.dpe2_cross_switch_base) {
				if (need_cap & CAP_CROSS_SWITCH) {
					hisifd->dss_glb.dpe2_cross_switch_val =
						set_bits32(hisifd->dss_glb.dpe2_cross_switch_val, 0x1, 1, 0);
					hisifd->dss_glb.dpe2_cross_switch_used = 1;
				} else {
					hisifd->dss_glb.dpe2_cross_switch_val =
						set_bits32(hisifd->dss_glb.dpe2_cross_switch_val, 0x0, 1, 0);
					hisifd->dss_glb.dpe2_cross_switch_used = 1;
				}
			} else {
				HISI_FB_ERR("chn_idx(%d), dpe2_cross_switch_base is invalid!\n", chn_idx);
			}
		}
		//break;
	case DPE2_CHN2:
	case DPE2_CHN3:
		{
			/* CHN MUX */
			tmp_chn_idx = chn_idx;
			if (need_cap & CAP_CROSS_SWITCH) {
				if (tmp_chn_idx == DPE2_CHN0) {
					tmp_chn_idx = DPE2_CHN1;
				} else if (tmp_chn_idx == DPE2_CHN1) {
					tmp_chn_idx = DPE2_CHN0;
				} else {
					; /* do nothing */
				}
			}

			if (hisifd->dss_glb.chn_mux_base[tmp_chn_idx]) {
				/* 0:to pdp 1:to sdp 2:reserved 3:to adp */
				if (hisifd->ovl_type == DSS_OVL_PDP) {
					tmp_val = 0x0;
				} else if (hisifd->ovl_type == DSS_OVL_SDP) {
					tmp_val = 0x1;
				} else if (hisifd->ovl_type == DSS_OVL_ADP) {
					tmp_val = 0x3;
				} else {
					HISI_FB_ERR("layer_idx(%d): not support this ovl_type(%d)!\n",
						layer_idx, hisifd->ovl_type);
					tmp_val = -1;
				}

				if (tmp_val >= 0) {
					hisifd->dss_glb.chn_mux_val[tmp_chn_idx] =
						set_bits32(hisifd->dss_glb.chn_mux_val[tmp_chn_idx], tmp_val, 2, 0);
					hisifd->dss_glb.chn_mux_used[tmp_chn_idx] = 1;
				}
			} else {
				HISI_FB_ERR("layer_idx(%d), chn_mux_base is NULL!\n", layer_idx);
			}
		}
		break;

	case DPE3_CHN0:
		{
			/* CHN MUX */
			if (hisifd->dss_glb.chn_mux_base[chn_idx]) {
				/* 0:to ov2 1:to wbe1_dma0 */
				if (pov_req->wb_enable &&
					(pov_req->wb_layer_info.chn_idx == WBE1_CHN0)) {
					hisifd->dss_glb.chn_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.chn_mux_val[chn_idx], 0x1, 1, 0);
					hisifd->dss_glb.chn_mux_used[chn_idx] = 1;
				} else {
					hisifd->dss_glb.chn_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.chn_mux_val[chn_idx], 0x0, 1, 0);
					hisifd->dss_glb.chn_mux_used[chn_idx] = 1;
				}
			} else {
				HISI_FB_ERR("layer_idx(%d), chn_mux_base is NULL!\n", layer_idx);
			}
		}
		break;
	case DPE3_CHN1:
		{
			/* ROT mux */
			if (need_cap & CAP_ROT) {
				/* ROT mux */
				if (hisifd->dss_glb.rot_mux_base[chn_idx]) {
					/* 0:DPE3 1:WBE1 */
					hisifd->dss_glb.rot_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.rot_mux_val[chn_idx], 0x0, 1, 0);
					hisifd->dss_glb.rot_mux_used[chn_idx] = 1;
				} else {
					HISI_FB_ERR("layer_idx(%d), rot_mux_base is NULL!\n", layer_idx);
				}
			}

			/* SCF mux */
			if (need_cap & CAP_SCL) {
				if (hisifd->dss_glb.scf_mux_base[chn_idx]) {
					/* 1:WBE1, 2:DPE3 */
					hisifd->dss_glb.scf_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.scf_mux_val[chn_idx], 0x2, 2, 0);
					hisifd->dss_glb.scf_mux_used[chn_idx] = 1;
				} else {
					HISI_FB_ERR("layer_idx(%d), scf_mux_base is NULL!\n", layer_idx);
				}
			}
		}
		break;
	case DPE3_CHN2:
	case DPE3_CHN3:
		break;

	default:
		HISI_FB_ERR("not support this chn(%d)!\n", chn_idx);
		break;
	}

	return 0;
}

int hisi_dss_wbe_mux_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req, dss_wb_layer_t *layer)
{
	int tmp_val = 0;
	int chn_idx = 0;
	uint32_t need_cap = 0;
	uint8_t ovl_type = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON(layer == NULL);

	chn_idx = layer->chn_idx;
	need_cap = layer->need_cap;
	ovl_type = hisifd->ovl_type;

	/*
	** handle glb: OVL_MUX
	** OV0_MUX
	** OV1_MUX
	** OV2_MUX
	**
	** handle glb:
	** WBE0_MUX: 0:ov0 offline 1:pdp dfs write back 2:sdp
	** SDP_MUX: 0:dpe1 chn0; 2:sdp dfs write back
	*/
	if (ovl_type == DSS_OVL_PDP) {
		if (hisifd->dss_glb.ovl_mux_base) {
			hisifd->dss_glb.ovl_mux_val =
				set_bits32(hisifd->dss_glb.ovl_mux_val, (0x1 | (0x1 << 2)), 4, 0);
			hisifd->dss_glb.ovl_mux_used = 1;
		}

		if (hisifd->dss_glb.chn_mux_base[chn_idx]) {
			if (pov_req->ovl_flags == DSS_SCENE_WBE0_WO_OFFLINE) {
				tmp_val = 0x0;
			} else if (pov_req->ovl_flags == DSS_SCENE_PDP_ONLINE) {
				tmp_val = 0x1;
			} else {
				tmp_val = 0x1;
			}

			hisifd->dss_glb.chn_mux_val[chn_idx] =
				set_bits32(hisifd->dss_glb.chn_mux_val[chn_idx], tmp_val, 2, 0);
			hisifd->dss_glb.chn_mux_used[chn_idx] = 1;
		} else {
			HISI_FB_ERR("ovl_type(%d), WBE0_MUX is invalid!\n", ovl_type);
		}
	} else if (ovl_type == DSS_OVL_SDP) {
		if (hisifd->dss_glb.ovl_mux_base) {
			hisifd->dss_glb.ovl_mux_val =
				set_bits32(hisifd->dss_glb.ovl_mux_val, (0x1 | (0x1 << 2)), 3, 0);
			hisifd->dss_glb.ovl_mux_used = 1;
		}

		if (hisifd->dss_glb.chn_mux_base[chn_idx]) {
			hisifd->dss_glb.chn_mux_val[chn_idx] =
				set_bits32(hisifd->dss_glb.chn_mux_val[chn_idx], 0x2, 2, 0);
			hisifd->dss_glb.chn_mux_used[chn_idx] = 1;
		} else {
			HISI_FB_ERR("ovl_type(%d), WBE0_MUX is invalid!\n", ovl_type);
		}

		if (hisifd->dss_glb.sdp_mux_base) {
			hisifd->dss_glb.sdp_mux_val =
				set_bits32(hisifd->dss_glb.sdp_mux_val, 0x2, 2, 0);
			hisifd->dss_glb.sdp_mux_used = 1;
		} else {
			HISI_FB_ERR("ovl_type(%d), SDP_MUX is invalid!\n", ovl_type);
		}
	} else if (ovl_type == DSS_OVL_ADP) {
		if (hisifd->dss_glb.ovl_mux_base) {
			hisifd->dss_glb.ovl_mux_val =
				set_bits32(hisifd->dss_glb.ovl_mux_val, 0x1, 1, 3);
			hisifd->dss_glb.ovl_mux_used = 1;
		}
	} else {
		HISI_FB_ERR("ovl_mux, ovl_type(%d) is invalid!\n", ovl_type);
	}

	/*
	** handle glb: OVL_SCENE
	** DSS_GLB_OV0_SCENE
	** DSS_GLB_OV1_SCENE
	** DSS_GLB_OV2_SCENE
	*/
	if (hisifd->dss_glb.ovl_scene_base) {
		hisifd->dss_glb.ovl_scene_val =
			set_bits32(hisifd->dss_glb.ovl_scene_val, pov_req->ovl_flags, 3, 0);
		hisifd->dss_glb.ovl_scene_used = 1;
	} else {
		HISI_FB_ERR("ovl_type(%d), ovl_scene_base is invalid!\n", ovl_type);
	}

	/*
	** handle glb: CHN_CTL and CHN_SCENE
	** WBE0_CH0_CTL WBE0_CH1_CTL
	** WBE1_CH0_CTL WBE1_CH1_CTL
	*/
	if (hisifd->dss_glb.chn_ctl_base[chn_idx]) {
		hisifd->dss_glb.chn_ctl_val[chn_idx] =
			set_bits32(hisifd->dss_glb.chn_ctl_val[chn_idx], 0x1, 1, 0);
		hisifd->dss_glb.chn_ctl_val[chn_idx] =
			set_bits32(hisifd->dss_glb.chn_ctl_val[chn_idx], layer->flags, 3, 4);
		hisifd->dss_glb.chn_ctl_used[chn_idx] = 1;
	} else {
		HISI_FB_ERR("chn_idx(%d), chn_ctl_base is NULL!\n", chn_idx);
	}

	switch (chn_idx) {
	case WBE0_CHN0:
		break;
	case WBE0_CHN1:
		{
			/*
			** handle glb: ROT mux
			** 0: DPE2; 1:WBE0
			*/
			if (need_cap & CAP_ROT) {
				if (hisifd->dss_glb.rot_mux_base[chn_idx]) {
					/* 0:DPE3 1:WBE1 */
					hisifd->dss_glb.rot_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.rot_mux_val[chn_idx], 0x1, 1, 0);
					hisifd->dss_glb.rot_mux_used[chn_idx] = 1;
				} else {
					HISI_FB_ERR("chn_idx(%d), rot_mux_base is null!\n", chn_idx);
				}
			}

			/*
			** handle glb: SCF mux
			** 1:WBE0, 2:DPE1
			*/
			if (need_cap & CAP_SCL) {
				if (hisifd->dss_glb.scf_mux_base[chn_idx]) {
					/* 1:WBE1, 2:DPE3 */
					hisifd->dss_glb.scf_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.scf_mux_val[chn_idx], 0x1, 2, 0);
					hisifd->dss_glb.scf_mux_used[chn_idx] = 1;
				} else {
					HISI_FB_ERR("chn_idx(%d), scf_mux_base is null!\n", chn_idx);
				}
			}
		}
		break;
	case WBE1_CHN0:
		break;
	case WBE1_CHN1:
		{
			/*
			** handle glb: ROT mux
			** 0:DPE3 1:WBE1
			*/
			if (need_cap & CAP_ROT) {
				if (hisifd->dss_glb.rot_mux_base[chn_idx]) {
					/* 0:DPE3 1:WBE1 */
					hisifd->dss_glb.rot_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.rot_mux_val[chn_idx], 0x1, 1, 0);
					hisifd->dss_glb.rot_mux_used[chn_idx] = 1;
				} else {
					HISI_FB_ERR("chn_idx(%d), rot_mux_base is null!\n", chn_idx);
				}
			}

			/*
			** handle glb: SCF mux
			** 1:WBE1, 2:DPE3
			*/
			if (need_cap & CAP_SCL) {
				if (hisifd->dss_glb.scf_mux_base[chn_idx]) {
					/* 1:WBE1, 2:DPE3 */
					hisifd->dss_glb.scf_mux_val[chn_idx] =
						set_bits32(hisifd->dss_glb.scf_mux_val[chn_idx], 0x1, 2, 0);
					hisifd->dss_glb.scf_mux_used[chn_idx] = 1;
				} else {
					HISI_FB_ERR("chn_idx(%d), scf_mux_base is null!\n", chn_idx);
				}
			}
		}
		break;
	default:
		HISI_FB_ERR("not support this chn_idx(%d)!", chn_idx);
		break;
	}

	return 0;
}


/*******************************************************************************
** DSS SBL
*/
static void hisi_dss_sbl_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *sbl_base, dss_sbl_t *s_sbl)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(sbl_base == NULL);
	BUG_ON(s_sbl == NULL);

	hisifd->set_reg(hisifd, sbl_base + SBL_BACKLIGHT_L, s_sbl->sbl_backlight_l, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_BACKLIGHT_H, s_sbl->sbl_backlight_h, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_AMBIENT_LIGHT_L, s_sbl->sbl_ambient_light_l, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_AMBIENT_LIGHT_H, s_sbl->sbl_ambient_light_h, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_CALIBRATION_A_L, s_sbl->sbl_calibration_a_l, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_CALIBRATION_A_H, s_sbl->sbl_calibration_a_h, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_CALIBRATION_B_L, s_sbl->sbl_calibration_b_l, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_CALIBRATION_B_H, s_sbl->sbl_calibration_b_h, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_CALIBRATION_C_L, s_sbl->sbl_calibration_c_l, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_CALIBRATION_C_H, s_sbl->sbl_calibration_c_h, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_CALIBRATION_D_L, s_sbl->sbl_calibration_d_l, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_CALIBRATION_D_H, s_sbl->sbl_calibration_d_h, 8, 0);
	hisifd->set_reg(hisifd, sbl_base + SBL_ENABLE, s_sbl->sbl_enable, 1, 0);

	hisifd->sbl_enable = (s_sbl->sbl_enable > 0) ? 1 : 0;
}


/*******************************************************************************
** DSS WDMA BRIDGE
*/
static void hisi_dss_wdma_bridge_init(char __iomem *wdma_bridge_base,
	dss_wdma_bridge_t *s_wdma_bridge)
{
	BUG_ON(wdma_bridge_base == NULL);
	BUG_ON(s_wdma_bridge == NULL);

	memset(s_wdma_bridge, 0, sizeof(dss_wdma_bridge_t));

	s_wdma_bridge->wbrg_comm_ctl =
		inp32(wdma_bridge_base + WBRG_COMM_CTL);
	s_wdma_bridge->wbrg_ctl0 =
		inp32(wdma_bridge_base + WBRG_CTL0);
	s_wdma_bridge->wbrg_ctl1 =
		inp32(wdma_bridge_base + WBRG_CTL1);
	s_wdma_bridge->wbrg_wo_ch_ctl =
		inp32(wdma_bridge_base + WBRG_WO_CH_CTL);
	s_wdma_bridge->wbrg_ws_ch_ctl =
		inp32(wdma_bridge_base + WBRG_WS_CH_CTL);
}

static void hisi_dss_wdma_bridge_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *wdma_bridge_base, dss_wdma_bridge_t *s_wdma_bridge)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(wdma_bridge_base == NULL);
	BUG_ON(s_wdma_bridge == NULL);

	/* FIXME: not reload */
	hisifd->set_reg(hisifd, wdma_bridge_base + WBRG_COMM_CTL,
		s_wdma_bridge->wbrg_comm_ctl, 32, 0);
	/* FIXME: not reload */
	hisifd->set_reg(hisifd, wdma_bridge_base + WBRG_CTL0,
		s_wdma_bridge->wbrg_ctl0, 32, 0);
	/* FIXME: not reload */
	hisifd->set_reg(hisifd, wdma_bridge_base + WBRG_CTL1,
		s_wdma_bridge->wbrg_ctl1, 32, 0);

	hisifd->set_reg(hisifd, wdma_bridge_base + WBRG_WO_CH_CTL,
		s_wdma_bridge->wbrg_wo_ch_ctl , 32, 0);
	hisifd->set_reg(hisifd, wdma_bridge_base + WBRG_WS_CH_CTL,
		s_wdma_bridge->wbrg_ws_ch_ctl, 32, 0);
}

int hisi_set_dpe_wdma_bridge(struct hisi_fb_data_type *hisifd, int dpe_type, dss_overlay_t *pov_req)
{
	dss_wdma_bridge_t *wdma_bridge = NULL;
	dss_wb_layer_t *wb_layer = NULL;
	int chn_idx = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON((dpe_type < 0 || dpe_type >= DSS_ENG_MAX));

	wdma_bridge = &(hisifd->dss_module.wdma_bridge[dpe_type]);
	hisifd->dss_module.wdma_bridge_used[dpe_type] = 1;

	wb_layer = &(pov_req->wb_layer_info);
	chn_idx = wb_layer->chn_idx;

	if ((chn_idx == WBE0_CHN0) || (chn_idx == WBE1_CHN0)) {
		wdma_bridge->wbrg_comm_ctl = set_bits32(wdma_bridge->wbrg_comm_ctl, 0x1, 1, 4);
		wdma_bridge->wbrg_comm_ctl = set_bits32(wdma_bridge->wbrg_comm_ctl, 0x1, 1, 5);
		wdma_bridge->wbrg_ctl0 = set_bits32(wdma_bridge->wbrg_ctl0, 0x1, 1, 0);
		wdma_bridge->wbrg_ctl0 = set_bits32(wdma_bridge->wbrg_ctl0, 0x7, 4, 1);
		wdma_bridge->wbrg_ctl0 = set_bits32(wdma_bridge->wbrg_ctl0, 0x1, 1, 5);
		wdma_bridge->wbrg_ws_ch_ctl = set_bits32(wdma_bridge->wbrg_ws_ch_ctl, AXI_CH0, 1, 0);
		if (wb_layer->transform & HISI_FB_TRANSFORM_ROT_90)
			wdma_bridge->wbrg_ws_ch_ctl = set_bits32(wdma_bridge->wbrg_ws_ch_ctl, 0x8, 5, 7);
		else
			wdma_bridge->wbrg_ws_ch_ctl = set_bits32(wdma_bridge->wbrg_ws_ch_ctl, 0x3, 5, 7);
	} else if ((chn_idx == WBE0_CHN1) || (chn_idx == WBE1_CHN1)) {
		wdma_bridge->wbrg_comm_ctl = set_bits32(wdma_bridge->wbrg_comm_ctl, 0x1, 1, 2);
		wdma_bridge->wbrg_comm_ctl = set_bits32(wdma_bridge->wbrg_comm_ctl, 0x1, 1, 3);
		wdma_bridge->wbrg_ctl1 = set_bits32(wdma_bridge->wbrg_ctl1, 0x1, 1, 0);
		wdma_bridge->wbrg_ctl1 = set_bits32(wdma_bridge->wbrg_ctl1, 0x7, 4, 1);
		wdma_bridge->wbrg_ctl1 = set_bits32(wdma_bridge->wbrg_ctl1, 0x1, 1, 5);
		wdma_bridge->wbrg_wo_ch_ctl = set_bits32(wdma_bridge->wbrg_wo_ch_ctl, AXI_CH1, 1, 0);
		if (wb_layer->transform & HISI_FB_TRANSFORM_ROT_90)
			wdma_bridge->wbrg_wo_ch_ctl = set_bits32(wdma_bridge->wbrg_wo_ch_ctl, 0x8, 5, 7);
		else
			wdma_bridge->wbrg_wo_ch_ctl = set_bits32(wdma_bridge->wbrg_wo_ch_ctl, 0x3, 5, 7);
	} else {
		HISI_FB_ERR("chn_idx=%d is invalid!\n", chn_idx);
		return -EINVAL;
	}

	return 0;
}

int hisi_dss_wdma_bridge_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req)
{
	BUG_ON(hisifd == NULL);

	if (hisifd->ovl_type == DSS_OVL_PDP) {
		if (hisi_set_dpe_wdma_bridge(hisifd, DSS_ENG_WBE0, pov_req)) {
			HISI_FB_ERR("hisi_set_dpe_wdma_bridge for DSS_WBE0 failed!\n");
			return -EINVAL;
		}
	} else if (hisifd->ovl_type == DSS_OVL_SDP) {
		if (hisi_set_dpe_wdma_bridge(hisifd, DSS_ENG_WBE0, pov_req)) {
			HISI_FB_ERR("hisi_set_dpe_wdma_bridge for DSS_WBE0 failed!\n");
			return -EINVAL;
		}
	} else if (hisifd->ovl_type == DSS_OVL_ADP) {
		if (hisi_set_dpe_wdma_bridge(hisifd, DSS_ENG_WBE1, pov_req)) {
			HISI_FB_ERR("hisi_set_dpe_wdma_bridge for DSS_WBE1 failed!\n");
			return -EINVAL;
		}
	} else {
		HISI_FB_ERR("not support this ovl_type %d \n", hisifd->ovl_type);
		return -EINVAL;
	}

	return 0;
}


/*******************************************************************************
** DSS WDMA
*/
static uint32_t hisi_calculate_display_addr_wb(bool mmu_enable,
	dss_wb_layer_t *wb_layer, dss_rect_t aligned_rect)
{
	uint32_t addr = 0;
	uint32_t dst_addr = 0;
	int left = 0, top = 0, right = 0, bottom = 0;
	int bpp = 0;

	left = aligned_rect.x;
	top = aligned_rect.y;
	right = DSS_WIDTH(left + aligned_rect.w);
	bottom = DSS_HEIGHT(top + aligned_rect.h);

	dst_addr = mmu_enable ? wb_layer->dst.vir_addr : wb_layer->dst.phy_addr;
	bpp = wb_layer->dst.bpp;

	switch(wb_layer->transform) {
	case HISI_FB_TRANSFORM_NOP:
		addr = dst_addr + top * wb_layer->dst.stride + left * bpp;
		break;
	case HISI_FB_TRANSFORM_FLIP_H:
		addr = dst_addr + top * wb_layer->dst.stride + right * bpp;
		break;
	case HISI_FB_TRANSFORM_FLIP_V:
		addr = dst_addr +
			(aligned_rect.h - 1) * wb_layer->dst.stride + left * bpp;
		break;
	case HISI_FB_TRANSFORM_ROT_90:
		addr = dst_addr + top * wb_layer->dst.stride + right * bpp;
		break;
	case HISI_FB_TRANSFORM_ROT_180:
		addr = dst_addr +
			bottom * wb_layer->dst.stride + right * bpp;
		break;
	case HISI_FB_TRANSFORM_ROT_270:
		addr = dst_addr +
			(aligned_rect.h - 1) * wb_layer->dst.stride + left * bpp;
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H):
		addr = dst_addr +
			bottom * wb_layer->dst.stride + right * bpp;
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V):
		addr = dst_addr + top * wb_layer->dst.stride + left * bpp;
		break;
	default:
		addr = dst_addr + top * wb_layer->dst.stride + left * bpp;
		break;
	}

	return addr;
}

static void hisi_dss_wdma_init(char __iomem *wdma_base, dss_wdma_t *s_wdma)
{
	BUG_ON(wdma_base == NULL);
	BUG_ON(s_wdma == NULL);

	memset(s_wdma, 0, sizeof(dss_wdma_t));

	s_wdma->wdma_data_addr = inp32(wdma_base + WDMA_DATA_ADDR);
	s_wdma->wdma_oft_x0 = inp32(wdma_base + WDMA_OFT_X0);
	s_wdma->wdma_oft_y0 = inp32(wdma_base + WDMA_OFT_Y0);
	s_wdma->wdma_rot_qos_lev = inp32(wdma_base + WDMA_ROT_QOS_LEV);
	s_wdma->wdma_stride = inp32(wdma_base + WDMA_STRIDE);
	s_wdma->wdma_oft_x1 = inp32(wdma_base + WDMA_OFT_X1);
	s_wdma->wdma_oft_y1 = inp32(wdma_base + WDMA_OFT_Y1);
	s_wdma->wdma_ctrl = inp32(wdma_base + WDMA_CTRL);
	s_wdma->wdma_data_num = inp32(wdma_base + WDMA_DATA_NUM);
	s_wdma->wdma_tile_scram = inp32(wdma_base + WDMA_TILE_SCRAM);
	s_wdma->wdma_sw_mask_en = inp32(wdma_base + WDMA_SW_MASK_EN);
	s_wdma->wdma_hdr_start_addr = inp32(wdma_base + WDMA_HDR_START_ADDR);
	s_wdma->wdma_hdr_stride = inp32(wdma_base + WDMA_HDR_STRIDE);
	s_wdma->wdma_start_mask = inp32(wdma_base + WDMA_START_MASK);
	s_wdma->wdma_end_mask = inp32(wdma_base + WDMA_END_MASK);
	s_wdma->wdma_wbrg_credit_ctl = inp32(wdma_base + WDMA_WBRG_CREDIT_CTL);
}

static void hisi_dss_wdma_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *wdma_base, dss_wdma_t *s_wdma)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(wdma_base == NULL);
	BUG_ON(s_wdma == NULL);

	hisifd->set_reg(hisifd, wdma_base + WDMA_DATA_ADDR, s_wdma->wdma_data_addr, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_OFT_X0, s_wdma->wdma_oft_x0, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_OFT_Y0, s_wdma->wdma_oft_y0, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_ROT_QOS_LEV, s_wdma->wdma_rot_qos_lev, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_STRIDE, s_wdma->wdma_stride, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_OFT_X1, s_wdma->wdma_oft_x1, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_OFT_Y1, s_wdma->wdma_oft_y1, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_CTRL, s_wdma->wdma_ctrl, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_DATA_NUM, s_wdma->wdma_data_num, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_TILE_SCRAM, s_wdma->wdma_tile_scram, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_SW_MASK_EN, s_wdma->wdma_sw_mask_en, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_HDR_START_ADDR, s_wdma->wdma_hdr_start_addr, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_HDR_STRIDE, s_wdma->wdma_hdr_stride, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_START_MASK, s_wdma->wdma_start_mask, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_END_MASK, s_wdma->wdma_end_mask, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + WDMA_WBRG_CREDIT_CTL, s_wdma->wdma_wbrg_credit_ctl, 32, 0);
}

int hisi_dss_wdma_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req, dss_wb_layer_t *layer,
	dss_rect_t aligned_rect, dss_rect_t *block_rect)
{
	dss_wdma_t *wdma = NULL;
	int chn_idx = 0;
	int wdma_format = 0;
	int wdma_transform = 0;
	uint32_t wdma_oft_x0 = 0;
	uint32_t wdma_oft_x1 = 0;
	uint32_t wdma_oft_y0 = 0;
	uint32_t wdma_oft_y1 = 0;
	uint32_t wdma_data_num = 0;
	uint32_t wdma_addr = 0;
	uint32_t wdma_stride = 0;

	int wbrg_credit_qos_en = 1;
	int wbrg_credit_shaper = 0x20;
	int wbrg_credit_lower = 0x7F;
	int wbrg_credit_upper = 0xFF;

	uint32_t mask_left = 0;
	uint32_t mask_right = 0;

	dss_rect_t in_rect;
	int temp = 0;
	int aligned_pixel = 0;
	int l2t_interleave_n = 0;
	bool mmu_enable = false;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON(layer == NULL);

	chn_idx = layer->chn_idx;

	wdma = &(hisifd->dss_module.wdma[chn_idx]);
	hisifd->dss_module.wdma_used[chn_idx] = 1;

	wdma_format = hisi_pixel_format_hal2dma(layer->dst.format);
	if (wdma_format < 0) {
		HISI_FB_ERR("hisi_pixel_format_hal2dma failed!\n");
		return -EINVAL;
	}

	wdma_transform = hisi_transform_hal2dma(layer->transform, chn_idx);
	if (wdma_transform < 0) {
		HISI_FB_ERR("hisi_transform_hal2dma failed!\n");
		return -EINVAL;
	}

	if (layer->dst.is_tile) {
		l2t_interleave_n = hisi_get_rdma_tile_interleave(layer->dst.stride);
		if (l2t_interleave_n < MIN_INTERLEAVE) {
			HISI_FB_ERR("tile stride should be 256*2^n, error stride:%d!\n", layer->dst.stride);
			return -EINVAL;
		}
	}

	in_rect = aligned_rect;
	aligned_pixel = DMA_ALIGN_BYTES / layer->dst.bpp;

	if (layer->transform & HISI_FB_TRANSFORM_ROT_90) {
		temp = in_rect.w;
		in_rect.w = in_rect.h;
		in_rect.h = temp;

		temp = layer->dst_rect.h;
		layer->dst_rect.h = layer->dst_rect.w;
		layer->dst_rect.w = temp;
	}

	mmu_enable = (layer->dst.mmu_enable == 1) ? true : false;
	wdma_addr =  hisi_calculate_display_addr_wb(mmu_enable, layer, in_rect) /
		DMA_ALIGN_BYTES;
	wdma_stride = layer->dst.stride / DMA_ALIGN_BYTES;

	wdma_oft_x0 = in_rect.x / aligned_pixel;
	wdma_oft_x1 = DSS_WIDTH(in_rect.x + in_rect.w) / aligned_pixel;
	wdma_oft_y0 = in_rect.y;
	wdma_oft_y1 = DSS_HEIGHT(in_rect.y + in_rect.h);

	wdma_data_num = (wdma_oft_x1 - wdma_oft_x0 + 1) * (wdma_oft_y1- wdma_oft_y0 + 1);

	if (block_rect) {
		if ((aligned_rect.x != block_rect->x) || (aligned_rect.w != block_rect->w)) {
			mask_left = (block_rect->x - aligned_rect.x) * layer->dst.bpp;
			mask_right = (aligned_rect.w - block_rect->w) * layer->dst.bpp - mask_left;
		}
	} else {
		if ((aligned_rect.x != layer->dst_rect.x) || (aligned_rect.w != layer->dst_rect.w)) {
			mask_left = (layer->dst_rect.x - aligned_rect.x) * layer->dst.bpp;
			mask_right = (aligned_rect.w - layer->dst_rect.w) * layer->dst.bpp - mask_left;
		}
	}

	wdma->wdma_data_addr = set_bits32(wdma->wdma_data_addr, wdma_addr, 28, 0);
	wdma->wdma_oft_x0 = set_bits32(wdma->wdma_oft_x0, wdma_oft_x0, 12, 0);
	wdma->wdma_oft_y0 = set_bits32(wdma->wdma_oft_y0, wdma_oft_y0, 16, 0);
	wdma->wdma_rot_qos_lev = set_bits32(wdma->wdma_rot_qos_lev,
		GET_RDMA_ROT_HQOS_REMOVE_LEV(layer->src_rect.w * layer->src_rect.h) |
		(GET_RDMA_ROT_HQOS_ASSERT_LEV(layer->src_rect.w * layer->src_rect.h) << 12), 24, 0);
	wdma->wdma_stride = set_bits32(wdma->wdma_stride, wdma_stride, 13, 0);
	wdma->wdma_oft_x1 = set_bits32(wdma->wdma_oft_x1, wdma_oft_x1, 12, 0);
	wdma->wdma_oft_y1 = set_bits32(wdma->wdma_oft_y1, wdma_oft_y1, 16, 0);
	wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, 0x1, 1, 25);
	wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, wdma_format, 5, 3);
	wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, wdma_transform, 3, 9);
	wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, (mmu_enable ? 0x1 : 0x0), 1, 8);
	wdma->wdma_data_num = set_bits32(wdma->wdma_data_num, wdma_data_num, 30, 0);
	if (layer->dst.is_tile) {
		wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, 0x1, 1, 1);
		wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, l2t_interleave_n, 4, 13);
		wdma->wdma_tile_scram = set_bits32(wdma->wdma_tile_scram, 0x1, 1, 0);
	} else {
		wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, 0x0, 1, 1);
		wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, 0x0, 4, 13);
		wdma->wdma_tile_scram = set_bits32(wdma->wdma_tile_scram, 0x0, 1, 0);
	}
	if (layer->need_cap & CAP_FBC) {
		wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, 0x1, 1, 2);
		wdma->wdma_hdr_start_addr = set_bits32(wdma->wdma_hdr_start_addr,
			pov_req->hdr_start_addr / DMA_ALIGN_BYTES, 28, 0);
		wdma->wdma_hdr_stride = set_bits32(wdma->wdma_hdr_stride,
			pov_req->hdr_stride, 16, 0);
	} else {
		wdma->wdma_ctrl = set_bits32(wdma->wdma_ctrl, 0x0, 1, 2);
		wdma->wdma_hdr_start_addr = set_bits32(wdma->wdma_hdr_start_addr,
			0x0, 28, 0);
		wdma->wdma_hdr_stride = set_bits32(wdma->wdma_hdr_stride,
			0x0, 16, 0);
	}

	if ((mask_left <= 16) && (mask_right <= 16) &&
		(layer->transform == HISI_FB_TRANSFORM_NOP)) {
		wdma->wdma_sw_mask_en = set_bits32(wdma->wdma_sw_mask_en,
			0x1, 1, 0);
		wdma->wdma_start_mask = set_bits32(wdma->wdma_start_mask,
			0xFFFFFFFF << mask_left, 32 , 0);
		wdma->wdma_end_mask = set_bits32(wdma->wdma_end_mask,
			0xFFFFFFFF >> mask_right, 32, 0);
	} else if ((mask_left <= 32) && (mask_right <= 32) &&
		(layer->transform & HISI_FB_TRANSFORM_ROT_90)) {
		wdma->wdma_sw_mask_en = set_bits32(wdma->wdma_sw_mask_en,
			0x1, 1, 0);
		wdma->wdma_start_mask = set_bits32(wdma->wdma_start_mask,
			0xFFFFFFFF << mask_left, 32 , 0);
		wdma->wdma_end_mask = set_bits32(wdma->wdma_end_mask,
			0xFFFFFFFF >> mask_right, 32, 0);
	} else {
		wdma->wdma_sw_mask_en = set_bits32(wdma->wdma_sw_mask_en,
			0x0, 1, 0);
		wdma->wdma_start_mask = set_bits32(wdma->wdma_start_mask,
			0xFFFFFFFF, 32 , 0);
		wdma->wdma_end_mask = set_bits32(wdma->wdma_end_mask,
			0xFFFFFFFF, 32, 0);
	}
	wdma->wdma_wbrg_credit_ctl = set_bits32(wdma->wdma_wbrg_credit_ctl,
		((wbrg_credit_upper << 16) | (wbrg_credit_lower<< 8) |
		(wbrg_credit_shaper<< 1) | wbrg_credit_qos_en), 24, 0);

	return 0;
}


/*******************************************************************************
** DSS GLOBAL
*/
int hisi_dss_module_init(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	memcpy(&(hisifd->dss_glb), &(hisifd->dss_glb_default), sizeof(dss_global_reg_t));
	memcpy(&(hisifd->dss_module), &(hisifd->dss_module_default), sizeof(dss_module_reg_t));

	return 0;
}

int hisi_dss_offline_module_default(struct hisi_fb_data_type *hisifd)
{
	dss_global_reg_t *ovl_glb = NULL;
	dss_module_reg_t *dss_module = NULL;
	uint32_t module_base = 0;
	char __iomem *dss_base = NULL;
	uint8_t ovl_type = 0;
	int i = 0;
	int scf_idx = 0;

	BUG_ON(hisifd == NULL);
	dss_base = hisifd->dss_base;
	BUG_ON(dss_base == NULL);

	ovl_type = hisifd->ovl_type;

	ovl_glb = &(hisifd->dss_glb_default);
	dss_module = &(hisifd->dss_module_default);
	memset(ovl_glb, 0, sizeof(dss_global_reg_t));
	memset(dss_module, 0, sizeof(dss_module_reg_t));

	/**************************************************************************/
	module_base = g_dss_module_ovl_base[ovl_type][MODULE_OVL_MUX];
	if (module_base != 0) {
		ovl_glb->ovl_mux_base = dss_base + module_base;
	}

	module_base = g_dss_module_ovl_base[ovl_type][MODULE_OVL_SCENE];
	if (module_base != 0) {
		ovl_glb->ovl_scene_base = dss_base + module_base;
	}

	for (i = 0; i < ROT_TLB_SCENE_NUM; i++) {
		module_base = DSS_GLB_ROT_TLB0_SCENE + i * 0x4;
		if (module_base != 0) {
			ovl_glb->rot_tlb_scene_base[i] = dss_base + module_base;
		}
	}

	/* DPE2 */
	module_base = DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET + DPE2_SWITCH;
	if (module_base != 0) {
		ovl_glb->dpe2_cross_switch_base = dss_base + module_base;
	}

	if (ovl_type == DSS_OVL_PDP) {
		module_base = DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET + DPE0_SWITCH;
		if (module_base != 0) {
			ovl_glb->dpe0_cross_switch_base = dss_base + module_base;
		}
	} else if (ovl_type == DSS_OVL_SDP) {
		module_base = DSS_DPE1_OFFSET + DSS_DP_CTRL_OFFSET + SDP_MUX;
		if (module_base != 0) {
			ovl_glb->sdp_mux_base = dss_base + module_base;
		}
	} else if (ovl_type == DSS_OVL_ADP) {
		;
	} else {
		HISI_FB_ERR("fb%d not support this ovl_type(%d)!", hisifd->index, ovl_type);
	}

	for (i = DPE0_CHN0; i < DSS_CHN_MAX; i++) {
		if (ovl_type == DSS_OVL_ADP) {
			if ((!isChannelBelongtoDPE(DSS_ENG_DPE3, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_DPE2, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_WBE1, i))) {
				continue;
			}
		} else {
			HISI_FB_ERR("not support this ovl_type(%d)!", ovl_type);
			break;
		}

		module_base = g_dss_module_base[i][MODULE_GLB_CHN_CTL];
		if (module_base != 0) {
			ovl_glb->chn_ctl_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_GLB_CHN_MUX];
		if (module_base != 0) {
			ovl_glb->chn_mux_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_ROT_MUX];
		if (module_base != 0) {
			ovl_glb->rot_mux_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_SCF_MUX];
		if (module_base != 0) {
			ovl_glb->scf_mux_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_ROT_PM_CTL];
		if (module_base != 0) {
			ovl_glb->rot_pm_ctl_base[i] = dss_base + module_base;
		}
	}


	/**************************************************************************/
	for (i = 0; i < DSS_ENG_MAX; i++) {
		if (ovl_type == DSS_OVL_ADP) {
			if ((i != DSS_ENG_DPE3) &&
				(i != DSS_ENG_DPE2) &&
				(i != DSS_ENG_WBE1)) {
				continue;
			}
		} else {
			HISI_FB_ERR("not support this ovl_type(%d)!", ovl_type);
			break;
		}

		module_base = g_dss_module_eng_base[i][MODULE_ENG_DMA_BRG];
		if (module_base != 0) {
			dss_module->rdma_bridge_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_eng_base[i][MODULE_ENG_DMA_BRG];
		if (module_base != 0) {
			dss_module->wdma_bridge_base[i] = dss_base + module_base;
		}
	}

	for (i = DPE0_CHN0; i < DSS_CHN_MAX; i++) {
		if (ovl_type == DSS_OVL_ADP) {
			if ((!isChannelBelongtoDPE(DSS_ENG_DPE3, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_DPE2, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_WBE1, i))) {
				continue;
			}
		} else {
			HISI_FB_ERR("not support this ovl_type(%d)!", ovl_type);
			break;
		}

		module_base = g_dss_module_base[i][MODULE_MMU_DMA0];
		if (module_base != 0) {
			dss_module->mmu_dma0_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_MMU_DMA1];
		if (module_base != 0) {
			dss_module->mmu_dma1_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_DMA0];
		if (module_base != 0) {
			if (i >= DPE0_CHN0 && i <= DPE3_CHN3) {
				dss_module->rdma0_base[i] = dss_base + module_base;
			} else {
				dss_module->wdma_base[i] = dss_base + module_base;
			}
		}

		module_base = g_dss_module_base[i][MODULE_DMA1];
		if (module_base != 0) {
			if (i >= DPE0_CHN0 && i <= DPE3_CHN3) {
				dss_module->rdma1_base[i] = dss_base + module_base;
			}
		}
		module_base = g_dss_module_base[i][MODULE_FBDC];
		if (module_base != 0) {
			dss_module->fbdc_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_DFC];
		if (module_base != 0) {
			dss_module->dfc_base[i] = dss_base + module_base;
		}

		scf_idx = hisi_get_scf_index(i);
		if (scf_idx >= 0) {
			module_base = g_dss_module_base[i][MODULE_SCF];
			if (module_base != 0) {
				dss_module->scf_base[scf_idx] = dss_base + module_base;
			}
		}

		module_base = g_dss_module_base[i][MODULE_SCP];
		if (module_base != 0) {
			dss_module->scp_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_CSC];
		if (module_base != 0) {
			dss_module->csc_base[i] = dss_base + module_base;
		}

		module_base = g_dss_module_base[i][MODULE_FBC];
		if (module_base != 0) {
			dss_module->fbc_base[i] = dss_base + module_base;
		}
	}

	i = ovl_type;
	module_base = g_dss_module_ovl_base[i][MODULE_OVL_BASE];
	if (module_base != 0) {
		dss_module->ov_base[i] = dss_base + module_base;
	}

	module_base = DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET;
	if (module_base != 0) {
		dss_module->mmu_rptb_base = dss_base + module_base;
	}

	return 0;
}

int hisi_dss_module_default(struct hisi_fb_data_type *hisifd)
{
	dss_global_reg_t *ovl_glb = NULL;
	dss_module_reg_t *dss_module = NULL;
	uint32_t module_base = 0;
	char __iomem *dss_base = NULL;
	uint8_t ovl_type = 0;
	int i = 0;
	int scf_idx = 0;

	BUG_ON(hisifd == NULL);
	dss_base = hisifd->dss_base;
	BUG_ON(dss_base == NULL);

	ovl_type = hisifd->ovl_type;

	ovl_glb = &(hisifd->dss_glb_default);
	dss_module = &(hisifd->dss_module_default);
	memset(ovl_glb, 0, sizeof(dss_global_reg_t));
	memset(dss_module, 0, sizeof(dss_module_reg_t));

	/**************************************************************************/
	module_base = g_dss_module_ovl_base[ovl_type][MODULE_OVL_MUX];
	if (module_base != 0) {
		ovl_glb->ovl_mux_base = dss_base + module_base;
		ovl_glb->ovl_mux_val = inp32(ovl_glb->ovl_mux_base);
	}

	module_base = g_dss_module_ovl_base[ovl_type][MODULE_OVL_SCENE];
	if (module_base != 0) {
		ovl_glb->ovl_scene_base = dss_base + module_base;
		ovl_glb->ovl_scene_val = inp32(ovl_glb->ovl_scene_base);
	}

	for (i = 0; i < ROT_TLB_SCENE_NUM; i++) {
		module_base = DSS_GLB_ROT_TLB0_SCENE + i * 0x4;
		if (module_base != 0) {
			ovl_glb->rot_tlb_scene_base[i] = dss_base + module_base;
			ovl_glb->rot_tlb_scene_val[i] = inp32(ovl_glb->rot_tlb_scene_base[i]);
		}
	}

	/* DPE2 */
	module_base = DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET + DPE2_SWITCH;
	if (module_base != 0) {
		ovl_glb->dpe2_cross_switch_base = dss_base + module_base;
		ovl_glb->dpe2_cross_switch_val = inp32(ovl_glb->dpe2_cross_switch_base);
	}

	if (ovl_type == DSS_OVL_PDP) {
		module_base = DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET + DPE0_SWITCH;
		if (module_base != 0) {
			ovl_glb->dpe0_cross_switch_base = dss_base + module_base;
			ovl_glb->dpe0_cross_switch_val = inp32(ovl_glb->dpe0_cross_switch_base);
		}
	} else if (ovl_type == DSS_OVL_SDP) {
		module_base = DSS_DPE1_OFFSET + DSS_DP_CTRL_OFFSET + SDP_MUX;
		if (module_base != 0) {
			ovl_glb->sdp_mux_base = dss_base + module_base;
			ovl_glb->sdp_mux_val = inp32(ovl_glb->sdp_mux_base);
		}
	} else if (ovl_type == DSS_OVL_ADP) {
		;
	} else {
		HISI_FB_ERR("fb%d not support this ovl_type(%d)!", hisifd->index, ovl_type);
	}

	for (i = DPE0_CHN0; i < DSS_CHN_MAX; i++) {
		if (ovl_type == DSS_OVL_PDP) {
			if ((!isChannelBelongtoDPE(DSS_ENG_DPE0, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_DPE2, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_WBE0, i))) {
				continue;
			}
		} else if (ovl_type == DSS_OVL_SDP) {
			if ((!isChannelBelongtoDPE(DSS_ENG_DPE1, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_DPE2, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_WBE0, i))) {
				continue;
			}
		} else if (ovl_type == DSS_OVL_ADP) {
			if ((!isChannelBelongtoDPE(DSS_ENG_DPE3, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_DPE2, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_WBE1, i))) {
				continue;
			}
		} else {
			HISI_FB_ERR("not support this ovl_type(%d)!", ovl_type);
			break;
		}

		module_base = g_dss_module_base[i][MODULE_GLB_CHN_CTL];
		if (module_base != 0) {
			ovl_glb->chn_ctl_base[i] = dss_base + module_base;
			ovl_glb->chn_ctl_val[i] = inp32(ovl_glb->chn_ctl_base[i]);
		}

		module_base = g_dss_module_base[i][MODULE_GLB_CHN_MUX];
		if (module_base != 0) {
			ovl_glb->chn_mux_base[i] = dss_base + module_base;
			ovl_glb->chn_mux_val[i] = inp32(ovl_glb->chn_mux_base[i]);
		}

		module_base = g_dss_module_base[i][MODULE_ROT_MUX];
		if (module_base != 0) {
			ovl_glb->rot_mux_base[i] = dss_base + module_base;
			ovl_glb->rot_mux_val[i] = inp32(ovl_glb->rot_mux_base[i]);
		}

		module_base = g_dss_module_base[i][MODULE_SCF_MUX];
		if (module_base != 0) {
			ovl_glb->scf_mux_base[i] = dss_base + module_base;
			ovl_glb->scf_mux_val[i] = inp32(ovl_glb->scf_mux_base[i]);
		}

		module_base = g_dss_module_base[i][MODULE_ROT_PM_CTL];
		if (module_base != 0) {
			ovl_glb->rot_pm_ctl_base[i] = dss_base + module_base;
			ovl_glb->rot_pm_ctl_val[i] = inp32(ovl_glb->rot_pm_ctl_base[i]);
		}
	}


	/**************************************************************************/
	for (i = 0; i < DSS_ENG_MAX; i++) {
		if (ovl_type == DSS_OVL_PDP) {
			if ((i != DSS_ENG_DPE0) &&
				(i != DSS_ENG_DPE2) &&
				(i != DSS_ENG_WBE0)) {
				continue;
			}
		} else if (ovl_type == DSS_OVL_SDP) {
			if ((i != DSS_ENG_DPE1) &&
				(i != DSS_ENG_DPE2) &&
				(i != DSS_ENG_WBE0)) {
				continue;
			}
		} else if (ovl_type == DSS_OVL_ADP) {
			if ((i != DSS_ENG_DPE3) &&
				(i != DSS_ENG_DPE2) &&
				(i != DSS_ENG_WBE1)) {
				continue;
			}
		} else {
			HISI_FB_ERR("not support this ovl_type(%d)!", ovl_type);
			break;
		}

		module_base = g_dss_module_eng_base[i][MODULE_ENG_DMA_BRG];
		if (module_base != 0) {
			dss_module->rdma_bridge_base[i] = dss_base + module_base;
			hisi_dss_rdma_bridge_init(dss_module->rdma_bridge_base[i],
				&(dss_module->rdma_bridge[i]));
		}

		module_base = g_dss_module_eng_base[i][MODULE_ENG_DMA_BRG];
		if (module_base != 0) {
			dss_module->wdma_bridge_base[i] = dss_base + module_base;
			hisi_dss_wdma_bridge_init(dss_module->wdma_bridge_base[i],
				&(dss_module->wdma_bridge[i]));
		}
	}

	for (i = DPE0_CHN0; i < DSS_CHN_MAX; i++) {
		if (ovl_type == DSS_OVL_PDP) {
			if ((!isChannelBelongtoDPE(DSS_ENG_DPE0, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_DPE2, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_WBE0, i))) {
				continue;
			}
		} else if (ovl_type == DSS_OVL_SDP) {
			if ((!isChannelBelongtoDPE(DSS_ENG_DPE1, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_DPE2, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_WBE0, i))) {
				continue;
			}
		} else if (ovl_type == DSS_OVL_ADP) {
			if ((!isChannelBelongtoDPE(DSS_ENG_DPE3, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_DPE2, i)) &&
				(!isChannelBelongtoDPE(DSS_ENG_WBE1, i))) {
				continue;
			}
		} else {
			HISI_FB_ERR("not support this ovl_type(%d)!", ovl_type);
			break;
		}

		module_base = g_dss_module_base[i][MODULE_MMU_DMA0];
		if (module_base != 0) {
			dss_module->mmu_dma0_base[i] = dss_base + module_base;
			hisi_dss_mmu_dma_init(dss_module->mmu_dma0_base[i], &(dss_module->mmu_dma0[i]));
		}

		module_base = g_dss_module_base[i][MODULE_MMU_DMA1];
		if (module_base != 0) {
			dss_module->mmu_dma1_base[i] = dss_base + module_base;
			hisi_dss_mmu_dma_init(dss_module->mmu_dma1_base[i], &(dss_module->mmu_dma1[i]));
		}

		module_base = g_dss_module_base[i][MODULE_DMA0];
		if (module_base != 0) {
			if (i >= DPE0_CHN0 && i <= DPE3_CHN3) {
				dss_module->rdma0_base[i] = dss_base + module_base;
				hisi_dss_rdma_init(dss_module->rdma0_base[i], &(dss_module->rdma0[i]));
			} else {
				dss_module->wdma_base[i] = dss_base + module_base;
				hisi_dss_wdma_init(dss_module->wdma_base[i], &(dss_module->wdma[i]));
			}
		}

		module_base = g_dss_module_base[i][MODULE_DMA1];
		if (module_base != 0) {
			if (i >= DPE0_CHN0 && i <= DPE3_CHN3) {
				dss_module->rdma1_base[i] = dss_base + module_base;
				hisi_dss_rdma_init(dss_module->rdma1_base[i], &(dss_module->rdma1[i]));
			}
		}
		module_base = g_dss_module_base[i][MODULE_FBDC];
		if (module_base != 0) {
			dss_module->fbdc_base[i] = dss_base + module_base;
			hisi_dss_fbdc_init(dss_module->fbdc_base[i], &(dss_module->fbdc[i]));
		}

		module_base = g_dss_module_base[i][MODULE_DFC];
		if (module_base != 0) {
			dss_module->dfc_base[i] = dss_base + module_base;
			hisi_dss_dfc_init(dss_module->dfc_base[i], &(dss_module->dfc[i]));
		}

		scf_idx = hisi_get_scf_index(i);
		if (scf_idx >= 0) {
			module_base = g_dss_module_base[i][MODULE_SCF];
			if (module_base != 0) {
				dss_module->scf_base[scf_idx] = dss_base + module_base;
				hisi_dss_scf_init(dss_module->scf_base[scf_idx], &(dss_module->scf[scf_idx]));
			}
		}

		module_base = g_dss_module_base[i][MODULE_SCP];
		if (module_base != 0) {
			dss_module->scp_base[i] = dss_base + module_base;
			hisi_dss_scp_init(dss_module->scp_base[i], &(dss_module->scp[i]));
		}

		module_base = g_dss_module_base[i][MODULE_CSC];
		if (module_base != 0) {
			dss_module->csc_base[i] = dss_base + module_base;
			hisi_dss_csc_init(dss_module->csc_base[i], &(dss_module->csc[i]));
		}

		module_base = g_dss_module_base[i][MODULE_FBC];
		if (module_base != 0) {
			dss_module->fbc_base[i] = dss_base + module_base;
			hisi_dss_fbc_init(dss_module->fbc_base[i], &(dss_module->fbc[i]));
		}
	}

	i = ovl_type;
	module_base = g_dss_module_ovl_base[i][MODULE_OVL_BASE];
	if (module_base != 0) {
		dss_module->ov_base[i] = dss_base + module_base;
		hisi_dss_ovl_init(dss_module->ov_base[i], &(dss_module->ov[i]));
	}

	module_base = DSS_GLB2_OFFSET + DSS_MMU_RTLB_OFFSET;
	if (module_base != 0) {
		dss_module->mmu_rptb_base = dss_base + module_base;
		hisi_dss_mmu_rptb_init(dss_module->mmu_rptb_base, &(dss_module->mmu_rptb));
	}

	return 0;
}

int hisi_dss_module_config(struct hisi_fb_data_type *hisifd)
{
	dss_module_reg_t *dss_module = NULL;
	dss_global_reg_t *ovl_glb = NULL;
	uint8_t ovl_type = 0;
	int i = 0;

	BUG_ON(hisifd == NULL);

	ovl_type = hisifd->ovl_type;
	ovl_glb = &(hisifd->dss_glb);
	dss_module = &(hisifd->dss_module);

	/**************************************************************************/
	if (dss_module->dpe2_on_ref > 0) {
		if (hisifd->ovl_type == DSS_OVL_ADP)
			hisifd->set_reg(hisifd, hisifd->dss_base + DSS_GLB_DPE2_CLK_SW, 0x1, 1, 0);
		else
			hisifd->set_reg(hisifd, hisifd->dss_base + DSS_GLB_DPE2_CLK_SW, 0x0, 1, 0);

		dss_module->dpe2_on_ref = 0;
	}

	if (dss_module->rot1_on_ref > 0) {
		if (hisifd->ovl_type == DSS_OVL_ADP)
			hisifd->set_reg(hisifd, hisifd->dss_base + DSS_GLB_ROT1_CLK_SW, 0x1, 1, 0);
		else
			hisifd->set_reg(hisifd, hisifd->dss_base + DSS_GLB_ROT1_CLK_SW, 0x0, 1, 0);

		dss_module->rot1_on_ref = 0;
	}

	/**************************************************************************/
	if (ovl_glb->ovl_mux_used == 1) {
		HISI_FB_ASSERT(ovl_glb->ovl_mux_base);
		hisifd->set_reg(hisifd, ovl_glb->ovl_mux_base, ovl_glb->ovl_mux_val, 32, 0);
	}

	if (ovl_glb->ovl_scene_used == 1) {
		HISI_FB_ASSERT(ovl_glb->ovl_scene_base);
		hisifd->set_reg(hisifd, ovl_glb->ovl_scene_base, ovl_glb->ovl_scene_val, 32, 0);
	}

	for (i = 0; i < ROT_TLB_SCENE_NUM; i++) {
		if (ovl_glb->rot_tlb_scene_used[i] == 1) {
			HISI_FB_ASSERT(ovl_glb->rot_tlb_scene_base[i]);
			hisifd->set_reg(hisifd, ovl_glb->rot_tlb_scene_base[i],
				ovl_glb->rot_tlb_scene_val[i], 32, 0);
		}
	}

	/* DPE2 */
	if (ovl_glb->dpe2_cross_switch_used == 1) {
		HISI_FB_ASSERT(ovl_glb->dpe2_cross_switch_base);
		hisifd->set_reg(hisifd, ovl_glb->dpe2_cross_switch_base,
			ovl_glb->dpe2_cross_switch_val, 32, 0);
	}

	if (ovl_type == DSS_OVL_PDP) {
		if (ovl_glb->dpe0_cross_switch_used == 1) {
			HISI_FB_ASSERT(ovl_glb->dpe0_cross_switch_base);
			hisifd->set_reg(hisifd, ovl_glb->dpe0_cross_switch_base,
				ovl_glb->dpe0_cross_switch_val, 32, 0);
		}
	} else if (ovl_type == DSS_OVL_SDP) {
		if (ovl_glb->sdp_mux_used == 1) {
			HISI_FB_ASSERT(ovl_glb->sdp_mux_base);
			hisifd->set_reg(hisifd, ovl_glb->sdp_mux_base, ovl_glb->sdp_mux_val, 32, 0);
		}
	} else if (ovl_type == DSS_OVL_ADP) {
		;
	} else {
		HISI_FB_ERR("fb%d not support this ovl_type(%d)!", hisifd->index, ovl_type);
	}

	for (i = DPE0_CHN0; i < DSS_CHN_MAX; i++) {
		if (ovl_glb->chn_ctl_used[i] == 1) {
			HISI_FB_ASSERT(ovl_glb->chn_ctl_base[i]);
			hisifd->set_reg(hisifd, ovl_glb->chn_ctl_base[i], ovl_glb->chn_ctl_val[i], 32, 0);
		}

		if (ovl_glb->chn_mux_used[i] == 1) {
			HISI_FB_ASSERT(ovl_glb->chn_mux_base[i]);
			hisifd->set_reg(hisifd, ovl_glb->chn_mux_base[i], ovl_glb->chn_mux_val[i], 32, 0);
		}

		if (ovl_glb->rot_mux_used[i] == 1) {
			HISI_FB_ASSERT(ovl_glb->rot_mux_base[i]);
			hisifd->set_reg(hisifd, ovl_glb->rot_mux_base[i], ovl_glb->rot_mux_val[i], 32, 0);
		}

		if (ovl_glb->scf_mux_used[i] == 1) {
			HISI_FB_ASSERT(ovl_glb->scf_mux_base[i]);
			hisifd->set_reg(hisifd, ovl_glb->scf_mux_base[i], ovl_glb->scf_mux_val[i], 32, 0);
		}

		if (ovl_glb->rot_pm_ctl_used[i] == 1) {
			HISI_FB_ASSERT(ovl_glb->rot_pm_ctl_base[i]);
			hisifd->set_reg(hisifd, ovl_glb->rot_pm_ctl_base[i], ovl_glb->rot_pm_ctl_val[i], 32, 0);
		}
	}

	/**************************************************************************/
	for (i = 0; i < DSS_ENG_MAX; i++) {
		if (dss_module->rdma_bridge_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->rdma_bridge_base[i]);
			hisi_dss_rdma_bridge_set_reg(hisifd,
				dss_module->rdma_bridge_base[i], &(dss_module->rdma_bridge[i]));
		}

		if (dss_module->wdma_bridge_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->wdma_bridge_base[i]);
			hisi_dss_wdma_bridge_set_reg(hisifd,
				dss_module->wdma_bridge_base[i], &(dss_module->wdma_bridge[i]));
		}
	}

	for (i = DPE0_CHN0; i < DSS_CHN_MAX; i++) {
		if (dss_module->rdma0_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->rdma0_base[i]);
			hisi_dss_rdma_set_reg(hisifd, dss_module->rdma0_base[i], &(dss_module->rdma0[i]));
		}

		if (dss_module->rdma1_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->rdma1_base[i]);
			hisi_dss_rdma_set_reg(hisifd, dss_module->rdma1_base[i], &(dss_module->rdma1[i]));
		}

		if (dss_module->fbdc_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->fbdc_base[i]);
			hisi_dss_fbdc_set_reg(hisifd, dss_module->fbdc_base[i], &(dss_module->fbdc[i]));
		}

		if (dss_module->dfc_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->dfc_base[i]);
			hisi_dss_dfc_set_reg(hisifd, dss_module->dfc_base[i], &(dss_module->dfc[i]));
		}

		if (dss_module->scp_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->scp_base[i]);
			hisi_dss_scp_set_reg(hisifd, dss_module->scp_base[i], &(dss_module->scp[i]));
		}

		if (dss_module->csc_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->csc_base[i]);
			hisi_dss_csc_set_reg(hisifd, dss_module->csc_base[i], &(dss_module->csc[i]));
		}

		if (dss_module->fbc_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->fbc_base[i]);
			hisi_dss_fbc_set_reg(hisifd, dss_module->fbc_base[i], &(dss_module->fbc[i]));
		}

		if (dss_module->wdma_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->wdma_base[i]);
			hisi_dss_wdma_set_reg(hisifd, dss_module->wdma_base[i], &(dss_module->wdma[i]));
		}

		if (dss_module->mmu_dma0_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->mmu_dma0_base[i]);
			hisi_dss_mmu_dma_set_reg(hisifd, dss_module->mmu_dma0_base[i],
				&(dss_module->mmu_dma0[i]));
		}

		if (dss_module->mmu_dma1_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->rdma1_base[i]);
			hisi_dss_mmu_dma_set_reg(hisifd, dss_module->mmu_dma1_base[i],
				&(dss_module->mmu_dma1[i]));
		}
	}

	if (dss_module->mmu_rptb_used == 1) {
		HISI_FB_ASSERT(dss_module->mmu_rptb_base);
		hisi_dss_mmu_rptb_set_reg(hisifd, dss_module->mmu_rptb_base,
			&(dss_module->mmu_rptb));
	}

	for (i = 0; i < DSS_SCF_MAX; i++) {
		if (dss_module->scf_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->scf_base[i]);
			hisi_dss_scf_set_reg(hisifd, dss_module->scf_base[i], &(dss_module->scf[i]));
		}
	}

	for (i = 0; i < DSS_OVL_MAX; i++) {
		if (dss_module->ov_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->ov_base[i]);
			hisi_dss_ovl_set_reg(hisifd, dss_module->ov_base[i], &(dss_module->ov[i]));
		}
	}

	/**************************************************************************/
	if ((PRIMARY_PANEL_IDX == hisifd->index) &&
		hisifd->panel_info.sbl_support) {
			hisi_dss_sbl_set_reg(hisifd, hisifd->dss_base + DSS_DPP_SBL_OFFSET, &(hisifd->sbl));
	}

	return 0;
}


/*******************************************************************************
**
*/
int hisi_overlay_set_fastboot(struct hisi_fb_data_type *hisifd)
{
	struct fb_info *fbi = NULL;
	dss_overlay_t *pov_req = NULL;
	dss_layer_t *layer = NULL;

	BUG_ON(hisifd == NULL);
	fbi = hisifd->fbi;
	BUG_ON(fbi == NULL);
	pov_req = &(hisifd->ov_req);
	BUG_ON(pov_req == NULL);

	memset(pov_req, 0, sizeof(dss_overlay_t));
	layer = &(pov_req->layer_infos[0]);
	pov_req->layer_nums = 1;
	pov_req->ovl_flags = DSS_SCENE_PDP_ONLINE;

	pov_req->dirty_region_updt_info.dirty_rect.x = 0;
	pov_req->dirty_region_updt_info.dirty_rect.y = 0;
	pov_req->dirty_region_updt_info.dirty_rect.w = fbi->var.xres;
	pov_req->dirty_region_updt_info.dirty_rect.h = fbi->var.yres;

	layer->src.format = HISI_FB_PIXEL_FORMAT_BGRA_8888;
	layer->src.width = fbi->var.xres;
	layer->src.height = fbi->var.yres;
	layer->src.bpp = fbi->var.bits_per_pixel >> 3;
	layer->src.stride = fbi->fix.line_length;
	layer->src.phy_addr = fbi->fix.smem_start ;
	layer->src.vir_addr = 0;
	layer->src.ptba = 0;
	layer->src.ptva = 0;
	layer->src.buf_size = fbi->fix.line_length * fbi->var.yres;
	layer->src.is_tile = 0;
	layer->src.mmu_enable = 0;

	layer->src_rect.x = 0;
	layer->src_rect.y = 0;
	layer->src_rect.w = fbi->var.xres;
	layer->src_rect.h = fbi->var.yres;

	layer->dst_rect.x = 0;
	layer->dst_rect.y = 0;
	layer->dst_rect.w = fbi->var.xres;
	layer->dst_rect.h = fbi->var.yres;

	layer->transform = HISI_FB_TRANSFORM_NOP;
	layer->blending = HISI_FB_BLENDING_NONE;
	layer->glb_alpha = 0xFF;
	layer->color = 0x0;
	layer->layer_idx = 0;
	layer->chn_idx = DPE0_CHN2;
	layer->need_cap = 0;
	layer->flags = DSS_SCENE_PDP_ONLINE;


	hisifd->dss_exception.underflow_exception = 1;

	return 0;
}

int hisi_overlay_on(struct hisi_fb_data_type *hisifd, bool ov_base_enable)
{
	char __iomem *dss_base = 0;
	char __iomem *ovl_base = 0;
	uint32_t ovl_flags = 0;
	int ret = 0;
	dss_global_reg_t *ovl_glb_default = 0;
	dss_module_reg_t *dss_module_default = 0;
	int i = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;
	ovl_glb_default = &(hisifd->dss_glb_default);
	dss_module_default = &(hisifd->dss_module_default);

	hisifd->vactive0_start_flag = 0;

	//hisifd->ov_wb_enabled = 0;
	hisifd->ov_wb_done = 0;

	hisifd->dirty_region_updt.x = 0;
	hisifd->dirty_region_updt.y = 0;
	hisifd->dirty_region_updt.w = hisifd->panel_info.xres;
	hisifd->dirty_region_updt.h = hisifd->panel_info.yres;
	hisifd->dirty_region_updt_enable = 0;

	memset(&(hisifd->sbl), 0, sizeof(dss_sbl_t));
	hisifd->sbl_enable = 0;
	hisifd->sbl_lsensor_value = 0;
	hisifd->sbl_level = 0;

	memset(&hisifd->dss_exception, 0, sizeof(dss_exception_t));
	memset(&hisifd->dss_wb_exception, 0, sizeof(dss_exception_t));

	for (i = 0; i < HISI_DSS_OFFLINE_MAX_NUM; i++) {
		memset(&(hisifd->dss_rptb_info_prev[i]), 0, sizeof(dss_rptb_info_t));
		memset(&(hisifd->dss_rptb_info_cur[i]), 0, sizeof(dss_rptb_info_t));
		memset(&(hisifd->dss_wb_rptb_info_prev[i]), 0, sizeof(dss_rptb_info_t));
		memset(&(hisifd->dss_wb_rptb_info_cur[i]), 0, sizeof(dss_rptb_info_t));
	}

	if ((hisifd->index == PRIMARY_PANEL_IDX) || (hisifd->index == EXTERNAL_PANEL_IDX)) {
		if (!hisifd->dss_module_resource_initialized) {
			hisi_dss_module_default(hisifd);
			hisifd->dss_module_resource_initialized = true;

			if (hisifd->index == PRIMARY_PANEL_IDX) {
				ovl_glb_default->chn_ctl_val[DPE0_CHN2] = 0x0;
				ovl_glb_default->ovl_scene_val = 0x0;
				ovl_glb_default->ovl_mux_val = 0x0;

				dss_module_default->rdma_bridge[DSS_ENG_DPE0] =
					dss_module_default->rdma_bridge[DSS_ENG_DPE2];
				dss_module_default->rdma0[DPE0_CHN2] =
					dss_module_default->rdma0[DPE0_CHN0];
				dss_module_default->dfc[DPE0_CHN2] =
					dss_module_default->dfc[DPE0_CHN0];
				dss_module_default->ov[DSS_OVL_PDP].ovl_layer[2] =
					dss_module_default->ov[DSS_OVL_PDP].ovl_layer[1];
				dss_module_default->ov[DSS_OVL_PDP].ovl_layer_position[2] =
					dss_module_default->ov[DSS_OVL_PDP].ovl_layer_position[1];
			}
		}

		if (ov_base_enable) {
			ovl_base = dss_base +
				g_dss_module_ovl_base[hisifd->ovl_type][MODULE_OVL_BASE];

			if (hisifd->index == PRIMARY_PANEL_IDX) {
				ovl_flags = DSS_SCENE_PDP_ONLINE;
				outp32(hisifd->dss_base + DSS_GLB_PDP_S2_CPU_IRQ_CLR, 0xFFFFFFFF);
			} else {
				ovl_flags = DSS_SCENE_SDP_ONLINE;
				outp32(hisifd->dss_base + DSS_GLB_SDP_S2_CPU_IRQ_CLR, 0xFFFFFFFF);
			}

			ret = hisi_dss_module_init(hisifd);
			if (ret != 0) {
				HISI_FB_ERR("fb%d, failed to hisi_dss_module_init! ret = %d\n", hisifd->index, ret);
				return ret;
			}

			hisi_dss_config_ok_begin(hisifd);

			hisi_dss_handle_prev_ovl_req(hisifd, &(hisifd->ov_req));
			hisi_dss_handle_prev_ovl_req_wb(hisifd, &(hisifd->ov_req));

			ret = hisi_dss_ovl_base_config(hisifd, NULL, ovl_flags);
			if (ret != 0) {
				HISI_FB_ERR("fb%d, faile to hisi_dss_ovl_base_config! ret=%d\n", hisifd->index, ret);
				return ret;
			}

			hisifd->dss_module.fbc_used[WBE0_CHN1] = 1;
			hisifd->dss_module.fbc[WBE0_CHN1].fbc_pm_ctrl =
				set_bits32(hisifd->dss_module.fbc[WBE0_CHN1].fbc_pm_ctrl, 0x83AA, 32, 0);

			if (hisifd->index == PRIMARY_PANEL_IDX) {
				hisifd->dss_module.fbdc_used[DPE0_CHN3] = 1;
				hisifd->dss_module.fbdc[DPE0_CHN3].fbdc_pm_ctrl =
					set_bits32(hisifd->dss_module.fbdc[DPE0_CHN3].fbdc_pm_ctrl, 0x601AA, 32, 0);
			}

			ret = hisi_dss_module_config(hisifd);
			if (ret != 0) {
				HISI_FB_ERR("fb%d, failed to hisi_dss_module_config! ret = %d\n", hisifd->index, ret);
				return ret;
			}

			hisi_dss_config_ok_end(hisifd);
			single_frame_update(hisifd);

			hisi_dss_scf_coef_load(hisifd);

			if (!hisi_dss_check_reg_reload_status(hisifd)) {
				mdelay(17);
			}

			hisifd->dss_exception.underflow_exception = 0;
			memset(&hisifd->ov_req, 0, sizeof(dss_overlay_t));
		} else {
			hisi_dss_scf_coef_load(hisifd);
		}
	}

	return 0;
}

int hisi_overlay_off(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;
	uint32_t ovl_flags = 0;
	char __iomem *dss_base = 0;
	int i = 0;
	int chn_idx = 0;
	uint32_t module_base = 0;

	BUG_ON(hisifd == NULL);

	dss_base = hisifd->dss_base;

	if ((hisifd->index == PRIMARY_PANEL_IDX) || (hisifd->index == EXTERNAL_PANEL_IDX)) {
		hisifb_activate_vsync(hisifd);

		memset(&hisifd->dss_exception, 0, sizeof(dss_exception_t));
		hisifd->dss_exception.underflow_exception = 1;

		if (hisifd->index == PRIMARY_PANEL_IDX) {
			outp32(dss_base + DSS_GLB_PDP_S2_CPU_IRQ_CLR, 0xFFFFFFFF);
			ovl_flags = DSS_SCENE_PDP_ONLINE;
		} else {
			outp32(dss_base + DSS_GLB_SDP_S2_CPU_IRQ_CLR, 0xFFFFFFFF);
			ovl_flags = DSS_SCENE_SDP_ONLINE;
		}

		ret = hisi_dss_module_init(hisifd);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, failed to hisi_dss_module_init! ret = %d\n", hisifd->index, ret);
			return ret;
		}

		hisi_dss_config_ok_begin(hisifd);

		hisi_dss_handle_prev_ovl_req(hisifd, &(hisifd->ov_req));
		hisi_dss_handle_prev_ovl_req_wb(hisifd, &(hisifd->ov_req));

		ret = hisi_dss_ovl_base_config(hisifd, NULL, ovl_flags);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, faile to hisi_dss_ovl_base_config! ret=%d\n", hisifd->index, ret);
			return ret;
		}

		ret = hisi_dss_module_config(hisifd);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, failed to hisi_dss_module_config! ret = %d\n", hisifd->index, ret);
			return ret;
		}

		if (hisifd->panel_info.dirty_region_updt_support) {
			msleep(20);
			hisifd->dirty_region_updt_enable = 0;
			hisi_dss_dirty_region_updt_config(hisifd);
		}

		hisi_dss_config_ok_end(hisifd);
		single_frame_update(hisifd);

		if (!hisi_dss_check_reg_reload_status(hisifd)) {
			mdelay(17);
		}

		for (i = 0; i < hisifd->ov_req.layer_nums; i++) {
			chn_idx = hisifd->ov_req.layer_infos[i].chn_idx;
			module_base = g_dss_module_base[chn_idx][MODULE_GLB_CHN_CTL];
			if (module_base != 0)
				set_reg(dss_base + module_base, 0x0, 3, 4);
		}

		hisifb_deactivate_vsync(hisifd);

		hisi_dss_rptb_handler(hisifd, false, 0);
		hisi_dss_rptb_handler(hisifd, true, 0);

		hisi_dss_scf_coef_unload(hisifd);
	}

	hisifd->ldi_data_gate_en = 0;
	hisifd->dss_exception.underflow_exception = 0;
	memset(&hisifd->ov_req, 0, sizeof(dss_overlay_t));

	return 0;
}

bool hisi_dss_check_reg_reload_status(struct hisi_fb_data_type *hisifd)
{
	uint32_t tmp = 0;
	uint32_t offset = 0;
	uint32_t try_times = 0;
	bool is_ready = false;

	BUG_ON(hisifd == NULL);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		offset = DSS_GLB_PDP_S2_CPU_IRQ_RAWSTAT;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		offset = DSS_GLB_SDP_S2_CPU_IRQ_RAWSTAT;
	} else {
		HISI_FB_ERR("fb%d, not support!\n", hisifd->index);
		return false;
	}

	/* check reload status */
	is_ready = true;
	try_times= 0;
	tmp = inp32(hisifd->dss_base + offset);
	while ((tmp & BIT(0)) != 0x1) {
		udelay(10);
		if (++try_times > 4000) {
			HISI_FB_ERR("fb%d, check reload status failed, PDP_S2_CPU_IRQ_RAWSTAT=0x%x.\n",
				hisifd->index, tmp);
			is_ready= false;
			break;
		}

		tmp = inp32(hisifd->dss_base + offset);
	}

	mdelay(40);  //be sure that overlay base layer has been displayed on lcd

	return is_ready;
}

static int hisi_overlay_test(struct hisi_fb_data_type *hisifd, void __user *argp)
{
	int ret = 0;
	dss_overlay_test_t ov_test;
	dss_reg_t *pdss_reg = NULL;
	int i = 0;

	BUG_ON(hisifd == NULL);

	down(&hisifd->blank_sem);

	if (!hisifd->panel_power_on) {
		HISI_FB_DEBUG("fb%d panel is power off!", hisifd->index);
		//ret =-EINVAL;
		//goto err_return;
		return 0;
	}

	pdss_reg = (dss_reg_t *)kmalloc(sizeof(dss_reg_t) * HISI_FB_DSS_REG_TEST_NUM,
		GFP_ATOMIC);
	if (!pdss_reg) {
		HISI_FB_ERR("pdss_reg failed to alloc!\n");
		ret = -ENOMEM;
		goto err_return;
	}

	memset(&ov_test, 0, sizeof(dss_overlay_test_t));
	memset(pdss_reg, 0, sizeof(dss_reg_t) * HISI_FB_DSS_REG_TEST_NUM);
	ret = copy_from_user(&ov_test, argp, sizeof(ov_test));
	if (ret) {
		HISI_FB_ERR("copy_from_user failed!\n");
		goto err_return;
	}

	if ((ov_test.reg_cnt <= 0) || (ov_test.reg_cnt > HISI_FB_DSS_REG_TEST_NUM)) {
		HISI_FB_ERR("dss_reg_cnt(%d) is larger than DSS_REG_TEST_NUM(%d)!\n",
			ov_test.reg_cnt, HISI_FB_DSS_REG_TEST_NUM);
		goto err_return;
	}

	if (ov_test.reg_cnt > 0) {
		ret = copy_from_user(pdss_reg, ov_test.reg,
			ov_test.reg_cnt * sizeof(dss_reg_t));
		if (ret) {
			HISI_FB_ERR("copy_from_user failed!\n");
			goto err_return;
		}
	}

	//HISI_FB_DEBUG("dss_reg_cnt=%d\n", ov_test.reg_cnt);
	for (i = 0; i < ov_test.reg_cnt; i++) {
		if (pdss_reg[i].reg_addr) {
			//HISI_FB_DEBUG("(0x%x, 0x%x)\n", pdss_reg[i].reg_addr, pdss_reg[i].reg_val);
			outp32((char __iomem *)pdss_reg[i].reg_addr, pdss_reg[i].reg_val);
		}
	}

err_return:
	up(&hisifd->blank_sem);

	if (pdss_reg) {
		kfree(pdss_reg);
		pdss_reg = NULL;
	}

	return ret;
}

int hisi_overlay_ioctl_handler(struct hisi_fb_data_type *hisifd,
	uint32_t cmd, void __user *argp)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);

	switch (cmd) {
	case HISIFB_OV_ONLINE_PLAY:
		if (hisifd->ov_online_play) {
			down(&hisifd->blank_sem);
			ret = hisifd->ov_online_play(hisifd, argp);
			if (ret != 0) {
				HISI_FB_ERR("fb%d ov_online_play failed!\n", hisifd->index);
			}
			up(&hisifd->blank_sem);

			if (ret == 0) {
				if (hisifd->bl_update) {
					hisifd->bl_update(hisifd);
				}
			}
		}
		break;
	case HISIFB_OV_OFFLINE_PLAY:
		if (hisifd->ov_offline_play) {
			ret = hisifd->ov_offline_play(hisifd, argp);
			if (ret != 0) {
				HISI_FB_ERR("fb%d ov_offline_play failed!\n", hisifd->index);
			}
		}
		break;
	case HISIFB_OV_ONLINE_WB:
		if (hisifd->ov_online_wb) {
			down(&hisifd->blank_sem);
			ret = hisifd->ov_online_wb(hisifd, argp);
			if (ret != 0) {
				HISI_FB_ERR("fb%d ov_online_wb failed!\n", hisifd->index);
			}
			up(&hisifd->blank_sem);
		}
		break;

	case HISIFB_OV_ONLINE_WB_CTRL:
		if (hisifd->ov_online_wb_ctrl) {
			down(&hisifd->blank_sem);
			ret = hisifd->ov_online_wb_ctrl(hisifd, argp);
			if (ret != 0) {
				HISI_FB_ERR("fb%d ov_online_wb_ctrl failed!\n", hisifd->index);
			}
			up(&hisifd->blank_sem);
		}
		break;

	case HISIFB_OV_TEST:
		{
			ret = hisi_overlay_test(hisifd, argp);
			if (ret != 0) {
				HISI_FB_ERR("fb%d hisi_overlay_test failed!\n", hisifd->index);
			}
		}
		break;

	case HISIFB_PURE_LAYER_CHECK:
		{
			ret = hisi_dss_check_pure_layer(hisifd, argp);
			if (ret != 0) {
				HISI_FB_ERR("fb%d hisi_dss_check_pure_layer  failed!\n", hisifd->index);
			}
		}
		break;

	default:
		break;
	}

	return ret;
}

int hisi_overlay_init(struct hisi_fb_data_type *hisifd)
{
	dss_layer_t *layer = NULL;
	dss_overlay_t *pov_req = NULL;
	int i = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(hisifd->dss_base == NULL);

	hisifd->pan_display_fnc = hisi_overlay_pan_display;
	hisifd->ov_ioctl_handler = hisi_overlay_ioctl_handler;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		hisifd->ovl_type = DSS_OVL_PDP;
		hisifd->set_reg = hisifb_set_reg;
		hisifd->ov_online_play = hisi_ov_online_play;
		hisifd->ov_offline_play = NULL;
		hisifd->ov_online_wb = hisi_ov_online_writeback;
		hisifd->ov_wb_isr_handler = hisi_writeback_isr_handler;
		hisifd->ov_vactive0_start_isr_handler = hisi_vactive0_start_isr_handler;
		hisifd->ov_optimized = hisi_dss_ovl_optimized;
		hisifd->ov_online_wb_ctrl = hisi_online_writeback_control;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		hisifd->ovl_type = DSS_OVL_SDP;
		hisifd->set_reg = hisifb_set_reg;
		hisifd->ov_online_play = hisi_ov_online_play;
		hisifd->ov_offline_play = NULL;
		hisifd->ov_online_wb = hisi_ov_online_writeback;
		hisifd->ov_wb_isr_handler = hisi_writeback_isr_handler;
		hisifd->ov_vactive0_start_isr_handler = hisi_vactive0_start_isr_handler;
		hisifd->ov_optimized = hisi_dss_ovl_optimized;
		hisifd->ov_online_wb_ctrl = NULL;
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		hisifd->ovl_type = DSS_OVL_ADP;
		hisifd->set_reg = ov_cmd_list_output_32;
		hisifd->ov_online_play = NULL;
		hisifd->ov_offline_play = hisi_ov_offline_play;
		hisifd->ov_online_wb = NULL;
		hisifd->ov_wb_isr_handler = hisi_writeback_isr_handler;
		hisifd->ov_vactive0_start_isr_handler = NULL;
		hisifd->ov_optimized = NULL;
		hisifd->ov_online_wb_ctrl = NULL;
	} else {
		HISI_FB_ERR("fb%d not support this device!\n", hisifd->index);
		return -EINVAL;
	}

	hisifd->dss_module_resource_initialized = false;

	hisifd->vactive0_start_flag = 0;
	init_waitqueue_head(&hisifd->vactive0_start_wq);
	hisifd->ldi_data_gate_en = 0;

	sema_init(&hisifd->ov_wb_sem, 1);
	sema_init(&hisifd->dpe3_ch0_sem, 1);
	sema_init(&hisifd->dpe3_ch1_sem, 1);

	hisifd->ov_wb_enabled = 0;
	hisifd->ov_wb_done = 0;
	init_waitqueue_head(&hisifd->ov_wb_wq);

	memset(&hisifd->dss_exception, 0, sizeof(dss_exception_t));
	memset(&hisifd->dss_wb_exception, 0, sizeof(dss_exception_t));
	memset(&hisifd->ov_req, 0, sizeof(dss_overlay_t));
	memset(&hisifd->ov_wb_req, 0, sizeof(dss_overlay_t));
	memset(&hisifd->dss_glb, 0, sizeof(dss_global_reg_t));
	memset(&hisifd->dss_module, 0, sizeof(dss_module_reg_t));
	memset(&hisifd->dss_glb_default, 0, sizeof(dss_global_reg_t));
	memset(&hisifd->dss_module_default, 0, sizeof(dss_module_reg_t));
	for (i = 0; i < HISI_DSS_OFFLINE_MAX_NUM; i++) {
		memset(&(hisifd->dss_rptb_info_prev[i]), 0, sizeof(dss_rptb_info_t));
		memset(&(hisifd->dss_rptb_info_cur[i]), 0, sizeof(dss_rptb_info_t));
		memset(&(hisifd->dss_wb_rptb_info_prev[i]), 0, sizeof(dss_rptb_info_t));
		memset(&(hisifd->dss_wb_rptb_info_cur[i]), 0, sizeof(dss_rptb_info_t));
	}

	hisifd->dirty_region_updt.x = 0;
	hisifd->dirty_region_updt.y = 0;
	hisifd->dirty_region_updt.w = hisifd->panel_info.xres;
	hisifd->dirty_region_updt.h = hisifd->panel_info.yres;
	hisifd->dirty_region_updt_enable = 0;

	if (g_rptb_gen_pool == 0) {
		g_rptb_gen_pool = hisi_dss_rptb_init();
	}

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		pov_req = &(hisifd->ov_req);
		layer = &(pov_req->layer_infos[0]);
		pov_req->layer_nums = 1;
		layer->src.format = HISI_FB_PIXEL_FORMAT_BGRA_8888;
		layer->src.mmu_enable = 1;
		layer->chn_idx = DPE0_CHN0;
	}

	return 0;
}

int hisi_overlay_deinit(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	if (g_rptb_gen_pool)
		hisi_dss_rptb_deinit(g_rptb_gen_pool);

	return 0;
}

void hisi_writeback_isr_handler(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);
	hisifd->ov_wb_done = 1;
	wake_up_interruptible_all(&hisifd->ov_wb_wq);
}

void hisi_vactive0_start_isr_handler(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	if (is_mipi_cmd_panel(hisifd)) {
		hisifd->vactive0_start_flag = 1;

		if (hisifd->panel_info.dirty_region_updt_support) {
			disable_ldi(hisifd);
		}
	} else {
		hisifd->vactive0_start_flag++;
	}
	wake_up_interruptible_all(&hisifd->vactive0_start_wq);
	g_frame_count++;
}

/*lint -e665*/
int hisi_vactive0_start_config(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;
	uint32_t prev_vactive0_start = 0;
	uint32_t isr_s1 = 0;
	uint32_t isr_s2 = 0;
	uint32_t isr_s1_mask = 0;
	uint32_t isr_s2_mask = 0;
#if defined (CONFIG_HUAWEI_DSM)
	uint32_t read_value = 0;
#endif
	BUG_ON(hisifd == NULL);

	if (is_mipi_cmd_panel(hisifd)) {
		if (hisifd->vactive0_start_flag == 0) {
			ret = wait_event_interruptible_timeout(hisifd->vactive0_start_wq,
				hisifd->vactive0_start_flag, HZ / 10);
			if (ret <= 0) {
				HISI_FB_ERR("fb%d, 1wait_for vactive0_start_flag timeout!ret=%d, vactive0_start_flag=%d\n",
					hisifd->index, ret, hisifd->vactive0_start_flag);

				set_reg(hisifd->dss_base + PDP_LDI_CTRL, 0x0, 1, 2);
				ret = wait_event_interruptible_timeout(hisifd->vactive0_start_wq,
					hisifd->vactive0_start_flag, HZ / 10);
				if (ret <= 0) {
					HISI_FB_ERR("fb%d, 2wait_for vactive0_start_flag timeout!ret=%d, vactive0_start_flag=%d\n",
						hisifd->index, ret, hisifd->vactive0_start_flag);

					hisifd->ldi_data_gate_en = 0;

				#if defined (CONFIG_HUAWEI_DSM)
					outp32(hisifd->dss_base + DSS_MIPI_DSI0_OFFSET + MIPIDSI_GEN_HDR_OFFSET, 0x0A06);
					if (!mipi_dsi_read(&read_value, hisifd->dss_base + DSS_MIPI_DSI0_OFFSET))
						HISI_FB_ERR("Read LCD register 0x0A timeout\n");
					else
						HISI_FB_ERR("LCD power status 0x0A=0x%x\n", read_value);
				#endif

					ret = -ETIMEDOUT;
				} else {
					set_reg(hisifd->dss_base + PDP_LDI_CTRL, 0x1, 1, 2);
					hisifd->ldi_data_gate_en = 1;

					ret = 0;
				}
			} else {
				set_reg(hisifd->dss_base + PDP_LDI_CTRL, 0x1, 1, 2);
				hisifd->ldi_data_gate_en = 1;

				ret = 0;
			}
		} else {
			set_reg(hisifd->dss_base + PDP_LDI_CTRL, 0x1, 1, 2);
			hisifd->ldi_data_gate_en = 1;
		}
		hisifd->vactive0_start_flag = 0;
	} else {
		prev_vactive0_start = hisifd->vactive0_start_flag;
		ret = wait_event_interruptible_timeout(hisifd->vactive0_start_wq,
			(prev_vactive0_start != hisifd->vactive0_start_flag), HZ / 10);
		if (ret <= 0) {
			HISI_FB_ERR("fb%d, wait_for vactive0_start_flag timeout!ret=%d, "
				"prev_vactive0_start=%d, vactive0_start_flag=%d\n",
				hisifd->index, ret, prev_vactive0_start, hisifd->vactive0_start_flag);
			ret = -ETIMEDOUT;
		} else {
			ret = 0;
			hisifd->dss_exception.underflow_exception = 1;
		}
	}

	if (ret == -ETIMEDOUT) {
		if (hisifd->ovl_type == DSS_OVL_PDP) {
			isr_s1_mask = inp32(hisifd->dss_base + DSS_GLB_PDP_CPU_IRQ_MSK);
			isr_s1 = inp32(hisifd->dss_base + DSS_GLB_PDP_CPU_IRQ);
			isr_s2_mask = inp32(hisifd->dss_base + PDP_LDI_CPU_IRQ_MSK);
			isr_s2 = inp32(hisifd->dss_base + PDP_LDI_CPU_IRQ);
		} else if (hisifd->ovl_type == DSS_OVL_SDP) {
			isr_s1_mask = inp32(hisifd->dss_base + DSS_GLB_SDP_CPU_IRQ_MSK);
			isr_s1 = inp32(hisifd->dss_base + DSS_GLB_SDP_CPU_IRQ);
			isr_s2_mask = inp32(hisifd->dss_base + SDP_LDI_CPU_IRQ_MSK);
			isr_s2 = inp32(hisifd->dss_base + SDP_LDI_CPU_IRQ);
		}

		HISI_FB_ERR("fb%d, isr_s1_mask=0x%x, isr_s2_mask=0x%x, isr_s1=0x%x, isr_s2=0x%x.\n",
			hisifd->index, isr_s1_mask, isr_s2_mask, isr_s1, isr_s2);

	#if defined (CONFIG_HUAWEI_DSM)
		if (!dsm_client_ocuppy(lcd_dclient) && hisifd->lcd_self_testing == false) {
			dsm_client_record(lcd_dclient, "fb%d, isr_s1_mask=0x%x, isr_s2_mask=0x%x, "
				"isr_s1=0x%x, isr_s2=0x%x.\n", hisifd->index,
				isr_s1_mask, isr_s2_mask, isr_s1, isr_s2);
			dsm_client_notify(lcd_dclient, DSM_LCD_TE_TIME_OUT_ERROR_NO);
		}
	#endif
	}

	return ret;
}
/*lint +e665*/

void hisi_dss_dirty_region_updt_config(struct hisi_fb_data_type *hisifd)
{
	struct hisi_fb_panel_data *pdata = NULL;
	char __iomem *ovl_base = NULL;
	dss_rect_t dirty = {0};

	BUG_ON(hisifd == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (!hisifd->panel_info.dirty_region_updt_support)
		return ;

	if (!g_enable_dirty_region_updt
		|| !hisifd->dirty_region_updt_enable
		|| hisifd->sbl_enable
		|| hisifd->ov_wb_enabled) {
		dirty.x = 0;
		dirty.y = 0;
		dirty.w = hisifd->panel_info.xres;
		dirty.h = hisifd->panel_info.yres;
	} else {
		dirty = hisifd->ov_req.dirty_region_updt_info.dirty_rect;
	}

	ovl_base = hisifd->dss_base +
		g_dss_module_ovl_base[hisifd->ovl_type][MODULE_OVL_BASE];
	set_reg(ovl_base + OVL_OV_SIZE,
		(DSS_WIDTH(dirty.w) | DSS_HEIGHT(dirty.h) << 16), 31, 0);

	if (hisifd->ldi_data_gate_en == 1) {
		set_reg(hisifd->dss_base + PDP_LDI_CTRL, 0x1, 1, 2);
	}

	if (g_debug_dirty_region_updt) {
		HISI_FB_INFO("dirty_region(%d,%d, %d,%d). ldi_data_gate_en=%d.\n",
			dirty.x, dirty.y, dirty.w, dirty.h, hisifd->ldi_data_gate_en);
	}

	if ((dirty.x == hisifd->dirty_region_updt.x)
		&& (dirty.y == hisifd->dirty_region_updt.y)
		&& (dirty.w == hisifd->dirty_region_updt.w)
		&& (dirty.h == hisifd->dirty_region_updt.h)) {
		return ;
	}

	if (pdata && pdata->set_display_region) {
		pdata->set_display_region(hisifd->pdev, &dirty);
	}

	hisifd->dirty_region_updt = dirty;
}
