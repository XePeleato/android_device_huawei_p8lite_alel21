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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/

#include "secureboot_error.h"
#include "secureboot_basetypes.h"
#include "rsa.h"
#include "sb_hash.h"
#include "bootimagesverifier_def.h"
#include "bootimagesverifier_error.h"
#include "util.h"
#include "secureboot_gen.h"
#include "bootimagesverifier_parser.h"
#include "nvm.h"

#include "dx_pal_log.h"

/************************ Defines ******************************/
void UTIL_MemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );
DxUint32_t UTIL_MemCmp( DxUint8_t *buff1_ptr , DxUint8_t *buff2_ptr , DxUint32_t size );
void UTIL_ReverseMemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );
void UTIL_MemSet( DxUint8_t *buff_ptr , DxUint8_t val , DxUint32_t size );
/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Internal Functions ******************************/

/** 
 * @brief This function calculates the HASH over the PubKey (N, big endian) || Np (reversed - little endian).
 *        The function gets the Public key pointer and Np (Barrett n value) from the VRL and the 
 *        HASH from the OTP/NVM in case of a primary VRL or from the primary VRL in case of a secondary VRL,
 *        calculates HASH over the VRL N and Np and compares it.
 *                  
 * @param[in] hwBaseAddr -  cryptocell base address  
 * @param[in] NAndRevNp_ptr - pointer to N public key and Np in the VRL
 * @param[in] NHASH_ptr - a pointer to HASH of the public key
 * @param[in] HashSize - hash size (to compare) 
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_CalcPublicKeyHASHAndCompare(DxUint32_t hwBaseAddress,
					     DxUint32_t *NAndRevNp_ptr, 
					     HASH_Result_t NHASH_ptr,
					     DxUint32_t    HashSize);

/** 
 * @brief This function calculates the HASH over the Exp||PubKey.
 *        The function gets the Public key pointer from the VRL and the HASH from the
 *        OTP/NVM, calculates HASH over the VRL N and compares it.
 *        E is constant - RSA_EXP_VAL.
 *                  
 * @param[in] hwBaseAddr -  cryptocell base address  
 * @param[in] VRL_ptr - pointer to the VRL
 * @param[in] NParams_ptr - a pointer to the public key parameters
 * @param[in] vrlSignature_ptr - a pointer to the VRL signature structure from the VRL
 * @param[in] sizeOfVRL - size of the VRL to calculate the HASH on
 * @param[in] RSAAlg - RSA algorithm to use
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_VerifyVRLSignature(DxUint32_t hwBaseAddress, 
				    DxUint32_t *VRL_ptr, 
				    VRL_N_Params_t *NParams_ptr,
				    VRL_Sign_t *vrlSignature_ptr, 
				    DxUint32_t sizeOfVRL,
				    VRL_RSAAlg_t RSAAlg);

/** 
 * @brief This function parses the VRL and returns pointers to its different components
 *                   
 * @param[in] VRL_ptr - pointer to the VRL
 * @param[out] isSecondaryExist_ptr - returned secondary VRL flag
 * @param[out] rsaData_ptr - returned RSA data (N, Np, signature, RSA algorithm)
 * @param[out] swVersion_ptr - returned sw version parameters (if does not supported retunred DX_NULL)
 * @param[out] secHash_ptr - returned secondary hash pointer (if exist, else returned DX_NULL)
 * @param[out] secAddress_ptr - returned secondary VRL address (if doesnt exist will be filled with 0)
 * @param[out] hashSize_ptr - hash size in words according to the hash algorithm
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_InternalVRLParse(DxUint32_t *VRL_ptr, DxUint8_t *isSecondaryExist_ptr,
				  VRL_Parser_RSAData_t *rsaData_ptr, VRL_Parser_SW_Version_t *swVersion_ptr, 
				  DxUint32_t **secHash_ptr, VRL_SecAddress_t *secAddress_ptr, 
				  DxUint32_t *hashSize_ptr);  


/************************ Public Functions ******************************/


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
				  DxUint32_t *VRLBufferSize)
{
/* error variable */
DxError_t error = DX_OK;

/* temp vrl header */
VRL_Header_t *vrlHeader_ptr = (VRL_Header_t*)VRL_ptr;

/* VRL size */  
DxUint32_t   sizeOfVrl = 0;

/* number of sw components signed on in the VRL */
DxUint16_t   numOfComps = 0, sizeToRead = 0;

/* temp VRL address */
DxUint32_t tempAddr = VRLAddress;
/*------------------
    CODE
-------------------*/
/* In case there is additional data - need to skip it */
#ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED  
/* set the temporary address to after the additional data header */
tempAddr = VRLAddress + ADDITIONAL_DATA_SIZE_IN_BYTES;
sizeToRead = ADDITIONAL_DATA_SIZE_IN_BYTES;
#if DX_SB_RAM_USAGE_ONLY
/* In case the VRL_ptr already holds the entire VRL we need to forward the header pointer to after the 
   additional data */
vrlHeader_ptr = (VRL_Header_t*)((DxUint32_t*)vrlHeader_ptr + ADDITIONAL_DATA_SIZE_IN_WORDS);
#endif
#endif
/* Read the VRL header from the Flash */
error = flashRead_func(tempAddr, (DxUint8_t*)vrlHeader_ptr, sizeof(VRL_Header_t), userContext);

if (error != DX_OK)
	return error;

/* Verify Magic number */
/*---------------------*/
if (expectedMagicNumber == 0) {
	if (vrlHeader_ptr->magicNumber != DX_VRL_MAGIC_NUMBER_DEFAULT_VALUE){
		DX_PAL_LOG_DEBUG("magicNumber != default magicNumber\n");
		return DX_BOOT_IMG_VERIFIER_VRL_MAGIC_NUM_INCORRECT;
	}
} else {
	if (vrlHeader_ptr->magicNumber != expectedMagicNumber){
		DX_PAL_LOG_DEBUG("magicNumber != expected magicNumber\n");
		return DX_BOOT_IMG_VERIFIER_VRL_MAGIC_NUM_INCORRECT;
	}
}
/* Calculate the VRL size according to offset to signature + sec address + num of comps * (comps params) */
/*-------------------------------------------------------------------------------------------------------*/
/* Get the number of sw comnponents from the header vrlSize field */
numOfComps = (vrlHeader_ptr->vrlSize & VRL_LEN_NUM_OF_COMPS_BIT_MASK ) >> VRL_LEN_NUM_OF_COMPS_BIT_LOCATION;

sizeOfVrl = (DxUint32_t)(vrlHeader_ptr->vrlSize & VRL_LEN_SIGNATURE_OFFSET_BIT_MASK) /* signature offset */
	    + sizeof(VRL_Sign_t)/sizeof(DxUint32_t) /* signature */
	    + sizeof(VRL_SecAddress_t)/sizeof(DxUint32_t) /* secondary address */
	    + numOfComps * (sizeof(VRL_ParamRecordInfo_t)/sizeof(DxUint32_t)); /* records parameter * num of records*/


/* Load the VRL */
/*--------------*/
/* Verify that the VRL buffer size is big enough */
if (*VRLBufferSize < sizeOfVrl*sizeof(DxUint32_t)){
	DX_PAL_LOG_DEBUG("VRL buff size too small\n");
	return DX_BOOT_IMG_VERIFIER_WORKSPACE_SIZE_TOO_SMALL;
}
/* according to the header read the entire VRL table and save it in the user buffer - 
   VRL_ptr */
sizeToRead = sizeToRead + sizeOfVrl*sizeof(DxUint32_t);
error = flashRead_func(VRLAddress, (DxUint8_t*)VRL_ptr, sizeToRead, userContext);
if (error != DX_OK)
	return error;

/* Set the VRL size into output parameter */
*VRLBufferSize = sizeToRead;


return DX_OK;

}/* End of DX_BIV_VRLParserLoadVRL */

