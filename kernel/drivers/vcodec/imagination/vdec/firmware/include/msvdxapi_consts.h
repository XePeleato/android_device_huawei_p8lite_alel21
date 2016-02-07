/*************************************************************************/ /*!
@File           msvdxapi_consts.h
@Title          MSVDX API Header
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    Exported MSVDX API details
@License        Strictly Confidential.
*/ /**************************************************************************/
#ifndef __MSVDXAPI_CONSTS_H__
#define __MSVDXAPI_CONSTS_H__

#if defined (__cplusplus)
extern "C" {
#endif



/******************************************************************************
 Some defines...
******************************************************************************/

/* MSVDX Heap IDs, note: not all heaps are available to clients */
#define MSVDX_SHARED_HEAP_ID			0
#define MSVDX_BITSTREAM_HEAP_ID			1
#define MSVDX_DATA_HEAP_ID				2
#define MSVDX_FRAMES_HEAP_ID			3
#define MSVDX_TILED_FRAMES_2K_HEAP_ID	4
#define MSVDX_TILED_FRAMES_4K_HEAP_ID	5
#define MSVDX_TILED_FRAMES_8K_HEAP_ID	6

#define MSVDX_MAX_HEAP_ID				6
#define MSVDX_UNDEFINED_HEAP_ID			(~0UL)

/******************************************************************************
 * Capability Flags 
 *****************************************************************************/

/* FW capabilities shalt be defined in top byte of INT32 */
#define CAPABILITY_FW_FS					(IMG_UINT32)(1<<31)
#define CAPABILITY_FW_1KSEGMENT				(IMG_UINT32)(1<<30)
#define CAPABILITY_FW_VC1_HEADER_PARSING	(IMG_UINT32)(1<<29)
#define CAPABILITY_FW_H264_INTRA_DEBLOCK	(IMG_UINT32)(1<<28)
#define CAPABILITY_FW_BE_OPPS_SUPPORTED		(IMG_UINT32)(1<<27)
#define	CAPABILITY_FW_NALU_EXTRACTION		(IMG_UINT32)(1<<26)
#define	CAPABILITY_FW_PVDEC					(IMG_UINT32)(1<<25)


/* The firmware can explicity say that it does not support a standard */
#define	DISABLE_VC1							(IMG_UINT32)(1<<8)

/* 
	HW capabilities related to HW video format support should be defined in
	bytes 2 and 3 of a 4-byte word
*/
#define CAPABILITY_HEVC		(1<<21)
#define CAPABILITY_VP8		(1<<20)
#define CAPABILITY_SORENSON	(1<<19)
#define CAPABILITY_AVS		(1<<18)
#define CAPABILITY_RV		(1<<17)
#define CAPABILITY_VP6	    (1<<16)
#define CAPABILITY_MPEG2	(1<<15)
#define CAPABILITY_MPEG4	(1<<14)
#define CAPABILITY_MPEG4_DP	(1<<13)
#define CAPABILITY_H264		(1<<12)
#define CAPABILITY_WMV9		(1<<11)
#define CAPABILITY_JPEG		(1<<10)
#define CAPABILITY_VC1_BP	(1<<9)
#define CAPABILITY_VC1		(1<<8)

/* 
	HW capabilities not related to video format support
	should be defined in lower byte
*/
#define CAPABILITY_LOSSLESS_H264	(1<<5)
#define CAPABILITY_EXTENDED_STRIDE	(1<<4)
#define CAPABILITY_IDCT_RESIDUAL	(1<<3)
#define CAPABILITY_ROTATION			(1<<2)
#define CAPABILITY_SCALING			(1<<1)
#define CAPABILITY_HD				(1<<0)


#if defined (__cplusplus)
}
#endif

#endif /* __MSVDXAPI_CONSTS_H__ */
