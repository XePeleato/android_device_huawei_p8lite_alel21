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

#ifndef __ADE_OVERLAY_H__
#define __ADE_OVERLAY_H__

//#include <linux/hisi_ion.h>
#include <linux/smp.h>

#include "balong_ade.h"
#include "balong_fb_def.h"

#include <linux/hisi/reg_ops.h>
#include "soc_ao_sctrl_interface.h"
#include "soc_baseaddr_interface.h"
#include "soc_modem_sctrl_interface.h"

/*************************macro define******************************/

#define  ADE_UNIT_TRANS_MAX     (7)
#define  ADE_ROT_WIDTH_MAX  (1200)
#define  ADE_SCL_WIDTH_MAX  (1280)
#define  ADE_HEIGHT_MAX (1920)
#define ADE_INVAL_PATTERN_NUM   (0xffff)

#define  ADE_INVAL_REG      (0xffff)

extern u8* balongfd_reg_base_ade;

#if ADE_DEBUG_LOG_ENABLE
extern u32 g_command_file_wr_enable;
extern u32 g_ade_power_off_enable;
extern u32 g_ade_isr_disable;
extern u32 g_ade_offline_disable;
extern u32 g_debug_dis_ade_ioctl;
extern u32 g_debug_diable_ade;
extern u32 g_debug_frame_number;
#else
#warning "ADE_DEBUG_LOG_ENABLE is 0"
#endif

#define WFD_SUPPORT (1)


#define set_soft_rst_sel_bit(soft_rst, bit, lcd_type)   do { if ((bit != ADE_INVAL_REG) && (lcd_type != PANEL_MIPI_CMD))  \
                                                    set_value(&(soft_rst), 1, 1, bit); \
                                              } while(0)


#define set_reload_dis_bit(reload_dis, bit)   do { if (bit != ADE_INVAL_REG)  \
                                                    set_value(&(reload_dis), 1, 1, bit); \
                                              } while(0)

/*************************enum define*******************************/
enum {
	ADE_FALSE = 0,
	ADE_TRUE  = 1
};

enum {
    ADE_OVLY_TYPE_BYPASS = 0,
    ADE_OVLY_TYPE_FIRST_OVLY,
    ADE_OVLY_TYPE_SEC_OVLY
};

/* idle -> wait_cfg <==> cfg_succ -> idle */
enum {
    ADE_SCL2_STATE_IDLE = 0,
    ADE_SCL2_STATE_WAIT_CFG,
    ADE_SCL2_STATE_CFG_SUCC
};

enum {
	ADE_ROT_ANGLE_NOP = 0,
	ADE_ROT_ANGLE_180,
	ADE_ROT_ANGLE_270,
	ADE_ROT_ANGLE_90,
    ADE_ROT_ANGLE_H_MIRROR,
    ADE_ROT_ANGLE_V_MIRROR,
    ADE_ROT_ANGLE_90_V_MIRROR,
    ADE_ROT_ANGLE_90_H_MIRROR
};


/*****************reg enum*******************/

enum {
    TOP_OVLY1_TRANS_WDMA1_DIS = 0x0,
    TOP_OVLY1_TRANS_WDMA1_EN = 0x1
};

/* ovly2 trans */
enum {
    TOP_OVLY2_TRANS_WDMA3_EN = 0x1,
    TOP_OVLY2_TRANS_ROT_EN   = 0x2,
    TOP_OVLY2_TRANS_SCL2_EN  = 0x4,
    TOP_OVLY2_TRANS_DISP_EN  = 0x8,
    TOP_OVLY2_TRANS_WDMA2_EN = 0x10
};

enum {
    TOP_OVLY3_TRANS_SEC_OVLY_EN = 0x1,
    TOP_OVLY3_TRANS_ROT_EN      = 0x2,
    TOP_OVLY3_TRANS_SCL2_EN     = 0x4,
    TOP_OVLY3_TRANS_WDMA2_EN    = 0x8
};

/* disp trans */
enum {
    TOP_CTRAN5_TRANS_OVLY_DIS = 0x0,
    TOP_CTRAN5_TRANS_OVLY_EN = 0x1
};

