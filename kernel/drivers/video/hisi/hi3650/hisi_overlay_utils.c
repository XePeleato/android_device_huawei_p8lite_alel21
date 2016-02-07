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


static int g_dss_module_resource_initialized = 0;
uint32_t frame_count = 0;

uint32_t g_dss_module_base[DSS_CHN_MAX][MODULE_CHN_MAX] = {
	/* D0 */
	{
	BIT_DMA0_FLUSH_EN,
	DSS_MMU_RBRG1_OFFSET + DSS_MMU_RDMA2_OFFSET, //FIXME:
	0,
	DSS_RCH_D0_DMA_OFFSET,
	DSS_RCH_D0_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	},

	/* D1 */
	{
	BIT_DMA1_FLUSH_EN,
	DSS_MMU_RBRG1_OFFSET + DSS_MMU_RDMA3_OFFSET, //FIXME:
	0,
	DSS_RCH_D1_DMA_OFFSET,
	DSS_RCH_D1_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	},

	/* D2 */
	{
	BIT_DMA2_FLUSH_EN,
	DSS_MMU_RBRG0_OFFSET + DSS_MMU_RDMA0_OFFSET,
	0,
	DSS_RCH_D2_DMA_OFFSET,
	DSS_RCH_D2_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	},

	/* D3 */
	{
	BIT_DMA3_FLUSH_EN,
	DSS_MMU_RBRG0_OFFSET + DSS_MMU_RDMA1_OFFSET,
	0,
	DSS_RCH_D3_DMA_OFFSET,
	DSS_RCH_D3_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	},

	/* G0 */
	{
	BIT_RGB0_FLUSH_EN,
	DSS_MMU_RBRG0_OFFSET + DSS_MMU_RDMA4_OFFSET,
	0,
	DSS_RCH_G0_DMA_OFFSET,
	DSS_RCH_G0_DFC_OFFSET,
	DSS_RCH_G0_SCL_OFFSET,
	DSS_RCH_G0_SCL_LUT_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	},

	/* G1 */
	{
	BIT_RGB1_FLUSH_EN,
	DSS_MMU_RBRG1_OFFSET + DSS_MMU_RDMA1_OFFSET,  //FIXME:
	0,
	DSS_RCH_G1_DMA_OFFSET,
	DSS_RCH_G1_DFC_OFFSET,
	DSS_RCH_G1_SCL_OFFSET,
	DSS_RCH_G1_SCL_LUT_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	},

	/* VG0 */
	{
	BIT_VIG0_FLUSH_EN,
	DSS_MMU_RBRG0_OFFSET + DSS_MMU_RDMA2_OFFSET,
	DSS_MMU_RBRG0_OFFSET + DSS_MMU_RDMA3_OFFSET,
	DSS_RCH_VG0_DMA_OFFSET,
	DSS_RCH_VG0_DFC_OFFSET,
	DSS_RCH_VG0_SCL_OFFSET,
	DSS_RCH_VG0_SCL_LUT_OFFSET,
	DSS_RCH_VG0_SHARPNESS_OFFSET,
	DSS_RCH_VG0_CSC_OFFSET,
	DSS_RCH_VG0_CE_OFFSET,
	DSS_RCH_VG0_CE0_LUT_OFFSET,
	DSS_RCH_VG0_CE1_LUT_OFFSET,
	DSS_RCH_VG0_CE2_LUT_OFFSET,
	DSS_RCH_VG0_CE3_LUT_OFFSET,
	},

	/* VG1 */
	{
	BIT_VIG1_FLUSH_EN,
	DSS_MMU_RBRG1_OFFSET + DSS_MMU_RDMA0_OFFSET, //FIXME:
	0,
	DSS_RCH_VG1_DMA_OFFSET,
	DSS_RCH_VG1_DFC_OFFSET,
	DSS_RCH_VG1_SCL_OFFSET,
	DSS_RCH_VG1_SCL_LUT_OFFSET,
	DSS_RCH_VG1_SHARPNESS_OFFSET,
	DSS_RCH_VG1_CSC_OFFSET,
	DSS_RCH_VG1_CE_OFFSET,
	DSS_RCH_VG1_CE0_LUT_OFFSET,
	DSS_RCH_VG1_CE1_LUT_OFFSET,
	DSS_RCH_VG1_CE2_LUT_OFFSET,
	DSS_RCH_VG1_CE3_LUT_OFFSET,
	},

	/* W0 */
	{
	BIT_WB0_FLUSH_EN,
	DSS_WCH0_DMA_OFFSET,
	DSS_WCH0_DFC_OFFSET,
	0,
	0,
	0,
	DSS_WCH0_CSC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	},

	/* W1 */
	{
	BIT_WB1_FLUSH_EN,
	DSS_WCH1_DMA_OFFSET,
	DSS_WCH1_DFC_OFFSET,
	0,
	0,
	0,
	DSS_WCH1_CSC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	},
};

uint32_t g_dss_module_ovl_base[DSS_OVL_IDX_MAX][MODULE_OVL_MAX] = {
	{DSS_OVL0_OFFSET, DSS_MCTRL_CTL0_OFFSET},
	{DSS_OVL1_OFFSET, DSS_MCTRL_CTL1_OFFSET},
	{DSS_OVL2_OFFSET, DSS_MCTRL_CTL2_OFFSET},
	{DSS_OVL3_OFFSET, DSS_MCTRL_CTL3_OFFSET},
};

uint32_t g_dss_module_brg_base[DSS_RBRG_IDX_MAX] = {
	DSS_RDMA_BRIDAGE0_OFFSET,
	DSS_RDMA_BRIDAGE1_OFFSET,
	0,
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

		printk("\t layer_idx | chn_idx | transform | blending | alpha |   color  | need_cap \n");
		printk("\t-----------+---------+-----------+----------+-------+------------+----------\n");
		printk("\t %9d | %7d | %9d | %8u | %5d | 0x%8x | %8u \n",
			layer->layer_idx, layer->chn_idx,
			layer->transform, layer->blending, layer->glb_alpha,
			layer->color, layer->need_cap);

		printk("\n");
		printk("\t format |  width | height |  bpp   | stride | uStride | vStride |  phy_addr  |  vir_addr  | uOffset | vOffset \n");
		printk("\t--------+--------+--------+--------+--------+---------+---------+------------+------------+------------+------------\n");
		printk("\t %6u | %6u | %6u | %6u | %6u | %7u | %7u | %llu | %llu | %llu | %llu \n",
			layer->img.format, layer->img.width, layer->img.height, layer->img.bpp,
			layer->img.stride, layer->img.uStride, layer->img.vStride, layer->img.phy_addr,
			layer->img.vir_addr, layer->img.uOffset, layer->img.vOffset);

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

	for (j = 0; j < ov_data->layer_nums; j++) {
		wb_layer = &(ov_data->wb_layer_infos[j]);
		printk("    wb info:\n");

		printk("\n");
		printk("\t format |  width | height |  bpp   | stride | uStride | vStride |  phy_addr  |  vir_addr  | uOffset | vOffset \n");
		printk("\t--------+--------+--------+--------+--------+---------+---------+------------+------------+------------+------------\n");
		printk("\t %6u | %6u | %6u | %6u | %6u | %7u | %7u | %llu | %llu | %llu | %llu \n",
				wb_layer->img.format, wb_layer->img.width, wb_layer->img.height, wb_layer->img.bpp,
				wb_layer->img.stride, wb_layer->img.uStride, wb_layer->img.vStride, wb_layer->img.phy_addr,
				wb_layer->img.vir_addr, wb_layer->img.uOffset, wb_layer->img.vOffset);

		printk("\n");
		printk("\t         src_rect         |          dst_rect         | chn_idx |transform\n");
		printk("\t--------------------------+---------------------------|---------|---------|\n");
		printk("\t[%5d,%5d,%5d,%5d] | [%5d,%5d,%5d,%5d] |  %7d|%9d\n",
			wb_layer->src_rect.x, wb_layer->src_rect.y,
			wb_layer->src_rect.w, wb_layer->src_rect.h,
			wb_layer->dst_rect.x, wb_layer->dst_rect.y,
			wb_layer->dst_rect.w, wb_layer->dst_rect.h,
			wb_layer->chn_idx, wb_layer->transform);
	}

	printk("\n");
}

int hisi_dss_check_layer_par(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	if (layer->need_cap & (CAP_BASE | CAP_PURE_COLOR))
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

static bool isSrcRectMasked(dss_layer_t *dss_layer)
{
	BUG_ON(dss_layer == NULL);

	return ((dss_layer->src_rect_mask.w != 0) &&
		(dss_layer->src_rect_mask.h != 0));
}

static uint32_t isNeedRdmaStretchBlt(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	uint32_t v_stretch_ratio_threshold = 0;
	uint32_t v_stretch_ratio = 0;

	BUG_ON(layer == NULL);

	if (is_YUV_SP_420(layer->img.format) || is_YUV_P_420(layer->img.format)) {
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

static void hisi_adjust_clip_rect(dss_layer_t *layer, dss_rect_ltrb_t *clip_rect)
{
	uint32_t temp = 0;

	switch (layer->transform) {
	case HISI_FB_TRANSFORM_NOP:
		// do nothing
		break;
	case HISI_FB_TRANSFORM_FLIP_H:
		{
			temp = clip_rect->top;
			clip_rect->top = clip_rect->bottom;
			clip_rect->bottom = temp;
		}
		break;
	case HISI_FB_TRANSFORM_FLIP_V:
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
		stride = layer->img.stride;
		src_addr = mmu_enable ? layer->img.vir_addr : layer->img.phy_addr;
		bpp = layer->img.bpp;
	} else if (add_type == DSS_ADDR_PLANE1) {
		stride = layer->img.uStride;
		src_addr = mmu_enable ? (layer->img.vir_addr + layer->img.uOffset) :
			(layer->img.phy_addr + layer->img.uOffset);
		bpp = 1;

		if (is_YUV_P_420(layer->img.format) || is_YUV_P_422(layer->img.format)) {
			left /= 2;
			right /= 2;
		}

		if (is_YUV_SP_420(layer->img.format) || is_YUV_P_420(layer->img.format)) {
			top /= 2;
			bottom /= 2;
		}
	} else if (add_type == DSS_ADDR_PLANE2) {
		stride = layer->img.vStride;
		src_addr = mmu_enable ? (layer->img.vir_addr + layer->img.vOffset) :
			(layer->img.phy_addr + layer->img.vOffset);
		bpp = 1;

		if (is_YUV_P_420(layer->img.format) || is_YUV_P_422(layer->img.format)) {
			left /= 2;
			right /= 2;
		}

		if (is_YUV_SP_420(layer->img.format) || is_YUV_P_420(layer->img.format)) {
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
	case HISI_FB_PIXEL_FORMAT_BGR_565:
		ret = DMA_PIXEL_FORMAT_RGB_565;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_4444:
	case HISI_FB_PIXEL_FORMAT_BGRX_4444:
		ret = DMA_PIXEL_FORMAT_XRGB_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_4444:
	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
		ret = DMA_PIXEL_FORMAT_ARGB_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_5551:
	case HISI_FB_PIXEL_FORMAT_BGRX_5551:
		ret = DMA_PIXEL_FORMAT_XRGB_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_5551:
	case HISI_FB_PIXEL_FORMAT_BGRA_5551:
		ret = DMA_PIXEL_FORMAT_ARGB_5551;
		break;

	case HISI_FB_PIXEL_FORMAT_RGBX_8888:
	case HISI_FB_PIXEL_FORMAT_BGRX_8888:
		ret = DMA_PIXEL_FORMAT_XRGB_8888;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_8888:
	case HISI_FB_PIXEL_FORMAT_BGRA_8888:
		ret = DMA_PIXEL_FORMAT_ARGB_8888;
		break;

	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
		ret = DMA_PIXEL_FORMAT_YUV_422_I;
		break;

	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
		ret = DMA_PIXEL_FORMAT_YUV_422_P_HP;
		break;
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		ret = DMA_PIXEL_FORMAT_YUV_420_P_HP;
		break;

	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
		ret = DMA_PIXEL_FORMAT_YUV_422_SP_HP;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
		ret = DMA_PIXEL_FORMAT_YUV_420_P_HP;
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
		ret = DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_V;
		break;
	case HISI_FB_TRANSFORM_ROT_180:
		ret = DSS_TRANSFORM_FLIP_V |DSS_TRANSFORM_FLIP_H;
		break;
	case HISI_FB_TRANSFORM_ROT_270:
		ret = DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_H;
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H):
		ret =  DSS_TRANSFORM_ROT | DSS_TRANSFORM_FLIP_V | DSS_TRANSFORM_FLIP_H;
		break;
	case (HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V):
		ret = DSS_TRANSFORM_ROT;
		break;
	default:
		ret = -1;
		HISI_FB_ERR("not support transform(%d)!", transform);
		break;
	}

	return ret;
}

static int hisi_pixel_format_hal2dfc(int format)
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

	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;

	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
		ret = DFC_PIXEL_FORMAT_YVYU422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
		ret = DFC_PIXEL_FORMAT_YVYU422;
		break;

	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;

	default:
		HISI_FB_ERR("not support format(%d)!\n", format);
		ret = -1;
		break;
	}

	return ret;
}

static int hisi_rb_swap(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_BGR_565:
	case HISI_FB_PIXEL_FORMAT_BGRX_4444:
	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
	case HISI_FB_PIXEL_FORMAT_BGRX_5551:
	case HISI_FB_PIXEL_FORMAT_BGRA_5551:
	case HISI_FB_PIXEL_FORMAT_BGRX_8888:
	case HISI_FB_PIXEL_FORMAT_BGRA_8888:
		return 1;
	default:
		return 0;
	}
}

