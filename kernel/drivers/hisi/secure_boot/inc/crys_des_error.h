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

 
#ifndef CRYS_DES_ERROR_H
#define CRYS_DES_ERROR_H

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
   *  Object % crys_des_error.h    : %
   *  State           :  %state%
   *  Creation date   :  Sun Nov 21 11:07:08 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module containes the definitions of the CRYS AES errors.
   *
   *  \version crys_des_error.h#1:incl:1
   *  \author adams
   */




/************************ Defines ******************************/

/* The CRYS DES module errors */
#define CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR     (CRYS_DES_MODULE_ERROR_BASE + 0x0UL)
#define CRYS_DES_INVALID_IV_PTR_ON_NON_ECB_MODE_ERROR   (CRYS_DES_MODULE_ERROR_BASE + 0x1UL)
#define CRYS_DES_ILLEGAL_OPERATION_MODE_ERROR           (CRYS_DES_MODULE_ERROR_BASE + 0x2UL)
#define CRYS_DES_ILLEGAL_NUM_OF_KEYS_ERROR              (CRYS_DES_MODULE_ERROR_BASE + 0x3UL)
#define CRYS_DES_INVALID_KEY_POINTER_ERROR              (CRYS_DES_MODULE_ERROR_BASE + 0x4UL)
#define CRYS_DES_INVALID_ENCRYPT_MODE_ERROR             (CRYS_DES_MODULE_ERROR_BASE + 0x5UL)
#define CRYS_DES_USER_CONTEXT_CORRUPTED_ERROR           (CRYS_DES_MODULE_ERROR_BASE + 0x6UL) 
#define CRYS_DES_DATA_IN_POINTER_INVALID_ERROR          (CRYS_DES_MODULE_ERROR_BASE + 0x7UL)
#define CRYS_DES_DATA_OUT_POINTER_INVALID_ERROR         (CRYS_DES_MODULE_ERROR_BASE + 0x8UL)
#define CRYS_DES_DATA_SIZE_ILLEGAL                      (CRYS_DES_MODULE_ERROR_BASE + 0x9UL)
#define CRYS_DES_DATA_OUT_DATA_IN_OVERLAP_ERROR         (CRYS_DES_MODULE_ERROR_BASE + 0xAUL)
#define CRYS_DES_ILLEGAL_PARAMS_ERROR			(CRYS_DES_MODULE_ERROR_BASE + 0x13UL)
	
#define CRYS_DES_IS_NOT_SUPPORTED                       (CRYS_DES_MODULE_ERROR_BASE + 0x1FUL)

/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif


