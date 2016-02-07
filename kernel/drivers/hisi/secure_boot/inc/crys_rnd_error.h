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

 
 
#ifndef CRYS_RND_ERROR_H
#define CRYS_RND_ERROR_H

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
   *  Object % CRYS_RND_error.h    : %
   *  State           :  %state%
   *  Creation date   :  Sun Nov 21 11:07:08 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module contains the definitions of the CRYS AES errors.
   *
   *  \version CRYS_RND_error.h#1:incl:1
   *  \author adams
   */




/************************ Defines ******************************/
/* RND module on the CRYS layer base address - 0x00F00C00 */

#define CRYS_RND_DATA_OUT_POINTER_INVALID_ERROR        (CRYS_RND_MODULE_ERROR_BASE + 0x0UL)
#define CRYS_RND_CAN_NOT_GENERATE_RAND_IN_RANGE        (CRYS_RND_MODULE_ERROR_BASE + 0x1UL)
#define CRYS_RND_CPRNG_TEST_FAIL_ERROR		       (CRYS_RND_MODULE_ERROR_BASE + 0x2UL)
#define CRYS_RND_ADDITIONAL_INPUT_BUFFER_NULL	       (CRYS_RND_MODULE_ERROR_BASE + 0x3UL)
#define CRYS_RND_ADDITIONAL_INPUT_SIZE_ERROR	       (CRYS_RND_MODULE_ERROR_BASE + 0x4UL)
#define CRYS_RND_DATA_SIZE_OVERFLOW_ERROR	       (CRYS_RND_MODULE_ERROR_BASE + 0x5UL)
#define CRYS_RND_VECTOR_SIZE_ERROR			(CRYS_RND_MODULE_ERROR_BASE + 0x6UL)
#define CRYS_RND_RESEED_COUNTER_OVERFLOW_ERROR		(CRYS_RND_MODULE_ERROR_BASE + 0x7UL)
#define CRYS_RND_INSTANTIATION_NOT_DONE_ERROR		(CRYS_RND_MODULE_ERROR_BASE + 0x8UL)
#define CRYS_RND_TRNG_LOSS_SAMPLES_ERROR               (CRYS_RND_MODULE_ERROR_BASE + 0x9UL)
#define CRYS_RND_TRNG_TIME_EXCEED_ERROR         	(CRYS_RND_MODULE_ERROR_BASE + 0xAUL)
#define CRYS_RND_TRNG_LOSS_SAMPLES_AND_TIME_EXCEED_ERROR (CRYS_RND_MODULE_ERROR_BASE + 0xBUL)
#define CRYS_RND_IS_KAT_MODE_ERROR                     (CRYS_RND_MODULE_ERROR_BASE + 0xCUL)
#define CRYS_RND_OPERATION_IS_NOT_SUPPORTED_ERROR      (CRYS_RND_MODULE_ERROR_BASE + 0xDUL)
#define CRYS_RND_STATE_VALIDATION_TAG_ERROR            (CRYS_RND_MODULE_ERROR_BASE + 0xEUL)

#define CRYS_RND_IS_NOT_SUPPORTED                      (CRYS_RND_MODULE_ERROR_BASE + 0xFUL)

#define CRYS_RND_GEN_A_ERROR			       (CRYS_RND_MODULE_ERROR_BASE + 0x10UL)
#define CRYS_RND_GEN_B_ERROR			       (CRYS_RND_MODULE_ERROR_BASE + 0x11UL)
#define CRYS_RND_GEN_C_ERROR			       (CRYS_RND_MODULE_ERROR_BASE + 0x12UL)
#define CRYS_RND_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR (CRYS_RND_MODULE_ERROR_BASE + 0x13UL)

#define CRYS_RND_WORK_BUFFER_PTR_INVALID_ERROR          (CRYS_RND_MODULE_ERROR_BASE + 0x20UL)
#define CRYS_RND_ILLEGAL_AES_KEY_SIZE_ERROR             (CRYS_RND_MODULE_ERROR_BASE + 0x21UL)
#define CRYS_RND_ILLEGAL_DATA_PTR_ERROR                 (CRYS_RND_MODULE_ERROR_BASE + 0x22UL)
#define CRYS_RND_ILLEGAL_DATA_SIZE_ERROR                (CRYS_RND_MODULE_ERROR_BASE + 0x23UL)
#define CRYS_RND_ILLEGAL_PARAMETER_ERROR                (CRYS_RND_MODULE_ERROR_BASE + 0x24UL)
#define CRYS_RND_STATE_PTR_INVALID_ERROR                (CRYS_RND_MODULE_ERROR_BASE + 0x25UL)
#define CRYS_RND_TRNG_ERRORS_ERROR                      (CRYS_RND_MODULE_ERROR_BASE + 0x26UL)



/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif


