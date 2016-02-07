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

/* .............. CRYS level includes ................. */

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
#include "plat_print_dbg.h"

#include "debug.h"
#ifndef DX_OEM_FW
#include "cc_acl.h"
#endif



/* .............. LLF level includes ................. */

#if !defined(CRYS_NO_HASH_SUPPORT) && !defined(CRYS_NO_PKI_SUPPORT)

#include "llf_pki_rsa.h"

#endif /* !defined(CRYS_NO_HASH_SUPPORT) && !defined(CRYS_NO_PKI_SUPPORT) */
 
/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************************ Public Functions ******************************/


#if !defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)

/******************************************************************************************/
/**
 * @brief CRYS_RSA_Build_PubKey populates a CRYSRSAPubKey_t structure with
 *       the provided modulus and exponent.
 *
 *	Assumption : the modulus and the exponent are presented in big endian.
 *
 * @param[out] PubKey_ptr - a pointer to the public key structure. This structure will be
 *            used as an input to the CRYS_RSA_PRIM_Encrypt API.
 *
 * @param[in] Exponent_ptr - a pointer to the exponent stream of bytes ( Big endian ).
 * @param[in] ExponentSize - The size of the exponent in bytes.  
 * @param[in] Modulus_ptr  - a pointer to the modulus stream of bytes ( Big endian ) the MS
 *           bit must be set to '1'.
 * @param[in] ModulusSize  - The size of the modulus in bytes. Sizes supported according to
 *           used platform from 64 to 256 bytes and in some platforms up to 512 bytes.  
 * 
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CEXPORT_C CRYSError_t CRYS_RSA_Build_PubKey(  
					CRYS_RSAUserPubKey_t *UserPubKey_ptr,
					DxUint8_t *Exponent_ptr,
					DxUint16_t ExponentSize,
					DxUint8_t *Modulus_ptr,
					DxUint16_t ModulusSize )
{
   /* FUNCTION DECLARATIONS */

   /* the counter compare result */
   CRYS_COMMON_CmpCounter_t CounterCmpResult; 

   /* the effective size in bits of the modulus buffer */
   DxUint32_t ModulusEffectiveSizeInBits;
  
   /* the effective size in bits of the exponent buffer */
   DxUint32_t ExponentEffectiveSizeInBits;
   
   /* the public key database pointer */
   CRYSRSAPubKey_t *PubKey_ptr;

   /* the Error return code identifier */
   CRYSError_t Error = CRYS_OK; 

   /* Max Size of buffers in Key structure */
   DxUint32_t  buffSizeBytes = CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES;

   /* FUNCTION LOGIC */

  
   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   
   RETURN_IF_RSA_UNSUPPORTED( UserPubKey_ptr, Exponent_ptr, ExponentSize, 
                              Modulus_ptr, ModulusSize, CounterCmpResult, 
                              ModulusEffectiveSizeInBits, ExponentEffectiveSizeInBits, 
                              PubKey_ptr, buffSizeBytes, Error, Error, Error, Error, 
                              Error, Error, Error, Error, Error, Error, Error, Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
   /* ................. checking the validity of the pointer arguments ....... */
   /* ------------------------------------------------------------------------ */
   

   #ifndef DX_OEM_FW
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPubKey_ptr, sizeof(CRYS_RSAUserPubKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, Exponent_ptr, ExponentSize) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, Modulus_ptr, ModulusSize)){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
   #endif

   /* ...... checking the key database handle pointer .................... */
   if( UserPubKey_ptr == DX_NULL )
      return CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;
      
   /* ...... checking the validity of the exponent pointer ............... */
   if( Exponent_ptr == DX_NULL )
      return CRYS_RSA_INVALID_EXPONENT_POINTER_ERROR;
      
   /* ...... checking the validity of the modulus pointer .............. */
   if( Modulus_ptr == DX_NULL )
      return CRYS_RSA_INVALID_MODULUS_POINTER_ERROR;

   if( ModulusSize > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES )
	   return CRYS_RSA_INVALID_MODULUS_SIZE;

   if( ExponentSize > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES )
	   return CRYS_RSA_INVALID_EXPONENT_SIZE;
   
   /* .................. copy the buffers to the key handle structure .... */
   /* -------------------------------------------------------------------- */
   /* setting the pointer to the key database */
   PubKey_ptr = ( CRYSRSAPubKey_t * )UserPubKey_ptr->PublicKeyDbBuff;
   
   /* clear the public key db */
   DX_VOS_MemSet( PubKey_ptr , 0 , sizeof(CRYSRSAPubKey_t) );

   /* loading the buffers to little endian order of words in array; each word is loaded according to CPU endianness */
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PubKey_ptr->n, buffSizeBytes, Modulus_ptr, ModulusSize);
   if(Error)
	   return Error;

   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PubKey_ptr->e, buffSizeBytes, Exponent_ptr, ExponentSize);
   if(Error)
	   return Error;

   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */
         
   /* .......... initializing the effective counters size in bits .......... */
   ModulusEffectiveSizeInBits =  CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PubKey_ptr->n, (ModulusSize+3)/4); 
   ExponentEffectiveSizeInBits = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PubKey_ptr->e, (ExponentSize+3)/4);   

   /* .................. checking the validity of the counters ............... */
   /* ------------------------------------------------------------------------ */
   if((ModulusEffectiveSizeInBits < CRYS_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
      (ModulusEffectiveSizeInBits > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
      (ModulusEffectiveSizeInBits % CRYS_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS)){

	   Error = CRYS_RSA_INVALID_MODULUS_SIZE;    
	   goto End;
   }     
   /*  verifying the modulus is odd  */
   if((PubKey_ptr->n[0] & 1UL) == 0){
	   Error = CRYS_RSA_MODULUS_EVEN_ERROR; 
	   goto End;
   }   
   
   /*  checking the exponent size is not 0 in bytes */
   if( ExponentEffectiveSizeInBits == 0 ){
	   Error = CRYS_RSA_INVALID_EXPONENT_SIZE;
	   goto End;
   }    
      
   /*  verifying the exponent is less then the modulus */
   CounterCmpResult = CRYS_COMMON_CmpMsbUnsignedCounters(Exponent_ptr, ExponentSize, Modulus_ptr, ModulusSize);
     
   if(CounterCmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1){
	   Error = CRYS_RSA_INVALID_EXPONENT_VAL;
	   goto End;
   }   
    
   /*  verifying the exponent is not less then 3 */
   if(ExponentEffectiveSizeInBits < 32 && PubKey_ptr->e[0] < CRYS_RSA_MIN_PUB_EXP_VALUE){       
	    Error = CRYS_RSA_INVALID_EXPONENT_VAL;
	    goto End;
   }   
        
   /* ................. building the structure ............................. */
   /* ---------------------------------------------------------------------- */
     
   /* setting the modulus and exponent size in bits */ 
   PubKey_ptr->nSizeInBits = ModulusEffectiveSizeInBits;   
   PubKey_ptr->eSizeInBits = ExponentEffectiveSizeInBits;
  
   /* ................ initialize the low level data .............. */
   Error = LLF_PKI_RSA_InitPubKeyDb(PubKey_ptr);
   
   if(Error)
     goto End; 
           
   /* ................ set the tag ................ */  
   UserPubKey_ptr->valid_tag = CRYS_RSA_PUB_KEY_VALIDATION_TAG;
   
   /* ................. end of the function .................................. */
   /* ------------------------------------------------------------------------ */
   
   End:
        
   /* if the structure created is not valid - clear it */
   if( Error != CRYS_OK ){
      DX_VOS_MemSet(UserPubKey_ptr, 0, sizeof(CRYS_RSAUserPubKey_t));
   }

   return Error;

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* END OF CRYS_RSA_Build_PubKey */

#endif /*!defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)*/



#ifndef DX_OEM_FW

#if !defined(_INTERNAL_CRYS_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)
/******************************************************************************************/
/**
 * @brief CRYS_RSA_Build_PrivKey populates a CRYSRSAPrivKey_t structure with
 *        the provided modulus and exponent, marking the key as a "non-CRT" key.
 *
 *        Assumption : the modulus and the exponent are presented in big endian.
 *
 * @param[out] UserPrivKey_ptr - a pointer to the public key structure. this structure will be used as
 *                          an input to the CRYS_RSA_PRIM_Decrypt API.
 * @param[in] PrivExponent_ptr - a pointer to the private exponent stream of bytes ( Big endian ).
 * @param[in] PrivExponentSize - the size of the private exponent in bytes.
 * @param[in] Exponent_ptr - a pointer to the exponent stream of bytes ( Big endian ).
 * @param[in] ExponentSize - the size of the exponent in bytes.  
 * @param[in] Modulus_ptr  - a pointer to the modulus stream of bytes ( Big endian ) the MS
 *            bit must be set to '1'.
 * @param[in] ModulusSize  - the size of the modulus in bytes. Sizes supported according to
 *            used platform from 64 to 256 bytes and in some platforms up to 512 bytes.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                       value MODULE_* as defined in ...
 *
 */
CEXPORT_C CRYSError_t CRYS_RSA_Build_PrivKey(CRYS_RSAUserPrivKey_t 	 *UserPrivKey_ptr,
				                   DxUint8_t             *PrivExponent_ptr,
				                   DxUint16_t             PrivExponentSize,
				                   DxUint8_t             *PubExponent_ptr,
				                   DxUint16_t             PubExponentSize,
				                   DxUint8_t             *Modulus_ptr,
				                   DxUint16_t             ModulusSize )
{				                   
   /* FUNCTION DECLARATIONS */
 
    /* the counter compare result */
   CRYS_COMMON_CmpCounter_t CounterCmpResult; 
  
   /* the effective size in bits of the modulus buffer */
   DxUint32_t ModulusEffectiveSizeInBits;
  
   /* the effective sizes in bits of the private and public exponents */
   DxUint32_t PrivExponentEffectiveSizeInBits, PubExponentEffectiveSizeInBits;

   /* the private key database pointer */
   CRYSRSAPrivKey_t *PrivKey_ptr;

   /* Max Size of buffers in Key structure */
   DxUint32_t  buffSizeBytes = CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES;

   /* the Error return code identifier */
   CRYSError_t Error = CRYS_OK; 

   /* FUNCTION LOGIC */
 
   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserPrivKey_ptr, PrivExponent_ptr, PrivExponentSize, 
			PubExponent_ptr, PubExponentSize, Modulus_ptr, 
			ModulusSize, CounterCmpResult, ModulusEffectiveSizeInBits,
			PrivExponentEffectiveSizeInBits, PubExponentEffectiveSizeInBits, 
			PrivKey_ptr, buffSizeBytes, Error, Error, Error, Error, Error, 
			Error, Error, Error, Error ); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
 
   /* ................. checking the validity of the pointer arguments ....... */
   /* ------------------------------------------------------------------------ */
   
   /* ...... checking the key database handle pointer .................... */
   if(UserPrivKey_ptr == DX_NULL)
      return CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;
      
   /* ...... checking the validity of the exponents pointers ........... */
   if(PrivExponent_ptr == DX_NULL)
      return CRYS_RSA_INVALID_EXPONENT_POINTER_ERROR;
      
   /* ...... checking the validity of the modulus pointer .............. */
   if(Modulus_ptr == DX_NULL)
      return CRYS_RSA_INVALID_MODULUS_POINTER_ERROR;

   /* ...... checking the validity of the modulus size, private exponent can not be more than 256 bytes .............. */
   if(ModulusSize > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
      return CRYS_RSA_INVALID_MODULUS_SIZE;

   if(PrivExponentSize > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / 8)
      return CRYS_RSA_INVALID_EXPONENT_SIZE;

   if(PubExponent_ptr != DX_NULL && 
      PubExponentSize > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
      return CRYS_RSA_INVALID_EXPONENT_SIZE;

#ifndef DX_OEM_FW
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPrivKey_ptr, sizeof(CRYS_RSAUserPrivKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, PrivExponent_ptr, PrivExponentSize) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, PubExponent_ptr, PubExponentSize) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, Modulus_ptr, ModulusSize)){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
#endif

   /* .................. copy the buffers to the key handle structure .... */
   /* -------------------------------------------------------------------- */
   
   /* setting the pointer to the key database */
   PrivKey_ptr = (CRYSRSAPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;
  
   /* clear the private key db */
   DX_VOS_MemSet(PrivKey_ptr, 0, sizeof(CRYSRSAPrivKey_t));

   /* loading the buffers to little endian order of words in array; each word is loaded according to CPU endianness */
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->n, buffSizeBytes, Modulus_ptr, ModulusSize);
   if(Error != CRYS_OK)
	   return Error;

   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.NonCrt.d, buffSizeBytes, 
							PrivExponent_ptr, PrivExponentSize);
   if(Error != CRYS_OK)
	   return Error;

   /* get actual sizes of modulus and private exponent */
   ModulusEffectiveSizeInBits =  
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->n, (ModulusSize+3)/4); 
  
   PrivExponentEffectiveSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.NonCrt.d, (PrivExponentSize+3)/4);
 

   /* .................. checking the validity of the counters ............... */
   /* ------------------------------------------------------------------------ */
   
   /*  checking the size of the modulus  */
   if(( ModulusEffectiveSizeInBits < CRYS_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
      ( ModulusEffectiveSizeInBits > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
      ( ModulusEffectiveSizeInBits % CRYS_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS)){
	      Error = CRYS_RSA_INVALID_MODULUS_SIZE;    
	      goto End;
   }   
 
   /*  verifying the modulus is odd  */
   if((PrivKey_ptr->n[0] & 1UL) == 0){
      Error = CRYS_RSA_MODULUS_EVEN_ERROR;
      goto End;
   }   
 
   /*  checking the priv. exponent size is not 0 in bytes */   
   if(PrivExponentEffectiveSizeInBits == 0){ 
      Error = CRYS_RSA_INVALID_EXPONENT_SIZE;
      goto End;
   }   
      
   /* verifying the priv. exponent is less then the modulus */
   CounterCmpResult = CRYS_COMMON_CmpMsbUnsignedCounters(PrivExponent_ptr, PrivExponentSize, 
							Modulus_ptr, ModulusSize);
     
   if(CounterCmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1){
      Error = CRYS_RSA_INVALID_EXPONENT_VAL;
      goto End;
   }   
      
   /* verifying the priv. exponent is not less then 1 */
   if(PrivExponentEffectiveSizeInBits < 32 && 
      PrivKey_ptr->PriveKeyDb.NonCrt.d[0] < CRYS_RSA_MIN_PRIV_EXP_VALUE){
	Error = CRYS_RSA_INVALID_EXPONENT_VAL;
	goto End;
   }   
        
   /*  checking that the public exponent is an integer between 3 and modulus - 1 */
   if( PubExponent_ptr != DX_NULL )
   {
	   /* loading the buffer to little endian order of words in array; each word is loaded according to CPU endianness */
	   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords( PrivKey_ptr->PriveKeyDb.NonCrt.e, buffSizeBytes, 
								PubExponent_ptr, PubExponentSize);
	   if(Error)
		   return Error;

	   PubExponentEffectiveSizeInBits = 
		   CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.NonCrt.e, (PubExponentSize+3)/4);

	   /* verifying that the exponent is not less than 3 */
	   if(PubExponentEffectiveSizeInBits < 32 && 
	      PrivKey_ptr->PriveKeyDb.NonCrt.e[0] < CRYS_RSA_MIN_PUB_EXP_VALUE){       
		   Error = CRYS_RSA_INVALID_EXPONENT_VAL;
		   goto End;
	   }

	   /* verifying that the public exponent is less than the modulus */
	   CounterCmpResult = CRYS_COMMON_CmpMsbUnsignedCounters(PubExponent_ptr, PubExponentSize, 
								 Modulus_ptr, ModulusSize);

	   if(CounterCmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1){
		   Error = CRYS_RSA_INVALID_EXPONENT_VAL;
		   goto End;
	   }   
   }
   else
	PubExponentEffectiveSizeInBits = 0;


   /* ................. building the structure ............................. */
   /* ---------------------------------------------------------------------- */
   
   /* set the mode to non CRT mode */
   PrivKey_ptr->OperationMode = CRYS_RSA_NoCrt;
 
   /* set the key source as external */
   PrivKey_ptr->KeySource = CRYS_RSA_ExternalKey;
     
   /* setting the modulus and exponent size in bits */ 
   PrivKey_ptr->nSizeInBits                   = ModulusEffectiveSizeInBits;   
   PrivKey_ptr->PriveKeyDb.NonCrt.dSizeInBits = PrivExponentEffectiveSizeInBits;
   PrivKey_ptr->PriveKeyDb.NonCrt.eSizeInBits = PubExponentEffectiveSizeInBits;
   
   /* ................ initialize the low level data .............. */
   Error = LLF_PKI_RSA_InitPrivKeyDb(PrivKey_ptr);
   
   if(Error)
   
     goto End; 
           
   /* ................ set the tag ................ */  
   UserPrivKey_ptr->valid_tag = CRYS_RSA_PRIV_KEY_VALIDATION_TAG;
   
   /* ................. end of the function .................................. */
   /* ------------------------------------------------------------------------ */
   
   End:
      
   /* if the structure created is not valid - clear it */
   if(Error)
      DX_VOS_MemSet(UserPrivKey_ptr, 0, sizeof(CRYS_RSAUserPrivKey_t)); 

   return Error;

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     
   
}/* END OF CRYS_RSA_Build_PrivKey */


