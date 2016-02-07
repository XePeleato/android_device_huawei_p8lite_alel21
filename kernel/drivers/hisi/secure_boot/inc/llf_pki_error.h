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

 
 
#ifndef LLF_PKI_ERROR_H
#define LLF_PKI_ERROR_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "crys_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object % llf_pki_error.h    : %
   *  State           :  %state%
   *  Creation date   :  Sun Nov 21 11:07:08 2004
   *  Last modified   :  %modify_time% 
   */
  /** @file
   *  \brief The error definitions of the LLF AES module
   *
   *  \version CRYS_AES_error.h#1:incl:1
   *  \author adams
   */




/************************ Defines ******************************/

/* base address -  0x00F10B00 */

#define LLF_PKI_HW_VERSION_NOT_CORRECT_ERROR   (LLF_PKI_MODULE_ERROR_BASE + 0x0UL)
#define LLF_PKI_HW_PIC_DECRYPED_FAILURE_ERROR  (LLF_PKI_MODULE_ERROR_BASE + 0x1UL) 
#define LLF_PKI_KG_UNSUPPORTED_KEY_SIZE        (LLF_PKI_MODULE_ERROR_BASE + 0x2UL)
 
#define LLF_PKI_CONVERT_PRIV_KEY_TO_CRT_NOT_SUPPORTED       (LLF_PKI_MODULE_ERROR_BASE + 0x10UL)
#define LLF_PKI_CONVERT_PRIV_KEY_TO_CRT_FACTORS_NOT_FOUND   (LLF_PKI_MODULE_ERROR_BASE + 0x11UL)
#define LLF_PKI_COUNTERMEASURES_DFA_FAIL_ERROR 				(LLF_PKI_MODULE_ERROR_BASE + 0x12UL)
#define LLF_PKI_CRT_WITH_512_BITS_KEY_ERROR 				(LLF_PKI_MODULE_ERROR_BASE + 0x13UL)

/* Error definitions for PKA using */
#define LLF_PKI_PKA_ILLEGAL_PTR_ERROR                       (LLF_PKI_MODULE_ERROR_BASE + 0x20UL)
#define LLF_PKI_PKA_ENTRIES_COUNT_ERROR                     (LLF_PKI_MODULE_ERROR_BASE + 0x21UL)
#define LLF_PKI_PKA_REGISTER_SIZES_ERROR                    (LLF_PKI_MODULE_ERROR_BASE + 0x22UL)
#define LLF_PKI_PKA_SET_MAP_MODE_ERROR                      (LLF_PKI_MODULE_ERROR_BASE + 0x23UL)
#define LLF_PKI_PKA_ILLEGAL_OPERAND_LEN_ERROR               (LLF_PKI_MODULE_ERROR_BASE + 0x24UL)
#define LLF_PKI_PKA_ILLEGAL_OPERAND_ERROR                   (LLF_PKI_MODULE_ERROR_BASE + 0x25UL)    
#define LLF_PKI_PKA_ILLEGAL_OPCODE_ERROR                    (LLF_PKI_MODULE_ERROR_BASE + 0x26UL)
#define LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR                 (LLF_PKI_MODULE_ERROR_BASE + 0x27UL)
#define LLF_PKI_PKA_ILLEGAL_OPERAND_TYPE_ERROR              (LLF_PKI_MODULE_ERROR_BASE + 0x28UL)
#define LLF_PKI_PKA_INVERSION_NOT_EXISTS_ERROR              (LLF_PKI_MODULE_ERROR_BASE + 0x29UL) 
#define LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR              (LLF_PKI_MODULE_ERROR_BASE + 0x2AUL)
#define LLF_PKI_PKA_OPERATION_NOT_SUPPORTED_ERROR           (LLF_PKI_MODULE_ERROR_BASE + 0x2BUL)
#define LLF_PKI_PKA_MOD_EVEN_USE_OTHER_FUNC_ERROR           (LLF_PKI_MODULE_ERROR_BASE + 0x2CUL)
#define LLF_PKI_PKA_DIVIDER_IS_NULL_ERROR                   (LLF_PKI_MODULE_ERROR_BASE + 0x2EUL)
#define LLF_PKI_PKA_MODULUS_IS_NULL_ERROR                   (LLF_PKI_MODULE_ERROR_BASE + 0x2FUL)
#define LLF_PKI_PKA_DATA_SIZE_ERROR                         (LLF_PKI_MODULE_ERROR_BASE + 0x30UL)
#define LLF_PKI_PKA_OPERATION_SIZE_ERROR                    (LLF_PKI_MODULE_ERROR_BASE + 0x31UL)
 
/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif

