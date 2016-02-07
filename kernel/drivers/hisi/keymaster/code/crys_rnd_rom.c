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
#include "dx_pal_mem.h"
#include "crys.h"
#include "crys_rnd_error.h" 
#include "crys_rnd_local.h" 
#include "crys_common.h"
#include "crys_common_math.h"
#include "rng_rom_api.h"
#include "cc_acl.h"

/*  
*  Object %name    : % CRYS_RND.c
*  State           :  %state%
*  Creation date   :  18 May 2010
*/
/** @file
*  \brief CRYS RND module varsion compliant to NIST 800-90 standard.
*         Based on CTR DRBG Block Cipher (AES)
*
*  \version CRYS_RND.c#1:csrc:6
*  \author R.Levin
*/

/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/*********************************** Enums ******************************/
/*********************************Typedefs ******************************/

/**************** Global Data to be read by RNG function ****************/
  
/****************************************************************************************/
/*****************************       Public Functions      ******************************/
/****************************************************************************************/


/*****************************************************************************************
 * @brief The CRYS_RND_GenerateVector function generates a random vector,
 *			 using the FIPS-PUB [SP800-90].
 *
 *        The random function is based on the AES. 
 *
 *
 * @RndSize[in] - The size of random bytes that is required.
 *
 * @Output_ptr[in,out] The output vector.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_GenerateVector( DxUint16_t  RndSize, DxUint8_t *Output_ptr )
{
	/* FUNCTION DECLERATIONS */
	
	/* error identifier definition */
	CRYSError_t Error;
	
	
	/* FUNCTION LOGIC */
	
	/* ................... initializing local variables ..................... */
	/* ---------------------------------------------------------------------- */
	
	/* initialize the error identifier to O.K */
	Error = CRYS_OK;
	
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_RND_UNSUPPORTED( RndSize , Output_ptr , Error , Error,
			      Error , Error , Error , Error , Error , Error ,
			      Error , Error , Error , Error , Error ); 
			      
	
	/* ................... checking parameters validity ..................... */
	/* ---------------------------------------------------------------------- */
	
	if( Output_ptr == DX_NULL )
		return CRYS_RND_DATA_OUT_POINTER_INVALID_ERROR; 
	
	if (RndSize == 0)
		return CRYS_OK ; /* because of PSS*/
	

	/* check validity for priv */
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, Output_ptr, RndSize)) {
		return CRYS_RND_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	

	/* ................... executing the random operation - calling the ROM API to get the random value................... */
	/* ------------------------------------------------------------------------------------------------------------------- */   
	Error = DX_RNG_GetRandomVector( RndSize, Output_ptr);
	
	
	return Error;
   
}/* END OF CRYS_RND_GenerateVector */



/* -----------------------------------------------------------------------------
 * @brief The function performs instantiation of RNG and creates new 
 *        internal State (including Seed) of RNG. 
 *
 *        It implements the CTR_DRBG_Instantiate function of 9.1 [1]. 
 *  	  This function must be called at least once per system reset (boot) and
 *        required before any random generation can be produced. 
 * 
 * @param[in/out] entrEstimBuff_ptr - The temp buffer for specific operations 
 *                               on entropy estimator.
 *                         Note: for projects, which not use entropy estimator (e.g.
 *                               SW projects), the pointer may be set to NULL.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_Instantiation( CRYS_RND_EntropyEstimatData_t  *entrEstimBuff_ptr/*in/out*/ )   
{
   return CRYS_RND_OPERATION_IS_NOT_SUPPORTED_ERROR;  
}

#ifndef _INTERNAL_CRYS_ONE_SEED
/* -----------------------------------------------------------------------------
 * @brief The CRYS_RND_UnInstantiation cleans the unused RNG State for security goals.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_UnInstantiation(void)
{
    return CRYS_RND_OPERATION_IS_NOT_SUPPORTED_ERROR;	                                     
}


/* ------------------------------------------------------------/
 * @brief The function performs reseeding of RNG Seed, and performs: 
 * 	     1. Mixing of additional entropy into the working state.
 *		 2. Mixing additional input provided by the user called additional input buffer.        
 *
 *        The function implements the CTR_DRBG_Reseeding function of 9.2 [1]. 
 *  	  This function must be called if reseed counter > reseed interval,
 *        in our implementation it is 2^32-1. 
 *
 * @param[in/out] entrEstimBuff_ptr - The temp buffer for specific operations 
 *                               on entropy estimator.
 *                         Note: for projects, which not use entropy estimator (e.g.
 *                               SW projects), the pointer may be set to NULL.
 * 
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_Reseeding(  CRYS_RND_EntropyEstimatData_t  *entrEstimBuff_ptr/*in/out*/ )
{
   /* FUNCTION DECLERATIONS */  
   return CRYS_RND_OPERATION_IS_NOT_SUPPORTED_ERROR;
}/* END OF CRYS_RND_Reseeding */