static int hisi_uv_swap(int format)
{
	switch (format) {
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		return 1;

	default:
		return 0;
	}
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
void hisi_dss_unflow_handler(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, bool unmask)
{
	uint32_t tmp = 0;

	BUG_ON(hisifd == NULL);

	if (pov_req->ovl_idx == DSS_OVL0) {
		tmp = inp32(hisifd->dss_base + GLB_CPU_PDP_INT_MSK);
		if (unmask) {
			tmp &= ~BIT_LDI_UNFLOW;
		} else {
			tmp |= BIT_LDI_UNFLOW;
		}
		outp32(hisifd->dss_base + GLB_CPU_PDP_INT_MSK, tmp);

	} else if (pov_req->ovl_idx == DSS_OVL1) {
		tmp = inp32(hisifd->dss_base + GLB_CPU_SDP_INT_MSK);
		if (unmask) {
			tmp &= ~BIT_LDI_UNFLOW;
		} else {
			tmp |= BIT_LDI_UNFLOW;
		}
		outp32(hisifd->dss_base + GLB_CPU_SDP_INT_MSK, tmp);
	} else {
		; /* do nothing */
	}
}

int hisi_dss_handle_prev_ovl_req(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req)
{
	dss_layer_t *layer = NULL;
	int chn_idx = 0;
	int i = 0;
	uint32_t format = 0;
	uint32_t need_cap = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	for (i = 0; i < pov_req->layer_nums; i++) {
		layer = &pov_req->layer_infos[i];
		chn_idx = layer->chn_idx;
		format = layer->img.format;
		need_cap = layer->need_cap;

		/* RDMA */
		hisifd->dss_module.dma_used[chn_idx] = 1;

		/* MMU_DMA */
		if (layer->img.mmu_enable) {
			hisifd->dss_module.mmu_dma0_used[chn_idx] = 1;

			if (isYUVSemiPlanar(format) || isYUVPlanar(format)) {
				hisifd->dss_module.mmu_dma1_used[chn_idx] = 1;
			}

			if (isYUVPlanar(format)) {
				; //FIXME:
			}
		}

		/* DFC */
		hisifd->dss_module.dfc_used[chn_idx] = 1;

		/* SCF */
		if (need_cap & CAP_SCL) {
			hisifd->dss_module.scl_used[chn_idx] = 1;
		}

		/* CSC */
		if (isYUV(format)) {
			hisifd->dss_module.csc_used[chn_idx] = 1;
		}

		/* OV */
		hisifd->dss_module.ov_used[pov_req->ovl_idx] = 1;
	}

	return 0;
}


/*******************************************************************************
** DSS RDMA Bridge
*/
static int hisi_chnIdx2axiPort(int chn_idx, int ovl_idx)
{
	int ret = -1;

	if ((ovl_idx == DSS_OVL0) || (ovl_idx == DSS_OVL1)) {
		ret = AXI_CHN0;
	} else if ((ovl_idx == DSS_OVL2) || (ovl_idx == DSS_OVL3)) {
		ret = AXI_CHN1;
	} else {
		HISI_FB_ERR("not support this chn%d, ovl%d !\n", chn_idx, ovl_idx);
	}

	return ret;
}

static int hisi_chnIdx2brgIdx(int chn_idx)
{
	int ret = -1;

	switch (chn_idx) {
	case DSS_RCHN_D2:
	case DSS_RCHN_D3:
	case DSS_RCHN_VG0:
	case DSS_RCHN_G0:
		ret = DSS_RBRG0;
		break;

	case DSS_RCHN_VG1:
	case DSS_RCHN_G1:
	case DSS_RCHN_D0:
	case DSS_RCHN_D1:
		ret = DSS_RBRG1;
		break;

	case DSS_WCHN_W0:
	case DSS_WCHN_W1:
		ret = DSS_WBRG0;
		break;

	default:
		HISI_FB_ERR("not support this chn_idx(%d)!\n", chn_idx);
		break;
	}

	return ret;
}

static int hisi_chnIdx2brgDmaIdx(int chn_idx)
{
	int ret = 0;

	switch (chn_idx) {
	case DSS_RCHN_D2:
		ret = DSS_RDMA0;
		break;
	case DSS_RCHN_D3:
		ret = DSS_RDMA1;
		break;
	case DSS_RCHN_VG0:
		ret = DSS_RDMA2;
		break;
	case DSS_RCHN_G0:
		ret = DSS_RDMA4;
		break;

	case DSS_RCHN_VG1: //FIXME:
		ret = DSS_RDMA0;
		break;
	case DSS_RCHN_G1:
		ret = DSS_RDMA1;
		break;
	case DSS_RCHN_D0:
		ret = DSS_RDMA2;
		break;
	case DSS_RCHN_D1:
		ret = DSS_RDMA3;
		break;

	default:
		HISI_FB_ERR("not support this chn_idx(%d)!\n", chn_idx);
		break;
	}

	return ret;
}

static int hisi_get_rbrg_bpp(int format, int stretched_line_num)
{
	int bpp = 0;
	int rot_factor = 1;

	switch (format) {
	case DMA_PIXEL_FORMAT_ARGB_8888:
	case DMA_PIXEL_FORMAT_XRGB_8888:
		bpp = 8;
		break;

	case DMA_PIXEL_FORMAT_RGB_565:
	case DMA_PIXEL_FORMAT_ARGB_4444:
	case DMA_PIXEL_FORMAT_XRGB_4444:
	case DMA_PIXEL_FORMAT_ARGB_5551:
	case DMA_PIXEL_FORMAT_XRGB_5551:
	case DMA_PIXEL_FORMAT_YUV_422_I:
	case DMA_PIXEL_FORMAT_YVU_422_I:
	case DMA_PIXEL_FORMAT_AYUV_4444_I:
		bpp = 4;
		break;

	case DMA_PIXEL_FORMAT_YUV_422_SP_HP:
		bpp = 2 * rot_factor;
		break;

	case DMA_PIXEL_FORMAT_YUV_420_SP_HP:
		bpp = 2 * rot_factor;
		break;
	/*case DMA_PIXEL_FORMAT_YUV_420_SP_UV:
		bpp = (stretched_line_num > 0) ? 2 * rot_factor : 2;
		break;*/

	case DMA_PIXEL_FORMAT_YUV_422_P_HP:
		bpp = 2 * rot_factor;
		break;
	/*case DMA_PIXEL_FORMAT_YUV_422_P_HP_U:
	case DMA_PIXEL_FORMAT_YUV_422_P_HP_V:
		bpp = 1 * rot_factor;
		break;*/

	case DMA_PIXEL_FORMAT_YUV_420_P_HP:
		bpp = 2 * rot_factor;
		break;
	/*case DMA_PIXEL_FORMAT_YUV_420_P_U:
	case DMA_PIXEL_FORMAT_YUV_420_P_V:
		bpp = (stretched_line_num > 0) ? 1 * rot_factor : 1;
		break;*/

	default:
		bpp = -1;
		HISI_FB_ERR("not support the format: %d\n", format);
		break;
	}

	return bpp;
}

static int hisi_get_rbrg_mid(int chn_idx, int ovl_idx)
{
	int ret = 0;

	if ((ovl_idx == DSS_OVL0) || (ovl_idx == DSS_OVL1)) {
		ret = chn_idx;
	} else if ((ovl_idx == DSS_OVL2) || (ovl_idx == DSS_OVL3)) {
		ret = chn_idx + 8;
	} else {
		; //do nothing
	}

	return ret;
}

static void hisi_dss_rdma_bridge_init(char __iomem *rdma_bridge_base,
	dss_rbridge_t *s_rdma_bridge)
{
	BUG_ON(s_rdma_bridge == NULL);
	BUG_ON(rdma_bridge_base == NULL);

	memset(s_rdma_bridge, 0, sizeof(dss_rbridge_t));

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
	char __iomem *rdma_bridge_base, dss_rbridge_t *s_rdma_bridge)
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

int hisi_dss_rdma_bridge_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, dss_rect_t *aligned_rect)
{
	dss_rbridge_t *bridge = NULL;
	dss_layer_t *layer = NULL;

	int i = 0;
	int k = 0;
	int dma_idx = 0;
	int dma_idx_tmp = 0;
	int dma_fomat = 0;

	int dma_axi_idx[RDMA_NUM] = {0};
	int dma_brg_idx[RDMA_NUM] = {0};
	int dma_bpp[RDMA_NUM] = {0};
	int dma_vscl[RDMA_NUM] = {0};
	int dma_hscl[RDMA_NUM] = {0};
	int dma_mid[RDMA_NUM] = {0};
	int dma_osd[RDMA_NUM] = {0};

	uint32_t sum_bw[DSS_RBRG_IDX_MAX][AXI_CHN_MAX];
	uint32_t sum_osd[DSS_RBRG_IDX_MAX][AXI_CHN_MAX];
	uint32_t sum_dma_used[DSS_RBRG_IDX_MAX][AXI_CHN_MAX];

	uint32_t stretched_line_num = 0;
	bool need_dump = false;

	int req_min_cont = 0;
	int req_max_cont = 0;
	uint32_t rbrg_outstanding_dep = 0;
	uint32_t rbrg_rdma_ctl = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	for (k = 0; k < RDMA_NUM; k++) {
		dma_axi_idx[k] = -1;
		dma_brg_idx[k] = -1;
		dma_bpp[k] = 0;
		dma_vscl[k] = RDMA_OSD_MULTIPLE;
		dma_hscl[k] = RDMA_OSD_MULTIPLE;
		dma_mid[k] = 0;
		dma_osd[k] = 0;
	}

	for (i = 0; i < pov_req->layer_nums; i++) {
		layer = &(pov_req->layer_infos[i]);

		if (layer->need_cap & (CAP_PURE_COLOR |CAP_BASE))
			continue;

		stretched_line_num = isNeedRdmaStretchBlt(hisifd, layer);

		dma_idx = hisi_chnIdx2brgDmaIdx(layer->chn_idx);
		if ((dma_idx < 0) || (dma_idx >= RDMA_NUM)) {
			HISI_FB_ERR("chn_idx(%d) hisi_chnIdx2dmaIdx failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		dma_fomat = hisi_pixel_format_hal2dma(layer->img.format);
		if (dma_fomat < 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_pixel_format_hal2dma failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		dma_axi_idx[dma_idx] = hisi_chnIdx2axiPort(layer->chn_idx, pov_req->ovl_idx);
		if (dma_axi_idx[dma_idx] < 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_chnIdx2axiPort failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		dma_brg_idx[dma_idx] = hisi_chnIdx2brgIdx(layer->chn_idx);
		if (dma_brg_idx[dma_idx] < 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_chnIdx2brgIdx failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		dma_bpp[dma_idx] = hisi_get_rbrg_bpp(dma_fomat, stretched_line_num);
		if (dma_bpp[dma_idx] <= 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_get_rbrg_bpp failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		if (aligned_rect->h > layer->dst_rect.h)
			dma_vscl[dma_idx] = RDMA_OSD_MULTIPLE * aligned_rect->h / layer->dst_rect.h;

		if (aligned_rect->w > hisifd->panel_info.xres)
			dma_hscl[dma_idx] = RDMA_OSD_MULTIPLE * aligned_rect->w / hisifd->panel_info.xres;

		if ((dma_vscl[dma_idx] <= 0) || (dma_hscl[dma_idx] <= 0)) {
			HISI_FB_ERR("layer_idx%d, dma_vscl=%d, dma_hscl=%d, "
				"src_rect(%d,%d,%d,%d), dst_rect(%d,%d,%d,%d)!\n",
				layer->layer_idx, dma_vscl[dma_idx], dma_hscl[dma_idx],
				layer->src_rect.x, layer->src_rect.y, layer->src_rect.w, layer->src_rect.h,
				layer->dst_rect.x, layer->dst_rect.y, layer->dst_rect.w, layer->dst_rect.h);

			dma_vscl[dma_idx] = RDMA_OSD_MULTIPLE;
			dma_hscl[dma_idx] = RDMA_OSD_MULTIPLE;
		}

		dma_mid[dma_idx] = hisi_get_rbrg_mid(layer->chn_idx, pov_req->ovl_idx);
		if (dma_mid[dma_idx] < 0) {
			HISI_FB_ERR("chn_idx(%d) hisi_get_rbrg_mid failed!\n", layer->chn_idx);
			return -EINVAL;
		}

		sum_dma_used[dma_brg_idx[dma_idx]][dma_axi_idx[dma_idx]]++;
		sum_bw[dma_brg_idx[dma_idx]][dma_axi_idx[dma_idx]] +=
			(dma_bpp[dma_idx] * dma_vscl[dma_idx] * dma_hscl[dma_idx]);

		if ((dma_idx == 0) && (isYUVSemiPlanar(layer->img.format) ||
			isYUVPlanar(layer->img.format))) {
			dma_idx_tmp = dma_idx + 1;
			dma_axi_idx[dma_idx_tmp] = dma_axi_idx[dma_idx];

			dma_bpp[dma_idx_tmp] = hisi_get_rbrg_bpp(dma_fomat + 1, stretched_line_num);
			if (dma_bpp[dma_idx_tmp] <= 0) {
				HISI_FB_ERR("chn_idx(%d) hisi_get_rbrg_bpp failed!\n", layer->chn_idx);
				return -EINVAL;
			}

			dma_vscl[dma_idx_tmp] = dma_vscl[dma_idx];
			dma_hscl[dma_idx_tmp] = dma_hscl[dma_idx];

			dma_mid[dma_idx_tmp] = hisi_get_rbrg_mid(layer->chn_idx, pov_req->ovl_idx);
			if (dma_mid[dma_idx_tmp] < 0) {
				HISI_FB_ERR("chn_idx(%d) hisi_get_rbrg_mid failed!\n", layer->chn_idx);
				return -EINVAL;
			}

			sum_dma_used[dma_brg_idx[dma_idx_tmp]][dma_axi_idx[dma_idx_tmp]]++;
			sum_bw[dma_brg_idx[dma_idx_tmp]][dma_axi_idx[dma_idx_tmp]] +=
				(dma_bpp[dma_idx_tmp] * dma_vscl[dma_idx_tmp] * dma_hscl[dma_idx_tmp]);
		}

		if ((dma_idx == 0) && isYUVPlanar(layer->img.format)) {
			dma_idx_tmp = dma_idx + 2;
			dma_axi_idx[dma_idx_tmp] = dma_axi_idx[dma_idx];

			dma_bpp[dma_idx_tmp] = hisi_get_rbrg_bpp(dma_fomat + 2, stretched_line_num);
			if (dma_bpp[dma_idx_tmp] <= 0) {
				HISI_FB_ERR("chn_idx(%d) hisi_get_rbrg_bpp failed!\n", layer->chn_idx);
				return -EINVAL;
			}

			dma_vscl[dma_idx_tmp] = dma_vscl[dma_idx];
			dma_hscl[dma_idx_tmp] = dma_hscl[dma_idx];

			dma_mid[dma_idx_tmp] = hisi_get_rbrg_mid(layer->chn_idx, pov_req->ovl_idx);
			if (dma_mid[dma_idx_tmp] < 0) {
				HISI_FB_ERR("chn_idx(%d) hisi_get_rbrg_mid failed!\n", layer->chn_idx);
				return -EINVAL;
			}

			sum_dma_used[dma_brg_idx[dma_idx_tmp]][dma_axi_idx[dma_idx_tmp]]++;
			sum_bw[dma_brg_idx[dma_idx_tmp]][dma_axi_idx[dma_idx_tmp]] +=
				(dma_bpp[dma_idx_tmp] * dma_vscl[dma_idx_tmp] * dma_hscl[dma_idx_tmp]);
		}
	}

	for (k = 0; k < RDMA_NUM; k++) {
		if ((dma_brg_idx[k] < 0) || (dma_axi_idx[k] < 0))
			continue;

		dma_osd[k] = AXI_OSD_BIG_SUM_MAX *
			(dma_bpp[k] * dma_vscl[k] * dma_hscl[k]) / sum_bw[dma_brg_idx[k]][dma_axi_idx[k]];

		if (dma_osd[k] > DMA_OSD_MAX_32)
			need_dump = true;

		sum_osd[dma_brg_idx[k]][dma_axi_idx[k]] += dma_osd[k];

		if (g_debug_ovl_osd) {
			HISI_FB_WARNING("brg%d, axi%d, dma%d, sum_dma_used=%d, "
				"sum_bw=%d, sum_osd=%d!\n",
				dma_brg_idx[k], dma_axi_idx[k], k,
				sum_dma_used[dma_brg_idx[k]][dma_axi_idx[k]],
				sum_bw[dma_brg_idx[k]][dma_axi_idx[k]],
				sum_osd[dma_brg_idx[k]][dma_axi_idx[k]]);

			HISI_FB_WARNING("brg%d, axi%d, dma%d, dma_osd=%d, dma_mid=%d, "
				"(dma_bpp=%d, dma_vscl=%d, dma_hscl=%d)!\n",
				dma_brg_idx[k], dma_axi_idx[k], k, dma_osd[k],  dma_mid[k],
				dma_bpp[k], dma_vscl[k], dma_hscl[k]);
		}
	}

	for (k = 0; k < RDMA_NUM; k++) {
		if (sum_osd[dma_brg_idx[k]][dma_axi_idx[k]] > AXI_OSD_SUM_MAX)
			need_dump = true;
	}

	if (need_dump) {
		for (k = 0; k < RDMA_NUM; k++) {
			HISI_FB_WARNING("brg%d, axi%d, dma%d, sum_dma_used=%d, "
				"sum_bw=%d, sum_osd=%d!\n",
				dma_brg_idx[k], dma_axi_idx[k], k,
				sum_dma_used[dma_brg_idx[k]][dma_axi_idx[k]],
				sum_bw[dma_brg_idx[k]][dma_axi_idx[k]],
				sum_osd[dma_brg_idx[k]][dma_axi_idx[k]]);

			HISI_FB_WARNING("brg%d, axi%d, dma%d, dma_osd=%d, dma_mid=%d, "
				"(dma_bpp=%d, dma_vscl=%d, dma_hscl=%d)!\n",
				dma_brg_idx[k], dma_axi_idx[k], k, dma_osd[k],  dma_mid[k],
				dma_bpp[k], dma_vscl[k], dma_hscl[k]);

			dma_osd[k]= (AXI_OSD_BIG_SUM_MAX / sum_dma_used[dma_brg_idx[k]][dma_axi_idx[k]]);
		}
	}

	for (k = 0; k < RDMA_NUM; k++) {
		if (dma_axi_idx[k] < 0) {
			rbrg_outstanding_dep = 0x0;
			rbrg_rdma_ctl = 0x0;
		} else {
			req_max_cont = (dma_osd[k] > RDMA_REQ_CONT_MAX) ? RDMA_REQ_CONT_MAX : dma_osd[k];
			req_max_cont -= 1;
			req_min_cont = req_max_cont / 2;

			rbrg_outstanding_dep = (dma_osd[k] | (dma_osd[k] << 8));
			rbrg_rdma_ctl = (req_min_cont | (req_max_cont << 4) | (dma_mid[k] << 8) | (dma_axi_idx[k] << 12));
		}

		bridge = &(hisifd->dss_module.bridge[dma_brg_idx[k]]);
		hisifd->dss_module.bridge_used[dma_brg_idx[k]] = 1;

		bridge->rbrg_outstanding_dep[k] = set_bits32(bridge->rbrg_outstanding_dep[k],
			rbrg_outstanding_dep, 16, 0);
		bridge->rbrg_rdma_ctl[k] = set_bits32(bridge->rbrg_rdma_ctl[k],
			rbrg_rdma_ctl, 13, 0);

		if (g_debug_ovl_osd) {
			HISI_FB_INFO("brg%d, axi%d, rdma%d, dma_osd(%d), dma_mid(%d),"
				"rbrg_outstanding_dep(0x%x), rbrg_rdma_ctl(0x%x).\n",
				dma_brg_idx[k], dma_axi_idx[k], k, dma_osd[k], dma_mid[k],
				rbrg_outstanding_dep, rbrg_rdma_ctl);
		}
	}

	return 0;
}


/*******************************************************************************
** DSS MMU
*/
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
	HISI_FB_ERR("mmu_format=%d, layer->transform=%d, layer->img.is_tile=%d,"
		" rdma_stretch_enable=%d, mmu_tlb_tag_org=0x%x\n",
		mmu_format, layer->transform, layer->img.is_tile,
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
}

int hisi_dss_mmu_config(struct hisi_fb_data_type *hisifd,
	dss_layer_t *layer, bool rdma_stretch_enable)
{
	dss_mmu_dma_t *mmu_dma0 = NULL;
	dss_mmu_dma_t *mmu_dma1 = NULL;
	dss_mmu_dma_t *mmu_dma2 = NULL;
	bool is_yuv_semi_planar = false;
	bool is_yuv_planar = false;
	uint32_t ptba = 0;
	uint32_t ptva = 0;

	int axi_cmd_mid = 0;
	int tlb_pri_thr = 0;
	int tlb_pri_force = 0;

	int tlb_cmd_accu = 0;
	int tlb_rtlb_sel = 0;
	int pref_va_ctl = 0;
	int tlb_flush = 0;
	int tlb_rtlb_en = 0;
	int tlb_en = 0;

	int chn_idx = 0;
	dss_img_t *img = 0;
	uint32_t need_cap = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	img = &(layer->img);
	chn_idx = layer->chn_idx;
	need_cap = layer->need_cap;

	if (img->mmu_enable == 0)
		return 0;

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

	mmu_dma0->mmu_ch_ctrl = set_bits32(mmu_dma0->mmu_ch_ctrl,
		hisi_get_mmu_tlb_tag_val(img->format, 0, img->is_tile, rdma_stretch_enable) |
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
static void hisi_dss_rdma_init(char __iomem *dma_base, dss_rdma_t *s_dma)
{
	BUG_ON(dma_base == NULL);
	BUG_ON(s_dma == NULL);

	s_dma->oft_x0 = inp32(dma_base + DMA_OFT_X0);
	s_dma->oft_y0 = inp32(dma_base + DMA_OFT_Y0);
	s_dma->oft_x1 = inp32(dma_base + DMA_OFT_X1);
	s_dma->oft_y1 = inp32(dma_base + DMA_OFT_Y1);
	s_dma->mask0 = inp32(dma_base + DMA_MASK0);
	s_dma->mask1 = inp32(dma_base + DMA_MASK1);
	s_dma->stretch_size_vrt = inp32(dma_base + DMA_STRETCH_SIZE_VRT);
	s_dma->ctrl = inp32(dma_base + DMA_CTRL);
	s_dma->tile_scram = inp32(dma_base + DMA_TILE_SCRAM);

	s_dma->ch_rd_shadow = inp32(dma_base + CH_RD_SHADOW);
	s_dma->ch_size = inp32(dma_base + CH_SIZE);
	s_dma->ch_enable = inp32(dma_base + CH_ENABLE);
	s_dma->ch_ctl = inp32(dma_base + CH_CTL);

	s_dma->data_addr0 = inp32(dma_base + DMA_DATA_ADDR0);
	s_dma->stride0 = inp32(dma_base + DMA_STRIDE0);
	s_dma->stretch_stride0 = inp32(dma_base + DMA_STRETCH_STRIDE0);
	s_dma->data_num0 = inp32(dma_base + DMA_DATA_NUM0);
}

static void hisi_dss_rdma_u_init(char __iomem *dma_base, dss_rdma_t *s_dma)
{
	BUG_ON(dma_base == NULL);
	BUG_ON(s_dma == NULL);

	s_dma->data_addr1 = inp32(dma_base + DMA_DATA_ADDR1);
	s_dma->stride1 = inp32(dma_base + DMA_STRIDE1);
	s_dma->stretch_stride1 = inp32(dma_base + DMA_STRETCH_STRIDE1);
	s_dma->data_num1 = inp32(dma_base + DMA_DATA_NUM1);
}

static void hisi_dss_rdma_v_init(char __iomem *dma_base, dss_rdma_t *s_dma)
{
	BUG_ON(dma_base == NULL);
	BUG_ON(s_dma == NULL);

	s_dma->data_addr2 = inp32(dma_base + DMA_DATA_ADDR2);
	s_dma->stride2 = inp32(dma_base + DMA_STRIDE2);
	s_dma->stretch_stride2 = inp32(dma_base + DMA_STRETCH_STRIDE2);
	s_dma->data_num2 = inp32(dma_base + DMA_DATA_NUM2);
}

static void hisi_dss_rdma_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *dma_base, dss_rdma_t *s_dma)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(dma_base == NULL);
	BUG_ON(s_dma == NULL);

	hisifd->set_reg(hisifd, dma_base + DMA_OFT_X0, s_dma->oft_x0, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_OFT_Y0, s_dma->oft_y0, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_OFT_X1, s_dma->oft_x1, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_OFT_Y1, s_dma->oft_y1, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_MASK0, s_dma->mask0, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_MASK1, s_dma->mask1, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_STRETCH_SIZE_VRT, s_dma->stretch_size_vrt, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_CTRL, s_dma->ctrl, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_TILE_SCRAM, s_dma->tile_scram, 32, 0);

	hisifd->set_reg(hisifd, dma_base + CH_RD_SHADOW, s_dma->ch_rd_shadow, 32, 0);
	hisifd->set_reg(hisifd, dma_base + CH_SIZE, s_dma->ch_size, 32, 0);
	hisifd->set_reg(hisifd, dma_base + CH_ENABLE, s_dma->ch_enable, 32, 0);
	hisifd->set_reg(hisifd, dma_base + CH_CTL, s_dma->ch_ctl, 32 , 0);

	hisifd->set_reg(hisifd, dma_base + DMA_DATA_ADDR0, s_dma->data_addr0, 32 , 0);
	hisifd->set_reg(hisifd, dma_base + DMA_STRIDE0, s_dma->stride0, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_STRETCH_STRIDE0, s_dma->stretch_stride0, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_DATA_NUM0, s_dma->data_num0, 32, 0);
}

static void hisi_dss_rdma_u_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *dma_base, dss_rdma_t *s_dma)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(dma_base == NULL);
	BUG_ON(s_dma == NULL);

	hisifd->set_reg(hisifd, dma_base + DMA_DATA_ADDR1, s_dma->data_addr1, 32 , 0);
	hisifd->set_reg(hisifd, dma_base + DMA_STRIDE1, s_dma->stride1, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_STRETCH_STRIDE1, s_dma->stretch_stride1, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_DATA_NUM1, s_dma->data_num1, 32, 0);
}

static void hisi_dss_rdma_v_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *dma_base, dss_rdma_t *s_dma)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(dma_base == NULL);
	BUG_ON(s_dma == NULL);

	hisifd->set_reg(hisifd, dma_base + DMA_DATA_ADDR2, s_dma->data_addr2, 32 , 0);
	hisifd->set_reg(hisifd, dma_base + DMA_STRIDE2, s_dma->stride2, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_STRETCH_STRIDE2, s_dma->stretch_stride2, 32, 0);
	hisifd->set_reg(hisifd, dma_base + DMA_DATA_NUM2, s_dma->data_num2, 32, 0);
}

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

int hisi_dss_rdma_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_layer_t *layer, dss_rect_ltrb_t *clip_rect,
	dss_rect_t *out_aligned_rect, bool *rdma_stretch_enable)
{
	dss_rdma_t *dma = NULL;

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
	int tmp = 0;

	int dfc_fmt = 0;
	int dfc_bpp = 0;
	int dfc_aligned = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);
	BUG_ON(pov_req == NULL);

	chn_idx = layer->chn_idx;
	new_src_rect = layer->src_rect;

	stretched_line_num = isNeedRdmaStretchBlt(hisifd, layer);
	*rdma_stretch_enable = (stretched_line_num > 0) ? true : false;
	src_rect_mask_enable = isSrcRectMasked(layer);

	mmu_enable = (layer->img.mmu_enable == 1) ? true : false;
	is_yuv_semi_planar = isYUVSemiPlanar(layer->img.format);
	is_yuv_planar = isYUVPlanar(layer->img.format);

#if 0
	if (is_yuv_semi_planar || is_yuv_planar) {
		if (!IS_EVEN(new_src_rect.h)) {
			HISI_FB_ERR("YUV semi_planar or planar format(%d), src_rect's height(%d) is invalid!\n",
				layer->img.format, new_src_rect.h);
		}
	}
#endif

	dma = &(hisifd->dss_module.dma[chn_idx]);
	hisifd->dss_module.dma_used[chn_idx] = 1;

	rdma_addr = mmu_enable ? layer->img.vir_addr : layer->img.phy_addr;
	if (rdma_addr & (DMA_ADDR_ALIGN - 1)) {
		HISI_FB_ERR("layer%d rdma_addr(0x%x) is not %d bytes aligned.\n",
			layer->layer_idx, rdma_addr, DMA_ADDR_ALIGN);
		return -EINVAL;
	}

	if (layer->img.stride & (DMA_STRIDE_ALIGN - 1)) {
		HISI_FB_ERR("layer%d stride(0x%x) is not %d bytes aligned.\n",
			layer->layer_idx, layer->img.stride, DMA_STRIDE_ALIGN);
		return -EINVAL;
	}

	rdma_format = hisi_pixel_format_hal2dma(layer->img.format);
	if (rdma_format < 0) {
		HISI_FB_ERR("layer format(%d) not support !\n", layer->img.format);
		return -EINVAL;
	}

	rdma_transform = hisi_transform_hal2dma(layer->transform, chn_idx);
	if (rdma_transform < 0) {
		HISI_FB_ERR("layer transform(%d) not support!\n", layer->transform);
		return -EINVAL;
	}

	if (layer->img.is_tile) {
		l2t_interleave_n = hisi_get_rdma_tile_interleave(layer->img.stride);
		if (l2t_interleave_n < MIN_INTERLEAVE) {
			HISI_FB_ERR("tile stride should be 256*2^n, error stride:%d!\n", layer->img.stride);
			return -EINVAL;
		}
	}

	dfc_fmt = hisi_pixel_format_hal2dfc(layer->img.format);
	if (dfc_fmt < 0) {
		HISI_FB_ERR("layer format (%d) not support !\n", layer->img.format);
		return -EINVAL;
	}

	dfc_bpp = hisi_dfc_get_bpp(dfc_fmt);
	if (dfc_bpp <= 0) {
		HISI_FB_ERR("dfc_bpp(%d) not support !\n", dfc_bpp);
		return -EINVAL;
	}

	dfc_aligned = (dfc_bpp <= 2) ? 4 : 2;

	bpp = (is_yuv_semi_planar || is_yuv_planar) ? 1 : layer->img.bpp;
	aligned_pixel = DMA_ALIGN_BYTES / bpp;

	/* handle aligned_rect and clip_rect */
	{
		/* aligned_rect */
		if (is_YUV_P_420(layer->img.format) || is_YUV_P_422(layer->img.format)) {
			aligned_rect.left = ALIGN_DOWN(new_src_rect.x, 2 * aligned_pixel);
			aligned_rect.right = ALIGN_UP(new_src_rect.x + new_src_rect.w, 2 * aligned_pixel) - 1;
		} else {
			aligned_rect.left = ALIGN_DOWN(new_src_rect.x, aligned_pixel);
			aligned_rect.right = ALIGN_UP(new_src_rect.x + new_src_rect.w, aligned_pixel) - 1;
		}

		if (is_YUV_SP_420(layer->img.format) || is_YUV_P_420(layer->img.format)) {
			aligned_rect.top = ALIGN_DOWN(new_src_rect.y, 2);
			aligned_rect.bottom = ALIGN_UP(new_src_rect.y + new_src_rect.h, 2) - 1;
		} else {
			aligned_rect.top = new_src_rect.y;
			aligned_rect.bottom = DSS_HEIGHT(new_src_rect.y + new_src_rect.h);
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
				layer->layer_idx, chn_idx, layer->img.format, layer->transform,
				layer->src_rect.x, layer->src_rect.y, layer->src_rect.w, layer->src_rect.h,
				aligned_rect.left, aligned_rect.right, aligned_rect.top, aligned_rect.bottom,
				clip_rect->left, clip_rect->right, clip_rect->top, clip_rect->bottom,
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
	rdma_stride = layer->img.stride;
	rdma_data_num = (rdma_oft_x1 - rdma_oft_x0 + 1) * (rdma_oft_y1- rdma_oft_y0 + 1);

	if (src_rect_mask_enable) {
		rdma_mask_x0 = ALIGN_UP(layer->src_rect_mask.x, aligned_pixel) / aligned_pixel;
		//rdma_mask_x1 = DSS_WIDTH(layer->src_rect_mask.x + layer->src_rect_mask.w) / aligned_pixel -1;
		rdma_mask_x1 = (ALIGN_DOWN(layer->src_rect_mask.x + layer->src_rect_mask.w, aligned_pixel) - 1) / aligned_pixel;
		rdma_mask_y0 = layer->src_rect_mask.y;
		rdma_mask_y1 = DSS_HEIGHT(layer->src_rect_mask.y + layer->src_rect_mask.h);
	}


	if (stretched_line_num > 0) {
		stretched_stride = stretched_line_num * layer->img.stride / DMA_ALIGN_BYTES;
		rdma_data_num = (stretch_size_vrt + 1) * (rdma_oft_x1 - rdma_oft_x0 + 1);
	} else {
		stretch_size_vrt = layer->src_rect.h;
		stretched_line_num = 0x0;
		stretched_stride = 0x0;
	}

	dma->oft_x0 = set_bits32(dma->oft_x0, rdma_oft_x0, 16, 0);
	dma->oft_y0 = set_bits32(dma->oft_y0, rdma_oft_y0, 16, 0);
	dma->oft_x1 = set_bits32(dma->oft_x1, rdma_oft_x1, 16, 0);
	dma->oft_y1 = set_bits32(dma->oft_y1, rdma_oft_y1, 16, 0);
	dma->mask0 = set_bits32(dma->mask0,
		(rdma_mask_y0 | (rdma_mask_x0 << 16)), 32, 0);
	dma->mask1 = set_bits32(dma->mask1,
		(rdma_mask_y1 | (rdma_mask_x1 << 16)), 32, 0);
	dma->stretch_size_vrt = set_bits32(dma->stretch_size_vrt,
		(stretch_size_vrt | (stretched_line_num << 13)), 19, 0);
	dma->ctrl = set_bits32(dma->ctrl, (layer->img.is_tile ? 0x1 : 0x0), 1, 1);
	dma->ctrl = set_bits32(dma->ctrl, rdma_format, 5, 3);
	dma->ctrl = set_bits32(dma->ctrl, (mmu_enable ? 0x1 : 0x0), 1, 8);
	//dma->ctrl = set_bits32(dma->ctrl, rdma_transform, 3, 9);
	dma->ctrl = set_bits32(dma->ctrl, ((stretched_line_num > 0) ? 0x1 : 0x0), 1, 12);
	dma->ctrl = set_bits32(dma->ctrl, (src_rect_mask_enable ? 0x1 : 0x0), 1, 17);
	dma->tile_scram = set_bits32(dma->tile_scram, (layer->img.is_tile ? 0x1 : 0x0), 1, 0);

	dma->ch_enable = set_bits32(dma->ch_enable, 1, 1, 0);

	dma->data_addr0 = set_bits32(dma->data_addr0, rdma_addr / DMA_ALIGN_BYTES, 32, 0);
	dma->stride0 = set_bits32(dma->stride0,
		((rdma_stride / DMA_ALIGN_BYTES) | (l2t_interleave_n << 16)), 20, 0);
	dma->stretch_stride0 = set_bits32(dma->stretch_stride0, stretched_stride, 19, 0);
	dma->data_num0 = set_bits32(dma->data_num0, rdma_data_num, 30, 0);


	if (is_yuv_semi_planar || is_yuv_planar) {
		if (is_YUV_P_420(layer->img.format) || is_YUV_P_422(layer->img.format)) {
			rdma_oft_x0 /= 2;
			rdma_oft_x1 = (rdma_oft_x1 + 1) / 2 - 1;
		}

		if (is_YUV_SP_420(layer->img.format) || is_YUV_P_420(layer->img.format)) {
			rdma_oft_y0 /= 2;
			rdma_oft_y1 = (rdma_oft_y1 + 1) / 2 - 1;

			stretched_line_num /= 2;
		}

		rdma_addr = hisi_calculate_display_addr(mmu_enable, layer, &aligned_rect, DSS_ADDR_PLANE1);
		rdma_stride = layer->img.uStride;
		rdma_data_num = (rdma_oft_x1 - rdma_oft_x0 + 1) * (rdma_oft_y1- rdma_oft_y0 + 1) * 2;

		if (*rdma_stretch_enable) {
			stretch_size_vrt = stretch_size_vrt;
			stretched_stride = stretched_line_num * layer->img.uStride / DMA_ALIGN_BYTES;

			rdma_data_num = (stretch_size_vrt + 1) * (rdma_oft_x1 - rdma_oft_x0 + 1) * 2;
		} else {
			stretch_size_vrt = 0;
			stretched_line_num = 0;
			stretched_stride = 0;
		}

		dma->data_addr1 = set_bits32(dma->data_addr1, rdma_addr / DMA_ALIGN_BYTES, 32, 0);
		dma->stride1 = set_bits32(dma->stride1,
			((rdma_stride / DMA_ALIGN_BYTES) | (l2t_interleave_n << 16)), 20, 0);
		dma->stretch_stride1 = set_bits32(dma->stretch_stride1, stretched_stride, 19, 0);
		dma->data_num1 = set_bits32(dma->data_num1, rdma_data_num, 30, 0);

		if (is_yuv_planar) {
			rdma_addr = hisi_calculate_display_addr(mmu_enable, layer, &aligned_rect, DSS_ADDR_PLANE2);
			rdma_stride = layer->img.vStride;

			dma->data_addr2 = set_bits32(dma->data_addr2, rdma_addr / DMA_ALIGN_BYTES, 32, 0);
			dma->stride2 = set_bits32(dma->stride2,
				((rdma_stride / DMA_ALIGN_BYTES) | (l2t_interleave_n << 16)), 20, 0);
			dma->stretch_stride2 = set_bits32(dma->stretch_stride1, stretched_stride, 19, 0);
			dma->data_num2 = set_bits32(dma->data_num1, rdma_data_num, 30, 0);
		}
	}

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
	s_dfc->clip_ctl_hrz = inp32(dfc_base + DFC_DISP_SIZE);
	s_dfc->clip_ctl_vrz = inp32(dfc_base + DFC_DISP_SIZE);
	s_dfc->ctl_clip_en = inp32(dfc_base + DFC_DISP_SIZE);
	s_dfc->icg_module = inp32(dfc_base + DFC_DISP_SIZE);
	s_dfc->dither_enable = inp32(dfc_base + DFC_DISP_SIZE);
	s_dfc->padding_ctl = inp32(dfc_base + DFC_DISP_SIZE);
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

	dfc_fmt = hisi_pixel_format_hal2dfc(layer->img.format);
	if (dfc_fmt < 0) {
		HISI_FB_ERR("layer format (%d) not support !\n", layer->img.format);
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
		HISI_FB_ERR("SIZE_HRT=%d mismatch!bpp=%d\n", size_hrz, layer->img.bpp);

		HISI_FB_ERR("layer_idx%d, format=%d, transform=%d, "
			"original_src_rect(%d,%d,%d,%d), rdma_out_rect(%d,%d,%d,%d), dst_rect(%d,%d,%d,%d)!\n",
			layer->layer_idx, layer->img.format, layer->transform,
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
	dfc->disp_fmt = set_bits32(dfc->disp_fmt,
		((dfc_fmt << 1) | (hisi_uv_swap(layer->img.format) << 6) | (hisi_rb_swap(layer->img.format) << 7)), 8, 0);

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

static void hisi_dss_scl_init(char __iomem *scl_base, dss_scl_t *s_scl)
{
	BUG_ON(scl_base == NULL);
	BUG_ON(s_scl == NULL);

	memset(s_scl, 0, sizeof(dss_scl_t));

	s_scl->en_hscl_str = inp32(scl_base + SCF_EN_HSCL_STR);
	s_scl->en_vscl_str = inp32(scl_base + SCF_EN_VSCL_STR);
	s_scl->h_v_order = inp32(scl_base + SCF_H_V_ORDER);
	s_scl->input_width_height = inp32(scl_base + SCF_INPUT_WIDTH_HEIGHT);
	s_scl->output_width_height = inp32(scl_base + SCF_OUTPUT_WIDTH_HEIGHT);
	s_scl->en_hscl = inp32(scl_base + SCF_EN_HSCL);
	s_scl->en_vscl = inp32(scl_base + SCF_EN_VSCL);
	s_scl->acc_hscl = inp32(scl_base + SCF_ACC_HSCL);
	s_scl->inc_hscl = inp32(scl_base + SCF_INC_HSCL);
	s_scl->inc_vscl = inp32(scl_base + SCF_INC_VSCL);
}

static void hisi_dss_scl_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *scl_base, dss_scl_t *s_scl)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(scl_base == NULL);
	BUG_ON(s_scl == NULL);

	hisifd->set_reg(hisifd, scl_base + SCF_EN_HSCL_STR, s_scl->en_hscl_str, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_EN_VSCL_STR, s_scl->en_vscl_str, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_H_V_ORDER, s_scl->h_v_order, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_INPUT_WIDTH_HEIGHT, s_scl->input_width_height, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_OUTPUT_WIDTH_HEIGHT, s_scl->output_width_height, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_EN_HSCL, s_scl->en_hscl, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_EN_VSCL, s_scl->en_vscl, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_ACC_HSCL, s_scl->acc_hscl, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_INC_HSCL, s_scl->inc_hscl, 32, 0);
	hisifd->set_reg(hisifd, scl_base + SCF_INC_VSCL, s_scl->inc_vscl, 32, 0);
}

int hisi_dss_scl_write_coefs(struct hisi_fb_data_type *hisifd,
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

int hisi_dss_scl_load_filter_coef(struct hisi_fb_data_type *hisifd, char __iomem *scl_base)
{
	char __iomem *h0_y_addr = NULL;
	char __iomem *y_addr = NULL;
	char __iomem *uv_addr = NULL;
	int ret = 0;

	BUG_ON(scl_base == NULL);

	h0_y_addr = scl_base + DSS_SCF_H0_Y_COEF_OFFSET;
	y_addr = scl_base + DSS_SCF_Y_COEF_OFFSET;
	uv_addr = scl_base + DSS_SCF_UV_COEF_OFFSET;

	ret = hisi_dss_scl_write_coefs(hisifd, h0_y_addr, (const int **)H0_Y_COEF, PHASE_NUM, TAP6);
	if (ret < 0) {
		HISI_FB_ERR("Error to write H0_Y_COEF coefficients.\n");
	}

	ret = hisi_dss_scl_write_coefs(hisifd, y_addr, (const int **)Y_COEF, PHASE_NUM, TAP5);
	if (ret < 0) {
		HISI_FB_ERR("Error to write Y_COEF coefficients.\n");
	}

	ret = hisi_dss_scl_write_coefs(hisifd, uv_addr, (const int **)UV_COEF, PHASE_NUM, TAP4);
	if (ret < 0) {
		HISI_FB_ERR("Error to write UV_COEF coefficients.\n");
	}

	return ret;
}

int hisi_dss_scl_coef_load(struct hisi_fb_data_type *hisifd)
{
	int i = 0;
	uint32_t module_base = 0;
	char __iomem *temp_base = NULL;

	BUG_ON(hisifd == NULL);

	for (i = 0; i < DSS_CHN_MAX; i++) {
		module_base = g_dss_module_base[i][MODULE_SCL];
		if (module_base != 0) {
			temp_base = hisifd->dss_base + module_base;
			hisi_dss_scl_load_filter_coef(hisifd, temp_base);
		}
	}

	return 0;
}

int hisi_dss_scl_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer,
	dss_rect_t *aligned_rect, bool rdma_stretch_enable)
{
	dss_scl_t *scl = NULL;
	dss_rect_t src_rect;
	dss_rect_t dst_rect;
	uint32_t need_cap = 0;
	int chn_idx = 0;
	uint32_t transform = 0;

	bool en_hscl = false;
	bool en_vscl = false;
	uint32_t h_ratio = 0;
	uint32_t v_ratio = 0;
	uint32_t h_v_order = 0;
	uint32_t acc_hscl = 0;
	uint32_t acc_vscl = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	need_cap = layer->need_cap;
	chn_idx = layer->chn_idx;
	transform = layer->transform;
	if (aligned_rect)
		src_rect = *aligned_rect;
	else
		src_rect = layer->src_rect;
	dst_rect = layer->dst_rect;

	do {
		if (src_rect.w == dst_rect.w)
			break;

		en_hscl = true;

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
	h_v_order = (src_rect.w > dst_rect.w) ? 0 : 1;

	scl = &(hisifd->dss_module.scl[chn_idx]);
	hisifd->dss_module.scl_used[chn_idx] = 1;

	//if (DSS_WIDTH(src_rect.w) * 2 >= DSS_WIDTH(dst_rect.w)) {
	if (h_ratio >= 2 * SCF_CONST_FACTOR) {
		scl->en_hscl_str = set_bits32(scl->en_hscl_str, 0x1, 1, 0);
	} else {
		scl->en_hscl_str = set_bits32(scl->en_hscl_str, 0x0, 1, 0);
	}

	//if (DSS_HEIGHT(src_rect.h) * 2 >= DSS_HEIGHT(dst_rect.h)) {
	if (v_ratio >= 2 * SCF_CONST_FACTOR) {
		scl->en_vscl_str = set_bits32(scl->en_vscl_str, 0x1, 1, 0);
	} else {
		scl->en_vscl_str = set_bits32(scl->en_vscl_str, 0x0, 1, 0);
	}

	scl->h_v_order = set_bits32(scl->h_v_order, h_v_order, 1, 0);
	scl->input_width_height = set_bits32(scl->input_width_height,
		DSS_HEIGHT(src_rect.h), 13, 0);
	scl->input_width_height = set_bits32(scl->input_width_height,
		DSS_WIDTH(src_rect.w), 13, 16);
	scl->output_width_height = set_bits32(scl->output_width_height,
		DSS_HEIGHT(dst_rect.h), 13, 0);
	scl->output_width_height = set_bits32(scl->output_width_height,
		DSS_WIDTH(dst_rect.w), 13, 16);
	scl->en_hscl = set_bits32(scl->en_hscl, (en_hscl ? 0x1 : 0x0), 1, 0);
	scl->en_vscl = set_bits32(scl->en_vscl, (en_vscl ? 0x1 : 0x0), 1, 0);
	scl->acc_hscl = set_bits32(scl->acc_hscl, acc_hscl, 31, 0);
	scl->inc_hscl = set_bits32(scl->inc_hscl, h_ratio, 24, 0);
	scl->inc_vscl = set_bits32(scl->inc_vscl, v_ratio, 24, 0);

	return 0;
}


/*******************************************************************************
** DSS CSC
*/
#define CSC_ROW	(3)
#define CSC_COL	(5)

/*
** Rec.601 for Computer
** [ p00 p01 p02 cscidc2 cscodc2 ]
** [ p10 p11 p12 cscidc1 cscodc1 ]
** [ p20 p21 p22 cscidc0 cscodc0 ]
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

	s_csc->idc = inp32(csc_base + CSC_IDC);
	s_csc->odc = inp32(csc_base + CSC_ODC);
	s_csc->p0 = inp32(csc_base + CSC_P0);
	s_csc->p1 = inp32(csc_base + CSC_P1);
	s_csc->p2 = inp32(csc_base + CSC_P2);
	s_csc->p3 = inp32(csc_base + CSC_P3);
	s_csc->p4 = inp32(csc_base + CSC_P4);
	s_csc->icg_module = inp32(csc_base + CSC_ICG_MODULE);
}

static void hisi_dss_csc_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *csc_base, dss_csc_t *s_csc)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(csc_base == NULL);
	BUG_ON(s_csc == NULL);

	hisifd->set_reg(hisifd, csc_base + CSC_IDC, s_csc->idc, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_ODC, s_csc->odc, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_P0, s_csc->p0, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_P1, s_csc->p1, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_P2, s_csc->p2, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_P3, s_csc->p3, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_P4, s_csc->p4, 32, 0);
	hisifd->set_reg(hisifd, csc_base + CSC_ICG_MODULE, s_csc->icg_module, 32, 0);
}

int hisi_dss_csc_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	dss_csc_t *csc = NULL;
	int chn_idx = 0;
	uint32_t format = 0;
	uint32_t csc_mode = 0;
	int (*csc_coe_yuv2rgb)[CSC_COL];
	int (*csc_coe_rgb2yuv)[CSC_COL];

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	chn_idx = layer->chn_idx;
	format = layer->img.format;
	csc_mode = layer->img.csc_mode;

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

	csc = &(hisifd->dss_module.csc[chn_idx]);
	hisifd->dss_module.csc_used[chn_idx] = 1;

	csc->idc = set_bits32(csc->idc, 0x1, 1, 27);
	csc->idc = set_bits32(csc->idc,
		(csc_coe_yuv2rgb[2][3] |
		(csc_coe_yuv2rgb[1][3] << 9) |
		(csc_coe_yuv2rgb[0][3] << 18)), 27, 0);

	csc->odc = set_bits32(csc->odc,
		(csc_coe_yuv2rgb[2][4] |
		(csc_coe_yuv2rgb[1][4] << 9) |
		(csc_coe_yuv2rgb[0][4] << 18)), 27, 0);

	csc->p0 = set_bits32(csc->p0, csc_coe_yuv2rgb[0][0], 11, 0);
	csc->p0 = set_bits32(csc->p0, csc_coe_yuv2rgb[0][1], 11, 16);

	csc->p1 = set_bits32(csc->p1, csc_coe_yuv2rgb[0][2], 11, 0);
	csc->p1 = set_bits32(csc->p1, csc_coe_yuv2rgb[1][0], 11, 16);

	csc->p2 = set_bits32(csc->p2, csc_coe_yuv2rgb[1][1], 11, 0);
	csc->p2 = set_bits32(csc->p2, csc_coe_yuv2rgb[1][2], 11, 16);

	csc->p3 = set_bits32(csc->p3, csc_coe_yuv2rgb[2][0], 11, 0);
	csc->p3 = set_bits32(csc->p3, csc_coe_yuv2rgb[2][1], 11, 16);

	csc->p4 = set_bits32(csc->p4, csc_coe_yuv2rgb[2][2], 11, 0);

	csc->icg_module = set_bits32(csc->icg_module, 0x1, 1, 2);

	return 0;
}


/*******************************************************************************
** DSS SHARPNESS
*/
#if 0
static void hisi_dss_sharpness_init(char __iomem *sharpness_base,
	dss_sharpness_t *s_sharpness)
{
	BUG_ON(sharpness_base == NULL);
	BUG_ON(s_sharpness == NULL);
}

static void hisi_dss_sharpness_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *sharpness_base, dss_sharpness_t *s_sharpness)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(sharpness_base == NULL);
	BUG_ON(s_sharpness == NULL);
}

int hisi_dss_sharpness_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	return 0;
}
#endif

/*******************************************************************************
** DSS CE
*/
#if 0
static void hisi_dss_ce_init(char __iomem *ce_base, dss_ce_t *s_ce)
{
	BUG_ON(ce_base == NULL);
	BUG_ON(s_ce == NULL);
}

static void hisi_dss_ce_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *ce_base, dss_ce_t *s_ce)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(ce_base == NULL);
	BUG_ON(s_ce == NULL);
}

