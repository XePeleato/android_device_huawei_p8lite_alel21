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
#ifndef HISI_FB_H
#define HISI_FB_H

#include <linux/console.h>
#include <linux/uaccess.h>
#include <linux/leds.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/fb.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/raid/pq.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <linux/time.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/version.h>
#include <linux/backlight.h>
#include <linux/pwm.h>
#include <linux/pm_runtime.h>
#include <linux/io.h>

#if defined(CONFIG_ARCH_HI3630FPGA) || defined(CONFIG_HISI_3635_FPGA) \
	|| defined(CONFIG_HISI_3650_FPGA)
#include <linux/spi/spi.h>
#else
#include "hisi_udp_board_id.h"
#endif

#include <linux/ion.h>
#include <linux/hisi_ion.h>
#include <linux/gpio.h>

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>

#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/pinctrl/consumer.h>
#include <linux/file.h>
#include <linux/dma-buf.h>
#include <linux/genalloc.h>

#include <linux/huawei/hisi_irq_affinity.h>

#include "hisi_fb_def.h"
#include "hisi_fb_panel.h"
#include "hisi_dss_regs.h"
#include "hisi_dss.h"
#include "hisi_dpe_utils.h"
#include "hisi_mipi_dsi.h"


#define HISI_FB0_NUM	(3)
#define HISI_FB1_NUM	(2)
#define HISI_FB2_NUM	(1)

#define HISI_FB_MAX_DEV_LIST (32)
#define HISI_FB_MAX_FBI_LIST (32)

#define HISI_DSS_OFFLINE_MAX_NUM	(2)
#define HISI_DSS_OFFLINE_MAX_BLOCK	(64)
#define HISI_DSS_OFFLINE_MAX_LIST	(128)


//#define CONFIG_HISI_FB_COLORBAR_USED
//#define CONFIG_HISI_FB_VSYNC_THREAD
#define CONFIG_HISI_FB_BACKLIGHT_DELAY
//#define CONFIG_FAKE_VSYNC_USED
#define CONFIG_BUF_SYNC_USED
//#define CONFIG_HISI_FB_HEAP_CARVEOUT_USED
//#define CONFIG_MIPIDSI_BUGFIX

#define VSYNC_CTRL_EXPIRE_COUNT	(4)

enum {
    DISPLAY_LOW_POWER_LEVEL_FHD = 0x0,
    DISPLAY_LOW_POWER_LEVEL_HD = 0x1,
    DISPLAY_LOW_POWER_LEVEL_MAX,
};

struct hisifb_vsync {
	wait_queue_head_t vsync_wait;
	ktime_t vsync_timestamp;
	int vsync_created;
	int vsync_enabled;

	int vsync_ctrl_expire_count;
	int vsync_ctrl_enabled;
	int vsync_ctrl_disabled_set;
	struct work_struct vsync_ctrl_work;
	spinlock_t spin_lock;

	struct mutex vsync_lock;
#ifdef CONFIG_HISI_FB_VSYNC_THREAD
	struct task_struct *vsync_thread;
#endif

	atomic_t buffer_updated;
	void (*vsync_report_fnc) (int buffer_updated);

	struct hisi_fb_data_type *hisifd;
};

#ifdef CONFIG_BUF_SYNC_USED
#include <linux/sync.h>
#include <linux/sw_sync.h>

struct hisifb_buf_sync {
	struct sw_sync_timeline *timeline;
	int timeline_max;
	int refresh;
	spinlock_t refresh_lock;

};
#endif

struct hisifb_backlight {
#ifdef CONFIG_HISI_FB_BACKLIGHT_DELAY
	struct delayed_work bl_worker;
#endif
	struct semaphore bl_sem;
	int bl_updated;
	int bl_level_old;

};


struct hisi_fb_data_type {
	uint32_t index;
	uint32_t ref_cnt;
	uint32_t fb_num;
	uint32_t fb_imgType;
	uint32_t bl_level;

	char __iomem *dss_base;
	char __iomem *crgperi_base;

	uint32_t dpe_irq;
	uint32_t dsi0_irq;
	uint32_t dsi1_irq;

	struct regulator_bulk_data *dpe_regulator;

	const char *dss_clk_name;
	const char *dss_pxl0_clk_name;
	const char *dss_pxl1_clk_name;
	const char *dss_dphy0_clk_name;
	const char *dss_dphy1_clk_name;

