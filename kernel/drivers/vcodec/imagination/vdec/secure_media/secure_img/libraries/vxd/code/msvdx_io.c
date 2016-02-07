/*!
 *****************************************************************************
 *
 * @File       msvdx_io.c
 * @Title      Low-level MSVDX secure hardware access component
 * @Description    This file contains functions to communicate with an MSVDX core.
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

#include "img_include.h"
#include "img_defs.h"

#include "lst.h"
#include "tal.h"

#ifdef SECURE_MEDIA_REPORTING
#include "report_api.h"
#else
#define REPORT(MODULE, LEVEL, fmt, ...)
#define DEBUG_REPORT(MODULE, fmt, ...)
#endif

#include "msvdx_ext.h"
#include "mtxio.h"
#include "vxd_io.h"
#include "msvdx_io.h"
#include "vxd.h"
#include "vdecfw.h"
#include "target.h"
#include "dma_ll.h"

#include "reg_io2.h"
#include "msvdx_mtx_reg_io2.h"
#include "msvdx_vec_reg_io2.h"
#include "msvdx_vdmc_reg_io2.h"

#ifdef SECURE_CPU_REG_ACCESS
static IMG_BOOL bSecEnable = IMG_FALSE;
#endif

#define DMA_ReadRegMacro(_pMemSpceCtx, channel, reg)          REGIO_READ_TABLE_REGISTER(_pMemSpceCtx, DMAC, reg, channel)
#define DMA_WriteRegMacro(_pMemSpceCtx, channel, reg, val)    REGIO_WRITE_TABLE_REGISTER(_pMemSpceCtx, DMAC, reg, channel, val)

#undef MSVDX_CORE_WriteRegister
#undef MSVDX_CORE_ReadRegister

#undef MSVDX_VEC_WriteRegister
#undef MSVDX_VEC_ReadRegister

#undef DMAC_WriteRegister
#undef DMAC_ReadRegister

#undef MSVDX_VDMC_WriteRegister
#undef MSVDX_VDMC_ReadRegister

#undef MSVDX_VDEB_WriteRegister
#undef MSVDX_VDEB_ReadRegister

#undef MSVDX_VLR_WriteRegister
#undef MSVDX_VLR_ReadRegister

#undef MTX_CORE_WriteRegister
#undef MTX_CORE_ReadRegister

#undef MSVDX_TEST_WriteRegister
#undef MSVDX_TEST_ReadRegister

#undef MSVDX_TEST_3_WriteRegister
#undef MSVDX_TEST_3_ReadRegister

#define MSVDX_CORE_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_VXDSYS_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define MSVDX_CORE_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_VXDSYS_REGSPACE, o, MSVDX_DEF_WR_PIPE)

#define MSVDX_VEC_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_VEC_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define MSVDX_VEC_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_VEC_REGSPACE, o, MSVDX_DEF_WR_PIPE)

#define DMAC_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_DMAC_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define DMAC_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_DMAC_REGSPACE, o, MSVDX_DEF_WR_PIPE)

#define MSVDX_VDMC_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_VDMC_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define MSVDX_VDMC_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_VDMC_REGSPACE, o, MSVDX_DEF_WR_PIPE)

#define MSVDX_VDEB_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_VDEB_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define MSVDX_VDEB_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_VDEB_REGSPACE, o, MSVDX_DEF_WR_PIPE)

#define MSVDX_VLR_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_VLR_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define MSVDX_VLR_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_VLR_REGSPACE, o, MSVDX_DEF_WR_PIPE)

#define MTX_CORE_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_VXDMTX_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define MTX_CORE_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_VXDMTX_REGSPACE, o, MSVDX_DEF_WR_PIPE)

#define MSVDX_TEST_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_VXDTEST_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define MSVDX_TEST_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_VXDTEST_REGSPACE, o, MSVDX_DEF_WR_PIPE)

#define MSVDX_TEST_3_WriteRegister(h, o, v) \
        VXDIO_WriteRegister(h, REGION_VXDTEST_3_REGSPACE, o, v, \
                            MSVDX_DEF_WR_MASK, MSVDX_DEF_WR_PIPE)
#define MSVDX_TEST_3_ReadRegister(h, o) \
        VXDIO_ReadRegister(h, REGION_VXDTEST_3_REGSPACE, o, MSVDX_DEF_WR_PIPE)

//Enable test mode for fpgas that support it.
#ifdef SECURE_DEVICE_FPGA_TEST

#include "msvdx_test_3_regs.h"

#define SECURE_CPU_REG_ACCESS
#define SECURE_MEM_ACCESS

#define SECURE_REG_ACCESS \
    ((bSecEnable << MSVDX_TEST_3_SECURE_CPU_CTRL_0_SECURE_CPU_ENABLE_SHIFT) |\
    (1 << MSVDX_TEST_3_SECURE_CPU_CTRL_0_SECURE_CPU_TRANSACTION_SHIFT))

#endif


typedef struct
{
    IMG_UINT32 ui32Addr;          /* Byte offset into MTX memory (starts at 0). */
    IMG_UINT32 ui32Size;          /* Number of 32-bit words to transfer. */
    IMG_UINT32 *pui32Data;        /* Host data buffer. */

} MSVDXIO_DmacBufInfo;


typedef struct
{
    VXDIO_sContext        * psCommonCtx;                          /*!< Handle to common video device context. */

    MSVDXIO_sMTXSwInfo      sMTXSwInfo;                           /*!< Firmware parts.                        */
    MSVDXIO_sFw             sFw;                                  /*!< Buffers dedicated for firmware.        */
    MTXIO_sContext          sMsgContext;                          /*!< MTXIO specific data.                   */

    // Data for prepared firmware, ready to load.
    MSVDXIO_DmacBufInfo     asBufsInfo[VDEC_DMAC_LL_BUFS_COUNT];  /*!< Chunks used to upload firmware to MTX  */
    IMG_UINT32              ui32CurLLElem;                        /*!< Current upload chunk.                  */

} MSVDXIO_sContext;

static IMG_UINT32 ui32NumOfCoresInit = 0;

#ifdef SEC_USE_REAL_FW
// This table maps the VDEC video standard to codec parser module.
// This must be kept in step with VDEC_eVidStd.
static VDECFW_eCodecType aeStdToCodec[] =
{
    VDEC_CODEC_NONE,

    VDECFW_CODEC_MPEG2,
    VDECFW_CODEC_MPEG4,
    VDECFW_CODEC_MPEG4,
    VDECFW_CODEC_H264,
    VDECFW_CODEC_VC1,
    VDECFW_CODEC_AVS,
    VDECFW_CODEC_RV,
    VDECFW_CODEC_JPEG,
    VDECFW_CODEC_VP6,
    VDECFW_CODEC_VP8,
    VDECFW_CODEC_MPEG4,

    VDECFW_CODEC_MAX,
};
#endif

#ifdef SEC_USE_REAL_FW

/*!
******************************************************************************

 @Function              msvdxio_WaitForMTX

******************************************************************************/
static IMG_RESULT
msvdxio_WaitForMTX(
    const IMG_HANDLE    hCoreContext
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_RESULT          ui32Result;

    IMG_ASSERT(psContext);
    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }


#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    TAL_PdumpComment(psContext->psCommonCtx->pahMemSpace[REGION_VXDSYS_MEMSPACE], "msvdxio_WaitForMTX");
#endif

    ui32Result = MSVDXIO_Poll(psContext,
                              REGION_VXDSYS_MEMSPACE,
                              MSVDX_CORE_CR_MTX_DEBUG_OFFSET,
                              0,
                              MSVDX_CORE_CR_MTX_DEBUG_CR_MTX_DBG_GPIO_OUT_MASK,
                              VXDIO_POLL_EQUAL);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Timeout waiting for MTX.");
        goto error;
    }

error:
    return ui32Result;

}


/*!
******************************************************************************

 @Function              msvdxio_MTXWriteWords

******************************************************************************/
static IMG_RESULT
ui32GetDataChecksum(
    MSVDXIO_sContext  * psContext,
    IMG_UINT32          ui32Addr,
    IMG_UINT32          ui32Size,
    IMG_UINT32          ui32MTXBankSize,
    IMG_UINT32        * pui32DataChecksum
)
{
    IMG_UINT32 ui32CalcChecksum = 0;
    IMG_UINT32 ui32Reg;
    IMG_UINT32 ui32RamId;
    IMG_UINT32 ui32MtxTimeOut;
    IMG_RESULT ui32Result = IMG_SUCCESS;

    /* Initiate read access to RAM block...*/
    ui32RamId = MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize);
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));

    if (ui32Size > 1)
    {
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
    }

    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMR, 1);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);

    /* Repeat for all words...*/
    while ((ui32Size > 0)
       &&  (IMG_ERROR_TIMEOUT != ui32Result))
    {
        /* Wait for read to complete...*/
        ui32MtxTimeOut = MSVDXIO_TIMEOUT_COUNTER;

        do
        {
            /* Check MTX is OK...*/
            ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_FAULT0);
            if (ui32Reg != 0)
            {
                ui32Result = IMG_ERROR_TIMEOUT;
            }

            ui32MtxTimeOut--;
            ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS);
        }
        while ((REGIO_READ_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS, MTX_MTX_MCM_STAT) == 0)
           &&  (ui32MtxTimeOut != 0)
           &&  (IMG_ERROR_TIMEOUT != ui32Result));

        /* go ahead if all ok */
        if(IMG_ERROR_TIMEOUT != ui32Result)
        {

            IMG_ASSERT(ui32MtxTimeOut != 0);

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
            TAL_PdumpComment(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_REGSPACE], "MTX LR Comms Poll - Read.");
            TAL_Poll(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_REGSPACE],
                     MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_OFFSET,
                     TAL_CHECKFUNC_ISEQUAL,
                     MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                     MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                     1000000, 100);
#endif

            /* calculate checksum */
            ui32CalcChecksum += REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_DATA_TRANSFER);

            /* decrement remaining bytes */
            ui32Size--;

            /* If we have move from one RAM to the next...*/
            ui32Addr += 4;

            if ((MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize)) != ui32RamId)
            {
                /* Initiate read access to RAM block...*/
                ui32RamId = (MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize));
                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));

                if (ui32Size > 1)
                {
                    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
                }

                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMR, 1);
                REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);
            }
        }
    }

    /* copy calculated value to pointed data */
    *pui32DataChecksum = ui32CalcChecksum;

    return ui32Result;
}


/*!
******************************************************************************

 @Function              msvdxio_MTXWriteWords

******************************************************************************/
static IMG_RESULT
msvdxio_MTXWriteWords(
    const IMG_HANDLE    hCoreContext,
    IMG_UINT32          ui32Addr,         // Byte offset into MTX memory (starts at 0).
    IMG_UINT32          ui32Size,         // Number of 32-bit words to write.
    IMG_UINT32          ui32MTXBankSize,
    const IMG_UINT32  * pui32Values,
    IMG_BOOL            bDataCheckReq
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_RESULT          ui32Result = IMG_SUCCESS;
    IMG_INT32           ui32Reg;
    IMG_UINT32          ui32MtxTimeOut;
    IMG_UINT32          ui32RamId;
    IMG_UINT32          ui32AccessControl;

    IMG_UINT32            ui32WrDataChecksum = 0;
    IMG_UINT32            ui32DataChecksum = 0;
    IMG_UINT32          ui32SizeCopy;
    IMG_UINT32          ui32AddrCopy;
    ui32SizeCopy = ui32Size;
    ui32AddrCopy = ui32Addr;
    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if ((ui32Addr & 0x3) != 0 ||
        ui32Size == 0)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A valid MTX RAM location (offset word aligned) must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if (pui32Values == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A valid container for write words not provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    /* Request the bus from the Dash...*/
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_IS_SLAVE, 1);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_GPIO_IN, 0x02);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);
    msvdxio_WaitForMTX(psContext);

    /* Set HOST_ACCESS so Codescape knows the host is using the bus. */
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_GPIO_IN, 0x03);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);

    /* Save the access control register...*/
    ui32AccessControl = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL);

    /* Initiate write access to RAM block...*/
    ui32RamId = MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize);
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));

    if (ui32Size > 1)
    {
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
    }

    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);

    /* Repeat for all words...*/
    while (ui32Size > 0)
    {
        ui32WrDataChecksum += (*pui32Values);
        /* Write the next word...*/
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_DATA_TRANSFER, *pui32Values);
        pui32Values++;

        /* Wait for write to complete...*/
        ui32MtxTimeOut = MSVDXIO_TIMEOUT_COUNTER;
        do
        {
             /* Check MTX is OK...*/
            ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_FAULT0);

            if (ui32Reg != 0)
            {
                ui32Result = IMG_ERROR_TIMEOUT;
                goto ErrorExit;
            }

            ui32MtxTimeOut--;
            ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS);
        }
        while ((REGIO_READ_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS, MTX_MTX_MCM_STAT) == 0) &&
               (ui32MtxTimeOut != 0));

        IMG_ASSERT(ui32MtxTimeOut != 0);

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
        TAL_PdumpComment(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_REGSPACE], "MTX LR Comms Poll - Write.");
        TAL_Poll(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_REGSPACE],
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_OFFSET,
                 TAL_CHECKFUNC_ISEQUAL,
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                 1000000, 100);
#endif

        ui32Size--;

        /* If we have move from one RAM to the next...*/
        ui32Addr += 4;
        if ((MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize)) != ui32RamId)
        {
            /* Initiate write access to RAM block...*/
            ui32RamId = (MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize));
            ui32Reg = 0;
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));
            if (ui32Size > 1)
            {
                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
            }
            REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);
        }
    }

    if(IMG_TRUE == bDataCheckReq)
    {
        ui32Result = ui32GetDataChecksum(psContext,
                                         ui32AddrCopy,
                                         ui32SizeCopy,
                                         ui32MTXBankSize,
                                         &ui32DataChecksum);
        if(ui32WrDataChecksum != ui32DataChecksum)
        {
            ui32Result = IMG_ERROR_TEST_MISMATCH;
        }
        else
        {
        }
    }
    else
    {
    }
