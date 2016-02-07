//******************************************************************************
//  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd.
//
//******************************************************************************
// File name     : c_union_define.h
// Version       : 2.0
// Author        : tangzhimin/195756
// Created       : 2013-12-23
// Last Modified : 
// Description   :  The C union definition file for the module ade
// Function List : 
// History       : 
// 1 Date        : 
// Author        : tangzhimin/195756
// Modification  : Create file
//******************************************************************************

#ifndef __ADE_UNION_DEFINE_H__
#define __ADE_UNION_DEFINE_H__

// Define the union U_ADE_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    frm_end_start         : 2   ; // [1..0] 
        unsigned int    dfs_buf_cfg           : 1   ; // [2] 
        unsigned int    rot_buf_cfg           : 3   ; // [5..3] 
        unsigned int    rd_ch5_nv             : 1   ; // [6] 
        unsigned int    rd_ch6_nv             : 1   ; // [7] 
        unsigned int    dfs_buf_unflow_lev1   : 13  ; // [20..8] 
        unsigned int    dfs_buf_unflow_lev2   : 11  ; // [31..21] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRL;

// Define the union U_ADE_SCL3_MUX_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    scl3_mux_cfg          : 1   ; // [0] 
        unsigned int    clip6_wdma3_trans_en  : 1   ; // [1] 
        unsigned int    Reserved_0            : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_MUX_CFG;

// Define the union U_ADE_SCL1_MUX_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    scl1_mux_cfg          : 2   ; // [1..0] 
        unsigned int    Reserved_1            : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_MUX_CFG;

// Define the union U_ADE_ROT_SRC_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rot_src_cfg           : 2   ; // [1..0] 
        unsigned int    Reserved_2            : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_ROT_SRC_CFG;

// Define the union U_ADE_SCL2_SRC_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    scl2_src_cfg          : 3   ; // [2..0] 
        unsigned int    Reserved_3            : 29  ; // [31..3] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_SRC_CFG;

// Define the union U_ADE_DISP_SRC_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_src_cfg          : 3   ; // [2..0] 
        unsigned int    Reserved_4            : 29  ; // [31..3] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_DISP_SRC_CFG;

// Define the union U_ADE_WDMA2_SRC_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wdma2_src_cfg         : 3   ; // [2..0] 
        unsigned int    Reserved_5            : 29  ; // [31..3] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_WDMA2_SRC_CFG;

// Define the union U_ADE_SEC_OVLY_SRC_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    sec_ovly_src_cfg      : 2   ; // [1..0] 
        unsigned int    Reserved_6            : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SEC_OVLY_SRC_CFG;

// Define the union U_ADE_WDMA3_SRC_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wdma3_src_cfg         : 3   ; // [2..0] 
        unsigned int    Reserved_7            : 29  ; // [31..3] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_WDMA3_SRC_CFG;

// Define the union U_ADE_OVLY1_TRANS_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovly1_wdma1_trans_en  : 1   ; // [0] 
        unsigned int    Reserved_8            : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY1_TRANS_CFG;

// Define the union U_ADE_CTRAN5_TRANS_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran5_ovly_trans_en  : 1   ; // [0] 
        unsigned int    Reserved_10           : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_TRANS_CFG;

// Define the union U_ADE_DMA_AXI_MUX
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_dma_ch1_axi_cfg    : 1   ; // [0] 
        unsigned int    rd_dma_ch2_axi_cfg    : 1   ; // [1] 
        unsigned int    rd_dma_ch3_axi_cfg    : 1   ; // [2] 
        unsigned int    rd_dma_ch4_axi_cfg    : 1   ; // [3] 
        unsigned int    rd_dma_ch5_axi_cfg    : 1   ; // [4] 
        unsigned int    rd_dma_ch6_axi_cfg    : 1   ; // [5] 
        unsigned int    rd_dma_disp_axi_cfg   : 1   ; // [6] 
        unsigned int    rd_dma_cmdq1_axi_cfg  : 1   ; // [7] 
        unsigned int    rd_dma_cmdq2_axi_cfg  : 1   ; // [8] 
        unsigned int    Reserved_14           : 1   ; // [9] 
        unsigned int    wr_dma_ch1_axi_cfg    : 1   ; // [10] 
        unsigned int    wr_dma_ch2_axi_cfg    : 1   ; // [11] 
        unsigned int    wr_dma_ch3_axi_cfg    : 1   ; // [12] 
        unsigned int    Reserved_13           : 1   ; // [13] 
        unsigned int    wr_dma_cmdq_axi_cfg   : 1   ; // [14] 
        unsigned int    Reserved_12           : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_DMA_AXI_MUX;

// Define the union U_ADE_DMA_SEC_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_dma_ch1_sec        : 1   ; // [0] 
        unsigned int    rd_dma_ch2_sec        : 1   ; // [1] 
        unsigned int    rd_dma_ch3_sec        : 1   ; // [2] 
        unsigned int    rd_dma_ch4_sec        : 1   ; // [3] 
        unsigned int    rd_dma_ch5_sec        : 1   ; // [4] 
        unsigned int    rd_dma_ch6_sec        : 1   ; // [5] 
        unsigned int    rd_dma_disp_sec       : 1   ; // [6] 
        unsigned int    rd_dma_cmdq1_sec      : 1   ; // [7] 
        unsigned int    rd_dma_cmdq2_sec      : 1   ; // [8] 
        unsigned int    Reserved_18           : 1   ; // [9] 
        unsigned int    wr_dma_ch1_sec        : 1   ; // [10] 
        unsigned int    wr_dma_ch2_sec        : 1   ; // [11] 
        unsigned int    wr_dma_ch3_sec        : 1   ; // [12] 
        unsigned int    Reserved_17           : 1   ; // [13] 
        unsigned int    wr_dma_cmdq_sec       : 1   ; // [14] 
        unsigned int    Reserved_16           : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_DMA_SEC_CFG;

// Define the union U_ADE_DMA_AXI_OSD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ade_axi0_rd_os        : 4   ; // [3..0] 
        unsigned int    ade_axi0_wr_os        : 4   ; // [7..4] 
        unsigned int    Reserved_20           : 8   ; // [15..8] 
        unsigned int    ade_axi1_rd_os        : 4   ; // [19..16] 
        unsigned int    ade_axi1_wr_os        : 4   ; // [23..20] 
        unsigned int    Reserved_19           : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_DMA_AXI_OSD;

// Define the union U_ADE_SOFT_RST0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_rdma_srst         : 1   ; // [0] 
        unsigned int    ch2_rdma_srst         : 1   ; // [1] 
        unsigned int    ch3_rdma_srst         : 1   ; // [2] 
        unsigned int    ch4_rdma_srst         : 1   ; // [3] 
        unsigned int    ch5_rdma_srst         : 1   ; // [4] 
        unsigned int    ch6_rdma_srst         : 1   ; // [5] 
        unsigned int    disp_rdma_srst        : 1   ; // [6] 
        unsigned int    cmdq1_rdma_srst       : 1   ; // [7] 
        unsigned int    cmdq2_rdma_srst       : 1   ; // [8] 
        unsigned int    Reserved_23           : 1   ; // [9] 
        unsigned int    ch1_wdma_srst         : 1   ; // [10] 
        unsigned int    ch2_wdma_srst         : 1   ; // [11] 
        unsigned int    ch3_wdma_srst         : 1   ; // [12] 
        unsigned int    Reserved_22           : 1   ; // [13] 
        unsigned int    cmdq_wdma_srst        : 1   ; // [14] 
        unsigned int    clip1_srst            : 1   ; // [15] 
        unsigned int    clip2_srst            : 1   ; // [16] 
        unsigned int    clip3_srst            : 1   ; // [17] 
        unsigned int    clip4_srst            : 1   ; // [18] 
        unsigned int    clip5_srst            : 1   ; // [19] 
        unsigned int    clip6_srst            : 1   ; // [20] 
        unsigned int    scl1_srst             : 1   ; // [21] 
        unsigned int    scl2_srst             : 1   ; // [22] 
        unsigned int    scl3_srst             : 1   ; // [23] 
        unsigned int    ctran1_srst           : 1   ; // [24] 
        unsigned int    ctran2_srst           : 1   ; // [25] 
        unsigned int    ctran3_srst           : 1   ; // [26] 
        unsigned int    ctran4_srst           : 1   ; // [27] 
        unsigned int    ctran5_srst           : 1   ; // [28] 
        unsigned int    ctran6_srst           : 1   ; // [29] 
        unsigned int    rot_srst              : 1   ; // [30] 
        unsigned int    Reserved_21           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SOFT_RST0;

// Define the union U_ADE_SOFT_RST1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cmdq1_srst            : 1   ; // [0] 
        unsigned int    cmdq2_srst            : 1   ; // [1] 
        unsigned int    Reserved_26           : 1   ; // [2] 
        unsigned int    gamma_srst            : 1   ; // [3] 
        unsigned int    dither_srst           : 1   ; // [4] 
        unsigned int    ovly1_srst            : 1   ; // [5] 
        unsigned int    ovly2_srst            : 1   ; // [6] 
        unsigned int    ovly3_srst            : 1   ; // [7] 
        unsigned int    Reserved_25           : 24  ; // [31..8] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SOFT_RST1;

// Define the union U_ADE_SOFT_RST_SEL0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_rdma_srst_sel     : 1   ; // [0] 
        unsigned int    ch2_rdma_srst_sel     : 1   ; // [1] 
        unsigned int    ch3_rdma_srst_sel     : 1   ; // [2] 
        unsigned int    ch4_rdma_srst_sel     : 1   ; // [3] 
        unsigned int    ch5_rdma_srst_sel     : 1   ; // [4] 
        unsigned int    ch6_rdma_srst_sel     : 1   ; // [5] 
        unsigned int    disp_rdma_srst_sel    : 1   ; // [6] 
        unsigned int    cmdq1_rdma_srst_sel   : 1   ; // [7] 
        unsigned int    cmdq2_rdma_srst_sel   : 1   ; // [8] 
        unsigned int    Reserved_29           : 1   ; // [9] 
        unsigned int    ch1_wdma_srst_sel     : 1   ; // [10] 
        unsigned int    ch2_wdma_srst_sel     : 1   ; // [11] 
        unsigned int    ch3_wdma_srst_sel     : 1   ; // [12] 
        unsigned int    Reserved_28           : 1   ; // [13] 
        unsigned int    cmdq_wdma_srst_sel    : 1   ; // [14] 
        unsigned int    clip1_srst_sel        : 1   ; // [15] 
        unsigned int    clip2_srst_sel        : 1   ; // [16] 
        unsigned int    clip3_srst_sel        : 1   ; // [17] 
        unsigned int    clip4_srst_sel        : 1   ; // [18] 
        unsigned int    clip5_srst_sel        : 1   ; // [19] 
        unsigned int    clip6_srst_sel        : 1   ; // [20] 
        unsigned int    scl1_srst_sel         : 1   ; // [21] 
        unsigned int    scl2_srst_sel         : 1   ; // [22] 
        unsigned int    scl3_srst_sel         : 1   ; // [23] 
        unsigned int    ctran1_srst_sel       : 1   ; // [24] 
        unsigned int    ctran2_srst_sel       : 1   ; // [25] 
        unsigned int    ctran3_srst_sel       : 1   ; // [26] 
        unsigned int    ctran4_srst_sel       : 1   ; // [27] 
        unsigned int    ctran5_srst_sel       : 1   ; // [28] 
        unsigned int    ctran6_srst_sel       : 1   ; // [29] 
        unsigned int    rot_srst_sel          : 1   ; // [30] 
        unsigned int    Reserved_27           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SOFT_RST_SEL0;

// Define the union U_ADE_SOFT_RST_SEL1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cmdq1_srst_sel        : 1   ; // [0] 
        unsigned int    cmdq2_srst_sel        : 1   ; // [1] 
        unsigned int    Reserved_31           : 1   ; // [2] 
        unsigned int    gamma_srst_sel        : 1   ; // [3] 
        unsigned int    dither_srst_sel       : 1   ; // [4] 
        unsigned int    ovly1_srst_sel        : 1   ; // [5] 
        unsigned int    ovly2_srst_sel        : 1   ; // [6] 
        unsigned int    ovly3_srst_sel        : 1   ; // [7] 
        unsigned int    Reserved_30           : 24  ; // [31..8] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SOFT_RST_SEL1;

// Define the union U_ADE_STAT0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_rdma_status       : 1   ; // [0] 
        unsigned int    ch2_rdma_status       : 1   ; // [1] 
        unsigned int    ch3_rdma_status       : 1   ; // [2] 
        unsigned int    ch4_rdma_status       : 1   ; // [3] 
        unsigned int    ch5_rdma_status       : 1   ; // [4] 
        unsigned int    ch6_rdma_status       : 1   ; // [5] 
        unsigned int    disp_rdma_status      : 1   ; // [6] 
        unsigned int    cmdq1_rdma_status     : 1   ; // [7] 
        unsigned int    cmdq2_rdma_status     : 1   ; // [8] 
        unsigned int    Reserved_34           : 1   ; // [9] 
        unsigned int    ch1_wdma_status       : 1   ; // [10] 
        unsigned int    ch2_wdma_status       : 1   ; // [11] 
        unsigned int    ch3_wdma_status       : 1   ; // [12] 
        unsigned int    Reserved_33           : 1   ; // [13] 
        unsigned int    cmdq_wdma_status      : 1   ; // [14] 
        unsigned int    clip1_status          : 1   ; // [15] 
        unsigned int    clip2_status          : 1   ; // [16] 
        unsigned int    clip3_status          : 1   ; // [17] 
        unsigned int    clip4_status          : 1   ; // [18] 
        unsigned int    clip5_status          : 1   ; // [19] 
        unsigned int    clip6_status          : 1   ; // [20] 
        unsigned int    scl1_status           : 1   ; // [21] 
        unsigned int    scl2_status           : 1   ; // [22] 
        unsigned int    scl3_status           : 1   ; // [23] 
        unsigned int    ctran1_status         : 1   ; // [24] 
        unsigned int    ctran2_status         : 1   ; // [25] 
        unsigned int    ctran3_status         : 1   ; // [26] 
        unsigned int    ctran4_status         : 1   ; // [27] 
        unsigned int    ctran5_status         : 1   ; // [28] 
        unsigned int    ctran6_status         : 1   ; // [29] 
        unsigned int    rot_status            : 1   ; // [30] 
        unsigned int    Reserved_32           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_STAT0;

