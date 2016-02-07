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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

#include "../k3_fb.h"
#include "../k3_fb_def.h"

//#include "k3_hdmi_hw.h"
#include "k3_hdmi_hw_tpi.h"
#include "k3_edid.h"
#include "k3_hdmi.h"
#include "k3_hdcp.h"
#include "k3_hdmi_log.h"
#include "k3_cec.h"

struct work_struct *mhl_work;
hdmi_device hdmi = {0};
u8 edid[HDMI_EDID_MAX_LENGTH] = {0};
u8 g_msg_print_level = 5;
extern mhl_tx_config g_mhl_tx_config;

volatile bool switch_connection = false;
volatile bool probe_complete = false;
#if ENABLE_REG_DUMP
#define HDMI_REG_LENGTH 256*8
#define HDMI_PHY_REG_LENGTH 256
static u8 hdmi_reg[HDMI_REG_LENGTH+HDMI_PHY_REG_LENGTH] = {0};
#endif

static int  hdmi_control_notify_hpd_status(bool onoff);
static int  hdmi_pw_set_video_power(hdmi_power_state v_power);
static int hdmi_pw_power_on_full(void);
static void hdmi_pw_power_off(void);
static int  reconfig(void);
static int  reset(void);
static irqreturn_t hdmi_irq_handler(int irq, void *arg);
#ifdef HDMI_HAS_EARLYSUSPEND
static void k3_hdmi_early_suspend(struct early_suspend *h);
static void k3_hdmi_late_resume(struct early_suspend *h);
#endif
extern void hi3620_pcm_hdmi_event(bool hdmiin,bool beforeNotify);
int mhl_rcp_register_input_dev(void);


static DEFINE_SPINLOCK(irqstatus_lock);

static void mhl_work_queue(struct work_struct *work)
{
    IN_FUNCTION;
    //wake_lock(&hdmi.wake_lock);
    enable_irq(hw_res.irq);
    k3_mhl_deviceisr();
    //wake_unlock(&hdmi.wake_lock);
    OUT_FUNCTION;
}

/******************************************************************************
* Function:       enable_hpd
* Description:    enable hpd will turn on hdmi device, disable will turn off it
* Data Accessed:
* Data Updated:
* Input:          true or false
* Output:
* Return:         0 for success, or error code.
* Others:
*******************************************************************************/
static int enable_hpd(bool enable)
{
    int ret = 0;

    IN_FUNCTION;
    if (!switch_connection || !probe_complete) {
        loge("enable failed.\n");
        return 0;
    }

    if(hdmi.hpd_enabled == enable) {
        logi("set hpd %d is same as current state, do nothing.\n", enable);
        OUT_FUNCTION;
        return 0;
    }

    hdmi.hpd_enabled = enable;

    /* use at least min power, keep suspended state if set */
    mutex_lock(&hdmi.lock);
    if (enable) {
        logi("set video power HDMI_POWER_MIN.\n");

        ret = hdmi_pw_set_video_power(HDMI_POWER_MIN);
        if (ret) {
            loge("set video power fail: %d.\n", ret);
        }

        if (!hw_support_mhl()) {
            hw_core_swreset_assert();
            hw_core_swreset_release();
        }

    } else {
        logi("set video power off.\n");
        hdmi.work_state = HDMI_WORK_STATE_IDLE;

        hdmi.custom_set = false;

        k3_mhl_reset_states();
        ret = hdmi_pw_set_video_power(HDMI_POWER_OFF);
        if (ret) {
            loge("set video power fail: %d.\n", ret);
        }
    }
    mutex_unlock(&hdmi.lock);

    OUT_FUNCTION;
    return ret;
}

/******************************************************************************
* Function:       update_cfg
* Description:    update hdmi config with video timing
* Data Accessed:
* Data Updated:
* Input:          video timing
* Output:         hdmi config
* Return:
* Others:
*******************************************************************************/
static void update_cfg(hdmi_config *cfg, hdmi_video_timings *timings)
{
    BUG_ON((NULL == cfg) || (NULL == timings));

    cfg->ppl = timings->x_res;
    cfg->lpp = timings->y_res;
    cfg->hbp = timings->hbp;
    cfg->hfp = timings->hfp;
    cfg->hsw = timings->hsw;
    cfg->vbp = timings->vbp;
    cfg->vfp = timings->vfp;
    cfg->vsw = timings->vsw;
    cfg->pixel_clock = timings->pixel_clock;

    return;
}

/******************************************************************************
* Function:       update_cfg_pol
* Description:    update hdmi config with timing index
* Data Accessed:
* Data Updated:
* Input:          timing index
* Output:
* Return:
* Others:
*******************************************************************************/
static void update_cfg_pol(hdmi_config *cfg, int index)
{
    BUG_ON(NULL == cfg);

    cfg->v_pol = edid_get_hvpol_byindex(index)->vsync_pol;
    cfg->h_pol = edid_get_hvpol_byindex(index)->hsync_pol;

    return;
}

/******************************************************************************
* Function:
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
/*
 * This function is used to configure Limited range/full range
 * with RGB format , with YUV format Full range is not supported
 * Please refer to section 6.6 Video quantization ranges in HDMI 1.3a
 * specification for more details.
 * Now conversion to Full range or limited range can either be done at
 * display controller or HDMI IP ,This function allows to select either
 * Please note : To convert to full range it is better to convert the video
 * in the dispc to full range as there will be no loss of data , if a
 * limited range data is sent ot HDMI and converted to Full range in HDMI
 * the data quality would not be good.
 */
 #if HDMI_USE_NOW
static void config_lr_fr(void)
{
    int ret = 0;

    if ((HDMI_CODE_TYPE_VESA == hdmi.mode)
       || ((HDMI_CODE_TYPE_CEA == hdmi.mode) && (1 == hdmi.code))) {
        ret = hw_configure_lrfr(HDMI_FULL_RANGE);
        if (ret) {
            loge("hw_configure_lrfr fail, ret: %d.\n", ret);
        }
    } else {
        ret = hw_configure_lrfr(HDMI_LIMITED_RANGE);
        if (ret) {
            loge("hw_configure_lrfr fail, ret: %d.\n", ret);
        }
    }

    return;
}
#endif
/******************************************************************************
* Function:       edid_get_timings_index
* Description:    get index of video timing from timing code
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:         timing index
* Others:
*******************************************************************************/
static int get_timings_index(void)
{
    int index = edid_get_timings_index(hdmi.mode, hdmi.code);

    if (INVALID_VALUE == index) {
        logw("the time index is invalid, mode: %d, code: %d.\n", hdmi.mode, hdmi.code);
        hdmi.mode = HDMI_DEFAULT_TIMING_MODE;
        hdmi.code = edid_get_default_code();
        index = edid_get_timings_index(hdmi.mode, hdmi.code);
    }

    return index;
}

/******************************************************************************
* Function:       get_s3d_timings_index
* Description:    get index of 3d video timing from timing code
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:         timing index
* Others:
*******************************************************************************/
static int get_s3d_timings_index(void)
{
    int index = edid_get_s3d_timings_index(hdmi.mode, hdmi.code);

    if (INVALID_VALUE == index) {
        hdmi.s3d_enabled = false;
        hdmi.mode = HDMI_DEFAULT_TIMING_MODE;
        hdmi.code = edid_get_default_code();
        index = edid_get_timings_index(hdmi.mode, hdmi.code);
    }

    return index;
}

/******************************************************************************
* Function:       set_custom_timing_code
* Description:    set code and mode to hdmi device
* Data Accessed:
* Data Updated:
* Input:          timing code and mode
* Output:
* Return:
* Others:
*******************************************************************************/
static int set_custom_timing_code(int code, int mode)
{
    int ret = 0;
    int index = INVALID_VALUE;

    /* for now only set this while on or on HPD */
    if ((hdmi.state != HDMI_DISPLAY_ACTIVE) && (!hdmi.manual_set)) {
        logw("the hdmi state is not active, can't to set it.\n");
        return -1;
    }

    if (!edid_is_valid_code(mode, code)) {
        loge("the code is invalid: m - %d, c - %d.\n", mode, code);
        return -EINVAL;
    }

    /* ignore invalid codes */
    hdmi.code = code;
    hdmi.mode = mode;

    /* index shall be valided */
    index = get_timings_index();
    hdmi.timings = edid_get_timings_byindex(index);
    if (hdmi.state == HDMI_DISPLAY_ACTIVE) {
        reset();
    }

    return ret;
}

/******************************************************************************
* Function:       hdmi_audio_set_param
* Description:    set audio parameter
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
int hdmi_audio_set_param(int sample_freq, int sample_size, bool bsio, int layout, int audiotype, int channel_number)
{
    int ret = 0;
    int i = 0;
    audio_format aformat = {0};
    bool support_pcm = false;

    IN_FUNCTION;

    logi("audio param: sample freq: %d, the sample size: %d, bsio: %d, layout: %d, audiotype: %d.\n",
        sample_freq, sample_size, bsio, layout, audiotype);
    /*if hdmi.audiochanged is true, recalculate config*/
    if ((hdmi.audiochanged == false) &&
        (hdmi.sample_freq == sample_freq) &&
        (hdmi.sample_size == sample_size) &&
        (hdmi.bsio == bsio) &&
        (hdmi.layout == layout) &&
        (hdmi.audiotype == audiotype) &&
        (hdmi.channel_number == channel_number)) {
        logi("same param: not set.\n");
        return 0;
    }

    if (!edid_get_audio_format((u8*)edid, &aformat)) {
        return 0;
    }

    hdmi.sample_freq = sample_freq;
    hdmi.sample_size =sample_size;
    hdmi.bsio = bsio;
    hdmi.layout = layout;
    hdmi.audiotype = audiotype;
    hdmi.audiochanged = true;
    hdmi.channel_number = channel_number;

    for (i = 0; i < aformat.number; i++) {
        if (0x01 == aformat.fmt[i].format) {
            support_pcm = true;
            break;
        }
    }

    switch (sample_freq) {
        case 48000:
            hdmi.audio_core_cfg.fs = FS_48000;
            hdmi.audio_core_cfg.if_fs = IF_FS_48000;
            break;
        case 44100:
            hdmi.audio_core_cfg.fs = FS_44100;
            hdmi.audio_core_cfg.if_fs = IF_FS_44100;
            break;
        case 32000:
            hdmi.audio_core_cfg.fs = FS_32000;
            hdmi.audio_core_cfg.if_fs = IF_FS_32000;
            break;
        case 96000:
            hdmi.audio_core_cfg.fs = FS_96000;
            hdmi.audio_core_cfg.if_fs = IF_FS_96000;
            if (support_pcm && 0 == (aformat.fmt[i].sampling_freq & 0x10)) {
                hdmi.downsample_needed = true;
            }
            break;
        case 192000:
            hdmi.audio_core_cfg.fs = FS_192000;
            hdmi.audio_core_cfg.if_fs = IF_FS_192000;
            if (support_pcm && 0 == (aformat.fmt[i].sampling_freq & 0x40)) {
                hdmi.downsample_needed = true;
            }
            break;
        case 176400:
            hdmi.audio_core_cfg.fs = FS_176400;
            hdmi.audio_core_cfg.if_fs = IF_FS_176400;
            if (support_pcm && 0 == (aformat.fmt[i].sampling_freq & 0x20)) {
                hdmi.downsample_needed = true;
            }
            break;
        default: {
            logi("sample freq is invalid(valid: 0 - 44.1K, 2 - 48K, 3 - 32K ,A - 96K): %d. \n", sample_freq);
            ret = -EINVAL;
            goto down;
        }
    }

    /*config sample size*/
    switch (sample_size) {
        case HDMI_SAMPLE_16BITS:
            hdmi.audio_core_cfg.i2schst_max_word_length = I2S_CHST_WORD_MAX_20;
            hdmi.audio_core_cfg.i2schst_word_length = I2S_CHST_WORD_16_BITS;
            hdmi.audio_core_cfg.i2s_in_bit_length = I2S_IN_LENGTH_16;
            hdmi.audio_core_cfg.i2s_justify = HDMI_AUDIO_JUSTIFY_LEFT;
            hdmi.audio_core_cfg.if_sample_size = IF_16BIT_PER_SAMPLE;
            hdmi.audio_fmt.sample_number = HDMI_ONEWORD_TWO_SAMPLES;
            hdmi.audio_fmt.sample_size = HDMI_SAMPLE_16BITS;
            hdmi.audio_fmt.justify = HDMI_AUDIO_JUSTIFY_LEFT;
            break;
        case HDMI_SAMPLE_24BITS:
            hdmi.audio_core_cfg.i2schst_max_word_length = I2S_CHST_WORD_MAX_24;
            hdmi.audio_core_cfg.i2schst_word_length = I2S_CHST_WORD_24_BITS;
            hdmi.audio_core_cfg.i2s_in_bit_length = I2S_IN_LENGTH_24;
            hdmi.audio_core_cfg.i2s_justify = HDMI_AUDIO_JUSTIFY_LEFT;
            hdmi.audio_core_cfg.if_sample_size = IF_24BIT_PER_SAMPLE;
            hdmi.audio_fmt.sample_number = HDMI_ONEWORD_ONE_SAMPLE;
            hdmi.audio_fmt.sample_size = HDMI_SAMPLE_24BITS;
            hdmi.audio_fmt.justify = HDMI_AUDIO_JUSTIFY_RIGHT;
            break;
        default:
            loge("input param is invalid(valid: 0 - 16bit, 1 - 24bit): %d.\n", sample_size);
            ret = -EINVAL;
            goto down;
    }

    //hdmi.audio_core_cfg.if_sample_size = IF_NO_PER_SAMPLE;
    hdmi.audio_fmt.left_before = HDMI_SAMPLE_LEFT_FIRST;

    /*set bsio layout and type*/

    if ((LAYOUT_2CH != layout) && (LAYOUT_8CH != layout)) {
        loge("input param layout invalid(valid: 0, 1): %d.\n", layout);
        ret = -EINVAL;
        goto down;
    }

    hdmi.audio_core_cfg.bsio = bsio;
    hdmi.audio_core_cfg.layout = layout;
    hdmi.audio_core_cfg.audiotype = audiotype;
    hdmi.audio_core_cfg.if_channel_number = channel_number;

