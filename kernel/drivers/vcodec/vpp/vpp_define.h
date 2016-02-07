/*
 **************************************************************************************
 *
 *       Filename:  vpp_define.h
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011-08-10 16:20:00
 *         Author:  j00140427
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#ifndef _VPP_DEFINE_H_
#define _VPP_DEFINE_H_

#include "vpp_drv_ext.h"

#define LINK_NUM 4
#define LINK_WB_LENGTH 16*4
#define ACC_CNT_NUM 16

#define VPP_RECT_MIN_W 64
#define VPP_RECT_MIN_H 64
#define VPP_RECT_MIN_X 0
#define VPP_RECT_MIN_Y 0
#define VPP_RECT_MAX_W 1920
#define VPP_RECT_MAX_H 1080
#define VPP_RECT_MAX_X (VPP_RECT_MAX_W - VPP_RECT_MIN_W)
#define VPP_RECT_MAX_Y (VPP_RECT_MAX_H - VPP_RECT_MIN_H)
#define OPTM_ALG_ACMCHECKPRINT 1
#define VPP_ACM_ABS(x)     (((x) < 0) ? -(x) : (x))
#define VPP_ACM_MIN2(x, y) (((x) < (y)) ? (x) : (y))
#define VPP_ACM_MAX2(x, y) (((x) > (y)) ? (x) : (y))

#define VPP_ALG_ACC_COEF_NUM (256*2)

typedef struct VPP_ADDRINFO_S_
{
    unsigned int   vhd_clum_addr;
    unsigned int   vhd_cchm_addr;

    unsigned int   vhd_llum_addr;       /*single frame,  planar : V address ; */
    unsigned int   vhd_lchm_addr;       /*single frame,  planar : not used ; */

    unsigned int   wbc1_addr ;

    unsigned int   die_BusAddr;
    unsigned int   die_VirtAddr;
    unsigned int   die_size;

    unsigned int   zme_BusAddr;
    unsigned int   zme_VirtAddr;

    unsigned int   acc_BusAddr;
    unsigned int   acc_VirtAddr;

    unsigned int   lluma_addr;       /* just for deinterlace*/
    unsigned int   lchroma_addr;
    unsigned int   cluma_addr;
    unsigned int   cchroma_addr;
    unsigned int   nluma_addr;
    unsigned int   nchroma_addr;

    unsigned short vhd_lum_str;     /*stride */
    unsigned short vhd_chm_str;
    unsigned short wbc1_str;
}VPP_ADDRINFO_S;

typedef enum HAL_LAYER_E_
{
    HAL_LAYER_VIDEO1 = 0,
    HAL_LAYER_WBC1,
    HAL_LAYER_BUTT
} HAL_LAYER_E;

/*视频数据读取模式*/
typedef enum HAL_DATARMODE_E_
{
    HAL_INTERLACE = 0,
    HAL_PROGRESSIVE = 1,
    HAL_TOP,
    HAL_BOTTOM,

    HAL_DATARMODE_BUTT
} HAL_DATARMODE_E;

/* vou interrupt mask type */
typedef enum HAL_INTERRUPTMASK_E_
{
    HAL_INTMSK_NONE      = 0,

    HAL_INTMSK_DSDVTTHD  = 0x1,
    HAL_INTMSK_DSDVTTHD2 = 0x2,
    HAL_INTMSK_DSDVTTHD3 = 0x4,
    HAL_INTMSK_DSDUFINT  = 0x8,

    HAL_INTMSK_DHDVTTHD  = 0x10,
    HAL_INTMSK_DHDVTTHD2 = 0x20,
    HAL_INTMSK_DHDVTTHD3 = 0x40,
    HAL_INTMSK_DHDUFINT  = 0x80,

    HAL_INTMSK_WTEINT   = 0x100,
    HAL_INTMSK_VTEINT   = 0x200,
    HAL_INTMSK_WBC2TEINT = 0x800,

    HAL_INTMSK_LNKTEINT = 0x1000,

    HAL_INTMSK_VSDRRINT = 0x100000,
    HAL_INTMSK_VADRRINT = 0x200000,
    HAL_INTMSK_VHDRRINT = 0x400000,
    HAL_INTMSK_G0RRINT  = 0x800000,

    HAL_INTMSK_G1RRINT = 0x1000000,

    HAL_INTMSK_RRERRINT = 0x10000000,
    HAL_INTMSK_UTENDINT = 0x40000000,
    HAL_INTMSK_BUSEERINT = 0x80000000,

    HAL_INTREPORT_ALL = 0xffffffff
} HAL_INTERRUPTMASK_E;

