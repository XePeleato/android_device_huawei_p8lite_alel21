/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#ifndef __BALONG_ADE_H__
#define __BALONG_ADE_H__

#include <linux/types.h>
#include <linux/fb.h>
#include <linux/hisi_fb_common.h>
#include "balong_fb_def.h"

/* ade drv memory share switch */
#define ADE_MEM_SHARE_EN            (1)

/* partial update support */
#define PARTIAL_UPDATE    (1)

/*ade set core rate command mask*/
#define SET_CORE_RATE_IMMEDIATELY (0x1)


#define ADE_IOCTL_MAGIC HISIFB_IOCTL_MAGIC
#define FB_CURSOR  _IOW(ADE_IOCTL_MAGIC, 130, struct fb_cursor)

#define FB_VSYNC_INT_SET  _IOW(ADE_IOCTL_MAGIC, 132, unsigned int)
#define FB_FRC_SET		  _IOW(ADE_IOCTL_MAGIC, 133, char *)

/* ADE RESOURCE INFO SET for top register */
#define ADE_OVERLAY_RES_SET     _IOWR(ADE_IOCTL_MAGIC, 135, struct overlay_resource_info)

/* ADE REGION INFO SET for ch register */
#define ADE_OVERLAY_REGION_SET  _IOWR(ADE_IOCTL_MAGIC, 136, struct overlay_region_info)
#define ADE_OVERLAY_COMMIT      _IOW(ADE_IOCTL_MAGIC, 137, struct overlay_compose_info)

/* self refresh cmd */
#define ADE_OVERLAY_SELF_REFS       _IOW(ADE_IOCTL_MAGIC, 140, unsigned int)
#define ADE_OVERLAY_LAYER_SET       _IOW(ADE_IOCTL_MAGIC, 141, struct overlay_layer_info)

#if ADE_MEM_SHARE_EN
/* test for mem share to codec */
#define ADE_MEM_SHARE_CODEC_PROC    _IOW(ADE_IOCTL_MAGIC, 160, struct mem_share_test)
#endif

#if PARTIAL_UPDATE
#define FB_SET_UPDATE_RECT _IOW(ADE_IOCTL_MAGIC, 161, struct ade_rect) /* partial update rect */
#endif

#define FB_SET_VIRT_DISP    _IOW(ADE_IOCTL_MAGIC, 162, int)

#define ADE_CLOCK_RATE_SET  _IOW(ADE_IOCTL_MAGIC, 163, struct clock_rate_info) /* ade core clock set */

#define ADE_COMPOSE_CH_MAX      (OVERLAY_PIPE_ADE_MAX - 1)

#if LCD_CHECK_MIPI_SUPPORT
#define FB_CHECK_MIPI_TR    _IOR(ADE_IOCTL_MAGIC, 701, int)
#define FB_SET_INVERSION    _IOW(ADE_IOCTL_MAGIC, 702, void *)
#endif
enum {
    ADE_DISABLE = 0,
    ADE_ENABLE
};

enum {
    ADE_REFRESH_PARTIAL = 0,  /* config wdma2 base on the new cmdfile */
    ADE_REFRESH_SWITCH = 1    /* config wdma2 base on the old cmdfile */
};

enum ADE_FORMAT {
    ADE_RGB_565 = 0,
    ADE_BGR_565,
    ADE_XRGB_8888,
    ADE_XBGR_8888,
    ADE_ARGB_8888,
    ADE_ABGR_8888,
    ADE_RGBA_8888,
    ADE_BGRA_8888,
    ADE_RGB_888,
    ADE_BGR_888 = 9,
    ADE_YUYV_I = 16,
    ADE_YVYU_I,
    ADE_UYVY_I,
    ADE_VYUY_I,
    ADE_YUV444,
    ADE_NV12,
    ADE_NV21,
    ADE_FORMAT_NOT_SUPPORT = 800
};
typedef unsigned int    ADE_FORMAT_UINT32;


enum ADE_SCL_FORMAT {
    ADE_SCL_ARGB888 = 0,
    ADE_SCL_YUV444,
    ADE_SCL_YUV422,
    ADE_SCL_YUV420
};

typedef unsigned int    ADE_SCL_FORMAT_UINT32;

enum ADE_ROT{
    ADE_ROT_NOP = 0,
    ADE_ROT_90,
    ADE_ROT_180,
    ADE_ROT_270,
    ADE_ROT_H_MIRROR,
    ADE_ROT_V_MIRROR,
    ADE_ROT_90_H_MIRROR,
    ADE_ROT_90_V_MIRROR,
    ADE_ROT_INVALID
};
typedef unsigned int    ADE_ROT_UINT32;

enum {
    ADE_RGB = 0,
    ADE_BGR
};

