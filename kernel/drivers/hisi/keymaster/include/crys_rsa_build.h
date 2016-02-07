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

 
 #ifndef CRYS_RSA_BUILD_H
#define CRYS_RSA_BUILD_H


#include "crys_error.h"
#include "crys_rsa_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *  Object name     :  CRYS_RSA_BUILD.h
 */
  
/** @file
 * \brief This module defines some utility functions for working with
 *        RSA cryptography.
 *
 * \version 
 * \author ronys
 */

/******************************************************************************************/
/**
 * @brief CRYS_RSA_Build_PubKey populates a CRYSRSAPubKey_t structure with
 *       the provided modulus and exponent.
 *
 *	Assumption : the modulus and the exponent are presented in big endian.
 *
 * @param[out] PubKey_ptr - a pointer to the public key structure. This structure will be
 *            used as an input to the CRYS_RSA_PRIM_Encrypt API.
 *
 * @param[in] Exponent_ptr - a pointer to the exponent stream of bytes ( Big endian ).
 * @param[in] ExponentSize - The size of the exponent in bytes.  
 * @param[in] Modulus_ptr  - a pointer to the modulus stream of bytes ( Big endian ) the MS
 *           bit must be set to '1'.
 * @param[in] ModulusSize  - The size of the modulus in bytes. Sizes supported according to
 *           used platform from 64 to 256 bytes and in some platforms up to 512 bytes.  
 * 
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CIMPORT_C CRYSError_t CRYS_RSA_Build_PubKey(  CRYS_RSAUserPubKey_t *UserPubKey_ptr,
                                    DxUint8_t *Exponent_ptr,
                                    DxUint16_t ExponentSize,
                                    DxUint8_t *Modulus_ptr,
                                    DxUint16_t ModulusSize );


/******************************************************************************************/
/**
 * @brief CRYS_RSA_Build_PrivKey populates a CRYSRSAPrivKey_t structure with
 *        the provided modulus and exponent, marking the key as a "non-CRT" key.
 *
 *        Assumption : the modulus and the exponent are presented in big endian.
 *
 * @param[out] UserPrivKey_ptr - a pointer to the public key structure. this structure will be used as
 *                          an input to the CRYS_RSA_PRIM_Decrypt API.
 * @param[in] PrivExponent_ptr - a pointer to the private exponent stream of bytes ( Big endian ).
 * @param[in] PrivExponentSize - the size of the private exponent in bytes.
 * @param[in] Exponent_ptr - a pointer to the exponent stream of bytes ( Big endian ).
 * @param[in] ExponentSize - the size of the exponent in bytes.  
 * @param[in] Modulus_ptr  - a pointer to the modulus stream of bytes ( Big endian ) the MS
 *            bit must be set to '1'.
 * @param[in] ModulusSize  - the size of the modulus in bytes. Sizes supported according to
 *            used platform from 64 to 256 bytes and in some platforms up to 512 bytes.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                       value MODULE_* as defined in ...
 *
 */
CIMPORT_C CRYSError_t CRYS_RSA_Build_PrivKey(CRYS_RSAUserPrivKey_t   *UserPrivKey_ptr,
	                               DxUint8_t               *PrivExponent_ptr,
                                   DxUint16_t               PrivExponentSize,
                                   DxUint8_t               *PubExponent_ptr,
                                   DxUint16_t               PubExponentSize,
                                   DxUint8_t               *Modulus_ptr,
                                   DxUint16_t               ModulusSize );

