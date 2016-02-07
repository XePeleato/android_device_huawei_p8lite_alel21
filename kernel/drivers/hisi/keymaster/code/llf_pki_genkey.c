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



/************* Include Files *******************************************************/

#include "dx_vos_basetypes.h"
#include "crys_rsa_types.h"
#include "crys_common_math.h" 
#include "llf_pki.h" 
#include "llf_pki_error.h"
#include "llf_pki_rsa.h" 
#include "dx_vos_mem.h"
#include "llf_pki_hw_defs.h"
 
/************************ Defines **************************************************/

/* canceling the lint warning:
Info 716: while(1) ... */ 
/*lint --e{716} */


/* temp for debug goals */
#ifdef LLF_PKI_PKA_DEBUG
#if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
extern DxUint8_t PQindex;
#endif
#endif

DxUint32_t g_MemOffsetAddr;

/************************ Enums *****************************************************/

/************************ Typedefs **************************************************/

/************************ Global Data ***********************************************/

/************* Private function prototype *******************************************/

/************************ Public Functions ******************************************/


/*************************************************************************************/
/**
 * @brief The LLF_PKI_genKeyNonCrt generates a public and private RSA keys in NonCRT mode.
 *
 *    The function initializes the PKA according to used registers size, calculates
 *    keys components and finishes PKA operations.
 *           
 *
 * @param[in] e_ptr              - The pointer to the public exponent.
 * @param[in] eSizeInBits        - Size of public exponent in bits.
 * @param[in] n_ptr              - The pointer .
 * @param[in] eSizeInBits        - The public exponent size in bits.
 * @param[in] nLenInBits         - The required size of the key in bits.
 * @param[in] testsCount         - Count of Miller-Rabin tests. If testsCount = 0,
 *                                 then count of tests will be set automatically
 * @param[in]  Success_ptr       - The pointer to the flag of success generation of P,Q.              
 * @param[out] p_ptr             - The pointer to the first factor.
 * @param[out] q_ptr             - The ponter to the second factor.
 * @param[out] n_ptr             - The pointer to the public modulus key.
 * @param[out] d_ptr             - The pointer to the private exponent (non CRT).
 * @param[in]  temp_ptr          - temp buffer of size 2 max RSA buffer size.
 * @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

CRYSError_t LLF_PKI_genKeyNonCrt(
			DxUint32_t *e_ptr,  
			DxUint32_t  eSizeInBits,  
			DxUint32_t  nSizeInBits,
			DxUint32_t  testsCount,            
			DxUint32_t *Success_ptr,
			DxUint32_t *p_ptr,
			DxUint32_t *q_ptr,
			DxUint32_t *n_ptr,  
			DxUint32_t *d_ptr, 
			DxUint32_t *temp_ptr, 
			DxUint32_t  VirtualHwBaseAddr )
{
	/* LOCAL DECLARATIONS */

	CRYSError_t Error=CRYS_OK;


	/* the primes P, Q size */
	DxUint32_t pSizeInBits, pSizeInWords;

	/* virtual pointers to PKA registers of single size */
	DxInt8_t   rE, rP, rQ;

	/* list of maximum 28 allowed temp PKA registers for functions. 
	   Note: numbers 0xFF - for debug goals */
	DxInt8_t regTemps[30] = {0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
	                        0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
	                        0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0xFF,0xFF }; 
	/* virtual pointers to single temp PKA registers */
	DxInt8_t rT1, rT2, rSwap;

	/* PKA status */
	DxUint32_t  status;


	/* FUNCTION LOGIC */

	/* ...................... initialize local variables ............................ */
	/* ------------------------------------------------------------------------------ */

	/* initialize the Error to OK */
	Error = CRYS_OK;

	/* setting the primes P,Q length ; Note: the size of the modulus n is even */
	pSizeInBits = nSizeInBits / 2;
	pSizeInWords = pSizeInBits / 32;

	/* initialize the success to 1 - success */
	*Success_ptr = DX_FALSE;


	/* -------------------------------------------------------------------------------- */
	/* .................. executing the internal key generation ....................... */
	/* -------------------------------------------------------------------------------- */

	/* set virtual registers pointers */
	rE =regTemps[0]; /*2*/
	rP =regTemps[1]; /*3*/ 
	rQ =regTemps[2]; /*4*/ 
	rT1=regTemps[3]; /*5*/ 
	rT2=regTemps[4]; /*6*/ 

	 /**********************************************************************************/
	 /*                     CALCULATIONS WITH SHORT REGISTERS                          */
	 /* init PKA on default mode according to P,Q operation size for creating P and Q. */
	 /*  Note: All PKA memory shall be cleaned, insert nSizeInBits/2 => entry 0,       */
	 /*        nSizeInBits/2+32 => entry 1                                             */
	 /**********************************************************************************/
	 
	 /* set additional sizes into RegsSizesTable: */   
	 LLF_PKI_PKA_SetRegSize(nSizeInBits/2, LenIDpq, VirtualHwBaseAddr); /* for P,Q */
	 LLF_PKI_PKA_SetRegSize(nSizeInBits/2+32, LenIDpq+1, VirtualHwBaseAddr);
	 LLF_PKI_PKA_SetRegSize(LLF_PKI_KG_X931_PRIME1_SIZE_IN_BITS, LenID101, VirtualHwBaseAddr); /* for P1,P2 - 101 bit */
	 LLF_PKI_PKA_SetRegSize(LLF_PKI_KG_X931_PRIME1_SIZE_IN_BITS+32, LenID101+1, VirtualHwBaseAddr);


	 /* inforcing the prime candidates P,Q so the size of they is keySize/2  */
	 /*----------------------------------------------------------------------*/
	 p_ptr[pSizeInWords - 1] |= 0xC0000000;
	 p_ptr[pSizeInWords] = 0;
	 p_ptr[0] |= 0x01; 

	 q_ptr[pSizeInWords - 1] |= 0xC0000000;
	 q_ptr[pSizeInWords] = 0;
	 q_ptr[0] |= 0x01; 

	 /* copy P,Q,E buffers into PKA registers */     
	 LLF_PKI_PKA_CopyDataIntoPkaReg(rP/*dstReg*/, LenIDpq+1, p_ptr/*src_ptr*/, 
	                 pSizeInWords, VirtualHwBaseAddr); 
	 LLF_PKI_PKA_CopyDataIntoPkaReg(rQ/*dstReg*/, LenIDpq+1, q_ptr/*src_ptr*/, 
	                 pSizeInWords, VirtualHwBaseAddr); 
	 LLF_PKI_PKA_CopyDataIntoPkaReg(rE/*dstReg*/, LenIDpq+1, e_ptr/*src_ptr*/, 
	                 (eSizeInBits+31)/32, VirtualHwBaseAddr); 

       
 /* for debug */
 #ifdef LLF_PKI_PKA_DEBUG 
 #if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
    PQindex = 0;
 #endif 
 #endif
      /* find the first prime vector P */  
     Error = LLF_PKI_KG_FindPrime( rP, pSizeInBits,
                                   rE, eSizeInBits,
                                   testsCount,
                                   regTemps+3, 28-3/*tempsCount*/, 
                                   temp_ptr, 
                                   VirtualHwBaseAddr );
                                   
     if( Error != CRYS_OK )
       return Error;

	 /* temp for debug */
 #ifdef LLF_PKI_PKA_DEBUG 
 #if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
	    PQindex = 1;
 #endif 
 #endif 
 
    /* find the secoond prime Q such that |P-Q| > 2^100 */  
    Error = LLF_PKI_KG_FindPrime( rQ, pSizeInBits,
                                  rE, eSizeInBits,
                                  testsCount,
                                  regTemps+3, 28-3/*tempsCount*/, 
                                  temp_ptr, 
                                  VirtualHwBaseAddr );

   if( Error != CRYS_OK )     
     return Error;

   /* ..... if Q is larger then P exchange the vectors - we want to have P>Q */
   LLF_PKI_PKA_Sub( LenIDpq, rP/*OpA*/, rQ/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr ); /*reg extend*/

   LLF_PKI_PKA_GET_StatusCarry( VirtualHwBaseAddr, status );

   if( status == 0 )
   {
      /*virtual*/
      rSwap = rP;
      rP = rQ;
      rQ = rSwap;
   } 
   
   /* Set rT1 = 2^(pSizeInBits-100), where 100 bit is the min difference between P and Q*/             
   LLF_PKI_PKA_Clear( LenIDpq+1, rT1, 0/*Tag*/, VirtualHwBaseAddr );
   LLF_PKI_PKA_Flip0( LenIDpq, rT1, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
   LLF_PKI_PKA_BigShift( PKA_SHL0, LenIDpq, rT1/*OpA*/, pSizeInBits-100/*S*/,
                         rT1/*res*/, 0/*Tag*/, VirtualHwBaseAddr );        
   
   /* compare: rT2 = (P-Q) > rT1 */
   LLF_PKI_PKA_Sub( LenIDpq, rP/*OpA*/, rQ/*OpB*/, rT2, 0/*Tag*/, VirtualHwBaseAddr ); 
   LLF_PKI_PKA_Sub( LenIDpq, rT2/*OpA*/, rT1/*OpB*/, rT2, 0/*Tag*/, VirtualHwBaseAddr ); 
   
   /* if P-Q >= rT1 - the primes are valid, else exit for continuing to search the primes */ 
   LLF_PKI_PKA_GET_StatusCarry( VirtualHwBaseAddr, status );
   
   /* if status == 1, then output P,Q values from PKA registers 
	  and calculate other RSA key parameters */ 
   if( status == 1 )
   {
	   LLF_PKI_PKA_CopyDataFromPkaReg( p_ptr/*dst_ptr*/, pSizeInWords, 
	                                  rP/*srcReg*/, VirtualHwBaseAddr );
	   LLF_PKI_PKA_CopyDataFromPkaReg( q_ptr/*dst_ptr*/, pSizeInWords, 
	                                  rQ/*srcReg*/, VirtualHwBaseAddr );

	   /* calculate modulus n and private exponent d (if appropriate pointer is not NULL */
	   Error = LLF_PKI_CalculateNandD(
				e_ptr, eSizeInBits,  
				p_ptr, q_ptr, pSizeInBits,
				n_ptr, d_ptr, 
				VirtualHwBaseAddr );
	                       	
	   if( Error == CRYS_OK )
	      *Success_ptr = DX_TRUE;
	   
   } /* end of:  if(status == 1)*/

   return Error;


}/* END OF LLF_PKI_genKeyNonCrt */



