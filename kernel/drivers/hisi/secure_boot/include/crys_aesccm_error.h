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

 
 
#ifndef CRYS_AESCCM_ERROR_H
#define CRYS_AESCCM_ERROR_H

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
   *  Object % CRYS_AESCCM_error.h    : %
   *  State           :  %state%
   *  Creation date   :  25 Sept.2008
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module contains the definitions of the CRYS AESCCM errors.
   *
   *  \version CRYS_AESCCM_error.h#1:incl:1
   *  \author R.Levin
   */

/************************ Defines ******************************/

/* The CRYS AESCCM module errors. 
   CRYS_AESCCM_MODULE_ERROR_BASE = 0x00F01500 */
#define CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR     (CRYS_AESCCM_MODULE_ERROR_BASE + 0x00UL)
#define CRYS_AESCCM_ILLEGAL_KEY_SIZE_ERROR                 (CRYS_AESCCM_MODULE_ERROR_BASE + 0x01UL)
#define CRYS_AESCCM_INVALID_KEY_POINTER_ERROR              (CRYS_AESCCM_MODULE_ERROR_BASE + 0x02UL)
#define CRYS_AESCCM_INVALID_ENCRYPT_MODE_ERROR             (CRYS_AESCCM_MODULE_ERROR_BASE + 0x03UL)
#define CRYS_AESCCM_USER_CONTEXT_CORRUPTED_ERROR           (CRYS_AESCCM_MODULE_ERROR_BASE + 0x04UL) 
#define CRYS_AESCCM_DATA_IN_POINTER_INVALID_ERROR          (CRYS_AESCCM_MODULE_ERROR_BASE + 0x05UL)
#define CRYS_AESCCM_DATA_OUT_POINTER_INVALID_ERROR         (CRYS_AESCCM_MODULE_ERROR_BASE + 0x06UL)
#define CRYS_AESCCM_DATA_IN_SIZE_ILLEGAL                   (CRYS_AESCCM_MODULE_ERROR_BASE + 0x07UL)
#define CRYS_AESCCM_DATA_OUT_DATA_IN_OVERLAP_ERROR         (CRYS_AESCCM_MODULE_ERROR_BASE + 0x08UL)
#define CRYS_AESCCM_DATA_OUT_SIZE_INVALID_ERROR            (CRYS_AESCCM_MODULE_ERROR_BASE + 0x09UL)
#define CRYS_AESCCM_ADDITIONAL_BLOCK_NOT_PERMITTED_ERROR   (CRYS_AESCCM_MODULE_ERROR_BASE + 0x0AUL)
#define CRYS_AESCCM_ILLEGAL_DMA_BUFF_TYPE_ERROR        	   (CRYS_AESCCM_MODULE_ERROR_BASE + 0x0BUL)
#define CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR           (CRYS_AESCCM_MODULE_ERROR_BASE + 0x0CUL)
#define CRYS_AESCCM_ILLEGAL_PARAMETER_PTR_ERROR            (CRYS_AESCCM_MODULE_ERROR_BASE + 0x0DUL)
#define CRYS_AESCCM_ILLEGAL_DATA_TYPE_ERROR                (CRYS_AESCCM_MODULE_ERROR_BASE + 0x0EUL)
#define CRYS_AESCCM_CCM_MAC_INVALID_ERROR                  (CRYS_AESCCM_MODULE_ERROR_BASE + 0x0FUL)
#define CRYS_AESCCM_LAST_BLOCK_NOT_PERMITTED_ERROR         (CRYS_AESCCM_MODULE_ERROR_BASE + 0x10UL)
#define CRYS_AESCCM_ILLEGAL_PARAMETER_ERROR                (CRYS_AESCCM_MODULE_ERROR_BASE + 0x11UL)
#define CRYS_AESCCM_NOT_ALL_ADATA_WAS_PROCESSED_ERROR      (CRYS_AESCCM_MODULE_ERROR_BASE + 0x13UL)
#define CRYS_AESCCM_NOT_ALL_DATA_WAS_PROCESSED_ERROR       (CRYS_AESCCM_MODULE_ERROR_BASE + 0x14UL)
#define CRYS_AESCCM_ADATA_WAS_PROCESSED_ERROR      	   (CRYS_AESCCM_MODULE_ERROR_BASE + 0x15UL)
#define CRYS_AESCCM_ILLEGAL_NONCE_SIZE_ERROR		   (CRYS_AESCCM_MODULE_ERROR_BASE + 0x16UL)
#define CRYS_AESCCM_ILLEGAL_TAG_SIZE_ERROR		   (CRYS_AESCCM_MODULE_ERROR_BASE + 0x17UL)

#define CRYS_AESCCM_ILLEGAL_PARAMS_ERROR		   (CRYS_AESCCM_MODULE_ERROR_BASE + 0x29UL)
#define CRYS_AESCCM_IS_NOT_SUPPORTED                       (CRYS_AESCCM_MODULE_ERROR_BASE + 0xFFUL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif


