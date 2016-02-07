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
#include <linux/types.h>
#include <linux/hisi/hisi_ion.h>
#include <linux/hisi/hi6xxx-lcd_type.h>
#include <trace/trace_kernel.h>
#include <huawei_platform/log/log_jank.h>

#if defined (CONFIG_HUAWEI_DSM)
#include <huawei_platform/dsm/dsm_pub.h>
#endif

#include "balong_fb.h"
#include "balong_ade.h"
#include "ldi_reg.h"
#include "mipi_reg.h"
#include "ade_overlay.h"
#include "balong_compose.h"
#include "ade_reg_offset.h"

#include "soc_baseaddr_interface.h"
#include "soc_ao_sctrl_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_media_sctrl_interface.h"
#include "soc_pmctrl_interface.h"
#include "soc_media_noc_interface.h"
#include <linux/pwrctrl_power_state_manager.h>
#include <linux/hw_lcd_common.h>
#if	ADE_DEBUG_LOG_ENABLE
#include <linux/debugfs.h>
#include <linux/string.h>

u32 g_fb_colorbar_mod;

balongfb_debug_log_info_union g_fb_log_printk_flag = {.ul32 = 0};
#endif

//u32 g_smmu_flag = 0;
extern unsigned int cpufreq_get_fb(unsigned int cpu);
#if defined (CONFIG_HUAWEI_DSM)
static struct dsm_dev dsm_lcd = {
    .name = "dsm_lcd",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
    .fops = NULL,
    .buff_size = 1024,
};

struct dsm_client *lcd_dclient = NULL;
#endif

#define BALONG_FB_MAX_DEV_LIST 32

#define MAX_FBI_LIST 32

#define HI6521_LCD_LDO_OCP_EVENT                (0x10)

static int chipset_type = 0;
static int board_type = 0;
STATIC int balong_fb_resource_initialized;
static struct platform_device *pdev_list[BALONG_FB_MAX_DEV_LIST] = {0};

STATIC int pdev_list_cnt;
STATIC struct fb_info *fbi_list[MAX_FBI_LIST] = {0};
STATIC int fbi_list_index;

STATIC struct balong_fb_data_type *balongfd_list[MAX_FBI_LIST] = {0};
static int balongfd_list_index;

u8* balongfd_reg_base_ade;
u8* balongfd_reg_base_dsi0;
static u32 balongfd_irq_ade;
static u32 balongfd_irq_ldi0;

static u32 balong_fb_pseudo_palette[16] = {
    0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};

#define MAX_BACKLIGHT_BRIGHTNESS 255

DEFINE_SEMAPHORE(balong_fb_overlay_sem);
DEFINE_SEMAPHORE(balong_fb_backlight_sem);
DEFINE_SEMAPHORE(balong_fb_blank_sem);


#ifdef PC_UT_TEST_ON
volatile unsigned int    g_pc_ut_reg_data[0xffff];          /* used for ut reg test */
#endif

/******************************************************************************
 ** FUNCTIONS PROTOTYPES
 */
STATIC void balong_fb_set_backlight_cmd_mode(struct balong_fb_data_type *balongfd, u32 bkl_lvl);
int balong_fb_blank_sub(int blank_mode, struct fb_info *info);

extern int iommu_remap_pfn_range(struct ion_handle *handle,struct vm_area_struct *vma, unsigned long addr1,
        unsigned long pfn, unsigned long size, pgprot_t prot);

extern struct ion_device * get_ion_device(void);

extern void hi6521_lcd_ldo_clear_int(void);
extern int lcd_ldo_ocp_register_notifier(struct notifier_block *nb);

STATIC int balongfb_frc_set_state(struct fb_info *info, unsigned long *argp)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;
    char buffer[4];

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    ret = copy_from_user(buffer, argp, sizeof(buffer));
    if (ret) {
        balongfb_loge("copy from user failed \n");
        return ret;
    }

    /*  Different input values mean different application scenes as follow:
     *  1) 1:1 or 1:0 mean Video is playing or stop playing;
     *  2) 2:1 or 2:0 mean Game is playing or stop playing;
     *  3) 3:1 or 3:0 mean Benchmark is runing or stop running.
     *  4) 4:1 or 4:0 mean Webkit is running or stop running.
     *  5) 5:1 or 5:0 mean Special Game is playing or stop playing.
     *  6) ......
     */

    switch (buffer[0]-'0') {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            {
                if (buffer[2] == '0') {
                    balongfd->frc_state &= ~(1 << (buffer[0] - '0'));
                } else {
                    balongfd->frc_state |= (1 << (buffer[0] - '0'));
                }
            }
            break;

        default:
            break;
    }
    return 0;
}

STATIC int balongfb_frc_set(struct balong_fb_data_type *balongfd)
{
    //TODO: set frc
    return 0;
}

int balongfb_frc_get_fps(struct balong_fb_data_type *balongfd)
{
    int fps = 0;
    BUG_ON(balongfd == NULL);

    if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
        switch (balongfd->frc_state) {
            case BALONG_FB_FRC_GAME_PLAYING:
            case BALONG_FB_FRC_GAME_PLAYING | BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_GAME_FPS_CMD;
                break;
            case BALONG_FB_FRC_SPECIAL_GAME_PLAYING:
            case BALONG_FB_FRC_SPECIAL_GAME_PLAYING | BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_SPECIAL_GAME_FPS;
                break;
                /* Modifed for set frc, begin */
            case BALONG_FB_FRC_WEBKIT_PLAYING:
            case BALONG_FB_FRC_WEBKIT_PLAYING | BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_WEBKIT_FPS;
                break;
                /* Modifed for set frc, end */
            default:
                fps = BALONG_FB_FRC_NORMAL_FPS;
                break;
        }
    } else {
        switch (balongfd->frc_state) {
            case BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_IDLE_FPS;
                break;
            case BALONG_FB_FRC_GAME_PLAYING:
            case BALONG_FB_FRC_GAME_PLAYING | BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_GAME_FPS;
                break;
            case BALONG_FB_FRC_SPECIAL_GAME_PLAYING:
            case BALONG_FB_FRC_SPECIAL_GAME_PLAYING | BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_SPECIAL_GAME_FPS;
                break;
            case BALONG_FB_FRC_VIDEO_IN_GAME:
            case BALONG_FB_FRC_VIDEO_IN_GAME | BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_VIDEO_FPS;
                break;
            case BALONG_FB_FRC_BENCHMARK_PLAYING:
            case BALONG_FB_FRC_BENCHMARK_PLAYING | BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_BENCHMARK_FPS;
                break;
            case BALONG_FB_FRC_WEBKIT_PLAYING:
            case BALONG_FB_FRC_WEBKIT_PLAYING | BALONG_FB_FRC_IDLE_PLAYING:
                fps = BALONG_FB_FRC_WEBKIT_FPS;
                break;
            case BALONG_FB_FRC_NONE_PLAYING:
            default:
                fps = BALONG_FB_FRC_NORMAL_FPS;
                break;
        }
    }


    return fps;
}

bool balongfb_frc_prepare(struct balong_fb_data_type *balongfd)
{
    //TODO: set frc
    return false;
}

#if LCD_ESD_CHECK_SUPPORT
/*for esd check*/
#define ESD_CHECK_TIME_PERIOD		5000//500ms
static enum hrtimer_restart balongfb_esd_timeout(struct hrtimer *timer)
{
    struct balong_fb_data_type *balongfd = NULL;

    balongfd  = container_of(timer, struct balong_fb_data_type, esd_hrtimer);
    BUG_ON(balongfd == NULL);

    balongfd->esd_check = true;
    if (balongfd->panel_info.type == PANEL_MIPI_VIDEO)
    {
        wake_up_interruptible(&balongfd->esd_wait);
    }
    else
    {
        //cmd panel esd
    }
    hrtimer_start(&balongfd->esd_hrtimer, ktime_set(ESD_CHECK_TIME_PERIOD/1000,(ESD_CHECK_TIME_PERIOD%1000)*1000000), HRTIMER_MODE_REL);
    return HRTIMER_NORESTART;
}
#endif //LCD_ESD_CHECK_SUPPORT

#if PARTIAL_UPDATE
//save dirty area
STATIC void balongfb_update_dirty(struct ade_rect *dirt_rect, struct fb_info *info)
{
    int xoffset = 0, yoffset = 0, width = 0, height = 0;

    struct balong_fb_data_type *balongfd;
    balongfd = (struct balong_fb_data_type *)info->par;

    xoffset = dirt_rect->x;
    yoffset = dirt_rect->y;
    width   = dirt_rect->w;
    height  = dirt_rect->h;

    balongfd->dirty_update  = true;
    balongfd->vbp           = balongfd->panel_info.ldi.v_back_porch + balongfd->panel_info.yres - height - yoffset;
    balongfd->hbp           = balongfd->panel_info.ldi.h_back_porch + balongfd->panel_info.xres - width - xoffset;
    balongfd->xoffset       = xoffset;
    balongfd->yoffset       = yoffset;
    balongfd->xwidth        = width;
    balongfd->yheight       = height;

    balongfb_logi_display_debugfs("dirty x/y/w/h{%d,%d,%d,%d} vbp[%d],hbp[%d]\n", \
            xoffset, yoffset, width, height, \
            balongfd->vbp, balongfd->hbp);

}

STATIC int balongfb_set_update_rect(struct fb_info *info, unsigned long *argp)
{
    int ret = 0;
    struct ade_rect dirty_rect;

    BUG_ON(info == NULL);

    ret = copy_from_user(&dirty_rect, argp, sizeof(dirty_rect));
    if (ret) {
        balongfb_loge("copy from user failed!\n");
        return -EFAULT;
    }

    balongfb_update_dirty(&dirty_rect, info);

    return ret;
}

//set dirty area
void balongfb_set_display_region(struct balong_fb_data_type *balongfd)
{

    struct balong_fb_panel_data *pdata = NULL;

    pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;

    //set ldi w/h
    set_LDI_DSP_SIZE_size(balongfd->ade_base, balongfd->yheight, balongfd->xwidth);
    //set ldi hbp/vbp
    set_LDI_HRZ_CTRL0(balongfd->ade_base, balongfd->panel_info.ldi.h_front_porch, balongfd->hbp);
    set_LDI_VRT_CTRL0(balongfd->ade_base, balongfd->panel_info.ldi.v_front_porch, balongfd->vbp);

    //lcd ldi w/h
    if (pdata && pdata->set_display_region) {
        pdata->set_display_region(balongfd->pdev, balongfd->xoffset, balongfd->yoffset, balongfd->xwidth, balongfd->yheight);
    }
}
#endif

static int balongfb_get_lcd_dirty_region(struct fb_info *info, unsigned long *argp)
{
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    if (copy_to_user(argp, &(balongfd->panel_info.dirty_region_info),
        sizeof(struct lcd_dirty_region_info))) {
        balongfb_loge("copy to user fail");
        return -EFAULT;
    }

    return 0;
}

int fb_get_board_type(void)
{
    return board_type;
}

int get_chipset_type(void)
{
    return chipset_type;
}

#if LCD_ESD_CHECK_SUPPORT
#define ESD_RECOVER_MAX_COUNT      5
#define ESD_CHECK_MAX_COUNT        3

#if defined (CONFIG_HUAWEI_DSM)
int dsm_lcd_esd_recover = -1;
#endif

STATIC int balongfb_esd_recover(struct balong_fb_data_type * balongfd)
{
	struct balong_fb_panel_data *pdata = NULL;
	int ret = 0;

	pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;
	BUG_ON(pdata == NULL);

	down(&balong_fb_blank_sem);
	if (balongfd->ade_ldi_on)
	{
		/*first step, only power off mipi_dsi*/
		ret = panel_next_off(balongfd->pdev);
		if (ret != 0x00)
		{
			balongfb_loge("power off panel failed!\n");
			up(&balong_fb_blank_sem);
			return ret;
		}
		else
		{
			balongfb_logi("power off panel succ!\n");
		#if defined (CONFIG_HUAWEI_DSM)
			if (dsm_lcd_esd_recover >= 0)
			{
				dsm_client_record(lcd_dclient,"[%s]ESD recovery:panel off\n",__func__);
				dsm_lcd_esd_recover++;
			}
		#endif
		}
		/*second step, only power on mipi_dsi*/
		ret = panel_next_on(balongfd->pdev);
		if (ret != 0x00)
		{
			balongfb_loge("power on panel failed!\n");
			up(&balong_fb_blank_sem);
			return ret;
		}
		else
		{
			/*set backlight level*/
			balong_fb_set_backlight(balongfd, balongfd->bl_level);
			balongfb_logi("power on panel succ!\n");
		#if defined (CONFIG_HUAWEI_DSM)
			if (dsm_lcd_esd_recover >= 0)
			{
				dsm_client_record(lcd_dclient,"[%s]ESD recovery:panel on\n",__func__);
				dsm_lcd_esd_recover++;
			}
		#endif
		}
	}
	else
	{
		balongfb_loge("panel is off!\n");
	}
	up(&balong_fb_blank_sem);
	return ret;
}

