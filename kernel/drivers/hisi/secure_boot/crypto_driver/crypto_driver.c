/*******************************************************************  
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents.                                            *
* Use of this Software as part of or with the CryptoCell product   *
* is governed by the CryptoCell commercial end user license        *
* agreement ("CryptoCell EULA").                                   *
* It is possible that copies of this Software might be distributed *
* under some type of GNU General Public License ("GPL").           *
* Notwithstanding any such distribution under the terms of GPL,    *
* GPL does not govern the use of this Software as part of or with  *
* the CryptoCell product, for which a CryptoCell EULA is required. *
* If CryptoCell EULA allows any copy or reproduction of this       *
* Software, then such copy or reproduction must include this       *
* Copyright Notice as well as any other notices provided           *
* thereunder.                                                      *
********************************************************************/


/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "secureboot_error.h"

#include "secureboot_general_hwdefs.h"
#include "systemdep.h"
#include "secureboot_gen.h"
#include "util.h"

#include "sb_hash.h"
#include "crypto_driver_defs.h"
#include "crypto_driver_gen.h"


/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

#ifdef BIG__ENDIAN
#define HASH_SHA256_VAL 0x19CDE05B, 0xABD9831F, 0x8C68059B, 0x7F520E51, 0x3AF54FA5, 0x72F36E3C, 0x85AE67BB, 0x67E6096A
#else
#define HASH_SHA256_VAL 0x5BE0CD19, 0x1F83D9AB, 0x9B05688C, 0x510E527F, 0xA54FF53A, 0x3C6EF372, 0xBB67AE85, 0x6A09E667
#endif

/*!  
 * This function initializes the AES and HASH HW engines according to required crypto operations.
 * This should be the first function called.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] aesKey_ptr	- a pointer to AES key
 * @param[in] aesIv_ptr		- a pointer to AES IV
 * @param[in] cryptoDriverMode  - can be one of CryptoDriverMode_t 
 *  
 */
void SBROM_CryptoInit(DxUint32_t hwBaseAddress, AES_Key_t *aesKey_ptr, AES_Iv_t *aesIv_ptr, CryptoDriverMode_t cryptoDriverMode)
{
	HwDesc_s desc;
	DxUint32_t keySizeInBytes = SEP_AES_128_BIT_KEY_SIZE; 

	const DxUint32_t hashInitialDigest[]  = { HASH_SHA256_VAL };

	const DxUint32_t sramAddr = SRAM_BASE_ADDRESS;
	DxUint32_t hashDigestSramAddr = sramAddr;
	DxUint32_t hashLengthSramAddr = hashDigestSramAddr + HASH_DIGEST_SIZE_IN_BYTES;
	DxUint32_t aesIvSramAddr  = hashLengthSramAddr + HASH_LENGTH_SIZE_IN_BYTES;
	DxUint32_t aesKeySramAddr = aesIvSramAddr + AES_IV_COUNTER_SIZE_IN_BYTES;

	/* Clear descriptor completion intterupt bit */
	DX_SB_ClearDescCompletionSignal(hwBaseAddress);

	if ( ( cryptoDriverMode == CRYPTO_DRIVER_HASH_MODE) || (cryptoDriverMode == CRYPTO_DRIVER_HASH_AES_CTR_MODE) ) {

		/* Load hash digest */
		HW_DESC_INIT(&desc);
		DX_SB_WriteBlockToSram(hwBaseAddress, (DxUint32_t *)hashInitialDigest, hashDigestSramAddr, HASH_DIGEST_SIZE_IN_WORDS);
		HW_DESC_SET_CIPHER_MODE(&desc, SEP_HASH_HW_SHA256);
		HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
		HW_DESC_SET_DIN_SRAM(&desc, hashDigestSramAddr, HASH_DIGEST_SIZE_IN_BYTES);
		DX_SB_AddDescSequence(hwBaseAddress, &desc);
	
		/* Load hash current length */
		HW_DESC_INIT(&desc);
		DX_SB_ClearBlockInSram(hwBaseAddress, hashLengthSramAddr, HASH_LENGTH_SIZE_IN_WORDS);
		HW_DESC_SET_CIPHER_MODE(&desc, SEP_HASH_HW_SHA256);
		HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
		HW_DESC_SET_DIN_SRAM(&desc, hashLengthSramAddr, HASH_LENGTH_SIZE_IN_BYTES);
		DX_SB_AddDescSequence(hwBaseAddress, &desc);
	}

	if ( (cryptoDriverMode == CRYPTO_DRIVER_AES_CTR_MODE) || (cryptoDriverMode == CRYPTO_DRIVER_HASH_AES_CTR_MODE) ) {

		/* Load CTR IV */
		DX_SB_WriteBlockToSram(hwBaseAddress, (DxUint32_t *)aesIv_ptr, aesIvSramAddr, AES_IV_COUNTER_SIZE_IN_WORDS);
		HW_DESC_INIT(&desc);
		HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CTR);
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
		HW_DESC_SET_KEY_SIZE_AES(&desc, keySizeInBytes);
		HW_DESC_SET_DIN_SRAM(&desc, aesIvSramAddr, AES_IV_COUNTER_SIZE_IN_BYTES);
		HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE1);
		DX_SB_AddDescSequence(hwBaseAddress, &desc);
	
		/* Load CTR key */
		DX_SB_WriteBlockToSram(hwBaseAddress, (DxUint32_t *)aesKey_ptr, aesKeySramAddr, AES_KEY_SIZE_IN_WORDS);
		HW_DESC_INIT(&desc);
		HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CTR);
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
		HW_DESC_SET_KEY_SIZE_AES(&desc, keySizeInBytes);
		HW_DESC_SET_DIN_SRAM(&desc, aesKeySramAddr, AES_KEY_SIZE_IN_BYTES);
		HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
		DX_SB_AddDescSequence(hwBaseAddress, &desc);
	}


	return;
}


