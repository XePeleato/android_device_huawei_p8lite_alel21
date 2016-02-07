/*!
 *****************************************************************************
 *
 * @File       mtxio.c
 * @Title      Low-level MTX interface component
 * @Description    This file contains functions to communicate with MTX firmware.  This
 *  includes sending and receiving messages and state indicators.
 *  It is a subcomponent of the Decoder block and operates on the Decoder
 *  block state.
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) Imagination Technologies Ltd.
 * 
 * The contents of this file are subject to the MIT license as set out below.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * Alternatively, the contents of this file may be used under the terms of the 
 * GNU General Public License Version 2 ("GPL")in which case the provisions of
 * GPL are applicable instead of those above. 
 * 
 * If you wish to allow use of your version of this file only under the terms 
 * of GPL, and not to allow others to use your version of this file under the 
 * terms of the MIT license, indicate your decision by deleting the provisions 
 * above and replace them with the notice and other provisions required by GPL 
 * as set out in the file called "GPLHEADER" included in this distribution. If 
 * you do not delete the provisions above, a recipient may use your version of 
 * this file under the terms of either the MIT license or GPL.
 * 
 * This License is also included in this distribution in the file called 
 * "MIT_COPYING".
 *
 *****************************************************************************/

#include "img_errors.h"
#include "img_types.h"
#include "lst.h"

#include "vdecfw_msg_mem_io.h"

#include "mtxio.h"
#include "msvdx_io.h"
#include "vxd_io.h"

#ifdef SECURE_MEDIA_REPORTING
#include "report_api.h"
#else
#define REPORT(MODULE, LEVEL, fmt, ...)
#define DEBUG_REPORT(MODULE, fmt, ...)
#endif

#include "vdecfw.h"

#include "msvdx_mtx_reg_io2.h"


#ifdef SECURE_USE_SYSOS

#include "sysos_api_km.h"
#define SEC_CreateEventObject(phEventHandle)                            SYSOSKM_CreateEventObject(phEventHandle)
#define SEC_WaitEventObject(hEventHandle, bUninterruptible)             SYSOSKM_WaitEventObject(hEventHandle, bUninterruptible)
#define SEC_SignalEventObject(hEventHandle)                             SYSOSKM_SignalEventObject(hEventHandle)
#define SEC_DestroyEventObject(hEventHandle)                            SYSOSKM_DestroyEventObject(hEventHandle)
#define SEC_CreateTimer(pfnTimer, pvParam, ui32TimeOut, phTimerHandle)  SYSOSKM_CreateTimer(pfnTimer, pvParam, ui32TimeOut, phTimerHandle)
#define SEC_DestroyTimer(hTimerHandle)                                  SYSOSKM_DestroyTimer(hTimerHandle)

#else

#define SEC_CreateEventObject(phEventHandle)                            IMG_ERROR_NOT_SUPPORTED
#define SEC_WaitEventObject(hEventHandle, bUninterruptible)             IMG_ERROR_NOT_SUPPORTED
#define SEC_SignalEventObject(hEventHandle)                             IMG_ERROR_NOT_SUPPORTED
#define SEC_DestroyEventObject(hEventHandle)                            IMG_ERROR_NOT_SUPPORTED
#define SEC_CreateTimer(pfnTimer, pvParam, ui32TimeOut, phTimerHandle)  IMG_ERROR_NOT_SUPPORTED
#define SEC_DestroyTimer(hTimerHandle)                                  IMG_ERROR_NOT_SUPPORTED

#endif


#define EXCLUDE_PDUMP

/*! Value to reset all of VEC RAM too at start */
#define MTXIO_VEC_RAM_RESET_VAL   (0x0)


/*! Value for ui32TimeOut in call to TALREG_CircBufPoll32 - should approximate to 1ms */
#define MTXIO_TAL_NUM_CYCLES_IN_1MS     (100)
/*! Value for ui32TimeOut in call to TALREG_Poll32 - should approximate to 100us */
#define MTXIO_TAL_NUM_CYCLES_IN_100US   (MTXIO_TAL_NUM_CYCLES_IN_1MS/10)
/*! Value for ui32PollCount in call to TALREG_Poll32 - should be long enough for FW to write a message
 *  If ui32TimeOut is correctly set to 100us, 0x0013_0000 should equate to just over 2mins
 *  Against real silicon we expect messages be produced at an interval around 30ms */
#define MTXIO_TAL_POLL_REPEAT_COUNT     (0x00130000)



