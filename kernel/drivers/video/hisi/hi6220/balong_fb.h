/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above
 *	   copyright notice, this list of conditions and the following
 *	   disclaimer in the documentation and/or other materials provided
 *	   with the distribution.
 *	 * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *	   contributors may be used to endorse or promote products derived
 *	   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __BALONG_FB_H__
#define __BALONG_FB_H__

#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/notifier.h>
#include <linux/fb.h>
#include <linux/spinlock.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include "balong_fb_def.h"
#include "balong_fb_panel.h"
#include "ade_overlay.h"

#if ADE_SYNC_SUPPORT
#include <linux/sw_sync.h>
#include <linux/file.h>
#endif

#include <linux/vmalloc.h>
#define EVT_TEM_LEVEL_MAX	4
/* FRC */
#define EVT_TEM_NORM		40
#define EVT_TEM_ABOVE_NORM	45
#define EVT_TEM_HIGH		50
#define EVT_TEM_THRD		57
#define FAKE_FPS_TEM_NORM_FPS		60
#define FAKE_FPS_TEM_ABOVE_NORM_FPS	45
#define FAKE_FPS_TEM_HIGH			36
#define FAKE_FPS_TEM_THRD			30
#define BALONG_FB_OVERLAY_USE_BUF 1

#define BALONG_FB_FRC_ENABLE	0

#define BALONG_FB_ESD_ENABLE	1

/* v9r1 bbit test macro  end */

/*FRC Frame definition*/
#define BALONG_FB_FRC_BENCHMARK_FPS	67
#define BALONG_FB_FRC_NORMAL_FPS	60
#define BALONG_FB_FRC_IDLE_FPS	30
#define BALONG_FB_FRC_VIDEO_FPS	60
#define BALONG_FB_FRC_WEBKIT_FPS	45
#define BALONG_FB_FRC_BENCHMARK_FPS	67
#define BALONG_FB_FRC_GAME_FPS		45
#define BALONG_FB_FRC_SPECIAL_GAME_FPS  60

/* Modified for set frc, begin */
/* FRC for Command Panel */
#define BALONG_FB_FRC_GAME_FPS_CMD	45
#define CMD_FPS_SPECIAL_GAME 45
/* Modified for set frc, end */

#define BALONG_FB_FRC_THRESHOLD	6

/* ESD */
#define BALONG_FB_ESD_THRESHOLD	45

/* SBL */
#define SBL_BKL_STEP	5
#define SBL_REDUCE_VALUE(x)	((x) * 70 / 100)

#define LCD_INFO_LENGTH		100

#if defined (CONFIG_HUAWEI_DSM)
#define UNDERFLOW_EXPIRE_COUNT (3)
#define UNDERFLOW_INTERVAL (1000)
#endif

/* display resolution limited */
#define BALONG_FB_MIN_WIDTH	    (16)
#define BALONG_FB_MIN_HEIGHT	(16)
#define BALONG_FB_MAX_WIDTH	    (1200)
#define BALONG_FB_MAX_HEIGHT	(1920)

#define ADE_OVERLAY_MAX_LAYERS   (13)

extern unsigned long hisi_reserved_fb_phymem;

/* frame buffer physical addr */
#define BALONG_FB_PA	hisi_reserved_fb_phymem
#define BALONG_NUM_FRAMEBUFFERS	 (3)
extern u8* balongfd_reg_base_ade;
extern u8* balongfd_reg_base_dsi0;
//extern u32 g_smmu_flag;


/* ADE */
#define ADE_CH_SECU_LINE	11


#define BALONGFB_DEFAULT_BGR_FORMAT	ADE_RGB

/* for MIPI Command LCD */
#define CLK_SWITCH     0      /* 1 */
#define CLK_GET_OK      (1)

#ifndef VM_RESERVED
#define VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)
#endif

/* for Vsync*/
#define VSYNC_TIMEOUT_MSEC 100 //50

enum {
    LCD_LANDSCAPE = 0,
    LCD_PORTRAIT
};

/* set backlight by pwm or mipi ... */
enum {
    BL_SET_BY_NONE = 0,
    BL_SET_BY_PWM = 0x1,
    BL_SET_BY_MIPI = 0x2
};

enum {
    IMG_PIXEL_FORMAT_RGB_565 = 1,
    IMG_PIXEL_FORMAT_RGBA_5551,
    IMG_PIXEL_FORMAT_RGBX_5551,
    IMG_PIXEL_FORMAT_RGBA_8888,
    IMG_PIXEL_FORMAT_RGBX_8888
};

enum {
    FB_64BYTES_ODD_ALIGN_NONE = 0,
    FB_64BYTES_ODD_ALIGN_UI,
    FB_64BYTES_ODD_ALIGN_VIDEO
};