/* vpp coef load mode */
typedef enum HAL_COEFMODE_E_
{
    HAL_COEFMODE_HOR  = 0,
    HAL_COEFMODE_VER,
    HAL_COEFMODE_LUT,
    HAL_COEFMODE_GAM,
    HAL_COEFMODE_ACC,

    HAL_COEFMODE_ALL
}HAL_COEFMODE_E;

/* vpp zoom mode */
typedef enum HAL_ZMEMODE_E_
{
    HAL_ZMEMODE_HORL = 0,
    HAL_ZMEMODE_HORC,
    HAL_ZMEMODE_VERL,
    HAL_ZMEMODE_VERC,

    HAL_ZMEMODE_HOR,
    HAL_ZMEMODE_VER,
    HAL_ZMEMODE_ALPHA,
    HAL_ZMEMODE_ALPHAV,
    HAL_ZMEMODE_VERT,
    HAL_ZMEMODE_VERB,

    HAL_ZMEMODE_ALL,
    HAL_ZMEMODE_NONL,
    HAL_ZMEMODE_BUTT

}HAL_ZMEMODE_E;

/* vpp wbc online mode */
typedef enum HAL_WBC_RESOSEL_E_
{
    HAL_WBC_RESOSEL_ONL = 0,
    HAL_WBC_RESOSEL_OFL,

    HAL_WBC_RESOSEL_BUTT
}HAL_WBC_RESOSEL_E;

/* RM frame or field infomation */
typedef enum HAL_IFIRMODE_E_
{
    HAL_IFIRMODE_DISEN = 0,  //保留
    HAL_IFIRMODE_COPY,      //色度IFIR复制模式
    HAL_IFIRMODE_DOUBLE,     //双线性插值
    HAL_IFIRMODE_6TAPFIR,    //6阶IFIR

    HAL_IFIRMODE_BUTT
}HAL_IFIRMODE_E;

/* vpp acm block id information */
typedef enum HAL_ACMBLK_ID_E_
{
    HAL_ACMBLK_ID0,
    HAL_ACMBLK_ID1,
    HAL_ACMBLK_ID2,
    HAL_ACMBLK_ID3,

    HAL_ACMBLK_ALL,

    HAL_ACMBLK_BUTT

}HAL_ACMBLK_ID_E;

typedef struct HAL_ACMBLKINFO_S_
{
    unsigned int   bacm_blk_en    ;

    unsigned char  u8_uIndex   ;
    unsigned char  u8_vIndex   ;

    char  s8_uoffset_a  ;
    char  s8_uoffset_b  ;
    char  s8_uoffset_c  ;
    char  s8_uoffset_d  ;

    char  s8_voffset_a  ;
    char  s8_voffset_b  ;
    char  s8_voffset_c  ;
    char  s8_voffset_d  ;

} HAL_ACMBLKINFO_S;

