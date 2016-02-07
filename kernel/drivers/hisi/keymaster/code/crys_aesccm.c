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

#include "crys_aesccm.h"
#include "crys_aesccm_error.h"
#include "aead.h"
#include "sep_ctx.h"
#include "sym_adaptor_driver.h"
#include "crys_bypass_api.h"
#include "dx_pal_mem.h"
#include "dma_buffer.h"
#include "key_buffer.h"
#include "cc_acl.h"
#include "dx_error.h"
#include "crys_context_relocation.h"


/************************ Defines ******************************/
#if ( SEP_CTX_SIZE_WORDS > CRYS_AESCCM_USER_CTX_SIZE_IN_WORDS )
#error CRYS_AESCCM_USER_CTX_SIZE_IN_WORDS is not defined correctly.
#endif

/* Since the user context in the TEE is doubled to allow it to be contiguous we must get */
/*  the real size of the context (SEP context) to get the private context pointer  */
#ifdef DX_CC_TEE
#define CRYS_AESCCM_USER_CTX_ACTUAL_SIZE_IN_WORDS	((CRYS_AESCCM_USER_CTX_SIZE_IN_WORDS - 3)/2)
#else
#define CRYS_AESCCM_USER_CTX_ACTUAL_SIZE_IN_WORDS	CRYS_AESCCM_USER_CTX_SIZE_IN_WORDS
#endif

/************************ Type definitions **********************/
#define AESCCM_PRIVATE_CONTEXT_SIZE_WORDS 1

typedef struct CRYS_AESCCMPrivateContext {
	DxUint32_t isA0BlockProcessed;
} CRYS_AESCCMPrivateContext_t;


/************************ Private Functions **********************/

/*!
 * Converts Symmetric Adaptor return code to CRYS error code.
 * 
 * \param symRetCode Symmetric Adaptor return error.
 * \param errorInfo Ignored.
 *  
 * \return CRYSError_t one of CRYS_* error codes defined in crys_error.h
 */
static CRYSError_t SymAdaptor2CrysAesCcmErr(int symRetCode, DxUint32_t errorInfo)
{
	switch (symRetCode) {
	case DX_RET_UNSUPP_ALG:
	case DX_RET_UNSUPP_ALG_MODE:
		return CRYS_AESCCM_IS_NOT_SUPPORTED;
	case DX_RET_INVARG:
	case DX_RET_INVARG_QID:
		return CRYS_AESCCM_ILLEGAL_PARAMS_ERROR;
	case DX_RET_INVARG_KEY_SIZE:
		return CRYS_AESCCM_ILLEGAL_KEY_SIZE_ERROR;
	case DX_RET_INVARG_CTX_IDX:
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;
	case DX_RET_INVARG_CTX:
		return CRYS_AESCCM_USER_CONTEXT_CORRUPTED_ERROR;
	case DX_RET_INVARG_BAD_ADDR:
		return CRYS_AESCCM_ILLEGAL_PARAMETER_PTR_ERROR;
	case DX_RET_NOMEM:
		return CRYS_OUT_OF_RESOURCE_ERROR;
	case DX_RET_INVARG_INCONSIST_DMA_TYPE:
		return CRYS_AESCCM_ILLEGAL_DMA_BUFF_TYPE_ERROR;
	case DX_RET_UNSUPP_OPERATION:
	case DX_RET_PERM:
	case DX_RET_NOEXEC:
	case DX_RET_BUSY:
	case DX_RET_OSFAULT:
	default:
		return CRYS_FATAL_ERROR;
	}
}

/*!
 * Format AES-CCM Block A0 according to the given header length
 * 
 * \param pA0Buff A0 block buffer
 * \param headerSize The actual header size
 * 
 * \return uint32_t Number of bytes encoded
 */