#endif /*_INTERNAL_CRYS_ONE_SEED*/


/**********************************************************************************************************/
/**
 * @brief The CRYS_RND_Init initializes the module         
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_Init( void )
{           
   return CRYS_RND_OPERATION_IS_NOT_SUPPORTED_ERROR; 
}/* END OF CRYS_RND_Init */


#ifndef _INTERNAL_CRYS_ONE_SEED
/**
* @brief The LLF_RND_SetAdditionalInput - This function load the AdditionaInput
*											from user to the global State buffer, sets
*											its Size and Valid flag to 1.
*
* @param[in/out] State - The pointer to the internal State buffer of DRNG.
* @param[in] AdditonalInput_ptr - The pointer to Additional input buffer.
* @param[in] AdditonalInputSize - The size of Additional input in words - must be up to 12 words.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure a
*                        value MODULE_* as defined in ...
*/
CEXPORT_C CRYSError_t CRYS_RND_AddAdditionalInput( 
										DxUint8_t *AdditonalInput_ptr,
										DxUint16_t AdditonalInputSizeBytes )
{
	return CRYS_RND_OPERATION_IS_NOT_SUPPORTED_ERROR;  
}

/**********************************************************************************************************/
/**
 * @brief The CRYS_RND_EnterKatMode function sets KAT mode bit into StateFlag
 *        of global CRYS_RND_WorkingState structure.
 * 
 *   In this, Non DMA, version of function all parameters are dummy and not
 *   used, but needed for compilation goals. 
 * 
 * @param entrData_ptr  - entropy data,
 * @param entrSize      - entropy size in bytes,
 * @param nonce_ptr     - nonce,
 * @param nonceSize     - nonce size in bytes,
 * @param workBuff_ptr  - RND working buffer, must be the same buffer, 
 *      		  which should be passed into Instantiation/Reseeding functions.
 *  
 *    Note: Total size of entropy and nonce must be not great than:
 *    	    CRYS_RND_MAX_KAT_ENTROPY_AND_NONCE_SIZE, defined 
 *  
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_EnterKatMode(
			DxUint8_t            *entrData_ptr,
			DxUint32_t            entrSize,
			DxUint8_t            *nonce_ptr,
			DxUint32_t            nonceSize,
			CRYS_RND_WorkBuff_t  *workBuff_ptr/*out*/)
{
	/* FUNCTION LOGIC */

	entrData_ptr = entrData_ptr;
	entrSize = entrSize;
	nonce_ptr = nonce_ptr;
	nonceSize = nonceSize;
	workBuff_ptr = workBuff_ptr;
	
	return CRYS_OK;

}/* END OF CRYS_RND_EnterKatMode  */

/**********************************************************************************************************/
/**
 * @brief The CRYS_RND_DisableKatMode function sets KAT mode bit into StateFlag
 *        of global CRYS_RND_WorkingState structure.
 * 
 *   The user must call this function before calling functions performing KAT tests.  
 * 
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C void CRYS_RND_DisableKatMode( void )
{
   /* FUNCTION LOGIC */
   /* CRYS_RND_OPERATION_IS_NOT_SUPPORTED_ERROR */      
   return;

}/* END OF CRYS_RND_DisableKatMode  */



