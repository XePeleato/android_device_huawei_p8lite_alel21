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
#include "crys_ccm.h"
#include "plat_print_dbg.h"
#include "crys_bypass_api.h"
#include "cc_acl.h"
#include "crys_common_math.h"
#include "crys_ecpki_error.h"
#include "crys_ecpki_local.h"
#include "crys_ecpki_ecdsa.h"

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
 *	              CRYS_ECDSA_Sign_Init function
 **************************************************************************/
/**
   \brief    
   The CRYS_ECDSA_Sign_Init functions user shall call first to perform the 
   EC DSA Signing operation.

   The function performs the following steps:
   -# Validates all the inputs of the function. If one of the received 
      parameters is not valid, the function returns an error. 
   -# Decrypts the received context to the working context after capturing 
      the working context by calling the CRYS_CCM_GetContext() function.
   -# Initializes the working context and other variables and structures. 
   -# Calls the CRYS_HASH_Init() function.
   -# Calls the CRYS_CCM_EncryptAndReleaseContext() function to encrypt 
      the information in the working context, store it in the user's 
      received context, and then release the working context.
   -# Exits the handler with the OK code.

   This function does not do ECDSA cryptographic processing. Rather, it
   prepares a context that is used by the Update() and Finish() functions.
   
   NOTE: Using of HASH functions with HASH size great, than EC modulus size, is not recommended!
   
   
   @param[in,out] SignUserContext_ptr A pointer to the user buffer for signing data.
   @param[in] 	  SignerPrivKey_ptr   A pointer to the private key that will be used to 
                                      sign the data.
   @param[in]     HashMode            Defines the hash mode used for DSA.

   @return <b>CRYSError_t</b>: <br> 
			 CRYS_OK<br>
			 CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR
			 CRYS_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR
			 CRYS_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR
			 CRYS_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR
			 CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR		 
*/
CEXPORT_C CRYSError_t CRYS_ECDSA_SignInit( 
				CRYS_ECDSA_SignUserContext_t  *SignUserContext_ptr, /*in/out*/
				CRYS_ECPKI_UserPrivKey_t      *SignerPrivKey_ptr,   /*in*/
				CRYS_ECPKI_HASH_OpMode_t       HashMode             /*in*/ )
{
	/* FUNCTION DECLERATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* This pointer is allocated in order to remove compiler alias problems */
	void *tempWorkingctx_ptr;
	
	/* defining a pointer to the active context allcated by the CCM */
	ECDSA_SignContext_t *ccmWorkingContext_ptr;         
	
	/* pointer to private key structure inside the SignerPrivKey  */
	CRYS_ECPKI_PrivKey_t    *PrivKey_ptr;
	
	/* FUNCTION LOGIC */
	
	/* ............... local initializations .............................. */
	/* -------------------------------------------------------------------- */
	
	/* initializing the Error to O.K */
	Error = CRYS_OK;
	
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_ECPKI_UNSUPPORTED( SignUserContext_ptr, SignerPrivKey_ptr, HashMode , 
				ccmWorkingContext_ptr, PrivKey_ptr, Error,   
				Error, Error, Error, Error, Error,
				Error, Error, Error, Error, Error,
				Error, Error, Error, Error, Error, Error); 
                              
	#ifndef CRYS_NO_HASH_SUPPORT                                      
	#ifndef CRYS_NO_ECPKI_SUPPORT                                      
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */
	
	/* if the users context ID pointer is DX_NULL return an error */
	if(SignUserContext_ptr == DX_NULL )   
		return CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR;
	
	/*if the private key pointer is DX_NULL or its validation tag is not valid return an error*/   
	if (SignerPrivKey_ptr == DX_NULL)
		return CRYS_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR;
		  
	/* check if the hash operation mode is legal */
	if(HashMode >= CRYS_ECPKI_HASH_NumOfModes)
		return CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR;       
	
	if(DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, SignUserContext_ptr, sizeof(CRYS_ECDSA_SignUserContext_t)))
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	
	if(SignerPrivKey_ptr->valid_tag != CRYS_ECPKI_PRIV_KEY_VALIDATION_TAG)
		return CRYS_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR;
	
	/* initialize the priv key pointer */
	PrivKey_ptr = (CRYS_ECPKI_PrivKey_t*)(void*)(SignerPrivKey_ptr->PrivKeyDbBuff);
	
	/* check the EC domain ID  */
	if(PrivKey_ptr->DomainID >= CRYS_ECPKI_DomainID_OffMode )
		return CRYS_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR;


	/* ................. acquiring the ECDSA context ............................. */
	/* ------------------------------------------------------------------------ */   
	
	Error = CRYS_CCM_GetContext( SignUserContext_ptr,   				 /* the users context space - in */
				(void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
				DX_ECDSA_SIGN_CONTEXT,               /* AES type - in */ 
				AES_DONT_DECRYPT_CONTEXT);			 /* need to decrypt context in AES_block*/   
	
	if( Error != CRYS_OK )    
		return Error;    
	ccmWorkingContext_ptr = (ECDSA_SignContext_t*)tempWorkingctx_ptr;                           
	
	/*Reset the Context handler for improper previous values initialized*/
	DX_VOS_MemSet(ccmWorkingContext_ptr, 0, sizeof(ECDSA_SignContext_t));
			     
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
		Error = CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR;
		goto END_WITH_ERROR;		 		
	}  
	
	/*Copying the ECPKI Private key value to the context*/       
	DX_VOS_FastMemCpy((DxUint8_t *)&ccmWorkingContext_ptr->ECDSA_SignerPrivKey, 
		         (DxUint8_t*)SignerPrivKey_ptr, sizeof(CRYS_ECPKI_UserPrivKey_t));
	
	/* set the ECDSA tag to the users context */
	SignUserContext_ptr->valid_tag = CRYS_ECDSA_SIGN_CONTEXT_VALIDATION_TAG;
	
	
	/* ................. release the context ................................. */
	/* ----------------------------------------------------------------------- */
	
	Error = CRYS_CCM_ReleaseContext(SignUserContext_ptr,			/* the users context space - in */
					   ccmWorkingContext_ptr,       /* the CCM context returned - in */
					   DX_ECDSA_SIGN_CONTEXT);      /* ECDSA SIGN type - in */ 
	return Error;
	
	END_WITH_ERROR:
	/* ................. release the context ................................. */
	/* ----------------------------------------------------------------------- */
	{   
		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(
		SignUserContext_ptr,		 /* the users context space - in */
		ccmWorkingContext_ptr,       /* the CCM context returned - in */
		DX_ECDSA_SIGN_CONTEXT);        /* ECDSA type - in */ 
		
		if(Error1 != CRYS_OK){
			PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));
		}
	}   
	
	/* .............. clearing the users context in case of error.......... */
	/* -------------------------------------------------------------------- */                           
	DX_VOS_MemSet(SignUserContext_ptr,0,sizeof(CRYS_ECDSA_SignUserContext_t));
	
	return Error;	
	
	#endif /* !CRYS_NO_HASH_SUPPORT */
	#endif /* !CRYS_NO_ECPKI_SUPPORT */                                     

}/* _DX_ECDSA_SignInit */



