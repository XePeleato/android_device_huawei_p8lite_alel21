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



#ifndef _BOOT_IMAGES_VERIFIER_API_H
#define _BOOT_IMAGES_VERIFIER_API_H


#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

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
				     DxUint32_t workspaceSize);

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
			       DxUint32_t hashBufSize);                                       

#ifdef __cplusplus
}
#endif

#endif


