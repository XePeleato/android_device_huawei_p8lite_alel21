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

 
 
#ifndef CRYS_CCM_H
#define CRYS_CCM_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "crys_error.h"


#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:44:53 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version CRYS_CCM.h#1:incl:12
   *  \author adams
   */

/************************ Defines ******************************/
            
/************************ Enums ********************************/
/* The enum determining the type of context to be allocated using the CRYS_CCM_GetContext function or released
   using the CRYS_CCM_ReleaseContext function */

typedef enum ContextType_enum
{
	 DX_HASH_MD5_CONTEXT   ,
	 DX_HASH_SHA1_CONTEXT  ,
	 DX_HMAC_CONTEXT,
	 DX_DES_1KEY_CONTEXT   ,
	 DX_DES_2KEY_CONTEXT   ,
	 DX_DES_3KEY_CONTEXT   ,
	 DX_RSA_SIGN_CONTEXT   ,
	 DX_RSA_VERIFY_CONTEXT ,
	 DX_AES_CONTEXT,
	 DX_RC4_CONTEXT,
 	 DX_ECDSA_SIGN_CONTEXT,
	 DX_ECDSA_VERIFY_CONTEXT,
	 DX_C2_CIPHER_CONTEXT,
	 DX_C2_HASH_CONTEXT,
	 DX_OTF_CONTEXT,
	 DX_AESCCM_CONTEXT,


	 ContextTypeLast= 0x7FFFFFFF,

}ContextType_t;

/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/
 
 /* ------------------------------------------------------------
 **
 * @brief This function does the following:
 *        1) activates a semaphore on the required context. 
 *        2) Allocates a free context managed by the context manager.
 *        3) copies the information from the users context to the allocated context.
 *        4) Decrypts the information in the context. 
 *
 * @param[in] UserContext_ptr - The users context pointer.
 * @param[in] Decrypt_flag - Weather or not to make a decrypt operation. e.g. in AES_Init a decrypt is not needed.
 * @param[out] CRYS_GlobalContext_ptr - The returned pointer of the allocated context.
 * @param[out] Type - The context type.
 *
 * @return CRYSError_t - On success CRYS_OK.
 */
 CRYSError_t CRYS_CCM_GetContext(void *UserContext_ptr,void ** CRYS_GlobalContext_ptr,ContextType_t Type,DxUint8_t Decrypt_flag);

#define CRYS_CCM_ReleaseContext(user,global,type) CRYS_OK

#define CRYS_CCM_Init() CRYS_OK

#define CRYS_CCM_Terminate() CRYS_OK

#ifdef __cplusplus
}
#endif

#endif



