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


  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  24 Oct. 2007
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This file contains functions for generation and checking of the 
   *         Diffie-Hellman (DLP) domain and public key parameters 
   *
   *  \version crys_dh_kg.c#1:csrc:5
   *  \author R.Levin
   */

/************* Include Files ****************/
#include "dx_vos_mem.h"
#include "crys_rnd.h"
#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif
#include "crys_common.h"
#include "crys_common_math.h"
#include "compiler.h"
#include "plat_print_dbg.h"
#include "crys_hash.h"
#include "crys_ccm.h"
#ifndef CRYS_NO_PKI_SUPPORT 
#include "llf_pki_rsa.h"
#endif
#include "crys_dh_error.h"
#include "crys_dh.h"
#include "crys_dh_kg.h"

#define RND_ROT32(x) ( (x) >> 16 | (x) << 16 )

/* inverse the bytes order in a word */
#define RND_REVERSE32(x)  ( ((RND_ROT32((x)) & 0xff00ff00UL) >> 8) | ((RND_ROT32((x)) & 0x00ff00ffUL) << 8) )  

/************************ Defines *******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/* run time LLF_PKI_mode  definition */

#ifdef DX_PKI_MODE_DMA_ONLY
#define  LLF_PKI_mode LLF_PKI_DMA_MODE_TYPE
#else
#ifdef DX_PKI_MODE_DIRECT_ONLY
#define  LLF_PKI_mode LLF_PKI_DIRECT_MODE_TYPE
#else
extern DxUint32_t LLF_PKI_mode;
#endif
#endif


/************************ Enums *********************************/

/************************ macros ********************************/

/** @brief This macro is required to remove compilers warnings if the HASH or PKI is not supported */

#if (defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) || defined(CRYS_NO_DH_SUPPORT))
#define RETURN_IF_DH_UNSUPPORTED( a , b , c , d , e , f , g , h , i , j , k , l , m, n , o ,p , q, a1 , b1 , c1 , d1 , e1 , f1 , g1 , h1 , i1 , j1, k1, l1, m1, n1 , o1 ,p1 , q1 ) \
  (a)=0;(b)=0;(c)=0;(d)=0;(e)=0;(f)=0;(g)=0;(h)=0;(i)=0;(j)=0;(k)=0;(l)=0;(m)=0;(n)=0;(o)=0;(p)=0;(q)=0; \
  (a1)=0;(b1)=0;(c1)=0;(d1)=0;(e1)=0;(f1)=0;(g1)=0;(h1)=0;(i1)=0;(j1)=0; \
  (a)=(a);(b)=(b);(c)=(c);(d)=(d);(e)=(e);(f)=(f);(g)=(g);(h)=(h);(i)=(i);(j)=(j);(k)=(k);(l)=(l); (m)=(m);(n)=(n);(o)=(o);(p)=(p);(q)=(q);\
  (a1)=(a1);(b1)=(b1);(c1)=(c1);(d1)=(d1);(e1)=(e1);(f1)=(f1);(g1)=(g1);(h1)=(h1);(i1)=(i1);(j1)=(j1);(k1)=(k1);(l1)=(l1); (m1)=(m1);(n1)=(n1);(o1)=(o1);(p1)=(p1);(q1)=(q1); \
  return CRYS_DH_IS_NOT_SUPPORTED
#else  
#define RETURN_IF_DH_UNSUPPORTED( a , b , c , d , e , f , g , h , i , j , k , l , m, n , o ,p , q,a1 , b1 , c1 , d1 , e1 , f1 , g1 , h1 , i1 , j1,  k1, l1, m1, n1 , o1 ,p1 , q1) 
#endif /* !CRYS_NO_HASH_SUPPORT && ! CRYS_NO_PKI_SUPPORT && ! CRYS_NO_DH_SUPPORT */

/*********************** Global data  ***************************/

/************* External functions prototypes  *****************************/ 

CRYSError_t DX_RND_GenerateWordsArrayInRange(
					DxUint32_t   rndSizeInBits, 
					DxUint32_t  *maxVect_ptr, 
					DxUint32_t  *rndVect_ptr,
					DxUint32_t  *tmp_ptr);

/******************************************************************************************/
/************************         Private Functions          ******************************/
/******************************************************************************************/

#if !(defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) || defined(CRYS_NO_DH_SUPPORT))


/********************************************************************************/
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
 *  
 * @return carry from last addition 
 */
static DxUint32_t DX_DH_KG_AddValueToMsbLsbBytesArray(DxUint8_t *arr_ptr, DxUint32_t val, DxUint32_t sizeBytes)
{
	DxInt32_t i;
	DxUint32_t *ptr = (DxUint32_t*)arr_ptr;
	DxUint32_t tmp, curr;

	for(i = sizeBytes/4 - 1; i >= 0; i--) {

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
			val = 0;
	}

	return val; /* carry */
}


/********************************************************************************/
/** 
 * @brief This function returns the effective size in bits of the MSB bytes array. 
 *
 *        Assumed, that MSB > 0 is stored in the most left most cell in the array.
 *
 * @param[in] arr_ptr -  The counter buffer.
 * @param[in] sizeInBytes -  The counter size in bytes. 
 *
 * @return result - The effective counters size in bits.
 */

 DxUint32_t DX_DH_KG_GetSizeInBitsOfMsbLsbBytesArray(DxUint8_t  *arr_ptr,
                                                     DxUint32_t  sizeInBytes)
{                                               
   /* FUNCTION LOCAL DECLERATIONS */

   /* loop variable */
   DxInt32_t i; 
   
   /* the effective size in bits */
   DxUint32_t sizeInBits = 8 * sizeInBytes;
   
   /* the effective MS byte */
   DxUint8_t msbVal = arr_ptr[0], mask = 0x80;

   /* FUNCTION LOGIC */
      
   /* adjusting the effective size in bits */
   for( i = 0; i < 8 ; i++ )
   {
     /* if the MS bit is set exit the loop */
     if(msbVal & mask)
	break;
       
     sizeInBits--;
     
     mask >>= 1;
     
   }
   
   return sizeInBits;    
   
}/* END OF  DX_DH_KG_GetSizeInBitsOfMsbLsbBytesArray */

  

/** @brief The function finds prime number Q for key generation according to X9.42-2001.
 *
 * @param[in]  QSizeBits          - The size of order of generator in bits. According to ANSI X9.43:
 *                                  m must be multiple of 32 bits and m >= 160. According to ANSI X9.30-1: 
 *                                  m = 160 bit. Because Q is prime factor of P-1, it must be < (P-1)/2.  
 *                                  In our implementation required, that orderQSize <= modPSizeBytes/2. 
 * @param[in]  seedSizeBits      - The  seed size in bits. Requirements: 
 *                                  seedSize >= orderQSize and seedSize <= modPSizeBytes ( the last is required 
 *                                  by our implementation). 
 * @param[in]  generateSeed       - The  flag defining whether the seed to be generated (1) or not (0),
 * @param[out] Q_ptr              - The pointer to the order Q of generator. The buffer must be aligned to 4 bytes.
 *                                  Note: The order Q is set as Words array, where LSWord is left most.
 * @param[out] S_ptr              - The random seed used for generation of primes. The buffer must be aligned to 4 bytes. 
 *      			    Note: The seed is set inthe buffer as BE bytes array.
 * @param[in]  TempBuff1_ptr      - The temp buffer of size not less than max modulus size, aligned to 4 bytes.
 * @param[in]  TempBuff2_ptr      - The temp buffer of size not less than max 
 *                                  modulus size, aligned to 4 bytes.
 * @param[in]  TempBuff2_ptr      - The large temp buffer (aligned to 4 bytes) of size: 
 *                                    - on HW platform not less than 8*CRYS_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 *                                    - on SW platform not less than 41*CRYS_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a predefined error code.
 *
 *
 */