down:

    OUT_FUNCTION;
    return ret;
}

/******************************************************************************
* Function:       reconfig
* Description:    reconfig hdmi device, will turn on again if it be turned on
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
static int reconfig(void)
{
    int ret = 0;

    IN_FUNCTION;

    /* don't reconfigure in power off state */
    if (HDMI_POWER_OFF == hdmi.power_state) {
        logw("don't reconfigure in power off state.\n");
        OUT_FUNCTION;
        return 0;
    }

    /* force a new power-up to read EDID */
    if (HDMI_POWER_FULL == hdmi.power_state) {
        logw("power state is min.\n");
        hdmi.power_state = HDMI_POWER_MIN;
    }

    logd("power video full.\n");
    ret = hdmi_pw_set_video_power(HDMI_POWER_FULL);

    OUT_FUNCTION;
    return ret;
}

/******************************************************************************
* Function:  reset
* Description: For timings change to take effect, phy needs to be off. Interrupts will be
*           ignored during this sequence to avoid queuing work items that generate
*           hotplug events.
* Data Accessed:
* Data Updated:
* Input:          hdmi_reset_phase -- reset state.
* Output:
* Return:
* Others:
*******************************************************************************/
static int reset(void)
{
    int ret = 0;

    IN_FUNCTION;

    if (HDMI_POWER_OFF == hdmi.power_state) {
        loge("the power state is OFF and it is invalid.\n");
        return -EINVAL;
    }

    if (hdmi.state == HDMI_DISPLAY_ACTIVE) {
        hw_core_blank_video(true);
    }

    mutex_lock(&hdmi.lock);

    /* don't reset PHY unless in full power state */
    if (HDMI_POWER_FULL == hdmi.power_state) {
        hdmi.in_reset = true;
        hdmi_pw_power_off();
    }
    /* power off state should not reset, so set min state to restart */
    hdmi.power_state = HDMI_POWER_MIN;

    ret = reconfig();
    if (ret) {
        loge("re config fail: %d.\n", ret);
    }
    hdmi.in_reset = false;

    mutex_unlock(&hdmi.lock);

    if (hdmi.state == HDMI_DISPLAY_ACTIVE) {
#if !USE_HDCP
        hw_core_blank_video(false);
#endif
    }

    OUT_FUNCTION;

    return ret;
}

/******************************************************************************
* Function:       config_phy_tmds
* Description:    to calculate best aclk_dig
* Data Accessed:
* Data Updated:
* Input:          vsdb_format
* Output:
* Return:
* Others:
*******************************************************************************/
static int config_phy_tmds(deep_color *vsdb_format){
    int phy       = 0;
    int max_tmds  = 0;
    int temp      = 0;
    int i         = 0;
    int min_diff  = 0;
    int diff      = 0;
    int tmp_aclk_dig = 0;
    int aclk_mult_factory[] = {1,2,3,4,5,6,10};
    int best_aclk_factory    = aclk_mult_factory[0];

    max_tmds = vsdb_format->max_tmds_freq * 500;//todo : why *500?,

    switch (hdmi.deep_color) {
        case HDMI_DEEP_COLOR_30BIT: {
            temp = (hdmi.timings->pixel_clock * 125) / 100 ;
            if (!hdmi.custom_set) { //todo: why?
                if (vsdb_format->bit_30) {
                    if (max_tmds != 0 && max_tmds >= temp) {
                        phy = temp;
                    }
                } else {
                    loge( "TV does not support Deep color\n");
                    goto err;
                }
            } else {
                phy = temp;
            }
            hdmi.cfg.deep_color = HDMI_DEEP_COLOR_30BIT;

            break;
        }
        case HDMI_DEEP_COLOR_36BIT: {
            if (148500 == hdmi.timings->pixel_clock) {
                loge("36 bit deep color not supported\n");
                goto err;
            }

            temp = (hdmi.timings->pixel_clock * 150) / 100;
            if (!hdmi.custom_set) {
                if (vsdb_format->bit_36) {
                    if (max_tmds != 0 && max_tmds >= temp) {
                        phy = temp;
                    }
                } else {
                    loge( "TV does not support Deep color\n");
                    goto err;
                }
            } else {
                phy = temp;
            }
            hdmi.cfg.deep_color = HDMI_DEEP_COLOR_36BIT;

            break;
        }
        case HDMI_DEEP_COLOR_24BIT:
        default: {
            phy = hdmi.timings->pixel_clock;
            hdmi.cfg.deep_color = HDMI_DEEP_COLOR_24BIT;

            break;
        }
    }

    /* TMDS freq_out in the PHY should be set based on the TMDS clock */
    logd("phy clock: %d.\n", phy);

    /*aclk_dig = dpcolor_multiplication_factor * aclkmult_factor * 5 * p_clkin (input clock frequency)*/
    for(i = 0; i < ARRAY_SIZE(aclk_mult_factory); i++){
        tmp_aclk_dig = (phy * 5) / aclk_mult_factory[i];
        if(HDMI_MAX_ACLK_DIG < tmp_aclk_dig || HDMI_MIN_ACLK_DIG > tmp_aclk_dig) {
            continue;
        }
        diff = tmp_aclk_dig - HDMI_BEST_ACLK_DIG;
        if(diff < 0) {
            diff = -diff;
        }
        if((0 == min_diff) || (diff < min_diff)) {
            min_diff = diff;
            best_aclk_factory = aclk_mult_factory[i];
        }
    }

    hw_phy_configure_aclk_dig(best_aclk_factory);

    return 0;

err:

    return -1;
}

/******************************************************************************
* Function:       hdmi_hdcp_notify
* Description:    notify hdcp anthentication state to user space
* Data Accessed:
* Data Updated:
* Input:          hdcp anthentication state
* Output:
* Return:
* Others:
*******************************************************************************/
void hdmi_hdcp_notify(char *result)
{
    char *envp[2] = {result, NULL};
    kobject_uevent_env(&hdmi.pdev->dev.kobj, KOBJ_CHANGE, envp);
}

bool hdmi_is_connect(void)
{
    u32 hpd_state = 0;
    if(hdmi.state != HDMI_DISPLAY_ACTIVE) {
        return false;
    }

    if (hw_support_mhl()) {
        if (g_mhl_tx_config.tmds_output_enabled) {
            return true;
        } else {
            return false;
        }
    }

    //hpd_state = read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_SYS_STAT);
    hpd_state = tpi_read_reg(TPI_INTERRUPT_STATUS_REG);
    if (hpd_state &  (RX_SENSE_MASK | HOT_PLUG_PIN_STATE_MASK)) {
        return true;
    }
    return false;
}

int hdmi_read_edid(void)
{
#define HDMI_READ_EDID_COUNT 6
    int i = 0;
    int ret = 0;
    if (!hdmi.edid_set) {
        memset(&edid, 0, sizeof(edid));
        for (i = 0; i < HDMI_READ_EDID_COUNT && hdmi_is_connect(); i++) {
            //ret = k3_mhl_read_edid();
            ret = edid_read(edid);
            if (!ret) {
                hdmi.edid_set = true;
                logi("read edid success.\n");
                break;
            }
            loge("read edid fail, and retry.\n");
            msleep(100);
        }
        if (ret) {
            loge("read edid fail.\n");
            memset(&edid, 0, sizeof(edid));
            hdmi.edid_set = false;
            ret = -EIO;
        }
    }
    return ret;
}

/******************************************************************************
* Function:       hdmi_pw_power_on_full
* Description:    turn on hdmi device and config hdmi device by edid
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
static int hdmi_pw_power_on_full(void)
{
    int ret       = 0;
    int index     = INVALID_VALUE;

    deep_color vsdb_format = {0};
    hdmi_cm cm = {INVALID_VALUE};
    audio_format aformat = {0};

    IN_FUNCTION;

    hdmi.power_state = HDMI_POWER_FULL;
    k3_fb1_power_ctrl(true);
    hdmi.timings = edid_get_timings_byindex(get_timings_index());

    logi("custom_set :%d edid_set :%d.\n", hdmi.custom_set, hdmi.edid_set);

    /* custom_set be setted to false in init or power off phase, so the edid_read must be called when power */
    if (!hdmi.custom_set) {
        logd("No edid set thus will be calling edid_read\n");

        if (!hdmi.edid_set) {
#if USE_HDCP
#if HDCP_FOR_CERTIFICATION
            logi("disable hdcp befor read edid.\n");
            //hdcp_wait_anth_stop();
#endif
            hdcp_off();
            /* waiting for 64ms to disable HDCP */
            msleep(64);
#endif
            //hdmi,mhl read edid process have been integrated,so keep one of them.
            ret = hdmi_read_edid();
            if (ret) {
#if !ENABLE_EDID_FAULT_TOLERANCE
                goto err;
#endif
            }
        }

        if (hdmi.s3d_enabled) {
            /* Update flag to convey if sink supports 3D */
            hdmi.s3d_enabled = edid_s3d_supported(edid);
            logi("hdmi.s3d_enabled = %s\n",hdmi.s3d_enabled ? "true":"false");
        }

        /* search for timings of default resolution */
        cm = edid_get_best_timing((hdmi_edid *)edid);
        if (INVALID_VALUE != cm.code) {
            if (!hdmi.manual_set) {
                hdmi.mode = cm.mode;
                if (cm.code > HDMI_CODE_VESA_OFFSET) {
                    hdmi.code = cm.code - HDMI_CODE_VESA_OFFSET;
                } else {
                    hdmi.code = cm.code;
                }
            }
            /*z00222844 20120817 should not set edid_set here because ENABLE_EDID_FAULT_TOLERANCE*/
        }

    }

    /* update config to hdmi device */
    update_cfg(&hdmi.cfg, hdmi.timings);

    if ((hdmi.s3d_enabled)
        && (HDMI_S3D_FRAME_PACKING ==  hdmi.cfg.s3d_structure)) {
        logd("using the s3d index.\n");
        index = get_s3d_timings_index();
    } else {
        logd("using the common index\n");
        index = get_timings_index();
    }
    update_cfg_pol(&hdmi.cfg, index);

    /* update pixel clock with deep color */
    hdmi.timings = edid_get_timings_byindex(index);

    logd("get best timing mode:%d code:%d index:%d\n", hdmi.mode, hdmi.code, index);
    edid_dump_video_timings(hdmi.timings);

    edid_get_deep_color_info(edid, &vsdb_format);

    logd("deep color : bit30=%d, bit36=%d, max_tmds_freq=%d.\n",
                 vsdb_format.bit_30, vsdb_format.bit_36,
                 vsdb_format.max_tmds_freq);

    if(config_phy_tmds(&vsdb_format)){
        loge("config_aclk_dig error \n");
        goto err;
    }

    hdmi.cfg.vsi_enabled = hdmi.s3d_enabled;
    hdmi.cfg.hdmi_dvi = (int) (edid_get_audio_format((u8*)edid, &aformat) || (edid_has_ieee_id((u8 *)edid) && hdmi.mode));
