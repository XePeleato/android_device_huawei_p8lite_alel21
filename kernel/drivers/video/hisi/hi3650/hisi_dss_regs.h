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
#ifndef HISI_DSS_REGS_H
#define HISI_DSS_REGS_H

#include "hisi_fb_def.h"
#include "hisi_dss.h"


/* MACROS */
#define DSS_WIDTH(width)	(width - 1)
#define DSS_HEIGHT(height)	(height - 1)

#define DSS_PRI_CLK_RATE_MAX	(300 * 1000000L)
#define DSS_AUX_CLK_RATE_MAX	(300 * 1000000L)
#define DSS_PCLK_CLK_RATE_MAX	(120 * 1000000L)

#define RES_720P	(720 * 1280)
#define RES_1080P	(1920 * 1080)
#define RES_1600P	(2560 * 1600)

#define DFC_MAX_CLIP_NUM	(31)

/* for DFS */
/* 1480 * 144bits */
#define DBUF_SRAM_ONE_MEM_DEPTH	(1408)

#define GET_THD_RQOS_IN(max_depth)	(max_depth * 10 / 100)
#define GET_THD_RQOS_OUT(max_depth)	(max_depth * 30 / 100)
#define GET_THD_WQOS_IN(max_depth)	(max_depth * 95 / 100)
#define GET_THD_WQOS_OUT(max_depth)	(max_depth * 70 / 100)
#define GET_THD_CG_IN(max_depth)	(max_depth -1)
#define GET_THD_CG_OUT(max_depth)	(max_depth * 70 / 100)
#define GET_THD_OTHER_DFS_CG_HOLD(max_depth)	(0x20)
#define GET_THD_OTHER_WR_WAIT(max_depth)	(max_depth * 90 / 100)

#define GET_RDMA_ROT_HQOS_ASSERT_LEV(max_depth)	(max_depth * 30 /100)
#define GET_RDMA_ROT_HQOS_REMOVE_LEV(max_depth)	(max_depth* 60 / 100)


enum {
	LCD_LANDSCAPE = 0,
	LCD_PORTRAIT,
};

enum {
	LCD_RGB565 = 0,
	LCD_RGB666,
	LCD_RGB888,
};

enum {
	LCD_RGB = 0,
	LCD_BGR,
};

enum dss_addr {
	DSS_ADDR_PLANE0 = 0,
	DSS_ADDR_PLANE1,
	DSS_ADDR_PLANE2,
};

enum dss_transform {
	DSS_TRANSFORM_NOP = 0x0,
	DSS_TRANSFORM_FLIP_H = 0x01,
	DSS_TRANSFORM_FLIP_V = 0x02,
	DSS_TRANSFORM_ROT = 0x04,
};

enum dss_dfc_format {
	DFC_PIXEL_FORMAT_RGB_565 = 0,
	DFC_PIXEL_FORMAT_XRGB_4444,
	DFC_PIXEL_FORMAT_ARGB_4444,
	DFC_PIXEL_FORMAT_XRGB_5551,
	DFC_PIXEL_FORMAT_ARGB_5551,
	DFC_PIXEL_FORMAT_XRGB_8888,
	DFC_PIXEL_FORMAT_ARGB_8888,
	DFC_PIXEL_FORMAT_BGR_565,
	DFC_PIXEL_FORMAT_XBGR_4444,
	DFC_PIXEL_FORMAT_ABGR_4444,
	DFC_PIXEL_FORMAT_XBGR_5551,
	DFC_PIXEL_FORMAT_ABGR_5551,
	DFC_PIXEL_FORMAT_XBGR_8888,
	DFC_PIXEL_FORMAT_ABGR_8888,

	DFC_PIXEL_FORMAT_YUV444,
	DFC_PIXEL_FORMAT_YVU444,
	DFC_PIXEL_FORMAT_YUYV422,
	DFC_PIXEL_FORMAT_YVYU422,
	DFC_PIXEL_FORMAT_VYUY422,
	DFC_PIXEL_FORMAT_UYVY422,
};

enum dss_dma_format {
	DMA_PIXEL_FORMAT_RGB_565 = 0,
	DMA_PIXEL_FORMAT_ARGB_4444,
	DMA_PIXEL_FORMAT_XRGB_4444,
	DMA_PIXEL_FORMAT_ARGB_5551,
	DMA_PIXEL_FORMAT_XRGB_5551,
	DMA_PIXEL_FORMAT_ARGB_8888,
	DMA_PIXEL_FORMAT_XRGB_8888,

	DMA_PIXEL_FORMAT_RESERVED0,

	DMA_PIXEL_FORMAT_YUV_422_I,
	DMA_PIXEL_FORMAT_YUV_420_SP_HP,
	DMA_PIXEL_FORMAT_YUV_420_P_HP,
	DMA_PIXEL_FORMAT_YUV_422_SP_HP,
	DMA_PIXEL_FORMAT_YUV_422_P_HP,
	DMA_PIXEL_FORMAT_AYUV_4444_I,
	DMA_PIXEL_FORMAT_YVU_422_I,
};

enum dss_mmu_tag_num {
	MMU_TAG_2 = 0x0,
	MMU_TAG_4 = 0x1,
	MMU_TAG_8 = 0x3,
};

enum dss_mmu_tag_size {
	MMU_TAG_16B = 0x0,
	MMU_TAG_32B = 0x1,
	MMU_TAG_64B = 0x2,
	MMU_TAG_128B = 0x3,
	MMU_TAG_256B = 0x4,
};

enum dss_mmu_tag_ctl {
	MMU_TAG_LEFT_ENDPOINT = 0x0,
	MMU_TAG_RIGHT_ENDPOINT = 0x1,
	MMU_TAG_NEW = 0x2,
};

enum dss_mmu_pixel_format {
	MMU_PIXEL_FORMAT_PACKAGE = 0x0,	/* RGB or YUV Package */
	MMU_PIXEL_FORMAT_YUV_SEMI_PLANAR = 0x1,
	MMU_PIXEL_FORMAT_YUV_PLANAR = 0x2,
};

enum dss_buf_format {
	DSS_BUF_LINEAR = 0,
	DSS_BUF_TILE,
};

/* stretch blt, linear/tile, rotation, pixel format */
enum dss_mmu_tlb_tag_org {
	MMU_TLB_TAG_ORG_0x0 = 0x0, /* 0 0 000 00 -> 0x0 */
	MMU_TLB_TAG_ORG_0x4 = 0x4, /* 0 0 001 00 -> 0x4 */
	MMU_TLB_TAG_ORG_0x2 = 0x2, /* 0 0 000 10 -> 0x2 */
	MMU_TLB_TAG_ORG_0x6 = 0x6, /* 0 0 001 10 -> 0x6 */

	MMU_TLB_TAG_ORG_0x1 = 0x1, /* 0 0 000 01 -> 0x1 */
	MMU_TLB_TAG_ORG_0x5 = 0x5, /* 0 0 001 01 -> 0x5 */

	MMU_TLB_TAG_ORG_0x8 = 0x8, /* 0 0 010 00 -> 0x8 */
	MMU_TLB_TAG_ORG_0xC = 0xC, /* 0 0 011 00 -> 0xC */
	MMU_TLB_TAG_ORG_0xA = 0xA, /* 0 0 010 10 -> 0xA */
	MMU_TLB_TAG_ORG_0xE = 0xE, /* 0 0 011 10 -> 0xE */

	MMU_TLB_TAG_ORG_0x9 = 0x9, /* 0 0 010 01 -> 0x9 */
	MMU_TLB_TAG_ORG_0xD = 0xD, /* 0 0 011 01 -> 0xD */

	MMU_TLB_TAG_ORG_0x10 = 0x10, /* 0 0 100 00 -> 0x10 */
	MMU_TLB_TAG_ORG_0x1C = 0x1C, /* 0 0 111 00 -> 0x1C */
	MMU_TLB_TAG_ORG_0x12 = 0x12, /* 0 0 100 10 -> 0x12 */
	MMU_TLB_TAG_ORG_0x1E = 0x1E, /* 0 0 111 10 -> 0x1E */

	MMU_TLB_TAG_ORG_0x11 = 0x11, /* 0 0 100 01 -> 0x11 */
	MMU_TLB_TAG_ORG_0x1D = 0x1D, /* 0 0 111 01 -> 0x1D */

	MMU_TLB_TAG_ORG_0x20 = 0x20, /* 0 1 000 00 -> 0x20 */
	MMU_TLB_TAG_ORG_0x24 = 0x24, /* 0 1 001 00 -> 0x24 */
	MMU_TLB_TAG_ORG_0x22 = 0x22, /* 0 1 000 10 -> 0x22 */
	MMU_TLB_TAG_ORG_0x26 = 0x26, /* 0 1 001 10 -> 0x26 */

	MMU_TLB_TAG_ORG_0x21 = 0x21, /* 0 1 000 01 -> 0x21 */
	MMU_TLB_TAG_ORG_0x25 = 0x25, /* 0 1 001 01 -> 0x25 */

	MMU_TLB_TAG_ORG_0x28 = 0x28, /* 0 1 010 00 -> 0x28 */
	MMU_TLB_TAG_ORG_0x2C = 0x2C, /* 0 1 011 00 -> 0x2C */
	MMU_TLB_TAG_ORG_0x2A = 0x2A, /* 0 1 010 10 -> 0x2A */
	MMU_TLB_TAG_ORG_0x2E = 0x2E, /* 0 1 011 10 -> 0x2E */

	MMU_TLB_TAG_ORG_0x29 = 0x29, /* 0 1 010 01 -> 0x29 */
	MMU_TLB_TAG_ORG_0x2D = 0x2D, /* 0 1 011 01 -> 0x2D */

	MMU_TLB_TAG_ORG_0x30 = 0x30, /* 0 1 100 00 -> 0x30 */
	MMU_TLB_TAG_ORG_0x3C = 0x3C, /* 0 1 111 00 -> 0x3C */
	MMU_TLB_TAG_ORG_0x32 = 0x32, /* 0 1 100 10 -> 0x32 */
	MMU_TLB_TAG_ORG_0x3E = 0x3E, /* 0 1 111 10 -> 0x3E */

