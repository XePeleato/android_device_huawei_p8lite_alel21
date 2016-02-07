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

#ifndef  DX_RNG_PLAT_H
#define  DX_RNG_PLAT_H
#include "log.h"

/****************  Defines  ********************/

/* NVM OTP TRNG defines*/
#define RNG_PLAT_OTP_READ_ADDR           	(0x1 << DX_HOST_AIB_ADDR_REG_READ_ACCESS_BIT_SHIFT)
#define RNG_PLAT_OTP_WRITE_ADDR           	(0x1 << DX_HOST_AIB_ADDR_REG_WRITE_ACCESS_BIT_SHIFT)
#define RNG_PLAT_OTP_TRNG_MASK			0xfff00000UL
#define RNG_PLAT_OTP_TRNG_OFFSET		(0x0A*sizeof(DxUint32_t)) /*DX_MNG_OTP_MANUFACTRER_FLAG_OFFSET*/

/* TRNG filds offsets and masks  */
#define DX_RNG_OTP_ROSCS_ALLOWED_BIT_OFFSET     20
#define DX_RNG_OTP_ROSCS_ALLOWED_BIT_MASK       0x0FUL 
#define DX_RNG_OTP_SUB_SAMPL_RATIO_BIT_OFFSET   24
#define DX_RNG_OTP_SUB_SAMPL_RATIO_BIT_MASK     0x7fUL
#define DX_RNG_OTP_SUB_SAMPL_RATIO_BIT_SIZE      7
#define DX_RNG_OTP_TRNG_MODE_BIT_OFFSET	        31

#define DX_RNG_OTP_SUB_SAMPL_RATIO_MAX_VAL      ((1UL << DX_RNG_OTP_SUB_SAMPL_RATIO_BIT_SIZE) - 1)

/* Default TRNG parameters: used when in OTP set 0 in appropriate bits */
#define DX_RNG_DEFAULT_TRNG_MODE           CRYS_RND_Fast
#define DX_RNG_DEFAULT_ROSCS_ALLOWED_FLAG       0xF

/* Default, increment and mininimal values, for Sampling Ratio */
/* On Fast mode */
#define DX_RNG_DEFAULT_SAMPL_RATIO_ON_FAST_MODE 30 
#define DX_RNG_SAMPL_RATIO_INCREM_ON_FAST_MODE  1     
#define DX_RNG_MIN_SAMPL_RATIO_ON_FAST_MODE     1     
/* On  Slow mode */
#define DX_RNG_DEFAULT_SAMPL_RATIO_ON_SLOW_MODE 1000
#define DX_RNG_SAMPL_RATIO_INCREM_ON_SLOW_MODE  50     
#define DX_RNG_MIN_SAMPL_RATIO_ON_SLOW_MODE     1000

/* Maximal value of SamplingRatio */
#define DX_RNG_MAX_SAMPL_RATIO_ON_FAST_MODE     DX_RNG_OTP_SUB_SAMPL_RATIO_MAX_VAL 
#define DX_RNG_MAX_SAMPL_RATIO_ON_SLOW_MODE     (DX_RNG_MIN_SAMPL_RATIO_ON_SLOW_MODE + \
						DX_RNG_SAMPL_RATIO_INCREM_ON_SLOW_MODE * DX_RNG_OTP_SUB_SAMPL_RATIO_MAX_VAL)

/* Poll on the LCS valid register */
#define RNG_PLAT_WAIT_ON_LCS_VALID_BIT() 						\
	do { 										\
		DxUint32_t regValTT; 							\
		do { 									\
			regValTT = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(HOST_RGF, LCS_IS_VALID)); \
		}while( !(regValTT & 0x1)); 						\
	}while(0) 
 

/* Poll on the AIB acknowledge bit */
#define RNG_PLAT_WAIT_ON_AIB_ACK_BIT() 						        \
do { 											\
	DxUint32_t regValTT; 								\
	do { 										\
		regValTT = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET(HOST_RGF, AIB_FUSE_ACK));    \
	}while( !(regValTT & 0x1));							\
}while(0)


	/*RNG_PLAT_WAIT_ON_AIB_ACK_BIT();*/                                    	\
/* Read from the AIB  */
#define RNG_PLAT_READ_WORD_FROM_AIB(nvmAddr, nvmData)				\
do { 										\
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_AIB_ADDR_REG), (nvmAddr)); \
	nvmData = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_AIB_RDATA_REG)); \
}while(0)



/*!
 * @brief This function returns a word from the OTP according to a given address. 	  
 *
 * @param[in] hwBaseAddress 	- cryptocell base address 
 * @param[in] otpAddress	- Address in OTP [in Bytes]
 * @param[out] otpWord		- the returned OTP word
 *
 * @return DxError_t 		- On success: the value DX_OK is returned, 
 *         			  On failure: a value from sbrom_management_error.h
 */    

/*
#define RNG_PLAT_READ_OTP(regVal) \
do { \
	RNG_PLAT_WAIT_ON_LCS_VALID_BIT(); \
	RNG_PLAT_READ_WORD_FROM_AIB(RNG_PLAT_OTP_TRNG_OFFSET | RNG_PLAT_OTP_READ_ADDR, (regVal)); \
} while (0)
*/

#define RNG_PLAT_READ_OTP(regVal) \
do { \
	mdelay(10); \
	RNG_PLAT_READ_WORD_FROM_AIB(RNG_PLAT_OTP_TRNG_OFFSET | RNG_PLAT_OTP_READ_ADDR, (regVal)); \
} while (0)

#endif  /* DX_RNG_PLAT_H */
