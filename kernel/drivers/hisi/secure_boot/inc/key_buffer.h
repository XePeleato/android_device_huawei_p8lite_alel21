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

#ifndef __KEY_BUFFER_H__
#define __KEY_BUFFER_H__

#include "key_buffer_plat.h"


#define KDF_LABEL_SIZE 4
#define KDF_CONTEXT_SIZE 16


typedef enum KeyPtrType {
	KEY_BUF_NULL,
	KEY_BUF_SEP,
	KEY_BUF_DLLI        	
} KeyPtrType_t;


typedef struct KeyBuffer {
        uint8_t *pKey; /*A pointer to the key data. May be SRAM/DCACHE/ICACHE/DLLI */
        DX_CRYPTO_KEY_TYPE_t cryptoKeyType;
        KeyPtrType_t keyPtrType; 
} KeyBuffer_s;

/*Key derivation function (KDF) struct according to nist 800-108*/
typedef struct KdfInputBuff {
	uint8_t iterationCount;        
	uint8_t label[KDF_LABEL_SIZE];
	uint8_t separator;
	uint8_t context[KDF_CONTEXT_SIZE];
	uint8_t kdfOutSize[sizeof(uint16_t)];
} KdfInputBuff_s;


/*!
 * Parse user buffer information that may be smart key pointer (key object) 
 * Return uniform Key information 
 *  
 *  
 * \param [in]  keyObj - the key buffer  
 * \param [out] keyAddr - key pointer
 * \param [out] cryptoKeyType - type of key (ROOT, USER,PROVISIONING ...) 
 * \param [out] keyPtrType  - type of pointer (SRAM ptr, DCAHE ptr, DLLI ptr)
 *  
 * \return 0 on success, (-1) on error 
 */
int getKeyDataFromKeyObj(uint8_t *keyObj, uint32_t *keyAddr, DX_CRYPTO_KEY_TYPE_t *cryptoKeyType, KeyPtrType_t *keyPtrType, enum dx_data_key_obj_api cryptoObjApi);

/*!
 * Build Key internal 
 *  
 * \param  pAesContext - aes context for AES_CMAC operation 
 * \param  KeySize - the key size
 * \param  keyAddr - key pointer 
 * \param  cryptoObjApi  - the API we are coming from.
 * \param  cryptoKeyType - type of key (ROOT, USER,PROVISIONING ...) 
 * \param  keySizeBytes  - the Of the key, in bytes) 
 *  
 */
int buildKeyInt(struct sep_ctx_cipher *AesContext, CRYS_AES_KeySize_t *KeySize, keyBuffer_t keyBuff, uint32_t *keyAddr, enum dx_data_key_obj_api cryptoObjApi, DX_CRYPTO_KEY_TYPE_t cryptoKeyType, DxUint32_t *keySizeBytes);

#endif /*__KEY_BUFFER_H__*/
