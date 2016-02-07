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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/raid/pq.h>
#include <linux/clk.h>

#include "balong_fb.h"
#include "mipi_reg.h"
#include "mipi_dsi.h"
#include "ldi_reg.h"
#include <linux/hw_lcd_common.h>
#if ADE_DEBUG_LOG_ENABLE
#include <linux/debugfs.h>
#include <linux/string.h>

/* used for debugfs to test LCD */
static struct lcd_debug_cmd g_mipi_lcd_debug_info = {0};
int g_panel_lcd_mode = 1;  /* 1: Command LCD, 0: Video LCD, used to set_dsi_mode() */

#endif
extern unsigned int cpufreq_get_fb(unsigned int cpu);
/* 15MHz */
#define	MAX_TX_ESC_CLK	(15)
#define	DSI_BURST_MODE	DSI_BURST_SYNC_PULSES_1
#define	DEFAULT_MIPI_CLK_RATE	19200000
//#define   ROUND(x,y)   ((x) / (y) + ((x) % (y) *10 / (y) >= 5 ? 1 : 0))


struct dsi_phy_seq_info dphy_seq_info[] = {
	{46,   62,    1,   7},
	{62,   93,    0,   7},
       {93,   125,  1,   6},
	{125,  187,  0,   6},
       {187,  250,  1,   5},
	{250,  375,  0,   5},
	{375,  500,  1,   4},
	{500,  750,  0,   4},
	{750,  1000, 1, 0},
	{1000, 1500, 0, 0}};

void get_dsi_phy_register(u32 *phy_freq, struct mipi_dsi_phy_register *phy_register)
{
	u32 ui = 0;
	u32 t_cfg_clk = 0;
	u32 seq_info_count = 0;
	u32 i = 0;
	u32 q_pll = 0;
	u32 m_pll = 0;
	u32 n_pll = 0;
	u32 r_pll =0;
	u32 m_n =0;
	u32 m_n_int =0;

    BUG_ON(phy_freq == NULL);
	BUG_ON(phy_register == NULL);

	t_cfg_clk = 1000 / (DEFAULT_MIPI_CLK_RATE / 1000000);

	// PLL parameters calculation
	seq_info_count = sizeof(dphy_seq_info)/ sizeof(struct dsi_phy_seq_info);
	for (i = 0; i < seq_info_count; i++) {
	    if (*phy_freq > dphy_seq_info[i].min_range && *phy_freq <= dphy_seq_info[i].max_range ) {
	        phy_register->rg_pll_vco_750M = dphy_seq_info[i].rg_pll_vco_750M;
	        phy_register->rg_hstx_ckg_sel = dphy_seq_info[i].rg_hstx_ckg_sel;
	        break;
	    }
	}

	switch(phy_register->rg_hstx_ckg_sel) {
		case 7:
			q_pll = 16;	break;
		case 6:
			q_pll = 8;	break;
		case 5:
			q_pll = 4;	break;
		case 4:
			q_pll = 2;	break;
		default:
			q_pll = 1;	break;
	}

	m_n_int = (*phy_freq) * q_pll * t_cfg_clk / 1000;
	m_n = (*phy_freq) * q_pll * t_cfg_clk % 1000 * 10 / 1000;
	if (m_n_int % 2 == 0) {
		if (m_n * 6 >= 50) {
			n_pll = 2;
			m_pll = (m_n_int + 1) * n_pll;
		}else if (m_n * 6 >= 30) {
			n_pll = 3;
			m_pll = m_n_int * n_pll + 2;
		}else {
			n_pll = 1;
			m_pll = m_n_int * n_pll;
		}
	}else {
		if (m_n * 6 >= 50) {
			n_pll = 1;
			m_pll = (m_n_int + 1) * n_pll;
		}else if (m_n * 6 >= 30) {
			n_pll = 1;
			m_pll = (m_n_int + 1) * n_pll;
		}else if (m_n * 6 >= 10) {
			n_pll = 3;
			m_pll = m_n_int * n_pll + 1;
		}else {
			n_pll = 2;
			m_pll = m_n_int * n_pll;
		}
	}




	if (n_pll == 1) {
		phy_register->rg_pll_fbd_p = 0;
		phy_register->rg_pll_pre_div1p = 1;
	}else {
		phy_register->rg_pll_fbd_p = n_pll;
		phy_register->rg_pll_pre_div1p = 0;
	}

	switch (phy_register->rg_pll_fbd_2p) {
		case 7:
			r_pll = 16;	break;
		case 6:
			r_pll = 8;		break;
		case 5:
			r_pll = 4;		break;
		case 4:
			r_pll = 2;		break;
		default:
			r_pll = 1;		break;
	}

	if (m_pll == 2) {
		phy_register->rg_pll_pre_p = 0;
		phy_register->rg_pll_fbd_s = 0;
		phy_register->rg_pll_fbd_div1f = 0;
		phy_register->rg_pll_fbd_div5f = 1;
	}else if (m_pll >= 2 * 2 * r_pll && m_pll <= 2 * 4 * r_pll) {
		phy_register->rg_pll_pre_p = m_pll / (2 * r_pll);
		phy_register->rg_pll_fbd_s = 0;
		phy_register->rg_pll_fbd_div1f = 1;
		phy_register->rg_pll_fbd_div5f = 0;
	}else if (m_pll >= 2 * 5 * r_pll && m_pll <= 2 * 150 * r_pll) {
        if (((m_pll / (2 * r_pll)) % 2) == 0) {
            phy_register->rg_pll_pre_p = (m_pll / (2 * r_pll)) / 2 - 1;
            phy_register->rg_pll_fbd_s = (m_pll / (2 * r_pll)) % 2 + 2;
        } else {
            phy_register->rg_pll_pre_p = (m_pll / (2 * r_pll)) / 2;
            phy_register->rg_pll_fbd_s = (m_pll / (2 * r_pll)) % 2;
        }
		phy_register->rg_pll_fbd_div1f = 0;
		phy_register->rg_pll_fbd_div5f = 0;
	}else {
		phy_register->rg_pll_pre_p = 0;
		phy_register->rg_pll_fbd_s = 0;
		phy_register->rg_pll_fbd_div1f = 0;
		phy_register->rg_pll_fbd_div5f = 1;
	}

	*phy_freq = 1000 * m_pll / (t_cfg_clk * n_pll * q_pll);
	ui = 1000 / (*phy_freq);

    if ( three_lane_flag == 1 )
	{
	    phy_register->clk_t_lpx = ROUND(50, 8 * 1000 / (*phy_freq));
		phy_register->clk_t_hs_prepare = ROUND(133, 16 * 1000 / (*phy_freq)) - 1;

		phy_register->clk_t_hs_zero = ROUND(262, 8 * 1000 / (*phy_freq));
		phy_register->clk_t_hs_trial = 2 * (ROUND(60, 8 * 1000 / (*phy_freq)) - 1) - 1;
		phy_register->clk_t_wakeup = ROUND(1000000,t_cfg_clk -1) > 0xFF ? 0xFF : ROUND(1000000,t_cfg_clk -1);
		phy_register->data_t_wakeup = phy_register->clk_t_wakeup;
		phy_register->data_t_lpx = phy_register->clk_t_lpx;
		phy_register->data_t_hs_prepare = ROUND(125 + 10 * 1000 / (*phy_freq), 16 * 1000 / (*phy_freq)) - 1;
		phy_register->data_t_hs_zero = ROUND(105 + 6 * 1000 / (*phy_freq), 8 * 1000 / (*phy_freq));
		phy_register->data_t_hs_trial = 2 * (ROUND(60 + 4 * 1000 / (*phy_freq), 8 * 1000 / (*phy_freq)) - 1) - 2;
		phy_register->data_t_ta_go = 3;
		phy_register->data_t_ta_get = 4;

		phy_register->rg_pll_enbwt = 1;
		phy_register->phy_clklp2hs_time = ROUND(407, 8 * 1000 / (*phy_freq)) + 12;
		phy_register->phy_clkhs2lp_time = ROUND(105 + 12 * 1000 / (*phy_freq), 8 * 1000 / (*phy_freq));
		phy_register->phy_lp2hs_time = ROUND(240 + 12 * 1000 / (*phy_freq), 8 * 1000 / (*phy_freq)) + 1;
		phy_register->phy_hs2lp_time = phy_register->phy_clkhs2lp_time;
		phy_register->clk_to_data_delay = 1+ phy_register->phy_clklp2hs_time;
		phy_register->data_to_clk_delay = ROUND(60 + 52 * 1000 / (*phy_freq), 8 * 1000 / (*phy_freq)) + phy_register->phy_clkhs2lp_time;
	}
    else
	{
		phy_register->clk_t_lpx = ROUND(50, 8 * ui);
		phy_register->clk_t_hs_prepare = ROUND(133, 16 * ui) - 1;

		phy_register->clk_t_hs_zero = ROUND(262, 8 * ui);
		phy_register->clk_t_hs_trial = 2 * (ROUND(60, 8 * ui) - 1);
		phy_register->clk_t_wakeup = ROUND(1000000,t_cfg_clk -1) > 0xFF ? 0xFF : ROUND(1000000,t_cfg_clk -1);
		phy_register->data_t_wakeup = phy_register->clk_t_wakeup;
		phy_register->data_t_lpx = phy_register->clk_t_lpx;
		phy_register->data_t_hs_prepare = ROUND(125 + 10 * ui, 16 * ui) - 1;
		phy_register->data_t_hs_zero = ROUND(105 + 6 * ui, 8 * ui);
		phy_register->data_t_hs_trial = 2 * (ROUND(60 + 4 * ui, 8 * ui) - 1);
		phy_register->data_t_ta_go = 3;
		phy_register->data_t_ta_get = 4;

		phy_register->rg_pll_enbwt = 1;
	    phy_register->phy_clklp2hs_time = ROUND(407, 8 * ui) + 12;
		phy_register->phy_clkhs2lp_time = ROUND(105 + 12 * ui, 8 * ui);
		phy_register->phy_lp2hs_time = ROUND(240 + 12 * ui, 8 * ui) + 1;
		phy_register->phy_hs2lp_time = phy_register->phy_clkhs2lp_time;
		phy_register->clk_to_data_delay = 1+ phy_register->phy_clklp2hs_time;
		phy_register->data_to_clk_delay = ROUND(60 + 52 * ui, 8 * ui) + phy_register->phy_clkhs2lp_time;
    }
	phy_register->lane_byte_clk = *phy_freq / 8;
	phy_register->clk_division = ((phy_register->lane_byte_clk % MAX_TX_ESC_CLK) > 0) ?
		(phy_register->lane_byte_clk / MAX_TX_ESC_CLK + 1) :
		(phy_register->lane_byte_clk / MAX_TX_ESC_CLK);

	phy_register->burst_mode = DSI_BURST_MODE;
	 balongfb_loge("*phy_freq=%d!!\n",*phy_freq);
}

