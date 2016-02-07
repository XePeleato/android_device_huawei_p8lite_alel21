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
#include "crys.h"
#include "crys_rsa_error.h"
#include "crys_rsa_types.h"
#include "crys_ccm.h"
#include "crys_rsa_local.h"
#include "plat_print_dbg.h"
#include "crys_bypass_api.h"
#include "cc_acl.h"

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


/************************ Public Functions ******************************/

#ifndef _INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT
/**********************************************************************************************************/
/**
   \brief RSA_SignInit initializes the Signing
   multi-call algorithm as defined in PKCS#1 v1.5 and 2.1

	The actual macro that will be used by the user is:
	CRYS_RSA_PSS_SignInit		 - for v2.1
	CRYS_RSA_PKCS1v15_SignInit	 - for v1.5

   This function does not do cryptographic processing. Rather, it
   prepares a context that is used by the Update
   and Finish functions.
   
   @param[in,out] UserContext_ptr - A pointer to a Context. The value
                                returned here must be passed to
                                the Update and Finish functions.
   @param[in] UserPrivKey_ptr - A pointer to the private key data
                            structure. \note The representation
                            (pair or quintuple) and hence the
                            algorithm (CRT or not) is determined
                            by the Private Key data
                            structure -using CRYS_Build_PrivKey
                            or CRYS_Build_PrivKeyCRT determines
                            which algorithm will be used.
   @param[in] hashFunc - The hash function to be used. Currently
                         only CRYS_PKCS1_SHA1 is supported. The
                         other hash functions recommended by PKCS#1
                         v2.1 are SHA-256/284/512.
   @param[in] MGF - The mask generation function. PKCS#1 v2.1
                    defines MGF1, so the only value allowed here is CRYS_PKCS1_MGF1.
   @param[in] SaltLen - The Length of the Salt buffer. relevant for PKCS#1 Ver 2.1 Only
   						Typical lengths are 0 and hLen (20 for SHA1)
   						The maximum length allowed is NSize - hLen - 2                    
   @param[in] PKCS1_ver - Ver 1.5 or 2.1, according to the functionality required
                       
   @return CRYSError_t - CRYS_OK, CRYS_BAD_PARAM
*/

