/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/

#include "hisi_mipi_dsi.h"


/* 10MHz */
#define MAX_TX_ESC_CLK	(20)
#define DSI_BURST_MODE	DSI_BURST_SYNC_PULSES_1
#define DEFAULT_MIPI_CLK_RATE	(192 * 100000L)

static void get_dsi_phy_ctrl(uint32_t dsi_bit_clk,
	struct mipi_dsi_phy_ctrl *phy_ctrl)
{
	uint32_t range = 0;
	uint32_t m_pll = 0;

	BUG_ON(phy_ctrl == NULL);

	range = dsi_bit_clk * 2;

	/*
	** Step 1: Determine PLL Input divider ratio (N)
	** Refernce frequency is 19.2MHz, N is set to 2 to match the following limitation:.
	** 5MHz <= Frefclk/N <= 40MHz*/
	phy_ctrl->n_pll = 0x2;

	phy_ctrl->pll_unlocking_filter = 0xFF;

	/* Step 2: Calculate PLL loop divider ratio (M) */
	m_pll = range *  phy_ctrl->n_pll * 10 / (DEFAULT_MIPI_CLK_RATE / 100000);
	phy_ctrl->m_pll_1 = (m_pll - 1) & 0x1F;
	phy_ctrl->m_pll_2 = ((m_pll - 1) >> 5) | 0x80;

	/* Step 3: Determine CP current and LPF ctrl*/
	if (range <= 110) {
		phy_ctrl->cp_current = 0x2;
		phy_ctrl->lpf_ctrl = 0x2;
	} else if (range > 110 && range <= 150) {
		phy_ctrl->cp_current = 0x2;
		phy_ctrl->lpf_ctrl = 0x1;
	} else if (range > 150 && range <= 200) {
		phy_ctrl->cp_current = 0x9;
		phy_ctrl->lpf_ctrl = 0x1;
	} else if (range > 200 && range <= 250) {
		phy_ctrl->cp_current = 0x9;
		phy_ctrl->lpf_ctrl = 0x04;
	} else if (range > 250 && range <= 300) {
		phy_ctrl->cp_current = 0x6;
		phy_ctrl->lpf_ctrl = 0x4;
	} else if (range > 300 && range <= 400) {
		phy_ctrl->cp_current = 0x9;
		phy_ctrl->lpf_ctrl = 0x4;
	} else if (range > 400 && range <= 500) {
		phy_ctrl->cp_current = 0x6;
		phy_ctrl->lpf_ctrl = 0x4;
	} else if (range > 500 && range <= 600) {
		phy_ctrl->cp_current = 0x6;
		phy_ctrl->lpf_ctrl = 0x4;
	} else if (range > 600 && range <= 700) {
		phy_ctrl->cp_current = 0xa;
		phy_ctrl->lpf_ctrl = 0x4;
	} else if (range > 700 && range <= 900) {
		phy_ctrl->cp_current = 0xa;
		phy_ctrl->lpf_ctrl = 0x4;
	} else if (range > 900 && range <= 1100) {
		phy_ctrl->cp_current = 0xb;
		phy_ctrl->lpf_ctrl = 0x8;
	}else if (range > 1100 && range <= 1300) {
		phy_ctrl->cp_current = 0xb;
		phy_ctrl->lpf_ctrl = 0x8;
	} else if (range > 1300 && range <= 1500) {
		phy_ctrl->cp_current = 0xb;
		phy_ctrl->lpf_ctrl = 0x8;
	} else {
		HISI_FB_ERR("unsupport the value of PLL loop divider ratio (M) (%d)!\n", m_pll);
		return;
	}

	/* Bypass CP&LPF default values*/
	phy_ctrl->lpf_ctrl |= 0xC0;

	/* Step 4: N and M factors effective*/
	phy_ctrl->factors_effective = 0x30;//0x33;