static void COUNT_ONE_BITS(DxUint32_t number, DxUint32_t* BitCount) 
{
 DxUint32_t tmp_num = number;
 *BitCount = 0;
 while (tmp_num)
   {
     tmp_num = tmp_num & (tmp_num - 1);
     *BitCount = *BitCount + 1;
   }
 } 


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
			   DxUint32_t *secondaryNHash_ptr )
{
/* error variable */
DxError_t error = DX_OK;

/* flag for secondary VRL */
DxUint8_t isSecExist = DX_FALSE;

/* RSA data */
VRL_Parser_RSAData_t rsaData;

/* sw version struct */
VRL_Parser_SW_Version_t swVersion;

/* pointer to hash section */
DxUint32_t *secHash_ptr = VRL_ptr;

/* temporary parameters */
DxUint32_t hashSizeToCompare = 0;  

/*------------------
    CODE
-------------------*/

/* Parse the VRL, get pointers to its different sections */
/*-------------------------------------------------------*/
error = DX_BIV_InternalVRLParse(VRL_ptr, &isSecExist, &rsaData, 
				&swVersion, &secHash_ptr, secVrlAddress_ptr, &hashSizeToCompare);
if (error != DX_OK)
	return error;

/* 1. Verify that the SW version is larger than the saved minimum SW version, 
     if SW revocation is supported (SW revocation can only be supported in primary VRL) */
  /*------------------------------------------------------------------------------------*/  
if (swVersion.swVersionSupported){	
	DxUint32_t currSwVersion_bitcount = 0, minAllowedSwVersion_bitcount = 0;	
	COUNT_ONE_BITS(swVersion.currSwVersion, &currSwVersion_bitcount);
	COUNT_ONE_BITS(swVersion.minAllowedSwVersion, &minAllowedSwVersion_bitcount);
	
	if (currSwVersion_bitcount < swMinVersion){
		DX_PAL_LOG_DEBUG("currSwVersion < swMinVersion\n");
		return DX_BOOT_IMG_VERIFIER_SW_VER_SMALLER_THAN_MIN_VER;
	}
    
	/* Check if current version in the VRL is larger than the minimum version saved on the 
	VRL */
	if (currSwVersion_bitcount < minAllowedSwVersion_bitcount) 
		return DX_BOOT_IMG_VERIFIER_VRL_CUR_VER_SMALLER_THAN_VRL_MIN_VER;  
}

/* 2. Compute HASH over the public key and exponent from the VRL and compare to given HASH */
/*-----------------------------------------------------------------------------------------*/
/* Verify public key HASH, if in primary VRL NHASH_ptr points to data from the OTP/NVM,
   is in secondary NHASH_ptr points to data taken from the primary VRL */
if (isPrimaryVRL) {
	/* The RSA struct is ordered to first point to N and than to Np (to calculate the HASH need N & Np) */
	error = DX_BIV_CalcPublicKeyHASHAndCompare(hwBaseAddress, rsaData.N_ptr, NHASH_ptr, hashSizeToCompare);
	if (error != DX_OK)
		return error;

	/* In case Secondary VRL exist, save the HASH of the secondary N & Np */
	if (isSecExist) {
		/* Copy the secondary HASH */   
	  UTIL_MemCopy((DxUint8_t*)secondaryNHash_ptr, (DxUint8_t*)secHash_ptr, (DxUint32_t)hashSizeToCompare);
        }
}/* End of Primary VRL */
else { /* Secondary VRL */
  error = DX_BIV_CalcPublicKeyHASHAndCompare(hwBaseAddress, rsaData.N_ptr, secondaryNHash_ptr, hashSizeToCompare);
	if (error != DX_OK)
		return error;
}

/* 3. Verify RSA signature */
/*-------------------------*/  
/* Verify the VRL signature */

error = DX_BIV_VerifyVRLSignature(hwBaseAddress, VRL_ptr, (VRL_N_Params_t*)rsaData.N_ptr, 
				  (VRL_Sign_t*)rsaData.signature, rsaData.vrlSizeInWordsForHash, 
				  rsaData.rsaAlg);
if (error != DX_OK)
	return error;


return DX_OK;  

}/* End of DX_BIV_VRLParser */


