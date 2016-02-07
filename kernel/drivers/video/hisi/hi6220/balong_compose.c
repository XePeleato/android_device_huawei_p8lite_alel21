/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/console.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/leds.h>
#include <linux/time.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/hisi/hisi_ion.h>
#include <trace/trace_kernel.h>
#include <linux/highmem.h>
#include <linux/memblock.h>
#include <linux/hw_lcd_common.h>
#include "balong_fb.h"
#include "balong_compose.h"
#include "ade_reg_offset.h"
#include "ldi_reg.h"
#include "mipi_reg.h"
#include "ade_overlay.h"

#ifdef  ADE_DEBUG_LOG_ENABLE
#include <linux/debugfs.h>
#include <linux/string.h>
#endif

#if ADE_DEBUG_LOG_ENABLE
u32 g_command_file_wr_enable = 0;
u32 g_ade_power_off_enable = 1;
u32 g_ade_isr_disable = 0;
u32 g_ade_offline_disable = 0;
u32 g_debug_diable_ade = 0;
u32 g_debug_dis_ade_ioctl = 0;
u32 g_debug_frame_number = 0;
extern int *debug_cmd_vaddr;
struct balong_fb_data_type *g_debug_balongfd = NULL;

#endif

static struct class* compose_class = NULL;
static uint   major = 199;

extern u8* balongfd_reg_base_ade;

extern struct semaphore balong_fb_overlay_sem;
extern struct semaphore balong_fb_backlight_sem;
extern struct semaphore balong_fb_blank_sem;


STATIC struct ade_compose_data_type *g_ade_pri_data = NULL;


//module_param(major, uint, 0644);
#if ADE_SYNC_SUPPORT
STATIC int balong_ade_overlay_fence_create(struct sw_sync_timeline * timeline,
        const char * fence_name, unsigned value)
{
    int    fd;
    struct sync_fence *fence;
    struct sync_pt    *pt;

    if (timeline == NULL){
        return -EINVAL;
    }

    fd = get_unused_fd();
    if (fd < 0) {
        balongfb_loge("failed to get fd in create fence!\n");
        return fd;
    }

    pt = sw_sync_pt_create(timeline, value);
    if (pt == NULL) {
        put_unused_fd(fd);
        balongfb_loge("failed to create pt in create fence!\n");
        return -ENOMEM;
    }

    fence = sync_fence_create(fence_name, pt);
    if (fence == NULL) {
        sync_pt_free(pt);
        put_unused_fd(fd);
        balongfb_loge("failed to create fence in create fence!\n");
        return -ENOMEM;
    }

    sync_fence_install(fence, fd);

    return fd;
}
#endif /* ADE_SYNC_SUPPORT */

/***************************** ioctl function**********************************/
STATIC int balong_ade_overlay_res_set(struct ade_compose_data_type    *ade_pri_data, void __user *p)
{
    struct overlay_resource_info res_info;
    struct balong_fb_data_type *balongfd = NULL;
    int ret = 0;

    BUG_ON(ade_pri_data == NULL);

    balongfb_logi_display_debugfs("balong_ade_overlay_res_set enter succ ! \n");

    if (copy_from_user(&res_info, p, sizeof(struct overlay_resource_info))) {
        balongfb_loge("copy from user failed!\n");
        return -EFAULT;
    }

    balongfb_logi_top_info("res_info.comp_mode= %d\n", res_info.comp_mode);
    balongfb_logi_top_info("res_info.disp_ch_res = 0x%x \n", res_info.disp_ch_res.ul32);
    balongfb_logi_top_info("res_info.ovly1_res = 0x%x \n", res_info.ovly1_res.ul32);
    balongfb_logi_top_info("res_info.ovly2_res = 0x%x \n", res_info.ovly2_res.ul32);
    balongfb_logi_top_info("res_info.ovly3_res = 0x%x\n", res_info.ovly3_res.ul32);
    balongfb_logi_top_info("res_info.rot_res = 0x%x\n", res_info.rot_res.ul32);
    balongfb_logi_top_info("res_info.scl2_res = 0x%x\n", res_info.scl2_res.ul32);
    balongfb_logi_top_info("res_info.compose_ch_res[0] = 0x%x\n", res_info.compose_ch_res[0].ul32);
    balongfb_logi_top_info("res_info.compose_ch_res[1] = 0x%x\n", res_info.compose_ch_res[1].ul32);
    balongfb_logi_top_info("res_info.compose_ch_res[2] = 0x%x\n", res_info.compose_ch_res[2].ul32);
    balongfb_logi_top_info("res_info.compose_ch_res[3] = 0x%x\n", res_info.compose_ch_res[3].ul32);
    balongfb_logi_top_info("res_info.compose_ch_res[4] = 0x%x\n", res_info.compose_ch_res[4].ul32);
    balongfb_logi_top_info("res_info.compose_ch_res[5] = 0x%x\n", res_info.compose_ch_res[5].ul32);

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    down(&balong_fb_blank_sem);
    if (!balongfd->ade_core_power_on) {
        up(&balong_fb_blank_sem);
        balongfb_logi("ade_core_power_on is false!\n");
        return -EPERM;
    }

    ret = ade_overlay_res_set(ade_pri_data, &res_info);

    if (ret != 0) {
        up(&balong_fb_blank_sem);
        balongfb_loge("ade_overlay_res_set ioctl failed!\n");
        return ret;
    }

    up(&balong_fb_blank_sem);

#if ADE_DEBUG_LOG_ENABLE
    if (g_command_file_wr_enable) {
        if (debug_cmd_vaddr == NULL) {
            debug_cmd_vaddr = kzalloc(ADE_CMD_BUFF_SIZE_MAX, GFP_KERNEL);
        }
    } else {
        if (debug_cmd_vaddr != NULL) {
            kzfree(debug_cmd_vaddr);
            debug_cmd_vaddr = NULL;
        }
    }
#endif

    balongfb_logi_display_debugfs("balong_ade_overlay_res_set exit succ ! \n");

    return 0;
}