	/* Step 5: Determine clk_lane_lp2hs_time, clk_lane_hs2lp_time, data_lane_lp2hs_time, data_lane_hs2lp_time and hsfreqrange*/
	if (range <= 90) {
		phy_ctrl->clk_lane_lp2hs_time = 32;
		phy_ctrl->clk_lane_hs2lp_time = 20;
		phy_ctrl->data_lane_lp2hs_time = 26;
		phy_ctrl->data_lane_hs2lp_time = 13;
		phy_ctrl->hsfreqrange = 0x00000000; /* 0000000 */
	} else if (range > 90 && range <= 100) {
		phy_ctrl->clk_lane_lp2hs_time = 35;
		phy_ctrl->clk_lane_hs2lp_time = 23;
		phy_ctrl->data_lane_lp2hs_time = 28;
		phy_ctrl->data_lane_hs2lp_time = 14;
		phy_ctrl->hsfreqrange = 0x00000010; /* 0010000 */
	} else if (range > 100 && range <= 110) {
		phy_ctrl->clk_lane_lp2hs_time = 32;
		phy_ctrl->clk_lane_hs2lp_time = 22;
		phy_ctrl->data_lane_lp2hs_time = 26;
		phy_ctrl->data_lane_hs2lp_time = 13;
		phy_ctrl->hsfreqrange = 0x00000020; /* 0100000 */
	} else if (range > 110 && range <= 130) {
		phy_ctrl->clk_lane_lp2hs_time = 31;
		phy_ctrl->clk_lane_hs2lp_time = 20;
		phy_ctrl->data_lane_lp2hs_time = 27;
		phy_ctrl->data_lane_hs2lp_time = 13;
		phy_ctrl->hsfreqrange = 0x00000001; /* 0000001 */
	} else if (range > 130 && range <= 140) {
		phy_ctrl->clk_lane_lp2hs_time = 33;
		phy_ctrl->clk_lane_hs2lp_time = 22;
		phy_ctrl->data_lane_lp2hs_time = 26;
		phy_ctrl->data_lane_hs2lp_time = 14;
		phy_ctrl->hsfreqrange = 0x00000011; /* 0010001 */
	} else if (range > 140 && range <= 150) {
		phy_ctrl->clk_lane_lp2hs_time = 33;
		phy_ctrl->clk_lane_hs2lp_time = 21;
		phy_ctrl->data_lane_lp2hs_time = 26;
		phy_ctrl->data_lane_hs2lp_time = 14;
		phy_ctrl->hsfreqrange = 0x00000021; /* 0100001 */
	} else if (range > 150 && range <= 170) {
		phy_ctrl->clk_lane_lp2hs_time = 32;
		phy_ctrl->clk_lane_hs2lp_time = 20;
		phy_ctrl->data_lane_lp2hs_time = 27;
		phy_ctrl->data_lane_hs2lp_time = 13;
		phy_ctrl->hsfreqrange = 0x00000002; /* 0000010 */
	} else if (range > 170 && range <= 180) {
		phy_ctrl->clk_lane_lp2hs_time = 36;
		phy_ctrl->clk_lane_hs2lp_time = 23;
		phy_ctrl->data_lane_lp2hs_time = 30;
		phy_ctrl->data_lane_hs2lp_time = 15;
		phy_ctrl->hsfreqrange = 0x00000012; /* 0010010 */
	} else if (range > 180 && range <= 200) {
		phy_ctrl->clk_lane_lp2hs_time = 40;
		phy_ctrl->clk_lane_hs2lp_time = 22;
		phy_ctrl->data_lane_lp2hs_time = 33;
		phy_ctrl->data_lane_hs2lp_time = 15;
		phy_ctrl->hsfreqrange = 0x00000022; /* 0100010 */
	} else if (range > 200 && range <= 220) {
		phy_ctrl->clk_lane_lp2hs_time = 40;
		phy_ctrl->clk_lane_hs2lp_time = 22;
		phy_ctrl->data_lane_lp2hs_time = 33;
		phy_ctrl->data_lane_hs2lp_time = 15;
		phy_ctrl->hsfreqrange = 0x00000003; /* 0000011 */
	} else if (range > 220 && range <= 240) {
		phy_ctrl->clk_lane_lp2hs_time = 44;
		phy_ctrl->clk_lane_hs2lp_time = 24;
		phy_ctrl->data_lane_lp2hs_time = 36;
		phy_ctrl->data_lane_hs2lp_time = 16;
		phy_ctrl->hsfreqrange = 0x00000013; /* 0010011 */
	} else if (range > 240 && range <= 250) {
		phy_ctrl->clk_lane_lp2hs_time = 48;
		phy_ctrl->clk_lane_hs2lp_time = 24;
		phy_ctrl->data_lane_lp2hs_time = 38;
		phy_ctrl->data_lane_hs2lp_time = 17;
		phy_ctrl->hsfreqrange = 0x00000023;  /* 0100011 */
	} else if (range > 250 && range <= 270) {
		phy_ctrl->clk_lane_lp2hs_time = 48;
		phy_ctrl->clk_lane_hs2lp_time = 24;
		phy_ctrl->data_lane_lp2hs_time = 38;
		phy_ctrl->data_lane_hs2lp_time = 17;
		phy_ctrl->hsfreqrange = 0x00000004; /* 0000100 */
	} else if (range > 270 && range <= 300) {
		phy_ctrl->clk_lane_lp2hs_time = 50;
		phy_ctrl->clk_lane_hs2lp_time = 27;
		phy_ctrl->data_lane_lp2hs_time = 41;
		phy_ctrl->data_lane_hs2lp_time = 18;
		phy_ctrl->hsfreqrange = 0x00000014; /* 0010100 */
	} else if (range > 300 && range <= 330) {
		phy_ctrl->clk_lane_lp2hs_time = 56;
		phy_ctrl->clk_lane_hs2lp_time = 28;
		phy_ctrl->data_lane_lp2hs_time = 45;
		phy_ctrl->data_lane_hs2lp_time = 18;
		phy_ctrl->hsfreqrange = 0x00000005; /* 0000101 */
	} else if (range > 330 && range <= 360) {
		phy_ctrl->clk_lane_lp2hs_time = 59;
		phy_ctrl->clk_lane_hs2lp_time = 28;
		phy_ctrl->data_lane_lp2hs_time = 48;
		phy_ctrl->data_lane_hs2lp_time = 19;
		phy_ctrl->hsfreqrange = 0x00000015; /* 0010101 */
	} else if (range > 360 && range <= 400) {
		phy_ctrl->clk_lane_lp2hs_time = 61;
		phy_ctrl->clk_lane_hs2lp_time = 30;
		phy_ctrl->data_lane_lp2hs_time = 50;
		phy_ctrl->data_lane_hs2lp_time = 20;
		phy_ctrl->hsfreqrange = 0x00000025; /* 0100101 */
	} else if (range > 400 && range <= 450) {
		phy_ctrl->clk_lane_lp2hs_time = 67;
		phy_ctrl->clk_lane_hs2lp_time = 31;
		phy_ctrl->data_lane_lp2hs_time = 55;
		phy_ctrl->data_lane_hs2lp_time = 21;
		phy_ctrl->hsfreqrange = 0x00000006; /* 0000110 */
	} else if (range > 450 && range <= 500) {
		phy_ctrl->clk_lane_lp2hs_time = 73;
		phy_ctrl->clk_lane_hs2lp_time = 31;
		phy_ctrl->data_lane_lp2hs_time = 59;
		phy_ctrl->data_lane_hs2lp_time = 22;
		phy_ctrl->hsfreqrange = 0x00000016; /* 0010110 */
	} else if (range > 500 && range <= 550) {
		phy_ctrl->clk_lane_lp2hs_time = 79;
		phy_ctrl->clk_lane_hs2lp_time = 36;
		phy_ctrl->data_lane_lp2hs_time = 63;
		phy_ctrl->data_lane_hs2lp_time = 24;
		phy_ctrl->hsfreqrange = 0x00000007; /* 0000111 */
	} else if (range > 550 && range <= 600) {
		phy_ctrl->clk_lane_lp2hs_time = 83;
		phy_ctrl->clk_lane_hs2lp_time = 37;
		phy_ctrl->data_lane_lp2hs_time = 68;
		phy_ctrl->data_lane_hs2lp_time = 25;
		phy_ctrl->hsfreqrange = 0x00000017; /* 0010111 */
	} else if (range > 600 && range <= 650) {
		phy_ctrl->clk_lane_lp2hs_time = 90;
		phy_ctrl->clk_lane_hs2lp_time = 38;
		phy_ctrl->data_lane_lp2hs_time = 73;
		phy_ctrl->data_lane_hs2lp_time = 27;
		phy_ctrl->hsfreqrange = 0x00000008; /* 0001000 */
	} else if (range > 650 && range <= 700) {
		phy_ctrl->clk_lane_lp2hs_time = 95;
		phy_ctrl->clk_lane_hs2lp_time = 40;
		phy_ctrl->data_lane_lp2hs_time = 77;
		phy_ctrl->data_lane_hs2lp_time = 28;
		phy_ctrl->hsfreqrange = 0x00000018; /* 0011000 */
	} else if (range > 700 && range <= 750) {
		phy_ctrl->clk_lane_lp2hs_time = 102;
		phy_ctrl->clk_lane_hs2lp_time = 40;
		phy_ctrl->data_lane_lp2hs_time = 84;
		phy_ctrl->data_lane_hs2lp_time = 28;
		phy_ctrl->hsfreqrange = 0x00000009; /* 0001001 */
	} else if (range > 750 && range <= 800) {
		phy_ctrl->clk_lane_lp2hs_time = 106;
		phy_ctrl->clk_lane_hs2lp_time = 42;
		phy_ctrl->data_lane_lp2hs_time = 87;
		phy_ctrl->data_lane_hs2lp_time = 30;
		phy_ctrl->hsfreqrange = 0x00000019; /* 0011001 */
	} else if (range > 800 && range <= 850) {
		phy_ctrl->clk_lane_lp2hs_time = 113;
		phy_ctrl->clk_lane_hs2lp_time = 44;
		phy_ctrl->data_lane_lp2hs_time = 93;
		phy_ctrl->data_lane_hs2lp_time = 31;
		phy_ctrl->hsfreqrange = 0x00000029; /* 0101001 */
	} else if (range > 850 && range <= 900) {
		phy_ctrl->clk_lane_lp2hs_time = 118;
		phy_ctrl->clk_lane_hs2lp_time = 47;
		phy_ctrl->data_lane_lp2hs_time = 98;
		phy_ctrl->data_lane_hs2lp_time = 32;
		phy_ctrl->hsfreqrange = 0x00000039; /* 0111001 */
	} else if (range > 900 && range <= 950) {
		phy_ctrl->clk_lane_lp2hs_time = 124;
		phy_ctrl->clk_lane_hs2lp_time = 47;
		phy_ctrl->data_lane_lp2hs_time = 102;
		phy_ctrl->data_lane_hs2lp_time = 34;
		phy_ctrl->hsfreqrange = 0x0000000a; /* 0001010 */
	} else if (range > 950 && range <= 1000) {
		phy_ctrl->clk_lane_lp2hs_time = 130;
		phy_ctrl->clk_lane_hs2lp_time = 49;
		phy_ctrl->data_lane_lp2hs_time = 107;
		phy_ctrl->data_lane_hs2lp_time = 35;
		phy_ctrl->hsfreqrange = 0x0000001a; /* 0011010 */
	} else if (range > 1000 && range <= 1050) {
		phy_ctrl->clk_lane_lp2hs_time = 135;
		phy_ctrl->clk_lane_hs2lp_time = 51;
		phy_ctrl->data_lane_lp2hs_time = 111;
		phy_ctrl->data_lane_hs2lp_time = 37;
		phy_ctrl->hsfreqrange = 0x0000002a; /* 0101010 */
	} else if (range > 1050 && range <= 1100) {
		phy_ctrl->clk_lane_lp2hs_time = 139;
		phy_ctrl->clk_lane_hs2lp_time = 51;
		phy_ctrl->data_lane_lp2hs_time = 114;
		phy_ctrl->data_lane_hs2lp_time = 38;
		phy_ctrl->hsfreqrange = 0x0000003a; /* 0111010 */
	} else if (range > 1100 && range <= 1150) {
		phy_ctrl->clk_lane_lp2hs_time = 146;
		phy_ctrl->clk_lane_hs2lp_time = 54;
		phy_ctrl->data_lane_lp2hs_time = 120;
		phy_ctrl->data_lane_hs2lp_time = 40;
		phy_ctrl->hsfreqrange = 0x0000000b; /* 0001011 */
	} else if (range > 1150 && range <= 1200) {
		phy_ctrl->clk_lane_lp2hs_time = 153;
		phy_ctrl->clk_lane_hs2lp_time = 57;
		phy_ctrl->data_lane_lp2hs_time = 125;
		phy_ctrl->data_lane_hs2lp_time = 41;
		phy_ctrl->hsfreqrange = 0x0000001b; /* 0011011 */
	} else if (range > 1200 && range <= 1250) {
		phy_ctrl->clk_lane_lp2hs_time = 158;
		phy_ctrl->clk_lane_hs2lp_time = 58;
		phy_ctrl->data_lane_lp2hs_time = 130;
		phy_ctrl->data_lane_hs2lp_time = 42;
		phy_ctrl->hsfreqrange = 0x0000002b; /* 0101011 */
	} else if (range > 1250 && range <= 1300) {
		phy_ctrl->clk_lane_lp2hs_time = 163;
		phy_ctrl->clk_lane_hs2lp_time = 58;
		phy_ctrl->data_lane_lp2hs_time = 135;
		phy_ctrl->data_lane_hs2lp_time = 44;
		phy_ctrl->hsfreqrange = 0x0000003b; /* 0111011 */
	} else if (range > 1300 && range <= 1350) {
		phy_ctrl->clk_lane_lp2hs_time = 168;
		phy_ctrl->clk_lane_hs2lp_time = 60;
		phy_ctrl->data_lane_lp2hs_time = 140;
		phy_ctrl->data_lane_hs2lp_time = 45;
		phy_ctrl->hsfreqrange = 0x0000000c; /* 0001100 */
	} else if (range > 1350 && range <= 1400) {
		phy_ctrl->clk_lane_lp2hs_time = 172;
		phy_ctrl->clk_lane_hs2lp_time = 64;
		phy_ctrl->data_lane_lp2hs_time = 144;
		phy_ctrl->data_lane_hs2lp_time = 47;
		phy_ctrl->hsfreqrange = 0x0000001c; /* 0011100 */
	} else if (range > 1400 && range <= 1450) {
		phy_ctrl->clk_lane_lp2hs_time = 176;
		phy_ctrl->clk_lane_hs2lp_time = 65;
		phy_ctrl->data_lane_lp2hs_time = 148;
		phy_ctrl->data_lane_hs2lp_time = 48;
		phy_ctrl->hsfreqrange = 0x0000002c; /* 0101100 */
	} else if (range > 1450 && range <= 1500) {
		phy_ctrl->clk_lane_lp2hs_time = 181;
		phy_ctrl->clk_lane_hs2lp_time = 66;
		phy_ctrl->data_lane_lp2hs_time = 153;
		phy_ctrl->data_lane_hs2lp_time = 50;
		phy_ctrl->hsfreqrange = 0x0000003c; /* 0111100 */
	} else {
		HISI_FB_ERR("unsupport this range(%d)!\n", range);
		return;
	}

