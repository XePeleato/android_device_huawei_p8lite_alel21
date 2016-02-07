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

 
 #ifndef CRYS_OTF_ERROR_H
#define CRYS_OTF_ERROR_H

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
   *  Object % CRYS_OTF_error.h    : %
   *  State           :  %state%
   *  Creation date   :  02 Sept. 2007
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module containes the definitions of the CRYS OTF errors.
   *
   *  \version CRYS_OTF_error.h#1:incl:1
   *  \authors R.Levin, yakovg
   */


/************************ Defines ******************************/

/* The CRYS OTF module errors */

#define CRYS_OTF_CSI_INVALID_CSI_STATUS_PTR_ERROR       (CRYS_OTF_MODULE_ERROR_BASE + 0x0UL)
#define CRYS_OTF_CSI_STATUS_VALUE_ERROR                 (CRYS_OTF_MODULE_ERROR_BASE + 0x1UL)
#define CRYS_OTF_CSI_INVALID_CSI_STATUS_FUNCTION_ERROR  (CRYS_OTF_MODULE_ERROR_BASE + 0x2UL)
#define CRYS_OTF_CSI_INVALID_CSI_START_FUNCTION_ERROR   (CRYS_OTF_MODULE_ERROR_BASE + 0x3UL) 

#define CRYS_OTF_INVALID_USER_CONTEXT_POINTER_ERROR	    (CRYS_OTF_MODULE_ERROR_BASE + 0x8UL)
#define CRYS_OTF_INVALID_USER_KEYS_POINTER_ERROR		(CRYS_OTF_MODULE_ERROR_BASE + 0x9UL)
#define CRYS_OTF_INVALID_ENG_MODE_POINTER_ERROR			(CRYS_OTF_MODULE_ERROR_BASE + 0xAUL)
#define CRYS_OTF_USER_CONTEXT_CORRUPTED_ERROR 			(CRYS_OTF_MODULE_ERROR_BASE + 0xBUL)
#define CRYS_OTF_ENGINE_CONFIG_ERROR     			    (CRYS_OTF_MODULE_ERROR_BASE + 0xCUL)
#define CRYS_OTF_MAC_BUFF_NULL_ERROR					(CRYS_OTF_MODULE_ERROR_BASE + 0xDUL)
#define CRYS_OTF_HASH_BUFF_NULL_ERROR				 	(CRYS_OTF_MODULE_ERROR_BASE + 0xEUL)

#ifdef __cplusplus
}
#endif

#endif


