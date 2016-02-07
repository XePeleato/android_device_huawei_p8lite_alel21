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

#ifndef  MLLI_PLAT_H
#define  MLLI_PLAT_H

#include "sep_lli.h"
#include "compiler.h"
#include "hw_queue_defs.h"

/******************************************************************************
*				DEFINITIONS
******************************************************************************/


/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

#define DX_GetIsMlliExternalAlloc(qid) 0
/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/


/*!
 * Rerturns the head of the  MLLI buffer.
 * 
 * \return uint32_t .
 */
uint8_t * DX_GetMLLIWorkspace(void);



#endif /*MLLI_PLAT_H*/


