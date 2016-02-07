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
#include "dx_pal_log.h"
#include "cc_plat.h"
#include "mlli.h"
#include "sep_ctx.h"
#include "hw_queue.h"
#include "dx_error.h"
#include "cipher.h"
#include "dx_hal_plat.h"
#include "sym_crypto_driver.h"
#ifdef DX_CC_SEP
#include "timing.h"
#else
#define TIMING_MARK(index)
#endif


DX_PAL_COMPILER_ASSERT(sizeof(struct sep_ctx_cipher)==SEP_CTX_SIZE,"sep_ctx_cipher is larger than 128 bytes!");

/******************************************************************************
*				PRIVATE FUNCTIONS
******************************************************************************/

void LoadCipherState(int qid, struct sep_ctx_cipher *pCtx,uint8_t is_zero_iv)
{
	SepCipherPrivateContext_s *pAesPrivateCtx = (SepCipherPrivateContext_s *)pCtx->reserved;
	HwDesc_s desc;
	uint32_t block_size;

	HW_DESC_INIT(&desc);

	switch (ReadContextWord(&pCtx->mode)) {
	case SEP_CIPHER_ECB:
		return;
	case SEP_CIPHER_CTR:
	case SEP_CIPHER_XTS:
	case SEP_CIPHER_OFB:
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE1);
		break;
	case SEP_CIPHER_CMAC:
		HW_DESC_SET_CIPHER_DO(&desc, AES_CMAC_INIT);
		/* fall through */
	default:
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
	}

	HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pCtx->mode));
	if (ReadContextWord(&pCtx->alg) == SEP_CRYPTO_ALG_AES) {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pAesPrivateCtx->isTunnelOp)?ReadContextWord(&pAesPrivateCtx->tunnetDir):ReadContextWord(&pCtx->direction));
		block_size = SEP_AES_BLOCK_SIZE;
		HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
		HW_DESC_SET_CIPHER_CONFIG1(&desc, ReadContextWord(&pAesPrivateCtx->isTunnelOp));
		if (ReadContextWord(&pCtx->crypto_key_type) == DX_XOR_HDCP_KEY) {
			HW_DESC_SET_AES_XOR_CRYPTO_KEY(&desc);
		}
		if (ReadContextWord(&pAesPrivateCtx->engineCore) == SEP_AES_ENGINE2) {
			HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES2);
		} else {
			HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
		}
	} else { /* DES */
		block_size = SEP_DES_IV_SIZE;
		HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_DES);
		HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pCtx->direction));
	}
	/*if is_zero_iv use ZeroBlock as IV*/
	if (is_zero_iv ==1 ){
 	    HW_DESC_SET_DIN_CONST(&desc, 0, block_size);
	} else {
		HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->block_state, block_size);
	}
	AddHWDescSequence(qid, &desc);

	#ifdef DX_CONFIG_HW_RESET_CONST_SUPPORT
	    HW_DESC_INIT(&desc);
	    HW_DESC_RESET_CONST_INPUT(&desc);
	    AddHWDescSequence(qid, &desc);
	#endif

}

void StoreCipherState(int qid, struct sep_ctx_cipher *pCtx)
{
	SepCipherPrivateContext_s *pAesPrivateCtx = (SepCipherPrivateContext_s *)pCtx->reserved;
	HwDesc_s desc;
	uint32_t block_size;

	if (ReadContextWord(&pCtx->mode) == SEP_CIPHER_ECB) {
		return;
	}

	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pCtx->mode));
	switch (ReadContextWord(&pCtx->mode)) {
	case SEP_CIPHER_XTS:
	case SEP_CIPHER_CTR:
	case SEP_CIPHER_OFB:
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE1);
		break;
	default:
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	}

	if (ReadContextWord(&pCtx->alg) == SEP_CRYPTO_ALG_AES) {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pAesPrivateCtx->isTunnelOp)?ReadContextWord(&pAesPrivateCtx->tunnetDir):ReadContextWord(&pCtx->direction));
		block_size = SEP_AES_BLOCK_SIZE;
		HW_DESC_SET_CIPHER_CONFIG1(&desc, ReadContextWord(&pAesPrivateCtx->isTunnelOp));
	
		if (ReadContextWord(&pAesPrivateCtx->engineCore) == SEP_AES_ENGINE2) {
			HW_DESC_SET_FLOW_MODE(&desc, S_AES2_to_DOUT);
		} else {
			HW_DESC_SET_FLOW_MODE(&desc, S_AES_to_DOUT);
		}
	} else {
		block_size = SEP_DES_IV_SIZE;
		HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pCtx->direction));
		HW_DESC_SET_FLOW_MODE(&desc, S_DES_to_DOUT);
	}
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->block_state, block_size);

	AddHWDescSequence(qid, &desc);
}

