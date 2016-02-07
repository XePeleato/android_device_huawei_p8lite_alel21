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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "sb_hash.h"
#include "llf_hash.h"
#include "secureboot_gen.h"


/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************* Private function prototype ****************/


/************************ Public Functions ******************************/

/**
 * @brief The HASH_Init function is the high level initialization function.
 *        it calls the low level hash init.
 *
 * @param[in] hwBaseAddr        -  cryptocell base address
 *
 */                              
void  HASH_Init ( DxUint32_t hwBaseAddr )
{  

/*------------------
    CODE
-------------------*/

LLF_HASH_Init(hwBaseAddr);

return;

}/* END OF HASH_Init */                                           

/**
 * @brief The HASH_UpdateData executes the HASH with the current data received. This is the high level function.     
 * 
 *
 * @param[in] hwBaseAddr        -  cryptocell base address
 *
 * @param[in/out] DataIn_ptr    -  The pointer to the users data input buffer it is updated to the 
 *                                 exact popsition after the first operation.
 * @param[in] DataInSize        -  The size of the data to update.
 *                                 If it is not the last block (TotalSizeInWords = 0 ) the size must be a 
 *                                 multiple of blocks. 
 *                                 buffer in the context that is loaded automaticly.
 * @param[in] TotalSizeInWords  - 0 - just updates the data otherwise this is the last packet and it loads the padding
 *                                 according to its value
 *
 * @param[in] PollFlag          -  if this flag is set the function will poll on the busy bits. if its clear the function will exit
 *                                 without polling ( this node is to support the interrupt mode ).
 *
 * @param[in] useDma            - the decide if to use the DMA or not.
 *
 */                              
void  HASH_UpdateData ( DxUint32_t    hwBaseAddr,
			DxUint32_t   *DataIn_ptr,                
			DxUint32_t    DataInSizeInWords,
			DxUint32_t    TotalSizeInWords,
			DxUint32_t    PollFlag,
			DxUint32_t    useDma )
{ 
/*------------------
    CODE
-------------------*/

LLF_HASH_UpdateData(hwBaseAddr, DataIn_ptr, DataInSizeInWords, TotalSizeInWords, PollFlag, useDma);

return;

}/* END OF HASH_UpdateData */

/**
 * @brief The HASH_UpdateDataWithoutWait executes the HASH with the current data received. This is the high level function.     
 *        This function does not wait till the HASH finishes before returning, but waits before starting
 *        the next operation.
 * 
 * @param[in] hwBaseAddr        -  cryptocell base address
 *
 * @param[in/out] DataIn_ptr    -  The pointer to the users data input buffer it is updated to the 
 *                                 exact popsition after the first operation.
 * @param[in] DataInSize        -  The size of the data to update.
 *                                 If it is not the last block (TotalSizeInWords = 0 ) the size must be a 
 *                                 multiple of blocks. 
 *                                 buffer in the context that is loaded automaticly.
 * @param[in] TotalSizeInWords  - 0 - just updates the data otherwise this is the last packet and it loads the padding
 *                                 according to its value
 *
 *
 * @param[in] useDma            - the decision if to use the DMA or not.
 *
 */                              
void  HASH_UpdateDataWithoutWait ( DxUint32_t    hwBaseAddr,
				   DxUint32_t   *DataIn_ptr,                
				   DxUint32_t    DataInSizeInWords,
				   DxUint32_t    TotalSizeInWords,                                   
				   DxUint32_t    useDma )
{ 
/*------------------
    CODE
-------------------*/
/* Wait on Busy before starting the HASH */
LLF_HASH_WAIT_ON_HASHAndDMABusy(hwBaseAddr);

/* Init the HASH operation again (in case it was interrupted) but without initializing the Hs */
LLF_HASH_Init_Crypto(hwBaseAddr);

/* Call the low level function without polling on busy */
LLF_HASH_UpdateData(hwBaseAddr, DataIn_ptr, DataInSizeInWords, TotalSizeInWords, DX_FALSE, useDma);

return;

}/* END OF HASH_UpdateDataWithoutWait */

/**
 * @brief The HASH_GetResults return the HASH result. This is the high level function.
 * 
 * @param[in] hwBaseAddr        -  cryptocell base address
 *        
 * @param[in] result - The HASH result.
 *
 */                              
void  HASH_GetResults( DxUint32_t hwBaseAddr, HASH_Result_t Result )
{                         

/*------------------
    CODE
-------------------*/

LLF_HASH_GetResults(hwBaseAddr, Result);

}/* END OF HASH_GetResults */  

/**
 * @brief The HASH_WaitAndGetResults wait till the HASH machine has finished and than 
 *        return the HASH result. This is the high level function.
 *
 * @param[in] hwBaseAddr        -  cryptocell base address
 *         
 * @param[in] result - The HASH result.
 *
 */                              
void  HASH_WaitAndGetResults( DxUint32_t hwBaseAddr, HASH_Result_t Result )
{                         

/*------------------
    CODE
-------------------*/
/* Wait on Busy before starting the HASH */
LLF_HASH_WAIT_ON_HASHAndDMABusy(hwBaseAddr);

LLF_HASH_GetResults(hwBaseAddr, Result);

}/* END OF HASH_GetResults */

/**
 * @brief The HASH_HashOnDataBlock function executes a HASH on a block of ram.
 *
 *        This function overides the RAM !!! - all data is overruned.
 *
 *        The function operates the follwing steps:
 *         
 *
 */                              
void HASH_HashOnDataBlock(DxUint32_t hwBaseAddr,
			  DxUint32_t *InputData_ptr,
			  DxInt32_t  SizeInWords,
			  DxUint32_t *WorkRam_ptr,
			  HASH_Result_t HashResult,
			  DxUint32_t  UseDma  )
{

/* LOCAL DECLERATIONS */

WorkRam_ptr = WorkRam_ptr;

/* FUNCTION LOGIC */

/* initialize the HASH */
HASH_Init(hwBaseAddr);

/* ............ update HASH on NOR flash case ............... */
/* ---------------------------------------------------------- */

HASH_UpdateData ( hwBaseAddr,
		  InputData_ptr,                
		  SizeInWords,
		  SizeInWords,
		  DX_TRUE,
		  UseDma );

/* get the results */
HASH_GetResults( hwBaseAddr, HashResult );

return;

}/* END OF SB_ExecuteHashOnDataBlock */