#ifndef _INTERNAL_CRYS_NO_RSA_CRT_SUPPORT
/******************************************************************************************

   @brief CRYS_RSA_Build_PrivKeyCRT populates a CRYSRSAPrivKey_t structure with
          the provided parameters, marking the key as a "CRT" key.
 
	Note: The "First" factor P must be great, than the "Second" factor Q.
 

   @param[out] UserPrivKey_ptr - A pointer to the public key structure. 
        		    This structure is used as input to the CRYS_RSA_PRIM_Decrypt API.
   @param[in] P_ptr - A pointer to the first factor stream of bytes (Big-Endian format)
   @param[in] PSize - The size of the first factor, in bytes.
   @param[in] Q_ptr - A pointer to the second factor stream of bytes (Big-Endian format)
   @param[in] QSize - The size of the second factor, in bytes.
   @param[in] dP_ptr - A pointer to the first factor's CRT exponent stream of bytes (Big-Endian format)
   @param[in] dPSize - The size of the first factor's CRT exponent, in bytes.
   @param[in] dQ_ptr - A pointer to the second factor's CRT exponent stream of bytes (Big-Endian format)
   @param[in] dQSize - The size of the second factor's CRT exponent, in bytes.
   @param[in] qInv_ptr - A pointer to the first CRT coefficient stream of bytes (Big-Endian format)
   @param[in] qInvSize - The size of the first CRT coefficient, in bytes.

*/
CEXPORT_C CRYSError_t CRYS_RSA_Build_PrivKeyCRT(
				CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
				DxUint8_t *P_ptr, 
				DxUint16_t PSize,
				DxUint8_t *Q_ptr,
				DxUint16_t QSize,
				DxUint8_t *dP_ptr, 
				DxUint16_t dPSize,
				DxUint8_t *dQ_ptr,
				DxUint16_t dQSize,
				DxUint8_t *qInv_ptr,
				DxUint16_t qInvSize)
{				                   
   /* FUNCTION DECLARATIONS */

    /* the counter compare result */
   CRYS_COMMON_CmpCounter_t CounterCmpResult; 

   /* the effective size in bits of the modulus factors buffer */
   DxUint32_t P_EffectiveSizeInBits;
   DxUint32_t Q_EffectiveSizeInBits;
   DxUint32_t dP_EffectiveSizeInBits;
   DxUint32_t dQ_EffectiveSizeInBits;
   DxUint32_t qInv_EffectiveSizeInBits;
   DxUint32_t ModulusEffectiveSizeInBits;

   /* the private key database pointer */
   CRYSRSAPrivKey_t *PrivKey_ptr;

   /* Max Size of buffers in CRT Key structure */
   DxUint32_t  buffSizeBytes = 4*((PSize + 3)/4) + 4;

   /* the Error return code identifier */
   CRYSError_t Error = CRYS_OK; 
  
   /* FUNCTION LOGIC */

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserPrivKey_ptr, P_ptr, PSize, Q_ptr, QSize, dP_ptr, 
                              dPSize, dQ_ptr, dQSize, qInv_ptr, qInvSize, qInv_ptr, qInvSize,
                              CounterCmpResult, P_EffectiveSizeInBits, Q_EffectiveSizeInBits,
                              dP_EffectiveSizeInBits, dQ_EffectiveSizeInBits, qInv_EffectiveSizeInBits,
                              ModulusEffectiveSizeInBits, PrivKey_ptr, Error ); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      

   /* ................. checking the validity of the pointer arguments ....... */
   /* ------------------------------------------------------------------------ */
   
   /* ...... checking the key database handle pointer .................... */
   if(UserPrivKey_ptr == DX_NULL)
      return CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;
      
   /* checking the first factor pointer validity */
   if(P_ptr == DX_NULL)
      return CRYS_RSA_INVALID_CRT_FIRST_FACTOR_POINTER_ERROR;   

   /* checking the second factor pointer validity */
   if(Q_ptr == DX_NULL)
      return CRYS_RSA_INVALID_CRT_SECOND_FACTOR_POINTER_ERROR; 
   
   /* checking the first factor exponent pointer validity */
   if(dP_ptr == DX_NULL)
      return CRYS_RSA_INVALID_CRT_FIRST_FACTOR_EXP_PTR_ERROR;   

   /* checking the second factor exponent pointer validity */
   if(dQ_ptr == DX_NULL)
      return CRYS_RSA_INVALID_CRT_SECOND_FACTOR_EXP_PTR_ERROR;
      
   /* checking the CRT coefficient */
   if(qInv_ptr == DX_NULL)
      return CRYS_RSA_INVALID_CRT_COEFFICIENT_PTR_ERROR;

   /* checking the input sizes */
   if(dPSize > PSize ||
      dQSize > QSize ||
      qInvSize > PSize){
	   return CRYS_RSA_INVALID_CRT_PARAMETR_SIZE_ERROR;
   }

   #ifndef DX_OEM_FW
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPrivKey_ptr, sizeof(CRYS_RSAUserPrivKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, P_ptr, PSize) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, Q_ptr, QSize) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, dP_ptr, dPSize) ||
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, dQ_ptr, dQSize) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, qInv_ptr, qInvSize)){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
   #endif 
   
   /* verifying the first factor exponent is less then the first factor */
   CounterCmpResult = 
      CRYS_COMMON_CmpMsbUnsignedCounters(dP_ptr, dPSize, P_ptr, PSize);
     
   if(CounterCmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1){ 
      Error = CRYS_RSA_INVALID_CRT_FIRST_FACTOR_EXPONENT_VAL;   
      goto End;
   }
     
   /* verifying the second factor exponent is less then the second factor */
   CounterCmpResult = 
      CRYS_COMMON_CmpMsbUnsignedCounters(dQ_ptr, dQSize, Q_ptr, QSize);
     
   if(CounterCmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1){ 
      Error = CRYS_RSA_INVALID_CRT_SECOND_FACTOR_EXPONENT_VAL;
      goto End;
   }   
      
   /* verifying the CRT coefficient is less then the first factor */
   CounterCmpResult = 
      CRYS_COMMON_CmpMsbUnsignedCounters(qInv_ptr, qInvSize, P_ptr, PSize);
     
   if(CounterCmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1){ 
      Error = CRYS_RSA_INVALID_CRT_COEFF_VAL;
      goto End;
   } 

    
   /* .................. copy the buffers to the key handle structure .... */
   /* -------------------------------------------------------------------- */

   /* setting the pointer to the key database */
   PrivKey_ptr = (CRYSRSAPrivKey_t*)UserPrivKey_ptr->PrivateKeyDbBuff;

   /* clear the private key db */
   DX_VOS_MemSet(PrivKey_ptr, 0, sizeof(CRYSRSAPrivKey_t));

   /* load the buffers to the data base */
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.P, buffSizeBytes, P_ptr, PSize);
   if(Error)
       return Error;
                              
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.Q, buffSizeBytes, Q_ptr, QSize);
   if(Error)
       return Error;
                              
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.dP, buffSizeBytes, dP_ptr, dPSize);
   if(Error)
       return Error;
                              
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.dQ, buffSizeBytes, dQ_ptr, dQSize);
   if(Error)
       return Error;
                              
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.qInv, buffSizeBytes, qInv_ptr, qInvSize);
   if(Error)
       return Error;

   /* ............... initialize local variables ......................... */
   /* -------------------------------------------------------------------- */
   
   /* initializing the effective counters size in bits */
   P_EffectiveSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.P, (PSize+3)/4);
       
   Q_EffectiveSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.Q, (QSize+3)/4);   
      
   dP_EffectiveSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.dP, (dPSize+3)/4); 
      
   dQ_EffectiveSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.dQ, (dQSize+3)/4);
      
   qInv_EffectiveSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.qInv, (qInvSize+3)/4); 
  
   /*  the first factor size is not 0 in bits */
   if(P_EffectiveSizeInBits == 0){
     Error = CRYS_RSA_INVALID_CRT_FIRST_FACTOR_SIZE;
     goto End;
   }  

   /* the second factor size is not 0 in bits */
   if(Q_EffectiveSizeInBits == 0){
     Error = CRYS_RSA_INVALID_CRT_SECOND_FACTOR_SIZE;
     goto End;
   }  

   /* checking that sizes of dP, dQ, qInv > 0 */
   if(dP_EffectiveSizeInBits == 0 || dQ_EffectiveSizeInBits == 0 || qInv_EffectiveSizeInBits == 0){
	   Error = CRYS_RSA_INVALID_CRT_PARAMETR_SIZE_ERROR;
	   goto End;
   }



   /* ............... calculate the modulus N ........................... */
   /* -------------------------------------------------------------------- */
  
  
   Error = LLF_PKI_RSA_CallRMul(PrivKey_ptr->PriveKeyDb.Crt.P, P_EffectiveSizeInBits,
                                PrivKey_ptr->PriveKeyDb.Crt.Q, PrivKey_ptr->n);
   if(Error)
     goto End;                              
 
   ModulusEffectiveSizeInBits =  
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->n, (2*P_EffectiveSizeInBits+31)/32); 
   
   /* .................. checking the validity of the counters ............... */
   /* ------------------------------------------------------------------------ */
   
   /* the size of the modulus  */
   if(( ModulusEffectiveSizeInBits < CRYS_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS) ||
      ( ModulusEffectiveSizeInBits > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS) ||
      ( ModulusEffectiveSizeInBits % CRYS_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS)){
      Error = CRYS_RSA_INVALID_MODULUS_SIZE;    
      goto End;
   }   
   
   if((P_EffectiveSizeInBits + Q_EffectiveSizeInBits != ModulusEffectiveSizeInBits) &&
      (P_EffectiveSizeInBits + Q_EffectiveSizeInBits != ModulusEffectiveSizeInBits - 1)){    
      Error = CRYS_RSA_INVALID_CRT_FIRST_AND_SECOND_FACTOR_SIZE;
      goto End;
   }   
     
  
   /* ................. building the structure ............................. */
   /* ---------------------------------------------------------------------- */

   /* set the mode to CRT mode */
   PrivKey_ptr->OperationMode = CRYS_RSA_Crt;
   
   /* set the key source as external */
   PrivKey_ptr->KeySource = CRYS_RSA_ExternalKey;
   
   /* loading to structure the buffer sizes... */
   
   PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits    = P_EffectiveSizeInBits; 
   PrivKey_ptr->PriveKeyDb.Crt.QSizeInBits    = Q_EffectiveSizeInBits; 
   PrivKey_ptr->PriveKeyDb.Crt.dPSizeInBits   = dP_EffectiveSizeInBits; 
   PrivKey_ptr->PriveKeyDb.Crt.dQSizeInBits   = dQ_EffectiveSizeInBits;
   PrivKey_ptr->PriveKeyDb.Crt.qInvSizeInBits = qInv_EffectiveSizeInBits;
   PrivKey_ptr->nSizeInBits = ModulusEffectiveSizeInBits;
 
   /* ................ initialize the low level data .............. */
   Error = LLF_PKI_RSA_InitPrivKeyDb(PrivKey_ptr);
   
   if( Error != CRYS_OK )
   
     goto End; 
     							
   /* ................ set the tag ................ */  
   UserPrivKey_ptr->valid_tag = CRYS_RSA_PRIV_KEY_VALIDATION_TAG;

   /* ................. end of the function .................................. */
   /* ------------------------------------------------------------------------ */

   End:
 
   /* if the structure created is not valid - clear it */
   if(Error)
   
      DX_VOS_MemSet(UserPrivKey_ptr, 0, sizeof(CRYS_RSAUserPrivKey_t)); 

   return Error;
 
   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     
      
}/* END OF CRYS_RSA_Build_PrivKeyCRT */
#endif