// Define the union U_ADE_STAT1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cmdq1_status          : 1   ; // [0] 
        unsigned int    cmdq2_status          : 1   ; // [1] 
        unsigned int    Reserved_37           : 1   ; // [2] 
        unsigned int    gamma_status          : 1   ; // [3] 
        unsigned int    dither_status         : 1   ; // [4] 
        unsigned int    ovly1_status          : 1   ; // [5] 
        unsigned int    ovly2_status          : 1   ; // [6] 
        unsigned int    ovly3_status          : 1   ; // [7] 
        unsigned int    Reserved_36           : 8   ; // [15..8] 
        unsigned int    dfs_buf_fill_lev      : 13  ; // [28..16] 
        unsigned int    Reserved_35           : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_STAT1;

// Define the union U_ADE_DMA_ERR_STAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_rdma_err_status   : 1   ; // [0] 
        unsigned int    ch2_rdma_err_status   : 1   ; // [1] 
        unsigned int    ch3_rdma_err_status   : 1   ; // [2] 
        unsigned int    ch4_rdma_err_status   : 1   ; // [3] 
        unsigned int    ch5_rdma_err_status   : 1   ; // [4] 
        unsigned int    ch6_rdma_err_status   : 1   ; // [5] 
        unsigned int    disp_rdma_err_status  : 1   ; // [6] 
        unsigned int    cmdq1_rdma_err_status  : 1   ; // [7] 
        unsigned int    cmdq2_rdma_err_status  : 1   ; // [8] 
        unsigned int    Reserved_43           : 1   ; // [9] 
        unsigned int    ch1_wdma_err_status   : 1   ; // [10] 
        unsigned int    ch2_wdma_err_status   : 1   ; // [11] 
        unsigned int    ch3_wdma_err_status   : 1   ; // [12] 
        unsigned int    Reserved_42           : 1   ; // [13] 
        unsigned int    cmdq_wdma_err_status  : 1   ; // [14] 
        unsigned int    Reserved_41           : 1   ; // [15] 
        unsigned int    ch1_rdma_end_err_status  : 1   ; // [16] 
        unsigned int    ch2_rdma_end_err_status  : 1   ; // [17] 
        unsigned int    ch3_rdma_end_err_status  : 1   ; // [18] 
        unsigned int    ch4_rdma_end_err_status  : 1   ; // [19] 
        unsigned int    ch5_rdma_end_err_status  : 1   ; // [20] 
        unsigned int    ch6_rdma_end_err_status  : 1   ; // [21] 
        unsigned int    disp_rdma_end_err_status  : 1   ; // [22] 
        unsigned int    cmdq1_rdma_end_err_status  : 1   ; // [23] 
        unsigned int    cmdq2_rdma_end_err_status  : 1   ; // [24] 
        unsigned int    Reserved_40           : 1   ; // [25] 
        unsigned int    ch1_wdma_end_err_status  : 1   ; // [26] 
        unsigned int    ch2_wdma_end_err_status  : 1   ; // [27] 
        unsigned int    ch3_wdma_end_err_status  : 1   ; // [28] 
        unsigned int    Reserved_39           : 1   ; // [29] 
        unsigned int    cmdq_wdma_end_err_status  : 1   ; // [30] 
        unsigned int    Reserved_38           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_DMA_ERR_STAT;

// Define the union U_ADE_CTRL1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    auto_clk_gate_en      : 1   ; // [0] 
        unsigned int    rot_buf_shr_out       : 1   ; // [1] 
        unsigned int    Reserved_44           : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRL1;

// Define the union U_ADE_SEC_CFG0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_rdma_sec          : 1   ; // [0] 
        unsigned int    ch2_rdma_sec          : 1   ; // [1] 
        unsigned int    ch3_rdma_sec          : 1   ; // [2] 
        unsigned int    ch4_rdma_sec          : 1   ; // [3] 
        unsigned int    ch5_rdma_sec          : 1   ; // [4] 
        unsigned int    ch6_rdma_sec          : 1   ; // [5] 
        unsigned int    disp_rdma_sec         : 1   ; // [6] 
        unsigned int    cmdq1_rdma_sec        : 1   ; // [7] 
        unsigned int    cmdq2_rdma_sec        : 1   ; // [8] 
        unsigned int    Reserved_47           : 1   ; // [9] 
        unsigned int    ch1_wdma_sec          : 1   ; // [10] 
        unsigned int    ch2_wdma_sec          : 1   ; // [11] 
        unsigned int    ch3_wdma_sec          : 1   ; // [12] 
        unsigned int    Reserved_46           : 1   ; // [13] 
        unsigned int    cmdq_wdma_sec         : 1   ; // [14] 
        unsigned int    clip1_sec             : 1   ; // [15] 
        unsigned int    clip2_sec             : 1   ; // [16] 
        unsigned int    clip3_sec             : 1   ; // [17] 
        unsigned int    clip4_sec             : 1   ; // [18] 
        unsigned int    clip5_sec             : 1   ; // [19] 
        unsigned int    clip6_sec             : 1   ; // [20] 
        unsigned int    scl1_sec              : 1   ; // [21] 
        unsigned int    scl2_sec              : 1   ; // [22] 
        unsigned int    scl3_sec              : 1   ; // [23] 
        unsigned int    ctran1_sec            : 1   ; // [24] 
        unsigned int    ctran2_sec            : 1   ; // [25] 
        unsigned int    ctran3_sec            : 1   ; // [26] 
        unsigned int    ctran4_sec            : 1   ; // [27] 
        unsigned int    ctran5_sec            : 1   ; // [28] 
        unsigned int    ctran6_sec            : 1   ; // [29] 
        unsigned int    rot_sec               : 1   ; // [30] 
        unsigned int    Reserved_45           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SEC_CFG0;

// Define the union U_ADE_SEC_CFG1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cmdq1_sec             : 1   ; // [0] 
        unsigned int    cmdq2_sec             : 1   ; // [1] 
        unsigned int    Reserved_49           : 1   ; // [2] 
        unsigned int    gamma_sec             : 1   ; // [3] 
        unsigned int    dither_sec            : 1   ; // [4] 
        unsigned int    ovly1_sec             : 1   ; // [5] 
        unsigned int    ovly2_sec             : 1   ; // [6] 
        unsigned int    ovly3_sec             : 1   ; // [7] 
        unsigned int    top_sec               : 1   ; // [8] 
        unsigned int    Reserved_48           : 23  ; // [31..9] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SEC_CFG1;

// Define the union U_ADE_OVLY_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_ovly_sel          : 2   ; // [1..0] 
        unsigned int    Reserved_57           : 2   ; // [3..2] 
        unsigned int    ch2_ovly_sel          : 2   ; // [5..4] 
        unsigned int    Reserved_56           : 2   ; // [7..6] 
        unsigned int    ch3_ovly_sel          : 2   ; // [9..8] 
        unsigned int    Reserved_55           : 2   ; // [11..10] 
        unsigned int    ch4_ovly_sel          : 2   ; // [13..12] 
        unsigned int    Reserved_54           : 2   ; // [15..14] 
        unsigned int    ch5_ovly_sel          : 2   ; // [17..16] 
        unsigned int    Reserved_53           : 2   ; // [19..18] 
        unsigned int    ch6_ovly_sel          : 2   ; // [21..20] 
        unsigned int    Reserved_52           : 2   ; // [23..22] 
        unsigned int    ch7_ovly_sel          : 2   ; // [25..24] 
        unsigned int    Reserved_51           : 2   ; // [27..26] 
        unsigned int    ch8_ovly_sel          : 2   ; // [29..28] 
        unsigned int    Reserved_50           : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CTL;

// Define the union U_ADE_AUTO_CLK_GT_EN0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_rdma_auto_clk_gt_en  : 1   ; // [0] 
        unsigned int    ch2_rdma_auto_clk_gt_en  : 1   ; // [1] 
        unsigned int    ch3_rdma_auto_clk_gt_en  : 1   ; // [2] 
        unsigned int    ch4_rdma_auto_clk_gt_en  : 1   ; // [3] 
        unsigned int    ch5_rdma_auto_clk_gt_en  : 1   ; // [4] 
        unsigned int    ch6_rdma_auto_clk_gt_en  : 1   ; // [5] 
        unsigned int    disp_rdma_auto_clk_gt_en  : 1   ; // [6] 
        unsigned int    cmdq1_rdma_auto_clk_gt_en  : 1   ; // [7] 
        unsigned int    cmdq2_rdma_auto_clk_gt_en  : 1   ; // [8] 
        unsigned int    Reserved_60           : 1   ; // [9] 
        unsigned int    ch1_wdma_auto_clk_gt_en  : 1   ; // [10] 
        unsigned int    ch2_wdma_auto_clk_gt_en  : 1   ; // [11] 
        unsigned int    ch3_wdma_auto_clk_gt_en  : 1   ; // [12] 
        unsigned int    Reserved_59           : 1   ; // [13] 
        unsigned int    cmdq_wdma_auto_clk_gt_en  : 1   ; // [14] 
        unsigned int    clip1_auto_clk_gt_en  : 1   ; // [15] 
        unsigned int    clip2_auto_clk_gt_en  : 1   ; // [16] 
        unsigned int    clip3_auto_clk_gt_en  : 1   ; // [17] 
        unsigned int    clip4_auto_clk_gt_en  : 1   ; // [18] 
        unsigned int    clip5_auto_clk_gt_en  : 1   ; // [19] 
        unsigned int    clip6_auto_clk_gt_en  : 1   ; // [20] 
        unsigned int    scl1_auto_clk_gt_en   : 1   ; // [21] 
        unsigned int    scl2_auto_clk_gt_en   : 1   ; // [22] 
        unsigned int    scl3_auto_clk_gt_en   : 1   ; // [23] 
        unsigned int    ctran1_auto_clk_gt_en  : 1   ; // [24] 
        unsigned int    ctran2_auto_clk_gt_en  : 1   ; // [25] 
        unsigned int    ctran3_auto_clk_gt_en  : 1   ; // [26] 
        unsigned int    ctran4_auto_clk_gt_en  : 1   ; // [27] 
        unsigned int    ctran5_auto_clk_gt_en  : 1   ; // [28] 
        unsigned int    ctran6_auto_clk_gt_en  : 1   ; // [29] 
        unsigned int    rot_auto_clk_gt_en    : 1   ; // [30] 
        unsigned int    Reserved_58           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_AUTO_CLK_GT_EN0;

// Define the union U_ADE_AUTO_CLK_GT_EN1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cmdq1_auto_clk_gt_en  : 1   ; // [0] 
        unsigned int    cmdq2_auto_clk_gt_en  : 1   ; // [1] 
        unsigned int    Reserved_62           : 1   ; // [2] 
        unsigned int    gamma_auto_clk_gt_en  : 1   ; // [3] 
        unsigned int    dither_auto_clk_gt_en  : 1   ; // [4] 
        unsigned int    ovly1_auto_clk_gt_en  : 1   ; // [5] 
        unsigned int    ovly2_auto_clk_gt_en  : 1   ; // [6] 
        unsigned int    ovly3_auto_clk_gt_en  : 1   ; // [7] 
        unsigned int    top_auto_clk_gt_en    : 1   ; // [8] 
        unsigned int    Reserved_61           : 23  ; // [31..9] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_AUTO_CLK_GT_EN1;

// Define the union U_ADE_FRM_DISGARD_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dfs_buf_unflow_lev0   : 12  ; // [11..0] 
        unsigned int    Reserved_64           : 4   ; // [15..12] 
        unsigned int    wdma2_disgard_en      : 1   ; // [16] 
        unsigned int    wdma3_disgard_en      : 1   ; // [17] 
        unsigned int    Reserved_63           : 14  ; // [31..18] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_FRM_DISGARD_CTRL;

// Define the union U_ADE_FRM_DISGARD_CNT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wdma2_disgard_cnt     : 16  ; // [15..0] 
        unsigned int    wdma3_disgard_cnt     : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_FRM_DISGARD_CNT;

// Define the union U_ADE_RELOAD_DIS0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_rdma_reload_dis   : 1   ; // [0] 
        unsigned int    ch2_rdma_reload_dis   : 1   ; // [1] 
        unsigned int    ch3_rdma_reload_dis   : 1   ; // [2] 
        unsigned int    ch4_rdma_reload_dis   : 1   ; // [3] 
        unsigned int    ch5_rdma_reload_dis   : 1   ; // [4] 
        unsigned int    ch6_rdma_reload_dis   : 1   ; // [5] 
        unsigned int    disp_rdma_reload_dis  : 1   ; // [6] 
        unsigned int    cmdq1_rdma_reload_dis  : 1   ; // [7] 
        unsigned int    cmdq2_rdma_reload_dis  : 1   ; // [8] 
        unsigned int    Reserved_67           : 1   ; // [9] 
        unsigned int    ch1_wdma_reload_dis   : 1   ; // [10] 
        unsigned int    ch2_wdma_reload_dis   : 1   ; // [11] 
        unsigned int    ch3_wdma_reload_dis   : 1   ; // [12] 
        unsigned int    Reserved_66           : 1   ; // [13] 
        unsigned int    cmdq_wdma_reload_dis  : 1   ; // [14] 
        unsigned int    clip1_reload_dis      : 1   ; // [15] 
        unsigned int    clip2_reload_dis      : 1   ; // [16] 
        unsigned int    clip3_reload_dis      : 1   ; // [17] 
        unsigned int    clip4_reload_dis      : 1   ; // [18] 
        unsigned int    clip5_reload_dis      : 1   ; // [19] 
        unsigned int    clip6_reload_dis      : 1   ; // [20] 
        unsigned int    scl1_reload_dis       : 1   ; // [21] 
        unsigned int    scl2_reload_dis       : 1   ; // [22] 
        unsigned int    scl3_reload_dis       : 1   ; // [23] 
        unsigned int    ctran1_reload_dis     : 1   ; // [24] 
        unsigned int    ctran2_reload_dis     : 1   ; // [25] 
        unsigned int    ctran3_reload_dis     : 1   ; // [26] 
        unsigned int    ctran4_reload_dis     : 1   ; // [27] 
        unsigned int    ctran5_reload_dis     : 1   ; // [28] 
        unsigned int    ctran6_reload_dis     : 1   ; // [29] 
        unsigned int    rot_reload_dis        : 1   ; // [30] 
        unsigned int    Reserved_65           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_RELOAD_DIS0;