STATIC int balongfb_esd_check_thread(void *data)
{
	struct balong_fb_data_type *balongfd = (struct balong_fb_data_type *)data;
	struct balong_fb_panel_data *pdata = NULL;
	int recover_count = 0;
	int esd_check_count = 0;

	pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;
	BUG_ON(pdata == NULL);

	while (!kthread_should_stop())
	{
		int ret = wait_event_interruptible(balongfd->esd_wait,(balongfd->esd_check == true));
		if (ret == 0 && balongfd->ade_ldi_on)
		{
			/*continue esd check 3 times fail, we recover, continue recover 5 times, we disable esd check function*/
			recover_count = 0;
			esd_check_count = 0;
		#if defined (CONFIG_HUAWEI_DSM)
			dsm_lcd_esd_recover = -1;
			if (!dsm_client_ocuppy(lcd_dclient)) {
				dsm_lcd_esd_recover++;
			}
		#endif
			while (recover_count < ESD_RECOVER_MAX_COUNT)
			{
				if (pdata->check_esd)
				{
					/*check esd*/
					if (esd_check_count < ESD_CHECK_MAX_COUNT)
					{
						ret = pdata->check_esd(balongfd->pdev);
						if (ret)
						{
							esd_check_count++;
							balongfb_loge("esd check abnormal, esd_check_count:%d\n",esd_check_count);
						}
						else
						{
							break;
						}
					}
					else
					{
						balongfb_loge("esd recover panel, recover_count:%d\n",recover_count);
						balongfb_esd_recover(balongfd);
						esd_check_count = 0;
						recover_count++;
					#if defined (CONFIG_HUAWEI_DSM)
						if (dsm_lcd_esd_recover >= 0)
						{
							dsm_client_record(lcd_dclient,"[%s]ESD recovery count: %d.\n",__func__, recover_count);
							dsm_lcd_esd_recover++;
						}
					#endif
					}
				}
				else
				{
					balongfb_loge("pdata->check_esd is NULL!\n");
				}
			}
			/*recover count equate 5, we disable esd check function*/
			if (recover_count >= ESD_RECOVER_MAX_COUNT)
			{
				hrtimer_cancel(&balongfd->esd_hrtimer);
				balongfd->panel_info.esd_enable = 0;
				balongfb_loge("esd recover %d count, disable esd function\n",ESD_RECOVER_MAX_COUNT);
			}
		#if defined (CONFIG_HUAWEI_DSM)
			if (recover_count > 0)
			{
				if (dsm_lcd_esd_recover > 0)
				{
					dsm_client_notify(lcd_dclient, DSM_LCD_ESD_RECOVERY_NO);
				}
				else if (!dsm_lcd_esd_recover)
				{
					dsm_client_unocuppy(lcd_dclient);
				}
			}
			else if (!dsm_lcd_esd_recover)
			{
				dsm_client_unocuppy(lcd_dclient);
			}
		#endif
		}
		/*wait timer wakeup*/
		balongfd->esd_check = false;
	}
	return 0;
}
#endif //LCD_ESD_CHECK_SUPPORT

STATIC int balongfb_wait_for_vsync_thread(void *data)
{
    struct balong_fb_data_type *balongfd = (struct balong_fb_data_type *)data;

    while (!kthread_should_stop()) {
        ktime_t timestamp = balongfd->vsync_info.timestamp;

        /*lint -e666 */
        int ret = wait_event_interruptible(balongfd->vsync_info.wait, ((!(ktime_equal(timestamp,balongfd->vsync_info.timestamp))) && (balongfd->vsync_info.active)));
        /*lint +e666 */
        if (ret == 0) {
            char *envp[2];
            char buf[64];
            snprintf(buf, sizeof(buf), "VSYNC=%llu",
                    ktime_to_ns(balongfd->vsync_info.timestamp));
            envp[0] = buf;
            envp[1] = NULL;
            balongfb_logi_vsync_debugfs("vsync report buf = %s balongfd->use_cmd_vsync = %d \n", buf, balongfd->use_cmd_vsync);
            kobject_uevent_env(&balongfd->pdev->dev.kobj, KOBJ_CHANGE, envp);
        }
    }

    return 0;
}

static void hisifb_vsync_ctrl_workqueue_handler(struct work_struct *work)
{
    struct balong_fb_data_type *balongfd = NULL;
    struct balongfb_vsync *vsync_info = NULL;
    unsigned long flags = 0;

    vsync_info = container_of(work, struct balongfb_vsync, vsync_ctrl_work);
    BUG_ON(vsync_info == NULL);
    balongfd = vsync_info->balongfd;
    BUG_ON(balongfd == NULL);
    BUG_ON(balongfd->ade_pri_data == NULL);

    down(&balong_fb_blank_sem);

    if (!balongfd->panel_power_on) {
        balongfb_loge("fb%d, panel is power off!", balongfd->index);
        up(&balong_fb_blank_sem);
        return;
    }

    if ((balongfd->vsync_info.active == 0)
        && (!balongfd->vsync_info.vsync_ctrl_disabled_set)
        && (balongfd->refresh == 0)) {
        spin_lock_irqsave(&(balongfd->vsync_info.spin_lock), flags);
        clear_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_FRAME_END_INT);
        if (balongfd->ldi_irq) {
            disable_irq(balongfd->ldi_irq);
            balongfd->vsync_info.vsync_ctrl_disabled_set = true;
        }
        spin_unlock_irqrestore(&(balongfd->vsync_info.spin_lock), flags);
    }
    up(&balong_fb_blank_sem);
}

#if defined (CONFIG_HUAWEI_DSM)
/*
*balongfb_debug_func - notify dmd log, if happen 3 times ldi underflow in one second, we notify dmd log
*/
static void balongfb_debug_func(struct work_struct *work)
{
    static u32 underflow_index = 0;
    static ktime_t underflow_timestamp[UNDERFLOW_EXPIRE_COUNT];
    s64 underflow_msecs = 0;
    static bool init_underflow_timestamp = false;
    int i;

    if (!init_underflow_timestamp)
    {
        underflow_timestamp[underflow_index] = ktime_get();
        underflow_index ++;
    }
    if (underflow_index == UNDERFLOW_EXPIRE_COUNT)
    {
        init_underflow_timestamp = true;
        underflow_timestamp[UNDERFLOW_EXPIRE_COUNT - 1] = ktime_get();
        underflow_msecs = ktime_to_ms(underflow_timestamp[UNDERFLOW_EXPIRE_COUNT - 1]) - ktime_to_ms(underflow_timestamp[0]);
        for(i = 0; i < UNDERFLOW_EXPIRE_COUNT - 1; i ++)
        {
            underflow_timestamp[i] = underflow_timestamp[i+1];
        }
    }

    if ((underflow_msecs <= UNDERFLOW_INTERVAL) && (underflow_msecs > 0))
    {
        balongfb_logi("abnormal, underflow times:%d, interval:%llu, expire interval:%d\n", UNDERFLOW_EXPIRE_COUNT, underflow_msecs, UNDERFLOW_INTERVAL);
    }
    else
    {
        balongfb_logi("normal, underflow times:%d, interval:%llu, expire interval:%d\n", UNDERFLOW_EXPIRE_COUNT, underflow_msecs, UNDERFLOW_INTERVAL);
        return;
    }

    if (!dsm_client_ocuppy(lcd_dclient))
    {
        dsm_client_record(lcd_dclient, "ldi underflow!\n");
        dsm_client_notify(lcd_dclient, DSM_LCD_LDI_UNDERFLOW_NO);
    }
}
#endif

/******************************************************************************/
void balongfb_te_inc(struct balong_fb_data_type *balongfd, bool te_should_enable)
{
    spin_lock(&balongfd->vsync_info.irq_lock);
    balongfd->vsync_info.te_refcount++;

    if (te_should_enable) {
        set_LDI_DSI_TE_CTRL_dsi_te_mask_en(balongfd->ade_base, 0);
    }

    spin_unlock(&balongfd->vsync_info.irq_lock);
}

void balongfb_te_dec(struct balong_fb_data_type *balongfd, bool te_should_disable)
{
    balongfb_logi_vsync_debugfs("balongfb_te_dec: te_should_disable= %d\n", te_should_disable);
    spin_lock(&balongfd->vsync_info.irq_lock);

    if (balongfd->vsync_info.te_refcount > 0)
        balongfd->vsync_info.te_refcount--;

    if (balongfd->vsync_info.te_refcount <= 0 && te_should_disable) {
        set_LDI_DSI_TE_CTRL_dsi_te_mask_en(balongfd->ade_base, 1);
    }
    spin_unlock(&balongfd->vsync_info.irq_lock);
}

STATIC enum hrtimer_restart balongfb_cmd_vsync(struct hrtimer *timer)
{
    struct balong_fb_data_type *balongfd = NULL;
    int fps = 60;

    balongfd  = container_of(timer, struct balong_fb_data_type, cmd_vsync_hrtimer);
    BUG_ON(balongfd == NULL);

    if (!balongfd->panel_power_on) {
        return HRTIMER_NORESTART;
    }

    if (balongfd->panel_info.frc_enable) {
        fps = balongfb_frc_get_fps(balongfd);

        /* Modified for set frc ,begin */
        if (balongfd->frc_state & BALONG_FB_FRC_SPECIAL_GAME_PLAYING){
            fps = CMD_FPS_SPECIAL_GAME;
        }
    } else {
        fps = BALONG_FB_FRC_NORMAL_FPS;
    }

    /* Modified for set frc ,begin */
    if (balongfd->use_cmd_vsync && balongfd->vsync_info.active && balongfd->vsync_info.thread) {
        balongfd->vsync_info.timestamp = ktime_get();
        wake_up_interruptible_all(&balongfd->vsync_info.wait);
    }

    /* for overlay idle detect (only cmd display mode) */
    if (!balongfd->vsync_info.active && balongfd->panel_info.type == PANEL_MIPI_CMD) {
        if (balongfd->ovcIdleCount < BALONG_FB_FRC_THRESHOLD) {
            balongfd->ovcIdleCount++;
            if (balongfd->ovcIdleCount == BALONG_FB_FRC_THRESHOLD) {
                clear_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_DSI_TE0_PIN_INT);
                balongfb_logi_vsync_debugfs("balongfb_cmd_vsync: TE_MASK succ! \n");
            }
        }
    } else {
        balongfd->ovcIdleCount = 0;
    }

    hrtimer_start(&balongfd->cmd_vsync_hrtimer, ktime_set(0, NSEC_PER_SEC / fps), HRTIMER_MODE_REL);
    return HRTIMER_NORESTART;
}

#if LCD_CHECK_MIPI_SUPPORT
static int balong_fb_monitor_lcd_status(struct balong_fb_data_type* balongfd)
{
    int ret = CHECK_MIPI_TR_OK;
    struct balong_fb_panel_data* pdata = NULL;

    pdata = dev_get_platdata(&balongfd->pdev->dev);
    BUG_ON(pdata == NULL);
    if ((pdata) && (pdata->check_mipi_tr))
    {
        down(&balongfd->sem);
        ret = pdata->check_mipi_tr(balongfd->pdev);
        if (ret == 0)
        {
            ret = CHECK_MIPI_TR_OK;
        }
        else
        {
            ret = CHECK_MIPI_TR_FAIL;
        }
        up(&balongfd->sem);
    }
    else
    {
        balongfb_loge("This panel can not support check mipi tr function");
        ret = CHECK_MIPI_TR_FAIL;
    }

    return ret;
}

static int balong_fb_check_mipi_tr(struct fb_info* info)
{
    int ret = CHECK_MIPI_TR_OK;
    struct balong_fb_data_type* balongfd = NULL;

    balongfd = (struct balong_fb_data_type*)info->par;
    BUG_ON(balongfd == NULL);

    if (!balongfd->panel_power_on)
    {
        balongfb_logw("system is now power off !\n");
        ret = CHECK_MIPI_TR_FAIL;
        goto check_mipi_tr_error;
    }
    ret = balong_fb_monitor_lcd_status(balongfd);
check_mipi_tr_error:
    return ret;
}

static int balong_fb_set_inversion_type(struct fb_info* info, void* argp)
{
    int ret = 0;
    struct balong_fb_panel_data* pdata = NULL;
    struct balong_fb_data_type* balongfd = NULL;
    unsigned int lcd_display_inversion = 0;

    balongfd = (struct balong_fb_data_type*)info->par;
    BUG_ON(balongfd == NULL);
    ret = copy_from_user(&lcd_display_inversion, argp, sizeof(lcd_display_inversion));
    if (ret)
    {
        printk(KERN_ERR "%s:balong_fb_set_inversion_type copy from user failed \n", __func__);
        return ret;
    }

    pdata = dev_get_platdata(&balongfd->pdev->dev);
    BUG_ON(pdata == NULL);

    if ((pdata) && (pdata->set_inversion_type))
    {
        down(&balongfd->sem);
        ret = pdata->set_inversion_type(balongfd->pdev, lcd_display_inversion);
        up(&balongfd->sem);
    }
    else
    {
        printk("This panel can not support dynamic set inversion type");
        return -EINVAL;
    }

    return ret;
}
#endif

/******************************************************************************
 ** handle isr
 */

