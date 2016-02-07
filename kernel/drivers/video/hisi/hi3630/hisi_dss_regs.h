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
#define DSS_WIDTH(width)	((width) - 1)
#define DSS_HEIGHT(height)	((height) - 1)

#define DSS_PRI_CLK_RATE_MAX	(300 * 1000000L)
#define DSS_AUX_CLK_RATE_MAX	(300 * 1000000L)
#define DSS_PCLK_CLK_RATE_MAX	(120 * 1000000L)

#define RES_720P	(720 * 1280)
#define RES_1080P	(1920 * 1080)
#define RES_1600P	(2560 * 1600)

#define DFC_MAX_CLIP_NUM	(31)

/* for DFS */
/* 1480 * 144bits */
#define DFS_SRAM_ONE_MEM_DEPTH	(1408)

#define GET_THD_RQOS_IN(max_depth)	((max_depth) * 10 / 100)
#define GET_THD_RQOS_OUT(max_depth)	((max_depth) * 30 / 100)
#define GET_THD_WQOS_IN(max_depth)	((max_depth) * 95 / 100)
#define GET_THD_WQOS_OUT(max_depth)	((max_depth) * 70 / 100)
#define GET_THD_CG_IN(max_depth)	((max_depth) -1)
#define GET_THD_CG_OUT(max_depth)	((max_depth) * 70 / 100)
#define GET_THD_OTHER_DFS_CG_HOLD(max_depth)	(0x20)
#define GET_THD_OTHER_WR_WAIT(max_depth)	((max_depth) * 90 / 100)

#define GET_RDMA_ROT_HQOS_ASSERT_LEV(max_depth)	((max_depth) * 30 /100)
#define GET_RDMA_ROT_HQOS_REMOVE_LEV(max_depth)	((max_depth) * 60 / 100)

#define DSS_SCF_MAX	(5)
#define DSS_ROT_MAX	(5)

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

	/* YUV */
	DFC_PIXEL_FORMAT_YUV444,
	DFC_PIXEL_FORMAT_YVU444,
	DFC_PIXEL_FORMAT_YUYV422,
	DFC_PIXEL_FORMAT_YVYU422,
	DFC_PIXEL_FORMAT_VYUY422,
	DFC_PIXEL_FORMAT_UYVY422,
};

enum dss_dma_format {
	DMA_PIXEL_FORMAT_RGB16BIT = 0,
	DMA_PIXEL_FORMAT_RGB32BIT,

	/* YUV Interleaved(Package) */
	DMA_PIXEL_FORMAT_YUV_422_I,

	/* YUV Semi-planar */
	DMA_PIXEL_FORMAT_YUV_422_SP_HP_Y,
	DMA_PIXEL_FORMAT_YUV_422_SP_HP_UV,
	DMA_PIXEL_FORMAT_YUV_422_SP_VP_Y, /* not support now */
	DMA_PIXEL_FORMAT_YUV_422_SP_VP_UV, /*not support now */
	DMA_PIXEL_FORMAT_YUV_420_SP_Y,
	DMA_PIXEL_FORMAT_YUV_420_SP_UV,

	/* YUV Planar */
	DMA_PIXEL_FORMAT_YUV_422_P_HP_Y,
	DMA_PIXEL_FORMAT_YUV_422_P_HP_U,
	DMA_PIXEL_FORMAT_YUV_422_P_HP_V,
	DMA_PIXEL_FORMAT_YUV_422_P_VP_Y, /* not support now */
	DMA_PIXEL_FORMAT_YUV_422_P_VP_U, /* not support now */
	DMA_PIXEL_FORMAT_YUV_422_P_VP_V, /* not support now */
	DMA_PIXEL_FORMAT_YUV_420_P_Y,
	DMA_PIXEL_FORMAT_YUV_420_P_U,
	DMA_PIXEL_FORMAT_YUV_420_P_V,
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

enum dss_module_chn {
	MODULE_DMA0 = 0,
	MODULE_DMA1,
	MODULE_FBDC,
	MODULE_FBC,
	MODULE_DFC,
	MODULE_SCF,
	MODULE_SCP,
	MODULE_CSC,
	MODULE_MMU_DMA0,
	MODULE_MMU_DMA1,
	MODULE_MMU_RTLB,

	MODULE_GLB_CHN_CTL,
	MODULE_GLB_CHN_MUX,
	MODULE_DPE_DP_CTL,
	MODULE_ROT_PM_CTL,
	MODULE_ROT_MUX,
	MODULE_SCF_MUX,
	MODULE_CROSS_SWITCH,

	MODULE_CHN_MAX,
};

enum dss_module_ovl {
	MODULE_OVL_BASE,
	MODULE_OVL_SCENE,
	MODULE_OVL_MUX,
	MODULE_OVL_CFG_OK,
	MODULE_OVL_WBE_EN,

	MODULE_OVL_MAX,
};

enum dss_module_eng {
	MODULE_ENG_BASE,
	MODULE_ENG_DMA_BRG,
	MODULE_ENG_DP_CTL,

	MODULE_ENG_MAX,
};

enum dss_eng_type {
	DSS_ENG_DPE0 = 0,
	DSS_ENG_DPE1,
	DSS_ENG_DPE2,
	DSS_ENG_DPE3,

	DSS_ENG_WBE0,
	DSS_ENG_WBE1,

	DSS_ENG_MAX,
};

enum dss_ovl_type{
	DSS_OVL_PDP = 0,
	DSS_OVL_SDP,
	DSS_OVL_ADP,