// Define the union U_ADE_RELOAD_DIS1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cmdq1_reload_dis      : 1   ; // [0] 
        unsigned int    cmdq2_reload_dis      : 1   ; // [1] 
        unsigned int    Reserved_69           : 1   ; // [2] 
        unsigned int    gamma_reload_dis      : 1   ; // [3] 
        unsigned int    dither_reload_dis     : 1   ; // [4] 
        unsigned int    ovly1_reload_dis      : 1   ; // [5] 
        unsigned int    ovly2_reload_dis      : 1   ; // [6] 
        unsigned int    ovly3_reload_dis      : 1   ; // [7] 
        unsigned int    Reserved_68           : 24  ; // [31..8] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_RELOAD_DIS1;

// Define the union U_ADE_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ade_en                : 1   ; // [0] 
        unsigned int    Reserved_70           : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_EN;

// Define the union U_RD_CH1_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch1_qos            : 4   ; // [3..0] 
        unsigned int    rd_ch1_qos_sec        : 4   ; // [7..4] 
        unsigned int    rd_ch1_qos_thd        : 4   ; // [11..8] 
        unsigned int    rd_ch1_qos_cfg        : 1   ; // [12] 
        unsigned int    Reserved_77           : 11  ; // [23..13] 
        unsigned int    rd_ch1_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_76           : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH1_PE;

// Define the union U_RD_CH1_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch1_partial        : 2   ; // [1..0] 
        unsigned int    rd_ch1_rd_dir         : 2   ; // [3..2] 
        unsigned int    rd_ch1_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch1_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_82           : 10  ; // [15..6] 
        unsigned int    rd_ch1_format         : 5   ; // [20..16] 
        unsigned int    Reserved_81           : 3   ; // [23..21] 
        unsigned int    rd_ch1_rot            : 3   ; // [26..24] 
        unsigned int    Reserved_80           : 1   ; // [27] 
        unsigned int    Reserved_79           : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH1_CTRL;

// Define the union U_RD_CH1_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch1_width          : 16  ; // [15..0] 
        unsigned int    rd_ch1_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH1_SIZE;

// Define the union U_RD_CH1_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch1_stride         : 16  ; // [15..0] 
        unsigned int    rd_ch1_stride_dir     : 1   ; // [16] 
        unsigned int    Reserved_83           : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH1_STRIDE;

// Define the union U_RD_CH1_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch1_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch1_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH1_PARTIAL_SIZE;

// Define the union U_RD_CH1_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch1_en             : 1   ; // [0] 
        unsigned int    Reserved_84           : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH1_EN;

// Define the union U_RD_CH1_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch1_axi_status     : 1   ; // [0] 
        unsigned int    rd_ch1_status         : 1   ; // [1] 
        unsigned int    Reserved_85           : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH1_STATUS;

// Define the union U_RD_CH2_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch2_qos            : 4   ; // [3..0] 
        unsigned int    rd_ch2_qos_sec        : 4   ; // [7..4] 
        unsigned int    rd_ch2_qos_thd        : 4   ; // [11..8] 
        unsigned int    rd_ch2_qos_cfg        : 1   ; // [12] 
        unsigned int    Reserved_87           : 11  ; // [23..13] 
        unsigned int    rd_ch2_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_86           : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH2_PE;

// Define the union U_RD_CH2_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch2_partial        : 2   ; // [1..0] 
        unsigned int    rd_ch2_rd_dir         : 2   ; // [3..2] 
        unsigned int    rd_ch2_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch2_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_92           : 10  ; // [15..6] 
        unsigned int    rd_ch2_format         : 5   ; // [20..16] 
        unsigned int    Reserved_91           : 3   ; // [23..21] 
        unsigned int    Reserved_90           : 3   ; // [26..24] 
        unsigned int    Reserved_89           : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH2_CTRL;

// Define the union U_RD_CH2_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch2_width          : 16  ; // [15..0] 
        unsigned int    rd_ch2_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH2_SIZE;

// Define the union U_RD_CH2_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch2_stride         : 16  ; // [15..0] 
        unsigned int    rd_ch2_stride_dir     : 1   ; // [16] 
        unsigned int    Reserved_93           : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH2_STRIDE;

// Define the union U_RD_CH2_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch2_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch2_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH2_PARTIAL_SIZE;

// Define the union U_RD_CH2_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch2_en             : 1   ; // [0] 
        unsigned int    Reserved_94           : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH2_EN;

// Define the union U_RD_CH2_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch2_axi_status     : 1   ; // [0] 
        unsigned int    rd_ch2_status         : 1   ; // [1] 
        unsigned int    Reserved_95           : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH2_STATUS;

// Define the union U_RD_CH3_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch3_qos            : 4   ; // [3..0] 
        unsigned int    rd_ch3_qos_sec        : 4   ; // [7..4] 
        unsigned int    rd_ch3_qos_thd        : 4   ; // [11..8] 
        unsigned int    rd_ch3_qos_cfg        : 1   ; // [12] 
        unsigned int    Reserved_97           : 11  ; // [23..13] 
        unsigned int    rd_ch3_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_96           : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH3_PE;

// Define the union U_RD_CH3_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch3_partial        : 2   ; // [1..0] 
        unsigned int    rd_ch3_rd_dir         : 2   ; // [3..2] 
        unsigned int    rd_ch3_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch3_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_102          : 10  ; // [15..6] 
        unsigned int    rd_ch3_format         : 5   ; // [20..16] 
        unsigned int    Reserved_101          : 3   ; // [23..21] 
        unsigned int    Reserved_100          : 3   ; // [26..24] 
        unsigned int    Reserved_99           : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH3_CTRL;

// Define the union U_RD_CH3_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch3_width          : 16  ; // [15..0] 
        unsigned int    rd_ch3_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH3_SIZE;

// Define the union U_RD_CH3_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch3_stride         : 16  ; // [15..0] 
        unsigned int    rd_ch3_stride_dir     : 1   ; // [16] 
        unsigned int    Reserved_103          : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH3_STRIDE;

// Define the union U_RD_CH3_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch3_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch3_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH3_PARTIAL_SIZE;

// Define the union U_RD_CH3_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch3_en             : 1   ; // [0] 
        unsigned int    Reserved_104          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH3_EN;

// Define the union U_RD_CH3_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch3_axi_status     : 1   ; // [0] 
        unsigned int    rd_ch3_status         : 1   ; // [1] 
        unsigned int    Reserved_105          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH3_STATUS;

// Define the union U_RD_CH4_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch4_qos            : 4   ; // [3..0] 
        unsigned int    rd_ch4_qos_sec        : 4   ; // [7..4] 
        unsigned int    rd_ch4_qos_thd        : 4   ; // [11..8] 
        unsigned int    rd_ch4_qos_cfg        : 1   ; // [12] 
        unsigned int    Reserved_107          : 11  ; // [23..13] 
        unsigned int    rd_ch4_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_106          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH4_PE;

// Define the union U_RD_CH4_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch4_partial        : 2   ; // [1..0] 
        unsigned int    rd_ch4_rd_dir         : 2   ; // [3..2] 
        unsigned int    rd_ch4_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch4_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_112          : 10  ; // [15..6] 
        unsigned int    rd_ch4_format         : 5   ; // [20..16] 
        unsigned int    Reserved_111          : 3   ; // [23..21] 
        unsigned int    Reserved_110          : 3   ; // [26..24] 
        unsigned int    Reserved_109          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH4_CTRL;

// Define the union U_RD_CH4_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch4_width          : 16  ; // [15..0] 
        unsigned int    rd_ch4_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH4_SIZE;

// Define the union U_RD_CH4_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch4_stride         : 16  ; // [15..0] 
        unsigned int    rd_ch4_stride_dir     : 1   ; // [16] 
        unsigned int    Reserved_113          : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH4_STRIDE;

// Define the union U_RD_CH4_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch4_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch4_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH4_PARTIAL_SIZE;

// Define the union U_RD_CH4_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch4_en             : 1   ; // [0] 
        unsigned int    Reserved_114          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH4_EN;

// Define the union U_RD_CH4_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch4_axi_status     : 1   ; // [0] 
        unsigned int    rd_ch4_status         : 1   ; // [1] 
        unsigned int    Reserved_115          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH4_STATUS;

// Define the union U_RD_CH5_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_qos            : 4   ; // [3..0] 
        unsigned int    rd_ch5_qos_sec        : 4   ; // [7..4] 
        unsigned int    rd_ch5_qos_thd        : 4   ; // [11..8] 
        unsigned int    rd_ch5_qos_cfg        : 1   ; // [12] 
        unsigned int    Reserved_117          : 11  ; // [23..13] 
        unsigned int    rd_ch5_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_116          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_PE;

// Define the union U_RD_CH5_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_partial        : 2   ; // [1..0] 
        unsigned int    rd_ch5_rd_dir         : 2   ; // [3..2] 
        unsigned int    rd_ch5_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch5_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_122          : 10  ; // [15..6] 
        unsigned int    rd_ch5_format         : 5   ; // [20..16] 
        unsigned int    Reserved_121          : 3   ; // [23..21] 
        unsigned int    rd_ch5_rot            : 3   ; // [26..24] 
        unsigned int    Reserved_120          : 1   ; // [27] 
        unsigned int    Reserved_119          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_CTRL;

// Define the union U_RD_CH5_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_width          : 16  ; // [15..0] 
        unsigned int    rd_ch5_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_SIZE;

// Define the union U_RD_CH5_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_stride         : 16  ; // [15..0] 
        unsigned int    rd_ch5_stride_dir     : 1   ; // [16] 
        unsigned int    Reserved_123          : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_STRIDE;

// Define the union U_RD_CH5_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch5_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_PARTIAL_SIZE;

// Define the union U_RD_CH5_UV_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_uv_width       : 16  ; // [15..0] 
        unsigned int    rd_ch5_uv_height      : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_UV_SIZE;

// Define the union U_RD_CH5_UV_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_uv_stride      : 16  ; // [15..0] 
        unsigned int    rd_ch5_uv_stride_dir  : 1   ; // [16] 
        unsigned int    Reserved_124          : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_UV_STRIDE;

// Define the union U_RD_CH5_UV_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_uv_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch5_uv_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_UV_PARTIAL_SIZE;

// Define the union U_RD_CH5_EXTRA_PARTIAL_WIDTH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_extra_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch5_uv_extra_partial_width  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_EXTRA_PARTIAL_WIDTH;

// Define the union U_RD_CH5_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_en             : 1   ; // [0] 
        unsigned int    Reserved_125          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_EN;

// Define the union U_RD_CH5_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch5_axi_status     : 1   ; // [0] 
        unsigned int    rd_ch5_status         : 1   ; // [1] 
        unsigned int    Reserved_126          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH5_STATUS;

// Define the union U_RD_CH6_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_qos            : 4   ; // [3..0] 
        unsigned int    rd_ch6_qos_sec        : 4   ; // [7..4] 
        unsigned int    rd_ch6_qos_thd        : 4   ; // [11..8] 
        unsigned int    rd_ch6_qos_cfg        : 1   ; // [12] 
        unsigned int    Reserved_128          : 11  ; // [23..13] 
        unsigned int    rd_ch6_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_127          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_PE;

// Define the union U_RD_CH6_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_partial        : 2   ; // [1..0] 
        unsigned int    rd_ch6_rd_dir         : 2   ; // [3..2] 
        unsigned int    rd_ch6_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch6_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_133          : 10  ; // [15..6] 
        unsigned int    rd_ch6_format         : 5   ; // [20..16] 
        unsigned int    Reserved_132          : 3   ; // [23..21] 
        unsigned int    rd_ch6_rot            : 3   ; // [26..24] 
        unsigned int    Reserved_131          : 1   ; // [27] 
        unsigned int    Reserved_130          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_CTRL;

// Define the union U_RD_CH6_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_width          : 16  ; // [15..0] 
        unsigned int    rd_ch6_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_SIZE;

// Define the union U_RD_CH6_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_stride         : 16  ; // [15..0] 
        unsigned int    rd_ch6_stride_dir     : 1   ; // [16] 
        unsigned int    Reserved_134          : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_STRIDE;

// Define the union U_RD_CH6_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch6_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_PARTIAL_SIZE;

// Define the union U_RD_CH6_UV_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_uv_width       : 16  ; // [15..0] 
        unsigned int    rd_ch6_uv_height      : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_UV_SIZE;

// Define the union U_RD_CH6_UV_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_uv_stride      : 16  ; // [15..0] 
        unsigned int    rd_ch6_uv_stride_dir  : 1   ; // [16] 
        unsigned int    Reserved_135          : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_UV_STRIDE;

// Define the union U_RD_CH6_UV_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_uv_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch6_uv_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_UV_PARTIAL_SIZE;

// Define the union U_RD_CH6_EXTRA_PARTIAL_WIDTH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_extra_partial_width  : 16  ; // [15..0] 
        unsigned int    rd_ch6_uv_extra_partial_width  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_EXTRA_PARTIAL_WIDTH;

// Define the union U_RD_CH6_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_en             : 1   ; // [0] 
        unsigned int    Reserved_136          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_EN;

// Define the union U_RD_CH6_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch6_axi_status     : 1   ; // [0] 
        unsigned int    rd_ch6_status         : 1   ; // [1] 
        unsigned int    Reserved_137          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH6_STATUS;

// Define the union U_RD_CH_DISP_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_disp_qos        : 4   ; // [3..0] 
        unsigned int    rd_ch_disp_qos_sec    : 4   ; // [7..4] 
        unsigned int    rd_ch_disp_qos_thd    : 4   ; // [11..8] 
        unsigned int    rd_ch_disp_qos_cfg    : 1   ; // [12] 
        unsigned int    Reserved_139          : 11  ; // [23..13] 
        unsigned int    rd_ch_disp_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_138          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_DISP_PE;

