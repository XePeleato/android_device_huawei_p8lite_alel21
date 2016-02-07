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

#include "crypto_driver_defs.h" 
#include "crypto_driver_gen.h"

#include "sbrom_management_defs.h"
#include "sbrom_management_api.h"


/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/*!  
 * This function is used to generate bytes stream for key derivation purposes. 
 * The function gets an input data and can use use one of the following keys: KDR, Provision, Session, or internal. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] aesKeyType	- 0 (Internal); 1 (RKEK); 2 (Provision); 3 (Session)
 * @param[in] pInternalKey	- a pointer to internal key
 * @param[in] pDataIn 		- a pointer to input buffer 
 * @param[in] dmaMode		- 0 (reserved); 1 (SRAM); 2 (DLLI); 3 (reserved) 
 * @param[in] blockSize 	- size of data in bytes
 * @param[in] pCMacResult 	- a pointer to output buffer
 *  
 */
void SBROM_AesCmac(DxUint32_t hwBaseAddress,
			HwCryptoKey_t  aesKeyType, 
			DxUint32_t *pInternalKey,
			DxUint32_t *pDataIn,
			DmaMode_t  dmaMode, 
			DxUint32_t  blockSize,
			DxUint32_t *pCMacResult)
{
	HwDesc_s desc;
	DxUint32_t keySizeInBytes;
	DxUint32_t hostBootConfig = 0;

	const DxUint32_t sramAddr = SRAM_BASE_ADDRESS;	
	DxUint32_t ivSramAddr = sramAddr;
	DxUint32_t inputSramAddr  = ivSramAddr + AES_IV_COUNTER_SIZE_IN_BYTES;
	DxUint32_t outputSramAddr = inputSramAddr + blockSize;
	DxUint32_t keySramAddr = outputSramAddr + blockSize;

	/* Check KDR size by HW; Other keys are fixed to 128bit */
	if (aesKeyType == ROOT_KEY) {
		SYS_ReadRegister(SB_REG_ADDR(hwBaseAddress, HOST_BOOT), hostBootConfig);
		hostBootConfig = (hostBootConfig >> DX_MNG_KDR_BOOT_CONFIG_SHIFT) & DX_MNG_KDR_BOOT_CONFIG_MASK;
		keySizeInBytes = hostBootConfig == 1 ? SEP_AES_256_BIT_KEY_SIZE : SEP_AES_128_BIT_KEY_SIZE; 
	}
	else {
		keySizeInBytes = SEP_AES_128_BIT_KEY_SIZE;
	}

	/* Clear descriptor completion intterupt bit */
	DX_SB_ClearDescCompletionSignal(hwBaseAddress);

	/* Load CMAC state */
	HW_DESC_INIT(&desc);
	DX_SB_ClearBlockInSram(hwBaseAddress, ivSramAddr, AES_IV_COUNTER_SIZE_IN_WORDS);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CMAC);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc, keySizeInBytes);
	HW_DESC_SET_DIN_SRAM(&desc, ivSramAddr, AES_IV_COUNTER_SIZE_IN_BYTES);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
	DX_SB_AddDescSequence(hwBaseAddress, &desc);

	/* Load key */
	HW_DESC_INIT(&desc);
	if (aesKeyType==USER_KEY) {
		DX_SB_WriteBlockToSram(hwBaseAddress, pInternalKey, keySramAddr, AES_Key128Bits_SIZE_IN_WORDS); 
		HW_DESC_SET_DIN_SRAM(&desc, keySramAddr, AES_Key128Bits_SIZE_IN_BYTES);
	}
	HW_DESC_SET_CIPHER_DO(&desc, aesKeyType); 
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CMAC);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc, keySizeInBytes);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	DX_SB_AddDescSequence(hwBaseAddress, &desc);

	/* Process input data */
	HW_DESC_INIT(&desc);
	if (dmaMode == DMA_SRAM) {
		DX_SB_WriteBlockToSram(hwBaseAddress, pDataIn, inputSramAddr, blockSize/sizeof(DxUint32_t));
		HW_DESC_SET_DIN_SRAM(&desc, inputSramAddr, blockSize);
	}
	else {
		HW_DESC_SET_DIN_TYPE(&desc, DMA_DLLI, (DxUint32_t)pDataIn, blockSize, SB_AXI_ID, AXI_SECURE);
	}
	HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
	DX_SB_AddDescSequence(hwBaseAddress, &desc);

	/* Get CMAC result */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CMAC);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	HW_DESC_SET_FLOW_MODE(&desc, S_AES_to_DOUT);
	HW_DESC_SET_DOUT_SRAM(&desc, outputSramAddr, AES_BLOCK_SIZE_IN_BYTES);
	HW_DESC_SET_ACK_NEEDED(&desc, SB_COUNTER_ID);
	DX_SB_AddDescSequence(hwBaseAddress, &desc);

	/* wait for sequence to complete */
	DX_SB_WaitDescCompletion(hwBaseAddress);

	/* copy output data from SRAM */
	DX_SB_ReadBlockFromSram(hwBaseAddress, outputSramAddr, pCMacResult,  AES_BLOCK_SIZE_IN_WORDS); 

	return;
}


