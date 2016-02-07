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



#ifndef _SBROM_MNG_ERROR_H
#define _SBROM_MNG_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* error definition - per bit */
#define DX_MNG_BASE_ERROR                    		0x0B000000
#define DX_MNG_ILLEGAL_INPUT_PARAM_ERR           	DX_MNG_BASE_ERROR + 0x00000001
#define DX_MNG_ILLEGAL_RKEK_VALUE_ERR     		DX_MNG_BASE_ERROR + 0x00000002   
#define DX_MNG_ILLEGAL_SCP_VALUE_ERR     		DX_MNG_BASE_ERROR + 0x00000004   
#define DX_MNG_ILLEGAL_OEM_HASH_VALUE_ERR     		DX_MNG_BASE_ERROR + 0x00000008   
#define DX_MNG_ILLEGAL_LCS_FOR_OPERATION_ERR   		DX_MNG_BASE_ERROR + 0x00000010   
#define DX_MNG_ILLEGAL_SESSION_KEY_ERR           	DX_MNG_BASE_ERROR + 0x00000020
#define DX_MNG_EXT_RAM_RESTORE_FAIL_ERR           	DX_MNG_BASE_ERROR + 0x00000040

/* error definition - mixed */
#define DX_MNG_ILLEGAL_OTP_KEY_ERR     			DX_MNG_BASE_ERROR + 0x00000006   

#ifdef __cplusplus
}
#endif

#endif



