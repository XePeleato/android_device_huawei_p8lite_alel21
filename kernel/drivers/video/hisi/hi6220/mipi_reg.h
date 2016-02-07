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

#ifndef MIPI_REG_H
#define MIPI_REG_H

#include "balong_ade.h"
#include "balong_fb.h"
#include "balong_fb_def.h"

#define ADE_MIPI_DSI_OFFSET         (0x7800)

/* MIPIDSI_OFFSET_2 based on EDC: to be fixed */
#define MIPIDSI_VERSION_ADDR                ( 0x0 )        /* Version of the DSI host controller */
#define MIPIDSI_PWR_UP_ADDR                 ( 0x4 )        /* Core power-up */
#define MIPIDSI_CLKMGR_CFG_ADDR             ( 0x8 )        /* Configuration of the internal clock dividers */
#define MIPIDSI_DPI_VCID_ADDR               ( 0xC )        /* DPI virtual channel id */
#define MIPIDSI_DPI_COLOR_CODING_ADDR       ( 0x10 )       /* DPI color coding */
#define MIPIDSI_DPI_CFG_POL_ADDR            ( 0x14 )       /* DPI polarity configuration */
#define MIPIDSI_DPI_LP_CMD_TIM_ADDR         ( 0x18 )       /* Low-power command timing configuration */
#define MIPIDSI_DBI_VCID_ADDR               ( 0x1C )       /* DBI virtual channel id */
#define MIPIDSI_DBI_CFG_ADDR                ( 0x20 )       /* DBI interface configuration */
#define MIPIDSI_DBI_PARTITIONING_EN_ADDR    ( 0x24 )       /* DBI parititoning enable */
#define MIPIDSI_DBI_CMDSIZE_ADDR            ( 0x28 )       /* DBI command size configuration */
#define MIPIDSI_PCKHDL_CFG_ADDR             ( 0x2C )       /* Packet handler configuration */
#define MIPIDSI_GEN_VCID_ADDR               ( 0x30 )       /* Generic interface virtual channel id */
#define MIPIDSI_MODE_CFG_ADDR               ( 0x34 )       /* Video or Command mode selection */
#define MIPIDSI_VID_MODE_CFG_ADDR           ( 0x38 )       /* Video mode configuration */
#define MIPIDSI_VID_PKT_SIZE_ADDR           ( 0x3C )       /* Video packet size */
#define MIPIDSI_VID_NUM_CHUNKS_ADDR         ( 0x40 )       /* Number of chunks */
#define MIPIDSI_VID_NULL_SIZE_ADDR          ( 0x44 )       /* Null packet size */
#define MIPIDSI_VID_HSA_TIME_ADDR           ( 0x48 )       /* Horizontal Sync Active time */
#define MIPIDSI_VID_HBP_TIME_ADDR           ( 0x4C )       /* Horizontal Back Porch time */
#define MIPIDSI_VID_HLINE_TIME_ADDR         ( 0x50 )       /* Line time */
#define MIPIDSI_VID_VSA_LINES_ADDR          ( 0x54 )       /* Vertical Sync Active period */
#define MIPIDSI_VID_VBP_LINES_ADDR          ( 0x58 )       /* Vertical Back Porch period */
#define MIPIDSI_VID_VFP_LINES_ADDR          ( 0x5C )       /* Vertical Front Porch period */
#define MIPIDSI_VID_VACTIVE_LINES_ADDR      ( 0x60 )       /* Vertical resolution */
#define MIPIDSI_EDPI_CMD_SIZE_ADDR          ( 0x64 )       /* Size for eDPI packets */
#define MIPIDSI_CMD_MODE_CFG_ADDR           ( 0x68 )       /* Command mode configuration */
#define MIPIDSI_GEN_HDR_ADDR                ( 0x6C )       /* Generic packet header configuration */
#define MIPIDSI_GEN_PLD_DATA_ADDR           ( 0x70 )       /* Generic payload data in and out */
#define MIPIDSI_CMD_PKT_STATUS_ADDR         ( 0x74 )       /* Command packet status */
#define MIPIDSI_TO_CNT_CFG_ADDR             ( 0x78 )       /* Timeout timers configuration */
#define MIPIDSI_HS_RD_TO_CNT_ADDR           ( 0x7C )       /* Peripheral response timeout definition after high-speed read operations */
#define MIPIDSI_LP_RD_TO_CNT_ADDR           ( 0x80 )       /* Peripheral response timeout definition after low-power read operations */
#define MIPIDSI_HS_WR_TO_CNT_ADDR           ( 0x84 )       /* Peripheral response timeout definition after high-speed write operations */
#define MIPIDSI_LP_WR_TO_CNT_ADDR           ( 0x88 )       /* Peripheral response timeout definition after low-power write operations */
#define MIPIDSI_BTA_TO_CNT_ADDR             ( 0x8C )       /* Peripheral response timeout definition after bus turnaround */
#define MIPIDSI_SDF_3D_ADDR                 ( 0x90 )       /* 3D control configuration */
#define MIPIDSI_LPCLK_CTRL_ADDR             ( 0x94 )       /* Low-power in clock lane */
#define MIPIDSI_PHY_TMR_LPCLK_CFG_ADDR      ( 0x98 )       /* D-PHY timing configuration for the clock lane */
#define MIPIDSI_PHY_TMR_CFG_ADDR            ( 0x9C )       /* D-PHY timing configuration for data lanes */
#define MIPIDSI_PHY_RSTZ_ADDR               ( 0xA0 )       /* D-PHY reset control */
#define MIPIDSI_PHY_IF_CFG_ADDR             ( 0xA4 )       /* D-PHY interface configuration */
#define MIPIDSI_PHY_ULPS_CTRL_ADDR          ( 0xA8 )       /* D-PHY Ultra Low-Power control */
#define MIPIDSI_PHY_TX_TRIGGERS_ADDR        ( 0xAC )       /* D-PHY transmit triggers */
#define MIPIDSI_PHY_STATUS_ADDR             ( 0xB0 )       /* D-PHY PPI status interface */
#define MIPIDSI_PHY_TST_CTRL0_ADDR          ( 0xB4 )       /* D-PHY test interface control 0 */
#define MIPIDSI_PHY_TST_CTRL1_ADDR          ( 0xB8 )       /* D-PHY test interface control 1 */
#define MIPIDSI_INT_ST0_ADDR                ( 0xBC )       /* interrupt status register 0 */
#define MIPIDSI_INT_ST1_ADDR                ( 0xC0 )       /* interrupt status register 1 */
#define MIPIDSI_INT_MSK0_ADDR               ( 0xC4 )       /* Masks the interrupt generation triggered by the INT_ST0 register */
#define MIPIDSI_INT_MSK1_ADDR               ( 0xC8 )       /* Masks the interrupt generation triggered by the INT_ST1 register */
#define MIPIDSI_NO_CONTINUE_ADDR      	    ( 0xCC )

