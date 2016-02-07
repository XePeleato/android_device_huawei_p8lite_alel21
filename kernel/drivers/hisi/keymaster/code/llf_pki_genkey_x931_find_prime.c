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

#include "dx_vos_basetypes.h"
#include "plat_print_dbg.h"
#include "crys_rsa_types.h"
#include "crys_common_math.h"
#include "crys_rnd.h"
#include "llf_pki.h"
#include "llf_pki_error.h"
#include "llf_rnd.h"
#include "dx_vos_mem.h"


/************************ Defines ******************************/

/* canceling the lint warning:
Info 716: while(1) ... */
/*lint --e{716} */
/* canceling the lint warning: Constant value Boolean
Warning 506 regarding while(1) ... */
/*lint --e{506} */

/* canceling the lint warning:
Info 506: Constant value Boolean ... */
/*lint --e{506} */

/* canceling the lint warning:
Info 774: Boolean within 'if' always evaluates to False */
/*lint --e{774} */

/* ..................... PRIME1 definitions ........................ */

/*if the number of Rabin-Miller tests in RSA key generation */
#define LLF_PKI_KG_X931_RABIN_TESTS_FOR_101_255_BITS    27  
#define LLF_PKI_KG_X931_RABIN_TESTS_FOR_256_511_BITS    15
#define LLF_PKI_KG_X931_RABIN_TESTS_FOR_512_1023_BITS    8  
#define LLF_PKI_KG_X931_RABIN_TESTS_FOR_GREAT_1023_BITS  4

#define PRIME_PRODUCT_NUM ( min(LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS/32,CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS) )
#define PRIME_PRODUCT_BUFF_SIZE ( max(LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS/32,CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS) )
#define PRIME_NUM 256

#define primeProdNum   (PRIME_PRODUCT_NUM/2 - 3) 



/**************************** Enums ********************************/ 


/************************ Global Data ******************************/

const DxUint32_t g_PrimeProduct[PRIME_PRODUCT_BUFF_SIZE] = 
{
  3234846615UL,95041567UL,907383479,4132280413UL,121330189,257557397UL,490995677,842952707, 
  1314423991UL,2125525169UL,3073309843UL,16965341,20193023,23300239,29884301,35360399,
  42749359UL,49143869,56466073,65111573,76027969,84208541,94593973,103569859,119319383,133390067UL,
  154769821UL,178433279,193397129,213479407,229580147,250367549,271661713,293158127,319512181,
  357349471UL,393806449,422400701,452366557,507436351,547978913,575204137,627947039,666785731,
  710381447UL,777767161UL,834985999UL,894826021UL,951747481UL,1019050649UL,1072651369UL,1125878063UL,1185362993UL,
  1267745273UL,1322520163UL,1391119619UL,1498299287UL,1608372013UL,1700725291UL,1805418283UL,1871456063UL,
  2008071007UL,2115193573UL,2178429527UL,2246284699UL,2385788087UL 
};

const DxUint16_t g_SmallPrime[PRIME_NUM] = 
{
  3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,
  113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,
  229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,
  359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,
  487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,
  619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,
  761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,
  911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,
  1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,
  1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,
  1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,
  1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,
  1579,1583,1597,1601,1607,1609,1613,1619,1621
};  

const DxUint16_t g_LastProductPrime[PRIME_PRODUCT_BUFF_SIZE] = 
{
  9,14,19,24,28,32,36,40,44,48,52,55,58,61,64,67,70,73,76,79,82,85,88,91,94,97,100,103,106,109,112,
  115,118,121,124,127,130,133,136,139,142,145,148,151,154,157,160,163,166,169,172,175,178,181,
  184,187,190,193,196,199,202,205,208,211,214,217
  
};

/*-----------------------------------------------------------------------*/
/* define Debug printing level for LLF_PKI_KG module */
#define DebPrint 5

#ifdef LLF_PKI_PKA_DEBUG 
#if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
	DxUint8_t PQindex;
	/* for debug goals */
	/* for Q */
	/* P3 big end 1E05B8F18D807DB7A47EF53567 nearest random is: 1E 05 B8 F1 8D 80 7D B7 A4 7E F5 35 99*/
	DxUint8_t  rBuff3[16] = { 
	/*0xFB,0xD4,0xD3,0x19,0xA8,0x6B,0x7C,0x8C,0x80,0x6D,0x66,0xB2,0x1C*/
	/*0xC3,0xB6,0x75,0xA3,0x29,0x20,0xFB,0x37,0x1F,0x86,0xD7,0xBE,0x19*/
	0x67,0x35,0xF5,0x7E,0xA4,0xB7,0x7D,0x80,0x8D,0xF1,0xB8,0x05,0x1E};
	/* P4 big end 1E05B8F18D807DB7A47EF5359A nearest random is: 1E 05 B8 F1 8D 80 7D B7 A4 7E F5 35 A7*/
	DxUint8_t  rBuff4[16] = { 
	/*0x29,0x18,0x50,0x8B,0x32,0x12,0x2E,0x56,0x2C,0x35,0x6D,0x6F,0x1F*/
	/*0x07,0x77,0xC3,0x08,0x45,0xEB,0x47,0xC1,0x7F,0xED,0x79,0xB6,0x1D*/
	0x9A,0x35,0xF5,0x7E,0xA4,0xB7,0x7D,0x80,0x8D,0xF1,0xB8,0x05,0x1E};
	
	/* for P */
	/* P1 big end 1E05B8F18D807DB7A47EF52563 nearest random is: 1E05B8F18D807DB7A47EF52563 */
	DxUint8_t  rBuff1[16] = {
	/*0xF1,0x20,0x85,0xD2,0x95,0xC8,0x61,0x3F,0x93,0x03,0x24,0xB2,0x1A*/
	/*0xEF,0xA7,0x21,0x4E,0x4B,0x49,0x60,0x2E,0xC5,0x7D,0x1B,0x83,0x17*/ 
	0x63,0x25,0xF5,0x7E,0xA4,0xB7,0x7D,0x80,0x8D,0xF1,0xB8,0x05,0x1E};
	/* P2 big end 1E05B8F18D807DB7A47EF51595 nearest random is: 1E05B8F18D807DB7A47EF515A1 */
	DxUint8_t  rBuff2[16] = {
	/*0xB5,0xE5,0x1E,0x07,0x67,0xBC,0xB0,0xB9,0xAC,0xA6,0x69,0x03,0x17*/
	/*0x4B,0xA5,0x0C,0x16,0x68,0x86,0x0F,0x1C,0xAF,0x43,0xDB,0xE3,0x19*/
	0x95,0x15,0xF5,0x7E,0xA4,0xB7,0x7D,0x80,0x8D,0xF1,0xB8,0x05,0x1E};
	
	
	/* temp buffers for output results of generation P1,P2 for P and Q  */ 
	DxUint32_t P1pR[4],  P2pR[4],  P1qR[4],  P2qR[4];
	/* final values of P1,P2 for P and Q */
	DxUint32_t P1pPr[4], P2pPr[4], P1qPr[4], P2qPr[4];
	DxUint32_t *P1R_ptr, *P2R_ptr, *P1Pr_ptr, *P2Pr_ptr;
	
	/* temp buffers and pointer for output the P,Q  after generation */
	DxUint32_t rBuffP[64], rBuffQ[64];
	DxUint32_t  *PQ_ptr;
#endif
#endif

/************************ External functions ******************************/ 

CRYSError_t DX_RND_GenerateWordsArrayInRange(
					DxUint32_t   rndSizeInBits, 
					DxUint32_t  *maxVect_ptr, 
					DxUint32_t  *rndVect_ptr,
					DxUint32_t  *tmp_ptr);

/************************* Private functions ******************************/ 

/********************************************************************************************/
/* @brief This function calculates Jacobi index .
 *
 * If there is such a vector b, that satisfies the condition b^2 = a mod p, the result is 1.
 * If there is no such vector, the result is -1.
 *
 * @param[in] LenId - the RegsSizesTable entry, containing the exact size of vector p in bits.
 * @param[in] rA - The virtual pointer to the base vector.
 * @param[in] rP - The virtual pointer to the prime to be tested (the modulos).
 * @param[out] JacRes_ptr - a pointer to the result var (1,0,-1) as described in the description.
 * @param[in] PrimeSizeInBits - The prime size in bits.
 *            operations on temp buffers.
 * @param[in] rA1, rP1, rT - virtual pointers to temp PKA registers.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 */