void mipi_init(struct balong_fb_data_type *balongfd)
{
	MIPIDSI_PHY_STATUS_UNION    mipidsi_phy_status;
	u32 hline_time = 0;
	u32 hsa_time = 0;
	u32 hbp_time = 0;
	u32 pixel_clk = 0;
	u32 i = 0;
	bool is_ready = false;
	u32 delay_count = 0;
	struct mipi_dsi_phy_register phy_register = {0};

	BUG_ON(balongfd == NULL);

	get_dsi_phy_register(&(balongfd->panel_info.mipi.dphy_freq), &phy_register);
	set_MIPIDSI_PHY_IF_CFG_n_lanes(balongfd->panel_info.mipi.lane_nums);
    set_MIPIDSI_CLKMGR_CFG_tx_esc_clk_division( phy_register.clk_division );

	/* config TE */
	if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
        set_LDI_DSI_CMD_MOD_CTRL(0x1);
        set_LDI_DSI_TE_CTRL(0x4001);
        set_LDI_DSI_TE_HS_NUM(0x0);
        set_LDI_DSI_TE_HS_WD(0x8002);
        set_LDI_DSI_TE_VS_WD(0x1001);
	}
	if(balongfd->panel_info.type == PANEL_MIPI_VIDEO)
	{
		 /*enable Halt signal*/
	        set_LDI_DSI_CMD_MOD_CTRL(0x2);
	}

	/* begin */
	set_MIPIDSI_PHY_RSTZ( 0x00000000 );

	set_MIPIDSI_PHY_TST_CTRL0( 0x00000000);
    set_MIPIDSI_PHY_TST_CTRL0( 0x00000001);
	set_MIPIDSI_PHY_TST_CTRL0( 0x00000000);

    /* clock lane Timing control - TLPX */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10010);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1(phy_register.clk_t_lpx);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /* clock lane Timing control - THS-PREPARE */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10011);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1(phy_register.clk_t_hs_prepare);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /* clock lane Timing control - THS-ZERO */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10012);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1(phy_register.clk_t_hs_zero);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /* clock lane Timing control - THS-TRAIL */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10013);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1(phy_register.clk_t_hs_trial);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /* clock lane Timing control - TWAKEUP */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10014);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1(phy_register.clk_t_wakeup);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /* data lane */
    for (i = 0; i < 4; i++) {
	    /* Timing control - TLPX*/
	    set_MIPIDSI_PHY_TST_CTRL1( 0x10020 + (i << 4));
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);
	    set_MIPIDSI_PHY_TST_CTRL1(phy_register.data_t_lpx);
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);

	    /* Timing control - THS-PREPARE */
	    set_MIPIDSI_PHY_TST_CTRL1( 0x10021 + (i << 4));
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);
	    set_MIPIDSI_PHY_TST_CTRL1(phy_register.data_t_hs_prepare);
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);

	    /* Timing control - THS-ZERO */
	    set_MIPIDSI_PHY_TST_CTRL1( 0x10022 + (i << 4));
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);
	    set_MIPIDSI_PHY_TST_CTRL1(phy_register.data_t_hs_zero);
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);

	    /* Timing control - THS-TRAIL */
	    set_MIPIDSI_PHY_TST_CTRL1( 0x10023 + (i << 4));
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);
	    set_MIPIDSI_PHY_TST_CTRL1(phy_register.data_t_hs_trial);
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);

	    /* Timing control - TTA-GO */
	    set_MIPIDSI_PHY_TST_CTRL1( 0x10024 + (i << 4));
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);
	    set_MIPIDSI_PHY_TST_CTRL1(phy_register.data_t_ta_go);
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);

	    /* Timing control - TTA-GET */
	    set_MIPIDSI_PHY_TST_CTRL1( 0x10025 + (i << 4));
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);
	    set_MIPIDSI_PHY_TST_CTRL1(phy_register.data_t_ta_get);
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);

	    /*  Timing control - TWAKEUP */
	    set_MIPIDSI_PHY_TST_CTRL1( 0x10026 + (i << 4));
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);
	    set_MIPIDSI_PHY_TST_CTRL1(phy_register.data_t_wakeup);
	    set_MIPIDSI_PHY_TST_CTRL0(0x2);
	    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    }

    /* physical configuration I  */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10060);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1(phy_register.rg_hstx_ckg_sel);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /* physical configuration pll II  */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10063);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1((phy_register.rg_pll_fbd_div5f << 5) +
		(phy_register.rg_pll_fbd_div1f << 4) + (phy_register.rg_pll_fbd_2p << 1) +
		phy_register.rg_pll_enbwt);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /* physical configuration pll II  */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10064);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1(phy_register.rg_pll_fbd_p);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /* physical configuration pll III  */
    set_MIPIDSI_PHY_TST_CTRL1( 0x10065);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1((1 << 4) + phy_register.rg_pll_fbd_s);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /*physical configuration pll IV*/
    set_MIPIDSI_PHY_TST_CTRL1( 0x10066);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1((phy_register.rg_pll_pre_div1p << 7) + phy_register.rg_pll_pre_p);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    /*physical configuration pll V*/
    set_MIPIDSI_PHY_TST_CTRL1( 0x10067);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);
    set_MIPIDSI_PHY_TST_CTRL1((5 << 5) + (phy_register.rg_pll_vco_750M << 4) +
		(phy_register.rg_pll_lpf_rs << 2) + phy_register.rg_pll_lpf_cs);
    set_MIPIDSI_PHY_TST_CTRL0(0x2);
    set_MIPIDSI_PHY_TST_CTRL0(0x0);

    set_MIPIDSI_PHY_RSTZ( 0x00000004 );
    udelay(1);
    set_MIPIDSI_PHY_RSTZ( 0x00000005 );
    udelay(1);
    set_MIPIDSI_PHY_RSTZ( 0x00000007 );
    msleep(1);

	while (1) {
		mipidsi_phy_status.ul32 = get_MIPIDSI_PHY_STATUS();
		if ((0x1 == mipidsi_phy_status.bits.phy_lock) || delay_count > 100) {
			is_ready = (delay_count < 100) ? true : false;
			delay_count = 0;
			break;
		} else {
			udelay(1);
			++delay_count;
		}
	}

	if (!is_ready) {
		balongfb_logi("phylock is not ready.\n");
	}

	while (1) {
		mipidsi_phy_status.ul32 = get_MIPIDSI_PHY_STATUS();
		if ((0x1 == mipidsi_phy_status.bits.phy_stopstateclklane) || delay_count > 100) {
			is_ready = (delay_count < 100) ? true : false;
			break;
		} else {
			udelay(1);
			++delay_count;
		}
	}

	if (!is_ready) {
		balongfb_logi("phystopstateclklane is not ready.\n");
	}

	//msleep(10);

	/*--------------configuring the DPI packet transmission----------------*/
	/*
	** 1. Global configuration
	** Configure Register PHY_IF_CFG with the correct number of lanes
	** to be used by the controller.
	*/

    /*
	** 3. Configure the TX_ESC clock frequency to a frequency lower than 20 MHz
	** that is the maximum allowed frequency for D-PHY ESCAPE mode.
	*/

	/*
	** 2. Configure the DPI Interface:
	** This defines how the DPI interface interacts with the controller.
	*/
    set_MIPIDSI_DPI_VCID(balongfd->panel_info.mipi.vc);
    set_MIPIDSI_DPI_COLOR_CODING_dpi_color_coding(balongfd->panel_info.mipi.color_mode);
    set_MIPIDSI_DPI_CFG_POL_hsync_active_low(balongfd->panel_info.ldi.hsync_plr);
    set_MIPIDSI_DPI_CFG_POL_vsync_active_low(balongfd->panel_info.ldi.vsync_plr);
    set_MIPIDSI_DPI_CFG_POL_dataen_active_low(balongfd->panel_info.ldi.data_en_plr);
    set_MIPIDSI_DPI_CFG_POL_shutd_active_low(0);
    set_MIPIDSI_DPI_CFG_POL_colorm_active_low(0);
    if (balongfd->panel_info.bpp == ADE_OUT_RGB_666) {
        set_MIPIDSI_DPI_COLOR_CODING_loosely18_en(1);
    }

    /*
	** 4. Define the DPI Horizontal timing configuration:
	**
	** Hsa_time = HSA*(PCLK period/Clk Lane Byte Period);
	** Hbp_time = HBP*(PCLK period/Clk Lane Byte Period);
	** Hline_time = (HSA+HBP+HACT+HFP)*(PCLK period/Clk Lane Byte Period);
	*/
	pixel_clk = balongfd->panel_info.clk_rate / 1000000;
	hsa_time = balongfd->panel_info.ldi.h_pulse_width * phy_register.lane_byte_clk / pixel_clk;
	hbp_time = balongfd->panel_info.ldi.h_back_porch * phy_register.lane_byte_clk / pixel_clk;
    hline_time  = ( balongfd->panel_info.ldi.h_pulse_width
                    + balongfd->panel_info.ldi.h_back_porch
                    + balongfd->panel_info.xres
                    + balongfd->panel_info.ldi.h_front_porch)
                * phy_register.lane_byte_clk / pixel_clk;
    set_MIPIDSI_VID_HSA_TIME(hsa_time);
    set_MIPIDSI_VID_HBP_TIME(hbp_time);
    set_MIPIDSI_VID_HLINE_TIME(hline_time);

    	/*
	** 5. Define the Vertical line configuration:
	*/
    if (balongfd->panel_info.ldi.v_pulse_width > 15){
        balongfd->panel_info.ldi.v_pulse_width = 15;
    }
    set_MIPIDSI_VID_VSA_LINES(balongfd->panel_info.ldi.v_pulse_width);
    set_MIPIDSI_VID_VBP_LINES(balongfd->panel_info.ldi.v_back_porch);
    set_MIPIDSI_VID_VFP_LINES(balongfd->panel_info.ldi.v_front_porch);
    set_MIPIDSI_VID_VACTIVE_LINES(balongfd->panel_info.yres);
    set_MIPIDSI_VID_PKT_SIZE(balongfd->panel_info.xres);

	if (balongfd->panel_info.type == PANEL_MIPI_VIDEO) {
        set_MIPIDSI_VID_MODE_CFG_lp_vsa_en(1);
        set_MIPIDSI_VID_MODE_CFG_lp_vbp_en(1);
        set_MIPIDSI_VID_MODE_CFG_lp_vfp_en(1);
        set_MIPIDSI_VID_MODE_CFG_lp_vact_en(1);
        set_MIPIDSI_VID_MODE_CFG_lp_hbp_en(1);
        set_MIPIDSI_VID_MODE_CFG_lp_hfp_en(1);
	 /*VSA/VBP/VFP max transfer byte in LP mode*/
	 set_MIPIDSI_DPI_CFG_LP_TIM(0);
	 /*enable LP command transfer*/
	 set_MIPIDSI_VID_MODE_CFG_lp_cmd_en(1);
	 /*config max read time*/
 	 set_MIPIDSI_PHY_TMR_CFG_max_rd_time(0xFFFF);
	}

    /* Configure core's phy parameters */
    set_MIPIDSI_BTA_TO_CNT_bta_to_cnt(4095);
    set_MIPIDSI_CLKMGR_CFG_to_clk_dividsion(0x1);
    set_MIPIDSI_TO_CNT_CFG(0x7FF);
    set_MIPIDSI_PHY_TMR_CFG_phy_lp2hs_time(phy_register.phy_lp2hs_time);
    set_MIPIDSI_PHY_TMR_CFG_phy_hs2lp_time(phy_register.phy_hs2lp_time);
    set_MIPIDSI_PHY_TMR_LPCLK_CFG_phy_clklp2hs_time(phy_register.phy_clklp2hs_time);
    set_MIPIDSI_PHY_TMR_LPCLK_CFG_phy_clkhs2lp_time(phy_register.phy_clkhs2lp_time);
    set_MIPIDSI_PHY_TMR_clk_to_data_delay(phy_register.clk_to_data_delay);
    set_MIPIDSI_PHY_TMR_data_to_clk_delay(phy_register.data_to_clk_delay);
	/*
	** 3. Select the Video Transmission Mode:
	** This defines how the processor requires the video line to be
	** transported through the DSI link.
	*/
    set_MIPIDSI_VID_MODE_CFG_frame_bta_ack_en(0);
    set_MIPIDSI_VID_MODE_CFG_vid_mode_type(phy_register.burst_mode);

    //set_NO_CONTINUE_MOD_CTRL(0xa0f);
    set_MIPIDSI_LPCLK_CTRL_auto_clklane_ctrl(1);

	/* for dsi read */
    set_MIPIDSI_PCKHDL_CFG_bta_en(1);

    /* Enable EOTP TX; Enable EDPI, ALLOWED_CMD_SIZE = 720*/
	if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
	    /* differ from balong, no edpi_en reg for EDPI settings */
        set_MIPIDSI_EDPI_CMD_SIZE(balongfd->panel_info.xres);
        //set_MIPIDSI_PCKHDL_CFG_eotp_tx_en(1);
	}

	/*------------DSI and D-PHY Initialization-----------------*/
	/* 1. Waking up Core */
    set_MIPIDSI_PWR_UP_shutdownz(1);

	balongfb_loge("%s , exit success!\n", __func__);

}