static uint32_t FormatCcmA0(uint8_t *pA0Buff, uint32_t headerSize)
{
	uint32_t len = 0;

	if (headerSize < ((1UL << 16) - (1UL << 8))) {
		len = 2;

		pA0Buff[0] = (headerSize >> 8) & 0xFF;
		pA0Buff[1] = headerSize & 0xFF;
	} else {
		len = 6;

		pA0Buff[0] = 0xFF;
		pA0Buff[1] = 0xFE;
		pA0Buff[2] = (headerSize >> 24) & 0xFF;
		pA0Buff[3] = (headerSize >> 16) & 0xFF;
		pA0Buff[4] = (headerSize >> 8) & 0xFF;
		pA0Buff[5] = headerSize & 0xFF;
	}

	return len;
}

/** 
* @brief This function transfers the AESCCM_init function parameters from CRYS-SEP to
*        SEP-Driver and backwards for operating AESCCM_init.
*         
* @param[in] ContextID_ptr - A pointer to the AESCCM context buffer, that is allocated by the user 
*                            and is used for the AESCCM operations.
* @param[in] EncrDecrMode  - Enumerator variable defining operation mode (0 - encrypt; 1 - decrypt).
* @param[in] CCM_Key       - A buffer, containing the AESCCM key passed by user (predefined size 128 bits).
* @param[in] KeySizeID     - An enum parameter, defines size of used key (128, 192, 256).
** @param[in] AdataSize     - Full size of additional data in bytes, which must be processed.  
*                            Limitation in our implementation is: AdataSize < 2^32. If Adata is absent, 
*                            then AdataSize = 0.
* @param[in] TextSize      - The full size of text data (in bytes), which must be processed by CCM. 
*
* @param[in] N_ptr	       - A pointer to Nonce - unique value assigned to all data passed into CCM.
*                            Bytes order - big endian form (MSB is the first).
* @param[in] SizeOfN       - The size of the user passed Nonce (in bytes). 
* 			     It is an element of {7,8,9,10,11,12,13}.
* @param[in] SizeOfT	   - Size of AESCCM MAC output T in bytes. Valid values: [4,6,8,10,12,14,16].   
*
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure an error according to CRYS_AESCCM_error.h
*
*/
CRYSError_t CRYS_AESCCM_Init(
	CRYS_AESCCM_UserContext_t *ContextID_ptr,
	CRYS_AES_EncryptMode_t EncrDecrMode,
	CRYS_AESCCM_Key_t CCM_Key,
	CRYS_AESCCM_KeySize_t KeySizeId,
	DxUint32_t AdataSize,
	DxUint32_t TextSize,
	DxUint8_t *N_ptr,
	DxUint8_t SizeOfN,
	DxUint8_t SizeOfT)
{
	DxUint32_t keySizeInBytes;
	struct sep_ctx_aead *pAeadContext;
	CRYS_AESCCMPrivateContext_t *pAesCcmPrivContext;
	DxUint8_t QFieldSize = 15 - SizeOfN;
	int symRc = DX_RET_OK;

	/* if the users context ID pointer is DX_NULL return an error */
	if ( ContextID_ptr == DX_NULL ) {
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	/* check key pointer (unless secret key is used) */    
	if ( CCM_Key == DX_NULL ) {
		return CRYS_AESCCM_ILLEGAL_PARAMETER_PTR_ERROR;
	}

	/* check Nonce pointer */    
	if ( N_ptr == DX_NULL ) {
		return CRYS_AESCCM_ILLEGAL_PARAMETER_PTR_ERROR;     
	}

	/* check the Q field size: according to our implementation QFieldSize <= 4*/ 
	if ( (QFieldSize < 2) || (QFieldSize > 8) ) {
		return CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR;
	}

	/* Check that TextSize fits into Q field (i.e., there are enough bits) */
	if ( (BITMASK(QFieldSize * 8) & TextSize) != TextSize ) {
		return CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR;
	}

	/* check Nonce size. Note: QFieldSize + SizeOfN == 15 */ 
	if ( (SizeOfN < 7) || (SizeOfN != (15 - QFieldSize)) ) {
		return CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR; 
	}

	 /* check CCM MAC size: [4,6,8,10,12,14,16] */ 
	if ( (SizeOfT < 4) || (SizeOfT > 16) || ((SizeOfT & 1) != 0) ) {
		return CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR;
	}

	/* check Key size ID */    
	if ( KeySizeId > CRYS_AES_Key256BitSize ) {
		return CRYS_AESCCM_ILLEGAL_KEY_SIZE_ERROR; 
	}

	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_AESCCM_UserContext_t)) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ, CCM_Key, KeySizeId * SEP_AES_128_BIT_KEY_SIZE) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ, N_ptr, SizeOfN) ) {
		return CRYS_AESCCM_ILLEGAL_PARAMS_ERROR;
	}


	 switch (KeySizeId) {
	 case CRYS_AES_Key128BitSize: 
		 keySizeInBytes = 16;
		 break;
	 case CRYS_AES_Key192BitSize:
		 keySizeInBytes = 24;
		 break;
	 case CRYS_AES_Key256BitSize:
		 keySizeInBytes = 32;
		 break;
	 default:
		 return CRYS_AESCCM_ILLEGAL_KEY_SIZE_ERROR;
	 }

	/* Get pointer to contiguous context in the HOST buffer */ 
	 pAeadContext = (struct sep_ctx_aead *)DX_InitUserCtxLocation(ContextID_ptr->buff,
						   sizeof(CRYS_AESCCM_UserContext_t), 
						   sizeof(struct sep_ctx_aead));

	 pAesCcmPrivContext = (CRYS_AESCCMPrivateContext_t *)
		&(((uint32_t*)pAeadContext)[CRYS_AESCCM_USER_CTX_ACTUAL_SIZE_IN_WORDS - AESCCM_PRIVATE_CONTEXT_SIZE_WORDS]);
	 /* clear private context fields */
	 pAesCcmPrivContext->isA0BlockProcessed = 0;

	 /* init. CCM context */
	 pAeadContext->alg = SEP_CRYPTO_ALG_AEAD;
	 pAeadContext->mode = SEP_CIPHER_CCM;
	 pAeadContext->direction = (enum sep_crypto_direction)EncrDecrMode;
	 pAeadContext->key_size = keySizeInBytes;
	 DX_PAL_MemCopy(pAeadContext->key, CCM_Key, keySizeInBytes);
	 pAeadContext->header_size = AdataSize;
	 pAeadContext->nonce_size = SizeOfN;
	 DX_PAL_MemCopy(pAeadContext->nonce, N_ptr, SizeOfN);
	 pAeadContext->tag_size = SizeOfT;
	 pAeadContext->text_size = TextSize;

	 symRc = SymDriverAdaptorInit((struct sep_ctx_generic *)pAeadContext);
	 return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
}

/** 
* @brief This function transfers the CRYS_AESCCM_BlockAdata function parameters from CRYS-SEP to
*        SEP-Driver and backwards for operating CRYS_AESCCM_BlockAdata on SEP.
*
* @param[in] ContextID_ptr - A pointer to the AESCCM context buffer allocated by the user that
*                            is used for the AESCCM machine operation. This should be the same 
*                            context that was used on the previous call of this session.
* @param[in] DataIn_ptr - A pointer to the buffer of the input additional data.
*                         The pointer does not need to be aligned.
* @param[in] DataInSize   - A size of the additional data in bytes. 
* 
* @return CRYSError_t - On success CRYS_OK is returned, on failure an error according to
*                       CRYS_AESCCM_error.h
*   Restrictions:
*	1. The input data (DataIn_ptr) may reside in host memory processor and recognized as
*	   direct DMA object or reside in SEP SRAM or D-Cache and referenced by a simple direct address.
*       2. The input data may partially processed by multiple invocations. The associated data size
*          MUST not exceed 2048 bytes in each invocation.
*/
CRYSError_t CRYS_AESCCM_BlockAdata(
	CRYS_AESCCM_UserContext_t *ContextID_ptr,   
	DxUint8_t *DataIn_ptr,
	DxUint32_t DataInSize)
{
	struct sep_ctx_aead *pAeadContext;
	uint32_t headerA0BorrowLen, actualHeaderLen, headerA0MetaDataLen;
	DmaBuffer_s *pDinDmaBuff = NULL;
#ifdef  CRYS_USE_DMA_OBJECT
	DmaBuffer_s DinDmaBuff;
#endif
	CRYS_AESCCMPrivateContext_t *pAesCcmPrivContext;
	int symRc = DX_RET_OK;
	/* if the users context ID pointer is DX_NULL return an error */
	if ( ContextID_ptr == DX_NULL )  {
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	/* if the users Data In pointer is illegal return an error */
	if ( DataIn_ptr == DX_NULL ) {
		return CRYS_AESCCM_DATA_IN_POINTER_INVALID_ERROR;
	}

	/* if the data size is illegal return an error */
	if ( DataInSize == 0 ) {
		return CRYS_AESCCM_DATA_IN_SIZE_ILLEGAL;
	}


	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_AESCCM_UserContext_t))) {
		return CRYS_AESCCM_ILLEGAL_PARAMS_ERROR;
	}

#ifdef  CRYS_USE_DMA_OBJECT
	if (IS_SMART_PTR(DataIn_ptr)) {
		pDinDmaBuff = PTR_TO_DMA_BUFFER(DataIn_ptr);
	} else { /*"dumb" pointer*/
		DinDmaBuff.dmaBufType = DMA_BUF_SEP;
		DinDmaBuff.pData = (uint32_t)DataIn_ptr;
		DinDmaBuff.axiNs = AXI_NOT_SECURE;
		DinDmaBuff.nTables = 0;
		pDinDmaBuff = &DinDmaBuff;
	}

	/* support DMA type of SEP or DLLI only */
	if ((pDinDmaBuff->dmaBufType != DMA_BUF_SEP) &&
	    (pDinDmaBuff->dmaBufType != DMA_BUF_DLLI)) {
		return CRYS_AESCCM_ILLEGAL_DMA_BUFF_TYPE_ERROR;
	}
#else
	pDinDmaBuff = PTR_TO_DMA_BUFFER(DataIn_ptr);
#endif


	/* Get pointer to contiguous context in the HOST buffer */ 
	pAeadContext = (struct sep_ctx_aead *)DX_GetUserCtxLocation(ContextID_ptr->buff);

	pAesCcmPrivContext = (CRYS_AESCCMPrivateContext_t *)
	       &(((uint32_t*)pAeadContext)[CRYS_AESCCM_USER_CTX_ACTUAL_SIZE_IN_WORDS - AESCCM_PRIVATE_CONTEXT_SIZE_WORDS]);

	/* additional data may be processed only once */
	if (pAesCcmPrivContext->isA0BlockProcessed == 1) {
		return CRYS_AESCCM_ADATA_WAS_PROCESSED_ERROR;
	}

	if (pAesCcmPrivContext->isA0BlockProcessed == 0) {
		DxUint8_t pA0Block[CRYS_AES_BLOCK_SIZE_IN_BYTES] = {0};

		/* formate A0 block only once */
		headerA0MetaDataLen = FormatCcmA0(pA0Block, DataInSize);
		headerA0BorrowLen = min((CRYS_AES_BLOCK_SIZE_IN_BYTES - headerA0MetaDataLen), DataInSize);
		actualHeaderLen = headerA0MetaDataLen + DataInSize;

		/* this is the first Adata block.
		*  Complete to AES block thus A0 = [META DATA 2B/6B | ADATA 14B/12B] */
#ifdef  CRYS_USE_DMA_OBJECT
		pDinDmaBuff->size = headerA0BorrowLen;
#endif
		symRc = CRYS_Bypass(DMA_BUFFER_TO_PTR(pDinDmaBuff), headerA0BorrowLen, pA0Block + headerA0MetaDataLen);
		if (symRc != DX_RET_OK) {
			return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
		}

		if (actualHeaderLen <= CRYS_AES_BLOCK_SIZE_IN_BYTES) {
			/* given additional data plus header meta data are smaller than AES block size: A0+Adata < 16 */
			symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pAeadContext, pA0Block, NULL, actualHeaderLen);
			if (symRc != DX_RET_OK) {
				return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
			}
		} else {
			/* given additional data plus header meta data are greater than AES block size: A0+Adata > 16 */
			/* process A0 block */
			symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pAeadContext, pA0Block, NULL, CRYS_AES_BLOCK_SIZE_IN_BYTES);
			if (symRc != DX_RET_OK) {
				return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
			}

			/* prepare DMA buffer for rest of data */
