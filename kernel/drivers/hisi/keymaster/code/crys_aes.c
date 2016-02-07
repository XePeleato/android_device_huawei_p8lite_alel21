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
#include "crys_aes.h"
#include "crys_aes_error.h"
#include "sym_adaptor_driver.h"
#include "cipher.h"
#include "sep_ctx.h"
#include "dma_buffer.h"
#include "key_buffer.h"
#include "cc_acl.h"
#include "dx_error.h"
#include "crys_context_relocation.h"


#if ( SEP_CTX_SIZE_WORDS > CRYS_AES_USER_CTX_SIZE_IN_WORDS )
#error CRYS_AES_USER_CTX_SIZE_IN_WORDS is not defined correctly.
#endif


/*!
 * Converts Symmetric Adaptor return code to CRYS error code.
 * 
 * \param symRetCode Symmetric Adaptor return error.
 * \param errorInfo Ignored.
 *  
 * \return CRYSError_t one of CRYS_* error codes defined in crys_error.h
 */
static CRYSError_t SymAdaptor2CrysAesErr(int symRetCode, DxUint32_t errorInfo)
{
	switch (symRetCode) {
	case DX_RET_UNSUPP_ALG:
		return CRYS_AES_IS_NOT_SUPPORTED;
	case DX_RET_UNSUPP_ALG_MODE:
	case DX_RET_UNSUPP_OPERATION:
		return CRYS_AES_ILLEGAL_OPERATION_MODE_ERROR;
	case DX_RET_INVARG:
	case DX_RET_INVARG_QID:
		return CRYS_AES_ILLEGAL_PARAMS_ERROR;
	case DX_RET_INVARG_KEY_SIZE:
		return CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
	case DX_RET_INVARG_CTX_IDX:
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;
	case DX_RET_INVARG_CTX:
		return CRYS_AES_USER_CONTEXT_CORRUPTED_ERROR;
	case DX_RET_INVARG_BAD_ADDR:
		return CRYS_AES_DATA_IN_POINTER_INVALID_ERROR;
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

static enum sep_cipher_mode MakeSepAesMode(CRYS_AES_OperationMode_t OperationMode)
{
	enum sep_cipher_mode result;

	switch(OperationMode) {
	case CRYS_AES_ECB_mode:
		result = SEP_CIPHER_ECB;
		break;
	case CRYS_AES_CBC_mode:
		result = SEP_CIPHER_CBC;
		break;
	case CRYS_AES_MAC_mode:
		result = SEP_CIPHER_CBC_MAC;
		break;
	case CRYS_AES_CTR_mode:
		result = SEP_CIPHER_CTR;
		break;
	case CRYS_AES_XCBC_MAC_mode:
		result = SEP_CIPHER_XCBC_MAC;
		break;
	case CRYS_AES_CMAC_mode:
		result = SEP_CIPHER_CMAC;
		break;
	case CRYS_AES_XTS_mode:
		result = SEP_CIPHER_XTS;
		break;
	case CRYS_AES_OFB_mode:
		result = SEP_CIPHER_OFB;
		break;
	case CRYS_AES_CBC_CTS_mode:
		result = SEP_CIPHER_CBC_CTS;
		break;
	default:
		result = SEP_CIPHER_NULL_MODE;
	}

	return result;
}

/****************************************************************************************************/
/** 
 * @brief This function is used to initialize the AES machine or SW structures.
 *        To perform the AES operations this should be the first function called.
 *
 *        The actual macros, that will be used by the user for calling this function, are described 
 *        in crys_aes.h file.
 *
 * @param[in] ContextID_ptr - A pointer to the AES context buffer that is allocated by the user 
 *                            and is used for the AES machine operation.
 * @param[in] IVCounter_ptr - A buffer containing an initial value: IV, Counter or Tweak according 
 *                            to operation mode:
 *                            - on ECB, XCBC, CMAC mode this parameter is not used and may be NULL,
 *                            - on CBC and MAC modes it contains the IV value,
 *                            - on CTR and OFB modes it contains the init counter,
 *                            - on XTS mode it contains the initial tweak value - 128-bit consecutive number 
 *                              of data unit (in little endian).
 * @param[in] Key_ptr  -  A pointer to the user's key buffer.
 * @param[in] KeySize  -  An enum parameter, defines size of used key (128, 192, 256, 512 bits):
 *                        On XCBC mode allowed 128 bit size only, on XTS - 256 or 512 bit, on other modes <= 256 bit.
 * @param[in] EncryptDecryptFlag - A flag specifying whether the AES should perform an Encrypt operation (0) 
 *                                 or a Decrypt operation (1). In XCBC, MAC and CMAC modes it must be Encrypt.
 * @param[in] OperationMode - The operation mode: ECB, CBC, MAC, CTR, OFB, XCBC (PRF and 96), CMAC.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 */
CIMPORT_C CRYSError_t  CRYS_AES_Init(  
	CRYS_AESUserContext_t    *ContextID_ptr,
	CRYS_AES_IvCounter_t     IVCounter_ptr,
	CRYS_AES_Key_t           Key_ptr,
	CRYS_AES_KeySize_t       KeySizeID,
	CRYS_AES_EncryptMode_t   EncryptDecryptFlag,
	CRYS_AES_OperationMode_t OperationMode )
{
	int symRc;
	
	/* Aes key size bytes */
	DxUint32_t keySizeBytes = 0;

	/* pointer on SEP AES context struct*/
	struct sep_ctx_cipher *pAesContext;

	uint32_t keyAddr;
	DX_CRYPTO_KEY_TYPE_t cryptoKeyType;
	KeyPtrType_t keyPtrType;

	/* FUNCTION LOGIC */

	/* ............... local initializations .............................. */
	/* -------------------------------------------------------------------- */

	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */

	/* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL ) {
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	/* check if the operation mode is legal */
	if( OperationMode >= CRYS_AES_NumOfModes || OperationMode == CRYS_AES_CCM_mode) {
		return  CRYS_AES_ILLEGAL_OPERATION_MODE_ERROR;
	}

	/* check if the OFB mode is supported */
#ifdef CRYS_NO_AES_OFB_SUPPORT
	if(OperationMode == CRYS_AES_OFB_mode) {
		return  CRYS_AES_ILLEGAL_OPERATION_MODE_ERROR;
	}
#endif

	/* if the operation mode selected is CBC,CTS, MAC, CTR, XTS or OFB then check the validity of
	the IV counter pointer (note: on XTS mode it is the Tweak pointer) */    
	if( ((OperationMode == CRYS_AES_CBC_mode)  ||
	     (OperationMode == CRYS_AES_CTR_mode)  ||
	     (OperationMode == CRYS_AES_MAC_mode)  ||
	     (OperationMode == CRYS_AES_XTS_mode)  ||
	     (OperationMode == CRYS_AES_CBC_CTS_mode)  ||
	     (OperationMode == CRYS_AES_OFB_mode)) &&
	     (IVCounter_ptr == DX_NULL) ) {
		return  CRYS_AES_INVALID_IV_OR_TWEAK_PTR_ERROR;     
	}
	/* in XCBC mode enable only key size = 128 bit */
	if( (OperationMode == CRYS_AES_XCBC_MAC_mode) &&
	   (KeySizeID != CRYS_AES_Key128BitSize) ) {
		return  CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
	}

	/* check the Encrypt / Decrypt flag validity */
	if ( EncryptDecryptFlag >= CRYS_AES_EncryptNumOfOptions ) {
		return  CRYS_AES_INVALID_ENCRYPT_MODE_ERROR;
	}
	/* in MAC,XCBC,CMAC modes enable only encrypt mode  */
	if( ((OperationMode == CRYS_AES_XCBC_MAC_mode) || 
	     (OperationMode == CRYS_AES_CMAC_mode) || 
	     (OperationMode == CRYS_AES_MAC_mode)) &&  
	   (EncryptDecryptFlag != CRYS_AES_Encrypt) ) {
		return  CRYS_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE;
	}


	/*  check the validity of the key pointer */  
	if ( Key_ptr == DX_NULL ) {
		return  CRYS_AES_INVALID_KEY_POINTER_ERROR;
	}

	if(getKeyDataFromKeyObj((uint8_t*)Key_ptr, &keyAddr, &cryptoKeyType, &keyPtrType, DX_AES_API_INIT) != CRYS_OK)
		return CRYS_AES_INVALID_KEY_POINTER_ERROR;


	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ, (uint8_t *)keyAddr, KeySizeID * SEP_AES_128_BIT_KEY_SIZE) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_AESUserContext_t)) ||
	     ((IVCounter_ptr != NULL) && DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, IVCounter_ptr, sizeof(CRYS_AES_IvCounter_t))) ) {
		return CRYS_AES_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pAesContext = (struct sep_ctx_cipher *)DX_InitUserCtxLocation(ContextID_ptr->buff,
								      sizeof(CRYS_AESUserContext_t),
								      sizeof(struct sep_ctx_cipher));

	pAesContext->alg = SEP_CRYPTO_ALG_AES;
	pAesContext->mode = MakeSepAesMode(OperationMode);
	pAesContext->direction = (enum sep_crypto_direction)EncryptDecryptFlag;
	pAesContext->key_size = 0;
	pAesContext->crypto_key_type = cryptoKeyType;

	/* check key size in XTS mode  */
	if ( OperationMode == CRYS_AES_XTS_mode ) {
		if( (KeySizeID != CRYS_AES_Key256BitSize) && 
		    (KeySizeID != CRYS_AES_Key512BitSize) ) {
			return  CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
		}
	} else if ( KeySizeID > CRYS_AES_Key256BitSize ) {
		/* check the max key size for all modes besides XTS */
		return  CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
	}
		
	/* get AES_Key size in bytes */
	switch( KeySizeID ) {
	case CRYS_AES_Key128BitSize:
		keySizeBytes = 16;
		break;
		
	case CRYS_AES_Key192BitSize:
		keySizeBytes = 24;   
		break;
	
	case CRYS_AES_Key256BitSize:
		keySizeBytes = 32;   
		break;    
	
	case CRYS_AES_Key512BitSize:
		keySizeBytes = 64;   
		break;    
	
	default:
		return CRYS_AES_ILLEGAL_KEY_SIZE_ERROR; /*for preventing compiler warnings*/
	}
	
	/* HDCP max allowed key size is 16 bytes */
	if ((pAesContext->crypto_key_type == DX_XOR_HDCP_KEY) && (keySizeBytes > 16)) {
		return CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
	}
	DX_PAL_MemCopy(pAesContext->key, (uint8_t *)keyAddr, keySizeBytes);
	pAesContext->key_size = keySizeBytes;
	

	if (pAesContext->mode == SEP_CIPHER_XTS ) {
		/* Divide by two (we have two keys of the same size) */
		pAesContext->key_size >>= 1;
		/* copy second half of the double-key as XEX-key */
		DX_PAL_MemCopy(pAesContext->xex_key, (uint8_t*)keyAddr + pAesContext->key_size, pAesContext->key_size);
	}

	
	switch(pAesContext->mode) {
	case SEP_CIPHER_CBC:
	case SEP_CIPHER_CTR:
	case SEP_CIPHER_XTS:
	case SEP_CIPHER_CBC_MAC:
	case SEP_CIPHER_OFB:
	case SEP_CIPHER_CBC_CTS:
		DX_PAL_MemCopy(pAesContext->block_state, IVCounter_ptr, CRYS_AES_IV_COUNTER_SIZE_IN_BYTES);
		/* CRYS has no information on XTS data unit size.
		   Setting this field to 0 tells the SEP-AES driver to set 
		   the data_unit_size based on size of first data unit */
		pAesContext->data_unit_size = 0;
		break;
	case SEP_CIPHER_XCBC_MAC:
	case SEP_CIPHER_CMAC:
		DX_PAL_MemSetZero(pAesContext->block_state, CRYS_AES_IV_COUNTER_SIZE_IN_BYTES);
		break;
	default:
		break;
	}

	symRc = SymDriverAdaptorInit((struct sep_ctx_generic *)pAesContext);


	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesErr);
}

