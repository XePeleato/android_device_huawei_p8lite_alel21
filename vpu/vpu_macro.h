/***************************************************************************************************
    File:
        vpu_macro.h
    Description:
        Macro definition in VPU module
    Author:
        Jian Huan
    Date:
        2010-11-23 21:45:57
 **************************************************************************************************/
#ifndef _VPU_MACRO_
#define _VPU_MACRO_

#include "vpu_type.h"

#define     VPU_OK                                  0
#define     VPU_ERR                                 1

#define     VDPU_BASE                               0x10104200

#define     VPU_MAX_FRAME_WIDTH                     1920
#define     VPU_MAX_FRAME_HEIGHT                    1088

#define     VPU_FRAME_NULL                          NULL

#define     DEC_X170_REGISTERS                      60

#define     DEC_X170_LITTLE_ENDIAN                  1
#define     DEC_X170_BIG_ENDIAN                     0

#define     DEC_X170_BUS_BURST_LENGTH_UNDEFINED     0
#define     DEC_X170_BUS_BURST_LENGTH_4             4
#define     DEC_X170_BUS_BURST_LENGTH_8             8
#define     DEC_X170_BUS_BURST_LENGTH_16            16

#define  VPU_CODEC_DEC_H264      0
#define  VPU_CODEC_DEC_MPEG4      1
#define  VPU_CODEC_DEC_H263      2
#define  VPU_CODEC_DEC_JPEG      3
#define  VPU_CODEC_DEC_VC1      4
#define  VPU_CODEC_DEC_MPEG2      5
#define  VPU_CODEC_DEC_MPEG1      6
#define  VPU_CODEC_DEC_VP6      7
#define  VPU_CODEC_DEC_RV      8
#define  VPU_CODEC_DEC_VP7      9
#define  VPU_CODEC_DEC_VP8      10
#define  VPU_CODEC_DEC_AVS      11
#define  VPU_CODEC_DEC_SVC      12
#define  VPU_CODEC_DEC_VC2      13
#define  VPU_CODEC_DEC_MVC      14
#define  VPU_CODEC_DEC_THEORA     15
#define  VPU_CODEC_ENC_JPEG      16
#define  VPU_CODEC_ENC_H264      17
#define  VPU_CODEC_DEC_MJPEG     18
/*@jh: ??????*/
#define     VPU_CODEC_DEC_DIVX3                     18
#define     VPU_CODEC_DEC_SORESONSPARKLE            19

#define     VPU_OUTPUT_FRAME_TYPE      0
#define     VPU_OUTPUT_TOP_FIELD_FIRST_TYPE    1
#define     VPU_OUTPUT_BOT_FIELD_FIRST_TYPE    2

#define  VPU_OUTPUT_FORMAT_ARGB8888     0
#define  VPU_OUTPUT_FORMAT_ABGR8888     1
#define  VPU_OUTPUT_FORMAT_RGB888     2
#define  VPU_OUTPUT_FORMAT_RGB565     3
#define  VPU_OUTPUT_FORMAT_RGB555     4
#define  VPU_OUTPUT_FORMAT_YUV420_SEMIPLANAR      5
#define  VPU_OUTPUT_FORMAT_YUV420_PLANAR    6
#define  VPU_OUTPUT_FORMAT_YUV422     7
#define  VPU_OUTPUT_FORMAT_YUV444     8
#define  VPU_OUTPUT_FORMAT_YCH420     9

#define  VPU_PP_INPUT_FORMAT_YUV420_SEMIPLANAR  0
#define  VPU_PP_INPUT_FORMAT_YUV420_PLANAR   1
#define  VPU_PP_INPUT_FORMAT_YUV420_INTERLACE  2
#define  VPU_PP_INPUT_FORMAT_YUV422     3

#define  VPU_PP_OUTPUT_FORMAT_ARGB8888    VPU_OUTPUT_FORMAT_ARGB8888
#define  VPU_PP_OUTPUT_FORMAT_ABGR8888    VPU_OUTPUT_FORMAT_ABGR8888
#define  VPU_PP_OUTPUT_FORMAT_RGB888     VPU_OUTPUT_FORMAT_RGB888
#define  VPU_PP_OUTPUT_FORMAT_RGB565     VPU_OUTPUT_FORMAT_RGB565
#define  VPU_PP_OUTPUT_FORMAT_RGB555     VPU_OUTPUT_FORMAT_RGB555
#define  VPU_PP_OUTPUT_FORMAT_YUV420_SEMIPLANAR     VPU_OUTPUT_FORMAT_YUV420_SEMIPLANAR
#define  VPU_PP_OUTPUT_FORMAT_YUV420_PLANAR   VPU_OUTPUT_FORMAT_YUV420_PLANAR
#define  VPU_PP_OUTPUT_FORMAT_YUV422     VPU_OUTPUT_FORMAT_YUV422
#define  VPU_PP_OUTPUT_FORMAT_YUV444     VPU_OUTPUT_FORMAT_YUV444
#define  VPU_PP_OUTPUT_FORMAT_YCH420     VPU_OUTPUT_FORMAT_YCH420

#define  VPU_PP_SCALE_DISABLE      0
#define  VPU_PP_SCALE_ENABLE       1

#define  VPU_PP_ROTATE_DISABLE      0
#define  VPU_PP_ROTATE_ENABLE      1

#define  VPU_PP_DITHER_DISABLE      0
#define  VPU_PP_DITHER_ENABLE      1

#define  VPU_PP_DEBLK_DISABLE      0
#define  VPU_PP_DEBLK_ENABLE       1

#define  VPU_PP_DEINTERLACE_DISABLE     0
#define  VPU_PP_DEINTERLACE_ENABLE     1

#define     VPU_BITSTREAM_START_CODE        (0x524b5642)    /* RKVB, rockchip video bitstream */

#define     SWAP(_T_,A,B)       { _T_ tmp = A; A = B; B = tmp; }
#ifndef MIN
#define     MIN(X, Y)           ((X)<(Y)?(X):(Y))
#endif
#ifndef MAX
#define     MAX(X, Y)           ((X)>(Y)?(X):(Y))
#endif
#ifndef ABS
#define     ABS(X)              (((X) < 0) ? (-(X)) : (X))
#endif
#define     SIGN(X)             (((X)>0)?1:-1)
#define     BSWAP(a) \
    ((((a) & 0xff) << 24)  | (((a) & 0xff00) << 8) | \
           (((a) >> 8) & 0xff00) | (((a) >> 24) & 0xff))

#ifndef CLIP
#define     CLIP(X,AMIN,AMAX)   (((X)<(AMIN)) ? (AMIN) : ((X)>(AMAX)) ? (AMAX) : (X))
#endif

#define     rd(addr)            *((unsigned int volatile*)(addr))
#define     wr(addr, value)     *(((unsigned int volatile*)(addr))) = value

//#define FRAME_COPY

//#define _VPU_TRACE_ENABLE

#ifdef _VPU_TRACE_ENABLE
#include <utils/Log.h>
#define VPU_TRACE(fmt,args...) LOGD(__FILE__ ":%d: " fmt, __LINE__ , ## args)
#define VPU_T(fmt, args...)
#else
#define VPU_TRACE(fmt, args...)
#define VPU_T(fmt,args...) LOGD(__FILE__ ":%d: " fmt, __LINE__ , ## args)
#endif


#endif /*_VPU_MACRO_*/