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
#include "cc_acl.h"
#include "crys_common_math.h"
#include "crys_ecpki_error.h"
#include "crys_ecpki_local.h"

/* .............. LLF level includes ................. */
#include "llf_ecpki_export.h"

/* canceling the lint warning:
  Info 740: Unusual pointer cast 
  (incompatible indirect types) */
/*lint --e{740} */

/* canceling the lint warning:
   Info 826: Suspicious pointer-to-pointer conversion 
   (area too small) */
/*lint --e{826} */

/* canceling the lint warning:
Info 801: Use of goto is deprecated*/
/*lint --e{801} */
/* r */


/************************ Defines ******************************/

/************************ Enums ********************************/

/************************ Typedefs *****************************/

/************************ Global Data **************************/

/************* Private function prototype **********************/

/************************ Public Functions *********************/


/*****************************************************************************************
 *		       _DX_ECPKI_ELGAMAL_Encrypt function	
 *                        											  
 *****************************************************************************************/
/** 
   @brief: 	This function performs encryption of the message (transmitted key or any other 
            plain text)  with public key of the receiver  using EC Elgamal algorithm. 
			
            For calling this function from CRYS use the macro definition CRYS_ECPKI_ELGAMAL_Encrypt .
			
            Operation: The sender encrypts the message by the following sequence of steps:
		1. Checks validity of input pointers and parameters.
		2. Converts incoming message from big to little endian.
		3. Calls the LLF_ECPKI_ELGAMAL_Encrypt function, which performs encryption
		   according to Elgamal algorithm and outputs Encrypted message as two EC points V and B:
		4. Converts the encrypted message (V,B) into big endian form.
		5. Outputs the encrypted message as bytes array in big endian.
		6. Exits.
		
		Assumptions: 
		1. Public key W and EC domain parameters q, a, b, r, and G are valid and 
		   associated with each other; 
		2. Incoming receiver's public key must be in big endianness.  
		3. The size of incomming message f must be great than 0 and equal and less, than:
		   (size of EC modulus - 1)  - for case, that MSByte of modulus >= 100 :
		   (size of EC modulus - 2)  - for case, that MSByte of modulus < 100.
		   The size of the message must be agreed with the receiver (i.e. in both encrypt and decrypt 
        	   functions size of the plain message must be the of the same size).
                

   @param[in]  ReceiverUserPublKeyBE_ptr - A pointer to receiver public key (key in big endian).  
   @param[in]  MessageIn_ptr         - A pointer to message to be encrypted (message in big endian).
   @param[in]  MessageInSizeBytes    - A size of incoming message in bytes. The maximal size must be:
                                   MessageInSizeBytes <= ModulusSizeInBytes-1, but
				   if the value of MSByte of EC Domain modulus is less than 100,
				   then maximal size of the message must be decreased by one (now the
				   last condition is actual only for one of implemented domains - 
				   CRYS_ECPKI_DomainID_secp521r1).
   @param[in]  EncrMessageOut_ptr    - A pointer to buffer for encrypted message.
   @param[in,out] EncrMessOutSize_ptr- A pointer to size of uzer passed buffer for encrypted
                                       message (in) and actual size of encrypted message (out).
                                       Size of buffer must be not less, than 4*ModulusSizeInBytes. 
   @param[in]  IsEphemerKeyInternal  - A parameter defining whether ephemeral key internal or external (1,0).
   @param[in]  EphemerPrivKeyIn_ptr  - A pointer to ephemeral private key /user not uses this parameter/.
   @param[in]  EphemerPrivKeySizeBytes - A size (in bytes) of sender's ephemeral private key data, must be equal 
                                        or less than EC order size in bytes /user not uses this parameter/.
   @param[in]  TempData_ptr -          - A pointer to structure, containing temporary buffers of specified
                                         type.   
   @return CRYSError_t - CRYS_OK, 
			CRYS_EC_ELGAMAL_ENCR_INVALID_RECEIVER_PUBL_KEY_PTR_ERROR
			CRYS_EC_ELGAMAL_RECEIVER_PUBL_KEY_VALIDATION_TAG_ERROR
			CRYS_EC_ELGAMAL_ENCR_INVALID_DOMAIN_ID_ERROR
			CRYS_EC_ELGAMAL_ENCR_INVALID_MESSAGE_DATA_IN_PTR_ERROR
			CRYS_EC_ELGAMAL_ENCR_INVALID_MESSAGE_DATA_IN_SIZE_ERROR						 
			CRYS_EC_ELGAMAL_ENCR_INVALID_ENCR_MESSAGE_OUT_PTR_ERROR
			CRYS_ECPKI_ELGAM_ENCR_INVALID_ENCR_MESSAGE_OUT_SIZE_PTR_ERROR
			CRYS_ECPKI_ELGAM_ENCR_INVALID_ENCR_MESSAGE_OUT_SIZE_ERROR
			CRYS_EC_ELGAMAL_ENCR_INVALID_TEMP_DATA_BUFFER_PTR_ERROR                         						 						 
			CRYS_EC_ELGAMAL_ENCR_INVALID_EPHEMERAL_KEY_DATA_ERROR
			CRYS_EC_ELGAMAL_ENCR_INVALID_EPHEMERAL_PRIV_KEY_SIZE_ERROR						 
*/  
CEXPORT_C CRYSError_t  _DX_ECPKI_ELGAMAL_Encrypt (
				CRYS_ECPKI_UserPublKey_t       *ReceiverUserPublKey_ptr, /*in*/
				DxUint8_t                      *MessageIn_ptr,           /*in*/
				DxUint32_t                      MessageInSizeBytes,      /*in*/
				DxUint8_t   		       *EncrMessageOut_ptr,      /*out*/
				DxUint32_t		       *EncrMessOutSize_ptr,     /*in,out*/
				DxInt8_t                        IsEphemerKeyInternal,    /*in*/
				DxUint8_t                      *EphemerPrivKeyIn_ptr,    /*in*/ 
				DxUint32_t                      EphemerPrivKeySizeBytes, /*in*/ 
				CRYS_EC_ELGAMAL_TempData_t     *TempData_ptr             /*in*/ )
{
	/* LOCAL DECLERATIONS AND INITIALIZATIONS */
	
	/* the error identifier */
	CRYSError_t Error; 
	
	/* Pointer to receiver public key as key structure in big endian */
	CRYS_ECPKI_PublKey_t       *ReceiverPublKey_ptr; 
	
	/* EC Domain parameters */
	CRYS_ECPKI_DomainID_t       DomainID;
	CRYS_ECPKI_DomainInfo_t     DomainInfo;
	DxUint32_t ModSizeInBytes, ModSizeInWords, OrderSizeInBytes;
	/* pointer to temp buffer for output encrypted data from LLF */
	DxUint32_t *out32_ptr =  (DxUint32_t*)&(((LLF_ecpki_elgamal_db_t*)&TempData_ptr->crysElgamalIntBuff)->OutBuff.encrMessOut);
	ElgamalInBuff_t *inBuff_ptr =  &((LLF_ecpki_elgamal_db_t*)&TempData_ptr->crysElgamalIntBuff)->InBuff;
	
	
	/* FUNCTION LOGIC */
	
	/* ............... if ECPKI not supported: exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_ECPKI_UNSUPPORTED( ReceiverUserPublKey_ptr, MessageIn_ptr,                                 
				MessageInSizeBytes, EncrMessageOut_ptr, IsEphemerKeyInternal, TempData_ptr, 
				EncrMessOutSize_ptr, EphemerPrivKeyIn_ptr, EphemerPrivKeySizeBytes,
				ReceiverPublKey_ptr, DomainID, DomainInfo.EC_ModulusSizeInBits, 
				ModSizeInBytes, ModSizeInWords, OrderSizeInBytes, out32_ptr, 
				inBuff_ptr, Error, Error, Error, Error, Error );							 
			      
	#ifndef CRYS_NO_ECPKI_SUPPORT                                     
	
	/* ................. checking the validity of the input arguments ........ */
	/* ----------------------------------------------------------------------- */
	
	
	/* if the one of user passed pointers is NULL return an error  */
	if( ReceiverUserPublKey_ptr == DX_NULL )
		return CRYS_EC_ELGAMAL_ENCR_INVALID_RECEIVER_PUBL_KEY_PTR_ERROR; 
	
	if(ReceiverUserPublKey_ptr->valid_tag != CRYS_ECPKI_PUBL_KEY_VALIDATION_TAG)
	   return CRYS_EC_ELGAMAL_RECEIVER_PUBL_KEY_VALIDATION_TAG_ERROR; 
	
	/* if the users MessageDataIn pointer is illegal return an error */
	if( MessageIn_ptr == DX_NULL  )
		return CRYS_EC_ELGAMAL_ENCR_INVALID_MESSAGE_DATA_IN_PTR_ERROR;
	
	/* if the users EncrMessageOut_ptr pointer is illegal return an error */
	if( EncrMessageOut_ptr == DX_NULL  )
		return CRYS_EC_ELGAMAL_ENCR_INVALID_ENCR_MESSAGE_OUT_PTR_ERROR ;
	
	/* if the users EncrMessageOutSize_ptr pointer is illegal return an error */
	if( EncrMessOutSize_ptr == DX_NULL  )
		return CRYS_EC_ELGAMAL_ENCR_INVALID_ENCR_MESS_OUT_SIZE_PTR_ERROR;
	
	/* if the users TempData_ptr pointer is illegal return an error */
	if( TempData_ptr == DX_NULL  )
		return CRYS_EC_ELGAMAL_ENCR_INVALID_TEMP_DATA_BUFFER_PTR_ERROR ;
	
	/* if illegal IsEphemerKeyInternal index value */
	if( (IsEphemerKeyInternal != 1) && (IsEphemerKeyInternal != 0) )
		return CRYS_EC_ELGAMAL_ENCR_INVALID_EPHEMERAL_KEY_DATA_ERROR;
	
	/* if the Ephemeral private key is not internal and  EphemerPrivKeyData_ptr = Null       
	   or EphemerPrivKeySizeBytes == 0 return error */
	if( ( IsEphemerKeyInternal != 1 && (EphemerPrivKeyIn_ptr == DX_NULL || EphemerPrivKeySizeBytes == 0) ) )
		return CRYS_EC_ELGAMAL_ENCR_INVALID_EPHEMERAL_KEY_DATA_ERROR;
	
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, ReceiverUserPublKey_ptr, sizeof(CRYS_ECPKI_UserPublKey_t)) || 
	DxCcAcl_IsBuffAccessOk(ACCESS_READ, MessageIn_ptr, MessageInSizeBytes) || 
	DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, EncrMessageOut_ptr, *EncrMessOutSize_ptr) || 
	DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, EncrMessOutSize_ptr, sizeof(DxUint32_t)) || 
	((EphemerPrivKeyIn_ptr != DX_NULL) && DxCcAcl_IsBuffAccessOk(ACCESS_READ, EphemerPrivKeyIn_ptr, EphemerPrivKeySizeBytes)) ||
	DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, TempData_ptr, sizeof(CRYS_EC_ELGAMAL_TempData_t))){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/* Derive receiver public key structure from user public key data buffer */
	ReceiverPublKey_ptr = (CRYS_ECPKI_PublKey_t*)((void*)ReceiverUserPublKey_ptr->PublKeyDbBuff);
	/* Set DomainID */
	DomainID = ReceiverPublKey_ptr->DomainID;
	
	/* initialization of Error */
	Error = CRYS_OK;
	
	/* Get EC Domain information from LLF level */
	Error = LLF_ECPKI_GetDomainInfo(DomainID, &DomainInfo);
	
	if(Error != CRYS_OK)
	   return Error;
	
	ModSizeInBytes    =  (DomainInfo.EC_ModulusSizeInBits + 7) / 8;
	ModSizeInWords    =  (DomainInfo.EC_ModulusSizeInBits + 31) / 32;
	OrderSizeInBytes  =  (DomainInfo.EC_OrderSizeInBits + 7) / 8;   
	
	/*******    Continue checking of input parameters *******/
	
	/* if the user passed Domain ID is illegal then return an error.
	Note: because EC Elgamal now is used only in WM DRM, return an Error 
		also if DomainID != WM_DRM */
	  
#ifdef CRYS_EC_ELGAMAL_NOT_SUPPORT_ALL_DOMAINS
	if(DomainID >= CRYS_ECPKI_DomainID_OffMode || 
	  (DomainID != CRYS_ECPKI_DomainID_WMDRM10 &&
	   DomainID != CRYS_ECPKI_DomainID_secp192r1 &&
	   DomainID != CRYS_ECPKI_DomainID_secp256r1){ 
#else
	if(DomainID >= CRYS_ECPKI_DomainID_OffMode){   
#endif		   
		return CRYS_EC_ELGAMAL_ENCR_INVALID_DOMAIN_ID_ERROR;
	}
	
	/* if message size is greater than (EC modulus size  1) return error */
	if( DomainInfo.ModulusMSByte >= 100 ){
		if( MessageInSizeBytes > (ModSizeInBytes - 1) )
			return CRYS_EC_ELGAMAL_ENCR_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;  
	}
	else if( MessageInSizeBytes > (ModSizeInBytes - 2) ){
		return CRYS_EC_ELGAMAL_ENCR_INVALID_MESSAGE_DATA_IN_SIZE_ERROR; 
	}
	
	if(IsEphemerKeyInternal != 1 && EphemerPrivKeySizeBytes > OrderSizeInBytes)
		return CRYS_EC_ELGAMAL_ENCR_INVALID_EPHEMERAL_PRIV_KEY_SIZE_ERROR;
	
	/* Check the size of the user passed buffer for encrypted message */
	if(*EncrMessOutSize_ptr < 4*ModSizeInBytes){
		*EncrMessOutSize_ptr = 4*ModSizeInBytes;
		return CRYS_EC_ELGAMAL_ENCR_INVALID_ENCR_MESSAGE_OUT_SIZE_ERROR;
	}
	
	
	/*********** initializtions  ******************************/  
	
	/* Clean temp data */
	DX_VOS_MemSetZero((DxUint8_t*)TempData_ptr, sizeof(CRYS_EC_ELGAMAL_TempData_t));
	
	/* Reverse copy message and its size into TempData */
	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
		inBuff_ptr->m, ModSizeInBytes+4, 
		MessageIn_ptr, MessageInSizeBytes);

	if(Error != CRYS_OK)
		return Error;
	
	inBuff_ptr->mSizeBytes = MessageInSizeBytes;
	
	if(IsEphemerKeyInternal != 1)
	{   	     
		/*  Convert ephemeral private key to little endianness and set it into TempData */
		Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
			inBuff_ptr->EphemerPrivKey, (ModSizeInBytes+4), 
			EphemerPrivKeyIn_ptr, EphemerPrivKeySizeBytes);

		if(Error != CRYS_OK)
			return Error;
		
		inBuff_ptr->EphemerPrivKeySizeBytes = EphemerPrivKeySizeBytes;
	}
	
	
	/*************  CALL THE LLF ELGAMAL ENCRYPT FUNCTION   *****************/
	
	Error = LLF_ECPKI_ELGAMAL_Encrypt( 
				ReceiverPublKey_ptr, out32_ptr/*out*/, 
				IsEphemerKeyInternal, TempData_ptr);
	
	if(Error != CRYS_OK)
		goto End;

	/* Convert Encrypted message (V,B) = (SenderPublKeyLE,TempPoint) to big endian */
	/* Remark: Used static function for copying in place, because bug founded in CRYS_COMMON_ReverseMemcpy */
	Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
		EncrMessageOut_ptr, 4*ModSizeInWords, 
		out32_ptr/*Vx*/, ModSizeInBytes);
	if(Error)
		 return Error;
	Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
		EncrMessageOut_ptr + ModSizeInBytes/*Vy*/, 4*ModSizeInWords, 
		out32_ptr + ModSizeInWords, ModSizeInBytes);
	if(Error)
		 return Error;
	Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
		EncrMessageOut_ptr + 2*ModSizeInBytes/*Bx*/, 4*ModSizeInWords, 
		out32_ptr + 2*ModSizeInWords, ModSizeInBytes);
	if(Error)
		 return Error;
	Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
		EncrMessageOut_ptr + 3*ModSizeInBytes/*By*/, 4*ModSizeInWords, 
		out32_ptr + 3*ModSizeInWords, ModSizeInBytes);
	if(Error)
		 return Error;
	
