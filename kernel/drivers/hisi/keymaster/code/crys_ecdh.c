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

/* CRYS level includes  */
#include "dx_vos_mem.h"
#include "crys_ecpki_error.h"
#include "crys_ecpki_local.h"

/* LLF level includes  */
#include "llf_ecpki_export.h"
#include "dx_pal_log.h"
#include "cc_acl.h"

/************************ Defines *************************************/

/************************ Enums ***************************************/

/************************ Typedefs ************************************/

/************************ Global Data *********************************/

/************* Private function prototype *****************************/


/************************ Public Functions ****************************/


/***********************************************************************
 *		         CRYS_ECDH_SVDP_DH function 						   *							  
 ***********************************************************************/
/**
 @brief    Creates the shared secret value accordingly to the IEEE 1363-2000 
 			standard. 

	       This function performs the following:
 			
		-# Checks input parameters: pointers and EC Domain ID.
		-# Derives the partner public key and calls the LLF_ECPKI_SVDP_DH function,
		    which performs EC SVDP operations. On errors, outputs error messages.
		-# Exits.
	\note  The term "user" indicates any party who performs a calculation of a shared 
	secret value using this primitive. The term "partner" indicates any other party of 
	shared secret value calculation.
 
 @param[in]  PartnerPublKey_ptr         A pointer to a partner public key W 
 @param[in]  UserPrivKey_ptr	        A pointer to a user private key  
 @param[out] SharedSecretValue_ptr      A pointer to an output buffer that will contain
                                        the shared secret value 
 @param[in,out] SharedSecrValSize_ptr   A pointer to the size of user passed buffer (in) and 
                                        actual output size  of calculated shared secret value.
 @param[in]  TempBuff_ptr               A pointer to a temporary buffers of size specified in 
                                        the CRYS_ECDH_TempData_t structure.
 @return <b>CRYSError_t</b>: <br> 
			 CRYS_OK<br> 
                         CRYS_ECDH_SVDP_DH_INVALID_USER_PRIV_KEY_PTR_ERROR<br> 
                         CRYS_ECDH_SVDP_DH_USER_PRIV_KEY_VALID_TAG_ERROR<br> 
 			 CRYS_ECDH_SVDP_DH_INVALID_PARTNER_PUBL_KEY_PTR_ERROR<br> 
			 CRYS_ECDH_SVDP_DH_PARTNER_PUBL_KEY_VALID_TAG_ERROR<br> 
                         CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_PTR_ERROR<br> 
			 CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_PTR_ERROR<br> 
			 CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR<br> 
			 CRYS_ECDH_SVDP_DH_INVALID_TEMP_DATA_PTR_ERROR<br> 
			 
   NOTE:    1. The partner public key and user private key must relate to the same DomainID.
            2. The public key must be full validated befor using in this primitive.
            3. Buffer size for SharedSecretValue must be >= ModulusSizeInWords*4 bytes,  
               output size of shared value is equal to ModulusSizeInWords*4. 
			 
*/
CEXPORT_C CRYSError_t CRYS_ECDH_SVDP_DH( 
				CRYS_ECPKI_UserPublKey_t *PartnerPublKey_ptr,        /*in*/
				CRYS_ECPKI_UserPrivKey_t *UserPrivKey_ptr,           /*in*/							                
				DxUint8_t		 *SharedSecretValue_ptr,     /*out*/
				DxUint32_t               *SharedSecrValSize_ptr,     /*in/out*/
				CRYS_ECDH_TempData_t     *TempBuff_ptr               /*in*/ )
{
	/* LOCAL INITIALIZATIONS AND DECLERATIONS */
	
	/* the error identifier */
	CRYSError_t Error;
	
	CRYS_ECPKI_PublKey_t *PublKey_ptr;
	CRYS_ECPKI_PrivKey_t *PrivKey_ptr;
	
	/*  EC domain ID and pointer to the current Domain info structure*/
	CRYS_ECPKI_DomainID_t    DomainID;   
	CRYS_ECPKI_DomainInfo_t  DomainInfo;
	
	DxUint32_t      ModSizeInBytes;             
	
	
	/* FUNCTION LOGIC */    
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_ECPKI_UNSUPPORTED( PartnerPublKey_ptr, UserPrivKey_ptr, DomainID,
				SharedSecretValue_ptr, SharedSecrValSize_ptr, TempBuff_ptr,  
				PublKey_ptr, DomainInfo.EC_ModulusSizeInBits, 
				ModSizeInBytes, Error, Error, Error, Error, Error, 
				Error, Error, Error, Error, Error, Error, Error, Error );
			      
#ifndef CRYS_NO_HASH_SUPPORT                                    
#ifndef CRYS_NO_ECPKI_SUPPORT 

    
	/* Initialize Error */
	Error = CRYS_OK;                                     
	
	/* ................. checking the validity of the pointer arguments ...... */
	/* ----------------------------------------------------------------------- */
	
	/* ...... checking the validity of the user private key pointer .......... */
	if( UserPrivKey_ptr == DX_NULL )   
		return CRYS_ECDH_SVDP_DH_INVALID_USER_PRIV_KEY_PTR_ERROR;
	
	/* ...... checking the valid tag of the user private key pointer ......... */
	if( UserPrivKey_ptr->valid_tag != CRYS_ECPKI_PRIV_KEY_VALIDATION_TAG  )   
		return CRYS_ECDH_SVDP_DH_USER_PRIV_KEY_VALID_TAG_ERROR;
	
	/* .... checking the validity of the other partner public key pointer .... */
	if( PartnerPublKey_ptr == DX_NULL )   
		return CRYS_ECDH_SVDP_DH_INVALID_PARTNER_PUBL_KEY_PTR_ERROR;
	
	/* ...... checking the valid tag of the user private key pointer ......... */
	if( PartnerPublKey_ptr->valid_tag != CRYS_ECPKI_PUBL_KEY_VALIDATION_TAG  )   
		return CRYS_ECDH_SVDP_DH_PARTNER_PUBL_KEY_VALID_TAG_ERROR;   
	
	/* ...... checking the validity of the SharedSecretValue pointer ..........*/
	if( SharedSecretValue_ptr == DX_NULL )   
		return CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_PTR_ERROR;      
	
	/* ...... checking the validity of SharedSecrValSize_ptr pointer ......... */
	if( SharedSecrValSize_ptr == DX_NULL )   
		return CRYS_ECDH_SVDP_DH_INVALID_TEMP_DATA_PTR_ERROR;     
	
	/* ...... checking the validity of temp buffers         .................. */
	if( TempBuff_ptr == DX_NULL )   
		return CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_PTR_ERROR;  
	
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPrivKey_ptr, sizeof(CRYS_ECPKI_UserPrivKey_t)) || 
	    DxCcAcl_IsBuffAccessOk(ACCESS_READ, PartnerPublKey_ptr, sizeof(CRYS_ECPKI_UserPublKey_t)) ||
	    DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, SharedSecretValue_ptr, *SharedSecrValSize_ptr) || 
	    DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, SharedSecrValSize_ptr, sizeof(SharedSecrValSize_ptr)) || 
	    DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, TempBuff_ptr, sizeof(CRYS_ECDH_TempData_t))){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/* ..  initializtions  and other checking   .... */
	/* --------------------------------------------- */    
	
	/* Derive  DomainID from BuildedPartnerPublKey_ptr */
	PublKey_ptr = (CRYS_ECPKI_PublKey_t*)((void*)PartnerPublKey_ptr->PublKeyDbBuff);
	PrivKey_ptr = (CRYS_ECPKI_PrivKey_t*)((void*)UserPrivKey_ptr->PrivKeyDbBuff);
	
	DomainID = PublKey_ptr->DomainID;
	
	/* check DomainID of public key */
	if( DomainID >= CRYS_ECPKI_DomainID_OffMode )
		return  CRYS_ECDH_SVDP_DH_ILLEGAL_DOMAIN_ID_ERROR; 
	
	/* .... check that domain ID for public and private keys are identical ... */
	if( DomainID != PrivKey_ptr->DomainID) 
		return  CRYS_ECDH_SVDP_DH_NOT_CONCENT_PUBL_AND_PRIV_DOMAIN_ID_ERROR;
	
	/* Get EC Domain information from LLF level */
	Error = LLF_ECPKI_GetDomainInfo(DomainID, &DomainInfo);
	
	if(Error != CRYS_OK)   
		return Error;
	
	/* Modulus size */
	ModSizeInBytes   =  (DomainInfo.EC_ModulusSizeInBits + 7) / 8;
	
	/*  check size of buffer for Shared value  */
	if(*SharedSecrValSize_ptr < ModSizeInBytes){
		*SharedSecrValSize_ptr = ModSizeInBytes;
		return CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR;
	}

	/* Performing DH operations by calling  LLF_ECDH_SVDP_DH() function */
	/*------------------------------------------------------------------*/ 
	Error = LLF_ECPKI_SVDP_DH(
		PublKey_ptr, PrivKey_ptr->PrivKey,
		(DxUint32_t*)TempBuff_ptr/*out: SharedSecret*/,
		TempBuff_ptr );   
						      
	if(Error != CRYS_OK)
		return Error;

	Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
		SharedSecretValue_ptr, 4*((ModSizeInBytes+3)/4),
		(DxUint32_t*)TempBuff_ptr/*in32_ptr*/, ModSizeInBytes);

	if(Error != CRYS_OK)
		return Error;

	/* Set SharedSecrValSize = ModSizeInWords  for user control */
	*SharedSecrValSize_ptr = ModSizeInBytes;
	
	
	return Error; 
                   
  #endif /* CRYS_NO_ECPKI_SUPPORT */
  #endif /* CRYS_NO_HASH_SUPPORT */                                   

}/* END OF LLF_ECPKI_DH */                                       