/*************************************************************************************/
/**
 * @brief The LLF_PKI_CalculateNandD calculates RSA modulus and private key in NonCRT mode.
 *
 *    The function initializes the PKA according to used registers size, calculates
 *    keys components and finishes PKA operations.
 *           
 *
 * @param[in]  e_ptr             - The pointer to the public exponent.
 * @param[in]  eSizeInBits       - The public exponent size in bits.
 * @param[in]  p_ptr             - The first factor pointer (LSWord is the left most).
 * @param[in]  q_ptr             - The second factor pointer (LSWord is the left most).
 * @param[in]  primeSizeInBits   - The sze of the prime factors in bits.
 * @param[out] n_ptr             - The pointer to the RSA modulus buffer.
 * @param[out] d_ptr             - The pointer to the private exponent (non CRT).
 * @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

CRYSError_t LLF_PKI_CalculateNandD(
			DxUint32_t *e_ptr,  
			DxUint32_t  eSizeInBits,  
			DxUint32_t *p_ptr,
			DxUint32_t *q_ptr,
			DxUint32_t  primeSizeInBits,
			DxUint32_t *n_ptr,  
			DxUint32_t *d_ptr, 
			DxUint32_t  VirtualHwBaseAddr )
{
	/* LOCAL DECLARATIONS */

	CRYSError_t Error=CRYS_OK;

	/* virtual pointers to PKA registers of single size */
	DxInt8_t   r0, rE, rP, rQ, rD;
	
	/* modulus size in bits and bytes */	
	DxUint32_t  pSizeInWords;
	

	/* FUNCTION LOGIC */

	/* ...................... initialize local variables ............................ */
	/* ------------------------------------------------------------------------------ */

	/* initialize the Error to OK */
	Error = CRYS_OK;

	/* setting the primes P,Q length in bytes */
	pSizeInWords = (primeSizeInBits + 31) / 32;

 
	/*******************************************************************************************/
	/*                     CALCULATIONS WITH LONG REGISTERS                                    */
	/*  Init the PKA again on default mode according to N operation size.                      */ 
	/*  Note: All PKA memory shall be cleaned, nSizeInBits=> entry 0, nSizeInBits+32=> entry 1 */
	/*******************************************************************************************/
	if( n_ptr != DX_NULL )
	{
		/* Init: count of allowed registers for MaxSize of key: 7 including r0,r1,rT0,rT1 */
		Error = LLF_PKI_PKA_DefaultInitPKA(2*primeSizeInBits, LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS/32+1, VirtualHwBaseAddr);
		
		if( Error != CRYS_OK )     
		return Error;
		
		/* set virtual registers pointers */
		r0=0; rE=2; rP=3; rQ=4; rD=5; 
		
		/*  copy P, Q into PKA registers. Note: now size of registers is full.  */
		LLF_PKI_PKA_CopyDataIntoPkaReg(rP/*dstReg*/, LenIDn+1, p_ptr/*src_ptr*/, 
				       pSizeInWords , VirtualHwBaseAddr);
		
		LLF_PKI_PKA_CopyDataIntoPkaReg(rQ/*dstReg*/, LenIDn+1, q_ptr/*src_ptr*/, 
				       pSizeInWords , VirtualHwBaseAddr);
		/*-----------------------------------------------*/
		/*     N= r0= P*Q. LenID = 0 for full reg size   */
		/*-----------------------------------------------*/   
		
		LLF_PKI_PKA_LMul(LenIDn, rP/*OpA*/, rQ/*OpB*/, r0, 0/*Tag*/, VirtualHwBaseAddr);                                     
					     
		/* output the modulus N for releasing the r0 register, used also for LCM */
		LLF_PKI_PKA_CopyDataFromPkaReg(n_ptr,  2*pSizeInWords, r0/*srcReg*/, VirtualHwBaseAddr);
	}
     
	if( d_ptr != DX_NULL )
	{
	     
		/*--------------------------------------------------------------------------------*/
		/*.............     calculate D = E^-1 mod LCM(P-1)*(Q-1)           ............. */
		/*--------------------------------------------------------------------------------*/
		
		/* copy public exp E into rE register */
		LLF_PKI_PKA_CopyDataIntoPkaReg( rE/*dstReg*/, LenIDn+1, e_ptr/*src_ptr*/, 
					        (eSizeInBits+31)/32 , VirtualHwBaseAddr );
		
		/*  rN= (P-1)*(Q-1),  i.e. LCM of N */
		/*---------------------------------------------------*/
		LLF_PKI_PKA_Flip0( LenIDn, rP/*OpA*/, rP/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); /* rP= P-1 */
		LLF_PKI_PKA_Flip0( LenIDn, rQ/*OpA*/, rQ/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); /* rQ= Q-1 */
		LLF_PKI_PKA_LMul( LenIDn, rP/*OpA*/, rQ/*OpB*/, r0/*Res->r0*/, 0/*Tag*/, VirtualHwBaseAddr );                                     
		
		/* calc rD = modInv */
		Error = LLF_PKI_PKA_ExecFullModInv( rE/*OpB*/, rD/*Res*/, LenIDn, 
						 6/*rT0*/, 7/*rT1*/, 8/*rT2*/, 9/*rT3*/,
						 0/*Tag*/, VirtualHwBaseAddr );
		if( Error != CRYS_OK )                                             
		return Error;
		
		/*    output of the result value D */
		LLF_PKI_PKA_CopyDataFromPkaReg(d_ptr, 2*pSizeInWords, rD/*srcReg*/, VirtualHwBaseAddr);
	}

	return Error;


}/* END OF LLF_PKI_CalculateNandD */




