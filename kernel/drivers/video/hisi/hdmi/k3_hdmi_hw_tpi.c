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
#include <linux/clk.h>
#include <linux/clk-private.h>
#include <linux/regulator/consumer.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>

#include "k3_hdmi.h"
#include "k3_edid.h"

#include "k3_hdmi_hw_tpi.h"
#include "k3_hdcp.h"

#define LOG_TAG "hdmi-hw-tpi"
#include "k3_hdmi_log.h"
#include "../k3_fb.h"

extern hdmi_device hdmi;
hdmi_hw_res hw_res = {0};
extern mhl_tx_config g_mhl_tx_config;

#define FLD_MASK(start, end) (((1 << ((start) - (end) + 1)) - 1) << (end))
#define FLD_VAL(val, start, end) (((val) << (end)) & FLD_MASK(start, end))
#define FLD_GET(val, start, end) (((val) & FLD_MASK(start, end)) >> (end))
#define FLD_MOD(orig, val, start, end) \
    (((orig) & ~FLD_MASK(start, end)) | FLD_VAL(val, start, end))
#define TPI_REG_FLD_MOD(page, idx, val, start, end) \
    tpi_write_reg_internal(page, idx, \
        FLD_MOD(tpi_read_reg_internal(page, idx), val, start, end))

void hw_reset_dss_hdmi_on(void);
void hdcp_write(void);

void write_reg(u32 base, u32 idx, u32 val)
{
    writel(val, (volatile void *)(base + idx));
    return;
}

u32 read_reg(u32 base, u32 idx)
{
    return readl((volatile void *)(base + idx));
}
void tpi_write_reg(u32 idx, u32 val)
{
    writel(val, (volatile void *)(HDMI_CORE_SYS + idx));
    return;
}

u32 tpi_read_reg(u32 idx)
{
    return readl((volatile void *)(HDMI_CORE_SYS + idx));
}

void tpi_modify_reg(u32 idx, u32 mask, u32 val)
{
    u32 temp;
    temp = tpi_read_reg(idx);     // Read the current value of the register.
    temp &= ~mask;                // Clear the bits that are set in Mask.
    temp |= (val & mask);         // OR in new value. Apply Mask to Value for safety.
    tpi_write_reg(idx, temp);     // Write new value back to register.
}

void tpi_write_reg_internal(u32 page, u32 idx, u32 val)
{
    tpi_write_reg(TPI_INDEXED_PAGE_REG, page);     // Indexed page
    tpi_write_reg(TPI_INDEXED_OFFSET_REG, idx/4);  // Indexed register
    tpi_write_reg(TPI_INDEXED_VALUE_REG, val);     // Write value
}

u32 tpi_read_reg_internal(u32 page, u32 idx)
{
    tpi_write_reg(TPI_INDEXED_PAGE_REG, page);     // Indexed page
    tpi_write_reg(TPI_INDEXED_OFFSET_REG, idx/4);  // Indexed register
    return tpi_read_reg(TPI_INDEXED_VALUE_REG);    // read value
}

void tpi_modify_reg_internal(u32 page, u32 idx, u32 mask, u32 val)
{
    u32 temp;
    temp = tpi_read_reg_internal(page, idx);      // Read the current value of the register.
    temp &= ~mask;                                // Clear the bits that are set in Mask.
    temp |= (val & mask);                         // OR in new value. Apply Mask to Value for safety.
    tpi_write_reg_internal(page, idx, temp);      // Write new value back to register.
}

void tpi_write_reg_block(u32 idx, u32 count, u8* val)
{
    int i = 0;
    for (; i < count; i++) {
        tpi_write_reg(idx+i*4, val[i]);
    }
    return;
}

/***********************************************************************************
* Function:       hw_core_blank_video
* Description:    set vide blank
* vendor_data Accessed:
* vendor_data Updated:
* Input:          bool blank : true:need blank, false: need not blank
* Output:         NA
* Return:         NA
* Others:
***********************************************************************************/
void hw_core_blank_video(bool blank)
{
    if (blank) {
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_MUTED);
    } else {
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_NORMAL);
    }

    return;
}

/***********************************************************************************
* Function:       hw_core_mute_audio
* Description:    enable or disable audio mute.
* vendor_data Accessed:
* vendor_data Updated:
* Input:          bool benabled: true:enable audio mute, false:disable audio mute
* Output:         NA
* Return:         NA
* Others:
***********************************************************************************/
void hw_core_mute_audio(bool mute)
{
    if (mute) {
        tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_MUTE_MASK, AUDIO_MUTE_MUTED);
    } else {
        tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_MUTE_MASK, AUDIO_MUTE_NORMAL);
    }
    return;
}

/******************************************************************************
* Function:       get_ddc_access
* Description:    Request access to DDC bus from the receiver
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:         sys_ctrl_val:A read of TPI_SYSTEM_CONTROL is invalid while DDC is granted.
*                     Doing so will return 0xFF, and cause an invalid value to be written back.
* Return:         true:can use; false: can't use
* Others:
*******************************************************************************/
bool request_ddc (u32* sys_ctrl_val)
{
    u32 sysCtrl;

    IN_FUNCTION;
    sysCtrl = tpi_read_reg(TPI_SYSTEM_CONTROL_DATA_REG);  // Read and store original value. Will be passed into ReleaseDDC()
    *sys_ctrl_val = sysCtrl;

    tpi_write_reg(TPI_HDCP_CONTROL_DATA_REG, 0x20);   //HDCP

    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, DDC_BUS_GRANT_MASK,DDC_BUS_GRANT_NOT_AVAILABLE);  // register map should be update, bit[1] =0: tpi is handling ddc
    sysCtrl = tpi_read_reg(TPI_SYSTEM_CONTROL_DATA_REG);
    if (!(sysCtrl & 0x02)) {
        logd("TPI_SYSTEM_CONTROL_DATA_REG(0x1A):0x%x\n", sysCtrl);
        OUT_FUNCTION;
        return true;
    } else {
        logd("Get DDC access failed!\n");
        OUT_FUNCTION;
        return false;
    }

}

/******************************************************************************
* Function:       get_ddc_access
* Description:    Release DDC bus
* vendor_data Accessed:
* vendor_data Updated:
* Input:          pre_sys_ctrl_val: sys ctrl val when request
* Output:
* Return:
* Others:
*******************************************************************************/
bool release_ddc(u32 pre_sys_ctrl_val)
{
    #define T_DDC_ACCESS 50
    u32 ddc_req_timeout = T_DDC_ACCESS;
    u32 sys_ctrl_val;
    IN_FUNCTION;

    tpi_modify_reg(TPI_HW_DBG7, REG_HW_DDC_MASTER_MASK, 0x00); //release ddc master
    pre_sys_ctrl_val &= ~BITS_2_1; // Just to be sure bits [2:1] are 0 before it is written

    while (ddc_req_timeout--) {  // Loop till 0x1A[1] reads "0"
        tpi_write_reg(TPI_SYSTEM_CONTROL_DATA_REG, pre_sys_ctrl_val); //step 4
        sys_ctrl_val = tpi_read_reg(TPI_SYSTEM_CONTROL_DATA_REG);

        if (!(sys_ctrl_val & BITS_2_1)) { //step 5
            // When 0x1A[2:1] read "0"
            return true;
        }
    }
    return false;// Failed to release DDC bus control
}

/******************************************************************************
* Function:       hw_core_ddc_edid
* Description:    get edid data from ddc
* vendor_data Accessed:
* vendor_data Updated:
* Input:          index of edid block
* Output:         edid data
* Return:
* Others:
*******************************************************************************/
int read_edid_block(u8 *edid, int ext)
{
    u32 i = 0;
    u32 j = 0;
    u32 fifo_status = 0;
    u8 status = 0;
    u8 OverTime = 0;

    logd("read_edid_block  in , now ext is %d \n",ext);

    //get ddc process
    tpi_write_reg(TPI_HW_OPT3, 0xF6);
    tpi_write_reg(TPI_HW_DBG7, 0x80);

    TPI_REG_FLD_MOD(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_CMD, 0xA, 3, 0);//clock scl devices
    msleep(10);

    if(ext > 0)  {
        edid += 128;
    }

    // from i  0 ~ 7 read 16 byte edid
    for(i = 0; i < 8; i++) {
        OverTime = 10;

        //Step 1: Clear FIFO
        tpi_write_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_CMD, 0x09);

        //Check DDC Status and make sure it's cleared.
        fifo_status = tpi_read_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_STATUS);
        if(0x04 != (fifo_status & (0x04))) {
            loge("DDC fifo cleared error! in func : %s\n",__FUNCTION__);
        }

        //Step 2: Write target slave address
        tpi_write_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_ADDR, 0xA0);
        //msleep(20);

        //Step 3: Write target segment address
        //Default is block 0;

        //Step 4: Write target offset address
        if (0x00 == ext) {
            tpi_write_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_OFFSET, (0x00 + (i * 16)));
        } else {
            tpi_write_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_OFFSET, (0x80 + (i * 16)));  // blockNumber = 0x80 offset
        }

        //Step 5: Write number of bytes to read
        tpi_write_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_COUNT1, 16); //set the size of the EDID infomation
        //msleep(20);
        //Step 6: Begin to read.
        if (!ext) {
            tpi_write_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_CMD, 0x02); //sequential read without ack
        } else {
            tpi_write_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_CMD, 0x04);
        }

        msleep(20);

        while (OverTime--) {
            //Read when FIFO is full.
            status = tpi_read_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_DDC_STATUS);

            if (status & 0x08) {
                for (j = 0; j < 16; j++) {
                    edid[(i * 16) + j] = (u8)tpi_read_reg(HDMI_CORE_DDC_DATA);
                }
                break;
            } else {
                msleep(1);
            }
        }
    }

    return 0;
}


/******************************************************************************
* Function:       hw_core_read_edid
* Description:    read edid data
* vendor_data Accessed:
* vendor_data Updated:
* Input:          max length to read
* Output:         edid data
* Return:
* Others:
*******************************************************************************/
int hw_core_read_edid(u8 *edid, u16 max_length)
{

    u32 sys_ctrl_reg;
    int ret;
    int ext_num;
    int i = 0;
    int max_ext_blocks = (max_length / HDMI_EDID_BLOCK_LENGTH) - 1;

    BUG_ON(NULL == edid);

    if (max_length < HDMI_EDID_BLOCK_LENGTH) {
        loge("input max length invald(valid: >=128): %d.\n", max_length);
        return -EINVAL;
    }

    // Request access to DDC bus from the receiver
    if (request_ddc(&sys_ctrl_reg)) {
        // read first 128 bytes of EDID ROM
        ret = read_edid_block(edid, 0);
        if (ret) {
            release_ddc(sys_ctrl_reg);
            loge("hw read_edid_block 0 fail: %d.\n", ret);
            return ret;
        }
        ext_num = edid[NUM_OF_EXTEN_ADDR];
        if (ext_num > max_ext_blocks) {
            ext_num = max_ext_blocks;
        }
        if (ext_num > 0) {
            logi("ext_num:%d\n", ext_num);
            for (i = 1; i <= ext_num; i++) {
                // Parse 861 Extensions (short and long descriptors)
                ret = read_edid_block(edid, ext_num);
                if (ret) {
                    release_ddc(sys_ctrl_reg);
                    loge("hw read_edid_block %d fail :%d\n", i, ret);
                    return ret;
                }
            }
        }
        // Host must release DDC bus once it is done reading EDID
        if (!release_ddc(sys_ctrl_reg)) {
            loge("EDID -> DDC bus release failed\n");
            return -1;
        }
    }  else {
        loge("EDID -> DDC bus request failed\n");
        return -1;
    }

    return 0;
}

