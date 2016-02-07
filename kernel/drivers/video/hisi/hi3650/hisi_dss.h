/* include/linux/hisi_dss.h
 *
 * Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
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

#ifndef _HISI_DSS_H_
#define _HISI_DSS_H_

#include <linux/types.h>
#include <linux/fb.h>


#define HISIFB_IOCTL_MAGIC 'M'

#define HISIFB_VSYNC_CTRL _IOW(HISIFB_IOCTL_MAGIC, 0x01, unsigned int)
#define HISIFB_SBL_CTRL _IOW(HISIFB_IOCTL_MAGIC, 0x02, unsigned int)
#define HISIFB_DSS_CLK_RATE_SET _IOW(HISIFB_IOCTL_MAGIC, 0x03, struct dss_clk_rate)
#define HISIFB_PIXCLK_RATE_SET _IOW(HISIFB_IOCTL_MAGIC, 0x04, struct fb_var_screeninfo)
#define HISIFB_QUERY_UPDT_STATE _IOW(HISIFB_IOCTL_MAGIC, 0x05, int)
//#define HISIFB_DIRTY_REGION_UPDT_SET _IOW(HISIFB_IOCTL_MAGIC, 0x05, int)

#define HISIFB_OV_TEST _IOW(HISIFB_IOCTL_MAGIC, 0x10, struct dss_overlay_test)
#define HISIFB_OV_ONLINE_PLAY _IOW(HISIFB_IOCTL_MAGIC, 0x11, struct dss_overlay)
#define HISIFB_OV_OFFLINE_PLAY _IOW(HISIFB_IOCTL_MAGIC, 0x12, struct dss_overlay)
#define HISIFB_OV_ONLINE_WB _IOW(HISIFB_IOCTL_MAGIC, 0x13, struct dss_overlay)
#define HISIFB_OV_ONLINE_WB_CTRL _IOW(HISIFB_IOCTL_MAGIC, 0x14, int)

#define HISIFB_PURE_LAYER_CHECK _IOW(HISIFB_IOCTL_MAGIC, 0x20, struct dss_pure_layer_check)
#define HISIFB_RESOLUTION_SWITCH _IOWR(HISIFB_IOCTL_MAGIC, 0x21, int)
#define HISIFB_RESOLUTION_UPDATE _IOWR(HISIFB_IOCTL_MAGIC, 0x22, int)

#define HISIFB_CHECK_MIPI_TR _IOR(HISIFB_IOCTL_MAGIC, 0x30, int)
#define HISIFB_MIPI_DETECT _IOR(HISIFB_IOCTL_MAGIC, 0x31, int)


/* for fb0 fb1 fb2 and so on */
#define PRIMARY_PANEL_IDX	(0)
#define EXTERNAL_PANEL_IDX	(1)
#define AUXILIARY_PANEL_IDX	(2)

/* for YUV */
#define MAX_PLANES	(3)


enum hisi_fb_pixel_format {
	HISI_FB_PIXEL_FORMAT_RGB_565 = 0,
	HISI_FB_PIXEL_FORMAT_RGBX_4444,
	HISI_FB_PIXEL_FORMAT_RGBA_4444,
	HISI_FB_PIXEL_FORMAT_RGBX_5551,
	HISI_FB_PIXEL_FORMAT_RGBA_5551,
	HISI_FB_PIXEL_FORMAT_RGBX_8888,
	HISI_FB_PIXEL_FORMAT_RGBA_8888,

	HISI_FB_PIXEL_FORMAT_BGR_565,
	HISI_FB_PIXEL_FORMAT_BGRX_4444,
	HISI_FB_PIXEL_FORMAT_BGRA_4444,
	HISI_FB_PIXEL_FORMAT_BGRX_5551,
	HISI_FB_PIXEL_FORMAT_BGRA_5551,
	HISI_FB_PIXEL_FORMAT_BGRX_8888,
	HISI_FB_PIXEL_FORMAT_BGRA_8888,

	/* YUV Package */
	HISI_FB_PIXEL_FORMAT_YUV_422_I, /* YUY2*/

	/* YUV Semi-planar */
	HISI_FB_PIXEL_FORMAT_YCbCr_422_SP, /* NV16 */
	HISI_FB_PIXEL_FORMAT_YCrCb_422_SP,
	HISI_FB_PIXEL_FORMAT_YCbCr_420_SP,
	HISI_FB_PIXEL_FORMAT_YCrCb_420_SP, /* NV21*/

	/* YUV Planar */
	HISI_FB_PIXEL_FORMAT_YCbCr_422_P,
	HISI_FB_PIXEL_FORMAT_YCrCb_422_P,
	HISI_FB_PIXEL_FORMAT_YCbCr_420_P,
	HISI_FB_PIXEL_FORMAT_YCrCb_420_P, /* HISI_FB_PIXEL_FORMAT_YV12 */
};

enum hisi_fb_blending {
	HISI_FB_BLENDING_NONE = 0,
	HISI_FB_BLENDING_PREMULT = 1,
	HISI_FB_BLENDING_COVERAGE = 2,
	HISI_FB_BLENDING_MAX = 3,
};