ErrorExit:
    /* Restore the access control register...*/
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32AccessControl);

    /* Release the bus...*/
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_IS_SLAVE, 1);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);

    /* Ensure the MTX is OK....*/
    if (!psContext->psCommonCtx->bFakeMtx)
    {
        IMG_ASSERT(REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_FAULT0) == 0);
    }

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              msvdxio_MTXReadWords

******************************************************************************/
static IMG_RESULT
msvdxio_MTXReadWords(
    const IMG_HANDLE    hCoreContext,
    IMG_UINT32          ui32Addr,         // Byte address.
    IMG_UINT32          ui32Size,         // Number of 32-bit words to read.
    IMG_UINT32          ui32MTXBankSize,
    IMG_UINT32 *        pui32Values
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_RESULT          ui32Result = IMG_SUCCESS;
    IMG_UINT32          ui32Reg;
    IMG_UINT32          ui32MtxTimeOut;
    IMG_UINT32          ui32RamId;
    IMG_UINT32          ui32AccessControl;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if ((ui32Addr & 0x3) != 0 ||
        ui32Size == 0)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A valid MTX RAM location (offset word aligned) must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if (pui32Values == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A valid container for read words not provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    /* Request the bus from the Dash...*/
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_IS_SLAVE, 1);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_GPIO_IN, 0x02);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);
    msvdxio_WaitForMTX(psContext);

    /* Set HOST_ACCESS so Codescape knows the host is using the bus. */
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_GPIO_IN, 0x03);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);

    /* Save the access control register...*/
    ui32AccessControl = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL);

    /* Initiate read access to RAM block...*/
    ui32RamId = MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize);
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));

    if (ui32Size > 1)
    {
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
    }

    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMR, 1);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);

    /* Repeat for all words...*/
    while (ui32Size > 0)
    {
        /* Wait for read to complete...*/
        ui32MtxTimeOut = MSVDXIO_TIMEOUT_COUNTER;

        do
        {
            /* Check MTX is OK...*/
            ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_FAULT0);
            if (ui32Reg != 0)
            {
                ui32Result = IMG_ERROR_TIMEOUT;
                goto ErrorExit;
            }

            ui32MtxTimeOut--;
            ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS);
        }
        while ((REGIO_READ_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS, MTX_MTX_MCM_STAT) == 0) &&
               (ui32MtxTimeOut != 0));

        IMG_ASSERT(ui32MtxTimeOut != 0);

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
        TAL_PdumpComment(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_REGSPACE], "MTX LR Comms Poll - Read.");
        TAL_Poll(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_REGSPACE],
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_OFFSET,
                 TAL_CHECKFUNC_ISEQUAL,
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                 1000000, 100);
#endif

        *pui32Values = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_DATA_TRANSFER);

        pui32Values++;
        ui32Size--;

        /* If we have move from one RAM to the next...*/
        ui32Addr += 4;

        if ((MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize)) != ui32RamId)
        {
            /* Initiate read access to RAM block...*/
            ui32RamId = (MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize));
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));

            if (ui32Size > 1)
            {
                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
            }

            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMR, 1);
            REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);
        }
    }

ErrorExit:
    /* Restore the access control register...*/
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32AccessControl);

    /* Release the bus...*/
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_IS_SLAVE, 1);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);

    /* Ensure the MTX is OK....*/
    if (!psContext->psCommonCtx->bFakeMtx)
    {
        IMG_ASSERT(REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_FAULT0) == 0);
    }

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              msvdxio_MTXGetRamInfo

 @Description

 This is a non-destructive test (if the bank size can be determined); however
 the MTX must not be running.

******************************************************************************/
static IMG_RESULT
msvdxio_MTXGetRamInfo(
    const IMG_HANDLE        hCoreContext,
    VXDIO_sMTXRamInfo     * psMTXRamInfo
)
{
    IMG_UINT32 ui32RamInfo, ui32RamBanks;
    MSVDXIO_sContext * psContext = (MSVDXIO_sContext *)hCoreContext;

    /* Set default RAM config. */
    psMTXRamInfo->ui32MTXRamSize  = 0;       /* measured total MTX memory size.                                   */
    psMTXRamInfo->ui32MTXBankSize = 0x8000;  /* measured MTX RAM bank size (initial safe value for FPGA: 32768).  */
    psMTXRamInfo->ui32MTXRamMask  = 0;       /* mask of valid memory physical address bits.                       */

    /* Try and read the CR_MTX_DEBUG register as if it exists it will tell us everything. */
    ui32RamInfo = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG);
    ui32RamBanks = REGIO_READ_FIELD(ui32RamInfo, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_RAM_BANKS);

    /* Only derive information from this register if banks > 0 (ie. if the register exists). */
    if (ui32RamBanks > 0)
    {
        IMG_UINT32 ui32LastBankSize;

        psMTXRamInfo->ui32MTXBankSize = 1 << (REGIO_READ_FIELD(ui32RamInfo, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_RAM_BANK_SIZE) + 2);
        ui32LastBankSize = 1 << (REGIO_READ_FIELD(ui32RamInfo, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_LAST_RAM_BANK_SIZE) + 2);
        psMTXRamInfo->ui32MTXRamSize = (psMTXRamInfo->ui32MTXBankSize * (ui32RamBanks - 1)) + ui32LastBankSize;
        psMTXRamInfo->ui32MTXRamMask = psMTXRamInfo->ui32MTXRamSize - 1;
        psMTXRamInfo->ui32MTXRamMask |= psMTXRamInfo->ui32MTXRamMask >> 1;
        psMTXRamInfo->ui32MTXRamMask |= psMTXRamInfo->ui32MTXRamMask >> 2;
        psMTXRamInfo->ui32MTXRamMask |= psMTXRamInfo->ui32MTXRamMask >> 4;
        psMTXRamInfo->ui32MTXRamMask |= psMTXRamInfo->ui32MTXRamMask >> 8;
        psMTXRamInfo->ui32MTXRamMask |= psMTXRamInfo->ui32MTXRamMask >> 16;
    }
    else  /* No register - probe the memory to find the bank size. */
    {
        IMG_UINT32 ui32ThisSize, ui32NextSize;
        IMG_UINT32 ui32SaveLo, ui32SaveHi;
        IMG_UINT32 ui32Detect, ui32Temp, ui32MaxRamSize;
        IMG_UINT32 ui32ValueLo = 0x12345678;
        IMG_UINT32 ui32ValueHi = 0xaa55aa55;

        /* Set the bank size large so the read and write functions always use bank 0. */
        psMTXRamInfo->ui32MTXBankSize = MSVDXIO_MTX_MAX_BANK_SIZE << 1;

        /* Save the initial value replace it with test word 1. */
        msvdxio_MTXReadWords(hCoreContext,
                             MSVDXIO_MTX_MIN_BANK_SIZE - 4,
                             1,
                             psMTXRamInfo->ui32MTXBankSize,
                             &ui32SaveLo);
        msvdxio_MTXWriteWords(hCoreContext,
                              MSVDXIO_MTX_MIN_BANK_SIZE - 4,
                              1,
                              psMTXRamInfo->ui32MTXBankSize,
                              &ui32ValueLo,
                              IMG_FALSE);

        for (ui32ThisSize = MSVDXIO_MTX_MIN_BANK_SIZE;
             ui32ThisSize <= MSVDXIO_MTX_MAX_BANK_SIZE;
             ui32ThisSize = ui32NextSize)
        {
            ui32NextSize = ui32ThisSize << 1;

            /* Save the next word and replace it with test word 2. */
            msvdxio_MTXReadWords(hCoreContext,
                                 ui32NextSize - 4,
                                 1,
                                 psMTXRamInfo->ui32MTXBankSize,
                                 &ui32SaveHi);
            msvdxio_MTXWriteWords(hCoreContext,
                                  ui32NextSize - 4,
                                  1,
                                  psMTXRamInfo->ui32MTXBankSize,
                                  &ui32ValueHi,
                                  IMG_FALSE);

            /* Read the word from the end of the current bank sized bank. */
            msvdxio_MTXReadWords(hCoreContext,
                                 ui32ThisSize - 4,
                                 1,
                                 psMTXRamInfo->ui32MTXBankSize,
                                 &ui32Detect);

            /* Replace the lower word. */
            msvdxio_MTXWriteWords(hCoreContext,
                                  ui32ThisSize - 4,
                                  1,
                                  psMTXRamInfo->ui32MTXBankSize,
                                  &ui32SaveLo,
                                  IMG_FALSE);

            if (ui32Detect == ui32ValueHi)
            {
                psMTXRamInfo->ui32MTXBankSize = ui32ThisSize;
                break;
            }

            /* Swap the stored information for the next iteration. */
            ui32SaveLo = ui32SaveHi;  /*  We have written back ui32SaveLo already. */
            ui32Temp = ui32ValueLo;
            ui32ValueLo = ui32ValueHi;
            ui32ValueHi = ui32Temp;
        }

        // Fail if the bank size has not been determined.
        IMG_ASSERT(psMTXRamInfo->ui32MTXBankSize <= MSVDXIO_MTX_MAX_BANK_SIZE);

        ui32MaxRamSize = psMTXRamInfo->ui32MTXBankSize * 4;
        psMTXRamInfo->ui32MTXRamMask = ui32MaxRamSize - 1;

        /* Assume all banks are fully populated. */
        psMTXRamInfo->ui32MTXRamSize = ui32MaxRamSize;
    }

    return IMG_SUCCESS;
}


#ifdef USE_MTX_DMA_LOAD

/*!
******************************************************************************

 @Function              msvdxio_startDMA

 @Description

 Configures DMA and starts linked list transfer.

******************************************************************************/
static IMG_VOID
msvdxio_startDMA(
    const IMG_HANDLE    hCoreContext,
    IMG_UINT32          ui32TransferSize,
    IMG_UINT32          ui32LLDevVirtAddr
)
{
    IMG_UINT32 ui32Reg = 0;
    /* DMA channel used for transfer */
    IMG_UINT32 ui32Channel = 0;

#ifdef SECURE_MEM_ACCESS
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MEM_PROT_CTRL_2, CR_MEM_PROT_ID1_CTRL, 1);    //DMAC MEM_TAG
    REGIO_WRITE_REGISTER(hCoreContext, MSVDX_CORE, CR_MEM_PROT_CTRL_2, ui32Reg);
#endif

    /* Before starting the transfer, set the flow control */
    ui32Reg = REGIO_READ_REGISTER(hCoreContext, MSVDX_CORE, CR_MSVDX_CONTROL);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, DMAC_CH0_SELECT, 0);
    REGIO_WRITE_REGISTER(hCoreContext, MSVDX_CORE, CR_MSVDX_CONTROL, ui32Reg);

    ui32Reg = 0;
    /* Begin transfer. */
    REGIO_WRITE_FIELD_LITE(ui32Reg, MTX_CORE, CR_MTX_SYSC_CDMAC, ENABLE, 1);
    /* Size in 32-bit words. */
    REGIO_WRITE_FIELD_LITE(ui32Reg, MTX_CORE, CR_MTX_SYSC_CDMAC, LENGTH, ui32TransferSize);
    REGIO_WRITE_REGISTER(hCoreContext, MTX_CORE, CR_MTX_SYSC_CDMAC, ui32Reg);

    /* Set mtx core DMA start address. */
    REGIO_WRITE_REGISTER(hCoreContext, MTX_CORE, CR_MTX_SYSC_CDMAA, 0);

    DMA_WriteRegMacro(hCoreContext, ui32Channel, DMAC_IRQ_STAT, 0);

    /* Specify the holdover period to use for the channel */
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, DMAC, DMAC_PER_HOLD, PER_HOLD, 16);
    DMA_WriteRegMacro(hCoreContext, ui32Channel, DMAC_PER_HOLD, ui32Reg);

    /* Start address of the system memory from where to read the linked list */
    DMA_WriteRegMacro(hCoreContext, ui32Channel, DMAC_SETUP, ui32LLDevVirtAddr);

    /* Clear linked list status and peripheral register address registers */
    DMA_WriteRegMacro(hCoreContext, ui32Channel, DMAC_IRQ_STAT, 0);
    DMA_WriteRegMacro(hCoreContext, ui32Channel, DMAC_PERIPH, 0);

    /* Set LIST_FINCTRL to signal when entire linked list transfer
       has completed not just last one started. */
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, DMAC, DMAC_COUNT, LIST_FIN_CTL, 1);
    DMA_WriteRegMacro(hCoreContext, ui32Channel, DMAC_COUNT, ui32Reg);

    /* Enable linked list transfer */
    ui32Reg = DMA_ReadRegMacro(hCoreContext, ui32Channel, DMAC_COUNT);
    REGIO_WRITE_FIELD(ui32Reg, DMAC, DMAC_COUNT, LIST_EN, 1);
    DMA_WriteRegMacro(hCoreContext, ui32Channel, DMAC_COUNT, ui32Reg);
}


