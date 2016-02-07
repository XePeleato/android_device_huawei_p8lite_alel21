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



#ifndef _BOOT_IMAGES_VERIFIER_BASE_API_H
#define _BOOT_IMAGES_VERIFIER_BASE_API_H


#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

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
				 DxUint32_t workspaceSize);

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
					    DxUint32_t workspaceSize);

/** 
 * @brief This function is responsible to verification of the SW images signed in the VRL. 
 *        The API should be called for each SW component/image separately. 
 *
 * @param[in] vrl_Ptr - the vrl buffer (the vrl loaded to memory) 
 * @param[in] address - the new storage address to change to
 * @param[in] indexOfAddress - the index of the sw image (the index in which it was signed)
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_BaseVRLChangeSwCompStoreAddr(DxUint32_t *vrlPtr, DxUint32_t address, DxUint32_t indexOfAddress);



#ifdef __cplusplus
}
#endif

#endif