#if ENABLE_EDID_FAULT_TOLERANCE
    if (false == edid_is_valid_edid((u8*)edid)) {
        loge("edid isn't readed, and set to support audio.\n");
        hdmi.cfg.hdmi_dvi = HDMI_HDMI;
    }
#endif
    hdmi.cfg.video_format = hdmi.code;

    hdmi.cfg.supports_ai = edid_ai_supported(edid);

    logi("Supports AI: %s, %s: %d, res: %dx%d \n",
         hdmi.cfg.supports_ai ? "YES" : "NO",
         hdmi.cfg.hdmi_dvi ? "CEA" : "VESA",
         hdmi.code, hdmi.timings->x_res,
         hdmi.timings->y_res);

    if (HDMI_CODE_TYPE_CEA == hdmi.mode) {
        if(EDID_TIMING_MODE_I == edid_get_timing_mode(hdmi.code)) {
            hdmi.cfg.interlace = 1;
        } else {
            hdmi.cfg.interlace = 0;
        }
    }

#if HDMI_FOR_CERTIFICATION
    hdmi.cfg.has_vcdb = edid_has_vcdb((u8 *) edid);
#endif

    if (hw_support_mhl() && edid_supported_yuv422(edid) &&
        hdmi.timings->pixel_clock > MHL_PPMODE_PIXEL_CLOCK) {
        hdmi.pp_mode_needed = true;
    }

    hw_enable(&hdmi.cfg);

    if (hdmi.s3d_enabled) {
        hdmi.s3d_type = hdmi.cfg.s3d_structure;
    } else { //2d or set fail
        hdmi.s3d_type = HDMI_2D;
    }

#if ENABLE_REG_DUMP
    memcpy((void*)hdmi_reg, (const void*)HDMI_CORE_SYS, HDMI_REG_LENGTH);
    memcpy((void*)(hdmi_reg + HDMI_REG_LENGTH), (const void*)HDMI_PHY_BASE, HDMI_PHY_REG_LENGTH);
#endif

    return 0;

err:
    return ret;
}

/******************************************************************************
* Function:       hdmi_pw_power_on_min
* Description:    turn on hdmi device to min
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
static void hdmi_pw_power_on_min(void)
{
    IN_FUNCTION;
#if USE_HDCP
#if HDCP_FOR_CERTIFICATION
        logi("disable hdcp when power min.\n");
        //hdcp_wait_anth_stop();
#endif
#endif

#ifdef CONFIG_CPU_FREQ_GOV_K3HOTPLUG
    if (hdmi.has_request_ddr) {
        hdmi.has_request_ddr = false;
        pm_qos_remove_request(&hdmi.qos_request);
        logi("remove qos request\n");
    }
#endif
    if (hdmi.power_state ==  HDMI_POWER_FULL) {
        k3_fb1_power_ctrl(false);
    }

    hdmi.work_state = HDMI_WORK_STATE_IDLE;

    hdmi.power_state = HDMI_POWER_MIN;
    hdmi.custom_set = false;
    if (!hdmi.in_reset) {
        hdmi.edid_set = false;
        hdmi.connected = false;
    }

#if USE_HDCP
    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK,
        TMDS_OUTPUT_CONTROL_POWER_DOWN | AV_MUTE_MUTED);
#endif
    OUT_FUNCTION;

    return;
}

/******************************************************************************
* Function:       hdmi_pw_power_off
* Description:    turn off hdmi device, and notify to user space
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
static void hdmi_pw_power_off(void)
{
    int ret = 0;

    IN_FUNCTION;

#if USE_HDCP
    if (!hdmi.in_reset) {
        hdcp_off();
    }
#endif
    hdmi.state = HDMI_DISPLAY_DISABLED;
    //for real suspend/resume, should read edid. z36904
    if (!hdmi.in_reset) {
        hdmi.edid_set = false;
    }

    hw_enable_irqs(false);

    if (hw_res.irq && hdmi.irq_is_init) {
        free_irq(hw_res.irq, (void *)0);
        hdmi.irq_is_init = false;
    }


    if (hw_support_mhl() && !hdmi.in_reset) {
        if(mhl_work) {
            cancel_work_sync(mhl_work);
        }
        kfree(mhl_work);
    }
#if HDMI_PHY_PROCESS
    hw_phy_power_off();
#endif
    hw_core_power_off();
    /*in reset not notify*/
    if (!hdmi.in_reset) {
        hdmi.connected = false;
        ret = hdmi_control_notify_hpd_status(false);
        if (ret) {
            loge("notify hpd status fail: %d.\n", ret);
        }
    }
    OUT_FUNCTION;

    return;
}

/******************************************************************************
* Function:       hdmi_pw_set_power
* Description:    set hdmi device power on/off by parameter
* Data Accessed:
* Data Updated:
* Input:          detail refer below
* Output:
* Return:
* Others:
*******************************************************************************/
/*
 * set power state depending on device state and HDMI state.
 *
 * v_power, v_state is the desired video power/device state
 *
 */
static int hdmi_pw_set_video_power(hdmi_power_state v_power)
{
    int ret = 0;
    hdmi_power_state power_need = v_power;

    IN_FUNCTION;

    logi("power chg: (video:%d,state:%d)->(video:%d 0:OFF 1:MIN 2:FULL)\n",
         hdmi.video_power_state,
         hdmi.state, v_power);

    /* turn on HDMI */
    if ((hdmi.state != HDMI_DISPLAY_ACTIVE) && (power_need)) {
        logd("power on the core.\n");

        hw_core_power_on();
#if HDMI_PHY_PROCESS
        hw_phy_power_on();
#endif

        tpi_write_reg(TPI_ENABLE, 0x00);

#if USE_HDCP
        if (!hdmi.in_reset) {
            hdcp_init(&hdmi_hdcp_notify);
        }
#endif
        if (hw_support_mhl() && !hdmi.in_reset) {
            //wake_lock_init(&hdmi.wake_lock, WAKE_LOCK_SUSPEND, "mhl wake lock");
            //mhl_work = (struct work_struct *)kzalloc(sizeof(*mhl_work), GFP_ATOMIC);
            mhl_work = kzalloc(sizeof(struct work_struct), GFP_ATOMIC);
            if (mhl_work) {
                INIT_WORK(mhl_work, mhl_work_queue);
            } else {
                loge("alloc mhl_work error\n");
                goto err;
            }
        }

        logd("hdmi.irq_is_init: %d",hdmi.irq_is_init);
        if (!hdmi.irq_is_init) {
            hw_init_irqs();
            logd("request_irq \n");

            ret = request_irq(hw_get_irq(), hdmi_irq_handler, 0, "k3_hdmi_irq", (void *)0);
            if (ret) {
                loge("request_irq error %d.\n", ret);
                goto err;
            }
            hdmi.irq_is_init = true;
        }
        logd("enable irq. \n");
        hw_enable_irqs(true);
        mdelay(20);
        if (hw_support_mhl() && !hdmi.in_reset) {
            k3_mhl_initialize();
        }
    }

    hdmi.video_power_state = v_power;

    if (hdmi.power_state != power_need) {
        if (HDMI_POWER_FULL == power_need) {
            logi("power full.\n");
            ret = hdmi_pw_power_on_full();
#if CONFIG_HDMI_CEC_ENABLE
            hdmi_cec_start_use_thread();
#endif
        } else if (HDMI_POWER_MIN == power_need) {
            logi("power min.\n");
            hdmi_pw_power_on_min();
#if CONFIG_HDMI_CEC_ENABLE
            hdmi_cec_start_use_thread(); // for tv sleep
#endif
        } else {
            logi("power off.\n");
            if(HDMI_DISPLAY_ACTIVE == hdmi.state){
#if CONFIG_HDMI_CEC_ENABLE
                hdmi_cec_stop();
#endif
                hdmi_pw_power_off();
            }
        }

        if (!ret) {
            hdmi.power_state = power_need;
        }
    }

    OUT_FUNCTION;
err:
    return ret;
}



#if ENABLE_CHECK_SHORT_CIRCUIT
/******************************************************************************
* Function:       check_short_circuit
* Description:    check if has got short circuit and clear.
* Data Accessed:
* Data Updated:
* Input:          NA
* Output:
* Return:
* Others:
*******************************************************************************/
static void check_short_circuit(void)
{
    #define SHORT_CIRCUIT_MAX_COUNT 20
    #define SHORT_CIRCUIT_REG IO_ADDRESS(0xfcc0015c)   //k3v3todo
    #define SHORT_CIRCUIT_VAL 0x20

    static int check_count = 0;
    int val = 0;

    if(check_count == SHORT_CIRCUIT_MAX_COUNT) {
        return;
    }

    val = read_reg(SHORT_CIRCUIT_REG, 0);
    if (SHORT_CIRCUIT_VAL & val) {
        logw("short_circuit, count:%d\n",check_count);
        write_reg(SHORT_CIRCUIT_REG, 0, SHORT_CIRCUIT_VAL);
        check_count ++;
    }
}
#endif

/******************************************************************************
* Function:       hdmi_irq_work_queue
* Description:    process irq work queue
* Data Accessed:
* Data Updated:
* Input:          irq work
* Output:
* Return:
* Others:
*******************************************************************************/
static void hdmi_irq_work_queue(struct work_struct *ws)
{
    int ret    = 0;
    int action = 0;
    unsigned long time = 0;
    bool need_notify = false;
    bool notify_state = false;

    hdmi_work_info *work = container_of(ws, hdmi_work_info, work);
    u32 state = work->state;

    static ktime_t last_connect = {0};
    static ktime_t last_disconnect = {0};


    mutex_lock(&hdmi.lock);

    if ((NULL == hdmi.pdev) || (hdmi.state != HDMI_DISPLAY_ACTIVE)) {
        logw("HDMI is disabled, there cannot be any HDMI irqs.\n");
        goto done;
    }

    if (state & (HDMI_CONNECT | HDMI_DISCONNECT)) {
        action = hw_rx_detect();
    }

    logi("irq: 0x%08x(1:connected 2:disconnected 8:first hpd), state: %d(0:disable, 1:active, 2:suspend), power: %d/%d(0:off, 1:min, 2:full).\n"
         "action: %d(1:connect, 2:disconnect), connected: %d, reset: %d.\n"
         , state, hdmi.state, hdmi.power_state, hdmi.video_power_state, action, hdmi.connected, hdmi.in_reset);

    if ((HDMI_DISCONNECT == action)
        && (false == hdmi.in_reset)
        && (HDMI_POWER_OFF != hdmi.power_state)) {

        /*
         * Wait at least 100ms after HDMI_CONNECT to decide if
         * cable is really disconnected
         */
        last_disconnect = ktime_get();
        time = ktime_to_ms(ktime_sub(last_disconnect, last_connect));
        if (time < 100) {
            msleep(100 - time);
        }

        if ((hdmi.connected) || (HDMI_CONNECT == hw_rx_detect())) {
            logi("this is a disconnect event ,and the disconnect status is fake.\n");
            goto done;
        }

#if ENABLE_CHECK_SHORT_CIRCUIT
        if (!hw_support_mhl()) {
            check_short_circuit();
        }
#endif

        logd("notify hpd disconnect.\n");
        need_notify = true;
        notify_state = false;

        if (hdmi.state != HDMI_DISPLAY_ACTIVE) {
            logi("not active, no need to process.\n");
            goto done;
        }

        hdmi.edid_set = hdmi.custom_set = false;
        hdmi.manual_set = false;

#if HDMI_CHIP_VER
       if (hw_support_mhl() && g_mhl_tx_config.real_disconnection) {
#else
        logi("MHL DO NOT POWER OFF on fpga\n");//test for k3v3
        if (false/*hw_support_mhl()*/) {
#endif
            logi("HDMI_DISCONNECT, and go to power off.\n");
            mutex_unlock(&hdmi.lock);
            enable_hpd(false);
            mutex_lock(&hdmi.lock);
            g_mhl_tx_config.real_disconnection = false;
        } else {
            ret = hdmi_pw_set_video_power(HDMI_POWER_MIN);
            if (ret) {
                loge("set video power to power min fail: %d.\n", ret);
            }
        }
    }

    /* read connect timestamp */
    if (HDMI_CONNECT == action) {
        last_connect = ktime_get();
    }

    /*  the status of reconfig param, to notify only when manual set or not custom set*/
    if ((HDMI_CONNECT == action)
        && (HDMI_POWER_FULL == hdmi.power_state)
        && (!hdmi.custom_set || hdmi.manual_set)) {

        /*in this status, must to notify the deamon*/
        hdmi.hpd_current_state = false;
        need_notify = true;
        notify_state = true;
    }

    logd("hdmi.edid_set = %d, custom_set = %d.\n", hdmi.edid_set, hdmi.custom_set);

    /*  the status of first hpd
    *if this hpd is first, the power state isn't full; so it to
    *process the soft reset and notity the deamon to change the param
    */
    if ((state & HDMI_FIRST_HPD)) {
        logi("Enabling display - HDMI_FIRST_HPD\n");
        hdmi.custom_set = false;
        hdmi.edid_set = false;
        hdmi.manual_set = false;

        ret = reconfig();
        if (ret) {
            loge("re config fail: %d.\n", ret);
        }
        need_notify = true;
        notify_state = true;

        logd("Enabling display Done- HDMI_FIRST_HPD\n");
        goto done;
    } else if ((HDMI_CONNECT == action)
        && (HDMI_POWER_FULL != hdmi.power_state)
        && (HDMI_POWER_MIN == hdmi.video_power_state)) {

        /*1.suspend->min 2.resume->min 3.->irq 4.call reconfig -> full */

        logd("Physical Connect\n");

        ret = reconfig();
        if (ret) {
            loge("re config fail: %d.\n", ret);
        }

        if (hdmi.state != HDMI_DISPLAY_ACTIVE) {
            logi("not active, no need to process.\n");
            goto done;
        }

        logd("Physical Connect Done:\n");
    }
done:

    mutex_unlock(&hdmi.lock);
    kfree(work);

    if (need_notify) {
        ret = hdmi_control_notify_hpd_status(notify_state);//v2pro test
        if (ret) {
            loge("notify hpd status fail: %d.\n", ret);
        }
    }

#if USE_HDCP
    if (!hw_support_mhl() && hdmi.state == HDMI_DISPLAY_ACTIVE) {
        logi("hdmi irq hdcp process begin\n");
        hdcp_handler(state);
    }
#endif

    logd("out work queue.\n");
    return;
}

/******************************************************************************
* Function:       hdmi_irq_handle_work
* Description:    create work and put work to queue when notify irq
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
static void hdmi_irq_handle_work(u32 state)
{
    hdmi_work_info *work = NULL;
    if (state) {
        work = kzalloc(sizeof(*work), GFP_ATOMIC);
        if (work) {
            INIT_WORK(&work->work, hdmi_irq_work_queue);
            work->state = state;
            queue_work(hdmi.irq_wq, &work->work);
        } else {
            loge("malloc mem fail.\n");
        }
    }

    return;
}

static void hdmi_process_mhl_irq(int irq)
{
    disable_irq_nosync(irq);
    schedule_work(mhl_work);
}

static void hdmi_process_hdmi_irq(void)
{
     u32 hdmi_state = 0;
     unsigned long flags = 0;

     /* process interrupt in critical section to handle conflicts */
     spin_lock_irqsave(&irqstatus_lock, flags);
     hdmi_state = hw_get_hdmi_state();
     logd("Received IRQ state: 0x%08x\n", hdmi_state);

     if (0 == hdmi_state) {
         logd("needn't to process this irq.\n");
         spin_unlock_irqrestore(&irqstatus_lock, flags);
         return;
     }

     if (hdmi_state & HDMI_CONNECT) {
         logd("hdmi connect.\n");
         hdmi.connected = true;
     }

     if (hdmi_state & HDMI_DISCONNECT) {
         logd("hdmi disconnect.\n");
         hdmi.connected = false;
     }

     spin_unlock_irqrestore(&irqstatus_lock, flags);

     hdmi_irq_handle_work(hdmi_state);

     return;
}