/******************************************************************************
* Function:       hw_core_init
* Description:    init default parameter by deep color
* vendor_data Accessed:
* vendor_data Updated:
* Input:          deep color
* Output:         video and audio configure, avi, packet repeat
* Return:
* Others:
*******************************************************************************/
static void hw_core_init(hdmi_deep_color_mode deep_color,
                         hw_audio_configure *audio_cfg,
                         hdmi_core_infoframe_avi *avi,
                         hdmi_tpi_video_config* tpi_v_cfg)
{
    hdmi_s3d_config s3d_param = {0};
    BUG_ON((NULL == tpi_v_cfg) || (NULL == audio_cfg) || (NULL == avi));

    IN_FUNCTION;

    /* audio core */
    audio_cfg->fs = FS_48000;
    audio_cfg->n = 6144;
    audio_cfg->layout = LAYOUT_2CH;
    audio_cfg->if_fs = IF_FS_NO;
    audio_cfg->if_channel_number = 2;
    audio_cfg->if_sample_size = IF_NO_PER_SAMPLE;
    audio_cfg->if_audio_channel_location = HDMI_CEA_CODE_00;
    audio_cfg->i2schst_max_word_length = I2S_CHST_WORD_MAX_20;
    audio_cfg->i2schst_word_length = I2S_CHST_WORD_16_BITS;
    audio_cfg->i2s_in_bit_length = I2S_IN_LENGTH_16;
    audio_cfg->i2s_justify = HDMI_AUDIO_JUSTIFY_LEFT;
    audio_cfg->aud_par_busclk = 0;
    audio_cfg->bsio = true;
    audio_cfg->cts_mode = CTS_MODE_HW;

    /* info frame */
    avi->db1y_rgb_yuv422_yuv444 = INFOFRAME_AVI_DB1Y_RGB;
    avi->db1a_active_format_off_on = INFOFRAME_AVI_DB1A_ACTIVE_FORMAT_OFF;
    avi->db1b_no_vert_hori_verthori = INFOFRAME_AVI_DB1B_NO;
    avi->db1s_0_1_2 = INFOFRAME_AVI_DB1S_0;
    avi->db2c_no_itu601_itu709_extented = INFOFRAME_AVI_DB2C_NO;
    avi->db2m_no_43_169 = INFOFRAME_AVI_DB2M_NO;
    avi->db2r_same_43_169_149 = INFOFRAME_AVI_DB2R_SAME;
    avi->db3itc_no_yes = INFOFRAME_AVI_DB3ITC_NO;
    avi->db3ec_xvyuv601_xvyuv709 = INFOFRAME_AVI_DB3EC_XVYUV601;
    avi->db3q_default_lr_fr = INFOFRAME_AVI_DB3Q_DEFAULT;
    avi->db3sc_no_hori_vert_horivert = INFOFRAME_AVI_DB3SC_NO;
    avi->db4vic_videocode = 0;
    avi->db5pr_no_2_3_4_5_6_7_8_9_10 = INFOFRAME_AVI_DB5PR_NO;
    avi->db6_7_lineendoftop = 0 ;
    avi->db8_9_linestartofbottom = 0;
    avi->db10_11_pixelendofleft = 0;
    avi->db12_13_pixelstartofright = 0;

    //tpi video config
    tpi_v_cfg->pix_repeat = hdmi.timings->pix_rep;
    tpi_v_cfg->tclk_sel = TPI_TCLK_SEL_1X;
    tpi_v_cfg->clk_edge = TPI_CLK_EDGE_RISING;
    tpi_v_cfg->use_embedded_sync = false;
    tpi_v_cfg->use_de = true;
    tpi_v_cfg->input_color_space = TPI_COLOR_SPACE_RGB;

    switch(deep_color)
    {
        case HDMI_DEEP_COLOR_30BIT:
            tpi_v_cfg->input_color_depth = TPI_COLOR_DEPTH_10BIT;
            tpi_v_cfg->output_dither = TPI_DEPTH_DITHER_ENSABLE;
            break;
        case HDMI_DEEP_COLOR_36BIT:
            tpi_v_cfg->input_color_depth = TPI_COLOR_DEPTH_12BIT;
            tpi_v_cfg->output_dither = TPI_DEPTH_DITHER_ENSABLE;
            break;
        case HDMI_DEEP_COLOR_48BIT:
            tpi_v_cfg->input_color_depth = TPI_COLOR_DEPTH_16BIT;
            tpi_v_cfg->output_dither = TPI_DEPTH_DITHER_ENSABLE;
            break;
        case HDMI_DEEP_COLOR_24BIT:
        default:
            tpi_v_cfg->input_color_depth = TPI_COLOR_DEPTH_8BIT;
            tpi_v_cfg->output_dither = TPI_DEPTH_DITHER_DISABLE;
    }

    //get  s3d  config
    s3d_param.structure = hdmi.cfg.s3d_structure;
    s3d_param.s3d_ext_data = hdmi.cfg.subsamp_pos;

    tpi_v_cfg->input_range = HDMI_VIDEO_RANGE_AUTO;
    tpi_v_cfg->output_color_space = TPI_COLOR_SPACE_RGB;
    tpi_v_cfg->output_color_depth = TPI_COLOR_DEPTH_8BIT;
    tpi_v_cfg->output_range = HDMI_VIDEO_RANGE_AUTO;
    tpi_v_cfg->output_color_standard = TPI_COLOR_STANDARD_BT601;
    tpi_v_cfg->s3d_config = s3d_param;
    tpi_v_cfg->vic = hdmi.code;
    tpi_v_cfg->video_format = hdmi.cfg.video_format;
    tpi_v_cfg->aspect_ratio = hdmi.timings->aspect_ratio;
    tpi_v_cfg->is_hdmi = hdmi.cfg.hdmi_dvi;

    OUT_FUNCTION;
    return;
}

/******************************************************************************
* Function:       hw_core_swreset_release
* Description:    Normal Operation
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_core_swreset_release(void)
{
    IN_FUNCTION;

    tpi_modify_reg(TPI_SW_RST_ADDR, TPI_REG_SW_RST_MASK, TPI_SW_RST_RELEASE);

    OUT_FUNCTION;
    return;
}

/******************************************************************************
* Function:       hw_core_swreset_assert
* Description:    Reset all sections, including audio FIFO, but not writable registers or HDCP
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_core_swreset_assert(void)
{
    IN_FUNCTION;

    tpi_modify_reg(TPI_SW_RST_ADDR, TPI_REG_SW_RST_MASK, TPI_SW_RST_REQUEST);

    OUT_FUNCTION;
    return;
}

void hw_init_video(hdmi_tpi_video_config *video_conf)
{
#define VIDEO_PARAM_DATA_LEN 8
    u8 video_data[VIDEO_PARAM_DATA_LEN];
    u32 temp_color_depth = 0;
    BUG_ON(NULL == video_conf);

    memset(video_data, 0, VIDEO_PARAM_DATA_LEN);

    /* TPI 0x08[7:6] TClkSel1 Ratio of output TMDS clock to input video clock */
    tpi_modify_reg(TPI_PIX_REPETITION, TPI_VIDEO_TCLK_SEL_MASK, video_conf->tclk_sel & TWO_MSBITS);

    // Set VIDEO MODE DATA
    // write Pixel clock to TPI registers 0x00, 0x01
    video_data[0] = (hdmi.timings->pixel_clock / 10) & 0x00FF;
    video_data[1] = ((hdmi.timings->pixel_clock / 10) >> 8) & 0xFF;
    // write Vertical Frequency to TPI registers 0x02, 0x03
    video_data[2] = hdmi.timings->freq & 0x00FF;
    video_data[3] = (hdmi.timings->freq >> 8) & 0xFF;
    // write total number of pixels to TPI registers 0x04, 0x05
    video_data[4] = (hdmi.timings->x_res + hdmi.timings->hbp + hdmi.timings->hfp + hdmi.timings->hsw) & 0x00FF;
    video_data[5] = ((hdmi.timings->x_res + hdmi.timings->hbp + hdmi.timings->hfp + hdmi.timings->hsw) >> 8) & 0xFF;
    // write total number of lines to TPI registers 0x06, 0x07
    video_data[6] = (hdmi.timings->y_res + hdmi.timings->vbp + hdmi.timings->vfp + hdmi.timings->vsw) & 0x00FF;
    video_data[7] = (hdmi.timings->y_res + hdmi.timings->vbp + hdmi.timings->vfp + hdmi.timings->vsw) & 0xFF;
    // Write TPI Mode data.
    tpi_write_reg_block(TPI_VIDEO_MODE_DATA_REG, VIDEO_PARAM_DATA_LEN, video_data);

    video_data[0]  = (hdmi.timings->pix_rep) & LOW_BYTE; // Set pixel replication field of 0x08
    video_data[0] |= BIT_FULL_PIXEL_WIDE; // Set 24 bit bus

    video_data[0] |= video_conf->tclk_sel & TWO_MSBITS;

    if (TPI_CLK_EDGE_FALLING == video_conf->clk_edge) {
        video_data[0] &= ~BIT_EDGE_RISE; // Set to falling edge
    } else {
        video_data[0] |= BIT_EDGE_RISE;  // Set to rising edge
    }

    if (video_conf->input_color_depth == TPI_COLOR_DEPTH_16BIT) {
        // 16 bit Deep Color. Forcer 0x08[5] to 0 for half pixel mode
        video_data[0] &= ~BIT_FULL_PIXEL_WIDE;
    }
    tpi_write_reg(TPI_PIX_REPETITION, video_data[0]); // 0x08

    video_data[0] = video_conf->input_color_space;
    video_data[0] |= video_conf->input_range;

    if (VMD_COLOR_DEPTH_8BIT != video_conf->input_color_depth) {
        temp_color_depth = BIT_2;
    }

    video_data[0] = ((video_data[0] & 0x3F) | video_conf->input_color_depth); // Set Color Depth bits

    video_data[1] = video_conf->output_color_space;
    video_data[1] |= video_conf->output_range;
    video_data[1] |= video_conf->output_color_standard;
    video_data[1] |= video_conf->output_dither;
    video_data[1] = ((video_data[1] & 0x3F) | video_conf->output_color_depth);

    if (VMD_COLOR_DEPTH_8BIT != video_conf->output_color_depth) {
        temp_color_depth = BIT_2;
    }

    tpi_modify_reg(TPI_DEEP_COLOR_GCP, BIT_2, temp_color_depth);

    tpi_write_reg_block(TPI_INPUT_FORMAT_REG, 2, video_data);// Program TPI AVI Input and Output Format

    if (video_conf->is_hdmi) {
        tpi_write_reg(TPI_END_RIGHT_BAR_MSB, 0x00); // Set last uint8_t of TPI AVI InfoFrame for TPI AVI I/O Format to take effect
    }

    return ;
}

/******************************************************************************
* Function:       set_avi_infoframe
* Description:    Construct AVI InfoFrame video.
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
static void set_avi_infoframe(hdmi_core_infoframe_avi *info_avi)
{
    u32 sum = (0x82 + 0x02 + 0x0D);
    int i = 0;
    u8 avi_data[SIZE_AVI_INFOFRAME];

    IN_FUNCTION;
    memset(avi_data, 0, SIZE_AVI_INFOFRAME);

    // Disable transmission of AVI InfoFrames during re-configuration
    tpi_write_reg(MISC_INFO_FRAMES_CTRL, DISABLE_AVI); // Disbale MPEG/Vendor Specific InfoFrames

    avi_data[1] = (info_avi->db1y_rgb_yuv422_yuv444 << 5) |
          (info_avi->db1a_active_format_off_on << 4) |
          (info_avi->db1b_no_vert_hori_verthori << 2) |
          (info_avi->db1s_0_1_2);

    avi_data[2] = (info_avi->db2c_no_itu601_itu709_extented << 6) |
          (info_avi->db2m_no_43_169 << 4) |
          (info_avi->db2r_same_43_169_149);

    avi_data[3] = (info_avi->db3itc_no_yes << 7) |
          (info_avi->db3ec_xvyuv601_xvyuv709 << 4) |
          (info_avi->db3q_default_lr_fr << 2) |
          (info_avi->db3sc_no_hori_vert_horivert);

    avi_data[4] = info_avi->db4vic_videocode;
    avi_data[5] = info_avi->db5pr_no_2_3_4_5_6_7_8_9_10;

    avi_data[6] = info_avi->db6_7_lineendoftop & 0x00FF;

    avi_data[7] = ((info_avi->db6_7_lineendoftop >> 8) & 0x00FF);

    avi_data[8] = info_avi->db8_9_linestartofbottom & 0x00FF;

    avi_data[9] = ((info_avi->db8_9_linestartofbottom >> 8) & 0x00FF);

    avi_data[10] = info_avi->db10_11_pixelendofleft & 0x00FF;

    avi_data[11] = ((info_avi->db10_11_pixelendofleft >> 8) & 0x00FF);

    avi_data[12] = info_avi->db12_13_pixelstartofright & 0x00FF;

    avi_data[13] = ((info_avi->db12_13_pixelstartofright >> 8) & 0x00FF);

    for (; i < SIZE_AVI_INFOFRAME; i++) {
        sum += avi_data[i];
    }
    avi_data[0] = (0x100 - sum);

    /*
      Write the Inforframe data to the TPI Infoframe registers.  This automatically
      Enables InfoFrame transmission and repeat by over-writing Register 0x19.
    */
    tpi_write_reg_block(TPI_AVI_BYTE_0, SIZE_AVI_INFOFRAME, avi_data);
    tpi_write_reg(TPI_END_RIGHT_BAR_MSB, 0x0);

    OUT_FUNCTION;
    return;
}

