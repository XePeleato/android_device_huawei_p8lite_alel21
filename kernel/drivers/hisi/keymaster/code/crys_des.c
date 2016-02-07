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
#include "crys_des.h"
#include "crys_des_error.h"
#include "sym_adaptor_driver.h"
#include "dma_buffer.h"
#include "cc_acl.h"
#include "dx_error.h"
#include "crys_context_relocation.h"

#if ( SEP_CTX_SIZE_WORDS > CRYS_DES_USER_CTX_SIZE_IN_WORDS )
#error CRYS_DES_USER_CTX_SIZE_IN_WORDS is not defined correctly.
#endif

/*!
 * Converts Symmetric Adaptor return code to CRYS error code.
 * 
 * \param symRetCode Symmetric Adaptor return error.
 * \param errorInfo Ignored.
 *  
 * \return CRYSError_t one of CRYS_* error codes defined in crys_error.h
 */
static CRYSError_t SymAdaptor2CrysDesErr(int symRetCode, DxUint32_t errorInfo)
{
	switch (symRetCode) {
	case DX_RET_UNSUPP_ALG:
		return CRYS_DES_IS_NOT_SUPPORTED;
	case DX_RET_UNSUPP_ALG_MODE:
	case DX_RET_UNSUPP_OPERATION:
		return CRYS_DES_ILLEGAL_OPERATION_MODE_ERROR;
	case DX_RET_INVARG:
	case DX_RET_INVARG_QID:
		return CRYS_DES_ILLEGAL_PARAMS_ERROR;
	case DX_RET_INVARG_KEY_SIZE:
		return CRYS_DES_ILLEGAL_NUM_OF_KEYS_ERROR;
	case DX_RET_INVARG_CTX_IDX:
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;
	case DX_RET_INVARG_CTX:
		return CRYS_DES_USER_CONTEXT_CORRUPTED_ERROR;
	case DX_RET_INVARG_BAD_ADDR:
		return CRYS_DES_DATA_IN_POINTER_INVALID_ERROR;
	case DX_RET_NOMEM:
		return CRYS_OUT_OF_RESOURCE_ERROR;
	case DX_RET_INVARG_INCONSIST_DMA_TYPE:
	case DX_RET_PERM:
	case DX_RET_NOEXEC:
	case DX_RET_BUSY:
	case DX_RET_OSFAULT:
	default:
		return CRYS_FATAL_ERROR;
	}
}

static enum sep_cipher_mode MakeSepDesMode(CRYS_DES_OperationMode_t OperationMode)
{
	enum sep_cipher_mode result;

	switch(OperationMode) {
	case CRYS_DES_ECB_mode:
		result = SEP_CIPHER_ECB;
		break;
	case CRYS_DES_CBC_mode:
		result = SEP_CIPHER_CBC;
		break;
	default:
		result = SEP_CIPHER_NULL_MODE;
	}

	return result;
}

/**
 * @brief This function is used to initialize the DES machine.
 *        To operate the DES machine, this should be the first function called.
 *
 * @param[in] ContextID_ptr  - A pointer to the DES context buffer allocated by the user 
 *                       that is used for the DES machine operation.
 *
 * @param[in,out] IV_ptr - The buffer of the IV.
 *                          In ECB mode this parameter is not used.
 *                          In CBC this parameter should contain the IV values.
 *
 * @param[in] Key_ptr - A pointer to the user's key buffer.
 *
 * @param[in] NumOfKeys - The number of keys used: 1, 2, or 3 (defined in the enum).
 *
 * @param[in] EncryptDecryptFlag - A flag that determines whether the DES should perform 
 *                           an Encrypt operation (0) or a Decrypt operation (1).
 *
 * @param[in] OperationMode - The operation mode: ECB or CBC.
 *
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, 
 *                        and on failure a value from crys_error.h
 */

CIMPORT_C CRYSError_t  CRYS_DES_Init( CRYS_DESUserContext_t *ContextID_ptr,
				  CRYS_DES_Iv_t            IV_ptr,
				  CRYS_DES_Key_t           *Key_ptr,
				  CRYS_DES_NumOfKeys_t     NumOfKeys,
				  CRYS_DES_EncryptMode_t   EncryptDecryptFlag,
				  CRYS_DES_OperationMode_t OperationMode )
{
	int symRc = DX_RET_OK;
	
	/* pointer on SEP DES context struct*/
	struct sep_ctx_cipher *pDesContext;

	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */
	
	/* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL ) {
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;
	}
	  
	/* check if the operation mode is legal */
	if( OperationMode >= CRYS_DES_NumOfModes ) {
		return CRYS_DES_ILLEGAL_OPERATION_MODE_ERROR;
	}
	  
	/* if the operation mode selected is CBC then check the validity of
	  the IV counter pointer */    
	if( (OperationMode == CRYS_DES_CBC_mode) && (IV_ptr == DX_NULL) ) {
		return CRYS_DES_INVALID_IV_PTR_ON_NON_ECB_MODE_ERROR;
	}
	  
	/* If the number of keys in invalid return an error */
	if( (NumOfKeys >= CRYS_DES_NumOfKeysOptions) || (NumOfKeys == 0) ) {
		return CRYS_DES_ILLEGAL_NUM_OF_KEYS_ERROR;
	}
	 
	/*check the valisity of the key pointer */
	if( Key_ptr == DX_NULL ) {
		return CRYS_DES_INVALID_KEY_POINTER_ERROR;
	}
	  
	/* Check the Encrypt / Decrypt flag validity */
	if( EncryptDecryptFlag >= CRYS_DES_EncryptNumOfOptions ) {
		return CRYS_DES_INVALID_ENCRYPT_MODE_ERROR;
	}

	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ, Key_ptr, NumOfKeys * SEP_DES_ONE_KEY_SIZE) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_DESUserContext_t)) ||
	     ((IV_ptr != DX_NULL) && (DxCcAcl_IsBuffAccessOk(ACCESS_READ, IV_ptr, sizeof(CRYS_DES_Iv_t)))) ) {
		return CRYS_DES_ILLEGAL_PARAMS_ERROR;
	}
	/* Get pointer to contiguous context in the HOST buffer */ 
	pDesContext = (struct sep_ctx_cipher *)DX_InitUserCtxLocation(ContextID_ptr->buff,
						  sizeof(CRYS_DESUserContext_t), 
						  sizeof(struct sep_ctx_cipher));
	pDesContext->alg = SEP_CRYPTO_ALG_DES;
	pDesContext->mode = MakeSepDesMode(OperationMode);
	pDesContext->direction = (enum sep_crypto_direction)EncryptDecryptFlag;
	pDesContext->key_size = NumOfKeys * SEP_DES_BLOCK_SIZE;

	DX_PAL_MemCopy(pDesContext->key, Key_ptr, pDesContext->key_size);

	if (pDesContext->mode == SEP_CIPHER_CBC) {
		DX_PAL_MemCopy(pDesContext->block_state, IV_ptr, CRYS_DES_IV_SIZE_IN_BYTES);
	}

	symRc = SymDriverAdaptorInit((struct sep_ctx_generic *)pDesContext);
	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysDesErr);
}

/**
 * @brief This function is used to process a block on the DES machine.
 *        This function should be called after the CRYS_DES_Init function was called.
 *      
 *
 * @param[in] ContextID_ptr - a pointer to the DES context buffer allocated by the user that
 *                       is used for the DES machine operation. this should be the same context that was
 *                       used on the previous call of this session.
 *
 * @param[in] DataIn_ptr - The pointer to the buffer of the input data to the DES. The pointer does 
 *                         not need to be aligned.
 *
 * @param[in] DataInSize - The size of the input data in bytes: must be not 0 and must be multiple 
 *                         of 8 bytes.
 *
 * @param[in/out] DataOut_ptr - The pointer to the buffer of the output data from the DES. The pointer does not 
 *                              need to be aligned.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_des_error.h
 */