/****************************************************************************************************/
/** 
 * @brief This function is used to operate a block of data on the SW or on AES machine.
 *        This function should be called after the appropriate CRYS AES init function 
 *        (according to used AES operation mode).
 *
 * @param[in] ContextID_ptr - A pointer to the AES context buffer allocated by the user that
 *                            is used for the AES machine operation. This should be the same context that was
 *                            used on the previous call of this session.
 *
 * @param[in] DataIn_ptr - A pointer to the buffer of the input data to the AES. The pointer does 
 *                         not need to be aligned. On CSI input mode the pointer must be equal to
 *                         value (0xFFFFFFFC | DataInAlignment). 
 *
 * @param[in] DataInSize - A size of the input data must be multiple of 16 bytes and not 0, 
 *                         on all modes. Note last chunk (block) of data must be processed by 
 *                         CRYS_AES_Finish function but not by CRYS_AES_Block function;  
 *
 * @param[out] DataOut_ptr - A pointer to the buffer of the output data from the AES. The pointer  does not 
 *                             need to be aligned. On CSI output mode the pointer must be equal to
 *                             value (0xFFFFFFFC | DataOutAlignment). On all MAC modes (MAC,XCBC, CMAC) CSI 
 *                             output is not allowed.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                       value MODULE_* CRYS_DES_error.h
 *
 *     NOTES: 1. Temporarily it is not allowed, that both the Input and the Output simultaneously 
 *               were on CSI mode.
 *            2. Temporarily the CSI input or output are not allowed on XCBC, CMAC and XTS modes.    
 */