void set_de(void)
{
    //todo
}
void set_embedded_sync(void)
{
    //todo
}
void enable_embedded_sync(void)
{
    //todo
}


void hw_enable_tmds(void)
{
    /*tmds_oe enable*/
    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_ACTIVE);
    REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x1, 5, 5);
}

void hw_disable_tmds(void)
{
    /*0xFA205804[5]= 0 phy output disable*/
    REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x0, 5, 5);
    /*TMDS Output Control 0 Active 1 Powerdown(default)
           AV Mute 0 Normal audio/video 1 Mute HDMI audio/video at receiver*/
    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK,
                       TMDS_OUTPUT_CONTROL_POWER_DOWN | AV_MUTE_MUTED);
}

void hw_set_video_mode(hdmi_tpi_video_config *video_conf)
{
    IN_FUNCTION;

    // setp 1: host TPI: Blank the Display
    //hw_disable_tmds(); // turn off TMDS output

    // allow control InfoFrames to pass through to the sink device.
    msleep(T_RES_CHANGE_DELAY);

    /* todo:
    step 2: host TPI: Prepare for Resolution Change.
    Write TX PHY register 0x01[5] = 0 to start the resolution change process
    */
    //todo why init twice???
    if (video_conf->output_color_depth == TPI_COLOR_DEPTH_8BIT) {
        // Yes it is, so force 16bpps first!
        video_conf->output_color_depth = TPI_COLOR_DEPTH_16BIT;

        // Initialize mode with IDCLK set the multiplier
        hw_init_video(video_conf);

        // Now put it back to 8bit and go do the expected InitVideo() call
        video_conf->output_color_depth = TPI_COLOR_DEPTH_8BIT;
    }

    hw_init_video(video_conf);

    if (video_conf->is_hdmi) { // sink suppurts HDMI
        // Set 0x1A[0] to HDMI
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, OUTPUT_MODE_MASK, OUTPT_MODE_HDMI);
        // InfoFrames - only if output mode is HDMI
        set_avi_infoframe(&hdmi.avi_param);
    } else {
        // Set 0x1A[0] to DVI
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, OUTPUT_MODE_MASK, OUTPT_MODE_DVI);
    }

    // set de and reinit input param
    // THIS PATCH IS NEEDED BECAUSE SETTING UP AVI InfoFrames CLEARS 0x63
    if (video_conf->use_de) {
        u32 video_data = 0;
        tpi_modify_reg(TPI_SYNC_GEN_CTRL, MSBIT, 0); // set 0x60[7] = 0 for External Sync
        set_de(); // Call set_de() with Video Mode as a parameter
        video_data = (((BITS_IN_YCBCR422 | BITS_IN_AUTO_RANGE) & ~BIT_EN_DITHER_10_8) & ~BIT_EXTENDED_MODE); // 0x09
        video_data = ((video_data & 0x3F) | video_conf->input_color_depth);    // Set Color Depth bits

        if (video_conf->use_embedded_sync) {
            video_data = ((video_data & 0xFC) | 0x02A);   // Force YCbCr4:2:2 and Video Range Expansion OFF
        }

        tpi_write_reg(TPI_INPUT_FORMAT_REG, video_data);   // Program TPI AVI Input and Output Format
    }
    if (video_conf->use_embedded_sync) {
        set_embedded_sync();
        enable_embedded_sync();
    }
    // todo: YC Input Mode Select - Command.Arg[6] - offset 0x0B

    //hw_enable_tmds();

    return;
}

/******************************************************************************
* Function:       set_audio_infoframe
* Description:    Audio info frame setting refer to CEA-861-d spec p75
* Input:          audio config
* Output:
* Return:
* Others:
*******************************************************************************/
static void set_audio_infoframe(hw_audio_configure *audio_cfg)
{
    u8 audio_data[SIZE_AUDIO_INFOFRAME];  //15
    int i;

    IN_FUNCTION;

    memset(audio_data, 0, SIZE_AUDIO_INFOFRAME);

    //  Disbale MPEG/Vendor Specific InfoFrames
    tpi_write_reg(MISC_INFO_FRAMES_CTRL, DISABLE_AUDIO);

    audio_data[0] = TYPE_AUDIO_INFOFRAMES;      // 0x84
    audio_data[1] = MISC_INFOFRAMES_VERSION;    // 0x01
    audio_data[2] = AUDIO_INFOFRAMES_LENGTH;    // 0x0A
    audio_data[3] = TYPE_AUDIO_INFOFRAMES +      // Calculate checksum - 0x84 + 0x01 + 0x0A
                    MISC_INFOFRAMES_VERSION +
                    AUDIO_INFOFRAMES_LENGTH;

    //for mhl Certification, DB1 must be 0;
    if (TPI_AUDIO_TYPE_PCM == audio_cfg->audiotype) {
        audio_data[4] = (audio_cfg->if_channel_number - 1);  // 0 for "Refer to Stream Header" or for 2 Channels. 0x07 for 8 Channels;
        audio_data[5] = (audio_cfg->if_fs << 2) + audio_cfg->if_sample_size;
    } else {
        audio_data[4] = (audio_cfg->audiotype << 4) + (audio_cfg->if_channel_number - 1);
        audio_data[5] = (audio_cfg->if_fs << 2) + audio_cfg->if_sample_size;
    }

    /* channel location according to CEA spec */
    audio_data[7] = audio_cfg->if_audio_channel_location;;

    for (i = 4; i < SIZE_AUDIO_INFOFRAME; i++) {
        audio_data[3] += audio_data[i];
    }

    audio_data[3] = 0x100 - audio_data[3];

    tpi_write_reg(MISC_INFO_FRAMES_CTRL, EN_AND_RPT_AUDIO);// Re-enable Audio InfoFrame transmission and repeat

    tpi_write_reg_block(MISC_INFO_FRAMES_TYPE, SIZE_AUDIO_INFOFRAME, audio_data);

    OUT_FUNCTION;

    return ;
}

/******************************************************************************
* Function:       hw_core_audio_mode_enable
* Description:    Audio input stream enable
* vendor_data Accessed:
* vendor_data Updated:
* Input:          base address of HDMI IP Audio and video
* Output:
* Return:
* Others:
*******************************************************************************/
#if HDMI_USE_NOW
static void hw_core_audio_mode_disable(void)
{
    tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_INTERFACE_MASK, AUDIO_INTERFACE_DISABLED);
    hw_core_mute_audio(true);
    return;
}
#endif
static void hw_core_audio_mode_enable(u32  layout)
{
    tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_INTERFACE_MASK, layout);
    hw_core_mute_audio(false);
    return;
}

static void hw_core_audio_downsample(hw_audio_configure *audio_cfg)
{
    u8 downsamp_ratio = 0x00; //0x00----2:1, 0x02----4:1

    IN_FUNCTION;

    if (audio_cfg->layout != LAYOUT_2CH) {
        return;
    }

    if (hdmi.sample_freq <= 48000) {
        return;
    }

    switch(audio_cfg->fs) {
        case FS_88200 :
        case FS_96000 :
            downsamp_ratio = 0x00;
            break;
        case FS_176400 :
        case FS_192000 :
            downsamp_ratio = 0x20;
            break;
        default:
            downsamp_ratio = 0x00;
            break;
    }

    tpi_modify_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_ASRC, BIT_DOWNSAMPLE_RATIO, downsamp_ratio);

    if (true == audio_cfg->bsio) {
        tpi_modify_reg(TPI_I2S_EN, TPI_I2S_AUDIO_DOWN_SAMPLE_MASK, TPI_I2S_AUDIO_DOWN_SAMPLE_ENABLE);
        logd("TPI_I2S_EN:0x%x\n",tpi_read_reg(TPI_I2S_EN));
    } else {
        tpi_modify_reg(TPI_AUDIO_HANDLING, TPI_SPDIF_AUDIO_DOWN_SAMPLE_MASK, TPI_SPDIF_AUDIO_DOWN_SAMPLE_ENABLE);
        logd("TPI_AUDIO_HANDLING:0x%x\n",tpi_read_reg(TPI_AUDIO_HANDLING));
    }

    OUT_FUNCTION;
}
/******************************************************************************
* Function:       hw_core_audio_config
* Description:    set audio config
* vendor_data Accessed:
* vendor_data Updated:
* Input:          audio config
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_configure_audio(u32 base_addr, hw_audio_configure *audio_cfg)
{
    u8 val = 0;
    u8 i2s_fs = 0;
    u8 i2s_size = 0;

    tpi_modify_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_ACR_CTRL, BIT2, 0x0);

    IN_FUNCTION;
    if (true == audio_cfg->bsio) {  //1.Ensure that a valid I2S audio stream is coming into the transmitter.

        //2. Select I2S input mode using register bits 0x26[7:6], with Mute enabled (bit [4] = 1).
        tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_INTERFACE_MASK, AUDIO_INTERFACE_I2S);
        tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_MUTE_MASK, AUDIO_MUTE_MUTED);
        logd("TPI_AUDIO_INTERFACE_REG(0x26):0x%x\n", tpi_read_reg(TPI_AUDIO_INTERFACE_REG));
        //3. Write register 0x20 to select the general incoming SD format.
        tpi_write_reg(TPI_I2S_IN_CFG, 0x90);
        //4. Write register 0x1F up to four times, to program each of the SD inputs.
        if (2 == audio_cfg->if_channel_number) {
            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK| TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD0 | TPI_AUDIO_SD_FIFO0);
        } else if (4 == audio_cfg->if_channel_number) {
            audio_cfg->if_audio_channel_location = HDMI_CEA_CODE_03;

            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK| TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD0 | TPI_AUDIO_SD_FIFO0);
            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK| TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD1 | TPI_AUDIO_SD_FIFO1);
        } else if (6 == audio_cfg->if_channel_number) {
            audio_cfg->if_audio_channel_location = HDMI_CEA_CODE_0B;

            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK | TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD0 | TPI_AUDIO_SD_FIFO0);
            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK | TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD1 | TPI_AUDIO_SD_FIFO1);
            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK | TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD2 | TPI_AUDIO_SD_FIFO2);
        } else if (8 == audio_cfg->if_channel_number) {
            //test for channel mapping,need to modify infoframe speaker location
            audio_cfg->if_audio_channel_location = HDMI_CEA_CODE_13;

            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK | TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD2 | TPI_AUDIO_SD_FIFO0);
            logi("TPI_I2S_EN(0x1f): 0x%02x\n", tpi_read_reg(TPI_I2S_EN));
            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK | TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD1 | TPI_AUDIO_SD_FIFO1);
            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK | TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD2 | TPI_AUDIO_SD_FIFO2);
            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK | TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD3 | TPI_AUDIO_SD_FIFO3);
        } else {
            tpi_modify_reg(TPI_I2S_EN, TPI_AUDIO_SD_ENABLE_MASK | TPI_AUDIO_SD_MASK | TPI_AUDIO_SD_FIFO_MASK, TPI_AUDIO_SD_ENABLE | TPI_AUDIO_SD_SD0 | TPI_AUDIO_SD_FIFO0);
        }
        logd("TPI_I2S_EN(0x1F):0x%x\n", tpi_read_reg(TPI_I2S_EN));
        // 5. Program register bits 0x27[5:3] with the correct audio rate.
        switch (audio_cfg->fs) {
            case FS_32000:
                val |= TPI_AUDIO_FS_32000;
                i2s_fs = TPI_AUDIO_I2S_FS_32000;
                break;
            case FS_44100:
                val |= TPI_AUDIO_FS_44100;
                i2s_fs = TPI_AUDIO_I2S_FS_44100;
                break;
            case FS_48000:
                val |= TPI_AUDIO_FS_48000;
                i2s_fs = TPI_AUDIO_I2S_FS_48000;
                break;
            case FS_96000:
                val |= TPI_AUDIO_FS_96000;
                i2s_fs = TPI_AUDIO_I2S_FS_96000;
                break;
            case FS_176400:
                val |= TPI_AUDIO_FS_176400;
                i2s_fs = TPI_AUDIO_I2S_FS_176400;
                break;
            case FS_192000:
                val |= TPI_AUDIO_FS_192000;
                i2s_fs = TPI_AUDIO_I2S_FS_192000;
                break;
            case FS_88200:
                val |= TPI_AUDIO_FS_88200;
                i2s_fs = TPI_AUDIO_I2S_FS_88200;
                break;
            case FS_NOT_INDICATED:
            default:
                val |= TPI_AUDIO_FS_REFER;
        }
        tpi_write_reg(TPI_AUDIO_SAMPLE_CTRL,val);
        //6. Program registers 0x21 25 with the correct header information for the stream that will be sent over HDMI.
        tpi_modify_reg(TPI_I2S_CHST_3, TPI_I2S_FS_MASK, i2s_fs);//96k needed
        switch (audio_cfg->if_sample_size) {
            case IF_16BIT_PER_SAMPLE:
                i2s_size = TPI_AUDIO_I2S_SIZE_16;
                break;
            case IF_20BIT_PER_SAMPLE:
                i2s_size = TPI_AUDIO_I2S_SIZE_20;
                break;
            case IF_24BIT_PER_SAMPLE:
                i2s_size = TPI_AUDIO_I2S_SIZE_24;
                break;
            default:
                i2s_size = TPI_AUDIO_I2S_SIZE_16;
                break;
        }
        tpi_modify_reg(TPI_I2S_CHST_4, TPI_I2S_SAMPLE_LEN_MASK, i2s_size);
        //7. Write registers 0xBF CD with the appropriate Audio InfoFrame information.
        set_audio_infoframe(audio_cfg);
        /*8. Set the audio packet header layout indicator to 2-channel or multi-channel mode as needed using
           the sequence described below. Note that Audio InfoFrame byte 1 must also have this same setting.*/
        if (LAYOUT_2CH == audio_cfg->layout){
            tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_LAYOUT_MASK, AUDIO_LAYOUT_2CH);
        } else {
            tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_LAYOUT_MASK, AUDIO_LAYOUT_8CH);
        }
        //9. Again write register 0x26 with I2S selected, this time with Mute disabled (bit [4] = 0).
        tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_INTERFACE_MASK, AUDIO_INTERFACE_I2S);
        tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_MUTE_MASK, AUDIO_MUTE_NORMAL);
        logd("TPI_AUDIO_INTERFACE_REG(0x26):0x%x\n", tpi_read_reg(TPI_AUDIO_INTERFACE_REG));
    } else {
        // SPDIF out put
        tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_INTERFACE_MASK, AUDIO_INTERFACE_SPDIF);
        //Disable FS override
        tpi_modify_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_SPDIF_CTRL, BIT1, 0x0);
    }
