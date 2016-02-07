/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name     : hwifi_sfw_antijam.h
 * Version       : Initial Draft
 * Author        : yWX164715
 * Created       : 2013/03/21
 * Last Modified :
 * Description   : hwifi_sfw_antijam.h header file
 * Function List :
 * History       :
 *  1.Date        : 2013/03/21
      Author      : yWX164715
      Modification: Created file
 *
 */

#ifndef __HWIFI_SF_ANTIJAM_H__
#define __HWIFI_SF_ANTIJAM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include "hwifi_msg.h"
#include "hwifi_cfgapi.h"

/*
 * 2 Macro Definition
 */

/* sv antijam map line size */
#define CFP_MAP_LINE_SIZE      12
#define SV_MAP_LINE_SIZE       11
#define TZONE_JAM_INDEX        1
#define NOCFG                  2

#define FREQ_CHANNEL_INVALID   0
#define BFG_ENABLED            1

/* SFW Anti-Jam WID length */
#define ANTIJAM_WID_DATA_LEN(wid_type) (sizeof(wid_type) - sizeof(struct binary_wid) - 1)

/*
 * 3 Enum Type Definition
 */


/*
 * 4 Global Variable Declaring
 */


/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
#pragma pack(1)

/* Clock Frequency Plan Msg Definition */
struct clk_fplan_wid
{
    uint16  wid;
    uint16  len;

    uint32  cmu_cfg_2_addr;
    uint16  cmu_cfg_2_val_40m;
    uint32  p160m_dvalue_en_0_addr;
    uint16  p160m_dvalue_en_0_val_40m;
    uint32  p160m_dvalue_addr;
    uint16  p160m_dvalue_40m;
    uint32  p160m_dvalue_en_1_addr;
    uint16  p160m_dvalue_en_1_val_40m;
    uint32  p80m_dvalue_en_0_addr;
    uint16  p80m_dvalue_en_0_val_40m;
    uint32  p80m_dvalue_addr;
    uint16  p80m_dvalue_40m;
    uint32  p80m_dvalue_en_1_addr;
    uint16  p80m_dvalue_en_1_val_40m;
    uint32  aon_bfg_antii_clkmux_ctl_addr;
    uint16  aon_bfg_antii_clkmux_ctl_val_40m;
    uint32  aon_wl_antii_clkmux_ctl_addr;
    uint16  aon_wl_antii_clkmux_ctl_val_40m;
    uint32  cmu_gt_wl_addr;
    uint16  cmu_gt_wl_val_40m;

    uint8   checksum;
};

struct clk_fplan_msg
{
    struct hwifi_msg_header msg_hdr;
    struct clk_fplan_wid clk_fplan ;
};

/* Single Voice Anti-Jam Msg Definition */
struct sv_antijam_wid
{
    uint16  wid;
    uint16  len;

    uint8   spur_ctrl_cfg_addr;
    uint32  spur_ctrl_cfg_val;          /* bit[0]:0 2.4Ghz or bit[0]:1 5.0Ghz */
    uint8   spur_phase_step_cfg_addr;
    uint32  spur_phase_step_cfg_val;
    uint8   spur_cfg_addr;
    uint32  spur_cfg_val;
    uint8   r_tone_amp_th_addr;
    uint32  r_tone_amp_th_val;
    uint8   fft_weight_en_addr;
    uint32  fft_weight_en_val;
    uint8   subcarr_idx0_addr;
    uint32  subcarr_idx0_val;
    uint8   subcarr_idx1_addr;
    uint32  subcarr_idx1_val;
    uint8   fft_shift_bit_addr;
    uint32  fft_shift_bit_val;          /* fft weigth enable signal */
    uint8   subcarr_idx2_addr;
    uint32  subcarr_idx2_val;           /* subcarr according to map */
    uint8   subcarr_idx3_addr;
    uint32  subcarr_idx3_val;

    uint8   checksum;
};

/* single voice antijam msg def */
struct sv_antijam_msg
{
    struct hwifi_msg_header msg_hdr;
    struct sv_antijam_wid sv_antijam ;
};


/* DC_OFFSET Msg Definition. */
struct dc_offset_wid
{
    uint16  wid;
    uint16  len;

    uint8   vga;
    uint8   max_i;
    uint8   min_i;
    uint8   max_q;
    uint8   min_q;
    uint8   weigth;
    uint8   comp_swap;
    uint8   vlsb;
    uint8   cali_interv;

    uint8   checksum;
};
struct dc_offset_msg {
    struct hwifi_msg_header msg_hdr;
    struct dc_offset_wid    dc_offset;
};

#pragma pack()

/* SoftWare Anti-Jam Msg definition end */


/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
extern int32 wl_dc_offset_set(struct cfg_struct *cfg, struct dc_offset_param *param);
extern int32 wl_dc_offset_get(struct cfg_struct *cfg, struct dc_offset_param *param);

extern void sfw_antijam_dynamic_config(struct cfg_struct *cfg);


#ifdef __cplusplus
    #if __cplusplus
         }
    #endif
#endif

#endif /* end of hwifi_sfw_antijam.h */