STATIC int ldi_isr_cmd_mode(struct balong_fb_data_type *balongfd, u32 ints)
{
    u8*   ade_base;

    ade_base = balongfd->ade_base;
    if ((ints & LDI_ISR_DSI_TE0_PIN_INT) == LDI_ISR_DSI_TE0_PIN_INT) {
        /* clear TE0_PIN interrupt */
        outp32(ade_base + LDI_INT_CLR_REG, (u32)LDI_ISR_DSI_TE0_PIN_INT);

        if (!balongfd->use_cmd_vsync && balongfd->vsync_info.active && balongfd->vsync_info.thread) {
            balongfd->vsync_info.timestamp = ktime_get();
            wake_up_interruptible_all(&balongfd->vsync_info.wait);
        } else {
            clear_LDI_INT_MASK_bit(ade_base, LDI_ISR_DSI_TE0_PIN_INT);
        }
    }

    if ((ints & LDI_ISR_FRAME_END_INT) == LDI_ISR_FRAME_END_INT) {
        /* clear FRAME_END interrupt */
        outp32(ade_base + LDI_INT_CLR_REG, LDI_ISR_FRAME_END_INT);

        balongfd->cmd_bl_can_set = true;
        balongfd->update_frame = 1;
        wake_up_interruptible(&balongfd->frame_wq);

#if ADE_SYNC_SUPPORT
        spin_lock(&balongfd->refresh_lock);
        if (balongfd->refresh > 0) {
            sw_sync_timeline_inc(balongfd->timeline, balongfd->refresh);
            balongfd->refresh = 0;
        }
        spin_unlock(&balongfd->refresh_lock);
#endif

#if ADE_MEM_SHARE_EN
        balong_mem_share_result_proc(balongfd);
#endif

        if (balongfd->bl_level != balongfd->bl_level_cmd) {
            balong_fb_set_backlight_cmd_mode(balongfd, balongfd->bl_level);
        }
#if 0
        udelay(10);
        while (1) {
            phylock_status = inp32(balongfd->dsi_base + MIPIDSI_PHY_STATUS_ADDR);
            if (((phylock_status & 0xA94) == 0xA94) || delay_count > 20) {
                is_timeout = (delay_count > 20) ? true : false;
                delay_count = 0;
                break;
            } else {
                udelay(10);
                ++delay_count;
            }
        }

        if (!is_timeout) {
            set_MIPIDSI_PHY_RSTZ(0x4);
        }
#endif
    }

    /* check ldi_afifo_underflow_int interrupt */
    if ((ints & LDI_ISR_UNDER_FLOW_INT) == LDI_ISR_UNDER_FLOW_INT) {

#if defined (CONFIG_HUAWEI_DSM)
        if (!dsm_client_ocuppy(lcd_dclient)) {
            dsm_client_record(lcd_dclient, "ade_afifo_underflow_int of EDC0\
                    ldi!!!\n");
            dsm_client_notify(lcd_dclient, DSM_LCD_LDI_UNDERFLOW_NO);
        }
#endif

        clear_LDI_INT_MASK_bit(ade_base, LDI_ISR_UNDER_FLOW_INT);

        /* clear ldi interrupt */
        outp32(ade_base + LDI_INT_CLR_REG, LDI_ISR_UNDER_FLOW_INT);

        //outp32(ade_base + ADE_SOFT_RST0_REG, inp32(ade_base + ADE_RELOAD_DIS0_REG));
        //outp32(ade_base + ADE_SOFT_RST1_REG, inp32(ade_base + ADE_RELOAD_DIS1_REG));
        balongfb_logw_display_debugfs("ldi_afifo_underflow_int of ADE!!!\n");
    }

    return IRQ_HANDLED;
}

STATIC irqreturn_t ldi_isr_video_mode(struct balong_fb_data_type *balongfd, u32 ints)
{
    struct ade_compose_data_type *ade_data = NULL;

    /*
     ** check interrupt
     ** 0x1 for bas_stat_int
     ** 0x2 for bas_end_int
     */
    ade_data = balongfd->ade_pri_data;

    if ((ints & LDI_ISR_FRAME_END_INT) == LDI_ISR_FRAME_END_INT) {
        /* clear frame end interrupt */
        outp32(balongfd->ade_base + LDI_INT_CLR_REG, LDI_ISR_FRAME_END_INT);

#if ADE_SYNC_SUPPORT
        spin_lock(&balongfd->refresh_lock);
        if (balongfd->refresh > 0) {
            sw_sync_timeline_inc(balongfd->timeline, balongfd->refresh);
            balongfd->refresh = 0;
        }
        spin_unlock(&balongfd->refresh_lock);
#endif
        balongfd->update_frame = 1;
        wake_up_interruptible(&balongfd->frame_wq);

        if (balongfd->panel_info.frc_enable) {
            if (balongfb_frc_prepare(balongfd)) {
                balongfd->ldi_int_type |= FB_LDI_INT_TYPE_FRC;
            }
        }

        if (balongfd->ldi_int_type != FB_LDI_INT_TYPE_NONE) {
            set_reg(balongfd->ade_base + LDI_INT_EN_REG, 0x1, 1, 10);	 /* enable vfrontporch_end_int */
        }

        /* vsync */
        if (balongfd->vsync_info.active && balongfd->vsync_info.thread) {
            balongfd->vsync_info.timestamp = ktime_get();
            wake_up_interruptible_all(&balongfd->vsync_info.wait);
            balongfd->ovcIdleCount = 0;
        }

        /* if count is zero, skip disable ldi irq ctrl;
         * if count is positive number and reduce to zero,
         * then execute disable ldi irq ctrl.
         */
        spin_lock(&balongfd->vsync_info.spin_lock);
        if (balongfd->vsync_info.vsync_ctrl_expire_count) {
            balongfd->vsync_info.vsync_ctrl_expire_count--;
            if (balongfd->vsync_info.vsync_ctrl_expire_count == 0) {
                schedule_work(&(balongfd->vsync_info.vsync_ctrl_work));
            }
        }
        spin_unlock(&balongfd->vsync_info.spin_lock);
#if ADE_MEM_SHARE_EN
        balong_mem_share_result_proc(balongfd);
#endif
    }

    /* check vfrontporch_end_int interrupt*/
    if ((ints & LDI_ISR_VFP_END_INT) == LDI_ISR_VFP_END_INT) {
        /* clear LDI_ISR_VFP_END_INT interrupt */
        outp32(balongfd->ade_base + LDI_INT_CLR_REG, LDI_ISR_VFP_END_INT);

        if (balongfd->panel_info.frc_enable && (balongfd->ldi_int_type & FB_LDI_INT_TYPE_FRC)) {
            if (balongfb_frc_set(balongfd) < 0) {
                balongfb_loge("failed to set frc.\n");
            }
        }

        /* disable vfrontporch_end_int */
        clear_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_VFP_END_INT);
        balongfd->ldi_int_type = FB_LDI_INT_TYPE_NONE;
    }

    /* check ldi_afifo_underflow_int interrupt */
    if ((ints & LDI_ISR_UNDER_FLOW_INT) == LDI_ISR_UNDER_FLOW_INT) {

#if defined (CONFIG_HUAWEI_DSM)
        if (balongfd->balongfb_debug_workqueue)
            queue_work(balongfd->balongfb_debug_workqueue, &balongfd->balongfb_debug_work);
#endif

        clear_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_UNDER_FLOW_INT);

        /* clear ldi interrupt */
        outp32(balongfd->ade_base + LDI_INT_CLR_REG, LDI_ISR_UNDER_FLOW_INT);
        balongfb_logw_display_debugfs("ldi_afifo_underflow_int of ADE!!!\n");
    }

    return IRQ_HANDLED;
}

STATIC irqreturn_t ade_isr(int irq, void *data)
{
    struct balong_fb_data_type *balongfd = NULL;
    struct ade_compose_data_type *ade_data = NULL;
    u32 tmp0 = 0;
    u32 tmp1 = 0;
    u32 tmp2 = 0;
    u32 tmp3 = 0;
    u8* ade_base;

    balongfd = (struct balong_fb_data_type *)data;
    BUG_ON(balongfd == NULL);

    ade_data = balongfd->ade_pri_data;
    ade_base = balongfd->ade_base;

    if ((NULL == ade_data) || (!balongfd->ade_core_power_on))
        return IRQ_NONE;

    tmp0 = inp32(ade_base + INTR_MASK_STATE_CPU_0_REG);
    tmp1 = inp32(ade_base + INTR_MASK_STATE_CPU_1_REG);
    tmp2 = inp32(ade_base + INTR_SOFT_CMDQ2_0_REG);
    tmp3 = inp32(ade_base + INTR_SOFT_CMDQ2_1_REG);

    balongfb_logi_display_debugfs("ade_isr: intr_mask_state_0 = 0x%x \n", tmp0);
    balongfb_logi_display_debugfs("ade_isr: intr_mask_state_1 = 0x%x \n", tmp1);
    balongfb_logi_display_debugfs("ade_isr: intr_mask_state_2 = 0x%x \n", tmp2);
    balongfb_logi_display_debugfs("ade_isr: intr_mask_state_3 = 0x%x \n", tmp3);

    /* check DMA error */
    if ((tmp0 & ADE_ISR_DMA_ERROR) == ADE_ISR_DMA_ERROR) {
        /* clear dma_err intr */
        outp32(ade_base + INTR_CLEAR_CPU_0_REG, ADE_ISR_DMA_ERROR);

        balongfb_logi_display_debugfs("ade_isr: ADE_DMA_ERR_STAT_REG = 0x%x \n", inp32(ade_base + ADE_DMA_ERR_STAT_REG));
    }

    if ((tmp1 & ADE_ISR1_RES_SWITCH_CMPL) == ADE_ISR1_RES_SWITCH_CMPL) {
        /* clear ADE_ISR1_RES_SWITCH_CMPL intr */
        outp32(ade_base + INTR_CLEAR_CPU_1_REG, ADE_ISR1_RES_SWITCH_CMPL);

        ade_data->res_switch_cmpl = 1;
        wake_up_interruptible_all(&ade_data->wait_res_cmpl);

        /* disable res switch cmpl */
        //clear_TOP_INTR_MASK_CPU1_bit(ade_base, ADE_ISR1_RES_SWITCH_CMPL);
    }

    /* check WDMA1 cmpl */
    if ((tmp2 & ADE_WAIT_EVT_WDMA1_CMPL) == ADE_WAIT_EVT_WDMA1_CMPL) {
        ade_data->wdma1_cmpl = 1;
        wake_up_interruptible_all(&ade_data->wait_wdma1_cmpl);
    }

    if ((tmp1 & ADE_ISR1_CMDQ2_CMPL) == ADE_ISR1_CMDQ2_CMPL) {
        /* clear ADE_ISR1_CMDQ2_CMPL intr */
        outp32(ade_base + INTR_CLEAR_CPU_1_REG, ADE_ISR1_CMDQ2_CMPL);

        ade_data->cmdq2_cmpl = 1;
        wake_up_interruptible_all(&ade_data->wait_cmdq2_cmpl);

        /* ADE_ISR1_CMDQ2_CMPL */
        clear_TOP_INTR_MASK_CPU1_bit(ade_base, ADE_ISR1_CMDQ2_CMPL);
    }

    /* check ovly1 cmpl */
    if ((tmp3 & ADE_WAIT_EVT_OVLY1_CMPL) == ADE_WAIT_EVT_OVLY1_CMPL) {
        ade_data->ovly1_cmpl = 1;
        wake_up_interruptible_all(&ade_data->wait_ovly1_cmpl);
    }

    return IRQ_HANDLED;
}

STATIC irqreturn_t ldi_isr(int irq, void *data)
{
    struct balong_fb_data_type *balongfd = NULL;
    int ret = IRQ_HANDLED;
    u32 ints = 0;

    balongfd = (struct balong_fb_data_type *)data;
    BUG_ON(balongfd == NULL);

    /* ldi interrupt state */
    ints = inp32(balongfd->ade_base + LDI_MSK_INT_REG);

    /* check frm_start_int,frm_end_int, vsync */
    if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
        ret = ldi_isr_cmd_mode(balongfd, ints);
    } else {
        ret = ldi_isr_video_mode(balongfd, ints);
    }

    return IRQ_HANDLED;
}


/******************************************************************************/
STATIC int balongfb_vsync_int_set(struct fb_info *info, unsigned long *argp)
{
    int ret = 0;
    int vsync = 0;
    unsigned long flags = 0;
    bool cur_vsync_event = false;
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    ret = copy_from_user(&vsync, argp, sizeof(vsync));
    if (ret) {
        balongfb_loge("copy from user failed!\n");
        return ret;
    }

    down(&balong_fb_blank_sem);
    if (!balongfd->panel_power_on) {
        up(&balong_fb_blank_sem);
        return -EPERM;
    }

    cur_vsync_event = (vsync == 1) ? true : false;

    if (balongfd->vsync_info.active != cur_vsync_event) {
        balongfd->vsync_info.active = cur_vsync_event;
        if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
            if (!balongfd->vsync_info.active) {
                clear_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_DSI_TE0_PIN_INT);
            } else {
                balongfd->use_cmd_vsync = false;

                // Clear idle count
                balongfd->ovcIdleCount = 0;
                set_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_DSI_TE0_PIN_INT);
            }
        }

        if (balongfd->vsync_info.active) {
            /* recevied enable vsync, set VSYNC_CTRL_EXPIRE_COUNT to zero,
             * then skip disable ldi irq ctrl.
             */
            if (balongfd->panel_info.type == PANEL_MIPI_VIDEO) {

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

            if (balongfd->frc_state == BALONG_FB_FRC_IDLE_PLAYING) {
                balongfd->frc_state = BALONG_FB_FRC_NONE_PLAYING;
                balongfd->frc_threshold_count = 0;
            }
        } else {
            /* recevied disable vsync, save vsync ctrl count,
             * if count reduce to zero, execute disable ldi irq ctrl.
             */
            if (balongfd->panel_info.type == PANEL_MIPI_VIDEO) {
                if (!balongfd->vsync_info.vsync_ctrl_disabled_set) {
                    balongfd->vsync_info.vsync_ctrl_expire_count = VSYNC_CTRL_EXPIRE_COUNT;
                }
            }
            if (balongfd->frc_state == BALONG_FB_FRC_NONE_PLAYING) {
                balongfd->frc_state = BALONG_FB_FRC_IDLE_PLAYING;
            }
        }
    }

    up(&balong_fb_blank_sem);
    balongfb_logi_vsync_debugfs("balongfb_vsync_int_set:balongfd->vsync_info.active = %d \n", balongfd->vsync_info.active);

    return ret;
}

