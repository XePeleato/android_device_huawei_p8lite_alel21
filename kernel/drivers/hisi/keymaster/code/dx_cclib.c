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

#include "dx_pal_types.h"
#include "dx_pal_log.h"
#include "dx_cclib.h"
#include "dx_hal.h"
#include "dx_pal_init.h"
#include "hw_queue.h"
#include "completion.h"
#include "crys_rnd.h"
#include "sym_adaptor_driver.h"
#include "debug.h" //2012/11/12
#if ENABLE_RSA_DRIVER
#include "dx_asym_init.h"
#endif

static uint32_t dx_cclib_init_state = 0;
static CRYS_RND_WorkBuff_t  workBuff_ptr;

/*! 
 * TEE (Trusted Execution Environment) entry point. 
 * Init CryptoCell for TEE.
 * 
 * \return DX_CclibRetCode_t one of the error codes defined in dx_cclib.h
 */
DX_CclibRetCode_t DX_CclibInit(void)
{
	int rc = DX_CCLIB_RET_OK;
	int virtMemBaseAddr = 0;
       PRINT_INFO("->now entrt into DX_CclibInit \n");
	if(dx_cclib_init_state) {
		PRINT_INFO("->in DX_CclibInit :dx_cclib_init_state's value: 0x%x\n",dx_cclib_init_state);
		return DX_CCLIB_RET_OK;
	}
	PRINT_INFO("->in DX_CclibInit :before DX_PAL_Init \n");
	virtMemBaseAddr = DX_PAL_Init();
	PRINT_INFO("->in DX_CclibInit :DX_PAL_Init virtMemBaseAddr = %d\n",virtMemBaseAddr);
	if (NULL == (uint32_t *)virtMemBaseAddr) {
		PRINT_INFO("->in DX_CclibInit :DX_PAL_Init error,NULL == virtMemBaseAddr\n");
		rc = DX_CCLIB_RET_EINVAL;
		goto InitErr;
	}
	rc = DX_HAL_Init();
	PRINT_INFO("->in DX_CclibInit :DX_HAL_Init return:%d\n",rc);
	if (rc != DX_CCLIB_RET_OK) {
		PRINT_INFO("->in DX_CclibInit :DX_HAL_Init error,rc != DX_CCLIB_RET_OK\n");
		rc = DX_CCLIB_RET_HAL;
		goto InitErr1;
	}
	
    /* Call sw reset to reset the CC before starting to work with it */
   // DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_CC_SW_RST) , 0x1UL);
    /* wait for reset to be completed */
    //for (i=0; i<20; i++);

#ifdef BIG__ENDIAN
	/*initiate the Endian to BIG */
       PRINT_INFO("->into BIG__ENDIAN\n");
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_ENDIAN) , 0xCCUL);
 	//DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_ENDIAN) , 0x00UL);
#endif
       PRINT_INFO("->before InitHWQueue\n");
	rc = InitHWQueue();//InitHWQueue(NO_OS_QUEUE_ID);
	PRINT_INFO("->after InitHWQueue,InitHWQueue return value %d\n",rc);
	if (rc != DX_CCLIB_RET_OK) {
		PRINT_INFO("->n DX_CclibInit :InitHWQueue error,rc != DX_CCLIB_RET_OK\n");
		rc = DX_CCLIB_RET_HW_Q_INIT; 
		goto InitErr2;
	}
	PRINT_INFO("->before InitCompletion\n");
	rc = InitCompletion();
	PRINT_INFO("->after InitCompletion,InitCompletion return value %d\n",rc);
	if (rc != DX_CCLIB_RET_OK) {
		PRINT_INFO("->in DX_CclibInit :InitCompletion error,rc != DX_CCLIB_RET_OK\n");
		rc = DX_CCLIB_RET_COMPLETION; 
		goto InitErr2;
	}

	rc = SymDriverAdaptorModuleInit();
	if (rc != DX_CCLIB_RET_OK) {
		rc = DX_CCLIB_RET_COMPLETION;  // check
		goto InitErr;
	}

#if ENABLE_RSA_DRIVER
	/* Initialize PKA's semaphore */
PRINT_INFO("->into ENABLE_RSA_DRIVER\n");
	rc = DX_ASYM_Init();
#endif
        
#ifndef CRYS_NO_RND_SUPPORT
        /* Initialize RND module */
        rc = CRYS_RND_Instantiation(&workBuff_ptr);
	if (rc != CRYS_OK) {
		  goto InitErr2;
	}
#endif

	dx_cclib_init_state = DX_CCLIB_RET_OK;
	
	return 0;
InitErr2:
	DX_HAL_Terminate();

InitErr1:
	DX_PAL_Terminate();

InitErr:
	PRINT_INFO("->in DX_CclibInit :at end,the rc value is:%d\n",rc);
	return rc;
}

/*!
 * TEE (Trusted Execution Environment) exit point. 
 * Finalize CryptoCell for TEE operation, release associated resources.
 */
void DX_CclibFini(void)
{
	SymDriverAdaptorModuleTerminate();
	DX_HAL_Terminate();
	DX_PAL_Terminate();
	CRYS_RND_UnInstantiation();
}


/*!
 * Set requested CryptoCell priority queue 
 * This function must be invoked before DX_CclibInit
 * 
 * \param priority Requested priority queue
 *
 * \return DxCclib_RetCode
 */
DX_CclibRetCode_t DX_CclibSetPriority(DX_CclibDevPriority priority)
{
	
	return DX_CCLIB_RET_OK;	  
}
