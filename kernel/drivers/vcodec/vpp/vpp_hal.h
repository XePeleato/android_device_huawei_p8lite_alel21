/*
 **************************************************************************************
 *
 *       Filename:   vpp_hal.h
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011-08-10 16:20:00
 *         Author:  j00140427
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */
#ifndef __VPP_HAL_H__
#define __VPP_HAL_H__

#include <linux/slab.h>
#include <linux/of_address.h>
#include <asm/io.h>

#include "vpp_define.h"
#include "vpp_reg.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          This is defined for FPGA test and SDK
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned int  reg_read(unsigned int *addr);
void reg_write(unsigned int *addr, unsigned int value);

//初始化寄存器
int hal_init(struct device_node *node);
void hal_deinit(void);
void hal_set_regup(HAL_LAYER_E enLayer);

//查询视频层输入分辨率
void hal_die_layer_inreso(HAL_LAYER_E enLayer, unsigned int *pu32W, unsigned int *pu32H);
void hal_set_layer_rect(HAL_LAYER_E enLayer, HAL_RECT_S  stVideoRect);

void hal_set_lnk_lst_node_int_en(unsigned int u32Data);

void hal_set_pd_defthd(HAL_LAYER_E enLayer);
void hal_set_clk_gate_en(unsigned int u32Data);
void hal_set_intf_csc_coef(HAL_LAYER_E enLayer, HAL_CSCCOEF_S stCscCoef);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          There are common functions for both EDA and FPGA, SDK
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//-----------------------------------INTF------------------------------------//
//获取中断状态
unsigned int hal_get_int_sta(unsigned int u32IntMsk);
//清除中断状态
void hal_clear_int_sta(unsigned int u32IntMsk);
//打开中断Mask
void hal_set_int_mask(unsigned int u32MaskEn);
//关闭中断Mask
void hal_dis_int_mask(unsigned int u32MaskEn);
//打开中断& 打开WBC0/1 中断使能控制
void hal_set_int_enable(unsigned int u32MaskEn);
//关闭中断& 打开WBC0/1 中断使能控制
void hal_set_int_disable(unsigned int u32MaskEn);
//设置总线性能参数
void hal_set_rd_bus_id(unsigned int bMode);
void hal_set_rd_out_std(unsigned int bIdMd, unsigned char u8OsData);
void hal_set_wr_out_std(unsigned char u8OsData);

//-----------------------------------VIDEO------------------------------------//
void hal_enable_layer(HAL_LAYER_E enLayer, unsigned int bEnable);
//设置视频层读取地址
void hal_set_layer_addr(HAL_LAYER_E enLayer, unsigned char u8Chan, unsigned int u32LAddr, unsigned int u32CAddr, unsigned short u16LStr, unsigned short u16CStr);
//设置层读取模式
void hal_set_read_mode(HAL_LAYER_E enLayer,HAL_DATARMODE_E enLRMode,HAL_DATARMODE_E enCRMode);
//设置层场序信息
void hal_set_field_order   (HAL_LAYER_E enLayer, VPP_VHD_FOD_E uFieldOrder);
//设置层数据类型
//void hal_set_layer_data_fmt(HAL_LAYER_E enLayer,VPP_PIXELFORMAT_E  enDataFmt);
void  hal_set_layer_data_fmt(HAL_LAYER_E enLayer,
                                            VPP_PIXELFORMAT_E  enDataFmt,unsigned int w,unsigned int h);