CIMPORT_C CRYSError_t  CRYS_AES_Block( CRYS_AESUserContext_t   *ContextID_ptr,   
                                       DxUint8_t               *DataIn_ptr,     
                                       DxUint32_t              DataInSize,     
                                       DxUint8_t               *DataOut_ptr )
{
	int symRc;

	 /* pointer on SEP AES context struct*/
	struct sep_ctx_cipher *pAesContext = NULL;
	void *pOutData = NULL;

	/* if the users context ID pointer is DX_NULL return an error */
	if ( ContextID_ptr == DX_NULL ) {
		 return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	/* if the users Data In pointer is illegal return an error */
	if ( DataIn_ptr == DX_NULL ) {
		return CRYS_AES_DATA_IN_POINTER_INVALID_ERROR;
	}

	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_AESUserContext_t)) ) {
		return CRYS_AES_ILLEGAL_PARAMS_ERROR;
	}

	/* Get the pointer to contiguous context in the HOST buffer */ 
	pAesContext = (struct sep_ctx_cipher *)DX_GetUserCtxLocation(ContextID_ptr->buff);
	/* Size zero is not a valid block operation */
	if (DataInSize == 0) {
		return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
	}
	
	/* check the minimum data size according to mode */
	if ((pAesContext->mode == SEP_CIPHER_XTS) && (DataInSize < 2*CRYS_AES_BLOCK_SIZE_IN_BYTES)) {
			DX_PAL_LOG_ERR("Invalid XTS data size: %u\n", (unsigned int)DataInSize);
			return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
	}
	
	if ((pAesContext->mode != SEP_CIPHER_XTS) && ((DataInSize % CRYS_AES_BLOCK_SIZE_IN_BYTES) != 0)) {
		/* Only for XTS an intermediate data unit may be non aes block multiple */
		DX_PAL_LOG_ERR("Invalid data size: %u\n", (unsigned int)DataInSize);
		return CRYS_AES_DATA_IN_SIZE_ILLEGAL; 
	}
	
	/* set the data unit size if first block */
	if (pAesContext->data_unit_size == 0) {
		pAesContext->data_unit_size = DataInSize;
	}
	
	/* In XTS mode, all the data units must be of the same size */
	if ( (pAesContext->mode == SEP_CIPHER_XTS) && (pAesContext->data_unit_size != DataInSize)) {
		DX_PAL_LOG_ERR("Invalid XTS data size: DataInSize=%u data_unit_size=%u\n", 
				(unsigned int)DataInSize, (unsigned int)pAesContext->data_unit_size);
		return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
	}

	if( (pAesContext->mode == SEP_CIPHER_CMAC) ||
	    (pAesContext->mode == SEP_CIPHER_XCBC_MAC) ||
	    (pAesContext->mode == SEP_CIPHER_CBC_MAC) ) {
		pOutData = NULL;
	} else {
		pOutData = DataOut_ptr;
	}

	symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pAesContext,
				DataIn_ptr, pOutData, DataInSize);

	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesErr);
}
                              
