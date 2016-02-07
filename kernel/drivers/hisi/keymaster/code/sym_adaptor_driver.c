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

#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "dx_pal_dma.h"
#include "dx_pal_log.h"
#include "dx_error.h"
#include "cc_plat.h"
#include "sym_adaptor_driver.h"
#include "sym_crypto_driver.h"
#include "completion.h"
#include "dma_object.h"
#include "sep_ctx.h"
#include "combined.h"
#include "debug.h"

#define DX_SET_AXIM_VAL()	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_ADDR_AXIM_CTRL),0x4)

#define MAX_TAIL_BUFF_SIZE  (max(MIN_CRYPTO_TAIL_SIZE, DLLI_MAX_BUFF_SIZE))

#define SET_DMA_WITH_DLLI(pDmaBuff, physAddr, buffSize) {\
		pDmaBuff->pData = (uint32_t )physAddr;\
		pDmaBuff->size = buffSize;\
		pDmaBuff->nTables = 0;\
		pDmaBuff->dmaBufType = DMA_BUF_DLLI;\
		pDmaBuff->axiNs = AXI_SECURE;\
}

#define SET_DMA_WITH_NULL(pDmaBuff) {\
		pDmaBuff->pData = (uint32_t )NULL;\
		pDmaBuff->size = 0;\
		pDmaBuff->nTables = 0;\
		pDmaBuff->dmaBufType = DMA_BUF_NULL;\
		pDmaBuff->axiNs = AXI_SECURE;\
}

#define SET_DMA_WITH_MLLI(pDmaBuff, physAddr, buffSize) {\
		pDmaBuff->pData = (uint32_t )physAddr;\
		pDmaBuff->size = buffSize;\
		pDmaBuff->nTables = 1;\
		pDmaBuff->dmaBufType = DMA_BUF_MLLI_IN_HOST;\
		pDmaBuff->axiNs = AXI_SECURE;\
}

#define SET_LLI_LOCATION(word, location, val) \
	BITFIELD_SET(word[SEP_LLI_ ## location ## _WORD_OFFSET], \
		     SEP_LLI_ ## location ## _BIT_OFFSET, \
		     SEP_LLI_ ## location ## _BIT_SIZE, \
		     val)

#define COPY_DMA_BUFF(dmaDest, dmaSrc) {\
   	     dmaDest.pData = dmaSrc.pData;\
   	     dmaDest.size = dmaSrc.size;\
   	     dmaDest.nTables = dmaSrc.nTables;\
   	     dmaDest.dmaBufType = dmaSrc.dmaBufType;\
   	     dmaDest.axiNs = dmaSrc.axiNs;\
}

#define SINGLE_BLOCK_ENTRY 1

typedef enum {
	UNMAP_FLAG_NONE = 		0x0,
	UNMAP_FLAG_CONTIG_DLLI = 	0x1,
	UNMAP_FLAG_SMALL_SIZE_DLLI = 	0x2,
	UNMAP_FLAG_MLLI_MAIN = 		0x4,
	UNMAP_FLAG_MLLI_TAIL = 		0x8,
	UNMAP_FLAG_MLLI_TABLE = 	0x10
}eUnmapFlag_t;

typedef enum {
	DMA_BUILT_FLAG_NONE = 		0x0,
	DMA_BUILT_FLAG_BI_DIR = 	0x1,
	DMA_BUILT_FLAG_INPUT_BUFF = 	0x2,
	DMA_BUILT_FLAG_OUTPUT_BUFF = 	0x4
}eDmaBuiltFlag_t;

enum dx_driver_adaptor_dir
{
	dx_driver_adaptor_in,
	dx_driver_adaptor_out
};

typedef struct {
	uint32_t	lliEntry[2];  
}lliInfo_t;

typedef struct {
	DX_PAL_DmaBlockInfo_t  mlliBlockInfo;
	lliInfo_t  	       *pLliEntry;
}mlliTable_t;

typedef struct {
	uint32_t		numOfBlocks;
	DX_PAL_DmaBlockInfo_t  	pBlockEntry[FW_MLLI_TABLE_LEN];
}dmaBuffBlocksInfo_t;

typedef struct {
         uint8_t		*pVirtBuffer;
}tailBuffInfo_t;

typedef struct {
	mlliTable_t 		devBuffer;
	tailBuffInfo_t		tailBuff;
	dmaBuffBlocksInfo_t   	blocksList;
	DX_PAL_DmaBufferHandle	buffMainH;
	DX_PAL_DmaBufferHandle	buffTailH;
	DX_PAL_DmaBufferHandle	buffMlliH;
}interDmaBuildBuffer_t;

static interDmaBuildBuffer_t g_dmaInBuildBuffH;  // internal buffer for dma, built for to device or bi-directional
static interDmaBuildBuffer_t g_dmaOutBuildBuffH;   // internal buffer for dma, built for from device

/******************************************************************************
*				PRIVATE FUNCTIONS
******************************************************************************/

/*!
 * The function returns the context size according to the algorithem type
 *
 * 
 * \param pCtx 
 *  
 * \return int The size of the context in bytes.
 */
static int SymDriverAdaptorGetCtxSize(enum sep_crypto_alg alg)
{
	DxUint32_t ctxSize; /*size in words*/

	switch (alg){
		case SEP_CRYPTO_ALG_DES:
		case SEP_CRYPTO_ALG_AES:
			ctxSize = sizeof(struct sep_ctx_cipher);
			break;
		case SEP_CRYPTO_ALG_HMAC:
		case SEP_CRYPTO_ALG_HASH:
			ctxSize = sizeof(struct sep_ctx_hash);
			break;
		case SEP_CRYPTO_ALG_RC4:
			ctxSize = sizeof(struct sep_ctx_rc4);
			break;
		case SEP_CRYPTO_ALG_BYPASS:
			ctxSize = sizeof(struct sep_ctx_generic);
			break;
		case SEP_CRYPTO_ALG_AEAD:
			ctxSize = sizeof(struct sep_ctx_aead);
			break;
		case SEP_CRYPTO_ALG_COMBINED:
			ctxSize = sizeof(struct sep_ctx_combined);
			break;
		default:
			ctxSize = 0;
		break;
	} 
	return ctxSize;
}