/*************************************************************************************/
/**
 * @brief The LLF_PKI_genKeyCrt generates a public and private keys on CRT mode
 *
 *
 * @param[in]  e_ptr             - The pointer to the public exponent.
 * @param[in]  eSizeInBits       - The public exponent size in bits.
 * @param[in]  nSizeInBits       - The size of the key modulus in bits.
 * @param[out] Success_ptr       - The pointer to index of success of the generation.
 * @param[out] p_ptr             - The first factor pointer. In - random number, 
 *                                 out - prime (if successed).
 * @param[out] q_ptr             - The second factor pointer. In - random number, 
 *                                 out - prime (if successed).
 * @param[out] n_ptr             - The pointer to the modulus of the key.
 * @param[out] dp_ptr            - The private exponent for first factor.
 * @param[out] dq_ptr            - The private exponent for second factor.
 * @param[out] qinv_ptr          - The modular inverse of q relatively to modulus p.
 * @param[in]  VirtualHwBaseAddr - Virtual HW base address, passed by user.
 *
 *    Assuming: - eSizeInBits < nSizeInBits/2.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_PKI_genKeyCrt(
			DxUint32_t *e_ptr,         /*in*/ 
			DxUint32_t  eSizeInBits,   /*in*/  
			DxUint32_t  nSizeInBits,   /*in*/
			DxUint32_t  testsCount,    /*in*/        
			DxUint32_t *Success_ptr,   /*out*/
			DxUint32_t *p_ptr,         /*in/out*/
			DxUint32_t *q_ptr,         /*in/out*/
			DxUint32_t *n_ptr,         /*out*/
			DxUint32_t *dp_ptr,        /*out*/
			DxUint32_t *dq_ptr,        /*out*/
			DxUint32_t *qInv_ptr,      /*out*/
			DxUint32_t *temp_ptr,      /*in*/
			DxUint32_t   VirtualHwBaseAddr ) /*in*/
{
	/* LOCAL DECLARATIONS */
	
	CRYSError_t Error=CRYS_OK;
	
	
	/* FUNCTION LOGIC */
	
	/* ...................... initialize local variables ............................ */
	/* ------------------------------------------------------------------------------ */
	
	/* initialize the Error to OK */
	Error = CRYS_OK;
	
	/* initialize the success to 0 */
	*Success_ptr = DX_FALSE;
	 
	 
	/* ......................Generate NonCRT private Key ............................ */
	/* ------------------------------------------------------------------------------ */
	Error = LLF_PKI_genKeyNonCrt(
				e_ptr,       /*in*/  
				eSizeInBits, /*in*/  
				nSizeInBits, /*in*/
				testsCount,	/*in*/				        
				Success_ptr, /*out*/
				p_ptr,       /*out*/
				q_ptr,       /*out*/
				n_ptr,       /*out*/  
				DX_NULL,     /*out d_ptr - not needed*/
				temp_ptr,
				VirtualHwBaseAddr );
				
	if( Error != CRYS_OK || *Success_ptr != DX_TRUE )
	
		return Error;  /* return for generating new P,Q or to Stop */                        
	 
	
	/* ..................Calculate CRT private Key parameters ....................... */
	/* ------------------------------------------------------------------------------ */
	
	Error = LLF_PKI_CalculateCrtParams(
				e_ptr, 
				eSizeInBits,  
				nSizeInBits,
				p_ptr, q_ptr,
				dp_ptr, dq_ptr, qInv_ptr,
				VirtualHwBaseAddr );
	
	  
	return Error;
	
}/* END OF LLF_PKI_genKeyCrt */


