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
#include "crys_rsa_error.h"
#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif
#include "crys_common_math.h"
#include "crys_ccm.h"
#include "crys_rsa_local.h"
#include "plat_print_dbg.h"
#include "crys_bypass_api.h"

#ifndef DX_OEM_FW
#include "cc_acl.h"
#endif
/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */
#if ( CRYS_HASH_USER_CTX_SIZE_IN_WORDS > CRYS_PKA_RSA_HASH_CTX_SIZE_IN_WORDS )
#error CRYS_PKA_RSA_HASH_CTX_SIZE_IN_WORDS OR CRYS_HASH_USER_CTX_SIZE_IN_WORDS do not defined correctly.
#endif

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/


/************* Private function prototype ****************/


#ifndef	_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT
/************************ Private Functions ******************************/

/**
   \brief  RSA_VerifyInit initializes the Verify
   multi-call algorithm as defined in PKCS#1 v1.5 and 2.1

	The actual macro that will be used by the user is:
	CRYS_RSA_PKCS1v15_VerifyInit - for v1.5
	CRYS_RSA_PSS_VerifyInit		 - for v2.1
	
   @param[in] UserContext_ptr - A pointer to the public Context
	                           structure of the User.
   @param[in] UserPubKey_ptr - A pointer to the public key data
                           structure.	                           
   @param[in] hashFunc - The hash function to be used. Currently
                         only CRYS_PKCS1_SHA1 and CRYS_PKCS1_MD5
                         are supported. The other hash functions
                         recommended by PKCS#1 v2.1 are
                         SHA-256/284/512. If PKCS1_ver is CRYS_PKCS1_VER15, then
			 the value CRYS_RSA_After_HASH_NOT_KNOWN_mode will allow
			 the signature data to determine the hash function to be used.
   @param[in] MGF - The mask generation function. Only for PKCS#1 v2.1
                    defines MGF1, so the only value allowed for v2.1 
                    is CRYS_PKCS1_MGF1.
   @param[in] SaltLen - The Length of the Salt buffer. relevant for PKCS#1 Ver 2.1 Only
   						Typical lengths are 0 and hLen (20 for SHA1)
   						The maximum length allowed is NSize - hLen - 2
                        If the salt length is not available in this process, the user can use the define:
                        CRYS_RSA_VERIFY_SALT_LENGTH_UNKNOWN
                        Security Note: it is recommended not to use this flag and to support the Salt length on each verify
   @param[in] PKCS1_ver - Ver 1.5 or 2.1, according to the functionality required
   
   @return CRYSError_t - CRYS_OK, CRYS_INVALID_SIGNATURE,
                         CRYS_BAD_PARAM, CRYS_OUT_OF_RANGE 
*/

CEXPORT_C CRYSError_t _DX_RSA_VerifyInit(CRYS_RSAPubUserContext_t *UserContext_ptr,
                               CRYS_RSAUserPubKey_t *UserPubKey_ptr,
                               CRYS_RSA_HASH_OpMode_t hashFunc,
                               CRYS_PKCS1_MGF_t MGF,
                               DxUint16_t SaltLen,			      
                               CRYS_PKCS1_version PKCS1_ver)
{
   /* FUNCTION DECLERATIONS */

   /* The return error identifier */
   CRYSError_t Error;
      
   /* This pointer is allocated in order to remove compiler alias problems */
   void *tempWorkingctx_ptr;
   /* defining a pointer to the active context allcated by the CCM */
   RSAPubContext_t *ccmWorkingContext_ptr ;
         
   /*Pointer to the public key for lengths checking*/
   CRYSRSAPubKey_t *PubKey_ptr;

   /*The size of the modulus for lengths checking*/
   DxUint16_t ModulusSizeBytes;

   /* FUNCTION LOGIC */
   
   /* ............... local initializations .............................. */
   /* -------------------------------------------------------------------- */
   
   /* initializing the Error to O.K */
   Error = CRYS_OK;
   
   /* initialize the module size to cancel compilers warnings */
   ModulusSizeBytes = 0;
    

    /* ............... if not supported exit .............................. */
    /* -------------------------------------------------------------------- */   

    RETURN_IF_RSA_UNSUPPORTED(UserContext_ptr , UserPubKey_ptr , hashFunc , 
                              MGF , SaltLen , PKCS1_ver, ccmWorkingContext_ptr , 
                              PubKey_ptr , ModulusSizeBytes , Error , Error , 
                              Error , Error , Error , Error , Error , Error ,
                              Error , Error , Error , Error , Error); 
                              
    #ifndef CRYS_NO_HASH_SUPPORT                                      
    #ifndef CRYS_NO_PKI_SUPPORT                                      
      
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   
   /* if the users context ID pointer is DX_NULL return an error */
   if( UserContext_ptr == DX_NULL )
   
      return CRYS_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;
      
   /*if the private key object is DX_NULL return an error*/   
   if (UserPubKey_ptr == DX_NULL)
   	  return CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;
      
   /* check if the hash operation mode is legal */
   if( hashFunc >= CRYS_RSA_HASH_NumOfModes)
      return CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
      
   /* check if the MGF operation mode is legal */   
   if(MGF >= CRYS_RSA_NumOfMGFFunctions)
   	  return CRYS_RSA_MGF_ILLEGAL_ARG_ERROR;
   	  
   /* check that the PKCS1 version argument is legal*/
   if(PKCS1_ver >= CRYS_RSA_NumOf_PKCS1_versions)
   	  return CRYS_RSA_PKCS1_VER_ARG_ERROR;
   
   /*According to the PKCS1 ver 2.1 standart it is not recommended to use MD5 hash
   	therefore we do not support it */
   if(PKCS1_ver == CRYS_PKCS1_VER21 && hashFunc == CRYS_RSA_HASH_MD5_mode)
   		return CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
   
   #ifndef DX_OEM_FW
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserContext_ptr, sizeof(CRYS_RSAPubUserContext_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPubKey_ptr, sizeof(CRYS_RSAUserPubKey_t))){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
   #endif      
   
   /*If the validation tag is incorrect*/
   if(UserPubKey_ptr->valid_tag != CRYS_RSA_PUB_KEY_VALIDATION_TAG)
   	  return CRYS_RSA_PUB_KEY_VALIDATION_TAG_ERROR;   
		   
   		   
   /*Checking if a check on salt length is needed*/
   if(SaltLen != CRYS_RSA_VERIFY_SALT_LENGTH_UNKNOWN && PKCS1_ver == CRYS_PKCS1_VER21)
   {
        /*Initializing the Modulus Size in Bytes needed for SaltLength parameter check*/
        PubKey_ptr = (CRYSRSAPubKey_t *)UserPubKey_ptr->PublicKeyDbBuff;

        /*Note: the (-1) is due to the PKCS#1 Ver2.1 standard section 9.1.1*/
        ModulusSizeBytes =  (DxUint16_t)((PubKey_ptr->nSizeInBits -1) / 8);
        if((PubKey_ptr->nSizeInBits -1) % 8)
   		        ModulusSizeBytes++;
   }

   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */   
   
   /* ................. aquiring the RSA context ............................. */
   /* ----------------------------------------------------------------------- */
   
   	  
   /*Get the working context*/
   Error = CRYS_CCM_GetContext( UserContext_ptr,       				 /* the users context space - in */
				&tempWorkingctx_ptr,    /* the CCM context returned - out */
				DX_RSA_VERIFY_CONTEXT,                      /* AES type - in */ 
				AES_DONT_DECRYPT_CONTEXT);			 /* need to decrypt context in AES_block*/   
    
    if(Error != CRYS_OK ) 
	return Error;      
    ccmWorkingContext_ptr = (RSAPubContext_t*)tempWorkingctx_ptr;
   
    

   /*Reset the Context handler for improper previous values initialized*/
   DX_VOS_MemSet(ccmWorkingContext_ptr,0,sizeof(RSAPubContext_t));     
   
	                             
   /* ................. loading the context .................................. */
   /* ------------------------------------------------------------------------ */
 


   /*Initializing the Hash operation mode in the RSA Context level*/
   ccmWorkingContext_ptr->HashOperationMode = hashFunc;
   
   switch(ccmWorkingContext_ptr->HashOperationMode)
   {
   		 case CRYS_RSA_HASH_MD5_mode : 
				 ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_MD5_DIGEST_SIZE_IN_WORDS;	
                                       								   
			     Error = CRYS_HASH_Init( ((CRYS_HASHUserContext_t *)((ccmWorkingContext_ptr->CRYSPKAHashCtxBuff))),
	              		              CRYS_HASH_MD5_mode);		/*The MD5 mode according to the Hash level enum*/
	                           
			    if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      										 						
			   		
				break;
				
       case CRYS_RSA_HASH_SHA1_mode: 
            ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA1_DIGEST_SIZE_IN_WORDS;
       	    Error = CRYS_HASH_Init( ((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff))	,
	                    		       CRYS_HASH_SHA1_mode);	/*The SHA1 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			   		
			 	  break;
			 	  
	   case CRYS_RSA_HASH_SHA224_mode: 
            ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA224_DIGEST_SIZE_IN_WORDS;
       	    Error = CRYS_HASH_Init( ((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA224_mode);	/*The SHA224 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			   		
			 	  break;
		
	   case CRYS_RSA_HASH_SHA256_mode: 
            ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA256_DIGEST_SIZE_IN_WORDS;
       	    Error = CRYS_HASH_Init( ((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA256_mode);	/*The SHA256 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			   		
			 	  break;
			 	  
	   case CRYS_RSA_HASH_SHA384_mode: 
            ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA384_DIGEST_SIZE_IN_WORDS;
       	    Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA384_mode);	/*The SHA384 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			   		
			 	  break;
			 	  
		case CRYS_RSA_HASH_SHA512_mode: 
            ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA512_DIGEST_SIZE_IN_WORDS;
       	    Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA512_mode);	/*The SHA512 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			   		
			 	break;	  
			 	 	  
       case CRYS_RSA_After_MD5_mode: /*For PKCS1 v1.5 when the data in is already hashed with MD5*/
          ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_MD5_DIGEST_SIZE_IN_WORDS;
          break;	
			 	
       case CRYS_RSA_After_SHA1_mode: /*For PKCS1 v1.5 when the data in is already hashed with SHA1*/
          ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA1_DIGEST_SIZE_IN_WORDS;
          break;
          
       case CRYS_RSA_After_SHA224_mode: /*For PKCS1 v1.5 when the data in is already hashed with SHA224*/
          ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA224_DIGEST_SIZE_IN_WORDS;
          break;
          
       case CRYS_RSA_After_SHA256_mode: /*For PKCS1 v1.5 when the data in is already hashed with SHA256*/
          ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA256_DIGEST_SIZE_IN_WORDS;
          break;
       
       case CRYS_RSA_After_SHA384_mode: /*For PKCS1 v1.5 when the data in is already hashed with SHA384*/
          ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA384_DIGEST_SIZE_IN_WORDS;
          break;
          
       case CRYS_RSA_After_SHA512_mode: /*For PKCS1 v1.5 when the data in is already hashed with SHA512*/
          ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA512_DIGEST_SIZE_IN_WORDS;
          break;	
			 				 	
       case CRYS_RSA_HASH_NO_HASH_mode:	/*Used for PKCS1 v1.5 Encrypt and Decrypt - not relevant in Sign-Verify*/

		   break;	/*do nothing*/		 						

	   case CRYS_RSA_After_HASH_NOT_KNOWN_mode : /*used for PKCS1 v1.5 Verify only - possible to derive the hash mode from the signature*/
           
		   if( PKCS1_ver == CRYS_PKCS1_VER21)
		   {
			   Error = CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
			   goto END_WITH_ERROR;
		   }
		   
		   break;	/*do nothing*/		 						
         		
	   default: 
	 		Error = CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
	 		goto END_WITH_ERROR;
	}   

   switch(PKCS1_ver)
   {
   		case CRYS_PKCS1_VER15: 
   		
   		    ccmWorkingContext_ptr->PKCS1_Version=CRYS_PKCS1_VER15;
          break;
          
   		case CRYS_PKCS1_VER21: 
   		
   		    ccmWorkingContext_ptr->PKCS1_Version=CRYS_PKCS1_VER21;
                  
          /*Checking restriction of Salt Length ; Hash output size and the mosulus*/
          if(SaltLen != CRYS_RSA_VERIFY_SALT_LENGTH_UNKNOWN &&
             ModulusSizeBytes < (DxUint32_t)(ccmWorkingContext_ptr->HASH_Result_Size*4 + SaltLen + 2))
             {
                 Error = CRYS_RSA_PSS_ENCODING_MODULUS_HASH_SALT_LENGTHS_ERROR; 
                 goto END_WITH_ERROR;
             }
             break;
             
      default: 
								
             Error = CRYS_RSA_PKCS1_VER_ARG_ERROR; 
             goto END_WITH_ERROR;
   }

   switch(MGF)
   {
   		case CRYS_PKCS1_MGF1:
		case CRYS_PKCS1_NO_MGF:
   				ccmWorkingContext_ptr->MGF_2use = MGF;
				break;   									
   		default:
   				Error = CRYS_RSA_MGF_ILLEGAL_ARG_ERROR;
   				goto END_WITH_ERROR;
   }
   
   /*Copying the RSA Pub key argument to the context*/    
   DX_VOS_FastMemCpy((DxUint8_t *)&ccmWorkingContext_ptr->PubUserKey,(DxUint8_t *)UserPubKey_ptr,sizeof(CRYS_RSAUserPubKey_t));         
   
   /*Initial the Salt random length relevant for PKCS#1 Ver2.1*/
   ccmWorkingContext_ptr->SaltLen = SaltLen;
   
   /* Initialize the size of the modulus */
   ccmWorkingContext_ptr->nSizeInBytes = (((CRYSRSAPubKey_t*)UserPubKey_ptr->PublicKeyDbBuff)->nSizeInBits+7)/8;
      
   /* set the RSA tag to the users context */
   UserContext_ptr->valid_tag = CRYS_RSA_VERIFY_CONTEXT_VALIDATION_TAG;   
   
   goto END;
   
   END:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   
   Error = CRYS_CCM_ReleaseContext(UserContext_ptr,		/* the users context space - in */
                           ccmWorkingContext_ptr,       /* the CCM context returned - in */
                           DX_RSA_VERIFY_CONTEXT);      /* RSA type - in */ 
   
   return Error;
	
   END_WITH_ERROR:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   {   
   		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(UserContext_ptr,			/* the users context space - in */
						                           ccmWorkingContext_ptr,       /* the CCM context returned - in */
						                           DX_RSA_VERIFY_CONTEXT);      /* RSA type - in */ 
						                           
	
		if(Error1 != CRYS_OK)
        {
			PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" ,Error1));                              
        }
   }      

   /* .............. clearing the users context in case of error.......... */
   /* -------------------------------------------------------------------- */                           
   DX_VOS_MemSet(UserContext_ptr,0,sizeof(CRYS_RSAPubUserContext_t));
   
   return Error;
   
   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     
	
}/* END OF _DX_RSA_VerifyInit */  				  


/**********************************************************************************************************/
/**
   \brief RSA_VerifyUpdate processes the data to be verified
   in a given context, according to PKCS1 v1.5 and 2.1

   \note RSA_VerifyUpdate can be called multiple times with data
   
	The actual macro that will be used by the user is:
	CRYS_RSA_PKCS1v15_VerifyUpdate		- for v1.5
	CRYS_RSA_PSS_VerifyUpdate			- for v2.1
	
   @param[in] UserContext_ptr - A pointer to the public Context
	                           structure of the User.
   @param[in] DataIn_ptr - A pointer to the data whose signature is
                         to be verified.
   @param[in] DataInSize - The size, in bytes, of the data whose
                         signature is to be verified.

   @return CRYSError_t - CRYS_OK, CRYS_INVALID_SIGNATURE,
                         CRYS_BAD_PARAM, CRYS_OUT_OF_RANGE 
*/

CEXPORT_C CRYSError_t _DX_RSA_VerifyUpdate(CRYS_RSAPubUserContext_t *UserContext_ptr,
                                 DxUint8_t     *DataIn_ptr,
                                 DxUint32_t     DataInSize)
{
   /* FUNCTION DECLERATIONS */
   
   /* The return error identifier */
   CRYSError_t Error;

   /* This pointer is allocated in order to remove compiler alias problems */
   void *tempWorkingctx_ptr;
   /* defining a pointer to the active context allcated by the CCM */
   RSAPubContext_t *ccmWorkingContext_ptr;
   
   /* FUNCTION LOGIC */
   
   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */   
   
   /* initializing the Error to O.K */
   Error = CRYS_OK;      
 
   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserContext_ptr , DataIn_ptr , DataInSize , 
                              ccmWorkingContext_ptr , Error , Error, Error , 
                              Error , Error , 
                              Error , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
  
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   /* if the users context pointer is DX_NULL return an error */
   if( UserContext_ptr == DX_NULL )
      return CRYS_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;
   
   /* if the users Data In pointer is illegal and the data size is not 0 return an error */
   if( DataIn_ptr == DX_NULL && DataInSize )
      return CRYS_RSA_DATA_POINTER_INVALID_ERROR;
           
   /* if the data size is larger then 2^29 (to prevant an overflow on the transition to bits ) 
      return error */
   if( DataInSize >= (1UL << 29) )
      return CRYS_RSA_INVALID_MESSAGE_DATA_SIZE; 
	  
   #ifndef DX_OEM_FW
   /* DataIn can be smart pointer but it is tested either in the hash or in bypass operations */
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserContext_ptr, sizeof(CRYS_RSAPubUserContext_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, DataIn_ptr, DataInSize)){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
   #endif
   	
   /* if the users context TAG is illegal return an error - the context is invalid */
   if( UserContext_ptr->valid_tag != CRYS_RSA_VERIFY_CONTEXT_VALIDATION_TAG )
      return CRYS_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR;   
     
   /* ................. aquiring the RSA context ............................. */
   /* ------------------------------------------------------------------------ */
   
   Error = CRYS_CCM_GetContext( UserContext_ptr,       				 /* the users context space - in */
                                (void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
                                DX_RSA_VERIFY_CONTEXT,                 
                                AES_DECRYPT_CONTEXT);			 	 /* need to decrypt context in AES_block*/   
          
    if( Error != CRYS_OK ) 
     return Error;                                          
   ccmWorkingContext_ptr = (RSAPubContext_t*)tempWorkingctx_ptr;
    
   switch(ccmWorkingContext_ptr->HashOperationMode)
   {    
   /* The mode is either SHA1 or MD5 */
		case CRYS_RSA_HASH_SHA1_mode:
		case CRYS_RSA_HASH_MD5_mode:
   		case CRYS_RSA_HASH_SHA224_mode:
		case CRYS_RSA_HASH_SHA256_mode:
		case CRYS_RSA_HASH_SHA384_mode:
		case CRYS_RSA_HASH_SHA512_mode:
		
				/*Operate the Hash update function for relevant versions*/   			
				Error=CRYS_HASH_Update(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
							 			            DataIn_ptr,
										            DataInSize );
											
				if(Error != CRYS_OK)
				 	goto END_WITH_ERROR;   			
				 	
   				break;

		case CRYS_RSA_After_SHA1_mode:
		
		   		if(DataInSize != CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES)
		   		{	/* DataInSize must fit exactly to the size of Hash output that we support */
		   			Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE;
	   				goto END_WITH_ERROR;
	   			}
	   			
  			        /* Copy the DataIn_ptr to the HashResult in case it is an SSL mode*/	   		
				Error = CRYS_Bypass(DataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
				if (Error != CRYS_OK)
					goto END_WITH_ERROR; 
		   		break;
		
		case CRYS_RSA_After_SHA224_mode:
		
		   		if(DataInSize != CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES)
		   		{	/* DataInSize must fit exactly to the size of Hash output that we support */
		   			Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE;
	   				goto END_WITH_ERROR;
	   			}
	   			
			        /* Copy the DataIn_ptr to the HashResult in case it is an SSL mode*/	   		
				Error = CRYS_Bypass(DataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
				if (Error != CRYS_OK)
					goto END_WITH_ERROR; 
		   		break;
		   		
		case CRYS_RSA_After_SHA256_mode:
		
		   		if(DataInSize != CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES)
		   		{	/* DataInSize must fit exactly to the size of Hash output that we support */
		   			Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE;
	   				goto END_WITH_ERROR;
	   			}
	   			
			        /* Copy the DataIn_ptr to the HashResult in case it is an SSL mode*/	   		
				Error = CRYS_Bypass(DataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
				if (Error != CRYS_OK)
					goto END_WITH_ERROR; 
		   		break;
		   		
		case CRYS_RSA_After_SHA384_mode:
		
		   		if(DataInSize != CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES)
		   		{	/* DataInSize must fit exactly to the size of Hash output that we support */
		   			Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE;
	   				goto END_WITH_ERROR;
	   			}
	   			
			        /* Copy the DataIn_ptr to the HashResult in case it is an SSL mode*/	   		      
				Error = CRYS_Bypass(DataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
				if (Error != CRYS_OK)
					goto END_WITH_ERROR; 
		   		break;
		
		case CRYS_RSA_After_SHA512_mode:
		
		   		if(DataInSize != CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES)
		   		{	/* DataInSize must fit exactly to the size of Hash output that we support */
		   			Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE;
	   				goto END_WITH_ERROR;
	   			}
	   			
			        /* Copy the DataIn_ptr to the HashResult in case it is an SSL mode*/	   		
				Error = CRYS_Bypass(DataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
				if (Error != CRYS_OK)
					goto END_WITH_ERROR; 
		   		break;
		   		   		
		case CRYS_RSA_After_MD5_mode:
				   		
		   		if(DataInSize != CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES)
		   		{	/* DataInSize must fit exactly to the size of Hash output that we support */
		   			Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE;
	   				goto END_WITH_ERROR;
	   			}
	   			
			        /* Copy the DataIn_ptr to the HashResult in case it is an SSL mode*/	   		
				Error = CRYS_Bypass(DataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
				if (Error != CRYS_OK)
					goto END_WITH_ERROR; 
		   		break;		   		
		   		
		case CRYS_RSA_HASH_NO_HASH_mode:
				/*Do nothing*/
				break;
   
		case CRYS_RSA_After_HASH_NOT_KNOWN_mode : /*used for PKCS1 v1.5 Verify only - possible to derive the hash mode from the signature*/

			    if( DataInSize > CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES ||
					DataInSize < CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES )
				{	
					/* DataInSize must fit exactly to the size of Hash output that we support */
					Error = CRYS_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE;
					goto END_WITH_ERROR;
				}
				/* Copy the DataIn_ptr to the HashResult in case it is an SSL mode*/	
				Error = CRYS_Bypass(DataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
				if (Error != CRYS_OK)
					goto END_WITH_ERROR; 
		                /* set message size */
				ccmWorkingContext_ptr->HASH_Result_Size = (DxUint16_t)DataInSize / sizeof(DxUint32_t);				
				break;		   		
				
		default:
		 		Error = CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
		 		goto END_WITH_ERROR;
   }
   
   goto END;
   
   END:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   
   Error = CRYS_CCM_ReleaseContext(UserContext_ptr,		/* the users context space - in */
                                   ccmWorkingContext_ptr,       /* the CCM context returned - in */
                                   DX_RSA_VERIFY_CONTEXT);      /* RSA type - in */ 
   
   return Error;
	
   END_WITH_ERROR:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   {   
   		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(UserContext_ptr,			        /* the users context space - in */
                                                   ccmWorkingContext_ptr,       /* the CCM context returned - in */
                                                   DX_RSA_VERIFY_CONTEXT);      /* RSA type - in */ 
  
       
		if(Error1 != CRYS_OK)
        {
    		PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" ,Error1));                              
        }
   }      
                           
   /* .............. clearing the users context in case of error.......... */
   /* -------------------------------------------------------------------- */                           
   DX_VOS_MemSet(UserContext_ptr,0,sizeof(CRYS_RSAPubUserContext_t));
   
   return Error;	

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* END OF _DX_RSA_VerifyUpdate */			      		
			      

/**********************************************************************************************************/
/**

   \brief RSA_VerifyFinish implements the Finish Phase of the Verify algorithm
   as defined in PKCS#1 v2.1 or PKCS#1 v1.5

	The actual macro that will be used by the user is:
	CRYS_RSA_PKCS1v15_VerifyFinish - for v1.5
	CRYS_RSA_PSS_VerifyFinish	   - for v2.1
	
   @param[in] UserContext_ptr - A pointer to the public Context
	                           structure of the User.
   @param[in] Sig_ptr - A pointer to the signature to be verified. 
                        The length of the signature is PubKey_ptr->N.len bytes 
                        (that is, the size of the modulus, in bytes).

   @return CRYSError_t - CRYS_OK, CRYS_INVALID_SIGNATURE,
                         CRYS_BAD_PARAM, CRYS_OUT_OF_RANGE 
*/

CEXPORT_C CRYSError_t _DX_RSA_VerifyFinish(CRYS_RSAPubUserContext_t *UserContext_ptr,
                                 DxUint8_t *Sig_ptr)			      
{
   /* FUNCTION DECLERATIONS */
   
	/* The return error identifier */
   CRYSError_t Error;

   /* This pointer is allocated in order to remove compiler alias problems */
   void *tempWorkingctx_ptr;

   /* defining a pointer to the active context allcated by the CCM */
   RSAPubContext_t *ccmWorkingContext_ptr ;

   /*Pointer to the buffer inside the context for manipulation of Data*/
   DxUint8_t   *ED_ptr;
   
   /*Parameter for the new size of the modulus N in bytes according to PKCS1 Ver 2.1*/
   DxUint16_t PrvNNewSizeBytes;/*rounded number of Bytes for padding2 length*/
	
   /*Temporary for the N size*/
   CRYSRSAPubKey_t *PubKey_ptr;
   
   /*Pointers to the Decrypted Data and Size*/
   DxUint8_t *D_ptr;
   DxUint16_t *DSize_ptr;
	
   /*The local variables for PKCS#1 Ver 1.5*/
   BerParserObj_t BerParserObj;
    	
   /* FUNCTION LOGIC */
   
   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */   
   
   /* initializing the Error to O.K */
   Error = CRYS_OK;      

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

    RETURN_IF_RSA_UNSUPPORTED( UserContext_ptr , Sig_ptr , ccmWorkingContext_ptr , 
                              ED_ptr , PrvNNewSizeBytes , PubKey_ptr, D_ptr , 
                              DSize_ptr , BerParserObj.DigestAlg , 
                              Error , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
    #ifndef CRYS_NO_HASH_SUPPORT                                      
    #ifndef CRYS_NO_PKI_SUPPORT                                      
   
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   /* if the users context pointer is DX_NULL return an error */
   if( UserContext_ptr == DX_NULL )
      return CRYS_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;          
   
   /* if the users context pointer is DX_NULL return an error */
   if( Sig_ptr == DX_NULL )
      return CRYS_RSA_INVALID_SIGNATURE_BUFFER_POINTER;

   #ifndef DX_OEM_FW
   /* sizeof signature need to be fixed */
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserContext_ptr, sizeof(CRYS_RSAPubUserContext_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, Sig_ptr, sizeof(DxUint32_t))){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
   #endif

   /* if the users context TAG is illegal return an error - the context is invalid */
   if( UserContext_ptr->valid_tag != CRYS_RSA_VERIFY_CONTEXT_VALIDATION_TAG )
      return CRYS_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR;   

   /* ................. aquiring the RSA context ............................. */
   /* ------------------------------------------------------------------------ */
   
   Error = CRYS_CCM_GetContext( UserContext_ptr,       				 /* the users context space - in */
                                (void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
                                DX_RSA_VERIFY_CONTEXT,                 
                                AES_DECRYPT_CONTEXT);			 	 /* need to decrypt context in AES_block*/   
    
    if( Error != CRYS_OK ) 
     return Error;                                       
   ccmWorkingContext_ptr = (RSAPubContext_t*)tempWorkingctx_ptr;
    
   ED_ptr = (DxUint8_t *)ccmWorkingContext_ptr->EBD;
   
   PubKey_ptr = (CRYSRSAPubKey_t *)ccmWorkingContext_ptr->PubUserKey.PublicKeyDbBuff;
   
   PrvNNewSizeBytes = (DxUint16_t)(PubKey_ptr->nSizeInBits/8);

  	/*rounding */   
   if(((PubKey_ptr->nSizeInBits) % 8) != 0)
    	PrvNNewSizeBytes++;
    	
   D_ptr = ccmWorkingContext_ptr->T_Buf;
   DSize_ptr = &ccmWorkingContext_ptr->T_BufSize;
   *DSize_ptr = sizeof(ccmWorkingContext_ptr->T_Buf);
   
   PLAT_LOG_DEV_PRINT(( 10 ," \n In RSA_VerifyFinish Before PRIM_Encrypt operation:\n" ));        
   PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nModulus N =\n" ,(DxUint8_t *)PubKey_ptr->n , PubKey_ptr->nSizeInBits/8));   	    
   PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nExp e =\n" ,(DxUint8_t *)PubKey_ptr->e , PubKey_ptr->eSizeInBits/8));   	        
      
	/* execute the expomnent - after correct building it is working with either CRT or PAIR mode*/ 
    Error = CRYS_RSA_PRIM_Encrypt(&ccmWorkingContext_ptr->PubUserKey,
                                  &ccmWorkingContext_ptr->PrimeData,
				                  Sig_ptr,
				                  PrvNNewSizeBytes,
				                  ED_ptr);
        
    if( Error != CRYS_OK ) 
     	goto END_WITH_ERROR;               

    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nSig in =\n" ,(DxUint8_t *)Sig_ptr , PrvNNewSizeBytes));   	    
    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nIn RSA_VerifyFinish after CRYS_RSA_PRIM_Encrypt ; Output_ptr = " ,(DxUint8_t*)ED_ptr ,PrvNNewSizeBytes));				                  

	/*Initialize the Effective size in bits of the result*/
	ccmWorkingContext_ptr->EBDSizeInBits = CRYS_COMMON_GetBytesCounterEffectiveSizeInBits( ED_ptr,PrvNNewSizeBytes ); 			                        

   /*Operating the HASH Finish function only in case that Hash operation is needed*/
   if(ccmWorkingContext_ptr->HashOperationMode <= CRYS_RSA_HASH_SHA512_mode)
   	{
	   /*Operating the HASH Finish function*/
	   Error=CRYS_HASH_Finish(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
		                      ccmWorkingContext_ptr->HASH_Result);   		    
   	}		                      
	                      
   if( Error != CRYS_OK ) 
     goto END_WITH_ERROR;         

   switch(ccmWorkingContext_ptr->PKCS1_Version)
   {
#ifndef _INTERNAL_CRYS_NO_RSA_SCHEME_21_SUPPORT
	   case CRYS_PKCS1_VER21:
	      /*Operating the Verify primitive*/					                           
			Error = CRYS_RSA_PSS_Verify21(ccmWorkingContext_ptr);      

            if(Error!=CRYS_OK)
           		goto END_WITH_ERROR;
           		
			break;
#endif	   						
#ifndef _INTERNAL_CRYS_NO_RSA_SCHEME_15_SUPPORT
            case CRYS_PKCS1_VER15:	

			Error = CRYS_RSA_PKCS1_v1_5_Decode(ED_ptr,
												PrvNNewSizeBytes,
												D_ptr,
												DSize_ptr);
            if(Error!=CRYS_OK)
           		goto END_WITH_ERROR;
           			   						
		  	PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \n In RSA_VerifyFinish ; After CRYS_RSA_PKCS1_v1_5_Decode ; D_ptr = " ,(DxUint8_t *)D_ptr ,*DSize_ptr));		               
		   
            /*Space for the Ber Parser output*/
		    BerParserObj.MessageDigest_ptr=ED_ptr;
				
		    /***********************
		     * Call the BER Parser - it can derive the hash mode from the input signature
		     ***********************/
		    Error=CRYS_RSA_BER_BerParser(   D_ptr, 
                                      		*DSize_ptr,
		                          			&BerParserObj);
		    if(Error!=CRYS_OK)
			   	goto END_WITH_ERROR;                     				 

				 		   /* setting the ber parser digest algorithm according to the selected HASH algorithm */
            switch(ccmWorkingContext_ptr->HashOperationMode)
			{
 		   		case CRYS_RSA_HASH_SHA1_mode:
 		   		case CRYS_RSA_After_SHA1_mode:	 		   
                    if(BerParserObj.DigestAlg != (DxUint8_t)CRYS_HASH_SHA1_mode)
                    {
                        /*check that the hash operation mode is consistent with the hash that was derived in the Ber encoder*/
                        Error = CRYS_RSA_PKCS15_VERIFY_BER_ENCODING_HASH_TYPE;
                        goto END_WITH_ERROR;
                    }
			 		break;
			 		
			 	case CRYS_RSA_HASH_SHA224_mode:
 		   		case CRYS_RSA_After_SHA224_mode:	 		   
                    if(BerParserObj.DigestAlg != (DxUint8_t)CRYS_HASH_SHA224_mode)
                    {
                        /*check that the hash operation mode is consistent with the hash that was derived in the Ber encoder*/
                        Error = CRYS_RSA_PKCS15_VERIFY_BER_ENCODING_HASH_TYPE;
                        goto END_WITH_ERROR;
                    }
			 		break;
			 
			 	case CRYS_RSA_HASH_SHA256_mode:
 		   		case CRYS_RSA_After_SHA256_mode:	 		   
                    if(BerParserObj.DigestAlg != (DxUint8_t)CRYS_HASH_SHA256_mode)
                    {
                        /*check that the hash operation mode is consistent with the hash that was derived in the Ber encoder*/
                        Error = CRYS_RSA_PKCS15_VERIFY_BER_ENCODING_HASH_TYPE;
                        goto END_WITH_ERROR;
                    }
			 		break;
			 		
			 	case CRYS_RSA_HASH_SHA384_mode:
 		   		case CRYS_RSA_After_SHA384_mode:	 		   
                    if(BerParserObj.DigestAlg != (DxUint8_t)CRYS_HASH_SHA384_mode)
                    {
                        /*check that the hash operation mode is consistent with the hash that was derived in the Ber encoder*/
                        Error = CRYS_RSA_PKCS15_VERIFY_BER_ENCODING_HASH_TYPE;
                        goto END_WITH_ERROR;
                    }
			 		break;		
			 		
			 	case CRYS_RSA_HASH_SHA512_mode:
 		   		case CRYS_RSA_After_SHA512_mode:	 		   
                    if(BerParserObj.DigestAlg != (DxUint8_t)CRYS_HASH_SHA512_mode)
                    {
                        /*check that the hash operation mode is consistent with the hash that was derived in the Ber encoder*/
                        Error = CRYS_RSA_PKCS15_VERIFY_BER_ENCODING_HASH_TYPE;
                        goto END_WITH_ERROR;
                    }
			 		break;
			 	
				 		      			
 		   		case CRYS_RSA_HASH_MD5_mode:
 		   		case CRYS_RSA_After_MD5_mode:	 		   
				 		   		
                    if(BerParserObj.DigestAlg != (DxUint8_t)CRYS_HASH_MD5_mode)
                    {
                        /*check that the hash operation mode is consistent with the hash that was derived in the Ber encoder*/
                        Error = CRYS_RSA_PKCS15_VERIFY_BER_ENCODING_HASH_TYPE;
			 		    goto END_WITH_ERROR;
                    }
			 		break;
			 		
                case CRYS_RSA_After_HASH_NOT_KNOWN_mode : /*used for PKCS1 v1.5 Verify only - possible to derive the hash mode from the signature*/
                        /*do nothing ==> the BER encoder already derived the correct hash function type*/
                        break;

				case CRYS_RSA_HASH_NO_HASH_mode:
 		      	default:
 		      			Error = CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
 		      			goto END_WITH_ERROR;
            }		   				   
		   				   
	        if(DX_VOS_MemCmp(&BerParserObj.MessageDigest_ptr[0],(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result,ccmWorkingContext_ptr->HASH_Result_Size*4))/*The result Size is in words*/
	        {
	           Error = CRYS_RSA_ERROR_VER15_INCONSISTENT_VERIFY;
	           goto END_WITH_ERROR;     
	        }
						     
			break;
#endif
    default: 
			Error = CRYS_RSA_PKCS1_VER_ARG_ERROR;
			goto END_WITH_ERROR;

   }/*End of switch()*/
	    
   goto END;
   
   END:	/*On finish operation need to memset the context to zero so END and END_WITH_ERROR are the same*/
   END_WITH_ERROR:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   {   
   		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(UserContext_ptr,			/* the users context space - in */
						                           ccmWorkingContext_ptr,       /* the CCM context returned - in */
						                           DX_RSA_VERIFY_CONTEXT);      /* RSA type - in */ 
 
		if(Error1 != CRYS_OK)
        {
    		PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" ,Error1));                              
        }
   }   
                           
   /* .............. clearing the users context in case of error.......... */
   /* -------------------------------------------------------------------- */                           
   DX_VOS_MemSet(UserContext_ptr,0,sizeof(CRYS_RSAPubUserContext_t));
   
   return Error;
   
   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     
	
}/* END OF _DX_RSA_VerifyFinish */			      


/**********************************************************************************************************/
/**
   \brief RSA_Verify implements the RSASSA-PKCS1v15 algorithm
    in a single function, as defined in PKCS#1 v1.5 and v2.1

	The actual macro that will be used by the users is:
	CRYS_RSA_PKCS1v15_Verify			- for v1.5
	CRYS_RSA_PKCS1v15_MD5_Verify		- for v1.5 data in is hashed with MD5
	CRYS_RSA_PKCS1v15_SHA1_Verify		- for v1.5 data in is hashed with SHA1
	CRYS_RSA_PSS_Verify					- for v2.1
	CRYS_RSA_PSS_SHA1_Verify			- for v2.1 data in is hashed with SHA1
							- for v2.1 MD5 is not supported, since
							according to the PKCS#1 ver2.1 it is not recommended	
	
   @param[in] UserContext_ptr - A pointer to the public Context, 
   				for the use of the function as a space to work on
   @param[in] UserPubKey_ptr - A pointer to the public key data
                           structure of the user.
   @param[in] hashFunc - The hash function to be used. 
                         Currently only CRYS_PKCS1_SHA1 and CRYS_PKCS1_MD5
                         are supported. The other hash functions
                         recommended by PKCS#1 v2.1 are
                         SHA-256/284/512.  If PKCS1_ver is CRYS_PKCS1_VER15, then
			 the value CRYS_RSA_After_HASH_NOT_KNOWN_mode will allow
			 the signature data to determine the hash function to be used.
   @param[in] MGF - The mask generation function. only for PKCS#1 v2.1
                    defines MGF1, so the only value allowed for v2.1 
                    is CRYS_PKCS1_MGF1. 
   @param[in] SaltLen - The Length of the Salt buffer. relevant for PKCS#1 Ver 2.1 Only
   						Typical lengths are 0 and hLen (20 for SHA1)
   						The maximum length allowed is NSize - hLen - 2                                             
   @param[in] DataIn_ptr - A pointer to the data whose signature is
                         to be verified.
   @param[in] DataInSize - The size, in bytes, of the data whose
                         signature is to be verified.
   @param[in] Sig_ptr - A pointer to the signature to be verified. 
                        The length of the signature is PubKey_ptr->N.len bytes 
                        (that is, the size of the modulus, in bytes).
   @param[in] PKCS1_ver - Ver 1.5 or 2.1, according to the functionality required

   @return CRYSError_t - CRYS_OK, CRYS_INVALID_SIGNATURE,
                         CRYS_BAD_PARAM, CRYS_OUT_OF_RANGE 
*/
				     
CEXPORT_C CRYSError_t _DX_RSA_Verify(CRYS_RSAPubUserContext_t *UserContext_ptr,
                           CRYS_RSAUserPubKey_t *UserPubKey_ptr,
                           CRYS_RSA_HASH_OpMode_t hashFunc,
                           CRYS_PKCS1_MGF_t MGF,
                           DxUint16_t SaltLen,
                           DxUint8_t     *DataIn_ptr,
                           DxUint32_t     DataInSize,
                           DxUint8_t     *Sig_ptr,
                           CRYS_PKCS1_version PKCS1_ver)
{
    /* FUNCTION DECLERATIONS */
   
    /* The return error identifier */
    CRYSError_t Error;

    /* FUNCTION LOGIC */
   
    /* .................. initializing local variables ................... */
    /* ------------------------------------------------------------------- */   
   
    /* initializing the Error to O.K */
    Error = CRYS_OK;      
  
    /* ............... if not supported exit .............................. */
    /* -------------------------------------------------------------------- */   
 
   RETURN_IF_RSA_UNSUPPORTED( UserContext_ptr , UserPubKey_ptr , hashFunc , 
                              MGF , SaltLen , DataIn_ptr, DataInSize , 
                              Sig_ptr , PKCS1_ver , 
                              Error , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
  
	 /**********************************************************************
	  *
	  *  RSA_VerifyInit
	  *
	  **********************************************************************/   
	Error = _DX_RSA_VerifyInit(UserContext_ptr,
	                           UserPubKey_ptr,
	                           hashFunc,
                             MGF,
                             SaltLen,
                             PKCS1_ver);
  				  
    if(Error!=CRYS_OK)
	     return Error;
	       				
	 /**********************************************************************
	  *
	  *  RSA_VerifyUpdate
	  *
	  **********************************************************************/	       				  
	Error = _DX_RSA_VerifyUpdate(UserContext_ptr,
			                         DataIn_ptr,
			                         DataInSize);
			      
    if(Error!=CRYS_OK)
	     return Error;			      

	 /**********************************************************************
	  *
	  *  RSA_VerifyFinish
	  *
	  **********************************************************************/	       				  			      	  
	  
	Error = _DX_RSA_VerifyFinish(UserContext_ptr,
								               Sig_ptr);
								  
    return Error;

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     
    
}/* END OF _DX_RSA_Verify */

#endif /*_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT*/
