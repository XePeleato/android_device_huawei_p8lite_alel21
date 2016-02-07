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
 
#ifndef LLF_RND_H
#define LLF_RND_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "crys.h"
#include "crys_rnd_local.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : llf_rnd.h
   *  State           :  %state%
   *  Creation date   :  12 Jun. 2012
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief The file includes definitions of LLF functions called
   *   by CRYS level.
   *
   *  \version llf_rnd.h#1:incl:2
   *  \author R.Levin
   */

/************************ Defines ******************************/

/* Definitions describing the TRNG Entropy estimator parameters: 
width of bits prefix and correlation table size */
#define CRYS_RND_nb   8 
#define CRYS_RND_NB  (1 << CRYS_RND_nb)
#define H_BUFF_SIZE_WORDS  CRYS_RND_NB
#define EC_BUFF_SIZE_WORDS (CRYS_RND_NB/2)

/* Macro defining Multiplication  using 16x16 multiplier  */
#define    Mult16x16(a, b) (((a)&0xffff)*((b)&0xffff))
DxUint64_t Mult32x32(DxUint32_t a, DxUint32_t b);
DxUint64_t Mult48x16(DxUint64_t a, DxUint32_t b);


/************************ Enums ********************************/
/************************ Typedefs  ****************************/
/************************ Structs  *****************************/
/* structure containing parameters and buffers for entropy estimator */
typedef struct
{ 
   /* estimated entropy size */
   DxUint32_t EstimEntropySizeBits;     
   /* estimated error of entropy size */
   DxUint32_t EstimEntropySizeErrorInBits;  

   /* special buffers */
   DxUint32_t h[CRYS_RND_NB];         /* histogram */
   DxUint32_t ec[CRYS_RND_NB/2];      /* equality counter for prefix */
   
}LLF_rnd_entr_estim_db_t;

/******************** Public Functions *************************/

/**
 * @brief The LLF_RND_GetRngBytes returns size of random source needed for collection
 *        required entropy .
 *
 *        The function returns size of source needed for required entropy.
 *
 * @param[in/out] trngParams - The pointer to structure, containing TRNG parameters.
 * @entropySizeWords[in/out] - The pointer to size of random source. The user sets 
 *                    size of entropy that is required and the function returns
 *                    the actual size of source needed for this count of entropy. 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_RND_GetEntropySourceSize( 
                                 CRYS_RND_Params_t    *trngParams, /*in*/         
                                 DxUint16_t   *entropySizeWords_ptr);   /*in/out*/


/****************************************************************************************/
/**
 * @brief The function gets user provided parameters of RNG HW. 
 *
 *   This implementation is in user competence. Temporary a pseudo function 
 *   is implemented for testing goals. To use this implementation the user must define
 *   compilation flag "DX_RND_TEST_MODE", otherwise 
 *    
 *   Note: In temporary implementation assumed, that users parameters are placed
 *         in global structure UserRngParameters (now placed in ATP tests).  
 *   
 * @param[in] KeySizeWords - The key size: 4 or 8 words according to security 
 *                           strength 128 bits or 256 bits;
 * @param[in] TrngMode -  TRNG mode: 0 - Fast mode, 1 - Slow mode.  
 * @param[in] RoscsAllowed - Ring oscillator length level: should be set 
 *            as 2 bits value: 0,1,2,3.
 * @param[in] SampleCount - The sampling count - count of RND blocks of RNG HW 
 *            output, required for needed entropy accumulation:  
 *              - in "slow" mode a possible values are 4095 to 65535, in steps of 4096;
 *              - in "fast" mode, sampling counter limit is set to a low value - 
 *                typically 1 or 2. 
 * @param[in] MaxTrngTimeCoeff - coefficient defining relation between maximal allowed and expected  
 *  				time for random generation (in percents).  
 *
 * @return CRYSError_t - CRYS_OK  
 */
 CRYSError_t  LLF_RND_GetRngParams( 
			DxUint32_t  *KeySizeWords,
			DxUint32_t  *TrngMode,
			DxUint32_t  *RoscsAllowed,
			DxUint32_t  *SampleCount,
			DxUint32_t  *MaxTrngTimeCoeff);


