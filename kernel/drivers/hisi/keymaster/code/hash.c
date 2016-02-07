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

#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "sym_crypto_driver.h"
#include "dx_error.h"
#include "cc_plat.h"
#include "bsp.h"
#include "mlli.h"
#include "hw_queue.h"
#include "sep_ctx.h"
#include "hash_defs.h"
#include "hash.h"
#include "completion.h"


DX_PAL_COMPILER_ASSERT(sizeof(struct sep_ctx_hash)==SEP_CTX_SIZE,"sep_ctx_hash is larger than 128 bytes!");

/******************************************************************************
*				GLOBALS
******************************************************************************/
const uint32_t gLarvalSha1Digest[] DX_SRAM_CONST = { HASH_LARVAL_SHA1 };
const uint32_t gLarvalSha224Digest[] DX_SRAM_CONST = { HASH_LARVAL_SHA224 };
const uint32_t gLarvalSha256Digest[] DX_SRAM_CONST = { HASH_LARVAL_SHA256 };
#ifdef DX_CONFIG_HASH_SHA_512_SUPPORTED
const uint32_t gLarvalSha384Digest[] DX_SRAM_CONST = { HASH_LARVAL_SHA384 };
const uint32_t gLarvalSha512Digest[] DX_SRAM_CONST = { HASH_LARVAL_SHA512 };
#endif
const uint32_t gOpadCurrentLength[] DX_SRAM_CONST = {OPAD_CURRENT_LENGTH};


/* Real expected size */
const uint32_t gHashDigestSize[SEP_HASH_MODE_NUM] = {SEP_SHA1_DIGEST_SIZE, SEP_SHA256_DIGEST_SIZE, SEP_SHA224_DIGEST_SIZE, SEP_SHA512_DIGEST_SIZE, SEP_SHA384_DIGEST_SIZE};
/* SHA224 is processed as SHA256! */
const uint32_t gHashHwDigestSize[SEP_HASH_MODE_NUM] = {SEP_SHA1_DIGEST_SIZE, SEP_SHA256_DIGEST_SIZE, SEP_SHA256_DIGEST_SIZE, SEP_SHA512_DIGEST_SIZE,  SEP_SHA512_DIGEST_SIZE};
/*from the HW side, HASH512 and HASH384 are the same*/
const uint32_t gHashHwMode[SEP_HASH_MODE_NUM] = {SEP_HASH_HW_SHA1, SEP_HASH_HW_SHA256, SEP_HASH_HW_SHA256,SEP_HASH_HW_SHA512,SEP_HASH_HW_SHA512};


