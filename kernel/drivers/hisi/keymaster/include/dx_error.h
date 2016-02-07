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

#ifndef __DX_ERROR_H__
#define __DX_ERROR_H__

#ifdef __KERNEL__
//#include <linux/types.h>
#ifndef INT32_MAX
#define INT32_MAX 0x7FFFFFFFL
#endif
#else
//#include <stdint.h>
#endif

typedef enum DxSymRetCode {
	DX_RET_OK = 0, /* No error */
	DX_RET_UNSUPP_ALG, /* Unsupported algorithm */
	DX_RET_UNSUPP_ALG_MODE, /* Unsupported algorithm mode */
	DX_RET_UNSUPP_OPERATION, /* Unsupported operation */
	DX_RET_INVARG, /* Invalid parameter */
	DX_RET_INVARG_QID, /* Invalid queue ID */
	DX_RET_INVARG_KEY_SIZE, /* Invalid key size */
	DX_RET_INVARG_CTX_IDX, /* Invalid context index */
	DX_RET_INVARG_CTX, /* Bad or corrupted context */
	DX_RET_INVARG_BAD_ADDR, /* Bad address */
	DX_RET_INVARG_INCONSIST_DMA_TYPE, /* DIN is inconsist with DOUT DMA type */
	DX_RET_PERM, /* Operation not permitted */
	DX_RET_NOEXEC, /* Execution format error */
	DX_RET_BUSY, /* Resource busy */
	DX_RET_NOMEM, /* Out of memory */
	DX_RET_OSFAULT, /* Internal OS error */
	SEPSYMCRYPTO_RET_RESERVE32 = INT32_MAX /* assure this enum is 32b */
}DxSymRetCode_t;


#endif /*__DX_ERROR_H__*/