STATIC int balong_ade_overlay_region_set(struct ade_compose_data_type    *ade_pri_data, void __user *p)
{
    struct overlay_region_info region_info;
    struct balong_fb_data_type *balongfd = NULL;
    int ret = 0;

    BUG_ON(ade_pri_data == NULL);

    balongfb_logi_display_debugfs("balong_ade_overlay_set enter succ ! \n");

    if (copy_from_user(&region_info, p, sizeof(region_info))) {
        balongfb_loge("copy from user failed!\n");
        return -EFAULT;
    }

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    down(&balong_fb_blank_sem);
    if (!balongfd->ade_core_power_on) {
        up(&balong_fb_blank_sem);
        balongfb_logi("ade_core_power_on is false !\n");
        return -EPERM;
    }

    ret = ade_overlay_region_set(ade_pri_data, &region_info);

    if (ret) {
        balongfb_loge("ade_overlay_set ioctl failed!\n");
        up(&balong_fb_blank_sem);
        return ret;
    }

    up(&balong_fb_blank_sem);

    balongfb_logi_display_debugfs("balong_ade_overlay_set exit succ ! \n");

    return 0;
}

STATIC int balong_ion_free_mem_to_buddy(void)
{
    int i;
    u32 fb_heap_phy = 0;
    struct ion_heap_info_data mem_data;

    if (0 != hisi_ion_get_heap_info(ION_FB_HEAP_ID, &mem_data)) {
        balongfb_loge("fail to get ION_FB_HEAP_ID\n");
        return -EINVAL;
    }

    if (0 == mem_data.heap_size) {
        balongfb_loge("fb reserved size 0\n");
        return -EINVAL;
    }

    fb_heap_phy = mem_data.heap_phy;
    for(i = 0; i < ((mem_data.heap_size)/PAGE_SIZE); i++){
        free_reserved_page(phys_to_page(mem_data.heap_phy));
#ifdef CONFIG_HIGHMEM
        if (PageHighMem(phys_to_page(mem_data.heap_phy)))
            totalhigh_pages += 1;
#endif
        mem_data.heap_phy += PAGE_SIZE;
    }

    memblock_free(fb_heap_phy, mem_data.heap_size);
    return 0;
}

STATIC int balong_ade_overlay_commit(struct ade_compose_data_type    *ade_pri_data, void __user *p)
{
    struct overlay_compose_info comp_info;
    struct balong_fb_data_type *balongfd = NULL;
    int ret = 0;
    u32 struct_len = sizeof(struct ovly_hnd_info) * ADE_OVERLAY_MAX_LAYERS;
#if ADE_SYNC_SUPPORT
    int fenceId = 0;
    unsigned long flags;
#endif

    BUG_ON(ade_pri_data == NULL);

    balongfb_logi_display_debugfs("balong_ade_overlay_commit enter succ ! \n");

    if (copy_from_user(&comp_info, p, sizeof(comp_info))) {
        balongfb_loge("copy from user failed!\n");
        return -EFAULT;
    }

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);
    BUG_ON(balongfd == NULL);
    if ((!balongfd->frame_updated) && lcd_pwr_status.panel_power_on)
    {

        balongfd->frame_updated = 1;
        lcd_pwr_status.lcd_dcm_pwr_status |= BIT(2);
        do_gettimeofday(&lcd_pwr_status.tvl_set_frame);
        time_to_tm(lcd_pwr_status.tvl_set_frame.tv_sec, 0, &lcd_pwr_status.tm_set_frame);
    }
    down(&balong_fb_blank_sem);
    if (!balongfd->ade_core_power_on) {
        up(&balong_fb_blank_sem);
        balongfb_logi("ade_core_power_on is false !\n");
        return -EPERM;
    }


    if (ADE_TRUE == comp_info.is_finished) {
        spin_lock_irqsave(&balongfd->refresh_lock, flags);
        balongfd->refresh++;
        spin_unlock_irqrestore(&balongfd->refresh_lock, flags);
        balongfd->timeline_max++;
        //release the reserved buffer of fb
        if((true == ade_pri_data->fb_reserved_flag) && (ade_pri_data->frame_count)) {
            balong_ion_free_mem_to_buddy();
            ade_pri_data->fb_reserved_flag = false;
        }

        if (PANEL_MIPI_VIDEO == ade_pri_data->lcd_type) {
            spin_lock_irqsave(&(balongfd->vsync_info.spin_lock), flags);
            set_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_FRAME_END_INT);
            balongfd->vsync_info.vsync_ctrl_expire_count = 0;
            spin_unlock_irqrestore(&(balongfd->vsync_info.spin_lock), flags);

            if (balongfd->vsync_info.vsync_ctrl_disabled_set) {
                if (balongfd->ldi_irq) {
                    enable_irq(balongfd->ldi_irq);
                    balongfd->vsync_info.vsync_ctrl_disabled_set = false;
                }
            }
        }

#ifndef PC_UT_TEST_ON
#ifdef CONFIG_TRACING
        trace_dot(SF, "7", 0);
#endif
#endif

#if ADE_DEBUG_LOG_ENABLE
        g_debug_frame_number = comp_info.frame_number;
