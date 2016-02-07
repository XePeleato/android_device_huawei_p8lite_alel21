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
#include "secureboot_general_hwdefs.h"
#include "systemdep.h"
#include "sb_hash.h"
#include "llf_hash.h"
#include "llf_hash_hwdefs.h"
#include "util.h"


/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************* Private function prototype ****************/
static DxUint32_t UTIL_INVERSE_UINT32_BYTES (DxUint32_t val)
{
#ifdef BIG__ENDIAN
  return  (val);
#else
return  ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24));
#endif
}

static DxUint32_t UTIL_REVERT_UINT32_BYTES (DxUint32_t val)
{
#ifdef BIG__ENDIAN
return  ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) );
#else
  return  (val);
#endif
}

/************************ Public Functions ******************************/

void  LLF_HASH_Init_Crypto ( DxUint32_t hwBaseAddr )
{
/* LOCAL DECLERATIONS */

/* FUNCTION LOGIC */

/* ................. general registers initializations ..................... */
/* ------------------------------------------------------------------------- */ 
/* Setting clock to enable at the beginning of the operation */
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr, HASH_CLK_ENABLE) ,0x01UL );

/* setting the CRYPTO_CTL register to HASH mode - the machine is now configured to work on HASH */
SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr, CRYPTO_CTL) , HASH_HW_CRYPTO_CTL_HASH_MODE_VAL );

/* do not enable the padding */
SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr,HASH_PAD_EN), 0UL );

/* .................. HASH registers initialization ......................... */
/* -------------------------------------------------------------------------- */ 

/* ....... setting the HASH control register to SHA1 mode */

/* setting the register HASH control register to SHA1 mode */ 
SYS_WriteRegister(SB_REG_ADDR( hwBaseAddr, HASH_CONTROL) , HASH_HW_HASH_CTL_SHA256_VAL );

/* ................ waiting until the HW machine is enabled */

HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);
}
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
void  LLF_HASH_Init ( DxUint32_t hwBaseAddr )
{
/* LOCAL DECLERATIONS */

/* FUNCTION LOGIC */

/* ................. general registers initializations ..................... */
/* ------------------------------------------------------------------------- */ 
/* Setting clock to enable at the beginning of the operation */
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr, HASH_CLK_ENABLE) ,0x01UL );

/* setting the CRYPTO_CTL register to HASH mode - the machine is now configured to work on HASH */
SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr, CRYPTO_CTL) , HASH_HW_CRYPTO_CTL_HASH_MODE_VAL );

/* do not enable the padding */
SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr,HASH_PAD_EN), 0UL );

/* .................. HASH registers initialization ......................... */
/* -------------------------------------------------------------------------- */ 

/* ....... setting the HASH control register to SHA1 mode */

/* setting the register HASH control register to SHA1 mode */ 
SYS_WriteRegister(SB_REG_ADDR( hwBaseAddr, HASH_CONTROL) , HASH_HW_HASH_CTL_SHA256_VAL );

/* ................ waiting until the HW machine is enabled */

HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);

/* ................ setting the Hinit registers ........... */

/* on the SHA1 mode: load the HASH_H4 - HASH_H0 register */
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_H7) , HASH_SHA256_INIT_7);
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_H6) , HASH_SHA256_INIT_6);
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_H5) , HASH_SHA256_INIT_5);
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_H4) , HASH_SHA256_INIT_4);
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_H3) , HASH_SHA256_INIT_3);
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_H2) , HASH_SHA256_INIT_2);
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_H1) , HASH_SHA256_INIT_1);                             
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_H0) , HASH_SHA256_INIT_0); 


/* ................ waiting until the HW machine is enabled */

HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);

return;

}/* END OF LLF_HASH_Init */                                           

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
			    DxUint32_t    useDma )
{ 
/* LOCAL DECLERATIONS */

/* loop variables */
int i;


/* FUNCTION LOGIC */

/* ........................ updating the data ........................... */
/* ---------------------------------------------------------------------- */

/* if it is the last operation enble the padding */
if ( TotalSizeInWords ) {

	SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr,HASH_PAD_EN),HASH_HW_HASH_PAD_EN_VAL);

	/* load the current length of message being processed  - load high and low word for all cases */
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_CUR_LEN_0) ,(TotalSizeInWords - DataInSizeInWords)*sizeof(DxUint32_t));         
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,HASH_CUR_LEN_1) , 0UL );    

	HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);

}


/* .................... executing the DMA ............................... */
/* STEP2 : initialize the LLI */

