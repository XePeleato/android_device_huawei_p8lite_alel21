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

#include "ade_reg_offset.h"
#include "ade_union_define.h"
#include "ade_cmdqueue.h"
#include "balong_fb_def.h"

#define CALC_REG_MASK(bit_start, bit_end) \
    (((0x1 << (bit_end - bit_start + 1)) - 1) << bit_start)

/*************************TOP*******************/
inline void set_TOP_CTL(u8* ade_base, u32 val)
{
    outp32(ade_base + ADE_CTRL_REG, val);
}

inline void set_TOP_ADE_OVLY_CTRL(u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_OVLY_CTL_REG;
    outp32(reg_addr, val);
}

void set_TOP_CTL_clk_gate_en(u8* ade_base, u32 val)
{
    volatile U_ADE_CTRL1   ade_ctrl1;
    u8* reg_addr = ade_base + ADE_CTRL1_REG;

    ade_ctrl1.u32 = inp32(reg_addr);
    ade_ctrl1.bits.auto_clk_gate_en = val;

    outp32(reg_addr, ade_ctrl1.u32);
}

void set_TOP_CTL_frm_end_start (u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_CTRL_REG;
    volatile U_ADE_CTRL  ade_ctrl;

    ade_ctrl.u32 = inp32(reg_addr);
    ade_ctrl.bits.frm_end_start = val;
    outp32(reg_addr, ade_ctrl.u32);
}

void set_TOP_CTL_ch5_nv (u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_CTRL_REG;
    volatile U_ADE_CTRL  ade_ctrl;

    ade_ctrl.u32 = inp32(reg_addr);
    ade_ctrl.bits.rd_ch5_nv = val;
    outp32(reg_addr, ade_ctrl.u32);
}

void set_TOP_CTL_ch6_nv (u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_CTRL_REG;
    volatile U_ADE_CTRL  ade_ctrl;

    ade_ctrl.u32 = inp32(reg_addr);
    ade_ctrl.bits.rd_ch6_nv = val;
    outp32(reg_addr, ade_ctrl.u32);
}

#if ADE_MEM_SHARE_EN
void set_TOP_CTL1_rot_buf_shr_out (u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_CTRL1_REG;
    volatile U_ADE_CTRL1 ade_ctrl1;

    ade_ctrl1.u32 = inp32(reg_addr);
    ade_ctrl1.bits.rot_buf_shr_out = val;
    outp32(reg_addr, ade_ctrl1.u32);
}

u32 get_TOP_CTL1_rot_buf_shr_out(u8* ade_base)
{
    u8* reg_addr = ade_base + ADE_CTRL1_REG + 0x8000;
    volatile U_ADE_CTRL1 ade_ctrl1;

    ade_ctrl1.u32 = inp32(reg_addr);
    return ade_ctrl1.bits.rot_buf_shr_out;
}
u32 get_TOP_CTL_rot_buf_cfg (u8* ade_base)
{
    u8* reg_addr = ade_base + ADE_CTRL_REG + 0x8000;
    volatile U_ADE_CTRL  ade_ctrl;

    ade_ctrl.u32 = inp32(reg_addr);
    return ade_ctrl.bits.rot_buf_cfg;
}

#endif

void set_TOP_CTL_dfs_buf_cfg(u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_CTRL_REG;
    volatile U_ADE_CTRL  ade_ctrl;

    ade_ctrl.u32 = inp32(reg_addr);
    ade_ctrl.bits.dfs_buf_cfg = val;
    outp32(reg_addr, ade_ctrl.u32);
}

void set_TOP_CTL_rot_buf_cfg (u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_CTRL_REG;
    volatile U_ADE_CTRL  ade_ctrl;

    ade_ctrl.u32 = inp32(reg_addr);
    ade_ctrl.bits.rot_buf_cfg = val;
    outp32(reg_addr, ade_ctrl.u32);
}

inline void set_TOP_ADE_EN(u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_EN_REG;
    outp32(reg_addr, val);
}
inline void set_TOP_SCL1_MUX_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_SCL1_MUX_CFG_REG;
    outp32(reg_addr, val);
}
void set_TOP_SCL3_MUX_CFG_scl3_mux(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_SCL3_MUX_CFG_REG;
    volatile U_ADE_SCL3_MUX_CFG ade_scl3;

    ade_scl3.u32 = inp32(reg_addr);
    ade_scl3.bits.scl3_mux_cfg = val;
    outp32(reg_addr, ade_scl3.u32);
}

