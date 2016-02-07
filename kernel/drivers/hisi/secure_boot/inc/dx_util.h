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

#ifndef  _DX_UTIL_H
#define  _DX_UTIL_H

#include "dx_cc_defs.h"

/******************************************************************************
*                        	DEFINITIONS
******************************************************************************/

#define DX_UTIL_CMAC_DERV_MIN_DATA_IN_SIZE	0x8UL    
#define DX_UTIL_CMAC_DERV_MAX_DATA_IN_SIZE	0x400UL   
#define DX_UTIL_CMAC_DERV_DATA_OUT_SIZE		0x10UL    


typedef DxUint32_t DxUTILError_t;

#define DX_UTIL_OK		0x00UL

#define DX_UTIL_MODULE_ERROR_BASE 0x80000000

/* The CMAC derive module errors base address - 0x80000000 */
#define DX_UTIL_INVALID_KEY_TYPE             		(DX_UTIL_MODULE_ERROR_BASE + 0x00UL)
#define DX_UTIL_DATA_IN_POINTER_INVALID_ERROR		(DX_UTIL_MODULE_ERROR_BASE + 0x01UL)
#define DX_UTIL_DATA_IN_SIZE_INVALID_ERROR		(DX_UTIL_MODULE_ERROR_BASE + 0x02UL)
#define DX_UTIL_DATA_OUT_POINTER_INVALID_ERROR		(DX_UTIL_MODULE_ERROR_BASE + 0x03UL)
#define DX_UTIL_DATA_OUT_SIZE_INVALID_ERROR		(DX_UTIL_MODULE_ERROR_BASE + 0x04UL)
#define DX_UTIL_FATAL_ERROR				(DX_UTIL_MODULE_ERROR_BASE + 0x05UL)
#define DX_UTIL_ILLEGAL_PARAMS_ERROR			(DX_UTIL_MODULE_ERROR_BASE + 0x06UL)
#define DX_UTIL_BAD_ADDR_ERROR				(DX_UTIL_MODULE_ERROR_BASE + 0x07UL)


/*!  
 * This function is used to generate bytes stream for key derivation purposes. 
 * The function gets an input data and can use use one of the following keys: KDR, Session. 
 * 
 * @param[in] aesKeyType	- 1 (RKEK); 5 (Session)
 * @param[in] pDataIn 		- a pointer to input buffer 
 * @param[in] blockSize 	- size of data in bytes
 * @param[out] pCMacResult 	- a pointer to output buffer
 * @param[in] pCMacResultSize 	- output buffer size, must equal to DX_UTIL_CMAC_DERV_DATA_OUT_SIZE 
 *  
 * @return DX_UTIL_OK on success, otherwise failure 
 *  
 */
DxUTILError_t DX_UTIL_CmacDeriveKey(DX_CRYPTO_KEY_TYPE_t	aesKeyType, 
			            DxUint8_t  			*pDataIn,
			            DxUint32_t  		DataInSize,
			            DxUint8_t   		*pCmacResult,
				    DxUint32_t			cmacResultSize);

#endif /*_DX_UTIL_H*/
