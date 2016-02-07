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



#ifndef _BOOT_IMAGES_VERIFIER_ERROR_H
#define _BOOT_IMAGES_VERIFIER_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif


#define DX_BOOT_IMG_VERIFIER_INV_INPUT_PARAM                            DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000001
#define DX_BOOT_IMG_VERIFIER_NO_VRL_PASSED_VALIDATION                   DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000002
#define DX_BOOT_IMG_VERIFIER_PUBLIC_KEY_HASH_EMPTY                      DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000003
#define DX_BOOT_IMG_VERIFIER_SW_VER_SMALLER_THAN_MIN_VER                DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000004
#define DX_BOOT_IMG_VERIFIER_VRL_MAGIC_NUM_INCORRECT                    DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000005
#define DX_BOOT_IMG_VERIFIER_PUB_KEY_HASH_VALIDATION_FAILURE            DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000006
#define DX_BOOT_IMG_VERIFIER_RSA_SIG_VERIFICATION_FAILED                DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000007
#define DX_BOOT_IMG_VERIFIER_WORKSPACE_SIZE_TOO_SMALL                   DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000008
#define DX_BOOT_IMG_VERIFIER_SW_COMP_FAILED_VERIFICATION                DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000009
#define DX_BOOT_IMG_VERIFIER_SECONDARY_N_FAILED_VERIFICATION            DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000000A
#define DX_BOOT_IMG_VERIFIER_UNSUPPORTED_HASH_ALGORITHM                 DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000000B
#define DX_BOOT_IMG_VERIFIER_UNSUPPORTED_RSA_ALGORITHM                  DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000000C
#define DX_BOOT_IMG_VERIFIER_VRL_CUR_VER_SMALLER_THAN_VRL_MIN_VER       DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000000D
#define DX_BOOT_IMG_VERIFIER_PUB_KEY_VALIDATION_WITH_SAVED_KEY_FAILED   DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000000E
#define DX_BOOT_IMG_VERIFIER_HASH_BUF_TOO_SMALL                         DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000000F
#define DX_BOOT_IMG_VERIFIER_SW_COMP_INDEX_TOO_BIG                      DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000010
#define DX_BOOT_IMG_VERIFIER_SW_COMP_SIZE_IS_NULL                       DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000011
#define DX_BOOT_IMG_VERIFIER_PUB_KEY_INDEX_EXCEEDS_LIMITATION           DX_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000012



#ifdef __cplusplus
}
#endif

#endif


