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

 
 #ifndef LLF_PKI_RSA_H
#define LLF_PKI_RSA_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "crys_rsa_types.h"
#include "llf_pki_error.h"


#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:39:48 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version llf_pki_rsa.h#1:incl:1
   *  \author adams
   */

/************************ Defines ******************************/

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions ******************************/


/*******************************************************************************************/
/**
 * @brief This function initializes the low level key database public structure.
 *        On the Lite platform the Hn vector is initialized  
 *         
 *
 * @param[in] LLFSpesificParams - spesific parameters required on this LLF implementation.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_RSA_InitPubKeyDb( CRYSRSAPubKey_t *PubKey_ptr );


/*******************************************************************************************/
/**
 * @brief This function initializes the low level key database private structure.
 *        On the Lite platform the Hn vector is initialized  
 *         
 *
 * @param[in] LLFSpesificParams - spesific parameters required on this LLF implementation.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_RSA_InitPrivKeyDb( CRYSRSAPrivKey_t *PrivKey_ptr );


/*******************************************************************************************/
/**
 * @brief This function executes the RSA primitive public key exponent :
 *       
 *    PubData_ptr->DataOut =  PubData_ptr->DataIn ** PubKey_ptr->e  mod  PubKey_ptr->n,
 *    where: ** - exponent symbol.    
 *   
 *    Note: PKA registers used: r0-r4,   r30,r31, size of registers - Nsize.
 *
 * @param[in] PubKey_ptr  - The public key database.
 * @param[in] PubData_ptr - The structure, containing input data and output buffer.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */ 
CRYSError_t LLF_PKI_RSA_ExecPubKeyExp( CRYSRSAPubKey_t     *PubKey_ptr,
                                       CRYS_RSAPrimeData_t *PubData_ptr );
                                       

/*******************************************************************************************/                                       
/**
 * @brief This function executes the RSA primitive private key exponent engine 
 *         
 *
 * @param[in] PubKey_ptr - the private key database.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_RSA_ExecPrivKeyExp( CRYSRSAPrivKey_t *PrivKey_ptr ,
                                        CRYS_RSAPrimeData_t *PrivData_ptr );

/*******************************************************************************************/
/**
 * @brief This function generates a key pair
 *         
 *
 * @param[in] PubKey_ptr - the public key database.
 * @param[in] PrivKey_ptr - the private key database.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */ 
CRYSError_t LLF_PKI_RSA_GenerateKeyPair( CRYSRSAPubKey_t  *PubKey_ptr,
                                         CRYSRSAPrivKey_t *PrivKey_ptr,
                                         CRYS_RSAKGData_t *KeyGenData_ptr );
                                         

/*******************************************************************************************/                                         
/**
 * @brief This function calculates the N using P,Q ( CRT ) : N = P*Q
 *         
 *
 * @param[in] P_ptr - the first prime.
 * @param[in] P_SizeInBits - the first prime size in bits.
 * @param[in] Q_ptr - the second prime.
 * @param[in] Q_SizeInBits - the second prime size in bits.
 * @param[in] N_ptr        - the N vector. 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */ 
CRYSError_t LLF_PKI_RSA_CRTCalcN( DxUint32_t  *P_ptr, DxUint32_t PSizeInBits,
                                  DxUint32_t  *Q_ptr, DxUint32_t QSizeInBits,
                                  DxUint32_t  *N_ptr ); 
                                                         
                                  
/*******************************************************************************************/
/**
   @brief The LLF_PKI_RSA_ConvertPrivKeyToCRT function converts the non CRT private
          key to CRT private Key and initializes appropriate fields of 
          output structure provided by pointer PrivCRTKey_ptr.
          All input and output data is in little endian representation.
          
          NOTE 1: The input (non CRT) and output (CRT) key structures are in common union buffer,
                  Therefore in the function all input (non CRT) data is used or saved befor 
                  inserting any computed data into CRT key structure.
                
          NOTE 2: The function is not supported now on this project.    
   
   @param[in] PrivNonCRTKey_ptr -  A pointer to the private non CRT key structure. 
                                   This structure is used as input to the CRYS_RSA_PRIM_Encrypt API.
   @param[in] n_ptr             -  A pointer to a buffer containing modulus.
   @param[in] nSizeInBits       -  Size of modulus in bits.
   @param[in] PrivCRTKey_ptr    -  A pointer to the private non CRT key structure.
   @param[in] TempBuffers_ptr   -  A pointer to the temporary buffers used by LLF functions called by
                                   this function. Size of buffer: 4 + 36 = 40 maximum modulus sizes.
*/