/*!
******************************************************************************
 Enum describing fields in the #VDECFW_sCommsHeader structure
******************************************************************************/
typedef enum
{
    MTXIO_FIELD_SIZE = 0,
    MTXIO_FIELD_RD_INDEX,
    MTXIO_FIELD_WRT_INDEX,
    MTXIO_FIELD_OFFSET_INDEX,

    MTXIO_FIELD_MAX,            //!< end marker

} MTXIO_eFieldId;


/*!
******************************************************************************
 Type for a portion of memory equal to the largest message buffer
******************************************************************************/
typedef struct
{
    union {
        IMG_UINT32 aui32ControlMsgHdr[VDECFW_CONTROL_COMMS_BUF_SIZE];
        IMG_UINT32 aui3DecodeMsgHdr[VDECFW_DECODE_COMMS_BUF_SIZE];
        IMG_UINT32 aui3CompletionMsgHdr[VDECFW_COMPLETION_COMMS_BUF_SIZE];
    };

}MTXIO_sCommsBuf;


/*!
******************************************************************************
 Offset in VLR of each of the comms areas
******************************************************************************/
static const IMG_UINT32 gaui32VlrOffset[MTXIO_AREA_MAX] =
{
    VLR_CONTROL_COMMS_AREA_BASE_ADDR,        // MTXIO_AREA_CONTROL
    VLR_DECODE_COMMS_AREA_BASE_ADDR,         // MTXIO_AREA_DECODE
    VLR_COMPLETION_COMMS_AREA_BASE_ADDR,     // MTXIO_AREA_COMPLETION
};



/*!
******************************************************************************

 @Function              mtxio_commsWriteWords

******************************************************************************/
static IMG_VOID
mtxio_commsWriteWords(
    const MTXIO_sContext  * psContext,
    VXD_eCommsArea          eCommsArea,
    IMG_UINT32              ui32Offset,
    IMG_UINT32              ui32NumWords,
    const IMG_UINT32      * pui32Values
)
{
    IMG_UINT32      ui32Result;

    IMG_ASSERT(eCommsArea < MTXIO_AREA_MAX);

    ui32Result = VXDIO_VLRWriteWords(psContext->hMsvdxIoCtx,
                                     psContext->asComms[eCommsArea].eMemSpace,
                                     gaui32VlrOffset[eCommsArea] + psContext->asComms[eCommsArea].ui32BufOffset + ui32Offset,
                                     ui32NumWords,
                                     pui32Values);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
}


/*!
******************************************************************************

 @Function              mtxio_commsReadWords

******************************************************************************/
static IMG_VOID
mtxio_commsReadWords(
    const MTXIO_sContext  * psContext,
    VXD_eCommsArea          eCommsArea,
    IMG_UINT32              ui32Offset,
    IMG_UINT32              ui32NumWords,
    IMG_UINT32            * pui32Values,
    IMG_BOOL                bValidate
)
{
    IMG_UINT32      ui32Result;

    IMG_ASSERT(eCommsArea < MTXIO_AREA_MAX);

    ui32Result = VXDIO_VLRReadWords(psContext->hMsvdxIoCtx,
                                    psContext->asComms[eCommsArea].eMemSpace,
                                    gaui32VlrOffset[eCommsArea] + ui32Offset,
                                    ui32NumWords,
                                    pui32Values,
                                    bValidate);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
}


