/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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

#ifndef __LDI_REG_H__
#define __LDI_REG_H__

#include "ade_reg_offset.h"
#include "ade_union_define.h"
#include "balong_fb_def.h"
#include "balong_fb.h"

#define get_ade_virtual_addr()              ( balongfd_reg_base_ade )
#define set_LDI_DSI_CMD_MOD_CTRL( nVal )    { outp32(get_ade_virtual_addr() + DSI_CMD_MOD_CTRL_REG, nVal); }
#define set_LDI_DSI_TE_CTRL( nVal )         { outp32(get_ade_virtual_addr() + DSI_TE_CTRL_REG, nVal); }
#define set_LDI_DSI_TE_HS_NUM( nVal )       { outp32(get_ade_virtual_addr() + DSI_TE_HS_NUM_REG, nVal); }
#define set_LDI_DSI_TE_HS_WD( nVal )        { outp32(get_ade_virtual_addr() + DSI_TE_HS_WD_REG, nVal); }
#define set_LDI_DSI_TE_VS_WD( nVal )        { outp32(get_ade_virtual_addr() + DSI_TE_VS_WD_REG, nVal); }



/******************************************************************************
** FUNCTIONS PROTOTYPES
*/

void set_LDI_HRZ_CTRL0(u8* ade_base, u32 hfp, u32 hbp);
void set_LDI_HRZ_CTRL1_hsw(u8* ade_base, u32 nVal);
void set_LDI_VRT_CTRL0(u8* ade_base, u32 vfp, u32 vbp);
void set_LDI_VRT_CTRL1_vsw(u8* ade_base, u32 nVal);
void set_LDI_PLR_CTRL_vsync(u8* ade_base, u32 nVal);
void set_LDI_PLR_CTRL_hsync(u8* ade_base, u32 nVal);
void set_LDI_PLR_CTRL_pixel_clk(u8* ade_base, u32 nVal);
void set_LDI_PLR_CTRL_data_en(u8* ade_base, u32 nVal);
void set_LDI_DSP_SIZE_size(u8* ade_base, u32 hVal, u32 vVal);
void set_LDI_3D_CTRL_active_space(u8* ade_base, u32 nVal);
void set_LDI_CTRL_ldi_en(u8* ade_base, u32 nVal);
void set_LDI_CTRL_disp_mode(u8* ade_base, u32 nVal);
void set_LDI_CTRL_data_gate_en(u8* ade_base, u32 nVal);
void set_LDI_CTRL_bpp(u8* ade_base, u32 nVal);
void set_LDI_CTRL_wait_vsync_en(u8* ade_base, u32 nVal);
void set_LDI_CTRL_corlorbar_width(u8* ade_base, u32 nVal);
void set_LDI_CTRL_bgr(u8* ade_base, u32 nVal);
void set_LDI_CTRL_self_clr(u8* ade_base, u32 nVal);
void set_LDI_INT_CLR(u8* ade_base, u32 nVal);
void set_LDI_WORK_MODE_work_mode(u8* ade_base, u32 nVal);
void set_LDI_WORK_MODE_wback_en(u8* ade_base, u32 nVal);
void set_LDI_WORK_MODE_colorbar_en(u8* ade_base, u32 nVal);
void set_LDI_INT_MASK(u8* ade_base, u32 nVal);
void clear_LDI_INT_MASK_bit(u8* ade_base, u32 nVal);
void set_LDI_INT_MASK_bit(u8* ade_base, u32 nVal);

/* EDC register */
void set_LDI_DSI_TE_CTRL_dsi_te_mask_en(u8* ade_base, u32 nVal );

#endif  /* __LDI_REG_H__ */
