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


#ifndef __DX_HAL_H__
#define __DX_HAL_H__

#include "dx_hal_plat.h"
#ifndef INT32_MAX
#define INT32_MAX 2147483647
#endif
typedef enum {
	DX_HAL_OK = 0,
	DX_HAL_ENODEV,        /* Device not opened or does not exist */
	DX_HAL_EINTERNAL,     /* Internal driver error (check system log) */
	DX_HAL_MAPFAILED,
	DX_HAL_ENOTSUP,       /* Unsupported function/option */
	DX_HAL_ENOPERM,       /* Not enough permissions for request */
	DX_HAL_EINVAL,        /* Invalid parameters */
	DX_HAL_ENORSC,        /* No resources available (e.g., memory) */
	DX_HAL_RESERVE32B = INT32_MAX
} DxHal_RetCode_t;

/*!
 * HAL layer entry point. 
 * Mappes CryptoCell 4.X regisers to the HOST virtual address space.
 */
int DX_HAL_Init(void);

/*!
 * Busy wait upon Interrupt Request Register (IRR) signals. 
 * This function notifys for any CC interrupt, it is the caller responsiblity
 * to verify and prompt the expected case interupt source.
 * 
 * @param[in] data 	- input data for future use 
 * \return uint32_t The IRR value.
 */
uint32_t DX_HAL_WaitInterrupt(uint32_t data);

/*!
 * HAL exit point. 
 * Unmaps CryptoCell 4.X registers. 
 */
int DX_HAL_Terminate(void);


#endif

