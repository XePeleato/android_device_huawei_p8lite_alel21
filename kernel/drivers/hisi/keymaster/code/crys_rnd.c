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

#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "dx_pal_log.h"
#ifndef  CRYS_RND_SEM_DISABLE
#include "dx_pal_sem.h"
#endif
#include "compiler.h"
#include "crys.h"
#include "crys_common.h"
#include "crys_common_math.h"
#include "crys_rnd.h"
#include "crys_rnd_error.h"
#include "crys_rnd_local.h" 
#include "llf_rnd.h"
#include "debug.h"//2012/11/16


/*
*  Object %name    : % crys_rnd.c
*  State           :  %state%
*  Creation date   :  18 May 2010
*/
/** @file
*  \brief CRYS RND module version compliant to NIST 800-90 standard.
*         Based on CTR DRBG Block Cipher (AES)
*
*  \version crys_rnd.c#1:csrc:6
*  \author R.Levin
*/

/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/*********************************** Enums ******************************/
/*********************************Typedefs ******************************/

/*********************************Typedefs ******************************/
/* rotate 32-bits word by 16 bits */
#define RND_ROT32(x) ( (x) >> 16 | (x) << 16 )

/* inverse the bytes order in a word */
#define RND_REVERSE32(x)  ( ((RND_ROT32((x)) & 0xff00ff00UL) >> 8) | ((RND_ROT32((x)) & 0x00ff00ffUL) << 8) )  

/**************** Global Data to be read by RNG function ****************/

/* global pointer to RNG working State buffer */
CRYS_RND_State_t  CRYS_RND_WorkingState DX_SRAM_SBSS_VARIABLE;


/* external functions prototypes */
CRYSError_t RNG_PLAT_SetUserRngParameters(
			CRYS_RND_State_t   *rndState_ptr,         
			CRYS_RND_Params_t  *trngParams_ptr);


/************************************************************************************/
/***********************           Private functions            *********************/
/************************************************************************************/

/**
 *      The function adds value to the number N, presented as bytes array n_ptr,
 *      where MSbyte is a most left one.
 *  
 *      Algorithm:
 *          n = (N + val) mod 2^(8*sizeBytes).
 *      Assumed: The array and its size are aligned to 32-bit words.
 *      	 val > 0.
 * 
 * @author reuvenl (7/1/2012) 
 * 
 * @param n_ptr 
 * @param sizeBytes 
 * @param val - value to add
 */
static void DX_RND_AddValToIv(DxUint8_t *iv_ptr, DxUint32_t val)
{
	DxInt32_t i;
	DxUint32_t *ptr = (DxUint32_t*)iv_ptr;
	DxUint32_t tmp, curr;

	for(i = 3; i >= 0; i--) {
#ifndef BIG__ENDIAN
		tmp = curr = RND_REVERSE32(ptr[i]);
#else
		tmp = curr = ptr[i];
#endif
		tmp += val;

#ifndef BIG__ENDIAN
		ptr[i] = RND_REVERSE32(tmp);
#else
		ptr[i] = tmp;
#endif
		
		if(tmp < curr)
			val = 1;
		else
			break;
	}
}


/****************************************************************************************/
/**
 * @brief The function performs NIST 800-90, 10.2.1.2. algorithm of Update function.
 *        
 * @param[in/out] State - The pointer to the internal State buffer of DRNG.
 * @param[in] providedData_ptr - The pointer to provided data buffer. The size of data 
 *                               must be exactly of size of Seed.
 * @param[in/out] seed_ptr - The pointer to the Seed = (Key || V) buffer.
 * @param[in] skipSetUp - Flag, if set, then first two steps of algorithm sould be skipped.
 * @param[in/out] aesCtxID_ptr - The pointer to AES context.
 *
 *   Note: Updated result (Key||V) are in Seed buffer of the State.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
static CRYSError_t DX_RND_Update(
			CRYS_RND_State_t      *rndState_ptr,        /*in/out*/
			DxUint8_t             *providedData_ptr,    /*in*/
			DxUint8_t             *seed_ptr,            /*in/out - Key,V*/
			DxUint8_t              skipSetUp)           /*in*/
 {

	 /* LOCAL DECLARATIONS */

	CRYSError_t  Error = CRYS_OK;   
	CRYS_AES_KeySize_t   keySizeID;
	DxUint32_t keySizeWords;
	/* size of seed */
	DxUint32_t seedSizeInWords;
	CRYS_AESUserContext_t aesCtxID;

	/* pointers to current key and iv  */
	DxUint8_t *k_ptr, *iv_ptr; 


	/* FUNCTION LOGIC */

	/* Initializations */

	keySizeWords = rndState_ptr->KeySizeWords;
	if (keySizeWords == 4)
		keySizeID = CRYS_AES_Key128BitSize;
	else 
		keySizeID = CRYS_AES_Key256BitSize;


	/* seed size in AES blocks */
	seedSizeInWords = keySizeWords + CRYS_AES_BLOCK_SIZE_IN_WORDS;

	/* set key and iv pointers */
	k_ptr = (DxUint8_t*)&seed_ptr[0];
	iv_ptr = (DxUint8_t*)&seed_ptr[keySizeWords*sizeof(DxUint32_t)];

	/*----------------------------------------------------------------- */
	/*    NIST 800-90, 10.2.1.2. Algorithm of Update function           */
	/*  Where: output performed into StateSeed buffer without using of  */
	/*        temp buffer                                               */
	/*----------------------------------------------------------------- */

	/* Init AES operation on CTR mode */
	Error = CRYS_AES_Init(  
			&aesCtxID,	  /*AES context*/                    
			iv_ptr, /*IV*/
			k_ptr, /*calculated key*/
			keySizeID,
			CRYS_AES_Encrypt,           
			CRYS_AES_CTR_mode); 

	if( Error != CRYS_OK ) 
		return Error;

	/* if not set skipSetUp flag, then perform one dummy encrypt for
	   incrementing IV */
	if( !skipSetUp ){
	
		/* Dummy encrypt for increment the IV:  		      * 
		   V = (V+1) mod 2^outLenBits  				      */
		Error =  CRYS_AES_Block(
			 &aesCtxID,
			 providedData_ptr,
			 CRYS_AES_BLOCK_SIZE_IN_BYTES,
			 seed_ptr);
	
		if( Error != CRYS_OK ) 
			return Error;
	}

	/* 2.2. Encrypt the SEED on AES CTR mode */
	Error = CRYS_AES_Finish(
			&aesCtxID,
			providedData_ptr, /*in*/
			seedSizeInWords*sizeof(DxUint32_t),
			seed_ptr);  /*out*/ 

	return Error;

} /* End of CRYS_RND_Update */



