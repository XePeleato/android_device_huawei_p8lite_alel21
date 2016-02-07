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
#include "crys_ccm.h"
#include "dx_vos_mem.h"
#include "plat_print_dbg.h"
#include "crys_bypass_api.h"
#include "cc_acl.h"
#include "crys_ecpki_error.h"
#include "crys_ecpki_local.h"
#include "crys.h"
#include "crys_common.h"

/* .............. LLF level includes ................. */
#include "llf_ecpki_export.h"

/* canceling the lint warning:
   Warning 548: else expected */
/*lint --e{548} */

/* canceling the lint warning:
Info 801: Use of goto is deprecated */
/*lint --e{801} */

/************************ Defines *****************************************/
#if ( CRYS_HASH_USER_CTX_SIZE_IN_WORDS > CRYS_PKA_RSA_HASH_CTX_SIZE_IN_WORDS )
#error CRYS_PKA_RSA_HASH_CTX_SIZE_IN_WORDS OR CRYS_HASH_USER_CTX_SIZE_IN_WORDS do not defined correctly.
#endif

/************************ Enums *******************************************/

/************************ Typedefs ****************************************/

/************************ Global Data *************************************/

/************* Private function prototype *********************************/

/************************ Public Functions ********************************/

/**************************************************************************
 *	              CRYS_ECDSA_VerifyInit  function
 **************************************************************************/