#endif
    }

    ret = ade_overlay_commit(ade_pri_data, &comp_info);

    if ((ADE_TRUE == comp_info.is_finished)) {
        if (ret == 0) {
            set_LDI_INT_MASK_bit(ade_pri_data->ade_base, LDI_ISR_UNDER_FLOW_INT);
#if PARTIAL_UPDATE
            if ((PANEL_MIPI_CMD == ade_pri_data->lcd_type) && (true == balongfd->dirty_update)) {
                balongfb_set_display_region(balongfd);
                balongfd->dirty_update = false;
            }
#endif
            if (PANEL_MIPI_VIDEO == ade_pri_data->lcd_type) {
                if ((ade_pri_data->overlay_ctl.comp_info.compose_mode != OVERLAY_COMP_TYPE_ONLINE)
                    || (balongfd->vpu_power_on)){
                     /* ade_core_rate is default value (360M) */
                     balongfd->ade_set_core_rate = balongfd->ade_core_rate;
                }

                if (balongfd->last_ade_core_rate != balongfd->ade_set_core_rate) {
                    if (clk_set_rate(balongfd->ade_clk, balongfd->ade_set_core_rate) != 0) {
                          balongfb_loge("clk_set_rate ade_core_rate error \n");
                    }
                }

                balongfd->last_ade_core_rate = balongfd->ade_set_core_rate;

            }

            set_LDI_CTRL_ldi_en(ade_pri_data->ade_base, ADE_ENABLE);
            balongfd->ade_ldi_on = true;
            if (PANEL_MIPI_CMD == ade_pri_data->lcd_type) {
                set_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_DSI_TE0_PIN_INT);

                /* enable fake vsync timer */
                if (balongfd->frc_state != BALONG_FB_FRC_IDLE_PLAYING) {
                    balongfd->use_cmd_vsync = (balongfb_frc_get_fps(balongfd) < BALONG_FB_FRC_NORMAL_FPS ? true : false);
                }

                /* report vsync with timer */
                if (balongfd->use_cmd_vsync) {
                    hrtimer_restart(&balongfd->cmd_vsync_hrtimer);
                } else {
                    hrtimer_cancel(&balongfd->cmd_vsync_hrtimer);
                }
            }

#ifndef PC_UT_TEST_ON
#ifdef CONFIG_TRACING
            trace_dot(SF, "8", 0);
#endif
#endif

#if ADE_SYNC_SUPPORT
            /* In online/hybrid mode, ADE must create release fenceFd.
             * In offline mode, don't create release fenceFd
             * because ADE will read HAL's offline buffer instead of layer's buffer.
             */
            /*
               spin_lock_irqsave(&balongfd->refresh_lock, flags);
               balongfd->refresh++;
               spin_unlock_irqrestore(&balongfd->refresh_lock, flags);
               balongfd->timeline_max++;
             */
            if ((OVERLAY_COMP_TYPE_ONLINE == comp_info.compose_mode)
                    || (OVERLAY_COMP_TYPE_HYBRID == comp_info.compose_mode)) {

                fenceId = balong_ade_overlay_fence_create(balongfd->timeline, "ADE", balongfd->timeline_max);
                if (fenceId < 0) {
                    balongfb_loge("ADE failed to create fence!\n");
                }
                comp_info.release_fence = fenceId;
                if (copy_to_user((struct overlay_compose_info __user*)p, &comp_info, sizeof(struct overlay_compose_info))
                        && (fenceId >= 0)) {
                    fenceId = -EFAULT;
                    balongfb_loge("ADE failed to copy fence to user!\n");
                    put_unused_fd(comp_info.release_fence);

                    up(&balong_fb_blank_sem);
                    return fenceId;
                }
            }
#endif /* ADE_SYNC_SUPPORT */
            ade_overlay_handle_unlock(balongfd);
            memcpy(balongfd->locked_hnd, balongfd->locking_hnd, struct_len);
            memset(balongfd->locking_hnd, 0, struct_len);
        } else {
            ade_overlay_handle_unlock(balongfd);
            memcpy(balongfd->locked_hnd, balongfd->locking_hnd, struct_len);
            memset(balongfd->locking_hnd, 0, struct_len);
            up(&balong_fb_blank_sem);
            return ret;
        }
    }

    up(&balong_fb_blank_sem);

    balongfb_logi_display_debugfs("balong_ade_overlay_commit exit succ ! \n");
    return 0;
}

STATIC int balong_ade_overlay_refresh(struct ade_compose_data_type    *ade_pri_data, void __user *p)
{
    int ret = 0;
    u32 refresh_mode;
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(ade_pri_data == NULL);

    balongfb_logi_display_debugfs("balong_ade_overlay_refresh enter succ ! \n");

    if (copy_from_user(&refresh_mode, p, sizeof(refresh_mode))) {
        balongfb_loge("copy from user failed \n");
        return -EINVAL;
    }

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    down(&balong_fb_blank_sem);
    if (!balongfd->ade_core_power_on) {
        up(&balong_fb_blank_sem);
        balongfb_logi("ade_core_power_on is false !\n");
        return -EPERM;
    }

    ret = ade_overlay_refresh(ade_pri_data, refresh_mode);
    if (ret) {
        balongfb_loge("ade_overlay_refresh is error \n");
        up(&balong_fb_blank_sem);
        return ret;
    }

    up(&balong_fb_blank_sem);
    balongfb_logi_display_debugfs("balong_ade_overlay_refresh exit succ ! \n");
    return 0;
}

