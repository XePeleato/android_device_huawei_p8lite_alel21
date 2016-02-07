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
#include "dx_pal_mem.h"
#include "cc_plat.h"
#include "mlli.h"
#include "hw_queue.h"
#include "sep_ctx.h"
#include "completion.h"
#include "dx_error.h"
#include "combined.h"
#include "cipher.h"
#include "hash.h"
#include "crys_combined.h"
#include "cc_plat.h"


/*!
 * Sets the AES core engine in the given context.
 * 
 * \param pAesCtx The AES context 
 * \param combinedMode The user combined scheme represented by 32 bits
 * \param engIdx The engine index in the combinedMode
 *
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
static int SetAesCoreEngine(
	struct sep_ctx_cipher *pAesCtx,
	CombinedMode_t combinedMode,
	int engIdx)
{
	enum sep_engine_type engType;
	CrysCombinedEngineSource_e engSrc;
	SepCipherPrivateContext_s *pAesPrivateCtx =
		(SepCipherPrivateContext_s *)pAesCtx->reserved;

	SepCombinedEnginePropsGet(combinedMode, engIdx, &engSrc, &engType);

	if (engSrc == INPUT_NULL) {
		DX_PAL_LOG_ERR("Illigal AES engine source");
		return DX_RET_NOEXEC;
	} else if (engSrc == INPUT_DIN) {
		WriteContextWord(&pAesPrivateCtx->engineCore,SEP_AES_ENGINE1);
	} else {
		WriteContextWord(&pAesPrivateCtx->engineCore,SEP_AES_ENGINE2);
	}

	return DX_RET_OK;
}

/*!
 * Translates the Combined SeP mode to Combined HW mode.
 * 
 * \param pCtx The Combined context
 * \param HwCombinedMode_t [out] The HW mode as specified in combined.h
 *
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
static int GetCombinedHwMode(struct sep_ctx_combined *pCtx, HwCombinedMode_t *mode)
{
	switch (ReadContextWord(&pCtx->mode)) {
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_MODE:
		*mode =  HW_COMBINED_DIN_TO_AES_TO_HASH_MODE;
		break;
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_AND_DOUT_MODE:
		*mode =  HW_COMBINED_DIN_TO_AES_TO_HASH_AND_DOUT_MODE;
		break;
	case SEP_COMBINED_DIN_TO_AES_AND_HASH_MODE:
		*mode =  HW_COMBINED_DIN_TO_AES_AND_HASH_MODE;
		break;
	case SEP_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE:
		*mode =  HW_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE;
		break;
	default:
		DX_PAL_LOG_ERR("Unsupported Combined Mode\n");
		*mode = HW_COMBINED_NONE;
		return DX_RET_UNSUPP_ALG_MODE;
	}

	return DX_RET_OK;
}

/*!
 * Loads the state for each given sub-context associated within combined context. 
 * This function iterates thru the sub-contexts specified in the configuration 
 * scheme and invokes the propriatery LOAD state correspondingly. 
 * 
 * \param qid 
 * \param pCtx Combined context which contains the associated sub-contexts
 * \param hashPadding Is HASH padding enabled/disabled
 *
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
static int LoadCombinedState(
	int qid,
	struct sep_ctx_combined *pCtx,
	enum HashConfig1Padding hashPadding)
{
	HwCombinedMode_t combinedHwMode;
	int engIdx;
	int drvRc;
	struct sep_ctx_generic *sub_ctx;

	drvRc = GetCombinedHwMode(pCtx, &combinedHwMode);
	if (drvRc != DX_RET_OK) {
		return drvRc;
	}
	/* load state for each given context */
	for ( engIdx = 0;
	      ((engIdx < SEP_MAX_COMBINED_ENGINES) && ((void*)ReadContextWord(&pCtx->sub_ctx[engIdx]) != NULL));
	      engIdx++ ) {

		sub_ctx = (struct sep_ctx_generic *)ReadContextWord(&pCtx->sub_ctx[engIdx]);
		switch (ReadContextWord(&sub_ctx->alg)) {
		case SEP_CRYPTO_ALG_AES:
		{
			struct sep_ctx_cipher *pAesCtx = (struct sep_ctx_cipher *)ReadContextWord(&pCtx->sub_ctx[engIdx]);
			SepCipherPrivateContext_s *pAesPrivateCtx = (SepCipherPrivateContext_s *)pAesCtx->reserved;


			/* set tunneling mode */
			switch (combinedHwMode) {
			case HW_COMBINED_DIN_TO_AES_TO_AES_AND_HASH_MODE:
			case HW_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE:
			case HW_COMBINED_DIN_TO_AES_TO_AES_TO_HASH_MODE:
				WriteContextWord(&pAesPrivateCtx->isTunnelOp,(uint32_t)TUNNEL_ON);
				WriteContextWord(&pAesPrivateCtx->tunnetDir,(uint32_t)SEP_CRYPTO_DIRECTION_DECRYPT_ENCRYPT);
				break;
			default:
				WriteContextWord(&pAesPrivateCtx->isTunnelOp, (uint32_t)TUNNEL_OFF);
			}
			
			/* select AES core engine 1/2 */
			drvRc = SetAesCoreEngine(pAesCtx, (CombinedMode_t )ReadContextWord(&pCtx->mode), engIdx);
			if (drvRc != DX_RET_OK) {
				return drvRc;
			}


			if (ReadContextWord(&pAesCtx->mode) == SEP_CIPHER_XTS) {
				/* in XTS the key must be loaded first */
				LoadCipherKey(qid, pAesCtx);
				LoadCipherState(qid, pAesCtx,0);
			} else {
				LoadCipherState(qid, pAesCtx,0);
				LoadCipherKey(qid, pAesCtx);
			}
			break;
		}
		case SEP_CRYPTO_ALG_HASH:
			LoadHashState(qid, (struct sep_ctx_hash *)ReadContextWord(&pCtx->sub_ctx[engIdx]), hashPadding);
			break;
		default:
			DX_PAL_LOG_ERR("Invalid alg");
			return DX_RET_UNSUPP_ALG;
		}
	}

	return DX_RET_OK;
}

