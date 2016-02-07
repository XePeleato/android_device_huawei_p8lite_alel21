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
#include "crys.h"
#include "crys_common.h"
#include "crys_rsa_error.h"
#include "crys_rsa_local.h"
#include "crys_common_math.h"
#include "log.h"
#ifndef CRYS_NO_RSA_SELF_TEST_SUPPORT
#include "crys_self_test_local.h"
#endif

/* .............. LLF level includes ................. */

#if !defined(CRYS_NO_HASH_SUPPORT) && !defined(CRYS_NO_PKI_SUPPORT)
#include "llf_pki_rsa.h"
#include "cc_acl.h"
#endif /* !defined(CRYS_NO_HASH_SUPPORT) && !defined(CRYS_NO_PKI_SUPPORT) */

/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/************************ Enums ************************************/

/************************ Typedefs *********************************/

/************************ Global Data ******************************/

/* 
 For debugging the RSA_KG module define the following flags in project properties
 and perform the following:
   1. Define LLF_PKI_PKA_DEBUG.
   2. For finding the bad random vactors (P,Q,P1pR,P2pR,P1qR,P2qR): 
      define RSA_KG_FIND_BAD_RND flag, perform test and save (from memory) the finded bad vectors.
   3. For repeat the testing of finded bad vectors, write they as HW initialization of
      the following buffers: P=>RSA_KG_debugPvect, Q=>RSA_KG_debugQvect - in the CRYS_RSA_KG.c file, 
      and P1pR=>rBuff1, P2pR=>rBuff1, P1qR=>rBuff3, P2qR=>rBuff4 in the LLF_PKI_GenKeyX931FindPrime.c file.
      Define the flag RSA_KG_NO_RND instead previously defined RSA_KG_FIND_BAD_RND flag and 
      perform the test.
   4. For ordinary ATP or other tests (without debug) undefine all the named flags.      
*/
#ifdef LLF_PKI_PKA_DEBUG
#if (defined RSA_KG_FIND_BAD_RND || defined RSA_KG_NO_RND)
	DxUint8_t   RSA_KG_debugPvect[CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES] =
	{ 0x78,0x71,0xDF,0xC5,0x36,0x98,0x12,0x21,0xCA,0xAC,0x48,0x22,0x01,0x94,0xF7,0x1A,
	  0x1C,0xBF,0x82,0xE9,0x8A,0xE4,0x2C,0x84,0x43,0x46,0xCF,0x6D,0x60,0xFB,0x5B,0xD3}; 
	DxUint8_t   RSA_KG_debugQvect[CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES] =
	{ 0x46,0x13,0x9F,0xBA,0xBC,0x8E,0x21,0x13,0x35,0x8C,0x2C,0x2D,0xA8,0xD6,0x59,0x78,
	  0x8A,0x14,0x17,0x5F,0xA5,0xEC,0x22,0xD5,0x87,0xF9,0x99,0x45,0x1B,0x38,0xA3,0xF0};
#endif
#endif


/************* Private function prototype **************************/


/************************ Public Functions ******************************/