/**************************************************************************
 *	              CRYS_ECDSA_Sign_Update function
 **************************************************************************/
/**
   @brief  Performs a hash  operation on data allocated by the user 
           before finally signing it.
  
   	   In case user divides signing data by block, he must call the Update function 
   	   continuously a number of times until processing of the entire data block is complete.
 
       NOTE: Using of HASH functions with HASH size great, than EC modulus size, 
             is not recommended!

   @param[in,out] SignUserContext_ptr A pointer to the user buffer for signing the database.
   @param[in] 	  MessageDataIn_ptr   Message data for calculating Hash.   	
   @param[in]     DataInSize          The size of the message data block, in bytes.
   
   @return <b>CRYSError_t</b>: <br> 
			 CRYS_OK<br>
           		 CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR
                         CRYS_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR
			 CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR 
			 CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR
			 CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR
		 										 
 */
CEXPORT_C CRYSError_t CRYS_ECDSA_SignUpdate( 
				CRYS_ECDSA_SignUserContext_t  *SignUserContext_ptr,  /*in/out*/
				DxUint8_t                     *MessageDataIn_ptr,    /* in */ 
				DxUint32_t                     DataInSize            /* in */ )
{
   /* FUNCTION DECLERATIONS */
   
   /* The return error identifier */
   CRYSError_t Error;

   /* This pointer is allocated in order to remove compiler alias problems */
   void *tempWorkingctx_ptr;

   /* defining a pointer to the active context allcated by the CCM */
   ECDSA_SignContext_t *ccmWorkingContext_ptr;
      
   /* FUNCTION LOGIC */
   
   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */   
   
   /* initializing the Error to O.K */
   Error = CRYS_OK;      

   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_ECPKI_UNSUPPORTED( SignUserContext_ptr , MessageDataIn_ptr , DataInSize , 
                              ccmWorkingContext_ptr , Error , Error , 
                              Error , Error , Error , Error , Error , Error ,                              
                              Error , Error , Error , Error , Error , Error , Error , 
							  Error , Error , Error); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_ECPKI_SUPPORT                                      
   
   /* ............... checking the parameters validity ................... */
   /* -------------------------------------------------------------------- */
   
   /* if the users context pointer is DX_NULL return an error */
   if( SignUserContext_ptr == DX_NULL )
      return CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR;
      
   /* if the users context TAG is illegal return an error - the context is invalid */
   if( SignUserContext_ptr->valid_tag != CRYS_ECDSA_SIGN_CONTEXT_VALIDATION_TAG )
      return CRYS_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR;   
   
   /* if the users MessageDataIn pointer is illegal return an error */
   if( MessageDataIn_ptr == DX_NULL && DataInSize )
      return CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR;

   /* if the data size is larger then 2^29 (to prevent an overflow on the transition to bits ) 
      return error */
   if( DataInSize >= (1UL << 29) )
      return CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;    
            
   /* MessageDataIn_ptr can be smart pointer but it is tested either in the hash or in bypass operations */  
   if(DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, SignUserContext_ptr, sizeof(CRYS_ECDSA_SignUserContext_t)) ||
      DxCcAcl_IsBuffAccessOk(ACCESS_READ, MessageDataIn_ptr, DataInSize)){
	   return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
   }
      
   /* ................. acquiring the Sign context ........................... */
   /* ------------------------------------------------------------------------ */
   
   Error = CRYS_CCM_GetContext( SignUserContext_ptr,       		     /* the users context space - in */
                                (void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
                                DX_ECDSA_SIGN_CONTEXT,               
                                AES_DECRYPT_CONTEXT);			 	 /* need to decrypt context in AES_block*/   
    
   if( Error != CRYS_OK ) 
     return Error;                                       
   ccmWorkingContext_ptr = (ECDSA_SignContext_t*)tempWorkingctx_ptr;

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
        
		if(DataInSize != CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES)
		{	/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}
		/* Copy the DataIn_ptr to the HashResult */	   			
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize, (DxUint8_t*)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
		
		break;
		   		
 		   		
    	case CRYS_ECPKI_AFTER_HASH_SHA224_mode:
    			
		if(DataInSize != CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}

		/* Copy the DataIn_ptr to the HashResult */	   			
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t*)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 				
		
		break;
		   		
    	case CRYS_ECPKI_AFTER_HASH_SHA256_mode:
		
		if(DataInSize != CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}
		/* Copy the DataIn_ptr to the HashResult */	   							
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t*)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
		
		break;
		   		
    	case CRYS_ECPKI_AFTER_HASH_SHA384_mode:
    			
		if(DataInSize != CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}
		/* Copy the DataIn_ptr to the HashResult */	   			
		
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t*)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
		
		break;
		   		
    	case CRYS_ECPKI_AFTER_HASH_SHA512_mode:
		   		
		if(DataInSize != CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES){	
			/* DataInSize must fit exactly to the size of Hash output that we support */
			Error = CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
			goto END_WITH_ERROR;
		}
		/* Copy the DataIn_ptr to the HashResult */	   			
		
		Error = CRYS_Bypass(MessageDataIn_ptr,DataInSize,(DxUint8_t*)ccmWorkingContext_ptr->HASH_Result);
		if (Error != CRYS_OK)
			goto END_WITH_ERROR; 
		
		break;
		
        default:
		Error = CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR;
		goto END_WITH_ERROR;
    }


    /* ................. release the context ................................. */
    /* ----------------------------------------------------------------------- */
   
    Error = CRYS_CCM_ReleaseContext(SignUserContext_ptr,				/* the users context space - in */
	 	                            ccmWorkingContext_ptr,              /* the CCM context returned - in */
	 	                            DX_ECDSA_SIGN_CONTEXT);             /* ECDSA type - in */ 
    return Error;
    
	
    END_WITH_ERROR:
    /* ................. release the context ................................. */
    /* ----------------------------------------------------------------------- */
    {   
 	CRYSError_t Error1 = CRYS_CCM_ReleaseContext(SignUserContext_ptr,	 /* the users context space - in */
						 ccmWorkingContext_ptr,  /* the CCM context returned - in */
						 DX_ECDSA_SIGN_CONTEXT); /* ECDSA  type - in */ 
 	if(Error1 != CRYS_OK)	                           
        {
    		PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));
        }
    }   

    /* .............. clearing the users context in case of error.......... */
    /* -------------------------------------------------------------------- */                           
    DX_VOS_MemSet(SignUserContext_ptr,0,sizeof(CRYS_ECDSA_SignUserContext_t));
   
    return Error;	

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_ECPKI_SUPPORT */                                     

}/* CRYS_ECDSA_SignUpdate */




