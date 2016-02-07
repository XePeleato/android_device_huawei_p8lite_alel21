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
 
 
#ifndef LLF_AES_HWDEFS_H
#define LLF_AES_HWDEFS_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

//#include "cc_hw_defs.h"
#ifdef CRYS_AES_RKEK2_SUPPORT
#include "mng_hw_defs.h"
#endif
//#include "llf_common_hwdefs.h"


#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object % LLF_AES_HwDefs.h    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:40:57 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This file containes the hardware address of the registers relevant to the AES module.
   *
   *  \version LLF_AES_HwDefs.h#1:incl:1
   *  \author adams
   */



#define LLF_AES_HW_HLAG_192_256_VAL             0x1UL 

/* ********************** Macros ******************************* */
/* this value defines count of delays in waiting macros */
// !! Note: Change this value for increasing the performance 
#define LLF_AES_BUSY_DELAYS_COUNT  0 

/* defining a macro for waiting to the any control register */
#define LLF_AES_HW_WAIT_ON_REGISTER( VirtualHwBaseAddr, RegisterAddr ) \
do \
{ \
   volatile DxUint32_t output_reg_val;\
   for(output_reg_val = 0; (DxInt32_t)output_reg_val < LLF_AES_BUSY_DELAYS_COUNT ; output_reg_val++);\
   do \
   { \
      DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL,AES_BUSY)); \
   }while( output_reg_val & 0x1 ); \
}while(0)



/* defining a macros for waiting to the AES busy registers */							
#define LLF_AES_HW_WAIT_ON_AES_BUSY_BIT( VirtualHwBaseAddr )  LLF_AES_HW_WAIT_ON_REGISTER( VirtualHwBaseAddr, LLF_AES_HW_AES_BUSY_ADDR )


/************************ Enums ********************************/

#endif
#ifdef __cplusplus
}
#endif