enum {
    FB_LDI_INT_TYPE_NONE = 0x0,
    FB_LDI_INT_TYPE_FRC = 0x1,
    FB_LDI_INT_TYPE_ESD = 0x2
};

/* frc state definition */
enum {
    BALONG_FB_FRC_NONE_PLAYING = 0x0,
    BALONG_FB_FRC_VIDEO_PLAYING = 0x2,
    BALONG_FB_FRC_GAME_PLAYING = 0x4,
    BALONG_FB_FRC_VIDEO_IN_GAME = 0x6,
    BALONG_FB_FRC_BENCHMARK_PLAYING = 0x8,
    BALONG_FB_FRC_WEBKIT_PLAYING =0x10,
    BALONG_FB_FRC_SPECIAL_GAME_PLAYING = 0x20,
    BALONG_FB_FRC_IDLE_PLAYING = 0x40
};

/* ADE isr define */
enum {
    ADE_ISR_RD_CH1_DMA_CMPL = 0x1,
    ADE_ISR_RD_CH2_DMA_CMPL = 0x2,
    ADE_ISR_RD_CH3_DMA_CMPL = 0x4,
    ADE_ISR_RD_CH4_DMA_CMPL = 0x8,
    ADE_ISR_RD_CH5_DMA_CMPL = 0x10,
    ADE_ISR_RD_CH6_DMA_CMPL = 0x20,
    ADE_ISR_RD_DISP_DMA_CMPL    = 0x40,
    ADE_ISR_RD_CMDQ1_DMA_CMPL   = 0x80,
    ADE_ISR_RD_CMDQ2_DMA_CMPL   = 0x100,
    ADE_ISR_RD_CMDQ3_DMA_CMPL   = 0x200,
    ADE_ISR_WR_CH1_DMA_CMPL     = 0x400,
    ADE_ISR_WR_CH2_DMA_CMPL     = 0x800,
    ADE_ISR_WR_CH3_DMA_CMPL     = 0x1000,
    ADE_ISR_WR_CH4_DMA_CMPL     = 0x2000,
    ADE_ISR_WR_CMDQ_DMA_CMPL    = 0x4000,
    ADE_ISR_RD_CH1_AXI_DMA_CMPL = 0x8000,
    ADE_ISR_RD_CH2_AXI_DMA_CMPL     = 0x10000,
    ADE_ISR_RD_CH3_AXI_DMA_CMPL     = 0x20000,
    ADE_ISR_RD_CH4_AXI_DMA_CMPL     = 0x40000,
    ADE_ISR_RD_CH5_AXI_DMA_CMPL     = 0x80000,
    ADE_ISR_RD_CH6_AXI_DMA_CMPL     = 0x100000,
    ADE_ISR_RD_DISP_AXI_DMA_CMPL    = 0x200000,
    ADE_ISR_RD_CMDQ1_AXI_DMA_CMPL   = 0x400000,
    ADE_ISR_RD_CMDQ2_AXI_DMA_CMPL   = 0x800000,
    ADE_ISR_RD_CMDQ3_AXI_DMA_CMPL   = 0x1000000,
    ADE_ISR_DMA_ERROR               = 0x2000000,
    ADE_ISR_SCALOR1_CMPL            = 0x4000000,
    ADE_ISR_SCALOR2_CMPL            = 0x8000000,
    ADE_ISR_SCALOR3_CMPL            = 0x10000000,
    ADE_ISR_SCALOR1_ERROR           = 0x20000000,
    ADE_ISR_SCALOR2_ERROR           = 0x40000000,
    ADE_ISR_SCALOR3_ERROR           = 0x80000000
};

enum {
    ADE_ISR1_CMDQ1_CMPL = 0x200,
    ADE_ISR1_CMDQ2_CMPL = 0x400,
    ADE_ISR1_CMDQ1_ERR_CMD = 0x1000,
    ADE_ISR1_CMDQ2_ERR_CMD = 0x2000,
    ADE_ISR1_CMDQ1_WAIT_TIMER_OUT = 0x8000,
    ADE_ISR1_CMDQ2_WAIT_TIMER_OUT = 0x10000,
    ADE_ISR1_CMDQ1_RD_WR_TIMER_OUT = 0x40000,
    ADE_ISR1_CMDQ2_RD_WR_TIMER_OUT = 0x80000,
    ADE_ISR1_OVLY1_CMPL = 0x00200000,
    ADE_ISR1_RES_SWITCH_CMPL         = 0x80000000
};