/**
   @brief  Prepares a context that is used by the Update and Finish functions
           but does not perform elliptic curve cryptographic processing

		    The function:
			- Receives and decrypts user data (working context). 
			- Checks input parameters of  ECDSA Vrifying primitive.
			- Calls hash init function.
			- Initializes variables and structures for calling next functions.
			- Encrypts and releases working context.
			
			NOTE: Using of HASH functions with HASH size great, than EC modulus size, 
			is not recommended!

   @param[in,out] VerifyUserContext_ptr - A pointer to the user buffer for verifying database.
   @param[in] SignerPublKey_ptr - A pointer to a Signer public key structure.
   @param[in] HashMode - The enumerator variable defines the hash function to be used.
   
   @return <b>CRYSError_t</b>: <br> 
			 CRYS_OK<br>
                         CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR <br>
			 CRYS_ECDSA_VERIFY_INVALID_SIGNER_PUBL_KEY_PTR_ERROR <br>
                         CRYS_ECDSA_VERIFY_SIGNER_PUBL_KEY_VALIDATION_TAG_ERROR <br>
                         CRYS_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR <br>
			 CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR <br>
**/
CEXPORT_C CRYSError_t CRYS_ECDSA_VerifyInit( 
	                               CRYS_ECDSA_VerifyUserContext_t  *VerifyUserContext_ptr, /*in/out*/
								   CRYS_ECPKI_UserPublKey_t    	   *SignerPublKey_ptr,     /*in*/
								   CRYS_ECPKI_HASH_OpMode_t         HashMode               /*in*/ )
{
   /* FUNCTION DECLERATIONS */

   /* The return error identifier */
   CRYSError_t Error;
   
   /* This pointer is allocated in order to remove compiler alias problems */
   void *tempWorkingctx_ptr;

   /* defining a pointer to the active context allcated by the CCM */
   ECDSA_VerifyContext_t *ccmWorkingContext_ptr;         
   
   /* Pointer to public key structure inside the signer public key */
   CRYS_ECPKI_PublKey_t   *PublKey_ptr; 
   CRYS_ECPKI_DomainID_t    DomainID; 

   /* FUNCTION LOGIC */
   
   /* ............... local initializations .............................. */
   /* -------------------------------------------------------------------- */
   
   /* initializing the Error to OK */
   Error = CRYS_OK;

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_ECPKI_UNSUPPORTED( VerifyUserContext_ptr, SignerPublKey_ptr, HashMode , 
                              ccmWorkingContext_ptr, PublKey_ptr, DomainID,   
                              Error, Error, Error, Error, Error,
                              Error, Error, Error, Error, Error,
                              Error, Error, Error, Error, Error, Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_ECPKI_SUPPORT    
                                   
      
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   /* if the users context ID pointer is DX_NULL return an error */
   if( VerifyUserContext_ptr == DX_NULL )
   
      return CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR;
      
   /*if the private key object is DX_NULL return an error*/   
   if (SignerPublKey_ptr == DX_NULL)
   	  return CRYS_ECDSA_VERIFY_INVALID_SIGNER_PUBL_KEY_PTR_ERROR;
  	     	  
   /* check if the hash operation mode is legal */
   if( HashMode >= CRYS_ECPKI_HASH_NumOfModes)
      return CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR;     	  
 
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, VerifyUserContext_ptr, sizeof(CRYS_ECDSA_VerifyUserContext_t)) ||
       DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, SignerPublKey_ptr, sizeof(CRYS_ECPKI_UserPublKey_t))){
	   return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }

   if(SignerPublKey_ptr->valid_tag != CRYS_ECPKI_PUBL_KEY_VALIDATION_TAG)
	  return CRYS_ECDSA_VERIFY_SIGNER_PUBL_KEY_VALIDATION_TAG_ERROR;

   /*Initializing the DomainID */
   PublKey_ptr = (CRYS_ECPKI_PublKey_t *)((void*)SignerPublKey_ptr->PublKeyDbBuff);     
   DomainID = PublKey_ptr->DomainID;   

   /* ...... Continue checking: check the EC domain ID.................... */
   if( DomainID >= CRYS_ECPKI_DomainID_OffMode )   
      return CRYS_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR;

   /* ................. acquiring the ECDSA context ............................ */
   /* ----------------------------------------------------------------------- */ 
   Error = CRYS_CCM_GetContext( VerifyUserContext_ptr,   				 /* the users context space - in */
                                (void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
                                DX_ECDSA_VERIFY_CONTEXT,               /* AES type - in */ 
                                AES_DONT_DECRYPT_CONTEXT);			 /* need to decrypt context in AES_block*/      
   if( Error != CRYS_OK )     
        return Error;                        
   ccmWorkingContext_ptr = (ECDSA_VerifyContext_t*)tempWorkingctx_ptr;

   /*Reset the Context handler for improper previous values initialized*/
   DX_VOS_MemSet(ccmWorkingContext_ptr,0,sizeof(ECDSA_VerifyContext_t));
                             
   /* ................. loading the context .................................. */
   /* ------------------------------------------------------------------------ */
 
   /*Initializing the Hash operation mode in the ECDSA Context */
   ccmWorkingContext_ptr->HashMode = HashMode;
   
   switch(ccmWorkingContext_ptr->HashMode)
   {
 				
         case CRYS_ECPKI_HASH_SHA1_mode:
			 	ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA1_DIGEST_SIZE_IN_WORDS;
       		    Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA1_mode);	/*The SHA1 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			 	break;
   		 
   		 case CRYS_ECPKI_HASH_SHA224_mode:
   		 		ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA224_DIGEST_SIZE_IN_WORDS;
		 		Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA224_mode);	/*The SHA224 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			 	break;
			 	
		 case CRYS_ECPKI_HASH_SHA256_mode:
		 		ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA256_DIGEST_SIZE_IN_WORDS;
		 		Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA256_mode);	/*The SHA256 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			 	break;
			 	
		 case CRYS_ECPKI_HASH_SHA384_mode:
		 		ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA384_DIGEST_SIZE_IN_WORDS;
		 		Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA384_mode);	/*The SHA384 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			 	break;
			 	
		 case CRYS_ECPKI_HASH_SHA512_mode:
			 	ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA512_DIGEST_SIZE_IN_WORDS;
		 		Error = CRYS_HASH_Init(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
	                    		       CRYS_HASH_SHA512_mode);	/*The SHA512 mode according to the Hash level enum*/
	                           
	   			if(Error != CRYS_OK)
			   		goto END_WITH_ERROR;      
			 	break;
			 	
		 /* if input data is used as HASH representative (AFTER_HASH mode) set the sizes of HASH result */
		 	 	
         case CRYS_ECPKI_AFTER_HASH_SHA1_mode: 
			 	ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA1_DIGEST_SIZE_IN_WORDS;
			 	break;
			 	
		 case CRYS_ECPKI_AFTER_HASH_SHA224_mode: 
			 	ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA224_DIGEST_SIZE_IN_WORDS;
			 	break;
			 	
		 case CRYS_ECPKI_AFTER_HASH_SHA256_mode: 
			 	ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA256_DIGEST_SIZE_IN_WORDS;
			 	break;
			 	
		 case CRYS_ECPKI_AFTER_HASH_SHA384_mode: 
			 	ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA384_DIGEST_SIZE_IN_WORDS;
			 	break;
			 	
		 case CRYS_ECPKI_AFTER_HASH_SHA512_mode: 
			 	ccmWorkingContext_ptr->HASH_Result_Size = CRYS_HASH_SHA512_DIGEST_SIZE_IN_WORDS;
			 	break;	
			 	
			 	
 		 default: 
		 		Error = CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR;
		 		goto END_WITH_ERROR;
	} 

   /*Copying the ECPKI Public key value to the context*/       
   DX_VOS_FastMemCpy((DxUint8_t *)&ccmWorkingContext_ptr->ECDSA_SignerPublKey,(DxUint8_t *)SignerPublKey_ptr,sizeof(CRYS_ECPKI_UserPublKey_t));
   
   /* Copy the DomainID to Public key in working context */
   ((CRYS_ECPKI_PublKey_t*)(void*)(ccmWorkingContext_ptr->ECDSA_SignerPublKey.PublKeyDbBuff))->DomainID = 
	    ((CRYS_ECPKI_PublKey_t*)((void*)SignerPublKey_ptr->PublKeyDbBuff))->DomainID;

   /* set the ECDSA tag to the users context */
   VerifyUserContext_ptr->valid_tag = CRYS_ECDSA_VERIFY_CONTEXT_VALIDATION_TAG;
   
 
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   
   Error = CRYS_CCM_ReleaseContext(VerifyUserContext_ptr,		/* the users context space - in */
				   ccmWorkingContext_ptr,       /* the CCM context returned - in */
				   DX_ECDSA_VERIFY_CONTEXT);    /* ECDSA type - in */ 
   
   return Error;
	
   END_WITH_ERROR:
   /* ................. release the context ................................. */
   /* ----------------------------------------------------------------------- */
   {  
   
     
	CRYSError_t Error1 = CRYS_CCM_ReleaseContext(VerifyUserContext_ptr, /* the users context space - in */
						ccmWorkingContext_ptr,                 /* the CCM context returned - in */
						DX_ECDSA_VERIFY_CONTEXT);              /* ECDSA type - in */ 
	
	if(Error1 != CRYS_OK){
		PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));
	}
   }   

   /* .............. clearing the users context in case of error.......... */
   /* -------------------------------------------------------------------- */                           
   DX_VOS_MemSet(VerifyUserContext_ptr, 0, sizeof(CRYS_ECDSA_VerifyUserContext_t));
   
   return Error;	

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_ECPKI_SUPPORT */                                     

}/* _DX_ECDSA_VerifyInit */



