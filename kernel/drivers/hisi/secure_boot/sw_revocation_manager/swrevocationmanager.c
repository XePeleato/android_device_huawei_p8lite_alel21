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

#include "secureboot_basetypes.h"
#include "secureboot_error.h"
#include "swrevocationmanager_error.h"
#include "sb_hash.h"
#include "nvm.h"
#include "bootimagesverifier_def.h"
#include "bootimagesverifier_parser.h"
#include "secureboot_gen.h"
#include "util.h"
#include "dx_pal_log.h"
/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************************ Private functions ******************************/

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


/************************ Public functions ******************************/

/** 
 * @brief This function is responsible for checking the SW minimum version Vs. the OTP/NVM.
 *        The function will read the VRL from the Flash verify its signature, 
 *        get the SW minimum version from it, compare it to the version in the OTP/NVM and if 
 *        its bigger than return the new value to the caller.
 *        The SW minimum version in the NVM/OTP is saved as 'unary counter' => the versions are represented by
 *        bits set to 1 in the NVM/OTP (and therefore limited to 32 versions).
 *         
 *
 * @param[in] flashRead_func - User's Flash read function used to read data from the flash to memory location
 * @param[in] userContext - User's context for the usage of flashRead_func
 * @param[in] hwBaseAddress - base address for the CC HW engines
 * @param[in] vrlInfo - structure containing vrl data (vrl address etc.)
 * @param[out] swVersionVal_ptr - value of the minimum version
 * @param[in] workspace_ptr - temporary buffer to load the VRL and SW components to
 * @param[in] workspaceSize - the temporary buffer size in bytes
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from SWRevocationManager_error.h
 */
DxError_t DX_SWR_SwMinVersionUpdate(DX_SB_FlashRead flashRead_func,
                                    void *userContext,
                                    DxUint32_t hwBaseAddress,
				    SW_Verification_Data_t vrlInfo,
                                    DxUint32_t *swVersionVal_ptr,
                                    DxUint32_t *workspace_ptr,
                                    DxUint32_t workspaceSize)
{
	/* error variable */
	DxError_t error = DX_OK;
	
	/* pointer to VRL */
	DxUint32_t *VRL_ptr = workspace_ptr;
	
	/* flag for secondary VRL exist */
	DxUint8_t  isSecExist = DX_FALSE;
	
	/* RSA data */
	VRL_Parser_RSAData_t rsaData;
	
	/* sw version struct */
	VRL_Parser_SW_Version_t swVersion;
	
	/* secondary hash pointer */
	DxUint32_t *hash_ptr;
	
	/* seconda VRL address */
	VRL_SecAddress_t secVrlAdrr;
	
	/* sw min version */  
	DxUint32_t savedSwMinVersion, hashSizeToCompare = 0;
	
	/* Public key HASH as saved in the NVM/OTP */
	HASH_Result_t savedPublicKeyHASH;
	
	DxUint32_t vrlSize = workspaceSize;
	
	DxUint32_t currSwVersion_bitcount = 0, minAllowedSwVersion_bitcount = 0;
	
	/*------------------  
		CODE
	-------------------*/
	/* check input parameters */
	if (flashRead_func == DX_NULL || workspace_ptr == DX_NULL || workspaceSize == 0)
		return DX_SW_REVOCATION_INV_INPUT_PARAM;
	
	/* aquire control on the CC */
	/*--------------------------*/
	GEN_AcquireCCLock(hwBaseAddress);
	
	/* Get the public key HASH from the OTP/NVM */
	/*------------------------------------------*/  
	error = NVM_ReadHASHPubKey(hwBaseAddress, vrlInfo.OTP_Key_Index, savedPublicKeyHASH);
	if (error != DX_OK)
		goto RELEASE_AND_RETURN;
	
	/* Get the sw minimum version from the NVM/OTP */
	/*---------------------------------------------*/
	error = NVM_GetSwVersion(hwBaseAddress, &savedSwMinVersion);
	if (error != DX_OK)
		goto RELEASE_AND_RETURN;
	
	/* Load the VRL from the Flash */
	/*-----------------------------*/
	error = DX_BIV_VRLParserLoadVRL(flashRead_func, userContext, /* Flash read function & parameter */
					vrlInfo.VRL_Address,         /* VRL address inside the array */
					vrlInfo.Magic_Number,	       /* magic number */
					VRL_ptr,                     /* start of the workspace buffer */
					&vrlSize );                  /* Size of the workspace buffer */
	
	/* In case of failure return error */
	if (error != DX_OK)
		goto RELEASE_AND_RETURN;
	
	
	/* Parse the VRL, get pointers to its different sections */
	/*-------------------------------------------------------*/
	error = DX_BIV_InternalVRLParse(VRL_ptr, &isSecExist, &rsaData, 
					&swVersion, &hash_ptr, &secVrlAdrr,
					&hashSizeToCompare);
	
	if (error != DX_OK)
		goto RELEASE_AND_RETURN;
	
	/* Compute HASH over the public key and exponent from the VRL and compare to given HASH */
	/*--------------------------------------------------------------------------------------*/	
	/* Verify public key HASH */      
	/* The RSA struct is ordered to first point to N and than to Np (to calculate the HASH need N & Np) */
	error = DX_BIV_CalcPublicKeyHASHAndCompare(hwBaseAddress, rsaData.N_ptr, savedPublicKeyHASH, hashSizeToCompare);
	if (error != DX_OK)
		goto RELEASE_AND_RETURN;   
	
	/* Verify RSA signature */
	/*----------------------*/  
	/* Verify the VRL signature */
	error = DX_BIV_VerifyVRLSignature(hwBaseAddress, VRL_ptr, (VRL_N_Params_t*)rsaData.N_ptr, 
					  (VRL_Sign_t*)rsaData.signature, rsaData.vrlSizeInWordsForHash, 
					  rsaData.rsaAlg);
	if (error != DX_OK)
		goto RELEASE_AND_RETURN;
	
	/* Verify that SW revocation is supported */
	if (swVersion.swVersionSupported == DX_FALSE){
		error = DX_SW_REVOCATION_NOT_SUPPORTED;
		goto RELEASE_AND_RETURN;
	}
	
	/* Verify that the current version in VRL is not smaller than the new minimum version 
	in the VRL */
	COUNT_ONE_BITS(swVersion.currSwVersion, &currSwVersion_bitcount);
	COUNT_ONE_BITS(swVersion.minAllowedSwVersion, &minAllowedSwVersion_bitcount);
	
	if (currSwVersion_bitcount < minAllowedSwVersion_bitcount){
		error = DX_SW_REVOCATION_CUR_SW_VERSION_ILLEGAL;
		goto RELEASE_AND_RETURN;
	}
	
	/* Verify the sw version */
	if (minAllowedSwVersion_bitcount <= savedSwMinVersion){
		*swVersionVal_ptr = 0;
	}
	else{
		*swVersionVal_ptr = swVersion.minAllowedSwVersion;
	}
	
	
RELEASE_AND_RETURN: 
	
	/* Release CCLock */
	GEN_ReleaseCCLock(hwBaseAddress);
	
	return error; 
}