/****************************************************************************************/
/**
 * @brief The function performs NIST 800-90, 10.2.1.4.2. algorithm of 
 *        Seed Derivation function.
 *        
 * @param[in/out] rndState_ptr - The pointer to the internal State buffer.
 * @param[in] inputBuff_ptr - The pointer to input buffer, containing the input seed source 
 *      		      data, placed beginning from byte 8 and additional (at less 16)
 *      		      empty bytes for padding. The size of the buffer must be
 *      		      at less (8 + inputDataSizeBytes + 16) bytes.  
 * @param[in] inputDataSizeBytes - The size in bytes of the input data = actual size of 
 *                                 input seed source data to process (must be multiple of 4 bytes).
 * @param[out] output_ptr - The pointer to the output data buffer. 
 *                          The size (in bytes) of the buffer and output data
 *                          are equal to (AES key size + AES block size).
 * @param[in] outDataSizeBytes - The size of output data. According to NIST 800-90
 *                               the size must be <= 64 bytes.
 *
 *   Note: Overlapping of inputBuff and output is not allowed.
 *       
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
static CRYSError_t DX_RND_DF(
			CRYS_RND_State_t     *rndState_ptr,             /*in/out*/
			DxUint8_t            *inputBuff_ptr,            /*in*/
			DxUint32_t            inputDataSizeBytes,       /*in*/
			DxUint8_t            *output_ptr,               /*out*/
			DxUint32_t            outDataSizeBytes)         /*in*/
 {
	/* LOCAL DECLARATIONS */

	 CRYSError_t  Error = CRYS_OK;  

	 CRYS_AESUserContext_t    aesContext;
    
	/* AES key size in words (defining also security strength) and its ID */
	DxUint32_t keySizeWords;
	CRYS_AES_KeySize_t   keySizeID;
	/* pointers to precomputed initial MAC vectors (two-dimensional) and  * 
	*  current key and iv   					      */
	DxUint8_t *initMac_ptr, *k_ptr, *iv_ptr; 
	/* loop counter */
	DxUint32_t i;

	/*   Data for Security Strength = 128 and 256 bit.
	     Note: Key buffer is common for 128 and 256 bits*/
	const DxUint8_t Key[32] = 
	{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F, 
	 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};
	const DxUint8_t InitialMac128[2][CRYS_RND_BASIC_BLOCK_SIZE_IN_BYTES] =
	{{0xc6,0xa1,0x3b,0x37,0x87,0x8f,0x5b,0x82,0x6f,0x4f,0x81,0x62,0xa1,0xc8,0xd8,0x79}, 
	 {0x95,0x03,0xe3,0xa2,0x24,0x5a,0x2b,0xe4,0x3c,0x98,0x74,0xed,0xfe,0x1b,0xed,0x9e} };
	const DxUint8_t InitialMac256[3][CRYS_RND_BASIC_BLOCK_SIZE_IN_BYTES] =
	{{0xF2,0x90,0x00,0xB6,0x2A,0x49,0x9F,0xD0,0xA9,0xF3,0x9A,0x6A,0xDD,0x2E,0x77,0x80}, 
	 {0x9D,0xBA,0x41,0xA7,0x77,0xF3,0xB4,0x6A,0x37,0xB7,0xAA,0xAE,0x49,0xD6,0xDF,0x8D},
	 {0x2F,0x7A,0x3C,0x60,0x07,0x08,0xD1,0x24,0xAC,0xD3,0xC5,0xDE,0x3B,0x65,0x84,0x47} };


	/* FUNCTION LOGIC */
    PRINT_INFO("-->NOW INTO DX_RND_DF\n");
        /* Initializations */
    
	keySizeWords = rndState_ptr->KeySizeWords;
	if (keySizeWords == 4)
		keySizeID = CRYS_AES_Key128BitSize;
	else 
		keySizeID = CRYS_AES_Key256BitSize;


   	/*----------------------------------------------------------------- */
   	/* [1]: NIST 800-90, 10.2.1.4.2. Block_Cipher_df Process.           */
	/*      Algorithm of Seed Derivation function   		    */
	/*  Note: step 8 is done because init Key and IV are hard coded     */
   	/*----------------------------------------------------------------- */

	/* Set L, N and padding 0x80....0 in the input buffer.
	   Note: input data was set before; L, N values must be in bytes  */
#ifdef BIG__ENDIAN
	((DxUint32_t*)inputBuff_ptr)[0] = inputDataSizeBytes; /* L */
	((DxUint32_t*)inputBuff_ptr)[1] = outDataSizeBytes;   /* N */
#else	
	/* convert L,N to little endian */
	((DxUint32_t*)inputBuff_ptr)[0] = CRYS_COMMON_REVERSE32(inputDataSizeBytes); /* L */
	((DxUint32_t*)inputBuff_ptr)[1] = CRYS_COMMON_REVERSE32(outDataSizeBytes);   /* N */
#endif 

	inputBuff_ptr[8+inputDataSizeBytes] = 0x80;
	DX_PAL_MemSet(&inputBuff_ptr[8+1+inputDataSizeBytes], 0, 
		CRYS_AES_BLOCK_SIZE_IN_BYTES - ((8+1+inputDataSizeBytes)&0x0fUL));

	/* size of input to AES-MAC, rounded up to AES block */
	inputDataSizeBytes += (8/*2w*/ + 1/*0x80*/);
	inputDataSizeBytes = ((inputDataSizeBytes+CRYS_AES_BLOCK_SIZE_IN_BYTES-1) / CRYS_AES_BLOCK_SIZE_IN_BYTES) * CRYS_AES_BLOCK_SIZE_IN_BYTES;

	/*****************************************************
	* [1] 12: Compression of seed source material        * 
	******************************************************/

	for(i = 0; i < (outDataSizeBytes >> 4); i++) {

		/* set pointer to initial precomputed IV  value */
		if(keySizeWords == 4)
			initMac_ptr = (DxUint8_t*)&InitialMac128[i][0];
		else
			initMac_ptr = (DxUint8_t*)&InitialMac256[i][0];

		/* AES MAC */
		Error = CRYS_AES(
				initMac_ptr,       
				(DxUint8_t*)&Key[0],					
				keySizeID,            
				CRYS_AES_Encrypt, 
				CRYS_AES_MAC_mode,
				inputBuff_ptr,        
				inputDataSizeBytes,         
				output_ptr + i*CRYS_AES_BLOCK_SIZE_IN_BYTES );/*output*/

		if(Error)
			return Error;

	}

	/* set K and IV pointers on output buffer for AES_CTR encryption */
	k_ptr = (DxUint8_t*)output_ptr;
	iv_ptr = (DxUint8_t*)(output_ptr + keySizeWords*sizeof(DxUint32_t));


	/* Encrypt (K,IV) by AES-CBC using output buff */
	Error = CRYS_AES_Init(
			&aesContext,
			iv_ptr, /*IV*/       
			k_ptr,  /*key*/					
			keySizeID,            
			CRYS_AES_Encrypt, 
			CRYS_AES_CBC_mode);

	if(Error)
		return Error;

	DX_PAL_MemSet(output_ptr, 0, outDataSizeBytes);

	Error = CRYS_AES_Finish(
			&aesContext,
			output_ptr/*in*/,        
			outDataSizeBytes,         
			output_ptr/*out*/);

	return Error;


} /* END of DX_RND_DF */


