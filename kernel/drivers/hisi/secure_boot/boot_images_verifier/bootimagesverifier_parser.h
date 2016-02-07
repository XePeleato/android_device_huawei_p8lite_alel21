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



#ifndef _BOOT_IMAGES_VERIFIER_PARSER_H
#define _BOOT_IMAGES_VERIFIER_PARSER_H


#ifdef __cplusplus
extern "C"
{
#endif

/* Definitions used by the functions */
/*-----------------------------------*/

/* Definitions for vrl size in the vrl header */
/* mask to location of Offset to signature bits in the header VRL size parameter */
#define VRL_LEN_SIGNATURE_OFFSET_BIT_MASK         0x0000FFFFUL

/* mask to Num of components bits in the header VRL size parameter */
#define VRL_LEN_NUM_OF_COMPS_BIT_MASK             0xFFFF0000UL

/* Bit location of the num of copmponents in the header VRL size parameter */
#define VRL_LEN_NUM_OF_COMPS_BIT_LOCATION         16

/* Definitions for vrl flags in the vrl header */
/* SW revocation support bit location in the flag */
#define VRL_FLAG_SW_REV_SUPPORT_BIT_LOCATION      16

/* Secondary VRL exist bit location in the flag */
#define VRL_FLAG_SEC_VRL_EXIST_BIT_LOCATION       17

/* HASH algorithm mask in the header flag */
#define VRL_FLAG_RSA_ALG_BIT_MASK                 0x0000FF00UL

/* HASH algorithm mask in the header flag bit location */
#define VRL_FLAG_RSA_ALG_BIT_LOCATION             8

/* HASH algorithm mask in the header flag */
#define VRL_FLAG_HASH_ALG_BIT_MASK                0x000000FFUL

/* HASH algorithm mask in the header flag bit location */
#define VRL_FLAG_HASH_ALG_BIT_LOCATION            0

/* SW revocation support bit location in the flag */
#define VRL_FLAG_ENCRYPTION_USED_BIT_LOCATION     18



/* Structures used inside the parser code */
/*----------------------------------------*/
/* Table nonce used in composing iv for sw components decryption */
typedef DxUint32_t VRL_Parser_tableNonce_t[2];

/* Data on s/w components */
typedef struct {
	/* Pointer to HASH and load addresses of the s/w comps */
	DxUint32_t  *swHASHData_ptr;

	/* Additional s/w comps data - store addresses + length */
	DxUint32_t  *swAdditionalData_ptr;

	/* Num of s/w comps */
	DxUint32_t  numOfSwComps;

	/* Hash Algorithm to use */
	VRL_HashAlg_t   hashAlg;

	/* HASH size in words */
	DxUint32_t hashSizeWords;

	/* Nonce data */
	VRL_Parser_tableNonce_t nonce;

	/* Indicator if SW image is encrypted */
	DxUint8_t isSwComponentEncrypted;

}VRL_Parser_CompsData_t;

/* RSA data (united to one structure) */
typedef struct {
	/* Pointer to N */
	DxUint32_t  *N_ptr;

	/* Pointer to Np OR H according to algorithm used */
	DxUint32_t  *NDer_ptr;

	/* Pointer to RSA signature */
	DxUint32_t *signature;

	/* RSA algorithm */
	VRL_RSAAlg_t rsaAlg;

	/* Size of VRL fpr HASH computation (offset to signature) */
	DxUint32_t   vrlSizeInWordsForHash;

}VRL_Parser_RSAData_t;

/* 3. SW version structure */
typedef struct {
	/* Current SW version */
	DxUint32_t currSwVersion;

	/* Minimum allowed SW version */
	DxUint32_t minAllowedSwVersion;

	/* A flag indicating if the sw version is supported */
	DxUint8_t  swVersionSupported;

}VRL_Parser_SW_Version_t;



/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/** 
 * @brief This function is resposible for loading the VRL from the flash.
 *        The function will read the VRL header from the flash (using the Flash_read function), 
 *        get its size and load the entire VRL accordingly.
 *         
 *
 * @param[in] flashRead_func - User's Flash read function used to read data from the flash to memory location
 * @param[in] userContext - User's context for the usage of flashRead_func
 * @param[in] VRLAddress - Address of the VRL to load
 * @param[in] expectedMagicNumber - Magic number from the user to compare to, if 0 than the DX default value is used
 * @param[out] VRL_ptr - temporary buffer to load the VRL to, will hold the VRL at the end of the function
 * @param[in/out] VRLBufferSize - the temporary buffer size in bytes, in return it will hold the size of the 
 *                VRL in bytes
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */

DxError_t DX_BIV_VRLParserLoadVRL(DX_SB_FlashRead flashRead_func,
				  void *userContext,
				  DxUint32_t VRLAddress,
				  DxUint32_t expectedMagicNumber,
				  DxUint32_t *VRL_ptr,
				  DxUint32_t *VRLBufferSize);


/** 
* @brief This function is responsible to parsing the VRL. The function gets a VRL
*        pointer parse it and do the following verifications:
*        1. Verify the public key hash (with the one saved in the OTP/NVM and sent to the function)
*        2. Verify the SW minimum version
*        3. Verify RSA signature
*        4. If all verification passed the function return a pointer to the components HASH section
*           in the VRL and a pointer to the SW components addtitional data section in the VRL.
*         
*  
* @param[in] isPrimaryVRL - a flag indicating if its primary or secondary VRL (primary = DX_TRUE) 
* @param[in] hwBaseAddress - base address for the CC HW engines 
* @param[in] NHASH_ptr - a pointer to HASH of the public key
* @param[in] swMinVersion - SW minimum version as it was saved in the OTP/NVM
* @param[in] VRL_ptr - the VRL loaded to RAM
* @param[out] secVrlAddress_ptr - secondary VRL address is a secondary VRL exist 
* @param[in/out] secondaryNHash - the secondary N HASH as saved in the Primary VRL (if primary VRL - will be 
*                filled with the secondary N HASH if in secondary vrl will be verified against the VRL N)
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_VRLParser(DxUint8_t  isPrimaryVRL,
			   DxUint32_t hwBaseAddress,                           
			   DxUint32_t *NHASH_ptr,
			   DxUint32_t swMinVersion,
			   DxUint32_t *VRL_ptr,
			   VRL_SecAddress_t *secVrlAddress_ptr,                           
			   DxUint32_t *secondaryNHash_ptr );


/** 
* @brief This function returns the public key hash
*                   
* @param[in] SB_GetOTPHASH - function pointer, the function 
*       must return the public key hash value that is saved in
*       the OTP
* @param[in] hwBaseAddress - CC base address 
* @param[out] pubKeyHASH - will contain the HASH of the public 
*       key
* @param[in] pubKeyIndex - the public key in dex in the OTP 
* @param[in] externalHash - contains the Hash of public key in 
*       case it is not in the OTP
*  
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_GetPrimaryKeyHash(Dx_SB_GetOTPHASH SB_GetOTPHASH, DxUint32_t hwBaseAddress, HASH_Result_t pubKeyHASH,
				   DxUint32_t pubKeyIndex, DxUint32_t *externalHash);

/** 
* @brief This function parses the VRL and returns the VRL size (including the VRL header)
*                   
* @param[in] VRL_ptr - pointer to the VRL
* @param[out] vrlSize - the returned vrl size in words
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
void DX_BIV_ParseVrlGetVrlSize(DxUint32_t *VRL_ptr, DxUint32_t *vrlSizeWords);

/** 
* @brief This function parses the VRL and returns pointers to the SW components data (and if secondary VRL supported)
*                   
* @param[in] VRL_ptr - pointer to the VRL
* @param[out] swCompsData_ptr - returned sw comps data (HASH + memory load addresses section, num of components,
*             storage address + size section, hash algorithm)
* @param[out] isSecSupported - returned TRUE or FALSE according to secondary address (if exist)
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_ParseVrlGetSWComponentData(DxUint32_t *vrl_Ptr, VRL_Parser_CompsData_t *swCompsData_ptr,
					    DxUint8_t *isSecSupported);

/** 
* @brief This function is responsible to get the secondary relevant data from primary VRL.
*        It should be called only if secondary VRL is supported.
*	 The function parses the vrl and retrieves the secondary N hash and the secondary vrl address
*                   
* @param[in] VRL_ptr - pointer to the VRL
* @param[out] nHashData_ptr - contains the secondary N hash
* @param[in] hashSizeInWords - size of HASH to copy
* @param[out] secAddress_ptr - contains the secondary VRL address
*
* @return void
*/
void DX_BIV_ParseVrlGetSecData(DxUint32_t *vrl_ptr, DxUint32_t *nHashData_ptr, DxUint32_t hashSizeInWords, 
			       DxUint32_t *secAddress_ptr);

#ifdef __cplusplus
}
#endif

#endif