int hisi_dss_ce_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);

	return 0;
}
#endif

/*******************************************************************************
** DSS MCTL
*/
static void hisi_dss_mctl_init(char __iomem *mctl_base, dss_mctl_t *s_mctl)
{
	BUG_ON(mctl_base == NULL);
	BUG_ON(s_mctl == NULL);
}

static void hisi_dss_mctl_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *mctl_base, dss_mctl_t *s_mctl, int ovl_idx)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(mctl_base == NULL);
	BUG_ON(s_mctl == NULL);

	hisifd->set_reg(hisifd, mctl_base + MCTL_DMA_OV0_CFG + ovl_idx * 0xC,
			(s_mctl->chn_ovl_cfg[DSS_RCHN_D0] | (s_mctl->chn_ovl_cfg[DSS_RCHN_D1] << 8) |
			(s_mctl->chn_ovl_cfg[DSS_RCHN_D2] << 16) | (s_mctl->chn_ovl_cfg[DSS_RCHN_D3] << 24)), 32, 0);

	hisifd->set_reg(hisifd, mctl_base + MCTL_RGB_OV0_CFG + ovl_idx * 0xC,
			(s_mctl->chn_ovl_cfg[DSS_RCHN_G0] | (s_mctl->chn_ovl_cfg[DSS_RCHN_G1] << 8)), 16, 0);

	hisifd->set_reg(hisifd, mctl_base + MCTL_VIG_OV0_CFG + ovl_idx * 0xC,
		(s_mctl->chn_ovl_cfg[DSS_RCHN_VG0] | (s_mctl->chn_ovl_cfg[DSS_RCHN_VG1] << 8)), 16, 0);

	hisifd->set_reg(hisifd, mctl_base + MCTL_FLUSH, s_mctl->mctl_flush, 32, 0);
}