/*!  
 * This function sets zeroes to the AES engine key. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address  
 *  
 */
void SBROM_AesLockEngineKey(DxUint32_t hwBaseAddress)
{
	HwDesc_s desc;

	const DxUint32_t sramAddr = SRAM_BASE_ADDRESS;
	DxUint32_t keySramAddr = sramAddr;

	/* Load empty key */ 
	DX_SB_ClearBlockInSram(hwBaseAddress, keySramAddr, AES_Key256Bits_SIZE_IN_WORDS);
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_SRAM(&desc, keySramAddr, AES_Key256Bits_SIZE_IN_BYTES);
	HW_DESC_SET_CIPHER_DO(&desc, USER_KEY); 
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CMAC);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc, SEP_AES_256_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	HW_DESC_SET_ACK_NEEDED(&desc, SB_COUNTER_ID);
	DX_SB_AddDescSequence(hwBaseAddress, &desc);

	/* wait for sequence to complete */
	DX_SB_WaitDescCompletion(hwBaseAddress);

	return;
}


/*!  
 * This function is used to perform bypass operation. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address  
 * @param[in] pDataIn 		- a pointer to input buffer 
 * @param[in] dmaModeIn		- 0 (reserved); 1 (SRAM); 2 (DLLI); 3 (reserved) 
 * @param[in] pDataOut 		- a pointer to output buffer 
 * @param[in] dmaModeOut	- 0 (reserved); 1 (SRAM); 2 (DLLI); 3 (reserved) 
 * @param[in] blockSize 	- size of data in bytes
 *  
 */ 
void SBROM_Bypass(DxUint32_t hwBaseAddress, DxUint32_t *pDataIn, DmaMode_t  dmaModeIn, DxUint32_t *pDataOut, DmaMode_t  dmaModeOut, DxUint32_t blockSize)
{
	HwDesc_s desc;

	const DxUint32_t sramAddr = SRAM_BASE_ADDRESS;
	DxUint32_t inputSramAddr = sramAddr;
	DxUint32_t outputSramAddr = sramAddr;

	/* Process bypass flow */ 
	HW_DESC_INIT(&desc);

	if (dmaModeIn == DMA_SRAM) {
		DX_SB_WriteBlockToSram(hwBaseAddress, pDataIn, inputSramAddr, blockSize/sizeof(DxUint32_t)); 
		HW_DESC_SET_DIN_SRAM(&desc, inputSramAddr, blockSize);
	}
	else{
		HW_DESC_SET_DIN_TYPE(&desc, DMA_DLLI, (DxUint32_t)pDataIn, blockSize, SB_AXI_ID, AXI_SECURE);
	}

	if (dmaModeOut == DMA_SRAM) {
		HW_DESC_SET_DOUT_SRAM(&desc, outputSramAddr, blockSize);
	}
	else { 
		HW_DESC_SET_DOUT_TYPE(&desc, DMA_DLLI, (DxUint32_t)pDataOut, blockSize, SB_AXI_ID, AXI_SECURE);
	}

	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
	HW_DESC_SET_ACK_NEEDED(&desc, SB_COUNTER_ID);
	DX_SB_AddDescSequence(hwBaseAddress, &desc);

	/* wait for sequence to complete */
	DX_SB_WaitDescCompletion(hwBaseAddress);


	/* copy output data from SRAM */
	if (dmaModeOut == DMA_SRAM) {
		DX_SB_ReadBlockFromSram(hwBaseAddress, outputSramAddr, pDataOut,  blockSize/sizeof(DxUint32_t)); 
	}
	
	return;
}