	DSS_OVL_MAX,
};

enum dss_rdma_index {
	DSS_RDMA0 = 0,
	DSS_RDMA1,
	DSS_RDMA2,
	DSS_RDMA3,
	DSS_RDMA4,
};


/*******************************************************************************
** DSS Reg Value
*/

/*---------------PDP----------------*/
/* DSS_GLB_PDP_CPU_IRQ	(0x58C) */
#define BIT_PDP_S2_IRQ_CPU	BIT(9)
#define BIT_PDP_RTLB_CPU_IRQ	BIT(8)
#define BIT_PDP_DFS_IRQ_CPU	BIT(7)
#define BIT_CMDLIST_PDP_IRQ_CPU	BIT(6)
#define BIT_DPE2_MMU_IRQ_CPU	BIT(5)
#define BIT_DPE2_DMA_IRQ_CPU	BIT(4)
#define BIT_WBE0_MMU_IRQ_CPU	BIT(3)
#define BIT_DPE0_MMU_IRQ_CPU	BIT(2)
#define BIT_DPE0_DMA_IRQ_CPU	BIT(1)
#define BIT_PDP_LDI0_IRQ_CPU	BIT(0)

/*
** PDP_LDI_CPU_IRQ	(DSS_DPP0_CTRL_OFFSET + 0x300)
** SDP_LDI_CPU_IRQ	(DSS_DPP1_CTRL_OFFSET + 0x100)
*/
#define BIT_STRENGTH_INTP	BIT(14)
#define BIT_LDI_TE1_PIN_INT	BIT(13)
#define BIT_LDI_TE0_PIN_INT	BIT(12)
#define BIT_DSI_TE_TRI_INT	BIT(11)
#define BIT_VFRONTPOCH_END_INT	BIT(10)
#define BIT_VACTIVE1_END_INT	BIT(9)
#define BIT_VACTIVE1_START_INT	BIT(8)
#define BIT_VACTIVE0_END_INT	BIT(7)
#define BIT_VACTIVE0_START_INT	BIT(6)
#define BIT_VBACKPORCH_INT	BIT(5)
#define BIT_VFRONTPORCH_INT	BIT(4)
#define BIT_VSYNC_INT	BIT(3)
#define BIT_LDI_UNFLOW_INT	BIT(2)
#define BIT_FRM_END_INT	BIT(1)
#define BIT_BACKLIGHT_INT	BIT(0)

/*---------------SDP----------------*/
/* DSS_GLB_SDP_CPU_IRQ	(0x5E8) */
#define BIT_SDP_S2_IRQ_MCU		BIT(9)
#define BIT_SDP_RTLB_CPU_IRQ	BIT(8)
#define BIT_SDP_DFS_IRQ_MCU		BIT(7)
#define BIT_CMDLIST_SDP_IRQ_MCU	BIT(6)
#define BIT_DPE1_MMU_IRQ_MCU	BIT(2)
#define BIT_DPE1_DMA_IRQ_MCU	BIT(1)
#define BIT_SDP_LDI1_IRQ_MCU	BIT(0)

/*---------------ADP----------------*/
/* DSS_GLB_OFFLINE_CPU_IRQ_MSK	(0x684) */
#define BIT_CMDLIST_IRQ_CPU_OFF	BIT(11)
#define BIT_WBE0_MMU_IRQ_CPU_OFF	BIT(8)
#define BIT_WBE1_MMU_IRQ_CPU_OFF	BIT(3)
#define BIT_OFFLINE_S2_IRQ_CPU_OFF	BIT(0)

/* OFFLINE_WBE1_CPU_IRQ_MSK	(0x124) */
#define BIT_OFFLINE_WBE1_CH1_FRM_END_CPU	BIT(25)
#define BIT_OFFLINE_WBE1_CH0_FRM_END_CPU	BIT(24)

/*---------------WBE0----------------*/
/* PDP_WBE0_CPU_IRQ	(0x140) */
#define BIT_WBE0_FRAME_END_CPU_CH1	BIT(25)
#define BIT_WBE0_FRAME_END_CPU_CH0	BIT(24)


/* GLB EN_CLK_CFG(0x408) & DIS_CLK_CFG(0x420) */
#define BIT_DIV4_GT	BIT(28)
#define BIT_DIV2_GT	BIT(27)
#define BIT_CLK_PXL0DIV4	BIT(24)
#define BIT_CLK_PXL0DIV2	BIT(23)
#define BIT_CLK_AXI	BIT(22)
#define BIT_DSI1_PCLK	BIT(21)
#define BIT_DSI0_PCLK	BIT(20)
#define BIT_CLK_PDPPXL0	BIT(19)
#define BIT_CLK_ADPDPE3	BIT(12)
#define BIT_CLK_ADPAXI	BIT(11)
#define BIT_CLK_ADPDPE2	BIT(10)
#define BIT_CLK_DPE2AXI	BIT(9)
#define BIT_CLK_HDMIPXL1	BIT(8)
#define BIT_CLK_SDPPXL1	BIT(7)
#define BIT_CLK_SDPPRI	BIT(6)
#define BIT_CLK_SDPAXI	BIT(5)
#define BIT_CLK_PXL0DIV	BIT(4)
#define BIT_CLK_PDPDSI1	BIT(3)
#define BIT_CLK_PDPDSI0	BIT(2)
#define BIT_CLK_PDPPRI	BIT(1)
#define BIT_CLK_PDPAXI	BIT(0)

/* GLB EN_RST_CFG(0x410) and DIS_RST_CFG(0x424) */
#define BIT_DSI1_PRST	BIT(27)
#define BIT_DSI0_PRST	BIT(26)
#define BIT_DPE2_AXI_RST		BIT(25)
#define BIT_HDMI_IO_RSTPIN	BIT(24)
#define BIT_HDMIFC_PRST	BIT(23)
#define BIT_HDMI_PRST	BIT(22)
#define BIT_ADP_DPE3_RST	BIT(21)
#define BIT_ADP_DPE2_RST	BIT(20)
#define BIT_ADP_AXI_RST	BIT(19)
#define BIT_SDP_AFRCLR_MASK	BIT(18)
#define BIT_SDP_AFR_RST	BIT(17)
#define BIT_SDP_AFWCLR_MASK	BIT(16)
#define BIT_SDP_AFW_RST	BIT(15)
#define BIT_SDP_PXL1_RST	BIT(14)
#define BIT_SDP_AXI_RST	BIT(13)
#define BIT_SDP_PRI_RST	BIT(12)
#define BIT_PDP_AFRCLR_MASK		BIT(11)
#define BIT_PDP_AFR_RST	BIT(10)
#define BIT_PDP_AFWCLR_MASK	BIT(9)
#define BIT_PDP_AFW_RST		BIT(8)
#define BIT_PDP_DSI1_RST	BIT(7)
#define BIT_PDP_DSI0_RST		BIT(6)
#define BIT_PDP_PXL0_RST	BIT(5)
#define BIT_PDP_PRI_RST	BIT(4)
#define BIT_PDP_AXI_RST	BIT(3)
#define BIT_ROT_RST	BIT(2)
#define BIT_DSS_PRI_RST	BIT(1)
#define BIT_DSS_AXI_RST	BIT(0)


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

#define DSS_DPE0_OFFSET	(0x00000000)
#define DSS_DPE1_OFFSET	(0x00020000)
#define DSS_DPE2_OFFSET	(0x00030000)
#define DSS_DPE3_OFFSET	(0x00040000)

#define DSS_WBE0_OFFSET	(DSS_DPE0_OFFSET)
#define DSS_WBE1_OFFSET	(DSS_DPE3_OFFSET)

#define DSS_DPP_OFFSET	(0x00010000)

#define DSS_GLB1_OFFSET	(0x00000000)
#define DSS_GLB2_OFFSET	(0x00020000)


/*******************************************************************************
** DSS GLB
*/

#define DSS_GLB_ROT1_SEL_SECU	(0x014)
#define DSS_GLB_TEST_SEL	(0x020)
#define DSS_GLB_ROT1_SEL_SECU_DIS	(0x044)
#define DSS_GLB_ROT1_SEL_SECU_STATUS	(0x064)
#define DSS_GLB_SECU_SEL_EN0	(0x068)
#define DSS_GLB_SECU_SEL_DIS0	(0x070)
#define DSS_GLB_SECU_SEL_STATUS0	(0x078)
#define DSS_GLB_PDP_S2_MCU_IRQ	(0x168)
#define DSS_GLB_PDP_S2_MCU_IRQ_MSK	(0x16C)
#define DSS_GLB_PDP_S2_MCU_IRQ_CLR	(0x170)
#define DSS_GLB_PDP_MCU_IRQ	(0x180)
#define DSS_GLB_PDP_MCU_IRQ_MSK	(0x184)
#define DSS_GLB_SDP_S2_MCU_IRQ	(0x1C4)
#define DSS_GLB_SDP_S2_MCU_IRQ_MSK	(0x1C8)
#define DSS_GLB_SDP_S2_MCU_IRQ_CLR	(0x1CC)
#define DSS_GLB_SDP_MCU_IRQ	(0x1DC)
#define DSS_GLB_SDP_MCU_IRQ_MSK	(0x1E0)
#define DSS_GLB_OFFLINE_S2_MCU_IRQ	(0x25C)
#define DSS_GLB_OFFLINE_S2_MCU_IRQ_MSK	(0x260)
#define DSS_GLB_OFFLINE_S2_MCU_IRQ_CLR	(0x264)
#define DSS_GLB_OFFLINE_MCU_IRQ	(0x274)
#define DSS_GLB_OFFLINE_MCU_IRQ_MSK	(0x278)
#define DSS_GLB_PDP_SECU_CFG_OK	(0x300)
#define DSS_GLB_SDP_SECU_CFG_OK	(0x304)
#define DSS_GLB_OFFLINE0_SECU_CFG_OK0	(0x308)
#define DSS_GLB_OFFLINE0_SECU_CFG_OK1	(0x30C)
#define DSS_GLB_OFFLINE1_SECU_CFG_OK0	(0x310)
#define DSS_GLB_OFFLINE1_SECU_CFG_OK1	(0x314)
#define DSS_GLB_DSS_ID	(0x400)
#define DSS_GLB_APB_CTL	(0x404)
#define DSS_GLB_EN_CLK_CFG	(0x408)
#define DSS_GLB_ST_CLK_CFG	(0x40C)
#define DSS_GLB_EN_RST_CFG	(0x410)
#define DSS_GLB_HDMI_TMDS_DATA_SEL	(0x414)
#define DSS_GLB_DSS_SEL	(0x418)
#define DSS_GLB_PXL0_DIV	(0x41C)
#define DSS_GLB_DIS_CLK_CFG	(0x420)
#define DSS_GLB_DIS_RST_CFG	(0x424)
#define DSS_GLB_CTL_CLK_CFG	(0x428)
#define DSS_GLB_CTL_RST_CFG	(0x42C)
#define DSS_GLB_DPE2_CLK_SW	(0x430)
#define DSS_GLB_ROT1_CLK_SW	(0x434)
#define DSS_GLB_ADP_CLK_SW	(0x438)
#define DSS_GLB_PRI_CLK_SW	(0x43C)
#define DSS_GLB_LDI0_FRM_END_DEFER	(0x440)
#define DSS_GLB_LDI1_FRM_END_DEFER	(0x444)
#define DSS_GLB_DEBUG_IRQ	(0x480)
#define DSS_GLB_DSS_CH_STATUS	(0x484)
#define DSS_GLB_CH_END_ACK_TIMEOUT	(0x490)
#define DSS_GLB_WBE0_WS_SYNC_MASK	(0x4A0)
#define DSS_GLB_WBE0_WO_SYNC_MASK	(0x4A4)
#define DSS_GLB_WBE1_WS_SYNC_MASK	(0x4A8)
#define DSS_GLB_WBE1_WO_SYNC_MASK	(0x4AC)
#define DSS_GLB_WBE0_WS_FRM_END_MASK	(0x4B0)
#define DSS_GLB_WBE0_WO_FRM_END_MASK	(0x4B4)
#define DSS_GLB_WBE1_WS_FRM_END_MASK	(0x4B8)
#define DSS_GLB_WBE1_WO_FRM_END_MASK	(0x4BC)
#define DSS_GLB_SYNC_MODE	(0x4C0)
#define DSS_GLB_GLB_STATUS	(0x4C4)
#define DSS_GLB_ROT1_PM_CTRL	(0x4D8)
#define DSS_GLB_ROT1_STATE	(0x4DC)
#define DSS_GLB_PDP_S2_CPU_IRQ_RAWSTAT	(0x52C)
#define DSS_GLB_SDP_S2_CPU_IRQ_RAWSTAT	(0x530)
#define DSS_GLB_OFFLINE_S2_CPU_IRQ_RAWSTAT	(0x534)
#define DSS_GLB_PDP_S2_CPU_IRQ	(0x574)
#define DSS_GLB_PDP_S2_CPU_IRQ_MSK	(0x578)
#define DSS_GLB_PDP_S2_CPU_IRQ_CLR	(0x57C)
#define DSS_GLB_PDP_CPU_IRQ	(0x58C)
#define DSS_GLB_PDP_CPU_IRQ_MSK	(0x590)
#define DSS_GLB_SDP_S2_CPU_IRQ	(0x5D0)
#define DSS_GLB_SDP_S2_CPU_IRQ_MSK	(0x5D4)
#define DSS_GLB_SDP_S2_CPU_IRQ_CLR	(0x5D8)
#define DSS_GLB_SDP_CPU_IRQ	(0x5E8)
#define DSS_GLB_SDP_CPU_IRQ_MSK	(0x5EC)
#define DSS_GLB_OFFLINE_S2_CPU_IRQ	(0x668)
#define DSS_GLB_OFFLINE_S2_CPU_IRQ_MSK	(0x66C)
#define DSS_GLB_OFFLINE_S2_CPU_IRQ_CLR	(0x670)
#define DSS_GLB_OFFLINE_CPU_IRQ	(0x680)
#define DSS_GLB_OFFLINE_CPU_IRQ_MSK	(0x684)
#define DSS_GLB_OV0_SCENE	(0x6F4)
#define DSS_GLB_OV1_SCENE	(0x6F8)
#define DSS_GLB_OV2_SCENE	(0x6FC)
#define DSS_GLB_DPE0_CH0_CTL	(0x700)
#define DSS_GLB_DPE0_CH1_CTL	(0x704)
#define DSS_GLB_DPE0_CH2_CTL	(0x708)
#define DSS_GLB_DPE0_CH3_CTL	(0x70C)
#define DSS_GLB_WBE0_CH0_CTL	(0x710)
#define DSS_GLB_WBE0_CH1_CTL	(0x714)
#define DSS_GLB_DPE1_CH0_CTL	(0x718)
#define DSS_GLB_DPE1_CH3_CTL	(0x71C)
#define DSS_GLB_DPE2_CH0_CTL	(0x720)
#define DSS_GLB_DPE2_CH1_CTL	(0x724)
#define DSS_GLB_DPE2_CH2_CTL	(0x728)
#define DSS_GLB_DPE2_CH3_CTL	(0x72C)
#define DSS_GLB_DPE3_CH0_CTL	(0x730)
#define DSS_GLB_DPE3_CH1_CTL	(0x734)
#define DSS_GLB_DPE3_CH2_CTL	(0x738)
#define DSS_GLB_DPE3_CH3_CTL	(0x73C)
#define DSS_GLB_WBE1_CH0_CTL	(0x740)
#define DSS_GLB_WBE1_CH1_CTL	(0x744)
#define DSS_GLB_ROT_TLB0_SCENE	(0x748)
#define DSS_GLB_ROT_TLB1_SCENE	(0x74C)
#define DSS_GLB_ROT_TLB2_SCENE	(0x750)
#define DSS_GLB_ROT_TLB3_SCENE	(0x754)
#define DSS_GLB_ROT_TLB4_SCENE	(0x758)
#define DSS_GLB_ROT1_SEL	(0x7A4)
#define DSS_GLB_PDP_ONLINE_CFG_OK	(0x800)
#define DSS_GLB_SDP_ONLINE_CFG_OK	(0x804)
#define DSS_GLB_PDP_OFFLINE_START0	(0x808)
#define DSS_GLB_PDP_OFFLINE_START1	(0x80C)
#define DSS_GLB_ADP_OFFLINE_START0	(0x810)
#define DSS_GLB_ADP_OFFLINE_START1	(0x814)
#define DSS_GLB_GLB_CTL_GT	(0x818)
#define DSS_GLB_GLB_CFG_GT	(0x81C)
#define DSS_GLB_GLB_IRQ_GT	(0x820)
#define DSS_GLB_APB_GT	(0x824)
#define DSS_GLB_GLB_RSV0	(0x900)
#define DSS_GLB_GLB_RSV1	(0x904)
#define DSS_GLB_GLB_RSV2	(0x908)
#define DSS_GLB_GLB_RSV3	(0x90C)


/*******************************************************************************
** PDP_CTRL SDP_CTRL DPE2_CTRL ADP_CTRL
*/
#define DSS_DP_CTRL_OFFSET	(0x00002400)

/* PDP CTRL */
#define PDP_DPE0_DMA_MCU_IRQ	(0x000)
#define PDP_DPE0_DMA_MCU_IRQ_MSK	(0x004)
#define PDP_DPE0_DMA_MCU_IRQ_CLR	(0x008)
#define PDP_DPE0_MMU_MCU_IRQ	(0x010)
#define PDP_DPE0_MMU_MCU_IRQ_MSK	(0x014)
#define PDP_DPE0_MMU_MCU_IRQ_CLR	(0x018)
#define OFFLINE_DPE0_DMA_MCU_IRQ	(0x020)
#define OFFLINE_DPE0_DMA_MCU_IRQ_MSK	(0x024)
#define OFFLINE_DPE0_DMA_MCU_IRQ_CLR	(0x028)
#define OFFLINE_DPE0_MMU_MCU_IRQ	(0x030)
#define OFFLINE_DPE0_MMU_MCU_IRQ_MSK	(0x034)
#define OFFLINE_DPE0_MMU_MCU_IRQ_CLR	(0x038)
#define PDP_WBE0_MCU_IRQ	(0x040)
#define PDP_WBE0_MCU_IRQ_MSK	(0x044)
#define PDP_WBE0_MCU_IRQ_CLR	(0x048)
#define SDP_WBE0_MCU_IRQ	(0x050)
#define SDP_WBE0_MCU_IRQ_MSK	(0x054)
#define SDP_WBE0_MCU_IRQ_CLR	(0x058)
#define OFFLINE_WBE0_MCU_IRQ	(0x060)
#define OFFLINE_WBE0_MCU_IRQ_MSK	(0x064)
#define OFFLINE_WBE0_MCU_IRQ_CLR	(0x068)
#define PDP_DFS_MCU_IRQ	(0x070)
#define PDP_DFS_MCU_IRQ_MSK	(0x074)
#define PDP_DFS_MCU_IRQ_CLR	(0x078)
#define PDP_MUX_SECU_EN	(0x07C)
#define PDP_MUX_SECU_DIS	(0x080)
#define PDP_MUX_SECU_STATUS	0x084)
#define PDP_SECU_SEL_EN0	(0x088)
#define PDP_SECU_SEL_DIS0	(0x08C)
#define PDP_SECU_SEL_STATUS0	(0x090)
#define PDP_GATE_SECU	(0x094)
#define PDP_PAY_SECU_EN	(0x098)
#define PDP_DMA_SECU_EN	(0x0A0)
#define PDP_MMU_SECU_EN	(0x0A4)
#define PDP_DMA_SECU_DIS	(0x0A8)
#define PDP_MMU_SECU_DIS	(0x0AC)
#define PDP_DMA_SECU_STATUS	(0x0B0)
#define PDP_MMU_SECU_STATUS	(0x0B4)
#define PDP_DPE0_DMA_CPU_IRQ	(0x100)
#define PDP_DPE0_DMA_CPU_IRQ_MSK	(0x104)
#define PDP_DPE0_DMA_CPU_IRQ_CLR	(0x108)
#define PDP_DPE0_MMU_CPU_IRQ	(0x110)
#define PDP_DPE0_MMU_CPU_IRQ_MSK	(0x114)
#define PDP_DPE0_MMU_CPU_IRQ_CLR	(0x118)
#define OFFLINE_DPE0_DMA_CPU_IRQ	(0x120)
#define OFFLINE_DPE0_DMA_CPU_IRQ_MSK	(0x124)
#define OFFLINE_DPE0_DMA_CPU_IRQ_CLR	(0x128)
#define OFFLINE_DPE0_MMU_CPU_IRQ	(0x130)
#define OFFLINE_DPE0_MMU_CPU_IRQ_MSK	(0x134)
#define OFFLINE_DPE0_MMU_CPU_IRQ_CLR	(0x138)
#define PDP_WBE0_CPU_IRQ	(0x140)
#define PDP_WBE0_CPU_IRQ_MSK	(0x144)
#define PDP_WBE0_CPU_IRQ_CLR	(0x148)
#define SDP_WBE0_CPU_IRQ	(0x150)
#define SDP_WBE0_CPU_IRQ_MSK	(0x154)
#define SDP_WBE0_CPU_IRQ_CLR	(0x158)
#define OFFLINE_WBE0_CPU_IRQ	(0x160)
#define OFFLINE_WBE0_CPU_IRQ_MSK	(0x164)
#define OFFLINE_WBE0_CPU_IRQ_CLR	(0x168)
#define PDP_DFS_CPU_IRQ	(0x170)
#define PDP_DFS_CPU_IRQ_MSK	(0x174)
#define PDP_DFS_CPU_IRQ_CLR	(0x178)
#define DPE0_DMA_CPU_RAWSTAT	(0x180)
#define DPE0_MMU_CPU_RAWSTAT	(0x184)
#define WBE0_CPU_IRQ_RAWSTAT	(0x188)
#define PDP_DFS_CPU_IRQ_RAWSTAT	(0x18C)
#define PDP_CH_CLK_GT	(0x190)
#define PDP_CFG_GT	(0x194)
#define DPE0_CH1_MUX	(0x198)
#define DPE0_SWITCH	(0x19C)
#define OV0_MUX	(0x1A0)
#define WBE0_SCF1_SEL	(0x1A4)
#define WBE0_MUX	(0x1A8)
#define ROT0_PM_CTRL	(0x1AC)
#define ROT0_STATE	(0x1B0)
#define OV0_ERR_COLOR	(0x1B4)

/* DPE2 CTRL */
#define PDP_DPE2_DMA_MCU_IRQ	(0x0)
#define PDP_DPE2_DMA_MCU_IRQ_MSK	(0x004)
#define PDP_DPE2_DMA_MCU_IRQ_CLR	(0x008)
#define PDP_DPE2_MMU_MCU_IRQ	(0x010)
#define PDP_DPE2_MMU_MCU_IRQ_MSK	(0x014)
#define PDP_DPE2_MMU_MCU_IRQ_CLR	(0x018)
#define SDP_DPE2_DMA_MCU_IRQ	(0x020)
#define SDP_DPE2_DMA_MCU_IRQ_MSK	(0x024)
#define SDP_DPE2_DMA_MCU_IRQ_CLR	(0x028)
#define SDP_DPE2_MMU_MCU_IRQ	(0x030)
#define SDP_DPE2_MMU_MCU_IRQ_MSK	(0x034)
#define SDP_DPE2_MMU_MCU_IRQ_CLR	(0x038)
#define OFFLINE_DPE2_DMA_MCU_IRQ	(0x040)
#define OFFLINE_DPE2_DMA_MCU_IRQ_MSK	(0x044)
#define OFFLINE_DPE2_DMA_MCU_IRQ_CLR	(0x048)
#define OFFLINE_DPE2_MMU_MCU_IRQ	(0x050)
#define OFFLINE_DPE2_MMU_MCU_IRQ_MSK	(0x054)
#define OFFLINE_DPE2_MMU_MCU_IRQ_CLR	(0x058)
#define DPE2_MUX_SECU_EN	(0x60)
#define DPE2_MUX_SECU_DIS	(0x64)
#define DPE2_MUX_SECU_STATUS	(0x68)
#define DPE2_SECU_SEL_EN0	(0x6C)
#define DPE2_SECU_SEL_DIS0	(0x70)
#define DPE2_SECU_SEL_STATUS0	(0x74)
#define DPE2_DMA_SECU_EN	(0xA0)
#define DPE2_MMU_SECU_EN	(0xA4)
#define DPE2_DMA_SECU_DIS	(0xA8)
#define DPE2_MMU_SECU_DIS	(0xAC)
#define DPE2_DMA_SECU_STATUS	(0xB0)
#define DPE2_MMU_SECU_STATUS	(0xB4)
#define PDP_DPE2_DMA_CPU_IRQ	(0x100)
#define PDP_DPE2_DMA_CPU_IRQ_MSK	(0x104)
#define PDP_DPE2_DMA_CPU_IRQ_CLR	(0x108)
#define PDP_DPE2_MMU_CPU_IRQ	(0x110)
#define PDP_DPE2_MMU_CPU_IRQ_MSK	(0x114)
#define PDP_DPE2_MMU_CPU_IRQ_CLR	(0x118)
#define SDP_DPE2_DMA_CPU_IRQ	(0x120)
#define SDP_DPE2_DMA_CPU_IRQ_MSK	(0x124)
#define SDP_DPE2_DMA_CPU_IRQ_CLR	(0x128)
#define SDP_DPE2_MMU_CPU_IRQ	(0x130)
#define SDP_DPE2_MMU_CPU_IRQ_MSK	(0x134)
#define SDP_DPE2_MMU_CPU_IRQ_CLR	(0x138)
#define OFFLINE_DPE2_DMA_CPU_IRQ	(0x140)
#define OFFLINE_DPE2_DMA_CPU_IRQ_MSK	(0x144)
#define OFFLINE_DPE2_DMA_CPU_IRQ_CLR	(0x148)
#define OFFLINE_DPE2_MMU_CPU_IRQ	(0x150)
#define OFFLINE_DPE2_MMU_CPU_IRQ_MSK	(0x154)
#define OFFLINE_DPE2_MMU_CPU_IRQ_CLR	(0x158)
#define DPE2_DMA_CPU_RAWSTAT	(0x160)
#define DPE2_MMU_CPU_RAWSTAT	(0x164)
#define DPE2_CLK_GT	(0x170)
#define DPE2_CFG_GT	(0x190)
#define DPE2_CH0_MUX	(0x198)
#define DPE2_CH1_MUX	(0x19C)
#define DPE2_CH2_MUX	(0x1A0)
#define DPE2_CH3_MUX	(0x1A4)
#define DPE2_SWITCH	(0x1A8)

/* SDP CTRL */
#define SDP_DPE1_DMA_MCU_IRQ	(0x000)
#define SDP_DPE1_DMA_MCU_IRQ_MSK	(0x004)
#define SDP_DPE1_DMA_MCU_IRQ_CLR	(0x008)
#define SDP_DPE1_MMU_MCU_IRQ	(0x010)
#define SDP_DPE1_MMU_MCU_IRQ_MSK	(0x014)
#define SDP_DPE1_MMU_MCU_IRQ_CLR	(0x018)
#define OFFLINE_DPE1_DMA_MCU_IRQ	(0x020)
#define OFFLINE_DPE1_DMA_MCU_IRQ_MSK	(0x024)
#define OFFLINE_DPE1_DMA_MCU_IRQ_CLR	(0x028)
#define OFFLINE_DPE1_MMU_MCU_IRQ	(0x030)
#define OFFLINE_DPE1_MMU_MCU_IRQ_MSK	(0x034)
#define OFFLINE_DPE1_MMU_MCU_IRQ_CLR	(0x038)
#define SDP_DFS_MCU_IRQ	(0x040)
#define SDP_DFS_MCU_IRQ_MSK	(0x044)
#define SDP_DFS_MCU_IRQ_CLR	(0x048)
#define SDP_MUX_SECU_EN	(0x50)
#define SDP_MUX_SECU_DIS	(0x54)
#define SDP_MUX_SECU_STATUS	(0x58)
#define SDP_SECU_SEL_EN0	(0x60)
#define SDP_SECU_SEL_DIS0	(0x64)
#define SDP_SECU_SEL_STATUS	(0x68)
#define SDP_DSS_GATE_SECU	(0x70)
#define SDP_DMA_SECU_EN	(0xA0)
#define SDP_MMU_SECU_EN	(0xA4)
#define SDP_DMA_SECU_DIS	(0xA8)
#define SDP_MMU_SECU_DIS	(0xAC)
#define SDP_DMA_SECU_STATUS	(0xB0)
#define SDP_MMU_SECU_STATUS	(0xB4)
#define SDP_DPE1_DMA_CPU_IRQ	(0x100)
#define SDP_DPE1_DMA_CPU_IRQ_MSK	(0x104)
#define SDP_DPE1_DMA_CPU_IRQ_CLR	(0x108)
#define SDP_DPE1_MMU_CPU_IRQ	(0x110)
#define SDP_DPE1_MMU_CPU_IRQ_MSK	(0x114)
#define SDP_DPE1_MMU_CPU_IRQ_CLR	(0x118)
#define OFFLINE_DPE1_DMA_CPU_IRQ	(0x120)
#define OFFLINE_DPE1_DMA_CPU_IRQ_MSK	(0x124)
#define OFFLINE_DPE1_DMA_CPU_IRQ_CLR	(0x128)
#define OFFLINE_DPE1_MMU_CPU_IRQ	(0x130)
#define OFFLINE_DPE1_MMU_CPU_IRQ_MSK	(0x134)
#define OFFLINE_DPE1_MMU_CPU_IRQ_CLR	(0x138)
#define SDP_DFS_CPU_IRQ	(0x140)
#define SDP_DFS_CPU_IRQ_MSK	(0x144)
#define SDP_DFS_CPU_IRQ_CLR	(0x148)
#define DPE1_DMA_CPU_RAWSTAT	(0x150)
#define DPE1_MMU_CPU_RAWSTAT	(0x154)
#define SDP_DFS_CPU_IRQ_RAWSTAT	(0x158)
#define SDP_CH_CLK_GT	(0x160)
#define SDP_CFG_GT	(0x190)
#define DPE1_CH0_MUX	(0x198)
#define OV1_MUX	(0x19C)
#define SDP_MUX	(0x1A0)
#define OV1_ERR_COLOR	(0x1A4)

/* ADP CTRL */
#define OFFLINE_DPE3_DMA_MCU_IRQ	(0x000)
#define OFFLINE_DPE3_DMA_MCU_IRQ_MSK	(0x004)
#define OFFLINE_DPE3_DMA_MCU_IRQ_CLR	(0x008)
#define OFFLINE_DPE3_MMU_MCU_IRQ	(0x010)
#define OFFLINE_DPE3_MMU_MCU_IRQ_MSK	(0x014)
#define OFFLINE_DPE3_MMU_MCU_IRQ_CLR	(0x018)
#define OFFLINE_WBE1_MCU_IRQ	(0x020)
#define OFFLINE_WBE1_MCU_IRQ_MSK	(0x024)
#define OFFLINE_WBE1_MCU_IRQ_CLR	(0x028)
#define ADP_MUX_SECU_EN	(0x30)
#define ADP_MUX_SECU_DIS	(0x34)
#define ADP_MUX_SECU_STATUS	(0x38)
#define ADP_SECU_SEL_EN0	(0x3C)
#define ADP_SECU_SEL_DIS0	(0x40)
#define ADP_SECU_SEL_STATUS0	(0x44)
#define ADP_DSS_GATE_SECU	(0x48)
#define ADP_DMA_SECU_EN	(0xA0)
#define ADP_MMU_SECU_EN	(0xA4)
#define ADP_DMA_SECU_DIS	(0xA8)
#define ADP_MMU_SECU_DIS	(0xAC)
#define ADP_DMA_SECU_STATUS	(0xB0)
#define ADP_MMU_SECU_STATUS	(0xB4)
#define OFFLINE_DPE3_DMA_CPU_IRQ	(0x100)
#define OFFLINE_DPE3_DMA_CPU_IRQ_MSK	(0x104)
#define OFFLINE_DPE3_DMA_CPU_IRQ_CLR	(0x108)
#define OFFLINE_DPE3_MMU_CPU_IRQ	(0x110)
#define OFFLINE_DPE3_MMU_CPU_IRQ_MSK	(0x114)
#define OFFLINE_DPE3_MMU_CPU_IRQ_CLR	(0x118)
#define OFFLINE_WBE1_CPU_IRQ	(0x120)
#define OFFLINE_WBE1_CPU_IRQ_MSK	(0x124)
#define OFFLINE_WBE1_CPU_IRQ_CLR	(0x128)
#define DPE3_DMA_CPU_RAWSTAT	(0x130)
#define DPE3_MMU_CPU_RAWSTAT	(0x134)
#define WBE1_CPU_IRQ_RAWSTAT	(0x138)
#define ADP_CLK_GT	(0x13C)
#define ADP_CFG_GT	(0x140)
#define DPE3_CH0_MUX	(0x148)
#define OV2_MUX	(0x14C)
#define ADP_SCF3_SEL	(0x150)
#define ADP_ROT2_SEL	(0x154)
#define ROT2_PM_CTRL	(0x158)
#define ROT2_STATE	(0x15C)
#define ROT3_PM_CTRL	(0x160)
#define ROT3_STATE	(0x164)
#define ROT4_PM_CTRL	(0x168)
#define ROT4_STATE	(0x16C)
#define OV2_ERR_COLOR	(0x170)


/* TOP CTRL */
#define DSS_TOP_CTRL_OFFSET	(0x00020200)

#define PDP_ROT_TLB_MCU_IRQ	(0x0)
#define PDP_ROT_TLB_MCU_IRQ_MSK	(0x4)
#define PDP_ROT_TLB_MCU_IRQ_CLR	(0x8)
#define OFFLINE_ROT_TLB_MCU_IRQ	(0x010)
#define OFFLINE_ROT_TLB_MCU_IRQ_MSK	(0x014)
#define OFFLINE_ROT_TLB_MCU_IRQ_CLR	(0x018)
#define SDP_ROT_TLB_MCU_IRQ	(0x020)
#define SDP_ROT_TLB_MCU_IRQ_MSK	(0x024)
#define SDP_ROT_TLB_MCU_IRQ_CLR	(0x028)
#define CMDLST_SECU	(0x030)
#define ROT_TLB_SECU_EN	(0x034)
#define ROT_TLB_SECU_DIS	(0x038)
#define ROT_TLB_SECU_STATUS	(0x03C)
#define PDP_ROT_TLB_CPU_IRQ	(0x100)
#define PDP_ROT_TLB_CPU_IRQ_MSK	(0x104)
#define PDP_ROT_TLB_CPU_IRQ_CLR	(0x108)
#define OFFLINE_ROT_TLB_CPU_IRQ	(0x110)
#define OFFLINE_ROT_TLB_CPU_IRQ_MSK	(0x114)
#define OFFLINE_ROT_TLB_CPU_IRQ_CLR	(0x118)
#define SDP_ROT_TLB_CPU_IRQ	(0x120)
#define SDP_ROT_TLB_CPU_IRQ_MSK	(0x124)
#define SDP_ROT_TLB_CPU_IRQ_CLR	(0x128)
#define ROT_TLB_CPU_IRQ_RAWSTAT	(0x130)
#define TOP_CFG_CLK_GT	(0x134)


/*******************************************************************************
** RDMA_BRG
*/
#define DSS_DPE_RDMA_BRG_OFFSET	(0x00001000)

#define RBRG_RDMA0_CTL	(0x000)
#define RBRG_RDMA1_CTL	(0x004)
#define RBRG_RDMA2_CTL	(0x008)
#define RBRG_RDMA3_CTL	(0x00C)
#define RBRG_RDMA4_CTL	(0x010)
#define RBRG_CLR_CTL0	(0x014)
#define RBRG_CLR_CTL1	(0x018)
#define RBRG_OUTSTANDING_DEP0	(0x01C)
#define RBRG_OUTSTANDING_DEP1	(0x020)
#define RBRG_OUTSTANDING_DEP2	(0x024)
#define RBRG_OUTSTANDING_DEP3	(0x028)
#define RBRG_OUTSTANDING_DEP4	(0x02C)
#define RBRG_AXI0_CMDS_CTL	(0x030)
#define RBRG_AXI1_CMDS_CTL	(0x034)
#define RBRG_AXI0_RID_MAP	(0x038)
#define RBRG_AXI1_RID_MAP	(0x03C)
#define RBRG_AXI0_DBUF_FULL0	(0x040)
#define RBRG_AXI0_DBUF_FULL1	(0x044)
#define RBRG_AXI0_DBUF_FULL2	(0x048)
#define RBRG_AXI0_DBUF_FULL3	(0x04C)
#define RBRG_AXI1_DBUF_FULL0	(0x050)
#define RBRG_AXI1_DBUF_FULL1	(0x054)
#define RBRG_AXI1_DBUF_FULL2	(0x058)
#define RBRG_AXI1_DBUF_FULL3	(0x05C)
#define RBRG_RID_FIFO_LEV0	(0x060)
#define RBRG_RID_FIFO_LEV1	(0x064)
#define RBRG_AXI0_CMDS_STS	(0x068)
#define RBRG_AXI1_CMDS_STS	(0x06C)
#define RBRG_IDLE		(0x070)
#define RBRG_SW_CLR0	(0x074)
#define RBRG_SW_CLR1	(0x078)
#define RBRG_CORE_GT	(0x07C)
#define RBRG_AXI0_MEM_CTL0	(0x080)
#define RBRG_AXI0_MEM_CTL1	(0x084)


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

typedef struct dss_rdma_bridge {
	uint32_t rbrg_rdma_ctl[RDMA_NUM];
	uint32_t rbrg_outstanding_dep[RDMA_NUM];

} dss_rdma_bridge_t;


/*******************************************************************************
** RDMA
*/
#define DSS_DPE_RDMA0_OFFSET	(0x00001100)
#define DSS_DPE_RDMA1_OFFSET	(0x00001200)
#define DSS_DPE_RDMA2_OFFSET	(0x00001300)
#define DSS_DPE_RDMA3_OFFSET	(0x00001400)
#define DSS_DPE_RDMA4_OFFSET	(0x00001600)

#define RDMA_DATA_ADDR	(0x000)
#define RDMA_OFT_X0	(0x004)
#define RDMA_OFT_Y0	(0x008)
#define RDMA_ROT_QOS_LEV	(0x00C)
#define RDMA_STRIDE	(0x010)
#define RDMA_OFT_X1	(0x014)
#define RDMA_OFT_Y1	(0x018)
#define RDMA_MASK0	(0x01C)
#define RDMA_MASK1	(0x020)
#define RDMA_STRETCH_SIZE_VRT	(0x024)
#define RDMA_STRETCH_LINE_NUM	(0x028)
#define RDMA_CTRL	(0x02C)
#define RDMA_DATA_NUM	(0x030)
#define RDMA_TILE_SCRAM	(0x034)
#define RDMA_CORE_GT	(0x038)
#define RDMA_PULSE	(0x03C)
#define RDMA_TEST0	(0x040)
#define RDMA_TEST1	(0x044)
#define RDMA_TEST2	(0x048)
#define RDMA_TEST3	(0x04C)
#define RDMA_TEST4	(0x050)
#define RDMA_TEST5	(0x054)
#define RDMA_STATUS	(0x058)
#define RDMA_BAK	(0x05C)


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
	uint32_t rdma_data_addr;
	uint32_t rdma_oft_x0;
	uint32_t rdma_oft_y0;
	uint32_t rdma_rot_qos_lev;
	uint32_t rdma_stride;
	uint32_t rdma_oft_x1;
	uint32_t rdma_oft_y1;
	uint32_t rdma_mask0;
	uint32_t rdma_mask1;
	uint32_t rdma_stretch_size_vrt;
	uint32_t rdma_stretch_line_num;
	uint32_t rdma_ctrl;
	uint32_t rdma_data_num;
	uint32_t rdma_tile_scram;
	uint32_t rdma_core_gt;

} dss_rdma_t;


