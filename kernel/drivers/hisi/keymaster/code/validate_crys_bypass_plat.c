/*******************************************************************  
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents.                                            *
* Use of this Software as part of or with the CryptoCell product   *
* is governed by the CryptoCell commercial end user license        *
* agreement ("CryptoCell EULA").                                   *
* It is possible that copies of this Software might be distributed *
* under some type of GNU General Public License ("GPL").           *
* Notwithstanding any such distribution under the terms of GPL,    *
* GPL does not govern the use of this Software as part of or with  *
* the CryptoCell product, for which a CryptoCell EULA is required. *
* If CryptoCell EULA allows any copy or reproduction of this       *
* Software, then such copy or reproduction must include this       *
* Copyright Notice as well as any other notices provided           *
* thereunder.                                                      *
********************************************************************/



#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

#include "dx_pal_types.h"
#include "crys_bypass_api.h"
#include "sym_adaptor_driver.h"
#include "dma_buffer.h"
#include "bypass.h"
#include "dx_macros.h"
#include "validate_crys_bypass.h"



/*!
 * validate DMA object structure 
 *  
 * \param dmaObj the DMA object  
 * \param dataSize Data size given by the user (relevent only for SRAM data)
 *  
 * \return 0 on success, (-1) if ( dataSize != dma object data size), (-2) if sram pointer is out of range
 */

int validateParams(uint8_t *data, uint32_t dataSize)
{

	return CRYS_OK;
}

