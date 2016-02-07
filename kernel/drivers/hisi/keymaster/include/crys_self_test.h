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

 
 #ifndef CRYS_SELF_TEST_H
#define CRYS_SELF_TEST_H

#include "crys_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /** @file
   *  \brief Perform self tests for specified engines
   *
   *  \author adams
   */

/************************ Defines ******************************/

/* @brief the following defines enable us to select the engines we want to test
          these defines also describe the bit field of the result */
#define CRYS_SELF_TEST_AES_BIT				0x0001
#define CRYS_SELF_TEST_DES_BIT				0x0002
#define CRYS_SELF_TEST_HASH_BIT				0x0004
#define CRYS_SELF_TEST_HMAC_BIT				0x0008
#define CRYS_SELF_TEST_RSA_ENCDEC_BIT	    0x0010
#define CRYS_SELF_TEST_ECC_BIT  	    	0x0020
#define CRYS_SELF_TEST_RND_DRBG_BIT	    	0x0040
#define CRYS_SELF_TEST_RND_BIT  CRYS_SELF_TEST_RND_TRNG_BIT 


#define CRYS_SELF_TEST_ALL_BIT     (CRYS_SELF_TEST_AES_BIT |  \
                                    CRYS_SELF_TEST_DES_BIT |  \
                                    CRYS_SELF_TEST_HASH_BIT | \
				    CRYS_SELF_TEST_ECC_BIT |  \
				    CRYS_SELF_TEST_HMAC_BIT | \
                                    CRYS_SELF_TEST_RSA_ENCDEC_BIT)

#define CRYS_SELF_TEST_NO_RND     (CRYS_SELF_TEST_AES_BIT |  \
                                    CRYS_SELF_TEST_DES_BIT |  \
                                    CRYS_SELF_TEST_HASH_BIT | \
									CRYS_SELF_TEST_HMAC_BIT | \
                                    CRYS_SELF_TEST_RSA_ENCDEC_BIT )

/************************ Structs  ******************************/

#define CRYS_SELF_TEST_BUFFER_SIZE_IN_WORDS 538

typedef struct
{
   DxUint32_t temp1[CRYS_SELF_TEST_BUFFER_SIZE_IN_WORDS];
   
}CRYS_SelfTestData_t;   


/************************ Public Functions **********************/

/**
 * @brief This function executes the CRYS self test for the specified engines
 *
 * @param[in] EnginesSelect - Specify the engines to test according to the following values:\n
 *
 *                            CRYS_SELF_TEST_AES_BIT			\n
 *                            CRYS_SELF_TEST_DES_BIT			\n
 *                            CRYS_SELF_TEST_HASH_BIT			\n
 *							  CRYS_SELF_TEST_HMAC_BIT			\n
 *                            CRYS_SELF_TEST_RSA_ENCDEC_BIT     \n
 *                            CRYS_SELF_TEST_RND_TRNG_BIT    	\n
 *                            CRYS_SELF_TEST_RND_DRBG_BIT       \n
 *                            CRYS_SELF_TEST_ALL_BIT			\n
 *
 * @param[out] EnginesTestResult_ptr - a bit field describing the test results of the selected engines.
 *                                    The bit field is the same as described above.
 *                                    For each bit specified in the input: 0 - test passes, 1 - test failed. 
 *
 * @param[in] TempData_ptr - A pointer to a scratchpad buffer required for the self test.
 *							 The size of the buffer is specified in the typedef.
 *                            
 * @return CRYSError_t - On success the function returns the value CRYS_OK, and on failure a non-ZERO error.
 *
 * \note This function is useful mainly to test hardware implementations of the cryptographic engines upon 
 * system startup. This function is to fulfil the FIPS demands for power-up self tests.
 */

 CRYSError_t  CRYS_SelfTest(DxUint16_t EnginesSelect,
                            DxUint16_t *EnginesTestResult_ptr);

#ifdef __cplusplus
}
#endif

#endif


