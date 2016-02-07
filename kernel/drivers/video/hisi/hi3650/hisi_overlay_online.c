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
#include "hisi_dpe_utils.h"


int hisi_ov_compose_handler(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, 	dss_layer_t *layer,
	dss_rect_ltrb_t *clip_rect,
	dss_rect_t *aligned_rect,
	bool *rdma_stretch_enable)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON(layer == NULL);
	BUG_ON(clip_rect == NULL);
	BUG_ON(aligned_rect == NULL);
	BUG_ON(rdma_stretch_enable == NULL);

	ret = hisi_dss_check_layer_par(hisifd, layer);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_check_layer_par failed! ret = %d\n", ret);
		goto err_return;
	}

	if (layer->need_cap & (CAP_BASE | CAP_PURE_COLOR)) {
		ret = hisi_dss_ovl_layer_config(hisifd, layer, pov_req->ovl_idx);
		if (ret != 0) {
			HISI_FB_ERR("hisi_dss_ovl_config failed! need_cap=0x%x, ret=%d\n",
				layer->need_cap, ret);
			return ret;
		}

		return ret;
	}

	ret = hisi_dss_rdma_config(hisifd, pov_req, layer,
		clip_rect, aligned_rect, rdma_stretch_enable);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_rdma_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_mmu_config(hisifd, layer, *rdma_stretch_enable);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_mmu_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_rdfc_config(hisifd, layer, aligned_rect, *clip_rect);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_rdfc_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_scl_config(hisifd, layer, aligned_rect, *rdma_stretch_enable);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_scl_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_csc_config(hisifd, layer);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_csc_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_ovl_layer_config(hisifd, layer, pov_req->ovl_idx);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_ovl_config failed, ret = %d\n", ret);
		goto err_return;
	}

	return 0;

err_return:
	return ret;
}

int hisi_overlay_pan_display(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;
	struct fb_info *fbi = NULL;
	dss_overlay_t *pov_req = NULL;
	dss_layer_t *layer = NULL;
	dss_rect_ltrb_t clip_rect;
	dss_rect_t aligned_rect;
	bool rdma_stretch_enable = false;
	uint32_t offset = 0;
	uint32_t addr = 0;
	int hal_format = 0;

	BUG_ON(hisifd == NULL);
	fbi = hisifd->fbi;
	BUG_ON(fbi == NULL);
	pov_req = &(hisifd->ov_req);
	BUG_ON(pov_req == NULL);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel is power off!", hisifd->index);
		return 0;
	}

	hisifb_activate_vsync(hisifd);

	offset = fbi->var.xoffset * (fbi->var.bits_per_pixel >> 3) +
		fbi->var.yoffset * fbi->fix.line_length;
	addr = fbi->fix.smem_start + offset;
	if (!fbi->fix.smem_start) {
		HISI_FB_ERR("smem_start is null!\n");
		goto err_return;
	}

	if (fbi->fix.smem_len <= 0) {
		HISI_FB_ERR("smem_len(%d) is out of range!\n", fbi->fix.smem_len);
		goto err_return;
	}

	hal_format = hisi_get_hal_format(fbi);
	if (hal_format < 0) {
		HISI_FB_ERR("not support this fb_info's format!\n");
		goto err_return;
	}

	ret = hisi_vactive0_start_config(hisifd, pov_req);
	if (ret != 0) {
		HISI_FB_ERR("hisi_vactive0_start_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_module_init(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_init failed! ret = %d\n", ret);
		goto err_return;
	}

	hisi_dss_handle_prev_ovl_req(hisifd, pov_req);

	memset(pov_req, 0, sizeof(dss_overlay_t));
	layer = &(pov_req->layer_infos[0]);
	pov_req->layer_nums = 1;
	pov_req->ovl_idx = DSS_OVL0;

	layer->img.format = hal_format;
	layer->img.width = fbi->var.xres;
	layer->img.height = fbi->var.yres;
	layer->img.bpp = fbi->var.bits_per_pixel >> 3;
	layer->img.stride = fbi->fix.line_length;
	layer->img.phy_addr = addr;
	layer->img.vir_addr = addr;
	layer->img.ptba = hisifd->iommu_format.iommu_ptb_base;
	layer->img.ptva = hisifd->iommu_format.iommu_iova_base;
	layer->img.is_tile = 0;
#ifdef CONFIG_HISI_FB_HEAP_CARVEOUT_USED
	layer->img.mmu_enable = 0;
#else
	layer->img.mmu_enable = 1;