/* rot trans */
enum {
    TOP_ROT_TRANS_SEC_OVLY_EN = 0x1,
    TOP_ROT_TRANS_DISP_EN     = 0x2,
    TOP_ROT_TRANS_SCL2_EN     = 0x4,
    TOP_ROT_TRANS_WDMA2_EN    = 0x8,
    TOP_ROT_TRANS_WDMA3_EN    = 0x10
};

/* scl2 trans */
enum {
    TOP_SCL2_TRANS_SEC_OVLY_EN   = 0x1,
    TOP_SCL2_TRANS_DISP_EN       = 0x2,
    TOP_SCL2_TRANS_WDMA3_EN      = 0x4,
    TOP_SCL2_TRANS_WDMA2_EN      = 0x8
};

/* rot buff cfg val */
enum {
    TOP_ROT_BUFF_ROT_1080P,
    TOP_ROT_BUFF_CH6_1080P,
    TOP_ROT_BUFF_CH5_1080P,
    TOP_ROT_BUFF_CH5_CH6_720P,
    TOP_ROT_BUFF_CH6_ROT_720P
};

/* scl3 top cfg */
enum {
    TOP_SCL3_ON_CH5 = 0,
    TOP_SCL3_ON_WIFI
};

/* scl1 top cfg */
enum {
    TOP_SCL1_ON_CH6 = 0,
    TOP_SCL1_ON_CH1,
    TOP_SCL1_ON_CH5
};

/* ROT src cfg */
enum {
    TOP_ROT_SRC_RESERV = 0,
    TOP_ROT_SRC_OVLY2,
    TOP_ROT_SRC_OVLY3,
    TOP_ROT_SRC_RDMA
};

/* SCL2 src cfg */
enum {
    TOP_SCL2_SRC_RESERV = 0,
    TOP_SCL2_SRC_OVLY2,
    TOP_SCL2_SRC_OVLY3,
    TOP_SCL2_SRC_RDMA,
    TOP_SCL2_SRC_ROT
};

/* ldi src cfg */
enum {
    TOP_DISP_CH_SRC_RESERV = 0,
    TOP_DISP_CH_SRC_OVLY2,
    TOP_DISP_CH_SRC_RDMA,
    TOP_DISP_CH_SRC_ROT,
    TOP_DISP_CH_SRC_SCL2
};


/* wifi src cfg */
enum {
    TOP_WDMA3_SRC_RESERV = 0,
    TOP_WDMA3_SRC_RDMA,
    TOP_WDMA3_SRC_ROT,
    TOP_WDMA3_SRC_SCL2,
    TOP_WDMA3_SRC_OVLY1,
    TOP_WDMA3_SRC_OVLY2
};

/* wdma2 src cfg */
enum {
    TOP_WDMA2_SRC_RESERV = 0,
    TOP_WDMA2_SRC_RDMA,
    TOP_WDMA2_SRC_ROT,
    TOP_WDMA2_SRC_SCL2,
    TOP_WDMA2_SRC_OVLY2,
    TOP_WDMA2_SRC_OVLY3
};

/* sec ovly src cfg */
enum {
    TOP_SEC_OVLY_SRC_RESERV = 0,
    TOP_SEC_OVLY_SRC_OVLY3,
    TOP_SEC_OVLY_SRC_ROT,
    TOP_SEC_OVLY_SRC_SCL2
};

/****************demux and mux *******************/
enum {
    TOP_DEMUX_ROT_BUFF = 0,
    TOP_DEMUX_SCL1,
    TOP_DEMUX_SCL3,
    TOP_DEMUX_CTRAN5,
    TOP_DEMUX_OVLY1,
    TOP_DEMUX_ROT_BUF_SHR_OUT,      /* ADE_MEM_SHARE_EN */
    TOP_DEMUX_MAX
};

enum {
    TOP_MUX_ROT_SRC = 0,
    TOP_MUX_SCL2_SRC,
    TOP_MUX_DISP_SRC,
    TOP_MUX_WDMA2_SRC,
    TOP_MUX_SEC_OVLY_SRC,
    TOP_MUX_WDMA3_SRC,
    TOP_MUX_MAX
};