#ifdef POST_TEST
/*!
******************************************************************************

 @Function              checkDMAIsRunning

 @Description

 Function to check DMA is running.

******************************************************************************/
IMG_RESULT checkDMAIsRunning(
    const IMG_HANDLE    hCoreContext
)
{
    IMG_RESULT ui32Result;
    IMG_UINT32 ui32Reg;
    IMG_UINT16 ui16CntValue = 0;
    IMG_UINT16 ui16CntValueOld = 0;
    IMG_UINT32 ui32TimeOut = 1000;
    /* DMA channel used for transfer */
    IMG_UINT32 ui32Channel = 0;

    /* check if number of DMA transfers remaining is decreasing until timeout*/
    do
    {
        /* update old value */
        ui16CntValueOld = ui16CntValue;

        /* read DMAC_COUNT register */
        ui32Reg = REGIO_READ_TABLE_REGISTER(hCoreContext, DMAC, DMAC_COUNT, ui32Channel);

        /* get CNT value */
        ui16CntValue = REGIO_READ_FIELD(ui32Reg, DMAC, DMAC_COUNT, CNT);

        /* decrement timeout */
        ui32TimeOut--;
    }
    while((ui16CntValue >= ui16CntValueOld)
       && (ui32TimeOut > 0));

    /* check */
    if(0 == ui32TimeOut)
    {
        /* test FAILED */
        ui32Result = IMG_ERROR_FATAL;
    }
    else
    {
        /* test PASSED */
        ui32Result = IMG_SUCCESS;
    }

    return ui32Result;
}
#endif


/*!
******************************************************************************

 @Function              msvdxio_DMAMTX

 @Description

 Prepares DMAC linked list from given buffers and adds linked list elements
 which enables MTX to the end. Calls function to configure DMA and
 tigger transfer.

******************************************************************************/
static IMG_RESULT
msvdxio_DMAMTX(
    MSVDXIO_sContext      * psContext,
    IMG_UINT32              ui32MTXBankSize,
    IMG_UINT32              ui32NumElems,
    MSVDXIO_DmacBufInfo     asBufsInfo[],
    IMG_UINT32              ui32MtxStartAddr,
    VXDIO_sDdBufInfo      * psDmaLLBufInfo,
    VXDIO_sDdBufInfo      * psFwBaseBufInfo
)
{
    IMG_UINT32 ui32Elem;
    DMAC_LinkElement *psLinkElement;
    MSVDXIO_DmacBufInfo *psBufInfo;
    /* Host address of device buffer */
    IMG_UINT8 * pui8BufCpuAddr;
    /* Virtual address of device buffer as seen by MSVDX */
    IMG_UINT32 ui32BufDevAddr;
    IMG_UINT32 ui32TransferSize = 0;

    /* Check if buffers are continous */
    for (ui32Elem = 0; ui32Elem < ui32NumElems - 1; ui32Elem++)
    {
        if (asBufsInfo[ui32Elem].ui32Addr + asBufsInfo[ui32Elem].ui32Size*sizeof(*asBufsInfo[ui32Elem].pui32Data) != asBufsInfo[ui32Elem + 1].ui32Addr)
        {
            REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Blocks are not continous (block %d)!", ui32Elem);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
    }
    /* Calculate total count of words to transfer */
    for (ui32Elem = 0; ui32Elem < ui32NumElems; ui32Elem++)
    {
        ui32TransferSize += asBufsInfo[ui32Elem].ui32Size;
    }
    IMG_ASSERT(psDmaLLBufInfo->ui32BufSize >= (sizeof(DMAC_LinkElement)*ui32NumElems));
    IMG_ASSERT(psFwBaseBufInfo->ui32BufSize >= ui32TransferSize);

    pui8BufCpuAddr = (IMG_UINT8 *)psFwBaseBufInfo->pvCpuVirt;
    ui32BufDevAddr = psFwBaseBufInfo->ui32DevVirt;

    /* Loop through data blocks and add each element to DMA linked list */
    for (ui32Elem = 0; ui32Elem < ui32NumElems; ui32Elem++)
    {
        psBufInfo = &asBufsInfo[ui32Elem];

        /* Check data is 32-bit aligned. */
        IMG_ASSERT(!((IMG_UINTPTR)psBufInfo->pui32Data & 3));

        //printk("Element[%d/%d]: Words[%d] Data[%d/%d]\n", ui32Elem+1, ui32NumElems, psBufInfo->ui32Size, psBufInfo->ui32Size * 4, ui32TransferSize * 4);

        /* Copy whole buffer (with last word of last block!) to device buffer */
        IMG_MEMCPY(pui8BufCpuAddr, psBufInfo->pui32Data, psBufInfo->ui32Size * sizeof(*psBufInfo->pui32Data));

        /* Get host address of linked list element in device linked list buffer */
        psLinkElement = &((DMAC_LinkElement *)psDmaLLBufInfo->pvCpuVirt)[ui32Elem];
        IMG_MEMSET(psLinkElement, 0, sizeof (DMAC_LinkElement));

        /* Configure linked list element */
        DMA_LL_SET_WD0(psLinkElement->ui32DMAWord_0, DMAC_BSWAP_NO_SWAP, DMAC_DIR_MEM_TO_PERI, DMAC_PWIDTH_32_BIT);
        DMA_LL_SET_WD1(psLinkElement->ui32DMAWord_1, (ui32Elem < ui32NumElems -1) ? DMAC_LIST_FIN_OFF : DMAC_LIST_FIN_ON, DMAC_PI_4, DMAC_INCR_OFF, psBufInfo->ui32Size);
        DMA_LL_SET_WD2(psLinkElement->ui32DMAWord_2, MTX_CORE_CR_MTX_SYSC_CDMAT_OFFSET);
        DMA_LL_SET_WD3(psLinkElement->ui32DMAWord_3, DMAC_ACC_DEL_0, DMAC_BURST_1, DMAC_EXT_BURST_0, 0);
        DMA_LL_SET_WD4(psLinkElement->ui32DMAWord_4, DMAC_2DMODE_DISABLE, 0);
        /* Device virtual address of data to be transfered in this element in device buffer */
        DMA_LL_SET_WD6(psLinkElement->ui32DMAWord_6, ui32BufDevAddr);
        if (ui32Elem < ui32NumElems -1)
        {
            /* Next linked list element, device virtual address */
            DMA_LL_SET_WD7(psLinkElement->ui32DMAWord_7, (IMG_UINTPTR)(&((DMAC_LinkElement *)((IMG_UINTPTR)psDmaLLBufInfo->ui32DevVirt))[ui32Elem + 1]));
        }

        /* Increase host and device virtual addresses, so next linked list elements gets copied properly */
        pui8BufCpuAddr += psBufInfo->ui32Size * sizeof(*psBufInfo->pui32Data);
        ui32BufDevAddr += psBufInfo->ui32Size * sizeof(*psBufInfo->pui32Data);
    }

    /* Copy buffers to device */
    UPDATE_DEVICE(psFwBaseBufInfo, IMG_TRUE);

    /* Copy linked list to device */
    UPDATE_DEVICE(psDmaLLBufInfo, IMG_TRUE);

    /* Trigger transfer */
    msvdxio_startDMA(psContext->psCommonCtx, ui32TransferSize, psDmaLLBufInfo->ui32DevVirt);

    return IMG_SUCCESS;
}

#endif  // USE_MTX_DMA_LOAD


/*!
******************************************************************************

 @Function              msvdxio_AddTransferBlock

******************************************************************************/
static IMG_RESULT
msvdxio_AddTransferBlock(
    IMG_UINT32              ui32PhysAddr,  /* Physical MTX ui32PhysAddress */
    IMG_UINT32              ui32Size,      /* pui32Data ui32Size in 32bit words */
    IMG_UINT32            * pui32Data,
    MSVDXIO_DmacBufInfo   * psBufsInfo,
    IMG_UINT32            * pui32CurLLElem,
    IMG_UINT32              ui32MaxLLElems
)
{
    const IMG_UINT32 cui32DMALimit = (64 * 1024 - 1) / 4; // 64kB minus one in words
    IMG_UINT32 ui32I, ui32Num64 = ui32Size / cui32DMALimit;
    IMG_UINT32 ui32NumRem = ui32Size % cui32DMALimit;
    IMG_UINT32* pui32DataAux = IMG_NULL;
    IMG_RESULT ui32Result = IMG_SUCCESS;
    IMG_UINT32 ui32MtxAddress;
    for( ui32I = 0 ; ui32I < ui32Num64 ; ++ui32I )
    {
        pui32DataAux = &(pui32Data[cui32DMALimit * ui32I]);
        /* Destination address in MTX memory, in bytes */
        ui32MtxAddress = ui32PhysAddr + (cui32DMALimit * ui32I) * 4;

        psBufsInfo[*pui32CurLLElem].ui32Addr = ui32MtxAddress;
        psBufsInfo[*pui32CurLLElem].ui32Size = (cui32DMALimit);
        psBufsInfo[*pui32CurLLElem].pui32Data = pui32DataAux;
        (*pui32CurLLElem)++;
        if (*pui32CurLLElem >= ui32MaxLLElems) {
            return IMG_ERROR_STORAGE_TYPE_FULL;
        }
    }

    if(ui32NumRem > 0)
    {
        pui32DataAux = &(pui32Data[cui32DMALimit * ui32Num64]);
        /* Destination address in MTX memory, in bytes */
        ui32MtxAddress = ui32PhysAddr + (cui32DMALimit * ui32Num64) * 4;

        psBufsInfo[*pui32CurLLElem].ui32Addr = ui32MtxAddress;
        psBufsInfo[*pui32CurLLElem].ui32Size = ui32NumRem;
        psBufsInfo[*pui32CurLLElem].pui32Data = pui32DataAux;
        (*pui32CurLLElem)++;
        if (*pui32CurLLElem >= ui32MaxLLElems) {
            return IMG_ERROR_STORAGE_TYPE_FULL;
        }
    }

    return ui32Result;
}


/*!
******************************************************************************

 @Function              msvdxio_PackFwPsrInfo

******************************************************************************/
static IMG_RESULT
msvdxio_PackFwPsrInfo(
    const VDECFW_sFirmwareBinInfo * psFwBinModInfo,
    const VXDIO_sDdBufInfo        * psFwTextBufInfo,
    const VXDIO_sDdBufInfo        * psFwDataBufInfo,
    VDECFW_sPsrModInfo            * psPsrModInfo
)
{
#ifndef SEC_USE_FW_RELOC_INFO_PACKING
    IMG_UINT32 copyOffset = 0;
    IMG_UINT32 copySize = 0;

    /* Check if we have enough space in VDECFW_sTransaction to attach relocation data.
     * We need 9 bytes for each relocated text word and 4 bytes for each relocated
     * data word. */
    if ((psFwBinModInfo->ui32TextRelocSize * 9 +
         psFwBinModInfo->ui32DataRelocSize * 4) > sizeof(psPsrModInfo->aui32PsrRelocData))
    {
        REPORT(REPORT_MODULE_DECODER, REPORT_ERR,
               "Relocation data is too big to attach to firmware message.");
        return IMG_ERROR_GENERIC_FAILURE;
    }
#endif /* SEC_USE_FW_RELOC_INFO_PACKING */

    /* Virtual address and size of module text, we don't need UPDATE_DEVICE, as this was done at allocation time. */
    psPsrModInfo->uispPsrTextLoadAddr = psFwTextBufInfo->ui32DevVirt;
    psPsrModInfo->ui32PsrTextSize = psFwTextBufInfo->ui32BufSize;
    /* Virtual address and size of module data, we don't need UPDATE_DEVICE, as this was done at allocation time. */
    psPsrModInfo->uispPsrDataLoadAddr = psFwDataBufInfo->ui32DevVirt;
#ifndef SEC_USE_FW_RELOC_INFO_PACKING
    /* Size of module data segment in bytes taken directly from buffer info. */
    psPsrModInfo->ui32PsrDataSize = psFwDataBufInfo->ui32BufSize;
#else /* SEC_USE_FW_RELOC_INFO_PACKING */
    /* Size of module data segment in bytes calculated based on module info. */
    psPsrModInfo->ui32PsrDataSize = psFwBinModInfo->ui32DataSize * sizeof(IMG_UINT32);
    /* Packed relocation info size in bytes. */
    psPsrModInfo->ui32PsrPackedRelocInfoSize = psFwBinModInfo->ui32PackedRelocInfoSize * sizeof(IMG_UINT32);
#endif /* SEC_USE_FW_RELOC_INFO_PACKING */
#ifdef SEC_USE_FW_CTX_TRIMMING
    /* Parser context size in bytes. */
    psPsrModInfo->ui32PsrCtxSize = psFwBinModInfo->ui32PsrCtxSize * sizeof(IMG_UINT32);
#endif /* SEC_USE_FW_CTX_TRIMMING */

    /* Where to put module's text */
    psPsrModInfo->ui32PsrTextAddr = psFwBinModInfo->ui32TextOrigin;
    /* Address for which module's text was linked, needed for relocation */
    psPsrModInfo->ui32PsrTextOrig = psFwBinModInfo->ui32TextOrigin;
#ifndef SEC_USE_FW_RELOC_INFO_PACKING
    /* Size of relocation data (in words) */
    psPsrModInfo->ui32PsrTextRelocSize = psFwBinModInfo->ui32TextRelocSize;
#endif /* SEC_USE_FW_RELOC_INFO_PACKING */

    /* Where to put module's data */
    psPsrModInfo->ui32PsrDataAddr = psFwBinModInfo->ui32DataOrigin;
    /* Address for which module's data was linked, needed for relocation */
    psPsrModInfo->ui32PsrDataOrig = psFwBinModInfo->ui32DataOrigin;
#ifndef SEC_USE_FW_RELOC_INFO_PACKING
    /* Size of relocation data (in words) */
    psPsrModInfo->ui32PsrDataRelocSize = psFwBinModInfo->ui32DataRelocSize;
#endif /* SEC_USE_FW_RELOC_INFO_PACKING */

#ifndef SEC_USE_FW_RELOC_INFO_PACKING
    /* Attach relocation data to VDECFW_sTransaction */
    IMG_MEMSET(&psPsrModInfo->aui32PsrRelocData, 0, sizeof(psPsrModInfo->aui32PsrRelocData));
    copyOffset = 0;
    copySize = psFwBinModInfo->ui32TextRelocSize*sizeof(*psFwBinModInfo->aui32TextReloc);
    IMG_MEMCPY(&psPsrModInfo->aui32PsrRelocData[copyOffset], psFwBinModInfo->aui32TextReloc, copySize);
    copyOffset += copySize/sizeof(*psPsrModInfo->aui32PsrRelocData);

    copySize = psFwBinModInfo->ui32TextRelocSize*sizeof(*psFwBinModInfo->aui32TextRelocFullAddr);
    IMG_MEMCPY(&psPsrModInfo->aui32PsrRelocData[copyOffset], psFwBinModInfo->aui32TextRelocFullAddr, copySize);
    copyOffset += copySize/sizeof(*psPsrModInfo->aui32PsrRelocData);

    copySize = psFwBinModInfo->ui32TextRelocSize*sizeof(*psFwBinModInfo->aui8TextRelocType);
    IMG_MEMCPY(&psPsrModInfo->aui32PsrRelocData[copyOffset], psFwBinModInfo->aui8TextRelocType, copySize);
    /* align to 4 bytes */
    copySize = VDEC_ALIGN_SIZE(copySize, 4);
    copyOffset += copySize/sizeof(*psPsrModInfo->aui32PsrRelocData);

    copySize = psFwBinModInfo->ui32DataRelocSize*sizeof(*psFwBinModInfo->aui32DataReloc);
    IMG_MEMCPY(&psPsrModInfo->aui32PsrRelocData[copyOffset], psFwBinModInfo->aui32DataReloc, copySize);
    copyOffset += copySize/sizeof(*psPsrModInfo->aui32PsrRelocData);
#endif /* SEC_USE_FW_RELOC_INFO_PACKING */

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              msvdxio_PrepareFirmware

******************************************************************************/
static IMG_RESULT
msvdxio_PrepareFirmware(
    MSVDXIO_sContext          * psContext,
    const MSVDXIO_sMTXSwInfo  * psMTXSwInfo
)
{
    IMG_UINT32 ui32TextStart;  /* Physical RAM offset of text first byte. */
    IMG_UINT32 ui32DataStart;  /* Physical RAM offset of data first byte. */
    IMG_UINT32 ui32TextEnd;    /* Physical RAM offset of text last byte + 1. */
    IMG_UINT32 ui32DataEnd;    /* Physical RAM offset of data last byte + 1. */
    IMG_UINT32 ui32TextBytes;
    IMG_UINT32 ui32DataBytes;
    IMG_UINT32 ui32MaxAddr;
    IMG_RESULT ui32Result = IMG_SUCCESS;
    IMG_UINT32 aui32MtxTextDataSecGap[MSVDXIO_MTX_TEXT_DATA_SEC_GAP];

    /* Start by assuming the whole memory is available. */
    ui32MaxAddr = psContext->psCommonCtx->sMTXRamInfo.ui32MTXRamSize;

    /* Calculate text size. */
    ui32TextStart = (psMTXSwInfo->ui32TextAddr >> 1) & psContext->psCommonCtx->sMTXRamInfo.ui32MTXRamMask;  /* Text addr is MTX uncompressed address. */
    ui32TextBytes = psMTXSwInfo->ui32TextDmaSize << 2;  /* ui32TextSize is in words. */
    ui32TextEnd = ui32TextStart + ui32TextBytes;

    /* Calculate data size. */
    ui32DataStart = psMTXSwInfo->ui32DataAddr & psContext->psCommonCtx->sMTXRamInfo.ui32MTXRamMask;
    ui32DataBytes = psMTXSwInfo->ui32DataSize << 2;  /* ui32DataSize is in words. */
    ui32DataEnd = ui32DataStart + ui32DataBytes;

    /* Fail here if the firmware is empty. */
    if ((ui32TextBytes == 0) || (ui32DataBytes == 0))
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Firmware text or data missing!");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    /* Fail here if the firmware does not fit. */
    if ((ui32TextEnd > ui32MaxAddr) || (ui32DataEnd > ui32MaxAddr))
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Firmware data or text extends beyond the end of memory!");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (((ui32DataStart < ui32TextEnd) && (ui32DataEnd > ui32TextStart)) ||
        ((ui32TextStart < ui32DataEnd) && (ui32TextEnd > ui32DataStart)))
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Firmware data and text sections overlap!");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

#ifdef USE_MTX_DMA_LOAD
    if (ui32DataStart < ui32TextStart)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Placing data before text is not supported in current"
               " linked list implementation!");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
#endif /* USE_MTX_DMA_LOAD */

    DEBUG_REPORT(REPORT_MODULE_VXDIO,
                 "MTX Text address: 0x%08x, size 0x%08x (physical 0x%08x..0x%08x)",
                 psMTXSwInfo->ui32TextAddr,
                 ui32TextBytes,
                 ui32TextStart,
                 ui32TextEnd - 1);
    DEBUG_REPORT(REPORT_MODULE_VXDIO,
                 "MTX Data address: 0x%08x, size 0x%08x (physical 0x%08x..0x%08x)",
                 psMTXSwInfo->ui32DataAddr,
                 ui32DataBytes,
                 ui32DataStart,
                 ui32DataEnd - 1);

    /* Add MTX text section to list of transfer blocks */
    ui32Result = msvdxio_AddTransferBlock(ui32TextStart,
                                          psMTXSwInfo->ui32TextDmaSize,
                                          psMTXSwInfo->pui32Text,
                                          psContext->asBufsInfo,
                                          &psContext->ui32CurLLElem,
                                          VDEC_DMAC_LL_BUFS_COUNT);
    if (ui32Result != IMG_SUCCESS)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Failed to add transfer block (%d/%d)!",
               psContext->ui32CurLLElem, VDEC_DMAC_LL_BUFS_COUNT);
        return ui32Result;
    }

    /* Start of MTX data section is aligned to 8 bytes. Destination buffer of DMA linked list has to be
     * continuous, fill potential 1 word gap between text and data sections. */
    if ((psMTXSwInfo->ui32TextDmaSize & 1) == 1)
    {
        IMG_MEMSET(aui32MtxTextDataSecGap, 0, sizeof(aui32MtxTextDataSecGap));
        ui32Result = msvdxio_AddTransferBlock(ui32DataStart - sizeof(*psMTXSwInfo->pui32Data),
                                              MSVDXIO_MTX_TEXT_DATA_SEC_GAP,
                                              aui32MtxTextDataSecGap,
                                              psContext->asBufsInfo,
                                              &psContext->ui32CurLLElem,
                                              VDEC_DMAC_LL_BUFS_COUNT);
        if (ui32Result != IMG_SUCCESS)
        {
            REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Failed to add transfer block (%d/%d)!",
                    psContext->ui32CurLLElem, VDEC_DMAC_LL_BUFS_COUNT);
            return ui32Result;
        }
    }

    /* Add MTX data section to list of transfer blocks */
    ui32Result = msvdxio_AddTransferBlock(ui32DataStart,
                                          psMTXSwInfo->ui32DataSize,
                                          psMTXSwInfo->pui32Data,
                                          psContext->asBufsInfo,
                                          &psContext->ui32CurLLElem,
                                          VDEC_DMAC_LL_BUFS_COUNT);
    if (ui32Result != IMG_SUCCESS)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Failed to add transfer block (%d/%d)!",
               psContext->ui32CurLLElem, VDEC_DMAC_LL_BUFS_COUNT);
        return ui32Result;
    }

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              msvdxio_AuthenticateFirmware