/****************************************************************************************************/
/** 
 * @brief This function is used as finish operation on all AES modes.
 *
 *        The function must be called after AES_Block operations (or instead) for last chunck 
 *        of data with size > 0. 
 *
 *        The function performs all operations, including specific operations for last blocks of 
 *        data on some modes (XCBC, CMAC, MAC) and puts out the result. After all operations 
 *        the function cleans the secure sensitive data from context.  
 *        
 *        1. Checks the validation of all of the inputs of the function.
 *           If one of the received parameters is not valid it shall return an error.
 *        2. Decrypts the received context to the working context  by calling the 
 *           CRYS_CCM_GetContext function.
 *        3. Calls the LLF_AES_Finish function.
 *        4. Outputs the result and cleans working context.
 *        5. Exits   
 *      
 *
 * @param[in] ContextID_ptr - A pointer to the AES context buffer allocated by the user that
 *                            should be the same context that was used on the previous call
 *                            of this session.
 * @param[in] DataIn_ptr    - A pointer to the buffer of the input data to the AES. The pointer does 
 *                            not need to be aligned. On CSI input mode the pointer must be equal to
 *                            value (0xFFFFFFFC | DataInAlignment). 
 * @param[in] DataInSize    - A size of the input data must be:  DataInSize >= minimalSize, where: 
 *                            minimalSize =
 *                                  -  1 byte for CTR, OFB, XCBC, CMAC mode;
 *                                  - 16 bytes for other modes. 
 * @param[out] DataOut_ptr  - A pointer to the output buffer. The pointer  does not need to be aligned. 
 *                            On CSI output mode the pointer must be equal to value 
 *                            (0xFFFFFFFC | DataOutAlignment). On some modes (MAC,XCBC,CMAC,XTS) 
 *                            CSI output is not allowed. Temporarily is not allowed, that both the 
 *                            Input and the output are on CSI mode simultaneously. 
 *                            The size of the output buffer must be not less than:  
 *                                - 16 bytes for MAC, XCBC, CMAC modes;
 *                                - DataInSize for ECB,CBC,CTR,XTS,OFB modes. 
 *
 * @return CRYSError_t    - On success CRYS_OK is returned, on failure - a value defined in crys_aes_error.h.
 *
 */