void set_TOP_SCL3_MUX_CFG_wdma3_trans(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_SCL3_MUX_CFG_REG;
    volatile U_ADE_SCL3_MUX_CFG ade_scl3;

    ade_scl3.u32 = inp32(reg_addr);
    ade_scl3.bits.clip6_wdma3_trans_en = val;
    outp32(reg_addr, ade_scl3.u32);
}

inline void set_TOP_OVLY1_TRANS_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_OVLY1_TRANS_CFG_REG;
    outp32(reg_addr, val);
}
inline void set_TOP_ROT_SRC_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_ROT_SRC_CFG_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_SCL2_SRC_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_SCL2_SRC_CFG_REG;
    outp32(reg_addr, val);
}
inline void set_TOP_WDMA2_SRC_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_WDMA2_SRC_CFG_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_SEC_OVLY_SRC_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_SEC_OVLY_SRC_CFG_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_WDMA3_SRC_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_WDMA3_SRC_CFG_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_DISP_SRC_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_DISP_SRC_CFG_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_CTRAN5_TRANS_CFG(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_CTRAN5_TRANS_CFG_REG;
    outp32(reg_addr, val);
}
inline void set_TOP_DMA_AXI_MUX(u8* ade_base, u32 ch_num, u32 ch_type)
{
    //return;    //Temporary modify
#if 1
    u8*  reg_addr = ade_base + ADE_DMA_AXI_MUX_REG;
    u32  axi_num;
    volatile U_ADE_DMA_AXI_MUX dma_axi_mux;

    dma_axi_mux.u32 = inp32(reg_addr);

    dma_axi_mux.bits.rd_dma_cmdq2_axi_cfg = 0;
    dma_axi_mux.bits.rd_dma_cmdq1_axi_cfg = 0;
    if (OVERLAY_PIPE_TYPE_ONLINE == ch_type) {
        axi_num = 0;
        dma_axi_mux.bits.wr_dma_ch2_axi_cfg   = axi_num;
    } else {
        axi_num = 1;
        dma_axi_mux.bits.wr_dma_ch1_axi_cfg   = axi_num;
        dma_axi_mux.bits.wr_dma_ch3_axi_cfg   = axi_num;
    }

    switch (ch_num) {
        case OVERLAY_PIPE_ADE_CH1:
            dma_axi_mux.bits.rd_dma_ch1_axi_cfg = axi_num;
            break;
        case OVERLAY_PIPE_ADE_CH2:
            dma_axi_mux.bits.rd_dma_ch2_axi_cfg = axi_num;
            break;
        case OVERLAY_PIPE_ADE_CH3:
            dma_axi_mux.bits.rd_dma_ch3_axi_cfg = axi_num;
            break;
        case OVERLAY_PIPE_ADE_CH4:
            dma_axi_mux.bits.rd_dma_ch4_axi_cfg = axi_num;
            break;
        case OVERLAY_PIPE_ADE_CH5:
            dma_axi_mux.bits.rd_dma_ch5_axi_cfg = axi_num;
            break;
        case OVERLAY_PIPE_ADE_CH6:
            dma_axi_mux.bits.rd_dma_ch6_axi_cfg = axi_num;
            break;
        case OVERLAY_PIPE_ADE_DISP:
            dma_axi_mux.bits.rd_dma_disp_axi_cfg = 0;   /* disp channel is always online */
            break;
        default:
            break;
    }

    outp32(reg_addr, dma_axi_mux.u32);
#endif
}

inline void set_TOP_FRM_DISCARD_wdma2(u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_FRM_DISGARD_CTRL_REG;
    U_ADE_FRM_DISGARD_CTRL frm_dis_ctrl;

    frm_dis_ctrl.u32 = inp32(reg_addr);
    frm_dis_ctrl.bits.wdma2_disgard_en = val;

    outp32(reg_addr, frm_dis_ctrl.u32);
}

inline void set_TOP_FRM_DISCARD_wdma3(u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_FRM_DISGARD_CTRL_REG;
    U_ADE_FRM_DISGARD_CTRL frm_dis_ctrl;

    frm_dis_ctrl.u32 = inp32(reg_addr);
    frm_dis_ctrl.bits.wdma3_disgard_en = val;

    outp32(reg_addr, frm_dis_ctrl.u32);
}

inline void set_TOP_FRM_DISCARD_fifo(u8* ade_base, u32 val)
{
    u8* reg_addr = ade_base + ADE_FRM_DISGARD_CTRL_REG;
    U_ADE_FRM_DISGARD_CTRL frm_dis_ctrl;

    frm_dis_ctrl.u32 = inp32(reg_addr);
    frm_dis_ctrl.bits.dfs_buf_unflow_lev0 = val;

    outp32(reg_addr, frm_dis_ctrl.u32);
}

