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

 
#ifndef RSA_H
#define RSA_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "sb_hash.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Sun Feb 20 14:11:42 2005
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version RSA.h#1:incl:1
   *  \author adams
   *  \remarks Copyright (C) 2004 by Discretix Technologies Ltd.
   *           All Rights reserved
   */


/************************ Defines ******************************/

/* the modulus size ion bits */
#define RSA_MOD_SIZE_IN_BITS 2048UL
#define RSA_MOD_SIZE_IN_BYTES (RSA_MOD_SIZE_IN_BITS/8)
#define RSA_MOD_SIZE_IN_WORDS (RSA_MOD_SIZE_IN_BITS/32)
#define RSA_MOD_SIZE_IN_256BITS (RSA_MOD_SIZE_IN_BITS/256)
#define RSA_EXP_SIZE_IN_BITS  17UL
#define RSA_EXP_SIZE_IN_BYTES	((RSA_EXP_SIZE_IN_BITS+7)/8)

/* size of buffer for Barrett modulus tag NP, used in PKA algorithms */
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BITS    132 
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BYTES   ((RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BITS+7)/8)
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS ((RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BYTES+3)/4)

/* the exponent */
#ifndef BIG__ENDIAN
#define RSA_EXP_VAL          0x00010001UL 
#else
#define RSA_EXP_VAL          0x01000100UL 
#endif


/* RSA PSS verify definitions */ 
#define RSA_PSS_SALT_LENGTH  32
#define RSA_PSS_PAD1_LEN     8

/* RSA Encrypt definitions */
#define RSA_ENCR_RND_PS_SIZE_BYTES   8
#define RSA_ENCR_DATA_IN_SIZE_BYTES  16 

#define RSA_PKCS1_VER15   1
#define RSA_PKCS1_VER21   1



/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/
/*************************************************************************/			      

                           
#ifdef DX_SB_SEC_BOOT_PKCS_V15			      
/**

   \brief RSA_PSS_Verify implements the Verify algorithm
   as defined in PKCS#1 v1.5

   @param[in] hwBaseAddress - HW base address. Relevant for HW
                    	      implementation, for SW it is ignored.
   @param[in] DataInHashResult - the DataIn hash result.
   @param[in] N_ptr           - the modulus (2048 bits).
   @param[in] Np_ptr          - the Barrett tag. Relevant for HW
                    	      implementation, for SW it is ignored.   
   @param[in] Sig_ptr	 - the pointer to the signature to be
        		   verified.  
        Note: All input arrays are in LE format of bytes and words.
    
   @return CRYSError_t - TRUE - success , FALSE - failure 
*/ 
DxUint32_t RSA_PKCS15_Verify( DxUint32_t hwBaseAddress,
                         HASH_Result_t   DataInHashResult, 
                         DxUint32_t      *N_ptr,
                         DxUint32_t      *Np_ptr,
                         DxUint32_t      *Sig_ptr );

#else
/*************************************************************************/   
/**

   \brief RSA_PSS_Verify implements the Verify algorithm
   as defined in PKCS#1 v2.1

   @param[in] hwBaseAddress - HW base address. Relevant for HW
                    	      implementation, for SW it is ignored.
   @param[in] DataInHashResult - the DataIn hash result.
   @param[in] N_ptr           - the modulus (2048 bits).
   @param[in] Np_ptr          - the Barrett tag. Relevant for HW
                    	      implementation, for SW it is ignored.   
   @param[in] Sig_ptr	 - the pointer to the signature to be
        		   verified.  
        Note: All input arrays are in LE format of bytes and words.

   @return CRYSError_t - TRUE - success , FALSE - failure 
*/

DxUint32_t RSA_PSS_Verify( DxUint32_t      hwBaseAddress,
                           HASH_Result_t   DataInHashResult, 
                           DxUint32_t      *N_ptr,
                           DxUint32_t      *Np_ptr,
                           DxUint32_t      *Sig_ptr );
 