enum {
    ADE_BURST8 = 0,
    ADE_BURST16
};

enum {
    ADE_ALP_GLOBAL = 0,
    ADE_ALP_PIXEL,
    ADE_ALP_PIXEL_AND_GLB
};

enum {
    ADE_ALP_MUL_COEFF_0 = 0,    /* alpha */
    ADE_ALP_MUL_COEFF_1,        /* 1-alpha */
    ADE_ALP_MUL_COEFF_2,        /* 0 */
    ADE_ALP_MUL_COEFF_3         /* 1 */
};

enum {
    ADE_FRM_FMT_2D = 0,
    ADE_FRM_FMT_3D_SBS,
    ADE_FRM_FMT_3D_TTB,
    ADE_FRM_FMT_3D_CBC,
    ADE_FRM_FMT_3D_LBL,
    ADE_FRM_FMT_3D_QUINCUNX,
    ADE_FRM_FMT_3D_FBF
};

enum ade_yuv_info {
    ADE_YUV_NO_INFO = 0,
    ADE_YUV_601_NARROW,
    ADE_YUV_601_WIDE,
    ADE_YUV_709_NARROW,
    ADE_YUV_709_WIDE
};

enum OVERLAY_PIPE_NUM{
    OVERLAY_PIPE_ADE_CH1 = 0,
    OVERLAY_PIPE_ADE_CH2,
    OVERLAY_PIPE_ADE_CH3,
    OVERLAY_PIPE_ADE_CH4,
    OVERLAY_PIPE_ADE_CH5,
    OVERLAY_PIPE_ADE_CH6,
    OVERLAY_PIPE_ADE_DISP,

    OVERLAY_PIPE_ADE_MAX
};
typedef unsigned int    OVERLAY_PIPE_NUM_UINT32;

enum OVERLAY_SURF_NUM {
    OVERLAY_ADE_SURFACE_1 = 0,
    OVERLAY_ADE_SURFACE_2,
    OVERLAY_ADE_SURFACE_3,
    OVERLAY_ADE_SURFACE_4,
    OVERLAY_ADE_SURFACE_5,
    OVERLAY_ADE_SURFACE_6,
    OVERLAY_ADE_SURFACE_7,
    OVERLAY_ADE_SURFACE_8,
    OVERLAY_ADE_SURFACE_MAX
};
typedef unsigned int    OVERLAY_SURF_NUM_UINT32;

enum OVERLAY_OVLY_NUM {
    OVERLAY_OVLY_1 = 0,
    OVERLAY_OVLY_2,
    OVERLAY_OVLY_3,

    OVERLAY_OVLY_MAX
};
typedef unsigned int    OVERLAY_OVLY_NUM_UINT32;

enum OVERLAY_COMP_TYPE {
    OVERLAY_COMP_TYPE_ONLINE = 0,
    OVERLAY_COMP_TYPE_OFFLINE,
    OVERLAY_COMP_TYPE_HYBRID,
    OVERLAY_COMP_TYPE_COPYBIT,
    OVERLAY_COMP_TYPE_INVAL
};
typedef unsigned int   OVERLAY_COMP_TYPE_UINT32;

enum OVERLAY_PIPE_TYPE {
    OVERLAY_PIPE_TYPE_ONLINE = 0,
    OVERLAY_PIPE_TYPE_OFFLINE,
    OVERLAY_PIPE_TYPE_COPYBIT,
    OVERLAY_PIPE_TYPE_DISPLAY,
    OVERLAY_PIPE_TYPE_INVAL
};
typedef unsigned int  OVERLAY_PIPE_TYPE_UINT32;

enum OVERLAY_BLEND_TYPE {
    OVERLAY_BLENDING_NONE = 0,
    OVERLAY_BLENDING_PREMULT  = 0x0105,
    OVERLAY_BLENDING_COVERAGE = 0x0405,
    OVERLAY_BLENDING_DIM      = 0x0805,
    OVERLAY_BLENDING_SEC_OVLY = 0x1005,
    OVERLAY_BLENDING_PIX_GLB  = 0x2005
};
typedef unsigned int OVERLAY_BLEND_TYPE_UINT32;

enum OVERLAY_LAYER_SET_TYPE {
    OVERLAY_LAYER_SET_BLK_ROT = 0,
    OVERLAY_LAYER_SET_CTRAN,
    OVERLAY_LAYER_SET_INVALID
};
typedef unsigned int OVERLAY_LAYER_SET_TYPE_UINT32;

/******************************************************************************/

struct ade_rect {
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
};