/** 
 * @brief This function calculates the HASH over the PubKey (N, big endian) || Np (reversed - little endian).
 *        The function gets the Public key pointer and Np (Barrett n value) from the VRL and the 
 *        HASH from the OTP/NVM in case of a primary VRL or from the primary VRL in case of a secondary VRL,
 *        calculates HASH over the VRL N and Np and compares it.
 *                  
 * @param[in] hwBaseAddr -  cryptocell base address  
 * @param[in] NAndRevNp_ptr - pointer to N public key and Np in the VRL
 * @param[in] NHASH_ptr - a pointer to HASH of the public key
 * @param[in] HashSize - hash size (to compare) 
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_CalcPublicKeyHASHAndCompare(DxUint32_t hwBaseAddress,
					     DxUint32_t *NAndRevNp_ptr, 
					     HASH_Result_t NHASH_ptr,
					     DxUint32_t    HashSize)
{  
/* error variable */
DxError_t error = DX_OK;

/* HASH result of the E||N */
HASH_Result_t LocalHashResult;

/*------------------
    CODE
-------------------*/ 

/* calculate the HASH value of N (big endian)|| Np (reversed - little endian) */

/* initialize the HASH */
HASH_Init(hwBaseAddress);

/* HASH using DMA */                      
HASH_UpdateData ( hwBaseAddress,
		  NAndRevNp_ptr,                
		  SB_RSA_MOD_SIZE_IN_WORDS + SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS,
		  SB_RSA_MOD_SIZE_IN_WORDS + SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS,
		  DX_TRUE,
		  DX_FALSE );