	phy_ctrl->hsfreqrange = phy_ctrl->hsfreqrange << 1;

	phy_ctrl->lane_byte_clk = dsi_bit_clk / 4;
	phy_ctrl->clk_division = ((phy_ctrl->lane_byte_clk % MAX_TX_ESC_CLK) > 0) ?
		(phy_ctrl->lane_byte_clk / MAX_TX_ESC_CLK + 1) :
		(phy_ctrl->lane_byte_clk / MAX_TX_ESC_CLK);
	phy_ctrl->burst_mode = DSI_BURST_MODE;
}

static void mipi_init(struct hisi_fb_data_type *hisifd, char __iomem *mipi_dsi_base)
{
	uint32_t hline_time = 0;
	uint32_t hsa_time = 0;
	uint32_t hbp_time = 0;
	uint32_t pixel_clk = 0;
	unsigned long dw_jiffies = 0;
	struct mipi_dsi_phy_ctrl phy_ctrl = {0};
	uint32_t tmp = 0;
	bool is_ready = false;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);
	BUG_ON(mipi_dsi_base == NULL);

	pinfo = &(hisifd->panel_info);
	get_dsi_phy_ctrl(pinfo->mipi.dsi_bit_clk, &phy_ctrl);

	/* config TE */
	if (is_mipi_cmd_panel(hisifd)) {
		/* config to command mode */
		set_reg(mipi_dsi_base + MIPIDSI_MODE_CFG_OFFSET, 0x1, 1, 0);
		/* ALLOWED_CMD_SIZE */
		set_reg(mipi_dsi_base + MIPIDSI_EDPI_CMD_SIZE_OFFSET, pinfo->xres, 16, 0);
		/* cnt=2 in update-patial scene, cnt nees to be checked for different panels*/
		set_reg(mipi_dsi_base + MIPIDSI_HS_WR_TO_CNT_OFFSET, 0x1000002, 25, 0);

		/* phy_stop_wait_time */
		set_reg(mipi_dsi_base + MIPIDSI_PHY_IF_CFG_OFFSET, 0x30, 8, 8);

		/* FIXME: test tearing effect, if use gpio, no need */
		/*set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x1, 1, 0);*/
	}

	/*--------------configuring the DPI packet transmission----------------*/
	/*
	** 1. Global configuration
	** Configure Register PHY_IF_CFG with the correct number of lanes
	** to be used by the controller.
	*/
	set_reg(mipi_dsi_base + MIPIDSI_PHY_IF_CFG_OFFSET, pinfo->mipi.lane_nums, 2, 0);

	/*
	** 2. Configure the DPI Interface:
	** This defines how the DPI interface interacts with the controller.
	*/
	set_reg(mipi_dsi_base + MIPIDSI_DPI_VCID_OFFSET, pinfo->mipi.vc, 2, 0);
	set_reg(mipi_dsi_base + MIPIDSI_DPI_COLOR_CODING_OFFSET, pinfo->mipi.color_mode, 4, 0);

	set_reg(mipi_dsi_base + MIPIDSI_DPI_CFG_POL_OFFSET, pinfo->ldi.data_en_plr, 1, 0);
	set_reg(mipi_dsi_base + MIPIDSI_DPI_CFG_POL_OFFSET, pinfo->ldi.vsync_plr, 1, 1);
	set_reg(mipi_dsi_base + MIPIDSI_DPI_CFG_POL_OFFSET, pinfo->ldi.hsync_plr, 1, 2);
	set_reg(mipi_dsi_base + MIPIDSI_DPI_CFG_POL_OFFSET, 0x0, 1, 3);
	set_reg(mipi_dsi_base + MIPIDSI_DPI_CFG_POL_OFFSET, 0x0, 1, 4);

	if (pinfo->bpp == LCD_RGB666) {
		set_reg(mipi_dsi_base + MIPIDSI_DPI_COLOR_CODING_OFFSET, 0x1, 1, 8);
	}

	/*
	** 3. Select the Video Transmission Mode:
	** This defines how the processor requires the video line to be
	** transported through the DSI link.
	*/

	/* video mode: low power mode */
	set_reg(mipi_dsi_base + MIPIDSI_VID_MODE_CFG_OFFSET, 0x3f, 6, 8);
	/* set_reg(mipi_dsi_base + MIPIDSI_VID_MODE_CFG_OFFSET, 0x0, 1, 14); */

	/* burst mode */
	set_reg(mipi_dsi_base + MIPIDSI_VID_MODE_CFG_OFFSET, phy_ctrl.burst_mode, 2, 0);
	set_reg(mipi_dsi_base + MIPIDSI_VID_PKT_SIZE_OFFSET, pinfo->xres, 14, 0);

	/* for dsi read, BTA enable*/
	set_reg(mipi_dsi_base + MIPIDSI_PCKHDL_CFG_OFFSET, 0x1, 1, 2);

	/*
	** 4. Define the DPI Horizontal timing configuration:
	**
	** Hsa_time = HSA*(PCLK period/Clk Lane Byte Period);
	** Hbp_time = HBP*(PCLK period/Clk Lane Byte Period);
	** Hline_time = (HSA+HBP+HACT+HFP)*(PCLK period/Clk Lane Byte Period);
	*/
	pixel_clk = pinfo->pxl_clk_rate / 1000000;
	hsa_time = pinfo->ldi.h_pulse_width * phy_ctrl.lane_byte_clk / pixel_clk;
	hbp_time = pinfo->ldi.h_back_porch * phy_ctrl.lane_byte_clk / pixel_clk;
	hline_time = (pinfo->ldi.h_pulse_width + pinfo->ldi.h_back_porch +
		pinfo->xres + pinfo->ldi.h_front_porch) *
		phy_ctrl.lane_byte_clk / pixel_clk;
	set_reg(mipi_dsi_base + MIPIDSI_VID_HSA_TIME_OFFSET, hsa_time, 12, 0);
	set_reg(mipi_dsi_base + MIPIDSI_VID_HBP_TIME_OFFSET, hbp_time, 12, 0);
	set_reg(mipi_dsi_base + MIPIDSI_VID_HLINE_TIME_OFFSET, hline_time, 15, 0);

	/*
	** 5. Define the Vertical line configuration:
	*/
	set_reg(mipi_dsi_base + MIPIDSI_VID_VSA_LINES_OFFSET, pinfo->ldi.v_pulse_width, 10, 0);
	set_reg(mipi_dsi_base + MIPIDSI_VID_VBP_LINES_OFFSET, pinfo->ldi.v_back_porch, 10, 0);
	set_reg(mipi_dsi_base + MIPIDSI_VID_VFP_LINES_OFFSET, pinfo->ldi.v_front_porch, 10, 0);
	set_reg(mipi_dsi_base + MIPIDSI_VID_VACTIVE_LINES_OFFSET, pinfo->yres, 14, 0);
	set_reg(mipi_dsi_base + MIPIDSI_TO_CNT_CFG_OFFSET, 0x7FF, 16, 0);

	/* Configure core's phy parameters */
	set_reg(mipi_dsi_base + MIPIDSI_PHY_TMR_CFG_OFFSET, 4095, 15, 0);
	set_reg(mipi_dsi_base + MIPIDSI_PHY_TMR_LPCLK_CFG_OFFSET, phy_ctrl.clk_lane_lp2hs_time, 10, 0);
	set_reg(mipi_dsi_base + MIPIDSI_PHY_TMR_LPCLK_CFG_OFFSET, phy_ctrl.clk_lane_hs2lp_time, 10, 16);
	set_reg(mipi_dsi_base + MIPIDSI_PHY_TMR_CFG_OFFSET, phy_ctrl.data_lane_lp2hs_time, 8, 16);
	set_reg(mipi_dsi_base + MIPIDSI_PHY_TMR_CFG_OFFSET, phy_ctrl.data_lane_hs2lp_time, 8, 24);

	/*------------DSI and D-PHY Initialization-----------------*/
	/* 1. Waking up Core */
	set_reg(mipi_dsi_base + MIPIDSI_PWR_UP_OFFSET, 0x1, 1, 0);

	/*
	** 3. Configure the TX_ESC clock frequency to a frequency lower than 20 MHz
	** that is the maximum allowed frequency for D-PHY ESCAPE mode.
	*/

	set_reg(mipi_dsi_base + MIPIDSI_CLKMGR_CFG_OFFSET, phy_ctrl.clk_division, 8, 0);
	set_reg(mipi_dsi_base + MIPIDSI_CLKMGR_CFG_OFFSET, phy_ctrl.clk_division, 8, 8);


	/*
	** 4. Configure the DPHY PLL clock frequency through the TEST Interface to
	** operate at XX Hz,
	*/

	/* Write CP current */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010011);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, phy_ctrl.cp_current);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* Write LPF Control */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010012);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, phy_ctrl.lpf_ctrl);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/*Configured N and M factors effective*/
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010019);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, phy_ctrl.factors_effective);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* Write N Pll */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010017);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, phy_ctrl.n_pll-1);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* Write M Pll part 1 */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010018);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, phy_ctrl.m_pll_1);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* Write M Pll part 2 */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010018);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, phy_ctrl.m_pll_2);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* Set hsfreqrange */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010044);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, phy_ctrl.hsfreqrange);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* Set PLL unlocking filter */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010016);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, phy_ctrl.pll_unlocking_filter);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