/*******************************************************************************
** FBDC
*/
#define DSS_DPE_FBDC_OFFSET	(0x00001700)

#define FBDC_DATA_ADDR	(0x000)
#define FBDC_DATA_STRIDE	(0x004)
#define FBDC_HEAD_ADDR	(0x008)
#define FBDC_HEAD_STRIDE	(0x00C)
#define FBDC_SIZE	(0x010)
#define FBDC_CROP_S	(0x014)
#define FBDC_CTRL	(0x018)
#define FBDC_CG_CTRL	(0x01C)
#define FBDC_PM_CTRL	(0x020)
#define FBDC_SRC_SIZE	(0x024)
#define FBDC_REV	(0x028)


typedef struct dss_fbdc {
	uint32_t fbdc_data_addr;
	uint32_t fbdc_data_stride;
	uint32_t fbdc_head_addr;
	uint32_t fbdc_head_stride;
	uint32_t fbdc_size;
	uint32_t fbdc_crop_s;
	uint32_t fbdc_ctrl;
	uint32_t fbdc_pm_ctrl;
	uint32_t fbdc_src_size;

} dss_fbdc_t;


/*******************************************************************************
** DFC
*/
#define DSS_DPE_RDFC0_OFFSET	(0x00001900)
#define DSS_DPE_RDFC1_OFFSET	(0x00001A00)
#define DSS_DPE_RDFC2_OFFSET	(0x00001B00)
#define DSS_DPE_RDFC3_OFFSET	(0x00001C00)