/******************************************************************************************

   @brief CRYS_RSA_Build_PrivKeyCRT populates a CRYSRSAPrivKey_t structure with
          the provided parameters, marking the key as a "CRT" key.
 
	Note: The "First" factor P must be great, than the "Second" factor Q.
 

   @param[out] UserPrivKey_ptr - A pointer to the public key structure. 
        		    This structure is used as input to the CRYS_RSA_PRIM_Decrypt API.
   @param[in] P_ptr - A pointer to the first factor stream of bytes (Big-Endian format)
   @param[in] PSize - The size of the first factor, in bytes.
   @param[in] Q_ptr - A pointer to the second factor stream of bytes (Big-Endian format)
   @param[in] QSize - The size of the second factor, in bytes.
   @param[in] dP_ptr - A pointer to the first factor's CRT exponent stream of bytes (Big-Endian format)
   @param[in] dPSize - The size of the first factor's CRT exponent, in bytes.
   @param[in] dQ_ptr - A pointer to the second factor's CRT exponent stream of bytes (Big-Endian format)
   @param[in] dQSize - The size of the second factor's CRT exponent, in bytes.
   @param[in] qInv_ptr - A pointer to the first CRT coefficient stream of bytes (Big-Endian format)
   @param[in] qInvSize - The size of the first CRT coefficient, in bytes.

*/
CIMPORT_C CRYSError_t CRYS_RSA_Build_PrivKeyCRT(CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
                                      DxUint8_t *P_ptr, 
                                      DxUint16_t PSize,
                                      DxUint8_t *Q_ptr,
                                      DxUint16_t QSize,
                                      DxUint8_t *dP_ptr, 
                                      DxUint16_t dPSize,
                                      DxUint8_t *dQ_ptr,
                                      DxUint16_t dQSize,
                                      DxUint8_t *qInv_ptr,
                                      DxUint16_t qInvSize );
                                      

/******************************************************************************************
@brief The CRYS_RSA_Build_ConvertPrivKeyToCRT function convert the non CRT private
       key to CRT private Key and initializes appropriate fields of input -
       output structure provided by pointer UserPrivKey_ptr.
       All input and output data is in little endian representation.

@param[in,out] UserPrivKey_ptr - A pointer to the user private  key structure. The key must be in non CRT mode.
               This structure is used as input to the CRYS_RSA_PRIM_Encrypt API.

@param[in]   Buffers_ptr -     A pointer to the structure containing temporary buffers used by LLF functions 
             called by this function. Size of buffer: 5 + 36 = 41 maximum modulus sizes.

NOTE: All members of input UserPrivKey structure must be initialized, including public key
      e-pointer and it size.
*/
CIMPORT_C CRYSError_t CRYS_RSA_Build_ConvertPrivKeyToCRT( CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	 		                                    CRYS_RSAConvertKeyToCrtBuffers_t *Buffers_pt );
                                      

/******************************************************************************************
   @brief CRYS_RSA_Get_PubKey gets the e,n public key from the database.
   
   @param[in] UserPubKey_ptr - A pointer to the public key structure. 
                               This structure is used as input to the CRYS_RSA_PRIM_Encrypt API.
 
   @param[out] Exponent_ptr - A pointer to the exponent stream of bytes (Big-Endian format)
   @param[in,out] ExponentSize_ptr - the size of the exponent buffer in bytes, it is updated to the 
                  actual size of the exponent, in bytes.  
   @param[out] Modulus_ptr  - A pointer to the modulus stream of bytes (Big-Endian format).
			   The MS (most significant) bit must be set to '1'.
   @param[in,out] ModulusSize_ptr  - the size of the modulus buffer in bytes, it is updated to the 
                  actual size of the modulus, in bytes.

   NOTE: All members of input UserPrivKey structure must be initialized, including public key
         e pointer and it size.

*/
CIMPORT_C CRYSError_t CRYS_RSA_Get_PubKey(  CRYS_RSAUserPubKey_t *UserPubKey_ptr,
                                  DxUint8_t  *Exponent_ptr,
                                  DxUint16_t *ExponentSize_ptr,
                                  DxUint8_t  *Modulus_ptr,
                                  DxUint16_t *ModulusSize_ptr ); 
				              
/******************************************************************************************
   @brief CRYS_RSA_Get_ModSizeFromPubKey extracts the modulus size from a given public key data structure.
   
   @param[in] UserPubKey_ptr - A pointer to the public key structure, as returned by
                               CRYS_RSA_Build_PubKey.
 
   @param[out] ModulusSize_ptr  -  The actual size of the modulus, in bytes.  
*/
CIMPORT_C CRYSError_t CRYS_RSA_Get_ModSizeFromPubKey(CRYS_RSAUserPubKey_t *UserPubKey_ptr,
                                           DxUint16_t *ModulusSize_ptr );
                                           
                                           
                                           