/******************************************************************************
* Function:       hdmi_irq_handler
* Description:    irq handler, will get irqs from register
* Data Accessed:
* Data Updated:
* Input:          irq number and parameter, not use
* Output:
* Return:
* Others:
*******************************************************************************/
static irqreturn_t hdmi_irq_handler(int irq, void *arg)
{
    if (hw_support_mhl()) {
        hdmi_process_mhl_irq(irq);
    } else {
        hdmi.work_state = HDMI_WORK_STATE_PROCESS_HDMI;
    }

    if (hdmi.work_state == HDMI_WORK_STATE_PROCESS_HDMI) {
        hdmi_process_hdmi_irq();
    }

    return IRQ_HANDLED;
}

/******************************************************************************
* Function:       hdmi_control_notify_hpd_status
* Description:    notify cable connection state to user space
* Data Accessed:
* Data Updated:
* Input:          connection state
* Output:
* Return:
* Others:
*******************************************************************************/
static int hdmi_control_notify_hpd_status(bool onoff)
{
    int ret = -1;
    int count = 30;

    IN_FUNCTION;

    logi("hpd state: %d, hpd current state: %d. \n", onoff, hdmi.hpd_current_state);

    if (onoff == hdmi.hpd_current_state) {
        logi("don't need to send hpd message.\n");
        OUT_FUNCTION;
        return 0;
    }
    hdmi.audiochanged = true;
#ifdef FPGA_AUDIO_TEST
    hi3620_pcm_hdmi_event(onoff, true);
#endif
    ret = kobject_uevent(&hdmi.pdev->dev.kobj, onoff ? KOBJ_ADD : KOBJ_REMOVE);
    while (ret && (-2 != ret) && count > 0) {
        ret = kobject_uevent(&hdmi.pdev->dev.kobj, onoff ? KOBJ_ADD : KOBJ_REMOVE);
        /* kobject_uevent seems to always return an error (-2) even success. */
        if (ret && (-2 != ret)) {
            loge("send hot plug event err: %d and retry.\n", ret);
        }
        if ((!onoff) || (!hdmi.connected)) {
            break;
        }
        msleep(300);
        count --;
    }

#if ENABLE_AUTO_ROTATE_LANDSCAPE
    switch_set_state(&hdmi.hpd_switch, onoff ? 1 : 0);
#endif

#ifdef FPGA_AUDIO_TEST
    hi3620_pcm_hdmi_event(onoff, false);
#endif

    hdmi.hpd_current_state = onoff;

    OUT_FUNCTION;
    return 0;
}

int hdmi_control_notify_cec_cmd(char *cec_str)
{
    char *envp[2] = {cec_str, NULL};

    IN_FUNCTION;

    kobject_uevent_env(&hdmi.pdev->dev.kobj, KOBJ_CHANGE, envp);

    OUT_FUNCTION;
    return 0;
}


/******************************************************************************
* Function:       hdmi_get_timing_list
* Description:    get timing codes and store to buf
* Data Accessed:
* Data Updated:
* Input:          mhl_check: true: check mhl and return mhl supported code, false: not check
* Output:         buf: with video timing codes
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_get_timing_list(char *buf, bool mhl_check)
{
    int size = 0;

    int has_image_format = 0;
    int i = 0;
    int len = 0;
    char* p = buf;
    image_format* img_format = NULL;
    int oneNum = 0;
    hdmi_cm cm = {INVALID_VALUE,INVALID_VALUE};

    IN_FUNCTION;

#if ENABLE_EDID_FAULT_TOLERANCE
    if (false == edid_is_valid_edid((u8*)edid)) {
        loge("edid isn't readed, and use default code.\n");
        OUT_FUNCTION;
        return snprintf(buf, PAGE_SIZE, "%d\n", hdmi.code);
    }
#endif

    img_format = kzalloc(sizeof(*img_format), GFP_KERNEL);
    if (!img_format) {
        loge("malloc mem fail.\n");
        OUT_FUNCTION;
        return -ENOMEM;
    }

    /*get all support timing code*/
    has_image_format = edid_get_image_format(edid, img_format);
    if (!has_image_format) {
        logw("there isn't video image format in edid.\n");
    }

    logi("video format number: %d.\n", img_format->number);

    for (i = 0 ; i < img_format->number; i++) {
        /* now we only support p mode. */
        if (1 == img_format->fmt[i].code) {
            oneNum++;
        }

        if (edid_get_timing_order(img_format->fmt[i].code, mhl_check) >= 0) {
           len = snprintf(p, PAGE_SIZE-size, "%d,",img_format->fmt[i].code);
           size += len;
           p += len;
        }
    }

    cm = edid_get_timing_code((hdmi_edid *)edid, false);
    if (INVALID_VALUE != cm.code) {
        if (HDMI_CODE_TYPE_VESA == cm.mode) {
            if (edid_get_vesa_timing_order(cm.code + HDMI_CODE_VESA_OFFSET, mhl_check) >=0 ) {
                if ((cm.code + HDMI_CODE_VESA_OFFSET) > HDMI_MAX_VESA_TIMING_CODE) {
                    len = snprintf(p, PAGE_SIZE-size, "%d,",HDMI_DEFAULT_VESA_TIMING_CODE);
                } else {
                    len = snprintf(p, PAGE_SIZE-size, "%d,",(cm.code + HDMI_CODE_VESA_OFFSET));
                }
                size += len;
                p += len;
            } else {
                len = snprintf(p, PAGE_SIZE-size, "%d,",HDMI_DEFAULT_VESA_TIMING_CODE);
                size += len;
                p += len;
            }
        } else {
            if (edid_get_timing_order(cm.code, mhl_check) >= 0) {
                len = snprintf(p, PAGE_SIZE-size, "%d,", cm.code);
                size += len;
                p += len;
            } else {
                if (oneNum == img_format->number) {
                    len = snprintf(p, PAGE_SIZE-size, "%d,", HDMI_DEFAULT_MHL_TIMING_CODE);
                    size += len;
                    p += len;
                }
            }
        }
        has_image_format = 1;
    }

    if (size > 0) {
        buf[size-1] = '\n';
        buf[size] = '\0';
    }

    kfree(img_format);

#if HDCP_FOR_CERTIFICATION
    if (1) {
#else
    if ((!has_image_format) || (size == 0) || (hdmi.manual_set && mhl_check)) {
#endif
        if (0 == hdmi.mode) {
            size = snprintf(buf, PAGE_SIZE, "%d\n",hdmi.code+100);
        } else {
            size = snprintf(buf, PAGE_SIZE, "%d\n",hdmi.code);
        }
    }

    logi("edid: %s \n",buf);

    OUT_FUNCTION;
    return size;
}

/******************************************************************************
* Function:       hdmi_control_edid_show
* Description:    read func of sysfs--edid, write video timing codes to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with video timing codes
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_edid_show(struct device *dev,
                                      struct device_attribute *attr, char *buf)
{
    return hdmi_get_timing_list(buf, true);
}

/******************************************************************************
* Function:       hdmi_control_edidbin_show
* Description:    get edid data
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with video timing codes
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_edidbin_show(struct device *dev,
                                      struct device_attribute *attr, char *buf)
{
     memcpy(buf,edid,sizeof(edid));
     return sizeof(edid);
}

#if ENABLE_REG_DUMP
/******************************************************************************
* Function:       hdmi_control_regbin_show
* Description:    get edid data
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with video timing codes
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_regbin_show(struct device *dev,
                                      struct device_attribute *attr, char *buf)
{
     memcpy(buf,hdmi_reg,sizeof(hdmi_reg));
     return sizeof(hdmi_reg);
}
#endif

/******************************************************************************
* Function:       hdmi_control_edid_show_all
* Description:    read func of sysfs--edid, write all video timing codes to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with video timing codes
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_edid_show_all(struct device *dev,
                                      struct device_attribute *attr, char *buf)
{
    return hdmi_get_timing_list(buf, false);
}

/******************************************************************************
* Function:       hdmi_control_timing_show
* Description:    read func of sysfs--code, write current timing code to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with current timing code
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_timing_show(struct device *dev,
                                        struct device_attribute *attr, char *buf)
{
    int showCode = hdmi.code;
    if (HDMI_CODE_TYPE_VESA == hdmi.mode) {
        showCode = hdmi.code + HDMI_CODE_VESA_OFFSET;
    }
    return snprintf(buf, PAGE_SIZE, "%s:%u\n", hdmi.mode ? "CEA" : "VESA", showCode);
}

/******************************************************************************
* Function:       hdmi_control_defaultcode_show
* Description:    read func of sysfs--code, write current timing code to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with default timing code
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_defaultcode_show(struct device *dev,
                                        struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%u\n", edid_get_default_code());
}

/******************************************************************************
* Function:       hdmi_control_timing_store
* Description:    set timing code to hdmi device
* Data Accessed:
* Data Updated:
* Input:          buf: string of timing code, size: length of buf
*                 buf with '#': manual set, with '*': auto set
* Output:
* Return:         length of buf
* Others:
*******************************************************************************/
static ssize_t hdmi_control_timing_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t size)
{
    unsigned long code = 0;
    int mode = HDMI_CODE_TYPE_CEA;
    int ret = 0;
    bool auto_timing = false;
    hdmi_cm cm = {INVALID_VALUE};

    char *pstrchr = NULL;

    IN_FUNCTION;

    logi("Input param : %s.\n", buf);

    if ((!*buf)) {
        loge("Input param is error: %s.\n", buf);
        OUT_FUNCTION;
        return -EINVAL;
    }

#if HDCP_FOR_CERTIFICATION
    logi("for hdcp test, not set timing\n");
    return size;
#endif

    pstrchr = strchr(buf, '#');
    if (pstrchr) {
        hdmi.manual_set = true;
        *pstrchr = 0;
    }

    pstrchr = strchr(buf, '*');
    if (pstrchr) {
        hdmi.manual_set = false;
        auto_timing = true;
        *pstrchr = 0;
    }

    if (false == auto_timing) {
        ret = strict_strtoul(buf, 0, &code);
        if (ret) {
            loge("Input param is error: %s. \n", buf);
            OUT_FUNCTION;
            return -EINVAL;
        }
        hdmi.custom_set = true;
    } else {
        cm = edid_get_best_timing((hdmi_edid*)edid);
        code = cm.code;
        hdmi.custom_set = false;
    }

    if (code > HDMI_CODE_VESA_OFFSET) {
        code = code - HDMI_CODE_VESA_OFFSET;
        mode = HDMI_CODE_TYPE_VESA;
    } else {
        mode = HDMI_CODE_TYPE_CEA;
    }

    /* if the current code is same to the set code, so don't process */
    logi("set timing mode:%d code:%d manual:%d\n", mode, (int)code, hdmi.manual_set);
    if ((code != hdmi.code) || (mode != hdmi.mode)) {
        ret = set_custom_timing_code(code, mode);
        if (ret) {
            loge("set custom timing code fail: %d.\n", ret);
            OUT_FUNCTION;
            return ret;
        }
    } else {
        logi("set code is same as cur code, and do nothing.\n");
    }

    OUT_FUNCTION;
    return size;
}

