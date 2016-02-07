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
#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_COMPLETION

#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "dx_pal_abort.h"
#include "dx_error.h"
#include "dx_pal_log.h"
#include "cc_plat.h"
#include "bsp.h"
#include "compiler.h"
#include "completion.h"
#include "hw_queue.h"
#include "dx_hal.h"
#include "debug.h"


/******************************************************************************
*				GLOBALS
******************************************************************************/
static uint8_t gCompletionFifo[MAX_NUM_HW_QUEUES][COMPLETION_FIFO_LEN] DX_SRAM_SBSS_VARIABLE;

/* Items are inserted to head and removed from tail */
static uint32_t gCompletionFifoHead[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;
static uint32_t gCompletionFifoTail[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;
static uint32_t gAxiWriteCompleted[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;

static uint32_t gCompletionCount[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;
static CompletionType_t gCompletionCtx[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;
static uint32_t gTaskId[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;
static uint32_t gExpectedAxiWrites[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;

/******************************************************************************
*			FUNCTIONS PROTOTYPES
******************************************************************************/

/* The interrupt handlers are naked functions that call C handlers. The C
   handlers are marked as noinline to ensure they work correctly when the
   optimiser is on. */
static void CompletionDescHandler(void)		;


/******************************************************************************
*				FUNCTIONS
******************************************************************************/

static void CompletionDescHandler()
{
	int qid;
	uint32_t status;
	status = 0;
	
	/* Init to lowest prioirty */
	qid = 0;
	//PRINT_INFO("--->now into CompletionDescHandler()\n");
	/* Mask only one counter per queue */
	while ( (status |= ( DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL,DSCRPTR_COMPLETION_STATUS)) & BITMASK(MAX_COMPLETION_COUNTERS))) ) {
			     
				
		/* Loop will start at highest prioirty */
		qid = (qid + 1 ) & (MAX_COMPLETION_COUNTERS-1);
		
		if (status & BITMASK_AT(1, qid)) {
			
			/* Read completion counter register. Counter is cleared once we read it ! */
			gCompletionCount[qid] +=  DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_COMPLETION_COUNTER0) +
								qid*sizeof(uint32_t));
			
			/* status still set, but engine completion counter was already handled */
			if ( gCompletionCount[qid] == 0 ) {
				status = status & ~(BITMASK_AT(1, qid));
				continue;
			}
			
			/* Get completion info from fifo, if not already done so */
			if ( gCompletionCtx[qid] == COMPLETION_INVALID ) {
				CompletionFifoRemove(qid, &gTaskId[qid], &gCompletionCtx[qid], &gExpectedAxiWrites[qid]);
			}
			
			/* Actual bus values of AXI IDs for queues (0-3) DMA are 8, 9, A, B */
			gAxiWriteCompleted[qid] += DX_CC_REG_FLD_GET(CRY_KERNEL, AXIM_MON_COMP8, VALUE, 
									 DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_MON_COMP8) +
									QID_TO_AXI_ID(qid)*sizeof(uint32_t)));
			
			if ( gAxiWriteCompleted[qid] >= gExpectedAxiWrites[qid] ) {
				gAxiWriteCompleted[qid] -= gExpectedAxiWrites[qid];
				gCompletionCtx[qid] = COMPLETION_INVALID;
				gCompletionCount[qid]--;
			}
	
		} /* if (status & BITMASK_AT(1, qid)) */
	
	} /* while ( (status = ( READ_REGISTER... */

}

/*!
 * This function waits for current descriptor sequence completion.
 */
void WaitForSequenceCompletionPlat(void)
{
	int qid = CURR_QUEUE_ID();
	uint32_t taskId = CURR_TASK_ID();
	uint32_t lazyWait = 0;
	uint32_t irr = 0;
	uint32_t data = 0;

	/* Acknowledge completion to host */
	AddCompletionDesc(qid, taskId, (void *)COMPLETION_EXTERNAL);

	/* wait for descriptor completion signal */
	irr = DX_HAL_WaitInterrupt(data);
	/* clear interrupt */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_ICR), irr);

	/* check for compeltion */
	while (!HW_QUEUE_NO_OS_IS_DESC_COMPLETED(irr)) {
		/* completion may be on its way... */
		if (lazyWait++ > 100) {
			DX_PAL_Abort("Completion failed to arrive for too long");
		}
		irr = DX_HAL_WaitInterrupt(data);
		/* clear interrupt */
		DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_ICR), irr);
	}
	CompletionDescHandler();
	
	//DX_PAL_LOG_INFO("Sequence completed\n");
	//PRINT_INFO("Sequence completed\n");
}

/*!
 * This function initializes the completion counter event, clears the
 * state structure and sets completion counter "0" as the first available
 * counter to be used when calling "AllocCounter".
 * 
 * \return int one of the error codes defined in err.h
 */
int InitCompletionPlat(void)
{
	uint8_t dummy;
	int qid;

	/* Clear completion fifo */
	for ( qid=0; qid < MAX_NUM_HW_QUEUES; qid++ ) {
		/* Clear FIFO head/tail */
		gCompletionFifoHead[qid] = 0;
		gCompletionFifoTail[qid] = 0;

		/* The global AXI write counter is reset only once */
		gAxiWriteCompleted[qid] = 0;

		gCompletionCount[qid] = 0;
		gExpectedAxiWrites[qid] = 0;
		gCompletionCtx[qid] = COMPLETION_INVALID;

		DX_PAL_MemSetZero( gCompletionFifo, MAX_NUM_HW_QUEUES*COMPLETION_FIFO_LEN );
		
		/* Clear all past AXI completion counters */
		/* Actual bus values of AXI IDs for queues (0-3) DMA are 8, 9, A, B */
		dummy = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_MON_COMP8) + QID_TO_AXI_ID(qid)*sizeof(uint32_t));
		DX_PAL_LOG_DEBUG("Initial AXI_MON_COMP%d value=0x%08X\n", (int)qid, (unsigned int)dummy);
		dummy = dummy; /*avoid compiler warning*/
	}
	
	return DX_RET_OK;
}


/*!
 * This function adds a completion report to the completion fifo.
 *
 * \param qid The queue id.
 * \param taskId Completion task ID as set by vTaskSetCurrentTaskTag().
 * \param completionType COMPLETION_INTERNAL or COMPLETION_EXTERNAL.
 * \param axiWriteCount AXI transactions counter from previous completion report.
 */
void CompletionFifoInsert(int qid, uint32_t taskId, CompletionType_t *completionType, uint32_t axiWriteCount)
{
	uint8_t	fifoItem = 0;
	uint32_t fifoIdx;
	
	if (((CompletionType_t)completionType != COMPLETION_INTERNAL) &&
	    ((CompletionType_t)completionType != COMPLETION_EXTERNAL)) {
		DX_PAL_Abort("Bad completionType");
	}
	if (taskId >= BITMASK(FIFO_TASK_ID_BIT_SIZE)) {
		DX_PAL_Abort("Bad completionTaskId");
	}
	if (axiWriteCount >= BITMASK(FIFO_AXI_COUNTER_BIT_SIZE)) {
		DX_PAL_Abort("Bad axiWriteCount");
	}
	if ((gCompletionFifoHead[qid] - gCompletionFifoTail[qid]) >= COMPLETION_FIFO_LEN) {
		DX_PAL_Abort("Completion FIFO overflow");
	}
	
	SET_FIFO_COMPLETION_TYPE(fifoItem, (CompletionType_t)completionType);
	SET_FIFO_AXI_COUNTER(fifoItem, axiWriteCount);
	SET_FIFO_COMPLETION_TASK_ID(fifoItem, taskId);
	fifoIdx = gCompletionFifoHead[qid] & (COMPLETION_FIFO_LEN - 1);
	gCompletionFifo[qid][fifoIdx] = fifoItem;
	
	gCompletionFifoHead[qid]++;
			
	DX_PAL_LOG_DEBUG("qid=%d taskId=%d fifoIdx=%d gCompletionFifoHead[qid]=%d completionType=%s axiWriteCount=%d\n", 
		(int)qid, (int)taskId, (int)fifoIdx, (int)gCompletionFifoHead[qid],
		((CompletionType_t)completionType==COMPLETION_INTERNAL)?"INTERNAL":"EXTERNAL", (int)axiWriteCount);
}

/*!
 * This function remove a completion report to the completion fifo.
 * 
 * \param qid The queue id.
 * \param taskId Task ID to be signaled upon completion.
 * \param completionType COMPLETION_INTERNAL or COMPLETION_EXTERNAL.
 * \param axiWriteCount AXI transactions counter from previous completion report.
 */
void CompletionFifoRemove(int qid, uint32_t *taskId, CompletionType_t *completionType, uint32_t *axiWriteCount)
{
	uint8_t	fifoItem = 0;
	uint32_t fifoIdx;
	//PRINT_INFO("@@@@@@@@@@@@@now into CompletionFifoRemove\n");
	/* Check for fifo empty */
	if (gCompletionFifoHead[qid] == gCompletionFifoTail[qid]) {
		//DX_PAL_Abort("Completion FIFO empty");
		PRINT_INFO("Completion FIFO empty");
		return;//2012/11/13/lixiaojie
	}
	
	fifoIdx = gCompletionFifoTail[qid] & (COMPLETION_FIFO_LEN - 1);
	fifoItem = gCompletionFifo[qid][fifoIdx];
	
	*completionType = GET_FIFO_COMPLETION_TYPE(fifoItem);
	*axiWriteCount = GET_FIFO_AXI_COUNTER(fifoItem);
	*taskId = GET_FIFO_COMPLETION_TASK_ID(fifoItem);
	
	/* Note: we focibly comment out these lines since in CC54 project the FIFO mechanism runs from interrupt service
	 * routine while in other projects it runs from a task. Enabling this print will cause the CC54 SeP to stack in debug 
	 * mode. */
	//DX_PAL_LOG_DEBUG("qid=%d taskId=%d fifoIdx=%d gCompletionFifoTail[qid]=%d completionType=%s axiWriteCount=%d\n", 
	//	(int)qid, (int)*taskId, (int)fifoIdx, (int)gCompletionFifoTail[qid],
	//	(*completionType==COMPLETION_INTERNAL)?"INTERNAL":"EXTERNAL", (int)*axiWriteCount);

	gCompletionFifoTail[qid]++;

	if ((*completionType != COMPLETION_INTERNAL) && (*completionType != COMPLETION_EXTERNAL)) {
		//DX_PAL_Abort("Bad completionType");
		PRINT_INFO("Bad completionType");
		return;//
	}
	if (*taskId >= BITMASK(FIFO_TASK_ID_BIT_SIZE)) {
		//DX_PAL_Abort("Bad completionTaskId");
		PRINT_INFO("Bad completionTaskId");
		return;//
	}
	if (*axiWriteCount >= BITMASK(FIFO_AXI_COUNTER_BIT_SIZE)) {
		//DX_PAL_Abort("Bad axiWriteCount");
		PRINT_INFO("Bad axiWriteCount");
		return;//2012/11/14
	}
}

