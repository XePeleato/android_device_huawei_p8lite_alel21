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
#include "crys_ccm.h"
#include "crys_ccm_error.h"


/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */
 
/************************ Enums ********************************/

/************************ Typedefs *****************************/

/************************ Global Data **************************/

/* ********************************************************************** 
 *
 * implementation when we are not encrypting and decrypting the received context
 * from the user , we are actually passing the internal context buffer of the context allocated
 * by the user - on this method there is no protection on the context and no usage of global data 
 *
 * ********************************************************************** */   

//#if defined(CRYS_NO_CONTEXT_ENCRYPTION_PROTECTION) || defined(CRYS_NO_AES_SUPPORT) 

 /* ------------------------------------------------------------
 **
 * @brief This function does returnees the pointer on the internal context buffer.
 *
 * @param[in] UserContext_ptr - The users context pointer.
 * @param[in] Decrypt_flag - Weather or not to make a decrypt operation. e.g. in AES_Init a decrypt is not needed.
 * @param[out] CRYS_GlobalContext_ptr - The returned pointer of the allocated context.
 * @param[out] Type - The context type.
 *
 * @return CRYSError_t - On success CRYS_OK.
 */

 CRYSError_t CRYS_CCM_GetContext(void *UserContext_ptr,void ** CRYS_GlobalContext_ptr,ContextType_t Type,DxUint8_t AES_Dec_Enc_flag)
{
 
	/* LOCAL DECLERATIONS */

	/* error identifier */	
	CRYSError_t Error = CRYS_OK;
	/* FUNCTION LOGIC */

#ifndef CRYS_NO_PKI_SUPPORT    
	CRYS_RSAPrivUserContext_t *RSAPriv_user_context_ptr;
	CRYS_RSAPubUserContext_t  *RSAPub_user_context_ptr;
#endif

#ifndef CRYS_NO_ECPKI_SUPPORT  
	CRYS_ECDSA_SignUserContext_t    *ECDSA_sign_user_context_ptr;
	CRYS_ECDSA_VerifyUserContext_t  *ECDSA_verify_user_context_ptr;
#endif /* CRYS_NO_ECPKI_SUPPORT */



	/* ............... initializing local variables ....................... */
	/* -------------------------------------------------------------------- */

	/*This variable is not used when the flag Encryption compilation flag is not defined*/
	AES_Dec_Enc_flag = AES_Dec_Enc_flag;
    	
	/* initialize the error identifier to CRYS_OK */
	Error = CRYS_OK;
	
	/* switch case for setting the return pointer for the working context */
	switch(Type)
	{

	 #ifndef CRYS_NO_PKI_SUPPORT 
	   case  DX_RSA_SIGN_CONTEXT:
	
			RSAPriv_user_context_ptr = (CRYS_RSAPrivUserContext_t *)UserContext_ptr;
			*CRYS_GlobalContext_ptr = RSAPriv_user_context_ptr->context_buff;
			break;
	  #endif //CRYS_NO_PKI_SUPPORT
  					   
	   case DX_RSA_VERIFY_CONTEXT:
	   		RSAPub_user_context_ptr = (CRYS_RSAPubUserContext_t *)UserContext_ptr;
			*CRYS_GlobalContext_ptr = RSAPub_user_context_ptr->context_buff;
			break;


	  #ifndef CRYS_NO_ECPKI_SUPPORT 
			
   	   case  DX_ECDSA_SIGN_CONTEXT:
	
			ECDSA_sign_user_context_ptr = (CRYS_ECDSA_SignUserContext_t *)UserContext_ptr;
			*CRYS_GlobalContext_ptr = ECDSA_sign_user_context_ptr->context_buff;
			break;
   					   
	   case  DX_ECDSA_VERIFY_CONTEXT:
	   
	   		ECDSA_verify_user_context_ptr = (CRYS_ECDSA_VerifyUserContext_t *)UserContext_ptr;
			*CRYS_GlobalContext_ptr = ECDSA_verify_user_context_ptr->context_buff;
			break;	   

     #endif /* CRYS_NO_ECPKI_SUPPORT */
			

   default:
		return CRYS_CCM_CONTEXT_TYPE_ERROR ;		
	
	}/* end of setting the returned context pointer - switch case */
	    
	return Error;
		
 }/* END OF CRYS_CCM_GetContext */

//#endif /* defined(CRYS_NO_CONTEXT_ENCRYPTION_PROTECTION) || defined(CRYS_NO_AES_SUPPORT) */