void LoadCipherKey(int qid, struct sep_ctx_cipher *pCtx)
{
	HwDesc_s desc;
	uint32_t keySize = ReadContextWord(&pCtx->key_size);
	XcbcMacRfcKeys_s *XcbcKeys = (XcbcMacRfcKeys_s*)pCtx->key;
	SepCipherPrivateContext_s *pAesPrivateCtx = (SepCipherPrivateContext_s *)pCtx->reserved;
	enum sep_crypto_direction encDecFlag = ReadContextWord(&pCtx->direction);

	HW_DESC_INIT(&desc);

	/* key size 24 bytes count as 32 bytes, make sure to zero wise upper 8 bytes */
	if (keySize == 24) {
		keySize	= SEP_AES_KEY_SIZE_MAX;
		ClearCtxField(&pCtx->key[24], SEP_AES_KEY_SIZE_MAX - 24);
	}

	HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pCtx->mode));
	if (ReadContextWord(&pCtx->alg) == SEP_CRYPTO_ALG_AES) {
		if ((ReadContextWord(&pCtx->crypto_key_type) == DX_XOR_HDCP_KEY) &&
		    (ReadContextWord(&pCtx->direction) == SEP_CRYPTO_DIRECTION_DECRYPT)) {
			/* if the crypto operation is DECRYPT we still order the HW for ENCRYPT operation 
			when using HDCP key. The next decriptor which loads the HDCP XOR key will direct DECRYPT
			operation. */
			encDecFlag = SEP_CRYPTO_DIRECTION_ENCRYPT;
		}
		HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pAesPrivateCtx->isTunnelOp) ? ReadContextWord(&pAesPrivateCtx->tunnetDir) : encDecFlag);
		HW_DESC_SET_CIPHER_CONFIG1(&desc, ReadContextWord(&pAesPrivateCtx->isTunnelOp));
		switch (ReadContextWord(&pCtx->mode)) {
		case SEP_CIPHER_XCBC_MAC:
			HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)XcbcKeys->K1, SEP_AES_128_BIT_KEY_SIZE);
			HW_DESC_SET_KEY_SIZE_AES(&desc, SEP_AES_128_BIT_KEY_SIZE);
			if (ReadContextWord(&pCtx->crypto_key_type) == DX_XOR_HDCP_KEY) {
				HW_DESC_SET_AES_XOR_CRYPTO_KEY(&desc);
			}
			break;
		default:
			switch (ReadContextWord(&pCtx->crypto_key_type)) {
			case DX_ROOT_KEY:
				HW_DESC_SET_CIPHER_DO(&desc, ReadContextWord(&pCtx->crypto_key_type));
				break;
			case DX_SESSION_KEY:
				HW_DESC_SET_CIPHER_DO(&desc, SESSION_KEY); //value to be written to DO when session key is used
				break;
			case DX_XOR_HDCP_KEY:
				HW_DESC_SET_AES_XOR_CRYPTO_KEY(&desc);
				/*FALLTHROUGH*/
			case DX_USER_KEY:
			case DX_APPLET_KEY:
			default:
				HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->key, keySize);
			}
			HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
		}

		if (ReadContextWord(&pAesPrivateCtx->engineCore) == SEP_AES_ENGINE2) {
			HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES2);
		} else {
			HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
		}
	} else {
		HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->key, ReadContextWord(&pCtx->key_size));
		HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_DES);
		HW_DESC_SET_KEY_SIZE_DES(&desc, ReadContextWord(&pCtx->key_size));
		HW_DESC_SET_CIPHER_CONFIG0(&desc, encDecFlag);
	}

	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	AddHWDescSequence(qid, &desc);

	if ((ReadContextWord(&pCtx->alg) == SEP_CRYPTO_ALG_AES) &&
	    (ReadContextWord(&pCtx->crypto_key_type) == DX_XOR_HDCP_KEY)) {
		/* in HDCP, the user key being XORed with SEP_HDCP_CONST registers on the fly.
		   We reusing the descriptor and overwrite the necessary bits so DO NOT
		   clear the descriptor before. */
		HW_DESC_SET_DIN_NO_DMA(&desc, NO_DMA, 0);
		HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pCtx->direction)); /* user direction */
		HW_DESC_SET_CIPHER_DO(&desc, ReadContextWord(&pCtx->crypto_key_type));
		AddHWDescSequence(qid, &desc);
	}

	if (ReadContextWord(&pCtx->mode) == SEP_CIPHER_XTS) {
		HW_DESC_INIT(&desc);

		/* load XEX key */
		HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pCtx->mode));
		HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pAesPrivateCtx->isTunnelOp) ? ReadContextWord(&pAesPrivateCtx->tunnetDir) : encDecFlag);
		HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->xex_key, keySize);
		HW_DESC_SET_XEX_DATA_UNIT_SIZE(&desc, ReadContextWord(&pCtx->data_unit_size));
		HW_DESC_SET_CIPHER_CONFIG1(&desc, ReadContextWord(&pAesPrivateCtx->isTunnelOp));
		if (ReadContextWord(&pAesPrivateCtx->engineCore) == SEP_AES_ENGINE2) {
			HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES2);
		} else {
			HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
		}
		HW_DESC_SET_KEY_SIZE_AES(&desc, keySize);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_XEX_KEY);
		AddHWDescSequence(qid, &desc);
	}

	if (ReadContextWord(&pCtx->mode) == SEP_CIPHER_XCBC_MAC) {

		/* load K2 key */
		/* NO init - reuse previous descriptor settings */
		HW_DESC_SET_STATE_DIN_PARAM(&desc,(uint32_t)XcbcKeys->K2, SEP_AES_128_BIT_KEY_SIZE);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE1);
		AddHWDescSequence(qid, &desc);

		/* load K3 key */
		/* NO init - reuse previous descriptor settings */
		HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)XcbcKeys->K3, SEP_AES_128_BIT_KEY_SIZE);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE2);
		AddHWDescSequence(qid, &desc);
	}
}

