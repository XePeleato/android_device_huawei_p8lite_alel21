/*!
 *****************************************************************************
 *
 * @File       msvdx_msg.c
 * @Title      Messages to MSVDX firmware.
 * @Description    This file contains functions for creating messages to MSVDX firmware.
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

#include "report_api.h"
#include "rman_api.h"
#include "sysos_api_km.h"
#include "vxd_ext.h"
#include "hwctrl_api.h"
#include "msvdx_msg.h"
#include "msvdx_ctx.h"

#include "vdecfw_msg_mem_io.h"

#ifdef USE_REAL_FW
    #include "vdecfw_bin.h"
#endif

#ifndef SYSBRG_BRIDGING
    #include "talmmu_api.h"
#else
    //#include "msvdx_ext.h"
    #include "sysmem_utils.h"
#endif

#ifdef POST_TEST
    /* Calculation test expected result value */
    #define CALC_TEST_RESULT_VALUE                    ((IMG_UINT32)0x496A8B13)
#endif
/*!
******************************************************************************

 @Function              MSVDX_GetInitMessage

******************************************************************************/
IMG_RESULT
MSVDX_GetInitMessage(
    const IMG_HANDLE               hVxd,
    IMG_UINT8                   ** ppui8Msg,
    HWCTRL_sMsgStatus            * psHostMsgStatus,
    VDECDD_sDdDevConfig          * psDevConfig,
    VXDIO_sDdBufInfo             * psRendecBufInfo,
    const VDECFW_sCoreInitData   * psCoreInitData,
    const VXDIO_sDdBufInfo       * psDevPtdBufInfo
)
{
    MSVDX_sContext  * psMsvdxCtx = (MSVDX_sContext *)hVxd;
    IMG_UINT32        ui32I;

    /* Create a control picture message here from the config. */

    IMG_UINT8 * pui8Msg = IMG_MALLOC(V2_INITMSG_SIZE);
    if (pui8Msg == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "Failed to allocate message buffer");
        return IMG_ERROR_MALLOC_FAILED;
    }

    IMG_MEMSET(pui8Msg, 0, V2_INITMSG_SIZE);

    MEMIO_WRITE_FIELD(pui8Msg,V2_INITMSG_SIZE, V2_INITMSG_SIZE);
    MEMIO_WRITE_FIELD(pui8Msg,V2_INITMSG_MID, VDECFW_MSGID_FIRMWARE_INIT);

    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_COREID, psMsvdxCtx->ui32CoreNum);

    if (psDevConfig->eDecodeLevel == VDECDD_DECODELEVEL_FWHDRPARSING)
    {
        MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_PARSEONLY, 1);
    }
    else if (psDevConfig->eDecodeLevel == VDECDD_DECODELEVEL_FEHWDECODING)
    {
        MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_FAKEBE, 1);
    }

    if (psDevConfig->bLockStepDecode)
    {
        MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_LOCKSTEP, 1);
    }

    if (psDevConfig->bPerformanceLog)
    {
        MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_PEFORMANCELOG, 1);
    }

    if (!psDevConfig->bOptimisedPerformance)
    {
        MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_DISABLEOPT, 1);
    }

    //Updating the host pointer because the value was temporary stored in the device memory
    UPDATE_HOST(psDevPtdBufInfo, IMG_TRUE);

    // Populate MMU configuration.
    MEMIO_WRITE_TABLE_FIELD(pui8Msg, V2_INITMSG_MMU_DIR_LIST_BASE, 0, *((IMG_UINT32*)psDevPtdBufInfo->pvCpuVirt));

    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_MMU_CONTROL2, psCoreInitData->sMmuConfig.ui32MmuControl2);
    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_MMU_36_BIT_TWIDDLE, psCoreInitData->sMmuConfig.b36bitMemTwiddle);

    // Populate Rendec configuration.
    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_RENDEC_CONTROL0, psCoreInitData->sRendecConfig.ui32RegVecRendecControl0);
    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_RENDEC_CONTROL1, psCoreInitData->sRendecConfig.ui32RegVecRendecControl1);
    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_RENDEC_BUFFER_ADDR0, psRendecBufInfo[0].ui32DevVirt);
    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_RENDEC_BUFFER_ADDR1, psRendecBufInfo[1].ui32DevVirt);
    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_RENDEC_BUFFER_SIZE, psCoreInitData->sRendecConfig.ui32RegVecRendecBufferSize);
    for (ui32I = 0; ui32I < 6; ui32I++)
    {
        MEMIO_WRITE_TABLE_FIELD(pui8Msg, V2_INITMSG_RENDEC_INITIAL_CONTEXT, ui32I, psCoreInitData->sRendecConfig.aui32RendecInitialContext[ui32I]);
    }

    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_VEC_CONTROL_2, psCoreInitData->ui32RegVecControl2);

    /* Send the message to the firmware */
    MEMIO_WRITE_FIELD(pui8Msg, V2_INITMSG_FENCE_ID, ((++(psHostMsgStatus->aui8ControlFenceID[VDECFW_MSGID_FIRMWARE_INIT-VDECFW_MSGID_BASE_PADDING]))&0xFF));

    DEBUG_REPORT(REPORT_MODULE_VXDIO, "[MID=0x%02X] [CONTROL] [0x%02X]",
        psHostMsgStatus->aui8ControlFenceID[VDECFW_MSGID_FIRMWARE_INIT-VDECFW_MSGID_BASE_PADDING], VDECFW_MSGID_FIRMWARE_INIT);

    *ppui8Msg = pui8Msg;

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              MSVDX_GetDecodeMessage

