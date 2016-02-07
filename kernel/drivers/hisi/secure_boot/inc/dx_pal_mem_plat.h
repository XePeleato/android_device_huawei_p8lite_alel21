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



#ifndef _DX_PAL_MEM_INT_H
#define _DX_PAL_MEM_INT_H


//#ifdef __cplusplus
//extern "C"
//{
//#endif
//
//#include <stdlib.h>
//#include <string.h>
#include <linux/string.h>
#include "log.h"
/** 
* @brief File Description:
*        This file conatins the implementation for memory operations APIs. 
*        The functions implementations are generally just wrappers to different operating system calls.
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/** 
 * @brief A wrapper over memcmp functionality. The function compares two given buffers
 *        according to size.
 */
#define _DX_PAL_MemCmp        memcmp
		                      
/** 
 * @brief A wrapper over memmove functionality, the function copies from one 
 *        buffer to another according to given size
 *         
 */
#define _DX_PAL_MemCopy       memcpy

#define	_DX_PAL_MemMove	      memmove
		                  
/** 
 * @brief A wrapper over memset functionality, the function sets a buffer with given value
 *        according to size
 *         
 */
#define _DX_PAL_MemSet(aTarget, aChar, aSize)        memset(aTarget, aChar, aSize)		   

/** 
 * @brief A wrapper over memset functionality, the function sets a buffer with zeroes
 *        according to size
 *         
 */
#define _DX_PAL_MemSetZero(aTarget, aSize)    memset(aTarget,0x00, aSize)
		                  	  		                  
		                                          
    
//#ifdef __cplusplus
//}
//#endif

#endif