CEXPORT_C CRYSError_t _DX_RSA_SignInit(CRYS_RSAPrivUserContext_t *UserContext_ptr,
                             CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
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
   RSAPrivContext_t *ccmWorkingContext_ptr;
         
   /*Pointer to the private key*/
   CRYSRSAPrivKey_t *PrivKey_ptr ;

   /*The modulus size in Octets*/
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

   RETURN_IF_RSA_UNSUPPORTED( UserContext_ptr , UserPrivKey_ptr , hashFunc , 
                              MGF , SaltLen , PKCS1_ver , 
                              ccmWorkingContext_ptr , PrivKey_ptr , 
                              Error , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
      
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   /* if the users context ID pointer is DX_NULL return an error */
   if( UserContext_ptr == DX_NULL )
   
      return CRYS_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;
      
   /*if the private key object is DX_NULL return an error*/   
   if (UserPrivKey_ptr == DX_NULL)
   	  return CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;
   	  
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

   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPrivKey_ptr, sizeof(CRYS_RSAUserPrivKey_t)) ||        
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserContext_ptr, sizeof(CRYS_RSAPrivUserContext_t))){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }

   if(UserPrivKey_ptr->valid_tag != CRYS_RSA_PRIV_KEY_VALIDATION_TAG)
   	  return CRYS_RSA_PRIV_KEY_VALIDATION_TAG_ERROR;

   if(PKCS1_ver == CRYS_PKCS1_VER21)
   {
        /*Initializing the Modulus Size in Bytes needed for SaltLength parameter check*/
        PrivKey_ptr = (CRYSRSAPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;

        /*Note: the (-1) is due to the PKCS#1 Ver2.1 standard section 9.1.1*/
        ModulusSizeBytes =  (DxUint16_t)((PrivKey_ptr->nSizeInBits -1) / 8);
        if((PrivKey_ptr->nSizeInBits -1) % 8)
   		        ModulusSizeBytes++;
   }

   /* ................. aquiring the RSA context ............................. */
   /* ----------------------------------------------------------------------- */
   
  
   Error = CRYS_CCM_GetContext( UserContext_ptr,       				 /* the users context space - in */
                                (void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
                                DX_RSA_SIGN_CONTEXT,                      /* AES type - in */ 
                                AES_DONT_DECRYPT_CONTEXT);			 /* need to decrypt context in AES_block*/   
    
   if( Error != CRYS_OK )    
     return Error;                                    
   ccmWorkingContext_ptr = (RSAPrivContext_t*)tempWorkingctx_ptr;

   /*Reset the Context handler for improper previous values initialized*/
   DX_VOS_MemSet(ccmWorkingContext_ptr,0,sizeof(RSAPrivContext_t));
                             
   /* ................. loading the context .................................. */
   /* ------------------------------------------------------------------------ */

   /*Initializing the Hash operation mode in the RSA Context level*/
   ccmWorkingContext_ptr->HashOperationMode = hashFunc;
   
   switch(ccmWorkingContext_ptr->HashOperationMode)
   {
   		 case CRYS_RSA_HASH_MD5_mode : 
				ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_MD5_DIGEST_SIZE_IN_WORDS;										   
			    Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	            		               CRYS_HASH_MD5_mode);		/*The MD5 mode according to the Hash level enum*/
	                           
			    if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      										 						
			   		
				break;
				
         case CRYS_RSA_HASH_SHA1_mode: 
			 	ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA1_DIGEST_SIZE_IN_WORDS;
       		    Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA1_mode);	/*The SHA1 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			   		
			 	break;
   		 
   		 case CRYS_RSA_HASH_SHA224_mode:
   		 		ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA224_DIGEST_SIZE_IN_WORDS;
		 		Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA224_mode);	/*The SHA224 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			   		
			 	break;
			 	
		 case CRYS_RSA_HASH_SHA256_mode:
		 		ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA256_DIGEST_SIZE_IN_WORDS;
		 		Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
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
         		break;			 						
         		
		 default: 
		 		Error = CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
		 		goto END_WITH_ERROR;
		 		
	}   

   /*Loading the PKCS#1 Version*/
   switch(PKCS1_ver)
   {
   		case CRYS_PKCS1_VER15: 
   		   	ccmWorkingContext_ptr->PKCS1_Version=CRYS_PKCS1_VER15;
   			break;
   								
   		case CRYS_PKCS1_VER21: 
            /*Checking restriction of Salt Length ; Hash output size and the mosulus*/
            if(ModulusSizeBytes < (DxUint32_t)(ccmWorkingContext_ptr->HASH_Result_Size*4 + SaltLen + 2))
            {
                    Error = CRYS_RSA_PSS_ENCODING_MODULUS_HASH_SALT_LENGTHS_ERROR;
                    goto END_WITH_ERROR;
            }

   			ccmWorkingContext_ptr->PKCS1_Version=CRYS_PKCS1_VER21;
   			break;
   								
		default: 
			Error = CRYS_RSA_PKCS1_VER_ARG_ERROR;
			goto END_WITH_ERROR;
   
   }

   switch(MGF)
   {
   		case CRYS_PKCS1_MGF1:
   				ccmWorkingContext_ptr->MGF_2use = CRYS_PKCS1_MGF1;
   				break;
   				
		case CRYS_PKCS1_NO_MGF:
				ccmWorkingContext_ptr->MGF_2use = CRYS_PKCS1_NO_MGF;
				break;   									   									
				
   		default:
   				Error = CRYS_RSA_MGF_ILLEGAL_ARG_ERROR;
   				goto END_WITH_ERROR;
   }
   
   /*Copying the RSA Private key argument to the context*/       
   DX_VOS_FastMemCpy((DxUint8_t *)&ccmWorkingContext_ptr->PrivUserKey,(DxUint8_t *)UserPrivKey_ptr,sizeof(CRYS_RSAUserPrivKey_t));

   /*Initial the Salt random length relevant for PKCS#1 Ver2.1*/
   ccmWorkingContext_ptr->SaltLen = SaltLen;                    
   
	   /* set the RSA tag to the users context */
   UserContext_ptr->valid_tag = CRYS_RSA_SIGN_CONTEXT_VALIDATION_TAG;
   
   goto END;
   
   END:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   
   Error = CRYS_CCM_ReleaseContext(UserContext_ptr,				/* the users context space - in */
		                           ccmWorkingContext_ptr,       /* the CCM context returned - in */
		                           DX_RSA_SIGN_CONTEXT);        /* RSA type - in */ 
   
   return Error;
	
   END_WITH_ERROR:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   {   
   		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(UserContext_ptr,			 /* the users context space - in */
                           							ccmWorkingContext_ptr,       /* the CCM context returned - in */
                           							DX_RSA_SIGN_CONTEXT);        /* RSA type - in */ 
 
		if(Error1 != CRYS_OK)
        {

    		PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));                              
        }
   }   

   /* .............. clearing the users context in case of error.......... */
   /* -------------------------------------------------------------------- */                           
   DX_VOS_MemSet(UserContext_ptr,0,sizeof(CRYS_RSAPrivUserContext_t));
   
   return Error;	

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* _DX_RSA_SignInit */
				  
