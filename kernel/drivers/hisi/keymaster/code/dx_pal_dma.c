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



/************* Include Files ****************/
/*  modified by lixiaojie,2012/11/5/14:55
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
*/
#include "dx_pal_dma.h"

#include "debug.h"//added 2012/11/10

static 	int fd_mem = -1;
uint32_t gMemVirtBaseAddr = 0;
uint32_t gMemPhysBaseAddr = 0;
uint32_t gMemPoolLen = 0;
static struct gen_pool *mem_pool;

typedef enum {
	DX_PAL_DMA_BUFF_TYPE_PHYS = 0,
	DX_PAL_DMA_BUFF_TYPE_NEW_PHYS = 1,
	DX_PAL_DMA_BUFF_TYPE_MAX,
	DX_PAL_DMA_BUFF_TYPE_RESERVE32 = 0x7FFFFFFF
}DX_PAL_DmaBufType_t;

#define DX_PAL_MAX_COOKIES_NUM	260  // 2*FW_MLLI_TABLE_LEN + 10 reserved
#define DX_PAL_MAX_MAP_HANDLE   10   // in case of MLLI we need 3 for inBuff, 3 for outBuff and 4 reserved

#define PAL_PAGE_SHIFT 12
#define PAL_PAGE_SIZE (1 << PAL_PAGE_SHIFT)
#define PAL_PAGE_MASK (~(PAL_PAGE_SIZE-1))

#define DX_PAL_FALSE 0
#define DX_PAL_TRUE 1

typedef struct {
	uint32_t	      buffSize;
	uint8_t               *pVirtBuffAddr;
    uint8_t               *pPhyBuffAddr;

	DX_PAL_DmaBufType_t   buffType;
	uint8_t	      	      isTaken;
}DX_PAL_IntDmaMapCookies_t;

typedef struct {
	uint32_t			index;
	uint8_t	      	   		isUsed;
	uint32_t      	   		numOfTakenCookies;
	DX_PAL_IntDmaMapCookies_t  	cookeis[DX_PAL_MAX_COOKIES_NUM];
}DX_PAL_IntDmaMapHandle_t;


static DX_PAL_IntDmaMapHandle_t cookiesDB[DX_PAL_MAX_MAP_HANDLE];


static uint32_t DX_PAL_CalcPageSize(uint32_t index, 
			            uint32_t numPages, 
			            uint8_t * pDataBuffer, 
			            uint32_t buffSize, 
			            uint32_t startOffset) 
{
	uint32_t size = 0;

	if (index == 0) {
		if ((PAL_PAGE_SIZE - startOffset) >= buffSize) {
			return buffSize;
		} 
		return (PAL_PAGE_SIZE - startOffset);
	} 

	if (index == (numPages -1)) {
		size = ((uint32_t)(pDataBuffer + buffSize)) & (~PAL_PAGE_MASK);
		if(size == 0x0){
			size = PAL_PAGE_SIZE;
		}
		return size;
	}
	 
	return PAL_PAGE_SIZE;
}


static DX_PAL_IntDmaMapHandle_t * DX_PAL_GetDmaHandle(uint32_t *handle)
{
	uint32_t i;
	for (i=0; i<DX_PAL_MAX_MAP_HANDLE;i++ ) {
		if(cookiesDB[i].isUsed == DX_PAL_FALSE) {
			cookiesDB[i].isUsed = DX_PAL_TRUE;
			*handle = i;
			return &cookiesDB[i];
		}
	}
	return NULL;
}

static  void DX_PAL_FreeDmaHandle(uint32_t handle)
{
	if (handle >= DX_PAL_MAX_MAP_HANDLE) {
		return;
	}
	memset((uint8_t *)&cookiesDB[handle].cookeis, 0, sizeof(cookiesDB[handle].cookeis));
	cookiesDB[handle].isUsed = DX_PAL_FALSE;
}

