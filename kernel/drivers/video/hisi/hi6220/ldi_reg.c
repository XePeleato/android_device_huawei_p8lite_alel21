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

#include "ldi_reg.h"

/******************************************************************************
** FUNCTIONS IMPLEMENTATIONS
*/

void set_LDI_HRZ_CTRL0(u8* ade_base, u32 hfp, u32 hbp)
{
	u8* addr = ade_base + LDI_HRZ_CTRL0_REG;
	volatile U_LDI_HRZ_CTRL0 ldi_hrz_ctrl0;

	ldi_hrz_ctrl0.u32 = inp32(addr);
	ldi_hrz_ctrl0.bits.hfp = hfp;
	ldi_hrz_ctrl0.bits.hbp = hbp;
	outp32(addr, ldi_hrz_ctrl0.u32);
}




void set_LDI_HRZ_CTRL1_hsw(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_HRZ_CTRL1_REG;
	volatile U_LDI_HRZ_CTRL1 ldi_hrz_ctrl1;

	ldi_hrz_ctrl1.u32 = inp32(addr);
	ldi_hrz_ctrl1.bits.hsw = (nVal > 0) ? nVal - 1 : 0;
	outp32(addr, ldi_hrz_ctrl1.u32);
}

void set_LDI_VRT_CTRL0(u8* ade_base, u32 vfp, u32 vbp)
{
	u8* addr = ade_base + LDI_VRT_CTRL0_REG;
	volatile U_LDI_VRT_CTRL0 ldi_vrt_ctrl0;

	ldi_vrt_ctrl0.u32 = inp32(addr);
	ldi_vrt_ctrl0.bits.vfp = vfp;


	ldi_vrt_ctrl0.bits.vbp = vbp;
	outp32(addr, ldi_vrt_ctrl0.u32);
}

void set_LDI_VRT_CTRL1_vsw(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_VRT_CTRL1_REG;
	volatile U_LDI_VRT_CTRL1 ldi_vrt_ctrl1;

	ldi_vrt_ctrl1.u32 = inp32(addr);
	ldi_vrt_ctrl1.bits.vsw = (nVal > 0) ? nVal - 1 : 0;
	outp32(addr, ldi_vrt_ctrl1.u32);
}

void set_LDI_PLR_CTRL_vsync(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_PLR_CTRL_REG;
	volatile U_LDI_PLR_CTRL ldi_plr_ctrl;

	ldi_plr_ctrl.u32 = inp32(addr);
	ldi_plr_ctrl.bits.vsync_plr = nVal;
	outp32(addr, ldi_plr_ctrl.u32);
}

void set_LDI_PLR_CTRL_hsync(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_PLR_CTRL_REG;
	volatile U_LDI_PLR_CTRL ldi_plr_ctrl;

	ldi_plr_ctrl.u32 = inp32(addr);
	ldi_plr_ctrl.bits.hsync_plr = nVal;
	outp32(addr, ldi_plr_ctrl.u32);
}

void set_LDI_PLR_CTRL_pixel_clk(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_PLR_CTRL_REG;
	volatile U_LDI_PLR_CTRL ldi_plr_ctrl;

	ldi_plr_ctrl.u32 = inp32(addr);
	ldi_plr_ctrl.bits.pixel_clk_plr = nVal;
	outp32(addr, ldi_plr_ctrl.u32);
}

void set_LDI_PLR_CTRL_data_en(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_PLR_CTRL_REG;
	volatile U_LDI_PLR_CTRL ldi_plr_ctrl;

	ldi_plr_ctrl.u32 = inp32(addr);
	ldi_plr_ctrl.bits.data_en_plr = nVal;
	outp32(addr, ldi_plr_ctrl.u32);
}

void set_LDI_DSP_SIZE_size(u8* ade_base, u32 hVal, u32 vVal)
{
	u8* addr = ade_base + LDI_DSP_SIZE_REG;
	volatile U_LDI_DSP_SIZE ldi_dsp_size;

	ldi_dsp_size.bits.hsize = (hVal > 0) ? hVal-1 : 0;
	ldi_dsp_size.bits.vsize = (vVal > 0) ? vVal-1 : 0;
	outp32(addr, ldi_dsp_size.u32);
}

void set_LDI_3D_CTRL_active_space(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_3D_CTRL_REG;
	volatile U_LDI_3D_CTRL ldi_3d_ctrl;

	ldi_3d_ctrl.u32 = inp32(addr);
	ldi_3d_ctrl.bits.active_space = nVal;
	outp32(addr, ldi_3d_ctrl.u32);
}

void set_LDI_CTRL_ldi_en(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_CTRL_REG;
	volatile U_LDI_CTRL ldi_ctrl;

	ldi_ctrl.u32 = inp32(addr);
	ldi_ctrl.bits.ldi_en = nVal;
	outp32(addr, ldi_ctrl.u32);
}