/* get the results */
HASH_GetResults( hwBaseAddress, LocalHashResult );
/* compare the HASH results */
error = UTIL_MemCmp( (DxUint8_t*)LocalHashResult , (DxUint8_t*)NHASH_ptr , HashSize);
if (error != DX_TRUE){
	DX_PAL_LOG_DEBUG("PUB KEY HASH VALIDATION FAILURE\n");
	return DX_BOOT_IMG_VERIFIER_PUB_KEY_HASH_VALIDATION_FAILURE;
}

return DX_OK;
}/* End of DX_BIV_CalcPublicKeyHASHAndCompare */


/** 
 * @brief This function calculates the HASH over the Exp||PubKey.
 *        The function gets the Public key pointer from the VRL and the HASH from the
 *        OTP/NVM, calculates HASH over the VRL N and compares it.
 *        E is constant - RSA_EXP_VAL.
 *                  
 * @param[in] hwBaseAddr -  cryptocell base address  
 * @param[in] VRL_ptr - pointer to the VRL
 * @param[in] NParams_ptr - a pointer to the public key parameters
 * @param[in] vrlSignature_ptr - a pointer to the VRL signature structure from the VRL
 * @param[in] sizeOfVRL - size of the VRL to calculate the HASH on
 * @param[in] RSAAlg - RSA algorithm to use
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_VerifyVRLSignature(DxUint32_t hwBaseAddress, 
				    DxUint32_t *VRL_ptr, 
				    VRL_N_Params_t *NParams_ptr,
				    VRL_Sign_t *vrlSignature_ptr, 
				    DxUint32_t sizeOfVRL,
				    VRL_RSAAlg_t RSAAlg)
{
/* error variable */
DxError_t error = DX_OK;

/* a HASH result variable */
HASH_Result_t HashResult; 

/* reversed N public key */
DxUint32_t RevN[SB_RSA_MOD_SIZE_IN_WORDS];
DxUint32_t RevNp[SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS];

/*------------------
    CODE
-------------------*/ 

