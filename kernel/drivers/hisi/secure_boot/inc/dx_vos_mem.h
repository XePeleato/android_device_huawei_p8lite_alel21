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

 
 
#ifndef _DX_VOS_MEM_H
#define _DX_VOS_MEM_H
//deleted by lixiaojie,2012/11/6
//#include <string.h>

/*! \file DX_VOS_Mem.h
This module provide memory allocation & deallocation utilities
and memory manipulation utilities.
In debug mode the module also provides memory leak detection system.
The memory leak detection system writes all allocations and deallocation
to debug log and keep track of current memory usage.

Functions that returns DxStatus type will return:
- DX_SUCCESS on success
- DX_BAD_ARGUMENTS if a pointer passed to them is NULL.
- DX_BUFFER_IS_NOT_BIG_ENOUGH in all copy functions if 
	aSourceSize is bigger than aTargetSize
	In this case only that data that can fit aTarget will be copied. 

A Buffer pointer parameter may be NULL only if its size is 0.
*/

#include "dx_vos_basetypes.h"
#define DX_VOS_MemSet memset
#define DX_VOS_MemSetZero(buf_ptr, size) memset(buf_ptr,0,size)
#define DX_VOS_FastMemCpy memcpy
#define	DX_VOS_MemCmp memcmp                                    


#ifdef __cplusplus
}
#endif

#endif /*_DX_VOS_MEM_H*/

