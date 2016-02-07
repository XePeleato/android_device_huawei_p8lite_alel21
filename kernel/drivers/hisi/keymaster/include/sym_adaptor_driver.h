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

#ifndef  SEP_SYM_ADAPTOR_DRIVER_H
#define  SEP_SYM_ADAPTOR_DRIVER_H

#include "mlli.h"
#include "sep_ctx.h"
#include "dma_buffer.h"

/******************************************************************************
*				MACROS
******************************************************************************/
#define SPAD_GET_MAX_BLOCKS(size) ((size) / SEP_ALG_MAX_BLOCK_SIZE)
#define SPAD_BLOCKS2BYTES(blocks) ((blocks) * SEP_ALG_MAX_BLOCK_SIZE)
#define SPAD_BYTES2BLOCKS(bytes) ((bytes) / SEP_ALG_MAX_BLOCK_SIZE)

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

typedef enum SepRangeType {
	SEP_NULL,
	SEP_SRAM,
	SEP_ICACHE,
	SEP_DCACHE,
} SepRangeType_e;

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * Allocate sym adaptor driver resources 
 * 
 * \param None 
 *  
 * \return 0 for success, otherwise failure
 */
int SymDriverAdaptorModuleInit(void);

/*!
 * Release sym adaptor driver resources 
 * 
 * \param None 
 *  
 * \return always success
 */
int SymDriverAdaptorModuleTerminate(void);

/*!
 * Initializes the caller context by invoking the symmetric dispatcher driver. 
 * The caller context may resides in SRAM or DCACHE SEP areas. 
 * This function flow is synchronouse. 
 * 
 * \param pCtx 
 *  
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int SymDriverAdaptorInit(struct sep_ctx_generic *pCtx);

/*!
 * Process a cryptographic data by invoking the symmetric dispatcher driver. 
 * The invoker may request any amount of data aligned to the given algorithm 
 * block size. It uses a scratch pad to copy (in cpu mode) the user 
 * data from DCACHE/ICACHE to SRAM for processing. This function flow is 
 * synchronouse. 
 *  
 * \param pCtx may resides in SRAM or DCACHE SeP areas
 * \param pDataIn The input data buffer. It may reside in SRAM, DCACHE or ICACHE SeP address range
 * \param pDataOut The output data buffer. It may reside in SRAM or DCACHE SeP address range
 * \param DataSize The data input size in octets 
 *  
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int SymDriverAdaptorProcess(struct sep_ctx_generic *pCtx,
				void *pDataIn, void *pDataOut, uint32_t DataSize);
/*!
 * Finalizing the cryptographic data by invoking the symmetric dispatcher driver. 
 * It calls the `SymDriverDcacheAdaptorFinalize` function for processing by leaving 
 * any reminder for the finalize operation.
 * 
 * \param pCtx may resides in SRAM or DCACHE SeP areas
 * \param pDataIn The input data buffer. It may reside in SRAM, DCACHE or ICACHE SeP address range
 * \param pDataOut The output data buffer. It may reside in SRAM or DCACHE SeP address range
 * \param DataSize The data input size in octets 
 *  
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int SymDriverAdaptorFinalize(struct sep_ctx_generic *pCtx,
				void *pDataIn, void *pDataOut, uint32_t DataSize);



#endif /*SEP_SYM_ADAPTOR_DRIVER_H*/