enum {
    ADE_WAIT_EVT_CH1_CMPL = 0,
    ADE_WAIT_EVT_CH2_CMPL,
    ADE_WAIT_EVT_CH3_CMPL,
    ADE_WAIT_EVT_CH4_CMPL,
    ADE_WAIT_EVT_CH5_CMPL,
    ADE_WAIT_EVT_CH6_CMPL,
    ADE_WAIT_EVT_DISP_CMPL,
    ADE_WAIT_EVT_CMDQ1_CMPL,
    ADE_WAIT_EVT_CMDQ2_CMPL,
    ADE_WAIT_EVT_RESERVED01,
    ADE_WAIT_EVT_WDMA1_CMPL,
    ADE_WAIT_EVT_WDMA2_CMPL,
    ADE_WAIT_EVT_WDMA3_CMPL,
    ADE_WAIT_EVT_RESERVED02,
    ADE_WAIT_EVT_WCMDQ_CMPL,
    ADE_WAIT_EVT_CH1_AXI_CMPL = 0xf,
    ADE_WAIT_EVT_CH2_AXI_CMPL = 0x10,
    ADE_WAIT_EVT_CH3_AXI_CMPL,
    ADE_WAIT_EVT_CH4_AXI_CMPL,
    ADE_WAIT_EVT_CH5_AXI_CMPL,
    ADE_WAIT_EVT_CH6_AXI_CMPL,
    ADE_WAIT_EVT_DISP_AXI_CMPL,
    ADE_WAIT_EVT_CMDQ1_AXI_CMPL,
    ADE_WAIT_EVT_CMDQ2_AXI_CMPL,
    ADE_WAIT_EVT_RESERVED03,
    ADE_WAIT_EVT_DMA_ERR,
    ADE_WAIT_EVT_SCL1_CMPL = 0x1a,
    ADE_WAIT_EVT_SCL2_CMPL,
    ADE_WAIT_EVT_SCL3_CMPL,
    ADE_WAIT_EVT_SCL1_ERR,
    ADE_WAIT_EVT_SCL2_ERR,
    ADE_WAIT_EVT_SCL3_ERR = 0x1f,
    ADE_WAIT_EVT_ROT_CMPL = 0x20,
    ADE_WAIT_EVT_LDI_CMPL,
    ADE_WAIT_EVT_GAMMAR_CMPL,
    ADE_WAIT_EVT_CLIP1_CMPL,
    ADE_WAIT_EVT_CLIP2_CMPL,
    ADE_WAIT_EVT_CLIP3_CMPL,
    ADE_WAIT_EVT_CLIP4_CMPL,
    ADE_WAIT_EVT_CLIP5_CMPL,
    ADE_WAIT_EVT_CLIP6_CMPL = 0x28,
    ADE_WAIT_EVT_OVLY1_CMPL = 0x35,
    ADE_WAIT_EVT_OVLY2_CMPL,
    ADE_WAIT_EVT_OVLY3_CMPL
};

/*************************** struct define ********************************/
struct ch_capability {
    u32 bit_rot_enable : 1;
    u32 bit_scl1_enable : 1;
    u32 bit_scl3_enable : 1;
    u32 bit_csc_enable : 1;
    u32 bit_block_rot_enable : 1;
    u32 bit_ovly1      : 1;
    u32 bit_ovly2      : 1;
    u32 bit_ovly3      : 1;
    u32 bit_rot_1080p  : 1;
    u32 bit_rot_720p   : 1;
    u32 reserved       : 22;
};

typedef union {
    struct {
        u32     bit_h_mit  : 1; // hcoef_mitchell
        u32     bit_h_lzs2 : 1; // hcoef_lanzcos2
        u32     bit_h_lzs2_gaus10 : 1; //hcoef_lanzcos2_gauss10
        u32     bit_h_lzs2_gaus40 : 1; //hcoef_lanzcos2_gauss40
        u32     bit_h_lzs3 : 1;     //vcoef_lanzcos3
        u32     bit_v_mit  : 1; // vcoef_mitchell
        u32     bit_v_lzs2 : 1; // vcoef_lanzcos2
        u32     bit_v_lzs2_gaus10 : 1; //vcoef_lanzcos2_gauss10
        u32     bit_v_lzs2_gaus40 : 1; //vcoef_lanzcos2_gauss40
        u32     bit_v_lzs3 : 1;     //vcoef_lanzcos3
        u32     reserved : 22;
    }bits;

    u32 ul32;
} SCL_CONFIG_COEF;