int mipi_dsi_on(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;
	unsigned long timeout = jiffies;
	BUG_ON(pdev == NULL);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	/* set LCD init step before LCD on*/
	balongfd->panel_info.lcd_init_step = LCD_INIT_POWER_ON;
	ret = panel_next_on(pdev);

	/* mipi dphy clock enable */
	ret = clk_prepare_enable(balongfd->dsi_cfg_clk);
	if (ret != 0) {
		balongfb_loge("failed to enable dsi_cfg_clk, error=%d!\n", ret);
		return ret;
	}

	/* dsi pixel on */
	set_reg(balongfd->ade_base + LDI_HDMI_DSI_GT_REG, 0x0, 1, 0);
	/* mipi init */
	mipi_init(balongfd);

	/* modified for b052 bbit begin */
    /* switch to command mode */
    set_MIPIDSI_MODE_CFG(MIPIDSI_COMMAND_MODE);
    set_MIPIDSI_CMD_MODE_CFG_all_en_flag(1);

    /* 禁止向Clock Lane发起HS时钟传输请求 */
    set_MIPIDSI_LPCLK_CTRL_phy_txrequestclkhs(0);
	/* add for timeout print log */
	balongfb_loge("%s: dsi_on_time = %u,curfreq = %d\n",
			__func__,jiffies_to_msecs(jiffies-timeout),cpufreq_get_fb(0));
	timeout = jiffies;
    ret = panel_next_on(pdev);
    /* modified for b052 bbit begin */
	/* add for timeout print log */
	balongfb_loge("%s: panel_on_time = %u,curfreq = %d\n",
			__func__,jiffies_to_msecs(jiffies-timeout),cpufreq_get_fb(0));
    /* reset Core */
    set_MIPIDSI_PWR_UP_shutdownz(0);

	if (balongfd->panel_info.type == PANEL_MIPI_VIDEO) {
		/* switch to video mode */
        set_MIPIDSI_MODE_CFG(MIPIDSI_VIDEO_MODE);

#if ADE_DEBUG_LOG_ENABLE
        /* set to video lcd mode */
        g_panel_lcd_mode = 0;
#endif
	}

	if (balongfd->panel_info.type == PANEL_MIPI_CMD) {
        /* switch to cmd mode */
        set_MIPIDSI_CMD_MODE_CFG_all_en_flag(0);

#if ADE_DEBUG_LOG_ENABLE
        /* set to command lcd mode */
        g_panel_lcd_mode = 1;
#endif
	}

	/* enable generate High Speed clock */
    set_MIPIDSI_LPCLK_CTRL_phy_txrequestclkhs(1);
    /* Waking up Core */
    set_MIPIDSI_PWR_UP_shutdownz(1);
    /*set max packet size, 0x1 << 8 |0x37*/
    set_MIPIDSI_GEN_HDR(NULL, 0x137);
    lcd_pwr_status.lcd_dcm_pwr_status |= BIT(1);
    do_gettimeofday(&lcd_pwr_status.tvl_lcd_on);
    time_to_tm(lcd_pwr_status.tvl_lcd_on.tv_sec, 0, &lcd_pwr_status.tm_lcd_on);
	return ret;
}