STATIC int balong_ade_overlay_layer_set(struct ade_compose_data_type    *ade_pri_data, void __user *p)
{
    int ret = 0;
    struct overlay_layer_info ade_block_info;
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(ade_pri_data == NULL);

    balongfb_logi_display_debugfs("balong_ade_overlay_layer_set enter succ ! \n");

    if (copy_from_user(&ade_block_info, p, sizeof(struct overlay_layer_info))) {
        balongfb_loge("copy from user failed \n");
        return -EINVAL;
    }

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    down(&balong_fb_blank_sem);
    if (!balongfd->ade_core_power_on) {
        up(&balong_fb_blank_sem);
        balongfb_logi("ade_core_power_on is false !\n");
        return -EPERM;
    }

    ret = ade_overlay_layer_set(ade_pri_data, &ade_block_info);
    if (ret) {
        balongfb_loge("ade_overlay_refresh is error \n");
        up(&balong_fb_blank_sem);
        return ret;
    }
    up(&balong_fb_blank_sem);
    balongfb_logi_display_debugfs("balong_ade_overlay_layer_set exit succ ! \n");
    return 0;
}
/******************************file ops****************************************/

/*
 *  when open the compose dev, must open fb dev first, and
 *  the ADE subsystem will be power on at fb_open
 */
int balong_ade_open(struct inode* inode, struct file* file)
{
    struct balong_fb_data_type *pfb_data;

    BUG_ON(file == NULL);

    balongfb_logi("enter succ ! \n");

    if (g_ade_pri_data == NULL) {
        balongfb_loge("pfd_data is null \n");
        return -EINVAL;
    }

    file->private_data = g_ade_pri_data;
    if ((atomic_read(&g_ade_pri_data->ref_cnt))> 0) {
        balongfb_logi("have been opend, ref_cnt = error \n");
        return 0;
    }

    pfb_data = (struct balong_fb_data_type *)platform_get_drvdata(g_ade_pri_data->parent);
    BUG_ON(pfb_data == NULL);

    atomic_inc(&g_ade_pri_data->ref_cnt);

#if ADE_DEBUG_LOG_ENABLE
    g_debug_diable_ade = 0;
    g_debug_dis_ade_ioctl = 0;
#endif

    balongfb_logi("exit succ ! \n");
    return 0;
}

/*
 *  when release the compose dev, must release fb dev first, and
 *  the ADE subsystem will be power dowm at fb_release
 */
int balong_ade_release(struct inode* inode, struct file* file)
{
    struct ade_compose_data_type    *ade_pri_data = NULL;

    BUG_ON(file == NULL);

    balongfb_logi("enter succ ! \n");

    ade_pri_data = file->private_data;
    BUG_ON(ade_pri_data == NULL);
    // ade_pri_data->frame_count = 0;

    if (atomic_read(&ade_pri_data->ref_cnt) == 0) {
        balongfb_loge(" balong_ade_release ref_cnt = 0 \n");
        return -EINVAL;
    }

    if(!atomic_dec_and_test(&ade_pri_data->ref_cnt)) {
        balongfb_loge(" balong_ade_release atomic_dec_and_test error\n ");
        return -EINVAL;
    }

    file->private_data = NULL;

    balongfb_logi("exit succ ! \n");
    return 0;
}

/* SET ADE CLOCK RATE  */
int balong_ade_clock_rate_set(struct ade_compose_data_type *ade_pri_data, void __user *p)
{
    int     ret = 0;
    struct  balong_fb_data_type *balongfd = NULL;
    struct  clock_rate_info info;
    BUG_ON(ade_pri_data == NULL);

    if (copy_from_user(&info, p, sizeof(struct clock_rate_info))) {
        balongfb_loge("copy from user failed \n");
        return -EINVAL;
    }

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    balongfd->ade_set_core_rate = info.clock_rate;

    //idle release core rate
    if((PANEL_MIPI_VIDEO == ade_pri_data->lcd_type)
        && ((info.command & SET_CORE_RATE_IMMEDIATELY) == SET_CORE_RATE_IMMEDIATELY)
        && (!balongfd->vpu_power_on)
        && (balongfd->last_ade_core_rate != balongfd->ade_set_core_rate)) {
            if (clk_set_rate(balongfd->ade_clk, balongfd->ade_set_core_rate) != 0) {
                  balongfb_loge("clk_set_rate ade_core_rate error \n");
            }
        balongfd->last_ade_core_rate = balongfd->ade_set_core_rate;
    }

    balongfb_logi_display_debugfs(" balongfd->ade_set_core_rate:%d\n", balongfd->ade_set_core_rate);

    return ret;
}