/* compose CH resource */
typedef union {
    struct {
        unsigned int    ch_num       : 3;
        unsigned int    ch_en        : 1;
        unsigned int    has_block_rot  : 1;
        unsigned int    has_720p_rot   : 1;
        unsigned int    has_1080p_rot  : 1;
        unsigned int    has_scl1     : 1;
        unsigned int    has_scl3     : 1;
        unsigned int    has_ovly1    : 1;
        unsigned int    has_ovly2    : 1;
        unsigned int    has_ovly3    : 1;
        unsigned int    reserved     : 20;
    }bits;

    unsigned int ul32;
}CH_RES;

/* display CH resource */
typedef union {
    struct {
        unsigned int    ch_en     : 1;
        unsigned int    has_ldi   : 1;
        unsigned int    has_720p_rot    : 1;
        unsigned int    has_1080p_rot   : 1;
        unsigned int    has_scl2  : 1;
        unsigned int    has_scl3  : 1;
        unsigned int    has_ovly2 : 1;
        unsigned int    has_ovly3 : 1;
        unsigned int    has_wdma2 : 1;
        unsigned int    has_wdma3 : 1;
        unsigned int    reserved  : 22;
    }bits;

    unsigned int ul32;
}DISP_CH_RES;

typedef union {
    struct {
        unsigned int    ovly_num    : 2;
        unsigned int    ovly_en     : 1;
        unsigned int    has_wdma1   : 1;
        unsigned int    has_scl3    : 1;
        unsigned int    has_wdma3   : 1;
        unsigned int    reserved    : 26;
    }bits;

    unsigned int ul32;
}OVLY1_RES;

/* ovly2 resource */
typedef union {
    struct {
        unsigned int    ovly_num  : 2;
        unsigned int    ovly_en   : 1;
        unsigned int    has_wdma2 : 1;
        unsigned int    has_wdma3 : 1;
        unsigned int    has_scl3  : 1;
        unsigned int    has_720p_rot    : 1;
        unsigned int    has_1080p_rot   : 1;
        unsigned int    has_scl2  : 1;
        unsigned int    has_ldi   : 1;
        unsigned int    has_offline : 1;  /* 0:online, 1:offline */
        unsigned int    reserved    : 21;
    }bits;

    unsigned int ul32;
}OVLY2_RES;

/* ovly3 resource */
typedef union {
    struct {
        unsigned int    ovly_num  : 2;
        unsigned int    ovly_en   : 1;
        unsigned int    has_wdma2 : 1;
        unsigned int    has_720p_rot    : 1;
        unsigned int    has_1080p_rot   : 1;
        unsigned int    has_scl2    : 1;
        unsigned int    has_ovly1   : 1;
        unsigned int    has_ovly2   : 1;
        unsigned int    has_offline : 1;  /* 0:online, 1:offline */
        unsigned int    reserved    : 22;
    }bits;
    unsigned int ul32;
}OVLY3_RES;

typedef union {
    struct {
        unsigned int    rot_en    : 1;
        unsigned int    has_wdma2 : 1;
        unsigned int    has_wdma3 : 1;
        unsigned int    has_scl3  : 1;
        unsigned int    has_scl2    : 1;
        unsigned int    has_ovly1   : 1;
        unsigned int    has_ovly2   : 1;
        unsigned int    has_ldi     : 1;
        unsigned int    has_offline : 1;   /* 0:online, 1:offline */
        unsigned int    reserved    : 23;
    }bits;
    unsigned int ul32;
}ROT_RES;

typedef union {
    struct {
        unsigned int    scl2_en     : 1;
        unsigned int    has_wdma2   : 1;
        unsigned int    has_wdma3   : 1;
        unsigned int    has_scl3    : 1;
        unsigned int    has_ovly1   : 1;
        unsigned int    has_ovly2   : 1;
        unsigned int    has_ldi     : 1;
        unsigned int    has_offline : 1;  /* 0:online, 1:offline */
        unsigned int    reserved    : 24;
    }bits;
    unsigned int ul32;

}SCL2_RES;

/* ADE_MEM_SHARE_EN */
typedef union {
    struct {
        unsigned int    memshare_en : 2;
        unsigned int    reserved    : 30;
    }bits;
    unsigned int ul32;

}ADE_REG_INFO;

/* ADE resource info for top register */
struct overlay_resource_info {
    OVERLAY_COMP_TYPE_UINT32     comp_mode;
    CH_RES          compose_ch_res[ADE_COMPOSE_CH_MAX];
    DISP_CH_RES     disp_ch_res;
    OVLY1_RES       ovly1_res;
    OVLY2_RES       ovly2_res;
    OVLY3_RES       ovly3_res;
    ROT_RES         rot_res;
    SCL2_RES        scl2_res;
    ADE_REG_INFO    ade_reg_res;    /* ADE_MEM_SHARE_EN */
};

