/*
 **************************************************************************************
 *
 *       Filename:   vpp_drv_ext.h
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011-07-8 16:20:00
 *         Author:  j00140427
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */
#ifndef _VPP_DRV_EXT_H_
#define _VPP_DRV_EXT_H_

#define K3_SUCCESS            0
#define K3_FAILURE          (-1)

#define VPP_IRECT_MIN_W 96
#define VPP_IRECT_MIN_H 64
#define VPP_DIE_IRECT_MIN_W 128
#define VPP_IRECT_MAX_W 1920
#define VPP_IRECT_MAX_H 1088

#define VPP_ORECT_MIN_W 64
#define VPP_ORECT_MIN_H 64
#define VPP_ORECT_MAX_W 1920
#define VPP_ORECT_MAX_H 1088

typedef enum
{
    k3_false = 0,
    k3_true  = 1,
}k3_bool;

/*视频数据读取模式*/
typedef enum VPP_VHD_FOD_E_
{
    VPP_T_FIRST = 0,
    VPP_B_FIRST = 1,

    VPP_VHD_FOD_BUTT
}VPP_VHD_FOD_E;

typedef enum VPP_MEM_MODE_E_
{
    VPP_LINEAR = 0,
    VPP_TILE = 1,


    VPP_MEM_MODE_BUFF
}VPP_MEM_MODE_E;

/*VPP   DIE MODE */
 typedef enum VPP_DIE_MODE_E_
{
    VPP_DIE_MODE_5FIELD = 0,
    VPP_DIE_MODE_4FIELD,
    VPP_DIE_MODE_3FIELD,

    VPP_DIE_MODE_BUTT
}VPP_DIE_MODE_E;

/*图像输入格式*/
typedef enum VPP_PIXELFORMAT_E_
{
    VPP_INPUTFMT_YCBCR_SEMIPLANAR_400     = 0x1,
    VPP_INPUTFMT_YCBCR_SEMIPLANAR_420     = 0x3,
    VPP_INPUTFMT_YCBCR_SEMIPLANAR_422     = 0x4,
    VPP_INPUTFMT_YCBCR_SEMIPLANAR_444     = 0x5,

    VPP_INPUTFMT_CBYCRY_PACKAGE_422       = 0x9,
    VPP_INPUTFMT_YCBYCR_PACKAGE_422       = 0xA,
    VPP_INPUTFMT_YCRYCB_PACKAGE_422       = 0xB,
    VPP_INPUTFMT_CRYCBY_PACKAGE_422       = 0xC,

    VPP_PIXELFORMAT_BUTT
}VPP_PIXELFORMAT_E;

/*图像输出格式*/
typedef enum VPP_INTFDATAFMT_E_
{
    VPP_INTFDATAFMT_CBYCRY_PACKAGE_422   = 0x0,
    VPP_INTFDATAFMT_YCBYCR_PACKAGE_422   = 0x1,
    VPP_INTFDATAFMT_YCRYCB_PACKAGE_422   = 0x2,
    VPP_INTFDATAFMT_CRYCBY_PACKAGE_422   = 0x3,

    VPP_INTFDATAFMT_BUTT
} VPP_INTFDATAFMT_E;

typedef struct VPP_INIMAGE_S_
{
    VPP_PIXELFORMAT_E  pixformat;
    VPP_MEM_MODE_E     in_mem_mode;
    unsigned int       width;//输入图像宽度，以像素为单位
    unsigned int       height;//输入图像高度，以像素为单位
    unsigned int       buffer_bus_addr;//输入图像帧Buffer起始地址，物理地址,四字节对齐
    unsigned int       buffer_vir_addr;//输入图像帧Buffer起始地址，虚拟地址,四字节对齐
    unsigned int       stride;   //输入图像的stride

    unsigned int       buffersize;
    unsigned int       mmu_ch_ptba;
    unsigned int       mmu_ch_ptva;
}VPP_INIMAGE_S;

typedef struct VPP_OUTIMAGE_S_
{
    VPP_INTFDATAFMT_E  pixformat;
    VPP_MEM_MODE_E     out_mem_mode;
    unsigned int       width;   //输出图像宽度，以像素为单位
    unsigned int       height;  //输出图像高度，以像素为单位
    unsigned int       buffer_bus_addr;//输出图像帧Buffer起始地址，物理地址,四字节对齐
    unsigned int       buffer_vir_addr;//输入图像帧Buffer起始地址，虚拟地址,四字节对齐
    unsigned int       stride;

    unsigned int       buffersize;
    unsigned int       mmu_ch_ptba;
    unsigned int       mmu_ch_ptva;
}VPP_OUTIMAGE_S;

/* vpp die st mode */
typedef enum VPP_DIE_STMD_E_
{
    VPP_DIE_STMD_Q   = 0,
    VPP_DIE_STMD_N,
    VPP_DIE_STMD_M,
    VPP_DIE_STMD_L,
    VPP_DIE_STMD_K,
    VPP_DIE_STMD_KLM,
    VPP_DIE_STMD_KLQR,
    VPP_DIE_STMD_KLQRM,
    VPP_DIE_STMD_STKLM,
    VPP_DIE_STMD_KLMN,
    VPP_DIE_STMD_DISALL,

    VPP_DIE_STMD_BUTT

}VPP_DIE_STMD_E;

typedef struct VPP_CROPINFO_S_
{
    unsigned int cropX;
    unsigned int cropY;
    unsigned int height;
    unsigned int width;
}VPP_INCROPPING_S;

typedef struct VPP_DIE_ADDRINFO_S_
{
    unsigned int   lluma_addr;
    unsigned int   lchroma_addr;
    unsigned int   cluma_addr;
    unsigned int   cchroma_addr;
    unsigned int   nluma_addr;
    unsigned int   nchroma_addr;
}VPP_DIE_ADDRINFO_S;

typedef struct VPP_FRAME_ADDRINFO_S_
{
    unsigned int   vhd_clum_addr;
    unsigned int   vhd_cchm_addr;

    unsigned int   vhd_llum_addr;
    unsigned int   vhd_lchm_addr;

    unsigned short vhd_lum_str;
    unsigned short vhd_chm_str;
}VPP_FRAME_ADDRINFO_S;

typedef struct
{
    unsigned int            in_mmu_ch_ptba[3];
    unsigned int            in_mmu_ch_ptva[3];
    unsigned int            in_size;

    unsigned int            out_mmu_ch_ptba;
    unsigned int            out_mmu_ch_ptva;
    unsigned int            out_size;
}VPP_MMU_PTABLE_INFO;

typedef struct VPP_CONFIG_S_
{
    k3_bool crop_enable;
    k3_bool bdie_enbale;
    k3_bool mmu_enable;
    k3_bool pd_enable;//pulldown test
    unsigned int  acm_en;

    VPP_MMU_PTABLE_INFO sPTableInfo;
    VPP_INIMAGE_S           vpp_in_img;
    VPP_OUTIMAGE_S          vpp_out_img;
    VPP_INCROPPING_S          vpp_in_crop;
    VPP_VHD_FOD_E           edie_fod;//场序 TB OR BT
    VPP_DIE_MODE_E          edie_mode;//3 //3 field or 5field
    VPP_DIE_ADDRINFO_S      vpp_die_addrinfo;
}VPP_CONFIG_S;

#endif