static CRYSError_t LLF_PKI_KG_X931_Jacobi (
                                    DxUint32_t  LenID,        /*in*/                                   
                                    DxInt8_t    rA,           /*in*/  
                                    DxInt8_t    rP,           /*in*/
                                    DxInt32_t   *JacRes_ptr,  /*out*/
                                    DxInt8_t    rA1,          /*temp reg*/
                                    DxInt8_t    rP1,          /*temp reg*/
                                    DxInt8_t    rT,           /*temp reg*/
                                    DxUint32_t  VirtualHwBaseAddr )
{
   /* FUNCTION DECLARATIONS */

  DxInt32_t    k, s;
  DxUint32_t   residue;

  /* comper result */	
  DxUint32_t cmpRes;
	
  /* low words of A1 and P1 */
  DxUint32_t A1_0, P1_0;
	
  /* current address */
  DxUint32_t currAddr;
	
  /* temp swap value */
  DxInt8_t  rSw;

  /* PKA status */
  DxUint32_t status;
  
  
  /* FUNCTION LOGIC */

  /* .......................... initialize local variables ............................... */
  /* ------------------------------------------------------------------------------------- */
  
  /* copy the input vectors with extension */
  LLF_PKI_PKA_Copy( LenID+1, rA1/*dest*/, rA/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
  LLF_PKI_PKA_Copy( LenID+1, rP1/*dest*/, rP/*src*/, 0/*Tag*/, VirtualHwBaseAddr );

  /* initialize the result as 1 ( default ) */
  *JacRes_ptr = 1;
  
  /* ..................... if a is 1 return the result 1 ...................... */
  /* -------------------------------------------------------------------------- */
  
  /* step 3.  if a1 == 1, return - we have done */  
  LLF_PKI_PKA_CompareIm(LenID+1, rA1/*OpA*/, 1/*OpB*/, 0/*Tag*/, VirtualHwBaseAddr);

  /* read zero bit from the status; if Z == 1 then done */        
  LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );
  if( status == 1 )

    return CRYS_OK;
       
  
  /* ..................... do loop for finding the jacobi ..................... */
  /* -------------------------------------------------------------------------- */
  
  do
  {
	    /* Step 1.  If a == 0, return the result 0      */
	    /*----------------------------------------------*/
	    LLF_PKI_PKA_CompareIm(LenID, rA1/*OpA*/, 0/*OpB*/, 0/*Tag*/, VirtualHwBaseAddr);

	    /* read zero bit from the status; if Z == 1 then done */        
	    LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );
	    
	    if( status == 1 )
	    {
	       *JacRes_ptr = 0;
	       return CRYS_OK;
	    }
	    
	                                                            
	    /* Step 2. Find out larger power of two for A1  */
	    /*----------------------------------------------*/
	    
	    k = 0;
	    
	    /* check parity of A1 */
	    LLF_PKI_PKA_Tst0(LenID, rA1/*OpA*/,0/*Tag*/, VirtualHwBaseAddr);
	    
	    LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, cmpRes);
	    
	    while( cmpRes == 1 ) 
	    {
	      /* divide by 2 */
	      LLF_PKI_PKA_SHR0(LenID, rA1/*OpA*/, 1-1/*S*/, rA1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
		  LLF_PKI_PKA_Tst0(LenID, rA1/*OpA*/,0/*Tag*/, VirtualHwBaseAddr);
	      LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, cmpRes);
	      k++;
	    }

	    /* get low bytes of A1 and P1                   */
	    /*----------------------------------------------*/
	    
	    LLF_PKI_PKA_Terminate(0/*Tag*/, VirtualHwBaseAddr);
	    
	    LLF_PKI_PKA_ReadRegAddress(rA1, currAddr, VirtualHwBaseAddr);
	    LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , currAddr , A1_0 );
	  
	    LLF_PKI_PKA_ReadRegAddress(rP1 , currAddr, VirtualHwBaseAddr);
	    LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , currAddr , P1_0 );

	 
	    /* initialize s as 0 */
	    s = 0; 
	    
	    /* step 3.  if k is even set s=1                */
	    /*----------------------------------------------*/
	    if ((k & 1) == 0) 
	    {
	      s = 1;
	    } 
	    else 
	    {
	      /* else set s=1 if p = 1 or 7 (mod 8) or s=-1 if p = 3 or 5 (mod 8) */
	      residue = P1_0 & 7;

	      if (residue == 1 || residue == 7) 
	      {
	        s = 1;
	      } 
	      else if (residue == 3 || residue == 5) 
	      {
	        s = -1;
	      }
	    }

	    /* Step 4.  If p == 3 (mod 4) *and* a1 == 3 (mod 4) then s = -s */
	    /*--------------------------------------------------------------*/
	    if ( ((P1_0 & 3) == 3) && ((A1_0 & 3) == 3)) 
	    {
	      s = -s;
	    }
	   
	    /* Step 5 : Update the result                   */
	    *JacRes_ptr *= s;
	    

	    /* Step 6.  If a1 == 1, return - done           */
	    /*----------------------------------------------*/
	    
	    LLF_PKI_PKA_CompareIm(LenID, rA1/*OpA*/, 1/*OpB*/, 0/*Tag*/, VirtualHwBaseAddr);

	    LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );	    
	    if( status == 1 )
	      return CRYS_OK;

	    /* p1 = p1 mod a1 - the result is at rP1 register  */
	    LLF_PKI_PKA_Div( LenID , rP1/*OpA*/, rA1/*OpB*/, rT/*ResNotUsed*/, 0/*Tag*/, VirtualHwBaseAddr);

	    /* Step 7.  Exchange P1 & A1                    */
	    /*----------------------------------------------*/
	    rSw = rP1;
	    rP1   = rA1;
	    rA1   = rSw;
                     				         
  }while(1); /* end of do loop */

	 			         
}/* END OF LLF_PKI_KG_X931_Jacobi */