// Define the union U_RD_CH_DISP_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_disp_partial    : 2   ; // [1..0] 
        unsigned int    rd_ch_disp_rd_dir     : 2   ; // [3..2] 
        unsigned int    rd_ch_disp_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch_disp_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_144          : 10  ; // [15..6] 
        unsigned int    rd_ch_disp_format     : 5   ; // [20..16] 
        unsigned int    Reserved_143          : 3   ; // [23..21] 
        unsigned int    Reserved_142          : 3   ; // [26..24] 
        unsigned int    Reserved_141          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_DISP_CTRL;

// Define the union U_RD_CH_DISP_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_disp_width      : 16  ; // [15..0] 
        unsigned int    rd_ch_disp_height     : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_DISP_SIZE;

// Define the union U_RD_CH_DISP_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_disp_stride     : 16  ; // [15..0] 
        unsigned int    rd_ch_disp_stride_dir  : 1   ; // [16] 
        unsigned int    Reserved_145          : 15  ; // [31..17] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_DISP_STRIDE;

// Define the union U_RD_CH_DISP_BLANK_OFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_disp_blank_offset_width  : 16  ; // [15..0] 
        unsigned int    rd_ch_disp_blank_offset_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_DISP_BLANK_OFFSET;

// Define the union U_RD_CH_DISP_BLANK_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_disp_blank_width  : 16  ; // [15..0] 
        unsigned int    rd_ch_disp_blank_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_DISP_BLANK_SIZE;

// Define the union U_RD_CH_DISP_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_disp_en         : 1   ; // [0] 
        unsigned int    Reserved_147          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_DISP_EN;

// Define the union U_RD_CH_DISP_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_disp_axi_status  : 1   ; // [0] 
        unsigned int    rd_ch_disp_status     : 1   ; // [1] 
        unsigned int    Reserved_148          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_DISP_STATUS;

// Define the union U_RD_CH_CMDQ1_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_cmdq1_qos       : 4   ; // [3..0] 
        unsigned int    Reserved_150          : 20  ; // [23..4] 
        unsigned int    rd_ch_cmdq1_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_149          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ1_PE;

// Define the union U_RD_CH_CMDQ1_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_153          : 4   ; // [3..0] 
        unsigned int    rd_ch_cmdq1_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch_cmdq1_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_152          : 26  ; // [31..6] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ1_CTRL;

// Define the union U_RD_CH_CMDQ1_LEN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_cmdq1_len       : 16  ; // [15..0] 
        unsigned int    Reserved_154          : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ1_LEN;

// Define the union U_RD_CH_CMDQ1_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_cmdq1_en        : 1   ; // [0] 
        unsigned int    Reserved_155          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ1_EN;

// Define the union U_RD_CH_CMDQ1_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_cmdq1_axi_status  : 1   ; // [0] 
        unsigned int    rd_ch_cmdq1_status    : 1   ; // [1] 
        unsigned int    Reserved_156          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ1_STATUS;

// Define the union U_RD_CH_CMDQ2_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_cmdq2_qos       : 4   ; // [3..0] 
        unsigned int    Reserved_158          : 20  ; // [23..4] 
        unsigned int    rd_ch_cmdq2_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_157          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ2_PE;

// Define the union U_RD_CH_CMDQ2_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_161          : 4   ; // [3..0] 
        unsigned int    rd_ch_cmdq2_axi_fsh_int_disable  : 1   ; // [4] 
        unsigned int    rd_ch_cmdq2_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_160          : 26  ; // [31..6] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ2_CTRL;

// Define the union U_RD_CH_CMDQ2_LEN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_cmdq2_len       : 16  ; // [15..0] 
        unsigned int    Reserved_162          : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ2_LEN;

// Define the union U_RD_CH_CMDQ2_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_cmdq2_en        : 1   ; // [0] 
        unsigned int    Reserved_163          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ2_EN;

// Define the union U_RD_CH_CMDQ2_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rd_ch_cmdq2_axi_status  : 1   ; // [0] 
        unsigned int    rd_ch_cmdq2_status    : 1   ; // [1] 
        unsigned int    Reserved_164          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_RD_CH_CMDQ2_STATUS;

// Define the union U_WR_CH1_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch1_qos            : 4   ; // [3..0] 
        unsigned int    Reserved_166          : 20  ; // [23..4] 
        unsigned int    wr_ch1_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_165          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH1_PE;

// Define the union U_WR_CH1_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch1_partial        : 2   ; // [1..0] 
        unsigned int    wr_ch1_wr_dir         : 2   ; // [3..2] 
        unsigned int    Reserved_172          : 1   ; // [4] 
        unsigned int    wr_ch1_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_171          : 10  ; // [15..6] 
        unsigned int    wr_ch1_format         : 5   ; // [20..16] 
        unsigned int    Reserved_170          : 3   ; // [23..21] 
        unsigned int    Reserved_169          : 3   ; // [26..24] 
        unsigned int    Reserved_168          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH1_CTRL;

// Define the union U_WR_CH1_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch1_width          : 16  ; // [15..0] 
        unsigned int    wr_ch1_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH1_SIZE;

// Define the union U_WR_CH1_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch1_stride         : 16  ; // [15..0] 
        unsigned int    Reserved_173          : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH1_STRIDE;

// Define the union U_WR_CH1_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch1_partial_width  : 16  ; // [15..0] 
        unsigned int    wr_ch1_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH1_PARTIAL_SIZE;

// Define the union U_WR_CH1_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch1_en             : 1   ; // [0] 
        unsigned int    Reserved_174          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH1_EN;

// Define the union U_WR_CH1_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch1_axi_status     : 1   ; // [0] 
        unsigned int    wr_ch1_status         : 1   ; // [1] 
        unsigned int    Reserved_176          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH1_STATUS;

// Define the union U_WR_CH2_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch2_qos            : 4   ; // [3..0] 
        unsigned int    Reserved_178          : 20  ; // [23..4] 
        unsigned int    wr_ch2_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_177          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH2_PE;

// Define the union U_WR_CH2_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch2_partial        : 2   ; // [1..0] 
        unsigned int    wr_ch2_wr_dir         : 2   ; // [3..2] 
        unsigned int    Reserved_184          : 1   ; // [4] 
        unsigned int    wr_ch2_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_183          : 10  ; // [15..6] 
        unsigned int    wr_ch2_format         : 5   ; // [20..16] 
        unsigned int    Reserved_182          : 3   ; // [23..21] 
        unsigned int    Reserved_181          : 3   ; // [26..24] 
        unsigned int    Reserved_180          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH2_CTRL;

// Define the union U_WR_CH2_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch2_width          : 16  ; // [15..0] 
        unsigned int    wr_ch2_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH2_SIZE;

// Define the union U_WR_CH2_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch2_stride         : 16  ; // [15..0] 
        unsigned int    Reserved_185          : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH2_STRIDE;

// Define the union U_WR_CH2_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch2_en             : 1   ; // [0] 
        unsigned int    Reserved_186          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH2_EN;

// Define the union U_WR_CH2_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch2_axi_status     : 1   ; // [0] 
        unsigned int    wr_ch2_status         : 1   ; // [1] 
        unsigned int    Reserved_188          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH2_STATUS;

// Define the union U_WR_CH3_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_qos            : 4   ; // [3..0] 
        unsigned int    Reserved_190          : 20  ; // [23..4] 
        unsigned int    wr_ch3_min_burst_len  : 4   ; // [27..24] 
        unsigned int    Reserved_189          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_PE;

// Define the union U_WR_CH3_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_partial        : 2   ; // [1..0] 
        unsigned int    wr_ch3_wr_dir         : 2   ; // [3..2] 
        unsigned int    Reserved_195          : 1   ; // [4] 
        unsigned int    wr_ch3_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_194          : 10  ; // [15..6] 
        unsigned int    wr_ch3_format         : 5   ; // [20..16] 
        unsigned int    Reserved_193          : 3   ; // [23..21] 
        unsigned int    Reserved_192          : 6   ; // [29..24] 
        unsigned int    wr_ch3_yuv444_trans   : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_CTRL;

// Define the union U_WR_CH3_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_width          : 16  ; // [15..0] 
        unsigned int    wr_ch3_height         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_SIZE;

// Define the union U_WR_CH3_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_stride         : 16  ; // [15..0] 
        unsigned int    Reserved_196          : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_STRIDE;

// Define the union U_WR_CH3_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_partial_width  : 16  ; // [15..0] 
        unsigned int    wr_ch3_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_PARTIAL_SIZE;

// Define the union U_WR_CH3_UV_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_uv_width       : 16  ; // [15..0] 
        unsigned int    wr_ch3_uv_height      : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_UV_SIZE;

// Define the union U_WR_CH3_UV_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_uv_stride      : 16  ; // [15..0] 
        unsigned int    Reserved_197          : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_UV_STRIDE;

// Define the union U_WR_CH3_UV_PARTIAL_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_uv_partial_width  : 16  ; // [15..0] 
        unsigned int    wr_ch3_uv_partial_height  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_UV_PARTIAL_SIZE;

// Define the union U_WR_CH3_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_en             : 1   ; // [0] 
        unsigned int    Reserved_198          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_EN;

// Define the union U_WR_CH3_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch3_axi_status     : 1   ; // [0] 
        unsigned int    wr_ch3_status         : 1   ; // [1] 
        unsigned int    Reserved_199          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH3_STATUS;

// Define the union U_WR_CH_CMDQ_PE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch_cmdq_qos        : 4   ; // [3..0] 
        unsigned int    Reserved_200          : 28  ; // [31..4] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH_CMDQ_PE;

// Define the union U_WR_CH_CMDQ_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_204          : 4   ; // [3..0] 
        unsigned int    Reserved_203          : 1   ; // [4] 
        unsigned int    wr_ch_cmdq_fsh_int_disable  : 1   ; // [5] 
        unsigned int    Reserved_202          : 24  ; // [29..6] 
        unsigned int    wr_ch_cmdq_cmd_mux    : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH_CMDQ_CTRL;

// Define the union U_WR_CH_CMDQ_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch_cmdq_en         : 1   ; // [0] 
        unsigned int    Reserved_205          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH_CMDQ_EN;

// Define the union U_WR_CH_CMDQ_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wr_ch_cmdq_axi_status  : 1   ; // [0] 
        unsigned int    wr_ch_cmdq_status     : 1   ; // [1] 
        unsigned int    Reserved_206          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WR_CH_CMDQ_STATUS;

// Define the union U_ADE_OVLY_ALPHA_ST
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_alpha_value       : 1   ; // [0] 
        unsigned int    ch2_alpha_value       : 1   ; // [1] 
        unsigned int    ch3_alpha_value       : 1   ; // [2] 
        unsigned int    ch4_alpha_value       : 1   ; // [3] 
        unsigned int    ch5_alpha_value       : 1   ; // [4] 
        unsigned int    ch6_alpha_value       : 1   ; // [5] 
        unsigned int    ch7_alpha_value       : 1   ; // [6] 
        unsigned int    ch8_alpha_value       : 1   ; // [7] 
        unsigned int    Reserved_207          : 24  ; // [31..8] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_ALPHA_ST;

// Define the union U_ADE_OVLY_CH1_XY0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_y0                : 12  ; // [11..0] 
        unsigned int    Reserved_210          : 4   ; // [15..12] 
        unsigned int    ch1_x0                : 12  ; // [27..16] 
        unsigned int    Reserved_209          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH1_XY0;

// Define the union U_ADE_OVLY_CH2_XY0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2_y0                : 12  ; // [11..0] 
        unsigned int    Reserved_212          : 4   ; // [15..12] 
        unsigned int    ch2_x0                : 12  ; // [27..16] 
        unsigned int    Reserved_211          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH2_XY0;

// Define the union U_ADE_OVLY_CH3_XY0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch3_y0                : 12  ; // [11..0] 
        unsigned int    Reserved_214          : 4   ; // [15..12] 
        unsigned int    ch3_x0                : 12  ; // [27..16] 
        unsigned int    Reserved_213          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH3_XY0;

// Define the union U_ADE_OVLY_CH4_XY0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch4_y0                : 12  ; // [11..0] 
        unsigned int    Reserved_216          : 4   ; // [15..12] 
        unsigned int    ch4_x0                : 12  ; // [27..16] 
        unsigned int    Reserved_215          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH4_XY0;

// Define the union U_ADE_OVLY_CH5_XY0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch5_y0                : 12  ; // [11..0] 
        unsigned int    Reserved_218          : 4   ; // [15..12] 
        unsigned int    ch5_x0                : 12  ; // [27..16] 
        unsigned int    Reserved_217          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH5_XY0;

// Define the union U_ADE_OVLY_CH6_XY0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch6_y0                : 12  ; // [11..0] 
        unsigned int    Reserved_220          : 4   ; // [15..12] 
        unsigned int    ch6_x0                : 12  ; // [27..16] 
        unsigned int    Reserved_219          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH6_XY0;

// Define the union U_ADE_OVLY_CH7_XY0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch7_y0                : 12  ; // [11..0] 
        unsigned int    Reserved_222          : 4   ; // [15..12] 
        unsigned int    ch7_x0                : 12  ; // [27..16] 
        unsigned int    Reserved_221          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH7_XY0;

// Define the union U_ADE_OVLY_CH8_XY0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch8_y0                : 12  ; // [11..0] 
        unsigned int    Reserved_224          : 4   ; // [15..12] 
        unsigned int    ch8_x0                : 12  ; // [27..16] 
        unsigned int    Reserved_223          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH8_XY0;

// Define the union U_ADE_OVLY_CH1_XY1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_y1                : 12  ; // [11..0] 
        unsigned int    Reserved_226          : 4   ; // [15..12] 
        unsigned int    ch1_x1                : 12  ; // [27..16] 
        unsigned int    Reserved_225          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH1_XY1;

// Define the union U_ADE_OVLY_CH2_XY1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2_y1                : 12  ; // [11..0] 
        unsigned int    Reserved_228          : 4   ; // [15..12] 
        unsigned int    ch2_x1                : 12  ; // [27..16] 
        unsigned int    Reserved_227          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH2_XY1;

// Define the union U_ADE_OVLY_CH3_XY1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch3_y1                : 12  ; // [11..0] 
        unsigned int    Reserved_230          : 4   ; // [15..12] 
        unsigned int    ch3_x1                : 12  ; // [27..16] 
        unsigned int    Reserved_229          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH3_XY1;

// Define the union U_ADE_OVLY_CH4_XY1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch4_y1                : 12  ; // [11..0] 
        unsigned int    Reserved_232          : 4   ; // [15..12] 
        unsigned int    ch4_x1                : 12  ; // [27..16] 
        unsigned int    Reserved_231          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH4_XY1;

