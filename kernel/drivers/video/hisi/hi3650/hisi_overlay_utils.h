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
#ifndef HISI_OVERLAY_UTILS_H
#define HISI_OVERLAY_UTILS_H

#include "hisi_fb.h"


/*******************************************************************************
**
*/
extern uint32_t g_dss_module_base[DSS_CHN_MAX][MODULE_CHN_MAX];
extern uint32_t g_dss_module_ovl_base[DSS_OVL_IDX_MAX][MODULE_OVL_MAX];
extern uint32_t g_dss_module_brg_base[DSS_RBRG_IDX_MAX];


void dumpDssOverlay(dss_overlay_t *ov_data);

int hisi_get_hal_format(struct fb_info *info);
int hisi_overlay_init(struct hisi_fb_data_type *hisifd);
int hisi_overlay_deinit(struct hisi_fb_data_type *hisifd);
int hisi_overlay_on(struct hisi_fb_data_type *hisifd);
int hisi_overlay_off(struct hisi_fb_data_type *hisifd);
bool hisi_dss_check_reg_reload_status(struct hisi_fb_data_type *hisifd);
void hisi_vactive0_start_isr_handler(struct hisi_fb_data_type *hisifd);
int hisi_vactive0_start_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req);
void hisi_dss_dirty_region_updt_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, uint32_t *flush_bits_padding);

int hisi_ov_compose_handler(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, 	dss_layer_t *layer,
	dss_rect_ltrb_t *clip_rect,
	dss_rect_t *aligned_rect,
	bool *rdma_stretch_enable);

int hisi_overlay_pan_display(struct hisi_fb_data_type *hisifd);
int hisi_ov_online_play(struct hisi_fb_data_type *hisifd, void __user *argp);
int hisi_ov_offline_play(struct hisi_fb_data_type *hisifd, void __user *argp);
int hisi_overlay_ioctl_handler(struct hisi_fb_data_type *hisifd,
	uint32_t cmd, void __user *argp);

void hisi_dss_unflow_handler(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, bool unmask);

int hisi_dss_handle_prev_ovl_req(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req);

int hisi_dss_check_layer_par(struct hisi_fb_data_type *hisifd, dss_layer_t *layer);

int hisi_dss_rdma_bridge_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, dss_rect_t *aligned_rect);
int hisi_dss_mctl_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, int ovl_idx, uint32_t flush_bits_padding);
int hisi_dss_mmu_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer,
	bool rdma_stretch_enable);
int hisi_dss_rdma_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_layer_t *layer, dss_rect_ltrb_t *clip_rect, dss_rect_t *aligned_rect,
	bool *rdma_stretch_enable);
int hisi_dss_rdfc_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer,
	dss_rect_t *aligned_rect, dss_rect_ltrb_t clip_rect);
int hisi_dss_scl_coef_load(struct hisi_fb_data_type *hisifd);
int hisi_dss_scl_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer,
	dss_rect_t *aligned_rect, bool rdma_stretch_enable);
int hisi_dss_csc_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer);
int hisi_dss_sharpness_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer);
int hisi_dss_ce_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer);

int hisi_dss_ovl_base_config(struct hisi_fb_data_type *hisifd, int ovl_idx);
int hisi_dss_ovl_layer_config(struct hisi_fb_data_type *hisifd,
	dss_layer_t *layer, int ovl_idx);
int hisi_dss_ovl_optimized(struct hisi_fb_data_type *hisifd, int ovl_idx);

int hisi_dss_module_default(struct hisi_fb_data_type *hisifd);
int hisi_dss_module_init(struct hisi_fb_data_type *hisifd);
int hisi_dss_module_config(struct hisi_fb_data_type *hisifd);

bool isYUVPackage(int format);
bool isYUVSemiPlanar(int format);
bool isYUVPlanar(int format);
bool isYUV(int format);


#endif /* HISI_OVERLAY_UTILS_H */
