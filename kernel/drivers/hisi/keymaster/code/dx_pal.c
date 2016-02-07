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



/************* Include Files ****************/
#include "dx_pal_init.h"
#include "dx_pal_dma_plat.h"
#include "dx_pal_log.h"

//modified by lixiaojie.1012/11/6
#define WORKSPACE_FREE_MEM_BASE_ADR		0x10000000
#define WORKSPACE_CONTIG_FREE_MEM		0x1001000
//#define WORKSPACE_CONTIG_FREE_MEM		0x10000


/** 
 * @brief   PAL layer entry point. 
 *          The function initializes customer platform sub components,
 *           such as memory mapping used later by CRYS to get physical contiguous memory.
 *         
 *
 * @return Virtual start address of contiguous memory
 */ 	
int DX_PAL_Init(void)
{
	//DX_PAL_LogInit();
	return DX_PAL_DmaInit(WORKSPACE_CONTIG_FREE_MEM, WORKSPACE_FREE_MEM_BASE_ADR);
}


/** 
 * @brief   PAL layer entry point. 
 *          The function initializes customer platform sub components,
 *           such as memory mapping used later by CRYS to get physical contiguous memory.
 *         
 *
 * @return None
 */ 	
void DX_PAL_Terminate()
{
	DX_PAL_DmaTerminate();
}
		                      