/*!
 * Finalizes the combined operation according to the given SeP mode. 
 * This function distinguishes  
 *  
 * \param qid 
 * \param combinedSepMode Combined SeP mode as passed by the caller
 * \param pCtx Combined context
 * \param isDataToFinalize Is there's any data reminder
 *  
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
static int FinalizeCombinedOp(
	int qid,
	enum SepCombinedMode combinedSepMode,
	struct sep_ctx_combined *pCtx,
	int isDataToFinalize)
{
	HwDesc_s desc;
	int drvRc = DX_RET_OK;

	switch (combinedSepMode) {
	case SEP_COMBINED_DIN_TO_AES_AND_HASH_MODE:
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_MODE:
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_AND_DOUT_MODE:
	{
		struct sep_ctx_hash *pHashCtx = (struct sep_ctx_hash *)ReadContextWord(&pCtx->sub_ctx[1]);
		uint32_t hw_mode, DigestSize;
		DmaBuffer_s EmptyDmaBuffer;

		if (pHashCtx == NULL) {
			DX_PAL_LOG_ERR("NULL pointer for HASH context\n");
			drvRc = DX_RET_INVARG;
			goto EndWithErr;
		}

		if (isDataToFinalize) {
			drvRc = GetHashHwMode(ReadContextWord(&pHashCtx->mode), &hw_mode);
			if (drvRc != DX_RET_OK) {
				goto EndWithErr;
			}
			drvRc = GetHashHwDigestSize(ReadContextWord(&pHashCtx->mode), &DigestSize);
			if (drvRc != DX_RET_OK) {
				goto EndWithErr;
			}

			/* finalize operations with remaining data */
			HW_DESC_INIT(&desc);
			HW_DESC_SET_CIPHER_MODE(&desc, hw_mode);
			HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pHashCtx->digest, DigestSize);
			HW_DESC_SET_CIPHER_CONFIG0(&desc, HASH_DIGEST_RESULT_LITTLE_ENDIAN);
			HW_DESC_SET_CIPHER_CONFIG1(&desc, HASH_PADDING_DISABLED);
			HW_DESC_SET_CIPHER_DO(&desc, DO_NOT_PAD);
			HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
			HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
			AddHWDescSequence(qid, &desc);


