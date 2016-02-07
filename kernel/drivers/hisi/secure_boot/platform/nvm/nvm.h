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


 
#ifndef _NVM__H
#define _NVM__H

#ifdef __cplusplus
extern "C"
{
#endif


/*------------------------------------
    DEFINES
-------------------------------------*/

/** 
 * @brief This function reads the LCS from the SRAM/NVM
 *
 * @param[in] hwBaseAddress  -  cryptocell base address
 *
 * @param[in/out] lcs_ptr  - pointer to memory to store the LCS
 *
 * @return DxError_t - On success the value DX_OK is returned, and on failure   -a value from NVM_error.h
 */
DxError_t NVM_GetLCS(DxUint32_t hwBaseAddress, DxUint32_t *lcs_ptr);

/** 
 * @brief This function reads the LCS
 *
 * @param[in/out] lcs_ptr  -pointer to memory to store the LCS
 *
 * @return DxError_t - On success the value DX_OK is returned, and on failure   -a value from NVM_error.h
 */
DxError_t NVM_ReadRKEK(DxUint8_t* rkek_ptr);

/**
 * @brief The NVM_ReadHASHPubKey function is a NVM interface function - 
 *        The function retrieves the HASH of the device Public key (N & E) from the SRAM/NVM
 *        In case the flag DX_NO_PUB_KEY_VERIFICATION is set the function will return a 
 *        constant pre-defined value.
 *
 * @param[in] hwBaseAddress -  cryptocell base address
 *
 * @param[in] pubKeyIndex -  Index of HASH in the OTP
 *         
 * @param[out] PubKeyHASH   -  the public key HASH.
 *
 * @return DxError_t - On success the value DX_OK is returned, and on failure   -a value from NVM_error.h
 */
 
DxError_t NVM_ReadHASHPubKey(DxUint32_t hwBaseAddress, DxUint32_t pubKeyIndex, HASH_Result_t PubKeyHASH);

/**
 * @brief The NVM_GetSwVersion function is a NVM interface function - 
 *        The function retrieves the SW version from the SRAM/NVM
 *         
 * @param[in] hwBaseAddress -  cryptocell base address
 *
 * @param[out] swVersion   -  the minimum SW version
 *
 * @return DxError_t - On success the value DX_OK is returned, and on failure   -a value from NVM_error.h
 */
DxError_t NVM_GetSwVersion(DxUint32_t hwBaseAddress, DxUint32_t* swVersion);

/**
 * @brief The NVM_SetSwVersion function is a NVM interface function - 
 *        Sep implementation:
 *        The function changes the SW version, the function will write to the shared 
 *        area and the CC_Init will save the new version into the NVM/OTP.
 *        Flash implementation:
 *        The function will save the SW version directly to the NVM/OTP.
 *        The Sw vesion is saved as the number of 1's and therefor it is limited to value of 32.
 *
 * @param[in] hwBaseAddress -  cryptocell base address
 *
 * @param[in] swVersion -  the sw minimum version to update to the NVM (will be written to the NVM as
 *                         the number of 1's)
 *
 * @param[in] hostSharedAreaAddress - pointer to start of host's shared area
 *
 * @param[in] hostSharedAreaSize - host's shared area size
 *
 * @return DxError_t - On success the value DX_OK is returned, and on failure   -
 *                     a value from NVM_error.h
 */
DxError_t NVM_SetSwVersion(DxUint32_t hwBaseAddress,
                           DxUint32_t swVersion,
                           void *hostSharedAreaAddress,
                           DxUint32_t hostSharedAreaSize);


/**
 * @brief The GEN_SelectPubKey function selects the public key according to the ROM selector value. 
 *
 *        N_ptr is changed to the selected key in the ROM array. 
 *         
 *
 */                                           
void NVM_SelectPubKey( DxUint32_t hwBaseAddress, DxUint32_t **N_ptr );
          
#ifdef __cplusplus
}
#endif

#endif