STATIC int balongfb_set_virt_disp(struct fb_info *info, unsigned long *argp)
{
    int ret = 0;
    int virt_disp_enable = 0;
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    ret = copy_from_user(&virt_disp_enable, argp, sizeof(virt_disp_enable));
    if (ret) {
        balongfb_loge("copy from user failed!\n");
        return -EFAULT;
    }

    down(&balong_fb_blank_sem);
    if (virt_disp_enable) {
        balongfd->virtual_disp_ref++;
    } else {
        if (balongfd->virtual_disp_ref > 0)
            balongfd->virtual_disp_ref--;
    }

    up(&balong_fb_blank_sem);


    /* when virtual disp is disconnecting and panel was powered down
     *  we need to power down the ADE core
     */
    if (!balongfd->panel_power_on) {
        if (!balongfd->virtual_disp_ref) {
            ade_fb_suspend(balongfd->fbi);
        } else {
            ade_fb_resume(balongfd->fbi);
        }
    }

    return ret;
}



/******************************************************************************/

STATIC void balong_fb_set_backlight_cmd_mode(struct balong_fb_data_type *balongfd, u32 bkl_lvl)
{
    struct balong_fb_panel_data *pdata = NULL;
    int ret = 0;

    BUG_ON(balongfd == NULL);

    if (!balongfd->panel_power_on) {
        return;
    }

    pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;
    if ((!pdata) || (!pdata->set_backlight)) {
        balongfb_loge("no panel operation detected!\n");
        return;
    }

    balongfb_logi_backlight_debugfs("balong_fb_set_backlight_cmd_mode: bkl_lvl=%d \n", bkl_lvl);

    balongfd->bl_level = bkl_lvl;
    ret = pdata->set_backlight(balongfd->pdev);
    if (ret == 0) {
        balongfd->bl_level_cmd = balongfd->bl_level;
    } else {
        balongfb_loge("failed to set backlight bkl_lvl =%d.\n", bkl_lvl);
    }
}

void balong_fb_set_backlight(struct balong_fb_data_type *balongfd, u32 bkl_lvl)
{
    struct balong_fb_panel_data *pdata = NULL;
    int ret = 0;
    unsigned long timeout ;
    BUG_ON(balongfd == NULL);

    if (!balongfd->panel_power_on) {
        return;
    }

    pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;
    if ((!pdata) || (!pdata->set_backlight)) {
        balongfb_loge("no panel operation detected!\n");
        return;
    }

    /*
       if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
       if (balongfd->cmd_mode_refresh) {
       balongfb_logi_backlight_debugfs(" set backlight fail  balongfd->cmd_mode_refresh = %d ! \n", balongfd->cmd_mode_refresh);
       return;
       }
       }
       */
    balongfb_logi_backlight_debugfs("balong_fb_set_backlight: bkl_lvl=%d \n", bkl_lvl);
    down(&balong_fb_backlight_sem);
    /* add for timeout print log */
    //balongfb_loge("%s: set backlight to %d balongfd->bl_level=%d\n", __func__, bkl_lvl, balongfd->bl_level_old);
    if (!balongfd->bl_level_old && bkl_lvl)
    {
        balongfb_loge("%s: set backlight to %d\n", __func__, bkl_lvl);
        lcd_pwr_status.lcd_dcm_pwr_status |= BIT(3);
        do_gettimeofday(&lcd_pwr_status.tvl_backlight);
        time_to_tm(lcd_pwr_status.tvl_backlight.tv_sec, 0, &lcd_pwr_status.tm_backlight);
    }
    balongfd->bl_level = bkl_lvl;
    timeout = jiffies + HZ / 10;
    ret = pdata->set_backlight(balongfd->pdev);
    /* add for timeout print log */
    if (!time_before(jiffies, timeout))
    {
        balongfb_loge("%s: set backlight time = %u,curfreq = %d\n",
                      __func__, jiffies_to_msecs(jiffies - timeout + HZ / 10), cpufreq_get_fb(0));
    }
    balongfd->bl_level_old = bkl_lvl;
    if (ret == 0)
    {
        balongfd->bl_level_cmd = balongfd->bl_level;
    } else {
        balongfb_loge("failed to set backlight.\n");
    }

    up(&balong_fb_backlight_sem);
}

int balong_fb_blank_sub(int blank_mode, struct fb_info *info)
{
    int ret = 0;
#if ADE_SYNC_SUPPORT
    unsigned long flags;
#endif
    bool curr_pwr_state = false;

    struct balong_fb_data_type *balongfd = NULL;
    struct balong_fb_panel_data *pdata = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;
    if ((!pdata) || (!pdata->on) || (!pdata->off)) {
        balongfb_loge("no panel operation detected!\n");
        return -ENODEV;
    }

    down(&balong_fb_blank_sem);

    switch (blank_mode) {
        case FB_BLANK_UNBLANK:

            if (!balongfd->panel_power_on)
            {
                /* add for timeout print log *///queren
                //#ifdef CONFIG_HUAWEI_DSM
                lcd_pwr_status.panel_power_on = true;
                lcd_pwr_status.lcd_dcm_pwr_status |= BIT(0);
                do_gettimeofday(&lcd_pwr_status.tvl_unblank);
                time_to_tm(lcd_pwr_status.tvl_unblank.tv_sec, 0, &lcd_pwr_status.tm_unblank);
                //#endif
                ade_fb_resume(info);
                ret = pdata->on(balongfd->pdev);
                if (ret != 0) {
                    balongfb_loge("failed to turn on sub devices!\n");
                } else {
                    balongfd->panel_power_on = true;
                    if (balongfd->panel_info.type == PANEL_MIPI_CMD)
                    {
                        hrtimer_restart(&balongfd->cmd_vsync_hrtimer);
                    }
#if LCD_ESD_CHECK_SUPPORT
                    /*for esd check*/
                    if (balongfd->panel_info.esd_enable)
                    {
                        balongfd->esd_check = false;
                        hrtimer_start(&balongfd->esd_hrtimer, ktime_set(ESD_CHECK_TIME_PERIOD / 1000, (ESD_CHECK_TIME_PERIOD % 1000) * 1000000), HRTIMER_MODE_REL);
                    }
#endif //LCD_ESD_CHECK_SUPPORT

                    /* Already disable ldi irq, then enable it */
                    if (balongfd->vsync_info.vsync_ctrl_disabled_set) {
                        /* enable ldi irq */
                        if (balongfd->ldi_irq) {
                            enable_irq(balongfd->ldi_irq);
                            balongfd->vsync_info.vsync_ctrl_disabled_set = false;
                        }
                    }
                }
            }
            break;

        case FB_BLANK_VSYNC_SUSPEND:
        case FB_BLANK_HSYNC_SUSPEND:
        case FB_BLANK_NORMAL:
        case FB_BLANK_POWERDOWN:
        default:
            if (balongfd->panel_power_on) {
#if ADE_DEBUG_LOG_ENABLE
                if (!g_ade_power_off_enable) {
                    up(&balong_fb_blank_sem);
                    return 0;
                }
#endif
                lcd_pwr_status.panel_power_on = false;
                balongfd->frame_updated = 0;
                curr_pwr_state = balongfd->panel_power_on;

                /* Already disable ldi irq, then skip this.
                 * ldi irq is enabled, so disable it.
                 */
                if (!balongfd->vsync_info.vsync_ctrl_disabled_set) {
                    /* disable ldi irq*/
                    if (balongfd->ldi_irq) {
                        disable_irq(balongfd->ldi_irq);
                        balongfd->vsync_info.vsync_ctrl_disabled_set = true;
                    }
                }
                if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
                    balongfd->cmd_mode_refresh = true;//false;
                    balongfd->cmd_bl_can_set = false;
                    balongfd->bl_level_cmd = 0;

                    /* Add for set frc, begin */
                    balongfd->use_cmd_vsync = false;
                    hrtimer_cancel(&balongfd->cmd_vsync_hrtimer);
                    /* Add for set frc, end */
                }

                /* Add for set frc, begin */
                /* Set frame rate back to 60 */
                if(balongfd->panel_info.frc_enable){
                    balongfd->frc_state = BALONG_FB_FRC_NONE_PLAYING;
                    balongfb_frc_set(balongfd);
                }
                /* Add for set frc, end*/

#if LCD_ESD_CHECK_SUPPORT
                /*for esd check*/
                if (balongfd->panel_info.esd_enable)
                {
                    balongfd->esd_check = false;
                    hrtimer_cancel(&balongfd->esd_hrtimer);
                }
#endif //LCD_ESD_CHECK_SUPPORT
                set_TOP_CTL_frm_end_start(balongfd->ade_base, 2);

                ret = pdata->off(balongfd->pdev);
                if (ret != 0) {
                    balongfb_loge("failed to turn off sub devices!\n");
                    balongfd->panel_power_on = curr_pwr_state;
                } else {
                    balongfd->panel_power_on = false;
                    if (((balongfd->mem_share_free) || (balongfd->codec_pm_state == 0))
                    &&  (0 == balongfd->virtual_disp_ref)) {
                        ade_fb_suspend(info);
                    } else {
                        //ade_fb_backup_recover(balongfd);
                        set_TOP_SOFT_RST_SEL0(balongfd->ade_base, 0);
                        set_TOP_SOFT_RST_SEL1(balongfd->ade_base, 0);
                        set_TOP_RELOAD_DIS0(balongfd->ade_base, 0xffffffff);
                        set_TOP_RELOAD_DIS1(balongfd->ade_base, 0xffffffff);
                        set_TOP_SOFT_RST1(balongfd->ade_base, 0xffffffff);
                        set_TOP_SOFT_RST0(balongfd->ade_base, 0xffffffff);
                        balongfd->ade_pri_data->frame_count = 0;
                    }

                    ade_overlay_handle_unlock(balongfd);
                }

#if ADE_SYNC_SUPPORT
                /* Release fence when power off */
                spin_lock_irqsave(&balongfd->refresh_lock, flags);
                sw_sync_timeline_inc(balongfd->timeline, 1 + balongfd->refresh);
                balongfd->refresh = 0;
                spin_unlock_irqrestore(&balongfd->refresh_lock, flags);
                balongfd->timeline_max++;
#endif
            }
            break;
    }

    up(&balong_fb_blank_sem);

    return ret;
}

u32 balong_fb_line_length(u32 xres, int bpp)
{
#ifdef CONFIG_FB_64BYTES_ODD_ALIGN
    u32 stride = ALIGN_UP(xres * bpp, 64);
    return (((stride / 64) % 2 == 0) ? (stride + 64) : stride);
#else
    return xres * bpp;
#endif
}

int balong_fb_init_varinfo(struct balong_fb_data_type *balongfd, int pixel_fmt)
{
    struct fb_info *fbi = NULL;
    struct fb_var_screeninfo *var = NULL;
    int bpp = 0;

    BUG_ON(balongfd == NULL);

    balongfb_logi("%s enter succ!\n",__func__);

    fbi = balongfd->fbi;
    var = &fbi->var;

    var->xoffset = 0;  /* Offset from virtual to visible */
    var->yoffset = 0;  /* resolution */
    var->grayscale = 0;  /* No graylevels */
    var->nonstd = 0;  /* standard pixel format */
    var->activate = FB_ACTIVATE_VBL;  /* activate it at vsync */
    var->accel_flags = 0;  /* acceleration flags */
    var->sync = 0;	 /* see FB_SYNC_* */
    var->rotate = 0;   /* angle we rotate counter clockwise */
    var->vmode = FB_VMODE_NONINTERLACED;

    switch (pixel_fmt) {
        case IMG_PIXEL_FORMAT_RGB_565:
            var->blue.offset = 0;
            var->green.offset = 5;
            var->red.offset = 11;
            var->transp.offset = 0;
            var->blue.length = 5;
            var->green.length = 6;
            var->red.length = 5;
            var->transp.length = 0;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.msb_right = 0;
            bpp = 2;
            balongfd->fb_imgType = ADE_RGB_565;
            break;
        case IMG_PIXEL_FORMAT_RGBX_8888:
            var->blue.offset = 16;
            var->green.offset = 8;
            var->red.offset = 0;
            var->transp.offset = 24;
            var->blue.length = 8;
            var->green.length = 8;
            var->red.length = 8;
            var->transp.length = 0;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.msb_right = 0;
            bpp = 4;
            balongfd->fb_imgType = ADE_XBGR_8888;
            break;
        case IMG_PIXEL_FORMAT_RGBA_8888:
            var->blue.offset = 16;
            var->green.offset = 8;
            var->red.offset = 0;
            var->transp.offset = 24;
            var->blue.length = 8;
            var->green.length = 8;
            var->red.length = 8;
            var->transp.length = 8;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.msb_right = 0;
            bpp = 4;
            balongfd->fb_imgType = ADE_ABGR_8888;
            break;
        default:
            pr_err("balongfb, %s: fb %d unkown image type!\n", __func__, balongfd->index);
            return -EINVAL;
    }

    var->xres = balongfd->panel_info.xres;
    var->yres = balongfd->panel_info.yres;
    var->xres_virtual = balongfd->panel_info.xres;
    var->yres_virtual = balongfd->panel_info.yres * BALONG_NUM_FRAMEBUFFERS;
    var->bits_per_pixel = bpp * 8;

    pr_info("%s,fbi->screen_base:0x%p \n",__func__,fbi->screen_base);
    pr_info("%s,fbi->screen_size:0x%0x \n",__func__,(u32)fbi->screen_size);
    pr_info("%s,fbi->fix.smem_start:0x%0x \n",__func__,(u32)fbi->fix.smem_start);
    pr_info("%s,fbi->fix.smem_len:%d \n",__func__,fbi->fix.smem_len);
    pr_info("%s,fbi->fix.line_length:%d \n",__func__,fbi->fix.line_length);

    pr_info("%s,fbi->var.xres_virtual:%d \n",__func__,fbi->var.xres_virtual);
    pr_info("%s,fbi->var.yres_virtual:%d \n",__func__,fbi->var.yres_virtual);
    pr_info("%s,fbi->var.xres:%d \n",__func__,fbi->var.xres);
    pr_info("%s,fbi->var.yres:%d \n",__func__,fbi->var.yres);

    pr_info("%s,fbi->var.bits_per_pixel:%d \n",__func__,fbi->var.bits_per_pixel);

    pr_info("%s exit succ!\n",__func__);

    return 0;
}


