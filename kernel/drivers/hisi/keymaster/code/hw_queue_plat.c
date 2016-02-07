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
#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_HW_QUEUE

#include "dx_pal_types.h"
#include "dx_pal_log.h"
#include "hw_queue_defs.h"
#include "dx_error.h"
#include "dx_hal.h"

/******************************************************************************
*				FUNCTIONS
******************************************************************************/


/* Counts all AXI write descriptor (with LAST bit set) since last completion request */
uint32_t gAxiWriteCount[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;

/*!
 * HW queue init.
 * 
 * \param qid Should always be "0".
 * 
 * \return int
 */
int InitHwQueuePlat(int qid)
{
	if (qid != NO_OS_QUEUE_ID) {
		return DX_RET_INVARG_QID;
	}
	return DX_RET_OK;
}

/*!
 * Waits until the HW queue Water Mark is signaled.
 */
void WaitForHwQueueWaterMark(void)
{
	uint32_t lazyWait = 0;
	uint32_t irr;
	uint32_t data = 0;

	/* wait for watermark signal */
	irr = DX_HAL_WaitInterrupt(data);
	/* clear interrupt */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_ICR), irr);

	/* check if this is watermark signal */
	while (!HW_QUEUE_NO_OS_IS_QUEUE_WATERMARK(irr)) {
		/* completion may be on its way... */
		if (lazyWait++ > 100) {
			DX_PAL_Abort("WaterMark failed to arrive for too long");
		}
		irr = DX_HAL_WaitInterrupt(data);
		/* clear interrupt */
		DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_ICR), irr);
	}
}

