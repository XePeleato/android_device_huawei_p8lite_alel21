/***************************************************************************************************
    File:
        vpu_global.h
    Description:
        Global struct definition in VPU module
    Author:
        Jian Huan
    Date:
        2010-11-23 21:48:40
 **************************************************************************************************/
#ifndef _VPU_GLOBAL_
#define _VPU_GLOBAL_

#include "vpu_macro.h"
#include "vpu_mem.h"

typedef struct
{
    RK_U32   TimeLow;
    RK_U32   TimeHigh;
}TIME_STAMP;

typedef struct
{
    RK_U32   CodecType;
    RK_U32   ImgWidth;      //?16X??,??????
    RK_U32   ImgHeight;      //?16X??,??????
}VPU_GENERIC;

typedef enum DEC_TYPE_T {
   HW,
   SW,
}DEC_TYPE;

typedef struct
{
    RK_U32      StartCode;
    RK_U32      SliceLength;
    TIME_STAMP  SliceTime;
    RK_U32      SliceType;
    RK_U32      SliceNum;
    RK_U32      Res[2];
}VPU_BITSTREAM;  /*completely same as RK28*/

typedef struct
{
    RK_U32      InputAddr[2];
    RK_U32      OutputAddr[2];
    RK_U32      InputWidth;
    RK_U32      InputHeight;
    RK_U32      OutputWidth;
    RK_U32      OutputHeight;
    RK_U32      ColorType;
    RK_U32      ScaleEn;
    RK_U32      RotateEn;
    RK_U32      DitherEn;
    RK_U32      DeblkEn;
    RK_U32      DeinterlaceEn;
    RK_U32      Res[5];
}VPU_POSTPROCESSING;

typedef struct tVPU_FRAME
{
    RK_U32          FrameBusAddr[2];       //0: Y address; 1: UV address;
    RK_U32          FrameWidth;         //16X????
    RK_U32          FrameHeight;        //16X????
    RK_U32          OutputWidth;        //?16X??
    RK_U32          OutputHeight;       //?16X??
    RK_U32          DisplayWidth;       //????
    RK_U32          DisplayHeight;      //????
    RK_U32          CodingType;
    RK_U32          FrameType;          //frame;top_field_first;bot_field_first
    RK_U32          ColorType;
    RK_U32          DecodeFrmNum;
    TIME_STAMP      ShowTime;
    RK_U32          ErrorInfo;          //???????,?????????
    RK_U32	        employ_cnt;
    VPUMemLinear_t  vpumem;
    struct tVPU_FRAME *    next_frame;
    RK_U32          Res[4];
}VPU_FRAME;

typedef enum VPU_API_CMD
{
   VPU_API_ENC_SETCFG,
   VPU_API_ENC_GETCFG,
   VPU_API_ENC_SETFORMAT,
   VPU_API_ENC_SETIDRFRAME,
   VPU_API_ENABLE_DEINTERLACE,
   VPU_API_SET_VPUMEM_CONTEXT,
   VPU_API_USE_PRESENT_TIME_ORDER,
   VPU_API_SET_DEFAULT_WIDTH_HEIGH,
   VPU_API_SET_INFO_CHANGE,
}VPU_API_CMD;


#endif /*_VPU_GLOBAL_*/