/**************************************************************************
 *	              CRYS_ECDSA_VerifyUpdate function
 **************************************************************************/
/**
   @brief  Performs a hash  operation on data allocated by the user 
           before finally verifying its signature.
  
   	   In case user divides signing data by block, he must call the Update function 
   	   continuously a number of times until processing of the entire data block is complete.
   	   
       NOTE: Using of HASH functions with HASH size great, than EC modulus size, 
             is not recommended!

   @param[in,out] VerifyUserContext_ptr A pointer to the user buffer for verifying database.
   @param[in] 	  MessageDataIn_ptr   Message data for calculating Hash.   	
   @param[in]     DataInSize            The size of the message data block, in bytes.
   
   @return <b>CRYSError_t</b>: <br> 
			 CRYS_OK<br>
                         CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR <br>
			 CRYS_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR <br>
			 CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_PTR_ERROR <br>
			 CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR <br>
			 CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR <br>
 **/
CEXPORT_C CRYSError_t CRYS_ECDSA_VerifyUpdate( 
			CRYS_ECDSA_VerifyUserContext_t *VerifyUserContext_ptr, /*in/out*/
			DxUint8_t                      *MessageDataIn_ptr,     /* in */ 
			DxUint32_t                      DataInSize             /* in */ )
{
   /* FUNCTION DECLERATIONS */
   
   /* The return error identifier */
   CRYSError_t Error;

   /* This pointer is allocated in order to remove compiler alias problems */
   void *tempWorkingctx_ptr;

   /* defining a pointer to the active context allcated by the CCM */
   ECDSA_VerifyContext_t *ccmWorkingContext_ptr;
      
   /* FUNCTION LOGIC */
   
   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */   
   
   /* initializing the Error to O.K */
   Error = CRYS_OK;      

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_ECPKI_UNSUPPORTED( VerifyUserContext_ptr , MessageDataIn_ptr , DataInSize , 
                              ccmWorkingContext_ptr , Error , Error , 
                              Error , Error , Error , Error , Error ,
                              Error , Error , Error , Error , Error ,
                              Error , Error , Error , Error , Error , Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_ECPKI_SUPPORT                                      
   
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   /* if the users context pointer is DX_NULL return an error */
   if( VerifyUserContext_ptr == DX_NULL )
      return CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR;
      
   /* if the users context TAG is illegal return an error - the context is invalid */
   if( VerifyUserContext_ptr->valid_tag != CRYS_ECDSA_VERIFY_CONTEXT_VALIDATION_TAG )
      return CRYS_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR;   
   
   /* if the users MessageDataIn pointer is illegal return an error */
   if( MessageDataIn_ptr == DX_NULL && DataInSize )
      return CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_PTR_ERROR;

   /* if the data size is larger then 2^29 (to prevent an overflow on the transition to bits ) 
      return error */
   if( DataInSize >= (1UL << 29) )
      return CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;    

   /* MessageDataIn_ptr can be smart pointer but it is tested either in the hash or in bypass operations */
   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, VerifyUserContext_ptr, sizeof(CRYS_ECDSA_VerifyUserContext_t)) || 
       DxCcAcl_IsBuffAccessOk(ACCESS_READ, MessageDataIn_ptr, DataInSize)){
	   return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
      
   /* ................. acquiring the Verify context ......................... */
   /* ------------------------------------------------------------------------ */
   
   Error = CRYS_CCM_GetContext( VerifyUserContext_ptr,       		 /* the users context space - in */
                                (void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
                                DX_ECDSA_VERIFY_CONTEXT,               
                                AES_DECRYPT_CONTEXT);			 	 /* need to decrypt context in AES_block*/   
    
   if( Error != CRYS_OK ) 
     return Error;                                       
   ccmWorkingContext_ptr = (ECDSA_VerifyContext_t*)tempWorkingctx_ptr;

    switch(ccmWorkingContext_ptr->HashMode)
    {   
        /* The mode is either SHA1 or SHA2 */
        case CRYS_ECPKI_HASH_SHA1_mode:
    	case CRYS_ECPKI_HASH_SHA224_mode:
    	case CRYS_ECPKI_HASH_SHA256_mode:
    	case CRYS_ECPKI_HASH_SHA384_mode:
    	case CRYS_ECPKI_HASH_SHA512_mode:
		
		/*Operate the Hash update function for relevant version */   			
		Error=CRYS_HASH_Update( ((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
								MessageDataIn_ptr, DataInSize );
		
		if(Error != CRYS_OK)
			goto END_WITH_ERROR; 
			 
		break;
		
        /* if DataIn is used as HASH representative (AFTER_HASH mode), then copy it into 
           Context HASH result */ 
                                
        case CRYS_ECPKI_AFTER_HASH_SHA1_mode:
		
		if(DataInSize != CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}

		/* Copy the DataIn_ptr to the HashResult */
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
		
		break;
			
 		   		
    	case CRYS_ECPKI_AFTER_HASH_SHA224_mode:
		
		if(DataInSize != CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}
		/* Copy the DataIn_ptr to the HashResult */	   			
		
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
		
		break;
		   		
    	case CRYS_ECPKI_AFTER_HASH_SHA256_mode:
    			
		if(DataInSize != CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}
		/* Copy the DataIn_ptr to the HashResult */	   			
		
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
		
		break;
		   		
    	case CRYS_ECPKI_AFTER_HASH_SHA384_mode:
    			
		if(DataInSize != CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}
		/* Copy the DataIn_ptr to the HashResult */	   			
		
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
			
		break;
			
    	case CRYS_ECPKI_AFTER_HASH_SHA512_mode:
		   		
		if(DataInSize != CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}
		/* Copy the DataIn_ptr to the HashResult */	   			
		
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
		
		break;
		
        default:
		Error = CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR;
		goto END_WITH_ERROR;
    }


    /* ................. release the context ................................. */
    /* ----------------------------------------------------------------------- */
   
    Error = CRYS_CCM_ReleaseContext(VerifyUserContext_ptr,				/* the users context space - in */
	 	                            ccmWorkingContext_ptr,              /* the CCM context returned - in */
	 	                            DX_ECDSA_VERIFY_CONTEXT);           /* ECDSA type - in */ 
    return Error;

    END_WITH_ERROR:
    /* ................. release the context ................................. */
    /* ----------------------------------------------------------------------- */
    {   
	CRYSError_t Error1 = CRYS_CCM_ReleaseContext(VerifyUserContext_ptr,	 /* the users context space - in */
						 ccmWorkingContext_ptr,  /* the CCM context returned - in */
						 DX_ECDSA_VERIFY_CONTEXT); /* ECDSA  type - in */ 
	if(Error1 != CRYS_OK)			                           
	{
	    PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));
	}
    }   

    /* .............. clearing the users context in case of error.......... */
    /* -------------------------------------------------------------------- */                           
    DX_VOS_MemSet(VerifyUserContext_ptr,0,sizeof(CRYS_ECDSA_VerifyUserContext_t));
   
    return Error;	

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_ECPKI_SUPPORT */                                     

}/* CRYS_ECDSA_VerifyUpdate */

				  

/**************************************************************************
 *	              CRYS_ECDSA_VerifyFinish function
 **************************************************************************/
/**
   @brief  Performs initialization of variables and structures, 
           calls the hash function for the last block of data (if necessary),  
   		   than calls LLF_ECDSA_VerifyCalcCall function for verifying signature  
		   according to EC DSA algorithm.

       NOTE: Using of HASH functions with HASH size great, than EC modulus size, 
			 is not recommended!	   

   @param[in] VerifyUserContext_ptr - A pointer to the user buffer for verifying the database.
   @param[in] SignatureIn_ptr       - A pointer to a buffer for the signature to be compared
   @param[in] SignatureSizeBytes    - The size of a user passed signature (must be 2*OrderSizeInBytes).

   @return <b>CRYSError_t</b>: <br>
              CRYS_OK <br>
			  CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR <br>
			  CRYS_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR <br>
			  CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_IN_PTR_ERROR <br>
			  CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR <br>
			  CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_SIZE_ERROR <br>			  
			  CRYS_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR <br>
			  CRYS_ECDSA_VERIFY_INCONSISTENT_VERIFY_ERROR <br>
**/
CEXPORT_C CRYSError_t CRYS_ECDSA_VerifyFinish( 
			CRYS_ECDSA_VerifyUserContext_t *VerifyUserContext_ptr, /*in*/ 
			DxUint8_t                      *SignatureIn_ptr,       /*in*/
			DxUint32_t                      SignatureSizeBytes      /*in*/  )
{
	/* FUNCTION DECLERATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* This pointer is allocated in order to remove compiler alias problems */
	void *tempWorkingctx_ptr;
	
	/* defining a pointer to the active context allcated by the CCM */
	ECDSA_VerifyContext_t     *ccmWorkingContext_ptr;
	
	/* pointer to public key structure in ccmWorkingContext  */
	CRYS_ECPKI_PublKey_t    *PublKey_ptr;
	
	/*  EC domain ID and pointer to the current domain */
	CRYS_ECPKI_DomainID_t    DomainID;   
	CRYS_ECPKI_DomainInfo_t  DomainInfo;
	/* Defining input arguments for LLF_ECDSA_VerifyCalcCall primitive */
	DxUint32_t              *MessageRepresent_ptr;      
	DxUint32_t               HashSizeWords;       						              
	DxUint32_t              *SignatureC_ptr, *SignatureD_ptr;          
	DxUint32_t              *TempBuff_ptr; 
	
	/* Order size in bytes */
	DxUint32_t    ModSizeInWords;
	DxUint32_t    OrderSizeInBytes, OrderSizeInWords;
	
	/* Shift count and index for shifting */
	DxUint32_t    Shift, i ; 
	
	
	/* FUNCTION LOGIC */
	
	/* .................. initializing local variables ................... */
	/* ------------------------------------------------------------------- */   
	
	/* initializing the Error to O.K */
	Error = CRYS_OK;      
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_ECPKI_UNSUPPORTED( VerifyUserContext_ptr, SignatureIn_ptr, SignatureSizeBytes, 
	Error,ccmWorkingContext_ptr, PublKey_ptr, DomainID, 
	DomainInfo.EC_ModulusSizeInBits, MessageRepresent_ptr, HashSizeWords, SignatureC_ptr, 
	SignatureD_ptr, TempBuff_ptr, ModSizeInWords, OrderSizeInBytes, 
	OrderSizeInWords, Shift, i, Error, Error, Error, Error); 
	
	#ifndef CRYS_NO_HASH_SUPPORT                                      
	#ifndef CRYS_NO_ECPKI_SUPPORT                                      
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */
	
	/* if the users context pointer is DX_NULL return an error */
	if( VerifyUserContext_ptr == DX_NULL )
		return CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR;
	
	/* if the users context TAG is illegal return an error - the context is invalid */
	if( VerifyUserContext_ptr->valid_tag != CRYS_ECDSA_VERIFY_CONTEXT_VALIDATION_TAG )
		return CRYS_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR;   
	
	/* if the users Signature pointer is illegal then return an error */
	if( SignatureIn_ptr == DX_NULL  )
		return CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_IN_PTR_ERROR; 
	
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, VerifyUserContext_ptr, sizeof(CRYS_ECDSA_VerifyUserContext_t)) || 
	DxCcAcl_IsBuffAccessOk(ACCESS_READ, SignatureIn_ptr, SignatureSizeBytes)){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	
	/* ................. get the ECDSA context    ............................. */
	/* ------------------------------------------------------------------------ */
	
	Error = CRYS_CCM_GetContext( VerifyUserContext_ptr, 	 /* the users context space - in */
				(void **) &tempWorkingctx_ptr,   /* the CCM context returned - out */
				DX_ECDSA_VERIFY_CONTEXT,               
				AES_DECRYPT_CONTEXT);	 	 /* need to decrypt context in AES_block*/   
	
	if( Error != CRYS_OK ) 
		return Error;  
	                                     
	ccmWorkingContext_ptr = (ECDSA_VerifyContext_t*)tempWorkingctx_ptr;
	
	if(ccmWorkingContext_ptr->HashMode >= CRYS_ECPKI_HASH_NumOfModes){
		Error = CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR;
		goto END_WITH_ERROR;  
	}
	
	/* ................. checking the validity of the prameters in the context ........ */
	/* -------------------------------------------------------------------------------- */
	
	PublKey_ptr = (CRYS_ECPKI_PublKey_t *)(void*)ccmWorkingContext_ptr->ECDSA_SignerPublKey.PublKeyDbBuff; 
	
	/*Initializing the DomainID, DomainInfo pointer and sizes of arrays */
	DomainID = PublKey_ptr->DomainID;   
	
	/* ...... Continue checking: check the EC domain ID.................... */
	if(DomainID >= CRYS_ECPKI_DomainID_OffMode){
		Error = CRYS_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR;
		goto END_WITH_ERROR;
	}
	
	/* Get EC Domain information from LLF level */
	Error = LLF_ECPKI_GetDomainInfo(DomainID, &DomainInfo);   
	
	if(Error != CRYS_OK) 
		goto END_WITH_ERROR;  
	
	ModSizeInWords    =  (DomainInfo.EC_ModulusSizeInBits + 31) /32;
	OrderSizeInBytes  =  (DomainInfo.EC_OrderSizeInBits + 7) / 8;
	OrderSizeInWords  =  (OrderSizeInBytes + 3) / 4;
	
	/* if the user signature size is not equal to 2*ModSizeInBytes, then return an error */
	if( SignatureSizeBytes != 2*OrderSizeInBytes ){
		Error = CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_SIZE_ERROR; 
		goto END_WITH_ERROR;
	}
	
	
	/*Operating the HASH Finish function only in case that Hash operation is needed*/
	if(ccmWorkingContext_ptr->HashMode <= CRYS_ECPKI_HASH_SHA512_mode){
		Error = CRYS_HASH_Finish(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
			 ccmWorkingContext_ptr->HASH_Result);   		    
		if(Error != CRYS_OK)
			goto END_WITH_ERROR;			                                 
		
		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 30 , (char *)" \n In ECDSA_VerifyFinish ; HASH_Result = " ,(DxUint8_t *)ccmWorkingContext_ptr->HASH_Result , ccmWorkingContext_ptr->HASH_Result_Size*4));       
	}
	
	/* execute the  ECVP_DSA primitive by calling the function CRYS_ECDSA_VerifyCalc   */
	/* ----------------------------------------------------------------------------- */
	
	/* ............ Initialization of  CRYS_ECDSA_VerifyCalc arguments ............... */
	
	HashSizeWords        = ccmWorkingContext_ptr->HASH_Result_Size;          						              
	
	/* Temp buffers */
	SignatureC_ptr       = ((LLF_ecdsa_verify_db_t *)(ccmWorkingContext_ptr->crysEcdsaVerIntBuff))->Tempbuff;
	SignatureD_ptr       = SignatureC_ptr + OrderSizeInWords; /* Max lengths of C in whole words */
	MessageRepresent_ptr = SignatureD_ptr + OrderSizeInWords;
	TempBuff_ptr         = MessageRepresent_ptr + OrderSizeInWords;
	
	/* Clean memory  */
	DX_VOS_MemSetZero(SignatureC_ptr, 2*4*OrderSizeInWords);
	DX_VOS_MemSetZero(MessageRepresent_ptr, 4*OrderSizeInWords);
	
	
	/* Derive message representative from HASH_Result:
	MessageRepresent = leftmost OrderSizeInBits bits of HASH_Result */
	
	if(DomainInfo.EC_OrderSizeInBits >= 32*HashSizeWords){  /* Copy HASH_Result into MessageRepresent buffer */
		Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(MessageRepresent_ptr, 4*OrderSizeInWords,
		(DxUint8_t*)(ccmWorkingContext_ptr->HASH_Result), 4*HashSizeWords); 
		if(Error != CRYS_OK)
			return Error;
	}
	
	else{
		/* Copy only OrderSizeInBytes bytes of HASH_Result into   *     
		*  MessageRepresent     				      */
		Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(MessageRepresent_ptr, 4*OrderSizeInWords,
		(DxUint8_t*)(ccmWorkingContext_ptr->HASH_Result), OrderSizeInBytes);
		if(Error != CRYS_OK)
			return Error;
		
		/* Shift right MessageRepresent by redundant bits */
		Shift = (8 - (DomainInfo.EC_OrderSizeInBits & 7UL)) & 7UL;
		
		if(Shift){        
			for(i = 0; i < OrderSizeInWords - 1; i++){
				MessageRepresent_ptr[i] = (MessageRepresent_ptr[i] >> Shift) | 
						      (MessageRepresent_ptr[i+1] << (32-Shift));
			}        	
	
			MessageRepresent_ptr[i] = MessageRepresent_ptr[i] >> Shift;
		}        
	}

 
	/* Convert signature data to words array with little entian order of  * 
	*  words							      */
	{
		Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
			SignatureC_ptr, 4*OrderSizeInWords, SignatureIn_ptr, OrderSizeInBytes);
		if(Error != CRYS_OK)
		    return Error;

		Error= CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
			SignatureD_ptr, 4*OrderSizeInWords, SignatureIn_ptr + OrderSizeInBytes, OrderSizeInBytes);
		if(Error != CRYS_OK)
		    return Error;
	}

	/*---------------------------------------------------------------------------*/ 
        /* Perform verifying operation  - call the LLF_ECDSA_VerifyCalcCall function */
	/*---------------------------------------------------------------------------*/
	 
	Error =  LLF_ECDSA_VerifyCalcCall(PublKey_ptr, MessageRepresent_ptr, 						           						              
				       SignatureC_ptr, SignatureD_ptr, TempBuff_ptr);
	if( Error != CRYS_OK )
	    	goto END_WITH_ERROR;
	

	/* ................. release the context ................................. */
	/* ----------------------------------------------------------------------- */
	
	Error = CRYS_CCM_ReleaseContext(VerifyUserContext_ptr,		/* the users context space - in */
				    ccmWorkingContext_ptr,              /* the CCM context returned - in */
				    DX_ECDSA_VERIFY_CONTEXT);           /* ECDSA type - in */ 
	return Error;

 END_WITH_ERROR: 
 
	/* ................. release the context ................................. */
	/* ----------------------------------------------------------------------- */
	{    
		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(VerifyUserContext_ptr, /* the users context space - in */
					    ccmWorkingContext_ptr,      /* the CCM context returned - in */
					    DX_ECDSA_VERIFY_CONTEXT);   /* ECDSA type - in */ 
			      
		if(Error1 != CRYS_OK){
			PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" ,Error1));
		}
	}	
	
   /* .............. clearing the users context in case of error.......... */
   /* -------------------------------------------------------------------- */                           
   DX_VOS_MemSetZero(VerifyUserContext_ptr,sizeof(CRYS_ECDSA_VerifyUserContext_t));
   
   return Error;	

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_ECPKI_SUPPORT */                                     
				    
}/* End DX_ECDSA_VerifyFinish */


