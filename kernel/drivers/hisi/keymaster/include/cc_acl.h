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

#ifndef __CC_ACL_H__
#define __CC_ACL_H__

#include "dx_pal_types.h"

/************************ Typedefs *****************************/
typedef enum {
	ACCESS_READ,
	ACCESS_READ_WRITE,
} AccessType_e;


/*********************** Functions *****************************/

/*!
 * Checks if a pointer to a block of memory is valid in a given 
 * platform. The platform implementation details should reside in 
 * platform target domain. 
 * 
 * \param type Type of access. 
 * \param addr User pointer to start of block to check
 * \param size Size of block to check
 * 
 * \return DxUint32_t Returns DX_SUCCESS if the memory block may be valid, 
 * 		nonzero error if it is definitely invalid
 */
DxUint32_t DxCcAcl_IsBuffAccessOk(AccessType_e type, void *addr, size_t size);

#endif