typedef struct HAL_ACMINFO_S_
{
    HAL_ACMBLKINFO_S stAcmBlk0 ;
    HAL_ACMBLKINFO_S stAcmBlk1 ;
    HAL_ACMBLKINFO_S stAcmBlk2 ;
    HAL_ACMBLKINFO_S stAcmBlk3 ;

} HAL_ACMINFO_S;
typedef struct HAL_FIR_COEF_ADDR_S_
{
    unsigned int coef_addr_tap_l8c4_1;   //18*8+18*4
    unsigned int coef_addr_tap_l8c4_equal1;
    unsigned int coef_addr_tap_l8c4_075 ;
    unsigned int coef_addr_tap_l8c4_05  ;
    unsigned int coef_addr_tap_l8c4_033 ;
    unsigned int coef_addr_tap_l8c4_025 ;
    unsigned int coef_addr_tap_l8c4_0;

    unsigned int coef_addr_tap_l6c4_1;
    unsigned int coef_addr_tap_l6c4_equal1;
    unsigned int coef_addr_tap_l6c4_075;
    unsigned int coef_addr_tap_l6c4_05;
    unsigned int coef_addr_tap_l6c4_033;
    unsigned int coef_addr_tap_l6c4_025;
    unsigned int coef_addr_tap_l6c4_0;

    unsigned int coef_addr_tap_l4c4_1;
    unsigned int coef_addr_tap_l4c4_equal1 ;
    unsigned int coef_addr_tap_l4c4_075;
    unsigned int coef_addr_tap_l4c4_05;
    unsigned int coef_addr_tap_l4c4_033;
    unsigned int coef_addr_tap_l4c4_025;
    unsigned int coef_addr_tap_l4c4_0;

    unsigned int coef_addr_tap_l4c2_1;
    unsigned int coef_addr_tap_l4c2_equal1;
    unsigned int coef_addr_tap_l4c2_075;
    unsigned int coef_addr_tap_l4c2_05;
    unsigned int coef_addr_tap_l4c2_033;
    unsigned int coef_addr_tap_l4c2_025;
    unsigned int coef_addr_tap_l4c2_0;

    unsigned int coef_addr_vc2tap_1   ;
    unsigned int coef_addr_vc2tap_equal1 ;
    unsigned int coef_addr_vc2tap_075 ;
    unsigned int coef_addr_vc2tap_05  ;
    unsigned int coef_addr_vc2tap_033 ;
    unsigned int coef_addr_vc2tap_025 ;
    unsigned int coef_addr_vc2tap_0   ;
} HAL_FIR_COEF_ADDR_S;

//modified for zme coef config by l50053, begin
typedef enum VPP_ZOOM_TAP_E_
{
    VPP_ZOOM_TAP_8LH = 0,
    VPP_ZOOM_TAP_6LV ,
    VPP_ZOOM_TAP_4CH ,
    VPP_ZOOM_TAP_4CV ,
    VPP_ZOOM_TAP_2CV ,
    VPP_ZOOM_TAP_BUTT
} VPP_ZOOM_TAP_E;

typedef enum VPP_ALG_FLT_TAP_E_
{
    TAP8 = 0,  //8tap
    TAP6,      // 6tap
    TAP4,      //  4tap
    TAP2       //  2tap
}VPP_ALG_FLT_TAP_E;
//modified for zme coef config by l50053, end

//==============================================================================
/*矩形大小坐标信息*/
typedef struct HAL_RECT_S_
{
    int          s32SX;       /*source horizontal start position*/
    int          s32SY;       /*source vertical start position*/

    int          s32DXS;       /*dispaly horizontal start position*/
    int          s32DYS;       /*display vertical start position*/

    int          s32DXL;       /*dispaly horizontal end position*/
    int          s32DYL;       /*display vertical end position*/

    int          s32VX;       /*video horizontal start position*/
    int          s32VY;       /*video vertical start position*/

    unsigned int u32InWidth;   /*input width*/
    unsigned int u32InHeight;  /*input height*/
    unsigned int u32OutWidth;   /*output width*/
    unsigned int u32OutHeight;  /*output height*/
} HAL_RECT_S;

typedef struct RECT_S_
{
    int s32X;
    int s32Y;
    int s32Width;
    int s32Height;
} RECT_S;

/* vou background color */
typedef struct HAL_BKCOLOR_S_
{
    unsigned char u8Bkg_a;
    unsigned char u8Bkg_y;
    unsigned char u8Bkg_cb;
    unsigned char u8Bkg_cr;

} HAL_BKCOLOR_S;

typedef struct HAL_ACCTHD_S_
{
    unsigned int acc_multi    ;
    unsigned int thd_med_high ;
    unsigned int thd_med_low  ;
    unsigned int thd_high     ;
    unsigned int thd_low      ;
} HAL_ACCTHD_S;

typedef struct HAL_CSCCOEF_S_
{
    int csc_coef00;
    int csc_coef01;
    int csc_coef02;

    int csc_coef10;
    int csc_coef11;
    int csc_coef12;

    int csc_coef20;
    int csc_coef21;
    int csc_coef22;

    int csc_in_dc0;
    int csc_in_dc1;
    int csc_in_dc2;

    int csc_out_dc0;
    int csc_out_dc1;
    int csc_out_dc2;
} HAL_CSCCOEF_S;