#if 0
	/* Set the phy direction*/
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x000100b0);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00000001);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
#endif

	outp32(mipi_dsi_base + MIPIDSI_PHY_RSTZ_OFFSET, 0x0000000F);

	is_ready = false;
	dw_jiffies = jiffies + HZ / 2;
	do {
		tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
		if ((tmp & 0x00000001) == 0x00000001) {
			is_ready = true;
			break;
		}
	} while (time_after(dw_jiffies, jiffies));

	if (!is_ready) {
		HISI_FB_INFO("fb%d, phylock is not ready!MIPIDSI_PHY_STATUS_OFFSET=0x%x.\n",
			hisifd->index, tmp);
	}

	is_ready = false;
	dw_jiffies = jiffies + HZ / 2;
	do {
		tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
		if ((tmp & 0x00000004) == 0x00000004) {
			is_ready = true;
			break;
		}
	} while (time_after(dw_jiffies, jiffies));

	if (!is_ready) {
		HISI_FB_INFO("fb%d, phystopstateclklane is not ready! MIPIDSI_PHY_STATUS_OFFSET=0x%x.\n",
			hisifd->index, tmp);
	}
}

int mipi_dsi_clk_enable(struct hisi_fb_data_type *hisifd)
{
	int ret = 0;
	struct clk *clk_tmp = NULL;

	BUG_ON(hisifd == NULL);

	clk_tmp = hisifd->dss_dphy0_clk;
	if (clk_tmp) {
		ret = clk_prepare(clk_tmp);
		if (ret) {
			HISI_FB_ERR("fb%d dss_dphy0_clk clk_prepare failed, error=%d!\n",
				hisifd->index, ret);
			return -EINVAL;
		}

		ret = clk_enable(clk_tmp);
		if (ret) {
			HISI_FB_ERR("fb%d dss_dphy0_clk clk_enable failed, error=%d!\n",
				hisifd->index, ret);
			return -EINVAL;
		}
	}

	if (is_dual_mipi_panel(hisifd)) {
		clk_tmp = hisifd->dss_dphy1_clk;
		if (clk_tmp) {
			ret = clk_prepare(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_dphy1_clk clk_prepare failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}

			ret = clk_enable(clk_tmp);
			if (ret) {
				HISI_FB_ERR("fb%d dss_dphy1_clk clk_enable failed, error=%d!\n",
					hisifd->index, ret);
				return -EINVAL;
			}
		}
	}

	return 0;
}

