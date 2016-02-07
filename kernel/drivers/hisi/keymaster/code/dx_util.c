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


/************* Include Files ****************/
#include "dx_pal_types.h"
#include "dx_util.h"
#include "sym_adaptor_driver.h"
#include "dx_error.h"
#include "crys_context_relocation.h"


#define DX_UTIL_BUFF_IN_WORDS	(sizeof(struct sep_ctx_cipher)/2 + 3)
#define DX_UTIL_BUFF_IN_BYTES	(DX_UTIL_BUFF_IN_WORDS*sizeof(uint32_t))

/*!
 * Converts Symmetric Adaptor return code to CRYS error code.
 * 
 * \param symRetCode Symmetric Adaptor return error.
 * \param errorInfo Ignored.
 *  
 * \return CRYSError_t one of CRYS_* error codes defined in crys_error.h
 */
static DxUTILError_t SymAdaptor2CmacDeriveKeyErr(int symRetCode)
{
	switch (symRetCode) {
	case DX_RET_INVARG:
		return DX_UTIL_ILLEGAL_PARAMS_ERROR;
	case DX_RET_INVARG_BAD_ADDR:
		return DX_UTIL_BAD_ADDR_ERROR;
	case DX_RET_INVARG_CTX:
	case DX_RET_UNSUPP_ALG:
	default:
		return DX_UTIL_FATAL_ERROR;
	}
}

/*!  
 * This function is used to generate bytes stream for key derivation purposes. 
 * The function gets an input data and can use use one of the following keys: KDR, Session. 
 * 
 * @param[in] aesKeyType	- 1 (RKEK); 5 (Session)
 * @param[in] pDataIn 		- a pointer to input buffer 
 * @param[in] blockSize 	- size of data in bytes
 * @param[out] pCmacResult 	- a pointer to output buffer
 * @param[in] pCmacResultSize 	- output buffer size, must equal to DX_UTIL_CMAC_DERV_DATA_OUT_SIZE 
 *  
 */
DxUTILError_t DX_UTIL_CmacDeriveKey(DX_CRYPTO_KEY_TYPE_t	aesKeyType, 
			            DxUint8_t  			*pDataIn,
			            DxUint32_t  		DataInSize,
			            DxUint8_t   		*pCmacResult,
				    DxUint32_t			cmacResultSize)
{
	int symRc;
	DxUint32_t keySizeInBytes = 0; 
	DxUint32_t ctxBuff[DX_UTIL_BUFF_IN_WORDS];
	struct sep_ctx_cipher *pAesContext = (struct sep_ctx_cipher *)DX_InitUserCtxLocation(ctxBuff,
								      DX_UTIL_BUFF_IN_BYTES,
								      sizeof(struct sep_ctx_cipher));


	// check inputs
	if (DX_SESSION_KEY == aesKeyType) {
		keySizeInBytes = SEP_AES_128_BIT_KEY_SIZE;
	} else if (aesKeyType != DX_ROOT_KEY) {
		return DX_UTIL_INVALID_KEY_TYPE;
	}

	if (NULL == pDataIn) {
		return DX_UTIL_DATA_IN_POINTER_INVALID_ERROR;
	}
	if (NULL == pCmacResult) {
		return DX_UTIL_DATA_OUT_POINTER_INVALID_ERROR;
	}
	if ((DataInSize < DX_UTIL_CMAC_DERV_MIN_DATA_IN_SIZE) ||
	    (DataInSize > DX_UTIL_CMAC_DERV_MAX_DATA_IN_SIZE)) {
		return DX_UTIL_DATA_IN_SIZE_INVALID_ERROR;
	}
	if (cmacResultSize < DX_UTIL_CMAC_DERV_DATA_OUT_SIZE) {
		return DX_UTIL_DATA_OUT_SIZE_INVALID_ERROR;
	}


	/********************************************************************************************************/
	/* eqviv to CRYS_AES_Init with CMAC */
	/********************************************************************************************************/
	pAesContext->alg = SEP_CRYPTO_ALG_AES;
	pAesContext->mode = SEP_CIPHER_CMAC;
	pAesContext->direction = SEP_CRYPTO_DIRECTION_ENCRYPT;
	pAesContext->crypto_key_type = aesKeyType;
	pAesContext->key_size = keySizeInBytes; // in case of root key, key size is updated in driver init code (InitCipher())
	memset(pAesContext->block_state, 0, SEP_AES_BLOCK_SIZE);

	symRc = SymDriverAdaptorInit((struct sep_ctx_generic *)pAesContext);
	if (symRc != 0) {
		return SymAdaptor2CmacDeriveKeyErr(symRc); 
	}


	/********************************************************************************************************/
	/* eqviv to CRYS_AES_Finish with CMAC                                    				*/
	/********************************************************************************************************/
	/* set the data unit size if first block */
	pAesContext->data_unit_size = DataInSize;
	symRc = SymDriverAdaptorFinalize((struct sep_ctx_generic *)pAesContext,
					pDataIn, pCmacResult, DataInSize);

	if (symRc != 0) {
		return SymAdaptor2CmacDeriveKeyErr(symRc); 
	}

	return DX_UTIL_OK; 
}