void vpu_on_notice_ade(void)
{
    struct balong_fb_data_type     *balongfd;
    BUG_ON(g_ade_pri_data == NULL);

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(g_ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    balongfd->vpu_power_on = true;
    balongfb_logi_display_debugfs("balongfd->vpu_power_on =%d\n", balongfd->vpu_power_on);

}

void vpu_off_notice_ade(void)
{
    struct balong_fb_data_type     *balongfd;
    BUG_ON(g_ade_pri_data == NULL);

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(g_ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    balongfd->vpu_power_on = false;
    balongfb_logi_display_debugfs("balongfd->vpu_power_on =%d\n", balongfd->vpu_power_on);
}

//state: 0, suspend; 1, resume.
void vpu_pm_notify_ade(int state)
{
    struct balong_fb_data_type     *balongfd;
    BUG_ON(g_ade_pri_data == NULL);

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(g_ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    balongfb_loge("enter succ! balongfd->codec_pm_state =%d,state = %d\n", balongfd->codec_pm_state, state);
    balongfd->codec_pm_state = state;

    if (!balongfd->panel_power_on && !state) {
        balongfb_loge("panel_power_on is power off \n");
        ade_fb_suspend(balongfd->fbi);
        balongfd->codec_pm_state = -1;
        return;
    }

    /* resume ade, must power on ade first */
    if (state) {
        ade_fb_resume(balongfd->fbi);
        balongfd->codec_pm_state = -1;
        return;
    }
}

long balong_ade_ioctl(struct file* file, unsigned int ioctlCode,  unsigned long arg)
{
    struct ade_compose_data_type    *ade_pri_data = NULL;
    void __user *argp = (void __user *)arg;
    int ret = 0;
    sigset_t setmask;
    sigset_t oldmask;
    sigemptyset(&setmask);
    sigaddset(&setmask, SIGSTOP);
    sigaddset(&setmask, SIGCONT);

    BUG_ON(file == NULL);

    ade_pri_data = file->private_data;
    BUG_ON(ade_pri_data == NULL);

#if ADE_DEBUG_LOG_ENABLE
    if (g_debug_diable_ade) {
        balongfb_loge("resource set disable \n");
        return 0;
    }
#endif
    sigprocmask(SIG_SETMASK, &setmask, &oldmask);
    down(&balong_fb_overlay_sem);
    switch (ioctlCode) {
        case ADE_OVERLAY_RES_SET:
            ret = balong_ade_overlay_res_set(ade_pri_data, argp);
            break;
        case ADE_OVERLAY_REGION_SET:
            ret = balong_ade_overlay_region_set(ade_pri_data, argp);
            break;
        case ADE_OVERLAY_COMMIT:
            ret = balong_ade_overlay_commit(ade_pri_data, argp);
            break;
        case ADE_OVERLAY_SELF_REFS:
            ret = balong_ade_overlay_refresh(ade_pri_data, argp);
            break;
        case ADE_OVERLAY_LAYER_SET:
            ret = balong_ade_overlay_layer_set(ade_pri_data, argp);
            break;
#if ADE_MEM_SHARE_EN
        case ADE_MEM_SHARE_CODEC_PROC:
            ret = balong_ade_mem_share_test(ade_pri_data, argp);
            break;
#endif
        case ADE_CLOCK_RATE_SET:
            ret = balong_ade_clock_rate_set(ade_pri_data, argp);
            break;
        default:
            balongfb_loge("unknown ioctl (cmd=%d) received!\n", ioctlCode);
            ret = -EINVAL;
            break;
    }
    up(&balong_fb_overlay_sem);
    //sigprocmask(SIG_SETMASK, &oldmask, NULL);
    return ret;
}

#ifdef CONFIG_COMPAT
long balong_ade_compact_ioctl(struct file* file, unsigned int ioctlCode,  unsigned long arg)
{
    long ret = -ENOIOCTLCMD;
    arg = (unsigned long) compat_ptr(arg);
    ret = balong_ade_ioctl(file, ioctlCode, arg);
    return ret;
}
#endif

static struct file_operations compose_driver_fops =
{
    .owner      = THIS_MODULE,
    .open       = balong_ade_open,
    .release    = balong_ade_release,
    .unlocked_ioctl = balong_ade_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = balong_ade_compact_ioctl,
#endif
    .mmap       = NULL,
};
/********************************early suspend and late_resume *****************/
#ifdef CONFIG_HAS_EARLYSUSPEND
static void balong_compose_early_suspend(struct early_suspend *h)
{
    struct ade_compose_data_type *ade_pri_data = container_of(h, struct ade_compose_data_type, early_suspend);

    balongfb_logi("enter succ !");
    BUG_ON(ade_pri_data == NULL);

    ade_pri_data->frame_count = 0;
}

static void balong_compose_late_resume(struct early_suspend *h)
{
    struct ade_compose_data_type *ade_pri_data = container_of(h, struct ade_compose_data_type, early_suspend);

    balongfb_logi("enter succ !");
    BUG_ON(ade_pri_data == NULL);

    ade_pri_data->frame_count = 0;
}
#endif

/******************************************************************************/
STATIC int balong_compose_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct ade_compose_data_type     *ade_pri_data;
    struct balong_fb_data_type       *fb_data;

    BUG_ON(pdev == NULL);

    balongfb_logi("enter succ!");

    if (IS_ERR(compose_class)) {
        balongfb_loge("compose_class is NULL");
        return -EINVAL;
    }

    ade_pri_data = (struct ade_compose_data_type *)platform_get_drvdata(pdev);
    fb_data = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);

    ret = register_chrdev(major, DEV_ADE_COMPOSE_NAME, &compose_driver_fops);

    if (ret < 0) {
        balongfb_loge("register_chrdev is error %d", ret);
        return ret;
    }

    if (major == 0) {
        major = ret;
    }

    device_create(compose_class, NULL, MKDEV(major, 0), NULL, DEV_ADE_COMPOSE_NAME);

    /*************init private data********************************************/
    mutex_init(&ade_pri_data->ade_lock);

    ade_pri_data->have_refresh = false;
    ade_pri_data->res_switch_cmpl = 0;
    init_waitqueue_head(&ade_pri_data->wait_res_cmpl);

    ade_pri_data->ovly1_cmpl = 0;
    init_waitqueue_head(&ade_pri_data->wait_ovly1_cmpl);

    ade_pri_data->wdma1_cmpl = 0;
    init_waitqueue_head(&ade_pri_data->wait_wdma1_cmpl);

    ade_pri_data->cmdq1_cmpl = 0;
    init_waitqueue_head(&ade_pri_data->wait_cmdq1_cmpl);

    ade_pri_data->cmdq2_cmpl = 0;
    init_waitqueue_head(&ade_pri_data->wait_cmdq2_cmpl);

#ifdef CONFIG_HAS_EARLYSUSPEND
    ade_pri_data->early_suspend.suspend = balong_compose_early_suspend;
    ade_pri_data->early_suspend.resume = balong_compose_late_resume;
    ade_pri_data->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB - 2;
    register_early_suspend(&ade_pri_data->early_suspend);
#endif

    /* init ade overlay, only intialize one time */
    ade_overlay_init(&ade_pri_data->overlay_ctl);
    ade_cmdq_cmdfile_buff_init(fb_data->fb_ion_client, ade_pri_data->cmdq_list);
    ade_overlay_init_wdma2_buff(fb_data->fb_ion_client, ade_pri_data);

#if ADE_DEBUG_LOG_ENABLE
    g_debug_balongfd = fb_data;
#endif

    balongfb_logi("exit succ!");

    return 0;
}

STATIC int balong_compose_remove(struct platform_device *pdev)
{
    struct ade_compose_data_type   *ade_pri_data;
    struct balong_fb_data_type     *fb_data;
    struct cmdqueue_buff    *cmdq_list = NULL;
    struct cmdfile_buffer   *cf_buff = NULL;
    int i = 0;
    int j = 0;

    BUG_ON(compose_class == NULL);

    ade_pri_data = (struct ade_compose_data_type *)platform_get_drvdata(pdev);
    fb_data = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);

    for (i = 0; i < ADE_CMDQ_TYPE_MAX; i++) {
        cmdq_list = &ade_pri_data->cmdq_list[i];

        for (j = 0; j < ADE_CMD_FILE_BUFF_MAX; j++) {
            cf_buff = &cmdq_list->cf_list[j];
            if (cf_buff->cmd_ion_handle != NULL) {
                ion_unmap_kernel(fb_data->fb_ion_client, cf_buff->cmd_ion_handle);
                ion_free(fb_data->fb_ion_client, cf_buff->cmd_ion_handle);
            }
        }
    }
#if 0
    if(g_smmu_flag == ION_IOMMU_MODE){
        ion_free(fb_data->fb_ion_client, ade_pri_data->wdma2_ion_handle);
        ion_client_destroy(fb_data->fb_ion_client);
    }
    else{
        kfree(phys_to_virt(ade_pri_data->wdma2_phy));
    }
#endif
    memset(ade_pri_data->cmdq_list, 0, sizeof(ade_pri_data->cmdq_list));

    device_destroy(compose_class, MKDEV(major, 0));
    class_destroy(compose_class);
    unregister_chrdev(major, DEV_ADE_COMPOSE_NAME);

    return 0;
}