static void clearDmaBuildBuffers(interDmaBuildBuffer_t *pDmaBuildBuff)
{
	if (NULL == pDmaBuildBuff) {
		return;
	}
	DX_PAL_MemSetZero(pDmaBuildBuff->blocksList.pBlockEntry, FW_MLLI_TABLE_LEN * sizeof(DX_PAL_DmaBlockInfo_t));
	pDmaBuildBuff->blocksList.numOfBlocks = 0;

	DX_PAL_MemSetZero((uint8_t *)&pDmaBuildBuff->devBuffer.mlliBlockInfo, sizeof(DX_PAL_DmaBlockInfo_t));

	if (pDmaBuildBuff->tailBuff.pVirtBuffer != NULL) {
		DX_PAL_MemSetZero(pDmaBuildBuff->tailBuff.pVirtBuffer, MAX_TAIL_BUFF_SIZE);
	}
	if (pDmaBuildBuff->devBuffer.pLliEntry != NULL) {
		DX_PAL_MemSetZero(pDmaBuildBuff->devBuffer.pLliEntry, FW_MLLI_TABLE_LEN * sizeof(lliInfo_t));
	}
}


static void freeDmaBuildBuffers(interDmaBuildBuffer_t *pDmaBuildBuff)
{
	if (NULL == pDmaBuildBuff) {
		return;
	}
	if (pDmaBuildBuff->tailBuff.pVirtBuffer != NULL) {
		DX_PAL_DmaContigBufferFree(MAX_TAIL_BUFF_SIZE, pDmaBuildBuff->tailBuff.pVirtBuffer);
		pDmaBuildBuff->tailBuff.pVirtBuffer = NULL;
	}
	if (pDmaBuildBuff->devBuffer.pLliEntry != NULL) {
		DX_PAL_DmaContigBufferFree(FW_MLLI_TABLE_LEN * sizeof(lliInfo_t), (uint8_t *)pDmaBuildBuff->devBuffer.pLliEntry);
		pDmaBuildBuff->devBuffer.pLliEntry = NULL;  
	}
}

static 	uint32_t allocDmaBuildBuffers(interDmaBuildBuffer_t *pDmaBuildBuff)
{
	uint32_t rc = 0;
	uint8_t *tmpBuff = NULL;

	if (NULL == pDmaBuildBuff) {
		return DX_RET_INVARG;
	}
	tmpBuff = (uint8_t *)pDmaBuildBuff->devBuffer.pLliEntry;
	rc = DX_PAL_DmaContigBufferAllocate(FW_MLLI_TABLE_LEN * sizeof(lliInfo_t), 
					    &tmpBuff);
	if (rc != 0) {
		return DX_RET_NOMEM;
	}
	 pDmaBuildBuff->devBuffer.pLliEntry = (lliInfo_t *)tmpBuff;

	rc = DX_PAL_DmaContigBufferAllocate(MAX_TAIL_BUFF_SIZE, 
			                   &pDmaBuildBuff->tailBuff.pVirtBuffer);
	if (rc != 0) {
		freeDmaBuildBuffers(pDmaBuildBuff);
		return DX_RET_NOMEM;
	}
	clearDmaBuildBuffers(pDmaBuildBuff);
	return DX_RET_OK;
}


/** 
 * @brief   fills mlli entries based on physical addresses and sizes from blockList 
 *         
 *
 * @param[in] pUsrBlockList - list of blocks
 * @param[out] pDevBuffer - mlli list to fill
 *
 * @return success/fail
 */ 
static uint32_t   buildMlliTable(mlliTable_t *pDevBuffer, dmaBuffBlocksInfo_t *pUsrBlockList)
{
	uint32_t	i = 0;
	uint32_t	mlliEntries = 0;

	if ((NULL == pDevBuffer) ||
	    (NULL == pUsrBlockList)) {
		return DX_RET_INVARG;
	}
	// mlli table has 2 additional entries compares to pUsrBlockList->numOfBlocks:
	// first entry is dummy entry,and last entry is tail 32 bytes length
	mlliEntries = pUsrBlockList->numOfBlocks+2;
	// calculate mlli table size, dummy entry is not considered wuthin size calculation.
	// reduce 1 from MLLI entries count
	pDevBuffer->mlliBlockInfo.blockSize = (mlliEntries-1)*sizeof(lliInfo_t); 

	// fill first entry in MLLI table - dummy entry
	pDevBuffer->pLliEntry[0].lliEntry[SEP_LLI_SIZE_WORD_OFFSET] = SET_WORD_LE(0);
	pDevBuffer->pLliEntry[0].lliEntry[SEP_LLI_ADDR_WORD_OFFSET] = SET_WORD_LE(0);

	SET_LLI_LOCATION(pDevBuffer->pLliEntry[1].lliEntry, FIRST, 1);
	// fill other mlli table entries. Note that pUsrBlockList->pBlockEntry
	// has no dummy entry, therefor its indexes are (i-1)
	for (i = 1; i < mlliEntries; i++) {
		// set size bits(0-29) in second word of MLLI entry
		SET_LLI_LOCATION(pDevBuffer->pLliEntry[i].lliEntry, SIZE, pUsrBlockList->pBlockEntry[i-1].blockSize);
		// set first/last bits in bits(30/31 respectively)  in second word of MLLI entry
		if (i == (mlliEntries-1)) {
			SET_LLI_LOCATION(pDevBuffer->pLliEntry[i].lliEntry, LAST, 1);
		} else {
			SET_LLI_LOCATION(pDevBuffer->pLliEntry[i].lliEntry, LAST, 0);
		}
		// set physical address in first word of MLLI entry
		SET_LLI_LOCATION(pDevBuffer->pLliEntry[i].lliEntry, ADDR, pUsrBlockList->pBlockEntry[i-1].blockPhysAddr);

		// copy lliEntry to MLLI table - LE/BE must be considered
		pDevBuffer->pLliEntry[i].lliEntry[SEP_LLI_SIZE_WORD_OFFSET] = 
			SET_WORD_LE(pDevBuffer->pLliEntry[i].lliEntry[SEP_LLI_SIZE_WORD_OFFSET]);
		pDevBuffer->pLliEntry[i].lliEntry[SEP_LLI_ADDR_WORD_OFFSET] = 
			SET_WORD_LE(pDevBuffer->pLliEntry[i].lliEntry[SEP_LLI_ADDR_WORD_OFFSET]);
	}
	return 0;
}

