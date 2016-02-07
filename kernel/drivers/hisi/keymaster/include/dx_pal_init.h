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



#ifndef _DX_PAL_INIT_H
#define _DX_PAL_INIT_H

#include "dx_pal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * PAL layer entry point. 
 * The function initializes customer platform sub components, such as memory mapping used later by CRYS to get physical contiguous memory.
 */
int DX_PAL_Init(void);



/** 
 * @brief   PAL layer entry point. 
 *          The function releases customer platform initialized sub components,
 *           by DX_PAL_Init().
 *         
 *
 * @param[in] addr - Virtual start address of contiguous memory
 * @return None
 */ 	
void DX_PAL_Terminate(void);


   
#ifdef __cplusplus
}
#endif

#endif