CIMPORT_C CRYSError_t  CRYS_AES_Finish( CRYS_AESUserContext_t   *ContextID_ptr,  
                                        DxUint8_t               *DataIn_ptr,     
                                        DxUint32_t               DataInSize,     
                                        DxUint8_t               *DataOut_ptr )
{
	/* pointer on SEP AES context struct*/
	struct sep_ctx_cipher *pAesContext = NULL;
	int symRc;
	DxUint32_t DataSizeForBlock = 0;
	DxUint8_t LastBlockSize=0;
	#ifdef  CRYS_USE_DMA_OBJECT
	DmaBuffer_s *pDinDmaBuff = NULL;
	#endif

	 /* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL ) {
		 return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pAesContext = (struct sep_ctx_cipher *)DX_GetUserCtxLocation(ContextID_ptr->buff);


	if ((DataInSize % CRYS_AES_BLOCK_SIZE_IN_BYTES != 0) &&
	    ((pAesContext->mode == SEP_CIPHER_ECB) || 
	     (pAesContext->mode == SEP_CIPHER_CBC) || 
	     (pAesContext->mode == SEP_CIPHER_CBC_MAC))) {
		return CRYS_AES_DATA_IN_SIZE_ILLEGAL; 
	}
	/*Check, that in case of CTS mode data size is not less than CRYS_AES_BLOCK_SIZE_IN_BYTES */
	if ((DataInSize < CRYS_AES_BLOCK_SIZE_IN_BYTES)&&
	    (pAesContext->mode == SEP_CIPHER_CBC_CTS)) {
		return CRYS_AES_DATA_IN_SIZE_ILLEGAL; 
	}	


	/* set the data unit size if first block */
	if (pAesContext->data_unit_size == 0) {
		pAesContext->data_unit_size = DataInSize;
	} 
	
	if ( (pAesContext->mode == SEP_CIPHER_XTS) && (DataInSize != 0)) {
		/* For XTS all the data units must be of the same size */
		if ((DataInSize < 2*CRYS_AES_BLOCK_SIZE_IN_BYTES) ||
		    (pAesContext->data_unit_size != DataInSize)) {
			DX_PAL_LOG_ERR("Invalid XTS data size: DataInSize=%u data_unit_size=%u\n", 
				(unsigned int)DataInSize, (unsigned int)pAesContext->data_unit_size);
			return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
		}
	}
	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_AESUserContext_t)) )
		return CRYS_AES_ILLEGAL_PARAMS_ERROR;


	if((pAesContext->mode == SEP_CIPHER_CBC_CTS)&&(DataInSize>CRYS_AES_BLOCK_SIZE_IN_BYTES*2))
	{
		LastBlockSize = DataInSize&0xf;
		if (LastBlockSize == 0)
			LastBlockSize = CRYS_AES_BLOCK_SIZE_IN_BYTES;
		DataSizeForBlock = DataInSize - (CRYS_AES_BLOCK_SIZE_IN_BYTES+LastBlockSize);

	#ifdef  CRYS_USE_DMA_OBJECT
			if (IS_SMART_PTR(DataIn_ptr)) {
				pDinDmaBuff = PTR_TO_DMA_BUFFER(DataIn_ptr);
				pDinDmaBuff->size = DataSizeForBlock;
			} 	
			if (IS_SMART_PTR(DataOut_ptr)) {
				pDinDmaBuff = PTR_TO_DMA_BUFFER(DataOut_ptr);
				pDinDmaBuff->size = DataSizeForBlock;
			} 
	#endif
		symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pAesContext,
			DataIn_ptr, DataOut_ptr, DataSizeForBlock);

		if(symRc != DX_RET_OK) {			
			return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesErr);
		}

	#ifdef  CRYS_USE_DMA_OBJECT
		if (IS_SMART_PTR(DataIn_ptr)) {
				pDinDmaBuff = PTR_TO_DMA_BUFFER(DataIn_ptr);
				pDinDmaBuff->size = CRYS_AES_BLOCK_SIZE_IN_BYTES+LastBlockSize;
                                pDinDmaBuff->pData =pDinDmaBuff->pData+DataSizeForBlock;
		} 
        if (DataOut_ptr!=DataIn_ptr)
		if (IS_SMART_PTR(DataOut_ptr)) {
				pDinDmaBuff = PTR_TO_DMA_BUFFER(DataOut_ptr);
				pDinDmaBuff->size = CRYS_AES_BLOCK_SIZE_IN_BYTES+LastBlockSize;
				pDinDmaBuff->pData = pDinDmaBuff->pData+DataSizeForBlock;
		} 

		symRc = SymDriverAdaptorFinalize((struct sep_ctx_generic *)pAesContext,
			DataIn_ptr,DataOut_ptr,CRYS_AES_BLOCK_SIZE_IN_BYTES+LastBlockSize);
	#else

		symRc = SymDriverAdaptorFinalize((struct sep_ctx_generic *)pAesContext,
			DataIn_ptr+DataSizeForBlock, DataOut_ptr+DataSizeForBlock,CRYS_AES_BLOCK_SIZE_IN_BYTES+LastBlockSize);
	#endif

	}
	else
	{

		symRc = SymDriverAdaptorFinalize((struct sep_ctx_generic *)pAesContext,
			DataIn_ptr, DataOut_ptr, DataInSize);
	}


	if (symRc != DX_RET_OK) {
		return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesErr);
	}

 	return CRYS_OK;
}