/*!
 * Translate Hash mode to hardware specific Hash mode.
 * 
 * \param mode Hash mode
 * \param hwMode [out] A pointer to the hash mode return value
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int GetHashHwMode(const enum sep_hash_mode mode, uint32_t *hwMode)
{
	if (mode >= SEP_HASH_MODE_NUM) {
		DX_PAL_LOG_ERR("Unsupported hash mode");
		*hwMode = SEP_HASH_NULL;
		return DX_RET_UNSUPP_ALG_MODE;
	}

	*hwMode = gHashHwMode[mode];
	return DX_RET_OK;
}

/*!
 * Get Hash digest size in bytes.
 * 
 * \param mode Hash mode
 * \param digestSize [out] A pointer to the digest size return value
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int GetHashDigestSize(const enum sep_hash_mode mode, uint32_t *digestSize)
{
	if (mode >= SEP_HASH_MODE_NUM) {
		DX_PAL_LOG_ERR("Unsupported hash mode");
		*digestSize = 0;
		return DX_RET_UNSUPP_ALG_MODE;
	}

	*digestSize = gHashDigestSize[mode];
	return DX_RET_OK;
}

/*!
 * Get hardware digest size (HW specific) in bytes.
 * 
 * \param mode Hash mode
 * \param hwDigestSize [out] A pointer to the digest size return value
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int GetHashHwDigestSize(const enum sep_hash_mode mode, uint32_t *hwDigestSize)
{
	if (mode >= SEP_HASH_MODE_NUM) {
		DX_PAL_LOG_ERR("Unsupported hash mode");
		*hwDigestSize = 0;
		return DX_RET_UNSUPP_ALG_MODE;
	}

	*hwDigestSize = gHashHwDigestSize[mode];
	return DX_RET_OK;
}

/*!
 * Get Hash block size in bytes.
 * 
 * \param mode Hash mode
 * \param blockSize [out] A pointer to the hash block size return value
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int GetHashBlockSize(const enum sep_hash_mode mode, uint32_t *blockSize)
{
	if (mode >= SEP_HASH_MODE_NUM) {
		DX_PAL_LOG_ERR("Unsupported hash mode");
		*blockSize = 0;
		return DX_RET_UNSUPP_ALG_MODE;
	}
	if (mode <= SEP_HASH_SHA224)
		*blockSize = SEP_SHA1_224_256_BLOCK_SIZE;
	else
		*blockSize = SEP_SHA512_BLOCK_SIZE;
	return DX_RET_OK;
}

/*!
 * Loads the hash digest and hash length to the Hash HW machine.
 * 
 * \param qid 
 * \param pCtx Hash context
 * \param paddingSelection enable/disable Hash block padding by the Hash machine,
 *	  should be either HASH_PADDING_DISABLED or HASH_PADDING_ENABLED.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int LoadHashState(int qid, struct sep_ctx_hash *pCtx, enum HashConfig1Padding paddingSelection)
{
	HwDesc_s desc;
	SepHashPrivateContext_s *PrivateContext = (SepHashPrivateContext_s *)pCtx;
	struct sep_ctx_hmac *pCtxHmac = (struct sep_ctx_hmac *)pCtx;
	uint32_t hw_mode;
	uint32_t DigestSize;
	uint32_t tmpSrc = (uint32_t)pCtx->digest;
	int drvRc = DX_RET_OK;

	drvRc = GetHashHwMode(ReadContextWord(&pCtx->mode), &hw_mode);
	if (drvRc != DX_RET_OK) {
		return drvRc; 
	}

	/* SHA224 uses SHA256 HW mode with different init. val. */
	drvRc = GetHashHwDigestSize(ReadContextWord(&pCtx->mode), &DigestSize);
	if (drvRc != DX_RET_OK) {
		return drvRc; 
	}
    
	/* load intermediate hash digest */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, hw_mode);

	if (ReadContextWord(&PrivateContext->hmacFinalization) == 1) {
		tmpSrc = (uint32_t)pCtxHmac->k0;
	} 

	HW_DESC_SET_STATE_DIN_PARAM(&desc, tmpSrc, DigestSize);

	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
	AddHWDescSequence(qid, &desc);

	/* load the hash current length, should be greater than zero */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, hw_mode);
	HW_DESC_SET_CIPHER_CONFIG1(&desc, paddingSelection);
	HW_DESC_SET_CIPHER_DO(&desc, DO_NOT_PAD);

	tmpSrc = (uint32_t)PrivateContext->CurrentDigestedLength;
	/* The global array is used to set the HASH current length for HMAC finalization */
	if (ReadContextWord(&PrivateContext->hmacFinalization) == 1) {
#ifdef DX_CC_SEP
		tmpSrc = (uint32_t)gOpadCurrentLength;
#else
		HwDesc_s tdesc;
		uint32_t blockSize;

		/* In non SEP products the OPAD digest length constant is not in the SRAM     */
		/* and it might be non contiguous. In order to overcome this problem the FW   */
		/* copies the values into the CurrentDigestLength field. The coping operation */
		/* must be done with constant descriptors to keep the asynchronious mode working */
		HW_DESC_INIT(&tdesc);
		/*clear the current digest */
		HW_DESC_SET_DIN_CONST(&tdesc, 0x00, sizeof(PrivateContext->CurrentDigestedLength));
		HW_DESC_SET_STATE_DOUT_PARAM(&tdesc, tmpSrc, sizeof(PrivateContext->CurrentDigestedLength));
		AddHWDescSequence(qid, &tdesc);

		/* set the current length */
		HW_DESC_INIT(&tdesc);
		/*clear the current digest */
		GetHashBlockSize(ReadContextWord(&pCtx->mode), &blockSize);
		HW_DESC_SET_DIN_CONST(&tdesc, blockSize, sizeof(uint32_t));
		HW_DESC_SET_STATE_DOUT_PARAM(&tdesc, tmpSrc, sizeof(uint32_t));
		AddHWDescSequence(qid, &tdesc);
#endif
	}

	HW_DESC_SET_STATE_DIN_PARAM(&desc, tmpSrc, sizeof(PrivateContext->CurrentDigestedLength));
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	AddHWDescSequence(qid, &desc);

	return drvRc;
}

