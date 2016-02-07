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



#ifndef LLF_HASH_H
#define LLF_HASH_H

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

/* The HASH init value on SHA1 mode */
#define HASH_SHA1_INIT_0                     0x67452301UL
#define HASH_SHA1_INIT_1                     0xefcdab89UL
#define HASH_SHA1_INIT_2                     0x98badcfeUL
#define HASH_SHA1_INIT_3                     0x10325476UL
#define HASH_SHA1_INIT_4                     0xc3d2e1f0UL 

/* The HASH init value on SHA2 mode */
#define HASH_SHA256_INIT_0                   0x6a09e667UL
#define HASH_SHA256_INIT_1                   0xbb67ae85UL
#define HASH_SHA256_INIT_2                   0x3c6ef372UL
#define HASH_SHA256_INIT_3                   0xa54ff53aUL
#define HASH_SHA256_INIT_4                   0x510e527fUL
#define HASH_SHA256_INIT_5                   0x9b05688cUL 
#define HASH_SHA256_INIT_6                   0x1f83d9abUL 
#define HASH_SHA256_INIT_7                   0x5be0cd19UL


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/
void  LLF_HASH_Init_Crypto ( DxUint32_t hwBaseAddr );
/**
 * @brief The low level HASH_Init function initializes the hardware to operate on HASH at SHA1 mode
 *
 *        The function operates the follwing steps:
 *         
 *        1. Enable the clock of the HASH engine by setting the register CLK_ENABLE with 
 *           the value 0x4 (HASH_EN bit).
 *        2. Set the CRYPTO_CTL register to the HASH mode - value 0x7.
 *           On this stage the initialization of the hardware to operate the HASH machine is enabled. 
 *        3. Set the HASH_CONTROL register to SHA1 mode.
 *        4. Wait until the HASH_BUSY register is ready ( the value is 0 ).
 *           On this stage the initialization of the hardware to operate the HASH machine is enabled. 
 *        5. Initialize the HASH_H0 - HASH_H4 registes. 
 *        6. Wait until the HASH_BUSY register is ready ( the value is 0 ).
 *           On this stage the initialization of the hardware to operate the HASH machine is ended. 
 *        7. Exit the function.
 *
 * @param[in] hwBaseAddr        -  cryptocell base address
 *
 */                              
void  LLF_HASH_Init ( DxUint32_t hwBaseAddr );

/**
 * @brief The low level HASH_UpdateData executes the HASH with the current data received.     
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

void  LLF_HASH_UpdateData ( DxUint32_t    hwBaseAddr,
			    DxUint32_t   *DataIn_ptr,                
			    DxUint32_t    DataInSizeInWords,
			    DxUint32_t    TotalSizeInWords,
			    DxUint32_t    PollFlag,
			    DxUint32_t    useDma );

/**
 * @brief The low level LLF_HASH_GetResults reads the result from the HASH_Hi registers
 * 
 * @param[in] hwBaseAddr        -  cryptocell base address
 *        
 * @param[in] result - The HASH result.
 *
 */                              
void  LLF_HASH_GetResults( DxUint32_t hwBaseAddr, HASH_Result_t Result );

/**
 * @brief The low level LLF_HASH_WAIT_ON_Busy waits on DMA busy and HASH busy
 *         
 * @param[in] hwBaseAddr        -  cryptocell base address
 *
 */
void LLF_HASH_WAIT_ON_HASHAndDMABusy(DxUint32_t hwBaseAddr);

/************************ Private Functions ******************************/

#ifdef __cplusplus
}
#endif

#endif



