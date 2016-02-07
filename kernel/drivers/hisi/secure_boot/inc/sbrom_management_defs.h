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



#ifndef _SBROM_MNG_DEFS_H
#define _SBROM_MNG_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif


/* OTP memory layout */
#define DX_MNG_OTP_BASE_ADDR			0x00
#define DX_MNG_OTP_START_OFFSET			0x00
#define DX_MNG_OTP_KDR_OFFSET			0x00
#define DX_MNG_OTP_SCP_OFFSET			0x08
#define DX_MNG_OTP_MANUFACTRER_FLAG_OFFSET	0x0A
#define DX_MNG_OTP_OEM_FLAG_OFFSET		0x0B
#define DX_MNG_OTP_KCE_OFFSET			0x0C
#define DX_MNG_OTP_BASE_HASH_OFFSET		0x10
#define DX_MNG_OTP_HASH_INDEX_0_OFFSET		0x10
#define DX_MNG_OTP_HASH_INDEX_1_OFFSET		0x14
#define DX_MNG_OTP_SW_VERSION_OFFSET		0x18
#define DX_MNG_OTP_LAST_OFFSET			0x1F


/* NVM definitions */
#define DX_MNG_NVM_READ_ADDR           		(0x1 << DX_HOST_AIB_ADDR_REG_READ_ACCESS_BIT_SHIFT)
#define DX_MNG_NVM_WRITE_ADDR           	(0x1 << DX_HOST_AIB_ADDR_REG_WRITE_ACCESS_BIT_SHIFT)


/* Security flag word */
#define DX_MNG_SECURITY_DISABLE_FLAG_ON    	0xC     
#define DX_MNG_SECURITY_DISABLE_FLAG_OFF    	0x3  
#define DX_MNG_SECURITY_DISABLE_FLAG_MASK	0x0fUL
#define DX_MNG_SECURITY_DISABLE_FLAG_SHIFT	16


/* OTP error status flag */
#define DX_MNG_OTP_ERR_STATUS_SHIFT		8
#define DX_MNG_OTP_ERR_STATUS_MASK		0x3


/* Definitions for OEM HASH key size */
#define DX_MNG_HASH_SIZE_IN_WORDS	DX_SB_HASH_LENGTH_IN_WORDS
#define DX_MNG_TRUNCATED_HASH_INDEX_0		0
#define DX_MNG_TRUNCATED_HASH_INDEX_1		1
#define DX_MNG_MAX_HASH_SIZE_IN_WORDS		8
#define DX_MNG_NUM_OF_ZEROES_IN_HASH0_SHIFT	0
#define DX_MNG_NUM_OF_ZEROES_IN_HASH1_SHIFT	8
#define DX_MNG_NUM_OF_ZEROES_IN_HASH_MASK	0xffUL


/* RKEK size can be retrieved from host boot configuration register (hw bit description is missing) */
#define DX_MNG_KDR_BOOT_CONFIG_SHIFT		0x1
#define DX_MNG_KDR_BOOT_CONFIG_MASK		0x1


/* session key definition */
#define DX_MNG_SESSION_KEY_IS_SET		1
#define DX_MNG_SESSION_KEY_IS_UNSET		0
#define DX_MNG_SESSION_KEY_PREFIX_DATA1		0x01
#define DX_MNG_SESSION_KEY_PREFIX_DATA2		0x053
#define DX_MNG_SESSION_KEY_PREFIX_DATA3		0x00
#define DX_MNG_SESSION_KEY_SUFFIX_DATA		0x080


/* TRNG definition */
#define DX_MNG_OTP_TRNG_OFFSET			DX_MNG_OTP_MANUFACTRER_FLAG_OFFSET
#define DX_MNG_OTP_TRNG_MASK			0xfff00000UL


/* ********************** Macros ******************************* */

/*This function is based on an algorithm that was taken from :
  http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
**/
/* Count number of zeroes in 32-bit word */
#define DX_MNG_COUNT_ZEROES(regVal, regZero) 					\
	do {									\
		DxUint32_t v = regVal;                                         	\
		v = v - ((v >> 1) & 0x55555555);                   		\
		v = (v & 0x33333333) + ((v >> 2) & 0x33333333);    		\
		v = ((((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24); 	\
		regZero	+= (32 - v);						\
	}while(0)


#ifdef __cplusplus
}
#endif

#endif