enum {
    LDI_ISR_FRAME_START_INT         = 0x1,
    LDI_ISR_FRAME_END_INT           = 0x2,
    LDI_ISR_UNDER_FLOW_INT          = 0x4,
    LDI_ISR_VSYNC_START_INT         = 0x8,
    LDI_ISR_VFP_START_INT           = 0x10,
    LDI_ISR_VBP_START_INT           = 0x20,
    LDI_ISR_VACT_START_INT          = 0x40,
    LDI_ISR_VACT_END_INT            = 0x80,
    LDI_ISR_VFP_END_INT             = 0x400,
    LDI_ISR_DSI_TE_TRI_INT          = 0x800,
    LDI_ISR_DSI_TE0_PIN_INT         = 0x1000,
    LDI_ISR_DSI_TE1_PIN_INT         = 0x2000,
    LDI_ISR_VSYNC_LINE_INT          = 0x4000
};

enum {
    ADE_OUT_RGB_565 = 0,
    ADE_OUT_RGB_666,
    ADE_OUT_RGB_888
};

enum {
    LDI_DISP_MODE_NOT_3D_FBF = 0,
    LDI_DISP_MODE_3D_FBF
};

enum {
    LDI_TEST = 0,
    LDI_WORK
};

/* LCD TYPE */
enum {
    HD  = 0,
    FHD = 1,
};

/* BOARD TYPE */
enum {
    ASIC_BOARD = 0,
    SFT_BOARD  = 1,
};

/* CHIPSET TYPE */
enum {
    HI6220_CHIPSET   = 0,
    HI6210_CHIPSET   = 1,
};

#ifdef CONFIG_DEBUG_FS
/* lowpower debugfs IP Flag */
enum {
    DEBUG_CLEAR_LOWPOWER_DISABLE = 0,
    DEBUG_EDC_LOWPOWER_DISABLE  = 0x1,
    DEBUG_MIPI_LOWPOWER_DISABLE = 0x2,
    DEBUG_LCD_LOWPOWER_DISABLE  = 0x4,
    DEBUG_G2D_LOWPOWER_DISABLE  = 0x8,
    DEBUG_ALL_LOWPOWER_DISABLE  = 0xF
};

typedef union {
    struct {
        u32         bit_vsync     : 3;
        u32         bit_display   : 3;
        u32         bit_backlight : 3;
        u32         bit_topinfo   : 3;
        u32         bit_wr_cmdf   : 3;
        u32         reserved      : 17;
    } bits;
    u32             ul32;
} balongfb_debug_log_info_union;

extern u32 g_fb_colorbar_mod;
extern balongfb_debug_log_info_union g_fb_log_printk_flag;
extern u32 g_fb_ade_core_div;
extern u32 g_fb_ade_pixel_div;

#endif
/**
 * struct balongfb_vsync - vsync information+
 * @wait:              a queue for processes waiting for vsync
 * @timestamp:         the time of the last vsync interrupt
 * @active:            whether userspace is requesting vsync uevents
 * @irq_refcount:      reference count for the underlying irq
 * @irq_lock:          mutex protecting the irq refcount and register
 * @thread:            uevent-generating thread
 */
struct balongfb_vsync {
    wait_queue_head_t       wait;
    ktime_t                 timestamp;
    bool                    active;
    bool                    vsync_ctrl_disabled_set;
    char                    reserved[6];
    int                     vsync_ctrl_expire_count;
    int                     te_refcount;
    int                     irq_refcount;
    spinlock_t              irq_lock;
    spinlock_t              spin_lock;
    struct task_struct      *thread;
    struct work_struct      vsync_ctrl_work;
    struct balong_fb_data_type *balongfd;
};

struct balong_fb_data_type {
    u32 index;
    u32 ref_cnt;
    u32 bl_level;
    u32 bl_level_old;
    u32 bl_level_sbl;
    u32 bl_level_cmd;
    u32 fb_imgType;
    u32 fb_bgrFmt;
    u32 ade_irq;
    u8* ade_base;
    u8* dsi_base;
    u8* media_base;
    u8* media_noc_base;
    u32 frame_updated;
    u32 ldi_irq;
    s32 ldi_int_type;

    char ade_irq_name[64];
    char ldi_irq_name[64];

    panel_id_type panel;
    struct balong_panel_info panel_info;
    bool panel_power_on;
    bool ade_core_power_on;
    bool mem_share_free;
    bool ade_ldi_on;
    char reserved[4];

    struct fb_info *fbi;
    u32 graphic_ch;

    s32 update_frame;
    wait_queue_head_t frame_wq;
    struct work_struct frame_end_work;
    struct workqueue_struct *frame_end_wq;

    struct work_struct video_idle_work;
    struct workqueue_struct *video_idle_wq;


    bool cmd_mode_refresh;
    bool cmd_bl_can_set;
    char reserved2[2];
    u32  virtual_disp_ref;

    struct ade_compose_data_type    *ade_pri_data;
    struct platform_device  *pdev;
    struct semaphore sem;
    struct clk *ade_clk;
    struct clk *ade_par_clk;
    struct clk *ldi_clk;
    struct clk *media_noc_clk;
    struct clk *media_noc_par_clk;
    struct clk *dsi_cfg_clk;

