/*******************************************************************  
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents.                                            *
* Use of this Software as part of or with the Discretix CryptoCell *
* or Packet Engine products is governed by the products'           *
* commercial end user license agreement ("EULA").                  *
* It is possible that copies of this Software might be distributed *
* under some type of GNU General Public License ("GPL").           *
* Notwithstanding any such distribution under the terms of GPL,    *
* GPL does not govern the use of this Software as part of or with  *
* the Discretix CryptoCell or Packet Engine products, for which a  *
* EULA is required.                                                *
* If the product's EULA allows any copy or reproduction of this    *
* Software, then such copy or reproduction must include this       *
* Copyright Notice as well as any other notices provided           *
* thereunder.                                                      *
********************************************************************/
#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_SYM_DRIVER

#include "dx_pal_log.h"
#include "bypass.h"
#include "dx_macros.h"
#include "hw_queue_defs.h"
#include "hw_queue.h"
#include "dx_error.h"

/******************************************************************************
*				PRIVATE FUNCTIONS
******************************************************************************/

/*!
 * Copy data buffer indirectly using CC HW descriptors.
 * 
 * \param inType DMA type of the source buffer.
 * \param inAddr Input address of the source buffer, must be word aligned.
 * \param inSize Size in octets of the source buffer, must be multiple of word.
 * \param inAxiNs The AXI bus secure mode of the source buffer.
 * \param outType DMA type of the source buffer.
 * \param outAddr Output address of the destination buffer, must be word aligned.
 * \param outSize Size in octets of the destination buffer, must be multiple of word.
 * \param outAxiNs The AXI bus secure mode of the destination buffer.
 */
static void DescBypass(
	DmaMode_t inType, 
	uint32_t  inAddr, 
	uint32_t inSize,
	uint32_t inAxiNs,
	DmaMode_t outType, 
	uint32_t outAddr,
	uint32_t outSize,
	uint32_t outAxiNs )
{
	HwDesc_s desc;
	
	/* Execute BYPASS operation */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_TYPE(&desc, inType, inAddr, inSize, QID_TO_AXI_ID(NO_OS_QUEUE_ID), inAxiNs);
	HW_DESC_SET_DOUT_TYPE(&desc, outType, outAddr, outSize, QID_TO_AXI_ID(NO_OS_QUEUE_ID), outAxiNs);
	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
	AddHWDescSequence(NO_OS_QUEUE_ID, &desc);
}

/*!
 * Maps a given address DMA type to a BYPASS operation type.
 * 
 * \param dmaType The "addr" DMA type.
 * \param addr The address points to the data buffer.
 * 
 * \return Bypass_t BYPASS operation type.
 */
static Bypass_t GetBypassType(DmaBufType_t dmaType, uint32_t addr)
{
	switch (dmaType) {
	case DMA_BUF_DLLI:
		return BYPASS_DLLI;
	case DMA_BUF_SEP:
		/* [FIXME] enable this comment when IS_VALID_SRAM_ADDR macro will be fixed */
		/*if (IS_VALID_SRAM_ADDR(addr)) {
			return BYPASS_SRAM;
		}*/
		return BYPASS_SRAM;
	default:
		return BYPASS_MAX;
	}
}

/*!
 * Memory copy using HW engines
 *  
 *  reserved [unused]
 *  pDmaInputBuffer [in] -A structure which represents the DMA input buffer.
 *  pDmaOutputBuffer [in/out] -A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessBypass(struct sep_ctx_generic *reserved, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	Bypass_t dmaTypeIn, dmaTypeOut;
	int drvRc = DX_RET_OK;

	dmaTypeIn = GetBypassType(pDmaInputBuffer->dmaBufType, pDmaInputBuffer->pData);
	dmaTypeOut = GetBypassType(pDmaOutputBuffer->dmaBufType, pDmaOutputBuffer->pData);
	
	if ((dmaTypeIn == BYPASS_MAX) || (dmaTypeOut == BYPASS_MAX)) {
		DX_PAL_LOG_ERR("Invalid din/dout memory type\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}

	switch (dmaTypeIn) {
	case BYPASS_SRAM:
		switch (dmaTypeOut) {
		case BYPASS_DLLI:
			if (IS_ALIGNED(pDmaInputBuffer->pData, sizeof(uint32_t)) ||
			    IS_MULT(pDmaInputBuffer->size, sizeof(uint32_t))) { 
				DescBypass(
					DMA_SRAM, 
					pDmaInputBuffer->pData, 
					pDmaInputBuffer->size,
					pDmaInputBuffer->axiNs,
					DMA_DLLI, 
					pDmaOutputBuffer->pData,
					pDmaOutputBuffer->size,
					pDmaOutputBuffer->axiNs);
			} else {
				DX_PAL_LOG_ERR("Bad address or bad size. SRAM to DLLI copy -Input address %xl with %ul B\n",
					pDmaInputBuffer->pData, pDmaInputBuffer->size);
				drvRc = DX_RET_INVARG;
				goto EndWithErr;
			}
			break;
		default:
			DX_PAL_LOG_ERR("Invalid BYPASS mode\n");
			drvRc = DX_RET_UNSUPP_ALG_MODE;
			goto EndWithErr;
		}
		break;
	case BYPASS_DLLI:
		switch (dmaTypeOut) {
		case BYPASS_SRAM:
			if (IS_ALIGNED(pDmaInputBuffer->pData, sizeof(uint32_t)) ||
			    IS_MULT(pDmaInputBuffer->size, sizeof(uint32_t))) { 
				DescBypass(
					DMA_DLLI, 
					pDmaInputBuffer->pData, 
					pDmaInputBuffer->size,
					pDmaInputBuffer->axiNs,
					DMA_SRAM, 
					pDmaOutputBuffer->pData,
					pDmaOutputBuffer->size,
					pDmaOutputBuffer->axiNs);
			} else {
				DX_PAL_LOG_ERR("Bad address or bad size. SRAM to DLLI copy -Input address %xl with %ul B\n",
					pDmaInputBuffer->pData, pDmaInputBuffer->size);
				drvRc = DX_RET_INVARG;
				goto EndWithErr;
			}
			break;
		case BYPASS_DLLI:
			DescBypass(
				    DMA_BUF_TYPE_TO_MODE(pDmaInputBuffer->dmaBufType), 
				    pDmaInputBuffer->pData, 
				    pDmaInputBuffer->size,
				    pDmaInputBuffer->axiNs,
				    DMA_BUF_TYPE_TO_MODE(pDmaOutputBuffer->dmaBufType), 
				    pDmaOutputBuffer->pData,
				    pDmaOutputBuffer->size,
				    pDmaOutputBuffer->axiNs);
			break;
		default:
			DX_PAL_LOG_ERR("Invalid BYPASS mode\n");
			drvRc = DX_RET_UNSUPP_ALG_MODE;
			goto EndWithErr;
		}
		break;
	default:
		DX_PAL_LOG_ERR("Invalid BYPASS mode\n");
		drvRc = DX_RET_UNSUPP_ALG_MODE;
		break;
	}

EndWithErr:
	return drvRc;
}