******************************************************************************/
static IMG_RESULT
msvdxio_AuthenticateFirmware(
    MSVDXIO_sMTXSwInfo    * sMTXSwInfo
)
{
    return IMG_SUCCESS;
}

#endif

/*!
******************************************************************************

 @Function              MSVDXIO_Poll

******************************************************************************/
IMG_RESULT
MSVDXIO_Poll(
    const IMG_HANDLE    hCoreContext,
    IMG_UINT32          ui32MemRegion,
    IMG_UINT32          ui32Offset,
    IMG_UINT32          ui32RequValue,
    IMG_UINT32          ui32Enable,
    VXDIO_ePollMode     ePollMode
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_RESULT          ui32Result = IMG_SUCCESS;
    IMG_UINT32          ui32Wait;
    IMG_UINT32          ui32ReadCount = 0x1000UL;
    IMG_UINT32          ui32PollMode;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if (ui32MemRegion >= REGION_MAX)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A valid VXD memory space must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }


    switch(ePollMode)
    {
    case VXDIO_POLL_EQUAL:
        ui32PollMode = TAL_CHECKFUNC_ISEQUAL;
        break;
#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    case VXDIO_POLL_NOT_EQUAL:
        ui32PollMode = TAL_CHECKFUNC_NOTEQUAL;
        break;
#endif
    default:
        IMG_ASSERT("Invalid poll mode" == 0);
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Invalid poll mode");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if (psContext->psCommonCtx->bFakeMtx)
    {
        ui32Wait = 0x20UL;
    }
    else
    {
        ui32Wait = 0x1UL;
    }


    if (ui32MemRegion == REGION_VXDSYS_REGSPACE &&
        (ui32Offset == MSVDX_CORE_CR_MSVDX_CONTROL_OFFSET ||
         ui32Offset == MSVDX_CORE_CR_MMU_MEM_REQ_OFFSET))
    {
        ui32ReadCount = 10000;
        ui32Wait = 100000;
    }

#ifdef SECURE_DEVICE_FPGA_TEST
    {
        IMG_UINT32 ui32Count, ui32ReadVal;
        for( ui32Count = 0; ui32Count < ui32ReadCount; ui32Count++ )
        {
            TALREG_WriteWord32(TAL_GetMemSpaceHandle("C0_REG_MSVDX_TEST_3"),
                       MSVDX_TEST_3_SECURE_CPU_CTRL_0_OFFSET,
                       1 | (1 << 1));

            /* Read from the device */
            ui32Result = TALREG_ReadWord32(psContext->psCommonCtx->pahMemSpace[ui32MemRegion], ui32Offset, &ui32ReadVal);
            if (IMG_SUCCESS != ui32Result)
            {
                break;
            }

            if ((ui32ReadVal & ui32Enable) == ui32RequValue)
            {
                ui32Result = IMG_SUCCESS;
                break;
            }

            //Make the current task sleep until timeout jiffies have elapsed.
            msleep(1);
        }

        if ((IMG_SUCCESS != ui32Result) || (ui32Count >= ui32PollCount))
        {
            ui32Result = IMG_ERROR_TIMEOUT;
        }
    }

#else

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    ui32ReadCount = 1000000;
    ui32Wait = 100;
#endif

    ui32Result = TALREG_Poll32(psContext->psCommonCtx->pahMemSpace[ui32MemRegion],
                               ui32Offset,
                               ui32PollMode,
                               ui32RequValue,
                               ui32Enable,
                               ui32ReadCount,
                               ui32Wait);
#endif
    if (ui32Result != IMG_SUCCESS)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Poll timeout");
    }

error:

    return ui32Result;
}