static DX_PAL_IntDmaMapCookies_t * DX_PAL_GetCookie(uint32_t handle, uint32_t *cookieIdx)
{
	uint32_t i;

	if (handle >= DX_PAL_MAX_MAP_HANDLE) {
		return NULL;
	}
	if (cookiesDB[handle].numOfTakenCookies >= DX_PAL_MAX_COOKIES_NUM) {
		return NULL;
	}
	for (i=0; i<DX_PAL_MAX_COOKIES_NUM;i++ ) {
		if(cookiesDB[handle].cookeis[i].isTaken == DX_PAL_FALSE) {
			cookiesDB[handle].cookeis[i].isTaken = DX_PAL_TRUE;
			cookiesDB[handle].numOfTakenCookies++;
			*cookieIdx = i;
			return &cookiesDB[handle].cookeis[i];
		}
	}
	return NULL;
}

static DX_PAL_IntDmaMapCookies_t* DX_PAL_GetCookieByIndex(uint32_t handle, uint32_t cookieIndex)
{
	if ((handle >= DX_PAL_MAX_MAP_HANDLE) ||
	    (cookieIndex >= DX_PAL_MAX_COOKIES_NUM)) {
		return NULL;
	}
	if(cookiesDB[handle].cookeis[cookieIndex].isTaken == DX_PAL_FALSE) {
		return NULL;
	}
	return &cookiesDB[handle].cookeis[cookieIndex];
}


static uint32_t DX_PAL_ReleaseCookie(uint32_t handle, uint32_t cookieIndex)
{
	if ((handle >= DX_PAL_MAX_MAP_HANDLE) ||
	    (cookieIndex >= DX_PAL_MAX_COOKIES_NUM)) {
		return 1;
	}
	if(cookiesDB[handle].cookeis[cookieIndex].isTaken == DX_PAL_FALSE) {
		return 2;
	}
	if (cookiesDB[handle].numOfTakenCookies < 1) {
		return 3;
	}
	cookiesDB[handle].cookeis[cookieIndex].buffSize = 0;
	cookiesDB[handle].cookeis[cookieIndex].buffType = 0;
	cookiesDB[handle].cookeis[cookieIndex].pVirtBuffAddr = NULL;
	cookiesDB[handle].cookeis[cookieIndex].isTaken = DX_PAL_FALSE;
	cookiesDB[handle].numOfTakenCookies--;
	return 0;
}


static void DX_PAL_InitCookies(void)
{
	uint32_t i = 0;
	memset((uint8_t *)cookiesDB, 0, sizeof(cookiesDB));
	for (i = 0; i<DX_PAL_MAX_MAP_HANDLE ; i++) {
		cookiesDB[i].index = i;
	}
}

uint32_t DX_PAL_DmaInit(uint32_t  buffSize,
                        uint32_t  physBuffAddr)
{
	int ret;

	if(gMemVirtBaseAddr != 0)
	{
		return gMemVirtBaseAddr;
	}
	/*Size 2M*/
	gMemPoolLen = 2 * 1024 * 1024; 
	gMemVirtBaseAddr  = (uint32_t)dma_alloc_coherent(NULL, gMemPoolLen, (dma_addr_t *)&gMemPhysBaseAddr, GFP_KERNEL);
	if(gMemVirtBaseAddr == 0)
	{
		LOGE("allocate memory error ");
		return 0;
	}
	DX_PAL_InitCookies();
	LOGI("current dma map virtual %x \t physics %x\n",gMemVirtBaseAddr,gMemPhysBaseAddr);
	mem_pool = gen_pool_create(0, -1);
	ret = gen_pool_add(mem_pool, gMemVirtBaseAddr,gMemPoolLen, -1);
	if(ret)
	{
		LOGE("gen_pool_add error %d",ret);
	}
	return gMemPhysBaseAddr;
}

/** 
 * @brief   free system resources created in PD_PAL_DmaInit() 
 *         
 * @param[in] buffSize - buffer size in Bytes 
 *  
 * @return void
 */ 	
void DX_PAL_DmaTerminate(void)
{
	if (fd_mem < 0) {
		return;
	}
	dma_free_coherent(NULL, gMemPoolLen, (void *)gMemVirtBaseAddr, gMemPhysBaseAddr);
	gen_pool_destroy(mem_pool);
	fd_mem = -1;
	gMemVirtBaseAddr = 0;
	gMemPhysBaseAddr = 0;
	gMemPoolLen = 0;
	return;
}