#define DFC_DISP_SIZE	(0x000)
#define DFC_PIX_IN_NUM	(0x004)
#define DFC_GLB_ALPHA	(0x008)
#define DFC_DISP_FMT	(0x00C)
#define DFC_CLIP_CTL_HRZ	(0x010)
#define DFC_CLIP_CTL_VRZ	(0x014)
#define DFC_CTL_CLIP_EN	(0x018)
#define DFC_ICG_MODULE	(0x01C)
#define DFC_DITHER_ENABLE	(0x020)
#define DFC_PADDING_CTL	(0x024)


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
** SCF & SCP
*/
#define DSS_SCF_REG_OFFSET	(0x3000)
#define DSS_SCF0_OFFSET	(0x00004000 + DSS_SCF_REG_OFFSET)
#define DSS_SCF1_OFFSET	(0x00008000 + DSS_SCF_REG_OFFSET)
#define DSS_SCF2_OFFSET	(0x00004000 + DSS_SCF_REG_OFFSET)
#define DSS_SCF3_OFFSET	(0x00008000 + DSS_SCF_REG_OFFSET)
#define DSS_SCF4_OFFSET	(0x00004000 + DSS_SCF_REG_OFFSET)

#define DSS_SCF_H0_Y_COEF_OFFSET	(-0x3000)
#define DSS_SCF_Y_COEF_OFFSET	(-0x1000)
#define DSS_SCF_UV_COEF_OFFSET	(-0x800)

