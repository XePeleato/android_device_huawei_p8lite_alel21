
/***********************************************************************
 *
 * Copyright (C), 2001-2012, Huawei Tech. Co., Ltd.
 *
 *
 * File Name     : hwifi_sfw_antijam.c
 * Version       : Initial Draft
 * Author        : yWX164715
 * Created       : 2013/3/21
 * Last Modified :
 * Description   : SoftWare anti-jam
 * Function List :
 * History       :
 * 1.Date        : 2013/3/21
 *   Author      : yWX164715
 *   Modification: Created file
 *
 */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * 1 Header File Including
 */

#include <linux/skbuff.h>

#include "hwifi_sfw_antijam.h"
#include "cfg80211_stru.h"
#include "hwifi_cfg80211.h"
#include "hi110x_pm.h"


/*
 * 2 Global Variable Definition
 */

/* Clock frequency plan, frequency points needn't config */
static const uint16 g_cfp_external[] = {
    5500,
    5580,
    5600,
    5660,
    5680,
    5745,
    5765,
    0
};

/* Clock frequency plan config register addresses */
static const uint32 g_cfp_addr[] = {
    0x80004404,             /* cmu_cfg_2_addr                   */
    0x800044E0,             /* p160m_dvalue_en_0_addr           */
    0x800044E0,             /* p160m_dvalue_addr                */
    0x800044E0,             /* p160m_dvalue_en_1_addr           */
    0x800044E4,             /* p80m_dvalue_en_0_addr            */
    0x800044E4,             /* p160m_dvalue_addr                */
    0x800044E4,             /* p160m_dvalue_en_1_addr           */
    0x800044B0,             /* aon_bfg_antii_clkmux_ctl_addr    */
    0x800044A0,             /* aon_wl_antii_clkmux_ctl_addr     */
    0x80004410,             /* cmu_gt_wl_addr                   */
    0                       /* end flag                         */
};

