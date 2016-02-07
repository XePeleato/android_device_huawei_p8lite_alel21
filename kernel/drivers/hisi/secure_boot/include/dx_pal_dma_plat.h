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



#ifndef _DX_PAL_PLAT_H
#define _DX_PAL_PLAT_H


#include "log.h"

#ifdef __cplusplus
extern "C"
{
#endif


 
/** 
 * @brief   stub function, the function should initialize the DMA mapping of the platform (if needed)
 *         
 * @param[in] buffSize - buffer size in Bytes
 * @param[in] physBuffAddr - physical start address of the memory to map
 *
 * @return Virtual start address of contiguous memory
 */ 	
extern uint32_t DX_PAL_DmaInit(uint32_t  buffSize,
			       uint32_t  physBuffAddr);

/** 
 * @brief   free system resources created in PD_PAL_DmaInit() 
 *         
 *  
 * @return void
 */ 	
extern void DX_PAL_DmaTerminate(void);
#ifdef __cplusplus
}
#endif

#endif