/*!
 * Revert operation of the last MAC block processing
 * This function is used for AES-XCBC-MAC and AES-CMAC when finalize
 * has not data. It reverts the last block operation in order to allow
 * redoing it as final.
 * 
 * \param qid 
 * \param pCtx 
 *
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
static int RevertLastMacBlock(int qid, struct sep_ctx_cipher *pCtx)
{
	HwDesc_s desc;
	XcbcMacRfcKeys_s *XcbcKeys = (XcbcMacRfcKeys_s*)pCtx->key;

	/* Relevant only for AES-CMAC and AES-XCBC-MAC */
	if ((ReadContextWord(&pCtx->mode) != SEP_CIPHER_XCBC_MAC) && (ReadContextWord(&pCtx->mode) != SEP_CIPHER_CMAC)) {
		DX_PAL_LOG_ERR("Wrong mode for this function (mode %d)\n", ReadContextWord(&pCtx->mode));
		return DX_RET_UNSUPP_ALG_MODE;
	}
	if (ReadContextWord(&pCtx->crypto_key_type) == DX_ROOT_KEY) {
		DX_PAL_LOG_ERR("RKEK not allowed for XCBC-MAC/CMAC\n");
		return DX_RET_UNSUPP_ALG_MODE;
	}
	/* CMAC and XCBC must use 128b keys */
	if ((ReadContextWord(&pCtx->mode) == SEP_CIPHER_XCBC_MAC) && (ReadContextWord(&pCtx->key_size) != SEP_AES_128_BIT_KEY_SIZE)) {
		DX_PAL_LOG_ERR("Bad key for XCBC-MAC %x\n", (unsigned int)ReadContextWord(&pCtx->key_size));
		return DX_RET_INVARG_KEY_SIZE;
	}

	/* Load key for ECB decryption */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_ECB);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, SEP_CRYPTO_DIRECTION_DECRYPT);
	if (ReadContextWord(&pCtx->mode) == SEP_CIPHER_XCBC_MAC) { /* XCBC K1 key is used (always 128b) */
		HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)XcbcKeys->K1, SEP_AES_128_BIT_KEY_SIZE);
		HW_DESC_SET_KEY_SIZE_AES(&desc, SEP_AES_128_BIT_KEY_SIZE);
	} else  {/* CMAC */
		HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->key,
			(ReadContextWord(&pCtx->key_size) == 24) ? SEP_AES_KEY_SIZE_MAX : ReadContextWord(&pCtx->key_size));
		HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
	}
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	AddHWDescSequence(qid, &desc);

	/* Initiate decryption of block state to previous block_state-XOR-M[n] */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->block_state, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->block_state, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
	AddHWDescSequence(qid, &desc);

	return DX_RET_OK;
}