int mipi_dsi_off(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;

	BUG_ON(pdev == NULL);

	balongfd = platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	ret = panel_next_off(pdev);

    /* reset Core */
    set_MIPIDSI_PWR_UP_shutdownz(0);

    /* switch to command mode */
    set_MIPIDSI_MODE_CFG(MIPIDSI_COMMAND_MODE);
    set_MIPIDSI_CMD_MODE_CFG_all_en_flag(1);

    /* disable generate High Speed clock */
    set_MIPIDSI_LPCLK_CTRL_phy_txrequestclkhs(0);

    /* Waking up Core */
    set_MIPIDSI_PWR_UP_shutdownz(1);

    /* shutdown d_phy */
    set_MIPIDSI_PHY_RSTZ(0);

	clk_disable_unprepare(balongfd->dsi_cfg_clk);

	return ret;
}

int mipi_dsi_remove(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;

	BUG_ON(pdev == NULL);
	balongfd = platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	balongfb_logi_display_debugfs("index=%d, enter!\n", balongfd->index);

	if (!IS_ERR(balongfd->dsi_cfg_clk)) {
		clk_put(balongfd->dsi_cfg_clk);
	}

	ret = panel_next_remove(pdev);

	balongfb_logi_display_debugfs("index=%d, exit!\n", balongfd->index);

	return ret;
}

