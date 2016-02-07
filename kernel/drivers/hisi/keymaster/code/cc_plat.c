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

#include "dx_pal_types.h"
#include "dx_pal_log.h"
#include "dx_cclib.h"
#include "dx_hal.h"
#include "cc_plat.h"
#include "dx_cc_regs.h"

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
uint32_t ReadContextWord(void *addr )
{
	uint32_t val;
	_ReadValueFromSram((uint32_t*)addr , val );
	return val;
}

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
void WriteContextWord(void *addr,uint32_t data)
{
	_WriteWordsToSram((uint32_t*)addr ,&data , 4);
}

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
void ClearCtxField(void* addr, uint32_t size)
{
	_ClearSram((uint32_t*)addr , size );
}

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
void ReadContextField(void *addr, uint32_t* buff, uint32_t size )
{
	_ReadWordsFromSram((uint32_t*)addr , buff , size );
}

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
void WriteContextField(void *addr,const uint32_t* data, uint32_t size )
{
	_WriteWordsToSram((uint32_t*)addr , data , size );
}
