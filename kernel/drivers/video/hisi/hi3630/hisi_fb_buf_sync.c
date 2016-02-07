/* Copyright (c) 2008-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
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

#include "hisi_fb.h"


#ifdef CONFIG_BUF_SYNC_USED
#define BUF_SYNC_TIMEOUT_MSEC	(10 * MSEC_PER_SEC)
#define BUF_SYNC_FENCE_NAME	"hisi-dss-fence"
#define BUF_SYNC_TIMELINE_NAME	"hisi-dss-timeline"

#define HISI_DSS_LAYERBUF_FREE	"hisi-dss-layerbuf-free"


void hisifb_layerbuf_lock(struct hisi_fb_data_type *hisifd)
{
	struct hisifb_layerbuf *node, *_node_;
	struct list_head lock_list;
	unsigned long flags = 0;
	dss_overlay_t *pov_req = NULL;
	struct ion_handle  *ionhnd = NULL;
	struct iommu_map_format format;
	int i = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(hisifd->ion_client == NULL);
	INIT_LIST_HEAD(&lock_list);
	memset(&format, 0, sizeof(struct iommu_map_format));

	pov_req = &hisifd->ov_req;

	for (i = 0; i < pov_req->layer_nums; i++) {
		if (pov_req->layer_infos[i].src.shared_fd <= 0)
			continue;

		ionhnd = ion_import_dma_buf(hisifd->ion_client, pov_req->layer_infos[i].src.shared_fd);
		if (IS_ERR(ionhnd)) {
			HISI_FB_ERR("ion import dma buf err, ionclient %p, share_fd %d, layer index %d",
				hisifd->ion_client, pov_req->layer_infos[i].src.shared_fd, i);
			continue;
		}

		//HISI_FB_ERR("ion import dma buf success %d.\n", pov_req->layer_infos[i].src.shared_fd);
		node = kzalloc(sizeof(struct hisifb_layerbuf), GFP_KERNEL);
		if (node == NULL) {
			ion_free(hisifd->ion_client, ionhnd);
			HISI_FB_ERR("kzalloc struct hisifb_layerbuf fail\n");
			break;
		}
		node->ion_handle = ionhnd;
		node->timeline = 0;
		node->has_map_iommu = false;

		if (pov_req->layer_infos[i].src.phy_addr == 0) {
			ion_map_iommu(hisifd->ion_client, ionhnd, &format);
			node->has_map_iommu = true;
		}
		list_add_tail(&node->list_node, &lock_list);
	}

	spin_lock_irqsave(&(hisifd->buf_sync_ctrl.layerbuf_spinlock), flags);
	hisifd->buf_sync_ctrl.layerbuf_flushed = true;
	list_for_each_entry_safe(node, _node_, &lock_list, list_node) {
		list_del(&node->list_node);
		list_add_tail(&node->list_node, &(hisifd->buf_sync_ctrl.layerbuf_list));
	}
	spin_unlock_irqrestore(&(hisifd->buf_sync_ctrl.layerbuf_spinlock), flags);
}

static void hisifb_free_layerbuf_work(struct work_struct *work)
{
	struct hisifb_buf_sync *pbuf_sync = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	unsigned long flags;
	struct hisifb_layerbuf *node, *_node_;
	struct list_head free_list;

	pbuf_sync = container_of(work, struct hisifb_buf_sync, free_layerbuf_work);
	BUG_ON(pbuf_sync == NULL);
	hisifd = container_of(pbuf_sync, struct hisi_fb_data_type, buf_sync_ctrl);
	BUG_ON(hisifd == NULL);
	BUG_ON(hisifd->ion_client == NULL);

	INIT_LIST_HEAD(&free_list);

	spin_lock_irqsave(&pbuf_sync->layerbuf_spinlock, flags);
	list_for_each_entry_safe(node, _node_, &pbuf_sync->layerbuf_list, list_node) {
		if (node->timeline >= 2) {
			list_del(&node->list_node);
			list_add_tail(&node->list_node, &free_list);
		}
	}
	spin_unlock_irqrestore(&pbuf_sync->layerbuf_spinlock, flags);

	list_for_each_entry_safe(node, _node_, &free_list, list_node) {
		list_del(&node->list_node);
		//HISI_FB_ERR("free node %p, timeline %d\n", node->ion_handle, node->timeline);
		if (node->has_map_iommu) {
			ion_unmap_iommu(hisifd->ion_client, node->ion_handle);
		}
		ion_free(hisifd->ion_client, node->ion_handle);
		kfree(node);
	}
}

int hisifb_buf_sync_wait(int fence_fd)
{
	int ret = 0;
	struct sync_fence *fence = NULL;

	fence = sync_fence_fdget(fence_fd);
	if (fence == NULL){
		HISI_FB_ERR("sync_fence_fdget failed!\n");
		return -EINVAL;
	}

	ret = sync_fence_wait(fence, BUF_SYNC_TIMEOUT_MSEC);
	if (ret == -ETIME)
		ret = sync_fence_wait(fence, BUF_SYNC_TIMEOUT_MSEC);

	if (ret < 0)
		HISI_FB_WARNING("error waiting on fence: 0x%x\n", ret);

	sync_fence_put(fence);

	return ret;
}


void hisifb_buf_sync_signal(struct hisi_fb_data_type *hisifd)
{
	struct hisifb_layerbuf *node = NULL;
	struct hisifb_layerbuf *_node_ = NULL;
	struct list_head list;
	INIT_LIST_HEAD(&list);

	//HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	spin_lock(&hisifd->buf_sync_ctrl.refresh_lock);
	if (hisifd->buf_sync_ctrl.refresh) {
		sw_sync_timeline_inc(hisifd->buf_sync_ctrl.timeline, hisifd->buf_sync_ctrl.refresh);
		hisifd->buf_sync_ctrl.refresh = 0;
	}
	spin_unlock(&hisifd->buf_sync_ctrl.refresh_lock);

	spin_lock(&(hisifd->buf_sync_ctrl.layerbuf_spinlock));
	list_for_each_entry_safe(node, _node_, &(hisifd->buf_sync_ctrl.layerbuf_list), list_node) {
		if (hisifd->buf_sync_ctrl.layerbuf_flushed) {
			node->timeline++;
		}
	}
	hisifd->buf_sync_ctrl.layerbuf_flushed = false;
	spin_unlock(&(hisifd->buf_sync_ctrl.layerbuf_spinlock));

	queue_work(hisifd->buf_sync_ctrl.free_layerbuf_queue, &(hisifd->buf_sync_ctrl.free_layerbuf_work));

	//HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
}

void hisifb_buf_sync_suspend(struct hisi_fb_data_type *hisifd)
{
	unsigned long flags;

	spin_lock_irqsave(&hisifd->buf_sync_ctrl.refresh_lock,flags);
	sw_sync_timeline_inc(hisifd->buf_sync_ctrl.timeline, hisifd->buf_sync_ctrl.refresh + 1);
	hisifd->buf_sync_ctrl.refresh = 0;
	hisifd->buf_sync_ctrl.timeline_max++;
	spin_unlock_irqrestore(&hisifd->buf_sync_ctrl.refresh_lock,flags);
}

int hisifb_buf_sync_create_fence(struct hisi_fb_data_type *hisifd, unsigned value)
{
	int fd = -1;
	struct sync_fence *fence = NULL;
	struct sync_pt *pt = NULL;

	BUG_ON(hisifd == NULL);

	fd = get_unused_fd();
	if (fd < 0) {
		HISI_FB_ERR("get_unused_fd failed!\n");
		return fd;
	}

	pt = sw_sync_pt_create(hisifd->buf_sync_ctrl.timeline, value);
	if (pt == NULL) {
		return -ENOMEM;
	}

	fence = sync_fence_create(BUF_SYNC_FENCE_NAME, pt);
	if (fence == NULL) {
		sync_pt_free(pt);
		return -ENOMEM;
	}

	sync_fence_install(fence, fd);

	return fd;
}

void hisifb_buf_sync_register(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	spin_lock_init(&hisifd->buf_sync_ctrl.refresh_lock);
	hisifd->buf_sync_ctrl.refresh = 0;
	hisifd->buf_sync_ctrl.timeline_max = 1;
	hisifd->buf_sync_ctrl.timeline =
		sw_sync_timeline_create(BUF_SYNC_TIMELINE_NAME);
	if (hisifd->buf_sync_ctrl.timeline == NULL) {
		HISI_FB_ERR("cannot create time line!");
		return ; /* -ENOMEM */
	}


	spin_lock_init(&(hisifd->buf_sync_ctrl.layerbuf_spinlock));
	INIT_LIST_HEAD(&(hisifd->buf_sync_ctrl.layerbuf_list));
	hisifd->buf_sync_ctrl.layerbuf_flushed = false;

	INIT_WORK(&(hisifd->buf_sync_ctrl.free_layerbuf_work), hisifb_free_layerbuf_work);
	hisifd->buf_sync_ctrl.free_layerbuf_queue = create_singlethread_workqueue(HISI_DSS_LAYERBUF_FREE);
	if (!hisifd->buf_sync_ctrl.free_layerbuf_queue) {
		HISI_FB_ERR("failed to create free_layerbuf_queue!\n");
		return ;
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
}

void hisifb_buf_sync_unregister(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (hisifd->buf_sync_ctrl.timeline) {
		sync_timeline_destroy((struct sync_timeline *)hisifd->buf_sync_ctrl.timeline);
		hisifd->buf_sync_ctrl.timeline = NULL;
	}

	if (hisifd->buf_sync_ctrl.free_layerbuf_queue) {
		destroy_workqueue(hisifd->buf_sync_ctrl.free_layerbuf_queue);
		hisifd->buf_sync_ctrl.free_layerbuf_queue = NULL;
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
}
#else
void hisifb_layerbuf_lock(struct hisi_fb_data_type *hisifd)
{}
int hisifb_buf_sync_wait(int fence_fd)
{ return 0; }
void hisifb_buf_sync_signal(struct hisi_fb_data_type *hisifd)
{}
void hisifb_buf_sync_suspend(struct hisi_fb_data_type *hisifd)
{}
int hisifb_buf_sync_create_fence(struct hisi_fb_data_type *hisifd, unsigned value)
{ return 0; }
void hisifb_buf_sync_register(struct platform_device *pdev)
{}
void hisifb_buf_sync_unregister(struct platform_device *pdev)
{}
#endif
