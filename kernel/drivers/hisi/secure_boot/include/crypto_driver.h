
/**************************************************************************
 *   Copyright 2009 © Discretix Technologies Ltd. This software is         *
 *   protected by copyright, international treaties and various patents.   *
 *   Any copy or reproduction of this Software as permitted below, must    *
 *   include this Copyright Notice as well as any other notices provided   *
 *   under such license.                                                   *
 *                                                                         *
 *   This program shall be governed by, and may be used and redistributed  *
 *   under the terms and conditions of the GNU Lesser General Public       *
 *   License, version 2.1, as published by the Free Software Foundation.   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY liability and WARRANTY; without even the implied      *
 *   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 *   See the GNU General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, please write to the          *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef _CRYPTO_DRIVER_H
#define _CRYPTO_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!  
 * This function initializes the AES and HASH HW engines according to required crypto operations.
 * This should be the first function called.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] aesKey_ptr	- a pointer to AES key
 * @param[in] aesIv_ptr		- a pointer to AES IV
 * @param[in] cryptoDriverMode  - can be one of CryptoDriverMode_t 
 *  
 */
void SBROM_CryptoInit(DxUint32_t hwBaseAddress, AES_Key_t *aesKey_ptr, AES_Iv_t *aesIv_ptr, CryptoDriverMode_t cryptoDriverMode);


/*!  
 * This function is used to do cryptographic operations on a block(s) of data using HASH and/or AES machines.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] inputData_ptr 	- a pointer to the users data input buffer.
 * @param[out] outputData_ptr 	- a pointer to the users data output buffer.
 * @param[in] BlockSize		- number of bytes to update.
 *                                if it is not the last block, the size must be a multiple of AES blocks. 
 * @param[in] isLastBlock  	- if false, just updates the data; otherwise, enable hash padding
 * @param[in] cryptoDriverMode  - can be one of CryptoDriverMode_t
 *  
 */
void SBROM_CryptoUpdateBlock(DxUint32_t hwBaseAddress, DxUint32_t *inputData_ptr, DxUint32_t *outputData_ptr, DxUint32_t BlockSize, DxUint8_t isLastBlock, CryptoDriverMode_t cryptoDriverMode);


/*!  
 * This function returns the digest result of crypto hash operation.
 * 
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[out] hashResult 	- the HASH result.
 *  
 */
void SBROM_CryptoFinish(DxUint32_t hwBaseAddress, HASH_Result_t hashResult);


#ifdef __cplusplus
}
#endif

#endif



