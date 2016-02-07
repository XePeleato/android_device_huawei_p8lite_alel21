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

#ifndef  SEP_BYPASS_H
#define  SEP_BYPASS_H

#include "sep_ctx.h"
#include "dma_buffer.h"

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/
typedef enum BypassType {
	BYPASS_SRAM	= 0,
	BYPASS_DLLI	= 1,
	BYPASS_MLLI	= 2,
	BYPASS_MAX	= INT32_MAX
} Bypass_t;

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * Memory copy using HW engines
 *  
 *  reserved [unused]
 *  pDmaInputBuffer [in] -A structure which represents the DMA input buffer.
 *  pDmaOutputBuffer [in/out] -A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessBypass(struct sep_ctx_generic *reserved, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

#endif /*BYPASS_H*/