// SDK use it, not the above one, we need to unit in next editor.
typedef struct HAL_INTFCSCDCCOEF_S_
{
    int csc_in_dc0;
    int csc_in_dc1;
    int csc_in_dc2;

    int csc_out_dc0;
    int csc_out_dc1;
    int csc_out_dc2;
} HAL_INTFCSCDCCOEF_S;

typedef struct HAL_INTFCSCCOEF_S_
{
    int csc_coef00;
    int csc_coef01;
    int csc_coef02;

    int csc_coef10;
    int csc_coef11;
    int csc_coef12;

    int csc_coef20;
    int csc_coef21;
    int csc_coef22;
} HAL_INTFCSCCOEF_S;



/* vpp die dir mult mode */
typedef enum VPP_DIE_DIRMULT_E_
{
    VPP_DIE_DIRMULT_00 = 0,
    VPP_DIE_DIRMULT_01    ,
    VPP_DIE_DIRMULT_02    ,
    VPP_DIE_DIRMULT_03    ,
    VPP_DIE_DIRMULT_04    ,
    VPP_DIE_DIRMULT_05    ,
    VPP_DIE_DIRMULT_06    ,
    VPP_DIE_DIRMULT_07    ,
    VPP_DIE_DIRMULT_08    ,
    VPP_DIE_DIRMULT_09    ,
    VPP_DIE_DIRMULT_10    ,
    VPP_DIE_DIRMULT_11    ,
    VPP_DIE_DIRMULT_12    ,
    VPP_DIE_DIRMULT_13    ,
    VPP_DIE_DIRMULT_14    ,
    VPP_DIE_DIRMULT_00_03 ,
    VPP_DIE_DIRMULT_04_07 ,
    VPP_DIE_DIRMULT_08_11 ,
    VPP_DIE_DIRMULT_12_14 ,
    VPP_DIE_DIRMULT_ALL   ,

    VPP_DIE_DIRMULT_BUTT

}VPP_DIE_DIRMULT_E;



typedef struct HAL_ZOOM_BIT_S_
{
    int bits_0:
        10 ;
    int bits_1:
        10 ;
    int bits_2:
        10 ;
    int bits_32:
        2;
    int bits_38:
        8 ;
    int bits_4 :
        10 ;
    int bits_5:
        10 ;
    int bits_64 :
        4 ;
    int bits_66 :
        6 ;
    int bits_7 :
        10 ;
    int bits_8 :
        10 ;
    int bits_96 :
        6 ;
    int bits_94 :
        4 ;
    int bits_10 :
        10 ;
    int bits_11 :
        10 ;
    int bits_12 :
        8 ;
}HAL_ZOOM_BIT_S;

typedef struct HAL_ZOOM_BITARRAY_S_
{
    unsigned int        u32Size;
    HAL_ZOOM_BIT_S      stBit[12];
}HAL_ZOOM_BITARRAY_S;

typedef struct  VPP_VPP_S_
{
    bool               bopened;      //使能标志:TRUE, 可以创建win, 注册ISR；FALSE，销毁所有

    bool               boffline_mode_en;      //if in mosaic mode
    RECT_S             st_out_rect;
    RECT_S             st_in_rect;

    HAL_LAYER_E        enLayer;
    bool               bzoom_enable;
    bool               bcsc_enable;

    unsigned char      axi_ostd_r0;
    unsigned char      axi_ostd_r1;
    unsigned char      axi_ostd_wr;
} VPP_VPP_S;

//VPP ZME系数
typedef struct VPP_ZOOM_COEF_ADDR_S_
{
    unsigned int       StartPhyAddr; //modified for zme coef config by l50053
    unsigned int       *pStartVirAddr;
    unsigned int       CoefSize;     //add for zme coef config by l50053
    HAL_FIR_COEF_ADDR_S stCoefAddr;
}VPP_ZOOM_COEF_ADDR_S;

//S00198515 add for DIE 20120823 [start]
typedef struct VPP_DIE_ADDR_S_
{
    unsigned int       *die_BusAddr;
    unsigned int       *die_VirtAddr;
    unsigned int       die_size;
}VPP_DIE_ADDR_S;
//S00198515 add for DIE 20120823 [end]