// Define the union U_ADE_OVLY_CH5_XY1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch5_y1                : 12  ; // [11..0] 
        unsigned int    Reserved_234          : 4   ; // [15..12] 
        unsigned int    ch5_x1                : 12  ; // [27..16] 
        unsigned int    Reserved_233          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH5_XY1;

// Define the union U_ADE_OVLY_CH6_XY1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch6_y1                : 12  ; // [11..0] 
        unsigned int    Reserved_236          : 4   ; // [15..12] 
        unsigned int    ch6_x1                : 12  ; // [27..16] 
        unsigned int    Reserved_235          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH6_XY1;

// Define the union U_ADE_OVLY_CH7_XY1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch7_y1                : 12  ; // [11..0] 
        unsigned int    Reserved_238          : 4   ; // [15..12] 
        unsigned int    ch7_x1                : 12  ; // [27..16] 
        unsigned int    Reserved_237          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH7_XY1;

// Define the union U_ADE_OVLY_CH8_XY1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch8_y1                : 12  ; // [11..0] 
        unsigned int    Reserved_240          : 4   ; // [15..12] 
        unsigned int    ch8_x1                : 12  ; // [27..16] 
        unsigned int    Reserved_239          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH8_XY1;

// Define the union U_ADE_OVLY_CH1_XY2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_y2                : 12  ; // [11..0] 
        unsigned int    Reserved_242          : 4   ; // [15..12] 
        unsigned int    ch1_x2                : 12  ; // [27..16] 
        unsigned int    Reserved_241          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH1_XY2;

// Define the union U_ADE_OVLY_CH1_XY3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_y3                : 12  ; // [11..0] 
        unsigned int    Reserved_244          : 4   ; // [15..12] 
        unsigned int    ch1_x3                : 12  ; // [27..16] 
        unsigned int    Reserved_243          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH1_XY3;

// Define the union U_ADE_OVLY_CH1_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1_alp_mode          : 2   ; // [1..0] 
        unsigned int    ch1_alp_sel           : 2   ; // [3..2] 
        unsigned int    ch1_under_alp_sel     : 2   ; // [5..4] 
        unsigned int    ch1_en                : 1   ; // [6] 
        unsigned int    Reserved_247          : 1   ; // [7] 
        unsigned int    ch1_alp_gbl           : 8   ; // [15..8] 
        unsigned int    ch1_under_alp_gbl     : 8   ; // [23..16] 
        unsigned int    ch1_dimming_en        : 1   ; // [24] 
        unsigned int    Reserved_246          : 3   ; // [27..25] 
        unsigned int    ch1_sel               : 3   ; // [30..28] 
        unsigned int    Reserved_245          : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH1_CTL;

// Define the union U_ADE_OVLY_CH2_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2_alp_mode          : 2   ; // [1..0] 
        unsigned int    ch2_alp_sel           : 2   ; // [3..2] 
        unsigned int    ch2_under_alp_sel     : 2   ; // [5..4] 
        unsigned int    ch2_en                : 1   ; // [6] 
        unsigned int    Reserved_250          : 1   ; // [7] 
        unsigned int    ch2_alp_gbl           : 8   ; // [15..8] 
        unsigned int    ch2_under_alp_gbl     : 8   ; // [23..16] 
        unsigned int    ch2_dimming_en        : 1   ; // [24] 
        unsigned int    Reserved_249          : 3   ; // [27..25] 
        unsigned int    ch2_sel               : 3   ; // [30..28] 
        unsigned int    Reserved_248          : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH2_CTL;

// Define the union U_ADE_OVLY_CH3_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch3_alp_mode          : 2   ; // [1..0] 
        unsigned int    ch3_alp_sel           : 2   ; // [3..2] 
        unsigned int    ch3_under_alp_sel     : 2   ; // [5..4] 
        unsigned int    ch3_en                : 1   ; // [6] 
        unsigned int    Reserved_253          : 1   ; // [7] 
        unsigned int    ch3_alp_gbl           : 8   ; // [15..8] 
        unsigned int    ch3_under_alp_gbl     : 8   ; // [23..16] 
        unsigned int    ch3_dimming_en        : 1   ; // [24] 
        unsigned int    Reserved_252          : 3   ; // [27..25] 
        unsigned int    ch3_sel               : 3   ; // [30..28] 
        unsigned int    Reserved_251          : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH3_CTL;

// Define the union U_ADE_OVLY_CH4_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch4_alp_mode          : 2   ; // [1..0] 
        unsigned int    ch4_alp_sel           : 2   ; // [3..2] 
        unsigned int    ch4_under_alp_sel     : 2   ; // [5..4] 
        unsigned int    ch4_en                : 1   ; // [6] 
        unsigned int    Reserved_256          : 1   ; // [7] 
        unsigned int    ch4_alp_gbl           : 8   ; // [15..8] 
        unsigned int    ch4_under_alp_gbl     : 8   ; // [23..16] 
        unsigned int    ch4_dimming_en        : 1   ; // [24] 
        unsigned int    Reserved_255          : 3   ; // [27..25] 
        unsigned int    ch4_sel               : 3   ; // [30..28] 
        unsigned int    Reserved_254          : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH4_CTL;

// Define the union U_ADE_OVLY_CH5_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch5_alp_mode          : 2   ; // [1..0] 
        unsigned int    ch5_alp_sel           : 2   ; // [3..2] 
        unsigned int    ch5_under_alp_sel     : 2   ; // [5..4] 
        unsigned int    ch5_en                : 1   ; // [6] 
        unsigned int    Reserved_259          : 1   ; // [7] 
        unsigned int    ch5_alp_gbl           : 8   ; // [15..8] 
        unsigned int    ch5_under_alp_gbl     : 8   ; // [23..16] 
        unsigned int    ch5_dimming_en        : 1   ; // [24] 
        unsigned int    Reserved_258          : 3   ; // [27..25] 
        unsigned int    ch5_sel               : 3   ; // [30..28] 
        unsigned int    Reserved_257          : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH5_CTL;

// Define the union U_ADE_OVLY_CH6_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch6_alp_mode          : 2   ; // [1..0] 
        unsigned int    ch6_alp_sel           : 2   ; // [3..2] 
        unsigned int    ch6_under_alp_sel     : 2   ; // [5..4] 
        unsigned int    ch6_en                : 1   ; // [6] 
        unsigned int    Reserved_262          : 1   ; // [7] 
        unsigned int    ch6_alp_gbl           : 8   ; // [15..8] 
        unsigned int    ch6_under_alp_gbl     : 8   ; // [23..16] 
        unsigned int    ch6_dimming_en        : 1   ; // [24] 
        unsigned int    Reserved_261          : 3   ; // [27..25] 
        unsigned int    ch6_sel               : 3   ; // [30..28] 
        unsigned int    Reserved_260          : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH6_CTL;

// Define the union U_ADE_OVLY_CH7_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch7_alp_mode          : 2   ; // [1..0] 
        unsigned int    ch7_alp_sel           : 2   ; // [3..2] 
        unsigned int    ch7_under_alp_sel     : 2   ; // [5..4] 
        unsigned int    ch7_en                : 1   ; // [6] 
        unsigned int    Reserved_265          : 1   ; // [7] 
        unsigned int    ch7_alp_gbl           : 8   ; // [15..8] 
        unsigned int    ch7_under_alp_gbl     : 8   ; // [23..16] 
        unsigned int    ch7_dimming_en        : 1   ; // [24] 
        unsigned int    Reserved_264          : 3   ; // [27..25] 
        unsigned int    ch7_sel               : 3   ; // [30..28] 
        unsigned int    Reserved_263          : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH7_CTL;

// Define the union U_ADE_OVLY_CH8_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch8_alp_mode          : 2   ; // [1..0] 
        unsigned int    ch8_alp_sel           : 2   ; // [3..2] 
        unsigned int    ch8_under_alp_sel     : 2   ; // [5..4] 
        unsigned int    ch8_en                : 1   ; // [6] 
        unsigned int    Reserved_268          : 1   ; // [7] 
        unsigned int    ch8_alp_gbl           : 8   ; // [15..8] 
        unsigned int    ch8_under_alp_gbl     : 8   ; // [23..16] 
        unsigned int    ch8_dimming_en        : 1   ; // [24] 
        unsigned int    Reserved_267          : 3   ; // [27..25] 
        unsigned int    ch8_sel               : 3   ; // [30..28] 
        unsigned int    Reserved_266          : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY_CH8_CTL;

// Define the union U_ADE_OVLY1_OUTPUT_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    output_ysize          : 12  ; // [11..0] 
        unsigned int    Reserved_270          : 4   ; // [15..12] 
        unsigned int    output_xsize          : 12  ; // [27..16] 
        unsigned int    Reserved_269          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY1_OUTPUT_SIZE;

// Define the union U_ADE_OVLY2_OUTPUT_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    output_ysize          : 12  ; // [11..0] 
        unsigned int    Reserved_273          : 4   ; // [15..12] 
        unsigned int    output_xsize          : 12  ; // [27..16] 
        unsigned int    Reserved_272          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY2_OUTPUT_SIZE;

// Define the union U_ADE_OVLY3_OUTPUT_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    output_ysize          : 12  ; // [11..0] 
        unsigned int    Reserved_275          : 4   ; // [15..12] 
        unsigned int    output_xsize          : 12  ; // [27..16] 
        unsigned int    Reserved_274          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY3_OUTPUT_SIZE;

// Define the union U_ADE_OVLY1_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovly1_en              : 1   ; // [0] 
        unsigned int    Reserved_276          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY1_CTL;

// Define the union U_ADE_OVLY2_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovly2_en              : 1   ; // [0] 
        unsigned int    Reserved_278          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY2_CTL;

// Define the union U_ADE_OVLY3_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovly3_en              : 1   ; // [0] 
        unsigned int    Reserved_279          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_OVLY3_CTL;

// Define the union U_ADE_SCL1_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    form_i                : 2   ; // [1..0] 
        unsigned int    form_o                : 2   ; // [3..2] 
        unsigned int    hfir_order            : 1   ; // [4] 
        unsigned int    a_scl_en              : 1   ; // [5] 
        unsigned int    scl_en                : 1   ; // [6] 
        unsigned int    vfir_en               : 1   ; // [7] 
        unsigned int    hfir_en               : 1   ; // [8] 
        unsigned int    v_tap                 : 1   ; // [9] 
        unsigned int    bypass                : 1   ; // [10] 
        unsigned int    Reserved_280          : 21  ; // [31..11] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_CTRL;

// Define the union U_ADE_SCL1_HSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    h_step                : 22  ; // [21..0] 
        unsigned int    Reserved_282          : 2   ; // [23..22] 
        unsigned int    h_offset              : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_HSP;

// Define the union U_ADE_SCL1_UV_HSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    h_uv_step             : 22  ; // [21..0] 
        unsigned int    Reserved_283          : 2   ; // [23..22] 
        unsigned int    h_uv_offset           : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_UV_HSP;

// Define the union U_ADE_SCL1_VSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v_step                : 22  ; // [21..0] 
        unsigned int    Reserved_284          : 2   ; // [23..22] 
        unsigned int    v_offset              : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_VSP;

// Define the union U_ADE_SCL1_UV_VSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v_uv_step             : 22  ; // [21..0] 
        unsigned int    Reserved_285          : 2   ; // [23..22] 
        unsigned int    v_uv_offset           : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_UV_VSP;

// Define the union U_ADE_SCL1_ORES
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    width_o               : 12  ; // [11..0] 
        unsigned int    Reserved_287          : 4   ; // [15..12] 
        unsigned int    height_o              : 12  ; // [27..16] 
        unsigned int    Reserved_286          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_ORES;

// Define the union U_ADE_SCL1_IRES
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    width_i               : 12  ; // [11..0] 
        unsigned int    Reserved_289          : 4   ; // [15..12] 
        unsigned int    height_i              : 12  ; // [27..16] 
        unsigned int    Reserved_288          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_IRES;

// Define the union U_ADE_SCL1_START
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    scl_start             : 1   ; // [0] 
        unsigned int    Reserved_290          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_START;

// Define the union U_ADE_SCL1_ERR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_err              : 1   ; // [0] 
        unsigned int    Reserved_291          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_ERR;

// Define the union U_ADE_SCL1_HCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hcoef_m_2n            : 12  ; // [11..0] 
        unsigned int    Reserved_295          : 4   ; // [15..12] 
        unsigned int    hcoef_m_2n_1          : 12  ; // [27..16] 
        unsigned int    Reserved_294          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_HCOEF;

// Define the union U_ADE_SCL1_VCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vcoef_i_2j            : 12  ; // [11..0] 
        unsigned int    Reserved_297          : 4   ; // [15..12] 
        unsigned int    vcoef_i_2j_1          : 12  ; // [27..16] 
        unsigned int    Reserved_296          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL1_VCOEF;

// Define the union U_ADE_SCL2_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    form_i                : 2   ; // [1..0] 
        unsigned int    form_o                : 2   ; // [3..2] 
        unsigned int    hfir_order            : 1   ; // [4] 
        unsigned int    a_scl_en              : 1   ; // [5] 
        unsigned int    scl_en                : 1   ; // [6] 
        unsigned int    vfir_en               : 1   ; // [7] 
        unsigned int    hfir_en               : 1   ; // [8] 
        unsigned int    v_tap                 : 1   ; // [9] 
        unsigned int    bypass                : 1   ; // [10] 
        unsigned int    Reserved_298          : 21  ; // [31..11] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_CTRL;

// Define the union U_ADE_SCL2_HSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    h_step                : 22  ; // [21..0] 
        unsigned int    Reserved_300          : 2   ; // [23..22] 
        unsigned int    h_offset              : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_HSP;

// Define the union U_ADE_SCL2_UV_HSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    h_uv_step             : 22  ; // [21..0] 
        unsigned int    Reserved_301          : 2   ; // [23..22] 
        unsigned int    h_uv_offset           : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_UV_HSP;

// Define the union U_ADE_SCL2_VSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v_step                : 22  ; // [21..0] 
        unsigned int    Reserved_302          : 2   ; // [23..22] 
        unsigned int    v_offset              : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_VSP;