/****************************************************************************************************/
/**
 * @brief This function is used to perform the AES operation in one integrated process.
 *
 *        The input-output parameters of the function are the following:
 *
 * @param[in] IVCounter_ptr - A buffer containing an initial value: IV, Counter or Tweak according 
 *                            to operation mode:
 *                            - on ECB, XCBC, CMAC mode this parameter is not used and may be NULL,
 *                            - on CBC and MAC modes it contains the IV value,
 *                            - on CTR and OFB modes it contains the init counter,
 *                            - on XTS mode it contains the initial tweak value - 128-bit consecutive number 
 *                              of data unit (in little endian).
 * @param[in] Key_ptr  -  A pointer to the user's key buffer.
 * @param[in] KeySize  -  An enum parameter, defines size of used key (128, 192, 256 bits).
 * @param[in] EncryptDecryptFlag - A flag specifying whether the AES should perform an Encrypt operation (0) 
 *                                 or a Decrypt operation (1). In XCBC and CMAC modes it must be 0.
 * @param[in] OperationMode - The operation mode: ECB, CBC, MAC, CTR, XCBC (PRF and 96), CMAC, XTS, OFB.
 * @param[in] DataIn_ptr - A pointer to the buffer of the input data to the AES.
 *
 * @param[in] DataInSize - The size of the input data, it must be:
 *                         - on ECB,CBC,MAC modes must be a multiple of 16 bytes
 *                         - on CTR, XCBC, CMAC and OFB modes any value
 *      		   - on XTS mode: If input or output pointers are in the D-Cache range,
 *      		     only the following data sizes are supported: 64B, 512B, 520B, 521B. Otherwise,
 *      		     1024B and 4096B are also supported.
 * @param[out] DataOut_ptr - A pointer to the buffer of the output data from the AES
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a value defined in crys_aes_error.h
 *
 */