#if USE_HDCP
/******************************************************************************
* Function:       hdmi_control_hdcp_show
* Description:    read func of sysfs--hdcp, write whether hdcp enable to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with whether hdcp enable
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_hdcp_show(struct device *dev,
                                      struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "HDCP: %s\n", hdcp_get_enable() ? "ENABLE" : "DISABLE");
}

/******************************************************************************
* Function:       hdmi_control_hdcp_store
* Description:    enable or disable hdcp auth
* Data Accessed:
* Data Updated:
* Input:          buf: string of "yY1nN0", size: length of buf
* Output:
* Return:         length of buf
* Others:
*******************************************************************************/
static ssize_t hdmi_control_hdcp_store(struct device *dev,
                                       struct device_attribute *attr,
                                       const char *buf, size_t size)
{
    bool enable = false;

    IN_FUNCTION;

    if ((!*buf) ||(!strchr("yY1nN0", *buf))) {
        loge("Input param is error(valid: yY1nN0): %s. \n",buf);
        OUT_FUNCTION;
        return -EINVAL;
    }

    enable = !!strchr("yY1", *buf++);

    if (hdcp_get_enable() == enable) {
        OUT_FUNCTION;
        return size;
    }

    hdcp_set_enable(enable);

    logd("enable = %d, buf = %s",enable,  buf);
#if !HDCP_FOR_CERTIFICATION
    hw_reconfig_hdcp(enable);
#endif
    OUT_FUNCTION;

    return size;
}
#endif

/******************************************************************************
* Function:       hdmi_control_3d_show
* Description:    read func of sysfs--s3d, write current 3d type to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with current 3d type
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_3d_show(struct device *dev,
                                    struct device_attribute *attr, char *buf)
{
    char * type = "NONE";

    switch (hdmi.s3d_type) {
        case HDMI_S3D_FRAME_PACKING: {
            type = "HDMI_S3D_FRAME_PACKING";
            break;
        }
        case HDMI_S3D_FIELD_ALTERNATIVE : {
            type = "HDMI_S3D_FIELD_ALTERNATIVE";
            break;
        }
        case HDMI_S3D_LINE_ALTERNATIVE : {
            type = "HDMI_S3D_LINE_ALTERNATIVE";
            break;
        }
        case HDMI_S3D_SIDE_BY_SIDE_FULL: {
            type = "HDMI_S3D_SIDE_BY_SIDE_FULL";
            break;
        }
        case HDMI_S3D_L_DEPTH : {
            type = "HDMI_S3D_L_DEPTH";
            break;
        }
        case HDMI_S3D_L_DEPTH_GP_GP_DEPTH : {
            type = "HDMI_S3D_L_DEPTH_GP_GP_DEPTH";
            break;
        }
        case HDMI_S3D_SIDE_BY_SIDE_HALF : {
            type = "HDMI_S3D_SIDE_BY_SIDE_HALF";
            break;
        }
        case HDMI_S3D_TOP_TO_BOTTOM: {
            type = "HDMI_S3D_TOP_TO_BOTTOM";
            break;
        }
        default: {
            type = "NONE";
            break;
        }
    }

    return snprintf(buf, PAGE_SIZE, "3D: %s TYPE: %s\n", hdmi.s3d_enabled ? "ON" : "OFF", type);
}

/******************************************************************************
* Function:       hdmi_control_hdcp_store
* Description:    enable or disable hdcp auth
* Data Accessed:
* Data Updated:
* Input:          buf: string of "0nN1sS2tT", size: length of buf.
*                 0,n,N: 2d;  1,s,S: sidebyside; 2,t,T: top bottom
* Output:
* Return:         length of buf
* Others:
*******************************************************************************/
static ssize_t hdmi_control_3d_store(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf, size_t size)
{
    bool is_3d = true;

    IN_FUNCTION;

    if ((!*buf) || (!strchr("0nN1sS2tT", *buf))) {
        loge("Input param buf is error(valid: 0,n,N, 1,s,S, 2,t,T): %s. \n",buf);
        OUT_FUNCTION;
        return -EINVAL;
    }

    is_3d = !strchr("0nN", *buf++);
    if (*buf && (strcmp(buf, "\n"))) {
        loge("Input param buf is error, last char not is \\n . \n");
        OUT_FUNCTION;
        return -EINVAL;
    }

    if (is_3d) {
        switch (*(buf-1)) {
            case '1':
            case 's':
            case 'S': {
                logi("set s3d mode is SBS.\n");
                hdmi.cfg.s3d_structure = HDMI_S3D_SIDE_BY_SIDE_HALF;
                hdmi.cfg.subsamp_pos = HDMI_S3D_HOR_EL_ER;
                break;
            }
            case '2':
            case 't':
            case 'T': {
                logi("set s3d mode is TTB.\n");
                hdmi.cfg.s3d_structure = HDMI_S3D_TOP_TO_BOTTOM;
                break;
            }
            default: {
                loge("Input param buf is error(valid: 0,n,N, 1,s,S, 2,t,T): %s. \n",buf);
                OUT_FUNCTION;
                return -EINVAL;
            }
        }
    } else {
        logi("set to 2d mode.\n");
        hdmi.cfg.s3d_structure = HDMI_2D;
    }

    hdmi.custom_set = true;
    if ((hdmi.s3d_enabled != is_3d) || (hdmi.s3d_type != hdmi.cfg.s3d_structure)) {
        hdmi.s3d_enabled = is_3d;
        hdmi.s3d_type = hdmi.cfg.s3d_structure;
        reset();
    }

    OUT_FUNCTION;
    return size;
}

/******************************************************************************
* Function:       hdmi_control_audio_show
* Description:    read func of sysfs--dst, write audio formats to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: with audio format
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_audio_show(struct device *dev,
                                       struct device_attribute *attr, char *buf)
{
    int size = 0;
    int len  = 0;
    int i    = 0;
    int has_audio_format = 0;

    char* p  = buf;
    audio_format *aud_format = NULL;

    IN_FUNCTION;

#if ENABLE_EDID_FAULT_TOLERANCE
    if (false == edid_is_valid_edid((u8*)edid)) {
        logw("edid isn't readed, get default type: %d chn_num: %d freq: %d.\n",
                      HDMI_DEFAULT_AUDIO_TYPE,
                      HDMI_DEFAULT_AUDIO_CHANNL_NUM + 1,//return real num of ch, so muse add 1.
                      HDMI_DEFAULT_AUDIO_FREQUENCY);
        OUT_FUNCTION;
        return snprintf(p, PAGE_SIZE-size, "%c:%c:%c;",
                      HDMI_DEFAULT_AUDIO_TYPE,
                      HDMI_DEFAULT_AUDIO_CHANNL_NUM + 1,
                      HDMI_DEFAULT_AUDIO_FREQUENCY);
    }
#endif

    aud_format = kzalloc(sizeof(audio_format), GFP_KERNEL);
    if (!aud_format) {
        loge("malloc mem fail.\n");
        OUT_FUNCTION;
        return -ENOMEM;
    }

    has_audio_format = edid_get_audio_format(edid, aud_format);
    if (!has_audio_format) {
        logw("there isn't audio format.\n");
    }

    logd("audio format number: %d. \n", aud_format->number);

    for (i = 0 ; i < aud_format->number ; i++) {
        logi( "    format: %d, channel number: %d, sampling freq: %d. \n",
              aud_format->fmt[i].format,
              aud_format->fmt[i].num_of_ch,
              aud_format->fmt[i].sampling_freq);

        len= snprintf(p, PAGE_SIZE-size, "%c:%c:%c;",
                      aud_format->fmt[i].format,
                      aud_format->fmt[i].num_of_ch,
                      aud_format->fmt[i].sampling_freq);

        size += len;
        p += len;
    }

    kfree(aud_format);

    OUT_FUNCTION;
    return size;
}

/******************************************************************************
* Function:       hdmi_control_hpd_show
* Description:    read func of sysfs--hpd, write hdmi power state to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: hdmi power state
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_hpd_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
    char * power = "HDMI_POWER_OFF";

    switch (hdmi.video_power_state) {
        case HDMI_POWER_OFF: {
            power = "HDMI_POWER_OFF";
            break;
        }
        case HDMI_POWER_MIN: {
            power = "HDMI_POWER_MIN";
            break;
        }
        case HDMI_POWER_FULL : {
            power = "HDMI_POWER_FULL";
            break;
        }
        default: { /* fake branch */
            loge("video power state is invalid: %d.\n", hdmi.video_power_state);
            break;
        }
    }

    return snprintf(buf, PAGE_SIZE, "%s\n",power);
}

/******************************************************************************
* Function:       hdmi_control_hpd_store
* Description:    enable or disable hpd, if it is disable will not cause irq
* Data Accessed:
* Data Updated:
* Input:          buf: string of "yY1nN0", size: length of buf.
* Output:
* Return:         length of buf
* Others:
*******************************************************************************/
static ssize_t hdmi_control_hpd_store(struct device *dev,
                                      struct device_attribute *attr,
                                      const char *buf, size_t size)
{
    bool enable = false;
    int ret = 0;
    bool manual_set = false;

    char *pstrchr = NULL;

    IN_FUNCTION;

    if ((!*buf) ||(!strchr("yY1nN0", *buf))) {
        loge("Input param buf is error(valid: yY1nN0): %s. \n", buf);
        OUT_FUNCTION;
        return -EINVAL;
    }

    pstrchr = strchr(buf, '#');
    if (pstrchr) {
        manual_set = true;
        *pstrchr = 0;
    }

    enable = !!strchr("yY1", *buf++);

    if (*buf && (strcmp(buf, "\n"))) {
        loge("Input param buf is error, last char not is \\n. \n");
        OUT_FUNCTION;
        return -EINVAL;
    }

#if HDMI_CHIP_VER
    if (hw_support_mhl() && (!manual_set)) {
        logw("this device support mhl, and don't need to enable hpd.\n");
        OUT_FUNCTION;
        return -1;
    }
#endif
    logd("enable: %d, buf: %s. \n", enable, buf);

    ret = enable_hpd(enable);
    if (ret) {
        loge("enable hdp fail.\n");
        OUT_FUNCTION;
        return -1;
    }
    OUT_FUNCTION;
    return size;
}