void set_LDI_CTRL_disp_mode(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_CTRL_REG;
	volatile U_LDI_CTRL ldi_ctrl;

	ldi_ctrl.u32 = inp32(addr);
	ldi_ctrl.bits.disp_mode_buf = nVal;
	outp32(addr, ldi_ctrl.u32);
}

void set_LDI_CTRL_data_gate_en(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_CTRL_REG;
	volatile U_LDI_CTRL ldi_ctrl;

	ldi_ctrl.u32 = inp32(addr);
	ldi_ctrl.bits.date_gate_en = nVal;
	outp32(addr, ldi_ctrl.u32);
}

void set_LDI_CTRL_bpp(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_CTRL_REG;
	volatile U_LDI_CTRL ldi_ctrl;

	ldi_ctrl.u32 = inp32(addr);
	ldi_ctrl.bits.bpp = nVal;
	outp32(addr, ldi_ctrl.u32);
}

void set_LDI_CTRL_wait_vsync_en(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_CTRL_REG;
	volatile U_LDI_CTRL ldi_ctrl;

	ldi_ctrl.u32 = inp32(addr);
	ldi_ctrl.bits.wait_vsync_en = nVal;
	outp32(addr, ldi_ctrl.u32);
}

void set_LDI_CTRL_corlorbar_width(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_CTRL_REG;
	volatile U_LDI_CTRL ldi_ctrl;

	ldi_ctrl.u32 = inp32(addr);
	ldi_ctrl.bits.corlorbar_width = (nVal > 0) ? nVal - 1 : 0;
	outp32(addr, ldi_ctrl.u32);
}

void set_LDI_CTRL_bgr(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_CTRL_REG;
	volatile U_LDI_CTRL ldi_ctrl;

	ldi_ctrl.u32 = inp32(addr);
	ldi_ctrl.bits.bgr = nVal;
	outp32(addr, ldi_ctrl.u32);
}

void set_LDI_CTRL_self_clr(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_CTRL_REG;
	volatile U_LDI_CTRL ldi_ctrl;

	ldi_ctrl.u32 = inp32(addr);
	ldi_ctrl.bits.ldi_en_self_clr = nVal;
	outp32(addr, ldi_ctrl.u32);
}


void set_LDI_INT_CLR(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_INT_CLR_REG;

	outp32(addr, nVal);
}

void set_LDI_INT_MASK(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_INT_EN_REG;

	outp32(addr, nVal);
}

void clear_LDI_INT_MASK_bit(u8* ade_base, u32 nVal)
{
    u8* addr = ade_base + LDI_INT_EN_REG;
    u32 reg_val;

    reg_val = inp32(addr);
    reg_val &= (~nVal);
	outp32(addr, reg_val);
}

void set_LDI_INT_MASK_bit(u8* ade_base, u32 nVal)
{
    u8* addr = ade_base + LDI_INT_EN_REG;
    u32 reg_val;

    reg_val = inp32(addr);
    reg_val |= nVal;
	outp32(addr, reg_val);
}

void set_LDI_WORK_MODE_work_mode(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_WORK_MODE_REG;
	volatile U_LDI_WORK_MODE ldi_work_mode;

	ldi_work_mode.u32 = inp32(addr);
	ldi_work_mode.bits.work_mode = nVal;
	outp32(addr, ldi_work_mode.u32);
}

void set_LDI_WORK_MODE_wback_en(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_WORK_MODE_REG;
	volatile U_LDI_WORK_MODE ldi_work_mode;

	ldi_work_mode.u32 = inp32(addr);
	ldi_work_mode.bits.wback_en = nVal;
	outp32(addr, ldi_work_mode.u32);
}

void set_LDI_WORK_MODE_colorbar_en(u8* ade_base, u32 nVal)
{
	u8* addr = ade_base + LDI_WORK_MODE_REG;
	volatile U_LDI_WORK_MODE ldi_work_mode;

	ldi_work_mode.u32 = inp32(addr);
	ldi_work_mode.bits.colorbar_en = nVal;
	outp32(addr, ldi_work_mode.u32);
}

/* EDC register */
void set_LDI_DSI_TE_CTRL_dsi_te_mask_en(u8* ade_base, u32 nVal )
{
    u8* addr    = ade_base + DSI_TE_CTRL_REG;
    volatile U_DSI_TE_CTRL       ldi_dsi_te_ctrl;

    ldi_dsi_te_ctrl.u32                = inp32(addr);
    ldi_dsi_te_ctrl.bits.dsi_te_mask_en = nVal;
    outp32(addr, ldi_dsi_te_ctrl.u32);
}



