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

	if (layer->need_cap & (CAP_BASE | CAP_DIM)) {
		ret = hisi_dss_ovl_layer_config(hisifd, layer, NULL);
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

	if (layer->src.mmu_enable) {
		ret = hisi_dss_mmu_config(hisifd, pov_req, layer, NULL, *rdma_stretch_enable);
		if (ret != 0) {
			HISI_FB_ERR("hisi_dss_mmu_config failed! ret = %d\n", ret);
			goto err_return;
		}
	}

	ret = hisi_dss_fbdc_config(hisifd, pov_req, layer, *aligned_rect);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_fbdc_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_rdfc_config(hisifd, layer, aligned_rect, *clip_rect);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_rdfc_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_scf_config(hisifd, layer, NULL, aligned_rect, *rdma_stretch_enable);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_scf_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_scp_config(hisifd, layer);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_scp_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_csc_config(hisifd, layer, NULL);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_csc_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_mux_config(hisifd, pov_req, layer);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_mux_config failed, ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_ovl_layer_config(hisifd, layer, NULL);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_ovl_config failed, ret = %d\n", ret);
		goto err_return;
	}

	return 0;

err_return:
	return ret;
}

int hisi_ov_compose_handler_wb(struct hisi_fb_data_type *hisifd,
	dss_overlay_t *pov_req, 	dss_wb_layer_t *wb_layer,
	dss_rect_t *aligned_rect)
{
	int ret = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(pov_req == NULL);
	BUG_ON(wb_layer == NULL);
	BUG_ON(aligned_rect == NULL);

	ret = hisi_dss_wbe_mux_config(hisifd, pov_req, wb_layer);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_wbe_mux_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_csc_config(hisifd, NULL, wb_layer);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_csc_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_scf_config(hisifd, NULL, wb_layer, NULL, false);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_wb_scf_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_wdfc_config(hisifd, wb_layer, aligned_rect, NULL);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_wdfc_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_fbc_config(hisifd, wb_layer, *aligned_rect);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_fbc_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_wdma_config(hisifd, pov_req, wb_layer, *aligned_rect, NULL);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_wdma_config failed! ret = %d\n", ret);
		goto err_return;
	}

	if (wb_layer->dst.mmu_enable) {
		ret = hisi_dss_mmu_config(hisifd, pov_req, NULL, wb_layer, false);
		if (ret != 0) {
			HISI_FB_ERR("hisi_dss_mmu_config failed! ret = %d\n", ret);
			goto err_return;
		}
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
	uint8_t ovl_type = 0;

	BUG_ON(hisifd == NULL);
	fbi = hisifd->fbi;
	BUG_ON(fbi == NULL);
	pov_req = &(hisifd->ov_req);
	BUG_ON(pov_req == NULL);

	if (!hisifd->panel_power_on) {
		HISI_FB_DEBUG("fb%d, panel is power off!", hisifd->index);
		return 0;
	}

	hisifb_activate_vsync(hisifd);

	ovl_type = hisifd->ovl_type;
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

	ret = hisi_vactive0_start_config(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_vactive0_start_config failed! ret = %d\n", ret);

		if (is_mipi_cmd_panel(hisifd))
			hisifd->ldi_data_gate_en = 0;

		goto err_return;
	}

	ret = hisi_dss_module_init(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_init failed! ret = %d\n", ret);
		goto err_return;
	}

	hisi_dss_config_ok_begin(hisifd);

	hisi_dss_handle_prev_ovl_req(hisifd, pov_req);

	memset(pov_req, 0, sizeof(dss_overlay_t));
	layer = &(pov_req->layer_infos[0]);
	pov_req->layer_nums = 1;
	if (ovl_type == DSS_OVL_PDP) {
		pov_req->ovl_flags = DSS_SCENE_PDP_ONLINE;
	} else if (ovl_type == DSS_OVL_SDP) {
		pov_req->ovl_flags = DSS_SCENE_SDP_ONLINE;
	} else {
		HISI_FB_ERR("not support this ovl_type(%d)!\n", ovl_type);
	}

	pov_req->dirty_region_updt_info.dirty_rect.x = 0;
	pov_req->dirty_region_updt_info.dirty_rect.y = 0;
	pov_req->dirty_region_updt_info.dirty_rect.w = fbi->var.xres;
	pov_req->dirty_region_updt_info.dirty_rect.h = fbi->var.yres;

	layer->src.format = hal_format;
	layer->src.width = fbi->var.xres;
	layer->src.height = fbi->var.yres;
	layer->src.bpp = fbi->var.bits_per_pixel >> 3;
	layer->src.stride = fbi->fix.line_length;
	layer->src.phy_addr = addr;
	layer->src.vir_addr = addr;
	layer->src.ptba = hisifd->iommu_format.iommu_ptb_base;
	layer->src.ptva = hisifd->iommu_format.iommu_iova_base;
	layer->src.buf_size = fbi->fix.line_length * fbi->var.yres;
	layer->src.is_tile = 0;
#ifdef CONFIG_HISI_FB_HEAP_CARVEOUT_USED
	layer->src.mmu_enable = 0;
#else
	layer->src.mmu_enable = 1;
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
	layer->layer_idx = 0;
	layer->chn_idx = DPE0_CHN3;
	layer->need_cap = CAP_FB_POST;
	layer->flags = DSS_SCENE_PDP_ONLINE;

	if (g_debug_ovl_online_composer) {
		dumpDssOverlay(pov_req);
		HISI_FB_ERR("line_length=%d, bits_per_pixel=%d\n",
			fbi->fix.line_length, fbi->var.bits_per_pixel);
	}

	hisi_dss_handle_cur_ovl_req(hisifd, pov_req);

	ret = hisi_dss_ovl_base_config(hisifd, NULL, pov_req->ovl_flags);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_ovl_init failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_rdma_bridge_config(hisifd, pov_req);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_rdma_bridge_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_ov_compose_handler(hisifd, pov_req, layer,
		&clip_rect, &aligned_rect, &rdma_stretch_enable);
	if (ret != 0) {
		HISI_FB_ERR("hisi_ov_compose_handler failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_module_config(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_config failed! ret = %d\n", ret);
		goto err_return;
	}

	if (hisifd->panel_info.dirty_region_updt_support)
		hisi_dss_dirty_region_updt_config(hisifd);

	hisi_dss_config_ok_end(hisifd);
	single_frame_update(hisifd);
	hisi_dss_unflow_handler(hisifd, true);

	hisifb_deactivate_vsync(hisifd);

	//hisifb_layerbuf_lock(hisifd);

	return 0;

err_return:
	single_frame_update(hisifd);
	hisifb_deactivate_vsync(hisifd);
	return ret;
}

