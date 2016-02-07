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



#ifndef _SBROM_CRYPTO_DRIVER_H
#define _SBROM_CRYPTO_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!  
 * This function is used to generate bytes stream for key derivation purposes. 
 * The function gets an input data and can use use one of the following keys: KDR, Provision, Session, or internal. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] aesKeyType	- 0 (Internal); 1 (RKEK); 2 (Provision); 3 (Session)
 * @param[in] pInternalKey	- a pointer to internal key
 * @param[in] pDataIn 		- a pointer to input buffer
 * @param[in] dmaMode		- 0 (reserved); 1 (SRAM); 2 (DLLI); 3 (reserved) 
 * @param[in] blockSize 	- size of data in bytes
 * @param[in] pCMacResult 	- a pointer to output buffer
 *  
 */
void SBROM_AesCmac(DxUint32_t hwBaseAddress, HwCryptoKey_t aesKeyType, DxUint32_t *pInternalKey, DxUint32_t *pDataIn, DmaMode_t dmaMode, DxUint32_t  blockSize, DxUint32_t *pCMacResult);


/*!  
 * This function sets zeroes to the AES engine key. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address  
 *  
 */
void SBROM_AesLockEngineKey(DxUint32_t hwBaseAddress);

/*!  
 * This function is used to perform bypass operation. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address  
 * @param[in] pDataIn 		- a pointer to input buffer 
 * @param[in] dmaModeIn		- 0 (reserved); 1 (SRAM); 2 (DLLI); 3 (reserved) 
 * @param[in] pDataOut 		- a pointer to output buffer 
 * @param[in] dmaModeOut	- 0 (reserved); 1 (SRAM); 2 (DLLI); 3 (reserved) 
 * @param[in] blockSize 	- size of data in bytes
 *  
 */
void SBROM_Bypass(DxUint32_t hwBaseAddress, DxUint32_t *pDataIn, DmaMode_t  dmaModeIn, DxUint32_t *pDataOut, DmaMode_t  dmaModeOut, DxUint32_t blockSize);


#ifdef __cplusplus
}
#endif

#endif



