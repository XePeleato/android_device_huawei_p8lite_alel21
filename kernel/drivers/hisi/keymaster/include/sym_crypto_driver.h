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

#ifndef  SEP_SYM_CRYPTO_DRIVER_H
#define  SEP_SYM_CRYPTO_DRIVER_H

#include "sep_ctx.h"
#include "dma_buffer.h"
#include "sep_ctx.h"
#include "hw_queue_defs.h"

/******************************************************************************
*				DEFINITIONS
******************************************************************************/

/******************************************************************************
*				MACROS
******************************************************************************/

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/
#ifndef ZERO_BLOCK_DEFINED
extern const DxUint32_t ZeroBlock[SEP_AES_BLOCK_SIZE_WORDS];
#endif

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/
/*!
 * Initializes sym. driver resources.
 * 
 * \return int One of DX_SYM_* error codes defined in head of file.
 */
int SymDriverInit(void) DX_ICACHE_FUNCTION;

/*!
 * Delete sym. driver resources.
 * 
 * \return int One of DX_SYM_* error codes defined in head of file.
 */
int SymDriverFini(void) DX_ICACHE_FUNCTION;

/*!
 * This function is called from the SW queue manager which passes the
 * related context. The function casts the context buffer and diverts
 * to the specific CRYS Init API according to the cipher algorithm that
 * associated in the given context. It is also prepare the necessary
 * firmware private context parameters that are require for the crypto
 * operation, for example, computation of the AES-MAC k1, k2, k3 values.
 * The API has no affect on the user data buffers.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 * 
 * \return int One of DX_SYM_* error codes defined in head of file.
 */
int SymDriverDispatchInit(struct sep_ctx_generic *pCtx);

/*!
 * This function is called from the SW queue manager in order to process 
 * a symmetric crypto operation on the user data buffers.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in head of file.
 */
int SymDriverDispatchProcess(struct sep_ctx_generic *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

/*!
 * This function is called from the SW queue manager in order to complete
 * a crypto operation. The SW queue manager calls this API when the
 * "Process" bit "0x2" is set in the SW descriptor header. This function
 * may be invoked after "DispatchDriverProcess" or "DispatchDriverInit" with any
 * number of IN/OUT MLLI tables.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in head of file.
 */
int SymDriverDispatchFinalize(struct sep_ctx_generic *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

#endif /*SEP_SYM_CRYPTO_DRIVER_H*/

