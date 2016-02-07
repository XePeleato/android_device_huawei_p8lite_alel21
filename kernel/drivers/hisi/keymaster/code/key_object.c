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

#include "dx_pal_types.h"
#include "sep_ctx.h"
#include "crys_aes.h"
#include "key_buffer.h"
#include "key_object.h"
#include "compiler.h"


DX_PAL_COMPILER_ASSERT(sizeof(KeyBuffer_s) <= sizeof(DX_KeyObjContainer_t), "Container size is too small");

#define HANDLE_TO_OBJ(ptr)  PTR_TO_KEY_BUFFER(ptr)
#define OBJ_TO_HANDLE(pObj) KEY_BUFFER_TO_PTR(pObj)

/*!
 * Create a Key object for key buffer for user key in SRAM
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \param aKeyAddr user key pointer
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateSepUserKey(
	DX_KeyObjContainer_t container,
	uint32_t aKeyAddr)
{
	KeyBuffer_s *newObj = (KeyBuffer_s *)container;

	newObj->cryptoKeyType = DX_USER_KEY;
	newObj->pKey = (uint8_t*)aKeyAddr;
	newObj->keyPtrType = KEY_BUF_SEP;
	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Create a Key object for key buffer for user key in HOST
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \param aKeyAddr user key pointer
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateHostUserKey(
	DX_KeyObjContainer_t container,
	uint32_t aKeyAddr)
{
	KeyBuffer_s *newObj = (KeyBuffer_s *)container;

	newObj->cryptoKeyType = DX_USER_KEY;
	newObj->pKey = (uint8_t*)aKeyAddr;
	newObj->keyPtrType = KEY_BUF_DLLI;
	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Create a Key object for key buffer for ROOT key 
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateRkekKey(DX_KeyObjContainer_t container)
{
	KeyBuffer_s *newObj = (KeyBuffer_s *)container;

	newObj->cryptoKeyType = DX_ROOT_KEY;
	newObj->pKey = DX_NULL;
	newObj->keyPtrType = KEY_BUF_NULL;
	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Create a Key object for key buffer for APPLET key 
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateAppletKey(DX_KeyObjContainer_t container)
{
	KeyBuffer_s *newObj = (KeyBuffer_s *)container;

	newObj->cryptoKeyType = DX_APPLET_KEY;
	newObj->pKey = DX_NULL;
	newObj->keyPtrType = KEY_BUF_NULL;
	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Create a Key object for key buffer for Provisioning key
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateProvKey(DX_KeyObjContainer_t container)
{
	KeyBuffer_s *newObj = (KeyBuffer_s *)container;

	newObj->cryptoKeyType = DX_PROVISIONING_KEY;
	newObj->pKey = DX_NULL;
	newObj->keyPtrType = KEY_BUF_NULL;
	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Create a Key object for key buffer for HDCP key to be XORed 
 * with the CRYPTO-KEY register. 
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \param aKeyAddr user key pointer
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateHdcpKey(
	DX_KeyObjContainer_t container,
	uint32_t aKeyAddr)
{
	KeyBuffer_s *newObj = (KeyBuffer_s *)container;

	newObj->cryptoKeyType = DX_XOR_HDCP_KEY;
	newObj->pKey = (uint8_t*)aKeyAddr;
	newObj->keyPtrType = KEY_BUF_SEP;
	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Create a Key object for key buffer for SESSION key
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateSessionKey(DX_KeyObjContainer_t container)
{
	KeyBuffer_s *newObj = (KeyBuffer_s *)container;

	newObj->cryptoKeyType = DX_SESSION_KEY;
	newObj->pKey = DX_NULL;
	newObj->keyPtrType = KEY_BUF_NULL;
	return OBJ_TO_HANDLE(newObj);
}

/*!
 * Free resources of given KEY object. 
 * This function must be invoked before freeing or reusing the object container 
 * 
 * \param objHandle The KEY object handle
 */
void DX_KeyObjDestroy(DX_KeyObjHandle_t objHandle)
{
	/* Nothing to do today - just a place holder */
}
