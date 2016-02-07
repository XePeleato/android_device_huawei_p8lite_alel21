
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

#ifndef _DX_HAL_SB_H
#define _DX_HAL_SB_H

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/


/*!
 * Clear one bit in the Interrupt Clear Register (ICR). 
 *  
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] icrBit 		- icr bit number 
 *  
 */
void DX_HAL_SBROM_ClearInterruptBit(DxUint32_t hwBaseAddress, DxUint32_t icrBit);

/*!
 * Busy wait upon Interrupt Request Register (IRR) signals. 
 * This function notifys for any CC interrupt, it is the caller responsiblity
 * to verify and prompt the expected case interupt source. 
 *  
 * @param[in] hwBaseAddress 	- cryptocell base address 
 *  
 * \return uint32_t The IRR value.
 */
DxUint32_t DX_HAL_SBROM_WaitInterrupt(DxUint32_t hwBaseAddress);


#ifdef __cplusplus
}
#endif

#endif
