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

 
 #ifndef LLF_ECPKI_EXPORT_H
#define LLF_ECPKI_EXPORT_H
/*
   *  Object %name    : %LLF_ECPKI_Export.h
   *  State           :  %state%
   *  Creation date   :  22.05.2006
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version LLF_ECPKI_Export.h#1:incl:1
   *  \author R.Levin
   */

#ifdef __cplusplus
extern "C"
{
#endif


/************* Include Files ***********************************/

/* .............. CRYS level includes ........................ */
#include "dx_vos_sem.h"
#include "llf_ecpki.h"


/************************ Defines ******************************************/

/************************ Enums ********************************************/

/************************ Typedefs *****************************************/

/************************ Global Data **************************************/

#ifndef DX_OEM_FW
extern DX_PAL_SEM SemPkaId;
#else //DX_OEM_FW
#define DX_VOS_SemWait(SemPkaId,timeout)                CRYS_OK
#define DX_VOS_SemGive(SemPkaId)
#endif //DX_OEM_FW


/************* Private function prototype **********************************/

/************************ Public Functions *********************************/

/****************************************************************************************
                     _DX_LLF_ECPKI_InitPubPrivKeyDb function    
****************************************************************************************/
/**
 * @brief This function initializes the LLF public or private key database.
 *
 *        On the HW platform the Barrett NP and RP tags are initialized.
 *
 *        Note: 1. The function initializes only one of key database, which 
 *              pointer is not NULL.
 *              If both the public and the private keys pointers are NULL or
 *              both are not NULL, the function returns an error.
 *              2. For calling this function use the macros, given in LLF_ECPKI_Export.h.
 *         
 *
 * @param[in] PubKey_ptr - The pointer to public key structure.
 * @param[in] PrivKey_ptr - The pointer to private key structure.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t _DX_LLF_ECPKI_InitPubPrivKeyDb( CRYS_ECPKI_PublKey_t *PubKey_ptr, 
                                           CRYS_ECPKI_PrivKey_t *PrivKey_ptr );


/***************************************************************************************/
/*     Macro definitions for calling the LLF_ECPKI_InitPubPrivKeyDb function           */
/***************************************************************************************/

/* initialize public key database */
#define LLF_ECPKI_InitPubKeyDb( PubKey_ptr) \
	_DX_LLF_ECPKI_InitPubPrivKeyDb( (PubKey_ptr), DX_NULL )

/* initialize private key database */
#define LLF_ECPKI_InitPrivKeyDb(PrivKey_ptr) \
	_DX_LLF_ECPKI_InitPubPrivKeyDb( DX_NULL, (PrivKey_ptr) )


/*****************************************************************************************
 *		       LLF_ECPKI_CheckPublKeyCall function	
 *****************************************************************************************/
/** 
  @brief  This function used as interface between CRYS and LLF levels for 
            calling of LLF_ECPKI_CheckPublKeyCall() function from CRYS functions.          
   
   @param[out] PubKey_ptr      - A pointer to the public key structure. 
                                 This structure is used as input to the ECPKI 
                                 cryptographic primitives.
   @param[in]  CheckMode       - Parameter defines what checking of validity is performed:
                                 preliminary check - 0, partly check - 1, full check - 2.
   @param[in]  tempBuff_ptr    - pointer to temp buffer /not used in PKA HW platform /. 

   @return  CRYS_OK,
            Error from called function
*/
 CRYSError_t  LLF_ECPKI_CheckPublKeyCall(
				    	CRYS_ECPKI_PublKey_t      *PublKey_ptr,        /*in*/ 
				    	EC_PublKeyCheckMode_t      CheckMode,          /*in*/ 
				        DxUint32_t                *tempBuff_ptr        /*in*/ );



/*****************************************************************************************
 *		       LLF_ECPKI_GenKeyPairCall function
 *                        											  
 *****************************************************************************************/
/**
   @brief   This function used as interface between CRYS and LLF levels for 
            calling of LLF_ECPKI_GenKeyPair function from CRYS functions.	  

   @param[in]  DomainID        - The enumerator variable defines current EC domain
   @param[out] UserPrivKey_ptr - A pointer to the private key structure. 
                                 This structure is used as input to the ECPKI 
                                 cryptographic primitives.
   @param[out] UserPubKey_ptr  - A pointer to the public key structure. 
                                 This structure is used as input to the ECPKI 
                                 cryptographic primitives.
   @param[in]  TempData_ptr    - temporary buffers of defined size.

   @return CRYSError_t - CRYS_OK,

*/
CRYSError_t LLF_ECPKI_GenKeyPairCall(
				    	CRYS_ECPKI_DomainID_t  	    DomainID,	        /*in*/   
				    	CRYS_ECPKI_UserPrivKey_t   *UserPrivKey_ptr,    /*out*/
				 		CRYS_ECPKI_UserPublKey_t   *UserPublKey_ptr,    /*out*/   
				        CRYS_ECPKI_KG_TempData_t   *TempData_ptr        /*in*/ );


/*****************************************************************************************
 *		       LLF_ECPKI_DomainInfoAndCheckKey function	
 *                        											  
 *****************************************************************************************/
/**
@brief   The function transfers EC Domain data to CRYS level  and performs preliminary  
         checking of the user private and public keys.

   @param[in] DomainID -            EC Domain identifier.
   @param[in] DomainInfo_ptr -      A pointer to a domain information data .    			   				
   @param[in] PrivKey_ptr         - A pointer to private key (little endian).                         
   @param[in] PrivKeySizeInBytes  - Size of private key .
   @param[in] PublKey_ptr         - A pointer to public key (must containing X,Y coordinats
                                    of size = modulus size each one (little endian).                         
   @param[in] PublKeySizeInBytes  - Size of public key (without point control byte), 
                                    must be equalled 2*ModSizeBytes.
   @param[in] CheckKey            - Parameter, defines whether private ahd public key
                                    must be checked: 1 - check private key, 2 - check 
									public key, 3 - check both keys, 0 - not check keys.
   @return CRYSError_t - CRYS_OK,
                         LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_SIZE_ERROR 
						 LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_DATA_ERROR
						 LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_SIZE_ERROR
                         LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR
**/
CRYSError_t LLF_ECPKI_DomainInfoAndCheckKey(
			CRYS_ECPKI_DomainID_t      DomainID,	       /*in*/							  
			CRYS_ECPKI_DomainInfo_t   *DomainInfo_ptr,     /*in*/
			DxUint32_t 	          *PrivKey_ptr,        /*in*/
			DxUint32_t                 PrivKeySizeInBytes, /*in*/
			DxUint32_t		  *PublKey_ptr,        /*in*/
			DxUint32_t                 PublKeySizeInBytes, /*in*/
			DxInt8_t                   CheckKey            /*in*/ );


/***************************************************************************************/
/*     Macro definitions for calling the LLF_ECPKI_DomainInfoAndCheckKey function      */
/***************************************************************************************/
#define LLF_ECPKI_GetDomainInfo(DomainID,DomainInfo_ptr)\
	LLF_ECPKI_DomainInfoAndCheckKey((DomainID), (DomainInfo_ptr), DX_NULL, 0, DX_NULL, 0, 0)

#define LLF_ECPKI_CheckPrivKeySize( DomainID, PrivKey_ptr, PrivKeySizeInBytes)\
	LLF_ECPKI_DomainInfoAndCheckKey((DomainID), DX_NULL, (PrivKey_ptr), (PrivKeySizeInBytes), DX_NULL, 0, 1)

#define LLF_ECPKI_CheckPublKeySize(DomainID,PublKey_ptr,PublKeySizeInBytes)\
	LLF_ECPKI_DomainInfoAndCheckKey((DomainID), DX_NULL, DX_NULL, 0, (PublKey_ptr), (PublKeySizeInBytes), 2)

/***************************************************************************************/


/*****************************************************************************************
 		         LLF_ECPKI_SVDP_DH function 													  
 *****************************************************************************************/
/**
 @brief:    @brief: 	This function performs calculation of SharedSecretValue according 
                        ECC Diffie-Hellman algorithm (IEEE Std. 1363-2000):
                        1. Computes an elliptic curve point P(x,y) = s*W', 
                           where s - user private key, W' - partner public key.
                        2. Reverse copies and outputs the coordinate x as SharedSecretValue.                        
 
 @param[in]  PartnerPublKey_ptr    - A pointer to a Partner Public Key W 
 @param[in]  UserPrivKey_ptr	   - A pointer to a User Private Key  
 @param[Out] SharedSecretValue_ptr - A pointer to a buffer for output of SharedSecretValue
                                     (in big endian)
 @param[in]  TempBuff_ptr          - A pointer to a temporary buffers
  		
 @return CRYSError_t - CRYS_OK,
 					   Error from called functions
*/
CRYSError_t LLF_ECPKI_SVDP_DH(CRYS_ECPKI_PublKey_t  *PartnerPublKey_ptr,    /*in*/
			   DxUint32_t	            *UserPrivKey_ptr,       /*in*/
			   DxUint32_t		    *SharedSecretValue_ptr, /*out*/
			   CRYS_ECDH_TempData_t     *TempBuff_ptr           /*in*/ );
							   

/*****************************************************************************************
 *		       LLF_ECDSA_SignCalcCall function	/ named ECSP_DSA in IEEE-1363 std./
 *                        with internal or external ephemeral key												  
 *****************************************************************************************/
/**
   @brief: 	This function used as interface between CRYS and LLF levels for 
            calling the LLF_ECDSA_SignCalc() function from CRYS functions.
			
	
   @param[in]  SignerPrivateKey_ptr - A pointer to the User Private Key   
   @param[in]  MessageRepresent_ptr - A pointer to message representative  
   @param[in]  MessageRepresentSizeWord - Size of message representative in words
   @param[in]  IsEphemerKeyInternal, - indicates source for random ephemeral key:
                    IsEphemerKeyInternal, = 1 - source internal,
                    IsEphemerKeyInternal, = 0 - source external from RandEphemeralKey_ptr.
   @param[in]  EphemeralKeyData_ptr - Pointer to source for external ephemeral key data which
               contains: ephemeral priv. key u of length ModSizeInBytes + 4,
                         ephemeral public key Vx and Vy - each of length ModSizeInBytes .
   @param[out] SignatureC_ptr - Pointer to output of C value of signature. 
   @param[out] SignatureD_ptr - Pointer to output of D value of signature. 
   @param[in]  TempBuff_ptr -   Pointer to temporary buffer.                          
   
   @return CRYSError_t - CRYS_OK,    						 			 
						 Error from called functions 
*/
CRYSError_t LLF_ECDSA_SignCalcCall(  
			CRYS_ECPKI_PrivKey_t   *SignerPrivateKey_ptr,      /*in*/							
			DxUint32_t             *MessageRepresent_ptr,      /*in*/ 
			DxUint32_t              MessageRepresentSizeWords, /*in*/
			DxUint32_t              IsEphemerKeyInternal,      /*in*/ 
			DxUint32_t             *EphemerKeyData_ptr,        /*in*/							              
			DxUint32_t             *SignatureC_ptr,            /*out*/
			DxUint32_t             *SignatureD_ptr,            /*out*/
			DxUint32_t             *TempBuff_ptr               /*in*/ );

/*****************************************************************************************
 *		       LLF_ECDSA_VerifyCalcCall function	/ named ECVP_DSA in IEEE-1363 std.
 *****************************************************************************************/
/**
   @brief: 	This function used as interface between CRYS and LLF levels for 
            calling of LLF_ECDSA_SignVerify() function from CRYS functions.
	
			If signature is not valid the function returns CRYS_OK, else returns 
			an error message from called function. 
					
 @param[in]  SignerPublicKey_ptr  - A pointer to the Signer Public Key. 
 @param[in]  MessageRepresent_ptr - A pointer to the message representative.
 @param[in]  SignatureC_ptr       - A pointer to the signature C value. 
 @param[in]  SignatureD_ptr       - A pointer to the signature D value. 
 @param[in]  TempBuff_ptr         - A pointer to temporary buffer.
 		
 @return   CRYSError_t - CRYS_OK,				   
					     Error from called functions
*/
CRYSError_t LLF_ECDSA_VerifyCalcCall( 
			CRYS_ECPKI_PublKey_t   *SignerPublKey_ptr,         /*in*/							
			DxUint32_t             *MessageRepresent_ptr,      /*in*/ 								  							 							              
			DxUint32_t             *SignatureC_ptr,            /*in*/
			DxUint32_t             *SignatureD_ptr,            /*in*/
			DxUint32_t             *TempBuff_ptr               /*in*/ );




/*****************************************************************************************
 *		       LLF_ECPKI_ELGAMAL_Encrypt function	
 *                        											  
 *****************************************************************************************/
/**
   @brief: 	This function performs encryption of the message (transmitted key or any other 
            plain text)  with public key of the receiver and private key of the sender using
			EC Elgamal algorithm. 
			
            Operation: The sender encrypts the message by the following sequence of steps:
			
			1. Checks validity (partially) of receivers public key;
			2. Converts the message f into EC representative point M, such that Xm = f. 
			   Note: Only n-1 first (low) bytes of Xm  are significant and may be recovered 
			   as the message;
			3. Randomly generates senders private key u and calculates public 
			   key V(Xv,Yv)  = uG  with the same set of domain parameters as the receivers 
			   public key W; 
			4. Calculates EC point  B =  uW + M;
			5. Outputs the encrypted message as pair of points (V, B).
		    6. Exits.

			Assumptions: 
			    1.  The input parameters are valid because they are checked on CRYS level.
				    The receiver's public key W is converted from big endian into little endian.
				2.	Public key W and EC domain parameters q, a, b, r, and G are valid and 
				    associated with each other; f >= 0.
				3.	The message size must be less than EC modulus size minus 1.
   @param[in]  receiverPublKey_ptr   - A pointer to receiver public key (key in big endian).  
   @param[in]  EncrMessageOut_ptr    - A pointer to buffer for encrypted message.
   @param[in]  isEphemerKeyInternal  - A parameter defining whether ephemeral key internal (1) or external (0).
   @param[in]  TempData_ptr          - A pointer to temporary buffer.                         

   @return CRYSError_t - CRYS_OK, 
                         Error from called functions						 
*/  
CRYSError_t  LLF_ECPKI_ELGAMAL_Encrypt(
				CRYS_ECPKI_PublKey_t           *receiverPublKey_ptr,     /*in*/
				DxUint32_t   	  	       *EncrMessageOut_ptr,      /*out*/
				DxInt8_t                       isEphemerKeyInternal,    /*in*/
				CRYS_EC_ELGAMAL_TempData_t     *TempData_ptr );


/*****************************************************************************************
 *		       LLF_ECPKI_ELGAMAL_Decrypt function	
 *                        											  
 *****************************************************************************************/
/**
  The Decrypting  primitive uses the following data as input:
     	The EC Domain parameters q, a, b, r, and G  (See Annex A).
    	The  message (V, B), which includes two EC points:  V - the senders public key,  
        associated with the EC domain parameters and B, computed by sender by using 
		message and appropriates keys of sender and receiver.

  Operation: The receiver decrypts the message by the following sequence of steps:
  
    1.  Checks validity of input parameters, 
    2.  Converts  encrypted message (V,B) from big endian to little endian form.
    3.  Checks validity (partially) of senders ephemeral public key V,
    3.  Extracts the message EC representative m(Xm,Ym) from encrypted message:                  
	        m(Xm,Ym) = B - sV, 
    4.  Converts the Xm -coordinate into integer representative i; 
    5.  Outputs the first n-1 bytes of i as decrypted message f   ( n - modulus size).

  Output: Decrypted message f.

  Assumptions: Public key W and EC domain parameters q, a, b, r, and G are 
               valid and associated with each other; f >= 0.

   @param[in] ReceiverPrivKey_ptr - A pointer to a receiver private key structure. 
   @param[in] EncrMessage_ptr     - A pointer to affine points V,B of encrypted message.                         
   @param[out] DecrMessageOut_ptr - A pointer to buffer for output of decrypted message.
   @param[out] MessageOutSizeBytes - Size of the output buffer: must be not less, than ModSizeInBytes. 
   @param[in] TempData_ptr        - A pointer to temporary buffer (size defined in typedef).

   @return CRYSError_t - CRYS_OK,
                         Error from called functions     
*/
CRYSError_t  LLF_ECPKI_ELGAMAL_Decrypt(
				CRYS_ECPKI_PrivKey_t        *ReceiverPrivKey_ptr,  /*in*/	
				DxUint32_t                  *EncrMessage_ptr,      /*in*/
				DxUint32_t	            *DecrMessageOut_ptr,   /*out*/
				DxUint32_t	             MessageOutSizeBytes,  /*in*/
				CRYS_EC_ELGAMAL_TempData_t  *TempData_ptr          /*in*/);




#ifdef __cplusplus
}
#endif

#endif