/***********************************************************************************************/
/**
 * @brief This function executes the rabin miller test according to the the ANS X9.31 standard.
 *
 *    Algorithm:
 *        1. Let: prime candidate P = 1 + 2^a * m, where: m is odd and a > 0.
 *        2. For( i = 0; i < countTests; i++ ) do
 *             2.1. Generate random number b in range  1 < b < P.
 *             2.2. Calculate z = b^m mod P.
 *             2.4. For(j = 1; j < a; j++ ) do
 *             	     2.4.0. If j=0 and z = 1, or if z = P-1, then goto 3.
 *      	     2.4.
 *                   2.4.1. If j<a set z = z^2 mod P 
 *                   2.4.2. If  z != P-1, then goto st.3.
 *                   2.4.3. If  z = 1, then output "composite" and stop.
 *                   2.4.5. end do //2.4.
 *        3. End do //2.   
 *        4. Output "composite". Stop.
 *        5. Output P is "probable prime". Stop.
 *
 *       Assumings: - PKA is initialised on default mode for prime P as modulus (see near);
 *                  - the registers sizes table and mapping table are set on default mode,
 *                    according to exact P size, that means: 
 *                      -- registers sizes table entries are set by default as follows:
 *                           LenID - P_sizeBits, LenID+1 - (32*P_sizeWords + 32 bit);
 *                  - the prime candidate P is inserted in the modulus register 0;
 *                  - the Barrett tag NP for P is inserted into register 1;
 *                  - the PKA clocks are initialized.
 *
 *       NOTE: - The function uses 7 PKA data registers: 0,1,30,31, and 3 temp registers.   
 *        
 * @param[in]  LenID - The ID of entry in RegsSizesTable, containing PSizeInBits.
 * @param[in]  PSizeInBits - The prime candidate size
 * @param[out] SuccessCode_ptr - the success code : 0 - the test failed , 1 the test passed.
 * @param[in]  testsCount - Count of exponentiations in test. If CountTests = 0, then 
 *                          CountTests will be set automatically according to prime size, else the 
 *                          function performs recieved count of tests.
 *                             operations on temp buffers.
 * @param[in]  rT0,rT1,rT2 - virtual pointers to temp registers,
 * @param[in]  temp_ptr - pointer to temp buffer of size (2*PSizeInBits/8) bytes. 
 * @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * assumptions : the max size supported is 2112 bits.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

static CRYSError_t LLF_PKI_KG_X931_MillerRabinTest(
                                            DxUint32_t  LenID,           /*in*/
                                            DxUint32_t  PSizeInBits,     /*in*/
                                            DxInt8_t   *SuccessCode_ptr, /*out*/
                                            DxUint8_t   testsCount,      /*in*/ 
                                            DxInt8_t    rT0,    /*temp registers*/
                                            DxInt8_t    rT1,
                                            DxInt8_t    rT2,
                                            DxUint32_t  *temp_ptr,
                                            DxUint32_t  VirtualHwBaseAddr  /*in*/  )
{                                            
  /* FUNCTION DECLARATIONS */


  /* error return code */
  CRYSError_t Error;

  /* The number of iterations */
  DxUint8_t NumOfIterations;
  
  /* PKA registers pointers */
  DxInt8_t r0=0;

  /* loop variables */
  DxUint32_t  i, j;
  
  /* aValue - count of right most nulls in P-1 */ 
  DxUint32_t aValue, pSizeInWords;
  
  /* PKA status */
  DxUint32_t status;
  
  /* temp buffer 4 words for random generation */
  DxUint32_t *tempP_ptr, *tempB_ptr;
  
  /* FUNCTION LOGIC */
 
  /* ...................... initializations            ............................ */
  /* ------------------------------------------------------------------------------ */
  
  /* initialize the Error to O.K */
  Error = CRYS_OK;

  /*------------------------------------------------*/
  /* St. 1. Init variables and registers            */
  /*------------------------------------------------*/
  
  /* set the number of iterations */  
  if( testsCount != 0 )  
    NumOfIterations =  testsCount;
  
  else if( PSizeInBits < 256 )  
    NumOfIterations = LLF_PKI_KG_X931_RABIN_TESTS_FOR_101_255_BITS;
     
  else if( PSizeInBits < 512 )  
    NumOfIterations = LLF_PKI_KG_X931_RABIN_TESTS_FOR_256_511_BITS;
    
  else if( PSizeInBits < 1024 )
    NumOfIterations = LLF_PKI_KG_X931_RABIN_TESTS_FOR_512_1023_BITS;
    
  else if( PSizeInBits >= 1024 ) 
    NumOfIterations = LLF_PKI_KG_X931_RABIN_TESTS_FOR_GREAT_1023_BITS;
  
  /* prime size in words */
  pSizeInWords = (PSizeInBits+31)/32;
 
  tempP_ptr = &temp_ptr[pSizeInWords]; 
  tempB_ptr = &temp_ptr[2*pSizeInWords];  
                
  /* ................ clearing the temp registers ..................... */
  LLF_PKI_PKA_Clear(LenID+1, rT0/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );
  LLF_PKI_PKA_Clear(LenID+1, rT1/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );
  LLF_PKI_PKA_Clear(LenID+1, rT2/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );
  
  /*--------------------------------------------------------*/
  /* St. 1.2. Calculate a and m such, that P = 1 + 2^a * m  */
  /*        m=>rT0, a=>aValue                               */
  /*--------------------------------------------------------*/

  /* copy P into register rT0 */
  LLF_PKI_PKA_Copy( LenID+1, rT0/*dst*/, r0/*src=P*/, 0/*Tag*/, VirtualHwBaseAddr );   
  	  
  /* rT0 = P - 1 */
  LLF_PKI_PKA_SubIm( LenID, rT0/*P*/, 1/*imm*/, rT0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );

  /* set P-1 in tempP buff */
  LLF_PKI_PKA_CopyDataFromPkaReg(tempP_ptr, pSizeInWords, rT0/*srcReg*/, VirtualHwBaseAddr); 

  /* a = 1 */
  aValue = 1;
  
  while( 1 )
  {
	  /* divide: rT0 = rT0 / 2 */
  	  LLF_PKI_PKA_SHR0( LenID, rT0/*P*/, 1-1/*OpB*/, rT0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
  	
  	  /* test parity of rT0 */
  	  LLF_PKI_PKA_Tst0( LenID, rT0/*P*/, 0/*Tag*/, VirtualHwBaseAddr );
     
	  LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
    
	  if(status == 1)
  	  	aValue++;
  	  else
  	   	break;
  }

  /* --------------------------------------------------------------------- */
  /* St. 2. Rabin-Miller test main loop                                    */
  /* --------------------------------------------------------------------- */
  *SuccessCode_ptr = DX_TRUE;
  
  for( i = 0 ; i < NumOfIterations ; i++ )	
  {
	/*---------------------------------------------------------------------*/
	/* St. 2.1. Prepare a randon number b, used for the Rabin-Miller test  */
	/*          as the Base of exponentiation. The number must be not      */ 
	/*          larger, than 					      */
	/*---------------------------------------------------------------------*/   
				     
	/* generate a random number b=>rT1 for testing the primality of P by  * 
	*  exponentiation       					      */
	DX_VOS_MemSetZero(tempB_ptr, sizeof(DxUint32_t)*pSizeInWords);
	Error = DX_RND_GenerateWordsArrayInRange(
				PSizeInBits, 
				tempP_ptr/*(P-1) - maxVect*/, 
				tempB_ptr/*Rnd*/,
				temp_ptr/*temp buff*/);
	if( Error != CRYS_OK )       
		return Error;
	
	LLF_PKI_PKA_CopyDataIntoPkaReg( rT1/*dstReg*/, LenID, tempB_ptr/*src_ptr*/, 
			      pSizeInWords, VirtualHwBaseAddr );
	
	/*------------------------------------------------*/
	/* St. 2.2. Calculate: z = rT1 = z^m mod P        */
	/*        Set j = 0.                              */
	/*------------------------------------------------*/

	//!!Debug
	LLF_PKI_PKA_Copy( LenID+1, r0/*dst*/, r0/*src=P*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID+1, 1/*dst*/,  1/*src=NP*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID+1, rT1/*dst*/, rT1/*src=P*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID+1, rT0/*dst*/, rT0/*src=P*/, 0/*Tag*/, VirtualHwBaseAddr ); 
	
	LLF_PKI_PKA_ModExp( LenID, rT1/*opA=b*/, rT0/*OpB=m*/, rT1/*Res=z*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/*------------------------------------------------*/
	/* St. 2.3. Check; if z = 1 or z = P-1, then      */
	/*          generate next B                       */
	/*------------------------------------------------*/
	/* z == 1 ? */
	LLF_PKI_PKA_SubIm( LenID, rT1/*z*/, 1/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr );	  
	LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );  /* test zero of z-1 */  
	if( status == 1 )
		goto passed_this_iteration;
	
	/* rT2 = P - 1 */
	LLF_PKI_PKA_SubIm( LenID, r0/*P*/, 1/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* z == P-1 ? */
	LLF_PKI_PKA_Sub( LenID, rT2/*P*/, rT1/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr );   
	LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );  /* test zero */
	if( status == 1 )
		goto passed_this_iteration;
	
	/*------------------------------------------------*/
	/* St. 2.4. Loop: do while not meet conditions    */
	/*        (j == 0 && z == 1) or (z== P-1 )        */
	/*---------------------------- -------------------*/
	for( j = 1; j < aValue; j++ )
	{
		/* St. 2.4.1. z= z^2 mod m  */
		LLF_PKI_PKA_ModMul( LenID, rT1/*P*/, rT1/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		
		/* St. 2.4.2. if z == P-1, then break and next i */
		LLF_PKI_PKA_Sub( LenID, rT2/*P*/, rT1/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );  /* test zero */
		if( status == 1 )
			goto passed_this_iteration;
		
		/* St. 2.4.3. if z == 1, then output composite and stop */
		LLF_PKI_PKA_SubIm( LenID, rT1/*P*/, 1/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );  /* test zero */
		if( status == 1 )
		{
		   *SuccessCode_ptr = DX_FALSE;
		   goto End;
		}
		
	}/* end for */
    
	*SuccessCode_ptr = DX_FALSE;
	goto End;
       
 passed_this_iteration: ;
   
  } /* end main for */
  
End:

  /* delete secure sensitive data and exit */
  aValue = 0;
/* clear temp and tempP */
  DX_VOS_MemSetZero(temp_ptr, 2*sizeof(DxUint32_t)*pSizeInWords);
 
  return Error;

   
}/* END OF LLF_PKI_KG_X931_MillerRabinTestPKA */ 




/***********************************************************************************************/
/**
 * @brief This function executes the Lucas test according to the the X931 standard.
 *
 * @param[in]  LenID - The ID of entry in RegsSizesTable, containing exact size of P in bits.
 * @param[out] SuccessCode_ptr - the success code : 0 - the test failed , 1 the test passed.
 * @param[in]  CountTests - count of exponentiations in test. If CountTests = 0, then 
 *                          CountTests will be set automatically according to prime size, else the 
 *                          function performs recieved count of tests.
 * @param[in]  regTemps_ptr - pointer to temp registers list - 7 registers.
 * @param[in]  tempsCount   count of temp registers in the list.
 * @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * assumptions : the max size supported is 2112 bits.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

static CRYSError_t LLF_PKI_KG_X931_LucasPrimeTest(
                                            DxUint32_t  LenID,              /*In*/
                                            DxInt8_t   *SuccessCode_ptr,    /*out*/
                                            DxInt8_t   *regTemps_ptr,       /*In*/
                                            DxUint32_t  tempsCount,         /*In*/
                                            DxUint32_t  VirtualHwBaseAddr   /*In*/ ) 
{
   
   /* FUNCTION DECLARATIONS */

   /* the Error identifier */
   CRYSError_t Error; 

   
   /* virtual pointers to registers */
   DxInt8_t rP, rD, rK, rU, rV, rUnew, rVnew, rT;

   /* vector sizes */
   DxUint32_t kSizeInBits;
   
   /* internal variables */
   DxUint32_t d_abs;
   DxInt8_t d_is_positive;
   
   /* current memory address */
   DxUint32_t curAddr;
   
   /* Jacobi result */
   DxInt32_t JacobiRes;
 
   /* loop variable */
   DxInt32_t i;
   
   /* PKA status */
   DxUint32_t status;
      

   /* FUNCTION LOGIC */
   
   /* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
   if( tempsCount < 7 )   
      return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;  
#else
   tempsCount= tempsCount;
#endif
   
   /* ........................... initialize local variables ....................... */
   /* ------------------------------------------------------------------------------ */
   
   /* allocate registers */
   rP=0;  /* already is initialized by P */
   rD = regTemps_ptr[0];
   rK = regTemps_ptr[1]; 
   rU = regTemps_ptr[2]; 
   rV = regTemps_ptr[3]; 
   rUnew = regTemps_ptr[4]; 
   rVnew = regTemps_ptr[5]; 
   rT = regTemps_ptr[6]; 
   
 
   /* ............................ setting the d vector .............................. */
   /* -------------------------------------------------------------------------------- */
      /*  clear the temp buffer  */   
   LLF_PKI_PKA_Clear(LenID+1, rD/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );

   for( d_abs = 5 , d_is_positive = 1 ; ; d_abs+=2 , d_is_positive = !d_is_positive)
   {
	/* set D = d_abs  */
	LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );        
	LLF_PKI_PKA_GetRegAddress(rD,curAddr, VirtualHwBaseAddr);
	LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , curAddr , d_abs );
	
	/* if D is negative set D = P - D */
	if( d_is_positive == 0 ){
		  LLF_PKI_PKA_Sub( LenID, rP/*P*/, rD/*OpB*/, rD/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );		       
	}
	
	Error = LLF_PKI_KG_X931_Jacobi( LenID, rD, rP, &JacobiRes, 
				      rU/*temp*/, rV/*temp*/, rT/*temp*/,VirtualHwBaseAddr );
	
	if( Error != CRYS_OK )
		return Error;
	
	if( JacobiRes == -1 )
	
	break;                                
 
   }/* end of loop for finding d */
   

   /* ............................ init vectors for the test loop ................. */
   /* ----------------------------------------------------------------------------- */
 
   /* K = P + 1 */   
   LLF_PKI_PKA_AddIm( LenID, rP/*P*/, 1/*OpB*/, rK/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
   
   /* set the size of K in bits */
   kSizeInBits = LLF_PKI_PKA_GetRegEffectiveSizeInBits( LenID, rK/*reg*/, 0/*Tag*/, VirtualHwBaseAddr ); 
   
   /* init U and V to 1 */
   LLF_PKI_PKA_Clear(LenID+1, rU/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );
   LLF_PKI_PKA_Set0(LenID, rU/*regNum*/, rU/*Res*/, 0/*Tag*/,  VirtualHwBaseAddr );
   LLF_PKI_PKA_Copy( LenID+1, rV/*dest*/, rU/*src*/, 0/*Tag*/, VirtualHwBaseAddr );

   
   /* ..................... the main test loop      ............................ */
   /* -------------------------------------------------------------------------- */     
  
   for( i = (DxInt32_t)(kSizeInBits - 2) ; i >= 0 ; --i )
   {    
     /* a bit value */
     DxUint32_t bit;

     PLAT_LOG_DEV_PRINT(( 5 , "-------- i= %d  ------------------------------\n", i));
     
     /* Unew = U*V mod P */
     LLF_PKI_PKA_ModMul( LenID, rU/*OpA*/, rV/*OpB*/, rUnew/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); 
 
      /* Vnew = V^2 mod P */ 
     LLF_PKI_PKA_ModMul( LenID, rV/*OpA*/, rV/*OpB*/, rVnew/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); 
       
     /* rT = U^2 */
     LLF_PKI_PKA_ModMul( LenID, rU/*OpA*/, rU/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );

     /* rT= D * U^2 */
     LLF_PKI_PKA_ModMul( LenID, rD/*OpA*/, rT/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
     
     /* Vnew = (V^2 + D*U^2) */   
     LLF_PKI_PKA_Add( LenID+1, rT/*OpA*/, rVnew/*OpB*/, rVnew/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
     /* modular division by 2 */     
     LLF_PKI_PKA_ModDivideBy2( LenID, rVnew, rP/*mod*/, rVnew, 0/*Tag*/, VirtualHwBaseAddr );     

     /* swap V,Vnew and U,Unew */
     LLF_PKI_SwapInt8(rVnew,rV);
     LLF_PKI_SwapInt8(rUnew,rU);      

     /* get bit i from register K */
     bit = LLF_PKI_PKA_GetBitFromPkaReg( rK, LenID, i, rT, VirtualHwBaseAddr );
     
     if( bit != 0 )
     {
       /* Unew = (U+V)/2 */
       LLF_PKI_PKA_Add( LenID+1, rV/*OpA*/, rU/*OpB*/, rUnew/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
       /* modular division by 2 */     
       LLF_PKI_PKA_ModDivideBy2( LenID, rUnew, rP/*mod*/, rUnew, 0/*Tag*/, VirtualHwBaseAddr );     
       
       /* Vnew = (U*D+V)/2 */
       LLF_PKI_PKA_ModMul( LenID, rD/*OpA*/, rU/*OpB*/, rVnew/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
       LLF_PKI_PKA_Add( LenID+1, rV/*OpA*/, rVnew/*OpB*/, rVnew/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
       LLF_PKI_PKA_ModDivideBy2( LenID, rVnew, rP/*mod*/, rVnew, 0/*Tag*/, VirtualHwBaseAddr );     
   
       /* swap V,Vnew and U,Unew */
       LLF_PKI_SwapInt8(rVnew,rV);
       LLF_PKI_SwapInt8(rUnew,rU);

     }/* end of bit is set to 1 */

   }/* end of loop */

   /* U = U mod P */
   LLF_PKI_PKA_Div( LenID, rU/*OpA*/, rP/*OpB*/, rT/*ResNotUsed*/, 0/*Tag*/, VirtualHwBaseAddr );
   
   /* if U is equal to 0 return success code = 1, else 0 */
   /*----------------------------------------------------*/
   
   LLF_PKI_PKA_CompareIm( LenID, rU/*OpA*/, 0/*OpB immed*/, 0/*Tag*/, VirtualHwBaseAddr );

   LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );
    
   if( status == 1 )
	   *SuccessCode_ptr = 1;
     
   else   
	   *SuccessCode_ptr = 0;  

         
   return Error;
 
}/* END OF LLF_PKI_KG_X931_LucasPrimeTest */                                           


/*************************************************************************************/
/**
 * @brief This function is used to fined a valid prime1 (101 bits first stage prime) 
 *        for the Key Gen under the X931 standard.
 *
 * @param[in] LenID  - RegsSizesTable entry, used for setting exact prime size in this function.
 * @param[in] testsCount         - Count of Miller-Rabin tests. If testsCount = 0,
 *                                 then count of tests will be set automatically
 * @param[in,out] rP - The virtual pointer to the prime buff ( assumption its size is 101 bits ).
 * @param[in] regTemps_ptr - pointer to temp PKA registers list (5 single registers).
 * @param[in] tempsCount - count of temp registers in the list. 
 * @param[in] temp_ptr  temp buffer of size 2 max RSA buffer size.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * assumptions : This function supports a fixed size of 101 bits as required in the standard.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

static CRYSError_t LLF_PKI_KG_X931_FindPrime1(
                                      DxInt8_t     rP,
                                      DxUint32_t   testsCount,
                                      DxInt8_t    *regTemps_ptr,
                                      DxUint32_t   tempsCount /*5*/, 
                                      DxUint32_t  *temp_ptr, 
                                      DxUint32_t   VirtualHwBaseAddr )
{ 
  /* FUNCTION DECLARATIONS */
  
  /* The error identifier */  	 
  CRYSError_t Error;

  /* the reminder and prime product virtual pointers */
  DxInt8_t rRem, rPrimeProduct;
  
  /* temp register */
  DxInt8_t r0,rT, rT1, rT2;
  
  /* temp physical address of PKA registers*/
  DxUint32_t curAddr;
  
  /* loop variables */
  DxUint32_t i, c, d;
  
  /* the rabin miller success code */
  DxInt8_t SuccessCode;
    
  /* internal variables */
  DxUint32_t r, oneWord; 
  

  /* FUNCTION LOGIC */
  
  /* for prventing compiler warnings, because this 
     parameter currently is not used */
  testsCount = testsCount;
  
  /* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
  if( tempsCount < 5 )   
     return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;  
#else
  tempsCount = tempsCount;
#endif
      
    
  /* ...................... initialize local variables ............................ */
  /* ------------------------------------------------------------------------------ */
  
  /* initialize the Error to O.K */
  Error = CRYS_OK;
  
  /* allocation of the temp registers */
  rRem = regTemps_ptr[0];          
  rPrimeProduct = regTemps_ptr[1]; 
  rT  = regTemps_ptr[2];
  rT1 = regTemps_ptr[3];
  rT2 = regTemps_ptr[4];
  
  r0 = 0;
  
  /*  clearing the extended temp registers  */ 
  LLF_PKI_PKA_Clear( LenIDpq+1, rRem/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  LLF_PKI_PKA_Clear( LenIDpq+1, rPrimeProduct/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  LLF_PKI_PKA_Clear( LenIDpq+1, rT/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  
  /* set the LSB of the prime to insure it is an odd number: rP_ptr[0] |= 1 */
  LLF_PKI_PKA_Set0( LenIDpq/*LenID*/, rP/*OpA*/, rP/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
  
  /* ..................... calculating the prime reminder ................ */
  /* --------------------------------------------------------------------- */
   
  for(i = 0; i < primeProdNum ; ++i)
  {		
	/* load the current prime product into PKA register */
	LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );	
	LLF_PKI_PKA_GetRegAddress( rPrimeProduct, curAddr, VirtualHwBaseAddr );
	LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , curAddr , g_PrimeProduct[i] );
	
	/* copy rP=>rT and calculate the reminder */
	LLF_PKI_PKA_Copy( LenIDpq+1, rT/*dest*/, rP/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	Error = LLF_PKI_PKA_Div( LenIDpq, rT/*OpA*/, rPrimeProduct/*OpB*/, rT1/*resNotUsed*/, 0/*Tag*/, VirtualHwBaseAddr );
	if( Error != CRYS_OK )    
		return Error;                    
                
	/* read result rT word[0] and load it into reminder word[i] */
	/*----------------------------------------------------------*/
	
	LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );    
	LLF_PKI_PKA_GetRegAddress( rT, curAddr, VirtualHwBaseAddr );
	LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr, curAddr, oneWord );    
	
	/* load rem[i] = rT[0] */
	LLF_PKI_PKA_GetRegAddress( rRem, curAddr, VirtualHwBaseAddr );
	LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr, curAddr + 4*i, oneWord );
	
	PLAT_LOG_DEV_PRINT(( DebPrint , "reminder[%d]= %08X \n", i, oneWord ));
  }/* end of loop for calculating the reminders */ 
       

  /* .................... the main loop for finding a prime ............. */
  /* -------------------------------------------------------------------- */


  for( d = 0; ; d += 2)
  {
        PLAT_LOG_DEV_PRINT(( DebPrint , "Correction of remainder: d= %d \n", d));
        
		LLF_PKI_PKA_Clear( LenIDpq+1, rT/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );    
	    
	    /* ................. finding a candidate for a prime ................ */
       
		LLF_PKI_PKA_GetRegAddress( rRem, curAddr, VirtualHwBaseAddr );
	   
	    for( c = 0, i = 0; i < primeProdNum; ++i)
	    {
	      /* read rRem[i] word into oneWord buffer */
	      LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , curAddr + 4*i , oneWord );
	      
	      if(oneWord + d < d)    /* remark: [*] */
	      {
		      oneWord -= g_PrimeProduct[i];
		      /* load result into reminder rRem[i] */
		      LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , curAddr + 4*i , oneWord );
	      }
	      
	      r = oneWord + d;
	      
          PLAT_LOG_DEV_PRINT(( DebPrint , "i= %d c= %X reminder[%d]= %08X r= %08X\n", i, c, i, oneWord, r ));
	      
	      for(; c < g_LastProductPrime[i]; ++c)
	      {
	        PLAT_LOG_DEV_PRINT(( DebPrint , "Check r: c=%d small_prime= %08X (r mod small)= %08X \n", c, g_SmallPrime[c], r % g_SmallPrime[c])); 
	        if(r % g_SmallPrime[c] == 0)
	          goto Next_d;
	      }    
	    }
	     
        /* calculate P = P + d.  */
	    /* load d into register rT. Note: rT already cleared, except the LS word  */    
	    LLF_PKI_PKA_GetRegAddress( rT, curAddr, VirtualHwBaseAddr );
	    LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , curAddr, d );        
	    LLF_PKI_PKA_Add( LenIDpq, rP/*OpA*/, rT/*OpB*/, rP/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	    

	    /*  initialization of modular operations  */
	    /*----------------------------------------*/
	    
	    /* copy P into modulus register r0 */
	    if( rP != r0 )	  
	        LLF_PKI_PKA_Copy( LenIDpq+1, r0/*dst*/, rP/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	    
	    /* initialization of modular operations */
	    LLF_PKI_PKA_ExecCalcNpAndInitModOp( LenID101, LLF_PKI_KG_X931_PRIME1_SIZE_IN_BITS/*101*/, 
	                                        rT, rT1, rT2, VirtualHwBaseAddr );

	    /*  executing the Miller-Rabin test   */
	    /*------------------------------------*/
	    
	    Error = LLF_PKI_KG_X931_MillerRabinTest( 
	                                        LenID101,
	                                        LLF_PKI_KG_X931_PRIME1_SIZE_IN_BITS,
	                                        &SuccessCode, /*out*/
	                                        0, /*CountTests -  shall be set automatically */ 
	                                        rT, rT1, rT2,  /*temp registers*/
	                                        temp_ptr, 
	                                        VirtualHwBaseAddr );                                            
	    if( Error != CRYS_OK )
	    	return Error;	
	    
	    /* on sucess return CRYS_OK we have found a prime */  
	    if( SuccessCode == DX_TRUE )   
	    	return CRYS_OK;	
	    
	    /* update d and reminder to avoid overflow of d (unlikely event) */
	    /*---------------------------------------------------------------*/
	    
	    LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
	    LLF_PKI_PKA_GetRegAddress( rRem, curAddr, VirtualHwBaseAddr );
	 
	    for( i = 0; i < primeProdNum; ++i)
	    {
	   		LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr, curAddr+4*i, oneWord );
	   		 
	   		oneWord += d; /* remark: since [*] passed, there is no need to recheck */
	   		
	   		LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , curAddr+4*i, oneWord );
	   		
	   		PLAT_LOG_DEV_PRINT(( DebPrint , "Update d and Rem: i= %d Rem= %08X \n", i, oneWord ));	
	    }      

	    d = 0;

	    Next_d:
	     
		
	    continue;
  
  }/* end of main loop for finding a prime */
 
   
}/* END OF LLF_PKI_KG_X931_FindPrime1 */