******************************************************************************/
IMG_RESULT
MSVDX_GetDecodeMessage(
    const IMG_HANDLE              hVxd,
    IMG_UINT8                  ** ppui8Msg,
    HWCTRL_sMsgStatus           * psHostMsgStatus,
    const VDECDD_sDdDevConfig   * psDevConfig,
    const VXDIO_sDdBufInfo      * psStrPtdBufInfo,
    const VXDIO_sDdBufInfo      * psPsrModInfo,
    const VXDIO_sDdBufInfo      * psTransactionBufInfo
)
{
    IMG_UINT8 * pui8Msg = IMG_MALLOC(V2_DECODEMSG_SIZE);
    if (pui8Msg == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "Failed to allocate message buffer");
        return IMG_ERROR_MALLOC_FAILED;
    }
    IMG_MEMSET(pui8Msg, 0, V2_DECODEMSG_SIZE);

    MEMIO_WRITE_FIELD(pui8Msg, V2_DECODEMSG_SIZE, V2_DECODEMSG_SIZE);
    MEMIO_WRITE_FIELD(pui8Msg, V2_DECODEMSG_MID, VDECFW_MSGID_DECODE_PICTURE);


    //Updating the host pointer because the value was temporary stored in the device memory
    UPDATE_HOST(psStrPtdBufInfo, IMG_TRUE);

    MEMIO_WRITE_FIELD(pui8Msg, V2_DECODEMSG_STREAM_PTD, *((IMG_UINT32*)psStrPtdBufInfo->pvCpuVirt));

    MEMIO_WRITE_FIELD(pui8Msg, V2_DECODEMSG_TRANSACTION_ADDR, GET_HOST_ADDR(psTransactionBufInfo, !psDevConfig->bFakeMtx));
    MEMIO_WRITE_FIELD(pui8Msg, V2_DECODEMSG_TRANSACTION_SIZE, sizeof(VDECFW_sTransaction));
    UPDATE_DEVICE(psTransactionBufInfo, IMG_TRUE);

    if (psPsrModInfo->hMemory)
    {
        MEMIO_WRITE_FIELD(pui8Msg, V2_DECODEMSG_PSRMODINFO_ADDR, GET_HOST_ADDR(psPsrModInfo, !psDevConfig->bFakeMtx));
        MEMIO_WRITE_FIELD(pui8Msg, V2_DECODEMSG_PSRMODINFO_SIZE, sizeof(VDECFW_sPsrModInfo) * VDECFW_CODEC_MAX);
        UPDATE_DEVICE(psPsrModInfo, IMG_TRUE);
    }

    /* Send the message to the firmware */
    MEMIO_WRITE_FIELD(pui8Msg, V2_DECODEMSG_FENCE_ID, ((++(psHostMsgStatus->aui8DecodeFenceID[VDECFW_MSGID_DECODE_PICTURE-VDECFW_MSGID_PSR_PADDING]))&0xFF));

    *ppui8Msg = pui8Msg;

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              MSVDX_GetBatchMessage