/******************************************************************************
* Function:       hdmi_control_deepcolor_show
* Description:    get current deep color
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
static ssize_t hdmi_control_deepcolor_show(struct device *dev,
                                       struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", hdmi.deep_color);
}

/******************************************************************************
* Function:       hdmi_control_deepcolor_store
* Description:    set deep color
* Data Accessed:
* Data Updated:
* Input:          0: 24bit; 1: 30bit; 2: 36bit
* Output:
* Return:
* Others:
*******************************************************************************/
static ssize_t hdmi_control_deepcolor_store(struct device *dev,
                                                       struct device_attribute *attr,
                                                       const char *buf, size_t size)
{
    unsigned long deep_color = 0;

    int ret = strict_strtoul(buf, 0, &deep_color);
    if (ret || (deep_color > HDMI_DEEP_COLOR_36BIT))
    {
        loge("deep_color error, deep_color: %ld.\n", deep_color);
        return -EINVAL;
    }

    hdmi.custom_set = true;
    if (hdmi.deep_color != deep_color) {
        hdmi.deep_color = deep_color;
        reset();
    }
    return size;
}

/******************************************************************************
* Function:       hdmi_control_reset
* Description:    reset hdmi after para change
* Data Accessed:
* Data Updated:
* Input:          0: 24bit; 1: 30bit; 2: 36bit
* Output:
* Return:
* Others:
*******************************************************************************/
static ssize_t hdmi_control_reset(struct device *dev,
                                                       struct device_attribute *attr,
                                                       const char *buf, size_t size)
{
    int ret = 0;

    if (HDMI_POWER_OFF != hdmi.power_state) {
        ret = reset();
        if (ret) {
            loge("reset hdmi fail: %d.\n", ret);
            return -EINVAL;
        }
    }

    return size;
}

/******************************************************************************
* Function:       hdmi_control_reg_show
* Description:    read func of sysfs--reg, write registers dump to buf
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: registers dump
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_reg_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
    //dump_reg();

    return snprintf(buf, PAGE_SIZE, "reg_dump\n");
}

/******************************************************************************
* Function:       hdmi_control_conected_show
* Description:    get connected state
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: registers dump
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_conected_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", hdmi.connected ? 1 : 0);
}

/******************************************************************************
* Function:       hdmi_control_s3dsupport_show
* Description:    get device whether support 3d
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: 1 --- support s3d
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_s3dsupport_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", edid_s3d_supported(edid) ? 1 : 0);
}

/******************************************************************************
* Function:       hdmi_control_audiosupport_show
* Description:    get device whether support 3d
* Data Accessed:
* Data Updated:
* Input:          dev: device, attr: device attribute descriptor.
* Output:         buf: 1 --- support audio
* Return:         writed size
* Others:
*******************************************************************************/
static ssize_t hdmi_control_audiosupport_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
    int has_video_format = 0;
    int oneNum = 0;
    image_format img_format = {0};
    int i = 0;
    int audio_support = hdmi.cfg.hdmi_dvi;
    audio_format aud_format = {0};
#if ENABLE_EDID_FAULT_TOLERANCE
    if (false == edid_is_valid_edid((u8*)edid)) {
        loge("edid isn't readed, and set to support audio.\n");
        return snprintf(buf, PAGE_SIZE, "%d\n", HDMI_HDMI);
    }
#endif

    has_video_format = edid_get_image_format(edid, &img_format);
    if(has_video_format){
        for(i = 0; i < img_format.number ; i++){
            if(1 == img_format.fmt[i].code){
                oneNum ++;
            }
        }
        if(oneNum == img_format.number){
            audio_support = HDMI_DVI;
        }
    }

    //for Amplifier+monitor, is dvi device but support audio.
    if (audio_support == HDMI_DVI) {
        audio_support = edid_get_audio_format(edid, &aud_format);;
    }

    return snprintf(buf, PAGE_SIZE, "%d\n", audio_support);
}

/******************************************************************************
* Function:       hdmi_control_bufisfree_store
* Description:    check video is free
* Data Accessed:
* Data Updated:
* Input:          addres.
* Output:
* Return:         1 free, 0 in use, -1 err
* Others:
*******************************************************************************/
static ssize_t hdmi_control_bufisfree_store(struct device *dev,
                                                       struct device_attribute *attr,
                                                       const char *buf, size_t size)
{
    int addr = *(int*)buf;
    if ((hdmi.state != HDMI_DISPLAY_ACTIVE) || (size != sizeof(int)) || (addr == 0)) {
        return -1;
    }
    return size;
}

#if defined(CONFIG_HDMI_CEC_ENABLE)
/*
printf("cec_setcm 0  0x36 0          //<Standy> standy :ok\n");
printf("cec_setcm 0  0X04 0          //<Image View On> invoke TV out of standy :ok\n");
printf("cec_setcm 0  0X0d 0          //<Text View On> invoke TV out of standy :ok\n");

*/
static ssize_t hdmi_control_cec_store(struct device *dev,
                                                       struct device_attribute *attr,
                                                       const char *buf, size_t size)
{
    hdmi_cec_param cec_param;

    if (strstr(buf, "enable")) {
        hdmi_cec_enable();
        goto done;
    }
    if (strstr(buf, "disable")) {
        hdmi_cec_disable();
        goto done;
    }

    memset(&cec_param, 0, sizeof(cec_param));
    cec_param.en_dst_addr = 0;

    if (strstr(buf, "standby")) {
        cec_param.opcode = CEC_OPCODE_STANDBY;
    } else if (strstr(buf, "imageon")) {
        cec_param.opcode = CEC_OPCODE_IMAGE_VIEW_ON;
    } else if (strstr(buf, "texton")) {
        cec_param.opcode = CEC_OPCODE_TEXT_VIEW_ON;
    } else if (size == sizeof(hdmi_cec_param)) {
        memcpy(&cec_param, buf, size);
    } else {
        cec_str_tocmd(buf, &cec_param);
    }

    if (hdmi.state != HDMI_DISPLAY_ACTIVE) {
        loge("Hdmi is not power on, can not do cmd:%s !\n", buf);
        goto done;
    }
    hdmi_sys_lock();
    hdmi_cec_send_command(&cec_param);
    hdmi_sys_unlock();
done:
    return size;
}
#endif

static ssize_t hdmi_control_name_show(struct device *dev,
                                      struct device_attribute *attr, char *buf)
{
    #define MONITOR_NAME_LEN 13
    int i = 0;
    for (i = 0; i < EDID_SIZE_BLOCK0_TIMING_DESCRIPTOR; i++) {
        if (HDMI_EDID_DTD_TAG_MONITOR_NAME == ((hdmi_edid*)edid)->dtd[i].monitor_name.block_type){
            logi("get name:%s\n",((hdmi_edid*)edid)->dtd[i].monitor_name.text);
            snprintf(buf, MONITOR_NAME_LEN, "%s", ((hdmi_edid*)edid)->dtd[i].monitor_name.text);
            return strlen(buf);
        }
    }
    return 0;
}
#ifdef FPGA_AUDIO_TEST
static ssize_t hdmi_control_audiotest_store(struct device *dev,
                                                       struct device_attribute *attr,
                                                       const char *buf, size_t size)
{
    rpt(FPGA_AUDIO_TEST_TYPE);
    return size;
}
#endif

static ssize_t hdmi_debug_level_show(struct device *dev,
                                      struct device_attribute *attr, char *buf)
{

    return snprintf(buf, PAGE_SIZE, "Debug level: %d\n", g_msg_print_level);
}
static ssize_t hdmi_debug_level_store(struct device *dev,
                                                       struct device_attribute *attr,
                                                       const char *buf, size_t size)
{
    if ((!*buf) || (!strchr("12345678", *buf))) {
        loge("Input param buf is error(valid: 1,2,3,4,5,6,7,8): %s. \n", buf);
        return -EINVAL;
    }

    if (*(buf + 1) && (strcmp(buf + 1, "\n"))) {
        loge("Input param buf is error, last char is not \\n . \n");
        return -EINVAL;
    }
    g_msg_print_level = *buf - '0';

    return size;
}

static ssize_t hdmi_mhl_support_show(struct device *dev,
                                      struct device_attribute *attr, char *buf)
{

    return snprintf(buf, PAGE_SIZE, "is support mhl: %d\n", hdmi.support_mhl_interface);
}

static ssize_t hdmi_mhl_support_store(struct device *dev,
                                                       struct device_attribute *attr,
                                                       const char *buf, size_t size)
{
    bool is_support = false;
    int ret = 0;
    if ((!*buf) || (!strchr("01", *buf))) {
        loge("Input param buf is error(valid: 1,2,3,4,5,6,7,8): %s. \n", buf);
        return -EINVAL;
    }

    if (*(buf + 1) && (strcmp(buf + 1, "\n"))) {
        loge("Input param buf is error, last char is not \\n . \n");
        return -EINVAL;
    }

    is_support = (*buf - '0') ? true : false;

    if (hdmi.support_mhl_interface == is_support) {
        logi("Set param %d is same, do nothing.", is_support);
        return size;
    }

    hdmi.support_mhl_interface = is_support;
    if (hw_support_mhl()) {
        /*register mhl rcp input device*/
        ret = mhl_rcp_register_input_dev();
        if (ret) {
            loge("register rcp input device error %d\n",ret);
        }

        //wake_lock_init(&hdmi.wake_lock, WAKE_LOCK_SUSPEND, "mhl wake lock");
        //mhl_work = (struct work_struct *)kzalloc(sizeof(*mhl_work), GFP_ATOMIC);
        mhl_work = kzalloc(sizeof(struct work_struct), GFP_ATOMIC);
        if (mhl_work) {
            INIT_WORK(mhl_work, mhl_work_queue);
        } else {
            loge("alloc mhl_work error\n");
            return -1;
        }
    } else {
        //todo unregister input dev
        if(mhl_work) {
            cancel_work_sync(mhl_work);
        }
        kfree(mhl_work);
    }

    enable_hpd(false);
    enable_hpd(true);
    return size;
}

static ssize_t hdmi_frame_rate_show(struct device *dev,
                                                       struct device_attribute *attr,
                                                       char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", hdmi.video_frame_rate);
}

static ssize_t hdmi_frame_rate_store(struct device *dev,
                                                       struct device_attribute *attr,
                                                       const char *buf, size_t size)
{
    int ret = 0;
    unsigned long frameRate = 0;

    loge("Input param : %s.\n", buf);

    if ((!*buf)) {
        loge("Input param is error: %s.\n", buf);
        return -EINVAL;
    }

    ret = strict_strtoul(buf, 0, &frameRate);
    if (ret) {
        loge("Input param is error: %s. \n", buf);
        return -EINVAL;
    }

    if (frameRate > 0 && frameRate <= 60) {
        hdmi.video_frame_rate = frameRate;
    }

    return 0;
}

static ssize_t hdmi_control_mhl_probe_show(struct device *dev,
                                                       struct device_attribute *attr,
                                                       char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", hdmi.mhl_probe_success ? 1 : 0);
}

#define S_SYSFS_W (S_IWUSR|S_IWGRP)
/* sysfs attribute */
static DEVICE_ATTR(edid, S_IRUGO, hdmi_control_edid_show, NULL);
static DEVICE_ATTR(code, S_IRUGO| S_SYSFS_W, hdmi_control_timing_show, hdmi_control_timing_store);
static DEVICE_ATTR(s3d, S_IRUGO | S_SYSFS_W, hdmi_control_3d_show, hdmi_control_3d_store);
static DEVICE_ATTR(dst, S_IRUGO, hdmi_control_audio_show, NULL);
static DEVICE_ATTR(hpd, S_IRUGO| S_SYSFS_W, hdmi_control_hpd_show, hdmi_control_hpd_store);
#if USE_HDCP
static DEVICE_ATTR(hdcp, S_IRUGO | S_SYSFS_W, hdmi_control_hdcp_show, hdmi_control_hdcp_store);
#endif
static DEVICE_ATTR(deepcolor, S_IRUGO | S_SYSFS_W, hdmi_control_deepcolor_show, hdmi_control_deepcolor_store);
static DEVICE_ATTR(reset, S_IRUGO | S_SYSFS_W, NULL, hdmi_control_reset);
static DEVICE_ATTR(reg, S_IRUGO, hdmi_control_reg_show, NULL);
static DEVICE_ATTR(connected, S_IRUGO, hdmi_control_conected_show, NULL);
static DEVICE_ATTR(s3dsupport, S_IRUGO, hdmi_control_s3dsupport_show, NULL);
static DEVICE_ATTR(alledid, S_IRUGO, hdmi_control_edid_show_all, NULL);
static DEVICE_ATTR(audiosupport, S_IRUGO, hdmi_control_audiosupport_show, NULL);
static DEVICE_ATTR(bufisfree, S_IRUGO | S_SYSFS_W, NULL, hdmi_control_bufisfree_store);
static DEVICE_ATTR(edidbin, S_IRUGO, hdmi_control_edidbin_show, NULL);
static DEVICE_ATTR(defaultcode, S_IRUGO, hdmi_control_defaultcode_show, NULL);
#if ENABLE_REG_DUMP
static DEVICE_ATTR(regbin, S_IRUGO, hdmi_control_regbin_show, NULL);
#endif
static DEVICE_ATTR(name, S_IRUGO, hdmi_control_name_show, NULL);
#if defined(CONFIG_HDMI_CEC_ENABLE)
static DEVICE_ATTR(cec, S_IRUGO | S_SYSFS_W, NULL, hdmi_control_cec_store);
#endif
#ifdef FPGA_AUDIO_TEST
static DEVICE_ATTR(audiotest, S_IRUGO | S_SYSFS_W, NULL, hdmi_control_audiotest_store);
#endif
static DEVICE_ATTR(debuglevel, S_IRUGO | S_SYSFS_W, hdmi_debug_level_show, hdmi_debug_level_store);
static DEVICE_ATTR(supportmhl, S_IRUGO | S_SYSFS_W, hdmi_mhl_support_show, hdmi_mhl_support_store);
static DEVICE_ATTR(framerate, S_IRUGO | S_SYSFS_W, hdmi_frame_rate_show, hdmi_frame_rate_store);
static DEVICE_ATTR(mhlprobe, S_IRUGO, hdmi_control_mhl_probe_show, NULL);