#if 0
    //DSD
    if (audio_cfg->audiotype != 1) {
        tpi_modify_reg(TPI_AUDIO_INTERFACE_REG, AUDIO_INTERFACE_MASK, AUDIO_INTERFACE_SPDIF);
    }
#endif
    /* N */
    logd("n = %d\n", audio_cfg->n);

    logd("n1 = 0x%x\n", tpi_read_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_N_SVAL1));
    logd("n2 = 0x%x\n", tpi_read_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_N_SVAL2));
    logd("n3 = 0x%x\n", tpi_read_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_N_SVAL3));

    logd("HCTS1 = 0x%x\n", tpi_read_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_CTS_HVAL1));
    logd("HCTS2 = 0x%x\n", tpi_read_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_CTS_HVAL2));
    logd("HCTS3 = 0x%x\n", tpi_read_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_CTS_HVAL3));

    logd("ACR CTRL: 0x%x\n", tpi_read_reg_internal(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_ACR_CTRL));
    logd("TPI_AUDIO_INTERFACE_REG(0x26):0x%x\n", tpi_read_reg(TPI_AUDIO_INTERFACE_REG));

    if (hdmi.downsample_needed) {
        hw_core_audio_downsample(audio_cfg);
    }

    OUT_FUNCTION;
    return;
}

/******************************************************************************
* Function:       hw_core_audio_infoframe_avi
* Description:    set avi info
* vendor_data Accessed:
* vendor_data Updated:
* Input:          avi info
* Output:
* Return:
* Others:
*******************************************************************************/
static void hw_core_audio_infoframe_avi(hdmi_core_infoframe_avi info_avi)
{
    set_avi_infoframe(&info_avi);
    return ;
}

/******************************************************************************
* Function:       hw_configure_lrfr
* Description:    set lrfr config
* vendor_data Accessed:
* vendor_data Updated:
* Input:          lr fr config
* Output:
* Return:
* Others:
*******************************************************************************/
int hw_configure_lrfr(hdmi_range lr_fr)
{
    /*
     * Setting the AVI infroframe to respective limited range
     * 0 if for limited range 1 for full range
     */
    switch (lr_fr) {
        case HDMI_LIMITED_RANGE:
            hdmi.avi_param.db3q_default_lr_fr = INFOFRAME_AVI_DB3Q_LR;
            hw_core_audio_infoframe_avi(hdmi.avi_param);
            tpi_modify_reg(TPI_OUTPUT_FORMAT_REG,TPI_OUTPUT_QUAN_RANGE_MASK, HDMI_VIDEO_RANGE_LIMIT);
            break;
        case HDMI_FULL_RANGE:
            if (INFOFRAME_AVI_DB1Y_YUV422 == hdmi.avi_param.db1y_rgb_yuv422_yuv444) {
                loge("It is only limited range for YUV\n");
                return -1;
            }

            hdmi.avi_param.db3q_default_lr_fr = INFOFRAME_AVI_DB3Q_FR;
            hw_core_audio_infoframe_avi(hdmi.avi_param);
            tpi_modify_reg(TPI_OUTPUT_FORMAT_REG,TPI_OUTPUT_QUAN_RANGE_MASK, HDMI_VIDEO_RANGE_FULL);
            break;
        default:
            BUG_ON(NULL == NULL);
            break;
    }

    return 0;
}

/******************************************************************************
* Function:       hw_configure_acr
* Description:    config n and cts by pclk, fs and deep_color
* vendor_data Accessed:
* vendor_data Updated:
* Input:          pixel clock, audio fs
* Output:
* Return:
* Others:
*******************************************************************************/
int hw_configure_acr(u32 pclk, hdmi_core_fs audio_fs)
{
    u32 fs          = 0;
    u32 n           = 0;
    u32 cts         = 0;

    IN_FUNCTION;

    switch (audio_fs) {
        case FS_32000:
            fs = 32000;
            n = 4096;
            break;
        case FS_44100:
            fs = 44100;
            n = 6272;
            break;
        case FS_48000:
            fs = 48000;
            n = 6144;
            break;
        case FS_96000:
            fs = 96000;
            n = 12288;
            break;
        case FS_176400:
            fs = 176400;
            n = 25088;
            break;
        case FS_192000:
            fs = 192000;
            n = 24576;
            break;
        case FS_88200:
        case FS_NOT_INDICATED:
        default:
            loge("fs is invalid: %d.\n", audio_fs);
            OUT_FUNCTION;
            return -EINVAL;
    }

    /* Process CTS */
    cts = (pclk * (n / 128) * 10) / (fs / 100);

    hdmi.audio_core_cfg.n = n;
    hdmi.audio_core_cfg.cts = cts;

    OUT_FUNCTION;
    return 0;
}


/******************************************************************************
* Function:       hw_enable
* Description:    config hdmi by hdmi config or default
* vendor_data Accessed:
* vendor_data Updated:
* Input:          hdmi configs
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_enable(hdmi_config *cfg)
{
    /* HDMI core */
    hdmi_tpi_video_config tpi_v_cfg = {0};
    IN_FUNCTION;

    BUG_ON(NULL == cfg);
#ifdef CONFIG_CPU_FREQ_GOV_K3HOTPLUG
    {
        int hdmi_ddr_freq = HDMI_PM_QOS_DDR_MIN_FREQ;
        if (hdmi.has_request_ddr) {
            pm_qos_remove_request(&hdmi.qos_request);
        }

        if ((HDMI_EDID_EX_VIDEO_1920x1080p_50Hz_16_9 == hdmi.code || HDMI_EDID_EX_VIDEO_1920x1080p_60Hz_16_9 == hdmi.code)
            && HDMI_CODE_TYPE_CEA == hdmi.mode) {
            hdmi_ddr_freq = HDMI_1080P50_60_DDR_MIN_FREQ;
        }
        pm_qos_add_request(&hdmi.qos_request, PM_QOS_DDR_MIN_PROFILE, hdmi_ddr_freq);
        hdmi.has_request_ddr = true;
        logi("hw_enable set ddr freq:%d\n", hdmi_ddr_freq);
    }
#endif

    hw_core_init(cfg->deep_color,
                 &hdmi.audio_core_cfg,
                 &hdmi.avi_param,
                 &tpi_v_cfg);

    /****************************** CORE *******************************/
    /************* configure core video part ********************************/
    if (!hdmi.support_mhl_interface) {
        hw_core_swreset_assert();
    }

    hdmi.avi_param.db1y_rgb_yuv422_yuv444 = INFOFRAME_AVI_DB1Y_RGB;

    hdmi.avi_param.db1a_active_format_off_on = INFOFRAME_AVI_DB1A_ACTIVE_FORMAT_OFF;
    hdmi.avi_param.db1b_no_vert_hori_verthori = INFOFRAME_AVI_DB1B_NO;
    hdmi.avi_param.db1s_0_1_2 = INFOFRAME_AVI_DB1S_0;
    hdmi.avi_param.db2c_no_itu601_itu709_extented = INFOFRAME_AVI_DB2C_NO;

    /* Support AR in AVI infoframe */
    switch (cfg->video_format) {
            /* 16:9 */
        case HDMI_EDID_EX_VIDEO_720x480p_60Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1280x720p_60Hz_16_9:
        case HDMI_EDID_EX_VIDEO_720x240p_60Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1920x1080i_60Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1920x1080p_60Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1440x480p_60Hz_16_9:
        case HDMI_EDID_EX_VIDEO_720x576p_50Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1280x720p_50Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1920x1080i_50Hz_16_9:
        case HDMI_EDID_EX_VIDEO_720x288p_50Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1440x576p_50Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1920x1080p_50Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1920x1080p_24Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1920x1080p_25Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1920x1080p_30Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1280x720p_100Hz_16_9:
        case HDMI_EDID_EX_VIDEO_720x576p_100Hz_16_9:
        case HDMI_EDID_EX_VIDEO_1280x720p_120Hz_16_9:
        case HDMI_EDID_EX_VIDEO_720x480p_120Hz_16_9:
        case HDMI_EDID_EX_VIDEO_720x576p_200Hz_16_9:
        case HDMI_EDID_EX_VIDEO_720x480p_240Hz_16_9:
            hdmi.avi_param.db2m_no_43_169 = INFOFRAME_AVI_DB2M_169;
            break;
            /* 4:3 */
        case HDMI_EDID_EX_VIDEO_640x480p_60Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x480p_60Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x480i_60Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x576p_50Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x576i_50Hz_4_3:
        case HDMI_EDID_EX_VIDEO_1440x576p_50Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x240p_60Hz_4_3:
        case HDMI_EDID_EX_VIDEO_1440x480p_60Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x288p_50Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x576p_100Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x480p_120Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x576p_200Hz_4_3:
        case HDMI_EDID_EX_VIDEO_720x480p_240Hz_4_3:
            hdmi.avi_param.db2m_no_43_169 = INFOFRAME_AVI_DB2M_43;
            break;
        default:
            hdmi.avi_param.db2m_no_43_169 = INFOFRAME_AVI_DB2M_NO;
            break;
    }

    hdmi.avi_param.db2r_same_43_169_149 = INFOFRAME_AVI_DB2R_SAME;
    hdmi.avi_param.db3itc_no_yes = INFOFRAME_AVI_DB3ITC_NO;
    hdmi.avi_param.db3ec_xvyuv601_xvyuv709 = INFOFRAME_AVI_DB3EC_XVYUV601;
    hdmi.avi_param.db3sc_no_hori_vert_horivert = INFOFRAME_AVI_DB3SC_NO;
    hdmi.avi_param.db4vic_videocode = cfg->video_format;
    logi("cfg->video_format:%d\n",cfg->video_format);
    logi("hdmi.avi_param.db4vic_videocode:%d\n",hdmi.avi_param.db4vic_videocode);
    hdmi.avi_param.db5pr_no_2_3_4_5_6_7_8_9_10 = INFOFRAME_AVI_DB5PR_NO;
    hdmi.avi_param.db6_7_lineendoftop = 0;
    hdmi.avi_param.db8_9_linestartofbottom = 0;
    hdmi.avi_param.db10_11_pixelendofleft = 0;
    hdmi.avi_param.db12_13_pixelstartofright = 0;

    hdmi.tpi_v_cfg = tpi_v_cfg;

    //USE_PP_MODE
    if (hdmi.pp_mode_needed) {
        k3_mhl_tx_attempt_packed_pixel();
    }

   /* set video config*/
    hw_set_video_mode(&(hdmi.tpi_v_cfg));



    if (HDMI_HDMI == cfg->hdmi_dvi) {
        hdmi.audiochanged = true;
        hw_core_audio_mode_enable(hdmi.audio_core_cfg.layout);

#ifndef FPGA_AUDIO_TEST
        hdmi_audio_set_param(hdmi.sample_freq, hdmi.sample_size, hdmi.bsio, hdmi.layout, hdmi.audiotype, hdmi.channel_number);
#else
        AUDIO_FPGA_TPYE audio_type;
        audio_type = FPGA_AUDIO_TEST_TYPE;
        switch (audio_type) {
            case TX3_48K_I2S:
                //hdmi_audio_set_param(48000, HDMI_SAMPLE_16BITS, true, 0, 1, 2);
                //test for channel mapping
                //hdmi_audio_set_param(48000, HDMI_SAMPLE_16BITS, true, 1, 1, 4);
                //hdmi_audio_set_param(48000, HDMI_SAMPLE_16BITS, true, 1, 1, 6);
                //hdmi_audio_set_param(48000, HDMI_SAMPLE_16BITS, true, 1, 1, 8);

                //hdmi_audio_set_param(48000, HDMI_SAMPLE_24BITS, true, 0, 1, 2);//24bit
                //hdmi_audio_set_param(48000, HDMI_SAMPLE_24BITS, true, 1, 1, 8);//24bit
                hdmi_audio_set_param(48000, HDMI_SAMPLE_16BITS, true, 0, 1, 2);

                break;
            case TX3_48K_SPDIF:
                hdmi_audio_set_param(48000, HDMI_SAMPLE_16BITS, false, 0, 1, 2);
                //hdmi_audio_set_param(48000, HDMI_SAMPLE_24BITS, false, 0, 1, 2);//24bit
                break;

            case TX3_96K_I2S:
                hdmi_audio_set_param(96000, HDMI_SAMPLE_16BITS, true, 0, 1, 2);
                break;

            case TX3_96K_SPDIF:
                hdmi_audio_set_param(96000, HDMI_SAMPLE_16BITS, false, 0, 1, 2);
                break;

            case TX3_48K_DIRECT_SPDIF:
                hdmi_audio_set_param(48000, HDMI_SAMPLE_16BITS, false, 0, 2, 2);
                break;
        }
#endif
        /* config n and cts by pixel_clock and fs */
        hw_configure_acr(hdmi.timings->pixel_clock, hdmi.audio_core_cfg.fs);
        /* set audio config to hdmi device */
        hw_configure_audio(HDMI_CORE_AV, &hdmi.audio_core_cfg);

#ifdef FPGA_AUDIO_TEST
        configASP(FPGA_AUDIO_TEST_TYPE);//v2pro test
#endif
        hdmi.audiochanged = false;
    }

