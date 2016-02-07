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



#ifndef _CRYPTO_DRIVER_AES_H
#define _CRYPTO_DRIVER_AES_H

#ifdef __cplusplus
extern "C"
{
#endif


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!  
 * This function is used to generate bytes stream for key 
 * derivation purposes. The function gets an input data (public key hash) 
 * and using the KCP as a secret key into the AES-CMAC crypto engine. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] aesKeyType	- 0 (reserved); 1 (RKEK); 2 (Provision); 3 (Session)
 * @param[in] dataIn 		- pointer to data buffer in host memory (BE/LE)
 * @param[in] sizeInWords 	- size of data input 
 * @param[in] pCMacResult 	- a pointer to the out buffer (LE)
 *  
 */
void AES_CmacKeyDerivation(DxUint32_t hwBaseAddress, DxUint32_t aesKeyType, DxUint32_t *dataIn, DxUint32_t  sizeInWords, DxUint32_t *pCMacResult);


/*!  
 * This function locks the provisioning key. it sets zeroes to the AES engine key. 
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address  
 *  
 */
void AES_LockEngineKey(DxUint32_t hwBaseAddress);


#ifdef __cplusplus
}
#endif

#endif