int hisi_dss_mctl_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, int ovl_idx, uint32_t flush_bits_padding)
{
	dss_mctl_t *mctl = NULL;
	dss_layer_t *layer = NULL;
	int chn_idx = 0;
	bool has_base = false;
	int i = 0;

	BUG_ON(hisifd == NULL);

	mctl = &(hisifd->dss_module.mctl[ovl_idx]);
	hisifd->dss_module.mctl_used[ovl_idx] = 1;

	if (pov_req) {
		for (i = 0; i < pov_req->layer_nums; i++) {
			layer = &(pov_req->layer_infos[i]);
			chn_idx = layer->chn_idx;

			if (layer->need_cap & CAP_BASE) {
				has_base = true;
				continue;
			}

			mctl->chn_ovl_cfg[chn_idx] = has_base ? (layer->layer_idx - 1) : layer->layer_idx;
			mctl->mctl_flush |= g_dss_module_base[chn_idx][MODULE_CHN_FLUSH];
		}
	}

	mctl->mctl_flush |= (BIT_OV0_FLUSH_EN << ovl_idx);
	mctl->mctl_flush |= BIT_CTL_FLUSH_EN;
	mctl->mctl_flush |= flush_bits_padding;

	return 0;
}


/*******************************************************************************
** DSS OVL
*/
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

	has_per_pixel_alpha = hal_format_has_alpha(layer->img.format);

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

	s_ovl->ovl_size = inp32(ovl_base + OVL_SIZE);
	s_ovl->ovl_bg_color = inp32(ovl_base + OVL_BG_COLOR);
	s_ovl->ovl_dst_startpos = inp32(ovl_base + OVL_DST_STARTPOS);
	s_ovl->ovl_dst_endpos = inp32(ovl_base + OVL_DST_ENDPOS);
	s_ovl->ovl_gcfg = inp32(ovl_base + OVL_GCFG);

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

		s_ovl->ovl_layer_pos[i].layer_pspos =
			inp32(ovl_base + OVL_LAYER0_PSPOS + i * 0x8);
		s_ovl->ovl_layer_pos[i].layer_pepos =
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

	hisifd->set_reg(hisifd, ovl_base + OVL_SIZE, s_ovl->ovl_size, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OVL_BG_COLOR, s_ovl->ovl_bg_color, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OVL_DST_STARTPOS, s_ovl->ovl_dst_startpos, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OVL_DST_ENDPOS, s_ovl->ovl_dst_endpos, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OVL_GCFG, s_ovl->ovl_gcfg, 32, 0);

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
			s_ovl->ovl_layer_pos[i].layer_pspos, 32, 0);
		hisifd->set_reg(hisifd, ovl_base + OVL_LAYER0_PEPOS + i * 0x8,
			s_ovl->ovl_layer_pos[i].layer_pepos, 32, 0);
	}
}

