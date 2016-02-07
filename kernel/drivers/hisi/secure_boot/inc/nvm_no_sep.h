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


 
#ifndef _NVM_NO_SEP_H
#define _NVM_NO_SEP_H

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief The NVM_ReadAESKey function is a NVM interface function - 
 *        The function retrieves the AES CTR 128 bit key from the SRAM/NVM
 *        In case the flag DX_NO_PUB_KEY_VERIFICATION is set the function will return a 
 *        constant pre-defined value.
 *
 * @param[in] hwBaseAddress -  cryptocell base address
 *         
 * @param[out] AESKey   -  the AES key to read in
 *
 * @return DxError_t - On success the value DX_OK is returned, and on failure   -a value from NVM_error.h
 */                              
DxError_t NVM_ReadAESKey(DxUint32_t hwBaseAddress, AES_Key_t AESKey); 

          
#ifdef __cplusplus
}
#endif

#endif


