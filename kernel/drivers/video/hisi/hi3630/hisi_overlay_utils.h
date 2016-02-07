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
extern uint32_t g_dss_module_ovl_base[DSS_OVL_MAX][MODULE_OVL_MAX];
extern uint32_t g_dss_module_eng_base[DSS_ENG_MAX][MODULE_ENG_MAX];


void dumpDssOverlay(dss_overlay_t *ov_data);

int hisi_get_hal_format(struct fb_info *info);
int hisi_overlay_init(struct hisi_fb_data_type *hisifd);
int hisi_overlay_deinit(struct hisi_fb_data_type *hisifd);
int hisi_overlay_set_fastboot(struct hisi_fb_data_type *hisifd);
int hisi_overlay_on(struct hisi_fb_data_type *hisifd, bool ov_base_enable);
int hisi_overlay_off(struct hisi_fb_data_type *hisifd);
bool hisi_dss_check_reg_reload_status(struct hisi_fb_data_type *hisifd);
void hisi_writeback_isr_handler(struct hisi_fb_data_type *hisifd);
void hisi_vactive0_start_isr_handler(struct hisi_fb_data_type *hisifd);
int hisi_vactive0_start_config(struct hisi_fb_data_type *hisifd);
void hisi_dss_dirty_region_updt_config(struct hisi_fb_data_type *hisifd);

int hisi_ov_compose_handler(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, 	dss_layer_t *layer,
	dss_rect_ltrb_t *clip_rect,
	dss_rect_t *aligned_rect,
	bool *rdma_stretch_enable);
int hisi_ov_compose_handler_wb(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, 	dss_wb_layer_t *wb_layer,
	dss_rect_t *aligned_rect);

int hisi_overlay_pan_display(struct hisi_fb_data_type *hisifd);
int hisi_ov_online_play(struct hisi_fb_data_type *hisifd, void __user *argp);
int hisi_ov_offline_play(struct hisi_fb_data_type *hisifd, void __user *argp);
int hisi_ov_online_writeback(struct hisi_fb_data_type *hisifd, void __user *argp);
int hisi_online_writeback_control(struct hisi_fb_data_type *hisifd, void __user *argp);
int hisi_overlay_ioctl_handler(struct hisi_fb_data_type *hisifd,
	uint32_t cmd, void __user *argp);

void hisi_dss_unflow_handler(struct hisi_fb_data_type *hisifd, bool unmask);

void hisi_dss_config_ok_begin(struct hisi_fb_data_type *hisifd);
void hisi_dss_config_ok_end(struct hisi_fb_data_type *hisifd);

void hisi_dfs_wbe_enable(struct hisi_fb_data_type *hisifd);
void hisi_dfs_wbe_disable(struct hisi_fb_data_type *hisifd);

void hisi_adp_offline_start_enable(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req);
void hisi_adp_offline_start_disable(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req);

int hisi_dss_handle_prev_ovl_req(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req);
int hisi_dss_handle_cur_ovl_req(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req);
int hisi_dss_handle_prev_ovl_req_wb(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req);
int hisi_dss_handle_cur_ovl_req_wb(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req);
int hisi_dss_rptb_handler(struct hisi_fb_data_type *hisifd, bool is_wb, int rptb_info_idx);

int hisi_dss_check_layer_par(struct hisi_fb_data_type *hisifd, dss_layer_t *layer);
dss_rect_t hisi_dss_rdma_out_rect(struct hisi_fb_data_type *hisifd, dss_layer_t *layer);

int hisi_dss_rdma_bridge_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req);
int hisi_dss_mmu_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_layer_t *layer, dss_wb_layer_t *wb_layer, bool rdma_stretch_enable);
int hisi_dss_rdma_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_layer_t *layer, dss_rect_ltrb_t *clip_rect, dss_rect_t *aligned_rect,
	bool *rdma_stretch_enable);
int hisi_dss_fbdc_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_layer_t *layer, dss_rect_t aligned_rect);
int hisi_dss_rdfc_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer,
	dss_rect_t *aligned_rect, dss_rect_ltrb_t clip_rect);
int hisi_dss_scf_coef_load(struct hisi_fb_data_type *hisifd);
int hisi_dss_scf_coef_unload(struct hisi_fb_data_type *hisifd);
int hisi_dss_scf_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer,
	dss_wb_layer_t *wb_layer, dss_rect_t *aligned_rect, bool rdma_stretch_enable);
int hisi_dss_scp_config(struct hisi_fb_data_type *hisifd, dss_layer_t *layer);
int hisi_dss_csc_config(struct hisi_fb_data_type *hisifd,
	dss_layer_t *layer, dss_wb_layer_t *wb_layer);

int hisi_dss_ovl_base_config(struct hisi_fb_data_type *hisifd,
	dss_rect_t *wb_block_rect, uint32_t ovl_flags);
int hisi_dss_ovl_layer_config(struct hisi_fb_data_type *hisifd,
	dss_layer_t *layer, dss_rect_t *wb_block_rect);
int hisi_dss_ovl_optimized(struct hisi_fb_data_type *hisifd);
int hisi_dss_check_pure_layer(struct hisi_fb_data_type *hisifd, void __user *argp);

int hisi_dss_mux_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, dss_layer_t *layer);

int hisi_dss_module_default(struct hisi_fb_data_type *hisifd);
int hisi_dss_offline_module_default(struct hisi_fb_data_type *hisifd);

int hisi_dss_module_init(struct hisi_fb_data_type *hisifd);
int hisi_dss_module_config(struct hisi_fb_data_type *hisifd);

int hisi_dss_wbe_mux_config(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, dss_wb_layer_t *layer);
int hisi_dss_wdfc_config(struct hisi_fb_data_type *hisifd, dss_wb_layer_t *layer,
	dss_rect_t *aligned_rect, dss_rect_t *block_rect);
int hisi_dss_fbc_config(struct hisi_fb_data_type *hisifd,
	dss_wb_layer_t *layer, dss_rect_t aligned_rect);
int hisi_dss_wdma_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req,
	dss_wb_layer_t *layer, dss_rect_t aligned_rect, dss_rect_t *block_rect);
int hisi_dss_wdma_bridge_config(struct hisi_fb_data_type *hisifd, dss_overlay_t *pov_req);

void* hisi_dss_rptb_init(void);
void hisi_dss_rptb_deinit(void *handle);
uint32_t hisi_dss_rptb_alloc(void *handle, uint32_t size);
void hisi_dss_rptb_free(void *handle, uint32_t addr, uint32_t size);
int hisi_get_rot_index(int chn_idx);
int hisi_get_scf_index(int chn_idx);

bool isYUVPackage(int format);
bool isYUVSemiPlanar(int format);
bool isYUVPlanar(int format);
bool isYUV(int format);


#endif /* HISI_OVERLAY_UTILS_H */