/*!
******************************************************************************

 @Function              MSVDXIO_GetCoreState

******************************************************************************/
IMG_RESULT
MSVDXIO_GetCoreState(
    const IMG_HANDLE    hCoreContext,
    VXDIO_sState      * psState
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_UINT32          ui32Result;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    /* There's only one decoding pipe available in MSVDX. */
    psState->sFwState.asPipeState[0].bPipePresent = IMG_TRUE;

    if (psContext->psCommonCtx->bClocks)
    {
        IMG_UINT32             ui32RegVal;
        VDECFW_sFirmwareState  sFirmwareState;

        //Reading only channel 2 (DMA_CHANNEL_SR1)
        ui32RegVal = REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx, DMAC, DMAC_COUNT, 2);
        psState->sFwState.asPipeState[0].aui32DMACStatus[0] = REGIO_READ_FIELD(ui32RegVal, DMAC, DMAC_COUNT, CNT);

        // FE current MB number.
        // Obtain the last processed MB on front-end HW expressed as an X,Y offset into the frame.
        ui32RegVal = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_VEC, CR_VEC_ENTDEC_INFORMATION);
        psState->sFwState.asPipeState[0].sFeMb.ui32X = REGIO_READ_FIELD(ui32RegVal, MSVDX_VEC, CR_VEC_ENTDEC_INFORMATION, FE_ENTDEC_LATEST_MB_ADDR_X);
        psState->sFwState.asPipeState[0].sFeMb.ui32Y = REGIO_READ_FIELD(ui32RegVal, MSVDX_VEC, CR_VEC_ENTDEC_INFORMATION, FE_ENTDEC_LATEST_MB_ADDR_Y);

        // BE current MB number.
        // Obtain the last processed MB on back-end HW expressed as an X,Y offset into the frame.
        ui32RegVal = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_VDMC, CR_VDMC_MACROBLOCK_NUMBER);
        psState->sFwState.asPipeState[0].sBeMb.ui32X = REGIO_READ_FIELD(ui32RegVal, MSVDX_VDMC, CR_VDMC_MACROBLOCK_NUMBER, CR_VDMC_MACROBLOCK_X_OFFSET);
        psState->sFwState.asPipeState[0].sBeMb.ui32Y = REGIO_READ_FIELD(ui32RegVal, MSVDX_VDMC, CR_VDMC_MACROBLOCK_NUMBER, CR_VDMC_MACROBLOCK_Y_OFFSET);

        // MSVDX interrupt status.
        psState->sFwState.asPipeState[0].ui32IntStatus = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_INTERRUPT_STATUS);

        // Determine the latest transaction to have passed each checkpoint in the firmware.
        // Read the firmware state from VEC Local RAM
        VXDIO_VLRReadWords(psContext->psCommonCtx,
                             REGION_VLRFE_REGSPACE,
                             VLR_FIRMWARE_STATE_AREA_BASE_ADDR,
                             (sizeof(VDECFW_sFirmwareState)>>2),
                             (IMG_UINT32*)&sFirmwareState,
                             IMG_FALSE);
        // Update firmware state fields.
        IMG_MEMCPY(psState->sFwState.aui8ControlFenceID,
                   sFirmwareState.aui8ControlFenceID,
                   sizeof(psState->sFwState.aui8ControlFenceID));
        IMG_MEMCPY(psState->sFwState.aui8DecodeFenceID,
                   sFirmwareState.aui8DecodeFenceID,
                   sizeof(psState->sFwState.aui8DecodeFenceID));
        IMG_MEMCPY(psState->sFwState.aui8CompletionFenceID,
                   sFirmwareState.aui8CompletionFenceID,
                   sizeof(psState->sFwState.aui8CompletionFenceID));
        IMG_MEMCPY(psState->sFwState.asPipeState[0].aui32CheckPoint,
                   sFirmwareState.aui32CheckPoint,
                   sizeof(psState->sFwState.asPipeState[0].aui32CheckPoint));
        psState->sFwState.asPipeState[0].eFirmwareAction = sFirmwareState.eFirmwareAction;
        psState->sFwState.asPipeState[0].ui32FenceValue  = sFirmwareState.ui32FenceValue;
        psState->sFwState.asPipeState[0].ui8CurCodec     = sFirmwareState.ui8CurCodec;
        psState->sFwState.asPipeState[0].ui32FeSlices    = sFirmwareState.ui32FESlices;
        psState->sFwState.asPipeState[0].ui32BeSlices    = sFirmwareState.ui32FESlices;


        // Get runtime information from MSVDX.
#ifdef SEC_USE_REAL_FW
        if (!psContext->psCommonCtx->bFakeMtx)
        {
            IMG_UINT32  ui32Reg = 0;

            // MTX status.
            ui32Reg = 0;
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RNW, 1);         // Read
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_USPECIFIER, 5);  // PC or PCX
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RSPECIFIER, 0);  // PC
            REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, ui32Reg);
            psState->sEPRuntimeStatus.ui32MTX_PC = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_DATA);

            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RNW, 1);         // Read
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_USPECIFIER, 5);  // PC or PCX
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RSPECIFIER, 1);  // PCX
            REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, ui32Reg);
            psState->sEPRuntimeStatus.ui32MTX_PCX = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_DATA);

            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RNW, 1);         // Read
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_USPECIFIER, 3);  // A0StP
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RSPECIFIER, 0);
            REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, ui32Reg);
            psState->sEPRuntimeStatus.ui32MTX_A0StP = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_DATA);

            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RNW, 1);         // Read
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_USPECIFIER, 3);  // A0FrP
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RSPECIFIER, 1);
            REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, ui32Reg);
            psState->sEPRuntimeStatus.ui32MTX_A0FrP = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_DATA);

            psState->sEPRuntimeStatus.ui32MTX_ENABLE = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_ENABLE);
            psState->sEPRuntimeStatus.ui32MTX_STATUS_BITS = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_STATUS);
            psState->sEPRuntimeStatus.ui32MTX_FAULT0 = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_FAULT0);
        }
#endif
        psState->sEPRuntimeStatus.ui32DMA_COUNT[0] = REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx,  DMAC, DMAC_COUNT, 0);
        psState->sEPRuntimeStatus.ui32DMA_COUNT[1] = REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx,  DMAC, DMAC_COUNT, 1);
        psState->sEPRuntimeStatus.ui32DMA_COUNT[2] = REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx,  DMAC, DMAC_COUNT, 2);

        psState->sEPRuntimeStatus.ui32DMA_SETUP[0] = REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx,  DMAC, DMAC_SETUP, 0);
        psState->sEPRuntimeStatus.ui32DMA_SETUP[1] = REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx,  DMAC, DMAC_SETUP, 1);
        psState->sEPRuntimeStatus.ui32DMA_SETUP[2] = REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx,  DMAC, DMAC_SETUP, 2);

        psState->sEPRuntimeStatus.ui32DMA_PERIPHERAL_ADDR[0] =
            REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx, DMAC, DMAC_PERIPHERAL_ADDR, 0);
        psState->sEPRuntimeStatus.ui32DMA_PERIPHERAL_ADDR[1] =
            REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx, DMAC, DMAC_PERIPHERAL_ADDR, 1);
        psState->sEPRuntimeStatus.ui32DMA_PERIPHERAL_ADDR[2] =
            REGIO_READ_TABLE_REGISTER(psContext->psCommonCtx, DMAC, DMAC_PERIPHERAL_ADDR, 2);
    }

    return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDXIO_HandleInterrupts

******************************************************************************/
IMG_RESULT
MSVDXIO_HandleInterrupts(
    const IMG_HANDLE    hCoreContext,
    VXD_sIntStatus    * psIntStatus
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_UINT32          ui32Enable;
    IMG_UINT32          ui32Reg;
    IMG_UINT32          ui32Result;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if (psIntStatus == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A valid interrupt status context must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    psIntStatus->ui32Pending = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_INTERRUPT_STATUS);
    ui32Enable = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE);

    psIntStatus->ui32Pending &= ui32Enable;

    if (psIntStatus->ui32Pending & MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_MASK)
    {
        ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MMU_STATUS);
        psIntStatus->ui32Requestor = (psIntStatus->ui32Pending & MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_MASK)
                                            >> MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_SHIFT;
        psIntStatus->MMU_FAULT_ADDR = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_STATUS, CR_MMU_FAULT_ADDR) << 12 ;
        psIntStatus->MMU_FAULT_RNW = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_STATUS, CR_MMU_FAULT_RNW);
        psIntStatus->MMU_PF_N_RW = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_STATUS, CR_MMU_PF_N_RW);

        // Disable MMU interrupt until reset has completed since the
        // hardware may continue to raise fault interrupts.
        ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE);
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE,
                             ui32Reg & ~MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_MASK);
        /*
         * We're now expecting rest of the stack to eventually:
         * - read the interrupt status
         * - reset the core (depending on status- maybe fixing MMU PF)
         * - enable MMU PF interrupt
         */
    }

    if ((psIntStatus->ui32Pending & MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MTX_IRQ_MASK) &&
        (IMG_NULL != psIntStatus->psMsgQueue))
    {
        if (!LST_empty(&psIntStatus->psMsgQueue->sFreeMsgList))
        {
            /* Clear interrupt first to allow for others coming.
             * Interrupt has to be cleared before reading comms buffer, as MTX can be currently
             * waiting for free space in this buffer. If we tried to read the buffer first,
             * following scenario could occur (on assumption that comms buffer is full):
             * -> host reads buffer
             * -> MTX starts adding new messages, as there is a space for messages now
             * -> host ends reading buffer and checks if there are any new messages
             *    (there are none as MTX did not finish yet)
             * -> MTX ends writing to buffer and signal IRQ
             * -> host clears IRQ
             * Lockup occurs as host skipped interrupt. */
            // Clear firmware message interrupt.
            REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_INTERRUPT_CLEAR,
                                 MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MTX_IRQ_MASK);

            // Copy all the messages into a temporary host buffer.
            ui32Result = MTXIO_ProcessMTXMsgs(&psContext->sMsgContext, psIntStatus->psMsgQueue);
            IMG_ASSERT(ui32Result == IMG_SUCCESS);
        }
    }

    return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDXIO_SendFirmwareMessage

******************************************************************************/
IMG_RESULT
MSVDXIO_SendFirmwareMessage(
    const IMG_HANDLE    hCoreContext,
    VXD_eCommsArea      eArea,
    const IMG_VOID    * psMsgHdr
)
{
    const MSVDXIO_sContext    * psContext = (const MSVDXIO_sContext *)hCoreContext;
    IMG_UINT32                  ui32Reg;
    IMG_UINT32                  ui32Result;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if (psMsgHdr == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A valid message buffer must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef SECURE_MEM_ACCESS
    ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MEM_PROT_CTRL_2);
    ui32Reg &= 0xFFFFFFFE;
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MEM_PROT_CTRL_2, ui32Reg);
#endif

    ui32Result = MTXIO_SendMTXMsg(&psContext->sMsgContext,
                                  eArea,
                                  psMsgHdr);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }

    // Kick the MTX. Must write to fake MTX register space in memory.
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_KICKI, MTX_KICKI, 1);
    //REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_KICKI, ui32Reg);

#ifdef SECURE_CPU_REG_ACCESS
    TALREG_WriteWord32(psContext->psCommonCtx->pahMemSpace[REGION_VXDTEST_3_REGSPACE],
                       MSVDX_TEST_3_SECURE_CPU_CTRL_0_OFFSET,
                       bSecEnable | SECURE_REG_ACCESS);
#endif

    TALREG_WriteWord32(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_MEMSPACE], MTX_CORE_CR_MTX_KICKI_OFFSET, ui32Reg);
    return IMG_SUCCESS;

error:
    return ui32Result;
}


#ifdef POST_TEST
/*!
******************************************************************************

 @Function              checkMTXStart

 @Description

 Function to check MTX starts.

******************************************************************************/
IMG_RESULT checkMTXStart(
        IMG_HANDLE hMsvdxContext
)
{
    IMG_RESULT eResult;
    IMG_UINT32 ui32Register;
    IMG_UINT32 ui32TimeoutCounter = 1000;
    MSVDXIO_sContext *psContext = (MSVDXIO_sContext *)hMsvdxContext;

    /* wait MTX starts until timeout */
    do
    {
        /* read register. ATTENTION: an eventual invalid returned value is not managed */
        ui32Register = REGIO_READ_REGISTER(psContext, MTX_CORE, CR_MTX_ENABLE);

        /* decrement timeout counter */
        ui32TimeoutCounter--;
    }
    while((1 == REGIO_READ_FIELD(ui32Register, MTX_CORE, CR_MTX_ENABLE, MTX_TOFF))
       && (ui32TimeoutCounter > 0));

    /* set result */
    if(0 == ui32TimeoutCounter)
    {
        /* test FAILED */
        eResult = IMG_ERROR_FATAL;
    }
    else
    {
        /* test PASSED */
        eResult = IMG_SUCCESS;
    }

    return eResult;
}

#endif