void balong_compose_shutdown(struct platform_device *pdev)
{
    balongfb_logi("enter succ!");

    device_destroy(compose_class, MKDEV(major, 0));
    class_destroy(compose_class);

    unregister_chrdev(major, DEV_ADE_COMPOSE_NAME);
}

/******************************************************************************/

static struct platform_driver balong_compose_driver = {
    .probe = balong_compose_probe,
    .remove = balong_compose_remove,
    .suspend = NULL,
    .resume = NULL,
    .shutdown = balong_compose_shutdown,
    .driver = {
        .name = DRV_ADE_COMPOSE_NAME,
    },
};

static void balong_compose_alloc(struct platform_device *this_dev, struct platform_device *pdev, struct balong_fb_data_type *pfd)
{
#define BYTES_PER_LONG (BITS_PER_LONG/8)
#define PADDING (BYTES_PER_LONG - (sizeof(struct ade_compose_data_type) % BYTES_PER_LONG))

    struct ade_compose_data_type *ade_data;
    char *p;
    int size = sizeof(struct ade_compose_data_type);

    BUG_ON(pdev == NULL);
    BUG_ON(pfd == NULL);
    BUG_ON(this_dev == NULL);

    size += PADDING;
    p = (char*)kzalloc(size, GFP_KERNEL);

    ade_data = (struct ade_compose_data_type *)p;

    ade_data->ade_base = pfd->ade_base;
    ade_data->parent = pdev;
    ade_data->lcd_type = pfd->panel_info.type;
    ade_data->xres = pfd->panel_info.xres;
    ade_data->yres = pfd->panel_info.yres;
    ade_data->fb_imgType = pfd->fb_imgType;
    ade_data->fb_reserved_flag = true;
#if ADE_MEM_SHARE_EN
    /* memory share init */
    ade_data->mem_share_drv_info.drv_status = MEM_SHARE_DRV_IDLE;
    ade_data->mem_share_drv_info.drv_event  = MEM_SHARE_EVENT_IDLE;
    ade_data->mem_share_drv_info.buf_size   = 0;
#endif

    atomic_set(&ade_data->ref_cnt, 0);

    g_ade_pri_data = ade_data;
    pfd->ade_pri_data = ade_data;

    platform_set_drvdata(this_dev, ade_data);
    return;
#undef PADDING
#undef BYTES_PER_LONG

}

struct platform_device* balong_compose_add_device(struct platform_device *pdev, struct balong_fb_data_type *pfd)
{
    struct platform_device *this_dev = NULL;
    int ret = 0;

    BUG_ON(pdev == NULL);
    BUG_ON(pfd == NULL);

    balongfb_logi("enter succ !");

    this_dev = platform_device_alloc(DRV_ADE_COMPOSE_NAME, 1);
    BUG_ON(this_dev == NULL);

    balong_compose_alloc(this_dev,pdev, pfd);

    ret = platform_device_add(this_dev);

    if (ret) {
        balongfb_loge("failed to platform_device_add, error=%d!\n", ret);
        platform_device_put(this_dev);
        return NULL;
    }

    balongfb_logi("exit succ !");

    return this_dev;
}
EXPORT_SYMBOL(balong_compose_add_device);

int __init balong_compose_init(void)
{
    int ret = -ENODEV;

    ret = platform_driver_register(&balong_compose_driver);
    if (ret) {
        balongfb_loge("not able to register the driver, error=%d!\n", ret);
        return ret;
    }

    compose_class = class_create(THIS_MODULE, "graphics_compose");

    if (IS_ERR(compose_class)) {
        balongfb_loge("not able to create the graphics_compose class");
        return -ENODEV;
    }

    return ret;
}