	MMU_TLB_TAG_ORG_0x31 = 0x31, /* 0 1 100 01 -> 0x31 */
	MMU_TLB_TAG_ORG_0x3D = 0x3D, /* 0 1 111 01 -> 0x3D */

	MMU_TLB_TAG_ORG_0x40 = 0x40, /* 1 0 000 00 -> 0x40 */
	MMU_TLB_TAG_ORG_0x44 = 0x44, /* 1 0 001 00 -> 0x44 */
	MMU_TLB_TAG_ORG_0x42 = 0x42, /* 1 0 000 10 -> 0x42 */
	MMU_TLB_TAG_ORG_0x46 = 0x46, /* 1 0 001 10 -> 0x46 */

	MMU_TLB_TAG_ORG_0x41 = 0x41, /* 1 0 000 01 -> 0x41 */
	MMU_TLB_TAG_ORG_0x45 = 0x45, /* 1 0 001 01 -> 0x45 */

	MMU_TLB_TAG_ORG_0x48 = 0x48, /* 1 0 010 00 -> 0x48 */
	MMU_TLB_TAG_ORG_0x4C = 0x4C, /* 1 0 011 00 -> 0x4C */
	MMU_TLB_TAG_ORG_0x4A = 0x4A, /* 1 0 010 10 -> 0x4A */
	MMU_TLB_TAG_ORG_0x4E = 0x4E, /* 1 0 011 10 -> 0x4E */

	MMU_TLB_TAG_ORG_0x49 = 0x49, /* 1 0 010 01 -> 0x49 */
	MMU_TLB_TAG_ORG_0x4D = 0x4D, /* 1 0 011 01 -> 0x4D */

	MMU_TLB_TAG_ORG_0x50 = 0x50, /* 1 0 100 00 -> 0x50 */
	MMU_TLB_TAG_ORG_0x5C = 0x5C, /* 1 0 111 00 -> 0x5C */
	MMU_TLB_TAG_ORG_0x52 = 0x52, /* 1 0 100 10 -> 0x52 */
	MMU_TLB_TAG_ORG_0x5E = 0x5E, /* 1 0 111 10 -> 0x5E */

	MMU_TLB_TAG_ORG_0x51 = 0x51, /* 1 0 100 01 -> 0x51 */
	MMU_TLB_TAG_ORG_0x5D = 0x5D, /* 1 0 111 01 -> 0x5D */

	MMU_TLB_TAG_ORG_0x60 = 0x60, /* 1 1 000 00 -> 0x60 */
	MMU_TLB_TAG_ORG_0x64 = 0x64, /* 1 1 001 00 -> 0x64 */
	MMU_TLB_TAG_ORG_0x62 = 0x62, /* 1 1 000 10 -> 0x62 */
	MMU_TLB_TAG_ORG_0x66 = 0x66, /* 1 1 001 10 -> 0x66 */

	MMU_TLB_TAG_ORG_0x61 = 0x61, /* 1 1 000 01 -> 0x61 */
	MMU_TLB_TAG_ORG_0x65 = 0x65, /* 1 1 001 01 -> 0x65 */

	MMU_TLB_TAG_ORG_0x68 = 0x68, /* 1 1 010 00 -> 0x68 */
	MMU_TLB_TAG_ORG_0x6C = 0x6C, /* 1 1 011 00 -> 0x6C */
	MMU_TLB_TAG_ORG_0x6A = 0x6A, /* 1 1 010 10 -> 0x6A */
	MMU_TLB_TAG_ORG_0x6E = 0x6E, /* 1 1 011 10 -> 0x6E */

	MMU_TLB_TAG_ORG_0x69 = 0x69, /* 1 1 010 01 -> 0x69 */
	MMU_TLB_TAG_ORG_0x6D = 0x6D, /* 1 1 011 01 -> 0x6D */

	MMU_TLB_TAG_ORG_0x70 = 0x70, /* 1 1 100 00 -> 0x70 */
	MMU_TLB_TAG_ORG_0x7C = 0x7C, /* 1 1 111 00 -> 0x7C */
	MMU_TLB_TAG_ORG_0x72 = 0x72, /* 1 1 100 10 -> 0x72 */
	MMU_TLB_TAG_ORG_0x7E = 0x7E, /* 1 1 111 10 -> 0x7E */

	MMU_TLB_TAG_ORG_0x71 = 0x71, /* 1 1 100 01 -> 0x71 */
	MMU_TLB_TAG_ORG_0x7D = 0x7D, /* 1 1 111 01 -> 0x7D */
};

enum dss_blend_mode {
	DSS_BLEND_CLEAR = 0,
	DSS_BLEND_SRC,
	DSS_BLEND_DST,
	DSS_BLEND_SRC_OVER_DST,
	DSS_BLEND_DST_OVER_SRC,
	DSS_BLEND_SRC_IN_DST,
	DSS_BLEND_DST_IN_SRC,
	DSS_BLEND_SRC_OUT_DST,
	DSS_BLEND_DST_OUT_SRC,
	DSS_BLEND_SRC_ATOP_DST,
	DSS_BLEND_DST_ATOP_SRC,
	DSS_BLEND_SRC_XOR_DST,
	DSS_BLEND_SRC_ADD_DST,
	DSS_BLEND_FIX_OVER,
	DSS_BLEND_FIX_PER0,
	DSS_BLEND_FIX_PER1,
	DSS_BLEND_FIX_PER2,
	DSS_BLEND_FIX_PER3,
	DSS_BLEND_FIX_PER4,
	DSS_BLEND_FIX_PER5,
	DSS_BLEND_FIX_PER6,
	DSS_BLEND_FIX_PER7,
	DSS_BLEND_FIX_PER8,
	DSS_BLEND_FIX_PER9,
	DSS_BLEND_FIX_PER10,
	DSS_BLEND_FIX_PER11,
	DSS_BLEND_FIX_PER12,
	DSS_BLEND_FIX_PER13,

};

enum dss_chn_module {
	MODULE_CHN_FLUSH,
	MODULE_MMU_DMA0,
	MODULE_MMU_DMA1,
	MODULE_DMA,
	MODULE_DFC,
	MODULE_SCL,
	MODULE_SCL_LUT,
	MODULE_SHARPNESS,
	MODULE_CSC,
	MODULE_CE,
	MODULE_CE_LUT0,
	MODULE_CE_LUT1,
	MODULE_CE_LUT2,
	MODULE_CE_LUT3,
	MODULE_CHN_MAX,
};

enum dss_ovl_module {
	MODULE_OVL_BASE,
	MODULE_MCTL_BASE,
	MODULE_OVL_MAX,
};

enum dss_axi_idx {
	AXI_CHN0 = 0,
	AXI_CHN1,
	AXI_CHN_MAX,
};

enum dss_brg_idx {
	DSS_RBRG0 = 0,
	DSS_RBRG1,
	DSS_WBRG0,
	DSS_RBRG_IDX_MAX,
};

enum dss_rdma_idx {
	DSS_RDMA0 = 0,
	DSS_RDMA1,
	DSS_RDMA2,
	DSS_RDMA3,
	DSS_RDMA4,
};


/*******************************************************************************
**
*/
/* CRGPERI */
#define PEREN3_OFFSET	(0x030)
#define PERDIS3_OFFSET	(0x034)
#define PERRSTEN3_OFFSET	(0x084)
#define PERRSTDIS3_OFFSET	(0x088)


/*******************************************************************************
**
*/
// CPU_PDP_INTS 0x224
// CPU_PDP_INT_MSK 0x228
#define BIT_DBG_MCTL	BIT(22)
#define BIT_DBG_RCH	BIT(21)
#define BIT_DSI_TE_TRI	BIT(20)
#define BIT_LCD_TE0_TRI	BIT(19)
#define BIT_LCD_TE1_PIN	BIT(18)
#define BIT_FRM_START	BIT(17)
#define BIT_VFRONTPORCH_END	BIT(16)
#define BIT_VFRONTPORCH	BIT(15)
#define BIT_VBACKPORCH	BIT(14)
#define BIT_VACTIVE1_END	BIT(13)
#define BIT_VACTIVE1_START	BIT(12)
#define BIT_VACTIVE0_END	BIT(11)
#define BIT_VACTIVE0_START	BIT(10)
#define BIT_FRM_END	BIT(9)
#define BIT_VSYNC	BIT(8)
#define BIT_LDI_UNFLOW	BIT(7)
#define BIT_BACKLIGHT	BIT(6)
#define BIT_STRENGTH	BIT(5)
#define BIT_HIS_RDY	BIT(4)
#define BIT_CMD0	BIT(3)
#define BIT_RCH6_CE	BIT(2)
#define BIT_RCH4_CE	BIT(1)
#define BIT_RCH2_CE	BIT(0)

// CTL_FLUSH 0x0158
#define BIT_CTL_FLUSH_EN	BIT(21)
#define BIT_SCF_FLUSH_EN	BIT(19)
#define BIT_DPP0_FLUSH_EN	BIT(18)
#define BIT_DBUF1_FLUSH_EN	BIT(17)
#define BIT_DBUF0_FLUSH_EN	BIT(16)
#define BIT_OV3_FLUSH_EN	BIT(15)
#define BIT_OV2_FLUSH_EN	BIT(14)
#define BIT_OV1_FLUSH_EN	BIT(13)
#define BIT_OV0_FLUSH_EN	BIT(12)
#define BIT_WB1_FLUSH_EN	BIT(11)
#define BIT_WB0_FLUSH_EN	BIT(10)
#define BIT_DMA3_FLUSH_EN	BIT(9)
#define BIT_DMA2_FLUSH_EN	BIT(8)
#define BIT_DMA1_FLUSH_EN	BIT(7)
#define BIT_DMA0_FLUSH_EN	BIT(6)
#define BIT_RGB1_FLUSH_EN	BIT(4)
#define BIT_RGB0_FLUSH_EN	BIT(3)
#define BIT_VIG1_FLUSH_EN	BIT(1)
#define BIT_VIG0_FLUSH_EN	BIT(0)


