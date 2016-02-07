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
#include "crys_common_math.h"

#include "crys_ecpki_error.h"
#include "crys_ecpki_local.h"

/* .............. LLF level includes and definitions.............. */
#include "llf_ecpki_export.h"
#include "cc_acl.h"

/* canceling the lint warning:
   Info 717: do ... while(0) */
/*lint --e{717} */

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/* canceling the lint warning:
Info 716: while(1) ... */ 
/*lint --e{716} */

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************* Private function prototype ****************/

/************************ Public Functions ******************************/

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
CEXPORT_C CRYSError_t CRYS_ECPKI_GenKeyPair(
			CRYS_ECPKI_DomainID_t  	    DomainID,	        /*in*/   
			CRYS_ECPKI_UserPrivKey_t   *UserPrivKey_ptr,    /*out*/
			CRYS_ECPKI_UserPublKey_t   *UserPublKey_ptr,    /*out*/   
			CRYS_ECPKI_KG_TempData_t   *TempData_ptr        /*in*/ )
{
/* LOCAL INITIALIZATIONS AND DECLERATIONS */

/* the error identifier */
CRYSError_t Error;   


/* FUNCTION LOGIC */

/* ............... if not supported exit .............................. */
/* -------------------------------------------------------------------- */   

RETURN_IF_ECPKI_UNSUPPORTED( DomainID, UserPrivKey_ptr, UserPublKey_ptr, TempData_ptr, 
			Error, Error, Error, Error, Error, Error, 
			Error, Error, Error, Error, Error, Error, 
			Error, Error, Error, Error, Error, Error );							 
		      
#ifndef CRYS_NO_HASH_SUPPORT                                   
#ifndef CRYS_NO_ECPKI_SUPPORT                                      

	/* ................. checking the validity of the pointer arguments ...... */
	/* ----------------------------------------------------------------------- */
	
	/* ...... checking the EC domain ID.................... */
	if( DomainID >= CRYS_ECPKI_DomainID_OffMode )
		return CRYS_ECPKI_GEN_KEY_ILLEGAL_D0MAIN_ID_ERROR;
	
	/* ...... checking the validity of the user private key pointer .......... */
	if( UserPrivKey_ptr == DX_NULL )
		return CRYS_ECPKI_GEN_KEY_INVALID_PRIVATE_KEY_PTR_ERROR;
	
	/* ...... checking the validity of the user public key pointer ........... */
	if( UserPublKey_ptr == DX_NULL )
		return CRYS_ECPKI_GEN_KEY_INVALID_PUBLIC_KEY_PTR_ERROR;
	
	/* ...... checking the validity of temp buffers         .................. */
	if( TempData_ptr == DX_NULL )
		return CRYS_ECPKI_GEN_KEY_INVALID_TEMP_DATA_PTR_ERROR;  
	
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPrivKey_ptr, sizeof(CRYS_ECPKI_UserPrivKey_t)) || 
	    DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPublKey_ptr, sizeof(CRYS_ECPKI_UserPublKey_t)) || 
	    ((TempData_ptr != DX_NULL) && DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, TempData_ptr, sizeof(CRYS_ECPKI_KG_TempData_t)))){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/* ................ initializations  ............ */
	/* ---------------------------------------------- */
	
	/* initialize the error identifier to OK */
	Error = CRYS_OK;
	
	
	/* ................ clear all input structures ............................. */
	/* ------------------------------------------------------------------------- */
	
	DX_VOS_MemSet( UserPrivKey_ptr, 0, sizeof(CRYS_ECPKI_UserPrivKey_t) ); 
	DX_VOS_MemSet( UserPublKey_ptr, 0, sizeof(CRYS_ECPKI_UserPublKey_t) ); 
	
	/*     executing the key generation   */   
	
	Error = LLF_ECPKI_GenKeyPairCall( DomainID, UserPrivKey_ptr, 
				     UserPublKey_ptr, TempData_ptr );
	
	/*     on failure exit the function   */
	if( Error != CRYS_OK )  
		goto End;          
	
	/*     set the key valid tags         */   
	UserPrivKey_ptr->valid_tag  = CRYS_ECPKI_PRIV_KEY_VALIDATION_TAG; 
	UserPublKey_ptr->valid_tag  = CRYS_ECPKI_PUBL_KEY_VALIDATION_TAG; 
	
End:  
	
	/*     clear the KG data structure    */
	DX_VOS_MemSet ( TempData_ptr, 0, sizeof(CRYS_ECPKI_KG_TempData_t) );  
	
	return Error;
	

#endif /* CRYS_NO_ECPKI_SUPPORT */ 
#endif /* CRYS_NO_HASH_SUPPORT  */                                  

}/* END OF CRYS_ECPKI_GenKeyPair */                                       

/**********************************************************************/



