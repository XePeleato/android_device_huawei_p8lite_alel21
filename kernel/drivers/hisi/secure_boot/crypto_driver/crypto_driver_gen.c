
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

#include "secureboot_basetypes.h"
#include "secureboot_general_hwdefs.h"
#include "systemdep.h"
#include "util.h"

#include "dx_hal_sb.h"
#include "crypto_driver_defs.h" 

DxUint32_t UTIL_REVERT_UINT32_BYTES (DxUint32_t val)
{
#ifdef BIG__ENDIAN
  return  ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) );
#else
  return  (val);
#endif
}

/*!
 * @brief This function clears the descriptor completion interrupt bit.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * 
 * @return none
 */      
void DX_SB_ClearDescCompletionSignal(DxUint32_t hwBaseAddress)
{

	DX_HAL_SBROM_ClearInterruptBit(hwBaseAddress, DX_HOST_RGF_ICR_DSCRPTR_COMPLETION_BIT_SHIFT);

	return;
}


/*!
 * @brief This function waits for current descriptor sequence completion.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * 
 * @return none
 */      
void DX_SB_WaitDescCompletion(DxUint32_t hwBaseAddress)
{
	DxUint32_t irr;
	DxUint32_t irrMask;

	irrMask = 1 << DX_HOST_RGF_IRR_DSCRPTR_COMPLETION_MASK_BIT_SHIFT;

	/* Wait for */
	do {
		irr = DX_HAL_SBROM_WaitInterrupt(hwBaseAddress);
	} while ((irr & irrMask) == 0);


	DX_HAL_SBROM_ClearInterruptBit(hwBaseAddress, DX_HOST_RGF_IRR_DSCRPTR_COMPLETION_MASK_BIT_SHIFT);

	return;
}



/*!
 * @brief This function adds a HW descriptor sequence to HW queue 0.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] descSeq		- a pointer to a HW descriptor sequence (5 words)
 * 
 * @return none
 */      
void DX_SB_AddDescSequence(DxUint32_t hwBaseAddress, HwDesc_s *descSeq)
{
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, DSCRPTR_QUEUE0_WORD0), (descSeq)->word[0] );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, DSCRPTR_QUEUE0_WORD1), (descSeq)->word[1] );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, DSCRPTR_QUEUE0_WORD2), (descSeq)->word[2] );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, DSCRPTR_QUEUE0_WORD3), (descSeq)->word[3] );  
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, DSCRPTR_QUEUE0_WORD4), (descSeq)->word[4] );  

	return;

}


/*!
 * @brief This function copies data from host memory to SRAM.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] src_ptr		- pointer to source buffer in host memory (BE/LE)
 * @param[in] dstAddr		- addres of destination in SRAM (LE)
 * @param[in] blockSize		- number of words
 * 
 * @return none
 */      
void DX_SB_WriteBlockToSram(DxUint32_t hwBaseAddress, DxUint32_t *src_ptr, DxUint32_t dstAddr, DxUint32_t blockSize)
{
	DxUint32_t copied, val;

	/* Set SRAM start address only once */
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, SRAM_ADDR), dstAddr);  

	for(copied = 0; copied < blockSize; copied++){
		val = *(src_ptr + copied);
		val = UTIL_REVERT_UINT32_BYTES(val);
		SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, SRAM_DATA), val );  
		WAIT_SRAM_DATA_READY();
	}

	return;

}


/*!
 * @brief This function copies data from SRAM to host memory. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] srcAddr		- addres of source in SRAM
 * @param[in] dst_ptr		- pointer to destination buffer in host memory
 * @param[in] blockSize		- number of words
 * 
 * @return none
 */      
void DX_SB_ReadBlockFromSram(DxUint32_t hwBaseAddress, DxUint32_t srcAddr, DxUint32_t *dst_ptr, DxUint8_t blockSize)
{
	DxUint32_t copied, val;

	/* Set SRAM start address only once, and read dummy data */
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, SRAM_ADDR), srcAddr);  
	SYS_ReadRegister( SB_REG_ADDR(hwBaseAddress, SRAM_DATA), val );

	for(copied = 0; copied < blockSize; copied++){
		WAIT_SRAM_DATA_READY();
		SYS_ReadRegister( SB_REG_ADDR(hwBaseAddress, SRAM_DATA), val );  
		val = UTIL_REVERT_UINT32_BYTES(val);
		*(dst_ptr + copied) = val; 		
	}

	return;
}


/*!
 * @brief This function clears data in the SRAM. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] dstAddr		- addres of destination in SRAM
 * @param[in] blockSize		- number of words
 * 
 * @return none
 */     
void DX_SB_ClearBlockInSram(DxUint32_t hwBaseAddress, DxUint32_t dstAddr, DxUint32_t blockSize)
{
	DxUint32_t copied, zero = 0;

	/* Set SRAM start address only once */
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, SRAM_ADDR), dstAddr);  

	for(copied = 0; copied < blockSize; copied++){
		SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, SRAM_DATA), zero );  
		WAIT_SRAM_DATA_READY();
	}

	return;
}