CIMPORT_C CRYSError_t  CRYS_AES(CRYS_AES_IvCounter_t       IVCounter_ptr,       
				CRYS_AES_Key_t             Key_ptr,								
				CRYS_AES_KeySize_t         KeySize,            
				CRYS_AES_EncryptMode_t     EncryptDecryptFlag, 
				CRYS_AES_OperationMode_t   OperationMode,
				DxUint8_t                  *DataIn_ptr,        
				DxUint32_t                 DataInSize,         
				DxUint8_t                  *DataOut_ptr)
{
	CRYS_AESUserContext_t UserContext;
	CRYSError_t Error = CRYS_OK;
	uint32_t keyAddr = (uint32_t)Key_ptr;
	DX_CRYPTO_KEY_TYPE_t cryptoKeyType;
	KeyPtrType_t keyPtrType;
	DxUint32_t keySizeBytes;
	keyBuffer_t keyBuff;

	/* check, that data size is multiple of 16 bytes on relevant modes */
	if (((DataInSize % CRYS_AES_BLOCK_SIZE_IN_BYTES) != 0) &&
	    ((OperationMode == CRYS_AES_ECB_mode) ||
	     (OperationMode == CRYS_AES_CBC_mode) ||
	     (OperationMode == CRYS_AES_MAC_mode))) {
		return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
	}

	
	/* check the minimum data size according to mode */
	if ((OperationMode == CRYS_AES_XTS_mode) && (DataInSize < 2*CRYS_AES_BLOCK_SIZE_IN_BYTES)) {
			return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
	}
	/* in CMAC mode it may be applet key and we want to build it*/
	if (OperationMode == CRYS_AES_CMAC_mode) {
		/*  check the validity of the key pointer */
		if (Key_ptr == DX_NULL) {
			return  CRYS_AES_INVALID_KEY_POINTER_ERROR;
		}

		if (getKeyDataFromKeyObj((uint8_t*)Key_ptr, &keyAddr, &cryptoKeyType, &keyPtrType, DX_AES_CMAC) != CRYS_OK) {
			return CRYS_AES_INVALID_KEY_POINTER_ERROR;
		}

		Error = buildKeyInt((struct sep_ctx_cipher *)UserContext.buff, &KeySize, keyBuff, &keyAddr, DX_AES_CMAC, cryptoKeyType, &keySizeBytes);
		if(Error != CRYS_OK) {
			return CRYS_AES_INVALID_KEY_POINTER_ERROR;
		}
			
	}
	Error = CRYS_AES_Init(&UserContext, IVCounter_ptr, (uint8_t*)keyAddr, KeySize, EncryptDecryptFlag, OperationMode);
	if(Error != CRYS_OK) {
		goto end;
	}

		Error = CRYS_AES_Finish(&UserContext, DataIn_ptr, DataInSize, DataOut_ptr);

end:
	return Error;
}

/**************************************************************************
 *	              CRYS_AES_SetIv function                                *
 **************************************************************************/