/******************************************************************************
* Function:      hdmi_control_create_device_file
* Description:   create sysfs attribute file for device.
* Data Accessed:
* Data Updated:
* Input:         dev: device attr: device attribute descriptor.
* Output:
* Return:
* Others:
*******************************************************************************/
static int hdmi_control_create_device_file(struct device *dev,
        const struct device_attribute *attr)
{
    BUG_ON(NULL == dev);

    return sysfs_create_file(&dev->kobj, &attr->attr);
}

/******************************************************************************
* Function:      hdmi_control_remove_device_file
* Description:   remove sysfs attribute file.
* Data Accessed:
* Data Updated:
* Input:         dev: device  attr: device attribute descriptor
* Output:
* Return:
* Others:
*******************************************************************************/
static void hdmi_control_remove_device_file(struct device *dev,
        const struct device_attribute *attr)
{
    BUG_ON(NULL == dev);

    sysfs_remove_file(&dev->kobj, &attr->attr);

    return;
}

/******************************************************************************
* Function:       hdmi_control_create_sysfs
* Description:    create files of sysfs
* Data Accessed:
* Data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
static void hdmi_control_create_sysfs(struct platform_device *dev)
{
    BUG_ON(NULL == dev);

    IN_FUNCTION;

    /* register HDMI specific sysfs files */
    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_edid)) {
        loge("failed to create sysfs file --edid \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_code)) {
        loge("failed to create sysfs file --timing \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_s3d)) {
        loge("failed to create sysfs file --s3d \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_dst)) {
        loge("failed to create sysfs file --audio \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_hpd)) {
        loge("failed to create sysfs file --hpd \n");
    }
#if USE_HDCP
    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_hdcp)) {
        loge("failed to create sysfs file --hdcp \n");
    }
#endif
    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_deepcolor)) {
        loge("failed to create sysfs file --deepcolor \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_reg)) {
        loge("failed to create sysfs file --reg \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_connected)) {
        loge("failed to create connected file --connected \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_s3dsupport)) {
        loge("failed to create s3dsupport file --s3dsupport \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_alledid)) {
        loge("failed to create alledid file --alledid \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_audiosupport)) {
        loge("failed to create audiosupport file --audiosupport \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_bufisfree)) {
        loge("failed to create videoisfree file --videoisfree \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_reset)) {
        loge("failed to create reset file --reset \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_edidbin)) {
        loge("failed to create edidbin file --reset \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_defaultcode)) {
        loge("failed to create defaultcode file --reset \n");
    }

#if ENABLE_REG_DUMP
    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_regbin)) {
        loge("failed to create regbin file --regbin \n");
    }
#endif

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_name)) {
        loge("failed to create name file --name \n");
    }

#if defined(CONFIG_HDMI_CEC_ENABLE)
    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_cec)) {
        loge("failed to create cec file --cec \n");
    }
#endif

#ifdef FPGA_AUDIO_TEST
    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_audiotest)) {
        loge("failed to create audiotest file --audiotest \n");
    }
#endif
    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_debuglevel)) {
        loge("failed to create debuglevel file --debuglevel \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_supportmhl)) {
        loge("failed to create supportmhl file --supportmhl \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_framerate)) {
        loge("failed to create framerate file --framerate \n");
    }

    if (hdmi_control_create_device_file(&dev->dev, &dev_attr_mhlprobe)) {
        loge("failed to create framerate file --mhlprobe \n");
    }
    OUT_FUNCTION;

    return;
}

/******************************************************************************
* Function:       hdmi_control_remove_sysfs
* Description:    remove files of sysfs
* Data Accessed:
* Data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
static void hdmi_control_remove_sysfs(struct platform_device *dev)
{
    BUG_ON(NULL == dev);

    IN_FUNCTION;

    hdmi_control_remove_device_file(&dev->dev, &dev_attr_edid);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_code);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_s3d);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_dst);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_hpd);
#if USE_HDCP
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_hdcp);
#endif
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_deepcolor);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_reg);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_connected);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_s3dsupport);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_alledid);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_audiosupport);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_bufisfree);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_reset);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_edidbin);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_defaultcode);
#if ENABLE_REG_DUMP
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_regbin);
#endif
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_name);
#if defined(CONFIG_HDMI_CEC_ENABLE)
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_cec);
#endif

#ifdef FPGA_AUDIO_TEST
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_audiotest);
#endif
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_debuglevel);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_supportmhl);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_framerate);
    hdmi_control_remove_device_file(&dev->dev, &dev_attr_mhlprobe);

    OUT_FUNCTION;

    return;
}

/******************************************************************************
* Function:       k3_hdmi_audio_set_power
* Description:    turn on/off audio power
* Data Accessed:
* Data Updated:
* Input:          true for turn on, false for other
* Output:
* Return:
* Others:
*******************************************************************************/
int k3_hdmi_audio_set_power(bool audio_on)
{
    int ret = 0;

    IN_FUNCTION;

    if (audio_on == true) {
        if (hdmi.state != HDMI_DISPLAY_ACTIVE) {
            loge("the hdmi state is not active, can't to set it.\n");
            OUT_FUNCTION;
            return -1;
        }
        if (hdmi.audiochanged == false) {
            OUT_FUNCTION;
            return -1;
        }

        /* config n and cts by pixel_clock and fs */
        ret = hw_configure_acr(hdmi.timings->pixel_clock, hdmi.audio_core_cfg.fs);
        if (ret) {
            loge("configure hardware acr fail, %d. \n", ret);
            ret = -EINVAL;
        }

        /* set audio config to hdmi device */
        hw_configure_audio(HDMI_CORE_AV, &hdmi.audio_core_cfg);
        hdmi.audiochanged = false;
    }

    OUT_FUNCTION;
    return ret;
}

/******************************************************************************
* Function:       k3_hdmi_audio_set_param
* Description:    set audio parameter
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
int k3_hdmi_audio_set_param(int sample_freq, int sample_size, bool bsio, int layout, int audiotype, int channel_number)
{
    int ret = 0;

    IN_FUNCTION;

    mutex_lock(&hdmi.lock_aux);
    ret = hdmi_audio_set_param(sample_freq, sample_size, bsio, layout, audiotype, channel_number);
    mutex_unlock(&hdmi.lock_aux);

    OUT_FUNCTION;
    return ret;
}

EXPORT_SYMBOL(k3_hdmi_audio_set_power);
EXPORT_SYMBOL(k3_hdmi_audio_set_param);

/******************************************************************************
* Function:       start_hdmi_process
* Description:    enable hpd api for mhl driver
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void start_hdmi_process()
{
    IN_FUNCTION;
    hdmi.work_state = HDMI_WORK_STATE_PROCESS_HDMI;
    hdmi_process_hdmi_irq();
    OUT_FUNCTION;
    return;
}

/******************************************************************************
* Function:       hdmi_get_vsync_bycode
* Description:    get vsync pol by
* Data Accessed:
* Data Updated:
* Input:          true - eanble
* Output:
* Return:
* Others:
*******************************************************************************/
int hdmi_get_vsync_bycode(int code)
{
    int index = INVALID_VALUE;
    int mode = HDMI_CODE_TYPE_CEA;

    if (code >= HDMI_CODE_VESA_OFFSET) {
        mode = HDMI_CODE_TYPE_VESA;
        code -= HDMI_CODE_VESA_OFFSET;
    }

    if (edid_is_valid_code(mode, code)) {
        index = edid_get_timings_index(mode, code);
        if (INVALID_VALUE != index) {
            return edid_get_hvpol_byindex(index)->vsync_pol == 1 ? 0 : 1;
        }
    }

    return 0;
}

bool hdmi_get_cec_addr(u8* physical_addr){
    return edid_cec_getphyaddr(edid, physical_addr);
}

/******************************************************************************
* Function:       hdmi_get_hsync_bycode
* Description:    get hsync
* Data Accessed:
* Data Updated:
* Input:          timing code
* Output:
* Return:
* Others:
*******************************************************************************/
int hdmi_get_hsync_bycode(int code)
{
    int index = INVALID_VALUE;
    int mode = HDMI_CODE_TYPE_CEA;

    if (code >= HDMI_CODE_VESA_OFFSET) {
        mode = HDMI_CODE_TYPE_VESA;
        code -= HDMI_CODE_VESA_OFFSET;
    }
    if (edid_is_valid_code(mode, code)) {
        index = edid_get_timings_index(mode, code);
        if (INVALID_VALUE != index) {
            return edid_get_hvpol_byindex(index)->hsync_pol == 1 ? 0 : 1;
        }
    }

    return 0;
}

EXPORT_SYMBOL(hdmi_get_vsync_bycode);
EXPORT_SYMBOL(hdmi_get_hsync_bycode);

void k3_hdmi_enable_hpd(bool enable)
{
    IN_FUNCTION;
    switch_connection = true;
    enable_hpd(enable);
    OUT_FUNCTION;

    return;
}
EXPORT_SYMBOL(k3_hdmi_enable_hpd);
void hdmi_sys_lock()
{
    mutex_lock(&hdmi.lock);
}
void hdmi_sys_unlock()
{
    mutex_unlock(&hdmi.lock);
}

static struct k3_panel_info hdmi_panel_info = {0};
static struct k3_fb_panel_data hdmi_panel_data = {
    .panel_info = &hdmi_panel_info,
};

/******************************************************************************
* Function:       hdmi_regist_fb
* Description:    regist hdmi device to fb driver, fb will transmit image out to hdmi device
* Data Accessed:
* Data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
int hdmi_regist_fb(struct platform_device *pdev)
{
    struct k3_panel_info *pinfo = NULL;

    //set the fb1's timing to 1080p default
    int index = edid_get_timings_index(HDMI_DEFAULT_TIMING_MODE
                    ,edid_get_default_code());
    hdmi_video_timings * ptimings = edid_get_timings_byindex(index);

    BUG_ON(NULL == pdev);

    logi("hdmi_regist_fb pinfo!!!!!\n");
    pinfo = hdmi_panel_data.panel_info;
    pinfo->xres = ptimings->x_res;
    pinfo->yres = ptimings->y_res;
    pinfo->type = PANEL_HDMI;
    pinfo->orientation = LCD_LANDSCAPE;
    pinfo->bpp = EDC_OUT_RGB_888;
    pinfo->bgr_fmt = EDC_RGB;

    pinfo->pxl_clk_rate = ptimings->pixel_clock * 1000;
    pinfo->ldi.h_back_porch = ptimings->hbp;
    pinfo->ldi.h_front_porch = ptimings->hfp;
    pinfo->ldi.h_pulse_width = ptimings->hsw;
    pinfo->ldi.v_back_porch = ptimings->vbp;
    pinfo->ldi.v_front_porch = ptimings->vfp;
    pinfo->ldi.v_pulse_width = ptimings->vsw;
    pinfo->ldi.hsync_plr = hdmi_get_hsync_bycode(edid_get_default_code());
    pinfo->ldi.vsync_plr = hdmi_get_vsync_bycode(edid_get_default_code());
    pinfo->ldi.pixelclk_plr = 1;
    pinfo->ldi.data_en_plr = 0;

    /* alloc panel device data */
    if (platform_device_add_data(pdev, &hdmi_panel_data, sizeof(struct k3_fb_panel_data))) {
        loge("k3fb, platform_device_add_data failed!\n");
        return -ENOMEM;
    }

    if (NULL == k3_fb_add_device(pdev)) {
        loge("k3_fb_add_device error....\n");
        return -ENOMEM;
    }
    logi("hdmi_regist_fb OUT!!!!!\n");
    return 0;
}
static unsigned short rcp_key_code[] = {
    KEY_MENU,/* 139 */
    KEY_SELECT,/* 0x161 */
    KEY_UP,/* 103 */
    KEY_LEFT,/* 105 */
    KEY_RIGHT,/* 106 */
    KEY_DOWN,/* 108 */
    KEY_EXIT,/* 174 */
    KEY_1, /* 0x02 */
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9, /* 0x10 */
    KEY_0,
    KEY_DOT,/* 52 */
    KEY_ENTER,/* 28 */
    KEY_CLEAR,/* 0x163 *//* DEL */
    KEY_SOUND,/* 0x213 */
    KEY_PLAY,/* 207 */
    KEY_PAUSE,/* 119 */
    KEY_STOP,/* 128 */
    KEY_FASTFORWARD,/* 208 */
    KEY_REWIND,/* 168 */
    KEY_EJECTCD,/* 161 */
    KEY_FORWARD,/* 159 */
    KEY_BACK,/* 158 */
    KEY_PLAYCD,/* 200 */
    KEY_PAUSECD,/* 201 */
    KEY_STOP,/* 128 */
    KEY_F1,/* 59 */
    KEY_F2,/* 60 */
    KEY_F3,
    KEY_F4,
    KEY_F5,/* 63 */
    KEY_CHANNELUP, /* 0x192 */
    KEY_CHANNELDOWN, /* 0x193 */
    KEY_MUTE, /* 113 */
    KEY_PREVIOUS, /* 0x19c */
    KEY_VOLUMEDOWN, /* 114 */
    KEY_VOLUMEUP, /* 115 */
    KEY_RECORD, /* 167 */
    KEY_REPLY,
    KEY_PLAYPAUSE,
    KEY_PREVIOUSSONG, /* add  */
    KEY_BACKSPACE, /* DEL */
};

