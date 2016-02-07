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
#include "crys_hash.h"
#include "crys_hash_error.h"
#include "hash.h"
#include "sym_adaptor_driver.h"
#include "dma_buffer.h"
#include "cc_acl.h"
#include "dx_error.h"
#include "crys_context_relocation.h"
#include "debug.h"//2012/11/13


/************************ Defines ******************************/

#if ( SEP_CTX_SIZE_WORDS > CRYS_HASH_USER_CTX_SIZE_IN_WORDS )
#error CRYS_HASH_USER_CTX_SIZE_IN_WORDS is not defined correctly.
#endif
/* Since the user context in the TEE is doubled to allow it to be contiguous we must get */
/*  the real size of the context (SEP context) to get the private context pointer  */
#ifdef DX_CC_TEE
#define CRYS_HASH_USER_CTX_ACTUAL_SIZE_IN_WORDS	((CRYS_HASH_USER_CTX_SIZE_IN_WORDS - 3)/2)
#else
#define CRYS_HASH_USER_CTX_ACTUAL_SIZE_IN_WORDS	CRYS_HASH_USER_CTX_SIZE_IN_WORDS
#endif

/************************ Type definitions **********************/
typedef struct CRYS_HASHPrivateContext_t {
	uint32_t isLastBlockProcessed;
} CRYS_HASHPrivateContext_t;

/************************ Public Functions ******************************/

/*!
 * Converts Symmetric Adaptor return code to CRYS error code.
 * 
 * \param symRetCode Symmetric Adaptor return error.
 * \param errorInfo Ignored.
 *  
 * \return CRYSError_t one of CRYS_* error codes defined in crys_error.h
 */
static CRYSError_t SymAdaptor2CrysHashErr(int symRetCode, DxUint32_t errorInfo)
{
	switch (symRetCode) {
	case DX_RET_UNSUPP_ALG:
		return CRYS_HASH_IS_NOT_SUPPORTED;
	case DX_RET_UNSUPP_ALG_MODE:
	case DX_RET_UNSUPP_OPERATION:
		return CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR;
	case DX_RET_INVARG:
	case DX_RET_INVARG_QID:
		return CRYS_HASH_ILLEGAL_PARAMS_ERROR;
	case DX_RET_INVARG_KEY_SIZE:
	case DX_RET_INVARG_CTX_IDX:
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;
	case DX_RET_INVARG_CTX:
		return CRYS_HASH_USER_CONTEXT_CORRUPTED_ERROR;
	case DX_RET_INVARG_BAD_ADDR:
		return CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR;
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

/**
 * This function initializes the HASH machine on the CRYS level.
 *
 * This function allocates and initializes the HASH Context .
 * The function receives as input a pointer to store the context handle to HASH Context , 
 * it initializes the 
 * HASH Context with the cryptographic attributes that are needed for 
 * the HASH block operation ( initialize H's value for the HASH algorithm ).
 *
 * The function flow:
 *
 * 1) checking the validity of the arguments - returnes an error on an illegal argument case.
 * 2) Aquiring the working context from the CCM manager.
 * 3) Initializing the context with the parameters passed by the user and with the init values
 *    of the HASH.
 * 4) loading the user tag to the context.
 * 5) release the CCM context.
 * 
 * @param[in] ContextID_ptr - a pointer to the HASH context buffer allocated by the user that
 *                       is used for the HASH machine operation.
 *
 * @param[in] OperationMode - The operation mode : MD5 or SHA1.
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *      
 */
CEXPORT_C CRYSError_t CRYS_HASH_Init(CRYS_HASHUserContext_t*    ContextID_ptr,
                                     CRYS_HASH_OperationMode_t  OperationMode)
{
	struct sep_ctx_hash *pHashContext;
	CRYS_HASHPrivateContext_t *pHashPrivContext;
	int symRc = DX_RET_OK;
	if ( ContextID_ptr == DX_NULL ) {
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	if( OperationMode >= CRYS_HASH_NumOfModes ) {
		//PRINT_INFO("HASH MODE ERROR\n");
		return CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR;
	}

	/*pointer for CTX  allocation*/ 
	/* FUNCTION LOGIC */
	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_HASHUserContext_t))) {
		return CRYS_HASH_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pHashContext = (struct sep_ctx_hash *)DX_InitUserCtxLocation(ContextID_ptr->buff,
						  sizeof(CRYS_HASHUserContext_t), 
						  sizeof(struct sep_ctx_hash));
	pHashPrivContext = (CRYS_HASHPrivateContext_t *)&(((uint32_t*)pHashContext)[CRYS_HASH_USER_CTX_ACTUAL_SIZE_IN_WORDS-1]);

	pHashContext->alg = SEP_CRYPTO_ALG_HASH;
	pHashPrivContext->isLastBlockProcessed = 0;

	switch (OperationMode) {
		case CRYS_HASH_SHA1_mode:
			pHashContext->mode = SEP_HASH_SHA1;
			break;
		case CRYS_HASH_SHA224_mode:
			pHashContext->mode = SEP_HASH_SHA224;
			break;
		case CRYS_HASH_SHA256_mode:
			pHashContext->mode = SEP_HASH_SHA256;
			break;
		case CRYS_HASH_SHA384_mode:
			pHashContext->mode = SEP_HASH_SHA384;
			break;
		case CRYS_HASH_SHA512_mode:
			pHashContext->mode = SEP_HASH_SHA512;
			break;
		case CRYS_HASH_MD5_mode:
		default:
			return CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR;
	}
       //PRINT_INFO("--->NOW into CRYS_HASH_Init,begin SymDriverAdaptorInit\n");
	symRc = SymDriverAdaptorInit((struct sep_ctx_generic *)pHashContext);
	//PRINT_INFO("---> after SymDriverAdaptorInit,symRc=0x%x\n",symRc);
	return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysHashErr);
}

/**
 * This function process a block of data via the HASH Hardware.
 * The function receives as input an handle to the  HASH Context , that was initialized before
 * by an CRYS_HASH_Init function or by other CRYS_HASH_Update 
 * function. The function Sets the hardware with the last H's 
 * value that where stored in the CRYS HASH context and then 
 * process the data block using the hardware and in the end of 
 * the process stores in the HASH context the H's value HASH 
 * Context with the cryptographic attributes that are needed for 
 * the HASH block operation ( initialize H's value for the HASH 
 * algorithm ). This function is used in cases not all the data 
 * is arrange in one continues buffer.
 *
 * The function flow:
 *
 * 1) checking the parameters validty if there is an error the function shall exit with an error code. 
 * 2) Aquiring the working context from the CCM manager.
 * 3) If there isnt enouth data in the previous update data buff in the context plus the received data
 *    load it to the context buffer and exit the function.
 * 4) fill the previous update data buffer to contain an entire block. 
 * 5) Calling the hardware low level function to execute the update.
 * 6) fill the previous update data buffer with the data not processed at the end of the received data.
 * 7) release the CCM context.
 * 
 * @param[in] ContextID_ptr - a pointer to the HASH context buffer allocated by the user that
 *                       is used for the HASH machine operation.
 *
 * @param DataIn_ptr a pointer to the buffer that stores the data to be 
 *                       hashed .
 * 
 * @param DataInSize  The size of the data to be hashed in bytes. 
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *
 */
CEXPORT_C CRYSError_t CRYS_HASH_Update(CRYS_HASHUserContext_t*    ContextID_ptr,
                                       DxUint8_t*                 DataIn_ptr,
                                       DxUint32_t                 DataInSize )
{                             
	struct sep_ctx_hash *pHashContext;
	CRYS_HASHPrivateContext_t *pHashPrivContext;
	int symRc = DX_RET_OK;
	int hash_block_size_in_bytes = 0;

	if ( ContextID_ptr == DX_NULL ) {
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

	if( DataInSize == 0 ) {
		return CRYS_OK;
	}
	
	if( DataIn_ptr == DX_NULL ) {
		return CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR;
	}
      // PRINT_INFO("--->NOW enter into CRYS_HASH_Update\n");
	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_HASHUserContext_t))) {
		return CRYS_HASH_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pHashContext = (struct sep_ctx_hash *)DX_GetUserCtxLocation(ContextID_ptr->buff);

	pHashPrivContext = (CRYS_HASHPrivateContext_t *)&(((uint32_t*)pHashContext)[CRYS_HASH_USER_CTX_ACTUAL_SIZE_IN_WORDS-1]);

	if (pHashPrivContext->isLastBlockProcessed != 0) {
		return CRYS_HASH_LAST_BLOCK_ALREADY_PROCESSED_ERROR;
	}

	if (pHashContext->mode < SEP_HASH_SHA512)
		hash_block_size_in_bytes = CRYS_HASH_BLOCK_SIZE_IN_BYTES;
	else
		hash_block_size_in_bytes = CRYS_HASH_SHA512_BLOCK_SIZE_IN_BYTES;

	if ((DataInSize % hash_block_size_in_bytes) == 0) {
		symRc = SymDriverAdaptorProcess((struct sep_ctx_generic *)pHashContext,
					DataIn_ptr, NULL, DataInSize);

		if (symRc != DX_RET_OK) {
			return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysHashErr);
		}
	} else { /* this is the last block */
		pHashPrivContext->isLastBlockProcessed = 1;
		symRc = SymDriverAdaptorFinalize((struct sep_ctx_generic *)pHashContext,
						DataIn_ptr, NULL, DataInSize);

		if (symRc != DX_RET_OK) {
			return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysHashErr);
		}
	}
       //PRINT_INFO("--->NOW leave CRYS_HASH_Update\n");
	return CRYS_OK;
}

/**
 * This function finalize the hashing process of data block.
 * The function receives as input an handle to the HASH Context , that was initialized before
 * by an CRYS_HASH_Init function or by CRYS_HASH_Update function.
 * The function "adds" an header to the data block as the specific hash standard 
 * specifics , then it loads the hardware and reads the final message digest.
 *
 *  the function flow:
 *
 * 1) checking the parameters validty if there is an error the function shall exit with an error code. 
 * 2) Calling the hardware low level function to execute the 
 *    finish. 
 *
 *  @param[in] ContextID_ptr - a pointer to the HASH context buffer allocated by the user that
 *                       is used for the HASH machine operation.
 *
 *  @retval HashResultBuff a pointer to the target buffer where the 
 *                       HASE result stored in the context is loaded to.
 *
 *  @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 */

CEXPORT_C CRYSError_t CRYS_HASH_Finish( CRYS_HASHUserContext_t*   ContextID_ptr,
                                        CRYS_HASH_Result_t        HashResultBuff )
{  
	struct sep_ctx_hash *pHashContext;
	CRYS_HASHPrivateContext_t *pHashPrivContext;
	int symRc = DX_RET_OK;
       //PRINT_INFO("--->NOW enter into CRYS_HASH_Finish\n");
	if ( ContextID_ptr == DX_NULL ) {
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;
	}
   
	if ( HashResultBuff == DX_NULL ) {
		return CRYS_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR;
	}

	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_HASHUserContext_t))) {
		return CRYS_HASH_ILLEGAL_PARAMS_ERROR;
	}

	/* Get pointer to contiguous context in the HOST buffer */ 
	pHashContext = (struct sep_ctx_hash *)DX_GetUserCtxLocation(ContextID_ptr->buff);

	pHashPrivContext = (CRYS_HASHPrivateContext_t *)&(((uint32_t*)pHashContext)[CRYS_HASH_USER_CTX_ACTUAL_SIZE_IN_WORDS-1]);

	/* check access permission for applet */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, HashResultBuff, sizeof(struct sep_ctx_hash))) {
		return CRYS_HASH_ILLEGAL_PARAMS_ERROR;
	}

	if (pHashPrivContext->isLastBlockProcessed == 0) {
		symRc = SymDriverAdaptorFinalize((struct sep_ctx_generic *)pHashContext, NULL, NULL, 0);

		if (symRc != DX_RET_OK) {
			return DX_CRYS_RETURN_ERROR(symRc, 0, SymAdaptor2CrysHashErr);
		}
	}

	/* Copy the result to the user buffer */
	DX_PAL_MemCopy(HashResultBuff, pHashContext->digest, CRYS_HASH_RESULT_SIZE_IN_WORDS*sizeof(DxUint32_t));

	return CRYS_OK;
}

/**
 * @brief This function clears the hash context
 *
 * @param[in] ContextID_ptr - a pointer to the HASH context
 *  					 buffer allocated by the user that is
 *  					 used for the HASH machine operation.
 *  					 This should be the same context that
 *  					 was used on the previous call of this
 *  					 session.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_hash_error.h
 */
CEXPORT_C CRYSError_t  CRYS_HASH_Free( CRYS_HASHUserContext_t *ContextID_ptr )
{
       // PRINT_INFO("--->NOW into CRYS_HASH_Free\n");
	if ( ContextID_ptr == DX_NULL ) {
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;
	}

		/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ContextID_ptr, sizeof(CRYS_HASHUserContext_t))) {
		return CRYS_HASH_ILLEGAL_PARAMS_ERROR;
	}

	DX_PAL_MemSetZero(ContextID_ptr, sizeof(CRYS_HASHUserContext_t));
   //PRINT_INFO("--->NOW leave CRYS_HASH_Free\n");
	return CRYS_OK;   
}
                     
/**
 * This function provide HASH function to process one buffer of data.
 * The function allocates an internal HASH Context , it initializes the 
 * HASH Context with the cryptographic attributes that are needed for 
 * the HASH block operation ( initialize H's value for the HASH algorithm ).
 * Then the function loads the Hardware with the initializing values and after 
 * that process the data block using the hardware to do hash .
 * At the end the function return the message digest of the data buffer .
 *
 * @param[in] OperationMode - The operation mode : MD5 or SHA1.
 * 
 * @param DataIn_ptr a pointer to the buffer that stores the data to be 
 *                       hashed .
 * 
 * @param DataInSize  The size of the data to be hashed in bytes. 
 *
 * @retval HashResultBuff a pointer to the target buffer where the 
 *                      HASE result stored in the context is loaded to.
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *
 */
CEXPORT_C CRYSError_t CRYS_HASH  ( CRYS_HASH_OperationMode_t    OperationMode,
                                   DxUint8_t*                   DataIn_ptr,
                                   DxUint32_t                   DataSize,
                                   CRYS_HASH_Result_t           HashResultBuff )
{    
	CRYSError_t Error = CRYS_OK;
	CRYS_HASHUserContext_t UserContext;
	
	Error = CRYS_HASH_Init( &UserContext, OperationMode);
	if (Error != CRYS_OK) {
		goto end;
	}

	Error = CRYS_HASH_Update( &UserContext, DataIn_ptr, DataSize );
	if (Error != CRYS_OK) {
		goto end;
	}

	Error = CRYS_HASH_Finish( &UserContext, HashResultBuff );
	
end:
	CRYS_HASH_Free( &UserContext );

	return Error;
}