struct overlay_region_info {
    OVERLAY_PIPE_NUM_UINT32         ch_num;
    OVERLAY_PIPE_TYPE_UINT32        ch_type;
    OVERLAY_SURF_NUM_UINT32         surface_num;
    OVERLAY_COMP_TYPE_UINT32        compose_mode;
    OVERLAY_BLEND_TYPE_UINT32       blending;       /* alpha | blend_type */
    unsigned int                    dim_blending;   /* 0 is no dim, not 0 is dim alpha */
    OVERLAY_SURF_NUM_UINT32         sec_ovly_surf_num;   /* the surf num of sec_ovly  */
    unsigned int                    sec_ovly_ch_count;   /* how much channel send to sec ovly */

    struct  ade_rect    src_rect;
    struct  ade_rect    dst_rect;

    /* ovly rect */
    struct  ade_rect    ovly_output_rect;           /* for sec_ovly input size */
    struct  ade_rect    sec_ovly_output_rect;          /* when sec_ovly_ch_count > 1, for sec ovly output size */
    unsigned int        region_is_finished;
    unsigned char       clip_left;
    unsigned char       clip_top;
    unsigned char       clip_right;
    unsigned char       clip_bottom;
    ADE_ROT_UINT32      rotation;

    unsigned int        phy_addr;
    unsigned int        stride;
    unsigned int        width;
    unsigned int        dst_width;
    unsigned int        height;
    unsigned int        dst_height;
    ADE_FORMAT_UINT32   format;

    unsigned int        uv_addr;
    int                 share_fd;
    unsigned int        yuv_info;
    unsigned int        reserved;
};

struct overlay_ch_info {
    OVERLAY_PIPE_NUM_UINT32     ch_num;
    OVERLAY_PIPE_TYPE_UINT32    ch_type;
    OVERLAY_SURF_NUM_UINT32     surface_num;
};

struct overlay_compose_info {
    unsigned int    is_finished;            /* the last commit is true, other is false */
    unsigned int    compose_pattern_num;
    OVERLAY_COMP_TYPE_UINT32    compose_mode;

    /* wdma1 */
    unsigned int    offline_phy_addr;
    unsigned int    offline_frame_phy_addr;
    unsigned int    offline_dst_format;
    unsigned int    offline_stride;
    unsigned int    offline_frame_share_fd;
    struct ade_rect offline_dst_rect;

    /* scl3 & wdma3 */
    unsigned int    wifi_phy_addr;
    unsigned int    wifi_dst_format;
    unsigned int    wifi_stride;
    unsigned int    reserved2;
    struct ade_rect wifi_src_rect;
    struct ade_rect wifi_dst_rect;
    ADE_ROT_UINT32  wifi_rotation;

    unsigned char   dst_rotation;    /* after compose rotation */
    unsigned char   reserved3[3];
    unsigned int    rot_width;
    unsigned int    rot_height;

    struct ade_rect       src_rect;     /* after compose */
    struct ade_rect       dst_rect;     /* after compose */

    /* scl for low resolution */
    struct ade_rect       scl2_src_rect;     /* the source rect before scl */
    struct ade_rect       scl2_dst_rect;     /* the source rect after scl */

    int              release_fence;
    unsigned int     frame_number;
};

struct overlay_layer_info {
    OVERLAY_LAYER_SET_TYPE_UINT32  layer_set_type;

    /* for block rot */
    OVERLAY_PIPE_NUM_UINT32   ch_num;
    OVERLAY_SURF_NUM_UINT32   surface_num;
    ADE_ROT_UINT32            rotation;

    /* for scl */
    struct ade_rect       src_rect;
    struct ade_rect       dst_rect;

    /* for ctran */
    ADE_FORMAT_UINT32         dst_format;

    unsigned int              phy_addr;
    unsigned int              dst_phy_addr;
    unsigned int              stride;
    unsigned int              dst_stride;
    unsigned int              width;
    unsigned int              height;
    ADE_FORMAT_UINT32         format;
    unsigned int              wdma_num;
    unsigned int              uv_addr;
};
#if LCD_CHECK_MIPI_SUPPORT
enum INVERSION_TYPE {
    INVERSION_TYPE_1DOT = 0,
    INVERSION_TYPE_2DOT,
    INVERSION_TYPE_COLUMN,
    INVERSION_TYPE_ROW,
    INVERSION_TYPE_FRAME,
    INVERSION_TYPE_NUM,
};
#endif
#if ADE_MEM_SHARE_EN
struct mem_share_test {
    int     event_flag;
    int     mem_size;
};
#endif

struct clock_rate_info {
    int  clock_rate;
    int  command;
};

#endif /*_BALONG_ADE_H_*/