#define SCF_EN_HSCL_STR	(0x000)
#define SCF_EN_VSCL_STR	(0x004)
#define SCF_H_V_ORDER	(0x008)
#define SCF_CORE_GT	(0x00C)
#define SCF_INPUT_WIDTH_HEIGHT	(0x010)
#define SCF_OUTPUT_WIDTH_HEIGTH	(0x014)
#define SCF_COEF_PM	(0x018)
#define SCF_EN_HSCL	(0x01C)
#define SCF_EN_VSCL	(0x020)
#define SCF_ACC_HSCL	(0x024)
#define SCF_ACC_HSCL1	(0x028)
#define SCF_ACC_HSCL2	(0x02C)
#define SCF_ACC_HSCLA	(0x030)
#define SCF_INC_HSCL	(0x034)
#define SCF_ACC_VSCL	(0x038)
#define SCF_ACC_VSCL1	(0x03C)
#define SCF_ACC_VSCL2	(0x040)
#define SCF_ACC_VSCLA	(0x044)
#define SCF_INC_VSCL	(0x048)
#define SCF_EN_NONLINEAR	(0x04C)
#define SCF_NL_INCINC0	(0x050)
#define SCF_NL_INCINC1	(0x054)
#define SCF_NL_INCINC2	(0x058)
#define SCF_NL_INCINC3	(0x05C)
#define SCF_NL_INCINC4	(0x060)
#define SCF_NL_INCINC5	(0x064)
#define SCF_NL_RGN0	(0x068)
#define SCF_NL_RGN1	(0x06C)
#define SCF_NL_RGN2	(0x070)
#define SCF_NL_RGN3	(0x074)
#define SCF_NL_RGN4	(0x078)
#define SCF_EN_MMP	(0x07C)
#define SCF_DB_H0	(0x080)
#define SCF_DB_H1	(0x084)
#define SCF_DB_V0	(0x088)
#define SCF_DB_V1	(0x08C)
#define SCF_LB_PM	(0x090)


typedef struct dss_scf {
	uint32_t en_hscl_str;
	uint32_t en_vscl_str;
	uint32_t h_v_order;
	uint32_t input_width_height;
	uint32_t output_width_heigth;
	uint32_t en_hscl;
	uint32_t en_vscl;
	uint32_t acc_hscl;
	uint32_t inc_hscl;
	uint32_t inc_vscl;
} dss_scf_t;

/* SCPost */
#define DSS_DPE_SCP_OFFSET	(0x00001D00)

#define SCP_REG0	(0x0)
#define SCP_REG1	(0x4)
#define SCP_REG2	(0x8)
#define SCP_REG3	(0xC)
#define SCP_REG4	(0x10)
#define SCP_REG5	(0x14)
#define SCP_REG6	(0x18)
#define SCP_REG7	(0x1C)
#define SCP_REG8	(0x20)
#define SCP_REG9	(0x24)
#define SCP_REG10	(0x28)
#define SCP_REG11	(0x2C)
#define SCP_REG12	(0x30)
#define SCP_REG13	(0x34)

/* SCP_REG1 */
#define SCP_EN	BIT(10)

typedef struct dss_scp {
	uint32_t scp_reg0;
	uint32_t scp_reg1;
	uint32_t scp_reg2;
	uint32_t scp_reg3;
	uint32_t scp_reg4;
	uint32_t scp_reg5;
	uint32_t scp_reg6;
	uint32_t scp_reg7;
	uint32_t scp_reg8;
	uint32_t scp_reg9;
	uint32_t scp_reg10;
	uint32_t scp_reg11;
	uint32_t scp_reg12;
	uint32_t scp_reg13;
} dss_scp_t;


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


/*******************************************************************************
** CSC
*/
#define DSS_DPE_CSC0_OFFSET	(0x00001F00)
#define DSS_DPE_CSC1_OFFSET	(0x00002000)
#define DSS_DPE_CSC2_OFFSET	(0x00002100)
#define DSS_DPE_CSC3_OFFSET	(0x00002200)

#define CSC_VHDCSCIDC	(0x000)
#define CSC_VHDCSCODC	(0x004)
#define CSC_VHDCSCP0	(0x008)
#define CSC_VHDCSCP1	(0x00C)
#define CSC_VHDCSCP2	(0x010)
#define CSC_VHDCSCP3	(0x014)
#define CSC_VHDCSCP4	(0x018)
#define CSC_ICG_MODULE	(0x01C)


typedef struct dss_csc {
	uint32_t vhdcscidc;
	uint32_t vhdcscodc;
	uint32_t vhdcscp0;
	uint32_t vhdcscp1;
	uint32_t vhdcscp2;
	uint32_t vhdcscp3;
	uint32_t vhdcscp4;
	uint32_t icg_module;

} dss_csc_t;


/*******************************************************************************
** WBE WDMA_BRG
*/

#define DSS_WBE_WDMA_BRG0_OFFSET	(0x00020E00)
#define DSS_WBE_WDMA_BRG1_OFFSET	(0x00020F00)

#define WBRG_COMM_CTL	(0x000)
#define WBRG_STS	(0x00C)
#define WBRG_CTL0	(0x018)
#define WBRG_CTL1	(0x01C)
#define WBRG_CLK_CTRL	(0x034)
#define WBRG_WO_CH_CTL	(0x038)
#define WBRG_WS_CH_CTL	(0x03C)
#define WBRG_WS_MEM_CTRL    (0x040)
#define WBRG_WO_MEM_CTRL	(0x044)
#define WBRG_MMU_MEM_CTRL	(0x048)


typedef struct dss_wdma_bridge {
	uint32_t wbrg_comm_ctl;
	uint32_t wbrg_ctl0;
	uint32_t wbrg_ctl1;
	uint32_t wbrg_wo_ch_ctl;
	uint32_t wbrg_ws_ch_ctl;

} dss_wdma_bridge_t;

enum axi_ch {
	AXI_CH0 = 0,
	AXI_CH1,
};


/*******************************************************************************
** WBE WDMA_BRG
*/

#define DSS_WBE_WDMA0_OFFSET	(0x00003100)
#define DSS_WBE_WDMA1_OFFSET	(0x00003200)

#define WDMA_DATA_ADDR	(0x00)
#define WDMA_OFT_X0	(0x04)
#define WDMA_OFT_Y0	(0x08)
#define WDMA_ROT_QOS_LEV	(0x0C)
#define WDMA_STRIDE	(0x10)
#define WDMA_OFT_X1	(0x14)
#define WDMA_OFT_Y1	(0x18)
#define WDMA_CTRL	(0x2C)
#define WDMA_DATA_NUM	(0x30)
#define WDMA_TILE_SCRAM	(0x34)
#define WDMA_CORE_GT	(0x38)
#define WDMA_PULSE	(0x3C)
#define WDMA_TEST0	(0x40)
#define WDMA_TEST1	(0x44)
#define WDMA_TEST2	(0x48)
#define WDMA_TEST3	(0x4C)
#define WDMA_TEST4	(0x50)
#define WDMA_TEST5	(0x54)
#define WDMA_STATUS	(0x58)
#define WDMA_BAK	(0x5C)
#define WDMA_SW_MASK_EN	(0x60)
#define WDMA_CTRL_2	(0x64)
#define WDMA_HDR_START_ADDR	(0x70)
#define WDMA_HDR_STRIDE	(0x74)
#define WDMA_LOW_POWER_CTRL	(0x78)
#define WDMA_START_MASK	(0x7C)
#define WDMA_END_MASK	(0x80)
#define WDMA_WBRG_CREDIT_CTL	(0x84)
#define WDMA_KEY_SIG	(0x88)
#define WDMA_RSV0	(0x8C)
#define WDMA_RSV1	(0x90)
#define WDMA_RSV2	(0x94)

typedef struct dss_wdma {
	uint32_t wdma_data_addr;
	uint32_t wdma_oft_x0;
	uint32_t wdma_oft_y0;
	uint32_t wdma_rot_qos_lev;
	uint32_t wdma_stride;
	uint32_t wdma_oft_x1;
	uint32_t wdma_oft_y1;
	uint32_t wdma_ctrl;
	uint32_t wdma_data_num;
	uint32_t wdma_tile_scram;
	uint32_t wdma_sw_mask_en;
	uint32_t wdma_hdr_start_addr;
	uint32_t wdma_hdr_stride;
	uint32_t wdma_start_mask;
	uint32_t wdma_end_mask;
	uint32_t wdma_wbrg_credit_ctl;

} dss_wdma_t;


/*******************************************************************************
** WBE FBC
*/
#define DSS_WBE_FBC0_OFFSET	(0x00003300)

#define FBC_SIZE	(0x0000)
#define FBC_CTRL	(0x0004)
#define FBC_EN	(0x0008)
#define FBC_GT	(0x000C)
#define FBC_ZERO_NUM	(0x0010)
#define FBC_PM_CTRL	(0x0014)
#define FBC_MEM_CTRL	(0x0018)
#define FBC_LEN	(0x001C)
#define FBC_CODE01	(0X0050)
#define FBC_CODE02	(0X0054)
#define FBC_CODE03	(0X0058)
#define FBC_CODE04	(0X005C)
#define FBC_CODE05	(0X0060)
#define FBC_CODE06	(0X0064)
#define FBC_CODE07	(0X0068)
#define FBC_CODE08	(0X006C)
#define FBC_CODE09	(0X0070)
#define FBC_CODE10	(0X0074)
#define FBC_CODE11	(0X0078)
#define FBC_CODE12	(0X007C)
#define FBC_CODE13	(0X0080)
#define FBC_CODE14	(0X0084)
#define FBC_CODE15	(0X0088)
#define FBC_CODE16	(0X008C)
#define FBC_CODE17	(0X0090)
#define FBC_CODE18	(0X0094)
#define FBC_CODE19	(0X0098)
#define FBC_CODE20	(0X009C)
#define FBC_CODE21	(0X00A0)
#define FBC_CODE22	(0X00A4)
#define FBC_CODE23	(0X00A8)
#define FBC_CODE24	(0X00AC)
#define FBC_CODE25	(0X00B0)
#define FBC_CODE26	(0X00B4)
#define FBC_CODE27	(0X00B8)
#define FBC_CODE28	(0X00BC)
#define FBC_CODE29	(0X00C0)
#define FBC_CODE30	(0X00C4)
#define FBC_CODE31	(0X00C8)
#define FBC_CODE32	(0X00CC)
#define FBC_CODE_LUT1	(0x00D0)
#define FBC_CODE_LUT2	(0x00D4)
#define FBC_CODE_LUT3	(0x00D8)
#define FBC_CODE_LSB	(0x00DC)
#define FBC_CODE_LEN	(0x00E0)

enum fbc_arith_sel {
	FBC_ARITH_SEL0 = 0,
	FBC_ARITH_SEL1,
	FBC_ARITH_SEL2,
	FBC_ARITH_SEL3,
};

#define FBC_BLOCK_64_PIXELS	(64)
#define FBC_BLOCK_128_PIXELS	(128)

typedef struct dss_fbc {
	uint32_t fbc_size;
	uint32_t fbc_ctrl;
	uint32_t fbc_en;
	uint32_t fbc_len;
	uint32_t fbc_pm_ctrl;

} dss_fbc_t;


/*******************************************************************************
** WBE WDFC
*/

#define DSS_WBE_WDFC0_OFFSET	(0x00003500)
#define DSS_WBE_WDFC1_OFFSET	(0x00003600)


/*******************************************************************************
** WBE CSC
*/

#define DSS_WBE_CSC0_OFFSET	(0x00003700)
#define DSS_WBE_CSC1_OFFSET	(0x00003800)


/*******************************************************************************
** OVL
*/
#define DSS_OVL0_OFFSET	(DSS_DPE0_OFFSET + 0x00003D00)
#define DSS_OVL1_OFFSET	(DSS_DPE1_OFFSET + 0x00003D00)
#define DSS_OVL2_OFFSET	(DSS_DPE3_OFFSET + 0x00003D00)

