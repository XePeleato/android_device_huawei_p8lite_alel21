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

#include "bsp.h"
#include "dx_error.h"
#include "cc_plat.h"
#include "mlli.h"
#include "dma_buffer.h"
#include "hw_queue.h"
#include "sep_ctx.h"
#include "rc4.h"


/*!
 * This function is used to initialize the RC4 machine to perform the RC4
 * operations. This should be the first function called. It initializes
 * the permutation in the "S" array.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int InitRc4(struct sep_ctx_rc4 *pCtx)
{
	HwDesc_s desc;
	int qid = CURR_QUEUE_ID(); /* qid is stored in pxTaskTag field */

	/* check key size */
	if (ReadContextWord(&pCtx->key_size) < SEP_RC4_KEY_SIZE_MIN) {
		DX_PAL_LOG_ERR("RC4 key size MUST be >= 1\n");
		return DX_RET_INVARG;

	}
	if (ReadContextWord(&pCtx->key_size) > SEP_RC4_KEY_SIZE_MAX) {
		DX_PAL_LOG_ERR("RC4 key size MUST be <= 20\n");
		return DX_RET_INVARG;
	}

	/* load key -done only once per session */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->key, ReadContextWord(&pCtx->key_size));
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_RC4);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	AddHWDescSequence(qid, &desc);

	/* store state -this will ensure "S" array was created and stored in context */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->state, SEP_RC4_STATE_SIZE);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	HW_DESC_SET_FLOW_MODE(&desc, S_RC4_to_DOUT);
	AddHWDescSequence(qid, &desc);

	return DX_RET_OK;
}

/*!
 * This function is used to process a block(s) of data on RC4 machine.
 * This function may be called after the "InitRc4" function.
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessRc4(struct sep_ctx_rc4 *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint8_t *pInputData = NULL, *pOutputData = NULL;
	uint32_t DataInSize = 0, DataOutSize = 0;
	int nMlliTables = pDmaInputBuffer->nTables;
	const int isInplaceOp = (pDmaInputBuffer->pData == pDmaOutputBuffer->pData);
	HwDesc_s desc;
	DmaMode_t dmaMode = NO_DMA;
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;
	uint8_t outAxiNs = pDmaOutputBuffer->axiNs;
	int qid = CURR_QUEUE_ID(); /* qid is stored in pxTaskTag field */
	int drvRc = DX_RET_OK;

	/* load state */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
	HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->state, SEP_RC4_STATE_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_RC4);
	AddHWDescSequence(qid, &desc);


	/* set the input/output pointers according to the DMA mode */
	if (pDmaInputBuffer->dmaBufType != pDmaOutputBuffer->dmaBufType) {
		DX_PAL_LOG_ERR("Inconsistent DMA mode for in/out buffers\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}

	dmaMode = DMA_BUF_TYPE_TO_MODE(pDmaInputBuffer->dmaBufType);

	switch (dmaMode) {
	case DMA_MLLI:
		pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);
		PrepareFirstMLLITable(qid, pDmaInputBuffer, MLLI_INPUT_TABLE);

		/* get OUT MLLI tables pointer in SRAM (if not inplace operation) */
		if (isInplaceOp == 0) {
			pOutputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_OUTPUT_TABLE);
			PrepareFirstMLLITable(qid, pDmaOutputBuffer, MLLI_OUTPUT_TABLE);
		} else {
			pOutputData = pInputData;
		}

		/* data size is meaningless in DMA-MLLI mode */
		DataInSize = 0;
		DataOutSize = 0;

		break;
	case DMA_DLLI:
	case DMA_SRAM:
		pInputData = (uint8_t *)pDmaInputBuffer->pData;
		if (isInplaceOp == 0) {
			pOutputData  = (uint8_t *)pDmaOutputBuffer->pData;
		} else {
			pOutputData = pInputData;
		}

		/* data processing is done */
		nMlliTables = 0;		

		/* set the data size */
		DataInSize = pDmaInputBuffer->size;
		DataOutSize = pDmaOutputBuffer->size; 

		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}

	/* process the RC4 flow */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, DataInSize, QID_TO_AXI_ID(qid), inAxiNs);
	HW_DESC_SET_DOUT_TYPE(&desc, dmaMode, (uint32_t)pOutputData, DataOutSize, QID_TO_AXI_ID(qid), outAxiNs);
	HW_DESC_SET_FLOW_MODE(&desc, DIN_RC4_DOUT);
	AddHWDescSequence(qid, &desc);

	/* process the rest of MLLI tables (MLLI processing loop only) */
	while (--nMlliTables > 0) {

		/* prepare next input MLLI table in SRAM */
		PrepareNextMLLITable(qid, inAxiNs, MLLI_INPUT_TABLE);

		/* prepare next output MLLI table in SRAM */
		if (isInplaceOp == 0) {
			PrepareNextMLLITable(qid, outAxiNs, MLLI_OUTPUT_TABLE);
		}

		/* process the RC4 flow */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, DMA_MLLI, (uint32_t)pInputData, 0, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_TYPE(&desc, DMA_MLLI, (uint32_t)pOutputData, 0, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_RC4_DOUT);
		AddHWDescSequence(qid, &desc);
	}

	/* store state */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->state, SEP_RC4_STATE_SIZE);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	HW_DESC_SET_FLOW_MODE(&desc, S_RC4_to_DOUT);
	AddHWDescSequence(qid, &desc);

EndWithErr:
	return drvRc;
}

/*!
 * This function is used as finish the RC4 operation.
 * The function may either be called after "InitRc4" or "ProcessRc4".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeRc4(struct sep_ctx_rc4 *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint32_t isRemainingData = 0;
	int drvRc = DX_RET_OK;

	/* check if we have remaining data to process */
	switch (pDmaInputBuffer->dmaBufType) {
	case DMA_BUF_MLLI_IN_HOST:
		isRemainingData = pDmaInputBuffer->nTables;
		break;
	case DMA_BUF_SEP:
	case DMA_BUF_DLLI:
		isRemainingData = (pDmaInputBuffer->size > 0) ? 1 : 0;
		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}	
	
	if (isRemainingData) {
		/* process all tables and get state from the RC4 machine */
		drvRc = ProcessRc4(pCtx, pDmaInputBuffer, pDmaOutputBuffer);
	}

EndWithErr:
	return drvRc;
}