#ifdef DX_CONFIG_HASH_SHA_512_SUPPORTED
			{
				uint32_t tempUint,i;
				if ((ReadContextWord(&pHashCtx->mode) == SEP_HASH_SHA512 || (ReadContextWord(&pHashCtx->mode) == SEP_HASH_SHA384))){
					WaitForSequenceCompletion();
					for( i = 0 ; i < DigestSize/sizeof(uint32_t); i++ )
					{
						if(i%2){

							tempUint = ReadContextWord(&((uint32_t*)(pHashCtx->digest))[i - 1]);
							WriteContextWord(&((uint32_t*)(pHashCtx->digest))[i - 1],ReadContextWord(&((uint32_t*)(pHashCtx->digest))[i]));
							WriteContextWord(&((uint32_t*)(pHashCtx->digest))[i],tempUint);
						}	
					}

				}
			}
#endif

		} else {
			/* finalize operations without remaining data */
			EmptyDmaBuffer.dmaBufType = DMA_BUF_SEP;
			EmptyDmaBuffer.axiNs = AXI_SECURE;
			EmptyDmaBuffer.pData = 0;
			EmptyDmaBuffer.nTables = 0;
			EmptyDmaBuffer.size = 0;

			drvRc = FinalizeHash(pHashCtx, &EmptyDmaBuffer, NULL);
			if (drvRc != DX_RET_OK) {
				goto EndWithErr;
			}
		}

		break;
	}
	case SEP_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE:
		break;
	default:
		DX_PAL_LOG_ERR("Unsupported Combined Mode\n");
		drvRc = DX_RET_UNSUPP_ALG_MODE;
		break;
	}

EndWithErr:
	return drvRc;
}


/********************************************************************************/
/********************************************************************************/
/*!! we do not implement "InitCombined" since it does not perform any operation */
/********************************************************************************/
/********************************************************************************/