#ifdef STACK_USAGE_TEST
#ifdef SEC_USE_REAL_FW
/*!
******************************************************************************

 @Function              MSVDXIO_GetStackUsage

 @Description

 Function to get stack usage. It read stack memory space to calculated the
 overwritten filled part.

******************************************************************************/
IMG_RESULT
MSVDXIO_GetStackUsage(
    const IMG_HANDLE    hCoreContext,
    IMG_UINT32        * paui32StackInfoArray
)
{
    IMG_UINT32 ui32Reg;
    IMG_UINT32 ui32ReadData;
    IMG_UINT32 ui32RamId;
    IMG_UINT32 ui32MtxTimeOut;
    IMG_UINT32 ui32Addr;
    IMG_UINT32 ui32MTXBankSize;
    IMG_UINT32 ui32Size;
    IMG_UINT32 ui32NumOfOverwLoc;         /* number of overwritten locations */
    IMG_UINT32 ui32LastOverwAdd;          /* last overwritten address */
    IMG_UINT32 ui32StackUsage;            /* stack usage in words */
    IMG_RESULT ui32Result = IMG_SUCCESS;
    MSVDXIO_sContext  * pMsvdxioCtx = (MSVDXIO_sContext *)hCoreContext;
    VXDIO_sContext * pVxdioCtx = &(pMsvdxioCtx->psCommonCtx);

    /* init variables */
    ui32Addr = STACK_START_ADDRESS;
    ui32Size = STACK_SIZE;
    ui32MTXBankSize = pVxdioCtx->sMTXRamInfo.ui32MTXBankSize;
    ui32ReadData = 0;
    ui32StackUsage = 0;
    ui32NumOfOverwLoc = 0;
    ui32LastOverwAdd = 0;

    /* Initiate read access to RAM block...*/
    ui32RamId = MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize);
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));

    if (ui32Size > 1)
    {
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
    }

    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMR, 1);
    REGIO_WRITE_REGISTER(pVxdioCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);

    /* Repeat for all words...*/
    while ((ui32Size > 0)
       &&  (IMG_ERROR_TIMEOUT != ui32Result))
    {
        /* Wait for read to complete...*/
        ui32MtxTimeOut = MSVDXIO_TIMEOUT_COUNTER;

        do
        {
            /* Check MTX is OK...*/
            ui32Reg = REGIO_READ_REGISTER(pVxdioCtx, MTX_CORE, CR_MTX_FAULT0);
            if (ui32Reg != 0)
            {
                ui32Result = IMG_ERROR_TIMEOUT;
            }

            ui32MtxTimeOut--;
            ui32Reg = REGIO_READ_REGISTER(pVxdioCtx, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS);
        }
        while ((REGIO_READ_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS, MTX_MTX_MCM_STAT) == 0)
           &&  (ui32MtxTimeOut != 0)
           &&  (IMG_ERROR_TIMEOUT != ui32Result));

        /* go ahead if all ok */
        if(IMG_ERROR_TIMEOUT != ui32Result)
        {

            IMG_ASSERT(ui32MtxTimeOut != 0);

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
            TAL_PdumpComment(pVxdioCtx->pahMemSpace[REGION_VXDMTX_REGSPACE], "MTX LR Comms Poll - Read.");
            TAL_Poll(pVxdioCtx->pahMemSpace[REGION_VXDMTX_REGSPACE],
                     MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_OFFSET,
                     TAL_CHECKFUNC_ISEQUAL,
                     MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                     MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                     1000000, 100);
#endif

            /* get data */
            ui32ReadData = REGIO_READ_REGISTER(pVxdioCtx, MTX_CORE, CR_MTX_RAM_ACCESS_DATA_TRANSFER);
            /* if value is not a fill value */
            if(STACK_FILL_VALUE != ui32ReadData)
            {
                /* increment number of overwritten locations */
                ui32NumOfOverwLoc++;

                /* update last overwritten address */
                ui32LastOverwAdd = ui32Addr;
            }
            else
            {
                /* do nothing */
            }

            /* decrement remaining bytes */
            ui32Size--;

            /* If we have move from one RAM to the next...*/
            ui32Addr += 4;

            if ((MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize)) != ui32RamId)
            {
                /* Initiate read access to RAM block...*/
                ui32RamId = (MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize));
                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));

                if (ui32Size > 1)
                {
                    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
                }

                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMR, 1);
                REGIO_WRITE_REGISTER(pVxdioCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);
            }
        }
    }

    /* calculate stack usage in words */
    ui32StackUsage = (IMG_UINT32)((ui32LastOverwAdd - STACK_START_ADDRESS) / 4);

    /* copy stack usage in words to array location 1 */
    paui32StackInfoArray[0] = (IMG_UINT32)ui32StackUsage;

    /* calculate and copy stack usage in percentage to array location 2 */
    paui32StackInfoArray[1] = (IMG_UINT32)((ui32StackUsage * 100) / STACK_SIZE);

    /* copy overwritten space of used stack (effective stack usage) in words to array location 3 */
    paui32StackInfoArray[2] = (IMG_UINT32)ui32NumOfOverwLoc;

    /* calculate and copy overwritten space of used stack in percentage to array location 4 */
    paui32StackInfoArray[3] = (IMG_UINT32)((ui32NumOfOverwLoc * 100) / ui32StackUsage);

    /* calculate and copy overwritten space of entire stack in percentage (effective stack usage) to array location 5 */
    paui32StackInfoArray[4] = (IMG_UINT32)((ui32NumOfOverwLoc * 100) / STACK_SIZE);

    /* copy total stack size to array location 6 */
    paui32StackInfoArray[5] = STACK_SIZE;

    return ui32Result;
}


/*!
******************************************************************************

 @Function              msvdxio_MTXFillStack

 @Description

 Function to fill the MTX stack.

******************************************************************************/
static IMG_RESULT
msvdxio_MTXFillStack(
    const IMG_HANDLE    hCoreContext,
    IMG_UINT32          ui32Addr,         // Byte offset into MTX memory (starts at 0).
    IMG_UINT32          ui32Size,         // Number of 32-bit words to write.
    IMG_UINT32          ui32MTXBankSize,
    IMG_UINT32          ui32FillValue
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_RESULT          ui32Result = IMG_SUCCESS;
    IMG_INT32           ui32Reg;
    IMG_UINT32          ui32MtxTimeOut;
    IMG_UINT32          ui32RamId;
    IMG_UINT32          ui32AccessControl;

    /* Request the bus from the Dash...*/
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_IS_SLAVE, 1);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_GPIO_IN, 0x02);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);
    msvdxio_WaitForMTX(m);

    /* Set HOST_ACCESS so Codescape knows the host is using the bus. */
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_GPIO_IN, 0x03);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);

    /* Save the access control register...*/
    ui32AccessControl = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL);

    /* Initiate write access to RAM block...*/
    ui32RamId = MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize);
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));

    if (ui32Size > 1)
    {
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
    }

    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);

    /* Repeat for all words...*/
    while (ui32Size > 0)
    {
        /* Write the next word...*/
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_DATA_TRANSFER, ui32FillValue);

        /* Wait for write to complete...*/
        ui32MtxTimeOut = MSVDXIO_TIMEOUT_COUNTER;
        do
        {
             /* Check MTX is OK...*/
            ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_FAULT0);

            if (ui32Reg != 0)
            {
                ui32Result = IMG_ERROR_TIMEOUT;
                goto ErrorExit;
            }

            ui32MtxTimeOut--;
            ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS);
        }
        while ((REGIO_READ_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_STATUS, MTX_MTX_MCM_STAT) == 0) &&
               (ui32MtxTimeOut != 0));

        IMG_ASSERT(ui32MtxTimeOut != 0);

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
        TAL_PdumpComment(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_REGSPACE], "MTX LR Comms Poll - Write.");
        TAL_Poll(psContext->psCommonCtx->pahMemSpace[REGION_VXDMTX_REGSPACE],
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_OFFSET,
                 TAL_CHECKFUNC_ISEQUAL,
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                 MTX_CORE_CR_MTX_RAM_ACCESS_STATUS_MTX_MTX_MCM_STAT_MASK,
                 1000000, 100);
#endif

        ui32Size--;

        /* If we have move from one RAM to the next...*/
        ui32Addr += 4;
        if ((MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize)) != ui32RamId)
        {
            /* Initiate write access to RAM block...*/
            ui32RamId = (MSVDXIO_MTX_CORE_MEMORY + (ui32Addr / ui32MTXBankSize));
            ui32Reg = 0;
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMID, ui32RamId);
            REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCM_ADDR, (ui32Addr>>2));
            if (ui32Size > 1)
            {
                REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, MTX_MCMAI, 1);
            }
            REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32Reg);
        }
    }

ErrorExit:
    /* Restore the access control register...*/
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_RAM_ACCESS_CONTROL, ui32AccessControl);

    /* Release the bus...*/
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MTX_DEBUG, CR_MTX_DBG_IS_SLAVE, 1);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MTX_DEBUG, ui32Reg);

    /* Ensure the MTX is OK....*/
    if (!psContext->psCommonCtx->bFakeMtx)
    {
        IMG_ASSERT(REGIO_READ_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_FAULT0) == 0);
    }

    return ui32Result;
}
#else
/*!
******************************************************************************

 @Function              MSVDXIO_GetStackUsage

 @Description

 Function to get stack usage. It read stack memory space to calculated the
 overwritten filled part.

******************************************************************************/
IMG_RESULT
MSVDXIO_GetStackUsage(
    const IMG_HANDLE    hCoreContext,
    IMG_UINT32        * paui32StackInfoArray
)
{
    IMG_RESULT rResult;
    rResult = IMG_SUCCESS;

    /* do nothing at the moment */

    /* fake values */
    paui32StackInfoArray[0] = 0;
    paui32StackInfoArray[1] = 0;
    paui32StackInfoArray[2] = 0;
    paui32StackInfoArray[3] = 0;
    paui32StackInfoArray[4] = 0;
    paui32StackInfoArray[5] = 0;

    return rResult;
}


/*!
******************************************************************************

 @Function              msvdxio_MTXFillStack

 @Description

 Function to fill the MTX stack.

******************************************************************************/
static IMG_RESULT
msvdxio_MTXFillStack(
    const IMG_HANDLE    hCoreContext,
    IMG_UINT32          ui32Addr,         // Byte offset into MTX memory (starts at 0).
    IMG_UINT32          ui32Size,         // Number of 32-bit words to write.
    IMG_UINT32          ui32MTXBankSize,
    IMG_UINT32          ui32FillValue
)
{
    IMG_RESULT rResult;
    rResult = IMG_SUCCESS;

    /* do nothing at the moment */

    return rResult;
}
#endif
#endif


/*!
******************************************************************************

 @Function              MSVDXIO_LoadBaseFirmware

******************************************************************************/
IMG_RESULT
MSVDXIO_LoadBaseFirmware(
    const IMG_HANDLE        hCoreContext
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_UINT32          ui32Reg;
    IMG_UINT32          ui32Result = IMG_SUCCESS;
#ifdef POST_TEST
    IMG_UINT32            aui32FWCheckPoints[NUM_OF_FW_INIT_CHECKPOINTS];
    IMG_UINT8            ui8Index;
#endif

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    /* Need to init mtx comms area  */
    ui32Result = MTXIO_InitMTXComms(psContext->psCommonCtx,
                                    &psContext->sMsgContext);
    if(IMG_SUCCESS != ui32Result)
    {
        REPORT(REPORT_MODULE_HWCTRL,
                REPORT_ERR,
                "Failed to initialise MTX comms area");
        goto error;
    }

#ifdef POST_TEST
    if(IMG_TRUE == psContext->psCommonCtx->bPost)
    {
        /* Prepare FW VLR check-points */
        /* prepare values array */
        for(ui8Index = 0; ui8Index < NUM_OF_FW_INIT_CHECKPOINTS; ui8Index++)
        {
            aui32FWCheckPoints[ui8Index] = ((IMG_UINT32)(ui8Index + 1)) | CHKPNT_HOST_HIGHER_MASK_VALUE;
        }

        /* write values array in VLR */
        ui32Result = VXDIO_VLRWriteWords(psContext->psCommonCtx,
                                         REGION_VLRFE_REGSPACE,
                                         (VLR_FIRMWARE_STATE_AREA_BASE_ADDR + (VDECFW_CHECKPOINT_FI_START * 4)),
                                         NUM_OF_FW_INIT_CHECKPOINTS,
                                         &aui32FWCheckPoints[0]);
        if(IMG_SUCCESS != ui32Result)
        {
            /* Fail to write checkpoints values */

            REPORT(REPORT_MODULE_VXDIO,
                    REPORT_ERR,
                    "***** POST: VLR Check-points area initialisation FAILED");

            ui32Result = IMG_ERROR_FATAL;
            goto error;
        }
        else
        {
            /* checkpoints values initialised fine */

            REPORT(REPORT_MODULE_VXDIO,
                    REPORT_NOTICE,
                    "***** POST: VLR Check-points area initialisation PASSED");
        }
    }
    else
    {
        /* do nothing */
    }
#endif
#ifdef SEC_USE_REAL_FW
    if (!psContext->psCommonCtx->bFakeMtx)
    {
        IMG_BOOL        bHotStart = IMG_FALSE;
#ifndef USE_MTX_DMA_LOAD
        IMG_UINT32      ui32Elem;
#endif
        /* Set the MTX timer divide register - the value used is system dependent. */
        ui32Reg = 0;
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_SYSC_TIMERDIV, TIMER_EN, 1);                         /* Enabled. */
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_SYSC_TIMERDIV, TIMER_DIV, MSVDXIO_MTX_CLK_MHZ - 1);  /* Set for 1MHz timer. */
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_SYSC_TIMERDIV, ui32Reg);

        /* The MTX must not be enabled at this point, we are going to upload firmware soon. */
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_ENABLE, 0);

        // Ensure that the MTX is ready to have firmware loaded.
        ui32Result = msvdxio_WaitForMTX(psContext);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }

#ifdef STACK_USAGE_TEST
        if(IMG_TRUE == psContext->psCommonCtx->bStackUsageTest)
        {
            /* report the start of stack filling */
            REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                "***** STACK USAGE TEST - Filling stack space...");

            /* fill the stack */
            ui32Result =  msvdxio_MTXFillStack(psContext,
                                               STACK_START_ADDRESS,
                                               STACK_SIZE,
                                               psContext->psCommonCtx->sMTXRamInfo.ui32MTXBankSize,
                                               STACK_FILL_VALUE);

            /* report the end of stack filling */
            REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                "***** STACK USAGE TEST - Stack space filling done successfully");
        }
        else
        {
            /* do nothing */
        }
