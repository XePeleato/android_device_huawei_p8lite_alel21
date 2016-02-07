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

#ifndef __KEY_OBJECT_H__
#define __KEY_OBJECT_H__
/*
 * This header file defines the API for the Key object. 
 */

/* Object container size */
#define DX_KEY_OBJ_CONTAINER_SIZE 12
#define DX_KEY_OBJ_CONTAINER_SIZE_WORDS (DX_KEY_OBJ_CONTAINER_SIZE >> 2)

/* The Key object handle */
typedef uint32_t DX_KeyObjHandle_t;
#define DX_KEY_OBJ_INVALID_HANDLE 0

/* The Key object container - to be allocated by caller */
typedef uint32_t DX_KeyObjContainer_t[DX_KEY_OBJ_CONTAINER_SIZE_WORDS];

/*!
 * Create a Key object for key buffer for user key in SRAM
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \param aKeyAddr user key pointer
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateSepUserKey(
	DX_KeyObjContainer_t container,
	uint32_t aKeyAddr);

/*!
 * Create a Key object for key buffer for user key in HOST
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \param aKeyAddr user key pointer
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateHostUserKey(
	DX_KeyObjContainer_t container,
	uint32_t aKeyAddr);

/*!
 * Create a Key object for key buffer for ROOT key 
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateRkekKey(DX_KeyObjContainer_t container);

/*!
 * Create a Key object for key buffer for APPLET key 
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateAppletKey(DX_KeyObjContainer_t container);

/*!
 * Create a Key object for key buffer for Provisioning key
 * 
 * \param container A memory allocated by the caller to accomodate the object
 * \return DX_KeyObjHandle_t The created object handle
 */
DX_KeyObjHandle_t DX_KeyObjCreateProvKey(DX_KeyObjContainer_t container);

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
	uint32_t aKeyAddr);

/*!
 * Free resources of given KEY object. 
 * This function must be invoked before freeing or reusing the object container 
 * 
 * \param objHandle The KEY object handle
 */
void DX_KeyObjDestroy(DX_KeyObjHandle_t objHandle);



#endif /*__KEY_OBJECT_H__*/