******************************************************************************/
IMG_RESULT
MSVDX_GetBatchMessage(
    const IMG_HANDLE              hVxd,
    IMG_UINT8                  ** ppui8Msg,
    HWCTRL_sMsgStatus           * psHostMsgStatus,
    const VDECDD_sDdDevConfig   * psDevConfig,
    const IMG_UINT8               ui8BatchMessageID,
    LST_T                       * psDecPictSegList,
    HWCTRL_sBatchMsgData        * psBatchMsgData
)
{
    IMG_RESULT              ui32Result;
    IMG_UINT32              ui32Count = 0;
    DECODER_sDecPictSeg   * psDecPictSeg = IMG_NULL;
    IMG_UINT8             * pui8BatchMsg = psBatchMsgData->psBatchMsgBufInfo->pvCpuVirt;
    IMG_UINT8             * pui8Msg = IMG_MALLOC(V2_DECODEMSG_SIZE);
    if (pui8Msg == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "Failed to allocate message buffer");
        return IMG_ERROR_MALLOC_FAILED;
    }

    //Calculate the size the messages need
    psDecPictSeg = (DECODER_sDecPictSeg *) LST_first(psDecPictSegList);
    while (psDecPictSeg != IMG_NULL)
    {
        //Don't send to the FW the VDECDD_BSSEG_SKIP segments
        if (psDecPictSeg->psBitStrSeg)
        {
            if ((psDecPictSeg->psBitStrSeg->ui32BitStrSegFlag & VDECDD_BSSEG_SKIP) == 0)
            {
                if (psDecPictSeg->psBitStrSeg->ui32BitStrSegFlag &VDECDD_BSSEG_INSERTSCP)
                {
                    ui32Count++;
                }
                ui32Count++;
            }
        }
        psDecPictSeg = LST_next(psDecPictSeg);
    }
    //If it is bigger than the buffer fail.
    IMG_ASSERT(V2_DECODEMSG_SIZE + ui32Count*V2_BUFFERMSG_SIZE <= BATCH_MSG_BUFFER_SIZE);
    if (V2_DECODEMSG_SIZE + ui32Count*V2_BUFFERMSG_SIZE > BATCH_MSG_BUFFER_SIZE)
    {
        //This is temporary and the size of the buffer will be arranged to
        //never fail.
        ui32Result = ~IMG_SUCCESS;
        goto error;
    }

    IMG_MEMSET(pui8BatchMsg, 0, psBatchMsgData->psBatchMsgBufInfo->ui32BufSize);

    /* Create Decode Message */
    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_DECODEMSG_SIZE, V2_DECODEMSG_SIZE);
    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_DECODEMSG_MID, VDECFW_MSGID_DECODE_PICTURE);


    //Updating the host pointer because the value was temporary stored in the device memory
    UPDATE_HOST(psBatchMsgData->psStrPtdBufInfo, IMG_TRUE);

    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_DECODEMSG_STREAM_PTD, *((IMG_UINT32*)psBatchMsgData->psStrPtdBufInfo->pvCpuVirt));

    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_DECODEMSG_TRANSACTION_ADDR,
                      GET_HOST_ADDR(psBatchMsgData->psTransactionBufInfo, !psDevConfig->bFakeMtx));
    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_DECODEMSG_TRANSACTION_SIZE, sizeof(VDECFW_sTransaction));
    UPDATE_DEVICE(psBatchMsgData->psTransactionBufInfo, IMG_TRUE);

    if (psBatchMsgData->psPsrModInfo->hMemory)
    {
        MEMIO_WRITE_FIELD(pui8BatchMsg, V2_DECODEMSG_PSRMODINFO_ADDR,
                          GET_HOST_ADDR(psBatchMsgData->psPsrModInfo, !psDevConfig->bFakeMtx));
        MEMIO_WRITE_FIELD(pui8BatchMsg, V2_DECODEMSG_PSRMODINFO_SIZE,
                          sizeof(VDECFW_sPsrModInfo) * VDECFW_CODEC_MAX);
        UPDATE_DEVICE((psBatchMsgData->psPsrModInfo), IMG_TRUE);
    }

    pui8BatchMsg += V2_DECODEMSG_SIZE;

    /* Create Bitstream messages */
    psDecPictSeg = (DECODER_sDecPictSeg *) LST_first(psDecPictSegList);
    while (psDecPictSeg != IMG_NULL)
    {
        //Don't send to the FW the VDECDD_BSSEG_SKIP segments
        if (psDecPictSeg->psBitStrSeg)
        {
            if ((psDecPictSeg->psBitStrSeg->ui32BitStrSegFlag & VDECDD_BSSEG_SKIP) == 0)
            { 
                VDECDD_sDdBufMapInfo *  psDdBufMapInfo;

                /* Get access to map info context...*/
                ui32Result = RMAN_GetResource(psDecPictSeg->psBitStrSeg->ui32BufMapId, VDECDD_BUFMAP_TYPE_ID, (IMG_VOID **)&psDdBufMapInfo, IMG_NULL);
                IMG_ASSERT(ui32Result == IMG_SUCCESS);
                if (ui32Result != IMG_SUCCESS)
                {
                    goto error;
                }

                if (psDecPictSeg->psBitStrSeg->ui32BitStrSegFlag & VDECDD_BSSEG_INSERTSCP)
                {
                    IMG_UINT32 ui32StartCodeLength = psBatchMsgData->psStartCodeBufInfo->ui32BufSize;

                    if (psDecPictSeg->psBitStrSeg->ui32BitStrSegFlag & VDECDD_BSSEG_INSERT_STARTCODE)
                    {
                        IMG_UINT8 * pui8StartCode = psBatchMsgData->psStartCodeBufInfo->pvCpuVirt;

                        pui8StartCode[ui32StartCodeLength - 1] = psDecPictSeg->psBitStrSeg->ui8StartCodeSuffix;

                        UPDATE_DEVICE(psBatchMsgData->psStartCodeBufInfo, IMG_TRUE);
                    }
                    else
                    {
                        ui32StartCodeLength -= 1;
                    }

                    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_SIZE, V2_BUFFERMSG_SIZE);
                    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_MID, VDECFW_MSGID_BITSTREAM_BUFFER);
                    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_FENCE_ID,
                                      ((++(psHostMsgStatus->aui8DecodeFenceID[VDECFW_MSGID_BITSTREAM_BUFFER-VDECFW_MSGID_PSR_PADDING]))&0xFF));

                    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_DATAADDR, psBatchMsgData->psStartCodeBufInfo->ui32DevVirt);
                    MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_DATASIZE, ui32StartCodeLength);

                    pui8BatchMsg += V2_BUFFERMSG_SIZE;

                    DEBUG_REPORT(REPORT_MODULE_VXDIO, "[MID=0x%02X] [DECODE] [0x%02X]",
                                 psHostMsgStatus->aui8DecodeFenceID[VDECFW_MSGID_BITSTREAM_BUFFER-VDECFW_MSGID_PSR_PADDING],
                                 VDECFW_MSGID_BITSTREAM_BUFFER);
                }

                MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_SIZE, V2_BUFFERMSG_SIZE);
                MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_MID, VDECFW_MSGID_BITSTREAM_BUFFER);
                MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_FENCE_ID,
                                  ((++(psHostMsgStatus->aui8DecodeFenceID[VDECFW_MSGID_BITSTREAM_BUFFER-VDECFW_MSGID_PSR_PADDING]))&0xFF));

                MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_DATAADDR,
                                  psDdBufMapInfo->sDdBufInfo.ui32DevVirt + psDecPictSeg->psBitStrSeg->ui32DataByteOffset);
                MEMIO_WRITE_FIELD(pui8BatchMsg, V2_BUFFERMSG_DATASIZE,
                                  psDecPictSeg->psBitStrSeg->ui32DataSize);

                IMG_ASSERT(psDecPictSeg->psBitStrSeg->ui32DataSize <= psDdBufMapInfo->sDdBufInfo.ui32BufSize);

                UPDATE_DEVICE((&psDdBufMapInfo->sDdBufInfo), IMG_TRUE);

                pui8BatchMsg += V2_BUFFERMSG_SIZE;

                DEBUG_REPORT(REPORT_MODULE_VXDIO, "[MID=0x%02X] [DECODE] [0x%02X]",
                             psHostMsgStatus->aui8DecodeFenceID[VDECFW_MSGID_BITSTREAM_BUFFER-VDECFW_MSGID_PSR_PADDING],
                             VDECFW_MSGID_BITSTREAM_BUFFER);
            }
        }

        psDecPictSeg = LST_next(psDecPictSeg);
    }

    /* Finally create the Batch message */
    IMG_MEMSET(pui8Msg, 0, V2_DECODEMSG_SIZE);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BATCHMSG_SIZE, V2_BATCHMSG_SIZE);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BATCHMSG_MID, VDECFW_MSGID_BATCH);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BATCHMSG_FLAGS, 0);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BATCHMSG_STREAM_PTD, *((IMG_UINT32*)psBatchMsgData->psStrPtdBufInfo->pvCpuVirt));
    MEMIO_WRITE_FIELD(pui8Msg, V2_BATCHMSG_BATCHADDR, GET_HOST_ADDR(psBatchMsgData->psBatchMsgBufInfo, !psDevConfig->bFakeMtx));
    MEMIO_WRITE_FIELD(pui8Msg, V2_BATCHMSG_BATCHSIZE, V2_DECODEMSG_SIZE + ui32Count*V2_BUFFERMSG_SIZE);

    UPDATE_DEVICE(psBatchMsgData->psBatchMsgBufInfo, IMG_TRUE);

    MEMIO_WRITE_FIELD(pui8Msg, V2_BATCHMSG_FENCE_ID, ui8BatchMessageID);

    *ppui8Msg = pui8Msg;

    return IMG_SUCCESS;

