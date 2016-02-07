/* Copyright (c) 2008-2013, Hisilicon Tech. Co., Ltd. All rights reserved.
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
 * file: k3_drv_mhl_tx.h
 */

#ifndef __K3_DRV_MHL_TX_H__
#define __K3_DRV_MHL_TX_H__

#include "k3_hdmi_hw_tpi.h"
//#include <mach/hardware.h>


// DEVCAP we will initialize to
#define MHL_LOGICAL_DEVICE_MAP (MHL_DEV_LD_AUDIO | MHL_DEV_LD_VIDEO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_GUI)

#define INCREMENT_CBUS_REFERENCE_COUNT(func) {g_mhl_tx_config.cbus_reference_count++; }
#define DECREMENT_CBUS_REFERENCE_COUNT(func) {g_mhl_tx_config.cbus_reference_count--; }

#define SET_MISC_FLAG(func, x) { g_mhl_tx_config.misc_flags |= (x);}
#define CLR_MISC_FLAG(func, x) { g_mhl_tx_config.misc_flags &= ~(x);}
#define TestMiscFlag(x) (g_mhl_tx_config.misc_flags & (x))

u32 k3_mhl_reg_read(u32 virtual_addr);
void k3_mhl_reg_write(u32 virtual_addr, u32 value);
void k3_mhl_reg_modify(u32 virtual_addr, u32 mask, u32 value);
void k3_mhl_reg_write_block(u32 virtual_addr,  u8 *pbuffer, u8 count);
void k3_mhl_reg_read_block(u32 virtual_addr, u8 *pbuffer, u8 count);
bool k3_mhl_sw_tpi_bank0(void);
bool k3_mhl_hw_tpimode(void);
u32 k3_mhl_slave2_parallel_addr(u32 slave_addr);
bool k3_mhl_hardware_tpimode(void);
bool k3_mhl_cbus_busy(void);
void k3_mhl_chip_initialize(void);
void k3_mhl_deviceisr(void);
void k3_mhl_process_rgnd(void);
void k3_mhl_tmds_control(bool enable);
bool k3_mhl_send_cbus_command(cbus_req_t * p_req);
void k3_mhl_pow_bit_change(bool enable);
u8 k3_mhl_read_edid(void);

#if USE_PP_MODE
void k3_mhl_tx_drv_set_3D_mode(bool s3D, u8 s3d_structure);
void k3_mhl_tx_drv_set_pixel_clk_freq(u32 pixel_clkfreq);
void k3_mhl_tx_drv_set_packed_pixel_status(int support_packed_pixel);
u8 k3_mhl_tx_drv_get_PPstatus(void);
void k3_mhl_tx_drv_apply_PPmode_settings(void);
#endif
extern int fsa9685_manual_detach(void);

#endif  // __K3_DRV_MHL_TX_H__