/******************************************************************************************
@brief The CRYS_RSA_Build_ConvertPrivKeyToCRT function convert the non CRT private
       key to CRT private Key and initializes appropriate fields of input -
       output structure provided by pointer UserPrivKey_ptr.
       All input and output data is in little endian representation.

@param[in,out] UserPrivKey_ptr - A pointer to the user private key structure. The key must 
               be in non CRT mode. This structure is used as input to the
               CRYS_RSA_PRIM_Encrypt API.

@param[in]   Buffers_ptr -     A pointer to the structure containing temporary buffers used by LLF functions 
             called by this function. Size of buffer: 5 + 36 = 41 maximum modulus sizes.

NOTE: All members of input UserPrivKey structure must be initialized, including public key
      e-pointer and it size.
*/
CEXPORT_C CRYSError_t CRYS_RSA_Build_ConvertPrivKeyToCRT(CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	 		                                 CRYS_RSAConvertKeyToCrtBuffers_t *Buffers_ptr)
{
 	/******** FUNCTION DECLARATIONS **********/
 
   /* the pointers to the non CRT private key members: modulus n, private key d, public key e */
   DxUint32_t *n_ptr; 
   DxUint32_t *e_ptr; 
   
   /* the modulus effective size in bits and in bytes */
   DxUint32_t	nSizeInBits;
   DxUint32_t	nSizeInBytes;
   /* the effective size in bits of the private and public key exp  */
   DxUint32_t dSizeInBits;
   DxUint32_t eSizeInBits;

   /* the private key database pointer */
   CRYSRSAPrivKey_t *PrivKey_ptr;
 
   /* the private non CRT key structure pointer (union with CRT key) */
   CRYSRSAPrivNonCRTKey_t	*PrivNonCRTKey_ptr;
  
   /* the private CRT key structure pointer (union with non CRT key) */
   CRYSRSAPrivCRTKey_t	*PrivCRTKey_ptr; 

   /* the Error return code identifier */
   CRYSError_t Error = CRYS_OK;    
     
  
   /*************   FUNCTION LOGIC   ************************/

   /* ............... If not supported exit .............................. */    

   RETURN_IF_RSA_UNSUPPORTED( UserPrivKey_ptr, Buffers_ptr, n_ptr, e_ptr,  
                              nSizeInBits, nSizeInBytes, dSizeInBits, eSizeInBits,  
                              PrivKey_ptr, PrivNonCRTKey_ptr, PrivCRTKey_ptr,  
                              CounterCmpResult, Error, Error, Error, Error,
                              Error, Error, Error, Error, Error, Error ); 
                              
 
#ifndef CRYS_NO_HASH_SUPPORT                                      
#ifndef CRYS_NO_PKI_SUPPORT   
  
  
   /* ...... Checking the validity of the pointer arguments ........... */ 

   /*  Checking the key database handle pointer  */
   if(UserPrivKey_ptr == DX_NULL)
      return CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;
   
   /* If the users TAG is illegal return an error - the context is invalid */
   if(UserPrivKey_ptr->valid_tag != CRYS_RSA_PRIV_KEY_VALIDATION_TAG)
      return CRYS_RSA_PRIV_KEY_VALIDATION_TAG_ERROR;   
       
   /* Checking the temp buffers pointer validity */
   if(Buffers_ptr == DX_NULL)
      return CRYS_RSA_CONV_TO_CRT_INVALID_TEMP_BUFF_POINTER_ERROR; 
      
#ifndef DX_OEM_FW
   if(DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPrivKey_ptr, sizeof(CRYS_RSAUserPrivKey_t)) || 
      DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, Buffers_ptr, sizeof(CRYS_RSAConvertKeyToCrtBuffers_t))){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
#endif      

   /*****    Initialization of pointers and variables       *********/   
   
   /* The pointer to the key database */
   PrivKey_ptr = ( CRYSRSAPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;  

   /* The private non CRT key structure pointer (union with CRT key) */
   PrivNonCRTKey_ptr = (CRYSRSAPrivNonCRTKey_t *)&PrivKey_ptr->PriveKeyDb;
   
   /* The private CRT key structure pointer (union with non CRT key) */
   PrivCRTKey_ptr = (CRYSRSAPrivCRTKey_t *)&PrivKey_ptr->PriveKeyDb;

   /* The pointers to modulus, private key and public key exponents */
   n_ptr = PrivKey_ptr->n;
   e_ptr = PrivKey_ptr->PriveKeyDb.NonCrt.e;
   
   /* The modulus size in bits and in bytes */
   nSizeInBits = PrivKey_ptr->nSizeInBits;
   nSizeInBytes	= (nSizeInBits + 7) / 8; 

   /* The private key and public key exponents size in bits  */
   dSizeInBits = PrivKey_ptr->PriveKeyDb.NonCrt.dSizeInBits;
   eSizeInBits = PrivKey_ptr->PriveKeyDb.NonCrt.eSizeInBits;
 
   /***************** Additional checking *****************************/
   
   /*  check the key operation mode and public key size  */
   if(PrivKey_ptr->OperationMode != CRYS_RSA_NoCrt)
	   return CRYS_RSA_WRONG_PRIVATE_KEY_TYPE;   

   if(e_ptr == DX_NULL)
	   return CRYS_RSA_INVALID_EXPONENT_POINTER_ERROR;

   if(n_ptr == DX_NULL)
	   return CRYS_RSA_INVALID_MODULUS_POINTER_ERROR;
    		
   if(PrivNonCRTKey_ptr->eSizeInBits == 0)
	   return CRYS_RSA_INVALID_EXPONENT_SIZE;
      
   /*  check the size of the modulus  */
   if(( nSizeInBits < CRYS_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
       ( nSizeInBits > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
       ( nSizeInBits % CRYS_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS))
	   return CRYS_RSA_INVALID_MODULUS_SIZE;       
      
   /* verifying that the modulus is odd */
   if((n_ptr[0] & 1UL) == 0) 
	   return CRYS_RSA_MODULUS_EVEN_ERROR;  
 

   /******* Call LLF function for converting private key to CRT mode ******/  
   
   Error = LLF_PKI_RSA_ConvertPrivKeyToCRT( 
				PrivNonCRTKey_ptr, 
				n_ptr, nSizeInBits,
				PrivCRTKey_ptr,
				Buffers_ptr);
   
   /************* Building the private CRT key structure ************/ 
                                
   if( Error == CRYS_OK )   
   {
	   /* Calculating the effective sizes of CRT private key members (in bits) */
	    
	   PrivCRTKey_ptr->PSizeInBits  = 
	      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivCRTKey_ptr->P, (DxUint16_t)(nSizeInBytes+3)/4);
	       
	   PrivCRTKey_ptr->QSizeInBits = 
	      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivCRTKey_ptr->Q, (DxUint16_t)(nSizeInBytes+3)/4);   
	      
	   PrivCRTKey_ptr->dPSizeInBits = 
	      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivCRTKey_ptr->dP, (DxUint16_t)(nSizeInBytes+3)/4); 
	      
	   PrivCRTKey_ptr->dQSizeInBits = 
	      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivCRTKey_ptr->dQ, (DxUint16_t)(nSizeInBytes+3)/4);
	      
	   PrivCRTKey_ptr->qInvSizeInBits = 
	      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivCRTKey_ptr->qInv, (DxUint16_t)(nSizeInBytes+3)/4); 
	  
	   /* set the mode to CRT mode */
	   PrivKey_ptr->OperationMode = CRYS_RSA_Crt;
	   
	   /* set the key source as external */
	   PrivKey_ptr->KeySource = CRYS_RSA_ExternalKey;      	
   }

   /* Delete private information from temporary buffers */
   DX_VOS_MemSetZero((DxUint8_t *)Buffers_ptr, sizeof(CRYS_RSAConvertKeyToCrtBuffers_t));
  

   return Error;
 
