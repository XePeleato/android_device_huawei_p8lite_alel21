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

#include "dx_vos_mem.h"
#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif
#include "crys_common.h"
#include "crys_common_math.h"
#include "crys_rsa_error.h"
#include "crys_rsa_local.h"
#include "llf_pki_rsa.h"
#include "plat_print_dbg.h"
#ifndef DX_OEM_FW
#include "cc_acl.h"
#endif
/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */
/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Public Functions ******************************/

#if !defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNALCRYS_NO_RSA_VERIFY_SUPPORT)

/**********************************************************************************/ 
/**
@brief
CRYS_RSA_PRIM_Encrypt implements the RSAEP algorithm as defined in PKCS#1 v2.1 6.1.1

  @param[in] UserPubKey_ptr - Pointer to the public key data structure.
  @param[in] PrimeData_ptr - A pointer to a structure containing temp buffers.
  @param[in] Data_ptr - Pointer to the input data to be encrypted.
  @param[in] DataSize - The size, in bytes, of the input data. 
			\Note: DataSize <= modulus size is supported, but recommended
			that size is equal to modulus size. If smaller, the data will be 
			zero-padded on left side up to the modulus size and therefore, 
			after further decrypt operation its result will contain 
			padding zeros also.
  @param[out] Output_ptr - Pointer to the encrypted data. The size of output data 
			is always equal to size modulus size. The output buffer
                        must be at least of modulus size in bytes.

  @return CRYSError_t - CRYS_OK,
                        CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
                        CRYS_RSA_PUB_KEY_VALIDATION_TAG_ERROR,
                        CRYS_RSA_PRIM_DATA_STRUCT_POINTER_INVALID,
                        CRYS_RSA_DATA_POINTER_INVALID_ERROR,
                        CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR,
                        CRYS_RSA_INVALID_MESSAGE_BUFFER_SIZE,
                        CRYS_RSA_INVALID_MESSAGE_DATA_SIZE,
                        CRYS_RSA_INVALID_MESSAGE_VAL
 */