static void CalcXcbcKeys(int qid, struct sep_ctx_cipher *pCtx)
{
	int i;
	HwDesc_s setup_desc;
	HwDesc_s data_desc;
	/* Overload key+xex_key fields with Xcbc keys */
	XcbcMacRfcKeys_s *XcbcKeys = (XcbcMacRfcKeys_s*)pCtx->key;
	//const uint8_t *keyConst = XcbcKeysConst.K1;
	uint8_t *derivedKey = XcbcKeys->K1;
	uint32_t constKey = 0x01010101;

	/* Prepare key setup descriptor (same for all XCBC-MAC keys) */
	HW_DESC_INIT(&setup_desc);
	HW_DESC_SET_CIPHER_MODE(&setup_desc, SEP_CIPHER_ECB);
	HW_DESC_SET_CIPHER_CONFIG0(&setup_desc, SEP_CRYPTO_DIRECTION_ENCRYPT);
	HW_DESC_SET_STATE_DIN_PARAM(&setup_desc, (uint32_t)XcbcKeys->K, SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_KEY_SIZE_AES(&setup_desc, SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&setup_desc, S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&setup_desc, SETUP_LOAD_KEY0);

	/* load user key */
	AddHWDescSequence(qid, &setup_desc);

	HW_DESC_INIT(&data_desc);
	HW_DESC_SET_FLOW_MODE(&data_desc, DIN_AES_DOUT);

	for (i = 0; i < AES_XCBC_MAC_NUM_KEYS ; i++) {

		/* encrypt each XCBC constant with the user given key to get K1, K2, K3 */
		HW_DESC_SET_DIN_CONST(&data_desc, (constKey * (i+1)), SEP_AES_128_BIT_KEY_SIZE);
		HW_DESC_SET_STATE_DOUT_PARAM(&data_desc, (uint32_t)derivedKey, SEP_AES_128_BIT_KEY_SIZE);
		HW_DESC_LOCK_QUEUE(&data_desc, 1); /* Lock until RESET_CONST */
		AddHWDescSequence(qid, &data_desc);
		/* Procede to next derived key calculation */
		derivedKey += SEP_AES_128_BIT_KEY_SIZE;
	}
#ifdef DX_CONFIG_HW_RESET_CONST_SUPPORT
	HW_DESC_INIT(&data_desc);
	HW_DESC_RESET_CONST_INPUT(&data_desc);
	AddHWDescSequence(qid, &data_desc);
#endif
}


/******************************************************************************
*				FUNCTIONS
******************************************************************************/

/*!
 * This function is used to initialize the AES machine to perform the AES
 * operations. This should be the first function called.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int InitCipher(struct sep_ctx_cipher *pCtx)
{
	SepCipherPrivateContext_s *pCipherPrivateCtx = (SepCipherPrivateContext_s *)pCtx->reserved;
	int qid = CURR_QUEUE_ID(); /* qid is stored in pxTaskTag field */

	if (ReadContextWord(&pCtx->alg) == SEP_CRYPTO_ALG_DES) {
	    /*in caes of double DES k1 = K3, copy k1-> K3*/
	    if (ReadContextWord(&pCtx->key_size) == SEP_DES_DOUBLE_KEY_SIZE){
#ifdef DX_CC_SRAM_INDIRECT_ACCESS
		    /*temporary buffer to allow key coping, must be aligned to words*/
		    uint32_t tKeybuff[SEP_DES_ONE_KEY_SIZE/sizeof(uint32_t)];
		    ReadContextField(pCtx->key, tKeybuff, SEP_DES_ONE_KEY_SIZE);
		    WriteContextField((pCtx->key + SEP_DES_DOUBLE_KEY_SIZE), tKeybuff, SEP_DES_ONE_KEY_SIZE);
		    WriteContextWord(&pCtx->key_size, SEP_DES_TRIPLE_KEY_SIZE);
#else
		    DX_PAL_MemCopy((pCtx->key + SEP_DES_DOUBLE_KEY_SIZE), pCtx->key, SEP_DES_ONE_KEY_SIZE);
		    pCtx->key_size = SEP_DES_TRIPLE_KEY_SIZE;
#endif
	    }
	    return DX_RET_OK;
	}

	switch (ReadContextWord(&pCtx->mode)) {
	case SEP_CIPHER_CMAC:
		ClearCtxField(pCtx->block_state, SEP_AES_BLOCK_SIZE);
		if(ReadContextWord(&pCtx->crypto_key_type) == DX_ROOT_KEY) {
			uint32_t keySize;
			GET_ROOT_KEY_SIZE(keySize);
			WriteContextWord(&pCtx->key_size,keySize);
		}
		break;
	case SEP_CIPHER_XCBC_MAC:
		if (ReadContextWord(&pCtx->key_size) != SEP_AES_128_BIT_KEY_SIZE) {
			DX_PAL_LOG_ERR("Invalid key size\n");
			return DX_RET_INVARG;
		}
		ClearCtxField(pCtx->block_state, SEP_AES_BLOCK_SIZE);
		CalcXcbcKeys(qid, pCtx);
		break;
	default:
		break;
	}
 	
	/* init private context */
	WriteContextWord(&pCipherPrivateCtx->engineCore,SEP_AES_ENGINE1);
	WriteContextWord(&pCipherPrivateCtx->isTunnelOp, TUNNEL_OFF);
	WriteContextWord(&pCipherPrivateCtx->isDataBlockProcessed,0);

	return DX_RET_OK;
}