#define OVL_OV_SIZE	(0x000)
#define OVL_OV_BG_COLOR	(0x004)
#define OVL_DST_STARTPOS	(0x008)
#define OVL_DST_ENDPOS	(0x00C)
#define OVL_OV_GCFG	(0x010)
#define OVL_LAYER0_POS	(0x014)
#define OVL_LAYER0_SIZE	(0x018)
#define OVL_LAYER0_SRCLOKEY	(0x01C)
#define OVL_LAYER0_SRCHIKEY	(0x020)
#define OVL_LAYER0_DSTLOKEY	(0x024)
#define OVL_LAYER0_DSTHIKEY	(0x028)
#define OVL_LAYER0_PATTERN	(0x02C)
#define OVL_LAYER0_ALPHA	(0x030)
#define OVL_LAYER0_CFG	(0x034)
#define OVL_LAYER0_INFO_ALPHA	(0x038)
#define OVL_LAYER0_INFO_SRCCOLOR	(0x03C)
#define OVL_LAYER1_POS	(0x040)
#define OVL_LAYER1_SIZE	(0x044)
#define OVL_LAYER1_SRCLOKEY	(0x048)
#define OVL_LAYER1_SRCHIKEY	(0x04C)
#define OVL_LAYER1_DSTLOKEY	(0x050)
#define OVL_LAYER1_DSTHIKEY	(0x054)
#define OVL_LAYER1_PATTERN	(0x058)
#define OVL_LAYER1_ALPHA	(0x05C)
#define OVL_LAYER1_CFG	(0x060)
#define OVL_LAYER1_INFO_ALPHA	(0x064)
#define OVL_LAYER1_INFO_SRCCOLOR	(0x068)
#define OVL_LAYER2_POS	(0x06C)
#define OVL_LAYER2_SIZE	(0x070)
#define OVL_LAYER2_SRCLOKEY	(0x074)
#define OVL_LAYER2_SRCHIKEY	(0x078)
#define OVL_LAYER2_DSTLOKEY	(0x07C)
#define OVL_LAYER2_DSTHIKEY	(0x080)
#define OVL_LAYER2_PATTERN	(0x084)
#define OVL_LAYER2_ALPHA	(0x088)
#define OVL_LAYER2_CFG	(0x08C)
#define OVL_LAYER2_INFO_ALPHA	(0x090)
#define OVL_LAYER2_INFO_SRCCOLOR	(0x094)
#define OVL_LAYER3_POS	(0x098)
#define OVL_LAYER3_SIZE	(0x09C)
#define OVL_LAYER3_SRCLOKEY	(0x0A0)
#define OVL_LAYER3_SRCHIKEY	(0x0A4)
#define OVL_LAYER3_DSTLOKEY	(0x0A8)
#define OVL_LAYER3_DSTHIKEY	(0x0AC)
#define OVL_LAYER3_PATTERN	(0x0B0)
#define OVL_LAYER3_ALPHA	(0x0B4)
#define OVL_LAYER3_CFG	(0x0B8)
#define OVL_LAYER3_INFO_ALPHA	(0x0BC)
#define OVL_LAYER3_INFO_SRCCOLOR	(0x0C0)
#define OVL_LAYER4_POS	(0x0C4)
#define OVL_LAYER4_SIZE	(0x0C8)
#define OVL_LAYER4_SRCLOKEY	(0x0CC)
#define OVL_LAYER4_SRCHIKEY	(0x0D0)
#define OVL_LAYER4_DSTLOKEY	(0x0D4)
#define OVL_LAYER4_DSTHIKEY	(0x0D8)
#define OVL_LAYER4_PATTERN	(0x0DC)
#define OVL_LAYER4_ALPHA	(0x0E0)
#define OVL_LAYER4_CFG	(0x0E4)
#define OVL_LAYER4_INFO_ALPHA	(0x0E8)
#define OVL_LAYER4_INFO_SRCCOLOR	(0x0EC)
#define OVL_LAYER5_POS	(0x0F0)
#define OVL_LAYER5_SIZE	(0x0F4)
#define OVL_LAYER5_SRCLOKEY	(0x0F8)
#define OVL_LAYER5_SRCHIKEY	(0x0FC)
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
#define OVL_LAYER0_STALL_INFO	(0x178)
#define OVL_LAYER1_STALL_INFO	(0x17C)
#define OVL_LAYER2_STALL_INFO	(0x180)
#define OVL_LAYER3_STALL_INFO	(0x184)
#define OVL_LAYER4_STALL_INFO	(0x188)
#define OVL_LAYER5_STALL_INFO	(0x18C)
#define OVL_LAYER6_STALL_INFO	(0x190)
#define OVL_LAYER7_STALL_INFO	(0x194)
#define OVL_GATE_CTRL	(0x198)
#define OVL_LAYER0_PSPOS	(0x1A0)
#define OVL_LAYER0_PEPOS	(0x1A4)
#define OVL_LAYER1_PSPOS	(0x1A8)
#define OVL_LAYER1_PEPOS	(0x1AC)
#define OVL_LAYER2_PSPOS	(0x1B0)
#define OVL_LAYER2_PEPOS	(0x1B4)
#define OVL_LAYER3_PSPOS	(0x1B8)
#define OVL_LAYER3_PEPOS	(0x1BC)
#define OVL_LAYER4_PSPOS	(0x1C0)
#define OVL_LAYER4_PEPOS	(0x1C4)
#define OVL_LAYER5_PSPOS	(0x1C8)
#define OVL_LAYER5_PEPOS	(0x1CC)
#define OVL_LAYER6_PSPOS	(0x1D0)
#define OVL_LAYER6_PEPOS	(0x1D4)
#define OVL_LAYER7_PSPOS	(0x1D8)
#define OVL_LAYER7_PEPOS	(0x1DC)


/* LAYER0_CFG */
#define BIT_OVL_LAYER_SRC_CFG	BIT(8)
#define BIT_OVL_LAYER_ENABLE	BIT(0)

/* LAYER0_INFO_ALPHA */
#define BIT_OVL_LAYER_SRCALPHA_FLAG	BIT(3)
#define BIT_OVL_LAYER_DSTALPHA_FLAG	BIT(2)

/* LAYER0_INFO_SRCCOLOR */
#define BIT_OVL_LAYER_SRCCOLOR_FLAG	BIT(0)


#define OVL_LAYER_NUM	(8)

typedef struct dss_ovl_layer {
	uint32_t layer_pos;
	uint32_t layer_size	;
	uint32_t layer_pattern;
	uint32_t layer_alpha;
	uint32_t layer_cfg;

} dss_ovl_layer_t;

typedef struct dss_ovl_layer_position {
	uint32_t ovl_layer_pspos;
	uint32_t ovl_layer_pepos;

} dss_ovl_layer_position_t;

typedef struct dss_ovl {
	uint32_t ovl_ov_size;
	uint32_t ovl_ov_bg_color;
	uint32_t ovl_dst_startpos;
	uint32_t ovl_dst_endpos;
	uint32_t ovl_ov_gcfg;
	dss_ovl_layer_t ovl_layer[OVL_LAYER_NUM];
	dss_ovl_layer_position_t ovl_layer_position[OVL_LAYER_NUM];

} dss_ovl_t;


/*******************************************************************************
** DSS MMU_DMA & MMU_RPTB
*/
#define DSS_DPE_MMU_OFFSET	(0x00002800)
#define DSS_WBE_MMU_OFFSET	(0x00003A00)

#define DSS_MMU_RDMA0_OFFSET	(DSS_DPE_MMU_OFFSET + 0x0)
#define DSS_MMU_RDMA1_OFFSET	(DSS_DPE_MMU_OFFSET + 0x100)
#define DSS_MMU_RDMA2_OFFSET	(DSS_DPE_MMU_OFFSET + 0x200)
#define DSS_MMU_RDMA3_OFFSET	(DSS_DPE_MMU_OFFSET + 0x300)
#define DSS_MMU_RDMA4_OFFSET	(DSS_DPE_MMU_OFFSET + 0x400)

#define DSS_MMU_WDMA0_OFFSET	(DSS_WBE_MMU_OFFSET + 0x0)
#define DSS_MMU_WDMA1_OFFSET	(DSS_WBE_MMU_OFFSET + 0x100)

/* MMU_DMA */
#define MMU_CH_CTRL	(0x0)
#define MMU_CH_PTBA	(0x8)
#define MMU_CH_PTVA	(0xC)
#define MMU_CH_AMSK	(0x1C)
#define MMU_CH_SECU	(0x20)
#define MMU_AXI_ARB	(0x52C)


/* MACROS */
/* MMU aligned limited: PTBA(128Bytes aligned)  PTVA(128KBytes aligned) */
#define MMU_CH_PTBA_ALIGN	(128)
#define MMU_CH_PTVA_ALIGN	(128 * 1024)
#define MMU_CH_AMSK_THRESHOLD	(4096)

#define MMU_PAGE_SIZE	(4096)
#define MMU_RPTB_SIZE_ALIGN	(8)
#define MMU_RPTB_WIDTH	(128 / BITS_PER_BYTE)
#define MMU_RPTB_HEIGH	(4096)
#define MMU_RPTB_SIZE_MAX	(MMU_RPTB_HEIGH * MMU_RPTB_WIDTH)

typedef struct dss_mmu_dma {
	uint32_t mmu_ch_ctrl;
	uint32_t mmu_ch_ptba;
	uint32_t mmu_ch_ptva;
	uint32_t mmu_ch_amsk;

} dss_mmu_dma_t;


#define DSS_MMU_RTLB_OFFSET	(0x00000000)

/* MMU_RPTB */
#define MMU_RPTB_CTL	(0x000)
#define MMU_RPTB_LOAD0	(0x008)
#define MMU_RPTB_LOAD1	(0x00C)
#define MMU_RPTB_LOAD2	(0x010)
#define MMU_RPTB_LOAD3	(0x014)
#define MMU_RPTB_LOAD4	(0x018)
#define MMU_RPTB_BA0	(0x01C)
#define MMU_RPTB_BA1	(0x020)
#define MMU_RPTB_BA2	(0x024)
#define MMU_RPTB_BA3	(0x028)
#define MMU_RPTB_BA4	(0x02C)
#define MMU_RPTB_SECU	(0x030)
#define MMU_RTLB_PM	(0x058)
#define MMU_RTLB_SEL	(0x05C)

typedef struct dss_mmu_rptb {
	uint32_t mmu_rptb_ctl;
	uint32_t mmu_rtlb_sel;
	uint32_t mmu_rptb_load[DSS_ROT_MAX];
	uint32_t mmu_rptb_ba[DSS_ROT_MAX];
	uint32_t mmu_rptb_idx_used[DSS_ROT_MAX];

} dss_mmu_rptb_t;


/*******************************************************************************
** WBE DFS
*/

/* DFS */
#define DSS_WBE_DFS_OFFSET	(0x00003900)

#define DFS_FRM_SIZE	(DSS_WBE_DFS_OFFSET + 0x000)
#define DFS_FRM_HSIZE	(DSS_WBE_DFS_OFFSET + 0x004)
#define DFS_SRAM_VALID_NUM	(DSS_WBE_DFS_OFFSET + 0x008)
#define DFS_WBE_EN	(DSS_WBE_DFS_OFFSET + 0x00C)
#define DFS_THD_FILL_LEV0	(DSS_WBE_DFS_OFFSET + 0x010)
#define DFS_FILL_LEV1	(DSS_WBE_DFS_OFFSET + 0x014)
#define DFS_THD_RQOS	(DSS_WBE_DFS_OFFSET + 0x018)
#define DFS_THD_WQOS	(DSS_WBE_DFS_OFFSET + 0x01C)
#define DFS_THD_CG	(DSS_WBE_DFS_OFFSET + 0x020)
#define DFS_THD_OTHER	(DSS_WBE_DFS_OFFSET + 0x024)
#define DFS_FILL_LEV0_CNT	(DSS_WBE_DFS_OFFSET + 0x028)
#define DFS_FILL_LEV1_CNT	(DSS_WBE_DFS_OFFSET + 0x02C)
#define DFS_FILL_LEV2_CNT	(DSS_WBE_DFS_OFFSET + 0x030)
#define DFS_FILL_LEV3_CNT	(DSS_WBE_DFS_OFFSET + 0x034)
#define DFS_FILL_LEV4_CNT	(DSS_WBE_DFS_OFFSET + 0x038)
#define DFS_RD_PTR	(DSS_WBE_DFS_OFFSET + 0x03C)
#define DFS_WR_PTR	(DSS_WBE_DFS_OFFSET + 0x040)
#define DFS_STATUS	(DSS_WBE_DFS_OFFSET + 0x044)
#define DFS_LP_CTRL	(DSS_WBE_DFS_OFFSET + 0x048)