/* Currently only RSA_PSS_2048 is supported */
#ifndef DX_SB_SEC_BOOT_PKCS_V15
if (RSAAlg != RSA_PSS_2048){
        DX_PAL_LOG_DEBUG("UNSUPPORTED RSA ALGORITHM\n");
	return DX_BOOT_IMG_VERIFIER_UNSUPPORTED_RSA_ALGORITHM;
}
#else
if (RSAAlg != RSA_PKCS15_2048){
        DX_PAL_LOG_DEBUG("UNSUPPORTED RSA ALGORITHM\n");
	return DX_BOOT_IMG_VERIFIER_UNSUPPORTED_RSA_ALGORITHM;}
#endif

/* Calculate HASH on the VRL table */
/*-------------------------------- */
/* calc the VRL HASH according to the length minus the signature struct size (N,Np & signature)*/
HASH_HashOnDataBlock( hwBaseAddress,
		      VRL_ptr,
		      sizeOfVRL,
		      DX_NULL,
		      HashResult,
		      DX_FALSE);


/* Verify the RSA signature of the VRL table */
/*-------------------------------------------*/
/* Reverse the N for the RSA usage, Np is already set in little endian */
UTIL_ReverseMemCopy( (DxUint8_t*) RevN, (DxUint8_t*) NParams_ptr->N, RSA_MOD_SIZE_IN_BYTES);
UTIL_ReverseMemCopy( (DxUint8_t*) RevNp, (DxUint8_t*) NParams_ptr->Np, sizeof(DxUint32_t)*SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS);


/* Verify the RSA signature */
#ifndef DX_SB_SEC_BOOT_PKCS_V15
error = RSA_PSS_Verify( hwBaseAddress, HashResult, RevN , RevNp , vrlSignature_ptr->sig );    
#else
error = RSA_PKCS15_Verify( hwBaseAddress, HashResult, RevN , RevNp , vrlSignature_ptr->sig );
#endif

/* on failure exit with an error code */
if (error != DX_TRUE) {
	DX_PAL_LOG_DEBUG("RSA sig verification faild\n");
	return DX_BOOT_IMG_VERIFIER_RSA_SIG_VERIFICATION_FAILED;
}

return DX_OK;

}/* End of DX_BIV_VerifyVRLSignature */


/** 
* @brief This function parses the VRL and returns pointers to its different components
*                   
* @param[in] VRL_ptr - pointer to the VRL
* @param[out] isSecondaryExist_ptr - returned secondary VRL flag
* @param[out] rsaData_ptr - returned RSA data (N, Np, signature, RSA algorithm)
* @param[out] swVersion_ptr - returned sw version parameters (if does not supported retunred DX_NULL)
* @param[out] secHash_ptr - returned secondary hash pointer (if exist, else returned DX_NULL)
* @param[out] secAddress_ptr - returned secondary VRL address (if doesnt exist will be filled with 0)
* @param[out] hashSize_ptr - hash size in words according to the hash algorithm
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_InternalVRLParse(DxUint32_t *VRL_ptr, DxUint8_t *isSecondaryExist_ptr,
				  VRL_Parser_RSAData_t *rsaData_ptr, VRL_Parser_SW_Version_t *swVersion_ptr, 
				  DxUint32_t **secHash_ptr, VRL_SecAddress_t *secAddress_ptr, 
				  DxUint32_t *hashSize_ptr)
{
/* the VRL header data */
VRL_Header_t *vrlHeader;  

/* offset from VRL start */
DxUint32_t offset = 0;

/* temporary parameters */
DxUint32_t hashSize = 0;

/* temporary pointer */
DxUint32_t *tempPtr = VRL_ptr;
/*------------------
    CODE
-------------------*/

/* In case there is additional data - need to skip it */
#ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED  
/* set the temporary address to after the additional data header */
tempPtr = VRL_ptr + ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t);
#endif

/* Point to the VRL header */
vrlHeader = (VRL_Header_t*)tempPtr;

