/*******************************************************************  
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents.                                            *
* Use of this Software as part of or with the CryptoCell product   *
* is governed by the CryptoCell commercial end user license        *
* agreement ("CryptoCell EULA").                                   *
* It is possible that copies of this Software might be distributed *
* under some type of GNU General Public License ("GPL").           *
* Notwithstanding any such distribution under the terms of GPL,    *
* GPL does not govern the use of this Software as part of or with  *
* the CryptoCell product, for which a CryptoCell EULA is required. *
* If CryptoCell EULA allows any copy or reproduction of this       *
* Software, then such copy or reproduction must include this       *
* Copyright Notice as well as any other notices provided           *
* thereunder.                                                      *
********************************************************************/

#ifndef __DX_REG_BASE_HOST_H__
#define __DX_REG_BASE_HOST_H__


extern void __iomem *g_dx_base_va;

#define DX_BASE_CC g_dx_base_va
#define DX_BASE_ENV_REGS g_dx_base_va
#define DX_BASE_ENV_CC_MEMORIES g_dx_base_va
#define DX_BASE_ENV_PERF_RAM g_dx_base_va

#define DX_BASE_CRY_KERNEL     0x0UL
#define DX_BASE_ROM     g_dx_base_va

#define DX_BASE_HOST_RGF     0x0000UL
#define DX_BASE_RNG     0x0000UL

#endif //__DX_REG_BASE_HOST_H__