inline void set_TOP_INTR_MASK_CPU0(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_MASK_CPU_0_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_INTR_MASK_CPU1(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_MASK_CPU_1_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_INTR_MASK_CPU0_bit(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_MASK_CPU_0_REG;
    u32  cpu0_intr;

    cpu0_intr = inp32(reg_addr);
    cpu0_intr |= val;
    outp32(reg_addr, cpu0_intr);
}

inline void clear_TOP_INTR_MASK_CPU0_bit(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_MASK_CPU_0_REG;
    u32  cpu0_intr;

    cpu0_intr = inp32(reg_addr);
    cpu0_intr &= (~val);
    outp32(reg_addr, cpu0_intr);
}

inline void set_TOP_INTR_MASK_CPU1_bit(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_MASK_CPU_1_REG;
    u32  cpu0_intr;

    cpu0_intr = inp32(reg_addr);
    cpu0_intr |= val;
    outp32(reg_addr, cpu0_intr);
}

inline void clear_TOP_INTR_MASK_CPU1_bit(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_MASK_CPU_1_REG;
    u32  cpu0_intr;

    cpu0_intr = inp32(reg_addr);
    cpu0_intr &= (~val);
    outp32(reg_addr, cpu0_intr);
}


inline void clear_TOP_INTR_ORG_CPU0(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_CLEAR_CPU_0_REG;
    outp32(reg_addr, val);
}

inline void clear_TOP_INTR_ORG_CPU1(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_CLEAR_CPU_1_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_INTR_CLEAR_CPU0(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_CLEAR_CPU_0_REG;
    outp32(reg_addr, val);
}

inline void set_TOP_INTR_CLEAR_CPU1(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + INTR_CLEAR_CPU_1_REG;
    outp32(reg_addr, val);
}

void set_TOP_OVLY_CTRL_all(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + ADE_OVLY_CTL_REG;
    outp32(reg_addr, val);
}
void set_TOP_OVLY_CTRL(u8* ade_base, u16 start_bit, u32 val)
{
    u8*  reg_addr = ade_base + ADE_OVLY_CTL_REG;
    u32  reg_val;
    u32  mask;

    mask = CALC_REG_MASK(start_bit, (start_bit + 1));
    reg_val = inp32(reg_addr) & ~mask;
    reg_val |= (mask & (val << start_bit));
    outp32(reg_addr, reg_val);
}

void set_TOP_AUTO_CLK_GT_EN0_cmdq1_rdma(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_AUTO_CLK_GT_EN0_REG;
    volatile U_ADE_AUTO_CLK_GT_EN0 auto_clk_gt_en0;

    auto_clk_gt_en0.u32 = inp32(addr);
    auto_clk_gt_en0.bits.cmdq1_rdma_auto_clk_gt_en = val;
    outp32(addr, auto_clk_gt_en0.u32);
}

void set_TOP_AUTO_CLK_GT_EN0_cmdq2_rdma(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_AUTO_CLK_GT_EN0_REG;
    volatile U_ADE_AUTO_CLK_GT_EN0 auto_clk_gt_en0;

    auto_clk_gt_en0.u32 = inp32(addr);
    auto_clk_gt_en0.bits.cmdq2_rdma_auto_clk_gt_en = val;
    outp32(addr, auto_clk_gt_en0.u32);
}

inline void set_TOP_SOFT_RST0(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_SOFT_RST0_REG;
    outp32(addr, val);
}
inline void set_TOP_SOFT_RST1(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_SOFT_RST1_REG;

    outp32(addr, val);
}

inline void set_TOP_SOFT_RST_SEL0(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    outp32(addr, val);
}
inline void set_TOP_SOFT_RST_SEL1(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_SOFT_RST_SEL1_REG;

    outp32(addr, val);
}

inline void set_TOP_RELOAD_DIS0(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_RELOAD_DIS0_REG;
    outp32(addr, val);
}
inline void set_TOP_RELOAD_DIS1(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_RELOAD_DIS1_REG;

    outp32(addr, val);
}

inline void set_TOP_RELOAD_DIS1_gamma(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_RELOAD_DIS1_REG;
    volatile U_ADE_RELOAD_DIS1 reload_dis1;

    reload_dis1.u32 = inp32(addr);
    reload_dis1.bits.gamma_reload_dis = val;
    outp32(addr, reload_dis1.u32);
}
inline void set_TOP_RELOAD_DIS1_dither(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_RELOAD_DIS1_REG;

    volatile U_ADE_RELOAD_DIS1 reload_dis1;

    reload_dis1.u32 = inp32(addr);
    reload_dis1.bits.dither_reload_dis = val;
    outp32(addr, reload_dis1.u32);
}

inline void set_TOP_RELOAD_DIS1_cmdq1(u8* ade_base, u32 val)
{
    u8* addr = ade_base + ADE_RELOAD_DIS1_REG;

    volatile U_ADE_RELOAD_DIS1 reload_dis1;

    reload_dis1.u32 = inp32(addr);
    reload_dis1.bits.cmdq1_reload_dis = val;
    outp32(addr, reload_dis1.u32);
}

inline void set_TOP_SOFT_RST_SEL0_ch1_rdma(u8* ade_base, u32 val)
{
    u8*  addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch1_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ch2_rdma(u8* ade_base, u32 val)
{
    u8*  addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch2_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}

inline void set_TOP_SOFT_RST_SEL0_ch3_rdma(u8* ade_base, u32 val)
{
    u8*   addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch3_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ch4_rdma(u8* ade_base, u32 val)
{
    u8*   addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch4_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ch5_rdma(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch5_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ch6_rdma(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch6_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_disp_rdma(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.disp_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_cmdq1_rdma(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.cmdq1_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}

inline void set_TOP_RELOAD_DIS0_cmdq1_rdma(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_RELOAD_DIS0_REG;
    volatile U_ADE_RELOAD_DIS0 ade_reload_dis;

    ade_reload_dis.u32 = inp32(addr);
    ade_reload_dis.bits.cmdq1_rdma_reload_dis = val;

    outp32(addr, ade_reload_dis.u32);
}

inline void set_TOP_SOFT_RST_SEL0_cmdq2_rdma(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.cmdq2_rdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_wdma1(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch1_wdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_wdma2(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch2_wdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_wdma3(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ch3_wdma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}

inline void set_TOP_SOFT_RST_SEL0_clip1(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.clip1_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_clip2(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.clip2_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_clip3(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.clip3_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_clip4(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.clip4_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_clip5(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.clip5_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_clip6(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.clip6_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_scl1(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.scl1_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_scl2(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.scl2_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_scl3(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.scl3_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ctran1(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ctran1_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ctran2(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ctran2_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ctran3(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ctran3_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ctran4(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ctran4_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ctran5(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ctran5_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_ctran6(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ctran6_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL0_rot(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL0_REG;
    volatile U_ADE_SOFT_RST_SEL0 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.rot_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}

inline void set_TOP_SOFT_RST_SEL1_cmdq1(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL1_REG;
    volatile U_ADE_SOFT_RST_SEL1 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.cmdq1_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL1_cmdq2(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL1_REG;
    volatile U_ADE_SOFT_RST_SEL1 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.cmdq2_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}

inline void set_TOP_SOFT_RST_SEL1_gamma(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL1_REG;
    volatile U_ADE_SOFT_RST_SEL1 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.gamma_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL1_dither(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL1_REG;
    volatile U_ADE_SOFT_RST_SEL1 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.dither_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL1_ovly1(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL1_REG;
    volatile U_ADE_SOFT_RST_SEL1 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ovly1_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL1_ovly2(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL1_REG;
    volatile U_ADE_SOFT_RST_SEL1 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ovly2_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
inline void set_TOP_SOFT_RST_SEL1_ovly3(u8* ade_base, u32 val)
{
    u8*          addr = ade_base + ADE_SOFT_RST_SEL1_REG;
    volatile U_ADE_SOFT_RST_SEL1 ade_soft_rst;

    ade_soft_rst.u32 = inp32(addr);
    ade_soft_rst.bits.ovly3_srst_sel = val;

    outp32(addr, ade_soft_rst.u32);
}
/*********** CMDQ RDMA************************/
inline void set_CMDQ_RDMA1_PE(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ1_PE_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA1_CTRL(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ1_CTRL_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA1_ADDR(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ1_ADDR_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA1_LEN(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ1_LEN_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA1_EN(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ1_EN_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA2_PE(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ2_PE_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA2_CTRL(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ2_CTRL_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA2_ADDR(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ2_ADDR_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA2_LEN(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ2_LEN_REG;
    outp32(reg_addr, val);
}
inline void set_CMDQ_RDMA2_EN(u8* ade_base, u32 val)
{
    u8*  reg_addr = ade_base + RD_CH_CMDQ2_EN_REG;
    outp32(reg_addr, val);
}

