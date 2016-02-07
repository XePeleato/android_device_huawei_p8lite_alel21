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




#ifndef _BOOT_IMAGES_VERIFIER_SWVALIDATION_H
#define _BOOT_IMAGES_VERIFIER_SWVALIDATION_H


#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/** 
* @brief This function is responsible to verification of the SW components.
*        The function will go over the SW components load each component,
*        compute its HASH and compare it with the HASH saved in the VRL.
*         
*
* @param[in] flashRead_func - User's Flash read function used to read data from the flash to memory location
* @param[in] userContext - User's context for the usage of flashRead_func
* @param[in] hwBaseAddress - base address for the CC HW engines 
* @param[in] swCompsData_ptr - s/w comps data, pointers to VRL locations of the s/w comps HASH data and the s/w comps 
*            additional data (storage addresses & length)
* @param[out] isSecVrlExist - indicator if secondary VRL exists
* @param[in] workspace_ptr - temporary buffer to load the SW components to (SW components without 
*            loading address).
* @param[in] workspaceSize - the temporary buffer size in bytes
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_SWCompValidation(DX_SB_FlashRead flashRead_func,
				  void *userContext,
				  DxUint32_t hwBaseAddress,                                  
				  VRL_Parser_CompsData_t *swCompsData_ptr, 
				  DxUint8_t *isSecVrlExist,
				  DxUint32_t *workspace_ptr,
				  DxUint32_t workspaceSize);
/** 
* @brief This function load the SW component to RAM, calculates HASH on it and compares the
*        result with the given HASH (taken from the VRL).
*        This function calculates the HASH simultaneously to reading data from the Flash. 
*         
*
* @param[in] preHashflashRead_func - User's Flash read function used to read data from the flash to memory location. 
*	      this is the first function used (before the hash)
* @param[in] preHashUserContext - User's context for the usage of preHashflashRead_func
* @param[in] postHashflashRead_func - User's Flash read function used to copy the data to the final location 
*	      (used only in cases that the hash cannot be performed in th final location)
* @param[in] postHashUserContext - User's context for the usage of postHashflashRead_func
* @param[in] hwBaseAddress - base address for the CC HW engines
* @param[in] hashOutputSize - hash output size (defined according to given HASH algorithm) in bytes
* @param[in] recHashData_ptr - a pointer to record hash data and memory load address
* @param[in] recHashData_ptr - a pointer to record data (storage address and length)
* @param[out] swCompLoadAddress - the final address that the sw comp was loaded to
* @param[in] workspace_ptr - temporary buffer to load the SW components to (SW components without 
*            loading address).
* @param[in] workspaceSize - the temporary buffer size in bytes, minimal allowed size is 
*            DX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES
*
* @return DxError_t - On success the value DX_OK is returned, 
*         on failure - a value from BootImagesVerifier_error.h
*/
DxError_t DX_BIV_CalcHASHOnSWRecAndCompare(DX_SB_FlashRead preHashflashRead_func,
					   void *preHashUserContext,
					   DX_SB_FlashRead postHashflashRead_func,
					   void *postHashUserContext,
					   DxUint32_t hwBaseAddress,
					   DxUint32_t hashOutputSize,
					   DxUint32_t *recHashData_ptr,
					   VRL_ParamRecordInfo_t *recAddInfo_ptr,  
					   DxUint32_t *swCompLoadAddress,
					   DxUint32_t *workspace_ptr,
					   DxUint32_t workspaceSize);



#ifdef __cplusplus
}
#endif

#endif