#endif
#ifndef USE_MTX_DMA_LOAD
        for (ui32Elem = 0; ui32Elem < psContext->ui32CurLLElem; ui32Elem++)
        {
            ui32Result = msvdxio_MTXWriteWords(psContext,
                                               psContext->asBufsInfo[ui32Elem].ui32Addr,
                                               psContext->asBufsInfo[ui32Elem].ui32Size,
                                               psContext->psCommonCtx->sMTXRamInfo.ui32MTXBankSize,
                                               psContext->asBufsInfo[ui32Elem].pui32Data,
                                               IMG_TRUE);
            IMG_ASSERT(ui32Result == IMG_SUCCESS);
            if (ui32Result != IMG_SUCCESS)
            {
                goto error;
            }
        }

#else

        // Load base component.
        ui32Result = msvdxio_DMAMTX(psContext,
                                    psContext->psCommonCtx->sMTXRamInfo.ui32MTXBankSize,
                                    psContext->ui32CurLLElem,
                                    psContext->asBufsInfo,
                                    psContext->sMTXSwInfo.ui32TextAddr,
                                    &psContext->sFw.sDmaLLBufInfo,
                                    &psContext->sFw.sFwBaseBufInfo);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }

#ifdef POST_TEST
        if(IMG_TRUE == psContext->psCommonCtx->bPost)
        {
            /* Check if DMA is running */
            ui32Result = checkDMAIsRunning(psContext->psCommonCtx);

            /* set return value */
            if(IMG_SUCCESS != ui32Result)
            {
                REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
                        "***** POST: Start DMA check FAILED");
                goto error;
            }
            else
            {
                /* DMA is running */
                REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                        "***** POST: Start DMA check PASSED");
            }
        }
        else
        {
            /* do nothing */
        }
#endif

        // Wait for linked list transfer to complete.
        // LIST_FINCTRL is set in count register to ensure that LIST_FIN signals
        // the completion of last link-list element not the the start.
        ui32Result = MSVDXIO_Poll(psContext,
                          REGION_DMAC_REGSPACE,
                          DMAC_DMAC_IRQ_STAT_OFFSET,
                          DMAC_DMAC_IRQ_STAT_LIST_FIN_MASK,
                          DMAC_DMAC_IRQ_STAT_LIST_FIN_MASK,
                          VXDIO_POLL_EQUAL);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            REPORT(REPORT_MODULE_VXDIO, REPORT_ERR, "Timeout waiting for firmware DMA linked list finished.");
            goto error;
        }
#endif

         /* The PC must be set before starting the MTX.
        Write the value (must be done before the register write request is issued).
        The HotStart address is 16 bytes after the normal cold entry point. */
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_DATA,
                             psContext->sMTXSwInfo.ui32TextOrigin + (bHotStart ? 16 : 0));

        /* Create register write request. */
        ui32Reg = 0;
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RNW, 0);         // Write
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_USPECIFIER, 5);  // PC or PCX
        REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RSPECIFIER, 0);  // PC
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, ui32Reg);

        /* Set the MTX running. */
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MTX_CORE, CR_MTX_ENABLE, 1);

#ifdef POST_TEST
        if(IMG_TRUE == psContext->psCommonCtx->bPost)
        {
            /* Execute MTX start test: check if MTX starts */
            ui32Result = checkMTXStart(psContext->psCommonCtx);
            if(IMG_SUCCESS != ui32Result)

            {
                /* test FAILED */
                REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
                    "***** POST: Start MTX check FAILED");
                goto error;
            }
            else
            {
                /* test SUCCESS */
                REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                    "***** POST: Start MTX check PASSED");
            }
        }
        else
        {
            /* do nothing */
        }
#endif

    }
#else
#ifdef STACK_USAGE_TEST
    if(IMG_TRUE == psContext->psCommonCtx->bStackUsageTest)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
           "***** STACK USAGE TEST - ATTENTION: Stack Usage Test is NOT performed with fake MTX: Dummy 0 values will be reported.");
    }
    else
    {
        /* do nothing */
    }
#endif
#endif

    /* Enable MTX interrupts. */
    ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_INTERRUPT_STATUS, CR_MTX_IRQ, 1);
    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE, ui32Reg);


error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDXIO_PrepareFirmware

******************************************************************************/
IMG_RESULT
MSVDXIO_PrepareFirmware(
    const IMG_HANDLE            hCoreContext,
    const IMG_HANDLE            hFirmware
)
{
    MSVDXIO_sContext      * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_UINT32              ui32Result = IMG_SUCCESS;
    const MSVDXIO_sFw     * psFw = (const MSVDXIO_sFw *)hFirmware;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef SEC_USE_REAL_FW
    if (!psContext->psCommonCtx->bFakeMtx)
    {
        /* Setup MTX base component data. */
        IMG_MEMSET(&psContext->sMTXSwInfo, 0, sizeof(psContext->sMTXSwInfo));

        psContext->sMTXSwInfo.ui32TextOrigin = psFw->sFwBaseBinInfo.ui32TextOrigin;
        psContext->sMTXSwInfo.ui32TextAddr = psFw->sFwBaseBinInfo.ui32TextOrigin;
        psContext->sMTXSwInfo.pui32Text = psFw->sFwBaseBinInfo.aui32Text;
        psContext->sMTXSwInfo.ui32TextDmaSize = psFw->sFwBaseBinInfo.ui32TextDmaSize;
        psContext->sMTXSwInfo.ui32DataOrigin = psFw->sFwBaseBinInfo.ui32DataOrigin;
        psContext->sMTXSwInfo.ui32DataAddr = psFw->sFwBaseBinInfo.ui32DataOrigin;
        psContext->sMTXSwInfo.pui32Data = psFw->sFwBaseBinInfo.aui32Data;
        psContext->sMTXSwInfo.ui32DataSize = psFw->sFwBaseBinInfo.ui32DataSize;

        // Authenticate the firmware here.
        msvdxio_AuthenticateFirmware(&psContext->sMTXSwInfo);

        ui32Result = msvdxio_PrepareFirmware(psContext, &psContext->sMTXSwInfo);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }

        // Prepare the parser section information when using modules.
        {
            VDEC_eVidStd            eStd;
            VDECFW_sPsrModInfo    * psPsrModInfo;
            IMG_BOOL                bUsingModules = IMG_FALSE;

            psPsrModInfo = psFw->sPsrModInfo.pvCpuVirt;

            // Copy the firmware into secure memory.
            for (eStd = 0; eStd < VDEC_STD_MAX; eStd++)
            {
                if (psFw->asFwBinModInfo[eStd].aui32Text)
                {
                    VDECFW_eCodecType eCodecType;

                    eCodecType = aeStdToCodec[eStd];

                    IMG_ASSERT((psFw->asFwBinModInfo[eStd].ui32TextDmaSize * sizeof(IMG_UINT32)) == psFw->asFwTextBufInfo[eStd].ui32BufSize);
                    IMG_MEMCPY(psFw->asFwTextBufInfo[eStd].pvCpuVirt,
                               psFw->asFwBinModInfo[eStd].aui32Text,
                               psFw->asFwTextBufInfo[eStd].ui32BufSize);
                    IMG_MEMCPY(psFw->asFwDataBufInfo[eStd].pvCpuVirt,
                               psFw->asFwBinModInfo[eStd].aui32Data,
                               psFw->asFwDataBufInfo[eStd].ui32BufSize);
                    UPDATE_DEVICE((&psFw->asFwTextBufInfo[eStd]), IMG_TRUE);
                    UPDATE_DEVICE((&psFw->asFwDataBufInfo[eStd]), IMG_TRUE);

                    msvdxio_PackFwPsrInfo(&psFw->asFwBinModInfo[eStd],
                                          &psFw->asFwTextBufInfo[eStd],
                                          &psFw->asFwDataBufInfo[eStd],
                                          &psPsrModInfo[eCodecType]);

                    bUsingModules = IMG_TRUE;
                }
            }

            if (bUsingModules)
            {
                // Update the device memory once data for all parser modules has been packed.
                UPDATE_DEVICE((&psFw->sPsrModInfo), IMG_TRUE);
            }
        }
    }
#endif

    // Take a copy of the firmware information.
    psContext->sFw = *psFw;

    return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDXIO_DisableClocks

 @Description

 This function disables MSVDX clocks.

 @Input     hCoreContext    : Handle to MSVDX IO context.

 @Return    IMG_RESULT      : This function returns either IMG_SUCCESS or an
                              error code.

******************************************************************************/
IMG_RESULT
MSVDXIO_DisableClocks(
    const IMG_HANDLE    hCoreContext
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_UINT32          ui32Reg;
    IMG_UINT32          ui32Result;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE,
        MSVDX_CORE_CR_MSVDX_MAN_CLK_ENABLE_CR_CORE_MAN_CLK_ENABLE_MASK);
    ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE);
    // Make sure all the clocks are off except core
    //IMG_ASSERT(ui32Reg == MSVDX_CORE_CR_MSVDX_MAN_CLK_ENABLE_CR_CORE_MAN_CLK_ENABLE_MASK);

    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE,
        ui32Reg & ~(MSVDX_CORE_CR_MSVDX_MAN_CLK_ENABLE_CR_CORE_MAN_CLK_ENABLE_MASK));

    psContext->psCommonCtx->bClocks = IMG_FALSE;

    DEBUG_REPORT(REPORT_MODULE_VXDIO, "MSVDX Clocks Disabled");

    return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDXIO_EnableClocks

******************************************************************************/
IMG_RESULT
MSVDXIO_EnableClocks(
    const IMG_HANDLE    hCoreContext,
    IMG_BOOL            bAutoClockGatingSupport,
    IMG_BOOL            bExtClockGating,
    IMG_BOOL            bForceManual
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;
    IMG_UINT32          ui32RegMsvdxManClkEnable = 0;
    IMG_UINT32          ui32RegMsvdxManClkEnable2 = 0;
    IMG_UINT32          ui32Result;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    // Core and MTX must ALWAYS be manual.
    REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
        MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_CORE_MAN_CLK_ENABLE,
        1);
    REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
        MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_MTX_MAN_CLK_ENABLE,
        1);

    // Only use auto clock-gating on more recent cores.
    if (bAutoClockGatingSupport && !bForceManual)
    {
        // Auto gating.
        if (!bExtClockGating)
        {
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VDEB_PROCESS_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VDEB_ACCESS_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VDMC_AUTO_CLK_ENABLE,
                1);
        }
        else
        {
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VEC_REGS_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_MEM_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_SCALE_BE_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_SCALE_FE_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_PROCESS2_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_PROCESS1_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_ACCESS2_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_ACCESS1_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDMC2_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDMC1_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VEC_FE_GRP2_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VEC_FE_GRP1_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_REG_AUTO_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_DMAC_AUTO_CLK_ENABLE,
                1);
        }

        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VEC_ENTDEC_AUTO_CLK_ENABLE,
            1);
        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VEC_ITRANS_AUTO_CLK_ENABLE,
            1);
        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VEC_FE_AUTO_CLK_ENABLE,
            1);
        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VEC_BE_AUTO_CLK_ENABLE,
            1);
        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VEC_DEC_AUTO_CLK_ENABLE,
            1);
    }
    else
    {
        // Manual gating.
        if (bExtClockGating)
        {
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VEC_REGS_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_MEM_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_SCALE_BE_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_SCALE_FE_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_PROCESS2_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_PROCESS1_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_ACCESS2_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDEB_ACCESS1_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDMC2_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VDMC1_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VEC_FE_GRP2_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_VEC_FE_GRP1_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_REG_MAN_CLK_ENABLE,
                1);
            REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable2,
                MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2, CR_DMAC_MAN_CLK_ENABLE,
                1);
        }

        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VDEB_PROCESS_MAN_CLK_ENABLE,
            1);
        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VDEB_ACCESS_MAN_CLK_ENABLE,
            1);
        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VDMC_MAN_CLK_ENABLE,
            1);
        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VEC_ENTDEC_MAN_CLK_ENABLE,
            1);
        REGIO_WRITE_FIELD(ui32RegMsvdxManClkEnable,
            MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE, CR_VEC_ITRANS_MAN_CLK_ENABLE,
            1);
    }

    // We now can turn the clocks on flag for msvdxio context before tuning clocks on
    if(!psContext->psCommonCtx->bClocks)
    {
        psContext->psCommonCtx->bClocks = IMG_TRUE;
    }

    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE,
        MSVDX_CORE_CR_MSVDX_MAN_CLK_ENABLE_CR_CORE_MAN_CLK_ENABLE_MASK);

    // Make sure all the clocks are off except core
    //IMG_ASSERT(REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE) == MSVDX_CORE_CR_MSVDX_MAN_CLK_ENABLE_CR_CORE_MAN_CLK_ENABLE_MASK);

    REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE,
        ui32RegMsvdxManClkEnable);

    if (bExtClockGating)
    {
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_MAN_CLK_ENABLE2,
            ui32RegMsvdxManClkEnable2);
    }

    DEBUG_REPORT(REPORT_MODULE_VXDIO, "MSVDX Clocks Enabled");

    return IMG_SUCCESS;

error:
    return ui32Result;
}