/**********************************************************************************************************/
/* 
 * @brief The CRYS_RND_GenerateVectorInRange function generates a random vector Rand in range:
 *            1 < RandVect < MaxVect,   using the FIPS-PUB 186-2 standard appendix 3 :
 *        The function performs the following:
 *        1.  Check input parameters.
 *        2.  If maxVect != 0 (maxVect is provided), then calculate required size of random 
 *            equaled to actual bit size of MaxVector, else set it = rndSizeInBits.
 *        3.  Calls the CRYS_RND_GenerateVector() function for generating random vector 
 *            RndVect of required size.
 *        4.  If maxVect is provided, then:
 *             4.1. Sets all high bits of RndVect, greatest than MSBit of MaxVector, to 0.
 *             4.2. If size of random vector > 16 bytes, then: 
 *                     4.2.1. Compares high 16 bytes of randVect to maxVect and low limit 
 *                     4.2.2. If condition 1 < randVect16 < maxVect16 is not satisfied, 
 *                            then generate new high 16 bytes rndVect16 and go to step 4.2.1.
 *             4.3. Compares full value of RndVect with MaxVector and with 1 . If condition  
 *                 1 < RandVect < MaxVector is not satisfied, then go to step 3, else go to 6.
 *        5. Else if maxVect is not provided, then set MSBit of rndVect to 1.
 *        6. Output the result and Exit. 
 *
 *         Note: Random and Max vectors are given as sequence of bytes, where LSB is most left byte
 *               and MSB = most right one. 
 *
 * @rndSize[in]         - The maximal size (in bits) of random vector if maxVect is given or exact size 
 *                        of required random vector.
 * @maxVect_ptr[in]     - The pointer to vector defines high limit of random vector.
 * @rndVect_ptr[in,out] - The output random vector.
 * @return CRYSError_t  - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_GenerateVectorInRange(DxUint32_t  rndSizeInBits, DxUint8_t *maxVect_ptr, DxUint8_t *rndVect_ptr )
{
   /* FUNCTION DECLARATIONS */

	CRYSError_t Error;
	DxInt32_t   i, k, extraBytes;
	DxInt8_t    shift;
	DxUint8_t   mask, flag0, flag1;
	DxInt32_t   rndSizeInBytes, checkingSizeBytes;
	DxInt32_t   maxVectSizeBits, maxVectSizeBytes;
	CRYS_COMMON_CmpCounter_t CompRes;
    DxUint8_t   smin;
	/* total size of generated random data */
	DxInt32_t   totalRandSize, roundedSize;

 
   /* FUNCTION LOGIC */
 
   /* Initializations */
	Error = CRYS_OK;
	maxVectSizeBytes = 0;
   
   /* the lower limit of the random vector value */
   smin = 1;

   /*----------------------------------------------------------------*/
   /* calculation of actual size of random (excluding leading zeros) */
   /*----------------------------------------------------------------*/
   
	/* user passed size of random in bytes */
   rndSizeInBytes = rndSizeInBits / 8;
   if( rndSizeInBits % 8 )
		rndSizeInBytes++;

   if( maxVect_ptr != DX_NULL )
   {
	   /* check validity for priv */
	   if (DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, maxVect_ptr, rndSizeInBytes)) {
		   return CRYS_RND_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	   }

	   /* calculate actual size of MaxVector in bits*/
	   maxVectSizeBits = CRYS_COMMON_GetBytesCounterEffectiveSizeInBits( maxVect_ptr, (DxUint16_t)rndSizeInBytes );

	   maxVectSizeBytes = maxVectSizeBits / 8;
	   if( maxVectSizeBits % 8 )
		   maxVectSizeBytes++;

	   /* calculate count of extra 0-bytes in maxVector */
	   extraBytes = rndSizeInBytes - maxVectSizeBytes;

	   /* intermediate checking size */
	   if( maxVectSizeBytes > CRYS_AES_BLOCK_SIZE_IN_BYTES )
		   checkingSizeBytes = CRYS_AES_BLOCK_SIZE_IN_BYTES;
	   else
		   /* intermediate checking size */
		   checkingSizeBytes = maxVectSizeBytes;
   }
   /* case that maxVect not provided and rndVect must have exact size = rndSizeInBits */
   else
   {
	   extraBytes = 0;
	   /* intermediate checking size */
	   checkingSizeBytes = rndSizeInBytes;
   }

  /* calculate count of extra bits for shifting */
   shift = (DxInt8_t)(8 - (rndSizeInBits & 7))&7;   
   mask = 0xFF >> shift; 

   /* size of rand vector rounded up in AES blocks */
   roundedSize = CRYS_AES_BLOCK_SIZE_IN_BYTES * ((rndSizeInBytes + CRYS_AES_BLOCK_SIZE_IN_BYTES - 1) / CRYS_AES_BLOCK_SIZE_IN_BYTES);
  
   /* initialization of total size of random data, generated in each call of this function.
      Total size should be less, than 2^28 (according to CRYS requirements) */
   totalRandSize = 0; 

   /* main loop for generating random number    */
   /*-------------------------------------------*/
   k = 0;

   while( k < 0xFFFF )
   {
	   /* init checking flags */
	   flag0 = 0;
	   flag1 = 0;

	   /* generate full size random vector */
       Error = DX_RNG_GetRandomVector( (DxUint16_t)rndSizeInBytes, rndVect_ptr);	   
	   if( Error != CRYS_OK )    
		   goto End;

       /* count of generated random blocks of size 16 bytes each one */
	   totalRandSize += roundedSize;

       /* set non significant bytes to 0 */
	   for ( i = 0; i < extraBytes; i++ )
	   {
		   rndVect_ptr[rndSizeInBytes-1 - i] = 0;
	   }

	   /* mask the non significant high bits */
       rndVect_ptr[rndSizeInBytes - extraBytes - 1] &= mask;
   
	   /* in case, that maxVect is provided, check 1 < rndVect < maxVect, 
	      else generate new random */
	   if( maxVect_ptr != DX_NULL )
	   {
		  /* step1 check high part of random */
		   if( checkingSizeBytes == CRYS_AES_BLOCK_SIZE_IN_BYTES )
		   {
			   while( 1 )
			   {
				   CompRes = CRYS_COMMON_CmpLsbUnsignedCounters( 
					                  rndVect_ptr + rndSizeInBytes - CRYS_AES_BLOCK_SIZE_IN_BYTES, 
					                  CRYS_AES_BLOCK_SIZE_IN_BYTES, 
									  maxVect_ptr  + rndSizeInBytes - CRYS_AES_BLOCK_SIZE_IN_BYTES,
									  CRYS_AES_BLOCK_SIZE_IN_BYTES );
				
				   if( CompRes == CRYS_COMMON_CmpCounter2GraterThenCounter1 )
				   {
					   flag0 = 1;
					   goto CheckMinimal;  /* random is found */
				   }

				   else if ( CompRes == CRYS_COMMON_CmpCounter1AndCounter2AreIdentical )
					   break; /* go to check full size */

				   /* generate new 16 random high bytes */
                   Error = DX_RNG_GetRandomVector( CRYS_AES_BLOCK_SIZE_IN_BYTES, rndVect_ptr + rndSizeInBytes - CRYS_AES_BLOCK_SIZE_IN_BYTES);				   
				   if( Error != CRYS_OK )    
					   goto End;


				   /* set non significant bytes of random to 0 */
				   for ( i = 0; i < extraBytes; i++ )
				   {
					   rndVect_ptr[rndSizeInBytes - 1 - i] = 0;
				   }

				   /* mask the non significant high bits */
				   rndVect_ptr[rndSizeInBytes - extraBytes - 1] &= mask;
			   }
		   }
		   	
		   /* check full size relating to max vector */
		   CompRes = CRYS_COMMON_CmpLsbUnsignedCounters( rndVect_ptr, (DxUint16_t)rndSizeInBytes, 
			                                             maxVect_ptr, (DxUint16_t)maxVectSizeBytes );
		  
		   if ( CompRes == CRYS_COMMON_CmpCounter2GraterThenCounter1 )
			   flag0 = 1;

CheckMinimal:
		   /* check minimal value */ 
		   CompRes = CRYS_COMMON_CmpLsbUnsignedCounters( rndVect_ptr, (DxUint16_t)rndSizeInBytes, 
			                                             &smin, 1 );
		   
		   if( CompRes == CRYS_COMMON_CmpCounter1GraterThenCounter2 )
			   flag1 = 1;

		   if( flag0 && flag1 )
			   goto End;
	   }

	   else  /* in case, that maxVector is not provided, set MSBit of rndVect to 1 */
	   {
		   rndVect_ptr[rndSizeInBytes - extraBytes - 1] |= (0x80 >> shift);
		   goto End;
	   }

	   /* increment loop counter */
	   k++;
   }
   
	/* if all tries are Fail, then return the Error */
    Error = CRYS_RND_CAN_NOT_GENERATE_RAND_IN_RANGE;

 End:

    return Error;

} /* End of CRYS_RND_GenerateVectorInRange function */