/*!
 * This function is used to process block(s) of data using the AES machine.
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessCipher(struct sep_ctx_cipher *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint8_t *pInputData = NULL, *pOutputData = NULL;
	uint32_t isNotLastDescriptor = 0;
	uint32_t DataInSize = 0, DataOutSize = 0;
	uint32_t flowMode;
	HwDesc_s desc;
	DmaMode_t dmaMode = NO_DMA;
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;
	uint8_t outAxiNs = pDmaOutputBuffer->axiNs;
	SepCipherPrivateContext_s *pAesPrivateCtx = (SepCipherPrivateContext_s *)pCtx->reserved;
	int nMlliTables = pDmaInputBuffer->nTables;
	int qid = CURR_QUEUE_ID(); /* qid is stored in pxTaskTag field */
	int drvRc = DX_RET_OK;
	const int isInplaceOp = (pDmaInputBuffer->pData == pDmaOutputBuffer->pData ||
				ReadContextWord(&pCtx->mode) == SEP_CIPHER_CBC_MAC ||
				ReadContextWord(&pCtx->mode) == SEP_CIPHER_XCBC_MAC ||
				ReadContextWord(&pCtx->mode) == SEP_CIPHER_CMAC);



	if (ReadContextWord(&pCtx->mode) == SEP_CIPHER_XTS) {
		/* in XTS the key must be loaded first */
		LoadCipherKey(qid, pCtx);
	 	LoadCipherState(qid, pCtx,0);
 	} else if(ReadContextWord(&pCtx->mode) == SEP_CIPHER_CBC_CTS){
		WriteContextWord(&pCtx->mode,SEP_CIPHER_CBC);
	 	LoadCipherState(qid, pCtx,0);
		LoadCipherKey(qid, pCtx);
		WriteContextWord(&pCtx->mode,SEP_CIPHER_CBC_CTS);
	} else {
		LoadCipherState(qid, pCtx,0);
		LoadCipherKey(qid, pCtx);
	}

	/* set the input/output pointers according to the DMA mode */
	if ((!isInplaceOp) && pDmaInputBuffer->dmaBufType != pDmaOutputBuffer->dmaBufType) {
		DX_PAL_LOG_ERR("Inconsistent DMA mode for in/out buffers");
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

	if ((ReadContextWord(&pCtx->mode) == SEP_CIPHER_CMAC) || (ReadContextWord(&pCtx->mode) == SEP_CIPHER_XCBC_MAC)) {
		isNotLastDescriptor = 1; 
	}

	/* process the AES flow */
	HW_DESC_INIT(&desc);
	
	HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, DataInSize, QID_TO_AXI_ID(qid), inAxiNs);
	if (isNotLastDescriptor) {
		HW_DESC_SET_DIN_NOT_LAST_INDICATION(&desc);
	}

	switch (ReadContextWord(&pCtx->mode)) {
	case SEP_CIPHER_CBC_MAC:
	case SEP_CIPHER_CMAC:
	case SEP_CIPHER_XCBC_MAC:
		break;
	default:
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode, (uint32_t)pOutputData, DataOutSize, QID_TO_AXI_ID(qid), outAxiNs);
	}

	flowMode = (ReadContextWord(&pCtx->alg) == SEP_CRYPTO_ALG_AES) ? DIN_AES_DOUT : DIN_DES_DOUT;

	HW_DESC_SET_FLOW_MODE(&desc, flowMode);

#ifdef SEP_PERFORMANCE_TEST
	/* For testing exact HW time */
	HW_QUEUE_WAIT_UNTIL_EMPTY(qid);
	TIMING_MARK(1);
	TIMING_MARK(2);
#endif
	AddHWDescSequence(qid, &desc);
	
#ifdef SEP_PERFORMANCE_TEST
	TIMING_MARK(2);
	HW_QUEUE_WAIT_UNTIL_EMPTY(qid);
	TIMING_MARK(1);
#endif

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
		if (isNotLastDescriptor) {
			HW_DESC_SET_DIN_NOT_LAST_INDICATION(&desc);
		}

		switch (ReadContextWord(&pCtx->mode)) {
		case SEP_CIPHER_CMAC:
		case SEP_CIPHER_CBC_MAC:
		case SEP_CIPHER_XCBC_MAC:
			break;
		default:
			HW_DESC_SET_DOUT_TYPE(&desc, DMA_MLLI, (uint32_t)pOutputData, 0, QID_TO_AXI_ID(qid), outAxiNs);
		}
		HW_DESC_SET_FLOW_MODE(&desc, flowMode);

		AddHWDescSequence(qid, &desc);
	}

	/* at least one block of data processed */
	WriteContextWord(&pAesPrivateCtx->isDataBlockProcessed,1);

	/* get machine state */
	StoreCipherState(qid, pCtx);

EndWithErr:
	return drvRc;
}