#define MIPIDSI_COMMAND_MODE                ( 1 )
#define MIPIDSI_VIDEO_MODE                  ( 0 )

#define get_dsi_virtual_addr()              ( balongfd_reg_base_dsi0 )
#define get_MIPIDSI_PHY_STATUS()            (inp32(get_dsi_virtual_addr() + MIPIDSI_PHY_STATUS_ADDR))
#define get_MIPIDSI_CMD_PKT_STATUS()        (inp32(get_dsi_virtual_addr() + MIPIDSI_CMD_PKT_STATUS_ADDR))
#define get_MIPIDSI_GEN_PLD_DATA()          (inp32(get_dsi_virtual_addr() + MIPIDSI_GEN_PLD_DATA_ADDR))

#define set_NO_CONTINUE_MOD_CTRL( nVal )    { outp32(get_dsi_virtual_addr() + MIPIDSI_NO_CONTINUE_ADDR, nVal); }
/*****************************************************************************
    MIPI DSI register union
*****************************************************************************/

/* Version of the DSI host controller */
typedef union {
    struct {
        u32         version :32;
    } bits;
    u32             ul32;
} MIPIDSI_VERSION_UNION;

/* Core power-up */
typedef union {
    struct {
        u32         shutdownz   : 1;                 /*0:reset, 1:dis reset*/
        u32         reserved0   : 31;
    } bits;
    u32             ul32;
} MIPIDSI_PWR_UP_UNION;