#endif /* !CRYS_NO_HASH_SUPPORT */
#endif /* !CRYS_NO_PKI_SUPPORT */                                     
 	
} /* end of CRYS_RSA_Build_ConvertPrivKeyToCRT() */

#endif /*!defined(_INTERNAL_CRYS_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)*/



#if !defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)

/******************************************************************************************
   @brief CRYS_RSA_Get_PubKey gets the e,n public key from the database.
   
   @param[in] UserPubKey_ptr - A pointer to the public key structure. 
                               This structure is used as input to the CRYS_RSA_PRIM_Encrypt API.
 
   @param[out] Exponent_ptr - A pointer to the exponent stream of bytes (Big-Endian format)
   @param[in,out] ExponentSize_ptr - the size of the exponent buffer in bytes, it is updated to the 
                  actual size of the exponent, in bytes.  
   @param[out] Modulus_ptr  - A pointer to the modulus stream of bytes (Big-Endian format).
			   The MS (most significant) bit must be set to '1'.
   @param[in,out] ModulusSize_ptr  - the size of the modulus buffer in bytes, it is updated to the 
                  actual size of the modulus, in bytes.

   NOTE: All members of input UserPrivKey structure must be initialized, including public key
         e pointer and it size.

*/
CEXPORT_C CRYSError_t CRYS_RSA_Get_PubKey(  
				CRYS_RSAUserPubKey_t *UserPubKey_ptr,
				DxUint8_t  *Exponent_ptr,
				DxUint16_t *ExponentSize_ptr,
				DxUint8_t  *Modulus_ptr,
				DxUint16_t *ModulusSize_ptr )
{	
   /* LOCAL DECLERATIONS */
	
   /* the size in bytes of the modulus and the exponent */
   DxUint32_t nSizeInBytes;
   DxUint32_t eSizeInBytes;	   
   /* the public key database pointer */
   CRYSRSAPubKey_t *PubKey_ptr;

   CRYSError_t Error;
		                   
   /* FUNCTION DECLERATIONS */

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserPubKey_ptr , Exponent_ptr , ExponentSize_ptr , 
                              Modulus_ptr , ModulusSize_ptr , nSizeInBytes , 
                              eSizeInBytes , PubKey_ptr , PubKey_ptr,
                              PubKey_ptr , PubKey_ptr , PubKey_ptr , PubKey_ptr ,
                              PubKey_ptr , PubKey_ptr , PubKey_ptr ,
                              PubKey_ptr , PubKey_ptr , PubKey_ptr ,
                              PubKey_ptr , PubKey_ptr , PubKey_ptr ); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
 
   /* ................. checking the validity of the pointer arguments ....... */
   /* ------------------------------------------------------------------------ */
   
   /* ...... checking the key database handle pointer .................... */
   if(UserPubKey_ptr == DX_NULL)
      return CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;
      
   /* ...... checking the validity of the exponent pointer ............... */
   if(Exponent_ptr == DX_NULL)
      return CRYS_RSA_INVALID_EXPONENT_POINTER_ERROR;
      
   /* ...... checking the validity of the modulus pointer .............. */
   if(Modulus_ptr == DX_NULL)
      return CRYS_RSA_INVALID_MODULUS_POINTER_ERROR;

   if(ExponentSize_ptr == DX_NULL)
      return CRYS_RSA_INVALID_EXP_BUFFER_SIZE_POINTER;

   if(ModulusSize_ptr == DX_NULL)
      return CRYS_RSA_INVALID_MOD_BUFFER_SIZE_POINTER;
      
   /* if the users TAG is illegal return an error - the context is invalid */
   if(UserPubKey_ptr->valid_tag != CRYS_RSA_PUB_KEY_VALIDATION_TAG)
      return CRYS_RSA_PUB_KEY_VALIDATION_TAG_ERROR;      
   
   #ifndef DX_OEM_FW
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPubKey_ptr, sizeof(CRYS_RSAUserPubKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, Exponent_ptr, *ExponentSize_ptr) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, Modulus_ptr, *ModulusSize_ptr) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, ExponentSize_ptr, sizeof(DxUint32_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ModulusSize_ptr, sizeof(DxUint32_t))){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
   #endif      
      
   /* ...... checking the exponent size ................................ */
   
   /* setting the pointer to the key database */
   PubKey_ptr = ( CRYSRSAPubKey_t * )UserPubKey_ptr->PublicKeyDbBuff;

   /* calculating the required size in bytes */   
   nSizeInBytes = (PubKey_ptr->nSizeInBits + 7) / 8;
   eSizeInBytes = (PubKey_ptr->eSizeInBits + 7) / 8;
   
   /* if the size of the modulus is to small return error */
   if(nSizeInBytes > *ModulusSize_ptr)
     return CRYS_RSA_INVALID_MODULUS_SIZE;
     
   /* if the size of the exponent buffer is to small return error */
   if(eSizeInBytes > *ExponentSize_ptr)
     return CRYS_RSA_INVALID_EXPONENT_SIZE;
     
   /* .............. loading the output arguments and buffers ............... */
   /* ----------------------------------------------------------------------- */ 
     
   /* loading the buffers */

   Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(Exponent_ptr, 4*((*ExponentSize_ptr+3)/4), 
						PubKey_ptr->e, eSizeInBytes );
   if(Error != CRYS_OK)
       return Error;
   
   Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(Modulus_ptr, 4*((*ModulusSize_ptr+3)/4), 
						PubKey_ptr->n, nSizeInBytes );
   if(Error != CRYS_OK)
       return Error;
   
   /* updating the buffer sizes */
   *ModulusSize_ptr  = (DxUint16_t)nSizeInBytes;
   *ExponentSize_ptr = (DxUint16_t)eSizeInBytes;
     
   return CRYS_OK;   
 
   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     
 
}/* END OF CRYS_RSA_Get_PubKey */  

 