void hal_set_zme_copy(unsigned int vsc_copy);
void hal_set_zme_core(HAL_LAYER_E enLayer, unsigned int vsc_core);
//设置层插值模式
void hal_set_ifir_mode(HAL_LAYER_E enLayer, HAL_IFIRMODE_E enMode);
void hal_set_ifir_coef(HAL_LAYER_E enLayer, int * u32Coef);
//设置层的背景色
void hal_set_layer_bkg(HAL_LAYER_E enLayer, HAL_BKCOLOR_S stBkg);
//设置层的timeout
void hal_set_time_out(HAL_LAYER_E enLayer, unsigned char u8TData);
//设置层寄存器更新
void hal_set_regupNoRatio(HAL_LAYER_E enLayer);
//-----ACM FUNC----//
void hal_set_acm_enable(HAL_ACMBLK_ID_E enAcmId, unsigned int bAcmEn);
void hal_set_acm_coef(HAL_ACMBLK_ID_E enAcmId, HAL_ACMBLKINFO_S stCoef);
void hal_set_acm_test_enable(unsigned int bAcmEn);
//-----ACC FUNC----//
void hal_set_acc_thd(HAL_LAYER_E enLayer, HAL_ACCTHD_S stAccThd);
void hal_set_acc_tab(HAL_LAYER_E enLayer, unsigned int *upTable);
void hal_set_acc_ctrl(HAL_LAYER_E enLayer, unsigned int uAccEn, unsigned int uAccMode);
void hal_set_acc_weight_addr(unsigned int uAccAddr);
void hal_set_acc_rst(HAL_LAYER_E enLayer, unsigned int uAccRst);
//-----CSC FUNC----//
//设置层色彩空间转换参数
void hal_layer_set_csccoef(HAL_LAYER_E enChan, HAL_INTFCSCCOEF_S *pstCscCoef);
//设置层CSC直流分量
void hal_layer_set_cscdccoef(HAL_LAYER_E enChan, HAL_INTFCSCDCCOEF_S *pstCscDcCoef);
//配置层CSC使能
void hal_set_layer_csc(HAL_LAYER_E enLayer, unsigned int bCscEn);
//配置源窗口
void hal_set_layer_in_rect(HAL_LAYER_E enLayer, RECT_S stRect);
//配置输出窗口
void hal_set_layer_out_rect(HAL_LAYER_E enLayer, RECT_S stRect);
//设置视频显示区域窗口
void hal_set_layer_disp_rect(HAL_LAYER_E enLayer, RECT_S stRect);

//-----------------------------------WBC------------------------------------//
void hal_set_wbc_enable(HAL_LAYER_E enLayer, unsigned int bEnable);
void hal_set_wbc_addr(HAL_LAYER_E enLayer, unsigned int u32Addr);
void hal_set_wbc_stride(HAL_LAYER_E enLayer, unsigned short u16Str);
void hal_set_wbc_out_intf(HAL_LAYER_E enLayer, HAL_DATARMODE_E enRdMode);
void hal_set_wbc_md    (HAL_WBC_RESOSEL_E enMdSel);
void hal_set_wbc_spd   (HAL_LAYER_E enLayer, unsigned int u16ReqSpd);
void hal_set_wbc_out_fmt(HAL_LAYER_E enLayer, VPP_INTFDATAFMT_E stIntfFmt);
void hal_set_lnk_wb_enable(unsigned int u32Enable);
void hal_set_wbc_wr_mode(HAL_LAYER_E enLayer, unsigned int u32Data);

