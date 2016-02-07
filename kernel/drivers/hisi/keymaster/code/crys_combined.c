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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "dx_pal_compiler.h"
#include "crys_combined.h"
#include "crys_combined_error.h"
#include "combined.h"
#include "sym_adaptor_driver.h"
#include "hw_queue_defs.h"
#include "completion.h"
#include "dma_buffer.h"
#include "cc_acl.h"
#include "dx_error.h"
#include "crys_context_relocation.h"

int GetHashDigestSize(enum sep_hash_mode mode, uint32_t *digestSize);

#ifdef DX_CC_TEE
/* In order to allow contiguous context we double the user context (+ 3 words */
/* for managment) in the tee environment. */
#define CRYS_COMBINED_USER_CTX_SIZE_IN_WORDS (sizeof(struct sep_ctx_combined)/2+ 3)
#else
#define	CRYS_COMBINED_USER_CTX_SIZE_IN_WORDS (sizeof(struct sep_ctx_combined)/4)
#endif



typedef struct CRYS_COMBINED_UserContext_t 
{
   DxUint32_t  buff[CRYS_COMBINED_USER_CTX_SIZE_IN_WORDS]; 
}CRYS_COMBINED_UserContext_t;




/************************* PRIVATE FUNCTIONS ******************************/

/*!
 * Converts Symmetric Adaptor return code to CRYS error code.
 * 
 * \param symRetCode Symmetric Adaptor return error.
 * \param errorInfo Ignored.
 *  
 * \return CRYSError_t one of CRYS_* error codes defined in crys_error.h
 */
static CRYSError_t SymAdaptor2CrysCombinedErr(int symRetCode, DxUint32_t errorInfo)
{
	switch (symRetCode) {
	case DX_RET_UNSUPP_ALG:
	case DX_RET_UNSUPP_ALG_MODE:
		return CRYS_COMBINED_IS_NOT_SUPPORTED;
	case DX_RET_INVARG:
	case DX_RET_INVARG_QID:
	case DX_RET_INVARG_KEY_SIZE:
		return CRYS_COMBINED_ILLEGAL_PARAMS_ERROR;
	case DX_RET_INVARG_CTX_IDX:
	case DX_RET_INVARG_CTX:
		return CRYS_COMBINED_INVALID_NODES_CONFIG_POINTER_ERROR;
	case DX_RET_INVARG_BAD_ADDR:
		return CRYS_COMBINED_DATA_OUT_POINTER_INVALID_ERROR;
	case DX_RET_NOMEM:
		return CRYS_OUT_OF_RESOURCE_ERROR;
	case DX_RET_INVARG_INCONSIST_DMA_TYPE:
	case DX_RET_UNSUPP_OPERATION:
	case DX_RET_PERM:
	case DX_RET_NOEXEC:
	case DX_RET_BUSY:
	case DX_RET_OSFAULT:
	default:
		return CRYS_FATAL_ERROR;
	}
}

static enum sep_engine_type GetCryptoEngineType(enum sep_crypto_alg alg)
{
	switch (alg) {
	case SEP_CRYPTO_ALG_AES:
		return SEP_ENGINE_AES;
	case SEP_CRYPTO_ALG_HASH:
	case SEP_CRYPTO_ALG_HMAC:
		return SEP_ENGINE_HASH;
	default:
		return SEP_ENGINE_NULL;
	}
}

static CRYSError_t ValidateSupportedModes(struct sep_ctx_combined *pCombinedCtx)
{
	int engineIdx;
	enum sep_crypto_alg alg;
	enum sep_crypto_direction prevDirection = SEP_CRYPTO_DIRECTION_NULL;

	for ( engineIdx = 0;
	      (engineIdx < CRYS_COMBINED_MAX_NODES) && (pCombinedCtx->sub_ctx[engineIdx] != NULL);
	      engineIdx++) {
		
		alg = (pCombinedCtx->sub_ctx[engineIdx])->alg;

		switch (alg) {
		case SEP_CRYPTO_ALG_AES:
		{
			struct sep_ctx_cipher *pAesCtx = (struct sep_ctx_cipher *)pCombinedCtx->sub_ctx[engineIdx];
	
			if ((pAesCtx->mode != SEP_CIPHER_ECB) &&
			    (pAesCtx->mode != SEP_CIPHER_CBC) &&
			    (pAesCtx->mode != SEP_CIPHER_CTR)) {
				return CRYS_COMBINED_UNSUPPORTED_SUB_OPERATION_MODE_ERROR;
			}
			if (prevDirection == SEP_CRYPTO_DIRECTION_NULL) {
				prevDirection = pAesCtx->direction;
			} else {
				/* Allow only decrypt->encrypt operation */
				if (!(prevDirection == SEP_CRYPTO_DIRECTION_DECRYPT &&
				    pAesCtx->direction == SEP_CRYPTO_DIRECTION_ENCRYPT))
				{
					return CRYS_COMBINED_ILLEGAL_PARAMS_ERROR;
				}
			}
			break;
		}
		case SEP_CRYPTO_ALG_HASH:
		{
			struct sep_ctx_hash *pHashCtx = (struct sep_ctx_hash *)pCombinedCtx->sub_ctx[engineIdx];
	
			if (  (pHashCtx->mode != SEP_HASH_SHA1) 
			    &&(pHashCtx->mode != SEP_HASH_SHA224)
			    &&(pHashCtx->mode != SEP_HASH_SHA256)
#ifdef DX_CONFIG_HASH_SHA_512_SUPPORTED
			    &&(pHashCtx->mode != SEP_HASH_SHA384)
			    &&(pHashCtx->mode != SEP_HASH_SHA512)
#endif
			    ) {
				return CRYS_COMBINED_UNSUPPORTED_SUB_OPERATION_MODE_ERROR;
			}
			break;
		}
		default:
			return CRYS_COMBINED_UNSUPPORTED_SUB_OPERATION_ALG_ERROR;
		}
	}
	return CRYS_OK;
}