int hisi_ov_online_play(struct hisi_fb_data_type *hisifd, void __user *argp)
{
	dss_overlay_t *pov_req = NULL;
	dss_layer_t *layer = NULL;
	dss_wb_layer_t *wb_layer = NULL;
	dss_rect_ltrb_t clip_rect;
	dss_rect_t aligned_rect;
	bool rdma_stretch_enable = false;
#ifdef CONFIG_BUF_SYNC_USED
	unsigned long flags = 0;
#endif
	int i = 0;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pov_req = &(hisifd->ov_req);
	BUG_ON(pov_req == NULL);

	if (!hisifd->panel_power_on) {
		HISI_FB_DEBUG("fb%d panel is power off!", hisifd->index);
		return 0;
	}

	if (hisifd->index == EXTERNAL_PANEL_IDX && !hisifd->is_hdmi_power_full) {
		HISI_FB_ERR("fb%d hdmi is not power full!", hisifd->index);
		return 0;
	}

	hisifb_activate_vsync(hisifd);

	ret = hisi_vactive0_start_config(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_vactive0_start_config failed! ret = %d\n", ret);

		if (is_mipi_cmd_panel(hisifd)) {
			hisifd->dss_exception.underflow_exception = 1;
			hisifd->ldi_data_gate_en = 0;
		}

		goto err_return;
	}

	ret = hisi_dss_module_init(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_init failed! ret = %d\n", ret);
		goto err_return;
	}

	hisi_dss_config_ok_begin(hisifd);

	hisi_dss_handle_prev_ovl_req(hisifd, pov_req);
	hisi_dss_handle_prev_ovl_req_wb(hisifd, pov_req);
	if (pov_req->wb_enable && !hisifd->ov_wb_enabled) {
		hisi_dss_rptb_handler(hisifd, true, 0);
	}

	ret = copy_from_user(pov_req, argp, sizeof(dss_overlay_t));
	if (ret) {
		HISI_FB_ERR("copy_from_user failed!\n");
		goto err_return;
	}

	if (g_debug_ovl_online_composer)
		dumpDssOverlay(pov_req);

	hisi_dss_handle_cur_ovl_req(hisifd, pov_req);

	ret = hisi_dss_ovl_base_config(hisifd, NULL, pov_req->ovl_flags);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_ovl_init failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_rdma_bridge_config(hisifd, pov_req);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_rdma_bridge_config failed! ret = %d\n", ret);
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

	if (pov_req->wb_enable && hisifd->ov_wb_enabled) {
		wb_layer = &(pov_req->wb_layer_info);

		ret = hisi_ov_compose_handler_wb(hisifd, pov_req, wb_layer, &aligned_rect);
		if (ret != 0) {
			HISI_FB_ERR("hisi_ov_compose_handler_wb failed! ret = %d\n", ret);
			goto err_return;
		}

		ret = hisi_dss_wdma_bridge_config(hisifd, pov_req);
		if (ret != 0) {
			HISI_FB_ERR("hisi_dss_wdma_bridge_config failed! ret = %d\n", ret);
			goto err_return;
		}
	}
	ret = hisi_dss_module_config(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_config failed! ret = %d\n", ret);
		goto err_return;
	}

	if (hisifd->panel_info.dirty_region_updt_support)
		hisi_dss_dirty_region_updt_config(hisifd);

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

	if (pov_req->wb_enable && hisifd->ov_wb_enabled) {
		if (pov_req->wb_layer_info.acquire_fence >= 0) {
			hisifb_buf_sync_wait(pov_req->wb_layer_info.acquire_fence);
		}
		pov_req->wb_layer_info.release_fence = -1;
	}

	if (copy_to_user(argp, pov_req, sizeof(dss_overlay_t))) {
		ret = -EFAULT;
		put_unused_fd(pov_req->release_fence);
		goto err_return;
	}

	spin_lock_irqsave(&hisifd->buf_sync_ctrl.refresh_lock, flags);
	hisi_dss_config_ok_end(hisifd);
	single_frame_update(hisifd);
	hisi_dss_unflow_handler(hisifd, true);

	hisifd->buf_sync_ctrl.refresh++;
	spin_unlock_irqrestore(&hisifd->buf_sync_ctrl.refresh_lock, flags);
#else
	hisi_dss_config_ok_end(hisifd);
	single_frame_update(hisifd);
	hisi_dss_unflow_handler(hisifd, true);
#endif

	hisifb_deactivate_vsync(hisifd);

	hisifb_layerbuf_lock(hisifd);

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		if (!hisifd->fb_mem_free_flag) {
			hisifb_free_fb_buffer(hisifd);
			hisifd->fb_mem_free_flag = true;
		}
	}

	return 0;

