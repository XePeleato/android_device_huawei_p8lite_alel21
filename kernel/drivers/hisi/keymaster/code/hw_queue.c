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

#include "bsp.h"
#include "compiler.h"
#include "completion.h"
#include "hw_queue_defs.h"
#include "hw_queue.h"
#include "dx_error.h"
#include "dx_pal_log.h"
#include "dx_pal_abort.h"
#include "debug.h"
/*#warning The use of "HW_DESC_LOCK_QUEUE()" macro should be completely removed from \
	 any related code once the CC54 generic FPGA will be delivered.*/
/*
	 Explanation: In CC54-Intel-tngr project we are still using the water-mark-interrupt \
	 technique to avoid potential deadlock (bug #7299), once the HW-Q-LOCK will be eliminated \
	 we must kill the "#if CC_DEV_ID == 54" defines and adapt the "HW_QUEUE_POLL_QUEUE_UNTIL_FREE_SLOTS()" \
	 macro polling technique.
*/

/******************************************************************************
*				GLOBALS
******************************************************************************/
extern uint32_t gAxiWriteCount[MAX_NUM_HW_QUEUES] DX_SRAM_SBSS_VARIABLE;

/******************************************************************************
*				FUNCTIONS
******************************************************************************/

/*!
 * This function initializes a HW queue, sets up the watermark event and 
 * the mutex related to that queue.
 * 
 * \param qid An ID of a HW queue according to the queue priority. "1" is the lowest priority.
 * 
 * \return int one of the error codes defined in err.h
 */
int InitHWQueue(void)
{
	int rc = DX_RET_OK;
	int qid;
       PRINT_INFO("--->NOW into InitHWQueue\n");
	for (qid = 0; qid < MAX_NUM_HW_QUEUES; qid++) {
		/* initializes resources of the specific platform */
		rc = InitHwQueuePlat(qid);
		if (rc != DX_RET_OK) {
			goto EndWithErr;
		}
	}

EndWithErr:
	PRINT_INFO("--->NOW leave InitHWQueue\n");
	return rc;
}

/*!
 * This function adds a HW descriptor sequence to a HW queue. If not
 * enough free slot are available in the HW queue, the function will set
 * up the "Water Mark" register and wait on an event until free slots are
 * available. This function will always mark the last descriptor in the
 * sequence as "last", even if the "last" bit was left clear. The caller
 * can leave the "Ack needed" field un-initialized. This function will
 * set the "Ack needed" field in each descriptor to either zero or to the
 * CounterId for the last descriptor.
 * The caller can indirectly control whether the function will block until
 * the descriptor is complete or return without blocking for asynchronous
 * mode. This is done by referring to a completion counter ID that is
 * defined as "external completion" or "internal completion".
 *
 * \param qid An ID of a HW queue according to the queue priority. "1" is the lowest priority.
 * \param descSeq A pointer to a HW descriptor sequence. All descriptor
 *		      structures are 5 words long according to [CC54-DESC].
 *		      The sequence buffer is a group of word aligned sequential
 *		      descriptor buffers.
 */
void AddHWDescSequence(int qid, HwDesc_s* descSeq)
{
#if (CC_DEV_ID == 54)
	uint32_t freeSlots;
#endif
	
	DX_PAL_LOG_DEBUG("qid=%d\n", (int)qid);
	
#if (CC_DEV_ID == 54)
	/* check if there is space in queue */
	freeSlots = HW_QUEUE_FREE_SLOTS_GET(qid);
	DX_PAL_LOG_DEBUG("Free slots=%lx\n", freeSlots);

	/* wait until there is sufficient space in queue */
	while (freeSlots == 0) {
		DX_PAL_LOG_WARN("Free slots in qid=%d is 0\n", qid);

		/* set the watermark register with the amount of space needed */
		DX_HAL_WriteCcRegister(GET_HW_Q_WATERMARK(qid), 1);

		DX_PAL_LOG_DEBUG("Taking waterMarkEvent qid=%d\n", (int)qid);

		/* wait until warter mark is reached */
		WaitForHwQueueWaterMark();

		freeSlots = HW_QUEUE_FREE_SLOTS_GET(qid);
	}
#else
	/* We intentionally force busy wait and not blocking this task with
	a semaphore to prevent task context switch.
	IMPORTANT: due to a bug (#7299) that is under investigation we're forced to 
	maintain this code only in PE project since this bug appears in multi-queue only.*/
	HW_QUEUE_POLL_QUEUE_UNTIL_FREE_SLOTS(qid, 1);
#endif

	/* Push to HW queue */
	/* Check if dout is of AXI type */
	if (IS_HW_DESC_DOUT_TYPE_AXI(descSeq)) {
		gAxiWriteCount[qid]++;

		if (gAxiWriteCount[qid] == (1 << FIFO_AXI_COUNTER_BIT_SIZE)) {
			DX_PAL_Abort("Too many AXI dout in sequence!");
		}

	}
		
	HW_DESC_PUSH_TO_QUEUE(qid, descSeq);
}