int mipi_dsi_set_backlight(struct platform_device *pdev)
{
	return panel_next_set_backlight(pdev);
}

int mipi_dsi_set_timing(struct platform_device *pdev)
{
	int ret = 0;
	struct balong_fb_data_type *balongfd = NULL;
	u32 hline_time = 0;
	u32 hsa_time = 0;
	u32 hbp_time = 0;
	u32 pixel_clk = 0;
	struct mipi_dsi_phy_register phy_ctrl = {0};

	BUG_ON(pdev == NULL);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	get_dsi_phy_register(&(balongfd->panel_info.mipi.dphy_freq), &phy_ctrl);

    set_MIPIDSI_VID_PKT_SIZE( balongfd->panel_info.xres );

	pixel_clk = balongfd->panel_info.clk_rate / 1000000;
	hsa_time = balongfd->panel_info.ldi.h_pulse_width * phy_ctrl.lane_byte_clk / pixel_clk;
	hbp_time = balongfd->panel_info.ldi.h_back_porch * phy_ctrl.lane_byte_clk / pixel_clk;
	hline_time = (balongfd->panel_info.ldi.h_pulse_width + balongfd->panel_info.ldi.h_back_porch +
		balongfd->panel_info.xres + balongfd->panel_info.ldi.h_front_porch) *
		phy_ctrl.lane_byte_clk / pixel_clk;
    set_MIPIDSI_VID_HSA_TIME( hsa_time );
    set_MIPIDSI_VID_HBP_TIME( hbp_time );
    set_MIPIDSI_VID_HLINE_TIME( hline_time );

	if (balongfd->panel_info.ldi.v_pulse_width > 15)
		balongfd->panel_info.ldi.v_pulse_width = 15;
    set_MIPIDSI_VID_VSA_LINES( balongfd->panel_info.ldi.v_pulse_width );
    set_MIPIDSI_VID_VBP_LINES( balongfd->panel_info.ldi.v_back_porch );
    set_MIPIDSI_VID_VFP_LINES( balongfd->panel_info.ldi.v_front_porch );
    set_MIPIDSI_VID_VACTIVE_LINES( balongfd->panel_info.yres );

	ret = panel_next_set_timing(pdev);

	return ret;
}

STATIC int mipi_dsi_set_frc(struct platform_device *pdev, int target_fps)
{
	struct balong_fb_data_type *balongfd = NULL;
	u32 hline_time = 0;
	u32 pixel_clk = 0;
	u32 vertical_timing = 0;
	u32 horizontal_timing = 0;
	u32 h_front_porch = 0;
	u32 h_back_porch = 0;
	int ret = 0;

	BUG_ON(pdev == NULL);
	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	/* calculate new HFP based on target_fps */
    vertical_timing     = balongfd->panel_info.yres
                        + balongfd->panel_info.ldi.v_back_porch
                        + balongfd->panel_info.ldi.v_front_porch
                        + balongfd->panel_info.ldi.v_pulse_width;
	horizontal_timing = balongfd->panel_info.clk_rate / (vertical_timing * target_fps);

	/* new HFP*/
	/*
	balongfd->panel_info.ldi.h_front_porch = horizontal_timing - balongfd->panel_info.xres
		-balongfd->panel_info.ldi.h_back_porch - balongfd->panel_info.ldi.h_pulse_width;
	*/
    h_front_porch       = horizontal_timing
                        - balongfd->panel_info.xres
                        - balongfd->panel_info.ldi.h_back_porch
                        - balongfd->panel_info.ldi.h_pulse_width;
    h_back_porch        = balongfd->panel_info.ldi.h_back_porch;

	pixel_clk = balongfd->panel_info.clk_rate / 1000000;

    /*update hline_time*/
    hline_time          = ( balongfd->panel_info.ldi.h_pulse_width
                            + balongfd->panel_info.ldi.h_back_porch
                            + balongfd->panel_info.xres
                            + h_front_porch)
                        * (balongfd->panel_info.mipi.dphy_freq / 8) / pixel_clk;

    /* remember current fps*/
	balongfd->panel_info.frame_rate = target_fps;

    /* Reset DSI core */
    set_MIPIDSI_PWR_UP( 0 );

    set_MIPIDSI_VID_HLINE_TIME( hline_time );

	set_LDI_HRZ_CTRL0(balongfd->ade_base, h_front_porch, h_back_porch);

    /* power on DSI core */
    set_MIPIDSI_PWR_UP( 1 );

	return ret;
}