err_return:
	single_frame_update(hisifd);
	hisifb_deactivate_vsync(hisifd);
	return ret;
}

int hisi_ov_online_writeback(struct hisi_fb_data_type *hisifd, void __user *argp)
{
	dss_overlay_t *pov_req = NULL;
	dss_wb_layer_t *wb_layer = NULL;
	dss_rect_t aligned_rect;
	int ret = 0;

	BUG_ON(hisifd == NULL);
	pov_req = &(hisifd->ov_wb_req);
	BUG_ON(pov_req == NULL);

	if (!hisifd->panel_power_on) {
		HISI_FB_DEBUG("fb%d, panel is power off!", hisifd->index);
		return 0;
	}

	if (!hisifd->ov_wb_enabled) {
		HISI_FB_ERR("fb%d, wb disabled!", hisifd->index);
		return 0;
	}

	hisifb_activate_vsync(hisifd);

	ret = hisi_dss_module_init(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_init failed! ret = %d\n", ret);
		goto err_return;
	}

	hisi_dss_config_ok_begin(hisifd);

	hisi_dss_handle_prev_ovl_req_wb(hisifd, pov_req);

	ret = copy_from_user(pov_req, argp, sizeof(dss_overlay_t));
	if (ret) {
		HISI_FB_ERR("copy_from_user failed!\n");
		goto err_return;
	}

	wb_layer = &(pov_req->wb_layer_info);

	hisi_dss_handle_cur_ovl_req_wb(hisifd, pov_req);

	ret = hisi_ov_compose_handler_wb(hisifd, pov_req, wb_layer, &aligned_rect);
	if (ret != 0) {
		HISI_FB_ERR("hisi_ov_compose_handler_wb failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_wdma_bridge_config(hisifd, pov_req);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_wdma_bridge_config failed! ret = %d\n", ret);
		goto err_return;
	}

	ret = hisi_dss_module_config(hisifd);
	if (ret != 0) {
		HISI_FB_ERR("hisi_dss_module_config failed! ret = %d\n", ret);
		goto err_return;
	}

	hisifd->ov_wb_done = 0;

	hisi_dss_config_ok_end(hisifd);
	hisi_dss_unflow_handler(hisifd, true);

	ret = wait_event_interruptible_timeout(hisifd->ov_wb_wq,
		(hisifd->ov_wb_done == 1), HZ);
	if (ret <= 0) {
		HISI_FB_ERR("wait_for online write back done timeout, ret=%d!\n", ret);
		ret = -ETIMEDOUT;
		goto err_return;
	} else {
		ret = 0;
	}

	hisifb_deactivate_vsync(hisifd);

	return 0;

err_return:
	hisifb_deactivate_vsync(hisifd);
	return ret;
}