/**
   @brief  The CRYS_AES_SetIv function puts a new initial vector into
   an existing context.

   Inputs:  New IV vector
  
   Outputs: Result
   
   @param[in/out] ContextID_ptr - A pointer to the AES context buffer that is allocated by the user 
                                  and is used for the AES machine operation.
			   
   @return CRYSError_t - CRYS_OK, or error message 
			 CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR          
			 
*/                               
CIMPORT_C  CRYSError_t CRYS_AES_SetIv(CRYS_AESUserContext_t   *ContextID_ptr,  
		                       DxUint8_t *iv_ptr)
{
	/* Get pointer to contiguous context in the HOST buffer */ 
	struct sep_ctx_cipher *pAesContext = (struct sep_ctx_cipher *)DX_GetUserCtxLocation(ContextID_ptr->buff);
	
	if ((pAesContext->mode != SEP_CIPHER_CBC) &&
	(pAesContext->mode != SEP_CIPHER_CTR) &&
	(pAesContext->mode != SEP_CIPHER_XTS) &&
	(pAesContext->mode != SEP_CIPHER_CBC_CTS) &&
	(pAesContext->mode != SEP_CIPHER_OFB) ) {
		return CRYS_AES_ILLEGAL_OPERATION_MODE_ERROR;
	}
	
	if (pAesContext == DX_NULL) { 
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;
	}
	
	if (iv_ptr == DX_NULL ||
	    /* Support IV pointer in SeP memories only */
	    (IS_SMART_PTR(iv_ptr) && (PTR_TO_DMA_BUFFER(iv_ptr)->dmaBufType != DMA_BUF_SEP))) {
		return CRYS_AES_INVALID_IV_OR_TWEAK_PTR_ERROR;
	}
	
	if (IS_SMART_PTR(iv_ptr)) /* Translate to plain pointer */
		iv_ptr = (DxUint8_t *)PTR_TO_DMA_BUFFER(iv_ptr)->pData;

	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, iv_ptr, sizeof(CRYS_AES_IvCounter_t)) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, pAesContext, sizeof(struct sep_ctx_cipher)) ) {
		return CRYS_AES_ILLEGAL_PARAMS_ERROR;
	}

	DX_PAL_MemCopy(pAesContext->block_state, iv_ptr, sizeof(CRYS_AES_IvCounter_t));
	
	return CRYS_OK;
}
 


/**************************************************************************
 *	              CRYS_AES_GetIv function                                *
 **************************************************************************/
/**
   @brief  The CRYS_AES_GetIv function retrieves the initial vector from
   the context.

   Inputs:  IV vector buffer
  
   Outputs: Result
   
   @param[in/out] ContextID_ptr - A pointer to the AES context buffer that is allocated by the user 
                                  and is used for the AES machine operation.
			   
   @return CRYSError_t - CRYS_OK, or error message 
			 CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR          
			 
*/                               
CIMPORT_C CRYSError_t CRYS_AES_GetIv(CRYS_AESUserContext_t   *ContextID_ptr,  
		                       DxUint8_t *iv_ptr)
{
	/* Get pointer to contiguous context in the HOST buffer */ 
   	struct sep_ctx_cipher *pAesContext = (struct sep_ctx_cipher *)DX_GetUserCtxLocation(ContextID_ptr->buff);

	if ((pAesContext->mode != SEP_CIPHER_CBC) &&
	(pAesContext->mode != SEP_CIPHER_CTR) &&
	(pAesContext->mode != SEP_CIPHER_XTS) &&
	(pAesContext->mode != SEP_CIPHER_CBC_CTS) &&
	(pAesContext->mode != SEP_CIPHER_OFB)) {
		return CRYS_AES_ILLEGAL_OPERATION_MODE_ERROR;
	}
	
	if (pAesContext == DX_NULL) { 
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;
	}
	
	if (iv_ptr == DX_NULL ||
	    /* Support IV pointer in SeP memories only */
	    (IS_SMART_PTR(iv_ptr) && (PTR_TO_DMA_BUFFER(iv_ptr)->dmaBufType != DMA_BUF_SEP))) {
		return CRYS_AES_INVALID_IV_OR_TWEAK_PTR_ERROR;
	}

	if (IS_SMART_PTR(iv_ptr)) /* Translate to plain pointer */
		iv_ptr = (DxUint8_t *)PTR_TO_DMA_BUFFER(iv_ptr)->pData;

	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, iv_ptr, sizeof(CRYS_AES_IvCounter_t)) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, pAesContext, sizeof(struct sep_ctx_cipher)) ) {		
		return CRYS_AES_ILLEGAL_PARAMS_ERROR;
	}
	
	DX_PAL_MemCopy(iv_ptr, pAesContext->block_state, sizeof(CRYS_AES_IvCounter_t));
	
	return CRYS_OK;
}