typedef union {
    struct {
        u32         tx_esc_clk_division : 8;
        u32         to_clk_dividsion    : 8;
        u32         reserved0           : 16;
    } bits;
    u32             ul32;
} MIPIDSI_CLKMGR_CFG_UNION;
typedef union {
    struct {
        u32          dpi_vcid   : 2;
        u32          reserved0  : 30;
    } bits;
    u32             ul32;
} MIPIDSI_DPI_VCID_UNION;
typedef union {
    struct {
        u32         dpi_color_coding    : 4;
        u32         reserved0           : 4;
        u32         loosely18_en        : 1;
        u32         reserved1           : 23;
    } bits;
    u32             ul32;
} MIPIDSI_DPI_COLOR_CODING_UNION;
typedef union {
    struct {
        u32         dataen_active_low   : 1;
        u32         vsync_active_low    : 1;
        u32         hsync_active_low    : 1;
        u32         shutd_active_low    : 1;
        u32         colorm_active_low   : 1;
        u32         reserved0           : 27;
    } bits;
    u32             ul32;
} MIPIDSI_DPI_CFG_POL_UNION;
typedef union {
    struct {
        u32         invact_lpcmd_time   : 8;
        u32         reserved0           : 8;
        u32         outvact_lpcmd_time  : 8;
        u32         reserved1           : 8;
    } bits;
    u32             ul32;
} MIPIDSI_DPI_LP_CMD_TIM_UNION;
typedef union {
    struct {
        u32         dbi_vcid    : 2;
        u32         reserved0   : 30;
    } bits;
    u32             ul32;
} MIPIDSI_DBI_VCID_UNION;
typedef union {
    struct {
        u32         in_dbi_conf     : 4;
        u32         reserved0       : 4;
        u32         out_dbi_conf    : 4;
        u32         reserved1       : 4;
        u32         lut_size_conf   : 2;
        u32         reserved2       : 14;
    } bits;
    u32             ul32;
} MIPIDSI_DBI_CFG_UNION;
typedef union {
    struct {
        u32         partitioning_en : 1;
        u32         reserved0       : 31;
    } bits;
    u32             ul32;
} MIPIDSI_DBI_PARTITIONING_EN_UNION;
typedef union {
    struct {
        u32         wr_cmd_size         : 16;
        u32         allowed_cmd_size    : 16;
    } bits;
    u32             ul32;
} MIPIDSI_DBI_CMDSIZE_UNION;
typedef union {
    struct {
        u32         eotp_tx_en  : 1;
        u32         eotp_rx_en  : 1;
        u32         bta_en      : 1;
        u32         ecc_rx_en   : 1;
        u32         crc_rx_en   : 1;
        u32         reserved0   : 27;
    } bits;
    u32             ul32;
} MIPIDSI_PCKHDL_CFG_UNION;
typedef union {
    struct {
        u32         gen_vcid_rx : 2;
        u32         reserved0   : 30;
    } bits;
    u32             ul32;
} MIPIDSI_GEN_VCID_UNION;
typedef union {
    struct {
        u32         cmd_video_mode  : 1;            /* 0:Video mode; 1:Command mode */
        u32         reserved0       : 31;
    } bits;
    u32             ul32;
} MIPIDSI_MODE_CFG_UNION;
typedef union {
    struct {
        u32         vid_mode_type    : 2;
        u32         reserved0        : 6;
        u32         lp_vsa_en        : 1;
        u32         lp_vbp_en        : 1;
        u32         lp_vfp_en        : 1;
        u32         lp_vact_en       : 1;
        u32         lp_hbp_en        : 1;
        u32         lp_hfp_en        : 1;
        u32         frame_bta_ack_en : 1;
        u32         lp_cmd_en        : 1;
        u32         reserved1        : 16;
    } bits;
    u32             ul32;
} MIPIDSI_VID_MODE_CFG_UNION;
typedef union {
    struct {
        u32         vid_pkt_size    : 14;
        u32         reserved0       : 18;
    } bits;
    u32             ul32;
} MIPIDSI_VID_PKT_SIZE_UNION;
typedef union {
    struct {
        u32         vid_num_chunks  : 13;
        u32         reserved0       : 19;
    } bits;
    u32             ul32;
} MIPIDSI_VID_NUM_CHUNKS_UNION;
typedef union {
    struct {
        u32         vid_null_size   : 13;
        u32         reserved0       : 19;
    } bits;
    u32             ul32;
} MIPIDSI_VID_NULL_SIZE_UNION;
typedef union {
    struct {
        u32         vid_hsa_time    : 12;
        u32         reserved0       : 20;
    } bits;
    u32             ul32;
} MIPIDSI_VID_HSA_TIME_UNION;
typedef union {
    struct {
        u32         vid_hbp_time    : 12;
        u32         reserved0       : 20;
    } bits;
    u32             ul32;
} MIPIDSI_VID_HBP_TIME_UNION;
typedef union {
    struct {
        u32         vid_hline_time  : 15;
        u32         reserved0       : 17;
    } bits;
    u32             ul32;
} MIPIDSI_VID_HLINE_TIME_UNION;
typedef union {
    struct {
        u32         vsa_lines   : 10;
        u32         reserved0   : 22;
    } bits;
    u32             ul32;
} MIPIDSI_VID_VSA_LINES_UNION;
typedef union {
    struct {
        u32         vbp_lines   : 10;
        u32         reserved0   : 22;
    } bits;
    u32             ul32;
} MIPIDSI_VID_VBP_LINES_UNION;
typedef union {
    struct {
        u32         vfp_lines   : 10;
        u32         reserved0   : 22;
    } bits;
    u32             ul32;
} MIPIDSI_VID_VFP_LINES_UNION;
typedef union {
    struct {
        u32         v_active_lines  : 14;
        u32         reserved0       : 18;
    } bits;
    u32             ul32;
} MIPIDSI_VID_VACTIVE_LINES_UNION;
typedef union {
    struct {
        u32         edpi_allowed_cmd_size   : 16;
        u32         reserved0               : 16;
    } bits;
    u32             ul32;
}MIPIDSI_EDPI_CMD_SIZE_UNION;
typedef union {
    struct {
        u32         tear_fx_en      : 1;        /* bit 0 */
        u32         ack_rqst_en     : 1;        /* bit 1 */
        u32         reserved0       : 6;        /* bit 2~7 */
        u32         gen_sw_0p_tx    : 1;        /* bit 8 */
        u32         gen_sw_1p_tx    : 1;        /* bit 9 */
        u32         gen_sw_2p_tx    : 1;        /* bit 10 */
        u32         gen_sr_0p_tx    : 1;        /* bit 11 */
        u32         gen_sr_1p_tx    : 1;        /* bit 12 */
        u32         gen_sr_2p_tx    : 1;        /* bit 13 */
        u32         gen_lw_tx       : 1;        /* bit 14 */
        u32         reserved1       : 1;        /* bit 15 */
        u32         dcs_sw_0p_tx    : 1;        /* bit 16 */
        u32         dcs_sw_1p_tx    : 1;        /* bit 17 */
        u32         dcs_sr_0p_tx    : 1;        /* bit 18 */
        u32         dcs_lw_tx       : 1;        /* bit 19 */
        u32         reserved2       : 4;        /* bit 20~23 */
        u32         max_rd_pkt_size : 1;        /* bit 24 */
        u32         reserved3       : 7;        /* bit 25~31 */
    } bits;
    u32             ul32;
}MIPIDSI_CMD_MODE_CFG_UNION;
typedef union {
    struct {
        u32         gen_dt          : 6;
        u32         gen_vc          : 2;
        u32         gen_wc_lsbyte   : 8;
        u32         gen_wc_msbyte   : 8;
        u32         reserved0       : 8;
    } bits;
    u32             ul32;
}MIPIDSI_GEN_HDR_UNION;
typedef union {
    struct {
        u32         gen_pld_b1  : 8;
        u32         gen_pld_b2  : 8;
        u32         gen_pld_b3  : 8;
        u32         gen_pld_b4  : 8;
    } bits;
    u32             ul32;
}MIPIDSI_GEN_PLD_DATA_UNION;
typedef union {
    struct {
        u32         gen_cmd_empty   : 1;        /* bit 0 */
        u32         gen_cmd_full    : 1;        /* bit 1 */
        u32         gen_pld_w_empty : 1;        /* bit 2 */
        u32         gen_pld_w_full  : 1;        /* bit 3 */
        u32         gen_pld_r_empty : 1;        /* bit 4 */
        u32         gen_pld_r_full  : 1;        /* bit 5 */
        u32         gen_rd_cmd_busy : 1;        /* bit 6 */
        u32         reserved0       : 1;
        u32         dbi_cmd_empy    : 1;        /* bit 8 */
        u32         dbi_cmd_full    : 1;        /* bit 9 */
        u32         dbi_pld_w_empty : 1;        /* bit 10 */
        u32         dbi_pld_w_full  : 1;        /* bit 11 */
        u32         dbi_pld_r_empty : 1;        /* bit 12 */
        u32         dbi_pld_r_full  : 1;        /* bit 13 */
        u32         dbi_rd_cmd_busy : 1;        /* bit 14 */
        u32         reserved1       : 17;
    } bits;
    u32             ul32;
}MIPIDSI_CMD_PKT_STATUS_UNION;
typedef union {
    struct {
        u32         lprx_to_cnt : 16;
        u32         hstx_to_cnt : 16;
    } bits;
    u32             ul32;
}MIPIDSI_TO_CNT_CFG_UNION;
typedef union {
    struct {
        u32         hs_rd_to_cnt    : 16;
        u32         reserved0       : 16;
    } bits;
    u32             ul32;
}MIPIDSI_HS_RD_TO_CNT_UNION;
typedef union {
    struct {
        u32         lp_rd_to_cnt    : 16;
        u32         reserved0       : 16;
    } bits;
    u32             ul32;
}MIPIDSI_LP_RD_TO_CNT_UNION;
typedef union {
    struct {
        u32         hs_wr_to_cnt    : 16;
        u32         reserved0       : 8;
        u32         presp_to_mode   : 1;
        u32         reserved1       : 7;
    } bits;
    u32             ul32;
}MIPIDSI_HS_WR_TO_CNT_UNION;
typedef union {
    struct {
        u32         lp_wr_to_cnt    : 16;
        u32         reserved0       : 16;
    } bits;
    u32             ul32;
}MIPIDSI_LP_WR_TO_CNT_UNION;
typedef union {
    struct {
        u32         bta_to_cnt  : 16;
        u32         reserved0   : 16;
    } bits;
    u32             ul32;
}MIPIDSI_BTA_TO_CNT_UNION;
typedef union {
    struct {
        u32         mode_3d      : 2;
        u32         format_3d    : 2;
        u32         second_vsync : 1;
        u32         right_first  : 1;
        u32         reserved0    : 10;
        u32         send_3d_cfg  : 1;
        u32         reserved1    : 15;
    } bits;
    u32             ul32;
}MIPIDSI_SDF_3D_UNION;
typedef union {
    struct {
        u32         phy_txrequestclkhs  : 1;
        u32         auto_clklane_ctrl   : 1;
        u32         reserved0           : 30;
    } bits;
    u32             ul32;
}MIPIDSI_LPCLK_CTRL_UNION;
typedef union {
    struct {
        u32         phy_clklp2hs_time   : 10;
        u32         reserved0           : 6;
        u32         phy_clkhs2lp_time   : 10;
        u32         reserved1           : 6;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_TMR_LPCLK_CFG_UNION;
typedef union {
    struct {
        u32         max_rd_time     : 15;
        u32         reserved0       : 1;
        u32         phy_lp2hs_time  : 8;
        u32         phy_hs2lp_time  : 8;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_TMR_CFG_UNION;
typedef union {
    struct {
        u32         clk_to_data_delay    : 8;
        u32         data_to_clk_delay    : 8;
        u32         txclkesc_neg_sample  : 1;
        u32         reserved0  : 15;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_CLK_DATA_TMG_CFG_UNION;
typedef union {
    struct {
        u32         phy_shutdownz   : 1;
        u32         phy_rstz        : 1;
        u32         phy_enableclk   : 1;
        u32         phy_forcepll    : 1;
        u32         reserved0       : 28;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_RSTZ_UNION;
typedef union {
    struct {
        u32         n_lanes             : 2;
        u32         reserved0           : 6;
        u32         phy_stop_wait_time  : 8;
        u32         reserved1           : 16;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_IF_CFG_UNION;
typedef union {
    struct {
        u32         phy_txrequlpsclk   : 1;
        u32         phy_txexitulpsclk  : 1;
        u32         phy_txrequlpslan   : 1;
        u32         phy_txexitulpslan  : 1;
        u32         reserved0          : 28;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_ULPS_CTRL_UNION;
typedef union {
    struct {
        u32         phy_tx_triggers : 4;
        u32         reserved0       : 28;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_TX_TRIGGERS_UNION;
typedef union {
    struct {
        u32         phy_lock                : 1;    /* bit 0 */
        u32         phy_direction           : 1;    /* bit 1 */
        u32         phy_stopstateclklane    : 1;    /* bit 2 */
        u32         phy_ulpsactivenotclk    : 1;    /* bit 3 */
        u32         phy_stopstate0lane      : 1;    /* bit 4 */
        u32         phy_ulpsactivenot0lane  : 1;    /* bit 5 */
        u32         phy_rxulpsesc0lane      : 1;    /* bit 6 */
        u32         phy_stopstate1lane      : 1;    /* bit 7 */
        u32         phy_ulpsactivenot1lane  : 1;    /* bit 8 */
        u32         phy_stopstate2lane      : 1;    /* bit 9 */
        u32         phy_ulpsactivenot2lane  : 1;    /* bit 10 */
        u32         phy_stopstate3lane      : 1;    /* bit 11 */
        u32         phy_ulpsactivenot3lane  : 1;    /* bit 12 */
        u32         reserved0               : 19;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_STATUS_UNION;
typedef union {
    struct {
        u32         phy_testclr : 1;
        u32         phy_testclk : 1;
        u32         reserved0   : 30;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_TST_CTRL0_UNION;


/* D-PHY test interface control 1 */
typedef union {
    struct {
        u32         phy_testdin     : 8;
        u32         phy_testdout    : 8;
        u32         phy_testen      : 1;
        u32         reserved0       : 15;
    } bits;
    u32             ul32;
}MIPIDSI_PHY_TST_CTRL1_UNION;


/* interrupt status register 0 */
typedef union {
    struct {
        u32         ack_with_err_0  : 1;    /* bit 0 */
        u32         ack_with_err_1  : 1;    /* bit 1 */
        u32         ack_with_err_2  : 1;    /* bit 2 */
        u32         ack_with_err_3  : 1;    /* bit 3 */
        u32         ack_with_err_4  : 1;    /* bit 4 */
        u32         ack_with_err_5  : 1;    /* bit 5 */
        u32         ack_with_err_6  : 1;    /* bit 6 */
        u32         ack_with_err_7  : 1;    /* bit 7 */
        u32         ack_with_err_8  : 1;    /* bit 8 */
        u32         ack_with_err_9  : 1;    /* bit 9 */
        u32         ack_with_err_10 : 1;    /* bit 10 */
        u32         ack_with_err_11 : 1;    /* bit 11 */
        u32         ack_with_err_12 : 1;    /* bit 12 */
        u32         ack_with_err_13 : 1;    /* bit 13 */
        u32         ack_with_err_14 : 1;    /* bit 14 */
        u32         ack_with_err_15 : 1;    /* bit 15 */
        u32         dphy_errors_0   : 1;    /* bit 16 */
        u32         dphy_errors_1   : 1;    /* bit 17 */
        u32         dphy_errors_2   : 1;    /* bit 18 */
        u32         dphy_errors_3   : 1;    /* bit 19 */
        u32         dphy_errors_4   : 1;    /* bit 20 */
        u32         reserved0       : 11;
    } bits;
    u32             ul32;
}MIPIDSI_INT_ST0_UNION;


/* interrupt status register 1 */
typedef union {
    struct {
        u32         to_hs_tx            : 1;    /* bit 0 */
        u32         to_lp_rx            : 1;    /* bit 1 */
        u32         ecc_single_err      : 1;    /* bit 2 */
        u32         ecc_milti_err       : 1;    /* bit 3 */
        u32         crc_err             : 1;    /* bit 4 */
        u32         pkt_size_err        : 1;    /* bit 5 */
        u32         eopt_err            : 1;    /* bit 6 */
        u32         dpi_pld_wr_err      : 1;    /* bit 7 */
        u32         gen_cmd_wr_err      : 1;    /* bit 8 */
        u32         gen_pld_wr_err      : 1;    /* bit 9 */
        u32         gen_pld_send_err    : 1;    /* bit 10 */
        u32         gen_pld_rd_err      : 1;    /* bit 11 */
        u32         gen_pld_recev_err   : 1;    /* bit 12 */
        u32         dbi_cmd_wr_err      : 1;    /* bit 13 */
        u32         dbi_pld_wr_err      : 1;    /* bit 14 */
        u32         dbi_pld_rd_err      : 1;    /* bit 15 */
        u32         dbi_pld_recv_err    : 1;    /* bit 16 */
        u32         dbi_ilegal_comm_err : 1;    /* bit 17 */
        u32         reserved0           : 14;
    } bits;
    u32             ul32;
}MIPIDSI_INT_ST1_UNION;
typedef union {
    struct {
        u32         ack_with_err_0  : 1;    /* bit 0 */
        u32         ack_with_err_1  : 1;    /* bit 1 */
        u32         ack_with_err_2  : 1;    /* bit 2 */
        u32         ack_with_err_3  : 1;    /* bit 3 */
        u32         ack_with_err_4  : 1;    /* bit 4 */
        u32         ack_with_err_5  : 1;    /* bit 5 */
        u32         ack_with_err_6  : 1;    /* bit 6 */
        u32         ack_with_err_7  : 1;    /* bit 7 */
        u32         ack_with_err_8  : 1;    /* bit 8 */
        u32         ack_with_err_9  : 1;    /* bit 9 */
        u32         ack_with_err_10 : 1;    /* bit 10 */
        u32         ack_with_err_11 : 1;    /* bit 11 */
        u32         ack_with_err_12 : 1;    /* bit 12 */
        u32         ack_with_err_13 : 1;    /* bit 13 */
        u32         ack_with_err_14 : 1;    /* bit 14 */
        u32         ack_with_err_15 : 1;    /* bit 15 */
        u32         dphy_errors_0   : 1;    /* bit 16 */
        u32         dphy_errors_1   : 1;    /* bit 17 */
        u32         dphy_errors_2   : 1;    /* bit 18 */
        u32         dphy_errors_3   : 1;    /* bit 19 */
        u32         dphy_errors_4   : 1;    /* bit 20 */
        u32         reserved0       : 11;
    } bits;
    u32             ul32;
}MIPIDSI_INT_MSK0_UNION;
typedef union {
    struct {
        u32         to_hs_tx            : 1;    /* bit 0 */
        u32         to_lp_rx            : 1;    /* bit 1 */
        u32         ecc_single_err      : 1;    /* bit 2 */
        u32         ecc_milti_err       : 1;    /* bit 3 */
        u32         crc_err             : 1;    /* bit 4 */
        u32         pkt_size_err        : 1;    /* bit 5 */
        u32         eopt_err            : 1;    /* bit 6 */
        u32         dpi_pld_wr_err      : 1;    /* bit 7 */
        u32         gen_cmd_wr_err      : 1;    /* bit 8 */
        u32         gen_pld_wr_err      : 1;    /* bit 9 */
        u32         gen_pld_send_err    : 1;    /* bit 10 */
        u32         gen_pld_rd_err      : 1;    /* bit 11 */
        u32         gen_pld_recev_err   : 1;    /* bit 12 */
        u32         dbi_cmd_wr_err      : 1;    /* bit 13 */
        u32         dbi_pld_wr_err      : 1;    /* bit 14 */
        u32         dbi_pld_rd_err      : 1;    /* bit 15 */
        u32         dbi_pld_recv_err    : 1;    /* bit 16 */
        u32         dbi_ilegal_comm_err : 1;    /* bit 17 */
        u32         reserved0           : 14;
    } bits;
    u32             ul32;
}MIPIDSI_INT_MSK1_UNION;

enum {
    CMD_MODE = 0,
    HSPD_MODE
};


extern void set_MIPIDSI_PWR_UP( u32 nVal );
extern void set_MIPIDSI_PWR_UP_shutdownz( u32 nVal );
extern void set_MIPIDSI_CLKMGR_CFG_tx_esc_clk_division( u32 nVal );
extern void set_MIPIDSI_CLKMGR_CFG_to_clk_dividsion( u32 nVal );
extern void set_MIPIDSI_DPI_VCID( u32 nVal );
extern void set_MIPIDSI_DPI_COLOR_CODING_dpi_color_coding( u32 nVal );
extern void set_MIPIDSI_DPI_COLOR_CODING_loosely18_en( u32 nVal );
extern void set_MIPIDSI_DPI_CFG_POL_dataen_active_low( u32 nVal );
extern void set_MIPIDSI_DPI_CFG_POL_vsync_active_low( u32 nVal );
extern void set_MIPIDSI_DPI_CFG_POL_hsync_active_low( u32 nVal );
extern void set_MIPIDSI_DPI_CFG_POL_shutd_active_low( u32 nVal );
extern void set_MIPIDSI_DPI_CFG_POL_colorm_active_low( u32 nVal );
extern void set_MIPIDSI_PCKHDL_CFG_bta_en( u32 nVal );
extern void set_MIPIDSI_PCKHDL_CFG_eotp_tx_en( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_lp_vsa_en( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_lp_vbp_en( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_lp_vfp_en( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_lp_vact_en( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_lp_hbp_en( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_lp_hfp_en( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_frame_bta_ack_en( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_vid_mode_type( u32 nVal );
extern void set_MIPIDSI_MODE_CFG( u32 nVal );
extern void set_MIPIDSI_VID_PKT_SIZE( u32 nVal );
extern void set_MIPIDSI_VID_HSA_TIME( u32 nVal );
extern void set_MIPIDSI_VID_HBP_TIME( u32 nVal );
extern void set_MIPIDSI_VID_HLINE_TIME( u32 nVal );
extern void set_MIPIDSI_VID_VSA_LINES( u32 nVal );
extern void set_MIPIDSI_VID_VBP_LINES( u32 nVal );
extern void set_MIPIDSI_VID_VFP_LINES( u32 nVal );
extern void set_MIPIDSI_VID_VACTIVE_LINES( u32 nVal );
extern void set_MIPIDSI_CMD_MODE_CFG_all_en_flag( u32 nVal );
extern void set_MIPIDSI_TO_CNT_CFG(u32 nVal);
extern void set_MIPIDSI_BTA_TO_CNT_bta_to_cnt( u32 nVal );
extern void set_MIPIDSI_PHY_TMR_CFG_phy_lp2hs_time( u32 nVal );
extern void set_MIPIDSI_PHY_TMR_CFG_phy_hs2lp_time( u32 nVal );
extern void set_MIPIDSI_GEN_HDR( u8 *dsi_base, u32 nVal );
extern void set_MIPIDSI_GEN_PLD_DATA( u32 nVal );
extern void set_MIPIDSI_LPCLK_CTRL_phy_txrequestclkhs( u32 nVal );
extern void set_MIPIDSI_LPCLK_CTRL_auto_clklane_ctrl( u32 nVal );
extern void set_MIPIDSI_PHY_RSTZ( u32 nVal );
extern void set_MIPIDSI_PHY_IF_CFG_n_lanes( u32 nVal );
extern void set_MIPIDSI_PHY_TST_CTRL0( u32 nVal );
extern void set_MIPIDSI_PHY_TST_CTRL1( u32 nVal );
extern void set_MIPIDSI_EDPI_CMD_SIZE( u32 nVal );
extern void set_MIPIDSI_PHY_TMR_LPCLK_CFG_phy_clklp2hs_time( u32 nVal );
extern void set_MIPIDSI_PHY_TMR_LPCLK_CFG_phy_clkhs2lp_time( u32 nVal );
extern void set_MIPIDSI_PHY_TMR_clk_to_data_delay( u32 nVal );
extern void set_MIPIDSI_PHY_TMR_data_to_clk_delay( u32 nVal );
extern void set_MIPIDSI_PHY_TMR_txclkesc_neg_smaple( u32 nVal );
void set_dsi_mode(u32 mode);
extern void set_MIPIDSI_DPI_CFG_LP_TIM( u32 nVal );
extern void set_MIPIDSI_VID_MODE_CFG_lp_cmd_en( u32 nVal );
extern void set_MIPIDSI_PHY_TMR_CFG_max_rd_time( u32 nVal );
#endif  /* MIPI_REG_H */