/* Parse the VRL */
/*---------------*/  
/* 1. From the VRL header - secondary VRL exist flag */
*isSecondaryExist_ptr = (vrlHeader->vrlFlags & (0x1 << VRL_FLAG_SEC_VRL_EXIST_BIT_LOCATION)) >> VRL_FLAG_SEC_VRL_EXIST_BIT_LOCATION;

/* 2. Get RSA parameters (N and Np) */
/* Point to N, located in the VRL after the header */
offset = sizeof(VRL_Header_t)/sizeof(DxUint32_t);
rsaData_ptr->N_ptr = tempPtr + offset;

/* Point to Np */
offset = offset + SB_RSA_MOD_SIZE_IN_WORDS;
rsaData_ptr->NDer_ptr = tempPtr + offset;

/* Get the RSA algorithm */
rsaData_ptr->rsaAlg = (VRL_RSAAlg_t)(vrlHeader->vrlFlags & VRL_FLAG_RSA_ALG_BIT_MASK) >> VRL_FLAG_RSA_ALG_BIT_LOCATION;

/* 3. Get the sw revocation parameters */
offset = offset + RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS;
if (vrlHeader->vrlFlags & (0x1 << VRL_FLAG_SW_REV_SUPPORT_BIT_LOCATION)) {
	UTIL_MemCopy((DxUint8_t*)swVersion_ptr, (DxUint8_t*)(tempPtr + offset), sizeof(VRL_SW_Version_t));
	swVersion_ptr->swVersionSupported = DX_TRUE;
} else {
	swVersion_ptr->swVersionSupported = DX_FALSE;
}
offset = offset + sizeof(VRL_SW_Version_t)/sizeof(DxUint32_t);

if (*isSecondaryExist_ptr) {
	*secHash_ptr = tempPtr + offset;          
} else {
	*secHash_ptr = DX_NULL;
}    

/* The next offset is set according to HASH algorithm (hash output size) */    
/* Switch on the HASH algorithm to decide output size (in words) */
switch ((VRL_HashAlg_t)(vrlHeader->vrlFlags & VRL_FLAG_HASH_ALG_BIT_MASK)) {
	case HASH_SHA256_Alg_Output:
		hashSize = sizeof(HASH_Result_t);
		break;

	case HASH_SHA256_Alg_128_Output:
		hashSize = sizeof(HASH_Result_t)/2;
		break;

	default:
		DX_PAL_LOG_DEBUG("unsupported HASH algorithem\n");
		return DX_BOOT_IMG_VERIFIER_UNSUPPORTED_HASH_ALGORITHM;  
}


*hashSize_ptr = hashSize;

/* Point to RSA signature , set the VRL size for HASH (offset to signature) */
offset = vrlHeader->vrlSize & VRL_LEN_SIGNATURE_OFFSET_BIT_MASK;

#ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED  
rsaData_ptr->vrlSizeInWordsForHash = ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t) + offset;
#else
rsaData_ptr->vrlSizeInWordsForHash = offset ;
#endif

rsaData_ptr->signature = tempPtr + offset;

/* The offset is set to the secondary address */
offset = offset + sizeof(VRL_Sign_t)/sizeof(DxUint32_t); /* signature */

secAddress_ptr->SecVrlAddress = *(tempPtr + offset);

return DX_OK;

}/* End of DX_BIV_InternalVRLParse */


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
				   DxUint32_t pubKeyIndex, DxUint32_t *externalHash)
{
/* error return */
DxError_t error = DX_OK;

/*------------------
    CODE
-------------------*/
/* In case the OTP index points to external hash the PubKeyHash will get value from the 
   external HASH */
if (pubKeyIndex == DX_SB_HASH_INDEX_NOT_USE_INTERNAL_HASH) {
	UTIL_MemCopy((DxUint8_t*)pubKeyHASH, (DxUint8_t*)externalHash, DX_SB_HASH_LENGTH_IN_WORDS*sizeof(DxUint32_t));
} else {	
	if (pubKeyIndex > DX_SB_MAX_NUM_OF_OTP_KEYS || pubKeyIndex == DX_SB_MAX_NUM_OF_OTP_KEYS) {		
		return DX_BOOT_IMG_VERIFIER_PUB_KEY_INDEX_EXCEEDS_LIMITATION;
	}
	error = SB_GetOTPHASH(hwBaseAddress, pubKeyIndex, (DxUint32_t*)pubKeyHASH);	
}

return error;
}