End:	
	/* Output the actual size of encrypted message */
	*EncrMessOutSize_ptr = 4 * ModSizeInBytes;
	
	/* Clean temp data for security goals */
	DX_VOS_MemSetZero((DxUint8_t*)TempData_ptr, sizeof(CRYS_EC_ELGAMAL_TempData_t));
	
	return Error;
	
	#endif /* !CRYS_NO_ECPKI_SUPPORT */                                   

}/* END OF CRYS_ECPKI_ELGAMAL_Encrypt */                                       

/**********************************************************************/




/*************************************************************************************
 *		       CRYS_ECPKI_ELGAMAL_Decrypt function	
 *                        											  
 *************************************************************************************/
/**
  @brief:  This function performs decryption of the encrypted message (transmitted key or any other 
        text) with private key of the receiver using EC Elgamal algorithm. 
 
        The Decrypting  primitive uses the following data as input:
	       - The EC Domain parameters (received through DomainID included in ReceiverUserPrivKey).
	       - The  message, which includes two EC points (V, B). The message must be in big endianness
	         sequence.

  <b>Operation:</b> The receiver decrypts the message by the following sequence of steps:
    1.  Checks validity of input parameters (partially).
	2.  Converts incoming message (V, B) to little endian form.
	3.  Calls the LLF_ECPKI_ELGAMAL_Decrypt function, which performs decryption
		according to Elgamal algorithm (used in WMDRM ), which performs all 
		decryption operations.
    4.  Converts decrypted message to big endian form.
    5.  Outputs the decrypted message as byte string of receiver passed length.   

	\Note: 1. The maximal size of decrypted message f, which may be derived right from encrypted message (V, B), 
	          is equalled:
                       - (size of EC modulus - 1)  - for case, that MSByte of modulus >= 100 :
                       - (size of EC modulus - 2)  - for case, that MSByte of modulus < 100.
	          The size of the decrypted message, provided by receiver, must be equall to required size, 
			  but not great, than said above maximal sizes.

   <b>Assumptions:</b> EC domain parameters q, a, b, r, and G are valid and associated with each other. 
 
   @param[in] ReceiverUserPrivKey_ptr    - A pointer to a receiver private key structure
   			                              (in affine coordinates).    				
   @param[in] EncrMessageIn_ptr          - The user passed pointer to the encrypted message 
                                           buffer.
   @param[in] EncrMessageSizeInBytes     - The size of input encrypted message. Must be equal to 
                                           4*(EC modulus size in bytes)   
   @param[out] DecrMessageOut_ptr        - The user passed pointer to buffer for output of  
   			                               decrypted message.
   @param[in/out] DecrMessageOutSize_ptr - A pointer to size of decrypted message: see note 1 above.
   @param[in] TempData_ptr               - A pointer to structure, containing temporary buffers of specified
                                           type.

   @return CRYSError_t - CRYS_OK,
			CRYS_EC_ELGAMAL_DECR_INVALID_RECEIVER_PRIV_KEY_PTR_ERROR
			CRYS_EC_ELGAMAL_RECEIVER_PRIV_KEY_VALIDATION_TAG_ERROR
			CRYS_EC_ELGAMAL_DECR_INVALID_DOMAIN_ID_ERROR						   
			CRYS_EC_ELGAMAL_DECR_INVALID_ENCR_MESSAGE_IN_PTR_ERROR 
			CRYS_EC_ELGAMAL_DECR_INVALID_ENCR_MESSAGE_IN_SIZE_ERROR
			CRYS_EC_ELGAMAL_DECR_INVALID_DECR_MESSAGE_OUT_PTR_ERROR
			CRYS_EC_ELGAMAL_DECR_INVALID_DECR_MESSAGE_OUT_SIZE_PTR_ERROR
			CRYS_EC_ELGAMAL_DECR_INVALID_DECR_MESSAGE_OUT_SIZE_ERROR
			CRYS_EC_ELGAMAL_DECR_INVALID_TEMP_DATA_BUFFER_PTR_ERROR
						 
*/
CEXPORT_C CRYSError_t  CRYS_ECPKI_ELGAMAL_Decrypt (			      
			CRYS_ECPKI_UserPrivKey_t   *ReceiverUserPrivKey_ptr,  /*in*/	
			DxUint8_t   		   *EncrMessageIn_ptr,          /*in*/
			DxUint32_t                  EncrMessageSizeInBytes,   /*in*/
			DxUint8_t		   *DecrMessageOut_ptr,       /*out*/
			DxUint32_t                 *DecrMessageOutSize_ptr,   /*in/out*/
			CRYS_EC_ELGAMAL_TempData_t *TempData_ptr              /*in*/ )