/*******************************************************************************
** MODULE BASE ADDRESS
*/
// 1	BASE ADDRESS	00000000
#define DSS_GLB0_OFFSET	(0x0000)

// 2 BASE ADDRESS	00002000
#define DSS_CMDLIST_OFFSET	(0x2000)

#define DSS_DAB_OFFSET	(0x7000)
#define DSS_WDMA_BRIDAGE1_OFFSET	(0x7800)

#define DSS_RDMA_BRIDAGE0_OFFSET	(0x8000)
#define DSS_RDMA_BRIDAGE1_OFFSET	(0x8400)

#define DSS_MMU_WBRG1_OFFSET	(0x9800)
#define DSS_MMU_RBRG0_OFFSET	(0xA000)
#define DSS_MMU_RBRG1_OFFSET	(0xA800)

// 3	BASE ADDRESS	00010000
#define DSS_MCTRL_CTL0_OFFSET	(0x10000)
#define DSS_MCTRL_CTL1_OFFSET	(0x10400)
#define DSS_MCTRL_CTL2_OFFSET	(0x10800)
#define DSS_MCTRL_CTL3_OFFSET	(0x10C00)

// 8 BASE ADDRESS 00060000
#define DSS_OVL0_OFFSET	(0x60000)
#define DSS_OVL1_OFFSET	(0x60400)
#define DSS_OVL2_OFFSET	(0x60800)
#define DSS_OVL3_OFFSET	(0x60C00)
#define DSS_SCF_OFFSET	(0x64000)
#define DSS_DBUF0_OFFSET	(0x6D000)
#define DSS_DBUF1_OFFSET	(0x6E000)

// 9 BASE ADDRESS 00070000
#define DSS_DPP_OFFSET	(0x70000)
#define DSS_TOP_OFFSET	(0x70000)
#define DSS_LCP_OFFSET	(0x70100)
#define DSS_ACM_OFFSET	(0x70200)
#define DSS_GAMA_OFFSET	(0x70300)
#define DSS_ACM_LUT_OFFSET	(0x70800)
#define DSS_GAMA_LUT_OFFSET	(0x72000)
#define DSS_LCP_LUT_OFFSET	(0x73000)
#define DSS_DPP_SBL_OFFSET	(0x7C000)
#define DSS_LDI0_OFFSET	(0x7D000)
#define DSS_IFBC_OFFSET	(0x7D400)
#define DSS_LDI1_OFFSET	(0x7E000)

// RCH_V
#define DSS_RCH_VG0_DMA_OFFSET	(0x20000)
#define DSS_RCH_VG0_DFC_OFFSET (0x20100)
#define DSS_RCH_VG0_SCL_OFFSET	(0x20200)
#define DSS_RCH_VG0_SHARPNESS_OFFSET	(0x20300)
#define DSS_RCH_VG0_CE_OFFSET	(0x20400)
#define DSS_RCH_VG0_CSC_OFFSET	(0x20500)
#define DSS_RCH_VG0_SCL_LUT_OFFSET		(0x21000)
#define DSS_RCH_VG0_CE0_LUT_OFFSET	(0x25000)
#define DSS_RCH_VG0_CE1_LUT_OFFSET	(0x25400)
#define DSS_RCH_VG0_CE2_LUT_OFFSET	(0x25800)
#define DSS_RCH_VG0_CE3_LUT_OFFSET	(0x25C00)

#define DSS_RCH_VG1_DMA_OFFSET	(0x28000)
#define DSS_RCH_VG1_DFC_OFFSET	(0x28100)
#define DSS_RCH_VG1_SCL_OFFSET	(0x28200)
#define DSS_RCH_VG1_SHARPNESS_OFFSET	(0x28300)
#define DSS_RCH_VG1_CE_OFFSET	(0x28400)
#define DSS_RCH_VG1_CSC_OFFSET	(0x28500)
#define DSS_RCH_VG1_SCL_LUT_OFFSET	(0x29000)
#define DSS_RCH_VG1_CE0_LUT_OFFSET	(0x2D000)
#define DSS_RCH_VG1_CE1_LUT_OFFSET	(0x2D400)
#define DSS_RCH_VG1_CE2_LUT_OFFSET	(0x2D800)
#define DSS_RCH_VG1_CE3_LUT_OFFSET	(0x2DC00)

// RCH_G
#define DSS_RCH_G0_DMA_OFFSET	(0x38000)
#define DSS_RCH_G0_DFC_OFFSET	(0x38100)
#define DSS_RCH_G0_SCL_OFFSET	(0x38200)
#define DSS_RCH_G0_SCL_LUT_OFFSET	(0x39000)

#define DSS_RCH_G1_DMA_OFFSET	(0x40000)
#define DSS_RCH_G1_DFC_OFFSET	(0x40100)
#define DSS_RCH_G1_SCL_OFFSET	(0x40200)
#define DSS_RCH_G1_SCL_LUT_OFFSET	(0x41000)

// RCH_D
#define DSS_RCH_D0_DMA_OFFSET	(0x50000)
#define DSS_RCH_D0_DFC_OFFSET	(0x50100)

#define DSS_RCH_D1_DMA_OFFSET	(0x51000)
#define DSS_RCH_D1_DFC_OFFSET	(0x51100)

#define DSS_RCH_D2_DMA_OFFSET	(0x52000)
#define DSS_RCH_D2_DFC_OFFSET	(0x52100)

#define DSS_RCH_D3_DMA_OFFSET	(0x53000)
#define DSS_RCH_D3_DFC_OFFSET	(0x53100)

// 4 WCH
#define DSS_WCH0_DMA_OFFSET	(0x5A000)
#define DSS_WCH0_DFC_OFFSET	(0x5A500)
#define DSS_WCH0_CSC_OFFSET	(0x5A500)

#define DSS_WCH1_DMA_OFFSET	(0x5C000)
#define DSS_WCH1_DFC_OFFSET	(0x5C500)
#define DSS_WCH1_CSC_OFFSET	(0x5C500)


/*******************************************************************************
** GLB
*/
#define GLB_APB_MHL_SECU	(0x0000)
#define GLB_APB_CTL	(0x0004)

#define GLB_PXL0_DIV2_GT_EN	(0x0100)
#define GLB_PXL0_DIV4_GT_EN	(0x0104)
#define GLB_PXL0_GT_EN	(0x0108)
#define GLB_PXL0_DSI_GT_EN	(0x010C)
#define GLB_PXL0_DIVXCFG	(0x0110)

#define GLB_DSS_AXI_RST_EN	(0x0114)
#define GLB_DSS_APB_RST_EN	(0x0118)
#define GLB_DSS_CORE_RST_EN	(0x011C)
#define GLB_PXL0_DIV2_RST_EN	(0x0120)
#define GLB_PXL0_DIV4_RST_EN	(0x0124)
#define GLB_PXL0_RST_EN	(0x0128)
#define GLB_PXL0_DSI_RST_EN	(0x012C)
#define GLB_MHL_RST_EN	(0x0130)
#define GLB_DSS_PXL1_RST_EN	(0x0134)

#define GLB_RCH2_THREAD	(0x0200)
#define GLB_RCH4_THREAD	(0x204)
#define GLB_RCH6_THREAD	(0x208)

#define GLB_MCU_PDP_INTS	(0x20C)
#define GLB_MCU_PDP_INT_MSK	(0x210)
#define GLB_MCU_SDP_INTS	(0x214)
#define GLB_MCU_SDP_INT_MSK	(0x218)
#define GLB_MCU_OFF_INTS	(0x21C)
#define GLB_MCU_OFF_INT_MSK	(0x220)
#define GLB_CPU_PDP_INTS	(0x224)
#define GLB_CPU_PDP_INT_MSK	(0x228)
#define GLB_CPU_SDP_INTS	(0x22C)
#define GLB_CPU_SDP_INT_MSK	(0x230)
#define GLB_CPU_OFF_INTS	(0x234)
#define GLB_CPU_OFF_INT_MSK	(0x238)

#define GLB_DBG_MCTL_INTS	(0x23C)
#define GLB_DBG_MCTL_INT_MSK	(0x240)
#define GLB_DBG_WCH0_INTS	(0x244)
#define GLB_DBG_WCH0_INT_MSK	(0x248)
#define GLB_DBG_WCH1_INTS	(0x24C)
#define GLB_DBG_WCH1_INT_MSK	(0x250)
#define GLB_DBG_RCH0_INTS	(0x254)
#define GLB_DBG_RCH0_INT_MSK	(0x258)
#define GLB_DBG_RCH1_INTS	(0x25C)
#define GLB_DBG_RCH1_INT_MSK	(0x260)
#define GLB_DBG_RCH2_INTS	(0x264)
#define GLB_DBG_RCH2_INT_MSK	(0x268)
#define GLB_DBG_RCH3_INTS	(0x26C)
#define GLB_DBG_RCH3_INT_MSK	(0x270)
#define GLB_DBG_RCH4_INTS	(0x274)
#define GLB_DBG_RCH4_INT_MSK	(0x278)
#define GLB_DBG_RCH5_INTS	(0x27C)
#define GLB_DBG_RCH5_INT_MSK	(0x280)
#define GLB_DBG_RCH6_INTS	(0x284)
#define GLB_DBG_RCH6_INT_MSK	(0x288)
#define GLB_DBG_RCH7_INTS	(0x28C)
#define GLB_DBG_RCH7_INT_MSK	(0x290)
#define GLB_DBG_RCH8_INTS	(0x294)
#define GLB_DBG_RCH8_INT_MSK	(0x298)
#define GLB_DBG_RCH9_INTS	(0x29C)
#define GLB_DBG_RCH9_INT_MSK	(0x2A0)

#define GLB_MODULE_CLK_SEL	(0x0300)
#define GLB_MODULE_CLK_EN	(0x0304)
#define GLB_TP_SEL	(0x0400)
#define GLB_DSI1_CLK_SEL	(0x0404)