/** 
* @brief This function parses the VRL and returns the VRL size (including the VRL header)
*                   
* @param[in] VRL_ptr - pointer to the VRL
* @param[out] vrlSize - the returned vrl size in words
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
void DX_BIV_ParseVrlGetVrlSize(DxUint32_t *VRL_ptr, DxUint32_t *vrlSizeWords)
{

/* the VRL header data */
VRL_Header_t *vrlHeader;  

/* number of components, vrl size*/
DxUint32_t numOfComps, sizeOfVrl;

/*------------------
    CODE
-------------------*/

vrlHeader = (VRL_Header_t*)VRL_ptr;

/* Get the number of sw comnponents from the header vrlSize field */
numOfComps = (vrlHeader->vrlSize & VRL_LEN_NUM_OF_COMPS_BIT_MASK ) >> VRL_LEN_NUM_OF_COMPS_BIT_LOCATION;

sizeOfVrl = (DxUint32_t)(vrlHeader->vrlSize & VRL_LEN_SIGNATURE_OFFSET_BIT_MASK) /* signature offset */
	    + sizeof(VRL_Sign_t)/sizeof(DxUint32_t) /* signature */
	    + sizeof(VRL_SecAddress_t)/sizeof(DxUint32_t) /* secondary address */
	    + numOfComps * (sizeof(VRL_ParamRecordInfo_t)/sizeof(DxUint32_t)); /* records parameter * num of records*/

#ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED
sizeOfVrl = sizeOfVrl + ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t);
#endif
*vrlSizeWords = sizeOfVrl;

}/* End of DX_BIV_ParseVrlGetVrlSize */

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
					    DxUint8_t *isSecSupported)
{
/* the VRL header data */
VRL_Header_t *vrlHeader;  

/* offset from VRL start */
DxUint32_t offset = 0;

/* temporary pointer */
DxUint32_t *tempPtr = vrl_Ptr;
/*------------------
    CODE
-------------------*/

/* In case there is additional data - need to skip it */
#ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED  
/* set the temporary address to after the additional data header */
tempPtr = vrl_Ptr + ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t);
#endif

/* Point to the VRL header */
vrlHeader = (VRL_Header_t*)tempPtr;

/* Jump over: header + RSA key + barrett + sw ver */
offset = sizeof(VRL_Header_t)/sizeof(DxUint32_t) + SB_RSA_MOD_SIZE_IN_WORDS + 
	 SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS + sizeof(VRL_SW_Version_t)/sizeof(DxUint32_t);

/* check if SW components is encrypted */
if (vrlHeader->vrlFlags & (0x1 << VRL_FLAG_ENCRYPTION_USED_BIT_LOCATION)) {
	swCompsData_ptr->isSwComponentEncrypted = DX_TRUE;
	UTIL_MemCopy((DxUint8_t*)swCompsData_ptr->nonce, (DxUint8_t*)(tempPtr + offset), sizeof(VRL_Parser_tableNonce_t));
	offset += sizeof(VRL_Parser_tableNonce_t)/sizeof(DxUint32_t);
}
else {
	swCompsData_ptr->isSwComponentEncrypted = DX_FALSE;
  	UTIL_MemSet((DxUint8_t*)swCompsData_ptr->nonce, 0, sizeof(VRL_Parser_tableNonce_t));
}

/* Get the HASH size */    
swCompsData_ptr->hashAlg = (VRL_HashAlg_t)(vrlHeader->vrlFlags & VRL_FLAG_HASH_ALG_BIT_MASK);

