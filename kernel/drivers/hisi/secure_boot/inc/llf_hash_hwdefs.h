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


#ifndef LLF_HASH_HWDEFS_H
#define LLF_HASH_HWDEFS_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#ifdef __cplusplus
extern "C"
{
#endif

/*
 *  Object % LLF_HASH_HwDefs.h    : %
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

/* the definitions of the HASH hardware registers offset above the base address 
   determaind by the user and the relevant bits */
 
#define HASH_HW_HASH_CTL_MD5_VAL             0x0000UL
#define HASH_HW_HASH_CTL_SHA1_VAL            0x0001UL
#define HASH_HW_HASH_CTL_SHA256_VAL	         0x0002UL

#define HASH_HW_HASH_PAD_EN_VAL	             0x1UL
#define HASH_HW_HASH_DO_PADDING_VAL	         0x4UL

#define HASH_HW_SRC_LLI_WORD1_FIRST_LLI_WORD_POS    30 
#define HASH_HW_SRC_LLI_WORD1_LAST_LLI_WORD_POS     31 

#define HASH_HW_CRYPTO_CTL_HASH_MODE_VAL     0x0007UL


/* ********************** Macros ******************************* */

#define HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr) \
do\
{ \
   DxUint32_t output_reg_val; \
   do \
   { \
      SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_BUSY) , output_reg_val ); \
   }while( output_reg_val ); \
}while(0)    

/* defining a macro for waiting to the DMA busy register */
#ifndef DX_DMA_DONE_IN_SRAM
#define HASH_HW_WAIT_ON_DMA_SRC_BUSY_BIT(hwBaseAddr) \
do \
{ \
   DxUint32_t output_reg_val; \
   do \
   { \
	SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, DIN_MEM_DMA_BUSY) , output_reg_val ); \
   }while( output_reg_val & 0x01 ); \
}while(0) 
#else
#define HASH_HW_WAIT_ON_DMA_SRC_BUSY_BIT(hwBaseAddr) \
do \
{ \
   DxUint32_t output_reg_val; \
   do \
   { \
	SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, DIN_SRAM_DMA_BUSY) , output_reg_val ); \
   }while( output_reg_val & 0x01 ); \
}while(0) 
#endif

/************************ Enums ********************************/

#ifdef __cplusplus
}
#endif

#endif
