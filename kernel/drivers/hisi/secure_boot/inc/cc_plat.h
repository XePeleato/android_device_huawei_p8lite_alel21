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

#ifndef  CC_PLAT_H
#define  CC_PLAT_H

#define CURR_TASK_ID() (0) /*single task -single queue*/
#define CURR_QUEUE_ID() CURR_TASK_ID()
#define IS_SCHEDULER_RUNNING() (1) /*in signle task application always busy*/


#include "dx_pal_types_plat.h"

/******************************************************************/
/******************************************************************/
/* The below MACROS are used by the driver to access the context. */
/* Since the context is in the SRAM it must use indirect access to*/
/* the CC internal SRAM.                                          */
/******************************************************************/
/******************************************************************/
#define _WriteWordsToSram(addr , data , size ) \
do { \
	uint32_t ii; \
	volatile uint32_t dummy; \
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_ADDR),((uint32_t)addr) ); \
	for( ii = 0 ; ii < size/sizeof(uint32_t) ; ii++ ) { \
		   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_DATA), SWAP_TO_LE(((DxUint32_t*)data)[ii])); \
		   do { \
		     dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
		   }while(!(dummy & 0x1)); \
	} \
}while(0)  



#define _ClearSram(addr , size ) \
do { \
	uint32_t ii; \
	volatile uint32_t dummy; \
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_ADDR) ,((uint32_t)addr) ); \
	for( ii = 0 ; ii < size/sizeof(uint32_t) ; ii++ ) { \
		DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_DATA), 0x0UL ); \
		do { \
			dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
		}while(!(dummy & 0x1)); \
	}\
}while(0)   


#define _ReadValueFromSram(addr , Val ) \
do { \
	volatile uint32_t dummy; \
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_ADDR) ,(((uint32_t)addr)) ); \
	dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_DATA)); \
	do { \
		dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
	}while(!(dummy & 0x1)); \
	dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_DATA)); \
	(Val) = SWAP_TO_LE(dummy);\
	do { \
		dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY) ); \
	}while(!(dummy & 0x1)); \
}while(0)   


#define _ReadWordsFromSram( addr , data , size ) \
do { \
	uint32_t ii; \
	volatile uint32_t dummy; \
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_ADDR) ,((uint32_t)addr) ); \
	dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_DATA)); \
	for( ii = 0 ; ii < size/sizeof(uint32_t) ; ii++ ) { \
		do { \
			dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
		}while(!(dummy & 0x1)); \
		dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF,SRAM_DATA));\
		((uint32_t*)data)[ii] = SWAP_TO_LE(dummy); \
	} \
	do { \
		dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
	}while(!(dummy & 0x1)); \
}while(0)   




/****************************************************************************************/
/**
 * 
 * @brief The function gets one word from the context.
 * 
 * 
 * @param[in] addr - The address of the word ( pointer to a word in the context).
 *
 * @return uint32_t - The value of that address.  
 */
uint32_t ReadContextWord(void* addr);


/****************************************************************************************/
/**
 * 
 * @brief The function writes one word to the context.
 * 
 * 
 * @param[in] addr - The address of the word ( pointer to a word in the context).
 *
 * @param[in] data - The vaule to be written.
 *
 * @return void.
 */
void WriteContextWord(void* addr,uint32_t data);

/****************************************************************************************/
/**
 * 
 * @brief The function clears field in the context.
 * 
 * 
 * @param[in] addr - The address of the field ( pointer to the field in the context).
 *
 * @param[in] size - The size of the field in bytes.
 *
 * @return void.
 */
void ClearCtxField(void* addr, uint32_t size);

/****************************************************************************************/
/**
 * 
 * @brief The function update array field in the context (more than one word).
 * 
 * 
 * @param[in] addr - The address of the field ( pointer to the field in the context).
 *
 * @param[in] data - The data to write to the field.
 *
 * @param[in] size - The size of the field in bytes.
 *
 * @return void.
 */
void WriteContextField(void *addr,const uint32_t* data, uint32_t size );

/****************************************************************************************/
/**
 * 
 * @brief The function reads array field in the context (more than one word).
 * 
 * 
 * @param[in] addr - The address of the field ( pointer to the field in the context).
 *
 * @param[in] data - buffer to read the data into.
 *
 * @param[in] size - The size of the field in bytes.
 *
 * @return void.
 */
void ReadContextField(void *addr, uint32_t* data, uint32_t size );
#endif