/*!
 * This function adds a dummy completion HW descriptor to a HW queue in
 * order to later on signal an internal completion event.
 * The dummy HW completion descriptor is created by using the DMA bypass
 * mode with zero size DIN and DOUT data. A counter ID related to the qid is always
 * used to setup the "Ack required" field in the HW descriptor.
 * 
 * \param qid An ID of a HW queue according to the queue priority. "1" is the lowest priority.
 * \param taskId Task ID as set by vTaskSetCurrentTaskTag().
 * \param completionCtx Completion context contains the required information for completion (platform specific).
 */
void AddCompletionDesc(int qid, uint32_t taskId, void *completionCtx)
{
#if (CC_DEV_ID == 54)
	uint32_t freeSlots;
#endif
	HwDesc_s desc;

	DX_PAL_LOG_DEBUG("qid=%d taskId=%d\n", (int)qid, (int)taskId);

	HW_DESC_INIT(&desc);

	/* create NOP descriptor */
	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);

	/* set the ACK bits with the completion counter if corresponding the queue id */
	HW_DESC_SET_ACK_NEEDED(&desc, QID_TO_COMPLETION_COUNTER_ID(qid));

	/* Lock the HW queue */
	HW_QUEUE_LOCK();

#if (CC_DEV_ID == 54)
	/* Lock the HW queue */
	HW_QUEUE_LOCK();

	/* check if there is space in queue */
	freeSlots = HW_QUEUE_FREE_SLOTS_GET(qid);

	/* wait until there is sufficient space in queue */
	while (freeSlots < 1) {
		DX_PAL_LOG_WARN("Free slots in qid=%d is %lu\n", qid, freeSlots);

		/* set the watermark register with the amount of space needed */
		DX_HAL_WriteCcRegister(GET_HW_Q_WATERMARK(qid), 1);
		
		/* wait until warter mark is reached */
		WaitForHwQueueWaterMark();

		freeSlots = HW_QUEUE_FREE_SLOTS_GET(qid);
	}
#else
	/* We intentionally force busy wait and not blocking this task with
	a semaphore to prevent task context switch.
	IMPORTANT: due to a bug (#7299) that is under investigation we're forced to 
	maintain this code only in PE project since this bug appears in multi-queue only.*/
	HW_QUEUE_POLL_QUEUE_UNTIL_FREE_SLOTS(qid, 1);
#endif

	/* Add sequence to completion fifo */
	/* Mark according to completion type: internal or external */
	CompletionFifoInsert(qid, taskId, completionCtx, gAxiWriteCount[qid]);

	HW_DESC_PUSH_TO_QUEUE(qid, &desc);

	/* Clear the AXI counter */
	gAxiWriteCount[qid] = 0;

	/* Unlock HW queue */
	HW_QUEUE_UNLOCK();
}

/*!
 * This function adds a set register HW descriptor to a HW queue.
 *  
 * \param qid An ID of a HW queue according to the queue priority. "1" is the lowest priority.
 * \param address The register address.
 * \param value The register value.
 * \param lockQueue LOCK_HW_QUEUE or UNLOCK_HW_QUEUE.
 */
void AddSetRegisterValueDesc(int qid, uint32_t address, uint32_t value, uint8_t lockQueue)
{
	HwDesc_s desc;
		
	DX_PAL_LOG_DEBUG("qid=%d, register=0x%08X, value=%d, lockQueue=%d\n", 
		(int)qid, (unsigned int)address, (int)value, (int)lockQueue);

	HW_DESC_INIT(&desc);
	
	/* create register set descriptor */
	HW_DESC_SET_DIN_NO_DMA(&desc, address, SET_REGISTER_DESC_MARK);
	HW_DESC_SET_DOUT_NO_DMA(&desc, value, 0, 1);	
	HW_DESC_LOCK_QUEUE(&desc, lockQueue);
	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);

	AddHWDescSequence(qid, &desc);
}