/*******************************************************************************
** CMDLIST
*/
#define CMDLIST_CH0_CTRL	(0x0000)
#define CMDLIST_CH0_STATUS	(0x0004)
#define CMDLIST_CH0_STAAD	(0x0008)
#define CMDLIST_CH0_CURAD	(0x000C)
#define CMDLIST_CH0_INTE	(0x0010)
#define CMDLIST_CH0_INTC	(0x0014)
#define CMDLIST_CH0_INTS	(0x0018)
#define CMDLIST_CH1_CTRL	(0x001C)
#define CMDLIST_CH1_STATUS	(0x0020)
#define CMDLIST_CH1_STAAD	(0x0024)
#define CMDLIST_CH1_CURAD	(0x0028)
#define CMDLIST_CH1_INTE	(0x002C)
#define CMDLIST_CH1_INTC	(0x0030)
#define CMDLIST_CH1_INTS	(0x0034)
#define CMDLIST_CH2_CTRL	(0x003C)
#define CMDLIST_CH2_STATUS	(0x0040)
#define CMDLIST_CH2_STAAD	(0x0044)
#define CMDLIST_CH2_CURAD	(0x0048)
#define CMDLIST_CH2_INTE	(0x004C)
#define CMDLIST_CH2_INTC	(0x0050)
#define CMDLIST_CH2_INTS	(0x0054)
#define CMDLIST_CH3_CTRL	(0x0058)
#define CMDLIST_CH3_STATUS	(0x005C)
#define CMDLIST_CH3_STAAD	(0x0060)
#define CMDLIST_CH3_CURAD	(0x0064)
#define CMDLIST_CH3_INTE	(0x0068)
#define CMDLIST_CH3_INTC	(0x006C)
#define CMDLIST_CH3_INTS	(0x0070)
#define CMDLIST_CH4_CTRL	(0x0074)
#define CMDLIST_CH4_STATUS	(0x0078)
#define CMDLIST_CH4_STAAD	(0x007C)
#define CMDLIST_CH4_CURAD	(0x0080)
#define CMDLIST_CH4_INTE	(0x0084)
#define CMDLIST_CH4_INTC	(0x0088)
#define CMDLIST_CH4_INTS	(0x008C)
#define CMDLIST_CH5_CTRL	(0x0090)
#define CMDLIST_CH5_STATUS	(0x0094)
#define CMDLIST_CH5_STAAD	(0x0098)
#define CMDLIST_CH5_CURAD	(0x009C)
#define CMDLIST_CH5_INTE	(0x00A0)
#define CMDLIST_CH5_INTC	(0x00A4)
#define CMDLIST_CH5_INTS	(0x00A8)
#define CMDLIST_CTRL	(0x00AC)
#define CMDLIST_SECU	(0x00B0)
#define CMDLIST_INTS	(0x00B4)
#define CMDLIST_SWRST	(0x00B8)
#define CMDLIST_CH0_ST	(0x00C0)
#define CMDLIST_CH1_ST	(0x00C4)
#define CMDLIST_CH2_ST	(0x00C8)
#define CMDLIST_CH3_ST	(0x00CC)
#define CMDLIST_CH4_ST	(0x00D0)
#define CMDLIST_CH5_ST	(0x00D4)
#define CMDLIST_CH0_DBG	(0x00E0)
#define CMDLIST_CH1_DBG	(0x00E4)
#define CMDLIST_CH2_DBG	(0x00E8)
#define CMDLIST_CH3_DBG	(0x00EC)
#define CMDLIST_CH4_DBG	(0x00F0)
#define CMDLIST_CH5_DBG	(0x00F4)


#define HISI_DSS_OFFLINE_MAX_BLOCK	(64)
#define HISI_DSS_OFFLINE_MAX_LIST	(128)


/*******************************************************************************
** DAB
*/
#define DAB_AXI0_ID_MSK0	(0x0000)
#define DAB_AXI0_ID_MSK1	(0x0004)
#define DAB_AXI1_ID_MSK0	(0x0008)
#define DABR_AXI1_ID_MSK1	(0x000C)
#define DABR_AXI0_ID_MAP_L	(0x0010)
#define DABR_AXI0_ID_MAP_H	(0x0014)
#define DABR_AXI1_ID_MAP_L	(0x0018)
#define DABR_AXI1_ID_MAP_H	(0x001C)
#define DABR_AXI0_QOS_TBL	(0x0024)
#define DABR_AXI1_QOS_TBL	(0x0028)
#define DABW_AXI0_QOS_TBL	(0x002C)
#define DABW_AXI1_QOS_TBL	(0x0030)
#define DABW_AXI0_GT	(0x0034)
#define DABW_AXI1_GT	(0x0038)
#define DABR_AXI0_WR_GT	(0x003C)
#define DABR_AXI1_WR_GT	(0x0040)
#define DABR_ARB_MOD	(0x0044)
#define DAB_PRESS_DIS	(0x0048)
#define DAB_WOTDD	(0x004C)


/*******************************************************************************
** WBRG
*/
#define WBRG_COMM_CTL	(0x0000)
#define WBRG_STS	(0x000C)
#define WBRG_CTL0	(0x0018)
#define WBRG_CTL1	(0x001C)
#define WBRG_CREDIT_CTL0	(0x0020)
#define WBRG_CREDIT_CTL1	(0x0024)
#define WBRG_CLK_CTRL	(0x0034)
#define WBRG_WO_CH_CTL	(0x0038)
#define WBRG_WS_CH_CTL	(0x003C)
#define WBRG_WS_MEM_CTRL	(0x0040)
#define WBRG_WO_MEM_CTRL	(0x0044)
#define WBRG_MMU_MEM_CTRL	(0x0048)


/*******************************************************************************
** RBRG
*/
#define RBRG_RDMA0_CTL	(0x0000)
#define RBRG_RDMA1_CTL	(0x0004)
#define RBRG_RDMA2_CTL	(0x0008)
#define RBRG_RDMA3_CTL	(0x000C)
#define RBRG_RDMA4_CTL	(0x0010)
#define RD_SHADOW_SEL	(0x0014)
#define RBRG_OUTSTANDING_DEP0	(0x001C)
#define RBRG_OUTSTANDING_DEP1	(0x0020)
#define RBRG_OUTSTANDING_DEP2	(0x0024)
#define RBRG_OUTSTANDING_DEP3	(0x0028)
#define RBRG_OUTSTANDING_DEP4	(0x002C)
#define RBRG_AXI0_CMDS_CTL	(0x0030)
#define RBRG_AXI1_CMDS_CTL	(0x0034)
#define RBRG_AXI0_RID_MAP	(0x0038)
#define RBRG_AXI1_RID_MAP	(0x003C)
#define RBRG_AXI0_DBUF_FULL0	(0x0040)
#define RBRG_AXI0_DBUF_FULL1	(0x0044)
#define RBRG_AXI0_DBUF_FULL2	(0x0048)
#define RBRG_AXI0_DBUF_FULL3	(0x004C)
#define RBRG_AXI1_DBUF_FULL0	(0x0050)
#define RBRG_AXI1_DBUF_FULL1	(0x0054)
#define RBRG_AXI1_DBUF_FULL2	(0x0058)
#define RBRG_AXI1_DBUF_FULL3	(0x005C)
#define RBRG_RID_FIFO_LEV0	(0x0060)
#define RBRG_RID_FIFO_LEV1	(0x0064)
#define RBRG_AXI0_CMDS_STS	(0x0068)
#define RBRG_AXI1_CMDS_STS	(0x006C)
#define RBRG_IDLE	(0x0070)
#define RBRG_CORE_GT	(0x007C)
#define RBRG_AXI0_MEM_CTL0	(0x0088)
#define RBRG_AXI0_MEM_CTL1	(0x008C)

/* MACROS */
#define RDMA_NUM	(5)
#define RDMA_OSD_FORMULA	(32)
#define RDMA_MID_NUM	(18)
#define RDMA_OSD_MULTIPLE	(10)
#define DMA_OSD_MAX_32	(32)
#define DMA_OSD_MAX_64	(64)
#define AXI_OSD_BIG_SUM_MAX	(32)
#define AXI_OSD_SUM_MAX	(128)
#define RDMA_REQ_CONT_MAX	(16)

typedef struct dss_rbridge {
	uint32_t rbrg_rdma_ctl[RDMA_NUM];
	uint32_t rbrg_outstanding_dep[RDMA_NUM];
} dss_rbridge_t;


/*******************************************************************************
** MMU
*/
#define DSS_MMU_RDMA0_OFFSET	(0x0)
#define DSS_MMU_RDMA1_OFFSET	(0x100)
#define DSS_MMU_RDMA2_OFFSET	(0x200)
#define DSS_MMU_RDMA3_OFFSET	(0x300)
#define DSS_MMU_RDMA4_OFFSET	(0x400)

#define DSS_MMU_WDMA0_OFFSET	(0x0)
#define DSS_MMU_WDMA1_OFFSET	(0x100)

#define MMU_CH_CTRL	(0x0100)
#define MMU_CH_STATUS	(0x0104)
#define MMU_CH_PTBA	(0x0108)
#define MMU_CH_PTVA	(0x010C)
#define MMU_CH_VA	(0x0110)
#define MMU_CH_PA	(0x0114)
#define MMU_CH_PFVA	(0x0118)
#define MMU_CH_AMSK	(0x011C)
#define MMU_CH_SECU	(0x0120)

#define MMU_AXI_ARB	(0x052C)
#define MMU_RD_SHADOW_SEL	(0x0600)
#define MMU_MEM_CTRL	(0x0604)
#define MMU_CLK_SEL	(0x0608)
#define MMU_CLK_EN	(0x060C)


/* MACROS */
/* MMU aligned limited: PTBA(128Bytes aligned)  PTVA(128KBytes aligned) */
#define MMU_CH_PTBA_ALIGN	(128)
#define MMU_CH_PTVA_ALIGN	(128 * 1024)
#define MMU_CH_AMSK_THRESHOLD	(4096)

#define MMU_PAGE_SIZE	(4096)

typedef struct dss_mmu_dma {
	uint32_t mmu_ch_ctrl;
	uint32_t mmu_ch_ptba;
	uint32_t mmu_ch_ptva;
	uint32_t mmu_ch_amsk;
} dss_mmu_dma_t;


/*******************************************************************************
** RDMA
*/

