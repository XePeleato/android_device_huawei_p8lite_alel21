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



#ifndef _SW_REVOCATION_MANAGER_ERROR_H
#define _SW_REVOCATION_MANAGER_ERROR_H

#include "secureboot_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define DX_SW_REVOCATION_INV_INPUT_PARAM            DX_SW_REVOCATION_BASE_ERROR + 0x00000001
#define DX_SW_REVOCATION_WORKSPACE_SIZE_TOO_SMALL   DX_SW_REVOCATION_BASE_ERROR + 0x00000002
#define DX_SW_REVOCATION_PUBLIC_KEY_HASH_EMPTY      DX_SW_REVOCATION_BASE_ERROR + 0x00000003
#define DX_SW_REVOCATION_CUR_SW_VERSION_ILLEGAL     DX_SW_REVOCATION_BASE_ERROR + 0x00000004
#define DX_SW_REVOCATION_UNSUPPORTED_HASH_ALGORITHM DX_SW_REVOCATION_BASE_ERROR + 0x00000005
#define DX_SW_REVOCATION_NOT_SUPPORTED              DX_SW_REVOCATION_BASE_ERROR + 0x00000006


#ifdef __cplusplus
}
#endif

#endif