#ifdef  CRYS_USE_DMA_OBJECT
			pDinDmaBuff->pData = (uint32_t)(pDinDmaBuff->pData + headerA0BorrowLen);
			pDinDmaBuff->size = DataInSize - headerA0BorrowLen;
#else
			DataIn_ptr += headerA0BorrowLen;
			pDinDmaBuff = PTR_TO_DMA_BUFFER(DataIn_ptr);
#endif
			/* process user remaining additional data */
			symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pAeadContext,
						DMA_BUFFER_TO_PTR(pDinDmaBuff), NULL, DataInSize - headerA0BorrowLen);
			if (symRc != DX_RET_OK) {
				return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
			}
		}

		pAesCcmPrivContext->isA0BlockProcessed = 1;
		goto EndBlockAdata;
	}

	symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pAeadContext, DataIn_ptr, NULL, DataInSize);

EndBlockAdata:
	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
}


/** 
* @brief This function transfers the CRYS_AESCCM_BlockTextData function parameters from CRYS-SEP to
*        SEP-Driver and backwarderrors for operating CRYS_AESCCM_BlockTextData on SEP.
*
* @param[in] ContextID_ptr - A pointer to the AESCCM context buffer allocated by the user that
*                            is used for the AES machine operation. This should be the same
*                            context that was used on the previous call of this session.
* @param[in] DataIn_ptr - A pointer to the buffer of the input data (plain or cipher text).
*                         The pointer does not need to be aligned.
* @param[in] DataInSize  - A size of the data block in bytes: must be multiple of 16 bytes and not 0.
*                          The block of data must be not a last block, that means:
*                            - on Encrypt mode: DataInSize < CCM_Context->RemainTextSize;
*                            - on Decrypt mode: DataInSize < CCM_Context->RemainTextSize - SizeOfT;
* @param[out] DataOut_ptr - A pointer to the output buffer (cipher or plain text).
*                          The pointer does not need to be aligned.
*                          Size of the output buffer must be not less, than DataInSize.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure a
*                       value MODULE_* CRYS_AESCCM_error.h
*   Notes:
*      1. Overlapping of the in-out buffers is not allowed, excluding the in placement case:
*         DataIn_ptr = DataOut_ptr.
*/
CRYSError_t CRYS_AESCCM_BlockTextData(
	CRYS_AESCCM_UserContext_t *ContextID_ptr,
	DxUint8_t *DataIn_ptr,
	DxUint32_t DataInSize,
	DxUint8_t *DataOut_ptr)
{
	struct sep_ctx_aead *pAeadContext = (struct sep_ctx_aead *)ContextID_ptr->buff;
	int symRc = DX_RET_OK;
	/* if the users context ID pointer is DX_NULL return an error */
	if ( ContextID_ptr == DX_NULL ) {
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	/* if the users Data In pointer is illegal return an error */
	if ( DataIn_ptr == DX_NULL ) {
		return CRYS_AESCCM_DATA_IN_POINTER_INVALID_ERROR;
	}

	/* if the Data In size is 0, return an error */
	if ( DataInSize == 0 ) {
		return CRYS_AESCCM_DATA_IN_SIZE_ILLEGAL;
	}

	/* if the users Data Out pointer is illegal return an error */
	if ( DataOut_ptr == DX_NULL ) {
		return CRYS_AESCCM_DATA_OUT_POINTER_INVALID_ERROR;
	}

	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_AESCCM_UserContext_t))) {
		return CRYS_AESCCM_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */
	pAeadContext = (struct sep_ctx_aead *)DX_GetUserCtxLocation(ContextID_ptr->buff);

	symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pAeadContext, DataIn_ptr, DataOut_ptr, DataInSize);
	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
}