/*************************************************************************************/
/**
 * @brief This function is used to find a valid prime2 (second stage prime) for the 
 *        Key Gen under the X9.31 standard .
 *
 * @param[in,out] rP - The virtual pointer to the prime P (P or Q in RSA).
 * @param[in] rDelta - Te virtual pointer to the delta factor.
 * @param[in] rE - a virtual pointer to public exponent.
 * @param[in] PSizeInBits - size of prime to be generated.
 * @param[in] ESizeInBits - size of exponent. 
 * @param[in] testCount   - count of Rabin-miller tests to perform.
 * @param[in] regTemps_ptr - pointer to temp PKA registers list.
 * @param[in] tempsCount - count of temp registers in the list (6 single registers). 
 * @param[in] temp_ptr  temp buffer of size 2 max RSA buffer size.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * assumptions : This function supports a fixed size of 101 bits as required in the standard.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

static CRYSError_t LLF_PKI_KG_X931_FindPrime2(
                                 DxInt8_t    rP,
                                 DxInt8_t    rDelta,
                                 DxInt8_t    rE,
                                 DxUint32_t  PSizeInBits,
                                 DxUint32_t  ESizeInBits,
                                 DxUint32_t  testsCount,
                                 DxInt8_t   *regTemps_ptr,
                                 DxUint32_t  tempsCount, 
                                 DxUint32_t *temp_ptr,
                                 DxUint32_t  VirtualHwBaseAddr )
{

  /* FUNCTION DECLARATIONS */

  /* error identifier */
  CRYSError_t Error;
  
  /* the reminder and prime product virtual pointers */
  DxInt8_t rRem, rRdelta, rPrimeProduct;
  
  /* temp register */
  DxInt8_t rT, rT1, rT2;
  
  /* current register address */
  DxUint32_t curAddr, curAddr1;
  
  /* Lucas and Rabin-Miller tests result */
  DxInt8_t SuccessCode;
 
  /* loop variables */
  DxUint32_t i, c;
  
  /* PKA status */
  DxUint32_t status;
  
  DxUint32_t oneWord, oneWord1; /* temp variables */
  