module_init(balong_compose_init);

#if ADE_DEBUG_LOG_ENABLE

int __init balong_compose_debug_init(void)
{
    debugfs_create_u32("mit1_compose", 0664, NULL, &g_command_file_wr_enable);
    debugfs_create_u32("mit1_ade_off", 0664, NULL, &g_ade_power_off_enable);
    debugfs_create_u32("mit1_ade_isr_off", 0664, NULL, &g_ade_isr_disable);
    debugfs_create_u32("mit1_offline_disable", 0664, NULL, &g_ade_offline_disable);
    debugfs_create_u32("mit1_dis_ade_ioctl", 0664, NULL, &g_debug_dis_ade_ioctl);
    return 0;
}

late_initcall(balong_compose_debug_init);

#endif


/*
 * We provide func: DRV_ADEMemAlloc(u32 memSize, pADEMemCB AllocNotifyCB)
 * For Share Memory to Codec
 * Your should enable "ADE_MEM_SHARE_EN"
 * Which included by:
 * kernel --- drivers/video/hi6210/balong_ade.h
 * bionic --- libc/kernel/common/linux/balong_ade.h
 * vendor --- hisi/confidential/g2d/ade/hwcomposer_v1/type_ade.h
 *
 */
#if ADE_MEM_SHARE_EN
int ADEMemCB_test(u32 memSize, u32 memAddress)
{
    if (memSize > 0) {
        balongfb_logi("====MemShare=====: memSize = %d, memAddress = 0x%x\n",
                memSize, memAddress);
    }
    return 0;
}

/* test case for Codec share event */
int balong_ade_mem_share_test(struct ade_compose_data_type *ade_pri_data, void __user *p)
{
    int     ret = 0;
    struct  mem_share_test      mem_share_para = {0,0};
    struct  balong_fb_data_type *balongfd = NULL;

    BUG_ON(ade_pri_data == NULL);

    if (copy_from_user(&mem_share_para, p, sizeof(struct  mem_share_test))) {
        balongfb_loge("copy from user failed \n");
        return -EINVAL;
    }

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    down(&balong_fb_blank_sem);
    if (!balongfd->panel_power_on) {
        up(&balong_fb_blank_sem);
        return -EPERM;
    }

    // alloc eventflag = 1
    if (mem_share_para.event_flag == 1) {
        ret = DRV_ADEMemAlloc(mem_share_para.mem_size, ADEMemCB_test);
        if (ret) {
            return -1;
        }
    }
    // free evenflag = 2
    if (mem_share_para.event_flag == 2) {
        DRV_ADEMemFree();
    }

    up(&balong_fb_blank_sem);
    return ret;
}

/*****************************************************************************
  send memory share event to ade hal
 *****************************************************************************/