/*!
******************************************************************************

 @Function              mtxio_commsAreaSet

******************************************************************************/
static IMG_VOID
mtxio_commsAreaSet(
    const MTXIO_sContext  * psContext,
    VXD_eCommsArea          eArea,
    MTXIO_eFieldId          eFieldId,
    IMG_UINT32              ui32WriteValue
)
{
    const MTXIO_sCommsInfo    * psCommsInfo;
    IMG_UINT32                  ui32Address = 0;
    IMG_UINT32                  ui32AreaOffset;
    IMG_UINT32                  ui32Result;
#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    IMG_CHAR                    szPdumpComment[100];
#endif

    IMG_ASSERT( eArea < MTXIO_AREA_MAX );
    IMG_ASSERT( eFieldId < MTXIO_FIELD_MAX );

    /* get comms area information and offset for it in VLR */
    psCommsInfo = &psContext->asComms[eArea];
    ui32AreaOffset = gaui32VlrOffset[eArea];

    switch ( eFieldId )
    {
    case MTXIO_FIELD_RD_INDEX:
        ui32Address = ui32AreaOffset + psCommsInfo->ui32RdIndexOffset;
        break;
    case MTXIO_FIELD_WRT_INDEX:
        ui32Address = ui32AreaOffset + psCommsInfo->ui32WrtIndexOffset;
        break;
    case MTXIO_FIELD_SIZE:
        // Setting the size field is permitted as this is required at start-up
        // but this is a constant and must no change
        ui32Address = ui32AreaOffset + psCommsInfo->ui32SizeOffset;
        ui32WriteValue = psCommsInfo->ui32Size;
        break;
    case MTXIO_FIELD_OFFSET_INDEX:
    default:
        IMG_ASSERT(IMG_NULL=="mtxio_commsAreaSet: Not permitted to write to this field");
    }
#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)

    snprintf(szPdumpComment, 99, "mtxio_commsAreaSet: Update %s area %s", eArea == MTXIO_AREA_CONTROL ?  "CONTROL" :
                                                                          eArea == MTXIO_AREA_DECODE ? "DECODE" :
                                                                          eArea == MTXIO_AREA_COMPLETION ? "COMPLETION" : "UNKNOWN",
                                                                          eFieldId == MTXIO_FIELD_RD_INDEX ? "READ INDEX" :
                                                                          eFieldId == MTXIO_FIELD_WRT_INDEX ? "WRITE INDEX" :
                                                                          eFieldId == MTXIO_FIELD_SIZE ? "SIZE" : "UNKNOWN");
    szPdumpComment[99] = 0;
    VXDIO_PDUMPComment(psContext->hMsvdxIoCtx, psContext->asComms[eArea].eMemSpace,
        szPdumpComment);

#endif

    ui32Result = VXDIO_VLRWriteWords(psContext->hMsvdxIoCtx,
                                     psCommsInfo->eMemSpace,
                                     ui32Address,
                                     1,
                                     &ui32WriteValue);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
}


/*!
******************************************************************************

 @Function              mtxio_commsAreaGet

******************************************************************************/
static IMG_UINT32
mtxio_commsAreaGet(
    const MTXIO_sContext  * psContext,
    VXD_eCommsArea          eArea,
    MTXIO_eFieldId          eFieldId
)
{
    const MTXIO_sCommsInfo    * psCommsInfo;
    IMG_UINT32                  ui32Address = 0;
    IMG_UINT32                  ui32AreaOffset = 0;
    IMG_UINT32                  ui32ReadValue = 0;
    IMG_UINT32                  ui32Result;

    IMG_ASSERT( eArea < MTXIO_AREA_MAX );
    IMG_ASSERT( eFieldId < MTXIO_FIELD_MAX );

    /* get comms area information and offset for it in VLR */
    psCommsInfo = &psContext->asComms[eArea];
    ui32AreaOffset = gaui32VlrOffset[eArea];

    switch ( eFieldId )
    {
    case MTXIO_FIELD_RD_INDEX:
        ui32Address = ui32AreaOffset + psCommsInfo->ui32RdIndexOffset;
        break;
    case MTXIO_FIELD_WRT_INDEX:
        ui32Address = ui32AreaOffset + psCommsInfo->ui32WrtIndexOffset;
        break;
    case MTXIO_FIELD_SIZE:
        ui32Address = ui32AreaOffset + psCommsInfo->ui32SizeOffset;
        break;
    case MTXIO_FIELD_OFFSET_INDEX:
    default:
        IMG_ASSERT(IMG_NULL=="mtxio_commsAreaSet: Not permitted to read to this field");
    }

    ui32Result = VXDIO_VLRReadWords(psContext->hMsvdxIoCtx,
                                    psCommsInfo->eMemSpace,
                                    ui32Address,
                                    1,
                                    &ui32ReadValue,
                                    IMG_FALSE);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);

    return ui32ReadValue;
}