int hisi_dss_ovl_base_config(struct hisi_fb_data_type *hisifd,
	int ovl_idx)
{
	dss_ovl_t *ovl = NULL;
	int img_width = 0;
	int img_height = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON((ovl_idx < DSS_OVL0) || (ovl_idx >= DSS_OVL_IDX_MAX));

	ovl = &(hisifd->dss_module.ov[ovl_idx]);
	hisifd->dss_module.ov_used[ovl_idx] = 1;

	img_width = hisifd->panel_info.xres;
	img_height = hisifd->panel_info.yres;

	ovl->ovl_size = set_bits32(ovl->ovl_size, DSS_WIDTH(img_width), 15, 0);
	ovl->ovl_size = set_bits32(ovl->ovl_size, DSS_HEIGHT(img_height), 15, 16);
	ovl->ovl_bg_color= set_bits32(ovl->ovl_bg_color, 0xFF000000, 32, 0);
	ovl->ovl_dst_startpos = set_bits32(ovl->ovl_dst_startpos, 0x0, 32, 0);
	ovl->ovl_dst_endpos = set_bits32(ovl->ovl_dst_endpos, DSS_WIDTH(img_width), 15, 0);
	ovl->ovl_dst_endpos = set_bits32(ovl->ovl_dst_endpos, DSS_HEIGHT(img_height), 15, 16);
	ovl->ovl_gcfg = set_bits32(ovl->ovl_gcfg, 0x1, 1, 0);
	ovl->ovl_gcfg = set_bits32(ovl->ovl_gcfg, 0x1, 1, 16);

	return 0;
}