/* Switch on the HASH algorithm to decide output size (in words) */
switch (swCompsData_ptr->hashAlg) {
	case HASH_SHA256_Alg_Output:
		swCompsData_ptr->hashSizeWords = sizeof(HASH_Result_t)/sizeof(DxUint32_t);
		break;

	case HASH_SHA256_Alg_128_Output:
		swCompsData_ptr->hashSizeWords = (sizeof(HASH_Result_t)/2)/sizeof(DxUint32_t);
		break;

	default:
		DX_PAL_LOG_DEBUG("unsupported HASH algorithem\n");
		return DX_BOOT_IMG_VERIFIER_UNSUPPORTED_HASH_ALGORITHM;  
}
/* increment the offset according to the secondary VRL HASH size */
offset = offset + swCompsData_ptr->hashSizeWords;

/* Get the sw components data */
/* Num of components */
swCompsData_ptr->numOfSwComps = (vrlHeader->vrlSize & VRL_LEN_NUM_OF_COMPS_BIT_MASK ) >> VRL_LEN_NUM_OF_COMPS_BIT_LOCATION;

/* sw comps HASH section */  
swCompsData_ptr->swHASHData_ptr = tempPtr + offset;

/* Point to RSA signature , set the VRL size for HASH (offset to signature) */
offset = vrlHeader->vrlSize & VRL_LEN_SIGNATURE_OFFSET_BIT_MASK;

/* The offset is set to the additional data parameters section , after the signature and secondary address */
offset = offset + sizeof(VRL_Sign_t)/sizeof(DxUint32_t) + sizeof(VRL_SecAddress_t)/sizeof(DxUint32_t);

/* sw components additional parameters section */    
swCompsData_ptr->swAdditionalData_ptr = tempPtr + offset;

/* offset points to secondary address */ 
if (isSecSupported != DX_NULL) {
	*isSecSupported = (vrlHeader->vrlFlags & (0x1 << VRL_FLAG_SEC_VRL_EXIST_BIT_LOCATION)) >> VRL_FLAG_SEC_VRL_EXIST_BIT_LOCATION;
}

return DX_OK;
}/* End of DX_BIV_ParseVrlGetSWComponentData */

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
			       DxUint32_t *secAddress_ptr)
{
/* the VRL header data */
VRL_Header_t *vrlHeader;  

/* offset from VRL start */
DxUint32_t offset = 0;

/* VRL flags */
DxUint32_t vrlFlags = 0;

/* temporary pointer */
DxUint32_t *tempPtr = vrl_ptr;
/*------------------
CODE
-------------------*/

/* In case there is additional data - need to skip it */
#ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED  
/* set the temporary address to after the additional data header */
tempPtr = vrl_ptr + ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t);
#endif

/* Point to the VRL header */
vrlHeader = (VRL_Header_t*)tempPtr;

/* get the secondary HASH */
offset = sizeof(VRL_Header_t)/sizeof(DxUint32_t) + SB_RSA_MOD_SIZE_IN_WORDS + SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS +
	 sizeof(VRL_SW_Version_t)/sizeof(DxUint32_t);

vrlFlags = *(tempPtr + SB_VRL_FLAG_OFFSET_IN_WORDS); 

/* check if SW components is encrypted - if so, skip nonce words*/
if ( vrlFlags & (0x1 << VRL_FLAG_ENCRYPTION_USED_BIT_LOCATION)) {    
	offset += sizeof(VRL_Parser_tableNonce_t)/sizeof(DxUint32_t);
}

UTIL_MemCopy((DxUint8_t*)nHashData_ptr, (DxUint8_t*)(tempPtr + offset), hashSizeInWords*sizeof(DxUint32_t));

/* Point to RSA signature , set the VRL size for HASH (offset to signature) */
offset = vrlHeader->vrlSize & VRL_LEN_SIGNATURE_OFFSET_BIT_MASK;

/* get the secondary address = offset to the RSA signature + size of signature all in wards*/
 *secAddress_ptr = *(tempPtr + offset + SB_RSA_MOD_SIZE_IN_WORDS); 




return;
}/* End of DX_BIV_ParseVrlGetSecData */