/******************************************************************************
* Function:       mhl_rcp_register_input_dev
* Description:    register input dev to report rcp code
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
int mhl_rcp_register_input_dev(void)
{
    int i = 0;
    int ret = 0;

    /*register mhl rcp input device*/
    hdmi.rcp_dev = input_allocate_device();
    if (!hdmi.rcp_dev) {
        loge("mhl input_allocate_device error\n");
        return -1;
    }

    hdmi.rcp_dev->name = "mhl_rcp_dev";
    hdmi.rcp_dev->id.bustype = BUS_I2C;
    hdmi.rcp_dev->keycode = rcp_key_code;
    hdmi.rcp_dev->keycodesize = sizeof(rcp_key_code[0]);
    hdmi.rcp_dev->keycodemax = ARRAY_SIZE(rcp_key_code);

    __set_bit(EV_KEY, hdmi.rcp_dev->evbit);
    __clear_bit(EV_REP, hdmi.rcp_dev->evbit);

    for (i = 0; i < ARRAY_SIZE(rcp_key_code); i++) {
        input_set_capability(hdmi.rcp_dev, EV_KEY, rcp_key_code[i]);
    }

    __clear_bit(KEY_RESERVED, hdmi.rcp_dev->keybit);

    ret = input_register_device(hdmi.rcp_dev);
    if (ret) {
        loge("MHL register input device failed: %d\n",ret);
        input_free_device(hdmi.rcp_dev);
    }

    return ret;
}

static void k3_fb_blanked(int blank)
{
    if (hdmi.dss_inited)
        return;

    if (blank == FB_BLANK_UNBLANK)
        hdmi.dss_inited = true;
}

/******************************************************************************
* Function:        k3_hdmi_event_notify
* Description:    receive framebuffer notification
* Input:          action & FBINFO
* Output:
* Return:
*******************************************************************************/
static int k3_hdmi_event_notify(struct notifier_block *self,
                              unsigned long action, void *data)
{
    struct fb_event *event = data;
    struct fb_info *info = event->info;
    struct k3_fb_data_type *k3fd = info->par;

    if (k3fd->index != K3_DSS_PDP)
        return 0;

    switch(action) {
    case FB_EVENT_BLANK:
        k3_fb_blanked(*(int *)event->data);
        break;
    default:
        break;
    }

    return 0;
}

static struct notifier_block k3_fb_event_notifier = {
    .notifier_call  = k3_hdmi_event_notify,
};

/******************************************************************************
* Function:       k3_hdmi_remove
* Description:    resources recycle when hdmi driver removed
* Data Accessed:
* Data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
int k3_hdmi_remove(struct platform_device* dev)
{
    int ret = 0;

    IN_FUNCTION;

    ret = enable_hpd(false);
    if (ret) {
        loge("disable hpd fail: %d.\n", ret);
    }

#if USE_HDCP
    hdcp_off();
#endif

    if(hdmi.irq_wq) {
        destroy_workqueue(hdmi.irq_wq);
    }
#if 0
    if (hw_support_mhl()) {
        if(mhl_work) {
            cancel_work_sync(mhl_work);
        }
        kfree(mhl_work);
    }
#endif
    hw_free_resources();

    hdmi_control_remove_sysfs(dev);

#if ENABLE_AUTO_ROTATE_LANDSCAPE
    switch_dev_unregister(&hdmi.hpd_switch);
#endif
    ret = fb_unregister_client(&k3_fb_event_notifier);
    if (ret) {
        loge("Unable to unregister framebuffer notifier for HDMI.\n");
    }

    memset(&hdmi, 0, sizeof(hdmi_device));
    probe_complete = false;

    OUT_FUNCTION;
    return 0;
}

/******************************************************************************
* Function:       k3_hdmi_probe
* Description:    probe func of hdmi driver, init hdmi device
* Data Accessed:
* Data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
int k3_hdmi_probe(struct platform_device *pdev)
{
    int ret = 0;

    if (k3_fb_device_probe_defer(PANEL_HDMI)) {
        goto err_probe_defer;
    }

    IN_FUNCTION;

    memset(&hdmi, 0, sizeof(hdmi));

#ifdef MHL_SUPPORT
    hdmi.support_mhl_interface = true;
#else
    hdmi.support_mhl_interface = false;
#endif

    hdmi.pdev = pdev;
    hdmi.irq_wq = create_singlethread_workqueue("hdmi_irq_wq");
    hdmi.code = edid_get_default_code();
    hdmi.mode = HDMI_DEFAULT_TIMING_MODE;
    hdmi.timings = edid_get_timings_byindex(get_timings_index());
    hdmi.deep_color = HDMI_DEEP_COLOR_24BIT;

    hdmi.s3d_type = HDMI_2D;
    hdmi.cfg.s3d_structure = HDMI_2D;

    /*init audio default param*/
    hdmi.sample_freq = 48000;
    hdmi.sample_size = HDMI_SAMPLE_16BITS;
    hdmi.bsio = false;
    hdmi.layout = LAYOUT_2CH;
    hdmi.audiotype = 0;

    mutex_init(&hdmi.lock);
    mutex_init(&hdmi.lock_aux);

    hdmi_control_create_sysfs(pdev);

    /* get device resources */
    ret= hw_get_resources(pdev);
    if (ret) {
        loge("get hdmi resources error %d\n",ret);
        goto err;
    }

    if (hw_support_mhl()) {
        /*register mhl rcp input device*/
        ret = mhl_rcp_register_input_dev();
        if (ret) {
            loge("register rcp input device error %d\n",ret);
            goto err;
        }
#if 0
        wake_lock_init(&hdmi.wake_lock, WAKE_LOCK_SUSPEND, "mhl wake lock");
        //mhl_work = (struct work_struct *)kzalloc(sizeof(*mhl_work), GFP_ATOMIC);
        mhl_work = kzalloc(sizeof(struct work_struct), GFP_ATOMIC);
        if (mhl_work) {
            INIT_WORK(mhl_work, mhl_work_queue);
        } else {
            loge("alloc mhl_work error\n");
            goto err;
        }
#endif
    }

    wake_lock_init(&hdmi.wake_lock, WAKE_LOCK_SUSPEND, "mhl wake lock");

#if ENABLE_AUTO_ROTATE_LANDSCAPE
    /* register hdmi switch*/
    hdmi.hpd_switch.name = "hdmi";

    ret = switch_dev_register(&hdmi.hpd_switch);

    if(ret < 0)
    {
        logw("switch device register error %d\n",ret);
    }
#endif

    /* regist hdmi to fb driver */
    ret = hdmi_regist_fb(pdev);
    if (ret) {
        loge("regist fb device error %d\n", ret);
        goto err;
    }

#ifdef HDMI_HAS_EARLYSUSPEND
    hdmi.early_suspend.suspend = k3_hdmi_early_suspend;
    hdmi.early_suspend.resume = k3_hdmi_late_resume;
    hdmi.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
    register_early_suspend(&hdmi.early_suspend);
#endif

#if USE_HDCP
    hdcp_set_enable(true);
#endif

#if defined(CONFIG_HDMI_CEC_ENABLE)
    hdmi_cec_init();
#endif
    ret = fb_register_client(&k3_fb_event_notifier);
    if (ret) {
        loge("Unable to register framebuffer notifier for HDMI.\n");
    }

    probe_complete = true;
    loge("in enable_hpd, switch_connection: %d\n", switch_connection);
    if (switch_connection) {
        enable_hpd(true);
    }
    //enable_hpd(true);

    hdmi.mhl_probe_success = true;
    OUT_FUNCTION;
    return 0;

err:
    k3_hdmi_remove(pdev);

    hdmi.mhl_probe_success = false;
    OUT_FUNCTION;
    return ret;

err_probe_defer:
    hdmi.mhl_probe_success = false;
    return -EPROBE_DEFER;
}

#if defined(CONFIG_PM) && !defined(HDMI_HAS_EARLYSUSPEND)
/******************************************************************************
* Function:       k3_hdmi_suspend
* Description:    hdmi device suspend when product suspend
* Data Accessed:
* Data Updated:
* Input:          platform device and power manager state
* Output:
* Return:
* Others:
*******************************************************************************/
static int k3_hdmi_suspend(struct platform_device *dev, pm_message_t state)
{
    printk("%s+.\n",__func__);

    hdmi.last_hpd_enabled = hdmi.hpd_enabled;

    printk("%s-.\n",__func__);
    return 0;
}

/******************************************************************************
* Function:       k3_hdmi_resume
* Description:    hdmi device resume when product resume
* Data Accessed:
* Data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
static int k3_hdmi_resume(struct platform_device *dev)
{
    printk("%s+.\n",__func__);

    printk("%s-.\n",__func__);
    return 0;
}
#else
#define k3_hdmi_suspend NULL
#define k3_hdmi_resume NULL
#endif


#ifdef HDMI_HAS_EARLYSUSPEND
/******************************************************************************
* Function:       k3_hdmi_early_suspend
* Description:    hdmi device early suspend when product suspend
* Data Accessed:
* Data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
static void k3_hdmi_early_suspend(struct early_suspend *h)
{
    printk("[%s] +\n", __func__);

    hdmi.last_hpd_enabled = hdmi.hpd_enabled;
    enable_hpd(false);
    printk("[%s] -\n", __func__);
}

/******************************************************************************
* Function:       k3_hdmi_late_resume
* Description:    hdmi device late resume when product resume
* Data Accessed:
* Data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
static void k3_hdmi_late_resume(struct early_suspend *h)
{
    printk("[%s] +\n", __func__);

    if(!hdmi.hpd_enabled){
        enable_hpd(hdmi.last_hpd_enabled);
    }
    printk("[%s] -\n", __func__);
}
#endif


static const struct of_device_id k3_hdmi_match_table[] = {
    {
        .compatible = DTS_COMP_HDMI_NAME,
        .data = NULL,
    },
};
MODULE_DEVICE_TABLE(of, k3_hdmi_match_table);


static struct platform_driver this_driver = {
    .probe   = k3_hdmi_probe,
    .remove  = k3_hdmi_remove,
    .suspend = k3_hdmi_suspend,
    .resume  = k3_hdmi_resume,
    .driver  = {
        .name = "k3_hdmi",
        .of_match_table = of_match_ptr(k3_hdmi_match_table),
    },
};

static int __init k3_hdmi_init(void)
{
    int ret = 0;

    ret = platform_driver_register(&this_driver);
    if (ret) {
        K3_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
        return ret;
    }

    return ret;
}

module_init(k3_hdmi_init);

MODULE_LICENSE("GPL");