/*************************************************************************************/
/**
 * @brief The LLF_PKI_CalculateCrtParams calculates a private key on CRT mode
 *
 *
 * @param[in]  e_ptr             - The pointer to the public exponent.
 * @param[in]  eSizeInBits       - The public exponent size in bits.
 * @param[in]  nSizeInBits       - The size of the key modulus in bits.
 * @param[in]  p_ptr             - The first factor pointer.
 * @param[in]  q_ptr             - The second factor pointer.
 * @param[out] dp_ptr            - The private exponent for first factor.
 * @param[out] dq_ptr            - The private exponent for second factor.
 * @param[out] qinv_ptr          - The modular inverse of q relatively to modulus p.
 * @param[in]  VirtualHwBaseAddr - Virtual HW base address, passed by user.
 *
 *   ????? Assuming: - eSizeInBits < nSizeInBits/2.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_PKI_CalculateCrtParams(
				DxUint32_t *e_ptr,  
				DxUint32_t eSizeInBits,  
				DxUint32_t nSizeInBits,
				DxUint32_t *p_ptr, 
				DxUint32_t *q_ptr,
				DxUint32_t *dp_ptr,
				DxUint32_t *dq_ptr,
				DxUint32_t *qInv_ptr,
				DxUint32_t  VirtualHwBaseAddr )
{
	/* LOCAL DECLARATIONS */
	
	CRYSError_t Error=CRYS_OK;
	
	
	/* the primes P, Q size */
	DxUint32_t pSizeInWords;
	
	/* virtual pointers to PKA registers of single size */
	DxInt8_t  r0, rP, rdP, rQ, rdQ, rQinv, rE;
	
	/* virtual pointers to single temp PKA registers */
	DxInt8_t rT1;
	
	DxUint8_t  LenID;
	
	
	/* FUNCTION LOGIC */
	
	/* ...................... initialize local variables ............................ */
	/* ------------------------------------------------------------------------------ */
	
	
	/* initialize the Error to OK */
	Error = CRYS_OK;
	
	/* setting the primes P,Q length ; Note: the size of the modulus n is even */
	pSizeInWords = nSizeInBits / (2*32);
	
	LenID = 0;
	
	/* init PKA again according to P and Q size */     
	Error = LLF_PKI_PKA_DefaultInitPKA( nSizeInBits/2, LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS/32+1, VirtualHwBaseAddr );
	
	if( Error != CRYS_OK )     
		return Error;
	
	/* set virtual registers pointers  */
	r0=0; rE=2; rP=3; rQ=4; rdP=5; rdQ=6; rQinv=7; rT1=8;
	
	/* copy data into PKA registers */
	/*------------------------------*/
	LLF_PKI_PKA_CopyDataIntoPkaReg(rE/*dstReg*/, LenID+1, e_ptr/*src_ptr*/, 
				      (eSizeInBits+31)/32 , VirtualHwBaseAddr);
	LLF_PKI_PKA_CopyDataIntoPkaReg(rP/*dstReg*/, LenID+1, p_ptr/*src_ptr*/, 
				      pSizeInWords, VirtualHwBaseAddr);
	LLF_PKI_PKA_CopyDataIntoPkaReg(rQ/*dstReg*/, LenID+1, q_ptr/*src_ptr*/, 
				      pSizeInWords, VirtualHwBaseAddr);
	
	/*--------------------------------------------------------------------------------*/
	/*....................... calc dP , dQ, Qinv..................................... */
	/*      dP = E^-1 mod (P-1); dQ = E^-1 mod (Q-1); qInv = Q^-1 mod P;              */
	/*--------------------------------------------------------------------------------*/
	
	/* dQ: set mod register r0=Q-1 and perform ModInv operation */
	/*-----------------------------------------------------------*/
	LLF_PKI_PKA_Flip0( LenID+1, rQ/*opA*/, r0/*res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID+1, rT1/*ds*/, rE/*src*/, 0/*Tag*/, VirtualHwBaseAddr );                             
	Error = LLF_PKI_PKA_ExecFullModInv( rT1/*OpB*/, rdQ/*Res*/, LenID, 
					     rT1+1, rT1+2, rT1+3, rT1+4,
					     0/*Tag*/, VirtualHwBaseAddr );
	if( Error != CRYS_OK )                                             
		return Error;
	
	
	/* dP: set mod register r0<=P-1 and perform ModInv operation */
	/*-----------------------------------------------------------*/
	LLF_PKI_PKA_Flip0( LenID+1, rP/*dst*/, r0/*res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID+1, rT1/*ds*/, rE/*src*/, 0/*Tag*/, VirtualHwBaseAddr );                             
	Error = LLF_PKI_PKA_ExecFullModInv( rT1/*OpB*/, rdP/*Res*/, LenID, 
					     rT1+1, rT1+2, rT1+3, rT1+4, 
					     0/*Tag*/, VirtualHwBaseAddr );
	if( Error != CRYS_OK )                                             
		return Error;
	
	/* Qinv: set mod register r0<=P and perform ModInv operation */
	/*-----------------------------------------------------------*/
	LLF_PKI_PKA_Flip0( LenID+1, r0/*OpA*/, r0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); /* r0= P */
	LLF_PKI_PKA_Copy( LenID+1, rT1/*dst*/, rQ/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModInv( LenID, rT1/*OpB*/, rQinv/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	
	/*-----------------------------------------------------------*/
	/*    output of the result values dP,dQ,qInv                 */
	/*-----------------------------------------------------------*/
	
	LLF_PKI_PKA_CopyDataFromPkaReg(dp_ptr, pSizeInWords, rdP/*srcReg*/, VirtualHwBaseAddr);
	
	LLF_PKI_PKA_CopyDataFromPkaReg(dq_ptr, pSizeInWords, rdQ/*srcReg*/, VirtualHwBaseAddr);
	
	LLF_PKI_PKA_CopyDataFromPkaReg(qInv_ptr, pSizeInWords, rQinv/*srcReg*/, VirtualHwBaseAddr);
	  
	/* end of calculations */
	return Error;
	
}/* END OF LLF_PKI_CalculateCrtParams */