//DMA_CMN
#define DMA_OFT_X0	(0x0004)
#define DMA_OFT_Y0	(0x0008)
#define DMA_OFT_X1	(0x0014)
#define DMA_OFT_Y1	(0x0018)
#define DMA_MASK0	(0x001C)
#define DMA_MASK1	(0x0020)
#define DMA_STRETCH_SIZE_VRT	(0x0024)
#define DMA_CTRL	(0x002C)
#define DMA_TILE_SCRAM	(0x0034)

//WDMA_CMN
#define WDMA_SW_MASK_EN	(0x0060)
#define WDMA_START_MASK0	(0x007C)
#define WDMA_END_MASK0	(0x0080)
#define WDMA_START_MASK1	(0x00A8)
#define WDMA_END_MASK1	(0x00AC)

//Y
#define DMA_DATA_ADDR0	(0x0000)
#define DMA_STRIDE0	(0x0010)
#define DMA_STRETCH_STRIDE0	(0x0028)
#define DMA_DATA_NUM0	(0x0030)

//U
#define DMA_DATA_ADDR1	(0x0098)
#define DMA_STRIDE1	(0x00A0)
#define DMA_STRETCH_STRIDE1	(0x00BC)
#define DMA_DATA_NUM1	(0x00A4)

//V
#define DMA_DATA_ADDR2	(0x009C)
#define DMA_STRIDE2	(0x0084)
#define DMA_STRETCH_STRIDE2	(0x00E8)
#define DMA_DATA_NUM2	(0x00EC)

//CH
#define CH_RD_SHADOW	(0x00F0)
#define CH_SIZE	(0x00F4)
#define CH_ENABLE	(0x00F8)
#define CH_CTL	(0x00FC)


/* MACROS */
#define MIN_INTERLEAVE	(7)
#define MAX_TILE_SURPORT_NUM	(6)

/* DMA aligned limited:  128bits aligned */
#define DMA_ALIGN_BYTES	(128 / BITS_PER_BYTE)
#define DMA_ADDR_ALIGN	(128 / BITS_PER_BYTE)
#define DMA_STRIDE_ALIGN	(128 / BITS_PER_BYTE)

#define DMA_IN_WIDTH_MAX	(2048)
#define DMA_IN_HEIGHT_MAX	(8192)


typedef struct dss_rdma {
	uint32_t oft_x0;
	uint32_t oft_y0;
	uint32_t oft_x1;
	uint32_t oft_y1;
	uint32_t mask0;
	uint32_t mask1;
	uint32_t stretch_size_vrt;
	uint32_t ctrl;
	uint32_t tile_scram;

	uint32_t data_addr0;
	uint32_t stride0;
	uint32_t stretch_stride0;
	uint32_t data_num0;

	uint32_t data_addr1;
	uint32_t stride1;
	uint32_t stretch_stride1;
	uint32_t data_num1;

	uint32_t data_addr2;
	uint32_t stride2;
	uint32_t stretch_stride2;
	uint32_t data_num2;

	uint32_t ch_rd_shadow;
	uint32_t ch_size;
	uint32_t ch_enable;
	uint32_t ch_ctl;
} dss_rdma_t;


/*******************************************************************************
** DFC
*/
#define DFC_DISP_SIZE	(0x0100)
#define DFC_PIX_IN_NUM	(0x0104)
#define DFC_GLB_ALPHA	(0x0108)
#define DFC_DISP_FMT	(0x010C)
#define DFC_CLIP_CTL_HRZ	(0x0110)
#define DFC_CLIP_CTL_VRZ	(0x0114)
#define DFC_CTL_CLIP_EN	(0x0118)
#define DFC_ICG_MODULE	(0x011C)
#define DFC_DITHER_ENABLE	(0x0120)
#define DFC_PADDING_CTL	(0x0124)

typedef struct dss_dfc {
	uint32_t disp_size;
	uint32_t pix_in_num;
	uint32_t disp_fmt;
	uint32_t clip_ctl_hrz;
	uint32_t clip_ctl_vrz;
	uint32_t ctl_clip_en;
	uint32_t icg_module;
	uint32_t dither_enable;
	uint32_t padding_ctl;
} dss_dfc_t;


/*******************************************************************************
** CSC
*/
#define CSC_IDC	(0x0500)
#define CSC_ODC	(0x0504)
#define CSC_P0	(0x0508)
#define CSC_P1	(0x050C)
#define CSC_P2	(0x0510)
#define CSC_P3	(0x0514)
#define CSC_P4	(0x0518)
#define CSC_ICG_MODULE	(0x051C)

typedef struct dss_csc {
	uint32_t idc;
	uint32_t odc;
	uint32_t p0;
	uint32_t p1;
	uint32_t p2;
	uint32_t p3;
	uint32_t p4;
	uint32_t icg_module;
} dss_csc_t;


/*******************************************************************************
** SCF
*/
#define DSS_SCF_H0_Y_COEF_OFFSET	(-0x3000)
#define DSS_SCF_Y_COEF_OFFSET	(-0x1000)
#define DSS_SCF_UV_COEF_OFFSET	(-0x800)

#define SCF_EN_HSCL_STR	(0x0000)
#define SCF_EN_VSCL_STR	(0x0004)
#define SCF_H_V_ORDER	(0x0008)
#define SCF_SCF_CORE_GT	(0x000C)
#define SCF_INPUT_WIDTH_HEIGHT	(0x0010)
#define SCF_OUTPUT_WIDTH_HEIGHT	(0x0014)
#define SCF_COEF_PM	(0x0018)
#define SCF_EN_HSCL	(0x001C)
#define SCF_EN_VSCL	(0x0020)
#define SCF_ACC_HSCL	(0x0024)
#define SCF_ACC_HSCL1	(0x0028)
#define SCF_INC_HSCL	(0x0034)
#define SCF_ACC_VSCL	(0x0038)
#define SCF_ACC_VSCL1	(0x003C)
#define SCF_INC_VSCL	(0x0048)
#define SCF_EN_NONLINEAR	(0x004C)
#define SCF_EN_MMP	(0x007C)
#define SCF_DB_H0	(0x0080)
#define SCF_DB_H1	(0x0084)
#define SCF_DB_V0	(0x0088)
#define SCF_DB_V1	(0x008C)
#define SCF_LB_PM	(0x0090)
#define SCF_RD_SHADOW	(0x00F0)


/* MACROS */
/* SCF min input pix 6x6 */
#define SCF_MIN_INPUT	(6)
/* SCF min output pix 4x4 */
#define SCF_MIN_OUTPUT	(4)
/* if SCF input smaller than SCF_MIN_INPUT, then read more data */
#define SCF_MIN_OFFSET	(SCF_MIN_INPUT - 3)

/* Threshold for SCF Stretch and SCF filter */
#define SCF_STRETCH_THRESHOLD	(2)
#define SCF_CONST_FACTOR	(262144)
#define SCF_UPSCALE_MAX	(8)
#define SCF_DOWNSCALE_MAX	(64)
#define SCF_EDGE_FACTOR (3)

typedef struct dss_scl {
	uint32_t en_hscl_str;
	uint32_t en_vscl_str;
	uint32_t h_v_order;
	uint32_t input_width_height;
	uint32_t output_width_height;
	uint32_t en_hscl;
	uint32_t en_vscl;
	uint32_t acc_hscl;
	uint32_t inc_hscl;
	uint32_t inc_vscl;
} dss_scl_t;


/*******************************************************************************
** SHARPNESS
*/
#define SHARPNESS_FILT_C	(0x0300)
#define SHARPNESS_MODE	(0x0304)
#define SHARPNESS_GAIN		(0x0308)
#define SHARPNESS_WEIGHT	(0x030C)
#define SHARPNESS_DBG0	(0x0310)
#define SHARPNESS_DBG1	(0x0314)
#define SHARPNESS_MODULE_CORE_GT	(0x0318)

typedef struct dss_sharpness {
	uint32_t filt_c;
	uint32_t mode;
	uint32_t gain;
	uint32_t weight;
	uint32_t dbg0;
	uint32_t dbg1;
	uint32_t module_core_gt;
} dss_sharpness_t;


/*******************************************************************************
** CE
*/
#define CE_HIST_RESET	(0x0400)
#define CE_HIST_CTL	(0x0404)
#define CE_HIST_FRAME_CNT	(0x0408)
#define CE_SIZE	(0x40C)
#define CE_NO_STAT_LINES	(0x410)
#define CE_BLACK_REGION_THRE	(0x414)
#define CE_WHITE_REGION_THRE	(0x418)
#define CE_LUT_SEL	(0x41C)
#define CE_BYPASS	(0x420)
#define CE_LUT_USING_IND	(0x454)
#define CE_STATE_IND	(0x458)
#define CE_MEM_CTRL0	(0x45c)
#define CE_MEM_CTRL1	(0x0460)

#define CE_TOTALGRAY	(0x0430)
#define CE_TOTALPIXL	(0x0434)
#define CE_MEAN	(0x0438)
#define CE_PEC0_3	(0x043C)
#define CE_PEC4_7	(0x0440)
#define CE_PEC8	(0x0444)
#define CE_BLACK_NUM	(0x0448)
#define CE_WHITE_NUM	(0x044C)
#define CE_HIST_RPT_IND	(0x0450)

typedef struct dss_ce {
	uint32_t hist_reset;
	uint32_t hist_ctl;
	uint32_t hist_frame_cnt;
	uint32_t size;
	uint32_t no_stat_lines;
	uint32_t black_region_thre;
	uint32_t white_region_thre;
	uint32_t lut_sel;
	uint32_t bypass;
	uint32_t lut_using_ind;
	uint32_t state_ind;
	uint32_t mem_ctrl0;
	uint32_t mem_ctrl1;

	uint32_t totalgray;
	uint32_t totalpixl;
	uint32_t mean;
	uint32_t pec0_3;
	uint32_t pec4_7;
	uint32_t pec8;
	uint32_t black_num;
	uint32_t white_num;
	uint32_t hist_rpt_ind;
} dss_ce_t;