/*
******************************************************************************

 @Function              MSVDXIO_DeInitialise

******************************************************************************/
IMG_RESULT
MSVDXIO_DeInitialise(
    IMG_UINT32          ui32CoreNum,
    IMG_HANDLE          hCoreContext
)
{
    MSVDXIO_sContext  * psContext = (MSVDXIO_sContext *)hCoreContext;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_FREE(psContext->psCommonCtx->pahMemSpace);
    psContext->psCommonCtx->pahMemSpace = IMG_NULL;
    IMG_FREE(psContext);
    return IMG_SUCCESS;
}

/*
******************************************************************************

 @Function              msvdxio_GetCommonFunctions

******************************************************************************/
static IMG_VOID
msvdxio_GetCommonFunctions(
    VXDIO_sFunctions        * psFuncs
)
{
    IMG_ASSERT(psFuncs != IMG_NULL);

    psFuncs->pfnSecRegAccess = MSVDXIO_SetSecRegAccess;
    psFuncs->pfnPoll = MSVDXIO_Poll;
    psFuncs->pfnSelectPipe = IMG_NULL;
    psFuncs->pfnGetCoreState = MSVDXIO_GetCoreState;
    psFuncs->pfnHandleInterrupts = MSVDXIO_HandleInterrupts;
    psFuncs->pfnSendFirmwareMessage = MSVDXIO_SendFirmwareMessage;
    psFuncs->pfnLoadBaseFirmware = MSVDXIO_LoadBaseFirmware;
    psFuncs->pfnPrepareFirmware = MSVDXIO_PrepareFirmware;
    psFuncs->pfnDisableClocks = MSVDXIO_DisableClocks;
    psFuncs->pfnEnableClocks = MSVDXIO_EnableClocks;
    psFuncs->pfnDeInitialise = MSVDXIO_DeInitialise;
#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
    psFuncs->pfnPDUMPLock = MSVDXIO_PDUMPLock;
    psFuncs->pfnPDUMPUnLock = MSVDXIO_PDUMPUnLock;
    psFuncs->pfnPDUMPSync = MSVDXIO_PDUMPSync;
#endif
}

#if !defined(SECURE_MEDIA_SUPPORT) && !defined(VXD_BRIDGING)
/*
******************************************************************************

 @Function              MSVDXIO_PDUMPLock

******************************************************************************/
IMG_RESULT
MSVDXIO_PDUMPSync(
    const IMG_HANDLE  hCoreContext
)
{
    MSVDXIO_sContext  * psContext;
    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle container must be provided");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    psContext = (MSVDXIO_sContext *)hCoreContext;

    TALPDUMP_SyncWithId(psContext->psCommonCtx->pahMemSpace[REGION_VLRFE_REGSPACE], VDEC_BE_FE_SYNC_ID);
    TALPDUMP_SyncWithId(psContext->psCommonCtx->pahMemSpace[REGION_VLRBE_REGSPACE], VDEC_BE_FE_SYNC_ID);

    return IMG_SUCCESS;
}

/*
******************************************************************************

 @Function              MSVDXIO_PDUMPLock

******************************************************************************/
IMG_RESULT
MSVDXIO_PDUMPLock(
    const IMG_HANDLE  hCoreContext
)
{

    MSVDXIO_sContext  * psContext;
    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle container must be provided");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    psContext = (MSVDXIO_sContext *)hCoreContext;

    TALPDUMP_Lock(psContext->psCommonCtx->pahMemSpace[REGION_VLRFE_REGSPACE], VDEC_BE_FE_PDUMP_SEM);

    return IMG_SUCCESS;
}

/*
******************************************************************************

 @Function              MSVDXIO_PDUMPLock

******************************************************************************/
IMG_RESULT
MSVDXIO_PDUMPUnLock(
    const IMG_HANDLE  hCoreContext
)
{

    MSVDXIO_sContext  * psContext;
    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle container must be provided");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    psContext = (MSVDXIO_sContext *)hCoreContext;

    TALPDUMP_Unlock(psContext->psCommonCtx->pahMemSpace[REGION_VLRBE_REGSPACE], VDEC_BE_FE_PDUMP_SEM);

    return IMG_SUCCESS;
}

#endif

/*
******************************************************************************

 @Function              MSVDXIO_Initialise

******************************************************************************/
IMG_RESULT
MSVDXIO_Initialise(
    IMG_BOOL               bFakeMtx,
    IMG_BOOL               bPostReq,
    IMG_BOOL               bStackUsageTestReq,
    IMG_UINT32             ui32CoreNum,
    IMG_HANDLE           * hCommonCtx,
    IMG_HANDLE           * phCoreContext
)
{
    MSVDXIO_sContext  * psContext = IMG_NULL;
    IMG_UINT32          ui32Result;
    IMG_HANDLE        * pahMemSpace;
    VXDIO_sContext    * psCommonCtx = (VXDIO_sContext *)hCommonCtx;
    IMG_CHAR           acMemSpcePrefix[VXDIO_MEMSPACE_PREFIX_MAX];

#ifdef VXD_BRIDGING
    (void)gsTargetConfig; /* Avoid compiler warning. */
#endif /* VXD_BRIDGING */

    if (phCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle container must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    if (psCommonCtx == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An VXD common core context must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    psContext = IMG_MALLOC(sizeof(*psContext));
    IMG_ASSERT(psContext);
    if (psContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
            "Failed to allocate memory for MSVDXIO context");
        return IMG_ERROR_OUT_OF_MEMORY;
    }
    pahMemSpace = IMG_MALLOC(REGION_MAX*sizeof(IMG_HANDLE));
    IMG_ASSERT(pahMemSpace);
    if (pahMemSpace == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
            "Failed to allocate memory for memory spaces handles!");
        IMG_FREE(psContext);
        return IMG_ERROR_OUT_OF_MEMORY;
    }
    IMG_MEMSET(psContext, 0, sizeof(*psContext));
    *phCoreContext = psContext;
    psContext->psCommonCtx = psCommonCtx;
    psCommonCtx->pahMemSpace = pahMemSpace;
    psCommonCtx->ui32MemSpaceCount = REGION_MAX;
    IMG_MEMSET(pahMemSpace, 0, psCommonCtx->ui32MemSpaceCount*sizeof(IMG_HANDLE));

    msvdxio_GetCommonFunctions(&psCommonCtx->sCoreFuncs);

    // Create the memory space name prefix based upon the core number.
    snprintf(acMemSpcePrefix, VXDIO_MEMSPACE_PREFIX_MAX, "C%d_", ui32CoreNum);

    /* This is a simplification: setting TAL memory space handles for
       multicore is a little more involved */
    pahMemSpace[REGION_VXDSYS_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_SYS");
    pahMemSpace[REGION_VEC_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_VEC");
    pahMemSpace[REGION_DMAC_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_DMAC");
    pahMemSpace[REGION_VDMC_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_VDMC");
    pahMemSpace[REGION_VDEB_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_VDEB");
    pahMemSpace[REGION_VLR_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_VEC_RAM");
#if 0
    pahMemSpace[REGION_VXDPINTEST_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_PIN_TEST");
#endif

    if (bFakeMtx)
    {
        pahMemSpace[REGION_VXDMTX_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_MTX");
        pahMemSpace[REGION_VXDMTX_MEMSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "FAKE_REG_MSVDX_MTX");
        pahMemSpace[REGION_VXDSYS_MEMSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "FAKE_REG_MSVDX_SYS");
        pahMemSpace[REGION_VLRBE_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "FAKE_REG_MSVDX_VEC_RAM");
        pahMemSpace[REGION_VLRFE_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "FAKE_REG_MSVDX_VEC_RAM");
    }
    else
    {
        pahMemSpace[REGION_VXDMTX_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_MTX");
        pahMemSpace[REGION_VXDMTX_MEMSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_MTX");
        pahMemSpace[REGION_VXDSYS_MEMSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_SYS");
        pahMemSpace[REGION_VLRBE_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_VEC_RAM_BE");
        pahMemSpace[REGION_VLRFE_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_VEC_RAM_FE");
    }
#ifdef SECURE_DEVICE_FPGA_TEST
    pahMemSpace[REGION_VXDTEST_3_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_TEST_3");
#endif
    psContext->psCommonCtx->bFakeMtx = bFakeMtx;
    psContext->psCommonCtx->bInitialised = IMG_TRUE;
#ifdef POST_TEST
    /* update POST flag */
    psContext->psCommonCtx->bPost = bPostReq;
#endif
#ifdef STACK_USAGE_TEST
    /* update STACK USAGE TEST flag */
    psContext->psCommonCtx->bStackUsageTest = bStackUsageTestReq;
#endif

    // Initially turn the clocks on so that
    // general registers can be accessed.
    ui32Result = MSVDXIO_EnableClocks(psContext, IMG_FALSE, IMG_FALSE, IMG_FALSE);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }

#ifdef SEC_USE_REAL_FW
    if (!psContext->psCommonCtx->bFakeMtx)
    {
        /* Get MTX RAM info for this core. */
        ui32Result = msvdxio_MTXGetRamInfo(psContext, &psContext->psCommonCtx->sMTXRamInfo);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }
    }
#endif /* SEC_USE_REAL_FW */

    {
        IMG_UINT32 ui32Reg;
        IMG_UINT32  ui32MajRev;
        IMG_UINT32  ui32MinRev;
        IMG_UINT32  ui32MaintRev;

        /* Core Revision Information */
        ui32Reg = REGIO_READ_REGISTER(psContext->psCommonCtx, MSVDX_CORE, CR_MSVDX_CORE_REV);
        ui32MajRev = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CORE_REV, CR_MSVDX_MAJOR_REV);
        ui32MinRev = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CORE_REV, CR_MSVDX_MINOR_REV);
        ui32MaintRev = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CORE_REV, CR_MSVDX_MAINT_REV);

        // Use the new test register space from 4.0.3 onwards.
        if (ui32MajRev > 4 ||
            (ui32MajRev == 4 && ui32MinRev > 0) ||
            (ui32MajRev == 4 && ui32MinRev == 0 && ((IMG_INT32)(ui32MaintRev) >= 3)))
        {
            pahMemSpace[REGION_VXDTEST_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_TEST_2");
        }
        else
        {
            pahMemSpace[REGION_VXDTEST_REGSPACE] = VXDIO_GetHandle(acMemSpcePrefix, "REG_MSVDX_TEST");
        }
    }


#ifdef SECURE_MEM_ACCESS
    {
        IMG_UINT32 ui32Reg = 0;
        //Secure Memory
        REGIO_WRITE_FIELD(ui32Reg, MSVDX_TEST_3, SECURE_MEM_CTRL_0, SECURE_MEM_BOUNDARY, 0x8000);
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_TEST_3, SECURE_MEM_CTRL_0, ui32Reg);
    }
#endif


#ifdef SECURE_CPU_REG_ACCESS
    {
        IMG_UINT32 ui32Reg = 0;
        //Secure Reg Access
        REGIO_WRITE_FIELD(ui32Reg, MSVDX_TEST_3, SECURE_CPU_CTRL_0, SECURE_CPU_ENABLE, 1);
        REGIO_WRITE_REGISTER(psContext->psCommonCtx, MSVDX_TEST_3, SECURE_CPU_CTRL_0, ui32Reg);
        bSecEnable = IMG_TRUE;
    }
#endif

    // Sanity check VDEC_eVidStd.
    IMG_ASSERT(VDEC_STD_UNDEFINED == 0);
    IMG_ASSERT(VDEC_STD_MPEG2 == 1);
    IMG_ASSERT(VDEC_STD_MPEG4 == 2);
    IMG_ASSERT(VDEC_STD_H263 == 3);
    IMG_ASSERT(VDEC_STD_H264 == 4);
    IMG_ASSERT(VDEC_STD_VC1 == 5);
    IMG_ASSERT(VDEC_STD_AVS == 6);
    IMG_ASSERT(VDEC_STD_REAL == 7);
    IMG_ASSERT(VDEC_STD_JPEG == 8);
    IMG_ASSERT(VDEC_STD_VP6 == 9);
    IMG_ASSERT(VDEC_STD_VP8 == 10);
    IMG_ASSERT(VDEC_STD_SORENSON == 11);
    IMG_ASSERT(VDEC_STD_HEVC == 12);
    IMG_ASSERT(VDEC_STD_MAX == 13);

    ui32NumOfCoresInit++;

    return IMG_SUCCESS;

error:

    if(psContext)
    {
        IMG_FREE(psContext);
    }

    return ui32Result;
}

/*
******************************************************************************

 @Function              MSVDXIO_SetSecRegAccess

******************************************************************************/
IMG_VOID
MSVDXIO_SetSecRegAccess(
    const IMG_HANDLE       hCoreContext
)
{
#ifdef SECURE_CPU_REG_ACCESS
    const MSVDXIO_sContext    * psContext = (const MSVDXIO_sContext *)hCoreContext;
    IMG_UINT32                  ui32Reg;
    IMG_UINT32                  ui32Result;

    if (hCoreContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX core context handle must be provided");
        return;
    }
    IMG_UINT32  ui32SecRegVal = bSecEnable | SECURE_REG_ACCESS;
    TALREG_WriteWord32(psContext->psCommonCtx->pahMemSpace[REGION_VXDTEST_3_REGSPACE], MSVDX_TEST_3_SECURE_CPU_CTRL_0_OFFSET, ui32SecRegVal);
#endif
}


/* EOF */