/****************************************************************************************/
/**
 * 
 * @brief The function sets user provided parameters of TRNG into
 *        global State structure and into trngParams_ptr structure. 
 * 
 * @author reuvenl (6/26/2012)
 * 
 * @param[out] rndState_ptr - The pointer to structure containing RNG
 *                            parameters.
 * @param[out] trngParams_ptr - The pointer to structure containing parameters
 *                            of HW TRNG.
 *
 * @return CRYSError_t - no return value  
 */
CRYSError_t LLF_RND_SetUserRngParameters(
			CRYS_RND_State_t   *rndState_ptr,         
			CRYS_RND_Params_t  *trngParams_ptr); 



/****************************************************************************************/ 
/**
 * @brief The function starts the TRNG with given parameters and ROSCs lengths
 * 
 *      NOTE: It is assumed, that before calling this function, the previously
 *            started TRNG processes were compleated and Interrupts cleared.
 * 
 *      Algorithm:
 *      1. If is continued mode, the function does:
 *              checks actual parameters, loaded in TRNG registers,
 *              vs. user given parameters; if any not matchs - returns Error.
 *         Else /do restart/:
 *      	sets ROSCs to start: for "fast" - all allowed, for "slow" -
 *      	fastest from allowed; gets the user given parameters and sets
 *      	them in the HW, starts the TRNG clocks and sets TRNG parameters
 *      	into HW registers.
 *      2. Initializes the RND DMA according to ROSCs required to start,
 *         initializes the TRNG Interrupt. Enables RNG source.
 *      3. Exits.
 *       
 * @param[in/out] rndState_ptr - The pointer to the internal State buffer of DRNG.
 * @param[in/out] trngParams_ptr - The pointer to structure, containing TRNG parameters.
 * @isContinued[in] isRestart - The variable indicates is a restart required or not.
 * @roscsToStart[in] roscsToStart_ptr - The variable, defining which ROSCs to 
 *      			    start according to bits set: b'0...b'3. When
 *      			    isRestart=TRUE, then:
 *      			    for "fast" - starts all allowed ROSCs, for
 *      			    "slow" - starts fastest ROSC from allowed.
 *      			    Note: if isRestart = 1, then 
 * @sramAddr[in] SRAM address to write the random source.
 *  
 * @return CRYSError_t - no return value
 */
CRYSError_t LLF_RND_StartTrngHW(
			CRYS_RND_State_t  *rndState_ptr,
			CRYS_RND_Params_t *trngParams_ptr,
			DxBool_t           isRestart,
			DxUint32_t         *roscsToStart_ptr,
	                DxUint32_t         sramAddr);

/************************************************************************************/
/**
 * @brief The LLF_RND_TurnOffTrng stops the hardware random bits collection
 *        closes RND clocks and releases HW semaphore.
 *
 * 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void LLF_RND_TurnOffTrng(void);

/*******************************************************************************/
/**
 * @brief The LLF_RND_GetTrngSource reads random source of needed size from TRNG.
 *
 *        The function is used in Self, Instantiation and Reseeding functions.
 *
 * @param[in/out] rndState_ptr - The pointer to the internal State buffer of DRNG.
 * @param[in/out] trngParams_ptr - The pointer to structure, containing TRNG parameters.
 * @isContinued[in] isContinued - The variable indicates is the required process should  
 *                  continue a  previous one or restart TRNG.
 * @entropySize_ptr[in/out] - The pointer to size of entropy in bits: input - required,
 *                            output - actual size.
 * @sourceOut_ptr_ptr[out] - The pointer to to pointer to the entropy source buffer. 
 *      		     The buffer contains one empty word for using by CRYS level 
 *      		     and then buffer for output the rng source.  
 * @param[out] - sourceOutSize_ptr - The pointer to the size in bytes of entropy source    			   
 *      			    in - requirred size, output - actual size. 
 * @param[in/out] - rndWorkBuff_ptr - The pointer to the temp buffer for allocation of 
 *      			   estimator buffers.
 * 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_RND_GetTrngSource( 
			CRYS_RND_State_t   *rndState_ptr,     /*in/out*/ 
			CRYS_RND_Params_t  *trngParams_ptr,   /*in/out*/        
			DxBool_t            isContinued,      /*in*/
			DxUint32_t         *entropySize_ptr,  /*in/out*/
			DxUint32_t         **sourceOut_ptr_ptr, /*out*/ 								     
			DxUint32_t         *sourceOutSize_ptr,/*in/out*/
			DxUint32_t         *rndWorkBuff_ptr); /*in*/ 


#ifdef __cplusplus
}
#endif

#endif