/******************************************************************************************
   @brief CRYS_RSA_Get_ModSizeFromPubKey extracts the modulus size from a given public key data structure.
   
   @param[in] UserPubKey_ptr - A pointer to the public key structure, as returned by
                               CRYS_RSA_Build_PubKey.
 
   @param[out] ModulusSize_ptr  -  The actual size of the modulus, in bytes.  
*/
CEXPORT_C CRYSError_t CRYS_RSA_Get_ModSizeFromPubKey(
					CRYS_RSAUserPubKey_t *UserPubKey_ptr,
					DxUint16_t *ModulusSize_ptr)
{	
   /* LOCAL DECLERATIONS */
		
   /* the public key database pointer */
   CRYSRSAPubKey_t *PubKey_ptr;
		                   
   /* FUNCTION DECLERATIONS */

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserPubKey_ptr , ModulusSize_ptr , PubKey_ptr , 
                              PubKey_ptr , PubKey_ptr , PubKey_ptr , 
                              PubKey_ptr , PubKey_ptr , PubKey_ptr,
                              PubKey_ptr , PubKey_ptr , PubKey_ptr , PubKey_ptr ,
                              PubKey_ptr , PubKey_ptr , PubKey_ptr ,
                              PubKey_ptr , PubKey_ptr , PubKey_ptr ,
                              PubKey_ptr , PubKey_ptr , PubKey_ptr ); 
                              
