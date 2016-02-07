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

#ifndef  SEP_HMAC_H
#define  SEP_HMAC_H

#include "cc_plat.h"
#include "mlli.h"
#include "sep_ctx.h"
#include "dma_buffer.h"

/******************************************************************************
*				DEFINITIONS
******************************************************************************/

/******************************************************************************
*				MACROS
******************************************************************************/
/* the MAC key IPAD and OPAD bytes */
#define MAC_KEY_IPAD_BYTE 0x36
#define MAC_KEY_OPAD_BYTE 0x5C

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * This function is used to initialize the HMAC machine to perform the HMAC
 * operations. This should be the first function called.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 *
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int InitHmac(struct sep_ctx_hmac *pCtx);


/********************************************************************************/
/********************************************************************************/
/*!! we do not implement "ProcessHmac" since it directly calls ProcessHash 	*/
/********************************************************************************/
/********************************************************************************/

/*!
 * This function is used as finish operation of the HMAC machine.
 * The function may be called after "InitHmac".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeHmac(struct sep_ctx_hmac *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

#endif /*SEP_HMAC_H*/

