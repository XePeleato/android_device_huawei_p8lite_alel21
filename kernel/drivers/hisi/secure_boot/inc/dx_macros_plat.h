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

#ifndef  DX_MACROS_PLAT_H
#define  DX_MACROS_PLAT_H


#include "dx_cc_regs.h"
#include "dx_reg_common.h"

#define IS_VALID_DCACHE_ADDR(addr) (0)
#define IS_VALID_ICACHE_ADDR(addr) (0)
#define IS_VALID_CACHE_ADDR(addr) (0)

/* temp solution for SRAM size in CC44 */
/*#define SEP_SRAM_SIZE_GET()\
	(1 << (DX_CC_REG_FLD_GET(SEP_RGF, SEP_BOOT, SRAM_SIZE, DX_HAL_ReadCcRegister(DX_CC_REG_ADDR(SEP_RGF, SEP_BOOT))) + 11)) */

#define SEP_SRAM_SIZE_GET() 12*1024
#define IS_VALID_SRAM_ADDR(addr)\
	(((uint32_t)(addr)) < SEP_SRAM_SIZE_GET())
#endif /*DX_MACROS_PLAT_H*/