/******************************************************************************/

int balong_fb_open(struct fb_info *info, int user)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;
    struct fb_var_screeninfo *var = NULL;
    var = &info->var;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    if (!balongfd->ref_cnt) {
        ret = balong_fb_blank_sub(FB_BLANK_UNBLANK, info);
        if (ret != 0) {
            balongfb_loge("can't turn on display!\n");
            return ret;
        }
        LOG_JANK_D(JLID_KERNEL_LCD_OPEN, "%s", "JL_KERNEL_LCD_OPEN");
        balongfb_logi("index=%d, exit!\n", balongfd->index);
    }
    balongfd->ref_cnt++;

    return ret;
}

STATIC int balong_fb_release(struct fb_info *info, int user)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    if (!balongfd->ref_cnt) {
        return -EINVAL;
    }

    balongfd->ref_cnt--;
    if (!balongfd->ref_cnt) {
        ret = balong_fb_init_varinfo(balongfd, IMG_PIXEL_FORMAT_RGBA_8888);
        if (ret != 0) {
            balongfb_logi("index=%d, enter!\n", balongfd->index);
            return ret;
        }
        ret = balong_fb_blank_sub(FB_BLANK_POWERDOWN, info);
        if (ret != 0) {
            balongfb_loge("can't turn off display!\n");
            return ret;
        }
    }

    return ret;
}

STATIC int balong_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    balongfb_logi("enter succ !");

    if (var->rotate != FB_ROTATE_UR) {
        balongfb_loge("rotate %d!\n", var->rotate);
        return -EINVAL;
    }

    if (var->grayscale != info->var.grayscale) {
        balongfb_loge("grayscale %d!\n", var->grayscale);
        return -EINVAL;
    }

    switch (var->bits_per_pixel) {
        /* RGBA8888 RGBX8888*/
        case 32:
            {
                if (var->blue.offset == 0) {
                    if (var->red.offset != 16) {
                        balongfb_loge("not match EDC_RGB, bpp=32!\n");
                        return -EINVAL;
                    }
                } else if (var->blue.offset == 16) {
                    if (var->red.offset != 0) {
                        balongfb_loge("not match EDC_BGR, bpp=32!\n");
                        return -EINVAL;
                    }
                } else {
                    balongfb_loge("1 not match RGBA8888 or RGBX8888!\n");
                    return -EINVAL;
                }

                /* Check the common values for RGBA8888 and RGBX8888 */
                if ((var->green.offset != 8) ||
                        (var->transp.offset != 24) ||
                        (var->blue.length != 8) ||
                        (var->green.length != 8) ||
                        (var->red.length != 8) ||
                        ((var->transp.length != 8) && (var->transp.length != 0)) ||
                        (var->blue.msb_right != 0) ||
                        (var->green.msb_right != 0) ||
                        (var->red.msb_right != 0) ||
                        (var->transp.msb_right != 0)) {
                    balongfb_loge("2 not match RGBA8888 or RGBX8888!\n");
                    return -EINVAL;
                }
            }
            break;
            /* RGB565 RGBA5551 RGBX5551*/
        case 16:
            break;
        default:
            balongfb_loge("bits_per_pixel=%d not supported!\n", var->bits_per_pixel);
            return -EINVAL;

    }

    if ((var->xres_virtual < BALONG_FB_MIN_WIDTH) || (var->yres_virtual < BALONG_FB_MIN_HEIGHT)) {
        balongfb_loge("xres_virtual=%d yres_virtual=%d out of range!", var->xres_virtual, var->yres_virtual);
        return -EINVAL;
    }

    if (var->xoffset > (var->xres_virtual - var->xres)) {
        balongfb_loge("xoffset=%d out of range!\n", var->xoffset);
        return -EINVAL;
    }

    if (var->yoffset > (var->yres_virtual - var->yres)) {
        balongfb_loge("yoffset=%d out of range!\n", var->yoffset);
        return -EINVAL;
    }

    if (info->fix.smem_len < (balong_fb_line_length(var->xres_virtual, (var->bits_per_pixel >> 3))  * var->yres_virtual)) {
        balongfb_loge("smem_len=%d out of range!\n", info->fix.smem_len);
        return -EINVAL;
    }

    balongfb_logi("exit succ !");

    return ret;
}

STATIC int balong_fb_set_par(struct fb_info *info)
{
    struct balong_fb_data_type *balongfd = NULL;
    struct fb_var_screeninfo *var = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    balongfb_logi("enter succ !");

    var = &info->var;
    switch (var->bits_per_pixel) {
        case 32:
            {
                if (var->blue.offset == 0) {
                    balongfd->fb_bgrFmt = ADE_RGB;
                } else {
                    balongfd->fb_bgrFmt = ADE_BGR;
                }

                if (var->transp.length == 8) {
                    if (var->transp.offset == 24) {
                        if (var->blue.offset == 0) {
                            balongfd->fb_imgType = ADE_ARGB_8888;
                        } else {
                            balongfd->fb_imgType = ADE_ABGR_8888;
                        }
                    } else {
                        if (var->blue.offset == 24) {
                            balongfd->fb_imgType = ADE_BGRA_8888;
                        } else {
                            balongfd->fb_imgType = ADE_RGBA_8888;
                        }
                    }
                } else {
                    if (var->blue.offset == 0) {
                        balongfd->fb_imgType = ADE_XRGB_8888; //MSB is Blue
                    } else {
                        balongfd->fb_imgType = ADE_XBGR_8888;
                    }
                }
            }
            break;
        case 16:
            if (var->blue.offset == 0) {
                balongfd->fb_imgType = ADE_RGB_565;
            } else {
                balongfd->fb_imgType = ADE_BGR_565;
            }
            break;
        default:
            balongfb_loge("bits_per_pixel=%d not supported!\n", var->bits_per_pixel);
            return -EINVAL;
    }

    if (info->fix.xpanstep)
        info->var.xoffset = (var->xoffset / info->fix.xpanstep) * info->fix.xpanstep;

    if (info->fix.ypanstep)
        info->var.yoffset = (var->yoffset / info->fix.ypanstep) * info->fix.ypanstep;

    balongfb_logi("exit succ  balongfd->fb_imgType %d!", balongfd->fb_imgType);
    return 0;
}

STATIC int balong_fb_pan_display_cmd_mode(struct fb_var_screeninfo *var, struct fb_info *info)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;
#if ADE_SYNC_SUPPORT
    unsigned long flags;
#endif

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    ret = wait_event_interruptible_timeout(balongfd->frame_wq, balongfd->update_frame, HZ);
    if (ret <= 0) {
        balongfb_logw("balong_fb_pan_display_cmd_mode:wait_event_interruptible_timeout frame_wq is  timeout!\n");

#if defined (CONFIG_HUAWEI_DSM)
        if (!dsm_client_ocuppy(lcd_dclient)) {
            dsm_client_record(lcd_dclient, "wait_event_interruptible_timeout\n");
            dsm_client_notify(lcd_dclient, DSM_LCD_TE_TIME_OUT_ERROR_NO);
        }
#endif

        balongfd->update_frame = 0;

        if (balongfd->refresh > 0) {
            sw_sync_timeline_inc(balongfd->timeline, balongfd->refresh);
            balongfd->refresh = 0;
        }
        return -ETIME;
    }
    balongfd->update_frame = 0;

#if ADE_SYNC_SUPPORT
    spin_lock_irqsave(&balongfd->refresh_lock,flags);
    balongfd->refresh++;
    spin_unlock_irqrestore(&balongfd->refresh_lock,flags);
    balongfd->timeline_max++;
#endif

    ret = ade_fb_pan_display(var, info);
    if (ret != 0) {
        balongfb_loge("ade_fb_pan_display err!\n");
        return -EINVAL;
    }

#if PARTIAL_UPDATE
    if (true == balongfd->dirty_update) {
        balongfb_set_display_region(balongfd);
        balongfd->dirty_update = false;
    }
#endif
    //	set_MIPIDSI_PHY_RSTZ(0x7);
    set_LDI_CTRL_ldi_en(balongfd->ade_base, ADE_ENABLE);
    set_LDI_INT_MASK_bit(balongfd->ade_base, LDI_ISR_DSI_TE0_PIN_INT);

    /* enable fake vsync timer */
    if (balongfd->frc_state != BALONG_FB_FRC_IDLE_PLAYING) {
        balongfd->use_cmd_vsync = (balongfb_frc_get_fps(balongfd) < BALONG_FB_FRC_NORMAL_FPS ? true : false);
    }

    balongfb_logi_vsync_debugfs("balongfd->frc_state = %d \n", balongfd->frc_state);

    /* report vsync with timer */
    if (balongfd->use_cmd_vsync) {
        hrtimer_restart(&balongfd->cmd_vsync_hrtimer);
    } else {
        hrtimer_cancel(&balongfd->cmd_vsync_hrtimer);
    }

    return ret;
}

STATIC int balong_fb_pan_display_video_mode(struct fb_var_screeninfo *var, struct fb_info *info)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;
    unsigned long flags;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    ret = ade_fb_pan_display(var, info);
    if (ret != 0) {
        balongfb_logw("ade_fb_pan_display err!\n");
        return -EINVAL;
    }

#if ADE_SYNC_SUPPORT
    spin_lock_irqsave(&balongfd->refresh_lock,flags);
    balongfd->refresh ++;
    spin_unlock_irqrestore(&balongfd->refresh_lock,flags);
    balongfd->timeline_max++;
#endif

    set_LDI_CTRL_ldi_en(balongfd->ade_base, ADE_ENABLE);
    balongfd->ade_ldi_on = true;

    return ret;
}

int balong_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
    int ret = 0;
    u32 struct_len = sizeof(struct ovly_hnd_info) * ADE_OVERLAY_MAX_LAYERS;
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

#ifndef PC_UT_TEST_ON
#ifdef CONFIG_TRACING
    trace_dot(SF, "7", 0);
#endif
#endif

    down(&balong_fb_blank_sem);
    if (!balongfd->panel_power_on) {
        up(&balong_fb_blank_sem);
        return -EPERM;
    }

    memset(balongfd->locking_hnd, 0, struct_len);

    if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
        ret = balong_fb_pan_display_cmd_mode(var, info);
    } else {
        ret = balong_fb_pan_display_video_mode(var, info);
    }

    ade_overlay_handle_unlock(balongfd);
    memset(balongfd->locked_hnd, 0, struct_len);

    up(&balong_fb_blank_sem);
    return ret;
}

STATIC int balong_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    int ret = 0;
#if LCD_CHECK_MIPI_SUPPORT
    int mipi_tr = -1;
    struct balong_fb_data_type *balongfd = NULL;
    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
#endif
    BUG_ON(info == NULL);

    down(&balong_fb_overlay_sem);
    switch (cmd) {
        case FB_VSYNC_INT_SET:
            ret = balongfb_vsync_int_set(info, argp);
            break;
        case FB_FRC_SET:
            ret = balongfb_frc_set_state(info, argp);
            break;
#if PARTIAL_UPDATE
        case FB_SET_UPDATE_RECT:
            ret = balongfb_set_update_rect(info, argp);
            break;
#endif
        case FB_SET_VIRT_DISP:
            ret = balongfb_set_virt_disp(info, argp);
            break;
        #if LCD_CHECK_MIPI_SUPPORT
        case FB_CHECK_MIPI_TR:
            mipi_tr = balong_fb_check_mipi_tr(info);
            balongfb_loge("K3FB_CHECK_MIPI_TR received! lcd_readvalue = 0x%x\n",balongfd->lcd_readvalue);
            ret = copy_to_user(argp, &mipi_tr, sizeof(mipi_tr));
            if (ret)
            {
                printk(KERN_ERR "%s: BALONGFB_CHECK_MIPI_TR ioctl failed \n", __func__);
            }
            break;
        case FB_SET_INVERSION:
            balongfb_loge("FB_SET_INVERSION received!\n");
            ret = balong_fb_set_inversion_type(info, argp);
            break;
        #endif
        case HISIFB_LCD_DIRTY_REGION_INFO_GET:
            ret = balongfb_get_lcd_dirty_region(info, argp);
            break;

        default:
            balongfb_loge("unknown ioctl (cmd=%d) received!\n", cmd);
            ret = -EINVAL;
            break;
    }

    up(&balong_fb_overlay_sem);
    return ret;
}

STATIC int balong_fb_blank(int blank_mode, struct fb_info *info)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;

    BUG_ON(info == NULL);
    balongfd = (struct balong_fb_data_type *)info->par;
    BUG_ON(balongfd == NULL);

    balongfb_logi("blank_mode=%d, enter!\n", blank_mode);

    ret = balong_fb_blank_sub(blank_mode, info);
    if (ret != 0) {
        balongfb_loge("blank mode %d failed!\n", blank_mode);
    }

    balongfb_logi("index=%d, exit!\n", balongfd->index);

    return ret;
}

#ifdef CONFIG_COMPAT
int balong_fb_compact_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
    long ret = -ENOIOCTLCMD;
    arg = (unsigned long) compat_ptr(arg);
    ret = balong_fb_ioctl(info, cmd, arg);
    return ret;
}
#endif
/******************************************************************************/

