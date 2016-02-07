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

 
 #ifndef CRYS_ECPKI_KG_H
#define CRYS_ECPKI_KG_H

/*
 *  Object name     :  CRYS_ECPKI_KG.h
 *  State           :  %state%
 *  Creation date   :  02.02.2006
 *  Last modified   :  %modify_time%
 */
/** @file
 * \brief Defines the API of a function for generating ECC private and public keys.
 *
 *  \version CRYS_ECPKI_KG.h#1:cinc:1
 * \author R.Levin
 */

#include "crys_defs.h"
#include "crys_error.h"
#include "crys_ecpki_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


  /*****************  CRYS_ECPKI_GenKeyPair function   **********************/
  /**
   @brief Generates a pair of private and public keys  
          in little endian ordinary (non-Montgomery) form. 
	  
	  This function generates a new key pair and initializes 
	  the variables and structures so that these can be used by other EC primitives. 
	  The function performs the following:
		1. Checks the validity of all of the function inputs. If one of the received 
		    parameters is not valid, it returns an error. The major checks are:
		   - Whether DomainID is valid 
		   - Whether the user private key pointer(UserPrivKey_ptr) is not NULL 
		   - Whether the User Public Key pointer (UserPublKey_ptr) is not NULL 
		   - Whether the User passed temp data buffer is not NULL.
		2. Cleans buffers for private and public key structures.
		3. Calls the low level function  LLF_ECPKI_GenKeyPair.
		4. Outputs user public and private key structures in little endian form.
		5. Cleans temporary buffers. 
		6. Exits. 
 
   @param[in]  DomainID        - The enumerator variable defines current EC domain.
   @param[out] UserPrivKey_ptr - A pointer to the private key structure.                                  
   @param[out] UserPubKey_ptr  - A pointer to the public key structure.                                  
   @param[in]  TempData_ptr    - Temporary buffers of size defined in CRYS_ECPKI_KG_TempData_t.

   @return <b>CRYSError_t</b>: <br> 
			 CRYS_OK<br> 
			 CRYS_ECPKI_GEN_KEY_ILLEGAL_D0MAIN_ID_ERROR<br>
			 CRYS_ECPKI_GEN_KEY_INVALID_PRIVATE_KEY_PTR_ERROR<br>
			 CRYS_ECPKI_GEN_KEY_INVALID_PUBLIC_KEY_PTR_ERROR<br>
			 CRYS_ECPKI_GEN_KEY_INVALID_TEMP_DATA_PTR_ERROR<br>
*/
CIMPORT_C CRYSError_t CRYS_ECPKI_GenKeyPair(
				    	CRYS_ECPKI_DomainID_t  	    DomainID,	        /*in*/   
				    	CRYS_ECPKI_UserPrivKey_t   *UserPrivKey_ptr,    /*out*/
				        CRYS_ECPKI_UserPublKey_t   *UserPublKey_ptr,    /*out*/   
				        CRYS_ECPKI_KG_TempData_t   *TempData_ptr        /*in*/ );

				              

#ifdef __cplusplus
}
#endif

#endif