/**
* @brief This function transfers the CRYS_AESCCM_BlockLastTextData function parameters from CRYS-SEP to
*        SEP-Driver and backwards for operating CRYS_AESCCM_BlockLastTextData on SEP.
*
* @param[in] ContextID_ptr - A pointer to the AESCCM context buffer, allocated by the user, 
*                          that is used for the AESCCM operations. This should be the same 
*                          context that was used on the previous call of this session.
* @param[in] DataIn_ptr  - A pointer to the buffer of the input data (plain or cipher text). 
*                          The pointer does not need to be aligned.
* @param[in] DataInSize  - A size of the data block in bytes. The size must be equal to remaining
*                          size value, saved in the context.
* @param[in] DataOut_ptr - A pointer to the output buffer (cipher or plain text). If 
*                          user passes DataInSize 0 bytes the DataOut_ptr may be equal to NULL. 
*                          The pointer does not need to be aligned.
* @param[in] MacRes -   A pointer to the Mac buffer. 
* @param[out] SizeOfT - size of MAC in bytes as defined in CRYS_AESCCM_Init function.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure a 
*                       value MODULE_* CRYS_AESCCM_error.h
*   Notes:   
*      1. Overlapping of the in-out buffers is not allowed, excluding the in placement case:
*         DataIn_ptr = DataOut_ptr.
*/
CEXPORT_C CRYSError_t CRYS_AESCCM_Finish(
	CRYS_AESCCM_UserContext_t *ContextID_ptr,   
	DxUint8_t *DataIn_ptr,
	DxUint32_t DataInSize,
	DxUint8_t *DataOut_ptr,
	CRYS_AESCCM_Mac_Res_t MacRes,
	DxUint8_t *SizeOfT)
{                              
	struct sep_ctx_aead *pAeadContext;
	int symRc = DX_RET_OK;
	/* if the users context ID pointer is DX_NULL return an error */
	if ( ContextID_ptr == DX_NULL )  {
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	/* if the users Data In pointer is illegal return an error */
	if ( (DataIn_ptr == DX_NULL) && (DataInSize != 0) ) {
		return CRYS_AESCCM_DATA_IN_POINTER_INVALID_ERROR;
	}

	/* if the users Data Out pointer is illegal return an error */
	if ( (DataOut_ptr == DX_NULL) && (DataInSize != 0) ) {
		return CRYS_AESCCM_DATA_OUT_POINTER_INVALID_ERROR;
	}

	/* if the users context ID pointer is DX_NULL return an error */
	if ( ContextID_ptr == DX_NULL ) {
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	if ( SizeOfT == DX_NULL )  {
		return CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR;
	}

	/* check validity for priv */
	if ( DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_AESCCM_UserContext_t)) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ, SizeOfT, sizeof(uint32_t)) ||
	     DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, MacRes, *SizeOfT) ) {
		return CRYS_AESCCM_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pAeadContext = (struct sep_ctx_aead *)DX_GetUserCtxLocation(ContextID_ptr->buff);

	symRc = SymDriverAdaptorFinalize((struct sep_ctx_generic *)pAeadContext, DataIn_ptr, DataOut_ptr, DataInSize);
	if (symRc != DX_RET_OK) {
		return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
	}

		/* copy MAC result to context */
	*SizeOfT = pAeadContext->tag_size;
#ifdef DX_CC_TEE
	if (pAeadContext->direction == SEP_CRYPTO_DIRECTION_DECRYPT) {
		if(DX_PAL_MemCmp(MacRes, pAeadContext->mac_state, *SizeOfT)) {
			return CRYS_AESCCM_CCM_MAC_INVALID_ERROR;
		}
	} else {
		DX_PAL_MemCopy(MacRes, pAeadContext->mac_state, *SizeOfT);
	}
#else
#warning Decrypt should be changed to check the MAC result
	DX_PAL_MemCopy(MacRes, pAeadContext->mac_state, *SizeOfT);
#endif
	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
}