/** 
 * @brief   sets user buffer into dma buffer to be used by HW 
 *         
 *
 * @param[in] pUsrBuffer - address of the buffer allocated by user
 * @param[in] usrBuffSize - data direction: into device, from device or bidirectional
 * @param[in] direction - bi-directional/to/from device
 * @param[out] pDmaBuff - dma buffer to be used by HW
 * @param[out] pInterBuildBuff - mlli list,  page list abd cookies used to build the dms bufffer
 *
 * @return success/fail
 */ 	
static uint32_t buildDmaFromDataPtr(uint8_t *pUsrBuffer, 
				    uint32_t usrBuffSize, 
				    DX_PAL_DmaBufferDirection_t direction, 
				    DmaBuffer_s *pDmaBuff,
				    interDmaBuildBuffer_t *pInterBuildBuff)
{
	uint32_t		rc = 0;
	uint32_t 		numOfBlocks = 0;
	mlliTable_t 		*pDevBuffer = NULL;
	dmaBuffBlocksInfo_t   	*pUsrBlockList = NULL;
	tailBuffInfo_t		*pTailBuff = NULL;
	uint32_t		errorClearFlag = 0;

	/// check inputs
	if ((NULL == pInterBuildBuff) ||
	    (NULL == pDmaBuff)) {
		PRINT_INFO("invalid parameters\n");
		return DX_RET_INVARG;
	}
	pDevBuffer = &pInterBuildBuff->devBuffer;
	pUsrBlockList = &pInterBuildBuff->blocksList;
	pTailBuff = &pInterBuildBuff->tailBuff;
	pUsrBlockList->numOfBlocks = (FW_MLLI_TABLE_LEN - 2);
	
        // first check if buffer is NULL, build simple empty dma buffer
	if ((NULL == pUsrBuffer) ||
	    (0 == usrBuffSize)) {
		SET_DMA_WITH_NULL(pDmaBuff);	
		return 0;
	}

	// second case, if buffer is contiguous build DLLI
	if (DX_PAL_IsDmaBufferContiguous(pUsrBuffer, usrBuffSize)) {
		pUsrBlockList->numOfBlocks = SINGLE_BLOCK_ENTRY;
		rc = DX_PAL_DmaBufferMap((uint8_t *)pUsrBuffer, 
					  usrBuffSize, 
					  direction, 
					  &pUsrBlockList->numOfBlocks,
					  pUsrBlockList->pBlockEntry, 
					  &pInterBuildBuff->buffMainH);
		if (rc != 0) {
			PRINT_INFO("failed to DX_PAL_DmaBufferMap for dlli contig user buffer 0x%x\n", rc);
			rc = DX_RET_NOMEM;
			goto endError_dataPtrToDma;
		}
		// if case numOfBlocks returned bigger than 1, we declare error
		if (pUsrBlockList->numOfBlocks > SINGLE_BLOCK_ENTRY) {
			errorClearFlag  = UNMAP_FLAG_CONTIG_DLLI;
			PRINT_INFO("failed to DX_PAL_DmaBufferMap for contig mem numOfBlocks > 1\n");
			rc = DX_RET_OSFAULT;
			goto endError_dataPtrToDma;
		}
		SET_DMA_WITH_DLLI(pDmaBuff, pUsrBlockList->pBlockEntry[0].blockPhysAddr, usrBuffSize);
		return 0;
	} 


	// in case buffer is not contiguous:
	// if buffer size smaller than  DLLI_MAX_BUFF_SIZE:
	// 					copy user buffer to tailBuff to improve performance and build DLLI
	if (usrBuffSize < DLLI_MAX_BUFF_SIZE) {
		// copy userBuffer to tailBuffer
		if ((DX_PAL_DMA_DIR_TO_DEVICE == direction) ||
		    (DX_PAL_DMA_DIR_BI_DIRECTION == direction)) {
			DX_PAL_MemCopy(pTailBuff->pVirtBuffer, (uint8_t *)pUsrBuffer, usrBuffSize);
		}
		// map tailBuff to get physical address and lock+invalidate
		pUsrBlockList->numOfBlocks = SINGLE_BLOCK_ENTRY;
		rc = DX_PAL_DmaBufferMap((uint8_t *)pTailBuff->pVirtBuffer, 
					  usrBuffSize, 
					  direction, 
					  &pUsrBlockList->numOfBlocks,
					  pUsrBlockList->pBlockEntry, 
					  &pInterBuildBuff->buffTailH);
		if (rc != 0) {
			PRINT_INFO("failed to DX_PAL_DmaBufferMap for dlli tail user buffer 0x%x\n", rc);
			rc = DX_RET_NOMEM;
			goto endError_dataPtrToDma;
		}
		// if case numOfBlocks returned bigger than 1, we declare error
		if (pUsrBlockList->numOfBlocks > SINGLE_BLOCK_ENTRY) {
			PRINT_INFO("failed to DX_PAL_DmaBufferMap for dlli tail numOfBlocks > 1\n");
			errorClearFlag  = UNMAP_FLAG_SMALL_SIZE_DLLI;
			rc = DX_RET_OSFAULT;
			goto endError_dataPtrToDma;
		}
		SET_DMA_WITH_DLLI(pDmaBuff, pUsrBlockList->pBlockEntry[0].blockPhysAddr, usrBuffSize);
		return 0;
	} 

	// otherwise (buffer size is not smaller than  DLLI_MAX_BUFF_SIZE) build MLLI: 
	// 	first seperate tail from user buffer, 
	if ((DX_PAL_DMA_DIR_TO_DEVICE == direction) ||
	    (DX_PAL_DMA_DIR_BI_DIRECTION == direction)) {
		DX_PAL_MemCopy(pTailBuff->pVirtBuffer, (uint8_t *)&pUsrBuffer[usrBuffSize-MIN_CRYPTO_TAIL_SIZE], MIN_CRYPTO_TAIL_SIZE);
	}
	// map the buffer except the last MIN_CRYPTO_TAIL_SIZE bytes
	pUsrBlockList->numOfBlocks = FW_MLLI_TABLE_LEN - 2;
	rc = DX_PAL_DmaBufferMap((uint8_t *)pUsrBuffer, 
				  (usrBuffSize-MIN_CRYPTO_TAIL_SIZE),
				  direction, 
				  &pUsrBlockList->numOfBlocks,
				  pUsrBlockList->pBlockEntry, 
				  &pInterBuildBuff->buffMainH);
	if (rc != 0) {
		PRINT_INFO("failed to DX_PAL_DmaBufferMap for mlli user buffer 0x%x\n", rc);
		rc = DX_RET_NOMEM;
		goto endError_dataPtrToDma;
	}
	// if case numOfBlocks returned bigger than (FW_MLLI_TABLE_LEN-2), we declare error since we have no room for 
	// first dummy entry in MLLI and the last entry for tail
	if (pUsrBlockList->numOfBlocks > (FW_MLLI_TABLE_LEN-2)) {
		PRINT_INFO("failed to DX_PAL_DmaBufferMap for mlli numOfBlocks > (FW_MLLI_TABLE_LEN-2)\n");
		errorClearFlag  = UNMAP_FLAG_MLLI_MAIN;
		rc = DX_RET_OSFAULT;
		goto endError_dataPtrToDma;
	}
	// map the tail
	numOfBlocks = SINGLE_BLOCK_ENTRY;
	rc = DX_PAL_DmaBufferMap((uint8_t *)pTailBuff->pVirtBuffer, 
				  MIN_CRYPTO_TAIL_SIZE, 
				  direction, 
				  &numOfBlocks,
				  &pUsrBlockList->pBlockEntry[pUsrBlockList->numOfBlocks], 
				  &pInterBuildBuff->buffTailH);
	if (rc != 0) {
		errorClearFlag  = (UNMAP_FLAG_MLLI_MAIN);
		PRINT_INFO("failed to DX_PAL_DmaBufferMap for mllitail 0x%x\n", rc);
		rc = DX_RET_NOMEM;
		goto endError_dataPtrToDma;
	}
	// if case numOfBlocks returned bigger than 1, we declare error
	if (numOfBlocks > SINGLE_BLOCK_ENTRY) {
		PRINT_INFO("failed to DX_PAL_DmaBufferMap for mlli numOfBlocks > 1\n");
		errorClearFlag  = (UNMAP_FLAG_MLLI_MAIN | UNMAP_FLAG_MLLI_TAIL);
		rc = DX_RET_OSFAULT;
		goto endError_dataPtrToDma;
	}

	// build MLLI
	buildMlliTable(pDevBuffer, pUsrBlockList);

	// map MLLI 
	numOfBlocks = SINGLE_BLOCK_ENTRY;
	rc = DX_PAL_DmaBufferMap((uint8_t *)pDevBuffer->pLliEntry, 
				  (pUsrBlockList->numOfBlocks+2)*sizeof(lliInfo_t), 
				  DX_PAL_DMA_DIR_BI_DIRECTION, 
				  &numOfBlocks,
				  &pDevBuffer->mlliBlockInfo, 
				  &pInterBuildBuff->buffMlliH);
	if (rc != 0) {
		PRINT_INFO("failed to DX_PAL_DmaBufferMap for mlli table 0x%x\n", rc);
		errorClearFlag  = (UNMAP_FLAG_MLLI_MAIN | UNMAP_FLAG_MLLI_TAIL );
		rc = DX_RET_NOMEM;
		goto endError_dataPtrToDma;
	}
	// if case numOfBlocks returned bigger than 1, we declare error
	if (numOfBlocks > SINGLE_BLOCK_ENTRY) {
		PRINT_INFO("failed to DX_PAL_DmaBufferMap for mlli numOfBlocks > 1\n");
		errorClearFlag  = (UNMAP_FLAG_MLLI_MAIN | UNMAP_FLAG_MLLI_TAIL | UNMAP_FLAG_MLLI_TABLE);
		rc = DX_RET_OSFAULT;
		goto endError_dataPtrToDma;
	}
	SET_DMA_WITH_MLLI(pDmaBuff, pDevBuffer->mlliBlockInfo.blockPhysAddr, pDevBuffer->mlliBlockInfo.blockSize);
	return 0;

endError_dataPtrToDma:
	if (UNMAP_FLAG_CONTIG_DLLI & errorClearFlag) {
		DX_PAL_DmaBufferUnmap((uint8_t *)pUsrBuffer, 
					  usrBuffSize, 
					  direction, 
					  pUsrBlockList->numOfBlocks,
					  pUsrBlockList->pBlockEntry, 
					  pInterBuildBuff->buffMainH);
	}
	if (UNMAP_FLAG_SMALL_SIZE_DLLI & errorClearFlag) {
		DX_PAL_DmaBufferUnmap((uint8_t *)pTailBuff->pVirtBuffer, 
					  usrBuffSize, 
					  direction, 
					  pUsrBlockList->numOfBlocks,
					  pUsrBlockList->pBlockEntry, 
					  pInterBuildBuff->buffTailH);
	}
	if (UNMAP_FLAG_MLLI_MAIN & errorClearFlag) {
		DX_PAL_DmaBufferUnmap((uint8_t *)pUsrBuffer, 
					  (usrBuffSize-MIN_CRYPTO_TAIL_SIZE), 
					  direction, 
					  pUsrBlockList->numOfBlocks,
					  pUsrBlockList->pBlockEntry, 
					  pInterBuildBuff->buffMainH);
	}
	if (UNMAP_FLAG_MLLI_TAIL & errorClearFlag) {
		DX_PAL_DmaBufferUnmap((uint8_t *)pTailBuff->pVirtBuffer, 
				       MIN_CRYPTO_TAIL_SIZE, 
				       direction, 
				       SINGLE_BLOCK_ENTRY,
				       &pUsrBlockList->pBlockEntry[pUsrBlockList->numOfBlocks], 
				       pInterBuildBuff->buffTailH);
	}
	if (UNMAP_FLAG_MLLI_TABLE & errorClearFlag) {
		DX_PAL_DmaBufferUnmap((uint8_t *)pDevBuffer->pLliEntry, 
				      (pUsrBlockList->numOfBlocks+2)*sizeof(lliInfo_t), 
				      DX_PAL_DMA_DIR_BI_DIRECTION, 
				      SINGLE_BLOCK_ENTRY,
				      &pDevBuffer->mlliBlockInfo, 
				      pInterBuildBuff->buffMlliH);
	}
	return rc;

}


