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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_INFRA

/* Implementation of the DMA object */
#include "dma_buffer.h"
#include "dma_object.h"
#include "dx_macros.h"
#include "compiler.h"
#include "hw_queue_defs.h"


DX_PAL_COMPILER_ASSERT(sizeof(DmaBuffer_s) <= sizeof(DX_DmaObjContainer_t), "Container size is too small");

#define HANDLE_TO_OBJ(ptr)  PTR_TO_DMA_BUFFER(ptr)
#define OBJ_TO_HANDLE(pObj) ((DX_DmaObjHandle_t)DMA_BUFFER_TO_PTR(pObj))

/*!
 * Create a DMA object for data buffer in SeP memory (SRAM)
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \param dataAddr SRAM address
 * \param dataSize Data size in bytes
 * 
 * \return DX_DmaObjHandle_t The created object handle
 */
DX_DmaObjHandle_t DX_DmaObjCreateSepBuffer(
	DX_DmaObjContainer_t container,
	uint32_t dataAddr, uint32_t dataSize)
{
	DmaBuffer_s *newObj = (DmaBuffer_s *)container;

	newObj->dmaBufType = DMA_BUF_SEP;
	newObj->axiNs = DEFALUT_AXI_SECURITY_MODE;
	newObj->pData = dataAddr;
	newObj->size = dataSize;
	newObj->nTables = 0;

	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Create a DMA object for data buffer in host memory
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \param dataAddr DMA address of host memory buffer
 * \param dataSize Data size in bytes
 * 
 * \return DX_DmaObjHandle_t The created object handle
 */
DX_DmaObjHandle_t DX_DmaObjCreateHostBuffer(
	DX_DmaObjContainer_t container,
	uint32_t dataAddr, uint32_t dataSize)
{
	DmaBuffer_s *newObj = (DmaBuffer_s *)container;

	newObj->dmaBufType = DMA_BUF_DLLI;
	newObj->axiNs = DEFALUT_AXI_SECURITY_MODE;
	newObj->pData = dataAddr;
	newObj->size = dataSize;
	newObj->nTables = 0;

	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Create a DMA object for data given in MLLI tables list
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \param mlliAddr DMA address of (first) MLLI table
 * \param mlliSize Size in bytes of (first) MLLI table
 * \param nTables Number of MLLI tables in MLLI tables list
 * 
 * \return DX_DmaObjHandle_t The created object handle
 */
DX_DmaObjHandle_t DX_DmaObjCreateHostMlliBuffer(
	DX_DmaObjContainer_t container,
	uint32_t mlliAddr,
	uint32_t mlliSize,
	unsigned int nTables)
{
	DmaBuffer_s *newObj = (DmaBuffer_s *)container;

	newObj->dmaBufType = DMA_BUF_MLLI_IN_HOST;
	newObj->axiNs = DEFALUT_AXI_SECURITY_MODE;
	newObj->pData = mlliAddr;
	newObj->size = mlliSize;
	newObj->nTables = nTables;

	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Free resources of given DMA object. 
 * This function must be invoked before freeing or reusing the object container 
 * 
 * \param objHandle The DMA object handle
 */
void DX_DmaObjDestroy(DX_DmaObjHandle_t objHandle)
{
	/* Nothing to do today - just a place holder */
}

/*!
 * Set DMA object as a secure AXI transaction 
 * This function has no effect for SEP DMA objects 
 * 
 * \param objHandle DMA object handle
 */
void DX_DmaObjAxiSetSecure(DX_DmaObjHandle_t objHandle)
{
	DmaBuffer_s *thisObj = HANDLE_TO_OBJ(objHandle);

	thisObj->axiNs = AXI_SECURE;
}

/*!
 * Set DMA object as a non-secure AXI transaction 
 * This function has no effect for SEP DMA objects  
 * 
 * \param objHandle DMA object handle
 */
void DX_DmaObjAxiSetNonSecure(DX_DmaObjHandle_t objHandle)
{
	DmaBuffer_s *thisObj = HANDLE_TO_OBJ(objHandle);

	thisObj->axiNs = AXI_NOT_SECURE;
}