static struct fb_ops balong_fb_ops = {
    .owner = THIS_MODULE,
    .fb_open = balong_fb_open,
    .fb_release = balong_fb_release,
    .fb_read = NULL,
    .fb_write = NULL,
    .fb_cursor = NULL,
    .fb_check_var = balong_fb_check_var,  /* vinfo check */
    .fb_set_par = balong_fb_set_par,  /* set the video mode according to info->var */
    .fb_setcolreg = NULL,  /* set color register */
    .fb_blank = balong_fb_blank, /*blank display */
    .fb_pan_display = balong_fb_pan_display,  /* pan display */
    .fb_fillrect = NULL,  /* Draws a rectangle */
    .fb_copyarea = NULL,  /* Copy data from area to another */
    .fb_imageblit = NULL,  /* Draws a image to the display */
    .fb_rotate = NULL,
    .fb_sync = NULL,  /* wait for blit idle, optional */
    .fb_ioctl = balong_fb_ioctl,  /* perform fb specific ioctl (optional) */
#ifdef CONFIG_COMPAT
    .fb_compat_ioctl = balong_fb_compact_ioctl,
#endif
    .fb_mmap  = NULL,
};

STATIC void balong_fb_set_bl_brightness(struct led_classdev *led_cdev,
        enum led_brightness value)
{
    struct balong_fb_data_type *balongfd = dev_get_drvdata(led_cdev->dev->parent);
    int bl_lvl;

    if (value > MAX_BACKLIGHT_BRIGHTNESS)
        value = MAX_BACKLIGHT_BRIGHTNESS;

    bl_lvl = value;
    balongfd->bl_level = value;
    balongfd->bl_level_sbl = value;

    if (balongfd->panel_info.bl_set_type & BL_SET_BY_PWM) {
        /*
         ** This maps android backlight level 0 to 255 into
         ** driver backlight level 0 to bl_max with rounding
         ** bl_lvl = (2 * value * balongfd->panel_info.bl_max + MAX_BACKLIGHT_BRIGHTNESS)
         ** (2 * MAX_BACKLIGHT_BRIGHTNESS);
         */
        bl_lvl = (value * balongfd->panel_info.bl_max) / MAX_BACKLIGHT_BRIGHTNESS;
        bl_lvl &= 0xFF;

        if (!bl_lvl && value)
            bl_lvl = 1;

        balongfd->bl_level = bl_lvl;
        balongfd->bl_level_sbl = bl_lvl;
    }

    if ((balongfd->panel_info.type == PANEL_MIPI_CMD) && (!balongfd->cmd_bl_can_set)) {
        balongfb_logi_backlight_debugfs(" set backlight fail  balongfd->cmd_bl_can_set = %d ! \n", balongfd->cmd_bl_can_set);
        return;
    }

    balong_fb_set_backlight(balongfd, balongfd->bl_level);
}

static struct led_classdev backlight_led = {
    .name = DEV_NAME_LCD_BKL,
    .brightness = MAX_BACKLIGHT_BRIGHTNESS,
    .brightness_set = balong_fb_set_bl_brightness,
};

#ifdef CONFIG_HAS_EARLYSUSPEND
static void balongfb_early_suspend(struct early_suspend *h)
{
    struct balong_fb_data_type *balongfd = container_of(h, struct balong_fb_data_type, early_suspend);

    BUG_ON(balongfd == NULL);

    balongfb_logi("index=%d, enter!\n", balongfd->index);

    if (balong_fb_blank_sub(FB_BLANK_POWERDOWN, balongfd->fbi) != 0) {
        balongfb_loge("failed to early suspend!\n");
    }

    balongfb_logi("index=%d, exit!\n", balongfd->index);
}

static void balongfb_late_resume(struct early_suspend *h)
{
    struct balong_fb_data_type *balongfd = container_of(h, struct balong_fb_data_type, early_suspend);

    BUG_ON(balongfd == NULL);

    balongfb_logi("index=%d, enter!\n", balongfd->index);

    if (balong_fb_blank_sub(FB_BLANK_UNBLANK, balongfd->fbi) != 0) {
        balongfb_loge("failed to late resume!\n");
    }

    balongfb_logi("index=%d, exit!\n", balongfd->index);
}
#endif
STATIC int balong_fb_init_par(struct balong_fb_data_type *balongfd, int pixel_fmt)
{
    struct fb_info *fbi = NULL;
    struct fb_var_screeninfo *var = NULL;
    struct fb_fix_screeninfo *fix = NULL;
    struct ion_device *ion_dev = NULL;
    struct ion_heap_info_data mem_data;
    unsigned long   fb_size;

    int bpp = 0;

    BUG_ON(balongfd == NULL);

    fbi = balongfd->fbi;
    fix = &fbi->fix;
    var = &fbi->var;

    fix->type_aux = 0;	/* if type == FB_TYPE_INTERLEAVED_PLANES */
    fix->type = FB_TYPE_PACKED_PIXELS;
    fix->visual = FB_VISUAL_TRUECOLOR;	/* True Color */
    fix->xpanstep = 1;
    fix->ypanstep = 1;
    fix->ywrapstep = 0;  /* No support */
    fix->mmio_start = 0;  /* No MMIO Address */
    fix->mmio_len = 0;	/* No MMIO Address */
    fix->accel = FB_ACCEL_NONE;  /* No hardware accelerator */
    fix->reserved[0] = (balongfd->panel_info.type == PANEL_MIPI_CMD) ? 1 : 0;

    var->xoffset = 0;  /* Offset from virtual to visible */
    var->yoffset = 0;  /* resolution */
    var->grayscale = 0;  /* No graylevels */
    var->nonstd = 0;  /* standard pixel format */
    var->activate = FB_ACTIVATE_VBL;  /* activate it at vsync */
    var->height = balongfd->panel_info.height;  /* height of picture in mm */
    var->width = balongfd->panel_info.width;  /* width of picture in mm */
    var->accel_flags = 0;  /* acceleration flags */
    var->sync = 0;	 /* see FB_SYNC_* */
    var->rotate = 0;   /* angle we rotate counter clockwise */
    var->vmode = FB_VMODE_NONINTERLACED;

    switch (pixel_fmt) {
        case IMG_PIXEL_FORMAT_RGB_565:
            var->blue.offset = 0;
            var->green.offset = 5;
            var->red.offset = 11;
            var->transp.offset = 0;
            var->blue.length = 5;
            var->green.length = 6;
            var->red.length = 5;
            var->transp.length = 0;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.msb_right = 0;
            bpp = 2;
            balongfd->fb_imgType = ADE_RGB_565;
            break;
        case IMG_PIXEL_FORMAT_RGBX_8888:
            var->blue.offset = 16;
            var->green.offset = 8;
            var->red.offset = 0;
            var->transp.offset = 24;
            var->blue.length = 8;
            var->green.length = 8;
            var->red.length = 8;
            var->transp.length = 0;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.msb_right = 0;
            bpp = 4;
            balongfd->fb_imgType = ADE_XBGR_8888;
            break;
        case IMG_PIXEL_FORMAT_RGBA_8888:
            var->blue.offset = 16;
            var->green.offset = 8;
            var->red.offset = 0;
            var->transp.offset = 24;
            var->blue.length = 8;
            var->green.length = 8;
            var->red.length = 8;
            var->transp.length = 8;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.msb_right = 0;
            bpp = 4;
            balongfd->fb_imgType = ADE_ABGR_8888;
            break;
        default:
            balongfb_loge("index=%d, unkown image type!\n", balongfd->index);
            return -EINVAL;
    }

    var->xres = balongfd->panel_info.xres;
    var->yres = balongfd->panel_info.yres;

    var->xres_virtual = balongfd->panel_info.xres;
    var->yres_virtual = balongfd->panel_info.yres * BALONG_NUM_FRAMEBUFFERS;
    var->bits_per_pixel = bpp * 8;

    fix->line_length = balong_fb_line_length(var->xres_virtual, bpp);
    fix->smem_len = roundup(fix->line_length * var->yres_virtual, PAGE_SIZE);
    fbi->screen_size = fix->smem_len;

    /* id field for fb app */
    snprintf(fix->id, sizeof(fix->id), "balongfb %d", balongfd->index);

    fbi->fbops = &balong_fb_ops;

    fbi->flags = FBINFO_FLAG_DEFAULT; /* FBINFO_HWACCEL_YPAN | FBINFO_HWACCEL_XPAN */
    fbi->pseudo_palette = balong_fb_pseudo_palette;

    /*also creat ion client */
    ion_dev = get_ion_device();
    balongfd->fb_ion_client = ion_client_create(ion_dev, "fb_ion");
    if (balongfd->fb_ion_client == NULL) {
        balongfb_loge("create ion client create fail \n");
        return -EINVAL;
    }


    if (0 != hisi_ion_get_heap_info(ION_FB_HEAP_ID, &mem_data)) {
        balongfb_loge("fail to get ION_FB_HEAP_ID\n");
        return -EINVAL;
    }

    if (0 == mem_data.heap_size) {
        balongfb_loge("fb reserved size 0\n");
        return -EINVAL;
    }

    fb_size = mem_data.heap_size;
    fbi->fix.smem_start = (u32)mem_data.heap_phy; //BALONG_FB_PA
    fbi->screen_size = fix->smem_len;

    if(fbi->screen_size > fb_size)
    {
        balongfb_loge("screen_size larger than reserved size 0\n");
    }

    return 0;
}
//unsigned long esd_current_time = 0;
static int recover_esd(struct notifier_block *nb,
        unsigned long event, void *data)
{
    int ret = 0;
    struct balong_fb_panel_data *pdata = NULL;
    /* primary panel */
    struct balong_fb_data_type *balongfd = balongfd_list[0];
    BUG_ON(balongfd == NULL);
    if(event == HI6521_LCD_LDO_OCP_EVENT)
    {
        down(&balong_fb_blank_sem);
        if (balongfd->ade_ldi_on)
        {
            ret = panel_next_off(balongfd->pdev);
            hi6521_lcd_ldo_clear_int();
            ret = panel_next_on(balongfd->pdev);
            balong_fb_set_backlight(balongfd, balongfd->bl_level);
            balongfb_loge("lcd ocp recover!\n");
#if defined (CONFIG_HUAWEI_DSM)
        if (!dsm_client_ocuppy(lcd_dclient))
        {
            dsm_client_record(lcd_dclient, "ESD OCP RECOVERY\n");
            dsm_client_notify(lcd_dclient, DSM_LCD_ESD_OCP_RECOVERY_NO);
        }
#endif

        }
        up(&balong_fb_blank_sem);
    }
    return 0;
}


#ifdef CONFIG_FASTBOOT_DISP_ENABLE
extern int scharger_register_notifier(struct notifier_block *nb);
STATIC int balongfb_fastboot_power_on(struct notifier_block *nb,
        unsigned long event, void *data)
{
    struct balong_fb_panel_data *pdata = NULL;
    /* primary panel */
    struct balong_fb_data_type *balongfd = balongfd_list[0];
    BUG_ON(balongfd == NULL);

    pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;
    if (pdata && pdata->set_fastboot) {
        pdata->set_fastboot(balongfd->pdev);
    }
    return 0;
}
#endif