/** 
 * @brief   sets user buffer into dma buffer to be used by HW 
 *         
 *
 * @param[in] pUsrBuffer - address of the buffer allocated by user
 * @param[in] usrBuffSize - data direction: into device, from device or bidirectional
 * @param[in] direction - bi-directional/to/from device
 * @param[in] pDmaBuff - dma buffer to be used by HW
 * @param[in] pInterBuildBuff - mlli list,  page list abd cookies used to build the dms bufffer
 *
 * @return success/fail
 */ 	
static uint32_t buildDataPtrFromDma(uint8_t 		*pUsrBuffer, 
		                    uint32_t 		usrBuffSize, 
		                    DX_PAL_DmaBufferDirection_t direction, 
		                    DmaBuffer_s 	*pDmaBuff,
		                    interDmaBuildBuffer_t *pInterBuildBuff)
{
	uint32_t		rc = 0;
	tailBuffInfo_t		*pTailBuff = NULL;
	mlliTable_t 		*pDevBuffer = NULL;
	dmaBuffBlocksInfo_t   	*pUsrBlockList = NULL;

	/// check inputs
	if (NULL == pInterBuildBuff) {
		PRINT_INFO("invalid parameters\n");
		return DX_RET_INVARG;
	}
        // first check if buffer is NULL, build simple empty dma buffer
	if ((NULL == pUsrBuffer) ||
	    (0 == usrBuffSize)) {
		return 0;
	}

	pDevBuffer = &pInterBuildBuff->devBuffer;
	pUsrBlockList = &pInterBuildBuff->blocksList;
	pTailBuff = &pInterBuildBuff->tailBuff;
	
	// second case, if buffer is contiguous build DLLI
	if (DX_PAL_IsDmaBufferContiguous(pUsrBuffer, usrBuffSize)) {
		rc = DX_PAL_DmaBufferUnmap((uint8_t *)pUsrBuffer, 
				       usrBuffSize, 
				       direction, 
				       pUsrBlockList->numOfBlocks,
				       pUsrBlockList->pBlockEntry, 
				       pInterBuildBuff->buffMainH);
		goto endError_dmaToDataPtr;
	} 


	// in case buffer is not contiguous:
	// if buffer size smaller than  DLLI_MAX_BUFF_SIZE:
	// 					copy user buffer to tailBuff to improve performance and build DLLI
	if (usrBuffSize < DLLI_MAX_BUFF_SIZE) {
		// map tailBuff to get physical address and lock+invalidate
		rc = DX_PAL_DmaBufferUnmap((uint8_t *)pTailBuff->pVirtBuffer, 
				       usrBuffSize, 
				       direction, 
				       pUsrBlockList->numOfBlocks,
				       pUsrBlockList->pBlockEntry, 
				       pInterBuildBuff->buffTailH);
		// copy userBuffer to tailBuffer
		if ((DX_PAL_DMA_DIR_FROM_DEVICE == direction) ||
		    (DX_PAL_DMA_DIR_BI_DIRECTION == direction)) {
			DX_PAL_MemCopy((uint8_t *)pUsrBuffer, pTailBuff->pVirtBuffer, usrBuffSize);
		}
		goto endError_dmaToDataPtr;
	} 

	// otherwise (buffer size smaller than  DLLI_MAX_BUFF_SIZE) build MLLI: 
	// 					first seperate tail from user buffer, 
	// unmap the buffer except the last MIN_CRYPTO_TAIL_SIZE bytes
	rc = DX_PAL_DmaBufferUnmap((uint8_t *)pUsrBuffer, 
			       (usrBuffSize-MIN_CRYPTO_TAIL_SIZE), 
			       direction, 
			       pUsrBlockList->numOfBlocks,
			       pUsrBlockList->pBlockEntry, 
			       pInterBuildBuff->buffMainH);
	// Unmap the tail
	rc |= DX_PAL_DmaBufferUnmap((uint8_t *)pTailBuff->pVirtBuffer, 
			      MIN_CRYPTO_TAIL_SIZE, 
			      direction, 
			      SINGLE_BLOCK_ENTRY,
			      &pUsrBlockList->pBlockEntry[pUsrBlockList->numOfBlocks], 
			      pInterBuildBuff->buffTailH);
	if ((DX_PAL_DMA_DIR_FROM_DEVICE == direction) ||
	    (DX_PAL_DMA_DIR_BI_DIRECTION == direction)) {
		DX_PAL_MemCopy((uint8_t *)&pUsrBuffer[usrBuffSize-MIN_CRYPTO_TAIL_SIZE], pTailBuff->pVirtBuffer, MIN_CRYPTO_TAIL_SIZE);
	}

	// Unmap MLLI 
	rc |= DX_PAL_DmaBufferUnmap((uint8_t *)pDevBuffer->pLliEntry, 
			       (pUsrBlockList->numOfBlocks+2)*sizeof(lliInfo_t), 
			       DX_PAL_DMA_DIR_BI_DIRECTION, 
			       SINGLE_BLOCK_ENTRY,
			       &pDevBuffer->mlliBlockInfo, 
			       pInterBuildBuff->buffMlliH);

endError_dmaToDataPtr:  
	if (rc != 0) {
		rc = DX_RET_BUSY;
	}
	return rc;

}