/*!
 * This function is used as finish operation of AES on XCBC, CMAC, CBC
 * and other modes besides XTS mode.
 * The function may either be called after "InitCipher" or "ProcessCipher".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeCipher(struct sep_ctx_cipher *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint32_t isRemainingData = 0;
	uint32_t DataInSize = 0;
	uint8_t *pInputData = NULL;
	HwDesc_s desc;
	DmaMode_t dmaMode = NO_DMA;
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;
	int qid = CURR_QUEUE_ID(); /* qid is stored in pxTaskTag field */
	int drvRc = DX_RET_OK;
	SepCipherPrivateContext_s *pAesPrivateCtx = (SepCipherPrivateContext_s *)pCtx->reserved;

	HW_DESC_INIT(&desc);

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
	
	switch(ReadContextWord(&pCtx->mode)) {
	case SEP_CIPHER_CMAC:
	case SEP_CIPHER_XCBC_MAC:
	{
		if (isRemainingData > 1) {
			/* this case only apply to DMA_MLLI mode! */
			pDmaInputBuffer->nTables--;
			ProcessCipher(pCtx, pDmaInputBuffer, pDmaOutputBuffer);
			PrepareNextMLLITable(qid, inAxiNs, MLLI_INPUT_TABLE);
			pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);
			DataInSize = 0;
		} else if (isRemainingData == 1) {
			if (dmaMode == DMA_MLLI) {
				PrepareFirstMLLITable(qid, pDmaInputBuffer, MLLI_INPUT_TABLE);
				pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);
				DataInSize = 0;
			} else {
				pInputData = (uint8_t *)pDmaInputBuffer->pData;
				DataInSize = pDmaInputBuffer->size;
			}
		}


		/* Prepare processing descriptor to be pushed after loading state+key */
		HW_DESC_INIT(&desc);
		if (isRemainingData == 0) {
			if (ReadContextWord(&pAesPrivateCtx->isDataBlockProcessed) == 0) {
				/* MAC for 0 bytes */
				HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pCtx->mode));
				HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
				HW_DESC_SET_CMAC_SIZE0_MODE(&desc);
				HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
			} else {
				/* final with 0 data but MAC total data size > 0 */
				drvRc = RevertLastMacBlock(qid, pCtx); /* Get C[n-1]-xor-M[n] */
				if (drvRc != DX_RET_OK) {
					goto EndWithErr;
				}
				/* Finish with data==0 is identical to "final"
				   op. on the last (prev.) block (XOR with 0) */
				HW_DESC_SET_DIN_CONST(&desc, 0x00, SEP_AES_BLOCK_SIZE);
				HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
			}
		} else {
			HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, DataInSize, QID_TO_AXI_ID(qid), inAxiNs);
			HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		}

		/* load AES key and iv length and digest */
		LoadCipherState(qid, pCtx,0);
		LoadCipherKey(qid, pCtx);

		/* Process last block */
		AddHWDescSequence(qid, &desc);
#ifdef DX_CONFIG_HW_RESET_CONST_SUPPORT
		HW_DESC_INIT(&desc);
		HW_DESC_RESET_CONST_INPUT(&desc);
		AddHWDescSequence(qid, &desc);
#endif
		/* get machine state */
		StoreCipherState(qid, pCtx);
		break;
	}
	case SEP_CIPHER_CBC_CTS:
	{ 
		/*In case of data size = SEP_AES_BLOCK_SIZE check that no blocks were processed before*/
		if ((pDmaInputBuffer->size == SEP_AES_BLOCK_SIZE)&&
		    (ReadContextWord(&pAesPrivateCtx->isDataBlockProcessed) == 1)){  
			DX_PAL_LOG_ERR("Invalid dataIn size\n");
			drvRc = DX_RET_INVARG;
			goto EndWithErr;
		}
		/*Call ProcessCTSFinalizeCipher to process AES CTS finalize operation */
		drvRc = ProcessCTSFinalizeCipher(pCtx, pDmaInputBuffer, pDmaOutputBuffer);
		if (drvRc != DX_RET_OK) {
                goto EndWithErr;
		}
		break;
	}
	default:
		if (isRemainingData) {
			/* process all tables and get state from the AES machine */
			drvRc = ProcessCipher(pCtx, pDmaInputBuffer, pDmaOutputBuffer);
			if (drvRc != DX_RET_OK) {
				goto EndWithErr;
			}
		} else if (ReadContextWord(&pCtx->mode) == SEP_CIPHER_CBC_MAC) {
			/* in-case ZERO data has processed the output would be the encrypted IV */
			if (ReadContextWord(&pAesPrivateCtx->isDataBlockProcessed) == 0) {
				/* load AES key and iv length and digest */
				LoadCipherState(qid, pCtx,0);
				LoadCipherKey(qid, pCtx);

				HW_DESC_INIT(&desc);
				HW_DESC_SET_DIN_CONST(&desc, 0x00, SEP_AES_BLOCK_SIZE);
				HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
				AddHWDescSequence(qid, &desc);
#ifdef DX_CONFIG_HW_RESET_CONST_SUPPORT
				HW_DESC_INIT(&desc);
				HW_DESC_RESET_CONST_INPUT(&desc);
				AddHWDescSequence(qid, &desc);
#endif
				/* get mac result */
				StoreCipherState(qid, pCtx);
			}
		}
	}

EndWithErr:
	return drvRc;
}

