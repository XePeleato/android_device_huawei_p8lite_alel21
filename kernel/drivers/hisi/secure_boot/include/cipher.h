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

#ifndef  SEP_CIPHER_H
#define  SEP_CIPHER_H

#include "mlli.h"
#include "sep_ctx.h"
#include "dma_buffer.h"
#include "hw_queue_defs.h"

/******************************************************************************
*				DEFINITIONS
******************************************************************************/
#define AES_XCBC_MAC_NUM_KEYS 3
#define AES_XCBC_MAC_KEY1 1
#define AES_XCBC_MAC_KEY2 2
#define AES_XCBC_MAC_KEY3 3

#define AES_BLOCK_MASK 0xF
/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

typedef enum SepAesCoreEngine {
        SEP_AES_ENGINE1,
        SEP_AES_ENGINE2,
        SEP_AES_ENGINES_RESERVE32B = INT32_MAX
}SepAesCoreEngine_t;

/*
  RFC 3566 chapter 4 keys definitions
  This structure must be located at struct sep_ctx_cipher field "key"
*/
typedef struct XcbcMacRfcKeys {
	uint8_t K[SEP_AES_128_BIT_KEY_SIZE];
	uint8_t K1[SEP_AES_128_BIT_KEY_SIZE];
	uint8_t K2[SEP_AES_128_BIT_KEY_SIZE];
	uint8_t K3[SEP_AES_128_BIT_KEY_SIZE];
} XcbcMacRfcKeys_s;

typedef struct SepAesPrivateContext {
	/* this flag indicates whether the user processed at least
	   one data block:
	   "0" no data blocks processed
	   "1" at least one data block processed */
	uint32_t isDataBlockProcessed;
	TunnelOp_t isTunnelOp;
	SepAesCoreEngine_t engineCore;
	uint32_t tunnetDir;
} SepCipherPrivateContext_s;

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/


/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/
/*!
 * Load AES state from context
 * 
 * \param qid 
 * \param pCtx 
 */
void LoadCipherState(int qid, struct sep_ctx_cipher *pCtx,uint8_t is_zero_iv);

/*!
 * Store AES state to context
 *  
 * \param qid 
 * \param pCtx 
 */
void StoreCipherState(int qid, struct sep_ctx_cipher *pCtx);

/*!
 * Load AES key
 * 
 * \param qid 
 * \param pCtx 
 */
void LoadCipherKey(int qid, struct sep_ctx_cipher *pCtx);

/*!
 * This function is used to initialize the AES machine to perform the AES
 * operations. This should be the first function called.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int InitCipher(struct sep_ctx_cipher *pCtx);

/*!
 * This function is used to process block(s) of data using the AES machine.
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessCipher(struct sep_ctx_cipher *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

/*!
 * This function is used as finish operation of AES on XCBC, CMAC, CBC
 * and other modes besides XTS mode.
 * The function may either be called after "InitCipher" or "ProcessCipher".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeCipher(struct sep_ctx_cipher *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

/*!
 * This function is used as finish operation of AES CTS mode.
 * The function may either be called after "InitCipher" or "ProcessCipher".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessCTSFinalizeCipher(struct sep_ctx_cipher *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

#endif /*SEP_CIPHER_H*/