static CRYSError_t DX_DH_X942_FindPrimeQ( 
                          DxUint32_t          QsizeBits,           /*in */
                          DxUint32_t          seedSizeBits,        /*in */
                          DxUint32_t          generateSeed,        /*in */
                          DxUint32_t         *Q_ptr,               /*out*/
                          DxUint8_t          *S_ptr,               /*out*/
                          DxUint32_t         *TempBuff1_ptr,       /*in */
                          DxUint32_t         *TempBuff2_ptr,       /*in - large buffer*/
                          DxUint32_t         *TempBuff3_ptr )      /*in*/
{
  
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* size of order in 160-bit blocks: M1 */
	DxUint32_t  M1;   
	
	/* primality flag (if prime, then isPrime = 1, else 0 ) */
	DxInt8_t  isPrime;
	
	/* flag of first hash calculating */
	DxUint8_t  isFirst = 1; 
	
	
	/* HASH input and result pointers */
	DxUint8_t  *hashDataIn1_ptr, *hashDataIn2_ptr; 
	CRYS_HASH_Result_t   *hashRes1_ptr, *hashRes2_ptr;
	
	/* current data pointer and sizes */
	DxUint8_t  *current_ptr;
	
	/* order size in bytes and in words */
	DxUint32_t  QsizeBytes, QsizeWords;
	
	/* exact seed size in bits and in words */
	DxUint32_t  seedSizeBytes, seedSizeWords, remainingSize;
	
	/* shift value (in bits) for adding counter to seed */
	DxUint32_t shift;
	DxUint8_t  mask, mask1;
	
	/* loop counters */
	DxUint32_t i, j;
	
	/* temp buffers pointers */
	DxUint32_t  *TempBuff4_ptr;
	
	/* FUNCTION  LOGIC */
	
	Error = CRYS_OK;
	
	
	
	/* Step 1. Check input parameters */
	/*------------------------------- */
	
	/* check pointers: modP, generator and tempBuff. Note: other pointers may be NULL  */
	if(Q_ptr == DX_NULL ||
	   S_ptr == DX_NULL ||
	   TempBuff1_ptr == DX_NULL ||
	   TempBuff2_ptr == DX_NULL){
		return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;
	}

	/* --------------------------------- */  
	/*  Step 2.  Initializations         */
	/* --------------------------------- */   
	
	/* order and seed sizes */
	QsizeBytes = (QsizeBits + 7) / 8;
	QsizeWords = (QsizeBits + 31) / 32;
	seedSizeWords = (seedSizeBits + 31) / 32;
	seedSizeBytes = (seedSizeBits + 7) / 8;
	
	/* temp buffers pointers */
	TempBuff4_ptr = TempBuff2_ptr + max(QsizeWords, seedSizeWords) + 2;
	
	/* order size M1 in 160-bit blocks (rounded up) */
	M1 = (QsizeBits + CRYS_DH_SEED_MIN_SIZE_IN_BITS - 1) / CRYS_DH_SEED_MIN_SIZE_IN_BITS;
	
	/* if QsizeBits >= CRYS_DH_SEED_MIN_SIZE_IN_BITS*256 = 40960 bits,    * 
	*  then return error. This checkin is for preventing KW warnings      */
	if(M1 >= 256) 
		return CRYS_DH_INVALID_ORDER_SIZE_ERROR;
	
	/* zeroing  Q buffer */
	DX_VOS_MemSetZero(Q_ptr, QsizeBytes);
	
	/* set HASH pointers to temp buffer */
	hashDataIn1_ptr = (DxUint8_t*)TempBuff1_ptr;
	hashDataIn2_ptr = (DxUint8_t*)TempBuff2_ptr;
	hashRes1_ptr = (CRYS_HASH_Result_t*)TempBuff4_ptr;      
	hashRes2_ptr = hashRes1_ptr + 1;  
	
	/*------------------------------- */   
	/* Step 3. Create random prime Q  */
	/*------------------------------- */ 
	
	/* check size and copy seed S into HASH input buffer */
	if(generateSeed != 1){
		if(seedSizeBits != DX_DH_KG_GetSizeInBitsOfMsbLsbBytesArray(S_ptr, seedSizeBytes))
			return CRYS_DH_INVALID_SEED_SIZE_ERROR;

		/* check that (seed + 0xff) is not less than 2^seedSizeBits of
		   seed, i.e. prevent seed addition overflow        */
		DX_VOS_FastMemCpy(hashDataIn1_ptr, S_ptr, seedSizeBytes);
		DX_VOS_FastMemCpy(hashDataIn2_ptr, S_ptr, seedSizeBytes);
		if(DX_DH_KG_AddValueToMsbLsbBytesArray(hashDataIn2_ptr, M1, seedSizeBytes) != 0)
			return CRYS_DH_PASSED_INVALID_SEED_ERROR;
	}
	
	/* shift value to bit position of MSbit of the seed  */
	shift = 8*seedSizeBytes - seedSizeBits;
	mask = 0xFF >> shift;
	mask1 = 0x80 >> shift;
	
	/* initialize isPrime, orderSizeInBlocks, and Q buffer */ 
	isPrime = DX_FALSE;
	
	/* Step 3.1. Try Q candidates     */
	/*--------------------------------*/
	while(isPrime != DX_TRUE) 
	{ 
		/* Step 3.1.1. Create random seed  S  */
		
		if(generateSeed == 1)
		{   	
			/* generation of random vector */
			Error = CRYS_RND_GenerateVector((DxUint16_t)seedSizeBytes, S_ptr);
			
			if(Error != CRYS_OK)      
				goto EndWithError;  
			
			/* Set the MS bit of S and provide exact size of seed in bits */
			S_ptr[0] = (S_ptr[0] & mask) | mask1;     
			
			/* copy seed into hashDataIn1 */
			DX_VOS_FastMemCpy(hashDataIn1_ptr, S_ptr, seedSizeBytes);

			/* copy seed in second buffer and add M1*/
			DX_VOS_FastMemCpy(hashDataIn2_ptr, hashDataIn1_ptr, seedSizeBytes);
			DX_DH_KG_AddValueToMsbLsbBytesArray(hashDataIn2_ptr, M1, seedSizeBytes);
		}
		else if(isFirst == 0)
			return  CRYS_DH_PASSED_INVALID_SEED_ERROR;

		
		/* set current pointer and size for copying HASH results into * 
		*  TempBuff3 as big endian bytes			      */
		current_ptr = &((DxUint8_t*)Q_ptr)[QsizeBytes - CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES]; 
		remainingSize = QsizeBytes; 
		
		/* Step 3.1.2. Create Q candidate:  For i=0 to M1 do:  
		Q = Q + (SHA1(S+i) XOR SHA1(S+M1+i))*(2^(160*i)) */   		     
		for(i = 0; i < M1; i++) {  
			if(i != 0) {  
				/* increment hashDataIn1 and hashDataIn2 by 1 * 
				*  starting from second cycle   	      */
				DX_DH_KG_AddValueToMsbLsbBytesArray(hashDataIn1_ptr, 1, seedSizeBytes);  
				DX_DH_KG_AddValueToMsbLsbBytesArray(hashDataIn2_ptr, 1, seedSizeBytes);
			}
			
			/* calculate first HASH result */
			Error = CRYS_HASH(
				CRYS_HASH_SHA1_mode,
				hashDataIn1_ptr,
				seedSizeBytes,
				*hashRes1_ptr );
			
			if( Error != CRYS_OK )      
				goto EndWithError;
			
			
			/* calculate  second HASH result */
			Error = CRYS_HASH( 
				CRYS_HASH_SHA1_mode,
				(DxUint8_t*)hashDataIn2_ptr,
				seedSizeBytes,
				*hashRes2_ptr);
			
			if( Error != CRYS_OK )      
				goto EndWithError;
			
			/* XOR HASH results */
			for(j = 0; j < CRYS_HASH_SHA1_DIGEST_SIZE_IN_WORDS; j++) {
				(*hashRes1_ptr)[j] ^= (*hashRes2_ptr)[j];
			}
			
			/* copying HASH results into Q buffer */
			if(remainingSize >= CRYS_DH_SEED_MIN_SIZE_IN_BYTES){
				DX_VOS_FastMemCpy(current_ptr, hashRes1_ptr, CRYS_DH_SEED_MIN_SIZE_IN_BYTES);
					remainingSize -=  CRYS_DH_SEED_MIN_SIZE_IN_BYTES;
					current_ptr -= CRYS_DH_SEED_MIN_SIZE_IN_BYTES;
			} 
			else{   /* copy remaining low bytes to hashRes1_ptr */
				DX_VOS_FastMemCpy((DxUint8_t*)Q_ptr, 
					(DxUint8_t*)hashRes1_ptr + CRYS_DH_SEED_MIN_SIZE_IN_BYTES - remainingSize, 
					remainingSize);
			}
			
			/* set flag */
			isFirst = 0; 
			
		} /* end of for() loop */
		
		/* set the High and Low bits of Q equal to 1 */
		((DxUint8_t*)Q_ptr)[0] |= 0x80;           /* MS bit - big endian */
		((DxUint8_t*)Q_ptr)[QsizeBytes-1] |= 0x01;  /* LS bit - big endian */   
		
		
		/* Step 3.2. Perform primality tests on Q: 8 Miller-Rabin and 1 Lucas tests (X9.42-2001) */  
		/*---------------------------------------------------------------------------------------*/
		
		/* convert Q to words */
		Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(Q_ptr, QsizeBytes, (DxUint8_t*)Q_ptr, QsizeBytes);
		if(Error)
			return Error;

		Error = LLF_PKI_RSA_primeTestCall(
			Q_ptr, 
			QsizeBytes/4, 
			8 /*rabinTestsCount*/,
			&isPrime,
			TempBuff3_ptr,
			CRYS_DH_PRIME_TEST_MODE);
		
		if( Error != CRYS_OK )      
			goto EndWithError;
	
	} /* END of while() loop */
	
	
	/* End of function */
	
	return Error;
	
EndWithError:
	
	DX_VOS_MemSetZero((DxUint8_t*)Q_ptr, QsizeBytes);
	DX_VOS_MemSetZero((DxUint8_t*)S_ptr, seedSizeBytes);
	
	return Error;
	
} /* End of DX_DH_X942_FindPrimeQ */

                          
/******************************************************************************************/
/**
 * @brief The function finds prime modulus P for key generation according to X9.42-2001.
 *
 * 
 * @param[in]  modPSizeBits       - The  modulus (prime) P size in bits equal 256*n, where n >= 4.  
 * @param[in]  QSizeBbytes        - The size of order of generator in bytes. Must be m >= 20 bytes and
 *                                  multiple of 4 bytes. According to ANSI X9.30-1: size = 20.
 * @param[in]  orderQSizeBits     - The size of order of generator in bits. Must be m >= 160 and
 *                                  multiple of 32 bits. According to ANSI X9.30-1: m = 160.
 * @param[in]  seedSizeBits       - The  seed size in bits (the size must be:  seedSizeBits >= 160,
 *                                  seedSizeBits <= modPSizeBits - 1 (the last required by implementation).
 * @param[out] P_ptr              - The prime modulus P of structure P = j*Q + 1, where Q is prime
 *                                  and j is an integer.The buffer must be aligned to 4 bytes.
 * @param[out] Q_ptr              - The pointer to the order Q of generator. The buffer must be aligned to 4 bytes.
 * @param[out] S_ptr              - The random seed used for generation of primes. The buffer must be aligned to 4 bytes.
 * @param[out] pgenCounter_ptr    - The pointer to counter of tries to generate the primes. 
 * @param[in]  TempBuff1_ptr      - The temp buffer of size not less than max modulus size, aligned to 4 bytes.
 * @param[in]  TempBuff2_ptr      - The large temp buffer of size: 
 *                                - on HW platform not less than 8*CRYS_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 *                                - on SW platform not less than 41*CRYS_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 * @param[in]  TempBuff3_ptr      - The temp buffer of size: 2*CRYS_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a predefined error code.
 *
 *
 */