int mipi_dsi_clk_disable(struct hisi_fb_data_type *hisifd)
{
	struct clk *clk_tmp = NULL;

	BUG_ON(hisifd == NULL);

	clk_tmp = hisifd->dss_dphy0_clk;
	if (clk_tmp) {
		clk_disable(clk_tmp);
		clk_unprepare(clk_tmp);
	}

	if (is_dual_mipi_panel(hisifd)) {
		clk_tmp = hisifd->dss_dphy1_clk;
		if (clk_tmp) {
			clk_disable(clk_tmp);
			clk_unprepare(clk_tmp);
		}
	}

	return 0;
}


/*******************************************************************************
**
*/
static int mipi_dsi_on_sub1(struct hisi_fb_data_type *hisifd, char __iomem *mipi_dsi_base)
{
	BUG_ON(mipi_dsi_base == NULL);

	/* mipi init */
	mipi_init(hisifd, mipi_dsi_base);

	/* switch to cmd mode */
	set_reg(mipi_dsi_base + MIPIDSI_MODE_CFG_OFFSET, 0x1, 1, 0);
	/* cmd mode: low power mode */
	set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x7f, 7, 8);
	set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0xf, 4, 16);
	set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x1, 1, 24);
	/* disable generate High Speed clock */
	/* delete? */
	set_reg(mipi_dsi_base + MIPIDSI_LPCLK_CTRL_OFFSET, 0x0, 1, 0);

	return 0;
}