#define DebPrint1 5


  /* FUNCTION LOGIC */
  
  /* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
  if( tempsCount < 6 )   
     return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;  
#endif

  
  /* ............................ initialize local variables ............................ */
  /* ------------------------------------------------------------------------------------ */
  ESizeInBits =ESizeInBits;
  /* init the error to success */
  Error = CRYS_OK;
  
    
  /* allocation of the reminder and product on temp registers */
  rRem = regTemps_ptr[0];
  rRdelta = regTemps_ptr[1];
  rPrimeProduct = regTemps_ptr[2]; 
  rT  = regTemps_ptr[3];
  rT1 = regTemps_ptr[4];
  rT2 = regTemps_ptr[5];
  
  
  /* ..................... calculating the prime reminder ................ */
  /* --------------------------------------------------------------------- */
  
  /*  clearing the temp registers (extended) */ 
  LLF_PKI_PKA_Clear( LenIDpq+1, rRem/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  LLF_PKI_PKA_Clear( LenIDpq+1, rPrimeProduct/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );
  LLF_PKI_PKA_Clear( LenIDpq+1, rRdelta/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  LLF_PKI_PKA_Clear( LenIDpq+1, rT/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr ); 

  /* ................... calculate Rdelta and the Reminder ................ */
  /* ---------------------------------------------------------------------- */
 
  /* if the prime candidate P is even add the delta */
  LLF_PKI_PKA_Tst0(LenIDpq, rP/*OpA*/,0/*Tag*/, VirtualHwBaseAddr);
  LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );
    
  if( status == 1 )
       LLF_PKI_PKA_Add( LenIDpq/*LenID*/, rP/*OpA*/, rDelta/*OpB*/, rP/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
  
  /* multiply delta by 2 */
  LLF_PKI_PKA_Add( LenIDpq/*LenID*/, rDelta/*OpA*/, rDelta/*OpB*/, rDelta/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
  
  PLAT_LOG_DEV_PRINT(( DebPrint1 , " In func find_next_prime_2: \n"));
     
 
  /* ........loop for calculating the products .......*/
  /*--------------------------------------------------*/
  
  for(i = 0; i < primeProdNum ; ++i)
  {		
	/* load the current rPrimeProduct[0] = g_PrimeProduct[i] */
	LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );    
	LLF_PKI_PKA_GetRegAddress( rPrimeProduct, curAddr, VirtualHwBaseAddr );
	LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr, curAddr, g_PrimeProduct[i] );    
	
	/* copy rP=>rT and calculate the reminder in reg rT */
	LLF_PKI_PKA_Copy( LenIDpq+1, rT/*dest*/, rP/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Div( LenIDpq, rT/*OpA*/, rPrimeProduct/*OpB*/, rT1/*ResNotUsed*/, 0/*Tag*/, VirtualHwBaseAddr );
			
	/* load the next word of reminder: rRem[i] = rT[0] */
	LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr ); 
	LLF_PKI_PKA_GetRegAddress( rT, curAddr, VirtualHwBaseAddr ); 
	LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr, curAddr, oneWord ); /* read rT[0]*/    
	LLF_PKI_PKA_GetRegAddress( rRem, curAddr, VirtualHwBaseAddr ); 
	LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr, curAddr + 4*i, oneWord ); /*Rem[i] = rT[0]*/  
	
	PLAT_LOG_DEV_PRINT(( DebPrint1 , "i= %d  reminder[i]= 0x%08X \n",i, oneWord ));
	
	/* calculate the Rdelta */
	LLF_PKI_PKA_Copy( LenIDpq+1, rT/*dest*/, rDelta/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Div( LenIDpq, rT/*OpA*/, rPrimeProduct/*OpB*/, rT1/*ResNotUsed*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* load the Rdelta with the result rRdeltam[i] = rT[0]*/
	LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr ); 
	LLF_PKI_PKA_GetRegAddress( rT, curAddr, VirtualHwBaseAddr ); /* read rT[0]*/
	LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr, curAddr, oneWord );     
	LLF_PKI_PKA_GetRegAddress( rRdelta, curAddr, VirtualHwBaseAddr ); 
	LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr, curAddr + 4*i, oneWord ); /*Rdelta[i] = rT[0]*/  
	
	PLAT_LOG_DEV_PRINT(( DebPrint , " rRdelta[i]= 0x%08X\n", oneWord ));
	
	
  }/* end of loop for calculating the reminders */ 

  /* ------------------------------------------------------------------------ */
  /* ..................... main loop for finding the prime .................. */
  /* ------------------------------------------------------------------------ */
  
  while(1)
  {  
	//WATCH_DOG_RESET(); // obsolete. Watchdog should be fed by dedicated task
	
	/* checking if the current prime should be tested */
	
	LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GetRegAddress( rRem, curAddr, VirtualHwBaseAddr );
	
	for( c=0 , i=0 ; i < primeProdNum ; i++ )
	{
		for( ; c < g_LastProductPrime[i] ; c++ )
		{
			LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr, curAddr + 4*i, oneWord );    
			
			if( (oneWord % g_SmallPrime[c]) == 0 )
			{
				PLAT_LOG_DEV_PRINT(( DebPrint1 , "goto NextPrime from check1; c= %d\n", c));
				goto NextPrime;
			}           
		}      
	}
  
	/* .......... execute rT = GCD(e,P-1) ............. */
	/*--------------------------------------------------*/
	 
	LLF_PKI_PKA_SubIm( LenIDpq, rP/*OpA*/, 1/*imm*/,  rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); /* rP= rP-1 */
	LLF_PKI_PKA_Copy( LenIDpq+1, 0/*dest*/, rE/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* rT = GCD */
	Error = LLF_PKI_PKA_ModInv( LenIDpq, rT/*OpA*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	if( Error != CRYS_OK )     
		return Error;
		  
	/* if the GCD != 1, go to the next prime */
	LLF_PKI_PKA_SubIm( LenIDpq, rT/*OpA*/, 1/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );    
	if( status != 1 )        
	{
		PLAT_LOG_DEV_PRINT(( DebPrint1 , "goto NextPrime from check2 GCD\n" ));
		goto NextPrime;
	}
	
	/*  initialization of modular operations for modulus P */
	/*-----------------------------------------------------*/
	    
	/* reset modulus in register r0 = rP */
	LLF_PKI_PKA_Copy( LenIDpq+1, 0/*dst*/, rP/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	    
	/* initialization of modular operations */
	LLF_PKI_PKA_ExecCalcNpAndInitModOp( LenIDpq, PSizeInBits, rT, rT1, rT2, VirtualHwBaseAddr );
	
	
	/* .........  perform primality tests   ............ */
	/*---------------------------------------------------*/
	
	/* init lhe test flag to FALSE */
	SuccessCode = DX_FALSE;
	
	/* execute the Miller-Rabin test */ 
	Error = LLF_PKI_KG_X931_MillerRabinTest( 
					LenIDpq,
					PSizeInBits,
					&SuccessCode,    /*out*/
					0, /*countTests -  shall be set automatically */ 
					rT, rT1, rT2,    /*temp registers*/
					temp_ptr,
					VirtualHwBaseAddr );                                            
	if( Error != CRYS_OK )     
		return Error;                                       
	
	/* if the previous test succeeded, execute the Lucas test */
	if( SuccessCode == DX_TRUE )
	{ 
		Error = LLF_PKI_KG_X931_LucasPrimeTest(
					    LenIDpq,
					    &SuccessCode,     /*out*/
					    regTemps_ptr + 3, /*temp registers list*/   
					    tempsCount-3,             
					    VirtualHwBaseAddr );                                              
	if( Error != CRYS_OK )     
		return Error; 
	}
	
	/* if both tests are passed, exit - we have finded a prime */
	if( SuccessCode == DX_TRUE ) 
		return CRYS_OK;	
	
	PLAT_LOG_DEV_PRINT(( 20 , "goto NextPrime from PrimeTests\n" )); 
	
	/*--------------------------------------------*/  
	/*    finding the next prime candidate        */ 
	/*--------------------------------------------*/      
	NextPrime: 
	
	/* updating of remainders Rem[i] */
	
	LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GetRegAddress( rRem, curAddr, VirtualHwBaseAddr );
	LLF_PKI_PKA_GetRegAddress( rRdelta, curAddr1, VirtualHwBaseAddr );
	
	for( i = 0 ; i < primeProdNum ; i++ ){                                     
		LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr, curAddr + 4*i, oneWord ); /*Rem[i]*/   
		LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr, curAddr1 + 4*i, oneWord1 ); /*rDelta[i]*/   
		
		oneWord += oneWord1;
		if( oneWord < oneWord1 )
			oneWord -= g_PrimeProduct[i];
		
		LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr, curAddr + 4*i, oneWord ); /*Rem[i]*/   
		
		PLAT_LOG_DEV_PRINT(( DebPrint1 , "Updating: i= %d reminder[i]= 0x%08X\n ",i, oneWord ));
	}     
	
	/* the new prime candidate: P = P + Delta */
	LLF_PKI_PKA_Add( LenIDpq, rP/*OpA*/, rDelta/*OpB*/, rP/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
  }/* end of searching for a prime loop */
    
}/* END OF LLF_PKI_KG_X931_FindPrime2 */                                       