/*
******************************************************************************

 @Function              MTXIO_InitMTXComms

******************************************************************************/
IMG_RESULT
MTXIO_InitMTXComms(
    const IMG_HANDLE    hMsvdxIoCtx,
    MTXIO_sContext    * psMtxIoCtx
)
{
    IMG_UINT32  ui32Offset;
    IMG_UINT32  ui32VecRamVal = 0;
    IMG_UINT32  ui32Result;

    /* setup buffer pointers, sizes and handles */
    VDECFW_sCommsControl * psControlArea = IMG_NULL;
    VDECFW_sCommsDecode * psDecodeArea = IMG_NULL;
    VDECFW_sCommsCompletion * psCompletionArea = IMG_NULL;

#ifdef VXD_BRIDGING
    (void)gsTargetConfig; /* Avoid compiler warning. */
#endif /* VXD_BRIDGING */

    IMG_MEMSET(psMtxIoCtx, 0, sizeof(*psMtxIoCtx));

    psMtxIoCtx->hMsvdxIoCtx = hMsvdxIoCtx;

    /* Initially set all areas to general VLRFE_REGSPACE so the setup of VLR memory comes to FE Pdump Context */
    psMtxIoCtx->asComms[MTXIO_AREA_CONTROL].eMemSpace = REGION_VLRFE_REGSPACE;
    psMtxIoCtx->asComms[MTXIO_AREA_DECODE].eMemSpace = REGION_VLRFE_REGSPACE;
    psMtxIoCtx->asComms[MTXIO_AREA_COMPLETION].eMemSpace = REGION_VLRFE_REGSPACE;

    /* Set field offsets */
    psMtxIoCtx->asComms[MTXIO_AREA_CONTROL].ui32SizeOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psControlArea->sHeader.ui32BufSize - (IMG_UINTPTR)&psControlArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_CONTROL].ui32RdIndexOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psControlArea->sHeader.ui32RdIndex - (IMG_UINTPTR)&psControlArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_CONTROL].ui32WrtIndexOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psControlArea->sHeader.ui32WrtIndex - (IMG_UINTPTR)&psControlArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_CONTROL].ui32BufOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psControlArea->sHeader.aui32Buf - (IMG_UINTPTR)&psControlArea->sHeader);

    psMtxIoCtx->asComms[MTXIO_AREA_DECODE].ui32SizeOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psDecodeArea->sHeader.ui32BufSize - (IMG_UINTPTR)&psDecodeArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_DECODE].ui32RdIndexOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psDecodeArea->sHeader.ui32RdIndex - (IMG_UINTPTR)&psDecodeArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_DECODE].ui32WrtIndexOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psDecodeArea->sHeader.ui32WrtIndex - (IMG_UINTPTR)&psDecodeArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_DECODE].ui32BufOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psDecodeArea->sHeader.aui32Buf - (IMG_UINTPTR)&psDecodeArea->sHeader);

    psMtxIoCtx->asComms[MTXIO_AREA_COMPLETION].ui32SizeOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psCompletionArea->sHeader.ui32BufSize - (IMG_UINTPTR)&psCompletionArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_COMPLETION].ui32RdIndexOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psCompletionArea->sHeader.ui32RdIndex - (IMG_UINTPTR)&psCompletionArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_COMPLETION].ui32WrtIndexOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psCompletionArea->sHeader.ui32WrtIndex - (IMG_UINTPTR)&psCompletionArea->sHeader);
    psMtxIoCtx->asComms[MTXIO_AREA_COMPLETION].ui32BufOffset =
        (IMG_UINT32)((IMG_UINTPTR)&psCompletionArea->sHeader.aui32Buf - (IMG_UINTPTR)&psCompletionArea->sHeader);

    /* set buffer sizes - in 32-bit words */
    psMtxIoCtx->asComms[MTXIO_AREA_CONTROL].ui32Size =
        (VLR_CONTROL_COMMS_AREA_SIZE - psMtxIoCtx->asComms[MTXIO_AREA_CONTROL].ui32BufOffset) >> 2;
    psMtxIoCtx->asComms[MTXIO_AREA_DECODE].ui32Size =
        (VLR_DECODE_COMMS_AREA_SIZE - psMtxIoCtx->asComms[MTXIO_AREA_DECODE].ui32BufOffset) >> 2;
    psMtxIoCtx->asComms[MTXIO_AREA_COMPLETION].ui32Size =
        (VLR_COMPLETION_COMMS_AREA_SIZE - psMtxIoCtx->asComms[MTXIO_AREA_COMPLETION].ui32BufOffset) >> 2;

    /* clear VLR */
    ui32VecRamVal = MTXIO_VEC_RAM_RESET_VAL;
    for (ui32Offset = 0; ui32Offset < VLR_SIZE; ui32Offset += 4)
    {
        ui32Result = VXDIO_VLRWriteWords(hMsvdxIoCtx,
                                         REGION_VLRFE_REGSPACE,
                                         ui32Offset,
                                         1,
                                         &ui32VecRamVal);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
    }

    /* set up message buffers ready to use */
    /* size is forced to a value derived from system.h in mtxio_initCommsInfo() */
    mtxio_commsAreaSet(psMtxIoCtx, MTXIO_AREA_CONTROL, MTXIO_FIELD_SIZE, 0);
    mtxio_commsAreaSet(psMtxIoCtx, MTXIO_AREA_DECODE, MTXIO_FIELD_SIZE, 0);
    mtxio_commsAreaSet(psMtxIoCtx, MTXIO_AREA_COMPLETION, MTXIO_FIELD_SIZE, 0);

    /* After setup set each area to the right memspace so they come up to the right Pdump context */
    psMtxIoCtx->asComms[MTXIO_AREA_COMPLETION].eMemSpace = REGION_VLRBE_REGSPACE;

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              mtxio_fnTimer

