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

#ifndef __KEY_BUFFER_PLAT_H__
#define __KEY_BUFFER_PLAT_H__


/* Host pointer is always "smart pointer" hence, no manipulation has
   to be made but compiling to an empty macro */
#define PTR_TO_KEY_BUFFER(ptr) ((KeyBuffer_s *)((uint32_t)(ptr)))
#define KEY_BUFFER_TO_PTR(pKeyBuffer) ((uint32_t)(pKeyBuffer))

#define INT_KEY_BUFF_SIZE_IN_BYTES 32
typedef uint8_t keyBuffer_t[INT_KEY_BUFF_SIZE_IN_BYTES]; 


enum dx_data_key_obj_api {
	DX_AES_API = 0,
	DX_AES_API_INIT = 1,
	DX_AES_WRAP_API = 2,
	DX_AES_OEM_API = 3,
	DX_AES_CMAC = 4,
};



#endif /*__KEY_BUFFER_PLAT_H__*/
