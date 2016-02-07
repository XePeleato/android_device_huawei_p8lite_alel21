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

#include "dx_pal_log.h"
#include "dma_buffer.h"
#include "hw_queue_defs.h"
#include "dx_macros.h"


#if (DX_DSCRPTR_QUEUE0_WORD3_NS_BIT_BIT_SHIFT != DX_DSCRPTR_QUEUE0_WORD1_NS_BIT_BIT_SHIFT) || (DX_DSCRPTR_QUEUE0_WORD1_DIN_VIRTUAL_HOST_BIT_SHIFT != DX_DSCRPTR_QUEUE0_WORD3_DOUT_VIRTUAL_HOST_BIT_SHIFT)
#error AxiId/NS-bit fields mismatch between DIN and DOUT - functions need to be updated...
#endif 

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
	unsigned int *nTables)
{
	DmaBuffer_s *dmaBuffer;

	/* default not-secure bit */
	*pDmaAxiNs = AXI_NOT_SECURE;
	/* Parse input data buffer details */
	dmaBuffer = PTR_TO_DMA_BUFFER(dataPtr);

	return 0;
}



/*!
 * validate DMA object structure 
 *  
 * \param dmaObj the DMA object  
 * \param dataSize Data size given by the user (relevent only for SRAM data)
 *  
 * \return 0 on success, (-1) if ( dataSize != dma object data size), (-2) if sram pointer is out of range
 */

int validateDmaBuffer(DmaBuffer_s *dmaObj, uint32_t dataSize)
	
{
	if ((dmaObj->dmaBufType != DMA_BUF_DLLI) || (dmaObj->size != dataSize))
		return -1; /* data size mismatch */
		
	return 0;
}