/******************************************************************************************
   @brief CRYS_RSA_Get_PrivKey gets the D,e,n of private key from the database.
   
   @param[in] UserPrivKey_ptr - A pointer to the private key structure. 
                               This structure is used as input to the CRYS_RSA_PRIM_Decrypt API.
 
   @param[out] PrivExponent_ptr - A pointer to the exponent stream of bytes (Big-Endian format)

   @param[in/out] PrivExponentSize - the size of the exponent buffer in bytes , it is updated to the 
                  actual size of the exponent, in bytes.  

   @param[out] PubExponent_ptr - a pointer to the public exponent stream of bytes ( Big endian ).
   
   @param[in/out] PubExponentSize - the size of the exponent buffer in bytes , it is updated to the 
                  actual size of the exponent, in bytes.    
   
   @param[out] Modulus_ptr  - A pointer to the modulus stream of bytes (Big-Endian format).
			   The MS (most significant) bit must be set to '1'.

   @param[in/out] ModulusSize_ptr  - the size of the modulus buffer in bytes , it is updated to the 
                  actual size of the modulus, in bytes.  
*/
CIMPORT_C CRYSError_t CRYS_RSA_Get_PrivKey(CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
				                   DxUint8_t             *PrivExponent_ptr,
				                   DxUint16_t            *PrivExponentSize_ptr,
				                   DxUint8_t             *PubExponent_ptr,
				                   DxUint16_t            *PubExponentSize_ptr,
				                   DxUint8_t             *Modulus_ptr,
				                   DxUint16_t            *ModulusSize_ptr );

/******************************************************************************************

   @brief CRYS_RSA_Get_PrivKeyCRT exports a CRYSRSAPrivKey_t structure data

   @param[In] UserPrivKey_ptr - a pointer to the public key structure. this structure will be used as
                            an input to the CRYS_RSA_PRIM_Decrypt API.
 
   @param[out] P_ptr - a pointer to the first factor stream of bytes ( Big endian ).
   @param[in/out] PSize_ptr - the size of the first factor buffer in bytes , it is updated to the 
                  actual size of the first factor, in bytes.
   @param[out] Q_ptr - a pointer to the second factor stream of bytes ( Big endian ).
   @param[in/out] QSize_ptr - the size of the second factor buffer in bytes , it is updated to the 
                  actual size of the second factor, in bytes.
   @param[out] dP_ptr - a pointer to the first factors CRT exponent stream of bytes ( Big endian ).
   @param[in/out] dPSize_ptr - the size of the first factor exponent buffer in bytes , it is updated to the 
                  actual size of the first factor exponent, in bytes.
   @param[out] dQ_ptr - a pointer to the second factors CRT exponent stream of bytes ( Big endian ).
   @param[in/out] dQSize_ptr - the size of the second factors CRT exponent buffer in bytes , it is updated to the 
                  actual size of the second factors CRT exponent, in bytes.
   @param[out] qInv_ptr - a pointer to the first CRT coefficient stream of bytes ( Big endian ).
   @param[in/out] qInvSize_ptr -  the size of the first CRT coefficient buffer in bytes , it is updated to the 
                  actual size of the first CRT coefficient, in bytes.
*/

CIMPORT_C CRYSError_t CRYS_RSA_Get_PrivKeyCRT(CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
				                      DxUint8_t *P_ptr, 
				                      DxUint16_t *PSize_ptr,
				                      DxUint8_t *Q_ptr,
				                      DxUint16_t *QSize_ptr,
				                      DxUint8_t *dP_ptr, 
				                      DxUint16_t *dPSize_ptr,
				                      DxUint8_t *dQ_ptr,
				                      DxUint16_t *dQSize_ptr,
				                      DxUint8_t *qInv_ptr,
				                      DxUint16_t *qInvSize_ptr);



/******************************************************************************************

   @brief CRYS_RSA_Get_PrivKeyModulus export the modulus vector from CRYSRSAPrivKey_t structure.

   @param[out] UserPrivKey_ptr - a pointer to the private key structure. 
 
   @param[out] N_ptr - a pointer to the modulus vector of bytes ( Big endian ).
   @param[in/out] NSize_ptr - the size of the modulus buffer in bytes , it is updated to the 
                  actual size in bytes.
*/

CEXPORT_C CRYSError_t CRYS_RSA_Get_PrivKeyModulus(CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
				                      DxUint8_t *N_ptr, 
				                      DxUint16_t *NSize_ptr);

#ifdef __cplusplus
}
#endif

#endif
