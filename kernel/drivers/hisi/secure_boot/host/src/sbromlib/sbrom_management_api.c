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

 
/************* Include Files ****************/
 
#include "secureboot_basetypes.h"
#include "secureboot_general_hwdefs.h"
#include "sb_hash.h"  
#include "systemdep.h"
#include "util.h"

#include "crypto_driver_defs.h"

#include "sbrom_management_defs.h"
#include "sbrom_management_api.h"
#include "sbrom_management_hw_defs.h"
#include "sbrom_management_error.h"
#include "sbrom_crypto_driver.h"

/************************ Defines ******************************/
void UTIL_MemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );
/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Private functions ******************************/
static DxUint32_t UTIL_REVERT_UINT32_BYTES (DxUint32_t val)
{
#ifdef BIG__ENDIAN
  return  ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) );
#else
  return  (val);
#endif
}

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
DxError_t DX_MNG_GetLcs(DxUint32_t hwBaseAddress, DxUint32_t *pLcs)
{
	DxError_t  error = DX_OK;
	DxUint32_t manufacturerFlag = 0;
	DxUint32_t lcsMask, lcsVal = 0;


	/* Check input variables */
	if (pLcs == DX_NULL) {
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;
	}
  
	/* Wait on LCS valid before reading the LCS register */
	DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress);  
     	
	/* Read the LCS register */ 	
	SYS_ReadRegister(SB_REG_ADDR(hwBaseAddress, LCS_REG), lcsVal);

	/* Parse the LCS value */ 
	lcsMask = (1 << DX_LCS_REG_LCS_REG_BIT_SIZE) -1;
	lcsVal &= lcsMask;

	/* Return the LCS value */ 
	*pLcs = lcsVal;

	/* check KDR and SCP error flags, for all lifecycles except CM */
	if(lcsVal != DX_MNG_CHIP_MANUFACTURE_LCS) {

		/* Check RKEK error bit */
		DX_MNG_IS_OTP_KDR_ERROR(hwBaseAddress, error);
		if (error) {
			return DX_MNG_ILLEGAL_RKEK_VALUE_ERR;
		}

		/* Check Provision (Scp) error bit */
		DX_MNG_IS_OTP_PROV_ERROR(hwBaseAddress, error);
		if (error) {
			return DX_MNG_ILLEGAL_SCP_VALUE_ERR;
		}
	}

	/* If DM or Secure mode, test security disable flag */
	if ((lcsVal == DX_MNG_DEVICE_MANUFACTURE_LCS) || (lcsVal == DX_MNG_SECURE_LCS)) {

		DX_MNG_READ_WORD_FROM_AIB(hwBaseAddress, 
			DX_MNG_NVM_READ_ADDR | DX_MNG_OTP_MANUFACTRER_FLAG_OFFSET*sizeof(DxUint32_t), manufacturerFlag);

		/* Verify security disable flag is off */
		if ( (manufacturerFlag >> DX_MNG_SECURITY_DISABLE_FLAG_SHIFT & DX_MNG_SECURITY_DISABLE_FLAG_MASK) != DX_MNG_SECURITY_DISABLE_FLAG_OFF ) {
			return DX_MNG_ILLEGAL_LCS_FOR_OPERATION_ERR;
		}
	}
                                                      

	return DX_OK;
}



