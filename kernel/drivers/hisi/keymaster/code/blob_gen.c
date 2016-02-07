
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

#include "crys_aes.h"
#include "sym_adaptor_driver.h"
#include "dx_macros.h"
#include "dx_cclib.h"

#include "dx_pal_mem.h"

#include "crys_aesccm.h"
#include "crys_aesccm_error.h"
#include "cc_acl.h"
#include "dx_error.h"

#include "blob_gen.h"

/************************ Defines ******************************/

#ifdef __BIG_ENDIAN__
#define SWAP_BYTE_ORDER(val)    (((val)&0xFF)<<24|((val)&0xFF00)<<8|((val)&0xFF0000)>>8|((val)&0xFF000000)>>24)
#else
#define SWAP_BYTE_ORDER(val)	val
#endif

/************************ Type definitions **********************/

#define AESCCM_PRIVATE_CONTEXT_SIZE_WORDS 1

typedef struct CRYS_AESCCMPrivateContext {
	DxUint32_t isA0BlockProcessed;
} CRYS_AESCCMPrivateContext_t;


/************************ Private Functions **********************/

static CRYSError_t CRYS_AESCCM_BlobInit(
	CRYS_AESCCM_UserContext_t *ContextID_ptr,
	CRYS_AES_EncryptMode_t EncrDecrMode,
	CRYS_AESCCM_Key_t CCM_Key,
	CRYS_AESCCM_KeySize_t KeySizeId,
	DxUint32_t AdataSize,
	DxUint32_t TextSize,
	DxUint8_t *N_ptr,
	DxUint8_t SizeOfN,
	DxUint8_t SizeOfT);

static CRYSError_t SymAdaptor2CrysAesCcmErr(int symRetCode, DxUint32_t errorInfo);


/*! 
 * @brief Create a blob in the CC44 secure environment with the session key. 
 * The blob is built of the AES encrypted key and the restriction data with 
 * authentication (by AES-CCM).
 * 
 * @param[in] encryptFlag	- An enum parameter, defines Encrypt operation (0) or a Decrypt operation (1).
 * @param[in] aesMode		- An enum parameter, defines AES Operation mode.
 * @param[in] baseAddr		- The blob base address.
 * @param[in] tailAddr		- The blob tail address.
 * @param[in] nonce		- A pointer to Nonce - unique value assigned to all data passed into CCM.
 * @param[in] userKeyPlain	- A pointer to the input text data buffer. The pointer does not need to be aligned.
 * @param[in] userKeySize	- An enum parameter, defines CRYS_AES_Key128BitSize or CRYS_AES_Key256BitSize.
 * @param[out] blobBuf		- A pointer to the generated blob message. 
 * @param[out] pBlobSize	- The size of the generated blob message.

 * \return DX_CclibRetCode_t one of the error codes defined in dx_cclib.h
 */