/*******************************************************************************
** DSS Command List
*/
#define DSS_CMD_LIST_OFFSET	(0x00020400)

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
#define CMDLIST_CH5_INTS	(0x00A4)
#define CMDLIST_CH5_INTC	(0x00A8)
#define CMDLIST_CTRL	(0x00AC)
#define CMDLIST_SECU	(0x00B0)
#define CMDLIST_INTS	(0x00B4)
#define CMDLIST_SWRST	(0x00B8)

/* CMDLIST_CH_STATUS */
#define BIT_CHX_IDLE	BIT(10)

/* CMDLIST_INTS	(0x00B4) */
#define BIT_CH0_INTS	BIT(0)
#define BIT_CH1_INTS	BIT(1)
#define BIT_CH2_INTS	BIT(2)
#define BIT_CH3_INTS	BIT(3)
#define BIT_CH4_INTS	BIT(4)
#define BIT_CH5_INTS	BIT(5)

/*CMDLIST CHN INTS*/
#define BIT_ONE_DONE_INTS	BIT(0)
#define BIT_ALL_DONE_INTS	BIT(1)
#define BIT_AXI_ERR_INTS	BIT(2)
#define BIT_PENDING_INTS	BIT(3)
#define BIT_START_INTS		BIT(4)
#define BIT_BAD_CMD_INTS	BIT(5)

#define MAX_CMD_LIST_HEADER_NUM	(128)
#define MAX_CMD_LIST_ITEM_SIZE	(256)


/*******************************************************************************
** DPP LDI
*/

/* LDI_PDP */
#define DSS_DPP_LDI_PDP_OFFSET	(DSS_DPP_OFFSET + 0x0000A400)

#define PDP_LDI_DPI0_HRZ_CTRL0	(DSS_DPP_LDI_PDP_OFFSET + 0x000)
#define PDP_LDI_DPI0_HRZ_CTRL1	(DSS_DPP_LDI_PDP_OFFSET + 0x004)
#define PDP_LDI_DPI1_HRZ_CTRL0	(DSS_DPP_LDI_PDP_OFFSET + 0x008)
#define PDP_LDI_DPI1_HRZ_CTRL1	(DSS_DPP_LDI_PDP_OFFSET + 0x00C)
#define PDP_LDI_VRT_CTRL0	(DSS_DPP_LDI_PDP_OFFSET + 0x010)
#define PDP_LDI_VRT_CTRL1	(DSS_DPP_LDI_PDP_OFFSET + 0x014)
#define PDP_LDI_PLR_CTRL	(DSS_DPP_LDI_PDP_OFFSET + 0x018)
#define PDP_LDI_OVERLAP_SIZE	(DSS_DPP_LDI_PDP_OFFSET + 0x01C)
#define PDP_LDI_3D_CTRL	(DSS_DPP_LDI_PDP_OFFSET + 0x020)
#define PDP_LDI_CTRL	(DSS_DPP_LDI_PDP_OFFSET + 0x024)
#define PDP_LDI_WORK_MODE	(DSS_DPP_LDI_PDP_OFFSET + 0x028)
#define PDP_LDI_DE_SPACE_LOW	(DSS_DPP_LDI_PDP_OFFSET + 0x02C)

#define PDP_DSI_CMD_MOD_CTRL	(DSS_DPP_LDI_PDP_OFFSET + 0x030)
#define PDP_DSI_TE_CTRL	(DSS_DPP_LDI_PDP_OFFSET + 0x034)
#define PDP_DSI_TE_HS_NUM	(DSS_DPP_LDI_PDP_OFFSET + 0x038)
#define PDP_DSI_TE_HS_WD	(DSS_DPP_LDI_PDP_OFFSET + 0x03C)
#define PDP_DSI_TE_VS_WD	(DSS_DPP_LDI_PDP_OFFSET + 0x040)

#define PDP_LDI_FRM_MSK	(DSS_DPP_LDI_PDP_OFFSET + 0x044)
#define PDP_LDI_VINACT_MSK_LEN	(DSS_DPP_LDI_PDP_OFFSET + 0x048)
#define PDP_LDI_CG_CTRL	(DSS_DPP_LDI_PDP_OFFSET + 0x04C)
#define PDP_SRAM_LP_CTRL	(DSS_DPP_LDI_PDP_OFFSET + 0x050)

/* LDI_SDP */
#define DSS_LDI_SDP_OFFSET	(DSS_DPE1_OFFSET + 0x0000F000)

#define SDP_LDI_HRZ_CTRL0	(DSS_LDI_SDP_OFFSET + 0x000)
#define SDP_LDI_HRZ_CTRL1	(DSS_LDI_SDP_OFFSET + 0x004)
#define SDP_LDI_RESERVED0	(DSS_LDI_SDP_OFFSET + 0x008)
#define SDP_LDI_RESERVED1	(DSS_LDI_SDP_OFFSET + 0x00C)
#define SDP_LDI_VRT_CTRL0	(DSS_LDI_SDP_OFFSET + 0x010)
#define SDP_LDI_VRT_CTRL1	(DSS_LDI_SDP_OFFSET + 0x014)
#define SDP_LDI_PLR_CTRL	(DSS_LDI_SDP_OFFSET + 0x018)
#define SDP_LDI_RESERVED2	(DSS_LDI_SDP_OFFSET + 0x01C)
#define SDP_LDI_3D_CTRL	(DSS_LDI_SDP_OFFSET + 0x020)
#define SDP_LDI_CTRL	(DSS_LDI_SDP_OFFSET + 0x024)
#define SDP_LDI_WORK_MODE	(DSS_LDI_SDP_OFFSET + 0x028)
#define SDP_LDI_DE_SPACE_LOW	(DSS_LDI_SDP_OFFSET + 0x02C)

#define SDP_DSI_CMD_MOD_CTRL	(DSS_LDI_SDP_OFFSET + 0x030)
#define SDP_DSI_TE_CTRL	(DSS_LDI_SDP_OFFSET + 0x034)
#define SDP_DSI_TE_HS_NUM	(DSS_LDI_SDP_OFFSET + 0x038)
#define SDP_DSI_TE_HS_WD	(DSS_LDI_SDP_OFFSET + 0x03C)
#define SDP_DSI_TE_VS_WD	(DSS_LDI_SDP_OFFSET + 0x040)

#define SDP_LDI_FRM_MSK	(DSS_LDI_SDP_OFFSET + 0x044)
#define SDP_LDI_VINACT_MSK_LEN	(DSS_LDI_SDP_OFFSET + 0x048)


/*******************************************************************************
** DPP_CTRL
*/
#define DSS_DPP0_CTRL_OFFSET	(DSS_DPP_OFFSET + 0x0000A000)
#define DSS_DPP1_CTRL_OFFSET	(DSS_DPE1_OFFSET+ 0x0000A200)

/* DPP0 */
#define DPP_ID	(0x0)
#define DPP_IMG_HRZ	(0x04)
#define DPP_IMG_VRT	(0x08)
#define DPP_CLRBAR_CTRL	(0x14)
#define DPP_DITHER	(0x18)
#define DPP_SBL	(0x1C)
#define DPP_CLK_GT	(0x20)

#define PDP_LDI_MCU_IRQ	(DSS_DPP0_CTRL_OFFSET + 0x200)
#define PDP_LDI_MCU_IRQ_MSK	(DSS_DPP0_CTRL_OFFSET + 0x204)
#define PDP_LDI_MCU_IRQ_CLR	(DSS_DPP0_CTRL_OFFSET + 0x208)
#define PDP_LDI_CPU_IRQ	(DSS_DPP0_CTRL_OFFSET + 0x300)
#define PDP_LDI_CPU_IRQ_MSK	(DSS_DPP0_CTRL_OFFSET + 0x304)
#define PDP_LDI_CPU_IRQ_CLR	(DSS_DPP0_CTRL_OFFSET + 0x308)
#define PDP_LDI_CPU_IRQ_RAWSTAT	(DSS_DPP0_CTRL_OFFSET + 0x30C)
#define PDP_DPP0_CFG_GT	(DSS_DPP0_CTRL_OFFSET + 0x310)
#define PDP_DFS_CG_GT	(DSS_DPP0_CTRL_OFFSET + 0x314)
#define PDP_DPP0_RSV0	(DSS_DPP0_CTRL_OFFSET + 0x318)

/* DPP1 */
#define SDP_LDI_MCU_IRQ	(DSS_DPP1_CTRL_OFFSET + 0x0)
#define SDP_LDI_MCU_IRQ_MSK	(DSS_DPP1_CTRL_OFFSET + 0x4)
#define SDP_LDI_MCU_IRQ_CLR	(DSS_DPP1_CTRL_OFFSET + 0x8)
#define SDP_LDI_CPU_IRQ	(DSS_DPP1_CTRL_OFFSET + 0x100)
#define SDP_LDI_CPU_IRQ_MSK	(DSS_DPP1_CTRL_OFFSET + 0x104)
#define SDP_LDI_CPU_IRQ_CLR	(DSS_DPP1_CTRL_OFFSET + 0x108)
#define SDP_LDI_CPU_IRQ_RAWSTAT		(DSS_DPP1_CTRL_OFFSET + 0x10C)
#define SDP_DPP1_CFG_GT	(DSS_DPP1_CTRL_OFFSET + 0x110)
#define SDP_DFS_CG_GT	(DSS_DPP1_CTRL_OFFSET + 0x114)
#define SDP_MODE_3D	(DSS_DPP1_CTRL_OFFSET + 0x118)
#define SDP_DPP1_RSV0	(DSS_DPP1_CTRL_OFFSET + 0x11C)


/*******************************************************************************
** SBL
*/
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


#define DSS_DPP_SBL_OFFSET	(DSS_DPP_OFFSET + 0x0000F000)