/**
 * @brief This function is used to perform the AES_CCM operation in one integrated process. 
 *
 *        The function preforms CCM algorithm according to NIST 800-38C by call the CRYS_CCM
 *        Init, Block and Finish functions. 
 *
 *        The input-output parameters of the function are the following:
 *
 * @param[in] EncrDecrMode  - Enumerator variable defining operation mode (0 - encrypt; 1 - decrypt).
 * @param[in] CCM_Key       - A buffer, containing the AESCCM key passed by user (predefined size 128 bits).
 * @param[in] KeySizeId     - An ID of AESCCM key size (according to 128, 192, or 256 bits size).
 * @param[in] N_ptr	    - A pointer to Nonce - unique value assigned to all data passed into CCM.
 *                            Bytes order - big endian form (MSB is the first).
 * @param[in] SizeOfN       - The size of the user passed Nonce (in bytes). 
 * 			      It is an element of {7,8,9,10,11,12,13}.
 * @param[in] ADataIn_ptr    - A pointer to the additional data buffer. The pointer does 
 *                             not need to be aligned.
 * @param[in] ADataInSize    - The size of the additional data in bytes;  
 * @param[in] TextDataIn_ptr - A pointer to the input text data buffer (plain or cipher according to
 *                             encrypt-decrypt mode). The pointer does not need to be aligned.
 * @param[in] TextDataInSize - The size of the input text data in bytes:
 *                               - on encrypt mode: (2^32 - SizeOfT) > DataInSize >= 0;
 *                               - on Decrypt mode: 2^32 > DataInSize >= SizeOfT (SizeOfT from context).
 * @param[out] TextDataOut_ptr - The output text data pointer (cipher or plain text data). 
 *
 * @param[in] SizeOfT	    - Size of AES-CCM MAC output T in bytes. Valid values: [4,6,8,10,12,14,16].  
* 
 * @param[in/out] Mac_Res	    -  AES-CCM MAC input/output .  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a value defined in CRYS_AESCCM_error.h
 *
 */
CIMPORT_C CRYSError_t CRYS_AESCCM( 
	CRYS_AES_EncryptMode_t EncrDecrMode,
	CRYS_AESCCM_Key_t CCM_Key,
	CRYS_AESCCM_KeySize_t KeySizeId,
	DxUint8_t *N_ptr,
	DxUint8_t SizeOfN,
	DxUint8_t *ADataIn_ptr,
	DxUint32_t ADataInSize,
	DxUint8_t *TextDataIn_ptr,
	DxUint32_t TextDataInSize,
	DxUint8_t *TextDataOut_ptr,
	DxUint8_t SizeOfT,
	CRYS_AESCCM_Mac_Res_t MacRes)
{
	CRYSError_t crysRc = CRYS_OK;
	CRYS_AESCCM_UserContext_t ContextID;

	crysRc = CRYS_AESCCM_Init(&ContextID, EncrDecrMode, CCM_Key,
				KeySizeId, ADataInSize, TextDataInSize,
				N_ptr, SizeOfN, SizeOfT);
	if (crysRc != CRYS_OK) {
		return crysRc;
	}

	if (ADataInSize > 0) {
		crysRc = CRYS_AESCCM_BlockAdata(&ContextID, ADataIn_ptr, ADataInSize);
		if (crysRc != CRYS_OK) {
			return crysRc;
		}
	}

	return CRYS_AESCCM_Finish(&ContextID, TextDataIn_ptr, TextDataInSize, TextDataOut_ptr, MacRes, &SizeOfT);
}

