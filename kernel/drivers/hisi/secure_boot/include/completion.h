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

#ifndef  FW_COMPLETION_H
#define  FW_COMPLETION_H


#include "completion_plat.h"
#include "compiler.h"

/******************************************************************************
*			   	    MACROS
******************************************************************************/
#define GET_FIFO_COMPLETION_TYPE(fifoItem)		BITFIELD_GET(fifoItem, FIFO_COMPLETION_TYPE_BIT_OFFSET, FIFO_COMPLETION_TYPE_BIT_SIZE)
#define GET_FIFO_AXI_COUNTER(fifoItem)			BITFIELD_GET(fifoItem, FIFO_AXI_COUNTER_BIT_OFFSET, FIFO_AXI_COUNTER_BIT_SIZE)
#define GET_FIFO_COMPLETION_TASK_ID(fifoItem)		BITFIELD_GET(fifoItem, FIFO_TASK_ID_BIT_OFFSET, FIFO_TASK_ID_BIT_SIZE)

#define SET_FIFO_COMPLETION_TYPE(fifoItem, val)		BITFIELD_SET(fifoItem, FIFO_COMPLETION_TYPE_BIT_OFFSET, FIFO_COMPLETION_TYPE_BIT_SIZE, val)
#define SET_FIFO_AXI_COUNTER(fifoItem, val)		BITFIELD_SET(fifoItem, FIFO_AXI_COUNTER_BIT_OFFSET, FIFO_AXI_COUNTER_BIT_SIZE, val)
#define SET_FIFO_COMPLETION_TASK_ID(fifoItem, val)	BITFIELD_SET(fifoItem, FIFO_TASK_ID_BIT_OFFSET, FIFO_TASK_ID_BIT_SIZE, val)

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * This function calls the platform specific Completion Initializer function.
 * 
 * \return int one of the error codes defined in err.h
 */
#define InitCompletion InitCompletionPlat

/*! 
 * This function waits for current descriptor sequence completion. 
 * The "WaitForSequenceCompletionPlat" function must implement by
 * the platform port layer. 
 */
#define WaitForSequenceCompletion WaitForSequenceCompletionPlat

#endif /*FW_COMPLETION_H*/