/*!
 * This function is used to process a block(s) of data in combined or tunneling mode.
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessCombined(struct sep_ctx_combined *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint8_t *pInputData = NULL, *pOutputData = NULL;
	uint32_t DataInSize = 0, DataOutSize = 0;
	DmaMode_t dmaMode = NO_DMA;
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;
	uint8_t outAxiNs = pDmaOutputBuffer->axiNs;
	int nMlliTables = pDmaInputBuffer->nTables;
	const int isInplaceOp = ((pDmaInputBuffer->pData == pDmaOutputBuffer->pData) ||
				 (pDmaOutputBuffer->pData == 0));
	HwCombinedMode_t combinedHwMode = HW_COMBINED_NONE;
	HwDesc_s desc;
	struct sep_ctx_generic *sub_ctx;

	int qid = CURR_QUEUE_ID();
	int engIdx;
	int drvRc = DX_RET_OK;

	drvRc = GetCombinedHwMode(pCtx, &combinedHwMode);
	if (drvRc != DX_RET_OK) {
		return drvRc;
	}

	DX_PAL_LOG_INFO("Combined SEP mode = 0x%08x\n", (unsigned int)ReadContextWord(&pCtx->mode));
	DX_PAL_LOG_INFO("Combined HW mode = 0x%08x\n", combinedHwMode);

	/* load state for each given sub-context */
	drvRc = LoadCombinedState(qid, pCtx, HASH_PADDING_DISABLED);
	if (drvRc != DX_RET_OK) {
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
			pOutputData = (uint8_t *)pDmaOutputBuffer->pData;
		} else {
			pOutputData = pInputData;
		}

		/* not relevant */
		nMlliTables = 0;

		/* set the data size */
		DataInSize = pDmaInputBuffer->size;
		DataOutSize = pDmaOutputBuffer->size; 

		break;
	case DMA_MODE_NULL:
		pInputData = 0;
		pOutputData = 0;

		/* data processing is done */
		nMlliTables = 0;

		/* data size is meaningless in DMA-MLLI mode */
		DataInSize = 0;
		DataOutSize = 0;

		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}

	/* process the flow */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, DataInSize, QID_TO_AXI_ID(qid), inAxiNs);
	switch (combinedHwMode) {
	case HW_COMBINED_DIN_TO_AES_TO_HASH_MODE:
	case HW_COMBINED_DIN_TO_AES_TO_AES_TO_HASH_MODE:
		break;
	default:
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode, (uint32_t)pOutputData, DataOutSize, QID_TO_AXI_ID(qid), outAxiNs);
	}
	HW_DESC_SET_FLOW_MODE(&desc, combinedHwMode);
	AddHWDescSequence(qid, &desc);

	/* process each MLLI table (MLLI processing loop only) */
	while (--nMlliTables > 0) {
		/* prepare next input MLLI table in SRAM */
		PrepareNextMLLITable(qid, inAxiNs, MLLI_INPUT_TABLE);
		
		/* prepare next output MLLI table in SRAM */
		if (isInplaceOp == 0) {
			PrepareNextMLLITable(qid, outAxiNs, MLLI_OUTPUT_TABLE);
		}

		/* process the AES flow */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, DMA_MLLI, (uint32_t)pInputData, 0, QID_TO_AXI_ID(qid), inAxiNs);
		switch (combinedHwMode) {
		case HW_COMBINED_DIN_TO_AES_TO_HASH_MODE:
		case HW_COMBINED_DIN_TO_AES_TO_AES_TO_HASH_MODE:
			break;
		default:
			HW_DESC_SET_DOUT_TYPE(&desc, DMA_MLLI, (uint32_t)pOutputData, 0, QID_TO_AXI_ID(qid), outAxiNs);
		}
		HW_DESC_SET_FLOW_MODE(&desc, combinedHwMode);
		AddHWDescSequence(qid, &desc);
	}

	/* store machine state for each sub context */
	for ( engIdx = 0;
	      ((engIdx < SEP_MAX_COMBINED_ENGINES) && ((uint32_t*)ReadContextWord(&pCtx->sub_ctx[engIdx]) != NULL));
	      engIdx++ ) {
		sub_ctx = (struct sep_ctx_generic* )ReadContextWord(&pCtx->sub_ctx[engIdx]);
		switch (ReadContextWord(&sub_ctx->alg)) {
		case SEP_CRYPTO_ALG_AES:
			StoreCipherState(qid, (struct sep_ctx_cipher*)sub_ctx);
			break;
		case SEP_CRYPTO_ALG_HASH:
			StoreHashState(qid, (struct sep_ctx_hash*)sub_ctx);
			break;
		default:
			DX_PAL_LOG_ERR("Invalid Alg mode\n");
			drvRc = DX_RET_UNSUPP_ALG;
			goto EndWithErr;
		}
	}

EndWithErr:
	return drvRc;
}