static CRYSError_t DX_DH_X942_FindPrimeP( 
                          DxUint32_t          modPsizeBits,         /*in */
                          DxUint32_t          orderQsizeBits,       /*in */
                          DxUint32_t          seedSizeBits,         /*in */
                          DxUint32_t          *P_ptr,               /*out*/
                          DxUint32_t          *Q_ptr,               /*out*/
                          DxUint8_t           *S_ptr,               /*out*/
                          DxUint32_t          *pgenCounter_ptr,     /*out*/
                          DxUint32_t          *TempBuff1_ptr,       /*in */
                          DxUint32_t          *TempBuff2_ptr,       /*in - large buffer*/
                          DxUint32_t          *TempBuff_ptr )       /*in*/
{
  
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* mod size in bytes and in words */
	DxUint32_t  modSizeBytes, modSizeWords; 
	
	/* seed size in bytes and words */
	DxUint32_t seedSizeBytes;
	
	/* mod size in 160 bits blocks (rounded up) */
	DxUint32_t  L1;  
	
	/* order sizes: M1 - in 160-bit blocks (rounded up) */
	DxUint32_t  orderSizeBytes, orderSizeWords, M1; 
	
	/* flag of first hash calculating */
	DxUint8_t  isFirst = 1; 
	
	/* primality flag (if prime, then isPrime = 1, else 0 ) */
	DxUint8_t  isPrime;
	
	/* HASH input and result pointers */
	DxUint8_t  *hashDataIn_ptr; 
	CRYS_HASH_Result_t   *hashRes_ptr;
	
	/* current data pointer and size */
	DxUint8_t  *current_ptr;
	DxInt32_t  remainingSize;
	
	CRYS_COMMON_CmpCounter_t  cmpRes;
	
	/* loop counter and carry */
	DxUint32_t i, carry;
	
	/* temp buffers pointers */
	DxUint32_t  *TempBuff3_ptr, *TempBuff4_ptr, *TempBuff5_ptr;
	
	/* FUNCTION  LOGIC */
	
	Error = CRYS_OK;
	
	
	/* --------------------------------- */
	/* Step 1. Check input parameters    */
	/*---------------------------------- */
	
	/* check pointers: modP, generator and tempBuff. Note: other pointers may be NULL  */
	if(P_ptr == DX_NULL ||
	   Q_ptr == DX_NULL ||
	   S_ptr == DX_NULL ||
	   pgenCounter_ptr == DX_NULL ||
	   TempBuff1_ptr == DX_NULL   ||
	   TempBuff2_ptr == DX_NULL) {
		return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;
	}
	
	/* --------------------------------- */ 
	/*  Step 2.  Initializations         */
	/* --------------------------------- */     
	
	/* mod sizes in bytes */
	modSizeBytes = (modPsizeBits + 7) / 8;
	modSizeWords = (modPsizeBits + 31) / 32;
	/* mod size in 160 bit blocks */
	L1 = (modPsizeBits + CRYS_DH_SEED_MIN_SIZE_IN_BITS - 1) / CRYS_DH_SEED_MIN_SIZE_IN_BITS; 
	
	/* order size: M1 - in 160-bit blocks (rounded up) */
	M1 = (orderQsizeBits + CRYS_DH_SEED_MIN_SIZE_IN_BITS - 1) / CRYS_DH_SEED_MIN_SIZE_IN_BITS;
	orderSizeBytes = (orderQsizeBits + 7) / 8;
	orderSizeWords = (orderQsizeBits + 31) / 32;
	
	/* seedSize in bytes */
	seedSizeBytes = (seedSizeBits + 7) / 8;
	
	/* zeroing of P  */
	DX_VOS_MemSetZero(P_ptr, modSizeBytes + 2);
	
	/* temp buffers pointers */
	TempBuff3_ptr = TempBuff2_ptr + modSizeWords + 2;
	TempBuff4_ptr = TempBuff3_ptr + 2*modSizeWords + 2;
	TempBuff5_ptr = TempBuff4_ptr + 2*modSizeWords + 2;
	
	/*------------------------------------------------------ */   
	/* Step 3.   Create random prime P = (Q*J + 1)           */
	/*------------------------------------------------------ */ 
	
	/* set pgenCounter 0 */
	*pgenCounter_ptr = 0;
	
	/* set HASH pointers to temp buffer */
	hashDataIn_ptr = (DxUint8_t*)TempBuff1_ptr;
	hashRes_ptr = (CRYS_HASH_Result_t*)TempBuff4_ptr;   /* used as temp buffer */
	
	/* Calculating R = seed + 2*M1 , where R is set in hashDataIn:
	  copy the seed into hashDataIn_ptr (big endian);
	  set other bytes to 0; add M1 */
	DX_VOS_FastMemCpy(hashDataIn_ptr, S_ptr, seedSizeBytes);
	DX_DH_KG_AddValueToMsbLsbBytesArray(hashDataIn_ptr, 2*M1, seedSizeBytes);
	
	/* initialize isPrime, orderSizeInBlocks, and Q buffer */ 
	isPrime = DX_FALSE;
	
	/* Step 3.1. Main loop - try P candidates */
	/*----------------------------------------*/
	while( isPrime != DX_TRUE ) 
	{  
		/* Step 3.1. Create P candidate: 
		For i=0 to L1 do:  P = P + SHA1(R+i) *(2^(160*i)) */   
		
		/* set current pointer and size for copying HASH results into *
		*  mod P as big endian bytes 		      */
		current_ptr = &((DxUint8_t*)P_ptr)[modSizeBytes - CRYS_DH_SEED_MIN_SIZE_IN_BYTES]; 
		remainingSize = modSizeBytes; 
		
		for(i = 0; i < L1; i++) {   
			    	    
			/* Adding 1 to hashDataIn excluding the first hashing operation */
			if(isFirst != 1)
				DX_DH_KG_AddValueToMsbLsbBytesArray(hashDataIn_ptr, 1, seedSizeBytes);
			
			/* set 0 to isFirst */
			isFirst = 0;
			
			/* calculate HASH result */
			Error = CRYS_HASH( 
					CRYS_HASH_SHA1_mode,
					hashDataIn_ptr,
					seedSizeBytes,
					*hashRes_ptr );
			
			if(Error != CRYS_OK)      
				goto EndWithError;
			
			/* set size for copying HASH result into P buffer */
			if(remainingSize >= CRYS_DH_SEED_MIN_SIZE_IN_BYTES){
				DX_VOS_FastMemCpy(current_ptr, hashRes_ptr, CRYS_DH_SEED_MIN_SIZE_IN_BYTES);
				remainingSize -=  CRYS_DH_SEED_MIN_SIZE_IN_BYTES;
				current_ptr -= CRYS_DH_SEED_MIN_SIZE_IN_BYTES;
			} 
			else{
				DX_VOS_FastMemCpy((DxUint8_t*)P_ptr, 
				(DxUint8_t*)hashRes_ptr + CRYS_DH_SEED_MIN_SIZE_IN_BYTES - remainingSize, 
				remainingSize);
			}
			
		} /* end of j - loop */
		
		/*-----------------------------------------------------------------------*/ 
		
		/* convert P to LSW array */
		Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(P_ptr, modSizeWords*4, (DxUint8_t*)P_ptr, modSizeBytes);
		if(Error)
			return Error;
		
		/*----------------------------------------*/ 
		/* Step 3.2. Set P = P - (P mod 2*Q) + 1  */
		/* Note: Now all operations on LSW arrays */
		/*----------------------------------------*/ 
		
		/* set the High and Low bits of Q equal to 1 */
		P_ptr[modSizeWords-1] |= 0x80000000; /* MS bit */           
		
		/* set TempBuff3 = 2*Q. Note: Result size is large by 1 byte (and word), than Q size */
		carry = CRYS_COMMON_Add2vectors(Q_ptr, Q_ptr, orderSizeWords, TempBuff3_ptr);
		
		/* if carry occurs, set next word of TempBuff3 to 1, else to 0 */
		if( carry != 0 )
			TempBuff3_ptr[orderSizeWords] = 1;
		else
			TempBuff3_ptr[orderSizeWords] = 0;
		
		/* calculate TempBuff4 = P mod 2*Q */       
		DX_VOS_MemSetZero((DxUint8_t*)TempBuff4_ptr, modSizeBytes);
		
		LLF_PKI_RSA_Call_Div( 
				P_ptr,         		    /*numerator P*/
				modSizeWords,               /*P_size in words*/
				TempBuff3_ptr,              /*divider */   		                    
				orderSizeWords + 1,         /*divider_size in words*/ 
				TempBuff4_ptr,              /*ModRes_ptr*/
				TempBuff2_ptr,              /*DivRes_ptr*/
				TempBuff5_ptr               /*TempBuff_ptr - 2*N_size */ );
		
		/* subtract: P = P - TempBuff4 */
		CRYS_COMMON_SubtractUintArrays(P_ptr, TempBuff4_ptr, modSizeWords, P_ptr);
		
		/* add 1 to P */
		CRYS_COMMON_IncLsbUnsignedCounter(P_ptr, 1, (DxUint8_t)modSizeWords);
		
		/* check: if P > 2^(L-1), then perform step 3.3. */
		/*-----------------------------------------------*/
		
		/*  set TempBuff5 = 2^(L-1): Note: L = modPsizeBits is        * 
		*   multiple of 32 bits 				      */
		DX_VOS_MemSetZero((DxUint8_t*)TempBuff4_ptr, modSizeBytes);
		TempBuff4_ptr[modSizeWords - 1] = 0x80000000;	
		
		/* compare */
		cmpRes = CRYS_COMMON_CmpLsWordsUnsignedCounters(
				P_ptr, (DxUint16_t)modSizeWords, TempBuff4_ptr, (DxUint16_t)modSizeWords);
		
		
		/* Step 3.3. If P is not diverted, then perform primality tests on P: 
		8 Rabin-Miller and 1 Lucas tests (X9.42-2001)              */
		/*----------------------------------------------------------------------*/ 
		
		if(cmpRes == CRYS_COMMON_CmpCounter1GraterThenCounter2){

			Error = LLF_PKI_RSA_primeTestCall(
					P_ptr, 
					modSizeWords, 
					8 /*rabinTestsCount*/,
					(DxInt8_t*)&isPrime,
					TempBuff_ptr, /* size as defined in function header */
					CRYS_DH_PRIME_TEST_MODE);
			
			if( Error != CRYS_OK )
				goto EndWithError;
		}
		
		/* if pgenCounter >= 4096*N then return "generation is fail" */
		if(*pgenCounter_ptr >= 4096*(modPsizeBits + 1024 - 1)/1024){
			Error = CRYS_DH_PRIME_P_GENERATION_FAILURE_ERROR;
			goto EndWithError;
		}
		
		/* update pgenCounter_ptr */
		*pgenCounter_ptr += 1; 
		
	} /* END of while(isPrime != DX_TRUE)*/
	
	/* correction of pgenCounter */
	*pgenCounter_ptr -= 1; 
	
	/* End of function */
	return Error;
	
EndWithError:
	
	DX_VOS_MemSetZero(P_ptr, modSizeBytes);	
	
	return Error;
    
}/* End of DX_DH_X942_FindPrimeP */



                          
/******************************************************************************************/
/**
 * @brief The function creates generator of GF(P) subgroup for key generation according to X9.42-2001.
 *
 *
 * @param[out] P_ptr              - The prime modulus P of structure P = j*Q + 1, where Q is prime
 *                                  and j is an integer.The buffer must be aligned to 4 bytes.
 * @param[in]  modPSizeBits       - The  modulus (prime) P size in bytes must be multiple of 4 bytes.  
 * @param[out] Q_ptr              - The pointer to the order Q of generator. The buffer must be aligned to 4 bytes.
 * @param[in]  orderSizeBits      - The size of order of generator in bytes. Must be multiple of 4 bytes.
 * @param[out] G_ptr              - The generator of GF(P) subgroup. The buffer must be aligned to 4 bytes.
 *                                  size of buffer not less than modPSize in bytes. 
 * @param[in]  tempBuff1_ptr      - The temp buffer of size not less than DH max modulus size, aligned to 4 bytes.
 * @param[in]  expTempBuff_ptr    - The temp buffer of defined structure.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a predefined error code.
 *
 *
 */