int balong_fb_register(struct balong_fb_data_type *balongfd)
{
    int ret = 0;
#ifdef CONFIG_FASTBOOT_DISP_ENABLE
    u32 ldi_isr_enable = 0;
#endif
    struct fb_info *fbi = NULL;
    struct fb_fix_screeninfo *fix = NULL;

    BUG_ON(balongfd == NULL);

    /* fb info initialization */
    fbi = balongfd->fbi;
    fix = &fbi->fix;

    ret = balong_fb_init_par(balongfd, IMG_PIXEL_FORMAT_RGBA_8888);
    if (ret != 0) {
        balongfb_loge("index=%d, balong_fb_init_par failed!\n", balongfd->index);
        return ret;
    }

    balongfd->fb_bgrFmt = BALONGFB_DEFAULT_BGR_FORMAT;
    balongfd->ref_cnt = 0;
    balongfd->virtual_disp_ref = 0;
    balongfd->panel_power_on = false;
    balongfd->ade_core_power_on = false;
    balongfd->mem_share_free = true;
    balongfd->ade_ldi_on = false;

    sema_init(&balongfd->sem, 1);
    sema_init(&balongfd->power_on_sem, 1);
    if (!(balongfd->panel_info.bl_set_type & BL_SET_BY_PWM)) {
        balongfd->bl_level = 102;
    } else {
        balongfd->bl_level = 40;
    }
    balongfd->ldi_int_type = FB_LDI_INT_TYPE_NONE;
    balongfd->cmd_mode_refresh = true;//false;
    balongfd->cmd_bl_can_set = false;
    /* register framebuffer */
    if (register_framebuffer(fbi) < 0) {
        balongfb_loge("not able to register framebuffer %d!\n", balongfd->index);
        return -EPERM;
    }

    balongfd->update_frame = 0;
    init_waitqueue_head(&balongfd->frame_wq);


    /* request ade irq */
    snprintf(balongfd->ade_irq_name, sizeof(balongfd->ade_irq_name), "%s_ade", fix->id);
    ret = request_irq(balongfd->ade_irq, ade_isr, IRQF_SHARED, balongfd->ade_irq_name, (void *)balongfd);
    if (ret != 0) {
        balongfb_loge("index=%d unable to request ade irq\n", balongfd->index);
    }

    disable_irq(balongfd->ade_irq);

    /* register ade_irq to core 1
     *
     * TODO: to confirm balongv2_irqaffinity_register is OK or not.
     */
    //balongv2_irqaffinity_register(balongfd->ade_irq, 1);

    /* request ldi irq */
    snprintf(balongfd->ldi_irq_name, sizeof(balongfd->ldi_irq_name), "%s_ldi", fix->id);
    ret = request_irq(balongfd->ldi_irq, ldi_isr, IRQF_SHARED, balongfd->ldi_irq_name, (void *)balongfd);
    if (ret != 0) {
        balongfb_loge("index=%d unable to request ldi irq\n", balongfd->index);
    }

    disable_irq(balongfd->ldi_irq);

    /* register ldi_irq to core 1 */
    //balongv2_irqaffinity_register(balongfd->ldi_irq, 1);

    if (balongfd->panel_info.frc_enable) {
        balongfd->frc_threshold_count = 0;
        balongfd->frc_state = BALONG_FB_FRC_NONE_PLAYING;
        balongfd->frc_timestamp = jiffies;
    }

#if LCD_ESD_CHECK_SUPPORT
    /*for esd check*/
    if (balongfd->panel_info.esd_enable)
    {
        /* hrtimer for ESD timing */
        balongfd->esd_check = false;
        hrtimer_init(&balongfd->esd_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        balongfd->esd_hrtimer.function = balongfb_esd_timeout;
        hrtimer_start(&balongfd->esd_hrtimer, ktime_set(ESD_CHECK_TIME_PERIOD/1000,(ESD_CHECK_TIME_PERIOD%1000)*1000000), HRTIMER_MODE_REL);
        if (balongfd->panel_info.type == PANEL_MIPI_VIDEO)
        {
            init_waitqueue_head(&balongfd->esd_wait);
            /* Create esd check thread */
            balongfd->esd_thread = kthread_run(balongfb_esd_check_thread, balongfd, "balongfb-esd");
            if (IS_ERR(balongfd->esd_thread))
            {
                balongfb_loge("failed to run esd check thread\n");
                balongfd->esd_thread = NULL;
            }
        }
        else
        {
            //cmd panel esd check!
        }
    }
#endif //LCD_ESD_CHECK_SUPPORT

#if defined (CONFIG_HUAWEI_DSM)
    /*create work queue for ldi underflow*/
    balongfd->balongfb_debug_workqueue= create_singlethread_workqueue("balongfb_debug");
    if (!balongfd->balongfb_debug_workqueue)
    {
        balongfb_loge("create dss debug workqueue failed\n");
    }
    INIT_WORK(&balongfd->balongfb_debug_work, balongfb_debug_func);
#endif

    memset(&(balongfd->vsync_info), 0, sizeof(balongfd->vsync_info));
    spin_lock_init(&(balongfd->vsync_info.irq_lock));
    spin_lock_init(&(balongfd->vsync_info.spin_lock));

    if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
        /* Add for set frc, begin */
        balongfd->use_cmd_vsync = false;
        /* Add for set frc ,end */

        /* hrtimer for  vsync timing */
        hrtimer_init(&balongfd->cmd_vsync_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        balongfd->cmd_vsync_hrtimer.function = balongfb_cmd_vsync;
        hrtimer_start(&balongfd->cmd_vsync_hrtimer, ktime_set(0, NSEC_PER_SEC / 60), HRTIMER_MODE_REL);
    } else {
        // init_video_idle_workqueue(balongfd);
    }

    //init_frame_end_workqueue(balongfd);

    /* Vsync */
    init_waitqueue_head(&balongfd->vsync_info.wait);

    /* first init disable ldi irq, so vsync ctrl disable set TRUE */
    balongfd->vsync_info.vsync_ctrl_disabled_set = true;
    INIT_WORK(&balongfd->vsync_info.vsync_ctrl_work, hisifb_vsync_ctrl_workqueue_handler);
    balongfd->vsync_info.balongfd = balongfd;

    /* Create vsync thread */
    balongfd->vsync_info.thread = kthread_run(balongfb_wait_for_vsync_thread, balongfd, "balongfb-vsync");
    if (IS_ERR(balongfd->vsync_info.thread)) {
        balongfb_loge("failed to run vsync thread\n");
        balongfd->vsync_info.thread = NULL;
    }

#ifdef CONFIG_HAS_EARLYSUSPEND
    balongfd->early_suspend.suspend = balongfb_early_suspend;
    balongfd->early_suspend.resume = balongfb_late_resume;
    balongfd->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB - 2;
    register_early_suspend(&balongfd->early_suspend);
#endif

#if PARTIAL_UPDATE
    balongfd->dirty_update  = false;
    balongfd->xoffset       = 0;
    balongfd->yoffset       = 0;
    balongfd->xwidth        = balongfd->panel_info.xres;
    balongfd->yheight       = balongfd->panel_info.yres;
#endif

    balongfd->ade_set_core_rate     = balongfd->ade_core_rate;
    balongfd->last_ade_core_rate    = balongfd->ade_core_rate;
    balongfd->vpu_power_on          = false;
    balongfd->codec_pm_state        = -1;
    balongfd->pw_state_qos_id       = PWRCTRL_POWER_STAT_INVALID_ID;

    balongfd->fb.notifier_call = recover_esd;
    lcd_ldo_ocp_register_notifier(&balongfd->fb);

    /* boardid is NOT sft, then set CONFIG_FASTBOOT_DISP_ENABLE enable */
    if ((SFT_BOARD != fb_get_board_type())/* && (FHD != get_isfullhd())*/) {
#ifdef CONFIG_FASTBOOT_DISP_ENABLE

        balongfd->panel_power_on = true;
        balongfd->ade_core_power_on = true;
        balongfd->ref_cnt = 1;
        balongfd->virtual_disp_ref = 0;

        balongfd->nb.notifier_call = balongfb_fastboot_power_on;
        scharger_register_notifier(&balongfd->nb);

        /* mask ADE all intr */
        set_TOP_INTR_MASK_CPU0(balongfd->ade_base, 0x0);
        set_TOP_INTR_MASK_CPU1(balongfd->ade_base, 0x0);

        /* mask LDI all intr */
        set_LDI_INT_MASK(balongfd->ade_base, 0x0);

        /* enable ade irq */
        if (balongfd->ade_irq)
            enable_irq(balongfd->ade_irq);
        /* enable ldi irq */
        if (balongfd->ldi_irq) {
            enable_irq(balongfd->ldi_irq);
            balongfd->vsync_info.vsync_ctrl_disabled_set = false;
        }

        ldi_isr_enable = LDI_ISR_FRAME_END_INT | LDI_ISR_UNDER_FLOW_INT;
        /* enable ldi intr */
        set_LDI_INT_MASK(balongfd->ade_base, ldi_isr_enable);

        /* enable clk gate */
        set_TOP_CTL_clk_gate_en(balongfd->ade_base, 1);

        if(0 != clk_prepare_enable(balongfd->ade_clk)) {
            balongfb_loge("fail to clk_prepare_enable ade_clk\n");
        }

        if(0 != clk_prepare_enable(balongfd->media_noc_clk)) {
            balongfb_loge("fail to clk_prepare_enable media_noc_clk\n");
        }

        if(0 != clk_prepare_enable(balongfd->dsi_cfg_clk)) {
            balongfb_loge("fail to clk_prepare_enable dsi_cfg_clk\n");
        }

        ade_set_medianoc_for_dfs(balongfd);


        /* S1 qos request */
        if (0 != pwrctrl_request_power_state(PWRCTRL_SLEEP_ADE, PWRCTRL_SYS_STAT_S1, &balongfd->pw_state_qos_id)) {
            balongfb_loge("fail to request S1! \n");
        }

#endif
    }
    return ret;
}

/******************************************************************************/
STATIC int balong_fb_probe(struct platform_device *pdev)
{
    struct balong_fb_data_type *balongfd = NULL;
    struct resource *res = 0;
    struct device_node *np;
    int ret = 0;

    pr_info("%s enter! \n",__func__);

    if (balong_fb_resource_initialized == 0) {
        pdev->id = 0;
        np = pdev->dev.of_node;
        ret = of_property_read_u32(np, "is_hi6210", &chipset_type);
        if (ret) {
            balongfb_loge("failed to get is_hi6210!\n");
            return ret;
        }
        balongfb_loge("is_hi6210 =  %d!\n", chipset_type);

        ret = of_property_read_u32(np, "board_is_sft", &board_type);
        if (ret) {
            balongfb_loge("failed to get board_is_sft!\n");
            return ret;
        }
        balongfb_loge("board_is_sft =  %d!\n", board_type);

        res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
        if (!res) {
            balongfb_loge("failed to get irq_ade resource.\n");
            return -ENXIO;
        }
        balongfd_irq_ade = res->start;

        res = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
        if (!res) {
            balongfb_loge("failed to get irq_ldi resource.\n");
            return -ENXIO;
        }
        balongfd_irq_ldi0 = res->start;

        res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        if (!res) {
            balongfb_loge("failed to get reg_base_ade resource.\n");
            return -ENXIO;
        }

        /*获取smmu状态，0smmu关闭 1smmu打开*/
        //g_smmu_flag = hisi_ion_get_media_mode();

        //pr_info("%s smmu falg %d! \n",__func__,g_smmu_flag);

        balongfd_reg_base_ade = (u8*)ioremap(res->start,REG_ADE_IOSIZE);
        balongfd_reg_base_dsi0  = balongfd_reg_base_ade + ADE_MIPI_DSI_OFFSET;
        balong_fb_resource_initialized = 1;

        return 0;
    }

    if (!balong_fb_resource_initialized) {
        balongfb_loge("fb resource not initialized!\n");
        return -EPERM;
    }

    if (pdev_list_cnt >= BALONG_FB_MAX_DEV_LIST) {
        balongfb_loge("too many fb devices!\n");
        return -ENOMEM;
    }

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    BUG_ON(balongfd == NULL);

#if ADE_SYNC_SUPPORT
    /* ADE fence init */
    balongfd->timeline = sw_sync_timeline_create("ADE");
    balongfd->timeline_max = 1;
    balongfd->refresh = 0;
    spin_lock_init(&balongfd->refresh_lock);
#endif

	sema_init(&balongfd->mipi_read_sem, 1);

    balongfd->media_base = (u8*)ioremap(SOC_MEDIA_SCTRL_BASE_ADDR, 0x1000);
    balongfd->media_noc_base = (u8*)ioremap(SOC_MEDIANOC_SERVICE_ADE_BASE_ADDR, 0x1000);

    /* fb register */
    ret = balong_fb_register(balongfd);
    if (ret != 0) {
        balongfb_loge("fb register failed!\n");
        return ret;
    }

    /* fb sysfs create */
    hisi_fb_sysfs_create(pdev);

    /* android supports only one lcd-backlight/lcd for now */
    if (led_classdev_register(&pdev->dev, &backlight_led))
        balongfb_loge("led_classdev_register failed!\n");

    pdev_list[pdev_list_cnt++] = pdev;

#if defined (CONFIG_HUAWEI_DSM)
    if (!lcd_dclient) {
        lcd_dclient = dsm_register_client(&dsm_lcd);
    }
#endif

    return 0;
}

STATIC int balong_fb_remove(struct platform_device *pdev)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;
    struct balong_fb_panel_data *pdata = NULL;

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    if (!balongfd) {
        return 0;
    }

    balongfb_logi("index=%d, enter!\n", balongfd->index);

    pdata = (struct balong_fb_panel_data *)balongfd->pdev->dev.platform_data;
    if (!pdata) {
        balongfb_loge("balong_fb_panel_data is null!\n");
        return -ENODEV;
    }

    ret = balong_fb_blank_sub(FB_BLANK_POWERDOWN, balongfd->fbi);
    if (ret != 0) {
        balongfb_loge("can't stop the device %d!\n", balongfd->index);
    }

    if (pdata->remove) {
        ret = pdata->remove(balongfd->pdev);
        if (ret != 0) {
            balongfb_loge("no panel operation remove detected!\n");
        }
    }

    /* put ade clock */
    if (!IS_ERR(balongfd->ade_clk)) {
        clk_put(balongfd->ade_clk);
    }

    /* put ade cfg clock */
    if (!IS_ERR(balongfd->media_noc_clk)) {
        clk_put(balongfd->media_noc_clk);
    }

    /* sys clock */
    if (!IS_ERR(balongfd->sys_pll)) {
        clk_put(balongfd->sys_pll);
    }

    /* media clock */
    if (!IS_ERR(balongfd->media_pll)) {
        clk_put(balongfd->media_pll);
    }

#if 0
    if (balongfd->panel_info.sbl_enable && balongfd->sbl_wq) {
        destroy_workqueue(balongfd->sbl_wq);
        balongfd->sbl_wq = NULL;
    }

    if (balongfd->frame_end_wq) {
        destroy_workqueue(balongfd->frame_end_wq);
        balongfd->frame_end_wq = NULL;
    }
#endif

    if (balongfd->video_idle_wq) {
        destroy_workqueue(balongfd->video_idle_wq);
        balongfd->video_idle_wq = NULL;
    }

    if (balongfd->vsync_info.thread) {
        kthread_stop(balongfd->vsync_info.thread);
    }

#if ADE_SYNC_SUPPORT
    if (balongfd->timeline) {
        sync_timeline_destroy(balongfd->timeline);
        balongfd->timeline = NULL;
    }
#endif

    hisi_fb_sysfs_remove(pdev);

    led_classdev_unregister(&backlight_led);

    /* remove /dev/fb* */
    ret = unregister_framebuffer(balongfd->fbi);
    if (ret != 0) {
        balongfb_loge("can't unregister framebuffer %d!\n", balongfd->index);
    }

#if 0
    ion_unmap_kernel(balongfd->fb_ion_client, balongfd->fb_ion_handle);
    ion_free(balongfd->fb_ion_client, balongfd->fb_ion_handle);
#endif

    iounmap((void*)balongfd_reg_base_ade);
    iounmap((void*)balongfd_reg_base_dsi0);
    iounmap((void *)balongfd->media_noc_base);

    iounmap(balongfd->media_base);
    balongfb_logi("index=%d, exit!\n", balongfd->index);

    return ret;
}

STATIC void balong_fb_shutdown(struct platform_device *pdev)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
    if (!balongfd) {
        return;
    }

    balongfb_logi("index=%d, enter!\n", balongfd->index);

    ret = balong_fb_blank_sub(FB_BLANK_POWERDOWN, balongfd->fbi);
    if (ret != 0) {
        balongfb_loge("can't stop the device %d\n", balongfd->index);
    }

    balongfb_logi("index=%d, exit!\n", balongfd->index);
}

