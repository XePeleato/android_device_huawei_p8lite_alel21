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

#include "hisi_fb.h"
#include "hisi_overlay_utils.h"


#define DTS_COMP_HISI_OFFLINECOMPOSER_PANEL	"hisilicon,offlinecomposer_panel"


/*******************************************************************************
**
*/
static int hisi_offlinecompser_panel_on(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	int i = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("index=%d, enter!\n", hisifd->index);

	for (i = 0; i < HISI_DSS_OFFLINE_MAX_NUM; i++) {
		char __iomem *ctl_base = hisifd->dss_base + DSS_GLB_WBE1_CH0_CTL +
				i * (DSS_GLB_WBE1_CH1_CTL - DSS_GLB_WBE1_CH0_CTL);

		cmdlist_config_start(hisifd, i);
		hisifd->offline_wb_status[i] = e_status_wait;
		set_reg(ctl_base, 0x1, 1, 8);
		set_reg(ctl_base, 0x0, 1, 8);
	}

	HISI_FB_DEBUG("index=%d, exit!\n", hisifd->index);

	return ret;
}

static int hisi_offlinecompser_panel_off(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	int i = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("index=%d, enter!\n", hisifd->index);

	for (i = 0; i < HISI_DSS_OFFLINE_MAX_NUM; i++) {
		char __iomem *ctl_base = hisifd->dss_base + DSS_GLB_WBE1_CH0_CTL +
				i * (DSS_GLB_WBE1_CH1_CTL - DSS_GLB_WBE1_CH0_CTL);

		cmdlist_config_stop(hisifd, i);
		hisifd->offline_wb_status[i] = e_status_idle;
		set_reg(ctl_base, 0x1, 1, 8);
		set_reg(ctl_base, 0x0, 1, 8);
	}

	HISI_FB_DEBUG("index=%d, exit!\n", hisifd->index);

	return ret;
}

static int hisi_offlinecompser_panel_remove(struct platform_device *pdev)
{
	int ret = 0;
	int i = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	/*BUG_ON(hisifd == NULL);*/

	if (!hisifd) {
		return 0;
	}

	HISI_FB_DEBUG("index=%d, enter!\n", hisifd->index);

	for (i = 0; i < HISI_DSS_OFFLINE_MAX_BLOCK; i++) {
		kfree(hisifd->block_rects[i]);
		hisifd->block_rects[i] = NULL;
	}

	if (hisifd->block_overlay) {
		kfree(hisifd->block_overlay);
		hisifd->block_overlay = NULL;
	}

	for(i = 0; i < HISI_DSS_OFFLINE_MAX_LIST; i++){
		cmdlist_free_one_list(hisifd->cmdlist_node[i], hisifd->ion_client);
	}

	HISI_FB_DEBUG("index=%d, exit!\n", hisifd->index);

	return ret;
}

static struct hisi_panel_info hisi_offlinecompser_panel_info = {0};
static struct hisi_fb_panel_data hisi_offlinecomposer_panel_data = {
	.panel_info = &hisi_offlinecompser_panel_info,
	.set_fastboot = NULL,
	.on = hisi_offlinecompser_panel_on,
	.off = hisi_offlinecompser_panel_off,
	.remove = hisi_offlinecompser_panel_remove,
	.set_backlight = NULL,
	.sbl_ctrl = NULL,
	.vsync_ctrl = NULL,
	.frc_handle = NULL,
	.esd_handle = NULL,
};

static int hisi_offlinecompser_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_panel_info *pinfo = NULL;
	struct hisi_fb_data_type *hisifd = NULL;
	struct platform_device *reg_dev = NULL;
	int i = 0;

	if (hisi_fb_device_probe_defer(PANEL_OFFLINECOMPOSER)) {
		goto err_probe_defer;
	}

	HISI_FB_DEBUG("+.\n");

	BUG_ON(pdev == NULL);

	pdev->id = 1;
	pinfo = hisi_offlinecomposer_panel_data.panel_info;
	memset(pinfo, 0, sizeof(struct hisi_panel_info));
	pinfo->xres = g_primary_lcd_xres;
	pinfo->yres = g_primary_lcd_yres;
	pinfo->type = PANEL_OFFLINECOMPOSER;

	/* alloc panel device data */
	ret = platform_device_add_data(pdev, &hisi_offlinecomposer_panel_data,
		sizeof(struct hisi_fb_panel_data));
	if (ret) {
		HISI_FB_ERR("platform_device_add_data failed!\n");
		goto err_device_put;
	}

	reg_dev = hisi_fb_add_device(pdev);
	if (!reg_dev) {
		HISI_FB_ERR("hisi_fb_add_device failed!\n");
		goto err_device_put;
	}

	hisifd = platform_get_drvdata(reg_dev);
	BUG_ON(hisifd == NULL);

	for (i = 0; i < HISI_DSS_OFFLINE_MAX_NUM; i++) {
		INIT_LIST_HEAD(&hisifd->offline_cmdlist_head[i]);
		hisifd->offline_wb_status[i] = e_status_idle;
		init_waitqueue_head(&hisifd->offline_writeback_wq[i]);
	}

	for (i = 0; i < HISI_DSS_OFFLINE_MAX_BLOCK; i++) {
		hisifd->block_rects[i] = (dss_rect_t *)kmalloc(sizeof(dss_rect_t), GFP_ATOMIC);
		if (!hisifd->block_rects[i]) {
			HISI_FB_ERR("block_rects[%d] failed to alloc!", i);
			goto err_device_put;
		}
	}

	hisifd->block_overlay = (dss_overlay_t *)kmalloc(sizeof(dss_overlay_t), GFP_ATOMIC);
	if (!hisifd->block_overlay) {
		HISI_FB_ERR("hisifd->block_overlay no mem.\n");
		goto err_device_put;
	}
	memset(hisifd->block_overlay, 0, sizeof(dss_overlay_t));

	for(i = 0; i < HISI_DSS_OFFLINE_MAX_LIST; i++){
		ret = cmdlist_alloc_one_list(&hisifd->cmdlist_node[i], hisifd->ion_client);
		if(ret < 0){
			HISI_FB_ERR("cmdlist_alloc_one_list no mem.\n");
			goto err_device_put;
		}
	}

	hisi_dss_offline_module_default(hisifd);
	hisifd->dss_module_resource_initialized = true;

	HISI_FB_DEBUG("-.\n");

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
err_probe_defer:
	return -EPROBE_DEFER;
}

static const struct of_device_id hisi_panel_match_table[] = {
	{
		.compatible = DTS_COMP_HISI_OFFLINECOMPOSER_PANEL,
		.data = NULL,
	},
	{},
};
MODULE_DEVICE_TABLE(of, hisi_panel_match_table);

static struct platform_driver this_driver = {
	.probe = hisi_offlinecompser_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = "hisi_offlinecompser_panel",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hisi_panel_match_table),
	},
};

static int __init hisi_offlinecompser_panel_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(hisi_offlinecompser_panel_init);