static CRYSError_t DX_DH_X942_CreateGenerator(                           
                          DxUint32_t           *P_ptr,               /*in */
                          DxUint32_t           modSizeBits,          /*in */
                          DxUint32_t           *Q_ptr,               /*in */
                          DxUint32_t           orderSizeBits,        /*in */
                          DxUint32_t           *G_ptr,               /*out*/
                          DxUint32_t           *tempBuff1_ptr,       /*in */
                          CRYS_DH_ExpTemp_t    *expTempBuff_ptr )    /*in */
{
  
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* modulus and order sizes in words */
	DxUint32_t modSizeBytes, modSizeWords, orderSizeBytes, orderSizeWords;
	
	DxUint32_t J_effectiveSizeBits;
	
	/* compare flag */
	CRYS_COMMON_CmpCounter_t  compFlag;
	
	
	/* INITIALIZATIONS */
	
	Error = CRYS_OK; 
	modSizeBytes = modSizeBits  / 8; 
	modSizeWords = modSizeBits  / 32;
	orderSizeWords = (orderSizeBits + 31) / 32;
	orderSizeBytes = (orderSizeBits + 7) / 8;
	
	
	/* FUNCTION  LOGIC */
	
	/*-------------------------------------*/
	/* Step 1. Calculate J = (P - 1)/Q     */
	/*-------------------------------------*/
	
	/*  copy modulus into TempBuff1  */
	DX_VOS_FastMemCpy((DxUint8_t*)expTempBuff_ptr->PubKey.n, (DxUint8_t*)P_ptr, modSizeBytes);
	DX_VOS_MemSetZero((DxUint8_t*)expTempBuff_ptr->PubKey.n + modSizeBytes, CRYS_DH_MAX_MOD_SIZE_IN_BYTES - modSizeBytes);
	/* copy order Q into aligned buffer */
	DX_VOS_FastMemCpy((DxUint8_t*)expTempBuff_ptr->TempBuff, (DxUint8_t*)Q_ptr, orderSizeBytes);
	DX_VOS_MemSetZero((DxUint8_t*)expTempBuff_ptr->TempBuff + orderSizeBytes, CRYS_DH_MAX_MOD_SIZE_IN_BYTES - orderSizeBytes);
	
	/* subtract: P - 1 */ 
	CRYS_COMMON_DecrLsbUnsignedCounter(expTempBuff_ptr->PubKey.n, 1, modSizeWords);
	
	/* divide */ 
	LLF_PKI_RSA_Call_Div( 
			expTempBuff_ptr->PubKey.n,            /*numerator B*/
			modSizeWords,                         /*B_size in words*/
			expTempBuff_ptr->TempBuff,            /*Q - divider*/   		                    
			(orderSizeBytes+3)/4,                 /*Q_size in words*/ 
			expTempBuff_ptr->PubKey.e,            /*ModRes_ptr*/
			tempBuff1_ptr,                        /*DivRes_ptr*/
			expTempBuff_ptr->PrimeData.DataIn);  /*TempBuff_ptr: len=2*modSize (using also PrimeData_ptr->DataOut buff) */ 
	
	/* calculate actual size of J in bits */
	J_effectiveSizeBits = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits( 
								tempBuff1_ptr, modSizeWords);  
	
	/*---------------------------------------------------------------*/
	/* Step 2. Generate random G : 1 < G < (P-1)  and                */
	/*         set it into DataIn buffer, other bytes of buffer = 0  */
	/*---------------------------------------------------------------*/
	/* cleaning of temp buffer */
	DX_VOS_MemSetZero((DxUint8_t*)&expTempBuff_ptr->PrimeData, sizeof(CRYS_DHPrimeData_t));
	
	/* generating rnd vector */
	Error = DX_RND_GenerateWordsArrayInRange(
				modSizeBits, expTempBuff_ptr->PubKey.n /*P-1*/, 
				expTempBuff_ptr->PrimeData.DataIn /*RND*/, 
				expTempBuff_ptr->PrimeData.DataOut/*temp*/);
	if(Error != CRYS_OK)
		goto End; 
	
	/*----------------------------------------------------*/
	/* Step 3. Initialization of PubKey and PrivData      */
	/*         structures for exponentiation              */
	/*----------------------------------------------------*/ 
	
	/* cleaning of temp buffer */
	DX_VOS_MemSetZero((DxUint8_t*)&expTempBuff_ptr->PubKey, sizeof(expTempBuff_ptr->PubKey));
	
	/* set modulus in DH_PubKey structure for exponentiation G^J mod P */
	DX_VOS_FastMemCpy((DxUint8_t*)expTempBuff_ptr->PubKey.n, (DxUint8_t*)P_ptr, modSizeBytes);
	expTempBuff_ptr->PubKey.nSizeInBits = modSizeBits;
	/* set exponent J and its size */ 
	DX_VOS_FastMemCpy( (DxUint8_t*)expTempBuff_ptr->PubKey.e, (DxUint8_t*)tempBuff1_ptr, 4*((J_effectiveSizeBits+31)/32) );
	expTempBuff_ptr->PubKey.eSizeInBits = J_effectiveSizeBits;
	
	/*  initialize the H value in LLF of PubKey for exponentiation  */
	Error = LLF_PKI_RSA_InitPubKeyDb(&expTempBuff_ptr->PubKey);
	
	if( Error != CRYS_OK )
		goto End; 
	
	
	/*-----------------------------------------------------------*/
	/* Step 4. Calculate G = G ^ J mod P , if G == 1, change     */
	/*         G (DataIn) and repeat exponentiation              */
	/*-----------------------------------------------------------*/
	
	compFlag = CRYS_COMMON_CmpCounter1AndCounter2AreIdentical;  /* 0 - means G == 1 */
	
	while(compFlag == 0)
	{
		/* exponentiation DataOut = DataIn ^ Exp mod P */   
		Error = LLF_PKI_RSA_ExecPubKeyExp(&expTempBuff_ptr->PubKey, &expTempBuff_ptr->PrimeData);
				 
		if(Error != CRYS_OK)
			return Error;
		
		/* set 1 to tempBuff1_ptr */
		DX_VOS_MemSetZero((DxUint8_t*)tempBuff1_ptr, modSizeBytes);
		tempBuff1_ptr[0] = 1;
		
		/* compare DataOut to 1: */
		compFlag = CRYS_COMMON_CmpLsWordsUnsignedCounters(
				expTempBuff_ptr->PrimeData.DataOut, modSizeWords, tempBuff1_ptr, modSizeWords); 
		
		/* if G == 1 change DataIn (by adding 1) for trying next G value */
		if(compFlag == 0) {
			CRYS_COMMON_IncLsbUnsignedCounter(
				expTempBuff_ptr->PrimeData.DataIn, 1, (DxUint8_t)modSizeWords);
		}
	}

	/* copy generator into output */
	DX_VOS_FastMemCpy((DxUint8_t*)G_ptr, (DxUint8_t*)expTempBuff_ptr->PrimeData.DataOut, modSizeBytes);

/* End of function */	
End:	
	return Error;
    
}/* End of DX_DH_X942_CreateGenerator */