/* Clock frequency plan map table */
static const int16 g_cfp_map[][CFP_MAP_LINE_SIZE] =
{
    {4920,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {4940,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {4960,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {4980,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5040,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5060,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5080,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5100,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5170,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5180,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5190,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5200,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5210,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5220,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5230,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5240,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5260,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5270,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5280,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5300,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5310,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5320,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5500,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5510,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5520,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5540,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5550,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5560,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5580,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5590,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5600,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5620,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5630,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5640,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5660,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5670,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5680,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5700,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5745,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5755,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5765,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5785,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5795,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5805,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5825,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5845,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5865,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5885,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5905,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5925,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {5945,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0,     0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/* Clock frequency plan config register addresses */
static const uint8 g_sv_addr[] = {
    0x70,               /* spur_ctrl_cfg_addr       */
    0x71,               /* spur_phase_step_cfg_addr */
    0x72,               /* spur_cfg_addr            */
    0x73,               /* r_tone_amp_th_addr       */
    0x74,               /* fft_weight_en_addr       */
    0x75,               /* sub_carr_idx0_addr       */
    0x76,               /* sub_carr_idx1_addr       */
    0x77,               /* fft_shift_bit_addr       */
    0x78,               /* sub_carr_idx2_addr       */
    0x79,               /* sub_carr_idx3_addr       */
    0                   /* end flag                 */
};


/* Single voice map table */
static const uint32 g_sv_map[][SV_MAP_LINE_SIZE] =
{/*rf bank5 0x70, 0x71, 0x72,      0x73,       0x74,0x75,   0x76,0x77,  0x78,0x79*/
    {4920,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {4940,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {4960,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {4980,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5040,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5060,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5080,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5100,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5170,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5180,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5190,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5200,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5210,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5220,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5230,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5240,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5260,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5270,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5280,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5300,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5310,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5320,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5500,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5510,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5520,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5540,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5550,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5560,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5580,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5590,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5600,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5620,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5630,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5640,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5660,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5670,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5680,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5700,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5745,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5755,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5765,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x1, 0xB000000,	0x0, 0xC0,	0x0, 0x0},
    {5785,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5795,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5805,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5825,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5845,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5865,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5885,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5905,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5925,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {5945,  0x3,  0x0, 0x0C000024, 0x0000E000, 0x0, 0x0,	0x0, 0x0,	0x0, 0x0},
    {0,     0,    0,   0,          0,          0,   0,      0,   0,     0,   0}
};

/* for dereference globle wlan info */
extern struct wlan_pm_s     *g_wlan_info;

/*
 * Prototype    : hwifi_fill_clk_fplan
 * Description  : clock frequency plan wid set
 * Input        : struct clk_fplan_wid *clk_fplan, struct clk_fplan_param *param
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : yWX164715
 *     Modification : Created function
 */
static void hwifi_fill_clk_fplan(struct clk_fplan_wid *clk_fplan,
                              struct clk_fplan_param *param )
{
    HWIFI_ASSERT((NULL != clk_fplan) && (NULL != param));

    /* fill single voice clk_fplan_wid struct */
    clk_fplan->wid              = WID_FREQ_PLAN;
    clk_fplan->len              = ANTIJAM_WID_DATA_LEN(struct clk_fplan_wid);

    clk_fplan->cmu_cfg_2_addr               = param->cmu_cfg_2_addr;
    clk_fplan->cmu_cfg_2_val_40m            = param->cmu_cfg_2_40m;
    clk_fplan->p160m_dvalue_en_0_addr       = param->p160m_dvalue_en_0_addr;
    clk_fplan->p160m_dvalue_en_0_val_40m    = param->p160m_dvalue_en_0_40m;
    clk_fplan->p160m_dvalue_addr            = param->p160m_dvalue_addr;
    clk_fplan->p160m_dvalue_40m             = param->p160m_dvalue_40m;
    clk_fplan->p160m_dvalue_en_1_addr       = param->p160m_dvalue_en_1_addr;
    clk_fplan->p160m_dvalue_en_1_val_40m    = param->p160m_dvalue_en_1_40m;
    clk_fplan->p80m_dvalue_en_0_addr        = param->p80m_dvalue_en_0_addr;
    clk_fplan->p80m_dvalue_en_0_val_40m     = param->p80m_dvalue_en_0_40m;
    clk_fplan->p80m_dvalue_addr             = param->p80m_dvalue_addr;
    clk_fplan->p80m_dvalue_40m              = param->p80m_dvalue_40m;
    clk_fplan->p80m_dvalue_en_1_addr        = param->p80m_dvalue_en_1_addr;
    clk_fplan->p80m_dvalue_en_1_val_40m     = param->p80m_dvalue_en_1_40m;
    clk_fplan->aon_bfg_antii_clkmux_ctl_addr        = param->aon_bfg_antii_clkmux_ctl_addr;
    clk_fplan->aon_bfg_antii_clkmux_ctl_val_40m     = param->aon_bfg_antii_clkmux_ctl_40m;
    clk_fplan->aon_wl_antii_clkmux_ctl_addr         = param->aon_wl_antii_clkmux_ctl_addr;
    clk_fplan->aon_wl_antii_clkmux_ctl_val_40m      = param->aon_wl_antii_clkmux_ctl_40m;
    clk_fplan->cmu_gt_wl_addr               = param->cmu_gt_wl_addr;
    clk_fplan->cmu_gt_wl_val_40m            = param->cmu_gt_wl_40m;

    clk_fplan->checksum         = bytes_check_sum((uint8 *)&clk_fplan->cmu_cfg_2_addr, clk_fplan->len);
}


/*
 * Prototype    : hwifi_fill_sv_antijam
 * Description  : single voice anti-jam wid set
 * Input        : struct sv_antijam_wid *sv_antijam, struct sv_antijam_param *param
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : yWX164715
 *     Modification : Created function
 */
static void hwifi_fill_sv_antijam(struct sv_antijam_wid *sv_antijam,
                                 struct sv_antijam_param *param )
{
    HWIFI_ASSERT((NULL != sv_antijam) && (NULL != param));

    /* fill single voice sv_antijam_wid struct */
    sv_antijam->wid         = WID_SINGLE_TYPE;
    sv_antijam->len         = ANTIJAM_WID_DATA_LEN(struct sv_antijam_wid);

    sv_antijam->spur_ctrl_cfg_addr          = param->spur_ctrl_cfg_addr;
    sv_antijam->spur_ctrl_cfg_val           = param->spur_ctrl_cfg;
    sv_antijam->spur_phase_step_cfg_addr    = param->spur_phase_step_cfg_addr;
    sv_antijam->spur_phase_step_cfg_val     = param->spur_phase_step_cfg;
    sv_antijam->spur_cfg_addr               = param->spur_cfg_addr;
    sv_antijam->spur_cfg_val                = param->spur_cfg;
    sv_antijam->r_tone_amp_th_addr  = param->r_tone_amp_th_addr;
    sv_antijam->r_tone_amp_th_val   = param->r_tone_amp_th;
    sv_antijam->fft_weight_en_addr  = param->fft_weight_en_addr;
    sv_antijam->fft_weight_en_val   = param->fft_weight_en;
    sv_antijam->subcarr_idx0_addr   = param->subcarr_idx0_addr;
    sv_antijam->subcarr_idx0_val    = param->subcarr_idx0;
    sv_antijam->subcarr_idx1_addr   = param->subcarr_idx1_addr;
    sv_antijam->subcarr_idx1_val    = param->subcarr_idx1;
    sv_antijam->fft_shift_bit_addr  = param->fft_shift_bit_addr;
    sv_antijam->fft_shift_bit_val   = param->fft_shift_bit;
    sv_antijam->subcarr_idx2_addr   = param->subcarr_idx2_addr;
    sv_antijam->subcarr_idx2_val    = param->subcarr_idx2;
    sv_antijam->subcarr_idx3_addr   = param->subcarr_idx3_addr;
    sv_antijam->subcarr_idx3_val    = param->subcarr_idx3;

    sv_antijam->checksum    = bytes_check_sum(&sv_antijam->spur_ctrl_cfg_addr, sv_antijam->len);
}


/*
 * Prototype    : hwifi_fill_dc_offset
 * Description  : dc offset wid set
 * Input        : struct dc_offset_msg *msg, struct dc_offset_param *dc_param
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : yWX164715
 *     Modification : Created function
 */
static void hwifi_fill_dc_offset(struct dc_offset_wid *dc_offset,
                              struct dc_offset_param *param)
{
    HWIFI_ASSERT((NULL != dc_offset) && (NULL != param));

    /* fill single voice dc_offset_wid struct */
    dc_offset->wid          = WID_DC_OFFSET_TYPE;
    dc_offset->len          = ANTIJAM_WID_DATA_LEN(struct dc_offset_wid);

    dc_offset->vga          = param->vga;
    dc_offset->min_i        = param->min_i;
    dc_offset->max_i        = param->max_i;
    dc_offset->min_q        = param->min_q;
    dc_offset->max_q        = param->max_q;
    dc_offset->weigth       = param->weigth;
    dc_offset->comp_swap    = param->comp_swap;
    dc_offset->vlsb         = param->vlsb;
    dc_offset->cali_interv  = param->cali_interv;

    dc_offset->checksum     = bytes_check_sum(&dc_offset->vga, dc_offset->len);
}


/*
 * Prototype    : cfp_find_map_index
 * Description  : return index by find rf frequency point
 * Input        : uint16 cur_freq
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/30
 *     Author       : yWX164715
 *     Modification : Created function
 */
static int32 cfp_find_map_index(uint16 cur_freq)
{
    int16   index;

    /* find current frequency in single voice map */
    for (index = 0; g_cfp_map[index][0] != 0; index++)
    {
        if (cur_freq == g_cfp_map[index][0])
        {
            HWIFI_INFO("Has found index %d.", index);
            return index;
        }
    }

    HWIFI_WARNING("Cann't find the %d frequency in map!", cur_freq);

    return -EFAIL;
}


/*
 * Prototype    : hwifi_curr_freq_get
 * Description  : get valid current frequency
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : uint16
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/05/07
 *     Author       : yWX164715
 *     Modification : Created function
 */
static uint16 hwifi_curr_freq_get(struct cfg_struct *cfg)
{
    uint8                  cur_channel;
    uint16                 cur_freq;

    HWIFI_ASSERT(NULL != cfg);

    /* get station current work channel and frequency */
    if (IS_STA(cfg))
    {
        cur_channel = cfg->sta_info.curr_channel;
    }
    else /* get ap current work channel and frequency */
    {
        cur_channel = cfg->ap_info.curr_channel;
    }

    if (FREQ_CHANNEL_INVALID == cur_channel)
    {
        HWIFI_WARNING("Failed to get current channe ");
        return FREQ_CHANNEL_INVALID;
    }

    cur_freq = hwifi_channel_index_to_freq(cur_channel);
    if (FREQ_CHANNEL_INVALID == cur_freq)
    {
        HWIFI_WARNING("Failed to get current frequency!");
        return FREQ_CHANNEL_INVALID;
    }

    return cur_freq;
}


/*
 * Prototype    : clk_fplan_param_init
 * Description  : init Clock Frequency Plan param
 * Input        : struct clk_fplan_param *clk_fplan, uint32 rf_fpoint
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    : clk_fplan_config
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : yWX164715
 *     Modification : Created function
 */
static int32 clk_fplan_param_init(struct cfg_struct *cfg, struct clk_fplan_param *clk_fplan)
{
    uint16                  cur_freq;
    int32                   index;
    uint16                  i, j;

    HWIFI_ASSERT(NULL != clk_fplan && NULL != cfg);

    if (NULL == g_wlan_info || NULL == g_wlan_info->check_bfg_state)
    {
       HWIFI_ERROR("Cann't get bfg state, check_bfg_state is null.");
       return -EFAIL;
    }

    if (BFG_ENABLED == g_wlan_info->check_bfg_state())
    {
        HWIFI_DEBUG("BFG enabled, needn't frequency plan config.");
        return -NOCFG;
    }

    /* get valid current frequency */
    cur_freq = hwifi_curr_freq_get(cfg);
    if (FREQ_CHANNEL_INVALID == cur_freq)
    {
        HWIFI_WARNING("Failed to get current frequency!");
        return -EFAIL;
    }

    /* judge whether clock frequency plan config or not,
     * according to current rf frequency.
     */
    for (i = 0; g_cfp_external[i] != 0; i++)
    {
        if (g_cfp_external[i] == cur_freq)
        {
            HWIFI_DEBUG("Fixed frequency point, needn't frequency plan config.");
            return -NOCFG;
        }
    }

    /* find the index of current frequency in map */
    index = cfp_find_map_index(cur_freq);
    if (-EFAIL == index)
    {
        HWIFI_WARNING("Failed to find frequency(%u) in map!", cur_freq);
        return -EFAIL;
    }

     /* The first index of clock frequency plan address map */
    i   = 0;

    /* The first index of registers' value */
    j   = 2;

    /* init param according to current frequency */
    clk_fplan->cmu_cfg_2_addr           = g_cfp_addr[i++];
    clk_fplan->cmu_cfg_2_40m            = g_cfp_map[index][j++];
    clk_fplan->p160m_dvalue_en_0_addr   = g_cfp_addr[i++];
    clk_fplan->p160m_dvalue_en_0_40m    = g_cfp_map[index][j++];
    clk_fplan->p160m_dvalue_addr        = g_cfp_addr[i++];
    clk_fplan->p160m_dvalue_40m         = g_cfp_map[index][j++];
    clk_fplan->p160m_dvalue_en_1_addr   = g_cfp_addr[i++];
    clk_fplan->p160m_dvalue_en_1_40m    = g_cfp_map[index][j++];
    clk_fplan->p80m_dvalue_en_0_addr    = g_cfp_addr[i++];
    clk_fplan->p80m_dvalue_en_0_40m     = g_cfp_map[index][j++];
    clk_fplan->p80m_dvalue_addr         = g_cfp_addr[i++];
    clk_fplan->p80m_dvalue_40m          = g_cfp_map[index][j++];
    clk_fplan->p80m_dvalue_en_1_addr    = g_cfp_addr[i++];
    clk_fplan->p80m_dvalue_en_1_40m     = g_cfp_map[index][j++];

    clk_fplan->aon_bfg_antii_clkmux_ctl_addr    = g_cfp_addr[i++];
    clk_fplan->aon_bfg_antii_clkmux_ctl_40m     = g_cfp_map[index][j++];
    clk_fplan->aon_wl_antii_clkmux_ctl_addr     = g_cfp_addr[i++];
    clk_fplan->aon_wl_antii_clkmux_ctl_40m      = g_cfp_map[index][j++];

    clk_fplan->cmu_gt_wl_addr           = g_cfp_addr[i++];
    clk_fplan->cmu_gt_wl_40m            = g_cfp_map[index][j++];

    return SUCC;
}


/*
 * Prototype    : find_rf_fpoint_index
 * Description  : return index by find rf frequency point
 * Input        : uint16 cur_freq
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/30
 *     Author       : yWX164715
 *     Modification : Created function
 */
static int32 find_rf_fpoint_index(uint16 cur_freq)
{
    int16   index;

    /* find current frequency in single voice map */
    for (index = 0; g_sv_map[index][0] != 0; index++)
    {
        if (cur_freq == g_sv_map[index][0])
        {
            HWIFI_INFO("Has found index %d.", index);
            return index;
        }
    }

    HWIFI_WARNING("Can't find the %d frequency in map!", cur_freq);

    return -EFAIL;
}


/*
 * Prototype    : tzone_jam_fpoint
 * Description  : return time zone jam frequency point by find rf frequency point
 * Input        : int32 index
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/30
 *     Author       : yWX164715
 *     Modification : Created function
 */
int32 tzone_jam_fpoint(int32 index)
{
    int32   fpoint;
    uint16  mem_val;
    uint64  shift_save;

    if (g_sv_map[index][1] >= 0)
    {
        /* the most jam frequency point value is positive */
        mem_val     = (uint16)g_sv_map[index][1];
        shift_save  = (uint64)mem_val << 30;

        do_div(shift_save, 80u);
        fpoint      = (int32)shift_save;
    }
    else
    {
        /* the most jam frequency point value is negative */
        mem_val     = (uint16)(-1 * g_sv_map[index][1]);
        shift_save  = (uint64)mem_val << 30;

        do_div(shift_save, 80u);
        fpoint      = (int32)((1u << 30) - (uint32)shift_save);
    }

    HWIFI_DEBUG("Time zone jam frequency point [%d].", fpoint);

    return fpoint;
}


/*
 * Prototype    : calc_subcarr
 * Description  : calculate subcarry wave
 * Input        : int16 fspur
 * Output       : None
 * Return Value : uint32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/05/10
 *     Author       : yWX164715
 *     Modification : Created function
 */
static uint32 calc_subcarr(int16 fspur)
{
    uint16 abs_val;
    uint32 subcarr;

    /* using different algorithm according to fspur signed */
    if (fspur >= 0)
    {
        abs_val             = (uint16)fspur;
        subcarr             = (abs_val * 10000u) / 3125;
    }
    else
    {
        abs_val             = (uint16)(-1 * fspur);
        subcarr             = 128 - (abs_val * 10000u) / 3125;
    }

    return subcarr;
}


/*
 * Prototype    : fzone_subcarr_init
 * Description  : init frequency zone subcarr
 * Input        : int32 index, uint32 *subcarr_idxs
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/30
 *     Author       : yWX164715
 *     Modification : Created function
 */
int32 fzone_subcarr_init(int32 index, uint32 *subcarr_idxs)
{
    uint8   i, j;
    int16   fspur;
    uint32  subcarr;

    HWIFI_ASSERT(subcarr_idxs != NULL);

    /* according to hw info, select two most largest anti-point */
    for (i = 1, j = 0; i < 3; i++, j = j + 2)
    {
        /* 40M recv 40M */
        fspur               = g_sv_map[index][i];
        subcarr             = calc_subcarr(fspur);
        subcarr_idxs[j]     |= subcarr;
        subcarr_idxs[j+1]   |= subcarr + 1;

        /* 20M */
        subcarr_idxs[j]     |= subcarr  << 24;
        subcarr_idxs[j+1]   |= (subcarr + 1) << 24;

        /* 40M recv 20M up band */
        fspur               = g_sv_map[index][i] - 10;
        subcarr             = calc_subcarr(fspur);
        subcarr_idxs[j]     |= subcarr << 8;
        subcarr_idxs[j+1]   |= (subcarr + 1) << 8;

        /* 40M recv 20M down band */
        fspur               = g_sv_map[index][i] + 10;
        subcarr             = calc_subcarr(fspur);
        subcarr_idxs[j]     |= subcarr << 16;
        subcarr_idxs[j+1]   |= (subcarr + 1) << 16;
    }

    HWIFI_DEBUG("Frequency zone subcarr init successfully.");

    return SUCC;
}


/*
 * Prototype    : sv_antijam_param_init
 * Description  : init single voice antijam param
 * Input        : struct cfg_struct *cfg, struct sv_antijam_param *sv_antijam
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/30
 *     Author       : yWX164715
 *     Modification : Created function
 */
static int32 sv_antijam_param_init(struct cfg_struct *cfg, struct sv_antijam_param *sv_antijam)
{
    uint16                 i, j;
    uint16                 cur_freq;
    //int32                  ret = SUCC;
    int32                  index;
    //int32                  jam_fpoint;
    uint32                 subcarr_idxs[4];

    HWIFI_ASSERT((NULL != cfg) && (NULL != sv_antijam));

    memset(subcarr_idxs, 0, sizeof(subcarr_idxs));

    /* get current valid rf work frequency */
    cur_freq = hwifi_curr_freq_get(cfg);
    if (FREQ_CHANNEL_INVALID == cur_freq)
    {
        HWIFI_WARNING("Failed to get current frequency!");
        return -EFAIL;
    }

    HWIFI_DEBUG("The current frequency %d.", cur_freq);

    index = find_rf_fpoint_index(cur_freq);
    if (-EFAIL == index)
    {
        HWIFI_WARNING("Failed to find the %d frequency point jam info!", cur_freq);
        return -EFAIL;
    }

    /* The first index of clock frequency
     * plan address map
     */
    i   = 0;
    j   = 1;

    /* take effect at 5.0Ghz */
    sv_antijam->spur_ctrl_cfg_addr      = g_sv_addr[i++];
    if (cur_freq < HWIFI_BASE_FREQ_4_9G)
    {   /* 2.4Ghz */
        sv_antijam->spur_ctrl_cfg      = 0x00;
    }
    else
    {   /* 5.0Ghz */
        sv_antijam->spur_ctrl_cfg      = g_sv_map[index][j++];
    }


    sv_antijam->spur_phase_step_cfg_addr    = g_sv_addr[i++];
    sv_antijam->spur_phase_step_cfg         = g_sv_map[index][j++];

    /* Init param from globle single voice map */
    sv_antijam->spur_cfg_addr               = g_sv_addr[i++];
    sv_antijam->spur_cfg                    = g_sv_map[index][j++];
    sv_antijam->r_tone_amp_th_addr          = g_sv_addr[i++];
    sv_antijam->r_tone_amp_th               = g_sv_map[index][j++];
    sv_antijam->fft_weight_en_addr          = g_sv_addr[i++];
    sv_antijam->fft_weight_en               = g_sv_map[index][j++];

    /* Init subcarray waves param by calculating
     * frequency zone jam subcarrs
     */
    sv_antijam->subcarr_idx0_addr       = g_sv_addr[i++];
    sv_antijam->subcarr_idx0            = g_sv_map[index][j++];
    sv_antijam->subcarr_idx1_addr       = g_sv_addr[i++];
    sv_antijam->subcarr_idx1            = g_sv_map[index][j++];
    sv_antijam->fft_shift_bit_addr      = g_sv_addr[i++];
    sv_antijam->fft_shift_bit           = g_sv_map[index][j++];
    sv_antijam->subcarr_idx2_addr       = g_sv_addr[i++];
    sv_antijam->subcarr_idx2            = g_sv_map[index][j++];
    sv_antijam->subcarr_idx3_addr       = g_sv_addr[i++];
    sv_antijam->subcarr_idx3            = g_sv_map[index][j++];

    return SUCC;
}


/*
 * Prototype    : wl_clk_fplan_set
 * Description  : clock frequency plan set
 * Input        : struct cfg_struct *cfg, struct clk_fplan_param *param
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : yWX164715
 *     Modification : Created function
 */
static int32 wl_clk_fplan_set(struct cfg_struct *cfg, struct clk_fplan_param *param)
{
    struct sk_buff           *skb;
    struct clk_fplan_msg     *msg;
    uint16                   msg_size;
    int32                    ret;

    msg_size = sizeof(struct clk_fplan_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        HWIFI_WARNING("Failed to alloc for clock frequency plan msg!");
        return -EFAIL;
    }

    /* fill the msg header */
    msg = (struct clk_fplan_msg*)skb_put(skb, sizeof(struct clk_fplan_msg));
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    /* fill clk_fplan_msg */
    hwifi_fill_clk_fplan(&msg->clk_fplan, param);

    HWIFI_DEBUG("\nClock Freq Plan\n"
                "   cmu_cfg_2_addr                  : %x\n"
                "   cmu_cfg_2_val_40m               : %x\n"
                "   p160m_dvalue_en_0_addr          : %x\n"
                "   p160m_dvalue_en_0_val_40m       : %x\n"
                "   p160m_dvalue_addr               : %x\n"
                "   p160m_dvalue_40m                : %x\n"
                "   p160m_dvalue_en_1_addr          : %x\n"
                "   p160m_dvalue_en_1_val_40m       : %x\n"
                "   p80m_dvalue_en_0_addr           : %x\n"
                "   p80m_dvalue_en_0_val_40m        : %x\n"
                "   p80m_dvalue_addr                : %x\n"
                "   p80m_dvalue_40m                 : %x\n"
                "   p80m_dvalue_en_1_addr           : %x\n"
                "   p80m_dvalue_en_1_val_40m        : %x\n"
                "   aon_bfg_antii_clkmux_ctl_addr   : %x\n"
                "   aon_bfg_antii_clkmux_ctl_val_40m: %x\n"
                "   aon_wl_antii_clkmux_ctl_addr    : %x\n"
                "   aon_wl_antii_clkmux_ctl_val_40m : %x\n"
                "   cmu_gt_wl_addr                  : %x\n"
                "   cmu_gt_wl_val_40m               : %x\n"
                "   checksum                        : %x\n",
                msg->clk_fplan.cmu_cfg_2_addr,
                msg->clk_fplan.cmu_cfg_2_val_40m,
                msg->clk_fplan.p160m_dvalue_en_0_addr,
                msg->clk_fplan.p160m_dvalue_en_0_val_40m,
                msg->clk_fplan.p160m_dvalue_addr,
                msg->clk_fplan.p160m_dvalue_40m,
                msg->clk_fplan.p160m_dvalue_en_1_addr,
                msg->clk_fplan.p160m_dvalue_en_1_val_40m,
                msg->clk_fplan.p80m_dvalue_en_0_addr,
                msg->clk_fplan.p80m_dvalue_en_0_val_40m,
                msg->clk_fplan.p80m_dvalue_addr,
                msg->clk_fplan.p80m_dvalue_40m,
                msg->clk_fplan.p80m_dvalue_en_1_addr,
                msg->clk_fplan.p80m_dvalue_en_1_val_40m,
                msg->clk_fplan.aon_bfg_antii_clkmux_ctl_addr,
                msg->clk_fplan.aon_bfg_antii_clkmux_ctl_val_40m,
                msg->clk_fplan.aon_wl_antii_clkmux_ctl_addr,
                msg->clk_fplan.aon_wl_antii_clkmux_ctl_val_40m,
                msg->clk_fplan.cmu_gt_wl_addr,
                msg->clk_fplan.cmu_gt_wl_val_40m,
                msg->clk_fplan.checksum);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Fail to send clock frequency plan set msg!");
        return -EFAIL;
    }

    HWIFI_DEBUG("Succeed to send clock frequency plan set msg.");

    return SUCC;
}


/*
 * Prototype    : wl_sv_antijam_set
 * Description  : single voice antijam set
 * Input        : struct cfg_struct *cfg, struct sv_antijam_param *param
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : yWX164715
 *     Modification : Created function
 */
static int32 wl_sv_antijam_set(struct cfg_struct *cfg, struct sv_antijam_param *param)
{
    uint16                   msg_size;
    int32                    ret;
    struct sk_buff           *skb;
    struct sv_antijam_msg    *msg;

    msg_size = sizeof(struct sv_antijam_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        return -EFAIL;
    }

    /* fill the msg header */
    msg = (struct sv_antijam_msg*)skb_put(skb, sizeof(struct sv_antijam_msg));
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    /* fill sv_antijam_msg */
    hwifi_fill_sv_antijam(&msg->sv_antijam, param);

    HWIFI_DEBUG("\nSingle voice\n"
                "   spur_ctrl_cfg_addr      : %x\n"
                "   spur_ctrl_cfg_val       : %x\n"
                "   spur_phase_step_cfg_addr: %x\n"
                "   spur_phase_step_cfg_val : %x\n"
                "   spur_cfg_addr           : %x\n"
                "   spur_cfg_val            : %x\n"
                "   r_tone_amp_th_addr      : %x\n"
                "   r_tone_amp_th_val       : %x\n"
                "   fft_weight_en_addr      : %x\n"
                "   fft_weight_en_val       : %x\n"
                "   subcarr_idx0_addr       : %x\n"
                "   subcarr_idx0_val        : %x\n"
                "   subcarr_idx1_addr       : %x\n"
                "   subcarr_idx1_val        : %x\n"
                "   fft_shift_bit_addr      : %x\n"
                "   fft_shift_bit_val       : %x\n"
                "   subcarr_idx2_addr       : %x\n"
                "   subcarr_idx2_val        : %x\n"
                "   subcarr_idx3_addr       : %x\n"
                "   subcarr_idx3_val        : %x\n"
                "   checksum                : %x\n",
                msg->sv_antijam.spur_ctrl_cfg_addr,
                msg->sv_antijam.spur_ctrl_cfg_val,
                msg->sv_antijam.spur_phase_step_cfg_addr,
                msg->sv_antijam.spur_phase_step_cfg_val,
                msg->sv_antijam.spur_cfg_addr,
                msg->sv_antijam.spur_cfg_val,
                msg->sv_antijam.r_tone_amp_th_addr,
                msg->sv_antijam.r_tone_amp_th_val,
                msg->sv_antijam.fft_weight_en_addr,
                msg->sv_antijam.fft_weight_en_val,
                msg->sv_antijam.subcarr_idx0_addr,
                msg->sv_antijam.subcarr_idx0_val,
                msg->sv_antijam.subcarr_idx1_addr,
                msg->sv_antijam.subcarr_idx1_val,
                msg->sv_antijam.fft_shift_bit_addr,
                msg->sv_antijam.fft_shift_bit_val,
                msg->sv_antijam.subcarr_idx2_addr,
                msg->sv_antijam.subcarr_idx2_val,
                msg->sv_antijam.subcarr_idx3_addr,
                msg->sv_antijam.subcarr_idx3_val,
                msg->sv_antijam.checksum);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Fail to send Single Voice Anti-Jam set msg!");
        return -EFAIL;
    }

    HWIFI_DEBUG("Succeed to send Single Voice Anti-Jam set msg.");

    return SUCC;
}


/*
 * Prototype    : wl_dc_offset_set
 * Description  : dc offset thresholds and weigth set
 * Input        : struct cfg_struct *cfg, struct dc_offset_param *param
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    : hwifi_ioctl_cmd()
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : yWX164715
 *     Modification : Created function
 */
int32 wl_dc_offset_set(struct cfg_struct *cfg, struct dc_offset_param *param)
{
    struct sk_buff           *skb;
    struct dc_offset_msg     *msg;
    uint16                   msg_size;
    int32                    ret;

    msg_size = sizeof(struct dc_offset_msg);

    skb = hwifi_alloc_skb_for_cmd(msg_size);
    if (NULL == skb)
    {
        HWIFI_WARNING("Failed to alloc for dc offset msg");
        return -EFAIL;
    }

    /* fill the msg header */
    msg = (struct dc_offset_msg*)skb_put(skb, sizeof(struct dc_offset_msg));
    hwifi_fill_msg_hdr(&msg->msg_hdr, HOST_CMD_CONFIG, msg_size);

    /* fill dc_offset_msg */
    hwifi_fill_dc_offset(&msg->dc_offset, param);

    HWIFI_DEBUG( "\nDC Offset\n"
                 "   vga_gain_threshold: %x\n"
                 "   min_dc_i_threshold: %x\n"
                 "   max_dc_i_threshold: %x\n"
                 "   min_dc_q_threshold: %x\n"
                 "   max_dc_q_threshold: %x\n"
                 "   dc_i_q_threshold  : %x\n"
                 "   dc_comp_swap      : %x\n"
                 "   vlsb              : %x\n"
                 "   cali_interval     : %x\n"
                 "   checksum          : %x\n",
                 msg->dc_offset.vga,
                 msg->dc_offset.min_i,
                 msg->dc_offset.max_i,
                 msg->dc_offset.min_q,
                 msg->dc_offset.max_q,
                 msg->dc_offset.weigth,
                 msg->dc_offset.comp_swap,
                 msg->dc_offset.vlsb,
                 msg->dc_offset.cali_interv,
                 msg->dc_offset.checksum);

    ret = hwifi_send_cmd(cfg, skb);
    if (SUCC != ret)
    {
        HWIFI_WARNING("Failed to send dc offset set msg!");
        return -EFAIL;
    }

    cfg->dc_offset.vga          = param->vga;
    cfg->dc_offset.max_i        = param->max_i;
    cfg->dc_offset.min_i        = param->min_i;
    cfg->dc_offset.max_q        = param->max_q;
    cfg->dc_offset.min_q        = param->min_q;
    cfg->dc_offset.weigth       = param->weigth;
    cfg->dc_offset.comp_swap    = param->comp_swap;
    cfg->dc_offset.vlsb         = param->vlsb;
    cfg->dc_offset.cali_interv  = param->cali_interv;

    HWIFI_DEBUG("Succeed to send dc offset thresholds msg");

    return SUCC;
}


/*
 * Prototype    : wl_aggre_mode_get
 * Description  : get device aggregation mode
 * Input        : struct cfg_struct *cfg, struct wifi_ioctl_data_struct *ioctl_data
 * Output       : NONE
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/23
 *     Author       : ywx164715
 *     Modification : Created function
 *
 */
int32 wl_dc_offset_get(struct cfg_struct *cfg, struct dc_offset_param *param)
{
    HWIFI_ASSERT(cfg != NULL);

    /* if getting dc offset values before setting it at first, all values is zero */
    param->vga           = cfg->dc_offset.vga;
    param->min_i         = cfg->dc_offset.min_i;
    param->max_i         = cfg->dc_offset.max_i;
    param->min_q         = cfg->dc_offset.min_q;
    param->max_q         = cfg->dc_offset.max_q;
    param->weigth        = cfg->dc_offset.weigth;
    param->comp_swap     = cfg->dc_offset.comp_swap;
    param->vlsb          = cfg->dc_offset.vlsb;
    param->cali_interv   = cfg->dc_offset.cali_interv;

    return SUCC;
}


/*
 * Prototype    : clk_fplan_config
 * Description  : Clock Frequency Plan config interface
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/21
 *     Author       : yWX164715
 *     Modification : Created function
 */
void clk_fplan_config(struct cfg_struct *cfg)
{
    int32                   ret;
    struct clk_fplan_param  clk_fplan;

    HWIFI_ASSERT(NULL != cfg);

    ret = clk_fplan_param_init(cfg, &clk_fplan);
    if (-EFAIL == ret)
    {
        HWIFI_WARNING("Failed to init clock frequency plan param!");
        return;
    }
    else if (-NOCFG == ret)
    {
        HWIFI_WARNING("Clock frequency plan needn't config!");
        return;
    }

    ret = wl_clk_fplan_set(cfg, &clk_fplan);
    if (-EFAIL == ret)
    {
        HWIFI_WARNING("Failed to config Clock frequency plan!");
        return;
    }

    HWIFI_DEBUG("Clock frequency plan config successfully.");
}


/*
 * Prototype    : sv_antijam_config
 * Description  : single voice antijam config
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/03/30
 *     Author       : yWX164715
 *     Modification : Created function
 */
static void sv_antijam_config(struct cfg_struct *cfg)
{
    int32                   ret;
    struct sv_antijam_param sv_antijam;

    HWIFI_ASSERT(cfg != NULL);

    ret = sv_antijam_param_init(cfg, &sv_antijam);
    if (-EFAIL == ret)
    {
        HWIFI_WARNING("Failed to init single voice anti-jam config param!");
        return;
    }

    ret = wl_sv_antijam_set(cfg, &sv_antijam);
    if (-EFAIL == ret)
    {
        HWIFI_WARNING("Failed to config single voice anti-jam !");
        return;
    }

    HWIFI_DEBUG("Single voice antijam config successfully.");
}


/*
 * Prototype    : sfw_antijam_dynamic_config
 * Description  : dynamic SoftWare Anti-jam config
 * Input        : struct cfg_struct *cfg
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/04/01
 *     Author       : yWX164715
 *     Modification : Created function
 */
 void sfw_antijam_dynamic_config(struct cfg_struct *cfg)
 {
    HWIFI_ASSERT(NULL != cfg);
// cancel by luolingzhi
#if 0
    /* clock frequency plan config */
    clk_fplan_config(cfg);
#endif
    /* Single voice antijam config */
    sv_antijam_config(cfg);

    HWIFI_DEBUG("Software antijam dynamic config end.");
 }

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif  /* end of hwifi_sfw_antijam.c */

