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




/************* Include Files ****************/

#include "dx_crys_kernel.h"
#include "dx_hal_plat.h"
#include "dx_pal_types.h"
#include "dx_reg_base_host.h"
#include "dx_cc_regs.h"
#include "dx_host.h"
#include "crys_rnd.h"
#include "crys_rnd_local.h"
#include "dx_rng_plat.h"


	 
/****************  Defines  ********************/


/****************************************************************************************/
/**
 * 
 * @brief The function retrievess the TRNG parameters, provided by the User trough NVM,
 *        and sets them into structures given by pointers rndState_ptr and trngParams_ptr. 
 * 
 * @author reuvenl (6/26/2012)
 * 
 * @param[out] rndState_ptr - The pointer to structure, containing PRNG data and
 *                            parameters.
 * @param[out] trngParams_ptr - The pointer to structure, containing parameters
 *                            of HW TRNG.
 *
 * @return CRYSError_t - no return value  
 */
CRYSError_t RNG_PLAT_SetUserRngParameters(
			CRYS_RND_State_t   *rndState_ptr,         
			CRYS_RND_Params_t  *trngParams_ptr) 
{
	/* DECLARATIONS   */

	CRYSError_t  error = CRYS_OK;

#ifndef RND_DEBUG_GET_PARAMS

	DxUint32_t   regVal;
	DxUint32_t   trngInfo;

	
   	/* FUNCTION LOGIC */
   	
	/* Set the AES key size as max. supported size */
	/*---------------------------------------------*/ 
	regVal = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL, AES_HW_FLAGS));
	regVal = DX_CC_REG_FLD_GET(CRY_KERNEL, AES_HW_FLAGS, SUPPORT_256_192_KEY, regVal);
	if(regVal == 1) 
		rndState_ptr->KeySizeWords = 8; /*SUPPORT_256_192_KEY*/
	else 
		rndState_ptr->KeySizeWords = 4;


	/* Get TRNG Info bits from NVM */
	/*-----------------------------*/
	RNG_PLAT_READ_OTP(regVal) ;

	/* TRNG info value */
	trngInfo = regVal & RNG_PLAT_OTP_TRNG_MASK;


	/* Set TRNG parameters         */
	/*-----------------------------*/ 

	/* TRNG operation mode:  0 - Fast, 1 - Slow */
	trngParams_ptr->TrngMode = ((trngInfo >> DX_RNG_OTP_TRNG_MODE_BIT_OFFSET) & 0x1UL); 
	
	/* Allowed ROSCs lengths b'0-3. If bit value 1 - appropriate ROSC is  * 
	*  allowed. Default value is 0xF - all ROSCs are allowed.             */
	/*  If this field is not initialized, then set it to 0xF */
	trngParams_ptr->RoscsAllowed = ((trngInfo >> DX_RNG_OTP_ROSCS_ALLOWED_BIT_OFFSET) & DX_RNG_OTP_ROSCS_ALLOWED_BIT_MASK);
	if(trngParams_ptr->RoscsAllowed == 0)
		trngParams_ptr->RoscsAllowed = DX_RNG_DEFAULT_ROSCS_ALLOWED_FLAG;
		
	/* Sampling ratio: according to cc44 Sys. spec. Note: Sampling ratio  *                                                 
	   bits (7 bits) are common for Fast and Slow modes  		      */
	regVal = ((trngInfo >> DX_RNG_OTP_SUB_SAMPL_RATIO_BIT_OFFSET) & DX_RNG_OTP_SUB_SAMPL_RATIO_BIT_MASK);

	if (trngParams_ptr->TrngMode == CRYS_RND_Fast) {
		if(regVal == 0)
			trngParams_ptr->SubSamplingRatio = DX_RNG_DEFAULT_SAMPL_RATIO_ON_FAST_MODE;
		else trngParams_ptr->SubSamplingRatio = regVal; 
	}
	else { /* Slow mode */
		trngParams_ptr->SubSamplingRatio =  DX_RNG_MIN_SAMPL_RATIO_ON_SLOW_MODE + \
						    DX_RNG_SAMPL_RATIO_INCREM_ON_SLOW_MODE * regVal; 
	}

#else
 
	/*--------------------------*/
	/* Testing implementation   */
	/*--------------------------*/

	/* The AES Key size in words (defines security strength) */
	rndState_ptr->KeySizeWords   = 8;

	/* The TRNG operation mode flag: 
          Slow - when the flag is set, Fast - when not set */
	trngParams_ptr->TrngMode = CRYS_RND_Fast; 
    
	/* The ring oscillator length level - defined by 2-bits   */
	trngParams_ptr->RoscsAllowed = 0x0F;
	   
	/* The sampling ratio */
	if (trngParams_ptr->TrngMode == CRYS_RND_Fast) 
		trngParams_ptr->SubSamplingRatio = 30;
	else 	
		trngParams_ptr->SubSamplingRatio = 1000;

#endif

	return error;

} /* End of RNG_PLAT_SetUserRngParameters */