/******************************************************************************************/
/**
 * @brief The function generates a DH (DLP) domain parameters in GF(P) (see X9.42-2001)
 *
 *   The function parameters are the same as in CRYS_DH_CreateDomainParams() function (see below) 
 *   besides one difference: this function not checks input parameters, because it is also used locally  
 *   in some other functions with input pointers = NULL.
 * 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure - a predefined error code.
 *
 */
static CRYSError_t DX_DH_CreateDomainParams( 
				DxUint32_t          modPsizeBits,             /*in */
				DxUint32_t          orderQsizeBits,           /*in */
				DxUint32_t          seedSizeBits,             /*in */
				DxUint8_t          *modP_ptr,                 /*out*/  
				DxUint8_t          *orderQ_ptr,               /*out*/
				DxUint8_t          *generatorG_ptr,           /*out*/
				DxUint32_t         *generGsizeBytes_ptr,      /*in/out*/
				DxUint8_t          *factorJ_ptr,              /*out*/
				DxUint32_t         *JsizeBytes_ptr,           /*in/out*/ 
				DxUint8_t          *seedS_ptr,                /*in/out*/
				DxInt8_t            generateSeed,             /*in*/ 
				DxUint32_t         *pgenCounter_ptr,          /*out*/
				CRYS_DHKGData_t    *DHKGbuff_ptr              /*in */)
{
  
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* pointers to temp buffers for candidates to order Q, modulus P, seed S, generator G */
	DxUint32_t  *Q_ptr, *P_ptr, *G_ptr, *J_ptr;
	DxUint8_t  *S_ptr;
	
	/* tries counter */
	DxUint32_t  pgenCounter;
	
	DxUint32_t  modSizeBytes, generatorSizeBits;
	
	/* temp buffer pointers */
	DxUint32_t  *TempBuff1_ptr, *TempBuff2_ptr;
	
	
	/* FUNCTION  LOGIC */
	
	Error = CRYS_OK;
	
	
	/* --------------------------------- */
	/*  Step 2.  Initializations         */
	/* --------------------------------- */ 
	
	/* clean DHKGbuff_ptr */
	DX_VOS_MemSetZero(DHKGbuff_ptr, sizeof(CRYS_DHKGData_t));
	
	/* set Q, S and G- pointers on DHKGbuff_ptr->PrimData temp buffers */
	Q_ptr = DHKGbuff_ptr->TempBuff2;
	P_ptr = DHKGbuff_ptr->TempBuff3;
	G_ptr = DHKGbuff_ptr->TempBuff4;
	J_ptr = DHKGbuff_ptr->TempBuff5;
	S_ptr = (DxUint8_t*)J_ptr;  
	
	/* set 32-bit temp pointers on KGData and PrimData temp buffers */
	TempBuff1_ptr = DHKGbuff_ptr->TempBuff1;
	TempBuff2_ptr = (DxUint32_t*)&(DHKGbuff_ptr->ExpTemps);  
	
	if( generateSeed == 0 )    
		DX_VOS_FastMemCpy((DxUint8_t*)S_ptr, seedS_ptr, (seedSizeBits+7)/8);
	
	modSizeBytes = (modPsizeBits + 7)/ 8; 
	
	
	/*------------------------------------------------------------------- */   
	/* Step 1. Find random prime Q and its Seed S according to ANSI X9.42 */
	/*------------------------------------------------------------------- */ 
	
	Error = DX_DH_X942_FindPrimeQ(    
				orderQsizeBits,            /*in */
				seedSizeBits,              /*in */
				generateSeed,              /*in */
				Q_ptr,                     /*out*/
				S_ptr,                     /*in/out*/
				TempBuff1_ptr,             /*in */
				TempBuff2_ptr,             /*in */
				DHKGbuff_ptr->TempBuff6);  /*in */
	if( Error != CRYS_OK )
		goto EndWithError; 
	
	
	/*------------------------------------------------------ */   
	/* Step 2.   Create random prime P = (Q*J + 1)           */
	/*------------------------------------------------------ */ 
	
	Error = DX_DH_X942_FindPrimeP( 
				modPsizeBits,        /*in */
				orderQsizeBits,      /*in */
				seedSizeBits,        /*in */
				P_ptr,               /*out*/
				Q_ptr,               /*out*/
				S_ptr,               /*in */
				&pgenCounter,        /*out*/
				TempBuff1_ptr,       /*in */
				TempBuff2_ptr,       /*in */
				DHKGbuff_ptr->TempBuff6); /*in */
	
	if(Error != CRYS_OK)
		goto EndWithError; 	  
	
	
	/*------------------------------------------------------ */   
	/* Step 3.   Create generator of GF(P) subgroup          */
	/*------------------------------------------------------ */ 
	if( generatorG_ptr != DX_NULL ) {
	
		Error = DX_DH_X942_CreateGenerator(                           
				P_ptr,                                       /*in */
				modPsizeBits,                                /*in */
				Q_ptr,                                       /*in */
				orderQsizeBits,                              /*in */
				G_ptr,                                       /*out*/
				TempBuff1_ptr,                               /*in */
				(CRYS_DH_ExpTemp_t*)&DHKGbuff_ptr->ExpTemps  /*in */);    	
		
		if( Error != CRYS_OK )
			goto EndWithError; 
		
		/* calculate size of generator and output it in big endianness */
		generatorSizeBits = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(G_ptr, (DxUint16_t)modSizeBytes/4);
		*generGsizeBytes_ptr = (generatorSizeBits + 7)/ 8;

		CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes( 
				generatorG_ptr, *generGsizeBytes_ptr, G_ptr, *generGsizeBytes_ptr);
	}
	
	/* output of result parameters (in big endianness) */
	CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(modP_ptr, modSizeBytes, P_ptr, modSizeBytes);
	
	if(orderQ_ptr != DX_NULL)
		CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes( 
					orderQ_ptr, orderQsizeBits/8, Q_ptr, (orderQsizeBits+7)/8);
	
	/* copy generated seed into output */
	if(generateSeed == 1)
		DX_VOS_FastMemCpy(seedS_ptr, (DxUint8_t*)S_ptr, (seedSizeBits + 7)/8);
	
	/* if factorJ_ptr != DX_NULL, then calculate this factor and its size. J = (P-1) / Q */
	if(factorJ_ptr != DX_NULL) {
		LLF_PKI_RSA_Call_Div( 
				P_ptr,                 /*numerator B*/
				modPsizeBits / 32,     /*B_size in words*/
				Q_ptr,                 /*divider N */   		                    
				orderQsizeBits / 32,   /*N_size in words*/ 
				TempBuff1_ptr,         /*ModRes_ptr*/
				J_ptr,                 /*DivRes_ptr*/
				TempBuff2_ptr);        /*TempBuff_ptr (size >= 2*N_Size)*/ 
		
		/* calculate size of J in bits */	                    
		*JsizeBytes_ptr = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(J_ptr, (DxUint16_t)modSizeBytes/4);  
		
		/* calculate size of J in bytes */	                    
		*JsizeBytes_ptr = (*JsizeBytes_ptr + 7) / 8;                                              
		
		/* convert result to MSB bytes and output into factorJ_ptr buffer */
		CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes( 
			factorJ_ptr, *JsizeBytes_ptr, 
			J_ptr, *JsizeBytes_ptr);
	}
	
	/*  if pgenCounter_ptr != DX_NULL put out pgenCounter */
	if(pgenCounter_ptr != DX_NULL)
		*pgenCounter_ptr = pgenCounter;
	
	goto End;
	
	/* End of function */
	
EndWithError:
	
	/* cleaning output buffers used also in internal computations */
	DX_VOS_MemSetZero(modP_ptr, (modPsizeBits+7)/8);   
	DX_VOS_MemSetZero(generatorG_ptr, *generGsizeBytes_ptr);
	
	if(orderQ_ptr != DX_NULL)
		DX_VOS_MemSetZero(orderQ_ptr, (orderQsizeBits+7)/8);
	
	if(factorJ_ptr != DX_NULL)
		DX_VOS_MemSetZero(factorJ_ptr, *JsizeBytes_ptr); 
	
	if(generateSeed == 1)
		DX_VOS_MemSetZero(seedS_ptr, (seedSizeBits + 7)/8);   
	
End:   
	/* cleaning of temp buffer */
	DX_VOS_MemSetZero(DHKGbuff_ptr, sizeof(DHKGbuff_ptr));
	
	return Error;
	
}/* End of DX_DH_CreateDomainParams */