#endif

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
	layer->layer_idx = 0x0;
	layer->chn_idx = DSS_RCHN_D2;
	layer->need_cap = 0;

	ret = hisi_dss_ovl_base_config(hisifd, pov_req->ovl_idx);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_ovl_init failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_ov_compose_handler(hisifd, pov_req, layer,
		&clip_rect, &aligned_rect, &rdma_stretch_enable);
	if (ret != 0) {
		HISI_FB_ERR("hisi_ov_compose_handler failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_rdma_bridge_config(hisifd, pov_req, &aligned_rect);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_rdma_bridge_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_mctl_config(hisifd, pov_req, pov_req->ovl_idx, 0);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_mctl_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_module_config(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_config failed! ret = %d\n", ret);
		goto err_return;
	}

	single_frame_update(hisifd);
	hisi_dss_unflow_handler(hisifd, pov_req, true);

	hisifb_deactivate_vsync(hisifd);

	return 0;

err_return:
	hisifb_deactivate_vsync(hisifd);
	return ret;
}

int hisi_ov_online_play(struct hisi_fb_data_type *hisifd, void __user *argp)
{
	dss_overlay_t *pov_req = NULL;
	dss_layer_t *layer = NULL;
	dss_rect_ltrb_t clip_rect;
	dss_rect_t aligned_rect;
	bool rdma_stretch_enable = false;
	uint32_t flush_bits_padding = 0;
#ifdef CONFIG_BUF_SYNC_USED
	unsigned long flags = 0;
#endif
	int i = 0;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pov_req = &(hisifd->ov_req);
	BUG_ON(pov_req == NULL);

	if (!hisifd->panel_power_on) {
		HISI_FB_ERR("fb%d, panel is power off!", hisifd->index);
		return 0;
	}

	hisifb_activate_vsync(hisifd);

	ret = hisi_vactive0_start_config(hisifd, pov_req);
	if (ret != 0) {
		HISI_FB_ERR("hisi_vactive0_start_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_module_init(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_init failed! ret = %d\n", ret);
		goto err_return;
	}

	hisi_dss_handle_prev_ovl_req(hisifd, pov_req);

	ret = copy_from_user(pov_req, argp, sizeof(dss_overlay_t));
	if (ret) {
		HISI_FB_ERR("copy_from_user failed!\n");
		goto err_return;
	}

	if (g_debug_ovl_online_composer)
		dumpDssOverlay(pov_req);

	ret = hisi_dss_ovl_base_config(hisifd, pov_req->ovl_idx);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_ovl_init failed! ret = %d\n", ret);
		goto err_return;
	}

	/* Go through all layers */
	for (i = 0; i < pov_req->layer_nums; i++) {
		layer = &(pov_req->layer_infos[i]);
		memset(&clip_rect, 0, sizeof(dss_rect_ltrb_t));
		memset(&aligned_rect, 0, sizeof(dss_rect_ltrb_t));
		rdma_stretch_enable = false;

		ret = hisi_ov_compose_handler(hisifd, pov_req, layer,
			&clip_rect, &aligned_rect, &rdma_stretch_enable);
		if (ret != 0) {
			HISI_FB_ERR("hisi_ov_compose_handler failed! ret = %d\n", ret);
			goto err_return;
		}
	}

	ret = hisi_dss_rdma_bridge_config(hisifd, pov_req, &aligned_rect);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_rdma_bridge_config failed! ret = %d\n", ret);
		goto err_return;
	}

	if (hisifd->panel_info.dirty_region_updt_support)
		hisi_dss_dirty_region_updt_config(hisifd, pov_req, &flush_bits_padding);

	ret = hisi_dss_mctl_config(hisifd, pov_req, pov_req->ovl_idx, flush_bits_padding);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_mctl_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_module_config(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_config failed! ret = %d\n", ret);
		goto err_return;
	}

#ifdef CONFIG_BUF_SYNC_USED
	for (i = 0; i < pov_req->layer_nums; i++) {
		if (pov_req->layer_infos[i].acquire_fence >= 0){
			hisifb_buf_sync_wait(pov_req->layer_infos[i].acquire_fence);
		}
	}

	ret = hisifb_buf_sync_create_fence(hisifd, ++hisifd->buf_sync_ctrl.timeline_max);
	if (ret < 0) {
		HISI_FB_ERR("hisi_create_fence failed! ret = 0x%x\n", ret);
		goto err_return;
	}

	pov_req->release_fence = ret;
	if (copy_to_user((struct dss_overlay_t __user *)argp,
			pov_req, sizeof(dss_overlay_t))) {
		ret = -EFAULT;
		put_unused_fd(pov_req->release_fence);
		goto err_return;
	}

	spin_lock_irqsave(&hisifd->buf_sync_ctrl.refresh_lock, flags);
	single_frame_update(hisifd);
	hisi_dss_unflow_handler(hisifd, pov_req, true);

	hisifd->buf_sync_ctrl.refresh++;
	spin_unlock_irqrestore(&hisifd->buf_sync_ctrl.refresh_lock, flags);
#else
	single_frame_update(hisifd);
	hisi_dss_unflow_handler(hisifd, pov_req, true);
#endif

	hisifb_deactivate_vsync(hisifd);

	return 0;

err_return:
	single_frame_update(hisifd);
	hisifb_deactivate_vsync(hisifd);
	return ret;
}