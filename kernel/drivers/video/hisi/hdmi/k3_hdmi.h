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

#ifndef __K3_HDMI_H__
#define __K3_HDMI_H__

#include <linux/switch.h>
#ifdef HDMI_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#ifdef CONFIG_CPU_FREQ_GOV_K3HOTPLUG
#include <linux/pm_qos_params.h>
#define HDMI_PM_QOS_DDR_MIN_FREQ           120000
#endif

#include "k3_edid.h"
//#include "k3_hdmi_hw.h"
#include "k3_hdmi_hw_tpi.h"

#include <linux/wakelock.h>
#include <linux/input.h>

#define HDMI_CHIP_VER   1  //release: 1

#define HDMI_PHY_PROCESS 1

#ifndef HDMI_USE_NOW
#define HDMI_USE_NOW 0
#endif
#define HDMI_FOR_CERTIFICATION     0
#define HDCP_FOR_CERTIFICATION     0

#if HDMI_FOR_CERTIFICATION
#ifdef USE_HDCP
#undef USE_HDCP
#endif
#define USE_HDCP    0

#ifdef INFO_LEVEL
#undef INFO_LEVEL
#endif
#define INFO_LEVEL  0
#endif

#ifndef USE_HDCP
#define USE_HDCP    0
#endif

#ifndef ENABLE_REG_DUMP
#define ENABLE_REG_DUMP 0
#endif

#ifndef CONFIG_HDMI_CEC_ENABLE
#define CONFIG_HDMI_CEC_ENABLE 0 //sangjiang skip cec for mhl test
#endif

#define ENABLE_MOCK_HDMI_TO_MHL      0 //release: 0
#define ENABLE_MOCK_24FPS            0 //release: 0

#define ENABLE_AUTO_ROTATE_LANDSCAPE 1 //release: 1
#define ENABLE_EDID_FAULT_TOLERANCE  1 //release: 1
#define ENABLE_CHECK_SHORT_CIRCUIT   0 //release: 1
#define HDMI_DEFAULT_PCM_PLAY        1 //release: 1

#define HDMI_USE_DEFAULT_TIMING 0
#define K3_DSS_PDP 0

typedef enum _hdmi_s3d_frame_structure {
    HDMI_S3D_FRAME_PACKING          = 0,    /* frame packing type of 3d */
    HDMI_S3D_FIELD_ALTERNATIVE      = 1,    /* field alternative type of 3d */
    HDMI_S3D_LINE_ALTERNATIVE       = 2,    /* line alternative type of 3d */
    HDMI_S3D_SIDE_BY_SIDE_FULL      = 3,    /* side by side full type of 3d */
    HDMI_S3D_L_DEPTH                = 4,    /* L depth type of 3d */
    HDMI_S3D_L_DEPTH_GP_GP_DEPTH    = 5,    /* L depth gp gp depth type of 3d */
    HDMI_S3D_TOP_TO_BOTTOM          = 6,    /* top to bottom type of 3d */
    HDMI_S3D_SIDE_BY_SIDE_HALF      = 8,    /* side by side half type of 3d */
    HDMI_2D                         = 0xFF  /* 2d type not 3d type */
} hdmi_s3d_frame_structure;

/* Subsampling types used for Sterioscopic 3D over HDMI. Below HOR
 * stands for Horizontal, QUI for Quinxcunx Subsampling, O for odd fields,
 * E for Even fields, L for left view and R for Right view */
typedef enum _hdmi_s3d_subsampling_type {
    /* horizontal subsampling with odd fields
     * from left view and even fields from the right view */
    HDMI_S3D_HOR_OL_OR = 0,
    HDMI_S3D_HOR_OL_ER = 1,
    HDMI_S3D_HOR_EL_OR = 2,
    HDMI_S3D_HOR_EL_ER = 3,
    HDMI_S3D_QUI_OL_OR = 4,
    HDMI_S3D_QUI_OL_ER = 5,
    HDMI_S3D_QUI_EL_OR = 6,
    HDMI_S3D_QUI_EL_ER = 7
} hdmi_s3d_subsampling_type;

enum {
    EDC_OUT_RGB_565 = 0,
    EDC_OUT_RGB_666,
    EDC_OUT_RGB_888,
};
enum {
    EDC_FRM_FMT_2D = 0,
    EDC_FRM_FMT_3D_SBS,
    EDC_FRM_FMT_3D_TTB,
    EDC_FRM_FMT_3D_CBC,
    EDC_FRM_FMT_3D_LBL,
    EDC_FRM_FMT_3D_QUINCUNX,
    EDC_FRM_FMT_3D_FBF,
};
enum {
    EDC_RGB = 0,
    EDC_BGR,
};