int hisi_online_writeback_control(struct hisi_fb_data_type *hisifd, void __user *argp)
{
	int ret = 0;
	int enable = 0;
	dss_overlay_t *pov_req = NULL;

	BUG_ON(hisifd == NULL);
	pov_req = &(hisifd->ov_wb_req);
	BUG_ON(pov_req == NULL);

	ret = copy_from_user(&enable, argp, sizeof(enable));
	if (ret) {
		HISI_FB_ERR("copy_from_user ioctl failed!\n");
		return ret;
	}

	enable = (enable) ? 1 : 0;
	HISI_FB_INFO("online WB switch, enable:%d, prev:%d.\n",
		enable, hisifd->ov_wb_enabled);

	if (hisifd->ov_wb_enabled == enable)
		return 0;

	hisifd->ov_wb_enabled = enable;

	if (!hisifd->panel_power_on) {
		hisifd->ov_wb_enabled = 0;
		HISI_FB_DEBUG("fb%d, panel is power off!", hisifd->index);
		//return -1;
		return 0;
	}

	hisifb_activate_vsync(hisifd);

	if (enable) {
		hisi_dfs_wbe_enable(hisifd);
	} else {
		hisi_dfs_wbe_disable(hisifd);
	}

	hisifb_deactivate_vsync(hisifd);

	return 0;
}