int hisi_dss_ovl_layer_config(struct hisi_fb_data_type *hisifd,
	dss_layer_t *layer, int ovl_idx)
{
	dss_ovl_t *ovl = NULL;
	dss_mctl_t *mctl = NULL;
	int chn_idx = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(layer == NULL);
	BUG_ON((ovl_idx < DSS_OVL0) || (ovl_idx >= DSS_OVL_IDX_MAX));

	chn_idx = layer->chn_idx;

	ovl = &(hisifd->dss_module.ov[ovl_idx]);
	hisifd->dss_module.ov_used[ovl_idx] = 1;

	mctl = &(hisifd->dss_module.mctl[ovl_idx]);
	hisifd->dss_module.mctl_used[ovl_idx] = 1;

	if (layer->need_cap & CAP_BASE) {
		if ((layer->layer_idx == 0) &&
			(layer->dst_rect.x == 0) &&
			(layer->dst_rect.y == 0)  &&
			(layer->dst_rect.w == hisifd->panel_info.xres) &&
			(layer->dst_rect.h == hisifd->panel_info.yres)) {

			ovl->ovl_bg_color = set_bits32(ovl->ovl_bg_color, layer->color, 32, 0);
			ovl->ovl_gcfg = set_bits32(ovl->ovl_gcfg, 0x1, 1, 16);
		} else {
			HISI_FB_ERR("layer%d not a base layer!", layer->layer_idx);
		}

		return 0;
	}

	ovl->ovl_layer[layer->layer_idx].layer_pos = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pos,
		layer->dst_rect.x, 15, 0);
	ovl->ovl_layer[layer->layer_idx].layer_pos = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pos,
		layer->dst_rect.y, 15, 16);

	ovl->ovl_layer[layer->layer_idx].layer_size = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_size,
		DSS_WIDTH(layer->dst_rect.x + layer->dst_rect.w), 15, 0);
	ovl->ovl_layer[layer->layer_idx].layer_size = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_size,
		DSS_HEIGHT(layer->dst_rect.y + layer->dst_rect.h), 15, 16);

	ovl->ovl_layer[layer->layer_idx].layer_alpha = set_bits32(ovl->ovl_layer[layer->layer_idx].layer_alpha,
		get_ovl_blending_alpha_val(layer), 32, 0);

	if (layer->need_cap & CAP_PURE_COLOR) {
		ovl->ovl_layer[layer->layer_idx].layer_pattern =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pattern, layer->color, 32, 0);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, 0x1, 1, 0);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, 0x1, 1, 8);
	} else {
		ovl->ovl_layer[layer->layer_idx].layer_pattern =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_pattern, 0x0, 32, 0);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, 0x1, 1, 0);
		ovl->ovl_layer[layer->layer_idx].layer_cfg =
			set_bits32(ovl->ovl_layer[layer->layer_idx].layer_cfg, 0x0, 1, 8);
	}

	ovl->ovl_layer_pos[layer->layer_idx].layer_pspos =
		set_bits32(ovl->ovl_layer_pos[layer->layer_idx].layer_pspos, layer->dst_rect.x, 15, 0);
	ovl->ovl_layer_pos[layer->layer_idx].layer_pspos =
		set_bits32(ovl->ovl_layer_pos[layer->layer_idx].layer_pspos, layer->dst_rect.y, 15, 16);
	ovl->ovl_layer_pos[layer->layer_idx].layer_pepos =
		set_bits32(ovl->ovl_layer_pos[layer->layer_idx].layer_pepos,
		DSS_WIDTH(layer->dst_rect.x + layer->dst_rect.w), 15, 0);
	ovl->ovl_layer_pos[layer->layer_idx].layer_pepos =
		set_bits32(ovl->ovl_layer_pos[layer->layer_idx].layer_pepos,
		DSS_HEIGHT(layer->dst_rect.y + layer->dst_rect.h), 15, 16);

	return 0;
}