void balong_mem_share_event_send(int event_flag)
{
    struct balong_fb_data_type *balongfd;
    char       *envp[2];
    char        buf[64];

    if (g_ade_pri_data == NULL) {
        return;
    }

    balongfd    = (struct balong_fb_data_type *)platform_get_drvdata(g_ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    memset(buf, '\0', sizeof(buf));
    snprintf(buf, sizeof(buf), "MEMSHARE_EVENT=%d", event_flag);

    envp[0]     = buf;
    envp[1]     = NULL;

    kobject_uevent_env(&balongfd->pdev->dev.kobj, KOBJ_CHANGE, envp);
}

/****************************************************************************
function:call by Codec to alloc memory
parameters:
memSize - Codec req memory size
AllocNotifyCB - callback addr
return: 0 -- succ; -1 -- fail
 ****************************************************************************/
int DRV_ADEMemAlloc(u32 memSize, pADEMemCB AllocNotifyCB)
{
    mem_share_ade_drv_info         *p_mem_share_info;
    struct balong_fb_data_type     *pfb_data;

    if (g_ade_pri_data == NULL) {
        return -EINVAL;
    }

    p_mem_share_info = &(g_ade_pri_data->mem_share_drv_info);
    BUG_ON(p_mem_share_info == NULL);

    balongfb_logi("===DRV_ADEMemAlloc IN=== memSize = %d, drv_event = %d, buf_size = %d, drv_status = %d ! \n",
            memSize, p_mem_share_info->drv_event, p_mem_share_info->buf_size, p_mem_share_info->drv_status);

    /* 1. if ADE power off, alloc fail */
    pfb_data = (struct balong_fb_data_type *)platform_get_drvdata(g_ade_pri_data->parent);
    BUG_ON(pfb_data == NULL);

    if (!pfb_data->ade_core_power_on) {
        balongfb_logi("not power on, ade_core_power_on = %d", pfb_data->ade_core_power_on);
        return -1;
    }

    /* 2. current is alloc event, new event is also alloc event: return */
    if ((p_mem_share_info->drv_event == MEM_SHARE_EVENT_ALLOC_720P)
            || (p_mem_share_info->drv_event == MEM_SHARE_EVENT_ALLOC_1080P)) {
        balongfb_loge("repeat alloc memory ! reject! \n");
        return -1;
    }

    /* 3. parameter wrong */
    if (AllocNotifyCB == NULL) {
        balongfb_loge("callback function is NULL !\n");
        return -1;
    }

    /* check memshare size */
    if ((memSize > 0) && (memSize <= MEM_SHARE_BUF_SIZE_76800B)) {
        p_mem_share_info->buf_size  = MEM_SHARE_BUF_SIZE_76800B;
        p_mem_share_info->drv_event = MEM_SHARE_EVENT_ALLOC_720P;

    } else if ((memSize > MEM_SHARE_BUF_SIZE_76800B)
            && (memSize <= MEM_SHARE_BUF_SIZE_153600B)) {
        p_mem_share_info->buf_size  = MEM_SHARE_BUF_SIZE_153600B;
        p_mem_share_info->drv_event = MEM_SHARE_EVENT_ALLOC_1080P;

    } else {
        p_mem_share_info->buf_size  = 0;
        balongfb_loge("memory size is not suitably,memSize = %d !\n", memSize);
        return -1;
    }

    /* 4. record parameter */
    p_mem_share_info->mem_share_cb  = AllocNotifyCB;

    /* send memory share event to ADE HAL */
    balong_mem_share_event_send(p_mem_share_info->drv_event);

    p_mem_share_info->drv_status    = MEM_SHARE_DRV_WAIT_ALLOC_RSP;

    return 0;
}


/****************************************************************************
function:call by Codec to free memory
parameters:
none
return: 0 -- succ; -1 -- fail
 ****************************************************************************/
void DRV_ADEMemFree(void)
{
    mem_share_ade_drv_info *p_mem_share_info;
    struct balong_fb_data_type     *balongfd;

    if (g_ade_pri_data == NULL) {
        return;
    }

    p_mem_share_info = &(g_ade_pri_data->mem_share_drv_info);

    BUG_ON(p_mem_share_info == NULL);

    balongfb_logi("===DRV_ADEMemFree IN===  buf_size = %d, drv_status = %d ! \n",
            p_mem_share_info->buf_size, p_mem_share_info->drv_status);

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(g_ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    /* current is free event, new event is also free event: return */
    if (p_mem_share_info->drv_event == MEM_SHARE_EVENT_FREE) {
        balongfb_loge("repeat free memory ! reject! \n");
        return;
    }
    p_mem_share_info->drv_event = MEM_SHARE_EVENT_FREE;

    /* send memory share event to ADE HAL */
    balong_mem_share_event_send(p_mem_share_info->drv_event);

    p_mem_share_info->drv_status = MEM_SHARE_DRV_WAIT_FREE_RSP;

    down(&balong_fb_blank_sem);
    if (!(balongfd->panel_power_on)) {
        balongfb_logw("===DRV_ADEMemFree ===  ade_fb_suspend ! \n");

        /* change ade status */
        p_mem_share_info->drv_status = MEM_SHARE_DRV_IDLE;

        /* clear mem share event */
        p_mem_share_info->drv_event = MEM_SHARE_EVENT_IDLE;

        /* clear the buf_size */
        p_mem_share_info->buf_size = 0;

        ade_fb_suspend(balongfd->fbi);
    }

    balongfd->mem_share_free = true;
    up(&balong_fb_blank_sem);

}

/*****************************************************************************
  memory share register cfg, called by frame end interrupt
 *****************************************************************************/
void balong_mem_share_result_proc(struct balong_fb_data_type *balongfd)
{
    mem_share_ade_drv_info  *p_mem_share_info;
    u32 MemShareOutEn = 0;
    u32 ShareSizeFlag = 0;

    if (g_ade_pri_data == NULL) {
        return;
    }

    p_mem_share_info = &(g_ade_pri_data->mem_share_drv_info);

    if (p_mem_share_info == NULL) {
        return;
    }

    if (p_mem_share_info->drv_status == MEM_SHARE_DRV_RCV_FREE_RSP) {

        /* change ade status */
        p_mem_share_info->drv_status = MEM_SHARE_DRV_IDLE;

        /* clear mem share event */
        p_mem_share_info->drv_event = MEM_SHARE_EVENT_IDLE;

        /* clear the buf_size */
        p_mem_share_info->buf_size = 0;

        balongfb_logw_display_debugfs("===DRV_ADEMemFree OUT=== drv_event = %d, buf_size = %d, drv_status = %d ! \n",
                p_mem_share_info->drv_event, p_mem_share_info->buf_size, p_mem_share_info->drv_status);
    } else if (p_mem_share_info->drv_status == MEM_SHARE_DRV_RCV_ALLOC_RSP) {

        MemShareOutEn = get_TOP_CTL1_rot_buf_shr_out(balongfd->ade_base);

        if (MemShareOutEn == 1) {

            ShareSizeFlag = get_TOP_CTL_rot_buf_cfg(balongfd->ade_base);

            if ((ShareSizeFlag == 3)
                    && (p_mem_share_info->drv_event == MEM_SHARE_EVENT_ALLOC_720P)) {

                p_mem_share_info->mem_share_cb(MEM_SHARE_BUF_SIZE_76800B, MEM_SHARE_BUF_PHY_ADDR);

                p_mem_share_info->drv_status = MEM_SHARE_DRV_ALLOC_SUCC;

            } else if ((ShareSizeFlag == 2)
                    && (p_mem_share_info->drv_event == MEM_SHARE_EVENT_ALLOC_1080P)) {

                p_mem_share_info->mem_share_cb(MEM_SHARE_BUF_SIZE_153600B, MEM_SHARE_BUF_PHY_ADDR);

                p_mem_share_info->drv_status = MEM_SHARE_DRV_ALLOC_SUCC;

            } else {
                balongfb_logw_display_debugfs("DRV_ADEMemAlloc MemShareout went error!!!\n");
            }

            balongfd->mem_share_free = false;
            balongfb_logw_display_debugfs("===DRV_ADEMemAlloc OUT=== drv_event = %d, buf_size = %d, drv_status = %d ! \n",
                    p_mem_share_info->drv_event, p_mem_share_info->buf_size, p_mem_share_info->drv_status);
        }
    }
}
#endif /* ADE_MEM_SHARE_EN */