/******************************************************************************
*				PUBLIC FUNCTIONS
******************************************************************************/

/*!
 * Allocate sym adaptor driver resources 
 * 
 * \param None 
 *  
 * \return 0 for success, otherwise failure
 */
int SymDriverAdaptorModuleInit()
{
	int symRc = DX_RET_OK;

	symRc = allocDmaBuildBuffers(&g_dmaInBuildBuffH);
	if (symRc != DX_RET_OK) {
		return DX_RET_NOMEM;
	}

	symRc = allocDmaBuildBuffers(&g_dmaOutBuildBuffH);
	if (symRc != DX_RET_OK) {
		freeDmaBuildBuffers(&g_dmaInBuildBuffH);
		return DX_RET_NOMEM;
	}

	return DX_RET_OK;
}


/*!
 * Release sym adaptor driver resources 
 * 
 * \param None 
 *  
 * \return always success
 */
int SymDriverAdaptorModuleTerminate()
{
	freeDmaBuildBuffers(&g_dmaInBuildBuffH);
	freeDmaBuildBuffers(&g_dmaOutBuildBuffH);
	return DX_RET_OK;
}

static 	uint32_t symDriverAdaptorCopySramBuff(enum dx_driver_adaptor_dir dir ,uint32_t *PSram, uint32_t *buff, uint32_t size)
{
	DmaBuffer_s dmaBuffIn, dmaBuffOut;
	DX_PAL_DmaBufferHandle dmaHandle;
	DX_PAL_DmaBlockInfo_t	dmaBlockEntry;
	uint32_t numOfBlocks;
	uint32_t rc,symRc;
	numOfBlocks = SINGLE_BLOCK_ENTRY;

	rc = DX_PAL_DmaBufferMap((uint8_t *)buff, 
				  size, 
				  DX_PAL_DMA_DIR_BI_DIRECTION, 
				  &numOfBlocks,
				  &dmaBlockEntry, 
				  &dmaHandle);
	if (rc != 0) {
		PRINT_INFO("failed to DX_PAL_DmaBufferMap for contig user context  0x%x\n", rc);
		return DX_RET_NOMEM;
	}

	if(dir == dx_driver_adaptor_in) {
		SET_DMA_WITH_DLLI(((DmaBuffer_s*)&dmaBuffIn), dmaBlockEntry.blockPhysAddr, size);

		dmaBuffOut.size = size;
		dmaBuffOut.pData = (uint32_t)PSram;
		dmaBuffOut.nTables = 0;
		dmaBuffOut.dmaBufType = DMA_BUF_SEP;
		dmaBuffOut.axiNs = 0;
	}else {
		SET_DMA_WITH_DLLI(((DmaBuffer_s*)&dmaBuffOut), dmaBlockEntry.blockPhysAddr, size);

		dmaBuffIn.size = size;
		dmaBuffIn.pData = (uint32_t)PSram;
		dmaBuffIn.nTables = 0;
		dmaBuffIn.dmaBufType = DMA_BUF_SEP;
		dmaBuffIn.axiNs = 0;

	}
	/* Write BYPASS to the context cache last word so the sydriverdispather will recognize it as bypass */
	/* using rc as temporary buffer as it must be zero in this stage  */
	rc = SEP_CRYPTO_ALG_BYPASS;
	_WriteWordsToSram(DX_DRIVER_ADAPTOR_CONTEXT_CACHE_LAST_WORD_ADDR,&rc,sizeof(rc));
	symRc = SymDriverDispatchProcess((struct sep_ctx_generic *)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_LAST_WORD_ADDR, &dmaBuffIn, &dmaBuffOut);
	if (symRc != DX_RET_OK) {
		goto EndWithErr;
	}
	WaitForSequenceCompletion();

EndWithErr:
	rc = DX_PAL_DmaBufferUnmap((uint8_t *)buff, 
				  size, 
				  DX_PAL_DMA_DIR_BI_DIRECTION, 
				  numOfBlocks,
				  &dmaBlockEntry, 
				  dmaHandle);
	if(symRc) {
		return symRc;
	}

	return rc;

}