enum hisi_fb_transform{
	HISI_FB_TRANSFORM_NOP = 0x0,
	/* flip source image horizontally (around the vertical axis) */
	HISI_FB_TRANSFORM_FLIP_H = 0x01,
	/* flip source image vertically (around the horizontal axis)*/
	HISI_FB_TRANSFORM_FLIP_V = 0x02,
	/* rotate source image 90 degrees clockwise */
	HISI_FB_TRANSFORM_ROT_90 = 0x04,
	/* rotate source image 180 degrees */
	HISI_FB_TRANSFORM_ROT_180 = 0x03,
	/* rotate source image 270 degrees clockwise */
	HISI_FB_TRANSFORM_ROT_270 = 0x07,
};

enum dss_csc_mode {
	DSS_CSC_601_WIDE = 0,
	DSS_CSC_601_NARROW,
	DSS_CSC_709_WIDE,
	DSS_CSC_709_NARROW,
};

enum dss_chn_idx {
	DSS_RCHN_NONE = -1,
	DSS_RCHN_D0 = 0,
	DSS_RCHN_D1,
	DSS_RCHN_D2,
	DSS_RCHN_D3,

	DSS_RCHN_G0,
	DSS_RCHN_G1,

	DSS_RCHN_VG0,
	DSS_RCHN_VG1,

	DSS_WCHN_W0,
	DSS_WCHN_W1,

	DSS_CHN_MAX,
};

enum dss_ovl_idx {
	DSS_OVL0= 0,
	DSS_OVL1,
	DSS_OVL2,
	DSS_OVL3,
	DSS_OVL_IDX_MAX,
};

/* dss capability priority description */
#define CAP_YUV_P	(1 << 5) /* YUV Planar */
#define CAP_YUV_SP	(1 << 4) /* YUV SemiPlanar */
#define CAP_SCL	(1 << 3) /* Scale */
#define CAP_ROT	(1 << 2) /* Rotate */
#define CAP_PURE_COLOR	(1 << 1)	/* PURE COLOR */
#define CAP_BASE	(1 << 0) /* Background */

typedef struct dss_rect {
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
} dss_rect_t;

typedef struct dss_rect_ltrb {
	uint32_t left;
	uint32_t top;
	uint32_t right;
	uint32_t bottom;
} dss_rect_ltrb_t;

typedef struct dss_img {
	uint32_t format;
	uint32_t width;
	uint32_t height;
	uint32_t bpp;	/* bytes per pixel */
	uint32_t stride;
	uint32_t uStride;
	uint32_t vStride;
	uint32_t buf_size;
	uint64_t phy_addr;
	uint64_t vir_addr;
	uint64_t uOffset;
	uint64_t vOffset;

	uint32_t ptba;
	uint32_t ptva;
	uint32_t is_tile;
	uint32_t mmu_enable;
	uint32_t csc_mode;
	uint32_t secure_mode;
} dss_img_t;

typedef struct dss_layer {
	dss_img_t img;
	dss_rect_t src_rect;
	dss_rect_t src_rect_mask;
	dss_rect_t dst_rect;
	uint32_t transform;
	int32_t blending;
	uint32_t glb_alpha;
	uint32_t color; /* background color or dim color */
	int32_t layer_idx;
	int32_t chn_idx;
	uint32_t need_cap;

	int32_t acquire_fence;
} dss_layer_t;

typedef struct dss_wb_layer {
	dss_img_t img;
	dss_rect_t src_rect;
	dss_rect_t dst_rect;
	uint32_t transform;
	int32_t chn_idx;
	uint32_t need_cap;
	uint32_t reserved0;
	int32_t acquire_fence;
	int32_t release_fence;
} dss_wb_layer_t;


/* Max multi-src channel number of the DSS. */
#define MAX_DSS_SRC_NUM	(9)
#define MAX_DSS_DST_NUM	(2)

typedef struct dss_overlay {
	dss_layer_t layer_infos[MAX_DSS_SRC_NUM];
	int32_t layer_nums;
	int32_t ovl_idx;

	dss_wb_layer_t wb_layer_infos[MAX_DSS_DST_NUM];
	int32_t wb_layer_nums;

	int32_t release_fence;

	dss_rect_t dirty_rect_updt;
} dss_overlay_t;


typedef struct dss_clk_rate {
	uint64_t dss_pri_clk_rate;
	uint64_t dss_aux_clk_rate;
	uint64_t dss_pclk_clk_rate;
} dss_clk_rate_t;

typedef struct dss_pure_layer_check {
	int32_t layer_index;
	uint32_t color_flag;
	uint32_t alpha_flag;
	uint32_t color; /* argb8888 (31:0 AARRGGBB) */
} dss_pure_layer_check_t;


#define HISI_FB_DSS_REG_TEST_NUM	(1024)

typedef struct dss_reg {
	uint64_t reg_addr;
	uint32_t reg_val;
	uint32_t reserved0;
} dss_reg_t;

typedef struct dss_overlay_test {
	uint32_t reg_cnt;
	uint32_t reserved0;
	dss_reg_t *reg;
} dss_overlay_test_t;


#endif /*_HISI_DSS_H_*/