/** 
 * @brief   Maps a given buffer of any type. Returns the list of DMA-able blocks that the buffer maps to. 
 *         
 * @param[in] pDataBuffer -  Address of the buffer to map
 * @param[in] buffSize - Buffer size in bytes
 * @param[in] copyDirection - Copy direction of the buffer. Can be TO_DEVICE, FROM_DEVICE or BI_DIRECTION
 * @param[in/out] numOfBlocks - maximum numOfBlocks to fill, as output the actual number
 * @param[out] pDmaBlockList - List of DMA-able blocks that the buffer maps to 
 * @param[out] dmaBuffHandle - A handle to the mapped buffer private resources
 *
 * @return Returns a non-zero value in case of failure
 */ 	
uint32_t DX_PAL_DmaBufferMap(uint8_t                	  *pDataBuffer,
			     uint32_t                     buffSize,
			     DX_PAL_DmaBufferDirection_t  copyDirection,
			     uint32_t                     *pNumOfBlocks,
			     DX_PAL_DmaBlockInfo_t        *pDmaBlockList,
			     DX_PAL_DmaBufferHandle       *dmaBuffHandle)
{
	uint32_t retCode = 0;
	uint32_t index = 0,rIndex=0;
	uint32_t cookie = 0;
	uint32_t dmaHandle = 0;
	DX_PAL_IntDmaMapHandle_t *plDmaHandle = NULL;
	DX_PAL_IntDmaMapCookies_t *plCookeis = NULL;
	uint32_t size = 0;
	uint32_t endPage = 0;
	uint32_t startPage = 0;
	uint32_t startOffset = 0;
	uint32_t numPages = 0;
	uint8_t  *pTmpBuff = pDataBuffer;


	if ((NULL == pNumOfBlocks) ||
	    (NULL == pDmaBlockList) ||
	    (NULL == dmaBuffHandle)) {
		return 1;
	}
	*(uint32_t*)dmaBuffHandle = 0;
	plDmaHandle = DX_PAL_GetDmaHandle((uint32_t*)&dmaHandle);
	if(NULL == plDmaHandle){
		return 2;
	}
	// First check whether pDataBuffer is already mapped to physical contiguous memory defined in DX_PAL_DmaInit()
	if ((pDataBuffer >= (uint8_t *)gMemVirtBaseAddr) &&
	    (pDataBuffer < ((uint8_t *)gMemVirtBaseAddr + gMemPoolLen))) {
		plCookeis = DX_PAL_GetCookie(dmaHandle, (uint32_t*)&cookie);
		if(plCookeis == DX_NULL) {
			DX_PAL_FreeDmaHandle(dmaHandle);
			return 3;
		}
		plCookeis->pVirtBuffAddr = pDataBuffer;
		plCookeis->buffSize = buffSize;
		plCookeis->buffType = DX_PAL_DMA_BUFF_TYPE_PHYS;
		*pNumOfBlocks = 1;
		pDmaBlockList[0].blockPhysAddr = (uint32_t)(pDataBuffer - gMemVirtBaseAddr) + gMemPhysBaseAddr;
		pDmaBlockList[0].blockSize = buffSize; 
		*(uint32_t**)dmaBuffHandle = &plDmaHandle->index;
		LOGI("dma mmap buffer %x phyAddr %x virtual %x",pDataBuffer,pDmaBlockList[0].blockPhysAddr,plCookeis->pVirtBuffAddr);
		return 0;

	}

	// calculate number of blocks(pages) held by pDataBuffer
        endPage = (uint32_t)((pDataBuffer + buffSize) - 1) >> PAL_PAGE_SHIFT;
        startPage = ((uint32_t)pDataBuffer) >> PAL_PAGE_SHIFT;
        numPages = endPage - startPage + 1; 
	if ((0 == numPages) || (numPages > *pNumOfBlocks)) {
		DX_PAL_FreeDmaHandle(dmaHandle);
		return 4;
	}

	startOffset = (uint32_t)pDataBuffer & (~PAL_PAGE_MASK);
	*pNumOfBlocks = numPages;
	pTmpBuff = pDataBuffer;

	// fill rest of the pages in array 
	for (index = 0; index < numPages; index++) {
		size = DX_PAL_CalcPageSize(index, numPages, pDataBuffer, buffSize, startOffset);
		LOGI("here calcure page size return %d",size);
		// get block's cookie
		plCookeis = DX_PAL_GetCookie(dmaHandle, (uint32_t*)&cookie);
		LOGI("after getcookie");
		if(plCookeis == DX_NULL) {
			/* release all the allocated memories and cookies */
			for(rIndex = 0; rIndex < index ; rIndex++) {
				plCookeis = DX_PAL_GetCookieByIndex(dmaHandle, rIndex);
				DX_PAL_DmaContigBufferFree(plCookeis->buffSize, 
							   (plCookeis->pVirtBuffAddr));
				DX_PAL_ReleaseCookie(dmaHandle, rIndex);
			}
			DX_PAL_FreeDmaHandle(dmaHandle);
			return 5;
		}
		plCookeis->buffSize = size;
		// if we got here pDataBuffer is not mapped to physical contiguous memory, 
		// so we have to allocate it with in pool and copy buffer according to copyDirection
		retCode = DX_PAL_DmaContigBufferAllocate(plCookeis->buffSize, 
							&(plCookeis->pVirtBuffAddr));
		if (retCode != 0) {
			/* release all the allocated memories and cookies */
			for(rIndex = 0; rIndex < index ; rIndex++) {
				plCookeis = DX_PAL_GetCookieByIndex(dmaHandle, rIndex);
				DX_PAL_DmaContigBufferFree(plCookeis->buffSize, 
							   (plCookeis->pVirtBuffAddr));
				DX_PAL_ReleaseCookie(dmaHandle, rIndex);
			}
			DX_PAL_ReleaseCookie(dmaHandle, cookie);
			DX_PAL_FreeDmaHandle(dmaHandle);
			LOGI("return retcode %d",retCode);
			return retCode;
		}

		plCookeis->buffType = DX_PAL_DMA_BUFF_TYPE_NEW_PHYS;
		pDmaBlockList[index].blockSize = plCookeis->buffSize;
		pDmaBlockList[index].blockPhysAddr = ((uint32_t)(plCookeis->pVirtBuffAddr)) - gMemVirtBaseAddr + gMemPhysBaseAddr;
		// now copy according to copy direction
		if ((DX_PAL_DMA_DIR_TO_DEVICE == copyDirection) ||
		    (DX_PAL_DMA_DIR_BI_DIRECTION == copyDirection)) {
			memcpy((uint8_t *)(plCookeis->pVirtBuffAddr), 
				(uint8_t *)pTmpBuff, 
				plCookeis->buffSize);
		}

		pTmpBuff += pDmaBlockList[index].blockSize;
	}

	*(uint32_t**)dmaBuffHandle = &plDmaHandle->index;
	LOGI("return 0");
	LOGI("dma mmap return 0 phyAddr %x \tbuffer %xvirtual %x",pDmaBlockList[0].blockPhysAddr,
					pDataBuffer, plCookeis->pVirtBuffAddr);
	return 0;
}
 