int hisi_dss_ovl_optimized(struct hisi_fb_data_type *hisifd, int ovl_idx)
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
		g_dss_module_ovl_base[ovl_idx][MODULE_OVL_BASE];

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
				HISI_FB_INFO("ovl img%d: layer_cfg_val(0x%x), layer_info_alpha(0x%x), "
					"layer_info_srccolor(0x%x), layer_pattern(0x%x).\n",
					i, layer_cfg_val, layer_info_alpha_val, layer_info_srccolor_val, layer_pattern);
			}
		}
	}

	return 0;
}


/*******************************************************************************
** DSS GLOBAL
*/
int hisi_dss_module_init(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	memcpy(&(hisifd->dss_module), &(hisifd->dss_module_default), sizeof(dss_module_reg_t));

	return 0;
}

int hisi_dss_module_default(struct hisi_fb_data_type *hisifd)
{
	dss_module_reg_t *dss_module = NULL;
	uint32_t module_base = 0;
	char __iomem *dss_base = NULL;
	int i = 0;

	BUG_ON(hisifd == NULL);
	dss_base = hisifd->dss_base;
	BUG_ON(dss_base == NULL);

	dss_module = &(hisifd->dss_module_default);
	memset(dss_module, 0, sizeof(dss_module_reg_t));

	for (i = 0; i < DSS_RBRG_IDX_MAX; i++) {
		module_base = g_dss_module_brg_base[i];
		if (module_base != 0) {
			dss_module->bridge_base[i] = dss_base + module_base;
			hisi_dss_rdma_bridge_init(dss_module->bridge_base[i],
				&(dss_module->bridge[i]));
		}
	}

	for (i = 0; i < DSS_OVL_IDX_MAX; i++) {
		module_base = g_dss_module_ovl_base[i][MODULE_OVL_BASE];
		if (module_base != 0) {
			dss_module->ov_base[i] = dss_base + module_base;
			hisi_dss_ovl_init(dss_module->ov_base[i], &(dss_module->ov[i]));
		}

		module_base = g_dss_module_ovl_base[i][MODULE_MCTL_BASE];
		if (module_base != 0) {
			dss_module->mctl_base[i] = dss_base + module_base;
			hisi_dss_mctl_init(dss_module->mctl_base[i], &(dss_module->mctl[i]));
		}
	}

	for (i = DSS_RCHN_D0; i < DSS_CHN_MAX; i++) {
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

		module_base = g_dss_module_base[i][MODULE_DMA];
		if (module_base != 0) {
			dss_module->dma_base[i] = dss_base + module_base;
			hisi_dss_rdma_init(dss_module->dma_base[i], &(dss_module->dma[i]));
			if ((i == DSS_RCHN_VG0) || (i == DSS_RCHN_VG1)) {
				hisi_dss_rdma_u_init(dss_module->dma_base[i], &(dss_module->dma[i]));
			}

			if (i == DSS_RCHN_VG1) {
				hisi_dss_rdma_v_init(dss_module->dma_base[i], &(dss_module->dma[i]));
			}
		}

		module_base = g_dss_module_base[i][MODULE_DFC];
		if (module_base != 0) {
			dss_module->dfc_base[i] = dss_base + module_base;
			hisi_dss_dfc_init(dss_module->dfc_base[i], &(dss_module->dfc[i]));
		}

		module_base = g_dss_module_base[i][MODULE_SCL];
		if (module_base != 0) {
			dss_module->scl_base[i] = dss_base + module_base;
			hisi_dss_scl_init(dss_module->scl_base[i], &(dss_module->scl[i]));
		}

		module_base = g_dss_module_base[i][MODULE_CSC];
		if (module_base != 0) {
			dss_module->csc_base[i] = dss_base + module_base;
			hisi_dss_csc_init(dss_module->csc_base[i], &(dss_module->csc[i]));
		}
	}

	return 0;
}

int hisi_dss_module_config(struct hisi_fb_data_type *hisifd)
{
	dss_module_reg_t *dss_module = NULL;
	int i = 0;

	BUG_ON(hisifd == NULL);

	dss_module = &(hisifd->dss_module);

	for (i = 0; i < DSS_RBRG_IDX_MAX; i++) {
		if (dss_module->bridge_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->bridge_base[i]);
			hisi_dss_rdma_bridge_set_reg(hisifd,
				dss_module->bridge_base[i], &(dss_module->bridge[i]));
		}
	}

	for (i = 0; i < DSS_OVL_IDX_MAX; i++) {
		if (dss_module->ov_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->ov_base[i]);
			hisi_dss_ovl_set_reg(hisifd, dss_module->ov_base[i], &(dss_module->ov[i]));
		}

		if (dss_module->mctl_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->mctl_base[i]);
			hisi_dss_mctl_set_reg(hisifd, dss_module->mctl_base[i], &(dss_module->mctl[i]), i);
		}
	}

	for (i = DSS_RCHN_D0; i < DSS_CHN_MAX; i++) {
		if (dss_module->dma_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->dma_base[i]);
			hisi_dss_rdma_set_reg(hisifd, dss_module->dma_base[i], &(dss_module->dma[i]));
			if ((i == DSS_RCHN_VG0) || (i == DSS_RCHN_VG1)) {
				hisi_dss_rdma_u_set_reg(hisifd, dss_module->dma_base[i], &(dss_module->dma[i]));
			}

			if (i == DSS_RCHN_VG1) {
				hisi_dss_rdma_v_set_reg(hisifd, dss_module->dma_base[i], &(dss_module->dma[i]));
			}

		}

		if (dss_module->dfc_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->dfc_base[i]);
			hisi_dss_dfc_set_reg(hisifd, dss_module->dfc_base[i], &(dss_module->dfc[i]));
		}

		if (dss_module->scl_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->scl_base[i]);
			hisi_dss_scl_set_reg(hisifd, dss_module->scl_base[i], &(dss_module->scl[i]));
		}

		if (dss_module->csc_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->csc_base[i]);
			hisi_dss_csc_set_reg(hisifd, dss_module->csc_base[i], &(dss_module->csc[i]));
		}

		if (dss_module->mmu_dma0_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->mmu_dma0_base[i]);
			hisi_dss_mmu_dma_set_reg(hisifd, dss_module->mmu_dma0_base[i],
				&(dss_module->mmu_dma0[i]));
		}

		if (dss_module->mmu_dma1_used[i] == 1) {
			HISI_FB_ASSERT(dss_module->mmu_dma1_base[i]);
			hisi_dss_mmu_dma_set_reg(hisifd, dss_module->mmu_dma1_base[i],
				&(dss_module->mmu_dma1[i]));
		}
	}

	return 0;
}