/**********************************************************************************************************/
/**
   \brief CRYS_RSA_PSS_SignUpdate processes the data to be signed
   in a given context.

   \note RSA_PSS_SignUpdate can be called multiple times
   with data 
   
	The actual macro that will be used by the users is (one of):
	CRYS_RSA_PSS_SignUpdate
	CRYS_RSA_PKCS1v15_SignUpdate

   \note CRYS_RSA_PSS_SignUpdate can be called multiple times
   with data 

   @param[in] UserContext_ptr - A pointer to a valid context,
                            as returned by CRYS_RSA_PSS_SignInit.
   @param[in] DataIn_ptr - A pointer to the data to sign.
   @param[in] DataInSize - The size, in bytes, of the data to sign.

   @return CRYSError_t - CRYS_OK, CRYS_BAD_PARAM
*/

CEXPORT_C CRYSError_t _DX_RSA_SignUpdate(CRYS_RSAPrivUserContext_t *UserContext_ptr,
                               DxUint8_t     *DataIn_ptr,
                               DxUint32_t     DataInSize)
{
   /* FUNCTION DECLERATIONS */
   
   /* The return error identifier */
   CRYSError_t Error;

   /* This pointer is allocated in order to remove compiler alias problems */
   void *tempWorkingctx_ptr;
   /* defining a pointer to the active context allcated by the CCM */
   RSAPrivContext_t *ccmWorkingContext_ptr;
      
   /* FUNCTION LOGIC */
   
   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */   
   
   /* initializing the Error to O.K */
   Error = CRYS_OK;      

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserContext_ptr , DataIn_ptr , DataInSize , 
                              ccmWorkingContext_ptr , Error , Error , 
                              Error , Error , 
                              Error , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
   
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   /* if the users context pointer is DX_NULL return an error */
   if( UserContext_ptr == DX_NULL )
      return CRYS_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;
      
   /* if the users context TAG is illegal return an error - the context is invalid */
   if( UserContext_ptr->valid_tag != CRYS_RSA_SIGN_CONTEXT_VALIDATION_TAG )
      return CRYS_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR;   
   
   /* if the users Data In pointer is illegal return an error */
   if( DataIn_ptr == DX_NULL && DataInSize )
      return CRYS_RSA_DATA_POINTER_INVALID_ERROR;
           
   /* if the data size is larger then 2^29 (to prevant an overflow on the transition to bits ) 
      return error */
   if( DataInSize >= (1 << 29) )
      return CRYS_RSA_INVALID_MESSAGE_DATA_SIZE;   
   
   /* DataIn can be smart pointer but it is tested either in the hash or in bypass operations */
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserContext_ptr, sizeof(CRYS_RSAPrivUserContext_t)) ||
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, DataIn_ptr, DataInSize)){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
      
   /* ................. aquiring the RSA context ............................. */
   /* ------------------------------------------------------------------------ */   

   Error = CRYS_CCM_GetContext( UserContext_ptr,       				 /* the users context space - in */
                                (void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
                                DX_RSA_SIGN_CONTEXT,                 
                                AES_DECRYPT_CONTEXT);			 	 /* need to decrypt context in AES_block*/   
    
    if( Error != CRYS_OK ) 
    {
      goto END_WITH_ERROR;                                       
    }
    ccmWorkingContext_ptr = (RSAPrivContext_t*)tempWorkingctx_ptr;

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
				Error=CRYS_HASH_Update( ((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
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
		   		
        case CRYS_RSA_HASH_NO_HASH_mode:
				/*Do nothing*/
				break;
				
        default:
		 		Error = CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
		 		goto END_WITH_ERROR;
    }
   
    goto END;
   
    END:
    /* ................. release the context ................................. */
    /* ----------------------------------------------------------------------- */
   
    Error = CRYS_CCM_ReleaseContext(UserContext_ptr,				/* the users context space - in */
	 	                            ccmWorkingContext_ptr,       /* the CCM context returned - in */
	 	                            DX_RSA_SIGN_CONTEXT);        /* RSA type - in */ 
   
    return Error;
	
    END_WITH_ERROR:
    /* ................. release the context ................................. */
    /* ----------------------------------------------------------------------- */
    {   
   		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(UserContext_ptr,			 /* the users context space - in */
                           							ccmWorkingContext_ptr,       /* the CCM context returned - in */
                           							DX_RSA_SIGN_CONTEXT);        /* RSA type - in */ 
  
		if(Error1 != CRYS_OK)
        {
    		PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));                              
        }
    }   

    /* .............. clearing the users context in case of error.......... */
    /* -------------------------------------------------------------------- */                           
    DX_VOS_MemSet(UserContext_ptr,0,sizeof(CRYS_RSAPrivUserContext_t));
   
    return Error;	

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* _DX_RSA_SignUpdate */


/**********************************************************************************************************/
/**
   \brief _DX_RSA_SignFinish calculates the signature on the
   data passed to one or more calls to CRYS_RSA_PSS_SignUpdate,
   and releases the context.

	The actual macro that will be used by the user is:
	CRYS_RSA_PSS_SignFinish			- for v2.1
	CRYS_RSA_PKCS1v15_SignFinish	- for v1.5
	
   @param[in,out] UserContext_ptr - A pointer to the Context
                                initialized by the SignInit function 
                                and used by the SignUpdate function
   @param[out] Output_ptr - A pointer to the signature. 
                            The buffer must be at least PrivKey_ptr->N.len bytes long 
                            (that is, the size of the modulus, in bytes).
   @param[in,out] OutputSize_ptr - A pointer to the Signature Size value - the input value
                                   is the signature buffer size allocated, the output value is 
                                   the signature size used.
                                   The buffer must be at least PrivKey_ptr->N.len bytes long 
                                   (that is, the size of the modulus, in bytes).

   @return CRYSError_t - CRYS_OK,
                         CRYS_RSA_INVALID_USER_CONTEXT_POINTER_ERROR,
                         CRYS_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR,
                         CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR,
                         CRYS_RSA_INVALID_SIGNATURE_BUFFER_SIZE,
                         CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR,
                         CRYS_RSA_PKCS1_VER_ARG_ERROR
*/

CEXPORT_C CRYSError_t _DX_RSA_SignFinish(CRYS_RSAPrivUserContext_t *UserContext_ptr,
                               DxUint8_t                   *Output_ptr,
                               DxUint16_t                  *OutputSize_ptr)  
{
   /* FUNCTION DECLERATIONS */
   
	 /* The return error identifier */
   CRYSError_t Error;

   /* This pointer is allocated in order to remove compiler alias problems */
   void *tempWorkingctx_ptr;

   /* defining a pointer to the active context allocated by the CCM */
   RSAPrivContext_t *ccmWorkingContext_ptr;
   
   /*The modulus size in Octets*/
   DxUint16_t K;
   
   /*The local variables for PKCS#1 Ver 1.5*/
    BerParserObj_t BerParserObj;
    DxUint8_t *FormatedBlock_ptr;
    DxUint8_t *FormatedBlockSize_ptr;   
    
   	CRYSRSAPrivKey_t *PrivKey_ptr;
   	   	
   /* FUNCTION LOGIC */
   
   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */   
   
   /* initializing the Error to O.K */
   Error = CRYS_OK;      

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_RSA_UNSUPPORTED( UserContext_ptr , Output_ptr , OutputSize_ptr , 
                              ccmWorkingContext_ptr , K , BerParserObj.DigestAlg , 
                              FormatedBlock_ptr , FormatedBlockSize_ptr , 
                              PrivKey_ptr , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
   
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   /* if the users context pointer is DX_NULL return an error */
   if( UserContext_ptr == DX_NULL )
      return CRYS_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;
         
   /*If the output pointer is DX_NULL return Error*/   
   if(Output_ptr == DX_NULL)
   	  return CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR;
   	     	        
   /*If the output Size pointer is DX_NULL return Error*/   
   if(OutputSize_ptr == DX_NULL)
   	  return CRYS_RSA_INVALID_OUTPUT_SIZE_POINTER_ERROR;

   /* DataIn can be smart pointer but it is tested either in the hash or in bypass operations */
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserContext_ptr, sizeof(CRYS_RSAPrivUserContext_t)) ||
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, Output_ptr, *OutputSize_ptr) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, OutputSize_ptr, sizeof(DxUint32_t))){
	   return CRYS_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }

   /* if the users context TAG is illegal return an error - the context is invalid */
   if( UserContext_ptr->valid_tag != CRYS_RSA_SIGN_CONTEXT_VALIDATION_TAG )
      return CRYS_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR;   


   /* ................. aquiring the RSA context ............................. */
   /* ------------------------------------------------------------------------ */
   
   Error = CRYS_CCM_GetContext( UserContext_ptr,       				 /* the users context space - in */
                                (void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
                                DX_RSA_SIGN_CONTEXT,                 
                                AES_DECRYPT_CONTEXT);			 	 /* need to decrypt context in AES_block*/   
    
   if( Error != CRYS_OK ) 
     return Error;                                       
   ccmWorkingContext_ptr = (RSAPrivContext_t*)tempWorkingctx_ptr;

   /* ................. checking the validity of the prameters in the context ........ */
   /* -------------------------------------------------------------------------------- */
          
   PrivKey_ptr = (CRYSRSAPrivKey_t *)ccmWorkingContext_ptr->PrivUserKey.PrivateKeyDbBuff; 
   
   /*Initializing the Modulus Size in Bytes*/
   K =  (DxUint16_t)(PrivKey_ptr->nSizeInBits / 8);
   if(PrivKey_ptr->nSizeInBits % 8)
   		K++;
 	
 	 /* If the received output buffer is small then return an error */
   if( *OutputSize_ptr < K )
   {
       *OutputSize_ptr = K;
      Error = CRYS_RSA_INVALID_SIGNATURE_BUFFER_SIZE;
      goto END_WITH_ERROR_WITHOUT_CLEAN_CONTEXT;/*give the user second changce*/
   }   
     

   /*Operating the HASH Finish function only in case that Hash operation is needed*/
   if(ccmWorkingContext_ptr->HashOperationMode <= CRYS_RSA_HASH_SHA512_mode)
   {   
	    Error=CRYS_HASH_Finish(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
		                         ccmWorkingContext_ptr->HASH_Result);   		    
	    if( Error != CRYS_OK ) 
	    	goto END_WITH_ERROR;
   }				                                 
   
   PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \n In RSA_SignFinish ; HASH_Result = " ,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result , ccmWorkingContext_ptr->HASH_Result_Size*4));       
   
   /* ........................... execute the signiture ........................... */
   /* ----------------------------------------------------------------------------- */
   
   switch(ccmWorkingContext_ptr->PKCS1_Version)
   {
#ifndef _INTERNAL_CRYS_NO_RSA_SCHEME_21_SUPPORT
   case CRYS_PKCS1_VER21:
	
		Error = CRYS_RSA_PSS_Sign21(ccmWorkingContext_ptr,
				    Output_ptr);
		if(Error!=CRYS_OK)
		goto END_WITH_ERROR;
		
		/* set the output size to the modolous size */
		*OutputSize_ptr	= K;
		
		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \n In RSA_SignFinish ; After CRYS_RSA_PSS_Sign21 ; Output_ptr = " ,(DxUint8_t *)Output_ptr , K));
		break;
#endif
#ifndef _INTERNAL_CRYS_NO_RSA_SCHEME_15_SUPPORT        
   case CRYS_PKCS1_VER15:
	   	
        /* Call BerEncoder */
        BerParserObj.MessageDigest_ptr=(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result;
	 		  BerParserObj.MessageDigestSize=(DxUint8_t)ccmWorkingContext_ptr->HASH_Result_Size*4; /*The HASH_Result_Size is in words*/	 		   

	 		  /* setting the ber parser digest algorithm according to the selected HASH algorithm */
        switch(ccmWorkingContext_ptr->HashOperationMode)
        {
	case CRYS_RSA_HASH_SHA1_mode:
	case CRYS_RSA_After_SHA1_mode:	 		   
		BerParserObj.DigestAlg = (DxUint8_t)CRYS_HASH_SHA1_mode; 	 		   
		break;
	
	case CRYS_RSA_HASH_MD5_mode:
	case CRYS_RSA_After_MD5_mode:	 		   
		BerParserObj.DigestAlg = (DxUint8_t)CRYS_HASH_MD5_mode; 	 		   
		break;
	
	case CRYS_RSA_HASH_SHA224_mode:
	case CRYS_RSA_After_SHA224_mode:	 		   
		BerParserObj.DigestAlg = (DxUint8_t)CRYS_HASH_SHA224_mode; 	 		   
		break;
		
	case CRYS_RSA_HASH_SHA256_mode:
	case CRYS_RSA_After_SHA256_mode:	 		   
		BerParserObj.DigestAlg = (DxUint8_t)CRYS_HASH_SHA256_mode; 	 		   
		break;
		
	case CRYS_RSA_HASH_SHA384_mode:
	case CRYS_RSA_After_SHA384_mode:	 		   
		BerParserObj.DigestAlg = (DxUint8_t)CRYS_HASH_SHA384_mode; 	 		   
		break;
		
	case CRYS_RSA_HASH_SHA512_mode:
	case CRYS_RSA_After_SHA512_mode:	 		   
		BerParserObj.DigestAlg = (DxUint8_t)CRYS_HASH_SHA512_mode; 	 		   
		break;
		
	case CRYS_RSA_HASH_NO_HASH_mode:
	default:
		Error = CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
		goto END_WITH_ERROR;
			   
	}/* end of ccmWorkingContext_ptr->HashOperationMode switch */
	 		      			 		   			
        FormatedBlock_ptr=(DxUint8_t*)ccmWorkingContext_ptr->BER;
        FormatedBlockSize_ptr=(DxUint8_t*)&ccmWorkingContext_ptr->BERSize;	   

        Error=CRYS_RSA_BER_BerEncoder(&FormatedBlock_ptr ,
                                      FormatedBlockSize_ptr,
                                      (DxUint16_t)CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS,
                                      &BerParserObj);
			
        if(Error!=CRYS_RSA_BER_ENCODING_OK)
        {		
          goto END_WITH_ERROR;
        }
        PLAT_LOG_DEV_PRINT(( 10 ," \n In RSA_SignFinish ; After CRYS_RSA_BER_BerEncoder ; Hash type is %lx:\n", BerParserObj.DigestAlg));        					 
        PLAT_LOG_DEV_PRINT_DisplayBuffer(( 22 , (char *)" \n In RSA_SignFinish ; After CRYS_RSA_BER_BerEncoder FormatedBlock_ptr = " ,(DxUint8_t *)FormatedBlock_ptr , *FormatedBlockSize_ptr));       		              

        /*The ouput Size should be in the octet size of the Modulus = K*/
        Error = CRYS_RSA_PKCS1_v1_5_Encode(K,
				/*Private key uses Block Type 01*/
				CRYS_RSA_PKCS1_15_MODE01,
				FormatedBlock_ptr,
				*FormatedBlockSize_ptr,
				(DxUint8_t *)ccmWorkingContext_ptr->EBD);
          				
	if(Error!=CRYS_OK) 
		goto END_WITH_ERROR;
       
	PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \n In RSA_SignFinish ; After CRYS_RSA_PKCS1_v1_5_Encode ; Output_ptr = " ,(DxUint8_t *)ccmWorkingContext_ptr->EBD , K));		               
		               
        /*-------------------------------------------*/
        /*                                           */
        /* RSA computation			                  */
        /*                                           */
        /*-------------------------------------------*/

        /* ........... STEP1 : building the key database  ........................ */
        /* ----------------------------------------------------------------------- */
					   
				/*The Private Key is already built inside the context with the correct Pair or CRT mode*/
					 
        /* .................STEP2:   execute RSA encryped ....................... */
        /* ---------------------------------------------------------------------- */
					   
        PLAT_LOG_DEV_PRINT(( 10 ," \n In RSA_SignFinish Before PRIM_Decrypt operation:\n" ));        					 
        PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nModulus N = " ,(DxUint8_t *)PrivKey_ptr->n , PrivKey_ptr->nSizeInBits/8));   
			    
        #ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED
				    
        if(PrivKey_ptr->OperationMode == CRYS_RSA_NoCrt)
        {
          PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nExp D =\n" ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.NonCrt.d , PrivKey_ptr->PriveKeyDb.NonCrt.dSizeInBits/8));   	    
        }
        else
        {
          PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nP = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.P , PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits/8));   	    	    
          PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nQ = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.Q , PrivKey_ptr->PriveKeyDb.Crt.QSizeInBits/8));   	    	    
          PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \ndP = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.dP , PrivKey_ptr->PriveKeyDb.Crt.dPSizeInBits/8));   	    	    
          PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \ndQ = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.dQ , PrivKey_ptr->PriveKeyDb.Crt.dQSizeInBits/8));   	    	    
          PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nqInv = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.qInv , PrivKey_ptr->PriveKeyDb.Crt.qInvSizeInBits/8));   	    	    
        }
				
        #endif					   
						
        PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nDataIn_ptr = " ,(DxUint8_t *)ccmWorkingContext_ptr->EBD , K));       						   
			
	/* execute the expomnent - after correct building it is working with either CRT or PAIR mode*/ 
	Error = CRYS_RSA_PRIM_Decrypt(&ccmWorkingContext_ptr->PrivUserKey,
					&ccmWorkingContext_ptr->PrimeData,
					(DxUint8_t *)ccmWorkingContext_ptr->EBD,
					K,
					Output_ptr);
	
	if(Error!=CRYS_OK) 
		goto END_WITH_ERROR;
		
	/* set the output size to the modolous size */
	*OutputSize_ptr	= K;
	
	PLAT_LOG_DEV_PRINT_DisplayBuffer(( 10 , (char *)" \nCRYS_RSA_PRIM_Decrypt output = Output_ptr = " ,(DxUint8_t *)Output_ptr , K));   	    	    		               
	
	break;