/**************************************************************************
 *	              _DX_ECDSA_Sign_Finish function
 **************************************************************************/
/** 
   @brief  Performs initialization of variables and structures, calls the hash function  
           for the last block of data (if necessary) and then calculates digital 
		   signature according to the EC DSA algorithm.
   	   
   	   NOTE: Using of HASH functions with HASH size great, than EC modulus size, 
	         is not recommended!

   @param[in] SignUserContext_ptr      A pointer to the user buffer for signing database.
   @param[in] SignatureOut_ptr         A pointer to a buffer for output of signature.
   @param[in,out] SignatureOutSize_ptr The size of a user passed buffer for signature (in)
                                       and size of actual signature (out). The size of buffer 
					must be not less than 2*OrderSizeInBytes.
   @param[out] IsEphemerKeyInternal    A parameter defining whether the ephemeral key 
                                       is internal or external (1 or 0).
   @param[out] EphemerKeyData_ptr      A pointer to external ephemeral key data buffer of size 
					not less, than size of CRYS_ECPKI_KG_TempData_t. The buffer must
                                        contain at the first place the ephemeral private key, 
					given as big endian bytes array.
   @return <b>CRYSError_t</b>: <br> 
			 CRYS_OK<br>
                         CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR <br>
			 CRYS_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR <br>
			 CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR <br>
			 CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR <br>
			 CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR <br>
			 CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR <br>
			 CRYS_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR <br>
			 CRYS_ECDSA_SIGN_INVALID_IS_EPHEMER_KEY_INTERNAL_ERROR <br>
			 CRYS_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR <br>
**/
CEXPORT_C  CRYSError_t _DX_ECDSA_SignFinish( 
				CRYS_ECDSA_SignUserContext_t   *SignUserContext_ptr, /*in*/ 
				DxUint8_t                      *SignatureOut_ptr,    /*out*/
				DxUint32_t                     *SignatureOutSize_ptr,/*in/out*/
				DxInt8_t                       IsEphemerKeyInternal, /*in*/
				DxUint32_t                     *EphemerKeyData_ptr   /*in*/ ) 
{
	/* FUNCTION DECLERATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* This pointer is allocated in order to remove compiler alias problems */
	void *tempWorkingctx_ptr;
	
	/* defining a pointer to the active context allcated by the CCM */
	ECDSA_SignContext_t     *ccmWorkingContext_ptr;
	
	/* pointer to private key structure in ccmWorkingContext  */
	CRYS_ECPKI_PrivKey_t    *PrivKey_ptr;
	
	/*  EC domain ID and pointer to the current domain info */
	CRYS_ECPKI_DomainID_t    DomainID;   
	CRYS_ECPKI_DomainInfo_t  DomainInfo;
	/* Modulus and Order sizes  */
	DxUint32_t     ModSizeInWords;
	DxUint32_t     OrderSizeInBytes, OrderSizeInWords;
	
	/* Pointers to arguments for CRYS_ECDSA_SignCalc() primitive */
	DxUint32_t     *SignatureC_ptr, *SignatureD_ptr;
	DxUint32_t     *MessageRepresent_ptr;
	
	DxUint32_t      HashSizeWords; 
	
	/* Shift count and index for shifting */
	DxUint32_t      Shift, i ; 
	  
	DxUint32_t     *TempBuff_ptr;    
	
	/* FUNCTION LOGIC */
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_ECPKI_UNSUPPORTED( SignUserContext_ptr , SignatureOut_ptr , SignatureOutSize_ptr , 
			      IsEphemerKeyInternal , EphemerKeyData_ptr , ccmWorkingContext_ptr ,  
			      PrivKey_ptr , DomainID , DomainInfo.EC_ModulusSizeInBits , MessageRepresent_ptr, 
			      HashSizeWords , SignatureC_ptr , SignatureD_ptr , TempBuff_ptr , 
			      ModSizeInWords , OrderSizeInBytes  , OrderSizeInWords , Shift ,
				i , Error , Error , Error ); 
			      
	#ifndef CRYS_NO_HASH_SUPPORT                                      
	#ifndef CRYS_NO_ECPKI_SUPPORT                                      
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */
	
	/* if the users context pointer is DX_NULL return an error */
	if( SignUserContext_ptr == DX_NULL )
		return CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR;
	
	/* if the users context tag is illegal return an error - the context is invalid */
	if( SignUserContext_ptr->valid_tag != CRYS_ECDSA_SIGN_CONTEXT_VALIDATION_TAG )
		return CRYS_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR;   
	
	/* if the users SignatureOut pointer is illegal return an error */
	if( SignatureOut_ptr == DX_NULL  )
		return CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR;     
	
	/* if the users SignatureOutSize pointer is illegal return an error */
	if( SignatureOutSize_ptr == DX_NULL  )
		return CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR;     
	
	/* if user set an illegal IsEphemerKeyInternal value or illegal pointer return an error */
	if( IsEphemerKeyInternal != 1 && IsEphemerKeyInternal != 0  )
		return CRYS_ECDSA_SIGN_INVALID_IS_EPHEMER_KEY_INTERNAL_ERROR;  
	
	if( IsEphemerKeyInternal == 0 && EphemerKeyData_ptr == DX_NULL )
		return CRYS_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR;
	
	if(DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, SignUserContext_ptr, sizeof(CRYS_ECDSA_SignUserContext_t)) ||
	   DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, SignatureOut_ptr, *SignatureOutSize_ptr) ||
	   DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, SignatureOutSize_ptr, sizeof(DxUint32_t)) ||
	   ((EphemerKeyData_ptr != DX_NULL) && DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, EphemerKeyData_ptr, sizeof(DxUint32_t)))){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/* ................. acquiring the ECDSA context .......................... */
	/* ------------------------------------------------------------------------ */
	
	Error = CRYS_OK;     /* initializing the Error to O.K */
	
	Error = CRYS_CCM_GetContext( SignUserContext_ptr,       		 /* the users context space - in */
				(void **) &tempWorkingctx_ptr,    /* the CCM context returned - out */
				DX_ECDSA_SIGN_CONTEXT,               
				AES_DECRYPT_CONTEXT);			 	 /* need to decrypt context in AES_block*/   
	
	if( Error != CRYS_OK ) 
		return Error; 
					  
	ccmWorkingContext_ptr = (ECDSA_SignContext_t*)tempWorkingctx_ptr;
	
	if( ccmWorkingContext_ptr->HashMode >= CRYS_ECPKI_HASH_NumOfModes ){
		Error = CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR;
		goto END_WITH_ERROR;  
	}    
	
	/* ................. checking the validity of the prameters in the context ........ */
	/* -------------------------------------------------------------------------------- */
	 
	PrivKey_ptr = (CRYS_ECPKI_PrivKey_t *)(void*)ccmWorkingContext_ptr->ECDSA_SignerPrivKey.PrivKeyDbBuff; 
	
	/*Initializing the DomainID, domain Pointer and Order Size in Bytes*/
	DomainID = PrivKey_ptr->DomainID;
	
	/* ...... Continue checking: check the EC domain ID .......... */
	if( DomainID >= CRYS_ECPKI_DomainID_OffMode ) 
	{
		Error = CRYS_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR;
		goto END_WITH_ERROR;
	}
	/* Get EC Domain information from LLF level */
	Error = LLF_ECPKI_GetDomainInfo(DomainID, &DomainInfo);
	
	if( Error != CRYS_OK ) 
		 goto END_WITH_ERROR;  
	
	/* Modulus sizes */ 
	ModSizeInWords   =  (DomainInfo.EC_ModulusSizeInBits + 31) / 32;
	OrderSizeInBytes =  (DomainInfo.EC_OrderSizeInBits + 7) / 8;
	OrderSizeInWords =  (DomainInfo.EC_OrderSizeInBits + 31) / 32;
	
	HashSizeWords        = ccmWorkingContext_ptr->HASH_Result_Size; 
	
	/* Temp buffers */
	MessageRepresent_ptr = ((LLF_ecdsa_sign_db_t *)(ccmWorkingContext_ptr->crysEcdsaSignIntBuff))->Tempbuff;
	SignatureC_ptr       = MessageRepresent_ptr + OrderSizeInWords;
	SignatureD_ptr       = SignatureC_ptr + OrderSizeInWords; /* Max lengths of C in whole words */
	TempBuff_ptr         = SignatureD_ptr + OrderSizeInWords;
	
	/* If the received output buffer is small than 2*OrderSizeInBytes then return an error */
	if(*SignatureOutSize_ptr < 2*OrderSizeInBytes ){
		*SignatureOutSize_ptr = 2*OrderSizeInBytes;
		Error = CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR;      
		
		goto END_WITH_ERROR_WITHOUT_CLEAN_CONTEXT;/*give the user second changce*/
	}   
	
	
	/*Operating the HASH Finish function only in case that Hash operation is needed*/
	if(ccmWorkingContext_ptr->HashMode <= CRYS_ECPKI_HASH_SHA512_mode)
	{
		Error = CRYS_HASH_Finish(((CRYS_HASHUserContext_t *)(ccmWorkingContext_ptr->CRYSPKAHashCtxBuff)),
					 ccmWorkingContext_ptr->HASH_Result);   		    
		if( Error != CRYS_OK ) 
			goto END_WITH_ERROR;			                                 
		
		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 30 , (char *)" \n In ECDSA_SignFinish ; HASH_Result = " ,
			     (DxUint8_t*)ccmWorkingContext_ptr->HASH_Result , ccmWorkingContext_ptr->HASH_Result_Size*4));       
	}
	
	
	/* Derive message representative from HASH_Result: MessageRepresent =
	   leftmost OrderSizeInBits bits of HASH_Result */
	
	/* Set 0 to MessageRepresent buffer of length ModSizeInWords + 1 */
	DX_VOS_MemSetZero(MessageRepresent_ptr, 4*OrderSizeInWords);
	
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

	/*********  CALL LLF ECDSA Sinature function  *****************************************/
	
	Error =  LLF_ECDSA_SignCalcCall(PrivKey_ptr, MessageRepresent_ptr, 
					min(OrderSizeInWords, HashSizeWords),
					IsEphemerKeyInternal,  EphemerKeyData_ptr,
					SignatureC_ptr, SignatureD_ptr, TempBuff_ptr);
	if(Error!=CRYS_OK) 
		goto END_WITH_ERROR;
	
	/*********** Switch output according to DomainID ****************************/

	/* Output the reversed C,D strings of length OrderSizeInBytes */
	Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
		SignatureOut_ptr, 4*OrderSizeInWords,
		SignatureC_ptr, OrderSizeInBytes);
	if(Error != CRYS_OK)
	    return Error;

	Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
		SignatureOut_ptr + OrderSizeInBytes, 4*OrderSizeInWords,
		SignatureD_ptr, OrderSizeInBytes);
	if(Error != CRYS_OK)
	    return Error;
	
	*SignatureOutSize_ptr = 2*OrderSizeInBytes;                               
	
	goto END;
	
	END_WITH_ERROR_WITHOUT_CLEAN_CONTEXT:
	/* ................. release the context ................................. */
	/* ----------------------------------------------------------------------- */
	{   
		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(SignUserContext_ptr,	 /* the users context space - in */
						     ccmWorkingContext_ptr,  /* the CCM context returned - in */
						     DX_ECDSA_SIGN_CONTEXT); /* ECDSA type - in */ 
		
		if(Error1 != CRYS_OK){
			PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));
		}
	}
	
	return Error;	