enum {
    LDI_DISP_MODE_NOT_3D_FBF = 0,
    LDI_DISP_MODE_3D_FBF,
};
typedef struct _hdmi_s3d_info {
    bool subsamp;       /* whether has subsamp_pos info */ //todo: don't use, delete it?, zhangenzhong
    hdmi_s3d_subsampling_type subsamp_pos;   /* Subsampling used in Vendor Specific Infoframe */  //todo: don't use, delete it?, zhangenzhong
    hdmi_s3d_frame_structure  type;          /* Frame Structure for the S3D Frame */
} hdmi_s3d_info;

typedef struct _hdmi_work_info {
    struct work_struct work;    /* work struct */
    u32 state;                  /* current work state */
} hdmi_work_info;

typedef enum _hdmi_work_state{
    HDMI_WORK_STATE_IDLE = 0,
    HDMI_WORK_STATE_PROCESS_MHL,
    HDMI_WORK_STATE_PROCESS_HDMI
} hdmi_work_state;

typedef struct _hdmi_device {
    /* device */
    struct platform_device *pdev;

    /* mutex */
    struct mutex lock;    /*mutex of video register set*/
    struct mutex lock_aux;/*mutex of audio register set*/

    /* hpd enable*/
    bool hpd_enabled;
    bool last_hpd_enabled;

    /* config */
    hdmi_config cfg;    /* current config */
//#if USE_PP_MODE
    hdmi_tpi_video_config tpi_v_cfg;
//#endif
    hdmi_core_infoframe_avi avi_param;
    hdmi_audio_format audio_fmt;
    hw_audio_configure audio_core_cfg;

    /* timings */
    int code;                       /* current timing code */
    int mode;                       /* current timing mode */
    hdmi_video_timings *timings;    /* current timing array */
    bool manual_set;                /* for manual set timing*/

    int deep_color;                 /* deep color bit*/
    hdmi_display_state state;       /* current display state */

    /* irq */
    struct workqueue_struct *irq_wq;        /* irq workqueue struct */
    bool irq_is_init;                       /* whether the irq has been inited*/

    /* hdmi power */
    hdmi_power_state power_state;           /* current power */
    hdmi_power_state video_power_state;     /* video power state */

    /* hdmi status */
    bool in_reset;              /* hdmi is reseting */
    bool connected;             /* cable is connected */
    bool hpd_current_state;     /* hpd state - last notification state */
    bool edid_set;              /* edid data is readed */
    bool custom_set;            /* custom set video timing code */

    /* 3d */
    bool s3d_enabled;                   /* current 3d mode is enabled */
    hdmi_s3d_frame_structure  s3d_type; /* current 3d type */
    struct switch_dev hpd_switch;       /* switch device*/

    /* video */
    int video_frame_rate;

    /*audio */
    bool audio_power;
    int sample_freq;
    int sample_size;
    bool bsio;
    int layout;
    int audiotype;
    int channel_number;
    bool audiochanged;

    bool downsample_needed;
#ifdef HDMI_HAS_EARLYSUSPEND
    /*earlay suspend*/
    struct early_suspend early_suspend;
#endif

    bool has_request_ddr;
#ifdef CONFIG_CPU_FREQ_GOV_K3HOTPLUG
    struct pm_qos_request_list qos_request;
#endif

    bool support_mhl_interface;
    hdmi_work_state work_state;


    struct wake_lock wake_lock;
    struct input_dev *rcp_dev;

    bool pp_mode_needed;
    bool dss_inited;

    bool mhl_probe_success;
} hdmi_device;

int hdmi_audio_set_param(int sample_freq, int sample_size, bool bsio, int layout, int audiotype, int channel_number);

int k3_hdmi_audio_set_power(bool audio_on);
int k3_hdmi_audio_set_param(int sample_freq, int sample_size, bool bsio, int layout, int audiotype, int channel_number);
void start_hdmi_process(void);
int hdmi_get_vsync_bycode(int code);
int hdmi_get_hsync_bycode(int code);
bool hdmi_get_cec_addr(u8* physical_addr);
bool hdmi_is_connect(void);
void hdmi_sys_lock(void);
void hdmi_sys_unlock(void);
int hdmi_control_notify_cec_cmd(char *cec_cmd);
void k3_hdmi_enable_hpd(bool enable);
#endif