/*!
 * @brief This function retrieves the public key hash from the OTP according to the given index.
 *        Note:
 *        DX_CONFIG_SB_MAX_NUM_OF_OTP_KEYS (set pre-compilation) defines the number of OTP keys.
 *        Currently, we support 2 options: either one 256bit key or two 128bit truncated keys.
 *
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] index 		- index in OTP in which the required HASH resides 
 * @param[out] hashedPubKey	- the HASH of the public key
 * @param[in] hashResultSizeWord- the HASH buffer size in words
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */    
DxError_t DX_MNG_GetPubKeyHash(DxUint32_t hwBaseAddress, DxUint32_t index, DxUint32_t *hashedPubKey, DxInt32_t hashResultSizeWords)
{
	int i, address, numOfZeroes = 0;
	DxUint32_t regVal = 0, cntZero = 0;
	DxUint32_t oemFlag = 0;

	/* Check input variables */
	if (hashedPubKey == DX_NULL) {
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;
	}
	if (hashResultSizeWords < DX_MNG_HASH_SIZE_IN_WORDS) {
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;
	}
	if (index > DX_SB_MAX_NUM_OF_OTP_KEYS - 1) {
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;
	}

	/* Verify that the HW finished initialization */  
	DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress);
  
	/* Set the operation to read */
	address = DX_MNG_NVM_READ_ADDR | 
		  (index == DX_MNG_TRUNCATED_HASH_INDEX_0 ? DX_MNG_OTP_HASH_INDEX_0_OFFSET : DX_MNG_OTP_HASH_INDEX_1_OFFSET)*sizeof(DxUint32_t);

	/* Read the HASH from the OTP */  
	for (i = 0; i < DX_MNG_HASH_SIZE_IN_WORDS; i++) {
  
		DX_MNG_READ_WORD_FROM_AIB(hwBaseAddress, address + i*sizeof(DxUint32_t), regVal);

		*(hashedPubKey + i) = regVal;

		/* Accumulate number of zeroes */
		DX_MNG_COUNT_ZEROES(regVal, cntZero);
                              
	}/* End of for loop */  
	

	/* Read OEM programmer flags (word 0x19 in OTP) and get number of zeroes set in the OTP */  
	DX_MNG_READ_WORD_FROM_AIB(hwBaseAddress, 
		DX_MNG_NVM_READ_ADDR | DX_MNG_OTP_OEM_FLAG_OFFSET*sizeof(DxUint32_t), oemFlag); 

	if (index == DX_MNG_TRUNCATED_HASH_INDEX_0) {
		numOfZeroes = (oemFlag >> DX_MNG_NUM_OF_ZEROES_IN_HASH0_SHIFT) & DX_MNG_NUM_OF_ZEROES_IN_HASH_MASK; 
	}
	else {
		numOfZeroes = (oemFlag >> DX_MNG_NUM_OF_ZEROES_IN_HASH1_SHIFT) & DX_MNG_NUM_OF_ZEROES_IN_HASH_MASK; 
	}


	/* Verify the number of "0" bits in the hash key */
	if(numOfZeroes != cntZero) {
		return DX_MNG_ILLEGAL_OEM_HASH_VALUE_ERR;
	}
  
	return DX_OK;
  
}



/*!
 * @brief This function is used to generate and lock the provisioning key. 
 * 	  Atfer it computes AES-CMAC with KCP key, it load 0 to the AES engine key. 
 *
 * @param[out] macResult 	- AES CMAC output buffer 
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */      
DxError_t DX_MNG_GetProvisionKey(DxUint32_t hwBaseAddress, DX_MNG_AES_CMAC_RESULT_t macResult)
{
	DxError_t  error = DX_OK;
	DxUint32_t dataIn[DX_MNG_MAX_HASH_SIZE_IN_WORDS];

	/* Check input variables */
	if (macResult == DX_NULL)
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;

	/* Verify that the HW finished initialization */  
	DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress);

	/* Check RKEK error bit */
	DX_MNG_IS_OTP_KDR_ERROR(hwBaseAddress, error);
	if (error) {
		return DX_MNG_ILLEGAL_RKEK_VALUE_ERR;
	}

	/* Check Provision (Scp) error bit */
	DX_MNG_IS_OTP_PROV_ERROR(hwBaseAddress, error);
	if (error) {
		return DX_MNG_ILLEGAL_SCP_VALUE_ERR;
	}

	/* Check session key validity is low (to assure aes will load Kcp correctly ) */
	DX_MNG_IS_SESSION_KEY_VALID(hwBaseAddress, error);
	if (error == DX_MNG_SESSION_KEY_IS_SET) {
		return DX_MNG_ILLEGAL_SESSION_KEY_ERR;
	}

	/* Read H(BK0) */
	error = DX_MNG_GetPubKeyHash(hwBaseAddress, DX_MNG_TRUNCATED_HASH_INDEX_0, dataIn, DX_MNG_HASH_SIZE_IN_WORDS); 
	if (error != DX_OK) {
		return error;
	}
	/* Read H(BK1) if exist, and append it to H(BK0) */
	if (DX_SB_MAX_NUM_OF_OTP_KEYS > DX_MNG_TRUNCATED_HASH_INDEX_1) {
		error = DX_MNG_GetPubKeyHash(hwBaseAddress, DX_MNG_TRUNCATED_HASH_INDEX_1, &dataIn[DX_MNG_HASH_SIZE_IN_WORDS], DX_MNG_HASH_SIZE_IN_WORDS); 
		if (error != DX_OK) {
			return error;
		}
	}

	/* Compute AES_CMAC: 
		data = public key hash; 
		key = secret key (provision) */  
	SBROM_AesCmac(hwBaseAddress, PROVISIONING_KEY, DX_NULL, dataIn, DMA_SRAM, DX_MNG_MAX_HASH_SIZE_IN_WORDS*sizeof(DxUint32_t), (DxUint32_t *)macResult);

	/* Load 0 to the AES engine key */
	SBROM_AesLockEngineKey(hwBaseAddress);
	
	return DX_OK;
}



