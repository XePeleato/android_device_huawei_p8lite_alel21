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

#ifndef __BALONG_COMPOSE_H__
#define __BALONG_COMPOSE_H__

#include <linux/fb.h>
#include <linux/spinlock.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include "ade_cmdqueue.h"

#define DRV_ADE_COMPOSE_NAME    "ade-compose"
#define DEV_ADE_COMPOSE_NAME    "ade"

extern u32 g_command_file_enable;

#if ADE_MEM_SHARE_EN
#define MEM_SHARE_BUF_SIZE_76800B           (76800)
#define MEM_SHARE_BUF_SIZE_153600B          (153600)

#define MEM_SHARE_BUF_PHY_ADDR              (0xF5000000)

#define get_TOP_ADE_CTRL1_val(ade_base)     (inp32(ade_base + ADE_CTRL1_REG))

/* ade hal's response to ade drv */
typedef enum {
    MEM_SHARE_FUNC_IDLE = 0,                            /* ADE HAL rsp to ade drv: no rsp */
    MEM_SHARE_FUNC_ENABLE,                              /* ADE HAL rsp to ade drv: accept alloc request */
    MEM_SHARE_FUNC_DISABLE,                             /* ADE HAL rsp to ade drv: accept free request */
}MEM_SHARE_HAL_RSP_FLAG;

/* ade event flag */
typedef enum {
    MEM_SHARE_EVENT_IDLE = 0,                           /* no event */
    MEM_SHARE_EVENT_ALLOC_720P,                         /* Codec reqest event:buf <= 76800 bytes */
    MEM_SHARE_EVENT_ALLOC_1080P,                        /* Codec reqest event:buf <= 153600 bytes */
    MEM_SHARE_EVENT_FREE,                               /* Codec free buf event */
}MEM_SHARE_EVENT;


/* ade driver status */
typedef enum {
    MEM_SHARE_DRV_IDLE = 0,                             /* idle status */
    MEM_SHARE_DRV_ALLOC_SUCC,                           /* ade alloc memory succ */
    MEM_SHARE_DRV_WAIT_ALLOC_RSP,                       /* ade drv wait for alloc rsp from ade hal */
    MEM_SHARE_DRV_RCV_ALLOC_RSP,                        /* ade drv receive alloc rsp from ade hal */
    MEM_SHARE_DRV_WAIT_FREE_RSP,                        /* ade drv wait for free rsp from ade hal */
    MEM_SHARE_DRV_RCV_FREE_RSP,                         /* ade drv receive free rsp from ade hal */
} MEM_SHARE_ADE_DRV_STATUS;

typedef int (*pADEMemCB)(u32 memSize, u32 memAddress);

/* record memory share driver info */
typedef struct {
    u32                 drv_status;                     /* ade drv status */
    u32                 drv_event;                      /* ade drv event: alloc, free */
    u32                 buf_size;                       /* Codec request memory size */
    u32                 reserved;

    /* function entry chain */
    pADEMemCB           mem_share_cb;
}mem_share_ade_drv_info;


#endif /* ADE_MEM_SHARE_EN */

/*************************struct*********************************/

struct ade_compose_data_type {
    /* private platform driver data */
    u8*     ade_base;
    atomic_t     ref_cnt;
    u32     lcd_type;
    u32     xres;
    u32     yres;
    u32     fb_imgType;
    u32     reserved1;

    u32     soft_rst0;
    u32     soft_rst1;
    u32     reload_dis0;
    u32     reload_dis1;
    struct  platform_device *parent;

    bool    have_refresh;
    bool    fb_reserved_flag;
    bool    pre_frm_have_virt;
    char    reserved[1];
    u32     cmdq1_cmpl;
    u32     cmdq2_cmpl;
    u32     res_switch_cmpl;
    u32     ovly1_cmpl;
    u32     wdma1_cmpl;
    wait_queue_head_t wait_cmdq1_cmpl;
    wait_queue_head_t wait_cmdq2_cmpl;
    wait_queue_head_t wait_res_cmpl;
    wait_queue_head_t wait_ovly1_cmpl;
    wait_queue_head_t wait_wdma1_cmpl;

    struct  ade_overlay_ctrl overlay_ctl;
    struct  cmdqueue_buff    cmdq_list[ADE_CMDQ_TYPE_MAX];
    u32     frame_count;
    u32     reserved2;
    /* wdma2 buffer */
    struct ion_handle *wdma2_ion_handle;
    u32 wdma2_phy;
    u32 reserved3;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif

    struct mutex        ade_lock;

    u32     top_ovly_ctrl; /* record the TOP OVLY CTRL REG of online */
    u32     reserved4;

#if ADE_MEM_SHARE_EN
    mem_share_ade_drv_info  mem_share_drv_info;
#endif
};

/****************************function*************************/
struct platform_device* balong_compose_add_device(struct platform_device *pdev, struct balong_fb_data_type *pfd);
extern void ade_overlay_handle_unlock(struct balong_fb_data_type *fb_data);
extern void ade_overlay_handle_lock(struct ade_compose_data_type *ade_pri_data, struct  overlay_region_info *pipe_info);
int ade_sbl_ctrl_set(struct balong_fb_data_type *balongfd);
int ade_sbl_ctrl_resume(struct balong_fb_data_type *balongfd);
int ade_sbl_bkl_set(struct balong_fb_data_type *balongfd, u32 value);
int ade_overlay_res_set(struct ade_compose_data_type* ade_pri_data, struct overlay_resource_info* res_info);
int ade_overlay_region_set(struct ade_compose_data_type *ade_pri_data, struct overlay_region_info *region_info);
int ade_overlay_commit(struct ade_compose_data_type *ade_pri_data,struct overlay_compose_info *comp_info);
int ade_overlay_refresh(struct ade_compose_data_type    *ade_pri_data, u32 refresh_mode);
int ade_overlay_layer_set(struct ade_compose_data_type  *ade_pri_data, struct overlay_layer_info  *ade_layer_info);
void ade_overlay_init(struct ade_overlay_ctrl *overlay_ctrl);
void ade_overlay_init_wdma2_buff(struct ion_client *fb_ion_client, struct ade_compose_data_type *ade_pri_data);

int ade_fb_start_video_idle(struct balong_fb_data_type *balongfd);
int ade_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
int ade_fb_resume(struct fb_info *info);
void ade_fb_suspend(struct fb_info *info);
void ade_fb_backup_recover(struct balong_fb_data_type      *balongfd);
int ade_fb_disable(struct fb_info *info);
int ade_fb_enable(struct fb_info *info);
void ade_set_medianoc_for_dfs(struct balong_fb_data_type *balongfd);

#if ADE_MEM_SHARE_EN
extern void balong_mem_share_event_send(int event_flag);
extern int DRV_ADEMemAlloc(u32 memSize, pADEMemCB AllocNotifyCB);
extern void DRV_ADEMemFree(void);
extern void balong_mem_share_result_proc(struct balong_fb_data_type *balongfb);

extern int ADEMemCB_test(u32 memSize, u32 memAddress);
extern int balong_ade_mem_share_test(struct ade_compose_data_type *ade_pri_data, void __user *p);
#endif /* ADE_MEM_SHARE_EN */

#endif /* BALONG_COMPOSE_H */