//-----------------------------------DIE------------------------------------//
void hal_die_addr(HAL_LAYER_E enLayer, unsigned int uDieAddr);
void hal_set_die_enable(HAL_LAYER_E enLayer, unsigned int bDieLumEn, unsigned int bDieChmEn);
void hal_set_die_outsel(HAL_LAYER_E enLayer, unsigned int bOutSel);
void hal_set_die_chm_outsel(HAL_LAYER_E enLayer, unsigned int bOutSel);
void hal_set_die_chm_mode(HAL_LAYER_E enLayer, unsigned int bChmMode);
void hal_set_die_stp_rst(HAL_LAYER_E enLayer, unsigned int bRstEn);
void hal_set_die_stp_update(HAL_LAYER_E enLayer, unsigned int bUpdateEn);
void hal_set_die_dir_inten(HAL_LAYER_E enLayer, unsigned int dData);
void hal_set_die_ver_dir_inten(HAL_LAYER_E enLayer, unsigned int dData);
void hal_set_die_ver_min_inten(HAL_LAYER_E enLayer, short s16Data);
void hal_set_die_scale(HAL_LAYER_E enLayer, unsigned char u8Data);
void hal_set_die_ck_gain(HAL_LAYER_E enLayer, unsigned char u8CkId, unsigned int dData);
void hal_set_die_ck_gn_range(HAL_LAYER_E enLayer, unsigned char u8CkId, unsigned int dData);
void hal_set_die_ck_max_range(HAL_LAYER_E enLayer, unsigned char u8CkId, unsigned char u8Data);
void hal_set_die_def_thd(HAL_LAYER_E enLayer);
void hal_set_die_luma_mode(HAL_LAYER_E enLayer, unsigned int bLumaMode);
void hal_set_die_luma_q_tab(HAL_LAYER_E enLayer, unsigned char * up8Thd);
void hal_set_die_luma_req_tab(HAL_LAYER_E enLayer, unsigned char * up8Thd);
void hal_set_die_scaleRatio(HAL_LAYER_E enLayer, unsigned int u32Ratio);
void hal_set_die_win_size(HAL_LAYER_E enLayer, unsigned int u32Size);
void hal_set_die_sce_max(HAL_LAYER_E enLayer, unsigned int u32Sel);
void hal_set_die_qrst_max(HAL_LAYER_E enLayer, unsigned int u32Sel);
void hal_set_die_st_enable(HAL_LAYER_E enLayer, VPP_DIE_STMD_E enMd, unsigned int u32Enable);
void hal_set_die_st_addr(HAL_LAYER_E enLayer, VPP_DIE_STMD_E enMd, unsigned int u32Addr);
void hal_set_die_addr(HAL_LAYER_E enLayer, VPP_DIE_STMD_E enMd, unsigned int *pu32AddrCtrl, int nRepeat);
void hal_set_die_chm_ccr_enable(HAL_LAYER_E enLayer, unsigned int u32En);
void hal_set_die_chm_ma_offset(HAL_LAYER_E enLayer, unsigned int u32Off);
void hal_set_die_xchm_max(HAL_LAYER_E enLayer, unsigned int u32Max);
void hal_set_die_ccr_detect(HAL_LAYER_E enLayer, unsigned int u32Thd, unsigned int u32Max, unsigned int u32Bld);
void hal_set_die_similar(HAL_LAYER_E enLayer, unsigned int u32Thd, unsigned int u32Max);
void hal_set_die_st_mode(HAL_LAYER_E enLayer, unsigned int enMd);
void hal_set_die_dir_mult(HAL_LAYER_E enLayer, unsigned int u32Dir, unsigned int u32Data);
void hal_set_die_mf_max(HAL_LAYER_E enLayer, unsigned int u32LumMd, unsigned int u32ChmMd);
void hal_set_die_ck_enh(HAL_LAYER_E enLayer, unsigned int u32Enh);

void hal_set_diestpflt_enable(HAL_LAYER_E enLayer, unsigned int bFltEn);
void hal_set_diesadthd(HAL_LAYER_E enLayer, unsigned char u8Thd);
void hal_set_diestdthd(HAL_LAYER_E enLayer, unsigned char u8Thd);
void hal_set_diestpthd(HAL_LAYER_E enLayer, unsigned char u8Thd);
void hal_set_diesadthdtab(HAL_LAYER_E enLayer, unsigned char * up8Thd);
void hal_set_diesadtsmix(HAL_LAYER_E enLayer, unsigned char u8Thd, unsigned char u8Rate);
void hal_set_diestdtsmix(HAL_LAYER_E enLayer, unsigned char u8Thd, unsigned char u8Rate);

//-----------------------------------PULL DOWN------------------------------------//
void hal_set_pd_dir_mch(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_chm_dir_mch(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_blk_pos(HAL_LAYER_E enLayer, unsigned int u32Mode, unsigned int u32PosX, unsigned int u32PosY);
void hal_set_pd_stl_blk_thd(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_diff_thd(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_hist_thd(HAL_LAYER_E enLayer, unsigned int * u32Data);
void hal_set_pd_um_thd(HAL_LAYER_E enLayer, unsigned int * u32Data);
void hal_set_pd_coring_tkr(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_coring_norm(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_coring_blk(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_pcc_hthd(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_pcc_vthd(HAL_LAYER_E enLayer, unsigned int * u32Data);
void hal_set_pd_it_diff_thd(HAL_LAYER_E enLayer, unsigned int * u32Data);
void hal_set_pd_lasi_thd(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_edge_thd(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_smt_enable(HAL_LAYER_E enLayer, unsigned int u32Data);
void hal_set_pd_smt_thd(HAL_LAYER_E enLayer, unsigned int u32Data);


//-----------------------------------ZME------------------------------------//
void hal_set_coef_addr     (HAL_LAYER_E enLayer, HAL_COEFMODE_E enMode, unsigned int u32Addr);
void hal_set_layer_para_upd      (HAL_LAYER_E enLayer, HAL_COEFMODE_E enMode);
void hal_set_zme_enable    (HAL_LAYER_E enLayer, HAL_ZMEMODE_E enMode, unsigned int bEnable);
void hal_set_zme_fir_enable (HAL_LAYER_E enLayer, HAL_ZMEMODE_E enMode, unsigned int bEnable);
void hal_set_mid_enable    (HAL_LAYER_E enLayer, HAL_ZMEMODE_E enMode, unsigned int bEnable);
void hal_set_hfir_order    (HAL_LAYER_E enLayer, unsigned int uHfirOrder);
void HAL_SetMirNum       (HAL_LAYER_E enLayer, HAL_ZMEMODE_E enMode, unsigned int uMirNum);
void hal_set_zme_vertap    (HAL_LAYER_E enLayer, HAL_ZMEMODE_E enMode, unsigned int uVerTap);
void hal_set_zme_ver_type   (HAL_LAYER_E enLayer, unsigned int uVerType,unsigned int w,unsigned int h);
void hal_set_zme_phase     (HAL_LAYER_E enLayer, HAL_ZMEMODE_E enMode, int s32Phase);
void hal_set_zme_reso      (HAL_LAYER_E enLayer, HAL_RECT_S stZmeReso);
void hal_set_ver_ratio     (HAL_LAYER_E enLayer, unsigned int uRatio);
void hal_set_hor_ratio     (HAL_LAYER_E enLayer, unsigned int uRatio);
//------------------------------查询状态----------------------------------//
//查询层开关状态
unsigned int hal_get_layer_enable(HAL_LAYER_E enLayer);

//查询输出窗口
void  hal_get_layer_out_rect(HAL_LAYER_E enLayer, RECT_S *pstRect);

void hal_set_zme_ratio(void);
void log_reg_value(void);

//add for mmu
void hal_set_mmu_burst(unsigned int bmd);
void hal_set_mmu_core(unsigned int comd);
void hal_set_mmu_cfg(unsigned int cfgmd);
void hal_set_mmu_tile_num(unsigned int width,unsigned int rwdata);
void hal_set_mmu_bit7_xor(unsigned int bit7xor,unsigned int rwdata);
void hal_set_mmu_in_tile_enb(unsigned int intile);
void hal_set_mmu_out_tile_enb(unsigned int outile);
void hal_set_mmu_ch_ptba(unsigned int chnum,unsigned int pt_baddr);
void hal_set_mmu_ch_ptva(unsigned int chnum,unsigned int pt_vaddr);
void hal_enb_mmu_ch_inte(unsigned int chnum,unsigned int inte);
unsigned int hal_get_mmu_ch_ints(unsigned int chnum);
unsigned int hal_get_mmu_ints(void);
void hal_clear_mmu_ch_intc(unsigned int chnum,unsigned int intc);
void hal_set_mmu_enable(HAL_LAYER_E enLayer,VPP_DIE_MODE_E edie_mode,VPP_PIXELFORMAT_E pixformat,unsigned int dieable,unsigned int mmu_enb);
void hal_set_mmu_ch_start_addr(unsigned int chnum,unsigned int startaddr);
void hal_set_mmu_ch_end_addr(unsigned int chnum,unsigned int endaddr);
void hal_set_mmu_ch_addr(VPP_CONFIG_S *pVppConfig);
void hal_set_mmu__start(unsigned int flag);

    #endif

