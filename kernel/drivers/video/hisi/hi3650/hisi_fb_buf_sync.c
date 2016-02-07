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
	//HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	spin_lock(&hisifd->buf_sync_ctrl.refresh_lock);
	if (hisifd->buf_sync_ctrl.refresh) {
		sw_sync_timeline_inc(hisifd->buf_sync_ctrl.timeline, hisifd->buf_sync_ctrl.refresh);
		hisifd->buf_sync_ctrl.refresh = 0;
	}
	spin_unlock(&hisifd->buf_sync_ctrl.refresh_lock);

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
		return; /* -ENOMEM */
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

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);
}
#else
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