error:
    IMG_FREE(pui8Msg);
    return ui32Result;
}

/*!
******************************************************************************

 @Function              MSVDX_GetFragmentMessage

******************************************************************************/
IMG_RESULT
MSVDX_GetFragmentMessage(
    const IMG_HANDLE         hVxd,
    IMG_UINT8             ** ppui8Msg,
    HWCTRL_sMsgStatus      * psHostMsgStatus,
    VDECDD_sDdBufMapInfo   * psDdBufMapInfo,
    DECODER_sDecPictSeg    * psDecPictSeg
)
{
    IMG_UINT8 * pui8Msg = IMG_MALLOC(V2_BUFFERMSG_SIZE);
    if (pui8Msg == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "Failed to allocate message buffer");
        return IMG_ERROR_MALLOC_FAILED;
    }
    IMG_MEMSET(pui8Msg, 0, V2_BUFFERMSG_SIZE);

    /* Create a message here for bitstream dma buffer for the fragment */
    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_SIZE, V2_BUFFERMSG_SIZE);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_MID, VDECFW_MSGID_BITSTREAM_BUFFER);

    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_DATAADDR,
                      psDdBufMapInfo->sDdBufInfo.ui32DevVirt +
                      psDecPictSeg->psBitStrSeg->ui32DataByteOffset);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_DATASIZE,
                      psDecPictSeg->psBitStrSeg->ui32DataSize);

    IMG_ASSERT(psDecPictSeg->psBitStrSeg->ui32DataSize <= psDdBufMapInfo->sDdBufInfo.ui32BufSize);

    UPDATE_DEVICE((&psDdBufMapInfo->sDdBufInfo), IMG_TRUE);

    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_FENCE_ID,
                      ((++(psHostMsgStatus->aui8DecodeFenceID[VDECFW_MSGID_BITSTREAM_BUFFER-VDECFW_MSGID_PSR_PADDING]))&0xFF));

    *ppui8Msg = pui8Msg;

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              MSVDX_GetEndBytesMessage

