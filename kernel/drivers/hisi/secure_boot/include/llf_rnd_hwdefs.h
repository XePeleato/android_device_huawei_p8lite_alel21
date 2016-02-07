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

#ifndef LLF_RND_HW_DEFS_H
#define LLF_RND_HW_DEFS_H


/************************ Defines ******************************/

/* SRAM base address */
extern DxUint32_t g_MemOffsetAddr;

/* The number of words generated in the entropy holding register (EHR) 
   6 words (192 bit) according to HW implementation */
#define LLF_RND_HW_TRNG_EHR_WIDTH_IN_WORDS	6   
#define LLF_RND_HW_TRNG_EHR_WIDTH_IN_BYTES	(LLF_RND_HW_TRNG_EHR_WIDTH_IN_WORDS * sizeof(DxUint32_t))
#define LLF_RND_HW_TRNG_EHR_WIDTH_IN_BITS       (8*LLF_RND_HW_TRNG_EHR_WIDTH_IN_BYTES)

/* ring oscillator length maximal level */
#define LLF_RND_HW_TRNG_ROSC_FAST_MAX_LEVEL    3UL
#define LLF_RND_HW_TRNG_ROSC_LENGTH_MASK       0x0f

/* TRNG_CONFIG value for SRC_SEL = 0, SOP_SEL = 1 (SOP = TRNG EHR output)*/
#define LLF_RND_HW_TRNG_WITH_DMA_CONFIG_VAL    0x4 

/* HW_TRNG values on Fast mode */
/*---------------------------------------*/ 
#define LLF_RND_HW_DEBUG_CONTROL_VALUE_ON_FAST_MODE  0x0000000E
#define LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_FAST_MODE  21UL

/* HW_TRNG registers values on Slow mode */
/*---------------------------------------*/ 
#define LLF_RND_HW_DEBUG_CONTROL_VALUE_ON_SLOW_MODE  0x00000000
#define LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_SLOW_MODE  2UL /*for both AES128 and AES256*/

/* HW RND DMA SRAM address offset, bytes */
#ifndef LLF_RND_HW_DMA_SRAM_ADDRESS
#define LLF_RND_HW_DMA_SRAM_ADDRESS  (6*1024) 
#endif

#define LLF_RND_HW_RND_DMA_ENABLE_VAL      	1UL
#define LLF_RND_HW_RND_DMA_DISABLE_VAL      	0UL
#define LLF_RND_HW_RND_SRC_ENABLE_VAL      	1UL
#define LLF_RND_HW_RND_SRC_DISABLE_VAL      	0UL
#define LLF_RND_HW_RND_CLK_ENABLE_VAL      	1UL
#define LLF_RND_HW_RND_CLK_DISABLE_VAL     	0UL

/* 
   LLF_RND_TRNG_MAX_TIME_COEFF - scaled coefficient, defining relation of
   maximal allowed time for TRNG generation (per one ROSC) expected minimal
   time: MaxAllowedTime = (ExpectTime*LLF_RND_TRNG_MAX_TIME_COEFF) / 64, where
   64 = (1<<6) is a scaling coefficient.
   Example: if LLF_RND_TRNG_MAX_TIME_COEFF = 128, then MaxAllowedTime =
   2*ExpectTime.
*/ 
#define LLF_RND_TRNG_MAX_TIME_SCALE         6   /* scaling down by 64 */
#define LLF_RND_TRNG_MAX_TIME_COEFF       128   /* preferable set value as power of 2  */
#define LLF_RND_TRNG_VON_NEUMAN_COEFF	    4   /* increases time because part of bits are rejected */


/* RNG interrupt masks */
/* on 'slow' DMA mode masked bits:  EHR_VALID, CTRNGT; 
   unmasked:  AutoCorrT, VNT and RNG_DMA_DONE, other bits masked: 0xFFFFFFC5*/
#define LLF_RNG_INT_MASK_ON_SLOW_MODE  0xFFFFFFC5     
					      
/* on 'fast' mode:  masked all bits besides b'5 - RNG_DMA_DONE: 0xFFFFFFDF   */
#define LLF_RNG_INT_MASK_ON_FAST_MODE  0xFFFFFFDF        

/* TRNG errors mask - masking all bits besides TRNG errors: AutoCorr + VN +   * 
*  Watchdog     							      */
#define LLF_RNG_ERRORS_MASK	\
                ((1UL<<DX_RNG_IMR_AUTOCORR_ERR_INT_MASK_BIT_SHIFT) | \
		 (1UL<<DX_RNG_IMR_VN_ERR_INT_MASK_BIT_SHIFT)       | \
		 (1UL<<DX_RNG_IMR_WATCHDOG_INT_MASK_BIT_SHIFT)) 

/* auxilary defines */
#define DX_SEP_HW_RESET_SEED_OVERRIDE_FLAG     0x2Ul

    

/*********************** Macros ********************************/

/* This busy loop waits for the valid bit to be set to 0x1 */
#define LLF_RND_HW_WAIT_VALID_EHR_BIT( void ) \
do \
{ \
   volatile DxUint32_t output_reg_val; \
   for(output_reg_val = 0; output_reg_val < 2 ; output_reg_val++);\
   do \
   { \
	output_reg_val = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(RNG,TRNG_VALID)); \
   }while( output_reg_val == 0x0 ); \
}while(0)  


/* Macros for indirect access to SRAM */

/* defining a macro to clear SRAM memory */
#define LLF_RND_HW_CLEAR_SRAM( SramOffset , SizeWords ) \
do \
{ \
   DxUint32_t ii1; \
   volatile DxUint32_t dummy; \
   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_ADDR) ,(SramOffset + 4*ii1) ); \
   for( ii1 = 0 ; ii1 < (DxUint32_t)(SizeWords) ; ii1++ ) \
   { \
      DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_DATA), 0x0UL ); \
      do \
      { \
	 dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
      }while(!(dummy & 0x1)); \
   }\
}while(0)   


#ifdef __cplusplus
}
#endif

#endif