struct commit_comp_unit {
    u32 bit_wdma1   : 1;
    u32 bit_wdma2   : 1;
    u32 bit_offline_wdma3   : 1;
    u32 bit_online_wdma3   : 1;
    u32 bit_rot     : 1;
    u32 bit_scl2    : 1;
    u32 bit_scl3    : 1;
    u32 bit_sec_ovly1   : 1;
    u32 bit_sec_ovly2   : 1;
    u32 bit_ldi     : 1;
    u32 bit_part_update : 1;
    u32 reserved    : 21;
};

struct ade_overlay_rot {
    u8  rot_en;
    u8  rot_buff;
    u8  rotation;
    u8  is_configed;
    u32 rot_width;
    u32 rot_height;
};
struct ade_overlay_scl2 {
    u8  scl2_en;
    u8  scl2_iformat;
    u8  scl2_state;
    u8  reserved[5];
    struct ade_rect src_rect;
    struct ade_rect dst_rect;
};


struct ovly_info {
    struct ade_rect  input_rect;
    struct ade_rect  output_rect;
    u8  sec_ovly_surf_num;
    u8  sec_ovly_ch_num;
    u8  reserved;
    u8  ch_count;
};

struct rot_block_info {
    u32 blk_enable;

    u32 blk_width_b;
    u32 blk_height_b;
    u32 blk_width_l;
    u32 blk_height_l;

    u32 blk_w_width_b;
    u32 blk_w_height_b;
    u32 blk_w_width_l;
    u32 blk_w_height_l;

    //u32 blk_w_delta_width_b;  /* added for wdma align with 16 byte */
    //u32 blk_w_delta_height_b; /* added for wdma align with 16 byte */

    u32 rdma_uv_addr;
    u32 rdma_addr;

    u32 wdma_addr;
    u32 wdma_uv_addr;
};

/* for block rot */
struct rot_layer_info {
    struct rot_block_info  blk_info[4];

    u32 layer_width;
    u32 layer_height;
    u32 layer_stride;
    u32 layer_rot;
    u32 layer_format;
    u32 dst_format;  /* dst_format must be same with layer_format when using wdma3 */
    u32 dst_stride;
    u32 wdma_num;    /* only wdma1 or wdma3 is valid */
};

struct ch_unit_reg {
    /* RDMA 15 *2 */
    u16 ch_rdma_ch_pe;
    u16 ch_rdma_ch_ctrl;
    u16 ch_rdma_ch_addr;
    u16 ch_rdma_ch_size;
    u16 ch_rdma_ch_stride;
    u16 ch_rdma_ch_space;
    u16 ch_rdma_ch_partial_size;
    u16 ch_rdma_ch_partial_space;
    u16 ch_rdma_ch_uv_addr;
    u16 ch_rdma_ch_uv_size;
    u16 ch_rdma_ch_uv_stride;
    u16 ch_rdma_ch_uv_space;
    u16 ch_rdma_ch_uv_partial_size;
    u16 ch_rdma_ch_uv_partial_space;
    u16 ch_rdma_ch_extra_partial_width;
    u16 ch_rdma_ch_en;
    u16 rdma_rst_sel_bit;

    /* clip */
    u16 clip_disable;
    u16 clip_size0;
    u16 clip_size1;
    u16 clip_size2;
    u16 clip_cfg_ok;
    u16 clip_rst_sel_bit;

    /* scl 10 *2 */
    u16 scl_ctrl;
    u16 scl_hsp;
    u16 scl_uv_hsp;
    u16 scl_vsp;
    u16 scl_uv_vsp;
    u16 scl_ores;
    u16 scl_ires;
    u16 scl_start;
    u16 scl_pix_ofst;
    u16 scl_uv_pix_ofst;
    u16 scl_hcoef;
    u16 scl_vcoef;
    u16 scl_rst_sel_bit;

    /* ctrans 7 * 2 */
    u16 ctran_dis;
    u16 ctran_mode_choose;
    u16 ctran_chdc0;
    u16 ctran_csc0;
    u16 ctran_image_size;
    u16 ctran_cfg_ok;
    u16 ctran_rst_sel_bit;
    u16 reserved[3];
};