    struct clk *sys_pll;
    struct clk *media_pll;

    u32 ade_core_rate;
    u32 media_noc_rate;

    struct balongfb_vsync	vsync_info;
    u32    ovcIdleCount;
    u32    reserved3;
    struct semaphore power_on_sem;
    struct platform_device *ade_pdev;
    struct ion_client *fb_ion_client;
    struct ion_handle *fb_ion_handle;

    /* Add for set frc, begin */
    bool use_cmd_vsync;

    /* Add for set/lase ade core rate */
    bool vpu_power_on;
    int codec_pm_state;
    u32 ade_set_core_rate;
    u32 last_ade_core_rate;

    /* Add for set frc, end */
    struct hrtimer cmd_vsync_hrtimer;

    /* for sbl */
    u32 sbl_enable;
    u32 sbl_lsensor_value;
    struct work_struct sbl_work;
    struct workqueue_struct *sbl_wq;

    /*for cabc*/
    u32 g_CABC_mode;

    /*for gram check*/
    u32 lcd_readvalue;
    u32 lcd_inversion_mode;

    /* for frc */
    s32 frc_state;
    s32 frc_threshold_count;

#if LCD_ESD_CHECK_SUPPORT
    /* for esd */
    struct hrtimer esd_hrtimer;
    bool esd_check;
    struct task_struct      *esd_thread;
    wait_queue_head_t        esd_wait;
#endif //LCD_ESD_CHECK_SUPPORT

#if defined (CONFIG_HUAWEI_DSM)
    struct workqueue_struct *balongfb_debug_workqueue;
    struct work_struct balongfb_debug_work;
#endif

    struct semaphore mipi_read_sem;


#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend;
#endif

#if ADE_SYNC_SUPPORT
    spinlock_t refresh_lock;
    int timeline_max;
    int refresh;
    struct sw_sync_timeline *timeline;
#endif
    unsigned long frc_timestamp;

    struct ovly_hnd_info locking_hnd[ADE_OVERLAY_MAX_LAYERS];
    struct ovly_hnd_info locked_hnd[ADE_OVERLAY_MAX_LAYERS];

#if PARTIAL_UPDATE
    int    dirty_update;
    int    xoffset;
    int    yoffset;
    int    xwidth;
    int    yheight;
    int    vbp;
    int    hbp;
#endif

    int    pw_state_qos_id;
    struct notifier_block nb;
    struct notifier_block fb;
};
#if LCD_CHECK_MIPI_SUPPORT
enum CHECK_MIPI_TR
{
    CHECK_MIPI_TR_FAIL = -1,
    CHECK_MIPI_TR_OK = 0,
};
extern int lcd_readvalue;
#endif
/******************************************************************************
 ** global variable
 ******************************************************************************/
#ifdef PC_UT_TEST_ON
//#define MAX_FBI_LIST    32
extern STATIC bool                      hdmi_is_connected;
extern STATIC struct fb_info           *fbi_list[32];
extern STATIC struct balong_fb_data_type   *balongfd_list[32];
extern STATIC int                       fbi_list_index;
extern STATIC int                       balong_fb_resource_initialized;
extern STATIC int                       pdev_list_cnt;

#endif

#if defined (CONFIG_HUAWEI_DSM)
extern struct dsm_client *lcd_dclient;
#endif

/*add lcd info*/
extern char g_lcd_name_buf[100];

#define VSYNC_CTRL_EXPIRE_COUNT    (4)

extern u32 three_lane_flag;
/* Define a flag for adjusting of color temperature. */
extern u32 colortemp_adjust_flag;

/******************************************************************************
 ** FUNCTIONS PROTOTYPES
 */
struct platform_device *balong_fb_add_device(struct platform_device *pdev);
void balong_fb_set_backlight(struct balong_fb_data_type *balongfd, u32 bkl_lvl);
u32 balong_fb_line_length(u32 xres, int bpp);
extern void balongfb_te_inc(struct balong_fb_data_type *balongfd, bool te_should_enable);
extern void balongfb_te_dec(struct balong_fb_data_type *balongfd, bool te_should_disable);
extern int balongfb_frc_get_fps(struct balong_fb_data_type *balongfd);
#if PARTIAL_UPDATE
extern void balongfb_set_display_region(struct balong_fb_data_type *balongfd);
#endif
extern void * hisi_memset(void *, int, __kernel_size_t);
extern int fb_get_board_type(void);
extern int get_chipset_type(void);

void *mem_remap_type(unsigned long phys_addr, size_t size, pgprot_t pgprot);

extern int hisi_fb_sysfs_create(struct platform_device *pdev);
extern void hisi_fb_sysfs_remove(struct platform_device *pdev);
#endif /* BALONG_FB_H */