{				
	/* LOCAL DECLERATIONS AND INITIALIZATIONS */
	
	/* the error identifier */
	CRYSError_t                 Error; 
	CRYS_ECPKI_PrivKey_t       *ReceiverPrivKey_ptr;
	
	/* EC Domain parameters */
	CRYS_ECPKI_DomainID_t       DomainID;
	CRYS_ECPKI_DomainInfo_t     DomainInfo;
	DxUint32_t ModSizeInBytes, ModSizeInWords;
	DxUint32_t *tmp32_ptr = (DxUint32_t*)TempData_ptr;
	
	
	/* FUNCTION LOGIC */
	
	/* ............... if ECPKI not supported: exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_ECPKI_UNSUPPORTED( ReceiverUserPrivKey_ptr, EncrMessageIn_ptr,                                 
				EncrMessageSizeInBytes, DecrMessageOut_ptr, DecrMessageOutSize_ptr,  
				TempData_ptr, ReceiverPrivKey_ptr, DomainID, DomainInfo.EC_ModulusSizeInBits, 
				ModSizeInBytes, ModSizeInWords, Error, Error, Error, Error, 								
				Error, Error, Error, Error, Error, Error,  Error );
							 
			      
	#ifndef CRYS_NO_ECPKI_SUPPORT    
	
	/* initialize the error identifier to OK */
	Error = CRYS_OK;
				 
	
	/* ................. checking the validity of the input arguments ........ */
	/* ----------------------------------------------------------------------- */
	
	/* if the one of user passed pointers is NULL return an error  */
	if( ReceiverUserPrivKey_ptr == DX_NULL )
		return CRYS_EC_ELGAMAL_DECR_INVALID_RECEIVER_PRIV_KEY_PTR_ERROR;  
	
	if(ReceiverUserPrivKey_ptr->valid_tag != CRYS_ECPKI_PRIV_KEY_VALIDATION_TAG)
		return CRYS_EC_ELGAMAL_RECEIVER_PRIV_KEY_VALIDATION_TAG_ERROR; 
	
	/* if the users MessageDataIn pointer is illegal return an error */
	if( EncrMessageIn_ptr == DX_NULL  )
		return CRYS_EC_ELGAMAL_DECR_INVALID_ENCR_MESSAGE_IN_PTR_ERROR;
	
	/* if the users EncrMessageOut_ptr pointer is illegal return an error */
	if( DecrMessageOut_ptr == DX_NULL  )
		return CRYS_EC_ELGAMAL_DECR_INVALID_DECR_MESSAGE_OUT_PTR_ERROR;
	
	/* if the users EncrMessageOutSize_ptr pointer is illegal return an error */
	if( DecrMessageOutSize_ptr == DX_NULL  )
		return CRYS_EC_ELGAMAL_DECR_INVALID_DECR_MESSAGE_OUT_SIZE_PTR_ERROR;
	
	/* if the users TempData_ptr pointer is illegal return an error */
	if( TempData_ptr == DX_NULL  )
		return CRYS_EC_ELGAMAL_DECR_INVALID_TEMP_DATA_BUFFER_PTR_ERROR ;
	
	if(DxCcAcl_IsBuffAccessOk(ACCESS_READ, ReceiverUserPrivKey_ptr, sizeof(CRYS_ECPKI_UserPrivKey_t)) || 
	   DxCcAcl_IsBuffAccessOk(ACCESS_READ, EncrMessageIn_ptr, EncrMessageSizeInBytes) || 
	   DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, DecrMessageOut_ptr, *DecrMessageOutSize_ptr) || 
	   DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, DecrMessageOutSize_ptr, sizeof(DxUint32_t)) || 
	   DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, TempData_ptr, sizeof(CRYS_EC_ELGAMAL_TempData_t))){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/* Derive receiver public key structure from user private key data buffer */
	ReceiverPrivKey_ptr = (CRYS_ECPKI_PrivKey_t*)((void*)ReceiverUserPrivKey_ptr->PrivKeyDbBuff);
	
	/* Set DomainID from ReceiverPublKey_ptr */
	DomainID = ReceiverPrivKey_ptr->DomainID;
	
	/* Get EC Domain information from LLF level */
	Error = LLF_ECPKI_GetDomainInfo(DomainID, &DomainInfo);
	
	if(Error != CRYS_OK) 
		return Error;
	
	ModSizeInBytes  =  (DomainInfo.EC_ModulusSizeInBits + 7) / 8;
	ModSizeInWords  =  (DomainInfo.EC_ModulusSizeInBits + 31) / 32;
	
	/* if the user passed Domain ID is illegal then return an error.
	Note: because EC Elgamal now is used only in WM DRM, return an Error 
	also if DomainID != WM_DRM */
	if( DomainID >= CRYS_ECPKI_DomainID_OffMode  
	   #ifdef CRYS_EC_ELGAMAL_NOT_SUPPORT_ALL_DOMAINS
	   || (DomainID != CRYS_ECPKI_DomainID_WMDRM10 &&
	   DomainID != CRYS_ECPKI_DomainID_secp192r1 &&
	   DomainID != CRYS_ECPKI_DomainID_secp256r1) 
	   #endif		   
	 )
	        return CRYS_EC_ELGAMAL_ENCR_INVALID_DOMAIN_ID_ERROR;
	
	/* Check the user passed size of encrypted message */
	if(EncrMessageSizeInBytes != 4*ModSizeInBytes) 
		return CRYS_EC_ELGAMAL_DECR_INVALID_ENCR_MESSAGE_IN_SIZE_ERROR;
	
	/* if size of user passed decrypted message size is great than max size, then return error */
	/* if message size is great than allowed, then return error */
	if(DomainInfo.ModulusMSByte >= 100)
	{
		if(*DecrMessageOutSize_ptr > (ModSizeInBytes - 1)){
		   *DecrMessageOutSize_ptr = (ModSizeInBytes - 1);
			return CRYS_EC_ELGAMAL_DECR_INVALID_DECR_MESSAGE_OUT_SIZE_ERROR;
		}
	}
	else 
	{
		if(*DecrMessageOutSize_ptr > (ModSizeInBytes - 2) ){
		  *DecrMessageOutSize_ptr = (ModSizeInBytes - 2);
			return CRYS_EC_ELGAMAL_DECR_INVALID_DECR_MESSAGE_OUT_SIZE_ERROR; 
		}
	}
	
	
	/*********** initializations  ******************************/  
	
	/* Clean temp data buffer */
	DX_VOS_MemSetZero((DxUint8_t*)TempData_ptr, sizeof(CRYS_EC_ELGAMAL_TempData_t));

	/* reverse copy encrypted message V into into aligned buffer */
	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
		tmp32_ptr/*dstReg*/, 4*ModSizeInWords, 
		EncrMessageIn_ptr/*src_ptr*/, ModSizeInBytes);
	if(Error)
		 return Error;

	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
		tmp32_ptr + ModSizeInWords/*dstReg*/, 4*ModSizeInWords, 
		EncrMessageIn_ptr + ModSizeInBytes/*src_ptr*/, ModSizeInBytes);
	if(Error)
		 return Error;

	/* reverse copy encrypted message B into aligned buffer */
	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
		tmp32_ptr + 2*ModSizeInWords/*dstReg*/, 4*ModSizeInWords, 
		EncrMessageIn_ptr + 2*ModSizeInBytes/*src_ptr*/, ModSizeInBytes);
	if(Error)
		 return Error;

	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
		tmp32_ptr + 3*ModSizeInWords/*dstReg*/, 4*ModSizeInWords, 
		EncrMessageIn_ptr + 3*ModSizeInBytes/*src_ptr*/, ModSizeInBytes);
	if(Error)
		 return Error;

	
	/*************  CALL THE LLF ELGAMAL ENCRYPT FUNCTION   *****************/
	
	Error = LLF_ECPKI_ELGAMAL_Decrypt(ReceiverPrivKey_ptr, tmp32_ptr/*in*/, 
					  tmp32_ptr/*out*/, *DecrMessageOutSize_ptr, 
					  TempData_ptr);
	if(Error != CRYS_OK)
		goto END;
	
	/* reverse the decrypted message */
	Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
		DecrMessageOut_ptr/*out*/, sizeof(DxUint32_t)*ModSizeInWords,
		tmp32_ptr/*in*/, *DecrMessageOutSize_ptr);
	if(Error)
		 return Error;
	
END:
	/* Clean temp data for security goals */
	DX_VOS_MemSetZero((DxUint8_t*)TempData_ptr, sizeof(CRYS_EC_ELGAMAL_TempData_t));
	
	return Error;
	
#endif /* !CRYS_NO_ECPKI_SUPPORT */                                   
	
}/* END OF CRYS_ECPKI_ELGAMAL_Decrypt */                  



