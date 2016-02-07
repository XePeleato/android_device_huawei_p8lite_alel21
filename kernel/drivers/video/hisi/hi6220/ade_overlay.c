/* Copyright (c) 2008-2010, Hisilicon Tech. Co., Ltd. All rights reserved.
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

#include <linux/fb.h>
#include <linux/clk.h>
#include <linux/hisi/hisi_ion.h>
#include <linux/delay.h>
#include <trace/trace_kernel.h>
#include <asm/cacheflush.h>
#ifndef CONFIG_ARM64
#include <asm/outercache.h>
#endif
#include <trace/trace_kernel.h>

#include "ade_overlay.h"
#include "balong_ade.h"
#include "ade_reg_offset.h"
#include "ldi_reg.h"
#include "mipi_reg.h"
#include "balong_compose.h"
#include "soc_baseaddr_interface.h"
#include "soc_ao_sctrl_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_media_sctrl_interface.h"
#include "soc_pmctrl_interface.h"
#include "soc_media_noc_interface.h"
#include <linux/pwrctrl_power_state_manager.h>

STATIC u32 g_ade_online_pe  = 0xf000004;
STATIC u32 g_ade_offline_pe = 0xf000002;

extern s32 scl_lanczos2[390];
extern s32 scl_lanczos3[390];
extern u32 scl_blk_rot[390];

#if ADE_DEBUG_LOG_ENABLE
extern int *debug_cmd_vaddr;
#endif
/****************************little function **************/
struct ade_overlay_pipe *ade_overlay_idx2pipe(struct  ade_overlay_ctrl *overlay_ctl, int ndx)
{
    struct ade_overlay_pipe  *pipe = NULL;

    BUG_ON(overlay_ctl == NULL);

    if ((ndx < 0) || (ndx > ADE_COMPOSE_CH_MAX)) {
        return NULL;
    }

    pipe = &(overlay_ctl->pipe_list[ndx]);

    return pipe;
}
struct ade_overlay_ovly *ade_overlay_cap2ovly(struct  ade_overlay_ctrl *overlay_ctl, struct ch_capability pipe_cap)
{
    struct ade_overlay_ovly     *ovly = NULL;

    BUG_ON(overlay_ctl == NULL);

    if (pipe_cap.bit_ovly1) {
        ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_1]);
    } else if (pipe_cap.bit_ovly2) {
        ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_2]);
    } else if (pipe_cap.bit_ovly3) {
        ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_3]);
    } else {
        balongfb_logi_display_debugfs("find ovly overlay_pipe_ade_disp for ch \n");
        return NULL;
    }

    return ovly;
}

struct cmdfile_buffer* ade_overlay_type2cmdfile(struct  cmdqueue_buff *cmdq_list, OVERLAY_PIPE_TYPE_UINT32 ch_type)
{
    struct cmdfile_buffer* cf_buff = NULL;
    u32    cmdbuff_idx = 0;

    if ((OVERLAY_PIPE_TYPE_ONLINE == ch_type) || (OVERLAY_PIPE_TYPE_DISPLAY == ch_type)) {
        cmdbuff_idx = cmdq_list[ADE_CMDQ_TYPE_ONLINE].current_idx;
        cf_buff = &(cmdq_list[ADE_CMDQ_TYPE_ONLINE].cf_list[cmdbuff_idx]);
    } else {
        cmdbuff_idx = cmdq_list[ADE_CMDQ_TYPE_OFFLINE].current_idx;
        cf_buff = &(cmdq_list[ADE_CMDQ_TYPE_OFFLINE].cf_list[cmdbuff_idx]);
    }

    balongfb_logi_display_debugfs("ade_overlay_type2cmdfile: ch_type= %d, cmdfile paddr=0x%x, vaddr=0x%p \n", ch_type, cf_buff->paddr, cf_buff->vaddr);

    if (!cf_buff->is_busy) {
        balongfb_loge("pinfo->ch_type:%d, cmd buff index %d error ,cmd buffer is not busy",ch_type, cmdbuff_idx);
        return NULL;
    }

    return cf_buff;
}

bool ade_overlay_need_yuv2rgb(int format)
{
    switch (format) {
        case ADE_YUYV_I:
        case ADE_YVYU_I:
        case ADE_UYVY_I:
        case ADE_VYUY_I:
        case ADE_YUV444:
        case ADE_NV12:
        case ADE_NV21:
            return true;
        default:
            return false;
    }

    return false;
}

bool ade_overlay_need_rgb2yuv(int format)
{
    switch (format) {
        case ADE_YUYV_I:
        case ADE_YVYU_I:
        case ADE_UYVY_I:
        case ADE_VYUY_I:
        case ADE_YUV444:
        case ADE_NV12:
        case ADE_NV21:
            return true;
        default:
            return false;
    }
}

u32 ade_overlay_format2bpp(int format)
{
    switch (format)
    {
        case ADE_RGB_565:
        case ADE_BGR_565:
            return 2;
        case ADE_XRGB_8888:
        case ADE_XBGR_8888:
        case ADE_ARGB_8888:
        case ADE_ABGR_8888:
        case ADE_RGBA_8888:
        case ADE_BGRA_8888:
            return 4;
        case ADE_RGB_888:
        case ADE_BGR_888:
        case ADE_YUV444:
            return 3;
        case ADE_YUYV_I:
        case ADE_YVYU_I:
        case ADE_UYVY_I:
        case ADE_VYUY_I:
            return 2;
            /* TODO: nv12,nv21 bpp is 1.5 */
        case ADE_NV12:
        case ADE_NV21:
            return 1;
        default:
            balongfb_loge("not support format \n");
            return 0;
    }
}
int ade_overlay_disp_src2wdma2_src(int disp_src)
{
    switch (disp_src) {
        case TOP_DISP_CH_SRC_OVLY2:
            return TOP_WDMA2_SRC_OVLY2;
        case TOP_DISP_CH_SRC_RDMA:
            return TOP_WDMA2_SRC_RDMA;
        case TOP_DISP_CH_SRC_ROT:
            return TOP_WDMA2_SRC_ROT;;
        case TOP_DISP_CH_SRC_SCL2:
            return TOP_WDMA2_SRC_SCL2;
        case TOP_DISP_CH_SRC_RESERV:
        default:
            balongfb_loge("invalid disp src = %d \n", disp_src);
            return TOP_DISP_CH_SRC_RESERV;
    }
}

void ade_overlay_handle_lock(struct ade_compose_data_type *ade_pri_data, struct  overlay_region_info *pipe_info)
{
    struct balong_fb_data_type *fb_data = NULL;
    struct ovly_hnd_info *hnd_info = NULL;
    struct ion_handle *hnd = NULL;
    int i = 0;
    struct iommu_map_format iommu_format;

    memset(&iommu_format, 0, sizeof(struct iommu_map_format));

    if ((ade_pri_data == NULL) || (pipe_info == NULL)) {
        return;
    }

    if (pipe_info->ch_num >= OVERLAY_PIPE_ADE_MAX) {
        balongfb_loge("ade_overlay_handle_lock: ch_num overflow \n");
        return;
    }

    fb_data  = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);

    for (i = 0; i < ADE_OVERLAY_MAX_LAYERS; i++) {
        hnd_info = &fb_data->locking_hnd[i];
        if (!hnd_info->lock_succ) {
            hnd_info->share_fd = pipe_info->share_fd;
            hnd_info->phy_addr = pipe_info->phy_addr;

            hnd = ion_import_dma_buf(fb_data->fb_ion_client, pipe_info->share_fd);
            if (IS_ERR(hnd)) {
                balongfb_loge("ade_overlay_handle_lock: pipe_info->share_fd= %d, pipe_info->phy_addr=0x%x \n", pipe_info->share_fd, pipe_info->phy_addr);
                hnd_info->lock_succ = ADE_FALSE;
                return;
            }
            iommu_format.is_tile = 0;
            if (0 != ion_map_iommu(fb_data->fb_ion_client,hnd,&iommu_format))
            {
                balongfb_loge("ion_map_iommu failed \n");
                hnd_info->lock_succ = ADE_FALSE;
                return;
            }
            hnd_info->phy_addr = iommu_format.iova_start;
            hnd_info->buff_hnd = hnd;
            hnd_info->lock_succ = ADE_TRUE;
            balongfb_loge_display_debugfs("ade_overlay_handle_lock:pipe_info->share_fd=%d,hnd_info->phy_addr=0x%x \n", pipe_info->share_fd, hnd_info->phy_addr);
            return;
        } else if (hnd_info->share_fd == pipe_info->share_fd) {
            balongfb_loge_display_debugfs("ade_overlay_handle_lock:pipe_info->share_fd=%d has lock succ \n", pipe_info->share_fd);
            return;
        }
    }

    balongfb_loge("find hnd buffer error \n");

}

void ade_overlay_handle_unlock(struct balong_fb_data_type *fb_data)
{
    struct ovly_hnd_info *hnd_info = NULL;
    int i = 0;

    BUG_ON(fb_data == NULL);

    for (i = 0; i < ADE_OVERLAY_MAX_LAYERS; i++) {
        hnd_info = &fb_data->locked_hnd[i];
        if (hnd_info->lock_succ) {
            balongfb_loge_display_debugfs("ade_overlay_handle_unlock:hnd_info->share_fd=%d, ph_addr=0x%x \n", hnd_info->share_fd, hnd_info->phy_addr);
            ion_unmap_iommu(fb_data->fb_ion_client,hnd_info->buff_hnd);
            ion_free(fb_data->fb_ion_client, hnd_info->buff_hnd);
            hnd_info->buff_hnd = NULL;
            hnd_info->lock_succ = ADE_FALSE;

        }
    }
}

static bool ade_overlay_check_ade_top_change(u8 *ade_base)
{
    int len;
    int i = 0;
    u32 top_reg[] = {ADE_CTRL_REG,
        ADE_SCL3_MUX_CFG_REG,
        ADE_SCL1_MUX_CFG_REG,
        ADE_ROT_SRC_CFG_REG,
        ADE_SCL2_SRC_CFG_REG,
        ADE_DISP_SRC_CFG_REG,
        ADE_WDMA2_SRC_CFG_REG,
        ADE_SEC_OVLY_SRC_CFG_REG,
        ADE_WDMA3_SRC_CFG_REG,
        ADE_OVLY1_TRANS_CFG_REG,
        ADE_CTRAN5_TRANS_CFG_REG,
        ADE_DMA_AXI_MUX_REG,
        ADE_CTRL1_REG,
        ADE_OVLY_CTL_REG};

    len = sizeof(top_reg)/sizeof(u32);
    for (i = 0; i < len; i++) {
        if ((inp32(ade_base + top_reg[i])) != (inp32(ade_base + top_reg[i] + 0x8000))) {
            return true;
        }
    }

    return false;
}

static void ade_overlay_dump_ade_top_reg(u8* ade_base)
{
    int len;
    int i = 0;
    u32 top_reg[] = {ADE_CTRL_REG,
        ADE_SCL3_MUX_CFG_REG,
        ADE_SCL1_MUX_CFG_REG,
        ADE_ROT_SRC_CFG_REG,
        ADE_SCL2_SRC_CFG_REG,
        ADE_DISP_SRC_CFG_REG,
        ADE_WDMA2_SRC_CFG_REG,
        ADE_SEC_OVLY_SRC_CFG_REG,
        ADE_WDMA3_SRC_CFG_REG,
        ADE_OVLY1_TRANS_CFG_REG,
        ADE_CTRAN5_TRANS_CFG_REG,
        ADE_DMA_AXI_MUX_REG,
        ADE_SOFT_RST_SEL0_REG,
        ADE_SOFT_RST_SEL1_REG,
        ADE_STAT0_REG,
        ADE_STAT1_REG,
        ADE_DMA_ERR_STAT_REG,
        ADE_CTRL1_REG,
        ADE_OVLY_CTL_REG,
        ADE_RELOAD_DIS0_REG,
        ADE_RELOAD_DIS1_REG,
        RD_CH_CMDQ1_STATUS_REG,
        RD_CH_CMDQ2_STATUS_REG};

    len = sizeof(top_reg)/sizeof(u32);
    for (i = 0; i < len; i++) {
        balongfb_loge("TOP shadow:0x%x = 0x%x \n", top_reg[i], inp32(ade_base + top_reg[i]));
        balongfb_loge("TOP main:0x%x = 0x%x \n", top_reg[i], inp32(ade_base + top_reg[i] + 0x8000));
    }
}

static void ade_overlay_init_scl_coef(u8* ade_base, u32 hcoef, u32 vcoef)
{
    volatile U_ADE_SCL1_VCOEF  scl_vcoef;
    volatile U_ADE_SCL1_HCOEF  scl_hcoef;
    u32 tmp_cmd_len;
    u32 coef_len = 0;
    u32 i = 0;

    i = 0;
    scl_hcoef.u32 = 0;
    tmp_cmd_len = 0;
    coef_len = sizeof(scl_lanczos3) / sizeof(s32);

    /* init hcoef */
    while (tmp_cmd_len < coef_len) {
        scl_hcoef.bits.hcoef_m_2n = scl_lanczos3[tmp_cmd_len];
        scl_hcoef.bits.hcoef_m_2n_1 = scl_lanczos3[tmp_cmd_len + 1];
        outp32(ade_base + hcoef + i * 4, scl_hcoef.u32);
        tmp_cmd_len += 2;
        i++;
    }

    i = 0;
    scl_vcoef.u32 = 0;
    tmp_cmd_len = 0;
    coef_len = sizeof(scl_lanczos2) / sizeof(s32);

    /* init vcoef */
    while (tmp_cmd_len < coef_len) {
        scl_vcoef.bits.vcoef_i_2j = scl_lanczos2[tmp_cmd_len];
        scl_vcoef.bits.vcoef_i_2j_1 = scl_lanczos2[tmp_cmd_len + 1];
        outp32(ade_base + vcoef + i * 4, scl_vcoef.u32);
        tmp_cmd_len += 2;
        i++;
    }
}

static void ade_overlay_set_blk_rot_scl_coef(u8* ade_base, u32 hcoef, u32 vcoef)
{
    volatile U_ADE_SCL1_VCOEF  scl_vcoef;
    volatile U_ADE_SCL1_HCOEF  scl_hcoef;
    u32 tmp_cmd_len;
    u32 coef_len = 0;
    u32 i = 0;

    i = 0;
    scl_hcoef.u32 = 0;
    tmp_cmd_len = 0;
    coef_len = sizeof(scl_blk_rot) / sizeof(u32);

    /* init hcoef */
    while (tmp_cmd_len < coef_len) {
        scl_hcoef.bits.hcoef_m_2n = scl_blk_rot[tmp_cmd_len];
        scl_hcoef.bits.hcoef_m_2n_1 = scl_blk_rot[tmp_cmd_len + 1];
        outp32(ade_base + hcoef + i * 4, scl_hcoef.u32);
        tmp_cmd_len += 2;
        i++;
    }

    i = 0;
    scl_vcoef.u32 = 0;
    tmp_cmd_len = 0;
    coef_len = sizeof(scl_blk_rot) / sizeof(s32);

    /* init vcoef */
    while (tmp_cmd_len < coef_len) {
        scl_vcoef.bits.vcoef_i_2j = scl_blk_rot[tmp_cmd_len];
        scl_vcoef.bits.vcoef_i_2j_1 = scl_blk_rot[tmp_cmd_len + 1];
        outp32(ade_base + vcoef + i * 4, scl_vcoef.u32);
        tmp_cmd_len += 2;
        i++;
    }
}

void ade_overlay_pipe_init4ch1(struct ade_overlay_pipe     *pipe)
{
    struct  ch_unit_reg         *unit_reg = NULL;

    BUG_ON(pipe == NULL);

    memset(&pipe->pipe_info, 0, sizeof(pipe->pipe_info));

    pipe->pipe_cap.bit_rot_enable       = 0;
    pipe->pipe_cap.bit_scl1_enable       = 0;
    pipe->pipe_cap.bit_scl3_enable       = 0;
    pipe->pipe_cap.bit_csc_enable       = 0;
    pipe->pipe_cap.bit_block_rot_enable = 1;
    pipe->pipe_cap.bit_ovly1            = 0;
    pipe->pipe_cap.bit_ovly2            = 1;
    pipe->pipe_cap.bit_ovly3            = 0;
    pipe->ade_base = balongfd_reg_base_ade;

    unit_reg = &pipe->pipe_reg;
    unit_reg->ch_rdma_ch_addr = RD_CH1_ADDR_REG;
    unit_reg->ch_rdma_ch_ctrl = RD_CH1_CTRL_REG;
    unit_reg->ch_rdma_ch_en   = RD_CH1_EN_REG;
    unit_reg->ch_rdma_ch_partial_size  = RD_CH1_PARTIAL_SIZE_REG;
    unit_reg->ch_rdma_ch_partial_space = RD_CH1_PARTIAL_SPACE_REG;
    unit_reg->ch_rdma_ch_pe            = RD_CH1_PE_REG;
    unit_reg->ch_rdma_ch_size          = RD_CH1_SIZE_REG;
    unit_reg->ch_rdma_ch_space         = RD_CH1_SPACE_REG;
    unit_reg->ch_rdma_ch_stride        = RD_CH1_STRIDE_REG;
    unit_reg->ch_rdma_ch_uv_addr       = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_partial_size = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_partial_space = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_size       = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_space      = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_stride     = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_extra_partial_width = ADE_INVAL_REG;

    unit_reg->clip_cfg_ok  = ADE_CLIP1_CFG_OK_REG;
    unit_reg->clip_disable = ADE_CLIP1_DISABLE_REG;
    unit_reg->clip_size0   = ADE_CLIP1_SIZE0_REG;
    unit_reg->clip_size1   = ADE_CLIP1_SIZE1_REG;
    unit_reg->clip_size2   = ADE_CLIP1_SIZE2_REG;

    unit_reg->ctran_cfg_ok = ADE_INVAL_REG;
    unit_reg->ctran_dis = ADE_INVAL_REG;
    unit_reg->ctran_mode_choose = ADE_INVAL_REG;
    unit_reg->ctran_chdc0 = ADE_INVAL_REG;
    unit_reg->ctran_csc0 = ADE_INVAL_REG;
    unit_reg->ctran_image_size = ADE_INVAL_REG;

    unit_reg->scl_ctrl = ADE_SCL1_CTRL_REG;
    unit_reg->scl_hsp  = ADE_SCL1_HSP_REG;
    unit_reg->scl_uv_hsp = ADE_SCL1_UV_HSP_REG;
    unit_reg->scl_vsp  = ADE_SCL1_VSP_REG;
    unit_reg->scl_uv_vsp = ADE_SCL1_UV_VSP_REG;
    unit_reg->scl_ires = ADE_SCL1_IRES_REG;
    unit_reg->scl_ores = ADE_SCL1_ORES_REG;
    unit_reg->scl_hcoef = ADE_SCL1_HCOEF_0_REG;
    unit_reg->scl_vcoef = ADE_SCL1_VCOEF_0_REG;
    unit_reg->scl_start = ADE_SCL1_START_REG;

    unit_reg->scl_rst_sel_bit      = 21;
    unit_reg->rdma_rst_sel_bit     = 0;
    unit_reg->clip_rst_sel_bit     = 15;
    unit_reg->ctran_rst_sel_bit    = ADE_INVAL_REG;
}
void ade_overlay_pipe_init4ch2(struct ade_overlay_pipe     *pipe)
{
    struct  ch_unit_reg         *unit_reg = NULL;

    BUG_ON(pipe == NULL);

    memset(&pipe->pipe_info, 0, sizeof(pipe->pipe_info));

    pipe->pipe_cap.bit_rot_enable       = 0;
    pipe->pipe_cap.bit_scl1_enable       = 0;
    pipe->pipe_cap.bit_scl3_enable       = 0;
    pipe->pipe_cap.bit_csc_enable       = 0;
    pipe->pipe_cap.bit_block_rot_enable = 0;
    pipe->pipe_cap.bit_ovly1            = 0;
    pipe->pipe_cap.bit_ovly2            = 1;
    pipe->pipe_cap.bit_ovly3            = 0;
    pipe->ade_base = balongfd_reg_base_ade;

    unit_reg = &pipe->pipe_reg;
    unit_reg->ch_rdma_ch_addr = RD_CH2_ADDR_REG;
    unit_reg->ch_rdma_ch_ctrl = RD_CH2_CTRL_REG;
    unit_reg->ch_rdma_ch_en   = RD_CH2_EN_REG;
    unit_reg->ch_rdma_ch_partial_size  = RD_CH2_PARTIAL_SIZE_REG;
    unit_reg->ch_rdma_ch_partial_space = RD_CH2_PARTIAL_SPACE_REG;
    unit_reg->ch_rdma_ch_pe            = RD_CH2_PE_REG;
    unit_reg->ch_rdma_ch_size          = RD_CH2_SIZE_REG;
    unit_reg->ch_rdma_ch_space         = RD_CH2_SPACE_REG;
    unit_reg->ch_rdma_ch_stride        = RD_CH2_STRIDE_REG;
    unit_reg->ch_rdma_ch_uv_addr       = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_partial_size = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_partial_space = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_size       = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_space      = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_stride     = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_extra_partial_width = ADE_INVAL_REG;

    unit_reg->clip_cfg_ok  = ADE_CLIP2_CFG_OK_REG;
    unit_reg->clip_disable = ADE_CLIP2_DISABLE_REG;
    unit_reg->clip_size0   = ADE_CLIP2_SIZE0_REG;
    unit_reg->clip_size1   = ADE_CLIP2_SIZE1_REG;
    unit_reg->clip_size2   = ADE_CLIP2_SIZE2_REG;

    unit_reg->ctran_cfg_ok = ADE_INVAL_REG;
    unit_reg->ctran_dis = ADE_INVAL_REG;
    unit_reg->ctran_mode_choose = ADE_INVAL_REG;
    unit_reg->ctran_chdc0 = ADE_INVAL_REG;
    unit_reg->ctran_csc0 = ADE_INVAL_REG;
    unit_reg->ctran_image_size = ADE_INVAL_REG;

    unit_reg->scl_ctrl = ADE_INVAL_REG;
    unit_reg->scl_hsp  = ADE_INVAL_REG;
    unit_reg->scl_uv_hsp = ADE_INVAL_REG;
    unit_reg->scl_vsp  = ADE_INVAL_REG;
    unit_reg->scl_uv_vsp = ADE_INVAL_REG;
    unit_reg->scl_ires = ADE_INVAL_REG;
    unit_reg->scl_ores = ADE_INVAL_REG;
    unit_reg->scl_hcoef = ADE_INVAL_REG;
    unit_reg->scl_vcoef = ADE_INVAL_REG;
    unit_reg->scl_start = ADE_INVAL_REG;

    unit_reg->rdma_rst_sel_bit     = 1;
    unit_reg->clip_rst_sel_bit     = 16;
    unit_reg->ctran_rst_sel_bit    = ADE_INVAL_REG;
    unit_reg->scl_rst_sel_bit      = ADE_INVAL_REG;
}
void ade_overlay_pipe_init4ch3(struct ade_overlay_pipe     *pipe)
{
    struct  ch_unit_reg         *unit_reg = NULL;

    BUG_ON(pipe == NULL);

    memset(&pipe->pipe_info, 0, sizeof(pipe->pipe_info));

    pipe->pipe_cap.bit_rot_enable       = 0;
    pipe->pipe_cap.bit_scl1_enable       = 0;
    pipe->pipe_cap.bit_scl3_enable       = 0;
    pipe->pipe_cap.bit_csc_enable       = 0;
    pipe->pipe_cap.bit_block_rot_enable = 0;
    pipe->pipe_cap.bit_ovly1            = 0;
    pipe->pipe_cap.bit_ovly2            = 1;
    pipe->pipe_cap.bit_ovly3            = 0;
    pipe->ade_base = balongfd_reg_base_ade;

    unit_reg = &pipe->pipe_reg;
    unit_reg->ch_rdma_ch_addr = RD_CH3_ADDR_REG;
    unit_reg->ch_rdma_ch_ctrl = RD_CH3_CTRL_REG;
    unit_reg->ch_rdma_ch_en   = RD_CH3_EN_REG;
    unit_reg->ch_rdma_ch_partial_size  = RD_CH3_PARTIAL_SIZE_REG;
    unit_reg->ch_rdma_ch_partial_space = RD_CH3_PARTIAL_SPACE_REG;
    unit_reg->ch_rdma_ch_pe            = RD_CH3_PE_REG;
    unit_reg->ch_rdma_ch_size          = RD_CH3_SIZE_REG;
    unit_reg->ch_rdma_ch_space         = RD_CH3_SPACE_REG;
    unit_reg->ch_rdma_ch_stride        = RD_CH3_STRIDE_REG;
    unit_reg->ch_rdma_ch_uv_addr       = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_partial_size = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_partial_space = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_size       = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_space      = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_stride     = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_extra_partial_width = ADE_INVAL_REG;

    unit_reg->clip_cfg_ok  = ADE_CLIP3_CFG_OK_REG;
    unit_reg->clip_disable = ADE_CLIP3_DISABLE_REG;
    unit_reg->clip_size0   = ADE_CLIP3_SIZE0_REG;
    unit_reg->clip_size1   = ADE_CLIP3_SIZE1_REG;
    unit_reg->clip_size2   = ADE_CLIP3_SIZE2_REG;

    unit_reg->ctran_cfg_ok = ADE_INVAL_REG;
    unit_reg->ctran_dis = ADE_INVAL_REG;
    unit_reg->ctran_mode_choose = ADE_INVAL_REG;
    unit_reg->ctran_chdc0 = ADE_INVAL_REG;
    unit_reg->ctran_csc0 = ADE_INVAL_REG;
    unit_reg->ctran_image_size = ADE_INVAL_REG;

    unit_reg->scl_ctrl = ADE_INVAL_REG;
    unit_reg->scl_hsp  = ADE_INVAL_REG;
    unit_reg->scl_uv_hsp = ADE_INVAL_REG;
    unit_reg->scl_vsp  = ADE_INVAL_REG;
    unit_reg->scl_uv_vsp = ADE_INVAL_REG;
    unit_reg->scl_ires = ADE_INVAL_REG;
    unit_reg->scl_ores = ADE_INVAL_REG;
    unit_reg->scl_hcoef = ADE_INVAL_REG;
    unit_reg->scl_vcoef = ADE_INVAL_REG;
    unit_reg->scl_start = ADE_INVAL_REG;

    unit_reg->rdma_rst_sel_bit     = 2;
    unit_reg->clip_rst_sel_bit     = 17;
    unit_reg->ctran_rst_sel_bit    = ADE_INVAL_REG;
    unit_reg->scl_rst_sel_bit      = ADE_INVAL_REG;
}

void ade_overlay_pipe_init4ch4(struct ade_overlay_pipe     *pipe)
{
    struct  ch_unit_reg         *unit_reg = NULL;

    BUG_ON(pipe == NULL);

    memset(&pipe->pipe_info, 0, sizeof(pipe->pipe_info));

    pipe->pipe_cap.bit_rot_enable       = 0;
    pipe->pipe_cap.bit_scl1_enable       = 0;
    pipe->pipe_cap.bit_scl3_enable       = 0;
    pipe->pipe_cap.bit_csc_enable       = 0;
    pipe->pipe_cap.bit_block_rot_enable = 0;
    pipe->pipe_cap.bit_ovly1            = 0;
    pipe->pipe_cap.bit_ovly2            = 1;
    pipe->pipe_cap.bit_ovly3            = 0;
    pipe->ade_base = balongfd_reg_base_ade;

    unit_reg = &pipe->pipe_reg;
    unit_reg->ch_rdma_ch_addr = RD_CH4_ADDR_REG;
    unit_reg->ch_rdma_ch_ctrl = RD_CH4_CTRL_REG;
    unit_reg->ch_rdma_ch_en   = RD_CH4_EN_REG;
    unit_reg->ch_rdma_ch_partial_size  = RD_CH4_PARTIAL_SIZE_REG;
    unit_reg->ch_rdma_ch_partial_space = RD_CH4_PARTIAL_SPACE_REG;
    unit_reg->ch_rdma_ch_pe            = RD_CH4_PE_REG;
    unit_reg->ch_rdma_ch_size          = RD_CH4_SIZE_REG;
    unit_reg->ch_rdma_ch_space         = RD_CH4_SPACE_REG;
    unit_reg->ch_rdma_ch_stride        = RD_CH4_STRIDE_REG;
    unit_reg->ch_rdma_ch_uv_addr       = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_partial_size = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_partial_space = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_size       = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_space      = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_uv_stride     = ADE_INVAL_REG;
    unit_reg->ch_rdma_ch_extra_partial_width = ADE_INVAL_REG;

    unit_reg->clip_cfg_ok  = ADE_CLIP4_CFG_OK_REG;
    unit_reg->clip_disable = ADE_CLIP4_DISABLE_REG;
    unit_reg->clip_size0   = ADE_CLIP4_SIZE0_REG;
    unit_reg->clip_size1   = ADE_CLIP4_SIZE1_REG;
    unit_reg->clip_size2   = ADE_CLIP4_SIZE2_REG;

    unit_reg->ctran_cfg_ok = ADE_INVAL_REG;
    unit_reg->ctran_dis = ADE_INVAL_REG;
    unit_reg->ctran_mode_choose = ADE_INVAL_REG;
    unit_reg->ctran_chdc0 = ADE_INVAL_REG;
    unit_reg->ctran_csc0 = ADE_INVAL_REG;
    unit_reg->ctran_image_size = ADE_INVAL_REG;

    unit_reg->scl_ctrl = ADE_INVAL_REG;
    unit_reg->scl_hsp  = ADE_INVAL_REG;
    unit_reg->scl_uv_hsp = ADE_INVAL_REG;
    unit_reg->scl_vsp  = ADE_INVAL_REG;
    unit_reg->scl_uv_vsp = ADE_INVAL_REG;
    unit_reg->scl_ires = ADE_INVAL_REG;
    unit_reg->scl_ores = ADE_INVAL_REG;
    unit_reg->scl_hcoef = ADE_INVAL_REG;
    unit_reg->scl_vcoef = ADE_INVAL_REG;
    unit_reg->scl_start = ADE_INVAL_REG;

    unit_reg->rdma_rst_sel_bit     = 3;
    unit_reg->clip_rst_sel_bit     = 18;
    unit_reg->ctran_rst_sel_bit    = ADE_INVAL_REG;
    unit_reg->scl_rst_sel_bit      = ADE_INVAL_REG;
}

void ade_overlay_pipe_init4ch5(struct ade_overlay_pipe     *pipe)
{
    struct  ch_unit_reg         *unit_reg = NULL;

    BUG_ON(pipe == NULL);

    memset(&pipe->pipe_info, 0, sizeof(pipe->pipe_info));

    pipe->pipe_cap.bit_rot_enable       = 1;
    pipe->pipe_cap.bit_scl1_enable       = 0;
    pipe->pipe_cap.bit_scl3_enable       = 1;
    pipe->pipe_cap.bit_csc_enable       = 1;
    pipe->pipe_cap.bit_block_rot_enable = 0;
    pipe->pipe_cap.bit_ovly1            = 0;
    pipe->pipe_cap.bit_ovly2            = 1;
    pipe->pipe_cap.bit_ovly3            = 0;
    pipe->ade_base = balongfd_reg_base_ade;

    unit_reg = &pipe->pipe_reg;
    unit_reg->ch_rdma_ch_addr = RD_CH5_ADDR_REG;
    unit_reg->ch_rdma_ch_ctrl = RD_CH5_CTRL_REG;
    unit_reg->ch_rdma_ch_en   = RD_CH5_EN_REG;
    unit_reg->ch_rdma_ch_partial_size  = RD_CH5_PARTIAL_SIZE_REG;
    unit_reg->ch_rdma_ch_partial_space = RD_CH5_PARTIAL_SPACE_REG;
    unit_reg->ch_rdma_ch_pe            = RD_CH5_PE_REG;
    unit_reg->ch_rdma_ch_size          = RD_CH5_SIZE_REG;
    unit_reg->ch_rdma_ch_space         = RD_CH5_SPACE_REG;
    unit_reg->ch_rdma_ch_stride        = RD_CH5_STRIDE_REG;
    unit_reg->ch_rdma_ch_uv_addr       = RD_CH5_UV_ADDR_REG;
    unit_reg->ch_rdma_ch_uv_partial_size = RD_CH5_UV_PARTIAL_SIZE_REG;
    unit_reg->ch_rdma_ch_uv_partial_space = RD_CH5_UV_PARTIAL_SPACE_REG;
    unit_reg->ch_rdma_ch_uv_size       = RD_CH5_UV_SIZE_REG;
    unit_reg->ch_rdma_ch_uv_space      = RD_CH5_UV_SPACE_REG;
    unit_reg->ch_rdma_ch_uv_stride     = RD_CH5_UV_STRIDE_REG;
    unit_reg->ch_rdma_ch_extra_partial_width = RD_CH5_EXTRA_PARTIAL_WIDTH_REG;

    unit_reg->clip_cfg_ok  = ADE_CLIP5_CFG_OK_REG;
    unit_reg->clip_disable = ADE_CLIP5_DISABLE_REG;
    unit_reg->clip_size0   = ADE_CLIP5_SIZE0_REG;
    unit_reg->clip_size1   = ADE_CLIP5_SIZE1_REG;
    unit_reg->clip_size2   = ADE_CLIP5_SIZE2_REG;

    unit_reg->ctran_cfg_ok = ADE_CTRAN1_CFG_OK_REG;
    unit_reg->ctran_dis = ADE_CTRAN1_DIS_REG;
    unit_reg->ctran_mode_choose = ADE_CTRAN1_MODE_CHOOSE_REG;
    unit_reg->ctran_chdc0 = ADE_CTRAN1_CHDC0_REG;
    unit_reg->ctran_csc0 = ADE_CTRAN1_CSC0_REG;
    unit_reg->ctran_image_size = ADE_CTRAN1_IMAGE_SIZE_REG;

    unit_reg->scl_ctrl = ADE_SCL3_CTRL_REG;
    unit_reg->scl_hsp  = ADE_SCL3_HSP_REG;
    unit_reg->scl_uv_hsp = ADE_SCL3_UV_HSP_REG;
    unit_reg->scl_vsp  = ADE_SCL3_VSP_REG;
    unit_reg->scl_uv_vsp = ADE_SCL3_UV_VSP_REG;
    unit_reg->scl_ires = ADE_SCL3_IRES_REG;
    unit_reg->scl_ores = ADE_SCL3_ORES_REG;
    unit_reg->scl_hcoef = ADE_SCL3_HCOEF_0_REG;
    unit_reg->scl_vcoef = ADE_SCL3_VCOEF_0_REG;
    unit_reg->scl_start = ADE_SCL3_START_REG;

    unit_reg->rdma_rst_sel_bit     = 4;
    unit_reg->clip_rst_sel_bit     = 19;
    unit_reg->ctran_rst_sel_bit    = 24;
    unit_reg->scl_rst_sel_bit      = 23;
}

void ade_overlay_pipe_init4ch6(struct ade_overlay_pipe     *pipe)
{
    struct  ch_unit_reg         *unit_reg = NULL;

    BUG_ON(pipe == NULL);

    memset(&pipe->pipe_info, 0, sizeof(pipe->pipe_info));

    pipe->pipe_cap.bit_rot_enable       = 1;
    pipe->pipe_cap.bit_scl1_enable       = 1;
    pipe->pipe_cap.bit_scl3_enable       = 0;
    pipe->pipe_cap.bit_csc_enable       = 1;
    pipe->pipe_cap.bit_block_rot_enable = 0;
    pipe->pipe_cap.bit_ovly1            = 0;
    pipe->pipe_cap.bit_ovly2            = 1;
    pipe->pipe_cap.bit_ovly3            = 0;
    pipe->ade_base = balongfd_reg_base_ade;

    unit_reg = &pipe->pipe_reg;
    unit_reg->ch_rdma_ch_addr = RD_CH6_ADDR_REG;
    unit_reg->ch_rdma_ch_ctrl = RD_CH6_CTRL_REG;
    unit_reg->ch_rdma_ch_en   = RD_CH6_EN_REG;
    unit_reg->ch_rdma_ch_partial_size  = RD_CH6_PARTIAL_SIZE_REG;
    unit_reg->ch_rdma_ch_partial_space = RD_CH6_PARTIAL_SPACE_REG;
    unit_reg->ch_rdma_ch_pe            = RD_CH6_PE_REG;
    unit_reg->ch_rdma_ch_size          = RD_CH6_SIZE_REG;
    unit_reg->ch_rdma_ch_space         = RD_CH6_SPACE_REG;
    unit_reg->ch_rdma_ch_stride        = RD_CH6_STRIDE_REG;
    unit_reg->ch_rdma_ch_uv_addr       = RD_CH6_UV_ADDR_REG;
    unit_reg->ch_rdma_ch_uv_partial_size = RD_CH6_UV_PARTIAL_SIZE_REG;
    unit_reg->ch_rdma_ch_uv_partial_space = RD_CH6_UV_PARTIAL_SPACE_REG;
    unit_reg->ch_rdma_ch_uv_size       = RD_CH6_UV_SIZE_REG;
    unit_reg->ch_rdma_ch_uv_space      = RD_CH6_UV_SPACE_REG;
    unit_reg->ch_rdma_ch_uv_stride     = RD_CH6_UV_STRIDE_REG;
    unit_reg->ch_rdma_ch_extra_partial_width = RD_CH6_EXTRA_PARTIAL_WIDTH_REG;

    unit_reg->clip_cfg_ok  = ADE_CLIP6_CFG_OK_REG;
    unit_reg->clip_disable = ADE_CLIP6_DISABLE_REG;
    unit_reg->clip_size0   = ADE_CLIP6_SIZE0_REG;
    unit_reg->clip_size1   = ADE_CLIP6_SIZE1_REG;
    unit_reg->clip_size2   = ADE_CLIP6_SIZE2_REG;

    unit_reg->ctran_cfg_ok = ADE_CTRAN2_CFG_OK_REG;
    unit_reg->ctran_dis = ADE_CTRAN2_DIS_REG;
    unit_reg->ctran_mode_choose = ADE_CTRAN2_MODE_CHOOSE_REG;
    unit_reg->ctran_chdc0 = ADE_CTRAN2_CHDC0_REG;
    unit_reg->ctran_csc0 = ADE_CTRAN2_CSC0_REG;
    unit_reg->ctran_image_size = ADE_CTRAN2_IMAGE_SIZE_REG;

    unit_reg->scl_ctrl = ADE_SCL1_CTRL_REG;
    unit_reg->scl_hsp  = ADE_SCL1_HSP_REG;
    unit_reg->scl_uv_hsp = ADE_SCL1_UV_HSP_REG;
    unit_reg->scl_vsp  = ADE_SCL1_VSP_REG;
    unit_reg->scl_uv_vsp = ADE_SCL1_UV_VSP_REG;
    unit_reg->scl_ires = ADE_SCL1_IRES_REG;
    unit_reg->scl_ores = ADE_SCL1_ORES_REG;
    unit_reg->scl_hcoef = ADE_SCL1_HCOEF_0_REG;
    unit_reg->scl_vcoef = ADE_SCL1_VCOEF_0_REG;
    unit_reg->scl_start = ADE_SCL1_START_REG;

    unit_reg->rdma_rst_sel_bit     = 5;
    unit_reg->clip_rst_sel_bit     = 20;
    unit_reg->ctran_rst_sel_bit    = 25;
    unit_reg->scl_rst_sel_bit      = 21;
}

void ade_overlay_pipe_init4disp(struct ade_overlay_pipe     *pipe)
{
    struct ch_unit_reg         *unit_reg = NULL;
    struct ch_capability 		*union_disp_cap;

    BUG_ON(pipe == NULL);
    memset(&pipe->pipe_info, 0, sizeof(pipe->pipe_info));

    union_disp_cap 							= &pipe->pipe_cap;
    union_disp_cap->bit_block_rot_enable 	= 0;
    union_disp_cap->bit_scl1_enable      	= 0;
    union_disp_cap->bit_scl3_enable       	= 0;
    union_disp_cap->bit_csc_enable       	= 1;
    union_disp_cap->bit_block_rot_enable 	= 0;
    union_disp_cap->bit_ovly1            	= 0;
    union_disp_cap->bit_ovly2            	= 0;
    union_disp_cap->bit_ovly3            	= 0;
    union_disp_cap->bit_rot_1080p			= 0;
    union_disp_cap->bit_rot_720p			= 0;

    pipe->ade_base 							= balongfd_reg_base_ade;

    unit_reg 								= &pipe->pipe_reg;

    unit_reg->clip_cfg_ok  					= ADE_INVAL_REG;
    unit_reg->clip_disable 					= ADE_INVAL_REG;
    unit_reg->clip_size0   					= ADE_INVAL_REG;
    unit_reg->clip_size1  					= ADE_INVAL_REG;
    unit_reg->clip_size2   					= ADE_INVAL_REG;

    unit_reg->ch_rdma_ch_addr 				= RD_CH_DISP_ADDR_REG;
    unit_reg->ch_rdma_ch_ctrl 				= RD_CH_DISP_CTRL_REG;
    unit_reg->ch_rdma_ch_en   				= RD_CH_DISP_EN_REG;
    unit_reg->ch_rdma_ch_pe           		= RD_CH_DISP_PE_REG;
    unit_reg->ch_rdma_ch_size         		= RD_CH_DISP_SIZE_REG;
    unit_reg->ch_rdma_ch_space         		= RD_CH_DISP_SPACE_REG;
    unit_reg->ch_rdma_ch_stride        		= RD_CH_DISP_STRIDE_REG;
    unit_reg->ctran_cfg_ok 					= ADE_CTRAN5_CFG_OK_REG;
    unit_reg->ctran_dis 					= ADE_CTRAN5_DIS_REG;
    unit_reg->ctran_mode_choose 			= ADE_CTRAN5_MODE_CHOOSE_REG;
    unit_reg->ctran_chdc0 					= ADE_CTRAN5_CHDC0_REG;
    unit_reg->ctran_csc0 					= ADE_CTRAN5_CSC0_REG;
    unit_reg->ctran_image_size 				= ADE_CTRAN5_IMAGE_SIZE_REG;

    unit_reg->scl_ctrl 						= ADE_INVAL_REG;
    unit_reg->scl_hsp  						= ADE_INVAL_REG;
    unit_reg->scl_uv_hsp 					= ADE_INVAL_REG;
    unit_reg->scl_vsp  						= ADE_INVAL_REG;
    unit_reg->scl_uv_vsp 					= ADE_INVAL_REG;
    unit_reg->scl_ires 						= ADE_INVAL_REG;
    unit_reg->scl_ores 						= ADE_INVAL_REG;
    unit_reg->scl_hcoef 					= ADE_INVAL_REG;
    unit_reg->scl_vcoef 					= ADE_INVAL_REG;
    unit_reg->scl_start 					= ADE_INVAL_REG;

    unit_reg->rdma_rst_sel_bit   			= 6;
    unit_reg->clip_rst_sel_bit     			= ADE_INVAL_REG;
    unit_reg->ctran_rst_sel_bit   			= 28;
    unit_reg->scl_rst_sel_bit      			= ADE_INVAL_REG;
}

void ade_overlay_ovly_init4ovly(struct  ovly_reg *povly_reg)
{
    int   i = 0;

    BUG_ON(povly_reg == NULL);

    povly_reg->ovly_alpha_state = ADE_OVLY_ALPHA_ST_REG;
    povly_reg->ovly_ch1_xy2 = ADE_OVLY_CH1_XY2_REG;
    povly_reg->ovly_ch1_xy3 = ADE_OVLY_CH1_XY3_REG;

    for (i = 0; i < OVERLAY_ADE_SURFACE_MAX; i++) {
        povly_reg->ovly_ch_xy0[i] = ADE_OVLY_CH1_XY0_REG + 4 * i;
        povly_reg->ovly_ch_xy1[i] = ADE_OVLY_CH1_XY1_REG + 4 * i;
        povly_reg->ovly_ch_ctrl[i] = ADE_OVLY_CH1_CTL_REG + 4 * i;
        povly_reg->ovly_ch_sel_start[i] = i * 4;
    }
}

void ade_overlay_init(struct ade_overlay_ctrl *overlay_ctrl)
{
    struct ade_overlay_pipe     *pipe = NULL;
    struct ade_overlay_ovly     *ovly = NULL;

    BUG_ON(overlay_ctrl == NULL);

    pipe = &overlay_ctrl->pipe_list[OVERLAY_PIPE_ADE_CH1];
    pipe->pipe_num = OVERLAY_PIPE_ADE_CH1;
    ade_overlay_pipe_init4ch1(pipe);

    pipe = &overlay_ctrl->pipe_list[OVERLAY_PIPE_ADE_CH2];
    pipe->pipe_num = OVERLAY_PIPE_ADE_CH2;
    ade_overlay_pipe_init4ch2(pipe);

    pipe = &overlay_ctrl->pipe_list[OVERLAY_PIPE_ADE_CH3];
    pipe->pipe_num = OVERLAY_PIPE_ADE_CH3;
    ade_overlay_pipe_init4ch3(pipe);

    pipe = &overlay_ctrl->pipe_list[OVERLAY_PIPE_ADE_CH4];
    pipe->pipe_num = OVERLAY_PIPE_ADE_CH4;
    ade_overlay_pipe_init4ch4(pipe);

    pipe = &overlay_ctrl->pipe_list[OVERLAY_PIPE_ADE_CH5];
    pipe->pipe_num = OVERLAY_PIPE_ADE_CH5;
    ade_overlay_pipe_init4ch5(pipe);

    pipe = &overlay_ctrl->pipe_list[OVERLAY_PIPE_ADE_CH6];
    pipe->pipe_num = OVERLAY_PIPE_ADE_CH6;
    ade_overlay_pipe_init4ch6(pipe);

    pipe = &overlay_ctrl->pipe_list[OVERLAY_PIPE_ADE_DISP];
    pipe->pipe_num = OVERLAY_PIPE_ADE_DISP;
    ade_overlay_pipe_init4disp(pipe);

    /**********************ovly ***************************/
    ovly = &(overlay_ctrl->ovly_list[OVERLAY_OVLY_1]);
    ovly->ovly_num = OVERLAY_OVLY_1;
    ovly->ovly_cap = 0;
    ovly->ade_base = balongfd_reg_base_ade;
    memset(&(ovly->ovly_comp), 0, sizeof(ovly->ovly_comp));
    ovly->ovly_reg.ovly_output_size = ADE_OVLY1_OUTPUT_SIZE_REG;
    ovly->ovly_reg.ovly_ctrl        = ADE_OVLY1_CTL_REG;
    ovly->ovly_reg.ovly_rst_sel_bit = 5;
    ade_overlay_ovly_init4ovly(&(ovly->ovly_reg));


    ovly = &(overlay_ctrl->ovly_list[OVERLAY_OVLY_2]);
    ovly->ovly_num = OVERLAY_OVLY_2;
    ovly->ovly_cap = 0;
    ovly->ade_base = balongfd_reg_base_ade;
    memset(&(ovly->ovly_comp), 0, sizeof(ovly->ovly_comp));
    ovly->ovly_reg.ovly_output_size = ADE_OVLY2_OUTPUT_SIZE_REG;
    ovly->ovly_reg.ovly_ctrl        = ADE_OVLY2_CTL_REG;
    ovly->ovly_reg.ovly_rst_sel_bit = 6;
    ade_overlay_ovly_init4ovly(&(ovly->ovly_reg));


    ovly = &(overlay_ctrl->ovly_list[OVERLAY_OVLY_3]);
    ovly->ovly_num = OVERLAY_OVLY_3;
    ovly->ovly_cap = 0;
    ovly->ade_base = balongfd_reg_base_ade;
    memset(&(ovly->ovly_comp), 0, sizeof(ovly->ovly_comp));
    ovly->ovly_reg.ovly_output_size = ADE_OVLY3_OUTPUT_SIZE_REG;
    ovly->ovly_reg.ovly_ctrl        = ADE_OVLY3_CTL_REG;
    ovly->ovly_reg.ovly_rst_sel_bit = 7;
    ade_overlay_ovly_init4ovly(&(ovly->ovly_reg));

    overlay_ctrl->ovly2_ch_used = 0;
    overlay_ctrl->ovly1_ch_used = 0;
    memset(&overlay_ctrl->ade_res_info, 0, sizeof(struct overlay_resource_info));
    memset(&overlay_ctrl->comp_info, 0, sizeof(struct overlay_compose_info));
    memset(&overlay_ctrl->rot_info, 0, sizeof(struct ade_overlay_rot));
    memset(&overlay_ctrl->scl2_info, 0, sizeof(struct ade_overlay_scl2));
    memset(&overlay_ctrl->ade_demux_list, 0, sizeof(overlay_ctrl->ade_demux_list));
    memset(&overlay_ctrl->ade_mux_list, 0, sizeof(overlay_ctrl->ade_mux_list));
}

void ade_overlay_init_wdma2_buff(struct ion_client *fb_ion_client, struct ade_compose_data_type *ade_pri_data)
{
    ade_pri_data->wdma2_phy = 0;
#if 0
    u32   buff_size;
    u32   *wdma2_vaddr;

    BUG_ON(ade_pri_data == NULL);

    buff_size = ade_pri_data->xres * ade_pri_data->yres * ade_overlay_format2bpp(ade_pri_data->fb_imgType);
    if (0 == buff_size) {
        balongfb_loge("get wdma2 buffer size eroor!\n");
        return;
    }

    if(g_smmu_flag == ION_IOMMU_MODE){
        if (fb_ion_client == NULL) {
            balongfb_loge("fb_ion_client is null \n");
            return;
        }

        ade_pri_data->wdma2_ion_handle = ion_alloc(fb_ion_client, IOMMU_PAGE_ALIGN(buff_size), IOMMU_PAGE_SIZE, ION_HEAP(ION_GRALLOC_HEAP_ID), ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC);
        if (IS_ERR(ade_pri_data->wdma2_ion_handle)) {
            balongfb_loge("get wdma2 ion handle error buffer size = %d \n", IOMMU_PAGE_ALIGN(buff_size));
            return;
        }

        if (ion_phys(fb_ion_client, ade_pri_data->wdma2_ion_handle, (ion_phys_addr_t *)(&ade_pri_data->wdma2_phy), &buff_size) < 0) {
            balongfb_loge("ion_phys handle error \n");
            ade_pri_data->wdma2_phy = 0xdeadbeaf;
            return;
        }

        balongfb_loge(" ade_pri_data->wdma2_phy = 0x%x \n", ade_pri_data->wdma2_phy);
    }
    else{
        wdma2_vaddr = (u32 *)kzalloc(buff_size, GFP_KERNEL);
        if (wdma2_vaddr == NULL) {
            balongfb_loge("kzalloc wdma2 addr fail \n");
            return;
        }

        ade_pri_data->wdma2_phy = virt_to_phys(wdma2_vaddr);
    }
#endif
}

void ade_overlay_get_rect_from_rot(int rot, struct ade_rect *irect, struct ade_rect *orect)
{
    if ((irect == NULL) || (orect == NULL)) {
        balongfb_loge("irect or orect is null ptr \n");
        return;
    }

    //orect->x = irect->x;
    //orect->y = irect->y;
    switch (rot) {
        case ADE_ROT_NOP:
        case ADE_ROT_180:
        case ADE_ROT_H_MIRROR:
        case ADE_ROT_V_MIRROR:
            orect->w = irect->w;
            orect->h = irect->h;
            break;
        case ADE_ROT_90:
        case ADE_ROT_270:
        case ADE_ROT_90_H_MIRROR:
        case ADE_ROT_90_V_MIRROR:
            orect->w = irect->h;
            orect->h = irect->w;
            break;
        default :
            orect->w = irect->w;
            orect->h = irect->h;
            return;
    }

    return;
}

void ade_overlay_get_dma_rot_buff(u8 rot_buff, struct overlay_region_info *pipe_info, u32 *max_buff, u32 *uv_max_buff, u32 *enable_h)
{
    u32     format;
    u32     buff;

    BUG_ON((pipe_info == NULL) || (max_buff == NULL) || (uv_max_buff == NULL) || (enable_h == NULL));

    if (rot_buff == TOP_ROT_BUFF_ROT_720p) {
        buff = 38400; /* 76.8k / 2 * 1000 */
    } else if (rot_buff == TOP_ROT_BUFF_ROT_1080p) {
        buff = 76800; /* 153.6k / 2 * 1000 */
    } else if (rot_buff == TOP_ROT_BUFF_ROT_BLOCK){
        buff = 8192;  /* 16k / 2 * 1024  */
    } else {
        balongfb_logi_display_debugfs("ade_overlay_get_dma_rot_buff: no rot buff, not need to config rdma \n");
        return;
    }

    format = pipe_info->format;
    *uv_max_buff = 0;
    switch (format) {
        case ADE_RGB_565:
        case ADE_BGR_565:
            *max_buff = buff / 2;
            break;
        case ADE_RGB_888:
        case ADE_BGR_888:
        case ADE_YUV444:
            *max_buff = buff * 3 / 4;
            break;
        case ADE_NV12:
        case ADE_NV21:
            *max_buff = buff * 2 / 3;
            *uv_max_buff = buff / 3;
            break;
        default:
            *max_buff = buff;
            break;
    }

    switch (pipe_info->rotation) {
        case ADE_ROT_90:
        case ADE_ROT_270:
        case ADE_ROT_90_H_MIRROR:
        case ADE_ROT_90_V_MIRROR:
            *enable_h = 0;   /* first vertial, then h */
            break;
        case ADE_ROT_180:
        case ADE_ROT_H_MIRROR:
        case ADE_ROT_V_MIRROR:
            *enable_h = 1;   /* first h, then vertial */
            break;
        default:
            balongfb_logi_display_debugfs("ade_overlay_region_set_partial_rdma: rotation = %d is not need to partial !\n", pipe_info->rotation);
            return;
    }
}

int ade_overlay_set_rot_cmd(u8* ade_base, struct ade_overlay_rot* rot_info, struct cmdfile_buffer* cf_buff)
{
    BUG_ON((rot_info == NULL));

    if (cf_buff == NULL) {
        return -EINVAL;
    }

    if (!(rot_info->rot_en)) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_rot_cmd: ROT don't have para, is_busy = false, rot_info->rot_en is not enable \n");
        return -EINVAL;
    }

    if ((rot_info->rot_height == 0) || (rot_info->rot_width == 0)) {
        balongfb_logi_display_debugfs("rot_info->rot_height = 0, rot_info->rot_width = 0 \n");
        return 0;
    }

    ade_cmdq_wr_rot_cfg_cmd(ADE_ROT_CFG_REG, rot_info);
    ade_cmdq_wr_cmd(ADE_ROT_SIZE_REG, ((rot_info->rot_height -1) << 12) | (rot_info->rot_width - 1));
    ade_cmdq_wr_cmd(ADE_ROT_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* Not support to Rotate after composition */
    rot_info->is_configed = ADE_TRUE;
    return 0;
}

static void ade_overlay_set_scl2_cmd(u8* ade_base, struct ade_overlay_scl2 *scl2_info, struct cmdfile_buffer *cf_buff)
{
    BUG_ON((scl2_info == NULL));

    if (cf_buff == NULL) {
        return;
    }

    balongfb_logi_display_debugfs("ade_overlay_set_scl2_cmd: src_rect.w = %d, src_rect.h = %d \n", scl2_info->src_rect.w,scl2_info->src_rect.h);
    balongfb_logi_display_debugfs("ade_overlay_set_scl2_cmd: dst_rect.w = %d, dst_rect.h = %d \n", scl2_info->dst_rect.w,scl2_info->dst_rect.h);

    if (!(scl2_info->scl2_en)) {
        balongfb_logi_display_debugfs("ade_overlay_set_scl2_cmd: scl2 don't have para, is_busy = %d, scl2_info->scl2_en = %d \n", scl2_info->scl2_en);
        return;
    }

    if ((scl2_info->dst_rect.w == 0) || (scl2_info->dst_rect.h == 0)) {
        balongfb_logi_display_debugfs("ade_overlay_set_scl2_cmd: scl2 is not need to config, bsz w=0 or h = 0 !\n");
        return;
    }

    /* transform scl2_state from wait_cfg to CFG_SUCC */
    scl2_info->scl2_state = ADE_SCL2_STATE_CFG_SUCC;

    /* bypass scl2 */
    if ((scl2_info->src_rect.w == scl2_info->dst_rect.w)
     && (scl2_info->src_rect.h == scl2_info->dst_rect.h)) {
        ade_cmdq_wr_cmd(ADE_SCL2_CTRL_REG, 0x400);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(ADE_SCL2_ORES_REG, (scl2_info->dst_rect.h - 1) << 16 | (scl2_info->dst_rect.w - 1));
        ade_cmdq_wr_cmd(ADE_SCL2_IRES_REG, (scl2_info->src_rect.h - 1) << 16 | (scl2_info->src_rect.w - 1));
        ade_cmdq_wr_cmd(ADE_SCL2_START_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: scl2 is bypassed \n");

        return;
    }

    /* one wr cmd */
    /* scl2 can't support nv12 */
    ade_cmdq_wr_scl2_ctrl_cmd(ADE_SCL2_CTRL_REG, scl2_info, &(scl2_info->src_rect), &(scl2_info->dst_rect));
    ade_cmdq_wr_scl_hsp_cmd(ADE_SCL2_HSP_REG, &(scl2_info->src_rect), &(scl2_info->dst_rect));
    ade_cmdq_wr_cmd(ADE_SCL2_UV_HSP_REG, 0);  /* TODO:UV HSP */
    ade_cmdq_wr_scl_vsp_cmd(ADE_SCL2_VSP_REG, &(scl2_info->src_rect), &(scl2_info->dst_rect));
    ade_cmdq_wr_cmd(ADE_SCL2_UV_VSP_REG, 0); /* TODO:UV VSP */
    ade_cmdq_wr_cmd(ADE_SCL2_ORES_REG, (scl2_info->dst_rect.h - 1) << 16 | (scl2_info->dst_rect.w - 1));
    ade_cmdq_wr_cmd(ADE_SCL2_IRES_REG, (scl2_info->src_rect.h - 1) << 16 | (scl2_info->src_rect.w - 1));/* input size */
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* two wr cmd */
    ade_cmdq_wr_scl_hcoef_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_SCL2_HCOEF_0_REG, &(scl2_info->src_rect), &(scl2_info->dst_rect));
    ade_cmdq_wr_scl_vcoef_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_SCL2_VCOEF_0_REG, &(scl2_info->src_rect), &(scl2_info->dst_rect));

    /* four wr cmd */
    ade_cmdq_wr_cmd(ADE_SCL2_START_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* disable dfs buf */
    set_TOP_CTL_dfs_buf_cfg(ade_base, 1);
}

void ade_overlay_set_sec_ovly(struct ade_overlay_ctrl *overlay_ctl, struct ade_overlay_ovly *sec_ovly, struct cmdfile_buffer *cf_buff)
{
    struct ade_overlay_pipe *pipe = NULL;
    struct ovly_info        *ovly_comp;
    struct ovly_reg         *ovly_reg;
    struct ovly_ctrl_info    ch_ctrl_info;
    u32       xy;
    u32       output_size;
    u32       surface_num;

    BUG_ON((cf_buff == NULL) || (overlay_ctl == NULL));

    if (sec_ovly == NULL) {
        balongfb_loge("sec ovly error sec_ovly is null");
        return;
    }

    ovly_comp = &(sec_ovly->ovly_comp);
    ovly_reg  = &(sec_ovly->ovly_reg);

    if (ovly_comp->sec_ovly_surf_num >= OVERLAY_ADE_SURFACE_MAX) {
        balongfb_loge("sec_ovly_surf_num is overflow \n");
        return;
    }

    pipe = ade_overlay_idx2pipe(overlay_ctl, ovly_comp->sec_ovly_ch_num);
    if (pipe == NULL) {
        balongfb_loge(" ade_overlay_idx2pipe get pipe error,pipe_num %d ", ovly_comp->sec_ovly_ch_num);
        return;
    }

    balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly: ovly_comp->sec_ovly_surf_num = %d \n", ovly_comp->sec_ovly_surf_num);

    surface_num = ovly_comp->sec_ovly_surf_num;

    /* set top ovly ctrl, select ovly of the surface_num  */
    set_TOP_OVLY_CTRL(sec_ovly->ade_base, ovly_reg->ovly_ch_sel_start[surface_num], sec_ovly->ovly_num + 1);

    /* xy0 */
    xy = ((ovly_comp->input_rect.x - ovly_comp->output_rect.x)<< 16) | (ovly_comp->input_rect.y - ovly_comp->output_rect.y);
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy0[surface_num], xy);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly: sec_ovly: xy0 = 0x%x \n", xy);

    /* xy1 */
    xy = ((ovly_comp->input_rect.x - ovly_comp->output_rect.x + ovly_comp->input_rect.w - 1) << 16) \
         | (ovly_comp->input_rect.y - ovly_comp->output_rect.y + ovly_comp->input_rect.h - 1);
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy1[surface_num], xy);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly: sec_ovly: xy1 = 0x%x \n", xy);

    /*partial update must cfg ch1_xy2, ch1_xy3 */

    /* output size  */
    output_size = ((ovly_comp->output_rect.w -1) << 16) | (ovly_comp->output_rect.h - 1);
    ade_cmdq_wr_cmd(ovly_reg->ovly_output_size, output_size);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly: sec_ovly: output_size = 0x%x \n", output_size);

    /* ch ctrl */
    ch_ctrl_info.ovly_num = sec_ovly->ovly_num;
    ch_ctrl_info.ch_num = pipe->pipe_info.ch_num;
    ch_ctrl_info.blending = pipe->pipe_info.blending;
    ch_ctrl_info.dim_blending = pipe->pipe_info.dim_blending;
    ch_ctrl_info.format = pipe->pipe_info.format;
    ch_ctrl_info.sec_ovly_ch_count = pipe->pipe_info.sec_ovly_ch_count;
    if (1 == ch_ctrl_info.sec_ovly_ch_count) {
        ch_ctrl_info.ovly_type = ADE_OVLY_TYPE_FIRST_OVLY;
    } else {
        ch_ctrl_info.ovly_type = ADE_OVLY_TYPE_SEC_OVLY;
    }
    ade_cmdq_wr_ovly_ch_ctrl_cmd(ovly_reg->ovly_ch_ctrl[surface_num], ch_ctrl_info);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    if (OVERLAY_OVLY_2 == sec_ovly->ovly_num) {
        overlay_ctl->ovly2_ch_used |= 1 << surface_num;
    } else {
        overlay_ctl->ovly1_ch_used |= 1 << surface_num;
    }
}

void ade_overlay_set_bypass_ovly(struct ade_compose_data_type *ade_pri_data,
        struct ade_overlay_ctrl *overlay_ctl,
        struct ade_overlay_pipe *pipe,
        struct ade_overlay_ovly *ovly,
        struct cmdfile_buffer *cf_buff)
{
    struct overlay_region_info *pipe_info;
    struct ade_overlay_rot     *rot_info;
    struct ade_overlay_scl2    *scl2_info;
    struct ade_rect             ovly_irect;
    struct ovly_ctrl_info       ch_ctrl_info;
    struct ovly_reg            *ovly_reg;
    u32          surf_num;
    OVLY3_RES    ovly3_res;
    ROT_RES      rot_res;
    u32          xy0;
    u32          xy1;
    u32          output_size;

    if (ovly->ovly_num != OVERLAY_OVLY_3) {
        balongfb_loge("ade_overlay_set_bypass_ovly set sec ovly pass error, ovly_num = %d, != ovly3 \n", ovly->ovly_num);
        return;
    }

    pipe_info = &(pipe->pipe_info);

    if (pipe_info->sec_ovly_ch_count != 1) {
        balongfb_loge("ade_overlay_set_bypass_ovly sec_ovly_ch_count=%d != 1 \n", pipe_info->sec_ovly_ch_count);
        return;
    }

    if ((0 == pipe_info->phy_addr) || (0 == pipe_info->width) || (0 == pipe_info->height)) {
        balongfb_logi_display_debugfs("ade_overlay_set_bypass_ovly: phy_addr or w or h is 0, don't need config \n");
        return;
    }

    rot_info  = &overlay_ctl->rot_info;
    scl2_info = &overlay_ctl->scl2_info;
    //memcpy(&ovly_irect, &(pipe_info->src_rect), sizeof (struct ade_rect));
    memcpy(&ovly_irect, &(pipe->output_rect), sizeof (struct ade_rect));
    ovly_reg  = &(ovly->ovly_reg);

    /* when have the second ovly, the surface_num of ovly3 must be different with the surface_num of other ovly.
     * so, we confirm that the surface_8 will be allocated for ovly3, when have the second ovly.
     */
    surf_num = OVERLAY_ADE_SURFACE_8;

    /* when ovly3 have ROT unit, but the rotation is 0, must config ROT bypass */
    ovly3_res.ul32 = ovly->ovly_cap;
    rot_res.ul32   = overlay_ctl->ade_res_info.rot_res.ul32;
    if (ovly3_res.bits.has_720p_rot || ovly3_res.bits.has_1080p_rot) {
        rot_info->rotation = 0;
    }

    if ((ADE_SCL2_STATE_WAIT_CFG == scl2_info->scl2_state)
            && (ADE_TRUE == pipe->need_cfg_scl2)) {
        scl2_info->scl2_iformat = pipe_info->format;
        memcpy(&scl2_info->src_rect, &pipe->output_rect, sizeof(struct ade_rect));
        scl2_info->dst_rect.x     = pipe->output_rect.x;
        scl2_info->dst_rect.y     = pipe->output_rect.y;
        scl2_info->dst_rect.w     = pipe_info->dst_width;
        scl2_info->dst_rect.h     = pipe_info->dst_height;
    }

#if 0
    if (rot_res.bits.has_scl2 == ADE_TRUE) {
        is_need_scl2 = ADE_TRUE;
    }
}

if (ovly3_res.bits.has_scl2) {
    is_need_scl2 = ADE_TRUE;
}

/* scl must be config by once, can't be cut */
if (is_need_scl2 == ADE_TRUE) {
    scl2_info->scl2_iformat = pipe_info->format;
    scl2_info->need_cfg = ADE_TRUE;
    memcpy(&scl2_info->src_rect, &ovly_irect, sizeof(struct ade_rect));
    scl2_info->dst_rect.x     = ovly_irect.x;
    scl2_info->dst_rect.y     = ovly_irect.y;
    scl2_info->dst_rect.w     = pipe_info->dst_width;
    scl2_info->dst_rect.h     = pipe_info->dst_height;
}
#endif

/* set top ovly ctrl, select ovly of the surface_num  */
set_TOP_OVLY_CTRL(ovly->ade_base, ovly_reg->ovly_ch_sel_start[surf_num], ovly->ovly_num + 1);

xy0 = 0;
xy1 = ((ovly_irect.w - 1) << 16) | (ovly_irect.h - 1);

/* xy0 */
ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy0[surf_num], xy0);
ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

/* xy1 */
ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy1[surf_num], xy1);
ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

output_size = ((ovly_irect.w -1) << 16) | (ovly_irect.h - 1);

/* the first ovly output size  */
ade_cmdq_wr_cmd(ovly_reg->ovly_output_size, output_size);
ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
balongfb_logi_display_debugfs("ade_overlay_set_bypass_ovly:output_size = 0x%x, xy0 = 0x%x, xy1 = 0x%x \n",output_size, xy0, xy1);

/* ovly_ch ctrl */
ch_ctrl_info.ovly_num = ovly->ovly_num;
ch_ctrl_info.ch_num = pipe_info->ch_num;
ch_ctrl_info.blending = pipe_info->blending;
ch_ctrl_info.dim_blending = pipe_info->dim_blending;
ch_ctrl_info.format = pipe_info->format;
ch_ctrl_info.sec_ovly_ch_count = 1;
ch_ctrl_info.ovly_type = ADE_OVLY_TYPE_BYPASS;
ade_cmdq_wr_ovly_ch_ctrl_cmd(ovly_reg->ovly_ch_ctrl[surf_num], ch_ctrl_info);
ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

/* ovly ctrl start */
ade_cmdq_wr_cmd(ovly_reg->ovly_ctrl, ADE_TRUE);
ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

overlay_ctl->ovly2_ch_used |= 1 << surf_num;
overlay_ctl->ovly1_ch_used |= 1 << surf_num;

if (OVERLAY_PIPE_TYPE_ONLINE == pipe_info->ch_type) {
    set_soft_rst_sel_bit(ade_pri_data->soft_rst1, ovly_reg->ovly_rst_sel_bit, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis1, ovly_reg->ovly_rst_sel_bit);

    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd: ovly_rst_sel_bit=%d\n", ovly_reg->ovly_rst_sel_bit);
}

balongfb_logi_display_debugfs("ade_overlay_set_bypass_ovly: output_rect.x = %d,output_rect.y=%d \n", ovly_irect.x,ovly_irect.y);
balongfb_logi_display_debugfs("ade_overlay_set_bypass_ovly: output_rect.w = %d,output_rect.h=%d \n", ovly_irect.w,ovly_irect.h);
balongfb_logi_display_debugfs("ade_overlay_set_bypass_ovly: ovly_num=%d, ovly_ctrl start enable here!!! \n", ovly->ovly_num);
}

/*****************  TOP CFG BEGIN**************************************************/
int ade_overlay_get_ch_top(CH_RES ch_res, u32 *demux_list, struct  ch_capability   *pipe_cap)
{
    int     tmp = 0;

    BUG_ON((demux_list == NULL) || (pipe_cap == NULL));

    memset(pipe_cap, 0, sizeof(struct ch_capability));

    if (ADE_FALSE == ch_res.bits.ch_en) {
        return 0;
    }

    /*  1080p rot */
    if (ch_res.bits.has_1080p_rot) {
        if ((ch_res.bits.ch_num != OVERLAY_PIPE_ADE_CH5) && (ch_res.bits.ch_num != OVERLAY_PIPE_ADE_CH6)) {
            balongfb_loge("get ch top error: just ch6 and ch5 have 1080p rotation \n!");
        } else if (OVERLAY_PIPE_ADE_CH5 == ch_res.bits.ch_num) {
            demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH5_1080P;
            pipe_cap->bit_rot_enable = ADE_TRUE;
            pipe_cap->bit_rot_1080p  = ADE_TRUE;
            pipe_cap->bit_rot_720p   = ADE_FALSE;

            balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_1080p_rot \n",ch_res.bits.ch_num);
        } else {
            demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH6_1080P;
            pipe_cap->bit_rot_enable = ADE_TRUE;
            pipe_cap->bit_rot_1080p  = ADE_TRUE;
            pipe_cap->bit_rot_720p   = ADE_FALSE;

            balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_1080p_rot \n",ch_res.bits.ch_num);
        }
    }

    /*  720p rot */
    if (ch_res.bits.has_720p_rot) {
        if ((ch_res.bits.ch_num != OVERLAY_PIPE_ADE_CH5) && (ch_res.bits.ch_num != OVERLAY_PIPE_ADE_CH6)) {
            balongfb_loge("get ch top error: just ch6 and ch5 have 720p rotation \n!");
        } else if (OVERLAY_PIPE_ADE_CH5 == ch_res.bits.ch_num) {
            demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH5_CH6_720P;
            pipe_cap->bit_rot_enable = ADE_TRUE;
            pipe_cap->bit_rot_1080p  = ADE_FALSE;
            pipe_cap->bit_rot_720p   = ADE_TRUE;
            balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_720p_rot \n",ch_res.bits.ch_num);
        } else {
            if (demux_list[TOP_DEMUX_ROT_BUFF] != TOP_ROT_BUFF_CH5_CH6_720P) {
                demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH6_ROT_720P;
            }

            pipe_cap->bit_rot_enable = ADE_TRUE;
            pipe_cap->bit_rot_1080p  = ADE_FALSE;
            pipe_cap->bit_rot_720p   = ADE_TRUE;
            balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_720p_rot \n",ch_res.bits.ch_num);
        }
    }

    /*  block rot */
    if (ch_res.bits.has_block_rot) {
        if ((ch_res.bits.ch_num != OVERLAY_PIPE_ADE_CH6) && (ch_res.bits.ch_num != OVERLAY_PIPE_ADE_CH1)) {
            balongfb_loge("get ch top error: just ch6 and ch1 have block rotation \n!");
        } else if (OVERLAY_PIPE_ADE_CH1 == ch_res.bits.ch_num) {
            if ((TOP_ROT_BUFF_ROT_1080P == demux_list[TOP_DEMUX_ROT_BUFF])
                    || (TOP_ROT_BUFF_CH5_1080P == demux_list[TOP_DEMUX_ROT_BUFF])) {
                demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH6_1080P;
            }
            pipe_cap->bit_block_rot_enable = ADE_TRUE;
            pipe_cap->bit_rot_enable = ADE_TRUE;
            balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_block_rot \n",ch_res.bits.ch_num);
        } else {
            pipe_cap->bit_block_rot_enable = ADE_TRUE;
            pipe_cap->bit_rot_enable = ADE_TRUE;
            balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_block_rot \n",ch_res.bits.ch_num);
        }
    }

    /* scl1 */
    if (ch_res.bits.has_scl1) {
        pipe_cap->bit_scl1_enable = ADE_TRUE;
        if (OVERLAY_PIPE_ADE_CH6 == ch_res.bits.ch_num) {
            demux_list[TOP_DEMUX_SCL1] = TOP_SCL1_ON_CH6;
        } else if (OVERLAY_PIPE_ADE_CH1 == ch_res.bits.ch_num){
            demux_list[TOP_DEMUX_SCL1] = TOP_SCL1_ON_CH1;
        } else if (OVERLAY_PIPE_ADE_CH5 == ch_res.bits.ch_num) {
            demux_list[TOP_DEMUX_SCL1] = TOP_SCL1_ON_CH5;
        } else{
            pipe_cap->bit_scl1_enable   = ADE_FALSE;
            balongfb_loge("ch_num: %d can't have scl1", ch_res.bits.ch_num);
        }

        balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_scl1 \n",ch_res.bits.ch_num);
    }

    /* scl3 */
    if (ch_res.bits.has_scl3) {
        if (OVERLAY_PIPE_ADE_CH5 == ch_res.bits.ch_num) {
            demux_list[TOP_DEMUX_SCL3] = TOP_SCL3_ON_CH5;
            pipe_cap->bit_scl3_enable   = ADE_TRUE;
        } else {
            pipe_cap->bit_scl3_enable   = ADE_FALSE;
            balongfb_loge("ch_num: %d can't have scl3", ch_res.bits.ch_num);
        }

        balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_scl3 \n",ch_res.bits.ch_num);
    }

    /* ovly */
    if (ch_res.bits.has_ovly1) {
        pipe_cap->bit_ovly1 = ADE_TRUE;
        tmp++;

        balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_ovly1 \n",ch_res.bits.ch_num);
    }

    if (ch_res.bits.has_ovly2) {
        pipe_cap->bit_ovly2 = ADE_TRUE;
        tmp++;

        balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_ovly2 \n",ch_res.bits.ch_num);
    }

    if (ch_res.bits.has_ovly3) {
        pipe_cap->bit_ovly3 = ADE_TRUE;
        tmp++;

        balongfb_logi_display_debugfs("ade_overlay_get_ch_top: ch_res.bits.ch_num:%d has_ovly3 \n",ch_res.bits.ch_num);
    }

    if (tmp > 1) {
        balongfb_loge("ch_res have too much overlay, tmp = %d", tmp);
        return -EINVAL;
    }

    /* ctrans */
    if ((OVERLAY_PIPE_ADE_CH5 == ch_res.bits.ch_num)
            || (OVERLAY_PIPE_ADE_CH6 == ch_res.bits.ch_num)) {
        pipe_cap->bit_csc_enable = 1;
    }

    return 0;
}

void ade_overlay_get_disp_top(DISP_CH_RES disp_res, u32 *demux_list,  struct ade_mux_info  *mux_list)
{

    BUG_ON((demux_list == NULL) || (mux_list == NULL));

    if (!disp_res.bits.ch_en) {
        balongfb_logi_display_debugfs("ade_overlay_get_disp_top: disp_res is not enable! \n");
        return;
    }

    /* ROT */
    if (disp_res.bits.has_720p_rot || disp_res.bits.has_1080p_rot) {
        if (mux_list[TOP_MUX_ROT_SRC].is_used) {
            balongfb_loge("mux rot src have configed, mux_src:%d", mux_list[TOP_MUX_ROT_SRC].mux_src);
        } else {
            /* demux disp, demux rot_buff, rot src */
            demux_list[TOP_DEMUX_CTRAN5] |= TOP_CTRAN5_TRANS_OVLY_DIS;

            if (disp_res.bits.has_720p_rot) {
                demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH6_ROT_720P;
            } else {
                demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_ROT_1080P;
            }

            mux_list[TOP_MUX_ROT_SRC].mux_src = TOP_ROT_SRC_RDMA;
            mux_list[TOP_MUX_ROT_SRC].is_used = ADE_TRUE;
        }
    }

    /* scl2 */
    if (disp_res.bits.has_scl2) {
        if (mux_list[TOP_MUX_SCL2_SRC].is_used) {
            balongfb_loge("mux scl2 src have configed, mux_src:%d", mux_list[TOP_MUX_SCL2_SRC].mux_src);
        } else {
            /* demux disp, scl2 src, DISP->SCL2 */
            demux_list[TOP_DEMUX_CTRAN5] |= TOP_CTRAN5_TRANS_OVLY_DIS;
            mux_list[TOP_MUX_SCL2_SRC].mux_src = TOP_SCL2_SRC_RDMA;
            mux_list[TOP_MUX_SCL2_SRC].is_used = ADE_TRUE;
        }
    }

    /* LDI */
    if (disp_res.bits.has_ldi) {
        if (mux_list[TOP_MUX_DISP_SRC].is_used) {
            balongfb_loge("mux disp src have configed, mux_src:%d", mux_list[TOP_MUX_DISP_SRC].mux_src);
        } else {
            /* disp->ldi */
            demux_list[TOP_DEMUX_CTRAN5] |= TOP_CTRAN5_TRANS_OVLY_DIS;
            mux_list[TOP_MUX_DISP_SRC].mux_src = TOP_DISP_CH_SRC_RDMA;

            mux_list[TOP_MUX_DISP_SRC].is_used = ADE_TRUE;
        }
    }

    /* wdma3 */
    if (disp_res.bits.has_wdma3) {
        if (mux_list[TOP_MUX_WDMA3_SRC].is_used) {
            balongfb_loge("mux wdma3 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA3_SRC].mux_src);
        } else {
            /* demux disp, WDMA3 src, Disp->wdma3 */
            demux_list[TOP_DEMUX_CTRAN5] |= TOP_CTRAN5_TRANS_OVLY_DIS;
            mux_list[TOP_MUX_WDMA3_SRC].mux_src = TOP_WDMA3_SRC_RDMA;

            if (disp_res.bits.has_scl3) {
                demux_list[TOP_DEMUX_SCL3]  = TOP_SCL3_ON_WIFI;
            }

            mux_list[TOP_MUX_WDMA3_SRC].is_used = ADE_TRUE;
        }
    }

    /* wdma2 */
    if (disp_res.bits.has_wdma2) {
        if (mux_list[TOP_MUX_WDMA2_SRC].is_used) {
            balongfb_loge("mux wdma2 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA2_SRC].mux_src);
        } else {
            /* demux disp, WDMA2 src, Disp->wdma2 */
            demux_list[TOP_DEMUX_CTRAN5] |= TOP_CTRAN5_TRANS_OVLY_DIS;
            mux_list[TOP_MUX_WDMA2_SRC].mux_src = TOP_WDMA2_SRC_RDMA;

            mux_list[TOP_MUX_WDMA2_SRC].is_used = ADE_TRUE;
        }
    }

    /* ovly2 or ovly3 */
    if (disp_res.bits.has_ovly2 || disp_res.bits.has_ovly3) {
        demux_list[TOP_DEMUX_CTRAN5] |= TOP_CTRAN5_TRANS_OVLY_EN;
    }
}

void ade_overlay_get_ovly1_top(OVLY1_RES ovly_res, u32 *demux_list,  struct ade_mux_info  *mux_list)
{

    BUG_ON((demux_list == NULL) || (mux_list == NULL));

    if (!ovly_res.bits.ovly_en) {
        balongfb_logi_display_debugfs("ade_overlay_get_ovly1_top: ovly1 is not enable! \n");
        return;
    }

    if (ovly_res.bits.has_scl3) {
        demux_list[TOP_DEMUX_SCL3] = TOP_SCL3_ON_WIFI;
    }

    /* WDMA1 */
    if (ovly_res.bits.has_wdma1) {
        demux_list[TOP_DEMUX_OVLY1] = TOP_OVLY1_TRANS_WDMA1_EN;
    }

    /* WDMA3 */
    if (ovly_res.bits.has_wdma3) {
        if (mux_list[TOP_MUX_WDMA3_SRC].is_used) {
            balongfb_loge("mux wdma3 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA3_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_WDMA3_SRC].mux_src = TOP_WDMA3_SRC_OVLY1;
            mux_list[TOP_MUX_WDMA3_SRC].is_used = ADE_TRUE;
        }
    }
}

void ade_overlay_get_ovly2_top(OVLY2_RES ovly_res, u32 *demux_list, struct ade_mux_info  *mux_list)
{

    BUG_ON((demux_list == NULL) || (mux_list == NULL));

    if (!ovly_res.bits.ovly_en) {
        balongfb_logi_display_debugfs("ade_overlay2_get_ovly_top: ovly is not enable! \n");
        return;
    }

    if (ovly_res.bits.has_scl3) {
        demux_list[TOP_DEMUX_SCL3] = TOP_SCL3_ON_WIFI;
    }

    if (ovly_res.bits.has_720p_rot || ovly_res.bits.has_1080p_rot) {
        if (mux_list[TOP_MUX_ROT_SRC].is_used) {
            balongfb_loge("mux ROT src have configed, mux_src:%d", mux_list[TOP_MUX_ROT_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_ROT_SRC].mux_src = TOP_ROT_SRC_OVLY2;
            mux_list[TOP_MUX_ROT_SRC].is_used = ADE_TRUE;

            if (ovly_res.bits.has_720p_rot) {
                demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH6_ROT_720P;
            } else {
                demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_ROT_1080P;
            }
        }
    }

    if (ovly_res.bits.has_scl2) {
        if (mux_list[TOP_MUX_SCL2_SRC].is_used) {
            balongfb_loge("mux SCL2 src have configed, mux_src:%d", mux_list[TOP_MUX_SCL2_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_SCL2_SRC].mux_src = TOP_SCL2_SRC_OVLY2;
            mux_list[TOP_MUX_SCL2_SRC].is_used = ADE_TRUE;
        }
    }

    /* WDMA2 */
    if (ovly_res.bits.has_wdma2) {
        if (mux_list[TOP_MUX_WDMA2_SRC].is_used) {
            balongfb_loge("mux wdma2 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA2_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_WDMA2_SRC].mux_src = TOP_WDMA2_SRC_OVLY2;
            mux_list[TOP_MUX_WDMA2_SRC].is_used = ADE_TRUE;
        }
    }

    /* WDMA3 */
    if (ovly_res.bits.has_wdma3) {
        if (mux_list[TOP_MUX_WDMA3_SRC].is_used) {
            balongfb_loge("mux wdma3 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA3_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_WDMA3_SRC].mux_src = TOP_WDMA3_SRC_OVLY2;
            mux_list[TOP_MUX_WDMA3_SRC].is_used = ADE_TRUE;
        }
    }

    /* ldi */
    if (ovly_res.bits.has_ldi) {
        if (mux_list[TOP_MUX_DISP_SRC].is_used) {
            balongfb_loge("mux sec ovly src have configed, mux_src:%d", mux_list[TOP_MUX_DISP_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_DISP_SRC].mux_src = TOP_DISP_CH_SRC_OVLY2;
            mux_list[TOP_MUX_DISP_SRC].is_used = ADE_TRUE;
        }
    }
}

void ade_overlay_get_ovly3_top(OVLY3_RES ovly_res, u32 *demux_list,  struct ade_mux_info  *mux_list)
{

    BUG_ON((demux_list == NULL) || (mux_list == NULL));

    if (!ovly_res.bits.ovly_en) {
        balongfb_logi_display_debugfs("ade_overlay_get_ovly3_top: ovly is not enable! \n");
        return;
    }

    if (ovly_res.bits.has_720p_rot || ovly_res.bits.has_1080p_rot) {
        if (mux_list[TOP_MUX_ROT_SRC].is_used) {
            balongfb_loge("mux ROT src have configed, mux_src:%d", mux_list[TOP_MUX_ROT_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_ROT_SRC].mux_src = TOP_ROT_SRC_OVLY3;
            mux_list[TOP_MUX_ROT_SRC].is_used = ADE_TRUE;

            if (ovly_res.bits.has_720p_rot) {
                demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH6_ROT_720P;
            } else {
                demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_ROT_1080P;
            }
        }
    }

    if (ovly_res.bits.has_scl2) {
        if (mux_list[TOP_MUX_SCL2_SRC].is_used) {
            balongfb_loge("mux SCL2 src have configed, mux_src:%d", mux_list[TOP_MUX_SCL2_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_SCL2_SRC].mux_src = TOP_SCL2_SRC_OVLY3;
            mux_list[TOP_MUX_SCL2_SRC].is_used = ADE_TRUE;
        }
    }

    /* WDMA2 */
    if (ovly_res.bits.has_wdma2) {
        if (mux_list[TOP_MUX_WDMA2_SRC].is_used) {
            balongfb_loge("mux wdma2 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA2_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_WDMA2_SRC].mux_src = TOP_WDMA2_SRC_OVLY3;
            mux_list[TOP_MUX_WDMA2_SRC].is_used = ADE_TRUE;
        }
    }

    /* ovly1, ovly2 */
    if (ovly_res.bits.has_ovly1 || ovly_res.bits.has_ovly2) {
        if (mux_list[TOP_MUX_SEC_OVLY_SRC].is_used) {
            balongfb_loge("mux sec ovly src have configed, mux_src:%d", mux_list[TOP_MUX_SEC_OVLY_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_SEC_OVLY_SRC].mux_src = TOP_SEC_OVLY_SRC_OVLY3;
            mux_list[TOP_MUX_SEC_OVLY_SRC].is_used = ADE_TRUE;
        }
    }
}

void ade_overlay_get_rot_top(ROT_RES rot_res, u32 *demux_list,  struct ade_mux_info  *mux_list)
{

    BUG_ON((demux_list == NULL) || (mux_list == NULL));

    if (!rot_res.bits.rot_en) {
        balongfb_logi_display_debugfs("ade_overlay_get_rot_top: rot is not enable! \n");
        return;
    }

    if (rot_res.bits.has_scl3) {
        demux_list[TOP_DEMUX_SCL3] = TOP_SCL3_ON_WIFI;
    }

    if (rot_res.bits.has_scl2) {
        if (mux_list[TOP_MUX_SCL2_SRC].is_used) {
            balongfb_loge("mux SCL2 src have configed, mux_src:%d", mux_list[TOP_MUX_SCL2_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_SCL2_SRC].mux_src = TOP_SCL2_SRC_ROT;
            mux_list[TOP_MUX_SCL2_SRC].is_used = ADE_TRUE;
        }
    }

    /* WDMA2 */
    if (rot_res.bits.has_wdma2) {
        if (mux_list[TOP_MUX_WDMA2_SRC].is_used) {
            balongfb_loge("mux wdma2 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA2_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_WDMA2_SRC].mux_src = TOP_WDMA2_SRC_ROT;
            mux_list[TOP_MUX_WDMA2_SRC].is_used = ADE_TRUE;
        }
    }

    /* WDMA3 */
    if (rot_res.bits.has_wdma3) {
        if (mux_list[TOP_MUX_WDMA3_SRC].is_used) {
            balongfb_loge("mux wdma3 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA3_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_WDMA3_SRC].mux_src = TOP_WDMA3_SRC_ROT;
            mux_list[TOP_MUX_WDMA3_SRC].is_used = ADE_TRUE;
        }
    }

    /* ldi */
    if (rot_res.bits.has_ldi) {
        if (mux_list[TOP_MUX_DISP_SRC].is_used) {
            balongfb_loge("mux sec ovly src have configed, mux_src:%d", mux_list[TOP_MUX_DISP_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_DISP_SRC].mux_src = TOP_DISP_CH_SRC_ROT;
            mux_list[TOP_MUX_DISP_SRC].is_used = ADE_TRUE;
        }
    }

    /* ovly1, ovly2 */
    if (rot_res.bits.has_ovly1 || rot_res.bits.has_ovly2) {
        if (mux_list[TOP_MUX_SEC_OVLY_SRC].is_used) {
            balongfb_loge("mux sec ovly src have configed, mux_src:%d", mux_list[TOP_MUX_SEC_OVLY_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_SEC_OVLY_SRC].mux_src = TOP_SEC_OVLY_SRC_ROT;
            mux_list[TOP_MUX_SEC_OVLY_SRC].is_used = ADE_TRUE;
        }
    }
}

void ade_overlay_get_scl2_top(SCL2_RES scl_res, u32 *demux_list,  struct ade_mux_info  *mux_list)
{

    BUG_ON((demux_list == NULL) || (mux_list == NULL));

    if (!scl_res.bits.scl2_en) {
        balongfb_logi_display_debugfs("ade_overlay_get_scl2_top: scl2 is not enable! \n");
        return;
    }

    if (scl_res.bits.has_scl3) {
        demux_list[TOP_DEMUX_SCL3] = TOP_SCL3_ON_WIFI;
    }

    /* WDMA2 */
    if (scl_res.bits.has_wdma2) {
        if (mux_list[TOP_MUX_WDMA2_SRC].is_used) {
            balongfb_loge("mux wdma2 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA2_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_WDMA2_SRC].mux_src = TOP_WDMA2_SRC_SCL2;
            mux_list[TOP_MUX_WDMA2_SRC].is_used = ADE_TRUE;
        }
    }

    /* WDMA3 */
    if (scl_res.bits.has_wdma3) {
        if (mux_list[TOP_MUX_WDMA3_SRC].is_used) {
            balongfb_loge("mux wdma3 src have configed, mux_src:%d", mux_list[TOP_MUX_WDMA3_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_WDMA3_SRC].mux_src = TOP_WDMA3_SRC_SCL2;
            mux_list[TOP_MUX_WDMA3_SRC].is_used = ADE_TRUE;
        }
    }

    /* ldi */
    if (scl_res.bits.has_ldi) {
        if (mux_list[TOP_MUX_DISP_SRC].is_used) {
            balongfb_loge("mux sec ovly src have configed, mux_src:%d", mux_list[TOP_MUX_DISP_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_DISP_SRC].mux_src = TOP_DISP_CH_SRC_SCL2;
            mux_list[TOP_MUX_DISP_SRC].is_used = ADE_TRUE;
        }
    }

    /* ovly1, ovly2 */
    if (scl_res.bits.has_ovly1 || scl_res.bits.has_ovly2) {
        if (mux_list[TOP_MUX_SEC_OVLY_SRC].is_used) {
            balongfb_loge("mux sec ovly src have configed, mux_src:%d", mux_list[TOP_MUX_SEC_OVLY_SRC].mux_src);
        } else {
            mux_list[TOP_MUX_SEC_OVLY_SRC].mux_src = TOP_SEC_OVLY_SRC_SCL2;
            mux_list[TOP_MUX_SEC_OVLY_SRC].is_used = ADE_TRUE;
        }
    }
}

void ade_overlay_res_top_info_cfg(struct ade_compose_data_type *ade_pri_data, u32 *demux_list,  struct ade_mux_info  *mux_list)
{
    u8* ade_base;
    u32 i = 0;

    BUG_ON((demux_list == NULL) || (mux_list == NULL) || (ade_pri_data == NULL));

    ade_base = ade_pri_data->ade_base;

    ade_pri_data->soft_rst0 = 0;
    ade_pri_data->soft_rst1 = 0;
    ade_pri_data->reload_dis0 = 0;
    ade_pri_data->reload_dis1 = 0;

    ade_pri_data->top_ovly_ctrl = inp32(ade_base + ADE_OVLY_CTL_REG);
    for (i = 0; i < 8; i++) {
        if (((ade_pri_data->top_ovly_ctrl >> (i * 4)) & 0x3) == 0x1) {
            ade_pri_data->top_ovly_ctrl &= ~(1 << (i * 4));
        }
    }

    set_TOP_ADE_OVLY_CTRL(ade_base, 0);

    if (TOP_DISP_CH_SRC_RDMA == mux_list[TOP_MUX_DISP_SRC].mux_src) {
        /* set ovly ctrl = 1,for next step CSC of virtual disp,
         * don't need wait for resource switch cmpl intrrepute.
         */
        set_TOP_ADE_OVLY_CTRL(ade_base, 1);
    }

    /* enable dfs buf */
    if(PANEL_MIPI_VIDEO == ade_pri_data->lcd_type) {
        set_TOP_CTL_dfs_buf_cfg(ade_base, 0);
    }

    if ((!ade_pri_data->overlay_ctl.ade_res_info.disp_ch_res.bits.ch_en)
     || (ade_pri_data->overlay_ctl.ade_res_info.comp_mode != OVERLAY_COMP_TYPE_ONLINE)) {
        /* TOP_DEMUX_ROT_BUFF */
        balongfb_logi_top_info("ade_overlay_top_info_cfg: demux_list[TOP_DEMUX_ROT_BUFF] = 0x%x \n", demux_list[TOP_DEMUX_ROT_BUFF]);
        set_TOP_CTL_rot_buf_cfg(ade_base, demux_list[TOP_DEMUX_ROT_BUFF]);

    #if ADE_MEM_SHARE_EN
        balongfb_logi_top_info("ade_overlay_top_info_cfg: demux_list[TOP_DEMUX_ROT_BUF_SHR_OUT] = 0x%x \n", demux_list[TOP_DEMUX_ROT_BUF_SHR_OUT]);
        set_TOP_CTL1_rot_buf_shr_out(ade_base, demux_list[TOP_DEMUX_ROT_BUF_SHR_OUT]);
    #endif
    }
    /* TOP_DEMUX_SCL3 */
    balongfb_logi_top_info("ade_overlay_top_info_cfg: demux_list[TOP_DEMUX_SCL3] = 0x%x \n", demux_list[TOP_DEMUX_SCL3]);
    set_TOP_SCL3_MUX_CFG_scl3_mux(ade_base, demux_list[TOP_DEMUX_SCL3]);

    /* TOP_DEMUX_SCL1 */
    balongfb_logi_top_info("ade_overlay_top_info_cfg: demux_list[TOP_DEMUX_SCL1] = 0x%x \n", demux_list[TOP_DEMUX_SCL1]);
    set_TOP_SCL1_MUX_CFG(ade_base, demux_list[TOP_DEMUX_SCL1]);

    /* TOP_MUX_ROT_SRC */
    balongfb_logi_top_info("ade_overlay_get_top_info: mux_list[TOP_MUX_ROT_SRC] = 0x%x \n", mux_list[TOP_MUX_ROT_SRC].mux_src);
    set_TOP_ROT_SRC_CFG(ade_base, mux_list[TOP_MUX_ROT_SRC].mux_src);

    /* TOP_MUX_SCL2_SRC  */
    balongfb_logi_top_info("ade_overlay_get_top_info: mux_list[TOP_MUX_SCL2_SRC] = 0x%x \n", mux_list[TOP_MUX_SCL2_SRC].mux_src);
    set_TOP_SCL2_SRC_CFG(ade_base, mux_list[TOP_MUX_SCL2_SRC].mux_src);

    /* TOP_MUX_DISP_SRC  */
    set_TOP_DISP_SRC_CFG(ade_base, mux_list[TOP_MUX_DISP_SRC].mux_src);

    /* TOP_MUX_WDMA2_SRC  */
    balongfb_logi_top_info("ade_overlay_get_top_info: mux_list[TOP_MUX_WDMA2_SRC] = 0x%x \n", mux_list[TOP_MUX_WDMA2_SRC].mux_src);
    set_TOP_WDMA2_SRC_CFG(ade_base, mux_list[TOP_MUX_WDMA2_SRC].mux_src);

    /* TOP_MUX_SEC_OVLY_SRC  */
    balongfb_logi_top_info("ade_overlay_get_top_info: mux_list[TOP_MUX_SEC_OVLY_SRC] = 0x%x \n", mux_list[TOP_MUX_SEC_OVLY_SRC].mux_src);
    set_TOP_SEC_OVLY_SRC_CFG(ade_base, mux_list[TOP_MUX_SEC_OVLY_SRC].mux_src);

    /* TOP_MUX_WDMA3_SRC  */
    balongfb_logi_top_info("ade_overlay_get_top_info: mux_list[TOP_MUX_WDMA3_SRC] = 0x%x \n", mux_list[TOP_MUX_WDMA3_SRC].mux_src);
    set_TOP_WDMA3_SRC_CFG(ade_base, mux_list[TOP_MUX_WDMA3_SRC].mux_src);

    /* TOP_DEMUX_OVLY1 */
    balongfb_logi_top_info("ade_overlay_top_info_cfg: demux_list[TOP_DEMUX_OVLY1] = 0x%x \n", demux_list[TOP_DEMUX_OVLY1]);
    set_TOP_OVLY1_TRANS_CFG(ade_base, demux_list[TOP_DEMUX_OVLY1]);

    /* TOP_DEMUX_DISP  */
    balongfb_logi_top_info("ade_overlay_top_info_cfg: demux_list[TOP_DEMUX_CTRAN5] = 0x%x \n", demux_list[TOP_DEMUX_CTRAN5]);
    set_TOP_CTRAN5_TRANS_CFG(ade_base, demux_list[TOP_DEMUX_CTRAN5]);
}

int ade_overlay_res_get_cmdbuff_index(OVERLAY_COMP_TYPE_UINT32 comp_mode, struct cmdqueue_buff *cmdq_list)
{
    struct  cmdfile_buffer   *cf_list;
    int     idx = 0;

    BUG_ON(cmdq_list == NULL);

    if (comp_mode >= OVERLAY_COMP_TYPE_INVAL) {
        balongfb_loge("comp_mode %d is not right ! ", comp_mode);
        return -EINVAL;
    }

    /* get idle cmd buffer,
     * if the mode is offline, alos need online cmdqueue to config disp_rdma
     */
    if (OVERLAY_COMP_TYPE_COPYBIT != comp_mode) {
        idx = cmdq_list[ADE_CMDQ_TYPE_ONLINE].current_idx;
        cf_list = &(cmdq_list[ADE_CMDQ_TYPE_ONLINE].cf_list[idx]);
        cf_list->is_busy = ADE_FALSE;

        idx += 1;
        idx &= 0x1;

        cf_list = &(cmdq_list[ADE_CMDQ_TYPE_ONLINE].cf_list[idx]);
        cf_list->is_busy = ADE_TRUE;
        cf_list->cmd_len = 0;
        cmdq_list[ADE_CMDQ_TYPE_ONLINE].current_idx = idx;

        balongfb_logi_display_debugfs("ade_overlay_res_get_cmdbuff_index:current_idx =%d,cf_list->is_busy = %d \n", idx, cf_list->is_busy);
    }

    if (OVERLAY_COMP_TYPE_ONLINE != comp_mode) {
        idx = cmdq_list[ADE_CMDQ_TYPE_OFFLINE].current_idx;
        cf_list = &(cmdq_list[ADE_CMDQ_TYPE_OFFLINE].cf_list[idx]);
        cf_list->is_busy = ADE_FALSE;

        idx += 1;
        idx &= 0x1;

        cf_list = &(cmdq_list[ADE_CMDQ_TYPE_OFFLINE].cf_list[idx]);
        cf_list->is_busy = ADE_TRUE;
        cf_list->cmd_len = 0;
        cmdq_list[ADE_CMDQ_TYPE_OFFLINE].current_idx = idx;

        balongfb_logi_display_debugfs("ade_overlay_res_get_cmdbuff_index:offline current_idx =%d,cf_list->is_busy = %d \n", idx, cf_list->is_busy);
    }

    return 0;
}

int ade_overlay_res_get_top_info(struct  ade_overlay_ctrl  *overlay_ctl)
{
    struct overlay_resource_info   *ade_res_info;
    struct ade_mux_info            *mux_list;
    struct ade_overlay_pipe        *pipe;
    struct ade_overlay_ovly        *ovly;
    CH_RES      ch_res;
    u32         *demux_list;
    int         i = 0;
    int         ret = 0;

    BUG_ON(overlay_ctl == NULL);

    ade_res_info = &overlay_ctl->ade_res_info;
    demux_list   = overlay_ctl->ade_demux_list;
    mux_list     = overlay_ctl->ade_mux_list;

    memset(demux_list, 0, sizeof(overlay_ctl->ade_demux_list));
    memset(mux_list, 0, sizeof(overlay_ctl->ade_mux_list));

    for (i = 0; i < ADE_COMPOSE_CH_MAX; i++ ) {
        pipe = &(overlay_ctl->pipe_list[i]);
        memset(&pipe->output_rect, 0, sizeof(pipe->output_rect));
        pipe->need_cfg_rot_dma = ADE_FALSE;
        pipe->need_cfg_scl2    = ADE_FALSE;
        pipe->shadow_reg_count = 0;

        ch_res.ul32 = ade_res_info->compose_ch_res[i].ul32;
        ret = ade_overlay_get_ch_top(ch_res, demux_list, &pipe->pipe_cap);

        if (ret != 0) {
            balongfb_loge("ade_overlay_get_ch_top: get ch top error ch = %d", i);
            return -EINVAL;
        }

        /* when ch->ovly3->rot,or ch->ovly2->rot, must have to config ch RDMA partial */
        if (ch_res.bits.has_ovly2) {
            if (ade_res_info->ovly2_res.bits.has_720p_rot || ade_res_info->ovly2_res.bits.has_1080p_rot) {
                if (ade_res_info->rot_res.bits.rot_en) {
                    pipe->need_cfg_rot_dma = ADE_TRUE;
                }
            }

            if (ade_res_info->ovly2_res.bits.has_scl2) {
                if (ade_res_info->scl2_res.bits.scl2_en) {
                    pipe->need_cfg_scl2 = ADE_TRUE;
                }
            }
        }

        if (ch_res.bits.has_ovly3) {
            if (ade_res_info->ovly3_res.bits.has_720p_rot || ade_res_info->ovly3_res.bits.has_1080p_rot) {
                if (ade_res_info->rot_res.bits.rot_en) {
                    pipe->need_cfg_rot_dma = ADE_TRUE;

                    if (ade_res_info->rot_res.bits.has_scl2
                            && ade_res_info->scl2_res.bits.scl2_en) {
                        pipe->need_cfg_scl2 = ADE_TRUE;
                    }
                }
            }

            if (ade_res_info->ovly3_res.bits.has_scl2) {
                if (ade_res_info->scl2_res.bits.scl2_en) {
                    pipe->need_cfg_scl2 = ADE_TRUE;
                }
            }
        }
    }

    /* get disp ch demux and mux */
    ade_overlay_get_disp_top(ade_res_info->disp_ch_res, demux_list, mux_list);

    /* get ovly1 demux and mux */
    ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_1]);
    ovly->ovly_cap = ade_res_info->ovly1_res.ul32;
    ovly->ovly_ch_shadow_count = 0;
    memset(&ovly->ovly_comp, 0, sizeof(struct ovly_info));
    ade_overlay_get_ovly1_top(ade_res_info->ovly1_res, demux_list, mux_list);

    /* get ovly2 demux and mux */
    ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_2]);
    ovly->ovly_cap = ade_res_info->ovly2_res.ul32;
    memset(&ovly->ovly_comp, 0, sizeof(struct ovly_info));
    ovly->ovly_ch_shadow_count = 0;
    ade_overlay_get_ovly2_top(ade_res_info->ovly2_res, demux_list, mux_list);

    /* get ovly3 demux and mux */
    ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_3]);
    ovly->ovly_cap = ade_res_info->ovly3_res.ul32;
    memset(&ovly->ovly_comp, 0, sizeof(struct ovly_info));
    ovly->ovly_ch_shadow_count = 0;
    ade_overlay_get_ovly3_top(ade_res_info->ovly3_res, demux_list, mux_list);

    /* get ROT demux and mux */
    ade_overlay_get_rot_top(ade_res_info->rot_res, demux_list, mux_list);

    /* get SCL2 demux and mux */
    ade_overlay_get_scl2_top(ade_res_info->scl2_res, demux_list, mux_list);

    /* set scl1 and scl3 default value */
    if (TOP_SCL1_ON_CH1 == demux_list[TOP_DEMUX_SCL1]) {
        overlay_ctl->pipe_list[OVERLAY_PIPE_ADE_CH1].pipe_cap.bit_scl1_enable = ADE_TRUE;
    } else if (TOP_SCL1_ON_CH5 == demux_list[TOP_DEMUX_SCL1]) {
        overlay_ctl->pipe_list[OVERLAY_PIPE_ADE_CH5].pipe_cap.bit_scl1_enable = ADE_TRUE;
    } else {
        overlay_ctl->pipe_list[OVERLAY_PIPE_ADE_CH6].pipe_cap.bit_scl1_enable = ADE_TRUE;
    }

    if (TOP_SCL3_ON_CH5 == demux_list[TOP_DEMUX_SCL3]) {
        overlay_ctl->pipe_list[OVERLAY_PIPE_ADE_CH5].pipe_cap.bit_scl3_enable = ADE_TRUE;
    }

    /* init rot_info, for ROT cfg */
    memset(&overlay_ctl->rot_info, 0, sizeof(overlay_ctl->rot_info));
    if (ade_res_info->rot_res.bits.rot_en) {
        overlay_ctl->rot_info.rot_en = ADE_TRUE;
        if (demux_list[TOP_DEMUX_ROT_BUFF] == TOP_ROT_BUFF_ROT_1080P) {
            overlay_ctl->rot_info.rot_buff = TOP_ROT_BUFF_ROT_1080p;
        }else if (demux_list[TOP_DEMUX_ROT_BUFF] == TOP_ROT_BUFF_CH6_ROT_720P) {
            overlay_ctl->rot_info.rot_buff = TOP_ROT_BUFF_ROT_720p;
        } else {
            overlay_ctl->rot_info.rot_en = ADE_FALSE;
            overlay_ctl->rot_info.rot_buff = TOP_ROT_BUFF_ROT_NULL;
        }
    }

    /* init scl2_info, for scl2 cfg */
    memset(&overlay_ctl->scl2_info, 0, sizeof(overlay_ctl->scl2_info));
    if (ade_res_info->scl2_res.bits.scl2_en) {
        overlay_ctl->scl2_info.scl2_en = ADE_TRUE;
    }

    /* if disp_ch->scl2,need config scl2 at commit */
    if (TOP_SCL2_SRC_RDMA == mux_list[TOP_MUX_SCL2_SRC].mux_src) {
        overlay_ctl->scl2_info.scl2_state = ADE_SCL2_STATE_WAIT_CFG;
    }

    balongfb_logi_top_info("ade_overlay_res_get_top_info:overlay_ctl->scl2_info.scl2_state=%d\n ", overlay_ctl->scl2_info.scl2_state);

    return 0;
}
/*****************  TOP CFG END ************************************************/
void ade_overlay_region_set_block_rot(struct ade_overlay_ctrl *overlay_ctl, struct ade_overlay_pipe *pipe, struct cmdfile_buffer *cf_buff)
{
    struct ade_mux_info      *w3_ade_mux;

    BUG_ON((pipe == NULL) || (cf_buff == NULL) || (overlay_ctl == NULL));

    /*
     * check TOP config, block rot must be (ch1/ch6)-> ovly1 -> wdma3
     */
    if (!pipe->pipe_cap.bit_block_rot_enable) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_block_rot: block rot disable \n");
        return;
    }

    if (!pipe->pipe_cap.bit_ovly1) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_block_rot: bit_ovly1 disable \n");
        return;
    }

    w3_ade_mux = &(overlay_ctl->ade_mux_list[TOP_MUX_WDMA3_SRC]);
    if (!w3_ade_mux->is_used || (TOP_WDMA3_SRC_OVLY1 != w3_ade_mux->mux_src)) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_block_rot: dwma3 src mux is not ovly1, set to ovly1 \n");
        set_TOP_WDMA3_SRC_CFG(pipe->ade_base, TOP_WDMA3_SRC_OVLY1);
    }

    /*
     * generate cmd for dma, clip, ctran, scl, ovly1, wdma3,
     * the most complicated case like this:
     *
     * **************************************
     * *                             |      *
     * *                             |      *
     * *                             |      *
     * *                             |      *
     * *                             |      *
     * *                             |      *
     * *                             |      *
     * *                             |      *
     * *------------------------------------*
     * *                             |      *
     * **************************************
     */

    /* cut and calculate the addr, size for each block
     * if the w and h can be cut average and align, it will be easy
     */

    return;
}


/*****************  REGION CFG BEGIN *******************************************/
void ade_overlay_region_set_partial_rdma (struct overlay_region_info *pipe_info, struct  ch_unit_reg *unit_reg, struct cmdfile_buffer *cf_buff, u8 rot_buff)
{
    u32     max_buff = 0;
    u32     uv_max_buff = 0;
    u32     align_byte;
    u32     partial_stride;
    u32     partial_height;
    u32     uv_partial_stride = 0;
    u32     uv_partial_height = 0;
    u32     enable_h = 0;
    u32     layer_stride;
    u32     extra_stride = 0;
    u32     partial_n = 0;

    BUG_ON((pipe_info == NULL) || (cf_buff == NULL) || (unit_reg == NULL));

    if (0 == pipe_info->rotation) {
        return;
    }

    ade_overlay_get_dma_rot_buff(rot_buff, pipe_info, &max_buff, &uv_max_buff, &enable_h);

    if (0 == max_buff) {
        return;
    }

    if ((ADE_BGR_888 == pipe_info->format) || (ADE_RGB_888 == pipe_info->format)
            || (ADE_YUV444 == pipe_info->format)) {
        align_byte  = 12;
    } else {
        align_byte  = 16;
    }

    layer_stride = pipe_info->width * ade_overlay_format2bpp(pipe_info->format);
    if ((layer_stride * pipe_info->height) <= max_buff) {
        partial_stride = layer_stride;
        partial_height = pipe_info->height;
    } else {
        /* 180 rot */
        if (1 == enable_h) {
            partial_stride =layer_stride;// pipe_info->stride;
            partial_height =max_buff / layer_stride;   //max_buff / pipe_info->stride;
            while (pipe_info->height % partial_height) {
                partial_height--;
            }
        } else {
            partial_n = max_buff / (pipe_info->height * align_byte);
            partial_stride = align_byte * partial_n;
            partial_height = pipe_info->height;
            extra_stride = layer_stride % (align_byte * partial_n);
        }
    }

    if (uv_max_buff > 0) {
        uv_partial_stride = partial_stride;
        uv_partial_height = partial_height / 2;

        while ((pipe_info->height / 2) % uv_partial_height) {
            uv_partial_height--;
        }

        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_partial_size, (uv_partial_height << 16) | uv_partial_stride);
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_partial_space, uv_partial_height * pipe_info->stride);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    }

    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_partial_size, (partial_height << 16) | partial_stride);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_partial_space, partial_height * pipe_info->stride);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_extra_partial_width, (extra_stride << 16) | extra_stride);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    balongfb_logi_display_debugfs("ade_overlay_region_set_partial_rdma: max_buff = %d, partial_height = %d,  partial_stride = %d \n", max_buff, partial_height,partial_stride);
    balongfb_logi_display_debugfs("ade_overlay_region_set_partial_rdma: max_buff = %d, uv_partial_height = %d,  uv_partial_stride = %d \n", max_buff, uv_partial_height,uv_partial_stride);
    balongfb_logi_display_debugfs("ade_overlay_region_set_partial_rdma: extra_stride = %d, uv_extra_stride = %d \n", extra_stride, extra_stride);
}

static void ade_overlay_region_set_rdma_cmd(struct ade_compose_data_type *ade_pri_data, struct ade_overlay_pipe *pipe, struct ade_overlay_rot *rot_info, struct cmdfile_buffer *cf_buff)
{
    struct  ch_unit_reg      *unit_reg;
    struct  overlay_region_info *pipe_info;
    struct  ade_rect         *orect;
    struct  ade_rect         irect;
    u8*     ade_base;
    u8      rot_buff = TOP_ROT_BUFF_ROT_NULL;

    BUG_ON((pipe == NULL) || (cf_buff == NULL) || (rot_info == NULL));

    unit_reg  = &(pipe->pipe_reg);
    pipe_info = &(pipe->pipe_info);
    orect     = &(pipe->output_rect);
    ade_base  = pipe->ade_base;

    if ((pipe_info->phy_addr == 0) || (pipe_info->height == 0) || (pipe_info->stride == 0)) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_rdma_cmd: phy_addr or heigh or stride is 0 \n");
        return;
    }

    if ((pipe_info->ch_num < OVERLAY_PIPE_ADE_CH5) && (pipe_info->format > ADE_BGR_888)) {
        balongfb_loge("ade_overlay_region_set_rdma_cmd: the channel %d don't support the format %d \n",pipe_info->ch_num,pipe_info->format);
        return;
    }

    /* for DMA, the size is the surface size, not the region size */
    orect->x = pipe_info->dst_rect.x;
    orect->y = pipe_info->dst_rect.y;
    orect->w = pipe_info->width;
    orect->h = pipe_info->height;

    memcpy(&irect, orect, sizeof(struct ade_rect));

    /* get output size */
    ade_overlay_get_rect_from_rot(pipe_info->rotation, &irect, orect);
    balongfb_loge_display_debugfs("ade_overlay_region_set_rdma_cmd: pipe_info->phy_addr = 0x%x,ch_type = %d \n", pipe_info->phy_addr,pipe_info->ch_type);
    balongfb_logi_display_debugfs("ade_overlay_region_set_rdma_cmd: output_rect.x = %d,output_rect.y=%d \n", orect->x,orect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_rdma_cmd: output_rect.w = %d,output_rect.h=%d \n", orect->w,orect->h);

    /* set ADE TOP: burst length, qos, axi */
    set_TOP_DMA_AXI_MUX(ade_base, pipe_info->ch_num, pipe_info->ch_type);

    ade_cmdq_wr_rdma_pe_cmd(unit_reg->ch_rdma_ch_pe, pipe_info->ch_type, pipe_info->rotation);
    if (pipe_info->rotation > 0) {
        ade_cmdq_wr_rdma_ctrl_rot_cmd(unit_reg->ch_rdma_ch_ctrl, pipe_info->format, pipe_info->rotation);  /* CTRL */
    } else {
        ade_cmdq_wr_rdma_ctrl_cmd(unit_reg->ch_rdma_ch_ctrl, pipe_info);
    }

    /* Y or normal */
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_addr, pipe_info->phy_addr);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_size, pipe_info->height << 16 | pipe_info->width * ade_overlay_format2bpp(pipe_info->format));
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_stride, pipe_info->stride); /* stride dir for mirror */
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_space, pipe_info->stride * pipe_info->height);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* UV */
    if ((ADE_NV12 == pipe_info->format) || (ADE_NV21 == pipe_info->format)) {
        /* UV */
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_addr, pipe_info->uv_addr);
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_size, ((pipe_info->height / 2) << 16)| (pipe_info->width * ade_overlay_format2bpp(pipe_info->format)));
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_stride, pipe_info->stride);
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_space, (pipe_info->height /2 ) * pipe_info->stride);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    }

    /* partial spec is not used */
    if (ADE_TRUE == pipe->need_cfg_rot_dma) {
        rot_buff = rot_info->rot_buff;
    }

    /* 1080p buff, 153.6k */
    if (pipe->pipe_cap.bit_rot_enable) {
        if (pipe->pipe_cap.bit_rot_1080p) {
            rot_buff = TOP_ROT_BUFF_ROT_1080p;
        } else if (pipe->pipe_cap.bit_rot_720p) {
            rot_buff = TOP_ROT_BUFF_ROT_720p;
        } else {
            rot_buff = TOP_ROT_BUFF_ROT_NULL;
        }
    }

    /* set partial for DMA ROTATION */
    ade_overlay_region_set_partial_rdma(pipe_info, unit_reg, cf_buff, rot_buff);

    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_en, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    if (ade_pri_data != NULL) {
        /* lock the display buffer */
        ade_overlay_handle_lock(ade_pri_data, pipe_info);
        if (OVERLAY_PIPE_TYPE_ONLINE == pipe_info->ch_type) {
            set_soft_rst_sel_bit(ade_pri_data->soft_rst0, unit_reg->rdma_rst_sel_bit, ade_pri_data->lcd_type);
            set_reload_dis_bit(ade_pri_data->reload_dis0, unit_reg->rdma_rst_sel_bit);
            balongfb_logi_display_debugfs("ade_overlay_region_set_rdma_cmd: rdma_rst_sel_bit=%d\n", unit_reg->rdma_rst_sel_bit);
        }
    }
}
static void ade_overlay_region_set_clip_cmd(struct ade_compose_data_type *ade_pri_data, struct ade_overlay_pipe *pipe, struct cmdfile_buffer *cf_buff)
{
    struct  ch_unit_reg      *unit_reg;
    struct  overlay_region_info *pipe_info;
    struct  ade_rect         *clip_orect;
    u8* ade_base;

    BUG_ON((pipe == NULL) || (cf_buff == NULL));

    unit_reg  = &(pipe->pipe_reg);
    pipe_info = &(pipe->pipe_info);
    clip_orect = &(pipe->output_rect);
    ade_base = pipe->ade_base;

    if ((pipe_info->phy_addr == 0) || (pipe_info->height == 0) || (pipe_info->stride == 0)) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_clip_cmd: phy_addr or heigh or stride is 0, don't need config clip! \n");
        return;
    }

    if ((unsigned)(pipe_info->clip_left + pipe_info->clip_right) > clip_orect->w) {
        pipe_info->clip_left  = 0;
        pipe_info->clip_right = 0;
    }

    if ((unsigned)(pipe_info->clip_top + pipe_info->clip_bottom) > clip_orect->h) {
        pipe_info->clip_top = 0;
        pipe_info->clip_bottom = 0;
    }

    /* one wr cmd */
    if (pipe_info->clip_left == 0 && pipe_info->clip_right == 0 && pipe_info->clip_top == 0 && pipe_info->clip_bottom == 0) {
        ade_cmdq_wr_cmd(unit_reg->clip_disable, 1);
    } else {
        ade_cmdq_wr_cmd(unit_reg->clip_disable, 0);
    }

    ade_cmdq_wr_cmd(unit_reg->clip_size0, (clip_orect->w - 1) << 16 | (clip_orect->h - 1));
    ade_cmdq_wr_cmd(unit_reg->clip_size1, pipe_info->clip_left << 16 | pipe_info->clip_right);
    ade_cmdq_wr_cmd(unit_reg->clip_size2, pipe_info->clip_top << 16 | pipe_info->clip_bottom);

    ade_cmdq_wr_cmd(unit_reg->clip_cfg_ok, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* clip output rect, be used to next unit, such as scl, ctran */
    clip_orect->w = clip_orect->w - pipe_info->clip_left - pipe_info->clip_right;
    clip_orect->h = clip_orect->h - pipe_info->clip_top - pipe_info->clip_bottom;

    balongfb_logi_display_debugfs("ade_overlay_region_set_clip_cmd: output_rect.x = %d,output_rect.y=%d \n", clip_orect->x,clip_orect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_clip_cmd: output_rect.w = %d,output_rect.h=%d \n", clip_orect->w,clip_orect->h);

    if (OVERLAY_PIPE_TYPE_ONLINE == pipe_info->ch_type) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst0, unit_reg->clip_rst_sel_bit, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis0, unit_reg->clip_rst_sel_bit);

        balongfb_logi_display_debugfs("ade_overlay_region_set_clip_cmd: ade_pri_data->soft_rst0=0x%x\n", ade_pri_data->soft_rst0);
    }
}
static void ade_overlay_region_set_scl_cmd(struct ade_compose_data_type *ade_pri_data, struct ade_overlay_pipe *pipe, struct cmdfile_buffer *cf_buff)
{
    struct  ch_unit_reg      *unit_reg;
    struct  overlay_region_info *pipe_info;
    struct  ade_rect         *scl_irect;
    struct  ade_rect         scl_orect;
    struct  ade_rect         scl_uv_irect;
    int scl_iformat;

    BUG_ON((pipe == NULL) || (cf_buff == NULL));

    unit_reg  = &(pipe->pipe_reg);
    pipe_info = &(pipe->pipe_info);
    scl_irect = &(pipe->output_rect);

    if (!pipe->pipe_cap.bit_scl1_enable && !pipe->pipe_cap.bit_scl3_enable) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_scl_cmd: ch %d,scl is disable \n",pipe->pipe_num);
        return;
    }

    if ((pipe_info->phy_addr == 0) || (pipe_info->height == 0) || (pipe_info->stride == 0)) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_scl_cmd: phy_addr or heigh or stride is 0, don't need config scl! \n");
        return;
    }

    if (pipe_info->ch_num == OVERLAY_PIPE_ADE_CH5) {
        if (pipe->pipe_cap.bit_scl1_enable) {
            unit_reg->scl_ctrl = ADE_SCL1_CTRL_REG;
            unit_reg->scl_hsp  = ADE_SCL1_HSP_REG;
            unit_reg->scl_uv_hsp = ADE_SCL1_UV_HSP_REG;
            unit_reg->scl_vsp  = ADE_SCL1_VSP_REG;
            unit_reg->scl_uv_vsp = ADE_SCL1_UV_VSP_REG;
            unit_reg->scl_ires = ADE_SCL1_IRES_REG;
            unit_reg->scl_ores = ADE_SCL1_ORES_REG;
            unit_reg->scl_hcoef = ADE_SCL1_HCOEF_0_REG;
            unit_reg->scl_vcoef = ADE_SCL1_VCOEF_0_REG;
            unit_reg->scl_start = ADE_SCL1_START_REG;
            if (HI6220_CHIPSET == get_chipset_type()) {
                unit_reg->scl_pix_ofst  = ADE_SCL1_PIX_OFST;
                unit_reg->scl_uv_pix_ofst = ADE_SCL1_UV_PIX_OFST;
            }
            unit_reg->scl_rst_sel_bit = 21;
        } else if (pipe->pipe_cap.bit_scl3_enable) {
            unit_reg->scl_ctrl = ADE_SCL3_CTRL_REG;
            unit_reg->scl_hsp  = ADE_SCL3_HSP_REG;
            unit_reg->scl_uv_hsp = ADE_SCL3_UV_HSP_REG;
            unit_reg->scl_vsp  = ADE_SCL3_VSP_REG;
            unit_reg->scl_uv_vsp = ADE_SCL3_UV_VSP_REG;
            unit_reg->scl_ires = ADE_SCL3_IRES_REG;
            unit_reg->scl_ores = ADE_SCL3_ORES_REG;
            unit_reg->scl_hcoef = ADE_SCL3_HCOEF_0_REG;
            unit_reg->scl_vcoef = ADE_SCL3_VCOEF_0_REG;
            unit_reg->scl_start = ADE_SCL3_START_REG;
            if (HI6220_CHIPSET == get_chipset_type()) {
                unit_reg->scl_pix_ofst  = ADE_SCL3_PIX_OFST;
                unit_reg->scl_uv_pix_ofst = ADE_SCL3_UV_PIX_OFST;
            }
            unit_reg->scl_rst_sel_bit = 23;
        }
    }

    scl_iformat = getSclImageFormatType(pipe_info->format);

    /* bypass */
    if ((scl_irect->w == pipe_info->dst_width)
            && (scl_irect->h == pipe_info->dst_height)
            && (scl_iformat == ADE_SCL_ARGB888)) {
        ade_cmdq_wr_cmd(unit_reg->scl_ctrl, 0x400);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &cf_buff->cmd_len);

        ade_cmdq_wr_cmd(unit_reg->scl_ores, (scl_irect->h - 1) << 16 | (scl_irect->w - 1));
        ade_cmdq_wr_cmd(unit_reg->scl_ires, (scl_irect->h - 1) << 16 | (scl_irect->w - 1));
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    } else {
        scl_orect.w = pipe_info->dst_width;
        scl_orect.h = pipe_info->dst_height;
        scl_orect.x = scl_irect->x;
        scl_orect.y = scl_irect->y;

        /* yuv420/yuv422 must be scl to yuv444 */
        ade_cmdq_wr_scl_ctrl_cmd(unit_reg->scl_ctrl, pipe_info, scl_irect, &scl_orect);
        ade_cmdq_wr_scl_hsp_cmd(unit_reg->scl_hsp, scl_irect, &scl_orect);

        /*
         * yuv420: 240x320 scl to 200x200, means:
         * y: 240 scl to 200, 320 scl to 200,
         * uv: 120 scl to 200, 160 scl to 200.
         */
        scl_uv_irect.x = 0;
        scl_uv_irect.y = 0;
        if (scl_iformat == ADE_SCL_YUV420) {
            scl_uv_irect.w = scl_irect->w / 2;
            scl_uv_irect.h = scl_irect->h / 2;
            ade_cmdq_wr_scl_hsp_cmd(unit_reg->scl_uv_hsp, &scl_uv_irect, &scl_orect);
        } else if (scl_iformat == ADE_SCL_YUV422) {
            scl_uv_irect.w = scl_irect->w / 2;
            scl_uv_irect.h = scl_irect->h;
            ade_cmdq_wr_scl_hsp_cmd(unit_reg->scl_uv_hsp, &scl_uv_irect, &scl_orect);
        } else if (scl_iformat == ADE_SCL_YUV444) {
            scl_uv_irect.w = scl_irect->w;
            scl_uv_irect.h = scl_irect->h;
            ade_cmdq_wr_scl_hsp_cmd(unit_reg->scl_uv_hsp, &scl_uv_irect, &scl_orect);
        } else {
            ade_cmdq_wr_cmd(unit_reg->scl_uv_hsp, 0);
        }

        ade_cmdq_wr_scl_vsp_cmd(unit_reg->scl_vsp, scl_irect, &scl_orect);
        if (scl_iformat != ADE_SCL_ARGB888) {
            ade_cmdq_wr_scl_vsp_cmd(unit_reg->scl_uv_vsp, &scl_uv_irect, &scl_orect);
        } else {
            ade_cmdq_wr_cmd(unit_reg->scl_uv_vsp, 0);
        }

        ade_cmdq_wr_cmd(unit_reg->scl_ores, (scl_orect.h - 1) << 16 | (scl_orect.w - 1));
        ade_cmdq_wr_cmd(unit_reg->scl_ires, (scl_irect->h - 1) << 16 | (scl_irect->w - 1));  /* input size */
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_scl_hcoef_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), unit_reg->scl_hcoef, scl_irect, &scl_orect); /* two cmd */
        ade_cmdq_wr_scl_vcoef_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), unit_reg->scl_vcoef, scl_irect, &scl_orect); /* two cmd */

        /* get scl output size, be used to next unit, such ctran */
        scl_irect->w = scl_orect.w;
        scl_irect->h = scl_orect.h;
    }

    ade_cmdq_wr_cmd(unit_reg->scl_start, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
#if 0
    ade_cmdq_wr_cmd(unit_reg->scl_pix_ofst, 4);
    ade_cmdq_wr_cmd(unit_reg->scl_uv_pix_ofst, 4);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
#endif
    if (OVERLAY_PIPE_TYPE_ONLINE == pipe_info->ch_type) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst0, unit_reg->scl_rst_sel_bit, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis0, unit_reg->scl_rst_sel_bit);

        balongfb_logi_display_debugfs("ade_overlay_region_set_scl_cmd: ade_pri_data->soft_rst0=0x%x\n", ade_pri_data->soft_rst0);
    }

    balongfb_logi_display_debugfs("ade_overlay_region_set_scl_cmd: region_dst_rect.x = %d,region_dst_rect.y=%d \n", pipe_info->dst_rect.x,pipe_info->dst_rect.y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_scl_cmd: region_dst_rect.w = %d,region_dst_rect.h=%d \n", pipe_info->dst_rect.w,pipe_info->dst_rect.h);
    balongfb_logi_display_debugfs("ade_overlay_region_set_scl_cmd: output_rect.x = %d,output_rect.y=%d \n", scl_irect->x,scl_irect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_scl_cmd: output_rect.w = %d,output_rect.h=%d \n", scl_irect->w,scl_irect->h);
}
static void ade_overlay_region_set_ctran_cmd(struct ade_compose_data_type *ade_pri_data, struct ade_overlay_pipe *pipe, struct cmdfile_buffer *cf_buff)
{
    struct  ch_unit_reg      *unit_reg;
    struct  overlay_region_info *pipe_info;
    struct  ade_rect         *ctran_irect;
    bool    is_need_csc;
    u8      global_alpha;
    u32     ctran_factor;
    u8      i;

    BUG_ON((pipe == NULL) || (cf_buff == NULL));

    unit_reg  = &(pipe->pipe_reg);
    pipe_info = &(pipe->pipe_info);
    ctran_irect = &(pipe->output_rect);

    if (!pipe->pipe_cap.bit_csc_enable) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_ctran_cmd: ch %d, ctrans is disable \n",pipe->pipe_num);
        return;
    }

    if ((pipe_info->phy_addr == 0) || (pipe_info->height == 0) || (pipe_info->stride == 0)) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_ctran_cmd: phy_addr or heigh or stride is 0, don't need config ctran! \n");
        return;
    }

    /* not need csc and fmc, bypass ctran */
    is_need_csc = ade_overlay_need_yuv2rgb(pipe_info->format);
    if (!is_need_csc) {
        /* If the layer has valid global alpha and pixel alpha in pre-mult blending mode,
         * we use ctran to make src_color * global_alpha. */
        if ((pipe_info->blending & 0xffff) == OVERLAY_BLENDING_PIX_GLB) {
            global_alpha = (pipe_info->blending >> 16) & 0xff;
            ctran_factor = ROUND(global_alpha * 512, 255);

            ade_cmdq_wr_cmd(unit_reg->ctran_dis, 0);
            ade_cmdq_wr_cmd(unit_reg->ctran_mode_choose, 3);  /* ARGB to ARGB */
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

            /* config chdc0~chdc5 to zero */
            i = 0;
            while (i < 24) {
                ade_cmdq_wr_cmd(unit_reg->ctran_chdc0 + i, 0);
                i += 4;
            }

            /* config csc00/csc11/csc22 to ctran_factor, others set to zero */
            ade_cmdq_wr_cmd(unit_reg->ctran_csc0, ctran_factor);
            ade_cmdq_wr_cmd(unit_reg->ctran_csc0 + 0x4, 0);
            ade_cmdq_wr_cmd(unit_reg->ctran_csc0 + 0x8, ctran_factor);
            ade_cmdq_wr_cmd(unit_reg->ctran_csc0 + 0xC, 0);
            ade_cmdq_wr_cmd(unit_reg->ctran_csc0 + 0x10, ctran_factor);

            ade_cmdq_wr_cmd(unit_reg->ctran_image_size, (ctran_irect->w * ctran_irect->h - 1));
            ade_cmdq_wr_cmd(unit_reg->ctran_cfg_ok, 1);
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        } else {
            ade_cmdq_wr_cmd(unit_reg->ctran_dis, 1);
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

            ade_cmdq_wr_cmd(unit_reg->ctran_image_size, (ctran_irect->w * ctran_irect->h - 1));
            ade_cmdq_wr_cmd(unit_reg->ctran_cfg_ok, 1);
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        }
    } else {
        ade_cmdq_wr_cmd(unit_reg->ctran_dis, 0);
        ade_cmdq_wr_cmd(unit_reg->ctran_mode_choose, 0);  /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv */
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_ctran_chdc_cmd_with_yuv_info(unit_reg->ctran_chdc0, 0, pipe_info->yuv_info); /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv */
        ade_cmdq_wr_ctran_csc_cmd_with_yuv_info(unit_reg->ctran_csc0, 0, pipe_info->yuv_info);
        ade_cmdq_wr_cmd(unit_reg->ctran_image_size, (ctran_irect->w * ctran_irect->h - 1));
        ade_cmdq_wr_cmd(unit_reg->ctran_cfg_ok, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    }

    if (OVERLAY_PIPE_TYPE_ONLINE == pipe_info->ch_type) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst0, unit_reg->ctran_rst_sel_bit, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis0, unit_reg->ctran_rst_sel_bit);

        balongfb_logi_display_debugfs("ade_overlay_region_set_ctran_cmd: ade_pri_data->soft_rst0=0x%x\n", ade_pri_data->soft_rst0);
    }

    balongfb_logi_display_debugfs("ade_overlay_region_set_ctran_cmd: output_rect.x = %d,output_rect.y=%d \n", ctran_irect->x,ctran_irect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_ctran_cmd: output_rect.w = %d,output_rect.h=%d \n", ctran_irect->w,ctran_irect->h);

    return;
}

static void ade_overlay_region_set_ovly_cmd(struct ade_compose_data_type *ade_pri_data, struct ade_overlay_ctrl *overlay_ctl, struct ade_overlay_pipe *pipe, struct cmdfile_buffer *cf_buff)
{
    struct overlay_region_info *pipe_info;
    struct ade_overlay_ovly    *ovly = NULL;
    struct ovly_reg            *ovly_reg;
    struct ovly_ctrl_info       ch_ctrl_info;
    struct ade_rect             ovly_region_rect;
    struct ade_rect            *ovly_orect;
    struct ade_overlay_scl2    *scl2_info;
    u8           is_need_scl2 = ADE_FALSE;
    u32          xy0;
    u32          xy1;
    u32          output_size;
    u32          surf_num = 0;

    BUG_ON((pipe == NULL) || (cf_buff == NULL) || (overlay_ctl == NULL));

    pipe_info = &(pipe->pipe_info);
    surf_num  = pipe_info->surface_num;
    ovly_orect = &(pipe->output_rect);

    ovly = ade_overlay_cap2ovly(overlay_ctl, pipe->pipe_cap);
    if (ovly == NULL) {
        balongfb_logi_display_debugfs(" get ovly overlay_pipe_ade_disp pipe\n");
        return;
    }

    ovly_reg  = &(ovly->ovly_reg);

    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd: ovly num: %d \n", ovly->ovly_num);
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd:pipe_info->sec_ovly_ch_count = %d \n", pipe_info->sec_ovly_ch_count);
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd:ovly_reg->ovly_ch_sel_start[%d] = %d \n",surf_num, ovly_reg->ovly_ch_sel_start[surf_num]);

    /* if scl2_state is wait_cfg, so need init the scl2 information,
     * if scl2_state is cfg_succ, and the pipe need cfg scl2, and it is a new layer,
     * so need trnsform scl2_state from cfg_succ to wait_cfg!
     */
    scl2_info = &overlay_ctl->scl2_info;
    is_need_scl2 = ADE_FALSE;
    if ((ADE_SCL2_STATE_IDLE == scl2_info->scl2_state)
            && (ADE_TRUE == pipe->need_cfg_scl2)) {
        is_need_scl2 = ADE_TRUE;
    }

    if ((ADE_SCL2_STATE_CFG_SUCC == scl2_info->scl2_state)
            && (pipe_info->phy_addr != 0)
            && (ADE_TRUE == pipe->need_cfg_scl2)) {
        is_need_scl2 = ADE_TRUE;
    }

    if (ADE_TRUE == is_need_scl2) {
        scl2_info->scl2_state = ADE_SCL2_STATE_WAIT_CFG;
    }

    /* bypass ovly3 */
    if (pipe_info->sec_ovly_ch_count == 1) {
        ade_overlay_set_bypass_ovly(ade_pri_data, overlay_ctl, pipe, ovly, cf_buff);
        return;
    }

    if (surf_num >= OVERLAY_ADE_SURFACE_MAX) {
        balongfb_loge(" surf_num = %d is overflow \n ", surf_num);
        return;
    }

    /*****************************************
     *****************
     *               *
     *  dst_rect_1   *
     *               *
     *---------------*
     *               *
     *  dst_rect_2   *
     *               *
     *---------------*
     *               *
     *  dst_rect_3   *
     *               *
     *               *
     *****************
     ******************************************/
    /* set top ovly ctrl, select ovly of the surface_num  */
    set_TOP_OVLY_CTRL(ovly->ade_base, ovly_reg->ovly_ch_sel_start[surf_num], ovly->ovly_num + 1);

    memcpy(&ovly_region_rect,&(pipe_info->dst_rect), sizeof(struct ade_rect));

    /*if ((pipe_info->sec_ovly_ch_count > 1) && (OVERLAY_OVLY_3 == ovly->ovly_num)) {
      ovly_region_rect.w = pipe_info->src_rect.w;
      ovly_region_rect.h = pipe_info->src_rect.h;
      }*/

    /* xy0 */
    xy0 = ((ovly_region_rect.x - pipe_info->ovly_output_rect.x) << 16) | (ovly_region_rect.y - pipe_info->ovly_output_rect.y);
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy0[surf_num], xy0);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd:xy0 = 0x%x \n",xy0);

    /* xy1 */
    xy1 = ((ovly_region_rect.x - pipe_info->ovly_output_rect.x + ovly_region_rect.w - 1) << 16)  \
          | (ovly_region_rect.y - pipe_info->ovly_output_rect.y + ovly_region_rect.h - 1);
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy1[surf_num], xy1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd:xy1 = 0x%x \n",xy1);

    /* output size */
    output_size = ((pipe_info->ovly_output_rect.w -1) << 16) | (pipe_info->ovly_output_rect.h - 1);
    ade_cmdq_wr_cmd(ovly_reg->ovly_output_size, output_size);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd:output_size = 0x%x \n",output_size);
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd:pipe_info->ch_num = %d \n",pipe_info->ch_num);

    /* ovly_ch ctrl */
    ch_ctrl_info.ovly_num = ovly->ovly_num;
    ch_ctrl_info.ch_num = pipe_info->ch_num;
    ch_ctrl_info.blending = pipe_info->blending;
    ch_ctrl_info.dim_blending = pipe_info->dim_blending;
    ch_ctrl_info.format = pipe_info->format;
    ch_ctrl_info.sec_ovly_ch_count = pipe_info->sec_ovly_ch_count;
    ch_ctrl_info.ovly_type = ADE_OVLY_TYPE_FIRST_OVLY;
    ade_cmdq_wr_ovly_ch_ctrl_cmd(ovly_reg->ovly_ch_ctrl[surf_num], ch_ctrl_info);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    if (OVERLAY_OVLY_1 == ovly->ovly_num) {
        overlay_ctl->ovly1_ch_used |= 1 << surf_num;
    } else {
        overlay_ctl->ovly2_ch_used |= 1 << surf_num;
    }
    /* when multi channel compose, then Scalor,  the layer can't be cut
     * such as (ch1->ovly3 + ch2->ovly3)=>SCL2 =>ovly2=>ldi
     */
    if (pipe_info->sec_ovly_ch_count > 1) {
        memcpy(ovly_orect, &pipe_info->ovly_output_rect, sizeof(struct ade_rect));
    }

    if (OVERLAY_PIPE_TYPE_ONLINE == pipe_info->ch_type) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst1, ovly_reg->ovly_rst_sel_bit, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis1, ovly_reg->ovly_rst_sel_bit);

        balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd: ovly_rst_sel_bit=%d\n", ovly_reg->ovly_rst_sel_bit);
    }

    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd: output_rect.x = %d,output_rect.y=%d \n", ovly_orect->x,ovly_orect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd: output_rect.w = %d,output_rect.h=%d \n", ovly_orect->w,ovly_orect->h);
}

static void ade_overlay_region_set_rot_cmd(struct ade_compose_data_type *ade_pri_data, struct ade_overlay_pipe *pipe, struct ade_overlay_rot *rot_info, struct cmdfile_buffer *cf_buff)
{
    struct  overlay_region_info *pipe_info;
    struct  ade_rect            *rot_irect;
    struct  ade_rect             rot_orect;

    BUG_ON((pipe == NULL) || (cf_buff == NULL) || (rot_info == NULL));

    pipe_info = &(pipe->pipe_info);
    rot_irect = &(pipe->output_rect);

    if (pipe->need_cfg_rot_dma == ADE_FALSE) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_rot_cmd: need_cfg_rot_dma is false. \n");
        return;
    }

    if (pipe_info->sec_ovly_ch_count != 1) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_rot_cmd: sec_ovly_ch_count is not 1. \n");
        return;
    }

    if (rot_info->rot_en == ADE_FALSE) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_rot_cmd: ROT is not enable. \n");
        return;
    }

    if (rot_info->is_configed == ADE_TRUE) {
        return;
    }

    if ((pipe_info->phy_addr == 0) || (pipe_info->width) == 0 || (pipe_info->height) == 0) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_rot_cmd: phy_addr or width or height is 0 not need config ROT!\n");
        return;
    }

    rot_info->rot_width = rot_irect->w;
    rot_info->rot_height = rot_irect->h;
    ade_overlay_set_rot_cmd(pipe->ade_base, rot_info, cf_buff);

    /* Get Rot Output Rect for next unit. such as SCL2,OVLY2 */
    ade_overlay_get_rect_from_rot(rot_info->rotation, rot_irect, &rot_orect);

    memcpy(rot_irect, &rot_orect, sizeof(struct ade_rect));

    if (OVERLAY_PIPE_TYPE_ONLINE == pipe_info->ch_type) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 30, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis0, 30);

        balongfb_logi_display_debugfs("ade_overlay_region_set_rot_cmd: ade_pri_data->soft_rst0 = 0x%x \n", ade_pri_data->soft_rst0);
    }

    balongfb_logi_display_debugfs("ade_overlay_region_set_rot_cmd: output_rect.x = %d,output_rect.y=%d \n", rot_irect->x,rot_irect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_rot_cmd: output_rect.w = %d,output_rect.h=%d \n", rot_irect->w,rot_irect->h);
}

static void ade_overlay_region_set_scl2_cmd(struct ade_compose_data_type *ade_pri_data, struct ade_overlay_pipe *pipe, struct ade_overlay_scl2 *scl2_info, struct cmdfile_buffer *cf_buff)
{
    struct  overlay_region_info *pipe_info;
    struct  ade_rect            *scl_irect;

    BUG_ON((pipe == NULL) || (cf_buff == NULL) || (scl2_info == NULL));

    pipe_info = &(pipe->pipe_info);
    scl_irect = &(pipe->output_rect);

    if (scl2_info->scl2_en == ADE_FALSE) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: scl2_en = false \n");
        return;
    }

    /* ch_x->ovly_x->scl2 */
    if (pipe->need_cfg_scl2 == ADE_FALSE) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: pipe->num = %d,need_cfg_scl2 = false \n", pipe->pipe_num);
        return;
    }

    /* ch_x->ovly3->scl2 */
    if (pipe_info->sec_ovly_ch_count != 1) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: sec_ovly_ch_count != 1 \n");
        return;
    }

    /* only for that ch don't have scl1 and scl3  */
    if (pipe->pipe_cap.bit_scl1_enable || pipe->pipe_cap.bit_scl3_enable) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: pipe->num = %d, bit_scl_enable = true \n", pipe->pipe_num);
        return;
    }

    if ((pipe_info->phy_addr == 0) || (pipe_info->width) == 0 || (pipe_info->height) == 0) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: phy_addr or width or height is 0 not need config SCL2!\n");
        return;
    }

    if (ADE_SCL2_STATE_WAIT_CFG != scl2_info->scl2_state) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: scl2_info->scl2_state = %d is not wait cfg!\n", scl2_info->scl2_state);
        return;
    }


    balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: input_rect.x = %d,input_rect.y=%d \n", scl_irect->x,scl_irect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: input_rect.w = %d,input_rect.h=%d \n", scl_irect->w,scl_irect->h);

    /* SCL2:
     * ch does not have scl capability, but region need scl, so must use scl2
     */
    scl2_info->scl2_iformat = pipe_info->format;
    scl2_info->dst_rect.x = pipe_info->dst_rect.x;
    scl2_info->dst_rect.y = pipe_info->dst_rect.y;
    scl2_info->dst_rect.w = pipe_info->dst_width;
    scl2_info->dst_rect.h = pipe_info->dst_height;

    memcpy(&scl2_info->src_rect, scl_irect, sizeof(struct ade_rect));

    ade_overlay_set_scl2_cmd(pipe->ade_base, scl2_info, cf_buff);

    /* Get SCL2 output size for next unit, such as ovly2 */
    memcpy(scl_irect, &scl2_info->dst_rect, sizeof(struct ade_rect));

    if (OVERLAY_PIPE_TYPE_ONLINE == pipe_info->ch_type) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 22, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis0, 22);

        balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: ade_pri_data->soft_rst0 = 0x%x \n", ade_pri_data->soft_rst0);
    }

    balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: output_rect.x = %d,output_rect.y=%d \n", scl_irect->x,scl_irect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_scl2_cmd: output_rect.w = %d,output_rect.h=%d \n", scl_irect->w,scl_irect->h);
}

static void ade_overlay_region_set_sec_ovly_cmd(struct ade_compose_data_type *ade_pri_data, struct ade_overlay_ctrl *overlay_ctl, struct ade_overlay_pipe *pipe, struct cmdfile_buffer *cf_buff)
{
    struct ade_overlay_ovly    *ovly = NULL;
    struct ade_overlay_ovly    *sec_ovly = NULL;
    struct ovly_info           *ovly_comp;
    struct overlay_region_info *pipe_info;
    struct ade_rect            *ovly_irect;
    OVLY1_RES    ovly1_res;
    OVLY2_RES    ovly2_res;
    OVLY3_RES    ovly3_res;
    ROT_RES      rot_res;
    SCL2_RES     scl2_res;
    bool         need_sec_ovly1 = false;
    bool         need_sec_ovly2 = false;

    BUG_ON((pipe == NULL) || (cf_buff == NULL) || (overlay_ctl == NULL));

    pipe_info = &(pipe->pipe_info);
    if (pipe_info->sec_ovly_ch_count <= 0) {
        balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd: sec_ovly_ch_count= %d, not need sec ovly \n", pipe_info->sec_ovly_ch_count);
        return;
    }

    ovly_irect = &(pipe->output_rect);

    ovly = ade_overlay_cap2ovly(overlay_ctl, pipe->pipe_cap);
    if (ovly == NULL) {
        balongfb_loge(" get ovly error pipe_num=%d", pipe->pipe_num);
        return;
    }

    ovly1_res.ul32 = overlay_ctl->ade_res_info.ovly1_res.ul32;
    ovly2_res.ul32 = overlay_ctl->ade_res_info.ovly2_res.ul32;
    ovly3_res.ul32 = overlay_ctl->ade_res_info.ovly3_res.ul32;
    rot_res.ul32   = overlay_ctl->ade_res_info.rot_res.ul32;
    scl2_res.ul32  = overlay_ctl->ade_res_info.scl2_res.ul32;

    need_sec_ovly1 = ((ovly->ovly_num == OVERLAY_OVLY_3) &&
            (ovly3_res.bits.has_ovly1 || \
             rot_res.bits.has_ovly1 || \
             scl2_res.bits.has_ovly1) && \
            ovly1_res.bits.ovly_en);

    need_sec_ovly2 = ((ovly->ovly_num == OVERLAY_OVLY_3) &&
            (ovly3_res.bits.has_ovly2 || \
             rot_res.bits.has_ovly2 || \
             scl2_res.bits.has_ovly2) && \
            ovly2_res.bits.ovly_en);

    if (need_sec_ovly1) {
        sec_ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_1]);
    } else if (need_sec_ovly2) {
        sec_ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_2]);
    } else {
        balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd: not need to config sec ovly \n");
        return;
    }

    /* the channal nend to sec ovly
     *
     * when ch4->ovly3->ovly2, the sec_ovly_ch_count is 1,
     * when (ch3->ovly3 + ch4->ovly3)->ovly2, the sec_ovly_ch_count is 2,
     * they have different config
     */
    ovly_comp = &(sec_ovly->ovly_comp);

    ovly_comp->sec_ovly_ch_num   = pipe->pipe_num;
    ovly_comp->ch_count    = pipe_info->sec_ovly_ch_count;
    if (ovly_comp->ch_count > 1) {
        /*
         * 1,(ch3->ovly3 + ch4->ovly3)->ovly2(or ovly1)->LDI(or WDMA1)
         * 2,(ch3->ovly3 + ch4->ovly3)->ROT->SCL2->ovly2(or ovly1)->LDI(or WDMA1)
         * 3,(ch3->ovly3 + ch4->ovly3)->ROT->ovly2(or ovly1)->SCL2->LDI(or WDMA1)
         * so, need config ROT,SCL2,OVLY1/2 at commit cmd
         */
        ovly_comp->sec_ovly_surf_num = pipe_info->sec_ovly_surf_num;
        memcpy(&ovly_comp->input_rect, ovly_irect, sizeof(struct ade_rect));
        ovly_comp->input_rect.x = 0;
        ovly_comp->input_rect.y = 0;

        memcpy(&ovly_comp->output_rect, &pipe_info->sec_ovly_output_rect, sizeof(struct ade_rect));
        ovly_comp->output_rect.x = 0;
        ovly_comp->output_rect.y = 0;

        balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd:ovly_comp->input_rect.w = %d, input_rect.h=%d \n", ovly_comp->input_rect.w, ovly_comp->input_rect.h);
        balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd:ovly_comp->output_rect.w = %d, output_rect.h=%d \n", ovly_comp->output_rect.w, ovly_comp->output_rect.h);
        balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd:ovly_comp->sec_ovly_surf_num = %d \n", ovly_comp->sec_ovly_surf_num);
        balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd: sec_ovly_ch_count > 1,don't config sec ovly here \n");
        return;
    } else {
        ovly_comp->sec_ovly_surf_num = pipe_info->surface_num;
        memcpy(&ovly_comp->input_rect, &pipe_info->dst_rect, sizeof(struct ade_rect));
        memcpy(&ovly_comp->output_rect, &pipe_info->ovly_output_rect, sizeof(struct ade_rect));
    }

    /* the second ovly */
    ade_overlay_set_sec_ovly(overlay_ctl, sec_ovly, cf_buff);

#if 0
    if (pipe_info->region_is_finished && (pipe_info->sec_ovly_ch_count == 1)) {
        balongfb_logi_display_debugfs("set sec_ovly: overlay_ctl->ovly2_ch_used = 0x%x \n", overlay_ctl->ovly2_ch_used);

        /* disable unused channel of overlay */
        for (i = 0; i < OVERLAY_ADE_SURFACE_MAX; i++) {
            if ((overlay_ctl->ovly2_ch_used & (1 << i)) == 0) {
                ade_cmdq_wr_cmd(sec_ovly->ovly_reg.ovly_ch_ctrl[i], 0x11);
                ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
                balongfb_logi_display_debugfs("disable unused channel index: %d \n", i);
            }
        }
        overlay_ctl->ovly2_ch_used = 0;
        /*
           ade_cmdq_wr_cmd(sec_ovly->ovly_reg.ovly_ctrl, ADE_TRUE);
           ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

           balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd: ovly_num=%d, ovly_ctrl start enable here!!! \n", ovly->ovly_num);
           */
    }
#endif

    if (OVERLAY_OVLY_2 == sec_ovly->ovly_num) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst1, 6, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis1, 6);

        balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd: ade_pri_data->soft_rst1 = 0x%x \n",ade_pri_data->soft_rst1);
    }

    balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd:ovly_irect.x = %d,ovly_irect.y=%d \n", ovly_irect->x, ovly_irect->y);
    balongfb_logi_display_debugfs("ade_overlay_region_set_sec_ovly_cmd:ovly_irect.w = %d,ovly_irect.h=%d \n", ovly_irect->w, ovly_irect->h);
}
/*****************  REGION CFG END   *******************************************/

/***************** COMMIT CFG BEGIN ******************************************/
void ade_overlay_commit_get_unit(struct overlay_resource_info *ade_res_info, struct commit_comp_unit *commit_unit)
{
    DISP_CH_RES  disp_res;
    OVLY1_RES    ovly1_res;
    OVLY2_RES    ovly2_res;
    OVLY3_RES    ovly3_res;
    ROT_RES      rot_res;
    SCL2_RES     scl2_res;

    BUG_ON((ade_res_info == NULL) || (commit_unit == NULL));

    disp_res.ul32 = ade_res_info->disp_ch_res.ul32;
    ovly1_res.ul32 = ade_res_info->ovly1_res.ul32;
    ovly2_res.ul32 = ade_res_info->ovly2_res.ul32;
    ovly3_res.ul32 = ade_res_info->ovly3_res.ul32;
    rot_res.ul32 = ade_res_info->rot_res.ul32;
    scl2_res.ul32 = ade_res_info->scl2_res.ul32;

    /* bit ldi */
    commit_unit->bit_ldi = disp_res.bits.has_ldi    | \
                           ovly2_res.bits.has_ldi   | \
                           rot_res.bits.has_ldi     | \
                           scl2_res.bits.has_ldi;

    /* bit sec_ovly2 */
    commit_unit->bit_sec_ovly2 = (((ovly3_res.bits.has_ovly2) | \
                (rot_res.bits.has_ovly2)   | \
                (scl2_res.bits.has_ovly2)) & \
            (ovly2_res.bits.ovly_en));

    /* bit sec_ovly1 */
    commit_unit->bit_sec_ovly1 = (((ovly3_res.bits.has_ovly1) | \
                (rot_res.bits.has_ovly1)   | \
                (scl2_res.bits.has_ovly1)) & \
            (ovly1_res.bits.ovly_en));

    /* bit scl3 */
    commit_unit->bit_scl3 = disp_res.bits.has_scl3 | \
                            ovly1_res.bits.has_scl3   | \
                            ovly2_res.bits.has_scl3   | \
                            rot_res.bits.has_scl3  | \
                            scl2_res.bits.has_scl3;

    /* bit scl2 */
    commit_unit->bit_scl2 = (((disp_res.bits.has_scl2)    | \
                (ovly2_res.bits.has_scl2)   | \
                (ovly3_res.bits.has_scl2)   | \
                (rot_res.bits.has_scl2)) & \
            (scl2_res.bits.scl2_en));

    /* bit rot */
    commit_unit->bit_rot = (((disp_res.bits.has_720p_rot)  | \
                (disp_res.bits.has_1080p_rot) | \
                (ovly2_res.bits.has_720p_rot)  | \
                (ovly2_res.bits.has_1080p_rot) | \
                (ovly3_res.bits.has_720p_rot)  | \
                (ovly3_res.bits.has_1080p_rot)) & \
            (rot_res.bits.rot_en));


    /* bit offline wdma3 */
    commit_unit->bit_offline_wdma3 = ovly1_res.bits.has_wdma3;

    /* bit wdma3 */
    commit_unit->bit_online_wdma3 = disp_res.bits.has_wdma3  | \
                                    ovly2_res.bits.has_wdma3 | \
                                    rot_res.bits.has_wdma3   | \
                                    scl2_res.bits.has_wdma3;

    /* bit wdma2 */
    commit_unit->bit_wdma2 = disp_res.bits.has_wdma2 | \
                             ovly2_res.bits.has_wdma2 | \
                             ovly3_res.bits.has_wdma2 | \
                             rot_res.bits.has_wdma2   | \
                             scl2_res.bits.has_wdma2;

    commit_unit->bit_part_update = disp_res.bits.ch_en & \
                                   disp_res.bits.has_ovly2;

    /* bit wdma1 */
    commit_unit->bit_wdma1 = ovly1_res.bits.has_wdma1 & ovly1_res.bits.ovly_en;

}

static void ade_overlay_commit_set_ctran4(struct ade_compose_data_type *ade_pri_data, struct overlay_compose_info *pcomp_info, struct cmdfile_buffer *cf_buff, u32 has_scl3, u32 online)
{
    u32     ctran_size;
    bool    csc_en = false;

    if ((cf_buff == NULL) || (pcomp_info == NULL)) {
        balongfb_loge("cf_buff or pcomp_info is null ptr \n");
        return;
    }

    ctran_size = pcomp_info->src_rect.w * pcomp_info->src_rect.h - 1;
    csc_en     = ade_overlay_need_yuv2rgb(pcomp_info->wifi_dst_format);

    if (online) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 27, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis0, 27);
    }

    /* bypass ctran4 */
    if (!csc_en) {
        ade_cmdq_wr_cmd(ADE_CTRAN4_DIS_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(ADE_CTRAN4_IMAGE_SIZE_REG, ctran_size);
        ade_cmdq_wr_cmd(ADE_CTRAN4_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        return;
    }

    ade_cmdq_wr_cmd(ADE_CTRAN4_DIS_REG, 0);
    if (has_scl3) {
        ade_cmdq_wr_cmd(ADE_CTRAN4_MODE_CHOOSE_REG, 2);  /* 0: csc,yuv2rgb, 2: fmc, rgb888->yuv444 */
    } else {
        ade_cmdq_wr_cmd(ADE_CTRAN4_MODE_CHOOSE_REG, 1);  /* 0: csc,yuv2rgb, 1: fmc, rgb888->yuv422 */
    }
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_ctran_chdc_cmd(ADE_CTRAN4_CHDC0_REG, 1);
    ade_cmdq_wr_ctran_csc_cmd(ADE_CTRAN4_CSC0_REG, 1);
    ade_cmdq_wr_cmd(ADE_CTRAN4_IMAGE_SIZE_REG, ctran_size);
    ade_cmdq_wr_cmd(ADE_CTRAN4_CFG_OK_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

static void ade_overlay_commit_set_ctran5(struct ade_compose_data_type *ade_pri_data, struct ade_rect image_rect, u32 format, struct cmdfile_buffer *cf_buff)
{
    u32     ctran_size;
    bool    csc_en = false;

    if (cf_buff == NULL) {
        balongfb_loge("cf_buff is null ptr");
        return;
    }

    ctran_size = image_rect.w * image_rect.h - 1;
    csc_en     = ade_overlay_need_yuv2rgb(format);

    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 28, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 28);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_ctran5: ade_pri_data->soft_rst0 = 0x%x \n", ade_pri_data->soft_rst0);

    /* bypass ctran5 */
    if (!csc_en) {
        ade_cmdq_wr_cmd(ADE_CTRAN5_DIS_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(ADE_CTRAN5_IMAGE_SIZE_REG, ctran_size);
        ade_cmdq_wr_cmd(ADE_CTRAN5_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        return;
    }

    ade_cmdq_wr_cmd(ADE_CTRAN5_DIS_REG, 0);
    ade_cmdq_wr_cmd(ADE_CTRAN5_MODE_CHOOSE_REG, 0);  /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv */
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_ctran_chdc_cmd(ADE_CTRAN5_CHDC0_REG, 0);
    ade_cmdq_wr_ctran_csc_cmd(ADE_CTRAN5_CSC0_REG, 0);
    ade_cmdq_wr_cmd(ADE_CTRAN5_IMAGE_SIZE_REG, ctran_size);
    ade_cmdq_wr_cmd(ADE_CTRAN5_CFG_OK_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    return;
}

static void ade_overlay_commit_set_ctran6(struct ade_compose_data_type *ade_pri_data, bool need_csc, u32 w, u32 h, struct cmdfile_buffer *cf_buff)
{
    u32     ctran_size;

    if (cf_buff == NULL) {
        balongfb_loge("pcomp_info or cf_buff is null ptr \n");
        return;
    }

    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 29, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 29);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_ctran6: ade_pri_data->soft_rst0=0x%x \n", ade_pri_data->soft_rst0);

    ctran_size = w * h - 1;

    if(g_is_set_color_temperature == true)
    {
        ade_cmdq_wr_cmd(ADE_CTRAN6_DIS_REG, 0);
        ade_cmdq_wr_cmd(ADE_CTRAN6_MODE_CHOOSE_REG, 3);
        /* 0:YUV444>ARGB8888; 1:ARGB8888>YUV422; 2:ARGB8888>YUV444; 3:ARGB8888>ARGB8888 */
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_ctran_chdc_cmd(ADE_CTRAN6_CHDC0_REG, 3);
        ade_cmdq_wr_ctran_csc_cmd(ADE_CTRAN6_CSC0_REG, 3);
        ade_cmdq_wr_cmd(ADE_CTRAN6_IMAGE_SIZE_REG, ctran_size);
        ade_cmdq_wr_cmd(ADE_CTRAN6_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        return;
    }

    if (!need_csc) {
        /* bypass ctran6 */
        ade_cmdq_wr_cmd(ADE_CTRAN6_DIS_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(ADE_CTRAN6_IMAGE_SIZE_REG, ctran_size);
        ade_cmdq_wr_cmd(ADE_CTRAN6_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        return;
    }

    ade_cmdq_wr_cmd(ADE_CTRAN6_DIS_REG, 0);
    ade_cmdq_wr_cmd(ADE_CTRAN6_MODE_CHOOSE_REG, 0);  /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv */
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_ctran_chdc_cmd(ADE_CTRAN6_CHDC0_REG, 0);
    ade_cmdq_wr_ctran_csc_cmd(ADE_CTRAN6_CSC0_REG, 0);
    ade_cmdq_wr_cmd(ADE_CTRAN6_IMAGE_SIZE_REG, ctran_size);
    ade_cmdq_wr_cmd(ADE_CTRAN6_CFG_OK_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    return;
}
void ade_overlay_commit_set_wdma1(struct overlay_compose_info *pcomp_info, struct cmdfile_buffer *cf_buff)
{
    u32     offline_size;
    bool    need_fmc = false;
    u32     bpp = 0;

    BUG_ON((pcomp_info == NULL));

    if (cf_buff == NULL) {
        return;
    }

    offline_size = pcomp_info->offline_dst_rect.w * pcomp_info->offline_dst_rect.h - 1;

    bpp = ade_overlay_format2bpp(pcomp_info->offline_dst_format);
    need_fmc = ade_overlay_need_rgb2yuv(pcomp_info->offline_dst_format);

    /* CTRAN3 enable or disable */
    if (!need_fmc) {
        /* bypass */
        ade_cmdq_wr_cmd(ADE_CTRAN3_DIS_REG, 1);
        ade_cmdq_wr_cmd(ADE_CTRAN3_MODE_CHOOSE_REG, 3);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(ADE_CTRAN3_IMAGE_SIZE_REG, offline_size);
        ade_cmdq_wr_cmd(ADE_CTRAN3_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    } else {
        ade_cmdq_wr_cmd(ADE_CTRAN3_DIS_REG, 0);
        ade_cmdq_wr_cmd(ADE_CTRAN3_MODE_CHOOSE_REG, 1);  /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv,ARGB888 to yuv422 */
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_ctran_chdc_cmd(ADE_CTRAN3_CHDC0_REG, 1);
        ade_cmdq_wr_ctran_csc_cmd(ADE_CTRAN3_CSC0_REG, 1);
        ade_cmdq_wr_cmd(ADE_CTRAN3_IMAGE_SIZE_REG, offline_size);
        ade_cmdq_wr_cmd(ADE_CTRAN3_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    }

    ade_cmdq_wr_rdma_pe_cmd(WR_CH1_PE_REG, OVERLAY_PIPE_TYPE_OFFLINE, ADE_ROT_NOP);
    ade_cmdq_wr_wdma_ctrl_cmd(WR_CH1_CTRL_REG, pcomp_info);
    ade_cmdq_wr_cmd(WR_CH1_ADDR_REG, pcomp_info->offline_phy_addr);
    ade_cmdq_wr_cmd(WR_CH1_SIZE_REG, (pcomp_info->offline_dst_rect.w * bpp) | (pcomp_info->offline_dst_rect.h << 16));
    ade_cmdq_wr_cmd(WR_CH1_STRIDE_REG, pcomp_info->offline_stride);
    ade_cmdq_wr_cmd(WR_CH1_SPACE_REG, pcomp_info->offline_stride * pcomp_info->offline_dst_rect.h);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(WR_CH1_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

static void ade_overlay_commit_set_wdma3(struct ade_compose_data_type *ade_pri_data, struct overlay_compose_info  *pcomp_info, struct cmdfile_buffer *cf_buff, u32 has_scl3, u32 online)
{
    /* if wdma3 don't have scl3, we will use ctran4 to transform to yuv422,
     * if wifi dst format is yuv420, we must have scl3,
     * finally, wdma3 need write yuv420 or yuv422 format to buffer.
     */
    balongfb_logi_display_debugfs("ade_overlay_commit_set_wdma3: enter succ! has_scl3=%d \n", has_scl3);

    /* config once  */
    if ((pcomp_info->wifi_dst_rect.h == 0) || (pcomp_info->wifi_dst_rect.w == 0) || (pcomp_info->wifi_phy_addr == 0)) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_scl3: wifi dst rect w or h is 0 \n");
        return;
    }

    if (online) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 12, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis0, 12);

        balongfb_logi_display_debugfs("ade_overlay_commit_set_wdma3: ade_pri_data->soft_rst0 = 0x%x \n", ade_pri_data->soft_rst0);
    }
    if ((ADE_NV12 == pcomp_info->wifi_dst_format) || (ADE_NV21 == pcomp_info->wifi_dst_format)) {
        if (!has_scl3) {
            balongfb_loge("ade_overlay_commit_set_wdma3:wdma3 dst format is yuv420, it must have scl3, but now has_scl3 = 0 \n");
            return;
        }

        /* y */
        ade_cmdq_wr_rdma_pe_cmd(WR_CH3_PE_REG, OVERLAY_PIPE_TYPE_OFFLINE, ADE_ROT_NOP);
        ade_cmdq_wr_wdma3_ctrl_cmd(WR_CH3_CTRL_REG, pcomp_info, has_scl3);
        ade_cmdq_wr_cmd(WR_CH3_ADDR_REG, pcomp_info->wifi_phy_addr);
        ade_cmdq_wr_cmd(WR_CH3_SIZE_REG, pcomp_info->wifi_dst_rect.w | (pcomp_info->wifi_dst_rect.h << 16));
        ade_cmdq_wr_cmd(WR_CH3_STRIDE_REG, pcomp_info->wifi_dst_rect.w);
        ade_cmdq_wr_cmd(WR_CH3_SPACE_REG, pcomp_info->wifi_dst_rect.w * pcomp_info->wifi_dst_rect.h);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        /* uv */
        ade_cmdq_wr_cmd(WR_CH3_UV_ADDR_REG, pcomp_info->wifi_phy_addr + pcomp_info->wifi_dst_rect.w * pcomp_info->wifi_dst_rect.h);
        ade_cmdq_wr_cmd(WR_CH3_UV_SIZE_REG, pcomp_info->wifi_dst_rect.w | ((pcomp_info->wifi_dst_rect.h / 2) << 16));
        ade_cmdq_wr_cmd(WR_CH3_UV_STRIDE_REG, pcomp_info->wifi_dst_rect.w);
        ade_cmdq_wr_cmd(WR_CH3_UV_SPACE_REG, pcomp_info->wifi_dst_rect.w * (pcomp_info->wifi_dst_rect.h / 2));
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    } else {
        ade_cmdq_wr_rdma_pe_cmd(WR_CH3_PE_REG, OVERLAY_PIPE_TYPE_OFFLINE, ADE_ROT_NOP);
        ade_cmdq_wr_wdma3_ctrl_cmd(WR_CH3_CTRL_REG, pcomp_info, has_scl3);
        ade_cmdq_wr_cmd(WR_CH3_ADDR_REG, pcomp_info->wifi_phy_addr);
        ade_cmdq_wr_cmd(WR_CH3_SIZE_REG, pcomp_info->wifi_stride | (pcomp_info->wifi_dst_rect.h << 16));
        ade_cmdq_wr_cmd(WR_CH3_STRIDE_REG, pcomp_info->wifi_stride);
        ade_cmdq_wr_cmd(WR_CH3_SPACE_REG, pcomp_info->wifi_stride * pcomp_info->wifi_dst_rect.h);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        /* clear uv, only for v8r1 */
        if (HI6210_CHIPSET == get_chipset_type()) {
            ade_cmdq_wr_cmd(WR_CH3_UV_ADDR_REG, 0);
            ade_cmdq_wr_cmd(WR_CH3_UV_SIZE_REG, 0);
            ade_cmdq_wr_cmd(WR_CH3_UV_STRIDE_REG, 0);
            ade_cmdq_wr_cmd(WR_CH3_UV_SPACE_REG, 0);
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        }
    }

    ade_cmdq_wr_cmd(WR_CH3_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}


void ade_overlay_commit_set_ovly2_ch1(struct overlay_compose_info *pcomp_info,
        struct ade_compose_data_type *ade_pri_data,
        struct cmdfile_buffer *cf_buff)
{
    u8*     ade_base;
    u32     xy;
    struct ade_overlay_ovly  *ovly = NULL;
    struct ovly_reg         *ovly_reg;
    struct ovly_ctrl_info    ch_ctrl_info;

    BUG_ON(pcomp_info == NULL);
    BUG_ON(ade_pri_data == NULL);
    BUG_ON(cf_buff == NULL);

    ade_base = ade_pri_data->ade_base;

    /* set ch1 to ovly2 */
    set_TOP_OVLY_CTRL(ade_base, 0, OVERLAY_OVLY_2 + 1);

    ovly = &(ade_pri_data->overlay_ctl.ovly_list[OVERLAY_OVLY_2]);
    ovly_reg = &ovly->ovly_reg;

    /* xy0 */
    xy = 0;
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy0[OVERLAY_ADE_SURFACE_1], xy);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* xy1 */
    xy = ade_pri_data->xres << 16 | ade_pri_data->yres;
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy1[OVERLAY_ADE_SURFACE_1], xy);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* xy2 */
    xy = pcomp_info->dst_rect.x << 16 | pcomp_info->dst_rect.y;
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch1_xy2, xy);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* xy3 */
    xy = ((pcomp_info->dst_rect.x + pcomp_info->dst_rect.w) << 16) | (pcomp_info->dst_rect.y + pcomp_info->dst_rect.h);
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch1_xy3, xy);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* ch ctrl */
    ch_ctrl_info.ovly_num = OVERLAY_OVLY_2;
    ch_ctrl_info.ch_num = OVERLAY_PIPE_ADE_DISP + 1;
    ch_ctrl_info.blending = OVERLAY_BLENDING_NONE;
    ch_ctrl_info.dim_blending = 0;
    ch_ctrl_info.format = ade_pri_data->fb_imgType;
    ch_ctrl_info.sec_ovly_ch_count = 0;
    ch_ctrl_info.ovly_type = ADE_OVLY_TYPE_FIRST_OVLY;
    ade_cmdq_wr_ovly_ch_ctrl_cmd(ovly_reg->ovly_ch_ctrl[OVERLAY_ADE_SURFACE_1], ch_ctrl_info);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

static void ade_overlay_commit_set_scl3(struct ade_compose_data_type *ade_pri_data, struct overlay_compose_info *pcomp_info, struct cmdfile_buffer *cf_buff, u32 online)
{
    struct ade_rect scl3_uv_irect;
    int scl_iformat;

    BUG_ON((pcomp_info == NULL));

    if (cf_buff == NULL) {
        return;
    }

    /* config scl3 once */
    if ((pcomp_info->wifi_dst_rect.h == 0) || (pcomp_info->wifi_dst_rect.w == 0) || (pcomp_info->wifi_phy_addr == 0)) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_scl3: wifi dst rect w or h is 0 \n");
        return;
    }

    if (online) {
        set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 23, ade_pri_data->lcd_type);
        set_reload_dis_bit(ade_pri_data->reload_dis0, 23);
    }

    scl_iformat = getSclImageFormatType(pcomp_info->wifi_dst_format);

    /* bypass scl3 */
    if ((pcomp_info->wifi_src_rect.w == pcomp_info->wifi_dst_rect.w)
            && (pcomp_info->wifi_src_rect.h == pcomp_info->wifi_dst_rect.h)
            && (scl_iformat == ADE_SCL_ARGB888)) {
        ade_cmdq_wr_cmd(ADE_SCL3_CTRL_REG, 0x400);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(ADE_SCL3_ORES_REG, (pcomp_info->wifi_dst_rect.h - 1) << 16 | (pcomp_info->wifi_dst_rect.w - 1));
        ade_cmdq_wr_cmd(ADE_SCL3_IRES_REG, (pcomp_info->wifi_src_rect.h - 1) << 16 | (pcomp_info->wifi_src_rect.w - 1));
        ade_cmdq_wr_cmd(ADE_SCL3_START_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        return;
    }

    /* one wr cmd */
    ade_cmdq_wr_ovly_scl_ctrl_cmd(ADE_SCL3_CTRL_REG, pcomp_info);
    ade_cmdq_wr_scl_hsp_cmd(ADE_SCL3_HSP_REG, &(pcomp_info->wifi_src_rect), &(pcomp_info->wifi_dst_rect));
    /*
     * yuv420: 240x320 scl to 200x200, means:
     * y: 240 scl to 200, 320 scl to 200,
     * uv: 120 scl to 200, 160 scl to 200.
     */
    scl3_uv_irect.x = 0;
    scl3_uv_irect.y = 0;
    if (scl_iformat == ADE_SCL_YUV420) {
        scl3_uv_irect.w = pcomp_info->wifi_dst_rect.w / 2;
        scl3_uv_irect.h = pcomp_info->wifi_dst_rect.h / 2;
        ade_cmdq_wr_scl_hsp_cmd(ADE_SCL3_UV_HSP_REG, &(pcomp_info->wifi_src_rect), &(scl3_uv_irect));
    } else if (scl_iformat == ADE_SCL_YUV422) {
        scl3_uv_irect.w = pcomp_info->wifi_dst_rect.w / 2;
        scl3_uv_irect.h = pcomp_info->wifi_dst_rect.h;
        ade_cmdq_wr_scl_hsp_cmd(ADE_SCL3_UV_HSP_REG, &(pcomp_info->wifi_src_rect), &(scl3_uv_irect));
    } else if (scl_iformat == ADE_SCL_YUV444) {
        scl3_uv_irect.w = pcomp_info->wifi_dst_rect.w;
        scl3_uv_irect.h = pcomp_info->wifi_dst_rect.h;
        ade_cmdq_wr_scl_hsp_cmd(ADE_SCL3_UV_HSP_REG, &(pcomp_info->wifi_src_rect), &(scl3_uv_irect));
    } else {
        ade_cmdq_wr_cmd(ADE_SCL3_UV_HSP_REG, 0);
    }

    ade_cmdq_wr_scl_vsp_cmd(ADE_SCL3_VSP_REG, &(pcomp_info->wifi_src_rect), &(pcomp_info->wifi_dst_rect));

    if (scl_iformat != ADE_SCL_ARGB888) {
        ade_cmdq_wr_scl_vsp_cmd(ADE_SCL3_UV_VSP_REG, &(pcomp_info->wifi_src_rect), &(scl3_uv_irect));
    } else {
        ade_cmdq_wr_cmd(ADE_SCL3_UV_VSP_REG, 0);
    }

    ade_cmdq_wr_cmd(ADE_SCL3_ORES_REG, (pcomp_info->wifi_dst_rect.h - 1) << 16 | (pcomp_info->wifi_dst_rect.w - 1));
    ade_cmdq_wr_cmd(ADE_SCL3_IRES_REG, (pcomp_info->wifi_src_rect.h - 1) << 16 | (pcomp_info->wifi_src_rect.w - 1));/* input size */
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_scl_hcoef_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_SCL3_HCOEF_0_REG, &(pcomp_info->wifi_src_rect), &(pcomp_info->wifi_dst_rect));
    ade_cmdq_wr_scl_vcoef_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_SCL3_VCOEF_0_REG, &(pcomp_info->wifi_src_rect), &(pcomp_info->wifi_dst_rect));

    /* four wr cmd */
    ade_cmdq_wr_cmd(ADE_SCL3_START_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

void ade_overlay_commit_set_disp_ch(struct ade_compose_data_type *ade_pri_data, struct overlay_compose_info *pcomp_info, struct cmdfile_buffer *cf_buff)
{
    u32     format;
    u32     stride;
    struct  ade_rect disp_rect;
    struct overlay_region_info pipe_info;
    struct balong_fb_data_type *balongfd = NULL;

    balongfd  = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);

    BUG_ON((pcomp_info == NULL) || (cf_buff == NULL) ||(balongfd == NULL));

    format = pcomp_info->offline_dst_format;
    if (((format > ADE_BGR_888) && (format < ADE_YUYV_I))
            || (format > ADE_YUV444)) {
        balongfb_loge("offline_dst_format  = %d is not support \n", pcomp_info->offline_dst_format);
        return;
    }

    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 6, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 6);

    balongfb_logi_display_debugfs("ade_overlay_commit_set_disp_ch: disp_ch rdma soft rest sel bit = 6 \n");
    balongfb_logi_display_debugfs("cf_buff->vaddr=0x%p, cf_buff->cmd_len=%d \n", cf_buff->vaddr, cf_buff->cmd_len);

    stride = ade_pri_data->xres * ade_overlay_format2bpp(format);

    /* set disp channel into AXI0 */
    set_TOP_DMA_AXI_MUX(ade_pri_data->ade_base, OVERLAY_PIPE_ADE_DISP, OVERLAY_PIPE_TYPE_ONLINE);

    ade_cmdq_wr_rdma_pe_cmd(RD_CH_DISP_PE_REG, OVERLAY_PIPE_TYPE_ONLINE, ADE_ROT_NOP);
    ade_cmdq_wr_cmd(RD_CH_DISP_CTRL_REG, format << 16);

#if PARTIAL_UPDATE
    pcomp_info->offline_frame_phy_addr = pcomp_info->offline_frame_phy_addr
                                       + stride * balongfd->yoffset + balongfd->xoffset * ade_overlay_format2bpp(format);
    ade_cmdq_wr_cmd(RD_CH_DISP_ADDR_REG, pcomp_info->offline_frame_phy_addr);
    ade_cmdq_wr_cmd(RD_CH_DISP_SIZE_REG, (balongfd->yheight << 16) | (balongfd->xwidth * ade_overlay_format2bpp(format)));
    ade_cmdq_wr_cmd(RD_CH_DISP_STRIDE_REG, stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_SPACE_REG, balongfd->yheight * stride);
    disp_rect.x = balongfd->xoffset;
    disp_rect.y = balongfd->yoffset;
    disp_rect.w = balongfd->xwidth;
    disp_rect.h = balongfd->yheight;
#else
    ade_cmdq_wr_cmd(RD_CH_DISP_ADDR_REG, pcomp_info->offline_frame_phy_addr);
    ade_cmdq_wr_cmd(RD_CH_DISP_SIZE_REG, (ade_pri_data->yres << 16) | stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_STRIDE_REG, stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_SPACE_REG, ade_pri_data->yres * stride);
    disp_rect.x = 0;
    disp_rect.y = 0;
    disp_rect.w = ade_pri_data->xres;
    disp_rect.h = ade_pri_data->yres;
#endif

    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    ade_cmdq_wr_cmd(RD_CH_DISP_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* lock the last offline frame handle, will lock the handle once at each frame */
    if (pcomp_info->offline_frame_share_fd != 0) {
        pipe_info.ch_num = OVERLAY_PIPE_ADE_DISP;
        pipe_info.phy_addr = pcomp_info->offline_frame_phy_addr;
        pipe_info.share_fd = pcomp_info->offline_frame_share_fd;
        ade_overlay_handle_lock(ade_pri_data, &pipe_info);
    }

    /* ctrans 5 */
    ade_overlay_commit_set_ctran5(ade_pri_data, disp_rect, format, cf_buff);
}

#if WFD_SUPPORT

void ade_overlay_commit_set_wfd_ovly(struct  overlay_region_info *pipe_info, struct ade_overlay_ctrl *overlay_ctl, struct cmdfile_buffer *cf_buf)
{
    u32                                  surf_num;
    u32                                  xy1, xy0;
    u32                                  i;
    u32                                  output_size;
    struct ade_overlay_ovly              *ovly;
    struct ade_rect                      ovly_region_rect;
    struct ovly_reg                      *ovly_reg;

    struct ovly_ctrl_info                ch_ctrl_info;

    BUG_ON((pipe_info == NULL) || (overlay_ctl == NULL));

    memcpy(&ovly_region_rect,&(pipe_info->dst_rect), sizeof(struct ade_rect));
    ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_1]);
    //ovly1_res.ul32 = ovly->ovly_cap;
    ovly_reg = &ovly->ovly_reg;

    for (i = 0; i < OVERLAY_ADE_SURFACE_MAX; i++) {
        if ((overlay_ctl->ovly1_ch_used & (1 << i)) == 0) {
            ade_cmdq_wr_cmd(ovly->ovly_reg.ovly_ch_ctrl[i], 0x11);
            ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
            balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch:disable unused channel index: %d \n", i);
        }
    }
    overlay_ctl->ovly1_ch_used = 0;

    surf_num  = pipe_info->surface_num;

    overlay_ctl->ovly1_ch_used |= 1 << surf_num;

    //set_TOP_OVLY_CTRL(ovly->ade_base, ovly_reg->ovly_ch_sel_start[surf_num], 1);

    /* xy0 */
    xy0 = ((ovly_region_rect.x - pipe_info->ovly_output_rect.x) << 16) | (ovly_region_rect.y - pipe_info->ovly_output_rect.y);
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy0[surf_num], xy0);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch:xy0 = 0x%x \n",xy0);

    /* xy1 */
    xy1 = ((ovly_region_rect.x - pipe_info->ovly_output_rect.x + ovly_region_rect.w - 1) << 16)  \
          | (ovly_region_rect.y - pipe_info->ovly_output_rect.y + ovly_region_rect.h - 1);
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy1[surf_num], xy1);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch:xy1 = 0x%x \n",xy1);

    /* output size */
    output_size = ((pipe_info->ovly_output_rect.w -1) << 16) | (pipe_info->ovly_output_rect.h - 1);
    ade_cmdq_wr_cmd(ovly_reg->ovly_output_size, output_size);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd:output_size = 0x%x \n",output_size);
    balongfb_logi_display_debugfs("ade_overlay_region_set_ovly_cmd:pipe_info->ch_num = %d \n",pipe_info->ch_num);

    /* ovly_ch ctrl */
    ch_ctrl_info.ch_num = OVERLAY_PIPE_ADE_CH6;
    ch_ctrl_info.format = ADE_RGBA_8888;
    ch_ctrl_info.ovly_num = 0;
    ch_ctrl_info.blending = OVERLAY_BLENDING_NONE;
    ch_ctrl_info.dim_blending = 0;
    ch_ctrl_info.sec_ovly_ch_count = 0;
    ch_ctrl_info.ovly_type = ADE_OVLY_TYPE_FIRST_OVLY;//ADE_OVLY_TYPE_BYPASS;
    ade_cmdq_wr_ovly_ch_ctrl_cmd(ovly_reg->ovly_ch_ctrl[surf_num], ch_ctrl_info);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));

    ade_cmdq_wr_cmd(ovly_reg->ovly_ctrl, 1);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
}

int  ade_overlay_commit_set_wfd_ch(struct ade_compose_data_type *ade_pri_data, struct overlay_compose_info *pcomp_info, struct cmdfile_buffer *cf_buf)
{
    u32                         w, h, x, y ;
    u32                         tmp;
    u32                         wifiHeight, wifiWidth, srcWidth, srcHeight;
    u32                         hratio, wratio, ratio;
    u32                         bpp;
    bool                        need_fmc;
    volatile U_WR_CH1_CTRL      wr_ch_ctrl;
    struct ade_overlay_pipe     *pipe;
    struct ade_overlay_ctrl     *overlay_ctl;
    struct overlay_region_info  *pipe_info;
    struct ch_unit_reg          *unit_reg;
    struct ade_overlay_rot      *rot_info = NULL;

    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch: begin \n");

    BUG_ON((pcomp_info == NULL) || (cf_buf == NULL));
    pipe = &(ade_pri_data->overlay_ctl.pipe_list[OVERLAY_PIPE_ADE_CH6]);

    unit_reg = &(pipe->pipe_reg);

    if (pcomp_info->offline_frame_phy_addr == 0) {
        balongfb_loge("ade_overlay_commit_set_wfd_ch: offline_frame_phy_addr is 0 \n");
        return -EINVAL;
    }
    overlay_ctl = &(ade_pri_data->overlay_ctl);

    /* calc new x,y, w,h */
    wifiWidth  = pcomp_info->wifi_dst_rect.w;
    wifiHeight = pcomp_info->wifi_dst_rect.h;
    srcWidth   = pcomp_info->wifi_src_rect.w;
    srcHeight  = pcomp_info->wifi_src_rect.h;
    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch: wifi_rotation = %d \n", pcomp_info->wifi_rotation);
    if ((ADE_ROT_90 == pcomp_info->wifi_rotation || ADE_ROT_270 == pcomp_info->wifi_rotation)) {
        tmp       = srcWidth;
        srcWidth  = srcHeight;
        srcHeight = tmp;
    }

    hratio = wifiHeight * 10000 / srcHeight;
    wratio = wifiWidth * 10000 / srcWidth;
    ratio  = hratio > wratio ? wratio : hratio;

    w = (int)(srcWidth * ratio)/10000;
    h = (int)(srcHeight * ratio)/10000;
    x = (wifiWidth - w) / 2;
    y = (wifiHeight - h) / 2;

    /* config pipe_info for wifi display */
    pipe_info  = &(pipe->pipe_info);
    memset(pipe_info, 0, sizeof(struct overlay_region_info));
    pipe_info->ch_type     = OVERLAY_PIPE_TYPE_OFFLINE;
    pipe_info->ch_num      = OVERLAY_PIPE_ADE_CH6;
    pipe_info->surface_num = OVERLAY_ADE_SURFACE_1;
    pipe_info->phy_addr    = pcomp_info->offline_frame_phy_addr;
    pipe_info->format      = pcomp_info->offline_dst_format;
    pipe_info->rotation    = pcomp_info->wifi_rotation;
    pipe_info->stride      = pcomp_info->offline_stride;
    pipe_info->width       = pcomp_info->wifi_src_rect.w;
    pipe_info->height      = pcomp_info->wifi_src_rect.h;
    pipe_info->dst_width   = w;
    pipe_info->dst_height  = h;

    pipe_info->dst_rect.x  = x;
    pipe_info->dst_rect.y  = y;
    pipe_info->dst_rect.w  = w;
    pipe_info->dst_rect.h  = h;
    pipe_info->blending    = OVERLAY_BLENDING_NONE;

    memcpy(&pipe_info->ovly_output_rect, &pcomp_info->wifi_dst_rect, sizeof(struct ade_rect));

    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch: phy_addr = 0x%04x \n",  pipe_info->phy_addr);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch: scl x = %d, y = %d \n",  x, y);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch: scl w = %d, h = %d \n",  w, h);


    /********************rdma 6*********************/
    rot_info = &(ade_pri_data->overlay_ctl.rot_info);
    ade_overlay_region_set_rdma_cmd(NULL, pipe, rot_info, cf_buf);

    /********************clip 6*********************/
    ade_overlay_region_set_clip_cmd(ade_pri_data, pipe, cf_buf);

    /*********************scl**********************/
    ade_overlay_region_set_scl_cmd(ade_pri_data, pipe, cf_buf);

    /*********************ctran********************/
    ade_overlay_region_set_ctran_cmd(ade_pri_data, pipe, cf_buf);

    /*********************ovly*********************/
    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch:  cfg ovly \n");
    ade_overlay_commit_set_wfd_ovly(pipe_info, overlay_ctl, cf_buf);

    /********************after ctran3 ****************/
    need_fmc = ade_overlay_need_rgb2yuv(pcomp_info->wifi_dst_format);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch: need_fmc=%d \n", need_fmc);

    /* CTRAN3 enable or disable */
    if (!need_fmc) {
        /* bypass */
        ade_cmdq_wr_cmd(ADE_CTRAN3_DIS_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
        ade_cmdq_wr_cmd(ADE_CTRAN3_IMAGE_SIZE_REG,pcomp_info->wifi_dst_rect.w* pcomp_info->wifi_dst_rect.h - 1);
        ade_cmdq_wr_cmd(ADE_CTRAN3_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
    }
    else {
        ade_cmdq_wr_cmd(ADE_CTRAN3_DIS_REG, 0);
        ade_cmdq_wr_cmd(ADE_CTRAN3_MODE_CHOOSE_REG, 1);  /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv,ARGB888 to yuv422 */
        ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));

        ade_cmdq_wr_ctran_chdc_cmd(ADE_CTRAN3_CHDC0_REG, 1);
        ade_cmdq_wr_ctran_csc_cmd(ADE_CTRAN3_CSC0_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));

        ade_cmdq_wr_cmd(ADE_CTRAN3_IMAGE_SIZE_REG, pcomp_info->wifi_dst_rect.w* pcomp_info->wifi_dst_rect.h - 1);
        ade_cmdq_wr_cmd(ADE_CTRAN3_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
    }

    /********************wdma1 ********************/
    wr_ch_ctrl.u32 = 0;
    wr_ch_ctrl.bits.wr_ch1_partial =0; /* first h, then v */
    wr_ch_ctrl.bits.wr_ch1_wr_dir  = 0;
    wr_ch_ctrl.bits.wr_ch1_fsh_int_disable  = 0;
    wr_ch_ctrl.bits.wr_ch1_format  = pcomp_info->wifi_dst_format;

    bpp = ade_overlay_format2bpp(pcomp_info->wifi_dst_format);

    ade_cmdq_wr_cmd(WR_CH1_CTRL_REG, wr_ch_ctrl.u32);

    ade_cmdq_wr_cmd(WR_CH1_ADDR_REG, pcomp_info->wifi_phy_addr);
    ade_cmdq_wr_cmd(WR_CH1_SIZE_REG, (pcomp_info->wifi_dst_rect.h << 16) | (pcomp_info->wifi_dst_rect.w * bpp));
    ade_cmdq_wr_cmd(WR_CH1_STRIDE_REG, pcomp_info->wifi_stride);
    ade_cmdq_wr_cmd(WR_CH1_SPACE_REG, pcomp_info->wifi_dst_rect.h * pcomp_info->wifi_stride);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));

    ade_cmdq_wr_cmd(WR_CH1_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));

    balongfb_logi_display_debugfs("ade_overlay_commit_set_wfd_ch: end \n");
    return 0;

}

void ade_overlay_commit_set_wfd(struct ade_compose_data_type *ade_pri_data,
        struct overlay_compose_info  *pcomp_info,
        struct cmdfile_buffer *cf_buff)
{
    u32 ret;

    //config wifi display
    if (pcomp_info->wifi_phy_addr != 0) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_offline:wifi_phy_addr = 0x%04x \n", pcomp_info->wifi_phy_addr);

        //ade_cmdq_wr_wait_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_WAIT_EVT_WDMA1_CMPL);
        ret = ade_overlay_commit_set_wfd_ch(ade_pri_data, pcomp_info, cf_buff);
        if (ret != 0) {
            balongfb_loge("ade_overlay_commit_set_wfd:fail !!! \n");
            return;
        }

        ade_cmdq_wr_wait_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_WAIT_EVT_OVLY1_CMPL);
        ade_cmdq_wr_cmd(INTR_SOFT_CMDQ2_1_REG, ADE_WAIT_EVT_OVLY1_CMPL);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    }
}


#endif

void ade_overlay_commit_set_part_update(struct overlay_compose_info *pcomp_info,
        struct ade_compose_data_type *ade_pri_data,
        struct cmdfile_buffer *cf_buff)
{
    u32     format;
    u32     stride;
    u32     bpp;

    BUG_ON(pcomp_info == NULL);
    BUG_ON(ade_pri_data == NULL);
    BUG_ON(cf_buff == NULL);

    balongfb_logi_display_debugfs("ade_overlay_commit_set_part_update: enter succ ! \n");

    if (ade_pri_data->wdma2_phy == 0) {
        return;
    }

    format = ade_pri_data->fb_imgType;
    if (((format > ADE_BGR_888) && (format < ADE_YUYV_I))
            || (format > ADE_YUV444)) {
        balongfb_loge("ade_pri_data->fb_imgType  = %d is not support \n", ade_pri_data->fb_imgType);
    }

    bpp = ade_overlay_format2bpp(format);
    stride = ade_pri_data->xres * bpp;

    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 6, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 6);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_part_update: disp_ch rdma soft rest sel bit = 6 \n");

    ade_cmdq_wr_cmd(RD_CH_DISP_CTRL_REG, format << 16);
    ade_cmdq_wr_cmd(RD_CH_DISP_ADDR_REG, ade_pri_data->wdma2_phy);
    ade_cmdq_wr_cmd(RD_CH_DISP_SIZE_REG, (ade_pri_data->yres << 16) | stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_STRIDE_REG, stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_SPACE_REG, ade_pri_data->yres * stride);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* blank block */
    ade_cmdq_wr_cmd(RD_CH_DISP_BLANK_OFFSET_REG, (pcomp_info->dst_rect.y << 16) | (pcomp_info->dst_rect.x * bpp));
    ade_cmdq_wr_cmd(RD_CH_DISP_BLANK_SIZE_REG, pcomp_info->dst_rect.h << 16 | (pcomp_info->dst_rect.w * bpp));
    ade_cmdq_wr_cmd(RD_CH_DISP_BLANK_SPACE_REG, pcomp_info->dst_rect.h * (pcomp_info->dst_rect.w * bpp));
    ade_cmdq_wr_cmd(RD_CH_DISP_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* ctrans5 */
    ade_overlay_commit_set_ctran5(ade_pri_data, pcomp_info->dst_rect, format, cf_buff);

    /* set ovly2 for partial update */
    ade_overlay_commit_set_ovly2_ch1(pcomp_info, ade_pri_data, cf_buff);
}

/***************** COMMIT CFG END ******************************************/

/****************  CMD QUEUE CFG BEGIN *******************/
static void ade_overlay_commit_set_offline_cmdq(u8* ade_base, struct cmdfile_buffer  *cf_buff,u32 pattern_num)
{
    balongfb_logi_display_debugfs("ade_overlay_commit_set_offline_cmdq enter succ, paddr=0x%x \n", cf_buff->paddr);

    /* cfg reg to ADE TOP REG */
    set_CMDQ_RDMA2_PE(ade_base, g_ade_offline_pe);
    set_CMDQ_RDMA2_CTRL(ade_base, 0);
    set_CMDQ_RDMA2_ADDR(ade_base, cf_buff->paddr);
    set_CMDQ_RDMA2_LEN(ade_base, cf_buff->cmd_len / ADE_CMD_ALIGN_BYTE_LEN);
    //set_CMDQ_RDMA2_EN(ade_base, 1);

#if ADE_DEBUG_LOG_ENABLE
    if (g_command_file_wr_enable) {

        /* default is save cmdfile to file */
        ade_cmdq_wr_cmdbuff2file(cf_buff, false, pattern_num);
    }
#endif
}
static void ade_overlay_commit_set_online_cmdq(u8* ade_base, struct cmdfile_buffer  *cf_buff, u32 pattern_num)
{
    balongfb_logi_display_debugfs("ade_overlay_commit_set_online_cmdq enter succ, paddr=0x%x \n", cf_buff->paddr);

    /* cfg reg to ADE TOP REG */
    set_CMDQ_RDMA1_PE(ade_base, g_ade_online_pe);
    set_CMDQ_RDMA1_CTRL(ade_base, 0);
    set_CMDQ_RDMA1_ADDR(ade_base, cf_buff->paddr);
    set_CMDQ_RDMA1_LEN(ade_base, cf_buff->cmd_len / ADE_CMD_ALIGN_BYTE_LEN);

#if ADE_DEBUG_LOG_ENABLE
    if (g_command_file_wr_enable && (pattern_num != ADE_INVAL_PATTERN_NUM)) {

        /* default is save cmdfile to file */
        ade_cmdq_wr_cmdbuff2file(cf_buff, true, pattern_num);
    }
#endif

}

static void ade_overlay_commit_set_online_cmd_mode(struct ade_compose_data_type *ade_pri_data,
        struct balong_fb_data_type *fb_data,
        struct overlay_compose_info  *pcomp_info,
        struct cmdfile_buffer *cf_buff)
{
    u8* ade_base;

    ade_base = ade_pri_data->ade_base;

    set_TOP_SOFT_RST_SEL0_cmdq1_rdma(ade_base, 0); // cmdq_rdma1
    set_TOP_RELOAD_DIS0_cmdq1_rdma(ade_base, 1);

    /* 6: init cmdq_rdma1 */
    ade_overlay_commit_set_online_cmdq(ade_base, cf_buff, pcomp_info->compose_pattern_num);

    /* 7: if it is not the first frame, need enable res switch cmpl intr */
    if (wait_event_interruptible_timeout(fb_data->frame_wq, fb_data->update_frame, HZ) <= 0) {
        balongfb_loge("wait_event_interruptible_timeout update_frame !\n");
        ade_overlay_dump_ade_top_reg(ade_base);
        outp32(ade_base + ADE_SOFT_RST0_REG, 0xffffffff);
        outp32(ade_base + ADE_SOFT_RST1_REG, 0xffffffff);
    }
    fb_data->update_frame = 0;
    ade_pri_data->res_switch_cmpl = 0;
    set_TOP_ADE_EN(ade_base, ADE_TRUE);

    if (wait_event_interruptible_timeout(ade_pri_data->wait_res_cmpl, ade_pri_data->res_switch_cmpl, (HZ/4)) <= 0) {
        balongfb_loge("ade_overlay_commit_set_online_cmd_mode:wait_event_interruptible_timeout wait_res_cmpl timeout!\n");
        ade_overlay_dump_ade_top_reg(ade_base);
        outp32(ade_base + ADE_SOFT_RST0_REG, 0xffffffff);
        outp32(ade_base + ADE_SOFT_RST1_REG, 0xffffffff);

        set_LDI_DSI_CMD_MOD_CTRL(0x0);
        mdelay(1);
        set_LDI_DSI_CMD_MOD_CTRL(0x2);
    }
    ade_pri_data->res_switch_cmpl = 0;

    set_CMDQ_RDMA1_EN(ade_base, ADE_ENABLE);

}

static void ade_overlay_commit_set_online_video_mode(struct ade_compose_data_type *ade_pri_data,
        struct balong_fb_data_type *fb_data,
        struct overlay_compose_info  *pcomp_info,
        struct cmdfile_buffer *cf_buff)
{
    u8* ade_base;

    ade_base = ade_pri_data->ade_base;

    set_TOP_SOFT_RST_SEL0_cmdq1_rdma(ade_base, 1); // cmdq_rdma1
    set_TOP_RELOAD_DIS0_cmdq1_rdma(ade_base, 0);

    if (ade_pri_data->have_refresh && (OVERLAY_COMP_TYPE_HYBRID == pcomp_info->compose_mode)) {
        set_TOP_DISP_SRC_CFG(ade_base, ade_pri_data->overlay_ctl.ade_mux_list[TOP_MUX_DISP_SRC].mux_src);
    } else {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_online_video_mode:ade_pri_data->have_refresh= %d, pcomp_info->compose_mode=%d \n", ade_pri_data->have_refresh, pcomp_info->compose_mode);
    }

    /* 6: init cmdq_rdma1 */
    ade_overlay_commit_set_online_cmdq(ade_base, cf_buff, pcomp_info->compose_pattern_num);
    ade_pri_data->res_switch_cmpl = 0;
    set_TOP_ADE_EN(ade_base, ADE_TRUE);

    if (wait_event_interruptible_timeout(ade_pri_data->wait_res_cmpl, ade_pri_data->res_switch_cmpl, (HZ/4)) <= 0) {
        balongfb_loge("wait res_switch_cmpl timeout!,fb_data->panle_power_on=%d,ade_core_power_on=%d\n",fb_data->panel_power_on, fb_data->ade_core_power_on);
        balongfb_loge("frame_count=%d, frame_number=%d\n",ade_pri_data->frame_count, pcomp_info->frame_number);
        ade_overlay_dump_ade_top_reg(ade_base);

        outp32(ade_base + ADE_SOFT_RST0_REG, 0xffffffff);
        outp32(ade_base + ADE_SOFT_RST1_REG, 0xffffffff);

        set_LDI_DSI_CMD_MOD_CTRL(0x0);
        mdelay(1);
        set_LDI_DSI_CMD_MOD_CTRL(0x2);
    }
    ade_pri_data->res_switch_cmpl = 0;

    /* modify frm_end_start to 1, beacause blk rot will modify it to 2 */
    set_TOP_CTL_frm_end_start(ade_base, 1);
}

void ade_overlay_commit_set_rotation(struct ade_overlay_ctrl     *overlay_ctl,
        struct overlay_compose_info  *pcomp_info,
        struct cmdfile_buffer        *cf_buff,
        u8*  ade_base)
{
    struct ade_overlay_rot       *rot_info = NULL;

    if ((overlay_ctl == NULL) || (pcomp_info == NULL) || (cf_buff == NULL)) {
        balongfb_loge("overlay_ctl or pcomp_info or cf_buff is null \n");
        return;
    }

    rot_info = &overlay_ctl->rot_info;
    if ((rot_info->is_configed == ADE_TRUE) || (0 == cf_buff->cmd_len)) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_rotation: rot_info->is_configed is true,or cmdfile len is 0 \n");
        return;
    }

    rot_info->rotation   = pcomp_info->dst_rotation;
    rot_info->rot_width  = pcomp_info->rot_width;
    rot_info->rot_height = pcomp_info->rot_height;

    balongfb_logi_display_debugfs("ade_overlay_commit_set_rotation:rot_info->rotation=%d \n",rot_info->rotation);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_rotation:rot_info->rot_width=%d \n",rot_info->rot_width);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_rotation:rot_info->rot_height=%d \n",rot_info->rot_height);

    if (ade_overlay_set_rot_cmd(ade_base, rot_info, cf_buff) < 0) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_rotation: ade_overlay_set_rot_cmd error \n");
        return;
    }

    balongfb_logi_display_debugfs("ade_overlay_commit_set_rotation: rot_rst_sel_bit = 30 \n");
    rot_info->is_configed = ADE_FALSE;

    return;
}

static void ade_overlay_commit_set_scl2(struct ade_compose_data_type *ade_pri_data,
        struct ade_overlay_ctrl      *overlay_ctl,
        struct overlay_compose_info  *pcomp_info,
        struct cmdfile_buffer        *cf_buff,
        u8*  ade_base)
{
    struct ade_overlay_scl2      *scl2_info = NULL;

    scl2_info = &(overlay_ctl->scl2_info);

    /* scl2 is not used by channel */
    //if ((ADE_SCL2_STATE_WAIT_CFG != scl2_info->scl2_state) || (cf_buff->cmd_len == 0)) {
    if (ADE_SCL2_STATE_WAIT_CFG != scl2_info->scl2_state) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_scl2: scl2_info->scl2_state=%d,ADE_SCL2_STATE_WAIT_CFG=%d \n", scl2_info->scl2_state, ADE_SCL2_STATE_WAIT_CFG);
        return;
    }

    scl2_info->scl2_iformat = ADE_SCL_ARGB888;
    scl2_info->dst_rect = pcomp_info->scl2_dst_rect;
    scl2_info->src_rect = pcomp_info->scl2_src_rect;

    /* set scl2, and transform scl2_state wait_cfg to cfg_succ */
    ade_overlay_set_scl2_cmd(ade_base, scl2_info, cf_buff);

    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 22, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 22);

    balongfb_logi_display_debugfs("ade_overlay_commit: scl2_rst_sel_bit = 22 \n");

    return;
}

void ade_overlay_commit_set_sec_ovly(struct ade_overlay_ctrl      *overlay_ctl,
        struct overlay_resource_info *ade_res_info,
        struct ade_overlay_ovly      *sec_ovly,
        struct overlay_compose_info  *pcomp_info,
        struct cmdfile_buffer        *cf_buff)
{
    struct  ovly_info*   ovly_comp = NULL;
    int    i = 0;
    u32    ovly2_max = 0;
    u32    ovly1_max = 0;
    u32    ovly1_min = 0;


    if ((cf_buff == NULL) || (sec_ovly == NULL) || (pcomp_info == NULL)
            || (overlay_ctl == NULL) || (ade_res_info == NULL)) {
        balongfb_loge("cf_buff or sec_ovly or pcomp_info is null ptr \n");
        return;
    }

    ovly_comp = &sec_ovly->ovly_comp;

    if (cf_buff->cmd_len <= 0) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: cf_buff->cmd_len = 0 \n");
        return;
    }

    /* scl2->ovly2(ovly1) or rot->ovly2 decide the ovly2 input.
     * ovly2->rot or ovly2(ovly1)->scl2 decide the ovly2 output.
     */
    if ((ade_res_info->scl2_res.bits.has_ovly1 && (OVERLAY_OVLY_1 == sec_ovly->ovly_num))
            || (ade_res_info->scl2_res.bits.has_ovly2 && (OVERLAY_OVLY_2 == sec_ovly->ovly_num))) {
        memcpy(&ovly_comp->input_rect, &pcomp_info->scl2_dst_rect, sizeof(struct ade_rect));

        /* must set input_rect.y to 0, otherwise y offset of overlay2 is too big. */
        ovly_comp->input_rect.y = 0;
    }

    if ((ade_res_info->rot_res.bits.has_ovly1 && (OVERLAY_OVLY_1 == sec_ovly->ovly_num))
            || (ade_res_info->rot_res.bits.has_ovly2 && (OVERLAY_OVLY_2 == sec_ovly->ovly_num))) {
        if ((ADE_ROT_NOP == pcomp_info->dst_rotation)
                || (ADE_ROT_180 == pcomp_info->dst_rotation)) {
            ovly_comp->input_rect.w = pcomp_info->rot_width;
            ovly_comp->input_rect.h = pcomp_info->rot_height;
        } else {
            ovly_comp->input_rect.w = pcomp_info->rot_height;
            ovly_comp->input_rect.h = pcomp_info->rot_width;
        }
    }

    /* ovly2->scl2 */
    if ((OVERLAY_OVLY_2 == sec_ovly->ovly_num) && ade_res_info->ovly2_res.bits.has_scl2) {
        memcpy(&ovly_comp->output_rect, &pcomp_info->src_rect, sizeof(struct ade_rect));
    }

    /* ovly2->rot */
    if ((OVERLAY_OVLY_2 == sec_ovly->ovly_num)
            && (ade_res_info->ovly2_res.bits.has_720p_rot || ade_res_info->ovly2_res.bits.has_1080p_rot)) {
        ovly_comp->output_rect.w = pcomp_info->rot_width;
        ovly_comp->output_rect.h = pcomp_info->rot_height;
    }

    balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: sec_ovly num=%d \n", sec_ovly->ovly_num);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: ovly_comp->ch_count=%d \n", ovly_comp->ch_count);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: input_rect.x=%d, input_rect.y=%d \n", ovly_comp->input_rect.x, ovly_comp->input_rect.y);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: input_rect.w=%d, input_rect.h=%d  \n", ovly_comp->input_rect.w, ovly_comp->input_rect.h);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: output_rect.x=%d, output_rect.y=%d \n", ovly_comp->output_rect.x, ovly_comp->output_rect.y);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: output_rect.w=%d, output_rect.h=%d \n", ovly_comp->output_rect.w, ovly_comp->output_rect.h);

    if (ovly_comp->ch_count > 1) {
        /* enable overlay 3 */
        ade_cmdq_wr_cmd(ADE_OVLY3_CTL_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: ovly_num=2, ovly_ctrl start enable here\n", sec_ovly->ovly_num);

        /* the second ovly */
        ade_overlay_set_sec_ovly(overlay_ctl, sec_ovly, cf_buff);
    }

    /* must disable unused channel of overlay, just clean online ovly_ch,
     * channel of ovly1's don't clean, we will clean the TOP ovly_ctrl for ovly1
     */
    if (OVERLAY_OVLY_2 == sec_ovly->ovly_num) {
        if (OVERLAY_COMP_TYPE_ONLINE == pcomp_info->compose_mode) {
            ovly2_max = OVERLAY_ADE_SURFACE_MAX;
        } else {
            ovly2_max = OVERLAY_ADE_SURFACE_5;
        }

        for (i = 0; i < ovly2_max; i++) {
            if ((overlay_ctl->ovly2_ch_used & (1 << i)) == 0) {
                ade_cmdq_wr_cmd(sec_ovly->ovly_reg.ovly_ch_ctrl[i], 0x11);
                ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
                balongfb_logi_display_debugfs("ade_overlay_commit:disable unused channel index: %d \n", i);
            }
        }
        overlay_ctl->ovly2_ch_used = 0;
    }

    if (OVERLAY_OVLY_1 == sec_ovly->ovly_num) {
        if (OVERLAY_COMP_TYPE_OFFLINE == pcomp_info->compose_mode) {
            ovly1_max = OVERLAY_ADE_SURFACE_MAX;
            ovly1_min = 0;
        } else {
            ovly1_max = OVERLAY_ADE_SURFACE_8;
            ovly1_min = OVERLAY_ADE_SURFACE_5;
        }
        for (i = ovly1_min; i < ovly1_max; i++) {
            if ((overlay_ctl->ovly1_ch_used & (1 << i)) == 0) {
                ade_cmdq_wr_cmd(sec_ovly->ovly_reg.ovly_ch_ctrl[i], 0x11);
                ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
                balongfb_logi_display_debugfs("ade_overlay_commit:disable unused channel index: %d \n", i);
            }
        }
        overlay_ctl->ovly1_ch_used = 0;
    }

    /* enable ovly */
    ade_cmdq_wr_cmd(sec_ovly->ovly_reg.ovly_ctrl, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    balongfb_logi_display_debugfs("ade_overlay_commit_set_sec_ovly: ovly_num=%d, ovly_ctrl start enable here!!! \n", sec_ovly->ovly_num);

}

void ade_overlay_commit_set_offline(struct ade_compose_data_type *ade_pri_data,
        struct commit_comp_unit *commit_uint,
        struct overlay_compose_info  *pcomp_info,
        struct cmdfile_buffer *cf_buff)
{
    u8*     ade_base;
    BUG_ON(commit_uint == NULL);
    BUG_ON(pcomp_info == NULL);
    BUG_ON(ade_pri_data == NULL);

    balongfb_logi_display_debugfs("ade_overlay_commit_set_offline enter  succ \n");
    if (cf_buff == NULL) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_offline: offline cf_buff is null \n");
        return;
    }

    if (OVERLAY_COMP_TYPE_ONLINE == pcomp_info->compose_mode) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_offline: compose mode is OVERLAY_COMP_TYPE_ONLINE, can't config offline \n");
        return;
    }

    if (0 == cf_buff->cmd_len) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_offline: offline cf_buff len is 0 \n");
        return;
    }

    /* if don't need to config wdma1,must return */
    if (!commit_uint->bit_wdma1) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_offline: bit wdma1 is false \n");
        return;
    }

    if ((pcomp_info->offline_dst_rect.w == 0)
            || (pcomp_info->offline_dst_rect.h == 0)
            || (pcomp_info->offline_phy_addr == 0)) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_offline: offline dst rect w or h is 0 \n");
        return;
    }

    if ((ADE_NV12 == pcomp_info->offline_dst_format) || (ADE_NV21 == pcomp_info->offline_dst_format)) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_offline: offline format is error \n");
        return;
    }

    /*
     * if commit interface take offline info, we must config the offline WDMA.
     * no matter commit once or twice
     */
    ade_base = ade_pri_data->ade_base;

    /* config wdma1 */
    ade_overlay_commit_set_wdma1(pcomp_info, cf_buff);

    /* set ctran4, if bit_scl3 = 1, rgb888->yuv444,
     * set ctran4, if bit_scl3 = 0, rgb888->yuv422,
     */
    if (commit_uint->bit_offline_wdma3) {
        ade_overlay_commit_set_ctran4(ade_pri_data, pcomp_info, cf_buff, commit_uint->bit_scl3, ADE_FALSE);
        if (commit_uint->bit_scl3) {
            ade_overlay_commit_set_scl3(ade_pri_data, pcomp_info, cf_buff, ADE_FALSE);
        }
        ade_overlay_commit_set_wdma3(ade_pri_data, pcomp_info, cf_buff, commit_uint->bit_scl3, ADE_FALSE);
    }

    if (pcomp_info->is_finished == ADE_FALSE) {
        /* wait wdma1 cmpl */
        ade_cmdq_wr_wait_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_WAIT_EVT_WDMA1_CMPL);
        return;
    }

    /* offline is finished, then to config online */
    if (cf_buff->cmd_len > 0) {

        /* report wdma1 cmpl to cpu */
        ade_cmdq_wr_wait_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_WAIT_EVT_WDMA1_CMPL);
        ade_cmdq_wr_cmd(INTR_SOFT_CMDQ2_0_REG, ADE_WAIT_EVT_WDMA1_CMPL);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    #if WFD_SUPPORT
        //config wifi display
        ade_overlay_commit_set_wfd(ade_pri_data, pcomp_info, cf_buff);
    #endif
        ade_cmdq_wr_eof_cmd(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_overlay_commit_set_offline_cmdq(ade_base, cf_buff, pcomp_info->compose_pattern_num);

        cf_buff->cmd_len = 0;
    }
    balongfb_logi_display_debugfs("ade_overlay_commit_set_offline:ade_pri_data->have_refresh= %d \n", ade_pri_data->have_refresh);
    balongfb_logi_display_debugfs("ade_overlay_commit_set_offline exit  succ \n");
}

int ade_overlay_commit_set_online(struct ade_compose_data_type *ade_pri_data,
        struct commit_comp_unit *commit_uint,
        struct overlay_compose_info  *pcomp_info,
        struct cmdfile_buffer *cf_buff)
{
    u8*    ade_base;
    bool   ade_top_change = false;
    struct balong_fb_data_type       *fb_data;

    BUG_ON(commit_uint == NULL);
    BUG_ON(pcomp_info == NULL);
    BUG_ON(ade_pri_data == NULL);
    BUG_ON(cf_buff == NULL);

    ade_base = ade_pri_data->ade_base;
    fb_data  = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);

    BUG_ON(fb_data == NULL);

    balongfb_logi_display_debugfs("ade_overlay_commit_set_online enter  succ \n");

    if (0 == ade_pri_data->frame_count) {
        set_TOP_INTR_MASK_CPU1(ade_base, ADE_ISR1_RES_SWITCH_CMPL);
    }

    /* if offline, need to config disp ch */
    if (OVERLAY_COMP_TYPE_OFFLINE == pcomp_info->compose_mode) {
        if (pcomp_info->is_finished == ADE_FALSE) {
            balongfb_logi_display_debugfs("ade_overlay_commit_set_online: offline have not finished \n");
            return -EINVAL;
        }

        if (pcomp_info->offline_frame_phy_addr == 0) {
            balongfb_logi_display_debugfs("ade_overlay_commit_set_online: offline_frame_phy_addr is 0 \n");
            return -EINVAL;
        }

        /* config display ch and ctran5 */
        ade_overlay_commit_set_disp_ch(ade_pri_data, pcomp_info, cf_buff);
        ade_pri_data->have_refresh = ADE_TRUE;
    }

    if (cf_buff->cmd_len <= 0) {
        balongfb_logi_display_debugfs("online cmdfile len is 0 \n");
        return -EINVAL;
    }

    if (commit_uint->bit_online_wdma3) {
        /* set ctran4, if bit_scl3 = 1, rgb888->yuv444,
         * set ctran4, if bit_scl3 = 0, rgb888->yuv422,
         */
        ade_overlay_commit_set_ctran4(ade_pri_data, pcomp_info, cf_buff, commit_uint->bit_scl3, ADE_TRUE);
        if (commit_uint->bit_scl3) {
            ade_overlay_commit_set_scl3(ade_pri_data, pcomp_info, cf_buff, ADE_TRUE);
        }
        ade_overlay_commit_set_wdma3(ade_pri_data, pcomp_info, cf_buff, commit_uint->bit_scl3, ADE_TRUE);
    }

    if (OVERLAY_COMP_TYPE_OFFLINE == pcomp_info->compose_mode) {
#if PARTIAL_UPDATE
    ade_overlay_commit_set_ctran6(ade_pri_data, false, fb_data->xwidth, fb_data->yheight, cf_buff);
#else
    ade_overlay_commit_set_ctran6(ade_pri_data, false, ade_pri_data->xres, ade_pri_data->yres, cf_buff);
#endif
    } else {
        if (pcomp_info->offline_phy_addr == 0) {
            ade_overlay_commit_set_ctran6(ade_pri_data, false, pcomp_info->dst_rect.w, pcomp_info->dst_rect.h, cf_buff);
        }
    }

    if (ADE_FALSE == pcomp_info->is_finished) {
        balongfb_logi_display_debugfs("ade_overlay_commit_set_online: online have not finished \n");
        return -EINVAL;
    }

    /* config disp_rdma, ctran5 and ovly2_ch1 for partial update ,disp_rdma->ovly2 */
    if (commit_uint->bit_part_update) {
        ade_overlay_commit_set_part_update(pcomp_info, ade_pri_data, cf_buff);
    }

    ade_cmdq_wr_eof_cmd(cf_buff->vaddr, &(cf_buff->cmd_len));
    dsb(sy);

    if (OVERLAY_COMP_TYPE_ONLINE != pcomp_info->compose_mode) {
        ade_top_change = ade_overlay_check_ade_top_change(ade_base);

        /* 1: if it is not the first frame, need enable res switch cmpl intr */
        ade_pri_data->res_switch_cmpl = 0;
        ade_pri_data->wdma1_cmpl = 0;
        if (PANEL_MIPI_CMD == fb_data->panel_info.type) {
            if (OVERLAY_COMP_TYPE_OFFLINE != pcomp_info->compose_mode) {
                if (ade_top_change) {
                    if (wait_event_interruptible_timeout(fb_data->frame_wq, fb_data->update_frame, HZ) <= 0) {
                        balongfb_logw("wait_event_interruptible_timeout update_frame !\n");
                    }
                }
            }
        }

        /* 2: enable TOP for offline */
        /* 3: wait res switch cmpl intr */
        if (ade_top_change) {
            set_TOP_ADE_EN(ade_base, ADE_TRUE);
            if (wait_event_interruptible_timeout(ade_pri_data->wait_res_cmpl, ade_pri_data->res_switch_cmpl, (HZ/4)) <= 0) {
                balongfb_loge("wait_event_interruptible_timeout wait_res_cmpl !\n");
                set_LDI_DSI_CMD_MOD_CTRL(0x0);
                mdelay(1);
                set_LDI_DSI_CMD_MOD_CTRL(0x2);
            }
            ade_pri_data->res_switch_cmpl = 0;
        } else {
            if (ade_pri_data->pre_frm_have_virt) {
                if (wait_event_interruptible_timeout(ade_pri_data->wait_ovly1_cmpl, ade_pri_data->ovly1_cmpl, HZ) <= 0) {
                    balongfb_loge("wait_event_interruptible_timeout wait_ovly1_cmpl !\n");
                    set_TOP_SOFT_RST0(ade_base, 0xffffffff);
                    set_TOP_SOFT_RST1(ade_base, 0xffffffff);
                }
                ade_pri_data->ovly1_cmpl = 0;
            }
        }

        set_TOP_SOFT_RST0(ade_base, 0x4FFF9D3F); /* reset rdma1-6,wdma1-3,cmdq2_rdma */
        set_TOP_SOFT_RST1(ade_base, 0xE0);

        /* 4: enable cmdq_rdma2, start offline compose */
        set_CMDQ_RDMA2_EN(ade_base, 1);

        /* 5: wait wdma1 cmpl */
        if (wait_event_interruptible_timeout(ade_pri_data->wait_wdma1_cmpl, ade_pri_data->wdma1_cmpl, (HZ)) <= 0) {
            balongfb_loge("wait_event_interruptible_timeout wait_wdma1_cmpl ade_top_change = %d!\n",ade_top_change);
            ade_overlay_dump_ade_top_reg(ade_base);

            set_TOP_SOFT_RST0(ade_base, 0xffffffff);
            set_TOP_SOFT_RST1(ade_base, 0xffffffff);
            ade_pri_data->wdma1_cmpl = 0;

        #if ADE_DEBUG_LOG_ENABLE
            /* debug */
            balongfb_loge("frame number is %u.", g_debug_frame_number);

            if (g_ade_offline_disable) {
                g_debug_diable_ade = 1;
                return 0;
            }
        #endif

            return 0;
        }
        ade_pri_data->wdma1_cmpl = 0;

        ade_pri_data->pre_frm_have_virt = false;
        if (pcomp_info->wifi_phy_addr != 0) {
            ade_pri_data->pre_frm_have_virt = true;
        }
    }

    //cmdq1_rdma
    set_soft_rst_sel_bit(ade_pri_data->soft_rst1, 0, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis1, 0);
    set_soft_rst_sel_bit(ade_pri_data->soft_rst1, 3, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis1, 3);
    set_soft_rst_sel_bit(ade_pri_data->soft_rst1, 4, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis1, 4);

    set_TOP_SOFT_RST_SEL0(ade_base, ade_pri_data->soft_rst0);
    set_TOP_RELOAD_DIS0(ade_base, ade_pri_data->reload_dis0);
    set_TOP_SOFT_RST_SEL1(ade_base, ade_pri_data->soft_rst1);
    set_TOP_RELOAD_DIS1(ade_base, ade_pri_data->reload_dis1);

    if (PANEL_MIPI_CMD == fb_data->panel_info.type) {
        ade_overlay_commit_set_online_cmd_mode(ade_pri_data, fb_data, pcomp_info, cf_buff);
    } else {
        ade_overlay_commit_set_online_video_mode(ade_pri_data, fb_data, pcomp_info, cf_buff);
    }

    if (0 == ade_pri_data->frame_count) {
        ade_pri_data->frame_count++;
    }

    /* online and hybird mode don't need DISP_RDMA */
    if (OVERLAY_COMP_TYPE_OFFLINE != pcomp_info->compose_mode) {
        ade_pri_data->have_refresh = false;
    }

    balongfb_logi_display_debugfs("ade_overlay_commit_set_online exit  succ \n");

    return 0;
}


void ade_overlay_refresh_set_wdma2(struct ade_compose_data_type  *ade_pri_data, struct cmdfile_buffer  *online_cf_buff)
{
    u32     stride;

    BUG_ON(ade_pri_data == NULL);
    BUG_ON(online_cf_buff == NULL);

    if (ade_pri_data->wdma2_phy == 0) {
        return;
    }

    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 11, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 11);

    stride = ade_pri_data->xres * ade_overlay_format2bpp(ade_pri_data->fb_imgType);
    ade_cmdq_wr_rdma_pe_cmd(WR_CH2_PE_REG, OVERLAY_PIPE_TYPE_ONLINE, ADE_ROT_NOP);
    ade_cmdq_wr_cmd(WR_CH2_CTRL_REG, ade_pri_data->fb_imgType << 16);
    ade_cmdq_wr_cmd(WR_CH2_ADDR_REG, ade_pri_data->wdma2_phy);
    ade_cmdq_wr_cmd(WR_CH2_SIZE_REG, ade_pri_data->yres << 16 | stride);
    ade_cmdq_wr_cmd(WR_CH2_STRIDE_REG, stride);
    ade_cmdq_wr_cmd(WR_CH2_SPACE_REG, ade_pri_data->yres * stride);
    ade_cmdq_wr_cmd2buff(online_cf_buff->vaddr, &online_cf_buff->cmd_len);

    ade_cmdq_wr_cmd(WR_CH2_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(online_cf_buff->vaddr, &online_cf_buff->cmd_len);

    balongfb_logi_display_debugfs("ade_overlay_refresh_set_wdma2: ade_pri_data->wdma2_phy= 0x%x \n", ade_pri_data->wdma2_phy);
}
void ade_overlay_refresh_set_display(struct ade_compose_data_type  *ade_pri_data, struct cmdfile_buffer  *online_cf_buff)
{
    u32     stride;
    struct  ade_rect disp_rect;

    BUG_ON(ade_pri_data == NULL);
    BUG_ON(online_cf_buff == NULL);

    if (ade_pri_data->wdma2_phy == 0) {
        return;
    }

    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 6, ade_pri_data->lcd_type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 6);
    balongfb_logi_display_debugfs("ade_overlay_refreh_set_display: disp_ch rdma soft rest sel bit = 6 \n");

    stride = ade_pri_data->xres * ade_overlay_format2bpp(ade_pri_data->fb_imgType);

    /* set disp channel into AXI0 */
    set_TOP_DMA_AXI_MUX(ade_pri_data->ade_base, OVERLAY_PIPE_ADE_DISP, OVERLAY_PIPE_TYPE_ONLINE);

    ade_cmdq_wr_rdma_pe_cmd(RD_CH_DISP_PE_REG, OVERLAY_PIPE_TYPE_ONLINE, ADE_ROT_NOP);
    ade_cmdq_wr_cmd(RD_CH_DISP_CTRL_REG, ade_pri_data->fb_imgType << 16);
    ade_cmdq_wr_cmd(RD_CH_DISP_ADDR_REG, ade_pri_data->wdma2_phy);
    ade_cmdq_wr_cmd(RD_CH_DISP_SIZE_REG, ade_pri_data->yres << 16 | stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_STRIDE_REG, stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_SPACE_REG, ade_pri_data->yres * stride);
    ade_cmdq_wr_cmd2buff(online_cf_buff->vaddr, &(online_cf_buff->cmd_len));

    ade_cmdq_wr_cmd(RD_CH_DISP_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(online_cf_buff->vaddr, &(online_cf_buff->cmd_len));

    disp_rect.x = 0;
    disp_rect.y = 0;
    disp_rect.w = ade_pri_data->xres;
    disp_rect.h = ade_pri_data->yres;

    /* ctrans 5 */
    ade_overlay_commit_set_ctran5(ade_pri_data, disp_rect, ade_pri_data->fb_imgType, online_cf_buff);

    /* ctran 6 */
    ade_overlay_commit_set_ctran6(ade_pri_data, ADE_FALSE, disp_rect.w, disp_rect.h, online_cf_buff);
}

void ade_overlay_refresh_pre_frm_info(struct ade_compose_data_type *ade_pri_data,
        struct cmdfile_buffer  *online_cf_buff,
        struct cmdfile_buffer  *old_online_cf_buff)
{
    u32 ldi_src = 0;
    u8* ade_base;
    u32 wdma2_src = 0;
    int ret;

    BUG_ON(online_cf_buff == NULL);
    BUG_ON(ade_pri_data == NULL);

    balongfb_logi_display_debugfs("ade_overlay_refresh_pre_frm_info: old_online_cf_buff->cmd_len=%d,enter succ ! \n", old_online_cf_buff->cmd_len);
    if (old_online_cf_buff->cmd_len <= sizeof(NULL_EOF_CMD)) {
        balongfb_loge("get the old cmdfile len error cmd_len = %d \n", old_online_cf_buff->cmd_len);
        return;
    }

    /***************************write back to wdma2 ***********************************/
    /*************1: config TOP *******************/
    ade_base = ade_pri_data->ade_base;
    ldi_src = inp32(ade_base + ADE_DISP_SRC_CFG_REG);
    wdma2_src = ade_overlay_disp_src2wdma2_src(ldi_src);
    set_TOP_WDMA2_SRC_CFG(ade_base, wdma2_src); /* set wdma2 top */

    ade_pri_data->ovly1_cmpl = 0;
    clear_TOP_INTR_ORG_CPU0(ade_base, 0xffffffff);
    clear_TOP_INTR_ORG_CPU1(ade_base, 0xffffffff);

    /*************2: generate cmdfile *******************/
    ade_overlay_refresh_set_wdma2(ade_pri_data, online_cf_buff);       /* config wdma2 */

    old_online_cf_buff->cmd_len -= sizeof(NULL_EOF_CMD);               /* delete EOF cmd and write wdma2 configure */
    memcpy((void *)((u8 *)online_cf_buff->vaddr + online_cf_buff->cmd_len),
            (u8 *)old_online_cf_buff->vaddr, old_online_cf_buff->cmd_len);

    online_cf_buff->cmd_len += old_online_cf_buff->cmd_len;
    ade_cmdq_wr_eof_cmd(online_cf_buff->vaddr, &(online_cf_buff->cmd_len));
    dsb(sy);

    /*************3: config new cmdq_rdma1 *******************/
    ade_overlay_commit_set_online_cmdq(ade_base, online_cf_buff, 0xfffe);       /* commit cmdfile */

    /*************4: TOP ADE EN *******************/
    set_TOP_ADE_EN(ade_base, 1);

    /*************5: wait for wdma2 cmpl, then to config disp *******************/
    /*
    if (wait_event_interruptible_timeout(ade_pri_data->wait_wdma2_cmpl, ade_pri_data->wdma2_cmpl, HZ) <= 0) {
        balongfb_loge("wait wdma2 cmpl inttrupt timeout \n");
        ade_pri_data->wdma2_cmpl = 0;
        return;
    }
    ade_pri_data->wdma2_cmpl = 0;
    */

    /***************************read back from disp_ch*************************/
    /************************** 1: switch cmfbuffer *********************/
    ret = ade_overlay_res_get_cmdbuff_index(OVERLAY_COMP_TYPE_ONLINE, ade_pri_data->cmdq_list);
    if (ret != 0) {
        balongfb_loge("get cmd buff index error");
        return;
    }

    online_cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, OVERLAY_PIPE_TYPE_ONLINE);

    /************************** 3: config TOP for disp->ldi *********************/
    set_TOP_DISP_SRC_CFG(ade_base, TOP_DISP_CH_SRC_RDMA);

    set_TOP_WDMA2_SRC_CFG(ade_base, 0);
    set_TOP_SCL3_MUX_CFG_scl3_mux(ade_base, 0);
    set_TOP_SCL1_MUX_CFG(ade_base, 0);
    set_TOP_ROT_SRC_CFG(ade_base, 0);
    set_TOP_SCL2_SRC_CFG(ade_base, 0);
    set_TOP_SEC_OVLY_SRC_CFG(ade_base, 0);
    set_TOP_WDMA3_SRC_CFG(ade_base, 0);
    set_TOP_OVLY1_TRANS_CFG(ade_base, 0);
    set_TOP_CTRAN5_TRANS_CFG(ade_base, 0);
    set_TOP_OVLY_CTRL_all(ade_base, 0);

    set_TOP_SOFT_RST_SEL0(ade_base, 0);
    set_TOP_SOFT_RST_SEL1(ade_base, 0);
    set_TOP_RELOAD_DIS0(ade_base, 0);
    set_TOP_RELOAD_DIS1(ade_base, 0);

    set_TOP_SOFT_RST_SEL1_gamma(ade_base, 1);
    set_TOP_SOFT_RST_SEL1_dither(ade_base, 1);
    set_TOP_SOFT_RST_SEL1_cmdq1(ade_base, 1);
    set_TOP_RELOAD_DIS1_gamma(ade_base, 1);
    set_TOP_RELOAD_DIS1_dither(ade_base, 1);
    set_TOP_RELOAD_DIS1_cmdq1(ade_base, 1);

    /************************** 2: generate cmffile, config display *********************/
    ade_overlay_refresh_set_display(ade_pri_data, online_cf_buff);
    ade_cmdq_wr_eof_cmd(online_cf_buff->vaddr, &(online_cf_buff->cmd_len));
    dsb(sy);

    /************************** 4: config cmdq_rdma1 *********************/
    ade_overlay_commit_set_online_cmdq(ade_base,online_cf_buff, 0xfffd);
    set_TOP_SOFT_RST_SEL0_cmdq1_rdma(ade_base, 1);

    /************************** 5: config TOP ADE EN *********************/
    ade_pri_data->res_switch_cmpl = 0;
    set_TOP_ADE_EN(ade_base, 1);

    if (wait_event_interruptible_timeout(ade_pri_data->wait_res_cmpl, ade_pri_data->res_switch_cmpl, (HZ/4)) <= 0) {
        balongfb_logw("wait_event_interruptible_timeout wait_res_cmpl !\n");
        set_LDI_DSI_CMD_MOD_CTRL(0x0);
        mdelay(1);
        set_LDI_DSI_CMD_MOD_CTRL(0x2);
    }
    ade_pri_data->res_switch_cmpl = 0;

    balongfb_logi_display_debugfs("ade_overlay_refresh_pre_frm_info: exit succ ! \n");
}

void ade_overlay_layer_get_blk_info(struct overlay_layer_info  *ade_layer_info, struct rot_layer_info  *rot_layer_info)
{
    u32     align_pixel;
    u32     align_byte;
    u32     width_n = 1;
    u32     height_m = 1;
    u32     delta_width;
    u32     delta_height;
    u32     bpp;
    u32     i = 0;
    u32     tmp_w_l;
    u32     tmp_h_l;
    struct  rot_block_info tmp_block[4];
    u32     output_bpp;

    BUG_ON(ade_layer_info == NULL);
    BUG_ON(rot_layer_info == NULL);

    if (ADE_ROT_NOP == ade_layer_info->rotation) {
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: block rot angle is 0 \n");
        return;
    }

    memset(tmp_block, 0, (sizeof(struct  rot_block_info) * 4));

    bpp = ade_overlay_format2bpp(ade_layer_info->format);
    if(0 == bpp){
        return;
    }

    output_bpp = ade_overlay_format2bpp(ade_layer_info->dst_format);
    balongfb_logw_display_debugfs("ade_overlay_layer_get_blk_info: output_bpp = %d \n", output_bpp);
    if ((ADE_BGR_888 == ade_layer_info->format) || (ADE_RGB_888 == ade_layer_info->format)
            || (ADE_YUV444 == ade_layer_info->format)) {
        align_pixel = 12 / 3;
        align_byte  = 12;
    } else if ((ADE_NV12 == ade_layer_info->format) || (ADE_NV21 == ade_layer_info->format)) {
        align_pixel = 16;
        align_byte  = 16;
    } else {
        align_pixel = 16 / bpp;
        align_byte  = 16;
    }

    /* buffer = 16k / 2 * 1024  */
    switch (ade_layer_info->format) {
        case ADE_RGB_565:
        case ADE_BGR_565:
            /* 16*n*16*m <= buff, buff=16k/2*1024/2 = 4096,n*m<=16,
             * 32 * 64
             */
            width_n = 4;
            height_m = 4;
            break;
        case ADE_RGB_888:
        case ADE_BGR_888:
        case ADE_YUV444:
            /* 12*n*12*m <= buff, buff=16k/2*1024*3/4 = 6144,n*m <=42
             * 28 * 72
             */
            width_n = 7;
            height_m = 6;
            break;
        case ADE_NV12:
        case ADE_NV21:
            /* 16*n*16*m <= buff, buff=16k/2*1024*2/3 = 5461,n*m<=21
             * 80 * 64
             */
            width_n = 5;
            height_m = 4;
            break;
        default:
            /* 16*n*16*m <= buff, buff=16k/2*1024*1 = 8192,n*m<=32
             * 32 * 64
             */
            width_n = 8;
            height_m = 4;
            break;
    }

    tmp_w_l = (width_n * align_byte) / bpp;
    tmp_h_l = (height_m * align_byte);

    if (ADE_ROT_180 == ade_layer_info->rotation) {
        /* **********************
         * *                    *
         * *                    *
         * *                    *
         * *                    *
         * **********************
         */
        tmp_block[0].blk_enable = ADE_TRUE;
        tmp_block[0].blk_width_b = ade_layer_info->width;
        tmp_block[0].blk_height_b = ade_layer_info->height;
        tmp_block[0].blk_width_l = ade_layer_info->width;
        tmp_block[0].blk_height_l = 1;

        tmp_block[0].blk_w_width_b = ade_layer_info->width;
        tmp_block[0].blk_w_height_b = ade_layer_info->height;
        tmp_block[0].blk_w_width_l = ade_layer_info->width;
        tmp_block[0].blk_w_height_l = 1;

        //tmp_block[0].blk_w_delta_width_b = ade_layer_info->delta_width;

        tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr; //ade_overlay_layer_get_wdma_addr(ADE_ROT_180, ade_layer_info->format, &tmp_block[0]);
        tmp_block[0].wdma_uv_addr = ade_layer_info->dst_phy_addr + ade_layer_info->dst_stride * ade_layer_info->height;

        tmp_block[0].rdma_addr  = ade_layer_info->phy_addr;

        /* uv */
        tmp_block[0].rdma_uv_addr = ade_layer_info->uv_addr;
    } else {
        delta_width = ade_layer_info->width % tmp_w_l;
        delta_height = ade_layer_info->height % tmp_h_l;

        if (ade_layer_info->width < tmp_w_l) {
            delta_width = 0;
            tmp_w_l = ade_layer_info->width;
        }

        if (ade_layer_info->height < tmp_h_l) {
            delta_height = 0;
            tmp_h_l = ade_layer_info->height;
        }

        if ((0 == delta_width) && (0 == delta_height)) {
            /* cut average */
            tmp_block[0].blk_enable = ADE_TRUE;
            tmp_block[0].blk_width_b = ade_layer_info->width;
            tmp_block[0].blk_height_b = ade_layer_info->height;
            tmp_block[0].blk_width_l = tmp_w_l;
            tmp_block[0].blk_height_l = tmp_h_l;
            tmp_block[0].blk_w_width_b = tmp_block[0].blk_height_b;
            tmp_block[0].blk_w_height_b = tmp_block[0].blk_width_b;
            tmp_block[0].blk_w_width_l = tmp_block[0].blk_height_l;
            tmp_block[0].blk_w_height_l = tmp_block[0].blk_width_l;
            //tmp_block[0].blk_w_delta_width_b = 0;

            tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr;
            tmp_block[0].wdma_uv_addr = ade_layer_info->dst_phy_addr + ade_layer_info->dst_stride* ade_layer_info->width;

            if (ADE_ROT_90 == ade_layer_info->rotation) {
                /* read start
                 * from here  --> **********************
                 *                *                    *
                 *                *                    *
                 *                *                    *
                 *                *                    *
                 *                **********************
                 */
                tmp_block[0].rdma_addr  = ade_layer_info->phy_addr;
                tmp_block[0].rdma_uv_addr = ade_layer_info->uv_addr;
            }

            if (ADE_ROT_270 == ade_layer_info->rotation) {
                /* **********************
                 * *                    *
                 * *                    *
                 * *                    *
                 * *                    *
                 * **********************
                 */
                tmp_block[0].rdma_addr  = ade_layer_info->phy_addr;
                tmp_block[0].rdma_uv_addr = ade_layer_info->uv_addr;
            }
        } else if (0 == delta_width) {
            /* *************************
             * *                       *
             * *            0          *
             * *                       *
             * *-----------------------*
             * *            1          *
             * *************************
             */
            if (ADE_ROT_270 == ade_layer_info->rotation) {
                /* cut to 2 block */
                tmp_block[0].blk_enable = ADE_TRUE;
                tmp_block[0].blk_width_b = ade_layer_info->width;
                tmp_block[0].blk_height_b = ade_layer_info->height - delta_height;
                tmp_block[0].blk_width_l = tmp_w_l;
                tmp_block[0].blk_height_l = tmp_h_l;
                tmp_block[0].blk_w_width_b = tmp_block[0].blk_height_b;
                tmp_block[0].blk_w_height_b = tmp_block[0].blk_width_b;
                tmp_block[0].blk_w_width_l = tmp_block[0].blk_height_l;
                tmp_block[0].blk_w_height_l = tmp_block[0].blk_width_l;
                //tmp_block[1].blk_w_delta_width_b = 0;


                tmp_block[0].rdma_addr  = ade_layer_info->phy_addr;
                tmp_block[0].rdma_uv_addr = ade_layer_info->uv_addr;

                tmp_block[1].blk_enable = ADE_TRUE;
                tmp_block[1].blk_width_b = ade_layer_info->width;
                tmp_block[1].blk_height_b = delta_height;
                tmp_block[1].blk_width_l = tmp_w_l;
                tmp_block[1].blk_height_l = delta_height;
                tmp_block[1].blk_w_width_b = tmp_block[1].blk_height_b;
                tmp_block[1].blk_w_height_b = tmp_block[1].blk_width_b;
                tmp_block[1].blk_w_width_l = tmp_block[1].blk_height_l;
                tmp_block[1].blk_w_height_l = tmp_block[1].blk_width_l;
                tmp_block[1].rdma_addr  = tmp_block[0].rdma_addr + ade_layer_info->stride * tmp_block[0].blk_height_b;
                tmp_block[1].rdma_uv_addr = tmp_block[0].rdma_uv_addr + ade_layer_info->stride * (tmp_block[0].blk_height_b / 2);/* uv */

                // tmp_block[1].blk_w_delta_width_b = ade_layer_info->delta_width;
                tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr;
                tmp_block[0].wdma_uv_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride* ade_layer_info->width;

                tmp_block[1].wdma_addr = tmp_block[0].wdma_addr +  tmp_block[0].blk_w_width_b * output_bpp;
                tmp_block[1].wdma_uv_addr = tmp_block[0].wdma_uv_addr + tmp_block[0].blk_w_width_b;


            }

            /* *************************
             * *           1           *
             * *-----------------------*
             * *                       *
             * *           0           *
             * *                       *
             * *************************
             */
            if (ADE_ROT_90 == ade_layer_info->rotation) {
                /* cut to 2 block */
                tmp_block[1].blk_enable = ADE_TRUE;
                tmp_block[1].blk_width_b = ade_layer_info->width;
                tmp_block[1].blk_height_b = delta_height;
                tmp_block[1].blk_width_l = tmp_w_l;
                tmp_block[1].blk_height_l = delta_height;
                tmp_block[1].blk_w_width_b  = tmp_block[1].blk_height_b;
                tmp_block[1].blk_w_height_b = tmp_block[1].blk_width_b;
                tmp_block[1].blk_w_width_l  = tmp_block[1].blk_height_l;
                tmp_block[1].blk_w_height_l = tmp_block[1].blk_width_l;
                // tmp_block[1].blk_w_delta_width_b = ade_layer_info->delta_width;
                tmp_block[1].rdma_addr  = ade_layer_info->phy_addr;
                tmp_block[1].rdma_uv_addr = ade_layer_info->uv_addr;

                tmp_block[0].blk_enable = ADE_TRUE;
                tmp_block[0].blk_width_b = ade_layer_info->width;
                tmp_block[0].blk_height_b = ade_layer_info->height - delta_height;
                tmp_block[0].blk_width_l = tmp_w_l;
                tmp_block[0].blk_height_l = tmp_h_l;
                tmp_block[0].blk_w_width_b  = tmp_block[0].blk_height_b;
                tmp_block[0].blk_w_height_b = tmp_block[0].blk_width_b;
                tmp_block[0].blk_w_width_l  = tmp_block[0].blk_height_l;
                tmp_block[0].blk_w_height_l = tmp_block[0].blk_width_l;
                //   tmp_block[0].blk_w_delta_width_b = 0;
                tmp_block[0].rdma_addr  = tmp_block[1].rdma_addr + ade_layer_info->stride * tmp_block[1].blk_height_b;
                tmp_block[0].rdma_uv_addr = tmp_block[1].rdma_uv_addr + ade_layer_info->stride * (tmp_block[1].blk_height_b / 2);

                tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr;
                tmp_block[0].wdma_uv_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride* ade_layer_info->width;
                tmp_block[1].wdma_addr = tmp_block[0].wdma_addr + tmp_block[0].blk_w_width_b * output_bpp;
                tmp_block[1].wdma_uv_addr = tmp_block[0].wdma_uv_addr + tmp_block[0].blk_w_width_b;
            }
        } else if (0 == delta_height) {
            /* cut to 2 block */
            /* *************************
             * *                  |    *
             * *                  |    *
             * *          0       | 1  *
             * *                  |    *
             * *                  |    *
             * *************************
             */
            if (ADE_ROT_90 == ade_layer_info->rotation) {
                tmp_block[0].blk_enable = ADE_TRUE;
                tmp_block[0].blk_width_b = ade_layer_info->width - delta_width;
                tmp_block[0].blk_height_b = ade_layer_info->height;
                tmp_block[0].blk_width_l = tmp_w_l;
                tmp_block[0].blk_height_l = tmp_h_l;
                tmp_block[0].blk_w_width_b  = tmp_block[0].blk_height_b;
                tmp_block[0].blk_w_height_b = tmp_block[0].blk_width_b;
                tmp_block[0].blk_w_width_l  = tmp_block[0].blk_height_l;
                tmp_block[0].blk_w_height_l = tmp_block[0].blk_width_l;
                //tmp_block[0].blk_w_delta_width_b = 0;

                tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr;
                tmp_block[0].wdma_uv_addr = tmp_block[0].wdma_addr + ade_layer_info->height * ade_layer_info->width;
                tmp_block[0].rdma_addr  = ade_layer_info->phy_addr;
                tmp_block[0].rdma_uv_addr = ade_layer_info->uv_addr;

                tmp_block[1].blk_enable = ADE_TRUE;
                tmp_block[1].blk_width_b = delta_width;
                tmp_block[1].blk_height_b = ade_layer_info->height;
                tmp_block[1].blk_width_l = delta_width;
                tmp_block[1].blk_height_l = tmp_h_l;
                tmp_block[1].blk_w_width_b  = tmp_block[1].blk_height_b;
                tmp_block[1].blk_w_height_b = tmp_block[1].blk_width_b;
                tmp_block[1].blk_w_width_l  = tmp_block[1].blk_height_l;
                tmp_block[1].blk_w_height_l = tmp_block[1].blk_width_l;
                tmp_block[1].rdma_addr  = tmp_block[0].rdma_addr + tmp_block[0].blk_width_b * bpp;
                tmp_block[1].rdma_uv_addr = tmp_block[0].rdma_uv_addr + tmp_block[0].blk_width_b;
                //tmp_block[1].blk_w_delta_width_b = 0;

                tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr;
                tmp_block[0].wdma_uv_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride* ade_layer_info->width;
                tmp_block[1].wdma_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride * tmp_block[0].blk_w_height_b;
                tmp_block[1].wdma_uv_addr = tmp_block[0].wdma_uv_addr + ade_layer_info->dst_stride * tmp_block[0].blk_w_height_b / 2;


            }

            /* *************************
             * *   |                   *
             * *   |                   *
             * * 1 |       0           *
             * *   |                   *
             * *   |                   *
             * *************************
             */
            if (ADE_ROT_270 == ade_layer_info->rotation) {
                tmp_block[1].blk_enable = ADE_TRUE;
                tmp_block[1].blk_width_b = delta_width;
                tmp_block[1].blk_height_b = ade_layer_info->height;
                tmp_block[1].blk_width_l = delta_width;
                tmp_block[1].blk_height_l = tmp_h_l;
                tmp_block[1].blk_w_width_b  = tmp_block[1].blk_height_b;
                tmp_block[1].blk_w_height_b = tmp_block[1].blk_width_b;
                tmp_block[1].blk_w_width_l  = tmp_block[1].blk_height_l;
                tmp_block[1].blk_w_height_l = tmp_block[1].blk_width_l;
                //tmp_block[1].blk_w_delta_width_b = 0;
                tmp_block[1].rdma_addr  = ade_layer_info->phy_addr;
                tmp_block[1].rdma_uv_addr = ade_layer_info->uv_addr;

                tmp_block[0].blk_enable = ADE_TRUE;
                tmp_block[0].blk_width_b = ade_layer_info->width - delta_width;
                tmp_block[0].blk_height_b = ade_layer_info->height;
                tmp_block[0].blk_width_l = tmp_w_l;
                tmp_block[0].blk_height_l = tmp_h_l;
                tmp_block[0].blk_w_width_b  = tmp_block[0].blk_height_b;
                tmp_block[0].blk_w_height_b = tmp_block[0].blk_width_b;
                tmp_block[0].blk_w_width_l  = tmp_block[0].blk_height_l;
                tmp_block[0].blk_w_height_l = tmp_block[0].blk_width_l;
                //tmp_block[0].blk_w_delta_width_b = 0;
                tmp_block[0].rdma_addr  = tmp_block[1].rdma_addr + tmp_block[1].blk_width_b * bpp;
                tmp_block[0].rdma_uv_addr = tmp_block[1].rdma_uv_addr + tmp_block[1].blk_width_b;

                tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr;
                tmp_block[0].wdma_uv_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride * ade_layer_info->width;
                tmp_block[1].wdma_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride * tmp_block[0].blk_w_height_b;
                tmp_block[1].wdma_uv_addr = tmp_block[0].wdma_uv_addr + ade_layer_info->dst_stride * tmp_block[0].blk_w_height_b / 2;
            }
        } else {
            /* ****************************
             * *            1         | 3 *
             * *----------------------|---*
             * *                      |   *
             * *            0         | 2 *
             * *                      |   *
             * ****************************
             */
            if (ADE_ROT_90 == ade_layer_info->rotation) {
                /* cut to 4 block */
                tmp_block[1].blk_enable = ADE_TRUE;
                tmp_block[1].blk_width_b = ade_layer_info->width - delta_width;
                tmp_block[1].blk_height_b = delta_height;
                tmp_block[1].blk_width_l = tmp_w_l;
                tmp_block[1].blk_height_l = delta_height;
                tmp_block[1].blk_w_width_b  = tmp_block[1].blk_height_b;
                tmp_block[1].blk_w_height_b = tmp_block[1].blk_width_b;
                tmp_block[1].blk_w_width_l  = tmp_block[1].blk_height_l;
                tmp_block[1].blk_w_height_l = tmp_block[1].blk_width_l;

                //tmp_block[1].blk_w_delta_width_b = ade_layer_info->delta_width;
                tmp_block[1].rdma_addr  = ade_layer_info->phy_addr;
                tmp_block[1].rdma_uv_addr = ade_layer_info->uv_addr;

                tmp_block[3].blk_enable = ADE_TRUE;
                tmp_block[3].blk_width_b = delta_width;
                tmp_block[3].blk_height_b = delta_height;
                tmp_block[3].blk_width_l = delta_width;
                tmp_block[3].blk_height_l = delta_height;
                tmp_block[3].blk_w_width_b  = tmp_block[3].blk_height_b;
                tmp_block[3].blk_w_height_b = tmp_block[3].blk_width_b;
                tmp_block[3].blk_w_width_l  = tmp_block[3].blk_height_l;
                tmp_block[3].blk_w_height_l = tmp_block[3].blk_width_l;
                //tmp_block[3].blk_w_delta_width_b = ade_layer_info->delta_width;
                tmp_block[3].rdma_addr = tmp_block[1].rdma_addr + tmp_block[1].blk_width_b * bpp;
                tmp_block[3].rdma_uv_addr = tmp_block[1].rdma_uv_addr + tmp_block[1].blk_width_b;

                tmp_block[0].blk_enable = ADE_TRUE;
                tmp_block[0].blk_width_b = ade_layer_info->width - delta_width;
                tmp_block[0].blk_height_b = ade_layer_info->height - delta_height;
                tmp_block[0].blk_width_l = tmp_w_l;
                tmp_block[0].blk_height_l = tmp_h_l;
                tmp_block[0].blk_w_width_b  = tmp_block[0].blk_height_b;
                tmp_block[0].blk_w_height_b = tmp_block[0].blk_width_b;
                tmp_block[0].blk_w_width_l  = tmp_block[0].blk_height_l;
                tmp_block[0].blk_w_height_l = tmp_block[0].blk_width_l;
                //tmp_block[0].blk_w_delta_width_b = 0;
                tmp_block[0].rdma_addr  = tmp_block[1].rdma_addr + ade_layer_info->stride* tmp_block[1].blk_height_b;
                tmp_block[0].rdma_uv_addr = tmp_block[1].rdma_uv_addr + ade_layer_info->stride * tmp_block[1].blk_height_b / 2;

                tmp_block[2].blk_enable = ADE_TRUE;
                tmp_block[2].blk_width_b = delta_width;
                tmp_block[2].blk_height_b = ade_layer_info->height - delta_height;
                tmp_block[2].blk_width_l = delta_width;
                tmp_block[2].blk_height_l = tmp_h_l;
                tmp_block[2].blk_w_width_b  = tmp_block[2].blk_height_b;
                tmp_block[2].blk_w_height_b = tmp_block[2].blk_width_b;
                tmp_block[2].blk_w_width_l  = tmp_block[2].blk_height_l;
                tmp_block[2].blk_w_height_l = tmp_block[2].blk_width_l;
                //tmp_block[2].blk_w_delta_width_b = 0;
                tmp_block[2].rdma_addr = tmp_block[0].rdma_addr + tmp_block[0].blk_width_b * bpp;
                tmp_block[2].rdma_uv_addr = tmp_block[0].rdma_uv_addr + tmp_block[0].blk_width_b;

                tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr;
                tmp_block[0].wdma_uv_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride * ade_layer_info->width;
                tmp_block[1].wdma_addr = tmp_block[0].wdma_addr + tmp_block[0].blk_w_width_b * output_bpp;
                tmp_block[1].wdma_uv_addr = tmp_block[0].wdma_uv_addr + tmp_block[0].blk_w_width_b;
                tmp_block[2].wdma_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride * tmp_block[0].blk_width_b;
                tmp_block[2].wdma_uv_addr = tmp_block[0].wdma_uv_addr + ade_layer_info->dst_stride * tmp_block[0].blk_w_height_b / 2;
                tmp_block[3].wdma_addr = tmp_block[2].wdma_addr + tmp_block[2].blk_w_width_b * output_bpp;
                tmp_block[3].wdma_uv_addr = tmp_block[2].wdma_uv_addr + tmp_block[2].blk_w_width_b;
            }

            /* ****************************
             * *   |                      *
             * * 2 |         0            *
             * *   |                      *
             * *---|----------------------*
             * * 3 |         1            *
             * ****************************
             */
            if (ADE_ROT_270 == ade_layer_info->rotation) {
                /* cut to 4 block */
                tmp_block[2].blk_enable = ADE_TRUE;
                tmp_block[2].blk_width_b = delta_width;
                tmp_block[2].blk_height_b = ade_layer_info->height - delta_height;
                tmp_block[2].blk_width_l = delta_width;
                tmp_block[2].blk_height_l = tmp_h_l;
                tmp_block[2].blk_w_width_b  = tmp_block[2].blk_height_b;
                tmp_block[2].blk_w_height_b = tmp_block[2].blk_width_b;
                tmp_block[2].blk_w_width_l  = tmp_block[2].blk_height_l;
                tmp_block[2].blk_w_height_l = tmp_block[2].blk_width_l;
                //tmp_block[2].blk_w_delta_width_b = 0;
                tmp_block[2].rdma_addr = ade_layer_info->phy_addr;
                tmp_block[2].rdma_uv_addr = ade_layer_info->uv_addr;

                tmp_block[0].blk_enable = ADE_TRUE;
                tmp_block[0].blk_width_b = ade_layer_info->width - delta_width;
                tmp_block[0].blk_height_b = ade_layer_info->height - delta_height;
                tmp_block[0].blk_width_l = tmp_w_l;
                tmp_block[0].blk_height_l = tmp_h_l;
                tmp_block[0].blk_w_width_b  = tmp_block[0].blk_height_b;
                tmp_block[0].blk_w_height_b = tmp_block[0].blk_width_b;
                tmp_block[0].blk_w_width_l  = tmp_block[0].blk_height_l;
                tmp_block[0].blk_w_height_l = tmp_block[0].blk_width_l;
                //tmp_block[0].blk_w_delta_width_b = 0;
                tmp_block[0].rdma_addr  = tmp_block[2].rdma_addr + tmp_block[2].blk_width_b * bpp;
                tmp_block[0].rdma_uv_addr = tmp_block[2].rdma_uv_addr + tmp_block[2].blk_width_b;

                tmp_block[3].blk_enable = ADE_TRUE;
                tmp_block[3].blk_width_b = delta_width;
                tmp_block[3].blk_height_b = delta_height;
                tmp_block[3].blk_width_l = delta_width;
                tmp_block[3].blk_height_l = delta_height;
                tmp_block[3].blk_w_width_b  = tmp_block[3].blk_height_b;
                tmp_block[3].blk_w_height_b = tmp_block[3].blk_width_b;
                tmp_block[3].blk_w_width_l  = tmp_block[3].blk_height_l;
                tmp_block[3].blk_w_height_l = tmp_block[3].blk_width_l;
                // tmp_block[3].blk_w_delta_width_b = ade_layer_info->delta_width;
                tmp_block[3].rdma_addr  = tmp_block[2].rdma_addr + ade_layer_info->stride * tmp_block[2].blk_height_b;
                tmp_block[3].rdma_uv_addr = tmp_block[2].rdma_uv_addr + ade_layer_info->stride * (tmp_block[2].blk_height_b / 2);

                tmp_block[1].blk_enable = ADE_TRUE;
                tmp_block[1].blk_width_b = ade_layer_info->width - delta_width;
                tmp_block[1].blk_height_b = delta_height;
                tmp_block[1].blk_width_l = tmp_w_l;
                tmp_block[1].blk_height_l = delta_height;
                tmp_block[1].blk_w_width_b  = tmp_block[1].blk_height_b;
                tmp_block[1].blk_w_height_b = tmp_block[1].blk_width_b;
                tmp_block[1].blk_w_width_l  = tmp_block[1].blk_height_l;
                tmp_block[1].blk_w_height_l = tmp_block[1].blk_width_l;
                //tmp_block[1].blk_w_delta_width_b = ade_layer_info->delta_width;
                tmp_block[1].rdma_addr  = tmp_block[3].rdma_addr + tmp_block[3].blk_width_b * bpp;
                tmp_block[1].rdma_uv_addr = tmp_block[3].rdma_uv_addr + tmp_block[3].blk_width_b;

                tmp_block[0].wdma_addr = ade_layer_info->dst_phy_addr;
                tmp_block[0].wdma_uv_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride * ade_layer_info->width;
                tmp_block[1].wdma_addr = tmp_block[0].wdma_addr + tmp_block[0].blk_w_width_b * output_bpp;
                tmp_block[1].wdma_uv_addr = tmp_block[0].wdma_uv_addr + tmp_block[0].blk_w_width_b;
                tmp_block[2].wdma_addr = tmp_block[0].wdma_addr + ade_layer_info->dst_stride * tmp_block[0].blk_w_height_b;
                tmp_block[2].wdma_uv_addr = tmp_block[0].wdma_uv_addr + ade_layer_info->dst_stride * (tmp_block[0].blk_w_height_b / 2);
                tmp_block[3].wdma_addr = tmp_block[2].wdma_addr + tmp_block[2].blk_w_width_b * output_bpp;
                tmp_block[3].wdma_uv_addr = tmp_block[2].wdma_uv_addr + tmp_block[2].blk_w_width_b;
            }
        }
    }

    memcpy(rot_layer_info->blk_info, tmp_block, sizeof(tmp_block));

    balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info:rot_layer_info->layer_format=%d \n", rot_layer_info->layer_format);
    balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info:rot_layer_info->layer_width=%d \n", rot_layer_info->layer_width);
    balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info:rot_layer_info->layer_height=%d \n", rot_layer_info->layer_height);
    balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info:rot_layer_info->layer_stride=%d \n", rot_layer_info->layer_stride);
    balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info:rot_layer_info->layer_rot=%d \n", rot_layer_info->layer_rot);

    for (i = 0; i < 4; i++) {
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].blk_enable=%d\n", i, rot_layer_info->blk_info[i].blk_enable);
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].blk_width_b=%d\n", i, rot_layer_info->blk_info[i].blk_width_b);
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].blk_height_b=%d\n", i, rot_layer_info->blk_info[i].blk_height_b);
        //balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].blk_w_delta_width_b =%d\n", i, rot_layer_info->blk_info[i].blk_w_delta_width_b);
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].rdma_addr=0x%x\n", i, rot_layer_info->blk_info[i].rdma_addr);
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].blk_width_l=%d\n", i, rot_layer_info->blk_info[i].blk_width_l);
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].blk_height_l=%d\n", i, rot_layer_info->blk_info[i].blk_height_l);
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].wdma_addr=0x%x\n", i, rot_layer_info->blk_info[i].wdma_addr);
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].uv_rdma_addr=0x%x\n", i, rot_layer_info->blk_info[i].rdma_uv_addr);
        balongfb_logi_display_debugfs("ade_overlay_layer_get_blk_info: rot_layer_info->blk_info[%d].uv_wdma_addr=0x%x\n", i, rot_layer_info->blk_info[i].wdma_uv_addr);
    }
}

void ade_overlay_set_clip_bypass(struct ch_unit_reg *unit_reg, u32 layer_size, struct cmdfile_buffer  *cf_buff)
{
    ade_cmdq_wr_cmd(unit_reg->clip_disable, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(unit_reg->clip_size0, layer_size);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(unit_reg->clip_cfg_ok, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

void ade_overlay_set_ovly1_bypass(struct ade_overlay_ovly  *ovly, struct overlay_layer_info  *ade_layer_info,
        u32 width, u32 height, struct cmdfile_buffer  *cf_buff)
{

    u32 surf_num;
    struct ovly_reg *ovly_reg;
    struct ovly_ctrl_info ch_ctrl_info;

    ovly_reg = &ovly->ovly_reg;
    surf_num = ade_layer_info->surface_num;

    /* set top ovly ctrl, select ovly of the surface_num  */
    set_TOP_OVLY_CTRL(ovly->ade_base, ovly_reg->ovly_ch_sel_start[surf_num], 1);

    /* xy0 */
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy0[surf_num], 0);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* xy1 */
    ade_cmdq_wr_cmd(ovly_reg->ovly_ch_xy1[surf_num], ((width - 1) << 16) | (height - 1));
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* the first ovly output size  */
    ade_cmdq_wr_cmd(ovly_reg->ovly_output_size, ((width- 1) << 16) | (height - 1));
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ch_ctrl_info.ch_num = ade_layer_info->ch_num;
    ch_ctrl_info.format = ade_layer_info->format;
    ch_ctrl_info.ovly_num = 0;
    ch_ctrl_info.blending = OVERLAY_BLENDING_NONE;
    ch_ctrl_info.dim_blending = 0;
    ch_ctrl_info.sec_ovly_ch_count = 0;
    ch_ctrl_info.ovly_type = ADE_OVLY_TYPE_BYPASS;
    ade_cmdq_wr_ovly_ch_ctrl_cmd(ovly_reg->ovly_ch_ctrl[surf_num], ch_ctrl_info);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(ovly_reg->ovly_ctrl, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

void ade_overlay_layer_set_after_ctran( u32 wdma_num, u32 dst_format , u32 width, u32 height, struct cmdfile_buffer  *cf_buff)
{
    bool need_fmc;

    /*****************config ctran ******************************/
    if (1 == wdma_num) {
        need_fmc = ade_overlay_need_rgb2yuv(dst_format);

        /* CTRAN3 enable or disable */
        if (!need_fmc) {
            /* bypass */
            ade_cmdq_wr_cmd(ADE_CTRAN3_DIS_REG, 1);
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
            ade_cmdq_wr_cmd(ADE_CTRAN3_IMAGE_SIZE_REG, (width * height - 1));
            ade_cmdq_wr_cmd(ADE_CTRAN3_CFG_OK_REG, 1);
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

            return;
        }

        ade_cmdq_wr_cmd(ADE_CTRAN3_DIS_REG, 0);
        ade_cmdq_wr_cmd(ADE_CTRAN3_MODE_CHOOSE_REG, 1);  /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv,ARGB888 to yuv422 */
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_ctran_chdc_cmd(ADE_CTRAN3_CHDC0_REG, 1);
        ade_cmdq_wr_ctran_csc_cmd(ADE_CTRAN3_CSC0_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(ADE_CTRAN3_IMAGE_SIZE_REG, (width * height- 1));
        ade_cmdq_wr_cmd(ADE_CTRAN3_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    } else if (3 == wdma_num) {
        /*****************bypass ctran4 ******************************/
        ade_cmdq_wr_cmd(ADE_CTRAN4_DIS_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(ADE_CTRAN4_IMAGE_SIZE_REG, (width * height - 1));
        ade_cmdq_wr_cmd(ADE_CTRAN4_CFG_OK_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    }  else {
        balongfb_loge(" wdma_num = %d is error \n", wdma_num);
    }
}

void ade_overlay_set_scl1_bypass(u32 layer_size, struct cmdfile_buffer * cf_buff)
{
    ade_cmdq_wr_cmd(ADE_SCL1_CTRL_REG, 0x400);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    ade_cmdq_wr_cmd(ADE_SCL1_ORES_REG, layer_size);
    ade_cmdq_wr_cmd(ADE_SCL1_IRES_REG, layer_size);
    ade_cmdq_wr_cmd(ADE_SCL1_START_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

#if WFD_SUPPORT
void ade_overlay_layer_set_wfd_rdma( struct ade_overlay_pipe *pipe, struct overlay_layer_info  *ade_layer_info, struct ade_overlay_ovly  *ovly, struct cmdfile_buffer  *cf_buf)
{
    u32                         bpp;
    OVLY1_RES                   ovly1_res;
    struct ch_unit_reg          *unit_reg;
    struct  ade_rect            *orect;
    struct  ade_rect            irect;

    BUG_ON(ade_layer_info == NULL);
    BUG_ON(pipe == NULL);
    BUG_ON(cf_buf == NULL);

    unit_reg = &pipe->pipe_reg;
    bpp = ade_overlay_format2bpp(ade_layer_info->format);

    ovly1_res.ul32 = ovly->ovly_cap;

    orect     = &(pipe->output_rect);
    orect->x  = ade_layer_info->dst_rect.x;
    orect->y  = ade_layer_info->dst_rect.y;
    orect->w  = ade_layer_info->width;
    orect->h  = ade_layer_info->height;

    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_rdma: begin \n");

    memcpy(&irect, orect, sizeof(struct ade_rect));

    ade_overlay_get_rect_from_rot(ade_layer_info->rotation, &irect, orect);
    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_rdma: pipe_info->phy_addr = 0x%x,DMA ROTATION = %d \n", ade_layer_info->phy_addr,ade_layer_info->rotation);
    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_rdma: output_rect.x = %d,output_rect.y=%d \n", orect->x,orect->y);
    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_rdma: output_rect.w = %d,output_rect.h=%d \n", orect->w,orect->h);
    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_rdma: phy_addr = 0x%04x \n", ade_layer_info->phy_addr);
    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_rdma: format = 0x%04x \n", ade_layer_info->format);

    /* set burst length, qos */
    ade_cmdq_wr_rdma_pe_cmd(unit_reg->ch_rdma_ch_pe, OVERLAY_PIPE_TYPE_OFFLINE, ade_layer_info->rotation);

    if (ade_layer_info->rotation > 0) {
        ade_cmdq_wr_rdma_ctrl_rot_cmd(unit_reg->ch_rdma_ch_ctrl, ade_layer_info->format, ade_layer_info->rotation);  /* CTRL */
    } else {
        ade_cmdq_wr_rdma_ctrl_cmd(unit_reg->ch_rdma_ch_ctrl, &pipe->pipe_info);
    }

    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_addr, ade_layer_info->phy_addr);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_size, ade_layer_info->height << 16 | ( ade_layer_info->width* bpp));
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_stride, ade_layer_info->stride);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_space, ade_layer_info->height * ade_layer_info->stride);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));

    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_partial_size, 0);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_partial_space, 0);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));


    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_partial_size, 0);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_partial_space, 0);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));

    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_extra_partial_width, 0);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));


    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_en, 1);
    ade_cmdq_wr_cmd2buff(cf_buf->vaddr, &(cf_buf->cmd_len));
    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_rdma: end \n");

}


void ade_overlay_layer_set_wfd_clip( struct ade_overlay_pipe *pipe, struct overlay_layer_info  *ade_layer_info, struct cmdfile_buffer  *cf_buff)
{
    u32 layer_size = 0;
    struct ch_unit_reg *unit_reg;

    BUG_ON(ade_layer_info == NULL);
    BUG_ON(pipe == NULL);
    BUG_ON(cf_buff == NULL);

    unit_reg = &pipe->pipe_reg;
    layer_size = (ade_layer_info->width- 1) << 16 | (ade_layer_info->height- 1);

    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_clip: layer_size = 0x%x \n",layer_size);

    /*******************bypass clip **************/
    ade_overlay_set_clip_bypass(unit_reg, layer_size, cf_buff);
}
void ade_overlay_layer_set_wfd_scl1(struct ade_overlay_pipe *pipe, struct overlay_layer_info  *ade_layer_info, struct cmdfile_buffer  *cf_buff)
{
    //int scl_iformat;
    u32 layer_size = 0;

    BUG_ON(ade_layer_info == NULL);
    BUG_ON(pipe == NULL);
    BUG_ON(cf_buff == NULL);

    if (!pipe->pipe_cap.bit_scl1_enable) {
        balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_scl: bit_scl1_enable is false \n");
        return;
    }

    //scl_iformat = getSclImageFormatType(ade_layer_info->format);
    layer_size = (ade_layer_info->height- 1) << 16 | (ade_layer_info->width- 1);

    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_scl: layer_size = 0x%x \n",layer_size);

    /*******************bypass scl1 **************/
    ade_overlay_set_scl1_bypass(layer_size, cf_buff);
}


void ade_overlay_layer_set_wfd_ctran(struct ade_overlay_pipe *pipe, struct overlay_layer_info  *ade_layer_info, struct cmdfile_buffer  *cf_buff)
{
    u32 layer_size = 0;
    //int scl_iformat;
    struct ch_unit_reg *unit_reg;

    BUG_ON(ade_layer_info == NULL);
    BUG_ON(pipe == NULL);
    BUG_ON(cf_buff == NULL);

    if (pipe->pipe_num != OVERLAY_PIPE_ADE_CH6) {
        balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_ctran: channel is not ch6 \n");
        return;
    }

    unit_reg = &pipe->pipe_reg;
    //scl_iformat = getSclImageFormatType(ade_layer_info->format);
    layer_size = ade_layer_info->width* ade_layer_info->height- 1;

    /* bypass */
    ade_cmdq_wr_cmd(unit_reg->ctran_dis, 1);
    ade_cmdq_wr_cmd(unit_reg->ctran_mode_choose, 3);  /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv, 3:argb2argb */
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));


    ade_cmdq_wr_cmd(unit_reg->ctran_image_size, layer_size);
    ade_cmdq_wr_cmd(unit_reg->ctran_cfg_ok, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

void ade_overlay_layer_set_wfd_ovly(struct ade_overlay_ovly  *ovly, struct overlay_layer_info  *ade_layer_info, struct cmdfile_buffer  *cf_buff)
{
    BUG_ON(ade_layer_info == NULL);
    BUG_ON(ovly == NULL);
    BUG_ON(cf_buff == NULL);

    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_ovly, begin\n");

    /******************bypass ovly1 *******************/
    ade_overlay_set_ovly1_bypass(ovly, ade_layer_info, ade_layer_info->width, ade_layer_info->height, cf_buff);

    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_ovly: end \n");
}


void ade_overlay_layer_set_wfd_after_ctran( struct ade_overlay_pipe *pipe, struct overlay_layer_info  *ade_layer_info, struct cmdfile_buffer  *cf_buff)
{
    ade_overlay_layer_set_after_ctran( ade_layer_info->wdma_num, ade_layer_info->dst_format , ade_layer_info->width, ade_layer_info->height, cf_buff);
}

void ade_overlay_layer_set_wfd_wdma(struct overlay_layer_info  *ade_layer_info, struct cmdfile_buffer  *cf_buff)
{
    u32 output_bpp;
    u32 wdma_stride;
    volatile U_WR_CH1_CTRL  wr_ch_ctrl;

    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_wdma: wdma num is %d\n", ade_layer_info->wdma_num);

    /***************** config wdma ***************************/
    output_bpp = ade_overlay_format2bpp(ade_layer_info->dst_format);
    wdma_stride = ade_layer_info->dst_stride;

    if (1 == ade_layer_info->wdma_num) {
        ade_cmdq_wr_rdma_pe_cmd(WR_CH1_PE_REG, OVERLAY_PIPE_TYPE_OFFLINE, ade_layer_info->rotation);

        wr_ch_ctrl.u32 = 0;
        wr_ch_ctrl.bits.wr_ch1_partial = 0;
        wr_ch_ctrl.bits.wr_ch1_wr_dir  = 0;
        wr_ch_ctrl.bits.wr_ch1_fsh_int_disable  = 0;
        wr_ch_ctrl.bits.wr_ch1_format  = ade_layer_info->dst_format;
        ade_cmdq_wr_cmd(WR_CH1_CTRL_REG, wr_ch_ctrl.u32);
        ade_cmdq_wr_cmd(WR_CH1_ADDR_REG, ade_layer_info->dst_phy_addr);
        ade_cmdq_wr_cmd(WR_CH1_SIZE_REG, (ade_layer_info->height << 16) | ((ade_layer_info->width) * output_bpp));
        ade_cmdq_wr_cmd(WR_CH1_STRIDE_REG, wdma_stride);
        ade_cmdq_wr_cmd(WR_CH1_SPACE_REG, ade_layer_info->height * wdma_stride);
        ade_cmdq_wr_cmd(WR_CH1_PARTIAL_SIZE_REG, 0);
        ade_cmdq_wr_cmd(WR_CH1_PARTIAL_SPACE_REG, 0);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(WR_CH1_EN_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    } else {
        balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd_wdma: wdma num is error");
    }
}


#endif
void ade_overlay_layer_set_blk_rot_rdma(int block_num, struct rot_block_info *blk, struct ade_overlay_pipe *pipe,
        struct rot_layer_info  *rot_layer_info, struct cmdfile_buffer  *cf_buff)
{
    u32     blk_uv_height_l;
    u32     bpp;
    struct ch_unit_reg *unit_reg;

    BUG_ON(blk == NULL);
    BUG_ON(rot_layer_info == NULL);
    BUG_ON(pipe == NULL);
    BUG_ON(cf_buff == NULL);

    unit_reg = &pipe->pipe_reg;
    bpp = ade_overlay_format2bpp(rot_layer_info->layer_format);

    if (block_num > 1) {
        if (rot_layer_info->wdma_num == 3) {
            ade_cmdq_wr_wait_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_WAIT_EVT_WDMA3_CMPL);
        } else {
            ade_cmdq_wr_wait_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_WAIT_EVT_WDMA1_CMPL);
        }
    }

    /* set burst length, qos */
    ade_cmdq_wr_rdma_pe_cmd(unit_reg->ch_rdma_ch_pe, OVERLAY_PIPE_TYPE_OFFLINE, rot_layer_info->layer_rot);

    /* if format is nv12, this is the Y config */
    ade_cmdq_wr_rdma_ctrl_rot_cmd(unit_reg->ch_rdma_ch_ctrl, rot_layer_info->layer_format, rot_layer_info->layer_rot);  /* CTRL */
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_addr, blk->rdma_addr);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_size, blk->blk_height_b << 16 | (blk->blk_width_b * bpp));
    balongfb_logw_display_debugfs("ade_overlay_layer_set_blk_rot_rdma: layer_stride = %d, bpp = %d\n",rot_layer_info->layer_stride, bpp);
    balongfb_logw_display_debugfs("ade_overlay_layer_set_blk_rot_rdma: blk_height_b = %d\n",blk->blk_height_b);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_stride, rot_layer_info->layer_stride); /* stride dir for mirror */
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_space, blk->blk_height_b * rot_layer_info->layer_stride);
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_partial_size, (blk->blk_height_l << 16) | (blk->blk_width_l * bpp));
    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_partial_space, blk->blk_height_l * rot_layer_info->layer_stride);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* UV */
    if ((ADE_NV12 == rot_layer_info->layer_format) || (ADE_NV21 == rot_layer_info->layer_format)) {

        blk_uv_height_l = blk->blk_height_l / 2;
        if (rot_layer_info->layer_rot == ADE_ROT_180) {
            blk_uv_height_l = 1;
        }

        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_addr, blk->rdma_uv_addr);
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_size, (blk->blk_height_b / 2) << 16 | blk->blk_width_b);
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_stride, rot_layer_info->layer_stride); /* stride dir for mirror */
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_space, (blk->blk_height_b / 2) * rot_layer_info->layer_stride);
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_partial_size, (blk_uv_height_l << 16) | blk->blk_width_l);
        ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_uv_partial_space, blk_uv_height_l * rot_layer_info->layer_stride);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    }

    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_extra_partial_width, 0);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(unit_reg->ch_rdma_ch_en, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

void ade_overlay_layer_set_blk_rot_clip(bool need_fast_ch, bool *need_multi_scl, struct rot_block_info *blk, struct ade_overlay_pipe *pipe,
        struct rot_layer_info  *rot_layer_info, struct cmdfile_buffer  *cf_buff)
{
    u32 blk_size = 0;
    int scl_iformat;
    struct ch_unit_reg *unit_reg;

    BUG_ON(blk == NULL);
    BUG_ON(rot_layer_info == NULL);
    BUG_ON(pipe == NULL);
    BUG_ON(cf_buff == NULL);
    BUG_ON(need_multi_scl == NULL);

    unit_reg = &pipe->pipe_reg;
    scl_iformat = getSclImageFormatType(rot_layer_info->layer_format);
    blk_size = (blk->blk_w_width_b - 1) << 16 | (blk->blk_w_height_b - 1);

    *need_multi_scl = false;
    if ((ADE_SCL_YUV420 == scl_iformat) && (rot_layer_info->layer_rot != ADE_ROT_180) && !need_fast_ch) {
        blk_size = (blk->blk_w_width_l - 1) << 16 | (blk->blk_w_height_l - 1);
        *need_multi_scl = true;
    }

    balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_clip: blk_size = 0x%x \n",blk_size);

    /*******************bypass clip **************/
    ade_overlay_set_clip_bypass(unit_reg, blk_size, cf_buff);
}

void ade_overlay_layer_set_blk_rot_scl(struct rot_block_info *blk, struct ade_overlay_pipe *pipe,
        struct rot_layer_info  *rot_layer_info, struct cmdfile_buffer  *cf_buff)
{
    int scl_iformat;
    u32 blk_size = 0;

    BUG_ON(blk == NULL);
    BUG_ON(rot_layer_info == NULL);
    BUG_ON(pipe == NULL);
    BUG_ON(cf_buff == NULL);

    if (!pipe->pipe_cap.bit_scl1_enable) {
        balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_scl: bit_scl1_enable is false \n");
        return;
    }

    scl_iformat = getSclImageFormatType(rot_layer_info->layer_format);
    blk_size = (blk->blk_w_height_b - 1) << 16 | (blk->blk_w_width_b - 1);

    if (scl_iformat == ADE_SCL_YUV420) {
        ade_overlay_set_blk_rot_scl_coef(pipe->ade_base, ADE_SCL1_HCOEF_0_REG, ADE_SCL1_VCOEF_0_REG);
        if (rot_layer_info->layer_rot != ADE_ROT_180) {
            /* config scl with small size block */
            blk_size = (blk->blk_w_height_l - 1) << 16 | (blk->blk_w_width_l - 1);
        }

        balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_scl: blk_size = 0x%x \n",blk_size);

        /*******************config scl1 **************/
        ade_cmdq_wr_blk_rot_scl_ctrl_cmd(ADE_SCL1_CTRL_REG, ADE_SCL_YUV420);
        ade_cmdq_wr_cmd(ADE_SCL1_HSP_REG, 0x8000);
        ade_cmdq_wr_cmd(ADE_SCL1_UV_HSP_REG, 0xF0004000);
        ade_cmdq_wr_cmd(ADE_SCL1_VSP_REG, 0x8000);
        ade_cmdq_wr_cmd(ADE_SCL1_UV_VSP_REG, 0xF0004000);
        ade_cmdq_wr_cmd(ADE_SCL1_ORES_REG, blk_size);
        ade_cmdq_wr_cmd(ADE_SCL1_IRES_REG, blk_size);
        ade_cmdq_wr_cmd(ADE_SCL1_START_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        return;
    }

    balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_scl: blk_size = 0x%x \n",blk_size);

    /*******************bypass scl1 **************/
    ade_overlay_set_scl1_bypass(blk_size, cf_buff);
}

void ade_overlay_layer_set_blk_rot_ctran(struct rot_block_info *blk, struct ade_overlay_pipe *pipe,
        struct rot_layer_info  *rot_layer_info, struct cmdfile_buffer  *cf_buff)
{
    u32 blk_size = 0;
    int scl_iformat;
    struct ch_unit_reg *unit_reg;

    BUG_ON(blk == NULL);
    BUG_ON(rot_layer_info == NULL);
    BUG_ON(pipe == NULL);
    BUG_ON(cf_buff == NULL);

    if (pipe->pipe_num != OVERLAY_PIPE_ADE_CH6) {
        balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_ctran: channel is not ch6 \n");
        return;
    }

    unit_reg = &pipe->pipe_reg;
    scl_iformat = getSclImageFormatType(rot_layer_info->layer_format);
    blk_size = blk->blk_w_width_b * blk->blk_w_height_b - 1;

    /* bypass ctran if format convertion is not necessary */
    if (scl_iformat == ADE_SCL_ARGB888) {
        ade_cmdq_wr_cmd(unit_reg->ctran_dis, 1);
        ade_cmdq_wr_cmd(unit_reg->ctran_mode_choose, 3);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(unit_reg->ctran_image_size, blk_size);
        ade_cmdq_wr_cmd(unit_reg->ctran_cfg_ok, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        return;
    }

    /* convert YUV444 to ARGB */
    ade_cmdq_wr_cmd(unit_reg->ctran_dis, 0);
    ade_cmdq_wr_cmd(unit_reg->ctran_mode_choose, 0);  /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv */
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_ctran_chdc_cmd(unit_reg->ctran_chdc0, 0); /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv */
    ade_cmdq_wr_ctran_csc_cmd(unit_reg->ctran_csc0, 0);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(unit_reg->ctran_image_size, blk_size);
    ade_cmdq_wr_cmd(unit_reg->ctran_cfg_ok, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}

void ade_overlay_layer_set_blk_rot_ovly(struct rot_block_info *blk, struct ade_overlay_ovly  *ovly,
        struct overlay_layer_info  *ade_layer_info, struct cmdfile_buffer  *cf_buff)
{

    BUG_ON(blk == NULL);
    BUG_ON(ade_layer_info == NULL);
    BUG_ON(ovly == NULL);
    BUG_ON(cf_buff == NULL);

    balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_ovly:begin \n");

    /******************bypass ovly1 *******************/
    ade_overlay_set_ovly1_bypass(ovly, ade_layer_info, blk->blk_w_width_b, blk->blk_w_height_b, cf_buff);

    balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_ovly: end \n");
}

void ade_overlay_layer_set_blk_rot_after_ctran(struct rot_block_info *blk, struct ade_overlay_pipe *pipe,
        struct rot_layer_info  *rot_layer_info, struct cmdfile_buffer  *cf_buff)
{
    ade_overlay_layer_set_after_ctran( rot_layer_info->wdma_num, rot_layer_info->dst_format ,blk->blk_w_width_b, blk->blk_w_height_b, cf_buff);

}

static void ade_overlay_layer_set_blk_rot_after_scl(struct rot_block_info *blk, OVLY1_RES  ovly1_res, struct cmdfile_buffer  *cf_buff)
{
    BUG_ON(blk== NULL);
    BUG_ON(cf_buff== NULL);

    if (!ovly1_res.bits.has_scl3) {
        balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_after_scl: ovly1_res.bits.has_scl3 = 0 \n");
        return;
    }

    /**************** bypass scl3 *******************************/
    ade_cmdq_wr_cmd(ADE_SCL3_CTRL_REG, 0x400);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &cf_buff->cmd_len);

    ade_cmdq_wr_cmd(ADE_SCL3_ORES_REG, (blk->blk_w_height_b - 1) << 16 | (blk->blk_w_width_b - 1));
    ade_cmdq_wr_cmd(ADE_SCL3_IRES_REG, (blk->blk_w_height_b - 1) << 16 | (blk->blk_w_width_b - 1));
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(ADE_SCL3_START_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
}


static void ade_overlay_layer_set_blk_rot_wdma(struct rot_block_info *blk, struct rot_layer_info  *rot_layer_info, struct cmdfile_buffer  *cf_buff)
{
    u32 output_bpp;
    u32 wdma_stride;
    u32 w3dma_blk_w_height_l;

    /***************** config wdma ***************************/
    output_bpp = ade_overlay_format2bpp(rot_layer_info->dst_format);
    wdma_stride = rot_layer_info->dst_stride;

    balongfb_logw_display_debugfs("**ade_overlay_layer_set_blk_rot_wdma: wdma num is %d, wdma_stride = %d \n", rot_layer_info->wdma_num, wdma_stride);
    balongfb_logw_display_debugfs("ade_overlay_layer_set_blk_rot_wdma: wdma num is %d, wdma_stride = %d \n", rot_layer_info->wdma_num, wdma_stride);
    balongfb_logw_display_debugfs("ade_overlay_layer_set_blk_rot_wdma: blk_w_height_b = %d, blk->blk_w_width_b= %d\n", blk->blk_w_height_b, blk->blk_w_width_b);
    balongfb_logw_display_debugfs("ade_overlay_layer_set_blk_rot_wdma: blk_w_height_l = %d, blk->blk_w_width_l= %d\n", blk->blk_w_height_l, blk->blk_w_width_l);
    if (3 == rot_layer_info->wdma_num) {
        ade_cmdq_wr_rdma_pe_cmd(WR_CH3_PE_REG, OVERLAY_PIPE_TYPE_OFFLINE, rot_layer_info->layer_rot);
        ade_cmdq_wr_blk_rot_wdma3_ctrl_cmd(WR_CH3_CTRL_REG, rot_layer_info->dst_format);
        ade_cmdq_wr_cmd(WR_CH3_ADDR_REG, blk->wdma_addr);
        ade_cmdq_wr_cmd(WR_CH3_SIZE_REG, (blk->blk_w_height_b << 16) | ((blk->blk_w_width_b) * output_bpp));
        ade_cmdq_wr_cmd(WR_CH3_STRIDE_REG, wdma_stride);
        ade_cmdq_wr_cmd(WR_CH3_SPACE_REG, blk->blk_w_height_b * wdma_stride);
        ade_cmdq_wr_cmd(WR_CH3_PARTIAL_SIZE_REG, (blk->blk_w_height_l << 16) | ((blk->blk_w_width_l) * output_bpp));
        ade_cmdq_wr_cmd(WR_CH3_PARTIAL_SPACE_REG, blk->blk_w_height_l * wdma_stride);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        if ((ADE_NV12 == rot_layer_info->dst_format) || (ADE_NV21 == rot_layer_info->dst_format)) {
            ade_cmdq_wr_cmd(WR_CH3_UV_ADDR_REG, blk->wdma_uv_addr);
            ade_cmdq_wr_cmd(WR_CH3_UV_SIZE_REG, ((blk->blk_w_height_b / 2) << 16) | (blk->blk_w_width_b));
            ade_cmdq_wr_cmd(WR_CH3_UV_STRIDE_REG, wdma_stride);
            ade_cmdq_wr_cmd(WR_CH3_UV_SPACE_REG, (blk->blk_w_height_b / 2) * wdma_stride);

            w3dma_blk_w_height_l = (blk->blk_w_height_l / 2);
            if (rot_layer_info->layer_rot == ADE_ROT_180) {
                w3dma_blk_w_height_l = 1;
            }

            ade_cmdq_wr_cmd(WR_CH3_UV_PARTIAL_SIZE_REG, (w3dma_blk_w_height_l << 16) | (blk->blk_w_width_l));
            ade_cmdq_wr_cmd(WR_CH3_UV_PARTIAL_SPACE_REG, w3dma_blk_w_height_l * wdma_stride);
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        } else {
            ade_cmdq_wr_cmd(WR_CH3_UV_ADDR_REG, 0);
            ade_cmdq_wr_cmd(WR_CH3_UV_SIZE_REG, 0);
            ade_cmdq_wr_cmd(WR_CH3_UV_STRIDE_REG, 0);
            ade_cmdq_wr_cmd(WR_CH3_UV_SPACE_REG, 0);
            ade_cmdq_wr_cmd(WR_CH3_UV_PARTIAL_SIZE_REG, 0);
            ade_cmdq_wr_cmd(WR_CH3_UV_PARTIAL_SPACE_REG, 0);
            ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
        }

        ade_cmdq_wr_cmd(WR_CH3_EN_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    } else if (1 == rot_layer_info->wdma_num) {
        ade_cmdq_wr_rdma_pe_cmd(WR_CH1_PE_REG, OVERLAY_PIPE_TYPE_OFFLINE, rot_layer_info->layer_rot);
        ade_cmdq_wr_blk_rot_wdma1_ctrl_cmd(WR_CH1_CTRL_REG, rot_layer_info->dst_format);
        ade_cmdq_wr_cmd(WR_CH1_ADDR_REG, blk->wdma_addr);
        ade_cmdq_wr_cmd(WR_CH1_SIZE_REG, (blk->blk_w_height_b << 16) | ((blk->blk_w_width_b) * output_bpp));
        ade_cmdq_wr_cmd(WR_CH1_STRIDE_REG, wdma_stride);
        ade_cmdq_wr_cmd(WR_CH1_SPACE_REG, blk->blk_w_height_b * wdma_stride);
        ade_cmdq_wr_cmd(WR_CH1_PARTIAL_SIZE_REG, (blk->blk_w_height_l << 16) | ((blk->blk_w_width_l) * output_bpp));
        ade_cmdq_wr_cmd(WR_CH1_PARTIAL_SPACE_REG, blk->blk_w_height_l * wdma_stride);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(WR_CH1_EN_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    } else {
        balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_wdma: wdma num is error");
    }
}

static void ade_overlay_layer_set_blk_rot_multi_blk(bool need_multi_scl, struct rot_block_info *blk, struct ade_overlay_pipe *pipe,
        struct rot_layer_info  *rot_layer_info, struct cmdfile_buffer  *cf_buff)
{
    u32  small_blk_num;
    int  k;
    struct ch_unit_reg  *unit_reg;

    if (!need_multi_scl) {
        balongfb_logi_display_debugfs("ade_overlay_layer_set_blk_rot_multi_blk: need_multi_scl is false \n");
        return;
    }

    unit_reg = &pipe->pipe_reg;

    /* config clip and scl with small size block when YUV420 format is used by indirect path
     * (not ch6-->wdma3 direct path) with 90/270 rotation
     */

    small_blk_num = (blk->blk_w_height_b / blk->blk_w_height_l) * (blk->blk_w_width_b/ blk->blk_w_width_l);
    for (k = 0; k < (small_blk_num - 1); k++) {
        /*******************config clip with small size block**************/
        ade_cmdq_wr_cmd(unit_reg->clip_disable, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(unit_reg->clip_size0, (blk->blk_w_width_l - 1) << 16 | (blk->blk_w_height_l - 1));
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        ade_cmdq_wr_cmd(unit_reg->clip_cfg_ok, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

        /*******************config scl with small size block**************/
        ade_cmdq_wr_blk_rot_scl_ctrl_cmd(ADE_SCL1_CTRL_REG, ADE_SCL_YUV420);
        ade_cmdq_wr_cmd(ADE_SCL1_HSP_REG, 0x8000);
        ade_cmdq_wr_cmd(ADE_SCL1_UV_HSP_REG, 0xF0004000);
        ade_cmdq_wr_cmd(ADE_SCL1_VSP_REG, 0x8000);
        ade_cmdq_wr_cmd(ADE_SCL1_UV_VSP_REG, 0xF0004000);
        ade_cmdq_wr_cmd(ADE_SCL1_ORES_REG, (blk->blk_w_height_l - 1) << 16 | (blk->blk_w_width_l - 1));
        ade_cmdq_wr_cmd(ADE_SCL1_IRES_REG, (blk->blk_w_height_l - 1) << 16 | (blk->blk_w_width_l - 1));
        ade_cmdq_wr_cmd(ADE_SCL1_START_REG, 1);
        ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
    }
}

static int ade_overlay_layer_set_blk_rot(struct ade_overlay_pipe *pipe, struct ade_overlay_ovly  *ovly,
        struct rot_layer_info  *rot_layer_info, struct overlay_layer_info  *ade_layer_info,
        struct cmdfile_buffer  *cf_buff)
{
    struct rot_block_info    *blk;
    OVLY1_RES   ovly1_res;
    int i;
    int j = 0;
    bool need_multi_scl = false;
    bool need_fast_ch = false;

    if ((pipe == NULL) || (ovly == NULL) || (rot_layer_info == NULL)
            || (ade_layer_info == NULL) || (cf_buff == NULL)) {
        balongfb_loge(" null ptr error \n ");
        return -EINVAL;
    }

    if ((OVERLAY_PIPE_ADE_CH6 != pipe->pipe_num) && (OVERLAY_PIPE_ADE_CH1 != pipe->pipe_num)) {
        balongfb_loge("ade_overlay_layer_set_blk_rot: the channe num %d is not ch1 or ch6 \n",pipe->pipe_num);
        return -EINVAL;
    }

    /* valid pipe is ch1 or ch6; valid wdma is w1 or w3 */
    if (((ADE_NV12 == rot_layer_info->layer_format) || (ADE_NV21 == rot_layer_info->layer_format))
            && (OVERLAY_PIPE_ADE_CH6 != pipe->pipe_num)) {
        balongfb_loge("ade_overlay_layer_set_blk_rot: layer is nv12 or nv21, but the channel %d is not %d \n",pipe->pipe_num, OVERLAY_PIPE_ADE_CH6);
        return -EINVAL;
    }

    if ((ade_layer_info->wdma_num != 1) && (ade_layer_info->wdma_num != 3)) {
        balongfb_loge(" ade_layer_info->wdma_num = %d is not wdma1 and wdma3 \n ", ade_layer_info->wdma_num);
        return -EINVAL;
    }

    ovly1_res.ul32 = ovly->ovly_cap;

    /********************** TOP *******************/
    /* if channel num is 6 and wdma is 3, don't config these modules: scl/ovly/ctrans */
    if ((OVERLAY_PIPE_ADE_CH6 == pipe->pipe_num) && (rot_layer_info->wdma_num == 3)) {
        set_TOP_SCL3_MUX_CFG_wdma3_trans(pipe->ade_base, 1);
        set_TOP_OVLY1_TRANS_CFG(pipe->ade_base, 0);

        /* config scl1 to ch1 */
        set_TOP_SCL1_MUX_CFG(pipe->ade_base, TOP_SCL1_ON_CH1);

        /* config scl3 to ch5 */
        set_TOP_SCL3_MUX_CFG_scl3_mux(pipe->ade_base, TOP_SCL3_ON_CH5);

        set_TOP_WDMA3_SRC_CFG(pipe->ade_base, TOP_WDMA3_SRC_RESERV);

        need_fast_ch = true;
    } else {
        if (3 == rot_layer_info->wdma_num) {
            set_TOP_WDMA3_SRC_CFG(pipe->ade_base, TOP_WDMA3_SRC_OVLY1);
            set_TOP_OVLY1_TRANS_CFG(pipe->ade_base, 0);
        } else {
            set_TOP_WDMA3_SRC_CFG(pipe->ade_base, TOP_WDMA3_SRC_RESERV);
            set_TOP_OVLY1_TRANS_CFG(pipe->ade_base, 1);
        }

        set_TOP_SCL3_MUX_CFG_wdma3_trans(pipe->ade_base, 0);
        need_fast_ch = false;
    }
    balongfb_logw_display_debugfs("ade_overlay_layer_set:need_fast_ch = %d \n", need_fast_ch);
    for (i = 0; i < 4; i++) {
        if (!rot_layer_info->blk_info[i].blk_enable) {
            balongfb_logi_display_debugfs("ade_overlay_layer_set:blk_info[%d].bld_enble = false \n", i);
            continue;
        }

        blk = &rot_layer_info->blk_info[i];

        /********************dma *********************/
        ade_overlay_layer_set_blk_rot_rdma(j, blk, pipe, rot_layer_info, cf_buff);

        /********************clip *********************/
        ade_overlay_layer_set_blk_rot_clip(need_fast_ch, &need_multi_scl, blk, pipe, rot_layer_info, cf_buff);

        /* if channel num is 6 and wdma is 3, don't config these modules: scl/ovly/ctrans */
        if (!need_fast_ch) {

            /********************scl1 *********************/
            ade_overlay_layer_set_blk_rot_scl(blk, pipe, rot_layer_info, cf_buff);

            /********************ctran *********************/
            ade_overlay_layer_set_blk_rot_ctran(blk, pipe, rot_layer_info, cf_buff);

            /********************ovly *********************/
            ade_overlay_layer_set_blk_rot_ovly(blk, ovly, ade_layer_info, cf_buff);

            /********************after ctran *********************/
            ade_overlay_layer_set_blk_rot_after_ctran(blk, pipe, rot_layer_info, cf_buff);

            /********************scl3 *********************/
            ade_overlay_layer_set_blk_rot_after_scl(blk, ovly1_res, cf_buff);
        }

        /********************wdma *********************/
        ade_overlay_layer_set_blk_rot_wdma(blk, rot_layer_info, cf_buff);

        /********************multi blk *********************/
        ade_overlay_layer_set_blk_rot_multi_blk(need_multi_scl, blk, pipe, rot_layer_info, cf_buff);

        j++;
    }

    if (1 == ade_layer_info->wdma_num) {
        ade_cmdq_wr_wait_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_WAIT_EVT_WDMA1_CMPL);
    } else if (3 == ade_layer_info->wdma_num) {
        ade_cmdq_wr_wait_cmd(cf_buff->vaddr, &(cf_buff->cmd_len), ADE_WAIT_EVT_WDMA3_CMPL);
    }

    return 0;
}



/****************  CMD QUEUE CFG end *******************/

#if WFD_SUPPORT
int ade_overlay_layer_set_wfd(struct ade_overlay_pipe *pipe, struct ade_overlay_ovly  *ovly,
        struct rot_layer_info  *rot_layer_info, struct overlay_layer_info  *ade_layer_info,
        struct cmdfile_buffer  *cf_buf)
{
    struct ch_unit_reg              *unit_reg;
    struct overlay_region_info      *pipe_info;
    struct ade_rect                 *orect;
    struct ovly_reg                 *ovly_reg;

    //u32     ade_base;

    BUG_ON(pipe == NULL);
    BUG_ON(ovly == NULL);
    BUG_ON(rot_layer_info == NULL);
    BUG_ON(ade_layer_info == NULL);
    BUG_ON(cf_buf == NULL);

    unit_reg    = &(pipe->pipe_reg);
    pipe_info   = &(pipe->pipe_info);
    orect       = &(pipe->output_rect);
    //ade_base    = pipe->ade_base;
    ovly_reg    = &ovly->ovly_reg;

    balongfb_logi_display_debugfs("ade_overlay_layer_set_wfd: begin \n");

    if (OVERLAY_PIPE_ADE_CH6 != pipe->pipe_num) {
        balongfb_loge("ade_overlay_layer_set_wfd: the channe num %d is not ch6 \n",pipe->pipe_num);
        return -EINVAL;
    }

    /* valid pipe is ch6; valid wdma is w1 */
    if ((ADE_RGBA_8888 != rot_layer_info->layer_format) && (OVERLAY_PIPE_ADE_CH6 != pipe->pipe_num)) {
        balongfb_loge("ade_overlay_layer_set_wfd: layer is nv12 or nv21, but the channel %d is not %d \n",pipe->pipe_num, OVERLAY_PIPE_ADE_CH6);
        return -EINVAL;
    }

    if (ade_layer_info->wdma_num != 1) {
        balongfb_loge(" ade_layer_info->wdma_num = %d is not wdma1  \n ", ade_layer_info->wdma_num);
        return -EINVAL;
    }

#if ADE_DEBUG_LOG_ENABLE
    if (g_command_file_wr_enable) {
        if (debug_cmd_vaddr == 0) {
            debug_cmd_vaddr =(int*) kzalloc(ADE_CMD_BUFF_SIZE_MAX, GFP_KERNEL);
        }
    } else {
        if (debug_cmd_vaddr != 0) {
            kzfree(debug_cmd_vaddr);
            debug_cmd_vaddr = 0;
        }
    }
#endif

    /******************************pipe init**********************************/
    pipe->need_cfg_rot_dma      = ADE_FALSE;
    pipe->need_cfg_scl2         = ADE_FALSE;

    pipe->pipe_num              = OVERLAY_PIPE_ADE_CH6;

    pipe->output_rect.x         = ade_layer_info->dst_rect.x;
    pipe->output_rect.y         = ade_layer_info->dst_rect.y;
    pipe->output_rect.w         = ade_layer_info->dst_rect.w;
    pipe->output_rect.h         = ade_layer_info->dst_rect.h;

    pipe->pipe_info.format      = ade_layer_info->format;
    pipe->pipe_info.rotation    = ade_layer_info->rotation;

    pipe->pipe_cap.bit_scl1_enable = ADE_TRUE;

    /***************************1, config TOP and generate cmdfile  **************/
    balongfb_logi_display_debugfs("***********ade_overlay_layer_set_wfd: config TOP and generate cmdfile \n");

    /* config scl1 to ch1 */
    set_TOP_SCL1_MUX_CFG(pipe->ade_base, TOP_SCL1_ON_CH6);

    /* config ctran3 to ovly1 */
    set_TOP_OVLY1_TRANS_CFG(pipe->ade_base, 1);

    /********************dma6 *********************/
    balongfb_logi_display_debugfs("***********ade_overlay_layer_set_wfd: config dma6 \n");
    ade_overlay_layer_set_wfd_rdma( pipe, ade_layer_info,ovly, cf_buf);

    /********************clip 6*********************/
    balongfb_logi_display_debugfs("***********ade_overlay_layer_set_wfd: config clip 6 \n");
    ade_overlay_layer_set_wfd_clip( pipe,ade_layer_info, cf_buf);

    /********************scl1 **********************/
    balongfb_logi_display_debugfs("***********ade_overlay_layer_set_wfd: config scl1\n");
    ade_overlay_layer_set_wfd_scl1(pipe, ade_layer_info, cf_buf);

    /********************ctran2 ********************/
    balongfb_logi_display_debugfs("***********ade_overlay_layer_set_wfd: config ctran2\n");
    ade_overlay_layer_set_wfd_ctran(pipe, ade_layer_info, cf_buf);

    /******************bypass ovly1 *******************/
    balongfb_logi_display_debugfs("***********ade_overlay_layer_set_wfd: bypass ovly1\n");
    ade_overlay_layer_set_wfd_ovly(ovly, ade_layer_info, cf_buf);

    /********************after ctran3 ****************/
    balongfb_logi_display_debugfs("***********ade_overlay_layer_set_wfd: after ctran3\n");
    ade_overlay_layer_set_wfd_after_ctran( pipe, ade_layer_info, cf_buf);

    /********************wdma1 ********************/
    balongfb_logi_display_debugfs("***********ade_overlay_layer_set_wfd:wdma1, dst_format = %d \n", rot_layer_info->dst_format);
    ade_overlay_layer_set_wfd_wdma(ade_layer_info, cf_buf);

    if (1 == ade_layer_info->wdma_num) {
        ade_cmdq_wr_wait_cmd(cf_buf->vaddr, &(cf_buf->cmd_len), ADE_WAIT_EVT_WDMA1_CMPL);
    }

    return 0;
}

#endif

int ade_overlay_layer_set_type(struct ade_overlay_pipe *pipe,struct overlay_layer_info  *ade_layer_info,
        struct rot_layer_info *rot_layer_info, struct ade_overlay_ovly  *ovly,
        struct cmdfile_buffer  *cf_buf)
{
    int ret;

#if WFD_SUPPORT
    if( OVERLAY_LAYER_SET_CTRAN == ade_layer_info->layer_set_type ){
        balongfb_logi_display_debugfs("ade_overlay_layer_set:call ade_overlay_layer_set_wfd \n");
        ret = ade_overlay_layer_set_wfd(pipe, ovly, rot_layer_info, ade_layer_info, cf_buf);
        if (ret != 0) {
            balongfb_loge("ade_overlay_layer_WFD error ");
            return -EINVAL;
        }
    }
#endif

    if( OVERLAY_LAYER_SET_BLK_ROT == ade_layer_info->layer_set_type ){
        if ((ade_layer_info->rotation > 0) && !pipe->pipe_cap.bit_rot_enable) {
            balongfb_loge("pipe need rotation ,but pipe block rot is disable \n");
            return -ENODEV;
        }
        /* get block info */
        ade_overlay_layer_get_blk_info(ade_layer_info, rot_layer_info);

        /***************************config TOP and generate cmdfile for block rot **************/
        ret = ade_overlay_layer_set_blk_rot(pipe, ovly, rot_layer_info, ade_layer_info, cf_buf);
        if ( ret < 0 ) {
            balongfb_loge(" ade_overlay_layer_set_blk_rot return error \n");
            return -EINVAL;
        }
    }

    return 0;
}

/***********************************overlay interface***************************/
#if ADE_MEM_SHARE_EN
void ade_overlay_mem_share_reg_val_get (
        struct ade_compose_data_type *ade_pri_data,
        u32 mem_share_en_flag,
        u32 *p_rot_buf_shr_out )
{
    mem_share_ade_drv_info *p_memshare_info;

    p_memshare_info = &(ade_pri_data->mem_share_drv_info);

    /* change ade mem share driver status */
    if (mem_share_en_flag == MEM_SHARE_FUNC_ENABLE) {
        /* enable mem share out */
        *p_rot_buf_shr_out = 1;

        /* change the drv status */
        if (p_memshare_info->drv_status == MEM_SHARE_DRV_WAIT_ALLOC_RSP)
            p_memshare_info->drv_status = MEM_SHARE_DRV_RCV_ALLOC_RSP;

    } else {
        /* disable mem share out */
        *p_rot_buf_shr_out = 0;

        /* change the drv status */
        if (p_memshare_info->drv_status == MEM_SHARE_DRV_WAIT_FREE_RSP)
            p_memshare_info->drv_status = MEM_SHARE_DRV_RCV_FREE_RSP;
    }
}

/*****************************************************************************
  memory share: if compose not use g2d
 *****************************************************************************/
void ade_overlay_mem_share_g3d_proc(u8* ade_base, struct ade_compose_data_type *ade_pri_data)
{
    mem_share_ade_drv_info *p_mem_share_info;

    p_mem_share_info = &(ade_pri_data->mem_share_drv_info);
    /* now use G3D, ADE rot buffer idle */
    if (p_mem_share_info->drv_status == MEM_SHARE_DRV_WAIT_ALLOC_RSP) {

        if (p_mem_share_info->drv_event == MEM_SHARE_EVENT_ALLOC_720P) {
            set_TOP_CTL_rot_buf_cfg(ade_base, 3);
        } else {
            set_TOP_CTL_rot_buf_cfg(ade_base, 2);
        }

        /* config top reg: memshare enable */
        set_TOP_CTL1_rot_buf_shr_out(ade_base, 1);

        p_mem_share_info->drv_status = MEM_SHARE_DRV_RCV_ALLOC_RSP;
    }

    if (p_mem_share_info->drv_status == MEM_SHARE_DRV_WAIT_FREE_RSP) {

        /* config top reg: memshare disable */
        set_TOP_CTL1_rot_buf_shr_out(ade_base, 0);

        p_mem_share_info->drv_status = MEM_SHARE_DRV_RCV_FREE_RSP;
    }
}
#endif

/* TOP Register cfg, Phase II, config to the top value */
int ade_overlay_res_set(struct ade_compose_data_type* ade_pri_data,  struct overlay_resource_info* res_info)
{
    u8*     ade_base = 0;
    int     ret = 0;
    struct  ade_overlay_ctrl *overlay_ctl;
    struct  cmdqueue_buff    *cmdq_list;
    struct  cmdfile_buffer   *cf_buff;
    u32     ch_type;

#if ADE_MEM_SHARE_EN
    u32     rot_buf_shr_out = 0;
#endif

    BUG_ON((ade_pri_data == NULL) || (res_info == NULL));

    /* There is a bug that burst length is 11 or bigger in V8R2 SFT platform. */
    if (SFT_BOARD == fb_get_board_type()) {
        g_ade_online_pe  = 0x3000004;
        g_ade_offline_pe = 0x3000002;
    }

    overlay_ctl = &ade_pri_data->overlay_ctl;
    cmdq_list   = ade_pri_data->cmdq_list;

    memcpy(&(overlay_ctl->ade_res_info), res_info, sizeof(struct overlay_resource_info));

    ade_base = ade_pri_data->ade_base;

    ret = ade_overlay_res_get_top_info(overlay_ctl);
    if (ret != 0) {
        balongfb_loge("get top info error");
        return -EINVAL;
    }

#if ADE_MEM_SHARE_EN
    if((overlay_ctl->ade_res_info.disp_ch_res.bits.ch_en) && (overlay_ctl->ade_res_info.comp_mode == OVERLAY_COMP_TYPE_ONLINE)){
        ade_overlay_mem_share_g3d_proc(ade_base, ade_pri_data);
    }
    ade_overlay_mem_share_reg_val_get(ade_pri_data, res_info->ade_reg_res.bits.memshare_en, &rot_buf_shr_out);

    overlay_ctl->ade_demux_list[TOP_DEMUX_ROT_BUF_SHR_OUT]  = rot_buf_shr_out;

    if (rot_buf_shr_out) {

        balongfb_logi_display_debugfs("Before MemShareOut demux_list[TOP_DEMUX_ROT_BUFF] = 0x%x \n",
                overlay_ctl->ade_demux_list[TOP_DEMUX_ROT_BUFF]);

        /* set default value to last frame value, when drv_event is free,we must record the value */
        overlay_ctl->ade_demux_list[TOP_DEMUX_ROT_BUFF] = get_TOP_CTL_rot_buf_cfg(ade_base);
        if (ade_pri_data->mem_share_drv_info.drv_event == MEM_SHARE_EVENT_ALLOC_720P) {
            overlay_ctl->ade_demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH5_CH6_720P;
        } else if (ade_pri_data->mem_share_drv_info.drv_event == MEM_SHARE_EVENT_ALLOC_1080P) {
            overlay_ctl->ade_demux_list[TOP_DEMUX_ROT_BUFF] = TOP_ROT_BUFF_CH5_1080P;
        }

        balongfb_logi_display_debugfs("After MemShareOut demux_list[TOP_DEMUX_ROT_BUFF] = 0x%x \n",
                overlay_ctl->ade_demux_list[TOP_DEMUX_ROT_BUFF]);
    }
#endif

    ret = ade_overlay_res_get_cmdbuff_index(res_info->comp_mode, cmdq_list);
    if (ret != 0) {
        balongfb_loge("get cmd buff index error");
        return -EINVAL;
    }

    if (res_info->comp_mode != OVERLAY_COMP_TYPE_ONLINE) {
        ch_type = OVERLAY_PIPE_TYPE_OFFLINE;
    } else {
        ch_type = OVERLAY_PIPE_TYPE_ONLINE;
    }

    cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, ch_type);
    if (cf_buff == NULL) {
        balongfb_loge("cf_list is null \n");
        return -ENODEV;
    }

    ade_cmdq_wr_cmd(RD_CH5_CTRL_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_ADDR_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_SIZE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_STRIDE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_SPACE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_PARTIAL_SIZE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_PARTIAL_SPACE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_UV_ADDR_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_UV_SIZE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_UV_STRIDE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_UV_SPACE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_UV_PARTIAL_SIZE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_UV_PARTIAL_SPACE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_EXTRA_PARTIAL_WIDTH_REG, 0);
    ade_cmdq_wr_cmd(RD_CH5_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    mutex_lock(&ade_pri_data->ade_lock);
    ade_overlay_res_top_info_cfg(ade_pri_data, overlay_ctl->ade_demux_list, overlay_ctl->ade_mux_list);
    mutex_unlock(&ade_pri_data->ade_lock);

    return 0;
}

int ade_overlay_region_set(struct ade_compose_data_type *ade_pri_data, struct overlay_region_info *region_info)
{
    struct ade_overlay_pipe     *pipe  = NULL;
    struct overlay_region_info  *pinfo = NULL;
    struct ade_overlay_rot      *rot_info = NULL;
    struct ade_overlay_scl2     *scl2_info = NULL;
    struct ade_overlay_ctrl     *overlay_ctl = NULL;
    struct cmdfile_buffer       *cf_buff;
    struct ade_rect             *out_rect;

    BUG_ON((ade_pri_data == NULL) || (region_info == NULL));

    if (region_info->surface_num >= OVERLAY_ADE_SURFACE_MAX) {
        balongfb_loge("region_info->surface_num %d is over %d \n", region_info->surface_num, OVERLAY_ADE_SURFACE_MAX);
        return -EINVAL;
    }

    balongfb_logi_display_debugfs("enter succ!, region_info->ch_num = %d \n", region_info->ch_num);

    overlay_ctl = &(ade_pri_data->overlay_ctl);

    pipe = ade_overlay_idx2pipe(overlay_ctl, region_info->ch_num);
    if (pipe == NULL) {
        balongfb_loge("pipe is null ");
        return -ENODEV;
    }

    rot_info = &(overlay_ctl->rot_info);
    out_rect = &(pipe->output_rect);
    scl2_info = &(overlay_ctl->scl2_info);

    pinfo    = &(pipe->pipe_info);
    memcpy(pinfo, region_info, sizeof(struct overlay_region_info));

    if ((pinfo->ovly_output_rect.w == 0)
            || (pinfo->ovly_output_rect.h == 0)) {
        balongfb_loge("ovly_output_rect.w = %d, ovly_output_rect.h=%d\n", pinfo->ovly_output_rect.w, pinfo->ovly_output_rect.h);
        return -EINVAL;
    }

    cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, pinfo->ch_type);
    if (cf_buff == NULL) {
        balongfb_loge("cf_list is null \n");
        return -ENODEV;
    }

    mutex_lock(&ade_pri_data->ade_lock);

    /* the config sequence must be in turn,
     * such as RDMA->CLIP->SCL->CTRAN->OVLY->ROT->SCL2
     */

    /*****************DMA and ch rot**************/
    ade_overlay_region_set_rdma_cmd(ade_pri_data, pipe, rot_info, cf_buff);

    /*************** clip *****/
    ade_overlay_region_set_clip_cmd(ade_pri_data, pipe, cf_buff);

    /*************** scl ******/
    ade_overlay_region_set_scl_cmd(ade_pri_data, pipe, cf_buff);

    /*************** ctrans ******/
    ade_overlay_region_set_ctran_cmd(ade_pri_data, pipe, cf_buff);

    /*************** ovly ******/
    ade_overlay_region_set_ovly_cmd(ade_pri_data, overlay_ctl, pipe, cf_buff);

    /*************** ROT *******/
    ade_overlay_region_set_rot_cmd(ade_pri_data, pipe, rot_info, cf_buff);

    /*************** SCL2 ******/
    ade_overlay_region_set_scl2_cmd(ade_pri_data, pipe, scl2_info, cf_buff);

    /*************** second ovly *******/
    ade_overlay_region_set_sec_ovly_cmd(ade_pri_data, overlay_ctl, pipe, cf_buff);

    mutex_unlock(&ade_pri_data->ade_lock);

    balongfb_logi_display_debugfs("exit! \n");

    return 0;
}

int ade_overlay_commit(struct ade_compose_data_type *ade_pri_data, struct overlay_compose_info *comp_info)
{
    struct overlay_compose_info  *pcomp_info = NULL;
    struct overlay_resource_info *ade_res_info = NULL;
    struct ade_overlay_ctrl      *overlay_ctl = NULL;
    struct cmdfile_buffer        *online_cf_buff = NULL;
    struct cmdfile_buffer        *offline_cf_buff = NULL;
    struct cmdfile_buffer        *cf_buff = NULL;
    struct commit_comp_unit       commit_uint;
    struct ade_overlay_scl2      *scl2_info = NULL;
    struct ade_overlay_ovly      *ovly = NULL;
    u8*    ade_base;
    int    rslt = 0;
    u32    i;


    BUG_ON((ade_pri_data == NULL) || (comp_info == NULL));

    overlay_ctl  = &(ade_pri_data->overlay_ctl);
    pcomp_info   = &(overlay_ctl->comp_info);
    ade_res_info = &(overlay_ctl->ade_res_info);

    ade_base = ade_pri_data->ade_base;

    memcpy(pcomp_info, comp_info, sizeof(struct overlay_compose_info));

    ade_overlay_commit_get_unit(ade_res_info, &commit_uint);

    /* get command buffer, offline and hybird mode also need online cmdqueue */
    online_cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, OVERLAY_PIPE_TYPE_ONLINE);
    if (online_cf_buff == NULL) {
        balongfb_loge("get online cmd buff error");
        return -EINVAL;
    }

    if (pcomp_info->compose_mode != OVERLAY_COMP_TYPE_ONLINE) {
        offline_cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, OVERLAY_PIPE_TYPE_OFFLINE);
        if (offline_cf_buff == NULL) {
            balongfb_loge("get offline cmd buff error");
            return -EINVAL;
        }
    }

    /**************1, config after compose rotation ********************/
    if (commit_uint.bit_rot) {
        cf_buff = online_cf_buff;
        if (ade_res_info->rot_res.bits.has_offline) {
            cf_buff = offline_cf_buff;
        }

        ade_overlay_commit_set_rotation(overlay_ctl, pcomp_info, cf_buff, ade_base);
    }


    /**************2, config after compose SCL2 ********************/
    if (commit_uint.bit_scl2) {
        cf_buff = online_cf_buff;
        if (ade_res_info->scl2_res.bits.has_offline) {
            cf_buff = offline_cf_buff;
        }

        scl2_info = &(overlay_ctl->scl2_info);
        ade_overlay_commit_set_scl2(ade_pri_data, overlay_ctl, pcomp_info, cf_buff, ade_base);
    }

    /***************3, config sencond ovly or enable ovly start*****************
     * such as (ch1->ovly3 + ch2->ovly3)=>ROT=>SCL2=>OVLY2(OVLY1)=>LDI or
     *         (ch1->ovly3 + ch2->ovly3)=>ROT=>OVLY2=>SCL2=>LDI or
     *         (ch1->ovly3 + ch2->ovly3)=>SCL2=>OVLY2(OVLY1)=>LDI.
     *
     *  chx->ovly3->scl2->ovly2->ldi or chx->ovly3->rot->ovly2->scl2
     *  or chx->ovly2->ldi or chx->ovly1, need config ovly start enable.
     *
     *  if offline_phy_addr == 0, we need config online, else we need config offline
     */
    if ((pcomp_info->compose_mode != OVERLAY_COMP_TYPE_OFFLINE)
            && (0 == pcomp_info->offline_phy_addr)) {
        cf_buff = online_cf_buff;
        ovly = &overlay_ctl->ovly_list[OVERLAY_OVLY_2];
        if (ade_res_info->ovly2_res.bits.ovly_en) {
            ade_overlay_commit_set_sec_ovly(overlay_ctl, ade_res_info, ovly, pcomp_info, cf_buff);
        } else {
            balongfb_logi_display_debugfs("ade_overlay_commit:overlay_pipe_ade_disp\n");
        }
    }

    if ((pcomp_info->compose_mode != OVERLAY_COMP_TYPE_ONLINE)
            && (0 != pcomp_info->offline_phy_addr)) {
        cf_buff = offline_cf_buff;
        ovly = &overlay_ctl->ovly_list[OVERLAY_OVLY_1];
        if (ade_res_info->ovly1_res.bits.ovly_en) {
            ade_overlay_commit_set_sec_ovly(overlay_ctl, ade_res_info, ovly, pcomp_info, cf_buff);
        } else {
            balongfb_logi_display_debugfs("ade_overlay_commit:overlay_pipe_ade_disp\n");
        }
    }

    /***************4, config offline cmdq_rdma ****************************/
    ade_overlay_commit_set_offline(ade_pri_data, &commit_uint, pcomp_info, offline_cf_buff);

    /***************5, config online cmdq_rdma ****************************/
    rslt = ade_overlay_commit_set_online(ade_pri_data, &commit_uint, pcomp_info, online_cf_buff);
    if (rslt == 0) {

        /* if set online return 0, means that this frame have finish config and start to display */
        if (scl2_info != NULL) {
            scl2_info->scl2_state = ADE_SCL2_STATE_IDLE;
        }
    }

    /* clear overlay's parameters */
    for (i = OVERLAY_OVLY_1; i < OVERLAY_OVLY_MAX; i++) {
        ovly = &(overlay_ctl->ovly_list[i]);

        /* clear it for low resolution */
        ovly->ovly_comp.ch_count = 0;
    }

    balongfb_logi_display_debugfs("ade_overlay_commit:ade_pri_data->have_refresh= %d \n", ade_pri_data->have_refresh);
    balongfb_logi_display_debugfs("ade_overlay_commit: top ade shadow ovly ctrl =0x%x \n", inp32(ade_base + ADE_OVLY_CTL_REG));
    balongfb_logi_display_debugfs("exit succ !\n");
    return rslt;
}

int ade_overlay_refresh(struct ade_compose_data_type    *ade_pri_data, u32 refresh_mode)
{
    u32 ldi_src = 0;
    u8* ade_base;
    u32 wdma2_src = 0;
    int ret;
    struct cmdfile_buffer  *online_cf_buff;
    struct cmdfile_buffer  *old_online_cf_buff;
    struct balong_fb_data_type  *fb_data;

    BUG_ON(ade_pri_data == NULL);

    ade_base = ade_pri_data->ade_base;

    balongfb_logi_display_debugfs("ade_overlay_refresh enter succ ! \n");
    fb_data  = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);

    /* config refresh for CMD LCD */
    if (PANEL_MIPI_CMD == ade_pri_data->lcd_type) {
        set_TOP_OVLY_CTRL_all(ade_base, 0);
        ade_pri_data->have_refresh = false;
        ret = wait_event_interruptible_timeout(fb_data->frame_wq, fb_data->update_frame, HZ);
        if (ret <= 0) {
            balongfb_logw("ade_overlay_refresh:wait_event_interruptible_timeout frame_wq!\n");
            if (fb_data->refresh > 0) {
                sw_sync_timeline_inc(fb_data->timeline, fb_data->refresh);
                fb_data->refresh = 0;
            }
        }

        return 0;
    }

    /* get old online cmdfile buff */
    old_online_cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, OVERLAY_PIPE_TYPE_ONLINE);
    if (NULL == old_online_cf_buff) {
        balongfb_loge("get old_online_cf_buff error");
        return -EINVAL;
    }

    ret = ade_overlay_res_get_cmdbuff_index(OVERLAY_COMP_TYPE_ONLINE, ade_pri_data->cmdq_list);
    if (ret != 0) {
        balongfb_loge("get cmd buff index error");
        return -EINVAL;
    }
    /* get old online cmdfile buff */
    online_cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, OVERLAY_PIPE_TYPE_ONLINE);
    if (NULL == online_cf_buff) {
        balongfb_loge("get online_cf_buff error");
        return -EINVAL;
    }

    /* mode switch */
    if (ADE_REFRESH_SWITCH == refresh_mode) {
        ade_overlay_refresh_pre_frm_info(ade_pri_data, online_cf_buff, old_online_cf_buff);
    } else {
        ldi_src = inp32(ade_base + ADE_DISP_SRC_CFG_REG);
        wdma2_src = ade_overlay_disp_src2wdma2_src(ldi_src);
        set_TOP_WDMA2_SRC_CFG(ade_base, wdma2_src); /* set wdma2 top */

        ade_overlay_refresh_set_wdma2(ade_pri_data, online_cf_buff);
    }

    ade_pri_data->have_refresh = true;

    balongfb_logi_display_debugfs("ade_overlay_refresh exit succ ! \n");

    return 0;
}


/* ade_overlay_layer_set: it can be use to block rot, or scl, or ctran once, if the channel
 * support it. such as: use ch1/ch6 to block rotate, use ch5/ch6 to make yuv2rgb, or use ch1/ch5/ch6
 * to scalor only once.
 * the right flow is, first, send ADE_OVERLAY_RES_SET cmd to config TOP, then, send ADE_OVERLAY_BLOCK_SET cmd
 * to config echo module and finish the configuration.
 * DON'T send the ADE_OVERLAY_REGION_SET, and ADE_OVERLAY_COMMIT cmd, it DOESN'T work.
 */
int ade_overlay_layer_set(struct ade_compose_data_type  *ade_pri_data, struct overlay_layer_info  *ade_layer_info)
{
    struct cmdqueue_buff     *cmdq_list;
    struct cmdfile_buffer    *cf_buff;
    struct rot_layer_info     rot_layer_info;
    struct ade_overlay_pipe  *pipe  = NULL;
    struct ade_overlay_ctrl  *overlay_ctl = NULL;
    struct ade_overlay_ovly  *ovly;
    struct balong_fb_data_type *balongfd = NULL;
    int                       ret;
    u8*                       ade_base;
    u32                       reload_dis0;
    u32                       reload_dis1;
    bool                      ade_top_change = true;

    BUG_ON(ade_pri_data == NULL);
    BUG_ON(ade_layer_info == NULL);

#ifndef PC_UT_TEST_ON
#ifdef CONFIG_TRACING
    trace_dot(SF, "5", 0);
#endif
#endif

    if (ade_layer_info->ch_num >= OVERLAY_ADE_SURFACE_MAX) {
        balongfb_loge("region_info->surface_num %d is over %d \n", ade_layer_info->ch_num, OVERLAY_ADE_SURFACE_MAX);
        return -EINVAL;
    }
#if WFD_SUPPORT
    /************************** switch cmfbuffer *********************/
    balongfb_logi_display_debugfs("ade_overlay_layer_set,  layer_set_type = %d \n", ade_layer_info->layer_set_type);
    if (OVERLAY_LAYER_SET_CTRAN == ade_layer_info->layer_set_type) {
        ret = ade_overlay_res_get_cmdbuff_index(OVERLAY_COMP_TYPE_COPYBIT, ade_pri_data->cmdq_list);
        if (ret != 0) {
            balongfb_loge("get cmd buff index error");
            return -EINVAL;
        }
    }
#endif

    cmdq_list   = ade_pri_data->cmdq_list;

    cf_buff = ade_overlay_type2cmdfile(cmdq_list, OVERLAY_PIPE_TYPE_COPYBIT);
    if (cf_buff == NULL) {
        balongfb_loge("cf_list is null \n");
        return -EINVAL;
    }

    overlay_ctl = &(ade_pri_data->overlay_ctl);
    pipe = ade_overlay_idx2pipe(overlay_ctl, ade_layer_info->ch_num);
    if (pipe == NULL) {
        balongfb_loge("pipe is null \n");
        return -ENODEV;
    }

    balongfd = (struct balong_fb_data_type *)platform_get_drvdata(ade_pri_data->parent);
    BUG_ON(balongfd == NULL);

    ade_base = pipe->ade_base;
    ovly = &(overlay_ctl->ovly_list[OVERLAY_OVLY_1]);

    /* get block for RDMA, cut layer to several block, maybe 1,or 2, or 4 */
    memset(&rot_layer_info, 0, sizeof(rot_layer_info));
    rot_layer_info.layer_format = ade_layer_info->format;
    rot_layer_info.layer_width  = ade_layer_info->width;
    rot_layer_info.layer_height = ade_layer_info->height;
    rot_layer_info.layer_stride = ade_layer_info->stride;
    rot_layer_info.layer_rot    = ade_layer_info->rotation;
    rot_layer_info.wdma_num     = ade_layer_info->wdma_num;

    /* Currently we only support wdma1's output format different from input format. */
    rot_layer_info.dst_format   = ade_layer_info->dst_format;
    rot_layer_info.dst_stride   = ade_layer_info->dst_stride;

    /* clear cmdq_rdma1 */
    if (PANEL_MIPI_CMD == balongfd->panel_info.type) {
        set_TOP_SOFT_RST_SEL0_cmdq1_rdma(ade_base, 0);
        set_TOP_ADE_OVLY_CTRL(ade_base, 0);
    }

    if (0 == ade_pri_data->frame_count) {
        /* 1, config top for ade block rot */
        set_TOP_CTL_frm_end_start(ade_base, 2);
        set_TOP_ADE_OVLY_CTRL(ade_base, 0);
    }

    balongfb_logi_display_debugfs("-ade_overlay_layer_set:ade_pri_data->ovly_ctrl main = 0x%x \n", inp32(ade_base + 0x8000 + ADE_OVLY_CTL_REG));
    balongfb_logi_display_debugfs("-ade_overlay_layer_set:ade_pri_data->ovly_ctrl shadow= 0x%x \n", inp32(ade_base + ADE_OVLY_CTL_REG));
    balongfb_logi_display_debugfs("ade_overlay_layer_set:ade_pri_data->frame_count = %d \n", ade_pri_data->frame_count);

    /* config channel to AXI1 */
    set_TOP_DMA_AXI_MUX(pipe->ade_base, pipe->pipe_num, OVERLAY_PIPE_TYPE_OFFLINE);
    ret = ade_overlay_layer_set_type(pipe, ade_layer_info, &rot_layer_info, ovly, cf_buff );
    if (ret < 0) {
        balongfb_logw("ade_overlay_layer_set_type fail !\n");
        return -EINVAL;
    }
    ade_cmdq_wr_eof_cmd(cf_buff->vaddr, &(cf_buff->cmd_len));

    dsb(sy);

    ade_overlay_commit_set_offline_cmdq(ade_base, cf_buff, 0xffff);

    /***************************3, enable resource switch intr ********************/
    ade_pri_data->res_switch_cmpl = 0;
    ade_pri_data->cmdq2_cmpl = 0;
    clear_TOP_INTR_ORG_CPU0(ade_base, 0xffffffff);
    clear_TOP_INTR_ORG_CPU1(ade_base, 0xffffffff);
    set_TOP_INTR_MASK_CPU1_bit(ade_base, ADE_ISR1_RES_SWITCH_CMPL);
    set_TOP_INTR_MASK_CPU1_bit(ade_base, ADE_ISR1_CMDQ2_CMPL);

    balongfb_logi_display_debugfs("+ade_overlay_layer_set: ade_reset_sel0 = 0x%x \n", inp32(ade_base + ADE_SOFT_RST_SEL0_REG));
    balongfb_logi_display_debugfs("+ade_overlay_layer_set: ade_reset_sel1 = 0x%x \n", inp32(ade_base + ADE_SOFT_RST_SEL1_REG));
    balongfb_logi_display_debugfs("+ade_overlay_layer_set: ade_state0 = 0x%x \n", inp32(ade_base + ADE_STAT0_REG));
    balongfb_logi_display_debugfs("+ade_overlay_layer_set: ade_state1 = 0x%x \n", inp32(ade_base + ADE_STAT1_REG));

    /***************************4, enable ADE_EN ********************/

    ade_top_change = ade_overlay_check_ade_top_change(ade_base);
    if (ade_top_change) {
        set_TOP_ADE_EN(ade_base, ADE_TRUE);

        /***************************5, wait resource switch cmpl intr ********************/
        if (wait_event_interruptible_timeout(ade_pri_data->wait_res_cmpl, ade_pri_data->res_switch_cmpl, (HZ/4)) <= 0) {
            balongfb_loge("wait_event_interruptible_timeout wait_res_cmpl !\n");
            set_LDI_DSI_CMD_MOD_CTRL(0x0);
            mdelay(1);
            set_LDI_DSI_CMD_MOD_CTRL(0x2);
        }
        ade_pri_data->res_switch_cmpl = 0;
    }
    balongfb_logi_display_debugfs("+ade_overlay_layer_set:ade_pri_data->ovly_ctrl main = 0x%x \n", inp32(ade_base + 0x8000 + ADE_OVLY_CTL_REG));
    balongfb_logi_display_debugfs("+ade_overlay_layer_set:ade_pri_data->ovly_ctrl shadow = 0x%x \n", inp32(ade_base + ADE_OVLY_CTL_REG));

    balongfb_logi_display_debugfs("+ade_overlay_layer_set: ade_state0 = 0x%x \n", inp32(ade_base + ADE_STAT0_REG));
    balongfb_logi_display_debugfs("+ade_overlay_layer_set: ade_state1 = 0x%x \n", inp32(ade_base + ADE_STAT1_REG));
    balongfb_logi_display_debugfs("+ade_overlay_layer_set: ldi_ctrl = 0x%x \n", inp32(ade_base + LDI_CTRL_REG));

    /***************************6, enable cmdq_rdma2 ********************/
    set_CMDQ_RDMA2_EN(ade_base, 1);

    /***************************7, wait cmdq2 cmpl intr ********************/
    balongfb_logi_display_debugfs("ade_overlay_layer_set:cmdq2_cmpl=%d\n",ade_pri_data->cmdq2_cmpl);
    if (wait_event_interruptible_timeout(ade_pri_data->wait_cmdq2_cmpl, ade_pri_data->cmdq2_cmpl, (HZ * 2)) <= 0) {
        balongfb_loge("wait_event_interruptible_timeout wait_cmdq2_cmpl !\n");
        ade_overlay_dump_ade_top_reg(ade_base);

        reload_dis0 = inp32(ade_base + ADE_RELOAD_DIS0_REG + 0x8000);
        reload_dis1 = inp32(ade_base + ADE_RELOAD_DIS1_REG + 0x8000);
        outp32(ade_base + ADE_SOFT_RST0_REG, ~reload_dis0);
        outp32(ade_base + ADE_SOFT_RST1_REG, ~reload_dis1);
    }
    ade_pri_data->cmdq2_cmpl = 0;

    /***************************8, change top resource back  ********************/
    set_TOP_WDMA3_SRC_CFG(ade_base, overlay_ctl->ade_mux_list[TOP_MUX_WDMA3_SRC].mux_src);
    set_TOP_OVLY1_TRANS_CFG(ade_base, overlay_ctl->ade_demux_list[TOP_DEMUX_OVLY1]);
    set_TOP_SCL3_MUX_CFG_scl3_mux(ade_base, overlay_ctl->ade_demux_list[TOP_DEMUX_SCL3]);
    set_TOP_SCL3_MUX_CFG_wdma3_trans(ade_base, 0);
    set_TOP_SCL1_MUX_CFG(ade_base, overlay_ctl->ade_demux_list[TOP_DEMUX_SCL1]);

    //set_TOP_ADE_OVLY_CTRL(ade_base, 0);

    /***************************9 switch to new cmdfile for offline ************/
    if (OVERLAY_LAYER_SET_CTRAN != ade_layer_info->layer_set_type) {
        ret = ade_overlay_res_get_cmdbuff_index(OVERLAY_COMP_TYPE_COPYBIT, cmdq_list);
        if (ret != 0) {
            balongfb_loge("get cmd buff index error");
            return -EINVAL;
        }
    }

#ifndef PC_UT_TEST_ON
#ifdef CONFIG_TRACING
    trace_dot(SF, "6", 0);
#endif
#endif

    return 0;
}

/*****************************framebuffer interface******************/
int ade_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
    struct balong_fb_data_type *balongfd = NULL;
    struct cmdfile_buffer      *cf_buff;
    struct ade_compose_data_type    *ade_pri_data = NULL;
    u8* ade_base;
    u32 display_addr = 0;
    u32 stride = 0;
    int ret;

    BUG_ON((var == NULL) || (info == NULL));

    balongfd = (struct balong_fb_data_type *)info->par;

    balongfb_logi_display_debugfs("ade_fb_pan_display:enter succ ! \n");

    ade_base     = balongfd->ade_base;

#if PARTIAL_UPDATE
    display_addr = info->fix.smem_start + info->fix.line_length * var->yoffset
        +info->fix.line_length * balongfd->yoffset + balongfd->xoffset * (var->bits_per_pixel >> 3);

    balongfb_logi_display_debugfs("var->yoffset[%d],balongfd->xoffset[%d],balongfd->yoffset[%d]\n",\
            var->yoffset, balongfd->xoffset, balongfd->yoffset);
#else
    display_addr = info->fix.smem_start + info->fix.line_length * var->yoffset + var->xoffset * (var->bits_per_pixel >> 3);
#endif

    ade_pri_data = balongfd->ade_pri_data;

    ret = ade_overlay_res_get_cmdbuff_index(OVERLAY_COMP_TYPE_ONLINE, ade_pri_data->cmdq_list);
    if (ret != 0) {
        balongfb_loge("get cmd buff index error \n");
        return -EINVAL;
    }

    cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, OVERLAY_PIPE_TYPE_DISPLAY);
    if (cf_buff == NULL) {
        balongfb_loge("cf_list is null \n");
        return -EINVAL;
    }

    ade_pri_data->soft_rst0 = 0;
    ade_pri_data->soft_rst1 = 0;
    ade_pri_data->reload_dis0 = 0;
    ade_pri_data->reload_dis1 = 0;
    ade_pri_data->top_ovly_ctrl = 0;

    down(&balongfd->sem);

#if ADE_MEM_SHARE_EN
    ade_overlay_mem_share_g3d_proc(ade_base, ade_pri_data);
#endif

    /********************************TOP**************************/
    set_TOP_WDMA2_SRC_CFG(ade_base, 0);
    set_TOP_SCL3_MUX_CFG_scl3_mux(ade_base, 0);
    set_TOP_SCL1_MUX_CFG(ade_base, 0);
    set_TOP_ROT_SRC_CFG(ade_base, 0);
    set_TOP_SCL2_SRC_CFG(ade_base, 0);
    set_TOP_SEC_OVLY_SRC_CFG(ade_base, 0);
    set_TOP_WDMA3_SRC_CFG(ade_base, 0);
    set_TOP_OVLY1_TRANS_CFG(ade_base, 0);
    set_TOP_CTRAN5_TRANS_CFG(ade_base, 0);
    set_TOP_OVLY_CTRL_all(ade_base, 0);
    set_TOP_DISP_SRC_CFG(ade_base, TOP_DISP_CH_SRC_RDMA);

    set_TOP_SOFT_RST_SEL0(ade_base, 0);
    set_TOP_RELOAD_DIS0(ade_base, 0);
    set_TOP_SOFT_RST_SEL1(ade_base, 0);
    set_TOP_RELOAD_DIS1(ade_base, 0);

    /* set offline axi_mux */
    outp32(ade_base + ADE_DMA_AXI_MUX_REG, 0x763f);

    //disp_rdma
    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 6, balongfd->panel_info.type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 6);
    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 28, balongfd->panel_info.type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 28);
    set_soft_rst_sel_bit(ade_pri_data->soft_rst0, 29, balongfd->panel_info.type);
    set_reload_dis_bit(ade_pri_data->reload_dis0, 29);
    //cmdq1
    set_soft_rst_sel_bit(ade_pri_data->soft_rst1, 0, balongfd->panel_info.type);
    set_reload_dis_bit(ade_pri_data->reload_dis1, 0);
    set_soft_rst_sel_bit(ade_pri_data->soft_rst1, 3, balongfd->panel_info.type);
    set_reload_dis_bit(ade_pri_data->reload_dis1, 3);
    set_soft_rst_sel_bit(ade_pri_data->soft_rst1, 4, balongfd->panel_info.type);
    set_reload_dis_bit(ade_pri_data->reload_dis1, 4);

    set_TOP_SOFT_RST_SEL0(ade_base, ade_pri_data->soft_rst0);
    set_TOP_RELOAD_DIS0(ade_base, ade_pri_data->reload_dis0);
    set_TOP_SOFT_RST_SEL1(ade_base, ade_pri_data->soft_rst1);
    set_TOP_RELOAD_DIS1(ade_base, ade_pri_data->reload_dis1);

    if (PANEL_MIPI_CMD != balongfd->panel_info.type) {
        set_TOP_SOFT_RST_SEL0_cmdq1_rdma(ade_base, 1);
        set_TOP_RELOAD_DIS0_cmdq1_rdma(ade_base, 0);
    } else {
        set_TOP_SOFT_RST_SEL0_cmdq1_rdma(ade_base, 0);
        set_TOP_RELOAD_DIS0_cmdq1_rdma(ade_base, 1);
    }

    /********************************DISP DMA**************************/
    stride = info->fix.line_length;

    /* set disp channel into AXI0 */
    set_TOP_DMA_AXI_MUX(ade_base, OVERLAY_PIPE_ADE_DISP, OVERLAY_PIPE_TYPE_ONLINE);

    ade_cmdq_wr_rdma_pe_cmd(RD_CH_DISP_PE_REG, OVERLAY_PIPE_TYPE_ONLINE, ADE_ROT_NOP);
    ade_cmdq_wr_cmd(RD_CH_DISP_CTRL_REG, (balongfd->fb_imgType << 16) & 0x1f0000);
    ade_cmdq_wr_cmd(RD_CH_DISP_ADDR_REG, display_addr);

#if PARTIAL_UPDATE
    balongfb_logi_display_debugfs("ldi_y [%d], stride [%d]",balongfd->yheight, balongfd->xwidth, (balongfd->xwidth * (var->bits_per_pixel >> 3)));
    ade_cmdq_wr_cmd(RD_CH_DISP_SIZE_REG, (balongfd->yheight << 16) | (balongfd->xwidth * (var->bits_per_pixel >> 3)));
    ade_cmdq_wr_cmd(RD_CH_DISP_STRIDE_REG, stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_SPACE_REG, balongfd->yheight * stride);
#else
    ade_cmdq_wr_cmd(RD_CH_DISP_SIZE_REG, (var->yres << 16) | (var->xres_virtual * (var->bits_per_pixel >> 3)));
    ade_cmdq_wr_cmd(RD_CH_DISP_STRIDE_REG, stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_SPACE_REG, var->yres * stride);
#endif
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    ade_cmdq_wr_cmd(RD_CH_DISP_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /************************ ctran5 *************/
    ade_cmdq_wr_cmd(ADE_CTRAN5_DIS_REG, ADE_ENABLE);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));
#if PARTIAL_UPDATE
    ade_cmdq_wr_cmd(ADE_CTRAN5_IMAGE_SIZE_REG, balongfd->xwidth * balongfd->yheight - 1);
#else
    ade_cmdq_wr_cmd(ADE_CTRAN5_IMAGE_SIZE_REG, var->xres * var->yres - 1);
#endif
    ade_cmdq_wr_cmd(ADE_CTRAN5_CFG_OK_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /**************************CTRAN6***************************/
#if PARTIAL_UPDATE
    ade_overlay_commit_set_ctran6(ade_pri_data, ade_overlay_need_yuv2rgb(balongfd->fb_imgType), balongfd->xwidth, balongfd->yheight, cf_buff);
#else
    ade_overlay_commit_set_ctran6(ade_pri_data, ade_overlay_need_yuv2rgb(balongfd->fb_imgType), var->xres, var->yres, cf_buff);
#endif
    ade_cmdq_wr_eof_cmd(cf_buff->vaddr, &(cf_buff->cmd_len));

    dsb(sy);

    /* set ONLINE cmdQueue RDMA, 0xffff is invalid value */
    ade_overlay_commit_set_online_cmdq(ade_base, cf_buff, ADE_INVAL_PATTERN_NUM);
    if (0 == ade_pri_data->frame_count) {
        set_TOP_INTR_MASK_CPU1(ade_base, ADE_ISR1_RES_SWITCH_CMPL);
    }

    ade_pri_data->res_switch_cmpl = 0;
    set_TOP_ADE_EN(ade_base, 1);

    if (wait_event_interruptible_timeout(ade_pri_data->wait_res_cmpl, ade_pri_data->res_switch_cmpl, (HZ/4)) <= 0) {
        balongfb_logw("ade_fb_pan_display:wait_event_interruptible_timeout res_switch_cmpl timeout!\n");
        set_LDI_DSI_CMD_MOD_CTRL(0x0);
        mdelay(1);
        set_LDI_DSI_CMD_MOD_CTRL(0x2);
    }
    ade_pri_data->cmdq1_cmpl = 0;

    if (PANEL_MIPI_CMD == balongfd->panel_info.type) {
        set_CMDQ_RDMA1_EN(ade_base, ADE_ENABLE);
    } else {
        set_TOP_CTL_frm_end_start(ade_base, 1);
    }

    if (0 == ade_pri_data->frame_count) {
        ade_pri_data->frame_count++;
    }

    ade_pri_data->have_refresh = ADE_TRUE;
    if (PANEL_MIPI_CMD == balongfd->panel_info.type) {
        ade_pri_data->have_refresh = ADE_FALSE;
    }

#ifndef PC_UT_TEST_ON
#ifdef CONFIG_TRACING
    trace_dot(SF, "8", 0);
#endif
#endif

    up(&balongfd->sem);
    return 0;
}

int ade_fb_start_video_idle_config(struct ade_compose_data_type    *ade_pri_data)
{
    struct cmdfile_buffer  *cf_buff;
    struct ade_mux_info     ade_mux_list[TOP_MUX_MAX];
    struct ade_rect         ctran5_rect;
    u32     ade_demux_list[TOP_DEMUX_MAX];
    u32     format;
    u32     stride;
    u32     bpp;
    int     ret;
    u8*     ade_base;

    BUG_ON(ade_pri_data == NULL);

    if (ade_pri_data->wdma2_phy == 0) {
        return 0;
    }

    ade_base = ade_pri_data->ade_base;

    /* disp->ldi */
    memset(ade_mux_list, 0, (sizeof(struct ade_mux_info) * TOP_MUX_MAX));
    memset(ade_demux_list, 0, (sizeof(u32) * TOP_DEMUX_MAX));
    ade_mux_list[TOP_MUX_DISP_SRC].mux_src = TOP_DISP_CH_SRC_RDMA;
    ade_overlay_res_top_info_cfg(ade_pri_data, ade_demux_list, ade_mux_list);

    /* switch cmd buffer */
    ret = ade_overlay_res_get_cmdbuff_index(OVERLAY_COMP_TYPE_ONLINE, ade_pri_data->cmdq_list);
    if (ret != 0) {
        balongfb_loge("get cmd buff index error");
        return -EINVAL;
    }

    cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, OVERLAY_PIPE_TYPE_ONLINE);
    if (cf_buff == NULL) {
        balongfb_loge("ade_overlay_type2cmdfile get cmd buff error");
        return -EINVAL;
    }

    format = ade_pri_data->fb_imgType;
    if (((format > ADE_BGR_888) && (format < ADE_YUYV_I))
            || (format > ADE_YUV444)) {
        balongfb_loge("ade_pri_data->fb_imgType  = %d is not support \n", ade_pri_data->fb_imgType);
        return -EINVAL;
    }

    bpp = ade_overlay_format2bpp(format);
    stride = ade_pri_data->xres * bpp;

    /*  disp_Rdma */
    ade_cmdq_wr_cmd(RD_CH_DISP_CTRL_REG, format << 16);
    ade_cmdq_wr_cmd(RD_CH_DISP_ADDR_REG, ade_pri_data->wdma2_phy);
    ade_cmdq_wr_cmd(RD_CH_DISP_SIZE_REG, (ade_pri_data->yres << 16) | stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_STRIDE_REG, stride);
    ade_cmdq_wr_cmd(RD_CH_DISP_SPACE_REG, ade_pri_data->yres * stride);

    /* blank block */
    ade_cmdq_wr_cmd(RD_CH_DISP_BLANK_OFFSET_REG, 0);
    ade_cmdq_wr_cmd(RD_CH_DISP_BLANK_SIZE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH_DISP_BLANK_SPACE_REG, 0);
    ade_cmdq_wr_cmd(RD_CH_DISP_EN_REG, 1);
    ade_cmdq_wr_cmd2buff(cf_buff->vaddr, &(cf_buff->cmd_len));

    /* ctrans5 */
    ctran5_rect.x = 0;
    ctran5_rect.y = 0;
    ctran5_rect.w = ade_pri_data->xres;
    ctran5_rect.h = ade_pri_data->yres;
    ade_overlay_commit_set_ctran5(ade_pri_data, ctran5_rect, format, cf_buff);

    /* cmdq_rdma */
    ade_cmdq_wr_eof_cmd(cf_buff->vaddr, &(cf_buff->cmd_len));

    dsb(sy);

    ade_overlay_commit_set_online_cmdq(ade_base,cf_buff, 0xfffe);
    set_TOP_SOFT_RST_SEL0_cmdq1_rdma(ade_base, 1);
    set_TOP_ADE_EN(ade_base, 1);

    ade_pri_data->have_refresh = ADE_TRUE;
    return 0;
}

int ade_fb_start_video_idle(struct balong_fb_data_type *balongfd)
{
#if 0  //for b030
    struct ade_compose_data_type    *ade_pri_data;
    struct cmdfile_buffer  *online_cf_buff;
    u32     ade_base;
    int     ret;
    BUG_ON(balongfd == NULL);
    ade_pri_data = balongfd->ade_pri_data;
    ade_base = balongfd->ade_base;

    /* offline is simple, just change the reset unit of top */
    if (OVERLAY_COMP_TYPE_OFFLINE == ade_pri_data->overlay_ctl.comp_info.compose_mode) {
        /* TODO: */
        return 0;
    }

    online_cf_buff = ade_overlay_type2cmdfile(ade_pri_data->cmdq_list, OVERLAY_PIPE_TYPE_ONLINE);

    /* 1: change top to write back from WDMA2 */
    ade_overlay_refresh_pre_frm_info(ade_pri_data, online_cf_buff);

    /* 2: wait wdma2 cmpl, change the top,for disp_rdma */
    if (wait_event_interruptible_timeout(ade_pri_data->wait_wdma2_cmpl, ade_pri_data->wdma2_cmpl, HZ) < 0) {
        balongfb_loge("wait wdma2 cmpl inttrupt timeout \n");
        ade_pri_data->wdma2_cmpl = 0;
        return -ETIME;
    }
    ade_pri_data->wdma2_cmpl = 0;

    /* 3: config disp_rdma to read back from the wdma2 buffer */
    ret = ade_fb_start_video_idle_config(ade_pri_data);
    if (ret != 0) {
        balongfb_loge("config refresh disp ch fail \n");
        return ret;
    }
#endif
    return 0;
}


void ade_set_medianoc_for_dfs(struct balong_fb_data_type *balongfd)
{
    u32  tmp1 = 0;
    u32  tmp2 = 0;
    BUG_ON(balongfd == NULL);

    tmp1 = inp32(SOC_MEDIA_NOC_ADE0_QOSGENERATOR_MODE_ADDR(balongfd->media_noc_base));
    tmp2 = inp32(SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_ADDR(balongfd->media_noc_base));
    outp32(SOC_MEDIA_NOC_ADE0_QOSGENERATOR_MODE_ADDR(balongfd->media_noc_base), (tmp1 & 0xfffffffc) | 0x2);
    outp32(SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_ADDR(balongfd->media_noc_base), tmp2 | 0x1);

    tmp1 = inp32(SOC_MEDIA_NOC_ADE1_QOSGENERATOR_MODE_ADDR(balongfd->media_noc_base));
    tmp2 = inp32(SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_ADDR(balongfd->media_noc_base));
    outp32(SOC_MEDIA_NOC_ADE1_QOSGENERATOR_MODE_ADDR(balongfd->media_noc_base), (tmp1 & 0xfffffffc) | 0x2);
    outp32(SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_ADDR(balongfd->media_noc_base), tmp2 | 0x1);
}

void ade_core_power_on(struct balong_fb_data_type *balongfd)
{
    BUG_ON(balongfd == NULL);

    /* 1，外设区上电，软件不可控 */
    //phy_reg_writel(SOC_PERI_SCTRL_BASE_ADDR,SOC_PERI_SCTRL_SC_CLKCFG8BIT0_ADDR(0),0,31,0x01020183);

    /* 2，开媒体子系统时钟 */
    //phy_reg_writel(SOC_PERI_SCTRL_BASE_ADDR, SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_ADDR(0), 10, 10, 1);
    if(0 != clk_prepare_enable(balongfd->media_noc_clk)) {
        balongfb_loge("fail to clk_prepare_enable media_noc_clk\n");
    }

    /* 3.1, 使能父时钟 media_pll */

    /* 3.2 使能父时钟sys_pll,软件不能控制 */

    /* 4, 选择像素时钟的时钟源放在LDI的初始化中 */

    /* 5, 设置像素时钟分频，放在LDI的初始化中 */

    /* 6, 设置ADE core时钟分频值 */
    if (clk_set_rate(balongfd->ade_clk, balongfd->ade_core_rate) != 0) {
        balongfb_loge("clk_set_rate ade_core_rate error \n");
    }

    /* 7, 设置media_noc时钟分频值 */
    if (clk_set_rate(balongfd->media_noc_clk, balongfd->media_noc_rate) != 0) {
        balongfb_loge("clk_set_rate media_noc_rate error \n");
    }

    /* 8, 外设区解复位不需要ADE配置，软件不可配 */

    /* 9, 解复位ADE */
    outp32((balongfd->media_base + SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(0)), 0x20);

    /* 10,使能ADE时钟  */
    if(0 != clk_prepare_enable(balongfd->ade_clk)){
        balongfb_loge("fail to clk_prepare_enable ade_clk\n");
    }

    /* 11, S1低功耗状态投票 */
    if (0 != pwrctrl_request_power_state(PWRCTRL_SLEEP_ADE, PWRCTRL_SYS_STAT_S1, &balongfd->pw_state_qos_id)) {
        balongfb_loge("fail to request S1! \n");
    }

}

void ade_core_power_off(struct balong_fb_data_type *balongfd)
{
    BUG_ON(balongfd == NULL);

    /* 1，关闭ADE时钟软门控 */
    clk_disable_unprepare(balongfd->ade_clk);

    /* 2, 软复位ADE */
    outp32((balongfd->media_base + SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(0)), 0x20);
    /* 3, 外设区操作隔离，复位，不需要软件操作 */

    /* 4.1, 关父时钟sys_pll,软件不能控制 */

    /* 4.1, 关父时钟media_pll */

    /* 5, 关媒体子系统时钟门控 */
    //phy_reg_writel(SOC_PERI_SCTRL_BASE_ADDR, SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_ADDR(0), 10, 10, 1);
    clk_disable_unprepare(balongfd->media_noc_clk);

    /* 6, 关外设区电源，不需要软件配置 */

    /* 7, S1低功耗状态投票 */
    if (0 != pwrctrl_release_power_state(balongfd->pw_state_qos_id)) {
        balongfb_loge("fail to release S1! \n");
    }

}


int ade_fb_resume(struct fb_info *info)
{
    struct balong_fb_data_type      *balongfd = NULL;
    struct ade_compose_data_type    *ade_pri_data = NULL;
    u8*    ade_base;
    u32    cpu0_mask = 0;
    u32    cpu1_mask = 0;
    int i = 0;
    u32 struct_len = 0;

    BUG_ON(info == NULL);

    balongfd = (struct balong_fb_data_type *)info->par;

    BUG_ON(balongfd == NULL);

    balongfb_loge("enter succ ! \n");
    if (balongfd->ade_core_power_on) {
        return 0;
    }
    ade_base = balongfd->ade_base;

    for (i = 0; i < 3; i++) {
        scl_coef[i].ul32 = 0;
    }

    ade_pri_data = balongfd->ade_pri_data;
    ade_pri_data->frame_count = 0;
    ade_pri_data->top_ovly_ctrl = 0;

    balongfd->update_frame = 1;
    balongfd->refresh = 0;

    struct_len = sizeof(struct ovly_hnd_info) * ADE_OVERLAY_MAX_LAYERS;
    memset(balongfd->locked_hnd, 0, struct_len);
    memset(balongfd->locking_hnd, 0, struct_len);

    down(&balongfd->sem);
    ade_core_power_on(balongfd);
    ade_set_medianoc_for_dfs(balongfd);

    cpu0_mask = ADE_ISR_DMA_ERROR;
    cpu1_mask = ADE_ISR1_RES_SWITCH_CMPL;

#if ADE_DEBUG_LOG_ENABLE
    if (g_ade_isr_disable) {
        cpu0_mask = 0;
        cpu1_mask = 0;
    }
#endif
    set_TOP_INTR_MASK_CPU0(ade_base, cpu0_mask);
    set_TOP_INTR_MASK_CPU1(ade_base, cpu1_mask);
    set_TOP_CTL_frm_end_start(ade_base, 2);

    /* disable wdma2 and wdma3 frame discard */
    set_TOP_FRM_DISCARD_wdma2(ade_base, 0);
    set_TOP_FRM_DISCARD_wdma3(ade_base, 0);

    /* only for v8r1 */
    if (HI6210_CHIPSET == get_chipset_type()) {
        outp32(ade_base + ADE_AUTO_CLK_GT_EN0_REG, 0xffff8000);
        //set_TOP_AUTO_CLK_GT_EN0_cmdq1_rdma(ade_base, 0);
        //set_TOP_AUTO_CLK_GT_EN0_cmdq2_rdma(ade_base, 0);
    }
    set_TOP_SOFT_RST_SEL0(ade_base, 0);
    set_TOP_SOFT_RST_SEL1(ade_base, 0);
    set_TOP_RELOAD_DIS0(ade_base, 0);
    set_TOP_RELOAD_DIS1(ade_base, 0);

    /* enable clk gate */
    set_TOP_CTL_clk_gate_en(ade_base, 1);

    /* init ovly ctrl, if not,when the first frame is hybrid, will happen 32112222 */
    set_TOP_OVLY_CTRL_all(ade_base, 0);

    /* init scl coeff */
    ade_overlay_init_scl_coef(ade_base, ADE_SCL1_HCOEF_0_REG, ADE_SCL1_VCOEF_0_REG);
    ade_overlay_init_scl_coef(ade_base, ADE_SCL2_HCOEF_0_REG, ADE_SCL2_VCOEF_0_REG);
    ade_overlay_init_scl_coef(ade_base, ADE_SCL3_HCOEF_0_REG, ADE_SCL3_VCOEF_0_REG);

    if ((1 == balongfd->codec_pm_state) && (ade_pri_data->mem_share_drv_info.drv_status == MEM_SHARE_DRV_ALLOC_SUCC)) {
        set_TOP_CTL1_rot_buf_shr_out(ade_base, 1);
        if (ade_pri_data->mem_share_drv_info.drv_event == MEM_SHARE_EVENT_ALLOC_720P) {
            set_TOP_CTL_rot_buf_cfg(ade_base, 3);
        } else if (ade_pri_data->mem_share_drv_info.drv_event == MEM_SHARE_EVENT_ALLOC_1080P) {
            set_TOP_CTL_rot_buf_cfg(ade_base, 2);
        } else {
            balongfb_loge("drv_event is error event=%d \n", ade_pri_data->mem_share_drv_info.drv_event);
        }
        set_TOP_ADE_EN(ade_base, 1);

        balongfb_logi("ADE_CTRL1_REG main = 0x%x \n",inp32(ade_base + ADE_CTRL1_REG + 0x8000));
        balongfb_logi("ADE_CTRL_REG main = 0x%x \n",inp32(ade_base + ADE_CTRL_REG + 0x8000));
    }

    up(&balongfd->sem);

    balongfd->ade_core_power_on = true;

    /* enable ade irq */
    if (balongfd->ade_irq) {
        enable_irq(balongfd->ade_irq);
    }
    balongfb_loge("exit succ ! \n");
    return 0;

}

void ade_fb_suspend(struct fb_info *info)
{
    struct balong_fb_data_type      *balongfd = NULL;
    struct ade_compose_data_type    *ade_pri_data = NULL;
    u8*    ade_base;

    BUG_ON(info == NULL);

    balongfd = (struct balong_fb_data_type *)info->par;

    BUG_ON(balongfd == NULL);

    ade_base = balongfd->ade_base;
    ade_pri_data = balongfd->ade_pri_data;
    ade_pri_data->frame_count = 0;

    balongfb_loge("enter succ ! \n");
    if (!balongfd->ade_core_power_on) {
        return;
    }
    down(&balongfd->sem);
#if ADE_DEBUG_LOG_ENABLE
    if (g_ade_power_off_enable) {
        ade_core_power_off(balongfd);
    }
#else
    ade_core_power_off(balongfd);
#endif

    up(&balongfd->sem);

    balongfd->ade_core_power_on = false;

    /* disable ade irq */
    if (balongfd->ade_irq) {
        disable_irq(balongfd->ade_irq);
    }

    balongfb_loge("exit succ ! \n");
}

void ade_fb_backup_recover(struct balong_fb_data_type      *balongfd)
{
#define BACK_UP_ADE_TOP_REG_CNT     (14)
    u32 delay_cnt = 0;
    u8* ade_base;
    u32 i = 0;
    u32 top_value[BACK_UP_ADE_TOP_REG_CNT];
    u32 top_reg[] = {ADE_CTRL_REG,
                    ADE_SCL3_MUX_CFG_REG,
                    ADE_SCL1_MUX_CFG_REG,
                    ADE_ROT_SRC_CFG_REG,
                    ADE_SCL2_SRC_CFG_REG,
                    ADE_DISP_SRC_CFG_REG,
                    ADE_WDMA2_SRC_CFG_REG,
                    ADE_SEC_OVLY_SRC_CFG_REG,
                    ADE_WDMA3_SRC_CFG_REG,
                    ADE_OVLY1_TRANS_CFG_REG,
                    ADE_CTRAN5_TRANS_CFG_REG,
                    ADE_DMA_AXI_MUX_REG,
                    ADE_CTRL1_REG,
                    ADE_OVLY_CTL_REG};

    if (balongfd == NULL) {
        return;
    }

    ade_base = balongfd->ade_base;
    set_TOP_CTL_frm_end_start(ade_base, 2);

    /* backup */
    for (i = 0; i < BACK_UP_ADE_TOP_REG_CNT; i++) {
      top_value[i] = inp32(ade_base + top_reg[i]);
    }

    set_TOP_SOFT_RST_SEL0(ade_base, 0);
    set_TOP_SOFT_RST_SEL1(ade_base, 0);
    set_TOP_RELOAD_DIS0(ade_base, 0xffffffff);
    set_TOP_RELOAD_DIS1(ade_base, 0xffffffff);

    balongfd->ade_pri_data->res_switch_cmpl = 0;
    set_TOP_ADE_EN(ade_base, ADE_TRUE);
    if (wait_event_interruptible_timeout(balongfd->ade_pri_data->wait_res_cmpl, balongfd->ade_pri_data->res_switch_cmpl, (HZ/4)) <= 0) {
        balongfb_loge("wait res_switch_cmpl timeout!,fb_data->panle_power_on=%d,ade_core_power_on=%d\n",balongfd->panel_power_on, balongfd->ade_core_power_on);
        set_LDI_DSI_CMD_MOD_CTRL(0x0);
        mdelay(1);
        set_LDI_DSI_CMD_MOD_CTRL(0x2);
    }
    balongfd->ade_pri_data->res_switch_cmpl = 0;
    set_TOP_SOFT_RST1(ade_base, 0xffffffff);
    set_TOP_SOFT_RST0(ade_base, 0xffffffff);

    if (balongfd->virtual_disp_ref > 0) {
        return;
    }

    while(delay_cnt < 10) {
        if ((inp32(ade_base + ADE_STAT0_REG) == 0) && (inp32(ade_base + ADE_STAT1_REG) == 0)) {
            break;
        } else {
            udelay(10);
            delay_cnt++;
        }
    }

    outp32((balongfd->media_base + SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(0)), 0x20); /* 软复位ADE */
    outp32((balongfd->media_base + SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(0)), 0x20); /* 解复位ADE */

    /* recover */
    for (i = 0; i < BACK_UP_ADE_TOP_REG_CNT; i++) {
       outp32(ade_base + top_reg[i], top_value[i]);
    }

    /* disable wdma2 and wdma3 frame discard */
    set_TOP_FRM_DISCARD_wdma2(ade_base, 0);
    set_TOP_FRM_DISCARD_wdma3(ade_base, 0);
    set_TOP_CTL_clk_gate_en(ade_base, 1);
    set_TOP_OVLY_CTRL_all(ade_base, 0);
    set_TOP_ADE_EN(ade_base, ADE_TRUE);
}

#if 0
int ade_fb_disable(struct fb_info *info)
{

}
int ade_fb_enable(struct fb_info *info)
{

}


/******************************SBL************************************/
int ade_sbl_ctrl_set(struct balong_fb_data_type *balongfd)
{

}

int ade_sbl_ctrl_resume(struct balong_fb_data_type *balongfd)
{

}
int ade_sbl_bkl_set(struct balong_fb_data_type *balongfd, u32 value)
{

}
#endif