static 	uint32_t symDriverAdaptorCopyCtx(enum dx_driver_adaptor_dir dir ,uint32_t sram_address, struct sep_ctx_generic *pCtx )
{
	uint32_t rc = 0;
	switch (dir) {
		case dx_driver_adaptor_in:

			if(pCtx->alg == SEP_CRYPTO_ALG_COMBINED) {

				uint32_t engIdx;
				uint32_t ctx_size;
				struct sep_ctx_combined *pCombinedCtx =(struct sep_ctx_combined *)pCtx;
				uint32_t sram_temp_address = sram_address + SymDriverAdaptorGetCtxSize(pCtx->alg);

                		for ( engIdx = 0;
                		((engIdx < SEP_MAX_COMBINED_ENGINES) && (pCombinedCtx->sub_ctx[engIdx] != NULL));
                		engIdx++ ) {

					ctx_size = SymDriverAdaptorGetCtxSize(pCombinedCtx->sub_ctx[engIdx]->alg);
                			/*save host address of sub context*/
                			pCombinedCtx->hostAddr[engIdx] =  (uint32_t)pCombinedCtx->sub_ctx[engIdx];
                
                			/*copy sub context to SRAM*/
                			rc = symDriverAdaptorCopySramBuff(dx_driver_adaptor_in, (uint32_t*)sram_temp_address, 
                						(uint32_t*)(pCombinedCtx->sub_ctx[engIdx]), ctx_size);
					if(rc != 0) {
						return rc;
					}
                			/*insert sram address of sub context */
                			pCombinedCtx->sub_ctx[engIdx] = (struct sep_ctx_cache_entry*)sram_temp_address;
                
                			/*locate ptr of sram address to next position */
                			sram_temp_address += ctx_size;		
                		}
                
                	}
                	rc = symDriverAdaptorCopySramBuff(dx_driver_adaptor_in, (uint32_t*)sram_address, (uint32_t*)pCtx, SymDriverAdaptorGetCtxSize(pCtx->alg));
			break;
		case dx_driver_adaptor_out:
			if(pCtx->alg == SEP_CRYPTO_ALG_COMBINED) {

				uint32_t engIdx;
				uint32_t ctx_size;
				struct sep_ctx_combined *pCombinedCtx =(struct sep_ctx_combined *)pCtx;

				for ( engIdx = 0;
				      ((engIdx < SEP_MAX_COMBINED_ENGINES) && (pCombinedCtx->sub_ctx[engIdx] != NULL));
				      engIdx++ ) {
					ctx_size = SymDriverAdaptorGetCtxSize(((struct sep_ctx_cache_entry*)(pCombinedCtx->hostAddr[engIdx]))->alg);

					rc = symDriverAdaptorCopySramBuff(dx_driver_adaptor_out, (uint32_t*)pCombinedCtx->sub_ctx[engIdx],
        						(uint32_t*)pCombinedCtx->hostAddr[engIdx], ctx_size);

					if(rc != 0) {
						return rc;
					}
					/*insert sram address of sub context */
					pCombinedCtx->sub_ctx[engIdx] = (struct sep_ctx_cache_entry*)pCombinedCtx->hostAddr[engIdx];
				}
			} else {
        	
				rc = symDriverAdaptorCopySramBuff(dx_driver_adaptor_out, (uint32_t*)sram_address, (uint32_t*)pCtx,SymDriverAdaptorGetCtxSize(pCtx->alg));
			}
			break;
		default: break;
	}
	return rc;
}