******************************************************************************/
IMG_RESULT
MSVDX_GetEndBytesMessage(
    const IMG_HANDLE       hVxd,
    IMG_UINT8           ** ppui8Msg,
    HWCTRL_sMsgStatus    * psHostMsgStatus,
    VXDIO_sDdBufInfo     * psEndBytesBufInfo
)
{
    IMG_UINT8 * pui8Msg = IMG_MALLOC(V2_BUFFERMSG_SIZE);
    if (pui8Msg == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "Failed to allocate message buffer");
        return IMG_ERROR_MALLOC_FAILED;
    }
    IMG_MEMSET(pui8Msg, 0, V2_BUFFERMSG_SIZE);

    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_SIZE, V2_BUFFERMSG_SIZE);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_MID, VDECFW_MSGID_BITSTREAM_BUFFER);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_FLAGS, (1<<VDECFW_MSGFLAG_BUFFER_LAST_SHIFT));

    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_DATAADDR, psEndBytesBufInfo->ui32DevVirt);
    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_DATASIZE, psEndBytesBufInfo->ui32BufSize);

    /* Send the message to the firmware */
    MEMIO_WRITE_FIELD(pui8Msg, V2_BUFFERMSG_FENCE_ID,
                      (((++psHostMsgStatus->aui8DecodeFenceID[VDECFW_MSGID_BITSTREAM_BUFFER-VDECFW_MSGID_PSR_PADDING]))&0xFF));

    *ppui8Msg = pui8Msg;

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MSVDX_ReturnMsg