#if HDMI_FOR_CERTIFICATION
    if (cfg->has_vcdb) {
        int ret = hw_configure_lrfr(HDMI_LIMITED_RANGE);
        if (ret) {
            loge("hw_configure_lrfr fail, ret: %d.\n", ret);
        }
    }
#endif

#if USE_HDCP
    if (hdmi.in_reset) {
        hdcp_off();
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK,
            TMDS_OUTPUT_CONTROL_POWER_DOWN | AV_MUTE_MUTED);
        msleep(128);
    }
#endif
    hw_enable_tmds();

#if !USE_HDCP
    hw_core_blank_video(false);
#endif

    if (!hdmi.support_mhl_interface) {
        hw_core_swreset_release();
    }

    logd("TMDS CCTRL:%d\n",tpi_read_reg_internal(TPI_INDEXED_PAGE_0, 0x80*4));
    OUT_FUNCTION;
    return;
}

void hw_init_irqs(void)
{
    if (hw_support_mhl()) {
        //disable all interrupts
        tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4_MASK&0xFF)*4, ALL_INTERRUPT_DISABLED);
        tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM5_MASK&0xFF)*4, ALL_INTERRUPT_DISABLED);
        k3_mhl_reg_write(REG_CBUS_SET_INT_0_MASK,ALL_INTERRUPT_DISABLED);
        k3_mhl_reg_write(REG_CBUS_SET_INT_1_MASK, ALL_INTERRUPT_DISABLED);
        k3_mhl_reg_write(REG_CBUS_INT_0_MASK, ALL_INTERRUPT_DISABLED);
        k3_mhl_reg_write(REG_CBUS_INT_1_MASK, ALL_INTERRUPT_DISABLED);
        k3_mhl_reg_write(REG_CBUS_CEC_ABORT_INT_MASK, ALL_INTERRUPT_DISABLED);
        k3_mhl_reg_write(REG_CBUS_DDC_ABORT_INT_MASK, ALL_INTERRUPT_DISABLED);
        k3_mhl_reg_write(REG_CBUS_MSC_MT_ABORT_INT_MASK, ALL_INTERRUPT_DISABLED);
        k3_mhl_reg_write(REG_CBUS_MSC_MR_ABORT_INT_MASK, ALL_INTERRUPT_DISABLED);

        //clear all interrupts
        tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4 & 0x00FF)*4, 0x77);
        tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM5 & 0x00FF)*4, 0x1F);
        k3_mhl_reg_write(REG_CBUS_SET_INT_0, 0x0F);
        k3_mhl_reg_write(REG_CBUS_SET_INT_1, 0x02);
        k3_mhl_reg_write(REG_CBUS_INT_0, 0xFF);
        k3_mhl_reg_write(REG_CBUS_INT_1, 0xFE);
        k3_mhl_reg_write(REG_CBUS_CEC_ABORT_INT, 0xFF);
        k3_mhl_reg_write(REG_CBUS_DDC_ABORT_INT, 0x87);
        k3_mhl_reg_write(REG_CBUS_MSC_MT_ABORT_INT, 0x8F);
        k3_mhl_reg_write(REG_CBUS_MSC_MR_ABORT_INT, 0xBF);

        tpi_write_reg(TPI_INTERRUPT_ENABLE_REG, ALL_INTERRUPT_DISABLED);
        tpi_write_reg(TPI_INTERRUPT_STATUS_REG, 0xf3);
    } else {
        tpi_write_reg(TPI_INTERRUPT_ENABLE_REG, ALL_INTERRUPT_DISABLED);
        tpi_write_reg(TPI_INTERRUPT_STATUS_REG, 0xf3);

        logi("TPI_INTERRUPT_ENABLE_REG(0x3c):0x%x\n", tpi_read_reg(TPI_INTERRUPT_ENABLE_REG));
        logi("TPI_INTERRUPT_STATUS_REG(0x3d):0x%x\n", tpi_read_reg(TPI_INTERRUPT_STATUS_REG));
    }

    return;
}

/******************************************************************************
* Function:       hw_enable_irqs
* Description:    enable or disable hdmi irq
* vendor_data Accessed:
* vendor_data Updated:
* Input:          true to enable irq, false to disable irq
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_enable_irqs(bool enable)
{
    static bool irq_isdisabled = true;

    logd("irq_isdisabled:%d, enable:%d. \n", irq_isdisabled, enable);
    hw_init_irqs();

    if (hw_support_mhl()) {
        if (enable) {
            //enable interrupts registers
            tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4_MASK&0xFF)*4, 0xFF);
            k3_mhl_reg_write(REG_CBUS_SET_INT_0_MASK,0x0F);
            k3_mhl_reg_write(REG_CBUS_SET_INT_1_MASK, 0x02);
            k3_mhl_reg_write(REG_CBUS_INT_0_MASK, 0xFF);
            k3_mhl_reg_write(REG_CBUS_INT_1_MASK, 0xFE);
            k3_mhl_reg_write(REG_CBUS_DDC_ABORT_INT_MASK, 0x87);
            k3_mhl_reg_write(REG_CBUS_MSC_MT_ABORT_INT_MASK, 0x8F);
            k3_mhl_reg_write(REG_CBUS_MSC_MR_ABORT_INT_MASK, 0xBF);

            //tpi_modify_reg(TPI_INTERRUPT_ENABLE_REG, RECEIVER_SENSE_EVENT_EN_MASK | HOT_PLUG_EVENT_EN_MASK, RECEIVER_SENSE_EVENT_ENABLE |HOT_PLUG_EVENT_ENABLE);
#if USE_HDCP
            tpi_modify_reg(TPI_INTERRUPT_ENABLE_REG,
                HDCP_AUTH_STATUS_CHANGE_EN_MASK | HDCP_SECURITY_CHANGE_EN_MASK,
                HDCP_AUTH_STATUS_CHANGE_ENABLE | HDCP_SECURITY_CHANGE_ENABLE);
#endif
        } else {
            //disable all interrupts
            tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4_MASK&0xFF)*4, ALL_INTERRUPT_DISABLED);
            k3_mhl_reg_write(REG_CBUS_SET_INT_0_MASK,ALL_INTERRUPT_DISABLED);
            k3_mhl_reg_write(REG_CBUS_SET_INT_1_MASK, ALL_INTERRUPT_DISABLED);
            k3_mhl_reg_write(REG_CBUS_INT_0_MASK, ALL_INTERRUPT_DISABLED);
            k3_mhl_reg_write(REG_CBUS_INT_1_MASK, ALL_INTERRUPT_DISABLED);
            k3_mhl_reg_write(REG_CBUS_DDC_ABORT_INT_MASK, ALL_INTERRUPT_DISABLED);
            k3_mhl_reg_write(REG_CBUS_MSC_MT_ABORT_INT_MASK, ALL_INTERRUPT_DISABLED);
            k3_mhl_reg_write(REG_CBUS_MSC_MR_ABORT_INT_MASK, ALL_INTERRUPT_DISABLED);

            tpi_write_reg(TPI_INTERRUPT_ENABLE_REG, ALL_INTERRUPT_DISABLED);
        }
    } else {
        if (enable) {
            tpi_modify_reg(TPI_INTERRUPT_ENABLE_REG,
                /*RECEIVER_SENSE_EVENT_EN_MASK |*/HOT_PLUG_EVENT_EN_MASK,
                /*RECEIVER_SENSE_EVENT_ENABLE |*/HOT_PLUG_EVENT_ENABLE);
#if USE_HDCP
            tpi_modify_reg(TPI_INTERRUPT_ENABLE_REG,
                HDCP_AUTH_STATUS_CHANGE_EN_MASK | HDCP_SECURITY_CHANGE_EN_MASK,
                HDCP_AUTH_STATUS_CHANGE_ENABLE | HDCP_SECURITY_CHANGE_ENABLE);
#endif
            logi("TPI_INTERRUPT_ENABLE_REG(0x3c):0x%x\n", tpi_read_reg(TPI_INTERRUPT_ENABLE_REG));
            logi("TPI_INTERRUPT_STATUS_REG(0x3d):0x%x\n", tpi_read_reg(TPI_INTERRUPT_STATUS_REG));
        } else {
            tpi_write_reg(TPI_INTERRUPT_ENABLE_REG, ALL_INTERRUPT_DISABLED);
        }
    }

    return;
}