struct ovly_reg {
    u16 ovly_ch_xy0[OVERLAY_ADE_SURFACE_MAX];
    u16 ovly_ch_xy1[OVERLAY_ADE_SURFACE_MAX];
    u16 ovly_ch_ctrl[OVERLAY_ADE_SURFACE_MAX];
    u16 ovly_ch_sel_start[OVERLAY_ADE_SURFACE_MAX];
    u16 ovly_ch1_xy2;
    u16 ovly_ch1_xy3;
    u16 ovly_output_size;
    u16 ovly_ctrl;
    u16 ovly_alpha_state;
    u16 ovly_rst_sel_bit;
};


struct ovly_ctrl_info {
    u32 ch_num;
    u32 ovly_num;
    u32 blending;
    u32 dim_blending;
    u32 format;
    u32 ovly_type;   /* bypass, first ovly or second ovly */
    u32 sec_ovly_ch_count;
};

struct ovly_hnd_info {
    u32 phy_addr;
    int share_fd;
    u32 lock_succ;
    struct ion_handle *buff_hnd;
};

/**************************************************/

struct ade_overlay_pipe {
    u8*     ade_base;
    u32     pipe_num;
    u8      need_cfg_rot_dma;
    u8      need_cfg_scl2;
    u8      shadow_reg_count;
    u8      reserved;

    struct  ch_capability       pipe_cap;
    u32     reserved1;
    struct  overlay_region_info pipe_info;
    struct  ch_unit_reg         pipe_reg;
    struct  ade_rect            output_rect; /* the config sequence must be in turn, such as RDMA->CLIP->SCL->CTRAN->OVLY */
};

struct ade_overlay_ovly {
    u32     ovly_num;
    u32     ovly_cap;
    u8*     ade_base;
    u32     ovly_ch_shadow_count;
    u32     reserved;
    struct  ovly_info   ovly_comp;
    struct  ovly_reg    ovly_reg;
};

struct ade_mux_info {
    u32     is_used;
    u32     mux_src;
};

/***********************************************/
struct ade_overlay_ctrl {
    struct ade_overlay_pipe         pipe_list[OVERLAY_PIPE_ADE_MAX];
    struct ade_overlay_ovly         ovly_list[OVERLAY_OVLY_MAX];
    struct ade_overlay_rot          rot_info;
    struct ade_overlay_scl2         scl2_info;

    struct overlay_resource_info    ade_res_info;
    struct overlay_compose_info     comp_info;

    struct ade_mux_info      ade_mux_list[TOP_MUX_MAX];
    u32    ade_demux_list[TOP_DEMUX_MAX];

    u32    ovly2_ch_used;   // 2b'11 means surface0 and surface1
    u32    ovly1_ch_used;   // 2b'11 means surface0 and surface1
    u32    reserved;
};


/*******************************function**********************************/
extern SCL_CONFIG_COEF scl_coef[3];

#if ADE_DEBUG_LOG_ENABLE
extern void set_top_eof_cmd2file(void);
extern void  set_top_cfg2file(u32 reg_addr, u32 val);
#else
#define set_top_eof_cmd2file()
#endif

#if ADE_MEM_SHARE_EN
extern void set_TOP_CTL1_rot_buf_shr_out (u8* ade_base, u32 val);
extern u32 get_TOP_CTL1_rot_buf_shr_out (u8* ade_base);
extern u32 get_TOP_CTL_rot_buf_cfg (u8* ade_base);
#endif