static int mipi_dsi_on_sub2(struct hisi_fb_data_type *hisifd, char __iomem *mipi_dsi_base)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(mipi_dsi_base == NULL);

	if (is_mipi_video_panel(hisifd)) {
		/* switch to video mode */
		set_reg(mipi_dsi_base + MIPIDSI_MODE_CFG_OFFSET, 0x0, 1, 0);
	}

	if (is_mipi_cmd_panel(hisifd)) {
		/* cmd mode: high speed mode */
		set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 7, 8);
		set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 4, 16);
		set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 1, 24);
	}

	/* enable EOTP TX */
	set_reg(mipi_dsi_base + MIPIDSI_PCKHDL_CFG_OFFSET, 0x1, 1, 0);

	/* enable generate High Speed clock */
	set_reg(mipi_dsi_base + MIPIDSI_LPCLK_CTRL_OFFSET, 0x1, 1, 0);

	return 0;
}

int mipi_dsi_off_sub(struct hisi_fb_data_type *hisifd, char __iomem *mipi_dsi_base)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(mipi_dsi_base == NULL);

	/* switch to cmd mode */
	set_reg(mipi_dsi_base + MIPIDSI_MODE_CFG_OFFSET, 0x1, 1, 0);
	/* cmd mode: low power mode */
	set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x7f, 7, 8);
	set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0xf, 4, 16);
	set_reg(mipi_dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x1, 1, 24);

	/* disable generate High Speed clock */
	set_reg(mipi_dsi_base + MIPIDSI_LPCLK_CTRL_OFFSET, 0x0, 1, 0);

	/* shutdown d_phy */
	set_reg(mipi_dsi_base +  MIPIDSI_PHY_RSTZ_OFFSET, 0x0, 3, 0);

	return 0;
}

static int mipi_dsi_ulps_enter(struct hisi_fb_data_type *hisifd,
	char __iomem *mipi_dsi_base)
{
	uint32_t tmp = 0;
	uint32_t cmp_val = 0;
	uint32_t try_times = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(mipi_dsi_base == NULL);

	HISI_FB_DEBUG("fb%d, +!\n", hisifd->index);

	if (hisifd->panel_info.mipi.lane_nums >= DSI_4_LANES) {
		cmp_val = (BIT(5) | BIT(8) | BIT(10) | BIT(12));
	} else if (hisifd->panel_info.mipi.lane_nums >= DSI_3_LANES) {
		cmp_val = (BIT(5) | BIT(8) | BIT(10));
	} else if (hisifd->panel_info.mipi.lane_nums >= DSI_2_LANES) {
		cmp_val = (BIT(5) | BIT(8));
	} else {
		cmp_val = (BIT(5));
	}

	/* disable DPHY clock lane's Hight Speed Clock */
	set_reg(mipi_dsi_base + MIPIDSI_LPCLK_CTRL_OFFSET, 0x0, 1, 0);

	/* request that data lane and clock lane enter ULPS */
	set_reg(mipi_dsi_base + MIPIDSI_PHY_ULPS_CTRL_OFFSET, 0x4, 4, 0);

	/* check DPHY status */
	try_times = 0;
	tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
	while ((tmp & cmp_val) != 0) {
		udelay(10);
		if (++try_times > 100) {
			HISI_FB_ERR("fb%d, check DPHY data lane status failed! MIPIDSI_PHY_STATUS=0x%x.\n",
				hisifd->index, tmp);
			break;
		}

		tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
	}

	set_reg(mipi_dsi_base + MIPIDSI_PHY_ULPS_CTRL_OFFSET, 0x5, 4, 0);

	/* check DPHY status */
	try_times = 0;
	tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
	while ((tmp & BIT(3)) != 0) {
		udelay(10);
		if (++try_times > 100) {
			HISI_FB_ERR("fb%d, check DPHY clock lane status failed! MIPIDSI_PHY_STATUS=0x%x.\n",
				hisifd->index, tmp);
			break;
		}

		tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
	}

	/* disable DPHY PLL, force_pll = 0 */
	outp32(mipi_dsi_base + MIPIDSI_PHY_RSTZ_OFFSET, 0x7);

	/* Test Code @ 0x20 = 0x0 */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010020);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* disable refclk and cfgclk */
	if (is_dual_mipi_panel(hisifd))
		outp32(hisifd->crgperi_base + PERDIS3_OFFSET, 0x500000);
	else
		outp32(hisifd->crgperi_base + PERDIS3_OFFSET, 0x100000);

	HISI_FB_DEBUG("fb%d, -!\n", hisifd->index);

	return 0;
}

static int mipi_dsi_ulps_exit(struct hisi_fb_data_type *hisifd,
	char __iomem *mipi_dsi_base)
{
	uint32_t tmp = 0;
	uint32_t cmp_val = 0;
	uint32_t try_times = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(mipi_dsi_base == NULL);

	HISI_FB_DEBUG("fb%d, +!\n", hisifd->index);

	if (hisifd->panel_info.mipi.lane_nums >= DSI_4_LANES) {
		cmp_val = (BIT(3) | BIT(5) | BIT(8) | BIT(10) | BIT(12));
	} else if (hisifd->panel_info.mipi.lane_nums >= DSI_3_LANES) {
		cmp_val = (BIT(3) | BIT(5) | BIT(8) | BIT(10));
	} else if (hisifd->panel_info.mipi.lane_nums >= DSI_2_LANES) {
		cmp_val = (BIT(3) | BIT(5) | BIT(8));
	} else {
		cmp_val = (BIT(3) | BIT(5));
	}

	/* enable refclk and cfgclk */
	if (is_dual_mipi_panel(hisifd))
		outp32(hisifd->crgperi_base + PEREN3_OFFSET, 0x500000);
	else
		outp32(hisifd->crgperi_base + PEREN3_OFFSET, 0x100000);

	/* Test Code @ 0x20 = 0x45 */
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010020);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00000045);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* enable DPHY PLL, force_pll = 1 */
	outp32(mipi_dsi_base + MIPIDSI_PHY_RSTZ_OFFSET, 0xF);

	/* wait DPHY PLL Lock */
	try_times = 0;
	tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
	while ((tmp & BIT(0)) != 0X1) {
		udelay(10);
		if (++try_times > 100) {
			HISI_FB_ERR("fb%d, failed to wait DPHY PLL Lock!MIPIDSI_PHY_STATUS=0x%x.\n",
				hisifd->index, tmp);
			break;
		}

		tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
	}

	/* request that data lane and clock lane exit ULPS */
	outp32(mipi_dsi_base + MIPIDSI_PHY_ULPS_CTRL_OFFSET, 0xF);
	try_times= 0;
	tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
	while ((tmp & cmp_val) != cmp_val) {
		udelay(10);
		if (++try_times > 100) {
			HISI_FB_ERR("fb%d, failed to request that data lane and clock lane exit ULPS!MIPIDSI_PHY_STATUS=0x%x.\n",
				hisifd->index, tmp);
			break;
		}

		tmp = inp32(mipi_dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
	}

	/* mipi spec */
	mdelay(1);

	/* clear PHY_ULPS_CTRL */
	outp32(mipi_dsi_base + MIPIDSI_PHY_ULPS_CTRL_OFFSET, 0x0);

	/* enable DPHY clock lane's Hight Speed Clock */
	set_reg(mipi_dsi_base + MIPIDSI_LPCLK_CTRL_OFFSET, 0x1, 1, 0);

	HISI_FB_DEBUG("fb%d, -!\n", hisifd->index);

	return 0;
}

