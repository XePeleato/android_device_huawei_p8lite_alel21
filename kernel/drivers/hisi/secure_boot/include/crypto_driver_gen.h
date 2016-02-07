
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

#ifndef _CRYPTO_DRIVER_GEN_H
#define _CRYPTO_DRIVER_GEN_H

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
 * @brief This function clears the descriptor completion interrupt bit.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * 
 * @return none
 */      
void DX_SB_ClearDescCompletionSignal(DxUint32_t hwBaseAddress);

/*!
 * @brief This function waits for current descriptor sequence completion.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * 
 * @return none
 */      
void DX_SB_WaitDescCompletion(DxUint32_t hwBaseAddress);


/*!
 * @brief This function adds a HW descriptor sequence to HW queue 0.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] descSeq		- a pointer to a HW descriptor sequence (5 words)
 * 
 * @return none
 */      
void DX_SB_AddDescSequence(DxUint32_t hwBaseAddress, HwDesc_s *descSeq);


/*!
 * @brief This function copies data from host memory to SRAM. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] src_ptr		- pointer to source buffer in host memory
 * @param[in] dstAddr		- addres of destination in SRAM
 * @param[in] blockSize		- number of words
 * 
 * @return none
 */      
void DX_SB_WriteBlockToSram(DxUint32_t hwBaseAddress, DxUint32_t *src_ptr, DxUint32_t dstAddr, DxUint32_t blockSize);


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
void DX_SB_ReadBlockFromSram(DxUint32_t hwBaseAddress, DxUint32_t srcAddr, DxUint32_t *dst_ptr,  DxUint32_t blockSize);


/*!
 * @brief This function clears data in the SRAM. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address
 * @param[in] dstAddr		- addres of destination in SRAM
 * @param[in] blockSize		- number of words
 * 
 * @return none
 */      
void DX_SB_ClearBlockInSram(DxUint32_t hwBaseAddress, DxUint32_t dstAddr, DxUint32_t blockSize);


#ifdef __cplusplus
}
#endif

#endif