/*!
 * This function is used as finish operation of Combined modes.
 * The function should be called after "ProcessCombined".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeCombined(struct sep_ctx_combined *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	HwCombinedMode_t combinedHwMode = HW_COMBINED_NONE;
	const int isInplaceOp = ((pDmaInputBuffer->pData == pDmaOutputBuffer->pData) ||
				 (pDmaOutputBuffer->pData == 0));
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;
	uint8_t outAxiNs = pDmaOutputBuffer->axiNs;
	uint8_t *pInputData = NULL, *pOutputData = NULL;
	uint32_t DataInSize = 0, DataOutSize = 0;
	uint32_t isRemainingData = 0;
	DmaMode_t dmaMode = NO_DMA;
	HwDesc_s desc;
	int qid = CURR_QUEUE_ID();
	int drvRc = DX_RET_OK;

	HW_DESC_INIT(&desc);
	
	dmaMode = DMA_BUF_TYPE_TO_MODE(pDmaInputBuffer->dmaBufType);

	/* check if we have remaining data to process */
	switch (dmaMode) {
	case DMA_MLLI:
		isRemainingData = pDmaInputBuffer->nTables;
		/* data size is meaningless in DMA-MLLI mode */
		DataInSize = 0;
		DataOutSize = 0;
		break;
	case DMA_DLLI:
	case DMA_SRAM:
		isRemainingData = (pDmaInputBuffer->size > 0) ? 1 : 0;
		DataInSize = pDmaInputBuffer->size;
		DataOutSize = pDmaOutputBuffer->size;
		break;
	case DMA_MODE_NULL:
		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}	

	drvRc = GetCombinedHwMode(pCtx, &combinedHwMode);
	if (drvRc != DX_RET_OK) {
		return drvRc;
	}
	
	/* check if there is a remainder */
	if (isRemainingData > 0) {
		if (isRemainingData > 1) {
			/* this case only apply to DMA_MLLI mode! */
			pDmaInputBuffer->nTables--;
			ProcessCombined(pCtx, pDmaInputBuffer, pDmaOutputBuffer);
			/* process the last MLLI table */
			PrepareNextMLLITable(qid, inAxiNs, MLLI_INPUT_TABLE);
			if (!isInplaceOp) {
				PrepareNextMLLITable(qid, outAxiNs, MLLI_OUTPUT_TABLE);
			}
	
			/* load state for each given sub-context */
			drvRc = LoadCombinedState(qid, pCtx, HASH_PADDING_ENABLED);
			if (drvRc != DX_RET_OK) {
				goto EndWithErr;
			}
	
			/* get the pointer of the input MLLI in SRAM */
			pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);
			if (!isInplaceOp) {
				pOutputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_OUTPUT_TABLE);
			}
		} else if (isRemainingData == 1) {
			/* load state for each given sub-context */
			drvRc = LoadCombinedState(qid, pCtx, HASH_PADDING_ENABLED);
			if (drvRc != DX_RET_OK) {
				goto EndWithErr;
			}

			/* we have a single MLLI table or remaining DMA_SEP data */
			if (dmaMode == DMA_MLLI) {
				pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);
				PrepareFirstMLLITable(qid, pDmaInputBuffer, MLLI_INPUT_TABLE);
				if (!isInplaceOp) {
					pOutputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_OUTPUT_TABLE);
					PrepareFirstMLLITable(qid, pDmaOutputBuffer, MLLI_OUTPUT_TABLE);
				} else {
					pOutputData = pInputData;
				}
			} else {
				pInputData = (uint8_t *)pDmaInputBuffer->pData;
				pOutputData = (uint8_t *)pDmaOutputBuffer->pData;
			}
		}

		/* clobber remaining HASH data */
		HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, DataInSize, QID_TO_AXI_ID(qid), inAxiNs);
		switch (combinedHwMode) {
		case HW_COMBINED_DIN_TO_AES_TO_HASH_MODE:
		case HW_COMBINED_DIN_TO_AES_TO_AES_TO_HASH_MODE:
			break;
		default:
			HW_DESC_SET_DOUT_TYPE(&desc, dmaMode, (uint32_t)pOutputData, DataOutSize, QID_TO_AXI_ID(qid), outAxiNs);
		}
		HW_DESC_SET_FLOW_MODE(&desc, combinedHwMode);
		AddHWDescSequence(qid, &desc);
	}

	return FinalizeCombinedOp(qid, ReadContextWord(&pCtx->mode), pCtx, isRemainingData);

EndWithErr:
	return drvRc;
}

