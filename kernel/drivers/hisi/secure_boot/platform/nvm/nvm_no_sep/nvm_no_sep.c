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
#include "secureboot_error.h"
#include "sb_hash.h"  
#include "nvm.h"
#include "nvm_defs.h"
#include "nvm_error.h"
#include "secureboot_general_hwdefs.h"
#include "systemdep.h"
#include "secureboot_gen.h"
#include "util.h"
#include "rsa.h"
#include "crypto_driver_defs.h" 
#include "bootimagesverifier_def.h"
#include "rsa_hwdefs.h"

#include "sbrom_management_defs.h"
#include "sbrom_management_api.h"


/************************ Defines ******************************/
void UTIL_MemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Public functions ******************************/

extern DxError_t DX_MNG_GetSwVersion(DxUint32_t hwBaseAddress, DxUint32_t *swVersion);

/************************ Private functions ******************************/
static DxUint32_t UTIL_REVERT_UINT32_BYTES (DxUint32_t val)
{
#ifdef BIG__ENDIAN
  return  ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) );
#else
  return  (val);
#endif
}

/** 
 * @brief This function reads the LCS from the SRAM/NVM
 *
 * @param[in] hwBaseAddress  -  cryptocell base address
 *
 * @param[in/out] lcs_ptr  - pointer to memory to store the LCS
 *
 * @return DxError_t - On success the value DX_OK is returned, and on failure   -a value from NVM_error.h
 */
DxError_t NVM_GetLCS(DxUint32_t hwBaseAddress, DxUint32_t *lcs_ptr)
{
  
  DxError_t error = DX_OK;
  /*------------------
      CODE
  -------------------*/

  /* Get LCS from register */  
  error = DX_MNG_GetLcs(hwBaseAddress, lcs_ptr);

  return error;
}


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
DxError_t NVM_ReadHASHPubKey(DxUint32_t hwBaseAddress, DxUint32_t pubKeyIndex, HASH_Result_t PubKeyHASH)
{
  DxError_t error = DX_OK;
  int i;
  
  error = DX_MNG_GetPubKeyHash(hwBaseAddress, pubKeyIndex, PubKeyHASH, DX_MNG_HASH_SIZE_IN_WORDS);
  
  for (i=0; i < DX_MNG_HASH_SIZE_IN_WORDS; i++) {
	  PubKeyHASH[i] = UTIL_REVERT_UINT32_BYTES( PubKeyHASH[i] );
  }

  return error;
}


/**
 * @brief The NVM_ReadAESKey function is a NVM interface function - 
 *        The function retrieves the AES CTR 256 bit key from the NVM
 *
 * @param[in] hwBaseAddress -  cryptocell base address
 *         
 * @param[out] AESKey   -  the AES key to read in
 *
 * @return DxError_t - On success the value DX_OK is returned, and on failure   -a value from NVM_error.h
 */                              
DxError_t NVM_ReadAESKey(DxUint32_t hwBaseAddress, AES_Key_t AESKey)
{

  DxError_t error = DX_OK;
  DxUint32_t otpWord;
  int i;

  for (i=0; i<AES_KEY_SIZE_IN_WORDS; i++) {
	  error = DX_MNG_ReadOTPWord(hwBaseAddress, (i+DX_MNG_OTP_KCE_OFFSET)*sizeof(DxUint32_t), &otpWord);
	  otpWord = UTIL_REVERT_UINT32_BYTES(otpWord);
	  if (error == DX_OK) {
		  UTIL_MemCopy((DxUint8_t*)&AESKey[i], (DxUint8_t*)&otpWord, sizeof(DxUint32_t));
	  }
  }
  
  return error;
}



#ifndef DX_OTP_SW_VERSION_NOT_SUPPORTED
static void COUNT_ONE_BITS(DxUint32_t number, DxUint32_t* BitCount)
{
  DxUint32_t tmp_num = number;
  *BitCount = 0;
  while (tmp_num)
    {
      tmp_num = tmp_num & (tmp_num - 1);
      *BitCount = *BitCount + 1;
    }
}
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
DxError_t NVM_GetSwVersion(DxUint32_t hwBaseAddress, DxUint32_t* swVersion)
{	
	DxUint32_t swVersionNum = 0, versionBitCount = 0;
	DxError_t error = DX_OK;

	error = DX_MNG_GetSwVersion(hwBaseAddress, &swVersionNum);
	if (error)
		return error;

	/* return the number of 1 bits */
	COUNT_ONE_BITS(swVersionNum, &versionBitCount);

	*swVersion = versionBitCount;

	return error;
}

#endif
