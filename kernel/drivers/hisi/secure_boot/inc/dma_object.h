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

#ifndef __DMA_OBJECT_H__
#define __DMA_OBJECT_H__
/*
 * This header file defines the API for the DMA object. 
 * The DMA object is used by SeP OEM-FW and applets in order to provide 
 * CRYS API with reference to data buffers in host memory. 
 */

/* Object container size */
#define DX_DMA_OBJ_CONTAINER_SIZE 24
#define DX_DMA_OBJ_CONTAINER_SIZE_WORDS (DX_DMA_OBJ_CONTAINER_SIZE >> 2)

/* The DMA object handle */
typedef uint32_t DX_DmaObjHandle_t;
#define DX_DMA_OBJ_INVALID_HANDLE 0

/* The DMA object container - to be allocated by caller */
typedef uint32_t DX_DmaObjContainer_t[DX_DMA_OBJ_CONTAINER_SIZE_WORDS];

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
	uint32_t dataAddr, uint32_t dataSize);

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
	uint32_t dataAddr, uint32_t dataSize);

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
	unsigned int nTables);

/*!
 * Free resources of given DMA object. 
 * This function must be invoked before freeing or reusing the object container 
 * 
 * \param objHandle The DMA object handle
 */
void DX_DmaObjDestroy(DX_DmaObjHandle_t objHandle);

/*!
 * Set DMA object as a secure AXI transaction 
 * This function has no effect for SEP DMA objects 
 * 
 * \param objHandle DMA object handle
 */
void DX_DmaObjAxiSetSecure(DX_DmaObjHandle_t objHandle);

/*!
 * Set DMA object as a non-secure AXI transaction 
 * This function has no effect for SEP DMA objects  
 * 
 * \param objHandle DMA object handle
 */
void DX_DmaObjAxiSetNonSecure(DX_DmaObjHandle_t objHandle);

#endif /*__DMA_OBJECT_H__*/