/*******************************************************************************
** MCTL
*/
#define MCTL_SECU_OS	(0x0000)
#define MCTL_FLUSH_SECU	(0x0004)
#define MCTL_SECU_CFG	(0x0010)
#define MCTL_DMA_SECU	(0x0014)
#define MCTL_SECU_VIG_GATE	(0x0020)
#define MCTL_SECU_RGB_GATE	(0x0024)
#define MCTL_SECU_DMA_GATE	(0x0028)
#define MCTL_SECU_WCH_GATE	(0x002C)

#define MCTL_RD_SHADOW_CFG	(0x0100)
#define MCTL_CLK_GT	(0x0104)
#define MCTL_VIG_OV0_CFG	(0x0110)
#define MCTL_RGB_OV0_CFG	(0x0114)
#define MCTL_DMA_OV0_CFG	(0x0118)
#define MCTL_VIG_OV1_CFG	(0x011C)
#define MCTL_RGB_OV1_CFG	(0x0120)
#define MCTL_DMA_OV1_CFG	(0x0124)
#define MCTL_VIG_OV2_CFG	(0x0128)
#define MCTL_RGB_OV2_CFG	(0x012C)
#define MCTL_DMA_OV2_CFG	(0x0130)
#define MCTL_VIG_OV3_CFG	(0x0134)
#define MCTL_RGB_OV3_CFG	(0x0138)
#define MCTL_DMA_OV3_CFG	(0x013C)
#define MCTL_3D_SEL	(0x0150)
#define MCTL_TOP	(0x0154)
#define MCTL_FLUSH	(0x0158)
#define MCTL_RELOAD	(0x015C)
#define MCTL_START_MODE	(0x0170)
#define MCTL_START	(0x0174)
#define MCTL_CLEAR_MODE	(0x0180)
#define MCTL_CLEAR	(0x0184)
#define MCTL_CLR_ACK_TIMEOUT	(0x0188)
#define MCTL_CLR_ACK_STATUS	(0x018C)
#define MCTL_CMD_CFG	(0x01A0)
#define MCTL_CMD_FEND	(0x01A4)
#define MCTL_DEBUG_CFG	(0x01B0)
#define MCTL_CLK_SEL	(0x01B4)
#define MCTL_CLK_EN	(0x01B8)

typedef struct dss_mctl {
	uint32_t chn_ovl_cfg[DSS_CHN_MAX];

	uint32_t mctl_flush;
} dss_mctl_t;


/*******************************************************************************
** OVL
*/
#define OVL_SIZE	(0x0000)
#define OVL_BG_COLOR	(0x4)
#define OVL_DST_STARTPOS	(0x8)
#define OVL_DST_ENDPOS	(0xC)
#define OVL_GCFG	(0x10)
#define OVL_LAYER0_POS	(0x14)
#define OVL_LAYER0_SIZE	(0x18)
#define OVL_LAYER0_SRCLOKEY	(0x1C)
#define OVL_LAYER0_SRCHIKEY	(0x20)
#define OVL_LAYER0_DSTLOKEY	(0x24)
#define OVL_LAYER0_DSTHIKEY	(0x28)
#define OVL_LAYER0_PATTERN	(0x2C)
#define OVL_LAYER0_ALPHA	(0x30)
#define OVL_LAYER0_CFG	(0x34)
#define OVL_LAYER0_INFO_ALPHA	(0x38)
#define OVL_LAYER0_INFO_SRCCOLOR	(0x3C)
#define OVL_LAYER1_POS	(0x40)
#define OVL_LAYER1_SIZE	(0x44)
#define OVL_LAYER1_SRCLOKEY	(0x48)
#define OVL_LAYER1_SRCHIKEY	(0x4C)
#define OVL_LAYER1_DSTLOKEY	(0x50)
#define OVL_LAYER1_DSTHIKEY	(0x54)
#define OVL_LAYER1_PATTERN	(0x58)
#define OVL_LAYER1_ALPHA	(0x5C)
#define OVL_LAYER1_CFG	(0x60)
#define OVL_LAYER1_INFO_ALPHA	(0x64)
#define OVL_LAYER1_INFO_SRCCOLOR	(0x68)
#define OVL_LAYER2_POS	(0x6C)
#define OVL_LAYER2_SIZE	(0x70)
#define OVL_LAYER2_SRCLOKEY	(0x74)
#define OVL_LAYER2_SRCHIKEY	(0x78)
#define OVL_LAYER2_DSTLOKEY	(0x7C)
#define OVL_LAYER2_DSTHIKEY	(0x80)
#define OVL_LAYER2_PATTERN	(0x84)
#define OVL_LAYER2_ALPHA	(0x88)
#define OVL_LAYER2_CFG	(0x8C)
#define OVL_LAYER2_INFO_ALPHA	(0x90)
#define OVL_LAYER2_INFO_SRCCOLOR	(0x94)
#define OVL_LAYER3_POS	(0x98)
#define OVL_LAYER3_SIZE	(0x9C)
#define OVL_LAYER3_SRCLOKEY	(0xA0)
#define OVL_LAYER3_SRCHIKEY	(0xA4)
#define OVL_LAYER3_DSTLOKEY	(0xA8)
#define OVL_LAYER3_DSTHIKEY	(0xAC)
#define OVL_LAYER3_PATTERN	(0xB0)
#define OVL_LAYER3_ALPHA	(0xB4)
#define OVL_LAYER3_CFG	(0xB8)
#define OVL_LAYER3_INFO_ALPHA	(0xBC)
#define OVL_LAYER3_INFO_SRCCOLOR	(0xC0)
#define OVL_LAYER4_POS	(0xC4)
#define OVL_LAYER4_SIZE	(0xC8)
#define OVL_LAYER4_SRCLOKEY	(0xCC)
#define OVL_LAYER4_SRCHIKEY	(0xD0)
#define OVL_LAYER4_DSTLOKEY	(0xD4)
#define OVL_LAYER4_DSTHIKEY	(0xD8)
#define OVL_LAYER4_PATTERN	(0xDC)
#define OVL_LAYER4_ALPHA	(0xE0)
#define OVL_LAYER4_CFG	(0xE4)
#define OVL_LAYER4_INFO_ALPHA	(0xE8)
#define OVL_LAYER4_INFO_SRCCOLOR	(0xEC)
#define OVL_LAYER5_POS	(0xF0)
#define OVL_LAYER5_SIZE	(0xF4)
#define OVL_LAYER5_SRCLOKEY	(0xF8)
#define OVL_LAYER5_SRCHIKEY	(0xFC)
#define OVL_LAYER5_DSTLOKEY	(0x100)
#define OVL_LAYER5_DSTHIKEY	(0x104)
#define OVL_LAYER5_PATTERN	(0x108)
#define OVL_LAYER5_ALPHA	(0x10C)
#define OVL_LAYER5_CFG	(0x110)
#define OVL_LAYER5_INFO_ALPHA	(0x114)
#define OVL_LAYER5_INFO_SRCCOLOR	(0x118)
#define OVL_LAYER6_POS	(0x11C)
#define OVL_LAYER6_SIZE	(0x120)
#define OVL_LAYER6_SRCLOKEY	(0x124)
#define OVL_LAYER6_SRCHIKEY	(0x128)
#define OVL_LAYER6_DSTLOKEY	(0x12C)
#define OVL_LAYER6_DSTHIKEY	(0x130)
#define OVL_LAYER6_PATTERN	(0x134)
#define OVL_LAYER6_ALPHA	(0x138)
#define OVL_LAYER6_CFG	(0x13C)
#define OVL_LAYER6_INFO_ALPHA	(0x140)
#define OVL_LAYER6_INFO_SRCCOLOR	(0x144)
#define OVL_LAYER7_POS	(0x148)
#define OVL_LAYER7_SIZE	(0x14C)
#define OVL_LAYER7_SRCLOKEY	(0x150)
#define OVL_LAYER7_SRCHIKEY	(0x154)
#define OVL_LAYER7_DSTLOKEY	(0x158)
#define OVL_LAYER7_DSTHIKEY	(0x15C)
#define OVL_LAYER7_PATTERN	(0x160)
#define OVL_LAYER7_ALPHA	(0x164)
#define OVL_LAYER7_CFG	(0x168)
#define OVL_LAYER7_INFO_ALPHA	(0x16C)
#define OVL_LAYER7_INFO_SRCCOLOR	(0x170)
#define OVL_LAYER0_DST_STALL_INFO	(0x174)
#define OVL_LAYER0_ST_INFO	(0x178)
#define OVL_LAYER1_ST_INFO	(0x17C)
#define OVL_LAYER2_ST_INFO	(0x180)
#define OVL_LAYER3_ST_INFO	(0x184)
#define OVL_LAYER4_ST_INFO	(0x188)
#define OVL_LAYER5_ST_INFO	(0x18C)
#define OVL_LAYER6_ST_INFO	(0x190)
#define OVL_LAYER7_ST_INFO	(0x194)
#define OVL_LAYER0_DST_ISTALL_INFO	(0x198)
#define OVL_LAYER0_IST_INFO	(0x19C)
#define OVL_LAYER1_IST_INFO	(0x1A0)
#define OVL_LAYER2_IST_INFO	(0x1A4)
#define OVL_LAYER3_IST_INFO	(0x1A8)
#define OVL_LAYER4_IST_INFO	(0x1AC)
#define OVL_LAYER5_IST_INFO	(0x1B0)
#define OVL_LAYER6_IST_INFO	(0x1B4)
#define OVL_LAYER7_IST_INFO	(0x1B8)
#define OVL_GATE_CTRL	(0x1BC)
#define OVL_LAYER0_PSPOS	(0x1C0)
#define OVL_LAYER0_PEPOS	(0x1C4)
#define OVL_LAYER1_PSPOS	(0x1C8)
#define OVL_LAYER1_PEPOS	(0x1CC)
#define OVL_LAYER2_PSPOS	(0x1D0)
#define OVL_LAYER2_PEPOS	(0x1D4)
#define OVL_LAYER3_PSPOS	(0x1D8)
#define OVL_LAYER3_PEPOS	(0x1DC)
#define OVL_LAYER4_PSPOS	(0x1E0)
#define OVL_LAYER4_PEPOS	(0x1E4)
#define OVL_LAYER5_PSPOS	(0x1E8)
#define OVL_LAYER5_PEPOS	(0x1EC)
#define OVL_LAYER6_PSPOS	(0x1F0)
#define OVL_LAYER6_PEPOS	(0x1F4)
#define OVL_LAYER7_PSPOS	(0x1F8)
#define OVL_LAYER7_PEPOS	(0x1FC)
#define OVL_RD_SHADOW_SEL	(0x200)