END:	/*On finish operation need to memset the context to zero so END and END_WITH_ERROR are the same*/
END_WITH_ERROR: 
 
	/* ................. release the context ................................. */
	/* ----------------------------------------------------------------------- */
	{   
		CRYSError_t Error1 = CRYS_CCM_ReleaseContext(SignUserContext_ptr, /* the users context space - in */
						     ccmWorkingContext_ptr,    /* the CCM context returned - in */
						     DX_ECDSA_SIGN_CONTEXT);   /* ECDSA type - in */ 
		if(Error1 != CRYS_OK){
			PLAT_LOG_DEV_PRINT(( 10 ," \n Error1 from CRYS_CCM_ReleaseContext is %lX \n" , Error1 ));                              
		}
	}

	/* .............. clearing the users context in case of error.......... */
	/* -------------------------------------------------------------------- */                           
	DX_VOS_MemSet(SignUserContext_ptr,0,sizeof(CRYS_ECDSA_SignUserContext_t));
	
	return Error;	

#endif /* !CRYS_NO_HASH_SUPPORT */
#endif /* !CRYS_NO_ECPKI_SUPPORT */                                     
				    
}/* _DX_ECDSA_SignFinish */



/**************************************************************************
 *	              CRYS_ECDSA_Sign - integrated function
 **************************************************************************/