/****************************************************************************************/
/**
 * @brief The function performs: NIST 800-90, 10.2.1.3.2  Instantiate function or
 *        NIST 800-90, 10.2.1.4.2 Reseeding function, according to given flag.
 *        
 * @param[in/out] rndState_ptr - The pointer to the internal State buffer of DRNG.
 * @param[in] isInstantiate - The flag defining which algorithm to perform: 
 *                            0 - Instantiate; 1 - Reseeding. 
 * @isContinued[in] isContinued - The variable indicates is the required process should  
 *                  continue a  previous one or restart TRNG.
 * @param[in/out] workBuff_ptr - The temp buffer for specific operations 
 *      			 on entropy generation and estimation.
 * 
 *      NOTE! The function works according to TRNG random source generation state as follows:
 * 
 *      	1. If isContinued = 0, i.e. indicates that the TRNG was not started  
 *      	   previously, the function starts it, else waits to end of TRNG generation.
 *      	2. Performs deterministic part of NIST CTR_DRBG Instantiation or Reseeding algorithm.
 * 
 *      NOTE!! To ensure, that the function not uses the results from previous started TRNG, the user must
 *             call CRYS_RND_Uninstantiation function previously to this function.
 * 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in CRYS_Error.h
 */
 static CRYSError_t DX_RND_InstantiateOrReseed(
			CRYS_RND_State_t     *rndState_ptr,    /*in/out*/
			DxBool_t              isInstantiate,   /*in*/
			DxBool_t              isContinued,     /*in*/
			CRYS_RND_WorkBuff_t  *workBuff_ptr )   /*in/out*/
{
   	/* LOCAL DECLARATIONS */

	/* error identifier definition */
	CRYSError_t Error = CRYS_OK;

	DxUint32_t  *entrSource_ptr;
	DxUint32_t  keySizeWords, entropySizeBits;
	DxUint32_t  seedSizeBytes, sourceSizeBytes;

  	/* TRNG parameters structure */
	CRYS_RND_Params_t  trngParams; 


	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   

	RETURN_IF_RND_UNSUPPORTED( rndState_ptr, isInstantiate, isContinued, workBuff_ptr,  
		  entrSource_ptr, keySizeWords, entropySizeBits, seedSizeBytes,  
		  sourceSizeBytes, trngParams.TrngMode, Error, Error, Error, Error, Error ); 

	/* FUNCTION LOGIC */
    PRINT_INFO("-->NOW INTO DX_RND_InstantiateOrReseed\n");
	/* ............. check parameters ............... */

	if(rndState_ptr == DX_NULL) 
		return CRYS_RND_STATE_PTR_INVALID_ERROR;

	if(workBuff_ptr == DX_NULL) 
		return CRYS_RND_WORK_BUFFER_PTR_INVALID_ERROR;

	/* for Reseeding check valid tag and Instantiation done bit */
	if (isInstantiate == DX_FALSE){
		if(rndState_ptr->ValidTag != CRYS_RND_WORK_STATE_VALID_TAG)
			return CRYS_RND_STATE_VALIDATION_TAG_ERROR;
		
		if(!(rndState_ptr->StateFlag & CRYS_RND_Instantiated)) 
			return CRYS_RND_INSTANTIATION_NOT_DONE_ERROR;
	}

	/* set users TRNG parameters into rndState_ptr structure */
	if(isContinued == DX_FALSE) {
		Error = RNG_PLAT_SetUserRngParameters(rndState_ptr, &trngParams);
		if(Error != CRYS_OK)
			return Error;
	}

	/* key size */
	keySizeWords = rndState_ptr->KeySizeWords;
	
 	/* Set Entropy size according to operation mode and security strength:
   	     - for reseeding: entropySize = keySize;  
	     - for instantiation: entropySize = keySize + keySize/2, note: in
	       the last case the size includes a random Nonce*/
   	if(isInstantiate == 0) 
		entropySizeBits = keySizeWords*32; /*Reseeding*/       
	else
   		entropySizeBits = (keySizeWords + (keySizeWords>>1))*32; /*Instantiation*/

	/* Get entropy (including random Nonce) from TRNG and set 
	  it into Entropy Temp buffer. Update the needed size of 
	  TRNG source for receiving required entropy. Note:     */
	/*--------------------------------------------------------*/
	Error = LLF_RND_GetTrngSource( 
		rndState_ptr,      /*in/out*/
		&trngParams,       /*in/out*/
		isContinued,       /*in*/         
		&entropySizeBits,  /*in/out*/ 
		&entrSource_ptr,   /*out*/
		&sourceSizeBytes,  /*out*/
		(DxUint32_t*)workBuff_ptr /*in*/);

	if(Error != CRYS_OK)
		{
		PRINT_INFO("-->LLF_RND_GetTrngSource return:0x%x\n",Error);
		goto EndWithError;
		}

	/* Set additional data into work buffer */
	DX_PAL_MemCopy(
		(DxUint8_t*)&entrSource_ptr[(sourceSizeBytes>>2)+CRYS_RND_TRNG_SRC_INNER_OFFSET_WORDS], 
		(DxUint8_t*)&rndState_ptr->AdditionalInput[0],
		sizeof(DxUint32_t)*rndState_ptr->AddInputSizeWords);    

   	/*--------------------------------------------------------------------- */
   	/*   [1] NIST 800-90: 10.2.1.3.2  Instantiate or  10.2.1.4.2 Reseeding  */
   	/*--------------------------------------------------------------------- */
	/* set input and output data sizes for DF */
	sourceSizeBytes += sizeof(DxUint32_t)*rndState_ptr->AddInputSizeWords;
	seedSizeBytes = keySizeWords*sizeof(DxUint32_t) + CRYS_AES_BLOCK_SIZE_IN_BYTES;

	/* 2.1. if Derivation Function is used, call it */      
	Error = DX_RND_DF(
		rndState_ptr,              /*in*/
		(DxUint8_t*)entrSource_ptr,  /*in buffer - data starts from */
		sourceSizeBytes,           /*in - size of entropy  */
		(DxUint8_t*)&rndState_ptr->AdditionalInput[0],  /*out - seed material*/
		seedSizeBytes);            /*in*/ 
 
	if(Error != CRYS_OK)
		{
		PRINT_INFO("-->DX_RND_DF return:0x%x\n",Error);
		goto EndWithError;
		}
    
  	/* 3,4: Set Key = 0x00000... and IV = 0x0000... into Seed buffer */
	if(isInstantiate == 1)
		DX_PAL_MemSetZero(rndState_ptr->Seed, sizeof(DxUint32_t)*(keySizeWords + CRYS_AES_BLOCK_SIZE_IN_WORDS));

	/* 2.2. Call Update for Additional data */
	Error = DX_RND_Update(
		rndState_ptr,       /*in/out*/
		(DxUint8_t*)&rndState_ptr->AdditionalInput[0], /*in - provided data*/
		(DxUint8_t*)&rndState_ptr->Seed[0], /*in/out - Key||V inside the state: */
		0);                /*in - skipSetUp*/

	if(Error != CRYS_OK)
		{
		PRINT_INFO("-->DX_RND_Update return:0x%x\n",Error);
		 goto EndWithError;
		}

	/* [1] 6:  Reset State parameters           */
	/*------------------------------------------*/
    
	rndState_ptr->ReseedCounter = 1; 
    
	/* Set Instantiation flag = 1 (because it was zeroed in TRNG) */
	rndState_ptr->StateFlag |= CRYS_RND_Instantiated;

	/* Set a valid tag and disable previous value flag (only for
	   Instantiation mode) */
	if(isInstantiate == 1) {
		rndState_ptr->ValidTag = CRYS_RND_WORK_STATE_VALID_TAG;
		rndState_ptr->StateFlag &= ~CRYS_RND_PreviousIsValid; 
	}
    
	/* Clean additional input and working buffer */
	rndState_ptr->AddInputSizeWords = 0;
	DX_PAL_MemSetZero(rndState_ptr->AdditionalInput, sizeof(rndState_ptr->AdditionalInput));
	DX_PAL_MemSetZero((DxUint8_t*)workBuff_ptr, sizeof(CRYS_RND_WorkBuff_t));

	/* ................. end of function ........... */
	/* --------------------------------------------- */
	return Error;

EndWithError:

	/* In case of error, clean the secure sensitive data from rndState */
	DX_PAL_MemSetZero(rndState_ptr->Seed, sizeof(rndState_ptr->Seed));
	DX_PAL_MemSetZero(rndState_ptr->AdditionalInput, sizeof(rndState_ptr->AdditionalInput));
	rndState_ptr->AddInputSizeWords = 0;
	rndState_ptr->StateFlag &= ~CRYS_RND_Instantiated;
	rndState_ptr->ValidTag = 0xdeedbeef; /* set wrong value */

   	return Error;

} /* End of DX_RND_InstantiateOrReseed function */


