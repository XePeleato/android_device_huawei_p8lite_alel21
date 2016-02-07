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

#ifndef  _DX_BLOB_GEN_H
#define  _DX_BLOB_GEN_H


#include "dx_cclib.h"
/* The blob is created in the CC44 Secure environment, but should share definitions with CC44 Public */
#include "blob_defs.h"


/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

typedef DxUint8_t CRYS_BLOB_Message_t[DX_MAX_BLOB_SIZE_IN_BYTES];
typedef DxUint8_t CRYS_BLOB_nonce_t[DX_SIZE_OF_NONCE];

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*! 
 * @brief Create a blob in the CC44 secure environment with the session key. 
 * The blob is built of the AES encrypted key and the restriction data with 
 * authentication (by AES-CCM).
 * 
 * @param[in] encryptFlag	- An enum parameter, defines Encrypt operation (0) or a Decrypt operation (1).
 * @param[in] aesMode		- An enum parameter, defines AES Operation mode.
 * @param[in] baseAddr		- The blob base address.
 * @param[in] tailAddr		- The blob tail address.
 * @param[in] nonce		- A pointer to Nonce - unique value assigned to all data passed into CCM.
 * @param[in] userKeyPlain	- A pointer to the input text data buffer. The pointer does not need to be aligned.
 * @param[in] userKeySize	- An enum parameter, defines CRYS_AES_Key128BitSize or CRYS_AES_Key256BitSize.
 * @param[out] blobBuf		- A pointer to the generated blob message. 
 * @param[out] pBlobSize	- The size of the generated blob message.

 * \return DX_CclibRetCode_t one of the error codes defined in dx_cclib.h
 */
DX_CclibRetCode_t DX_CreateBlob(CRYS_AES_EncryptMode_t encryptFlag, enum sep_special_aes_mode aesMode,
				DxUint32_t baseAddr, DxUint32_t tailAddr, CRYS_BLOB_nonce_t nonce,
				DxUint8_t *userKey, CRYS_AES_KeySize_t userKeySize,
				CRYS_BLOB_Message_t blobBuf, DxUint32_t *pBlobSize);


/******************************************************************************
*				DEFINITIONS
******************************************************************************/

#define DX_BLOB_TOKEN_OFFSET		(DX_SEP_BLOB_TOKEN_OFFSET*sizeof(DxUint32_t))
#define DX_BLOB_NONCE_OFFSET		(DX_SEP_BLOB_NONCE_OFFSET*sizeof(DxUint32_t))
#define DX_BLOB_MODE_OFFSET		(DX_SEP_BLOB_DIR_MODE_OFFSET*sizeof(DxUint32_t))
#define DX_BLOB_MODE_SHIFT		DX_SEP_BLOB_MODE_SHIFT
#define DX_BLOB_BASE_ADDR_OFFSET	(DX_SEP_BLOB_BASE_OFFSET*sizeof(DxUint32_t))
#define DX_BLOB_TAIL_ADDR_OFFSET	(DX_SEP_BLOB_TAIL_OFFSET*sizeof(DxUint32_t))
#define DX_BLOB_KEY_OFFSET		(DX_SEP_BLOB_KEY_OFFSET*sizeof(DxUint32_t))

#define DX_BLOB_MIN_KEY_SIZE_IN_BYTES CRYS_AES_BLOCK_SIZE_IN_BYTES
#define DX_BLOB_MAX_KEY_SIZE_IN_BYTES CRYS_AES_BLOCK_SIZE_IN_BYTES*2

#define DX_ADATA_DIR_OFFSET		0
#define DX_ADATA_MODE_OFFSET		1
#define DX_ADATA_BASE_OFFSET		2 
#define DX_ADATA_TAIL_OFFSET		6 
#define DX_ADATA_TOKEN_OFFSET		10
#define DX_ADATA_SIZE_IN_BYTES		14	

#endif /* _DX_BLOB_GEN_H */