#ifndef CRYS_NO_HASH_SUPPORT                                      
#ifndef CRYS_NO_PKI_SUPPORT                                      
 
   /* ................. checking the validity of the pointer arguments ....... */
   /* ------------------------------------------------------------------------ */
   
   /* ...... checking the key database handle pointer .................... */
   if(UserPubKey_ptr == DX_NULL)
      return CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;
      
   /* checking the modulus size output pointer validity */
   if(ModulusSize_ptr == DX_NULL)
      return CRYS_RSA_INVALID_MOD_BUFFER_SIZE_POINTER;
            
   /* if the users TAG is illegal return an error - the context is invalid */
   if(UserPubKey_ptr->valid_tag != CRYS_RSA_PUB_KEY_VALIDATION_TAG)
      return CRYS_RSA_PUB_KEY_VALIDATION_TAG_ERROR;      
   
#ifndef DX_OEM_FW
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPubKey_ptr, sizeof(CRYS_RSAUserPubKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ModulusSize_ptr, sizeof(DxUint32_t))){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
#endif      
   
      
   /* setting the pointer to the key database */
   PubKey_ptr = (CRYSRSAPubKey_t*)UserPubKey_ptr->PublicKeyDbBuff;

   /* calculating the required size in bytes */   
   *ModulusSize_ptr = (DxUint16_t)((PubKey_ptr->nSizeInBits+7) / 8);
     
   return CRYS_OK;   
  
#endif /* !CRYS_NO_HASH_SUPPORT */
#endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* END OF CRYS_RSA_Get_ModSizeFromPubKey */   
#endif /*!defined(CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)*/

#endif //   #ifndef DX_OEM_FW