CRYSError_t LLF_PKI_RSA_ConvertPrivKeyToCRT( 
				CRYSRSAPrivNonCRTKey_t           *PrivNonCRTKey_ptr, 	/*in*/
				DxUint32_t			 *n_ptr, /*in*/
				DxUint32_t			  nSizeInBits, 		/*in*/
				CRYSRSAPrivCRTKey_t              *PrivCRTKey_ptr,	/*out*/
				CRYS_RSAConvertKeyToCrtBuffers_t *TempBuffers_ptr      /*in*/ );

/*******************************************************************************************/
/**
 * 
 * Function name: LLF_PKI_RSA_Call_Div
 *
 * Description: This function performs division of big numbers, passed by physical pointers, 
 *              using the PKA.
 *              .
 *     Computes modRes = A mod B. divRes_ptr = floor(A/B) 
 *     Lengths: A[ALen], B[BLen], modRes[BLen], divRes[ALen].
 *     Assumes:  c > 0.
 *
 *     PKA registers using: A=>r2, B=>r3, divRes=>r4, modRes=>r2 (r2 is rewritten by remainder).
 * 
 * Author: R.Levin 
 *
 * Last Revision: 1.00.00
 *
 * @param[in] A_ptr          - The pointer to numerator A vector.
 * @param[in] ASizeInWords   - Length of numerator A in words.
 * @param[in] B_ptr          - The pointer to divider B (modulus).  
 * @param[in] BSizeInWords   - The size of B vector in words.
 * @param[out] modRes_ptr    - The pointer to modulus result (reminder of division).
 * @param[out] divRes_ptr    - The pointer to result of division.
 * @param[in] tempBuff_ptr   - The pointer to temp buffer - not used, may be set NULL.
 *
 * @return  - no return value
 *
 * Update History:
 * Rev 1.00.00, Date 4 Feb. 2008,
 * 
 */
 CRYSError_t  LLF_PKI_RSA_Call_Div( 
                  DxUint32_t *A_ptr,        DxUint32_t ASizeInWords,
                  DxUint32_t *B_ptr,        DxUint32_t BSizeInWords,
                  DxUint32_t *modRes_ptr, 
                  DxUint32_t *divRes_ptr,
                  DxUint32_t *tempBuff_ptr);
                  

/*******************************************************************************************/
/**
 *
 * Function name: LLF_PKI_RSA_CallRMul
 *
 * Description: This function performs multiplication of big numbers, passed by physical  
 *              pointers, using the PKA. 
 *
 *        The RMul operation is : (A * B) 
 *
 *        The function performs the following algorithm:
 *
 *
 * @param[in] A_ptr       - The pointer of A vector.
 * @param[in] B_ptr       - The pointer of B vector.  
 * @param[in] SizeInBits - The size of vectors in bits.
 * @param[out] Res_ptr    - The pointer to the result buffer.
 *
 * @return CRYSError_t - CRYS_OK 
 */
 CRYSError_t LLF_PKI_RSA_CallRMul( DxUint32_t *A_ptr , DxUint32_t SizeInBits,
                                   DxUint32_t *B_ptr ,   
                                   DxUint32_t *result_ptr );                  


/*******************************************************************************************
 * @brief This function is used to test a primality according to ANSI X9.42 standard.
 *
 *        The function calls the LLF_PKI_primeTest function which performs said algorithm. 
 *
 * @param[in] P_ptr           - The pointer to the prime buff.
 * @param[in] sizeWords       - The prime size in words.
 * @param[in] rabinTestsCount - The count of Rabin-Miller tests repetition.
 * @param[in] isPrime         - The flag indicates primality:   
 *                                  if is not prime - DX_FALSE, otherwise - DX_TRUE.  
 * @param[in] TempBuff_ptr   - The temp buffer of minimum size:
 *                               - on HW platform  8*MaxModSizeWords,
 *                               - on SW platform  41*MaxModSizeWords.
 * @param[in] primeTestMode - primality testing mode (RSA or DH - defines how are performed some 
 *            operations on temp buffers.
 */
 CRYSError_t LLF_PKI_RSA_primeTestCall(  
                            DxUint32_t *P_ptr,                              
                            DxInt32_t   sizeWords, 
                            DxInt32_t   rabinTestsCount,
                            DxInt8_t   *isPrime_ptr,
                            DxUint32_t *TempBuff_ptr,
						    CRYS_RSA_DH_PrimeTestMode_t primeTestMode );
						    

/************************ Private Functions **********************/

#ifdef __cplusplus
}
#endif

#endif
