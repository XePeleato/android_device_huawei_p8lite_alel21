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

#ifndef  TEE_COMPLETION_PLAT_H
#define  TEE_COMPLETION_PLAT_H

#include "dx_pal_types.h"

/******************************************************************************
*				DEFINES
******************************************************************************/
#define MAX_COMPLETION_COUNTERS (MAX_NUM_HW_QUEUES)
#define QID_TO_COMPLETION_COUNTER_ID(qid) (qid)

/* Fifo size must be a power of 2 */
#define COMPLETION_FIFO_LEN			64

#define FIFO_COMPLETION_TYPE_BIT_OFFSET		0
#define FIFO_COMPLETION_TYPE_BIT_SIZE		2

#define FIFO_AXI_COUNTER_BIT_OFFSET		2
#define FIFO_AXI_COUNTER_BIT_SIZE		4

#define FIFO_TASK_ID_BIT_OFFSET			6
#define FIFO_TASK_ID_BIT_SIZE			2

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

typedef enum CompletionType {
	COMPLETION_EXTERNAL = 1,	/* Must be equal to one */
	COMPLETION_INVALID = 2,
	COMPLETION_INTERNAL = INT8_MAX,/* No internal completion but must be defined */
	COMPLETION_END = INT8_MAX,
} CompletionType_t;


/*!
 * This function initializes the completion counter event, clears the
 * state structure and sets completion counter "0" as the first available
 * counter to be used when calling "AllocCounter".
 * 
 * \return int one of the error codes defined in err.h
 */
int InitCompletionPlat(void);

/*!
 * This function waits for current descriptor sequence completion.
 */
void WaitForSequenceCompletionPlat(void);

/*!
 * This function adds a completion report to the completion fifo.
 *
 * \param qid The queue id.
 * \param taskId Task ID to be signaled upon completion.
 * \param pCompletionType The completion Type.
 * \param axiWriteCount AXI transactions counter from previous completion report.
 */
void CompletionFifoInsert(int qid, uint32_t taskId, CompletionType_t *pCompletionType, uint32_t axiWriteCount);

/*!
 * This function remove a completion report to the completion fifo.
 * 
 * \param qid The queue id.
 * \param taskId Task ID to be signaled upon completion.
 * \param pCompletionType The completion Type.
 * \param axiWriteCount AXI transactions counter from previous completion report.
 */
void CompletionFifoRemove(int qid, uint32_t *taskId, CompletionType_t *pCompletionType, uint32_t *axiWriteCount);

#endif /*TEE_COMPLETION_PLAT_H*/