/*******************************************************************************
**
*/
int hisi_overlay_on(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;
	int ovl_idx = 0;

	BUG_ON(hisifd == NULL);

	hisifd->vactive0_start_flag = 0;

	hisifd->dirty_region_updt.x = 0;
	hisifd->dirty_region_updt.y = 0;
	hisifd->dirty_region_updt.w = hisifd->panel_info.xres;
	hisifd->dirty_region_updt.h = hisifd->panel_info.yres;
	hisifd->dirty_region_updt_enable = 0;

	if (g_dss_module_resource_initialized == 0) {
		hisi_dss_module_default(hisifd);
		g_dss_module_resource_initialized = 1;
		hisifd->dss_module_resource_initialized = true;
	}

	if (!hisifd->dss_module_resource_initialized) {
		if (hisifd->index > 0) {
			memcpy(&(hisifd->dss_module_default),
				&(hisifd_list[0]->dss_module_default), sizeof(dss_module_reg_t));
		}
		hisifd->dss_module_resource_initialized = true;
	}

	if ((hisifd->index == PRIMARY_PANEL_IDX) ||
		(hisifd->index == EXTERNAL_PANEL_IDX)) {
		if (hisifd->index == PRIMARY_PANEL_IDX) {
			ovl_idx = DSS_OVL0;
		} else {
			ovl_idx = DSS_OVL1;
		}

		ret = hisi_dss_module_init(hisifd);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, failed to hisi_dss_module_init! ret = %d\n", hisifd->index, ret);
			return ret;
		}

		hisi_dss_handle_prev_ovl_req(hisifd, &(hisifd->ov_req));

		ret = hisi_dss_ovl_base_config(hisifd, ovl_idx);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, faile to hisi_dss_ovl_base_config! ret=%d\n", hisifd->index, ret);
			return ret;
		}

		ret = hisi_dss_mctl_config(hisifd, NULL, ovl_idx, hisifd->dss_flush_bits);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, faile to hisi_dss_mctl_config! ret=%d\n", hisifd->index, ret);
			return ret;
		}

		ret = hisi_dss_module_config(hisifd);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, failed to hisi_dss_module_config! ret = %d\n", hisifd->index, ret);
			return ret;
		}

		enable_ldi(hisifd);

		single_frame_update(hisifd);

		hisi_dss_scl_coef_load(hisifd);

		memset(&hisifd->ov_req, 0, sizeof(dss_overlay_t));
	}

	return 0;
}

int hisi_overlay_off(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;
	uint32_t ovl_idx = 0;

	BUG_ON(hisifd == NULL);

	if ((hisifd->index == PRIMARY_PANEL_IDX) ||
		(hisifd->index == EXTERNAL_PANEL_IDX)) {
		hisifb_activate_vsync(hisifd);

		if (hisifd->index == PRIMARY_PANEL_IDX) {
			ovl_idx = DSS_OVL0;
		} else {
			ovl_idx = DSS_OVL1;
		}

		ret = hisi_dss_module_init(hisifd);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, failed to hisi_dss_module_init! ret = %d\n", hisifd->index, ret);
			return ret;
		}

		hisi_dss_handle_prev_ovl_req(hisifd, &(hisifd->ov_req));

		ret = hisi_dss_ovl_base_config(hisifd, ovl_idx);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, faile to hisi_dss_ovl_base_config! ret=%d\n", hisifd->index, ret);
			return ret;
		}

		ret = hisi_dss_mctl_config(hisifd, NULL, ovl_idx, 0);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, faile to hisi_dss_mctl_config! ret=%d\n", hisifd->index, ret);
			return ret;
		}

		ret = hisi_dss_module_config(hisifd);
		if (ret != 0) {
			HISI_FB_ERR("fb%d, failed to hisi_dss_module_config! ret = %d\n", hisifd->index, ret);
			return ret;
		}

		single_frame_update(hisifd);

		if (!hisi_dss_check_reg_reload_status(hisifd)) {
			mdelay(17);
		}

		hisifb_deactivate_vsync(hisifd);
	}

	hisifd->dss_flush_bits = 0;

	return 0;
}

bool hisi_dss_check_reg_reload_status(struct hisi_fb_data_type *hisifd)
{
	bool is_ready = false;
#if 0
	uint32_t tmp = 0;
	uint32_t offset = 0;
	uint32_t try_times = 0;

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
#endif

	return is_ready;
}

static int hisi_overlay_test(struct hisi_fb_data_type *hisifd, void __user *argp)
{
	int ret = 0;
	dss_overlay_test_t ov_test;
	dss_reg_t *pdss_reg = NULL;
	int i = 0;

	BUG_ON(hisifd == NULL);

	pdss_reg = (dss_reg_t *)kmalloc(sizeof(dss_reg_t) * HISI_FB_DSS_REG_TEST_NUM,
		GFP_ATOMIC);
	if (!pdss_reg) {
		HISI_FB_ERR("pdss_reg failed to alloc!\n");
		return -ENOMEM;
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

	case HISIFB_OV_TEST:
		{
			ret = hisi_overlay_test(hisifd, argp);
			if (ret != 0) {
				HISI_FB_ERR("fb%d hisi_overlay_test failed!\n", hisifd->index);
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
	BUG_ON(hisifd == NULL);
	BUG_ON(hisifd->dss_base == NULL);

	hisifd->pan_display_fnc = hisi_overlay_pan_display;
	hisifd->ov_ioctl_handler = hisi_overlay_ioctl_handler;

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		hisifd->set_reg = hisifb_set_reg;
		hisifd->ov_online_play = hisi_ov_online_play;
		hisifd->ov_offline_play = NULL;
		hisifd->ov_wb_isr_handler = NULL;
		hisifd->ov_vactive0_start_isr_handler = hisi_vactive0_start_isr_handler;
		hisifd->ov_optimized = hisi_dss_ovl_optimized;
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		hisifd->set_reg = hisifb_set_reg;
		hisifd->ov_online_play = hisi_ov_online_play;
		hisifd->ov_offline_play = NULL;
		hisifd->ov_wb_isr_handler = NULL;
		hisifd->ov_vactive0_start_isr_handler = hisi_vactive0_start_isr_handler;
		hisifd->ov_optimized = hisi_dss_ovl_optimized;
	} else if (hisifd->index == AUXILIARY_PANEL_IDX) {
		;
	} else {
		HISI_FB_ERR("fb%d not support this device!\n", hisifd->index);
		return -EINVAL;
	}

	hisifd->dss_module_resource_initialized = false;

	hisifd->vactive0_start_flag = 0;
	init_waitqueue_head(&hisifd->vactive0_start_wq);

	hisifd->dss_flush_bits = 0;

	memset(&hisifd->ov_req, 0, sizeof(dss_overlay_t));
	memset(&hisifd->block_ov, 0, sizeof(dss_overlay_t));
	memset(&hisifd->dss_module, 0, sizeof(dss_module_reg_t));
	memset(&hisifd->dss_module_default, 0, sizeof(dss_module_reg_t));

	hisifd->dirty_region_updt.x = 0;
	hisifd->dirty_region_updt.y = 0;
	hisifd->dirty_region_updt.w = hisifd->panel_info.xres;
	hisifd->dirty_region_updt.h = hisifd->panel_info.yres;
	hisifd->dirty_region_updt_enable = 0;

	return 0;
}

int hisi_overlay_deinit(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	return 0;
}

void hisi_vactive0_start_isr_handler(struct hisi_fb_data_type *hisifd)
{
	BUG_ON(hisifd == NULL);

	if (is_mipi_cmd_panel(hisifd))
		hisifd->vactive0_start_flag = 1;
	else
		hisifd->vactive0_start_flag++;
	wake_up_interruptible_all(&hisifd->vactive0_start_wq);
}

int hisi_vactive0_start_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req)
{
	int ret = 0;
	uint32_t prev_vactive0_start = 0;
	uint32_t isr_s1 = 0;
	uint32_t isr_s1_mask = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);

	if (is_mipi_cmd_panel(hisifd)) {
		if (hisifd->vactive0_start_flag == 0) {
			ret = wait_event_interruptible_timeout(hisifd->vactive0_start_wq,
				hisifd->vactive0_start_flag, 1* HZ);
			if (ret <= 0) {
				HISI_FB_ERR("fb%d, wait_for vactive0_start_flag timeout!ret=%d, vactive0_start_flag=%d\n",
					hisifd->index, ret, hisifd->vactive0_start_flag);
				ret = -ETIMEDOUT;
			} else {
				ret = 0;
			}
		}
		hisifd->vactive0_start_flag = 0;
	} else {
		prev_vactive0_start = hisifd->vactive0_start_flag;
		ret = wait_event_interruptible_timeout(hisifd->vactive0_start_wq,
			(prev_vactive0_start != hisifd->vactive0_start_flag), 1 * HZ);
		if (ret <= 0) {
			HISI_FB_ERR("fb%d, wait_for vactive0_start_flag timeout!ret=%d, "
				"prev_vactive0_start=%d, vactive0_start_flag=%d\n",
				hisifd->index, ret, prev_vactive0_start, hisifd->vactive0_start_flag);
			ret = -ETIMEDOUT;
		} else {
			ret = 0;
		}
	}

	if (ret == -ETIMEDOUT) {
		if (pov_req->ovl_idx == DSS_OVL0) {
			isr_s1_mask = inp32(hisifd->dss_base + GLB_CPU_PDP_INT_MSK);
			isr_s1 = inp32(hisifd->dss_base + GLB_CPU_PDP_INTS);
		} else if (pov_req->ovl_idx == DSS_OVL1) {
			isr_s1_mask = inp32(hisifd->dss_base + GLB_CPU_SDP_INT_MSK);
			isr_s1 = inp32(hisifd->dss_base + GLB_CPU_SDP_INTS);
		} else {
			; //do nothing;
		}

		HISI_FB_ERR("fb%d, isr_s1_mask=0x%x, isr_s1=0x%x.\n",
			hisifd->index, isr_s1_mask, isr_s1);
	}

	return ret;
}

void hisi_dss_dirty_region_updt_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, uint32_t *flush_bits_padding)
{
	struct hisi_fb_panel_data *pdata = NULL;
	char __iomem *ovl_base = NULL;
	char __iomem *ldi_base = NULL;
	struct dss_rect dirty = {0};
	uint32_t h_porch_pading = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	pdata = dev_get_platdata(&hisifd->pdev->dev);
	BUG_ON(pdata == NULL);

	if (!hisifd->panel_info.dirty_region_updt_support ||
		!hisifd->dirty_region_updt_enable)
		return ;

	if (!g_enable_dirty_region_updt
		|| hisifd->sbl_enable) {
		dirty.x = 0;
		dirty.y = 0;
		dirty.w = hisifd->panel_info.xres;
		dirty.h = hisifd->panel_info.yres;
	} else {
		dirty = hisifd->ov_req.dirty_rect_updt;
		if (hisifd->panel_info.xres >= dirty.w) {
			h_porch_pading = hisifd->panel_info.xres - dirty.w;
		}
	}

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		*flush_bits_padding = (BIT_DBUF0_FLUSH_EN | BIT_DPP0_FLUSH_EN);

		ldi_base = hisifd->dss_base + DSS_LDI0_OFFSET;
		ovl_base = hisifd->dss_base +
			g_dss_module_ovl_base[pov_req->ovl_idx][MODULE_OVL_BASE];
	} else {
		HISI_FB_ERR("fb%d, not support!", hisifd->index);
		return ;
	}

	set_reg(ovl_base + OVL_SIZE,
		(DSS_WIDTH(dirty.w) | DSS_HEIGHT(dirty.h) << 16), 31, 0);

	outp32(hisifd->dss_base + LDI_DPI0_HRZ_CTRL0,
		(hisifd->panel_info.ldi.h_front_porch) | ((hisifd->panel_info.ldi.h_back_porch + h_porch_pading) << 16));

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

	if (g_debug_dirty_region_updt) {
		HISI_FB_INFO("dirty_region(%d,%d, %d,%d). h_porch_pading=%d\n",
			dirty.x, dirty.y, dirty.w, dirty.h, h_porch_pading);
	}
}
