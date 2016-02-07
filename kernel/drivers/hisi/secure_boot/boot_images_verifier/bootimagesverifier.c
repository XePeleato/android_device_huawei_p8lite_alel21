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
#include "bootimagesverifier_error.h"
#include "rsa.h"
#include "sb_hash.h"
#include "bootimagesverifier_def.h"
#include "bootimagesverifier_parser.h"
#include "bootimagesverifier_internal.h"
#include "bootimagesverifier_api.h"
#include "bootimagesverifier_swvalidation.h"
#include "nvm.h"
#include "secureboot_gen.h"
#include "util.h"

#define _MAIN_C_
#include "dx_pal_log.h"



/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/** 
 * @brief This function is responsible to verification of the VRL list.
 *        The function will go over the VRL list starting from the first and verify it, in case 
 *        the verification fail, the function will go to the next VRL and verify it, till a suuccessfull 
 *        verification or the end of the list is reached.
 *        The verification process verifies the following:
 *        1. The primary public key as saved in the VRL versus the HASHd public key that is saved in the NVM/OTP
 *        2. The RSA signature of the VRL
 *        3. The SW version that should be bigger than the SW minimum version as is saved in the NVM/OTP
 *        4. Each of the SW components HASH as appear in the VRL 
 *         
 *
 * @param[in] flashRead_func - User's Flash read function used to read data from the flash to memory location
 * @param[in] userContext - User's context for the usage of flashRead_func
 * @param[in] hwBaseAddress - base address for the CC HW engines
 * @param[in] vrlList - VRL list ordered, the first item will be the first to be verified  
 * @param[in] numOfVrlInList - number of VRLs in the given list
 * @param[out] index_ptr - the index in the VRL list of the VRL that passed verification
 * @param[in] workspace_ptr - temporary buffer to load the VRL and SW components to
 * @param[in] workspaceSize - the temporary buffer size in bytes
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_SwImageVerification(DX_SB_FlashRead flashRead_func,
				     void *userContext,
				     DxUint32_t hwBaseAddress,
				     DX_BIM_VRL_TABLE_t vrlList,
				     DxUint32_t numOfVrlInList,
				     DxUint32_t *index_ptr,
				     DxUint32_t *workspace_ptr,
				     DxUint32_t workspaceSize)
{
/* error variable */
DxError_t error = DX_OK;

/* internal index */
DxUint32_t i = 0;

/* secondary N hash that is saved in the Primary VRL to be verified with the secondary 
   N hash in secondary VRL */
DxUint32_t secondaryNHash[DX_SB_HASH_LENGTH_IN_WORDS];

/* s/w component data */
VRL_Parser_CompsData_t swCompsData;

/* indicator if secondary vrl exists */
DxUint8_t isSecExist = 0;

/* VRL info for secondary usage */
SW_Verification_Data_t secVrlData;

/*------------------
    CODE
-------------------*/

/* Verify input parameters */
if (flashRead_func == DX_NULL){
	DX_PAL_LOG_DEBUG("flashRead_func is NULL\n");
	return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
}

/* Verify vrlLis */
if (vrlList == DX_NULL || numOfVrlInList == 0){
	DX_PAL_LOG_DEBUG("vrlList or numOfVrlInList are 0\n");
	return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
}

if (index_ptr == DX_NULL || workspace_ptr == DX_NULL || workspaceSize == 0){
	DX_PAL_LOG_DEBUG("index_ptr || workspace_ptr || workspaceSize are zero\n");
	return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
}

/* aquire control on the CC */
/*--------------------------*/
error = GEN_AcquireCCLock(hwBaseAddress);
if (error != DX_OK)
	return error;