// Define the union U_ADE_SCL2_UV_VSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v_uv_step             : 22  ; // [21..0] 
        unsigned int    Reserved_303          : 2   ; // [23..22] 
        unsigned int    v_uv_offset           : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_UV_VSP;

// Define the union U_ADE_SCL2_ORES
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    width_o               : 12  ; // [11..0] 
        unsigned int    Reserved_305          : 4   ; // [15..12] 
        unsigned int    height_o              : 12  ; // [27..16] 
        unsigned int    Reserved_304          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_ORES;

// Define the union U_ADE_SCL2_IRES
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    width_i               : 12  ; // [11..0] 
        unsigned int    Reserved_307          : 4   ; // [15..12] 
        unsigned int    height_i              : 12  ; // [27..16] 
        unsigned int    Reserved_306          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_IRES;

// Define the union U_ADE_SCL2_START
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    scl_start             : 1   ; // [0] 
        unsigned int    Reserved_308          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_START;

// Define the union U_ADE_SCL2_ERR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_err              : 1   ; // [0] 
        unsigned int    Reserved_309          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_ERR;

// Define the union U_ADE_SCL2_HCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hcoef_m_2n            : 12  ; // [11..0] 
        unsigned int    Reserved_313          : 4   ; // [15..12] 
        unsigned int    hcoef_m_2n_1          : 12  ; // [27..16] 
        unsigned int    Reserved_312          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_HCOEF;

// Define the union U_ADE_SCL2_VCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vcoef_i_2j            : 12  ; // [11..0] 
        unsigned int    Reserved_315          : 4   ; // [15..12] 
        unsigned int    vcoef_i_2j_1          : 12  ; // [27..16] 
        unsigned int    Reserved_314          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL2_VCOEF;

// Define the union U_ADE_SCL3_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    form_i                : 2   ; // [1..0] 
        unsigned int    form_o                : 2   ; // [3..2] 
        unsigned int    hfir_order            : 1   ; // [4] 
        unsigned int    a_scl_en              : 1   ; // [5] 
        unsigned int    scl_en                : 1   ; // [6] 
        unsigned int    vfir_en               : 1   ; // [7] 
        unsigned int    hfir_en               : 1   ; // [8] 
        unsigned int    v_tap                 : 1   ; // [9] 
        unsigned int    bypass                : 1   ; // [10] 
        unsigned int    Reserved_316          : 21  ; // [31..11] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_CTRL;

// Define the union U_ADE_SCL3_HSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    h_step                : 22  ; // [21..0] 
        unsigned int    Reserved_318          : 2   ; // [23..22] 
        unsigned int    h_offset              : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_HSP;

// Define the union U_ADE_SCL3_UV_HSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    h_uv_step             : 22  ; // [21..0] 
        unsigned int    Reserved_319          : 2   ; // [23..22] 
        unsigned int    h_uv_offset           : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_UV_HSP;

// Define the union U_ADE_SCL3_VSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v_step                : 22  ; // [21..0] 
        unsigned int    Reserved_320          : 2   ; // [23..22] 
        unsigned int    v_offset              : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_VSP;

// Define the union U_ADE_SCL3_UV_VSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v_uv_step             : 22  ; // [21..0] 
        unsigned int    Reserved_321          : 2   ; // [23..22] 
        unsigned int    v_uv_offset           : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_UV_VSP;

// Define the union U_ADE_SCL3_ORES
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    width_o               : 12  ; // [11..0] 
        unsigned int    Reserved_323          : 4   ; // [15..12] 
        unsigned int    height_o              : 12  ; // [27..16] 
        unsigned int    Reserved_322          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_ORES;

// Define the union U_ADE_SCL3_IRES
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    width_i               : 12  ; // [11..0] 
        unsigned int    Reserved_325          : 4   ; // [15..12] 
        unsigned int    height_i              : 12  ; // [27..16] 
        unsigned int    Reserved_324          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_IRES;

// Define the union U_ADE_SCL3_START
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    scl_start             : 1   ; // [0] 
        unsigned int    Reserved_326          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_START;

// Define the union U_ADE_SCL3_ERR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_err              : 1   ; // [0] 
        unsigned int    Reserved_327          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_ERR;

// Define the union U_ADE_SCL3_HCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hcoef_m_2n            : 12  ; // [11..0] 
        unsigned int    Reserved_331          : 4   ; // [15..12] 
        unsigned int    hcoef_m_2n_1          : 12  ; // [27..16] 
        unsigned int    Reserved_330          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_HCOEF;

// Define the union U_ADE_SCL3_VCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vcoef_i_2j            : 12  ; // [11..0] 
        unsigned int    Reserved_333          : 4   ; // [15..12] 
        unsigned int    vcoef_i_2j_1          : 12  ; // [27..16] 
        unsigned int    Reserved_332          : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_SCL3_VCOEF;

// Define the union U_ADE_CTRAN1_DIS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_bypass          : 1   ; // [0] 
        unsigned int    Reserved_334          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_DIS;

// Define the union U_ADE_CTRAN1_MODE_CHOOSE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_mode            : 2   ; // [1..0] 
        unsigned int    Reserved_336          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_MODE_CHOOSE;

// Define the union U_ADE_CTRAN1_STAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_idle_indicate   : 1   ; // [0] 
        unsigned int    Reserved_337          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_STAT;

// Define the union U_ADE_CTRAN1_CHDC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_339          : 7   ; // [15..9] 
        unsigned int    ch0dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_338          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CHDC0;

// Define the union U_ADE_CTRAN1_CHDC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_341          : 7   ; // [15..9] 
        unsigned int    ch0dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_340          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CHDC1;

// Define the union U_ADE_CTRAN1_CHDC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_343          : 7   ; // [15..9] 
        unsigned int    ch1dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_342          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CHDC2;

// Define the union U_ADE_CTRAN1_CHDC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_345          : 7   ; // [15..9] 
        unsigned int    ch1dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_344          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CHDC3;

// Define the union U_ADE_CTRAN1_CHDC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_347          : 7   ; // [15..9] 
        unsigned int    ch2dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_346          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CHDC4;

// Define the union U_ADE_CTRAN1_CHDC5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_349          : 7   ; // [15..9] 
        unsigned int    ch2dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_348          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CHDC5;

// Define the union U_ADE_CTRAN1_CSC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc00                 : 13  ; // [12..0] 
        unsigned int    Reserved_351          : 3   ; // [15..13] 
        unsigned int    csc01                 : 13  ; // [28..16] 
        unsigned int    Reserved_350          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CSC0;

// Define the union U_ADE_CTRAN1_CSC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc02                 : 13  ; // [12..0] 
        unsigned int    Reserved_353          : 3   ; // [15..13] 
        unsigned int    csc10                 : 13  ; // [28..16] 
        unsigned int    Reserved_352          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CSC1;

// Define the union U_ADE_CTRAN1_CSC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc11                 : 13  ; // [12..0] 
        unsigned int    Reserved_355          : 3   ; // [15..13] 
        unsigned int    csc12                 : 13  ; // [28..16] 
        unsigned int    Reserved_354          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CSC2;

// Define the union U_ADE_CTRAN1_CSC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc20                 : 13  ; // [12..0] 
        unsigned int    Reserved_357          : 3   ; // [15..13] 
        unsigned int    csc21                 : 13  ; // [28..16] 
        unsigned int    Reserved_356          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CSC3;

// Define the union U_ADE_CTRAN1_CSC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc22                 : 13  ; // [12..0] 
        unsigned int    Reserved_358          : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CSC4;

// Define the union U_ADE_CTRAN1_IMAGE_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    image_size            : 22  ; // [21..0] 
        unsigned int    Reserved_359          : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_IMAGE_SIZE;

// Define the union U_ADE_CTRAN1_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_cfg_ok          : 1   ; // [0] 
        unsigned int    Reserved_360          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN1_CFG_OK;

// Define the union U_ADE_CTRAN2_DIS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_bypass          : 1   ; // [0] 
        unsigned int    Reserved_361          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_DIS;

// Define the union U_ADE_CTRAN2_MODE_CHOOSE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_mode            : 2   ; // [1..0] 
        unsigned int    Reserved_363          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_MODE_CHOOSE;

// Define the union U_ADE_CTRAN2_STAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_idle_indicate   : 1   ; // [0] 
        unsigned int    Reserved_364          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_STAT;

// Define the union U_ADE_CTRAN2_CHDC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_366          : 7   ; // [15..9] 
        unsigned int    ch0dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_365          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CHDC0;

// Define the union U_ADE_CTRAN2_CHDC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_368          : 7   ; // [15..9] 
        unsigned int    ch0dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_367          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CHDC1;

// Define the union U_ADE_CTRAN2_CHDC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_370          : 7   ; // [15..9] 
        unsigned int    ch1dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_369          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CHDC2;

// Define the union U_ADE_CTRAN2_CHDC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_372          : 7   ; // [15..9] 
        unsigned int    ch1dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_371          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CHDC3;

// Define the union U_ADE_CTRAN2_CHDC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_374          : 7   ; // [15..9] 
        unsigned int    ch2dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_373          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CHDC4;

// Define the union U_ADE_CTRAN2_CHDC5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_376          : 7   ; // [15..9] 
        unsigned int    ch2dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_375          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CHDC5;

// Define the union U_ADE_CTRAN2_CSC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc00                 : 13  ; // [12..0] 
        unsigned int    Reserved_378          : 3   ; // [15..13] 
        unsigned int    csc01                 : 13  ; // [28..16] 
        unsigned int    Reserved_377          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CSC0;

// Define the union U_ADE_CTRAN2_CSC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc02                 : 13  ; // [12..0] 
        unsigned int    Reserved_380          : 3   ; // [15..13] 
        unsigned int    csc10                 : 13  ; // [28..16] 
        unsigned int    Reserved_379          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CSC1;

// Define the union U_ADE_CTRAN2_CSC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc11                 : 13  ; // [12..0] 
        unsigned int    Reserved_382          : 3   ; // [15..13] 
        unsigned int    csc12                 : 13  ; // [28..16] 
        unsigned int    Reserved_381          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CSC2;

// Define the union U_ADE_CTRAN2_CSC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc20                 : 13  ; // [12..0] 
        unsigned int    Reserved_384          : 3   ; // [15..13] 
        unsigned int    csc21                 : 13  ; // [28..16] 
        unsigned int    Reserved_383          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CSC3;

// Define the union U_ADE_CTRAN2_CSC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc22                 : 13  ; // [12..0] 
        unsigned int    Reserved_385          : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CSC4;

// Define the union U_ADE_CTRAN2_IMAGE_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    image_size            : 22  ; // [21..0] 
        unsigned int    Reserved_386          : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_IMAGE_SIZE;

// Define the union U_ADE_CTRAN2_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_cfg_ok          : 1   ; // [0] 
        unsigned int    Reserved_387          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN2_CFG_OK;

// Define the union U_ADE_CTRAN3_DIS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_bypass          : 1   ; // [0] 
        unsigned int    Reserved_388          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_DIS;

// Define the union U_ADE_CTRAN3_MODE_CHOOSE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_mode            : 2   ; // [1..0] 
        unsigned int    Reserved_390          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_MODE_CHOOSE;

// Define the union U_ADE_CTRAN3_STAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_idle_indicate   : 1   ; // [0] 
        unsigned int    Reserved_391          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_STAT;

// Define the union U_ADE_CTRAN3_CHDC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_393          : 7   ; // [15..9] 
        unsigned int    ch0dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_392          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CHDC0;

// Define the union U_ADE_CTRAN3_CHDC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_395          : 7   ; // [15..9] 
        unsigned int    ch0dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_394          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CHDC1;

// Define the union U_ADE_CTRAN3_CHDC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_397          : 7   ; // [15..9] 
        unsigned int    ch1dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_396          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CHDC2;

// Define the union U_ADE_CTRAN3_CHDC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_399          : 7   ; // [15..9] 
        unsigned int    ch1dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_398          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CHDC3;

// Define the union U_ADE_CTRAN3_CHDC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_401          : 7   ; // [15..9] 
        unsigned int    ch2dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_400          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CHDC4;

// Define the union U_ADE_CTRAN3_CHDC5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_403          : 7   ; // [15..9] 
        unsigned int    ch2dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_402          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CHDC5;

// Define the union U_ADE_CTRAN3_CSC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc00                 : 13  ; // [12..0] 
        unsigned int    Reserved_405          : 3   ; // [15..13] 
        unsigned int    csc01                 : 13  ; // [28..16] 
        unsigned int    Reserved_404          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CSC0;

// Define the union U_ADE_CTRAN3_CSC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc02                 : 13  ; // [12..0] 
        unsigned int    Reserved_407          : 3   ; // [15..13] 
        unsigned int    csc10                 : 13  ; // [28..16] 
        unsigned int    Reserved_406          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CSC1;

// Define the union U_ADE_CTRAN3_CSC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc11                 : 13  ; // [12..0] 
        unsigned int    Reserved_409          : 3   ; // [15..13] 
        unsigned int    csc12                 : 13  ; // [28..16] 
        unsigned int    Reserved_408          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CSC2;

// Define the union U_ADE_CTRAN3_CSC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc20                 : 13  ; // [12..0] 
        unsigned int    Reserved_411          : 3   ; // [15..13] 
        unsigned int    csc21                 : 13  ; // [28..16] 
        unsigned int    Reserved_410          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CSC3;

// Define the union U_ADE_CTRAN3_CSC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc22                 : 13  ; // [12..0] 
        unsigned int    Reserved_412          : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CSC4;

// Define the union U_ADE_CTRAN3_IMAGE_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    image_size            : 22  ; // [21..0] 
        unsigned int    Reserved_413          : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_IMAGE_SIZE;

// Define the union U_ADE_CTRAN3_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_cfg_ok          : 1   ; // [0] 
        unsigned int    Reserved_414          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN3_CFG_OK;

// Define the union U_ADE_CTRAN4_DIS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_bypass          : 1   ; // [0] 
        unsigned int    Reserved_415          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_DIS;

// Define the union U_ADE_CTRAN4_MODE_CHOOSE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_mode            : 2   ; // [1..0] 
        unsigned int    Reserved_417          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_MODE_CHOOSE;

// Define the union U_ADE_CTRAN4_STAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_idle_indicate   : 1   ; // [0] 
        unsigned int    Reserved_418          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_STAT;

// Define the union U_ADE_CTRAN4_CHDC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_420          : 7   ; // [15..9] 
        unsigned int    ch0dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_419          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CHDC0;

