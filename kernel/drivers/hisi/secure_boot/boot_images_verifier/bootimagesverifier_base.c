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
#include "bootimagesverifier_def.h"
#include "sb_hash.h"
#include "bootimagesverifier_parser.h"
#include "bootimagesverifier_internal.h"

#include "crypto_driver_defs.h" 
#include "bootimagesverifier_swvalidation_anddecryption.h"

#include "nvm.h"
#include "nvm_no_sep.h" 
#include "secureboot_gen.h"
#include "util.h"
#include "bootimagesverifier_internal.h"

#include "dx_pal_log.h"
/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/** 
 * @brief This function is responsible to verification of a given VRL. The verification is only done for the VRL
 * 	  and not for the SW images the VRL signs on.
 *        The verification process is as follows:
 *        1. The primary public key as saved in the VRL versus the HASHd public key that is saved in the NVM/OTP
 *        2. The RSA signature of the VRL
 *        3. The SW version that should be bigger than the SW minimum version as is saved in the NVM/OTP        
 *        NOTE - this function can only be called for primary VRLs. 
 *
 * @param[in] flashRead_func - User's Flash read function used to read data from the flash to memory location
 * @param[in] userContext - User's context for the usage of flashRead_func
 * @param[in] hwBaseAddress - base address for the CC HW engines
 * @param[in] vrlData - VRL data structure (containing the vrl address etc.)
 * @param[in] workspace_ptr - temporary buffer to load the VRL to.
 * @param[in] workspaceSize - the temporary buffer size in bytes
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_BaseVRLVerification(DX_SB_FlashRead flashRead_func,
				 void *userContext,
				 DxUint32_t hwBaseAddress,
				 SW_Verification_Data_t *vrlData,				 
				 DxUint32_t *workspace_ptr,
				 DxUint32_t workspaceSize)
{
	DxError_t error = DX_OK;

	DxUint32_t secHash[DX_SB_HASH_LENGTH_IN_WORDS];

	if (flashRead_func == DX_NULL)
		return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
	if (vrlData == DX_NULL)
		return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
	if (workspace_ptr == DX_NULL)
		return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;

	/* aquire control on the CC */
	/*--------------------------*/
	error = GEN_AcquireCCLock(hwBaseAddress);
	if (error != DX_OK)
		return error;
	/* calling the internal vrl verification function, vrl must be Primary vrl */
	error = DX_BIV_VRLVerification(NVM_ReadHASHPubKey, flashRead_func, userContext,
				       hwBaseAddress, DX_TRUE, vrlData,
				       DX_NOT_TO_SET_PKA_OFFSET,
				       secHash, workspace_ptr, workspaceSize);

	GEN_ReleaseCCLock(hwBaseAddress);  

	return error;
}

/** 
 * @brief This function is responsible to verification of the SW images signed in the VRL. 
 *        The API should be called for each SW component/image separately. 
 *
 * @param[in] flashRead_func - User's Flash read function used to read data from the flash to memory location
 * @param[in] userContext - User's context for the usage of flashRead_func
 * @param[in] hwBaseAddress - base address for the CC HW engines
 * @param[in] vrl_Ptr - the vrl buffer (the vrl loaded to memory) 
 * @param[in] swCompIndex - the sw image to verify index's
 * @param[in] workspace_ptr - temporary buffer to load the VRL to
 * @param[in] workspaceSize - the temporary buffer size in bytes
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_BaseSwCompDecryptAndVerify(DX_SB_FlashRead preHashflashRead_func,
					    void *userContext,
					    DxUint32_t hwBaseAddress,
					    DxUint32_t *vrl_Ptr,
					    DxUint32_t swCompIndex,
					    DxUint32_t *workspace_ptr,
					    DxUint32_t workspaceSize)
{
	DxError_t error = DX_OK;
	VRL_Parser_CompsData_t swCompsData;
	DxUint8_t isSecVrlExist;

	DxUint16_t hashOutputSize = 0;
	DxUint32_t loadAddress;

	/* AES key buffer */
	AES_Key_t AESKey;

	/* internal pointer for the VRL buffer */
	DxUint32_t *currRecord_ptr = DX_NULL;
	DxUint32_t *currRecordInfo_ptr = DX_NULL;

	if (preHashflashRead_func == DX_NULL)
		return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
	if (vrl_Ptr == DX_NULL)
		return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
	if (workspace_ptr == DX_NULL)
		return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;

	/* aquire control on the CC */
	/*--------------------------*/
	error = GEN_AcquireCCLock(hwBaseAddress);
	if (error != DX_OK)
		return error;
		
	/* Parse the VRL to get pointers to the SW image data (hash + load addresses and storage addresses + size),
	The WSP contains the vrl data at this stage */
	error = DX_BIV_ParseVrlGetSWComponentData(vrl_Ptr, &swCompsData, &isSecVrlExist);
	
	if (error != DX_OK)
		return error;
	/* Point to first SW component */
	currRecord_ptr = swCompsData.swHASHData_ptr;
	currRecordInfo_ptr = swCompsData.swAdditionalData_ptr;

	/* Get hash output size in bytes */  
	hashOutputSize = swCompsData.hashSizeWords* sizeof(DxUint32_t);

	/* If SW components is encrypted, get the aes key from OTP */
	if (swCompsData.isSwComponentEncrypted) {
		NVM_ReadAESKey(hwBaseAddress, AESKey);
	}

	/* Skip to main_record[swCompIndex] and point to HASH data (each record size is HASH size + load address size ) */
	currRecord_ptr += swCompIndex*(hashOutputSize/sizeof(DxUint32_t) + 1);

	/* Skip to additional_record[swCompIndex] and point to additional data (record info size is fixed - storage address + length) */
	currRecordInfo_ptr += swCompIndex*sizeof(VRL_ParamRecordInfo_t)/sizeof(DxUint32_t);

	/* calculate the HASH on the current block */             
	error = DX_BIV_CalcHASHOnSWRecDecryptAndCompare(preHashflashRead_func, userContext,/* Flash Read function */
						 DX_NULL, DX_NULL,	     	/* post hash operation not needed here */
						 hwBaseAddress,		    	/* CC base address */
						 hashOutputSize,	     	/* HASH output size */
						 currRecord_ptr,	    	/* pointer to Hash and load address */
						 (VRL_ParamRecordInfo_t*)currRecordInfo_ptr, /* comp additional info */   
						 &loadAddress, 			/* output load address */
						 &AESKey,			/* code encryption key for sw component */
						 swCompsData.isSwComponentEncrypted, /* Indicator if SW image is encrypted */
						 swCompsData.nonce, 		/* nonce */
						 workspace_ptr, workspaceSize);	/* workspace & workspaceSize to load the SW component into */ 

	if (error != DX_OK)
		return error;

	GEN_ReleaseCCLock(hwBaseAddress);  

	return error;
}

/** 
 * @brief This function is responsible to change a SW image storage address in the vrl (the storage adress part 
 *        is not signed and can be modified). The function changes the address according to the image index (the order
 * 	  in which the image was signed on starting from 0). 
 *
 * @param[in] vrl_Ptr - the vrl buffer (the vrl loaded to memory) 
 * @param[in] address - the new storage address to change to
 * @param[in] indexOfAddress - the index of the sw image (the index in which it was signed)
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_BaseVRLChangeSwCompStoreAddr(DxUint32_t *vrlPtr, DxUint32_t address, DxUint32_t indexOfAddress)
{
	return DX_BIV_VRLChangeSwCompStoreAddr(vrlPtr, address, indexOfAddress);
}