#endif /*#if defined(!CRYS_NO_HASH_SUPPORT && ! CRYS_NO_PKI_SUPPORT && ! CRYS_NO_DH_SUPPORT*/


 
/******************************************************************************************/
/************************         Public Functions           ******************************/
/******************************************************************************************/

/******************************************************************************************/
/**
 * @brief The function generates a DH (DLP) domain parameters in GF(P) (see X9.42-2001)
 *
 * @param[in]  modPSizeBits       - The  modulus (prime) P size in bits equal 256*n, where n >= 4. 
 * @param[in]  orderQSizeBits     - The size of order of generator in bits. According to ANSI X9.43:
 *                                  m must be multiple of 32 bits and m >= 160. According to ANSI X9.30-1: 
 *                                  m = 160 bit. Because Q is prime factor of P-1, it must be <= (P-1)/2,  
 *                                  therefore orderQSizeBits <= modPSizeBits. 
 * @param[in]  seedSizeBits       - The  seed size in bits. Requirements: 
 *                                  seedSizeBits >= orderQSizeBits and seedSizeBits <= modPSizeBits ( the 
 *                                  last is required by our implementation).
 * @param[out] modP_ptr           - The prime modulus P of structure P = J*Q + 1, where Q is prime
 *                                  and j is an integer. Size of the buffer for output generated value must 
 *                                  be not less, than modulus size. 
 * @param[out] orderQ_ptr         - The pointer to the order Q of generator. Size of the buffer for output 
 *                                  generated value must be not less, than order size. 
 * @param[out] generatorG_ptr     - The pointer to the generator of multiplicative subgroup in GF(P).
 *                                  If the pointer == NULL, the function returns an error. Size of the buffer 
 *	                            for output generated value must be not less, than modulus size. 
 * @param [in/out]generGsizeBytes_ptr - The pointer to the one-word buffer, containing the generator size value (in bytes). 
 *                                  The user must set the size of allocated buffer, and the function returns the 
 *	                            actual size of the generator in bytes. 
 * @param[out] factorJ_ptr        - The pointer to buffer for integer factor J. If the pointer == NULL, the function 
 *                                  not puts this parameter out. In this case JsizeBytes_ptr must be set to NULL, 
 *                                  otherwise the function returns an error. The size of the buffer must be not less,
 *                                  than (modPSizesBytes - orderQSizeBytes + 1). 
 * @param [in/out] JsizeBytes_ptr  - The pointer to the size of integer factor J in bytes. If the pointer == NULL, 
*                                  the function not puts this parameter out. If output of the factor J is needed, the
*                                  user must set the J size value equal to the size of allocated buffer, and the 
*                                  function returns the actual size of J in bytes. 
 * @param [in/out] seedS_ptr       - The random seed used for generation of primes. The size of the buffer for output 
 *      			    generated value must be not less, than passed seed size and not less than
 *      			    20 bytes (160 bits). 
 * @param[in] generateSeed        - The flag, defining whether the seed generated randomly by the function
 *                                  (generateSeed = 1), or it is passed by the user (generateSeed = 0).                                 
 * @param[out] pgenCounter_ptr    - The pointer to a counter of tries to generate the prime. If the pointer == NULL, 
 *                                  the function not puts this parameter out.
 * @param[out] DHKGBuff_ptr       - The temp buffer for internal calculations. The buffer is defined as structure.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure - a predefined error code.
 *
 *     Note:  1. Input and Output vectors are in big endianness (most significant bit is left most one).
 *            2. In case of any error the function may clean the output buffers. 
 *
 */