inline void dump_intr_status( int irq_id, u32 intr_val)
{
    #define DUMP_INTR_STATUS(intr_val, intr_bit, bit) \
    do {\
        if (intr_val & intr_bit) {\
            logd("              %d %s\n", bit,#intr_bit);\
        }\
    }while(0);

    if ((1 == irq_id) && intr_val) {
        logd(" ================= irq1 ==================== \n");
        DUMP_INTR_STATUS(intr_val, HOT_PLUG_EVENT_MASK, 0);
        //DUMP_INTR_STATUS(intr_val, RECEIVER_SENSE_EVENT_MASK, 1);
        DUMP_INTR_STATUS(intr_val, HDCP_SECURITY_CHANGE_EVENT_MASK, 5);
        DUMP_INTR_STATUS(intr_val, HDCP_AUTH_STATUS_CHANGE_EVENT_MASK, 7);
        logd(" ================= irq1 ==================== \n\n");
    }

    return;
}

/******************************************************************************
* Function:       hw_get_hdmi_state
* Description:    get irq state by register state
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:         irq state
* Others:
*******************************************************************************/
int hw_get_hdmi_state(void)
{
    u32 set         = 0;
    u32 intr        = 0;
    int hdmi_state  = 0;
    static volatile int first_hpd = 0;
    static volatile int laststatus = 0;

    intr = tpi_read_reg(TPI_INTERRUPT_STATUS_REG);
    set = tpi_read_reg_internal(TPI_INDEXED_PAGE_0, HDMI_CORE_SYS_SYS_STAT);

    logd("sys_stat: 0x%x, hpd_intr: 0x%x.\n", set, intr);
    logd("TPI_INTERRUPT_ENABLE_REG: 0x%x.\n", tpi_read_reg(TPI_INTERRUPT_ENABLE_REG));
    logd("TPI_INTERRUPT_STATUS_REG: 0x%x.\n", intr);
    dump_intr_status(1, intr);

    if (!hw_support_mhl()) {
        tpi_modify_reg(TPI_INTERRUPT_STATUS_REG, 0xFB, intr);
    } else {
#if !USE_HDCP
        /* write to flush */
        tpi_modify_reg(TPI_INTERRUPT_STATUS_REG, 0xFB, intr);
#endif
    }

    if (!hw_support_mhl()) {
        if (intr & HOT_PLUG_EVENT_MASK) {
            if (intr & HOT_PLUG_PIN_STATE_HIGH) {
                logi("connect, \n");
                hdmi_state = HDMI_CONNECT;
                if (0 == first_hpd) {
                    hdmi_state |= HDMI_FIRST_HPD;
                    first_hpd++;
                    logi("first hpd\n");
                }
            } else if (!(intr & HOT_PLUG_PIN_STATE_HIGH) && (hdmi.connected == true)) {
                logi("Disconnect\n");
                first_hpd = 0;
                hdmi_state = HDMI_DISCONNECT;
            }
            laststatus = set;
        }
    } else {
        if (g_mhl_tx_config.disconnection_to_hdmi) {
            g_mhl_tx_config.disconnection_to_hdmi = false;
            first_hpd = 0;
            hdmi_state = HDMI_DISCONNECT;
        }
        if (g_mhl_tx_config.connection_to_hdmi) {
            logi("connect, \n");
            g_mhl_tx_config.connection_to_hdmi = false;
            hdmi_state = HDMI_CONNECT;
            if (hdmi.video_power_state != HDMI_POWER_FULL) {
                first_hpd = 0;
            }
            if (0 == first_hpd) {
                hdmi_state |= HDMI_FIRST_HPD;
                first_hpd++;
                logi("first hpd\n");
            }
        }
    }


#if USE_HDCP
    if (!hw_support_mhl()) {
        if (intr & HDCP_AUTH_STATUS_CHANGE_EVENT_MASK) {
            hdmi_state |= HDMI_HDCP_AUTH_STATUS_CHANGE;
        }
        if (intr & HDCP_SECURITY_CHANGE_EVENT_MASK) {
            hdmi_state |= HDMI_HDCP_SECURITY_CHANGE;
        }
    }
#endif

    return hdmi_state;
}

/******************************************************************************
* Function:       hw_re_detect
* Description:    get cable connect state by register state
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:         state of cable connect
* Others:
*******************************************************************************/
int hw_rx_detect(void)
{
    int hpd_state   = 0;
    int loop        = 0;

    if (hw_support_mhl()) {
        if (g_mhl_tx_config.tmds_output_enabled) {
            return HDMI_CONNECT;
        } else {
            return HDMI_DISCONNECT;
        }
    }

    do {
        /* read connect state from register */
        hpd_state = tpi_read_reg(TPI_INTERRUPT_STATUS_REG);

        if ((hpd_state & (RX_SENSE_MASK | HOT_PLUG_PIN_STATE_MASK)) == (RX_SENSE_ATTACHED | HOT_PLUG_PIN_STATE_HIGH)) {
            return HDMI_CONNECT;
        }
        loop ++;
    } while (loop < 100);

    return HDMI_DISCONNECT;
}

/******************************************************************************
* Function:       hw_phy_power_on
* Description:    turn on hdmi phy, may depend on tmds later
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_phy_power_on(void)
{
    IN_FUNCTION;
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_TDMS_CTL2, 1, 5, 5);
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_TDMS_CTL3, 1, 0, 0);
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_BIST_INSTRL, 1, 4, 4);

    /*
    0   8'b1101_0100   8'hD4 or 8'hD1 will do.
    1   8'b0011_1xx1   Red colored bit should be 1'b1, so that external resistor value is used for the output swing value.
    3   8'b1100_0000   DVI encoder of phy used,Red colored bit should be 1'b1, so that external resistor value is used for the output swing value.
    */
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_TDMS_CTL1, 1, 7, 7);
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_TDMS_CTL2, 3, 4, 3);
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_BIST_CNTL, 3, 7, 6);
    //mdelay(5);
    REG_FLD_MOD(hw_res.base_phy, PHY_OSC_CTRL_ADDR, 1, 0, 0);
    if (hw_support_mhl()) {
        //normal pixel mode, ppmode need to config
        REG_FLD_MOD(hw_res.base_phy, PHY_PLL_CTRL_ADDR, 0x4A, 7, 0);
#if USE_HDCP
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x13, 7, 0);
#else
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x33, 7, 0);
#endif
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL3_ADDR, 0x01, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL4_ADDR, 0x35, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL5_ADDR, 0x80, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_HDMI_DRV_CTRL_ADDR, 0x02, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_MHL_DRV_CTRL_ADDR, 0x1C, 7, 0);
#if PHY_MHL_30BIT
        REG_FLD_MOD(hw_res.base_phy, PHY_MHL_CTRL2_ADDR, 0x08, 7, 0);
#else
        //need to modify DSS_GLB HDMI_TMDS_DATA_SEL[0]=0x0
        REG_FLD_MOD(hw_res.base_phy, PHY_MHL_CTRL2_ADDR, 0x08, 7, 0);
#endif
    } else {
        REG_FLD_MOD(hw_res.base_phy, PHY_PLL_CTRL_ADDR, 0x54, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x32, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL2_ADDR, 0x20, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL3_ADDR, 0x01, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL5_ADDR, 0x00, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_BIAS_GEN_CTRL2_ADDR, 0x5d, 7, 0);
        REG_FLD_MOD(hw_res.base_phy, PHY_HDMI_DRV_CTRL_ADDR, 0x03, 7, 0);
    }

    OUT_FUNCTION;
    return;
}

/******************************************************************************
* Function:       hw_phy_power_off
* Description:    turn off hdmi phy
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_phy_power_off(void)
{
    IN_FUNCTION;

    /*0xFA205804[5]= 0 output disable*/
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_TDMS_CTL2, 0, 5, 5);
    /*0xFA205808[0]= 0 phy power down*/
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_TDMS_CTL3, 0, 0, 0);
    /*0xFA205818[4]= 0 close osc_cec_clk*/
    //REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_BIST_INSTRL, 0, 4, 4);

    REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0, 4, 4);
    REG_FLD_MOD(hw_res.base_phy, PHY_OSC_CTRL_ADDR, 0, 0, 0);
    OUT_FUNCTION;

    return;
}

/******************************************************************************
* Function:       hw_phy_configure_aclk_dig
* Description:    config phy aclk dig
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_phy_configure_aclk_dig(int factor)
{
    int aclk_factor = ACLK_MULT_FACTOR_1;
    switch(factor){
        case 1:
            aclk_factor = ACLK_MULT_FACTOR_1;
            break;
        case 2:
            aclk_factor = ACLK_MULT_FACTOR_2;
            break;
        case 3:
            aclk_factor = ACLK_MULT_FACTOR_3;
            break;
        case 4:
            aclk_factor = ACLK_MULT_FACTOR_4;
            break;
        case 5:
            aclk_factor = ACLK_MULT_FACTOR_5;
            break;
        case 6:
            aclk_factor = ACLK_MULT_FACTOR_6;
            break;
        case 10:
            aclk_factor = ACLK_MULT_FACTOR_10;
            break;
        default:
            loge("factory %d is incorrect!!\n",factor);
            break;
    }

#if HDMI_PHY_PROCESS
    //FPGA must use I2C to operate phy
    REG_FLD_MOD(hw_res.base_phy, HDMI_PHY_BIST_CNTL, aclk_factor >> 2, BIT_ACLK_COUNT2, BIT_ACLK_COUNT2);
#endif
    logd("set factor:%d factor_reg:%d\n",factor,aclk_factor);

    return;
}

bool mhl_is_connected(void)
{
    u32 state = 0;
    if(hdmi.state != HDMI_DISPLAY_ACTIVE){
        return false;
    }
    state = k3_mhl_reg_read(REG_CBUS_STATUS);
    return (state & BIT2) ? true:false;
}

/******************************************************************************
* Function:       hw_core_power_on
* Description:    turn on hdmi device
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_core_power_on(void)
{
    unsigned long wait_time = 0;
    static bool is_first_hpd = true;
    IN_FUNCTION;


#if HDMI_CHIP_VER

    if (!hdmi.in_reset) {

#ifdef CONFIG_CPU_FREQ_GOV_K3HOTPLUG
        pm_qos_add_request(&hdmi.qos_request, PM_QOS_DDR_MIN_PROFILE, HDMI_PM_QOS_DDR_MIN_FREQ);
        logd("set ddr freq:%d\n", HDMI_PM_QOS_DDR_MIN_FREQ);
#endif
        logi("is first_hpd:%d, is dss inited:%d.\n", is_first_hpd, hdmi.dss_inited);
        while (is_first_hpd && (!hdmi.dss_inited)) {
            msleep(10);
            wait_time++;
        }
        if (wait_time > 0) {
            loge("wait for dss inited is %ld ms.\n", (wait_time * 10));
        }
        is_first_hpd = false;

        wake_lock(&hdmi.wake_lock);

        k3_fb1_blank(FB_BLANK_UNBLANK);
        if(!hw_support_mhl()) {
            if (regulator_enable(hw_res.boost_vcc) != 0) {
                loge("failed to enable boost_vcc regulator.\n");
            }
        }

#if USE_HDCP
        hdcp_write();
        mdelay(20);
#endif
        hw_reset_dss_hdmi_on();
        /*enable hdmi clock*/
        hw_enable_clocks();

#if HDCP_WRITE_TEST
        run_all_bist_tests();
        mdelay(2);
#endif


#if 0
        if(!hw_support_mhl()) {
            hw_set_iomux(PINCTRL_STATE_DEFAULT);
        }
#endif
        tpi_write_reg(TPI_ENABLE, 0x00);
        mdelay(20);
        logd("TPI MODE OR NOT: %s.\n", k3_mhl_hw_tpimode() ? "YES" : "NO");
        mdelay(5);
    }

     /*init irq*/

    REG_FLD_MOD(HDMI_CORE_SYS, HDMI_CORE_CTRL1, 0x1, 0, 0);

    hw_core_mute_audio(true);
#else
    logd("in hw_core_power_on ........\n");

    if (!hw_support_mhl()) {

        gpio_request(40, "hdmi_par_sel");
        gpio_request(41, "ldo_power_on");
        gpio_request(42, "ddc_power_on");
        gpio_request(43, "hdmi_phy_reset");
        //gpio_request(44, "hdmi_mhl_sel");

        /* LDO Power on */
        gpio_direction_output(41, 1);
        gpio_set_value(41, 1);
        msleep(100);

        /* HDMI PHY reset */
        gpio_direction_output(43, 1);
        gpio_set_value(43, 1);
        msleep(10);

        gpio_direction_output(43, 1);
        gpio_set_value(43, 0);
        msleep(10);

        gpio_direction_output(43, 1);
        gpio_set_value(43, 1);
        msleep(10);

        /* DDC Power on */
        gpio_direction_output(42, 1);
        gpio_set_value(42, 1);
        msleep(10);

        /* hdmi par sel */
        gpio_direction_output(40, 1);
        gpio_set_value(40, 0);
        msleep(10);

        /* hdmi mhl sel */
        /*gpio_direction_output(44, 1);//mhl---0,hdmi---1
        gpio_set_value(44, 0);
        msleep(10);*/

        logd("HDMI GPIO DONE.\n");
    } else {
        //mhl gpio and phy register add here
    }