/*!
 * Initializes the caller context by invoking the symmetric dispatcher driver. 
 * The caller context may resides in SRAM or DCACHE SEP areas. 
 * This function flow is synchronouse. 
 * 
 * \param pCtx 
 *  
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int SymDriverAdaptorInit(struct sep_ctx_generic *pCtx)
{
	int symRc = DX_RET_OK;

	//DX_PAL_LOG_INFO("pCtx=%p\n", pCtx);
	if (pCtx == NULL) {
		PRINT_INFO("NULL pointer was given for ctx\n");
		symRc = DX_RET_INVARG_CTX;
		goto EndWithErr;
	}
	
	DX_SET_AXIM_VAL();
	symRc = symDriverAdaptorCopyCtx(dx_driver_adaptor_in, (uint32_t)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR, pCtx);
	if (symRc != DX_RET_OK) {
		goto EndWithErr;
	}

	/* call the dispatcher with the new context pointer in SRAM */
	symRc = SymDriverDispatchInit((struct sep_ctx_generic *)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR);
	if (symRc != DX_RET_OK) {
		goto EndWithErr;
	}
	WaitForSequenceCompletion();

	symRc = symDriverAdaptorCopyCtx(dx_driver_adaptor_out, (uint32_t)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR, pCtx );

EndWithErr:
	return symRc;
}

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
				void *pDataIn, void *pDataOut, uint32_t DataSize)
{
	int 			symRc = DX_RET_OK;
	DmaBuffer_s 		dmaBuffIn;
	DmaBuffer_s		dmaBuffOut;
	uint32_t                retCode;    
	struct sep_ctx_cipher 	*pAesContext = (struct sep_ctx_cipher *)pCtx;
	struct sep_ctx_combined *pCombinedCtx =(struct sep_ctx_combined *)pCtx;
	/* used to differ AES MAC modes (where the dout is not NULL, but is not access via DMA */
	uint32_t 		isMac = DX_FALSE;
	uint32_t		dmaBuiltFlag = DMA_BUILT_FLAG_NONE;

	//DX_PAL_LOG_INFO("pCtx=%p\n", pCtx);
//	PRINT_INFO("IN addr=%p OUT addr=%p DataSize=%d\n", pDataIn, pDataOut, DataSize);//2012/11/10
	

	if ((pCtx == NULL) || (pDataIn == NULL)) {
		PRINT_INFO("NULL pointer was given for ctx or din\n");//2012/11/10
		return DX_RET_INVARG_CTX;
	}

	/* In AES mac modes there is no output so it needs special treatment */
	if ((pCtx->alg == SEP_CRYPTO_ALG_AES) && 
	    ((pAesContext->mode == SEP_CIPHER_CBC_MAC) || 
	     (pAesContext->mode == SEP_CIPHER_XCBC_MAC)|| 
	     (pAesContext->mode == SEP_CIPHER_CMAC))){
		isMac = DX_TRUE;
		/* clear the output to mark that it is not used */
		pDataOut = DX_NULL;
	}
	if(pCtx->alg == SEP_CRYPTO_ALG_COMBINED) {
		if(pCombinedCtx->mode == SEP_COMBINED_DIN_TO_AES_TO_HASH_MODE) {
			/* clear the output to mark that it is not used */
			pDataOut = DX_NULL;
		}

	}

	// make sure buffers dont overlap
	if ((!isMac) && 
	    (((pDataIn > pDataOut) && (pDataOut + DataSize) >  pDataIn) ||
	    ((pDataIn < pDataOut) && (pDataIn + DataSize) >  pDataOut)) ) {
		PRINT_INFO("pDataIn and pDataOut overlap\n");//2012/11/10
		return DX_RET_INVARG_BAD_ADDR;
	}


	// in case of inplace - map only one buffer bi directional
	if (pDataIn == pDataOut) {
		retCode = buildDmaFromDataPtr((uint8_t *)pDataIn, DataSize, DX_PAL_DMA_DIR_BI_DIRECTION, 
						&dmaBuffIn, &g_dmaInBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDmaFromDataPtr for pDataIn inplace 0x%x\n", retCode);
			return retCode;
		}
		dmaBuiltFlag = DMA_BUILT_FLAG_BI_DIR;
		COPY_DMA_BUFF(dmaBuffOut, dmaBuffIn); 
	}else {
		retCode = buildDmaFromDataPtr((uint8_t *)pDataIn, DataSize, DX_PAL_DMA_DIR_TO_DEVICE, 
						&dmaBuffIn, &g_dmaInBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDmaFromDataPtr for pDataIn 0x%x\n", retCode);
			return retCode;
		}
		dmaBuiltFlag = DMA_BUILT_FLAG_INPUT_BUFF;
		retCode = buildDmaFromDataPtr((uint8_t *)pDataOut, DataSize, DX_PAL_DMA_DIR_FROM_DEVICE, 
						&dmaBuffOut, &g_dmaOutBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDmaFromDataPtr for pDataOut 0x%x\n", retCode);
			symRc = retCode;
			goto EndWithErr;
		}
		dmaBuiltFlag |= DMA_BUILT_FLAG_OUTPUT_BUFF;
	}

	DX_SET_AXIM_VAL();
	symRc = symDriverAdaptorCopyCtx(dx_driver_adaptor_in, (uint32_t)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR, pCtx);
	if (symRc != DX_RET_OK) {
		goto EndWithErr;
	}

	symRc = SymDriverDispatchProcess((struct sep_ctx_generic *)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR, &dmaBuffIn, &dmaBuffOut);
	if (symRc != DX_RET_OK) {
		goto EndWithErr;
	}

	WaitForSequenceCompletion();
	symRc = symDriverAdaptorCopyCtx(dx_driver_adaptor_out, (uint32_t)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR, pCtx);
EndWithErr:
	// in case of inplace - unmap only one buffer bi directional
	if (dmaBuiltFlag & DMA_BUILT_FLAG_BI_DIR) {
		retCode = buildDataPtrFromDma((uint8_t *)pDataIn, DataSize, DX_PAL_DMA_DIR_BI_DIRECTION, 
						&dmaBuffIn, &g_dmaInBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDataPtrFromDma for pDataIn inplace 0x%x\n", retCode);
			symRc = retCode;
		}
	}
	if (dmaBuiltFlag & DMA_BUILT_FLAG_INPUT_BUFF) {
		retCode = buildDataPtrFromDma((uint8_t *)pDataIn, DataSize, DX_PAL_DMA_DIR_TO_DEVICE, 
						&dmaBuffIn, &g_dmaInBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDataPtrFromDma for pDataIn 0x%x\n", retCode);
			symRc = retCode;
		}
	}
	if (dmaBuiltFlag & DMA_BUILT_FLAG_OUTPUT_BUFF) {
		retCode = buildDataPtrFromDma((uint8_t *)pDataOut, DataSize, DX_PAL_DMA_DIR_FROM_DEVICE, 
						&dmaBuffOut, &g_dmaOutBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDataPtrFromDma for pDataOut 0x%x\n", retCode);
			symRc = retCode;
		}
	}

	return symRc;
}

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
				void *pDataIn, void *pDataOut, uint32_t DataSize)
{
	DmaBuffer_s dmaBuffIn, dmaBuffOut;
	int symRc = DX_RET_OK;
	struct sep_ctx_cipher *pAesContext = (struct sep_ctx_cipher *)pCtx;
	struct sep_ctx_combined *pCombinedCtx =(struct sep_ctx_combined *)pCtx;
	uint32_t                retCode;    
	/* used to differ AES MAC modes (where the dout is not NULL, but is not access via DMA */
	uint32_t isMac = DX_FALSE;
	void 		*pTmpDataOut = pDataOut;
	uint32_t	dmaBuiltFlag = DMA_BUILT_FLAG_NONE;

//	PRINT_INFO("pCtx=%p\n", pCtx);
//	PRINT_INFO("IN addr=%p OUT addr=%p DataSize=%u\n", pDataIn, pDataOut, DataSize);

	/* do not check din pointer since hash/hmac algs has no data input */
	if (pCtx == NULL) {
		PRINT_INFO("NULL pointer was given for ctx\n");
		return DX_RET_INVARG_CTX;
	}
	if ((pCtx->alg == SEP_CRYPTO_ALG_AES) && 
	    ((pAesContext->mode == SEP_CIPHER_CBC_MAC) || 
	     (pAesContext->mode == SEP_CIPHER_XCBC_MAC) || 
	     (pAesContext->mode == SEP_CIPHER_CMAC))){
		isMac = DX_TRUE;
		pTmpDataOut = NULL; 
	}

	if(pCtx->alg == SEP_CRYPTO_ALG_COMBINED) {
		if(pCombinedCtx->mode == SEP_COMBINED_DIN_TO_AES_TO_HASH_MODE) {
			/* clear the output to mark that it is not used */
			pTmpDataOut = NULL; 
			pDataOut = DX_NULL;
		}

	}
	// make sure buffers dont overlap
	if ( ((!isMac) && ((pDataIn > pDataOut) && (pDataOut + DataSize) >  pDataIn))||
	     ((isMac) && ((pDataIn > pDataOut) && (pDataOut + SEP_AES_BLOCK_SIZE) >  pDataIn))||
	    ((pDataIn < pDataOut) && (pDataIn + DataSize) >  pDataOut) ) {
		PRINT_INFO("pDataIn and pDataOut overlap\n");
		return DX_RET_INVARG_BAD_ADDR;
	}
	// in case of inplace - map only one buffer bi directional
	if ((pDataIn == pDataOut) && !isMac) {
		retCode = buildDmaFromDataPtr((uint8_t *)pDataIn, DataSize, DX_PAL_DMA_DIR_BI_DIRECTION, 
						&dmaBuffIn, &g_dmaInBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDmaFromDataPtr for pDataIn inplace 0x%x\n", retCode);
			return retCode;
		}
		dmaBuiltFlag = DMA_BUILT_FLAG_BI_DIR;
		COPY_DMA_BUFF(dmaBuffOut, dmaBuffIn); 
	}else {
		retCode = buildDmaFromDataPtr((uint8_t *)pDataIn, DataSize, DX_PAL_DMA_DIR_TO_DEVICE, 
						&dmaBuffIn, &g_dmaInBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDmaFromDataPtr for pDataIn 0x%x\n", retCode);
			return retCode;
		}
		dmaBuiltFlag = DMA_BUILT_FLAG_INPUT_BUFF;
		retCode = buildDmaFromDataPtr((uint8_t *)pTmpDataOut, DataSize, DX_PAL_DMA_DIR_FROM_DEVICE, 
						&dmaBuffOut, &g_dmaOutBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDmaFromDataPtr for pDataOut 0x%x\n", retCode);
			symRc = retCode;
			goto EndWithErr;
		}
		dmaBuiltFlag |= DMA_BUILT_FLAG_OUTPUT_BUFF;
	}

	DX_SET_AXIM_VAL();
	symRc = symDriverAdaptorCopyCtx(dx_driver_adaptor_in, (uint32_t)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR, pCtx);
	if (symRc != DX_RET_OK) {
		goto EndWithErr;
	}

	symRc = SymDriverDispatchFinalize((struct sep_ctx_generic *)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR, &dmaBuffIn, &dmaBuffOut);
	if (symRc != DX_RET_OK) {
		goto EndWithErr;
	}

	WaitForSequenceCompletion();

	symRc = symDriverAdaptorCopyCtx(dx_driver_adaptor_out, (uint32_t)DX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR, pCtx);

EndWithErr:
	// in case of inplace - unmap only one buffer bi directional
	if (dmaBuiltFlag & DMA_BUILT_FLAG_BI_DIR) {
		retCode = buildDataPtrFromDma((uint8_t *)pDataIn, DataSize, DX_PAL_DMA_DIR_BI_DIRECTION, 
						&dmaBuffIn, &g_dmaInBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDataPtrFromDma for pDataIn inplace 0x%x\n", retCode);
			symRc = retCode;
		}
	}
	if  (dmaBuiltFlag & DMA_BUILT_FLAG_INPUT_BUFF) {
		retCode = buildDataPtrFromDma((uint8_t *)pDataIn, DataSize, DX_PAL_DMA_DIR_TO_DEVICE, 
						&dmaBuffIn, &g_dmaInBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDataPtrFromDma for pDataIn 0x%x\n", retCode);
			symRc = retCode;
		}
	}
	if  (dmaBuiltFlag & DMA_BUILT_FLAG_OUTPUT_BUFF) {
		retCode = buildDataPtrFromDma((uint8_t *)pTmpDataOut, DataSize, DX_PAL_DMA_DIR_FROM_DEVICE, 
						&dmaBuffOut, &g_dmaOutBuildBuffH);
		if (retCode != 0) {
			PRINT_INFO("failed to buildDataPtrFromDma for pDataOut 0x%x\n", retCode);
			symRc = retCode;
		}
	}

	if(isMac) {
		switch(pAesContext->mode) {
			case SEP_CIPHER_CBC_MAC:
			case SEP_CIPHER_XCBC_MAC:
			case SEP_CIPHER_CMAC:
				DX_PAL_MemCopy(pDataOut,pAesContext->block_state,SEP_AES_BLOCK_SIZE);
			default:
				break;
		}
	}

	return symRc;
}


