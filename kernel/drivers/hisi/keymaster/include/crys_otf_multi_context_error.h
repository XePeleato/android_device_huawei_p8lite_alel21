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

 
 #ifndef CRYS_OTF_MK_ERROR_H
#define CRYS_OTF_MK_ERROR_H

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
   *  Object % CRYS_OTF_MultiContext_error.h    : %
   *  State           :  %state%
   *  Creation date   :  10 Marth. 2009
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module containes the definitions of the CRYS OTF_MultiContext errors.
   *
   *  \version CRYS_OTF_MultiContext_error.h#1:incl:1
   *  \authors R.Levin
   */


/************************ Defines ******************************/

/* The CRYS OTF_MultiContext module errors */


#define CRYS_OTF_MC_INVALID_HW_CONTEXT_ID_ERROR	            (CRYS_OTF_MC_MODULE_ERROR_BASE + 0x00UL)
#define CRYS_OTF_MC_INVALID_SW_CONTEXT_POINTER_ERROR	    (CRYS_OTF_MC_MODULE_ERROR_BASE + 0x01UL)
#define CRYS_OTF_MC_SW_CONTEXT_CORRUPTED_ERROR 			    (CRYS_OTF_MC_MODULE_ERROR_BASE + 0x02UL)
#define CRYS_OTF_MC_INVALID_IV_POINTER_ERROR	        	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x03UL)
#define CRYS_OTF_MC_INVALID_IV_SIZE_ERROR	            	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x04UL)
#define CRYS_OTF_MC_INVALID_KEY_POINTER_ERROR	        	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x05UL)
#define CRYS_OTF_MC_INVALID_KEY_SIZE_ERROR	            	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x06UL)
#define CRYS_OTF_MC_INVALID_ENCR_DECR_MODE_ERROR           	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x07UL)
#define CRYS_OTF_MC_INVALID_OP_MODE_ERROR	            	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x08UL)
#define CRYS_OTF_MC_INVALID_DATA_DIRECTION_ERROR           	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x09UL)
#define CRYS_OTF_MC_INVALID_DO_TWEAK_ENCRYPT_ERROR         	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x0AUL)
#define CRYS_OTF_MC_INVALID_BYPASS_MODE_ERROR           	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x0BUL)
#define CRYS_OTF_MC_INVALID_STATUS_POINTER_ERROR           	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x0CUL)
#define CRYS_OTF_MC_CURRENT_CONTEXT_IS_BUSY_ERROR          	(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x0DUL)
#define CRYS_OTF_MC_AES_DECRYPT_NOT_ALLOWED_ON_CTR_MODE     (CRYS_OTF_MC_MODULE_ERROR_BASE + 0x0EUL)


/* this error is not returned by CRYS (returned by user function that writes to queue/memory) */
#define CRYS_OTF_MC_DATA_SIZE_ILLEGAL						(CRYS_OTF_MC_MODULE_ERROR_BASE + 0x10UL)
#define CRYS_OTF_MULTI_CONTEXT_IS_NOT_SUPPORTED             (CRYS_OTF_MC_MODULE_ERROR_BASE + 0xFFUL)



#ifdef __cplusplus
}
#endif

#endif