CEXPORT_C CRYSError_t CRYS_RSA_PRIM_Encrypt(CRYS_RSAUserPubKey_t *UserPubKey_ptr,
                                  CRYS_RSAPrimeData_t    *PrimeData_ptr,
                                  DxUint8_t              *Data_ptr,
                                  DxUint16_t              DataSize,
                                  DxUint8_t              *Output_ptr)
{
   /* FUNCTION LOCAL DECLERATIONS */
 
    /* the effective size in bits of the data message buffer */
   DxUint32_t DataEffectiveSizeInBits;

   /* the counter compare result */
   CRYS_COMMON_CmpCounter_t CounterCmpResult; 

   /* the public key database pointer */
   CRYSRSAPubKey_t *PubKey_ptr;
   
   /* the modulus size in bytes */
   DxUint32_t nSizeInBytes;

   /* The return error identifier */
   CRYSError_t Error;
  
   /* FUNCTION LOGIC */

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserPubKey_ptr , PrimeData_ptr , Data_ptr , 
                              DataSize , Output_ptr , DataEffectiveSizeInBits , CounterCmpResult , 
                              PubKey_ptr , nSizeInBytes , 
                              Error , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
 
   /* ............... initialize local variables ......................... */
   /* -------------------------------------------------------------------- */
   
   /* initialize the error identifier to O.K - success */
   Error = CRYS_OK;
   
   /* ............... checking the parameters pointers validity .......... */
   /* -------------------------------------------------------------------- */
   
   /* checking the key database handle pointer */
   if(UserPubKey_ptr == DX_NULL)
      return CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;
      
   /* if the users TAG is illegal return an error - the context is invalid */
   if(UserPubKey_ptr->valid_tag != CRYS_RSA_PUB_KEY_VALIDATION_TAG)
      return CRYS_RSA_PUB_KEY_VALIDATION_TAG_ERROR;      

   /* checking the Prime Data pointer */
   if(PrimeData_ptr == DX_NULL)
      return CRYS_RSA_PRIM_DATA_STRUCT_POINTER_INVALID;

   /* if the users Data pointer is illegal return an error */
   if(Data_ptr == DX_NULL)
      return CRYS_RSA_DATA_POINTER_INVALID_ERROR;

   /* if the users output pointer is illegal return an error */
   if(Output_ptr == DX_NULL)
      return CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR;
      
   #ifndef DX_OEM_FW
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPubKey_ptr, sizeof(CRYS_RSAUserPubKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, PrimeData_ptr, sizeof(CRYS_RSAPrimeData_t)) ||
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, Data_ptr, DataSize) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, Output_ptr, DataSize)){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
   #endif

#ifndef CRYS_NO_FIPS_SUPPORT      
   if(DX_GLOBAL_FIPS_MODE & DX_CRYS_FIPS_MODE_STATE)
   {
   	/* check if there was a FIPS error */
	if(DX_GLOBAL_FIPS_MODE & DX_CRYS_FIPS_MODE_ERROR_STATE)
   		return CRYS_RSA_FIPS_MODE_FAIL_ERROR;

	/* no check if the self test was already executed */
   	if(!(DX_GLOBAL_FIPS_SF_STATUS & CRYS_SELF_TEST_RSA_ENCDEC_BIT))
   		return CRYS_RSA_FIPS_MODE_SF_NOT_PERFORMED_ERROR;
   }
#endif

   /* if the data size is larger then the internal buffer return error */
   if(DataSize > sizeof(PrimeData_ptr->DataIn))
      return CRYS_RSA_INVALID_MESSAGE_BUFFER_SIZE;

   /* setting the pointer to the key database */
   PubKey_ptr = (CRYSRSAPubKey_t*)UserPubKey_ptr->PublicKeyDbBuff;   
   
   /* setting the modulus size in bytes */
   nSizeInBytes = (PubKey_ptr->nSizeInBits+7)/8;
   

   /* ................ copying the input data to the buffer .............. */
   /* -------------------------------------------------------------------- */   
  
   /* clear the input data */
   DX_VOS_MemSet(PrimeData_ptr->DataIn, 0, sizeof(PrimeData_ptr->DataIn));
               
   /* copy the input data to the aligned buffer on the data handler */
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PrimeData_ptr->DataIn, 4*((nSizeInBytes+3)/4),
						Data_ptr, DataSize);
   if(Error != CRYS_OK)
            return Error;
               
   /* ........... checking the data size and the message value ...................... */
   /* ------------------------------------------------------------------------------- */
   
   /* initializing the effective counters size in bits */
   DataEffectiveSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrimeData_ptr->DataIn, (DataSize+3)/4 ); 
  
   /* if the size is 0 - return an error */
   if( DataEffectiveSizeInBits == 0 ){
      Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE;
      goto End;
   } 
   
   /* compare the size of the message with the modulus it must be less then the modulous */
   CounterCmpResult = CRYS_COMMON_CmpLsWordsUnsignedCounters( 
				   PrimeData_ptr->DataIn, (DxUint16_t)(nSizeInBytes+3)/4, 
				   PubKey_ptr->n, (DxUint16_t)(nSizeInBytes+3)/4);
     
   if(CounterCmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1){
      Error = CRYS_RSA_INVALID_MESSAGE_VAL;
      goto End;
   }   
        
   /* ..................... executing the encryption ...................... */
   /* --------------------------------------------------------------------- */           
   
   PLAT_LOG_DEV_PRINT_DisplayBuffer(( 22 , (char *)" \n IN CRYS_RSA_PRIM_Encrypt: Before Exp operation PrimeData_ptr->DataIn = " ,
                                           (DxUint8_t *)PrimeData_ptr->DataIn ,
                                           sizeof(PrimeData_ptr->DataIn)));   
   
   /* executing the encryption */   
   Error = LLF_PKI_RSA_ExecPubKeyExp(PubKey_ptr, PrimeData_ptr);

   if( Error != CRYS_OK )
      goto End;
 
   /* copy the output data from the aligned buffer to the users data on big endian format */
   Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(Output_ptr, 4*((nSizeInBytes+3)/4), 
					PrimeData_ptr->DataOut, nSizeInBytes );
   if(Error != CRYS_OK)
            return Error;
   
   PLAT_LOG_DEV_PRINT_DisplayBuffer(( 22 , (char*)" \n IN CRYS_RSA_PRIM_Encrypt: After Exp operation PrimeData_ptr->DataOut = " ,
                                           (DxUint8_t*)PrimeData_ptr->DataOut ,
                                           sizeof(PrimeData_ptr->DataOut)));      
   
End:
   
   
   return Error;

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* END OF CRYS_RSA_PRIM_Encrypt */

#endif /*!defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)*/





#if !defined(_INTERNALCRYS_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)

/**********************************************************************************/ 
/**
@brief
CRYS_RSA_PRIM_Decrypt implements the RSADP algorithm as defined in PKCS#1 v2.1 6.1.2

  @param[in] PrivKey_ptr - Pointer to the private key data
                           structure. \note The RSA key parameters and hence the algorithm (CRT
                           or not) are determined by this structure. Using CRYS_Build_PrivKey or
                           CRYS_Build_PrivKeyCRT determines which algorithm will be used.
                           
  @param[in] PrimeData_ptr - A pointer to a structure containing internal
        		     buffers required for the RSA operation.
  @param[in] Data_ptr - Pointer to the data to be decrypted. 
  @param[in] DataSize - The size, in bytes, of the input data. To decrypt
			previously encrypted data its size must be equal to
			modulus size.
        		\Note: DataSize <= modulus size is supported, but
        		it is recommended that the size is equal to modulus size. 
			If smaller, the data will be zero-padded on left side 
			up to the modulus size and therefore, after further decrypt 
			operation its result will contain padding zeros also.
  @param[out] Output_ptr - Pointer to the decrypted data. The size of output data 
			is always equal to size modulus size. The output buffer
                        must be at least of modulus size in bytes.

  @return CRYSError_t - CRYS_OK,
                        CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                        CRYS_RSA_PRIM_DATA_STRUCT_POINTER_INVALID,
                        CRYS_RSA_PRIV_KEY_VALIDATION_TAG_ERROR,
                        CRYS_RSA_DATA_POINTER_INVALID_ERROR,
                        CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR,
                        CRYS_RSA_INVALID_MESSAGE_DATA_SIZE,
                        CRYS_RSA_INVALID_MESSAGE_VAL
 */