static int balong_fb_suspend(struct device *dev)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;

    balongfd = (struct balong_fb_data_type *)dev_get_drvdata(dev);
    if (!balongfd) {
        return 0;
    }

    balongfb_loge(" enter!\n");

    ret = balong_fb_blank_sub(FB_BLANK_POWERDOWN, balongfd->fbi);
    if (ret != 0) {
        balongfb_loge("failed to suspend, error=%d!\n", ret);
    }

    balongfb_loge(" enter!\n");

    return 0;
}

#if 0
static int balong_fb_resume(struct device *dev)
{
    int ret = 0;
    struct balong_fb_data_type *balongfd = NULL;

    balongfd = (struct balong_fb_data_type *)dev_get_drvdata(dev);
    if (!balongfd) {
        return 0;
    }

    balongfb_loge(" enter!\n");

    ret = balong_fb_blank_sub(FB_BLANK_UNBLANK, balongfd->fbi);
    if (ret != 0) {
        balongfb_loge("failed to resume, error=%d!\n", ret);
    }
    fb_set_suspend(balongfd->fbi, FBINFO_STATE_RUNNING);

    balongfb_loge("enter!\n");

    return ret;
}
#endif


static const struct of_device_id balong_fb_match[] = {
    {
        .compatible = "hisilicon,hi6210-fb",
        .data = NULL,
    },
    {},
};

static const struct dev_pm_ops balong_fb_pm_ops = {
    .suspend = balong_fb_suspend,
    .resume  = NULL,
};

/******************************************************************************/

static struct platform_driver balong_fb_driver = {
    .probe = balong_fb_probe,
    .remove = balong_fb_remove,
    .shutdown = balong_fb_shutdown,
    .suspend = NULL,
    .resume = NULL,
    .driver = {
        .name = DEV_NAME_FB,
        .pm = &balong_fb_pm_ops,
        .of_match_table = balong_fb_match,
    },
};

STATIC int balong_fb_clk_get(struct balong_fb_data_type *balongfd,struct platform_device *pdev)
{
    struct device_node *np;
    int  ret   = 0;
    if(balongfd == NULL) {
        return -EINVAL;
    }

    np = pdev->dev.of_node;
    balongfd->ade_clk     = clk_get(&pdev->dev, "clk_ade_core");
    balongfd->media_noc_clk = clk_get(&pdev->dev, "aclk_codec_jpeg_src");

    /*mipi dsi*/
    balongfd->dsi_cfg_clk   = clk_get(&pdev->dev, "pclk_dsi");

    /*ldi clock*/
    balongfd->ldi_clk  = clk_get(&pdev->dev, "clk_ade_pix");
    balongfd->sys_pll  = clk_get(&pdev->dev, "clk_syspll_src");
    balongfd->media_pll = clk_get(&pdev->dev, "clk_medpll_src");

    if (IS_ERR(balongfd->ade_clk)) {
        balongfb_loge("failed to get ade_clk!\n");
        return PTR_ERR(balongfd->ade_clk);
    }

    if (IS_ERR(balongfd->media_noc_clk)) {
        balongfb_loge("failed to get media_noc_clk!\n");
        return PTR_ERR(balongfd->media_noc_clk);
    }

    /*mipi dsi*/
    if (IS_ERR(balongfd->dsi_cfg_clk)) {
        balongfb_loge("failed to get dsi_cfg_clk!\n");
        return PTR_ERR(balongfd->dsi_cfg_clk);
    }

    /*ldi clock*/
    if (IS_ERR(balongfd->sys_pll)) {
        balongfb_loge("failed to get dsp_pll!\n");
        return PTR_ERR(balongfd->sys_pll);
    }

    if (IS_ERR(balongfd->media_pll)) {
        balongfb_loge("failed to get peri_pll!\n");
        return PTR_ERR(balongfd->media_pll);
    }

    if (IS_ERR(balongfd->ldi_clk)) {
        balongfb_loge("failed to get ldi_clk!\n");
        return PTR_ERR(balongfd->ldi_clk);
    }

    ret = of_property_read_u32(np, "ade_core_clk_rate", &balongfd->ade_core_rate);
    if (ret) {
        balongfb_loge("failed to get ade_core_clk_rate!\n");
        balongfd->ade_core_rate = 60000000;
    }


    ret = of_property_read_u32(np, "media_noc_clk_rate", &balongfd->media_noc_rate);
    if (ret) {
        balongfb_loge("failed to get media_noc_clk_rate!\n");
        balongfd->ade_core_rate = 40000000;
    }

    return CLK_GET_OK;
}

struct platform_device *balong_fb_add_device(struct platform_device *pdev)
{
    struct balong_fb_panel_data *pdata = NULL;
    struct platform_device *this_dev = NULL;
    struct fb_info *fbi = NULL;
    struct balong_fb_data_type *balongfd = NULL;
    u32 type = 0, id = 0, graphic_ch = 0;
    int ret = 0;

    BUG_ON(pdev == NULL);

    pdata = pdev->dev.platform_data;
    BUG_ON(pdata == NULL);

    if (fbi_list_index >= MAX_FBI_LIST) {
        balongfb_loge("no more framebuffer info list!\n");
        return NULL;
    }

    /* alloc panel device data */
    id = pdev->id;
    type = pdata->panel_info->type;
    this_dev = balong_fb_device_alloc(pdata, type, fbi_list_index, &graphic_ch);
    if (!this_dev) {
        balongfb_loge("failed to balong_fb_device_alloc!\n");
        return NULL;
    }

    /* alloc framebuffer info + par data */
    fbi = framebuffer_alloc(sizeof(struct balong_fb_data_type), NULL);
    if (fbi == NULL) {
        balongfb_loge("can't alloca framebuffer info data!\n");
        /*platform_device_put(this_dev);*/
        balong_fb_device_free(this_dev);
        return NULL;
    }

    balongfd = (struct balong_fb_data_type *)fbi->par;
    balongfd->fbi = fbi;
    balongfd->panel.type = type;
    balongfd->panel.id = id;
    balongfd->graphic_ch = graphic_ch;

    balongfd->index = fbi_list_index;
    balongfd->ade_base = balongfd_reg_base_ade;
    balongfd->ade_irq = balongfd_irq_ade;
    balongfd->ldi_irq = balongfd_irq_ldi0;

    ret = balong_fb_clk_get(balongfd,pdev);
    if( CLK_GET_OK != ret ){
        balongfb_loge("failed to get clk!\n");
        return NULL;
    }

    /* link to the latest pdev */
    balongfd->pdev = this_dev;

    balongfd_list[balongfd_list_index++] = balongfd;
    fbi_list[fbi_list_index++] = fbi;

    /* get/set panel info */
    memcpy(&balongfd->panel_info, pdata->panel_info, sizeof(struct balong_panel_info));

    /* set driver data */
    platform_set_drvdata(this_dev, balongfd);

    if (platform_device_add(this_dev)) {
        balongfb_loge("failed to platform_device_add!\n");
        /*platform_device_put(this_dev);*/
        framebuffer_release(fbi);
        balong_fb_device_free(this_dev);
        fbi_list_index--;
        return NULL;
    }

    balongfd->ade_pdev = balong_compose_add_device(this_dev, balongfd);

    if (balongfd->ade_pdev == NULL) {
        balongfb_loge("balong_compose_add_device is error!\n");
    }

    return this_dev;
}
EXPORT_SYMBOL(balong_fb_add_device);

int __init balong_fb_init(void)
{
    int ret = -ENODEV;

    ret = platform_driver_register(&balong_fb_driver);
    if (ret) {
        balongfb_loge("not able to register the driver, error=%d!\n", ret);
        return ret;
    }

    return ret;
}

module_init(balong_fb_init);


#if ADE_DEBUG_LOG_ENABLE

#define FB_DEBUG_STR_LEN_MAX  150


ssize_t balong_fb_debug_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    char  fb_debug_str[FB_DEBUG_STR_LEN_MAX] = {0};
    int   len = 0;

    if (g_fb_colorbar_mod == 1) {
        strcpy(fb_debug_str + len, "colorbar on ");
        len += 12;
    } else {
        strcpy(fb_debug_str + len, "colorbar off ");
        len += 13;
    }

    if (g_fb_log_printk_flag.bits.bit_vsync == 1) {
        strcpy(fb_debug_str + len, "vsync log enable ");
        len += 17;
    }

    if (g_fb_log_printk_flag.bits.bit_vsync == 0) {
        strcpy(fb_debug_str + len, "vsync log disable ");
        len += 18;
    }

    if (g_fb_log_printk_flag.bits.bit_display != 0) {
        strcpy(fb_debug_str + len, "display log enable ");
        len += 19;
    }

    if (g_fb_log_printk_flag.bits.bit_display == 0) {
        strcpy(fb_debug_str + len, "display log disable ");
        len += 20;
    }

    if (g_fb_log_printk_flag.bits.bit_backlight == 1) {
        strcpy(fb_debug_str + len, "backlight log enable ");
        len += 21;
    }

    if (g_fb_log_printk_flag.bits.bit_backlight == 0) {
        strcpy(fb_debug_str + len, "backlight log disable ");
        len += 22;
    }

    if (g_fb_log_printk_flag.bits.bit_topinfo == 0) {
        strcpy(fb_debug_str + len, "topinfo log disable ");
        len += 20;
    }

    if (g_fb_log_printk_flag.bits.bit_topinfo == 1) {
        strcpy(fb_debug_str + len, "topinfo log enable ");
        len += 19;
    }

    fb_debug_str[len] = 0;

    balongfb_logi("read file fb_debug_str = %s", fb_debug_str);

    return simple_read_from_buffer(user_buf, count, ppos, fb_debug_str, (size_t)len);
}

ssize_t balong_fb_debug_write (struct file *file,const char __user *user_buf, size_t count, loff_t *ppos)
{
    char  fb_debug_str[FB_DEBUG_STR_LEN_MAX] = {0};

    if (count > FB_DEBUG_STR_LEN_MAX -1) {
        balongfb_loge("str len error; count > %d",FB_DEBUG_STR_LEN_MAX);
        count = FB_DEBUG_STR_LEN_MAX -1;
    }

    if (copy_from_user(&fb_debug_str, user_buf, count)) {
        balongfb_loge("copy_from_user fail! \n");
        return -EFAULT;
    }

    fb_debug_str[count] = 0;

    balongfb_logi("echo str: %s", fb_debug_str);
    g_fb_log_printk_flag.bits.bit_display = 1;
    g_fb_log_printk_flag.bits.bit_topinfo = 1;

    if (strstr(fb_debug_str, "colorbar on") != NULL) {
        g_fb_colorbar_mod = 1;
    } else  if (strstr(fb_debug_str, "colorbar off") != NULL) {
        g_fb_colorbar_mod = 0;
    } else if (strstr(fb_debug_str, "vsync on") != NULL) {
        g_fb_log_printk_flag.bits.bit_vsync = 1;
    } else if (strstr(fb_debug_str, "vsync off") != NULL) {
        g_fb_log_printk_flag.bits.bit_vsync = 0;
    } else if (strstr(fb_debug_str, "display on 1") != NULL) {
        g_fb_log_printk_flag.bits.bit_display = 1;
    } else if (strstr(fb_debug_str, "display on 2") != NULL) {
        g_fb_log_printk_flag.bits.bit_display = 2;
    } else if (strstr(fb_debug_str, "display on 3") != NULL) {
        g_fb_log_printk_flag.bits.bit_display = 3;
    } else if (strstr(fb_debug_str, "display on 4") != NULL) {
        g_fb_log_printk_flag.bits.bit_display = 4;
    } else if (strstr(fb_debug_str, "display off") != NULL) {
        g_fb_log_printk_flag.bits.bit_display = 0;
    } else if (strstr(fb_debug_str, "all printk off") != NULL) {
        g_fb_log_printk_flag.ul32 = 0;
    } else if (strstr(fb_debug_str, "backlight on") != NULL) {
        g_fb_log_printk_flag.bits.bit_backlight = 1;
    } else if (strstr(fb_debug_str, "backlight off") != NULL) {
        g_fb_log_printk_flag.bits.bit_backlight = 0;
    } else if (strstr(fb_debug_str, "topinfo off") != NULL) {
        g_fb_log_printk_flag.bits.bit_topinfo = 0;
    } else if (strstr(fb_debug_str, "topinfo on 1") != NULL) {
        g_fb_log_printk_flag.bits.bit_topinfo = 1;
    } else if (strstr(fb_debug_str, "topinfo on 2") != NULL) {
        g_fb_log_printk_flag.bits.bit_topinfo = 2;
    } else if (strstr(fb_debug_str, "topinfo on 3") != NULL) {
        g_fb_log_printk_flag.bits.bit_topinfo = 3;
    } else if (strstr(fb_debug_str, "topinfo on 4") != NULL) {
        g_fb_log_printk_flag.bits.bit_topinfo = 4;
    }else if (strstr(fb_debug_str, "write cmdfile on") != NULL) {
        g_fb_log_printk_flag.bits.bit_wr_cmdf = 1;
    }else if (strstr(fb_debug_str, "write cmdfile off") != NULL) {
        g_fb_log_printk_flag.bits.bit_wr_cmdf = 0;
    }else{
        balongfb_loge("nothing to do");
    }

    return count;

}

struct file_operations balong_fb_debugfs_ops = {
    .read = balong_fb_debug_read,
    .write = balong_fb_debug_write,
};


int __init balong_fb_debug_init(void)
{
    debugfs_create_file("mit1_balongfb", 0664, NULL, NULL, &balong_fb_debugfs_ops);

    return 0;
}

late_initcall(balong_fb_debug_init);

#endif

