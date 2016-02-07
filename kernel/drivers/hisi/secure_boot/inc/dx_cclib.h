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


#ifndef __DX_CCLIB_H__
#define __DX_CCLIB_H__

#include "dx_pal_types.h"

typedef enum {
	DX_CCLIB_RET_OK = 0,
	DX_CCLIB_RET_ENODEV,        /* Device not opened or does not exist */
	DX_CCLIB_RET_EINTERNAL,     /* Internal driver error (check system log) */
	DX_CCLIB_RET_ENOTSUP,       /* Unsupported function/option */
	DX_CCLIB_RET_ENOPERM,       /* Not enough permissions for request */
	DX_CCLIB_RET_EINVAL,        /* Invalid parameters */
	DX_CCLIB_RET_HW_Q_INIT,
	DX_CCLIB_RET_COMPLETION,
	DX_CCLIB_RET_HAL,
	DX_CCLIB_RESERVE32B = 0x7FFFFFFFL
} DX_CclibRetCode_t;

#define DXDI_DEV_PRIO0 "/dev/dx_sep_q0"
#define DXDI_DEV_PRIO1 "/dev/dx_sep_q1"

typedef DxUint32_t DX_CclibDevPriority;

/*!
 * Init OS driver context
 * This function must be invoked before any other CC library API function
 * but for the functions which deal with library initialization properties:
 * DX_CclibSetPriority, DX_CclibGetPriority, DX_CclibGetMaxPriority
 * 
 * \return DxCclib_RetCode
 */
DX_CclibRetCode_t DX_CclibInit(void);

/*!
 * Finalize OS driver context - release associated resources
 */
void DX_CclibFini(void);

/*!
 * Set requested CryptoCell priority queue 
 * This function must be invoked before DX_CclibInit
 * 
 * \param priority Requested priority queue
 *
 * \return DxCclib_RetCode
 */
DX_CclibRetCode_t DX_CclibSetPriority(DX_CclibDevPriority priority);

/*!
 * Get selected priority 
 * (as set by DX_CclibSetPriority or default if it was not invoked)
 *
 * 
 * \return DX_CclibDevPriority
 */
DX_CclibDevPriority DX_CclibGetPriority(void);

/*!
 * Get the highest priority supported 
 *
 * \return DX_CclibDevPriority
 */
DX_CclibDevPriority DX_CclibGetMaxPriority(void);



#endif /*__DX_CCLIB_H__*/