/**************************************************************************
 *	              CRYS_ECDSA_Verify integrated function
 **************************************************************************/
/**
   @brief  Performs all ECDSA verifying operations simultaneously. 
		   
	   This function simply calls the Init, Update and Finish functions continuously.  
   	   This function's prototype is similar to the prototypes of the called functions 
   	   and includes all of their input and output arguments.
   	   
       NOTE: Using of HASH functions with HASH size great, than EC modulus size, 
			 is not recommended!
   	   

   @param[in]  VerifyUserContext_ptr - A pointer to the user buffer for verifying database.
   @param[in]  UserPublKey_ptr       - A pointer to a user public key structure.
   @param[in]  HashMode              - The enumerator variable defines the hash function to be used.
   @param[in]  MessageDataIn_ptr     - Message data for calculating hash.   			
   @param[in]  MessageSizeInBytes    - Size of block of message data in bytes.       
   @param[in]  SignatureIn_ptr       - A pointer to a buffer for output of signature. 
   @param[in]  SignatureSizeBytes    - Size of signature, in bytes (must be 2*OrderSizeInBytes).
                                       
   @return <b>CRYSError_t</b>: <br> 
			CRYS_OK <br>
			CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR <br> 
			CRYS_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR <br>
			CRYS_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR <br>
			CRYS_ECDSA_VERIFY_INVALID_SIGNER_PUBL_KEY_PTR_ERROR <br>
			CRYS_ECDSA_VERIFY_SIGNER_PUBL_KEY_VALIDATION_TAG_ERROR <br>
			CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR <br>                
			CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_PTR_ERROR <br>
			CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR <br>						                        
			CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_IN_PTR_ERROR <br>						 
			CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_SIZE_ERROR <br>
			CRYS_ECDSA_VERIFY_INCONSISTENT_VERIFY_ERROR <br> 
**/
CEXPORT_C CRYSError_t CRYS_ECDSA_Verify ( 
			CRYS_ECDSA_VerifyUserContext_t *VerifyUserContext_ptr, /*in/out*/
			CRYS_ECPKI_UserPublKey_t       *UserPublKey_ptr,        /*in*/
			CRYS_ECPKI_HASH_OpMode_t        HashMode,               /*in*/
			DxUint8_t                      *SignatureIn_ptr,        /*in*/
			DxUint32_t                      SignatureSizeBytes,     /*in*/
			DxUint8_t                      *MessageDataIn_ptr,      /*in*/ 
			DxUint32_t                      MessageSizeInBytes      /*in*/ )
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

    RETURN_IF_ECPKI_UNSUPPORTED( VerifyUserContext_ptr ,  UserPublKey_ptr , HashMode , 
				MessageDataIn_ptr , MessageSizeInBytes , SignatureIn_ptr , 
				SignatureSizeBytes , Error , Error , Error , Error , 
				Error , Error , Error , Error , Error , Error , Error ,
				Error , Error , Error , Error); 
                              
    #ifndef CRYS_NO_HASH_SUPPORT                                      
    #ifndef CRYS_NO_ECPKI_SUPPORT                                      
 
	/*****  CRYS_ECDSA_VerifyInit  ********/

    Error = CRYS_ECDSA_VerifyInit( VerifyUserContext_ptr, UserPublKey_ptr, HashMode );

    if(Error!=CRYS_OK)
	     return Error;

	/*****  CRYS_ECDSA_VerifyUpdate  ********/

    Error = CRYS_ECDSA_VerifyUpdate(VerifyUserContext_ptr, MessageDataIn_ptr, 
				MessageSizeInBytes);
    if(Error!=CRYS_OK)
	
		 return Error;
    

	/*****  CRYS_ECDSA_VerifyFinish  ********/

	Error = CRYS_ECDSA_VerifyFinish(VerifyUserContext_ptr, SignatureIn_ptr,    
				SignatureSizeBytes);  	
   return Error;

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_ECPKI_SUPPORT */                                     

}/* END OF CRYS_ECDSA_Verify */

