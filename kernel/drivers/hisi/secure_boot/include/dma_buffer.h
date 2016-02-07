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

#ifndef __DMA_BUFFER_H__
#define __DMA_BUFFER_H__
//modified by lixiaojie,2012/11/6
//#include <stdint.h>
#include "dma_buffer_plat.h"
//added by lixiaojie,2012/11/6
#include "dx_pal_types_plat.h"
/* Get the DmaMode_t to match DMA buffer type */
#define DMA_BUF_TYPE_TO_MODE(dmaBufType) \
        (((dmaBufType) == DMA_BUF_NULL) ? DMA_MODE_NULL :       \
	 ((dmaBufType) == DMA_BUF_SEP) ? DMA_SRAM :       \
	 ((dmaBufType) == DMA_BUF_DLLI) ? DMA_DLLI : DMA_MLLI)

/* DMA buffer type */
typedef enum DmaBufType {
	DMA_BUF_NULL = 0,
	DMA_BUF_SEP,
	DMA_BUF_DLLI,
	DMA_BUF_MLLI_IN_SEP,
	DMA_BUF_MLLI_IN_HOST
} DmaBufType_t;

typedef struct DmaBuffer {
	DmaBufType_t dmaBufType;
	uint32_t pData;           /* A pointer to the data (DMA_SRAM/DLLI) or MLLI table (DMA_MLLI_*) */
	uint32_t size;            /* The size of the data (DMA_SRAM/DLLI) or size of first MLLI table (DMA_MLLI_*) */
	unsigned int nTables;     /* Number of MLLI tables. Ignored for DMA_SEP/DLLI. */
	uint8_t axiNs;            /* AXI NS bit */
} DmaBuffer_s;

/*!
 * Parse user buffer information that may be smart-pointer (DMA object/buffer) 
 * Return uniform DMA information 
 *  
 * \param dataPtr Pointer given by the user 
 * \param dataSize Data size given by the user (relevant for non-smart-ptr)
 * \param pDmaType 
 * \param pDmaAddr 
 * \param pDmaSize 
 * \param pDmaAxiNs The AXI Secure bit
 * \param nTables The number of MLLI tables. Ignored in SEP or DLLI types.
 *  
 * \return 0 on success, !0 if parameter are invalid (e.g., dataSize != dma object data size) 
 */
int dataPtrToDma(
	uint8_t *dataPtr,        
	uint32_t dataSize,         
	DmaBufType_t *pDmaType,
	uint32_t *pDmaAddr, 
	uint32_t *pDmaSize,
	uint32_t *pDmaAxiNs,
	unsigned int *nTables);

/*!
 * validate DMA object structure 
 *  
 * \param dmaObj the DMA object  
 * \param dataSize Data size given by the user (relevent only for SRAM data)
 *  
 * \return 0 on success, (-1) if ( dataSize != dma object data size), (-2) if sram pointer is out of range
 */
int validateDmaBuffer(DmaBuffer_s *dmaObj, uint32_t dataSize);

#endif /*__DMA_BUFFER_H__*/
