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

#ifndef _DX_LOG_MASK_H_
#define _DX_LOG_MASK_H_

/* CCLIB4 specific component masks */
#define DX_LOG_MASK_CCLIB    				(1)
#define DX_LOG_MASK_CRYS_API				(1<<1)
#define DX_LOG_MASK_CRYS_SYM_DRIVER			(1<<2)
#define DX_LOG_MASK_MLLI				(1<<3)
#define DX_LOG_MASK_HW_QUEUE				(1<<4)
#define DX_LOG_MASK_COMPLETION				(1<<5)
#define DX_LOG_MASK_INFRA				(1<<6)
#define DX_LOG_MASK_CRYS_LLF				(1<<13)

#endif /*_DX_LOG_MASK_H_*/