static void InitCombinedContext(
	struct sep_ctx_combined *pCtx,
	CrysCombinedConfig_t *pConfig)
{
	CrysCombinedEngineSource_e engineSrc = INPUT_NULL;
	enum sep_engine_type engineType = SEP_ENGINE_NULL;
	int engineIdx;
	int done = 0;

	/* set alg */
	pCtx->alg = SEP_CRYPTO_ALG_COMBINED;
	pCtx->mode = 0;

	/* encode engines connections into SEP format */
	for (engineIdx = 0; (engineIdx < CRYS_COMBINED_MAX_NODES) && (!done); engineIdx++) {

		if(pConfig->node[engineIdx].pContext) {
			/* set context's pointers */
			pCtx->sub_ctx[engineIdx] = (struct sep_ctx_cache_entry *)DX_GetUserCtxLocation(pConfig->node[engineIdx].pContext);
		}
		else {
			pCtx->sub_ctx[engineIdx] = NULL;
		}
		/* set engine source */
		engineSrc = CrysCombinedEngineSrcGet(pConfig, engineIdx);

		/* set engine type */
		if (pCtx->sub_ctx[engineIdx] != NULL) {
			engineType = GetCryptoEngineType(pCtx->sub_ctx[engineIdx]->alg);
		} else if (engineSrc != INPUT_NULL) {
			/* incase engine source is not NULL and NULL sub-context
			is passed then DOUT is -DOUT */
			engineType = SEP_ENGINE_DOUT;
			done = 1; /* do not allow to set DOUT twice! */
		} else {
			/* both context pointer & input type are NULL -we're done */
			engineType = SEP_ENGINE_NULL;
			done = 1;
		}

		SepCombinedEnginePropsSet(&pCtx->mode, engineIdx, engineSrc, engineType);
		DX_PAL_LOG_DEBUG("engineSrc=%d  engineType=%d\n", engineSrc, engineType);
	}
}


/*!
 * Clears the configuration nodes.
 * 
 * \param pConfig A pointer to the configuration scheme array
 */
CIMPORT_C void CrysCombinedConfigInit(CrysCombinedConfig_t *pConfig)
{
	DX_PAL_MemSetZero(pConfig, sizeof(CrysCombinedConfig_t));
}

/*!
 * This function is used to initilize the combined or tunneling operations.
 * It should be called first in process sequence. 
 *  
 * \param pConfig A pointer to the Configuration Nodes array (NodesConfig). 
 * 			This array represents the user combined scheme. 
 * 
 * \return CIMPORT_C CRYSError_t On success the value CRYS_OK is returned, 
 * 			and on failure - a value from crys_combined_error.h
 */
CIMPORT_C CRYSError_t CRYS_Combined_Init(CrysCombinedConfig_t *pConfig)
{
	/* parameters check */
	if (pConfig == DX_NULL) {
		return CRYS_COMBINED_INVALID_NODES_CONFIG_POINTER_ERROR;
	}

	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, pConfig, sizeof(CrysCombinedConfig_t))) {
		return CRYS_COMBINED_ILLEGAL_PARAMS_ERROR;
	}

	/* we do not call adaptor init since the symmetric crypto driver
	does nothing for this API */

	return CRYS_OK;
}