/** 
 * @brief   Unmaps a given buffer, and frees its associated resources, if exist
 *         
 * @param[in] pDataBuffer -  Address of the buffer to map
 * @param[in] buffSize - Buffer size in bytes
 * @param[in] copyDirection - Copy direction of the buffer. Can be TO_DEVICE, FROM_DEVICE or BI_DIRECTION
 * @param[in] numOfBlocks - Number of DMA-able blocks that the buffer maps to
 * @param[in] pDmaBlockList - List of DMA-able blocks that the buffer maps to 
 * @param[in] dmaBuffHandle - A handle to the mapped buffer private resources
 *
 * @return Returns a non-zero value in case of failure
 */ 	
uint32_t DX_PAL_DmaBufferUnmap(uint8_t                	    *pDataBuffer,
			       uint32_t                     buffSize,
			       DX_PAL_DmaBufferDirection_t  copyDirection,
			       uint32_t                     numOfBlocks,
			       DX_PAL_DmaBlockInfo_t        *pDmaBlockList,
			       DX_PAL_DmaBufferHandle       dmaBuffHandle)
{
	uint32_t retCode = 0;
	uint32_t index = 0;
	uint8_t  *pTmpBuff;
	uint32_t dmaHandle = 0;
	DX_PAL_IntDmaMapCookies_t *plCookeis = NULL;

	pTmpBuff = pDataBuffer;

	if ((NULL == pDmaBlockList) ||
	    (NULL == dmaBuffHandle)) {
		return 1;
	}
	dmaHandle = *(uint32_t*)dmaBuffHandle;
	if (dmaHandle > DX_PAL_MAX_MAP_HANDLE) {
		return 1;
	}
	// free resources 
	for (index = 0; index < numOfBlocks; index++) {
		plCookeis = DX_PAL_GetCookieByIndex(dmaHandle, index);
		if(plCookeis == DX_NULL) {
			/* Although this is problem we can't stop as we must clear all cookies*/
			retCode = 2;
			continue;
		}
		// First make sure pCookies-> virtBuffAddr is mapped to physical contiguous memory. 
		// Otherwise return error.
		if(((plCookeis->pVirtBuffAddr) < (uint8_t *)gMemVirtBaseAddr) ||
		   ((plCookeis->pVirtBuffAddr) >= ((uint8_t *)(gMemVirtBaseAddr +gMemPoolLen)))) {
			/* Although we can't handle this memory (as it is not part of the PAL region )*/
			/* We must proceed with the cookies release */
			DX_PAL_ReleaseCookie(dmaHandle, index);
			retCode = 3;
			continue;
		}

		if (plCookeis->buffType == DX_PAL_DMA_BUFF_TYPE_NEW_PHYS)  {
			// in that case have to copy buffer according to copyDirection and free allocated buffer 
			if ((DX_PAL_DMA_DIR_FROM_DEVICE == copyDirection) ||
			    (DX_PAL_DMA_DIR_BI_DIRECTION == copyDirection)) {
				memcpy((uint8_t *)pTmpBuff, 
					(uint8_t *)(plCookeis->pVirtBuffAddr), 
					plCookeis->buffSize);
			}
			// if we got here pDataBuffer is not mapped to physical contiguous memory, 
			// so we have to allocate it with in pool and copy buffer according to copyDirection
			DX_PAL_DmaContigBufferFree(plCookeis->buffSize, 
						   (plCookeis->pVirtBuffAddr));
		}

		// if buffer was not allocated/copy in DX_PAL_DmaMap(), nothing left to be doen.Just return OK
		plCookeis->buffType = DX_PAL_DMA_BUFF_TYPE_RESERVE32;
		plCookeis->pVirtBuffAddr = NULL;
		pTmpBuff += plCookeis->buffSize;
		DX_PAL_ReleaseCookie(dmaHandle, index);
	}
	/*After releasing all cookies we release the dmaHandle */
	DX_PAL_FreeDmaHandle(dmaHandle);
	return retCode;
}
 
   
   