******************************************************************************/
static IMG_BOOL
mtxio_fnTimer(
    IMG_VOID * pvParam
)
{
    IMG_HANDLE hEventHandle = (IMG_HANDLE)pvParam;

    /* Wake up the suspended process...*/
    SEC_SignalEventObject(hEventHandle);

    /* Indiacte that we want to be rescheduled...*/
    return IMG_TRUE;
}


/*!
******************************************************************************

 @Function              mtxio_waitForCommsSpace

******************************************************************************/
static IMG_VOID
mtxio_waitForCommsSpace(
    const MTXIO_sContext  * psContext,
    VXD_eCommsArea          eArea,
    IMG_UINT32              ui32WriteIdx,
    IMG_UINT32              ui32MessageSize,
    IMG_UINT32              ui32BufferSize
)
{
    IMG_UINT32 ui32ReadIndex;
    IMG_UINT32 ui32WaitRetry = MSVDX_MAX_NUM_COMMS_SPACE_RETRIES;
 //   IMG_HANDLE hCommsMemSpace = psContext->asComms[eArea].hVlrMemSpace;

    /* setup wait object */
    IMG_HANDLE hEventHandle = IMG_NULL;
    IMG_HANDLE hTimerHandle = IMG_NULL;
    IMG_RESULT ui32Result;

    /* create an event object */
    ui32Result = SEC_CreateEventObject( &hEventHandle );
    IMG_ASSERT( ui32Result == IMG_SUCCESS );
    if ( ui32Result != IMG_SUCCESS )
    {
        return;
    }
    /* create a 1ms timer and attach to the event object */
    ui32Result = SEC_CreateTimer( mtxio_fnTimer, hEventHandle, 1, &hTimerHandle );
    IMG_ASSERT( ui32Result == IMG_SUCCESS );
    if ( ui32Result != IMG_SUCCESS )
    {
        SEC_DestroyEventObject(hEventHandle);
        return;
    }

    /* main wait loop */
    ui32ReadIndex = mtxio_commsAreaGet(psContext, eArea, MTXIO_FIELD_RD_INDEX);
    while (
                (
                    (ui32WaitRetry--) &&
                    /* write pointer will overtake read pointer */
                    (
                        (
                            (ui32WriteIdx < ui32ReadIndex) &&
                            ((ui32WriteIdx + ui32MessageSize) >= ui32ReadIndex)
                        ) ||
                        /* or message will fill buffer and hit read pointer at start of buffer */
                        (
                            (ui32ReadIndex == 0) &&
                            ((ui32WriteIdx + ui32MessageSize) == ui32BufferSize)
                        )
                    )
                )
            )
    {
        /* wait for a millisecond */
        SEC_WaitEventObject( hEventHandle, IMG_TRUE );

        ui32ReadIndex = mtxio_commsAreaGet(psContext, eArea, MTXIO_FIELD_RD_INDEX);
    }

    /* remove the timer before the event object */
    /* remove wait object */
    if ( hTimerHandle )
    {
        SEC_DestroyTimer( hTimerHandle );
    }

    /* remove wait object */
    if ( hEventHandle )
    {
        SEC_DestroyEventObject( hEventHandle );
    }

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    {
        IMG_CHAR szPdumpComment[100];
        snprintf(szPdumpComment, 99, "mtxio_WaitForCommsSpace: %s Area",
            eArea == MTXIO_AREA_CONTROL ?  "CONTROL" :
            eArea == MTXIO_AREA_DECODE ? "DECODE" :
            eArea == MTXIO_AREA_COMPLETION ? "COMPLETION" : "UNKNOWN");

        szPdumpComment[99] = 0;
        /* reproduce wait in a pdump script */
        VXDIO_PDUMPComment(psContext->hMsvdxIoCtx, psContext->asComms[eArea].eMemSpace, szPdumpComment);

        VXDIO_PDUMPPollCircBuff(psContext->hMsvdxIoCtx,
                             psContext->asComms[eArea].eMemSpace,
                             gaui32VlrOffset[eArea] + psContext->asComms[eArea].ui32RdIndexOffset,
                             ui32WriteIdx,
                             ui32MessageSize,
                             ui32BufferSize);
    }
#endif
}


