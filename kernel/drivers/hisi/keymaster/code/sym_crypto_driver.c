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


#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_SYM_DRIVER
#define ZERO_BLOCK_DEFINED

#include "bsp.h"
#include "dx_pal_types.h"
#include "dx_pal_sem.h"
#include "cc_plat.h"
#include "sym_crypto_driver.h"
#include "dx_error.h"
#include "hw_queue.h"
#include "sep_ctx.h"
#include "cipher.h"
#include "aead.h"
#include "hash.h"
#include "hmac.h"
#include "rc4.h"
#include "bypass.h"
#include "combined.h"

/* A buffer with 0s for algorithms which require using 0 valued block */
const DxUint32_t ZeroBlock[SEP_AES_BLOCK_SIZE_WORDS] DX_SRAM_CONST;

/******************************************************************************
*				FUNCTIONS
******************************************************************************/

typedef int (*InitFunc_t)(struct sep_ctx_generic *pCtx);
typedef int (*ProcessFunc_t)(struct sep_ctx_generic *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);
typedef int (*FinalizeFunc_t)(struct sep_ctx_generic *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer);

typedef struct FunctionDispatch_t
{
	InitFunc_t initFunc;
	ProcessFunc_t processFunc;
	FinalizeFunc_t finalizeFunc;
} FunctionDispatch;

#define ALG_FUNCS(init, process, finalize)\
	{(InitFunc_t)init, (ProcessFunc_t)process, (FinalizeFunc_t)finalize},\

FunctionDispatch gFuncDispatchList[SEP_CRYPTO_ALG_NUM] DX_SRAM_CONST =
{
#if ENABLE_AES_DRIVER
	ALG_FUNCS(InitCipher, ProcessCipher, FinalizeCipher)
#else
	{NULL, NULL, NULL},
#endif
#if ENABLE_DES_DRIVER
	ALG_FUNCS(InitCipher, ProcessCipher, FinalizeCipher)
#else
	{NULL, NULL, NULL},
#endif
#if ENABLE_HASH_DRIVER
	ALG_FUNCS(InitHash, ProcessHash, FinalizeHash)
#else
	{NULL, NULL, NULL},
#endif
#if ENABLE_RC4_DRIVER
	ALG_FUNCS(InitRc4, ProcessRc4, FinalizeRc4)
#else
	{NULL, NULL, NULL},
#endif
#ifdef ENABLE_C2_DRIVER
	ALG_FUNCS(InitC2, ProcessC2, FinalizeC2)
#else
	{NULL, NULL, NULL},
#endif
#if ENABLE_HMAC_DRIVER
	ALG_FUNCS(InitHmac, ProcessHash, FinalizeHmac)
#else
	{NULL, NULL, NULL},
#endif
#if ENABLE_AEAD_DRIVER
	ALG_FUNCS(InitAead, ProcessAead, FinalizeAead)
#else
	{NULL, NULL, NULL},
#endif
#if ENABLE_BYPASS_DRIVER
	ALG_FUNCS(NULL, ProcessBypass, NULL)
#else
	{NULL, NULL, NULL},
#endif
#if ENABLE_COMBINED_DRIVER
	ALG_FUNCS(NULL, ProcessCombined, FinalizeCombined)
#else
	{NULL, NULL, NULL},
#endif
};


/*!
 * Initializes sym. driver resources.
 * 
 * \return int One of DX_SYM_* error codes defined in head of file.
 */
int SymDriverInit(void)
{
	return DX_RET_OK;
}

/*!
 * Delete sym. driver resources.
 * 
 * \return int One of DX_SYM_* error codes defined in head of file.
 */
int SymDriverFini(void)
{
	return DX_RET_OK;
}

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
int SymDriverDispatchInit(struct sep_ctx_generic *pCtx)
{
	int retcode;
	uint32_t alg;

	DX_PAL_LOG_INFO("qid=%d pCtx=%p\n", CURR_QUEUE_ID(), pCtx);

	//mdelay(1000);
	alg = ReadContextWord(&pCtx->alg);

	if (gFuncDispatchList[alg].initFunc == NULL) {
		DX_PAL_LOG_ERR("Unsupported alg %d\n", alg);
		return DX_RET_UNSUPP_ALG;
	}

	HW_QUEUE_LOCK();

	retcode = (gFuncDispatchList[alg].initFunc)(pCtx);

	HW_QUEUE_UNLOCK();

	return retcode;
}

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
int SymDriverDispatchProcess(struct sep_ctx_generic *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	int retcode;
	uint32_t alg;
	DX_PAL_LOG_INFO("qid=%d pCtx=%p\n", CURR_QUEUE_ID(), pCtx );
	
	DX_PAL_LOG_INFO("pDmaInputBuffer: pData=%p DataSize=%d DmaMode=%d nTables=%d\n",
			(uint32_t *)pDmaInputBuffer->pData, (int)pDmaInputBuffer->size, 
			(int)pDmaInputBuffer->dmaBufType, (int)pDmaInputBuffer->nTables);
	
	DX_PAL_LOG_INFO("pDmaOutputBuffer: pData=%p DataSize=%d DmaMode=%d nTables=%d\n",
			(uint32_t *)pDmaOutputBuffer->pData, (int)pDmaOutputBuffer->size, 
			(int)pDmaOutputBuffer->dmaBufType, (int)pDmaOutputBuffer->nTables);
	//mdelay(1000);
	alg = ReadContextWord(&pCtx->alg);
	
	if (gFuncDispatchList[alg].processFunc == NULL) {
		DX_PAL_LOG_ERR("Unsupported alg %d\n", alg);
		return DX_RET_UNSUPP_ALG;
	}

	HW_QUEUE_LOCK();

	retcode = (gFuncDispatchList[alg].processFunc)(pCtx, pDmaInputBuffer, pDmaOutputBuffer);

	HW_QUEUE_UNLOCK();

	return retcode;
}

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
int SymDriverDispatchFinalize(struct sep_ctx_generic *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	int retcode;	
	uint32_t alg;

	DX_PAL_LOG_INFO("qid=%d pCtx=%p\n", CURR_QUEUE_ID(), pCtx);

	DX_PAL_LOG_INFO("pDmaInputBuffer: pData=%p DataSize=%d DmaMode=%d nTables=%d\n",
			(uint32_t *)pDmaInputBuffer->pData, (int)pDmaInputBuffer->size, 
			(int)pDmaInputBuffer->dmaBufType, (int)pDmaInputBuffer->nTables);
	
	DX_PAL_LOG_INFO("pDmaOutputBuffer: pData=%p DataSize=%d DmaMode=%d nTables=%d\n",
			(uint32_t *)pDmaOutputBuffer->pData, (int)pDmaOutputBuffer->size, 
			(int)pDmaOutputBuffer->dmaBufType, (int)pDmaOutputBuffer->nTables);
	//mdelay(1000);
	alg = ReadContextWord(&pCtx->alg);
		
	if (gFuncDispatchList[alg].finalizeFunc == NULL) {
		DX_PAL_LOG_ERR("Unsupported alg %d\n", alg);
		return DX_RET_UNSUPP_ALG;
	}

	HW_QUEUE_LOCK();

	retcode = (gFuncDispatchList[alg].finalizeFunc)(pCtx, pDmaInputBuffer, pDmaOutputBuffer);

	HW_QUEUE_UNLOCK();

	return retcode;
}

