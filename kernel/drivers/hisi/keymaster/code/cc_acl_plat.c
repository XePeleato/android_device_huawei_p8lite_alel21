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
#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CCLIB

#include "cc_acl.h"

/*!
 * SEP implementation for applet address accessing rights. 
 *  
 * \param type redundant
 * \param addr A pointer to check
 * \param size redundant
 * 
 * \return DxBool_t Returns true (zero) if applet has sufficient privilges 
 * 		to access block of memory, false (nonzero) otherwise.
 */
DxBool_t DxCcAcl_IsBuffAccessOk(AccessType_e type, void *addr, size_t size)
{
	return DX_SUCCESS;
}