/*
******************************************************************************

 @Function              MTXIO_SendMTXMsg

******************************************************************************/
IMG_RESULT
MTXIO_SendMTXMsg(
    const MTXIO_sContext  * psContext,
    VXD_eCommsArea          eArea,
    const IMG_VOID        * psMsgHdr
)
{
    IMG_UINT32      ui32BufferSize;
    IMG_UINT32      ui32WriteIdx;
    IMG_UINT32    * pui32Message = (IMG_UINT32*)psMsgHdr;
    IMG_UINT32      ui32MessageSize = (MEMIO_READ_FIELD(psMsgHdr, V2_PADMSG_SIZE) + 3) / 4;
    MTXIO_sCommsBuf sCommsBuf;
    IMG_UINT32    * pui32PaddingHeader = sCommsBuf.aui32ControlMsgHdr;
    IMG_UINT8       ui8PaddingID;
    IMG_UINT8       ui8PaddingSize;
    IMG_UINT8       ui32PaddingWords;
#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    IMG_CHAR        szPdumpComment[100];
#endif

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    snprintf(szPdumpComment, 99, "MTXIO_SendMTXMsg - Send message to FW area %s",
        eArea == MTXIO_AREA_CONTROL ?  "CONTROL" :
        eArea == MTXIO_AREA_DECODE ? "DECODE" :
        eArea == MTXIO_AREA_COMPLETION ? "COMPLETION" : "UNKNOWN");
    szPdumpComment[99] = 0;
    /* reproduce wait in a pdump script */
    VXDIO_PDUMPComment(psContext->hMsvdxIoCtx, psContext->asComms[eArea].eMemSpace,
       szPdumpComment);
#endif

    ui32BufferSize = mtxio_commsAreaGet(psContext, eArea, MTXIO_FIELD_SIZE);
    ui32WriteIdx = mtxio_commsAreaGet(psContext, eArea, MTXIO_FIELD_WRT_INDEX);
    if (psContext->asComms[eArea].ui32Size != ui32BufferSize)
    {
        REPORT(REPORT_MODULE_MTXIO, REPORT_ERR, "Comms area seems to be corrupted, wrong buffer size!");
        return IMG_ERROR_GENERIC_FAILURE;
    }

    IMG_ASSERT(ui32MessageSize <= ui32BufferSize);
    IMG_ASSERT( MTXIO_AREA_CONTROL == eArea || MTXIO_AREA_DECODE == eArea );
    IMG_MEMSET( pui32PaddingHeader, 0, sizeof(MTXIO_sCommsBuf) );

    // prepare a padding message
    ui8PaddingID = VDECFW_MSGID_BASE_PADDING;
    if ( MTXIO_AREA_DECODE == eArea )
    {
        ui8PaddingID = VDECFW_MSGID_PSR_PADDING;
    }

    // if there is not enough space in the buffer, send the padding message
    if ( (ui32WriteIdx + ui32MessageSize) > ui32BufferSize )
    {
        ui32PaddingWords = ui32BufferSize - ui32WriteIdx;
        if ( ((ui32PaddingWords * 4) > 0xFF) || (ui32BufferSize < ui32WriteIdx))
        {
            REPORT(REPORT_MODULE_MTXIO, REPORT_ERR, "Comms area seems to be corrupted, wrong write index!");
            return IMG_ERROR_GENERIC_FAILURE;
        }
        ui8PaddingSize = (ui32PaddingWords * 4);

        MEMIO_WRITE_FIELD(pui32PaddingHeader, V2_PADMSG_SIZE, ui8PaddingSize);
        MEMIO_WRITE_FIELD(pui32PaddingHeader, V2_PADMSG_MID, ui8PaddingID);

        // wait until there is space to write the message into the message buffer
        mtxio_waitForCommsSpace( psContext, eArea, ui32WriteIdx, ui32PaddingWords, ui32BufferSize );

        // write the message
        mtxio_commsWriteWords( psContext, eArea, (ui32WriteIdx<<2), ui32PaddingWords, pui32PaddingHeader );

        // set the new write index
        IMG_ASSERT(ui32WriteIdx + ui32PaddingWords == ui32BufferSize);
        ui32WriteIdx = 0;
        mtxio_commsAreaSet( psContext, eArea, MTXIO_FIELD_WRT_INDEX, ui32WriteIdx );
    }

    // wait until there is space to write the message into the message buffer
    mtxio_waitForCommsSpace( psContext, eArea, ui32WriteIdx, ui32MessageSize, ui32BufferSize );

    // write the message
    mtxio_commsWriteWords( psContext, eArea, (ui32WriteIdx<<2), ui32MessageSize, pui32Message );

    // calculate and set the new write index
    ui32WriteIdx += ui32MessageSize;
    if ( ui32WriteIdx >= ui32BufferSize )
    {
        ui32WriteIdx = 0;
    }
    mtxio_commsAreaSet( psContext, eArea, MTXIO_FIELD_WRT_INDEX, ui32WriteIdx );

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    /* reproduce wait in a pdump script */
    VXDIO_PDUMPComment(psContext->hMsvdxIoCtx, psContext->asComms[eArea].eMemSpace,
        "MTXIO_SendMTXMsg - Message to FW sent" );
#endif

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              mtxio_copyMessageData

******************************************************************************/
static IMG_UINT32
mtxio_copyMessageData(
    const MTXIO_sContext  * psContext,
    VXD_eCommsArea          eArea,
    IMG_UINT32              ui32ReadIdx,
    IMG_UINT32              ui32HostWriteIdx,
    IMG_UINT32              ui32HostReadIdx,
    IMG_UINT32              ui32MessageSize,
    IMG_UINT32              ui32BufferSize,
    VXDIO_sHISRMsg        * psMessage,
    IMG_UINT32            * pui32CopyMsgBuf
)
{

    if ( (ui32HostWriteIdx + ui32MessageSize) > MSVDX_SIZE_MSG_BUFFER )
    {
        ui32HostWriteIdx = 0;
    }

    // Sanity check - message container has been provided.
    IMG_ASSERT( psMessage );

    /* sanity check - message will fit in internal MTXIO buffer */
    IMG_ASSERT( (ui32HostWriteIdx + ui32MessageSize) <= MSVDX_SIZE_MSG_BUFFER );
    IMG_ASSERT( (ui32HostReadIdx <= ui32HostWriteIdx) || (ui32HostWriteIdx + ui32MessageSize) < ui32HostReadIdx );

    /* record the location of the message in the internal MTXIO buffer */
    psMessage->ui32MsgIndex = ui32HostWriteIdx;
    psMessage->ui32MsgSize = ui32MessageSize;

    /* if the message wraps in the MTX buffer, read the portion the the end of the buffer */
    if ( (ui32ReadIdx + ui32MessageSize) >= ui32BufferSize )
    {
        IMG_UINT32 ui32FirstPartSize = ui32BufferSize - ui32ReadIdx;

        mtxio_commsReadWords(psContext,
                             eArea,
                             psContext->asComms[eArea].ui32BufOffset + (ui32ReadIdx<<2),
                             ui32FirstPartSize,
                             &pui32CopyMsgBuf[ui32HostWriteIdx],
                             IMG_TRUE);

        ui32ReadIdx = 0;
        ui32MessageSize -= ui32FirstPartSize;
        ui32HostWriteIdx += ui32FirstPartSize;
    }

    if (ui32MessageSize > 0)
    {
        /* read the (rest of the) message (if there is a rest) */
        mtxio_commsReadWords(psContext,
                             eArea,
                             psContext->asComms[eArea].ui32BufOffset + (ui32ReadIdx<<2),
                             ui32MessageSize,
                             &pui32CopyMsgBuf[ui32HostWriteIdx],
                             IMG_TRUE);
    }

    ui32ReadIdx += ui32MessageSize;
    ui32HostWriteIdx += ui32MessageSize;

    /* update MTXIO internal buffer write index */
    psMessage->ui32NewWriteIndex = ui32HostWriteIdx;

    return ui32ReadIdx;
}


/*
******************************************************************************

 @Function              MTXIO_ProcessMTXMsgs

******************************************************************************/
IMG_RESULT
MTXIO_ProcessMTXMsgs(
    MTXIO_sContext    * psContext,
    VXD_sMsgQueue     * psMsgQStatus
)
{
    IMG_UINT32 ui32ReadIdx, ui32WriteIdx, ui32BufferSize;
    IMG_UINT32 ui32FirstWord, ui32MessageSize;
    IMG_UINT32 * pui32FirstWord;
    VDECFW_eMessageID eMessageID;
    VXDIO_sHISRMsg  * psMessage;

    /* we can only read messages from the Completion buffer */
    VXD_eCommsArea eArea = MTXIO_AREA_COMPLETION;
#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    IMG_CHAR        szPdumpComment[100];
#endif

    // Initialise the output flags.
    IMG_ASSERT(psMsgQStatus);
    psMsgQStatus->bEmpty = IMG_FALSE;
    psMsgQStatus->bQueued = IMG_FALSE;
    IMG_ASSERT(!LST_empty(&psMsgQStatus->sFreeMsgList));
    IMG_ASSERT(LST_empty(&psMsgQStatus->sNewMsgList));

    ui32ReadIdx = mtxio_commsAreaGet( psContext, eArea, MTXIO_FIELD_RD_INDEX );
    ui32WriteIdx = mtxio_commsAreaGet( psContext, eArea, MTXIO_FIELD_WRT_INDEX );
    ui32BufferSize = mtxio_commsAreaGet( psContext, eArea, MTXIO_FIELD_SIZE );

    /* While there are messages to read and somewhere to put them. */
    while (ui32ReadIdx != ui32WriteIdx &&
           LST_first(&psMsgQStatus->sFreeMsgList))
    {
#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
        /* reproduce message reading in a pdump script */
        VXDIO_PDUMPComment(psContext->hMsvdxIoCtx, psContext->asComms[eArea].eMemSpace,
            "MTXIO_ProcessMTXMsgs : Wait for COMPLETION area FW message");

        VXDIO_Poll(psContext->hMsvdxIoCtx,
            psContext->asComms[eArea].eMemSpace,
            gaui32VlrOffset[eArea] + psContext->asComms[eArea].ui32WrtIndexOffset,
            ui32ReadIdx,
            0xffffffff,
            VXDIO_POLL_NOT_EQUAL);
#endif

        /* read the first word of the message */
        mtxio_commsReadWords(psContext,
                             eArea,
                             psContext->asComms[eArea].ui32BufOffset + (ui32ReadIdx<<2),
                             1,
                             &ui32FirstWord,
                             IMG_TRUE);

        /* get the message size in words and ID */
        pui32FirstWord = &ui32FirstWord;
        ui32MessageSize = (MEMIO_READ_FIELD(pui32FirstWord, V2_PADMSG_SIZE) + 3) / 4;
        eMessageID = MEMIO_READ_FIELD(pui32FirstWord, V2_PADMSG_MID);

        /* sanity check - message fits in buffer and does not go past write pointer */
        IMG_ASSERT( ui32MessageSize < ui32BufferSize );
        IMG_ASSERT( (ui32ReadIdx > ui32WriteIdx) || ((ui32ReadIdx + ui32MessageSize) <= ui32WriteIdx) );
        /* sanity check - message is of the MTX -> Host type */
        IMG_ASSERT( eMessageID >= VDECFW_MSGID_BE_PADDING );

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)

        snprintf(szPdumpComment, 99, "MTXIO_ProcessMTXMsgs: %s message received",
            eMessageID == VDECFW_MSGID_BE_PADDING ?  "PADDING" :
            eMessageID == VDECFW_MSGID_PIC_DECODED ? "PIC DECODED" :
            eMessageID == VDECFW_MSGID_PIC_CRCS ? "CRC" :
            eMessageID == VDECFW_MSGID_PIC_PERFORMANCE ? "PERFORMANCE" : "UNKNOWN");
        szPdumpComment[99] = 0;

        VXDIO_PDUMPComment(psContext->hMsvdxIoCtx, psContext->asComms[eArea].eMemSpace,
            szPdumpComment);
#endif
        /* consume if the message is a padding message */
        if ( VDECFW_MSGID_BE_PADDING == eMessageID )
        {
            /* sanity check - message does infact pad to the end of the buffer */
            IMG_ASSERT( ui32ReadIdx > ui32WriteIdx );
            IMG_ASSERT( (ui32ReadIdx + ui32MessageSize) == ui32BufferSize );
            ui32ReadIdx = 0;
            mtxio_commsAreaSet( psContext, eArea, MTXIO_FIELD_RD_INDEX, ui32ReadIdx );
            continue;
        }

        psMessage = LST_removeHead(&psMsgQStatus->sFreeMsgList);
        IMG_ASSERT(psMessage);

        /* copy message into internal buffer and get new read index */
        ui32ReadIdx = mtxio_copyMessageData(psContext,
                                            eArea,
                                            ui32ReadIdx,
                                            psMsgQStatus->ui32WriteIdx,
                                            psMsgQStatus->ui32ReadIdx,
                                            ui32MessageSize,
                                            ui32BufferSize,
                                            psMessage,
                                            psMsgQStatus->aui32MsgBuf);

        psMsgQStatus->ui32WriteIdx = psMessage->ui32NewWriteIndex;

        /* Add the message to the HISR active list */
        LST_add(&psMsgQStatus->sNewMsgList, psMessage);

        /* update the read index */
        mtxio_commsAreaSet( psContext, eArea, MTXIO_FIELD_RD_INDEX, ui32ReadIdx );

        psMsgQStatus->bQueued = IMG_TRUE;

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
        VXDIO_PDUMPComment(psContext->hMsvdxIoCtx, psContext->asComms[eArea].eMemSpace,
            "MTXIO_ProcessMTXMsgs: FW message receive complete");
#endif
    }

    /* report success or otherwise */
    psMsgQStatus->bEmpty = ((ui32ReadIdx == ui32WriteIdx) ? IMG_TRUE: IMG_FALSE);

    return IMG_SUCCESS;
}


/* EOF */
