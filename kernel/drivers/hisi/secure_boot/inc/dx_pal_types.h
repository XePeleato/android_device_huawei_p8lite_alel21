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

 
#ifndef DX_PAL_TYPES_H
#define DX_PAL_TYPES_H
#include <linux/kernel.h>

#include "dx_pal_types_plat.h"

typedef DxUint8_t               DxByte_t;

typedef enum {
    DX_FALSE = 0,
    DX_TRUE = 1
} EDxBool;

typedef DxUint32_t              DxBool_t;
typedef DxUint32_t              DxStatus;

#define DxError_t               DxStatus
#define DX_SUCCESS              0UL
#define DX_FAIL			1UL
#define DX_INFINITE             0xFFFFFFFF
#define INT8_MAX                  127//2012/11/13  LIXIAOJIE
#ifndef INT32_MAX
#define INT32_MAX 0x7FFFFFFFL 
#endif
#endif