#endif		               
	default: 
	
		Error = CRYS_RSA_PKCS1_VER_ARG_ERROR;	  
		goto END_WITH_ERROR;
   
   }/* end of (ccmWorkingContext_ptr->PKCS1_Version ) switch */
					                                 
   goto END;
   
   END_WITH_ERROR_WITHOUT_CLEAN_CONTEXT:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   {   
   		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(UserContext_ptr,			   /* the users context space - in */
                                                   ccmWorkingContext_ptr,  /* the CCM context returned - in */
                                                   DX_RSA_SIGN_CONTEXT);   /* RSA type - in */ 
      
    
		if(Error1 != CRYS_OK)
        {
			PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));                              
        }
   }
   
   return Error;	
   END:	/*On finish operation need to memset the context to zero so END and END_WITH_ERROR are the same*/
   END_WITH_ERROR:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   {   
   		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(UserContext_ptr,			   /* the users context space - in */
                                                   ccmWorkingContext_ptr,  /* the CCM context returned - in */
                                                   DX_RSA_SIGN_CONTEXT);   /* RSA type - in */ 
      
    
		if(Error1 != CRYS_OK)
        {
			PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));                              
        }
   }
   /* .............. clearing the users context in case of error.......... */
   /* -------------------------------------------------------------------- */                           
   DX_VOS_MemSet(UserContext_ptr,0,sizeof(CRYS_RSAPrivUserContext_t));
   
   return Error;	

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     
				    
}/* _DX_RSA_SignFinish */
				    

