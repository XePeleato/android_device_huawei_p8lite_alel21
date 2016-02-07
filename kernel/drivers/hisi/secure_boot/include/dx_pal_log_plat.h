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

#ifndef _DX_PAL_LOG_PLAT_H_
#define _DX_PAL_LOG_PLAT_H_

#include "dx_log_mask.h"

/************** PRINTF rules ******************/
//#if defined(DEBUG)

//#define PRINTF printf

//#else /* Disable all prints */

#define PRINTF(...)  do {} while (0)

//#endif

#define __DX_PAL_LOG_PLAT(level, format, ...) PRINTF(format, ##__VA_ARGS__)

#endif /*_DX_PAL_LOG_PLAT_H_*/