/*!  
 * This function is used to do cryptographic operations on a block(s) of data using HASH and/or AES machines.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] inputData_ptr 	- a pointer to the users data input buffer.
 * @param[out] outputData_ptr 	- a pointer to the users data output buffer.
 * @param[in] BlockSize		 - number of bytes to update.
 *                                if it is not the last block, the size must be a multiple of AES blocks. 
 * @param[in] isLastBlock  	- if false, just updates the data; otherwise, enable hash padding
 * @param[in] cryptoDriverMode  - can be one of CryptoDriverMode_t
 *  
 */
void SBROM_CryptoUpdateBlock(DxUint32_t hwBaseAddress, DxUint32_t *inputData_ptr, DxUint32_t *outputData_ptr, DxUint32_t BlockSize, DxUint8_t isLastBlock, CryptoDriverMode_t cryptoDriverMode)
{
	HwDesc_s desc;

	const DxUint32_t sramAddr = SRAM_BASE_ADDRESS;
	DxUint32_t hashLengthSramAddr = sramAddr; 

	/* Check last block to enable padding */
	if ( (isLastBlock == DX_TRUE) &&
	    (( cryptoDriverMode == CRYPTO_DRIVER_HASH_MODE) || (cryptoDriverMode == CRYPTO_DRIVER_HASH_AES_CTR_MODE)) ) {

		/* Get hash current length */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_CIPHER_MODE(&desc, SEP_HASH_HW_SHA256);
		HW_DESC_SET_CIPHER_CONFIG1(&desc, HASH_PADDING_ENABLED);
		HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
		HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE1);
		HW_DESC_SET_DOUT_SRAM(&desc, hashLengthSramAddr, HASH_LENGTH_SIZE_IN_BYTES);
		DX_SB_AddDescSequence(hwBaseAddress, &desc);
	}
	
	/* Process input data */
	/**********************/
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_TYPE(&desc, DMA_DLLI, (DxUint32_t)inputData_ptr, BlockSize, SB_AXI_ID, AXI_SECURE);

	// yeal: add comment DMa mode
	switch ( cryptoDriverMode) {
	case CRYPTO_DRIVER_HASH_MODE:
		HW_DESC_SET_FLOW_MODE(&desc, DIN_HASH);
		break;
	case CRYPTO_DRIVER_HASH_AES_CTR_MODE:
		HW_DESC_SET_FLOW_MODE(&desc, AES_and_HASH);
		HW_DESC_SET_DOUT_TYPE(&desc, DMA_DLLI, (DxUint32_t)outputData_ptr, BlockSize, SB_AXI_ID, AXI_SECURE);
		break;
	case CRYPTO_DRIVER_AES_CTR_MODE:
		HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		HW_DESC_SET_DOUT_TYPE(&desc, DMA_DLLI, (DxUint32_t)outputData_ptr, BlockSize, SB_AXI_ID, AXI_SECURE);
	default:
		break;
	}

	HW_DESC_SET_ACK_NEEDED(&desc, SB_COUNTER_ID);
	DX_SB_AddDescSequence(hwBaseAddress, &desc);

	/* wait for sequence to complete */
	DX_SB_WaitDescCompletion(hwBaseAddress);

	return;
}


/*!  
 * This function returns the digest result of crypto hash operation.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[out] hashResult 	- the HASH result.
 *  
 */
void SBROM_CryptoFinish(DxUint32_t hwBaseAddress, HASH_Result_t hashResult)
{
	HwDesc_s desc;
	const DxUint32_t outputSramAddr = SRAM_BASE_ADDRESS;

	/* Get the hash digest result */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_HASH_HW_SHA256);
	HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, HASH_DIGEST_RESULT_LITTLE_ENDIAN);
	HW_DESC_SET_CIPHER_CONFIG1(&desc, HASH_PADDING_ENABLED);
	HW_DESC_SET_DOUT_SRAM(&desc, outputSramAddr, HASH_DIGEST_SIZE_IN_BYTES);
	HW_DESC_SET_ACK_NEEDED(&desc, SB_COUNTER_ID);
	DX_SB_AddDescSequence(hwBaseAddress, &desc);

	/* wait for sequence to complete */
	DX_SB_WaitDescCompletion(hwBaseAddress);

	/* copy output data from SRAM */
	DX_SB_ReadBlockFromSram(hwBaseAddress, outputSramAddr, hashResult,  HASH_DIGEST_SIZE_IN_WORDS); 

	return;
}


