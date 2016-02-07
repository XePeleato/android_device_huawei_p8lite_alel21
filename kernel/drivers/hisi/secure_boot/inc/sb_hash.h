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


#ifndef HASH_H
#define HASH_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %LLF_HASH.h    : %
   *  State           :  %state%
   *  Creation date   :  Mon Nov 29 14:09:42 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version LLF_HASH.h#1:incl:4
   *  \author adams
   *  \remarks Copyright (C) 2004 by Discretix Technologies Ltd.
   *           All Rights reserved
   */




/************************ Defines ******************************/


#ifdef DX_HASH_SHA1_MODE

/* the hashblock size in words */
#define HASH_BLOCK_SIZE_IN_WORDS             16

/* the HASH - SHA1 results in words */
#define HASH_RESULT_SIZE_IN_WORDS            5
#define HASH_RESULT_SIZE_IN_BYTES            20

#else

/* the hashblock size in words */
#define HASH_BLOCK_SIZE_IN_WORDS             16

/* the HASH - SHA2 results in words */
#define HASH_RESULT_SIZE_IN_WORDS            8
#define HASH_RESULT_SIZE_IN_BYTES            32

#endif

/************************ Enums ********************************/


/************************ Typedefs  ****************************/

/* defining the IV counter buffer */
typedef DxUint32_t HASH_Result_t[HASH_RESULT_SIZE_IN_WORDS];
#define NVM_HASH_Result_t HASH_Result_t

/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

/**
 * @brief The HASH_Init function is the high level initialization function.
 *        it calls the low level hash init.
 *
 * @param[in] hwBaseAddr        -  cryptocell base address
 *
 */                              
void  HASH_Init ( DxUint32_t hwBaseAddr );

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
                        DxUint32_t    useDma );
                        
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
                                   DxUint32_t    useDma );
                                                           
/**
 * @brief The HASH_GetResults return the HASH result. This is the high level function.
 * 
 * @param[in] hwBaseAddr        -  cryptocell base address
 *        
 * @param[in] result - The HASH result.
 *
 */                              
void  HASH_GetResults( DxUint32_t hwBaseAddr, HASH_Result_t Result );

/**
 * @brief The HASH_WaitAndGetResults wait till the HASH machine has finished and than 
 *        return the HASH result. This is the high level function.
 *
 * @param[in] hwBaseAddr        -  cryptocell base address
 *         
 * @param[in] result - The HASH result.
 *
 */                              
void  HASH_WaitAndGetResults( DxUint32_t hwBaseAddr, HASH_Result_t Result );

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
                          DxUint32_t  UseDma  );

/************************ Private Functions ******************************/

#ifdef __cplusplus
}
#endif

#endif