extern void set_TOP_CTL(u8* ade_base, u32 val);
extern void set_TOP_ADE_OVLY_CTRL(u8* ade_base, u32 val);
extern void set_TOP_CTL_clk_gate_en(u8* ade_base, u32 val);
extern void set_TOP_OVLY_CTRL_all(u8* ade_base, u32 val);
extern void set_TOP_OVLY_CTRL(u8* ade_base, u16 start_bit, u32 val);
extern void set_TOP_CTL_frm_end_start (u8* ade_base, u32 val);
extern void set_TOP_CTL_ch5_nv (u8* ade_base, u32 val);
extern void set_TOP_CTL_ch6_nv (u8* ade_base, u32 val);
extern void set_TOP_CTL_rot_buf_cfg (u8* ade_base, u32 val);
extern void set_TOP_CTL_dfs_buf_cfg(u8* ade_base, u32 val);
extern void set_TOP_SCL1_MUX_CFG(u8* ade_base, u32 val);
extern void set_TOP_SCL3_MUX_CFG_scl3_mux(u8* ade_base, u32 val);
extern void set_TOP_SCL3_MUX_CFG_wdma3_trans(u8* ade_base, u32 val);
extern void set_TOP_CTRAN5_TRANS_CFG(u8* ade_base, u32 val);
extern void set_TOP_DMA_AXI_MUX(u8* ade_base, u32 ch_num, u32 ch_type);
extern void set_TOP_FRM_DISCARD_wdma2(u8* ade_base, u32 val);
extern void set_TOP_FRM_DISCARD_wdma3(u8* ade_base, u32 val);
extern void set_TOP_FRM_DISCARD_fifo(u8* ade_base, u32 val);
extern void set_TOP_OVLY1_TRANS_CFG(u8* ade_base, u32 val);
extern void set_TOP_ROT_SRC_CFG(u8* ade_base, u32 val);
extern void set_TOP_SCL2_SRC_CFG(u8* ade_base, u32 val);
extern void set_TOP_DISP_SRC_CFG(u8* ade_base, u32 val);
extern void set_TOP_WDMA2_SRC_CFG(u8* ade_base, u32 val);
extern void set_TOP_SEC_OVLY_SRC_CFG(u8* ade_base, u32 val);
extern void set_TOP_WDMA3_SRC_CFG(u8* ade_base, u32 val);
extern void set_TOP_ADE_EN(u8* ade_base, u32 val);
extern int getSclImageFormatType(int format);
extern void set_CMDQ_RDMA1_PE(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA1_CTRL(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA1_ADDR(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA1_LEN(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA1_EN(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA2_PE(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA2_CTRL(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA2_ADDR(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA2_LEN(u8* ade_base, u32 val);
extern void set_CMDQ_RDMA2_EN(u8* ade_base, u32 val);
extern void set_TOP_AUTO_CLK_GT_EN0_cmdq1_rdma(u8* ade_base, u32 val);
extern void set_TOP_AUTO_CLK_GT_EN0_cmdq2_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST0(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL1(u8* ade_base, u32 val);
extern void set_TOP_RELOAD_DIS0(u8* ade_base, u32 val);
extern void set_TOP_RELOAD_DIS1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_disp_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ch1_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ch2_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ch3_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ch4_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ch5_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ch6_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_cmdq1_rdma(u8* ade_base, u32 val);
extern void set_TOP_RELOAD_DIS0_cmdq1_rdma(u8* ade_base, u32 val);
extern void set_TOP_RELOAD_DIS1_gamma(u8* ade_base, u32 val);
extern void set_TOP_RELOAD_DIS1_dither(u8* ade_base, u32 val);
extern void set_TOP_RELOAD_DIS1_cmdq1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_cmdq2_rdma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_wdma1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_wdma2(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_wdma3(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_clip1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_clip2(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_clip3(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_clip4(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_clip5(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_clip6(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_scl1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_scl2(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_scl3(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ctran1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ctran2(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ctran3(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ctran4(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ctran5(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_ctran6(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL0_rot(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL1_cmdq1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL1_cmdq2(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL1_gamma(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL1_dither(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL1_ovly1(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL1_ovly2(u8* ade_base, u32 val);
extern void set_TOP_SOFT_RST_SEL1_ovly3(u8* ade_base, u32 val);
extern void set_TOP_INTR_MASK_CPU0(u8* ade_base, u32 val);
extern void set_TOP_INTR_MASK_CPU0_bit(u8* ade_base, u32 val);
extern void clear_TOP_INTR_MASK_CPU0_bit(u8* ade_base, u32 val);
extern void set_TOP_INTR_MASK_CPU1_bit(u8* ade_base, u32 val);
extern void clear_TOP_INTR_MASK_CPU1_bit(u8* ade_base, u32 val);
extern void set_TOP_INTR_MASK_CPU1(u8* ade_base, u32 val);
extern void set_TOP_INTR_CLEAR_CPU0(u8* ade_base, u32 val);
extern void set_TOP_INTR_CLEAR_CPU1(u8* ade_base, u32 val);
extern void clear_TOP_INTR_ORG_CPU0(u8* ade_base, u32 val);
extern void clear_TOP_INTR_ORG_CPU1(u8* ade_base, u32 val);


#endif  /* _ADE_OVERLAY_H_  */