/***********************************************************************************************/
#ifndef _INTERNAL_CRYS_NO_RSA_KG_SUPPORT
/**
   @brief CRYS_RSA_KG_GenerateKeyPair generates a Pair of public and private keys on non CRT mode.
 
   @param[in] PubExp_ptr - The pointer to the public exponent (public key)
   @param[in] PubExpSizeInBytes - The public exponent size in bytes.
   @param[in] KeySize  - The size of the key, in bits. Supported sizes are:
                            - for PKI without PKA HW: all 256 bit multiples between 512 - 2048;
                            - for PKI with PKA: HW all 32 bit multiples between 512 - 2112;
   @param[out] UserPrivKey_ptr - A pointer to the private key structure. 
                           This structure is used as input to the CRYS_RSA_PRIM_Decrypt API.
   @param[out] UserPubKey_ptr - A pointer to the public key structure. 
                           This structure is used as input to the CRYS_RSA_PRIM_Encrypt API.
   @param[in] KeyGenData_ptr - a pointer to a structure required for the KeyGen operation.                       

   @return CRYSError_t - CRYS_OK,
                         CRYS_RSA_INVALID_EXPONENT_POINTER_ERROR,
                         CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                         CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
                         CRYS_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID,
                         CRYS_RSA_INVALID_MODULUS_SIZE,
                         CRYS_RSA_INVALID_EXPONENT_SIZE
*/
CEXPORT_C CRYSError_t CRYS_RSA_KG_GenerateKeyPair(
				DxUint8_t             *PubExp_ptr,
				DxUint16_t             PubExpSizeInBytes, 
				DxUint32_t             KeySize,
				CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
				CRYS_RSAUserPubKey_t  *UserPubKey_ptr,
				CRYS_RSAKGData_t      *KeyGenData_ptr )
{
	/* LOCAL INITIALIZATIONS AND DECLERATIONS */
	
	/* the error identifier */
	CRYSError_t Error;
	
	/* the pointers to the key structures */
	CRYSRSAPubKey_t  *PubKey_ptr;  
	CRYSRSAPrivKey_t *PrivKey_ptr; 
	
	/* a temp definition to solve a problem on release mode on VC++ */
	volatile DxUint32_t dummy = PubExpSizeInBytes;
	
#ifndef CRYS_NO_RSA_SELF_TEST_SUPPORT
	/* Data for Conditional test, after Key - Generation */
	const DxUint8_t Data_ptr[]={'D','i','s','c','r','e','t','i','x'};
	DxUint16_t DataSize=sizeof(Data_ptr);  
#endif
  

	/* FUNCTION LOGIC */
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_RSA_UNSUPPORTED( PubExp_ptr , PubExpSizeInBytes , KeySize , 
                              UserPrivKey_ptr , UserPubKey_ptr , KeyGenData_ptr , 
                              Error , PubKey_ptr , 
                              PrivKey_ptr , dummy , dummy , dummy ,
                              dummy,dummy,dummy,dummy,dummy,dummy,dummy,dummy,dummy,dummy); 
                              
#ifndef CRYS_NO_HASH_SUPPORT                                      
#ifndef CRYS_NO_PKI_SUPPORT                                      

	/* ................ initializaions and local declarations ............ */
	/* ------------------------------------------------------------------- */
	
	/* to avoid compilers warnings */
	dummy = dummy;
	
	/* initialize the error identifier to O.K */
	Error = CRYS_OK;
	
	
	/* ................. checking the validity of the pointer arguments ....... */
	/* ------------------------------------------------------------------------ */
	
	/* ...... checking the key database handle pointer .................... */
	if( PubExp_ptr == DX_NULL )
		return CRYS_RSA_INVALID_EXPONENT_POINTER_ERROR;
	
	/* ...... checking the validity of the exponent pointer ............... */
	if( UserPrivKey_ptr == DX_NULL )
		return CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;
	
	/* ...... checking the validity of the modulus pointer .............. */
	if( UserPubKey_ptr == DX_NULL )
		return CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;
	
	/* ...... checking the validity of the keygen data .................. */
	if( KeyGenData_ptr == DX_NULL )
		return CRYS_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID;
	
	/* ...... checking the exponent size .................. */
	if( PubExpSizeInBytes > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES )
		return CRYS_RSA_INVALID_EXPONENT_SIZE;
	
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, PubExp_ptr, PubExpSizeInBytes) ||
	DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPrivKey_ptr, sizeof(CRYS_RSAUserPrivKey_t)) || 
	DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPubKey_ptr, sizeof(CRYS_RSAUserPubKey_t)) || 
	DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, KeyGenData_ptr, sizeof(CRYS_RSAKGData_t))){
		return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/* ...... checking the required key size ............................ */
	if( ( KeySize < CRYS_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
	( KeySize > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
	( KeySize % CRYS_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS ) )
	
		return CRYS_RSA_INVALID_MODULUS_SIZE;
	
	/* set the public and private key structure pointers */
	PubKey_ptr  = ( CRYSRSAPubKey_t *)UserPubKey_ptr->PublicKeyDbBuff;
	PrivKey_ptr = ( CRYSRSAPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;
	
	
	/* ................ clear all input structures ............................. */
	/* ------------------------------------------------------------------------- */
	
	DX_VOS_MemSet( UserPrivKey_ptr , 0 , sizeof(CRYS_RSAUserPrivKey_t) ); 
	DX_VOS_MemSet( UserPubKey_ptr  , 0 , sizeof(CRYS_RSAUserPubKey_t) ); 
	DX_VOS_MemSet( KeyGenData_ptr  , 0 , sizeof(CRYS_RSAKGData_t) );
	
	/* ................ loading the public exponent to the structure .......... */
	/* ------------------------------------------------------------------------- */
	
	/* loading the buffers to start from LS word to MS word */
	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords( 
					PubKey_ptr->e, sizeof(PubKey_ptr->e), 
					PubExp_ptr, PubExpSizeInBytes);
	if(Error != CRYS_OK)
		return Error;
	       
	/* .......... initializing the effective counters size in bits .......... */
	PubKey_ptr->eSizeInBits = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PubKey_ptr->e, 
										(PubExpSizeInBytes+3)/4 );
	
	/* if the size in bits is 0 - return error */
	if(PubKey_ptr->eSizeInBits == 0 || PubKey_ptr->eSizeInBits > 17){
		Error = CRYS_RSA_INVALID_EXPONENT_SIZE;
			goto End;
	}
	
	/* verifying the exponent has legal value (currently only 0x3,0x11 and 0x10001) */
	if(PubKey_ptr->e[0] != CRYS_RSA_KG_PUB_EXP_ALLOW_VAL_1  &&
	   PubKey_ptr->e[0] != CRYS_RSA_KG_PUB_EXP_ALLOW_VAL_2  &&
	   PubKey_ptr->e[0] != CRYS_RSA_KG_PUB_EXP_ALLOW_VAL_3){
		Error = CRYS_RSA_INVALID_EXPONENT_VAL;
			goto End;
	}
	
	/* .......... initialize the public key on the private structure ............... */

	DX_VOS_FastMemCpy(PrivKey_ptr->PriveKeyDb.NonCrt.e, PubKey_ptr->e, 4*((PubExpSizeInBytes+3)/4));
	PrivKey_ptr->PriveKeyDb.NonCrt.eSizeInBits = PubKey_ptr->eSizeInBits;
	
	/* .......... initializing the key size in bits ......................... */
	
	/* this initialization is required for the low level function (LLF) - indicates the required
	size of the key to be found */
	PubKey_ptr->nSizeInBits  = KeySize;
	PrivKey_ptr->nSizeInBits = KeySize;
	
	/* .......... set the private mode to non CRT .............................. */   
	/* ------------------------------------------------------------------------- */
	
	/* set the mode to non CRT */
	PrivKey_ptr->OperationMode = CRYS_RSA_NoCrt;
	
	/* set the key source as internal */
	PrivKey_ptr->KeySource = CRYS_RSA_InternalKey;
	
	/* ................ executing the key generation ........................... */
	/* ------------------------------------------------------------------------- */
	
	/* on the SSDMA platform it the random generation is executed by the PIC - it is not required */
#if 1 /*LLF_PKI_ENGINE_TYPE != CRYS_DEFS_SSDMA_ENGINE_TYPE */
	
	/* generate the random */
   #if ( (!defined RSA_KG_FIND_BAD_RND && !defined RSA_KG_NO_RND) || defined RSA_KG_FIND_BAD_RND )   
	Error = CRYS_RSA_GenerateVectorInRangeX931(KeySize/(2*32), KeyGenData_ptr->KGData.p);          
	if( Error != CRYS_OK )
		goto End;
	
	Error = CRYS_RSA_GenerateVectorInRangeX931(KeySize/(2*32), KeyGenData_ptr->KGData.q);          
	if( Error != CRYS_OK )
		goto End;
   #endif

   #if defined RSA_KG_FIND_BAD_RND
	DX_VOS_FastMemCpy( RSA_KG_debugPvect, (DxUint8_t*)KeyGenData_ptr->KGData.p, KeySize/(2*8) ); 	 
	DX_VOS_FastMemCpy( RSA_KG_debugQvect, (DxUint8_t*)KeyGenData_ptr->KGData.q, KeySize/(2*8) ); 	 
   #endif    

   #if defined RSA_KG_NO_RND
	DX_VOS_FastMemCpy( (DxUint8_t*)KeyGenData_ptr->KGData.p, RSA_KG_debugPvect, KeySize/(2*8) ); 	 
	DX_VOS_FastMemCpy( (DxUint8_t*)KeyGenData_ptr->KGData.q, RSA_KG_debugQvect, KeySize/(2*8) ); 	 
   #endif    

   #if defined RSA_KG_FIND_BAD_RND || defined RSA_KG_NO_RND   
   #ifdef BIG__ENDIAN
	/* for big endiannes machine reverse bytes order in words according to Big Endian  */
	CRYS_COMMON_INVERSE_UINT32_IN_ARRAY( KeyGenData_ptr->KGData.p, KeySize/(2*32) );
	CRYS_COMMON_INVERSE_UINT32_IN_ARRAY( KeyGenData_ptr->KGData.q, KeySize/(2*32) );
   #endif    
   #endif

	/* clean the n-buffer */
	DX_VOS_MemSetZero( PrivKey_ptr->n, 4*CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS ); 
         
#endif /* LLF_PKI_ENGINE_TYPE != CRYS_DEFS_SSDMA_ENGINE_TYPE */
   
	/* ................ execute the low level keygen ........................... */
	Error = LLF_PKI_RSA_GenerateKeyPair( PubKey_ptr,
					PrivKey_ptr,
					KeyGenData_ptr );
	
	/* on failure exit the function */
	if( Error != CRYS_OK )
		goto End;
	  
	/* ................ initialize the low level key structures ................ */
	/* ------------------------------------------------------------------------- */
	
	Error = LLF_PKI_RSA_InitPubKeyDb( PubKey_ptr );
	
	if( Error != CRYS_OK )
		goto End; 
	
	Error = LLF_PKI_RSA_InitPrivKeyDb( PrivKey_ptr );
	
	if( Error != CRYS_OK )
		goto End; 
	

	/* ............... START : Conditional test for Key-Generation ............. */
	/* ------------------------------------------------------------------------- */

 #ifndef CRYS_NO_RSA_SELF_TEST_SUPPORT  

   	/* Clean and fill the data buffer */
	DX_VOS_MemSet(KeyGenData_ptr->PrimData.DataIn,  0, CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4);
	DX_VOS_MemSet(KeyGenData_ptr->PrimData.DataOut, 0, CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4);
	DX_VOS_FastMemCpy(KeyGenData_ptr->PrimData.DataIn, Data_ptr, DataSize);

	/* Execute the encryption
	**************************/
	Error= LLF_PKI_RSA_ExecPubKeyExp(PubKey_ptr,
					(CRYS_RSAPrimeData_t *)&KeyGenData_ptr->PrimData );	
	if( Error != CRYS_OK )
		goto End; 
	
	/* if the result is the same as the original data set the test as failure */
	if( (DX_VOS_MemCmp( KeyGenData_ptr->PrimData.DataOut, Data_ptr, sizeof(Data_ptr) ))==0 )     
	{
		DX_VOS_MemSet(PubKey_ptr, 0, sizeof(CRYS_RSAUserPubKey_t) );	
		DX_VOS_MemSet(PrivKey_ptr, 0, sizeof(CRYS_RSAUserPrivKey_t) );
	#ifndef CRYS_NO_FIPS_SUPPORT	
		DX_GLOBAL_FIPS_MODE|=DX_CRYS_FIPS_MODE_ERROR_STATE;
	#endif 
		Error = CRYS_RSA_KEY_GEN_CONDITIONAL_TEST_FAIL_ERROR;
			goto End;
	}

	/* Clean and fill the data buffer */
	DX_VOS_FastMemCpy(KeyGenData_ptr->PrimData.DataIn, KeyGenData_ptr->PrimData.DataOut, CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4);
	DX_VOS_MemSet(KeyGenData_ptr->PrimData.DataOut, 0, CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4);	
	
	/* Execute the decryption
	**************************/
	Error = LLF_PKI_RSA_ExecPrivKeyExp(
				PrivKey_ptr,
				(CRYS_RSAPrimeData_t *)&KeyGenData_ptr->PrimData);
	if( Error != CRYS_OK )
		return Error;

	/* if the result is not the same as the original data set the test as failure */
	if(DX_VOS_MemCmp( 
		KeyGenData_ptr->PrimData.DataOut, Data_ptr, DataSize ) )     
	{
		DX_VOS_MemSet(PubKey_ptr, 0, sizeof(CRYS_RSAUserPubKey_t ) );	
		DX_VOS_MemSet(PrivKey_ptr, 0, sizeof(CRYS_RSAUserPrivKey_t ) );
		#ifndef CRYS_NO_FIPS_SUPPORT	
		DX_GLOBAL_FIPS_MODE|=DX_CRYS_FIPS_MODE_ERROR_STATE;
		#endif 
		Error = CRYS_RSA_KEY_GEN_CONDITIONAL_TEST_FAIL_ERROR;
		goto End;
	}		

	 /* Test Passed - return Error = CRYS_OK */

#endif /* CRYS_NO_RSA_SELF_TEST_SUPPORT */

	/* ............... END : Conditional test for Key-Generation ............... */
	/* ------------------------------------------------------------------------- */
	
	
	/* ................ set the key valid tags ................................. */
	/* ------------------------------------------------------------------------- */
	UserPrivKey_ptr->valid_tag = CRYS_RSA_PRIV_KEY_VALIDATION_TAG; 
	UserPubKey_ptr->valid_tag  = CRYS_RSA_PUB_KEY_VALIDATION_TAG; 
	
End:  
	
	/* clear the KG data structure */
	DX_VOS_MemSet ( KeyGenData_ptr , 0 , sizeof(CRYS_RSAKGData_t) );  
		return Error;
	
#endif /* !CRYS_NO_HASH_SUPPORT */
#endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* END OF CRYS_RSA_KG_GenerateKeyPair */ 

                                      
/***********************************************************************************************/
/**
   @brief CRYS_RSA_KG_GenerateKeyPairCRT generates a Pair of public and private keys on CRT mode.

   @param[in] PubExp_ptr - The pointer to the public exponent (public key)
   @param[in] PubExpSizeInBytes - The public exponent size in bits. 
   @param[in] KeySize  - The size of the key, in bits. Supported sizes are:
                            - for PKI without PKA HW: all 256 bit multiples between 512 - 2048;
                            - for PKI with PKA: HW all 32 bit multiples between 512 - 2112;
   @param[out] UserPrivKey_ptr - A pointer to the private key structure. 
                           This structure is used as input to the CRYS_RSA_PRIM_Decrypt API.
   @param[out] UserPubKey_ptr - A pointer to the public key structure. 
                           This structure is used as input to the CRYS_RSA_PRIM_Encryped API.
   @param[in] KeyGenData_ptr - a pointer to a structure required for the KeyGen operation.                       

   @return CRYSError_t - CRYS_OK,
                         CRYS_RSA_INVALID_EXPONENT_POINTER_ERROR,
                         CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                         CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
                         CRYS_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID,
                         CRYS_RSA_INVALID_MODULUS_SIZE,
                         CRYS_RSA_INVALID_EXPONENT_SIZE
*/

CEXPORT_C CRYSError_t CRYS_RSA_KG_GenerateKeyPairCRT(DxUint8_t               *PubExp_ptr,
                                           DxUint16_t               PubExpSizeInBytes, 
                                           DxUint32_t               KeySize,
                                           CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
                                           CRYS_RSAUserPubKey_t  *UserPubKey_ptr,
                                           CRYS_RSAKGData_t      *KeyGenData_ptr )
{
   /* LOCAL INITIALIZATIONS AND DECLERATIONS */
   
   /* the error identifier */
   CRYSError_t Error;
      
   /* the pointers to the key structures */
   CRYSRSAPubKey_t  *PubKey_ptr;  
   CRYSRSAPrivKey_t *PrivKey_ptr; 
   DxUint32_t   pSizeWords; 
 
   /* FUNCTION LOGIC */

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( PubExp_ptr, PubExpSizeInBytes, KeySize, UserPrivKey_ptr, 
                              UserPubKey_ptr, KeyGenData_ptr, PubKey_ptr, PrivKey_ptr, 
                              keySizeWords, Error, Error, Error, Error, Error, Error, 
                              Error, Error, Error, Error, Error, Error, Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      

   /* ............... initializations and local declarations ............ */
   /* ------------------------------------------------------------------- */

   /* initialize the error identifier to O.K */
   Error = CRYS_OK;
   pSizeWords = KeySize / 64;

   /* ........... checking the validity of the pointer arguments ....... */
   /* ------------------------------------------------------------------ */
   
   /* ...... checking the key database handle pointer .................. */
   if( PubExp_ptr == DX_NULL )
      return CRYS_RSA_INVALID_EXPONENT_POINTER_ERROR;
      
   /* ...... checking the validity of the exponent pointer ............. */
   if( UserPrivKey_ptr == DX_NULL )
      return CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;
      
   /* ...... checking the validity of the modulus pointer .............. */
   if( UserPubKey_ptr == DX_NULL )
      return CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;
      
   /* ...... checking the validity of the keygen data .................. */
   if( KeyGenData_ptr == DX_NULL )
      return CRYS_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID;
      
   /* ...... checking the required key size ............................ */
   if( ( KeySize < CRYS_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
       ( KeySize > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
       ( KeySize % CRYS_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS ) ){
	   return CRYS_RSA_INVALID_MODULUS_SIZE;
   }

   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, PubExp_ptr, PubExpSizeInBytes) ||
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPrivKey_ptr, sizeof(CRYS_RSAUserPrivKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPubKey_ptr, sizeof(CRYS_RSAUserPubKey_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, KeyGenData_ptr, sizeof(CRYS_RSAKGData_t))){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }

   /* set the public and private key structure pointers */
   PubKey_ptr  = (CRYSRSAPubKey_t*)UserPubKey_ptr->PublicKeyDbBuff;
   PrivKey_ptr = (CRYSRSAPrivKey_t*)UserPrivKey_ptr->PrivateKeyDbBuff;

   
   /* ................ clear all input structures ............................. */
   /* ------------------------------------------------------------------------- */
   
   DX_VOS_MemSet(UserPrivKey_ptr, 0, sizeof(CRYS_RSAUserPrivKey_t)); 
   DX_VOS_MemSet(UserPubKey_ptr, 0, sizeof(CRYS_RSAUserPubKey_t)); 
   DX_VOS_MemSet(KeyGenData_ptr, 0, sizeof(CRYS_RSAKGData_t));
     
   /* ................ loading the public exponent to the structure .......... */
   /* ------------------------------------------------------------------------- */
   
   /* loading the buffers to start from LS word to MS word */
   Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords( 
				   PubKey_ptr->e, sizeof(PubKey_ptr->e), 
				   PubExp_ptr, PubExpSizeInBytes);
   if(Error != CRYS_OK)
            return Error;
               
   /* .......... initializing the effective counters size in bits .......... */
   PubKey_ptr->eSizeInBits = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PubKey_ptr->e, (PubExpSizeInBytes+3)/4);
 
   /* if the size in bits is 0 - return error */
   if(PubKey_ptr->eSizeInBits == 0 || PubKey_ptr->eSizeInBits > 17){
	   Error = CRYS_RSA_INVALID_EXPONENT_SIZE;
	   goto End;
   }
  
   /* verifing the exponent has legal value (currently only 0x3,0x11 and 0x10001) */
      /* verifying the exponent has legal value (currently only 0x3,0x11 and 0x10001) */
   if(PubKey_ptr->e[0] != CRYS_RSA_KG_PUB_EXP_ALLOW_VAL_1  &&
      PubKey_ptr->e[0] != CRYS_RSA_KG_PUB_EXP_ALLOW_VAL_2 &&
      PubKey_ptr->e[0] != CRYS_RSA_KG_PUB_EXP_ALLOW_VAL_3){
	   Error = CRYS_RSA_INVALID_EXPONENT_VAL;
	   goto End;
   }

   /* .......... initializing the key size in bits ......................... */

   /* this initialization is required for the low level function (LLF) - indicates the required
      size of the key to be found */
   PubKey_ptr->nSizeInBits  = KeySize;
   PrivKey_ptr->nSizeInBits = KeySize;
   
   /* .......... set the private mode to CRT .................................. */   
   /* ------------------------------------------------------------------------- */
   
   /* set the mode to CRT */
   PrivKey_ptr->OperationMode = CRYS_RSA_Crt;
   
   /* set the key source as internal */
   PrivKey_ptr->KeySource = CRYS_RSA_InternalKey;
      
   /* ................ executing the key generation ........................... */
   /* ------------------------------------------------------------------------- */
   
   /* ................ generate the prime1 and prime2 random numbers .......... */

   /* generate the random */
   Error = CRYS_RSA_GenerateVectorInRangeX931(pSizeWords, KeyGenData_ptr->KGData.p);          
   if( Error != CRYS_OK )
	   goto End;
 
   Error = CRYS_RSA_GenerateVectorInRangeX931(pSizeWords, KeyGenData_ptr->KGData.q);          
   if( Error != CRYS_OK )
	   goto End; 
     
   /* ................ execute the low level key gen ........................... */
   Error = LLF_PKI_RSA_GenerateKeyPair(PubKey_ptr,
                                       PrivKey_ptr,
                                       KeyGenData_ptr);
      
   /* on failure exit the function */
   if( Error != CRYS_OK )
	   goto End;
   
   /* ................ set the vector sizes ................................... */
   /* ------------------------------------------------------------------------- */
   
   PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.P, (DxUint16_t)pSizeWords);
   
   PrivKey_ptr->PriveKeyDb.Crt.QSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.Q, (DxUint16_t)pSizeWords);

   PrivKey_ptr->PriveKeyDb.Crt.dPSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.dP, (DxUint16_t)pSizeWords);
   
   PrivKey_ptr->PriveKeyDb.Crt.dQSizeInBits = 
      CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.dQ, (DxUint16_t)pSizeWords);

   PrivKey_ptr->PriveKeyDb.Crt.qInvSizeInBits = 
	   CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.qInv, (DxUint16_t)pSizeWords);

   /* ................ initialize the low level key structures ................ */
   /* ------------------------------------------------------------------------- */
 
   Error = LLF_PKI_RSA_InitPubKeyDb(PubKey_ptr);
   
   if( Error != CRYS_OK )
	   goto End; 
  
   Error = LLF_PKI_RSA_InitPrivKeyDb(PrivKey_ptr);
   
   if( Error != CRYS_OK )
	   goto End; 

   /* ................ set the key valid tags ................................. */
   /* ------------------------------------------------------------------------- */
   
   UserPrivKey_ptr->valid_tag = CRYS_RSA_PRIV_KEY_VALIDATION_TAG; 
   UserPubKey_ptr->valid_tag  = CRYS_RSA_PUB_KEY_VALIDATION_TAG; 
   