/* LAYER0_CFG */
#define BIT_OVL_LAYER_SRC_CFG	BIT(8)
#define BIT_OVL_LAYER_ENABLE	BIT(0)

/* LAYER0_INFO_ALPHA */
#define BIT_OVL_LAYER_SRCALPHA_FLAG	BIT(3)
#define BIT_OVL_LAYER_DSTALPHA_FLAG	BIT(2)

/* LAYER0_INFO_SRCCOLOR */
#define BIT_OVL_LAYER_SRCCOLOR_FLAG	BIT(0)


#define OVL_LAYER_NUM	(6)

typedef struct dss_ovl_layer {
	uint32_t layer_pos;
	uint32_t layer_size	;
	uint32_t layer_pattern;
	uint32_t layer_alpha;
	uint32_t layer_cfg;

} dss_ovl_layer_t;

typedef struct dss_ovl_layer_pos {
	uint32_t layer_pspos;
	uint32_t layer_pepos;

} dss_ovl_layer_pos_t;

typedef struct dss_ovl {
	uint32_t ovl_size;
	uint32_t ovl_bg_color;
	uint32_t ovl_dst_startpos;
	uint32_t ovl_dst_endpos;
	uint32_t ovl_gcfg;
	dss_ovl_layer_t ovl_layer[OVL_LAYER_NUM];
	dss_ovl_layer_pos_t ovl_layer_pos[OVL_LAYER_NUM];

} dss_ovl_t;


/*******************************************************************************
** DBUF
*/
#define DBUF_FRM_SIZE	(0x0000)
#define DBUF_FRM_HSIZE	(0x0004)
#define DBUF_SRAM_VALID_NUM	(0x0008)
#define DBUF_WBE_EN	(0x000C)
#define DBUF_THD_FILL_LEV0	(0x0010)
#define DBUF_DFS_FILL_LEV1	(0x0014)
#define DBUF_THD_RQOS	(0x0018)
#define DBUF_THD_WQOS	(0x001C)
#define DBUF_THD_CG	(0x0020)
#define DBUF_THD_OTHER	(0x0024)
#define DBUF_FILL_LEV0_CNT	(0x0028)
#define DBUF_FILL_LEV1_CNT	(0x002C)
#define DBUF_FILL_LEV2_CNT	(0x0030)
#define DBUF_FILL_LEV3_CNT	(0x0034)
#define DBUF_FILL_LEV4_CNT	(0x0038)
#define DBUF_ONLINE_FILL_LEVEL	(0x003C)
#define DBUF_WB_FILL_LEVEL	(0x0040)
#define DBUF_DFS_STATUS	(0x0044)
#define DBUF_DFS_CLK_GT	(0x0048)
#define DBUF_DFS_LP_CTRL	(0x004C)
#define DBUF_RD_SHADOW_SEL	(0x0050)


/*******************************************************************************
** SBL
*/
#define SBL_DATA_FORMAT	(0x0000)
#define SBL_CONTROL_REG1	(0x0004)
#define SBL_HS_POS_L	(0x0008)
#define SBL_HS_POS_H	(0x000C)
#define SBL_FRAME_WIDTH_L	(0x0010)
#define SBL_FRAME_WIDTH_H	(0x0014)
#define SBL_FRAME_HEIGHT_L	(0x0018)
#define SBL_FRAME_HEIGHT_H	(0x001C)
#define SBL_VS_POS_L	(0x0020)
#define SBL_VS_POS_H	(0x0024)
#define SBL_ENABLE	(0x0400)
#define SBL_STRENGTH	(0x0408)
#define SBL_VARIANCE_INTENSITY_SPACE	(0x040C)
#define SBL_SLOPE_MAX	(0x0410)
#define SBL_SLOPE_MIN	(0x0414)
#define SBL_BLACK_LEVEL_L	(0x0418)
#define SBL_BLACK_LEVEL_H	(0x041C)
#define SBL_WHITE_LEVEL_L	(0x0420)
#define SBL_WHITE_LEVEL_H	(0x0424)
#define SBL_BRIGHT_DARK_AMP_LIMIT	(0x0428)
#define SBL_DITHER_MODE	(0x042C)
#define SBL_CONFIG_BUFFER_MODE	(0x0430)
#define SBL_CONFIG_BUFFER_GLOBAL	(0x0434)
#define SBL_ASYMMETRY_LUT_ADDR	(0x0480)
#define SBL_ASYMMETRY_LUT_WRITE_DATA_L	(0x0484)
#define SBL_ASYMMETRY_LUT_WRITE_DATA_H		(0x0488)
#define SBL_COLOR_CORRECTION_LUT_ADDR	(0x04C0)
#define SBL_COLOR_CORRECTION_LUT_WRITE_DATA_L	(0x04C4)
#define SBL_COLOR_CORRECTION_LUT_WRITE_DATA_H	(0x04C8)
#define SBL_LOGO_LEFT	(0x0800)
#define SBL_LOGO_TOP	(0x0804)
#define SBL_APICAL_DITHER	(0x0840)
#define SBL_AUTOMATIC_START_CALC_STRENGTH_DRC_BACKLIGHT_SEL	(0x0C00)
#define SBL_STRENGTH_MANUAL	(0x0C04)
#define SBL_DRC_IN_L	(0x0C08)
#define SBL_DRC_IN_H	(0x0C0C)
#define SBL_BACKLIGHT_L	(0x0C10)
#define SBL_BACKLIGHT_H	(0x0C14)
#define SBL_AMBIENT_LIGHT_L	(0x0C18)
#define SBL_AMBIENT_LIGHT_H	(0x0C1C)
#define SBL_START_CALC	(0x0C20)
#define SBL_STRENGTH_LIMIT	(0x0C24)
#define SBL_CALIBRATION_A_L	(0x0C28)
#define SBL_CALIBRATION_A_H	(0x0C2C)
#define SBL_CALIBRATION_B_L	(0x0C30)
#define SBL_CALIBRATION_B_H	(0x0C34)
#define SBL_CALIBRATION_C_L	(0x0C38)
#define SBL_CALIBRATION_C_H	(0x0C3C)
#define SBL_CALIBRATION_D_L	(0x0C40)
#define SBL_CALIBRATION_D_H	(0x0C44)
#define SBL_T_FILTER_CONTROL	(0x0C48)
#define SBL_BACKLIGHT_MIN_L	(0x0C4C)
#define SBL_BACKLIGHT_MIN_H	(0x0C50)
#define SBL_BACKLIGHT_MAX_L	(0x0C54)
#define SBL_BACKLIGHT_MAX_H	(0x0C58)
#define SBL_BACKLIGHT_SCALE_L	(0x0C5C)
#define SBL_BACKLIGHT_SCALE_H	(0x0C60)
#define SBL_AMBIENT_LIGHT_MIN_L	(0x0C64)
#define SBL_AMBIENT_LIGHT_MIN_H	(0x0C68)
#define SBL_FILTER_A_L	(0x0C6C)
#define SBL_FILTER_A_H	(0x0C70)
#define SBL_FILTER_B	(0x0C74)
#define SBL_STRENGTH_OUT	(0x0C80)
#define SBL_DRC_OUT_L	(0x0C84)
#define SBL_DRC_OUT_H	(0x0C88)
#define SBL_BACKLIGHT_OUT_L	(0x0C8C)
#define SBL_BACKLIGHT_OUT_H	(0x0C90)
#define SBL_CALC_DONE	(0x0C94)
#define SBL_AL_CALIB_LUT_LUT_ADDR	(0x0F00)
#define SBL_AL_CALIB_LUT_LUT_WRITE_DATA_L	(0x0F10)
#define SBL_AL_CALIB_LUT_LUT_WRITE_DATA_H	(0x0F14)

typedef struct dss_sbl {
	int sbl_backlight_l;
	int sbl_backlight_h;
	int sbl_ambient_light_l;
	int sbl_ambient_light_h;
	int sbl_calibration_a_l;
	int sbl_calibration_a_h;
	int sbl_calibration_b_l;
	int sbl_calibration_b_h;
	int sbl_calibration_c_l;
	int sbl_calibration_c_h;
	int sbl_calibration_d_l;
	int sbl_calibration_d_h;
	int sbl_enable;
} dss_sbl_t;


/*******************************************************************************
** DPP
*/
#define DPP_RD_SHADOW_SEL	(0x0000)

#define DPP_ID	(0x0100)
#define DPP_IMG_HRZ	(0x0104)
#define DPP_IMG_VRT	(0x0108)
#define DPP_CLRBAR_CTRL	(0x010C)
#define DPP_DITHER	(0x0110)
#define DPP_SBL	(0x0114)

//LCP
#define LCP_XCC_COEF_00	(0x0200)
#define LCP_XCC_COEF_01	(0x0204)
#define LCP_XCC_COEF_02	(0x0208)
#define LCP_XCC_COEF_03	(0x020C)
#define LCP_XCC_COEF_10	(0x0210)
#define LCP_XCC_COEF_11	(0x0214)
#define LCP_XCC_COEF_12	(0x0218)
#define LCP_XCC_COEF_13	(0x021C)
#define LCP_XCC_COEF_20	(0x0220)
#define LCP_XCC_COEF_21	(0x0224)
#define LCP_XCC_COEF_22	(0x0228)
#define LCP_XCC_COEF_23	(0x022C)
#define LCP_GMP_BYPASS_EN	(0x0230)
#define LCP_XCC_BYPASS_EN	(0x0234)
#define LCP_IGM_GT	(0x0238)
#define LCP_GMP_GT	(0x023C)
#define LCP_XCC_GT	(0x0240)
#define LCP_IGM_MEM_CTRL_S	(0x0244)
#define LCP_GMP_MEM_CTRL_S	(0x0248)
#define LCP_XCC_MEM_CTRL_S	(0x024C)