if ( useDma ) {
#ifndef DX_DMA_DONE_IN_SRAM       
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,SRC_LLI_WORD0) , (DataIn_ptr) );//, (DxUint32_t)DataIn_ptr );
	/* initialize the source size and set the first & last LLI word - this triggers the operation */

	SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr,SRC_LLI_WORD1)  , 
			  (DataInSizeInWords * sizeof(DxUint32_t)) |
			  (1UL << HASH_HW_SRC_LLI_WORD1_FIRST_LLI_WORD_POS) | 
			  (1UL << HASH_HW_SRC_LLI_WORD1_LAST_LLI_WORD_POS) );

#else
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr,SRAM_SRC_ADDR) , (DataIn_ptr) );//
	/* initialize the source size and set the first & last LLI word - this triggers the operation */

	SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr,DIN_SRAM_BYTES_LEN)  , 
			  (DataInSizeInWords * sizeof(DxUint32_t)));

#endif /* DX_DMA_DONE_IN_SRAM */
	if ( PollFlag || TotalSizeInWords ) {
		/* STEP3 : wait on the DMA and HASH */
		HASH_HW_WAIT_ON_DMA_SRC_BUSY_BIT(hwBaseAddr);

		/* wait for the hardware machine to be ready */
		HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);

	}

}/* end of use DMA case */

else {

	/* loading the full blocks */
	for ( i = 0 ; i < DataInSizeInWords / 4 ; i++ ) {
		SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr, DIN_BUFFER) , UTIL_REVERT_UINT32_BYTES(*DataIn_ptr) );
		DataIn_ptr++;
		SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr, DIN_BUFFER) , UTIL_REVERT_UINT32_BYTES(*DataIn_ptr ));
		DataIn_ptr++;
		SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr, DIN_BUFFER) , UTIL_REVERT_UINT32_BYTES(*DataIn_ptr ));
		DataIn_ptr++;
		SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr, DIN_BUFFER) , UTIL_REVERT_UINT32_BYTES(*DataIn_ptr ));
		DataIn_ptr++;

		HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);

	}

	/* load the partial last block if required */
	if ( TotalSizeInWords ) {
		for ( i = 0 ; i < DataInSizeInWords % 4 ; i++ ) {
			SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr, DIN_BUFFER)  , UTIL_REVERT_UINT32_BYTES(*DataIn_ptr ));
			DataIn_ptr++;
		}
	}

}/* end of non DMA case */   


/* ....................... if it is the last packet load the remain data plus the padding .............. */
/* ----------------------------------------------------------------------------------------------------- */

if ( TotalSizeInWords  ) {

	HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);

	/* execute the padding */ 
	SYS_WriteRegister(SB_REG_ADDR(hwBaseAddr, HASH_PAD_CFG), HASH_HW_HASH_DO_PADDING_VAL);        

	HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);


}/* end of last block case */

return;

}/* END OF HASH_UpdateData */

/**
 * @brief The low level LLF_HASH_GetResults reads the result from the HASH_Hi registers
 * 
 * @param[in] hwBaseAddr        -  cryptocell base address
 *        
 * @param[in] result - The HASH result.
 *
 */                              
void  LLF_HASH_GetResults( DxUint32_t hwBaseAddr, HASH_Result_t Result )
{                         
/* LOCAL DECLERATIONS */

/* loop variable */
DxUint8_t i;  

/* FUNCTION LOGIC */

/* STEP 1 : reading the data stored in the hin registers */
/* ----------------------------------------------------- */

/* on the SHA1 mode read the HASH_H4 - HASH_H0 register */
SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_H7) , Result[7]);                               
SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_H6) , Result[6]);                               
SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_H5) , Result[5]);                               
SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_H4) , Result[4]);                               
SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_H3) , Result[3]);
SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_H2) , Result[2]);
SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_H1) , Result[1]);                             
SYS_ReadRegister( SB_REG_ADDR(hwBaseAddr, HASH_H0) , Result[0]); 

/* reverse the bytes */
for ( i = 0 ; i < HASH_RESULT_SIZE_IN_WORDS ; i++ )

	Result[i] = UTIL_INVERSE_UINT32_BYTES(Result[i]);

/* Setting clock to disable at the end of the operation */
SYS_WriteRegister( SB_REG_ADDR(hwBaseAddr, HASH_CLK_ENABLE) ,0x00UL );

return;

}/* END OF HASH_GetResults */  

/**
 * @brief The low level LLF_HASH_WAIT_ON_Busy waits on DMA busy and HASH busy
 *         
 * @param[in] hwBaseAddr        -  cryptocell base address
 *
 */
void LLF_HASH_WAIT_ON_HASHAndDMABusy(DxUint32_t hwBaseAddr)
{
/* wait on the DMA and HASH */
HASH_HW_WAIT_ON_DMA_SRC_BUSY_BIT(hwBaseAddr);

/* wait for the hardware machine to be ready */
HASH_HW_WAIT_ON_HASH_BUSY_BIT(hwBaseAddr);
}

