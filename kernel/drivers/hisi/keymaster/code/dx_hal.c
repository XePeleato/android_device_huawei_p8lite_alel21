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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CCLIB
//delete by lixiaojie 2012/11/5
//#include <unistd.h>
//#include <sys/mman.h>
//#include <fcntl.h>
#include "dx_host.h"
#include "dx_bitops.h"
#include "dx_cc_regs.h"
#include "dx_hal.h"
#include "sep_ctx.h"
#include <asm/io.h>
#include "soc_baseaddr_interface.h"

/******************************************************************************
*				DEFINITIONS
******************************************************************************/
#define DX_HAL_DEVNAME "/dev/mem"
#define DX_CC_REG_AREA_LEN 0x100000

/******************************************************************************
*				GLOBALS
******************************************************************************/
//static int halFileH = -1;//modified by lixiaojie 2012/11/1
//static uint32_t gCcRegBase = 0;
uint32_t gCcRegBase = 0;

void __iomem *g_dx_base_va = NULL;

/******************************************************************************
*				FUNCTIONS
******************************************************************************/

/*!
 * HAL layer entry point.
 * Mappes CryptoCell 4.X regisers to the HOST virtual address space.
 */
int DX_HAL_Init(void)
{
	g_dx_base_va = ioremap(SOC_SECENG_BASE_ADDR, 0x1000);
	gCcRegBase = (uint32_t)DX_BASE_CC;

	return DX_HAL_OK;
}


/*!
 * HAL exit point.
 * Unmaps CryptoCell 4.X registers.
 */
int DX_HAL_Terminate()
{/*
    //modified by lixiaojie 1012/11/1/17:27
	if (halFileH < 0) {
		return DX_HAL_ENODEV;
	}

	munmap((uint32_t *)gCcRegBase, DX_CC_REG_AREA_LEN);
	close(halFileH);
	halFileH = -1;
	gCcRegBase = 0;
	*/
	iounmap(g_dx_base_va);
	return DX_HAL_OK;
}


/*!
 * Busy wait upon Interrupt Request Register (IRR) signals.
 * This function notifys for any CC interrupt, it is the caller responsiblity
 * to verify and prompt the expected case interupt source.
 *
 * @param[in] data 	- input data for future use
 * \return uint32_t The IRR value.
 */
uint32_t DX_HAL_WaitInterrupt(uint32_t data)
{
	uint32_t irr = 0;

	/* busy wait upon IRR signal */
	while ( !(irr = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_IRR))) );

	return irr;
}

