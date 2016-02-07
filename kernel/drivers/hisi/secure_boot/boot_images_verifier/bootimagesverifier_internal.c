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
#include "rsa_pki_pka.h"
#include "sb_hash.h"
#include "bootimagesverifier_def.h"
#include "bootimagesverifier_parser.h"
#include "bootimagesverifier_internal.h"
#include "bootimagesverifier_swvalidation.h"
#include "nvm.h"
#include "secureboot_gen.h"
#include "util.h"
#include "bootimagesverifier_internal.h"

#include "dx_pal_log.h"
/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

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
				 DxUint32_t workspaceSize)
{
    /* error return */
    DxError_t error = DX_OK;
    
    /* pointers to inside the VRL */  
    DxUint32_t *VRL_ptr = workspace_ptr;
    
    /* public key hash saved in the NVM/OTP */
    HASH_Result_t savedPublicKeyHASH;
    
    /* temporary buffer */
    DxUint32_t VRLBufferSize = workspaceSize;
    
    /* SW minimum version */
    DxUint32_t savedSwMinVersion = 0;
    
    /* secondary VRL address */
    VRL_SecAddress_t secVRLAddress;
    
    /*------------------
        CODE
    -------------------*/

    #ifndef DX_SB_USE_RSA_SW
    /* Set the PKA offset if it is not set */
    if (pkaOffset != DX_NOT_TO_SET_PKA_OFFSET) {
	    DX_PAL_LOG_DEBUG("PKA offset is not set\n");
	    return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }
    #endif

    /* 1. Get the sw minimum version from the NVM/OTP (if supported) */
    /*---------------------------------------------------------------*/
    #ifndef DX_OTP_SW_VERSION_NOT_SUPPORTED
    error = NVM_GetSwVersion(hwBaseAddress, &savedSwMinVersion);
    if (error != DX_OK)
    	return error;
    #endif
    
    /* 2. Get the public key (N||Np) hash from the NVM/OTP or from the external HASH */
    /*-------------------------------------------------------------------------------*/
    if (isPrimaryVRL == DX_TRUE) {
    	error = DX_BIV_GetPrimaryKeyHash(SB_GetOTPHASH_func, hwBaseAddress, savedPublicKeyHASH, vrlData->OTP_Key_Index,
    					 vrlData->Ext_HASH_Value);    
    	if (error != DX_OK){
    		DX_PAL_LOG_DEBUG("DX_BIV_GetPrimaryKeyHash returned 0x%X\n", (unsigned int)error);
    		return error;
    	}
    }
    
    /* 3. Load the VRL from the Flash */
    /*--------------------------------*/
    error = DX_BIV_VRLParserLoadVRL(flashRead_func, userContext, /* Flash read function & parameter */
    				vrlData->VRL_Address,	 /* VRL address inside the array */
    				vrlData->Magic_Number,	/* expected magic number */
    				VRL_ptr,       /* start of the workspace buffer */
    				&VRLBufferSize /* Size of the workspace buffer */ );
    if (error != DX_OK){
    	DX_PAL_LOG_DEBUG("DX_BIV_VRLParserLoadVRL  returned 0x%X\n", (unsigned int)error);
    	return error;
    }
    
    /* 4. Parse the VRL, verify the RSA signature */
    /*--------------------------------------------*/
    error = DX_BIV_VRLParser(isPrimaryVRL,		/* indicating if this is Primary VRL */ 
    			 hwBaseAddress,		/* base address for CC HW */                         
    			 savedPublicKeyHASH,	/* PubHash*/
    			 savedSwMinVersion,	/* Sw Min Version */ 
    			 VRL_ptr,		/* a pointer to the VRL */ 
    			 &secVRLAddress,	/* Secondary VRL address, 0 means there is no second VRL */                         
    			 secondaryNHASH);	/* N buffer, secondary N will be copied to it if secondary VRL exist*/                      
    
    if (error != DX_OK){
    	DX_PAL_LOG_DEBUG("DX_BIV_VRLParser returned 0x%X\n", (unsigned int)error);
    }
    
    return error;
}/*End of DX_BIV_VRLVerification */


/** 
 *	@brief This function is responsible to change storage
 *	       addresses of the SW components in the VRL.
 *         
 *	@param[in] vrlPtr - a pointer to the VRL
 *	@param[in] address - the address to insert into the VRL
 *	@param[in] numOfAddressesInArray - number of addresses in the
 *	      previous array
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_VRLChangeSwCompStoreAddr(DxUint32_t *vrlPtr, DxUint32_t adresses, DxUint32_t indexOfAddress)
{
    /* temp vrl header */
    VRL_Header_t *vrlHeaderPtr;
    
    /* number of SW components, offset to comps data */
    DxUint32_t numOfComps = 0, offsetToSwCompsData = 0;
    
    /* temporary pointer to SW comps data */
    VRL_ParamRecordInfo_t *swCompDataPtr;
    
    /* temp pointer to VRL */
    DxUint32_t *tempVRLPtr = vrlPtr;
    
    /*------------------
        CODE
    -------------------*/
    
    /* Check inputs */
    if (vrlPtr == DX_NULL){
    	DX_PAL_LOG_DEBUG("VRL ptr is NULL\n");
    	return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }
    
    /* In case there is additional data - need to skip it */
    #ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED  
    /* set the temporary address to after the additional data header */
    tempVRLPtr = tempVRLPtr + ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t);
    #endif
    /* parse the VRL to point to the storage addresses */
    /*-------------------------------------------------*/
    vrlHeaderPtr = (VRL_Header_t*)tempVRLPtr;
    
    /* Calculate the VRL size according to offset to signature + sec address + num of comps * (comps params) */
    /*-------------------------------------------------------------------------------------------------------*/
    /* Get the number of sw comnponents from the header vrlSize field */
    numOfComps = (vrlHeaderPtr->vrlSize & VRL_LEN_NUM_OF_COMPS_BIT_MASK ) >> VRL_LEN_NUM_OF_COMPS_BIT_LOCATION;
    
    if (indexOfAddress > (numOfComps-1)) {
        DX_PAL_LOG_DEBUG("Invalid index\n");
    	return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM; 
    }
    offsetToSwCompsData = (DxUint32_t)(vrlHeaderPtr->vrlSize & VRL_LEN_SIGNATURE_OFFSET_BIT_MASK) /* signature offset */
    		      + sizeof(VRL_Sign_t)/sizeof(DxUint32_t) /* signature */
    		      + sizeof(VRL_SecAddress_t)/sizeof(DxUint32_t); /* secondary address */
    
    /* Point to the SW comps data */
    tempVRLPtr = tempVRLPtr + offsetToSwCompsData;
    
    /* Change the relevant address */
    swCompDataPtr = (VRL_ParamRecordInfo_t*)(tempVRLPtr + indexOfAddress*sizeof(VRL_ParamRecordInfo_t)/sizeof(DxUint32_t));
    swCompDataPtr->StoreAddr = adresses;
    
    return DX_OK;

}/* End of DX_BIV_VRLChangeSwCompStoreAddr */


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
DxError_t DX_BIV_GetSWImageInfo(DxUint32_t *vrlPtr, DxUint32_t swCompIndex, VRL_Internal_SW_Comp_Data_t *swCompInfo)
{
    /* error */
    DxError_t error = DX_OK;
    
    /* sw comp data */
    VRL_Parser_CompsData_t swCompsData;
    
    /* pointer to required additional data */
    VRL_ParamRecordInfo_t *recInfo_ptr;

    /* temp vrl header */
    VRL_Header_t *vrlHeaderPtr;
    
    /* number of SW components */
    DxUint32_t numOfComps = 0;
        
    /* temp pointer to VRL */
    DxUint32_t *tempVRLPtr = vrlPtr;
        
    /*------------------
        CODE
    -------------------*/
    /* Check inputs */
    if (vrlPtr == DX_NULL){
    	DX_PAL_LOG_DEBUG("VRL ptr is NULL\n");
    	return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }
    
    /* In case there is additional data - need to skip it */
    #ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED  
    /* set the temporary address to after the additional data header */
    tempVRLPtr = tempVRLPtr + ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t);
    #endif
    /* parse the VRL to point to the storage addresses */
    /*-------------------------------------------------*/
    vrlHeaderPtr = (VRL_Header_t*)tempVRLPtr;
    
    /* Calculate the VRL size according to offset to signature + sec address + num of comps * (comps params) */
    /*-------------------------------------------------------------------------------------------------------*/
    /* Get the number of sw comnponents from the header vrlSize field */
    numOfComps = (vrlHeaderPtr->vrlSize & VRL_LEN_NUM_OF_COMPS_BIT_MASK ) >> VRL_LEN_NUM_OF_COMPS_BIT_LOCATION;
    
    if (swCompIndex > (numOfComps-1)) {
        DX_PAL_LOG_DEBUG("Invalid index\n");
    	return DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM; 
    }

    /* parse the vrl get the sw comp data */
    error = DX_BIV_ParseVrlGetSWComponentData(vrlPtr, &swCompsData, DX_NULL);
    if (error != DX_OK) {
        return error;
    }
    
    /* according to the index get the load address */
    swCompInfo->loadAddress = *(swCompsData.swHASHData_ptr + swCompIndex*(swCompsData.hashSizeWords + 1) + swCompsData.hashSizeWords);
    
    /* Point to record additional info */
    recInfo_ptr = (VRL_ParamRecordInfo_t*)(swCompsData.swAdditionalData_ptr + swCompIndex*(sizeof(VRL_ParamRecordInfo_t)/sizeof(DxUint32_t)));
    
    /* return the size */
    swCompInfo->swCompSizeBytes = recInfo_ptr->Len * sizeof(DxUint32_t);
    
    return DX_OK;
}/* End of DX_BIV_SetSramPkaAddr */



