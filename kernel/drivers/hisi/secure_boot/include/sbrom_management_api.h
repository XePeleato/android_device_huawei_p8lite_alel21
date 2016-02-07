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



#ifndef _SBROM_MNG_API_H
#define _SBROM_MNG_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Life cycle state definitions */
#define DX_MNG_CHIP_MANUFACTURE_LCS		0x0
#define DX_MNG_DEVICE_MANUFACTURE_LCS		0x1
#define DX_MNG_SECURITY_DISABLED_LCS		0x3
#define DX_MNG_SECURE_LCS			0x5

/* session key definition */
#define DX_MNG_SESSION_KEY_MAX_SIZE		12
typedef DxUint8_t DX_MNG_SESSION_KEY_t[DX_MNG_SESSION_KEY_MAX_SIZE];

/* AES definitions */
#define DX_MNG_AES_BLOCK_SIZE_IN_BYTES	16
typedef DxUint8_t DX_MNG_AES_CMAC_RESULT_t[DX_MNG_AES_BLOCK_SIZE_IN_BYTES];

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*! 
 * @brief This function retrieves the security lifecycle from the HW register (when it is valid). 
 *        If the lifecycle is "secure" the function also needs to verify that the security disable flag
 *        (word 0x18 in OTP) is set to 4'b0011.
 *
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in/out] pLcs		- pointer to copy of current lifecycle state
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */
DxError_t DX_MNG_GetLcs(DxUint32_t hwBaseAddress,
			DxUint32_t *pLcs);


/*!
 * @brief This function retrieves the public key hash from the OTP according to the given index.
 * 	  Note: there are only 2 indexes supported, saved in truncated modes.
 *
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] index 		- index in OTP in which the required HASH resides 
 * @param[out] hashedPubKey	- the HASH of the public key
 * @param[in] hashResultSizeWord- the HASH buffer size in words
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */    
DxError_t DX_MNG_GetPubKeyHash(	DxUint32_t hwBaseAddress, 
				DxUint32_t index,
				DxUint32_t *hashedPubKey,
				DxInt32_t hashResultSizeWords);


/*!
 * @brief This function is used to generate and lock the provisioning key. 
 * 	  Atfer it computes AES-CMAC with KCP key, it load 0 to the AES engine key. 
 *
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[out] macResult 	- AES CMAC output buffer 
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */      
DxError_t DX_MNG_GetProvisionKey(DxUint32_t hwBaseAddress,
				 DX_MNG_AES_CMAC_RESULT_t macResult);


/*!
 * @brief This function get a random value (session key data), and computes AES-CMAC. 
 * 	  The output is written to the session key registers. 
 *
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] sessionKey 	- user sessionKey data (must be a random value)
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */ 
DxError_t DX_MNG_SetSessionKey(DxUint32_t hwBaseAddress,
			       DX_MNG_SESSION_KEY_t sessionKey);


/*!
 * @brief This function sets the "disable security" flag in the OTP manager,
 * 	  and writes a zero session key to lock the setting in place.
 *
 * @param[in] hwBaseAddress 	- cryptocell base address
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */   
DxError_t DX_MNG_DisableSecurity(DxUint32_t hwBaseAddress);


/*!
 * @brief This function gets the TRNG characterization information and mode from the OTP (word 0x18).
 *
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[out] trngParams 	- bits [31] - TRNG mode: 1- fast, 0- slow.
 * 				  bits [30:20] - TRNG characterization information.
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */ 
DxError_t DX_MNG_GetTrngInfo (DxUint32_t hwBaseAddress,
			      DxUint32_t *trngParams);

/*!
 * @brief This function retrieves the sw version from the OTP. 	  
 *
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[out] swVersion	- the sw version
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */    
DxError_t DX_MNG_GetSwVersion(DxUint32_t hwBaseAddress, DxUint32_t *swVersion);


/*!
 * @brief This function returns a word from the OTP according to a given address. 	  
 *
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] otpAddress	- Address in OTP [in Bytes]
 * @param[out] otpWord		- the returned OTP word
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */    
DxError_t DX_MNG_ReadOTPWord(DxUint32_t hwBaseAddress, DxUint32_t otpAddress, DxUint32_t *otpWord);



/*!
 * This function backup/restore on-chip secure RAM to/from external DRAM: 
 * It encrypts/decrypts the provided block (using the always-on state counter and destination address to construct the AES-CTR nonce);
 * Also, computes AES-CMAC signature, and appends/verifiys the signature.
 *  
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] srcAddr		- input Host memory address (aligned to 32bit).
 * @param[in] dstAddr		- output Host mamory address (aligned to 32bit).  
 * @param[in] blockSize 	- number of bytes to process (must be aligned to word boundaries).
 * @param[in] isSramBackup 	- if TRUE, SRAM backup; else, SRAM restore
 *  
 * 
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 * 
 */
DxUint32_t DX_MNG_ExtRamBackupAndRestore(DxUint32_t hwBaseAddress,	
					DxUint32_t srcAddr,
					DxUint32_t dstAddr,
					DxUint32_t blockSize,
					DxBool_t isSramBackup);


#define DX_MNG_ExtRamBackup(hwBaseAddress, srcAddr, dstAddr, blockSize)	\
	DX_MNG_ExtRamBackupAndRestore(hwBaseAddress, srcAddr, dstAddr, blockSize, DX_TRUE)


#define DX_MNG_ExtRamRestore(hwBaseAddress, srcAddr, dstAddr, blockSize)	\
	DX_MNG_ExtRamBackupAndRestore(hwBaseAddress, srcAddr, dstAddr, blockSize, DX_FALSE)



#ifdef __cplusplus
}
#endif

#endif