/*!
 * @brief This function get a random value (session key data), and computes AES-CMAC. 
 *        The output is written to the session key registers.
 *        Session key can be generated multiple times at runtine (in a boot cycle)
 *
 * @param[in] sessionKey 	- user sessionKey data (must be a random value)
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */ 
DxError_t DX_MNG_SetSessionKey(DxUint32_t hwBaseAddress, DX_MNG_SESSION_KEY_t sessionKey)
{   
	DxError_t  error = DX_OK;
	DxUint8_t dataInBytes[AES_BLOCK_SIZE_IN_BYTES]; 
	DxUint32_t dataInWords[AES_BLOCK_SIZE_IN_WORDS], dataOut[AES_BLOCK_SIZE_IN_WORDS]; 
	int i;	

	/* Check input variables */
	if (sessionKey == DX_NULL)
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;

	/* Verify that the HW finished initialization */  
	DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress);

	/* Check RKEK error bit */
	DX_MNG_IS_OTP_KDR_ERROR(hwBaseAddress, error);
	if (error) {
		return DX_MNG_ILLEGAL_RKEK_VALUE_ERR;
	}

	/* Check Provision (Scp) error bit */
	DX_MNG_IS_OTP_PROV_ERROR(hwBaseAddress, error);
	if (error) {
		return DX_MNG_ILLEGAL_SCP_VALUE_ERR;
	}

	/* Note: no need to check session key validity */

	/* Generate data input for CMAC operation */
	dataInBytes[0x0] = DX_MNG_SESSION_KEY_PREFIX_DATA1;
	dataInBytes[0x1] = DX_MNG_SESSION_KEY_PREFIX_DATA2;
	dataInBytes[0x2] = DX_MNG_SESSION_KEY_PREFIX_DATA3;
	for(i=0; i<DX_MNG_SESSION_KEY_MAX_SIZE; i++)
		dataInBytes[i+3] = sessionKey[i];
	dataInBytes[0xf] = DX_MNG_SESSION_KEY_SUFFIX_DATA;

	/* Keep input data aligned to 32bit */
	UTIL_MemCopy((DxUint8_t *)dataInWords, dataInBytes, AES_BLOCK_SIZE_IN_BYTES);

	/* Computes AES_CMAC with KDR and session key (as data input) */  
	SBROM_AesCmac(hwBaseAddress, ROOT_KEY, DX_NULL, dataInWords, DMA_SRAM, AES_BLOCK_SIZE_IN_BYTES, dataOut);

	/* Write the CMAC result to the session key registers */
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_SESSION_KEY0), UTIL_REVERT_UINT32_BYTES(dataOut[0]) );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_SESSION_KEY1), UTIL_REVERT_UINT32_BYTES(dataOut[1]) );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_SESSION_KEY2), UTIL_REVERT_UINT32_BYTES(dataOut[2]) );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_SESSION_KEY3), UTIL_REVERT_UINT32_BYTES(dataOut[3]) );  

	return DX_OK;
}


