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

 
#ifndef SECUREBOOT_GENERAL_HWDEFS_H
#define SECUREBOOT_GENERAL_HWDEFS_H
/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#define DX_CC_HOST_VIRT
#include "dx_cc_regs.h"

#ifdef DX_NO_SEP_SUPPORT
#include "dx_host.h" 
#include "dx_env.h"
#else
#include "dx_host.h" 
#endif

#include "dx_crys_kernel.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object % GeneralHwDefs.h    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:40:57 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This file containes the hardware address of the registers relevant to the HASH module.
   *
   *  \version LLF_HASH_HwDefs.h#1:incl:1
   *  \author adams
   *  \remarks Copyright (C) 2004 by Discretix Technologies Ltd.
   *           All Rights reserved
   */

/************************ Defines ******************************/
/* LCS offset in SRAM */
#define DX_LCS_OFFSET_IN_SRAM                   0xC

#ifdef DX_SB_USE_RSA_SW
/* OEM keys HASH in SRAM */
#define DX_FIRST_OEM_KEY_OFFSET_IN_SRAM		0xEE00
#endif

/* SEP params in SB usage */ 
#define DX_SB_SIGNATURE_VALID_VALUE    		0xdcc54000
#define DX_SB_GPR7_SEP_FATAL_ERROR_NOTIFICATION 0x1
#define DX_SB_GPR7_START_SECURE_BOOT_CMD_VAL    0x2


#ifndef DX_CC_SEP
#define SB_REG_ADDR(base, reg_name) 	DX_CC_REG_ADDR(base, CRY_KERNEL, reg_name)
#else
#define SB_REG_ADDR(base, reg_name) 	DX_CC_REG_ADDR(CRY_KERNEL, reg_name)
#endif

/* ********************** Macros ******************************* */
  
/************************ Enums ********************************/

#ifdef __cplusplus
}
#endif

#endif