// Define the union U_ADE_CTRAN4_CHDC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_422          : 7   ; // [15..9] 
        unsigned int    ch0dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_421          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CHDC1;

// Define the union U_ADE_CTRAN4_CHDC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_424          : 7   ; // [15..9] 
        unsigned int    ch1dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_423          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CHDC2;

// Define the union U_ADE_CTRAN4_CHDC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_426          : 7   ; // [15..9] 
        unsigned int    ch1dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_425          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CHDC3;

// Define the union U_ADE_CTRAN4_CHDC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_428          : 7   ; // [15..9] 
        unsigned int    ch2dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_427          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CHDC4;

// Define the union U_ADE_CTRAN4_CHDC5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_430          : 7   ; // [15..9] 
        unsigned int    ch2dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_429          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CHDC5;

// Define the union U_ADE_CTRAN4_CSC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc00                 : 13  ; // [12..0] 
        unsigned int    Reserved_432          : 3   ; // [15..13] 
        unsigned int    csc01                 : 13  ; // [28..16] 
        unsigned int    Reserved_431          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CSC0;

// Define the union U_ADE_CTRAN4_CSC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc02                 : 13  ; // [12..0] 
        unsigned int    Reserved_434          : 3   ; // [15..13] 
        unsigned int    csc10                 : 13  ; // [28..16] 
        unsigned int    Reserved_433          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CSC1;

// Define the union U_ADE_CTRAN4_CSC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc11                 : 13  ; // [12..0] 
        unsigned int    Reserved_436          : 3   ; // [15..13] 
        unsigned int    csc12                 : 13  ; // [28..16] 
        unsigned int    Reserved_435          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CSC2;

// Define the union U_ADE_CTRAN4_CSC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc20                 : 13  ; // [12..0] 
        unsigned int    Reserved_438          : 3   ; // [15..13] 
        unsigned int    csc21                 : 13  ; // [28..16] 
        unsigned int    Reserved_437          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CSC3;

// Define the union U_ADE_CTRAN4_CSC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc22                 : 13  ; // [12..0] 
        unsigned int    Reserved_439          : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CSC4;

// Define the union U_ADE_CTRAN4_IMAGE_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    image_size            : 22  ; // [21..0] 
        unsigned int    Reserved_440          : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_IMAGE_SIZE;

// Define the union U_ADE_CTRAN4_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_cfg_ok          : 1   ; // [0] 
        unsigned int    Reserved_441          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN4_CFG_OK;

// Define the union U_ADE_CTRAN5_DIS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_bypass          : 1   ; // [0] 
        unsigned int    Reserved_442          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_DIS;

// Define the union U_ADE_CTRAN5_MODE_CHOOSE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_mode            : 2   ; // [1..0] 
        unsigned int    Reserved_444          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_MODE_CHOOSE;

// Define the union U_ADE_CTRAN5_STAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_idle_indicate   : 1   ; // [0] 
        unsigned int    Reserved_445          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_STAT;

// Define the union U_ADE_CTRAN5_CHDC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_447          : 7   ; // [15..9] 
        unsigned int    ch0dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_446          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CHDC0;

// Define the union U_ADE_CTRAN5_CHDC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_449          : 7   ; // [15..9] 
        unsigned int    ch0dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_448          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CHDC1;

// Define the union U_ADE_CTRAN5_CHDC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_451          : 7   ; // [15..9] 
        unsigned int    ch1dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_450          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CHDC2;

// Define the union U_ADE_CTRAN5_CHDC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_453          : 7   ; // [15..9] 
        unsigned int    ch1dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_452          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CHDC3;

// Define the union U_ADE_CTRAN5_CHDC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_455          : 7   ; // [15..9] 
        unsigned int    ch2dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_454          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CHDC4;

// Define the union U_ADE_CTRAN5_CHDC5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_457          : 7   ; // [15..9] 
        unsigned int    ch2dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_456          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CHDC5;

// Define the union U_ADE_CTRAN5_CSC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc00                 : 13  ; // [12..0] 
        unsigned int    Reserved_459          : 3   ; // [15..13] 
        unsigned int    csc01                 : 13  ; // [28..16] 
        unsigned int    Reserved_458          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CSC0;

// Define the union U_ADE_CTRAN5_CSC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc02                 : 13  ; // [12..0] 
        unsigned int    Reserved_461          : 3   ; // [15..13] 
        unsigned int    csc10                 : 13  ; // [28..16] 
        unsigned int    Reserved_460          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CSC1;

// Define the union U_ADE_CTRAN5_CSC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc11                 : 13  ; // [12..0] 
        unsigned int    Reserved_463          : 3   ; // [15..13] 
        unsigned int    csc12                 : 13  ; // [28..16] 
        unsigned int    Reserved_462          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CSC2;

// Define the union U_ADE_CTRAN5_CSC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc20                 : 13  ; // [12..0] 
        unsigned int    Reserved_465          : 3   ; // [15..13] 
        unsigned int    csc21                 : 13  ; // [28..16] 
        unsigned int    Reserved_464          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CSC3;

// Define the union U_ADE_CTRAN5_CSC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc22                 : 13  ; // [12..0] 
        unsigned int    Reserved_466          : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CSC4;

// Define the union U_ADE_CTRAN5_IMAGE_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    image_size            : 22  ; // [21..0] 
        unsigned int    Reserved_467          : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_IMAGE_SIZE;

// Define the union U_ADE_CTRAN5_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_cfg_ok          : 1   ; // [0] 
        unsigned int    Reserved_468          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN5_CFG_OK;

// Define the union U_ADE_CTRAN6_DIS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_bypass          : 1   ; // [0] 
        unsigned int    Reserved_469          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_DIS;

// Define the union U_ADE_CTRAN6_MODE_CHOOSE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_mode            : 2   ; // [1..0] 
        unsigned int    Reserved_471          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_MODE_CHOOSE;

// Define the union U_ADE_CTRAN6_STAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_idle_indicate   : 1   ; // [0] 
        unsigned int    Reserved_472          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_STAT;

// Define the union U_ADE_CTRAN6_CHDC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_474          : 7   ; // [15..9] 
        unsigned int    ch0dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_473          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CHDC0;

// Define the union U_ADE_CTRAN6_CHDC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch0dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_476          : 7   ; // [15..9] 
        unsigned int    ch0dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_475          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CHDC1;

// Define the union U_ADE_CTRAN6_CHDC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_478          : 7   ; // [15..9] 
        unsigned int    ch1dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_477          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CHDC2;

// Define the union U_ADE_CTRAN6_CHDC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch1dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_480          : 7   ; // [15..9] 
        unsigned int    ch1dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_479          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CHDC3;

// Define the union U_ADE_CTRAN6_CHDC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc0                : 9   ; // [8..0] 
        unsigned int    Reserved_482          : 7   ; // [15..9] 
        unsigned int    ch2dc1                : 9   ; // [24..16] 
        unsigned int    Reserved_481          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CHDC4;

// Define the union U_ADE_CTRAN6_CHDC5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch2dc2                : 9   ; // [8..0] 
        unsigned int    Reserved_484          : 7   ; // [15..9] 
        unsigned int    ch2dc3                : 9   ; // [24..16] 
        unsigned int    Reserved_483          : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CHDC5;

// Define the union U_ADE_CTRAN6_CSC0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc00                 : 13  ; // [12..0] 
        unsigned int    Reserved_486          : 3   ; // [15..13] 
        unsigned int    csc01                 : 13  ; // [28..16] 
        unsigned int    Reserved_485          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CSC0;

// Define the union U_ADE_CTRAN6_CSC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc02                 : 13  ; // [12..0] 
        unsigned int    Reserved_488          : 3   ; // [15..13] 
        unsigned int    csc10                 : 13  ; // [28..16] 
        unsigned int    Reserved_487          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CSC1;

// Define the union U_ADE_CTRAN6_CSC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc11                 : 13  ; // [12..0] 
        unsigned int    Reserved_490          : 3   ; // [15..13] 
        unsigned int    csc12                 : 13  ; // [28..16] 
        unsigned int    Reserved_489          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CSC2;

// Define the union U_ADE_CTRAN6_CSC3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc20                 : 13  ; // [12..0] 
        unsigned int    Reserved_492          : 3   ; // [15..13] 
        unsigned int    csc21                 : 13  ; // [28..16] 
        unsigned int    Reserved_491          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CSC3;

// Define the union U_ADE_CTRAN6_CSC4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc22                 : 13  ; // [12..0] 
        unsigned int    Reserved_493          : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CSC4;

// Define the union U_ADE_CTRAN6_IMAGE_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    image_size            : 22  ; // [21..0] 
        unsigned int    Reserved_494          : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_IMAGE_SIZE;

// Define the union U_ADE_CTRAN6_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ctran_cfg_ok          : 1   ; // [0] 
        unsigned int    Reserved_495          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CTRAN6_CFG_OK;

// Define the union U_ADE_ROT_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rot_angle             : 3   ; // [2..0] 
        unsigned int    Reserved_497          : 1   ; // [3] 
        unsigned int    block_width           : 8   ; // [11..4] 
        unsigned int    Reserved_496          : 20  ; // [31..12] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_ROT_CFG;

// Define the union U_ADE_ROT_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    img_size_hrz          : 12  ; // [11..0] 
        unsigned int    img_size_vrt          : 12  ; // [23..12] 
        unsigned int    Reserved_499          : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_ROT_SIZE;

// Define the union U_ADE_ROT_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rot_en                : 1   ; // [0] 
        unsigned int    Reserved_500          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_ROT_EN;

// Define the union U_ADE_GAMMA_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    gamma_en              : 1   ; // [0] 
        unsigned int    Reserved_502          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_GAMMA_CTL;

// Define the union U_ADE_GAMMA_COEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_506          : 2   ; // [1..0] 
        unsigned int    gamma_b               : 8   ; // [9..2] 
        unsigned int    Reserved_505          : 2   ; // [11..10] 
        unsigned int    gamma_g               : 8   ; // [19..12] 
        unsigned int    Reserved_504          : 2   ; // [21..20] 
        unsigned int    gamma_r               : 8   ; // [29..22] 
        unsigned int    Reserved_503          : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_GAMMA_COEF;

// Define the union U_ADE_CLIP1_DISABLE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_disable          : 1   ; // [0] 
        unsigned int    Reserved_507          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP1_DISABLE;

// Define the union U_ADE_CLIP1_SIZE0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch_rsize_vrt          : 11  ; // [10..0] 
        unsigned int    Reserved_510          : 5   ; // [15..11] 
        unsigned int    ch_rsize_hrz          : 11  ; // [26..16] 
        unsigned int    Reserved_509          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP1_SIZE0;

// Define the union U_ADE_CLIP1_SIZE1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    right_clip            : 11  ; // [10..0] 
        unsigned int    Reserved_512          : 5   ; // [15..11] 
        unsigned int    left_clip             : 11  ; // [26..16] 
        unsigned int    Reserved_511          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP1_SIZE1;

// Define the union U_ADE_CLIP1_SIZE2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bot_clip              : 11  ; // [10..0] 
        unsigned int    Reserved_514          : 5   ; // [15..11] 
        unsigned int    top_clip              : 11  ; // [26..16] 
        unsigned int    Reserved_513          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP1_SIZE2;

// Define the union U_ADE_CLIP1_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_cfg_ok           : 1   ; // [0] 
        unsigned int    Reserved_515          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP1_CFG_OK;

// Define the union U_ADE_CLIP2_DISABLE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_disable          : 1   ; // [0] 
        unsigned int    Reserved_516          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP2_DISABLE;

// Define the union U_ADE_CLIP2_SIZE0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch_rsize_vrt          : 11  ; // [10..0] 
        unsigned int    Reserved_519          : 5   ; // [15..11] 
        unsigned int    ch_rsize_hrz          : 11  ; // [26..16] 
        unsigned int    Reserved_518          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP2_SIZE0;

// Define the union U_ADE_CLIP2_SIZE1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    right_clip            : 11  ; // [10..0] 
        unsigned int    Reserved_521          : 5   ; // [15..11] 
        unsigned int    left_clip             : 11  ; // [26..16] 
        unsigned int    Reserved_520          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP2_SIZE1;

// Define the union U_ADE_CLIP2_SIZE2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bot_clip              : 11  ; // [10..0] 
        unsigned int    Reserved_523          : 5   ; // [15..11] 
        unsigned int    top_clip              : 11  ; // [26..16] 
        unsigned int    Reserved_522          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP2_SIZE2;

// Define the union U_ADE_CLIP2_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_cfg_ok           : 1   ; // [0] 
        unsigned int    Reserved_524          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP2_CFG_OK;

// Define the union U_ADE_CLIP3_DISABLE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_disable          : 1   ; // [0] 
        unsigned int    Reserved_525          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP3_DISABLE;

// Define the union U_ADE_CLIP3_SIZE0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch_rsize_vrt          : 11  ; // [10..0] 
        unsigned int    Reserved_528          : 5   ; // [15..11] 
        unsigned int    ch_rsize_hrz          : 11  ; // [26..16] 
        unsigned int    Reserved_527          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP3_SIZE0;

// Define the union U_ADE_CLIP3_SIZE1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    right_clip            : 11  ; // [10..0] 
        unsigned int    Reserved_530          : 5   ; // [15..11] 
        unsigned int    left_clip             : 11  ; // [26..16] 
        unsigned int    Reserved_529          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP3_SIZE1;

// Define the union U_ADE_CLIP3_SIZE2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bot_clip              : 11  ; // [10..0] 
        unsigned int    Reserved_532          : 5   ; // [15..11] 
        unsigned int    top_clip              : 11  ; // [26..16] 
        unsigned int    Reserved_531          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP3_SIZE2;

// Define the union U_ADE_CLIP3_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_cfg_ok           : 1   ; // [0] 
        unsigned int    Reserved_533          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP3_CFG_OK;

// Define the union U_ADE_CLIP4_DISABLE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_disable          : 1   ; // [0] 
        unsigned int    Reserved_534          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP4_DISABLE;

// Define the union U_ADE_CLIP4_SIZE0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch_rsize_vrt          : 11  ; // [10..0] 
        unsigned int    Reserved_537          : 5   ; // [15..11] 
        unsigned int    ch_rsize_hrz          : 11  ; // [26..16] 
        unsigned int    Reserved_536          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP4_SIZE0;