	struct clk *dss_clk;
	struct clk *dss_pxl0_clk;
	struct clk *dss_pxl1_clk;
	struct clk *dss_dphy0_clk;
	struct clk *dss_dphy1_clk;

	struct hisi_panel_info panel_info;
	bool panel_power_on;
	struct semaphore blank_sem;

	void (*pm_runtime_register) (struct platform_device *pdev);
	void (*pm_runtime_unregister) (struct platform_device *pdev);
	void (*pm_runtime_get) (struct hisi_fb_data_type *hisifd);
	void (*pm_runtime_put) (struct hisi_fb_data_type *hisifd);
	void (*bl_register) (struct platform_device *pdev);
	void (*bl_unregister) (struct platform_device *pdev);
	void (*bl_update) (struct hisi_fb_data_type *hisifd);
	void (*bl_cancel) (struct hisi_fb_data_type *hisifd);
	int (*sbl_ctrl_fnc) (struct fb_info *info, void __user *argp);
	void (*sbl_isr_handler)(struct hisi_fb_data_type *hisifd);
	void (*vsync_register) (struct platform_device *pdev);
	void (*vsync_unregister) (struct platform_device *pdev);
	int (*vsync_ctrl_fnc) (struct fb_info *info, void __user *argp);
	void (*vsync_isr_handler) (struct hisi_fb_data_type *hisifd);
	void (*buf_sync_register) (struct platform_device *pdev);
	void (*buf_sync_unregister) (struct platform_device *pdev);
	void (*buf_sync_signal) (struct hisi_fb_data_type *hisifd);
	void (*buf_sync_suspend) (struct hisi_fb_data_type *hisifd);

	bool (*set_fastboot_fnc) (struct fb_info *info);
	int (*open_sub_fnc) (struct fb_info *info);
	int (*release_sub_fnc) (struct fb_info *info);
	int (*on_fnc) (struct hisi_fb_data_type *hisifd);
	int (*off_fnc) (struct hisi_fb_data_type *hisifd);
	int (*frc_fnc) (struct hisi_fb_data_type *hisifd, int fps);
	int (*esd_fnc) (struct hisi_fb_data_type *hisifd);
	int (*pan_display_fnc) (struct hisi_fb_data_type *hisifd);
	int (*ov_ioctl_handler) (struct hisi_fb_data_type *hisifd, uint32_t cmd, void __user *argp);
	int (*ov_online_play) (struct hisi_fb_data_type *hisifd, void __user *argp);
	int (*ov_offline_play) (struct hisi_fb_data_type *hisifd, void __user *argp);
	int (*ov_optimized) (struct hisi_fb_data_type *hisifd, int ovl_idx);
	void (*ov_wb_isr_handler) (struct hisi_fb_data_type *hisifd);
	void (*ov_vactive0_start_isr_handler) (struct hisi_fb_data_type *hisifd);
	void (*set_reg) (struct hisi_fb_data_type *hisifd,
		char __iomem *addr, uint32_t val, uint8_t bw, uint8_t bs);

	struct hisifb_backlight backlight;
	int sbl_enable;
	dss_sbl_t sbl;

	struct hisifb_vsync vsync_ctrl;
#ifdef CONFIG_BUF_SYNC_USED
	struct hisifb_buf_sync buf_sync_ctrl;
#endif
	struct dss_clk_rate dss_clk_rate;

#ifdef CONFIG_FAKE_VSYNC_USED
	bool fake_vsync_used;
	struct hrtimer fake_vsync_hrtimer;
#endif

	bool is_hdmi_power_full;

	uint32_t dss_flush_bits;
	dss_overlay_t ov_req;
	dss_module_reg_t dss_module;

	dss_rect_t *block_rects[HISI_DSS_OFFLINE_MAX_BLOCK];
	dss_overlay_t block_ov;

	bool dss_module_resource_initialized;
	dss_module_reg_t dss_module_default;

	struct dss_rect dirty_region_updt;
	uint8_t dirty_region_updt_enable;

	struct ion_client *ion_client;
	struct ion_handle *ion_handle;
	struct iommu_map_format iommu_format;

	struct fb_info *fbi;
	struct platform_device *pdev;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif

	int switch_res_flag;

	wait_queue_head_t vactive0_start_wq;
	uint32_t vactive0_start_flag;
};