/** 
 * @brief   Allocates a DMA-contiguous buffer, and returns both its physical and virtual addresses
 *         
 *
 * @param[in] buffSize - Buffer size in bytes
 * @param[out] ppVirtBuffAddr - Virtual address of the allocated buffer
 *
 * @return Returns a non-zero value in case of failure
 */ 	
uint32_t DX_PAL_DmaContigBufferAllocate(uint32_t          buffSize,
					uint8_t          **ppVirtBuffAddr)
{
	*ppVirtBuffAddr = (uint8_t *)gen_pool_alloc(mem_pool, buffSize);
	if (NULL == *ppVirtBuffAddr) {
		return 1;
	}
	LOGI("v_address %x",*ppVirtBuffAddr);
	return 0;
}



/** 
 * @brief   free resources previuosly allocated by DX_PAL_DmaContigBufferAllocate 
 *         
 *
 * @param[in] buffSize - buffer size in Bytes
 * @param[in] pVirtBuffAddr - virtual address of the buffer to free
 *
 * @return success/fail
 */ 	
uint32_t DX_PAL_DmaContigBufferFree(uint32_t          buffSize,
				    uint8_t          *pVirtBuffAddr)
{
	if(pVirtBuffAddr == NULL)
		return -1;
	gen_pool_free(mem_pool, (unsigned long) pVirtBuffAddr, buffSize);
	return 0;
}


    
/** 
 * @brief   release and free previously allocated buffers 
 *         
 * @param[in] pDataBuffer - User buffer address
 * @param[in] buffSize - User buffer size
 *
 * @return Returns TRUE if the buffer is guaranteed to be a single contiguous DMA block, and FALSE otherwise.
 */ 	
uint32_t DX_PAL_IsDmaBufferContiguous(uint8_t       *pDataBuffer,
				      uint32_t       buffSize)
{
	return 0; // false indication
}
