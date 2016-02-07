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
#include "mlli_plat.h"


/******************************************************************************
*				GLOBALS
******************************************************************************/


/******************************************************************************
*			FUNCTIONS DECLARATIONS
******************************************************************************/


/******************************************************************************
*				FUNCTIONS
******************************************************************************/


/*!
 * This function retrieves the pointer to the first LLI entry in the MLLI
 * table which resides in SRAM. The first LLI will always be located after
 * the link entry to the next MLLI table.
 *
 * \param qid [in] -The queue Id.
 * \param dir [in] -indicates MLLI_INPUT_TABLE or MLLI_OUTPUT_TABLE
 *
 * \return A pointer to the first LLI entry in the MLLI table
 */
uint32_t *GetFirstLliPtr(int qid, MLLIDirection_t direction)
{
	uint32_t *pBuf;

	DX_PAL_LOG_INFO("qid=%d\n",(int)qid);

	pBuf = (void *)(DX_GetMLLIWorkspace() + qid * MLLI_IN_OUT_BUF_SIZE);
	
	/* Set offset to input or output table */
	pBuf += ( direction == MLLI_INPUT_TABLE ? 0 : MLLI_BUF_SIZE_IN_WORDS );
	
	/* Skip the link entry */
	pBuf += SEP_LLI_ENTRY_WORD_SIZE;
	return pBuf;
}

/*!
 * This function initiates reading of MLLI table in given host memory to
 * the MLLI buffer in SRAM. It pushes DLLI-to-SRAM BYPASS descriptor.
 *
 * \param qid [in] -The queue Id.
 * \param mlliHostAddr [in] - Host DMA address of a structure which represents the 
 *			MLLI table as follow:
 *		     1. A pointer to the first input MLLI table in system RAM
 *		     	and it's size.
 *		     2. The total number of MLLI tables.
 *		     3. The table direction (can be either MLLI_INPUT_TABLE or
 *		     	MLLI_OUTPUT_TABLE).
 * \param tableSize The size in bytes of the pointed MLLI table.
 * \param axiNs The AXI NS bit
 * \param direction Denotes whether this is MLLI for input or for output
*/
void FetchMlliTable(int qid, uint32_t pMlliData, uint32_t size, uint8_t axiNs, MLLIDirection_t direction)
{
	uint32_t mlliAdr;
	HwDesc_s desc;

	/* Check if already allocated by external module */
	if ( DX_GetIsMlliExternalAlloc(qid) == 1 ) {
		DX_PAL_Abort("MLLI workspace is already allocated by external module");
	}
	
	if (size > (MLLI_BUF_SIZE - SEP_LLI_ENTRY_BYTE_SIZE)) {
		DX_PAL_LOG_ERR("Given MLLI size=%u B is too large!\n", (unsigned int)size);
		DX_PAL_Abort("Given MLLI size is too large!");
	}

	mlliAdr = (uint32_t)(DX_GetMLLIWorkspace() + qid * MLLI_IN_OUT_BUF_SIZE);
	mlliAdr += ( direction == MLLI_INPUT_TABLE ? 0 : MLLI_BUF_SIZE );

	/* prepare the first MLLI mlliTable from host */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_TYPE(&desc, DMA_DLLI, pMlliData, size, QID_TO_AXI_ID(qid), axiNs);
	HW_DESC_SET_DOUT_SRAM(&desc, mlliAdr, size);
	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
	AddHWDescSequence(qid, &desc);
}

/*!
 * This function prepares the next MLLI table. It looks for the source
 * address and size of the next LLI table in the first entry of the
 * populated MLLI table in SRAM and overwrites it in the SRAM using
 * "MLLI" transfer.
 *
 * \param qid [in] - The queue Id.
 * \param axiNs [in] - axi secure transactions
 * \param dir [in] - Refers to INPUT or OUTPUT MLLI table in SeP RAM.
 */
void PrepareNextMLLITable(int qid, uint8_t axiNs, MLLIDirection_t direction)
{
	uint32_t mlliAdr;
	HwDesc_s desc;

	/* Check if already allocated by external module */
	if ( DX_GetIsMlliExternalAlloc(qid) == 1 ) {
		DX_PAL_Abort("MLLI workspace is already allocated by external module");
	}
	
	mlliAdr = (uint32_t)(DX_GetMLLIWorkspace() + qid * MLLI_IN_OUT_BUF_SIZE);
	mlliAdr += ( direction == MLLI_INPUT_TABLE ? 0 : MLLI_BUF_SIZE );
	
	/* prepare the next MLLI table. The next table host address already
	   resides in SRAM and needs to be transfered to the same address
	   in SRAM */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_TYPE(&desc, DMA_MLLI, mlliAdr, 0, QID_TO_AXI_ID(qid), axiNs);
	HW_DESC_SET_DOUT_SRAM(&desc, mlliAdr, MLLI_BUF_SIZE - SEP_LLI_ENTRY_BYTE_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
	AddHWDescSequence(qid, &desc);
}