DX_CclibRetCode_t DX_CreateBlob(CRYS_AES_EncryptMode_t encryptFlag, enum sep_special_aes_mode aesMode,
				DxUint32_t baseAddr, DxUint32_t tailAddr, CRYS_BLOB_nonce_t nonce, 
				DxUint8_t *userKeyPlain, CRYS_AES_KeySize_t userKeySize,
				CRYS_BLOB_Message_t blobBuf, DxUint32_t *pBlobSize)
{

	DxError_t rc = DX_CCLIB_RET_OK;
	DxUint8_t AdataPtr[DX_ADATA_SIZE_IN_BYTES];
	DxUint8_t userKeyEncrypt[DX_BLOB_MAX_KEY_SIZE_IN_BYTES];
	CRYS_AESCCM_UserContext_t ccmCtxP;
	DxUint8_t macSize;
	CRYS_AESCCM_Mac_Res_t macRes;
	DxUint32_t blobMode;
	DxUint32_t token = DX_BLOB_TOKEN_VALUE;
	DxUint8_t nonceSize = DX_SIZE_OF_NONCE;
	DxUint32_t keySizeInBytes;

	/* check input variables */
	if ((encryptFlag != CRYS_AES_Decrypt) && 
	    (encryptFlag != CRYS_AES_Encrypt)) {
		return DX_CCLIB_RET_EINVAL;
	}
	if ((aesMode != SEP_SPECIAL_AES_ECB) && 
	    (aesMode != SEP_SPECIAL_AES_CBC) && 
	    (aesMode != SEP_SPECIAL_AES_CTR) && 
	    (aesMode != SEP_SPECIAL_AES_OFB)) {
		return DX_CCLIB_RET_EINVAL;
	}
	if (nonce == DX_NULL) {
		return DX_CCLIB_RET_EINVAL;
	}
	if (userKeyPlain == DX_NULL) {
		return DX_CCLIB_RET_EINVAL;
	}
	if (blobBuf == DX_NULL) {
		return DX_CCLIB_RET_EINVAL;
	}
	if (pBlobSize == DX_NULL) {
		return DX_CCLIB_RET_EINVAL;
	}
	switch (userKeySize) {
		case CRYS_AES_Key128BitSize:
			keySizeInBytes = DX_BLOB_MIN_KEY_SIZE_IN_BYTES;
			*pBlobSize = DX_MIN_BLOB_SIZE_IN_BYTES;
			break;
		case CRYS_AES_Key256BitSize:
			keySizeInBytes = DX_BLOB_MAX_KEY_SIZE_IN_BYTES;
			*pBlobSize = DX_MAX_BLOB_SIZE_IN_BYTES;
			break;
		default:
			rc = DX_CCLIB_RET_EINVAL;
			goto End;
	}

	/* build Additional data */	
	AdataPtr[DX_ADATA_DIR_OFFSET] = encryptFlag;
	AdataPtr[DX_ADATA_MODE_OFFSET] = aesMode;
	baseAddr = SWAP_BYTE_ORDER(baseAddr);
	memcpy(&AdataPtr[DX_ADATA_BASE_OFFSET],&baseAddr,sizeof(DxUint32_t));
	tailAddr = SWAP_BYTE_ORDER(tailAddr);
	memcpy(&AdataPtr[DX_ADATA_TAIL_OFFSET],&tailAddr,sizeof(DxUint32_t));
	token = SWAP_BYTE_ORDER(token);
	memcpy(&AdataPtr[DX_ADATA_TOKEN_OFFSET],&token,sizeof(DxUint32_t));

	/* initialize CCM with prive function to work with session key */
	rc = CRYS_AESCCM_BlobInit(&ccmCtxP, CRYS_AES_Encrypt,
		/* hw session key is used to generate a blob */
		NULL, CRYS_AES_Key128BitSize, DX_ADATA_SIZE_IN_BYTES,
		keySizeInBytes, nonce, nonceSize, sizeof(macRes));

	if (rc != CRYS_OK) {
		return rc;
	}

	/* process CCM crypto operation on additinal data */
	rc = CRYS_AESCCM_BlockAdata(&ccmCtxP, 
		AdataPtr, DX_ADATA_SIZE_IN_BYTES);
	if (rc != CRYS_OK) {
		return rc;
	}

	/* finish CCM crypto operation and get mac result and encrypted text */
	rc = CRYS_AESCCM_Finish(&ccmCtxP, 
		userKeyPlain, keySizeInBytes, userKeyEncrypt, 
		macRes, &macSize);
	if (rc != CRYS_OK) {
		return rc;
	}

	/* build the blob message */
	blobMode = (aesMode << DX_BLOB_MODE_SHIFT) | encryptFlag;
	memcpy(&blobBuf[DX_BLOB_TOKEN_OFFSET],&token,sizeof(DxUint32_t));
	memcpy(&blobBuf[DX_BLOB_NONCE_OFFSET],nonce,nonceSize);
	memcpy(&blobBuf[DX_BLOB_MODE_OFFSET],&blobMode,sizeof(DxUint32_t));
	memcpy(&blobBuf[DX_BLOB_BASE_ADDR_OFFSET],&baseAddr,sizeof(DxUint32_t));
	memcpy(&blobBuf[DX_BLOB_TAIL_ADDR_OFFSET],&tailAddr,sizeof(DxUint32_t));
	memcpy(&blobBuf[DX_BLOB_KEY_OFFSET],userKeyEncrypt,keySizeInBytes);
	memcpy(&blobBuf[DX_BLOB_KEY_OFFSET+keySizeInBytes],macRes,sizeof(macRes));


End:
	return rc;

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
* @param[in] AdataSize     - Full size of additional data in bytes, which must be processed.  
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

static CRYSError_t CRYS_AESCCM_BlobInit(
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
	struct sep_ctx_aead *pAeadContext = (struct sep_ctx_aead *)ContextID_ptr->buff;
	CRYS_AESCCMPrivateContext_t *pAesCcmPrivContext = (CRYS_AESCCMPrivateContext_t *)
		&(ContextID_ptr->buff[CRYS_AESCCM_USER_CTX_SIZE_IN_WORDS - AESCCM_PRIVATE_CONTEXT_SIZE_WORDS]);
	DxUint8_t QFieldSize = 15 - SizeOfN;
	int symRc = DX_RET_OK;

	/* if the users context ID pointer is DX_NULL return an error */
	if ( ContextID_ptr == DX_NULL ) {
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;
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

	 /* clear private context fields */
	 pAesCcmPrivContext->isA0BlockProcessed = 0;

	 /* init. CCM context */
	 pAeadContext->alg = SEP_CRYPTO_ALG_AEAD;
	 pAeadContext->mode = SEP_CIPHER_CCM;
	 pAeadContext->direction = (enum sep_crypto_direction)EncrDecrMode;
	 pAeadContext->crypto_key_type = DX_SESSION_KEY;
	 pAeadContext->key_size = keySizeInBytes;
	 pAeadContext->header_size = AdataSize;
	 pAeadContext->nonce_size = SizeOfN;
	 DX_PAL_MemCopy(pAeadContext->nonce, N_ptr, SizeOfN);
	 pAeadContext->tag_size = SizeOfT;
	 pAeadContext->text_size = TextSize;

	 symRc = SymDriverAdaptorInit((struct sep_ctx_generic *)pAeadContext);
	 return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysAesCcmErr);
}

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