// Define the union U_ADE_CLIP4_SIZE1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    right_clip            : 11  ; // [10..0] 
        unsigned int    Reserved_539          : 5   ; // [15..11] 
        unsigned int    left_clip             : 11  ; // [26..16] 
        unsigned int    Reserved_538          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP4_SIZE1;

// Define the union U_ADE_CLIP4_SIZE2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bot_clip              : 11  ; // [10..0] 
        unsigned int    Reserved_541          : 5   ; // [15..11] 
        unsigned int    top_clip              : 11  ; // [26..16] 
        unsigned int    Reserved_540          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP4_SIZE2;

// Define the union U_ADE_CLIP4_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_cfg_ok           : 1   ; // [0] 
        unsigned int    Reserved_542          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP4_CFG_OK;

// Define the union U_ADE_CLIP5_DISABLE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_disable          : 1   ; // [0] 
        unsigned int    Reserved_543          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP5_DISABLE;

// Define the union U_ADE_CLIP5_SIZE0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch_rsize_vrt          : 11  ; // [10..0] 
        unsigned int    Reserved_546          : 5   ; // [15..11] 
        unsigned int    ch_rsize_hrz          : 11  ; // [26..16] 
        unsigned int    Reserved_545          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP5_SIZE0;

// Define the union U_ADE_CLIP5_SIZE1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    right_clip            : 11  ; // [10..0] 
        unsigned int    Reserved_548          : 5   ; // [15..11] 
        unsigned int    left_clip             : 11  ; // [26..16] 
        unsigned int    Reserved_547          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP5_SIZE1;

// Define the union U_ADE_CLIP5_SIZE2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bot_clip              : 11  ; // [10..0] 
        unsigned int    Reserved_550          : 5   ; // [15..11] 
        unsigned int    top_clip              : 11  ; // [26..16] 
        unsigned int    Reserved_549          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP5_SIZE2;

// Define the union U_ADE_CLIP5_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_cfg_ok           : 1   ; // [0] 
        unsigned int    Reserved_551          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP5_CFG_OK;

// Define the union U_ADE_CLIP6_DISABLE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_disable          : 1   ; // [0] 
        unsigned int    Reserved_552          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP6_DISABLE;

// Define the union U_ADE_CLIP6_SIZE0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ch_rsize_vrt          : 11  ; // [10..0] 
        unsigned int    Reserved_555          : 5   ; // [15..11] 
        unsigned int    ch_rsize_hrz          : 11  ; // [26..16] 
        unsigned int    Reserved_554          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP6_SIZE0;

// Define the union U_ADE_CLIP6_SIZE1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    right_clip            : 11  ; // [10..0] 
        unsigned int    Reserved_557          : 5   ; // [15..11] 
        unsigned int    left_clip             : 11  ; // [26..16] 
        unsigned int    Reserved_556          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP6_SIZE1;

// Define the union U_ADE_CLIP6_SIZE2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bot_clip              : 11  ; // [10..0] 
        unsigned int    Reserved_559          : 5   ; // [15..11] 
        unsigned int    top_clip              : 11  ; // [26..16] 
        unsigned int    Reserved_558          : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP6_SIZE2;

// Define the union U_ADE_CLIP6_CFG_OK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_cfg_ok           : 1   ; // [0] 
        unsigned int    Reserved_560          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_CLIP6_CFG_OK;

// Define the union U_ADE_DITHER_CTL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dither_type           : 1   ; // [0] 
        unsigned int    g2d_dither_565out     : 1   ; // [1] 
        unsigned int    disp_pix_fmt          : 2   ; // [3..2] 
        unsigned int    dither_en             : 1   ; // [4] 
        unsigned int    Reserved_562          : 1   ; // [5] 
        unsigned int    disp_size_hrz         : 12  ; // [17..6] 
        unsigned int    Reserved_561          : 14  ; // [31..18] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ADE_DITHER_CTL;

// Define the union U_LDI_HRZ_CTRL0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hfp                   : 12  ; // [11..0] 
        unsigned int    Reserved_564          : 8   ; // [19..12] 
        unsigned int    hbp                   : 12  ; // [31..20] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_HRZ_CTRL0;

// Define the union U_LDI_HRZ_CTRL1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsw                   : 12  ; // [11..0] 
        unsigned int    Reserved_566          : 20  ; // [31..12] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_HRZ_CTRL1;

// Define the union U_LDI_VRT_CTRL0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vfp                   : 12  ; // [11..0] 
        unsigned int    Reserved_567          : 8   ; // [19..12] 
        unsigned int    vbp                   : 12  ; // [31..20] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_VRT_CTRL0;

// Define the union U_LDI_VRT_CTRL1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vsw                   : 12  ; // [11..0] 
        unsigned int    Reserved_568          : 20  ; // [31..12] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_VRT_CTRL1;

// Define the union U_LDI_PLR_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vsync_plr             : 1   ; // [0] 
        unsigned int    hsync_plr             : 1   ; // [1] 
        unsigned int    pixel_clk_plr         : 1   ; // [2] 
        unsigned int    data_en_plr           : 1   ; // [3] 
        unsigned int    Reserved_569          : 28  ; // [31..4] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_PLR_CTRL;

// Define the union U_LDI_DSP_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsize                 : 12  ; // [11..0] 
        unsigned int    Reserved_570          : 8   ; // [19..12] 
        unsigned int    vsize                 : 12  ; // [31..20] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_DSP_SIZE;

// Define the union U_LDI_3D_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    active_space          : 12  ; // [11..0] 
        unsigned int    Reserved_571          : 20  ; // [31..12] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_3D_CTRL;

// Define the union U_LDI_INT_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    frame_start_int_en    : 1   ; // [0] 
        unsigned int    frame_end_int_en      : 1   ; // [1] 
        unsigned int    edc_afifo_underflow_int_en  : 1   ; // [2] 
        unsigned int    vsync_int_en          : 1   ; // [3] 
        unsigned int    vfrontporch_int_en    : 1   ; // [4] 
        unsigned int    vfp_last_int_en       : 1   ; // [5] 
        unsigned int    vactive0_start_int_en  : 1   ; // [6] 
        unsigned int    vactive0_end_int_en   : 1   ; // [7] 
        unsigned int    vactive1_start_int_en  : 1   ; // [8] 
        unsigned int    vactive1_end_int_en   : 1   ; // [9] 
        unsigned int    vfrontporch_end_int_en  : 1   ; // [10] 
        unsigned int    dsi_te_tri_int_en     : 1   ; // [11] 
        unsigned int    dsi_te0_pin_int_en    : 1   ; // [12] 
        unsigned int    dsi_te1_pin_int_en    : 1   ; // [13] 
        unsigned int    vactive_line_int_en   : 1   ; // [14] 
        unsigned int    Reserved_572          : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_INT_EN;

// Define the union U_LDI_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ldi_en                : 1   ; // [0] 
        unsigned int    disp_mode_buf         : 1   ; // [1] 
        unsigned int    date_gate_en          : 1   ; // [2] 
        unsigned int    bpp                   : 2   ; // [4..3] 
        unsigned int    wait_vsync_en         : 1   ; // [5] 
        unsigned int    corlorbar_width       : 7   ; // [12..6] 
        unsigned int    bgr                   : 1   ; // [13] 
        unsigned int    color_mode            : 1   ; // [14] 
        unsigned int    shutdown              : 1   ; // [15] 
        unsigned int    vactive_line          : 12  ; // [27..16] 
        unsigned int    ldi_en_self_clr       : 1   ; // [28] 
        unsigned int    Reserved_573          : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_CTRL;

// Define the union U_LDI_ORG_INT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    frame_start_int       : 1   ; // [0] 
        unsigned int    frame_end_int         : 1   ; // [1] 
        unsigned int    edc_afifo_underflow_int  : 1   ; // [2] 
        unsigned int    vsync_int             : 1   ; // [3] 
        unsigned int    vfrontporch_int       : 1   ; // [4] 
        unsigned int    vbackporch_int        : 1   ; // [5] 
        unsigned int    vactive0_start_int    : 1   ; // [6] 
        unsigned int    vactive0_end_int      : 1   ; // [7] 
        unsigned int    vactive1_start_int    : 1   ; // [8] 
        unsigned int    vactive1_end_int      : 1   ; // [9] 
        unsigned int    vfrontporch_end_int   : 1   ; // [10] 
        unsigned int    dsi_te_tri_int        : 1   ; // [11] 
        unsigned int    dsi_te0_pin_int       : 1   ; // [12] 
        unsigned int    dsi_te1_pin_int       : 1   ; // [13] 
        unsigned int    vactive_line_int      : 1   ; // [14] 
        unsigned int    Reserved_574          : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_ORG_INT;

// Define the union U_LDI_MSK_INT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    frame_start_int       : 1   ; // [0] 
        unsigned int    frame_end_int         : 1   ; // [1] 
        unsigned int    edc_afifo_underflow_int  : 1   ; // [2] 
        unsigned int    vsync_int             : 1   ; // [3] 
        unsigned int    vfrontporch_int       : 1   ; // [4] 
        unsigned int    vbackport_int         : 1   ; // [5] 
        unsigned int    vactive0_start_int    : 1   ; // [6] 
        unsigned int    vactive0_end_int      : 1   ; // [7] 
        unsigned int    vactive1_start_int    : 1   ; // [8] 
        unsigned int    vactive1_end_int      : 1   ; // [9] 
        unsigned int    vfrontporch_end_int   : 1   ; // [10] 
        unsigned int    dsi_te_tri_int        : 1   ; // [11] 
        unsigned int    dsi_te0_pin_int       : 1   ; // [12] 
        unsigned int    dsi_te1_pin_int       : 1   ; // [13] 
        unsigned int    vactive_line_int      : 1   ; // [14] 
        unsigned int    Reserved_575          : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_MSK_INT;

// Define the union U_LDI_INT_CLR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    frame_start_int_clr   : 1   ; // [0] 
        unsigned int    frame_end_int_clr     : 1   ; // [1] 
        unsigned int    edc_afifo_underflow_int_clr  : 1   ; // [2] 
        unsigned int    vsync_int_clr         : 1   ; // [3] 
        unsigned int    vbackporch_int_clr    : 1   ; // [4] 
        unsigned int    vfrontporch_int_clr   : 1   ; // [5] 
        unsigned int    vactive0_start_int_clr  : 1   ; // [6] 
        unsigned int    vactive0_end_int_clr  : 1   ; // [7] 
        unsigned int    vactive1_start_int_clr  : 1   ; // [8] 
        unsigned int    vactive1_end_int_clr  : 1   ; // [9] 
        unsigned int    vfrontporch_end_int_clr  : 1   ; // [10] 
        unsigned int    dsi_te_tri_int_clr    : 1   ; // [11] 
        unsigned int    dsi_te0_pin_int_clr   : 1   ; // [12] 
        unsigned int    dsi_te1_pin_int_clr   : 1   ; // [13] 
        unsigned int    vactive_line_int_clr  : 1   ; // [14] 
        unsigned int    Reserved_576          : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_INT_CLR;

// Define the union U_LDI_WORK_MODE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    work_mode             : 1   ; // [0] 
        unsigned int    wback_en              : 1   ; // [1] 
        unsigned int    colorbar_en           : 1   ; // [2] 
        unsigned int    Reserved_577          : 29  ; // [31..3] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_WORK_MODE;

// Define the union U_LDI_HDMI_DSI_GT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsi_pclk_off          : 1   ; // [0] 
        unsigned int    hdmi_pclk_off         : 1   ; // [1] 
        unsigned int    Reserved_578          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_HDMI_DSI_GT;

// Define the union U_LDI_DE_SPACE_LOW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    de_space_low          : 1   ; // [0] 
        unsigned int    Reserved_579          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_DE_SPACE_LOW;

// Define the union U_DSI_CMD_MOD_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsi_halt_cmd_en       : 1   ; // [0] 
        unsigned int    dsi_halt_video_en     : 1   ; // [1] 
        unsigned int    dsi_wms_3d_mode       : 1   ; // [2] 
        unsigned int    Reserved_580          : 29  ; // [31..3] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSI_CMD_MOD_CTRL;

// Define the union U_DSI_TE_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsi_te_hard_en        : 1   ; // [0] 
        unsigned int    dsi_te0_pin_p         : 1   ; // [1] 
        unsigned int    dsi_te1_pin_p         : 1   ; // [2] 
        unsigned int    dsi_te_hard_sel       : 1   ; // [3] 
        unsigned int    dsi_te_pin_hd_sel     : 1   ; // [4] 
        unsigned int    dsi_te_mask_en        : 1   ; // [5] 
        unsigned int    dsi_te_mask_dis       : 4   ; // [9..6] 
        unsigned int    dsi_te_mask_und       : 4   ; // [13..10] 
        unsigned int    dsi_te_pin_en         : 1   ; // [14] 
        unsigned int    Reserved_581          : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSI_TE_CTRL;

// Define the union U_DSI_TE_HS_NUM
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsi_te0_hs_num        : 13  ; // [12..0] 
        unsigned int    dsi_te1_hs_num        : 13  ; // [25..13] 
        unsigned int    Reserved_582          : 6   ; // [31..26] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSI_TE_HS_NUM;

// Define the union U_DSI_TE_HS_WD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsi_te0_hs_wd         : 12  ; // [11..0] 
        unsigned int    dsi_te1_hs_wd         : 12  ; // [23..12] 
        unsigned int    Reserved_583          : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSI_TE_HS_WD;

// Define the union U_DSI_TE_VS_WD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsi_te0_vs_wd         : 12  ; // [11..0] 
        unsigned int    dsi_te1_vs_wd         : 12  ; // [23..12] 
        unsigned int    Reserved_584          : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSI_TE_VS_WD;

// Define the union U_LDI_MCU_INTS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mcu_frm_end_ints      : 1   ; // [0] 
        unsigned int    Reserved_585          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_MCU_INTS;

// Define the union U_LDI_MCU_INTE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mcu_frm_end_int_en    : 1   ; // [0] 
        unsigned int    Reserved_586          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_MCU_INTE;

// Define the union U_LDI_MCU_INTC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mcu_frm_end_clr       : 1   ; // [0] 
        unsigned int    Reserved_587          : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_LDI_MCU_INTC;

//==============================================================================






#endif // __ADE_UNION_DEFINE_H__

