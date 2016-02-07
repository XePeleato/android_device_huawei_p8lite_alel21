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

#ifndef  SEP_COMBINED_H
#define  SEP_COMBINED_H

#include "mlli.h"
#include "sep_ctx.h"
#include "dma_buffer.h"
#include "crys_combined.h"
#include "sep_sw_desc.h"

/***************************** TYPE DEFS **********************************/
typedef uint32_t CombinedMode_t;

/******************************* ENUMS **********************************/
enum SepCombinedMode {
	SEP_COMBINED_NONE = 0,

	SEP_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE = (_SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_AES, 0) |
						       _SepCombinedEnginePackNShift(INPUT_ENGINE_1, SEP_ENGINE_AES, 1) |
						       _SepCombinedEnginePackNShift(INPUT_ENGINE_2, SEP_ENGINE_DOUT, 2)),

	SEP_COMBINED_DIN_TO_AES_TO_HASH_MODE = (_SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_AES, 0) |
						_SepCombinedEnginePackNShift(INPUT_ENGINE_1, SEP_ENGINE_HASH, 1)),

	SEP_COMBINED_DIN_TO_AES_AND_HASH_MODE = (_SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_AES, 0) |
						 _SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_HASH, 1) |
						 _SepCombinedEnginePackNShift(INPUT_ENGINE_1, SEP_ENGINE_DOUT, 2)),

	SEP_COMBINED_DIN_TO_HASH_AND_DOUT_MODE = (_SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_HASH, 0) |
						 _SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_DOUT, 1)),

	SEP_COMBINED_DIN_TO_AES_AND_DOUT_MODE = (_SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_AES, 0) |
						 _SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_DOUT, 1)),

	SEP_COMBINED_DIN_TO_AES_TO_HASH_AND_DOUT_MODE = (_SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_AES, 0) |
							 _SepCombinedEnginePackNShift(INPUT_ENGINE_1, SEP_ENGINE_HASH, 1) |
							 _SepCombinedEnginePackNShift(INPUT_ENGINE_1, SEP_ENGINE_DOUT, 2)),

	SEP_COMBINED_DIN_TO_AES_TO_AES_AND_HASH_MODE = (_SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_AES, 0) |
							_SepCombinedEnginePackNShift(INPUT_ENGINE_1, SEP_ENGINE_AES, 1) |
							_SepCombinedEnginePackNShift(INPUT_ENGINE_1, SEP_ENGINE_HASH, 2)),

	SEP_COMBINED_DIN_TO_AES_TO_AES_TO_HASH_MODE = (_SepCombinedEnginePackNShift(INPUT_DIN, SEP_ENGINE_AES, 0) |
						       _SepCombinedEnginePackNShift(INPUT_ENGINE_1, SEP_ENGINE_AES, 1) |
						       _SepCombinedEnginePackNShift(INPUT_ENGINE_2, SEP_ENGINE_HASH, 2)),

	SEP_COMBINED_RESERVE32B = INT32_MAX
};

typedef enum HwCombinedMode {
        HW_COMBINED_NONE = 0,
        HW_COMBINED_DIN_TO_AES_TO_AES_TO_DOUT_MODE = 0x1,
        HW_COMBINED_DIN_TO_AES_TO_HASH_MODE = 0x2,
        HW_COMBINED_DIN_TO_AES_AND_HASH_MODE = 0x3,
        HW_COMBINED_DIN_TO_HASH_AND_DOUT_MODE = 0x8,
        HW_COMBINED_DIN_TO_AES_AND_DOUT_MODE = 0x9,
        HW_COMBINED_DIN_TO_AES_TO_HASH_AND_DOUT_MODE = 0xA,
        HW_COMBINED_DIN_TO_AES_TO_AES_AND_HASH_MODE = 0xD,
        HW_COMBINED_DIN_TO_AES_TO_AES_TO_HASH_MODE = 0xE,
        HW_COMBINED_RESERVE32B = INT32_MAX
}HwCombinedMode_t;

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*!! we do not implement "InitCombined" since it does not perform any operation */

/*!
 * This function is used to process a block(s) of data in combined or tunneling mode.
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessCombined(struct sep_ctx_combined *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

/*!
 * This function is used as finish operation of Combined modes.
 * The function should be called after "ProcessCombined".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeCombined(struct sep_ctx_combined *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

#endif /*SEP_COMBINED_H*/