******************************************************************************/
IMG_VOID MSVDX_ReturnMsg(
    const IMG_HANDLE    hVxd,
    IMG_UINT8         * pui8Msg
)
{
    IMG_FREE(pui8Msg);
}

/*!
******************************************************************************

 @Function              MSVDX_ProcessMessage

******************************************************************************/
IMG_RESULT
MSVDX_ProcessMessage(
    const IMG_HANDLE     hVxd,
    IMG_UINT32         * pui32Msg,
    HWCTRL_sMsgStatus  * psHostMsgStatus,
    LST_T              * psPendPictList,
#ifndef IMG_KERNEL_MODULE
    IMG_BOOL             bPdumpAndRes,
#endif
    IMG_BOOL           * pbDecodedMsg
)
{
    IMG_RESULT                 rResult;
    IMG_UINT32                 ui32MsgId;
    IMG_UINT32                 ui32Tid = 0;
    DECODER_sDecPict *         psDecPict;
    VDEC_sPictDecoded *        psPictDecoded = IMG_NULL;
    VDECFW_sPerformanceData *  psPerformanceData = IMG_NULL;
    VDEC_sPictHwCrc *          psPictHwCrc = IMG_NULL;

    rResult = IMG_SUCCESS;

    ui32Tid = MEMIO_READ_FIELD(pui32Msg, V2_DECODEDMSG_TID);
    psDecPict = LST_first(psPendPictList);
    while(psDecPict)
    {
        if (psDecPict->ui32TransactionId == ui32Tid)
        {
            break;
        }
        psDecPict = LST_next(psDecPict);
    }

    ui32MsgId = MEMIO_READ_FIELD(pui32Msg, V2_DECODEMSG_MID);
#ifdef POST_TEST
    /* if the message is a POST control response */
    if(VDECFW_MSGID_PIC_POST_RESP == ui32MsgId)
    {
        /* check the received control command feedback */
        switch(MEMIO_READ_FIELD(pui32Msg, V2_CONTROLMSG_VALUE0))
        {
        case VDECFW_POST_START_CALC:

            /* check the received response data */
            if( CALC_TEST_RESULT_VALUE == MEMIO_READ_FIELD(pui32Msg, V2_CONTROLMSG_VALUE1) )
            {
                REPORT(REPORT_MODULE_HWCTRL, REPORT_NOTICE,
                    "***** POST: MTX Calculation Test PASSED");
            }
            else
            {
                /* test FAILED */

                REPORT(REPORT_MODULE_HWCTRL, REPORT_ERR,
                    "***** POST: MTX Calculation Test FAILED");

                rResult = IMG_ERROR_FATAL;
            }
            break;

        case VDECFW_POST_GET_INIT_RES:

            /* check received result */
            if(IMG_SUCCESS == MEMIO_READ_FIELD(pui32Msg, V2_CONTROLMSG_VALUE1))
            {
                REPORT(REPORT_MODULE_HWCTRL, REPORT_NOTICE,
                    "***** POST: MTX Memory Test PASSED");
            }
            else
            {
                /* test FAILED */

                REPORT(REPORT_MODULE_HWCTRL, REPORT_NOTICE,
                    "***** POST: MTX Memory Test FAILED");

                rResult = IMG_ERROR_FATAL;
            }
            break;

        default:
            break;
        }
    }
    else
#endif
    {
    // We must have a picture in the list that matches the tid
    if(psDecPict == IMG_NULL)
    {
        IMG_ASSERT(IMG_FALSE);

        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
            "Firmware decoded message [TID: 0x%08X] received with no pending picture", ui32Tid);
            rResult = IMG_ERROR_FATAL;
            goto error;
    }


    switch ( ui32MsgId )
    {
    case VDECFW_MSGID_PIC_DECODED:
        *pbDecodedMsg = IMG_TRUE;

        if(psDecPict->psFirstFieldFwMsg->sPictDecoded.bFirstFieldReceived)
        {
            psPictDecoded = &psDecPict->psSecondFieldFwMsg->sPictDecoded;
        }
        else
        {
            psPictDecoded = &psDecPict->psFirstFieldFwMsg->sPictDecoded;
            psDecPict->psFirstFieldFwMsg->sPictDecoded.bFirstFieldReceived = IMG_TRUE;
        }

        psPictDecoded->ui32FEError = MEMIO_READ_FIELD(pui32Msg, V2_DECODEDMSG_FE_ERR);
        psPictDecoded->ui32NoBEDWT = MEMIO_READ_FIELD(pui32Msg, V2_DECODEDMSG_BEWDT);

        //if(FLAG_IS_SET(MEMIO_READ_FIELD(pui32Msg,V2_DECODEDMSG_FLAGS),VDECFW_MSGFLAG_DECODED_SKIP_PICTURE))
        //{
        //    psPictDecoded->bSkipPict = IMG_TRUE;
        //    psPictDecoded->ui32TransIdToFillSkipPict = MEMIO_READ_FIELD(pui32Msg, V2_DECODEDMSG_FILLPICID);
        //}

        psHostMsgStatus->aui8CompletionFenceID[VDECFW_MSGID_PIC_DECODED-VDECFW_MSGID_BE_PADDING] = MEMIO_READ_FIELD(pui32Msg, V2_DECODEDMSG_FENCE_ID);
        DEBUG_REPORT(REPORT_MODULE_VXDIO, "[MID=0x%02X] [COMPLETION_DECODED]", MEMIO_READ_FIELD(pui32Msg, V2_DECODEDMSG_FENCE_ID));

#ifndef IMG_KERNEL_MODULE
        if(bPdumpAndRes)
        {
            if(psDecPict->psAltPict != IMG_NULL)
            {
                UPDATE_HOST((&psDecPict->psAltPict->psPictBuf->sDdBufInfo), IMG_TRUE);
            }
            else
            {
                UPDATE_HOST((&psDecPict->psReconPict->psPictBuf->sDdBufInfo), IMG_TRUE);
            }
        }
#endif
        break;

    case VDECFW_MSGID_PIC_CRCS:

        if(psDecPict->psFirstFieldFwMsg->sPictHwCrc.bFirstFieldReceived)
        {
            psPictHwCrc = &psDecPict->psSecondFieldFwMsg->sPictHwCrc;
        }
        else
        {
            psPictHwCrc = &psDecPict->psFirstFieldFwMsg->sPictHwCrc;
            psDecPict->psFirstFieldFwMsg->sPictHwCrc.bFirstFieldReceived = IMG_TRUE;
        }

        // Fill up the CRC data
        psPictHwCrc->ui32CrcShiftRegFE        = /*MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_SR_FE)*/1;
        psPictHwCrc->ui32CrcVecCommand        = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VEC_CMD);
        psPictHwCrc->ui32CrcVecIxform         = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VEC_IXFORM);
        psPictHwCrc->ui32CrcVdmcPixRecon      = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VDMC_PIXRECON);
        psPictHwCrc->ui32VdebScaleWriteData   = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VDEB_SCALE_WDATA);
        psPictHwCrc->ui32VdebScaleAddr        = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VDEB_BURSTB_ADDR);
        psPictHwCrc->ui32VdebBBWriteData      = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VDEB_BURSTB_WDATA);
        psPictHwCrc->ui32VdebBBAddr           = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VDEB_BURSTB_ADDR);
        psPictHwCrc->ui32VdebSysMemAddr       = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VDEB_SYSMEM_ADDR);
        psPictHwCrc->ui32VdebSysMemWriteData  = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_CRC_VDEB_SYSMEM_WDATA);

        psHostMsgStatus->aui8CompletionFenceID[VDECFW_MSGID_PIC_CRCS-VDECFW_MSGID_BE_PADDING] = MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_FENCE_ID);
        DEBUG_REPORT(REPORT_MODULE_VXDIO, "[MID=0x%02X] [COMPLETION_CRC]", MEMIO_READ_FIELD(pui32Msg, V2_CRCMSG_FENCE_ID));
        break;

    case VDECFW_MSGID_PIC_PERFORMANCE:

        if(psDecPict->psFirstFieldFwMsg->sPerformanceData.bFirstFieldReceived)
        {
            psPerformanceData = &psDecPict->psSecondFieldFwMsg->sPerformanceData;
        }
        else
        {
            psPerformanceData = &psDecPict->psFirstFieldFwMsg->sPerformanceData;
            psDecPict->psFirstFieldFwMsg->sPerformanceData.bFirstFieldReceived = IMG_TRUE;
        }

        // Fill up the performance data
        //psPerformanceData->ui16HeightMBS          = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_WIDTH_MBS);
        //psPerformanceData->ui16HeightMBS          = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_HEIGHT_MBS);
        psPerformanceData->ui16NumSlices          = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_NUM_SLICES);
        //psPerformanceData->ui161SliceSize         = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_1SLICE_SIZE);
        psPerformanceData->ui32PictureStarted     = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_PICTURE_STARTED);
        psPerformanceData->ui32FirmwareReady      = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_FIRMWARE_READY);
        psPerformanceData->ui32PicmanComplete     = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_PICMAN_COMPLETE);
        psPerformanceData->ui32FirmwareSaved      = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_FIRMWARE_SAVED);
        psPerformanceData->ui32EntdecStarted      = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_ENTDEC_STARTED);
        psPerformanceData->ui32Fe1sliceDone       = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_FE_1SLICE_DONE);
        psPerformanceData->ui32FePictureComplete  = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_FE_PICTURE_COMPLETE);
        psPerformanceData->ui32BePictureStarted   = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_BE_PICTURE_STARTED);
        psPerformanceData->ui32Be1sliceDone       = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_BE_1SLICE_DONE);
        psPerformanceData->ui32BePictureComplete  = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_BE_PICTURE_COMPLETE);
        psPerformanceData->ui32SyncStart          = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_SYNC_START);
        psPerformanceData->ui32SyncComplete       = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_SYNC_COMPLETE);

        psPerformanceData->ui32TotParseSliceHeader = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_T1);
        psPerformanceData->ui32TotSetupRegisters   = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_T2);
        psPerformanceData->ui32TotVLC              = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_T4);
        psPerformanceData->ui32TotParserLoad       = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_T3);
        psPerformanceData->ui32TotParseAndSetupReg = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_PARSE_AND_SETUP_REG);
        psPerformanceData->ui32TotHwFEDecode       = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_TOT_HW_FE_DECODE);
        psPerformanceData->ui32TotHwBEDecode       = MEMIO_READ_FIELD(pui32Msg, V2_PERFMSG_TOT_HW_BE_DECODE);

        psHostMsgStatus->aui8CompletionFenceID[VDECFW_MSGID_PIC_PERFORMANCE-VDECFW_MSGID_BE_PADDING] = MEMIO_READ_FIELD(pui32Msg, V2_DECODEDMSG_FENCE_ID);
        DEBUG_REPORT(REPORT_MODULE_VXDIO, "[MID=0x%02X] [COMPLETION_PERFORMANCE]", MEMIO_READ_FIELD(pui32Msg, V2_DECODEDMSG_FENCE_ID));
        break;

    case VDECFW_MSGID_BE_PADDING:
        psHostMsgStatus->aui8CompletionFenceID[VDECFW_MSGID_BE_PADDING-VDECFW_MSGID_BE_PADDING] = MEMIO_READ_FIELD(pui32Msg, V2_PADMSG_FENCE_ID);
        DEBUG_REPORT(REPORT_MODULE_VXDIO, "[MID=0x%02X] [COMPLETION_BE_PADDING]", MEMIO_READ_FIELD(pui32Msg, V2_PADMSG_FENCE_ID));
        break;

    default:
        IMG_ASSERT(IMG_FALSE);
        break;
    }
    }
error:
    return rResult;
}