/*!
 * This function is used as finish operation of AES CTS mode.
 * The function may either be called after "InitCipher" or "ProcessCipher".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessCTSFinalizeCipher(struct sep_ctx_cipher *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint32_t numOfBlocks = 0;
	uint32_t DataInSize = 0;
	uint32_t lastBlockSize = 0;
	uint32_t dataSizeForDLLI = 0;
	DmaMode_t dmaMode = NO_DMA;
	uint8_t *pInputData = NULL, *pOutputData = NULL;
	uint32_t lastBlockOffset = 0,nextToLastBlkOffset = 0;
	HwDesc_s desc;
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;
	uint8_t outAxiNs = pDmaOutputBuffer->axiNs;
	int qid = CURR_QUEUE_ID();
	int drvRc = DX_RET_OK;
	uint8_t *tempBuff= NULL;
    dmaMode = DMA_BUF_TYPE_TO_MODE(pDmaInputBuffer->dmaBufType);


	/*Use context buffer aw temp buffer for internal operations */
	tempBuff = pCtx->xex_key;
 
 	/* check if we have remaining data to process */
	switch (dmaMode) {
	case DMA_MLLI:
	case DMA_SRAM:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		return drvRc;
	case DMA_DLLI:
		DataInSize = pDmaInputBuffer->size;		
		pInputData = (uint8_t*)pDmaInputBuffer->pData;
		pOutputData =(uint8_t*)pDmaOutputBuffer->pData;
		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		return drvRc;
	}

	/*Calculate last block size*/
	lastBlockSize = pDmaInputBuffer->size& AES_BLOCK_MASK;

	if (lastBlockSize == 0) {
		lastBlockSize = SEP_AES_BLOCK_SIZE; 
	}
	/*Calculte dataSizeForDLLI for ProcessCipher operation*/
	if (pDmaInputBuffer->size > SEP_AES_BLOCK_SIZE){
		dataSizeForDLLI = pDmaInputBuffer->size -(lastBlockSize + SEP_AES_BLOCK_SIZE);
	}
	else if(pDmaInputBuffer->size ==SEP_AES_BLOCK_SIZE){
          	dataSizeForDLLI = SEP_AES_BLOCK_SIZE;
    	}
	/*Process data with ProcessCipher */
	if (dataSizeForDLLI>0){
		/*Update data size for ProcessCipher operation*/
		pDmaInputBuffer->size = dataSizeForDLLI;
		pDmaOutputBuffer->size = dataSizeForDLLI;
           
		/*Call ProcessCipher*/
		drvRc = ProcessCipher(pCtx, pDmaInputBuffer, pDmaOutputBuffer);
		if (drvRc != DX_RET_OK) {
			goto EndWithErr;
		}
		/*Revert original value of data szie */
		pDmaInputBuffer->size = DataInSize;
		pDmaOutputBuffer->size =DataInSize;
        
		if (DataInSize ==SEP_AES_BLOCK_SIZE) {
			return drvRc;
		}
	}
 
	/*Calculate offsets of two last blocks */
	numOfBlocks = (DataInSize + SEP_AES_BLOCK_SIZE -1)/SEP_AES_BLOCK_SIZE;
	lastBlockOffset = (numOfBlocks - 1)*SEP_AES_BLOCK_SIZE;
	nextToLastBlkOffset = lastBlockOffset - SEP_AES_BLOCK_SIZE;
	lastBlockSize = DataInSize - lastBlockOffset;
    
	/*Change mode to SEP_CIPHER_CBC*/
	WriteContextWord(&pCtx->mode ,SEP_CIPHER_CBC);
	LoadCipherState(qid, pCtx,0);
	LoadCipherKey(qid, pCtx);

	/*Initialize  context's buffer for internal use*/
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_CONST(&desc, 0, SEP_AES_BLOCK_SIZE*2);
	HW_DESC_SET_DOUT_SRAM(&desc, (uint32_t)pCtx->xex_key, SEP_AES_BLOCK_SIZE*2);
	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
	AddHWDescSequence(qid, &desc);

	#ifdef DX_CONFIG_HW_RESET_CONST_SUPPORT
	    HW_DESC_INIT(&desc);
	    HW_DESC_RESET_CONST_INPUT(&desc);
	    AddHWDescSequence(qid, &desc);
	#endif

	/*Encrypt mode */
	if(ReadContextWord(&pCtx->direction) == SEP_CRYPTO_DIRECTION_ENCRYPT){
    
		/* process regular AES CBC flow on next to last block */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData+nextToLastBlkOffset,SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode, (uint32_t)pOutputData+nextToLastBlkOffset,SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		AddHWDescSequence(qid, &desc);

		/* 1. Copy next to last block to temp SRAM buff to save it*/
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc,dmaMode, (uint32_t)pOutputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_SRAM(&desc,(uint32_t)tempBuff, SEP_AES_BLOCK_SIZE); 
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);

 		/*2. Copy zero buff to place of second to last block to create zero padding */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_SRAM(&desc,(uint32_t)tempBuff + SEP_AES_BLOCK_SIZE, SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_DOUT_TYPE(&desc,dmaMode, (uint32_t)pOutputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);	
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);
        
		/*3. Copy last block to place of second to last */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc,dmaMode, (uint32_t)pInputData + lastBlockOffset, lastBlockSize, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_TYPE(&desc,dmaMode,(uint32_t)pOutputData + nextToLastBlkOffset,lastBlockSize, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);


  		/*4.Encrypt padded last block to temp2 buff in sram*/
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, dmaMode,(uint32_t)pOutputData + nextToLastBlkOffset,SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_SRAM(&desc,(uint32_t)tempBuff + SEP_AES_BLOCK_SIZE, SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		AddHWDescSequence(qid, &desc);
		/* Perform CTS swaping operation */
  

		/*5. Copy saved in temp1 block to place of second to last output block */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_SRAM(&desc,(uint32_t)tempBuff , SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_DOUT_TYPE(&desc,dmaMode, (uint32_t)pOutputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);	
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);

 		/*6.Copy  using BYPASS, "next to last block" to "last" block and truncate it to last block size*/
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc,dmaMode, (uint32_t)pOutputData + nextToLastBlkOffset, lastBlockSize, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode,(uint32_t)pOutputData + lastBlockOffset, lastBlockSize, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);

		/*7. Copy saved in temp2 block to place of second to last block */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_SRAM(&desc,(uint32_t)tempBuff + SEP_AES_BLOCK_SIZE , SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_DOUT_TYPE(&desc,dmaMode, (uint32_t)pOutputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);	
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);
	
	} else { /*decrypt operation*/
		
		/* Descriptor flow for Decrypt CBC CTS operation (by AlonZ):
		Decrypt the data up to block Cn-2 using regular AES-CBC; save the chaining value (output IV = Cn-2)
		Process the last one-plus-reminder blocks:
		1. BYPASS DMA blocks (Cn-1, Cn) into SRAM (Cn is a partial block of length k)
		2. Decrypt Cn-1 using AES-ECB into Dn-1
		3. BYPASS DMA 16-k last bytes of Dn-1 to the end of Cn producing Cn'
		4. Decrypt Cn' using AES-CBC (IV = Cn-2) to produce Pn-1 (in SRAM)
		5. Decrypt Cn-1 (again) using AES-CBC (IV = Cn') to produce padded Pn (in SRAM)
		6. BYPASS DMA Pn-1, Pn to output */
		
		/* 1 Save Cn-1 in temp buff (in SRAM) */		
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_SRAM(&desc,(uint32_t)tempBuff,SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);	
		 
		/*decrypt the next to last block with ECB operation*/
		/*change context aes mode to ecb*/
 		/*load key for ecb operation*/
		LoadCipherState(qid, pCtx,1);
		LoadCipherKey(qid, pCtx);
		
		/*2 perform ECB decrypt of next to last block*/
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode, (uint32_t)pOutputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		AddHWDescSequence(qid, &desc);


		/*3. Copy Cn, using BYPASS, to 16-k last bytes of decrypted with ECB next to last block to produce Cn'*/
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc,dmaMode, pInputData + lastBlockOffset,lastBlockSize, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode,pOutputData + nextToLastBlkOffset, lastBlockSize, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);
		
		
		/*4.Decrypt Cn' using AES-CBC and save it temp buff(IV = Cn-2, that was save in ctx by TST_StoreCipherState operation) 
		  to produce Dn-1 */
		/*restore aes mode to cbc for next operations*/
		LoadCipherState(qid, pCtx,0);
		LoadCipherKey(qid, pCtx);

		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pOutputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_SRAM(&desc,(uint32_t)tempBuff + SEP_AES_BLOCK_SIZE,SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		AddHWDescSequence(qid, &desc);


		/*5. Decrypt Cn-1(was saved in temp buff) again using AES-CBC (IV = Cn') to produce padded Dn (in SRAM)*/		
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_SRAM(&desc,(uint32_t)tempBuff, SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_DOUT_SRAM(&desc,(uint32_t)tempBuff,SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		AddHWDescSequence(qid, &desc);

		/*6. Copy the decrypted data to output*/
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_SRAM(&desc,(uint32_t)tempBuff, SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode,(uint32_t)pOutputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);

		/*copy partitial block to last block */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc,dmaMode,(uint32_t)pOutputData + nextToLastBlkOffset, lastBlockSize, QID_TO_AXI_ID(qid), inAxiNs);
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode,(uint32_t)pOutputData + lastBlockOffset, lastBlockSize, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);

		/*Copy the last blocks Dn-1 from temp SRAM buffer to output data*/
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_SRAM(&desc,(uint32_t)tempBuff +SEP_AES_BLOCK_SIZE, SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode,(uint32_t)pOutputData + nextToLastBlkOffset, SEP_AES_BLOCK_SIZE, QID_TO_AXI_ID(qid), outAxiNs);
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		AddHWDescSequence(qid, &desc);
	} 
EndWithErr:
	return drvRc;
}