#endif

        logd("Before HDMI OPEN.\n");
        REG_FLD_MOD(HDMI_CORE_SYS, HDMI_CORE_CTRL1, 0x1, 0, 0);
        REG_FLD_MOD(HDMI_CORE_AV, HDMI_CORE_AV_DPD, 0x7, 2, 0);
        mdelay(10);
     if (!hw_support_mhl()) {
        //TPI
        logi("TPI_DEVICE_ID: 0x%x.\n", tpi_read_reg(TPI_DEVICE_ID));
        tpi_write_reg(TPI_ENABLE,0x00);
        msleep(100);
        logi("TPI_DEVICE_ID(0x1B): 0x%x.\n", tpi_read_reg(TPI_DEVICE_ID));
        logi("TPI_RESERVED2(0x1D): 0x%x.\n", tpi_read_reg(TPI_RESERVED2));
        if(0xB4 != tpi_read_reg(TPI_DEVICE_ID))  {
            loge("TPI mode start failed!\n");
            return;
        }
        //D1
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, OUTPUT_MODE_MASK,OUTPUT_MODE_HDMI);
        tpi_modify_reg(TPI_DEVICE_POWER_STATE_CTRL_REG, TX_POWER_STATE_MASK, TX_POWER_STATE_D0);
        //tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_ACTIVE);
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_MUTED);
        logi("TPI_DEVICE_POWER_STATE_CTRL_REG(0x1E):0x%x\n", tpi_read_reg(TPI_DEVICE_POWER_STATE_CTRL_REG));
    }

    hdmi.state = HDMI_DISPLAY_ACTIVE;

    OUT_FUNCTION;

    return ;
}

/******************************************************************************
* Function:       hw_core_power_off
* Description:    turn off hdmi device
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void  hw_core_power_off(void)
{
    //int ret = 0;

    IN_FUNCTION;

    // TMDS Power down
    hw_disable_tmds();

#if HDMI_CHIP_VER
    if(!hdmi.in_reset) {
    /* Turn off DDC */
    TPI_REG_FLD_MOD(TPI_INDEXED_PAGE_1, HDMI_CORE_AV_DPD, 0x0, 2, 1);
    /* Turn off core */
    //TPI_REG_FLD_MOD(TPI_INDEXED_PAGE_0, HDMI_CORE_CTRL1, 0x0, 0, 0);
    // Go to D3 mode
    tpi_modify_reg(TPI_DEVICE_POWER_STATE_CTRL_REG, TX_POWER_STATE_MASK, TX_POWER_STATE_D3);

        /*set io mux low power*/
#if 0
        /* en_chg_pump_int off */
        if(!hw_support_mhl()) {
        hw_set_iomux(PINCTRL_STATE_IDLE);
        }
#endif
        /*disable hdmi clocks*/
        hw_disable_clocks();
        mdelay(2);
        if(!hw_support_mhl()) {
            regulator_disable(hw_res.boost_vcc);
        }
        /*disable edc1 regulator*/
		k3_fb1_blank(FB_BLANK_POWERDOWN);  //FB_BLANK_POWERDOWN
        mdelay(2);

        wake_unlock(&hdmi.wake_lock);

#ifdef CONFIG_CPU_FREQ_GOV_K3HOTPLUG
        pm_qos_remove_request(&hdmi.qos_request);
        logd("remove qos request\n");
#endif

    } else {
        msleep(100);
    }

#else
    if (!hw_support_mhl()) {
        gpio_direction_output(40, 1);
        gpio_set_value(40, 0);
        msleep(10);

        /* LDO Power on */
        gpio_direction_output(41, 1);
        gpio_set_value(41, 0);
        msleep(10);

        /* DDC Power on */
        gpio_direction_output(42, 1);
        gpio_set_value(42, 0);
        msleep(10);
    }
#endif
    return;
}

/******************************************************************************
* Function:       hw_set_clk_rate
* Description:    set clocks rate
* vendor_data Accessed:
* vendor_data Updated:
* Input:          clock index, rate
* Output:
* Return:
* Others:
*******************************************************************************/
int hw_set_clk_rate(u32 clks, u32 rate)
{
    int ret = -1;
    if (clks & HDMI_CLK_LDI1) {
        ret = clk_set_rate(hw_res.clk_ldi1, rate);
        if(ret){
            loge("enable ldi1 clock error %d\n",ret);
        }
    }
    if (clks & HDMI_CLK_EDC1) {
        ret = clk_set_rate(hw_res.clk_edc0, rate*12/10);
        if(ret){
            loge("enable ldi1 clock error %d\n",ret);
        }
    }
    return ret;
}


/******************************************************************************
* Function:       enable_clk
* Description:    enable some clocks
* vendor_data Accessed:
* vendor_data Updated:
* Input:          clock index
* Output:
* Return:
* Others:
*******************************************************************************/
static void enable_clk(u32 clks)
{
    int ret = 0;

    IN_FUNCTION;

    logd("enable clk 0x%x. \n", clks);

    if (clks & HDMI_CLK_EDC1) {
        ret = clk_prepare_enable(hw_res.clk_edc0);
        if(ret){
            loge("enable edc0 clock error %d\n",ret);
        }
    }

    if (clks & HDMI_CLK_LDI1) {
        ret = clk_set_parent(hw_res.clk_muxldi1,hw_res.clk_muxldi1->parents[2]);
        if (ret) {
         loge(" clk_set_parent LDI1 error!\n");
        }
        ret = clk_set_rate(hw_res.clk_ldi1, hdmi.timings->pixel_clock*1000);
        if (ret < 0) {
            loge(" clk_set_rate LDI1 error!\n");
        }
        ret = clk_prepare_enable(hw_res.clk_ldi1);
        if(ret){
            loge("enable ldi1 clock error %d\n",ret);
        }
        logd("clk_ldi1=%ld\n", clk_get_rate(hw_res.clk_ldi1));
    }

    if (clks & HDMI_CLK) {
        ret = clk_prepare_enable(hw_res.clk_pclk_hdmi);
        if (ret) {
            loge("enable pclk hdmi clock error %d\n",ret);
        }
    }

#if defined(CONFIG_HDMI_CEC_ENABLE)
    if (clks & HDMI_CLK_CEC) {
        ret = clk_prepare_enable(hw_res.clk_cec);
        if(ret){
            loge("enable cec clock error %d\n",ret);
        }
    }
#endif
    OUT_FUNCTION;

    return;
}

/******************************************************************************
* Function:       disable_clk
* Description:    disable some clocks
* vendor_data Accessed:
* vendor_data Updated:
* Input:          clock index
* Output:
* Return:
* Others:
*******************************************************************************/
static void disable_clk(u32 clks)
{
    IN_FUNCTION;

    logi("disable clk 0x%x. \n", clks);

    if (clks & HDMI_CLK) {
        clk_disable_unprepare(hw_res.clk_pclk_hdmi);
    }

    if (clks & HDMI_CLK_LDI1) {
        clk_disable_unprepare(hw_res.clk_ldi1);
    }

    if (clks & HDMI_CLK_EDC1) {
        clk_disable_unprepare(hw_res.clk_edc0);
    }

#if defined(CONFIG_HDMI_CEC_ENABLE)
    if (clks & HDMI_CLK_CEC) {
        clk_disable_unprepare(hw_res.clk_cec);
    }
#endif

    OUT_FUNCTION;
    return;
}

/******************************************************************************
* Function:       hw_enable_clocks
* Description:    enadle all clocks
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_enable_clocks(void)
{
    enable_clk(HDMI_CLK | HDMI_CLK_EDC1 | HDMI_CLK_LDI1 | HDMI_CLK_CEC);

    return;
}

/******************************************************************************
* Function:       hw_disable_clocks
* Description:    disable all clocks
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_disable_clocks(void)
{
    disable_clk(HDMI_CLK | HDMI_CLK_EDC1 | HDMI_CLK_LDI1);
    return;
}

#if HDMI_CHIP_VER
/******************************************************************************
* Function:
* Description:
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_set_iomux(char* mode)
{
    IN_FUNCTION;

    hw_res.h_pin->pin = devm_pinctrl_get_select(hw_res.h_pin->dev, mode);
    if (IS_ERR(hw_res.h_pin->pin)) {
        loge("failed to set iomux to %s mode.\n", mode);
    }

    OUT_FUNCTION;
}
#endif

/******************************************************************************
* Function:       hw_get_irq
* Description:    get object of irq
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
int hw_get_irq()
{
    return hw_res.irq;
}

/******************************************************************************
* Function:       hw_free_resources
* Description:    free resources, irq, clk.
* vendor_data Accessed:
* vendor_data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hw_free_resources(void)
{

    IN_FUNCTION;
#if HDMI_CHIP_VER
    if (hw_res.irq && hdmi.irq_is_init) {
        free_irq(hw_res.irq, (void *)0);
    }
    if (hw_res.clk_hdmiefc) {
        clk_put(hw_res.clk_hdmiefc);
        hw_res.clk_hdmiefc = NULL;
    }

    if (hw_res.clk_edc0) {
        clk_put(hw_res.clk_edc0);
        hw_res.clk_edc0 = NULL;
    }

    if (hw_res.clk_ldi1) {
        clk_put(hw_res.clk_ldi1);
        hw_res.clk_ldi1 = NULL;
    }

#if defined(CONFIG_HDMI_CEC_ENABLE)
    if (hw_res.clk_cec) {
        clk_put(hw_res.clk_cec);
        hw_res.clk_cec = NULL;
    }
#endif
    if (hw_res.clk_pclk_hdmi) {
        clk_put(hw_res.clk_pclk_hdmi);
        hw_res.clk_pclk_hdmi = NULL;
    }
    if (hw_res.clk_ppll2) {
        clk_put(hw_res.clk_ppll2);
        hw_res.clk_ppll2= NULL;
    }
    if (hw_res.clk_muxldi1) {
        clk_put(hw_res.clk_muxldi1);
        hw_res.clk_muxldi1= NULL;
    }
    if (hw_res.sdp_vcc) {
        regulator_put(hw_res.sdp_vcc);
        hw_res.sdp_vcc= NULL;
    }
#if 0
    if (hw_res.h_pin->pin) {
        devm_pinctrl_put(hw_res.h_pin->pin);
        hw_res.h_pin->pin= NULL;
    }
#endif
    if (hw_res.boost_vcc) {
        regulator_put(hw_res.boost_vcc);
        hw_res.boost_vcc= NULL;
    }
#endif
    OUT_FUNCTION;

    return;
}

/******************************************************************************
* Function:       hw_get_resources
* Description:    get resources, irq, clk, and config base address
* vendor_data Accessed:
* vendor_data Updated:
* Input:          platform device
* Output:
* Return:
* Others:
*******************************************************************************/
int hw_get_resources(struct platform_device *pdev)
{
    int ret = 0;
    struct device_node *np = NULL;
    struct device_node *node_fb = NULL;

    BUG_ON(NULL == pdev);

    IN_FUNCTION;

    memset(&hw_res, 0, sizeof(hdmi_hw_res));
    np = of_find_compatible_node(NULL, NULL, DTS_COMP_HDMI_NAME);
    if (!np) {
        loge("NOT FOUND device node %s!\n", DTS_COMP_HDMI_NAME);
        goto err;
    }
    node_fb = of_find_compatible_node(NULL, NULL, "hisilicon,k3fb");
    if (IS_ERR(node_fb)) {
        loge("of_find_compatible_node fb failed!\n");
        goto err;
    }

    of_property_read_u32(np, "id", &pdev->id);

    hw_res.irq = irq_of_parse_and_map(np, 0);

    if (hw_res.irq < 0) {
        loge("failed to get irq_hdmi resource.\n");
        goto err;
    }
    hw_res.base_core = (u32)of_iomap(np, 0);
    hw_res.base_core_av = hw_res.base_core + HDMI_AV_REG_OFFSET;
    hw_res.base_phy = hw_res.base_core + HDMI_PHY_REG_OFFSET;
    hw_res.base_cec = hw_res.base_core + HDMI_CEC_REG_OFFSET;

    hw_res.base_crgctrl =  (u32)of_iomap(np, 1);
    hw_res.base_pmctrl =  (u32)of_iomap(np, 2);

    hw_res.base_dss = (u32)of_iomap(node_fb, 0);
    hw_res.base_ldi1 = hw_res.base_dss + 0x2f000;

    logi("base core: 0x%x\n", (int)hw_res.base_core);

#if HDMI_CHIP_VER//zzm test
    hw_res.clk_pclk_hdmi = of_clk_get(np, 0);
    if (IS_ERR(hw_res.clk_pclk_hdmi)) {
        hw_res.clk_pclk_hdmi = NULL;
        loge("hdmi, %s: failed to get pclk hdmi clock\n", __func__);
        goto err;
    }

    hw_res.clk_hdmiefc = of_clk_get(np, 1);
    if (IS_ERR(hw_res.clk_hdmiefc)) {
        hw_res.clk_hdmiefc = NULL;
        loge("hdmi, %s: failed to get clk_hdmiefc clock\n", __func__);
        goto err;
    }
#if defined(CONFIG_HDMI_CEC_ENABLE)
    hw_res.clk_cec = of_clk_get(np, 2);
    if (IS_ERR(hw_res.clk_cec)) {
        hw_res.clk_cec = NULL;
        loge("hdmi, %s: failed to get clk_cec clock\n", __func__);
        goto err;
    }
#endif
    hw_res.clk_edc0 = of_clk_get(np, 3);
    if (IS_ERR(hw_res.clk_edc0)) {
        hw_res.clk_edc0 = NULL;
        loge("hdmi, %s: failed to get clk_edc1 clock\n", __func__);
        goto err;
    }
    hw_res.clk_ldi1 = of_clk_get(np, 4);
    if (IS_ERR(hw_res.clk_ldi1)) {
        hw_res.clk_ldi1 = NULL;
        loge("hdmi, %s: failed to get clk_ldi1 clock\n", __func__);
        goto err;
    }
    hw_res.clk_ppll2 = of_clk_get(np, 5);
    if (IS_ERR(hw_res.clk_ppll2)) {
        hw_res.clk_ppll2 = NULL;
        loge("hdmi, %s: failed to get clk_ppll2 clock\n", __func__);
        goto err;
    }
    hw_res.clk_muxldi1 = of_clk_get(np, 6);
    if (IS_ERR(hw_res.clk_muxldi1)) {
        hw_res.clk_muxldi1= NULL;
        loge("hdmi, %s: failed to get clk_muxldi1 clock\n", __func__);
        goto err;
    }

    hw_res.sdp_vcc = devm_regulator_get(&pdev->dev, "hdmivcc");
    if (IS_ERR(hw_res.sdp_vcc)) {
        hw_res.sdp_vcc= NULL;
        loge("hdmi, %s: failed to get sdp_vcc regulator\n", __func__);
        goto err;
    }

 if(!hw_support_mhl()) {
    hw_res.boost_vcc = devm_regulator_get(&pdev->dev, "hdmiboost");
    if (IS_ERR(hw_res.boost_vcc)) {
        hw_res.boost_vcc= NULL;
        loge("hdmi, %s: failed to get boost_vcc regulator\n", __func__);
        goto err;
    }
    }
#if 0
    hw_res.h_pin->dev = &pdev->dev;
    hw_res.h_pin->pin = devm_pinctrl_get(&pdev->dev);
    if (IS_ERR(hw_res.h_pin->pin)) {
        hw_res.h_pin->pin = NULL;
        loge("hdmi, %s: failed to get pinctrl\n", __func__);
        goto err;
    }
#endif
#endif

    OUT_FUNCTION;
    return 0;

err:
    hw_free_resources();
    return ret;
}