/**
* @brief: The function performs CPRNGT (Continued PRNG Test) according 
*         to NIST 900-80 and FIPS (if defined) standards. 
*        
* @param[in] prev_ptr - The pointer to previous saved generated random 
*                       value of size 16 bytes. 
* @param[in] buff_ptr - The pointer to generated random buffer. 
* @param[in] last_ptr - The pointer to last generated random block
*                       of size 16 bytes used for output last bytes.
* @param[in] countBlocks - The count of generated random blocks, including
*                          the last block. Assumed countBlocks > 0.
*                        
* @return CRYSError_t - On success CRYS_OK is returned, on failure a
*                        value MODULE_* as defined in CRYS_Error.h
*/
static CRYSError_t DX_RND_CPRNGT(
			   DxUint8_t            *prev_ptr,      /*in*/
			   DxUint8_t            *buff_ptr,      /*in*/
			   DxUint8_t            *last_ptr,      /*in*/
			   DxInt32_t             countBlocks)   /*in*/
{
	/* LOCAL DECLARATIONS */

	CRYSError_t error = CRYS_OK;
	DxInt32_t  i;

	/*  FUNCTION LOGIC */

	/* compare the previous Value and last block */
	if(countBlocks == 1){		
		if(DX_PAL_MemCmp(
			prev_ptr, /*prev*/ 
			last_ptr,/*last block*/ 
			CRYS_AES_BLOCK_SIZE_IN_BYTES) == 0){
				error =  CRYS_RND_CPRNG_TEST_FAIL_ERROR;
				goto End;
		}
	}		
	else{ /* countBlocks > 1, compare first and last blocks */
		if(DX_PAL_MemCmp(
			prev_ptr,  /*prev*/
			buff_ptr, /*first block*/
			CRYS_AES_BLOCK_SIZE_IN_BYTES) == 0){
				error =  CRYS_RND_CPRNG_TEST_FAIL_ERROR;
				goto End;
		}

		if(DX_PAL_MemCmp(
			buff_ptr + (countBlocks-2)*CRYS_AES_BLOCK_SIZE_IN_BYTES, /*prev*/
			last_ptr,/*last block*/ 
			CRYS_AES_BLOCK_SIZE_IN_BYTES) == 0){
				error =  CRYS_RND_CPRNG_TEST_FAIL_ERROR;
				goto End;
		}
	}
	/* compare intermediate blocks */
	if(countBlocks > 2 && error == CRYS_OK){
		for(i = 0; i < countBlocks-2; i++){
			/* compare all current with previous blocks */
			if(DX_PAL_MemCmp(
				buff_ptr + i*CRYS_AES_BLOCK_SIZE_IN_BYTES, 
				buff_ptr + (i+1)*CRYS_AES_BLOCK_SIZE_IN_BYTES, 
				CRYS_AES_BLOCK_SIZE_IN_BYTES) == 0){
					error = CRYS_RND_CPRNG_TEST_FAIL_ERROR;
					goto End;
			}
		} 
	} 

End:

	/* On FIPS mode set Error in DX_GLOBAL_FIPS_MODE state */
	if(error) {
	  #ifndef CRYS_NO_FIPS_SUPPORT
		DX_GLOBAL_FIPS_MODE|= DX_CRYS_FIPS_MODE_ERROR_STATE;
	  #endif	
	}

	return error;
} 


