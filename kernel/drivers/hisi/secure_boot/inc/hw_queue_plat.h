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

#ifndef  HW_QUEUE_PLAT_H
#define  HW_QUEUE_PLAT_H

#include "compiler.h"
#include "dx_hal.h"




/******************************************************************************
*				MACROS
******************************************************************************/
/* PLAT CC44 */
#define DEFAULT_AXI_ID 0 				/* Virtual Host */
#define DEFALUT_AXI_SECURITY_MODE AXI_SECURE		/* NS bit */
#define HW_DESC_STATE_LOCATION DMA_BUF_DLLI	


#define HW_QUEUE_NO_OS_IS_DESC_COMPLETED(irr) \
	DX_CC_REG_FLD_GET(HOST_RGF, HOST_RGF_IRR, DSCRPTR_COMPLETION_MASK, (irr))

#define HW_QUEUE_NO_OS_IS_QUEUE_WATERMARK(irr) \
	DX_CC_REG_FLD_GET(HOST_RGF, HOST_RGF_IRR, DSCRPTR_WATERMARK_INT, (irr))


/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * This function initializes the HW queue water mark semaphore event.
 * 
 * \param qid The hw queue ID.
 * 
 * \return int one of the error codes defined in err.h
 */
int InitHwQueuePlat(int qid) DX_ICACHE_FUNCTION;

/*!
 * Waits until the HW queue Water Mark is signaled.
 * 
 * \param qid An ID of a HW queue according to the queue priority. "1" is the lowest priority.
 */
void WaitForHwQueueWaterMark(void);


/*!
 * This function sets the DIN field of a HW descriptors to DLLI mode. 
 * The AXI and NS bits are set, hard coded to zero. this asiengment is
 * for TEE only. for PEE TBD to set the AXI and NS bits to 1.
 *
 *
 * \param pDesc pointer HW descriptor struct
 * \param dinAdr DIN address
 * \param dinSize Data size in bytes 
 */
#define HW_DESC_SET_STATE_DIN_PARAM(pDesc, dinAdr, dinSize)				\
	do {		                                                       	\
		HW_DESC_SET_DIN_SRAM(pDesc, dinAdr, dinSize);			\
	} while (0)
#define HW_DESC_SET_STATE_DOUT_PARAM(pDesc, doutAdr, doutSize)				\
	do {		                                                               	\
		HW_DESC_SET_DOUT_SRAM(pDesc, doutAdr, doutSize);			\
	} while (0)

/* No HW queue sequencer is needed */
#define _HW_QUEUE_LOCK()
#define _HW_QUEUE_UNLOCK()

/*!
 * This function sets the DIN field of a HW descriptors to DLLI mode. 
 * The AXI and NS bits are set, hard coded to zero. this asiengment is
 * for TEE only. for PEE TBD to set the AXI and NS bits to 1.
 *
 *
 * \param pDesc pointer HW descriptor struct
 * \param dinAdr DIN address
 * \param dinSize Data size in bytes 
 */
#define HW_DESC_SET_SPECIAL_DIN_PARAM(pDesc, dinAdr, dinSize)										\
	do {		                                                                                        		\
		/* use QUEUE0 since the offsets in each QUEUE does not depend on the QID */					\
		DX_CC_REG_FLD_SET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD0, VALUE, (pDesc)->word[0], ((uint32_t)(dinAdr)));		\
		DX_CC_REG_FLD_SET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD1, DIN_DMA_MODE, (pDesc)->word[1], NO_DMA);			\
		DX_CC_REG_FLD_SET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD1, DIN_SIZE, (pDesc)->word[1], (dinSize));			\
		DX_CC_REG_FLD_SET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD1, DIN_VIRTUAL_HOST, (pDesc)->word[1], (DEFAULT_AXI_ID));	\
		DX_CC_REG_FLD_SET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD1, NS_BIT, (pDesc)->word[1], (DEFALUT_AXI_SECURITY_MODE));	\
	} while (0)


#endif /*HW_QUEUE_PLAT_H*/