/*!
 * This function is used to process block of data in the combined or tunneling mode.
 * 
 * \param pConfig A pointer to the Configuration Nodes array (NodesConfig). 
 * 			This array represents the user combined scheme. 
 * \param cipherOffset Relevant in cases where the authenticated  data resides in 
 *      		a different offset from the cipher data.
 *      		Note: currently an error returned for any value other than zero.
 * \param pDataIn A pointer on a block of input data ready for processing.
 * \param dataInSize The size of the input data.
 * \param pDataOut A pointer on output data. Could be the same as input data pointer 
 *      		(for inplace operations) or NULL if there is only
 *      		authentication for output.
 * 
 * \return CIMPORT_C CRYSError_t On success the value CRYS_OK is returned, 
 * 			and on failure - a value from crys_combined_error.h
 */
CIMPORT_C CRYSError_t CRYS_Combined_Process(
	CrysCombinedConfig_t *pConfig,
	uint32_t cipherOffset,
	uint8_t *pDataIn,
	uint32_t dataInSize,
	uint8_t *pDataOut)
{
	CRYS_COMBINED_UserContext_t	combinedUsrCtx;
	struct sep_ctx_combined *pcombinedCtx;
	CRYSError_t crysErr = CRYS_OK;
	int symRc = DX_RET_OK;

	/* parameters check */
	if (pConfig == DX_NULL) {
		return CRYS_COMBINED_INVALID_NODES_CONFIG_POINTER_ERROR;
	}
	if (cipherOffset != 0) {
		/*currently cipher address must be equal to the auth address*/
		return CRYS_COMBINED_ILLEGAL_OPERATION_MODE_ERROR;
	}
	if (pDataIn == NULL) {
		return CRYS_COMBINED_DATA_IN_POINTER_INVALID_ERROR;
	}
	/* data size must be a positive number and a block size mult */
	if (dataInSize == 0) {
		return CRYS_COMBINED_DATA_IN_SIZE_ILLEGAL;
	}

	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, pConfig, sizeof(CrysCombinedConfig_t))) {
		return CRYS_COMBINED_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pcombinedCtx = (struct sep_ctx_combined *)DX_InitUserCtxLocation(combinedUsrCtx.buff,
								      sizeof(CRYS_COMBINED_UserContext_t),
								      sizeof(struct sep_ctx_combined));
	InitCombinedContext(pcombinedCtx, pConfig);

	switch (pcombinedCtx->mode) {
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_MODE:
		break;
	case SEP_COMBINED_DIN_TO_AES_AND_HASH_MODE:
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_AND_DOUT_MODE:
	case SEP_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE:
		if (pDataOut == NULL) {
			return CRYS_COMBINED_DATA_OUT_POINTER_INVALID_ERROR;
		}
		break;
	default:
		return CRYS_COMBINED_ILLEGAL_OPERATION_MODE_ERROR;
	}

	crysErr = ValidateSupportedModes(pcombinedCtx);
	if (crysErr != CRYS_OK) {
		return crysErr;
	}
		
	symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pcombinedCtx,
				pDataIn, pDataOut, dataInSize);

	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysCombinedErr);
}

/*!
 * This function is used to finish the combined or tunneling operations
 * It releases all used contexts (including suboperation ones).
 * 
 * \param pConfig A pointer to the Configuration Nodes array (NodesConfig). 
 * 			This array represents the user combined scheme. 
 * \param cipherOffset Relevant in cases where the authenticated  data resides in 
 *      		a different offset from the cipher data.
 *      		Note: currently an error returned for any value other than zero.
 * \param pDataIn A pointer on a block of input data ready for processing.
 * \param dataInSize The size of the input data.
 * \param pDataOut A pointer to output data. Could be the same as input data pointer 
 *      		(for inplace operations) or NULL if there is only
 *      		authentication for output.
 * \param pAuthDataOut A pointer to authenticated or digested output result.
 * 
 * \return CIMPORT_C CRYSError_t On success the value CRYS_OK is returned, 
 * 			and on failure - a value from crys_combined_error.h
 */