STATIC int mipi_dsi_check_esd(struct platform_device *pdev)
{
	BUG_ON(pdev == NULL);
	return panel_next_check_esd(pdev);
}

#if 0
static int mipi_dsi_check_mipi_tr(struct platform_device* pdev)
{
    BUG_ON(pdev == NULL);
    return panel_next_check_mipi_tr(pdev);
}

static int mipi_dsi_set_inversion_type(struct platform_device* pdev, unsigned int inversion_mode)
{
    BUG_ON(pdev == NULL);
    return panel_next_set_inversion_type(pdev, inversion_mode);
}
#endif

int mipi_dsi_probe(struct platform_device *pdev)
{
	struct balong_fb_data_type *balongfd = NULL;
	struct platform_device *ldi_dev = NULL;
	struct balong_fb_panel_data *pdata = NULL;
	int ret = 0;

    pr_info("%s enter succ!\n",__func__);
	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

    /* get dsi addr space start address:convert from phyical addr to virtual addr */
    balongfd->dsi_base      = balongfd_reg_base_dsi0;

	/* alloc ldi device */
	ldi_dev = platform_device_alloc(DEV_NAME_LDI, pdev->id);
	if (!ldi_dev) {
		balongfb_loge("ldi platform_device_alloc failed!\n");
		return -ENOMEM;
	}

	/* link to the latest pdev */
	balongfd->pdev = ldi_dev;

	/* alloc panel device data */
	if (platform_device_add_data(ldi_dev, pdev->dev.platform_data,
		sizeof(struct balong_fb_panel_data))) {
		balongfb_loge("failed to platform_device_add_data, error=%d!\n", ret);
		platform_device_put(ldi_dev);
		return -ENOMEM;
	}

	/* data chain */
	pdata = (struct balong_fb_panel_data *)ldi_dev->dev.platform_data;
	pdata->on = mipi_dsi_on;
	pdata->off = mipi_dsi_off;
	pdata->remove = mipi_dsi_remove;
	pdata->set_backlight = mipi_dsi_set_backlight;
	pdata->set_timing = mipi_dsi_set_timing;
	pdata->set_frc = mipi_dsi_set_frc;
	pdata->check_esd = mipi_dsi_check_esd;
	pdata->next = pdev;

	/* get/set panel info */
	memcpy(&balongfd->panel_info, pdata->panel_info, sizeof(struct balong_panel_info));

	/* set driver data */
	platform_set_drvdata(ldi_dev, balongfd);
	/* register in ldi driver */
	ret = platform_device_add(ldi_dev);
	if (ret) {
		balongfb_loge("failed to platform_device_add, error=%d!\n", ret);
		platform_device_put(ldi_dev);
		return ret;
	}
    pr_info("%s exit succ!\n",__func__);
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

int __init mipi_dsi_driver_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		balongfb_loge("not able to register the driver, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(mipi_dsi_driver_init);

#if ADE_DEBUG_LOG_ENABLE
void debug_lcd_parse_cmd_str(char ** str)
{
    char *cmd_seq = NULL;
    int  cmd_seq_num = 0;

    balongfb_logi("enter succ ! \n");
    memset(&g_mipi_lcd_debug_info, 0, sizeof(g_mipi_lcd_debug_info));

    while (NULL != (cmd_seq = strsep(str, " \\"))) {
        balongfb_logi("cmd_seq: %s, cmd_seq_num: %d \n", cmd_seq, cmd_seq_num);
        if (0 == cmd_seq_num) {
            if (strnicmp(cmd_seq, "GW", 2) == 0) {
                g_mipi_lcd_debug_info.cmd_type = DTYPE_GEN_WRITE;
                g_mipi_lcd_debug_info.ops_type = LCD_DEBUG_OPS_TYPE_WRITE;
            } else if (strnicmp(cmd_seq, "GR", 2) == 0) {
                g_mipi_lcd_debug_info.cmd_type = DTYPE_GEN_READ1;
                g_mipi_lcd_debug_info.ops_type = LCD_DEBUG_OPS_TYPE_READ;
            } else if (strnicmp(cmd_seq, "DW", 2) == 0) {
                g_mipi_lcd_debug_info.cmd_type = DTYPE_DCS_WRITE;
                g_mipi_lcd_debug_info.ops_type = LCD_DEBUG_OPS_TYPE_WRITE;
            } else if (strnicmp(cmd_seq, "DR", 2) == 0) {
                g_mipi_lcd_debug_info.cmd_type = DTYPE_DCS_READ;
                g_mipi_lcd_debug_info.ops_type = LCD_DEBUG_OPS_TYPE_READ;
            }
        } else if (cmd_seq_num == 1) {
            if (strnicmp(cmd_seq, "-1", 2) == 0) {
                g_mipi_lcd_debug_info.lcd_reg_msb = -1;
            } else {
                if( strict_strtol(cmd_seq, 0, &g_mipi_lcd_debug_info.lcd_reg_msb) < 0)
                {
                    balongfb_loge("strict_strtol lcd_reg_msb fail\n");
                }
            }
        } else if (cmd_seq_num == 2) {
            if( strict_strtol(cmd_seq, 0, &g_mipi_lcd_debug_info.lcd_reg_lsb) < 0)
            {
		balongfb_loge("strict_strtol lcd_reg_lsb fail\n");
            }
        } else {
            if (g_mipi_lcd_debug_info.ops_type == LCD_DEBUG_OPS_TYPE_READ) {
                if( strict_strtol(cmd_seq, 0, &g_mipi_lcd_debug_info.para_num) < 0)
	        {
                    balongfb_loge("strict_strtol para_num fail\n");
	        }
                break;
            } else {
                if (g_mipi_lcd_debug_info.para_num >= DEBUG_LCD_REG_PARA_MAX_NUM) {
                    g_mipi_lcd_debug_info.para_num = DEBUG_LCD_REG_PARA_MAX_NUM;
                    break;
                }

                if (0 == strict_strtol(cmd_seq, 0, &g_mipi_lcd_debug_info.para_array[g_mipi_lcd_debug_info.para_num])) {
                    g_mipi_lcd_debug_info.para_num++;
                    balongfb_logi("g_mipi_lcd_debug_info.para_array[%ld] = %ld \n ",g_mipi_lcd_debug_info.para_num - 1,
                                                                          g_mipi_lcd_debug_info.para_array[g_mipi_lcd_debug_info.para_num - 1] );
                }
            }
        }

        cmd_seq_num++;
    }

    if (DTYPE_GEN_WRITE == g_mipi_lcd_debug_info.cmd_type) {
        if (1 == g_mipi_lcd_debug_info.para_num) {
            g_mipi_lcd_debug_info.cmd_type = DTYPE_GEN_WRITE1;
        } else if (2 == g_mipi_lcd_debug_info.para_num) {
            g_mipi_lcd_debug_info.cmd_type = DTYPE_GEN_WRITE2;
        } else if (2 < g_mipi_lcd_debug_info.para_num) {
            g_mipi_lcd_debug_info.cmd_type = DTYPE_GEN_LWRITE;
        } else {
            balongfb_loge("generic write must have para! para_num = %ld \n", g_mipi_lcd_debug_info.para_num);
        }
    }

    if (DTYPE_DCS_WRITE == g_mipi_lcd_debug_info.cmd_type) {
        if (1 == g_mipi_lcd_debug_info.para_num) {
            g_mipi_lcd_debug_info.cmd_type = DTYPE_DCS_WRITE1;
        } else if (1 < g_mipi_lcd_debug_info.para_num) {
            g_mipi_lcd_debug_info.cmd_type = DTYPE_DCS_LWRITE;
        } else {
           /* do nothing */
        }
    }

    if (LCD_DEBUG_OPS_TYPE_READ == g_mipi_lcd_debug_info.ops_type) {
        if (0 == g_mipi_lcd_debug_info.para_num) {
            g_mipi_lcd_debug_info.para_num = 1;
        }

        if (g_mipi_lcd_debug_info.para_num > DEBUG_LCD_REG_PARA_MAX_NUM) {
            g_mipi_lcd_debug_info.para_num = DEBUG_LCD_REG_PARA_MAX_NUM;
        }


        if ((g_mipi_lcd_debug_info.para_num > 1)
         && (g_mipi_lcd_debug_info.cmd_type == DTYPE_GEN_READ1)) {
            g_mipi_lcd_debug_info.cmd_type = DTYPE_GEN_READ2;
        }
    }

    balongfb_logi("g_mipi_lcd_debug_info.cmd_type = 0x%x \n"
              " g_mipi_lcd_debug_info.lcd_reg_msb  = %ld \n "
              " g_mipi_lcd_debug_info.lcd_reg_lsb  = %ld \n "
              " g_mipi_lcd_debug_info.ops_type = %d (0:write,1:read) \n"
              " g_mipi_lcd_debug_info.para_num = %ld \n", g_mipi_lcd_debug_info.cmd_type,
                                                        g_mipi_lcd_debug_info.lcd_reg_msb,
                                                        g_mipi_lcd_debug_info.lcd_reg_lsb,
                                                        g_mipi_lcd_debug_info.ops_type,
                                                        g_mipi_lcd_debug_info.para_num);

    return;
}

void debug_lcd_write_lcd_reg(void)
{
    struct dsi_cmd_desc cmd_set;
    char*   cmd_payload = NULL;
    int     i = 0;
    int     start_idx = 0;

    cmd_set.dtype = g_mipi_lcd_debug_info.cmd_type;
    cmd_set.vc    = 0;
    cmd_set.wait  = 0;
    cmd_set.waittype = WAIT_TYPE_MS;
    cmd_set.payload  = NULL;

    /* lcd reg num, 1 or 2 byte */
    cmd_set.dlen = 1;
    if (-1 != g_mipi_lcd_debug_info.lcd_reg_msb) {
        cmd_set.dlen = 2;
    }

    cmd_set.dlen += g_mipi_lcd_debug_info.para_num;

    cmd_payload = (char*)kmalloc(cmd_set.dlen, GFP_KERNEL);

    if (cmd_payload == NULL) {
        balongfb_loge("cmd_payload malloc is fail ! \n");
        return;
    }

    cmd_payload[0] = g_mipi_lcd_debug_info.lcd_reg_lsb;

     start_idx = 1;
    if (-1 != g_mipi_lcd_debug_info.lcd_reg_msb) {
        start_idx = 2;
        cmd_payload[1] = g_mipi_lcd_debug_info.lcd_reg_msb;
    }

    for (i = 0; i < g_mipi_lcd_debug_info.para_num; i++) {
        cmd_payload[start_idx + i] = g_mipi_lcd_debug_info.para_array[i];
    }

    cmd_set.payload = cmd_payload;

    mipi_dsi_cmds_tx(&cmd_set, 1, NULL);

    balongfb_logi("cmd_set.dtype = 0x%x \n cmd_set.dlen  = %d \n", cmd_set.dtype, cmd_set.dlen);

    for (i = 0; i < cmd_set.dlen; i++) {
        balongfb_logi("cmd_set.payload[%d] = 0x%x \n", i, cmd_set.payload[i]);
    }

    kfree(cmd_payload);
}

void debug_lcd_read_lcd_reg(void)
{
    struct dsi_cmd_desc packet_size_cmd_set;
    volatile int cmd_reg;
    int     max_times;

    int i = 0;
    int reg_msb = g_mipi_lcd_debug_info.lcd_reg_msb;

    if (-1 == g_mipi_lcd_debug_info.lcd_reg_msb) {
        reg_msb = 0;
    }

    max_times = g_mipi_lcd_debug_info.para_num / 4 + 1;

    balongfb_logi("g_mipi_lcd_debug_info.para_num = %ld \n", g_mipi_lcd_debug_info.para_num);

    packet_size_cmd_set.dtype = DTYPE_MAX_PKTSIZE;
    packet_size_cmd_set.vc    = 0;

    /* video LCD must set the DSI to Command Mode */
    if (g_panel_lcd_mode == 0) {
        set_dsi_mode(CMD_MODE);
        mdelay(100);
    }

    packet_size_cmd_set.dlen = g_mipi_lcd_debug_info.para_num;

    mipi_dsi_max_return_packet_size(&packet_size_cmd_set, NULL);

    mdelay(50);

    cmd_reg = (reg_msb << 8 | g_mipi_lcd_debug_info.lcd_reg_lsb) << 8 | g_mipi_lcd_debug_info.cmd_type;

    /* send cmd */
    set_MIPIDSI_GEN_HDR(NULL, cmd_reg);

    for (i = 0; i < max_times; i++) {
        mdelay(50);

        g_mipi_lcd_debug_info.para_array[i] = get_MIPIDSI_GEN_PLD_DATA();
        balongfb_logi("get_MIPIDSI_GEN_PLD_DATA cmd_reg = 0x%x, lcd_reg_val = %ld \n", cmd_reg, g_mipi_lcd_debug_info.para_array[i]);
    }

    if (g_panel_lcd_mode == 0) {
        set_dsi_mode(HSPD_MODE);
        mdelay(20);
    }

    return;
}
void debug_lcd_send_cmd_set(void)
{

    if (LCD_DEBUG_OPS_TYPE_WRITE == g_mipi_lcd_debug_info.ops_type) {
        debug_lcd_write_lcd_reg();
    }


    if (LCD_DEBUG_OPS_TYPE_READ == g_mipi_lcd_debug_info.ops_type) {
        debug_lcd_read_lcd_reg();
    }

    return;
}

void debug_lcd_build_cmd_to_str(char * buf)
{
    int buf_para_start;
    int i = 0;
    int read_times;

    if (buf == NULL) {
        balongfb_loge("buf is null ! \n");
        return;
    }

    /* Generic or Dcs */
    buf[0] = 'G';
    if ((DTYPE_DCS_WRITE  == g_mipi_lcd_debug_info.cmd_type)
     || (DTYPE_DCS_WRITE1 == g_mipi_lcd_debug_info.cmd_type)
     || (DTYPE_DCS_READ   == g_mipi_lcd_debug_info.cmd_type)
     || (DTYPE_DCS_LWRITE == g_mipi_lcd_debug_info.cmd_type)) {
        buf[0] = 'D';
    }

    /* Write or Read */
    buf[1] = 'R';
    if (LCD_DEBUG_OPS_TYPE_WRITE == g_mipi_lcd_debug_info.ops_type) {
        buf[1] = 'W';
    }

    buf[2] = ' ';

    /* -1 or MSB of LCD_REG */
    if (-1 == g_mipi_lcd_debug_info.lcd_reg_msb) {
        sprintf(buf + 3, "-1 ");
        buf_para_start = 6;
    } else {
        sprintf(buf + 3, "%ld ", g_mipi_lcd_debug_info.lcd_reg_msb);
        buf_para_start = 8;
    }

    /* LSB of LCD_REG */
    sprintf(buf + buf_para_start, "%ld ", g_mipi_lcd_debug_info.lcd_reg_lsb);

    buf_para_start += 5;

    /* Para array for write LCD */
    if (LCD_DEBUG_OPS_TYPE_WRITE == g_mipi_lcd_debug_info.ops_type) {

        if (g_mipi_lcd_debug_info.para_num > 0) {
            /* Para_num */
            sprintf(buf + buf_para_start, "%ld ", g_mipi_lcd_debug_info.para_num);
        }

        for (i = 0; i < g_mipi_lcd_debug_info.para_num; i++) {
            buf_para_start +=  5;

            if ((buf_para_start + 5) > DEBUG_LCD_CMD_STR_MAX_BYTE) {
                balongfb_loge(" buf_para_start + 5 is overflow, buf_para_start + 5:%d", buf_para_start + 5);
                return;
            }

            sprintf(buf + buf_para_start, "%ld ", g_mipi_lcd_debug_info.para_array[i]);
        }
    } else {

        read_times = g_mipi_lcd_debug_info.para_num /4 + 1;

        /* Para_num */
        sprintf(buf + buf_para_start, "%ld\n", g_mipi_lcd_debug_info.para_num);

        buf_para_start += 5;

        for (i = 0; i < read_times; i++) {
            if ((buf_para_start + 11) > DEBUG_LCD_CMD_STR_MAX_BYTE) {
                balongfb_loge(" buf_para_start + 11 is overflow, buf_para_start + 11 = %d:", buf_para_start + 11);
                return;
            }

            sprintf(buf + buf_para_start, "%ld \n", g_mipi_lcd_debug_info.para_array[i]);

            buf_para_start += 11;
        }
    }


    if ((buf_para_start) < DEBUG_LCD_CMD_STR_MAX_BYTE) {
        buf[buf_para_start] = '\0';
    }

    balongfb_logi("buf is %s, buf_para_start = %d \n", buf, buf_para_start);
    return;

}

ssize_t debug_lcd_read_file(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    char buf[DEBUG_LCD_CMD_STR_MAX_BYTE];
    int    len = 0;

    if (g_mipi_lcd_debug_info.cmd_type < DTYPE_GEN_WRITE) {
        balongfb_loge("no cmd was sent to lcd! g_mipi_lcd_debug_info.cmd_type = %d \n", g_mipi_lcd_debug_info.cmd_type);

        if (copy_to_user(user_buf, "\0", 1)) {
            balongfb_loge("copy_to_user fail! \n");
            return -EFAULT;
        }

        return 0;
    }

    debug_lcd_build_cmd_to_str(buf);

    len = strlen(buf) + 1;

    balongfb_logi("buf is %s, len = %d \n", buf, len);

    return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

ssize_t debug_lcd_write_file(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    char cmd_buf[DEBUG_LCD_CMD_STR_MAX_BYTE] = {0};
    char *cmd_buf_str;


    balongfb_logi("enter succ! \n");

    if (cnt > DEBUG_LCD_CMD_STR_MAX_BYTE - 1) {
        balongfb_loge("input buffer cnt > cmd_buf_Len, cmd_buf_len:%d \n",  DEBUG_LCD_CMD_STR_MAX_BYTE);
        return -EINVAL;
    }

    if (copy_from_user(&cmd_buf, ubuf, cnt)) {
        balongfb_loge("copy_from_user fail! \n");
        return -EFAULT;
    }

    cmd_buf[cnt] = 0;

    balongfb_logi(" cmd_buf: %s, len= %zu \n", cmd_buf, sizeof(struct lcd_debug_cmd));

    cmd_buf_str = cmd_buf;

    debug_lcd_parse_cmd_str(&cmd_buf_str);

    debug_lcd_send_cmd_set();

    return cnt;
}

static const struct file_operations debug_lcd_cmd_fops = {
    .read    = debug_lcd_read_file,
    .write   = debug_lcd_write_file,
};

int __init mipi_lcd_debug_init(void)
{
    struct dentry *lcd_debugfs_dir = NULL;

    balongfb_logi("enter succ! \n");

    lcd_debugfs_dir = debugfs_create_dir(LCD_DEBUG_DIR_NAME, NULL);

    if (ERR_PTR(-ENODEV) == lcd_debugfs_dir) {
        balongfb_loge("debugfs_create_dir create dir %s fail! \n ", LCD_DEBUG_DIR_NAME);
        return -EINVAL;
    }

    debugfs_create_file(LCD_DEBUG_FILE_NAME, 0664, lcd_debugfs_dir, NULL, &debug_lcd_cmd_fops);

    return 0;
}

late_initcall(mipi_lcd_debug_init);

#endif