CEXPORT_C CRYSError_t CRYS_DH_CreateDomainParams( 
				DxUint32_t          modPsizeBits,             /*in */
				DxUint32_t          orderQsizeBits,           /*in */
				DxUint32_t          seedSizeBits,             /*in */
				DxUint8_t          *modP_ptr,                 /*out*/  
				DxUint8_t          *orderQ_ptr,               /*out*/
				DxUint8_t          *generatorG_ptr,           /*out*/
				DxUint32_t         *generGsizeBytes_ptr,      /*in/out*/
				DxUint8_t          *factorJ_ptr,              /*out*/
				DxUint32_t         *JsizeBytes_ptr,           /*in/out*/ 
				DxUint8_t          *seedS_ptr,                /*in/out*/
				DxInt8_t            generateSeed,             /*in*/ 
				DxUint32_t         *pgenCounter_ptr,          /*out*/
				CRYS_DHKGData_t    *DHKGbuff_ptr              /*in */ )
{
  
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	DxUint32_t  modSizeBytes, orderSizeBytes;
	
	/* FUNCTION  LOGIC */
	
	Error = CRYS_OK;
	
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_DH_UNSUPPORTED( 
			modPsizeBits, orderQsizeBits, seedSizeBits, 
			modP_ptr, orderQ_ptr, generatorG_ptr, generGsizeBytes_ptr, 
			factorJ_ptr, JsizeBytes_ptr, seedS_ptr, generateSeed, 
			pgenCounter_ptr, DHKGbuff_ptr, modSizeBytes, orderSizeBytes, Error,
			Error, Error, Error, Error, Error, Error, Error, Error, Error, 
			Error, Error, Error, Error, Error, Error, Error, Error, Error); 
	
#ifndef CRYS_NO_HASH_SUPPORT                                      
#ifndef CRYS_NO_PKI_SUPPORT                                      
#ifndef CRYS_NO_DH_SUPPORT
	
	/* Step 1. Check input parameters */
	/*------------------------------- */
	
	/* check pointers: modP, orderQ and temp buffer. Note: other pointers may be NULL, if not used  */
	if(modP_ptr == DX_NULL   ||
	   orderQ_ptr == DX_NULL ||
	   seedS_ptr == DX_NULL  ||
	   DHKGbuff_ptr == DX_NULL) {
		return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;
	}
	
	/* check sizes */
	else if(modPsizeBits < CRYS_DH_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ||
		modPsizeBits % 256 != 0 ||
		modPsizeBits > CRYS_DH_MAX_VALID_KEY_SIZE_VALUE_IN_BITS) {

		return CRYS_DH_INVALID_MODULUS_SIZE_ERROR;
	}
	
	/* init the sizes */
	modSizeBytes = (modPsizeBits + 7)/ 8; 
	orderSizeBytes = (orderQsizeBits + 7)/ 8; 
	
	if(orderQsizeBits < CRYS_DH_SEED_MIN_SIZE_IN_BITS ||
	   orderQsizeBits >  (modPsizeBits - 1)   ||
	   orderQsizeBits % 32 != 0) {
		return CRYS_DH_INVALID_ORDER_SIZE_ERROR;   	
	}
	
	if(seedSizeBits < orderQsizeBits/* according to X9.42-2001 */  ||
	seedSizeBits > modPsizeBits /* our limitation of buffer size */) {
	return CRYS_DH_INVALID_SEED_SIZE_ERROR;   	
	}  
	
	/* check generator G pointers and buffer size */
	if((generatorG_ptr == DX_NULL) ||
	   (generGsizeBytes_ptr == DX_NULL) ||
	   (*generGsizeBytes_ptr < modSizeBytes)) {
		return CRYS_DH_INVALID_GENERATOR_PTR_OR_SIZE_ERROR;	
	}

	/* check J-factor pointers and buffer size */
	if((factorJ_ptr == DX_NULL && JsizeBytes_ptr != DX_NULL) ||
	   (factorJ_ptr != DX_NULL && JsizeBytes_ptr == DX_NULL) ||
	   ((JsizeBytes_ptr != DX_NULL) && (*JsizeBytes_ptr < (modSizeBytes - orderSizeBytes + 1)))) {
		return CRYS_DH_INVALID_J_FACTOR_PTR_OR_SIZE_ERROR;	
	}
	
	/* check generateSeed parameter */
	if(generateSeed != 0 && generateSeed != 1) {
		return CRYS_DH_INVALID_ARGUMENT_OPERATION_MODE_ERROR;
	}
	
	/*   call exec function */
	Error = DX_DH_CreateDomainParams( 
			modPsizeBits,             /*in */
			orderQsizeBits,           /*in */
			seedSizeBits,             /*in */
			modP_ptr,                 /*out*/  
			orderQ_ptr,               /*out*/
			generatorG_ptr,           /*out*/
			generGsizeBytes_ptr,      /*in/out*/
			factorJ_ptr,              /*out*/
			JsizeBytes_ptr,           /*in/out*/ 
			seedS_ptr,                /*in/out*/
			generateSeed,             /*in*/ 
			pgenCounter_ptr,          /*out*/
			DHKGbuff_ptr);            /*in */ 
	
	return Error;
	
#endif /*CRYS_NO_DH_SUPPORT*/
#endif /*CRYS_NO_PKI_SUPPORT*/
#endif /*CRYS_NO_HASH_SUPPORT*/	

}/* End of CRYS_DH_CreateDomainParams */


