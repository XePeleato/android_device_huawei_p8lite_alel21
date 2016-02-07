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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_MLLI

#include "bsp.h"
#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "dx_pal_abort.h"
#include "cc_plat.h"
#include "completion.h"
#include "hw_queue.h"
#include "mlli.h"


/******************************************************************************
*				GLOBALS
******************************************************************************/

/* SRAM workspace buffer for input and output MLLI tables per queue */
static uint8_t *gMlliWorkspace = (uint8_t *)DX_MLLI_SRAM_BASE_ADDR;

static void SetMlliStopEntry(int qid);

/******************************************************************************
*			FUNCTIONS DECLARATIONS
******************************************************************************/


/******************************************************************************
*				FUNCTIONS
******************************************************************************/

/*!
 * This function allocates IN/OUT MLLI tables in SRAM and appends the
 * "Last LLI" marker to the end of the IN/OUT tables. Each MLLI table size
 * is 1K + 8 bytes of the "last LLI" entry.
 *
 * \param none
 * 	
 * \return one of the error codes defined in err.h
 */
void InitMlli()
{
	uint32_t qid;

	DX_PAL_LOG_INFO("Clear MLLI workspace at adr=0x%08X size=0x%08X\n", 
		(unsigned int)gMlliWorkspace, (unsigned int)(MAX_NUM_HW_QUEUES * MLLI_IN_OUT_BUF_SIZE));

	/* clear MLLI tables memory */
	_ClearSram((uint32_t*)gMlliWorkspace, MAX_NUM_HW_QUEUES * MLLI_IN_OUT_BUF_SIZE);
	for (qid = 0; qid < MAX_NUM_HW_QUEUES; qid++) {
		SetMlliStopEntry(qid);
	}

}


static void SetMlliStopEntry(int qid)
{
	uint32_t *pBuf = (void *)gMlliWorkspace;
	uint8_t	i;
	
	/* Buffer points to the begining of the corresponding queue by qid */
	pBuf += qid * MLLI_IN_OUT_BUF_SIZE_IN_WORDS;
	
	/* Buffer points to last entry in input table */
	pBuf += MLLI_BUF_SIZE_IN_WORDS - SEP_LLI_ENTRY_WORD_SIZE;
	
	/* Set stop entry in last position of every MLLI table. */
	/* We have two tables (IN/OUT) per queue */
	for ( i=0; i<2; i++ ) {
		DX_PAL_LOG_INFO("Set stop entry at adr=0x%08X\n", (unsigned int)pBuf);

		_WriteWordsToSram((uint32_t*)pBuf ,0x80000000 , 4);
		/* Set offset to next table */
		pBuf += MLLI_BUF_SIZE_IN_WORDS;
	}
}

uint8_t * DX_GetMLLIWorkspace(void)
{
	return gMlliWorkspace;
}

uint32_t DX_GetIsMlliExternalAlloc(uint32_t qid)
{
	return 0;
}