/* Go over the VRL list in a loop till a VRL passes verification */
/*---------------------------------------------------------------*/
for (i = 0; i < numOfVrlInList; i++) {	
	/* 1. Verify the VRL RSA signature and public key */
	/*------------------------------------------------*/    
	error = DX_BIV_VRLVerification(NVM_ReadHASHPubKey, /* Get OTP */
				       flashRead_func,
				       userContext,
				       hwBaseAddress,
				       DX_TRUE,
				       &vrlList[i],
				       DX_NOT_TO_SET_PKA_OFFSET,
				       secondaryNHash,
				       workspace_ptr,
				       workspaceSize);                             

	/* In case of error continue to the next VRL in list */
	if (error != DX_OK) {
		DX_PAL_LOG_DEBUG("DX_BIV_VRLVerification returned 0x%X\n", (unsigned int)error);
		continue;
	}

	/* 2. Validate the SW components */
	/*-------------------------------*/
	/* For each SW component in a loop load (if required) the SW component and verify its HASH */
	error = DX_BIV_SWCompValidation(flashRead_func, userContext,
					hwBaseAddress,                                  
					&swCompsData,  
					&isSecExist,
					workspace_ptr,
					workspaceSize);

	if (error != DX_OK) {
		DX_PAL_LOG_DEBUG("DX_BIV_SWCompValidation returned 0x%X\n", (unsigned int)error);
		continue;
	}

	/* If operation succeeded => quit from the loop and return the index of the VRL that 
	passed the verification */
	if (error == DX_OK && isSecExist == DX_FALSE) {
		/* the validated index */
		*index_ptr = i;      
		goto RELEASE_AND_RETURN;
	}

	/* 5. In case there is a secondary VRL, need to verify it */
	/*--------------------------------------------------------*/
	if (error == DX_OK && isSecExist == DX_TRUE) {
		/* Get secondary data (if exist) */

		DX_BIV_ParseVrlGetSecData(workspace_ptr, secondaryNHash, swCompsData.hashSizeWords, 
					  &secVrlData.VRL_Address);
		secVrlData.Magic_Number = vrlList[i].Magic_Number;

		/* 1. Load the VRL from the Flash */
		/*--------------------------------*/
		error = DX_BIV_VRLVerification(NVM_ReadHASHPubKey, /* Get OTP */
					       flashRead_func,
					       userContext,
					       hwBaseAddress,
					       DX_FALSE,
					       &secVrlData,
					       DX_NOT_TO_SET_PKA_OFFSET,
					       secondaryNHash,
					       workspace_ptr,
					       workspaceSize);                       
		/* In case of failure go to the next VRL in the table */
		if (error != DX_OK){
			DX_PAL_LOG_DEBUG("DX_BIV_VRLVerification returned 0x%X\n", (unsigned int)error);
			continue;
		}

		/* 2. Validate the SW components */
		/*-------------------------------*/
		/* For each SW component in a loop load (if required) the SW component and verify its HASH */
		error = DX_BIV_SWCompValidation(flashRead_func, userContext,
						hwBaseAddress,                                  
						&swCompsData,  
						&isSecExist,
						workspace_ptr,
						workspaceSize);

		if (error != DX_OK) {
			DX_PAL_LOG_DEBUG("DX_BIV_SWCompValidation returned 0x%X\n", (unsigned int)error);
			continue;
		} else {
			/* If verification succeeded of the secondary VRL return the index of the Primary VRL that 
			   was validated */
			/* the validated index */
			*index_ptr = i;      
			goto RELEASE_AND_RETURN;
		}       
	}/* End of Secondary VRL verification */
}/* End of VRL table loop */

/* If the loop ended without a validated VRL return error */
error = DX_BOOT_IMG_VERIFIER_NO_VRL_PASSED_VALIDATION;
DX_PAL_LOG_DEBUG("no VRL passes validation\n");
RELEASE_AND_RETURN:

GEN_ReleaseCCLock(hwBaseAddress);  
return error;

} /* End of DX_BIV_SwImageVerification */

/** 
 * @brief This function gets the index of the OTP in which the HASH of the public key resides.
 *        
 *         
 * @param[in] hwBaseAddress - base address for the CC HW engines
 * @param[in] indexInOTP - index in OTP in which the required HASH resides 
 * @param[out] hashOutput - the HASH of the public key
 * @param[in] hashBufSize - the HASH buffer size in bytes
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_GetPubKeyHASH(DxUint32_t hwBaseAddress,
			       DxUint32_t indexInOTP,
			       DxUint32_t *hashOutput,
			       DxUint32_t hashBufSize)
{
DxError_t error = DX_OK;

#ifdef BIG__ENDIAN
DxUint32_t i;
#endif    

/*------------------
    CODE
-------------------*/

/* Verify that the output buffer is in sufficient size */
if (hashBufSize < DX_SB_HASH_LENGTH_IN_WORDS* sizeof(DxUint32_t)){
	DX_PAL_LOG_DEBUG("HASH buf too small\n");
	return DX_BOOT_IMG_VERIFIER_HASH_BUF_TOO_SMALL;
}

/* Aquire control on the CC */
/*--------------------------*/
error = GEN_AcquireCCLock(hwBaseAddress);
if (error != DX_OK)
	return error;

/* Get the HASH from the SRAM */
/*----------------------------*/
error = NVM_ReadHASHPubKey(hwBaseAddress, indexInOTP, hashOutput);

#ifdef BIG__ENDIAN
for (i = 0; i < DX_SB_HASH_LENGTH_IN_WORDS; i++) {
	/* If in BIG__ENDIAN platforms revert the key (back to its original representation) */
	UTIL_ReverseBuff( (DxUint8_t*)&hashOutput[i] , sizeof(DxUint32_t) );
}
#endif
/* Release control on the CC */
/*---------------------------*/
GEN_ReleaseCCLock(hwBaseAddress);

return error;
}


/** 
 * @brief This function gets the hwBaseAddress and returns the LCS value.
 *         
 * @param[in] hwBaseAddress - base address for the CC HW engines
 * @param[in/out] lcs_ptr  - pointer to memory, to store the LCS
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_GetLCSValue(DxUint32_t hwBaseAddress, DxUint32_t *lcs_ptr)
{

DxError_t result = DX_OK;

result = GEN_AcquireCCLock(hwBaseAddress);
if (result != DX_OK)
	return result;

result = NVM_GetLCS(hwBaseAddress, lcs_ptr);

GEN_ReleaseCCLock(hwBaseAddress);  

return result;

}