#endif
                           
                           
/*************************************************************************/			      
/*!
 * 
 * \author reuvenl (7/28/2011)
 * 
 * \brief RSA_PKCS15_Encrypt implements the RSA_Encrypt algorithm
 *   as defined in PKCS#1 v1.5 with given below parameters.
 *  
 *  Assumptions and definitions (see rsa.h):
 *    -  RSA modulus size = RSA_MOD_SIZE_IN_BYTES = 256.
 *    -  Public Exponent = RSA_EXP_VAL = 0x010001.
 *    -  Length of random PS buffer = RSA_ENCR_RND_PS_SIZE_BYTES = 8 bytes;
 *    -  Actual input data size = RSA_ENCR_DATA_IN_SIZE_BYTES = 16 bytes.
 *    -  Input/output buffer must be of size >=256 bytes. 
 *    -  The function gives the encrypted result as big endian bytes array
 *       (MSByte is a left most one).
 *    -  The function is intended for littl endian processors.
 *
 * \param hwBaseAddress : the CryptoCell base address; relevant for HW
 *      		  implementation, for SW it is ignored.
 * \param N_ptr : the pointert to RSA modulus - little endian bytes array.
 * \param Np_ptr : the pointer to Barret tag (little endian bytes
 *        array) used in modular reduction; the size is defined as
 *        RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BYTES. Relevant for HW
 *	  implementation, for SW it is ignored.
 * \param Rnd_ptr : the pointert to random PS buffer of size 8 
 *        bytes; the data must be set as little endian bytes
 *        array; the pointer  must be aligned to 4-bytes word.
 * \param Data_ptr : the pointer to the buffer containing the input 
 *        data to be encrypted and also the output result.
 *        The size of the buffer must be >= 256 bytes; the
 *        actual (significant) input data (size = 16 bytes)
 *        must be set as bytes stream (from left most byte 0
 *        to byte 15); the pointer must be aligned to 4-bytes word.
 */
void RSA_PKCS15_Encrypt(   
                DxUint32_t    hwBaseAddress,
                DxUint32_t   *N_ptr,
                DxUint32_t   *Np_ptr,
                DxUint8_t    *Rnd_ptr,
                DxUint8_t    *Data_ptr );


/*************************************************************************/			      
/**
 * @brief The RSA_CalcExponent calculates The following:
 *
 *                   Res = (Base ^ Exp) mod N. ( Exp = 0x10001 )
 *
 *        The calculation is done in a secured way using the PIC.
 *
 * @param[in] hwBaseAddress - HW base address. Relevant for HW
 *                  	implementation, for SW it is ignored.
 * @Base_ptr[in]         - The pointer to the base buffer.
 * @N_ptr[in]            - The pointer to the modulus buffer (2048 bits).
 * @Np_ptr[in]           - The np vector buffer (160 bits). Relevant for HW
 *                         implementation, for SW it is ignored.
 * @Res_ptr[out]         - The pointer to the buffer that will contain the result.
 *  
 *     NOTE:  The SW version of the function uses a temp buffer, pointed by
 *            global pointer gSecBootExpTempBuff; the size of the buffer must be
 *            not less than 8*RSA_MOD_SIZE_IN_WORDS + 1 word.
 *  
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void RSA_CalcExponent( 
		DxUint32_t hwBaseAddress,
		DxUint32_t *Base_ptr,    
		DxUint32_t *N_ptr,        
		DxUint32_t *Np_ptr,        
		DxUint32_t *Res_ptr );

                
/******    RSA_HW_PKI_PKA_CalcNp function (physical pointers)     ***************/
/*!
 * 
 * \author reuvenl (7/28/2011)
 * 
 * \brief The function initializes  modulus and Barret tag NP, used in modular 
 *        PKA operations. 
 *
 *        The function does the following:
 *          - calculates mod size in bits and sets it into PKA table sizes;
 *          - calls the RSA_HW_PKI_PKA_ExecCalcNpAndInitModOp, which calculates
 *            the Barret tag NP.
 *          - initializes PKA registers and outputs calculated NP value.
 *
 *        Assumings: - The registers mapping table is set on default mode,
 *                     according to modulus size:
 *                      -- count of allowed registers is not less, than 7
 *                         (including 3 registers rT0,rT2,rT3 for internal
 *                         calculations and 4 default special registers
 *                         N,NP,T0,T1);modulus exact and exact+32 bit sizes
 *                         should be set into first two entries of sizes-table
 *                         accordingly.
 *
 * \param N_ptr : The pointer to the buffer, containing modulus N as little 
 *        endian words array - LSWord is left most one. 
 * \param N_sizeBits : The size of modulus in bytes, must be 
 *        16 <= N_sizeBytes <= 264.
 * \param NP_ptr : The pointer to the buffer, containing result - Barrett tag NP 
 *        given as little endian words array - LSWord is left most one.
 * \param VirtualHwBaseAddr : Virtual HW base address, passed by user.
 */
void RSA_HW_PKI_PKA_CalcNp(
                          DxUint32_t        *N_ptr,             /*in*/ 
                          DxUint32_t         N_sizeBits,        /*in*/ 
                          DxUint32_t        *NP_ptr,            /*out*/
                          DxUint32_t         VirtualHwBaseAddr  /*in*/ );                			      
                           
#ifdef __cplusplus
}
#endif

#endif



