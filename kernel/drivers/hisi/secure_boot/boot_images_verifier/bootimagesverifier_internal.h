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



#ifndef _BOOT_IMAGES_VERIFIER_INTERNAL
#define _BOOT_IMAGES_VERIFIER_INTERNAL 


#ifdef __cplusplus
extern "C"
{
#endif

/* Definition for struct that contains the VRL data output (address and length) of a specific 
   SW component */
typedef struct {
	/* SW Comp load address */
	DxUint32_t loadAddress;

	/* SW Comp size in bytes */
	DxUint32_t swCompSizeBytes;

}VRL_Internal_SW_Comp_Data_t;

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/                      

/** 
* @brief This function is responsible to verification of a VRL.
*        The verification process verifies the following:
*        1. The primary public key as saved in the VRL versus the HASHd public key that is saved in the NVM/OTP
*        2. The RSA signature of the VRL
*         
* @param[in] SB_GetOTPHASH_func - Function pointer to user's	
* 	      get public key HASH function
* @param[in] flashRead_func - User's Flash read function used
*	     to read data from the flash to memory location
* @param[in] userContext - User's context for the usage of flashRead_func
* @param[in] hwBaseAddress - base address for the CC HW engines
* @param[in] isPrimaryVRL - an indicator if this is a primary
*	      VRL (TRUE for primary, FALSE for secondary)
* @param[in] vrlData - VRL data including storage address,
*	  magic number etc.
* @param[out] secondaryNHASH - will contain the secondary
*	      public key HASH
* @param[in] workspace_ptr - temporary buffer to load the VRL and SW components to
* @param[in] workspaceSize - the temporary buffer size in bytes
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_VRLVerification(Dx_SB_GetOTPHASH SB_GetOTPHASH_func, 
				 DX_SB_FlashRead flashRead_func,
				 void *userContext,
				 DxUint32_t hwBaseAddress,
				 DxUint8_t isPrimaryVRL,
				 SW_Verification_Data_t *vrlData,
				 DxUint32_t pkaOffset,
				 DxUint32_t *secondaryNHASH,
				 DxUint32_t *workspace_ptr,
				 DxUint32_t workspaceSize);

/** 
*	@brief This function is responsible to change storage
*	       addresses of the SW components in the VRL.
*         
*	@param[in] vrlPtr - a pointer to the VRL
*	@param[in] adressesArray - an array filled with addresses
*	      that are to replace the storage address of the SW
*	      components. NOTE the order of the addresses in the
*	      array MUST be exactly the same as the order of the SW
*	      components inside the VRL.
*	@param[in] numOfAddressesInArray - number of addresses in the
*	      previous array
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_VRLChangeSwCompStoreAddr(DxUint32_t *vrlPtr, DxUint32_t adresses, DxUint32_t indexOfAddress);


/** 
* @brief The function is responsible to SW images verification  
*        
*         
* @param[in] preHashflashRead_func - User's Flash read function used
*	     to read data from the flash to memory location, this callback is used on the data
*	     before the HASH operation
* @param[in] preHashUserContext - User's context for the usage of flashRead_func
* @param[in] postHashflashRead_func - User's Flash read function, this function copies the data to the required 
*	      output location. Should be used only in cases that the final location is not accessible for hash operation
* @param[in] postHashUserContext - User's context for the usage of flashRead_func
* @param[in] hwBaseAddress - base address for the CC HW engines
* @param[in] vrl_Ptr - pointer to the start of the VRL
* @param[in] swCompIndex - the index of the sw component to verify (starting from 0)
* @param[out] swCompInfo - will contain the address that the SW comp was loaded to and the size in bytes
* @param[in] workspace_ptr - temporary buffer to load the SW components to (if needed)
* @param[in] workspaceSize - the temporary buffer size in bytes
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_SwComponentVerification(DX_SB_FlashRead preHashflashRead_func,
					 void *preHashUserContext,
					 DX_SB_FlashRead postHashflashRead_func,
					 void *postHashUserContext,
					 DxUint32_t hwBaseAddress,
					 DxUint32_t *vrl_Ptr,
					 DxUint32_t swCompIndex,
					 VRL_Internal_SW_Comp_Data_t *swCompInfo,
					 DxUint32_t *workspace_ptr,
					 DxUint32_t workspaceSize);

/** 
 * @brief This function gets the hwBaseAddress and returns the LCS value.
 *         
 * @param[in] hwBaseAddress - base address for the CC HW engines
 * @param[in/out] lcs_ptr  - pointer to memory, to store the LCS
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_GetLCSValue(DxUint32_t hwBaseAddress, DxUint32_t *lcs_ptr);

/** 
 * @brief This function is responsible to return the load 
 *        address and the sw image size according to given index
 *         
 * @param[in] vrlPtr - pointer to the vrl 
 * @param[in] swCompIndex - index of the imgae 
 * @param[out] swCompInfo - load address and size
 *  
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_GetSWImageInfo(DxUint32_t *vrlPtr, DxUint32_t swCompIndex, VRL_Internal_SW_Comp_Data_t *swCompInfo);


#ifdef __cplusplus
}
#endif

#endif


