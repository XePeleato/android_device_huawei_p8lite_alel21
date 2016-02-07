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

#ifndef  SEP_HASH_H
#define  SEP_HASH_H

#include "mlli.h"
#include "sep_ctx.h"
#include "dma_buffer.h"
#include "hash_defs.h"

/******************************************************************************
*				DEFINITIONS
******************************************************************************/

/******************************************************************************
*				MACROS
******************************************************************************/

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * Get Hash digest size in bytes.
 * 
 * \param mode Hash mode
 * \param digestSize [out] A pointer to the digest size return value
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int GetHashDigestSize(const enum sep_hash_mode mode, uint32_t *digestSize);

/*!
 * Get hardware digest size (HW specific) in bytes.
 * 
 * \param mode Hash mode
 * \param hwDigestSize [out] A pointer to the digest size return value
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int GetHashHwDigestSize(const enum sep_hash_mode mode, uint32_t *hwDigestSize);
    
/*!
 * Translate Hash mode to hardware specific Hash mode.
 * 
 * \param mode Hash mode
 * \param hwMode [out] A pointer to the hash mode return value
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int GetHashHwMode(const enum sep_hash_mode mode, uint32_t *hwMode);

/*!
 * Get Hash block size in bytes.
 * 
 * \param mode Hash mode
 * \param blockSize [out] A pointer to the hash block size return value
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int GetHashBlockSize(const enum sep_hash_mode mode, uint32_t *blockSize);

/*!
 * Loads the hash digest and hash length to the Hash HW machine.
 * 
 * \param qid 
 * \param pCtx Hash context
 * \param paddingSelection enable/disable Hash block padding by the Hash machine,
 *	  should be either HASH_PADDING_DISABLED or HASH_PADDING_ENABLED.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int LoadHashState(int qid, struct sep_ctx_hash *pCtx, enum HashConfig1Padding paddingSelection);

/*!
 * Writes the hash digest and hash length back to the Hash context.
 * 
 * \param qid 
 * \param pCtx Hash context
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int StoreHashState(int qid, struct sep_ctx_hash *pCtx);

/*!
 * This function is used to initialize the HASH machine to perform the 
 * HASH operations. This should be the first function called.
 * 
 * \param pCtx A pointer to the context buffer in SRAM. 
 *  
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int InitHash(struct sep_ctx_hash *pCtx);

/*!
 * This function is used to process a block(s) of data on HASH machine.
 * It accepts an input data aligned to hash block size, any reminder which is not
 * aligned should be passed on calling to "FinalizeHash".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessHash(struct sep_ctx_hash *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

/*!
 * This function is used as finish operation of the HASH machine.
 * The function may either be called after "InitHash" or "ProcessHash".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeHash(struct sep_ctx_hash *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

#endif /*SEP_HASH_H*/