/**********************************************************************************************************/
/**
   @brief
   RSA_Sign implements the Signing algorithm as defined
   in PKCS#1 v1.5 or v2.1, using a single function

   The actual macros that will be used by the user are:
   CRYS_RSA_PKCS1v15_Sign		- for v1.5
   CRYS_RSA_PKCS1v15_SHA1_Sign	- for v1.5 data is hashed with SHA1
   CRYS_RSA_PKCS1v15_MD5_Sign	- for v1.5 data is hashed with MD5
   CRYS_RSA_PSS_Sign			- for v2.1
   CRYS_RSA_PSS_SHA1_Sign		- for v2.1 data is hashed with SHA1
					- for v2.1 MD5 is not supported, since 
					according to the PKCS#1 ver2.1 it is not recommended
   
   @param[in] UserContext_ptr - A pointer to a Context. For the use of the 
   								function as a space to work on.
   @param[in] UserPrivKey_ptr - A pointer to the private key data
                            structure of the user. \note The representation
                            (pair or quintuple) and hence the
                            algorithm (CRT or not) is determined
                            by the Private Key data
                            structure - using CRYS_Build_PrivKey
                            or CRYS_Build_PrivKeyCRT determines
                            which algorithm will be used.
   @param[in] hashFunc - The hash function to be used. Currently
                         only CRYS_PKCS1_SHA1 and CRYS_PKCS1_MD5
                         are supported. 
   @param[in] MGF - The mask generation function. Only for PKCS#1 v2.1
                    defines MGF1, so the only value allowed for v2.1 
                    is CRYS_PKCS1_MGF1. 
   @param[in] SaltLen - The Length of the Salt buffer. relevant for PKCS#1 Ver 2.1 Only
   						Typical lengths are 0 and hLen (20 for SHA1)
   						The maximum length allowed is NSize - hLen - 2                                                                                       
   @param[in] DataIn_ptr - A pointer to the data to sign.
   @param[in] DataInSize - The size, in bytes, of the data to sign.
   @param[out] Output_ptr - A pointer to the signature. 
                            The buffer must be at least PrivKey_ptr->N.len bytes long 
                            (that is, the size of the modulus in bytes).
   @param[in,out] OutputSize_ptr - A pointer to the Signature Size value - the input value
                            is the signature buffer size allocated, the output value is 
                            the signature size actually used.
                            The buffer must be at least PrivKey_ptr->N.len bytes long 
                            (that is, the size of the modulus in bytes).
   @param[in] PKCS1_ver - Ver 1.5 or 2.1, according to the functionality required
      
   @return CRYSError_t - CRYS_OK,
                         CRYS_RSA_INVALID_USER_CONTEXT_POINTER_ERROR,
                         CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                         CRYS_RSA_PRIV_KEY_VALIDATION_TAG_ERROR,
                         CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR,
                         CRYS_RSA_MGF_ILLEGAL_ARG_ERROR,
                         CRYS_RSA_PKCS1_VER_ARG_ERROR,
                         CRYS_RSA_INVALID_MESSAGE_DATA_SIZE,
                         CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR,
                         CRYS_RSA_INVALID_SIGNATURE_BUFFER_SIZE
*/


