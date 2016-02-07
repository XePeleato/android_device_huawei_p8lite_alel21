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

#ifndef _DX_PAL_ABORT_PLAT_H
#define _DX_PAL_ABORT_PLAT_H

#include "dx_pal_log.h"
#include "dx_pal_compiler.h"
//#include "stdlib.h"

#define _DX_PAL_Abort(exp)  
/*
#define _DX_PAL_Abort(exp)	do {					\
	DX_PAL_LOG_ERR("ASSERT:%s:%d: %s", __FILE__, __LINE__, (#exp));	\
	abort();							\
	} while (0)
*/

#endif