/*************************************************************************************/
/**
 * @brief This function is used to find a valid prime for the Key Gen under
 *        the X931 standard .
 *
 *     Assumes: - the PKA is initialized on default mode according to PSizeInBits,
 *              - the PSizeInBits is set into LenID RegsSizesTable,
 *
 * @param[in] P1,P2 - The virtual pointers to the prime1, prime2 numbers 
 *            ( assumption their sizes are 101 bits ). 
 * @param[in/out] prime_ptr - The pointer to the prime.
 * @param[in] PSizeInBits - size of the prime.
 * @param[in] rE - The virtual pointer to public exponent.
 * @param[in] eSizeInBits - size of public exponent.
 * @param[in] testCount   - count of Rabin-miller tests to perform.
 * @param[in] regTemps_ptr - pointer to temp PKA registers list (7 single registers).
 * @param[in] tempsCount - count of temp registers in the list. 
 * @param[in] temp_ptr  temp buffer of size 2 max RSA buffer size.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * assumptions : This function supports a fixed size of 101 bits as required in the standard.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

static CRYSError_t LLF_PKI_KG_X931_FindX931Prime(
                                          DxInt8_t    rP1/**p1_ptr*/, 
                                          DxInt8_t    rP2/**p2_ptr*/, 
                                          DxInt8_t    rP /**prime_ptr*/, 
                                          DxUint32_t  PSizeInBits,
                                          DxUint32_t  rE /*e_ptr*/, 
                                          DxUint32_t  ESizeInBits,
                                          DxUint32_t  testsCount,
                                          DxInt8_t   *regTemps_ptr,
                                          DxUint32_t  tempsCount,
                                          DxUint32_t *temp_ptr, 
                                          DxUint32_t  VirtualHwBaseAddr ) 
{
  /* FUNCTION LOCAL DECLARATIONS */
  
  /* the Error identifier */
  CRYSError_t Error = CRYS_OK;
  
  /* virtual pointers to PKA registers */
  DxInt8_t    rP12, rPmodP12, rR1, rR2;
  
  /* virtual pointers to temp PKA registers */
  DxInt8_t rT1;    

  /* PKA status */
  DxUint32_t status, flag;
  
 
  /* FUNCTION LOGIC */
  

  /* ............... init local variables .............................. */
  /* ------------------------------------------------------------------- */
  
  /* allocation of registers */
  rP12 = regTemps_ptr[0];
  rPmodP12 = regTemps_ptr[1];
  rR1 = regTemps_ptr[2];
  rR2 = regTemps_ptr[3];
  rT1 = regTemps_ptr[4];

  /* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
  if( tempsCount < 5 )   
     return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;  
#endif

  /* ...... find the first primes P1, P2  of size 101 bit .............. */
  /* ------------------------------------------------------------------- */
  /* p1 */ 
  Error = LLF_PKI_KG_X931_FindPrime1( rP1, testsCount,
                                      regTemps_ptr, tempsCount, 
                                      temp_ptr, 
                                      VirtualHwBaseAddr );
  if( Error != CRYS_OK )
    return Error;

  /* p2 */       
  Error = LLF_PKI_KG_X931_FindPrime1( rP2, testsCount,
                                      regTemps_ptr, tempsCount, 
                                      temp_ptr, 
                                      VirtualHwBaseAddr );
  if( Error != CRYS_OK )
    return Error;

/* Debug */
#ifdef LLF_PKI_PKA_DEBUG
  LLF_PKI_PKA_Copy(LenIDpq+1, rP1/*dst*/, rP1/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
  LLF_PKI_PKA_Copy(LenIDpq+1, rP2/*dst*/, rP2/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
#endif

  /*  find P12 = P1*P2 , pModP12 = P mod P12 (operations size from LenID) */
  /*     Note: PSizeInBits must be set into LenID entry                   */ 
  /* -------------------------------------------------------------------- */
  
   
  /* P12 = P1 * P2 */
  LLF_PKI_PKA_LMul( LenIDpq, rP1/*OpA*/, rP2/*OpB*/, rP12/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
                                        
  /* PmodP12 = P mod P12 */
  LLF_PKI_PKA_Copy(LenIDpq+1, rPmodP12/*dst*/, rP/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
  LLF_PKI_PKA_Div( LenIDpq, rPmodP12/*OpA*/, rP12/*OpB*/, rT1/*ResNotUsed*/, 0/*Tag*/, VirtualHwBaseAddr );


  /* find; R1= (1/P2 mod P1)*P2 - (1/P1 mod P2)*P1; R2= ... similary   .. */
  /* -------------------------------------------------------------------- */

  /* ....... calculate R1 = (1/P2 mod P1)*P2 ..... */
  LLF_PKI_PKA_Copy(LenIDpq+1, 0/*mod reg*/, rP1/*src*/, 0/*Tag*/, VirtualHwBaseAddr ); /*set modulus P1*/ 
  LLF_PKI_PKA_Clear(LenIDpq+1, rT1/*dst*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  LLF_PKI_PKA_Copy(LenIDpq+1, rT1/*dst*/, rP2/*src*/, 0/*Tag*/, VirtualHwBaseAddr ); /* P1=>rT1 */
  
  /* if P1 > P2 set flag = 1, else flag = 0 */
  LLF_PKI_PKA_Sub(LenIDpq, rP2/*OpA*/, rP1/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr );
  LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status);
  if( status == 0 )
      flag = 1;
  else
  {
  	  /* set rT1 = P2 mod P1 = rT1 - rP1 */
  	  flag = 0;
	  LLF_PKI_PKA_Sub(LenIDpq, rT1/*OpA*/, rP1/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
  }

  /* R1 = (1/P2 mod P1) */
  Error = LLF_PKI_PKA_ModInv(LenIDpq, rT1/*OpB*/, rR1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
  if( Error != CRYS_OK )
      return Error; 
   
  LLF_PKI_PKA_LMul( LenIDpq, rR1/*OpA*/, rP2/*OpB*/, rR1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); 

  /* ....... calculate R2 = (1/P1 mod P2)*P1 ..... */
  LLF_PKI_PKA_Copy(LenIDpq+1, 0/*mod reg*/, rP2/*src*/, 0/*Tag*/, VirtualHwBaseAddr ); /*set modulus P2*/ 
  LLF_PKI_PKA_Clear(LenIDpq+1, rT1/*dst*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  LLF_PKI_PKA_Copy(LenIDpq+1, rT1/*dst*/, rP1/*src*/, 0/*Tag*/, VirtualHwBaseAddr ); /* P1=>rT1 */
  
  /* if flag == 1, i.e. P2 >= P1, then set rT1 = P1 mod P2 = P1 - P2 */
  if( flag == 1 )
	  LLF_PKI_PKA_Sub(LenIDpq, rT1/*OpA*/, rP2/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
  
  Error = LLF_PKI_PKA_ModInv(LenIDpq, rT1/*OpB*/, rR2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
  if( Error != CRYS_OK )
      return Error;
    
  LLF_PKI_PKA_LMul( LenIDpq, rR2/*OpA*/, rP1/*OpB*/, rR2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); 


  /* R=R1-R2; if(R <0) R= R+P12; */
  LLF_PKI_PKA_Sub(LenIDpq, rR1/*OpA*/, rR2/*OpB*/, rR1/*res*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status);
  if( status == 0 ) 
      LLF_PKI_PKA_Add(LenIDpq, rR1/*OpA*/, rP12/*OpB*/, rR1/*res*/, 0/*Tag*/, VirtualHwBaseAddr );
  	
  /* R=R-PmodP12; if(R<0) R=R+P12; */
  LLF_PKI_PKA_Sub(LenIDpq, rR1/*OpA*/, rPmodP12/*OpB*/, rR1/*res*/, 0/*Tag*/, VirtualHwBaseAddr ); 
  LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status);
  if( status == 0 ) 
      LLF_PKI_PKA_Add(LenIDpq, rR1/*OpA*/, rP12/*OpB*/, rR1/*res*/, 0/*Tag*/, VirtualHwBaseAddr );

  /* add P = P + R */
  LLF_PKI_PKA_Add(LenIDpq, rP/*OpA*/, rR1/*OpB*/, rP/*res*/, 0/*Tag*/, VirtualHwBaseAddr ); 

  /* find the prime P */  
  Error = LLF_PKI_KG_X931_FindPrime2( rP, rP12/*rDelta*/, rE,
                                      PSizeInBits, ESizeInBits,
                                      testsCount,
                                      regTemps_ptr + 1, tempsCount-1, 
                                      temp_ptr, 
                                      VirtualHwBaseAddr );

  return Error;                                      

}/* END OF LLF_PKI_KG_X931_FindX931Prime */



/*************************************************************************************/
/** 
 * @brief The function checks primality of big number relating to set of small prime numbers.
 *
 *   Notes:  - 3 PKA registers used: rP, rModRes, rSmallNum,
 *           - the PKA must be initialized according tp P size,
 *           - LenID+1 entry containing the extended register size.
 *
 * @param[in] LenID     - The SizesTable entry, containing the exact P size in bits.
 * @param[in] rP        - The virtual pointer to big number P register to be checked.
 * @param[in] rModRes   - The virtual pointer to temp register.
 * @param[in] rSmallNum - The virtual pointer to temp register.
 * @param[in] rT        - The virtual pointer to temp register.
 *
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code.
 *
 */
 static DxInt32_t LLF_PKI_quickPrimeTest( 
		                          DxUint8_t    LenID,              /*in */
		                          DxInt8_t     rP,                 /*in */
		                          DxInt8_t     rModRes,         /*temp reg*/
		                          DxInt8_t     rSmallPrime,     /*temp reg*/
		                          DxInt8_t     rT,              /*temp reg*/
		                          DxUint32_t   divCount,           /*in*/
		                          DxUint32_t   VirtualHwBaseAddr ) /*in */
 {
   /* FUNCTION DECLARATIONS */
 
   /* loop variable */
   DxUint32_t i;
   /* PKA status */
   DxUint32_t status;
   
   /* current address */
   DxUint32_t curAddr;
   
   	  
   /* FUNCTION  LOGIC */
   
   /* Initializations   */

   /* set pointer smallPrime_ptr to PKA register low word */
   /* clear rSmallPrime register (with extension) */
   LLF_PKI_PKA_Clear( LenID+1, rSmallPrime/*OpA*/, 0/*Tag*/,VirtualHwBaseAddr ); 
   
   /*-----------------------------------------------*/
   /* Check primality by dividing P by small primes */
   /*-----------------------------------------------*/
   
   for( i = 0; i < divCount; i++)
   {
        /* copy rP into rModReg for dividing */
        LLF_PKI_PKA_Copy( LenID+1, rModRes/*dst*/, rP/*src*/, 0/*Tag*/, VirtualHwBaseAddr);
 
        /* set the next small prime into PKA register */
        LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GetRegAddress( rSmallPrime, curAddr, VirtualHwBaseAddr );
        LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr, curAddr, g_SmallPrime[i] );
        
        /* calculate remainder: rModReg = rP % smallPrime */
        LLF_PKI_PKA_Div( LenID, rModRes/*OpA*/, rSmallPrime/*OpB*/, rT/*ResNotUsed*/, 0/*Tag*/, VirtualHwBaseAddr);
        
        /* check is the remainder equaled to 0 by add operation */
        LLF_PKI_PKA_AddIm( LenID, rModRes/*OpA*/, 0/*OpB*/, RES_DISCARD/*discard Res*/, 0/*Tag*/, VirtualHwBaseAddr);
        
        LLF_PKI_PKA_GET_StatusAluOutZero( VirtualHwBaseAddr, status );
    
        if(status)   
        	return DX_FALSE;
    }
 
   /* End - return SUCCESS */
   
   return DX_TRUE;

 	
} /* End of LLF_PKI_quickPrimeTest() */



/*************************************************************************************/
/*                  Public functions                                                 */
/*************************************************************************************/

/*************************************************************************************/
/**
 * @brief This function is used to fined a valid prime for the Key Gen.
 *
 * @param[in,out] rP - The virtual pointer to the prime P register.
 * @param[in] PSizeInBits - The prime size in bits.
 * @param[in] rE - The virtual pointer to the public exponent register.
 * @param[in] ESizeInBits - The public exponent size in bits.
 * @param[in] testsCount         - Count of Miller-Rabin tests. If testsCount = 0,
 *                                 then count of tests will be set automatically
 * @param[in] regTemps_ptr - pointer to temp PKA registers list (9 single registers
 *            of size according to PSizeInBits ).
 * @param[in] tempsCount - count of temp registers in the list (9). 
 * @param[in] temp_ptr  temp buffer of size 2 max RSA buffer size.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * assumptions : .  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_PKI_KG_FindPrime(
                                 DxInt8_t    rP, 
                                 DxUint32_t  PSizeInBits,
                                 DxInt8_t    rE, 
                                 DxUint32_t  ESizeInBits,
                                 DxUint32_t  testsCount,
                                 DxInt8_t   *regTemps_ptr,
                                 DxUint32_t  tempsCount, 
                                 DxUint32_t *temp_ptr,
                                 DxUint32_t  VirtualHwBaseAddr)
{                                 
                     
   /* LOCAL DECLARATIONS */
 
   /* the Error identifier */
   CRYSError_t Error = CRYS_OK;
   
   /* virtual pointers to PKA data registers */
   DxInt8_t rP1, rP2;
   

   /* temp buffer for 100 bits random number / big end 1E05B8F18D807DB7A47EF53567 
      nearest random is: 1E 05 B8 F1 8D 80 7D B7 A4 7E F5 35 99 */
   DxUint32_t  rBuff[4];

   
   /* FUNCTION LOGIC */
   
   
#ifdef LLF_PKI_PKA_DEBUG 
   if( tempsCount < 2 )
       return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#endif     
   
   /* allocate virtual pointers on temp registers */
   rP1 = regTemps_ptr[0];
   rP2 = regTemps_ptr[1];

#if defined RSA_KG_NO_RND 
  #ifdef LLF_PKI_PKA_DEBUG 
	rBuff[3] = 0;	
	if( PQindex == 0 )
		DX_VOS_FastMemCpy( rBuff, rBuff1, LLF_PKI_KG_X931_PRIME1_SIZE_IN_8BIT_WORDS); /*for P*/
	else 
		DX_VOS_FastMemCpy( rBuff, rBuff3, LLF_PKI_KG_X931_PRIME1_SIZE_IN_8BIT_WORDS); /*for Q*/

	#ifdef BIG__ENDIAN
	CRYS_COMMON_INVERSE_UINT32_IN_ARRAY(rBuff, LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS);
	#endif
  #endif

#else 
 
   /* get a random number of P1 /101 bit/   */
   /*---------------------------------------*/
   Error = CRYS_RND_GenerateVector(LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS*sizeof(DxUint32_t), (DxUint8_t*)rBuff );
   if( Error != CRYS_OK )   
     return Error;
   
#endif    
   
   /* clear the 3 LSBits - anD set the 4 LSB - the prime is 101 bits */
   rBuff[LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS-1] &= 0x0000001FUL;
   rBuff[LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS-1] |= 0x00000010UL; 
   /* set LSBit = 1 to ensure the odd number */
   rBuff[0] |= 1UL; 

#ifdef LLF_PKI_PKA_DEBUG
#if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)

   if(PQindex == 0){
   	  P1R_ptr = P1pR; P2R_ptr = P2pR;
   	  P1Pr_ptr = P1pPr; P2Pr_ptr = P2pPr;
   	  PQ_ptr = rBuffP;
   }
   else{
   	  P1R_ptr = P1qR; P2R_ptr = P2qR;
   	  P1Pr_ptr = P1qPr; P2Pr_ptr = P2qPr;
   	  PQ_ptr = rBuffQ;
   }
   
   DX_VOS_FastMemCpy(P1R_ptr, rBuff, LLF_PKI_KG_X931_PRIME1_SIZE_IN_8BIT_WORDS); /*for P*/
   
#endif   
#endif   
   
   /* copy random number into PKA register rP1 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( rP1/*dstReg*/, LenIDpq+1, rBuff/*src_ptr*/, 
                      LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS, VirtualHwBaseAddr ); 

#ifdef RSA_KG_NO_RND
  #ifdef LLF_PKI_PKA_DEBUG 
	rBuff[3] = 0;
	if( PQindex == 0 )   
		DX_VOS_FastMemCpy( rBuff, rBuff2, LLF_PKI_KG_X931_PRIME1_SIZE_IN_8BIT_WORDS); /*for P*/
	else
		DX_VOS_FastMemCpy( rBuff, rBuff4, LLF_PKI_KG_X931_PRIME1_SIZE_IN_8BIT_WORDS); /*for Q*/

	#ifdef BIG__ENDIAN
	CRYS_COMMON_INVERSE_UINT32_IN_ARRAY( rBuff, LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS );
	#endif
  #endif
  	    
#else

   /* get a random number of P2  /101 bit/  */
   /*---------------------------------------*/
   Error = CRYS_RND_GenerateVector(LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS*sizeof(DxUint32_t), (DxUint8_t*)rBuff );
   if( Error != CRYS_OK )   
     return Error;
   
#endif 
 
   /* clear the 3 MSBits - anD set the 4 MSBit - the prime is 101 bits */
   rBuff[LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS-1] &= 0x0000001FUL;
   rBuff[LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS-1] |= 0x00000010UL; 
   /* set LSBit = 1 to ensure the odd number */
   rBuff[0] |= 1UL; 

/*  Debug  */
#ifdef LLF_PKI_PKA_DEBUG 
#if defined RSA_KG_FIND_BAD_RND
   DX_VOS_FastMemCpy( P2R_ptr, rBuff, LLF_PKI_KG_X931_PRIME1_SIZE_IN_8BIT_WORDS); /*for P*/
#endif   
#endif   
   
   /* copy random number P2 into PKA register rP2 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( rP2/*dstReg*/, LenIDpq+1, rBuff/*src_ptr*/, 
                      LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS, VirtualHwBaseAddr ); 



   /*           find the primes P1,P2, P              */
   /*-------------------------------------------------*/
   Error = LLF_PKI_KG_X931_FindX931Prime( 
                                          rP1, rP2, 
                                          rP /*prime*/, PSizeInBits,
                                          rE /*exp*/, ESizeInBits,
                                          testsCount,                                          
                                          regTemps_ptr + 2, tempsCount-2, 
                                          temp_ptr,
                                          VirtualHwBaseAddr);
   /* clear temp buffers */
   DX_VOS_MemSetZero(rBuff, sizeof(rBuff));

/* Debug */                                            
#ifdef LLF_PKI_PKA_DEBUG 
#if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
   /* save finded results: rP1,rP2,rP for P and Q accordingly */
   LLF_PKI_PKA_CopyDataFromPkaReg( P1Pr_ptr/*dst_ptr*/, LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS, 
                                   rP1/*srcReg*/, VirtualHwBaseAddr );

   LLF_PKI_PKA_CopyDataFromPkaReg( P2Pr_ptr/*dst_ptr*/, LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS, 
                                   rP2/*srcReg*/, VirtualHwBaseAddr );
                                   
   LLF_PKI_PKA_CopyDataFromPkaReg( PQ_ptr/*dst_ptr*/, PSizeInBits/8, rP/*srcReg*/, VirtualHwBaseAddr );
#endif   
#endif   

   return Error;                                      
 
}/* END OF LLF_PKI_KG_FindPrime */  




/*************************************************************************************/
/**
 * @brief This function is used to test a primality of big numbers.
 *
 *        The function performs assigned count of Rabin-Miller tests and one Lucas-Lehmer
 *        test according to testing mode:
 *              - for RSA according to ANSI X9.31 standard. 
 *              - for DH  according to ANSI X9.42 standard. 
 *
 * @param[in] P_ptr           - The pointer to the prime buff.
 * @param[in] sizeWords       - The prime size in words.
 * @param[in] rabinTestsCount - The count of Rabin-Miller tests repetition.
 * @param[in] isPrime         - The flag indicates primality:   
 *                                  if is not prime - DX_FALSE, otherwise - DX_TRUE.  
 * @param[in] primeTestMode - primality testing mode (RSA or DH - defines how are performed some 
 *            operations on temp buffers.
 * @param[in] temp_ptr - temp buffer of size 2*sizeWords.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 *         Assumptions : the max size supported is 2112 bits.
 *         
 *         NOTE:  For using in RSA module  size of each temp buffer must be of minimum size 
 *                of prime number P in words. 
 *                For using in ANSI X9.42 standard (DH,DSA algorithms) size of each temp buffer 
 *                must be of minimum 2 size of prime number P in words.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code.
 *
 */

 CRYSError_t LLF_PKI_primeTest(  
			DxUint32_t *P_ptr,                              
			DxInt32_t   sizeWords, 
			DxInt32_t   rabinTestsCount,
			DxInt8_t   *isPrime_ptr,
			CRYS_RSA_DH_PrimeTestMode_t primeTestMode,
			DxUint32_t *temp_ptr,
			DxUint32_t  VirtualHwBaseAddr )
{
   
     /* FUNCTION DECLARATIONS */
   
   
      /* the Error identifier */
      CRYSError_t Error;
      
      /* virtual pointers to PKA regs*/
      DxInt8_t   rT0, rT1, rT2;
      
      /* registers list 2,3,4,5,6,7,8,9,10 */
      DxInt8_t  regTemps[11] = { 2,3,4,5,6,7,8,9,10,11 };
       
      DxUint32_t   PSizeInBits;
      
      DxUint32_t   divCount;

      /* FUNCTION  LOGIC */
   
      /*         Initializations          */
      /*----------------------------------*/
      
      Error = CRYS_OK;
      
      /* set registers pointers, note: r0=0, r1=1 by default reserved for N and NP */
      rT0=2; rT1=3; rT2=4; /* temp registers */
      
      /* exact size of P */
      PSizeInBits = 32 * sizeWords;
                                                                        
     /*------------------------------------------------------------------------*/
     /* initialize the PKA engine on default mode with size of registers       */
     /* according to operation size = max(Asize,Bsize)                         */
     /*------------------------------------------------------------------------*/
     
      /* initialize the PKA engine on default mode */
      Error = LLF_PKI_PKA_DefaultInitPKA( PSizeInBits, sizeWords+1, VirtualHwBaseAddr );

      if( Error != CRYS_OK )     
         goto End;
      
      /* set modulus into PKA register r0 */
      LLF_PKI_PKA_CopyDataIntoPkaReg( 0/*dstReg*/, 1/*LenID*/, P_ptr/*src_ptr*/, 
                                      sizeWords, VirtualHwBaseAddr );
      
      /* initialize modular operations */
      LLF_PKI_PKA_ExecCalcNpAndInitModOp( 0/*LenID*/, PSizeInBits, rT0, rT1, rT2, VirtualHwBaseAddr );

     /*---------------------------------*/
     /*   execute primality tests       */
     /*---------------------------------*/
     
     /* count of small primes (each one is DxUint16) to check:
         - for DH algorithm use predefined value,
           else use maximal value  */
     
     if(primeTestMode == CRYS_DH_PRIME_TEST_MODE)
     	divCount = LLF_PKI_QUICK_PRIME_TEST_DIVISIONS_COUNT;
     else
     	divCount = sizeof(g_SmallPrime) / sizeof(DxUint16_t);

     
      /* test by small prime numbers */
      *isPrime_ptr = (DxInt8_t)LLF_PKI_quickPrimeTest( 
					       0/*LenID*/, 
					       0 /*prime*/,   
					       rT0, rT1, rT2/*temp regs*/,
					       divCount, 
					       VirtualHwBaseAddr ); 
			   	                                       
       /* the Miller-Rabin test */ 
       if( *isPrime_ptr == DX_TRUE )
       {
	       Error = LLF_PKI_KG_X931_MillerRabinTest(
	                                    0 /*LenID*/,
	                                    PSizeInBits, 
                                            isPrime_ptr, /*out*/
                                            rabinTestsCount, 
                                            rT0, rT1, rT2,  /*temp regs*/
                                            temp_ptr,
                                            VirtualHwBaseAddr  );
	       if( Error != CRYS_OK )
	           goto End;                          
       }
   
       /* the Lucas test  */
       if( *isPrime_ptr == DX_TRUE )
       { 
            Error = LLF_PKI_KG_X931_LucasPrimeTest(
                                            0 /*LenID*/,
                                            isPrime_ptr, /*out*/
                                            regTemps,
                                            7 /*tempsCount*/, 
                                            VirtualHwBaseAddr );
       }
End:
	   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
	   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );

       return Error; 

}/* END OF LLF_PKI_primeTest */   
