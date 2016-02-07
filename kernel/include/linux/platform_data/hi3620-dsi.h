/*
 * Hisilicon Hi3620 MIPI DSI head file
 *
 * Copyright (c) 2013 Hisilicon Limited.
 * Copyright (c) 2013 Linaro Limited.
 *
 * Author:	Haojian Zhuang <haojian.zhuang@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef __HI3620_DSI_H
#define	__HI3620_DSI_H

enum DATA_TYPE {
	GEN_SHORT_WR_PARAM0 = 0x03,
	GEN_SHORT_WR_PARAM1 = 0x13,
	GEN_SHORT_WR_PARAM2 = 0x23,
	GEN_RD_PARAM0 = 0x04,
	GEN_RD_PARAM1 = 0x14,
	GEN_RD_PARAM2 = 0x24,
	DCS_SHORT_WR_PARAM0 = 0x05,
	DCS_SHORT_WR_PARAM1 = 0x15,
	DCS_RD_PARAM0 = 0x06,
	SET_MAX_PKT = 0x37,
	NULL_PKT = 0x09,
	BLANKING_PKT = 0x19,
	GEN_LONG_WR = 0x29,
	DCS_LONG_WR = 0x39,
	PACKED_PIXEL_16B = 0x0e,
	PACKED_PIXEL_18B = 0x1e,
	LOOSELY_PACKED_PIXEL_18B = 0x2e,
	PACKED_PIXEL_24B = 0x3e,
};

int hi3620_dsi_phy_write(void __iomem *reg_base, unsigned char addr,
			 unsigned char data);
unsigned char hi3620_dsi_phy_read(void __iomem *reg_base, unsigned char addr);

#endif