bool hw_support_mhl()
{
    return hdmi.support_mhl_interface;
}

void hw_reset_dss_hdmi_on(void)
{
    IN_FUNCTION;

    //hdmi phy reset
    REG_FLD_MOD(hw_res.base_crgctrl, 0x84, 0x1, 15, 15);
    REG_FLD_MOD(hw_res.base_crgctrl,  0x88, 0x1, 15, 15);

    mdelay(5);
    REG_FLD_MOD(hw_res.base_dss, 0x424, 0x1, 24, 24);
    REG_FLD_MOD(hw_res.base_dss, 0x424, 0x1, 22, 22);
    REG_FLD_MOD(hw_res.base_phy, 0xc, 0x1, 0, 0);

    logi("DSS:0x40c  val = 0x%x\n",read_reg(hw_res.base_dss, 0x40c));
    logi("DSS:0x428  val = 0x%x\n",read_reg(hw_res.base_dss, 0x428));
    logi("DSS:0x42c  val = 0x%x\n",read_reg(hw_res.base_dss, 0x42c));

    OUT_FUNCTION;
}

/******************************************************************************
* Function:      hw_reconfig_hdcp
* Description:   reconfig hdcp when user enable/disable hdcp
* Input:         hdcp_enable, true for enable, false for disable
* Output:        na
* Return:        na
*******************************************************************************/
void hw_reconfig_hdcp(bool hdcp_enable)
{
    logi("Currrent HDCP switch: %d\n", hdcp_get_enable());

    if (HDMI_POWER_FULL != hdmi.power_state) {
        logd("Not FULL state, don't reconfig HDCP.\n");
        return;
    }

    /* In connected state, enable HDCP */
    if (hdcp_enable == true) {
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_MUTED);

        /* config MHL PHY register to disable TMDS output, bit[5]=0 */
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x0, 5, 5);
        /* infoframe packet can send to sink after 128ms */
        msleep(128);

        /* HDCP handle function */
        hdcp_handler(0);

        /* config MHL PHY register to enable TMDS output, bit[5]=1 */
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x1, 5, 5);
    }
    /* In connected state, disable HDCP */
    else {
        hdcp_off();
        /* waiting for 64ms to disable HDCP */
        msleep(64);

        /* config MHL PHY register to disable TMDS output, bit[5]=0 */
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x0, 5, 5);
        /* infoframe packet can send to sink after 128ms */
        msleep(128);

        /* config MHL PHY register to enable TMDS output, bit[5]=1 */
        REG_FLD_MOD(hw_res.base_phy, PHY_DM_TX_CTRL1_ADDR, 0x1, 5, 5);

        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_NORMAL);
    }

    return;
}

#if USE_HDCP
#include "teek_client_api.h"
#include "teek_client_id.h"

bool is_teecd_ready(void)
{
    if (1 == TEEK_IsAgentAlive(0x46536673)) {
        return true;
    }

    return false;
}

void hdcp_write(void)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Result result;
    TEEC_Operation operation;
    TEEC_UUID svc_id = TEE_SERVICE_HDCP;

    #define MAX_WAITING_TIME 50
    int i = 0;

    memset(&session, 0x00, sizeof(session));

    do {
        if (is_teecd_ready()) {
            logd("teecd is ok, go to copy hdcp key.\n");
            break;
        } else {
            logd("teecd is not ready, wait for 1 second.\n");
            msleep(1000);
            i++;
        }
    } while (i < MAX_WAITING_TIME);

    if (i >= MAX_WAITING_TIME) {
        loge("teecd is not ready, can not copy hdcp key!\n");
        return;
    }

    logi("begin to copy hdcp key from teeos.\n");
    result = TEEK_InitializeContext(
               NULL,
               &context
               );

    if(result != TEEC_SUCCESS) {
        goto cleanup_1;
    }
    result = TEEK_OpenSession(
                &context,
                &session,
                &svc_id,
                TEEC_LOGIN_PUBLIC,
                NULL,
                NULL,
                NULL);

    if(result != TEEC_SUCCESS) {
        goto cleanup_2;
    }
    logi("session id 0x%x\n", session.session_id);

    memset(&operation, 0x00, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(
        TEEC_VALUE_INPUT,
        TEEC_NONE,
        TEEC_NONE,
        TEEC_NONE);
    operation.params[0].value.a = 0xFFFF;
    operation.params[0].value.b = 0xFFFC;
    result = TEEK_InvokeCommand(
                 &session,
                 ECHO_CMD_ID_SEND_CMD,
                 &operation,
                 NULL);

    if (result == TEEC_SUCCESS) {
        logi("[0] : %x %x\n", operation.params[0].value.a, operation.params[0].value.b);
    } else {
        loge("invoke failed\n");
        goto cleanup_3;
    }

cleanup_3:
    TEEK_CloseSession(&session);
cleanup_2:
    TEEK_FinalizeContext(&context);
cleanup_1:

    return;
}
#endif

#if HDCP_WRITE_TEST

#define HDMI_SRST        ((volatile void *)(HDMI_CORE_SYS + 0x14))
#define HDMI_HDCP_CTRL   ((volatile void *)(HDMI_CORE_SYS + 0x3C))
#define HDMI_VRES_H      ((volatile void *)(HDMI_CORE_SYS + 0xF4))
#define HDMI_EPST        ((volatile void *)(HDMI_CORE_SYS + 0x3E4))
#define HDMI_EPCM        ((volatile void *)(HDMI_CORE_SYS + 0x3E8))

#define HDMI_HDCP_AKSV1  ((volatile void *)(HDMI_CORE_SYS + 0x74))
#define HDMI_HDCP_AKSV2  ((volatile void *)(HDMI_CORE_SYS + 0x78))
#define HDMI_HDCP_AKSV3  ((volatile void *)(HDMI_CORE_SYS + 0x7c))
#define HDMI_HDCP_AKSV4  ((volatile void *)(HDMI_CORE_SYS + 0x80))
#define HDMI_HDCP_AKSV5  ((volatile void *)(HDMI_CORE_SYS + 0x84))

static int polling(void)
{
    u32 result = 0;
    u32 bist_result = 0;
    u32 retry_count = 200;

    IN_FUNCTION;

    while (--retry_count) {
        bist_result = readl(HDMI_EPST);
        logd("polling: bist_result: %x, retry_count: %d\n",
            bist_result, retry_count);

        /* check command done  */
        if (1 == (bist_result & 0x1)) {
            logd("polling: polling finished, bist_result: %x, retry_count: %d\n",
                bist_result, retry_count);
            break;
        }

        udelay(1000);
    }

    if (retry_count == 0) {
        result = -1;
        logd("polling error: polling retry 200 times, bist_result: %x.\n", bist_result);
        goto Error;
    }

    /* delay 2 ms test1*/
    udelay(2000);

    bist_result = readl(HDMI_EPST);
    logi("checking: bist_result: %x\n", bist_result);
    /* check error  */
    if (0 != (bist_result & 0x02)) {
        logd("checking error:, bist_result: %x\n", bist_result);
        result = -1;
    }
    /* delay 2 ms test1*/
    udelay(2000);
Error:

    OUT_FUNCTION;
    return result;
}

int run_all_bist_tests(void)
{
    u32 result = 0;

    IN_FUNCTION;

    /* soft reset */
    writel(0x01, HDMI_SRST);

    udelay(2000);
    /* HDCP control */
#if 1
    logd("HDMI_HDCP_CTRL:0x%x\n", readl(HDMI_HDCP_CTRL));

    writel(0x0C, HDMI_HDCP_CTRL);

    udelay(2000);
    logd("HDMI_HDCP_CTRL:0x%x\n", readl(HDMI_HDCP_CTRL));

    //msleep(20000);

#else

#endif


    /* soft reset */
    writel(0x00, HDMI_SRST);
    udelay(2000);

    logd("HDMI_HDCP_CTRL:0x%x\n", readl(HDMI_HDCP_CTRL));

    /* delay 2 ms test1*/
    udelay(2000);

    /* clear status */
    writel(0x00, HDMI_EPST);

    /* delay 2 ms test1*/
    udelay(2000);

    /* reload KSV */
    writel(0x20, HDMI_EPCM);

    /* delay 2 ms test1*/
    udelay(2000);

    /* polling */
    result = polling();
    if (-1 == result)
        goto Exit;

    /* clear status */
    writel(0x00, HDMI_EPST);

    /* delay 2 ms test1*/
    udelay(2000);

    /* enable CRC */
    writel(0x4, HDMI_EPCM);
    logd("HDMI EPCM(0x3E8):0x%x\n", readl(HDMI_EPCM));
    /* delay 2 ms test1*/
    udelay(2000);


    logd("HDCP----AKSV1:0x74 val=0x%x\n", readl(HDMI_HDCP_AKSV1));
    logd("HDCP----AKSV2:0x78 val=0x%x\n", readl(HDMI_HDCP_AKSV2));
    logd("HDCP----AKSV3:0x7C val=0x%x\n", readl(HDMI_HDCP_AKSV3));
    logd("HDCP----AKSV4:0x80 val=0x%x\n", readl(HDMI_HDCP_AKSV4));
    logd("HDCP----AKSV5:0x84 val=0x%x\n", readl(HDMI_HDCP_AKSV5));

    /* polling */
    result = polling();
    if (-1 == result)
        goto Exit;
Exit:

    logd("HDMI EPCM(0x3E8):0x%x\n", readl(HDMI_EPCM));
    logd("HDMI EPST(0x3E4):0x%x\n", readl(HDMI_EPST));
    logd("run_all_bist_tests: result: %d\n", result);

    OUT_FUNCTION;
    return result;
}

#endif