/**
   @brief  Performs all of the ECDSA signing operations simultaneously.
           This function simply calls the Init, Update and Finish functions continuously.  
   	       This function's prototype is similar to the prototypes of the called functions 
   	       and includes all of their input and output arguments.
   
   NOTE: Using of HASH functions with HASH size great, than EC modulus size, is not recommended!

   @param[in,out] SignUserContext_ptr - A pointer to the user buffer for signing database.
   @param[in]     SignerPrivKey_ptr   - A pointer to a user private key structure.    				
   @param[in]     HashMode            - The enumerator variable defines hash function to be used.                         
   @param[in] 	  MessageDataIn_ptr   - A message data for calculation of hash.   			
   @param[in]     MessageSizeInBytes  - A size of block of message data in bytes. 
   @param[in]     SignatureOut_ptr    - A pointer to a buffer for output of signature.                         
   @param[in,out] SignatureOutSize_ptr- Size of user passed buffer for signature (in)
                                        and size of actual signature (out). The size of buffer 
									    must be not less than 2*OrderSizeInBytes.
   @return <b>CRYSError_t</b>: <br> 
		CRYS_OK<br>
		CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR<br>
		CRYS_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR<br>
		CRYS_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR<br>
		CRYS_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR<br>
		CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR <br>                         
		CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR<br> 
		CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR<br>
		CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR<br>
		CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR<br>
		CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR<br>			 
		CRYS_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR <br>			 
**/
CEXPORT_C CRYSError_t CRYS_ECDSA_Sign(
					CRYS_ECDSA_SignUserContext_t  *SignUserContext_ptr,     /*in/out*/
					CRYS_ECPKI_UserPrivKey_t      *SignerPrivKey_ptr,       /*in*/
					CRYS_ECPKI_HASH_OpMode_t       HashMode,                /*in*/
					DxUint8_t                     *MessageDataIn_ptr,       /*in*/ 
					DxUint32_t                     MessageSizeInBytes,      /*in*/
					DxUint8_t                     *SignatureOut_ptr,        /*out*/ 
					DxUint32_t                    *SignatureOutSize_ptr     /*in*/) 
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

    RETURN_IF_ECPKI_UNSUPPORTED( SignUserContext_ptr ,  SignerPrivKey_ptr , HashMode , 
                                 MessageDataIn_ptr , MessageSizeInBytes , SignatureOut_ptr , 
				 SignatureOutSize_ptr , Error , Error , Error , Error , Error , Error ,
                                 Error , Error , Error , Error , Error , Error , Error , Error , Error); 
                              
    #ifndef CRYS_NO_HASH_SUPPORT                                      
    #ifndef CRYS_NO_ECPKI_SUPPORT                                      
 
	/*****  CRYS_ECDSA_SignInit  ********/

    Error = CRYS_ECDSA_SignInit( SignUserContext_ptr, SignerPrivKey_ptr, HashMode );

    if(Error!=CRYS_OK)
	     return Error;

	/*****  CRYS_ECDSA_SignUpdate  ********/

    Error = CRYS_ECDSA_SignUpdate(SignUserContext_ptr, MessageDataIn_ptr, 
		                MessageSizeInBytes);
    if(Error!=CRYS_OK)
	
		 return Error;    

	/*****  CRYS_ECDSA_SignFinish  ********/

	Error = CRYS_ECDSA_SignFinish(SignUserContext_ptr, SignatureOut_ptr,    
				SignatureOutSize_ptr);  	
   return Error;

   #endif /* !CRYS_NO_HASH_SUPPORT */
   #endif /* !CRYS_NO_ECPKI_SUPPORT */                                     

}/* END OF CRYS_ECDSA_Sign */