End:  
   
   /* clear the KG data structure */
   DX_VOS_MemSet(KeyGenData_ptr, 0, sizeof(CRYS_RSAKGData_t));  
   
   return Error;

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* END OF CRYS_RSA_KG_GenerateKeyPairCRT */



/**********************************************************************************************************/
/** 
 * @brief The CRYS_RSA_GenerateVectorInRangeX931 function generates a random vector in range:
 *            MinVect < RandVect < MaxVect, where:
 *            MinVect = sqwRoot(2) * 2^(RndSizeInBits-1),  MaxVect = 2^RndSizeInBits. 
 *
 *            Note: 1. MSBit of RandVect must be set to 1.
 *                  2. Words order of output vector is set from LS word to MS
 *      	       word. 
 *            
 *        This function is used in PKI RSA for random generation according to ANS X9.31 standard.
 *        If PKI_RSA is not supported, the function does nothing.
 *
 *        Functions algorithm:: 
 *
 *        1.  Calls the CRYS_RND_GenerateVector() function for generating random vector 
 *            RndVect of size RndSizeInWords, rounded up to bytes. Set index i
 *            to high word i = SizeInWords-1.
 *        2.  Check and adust candidate for msWord inside the random vector
 *            starting from msWord himselv, if msWord > high word of MinVect,
 *            goto step 3, else try next word i--; if no words to try, then goto
 *            step 1.
 *        3.  Set the found msWord to high position in array and generate new
 *            random words instead all checked and rejected words.
 *
 * @rndSizeWords[in]  - The size of random vectore that is required.
 * @rnd_ptr[out]      - The output buffer of size not less, than rndSizeWords.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                       value MODULE_* as defined in ...
 */