/*!
 * Writes the hash digest and hash length back to the Hash context.
 * 
 * \param qid 
 * \param pCtx Hash context
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int StoreHashState(int qid, struct sep_ctx_hash *pCtx)
{
	HwDesc_s desc;
	SepHashPrivateContext_s *PrivateContext = (SepHashPrivateContext_s *)pCtx;
	uint32_t hw_mode;
	uint32_t DigestSize;
	int drvRc = DX_RET_OK;

	drvRc = GetHashHwMode(ReadContextWord(&pCtx->mode), &hw_mode);
	if (drvRc != DX_RET_OK) {
		return drvRc; 
	}

	/* SHA224 uses SHA256 HW mode with different init. val. */
	drvRc = GetHashHwDigestSize(ReadContextWord(&pCtx->mode), &DigestSize);
	if (drvRc != DX_RET_OK) {
		return drvRc; 
	}

	/* store the hash digest result in the context */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, hw_mode);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->digest, DigestSize);
	HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	AddHWDescSequence(qid, &desc);

	/* store current hash length in the private context */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, hw_mode);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)PrivateContext->CurrentDigestedLength, sizeof(PrivateContext->CurrentDigestedLength));
	HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE1);
	AddHWDescSequence(qid, &desc);

	return drvRc;
}


/******************************************************************************
*				FUNCTIONS
******************************************************************************/

/*!
 * This function is used to initialize the HASH machine to perform the 
 * HASH operations. This should be the first function called.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 *  
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int InitHash(struct sep_ctx_hash *pCtx)
{
	SepHashPrivateContext_s *PrivateContext = (SepHashPrivateContext_s *)pCtx;

	/* copy the hash initial digest to the user context */
	switch(ReadContextWord(&pCtx->mode)) {
	case SEP_HASH_SHA1:
		 WriteContextField(pCtx->digest, gLarvalSha1Digest, SEP_SHA1_DIGEST_SIZE);
		break;
	case SEP_HASH_SHA224:
		WriteContextField(pCtx->digest, gLarvalSha224Digest, SEP_SHA256_DIGEST_SIZE);
		break;
	case SEP_HASH_SHA256:
		WriteContextField(pCtx->digest, gLarvalSha256Digest, SEP_SHA256_DIGEST_SIZE);
		break;
#ifdef DX_CONFIG_HASH_SHA_512_SUPPORTED
	case SEP_HASH_SHA384:
		WriteContextField(pCtx->digest, gLarvalSha384Digest, SEP_SHA512_DIGEST_SIZE);
		break;
	case SEP_HASH_SHA512:
		WriteContextField(pCtx->digest, gLarvalSha512Digest, SEP_SHA512_DIGEST_SIZE);
		break;
#endif
	default:
		DX_PAL_LOG_ERR("Unsupported hash mode %d\n", ReadContextWord(&pCtx->mode));
		return DX_RET_UNSUPP_ALG_MODE;
	}

	/* clear hash length and load it to the hash machine -we're starting a new transaction */
	ClearCtxField(PrivateContext->CurrentDigestedLength, sizeof(PrivateContext->CurrentDigestedLength));
	WriteContextWord(&PrivateContext->dataCompleted,0);
	WriteContextWord(&PrivateContext->hmacFinalization,0);

	return DX_RET_OK;
}