//add for mmu table
typedef struct VPP_MMU_ADDR_S_
{
    unsigned long        mmu_BusAddr;
    unsigned long        mmu_ch_ptva;
    unsigned long        mmu_ch_ptba;//页表基地址
    //unsigned int        mmu_size;
    //unsigned int        mmu_page_size;
    //unsigned int        mmu_page_num;
}VPP_MMU_ADDR_S;

typedef enum VPP_MMU_ENB_E_
{
    MMU_INDIS_OUTDIS = 0x0,
    MMU_INDIS_OUTENB = 0x400,
    MMU_INENB_OUTDIS_5FIELD = 0x3ff,
    MMU_PACKAGE_ENB         = 0x401,
    MMU_ENB_DIE_DIS         = 0x403,
    MMU_INENB_OUTENB_5FIELD = 0x7ff,
    MMU_INENB_OUTDIS_3FIELD = 0x2c7,
    MMU_INENB_OUTENB_3FIELD = 0x6c7,

}VPP_MMU_ENB_E;


typedef struct VPP_ZOOM_COEF_S_
{
    bool               bzoom_enable;    //缩放使能标记，0禁止，1使能。
    bool               bmid_enable;    //中值滤波使能
    bool               bzme_fir_enable;  //滤波使能，当为true时，中值滤波设置才有效果
    bool               bhfir_order_first;     //水平缩放顺序，0，水平缩放在前，1水平缩放在后
    HAL_LAYER_E        enLayer;     //配置的显示层
    HAL_ZMEMODE_E      enMode;       //缩放模式
    unsigned int       bus_address;
}VPP_ZOOM_COEF_S;

typedef enum VPP_ZOOM_COEF_E_
{
    VPP_ZOOM_COEF_1  = 0,
    VPP_ZOOM_COEF_EQU_1,
    VPP_ZOOM_COEF_075,
    VPP_ZOOM_COEF_05,
    VPP_ZOOM_COEF_033,
    VPP_ZOOM_COEF_025,
    VPP_ZOOM_COEF_0,
    VPP_ZOOM_COEF_BUTT
} VPP_ZOOM_COEF_E;

//add for zme coef config by l50053, begin
typedef struct VPP_SCALER_INFO_
{
    bool lum_vfir_en   ; //vertical scaler of lum enable
    bool lum_hfir_en   ; //horizontal scaler of lum enable
    bool chm_vfir_en   ; //vertical scaler of chroma enable
    bool chm_hfir_en   ; //horizontal scaler of chroma enable
    bool lum_vfir_mode ; //vertical scaler mode of lum: 0-copy mode; 1-filter mode
    bool lum_hfir_mode ; //horizontal scaler mode of lum: 0-copy mode; 1-filter mode
    bool chm_vfir_mode ; //vertical scaler mode of chroma: 0-copy mode; 1-filter mode
    bool chm_hfir_mode ; //horizontal scaler  modeof chroma: 0-copy mode; 1-filter mode
    bool lum_vmed_en   ; //median filter enable for lum vertical scaler
    bool lum_hmed_en   ; //median filter enable for lum horizontal scaler
    bool chm_vmed_en   ; //median filter enable for chroma vertical scaler
    bool chm_hmed_en   ; //median filter enable for chroma horizontal scaler
    bool hfir_order    ; //0-horizontal filter before vertical filter; 1-horizontal filter after vertical filter

    int  lum_vfir_tap  ; //filter tap of lum vertical scaler
    int  chm_vfir_tap  ; //filter tap of chroma vertical scaler
    int  lum_hfir_ratio; //horizontal ratio of lum scaler
    int  lum_vfir_ratio; //vertical ratio of lum scaler
    int  lum_hor_offset; //horizontal offset for lum
    int  chm_hor_offset; //horizontal offset for chroma
    int  lum_ver_offset; //vertical offset for lum
    int  chm_ver_offset; //vertical offset for chroma

    unsigned int coef_addr_h;
    unsigned int coef_addr_v;
} VPP_SCALER_INFO;
//add for zme coef config by l50053, end



#endif