/******************************************************************************************/
/** 
* @brief The function checks the obtained DH domain parameters according X9.42-2001.
*
*        There may be 3 case of checking:
*        1. Checking of primes only ( modulus P and order Q according to passed seed S and pgenCounter).
*           In this case all pointers and sizes of said parameters must be passed (not NULL), but generator 
*           G pointer and it size must be both set to NULL.
*        2. Checking of generator G only in assuming that primes parameters P, Q are valid. In ths case 
*           the user must to pass the P,Q,G pointers and sizes. The seed S pointer and size must be both
*           set to NULL, otherwise the function returns an error.
*        3. Checking all domain parameters. In this case all input parameters must be passed to the function.
*
*        If any of checked domain parameters is not compliant to X9.42-2001 standard and our implementation
*        limitation, the function returns an error according to CRYS_DH_error.h file. 
*
*        NOTE:  Detailed requirements to all used parameters are described above in CRYS_DH_CreateDomainParams 
*               functions API.
*
* @param[out] modP_ptr           - The prime modulus P. Must be of structure P = j*Q + 1, 
*                                  where Q is prime and j is an integer.
* @param[in]  modPSizeBits       - The  modulus (prime) P size in bits equal 256*n, where n >= 4.  
* @param[out] orderQ_ptr         - The pointer to the order Q of generator. 
* @param[in]  orderQSizeBytes    - The size of order of generator in bytes. According to ANSI X9.43:
*                                  m must be multiple of 32 bits and m >= 160. According to ANSI X9.30-1: 
*                                  m = 160 bit. Because Q is prime factor of P-1, it must be <= (P-1)/2.  
*                                  In our implementation required, that orderQSize <= modPSizeBytes/2. 
* @param[in]  generatorG_ptr     - The pointer to the generator of multiplicative subgroup in GF(P).
* @param[in]  generatorSizeBytes - The size of generator in bytes (must be set if generator will be checked). 
* @param[in]  seedS_ptr          - The random seed used for generation of primes (must be set if 
*                                  primes will be checked).
* @param[in]  seedSizeBits       - The seed size in bits. If the seed is used,
*       			   then its size must be:
*       			   seedSizeBits >= orderQSizeBits and
*       			   seedSizeBits <= modPSizeBits ( the last is
*       			   required by our implementation).
* @param[in]  pgenCounter        - The counter of tries to generate the primes (must be set if primes 
*                                  will be checked).
* @param[in] TempBuff_ptr        - The temp buffer of defined structure.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure or if one or more domain  
*                       parameters are invalid the function returns a predefined error code.
*
*     Note:  Input vectors are in big endianness.
*   
*/
CEXPORT_C CRYSError_t CRYS_DH_CheckDomainParams( 
                          DxUint8_t               *modP_ptr,           /*in */  
                          DxUint32_t               modPsizeBytes,      /*in */
                          DxUint8_t               *orderQ_ptr,         /*in */
                          DxUint32_t               orderQsizeBytes,    /*in */
                          DxUint8_t               *generatorG_ptr,     /*in */
                          DxUint32_t               generatorSizeBytes, /*in */
                          DxUint8_t               *seedS_ptr,          /*in */
                          DxUint32_t               seedSizeBits,       /*in */
                          DxUint32_t               pgenCounter,        /*in */
                          CRYS_DHKG_CheckTemp_t   *checkTempBuff_ptr   /*in */ )
{
   /* FUNCTION DECLARATIONS */

   /* The return error identifier */
   CRYSError_t Error;
   

   /* pointers to temp buffers */
   DxUint32_t  *Q_ptr, *P_ptr;
   CRYS_DHKGData_t *DHKGbuff_ptr = &checkTempBuff_ptr->DhKgBuff;
   DxUint32_t  *TempBuff_ptr = (DxUint32_t*)&checkTempBuff_ptr->CheckTempBuff;
   
   /* size of modulus in bits and in words */
   DxUint32_t  modPsizeBits, modPsizeWords;

   /* size  order Q (in bits) */
   DxUint32_t  orderQsizeBits;
   
   /* counter of trying to generate modulus P; pgenCounter*/
   DxUint32_t  pgenCounter1;
   

   /* FUNCTION  LOGIC */
   
   Error = CRYS_OK;

   
   /* ............... if not supported exit .............................. */
   /* -------------------------------------------------------------------- */   

   RETURN_IF_DH_UNSUPPORTED( modP_ptr, modPsizeBytes, orderQ_ptr, orderQsizeBytes,  
                             generatorG_ptr, generatorSizeBytes, seedS_ptr,  
                             seedSizeBits, pgenCounter, checkTempBuff_ptr, DHKGbuff_ptr,  
                             Q_ptr, P_ptr, modPsizeBits, modPsizeWords, orderQsizeBits, 
                             pgenCounter1, TempBuff_ptr, Error, Error, Error, Error, Error, Error,
                             Error, Error, Error, Error, Error, Error, Error, Error,  Error, Error ); 
                              
   #ifndef CRYS_NO_HASH_SUPPORT                                      
   #ifndef CRYS_NO_PKI_SUPPORT                                      
   #ifndef CRYS_NO_DH_SUPPORT
 
   /*------------------------------- */
   /* Step 1. Check input pointers   */
   /*------------------------------- */
   
   /* check pointers: modP, generator and tempBuff. Note: other pointers may be NULL  */
   if(modP_ptr == DX_NULL ||
      orderQ_ptr == DX_NULL ||
      checkTempBuff_ptr == DX_NULL ||
      DHKGbuff_ptr == DX_NULL ) {
   	   return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;
   }

   /* check modulus and order sizes */
   if(modPsizeBytes < CRYS_DH_MIN_VALID_KEY_SIZE_VALUE_IN_BITS / 8 ||
      modPsizeBytes % 32 != 0 ||
      modPsizeBytes > CRYS_DH_MAX_MOD_SIZE_IN_BYTES ) {
   	   return CRYS_DH_INVALID_MODULUS_SIZE_ERROR;
   }
     
   if (orderQsizeBytes < CRYS_DH_SEED_MIN_SIZE_IN_BITS / 8 ||
       orderQsizeBytes % 4 != 0 ||
       orderQsizeBytes > modPsizeBytes) {
   	   return CRYS_DH_INVALID_ORDER_SIZE_ERROR;   	
   }
   
   /* Seed pointer and size checking:
      If pointer or size of seed are illegal, then output an error.
      Note: In case that primes checking is not needed, the seed pointer and size must be
	        set to NULL  and are legal */
   if((seedSizeBits == 0 && seedS_ptr != DX_NULL) ||
      (seedSizeBits != 0 && seedS_ptr == DX_NULL)) {
   	   return CRYS_DH_CHECK_SEED_SIZE_OR_PTR_NOT_VALID_ERROR;
   }

   /* Generator pointer and size checking:
      If pointer or size of generator are illegal, then output an error.
      Note: In case that generator checking is not needed, its pointer and size are equaled to NULL */
   if((generatorSizeBytes == 0 && generatorG_ptr != DX_NULL) ||
      (generatorSizeBytes != 0 && generatorG_ptr == DX_NULL)) {
	   return CRYS_DH_CHECK_GENERATOR_SIZE_OR_PTR_NOT_VALID_ERROR;
   }
   
  
   /* --------------------------------- */
   /*  Step 2.  Initializations         */
   /* --------------------------------- */  

   /* clean TempBuff_ptr */
   DX_VOS_MemSetZero(TempBuff_ptr, sizeof(CRYS_DHKG_CheckTemp_t));

   /* calculate P and Q size in bits */
   modPsizeWords = (modPsizeBytes + 3) / 4;

   /* set Q, P and G- pointers on DHKGbuff_ptr->PrimData temp buffers */
   Q_ptr = TempBuff_ptr;
   P_ptr = Q_ptr + modPsizeWords;
   
   if(seedS_ptr != DX_NULL)
   {
	   /*--------------------------------------------- */    
	   /* Step 3. Calculate and check primes sizes     */
	   /*--------------------------------------------- */ 

	   /* temporary convert P and Q to little endian bytes arrays  *        
	   *  for calculating their sizes in bits       	       */
	   CRYS_COMMON_ReverseMemcpy((DxUint8_t*)P_ptr, modP_ptr, modPsizeBytes);
	   CRYS_COMMON_ReverseMemcpy((DxUint8_t*)Q_ptr, orderQ_ptr, orderQsizeBytes);

	   modPsizeBits = CRYS_COMMON_GetCounterEffectiveSizeInBits((DxUint8_t*)P_ptr, (DxUint16_t)modPsizeBytes);  
	   orderQsizeBits = CRYS_COMMON_GetCounterEffectiveSizeInBits((DxUint8_t*)Q_ptr, (DxUint16_t)orderQsizeBytes);
	                                      
	   /*------------------------------------------------------------------- */   
	   /* Step 4. Generate random primes P,Q for given seed Seed S according */ 
	   /*         to ANSI X9.42 for comparing with input parameters          */
	   /*         The called CreateDomainParams also checks sizes of input   */
	   /*         parameters                                                 */
	   /*------------------------------------------------------------------- */ 

	   Error =  DX_DH_CreateDomainParams( 
					modPsizeBits,                    /*in */
					orderQsizeBits,                  /*in */
					seedSizeBits,                    /*in */
					(DxUint8_t*)P_ptr,               /*out*/  
					(DxUint8_t*)Q_ptr,               /*out*/
					DX_NULL /*generatorG_ptr*/,      /*out*/
					DX_NULL /*generatorSize_ptr*/,   /*out*/
					DX_NULL /*factorJ_ptr*/,         /*out*/
					DX_NULL /*JsizeBytes_ptr*/,      /*out*/ 
					seedS_ptr,                       /*in/out*/
					DX_FALSE /*generateSeed*/,       /*in*/ 
					&pgenCounter1,                   /*out*/
					DHKGbuff_ptr);                   /*in */   
	                          
	   if(Error != CRYS_OK)
		  goto End;                                                        
                                  
	   /* -------------------------------------------------------------------*/                               
	   /* Step 5. Compare generated primes with input, if one of compares   */
	   /*         is not "equal", the output error                          */
	   /* -------------------------------------------------------------------*/
	   
	   if( DX_VOS_MemCmp(modP_ptr, (DxUint8_t*)P_ptr, modPsizeBytes) != 0) {
   			Error = CRYS_DH_CHECK_DOMAIN_PRIMES_NOT_VALID_ERROR;
   			goto End;
	   }
	   
	   else if(DX_VOS_MemCmp(orderQ_ptr, (DxUint8_t*)Q_ptr, orderQsizeBytes ) != 0) {
   			Error = CRYS_DH_CHECK_DOMAIN_PRIMES_NOT_VALID_ERROR;
   			goto End;   		
	   }

	   /* compare pgen counters */
	   else if(pgenCounter != pgenCounter1) {
   			Error = CRYS_DH_CHECK_DOMAIN_PRIMES_NOT_VALID_ERROR;
   			goto End;   		
	   }
   }

   /*-----------------------------------------------------------------*/
   /* Step 4. Check generator using the function for checking of      */
   /*    the public key because both perform identical operations     */
   /*    with appropriate parameters. In this case:                   */ 
   /*    if G > P-2, or G < 2, or G^Q != 1, then output an error      */ 
   /*-----------------------------------------------------------------*/   
   
   if(generatorG_ptr != DX_NULL)
   {
		Error = CRYS_DH_CheckPubKey( 
			  modP_ptr,                                   /*in */  
			  modPsizeBytes,                              /*in */
			  orderQ_ptr,                                 /*in */
			  orderQsizeBytes,                            /*in */
			  generatorG_ptr,                             /*in */
			  generatorSizeBytes,                         /*in */
			 &DHKGbuff_ptr->ExpTemps);                    /*in */ 

	   /* Set error code according to checked parameter issue */                          
	   if( Error == CRYS_DH_INVALID_PUBLIC_KEY_SIZE_ERROR || 
	       Error == CRYS_DH_INVALID_PUBLIC_KEY_ERROR ) { 
		    	
		Error =  CRYS_DH_CHECK_GENERATOR_NOT_VALID_ERROR;                      
	   }
   }   
   
End:      
   /* cleaning of temp buffers */
   DX_VOS_MemSetZero(&DHKGbuff_ptr->ExpTemps, sizeof(DHKGbuff_ptr->ExpTemps));
   
    /* End of function */
    
   	return Error;
	
  #endif /*CRYS_NO_DH_SUPPORT*/
  #endif /*CRYS_NO_PKI_SUPPORT*/
  #endif /*CRYS_NO_HASH_SUPPORT*/	

}
                          