CIMPORT_C CRYSError_t CRYS_Combined_Finish(
		CrysCombinedConfig_t *pConfig,
		uint32_t cipherOffset,
		uint8_t *pDataIn,
		uint32_t dataInSize,
		uint8_t *pDataOut,
		uint8_t *pAuthDataOut,
		uint32_t *pAuthDataOutSize)
{
	CRYS_COMBINED_UserContext_t	combinedUsrCtx;
	struct sep_ctx_combined *pcombinedCtx;
	CRYSError_t crysErr = CRYS_OK;
	int symRc = DX_RET_OK;

	/* parameters check */
	if (pConfig == DX_NULL) {
		return CRYS_COMBINED_INVALID_NODES_CONFIG_POINTER_ERROR;
	}

	if (cipherOffset != 0) {
		/*currently cipher address must be equal to the auth address*/
		return CRYS_COMBINED_ILLEGAL_OPERATION_MODE_ERROR;
	}

	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ, pConfig, sizeof(CrysCombinedConfig_t)) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, pAuthDataOutSize, sizeof(uint32_t)) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, pAuthDataOut, *pAuthDataOutSize)) {
		return CRYS_COMBINED_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pcombinedCtx = (struct sep_ctx_combined *)DX_InitUserCtxLocation(combinedUsrCtx.buff,
								      sizeof(CRYS_COMBINED_UserContext_t),
								      sizeof(struct sep_ctx_combined));
	InitCombinedContext(pcombinedCtx, pConfig);

	if ((pcombinedCtx->mode != SEP_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE) &&
	    (pAuthDataOutSize == DX_NULL)) {
		return CRYS_COMBINED_DATA_AUTH_BUFFER_SIZE_INVALID_ERROR;
	}
	switch (pcombinedCtx->mode) {
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_MODE:
	case SEP_COMBINED_DIN_TO_AES_AND_HASH_MODE:
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_AND_DOUT_MODE:
		if (pAuthDataOut == NULL) {
			return CRYS_COMBINED_DATA_AUTH_POINTER_INVALID_ERROR;
		}
		break;
	case SEP_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE:
		break;
	default:
		return CRYS_COMBINED_ILLEGAL_OPERATION_MODE_ERROR;
	}

	crysErr = ValidateSupportedModes(pcombinedCtx);
	if (crysErr != CRYS_OK) {
		return crysErr;
	}

	symRc = SymDriverAdaptorFinalize((struct sep_ctx_generic *)pcombinedCtx,
				pDataIn, pDataOut, dataInSize);
	if (symRc != DX_RET_OK) {
		return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysCombinedErr);
	}

	switch (pcombinedCtx->mode) {
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_MODE:
	case SEP_COMBINED_DIN_TO_AES_AND_HASH_MODE:
	case SEP_COMBINED_DIN_TO_AES_TO_HASH_AND_DOUT_MODE:
	{
		struct sep_ctx_hash *pHashCtx = (struct sep_ctx_hash *)pcombinedCtx->sub_ctx[1];
		uint32_t digestSize;

		crysErr = GetHashDigestSize(pHashCtx->mode, &digestSize);
		if (crysErr != CRYS_OK) {
			return CRYS_COMBINED_HASH_DIGEST_SIZE_ERROR;
		}

		if ((digestSize == 0) || (*pAuthDataOutSize < digestSize)) {
			return CRYS_COMBINED_DATA_AUTH_BUFFER_SIZE_INVALID_ERROR;
		}

		/* set the digest length out and copy digest result */
		*pAuthDataOutSize = digestSize;
		DX_PAL_MemCopy( pAuthDataOut, pHashCtx->digest, *pAuthDataOutSize );
		break;
	}
	case SEP_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE:
		break;
	default:
		return CRYS_COMBINED_ILLEGAL_OPERATION_MODE_ERROR;
	}

	return CRYS_OK;
}

/*!
 * This function is used to perform the combined or tunneling operations with 
 * one function call.
 * 
 * \param pConfig A pointer to the Configuration Nodes array. 
 * 		This array represents the user combined scheme. 
 * \param cipherOffset Relevant in cases where the authenticated  data resides in 
 *      		a different offset from the cipher data.
 *      		Note: currently an error returned for any value other than zero.
 * \param pDataIn A pointer on a block of input data ready for processing.
 * \param dataInSize The size of the input data.
 * \param pDataOut A pointer to output data. Could be the same as input data pointer 
 *      		(for inplace operations) or NULL if there is only
 *      		authentication result for output.
 * \param pAuthData A pointer to authentication or digest result output.
 * \param pAuthDataSize A pointer to size of user given output buffer [in] 
 * 			and actual size [out] of authentication or digest result . 
 * 
 * \return CIMPORT_C CRYSError_t On success the value CRYS_OK is returned, 
 * 			and on failure - a value from crys_combined_error.h
 */
CIMPORT_C CRYSError_t CRYS_Combined(
		CrysCombinedConfig_t *pConfig,
		uint32_t cipherOffset,
		uint8_t *pDataIn,
		uint32_t dataInSize,
		uint8_t *pDataOut,
		uint8_t *pAuthDataOut,
		uint32_t *pAuthDataOutSize)
{
	CRYSError_t crysErr = CRYS_OK;
	
	crysErr = CRYS_Combined_Init(pConfig);
	if (crysErr != CRYS_OK) {
		return crysErr;
	}

	return CRYS_Combined_Finish(pConfig, cipherOffset,
			pDataIn, dataInSize, pDataOut,
			pAuthDataOut, pAuthDataOutSize);
}