int mipi_dsi_ulps_cfg(struct hisi_fb_data_type *hisifd, int enable)
{
	int ret = 0;
	char __iomem *mipi_dsi0_base = NULL;
	char __iomem *mipi_dsi1_base = NULL;

	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi1_base = hisifd->dss_base + DSS_MIPI_DSI1_OFFSET;

	if (enable) {
		mipi_dsi_ulps_exit(hisifd, mipi_dsi0_base);
		if (is_dual_mipi_panel(hisifd))
			mipi_dsi_ulps_exit(hisifd, mipi_dsi1_base);
	} else {
		mipi_dsi_ulps_enter(hisifd, mipi_dsi0_base);
		if (is_dual_mipi_panel(hisifd))
			mipi_dsi_ulps_enter(hisifd, mipi_dsi1_base);
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

void mipi_dsi_bugfix(struct hisi_fb_data_type *hisifd)
{
	char __iomem *mipi_dsi0_base = NULL;
	char __iomem *mipi_dsi1_base = NULL;

	BUG_ON(hisifd == NULL);

	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi1_base = hisifd->dss_base + DSS_MIPI_DSI1_OFFSET;

	/* fix dphy bug: lp2hs unsteady */

	/*wait dsi stop  TBD*/

	/* Power Off PLL */
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010013);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00000040);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	/* Power On PLL */
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010013);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00000070);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
	outp32(mipi_dsi0_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

	if (is_dual_mipi_panel(hisifd)) {
		/* Power Off PLL */
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010013);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00000040);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);

		/* Power On PLL */
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00010013);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL1_OFFSET, 0x00000070);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000002);
		outp32(mipi_dsi1_base + MIPIDSI_PHY_TST_CTRL0_OFFSET, 0x00000000);
	}
}


