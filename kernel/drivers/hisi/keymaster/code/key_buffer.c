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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_INFRA

#include "crys_aes.h"
#include "sym_adaptor_driver.h"
#include "key_buffer.h"
#include "crys_aes_error.h"
#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "dx_macros.h"
#include "cipher.h"


#if (DX_DSCRPTR_QUEUE0_WORD3_NS_BIT_BIT_SHIFT != DX_DSCRPTR_QUEUE0_WORD1_NS_BIT_BIT_SHIFT) || (DX_DSCRPTR_QUEUE0_WORD1_DIN_VIRTUAL_HOST_BIT_SHIFT != DX_DSCRPTR_QUEUE0_WORD3_DOUT_VIRTUAL_HOST_BIT_SHIFT)
#error AxiId/NS-bit fields mismatch between DIN and DOUT - functions need to be updated...
#endif 


/*
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
int getKeyDataFromKeyObj(uint8_t *keyObj, uint32_t *keyAddr, DX_CRYPTO_KEY_TYPE_t *cryptoKeyType, KeyPtrType_t *keyPtrType, enum dx_data_key_obj_api cryptoObjApi)
{

	/* in the CC44 we use only user key in DLLI mode */
	*keyAddr = (uint32_t)keyObj;
	*cryptoKeyType = DX_USER_KEY;
	*keyPtrType = KEY_BUF_DLLI;

	return 0;
}


int buildKeyInt(struct sep_ctx_cipher *AesContext, CRYS_AES_KeySize_t *KeySize, keyBuffer_t keyBuff,
		uint32_t *keyAddr, enum dx_data_key_obj_api cryptoObjApi, DX_CRYPTO_KEY_TYPE_t cryptoKeyType, DxUint32_t *keySizeBytes)
{


	/*in tee when calling the function from CMAC */
	/* integrated mode, do nothing		    */
	if (cryptoObjApi == DX_AES_CMAC)
		return 0;

	switch (cryptoObjApi)
	{

		case DX_AES_API:
		case DX_AES_API_INIT:
		case DX_AES_WRAP_API:
		case DX_AES_CMAC:
			/* get AES_Key size in bytes */
			switch( *KeySize )
			{
				case CRYS_AES_Key128BitSize:
					*keySizeBytes = 16;
					break;
		
				case CRYS_AES_Key192BitSize:
					*keySizeBytes = 24;
					break;
		
				case CRYS_AES_Key256BitSize:
					*keySizeBytes = 32;
					break;
		
				default:
					return  CRYS_AES_WRAP_KEY_LENGTH_ERROR; /*for preventing compiler warnings*/
			}
			DX_PAL_MemCopy(AesContext->key, (uint8_t*)*keyAddr , *keySizeBytes );
		break;
		default:
			/*do nothing*/ 
			return 1;
	}
	return 0;

}

