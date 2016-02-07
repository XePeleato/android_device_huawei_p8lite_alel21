/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: OmRingBuffer.h                                                  */
/*                                                                           */
/* Author: Windriver                                                         */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-06                                                             */
/*                                                                           */
/* Description: implement ring buffer subroutine                             */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author: H59254                                                         */
/*    Modification: Adapt this file                                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef  _OM_RING_BUFFER_H
#define  _OM_RING_BUFFER_H

#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


typedef struct
{
    int  pToBuf;    /* offset from start of buffer where to write next */
    int  pFromBuf;  /* offset from start of buffer where to read next */
    int  bufSize;   /* size of ring in bytes */
    char acRsv[4];
    char *buf;      /* pointer to start of buffer */
} OM_RING;

typedef OM_RING * OM_RING_ID;

#if(FEATURE_ON == FEATURE_PTM)
OM_RING_ID OM_RingBufferCreateEx(char *pdata, int nbytes );
#endif

OM_RING_ID OM_RingBufferCreate( int nbytes );

void OM_RingBufferFlush( OM_RING_ID ringId );

int OM_RingBufferGet( OM_RING_ID rngId, char *buffer, int maxbytes );

int OM_RingBufferRemove( OM_RING_ID rngId, int maxbytes );

int OM_RingBufferGetReserve( OM_RING_ID rngId, char *buffer, int maxbytes );

int OM_RingBufferPut( OM_RING_ID rngId, char *buffer, int nbytes );

VOS_BOOL OM_RingBufferIsEmpty( OM_RING_ID ringId );

int OM_RingBufferFreeBytes( OM_RING_ID ringId);

int OM_RingBufferNBytes( OM_RING_ID ringId );


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _OM_RING_BUFFER_H */