/*******************************************************************************
**
*/
static int mipi_dsi_set_fastboot(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_set_fastboot(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int mipi_dsi_on(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	char __iomem *mipi_dsi1_base = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi1_base = hisifd->dss_base + DSS_MIPI_DSI1_OFFSET;

	/* set LCD init step before LCD on*/
	hisifd->panel_info.lcd_init_step = LCD_INIT_POWER_ON;
	ret = panel_next_on(pdev);

	mipi_dsi_clk_enable(hisifd);

	mipi_dsi_on_sub1(hisifd, mipi_dsi0_base);
	if (is_dual_mipi_panel(hisifd))
		mipi_dsi_on_sub1(hisifd, mipi_dsi1_base);

	ret = panel_next_on(pdev);

	mipi_dsi_on_sub2(hisifd, mipi_dsi0_base);
	if (is_dual_mipi_panel(hisifd))
		mipi_dsi_on_sub2(hisifd, mipi_dsi1_base);

	/* mipi hs video/command mode */
	ret = panel_next_on(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int mipi_dsi_off(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	char __iomem *mipi_dsi1_base = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi1_base = hisifd->dss_base + DSS_MIPI_DSI1_OFFSET;

	ret = panel_next_off(pdev);

	mipi_dsi_off_sub(hisifd, mipi_dsi0_base);
	if (is_dual_mipi_panel(hisifd))
		mipi_dsi_off_sub(hisifd, mipi_dsi1_base);

	mipi_dsi_clk_disable(hisifd);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int mipi_dsi_remove(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_remove(pdev);

	if (hisifd->dss_dphy0_clk) {
		clk_put(hisifd->dss_dphy0_clk);
		hisifd->dss_dphy0_clk = NULL;
	}

	if (hisifd->dss_dphy1_clk) {
		clk_put(hisifd->dss_dphy1_clk);
		hisifd->dss_dphy1_clk = NULL;
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int mipi_dsi_set_backlight(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_set_backlight(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int mipi_dsi_vsync_ctrl(struct platform_device *pdev, int enable)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	char __iomem *mipi_dsi1_base = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi1_base = hisifd->dss_base + DSS_MIPI_DSI1_OFFSET;

	if (enable) {
		ret = panel_next_vsync_ctrl(pdev, enable);
	} else {
		ret = panel_next_vsync_ctrl(pdev, enable);
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int mipi_dsi_frc_handle(struct platform_device *pdev, int fps)
{
#if 0
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	char __iomem *mipi_dsi1_base = NULL;
	uint32_t hline_time = 0;
	uint32_t pixel_clk = 0;
	uint32_t vertical_timing = 0;
	uint32_t horizontal_timing = 0;
	uint32_t h_front_porch = 0;
	int ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	pinfo = &(hisifd->panel_info);
	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi1_base = hisifd->dss_base + DSS_MIPI_DSI1_OFFSET;

	/* calculate new HFP based on target_fps */
	vertical_timing = pinfo->yres + pinfo->ldi.v_back_porch
		+ pinfo->ldi.v_front_porch + pinfo->ldi.v_pulse_width;
	horizontal_timing = pinfo->pxl_clk_rate / (vertical_timing * fps);

	/* new HFP*/
	/*
	pinfo->ldi.h_front_porch = horizontal_timing - pinfo->xres
		-pinfo->ldi.h_back_porch - pinfo->ldi.h_pulse_width;
	*/
	h_front_porch = horizontal_timing - pinfo->xres
		-pinfo->ldi.h_back_porch - pinfo->ldi.h_pulse_width;

	pixel_clk = pinfo->pxl_clk_rate / 1000000;
	/* update hline_time */
	hline_time = (pinfo->ldi.h_pulse_width + pinfo->ldi.h_back_porch +
		pinfo->xres + h_front_porch) *
		(pinfo->mipi.dsi_bit_clk / 4) / pixel_clk;


	/* reset dsi core */
	set_reg(dss_base + MIPIDSI_PWR_UP_OFFSET, 0x0, 1, 0);
	set_reg(dss_base + MIPIDSI_VID_HLINE_TIME_OFFSET, hline_time, 15, 0);
	set_reg(dss_base + LDI_HRZ_CTRL0_OFFSET, h_front_porch, 12, 0);
	/* power on dsi core */
	set_reg(dss_base + MIPIDSI_PWR_UP_OFFSET, 0x1, 1, 0);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
#endif

	return 0;
}

static int mipi_dsi_esd_handle(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = panel_next_esd_handle(pdev);

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static int mipi_dsi_set_display_region(struct platform_device *pdev, struct dss_rect *dirty)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL || dirty == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("index=%d, enter!\n", hisifd->index);

	ret = panel_next_set_display_region(pdev, dirty);

	HISI_FB_DEBUG("index=%d, exit!\n", hisifd->index);

	return ret;
}

static int mipi_dsi_lcd_check_reg(struct platform_device *pdev)
{
	BUG_ON(pdev == NULL);

	return panel_next_lcd_check_reg(pdev);
}

static int mipi_dsi_lcd_mipi_detect(struct platform_device *pdev)
{
	BUG_ON(pdev == NULL);

	return panel_next_lcd_mipi_detect(pdev);
}

static int mipi_dsi_clk_irq_setup(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	int ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	hisifd->dss_dphy0_clk = devm_clk_get(&pdev->dev, hisifd->dss_dphy0_clk_name);
	if (IS_ERR(hisifd->dss_dphy0_clk)) {
		ret = PTR_ERR(hisifd->dss_dphy0_clk);
		return ret;
	} else {
		ret = clk_set_rate(hisifd->dss_dphy0_clk, DEFAULT_MIPI_CLK_RATE);
		if (ret < 0) {
			HISI_FB_ERR("fb%d dss_dphy0_clk clk_set_rate(%lu) failed, error=%d!\n",
				hisifd->index, DEFAULT_MIPI_CLK_RATE, ret);
			return -EINVAL;
		}

		HISI_FB_INFO("dss_dphy0_clk:[%lu]->[%lu].\n",
			DEFAULT_MIPI_CLK_RATE, clk_get_rate(hisifd->dss_dphy0_clk));
	}

	if (is_dual_mipi_panel(hisifd)) {
		hisifd->dss_dphy1_clk = devm_clk_get(&pdev->dev, hisifd->dss_dphy1_clk_name);
		if (IS_ERR(hisifd->dss_dphy1_clk)) {
			ret = PTR_ERR(hisifd->dss_dphy1_clk);
			return ret;
		} else {
			ret = clk_set_rate(hisifd->dss_dphy1_clk, DEFAULT_MIPI_CLK_RATE);
			if (ret < 0) {
				HISI_FB_ERR("fb%d dss_dphy1_clk clk_set_rate(%lu) failed, error=%d!\n",
					hisifd->index, DEFAULT_MIPI_CLK_RATE, ret);
				return -EINVAL;
			}

			HISI_FB_INFO("dss_dphy1_clk:[%lu]->[%lu].\n",
				DEFAULT_MIPI_CLK_RATE, clk_get_rate(hisifd->dss_dphy1_clk));
		}
	}

	return ret;
}

static int mipi_dsi_probe(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct platform_device *dpp_dev = NULL;
	struct hisi_fb_panel_data *pdata = NULL;
	int ret = 0;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	ret = mipi_dsi_clk_irq_setup(pdev);
	if (ret) {
		HISI_FB_ERR("fb%d mipi_dsi_irq_clk_setup failed, error=%d!\n", hisifd->index, ret);
		goto err;
	}

	/* alloc device */
	dpp_dev = platform_device_alloc(DEV_NAME_DSS_DPE, pdev->id);
	if (!dpp_dev) {
		HISI_FB_ERR("fb%d platform_device_alloc failed, error=%d!\n", hisifd->index, ret);
		ret = -ENOMEM;
		goto err_device_alloc;
	}

	/* link to the latest pdev */
	hisifd->pdev = dpp_dev;

	/* alloc panel device data */
	ret = platform_device_add_data(dpp_dev, dev_get_platdata(&pdev->dev),
		sizeof(struct hisi_fb_panel_data));
	if (ret) {
		HISI_FB_ERR("fb%d platform_device_add_data failed error=%d!\n", hisifd->index, ret);
		goto err_device_put;
	}

	/* data chain */
	pdata = dev_get_platdata(&dpp_dev->dev);
	pdata->set_fastboot = mipi_dsi_set_fastboot;
	pdata->on = mipi_dsi_on;
	pdata->off = mipi_dsi_off;
	pdata->remove = mipi_dsi_remove;
	pdata->set_backlight = mipi_dsi_set_backlight;
	pdata->vsync_ctrl = mipi_dsi_vsync_ctrl;
	pdata->frc_handle = mipi_dsi_frc_handle;
	pdata->esd_handle = mipi_dsi_esd_handle;
	pdata->set_display_region= mipi_dsi_set_display_region;
	pdata->lcd_check_reg = mipi_dsi_lcd_check_reg;
	pdata->lcd_mipi_detect = mipi_dsi_lcd_mipi_detect;
	pdata->next = pdev;

	/* get/set panel info */
	memcpy(&hisifd->panel_info, pdata->panel_info, sizeof(struct hisi_panel_info));

	/* set driver data */
	platform_set_drvdata(dpp_dev, hisifd);
	/* device add */
	ret = platform_device_add(dpp_dev);
	if (ret) {
		HISI_FB_ERR("fb%d platform_device_add failed, error=%d!\n", hisifd->index, ret);
		goto err_device_put;
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;

err_device_put:
	platform_device_put(dpp_dev);
err_device_alloc:
err:
	return ret;
}

static struct platform_driver this_driver = {
	.probe = mipi_dsi_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = DEV_NAME_MIPIDSI,
	},
};

static int __init mipi_dsi_driver_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(mipi_dsi_driver_init);