/******************************************************************************
** FUNCTIONS PROTOTYPES
*/
extern int g_debug_ldi_underflow;
extern int g_debug_mmu_error;
extern int g_debug_ovl_osd;
extern int g_debug_online_vsync;
extern int g_debug_ovl_online_composer;
extern int g_debug_ovl_offline_composer;
extern int g_debug_ovl_optimized;
extern int g_enable_ovl_optimized;
extern int g_scf_stretch_threshold;
extern int g_enable_dirty_region_updt;
extern int g_debug_dirty_region_updt;

extern int g_debug_mipi_dphy_lp;
extern int g_debug_dss_clk_lp;

extern int g_primary_lcd_xres;
extern int g_primary_lcd_yres;

extern struct fb_info *fbi_list[HISI_FB_MAX_FBI_LIST];
extern struct hisi_fb_data_type *hisifd_list[HISI_FB_MAX_FBI_LIST];


/* backlight */
void hisifb_backlight_update(struct hisi_fb_data_type *hisifd);
void hisifb_backlight_cancel(struct hisi_fb_data_type *hisifd);
void hisifb_backlight_register(struct platform_device *pdev);
void hisifb_backlight_unregister(struct platform_device *pdev);
void hisifb_sbl_isr_handler(struct hisi_fb_data_type *hisifd);

/* vsync */
#ifdef CONFIG_FAKE_VSYNC_USED
enum hrtimer_restart hisifb_fake_vsync(struct hrtimer *timer);
#endif
void hisifb_activate_vsync(struct hisi_fb_data_type *hisifd);
void hisifb_deactivate_vsync(struct hisi_fb_data_type *hisifd);
int hisifb_vsync_ctrl(struct fb_info *info, void __user *argp);
int hisifb_vsync_resume(struct hisi_fb_data_type *hisifd);
int hisifb_vsync_suspend(struct hisi_fb_data_type *hisifd);
void hisifb_vsync_isr_handler(struct hisi_fb_data_type *hisifd);
void hisifb_vsync_register(struct platform_device *pdev);
void hisifb_vsync_unregister(struct platform_device *pdev);

/* buffer sync */
int hisifb_buf_sync_wait(int fence_fd);
void hisifb_buf_sync_signal(struct hisi_fb_data_type *hisifd);
void hisifb_buf_sync_suspend(struct hisi_fb_data_type *hisifd);
int hisifb_buf_sync_create_fence(struct hisi_fb_data_type *hisifd, unsigned value);
void hisifb_buf_sync_register(struct platform_device *pdev);
void hisifb_buf_sync_unregister(struct platform_device *pdev);

/* control */
int hisifb_ctrl_fastboot(struct hisi_fb_data_type *hisifd);
int hisifb_ctrl_on(struct hisi_fb_data_type *hisifd);
int hisifb_ctrl_off(struct hisi_fb_data_type *hisifd);
int hisifb_ctrl_sbl(struct fb_info *info, void __user *argp);
int hisifb_ctrl_dss_clk_rate(struct fb_info *info, void __user *argp);
int hisifb_ctrl_pixclk_rate(struct fb_info *info, void __user *argp);
int hisifb_ctrl_frc(struct hisi_fb_data_type *hisifd, int fps);
int hisifb_ctrl_esd(struct hisi_fb_data_type *hisifd);

void set_reg(char __iomem *addr, uint32_t val, uint8_t bw, uint8_t bs);
uint32_t set_bits32(uint32_t old_val, uint32_t val, uint8_t bw, uint8_t bs);
void hisifb_set_reg(struct hisi_fb_data_type *hisifd,
	char __iomem *addr, uint32_t val, uint8_t bw, uint8_t bs);
uint32_t hisifb_line_length(int index, uint32_t xres, int bpp);
void hisifb_get_timestamp(struct timeval *tv);
uint32_t hisifb_timestamp_diff(struct timeval *lasttime, struct timeval *curtime);

struct platform_device *hisi_fb_device_alloc(struct hisi_fb_panel_data *pdata,
	uint32_t type, uint32_t id);
struct platform_device *hisi_fb_add_device(struct platform_device *pdev);

int hisi_fb1_blank(int blank_mode);
void hisi_fb1_power_ctrl(bool power_full);

#endif /* HISI_FB_H */