#endif /*_INTERNAL_CRYS_ONE_SEED*/


/*************************************************************************************************/
/*******                     Functions for backward compatibility                  ***************/
/*************************************************************************************************/

#ifdef CRYS_RND_AES_OLD_128BIT_ONLY   

/**********************************************************************************************************/
/**
 * @brief The CRYS_RND_ResetSeed function performs Instantiate operation.
 *  
 *        This function needed for backward compatibility with some projects.
 *        It is recommended to call directly the CRYS_RND_Instantiation function.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_ResetSeed( void )
{
   /* FUNCTION DECLERATIONS */

   /* error identifier definition */
   CRYSError_t Error;

  	Error = CRYS_RND_Instantiation();

   return Error;

}/* END OF CRYS_RND_ResetSeed */


/**********************************************************************************************************/
/**
 * @brief The CRYS_RND_StartResetSeed is a dummy function needed 
 *        for backward compatibility with some projects
 * 
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_StartResetSeed( void )
{
   /* FUNCTION DECLARATIONS */

   return CRYS_OK;

}/* END OF CRYS_RND_StartResetSeed */
#endif


/**********************************************************************************************************/
/** 
 * @brief The DX_RND_GenerateLeWordsArrayInRange function generates a random words vector in range:
 *            1 < RndVect < MaxVect,   using the FIPS-PUB 186-2 standard appendix 3 :
 * 
 *        The function generates random array  using CRYS_RND_GenerateVectorInRange function and
 *        conversion of bytes to words.
 * 
 *         Note: RndVect and MaxVect arrayss are given as sequence of words, where LSWord is most left byte
 *               and MSWord - most right. 
 *
 * @rndSizeInBits[in]   - If maxVect_ptr is not given, then rndSizeInBits defining the exact size (in bits) 
 *                        of generated random vector. If maxVect is given, then it defines the 
 *	                  size (rounded up to words) of the maxVect_ptr buffer. The size must be not greate 
 *      		  than CRYS_RND_MAX_SIZE_OF_OUTPUT_BYTES/4
 * @maxVect_ptr[in]     - The pointer to vector defining a high limit of random vector.
 * @rndVect_ptr[out]    - The output buffer for the random vector. 
 * @tmp_ptr[int]        - The temp buffer for the random generation. The size must be not
 *      		  less, than rndSizeInBits converted to words (rounded up). 
 * 
 * @return CRYSError_t  - On success CRYS_OK is returned, on failure - a value,
 *      		  defined in crys_rnd_error.h.
 */

