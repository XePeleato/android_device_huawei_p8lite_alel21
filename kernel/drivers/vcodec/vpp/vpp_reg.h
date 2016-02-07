/*
 **************************************************************************************
 *
 *       Filename:  vpp_reg.h
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011-08-13
 *        Author:   j00140427
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#ifndef __VPP_REG_H__
#define __VPP_REG_H__

// Define the union U_VOCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    arb_mode              : 4   ; // [3..0]
        unsigned int    outstd_rid1           : 4   ; // [7..4]
        unsigned int    outstd_rid0           : 4   ; // [11..8]
        unsigned int    vo_id_sel             : 1   ; // [12]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    outstd_wid0           : 4   ; // [19..16]
        unsigned int    bus_dbg_en            : 2   ; // [21..20]
        unsigned int    Reserved_0            : 9   ; // [30..22]
        unsigned int    vo_ck_gt_en           : 1   ; // [31..22]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOCTRL;

// Define the union U_VOINTSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsdvtthd1_int         : 1   ; // [0]
        unsigned int    dsdvtthd2_int         : 1   ; // [1]
        unsigned int    dsdvtthd3_int         : 1   ; // [2]
        unsigned int    dsduf_int             : 1   ; // [3]
        unsigned int    dhdvtthd1_int         : 1   ; // [4]
        unsigned int    dhdvtthd2_int         : 1   ; // [5]
        unsigned int    dhdvtthd3_int         : 1   ; // [6]
        unsigned int    dhduf_int             : 1   ; // [7]
        unsigned int    wte_int               : 1   ; // [8]
        unsigned int    vte_int               : 1   ; // [9]
        unsigned int    Reserved_3            : 1   ; // [10]
        unsigned int    wbc2_te_int           : 1   ; // [11]
        unsigned int    lnk_tend_int          : 1   ; // [12]
        unsigned int    Reserved_2            : 7   ; // [19..13]
        unsigned int    vsdrr_int             : 1   ; // [20]
        unsigned int    vadrr_int             : 1   ; // [21]
        unsigned int    vhdrr_int             : 1   ; // [22]
        unsigned int    g0rr_int              : 1   ; // [23]
        unsigned int    g1rr_int              : 1   ; // [24]
        unsigned int    Reserved_1            : 3   ; // [27..25]
        unsigned int    vhd_regup_err_int     : 1   ; // [28]
        unsigned int    Reserved_0            : 1   ; // [29]
        unsigned int    ut_end_int            : 1   ; // [30]
        unsigned int    be_int                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOINTSTA;

// Define the union U_VOMSKINTSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsdvtthd1_int         : 1   ; // [0]
        unsigned int    dsdvtthd2_int         : 1   ; // [1]
        unsigned int    dsdvtthd3_int         : 1   ; // [2]
        unsigned int    dsduf_int             : 1   ; // [3]
        unsigned int    dhdvtthd1_int         : 1   ; // [4]
        unsigned int    dhdvtthd2_int         : 1   ; // [5]
        unsigned int    dhdvtthd3_int         : 1   ; // [6]
        unsigned int    dhduf_int             : 1   ; // [7]
        unsigned int    wte_int               : 1   ; // [8]
        unsigned int    vte_int               : 1   ; // [9]
        unsigned int    Reserved_3            : 1   ; // [10]
        unsigned int    wbc2_te_int           : 1   ; // [11]
        unsigned int    lnk_tend_int          : 1   ; // [12]
        unsigned int    Reserved_2            : 7   ; // [19..13]
        unsigned int    vsdrr_int             : 1   ; // [20]
        unsigned int    vadrr_int             : 1   ; // [21]
        unsigned int    vhdrr_int             : 1   ; // [22]
        unsigned int    g0rr_int              : 1   ; // [23]
        unsigned int    g1rr_int              : 1   ; // [24]
        unsigned int    Reserved_1            : 3   ; // [27..25]
        unsigned int    vhd_regup_err_int     : 1   ; // [28]
        unsigned int    Reserved_0            : 1   ; // [29]
        unsigned int    ut_end_int            : 1   ; // [30]
        unsigned int    be_int                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOMSKINTSTA;

// Define the union U_VOINTMSK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dsdvtthd1_intmsk      : 1   ; // [0]
        unsigned int    dsdvtthd2_intmsk      : 1   ; // [1]
        unsigned int    dsdvtthd3_intmsk      : 1   ; // [2]
        unsigned int    dsduf_intmsk          : 1   ; // [3]
        unsigned int    dhdvtthd1_intmsk      : 1   ; // [4]
        unsigned int    dhdvtthd2_intmsk      : 1   ; // [5]
        unsigned int    dhdvtthd3_intmsk      : 1   ; // [6]
        unsigned int    dhduf_intmsk          : 1   ; // [7]
        unsigned int    wte_intmsk            : 1   ; // [8]
        unsigned int    vte_intmsk            : 1   ; // [9]
        unsigned int    Reserved_3            : 1   ; // [10]
        unsigned int    wbc2_te_intmsk        : 1   ; // [11]
        unsigned int    lnk_tend_intmsk       : 1   ; // [12]
        unsigned int    Reserved_2            : 7   ; // [19..13]
        unsigned int    vsdrr_intmsk          : 1   ; // [20]
        unsigned int    vadrr_intmsk          : 1   ; // [21]
        unsigned int    vhdrr_intmsk          : 1   ; // [22]
        unsigned int    g0rr_intmsk           : 1   ; // [23]
        unsigned int    g1rr_intmsk           : 1   ; // [24]
        unsigned int    Reserved_1            : 3   ; // [27..25]
        unsigned int    vhd_regup_err_intmsk  : 1   ; // [28]
        unsigned int    Reserved_0            : 1   ; // [29]
        unsigned int    ut_end_intmsk         : 1   ; // [30]
        unsigned int    be_intmsk             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOINTMSK;

// Define the union U_VOUVERSION1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int vouversion0             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOUVERSION1;
// Define the union U_VOUVERSION2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int vouversion1             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOUVERSION2;
// Define the union U_VOMUXDATA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pin_test_data         : 24  ; // [23..0]
        unsigned int    dv_value              : 1   ; // [24]
        unsigned int    hsync_value           : 1   ; // [25]
        unsigned int    vsync_value           : 1   ; // [26]
        unsigned int    pin_test_mode         : 4   ; // [30..27]
        unsigned int    pin_test_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOMUXDATA;

// Define the union U_VOMUX
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac0_sel              : 2   ; // [1..0]
        unsigned int    dac1_sel              : 2   ; // [3..2]
        unsigned int    dac2_sel              : 2   ; // [5..4]
        unsigned int    dac3_sel              : 2   ; // [7..6]
        unsigned int    dac4_sel              : 2   ; // [9..8]
        unsigned int    dac5_sel              : 2   ; // [11..10]
        unsigned int    dsd_to_hd             : 1   ; // [12]
        unsigned int    dhd_to_sd             : 1   ; // [13]
        unsigned int    Reserved_2            : 2   ; // [15..14]
        unsigned int    hdmi_vid              : 3   ; // [18..16]
        unsigned int    Reserved_1            : 1   ; // [19]
        unsigned int    dv_neg                : 1   ; // [20]
        unsigned int    hsync_neg             : 1   ; // [21]
        unsigned int    vsync_neg             : 1   ; // [22]
        unsigned int    Reserved_0            : 1   ; // [23]
        unsigned int    data_sel0             : 2   ; // [25..24]
        unsigned int    data_sel1             : 2   ; // [27..26]
        unsigned int    data_sel2             : 2   ; // [29..28]
        unsigned int    sync_sel              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOMUX;

// Define the union U_VODEBUG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rm_en_chn             : 4   ; // [3..0]
        unsigned int    dhd_ff_info           : 2   ; // [5..4]
        unsigned int    dsd_ff_info           : 2   ; // [7..6]
        unsigned int    wbc0_ff_info          : 2   ; // [9..8]
        unsigned int    wbc1_ff_info          : 2   ; // [11..10]
        unsigned int    wbc2_ff_info          : 2   ; // [13..12]
        unsigned int    wbc_mode              : 4   ; // [17..14]
        unsigned int    node_num              : 6   ; // [23..18]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VODEBUG;

// Define the union U_VOLNKLSTCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    lnk_fst_num           : 8   ; // [7..0]
        unsigned int    lnk_fst_node_num      : 10  ; // [17..8]
        unsigned int    Reserved_0            : 13  ; // [30..18]
        unsigned int    lnk_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOLNKLSTCTRL;

// Define the union U_VOLNKLSTADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int lnk_addr                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOLNKLSTADDR;
// Define the union U_VOLNKLSTSTRT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0            : 31  ; // [30..0]
        unsigned int    lnk_start             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOLNKLSTSTRT;

// Define the union U_VOLNKLSTPENDINGSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int addr_pendig             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOLNKLSTPENDINGSTA;
// Define the union U_VOLNKLSTWORKINGSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int addr_working            : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOLNKLSTWORKINGSTA;
// Define the union U_VOLNKLSTENDSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int addr_end                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOLNKLSTENDSTA;
// Define the union U_VOLNKLSTNUMSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    node_num_pending      : 8   ; // [7..0]
        unsigned int    node_num_working      : 8   ; // [15..8]
        unsigned int    node_num_end          : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOLNKLSTNUMSTA;

// Define the union U_VOPARAUP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vhd_hcoef_upd         : 1   ; // [0]
        unsigned int    vhd_vcoef_upd         : 1   ; // [1]
        unsigned int    vad_hcoef_upd         : 1   ; // [2]
        unsigned int    vad_vcoef_upd         : 1   ; // [3]
        unsigned int    video_acc_upd         : 1   ; // [4]
        unsigned int    dhd_gamma_upd         : 1   ; // [5]
        unsigned int    vsd_hcoef_upd         : 1   ; // [6]
        unsigned int    vsd_vcoef_upd         : 1   ; // [7]
        unsigned int    g0_lut_upd            : 1   ; // [8]
        unsigned int    g1_lut_upd            : 1   ; // [9]
        unsigned int    g0_hcoef_upd          : 1   ; // [10]
        unsigned int    g0_vcoef_upd          : 1   ; // [11]
        unsigned int    g1_hcoef_upd          : 1   ; // [12]
        unsigned int    g1_vcoef_upd          : 1   ; // [13]
        unsigned int    Reserved_0            : 18  ; // [31..14]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOPARAUP;

// Define the union U_VHDHCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDHCOEFAD;
// Define the union U_VHDVCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDVCOEFAD;
// Define the union U_VADHCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADHCOEFAD;
// Define the union U_VADVCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADVCOEFAD;
// Define the union U_ACCAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_ACCAD;
// Define the union U_VSDHCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDHCOEFAD;
// Define the union U_VSDVCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDVCOEFAD;
// Define the union U_G0CLUTAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CLUTAD;
// Define the union U_G1CLUTAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CLUTAD;
// Define the union U_G0HCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0HCOEFAD;
// Define the union U_G0VCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0VCOEFAD;
// Define the union U_G1HCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1HCOEFAD;
// Define the union U_G1VCOEFAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1VCOEFAD;
// Define the union U_DHDGAMMAAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int coef_addr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDGAMMAAD;
// Define the union U_DACCTRL0_2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac0gc                : 6   ; // [5..0]
        unsigned int    dac1gc                : 6   ; // [11..6]
        unsigned int    dac2gc                : 6   ; // [17..12]
        unsigned int    ensc0                 : 1   ; // [18]
        unsigned int    ensc1                 : 1   ; // [19]
        unsigned int    ensc2                 : 1   ; // [20]
        unsigned int    enctr0                : 1   ; // [21]
        unsigned int    enctr1                : 1   ; // [22]
        unsigned int    enctr2                : 1   ; // [23]
        unsigned int    endac0                : 1   ; // [24]
        unsigned int    endac1                : 1   ; // [25]
        unsigned int    endac2                : 1   ; // [26]
        unsigned int    envbg                 : 1   ; // [27]
        unsigned int    enextref              : 1   ; // [28]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DACCTRL0_2;

// Define the union U_DACCTRL3_5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac3gc                : 6   ; // [5..0]
        unsigned int    dac4gc                : 6   ; // [11..6]
        unsigned int    dac5gc                : 6   ; // [17..12]
        unsigned int    ensc3                 : 1   ; // [18]
        unsigned int    ensc4                 : 1   ; // [19]
        unsigned int    ensc5                 : 1   ; // [20]
        unsigned int    enctr3                : 1   ; // [21]
        unsigned int    enctr4                : 1   ; // [22]
        unsigned int    enctr5                : 1   ; // [23]
        unsigned int    endac3                : 1   ; // [24]
        unsigned int    endac4                : 1   ; // [25]
        unsigned int    endac5                : 1   ; // [26]
        unsigned int    envbg                 : 1   ; // [27]
        unsigned int    enextref              : 1   ; // [28]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DACCTRL3_5;

// Define the union U_DACSTAT0_2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cableout0             : 1   ; // [0]
        unsigned int    cableout1             : 1   ; // [1]
        unsigned int    cableout2             : 1   ; // [2]
        unsigned int    Reserved_0            : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DACSTAT0_2;

// Define the union U_DACSTAT3_5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cableout3             : 1   ; // [0]
        unsigned int    cableout4             : 1   ; // [1]
        unsigned int    cableout5             : 1   ; // [2]
        unsigned int    Reserved_0            : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DACSTAT3_5;

// Define the union U_DACBANDGAP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bandgap_test          : 4   ; // [3..0]
        unsigned int    bandgap_en            : 1   ; // [4]
        unsigned int    bandgap_bgtrim        : 3   ; // [7..5]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DACBANDGAP;

// Define the union U_VHDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ifmt                  : 4   ; // [3..0]
        unsigned int    time_out              : 4   ; // [7..4]
        unsigned int    swp_chr_flag          : 1   ; // [8]
        unsigned int    Reserved_3            : 3   ; // [11..9]
        unsigned int    chm_rmode             : 2   ; // [13..12]
        unsigned int    lm_rmode              : 2   ; // [15..14]
        unsigned int    bfield_first          : 1   ; // [16]
        unsigned int    Reserved_2            : 1   ; // [17]
        unsigned int    ifir_mode             : 2   ; // [19..18]
        unsigned int    Reserved_1            : 3   ; // [22..20]
        unsigned int    vhd_sta_wr_en         : 1   ; // [23]
        unsigned int    ofl_btm               : 1   ; // [24]
        unsigned int    ofl_inter             : 1   ; // [25]
        unsigned int    wbc1_int_en           : 1   ; // [26]
        unsigned int    wbc1_en               : 1   ; // [27]
        unsigned int    mute_en               : 1   ; // [28]
        unsigned int    Reserved_0            : 1   ; // [29]
        unsigned int    resource_sel          : 1   ; // [30]
        unsigned int    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCTRL;

// Define the union U_VHDUPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDUPD;

// Define the union U_VHDLADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_laddr           : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDLADDR;
// Define the union U_VHDLCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_lcaddr          : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDLCADDR;
// Define the union U_VHDCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_caddr           : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCADDR;
// Define the union U_VHDCCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_ccaddr          : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCCADDR;
// Define the union U_VHDNADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_naddr           : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDNADDR;
// Define the union U_VHDNCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_ncaddr          : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDNCADDR;
// Define the union U_VHDSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    surface_stride        : 16  ; // [15..0]
        unsigned int    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDSTRIDE;

// Define the union U_VHDIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDIRESO;

// Define the union U_VHDLKEY
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    luma_low              : 8   ; // [7..0]
        unsigned int    luma_high             : 8   ; // [15..8]
        unsigned int    luma_mask             : 8   ; // [23..16]
        unsigned int    Reserved_0            : 7   ; // [30..24]
        unsigned int    luma_key_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDLKEY;

// Define the union U_VHDCBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    galpha                : 8   ; // [7..0]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCBMPARA;

// Define the union U_VHDDIEADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int dieaddr                 : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIEADDR;
// Define the union U_VHDWBC1ADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int wbc1addr                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDWBC1ADDR;
// Define the union U_VHDWBC1STRD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wbc1strd              : 16  ; // [15..0]
        unsigned int    req_interval          : 10  ; // [25..16]
        unsigned int    Reserved_0            : 3   ; // [28..26]
        unsigned int    wbc1_wr_mode          : 1   ; //29
        unsigned int    wbc1_dft              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDWBC1STRD;

// Define the union U_VHDDIESTADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int diestaddr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIESTADDR;
// Define the union U_VHDDFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xfpos            : 12  ; // [11..0]
        unsigned int    disp_yfpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDFPOS;

// Define the union U_VHDDLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xlpos            : 12  ; // [11..0]
        unsigned int    disp_ylpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDLPOS;

// Define the union U_VHDVFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xfpos           : 12  ; // [11..0]
        unsigned int    video_yfpos           : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDVFPOS;

// Define the union U_VHDVLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xlpos           : 12  ; // [11..0]
        unsigned int    video_ylpos           : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDVLPOS;

// Define the union U_VHDBK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbk_cr                : 8   ; // [7..0]
        unsigned int    vbk_cb                : 8   ; // [15..8]
        unsigned int    vbk_y                 : 8   ; // [23..16]
        unsigned int    vbk_alpha             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDBK;

// Define the union U_VHDCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 9   ; // [8..0]
        unsigned int    cscidc1               : 9   ; // [17..9]
        unsigned int    cscidc2               : 9   ; // [26..18]
        unsigned int    csc_en                : 1   ; // [27]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCSCIDC;

// Define the union U_VHDCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 9   ; // [8..0]
        unsigned int    cscodc1               : 9   ; // [17..9]
        unsigned int    cscodc2               : 9   ; // [26..18]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCSCODC;

// Define the union U_VHDCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp01                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCSCP0;

// Define the union U_VHDCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp10                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCSCP1;

// Define the union U_VHDCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp12                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCSCP2;

// Define the union U_VHDCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp21                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCSCP3;

// Define the union U_VHDCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 13  ; // [12..0]
        unsigned int    Reserved_0            : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCSCP4;

// Define the union U_VHDACM0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    acm0_en               : 1   ; // [28]
        unsigned int    acm1_en               : 1   ; // [29]
        unsigned int    acm2_en               : 1   ; // [30]
        unsigned int    acm3_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACM0;

// Define the union U_VHDACM1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    acm_test_en           : 1   ; // [20]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACM1;

// Define the union U_VHDACM2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACM2;

// Define the union U_VHDACM3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACM3;

// Define the union U_VHDACM4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACM4;

// Define the union U_VHDACM5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACM5;

// Define the union U_VHDACM6
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACM6;

// Define the union U_VHDACM7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACM7;

// Define the union U_VHDHSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hratio                : 16  ; // [15..0]
        unsigned int    Reserved_2            : 3   ; // [18..16]
        unsigned int    hfir_order            : 1   ; // [19]
        unsigned int    Reserved_1            : 3   ; // [22..20]
        unsigned int    hchfir_en             : 1   ; // [23]
        unsigned int    hlfir_en              : 1   ; // [24]
        unsigned int    Reserved_0            : 3   ; // [27..25]
        unsigned int    hchmid_en             : 1   ; // [28]
        unsigned int    hlmid_en              : 1   ; // [29]
        unsigned int    hchmsc_en             : 1   ; // [30]
        unsigned int    hlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDHSP;

// Define the union U_VHDHLOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hor_loffset           : 17  ; // [16..0]
        unsigned int    Reserved_0            : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDHLOFFSET;

// Define the union U_VHDHCOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hor_coffset           : 17  ; // [16..0]
        unsigned int    Reserved_0            : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDHCOFFSET;

// Define the union U_VHDVSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0            : 23  ; // [22..0]
        unsigned int    vchfir_en             : 1   ; // [23]
        unsigned int    vlfir_en              : 1   ; // [24]
        unsigned int    chroma_type           : 1   ; // [25]
        unsigned int    vsc_chroma_tap        : 1   ; // [26]
        unsigned int    vsc_luma_tap          : 1   ; // [27]
        unsigned int    vchmid_en             : 1   ; // [28]
        unsigned int    vlmid_en              : 1   ; // [29]
        unsigned int    vchmsc_en             : 1   ; // [30]
        unsigned int    vlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDVSP;

// Define the union U_VHDVSR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vratio                : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDVSR;

// Define the union U_VHDVOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vchroma_offset        : 16  ; // [15..0]
        unsigned int    vluma_offset          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDVOFFSET;

// Define the union U_VHDZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDZMEORESO;

// Define the union U_VHDZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDZMEIRESO;

// Define the union U_VHDZMEDBG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    need_rand_range       : 8   ; // [7..0]
        unsigned int    rdy_rand_range        : 8   ; // [15..8]
        unsigned int    need_mode             : 1   ; // [16]
        unsigned int    rdy_mode              : 1   ; // [17]
        unsigned int    Reserved_0            : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDZMEDBG;

// Define the union U_VHDHSP_K3V3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    chfir_in_width_k3v3      : 12  ; // [11..0]
        unsigned int    cvfir_in_height_k3v3     : 12  ; // [23..12]
        unsigned int    vsc_420_k3v3             : 1  ; // [24]
        unsigned int    vsc_copy_k3v3            : 1   ; // [25]
        unsigned int    vsc_core_gt              : 2   ; // [27..26]
        unsigned int    Reserved_0               : 4  ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDHSP_K3V3;

// Define the union U_VHDHSP_K3V3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    zme_out_width_k3v3      : 12  ; // [11..0]
        unsigned int    Reserved_0              : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDHLOFFSET_K3V3;

// Define the union U_VHDACCTHD1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    thd_low               : 10  ; // [9..0]
        unsigned int    thd_high              : 10  ; // [19..10]
        unsigned int    thd_med_low           : 10  ; // [29..20]
        unsigned int    acc_mode              : 1   ; // [30]
        unsigned int    acc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACCTHD1;

// Define the union U_VHDACCTHD2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    thd_med_high          : 10  ; // [9..0]
        unsigned int    acc_multiple          : 8   ; // [17..10]
        unsigned int    acc_rst               : 1   ; // [18]
        unsigned int    Reserved_0            : 13  ; // [31..19]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACCTHD2;

// Define the union U_VHDACCLOWN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACCLOWN;

// Define the union U_VHDACCMEDN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACCMEDN;

// Define the union U_VHDACCHIGHN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACCHIGHN;

// Define the union U_VHDACCMLN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACCMLN;

// Define the union U_VHDACCMHN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACCMHN;

// Define the union U_VHDACC3LOW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_low              : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACC3LOW;

// Define the union U_VHDACC3MED
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_med              : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACC3MED;

// Define the union U_VHDACC3HIGH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_high             : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACC3HIGH;

// Define the union U_VHDACC8MLOW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt8_med_low          : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACC8MLOW;

// Define the union U_VHDACC8MHIGH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt8_med_high         : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACC8MHIGH;

// Define the union U_VHDACCTOTAL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acc_pix_total         : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDACCTOTAL;

// Define the union U_VHDIFIRCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef0                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef1                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDIFIRCOEF01;

// Define the union U_VHDIFIRCOEF23
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef2                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef3                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDIFIRCOEF23;

// Define the union U_VHDIFIRCOEF45
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef4                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef5                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDIFIRCOEF45;

// Define the union U_VHDIFIRCOEF67
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef6                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef7                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDIFIRCOEF67;

// Define the union U_VHDDIECTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 16  ; // [15..0]
        unsigned int    stinfo_stop           : 1   ; // [16]
        unsigned int    die_rst               : 1   ; // [17]
        unsigned int    Reserved_0            : 6   ; // [23..18]
        unsigned int    die_chmmode           : 2   ; // [25..24]
        unsigned int    die_lmmode            : 2   ; // [27..26]
        unsigned int    die_out_sel_c         : 1   ; // [28]
        unsigned int    die_out_sel_l         : 1   ; // [29]
        unsigned int    die_chroma_en         : 1   ; // [30]
        unsigned int    die_luma_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIECTRL;

// Define the union U_VHDDIELMA0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    luma_qtbl1            : 8   ; // [7..0]
        unsigned int    luma_qtbl2            : 8   ; // [15..8]
        unsigned int    luma_qtbl3            : 8   ; // [23..16]
        unsigned int    scale_ratio_ppd       : 6   ; // [29..24]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIELMA0;

// Define the union U_VHDDIELMA1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    luma_reqtbl0          : 8   ; // [7..0]
        unsigned int    luma_reqtbl1          : 8   ; // [15..8]
        unsigned int    luma_reqtbl2          : 8   ; // [23..16]
        unsigned int    luma_reqtbl3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIELMA1;

// Define the union U_VHDDIELMA2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    luma_win_size         : 2   ; // [1..0]
        unsigned int    luma_scesdf_max       : 1   ; // [2]
        unsigned int    die_st_qrst_en        : 1   ; // [3]
        unsigned int    die_st_n_en           : 1   ; // [4]
        unsigned int    die_st_m_en           : 1   ; // [5]
        unsigned int    die_st_l_en           : 1   ; // [6]
        unsigned int    die_st_k_en           : 1   ; // [7]
        unsigned int    luma_qrst_max         : 1   ; // [8]
        unsigned int    luma_mf_max           : 1   ; // [9]
        unsigned int    chroma_mf_max         : 1   ; // [10]
        unsigned int    Reserved_0            : 16  ; // [26..11]
        unsigned int    die_st_mode           : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIELMA2;

// Define the union U_VHDDIEINTEN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dir_inten             : 4   ; // [3..0]
        unsigned int    Reserved_1            : 3   ; // [6..4]
        unsigned int    dir_ck_enh            : 1   ; // [7]
        unsigned int    dir_inten_ver         : 4   ; // [11..8]
        unsigned int    Reserved_0            : 4   ; // [15..12]
        unsigned int    ver_min_inten         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIEINTEN;

// Define the union U_VHDDIESCALE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    range_scale           : 8   ; // [7..0]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIESCALE;

// Define the union U_VHDDIECHECK1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ck_max_range          : 6   ; // [5..0]
        unsigned int    Reserved_2            : 2   ; // [7..6]
        unsigned int    ck_range_gain         : 4   ; // [11..8]
        unsigned int    Reserved_1            : 4   ; // [15..12]
        unsigned int    ck_gain               : 4   ; // [19..16]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIECHECK1;

// Define the union U_VHDDIECHECK2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ck_max_range          : 6   ; // [5..0]
        unsigned int    Reserved_2            : 2   ; // [7..6]
        unsigned int    ck_range_gain         : 4   ; // [11..8]
        unsigned int    Reserved_1            : 4   ; // [15..12]
        unsigned int    ck_gain               : 4   ; // [19..16]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIECHECK2;

// Define the union U_VHDDIEDIR0_3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dir0_mult             : 6   ; // [5..0]
        unsigned int    Reserved_3            : 2   ; // [7..6]
        unsigned int    dir1_mult             : 6   ; // [13..8]
        unsigned int    Reserved_2            : 2   ; // [15..14]
        unsigned int    dir2_mult             : 6   ; // [21..16]
        unsigned int    Reserved_1            : 2   ; // [23..22]
        unsigned int    dir3_mult             : 6   ; // [29..24]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIEDIR0_3;

// Define the union U_VHDDIEDIR4_7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dir4_mult             : 6   ; // [5..0]
        unsigned int    Reserved_3            : 2   ; // [7..6]
        unsigned int    dir5_mult             : 6   ; // [13..8]
        unsigned int    Reserved_2            : 2   ; // [15..14]
        unsigned int    dir6_mult             : 6   ; // [21..16]
        unsigned int    Reserved_1            : 2   ; // [23..22]
        unsigned int    dir7_mult             : 6   ; // [29..24]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIEDIR4_7;

// Define the union U_VHDDIEDIR8_11
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dir8_mult             : 6   ; // [5..0]
        unsigned int    Reserved_3            : 2   ; // [7..6]
        unsigned int    dir9_mult             : 6   ; // [13..8]
        unsigned int    Reserved_2            : 2   ; // [15..14]
        unsigned int    dir10_mult            : 6   ; // [21..16]
        unsigned int    Reserved_1            : 2   ; // [23..22]
        unsigned int    dir11_mult            : 6   ; // [29..24]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIEDIR8_11;

// Define the union U_VHDDIEDIR12_14
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dir12_mult            : 6   ; // [5..0]
        unsigned int    Reserved_2            : 2   ; // [7..6]
        unsigned int    dir13_mult            : 6   ; // [13..8]
        unsigned int    Reserved_1            : 2   ; // [15..14]
        unsigned int    dir14_mult            : 6   ; // [21..16]
        unsigned int    Reserved_0            : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIEDIR12_14;

// Define the union U_VHDDIESTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    die_ref_field         : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIESTA;

// Define the union U_VHDDIESTKADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int die_k_addr              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIESTKADDR;
// Define the union U_VHDDIESTLADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int die_l_addr              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIESTLADDR;
// Define the union U_VHDDIESTMADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int die_m_addr              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIESTMADDR;
// Define the union U_VHDDIESTNADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int die_n_addr              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIESTNADDR;
// Define the union U_VHDDIESTSQTRADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int die_sqtr_addr           : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDDIESTSQTRADDR;
// Define the union U_VHDCCRSCLR0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    no_ccr_detect_thd     : 8   ; // [7..0]
        unsigned int    no_ccr_detect_max     : 8   ; // [15..8]
        unsigned int    chroma_ma_offset      : 8   ; // [23..16]
        unsigned int    chroma_ccr_en         : 1   ; // [24]
        unsigned int    Reserved_0            : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCCRSCLR0;

// Define the union U_VHDCCRSCLR1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    max_xchroma           : 8   ; // [7..0]
        unsigned int    no_ccr_detect_blend   : 4   ; // [11..8]
        unsigned int    Reserved_0            : 4   ; // [15..12]
        unsigned int    similar_thd           : 8   ; // [23..16]
        unsigned int    similar_max           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDCCRSCLR1;

// Define the union U_VHDPDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 20  ; // [19..0]
        unsigned int    edge_smooth_ratio     : 8   ; // [27..20]
        unsigned int    Reserved_0            : 1   ; // [28]
        unsigned int    dir_mch_c             : 1   ; // [29]
        unsigned int    edge_smooth_en        : 1   ; // [30]
        unsigned int    dir_mch_l             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDCTRL;

// Define the union U_VHDPDBLKPOS0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    blk_x                 : 12  ; // [11..0]
        unsigned int    blk_y                 : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDBLKPOS0;

// Define the union U_VHDPDBLKPOS1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    blk_x                 : 12  ; // [11..0]
        unsigned int    blk_y                 : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDBLKPOS1;

// Define the union U_VHDPDBLKTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    stillblk_thd          : 8   ; // [7..0]
        unsigned int    diff_movblk_thd       : 8   ; // [15..8]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDBLKTHD;

// Define the union U_VHDPDHISTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hist_thd0             : 8   ; // [7..0]
        unsigned int    hist_thd1             : 8   ; // [15..8]
        unsigned int    hist_thd2             : 8   ; // [23..16]
        unsigned int    hist_thd3             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDHISTTHD;

// Define the union U_VHDPDUMTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    um_thd0               : 8   ; // [7..0]
        unsigned int    um_thd1               : 8   ; // [15..8]
        unsigned int    um_thd2               : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDUMTHD;

// Define the union U_VHDPDPCCCORING
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coring_tkr            : 8   ; // [7..0]
        unsigned int    coring_norm           : 8   ; // [15..8]
        unsigned int    coring_blk            : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCCORING;

// Define the union U_VHDPDPCCHTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pcc_hthd              : 8   ; // [7..0]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCHTHD;

// Define the union U_VHDPDPCCVTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pcc_vthd0             : 8   ; // [7..0]
        unsigned int    pcc_vthd1             : 8   ; // [15..8]
        unsigned int    pcc_vthd2             : 8   ; // [23..16]
        unsigned int    pcc_vthd3             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCVTHD;

// Define the union U_VHDPDITDIFFVTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    itdiff_vthd0          : 8   ; // [7..0]
        unsigned int    itdiff_vthd1          : 8   ; // [15..8]
        unsigned int    itdiff_vthd2          : 8   ; // [23..16]
        unsigned int    itdiff_vthd3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDITDIFFVTHD;

// Define the union U_VHDPDLASITHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    lasi_thd              : 8   ; // [7..0]
        unsigned int    edge_thd              : 8   ; // [15..8]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDLASITHD;

// Define the union U_VHDPDFRMITDIFF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int frm_it_diff             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDFRMITDIFF;
// Define the union U_VHDPDSTLBLKSAD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int stlblk_sad              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDSTLBLKSAD;
// Define the union U_VHDPDHISTBIN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int hsit_bin                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDHISTBIN;
// Define the union U_VHDPDUMMATCH0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int match_um                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDUMMATCH0;
// Define the union U_VHDPDUMNOMATCH0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int nomatch_um              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDUMNOMATCH0;
// Define the union U_VHDPDUMMATCH1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int match_um                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDUMMATCH1;
// Define the union U_VHDPDUMNOMATCH1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int nomatch_um              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDUMNOMATCH1;
// Define the union U_VHDPDPCCFFWD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_ffwd                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCFFWD;
// Define the union U_VHDPDPCCFWD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_fwd                 : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCFWD;
// Define the union U_VHDPDPCCBWD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_bwd                 : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCBWD;
// Define the union U_VHDPDPCCCRSS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_crss                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCCRSS;
// Define the union U_VHDPDPCCPW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_pw                  : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCPW;
// Define the union U_VHDPDPCCFWDTKR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_fwd_tkr             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCFWDTKR;
// Define the union U_VHDPDPCCBWDTKR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_bwd_tkr             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCBWDTKR;
// Define the union U_VHDPDPCCBLKFWD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_blk_fwd             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCBLKFWD;
// Define the union U_VHDPDPCCBLKBWD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int pcc_blk_bwd             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDPCCBLKBWD;
// Define the union U_VHDPDLASICNT14
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int lasi_cnt                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDLASICNT14;
// Define the union U_VHDPDLASICNT32
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int lasi_cnt                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDLASICNT32;
// Define the union U_VHDPDLASICNT34
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int lasi_cnt                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDPDLASICNT34;
// Define the union U_VADCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ifmt                  : 4   ; // [3..0]
        unsigned int    time_out              : 4   ; // [7..4]
        unsigned int    Reserved_3            : 4   ; // [11..8]
        unsigned int    chm_rmode             : 2   ; // [13..12]
        unsigned int    lm_rmode              : 2   ; // [15..14]
        unsigned int    bfield_first          : 1   ; // [16]
        unsigned int    Reserved_2            : 1   ; // [17]
        unsigned int    ifir_mode             : 2   ; // [19..18]
        unsigned int    Reserved_1            : 8   ; // [27..20]
        unsigned int    mute_en               : 1   ; // [28]
        unsigned int    Reserved_0            : 2   ; // [30..29]
        unsigned int    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCTRL;

// Define the union U_VADUPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADUPD;

// Define the union U_VADCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_caddr           : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCADDR;
// Define the union U_VADCCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_ccaddr          : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCCADDR;
// Define the union U_VADSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    surface_stride        : 16  ; // [15..0]
        unsigned int    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADSTRIDE;

// Define the union U_VADIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADIRESO;

// Define the union U_VADLKEY
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    luma_low              : 8   ; // [7..0]
        unsigned int    luma_high             : 8   ; // [15..8]
        unsigned int    luma_mask             : 8   ; // [23..16]
        unsigned int    Reserved_0            : 7   ; // [30..24]
        unsigned int    luma_key_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADLKEY;

// Define the union U_VADCBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    galpha                : 8   ; // [7..0]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCBMPARA;

// Define the union U_VADDFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xfpos            : 12  ; // [11..0]
        unsigned int    disp_yfpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADDFPOS;

// Define the union U_VADDLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xlpos            : 12  ; // [11..0]
        unsigned int    disp_ylpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADDLPOS;

// Define the union U_VADVFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xfpos           : 12  ; // [11..0]
        unsigned int    video_yfpos           : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADVFPOS;

// Define the union U_VADVLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xlpos           : 12  ; // [11..0]
        unsigned int    video_ylpos           : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADVLPOS;

// Define the union U_VADBK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbk_cr                : 8   ; // [7..0]
        unsigned int    vbk_cb                : 8   ; // [15..8]
        unsigned int    vbk_y                 : 8   ; // [23..16]
        unsigned int    vbk_alpha             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADBK;

// Define the union U_VADCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 9   ; // [8..0]
        unsigned int    cscidc1               : 9   ; // [17..9]
        unsigned int    cscidc2               : 9   ; // [26..18]
        unsigned int    csc_en                : 1   ; // [27]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCSCIDC;

// Define the union U_VADCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 9   ; // [8..0]
        unsigned int    cscodc1               : 9   ; // [17..9]
        unsigned int    cscodc2               : 9   ; // [26..18]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCSCODC;

// Define the union U_VADCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp01                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCSCP0;

// Define the union U_VADCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp10                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCSCP1;

// Define the union U_VADCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp12                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCSCP2;

// Define the union U_VADCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp21                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCSCP3;

// Define the union U_VADCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 13  ; // [12..0]
        unsigned int    Reserved_0            : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADCSCP4;

// Define the union U_VADACM0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    acm0_en               : 1   ; // [28]
        unsigned int    acm1_en               : 1   ; // [29]
        unsigned int    acm2_en               : 1   ; // [30]
        unsigned int    acm3_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACM0;

// Define the union U_VADACM1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    acm_test_en           : 1   ; // [20]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACM1;

// Define the union U_VADACM2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACM2;

// Define the union U_VADACM3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACM3;

// Define the union U_VADACM4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACM4;

// Define the union U_VADACM5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACM5;

// Define the union U_VADACM6
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACM6;

// Define the union U_VADACM7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACM7;

// Define the union U_VADHSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hratio                : 16  ; // [15..0]
        unsigned int    Reserved_2            : 3   ; // [18..16]
        unsigned int    hfir_order            : 1   ; // [19]
        unsigned int    Reserved_1            : 3   ; // [22..20]
        unsigned int    hchfir_en             : 1   ; // [23]
        unsigned int    hlfir_en              : 1   ; // [24]
        unsigned int    Reserved_0            : 3   ; // [27..25]
        unsigned int    hchmid_en             : 1   ; // [28]
        unsigned int    hlmid_en              : 1   ; // [29]
        unsigned int    hchmsc_en             : 1   ; // [30]
        unsigned int    hlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADHSP;

// Define the union U_VADHLOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hor_loffset           : 17  ; // [16..0]
        unsigned int    Reserved_0            : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADHLOFFSET;

// Define the union U_VADHCOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hor_coffset           : 17  ; // [16..0]
        unsigned int    Reserved_0            : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADHCOFFSET;

// Define the union U_VADVSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0            : 23  ; // [22..0]
        unsigned int    vchfir_en             : 1   ; // [23]
        unsigned int    vlfir_en              : 1   ; // [24]
        unsigned int    chroma_type           : 1   ; // [25]
        unsigned int    vsc_chroma_tap        : 1   ; // [26]
        unsigned int    vsc_luma_tap          : 1   ; // [27]
        unsigned int    vchmid_en             : 1   ; // [28]
        unsigned int    vlmid_en              : 1   ; // [29]
        unsigned int    vchmsc_en             : 1   ; // [30]
        unsigned int    vlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADVSP;

// Define the union U_VADVSR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vratio                : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADVSR;

// Define the union U_VADVOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vchroma_offset        : 16  ; // [15..0]
        unsigned int    vluma_offset          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADVOFFSET;

// Define the union U_VADZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADZMEORESO;

// Define the union U_VADZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADZMEIRESO;

// Define the union U_VADZMEDBG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    need_rand_range       : 8   ; // [7..0]
        unsigned int    rdy_rand_range        : 8   ; // [15..8]
        unsigned int    need_mode             : 1   ; // [16]
        unsigned int    rdy_mode              : 1   ; // [17]
        unsigned int    Reserved_0            : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADZMEDBG;

// Define the union U_VADMOSAIC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int Reserved_0              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADMOSAIC;
// Define the union U_VADACCTHD1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    thd_low               : 10  ; // [9..0]
        unsigned int    thd_high              : 10  ; // [19..10]
        unsigned int    thd_med_low           : 10  ; // [29..20]
        unsigned int    acc_mode              : 1   ; // [30]
        unsigned int    acc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACCTHD1;

// Define the union U_VADACCTHD2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    thd_med_high          : 10  ; // [9..0]
        unsigned int    acc_multiple          : 8   ; // [17..10]
        unsigned int    acc_rst               : 1   ; // [18]
        unsigned int    Reserved_0            : 13  ; // [31..19]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACCTHD2;

// Define the union U_VADACCLOWN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACCLOWN;

// Define the union U_VADACCMEDN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACCMEDN;

// Define the union U_VADACCHIGHN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACCHIGHN;

// Define the union U_VADACCMLN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACCMLN;

// Define the union U_VADACCMHN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACCMHN;

// Define the union U_VADACC3LOW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_low              : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACC3LOW;

// Define the union U_VADACC3MED
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_med              : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACC3MED;

// Define the union U_VADACC3HIGH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_high             : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACC3HIGH;

// Define the union U_VADACC8MLOW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt8_med_low          : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACC8MLOW;

// Define the union U_VADACC8MHIGH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt8_med_high         : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACC8MHIGH;

// Define the union U_VADACCTOTAL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acc_pix_total         : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADACCTOTAL;

// Define the union U_VADIFIRCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef0                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef1                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADIFIRCOEF01;

// Define the union U_VADIFIRCOEF23
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef2                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef3                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADIFIRCOEF23;

// Define the union U_VADIFIRCOEF45
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef4                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef5                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADIFIRCOEF45;

// Define the union U_VADIFIRCOEF67
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef6                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef7                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADIFIRCOEF67;

// Define the union U_VSDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ifmt                  : 4   ; // [3..0]
        unsigned int    time_out              : 4   ; // [7..4]
        unsigned int    Reserved_3            : 4   ; // [11..8]
        unsigned int    chm_rmode             : 2   ; // [13..12]
        unsigned int    lm_rmode              : 2   ; // [15..14]
        unsigned int    bfield_first          : 1   ; // [16]
        unsigned int    Reserved_2            : 1   ; // [17]
        unsigned int    ifir_mode             : 2   ; // [19..18]
        unsigned int    Reserved_1            : 8   ; // [27..20]
        unsigned int    mute_en               : 1   ; // [28]
        unsigned int    Reserved_0            : 1   ; // [29]
        unsigned int    resource_sel          : 1   ; // [30]
        unsigned int    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCTRL;

// Define the union U_VSDUPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDUPD;

// Define the union U_VSDCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_caddr           : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCADDR;
// Define the union U_VSDCCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_ccaddr          : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCCADDR;
// Define the union U_VSDSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    surface_stride        : 16  ; // [15..0]
        unsigned int    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDSTRIDE;

// Define the union U_VSDIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDIRESO;

// Define the union U_VSDLKEY
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    luma_low              : 8   ; // [7..0]
        unsigned int    luma_high             : 8   ; // [15..8]
        unsigned int    luma_mask             : 8   ; // [23..16]
        unsigned int    Reserved_0            : 7   ; // [30..24]
        unsigned int    luma_key_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDLKEY;

// Define the union U_VSDCBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    galpha                : 8   ; // [7..0]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCBMPARA;

// Define the union U_VSDDFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xfpos            : 12  ; // [11..0]
        unsigned int    disp_yfpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDDFPOS;

// Define the union U_VSDDLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xlpos            : 12  ; // [11..0]
        unsigned int    disp_ylpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDDLPOS;

// Define the union U_VSDVFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xfpos           : 12  ; // [11..0]
        unsigned int    video_yfpos           : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDVFPOS;

// Define the union U_VSDVLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xlpos           : 12  ; // [11..0]
        unsigned int    video_ylpos           : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDVLPOS;

// Define the union U_VSDBK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbk_cr                : 8   ; // [7..0]
        unsigned int    vbk_cb                : 8   ; // [15..8]
        unsigned int    vbk_y                 : 8   ; // [23..16]
        unsigned int    vbk_alpha             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDBK;

// Define the union U_VSDCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 9   ; // [8..0]
        unsigned int    cscidc1               : 9   ; // [17..9]
        unsigned int    cscidc2               : 9   ; // [26..18]
        unsigned int    csc_en                : 1   ; // [27]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCSCIDC;

// Define the union U_VSDCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 9   ; // [8..0]
        unsigned int    cscodc1               : 9   ; // [17..9]
        unsigned int    cscodc2               : 9   ; // [26..18]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCSCODC;

// Define the union U_VSDCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp01                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCSCP0;

// Define the union U_VSDCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp10                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCSCP1;

// Define the union U_VSDCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp12                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCSCP2;

// Define the union U_VSDCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp21                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCSCP3;

// Define the union U_VSDCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 13  ; // [12..0]
        unsigned int    Reserved_0            : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDCSCP4;

// Define the union U_VSDACM0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    acm0_en               : 1   ; // [28]
        unsigned int    acm1_en               : 1   ; // [29]
        unsigned int    acm2_en               : 1   ; // [30]
        unsigned int    acm3_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACM0;

// Define the union U_VSDACM1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    acm_test_en           : 1   ; // [20]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACM1;

// Define the union U_VSDACM2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACM2;

// Define the union U_VSDACM3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACM3;

// Define the union U_VSDACM4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACM4;

// Define the union U_VSDACM5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACM5;

// Define the union U_VSDACM6
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_u_off           : 5   ; // [4..0]
        unsigned int    acm_b_u_off           : 5   ; // [9..5]
        unsigned int    acm_c_u_off           : 5   ; // [14..10]
        unsigned int    acm_d_u_off           : 5   ; // [19..15]
        unsigned int    acm_fir_blk           : 4   ; // [23..20]
        unsigned int    acm_sec_blk           : 4   ; // [27..24]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACM6;

// Define the union U_VSDACM7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acm_a_v_off           : 5   ; // [4..0]
        unsigned int    acm_b_v_off           : 5   ; // [9..5]
        unsigned int    acm_c_v_off           : 5   ; // [14..10]
        unsigned int    acm_d_v_off           : 5   ; // [19..15]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACM7;

// Define the union U_VSDHSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hratio                : 16  ; // [15..0]
        unsigned int    Reserved_2            : 3   ; // [18..16]
        unsigned int    hfir_order            : 1   ; // [19]
        unsigned int    Reserved_1            : 3   ; // [22..20]
        unsigned int    hchfir_en             : 1   ; // [23]
        unsigned int    hlfir_en              : 1   ; // [24]
        unsigned int    Reserved_0            : 3   ; // [27..25]
        unsigned int    hchmid_en             : 1   ; // [28]
        unsigned int    hlmid_en              : 1   ; // [29]
        unsigned int    hchmsc_en             : 1   ; // [30]
        unsigned int    hlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDHSP;

// Define the union U_VSDHLOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hor_loffset           : 17  ; // [16..0]
        unsigned int    Reserved_0            : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDHLOFFSET;

// Define the union U_VSDHCOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hor_coffset           : 17  ; // [16..0]
        unsigned int    Reserved_0            : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDHCOFFSET;

// Define the union U_VSDVSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0            : 23  ; // [22..0]
        unsigned int    vchfir_en             : 1   ; // [23]
        unsigned int    vlfir_en              : 1   ; // [24]
        unsigned int    chroma_type           : 1   ; // [25]
        unsigned int    vsc_chroma_tap        : 1   ; // [26]
        unsigned int    vsc_luma_tap          : 1   ; // [27]
        unsigned int    vchmid_en             : 1   ; // [28]
        unsigned int    vlmid_en              : 1   ; // [29]
        unsigned int    vchmsc_en             : 1   ; // [30]
        unsigned int    vlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDVSP;

// Define the union U_VSDVSR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vratio                : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDVSR;

// Define the union U_VSDVOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vchroma_offset        : 16  ; // [15..0]
        unsigned int    vluma_offset          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDVOFFSET;

// Define the union U_VSDZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDZMEORESO;

// Define the union U_VSDZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDZMEIRESO;

// Define the union U_VSDZMEDBG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    need_rand_range       : 8   ; // [7..0]
        unsigned int    rdy_rand_range        : 8   ; // [15..8]
        unsigned int    need_mode             : 1   ; // [16]
        unsigned int    rdy_mode              : 1   ; // [17]
        unsigned int    Reserved_0            : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDZMEDBG;

// Define the union U_VSDMOSAIC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int Reserved_0              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDMOSAIC;
// Define the union U_VSDACCTHD1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    thd_low               : 10  ; // [9..0]
        unsigned int    thd_high              : 10  ; // [19..10]
        unsigned int    thd_med_low           : 10  ; // [29..20]
        unsigned int    acc_mode              : 1   ; // [30]
        unsigned int    acc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACCTHD1;

// Define the union U_VSDACCTHD2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    thd_med_high          : 10  ; // [9..0]
        unsigned int    acc_multiple          : 8   ; // [17..10]
        unsigned int    acc_rst               : 1   ; // [18]
        unsigned int    Reserved_0            : 13  ; // [31..19]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACCTHD2;

// Define the union U_VSDACCLOWN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACCLOWN;

// Define the union U_VSDACCMEDN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACCMEDN;

// Define the union U_VSDACCHIGHN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACCHIGHN;

// Define the union U_VSDACCMLN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACCMLN;

// Define the union U_VSDACCMHN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    table_data1n          : 10  ; // [9..0]
        unsigned int    table_data2n          : 10  ; // [19..10]
        unsigned int    table_data3n          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACCMHN;

// Define the union U_VSDACC3LOW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_low              : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACC3LOW;

// Define the union U_VSDACC3MED
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_med              : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACC3MED;

// Define the union U_VSDACC3HIGH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt3_high             : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACC3HIGH;

// Define the union U_VSDACC8MLOW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt8_med_low          : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACC8MLOW;

// Define the union U_VSDACC8MHIGH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cnt8_med_high         : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACC8MHIGH;

// Define the union U_VSDACCTOTAL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    acc_pix_total         : 21  ; // [20..0]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDACCTOTAL;

// Define the union U_VSDIFIRCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef0                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef1                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDIFIRCOEF01;

// Define the union U_VSDIFIRCOEF23
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef2                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef3                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDIFIRCOEF23;

// Define the union U_VSDIFIRCOEF45
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef4                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef5                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDIFIRCOEF45;

// Define the union U_VSDIFIRCOEF67
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef6                 : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef7                 : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDIFIRCOEF67;

// Define the union U_G0CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ifmt                  : 8   ; // [7..0]
        unsigned int    bitext                : 2   ; // [9..8]
        unsigned int    Reserved_1            : 16  ; // [25..10]
        unsigned int    read_mode             : 1   ; // [26]
        unsigned int    upd_mode              : 1   ; // [27]
        unsigned int    mute_en               : 1   ; // [28]
        unsigned int    gmm_en                : 1   ; // [29]
        unsigned int    Reserved_0            : 1   ; // [30]
        unsigned int    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CTRL;

// Define the union U_G0UPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0UPD;

// Define the union U_G0ADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_addr            : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0ADDR;
// Define the union U_G0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    surface_stride        : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0STRIDE;

// Define the union U_G0CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    galpha                : 8   ; // [7..0]
        unsigned int    palpha_range          : 1   ; // [8]
        unsigned int    Reserved_1            : 1   ; // [9]
        unsigned int    vedge_p               : 1   ; // [10]
        unsigned int    hedge_p               : 1   ; // [11]
        unsigned int    palpha_en             : 1   ; // [12]
        unsigned int    premult_en            : 1   ; // [13]
        unsigned int    key_en                : 1   ; // [14]
        unsigned int    key_mode              : 1   ; // [15]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CBMPARA;

// Define the union U_G0CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    keyb_max              : 8   ; // [7..0]
        unsigned int    keyg_max              : 8   ; // [15..8]
        unsigned int    keyr_max              : 8   ; // [23..16]
        unsigned int    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CKEYMAX;

// Define the union U_G0CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    keyb_min              : 8   ; // [7..0]
        unsigned int    keyg_min              : 8   ; // [15..8]
        unsigned int    keyr_min              : 8   ; // [23..16]
        unsigned int    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CKEYMIN;

// Define the union U_G0CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    kmsk_b                : 8   ; // [7..0]
        unsigned int    kmsk_g                : 8   ; // [15..8]
        unsigned int    kmsk_r                : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CMASK;

// Define the union U_G0IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0IRESO;

// Define the union U_G0ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0ORESO;

// Define the union U_G0SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    src_xfpos             : 7   ; // [6..0]
        unsigned int    Reserved_0            : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0SFPOS;

// Define the union U_G0DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xfpos            : 12  ; // [11..0]
        unsigned int    disp_yfpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0DFPOS;

// Define the union U_G0DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xlpos            : 12  ; // [11..0]
        unsigned int    disp_ylpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0DLPOS;

// Define the union U_G0HSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hratio                : 16  ; // [15..0]
        unsigned int    Reserved_2            : 3   ; // [18..16]
        unsigned int    hfir_order            : 1   ; // [19]
        unsigned int    Reserved_1            : 3   ; // [22..20]
        unsigned int    hafir_en              : 1   ; // [23]
        unsigned int    hfir_en               : 1   ; // [24]
        unsigned int    Reserved_0            : 3   ; // [27..25]
        unsigned int    hchmid_en             : 1   ; // [28]
        unsigned int    hlmid_en              : 1   ; // [29]
        unsigned int    hamid_en              : 1   ; // [30]
        unsigned int    hsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0HSP;

// Define the union U_G0HOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hor_coffset           : 16  ; // [15..0]
        unsigned int    hor_loffset           : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0HOFFSET;

// Define the union U_G0VSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_2            : 23  ; // [22..0]
        unsigned int    vafir_en              : 1   ; // [23]
        unsigned int    vfir_en               : 1   ; // [24]
        unsigned int    Reserved_1            : 1   ; // [25]
        unsigned int    Reserved_0            : 1   ; // [26]
        unsigned int    vsc_luma_tap          : 1   ; // [27]
        unsigned int    vchmid_en             : 1   ; // [28]
        unsigned int    vlmid_en              : 1   ; // [29]
        unsigned int    vamid_en              : 1   ; // [30]
        unsigned int    vsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0VSP;

// Define the union U_G0VSR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vratio                : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0VSR;

// Define the union U_G0VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbtm_offset           : 16  ; // [15..0]
        unsigned int    vtp_offset            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0VOFFSET;

// Define the union U_G0ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0ZMEORESO;

// Define the union U_G0ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0ZMEIRESO;

// Define the union U_G0CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 9   ; // [8..0]
        unsigned int    cscidc1               : 9   ; // [17..9]
        unsigned int    cscidc2               : 9   ; // [26..18]
        unsigned int    csc_en                : 1   ; // [27]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CSCIDC;

// Define the union U_G0CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 9   ; // [8..0]
        unsigned int    cscodc1               : 9   ; // [17..9]
        unsigned int    cscodc2               : 9   ; // [26..18]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CSCODC;

// Define the union U_G0CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp01                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CSCP0;

// Define the union U_G0CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp10                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CSCP1;

// Define the union U_G0CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp12                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CSCP2;

// Define the union U_G0CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp21                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CSCP3;

// Define the union U_G0CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 13  ; // [12..0]
        unsigned int    Reserved_0            : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0CSCP4;

// Define the union U_G1CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ifmt                  : 8   ; // [7..0]
        unsigned int    bitext                : 2   ; // [9..8]
        unsigned int    Reserved_0            : 16  ; // [25..10]
        unsigned int    read_mode             : 1   ; // [26]
        unsigned int    upd_mode              : 1   ; // [27]
        unsigned int    mute_en               : 1   ; // [28]
        unsigned int    gmm_en                : 1   ; // [29]
        unsigned int    g1_sel_zme            : 1   ; // [30]
        unsigned int    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CTRL;

// Define the union U_G1UPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1UPD;

// Define the union U_G1ADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_addr            : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1ADDR;
// Define the union U_G1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    surface_stride        : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1STRIDE;

// Define the union U_G1CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    galpha                : 8   ; // [7..0]
        unsigned int    palpha_range          : 1   ; // [8]
        unsigned int    Reserved_1            : 1   ; // [9]
        unsigned int    vedge_p               : 1   ; // [10]
        unsigned int    hedge_p               : 1   ; // [11]
        unsigned int    palpha_en             : 1   ; // [12]
        unsigned int    premult_en            : 1   ; // [13]
        unsigned int    key_en                : 1   ; // [14]
        unsigned int    key_mode              : 1   ; // [15]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CBMPARA;

// Define the union U_G1CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    keyb_max              : 8   ; // [7..0]
        unsigned int    keyg_max              : 8   ; // [15..8]
        unsigned int    keyr_max              : 8   ; // [23..16]
        unsigned int    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CKEYMAX;

// Define the union U_G1CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    keyb_min              : 8   ; // [7..0]
        unsigned int    keyg_min              : 8   ; // [15..8]
        unsigned int    keyr_min              : 8   ; // [23..16]
        unsigned int    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CKEYMIN;

// Define the union U_G1CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    kmsk_b                : 8   ; // [7..0]
        unsigned int    kmsk_g                : 8   ; // [15..8]
        unsigned int    kmsk_r                : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CMASK;

// Define the union U_G1IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1IRESO;

// Define the union U_G1ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1ORESO;

// Define the union U_G1SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    src_xfpos             : 7   ; // [6..0]
        unsigned int    Reserved_0            : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1SFPOS;

// Define the union U_G1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xfpos            : 12  ; // [11..0]
        unsigned int    disp_yfpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1DFPOS;

// Define the union U_G1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xlpos            : 12  ; // [11..0]
        unsigned int    disp_ylpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1DLPOS;

// Define the union U_G1HSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hratio                : 16  ; // [15..0]
        unsigned int    Reserved_2            : 3   ; // [18..16]
        unsigned int    hfir_order            : 1   ; // [19]
        unsigned int    Reserved_1            : 3   ; // [22..20]
        unsigned int    hafir_en              : 1   ; // [23]
        unsigned int    hfir_en               : 1   ; // [24]
        unsigned int    Reserved_0            : 3   ; // [27..25]
        unsigned int    hchmid_en             : 1   ; // [28]
        unsigned int    hlmid_en              : 1   ; // [29]
        unsigned int    hamid_en              : 1   ; // [30]
        unsigned int    hsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1HSP;

// Define the union U_G1HOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hor_coffset           : 16  ; // [15..0]
        unsigned int    hor_loffset           : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1HOFFSET;

// Define the union U_G1VSP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_2            : 23  ; // [22..0]
        unsigned int    vafir_en              : 1   ; // [23]
        unsigned int    vfir_en               : 1   ; // [24]
        unsigned int    Reserved_1            : 1   ; // [25]
        unsigned int    Reserved_0            : 1   ; // [26]
        unsigned int    vsc_luma_tap          : 1   ; // [27]
        unsigned int    vchmid_en             : 1   ; // [28]
        unsigned int    vlmid_en              : 1   ; // [29]
        unsigned int    vamid_en              : 1   ; // [30]
        unsigned int    vsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1VSP;

// Define the union U_G1VSR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vratio                : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1VSR;

// Define the union U_G1VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbtm_offset           : 16  ; // [15..0]
        unsigned int    vtp_offset            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1VOFFSET;

// Define the union U_G1ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1ZMEORESO;

// Define the union U_G1ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1ZMEIRESO;

// Define the union U_G1CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 9   ; // [8..0]
        unsigned int    cscidc1               : 9   ; // [17..9]
        unsigned int    cscidc2               : 9   ; // [26..18]
        unsigned int    csc_en                : 1   ; // [27]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CSCIDC;

// Define the union U_G1CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 9   ; // [8..0]
        unsigned int    cscodc1               : 9   ; // [17..9]
        unsigned int    cscodc2               : 9   ; // [26..18]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CSCODC;

// Define the union U_G1CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp01                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CSCP0;

// Define the union U_G1CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp10                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CSCP1;

// Define the union U_G1CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp12                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CSCP2;

// Define the union U_G1CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp21                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CSCP3;

// Define the union U_G1CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 13  ; // [12..0]
        unsigned int    Reserved_0            : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1CSCP4;

// Define the union U_HCCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ifmt                  : 8   ; // [7..0]
        unsigned int    bitext                : 2   ; // [9..8]
        unsigned int    Reserved_1            : 16  ; // [25..10]
        unsigned int    read_mode             : 1   ; // [26]
        unsigned int    upd_mode              : 1   ; // [27]
        unsigned int    Reserved_0            : 3   ; // [30..28]
        unsigned int    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCTRL;

// Define the union U_HCUPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCUPD;

// Define the union U_HCADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int surface_addr            : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCADDR;
// Define the union U_HCSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    surface_stride        : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCSTRIDE;

// Define the union U_HCCBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    galpha                : 8   ; // [7..0]
        unsigned int    palpha_range          : 1   ; // [8]
        unsigned int    Reserved_2            : 3   ; // [11..9]
        unsigned int    palpha_en             : 1   ; // [12]
        unsigned int    Reserved_1            : 1   ; // [13]
        unsigned int    key_en                : 1   ; // [14]
        unsigned int    key_mode              : 1   ; // [15]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCBMPARA;

// Define the union U_HCCKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    keyb_max              : 8   ; // [7..0]
        unsigned int    keyg_max              : 8   ; // [15..8]
        unsigned int    keyr_max              : 8   ; // [23..16]
        unsigned int    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCKEYMAX;

// Define the union U_HCCKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    keyb_min              : 8   ; // [7..0]
        unsigned int    keyg_min              : 8   ; // [15..8]
        unsigned int    keyr_min              : 8   ; // [23..16]
        unsigned int    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCKEYMIN;

// Define the union U_HCCMASK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    kmsk_b                : 8   ; // [7..0]
        unsigned int    kmsk_g                : 8   ; // [15..8]
        unsigned int    kmsk_r                : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCMASK;

// Define the union U_HCIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0]
        unsigned int    ih                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCIRESO;

// Define the union U_HCSFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    src_xfpos             : 7   ; // [6..0]
        unsigned int    Reserved_0            : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCSFPOS;

// Define the union U_HCDFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xfpos            : 12  ; // [11..0]
        unsigned int    disp_yfpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCDFPOS;

// Define the union U_HCDLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xlpos            : 12  ; // [11..0]
        unsigned int    disp_ylpos            : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCDLPOS;

// Define the union U_HCCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 9   ; // [8..0]
        unsigned int    cscidc1               : 9   ; // [17..9]
        unsigned int    cscidc2               : 9   ; // [26..18]
        unsigned int    csc_en                : 1   ; // [27]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCSCIDC;

// Define the union U_HCCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 9   ; // [8..0]
        unsigned int    cscodc1               : 9   ; // [17..9]
        unsigned int    cscodc2               : 9   ; // [26..18]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCSCODC;

// Define the union U_HCCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp01                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCSCP0;

// Define the union U_HCCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp10                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCSCP1;

// Define the union U_HCCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp12                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCSCP2;

// Define the union U_HCCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp21                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCSCP3;

// Define the union U_HCCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 13  ; // [12..0]
        unsigned int    Reserved_0            : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HCCSCP4;

// Define the union U_WBC0CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    req_interval          : 10  ; // [9..0]
        unsigned int    Reserved_2            : 2   ; // [11..10]
        unsigned int    wbc0_dft              : 2   ; // [13..12]
        unsigned int    Reserved_1            : 10  ; // [23..14]
        unsigned int    dfp_sel               : 3   ; // [26..24]
        unsigned int    wbc0_btm              : 1   ; // [27]
        unsigned int    Reserved_0            : 2   ; // [29..28]
        unsigned int    wbc0_inter            : 1   ; // [30]
        unsigned int    wbc0_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC0CTRL;

// Define the union U_WBC0UPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC0UPD;

// Define the union U_WBC0ADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int wbcaddr                 : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC0ADDR;
// Define the union U_WBC0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wbc0stride            : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC0STRIDE;

// Define the union U_WBC0ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC0ORESO;

// Define the union U_WBC0FCROP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wfcrop                : 12  ; // [11..0]
        unsigned int    hfcrop                : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC0FCROP;

// Define the union U_WBC0LCROP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wlcrop                : 12  ; // [11..0]
        unsigned int    hlcrop                : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC0LCROP;

// Define the union U_WBC2CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    req_interval          : 10  ; // [9..0]
        unsigned int    Reserved_2            : 2   ; // [11..10]
        unsigned int    wbc2_dft              : 2   ; // [13..12]
        unsigned int    Reserved_1            : 13  ; // [26..14]
        unsigned int    wbc2_btm              : 1   ; // [27]
        unsigned int    Reserved_0            : 2   ; // [29..28]
        unsigned int    wbc2_inter            : 1   ; // [30]
        unsigned int    wbc2_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC2CTRL;

// Define the union U_WBC2UPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC2UPD;

// Define the union U_WBC2ADDR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int wbcaddr                 : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC2ADDR;
// Define the union U_WBC2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wbc2stride            : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC2STRIDE;

// Define the union U_WBC2ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0]
        unsigned int    oh                    : 12  ; // [23..12]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_WBC2ORESO;

// Define the union U_CBMBKG1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cbm_bkgcr1            : 8   ; // [7..0]
        unsigned int    cbm_bkgcb1            : 8   ; // [15..8]
        unsigned int    cbm_bkgy1             : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_CBMBKG1;

// Define the union U_CBMBKG2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cbm_bkgcr2            : 8   ; // [7..0]
        unsigned int    cbm_bkgcb2            : 8   ; // [15..8]
        unsigned int    cbm_bkgy2             : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_CBMBKG2;

// Define the union U_CBMBKGV
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cbm_bkgcr_v           : 8   ; // [7..0]
        unsigned int    cbm_bkgcb_v           : 8   ; // [15..8]
        unsigned int    cbm_bkgy_v            : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_CBMBKGV;

// Define the union U_CBMATTR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    sur_attr0             : 1   ; // [0]
        unsigned int    sur_attr1             : 1   ; // [1]
        unsigned int    Reserved_0            : 30  ; // [31..2]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_CBMATTR;

// Define the union U_CBMMIX1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mixer_prio0           : 3   ; // [2..0]
        unsigned int    mixer_prio1           : 3   ; // [5..3]
        unsigned int    mixer_prio2           : 3   ; // [8..6]
        unsigned int    mixer_prio3           : 3   ; // [11..9]
        unsigned int    mixer_prio4           : 3   ; // [14..12]
        unsigned int    Reserved_1            : 1   ; // [15]
        unsigned int    mixer_v_pro0          : 1   ; // [16]
        unsigned int    mixer_v_pro1          : 1   ; // [17]
        unsigned int    bk_v_alpha_sel        : 1   ; // [18]
        unsigned int    mixer_v_en            : 1   ; // [19]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_CBMMIX1;

// Define the union U_CBMMIX2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mixer_prio0           : 3   ; // [2..0]
        unsigned int    mixer_prio1           : 3   ; // [5..3]
        unsigned int    mixer_prio2           : 3   ; // [8..6]
        unsigned int    Reserved_0            : 23  ; // [31..9]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_CBMMIX2;

// Define the union U_DHDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    intfdm                : 4   ; // [3..0]
        unsigned int    intfb                 : 2   ; // [5..4]
        unsigned int    synm                  : 1   ; // [6]
        unsigned int    iop                   : 1   ; // [7]
        unsigned int    ivs                   : 1   ; // [8]
        unsigned int    ihs                   : 1   ; // [9]
        unsigned int    idv                   : 1   ; // [10]
        unsigned int    Reserved_3            : 2   ; // [12..11]
        unsigned int    gmmmode               : 1   ; // [13]
        unsigned int    gmmen                 : 1   ; // [14]
        unsigned int    Reserved_2            : 1   ; // [15]
        unsigned int    clipen                : 1   ; // [16]
        unsigned int    Reserved_1            : 3   ; // [19..17]
        unsigned int    fpga_lmt_width        : 7   ; // [26..20]
        unsigned int    fpga_lmt_en           : 1   ; // [27]
        unsigned int    Reserved_0            : 3   ; // [30..28]
        unsigned int    intf_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCTRL;

// Define the union U_DHDVSYNC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vact                  : 12  ; // [11..0]
        unsigned int    vbb                   : 8   ; // [19..12]
        unsigned int    vfb                   : 8   ; // [27..20]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDVSYNC;

// Define the union U_DHDHSYNC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hact                  : 16  ; // [15..0]
        unsigned int    hbb                   : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSYNC1;

// Define the union U_DHDHSYNC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hfb                   : 16  ; // [15..0]
        unsigned int    hmid                  : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSYNC2;

// Define the union U_DHDVPLUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bvact                 : 12  ; // [11..0]
        unsigned int    bvbb                  : 8   ; // [19..12]
        unsigned int    bvfb                  : 8   ; // [27..20]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDVPLUS;

// Define the union U_DHDPWR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hpw                   : 16  ; // [15..0]
        unsigned int    vpw                   : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDPWR;

// Define the union U_DHDVTTHD3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vtmgthd3              : 13  ; // [12..0]
        unsigned int    Reserved_1            : 2   ; // [14..13]
        unsigned int    thd3_mode             : 1   ; // [15]
        unsigned int    vtmgthd4              : 13  ; // [28..16]
        unsigned int    Reserved_0            : 2   ; // [30..29]
        unsigned int    thd4_mode             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDVTTHD3;

// Define the union U_DHDVTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vtmgthd1              : 13  ; // [12..0]
        unsigned int    Reserved_1            : 2   ; // [14..13]
        unsigned int    thd1_mode             : 1   ; // [15]
        unsigned int    vtmgthd2              : 13  ; // [28..16]
        unsigned int    Reserved_0            : 2   ; // [30..29]
        unsigned int    thd2_mode             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDVTTHD;

// Define the union U_DHDCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 9   ; // [8..0]
        unsigned int    cscidc1               : 9   ; // [17..9]
        unsigned int    cscidc2               : 9   ; // [26..18]
        unsigned int    csc_en                : 1   ; // [27]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCSCIDC;

// Define the union U_DHDCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 9   ; // [8..0]
        unsigned int    cscodc1               : 9   ; // [17..9]
        unsigned int    cscodc2               : 9   ; // [26..18]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCSCODC;

// Define the union U_DHDCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp01                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCSCP0;

// Define the union U_DHDCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp10                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCSCP1;

// Define the union U_DHDCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp12                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCSCP2;

// Define the union U_DHDCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp21                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCSCP3;

// Define the union U_DHDCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 13  ; // [12..0]
        unsigned int    Reserved_0            : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCSCP4;

// Define the union U_DHDCLIPL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clipcl0               : 10  ; // [9..0]
        unsigned int    clipcl1               : 10  ; // [19..10]
        unsigned int    clipcl2               : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCLIPL;

// Define the union U_DHDCLIPH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clipch0               : 10  ; // [9..0]
        unsigned int    clipch1               : 10  ; // [19..10]
        unsigned int    clipch2               : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDCLIPH;

// Define the union U_DHDSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vback_blank           : 1   ; // [0]
        unsigned int    vblank                : 1   ; // [1]
        unsigned int    bottom_field          : 1   ; // [2]
        unsigned int    Reserved_0            : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDSTATE;

// Define the union U_DSDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    intfdm                : 4   ; // [3..0]
        unsigned int    intfb                 : 2   ; // [5..4]
        unsigned int    synm                  : 1   ; // [6]
        unsigned int    iop                   : 1   ; // [7]
        unsigned int    ivs                   : 1   ; // [8]
        unsigned int    ihs                   : 1   ; // [9]
        unsigned int    idv                   : 1   ; // [10]
        unsigned int    Reserved_2            : 5   ; // [15..11]
        unsigned int    clipen                : 1   ; // [16]
        unsigned int    Reserved_1            : 2   ; // [18..17]
        unsigned int    trigger_en            : 1   ; // [19]
        unsigned int    Reserved_0            : 11  ; // [30..20]
        unsigned int    intf_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCTRL;

// Define the union U_DSDVSYNC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vact                  : 12  ; // [11..0]
        unsigned int    vbb                   : 8   ; // [19..12]
        unsigned int    vfb                   : 8   ; // [27..20]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDVSYNC;

// Define the union U_DSDHSYNC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hact                  : 16  ; // [15..0]
        unsigned int    hbb                   : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDHSYNC1;

// Define the union U_DSDHSYNC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hfb                   : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDHSYNC2;

// Define the union U_DSDVPLUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bvact                 : 12  ; // [11..0]
        unsigned int    bvbb                  : 8   ; // [19..12]
        unsigned int    bvfb                  : 8   ; // [27..20]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDVPLUS;

// Define the union U_DSDPWR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hpw                   : 16  ; // [15..0]
        unsigned int    vpw                   : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDPWR;

// Define the union U_DSDVTTHD3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vtmgthd3              : 13  ; // [12..0]
        unsigned int    Reserved_1            : 2   ; // [14..13]
        unsigned int    thd3_mode             : 1   ; // [15]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDVTTHD3;

// Define the union U_DSDVTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vtmgthd1              : 13  ; // [12..0]
        unsigned int    Reserved_1            : 2   ; // [14..13]
        unsigned int    thd1_mode             : 1   ; // [15]
        unsigned int    vtmgthd2              : 13  ; // [28..16]
        unsigned int    Reserved_0            : 2   ; // [30..29]
        unsigned int    thd2_mode             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDVTTHD;

// Define the union U_DSDCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 9   ; // [8..0]
        unsigned int    cscidc1               : 9   ; // [17..9]
        unsigned int    cscidc2               : 9   ; // [26..18]
        unsigned int    csc_en                : 1   ; // [27]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCSCIDC;

// Define the union U_DSDCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 9   ; // [8..0]
        unsigned int    cscodc1               : 9   ; // [17..9]
        unsigned int    cscodc2               : 9   ; // [26..18]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCSCODC;

// Define the union U_DSDCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp01                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCSCP0;

// Define the union U_DSDCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp10                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCSCP1;

// Define the union U_DSDCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp12                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCSCP2;

// Define the union U_DSDCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 13  ; // [12..0]
        unsigned int    Reserved_1            : 3   ; // [15..13]
        unsigned int    cscp21                : 13  ; // [28..16]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCSCP3;

// Define the union U_DSDCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 13  ; // [12..0]
        unsigned int    Reserved_0            : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCSCP4;

// Define the union U_DSDCLIPL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clipcl0               : 10  ; // [9..0]
        unsigned int    clipcl1               : 10  ; // [19..10]
        unsigned int    clipcl2               : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCLIPL;

// Define the union U_DSDCLIPH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clipch0               : 10  ; // [9..0]
        unsigned int    clipch1               : 10  ; // [19..10]
        unsigned int    clipch2               : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDCLIPH;

// Define the union U_DSDSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vback_blank           : 1   ; // [0]
        unsigned int    vblank                : 1   ; // [1]
        unsigned int    bottom_field          : 1   ; // [2]
        unsigned int    Reserved_0            : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DSDSTATE;

// Define the union U_HDATE_VERSION
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int hdate_version           : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_VERSION;
// Define the union U_HDATE_EN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hd_en                 : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_EN;

// Define the union U_HDATE_POLA_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsync_in_pola         : 1   ; // [0]
        unsigned int    vsync_in_pola         : 1   ; // [1]
        unsigned int    fid_in_pola           : 1   ; // [2]
        unsigned int    hsync_out_pola        : 1   ; // [3]
        unsigned int    vsync_out_pola        : 1   ; // [4]
        unsigned int    fid_out_pola          : 1   ; // [5]
        unsigned int    Reserved_0            : 26  ; // [31..6]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_POLA_CTRL;

// Define the union U_HDATE_VIDEO_FORMAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_ft              : 4   ; // [3..0]
        unsigned int    sync_add_ctrl         : 3   ; // [6..4]
        unsigned int    video_out_ctrl        : 2   ; // [8..7]
        unsigned int    csc_ctrl              : 3   ; // [11..9]
        unsigned int    Reserved_0            : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_VIDEO_FORMAT;

// Define the union U_HDATE_STATE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    line_len              : 13  ; // [12..0]
        unsigned int    Reserved_2            : 3   ; // [15..13]
        unsigned int    frame_len             : 11  ; // [26..16]
        unsigned int    Reserved_1            : 1   ; // [27]
        unsigned int    mv_en_pin             : 1   ; // [28]
        unsigned int    Reserved_0            : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_STATE;

// Define the union U_HDATE_OUT_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vsync_sel             : 2   ; // [1..0]
        unsigned int    hsync_sel             : 2   ; // [3..2]
        unsigned int    video3_sel            : 2   ; // [5..4]
        unsigned int    video2_sel            : 2   ; // [7..6]
        unsigned int    video1_sel            : 2   ; // [9..8]
        unsigned int    src_ctrl              : 2   ; // [11..10]
        unsigned int    sync_lpf_en           : 1   ; // [12]
        unsigned int    sd_sel                : 1   ; // [13]
        unsigned int    Reserved_0            : 18  ; // [31..14]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_OUT_CTRL;

// Define the union U_HDATE_SRC_13_COEF1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap1_1           : 5   ; // [4..0]
        unsigned int    Reserved_1            : 11  ; // [15..5]
        unsigned int    coef_tap1_3           : 5   ; // [20..16]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF1;

// Define the union U_HDATE_SRC_13_COEF2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap2_1           : 6   ; // [5..0]
        unsigned int    Reserved_1            : 10  ; // [15..6]
        unsigned int    coef_tap2_3           : 6   ; // [21..16]
        unsigned int    Reserved_0            : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF2;

// Define the union U_HDATE_SRC_13_COEF3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap3_1           : 7   ; // [6..0]
        unsigned int    Reserved_1            : 9   ; // [15..7]
        unsigned int    coef_tap3_3           : 7   ; // [22..16]
        unsigned int    Reserved_0            : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF3;

// Define the union U_HDATE_SRC_13_COEF4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap4_1           : 8   ; // [7..0]
        unsigned int    Reserved_1            : 8   ; // [15..8]
        unsigned int    coef_tap4_3           : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF4;

// Define the union U_HDATE_SRC_13_COEF5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap5_1           : 9   ; // [8..0]
        unsigned int    Reserved_1            : 7   ; // [15..9]
        unsigned int    coef_tap5_3           : 9   ; // [24..16]
        unsigned int    Reserved_0            : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF5;

// Define the union U_HDATE_SRC_13_COEF6
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap6_1           : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef_tap6_3           : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF6;

// Define the union U_HDATE_SRC_13_COEF7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap7_1           : 11  ; // [10..0]
        unsigned int    Reserved_1            : 5   ; // [15..11]
        unsigned int    coef_tap7_3           : 11  ; // [26..16]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF7;

// Define the union U_HDATE_SRC_13_COEF8
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap8_1           : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef_tap8_3           : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF8;

// Define the union U_HDATE_SRC_13_COEF9
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap9_1           : 9   ; // [8..0]
        unsigned int    Reserved_1            : 7   ; // [15..9]
        unsigned int    coef_tap9_3           : 9   ; // [24..16]
        unsigned int    Reserved_0            : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF9;

// Define the union U_HDATE_SRC_13_COEF10
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap10_1          : 8   ; // [7..0]
        unsigned int    Reserved_1            : 8   ; // [15..8]
        unsigned int    coef_tap10_3          : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF10;

// Define the union U_HDATE_SRC_13_COEF11
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap11_1          : 7   ; // [6..0]
        unsigned int    Reserved_1            : 9   ; // [15..7]
        unsigned int    coef_tap11_3          : 7   ; // [22..16]
        unsigned int    Reserved_0            : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF11;

// Define the union U_HDATE_SRC_13_COEF12
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap12_1          : 6   ; // [5..0]
        unsigned int    Reserved_1            : 10  ; // [15..6]
        unsigned int    coef_tap12_3          : 6   ; // [21..16]
        unsigned int    Reserved_0            : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF12;

// Define the union U_HDATE_SRC_13_COEF13
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap13_1          : 5   ; // [4..0]
        unsigned int    Reserved_1            : 11  ; // [15..5]
        unsigned int    coef_tap13_3          : 5   ; // [20..16]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_13_COEF13;

// Define the union U_HDATE_SRC_24_COEF1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap1_2           : 5   ; // [4..0]
        unsigned int    Reserved_1            : 11  ; // [15..5]
        unsigned int    coef_tap1_4           : 5   ; // [20..16]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF1;

// Define the union U_HDATE_SRC_24_COEF2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap2_2           : 6   ; // [5..0]
        unsigned int    Reserved_1            : 10  ; // [15..6]
        unsigned int    coef_tap2_4           : 6   ; // [21..16]
        unsigned int    Reserved_0            : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF2;

// Define the union U_HDATE_SRC_24_COEF3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap3_2           : 7   ; // [6..0]
        unsigned int    Reserved_1            : 9   ; // [15..7]
        unsigned int    coef_tap3_4           : 7   ; // [22..16]
        unsigned int    Reserved_0            : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF3;

// Define the union U_HDATE_SRC_24_COEF4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap4_2           : 8   ; // [7..0]
        unsigned int    Reserved_1            : 8   ; // [15..8]
        unsigned int    coef_tap4_4           : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF4;

// Define the union U_HDATE_SRC_24_COEF5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap5_2           : 9   ; // [8..0]
        unsigned int    Reserved_1            : 7   ; // [15..9]
        unsigned int    coef_tap5_4           : 9   ; // [24..16]
        unsigned int    Reserved_0            : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF5;

// Define the union U_HDATE_SRC_24_COEF6
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap6_2           : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef_tap6_4           : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF6;

// Define the union U_HDATE_SRC_24_COEF7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap7_2           : 11  ; // [10..0]
        unsigned int    Reserved_1            : 5   ; // [15..11]
        unsigned int    coef_tap7_4           : 11  ; // [26..16]
        unsigned int    Reserved_0            : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF7;

// Define the union U_HDATE_SRC_24_COEF8
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap8_2           : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    coef_tap8_4           : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF8;

// Define the union U_HDATE_SRC_24_COEF9
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap9_1           : 9   ; // [8..0]
        unsigned int    Reserved_1            : 7   ; // [15..9]
        unsigned int    coef_tap9_2           : 9   ; // [24..16]
        unsigned int    Reserved_0            : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF9;

// Define the union U_HDATE_SRC_24_COEF10
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap10_2          : 8   ; // [7..0]
        unsigned int    Reserved_1            : 8   ; // [15..8]
        unsigned int    coef_tap10_4          : 8   ; // [23..16]
        unsigned int    Reserved_0            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF10;

// Define the union U_HDATE_SRC_24_COEF11
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap11_1          : 7   ; // [6..0]
        unsigned int    Reserved_1            : 9   ; // [15..7]
        unsigned int    coef_tap11_2          : 7   ; // [22..16]
        unsigned int    Reserved_0            : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF11;

// Define the union U_HDATE_SRC_24_COEF12
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap12_2          : 6   ; // [5..0]
        unsigned int    Reserved_1            : 10  ; // [15..6]
        unsigned int    coef_tap12_4          : 6   ; // [21..16]
        unsigned int    Reserved_0            : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF12;

// Define the union U_HDATE_SRC_24_COEF13
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_tap13_2          : 5   ; // [4..0]
        unsigned int    Reserved_1            : 11  ; // [15..5]
        unsigned int    coef_tap13_4          : 5   ; // [20..16]
        unsigned int    Reserved_0            : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_SRC_24_COEF13;

// Define the union U_HDATE_CSC_COEF1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc_coef_r_y          : 12  ; // [11..0]
        unsigned int    Reserved_1            : 4   ; // [15..12]
        unsigned int    csc_coef_r_cb         : 12  ; // [27..16]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CSC_COEF1;

// Define the union U_HDATE_CSC_COEF2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc_coef_r_cr         : 12  ; // [11..0]
        unsigned int    Reserved_1            : 4   ; // [15..12]
        unsigned int    csc_coef_g_y          : 12  ; // [27..16]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CSC_COEF2;

// Define the union U_HDATE_CSC_COEF3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc_coef_g_cb         : 12  ; // [11..0]
        unsigned int    Reserved_1            : 4   ; // [15..12]
        unsigned int    csc_coef_g_cr         : 12  ; // [27..16]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CSC_COEF3;

// Define the union U_HDATE_CSC_COEF4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc_coef_b_y          : 12  ; // [11..0]
        unsigned int    Reserved_1            : 4   ; // [15..12]
        unsigned int    csc_coef_b_cb         : 12  ; // [27..16]
        unsigned int    Reserved_0            : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CSC_COEF4;

// Define the union U_HDATE_CSC_COEF5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    csc_coef_b_cr         : 12  ; // [11..0]
        unsigned int    Reserved_0            : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CSC_COEF5;

// Define the union U_HDATE_TEST
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    test_data             : 10  ; // [9..0]
        unsigned int    Reserved_0            : 22  ; // [31..10]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_TEST;

// Define the union U_HDATE_VBI_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cgmsb_add_en          : 1   ; // [0]
        unsigned int    cgmsa_add_en          : 1   ; // [1]
        unsigned int    mv_en                 : 1   ; // [2]
        unsigned int    vbi_lpf_en            : 1   ; // [3]
        unsigned int    Reserved_0            : 28  ; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_VBI_CTRL;

// Define the union U_HDATE_CGMSA_DATA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cgmsa_data            : 20  ; // [19..0]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CGMSA_DATA;

// Define the union U_HDATE_CGMSB_H
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hdate_cgmsb_h         : 6   ; // [5..0]
        unsigned int    Reserved_0            : 26  ; // [31..6]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CGMSB_H;

// Define the union U_HDATE_CGMSB_DATA1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int cgmsb_data1             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CGMSB_DATA1;
// Define the union U_HDATE_CGMSB_DATA2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int cgmsb_data2             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CGMSB_DATA2;
// Define the union U_HDATE_CGMSB_DATA3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int cgmsb_data3             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CGMSB_DATA3;
// Define the union U_HDATE_CGMSB_DATA4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int cgmsb_data4             : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_HDATE_CGMSB_DATA4;
// Define the union U_DATE_COEFF0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_seq                : 1   ; // [0]
        unsigned int    chgain_en             : 1   ; // [1]
        unsigned int    sylp_en               : 1   ; // [2]
        unsigned int    chlp_en               : 1   ; // [3]
        unsigned int    oversam2_en           : 1   ; // [4]
        unsigned int    lunt_en               : 1   ; // [5]
        unsigned int    oversam_en            : 2   ; // [7..6]
        unsigned int    Reserved_1            : 1   ; // [8]
        unsigned int    luma_dl               : 4   ; // [12..9]
        unsigned int    agc_amp_sel           : 1   ; // [13]
        unsigned int    length_sel            : 1   ; // [14]
        unsigned int    sync_mode_scart       : 1   ; // [15]
        unsigned int    sync_mode_sel         : 2   ; // [17..16]
        unsigned int    style_sel             : 4   ; // [21..18]
        unsigned int    fm_sel                : 1   ; // [22]
        unsigned int    vbi_lpf_en            : 1   ; // [23]
        unsigned int    rgb_en                : 1   ; // [24]
        unsigned int    scanline              : 1   ; // [25]
        unsigned int    Reserved_0            : 3   ; // [28..26]
        unsigned int    dis_ire               : 1   ; // [29]
        unsigned int    clpf_sel              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF0;

// Define the union U_DATE_COEFF1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac_test              : 10  ; // [9..0]
        unsigned int    date_test_mode        : 2   ; // [11..10]
        unsigned int    date_test_en          : 1   ; // [12]
        unsigned int    amp_outside           : 10  ; // [22..13]
        unsigned int    c_limit_en            : 1   ; // [23]
        unsigned int    cc_seq                : 1   ; // [24]
        unsigned int    cgms_seq              : 1   ; // [25]
        unsigned int    vps_seq               : 1   ; // [26]
        unsigned int    wss_seq               : 1   ; // [27]
        unsigned int    cvbs_limit_en         : 1   ; // [28]
        unsigned int    c_gain                : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF1;

// Define the union U_DATE_COEFF7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt07_enf2             : 1   ; // [0]
        unsigned int    tt08_enf2             : 1   ; // [1]
        unsigned int    tt09_enf2             : 1   ; // [2]
        unsigned int    tt10_enf2             : 1   ; // [3]
        unsigned int    tt11_enf2             : 1   ; // [4]
        unsigned int    tt12_enf2             : 1   ; // [5]
        unsigned int    tt13_enf2             : 1   ; // [6]
        unsigned int    tt14_enf2             : 1   ; // [7]
        unsigned int    tt15_enf2             : 1   ; // [8]
        unsigned int    tt16_enf2             : 1   ; // [9]
        unsigned int    tt17_enf2             : 1   ; // [10]
        unsigned int    tt18_enf2             : 1   ; // [11]
        unsigned int    tt19_enf2             : 1   ; // [12]
        unsigned int    tt20_enf2             : 1   ; // [13]
        unsigned int    tt21_enf2             : 1   ; // [14]
        unsigned int    tt22_enf2             : 1   ; // [15]
        unsigned int    tt07_enf1             : 1   ; // [16]
        unsigned int    tt08_enf1             : 1   ; // [17]
        unsigned int    tt09_enf1             : 1   ; // [18]
        unsigned int    tt10_enf1             : 1   ; // [19]
        unsigned int    tt11_enf1             : 1   ; // [20]
        unsigned int    tt12_enf1             : 1   ; // [21]
        unsigned int    tt13_enf1             : 1   ; // [22]
        unsigned int    tt14_enf1             : 1   ; // [23]
        unsigned int    tt15_enf1             : 1   ; // [24]
        unsigned int    tt16_enf1             : 1   ; // [25]
        unsigned int    tt17_enf1             : 1   ; // [26]
        unsigned int    tt18_enf1             : 1   ; // [27]
        unsigned int    tt19_enf1             : 1   ; // [28]
        unsigned int    tt20_enf1             : 1   ; // [29]
        unsigned int    tt21_enf1             : 1   ; // [30]
        unsigned int    tt22_enf1             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF7;

// Define the union U_DATE_COEFF8
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tt_staddr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF8;
// Define the union U_DATE_COEFF9
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tt_edaddr               : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF9;
// Define the union U_DATE_COEFF10
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_pktoff             : 8   ; // [7..0]
        unsigned int    tt_mode               : 2   ; // [9..8]
        unsigned int    tt_highest            : 1   ; // [10]
        unsigned int    full_page             : 1   ; // [11]
        unsigned int    nabts_100ire          : 1   ; // [12]
        unsigned int    Reserved_0            : 18  ; // [30..13]
        unsigned int    tt_ready              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF10;

// Define the union U_DATE_COEFF11
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    date_clf2             : 10  ; // [9..0]
        unsigned int    date_clf1             : 10  ; // [19..10]
        unsigned int    cc_enf2               : 1   ; // [20]
        unsigned int    cc_enf1               : 1   ; // [21]
        unsigned int    Reserved_0            : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF11;

// Define the union U_DATE_COEFF12
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cc_f2data             : 16  ; // [15..0]
        unsigned int    cc_f1data             : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF12;

// Define the union U_DATE_COEFF13
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cg_f1data             : 20  ; // [19..0]
        unsigned int    cg_enf2               : 1   ; // [20]
        unsigned int    cg_enf1               : 1   ; // [21]
        unsigned int    Reserved_0            : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF13;

// Define the union U_DATE_COEFF14
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cg_f2data             : 20  ; // [19..0]
        unsigned int    Reserved_0            : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF14;

// Define the union U_DATE_COEFF15
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wss_data              : 14  ; // [13..0]
        unsigned int    wss_en                : 1   ; // [14]
        unsigned int    Reserved_0            : 17  ; // [31..15]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF15;

// Define the union U_DATE_COEFF16
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vps_data              : 24  ; // [23..0]
        unsigned int    vps_en                : 1   ; // [24]
        unsigned int    Reserved_0            : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF16;

// Define the union U_DATE_COEFF17
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int vps_data                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF17;
// Define the union U_DATE_COEFF18
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int vps_data                : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF18;
// Define the union U_DATE_COEFF19
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vps_data              : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF19;

// Define the union U_DATE_COEFF20
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt05_enf2             : 1   ; // [0]
        unsigned int    tt06_enf2             : 1   ; // [1]
        unsigned int    tt06_enf1             : 1   ; // [2]
        unsigned int    Reserved_0            : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF20;

// Define the union U_DATE_COEFF21
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac0_in_sel           : 3   ; // [2..0]
        unsigned int    Reserved_5            : 1   ; // [3]
        unsigned int    dac1_in_sel           : 3   ; // [6..4]
        unsigned int    Reserved_4            : 1   ; // [7]
        unsigned int    dac2_in_sel           : 3   ; // [10..8]
        unsigned int    Reserved_3            : 1   ; // [11]
        unsigned int    dac3_in_sel           : 3   ; // [14..12]
        unsigned int    Reserved_2            : 1   ; // [15]
        unsigned int    dac4_in_sel           : 3   ; // [18..16]
        unsigned int    Reserved_1            : 1   ; // [19]
        unsigned int    dac5_in_sel           : 3   ; // [22..20]
        unsigned int    Reserved_0            : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF21;

// Define the union U_DATE_COEFF22
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_phase_delta     : 11  ; // [10..0]
        unsigned int    Reserved_0            : 21  ; // [31..11]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF22;

// Define the union U_DATE_COEFF23
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac0_out_dly          : 3   ; // [2..0]
        unsigned int    Reserved_5            : 1   ; // [3]
        unsigned int    dac1_out_dly          : 3   ; // [6..4]
        unsigned int    Reserved_4            : 1   ; // [7]
        unsigned int    dac2_out_dly          : 3   ; // [10..8]
        unsigned int    Reserved_3            : 1   ; // [11]
        unsigned int    dac3_out_dly          : 3   ; // [14..12]
        unsigned int    Reserved_2            : 1   ; // [15]
        unsigned int    dac4_out_dly          : 3   ; // [18..16]
        unsigned int    Reserved_1            : 1   ; // [19]
        unsigned int    dac5_out_dly          : 3   ; // [22..20]
        unsigned int    Reserved_0            : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF23;

// Define the union U_DATE_ISRMASK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_mask               : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_ISRMASK;

// Define the union U_DATE_ISRSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_status             : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_ISRSTATE;

// Define the union U_DATE_ISR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_int                : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_ISR;

// Define the union U_DATE_VERSION
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int Reserved_0              : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_VERSION;
// Define the union U_VHDHLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDHLCOEF;

// Define the union U_VHDHCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDHCCOEF;

// Define the union U_VHDVLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDVLCOEF;

// Define the union U_VHDVCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VHDVCCOEF;

// Define the union U_VADHLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADHLCOEF;

// Define the union U_VADHCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADHCCOEF;

// Define the union U_VADVLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADVLCOEF;

// Define the union U_VADVCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VADVCCOEF;

// Define the union U_VSDHLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDHLCOEF;

// Define the union U_VSDHCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDHCCOEF;

// Define the union U_VSDVLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDVLCOEF;

// Define the union U_VSDVCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VSDVCCOEF;

// Define the union U_G0HLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0HLCOEF;

// Define the union U_G0HCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0HCCOEF;

// Define the union U_G0VLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0VLCOEF;

// Define the union U_G0VCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0VCCOEF;

// Define the union U_G1HLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1HLCOEF;

// Define the union U_G1HCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    hccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1HCCOEF;

// Define the union U_G1VLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vlcoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vlcoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1VLCOEF;

// Define the union U_G1VCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vccoefn1              : 10  ; // [9..0]
        unsigned int    Reserved_1            : 6   ; // [15..10]
        unsigned int    vccoefn2              : 10  ; // [25..16]
        unsigned int    Reserved_0            : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G1VCCOEF;

// Define the union U_DHDGAMMAN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    gamma_databn          : 10  ; // [9..0]
        unsigned int    gamma_datagn          : 10  ; // [19..10]
        unsigned int    gamma_datarn          : 10  ; // [29..20]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDGAMMAN;

//add for mmu
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vpp_mmu_en         : 11   ; // [10..0]
        unsigned int    tile_in            : 1   ; // [11]
        unsigned int    tile_out           : 1   ; // [12]
        unsigned int    bit7_xor           : 1   ; // [13]
        unsigned int    x_tile_num         : 4   ; // [17..14]
        unsigned int    cfg_gt_ctl         : 2   ; // [19..18]
        unsigned int    core_gt_ctl        : 2   ; // [21..20]
        unsigned int    burst_split        : 1   ; // [22]
        unsigned int    w_bit7_xor         : 1   ; // [23]
        unsigned int    w_x_tile_num       : 4   ; // [27..24]
        unsigned int    Reserved_0         : 3   ; // [30..28]
        unsigned int    frame_start        : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_TOP_CTL;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0       : 4   ; // [3..0]
        unsigned int    pt_baddr         : 28   ; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_CH_PTBA;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0       : 12   ; // [11..0]
        unsigned int    pt_vaddr         : 20   ; // [31..12]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_CH_PTVA;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    axi_err_inte          : 1   ; // [0]
        unsigned int    tlb_miss_inte         : 1   ; // [1]
        unsigned int    tlb_vbit_err_inte     : 1   ; //[2]
        unsigned int    Reserved_0            : 29  ; //[31..3]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_CH_INTE;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    axi_err_intc          : 1   ; // [0]
        unsigned int    tlb_miss_intc         : 1   ; // [1]
        unsigned int    tlb_vbit_err_intc     : 1   ; //[2]
        unsigned int    Reserved_0            : 29  ; //[31..3]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_CH_INTC;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    axi_err_ints          : 1   ; // [0]
        unsigned int    tlb_vbit_err_ints     : 1   ; // [1]
        unsigned int    tlb_vbit_err_inte     : 1   ; //[2]
        unsigned int    Reserved_0            : 29  ; //[31..3]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_CH_INTS;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0           : 12  ; //[11..0]
        unsigned int    frame_start_addr     : 20  ; //[31..12]

    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_CH_START_ADDR;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0           : 12  ; //[11..0]
        unsigned int    frame_end_addr       : 20  ; //[31..12]

    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_CH_END_ADDR;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vpp_mmu_ints         : 11   ; //[10..0]       : 20   ; //[31..12]
        unsigned int    vpp_mmu_idle         : 1  ;    //[11]
        unsigned int    Reserved_0           : 20   ; //[31..12]

    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_MMU_TOP_INTS;

//add for mmu
typedef struct {
    U_MMU_CH_PTBA       MMU_CH_PTBA;//0x0000
    U_MMU_CH_PTVA       MMU_CH_PTVA;//0x0004
    U_MMU_CH_INTE       MMU_CH_INTE;//0x0008
    unsigned int        Reserved_0; //0x000c
    U_MMU_CH_INTC       MMU_CH_INTC;//0x0010
    U_MMU_CH_INTS       MMU_CH_INTS;//0x0014
    unsigned int        Reserved_1[3];//0x0018
    U_MMU_CH_START_ADDR MMU_CH_START_ADDR;//0x0024
    U_MMU_CH_END_ADDR   MMU_CH_END_ADDR;////0x0028
}MMU_CH_S;

//modified for zme coef config by l50053, begin
//==============================================================================
// Define the global struct
typedef struct
{
    U_VOCTRL             VOCTRL;   //0000
    U_VOINTSTA           VOINTSTA;  //0004
    U_VOMSKINTSTA        VOMSKINTSTA; //0008
    U_VOINTMSK           VOINTMSK;    //000C
    U_VOUVERSION1        VOUVERSION1;//0010
    U_VOUVERSION2        VOUVERSION2;//0014
    U_VOMUXDATA          VOMUXDATA;//0018
    U_VOMUX              VOMUX;//001c
    U_VODEBUG            VODEBUG;//0020
    U_VOLNKLSTCTRL       VOLNKLSTCTRL;//0024
    U_VOLNKLSTADDR       VOLNKLSTADDR;//0028
    U_VOLNKLSTSTRT       VOLNKLSTSTRT;//002c
    U_VOLNKLSTPENDINGSTA VOLNKLSTPENDINGSTA;//0030
    U_VOLNKLSTWORKINGSTA VOLNKLSTWORKINGSTA;//0034
    U_VOLNKLSTENDSTA     VOLNKLSTENDSTA;//0038
    U_VOLNKLSTNUMSTA     VOLNKLSTNUMSTA;//003c
    U_VOPARAUP           VOPARAUP;    //0040
    U_VHDHCOEFAD         VHDHCOEFAD;  //0044
    U_VHDVCOEFAD         VHDVCOEFAD;  //0048
    U_VADHCOEFAD         VADHCOEFAD;  //004C
    U_VADVCOEFAD         VADVCOEFAD;  //0050
    U_ACCAD              ACCAD;         //0054

    unsigned int         Reserved_0[42];

    U_VHDCTRL            VHDCTRL;      //0100
    U_VHDUPD             VHDUPD;      //0104
    U_VHDLADDR           VHDLADDR;    //0108
    U_VHDLCADDR          VHDLCADDR;   //010C
    U_VHDCADDR           VHDCADDR;    //0110
    U_VHDCCADDR          VHDCCADDR;   //0114
    U_VHDNADDR           VHDNADDR;    //0118
    U_VHDNCADDR          VHDNCADDR;   //011C
    unsigned int         Reserved_2;
    U_VHDSTRIDE          VHDSTRIDE;    //0124
    U_VHDIRESO           VHDIRESO;     //0128
    unsigned int         Reserved_3;
    U_VHDLKEY            VHDLKEY;//0130
    U_VHDCBMPARA         VHDCBMPARA;//0134
    U_VHDDIEADDR         VHDDIEADDR;   //0138
    U_VHDWBC1ADDR        VHDWBC1ADDR;  //013C
    U_VHDWBC1STRD        VHDWBC1STRD;  //0140
    U_VHDDIESTADDR       VHDDIESTADDR;  //0144
    unsigned int         Reserved_4[6];
    U_VHDDFPOS           VHDDFPOS;//0160
    U_VHDDLPOS           VHDDLPOS;//0164
    U_VHDVFPOS           VHDVFPOS;     //0168
    U_VHDVLPOS           VHDVLPOS;    //016C
    U_VHDBK              VHDBK;       //0170
    unsigned int         Reserved_5[3];
    U_VHDCSCIDC          VHDCSCIDC;    //0180
    U_VHDCSCODC          VHDCSCODC;    //0184
    U_VHDCSCP0           VHDCSCP0;     //0188
    U_VHDCSCP1           VHDCSCP1;     //018C
    U_VHDCSCP2           VHDCSCP2;      //0190
    U_VHDCSCP3           VHDCSCP3;     //0194
    U_VHDCSCP4           VHDCSCP4;     //0198
    unsigned int         Reserved_6;
    U_VHDACM0            VHDACM0;      //01A0
    U_VHDACM1            VHDACM1;       //01A4
    U_VHDACM2            VHDACM2;      //01A8
    U_VHDACM3            VHDACM3;         //01AC
    U_VHDACM4            VHDACM4;         //01B0
    U_VHDACM5            VHDACM5;       //01B4
    U_VHDACM6            VHDACM6;       //01B8
    U_VHDACM7            VHDACM7;      //01BC
    U_VHDHSP             VHDHSP;        //01C0
    U_VHDHLOFFSET        VHDHLOFFSET;   //01C4
    U_VHDHCOFFSET        VHDHCOFFSET;   //01C8
    unsigned int         Reserved_7[3];
    U_VHDVSP             VHDVSP;//01D8
    U_VHDVSR             VHDVSR; //01DC
    U_VHDVOFFSET         VHDVOFFSET;   //01E0
    U_VHDZMEORESO        VHDZMEORESO;  //01E4
    U_VHDZMEIRESO        VHDZMEIRESO;  //01E8
    U_VHDZMEDBG          VHDZMEDBG;    //01EC

    U_VHDHSP_K3V3        VHDHSP_K3V3;  //0x01F0
    U_VHDHLOFFSET_K3V3   VHDHLOFFSET_K3V3;//0x01F4

    unsigned int         Reserved_8[2];
    U_VHDACCTHD1         VHDACCTHD1;   //0200
    U_VHDACCTHD2         VHDACCTHD2;   //0204
    unsigned int         Reserved_9[2];
    U_VHDACCLOWN         VHDACCLOWN[3];  //0210~0218
    unsigned int         Reserved_10;
    U_VHDACCMEDN         VHDACCMEDN[3];  //0220~0228
    unsigned int         Reserved_11;
    U_VHDACCHIGHN        VHDACCHIGHN[3];  //0230~0238
    unsigned int         Reserved_12;
    U_VHDACCMLN          VHDACCMLN[3];  //0240~0248
    unsigned int         Reserved_13;
    U_VHDACCMHN          VHDACCMHN[3];   //0250~0258
    unsigned int         Reserved_14;
    U_VHDACC3LOW         VHDACC3LOW;    //0260
    U_VHDACC3MED         VHDACC3MED;    //0264
    U_VHDACC3HIGH        VHDACC3HIGH;   //0268
    U_VHDACC8MLOW        VHDACC8MLOW;   //026c
    U_VHDACC8MHIGH       VHDACC8MHIGH;  //0270
    U_VHDACCTOTAL        VHDACCTOTAL;   //0274
    unsigned int         Reserved_15[2];//0278~027c
    U_VHDIFIRCOEF01      VHDIFIRCOEF01; //0280
    U_VHDIFIRCOEF23      VHDIFIRCOEF23; //0284
    U_VHDIFIRCOEF45      VHDIFIRCOEF45; //0288
    U_VHDIFIRCOEF67      VHDIFIRCOEF67; //028c
    unsigned int         Reserved_16[28];
    U_VHDDIECTRL         VHDDIECTRL;    //0300
    unsigned int         Reserved_17;
    U_VHDDIELMA0         VHDDIELMA0;    //0308
    U_VHDDIELMA1         VHDDIELMA1;    //030c
    U_VHDDIELMA2         VHDDIELMA2;    //0310
    U_VHDDIEINTEN        VHDDIEINTEN;   //0314
    U_VHDDIESCALE        VHDDIESCALE;   //0318
    U_VHDDIECHECK1       VHDDIECHECK1;  //031c
    U_VHDDIECHECK2       VHDDIECHECK2;  //0320
    U_VHDDIEDIR0_3       VHDDIEDIR0_3;  //0324
    U_VHDDIEDIR4_7       VHDDIEDIR4_7;  //0328
    U_VHDDIEDIR8_11      VHDDIEDIR8_11; //032c
    U_VHDDIEDIR12_14     VHDDIEDIR12_14;//0330
    unsigned int         Reserved_18;
    U_VHDDIESTA          VHDDIESTA;     //0338
    unsigned int         Reserved_19[26];
    U_VHDDIESTKADDR      VHDDIESTKADDR; //03a4
    U_VHDDIESTLADDR      VHDDIESTLADDR; //03a8
    U_VHDDIESTMADDR      VHDDIESTMADDR; //03ac
    U_VHDDIESTNADDR      VHDDIESTNADDR; //03b0
    U_VHDDIESTSQTRADDR   VHDDIESTSQTRADDR;//03b4
    U_VHDCCRSCLR0        VHDCCRSCLR0;     //03b8
    U_VHDCCRSCLR1        VHDCCRSCLR1;     //03bc
    unsigned int         Reserved_20[16];
    U_VHDPDCTRL          VHDPDCTRL;     //0400
    U_VHDPDBLKPOS0       VHDPDBLKPOS0;  //0404
    U_VHDPDBLKPOS1       VHDPDBLKPOS1;  //0408
    U_VHDPDBLKTHD        VHDPDBLKTHD;   //040c
    U_VHDPDHISTTHD       VHDPDHISTTHD;  //0410
    U_VHDPDUMTHD         VHDPDUMTHD;    //0414
    U_VHDPDPCCCORING     VHDPDPCCCORING;//0418
    U_VHDPDPCCHTHD       VHDPDPCCHTHD;  //041c
    U_VHDPDPCCVTHD       VHDPDPCCVTHD;  //0420
    U_VHDPDITDIFFVTHD    VHDPDITDIFFVTHD;//0424
    U_VHDPDLASITHD       VHDPDLASITHD;  //0428
    U_VHDPDFRMITDIFF     VHDPDFRMITDIFF;//042c
    U_VHDPDSTLBLKSAD     VHDPDSTLBLKSAD[16];//0x0430~0x046C
    U_VHDPDHISTBIN       VHDPDHISTBIN[4];//0x0470~0x47C
    U_VHDPDUMMATCH0      VHDPDUMMATCH0;//0x0480
    U_VHDPDUMNOMATCH0    VHDPDUMNOMATCH0;//0x0484
    U_VHDPDUMMATCH1      VHDPDUMMATCH1;//0x0488
    U_VHDPDUMNOMATCH1    VHDPDUMNOMATCH1;//0x048C
    U_VHDPDPCCFFWD       VHDPDPCCFFWD;//0x0490
    U_VHDPDPCCFWD        VHDPDPCCFWD;//0x0494
    U_VHDPDPCCBWD        VHDPDPCCBWD;//0x0498
    U_VHDPDPCCCRSS       VHDPDPCCCRSS;//0x49C
    U_VHDPDPCCPW         VHDPDPCCPW;//0x4A0
    U_VHDPDPCCFWDTKR     VHDPDPCCFWDTKR;//0x4A4
    U_VHDPDPCCBWDTKR     VHDPDPCCBWDTKR;//0x4A8
    U_VHDPDPCCBLKFWD     VHDPDPCCBLKFWD[9];//0x04AC~0x4cc
    U_VHDPDPCCBLKBWD     VHDPDPCCBLKBWD[9];//0x04D0~0x4F0
    U_VHDPDLASICNT14     VHDPDLASICNT14;//0x4F4
    U_VHDPDLASICNT32     VHDPDLASICNT32;//0x4F8
    U_VHDPDLASICNT34     VHDPDLASICNT34;//0x4FC

    unsigned int         Reserved_74[192];
    U_VHDHLCOEF          VHDHLCOEF[68];//0x800~0x90c
    unsigned int         Reserved_75[60];
    U_VHDHCCOEF          VHDHCCOEF[34];//0xa00~0xa84
    unsigned int         Reserved_76[94];
    U_VHDVLCOEF          VHDVLCOEF[68];//0xc00~0xd0c
    unsigned int         Reserved_77[60];
    U_VHDVCCOEF          VHDVCCOEF[34];//0xe00~0xe84
    //add for mmu
    unsigned int         Reserved_78[94];
    MMU_CH_S             MMU_CH[11];//0x1000
    unsigned int         Reserved_79[7];
    U_MMU_TOP_CTL        MMU_TOP_CTL;//0x1200
    U_MMU_TOP_INTS       MMU_TOP_INTS;//0x1204
} S_VPP_REGS_TYPE;
//modified for zme coef config by l50053, end


 /*
typedef struct {
    MMU_CH_S            MMU_CH[11];//0x0000
    U_MMU_TOP_CTL       MMU_TOP_CTL;//0x0200
    U_MMU_TOP_INTS      MMU_TOP_INTS;//0x0204
}S_MMU_REGS_TYPE;
*/
#endif