CRYSError_t CRYS_RSA_GenerateVectorInRangeX931(DxUint32_t   rndSizeWords, 
                                               DxUint32_t  *rnd_ptr)
{
	/* MIN_WORD = rounded up MS word of (2^(32*rndSizeWords-1))*sqwRoot(2)*/
	#define  MIN_VAL 0xB504F334

	/* FUNCTION DECLARATIONS */
	
	CRYSError_t error = CRYS_OK;
	DxUint32_t  msWord;
	DxInt32_t   i;
	DxBool_t    isFound = DX_FALSE;
	
#ifdef CRYS_NO_PKI_SUPPORT  
	/* prevent compiler warnings */
	rndSizeWords = rndSizeWords; rnd_ptr = rnd_ptr; 
#else
	
	/* FUNCTION LOGIC */
	

	/* .........         Rnd generation       .............. */
	/* ----------------------------------------------------- */
	
	while(1) 
	{
		/* Genrate random vector candidate */
		error = CRYS_RND_GenerateVector(
			(DxUint16_t)rndSizeWords*sizeof(DxUint32_t), 
			(DxUint8_t*)rnd_ptr); 
		if(error)    
			goto End;

		/* Find and adust candidate for msWord inside the random *  
		*  vector starting from msWord himselv  		 */

		for(i = rndSizeWords-1; i >= 0; i--) {
			/* Set MSBit to 1 */
			msWord = rnd_ptr[i] | 0x80000000;
			
			if(msWord > MIN_VAL){
				rnd_ptr[rndSizeWords-1] = msWord;
				isFound = 1;
			}
			
			/* Generate new random words instead checked yet for  * 
			*  sequrity goals       			      */
			if((isFound == 1) && (i < (DxInt32_t)rndSizeWords - 1)) {
				error = CRYS_RND_GenerateVector(
					(DxUint16_t)(rndSizeWords - 1 - i)*sizeof(DxUint32_t), 
					(DxUint8_t*)&rnd_ptr[i]); 

				if(error)    
					goto End;
			}

			if(isFound == 1)
				break;
		}

		if(isFound){
			rnd_ptr[0] |= 1UL; /* ensure odd result */
			break;
		}
	}
#endif
	
End:
	return error;	
	
} /* End of CRYS_RSA_GenerateVectorInRangeX931 */

#endif /*_INTERNAL_CRYS_NO_RSA_KG_SUPPORT*/
