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



#ifndef _DX_PAL_MEMALLOC_INT_H
#define _DX_PAL_MEMALLOC_INT_H

#include <linux/slab.h>

/** 
* @brief File Description:
*        This file contains wrappers for memory operations APIs. 
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/** 
 * @brief A wrapper over malloc functionality. The function allocates a buffer according to given size
 *
 */
 #define _DX_PAL_MemMalloc(x)    kmalloc(x,GFP_KERNEL)


/** 
 * @brief A wrapper over realloc functionality. The function allocates and copy a buffer
 *        according to size
 *         
 */
//#define _DX_PAL_MemRealloc    realloc


/** 
 * @brief A wrapper over free functionality/ The function will free allocated memory.
 *          
 */
#define _DX_PAL_MemFree       kfree
 

    

#endif