CIMPORT_C CRYSError_t  CRYS_DES_Block( CRYS_DESUserContext_t	*ContextID_ptr,
				       DxUint8_t 		*DataIn_ptr,
				       DxUint32_t 		DataInSize,
				       DxUint8_t 		*DataOut_ptr )
{
	int symRc = DX_RET_OK;

	 /* pointer on SEP DES context struct*/
	struct sep_ctx_cipher *pDesContext;
	/* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL ) {
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;
	}
	
	/* if the users Data In pointer is illegal return an error */
	if( DataIn_ptr == DX_NULL ) {
		return CRYS_DES_DATA_IN_POINTER_INVALID_ERROR;
	}
	
	/* if the users Data Out pointer is illegal return an error */
	if( DataOut_ptr == DX_NULL ) {
		return CRYS_DES_DATA_OUT_POINTER_INVALID_ERROR;
	}

	/* data size must be a positive number and a block size mult */
	if (((DataInSize % CRYS_DES_BLOCK_SIZE_IN_BYTES) != 0) || (DataInSize == 0)) {
		return CRYS_DES_DATA_SIZE_ILLEGAL; 
	}

	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_DESUserContext_t))) {
		return CRYS_DES_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pDesContext = (struct sep_ctx_cipher *)DX_GetUserCtxLocation(ContextID_ptr->buff);

	symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pDesContext,
					DataIn_ptr, DataOut_ptr, DataInSize);
	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysDesErr);
}
                              
/**
 * @brief This function is used to end the DES processing session.
 *        It is the last function called for the DES process.
 *      
 *
 * @param[in] ContextID_ptr  - A pointer to the DES context buffer allocated by the user that
 *                       is used for the DES machine operation. this should be the 
 *                       same context that was used on the previous call of this session.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, 
 *                        and on failure a value from crys_error.h
 */
CIMPORT_C CRYSError_t  CRYS_DES_Free(CRYS_DESUserContext_t  *ContextID_ptr)
{
	/* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL ) {
		 return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_DESUserContext_t))) {
		return CRYS_DES_ILLEGAL_PARAMS_ERROR;
	}

	DX_PAL_MemSetZero(ContextID_ptr, sizeof(CRYS_DESUserContext_t));

	return CRYS_OK;
}

/**
 * @brief This function is used to operate the DES machine in one integrated operation.
 *
 *        The actual macros that will be used by the users are:
 *      
 *
 * @param[in,out] IVCounter_ptr - this parameter is the buffer of the IV or counters on mode CTR.
 *                          On ECB mode this parameter has no use.
 *                          On CBC mode this parameter should containe the IV values.
 *
 * @param[in] Key_ptr - a pointer to the users key buffer.
 *
 * @param[in] KeySize - Thenumber of keys used by the DES as defined in the enum.
 *
 * @param[in] EncryptDecryptFlag - This flag determains if the DES shall perform an Encrypt operation [0] or a
 *                           Decrypt operation [1].
 *
 * @param[in] OperationMode - The operation mode : ECB or CBC.
 *
 * @param[in] DataIn_ptr - The pointer to the buffer of the input data to the DES. The pointer does 
 *                         not need to be aligned.
 *
 * @param[in] DataInSize - The size of the input data in bytes: must be not 0 and must be multiple 
 *                         of 8 bytes.
 *
 * @param[in/out] DataOut_ptr - CRYS_DES_BLOCK_SIZE_IN_BYTES The pointer to the 
 *      			buffer of the output data from the DES. The
 *      			pointer does not need to be aligned.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_des_error.h
 *
 */
 CIMPORT_C CRYSError_t  CRYS_DES(
                CRYS_DES_Iv_t             IV_ptr,
                CRYS_DES_Key_t           *Key_ptr,
                CRYS_DES_NumOfKeys_t      NumOfKeys,
                CRYS_DES_EncryptMode_t    EncryptDecryptFlag,
                CRYS_DES_OperationMode_t  OperationMode,
                DxUint8_t                *DataIn_ptr,
                DxUint32_t                DataInSize,
                DxUint8_t                *DataOut_ptr )
{
	CRYS_DESUserContext_t UserContext;
	CRYSError_t Error = CRYS_OK;

	/* if no data to process -we're done */
	if (DataInSize == 0) {
		goto end;
	}

	Error = CRYS_DES_Init(&UserContext, IV_ptr, Key_ptr, NumOfKeys, EncryptDecryptFlag, OperationMode);
	if(Error != CRYS_OK) {
		goto end;
	}

	Error = CRYS_DES_Block(&UserContext, DataIn_ptr, DataInSize, DataOut_ptr);
	if(Error != CRYS_OK) {
		goto end;
	}

	Error = CRYS_DES_Free(&UserContext);

end:
	return Error;
}
