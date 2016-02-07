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


#ifndef _SECUREBOOT_ERROR_H
#define _SECUREBOOT_ERROR_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#ifdef __cplusplus
extern "C"
{
#endif


/* error definitions */


#define DX_SECUREBOOT_BASE_ERROR                 0xF0000000

#define DX_SECUREBOOT_LAYER_BASE_ERROR           0x01000000

#define DX_SECUREBOOT_VERIFIER_LAYER_PREFIX      1
#define DX_SECUREBOOT_IMAGES_MNG_LAYER_PREFIX    2
#define DX_SECUREBOOT_SW_REVOCATION_LAYER_PREFIX 3
#define DX_NVM_LAYER_PREFIX                      4
#define DX_SB_GEN_LAYER_PREFIX                   5

/* Boot Images Manager Base error = 0xF1000000 */
#define DX_BOOT_IMG_VERIFIER_BASE_ERROR          DX_SECUREBOOT_BASE_ERROR + \
                                                 DX_SECUREBOOT_VERIFIER_LAYER_PREFIX*DX_SECUREBOOT_LAYER_BASE_ERROR
/* Boot Images Manager Base error = 0xF2000000 */
#define DX_BOOT_IMG_MANAGER_BASE_ERROR           DX_SECUREBOOT_BASE_ERROR + \
                                                 DX_SECUREBOOT_IMAGES_MNG_LAYER_PREFIX*DX_SECUREBOOT_LAYER_BASE_ERROR
/* SW revocation Base error = 0xF3000000 */
#define DX_SW_REVOCATION_BASE_ERROR              DX_SECUREBOOT_BASE_ERROR + \
                                                 DX_SECUREBOOT_SW_REVOCATION_LAYER_PREFIX*DX_SECUREBOOT_LAYER_BASE_ERROR                                                 
/* NVM Base error = 0xF4000000 */
#define DX_NVM_BASE_ERROR                        DX_SECUREBOOT_BASE_ERROR + \
                                                 DX_NVM_LAYER_PREFIX*DX_SECUREBOOT_LAYER_BASE_ERROR                                                 

#define DX_SB_GEN_BASE_ERROR                     DX_SECUREBOOT_BASE_ERROR + \
                                                 DX_SB_GEN_LAYER_PREFIX*DX_SECUREBOOT_LAYER_BASE_ERROR                                                 

/*CC - SB params - origen from init_cc.h*/
#define DX_SB_GEN_FATAL_ERROR_ERR       	 DX_SB_GEN_BASE_ERROR + 0x00000001
#define DX_SB_GEN_WRONG_STATE_ERR		 DX_SB_GEN_BASE_ERROR + 0x00000002


/* the discretix error definition */


/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/


#ifdef __cplusplus
}
#endif

#endif