/*!
 * This function is used to process a block(s) of data on HASH machine.
 * It accepts an input data aligned to hash block size, any reminder which is not
 * aligned should be passed on calling to "FinalizeHash".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessHash(struct sep_ctx_hash *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint8_t *pInputData = NULL;
	HwDesc_s desc;
	uint32_t DataInSize = 0;
	DmaMode_t dmaMode = NO_DMA;
	int nMlliTables = pDmaInputBuffer->nTables;
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;
	int qid = CURR_QUEUE_ID(); /* qid is stored in pxTaskTag field */
	int drvRc = DX_RET_OK;


	HW_DESC_INIT(&desc);

	/* load hash length and digest */
	drvRc = LoadHashState(qid, pCtx, HASH_PADDING_DISABLED);
	if (drvRc != DX_RET_OK) {
		goto EndWithErr; 
	}

	dmaMode = DMA_BUF_TYPE_TO_MODE(pDmaInputBuffer->dmaBufType);

	/* set the input pointer according to the DMA mode */
	switch (dmaMode) {
	case DMA_MLLI:
		pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);
		PrepareFirstMLLITable(qid, pDmaInputBuffer, MLLI_INPUT_TABLE);

		/* data size is meaningless in DMA-MLLI mode */
		DataInSize = 0;

		break;
	case DMA_DLLI:
	case DMA_SRAM:
		pInputData = (uint8_t *)pDmaInputBuffer->pData;

		/* data processing is done */
		nMlliTables = 0;

		/* set the data size */
		DataInSize = pDmaInputBuffer->size;

		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}

	/* process the HASH flow */
	HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, DataInSize, QID_TO_AXI_ID(qid), inAxiNs);
	HW_DESC_SET_FLOW_MODE(&desc, DIN_HASH);
	AddHWDescSequence(qid, &desc);

	/* process the rest of MLLI tables (MLLI processing loop only) */
	while (--nMlliTables > 0) {

		/* prepare next input MLLI table in SRAM */
		PrepareNextMLLITable(qid, inAxiNs, MLLI_INPUT_TABLE);

		/* process the HASH flow */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, DMA_MLLI, (uint32_t)pInputData, 0, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_HASH);
		AddHWDescSequence(qid, &desc);
	}

	/* write back digest and hash length */
	StoreHashState(qid, pCtx);

EndWithErr:
	return drvRc;
}

/*!
 * This function is used as finish operation of the HASH machine.
 * The function may either be called after "InitHash" or "ProcessHash".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeHash(struct sep_ctx_hash *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	HwDesc_s desc;
	uint32_t isRemainingData = 0;
	uint32_t DataInSize = 0;
	DmaMode_t dmaMode = NO_DMA;
	uint8_t *pInputData = NULL;
	uint32_t hw_mode;

	uint32_t DigestSize;
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;

	/* qid is stored in pxTaskTag field */
	int qid = CURR_QUEUE_ID();
	int drvRc = DX_RET_OK;

	HW_DESC_INIT(&desc);

	drvRc = GetHashHwMode(ReadContextWord(&pCtx->mode), &hw_mode);
	if (drvRc != DX_RET_OK) {
		return drvRc; 
	}

	/* SHA224 uses SHA256 HW mode with different init. val. */
	/*same for SHA384 with SHA512*/ 
	drvRc = GetHashHwDigestSize(ReadContextWord(&pCtx->mode), &DigestSize);
	if (drvRc != DX_RET_OK) {
		goto EndWithErr; 
	}

	dmaMode = DMA_BUF_TYPE_TO_MODE(pDmaInputBuffer->dmaBufType);

	/* check if we have remaining data to process */
	switch (dmaMode) {
	case DMA_MLLI:
		isRemainingData = pDmaInputBuffer->nTables;
		DataInSize = 0;
		break;
	case DMA_DLLI:
	case DMA_SRAM:
		isRemainingData = (pDmaInputBuffer->size > 0) ? 1 : 0;
		DataInSize = pDmaInputBuffer->size;
		break;
	case DMA_MODE_NULL:
		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}	

	/* check if there is a remainder */
	if (isRemainingData > 1) {
		/* this case only apply to DMA_MLLI mode! */
		pDmaInputBuffer->nTables--;
		ProcessHash(pCtx, pDmaInputBuffer, NULL);

		/* process the last MLLI table */
		PrepareNextMLLITable(qid, inAxiNs, MLLI_INPUT_TABLE);

		/* load hash length and digest */
		drvRc = LoadHashState(qid, pCtx, HASH_PADDING_ENABLED);
		if (drvRc != DX_RET_OK) {
			goto EndWithErr; 
		}

		/* get the pointer of the input MLLI in SRAM */
		pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);

		/* clobber remaining HASH data */
		HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, 0, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_HASH);
		AddHWDescSequence(qid, &desc);
	} else if (isRemainingData == 1) {
		/* load hash length and digest */
		drvRc = LoadHashState(qid, pCtx, HASH_PADDING_ENABLED);
		if (drvRc != DX_RET_OK) {
			goto EndWithErr; 
		}

		/* we have a single MLLI table */
		if (dmaMode == DMA_MLLI) {
			pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);
			PrepareFirstMLLITable(qid, pDmaInputBuffer, MLLI_INPUT_TABLE);
		} else {
			pInputData = (uint8_t *)pDmaInputBuffer->pData;

			//* check sram!
		}

		/* clobber remaining HASH data */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, DataInSize, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_HASH);
		AddHWDescSequence(qid, &desc);
	} else {
		/* (isRemainingData == 0) */

		SepHashPrivateContext_s *PrivateContext = (SepHashPrivateContext_s *)pCtx;
		/* load hash length and digest */
		drvRc = LoadHashState(qid, pCtx, HASH_PADDING_DISABLED);
		if (drvRc != DX_RET_OK) {
			goto EndWithErr; 
		}

		/* Workaround: do-pad must be enabled only when writing current length to HW */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_CIPHER_MODE(&desc, hw_mode);
		HW_DESC_SET_CIPHER_CONFIG1(&desc, HASH_PADDING_DISABLED);
		HW_DESC_SET_CIPHER_DO(&desc, DO_PAD);
		HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)PrivateContext->CurrentDigestedLength,
				sizeof(PrivateContext->CurrentDigestedLength));
		HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE1);
		AddHWDescSequence(qid, &desc);
	}



	/* store the hash digest result in the context */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, hw_mode);

	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->digest, DigestSize);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, HASH_DIGEST_RESULT_LITTLE_ENDIAN);
	HW_DESC_SET_CIPHER_CONFIG1(&desc, HASH_PADDING_DISABLED);
	HW_DESC_SET_CIPHER_DO(&desc, DO_NOT_PAD);
	HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	AddHWDescSequence(qid, &desc);
#ifdef DX_CONFIG_HASH_SHA_512_SUPPORTED
{
	uint32_t tempUint,i;
	if ((ReadContextWord(&pCtx->mode) == SEP_HASH_SHA512 || (ReadContextWord(&pCtx->mode) == SEP_HASH_SHA384))){
		WaitForSequenceCompletion();
		for( i = 0 ; i < DigestSize/sizeof(uint32_t); i++ )
		{
			if(i%2){

				tempUint = ReadContextWord(&((uint32_t*)(pCtx->digest))[i - 1]);
				WriteContextWord(&((uint32_t*)(pCtx->digest))[i - 1],ReadContextWord(&((uint32_t*)(pCtx->digest))[i]));
				WriteContextWord(&((uint32_t*)(pCtx->digest))[i],tempUint);
			}
		}

	}
}
#endif


EndWithErr:
	return drvRc;
}