/****************************************************************************************/
/*****************************       Public Functions      ******************************/
/****************************************************************************************/



/****************************************************************************************/
/**
 * @brief The function performs NIST 800-90, 10.2.1.5.2 algorithm of Generate function.
 * 
 * NOTE: The function should change the data in given output buffer also if an error occurs.
 *        
 * @param[out] outSizeBytes - The required size of random data in bytes.
 * @param[in] out_ptr - The pointer to output buffer. 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_GenerateVector(
				DxUint16_t   outSizeBytes, /*in*/
				DxUint8_t   *out_ptr)      /*out*/
{

   	/* LOCAL DECLARATIONS */

	CRYSError_t  error = CRYS_OK; 

	DxUint32_t keySizeWords, seedSizeWords;   
	DxUint32_t remainBytes, countBlocks;
	CRYS_AES_KeySize_t   keySizeID;
	CRYS_AESUserContext_t  AesUserContext;

	CRYS_RND_State_t  *rndState_ptr = &CRYS_RND_WorkingState;
	DxUint32_t temp[CRYS_RND_SEED_MAX_SIZE_WORDS+4];
    
    
	/* FUNCTION LOGIC */


	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   

	RETURN_IF_RND_UNSUPPORTED( outSizeBytes, out_ptr, keySizeWords, seedSizeWords, 
			           remainBytes, countBlocks,  keySizeID, AesUserContext.buff[0],   
			           rndState_ptr, error, error, error, error, error, error ); 


	/* ................... checking parameters validity ..................... */
	/* ---------------------------------------------------------------------- */

	if(out_ptr == DX_NULL)
		return CRYS_RND_DATA_OUT_POINTER_INVALID_ERROR; 

	if(outSizeBytes == 0)
		return CRYS_OK ; /* because of PSS*/

	if(rndState_ptr->ValidTag != CRYS_RND_WORK_STATE_VALID_TAG)
		return CRYS_RND_STATE_VALIDATION_TAG_ERROR;

	/* Check, that instantiation was done */
	if(!(rndState_ptr->StateFlag & CRYS_RND_Instantiated))
		return CRYS_RND_INSTANTIATION_NOT_DONE_ERROR;		
    
	/* [1] 1: Check Reseed counter in the rndState. 
	Note: In [1] reseedCounter must be less than 2^48. In our implementation
	supplied more severe limitation of this parameter (counter < 2^32) that 
	may only increase security */
	if(rndState_ptr->ReseedCounter >= CRYS_RND_MAX_RESEED_COUNTER)
		return CRYS_RND_RESEED_COUNTER_OVERFLOW_ERROR;

	if(outSizeBytes == 0)
		return CRYS_OK;

	/* Initializations */
	/*-----------------*/

	/* Set seed, key size and keyID */
	keySizeWords = rndState_ptr->KeySizeWords;
	seedSizeWords = keySizeWords + CRYS_AES_BLOCK_SIZE_IN_WORDS;
    
   	/*----------------------------------------------------------------- */
   	/*   [1] NIST 800-90, 10.2.1.5.2. CTR_DRBG Generate Process         */
   	/*----------------------------------------------------------------- */
    
	/* [1] 2:  If additional input valid, then call Derivation and Update functions */  
	if( rndState_ptr->AddInputSizeWords  > 0 ){

		/* move additional data two words right for DF operation */
		DX_PAL_MemCopy( (DxUint8_t*)&temp[CRYS_RND_TRNG_SRC_INNER_OFFSET_WORDS], 
			(DxUint8_t*)&rndState_ptr->AdditionalInput[0],	
			sizeof(DxUint32_t)*rndState_ptr->AddInputSizeWords);

		/* 2.1. Derivation Function call. If prediction resistance */
		error = DX_RND_DF(
				rndState_ptr,                                  /*in*/
				(DxUint8_t*)&temp[0], /*in - AdditionalInput*/
				sizeof(DxUint32_t)*rndState_ptr->AddInputSizeWords, /*in - AddInputSizeWords*/
				(DxUint8_t*)&rndState_ptr->AdditionalInput[0],      /*out - recalculated additional data*/
				sizeof(DxUint32_t)*seedSizeWords);                  /*in*/

		if(  error != CRYS_OK )
			goto End;

		/* 2.2. Call Update with recalculated additional (provided) data */
		error = DX_RND_Update(
				rndState_ptr,  /*in/out*/
				(DxUint8_t*)&rndState_ptr->AdditionalInput[0], /*in - provided data*/
				(DxUint8_t*)&rndState_ptr->Seed[0],            /*in/out - Key||V*/
				0);   /*in - skipSetUp*/ 
		
		if(  error != CRYS_OK )
			goto End;
	}
	else   /* 2.3. Set AdditionalInput = 000000...0  */
		DX_PAL_MemSetZero(rndState_ptr->AdditionalInput, sizeof(rndState_ptr->AdditionalInput));
	
		
	/*------------------------------------------------------------------------------*/ 
	/* [1] 4: Calculation of random: In loop {V = V+1; out = AES_ECB(Key,CTR=V)}    */
	/*        Note: This algorithm is equaled to out = AES_CTR(Key,dataIn=00000...) */    
	/*------------------------------------------------------------------------------*/ 
	
	/*   Initialization of AES engine with calculated Key on CTR mode */
	
	if (keySizeWords == 4)
		keySizeID = CRYS_AES_Key128BitSize;
	else 
		keySizeID = CRYS_AES_Key256BitSize;

	/* Increment counter V = V+1 */
	DX_RND_AddValToIv((DxUint8_t*)&rndState_ptr->Seed[keySizeWords], 1/*val*/);

	/* Init AES operation on CTR mode */
	error = CRYS_AES_Init(
			&AesUserContext,
			(DxUint8_t*)&rndState_ptr->Seed[keySizeWords],/*CTR=V*/
			(DxUint8_t*)&rndState_ptr->Seed[0],   /*calculated key*/
			keySizeID,           
			CRYS_AES_Encrypt,
			CRYS_AES_CTR_mode); 
	
	if(error != CRYS_OK)
		goto End;
	
	/* If mode is working mode and previous generated block is not valid,*  
	*  then generate one dummy block and save it as previous value        */
	if(!(rndState_ptr->StateFlag & CRYS_RND_KAT_mode) &&
	   !(rndState_ptr->StateFlag & CRYS_RND_PreviousIsValid)){ 
			       
		DX_PAL_MemSetZero(rndState_ptr->PreviousRandValue, sizeof(rndState_ptr->PreviousRandValue));			       
	
		error = CRYS_AES_Block(
				&AesUserContext,
				(DxUint8_t*)&rndState_ptr->PreviousRandValue[0],
				CRYS_AES_BLOCK_SIZE_IN_BYTES,
				(DxUint8_t*)&rndState_ptr->PreviousRandValue[0]); 
	
		if(error != CRYS_OK)
			goto End;
	
		/* set previous valid */
		rndState_ptr->StateFlag |= CRYS_RND_PreviousIsValid;
	}

	/* calculate remaining size in bytes  (must be > 0 for       *
	*  finish operation) */
	remainBytes = outSizeBytes & (CRYS_AES_BLOCK_SIZE_IN_BYTES-1);
	countBlocks = outSizeBytes >> 4; 
	if(remainBytes == 0) {
		remainBytes = CRYS_AES_BLOCK_SIZE_IN_BYTES; 
	}
	else
		countBlocks++;
	 
	
	/* generate full blocks of input data */
	if(outSizeBytes - remainBytes > 0){

		DX_PAL_MemSetZero(out_ptr, outSizeBytes-remainBytes);
	
		error = CRYS_AES_Block(
				&AesUserContext,
				out_ptr,
				outSizeBytes - remainBytes,
				out_ptr); 
	
		if(error != CRYS_OK)
			goto End;
	}

	/* save PreviousRandValue in temp buffer */
	DX_PAL_MemCopy(temp, rndState_ptr->PreviousRandValue, sizeof(rndState_ptr->PreviousRandValue));
	
	/* Generate full random block for last output data */
	DX_PAL_MemSetZero(rndState_ptr->PreviousRandValue, sizeof(rndState_ptr->PreviousRandValue));
	
	error = CRYS_AES_Finish(
			&AesUserContext,
			(DxUint8_t*)&rndState_ptr->PreviousRandValue[0],
			CRYS_AES_BLOCK_SIZE_IN_BYTES,
			(DxUint8_t*)&rndState_ptr->PreviousRandValue[0]); 
	
	if(error != CRYS_OK)
		goto End;
	
	/* output remain bytes */
	DX_PAL_MemCopy( out_ptr + outSizeBytes  - remainBytes, 
		(DxUint8_t*)&rndState_ptr->PreviousRandValue[0], remainBytes );
	
	
	/*  Perform CPRNGT - continuous test on each block  */
	/*---------------------------------------------------*/ 
	if(!(rndState_ptr->StateFlag & CRYS_RND_KAT_mode)) {
		
		error = DX_RND_CPRNGT(
				(DxUint8_t*)&temp[0], /*prev*/
				out_ptr,              /*buff_ptr*/
				(DxUint8_t*)&rndState_ptr->PreviousRandValue[0], /*last_ptr*/
				countBlocks);   /*in*/
		if(error)
			goto End;
	} 
	   
	/* calculate current value of the counter: V = V+countBlocks */
	DX_RND_AddValToIv((DxUint8_t*)&rndState_ptr->Seed[keySizeWords], countBlocks);

	/*------------------------------------------*/
	/* [1] 6:    Update Key,V in the State      */
	/*------------------------------------------*/


	error = DX_RND_Update(
		rndState_ptr, /*in/out*/
		(DxUint8_t*)&rndState_ptr->AdditionalInput[0],  /*in - saved additional input */
		(DxUint8_t*)&rndState_ptr->Seed[0], /*in/out - Key||V*/
		1);  /*skipSetUp*/

	if(error != CRYS_OK)
		goto End;

	/* [1] 6:    Increment Reseed counter       */
	/*------------------------------------------*/
	rndState_ptr->ReseedCounter++;
    
	/* Clean additional input  */
	if(rndState_ptr->AddInputSizeWords != 0){
		rndState_ptr->AddInputSizeWords = 0;
		DX_PAL_MemSetZero(rndState_ptr->AdditionalInput, sizeof(rndState_ptr->AdditionalInput));
	}
   	/* ................. end of function ........... */
   	/* --------------------------------------------- */
    
End:

   	return error;

} /* End of DX_RND_GenerateVector */



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
CEXPORT_C CRYSError_t CRYS_RND_Instantiation( CRYS_RND_WorkBuff_t  *workBuff_ptr/*in/out*/ )   
{

	/* error identifier definition */
	CRYSError_t Error = CRYS_OK;
  
	/* FUNCTION LOGIC */
   
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   

	RETURN_IF_RND_UNSUPPORTED(workBuff_ptr, Error, Error, Error, Error, Error, 
                                  Error, Error, Error, Error, Error, Error, Error, 
				  Error, Error); 
                              

	/* check parameters */
	if(workBuff_ptr == DX_NULL) 
		return CRYS_RND_WORK_BUFFER_PTR_INVALID_ERROR;

	/* call on Instantiation mode */
	Error = DX_RND_InstantiateOrReseed(
		&CRYS_RND_WorkingState, 
		DX_TRUE/*isInstantiate*/,
		DX_FALSE/*isContinued*/,
		workBuff_ptr);
	return Error;	                                     

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

	CRYSError_t Error = CRYS_OK;
 
	RETURN_IF_RND_UNSUPPORTED(Error, Error, Error, Error, Error, Error, 
                                  Error, Error, Error, Error, Error, Error, 
				  Error, Error, Error ); 
                              

	DX_PAL_MemSetZero(&CRYS_RND_WorkingState, sizeof(CRYS_RND_State_t));

	return Error;	                                     
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
 * 
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CEXPORT_C CRYSError_t CRYS_RND_Reseeding(CRYS_RND_WorkBuff_t  *workBuff_ptr/*in/out*/)
{

	/* FUNCTION DECLARATIONS */

	/* error identifier definition */
	CRYSError_t Error;
  
	/* FUNCTION LOGIC */

	/* ............... if not supported exit .............................. */
	RETURN_IF_RND_UNSUPPORTED(workBuff_ptr, Error, Error, Error,
                                  Error, Error, Error, Error, Error, 
				  Error, Error, Error, Error, Error, Error); 
                               
	/* check parameters */
	if(workBuff_ptr == DX_NULL) 
		return CRYS_RND_WORK_BUFFER_PTR_INVALID_ERROR;

	/* call on Reseeding mode */
	Error = DX_RND_InstantiateOrReseed(
			&CRYS_RND_WorkingState, /*in/out*/
			DX_FALSE/*isInstantiate*/,
			DX_FALSE/*isContinued*/,    
			workBuff_ptr );   /*in/out*/

	return Error;


}/* END OF CRYS_RND_Reseeding */

#endif /*_INTERNAL_CRYS_ONE_SEED*/


#ifndef _INTERNAL_CRYS_ONE_SEED
/******************************************************************************************/ 
/**
* @brief This function loads the AdditionaInput and its Size, given by the 
*       user, into the global CRYS_RND_WorkingState buffer; 
*
* @param[in] AdditonalInput_ptr - The pointer to Additional input buffer.
* @param[in] AdditonalInputSize - The size of Additional input in words - must be up to 12 words.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure a
*                        value MODULE_* as defined in crys_rnd_error.h
*/
CEXPORT_C CRYSError_t CRYS_RND_AddAdditionalInput(DxUint8_t *AdditonalInput_ptr,
						  DxUint16_t AdditonalInputSizeBytes)
{


	/* The return error identifiers */
	CRYSError_t Error;

	/* ............... if not supported exit .............................. */
	RETURN_IF_RND_UNSUPPORTED(AdditonalInput_ptr, AdditonalInputSizeBytes, Error, Error,
			          Error, Error, Error, Error, Error, Error, 
				  Error, Error, Error, Error, Error); 

	/* ............... local initializations ............................. */
	/* -------------------------------------------------------------------- */

	/* initializing the Error to O.K */
	Error = CRYS_OK;

	/* FUNCTION LOGIC */ 

	if((AdditonalInput_ptr == DX_NULL) &&
		(AdditonalInputSizeBytes != 0))
		return CRYS_RND_ADDITIONAL_INPUT_BUFFER_NULL;   

	/* check Additional Input size - must be up to 12 words*/
	if(AdditonalInputSizeBytes > sizeof(CRYS_RND_WorkingState.AdditionalInput))
		return CRYS_RND_ADDITIONAL_INPUT_SIZE_ERROR;

	DX_PAL_MemSetZero( CRYS_RND_WorkingState.AdditionalInput, 
			sizeof(CRYS_RND_WorkingState.AdditionalInput));	

	if(AdditonalInput_ptr != DX_NULL)
		/* Copy the data from user to the global buffer: AdditionalInput */
		DX_PAL_MemCopy( CRYS_RND_WorkingState.AdditionalInput,
				AdditonalInput_ptr,
				AdditonalInputSizeBytes );

	/* Set the AdditionalInput flag to indicate that data written to the buffer 
	and the size of the data */
	CRYS_RND_WorkingState.AddInputSizeWords = AdditonalInputSizeBytes / sizeof(DxUint32_t);

	/* ................. end of function ..................................... */
	/* ----------------------------------------------------------------------- */ 

	return Error;  
}

/**********************************************************************************************************/
/**
 * @brief The CRYS_RND_EnterKatMode function sets KAT mode bit into StateFlag
 *        of global CRYS_RND_WorkingState structure.
 * 
 *   The user must call this function before calling functions performing KAT tests.  
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

	/* FUNCTION DECLARATIONS */

	/* error identifier definition */
	CRYSError_t Error = CRYS_OK;
	DxUint8_t  *buf_ptr;

	/* ........     if not supported exit       ....... */
	RETURN_IF_RND_UNSUPPORTED(entrData_ptr, entrSize, nonce_ptr, nonceSize,
			          workBuff_ptr, buf_ptr, Error, Error, Error, Error, 
				  Error, Error, Error, Error, Error); 

	/* FUNCTION LOGIC */ 

	/* Set KAT mode */
	CRYS_RND_WorkingState.StateFlag |= CRYS_RND_KAT_DRBG_mode; 

	/* check Entropy Input size - must be up to 12 words*/
	if((entrData_ptr == DX_NULL) && (entrSize == 0))
		return CRYS_OK;

	else if((entrData_ptr == DX_NULL) || (entrSize == 0))
		return CRYS_RND_ILLEGAL_PARAMETER_ERROR;   

	if((nonce_ptr == DX_NULL) && (nonceSize != 0))
		return CRYS_RND_ILLEGAL_DATA_PTR_ERROR;   

	if(workBuff_ptr == DX_NULL)
		return CRYS_RND_WORK_BUFFER_PTR_INVALID_ERROR;   

	/* check entropy size */
	if(entrSize > sizeof(DxUint32_t)*CRYS_RND_ENTROPY_TEMP_BUFFER_MAX_SIZE_WORDS)
		return CRYS_RND_ILLEGAL_DATA_SIZE_ERROR;

	/* Copy concatenated entropy and nonce data with defined offset  */
	/*---------------------------------------------------------------*/ 
	/* set pointer to begin of RND entropy source */
	buf_ptr = (DxUint8_t*)workBuff_ptr + 
			sizeof(DxUint32_t)*(CRYS_RND_SRC_BUFF_OFFSET_WORDS + CRYS_RND_TRNG_SRC_INNER_OFFSET_WORDS); 

	DX_PAL_MemCopy(buf_ptr, entrData_ptr, entrSize);

	if((nonce_ptr != DX_NULL) && (nonceSize != 0)) {

		/* check nonce size */
		if((entrSize + nonceSize) > sizeof(DxUint32_t)*CRYS_RND_ENTROPY_TEMP_BUFFER_MAX_SIZE_WORDS)
			return CRYS_RND_ILLEGAL_DATA_SIZE_ERROR;

		DX_PAL_MemCopy(buf_ptr + entrSize, nonce_ptr, nonceSize);

		/* Calculate total source size */
		entrSize += nonceSize;
	}

	/* Set total size into workBuff on begin of RND source buffer, i.e.   * 
	*  two words backward   					      */
	*((DxUint32_t*)workBuff_ptr + CRYS_RND_SRC_BUFF_OFFSET_WORDS) = entrSize;

	return Error;

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
CEXPORT_C void CRYS_RND_DisableKatMode(void)
{
	/* FUNCTION LOGIC */
   
	CRYS_RND_WorkingState.StateFlag &= ~CRYS_RND_KAT_mode; 
    
	return;

}/* END OF CRYS_RND_DisableKatMode  */



/**********************************************************************************************************/
/** 
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
 * @rndSizeInBits[in]   - If maxVect_ptr is not given, then rndSizeInBits defining the exact size (in bits) 
 *                        of generated random vector. If maxVect is given, then it defines the 
 *	                  size (rounded up to bytes) of the maxVect_ptr buffer. 
 *                        
 * @maxVect_ptr[in]     - The pointer to vector defining a high limit
 *                        of random vector. 
 * @rndVect_ptr[in,out] - The output buffer for the random vector. 
 *	
 * @return CRYSError_t  - On success CRYS_OK is returned, on failure - a value,
 *      		  defined in crys_rnd_error.h.
 */
CEXPORT_C CRYSError_t CRYS_RND_GenerateVectorInRange(
					DxUint32_t  rndSizeInBits, 
					DxUint8_t  *maxVect_ptr, 
					DxUint8_t  *rndVect_ptr )
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

	/* to prevent compilation warnings */
	smin = 0; smin = smin; 
	totalRandSize = 0; totalRandSize = totalRandSize;
	roundedSize = 0; roundedSize = roundedSize;

	/* ............... if not supported exit .............................. */
	RETURN_IF_RND_UNSUPPORTED(rndSizeInBits, maxVect_ptr, rndVect_ptr, i,
				  k, shift, mask, flag0, flag1, rndSizeInBytes, 
				  checkingSizeBytes, maxVectSizeBits, maxVectSizeBytes, 
				  CompRes, Error); 


	/* Initializations */


	Error = CRYS_OK;
	maxVectSizeBytes = 0;

	/*  Check input parameters */
	if(rndVect_ptr == DX_NULL)
		return CRYS_RND_DATA_OUT_POINTER_INVALID_ERROR;

	/* given size of random vector in bytes */
	rndSizeInBytes = (rndSizeInBits + 7) / 8;

	if(rndSizeInBits == 0 || rndSizeInBytes > CRYS_RND_MAX_GEN_VECTOR_SIZE_BYTES)
		return CRYS_RND_VECTOR_SIZE_ERROR;

    
	/*----------------------------------------------------------------*/
	/* calculation of actual size of random (excluding leading zeros) */
	/*----------------------------------------------------------------*/
   
	/* the lower limit of the random vector value */
	smin = 1;

	if(maxVect_ptr != DX_NULL){
		/* calculate actual size of MaxVector in bits*/
		maxVectSizeBits = CRYS_COMMON_GetBytesCounterEffectiveSizeInBits(
			                                           maxVect_ptr, 
			                                           (DxUint16_t)rndSizeInBytes);
		maxVectSizeBytes = (maxVectSizeBits + 7) / 8;


		/* calculate count of extra 0-bytes in maxVector */
		extraBytes = rndSizeInBytes - maxVectSizeBytes;

	   /* intermediate checking size */
	   if(maxVectSizeBytes > CRYS_AES_BLOCK_SIZE_IN_BYTES)
		   checkingSizeBytes = CRYS_AES_BLOCK_SIZE_IN_BYTES;
	   else
		   /* intermediate checking size */
		   checkingSizeBytes = maxVectSizeBytes;
	}
	/* case that maxVect not provided and rndVect must have exact size = rndSizeInBits */
	else{
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

	while(k < 0xFFFF)
	{
		/* init checking flags */
		flag0 = 0;
		flag1 = 0;

		/* generate full size random vector */
		Error = CRYS_RND_GenerateVector((DxUint16_t)rndSizeInBytes, rndVect_ptr); 

		if(Error != CRYS_OK)    
			goto End;

		/* count of generated random blocks of size 16 bytes each one */
		totalRandSize += roundedSize;

		/* set non significant bytes to 0 */
		for(i = 0; i < extraBytes; i++){
			rndVect_ptr[rndSizeInBytes-1 - i] = 0;
		}

		/* mask the non significant high bits */
		rndVect_ptr[rndSizeInBytes - extraBytes - 1] &= mask;
   
		/* in case, that maxVect is provided, check 1 < rndVect < maxVect, 
		 else generate new random */
		if(maxVect_ptr != DX_NULL){
			/* step1 check high part of random */
			if(checkingSizeBytes == CRYS_AES_BLOCK_SIZE_IN_BYTES){
				while(1)
				{
					CompRes = CRYS_COMMON_CmpLsbUnsignedCounters( 
							rndVect_ptr + rndSizeInBytes - CRYS_AES_BLOCK_SIZE_IN_BYTES, 
							CRYS_AES_BLOCK_SIZE_IN_BYTES, 
							maxVect_ptr  + rndSizeInBytes - CRYS_AES_BLOCK_SIZE_IN_BYTES,
							CRYS_AES_BLOCK_SIZE_IN_BYTES);
				
					if(CompRes == CRYS_COMMON_CmpCounter2GraterThenCounter1){
						flag0 = 1;
						goto CheckMinimal;  /* random is found */
					}
					else if (CompRes == CRYS_COMMON_CmpCounter1AndCounter2AreIdentical)
						break; /* go to check full size */

					/* generate new 16 random high bytes */
					Error = CRYS_RND_GenerateVector(  
								CRYS_AES_BLOCK_SIZE_IN_BYTES,
								rndVect_ptr + rndSizeInBytes - CRYS_AES_BLOCK_SIZE_IN_BYTES); 
					if(Error != CRYS_OK)    
						goto End;

					/* set non significant bytes of random to 0 */
					for (i = 0; i < extraBytes; i++) {
						rndVect_ptr[rndSizeInBytes - 1 - i] = 0;
					}

					/* mask the non significant high bits */
					rndVect_ptr[rndSizeInBytes - extraBytes - 1] &= mask;
				}
			}
		   	
			/* check full size relating to max vector */
			CompRes = CRYS_COMMON_CmpLsbUnsignedCounters(rndVect_ptr, (DxUint16_t)rndSizeInBytes, 
			                                             maxVect_ptr, (DxUint16_t)maxVectSizeBytes);
		  
			if(CompRes == CRYS_COMMON_CmpCounter2GraterThenCounter1)
				flag0 = 1;

		CheckMinimal:
			/* check minimal value */ 
			CompRes = CRYS_COMMON_CmpLsbUnsignedCounters( rndVect_ptr, 
				(DxUint16_t)rndSizeInBytes, &smin, 1);
		   
			if(CompRes == CRYS_COMMON_CmpCounter1GraterThenCounter2) 
				flag1 = 1;

			if(flag0 && flag1)
				goto End;
		}

		else{
			/* in case, that maxVector is not provided, set MSBit of rndVect to 1 */
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
