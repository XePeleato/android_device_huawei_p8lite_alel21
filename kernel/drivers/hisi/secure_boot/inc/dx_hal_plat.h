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


#ifndef __DX_HAL_PLAT_H__
#define __DX_HAL_PLAT_H__

#include "log.h"
#include "dx_pal_types.h"
#include "dx_pal_abort.h"
#include "dx_host.h"
#include "dx_bitops.h"

#include "dx_reg_common.h" /*temporary (missing HW defines)*/


/******************************************************************************
*				DEFINITIONS
******************************************************************************/
#define DX_LARGE_SECRET_KEY_NUM_OF_BYTES 32
#define DX_SMALL_SECRET_KEY_NUM_OF_BYTES 16


#define DX_HAL_ALIGN_OK                        (0x0UL)
#define DX_HAL_ALIGN_INVALID_INPUT             (0x1UL)

/******************************************************************************
*                               MACROS
******************************************************************************/
extern uint32_t gCcRegBase;

/******************************************************************************
*                               MACROS
******************************************************************************/
/*get the size of the RKEK from HW */
//(key_size >> DX_NVM_CC_BOOT_LARGE_RKEK_LOCAL_BIT_SHIFT) & DX_NVM_CC_BOOT_LARGE_RKEK_LOCAL_BIT_SIZE
#define GET_ROOT_KEY_SIZE(key_size) \
do{ \
	key_size = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL, NVM_CC_BOOT));\
        if (DX_CC_REG_FLD_GET(CRY_KERNEL, NVM_CC_BOOT, LARGE_RKEK_LOCAL, key_size)) \
                key_size = DX_LARGE_SECRET_KEY_NUM_OF_BYTES; \
        else \
                key_size = DX_SMALL_SECRET_KEY_NUM_OF_BYTES; \
}while (0)


/*!
 * Read CryptoCell memory-mapped-IO register.
 * 
 * \param regOffset The offset of the CC register to read
 * \return uint32_t Return the value of the given register
 */
#define DX_HAL_ReadCcRegister(regOffset) 				\
		(*((volatile uint32_t *)(gCcRegBase + (regOffset))))

/*!
 * Write CryptoCell memory-mapped-IO register.
 *  
 * \param regOffset The offset of the CC register to write
 * \param val The value to write
 */
#define DX_HAL_WriteCcRegister(regOffset, val) 		\
		(*((volatile uint32_t *)(gCcRegBase + (regOffset))) = (val))



#endif /*__DX_HAL_PLAT_H__*/