CEXPORT_C CRYSError_t CRYS_RSA_PRIM_Decrypt(
                                  CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
                                  CRYS_RSAPrimeData_t   *PrimeData_ptr,
                                  DxUint8_t             *Data_ptr,
                                  DxUint16_t             DataSize,
                                  DxUint8_t             *Output_ptr)
{
   /* FUNCTION LOCAL DECLERATIONS */
 
    /* the effective size in bits of the data message buffer */
   DxUint32_t DataEffectiveSizeInBits;

   /* the counter compare result */
   CRYS_COMMON_CmpCounter_t CounterCmpResult;

   /* the private key database pointer */
   CRYSRSAPrivKey_t *PrivKey_ptr;
   
   /* the modulus size in bytes */
   DxUint32_t nSizeInBytes; 

   /* The return error identifier */
   CRYSError_t Error;
  
   /* FUNCTION LOGIC */
 
   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserPrivKey_ptr , PrimeData_ptr , Data_ptr , 
                              DataSize , Output_ptr , DataEffectiveSizeInBits , CounterCmpResult , 
                              PrivKey_ptr , nSizeInBytes , Error , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
  
   /* ............... initialize local variables ......................... */
   /* -------------------------------------------------------------------- */
   
   /* initialize the error identifier to O.K - success */
   Error = CRYS_OK;

   
   /* ............... checking the parameters pointers validity .......... */
   /* -------------------------------------------------------------------- */
   
   /* ...... checking the key database handle pointer .................... */
   if(UserPrivKey_ptr == DX_NULL)
      return CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;

   /* ...... checking the Prime Data pointer .................... */
   if(PrimeData_ptr == DX_NULL)
      return CRYS_RSA_PRIM_DATA_STRUCT_POINTER_INVALID;

   /* if the users TAG is illegal return an error - the context is invalid */
   if(UserPrivKey_ptr->valid_tag != CRYS_RSA_PRIV_KEY_VALIDATION_TAG)
      return CRYS_RSA_PRIV_KEY_VALIDATION_TAG_ERROR;      

   /* if the users Data pointer is DX_NULL return an error */
   if(Data_ptr == DX_NULL)
      return CRYS_RSA_DATA_POINTER_INVALID_ERROR;
      
   /* if the users Output pointer is DX_NULL return an error */
   if(Output_ptr == DX_NULL)
      return CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR;

   #ifndef DX_OEM_FW
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPrivKey_ptr, sizeof(CRYS_RSAUserPrivKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, PrimeData_ptr, sizeof(CRYS_RSAPrimeData_t)) ||
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, Data_ptr, DataSize) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, Output_ptr, DataSize)){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
   #endif

   /* setting the pointer to the key database */
   PrivKey_ptr = (CRYSRSAPrivKey_t*)UserPrivKey_ptr->PrivateKeyDbBuff;

   /* setting the modulus size in bytes */
   nSizeInBytes = (PrivKey_ptr->nSizeInBits+7)/8;

   /* if the data size is larger then the modulus, return error */
   if(DataSize > nSizeInBytes)

	   return CRYS_RSA_INVALID_MESSAGE_DATA_SIZE;

#ifndef CRYS_NO_FIPS_SUPPORT
   if(DX_GLOBAL_FIPS_MODE & DX_CRYS_FIPS_MODE_STATE)
   {
   	/* check if there was a FIPS error */
	if(DX_GLOBAL_FIPS_MODE & DX_CRYS_FIPS_MODE_ERROR_STATE)
   		return CRYS_RSA_FIPS_MODE_FAIL_ERROR;

	/* not check if the self test was already executed */
   	if(!(DX_GLOBAL_FIPS_SF_STATUS & CRYS_SELF_TEST_RSA_ENCDEC_BIT))
   		return CRYS_RSA_FIPS_MODE_SF_NOT_PERFORMED_ERROR;
   }
#endif
   /* ................ copying the input data to the buffer .............. */
   /* -------------------------------------------------------------------- */
   
   /* clear the input data */
   DX_VOS_MemSet( PrimeData_ptr->DataIn, 0, sizeof(PrimeData_ptr->DataIn));

   /* copy the input data to the aligned buffer on the data handler in little endian format */
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
		PrimeData_ptr->DataIn, 4*((nSizeInBytes+3)/4), Data_ptr, DataSize);
   if(Error)
            return Error;
                                       
   /* ................ checking the data size and the message value ....... */
   /* --------------------------------------------------------------------- */
  
   /* initializing the effective counters size in bits */
   DataEffectiveSizeInBits =  CRYS_COMMON_GetCounterEffectiveSizeInBits(
							Data_ptr, DataSize); 
 
   /* if the size is 0 - return an error */
   if(DataEffectiveSizeInBits == 0){
     Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE;
     goto End;
   } 
   
   /* compare the message with the modulus: it must be less then the modulus */
   CounterCmpResult = CRYS_COMMON_CmpLsWordsUnsignedCounters( 
                           PrimeData_ptr->DataIn, (DxUint16_t)(nSizeInBytes+3)/4, 
                           PrivKey_ptr->n, (DxUint16_t)(nSizeInBytes+3)/4);
     
   if(CounterCmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1){
     Error = CRYS_RSA_INVALID_MESSAGE_VAL;     
     goto End;
   }  
           
   /* ..................... executing the encryption ...................... */
   /* --------------------------------------------------------------------- */           
     
   PLAT_LOG_DEV_PRINT_DisplayBuffer(( 22 , (char*)" \n IN CRYS_RSA_PRIM_Decrypt: Before Exp operation PrimeData_ptr->DataIn = " ,
                                           (DxUint8_t*)PrimeData_ptr->DataIn ,
                                           sizeof(PrimeData_ptr->DataIn)));           
     
   Error = LLF_PKI_RSA_ExecPrivKeyExp(PrivKey_ptr, PrimeData_ptr);
   
   if(Error)
      goto End;
 
   /* copy the output data from the aligned words-buffer to the users data in MS-LS bytes order */
   Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(Output_ptr, 4*((nSizeInBytes+3)/4),
				PrimeData_ptr->DataOut, nSizeInBytes);
   if(Error)
      goto End;
 
   PLAT_LOG_DEV_PRINT_DisplayBuffer(( 22 , (char*)" \n IN CRYS_RSA_PRIM_Decrypt: After Exp operation PrimeData_ptr->DataOut = " ,
                                           (DxUint8_t*)PrimeData_ptr->DataOut ,
                                           sizeof(PrimeData_ptr->DataOut)));            
                                           
End:
 
   /* clear the data buffer, besides the PrimeData_ptr->DataOut, used by  
      CRYS_RSA_PRIM_DecryptExactSize() function */
   DX_VOS_MemSetZero(PrimeData_ptr->DataIn , sizeof(PrimeData_ptr->DataIn));
   DX_VOS_MemSetZero((DxUint8_t*)&PrimeData_ptr->DataOut + sizeof(PrimeData_ptr->DataOut),
                      sizeof(CRYS_RSAPrimeData_t) - 2*sizeof(PrimeData_ptr->DataIn));
  
   return Error;
  
   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     
  							       
}/* END OF CRYS_RSA_PRIM_Decrypt */



#endif /*!defined(_INTERNAL_CRYS_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)*/