/*!
 * @brief This function sets the "disable security" flag in the OTP manager,
 * 	  and writes a zero session key to lock the setting in place.
 *
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */   
DxError_t DX_MNG_DisableSecurity(DxUint32_t hwBaseAddress)
{
      
	DxUint32_t regVal = 0;	
	
	/* Verify that the HW finished initialization */  
	DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress);

	/* Write the zeroes to the session key registers */
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_SESSION_KEY0), regVal ); 
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_SESSION_KEY1), regVal );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_SESSION_KEY2), regVal );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_SESSION_KEY3), regVal );  

	/* Sets the security disabled register */  
	SYS_WriteRegister(SB_REG_ADDR(hwBaseAddress, HOST_CPC_SECURITY_DISABLE), DX_TRUE);

	return DX_OK;
}



/*!
 * @brief This function gets the TRNG characterization information and mode from the OTP (word 0x0A).
 *
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[out] trngParams 	- bits [31] - TRNG mode: 1- fast, 0- slow.
 * 				  bits [30:20] - TRNG characterization information.
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */ 
DxError_t DX_MNG_GetTrngInfo (DxUint32_t hwBaseAddress, DxUint32_t *trngParams)
{
      
	DxUint32_t regVal = 0;

	/* check input variables */
	if (trngParams == DX_NULL)
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;

	/* Verify that the HW finished initialization */  
	DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress);

	/* Read TRNG data */
	DX_MNG_READ_WORD_FROM_AIB(hwBaseAddress,
		 DX_MNG_OTP_TRNG_OFFSET*sizeof(DxUint32_t) | DX_MNG_NVM_READ_ADDR, regVal);

	/* Return the TRNG parameter */ 
	*trngParams = regVal & DX_MNG_OTP_TRNG_MASK;

	return DX_OK;
}

/*!
 * @brief This function retrieves the sw version from the OTP. 	  
 *
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[out] swVersion	- the sw version
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */    
DxError_t DX_MNG_GetSwVersion(DxUint32_t hwBaseAddress, DxUint32_t *swVersion)
{
	DxUint32_t regVal = 0;

	/* check input variables */
	if (swVersion == DX_NULL) {
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;
	}

	/* Verify that the HW finished initialization */  
	DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress);
  
	/* Read the SW version from the OTP */  
	DX_MNG_READ_WORD_FROM_AIB(hwBaseAddress,
		 DX_MNG_NVM_READ_ADDR | DX_MNG_OTP_SW_VERSION_OFFSET*sizeof(DxUint32_t), regVal);

	*swVersion = regVal;
  
	return DX_OK;
  
}

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
DxError_t DX_MNG_ReadOTPWord(DxUint32_t hwBaseAddress, DxUint32_t otpAddress, DxUint32_t *otpWord)
{
	DxUint32_t regVal = 0;

	/* check input variables */
	if (otpWord == DX_NULL) {
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;
	}
	 
	if ( (otpAddress < DX_MNG_OTP_START_OFFSET*sizeof(DxUint32_t)) || 
	     (otpAddress > DX_MNG_OTP_LAST_OFFSET*sizeof(DxUint32_t)) ) {
		return DX_MNG_ILLEGAL_INPUT_PARAM_ERR;
	}

	/* Verify that the HW finished initialization */  
	DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress);
  
	/* Fetch the word */
	DX_MNG_READ_WORD_FROM_AIB(hwBaseAddress, DX_MNG_NVM_READ_ADDR | otpAddress, regVal);

	*otpWord = regVal;
  
	return DX_OK;
  
}