#define SBL_DATA_FORMAT	(0x00000000)
#define SBL_CONTROL_REG1	(0x00000004)
#define SBL_HS_POS_L	(0x00000008)
#define SBL_HS_POS_H	(0x0000000C)
#define SBL_FRAME_WIDTH_L	(0x00000010)
#define SBL_FRAME_WIDTH_H	(0x00000014)
#define SBL_FRAME_HEIGHT_L	(0x00000018)
#define SBL_FRAME_HEIGHT_H	(0x0000001C)
#define SBL_VS_POS_L	(0x00000020)
#define SBL_VS_POS_H	(0x00000024)
#define SBL_ENABLE	(0x000000400)
#define SBL_STRENGTH	(0x000000408)
#define SBL_VARIANCE_INTENSITY_SPACE	(0x00000040C)
#define SBL_SLOPE_MAX	(0x000000410)
#define SBL_SLOPE_MIN	(0x000000414)
#define SBL_BLACK_LEVEL_L	(0x000000418)
#define SBL_BLACK_LEVEL_H	(0x00000041C)
#define SBL_WHITE_LEVEL_L	(0x000000420)
#define SBL_WHITE_LEVEL_H	(0x000000424)
#define SBL_BRIGHT_DARK_AMP_LIMIT	(0x000000428)
#define SBL_DITHER_MODE	(0x00000042C)
#define SBL_CONFIG_BUFFER_MODE	(0x000000430)
#define SBL_CONFIG_BUFFER_GLOBAL	(0x000000434)
#define SBL_ASYMMETRY_LUT_ADDR	(0x000000480)
#define SBL_ASYMMETRY_LUT_WRITE_DATA_L	(0x000000484)
#define SBL_ASYMMETRY_LUT_WRITE_DATA_H	(0x000000488)
#define SBL_COLOR_CORRECTION_LUT_ADDR	(0x0000004C0)
#define SBL_COLOR_CORRECTION_LUT_WRITE_DATA_L	(0x0000004C4)
#define SBL_COLOR_CORRECTION_LUT_WRITE_DATA_H	(0x0000004C8)
#define SBL_LOGO_LEFT	(0x000000800)
#define SBL_LOGO_TOP	(0x000000804)
#define SBL_APICAL_DITHER	(0x000000840)
#define SBL_AUTOMATIC_START_CALC_STRENGTH_DRC_BACKLIGHT_SEL	(0x000000C00)
#define SBL_STRENGTH_MANUAL	(0x000000C04)
#define SBL_DRC_IN_L	(0x000000C08)
#define SBL_DRC_IN_H	(0x000000C0C)
#define SBL_BACKLIGHT_L	(0x000000C10)
#define SBL_BACKLIGHT_H	(0x000000C14)
#define SBL_AMBIENT_LIGHT_L	(0x000000C18)
#define SBL_AMBIENT_LIGHT_H	(0x000000C1C)
#define SBL_START_CALC	(0x000000C20)
#define SBL_STRENGTH_LIMIT	(0x000000C24)
#define SBL_CALIBRATION_A_L	(0x000000C28)
#define SBL_CALIBRATION_A_H	(0x000000C2C)
#define SBL_CALIBRATION_B_L	(0x000000C30)
#define SBL_CALIBRATION_B_H	(0x000000C34)
#define SBL_CALIBRATION_C_L	(0x000000C38)
#define SBL_CALIBRATION_C_H	(0x000000C3C)
#define SBL_CALIBRATION_D_L	(0x000000C40)
#define SBL_CALIBRATION_D_H	(0x000000C44)
#define SBL_T_FILTER_CONTROL	(0x000000C48)
#define SBL_BACKLIGHT_MIN_L	(0x000000C4C)
#define SBL_BACKLIGHT_MIN_H	(0x000000C50)
#define SBL_BACKLIGHT_MAX_L	(0x000000C54)
#define SBL_BACKLIGHT_MAX_H	(0x000000C58)
#define SBL_BACKLIGHT_SCALE_L	(0x000000C5C)
#define SBL_BACKLIGHT_SCALE_H	(0x00000C60)
#define SBL_AMBIENT_LIGHT_MIN_L	(0x00000C64)
#define SBL_AMBIENT_LIGHT_MIN_H	(0x00000C68)
#define SBL_FILTER_A_L	(0x00000C6C)
#define SBL_FILTER_A_H	(0x00000C70)
#define SBL_FILTER_B	(0x00000C74)
#define SBL_STRENGTH_OUT	(0x00000C80)
#define SBL_DRC_OUT_L	(0x00000C84)
#define SBL_DRC_OUT_H	(0x00000C88)
#define SBL_BACKLIGHT_OUT_L	(0x00000C8C)
#define SBL_BACKLIGHT_OUT_H	(0x00000C90)
#define SBL_CALC_DONE	(0x00000C94)
#define SBL_AL_CALIB_LUT_LUT_ADDR	(0x00000F00)
#define SBL_AL_CALIB_LUT_LUT_WRITE_DATA_L	(0x00000F10)
#define SBL_AL_CALIB_LUT_LUT_WRITE_DATA_H	(0x00000F14)


/*******************************************************************************
** IFBC
*/
#define DSS_DPP_IFBC_OFFSET	(DSS_DPP_OFFSET + 0x0000A800)

#define IFBC_SIZE	(0x0)
#define IFBC_CTRL	(0x4)
#define IFBC_Himax_CTRL0	(0x8)
#define IFBC_Himax_CTRL1	(0xC)
#define IFBC_Himax_CTRL2	(0x10)
#define IFBC_Himax_CTRL3	(0x14)
#define IFBC_EN	(0x18)
#define IFBC_PM_CTRL	(0x1C)
#define IFBC_MEM_CTRL	(0x20)
#define IFBC_HIMAX_TEST_MODE	(0x24)
#define IFBC_CORE_GT	(0x28)


/*******************************************************************************
** LCP
*/
#define DSS_DPP_LCP_OFFSET	(DSS_DPP_OFFSET + 0x00000000)
#define LCP_IGM_R_LUT (0x8000)
#define LCP_IGM_G_LUT (0x8400)
#define LCP_IGM_B_LUT (0x8800)
#define LCP_XCC_COEF_00 (0x9000)
#define LCP_XCC_COEF_01 (0x9004)
#define LCP_XCC_COEF_02 (0x9008)
#define LCP_XCC_COEF_03 (0x900c)
#define LCP_XCC_COEF_10 (0x9010)
#define LCP_XCC_COEF_11 (0x9014)
#define LCP_XCC_COEF_12 (0x9018)
#define LCP_XCC_COEF_13 (0x901c)
#define LCP_XCC_COEF_20 (0x9020)
#define LCP_XCC_COEF_21 (0x9024)
#define LCP_XCC_COEF_22 (0x9028)
#define LCP_XCC_COEF_23 (0x902c)
#define LCP_GMP_BYPASS_EN (0x9100)
#define LCP_XCC_BYPASS_EN  (0x9104)
#define LCP_IGM_GT (0x9108)
#define LCP_GMP_GT (0x910c)
#define LCP_XCC_GT (0x9110)
#define LCP_IGM_MEM_CTRL_S (0x9114)
#define LCP_GMP_MEM_CTRL_S (0x9118)
#define LCP_XCC_MEM_CTRL_S (0x911c)

/*******************************************************************************
** GAMMA
*/
#define DSS_DPP_GAMMA_OFFSET	(DSS_DPP_OFFSET + 0x0000B000)
#define GAMMA_R_LUT (0x000)
#define GAMMA_G_LUT (0x400)
#define GAMMA_B_LUT (0x800)
#define GAMMA_SHORTBIT_EN (0xc00)
#define GAMMA_BYPASS_EN (0xc04)
#define GAMMA_CORE_GT (0xc08)
#define GAMMA_MEM_CTRL_S  (0xc0c)

/*******************************************************************************
** ACM
*/
#define DSS_DPP_ACM_OFFSET	(DSS_DPP_OFFSET + 0x0000C000)
#define ACM_R0_H		(0x1C00)
#define ACM_R1_H		(0x1C04)
#define ACM_R2_H		(0x1C08)
#define ACM_R3_H		(0x1C0C)
#define ACM_R4_H		(0x1C10)
#define ACM_R5_H		(0x1C14)
#define ACM_R6_H		(0x1C18)
#define ACM_EN			(0x1C1C)
#define ACM_LUT_HUE		(0x0000)
#define ACM_LUT_VALUE	(0x0800)
#define ACM_LUT_SATA	(0x0A00)
#define ACM_LUT_SATR	(0x0C00)

/*******************************************************************************
** MIPI DSI
*/
#define DSS_MIPI_DSI0_OFFSET	(DSS_DPE0_OFFSET + 0x0000C000)
#define DSS_MIPI_DSI1_OFFSET	(DSS_DPE0_OFFSET + 0x0000C100)

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
** HDMI
*/
#define DSS_HDMI_OFFSET	(DSS_DPE1_OFFSET + 0x0000C000)
#define DSS_HDMIEFC_OFFSET	(DSS_DPE1_OFFSET + 0x0000E000)



#define ROT_TLB_SCENE_NUM	(5)

typedef struct dss_global_reg {
	char __iomem *ovl_mux_base;
	char __iomem *ovl_scene_base;
	char __iomem *rot_tlb_scene_base[ROT_TLB_SCENE_NUM];
	char __iomem *dpe0_cross_switch_base;
	char __iomem *dpe2_cross_switch_base;
	char __iomem *chn_ctl_base[DSS_CHN_MAX];
	char __iomem *chn_mux_base[DSS_CHN_MAX];
	char __iomem *rot_mux_base[DSS_CHN_MAX];
	char __iomem *scf_mux_base[DSS_CHN_MAX];
	char __iomem *rot_pm_ctl_base[DSS_CHN_MAX];
	char __iomem *sdp_mux_base;

	uint32_t ovl_mux_val;
	uint32_t ovl_scene_val;
	uint32_t rot_tlb_scene_val[ROT_TLB_SCENE_NUM];
	uint32_t dpe0_cross_switch_val;
	uint32_t dpe2_cross_switch_val;
	uint32_t chn_ctl_val[DSS_CHN_MAX];
	uint32_t chn_mux_val[DSS_CHN_MAX];
	uint32_t rot_mux_val[DSS_CHN_MAX];
	uint32_t scf_mux_val[DSS_CHN_MAX];
	uint32_t rot_pm_ctl_val[DSS_CHN_MAX];
	uint32_t sdp_mux_val;

	uint8_t ovl_mux_used;
	uint8_t ovl_scene_used;
	uint8_t rot_tlb_scene_used[ROT_TLB_SCENE_NUM];
	uint8_t dpe0_cross_switch_used;
	uint8_t dpe2_cross_switch_used;
	uint8_t chn_ctl_used[DSS_CHN_MAX];
	uint8_t chn_mux_used[DSS_CHN_MAX];
	uint8_t rot_mux_used[DSS_CHN_MAX];
	uint8_t scf_mux_used[DSS_CHN_MAX];
	uint8_t rot_pm_ctl_used[DSS_CHN_MAX];
	uint8_t sdp_mux_used;
} dss_global_reg_t;


/*
**  DSS MODULE REG Struct
*/
typedef struct dss_module_reg {
	char __iomem *rdma_bridge_base[DSS_ENG_MAX];
	char __iomem *rdma0_base[DSS_CHN_MAX];
	char __iomem *rdma1_base[DSS_CHN_MAX];
	char __iomem *fbdc_base[DSS_CHN_MAX];
	char __iomem *dfc_base[DSS_CHN_MAX];
	char __iomem *scf_base[DSS_SCF_MAX];
	char __iomem *scp_base[DSS_CHN_MAX];
	char __iomem *csc_base[DSS_CHN_MAX];
	char __iomem *ov_base[DSS_OVL_MAX];
	char __iomem *fbc_base[DSS_CHN_MAX];
	char __iomem *wdma_base[DSS_CHN_MAX];
	char __iomem *wdma_bridge_base[DSS_ENG_MAX];
	char __iomem *mmu_dma0_base[DSS_CHN_MAX];
	char __iomem *mmu_dma1_base[DSS_CHN_MAX];
	char __iomem *mmu_rptb_base;

	dss_rdma_bridge_t rdma_bridge[DSS_ENG_MAX];
	dss_rdma_t rdma0[DSS_CHN_MAX];
	dss_rdma_t rdma1[DSS_CHN_MAX];
	dss_fbdc_t fbdc[DSS_CHN_MAX];
	dss_dfc_t dfc[DSS_CHN_MAX];
	dss_scf_t scf[DSS_SCF_MAX];
	dss_scp_t scp[DSS_CHN_MAX];
	dss_csc_t csc[DSS_CHN_MAX];
	dss_ovl_t ov[DSS_OVL_MAX];
	dss_fbc_t fbc[DSS_CHN_MAX];
	dss_wdma_t wdma[DSS_CHN_MAX];
	dss_wdma_bridge_t wdma_bridge[DSS_ENG_MAX];
	dss_mmu_dma_t mmu_dma0[DSS_CHN_MAX];
	dss_mmu_dma_t mmu_dma1[DSS_CHN_MAX];
	dss_mmu_rptb_t mmu_rptb;

	uint8_t rdma_bridge_used[DSS_ENG_MAX];
	uint8_t rdma0_used[DSS_CHN_MAX];
	uint8_t rdma1_used[DSS_CHN_MAX];
	uint8_t fbdc_used[DSS_CHN_MAX];
	uint8_t dfc_used[DSS_CHN_MAX];
	uint8_t scf_used[DSS_SCF_MAX];
	uint8_t scp_used[DSS_CHN_MAX];
	uint8_t csc_used[DSS_CHN_MAX];
	uint8_t ov_used[DSS_OVL_MAX];
	uint8_t fbc_used[DSS_CHN_MAX];
	uint8_t wdma_used[DSS_CHN_MAX];
	uint8_t wdma_bridge_used[DSS_ENG_MAX];
	uint8_t mmu_dma0_used[DSS_CHN_MAX];
	uint8_t mmu_dma1_used[DSS_CHN_MAX];
	uint8_t mmu_rptb_used;

	uint8_t dpe2_on_ref;
	uint8_t rot1_on_ref;
} dss_module_reg_t;

typedef struct dss_exception {
	uint8_t chn_used[DSS_CHN_MAX];
	uint32_t chn_format_used[DSS_CHN_MAX];
	uint8_t underflow_exception;
	uint8_t underflow_handler;
} dss_exception_t;

typedef struct dss_rptb_info {
	uint32_t rptb_offset[DSS_ROT_MAX];
	uint32_t rptb_size[DSS_ROT_MAX];
} dss_rptb_info_t;


#endif  /* HISI_DSS_REGS_H */
