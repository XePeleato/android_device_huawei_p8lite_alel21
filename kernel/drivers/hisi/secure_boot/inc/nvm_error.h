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

 
#ifndef _NVM_ERROR_H
#define _NVM_ERROR_H
#ifdef __cplusplus
extern "C"
{
#endif


#define DX_NVM_INV_INPUT_PARAM           DX_NVM_BASE_ERROR + 0x00000001
#define DX_NVM_OEM_HASH_ECC_ERROR        DX_NVM_BASE_ERROR + 0x00000002
#define DX_NVM_ZERO_AES_KEY_ERROR        DX_NVM_BASE_ERROR + 0x00000003

#ifdef __cplusplus
}
#endif

#endif