CEXPORT_C CRYSError_t _DX_RSA_Sign(CRYS_RSAPrivUserContext_t *UserContext_ptr,
                         CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
                         CRYS_RSA_HASH_OpMode_t hashFunc,
                         CRYS_PKCS1_MGF_t MGF,
                         DxUint16_t SaltLen,				  				  			      
                         DxUint8_t     *DataIn_ptr,
                         DxUint32_t     DataInSize,
                         DxUint8_t     *Output_ptr,
                         DxUint16_t    *OutputSize_ptr,
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

    RETURN_IF_RSA_UNSUPPORTED( UserContext_ptr , UserPrivKey_ptr , hashFunc , 
                              MGF , SaltLen , DataIn_ptr , DataInSize , 
                              Output_ptr , OutputSize_ptr , 
                              PKCS1_ver , Error , Error , Error ,
                              Error,Error,Error,Error,Error,Error,Error,Error,Error); 
                              
    #ifndef CRYS_NO_HASH_SUPPORT                                      
    #ifndef CRYS_NO_PKI_SUPPORT                                      
 
	 /**********************************************************************
	  *
	  *  RSA_SignInit
	  *
	  **********************************************************************/   
   
	Error = _DX_RSA_SignInit( UserContext_ptr,
                            UserPrivKey_ptr,
                            hashFunc,
	                          MGF,
	                          SaltLen,
	                          PKCS1_ver);

    if(Error!=CRYS_OK)
	     return Error;


	 /**********************************************************************
	  *
	  *  RSA_SignUpdate
	  *
	  **********************************************************************/
	  
    Error = _DX_RSA_SignUpdate(UserContext_ptr,
                               DataIn_ptr,
                               DataInSize);

    if(Error!=CRYS_OK)
	
		 return Error;
    

	 /**********************************************************************
	  *
	  * RSA_SignFinish
	  *
	  **********************************************************************/

	  
    Error = _DX_RSA_SignFinish(UserContext_ptr,
                               Output_ptr,
                               OutputSize_ptr);
	
   return Error;

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_PKI_SUPPORT */                                     

}/* END OF _DX_RSA_Sign */

#endif /*_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT*/