//ACM
#define ACM_R0_H	(0x0400)
#define ACM_R1_H	(0x0404)
#define ACM_R2_H	(0x0408)
#define ACM_R3_H	(0x040C)
#define ACM_R4_H	(0x0410)
#define ACM_R5_H	(0x0414)
#define ACM_R6_H	(0x0418)
#define ACM_EN	(0x041C)
#define ACM_HIS_STAS_CLEAR	(0x0420)
#define ACM_INT	(0x0424)
#define ACM_INT_EN	(0x0428)
#define ACM_POLL_FLAG	(0x042C)
#define ACM_GATE_CTRL	(0x0430)
#define ACM_POWCTRL0	(0x0434)
#define ACM_POWCTRL1	(0x0438)

//GAMA
#define GAMA_SHORTBIT_EN	(0x0600)
#define GAMA_BYPASS_EN	(0x0604)
#define GAMA_CORE_GT	(0x0608)
#define GAMA_MEM_CTRL_S	(0x060C)


/*******************************************************************************
** IFBC
*/
#define IFBC_SIZE	(0x0000)
#define IFBC_CTRL	(0x0004)
#define IFBC_HIMAX_CTRL0	(0x0008)
#define IFBC_HIMAX_CTRL1	(0x000C)
#define IFBC_HIMAX_CTRL2	(0x0010)
#define IFBC_HIMAX_CTRL3	(0x0014)
#define IFBC_EN	(0x0018)
#define IFBC_PM_CTRL	(0x001C)
#define IFBC_MEM_CTRL	(0x0020)
#define IFBC_HIMAX_TEST_MODE	(0x0024)
#define IFBC_CORE_GT	(0x0028)
#define IFBC_DAT_OSEL	(0x002C)
#define IFBC_RD_SHADOW	(0x0030)
#define IFBC_ORISE_CTL	(0x0034)
#define IFBC_ORSISE_DEBUG0	(0x0038)
#define IFBC_ORSISE_DEBUG1	(0x003C)
#define IFBC_RSP_COMP_TEST	(0x0040)


/*******************************************************************************
** LDI
*/
#define LDI_DPI0_HRZ_CTRL0	(0x0000)
#define LDI_DPI0_HRZ_CTRL1	(0x0004)
#define LDI_DPI0_HRZ_CTRL2	(0x0008)
#define LDI_VRT_CTRL0	(0x000C)
#define LDI_VRT_CTRL1	(0x0010)
#define LDI_VRT_CTRL2	(0x0014)
#define LDI_PLR_CTRL	(0x0018)
#define LDI_3D_CTRL	(0x0020)
#define LDI_CTRL	(0x0024)
#define LDI_WORK_MODE	(0x0028)
#define LDI_DE_SPACE_LOW	(0x002C)
#define LDI_DSI_CMD_MOD_CTRL	(0x0030)
#define LDI_DSI_TE_CTRL	(0x0034)
#define LDI_DSI_TE_HS_NUM	(0x0038)
#define LDI_DSI_TE_HS_WD	(0x003C)
#define LDI_DSI_TE_VS_WD	(0x0040)
#define LDI_FRM_MSK	(0x0044)
#define LDI_FRM_MSK_UP	(0x0048)
#define LDI_VINACT_MSK_LEN	(0x0050)
#define LDI_CG_CTRL	(0x0060)
#define LDI_SRAM_LP_CTRL	(0x0064)
#define LDI_ITF_RD_SHADOW	(0x006C)
#define LDI_DPI1_HRZ_CTRL0	(0x00F0)
#define LDI_DPI1_HRZ_CTRL1	(0x00F4)
#define LDI_DPI1_HRZ_CTRL2	(0x00F8)
#define LDI_OVERLAP_SIZE	(0x00FC)


/*******************************************************************************
** MIPI DSI
*/
#define DSS_MIPI_DSI0_OFFSET	(0x0000C000)
#define DSS_MIPI_DSI1_OFFSET	(0x0000C100)

#define MIPIDSI_VERSION_OFFSET			(0x00)
#define MIPIDSI_PWR_UP_OFFSET			(0x04)
#define MIPIDSI_CLKMGR_CFG_OFFSET		(0x08)
#define MIPIDSI_DPI_VCID_OFFSET			(0x0c)
#define MIPIDSI_DPI_COLOR_CODING_OFFSET	(0x10)
#define MIPIDSI_DPI_CFG_POL_OFFSET		(0x14)
#define MIPIDSI_DPI_LP_CMD_TIM_OFFSET	(0x18)
#define MIPIDSI_DBI_VCID_OFFSET			(0x1c)
#define MIPIDSI_DBI_CFG_OFFSET			(0x20)
#define MIPIDSI_DBI_PARTITIONING_EN_OFFSET	(0x24)
#define MIPIDSI_DBI_CMDSIZE_OFFSET		(0x28)
#define MIPIDSI_PCKHDL_CFG_OFFSET		(0x2c)
#define MIPIDSI_GEN_VCID_OFFSET			(0x30)
#define MIPIDSI_MODE_CFG_OFFSET			(0x34)
#define MIPIDSI_VID_MODE_CFG_OFFSET		(0x38)
#define MIPIDSI_VID_PKT_SIZE_OFFSET		(0x3c)
#define MIPIDSI_VID_NUM_CHUNKS_OFFSET	(0x40)
#define MIPIDSI_VID_NULL_SIZE_OFFSET		(0x44)
#define MIPIDSI_VID_HSA_TIME_OFFSET		(0x48)
#define MIPIDSI_VID_HBP_TIME_OFFSET		(0x4c)
#define MIPIDSI_VID_HLINE_TIME_OFFSET	(0x50)
#define MIPIDSI_VID_VSA_LINES_OFFSET		(0x54)
#define MIPIDSI_VID_VBP_LINES_OFFSET		(0x58)
#define MIPIDSI_VID_VFP_LINES_OFFSET		(0x5c)
#define MIPIDSI_VID_VACTIVE_LINES_OFFSET	(0x60)
#define MIPIDSI_EDPI_CMD_SIZE_OFFSET		(0x64)
#define MIPIDSI_CMD_MODE_CFG_OFFSET	(0x68)
#define MIPIDSI_GEN_HDR_OFFSET			(0x6c)
#define MIPIDSI_GEN_PLD_DATA_OFFSET		(0x70)
#define MIPIDSI_CMD_PKT_STATUS_OFFSET	(0x74)
#define MIPIDSI_TO_CNT_CFG_OFFSET		(0x78)
#define MIPIDSI_HS_RD_TO_CNT_OFFSET		(0x7C)
#define MIPIDSI_LP_RD_TO_CNT_OFFSET		(0x80)
#define MIPIDSI_HS_WR_TO_CNT_OFFSET		(0x84)
#define MIPIDSI_LP_WR_TO_CNT_OFFSET		(0x88)
#define MIPIDSI_BTA_TO_CNT_OFFSET		(0x8C)
#define MIPIDSI_SDF_3D_OFFSET			(0x90)
#define MIPIDSI_LPCLK_CTRL_OFFSET		(0x94)
#define MIPIDSI_PHY_TMR_LPCLK_CFG_OFFSET	(0x98)
#define MIPIDSI_PHY_TMR_CFG_OFFSET		(0x9c)
#define MIPIDSI_PHY_RSTZ_OFFSET			(0xa0)
#define MIPIDSI_PHY_IF_CFG_OFFSET		(0xa4)
#define MIPIDSI_PHY_ULPS_CTRL_OFFSET	(0xa8)
#define MIPIDSI_PHY_TX_TRIGGERS_OFFSET	(0xac)
#define MIPIDSI_PHY_STATUS_OFFSET		(0xb0)
#define MIPIDSI_PHY_TST_CTRL0_OFFSET	(0xb4)
#define MIPIDSI_PHY_TST_CTRL1_OFFSET	(0xb8)
#define MIPIDSI_INT_ST0_OFFSET			(0xbc)
#define MIPIDSI_INT_ST1_OFFSET			(0xc0)
#define MIPIDSI_INT_MSK0_OFFSET			(0xc4)
#define MIPIDSI_INT_MSK1_OFFSET			(0xc8)


/*******************************************************************************
**
*/
typedef struct dss_module_reg {
	char __iomem *bridge_base[DSS_RBRG_IDX_MAX];
	char __iomem *mmu_dma0_base[DSS_CHN_MAX];
	char __iomem *mmu_dma1_base[DSS_CHN_MAX];
	char __iomem *dma_base[DSS_CHN_MAX];
	char __iomem *dfc_base[DSS_CHN_MAX];
	char __iomem *scl_base[DSS_CHN_MAX];
	char __iomem *csc_base[DSS_CHN_MAX];
	char __iomem *ov_base[DSS_OVL_IDX_MAX];
	char __iomem *mctl_base[DSS_OVL_IDX_MAX];

	dss_rbridge_t bridge[DSS_RBRG_IDX_MAX];
	dss_mmu_dma_t mmu_dma0[DSS_CHN_MAX];
	dss_mmu_dma_t mmu_dma1[DSS_CHN_MAX];
	dss_rdma_t dma[DSS_CHN_MAX];
	dss_dfc_t dfc[DSS_CHN_MAX];
	dss_scl_t scl[DSS_CHN_MAX];
	dss_csc_t csc[DSS_CHN_MAX];
	dss_ovl_t ov[DSS_OVL_IDX_MAX];
	dss_mctl_t mctl[DSS_OVL_IDX_MAX];

	uint8_t bridge_used[DSS_RBRG_IDX_MAX];
	uint8_t mmu_dma0_used[DSS_CHN_MAX];
	uint8_t mmu_dma1_used[DSS_CHN_MAX];
	uint8_t dma_used[DSS_CHN_MAX];
	uint8_t dfc_used[DSS_CHN_MAX];
	uint8_t scl_used[DSS_CHN_MAX];
	uint8_t csc_used[DSS_CHN_MAX];
	uint8_t ov_used[DSS_OVL_IDX_MAX];
	uint8_t mctl_used[DSS_OVL_IDX_MAX];
} dss_module_reg_t;


#endif  /* HISI_DSS_REGS_H */