CRYSError_t DX_RND_GenerateWordsArrayInRange(
					DxUint32_t   rndSizeInBits, 
					DxUint32_t  *maxVect_ptr, 
					DxUint32_t  *rndVect_ptr,
					DxUint32_t  *tmp_ptr)
{
	/* FUNCTION DECLARATIONS */

	CRYSError_t err;
	DxUint32_t rndSizeInWords = (rndSizeInBits + 31) / 32;


	/* FUNCTION LOGIC */

	/* check parameters */
	if(rndVect_ptr == DX_NULL)
		return CRYS_RND_DATA_OUT_POINTER_INVALID_ERROR;

	/* given size of random vector in bytes */
	if(rndSizeInBits == 0 || rndSizeInWords*4 > CRYS_RND_MAX_GEN_VECTOR_SIZE_BYTES)
		return CRYS_RND_VECTOR_SIZE_ERROR;

	/* copy the maxVector into temp buffer and set endiannes as LE bytes  * 
	*  array							      */
	DX_PAL_MemMove((DxUint8_t*)tmp_ptr, (DxUint8_t*)maxVect_ptr, rndSizeInWords*sizeof(DxUint32_t));

#ifdef BIG__ENDIAN
	CRYS_COMMON_INVERSE_UINT32_IN_ARRAY(tmp_ptr, rndSizeInWords);
#endif
	/* generate vector in range [1...MaxVect] as LE bytes array */
	rndVect_ptr[rndSizeInWords-1] = 0;
	err = CRYS_RND_GenerateVectorInRange(rndSizeInBits, (DxUint8_t*)tmp_ptr, (DxUint8_t*)rndVect_ptr);

	if(err)
		return err;

	/* set endianness in output words according to LE words array */
#ifdef BIG__ENDIAN
	CRYS_COMMON_INVERSE_UINT32_IN_ARRAY(rndVect_ptr, rndSizeInWords);
#endif

	return err;
}

