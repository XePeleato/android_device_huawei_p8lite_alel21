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

 
 
#include "dx_vos_basetypes.h"
#include "dx_vos_mem.h"
#include "crys_defs.h"
#include "crys_common_math.h"
#include "crys_rnd.h"
#include "llf_pki.h"
#include "llf_pki_error.h"
#include "crys_ecpki_types.h"
#include "llf_ecpki_error.h"
#include "llf_ecpki.h"
#include "llf_ecpki_modular_arithmetic.h"

/* **************************************** defines ********************************* */

/* canceling the lint warning:
Info 716: while(1) ... */ 
/*lint --e{716} */

/* canceling the lint warning:
  Info 740: Unusual pointer cast 
  (incompatible indirect types) */
/*lint --e{740} */




/**************************************************************************
 *	              LLF_ECPKI_LucasSequence()  function                       *
 **************************************************************************/
/**
   @brief  This function computes the big integer numbers of Lucas 
           sequence according to algorithm  IEEE Std 1363-2000, A.2.4:
           
           Input: An odd integer modulus n > 2, integers P and Q, 
                  and a positive integer k. 
           Output: vk mod n  and  Q^|k/2| mod n
           
				1. Set v0 = 2, v1 = P, q0 = 1, q1 = 1.
				2. Let k = kr kr-1...k1 k0 be the binary representation of k, 
				   where the leftmost bit kr of k is 1.
				3. For i from r downto 0 do
				3.1 Set q0 = q0*q1 mod n.
				3.2 If ki = 1, then set
				       q1 = q0*Q mod n.
				       v0 = v0*v1 - P*q0 mod n.
				       v1 = v1^2  - 2*q1 mod n.
				Else set
				       q1 = q0,
				       v1 = v0*v1 - P*q0  mod n.
				       v0 = v0^2  - 2*q0  mod n.
				4. Output v0 and q0.
				
   Assumings: The PKA is initialized on default mode, including initialization for
              modular operations.				
   
   @param[in]  rP   - A pointer to parameter P.
   @param[in]  rQ   - A pointer to parameter Q.
   @param[in]  rk   - A pointer to integer k.
   @param[out] rv0  - A pointer to first result integer v0.
   @param[out] rq0  - A pointer to second result integer q0.
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
                               
   @return CRYSError_t - On success CRYS_OK is returned, on failure a
                         appropriate error value .
*/

CRYSError_t  LLF_ECPKI_LucasSequence( 
			DxInt8_t	 rP,                /*in*/
			DxInt8_t   	 rQ,                /*in*/
			DxInt8_t 	 rk,                /*in*/									 
			DxInt8_t	 rv0,               /*out*/
			DxInt8_t	 rq0,               /*out*/
			DxInt8_t        *regTemps_ptr,      /*in*/
			DxUint32_t       tempsCount,        /*in*/
			DxUint32_t       VirtualHwBaseAddr  /*in*/)
{
	/* 	LOCAL DECLARATIONS */

	CRYSError_t Error=0;
	DxInt32_t  i;
	
	/* virtual pointers */
	DxInt8_t  rv1, rq1, rT;
	/* effective size of k in bits */
	DxUint32_t  kSizeInBits;
	
	DxUint8_t  bit;
	
	/* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
	if( tempsCount < 3 )   
		return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;  
#endif

	
	/* Initialization of pointers and variables */
	
	Error = CRYS_OK;
	tempsCount = tempsCount;
	/* set virtual pointers */
	rv1   = regTemps_ptr[0];
	rq1   = regTemps_ptr[1];
	rT    = regTemps_ptr[2];    
	
	
	/*  FUNCTION LOGIC   */ 
	
	
	/* 1. Set v0 = 2, v1 = P, q0 = 1, q1 = 1 */
	
	/* q0 = 1 */
	LLF_PKI_PKA_Clr( LenID_n+1, rq0/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr ); 
	LLF_PKI_PKA_Set0( LenID_n, rq0/*OpA*/, rq0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* q1 = 1 */
	LLF_PKI_PKA_Copy( LenID_n+1, rq1/*dest*/, rq0/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* v0 = 2 */
	LLF_PKI_PKA_Add( LenID_n, rq0/*OpA*/, rq0/*OpB imm*/, rv0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* v1 = P) */ 
	LLF_PKI_PKA_Copy( LenID_n+1, rv1/*dest*/, rP/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	

	/* 2. Let k = kr kr-1...k1 k0 be the binary representation of k, 
	  calculate effective size of k in bits                     */
	kSizeInBits = LLF_PKI_PKA_GetRegEffectiveSizeInBits( LenID_n, rk, 0/*Tag*/, VirtualHwBaseAddr );

	/*---------------------------------------------------------------*/  
	/*         3. For i from r = kSizeInBits-1 downto 0 do          */
	/*---------------------------------------------------------------*/  
	for( i = (DxInt_t)kSizeInBits - 1; i >= 0; i-- )
	{
		/* 3.1. Set q0 = q0*q1  */
		LLF_PKI_PKA_ModMul( LenID_n, rq0/*OpA*/, rq1/*OpB*/, rq0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
        
		/* 3.2. If bit i of k == 1 then set */
		bit =  LLF_PKI_PKA_GetBitFromPkaReg( rk, LenID_n, i, rT, VirtualHwBaseAddr );        
		if( bit == 1 )
		{
			/* q1 = q0*Q mod n */
			LLF_PKI_PKA_ModMul( LenID_n, rq0/*OpA*/, rQ/*OpB*/, rq1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
						      
			/* v0 = v0*v1 - P*q0 mod n */  
			LLF_PKI_PKA_ModMul( LenID_n, rv0/*OpA*/, rv1/*OpB*/, rv0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			LLF_PKI_PKA_ModMul( LenID_n, rP/*OpA*/, rq0/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			LLF_PKI_PKA_ModSub( LenID_n, rv0/*OpA*/, rT/*OpB*/, rv0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			
			/* v1 = v1^2  - 2*q1 mod n */
			LLF_PKI_PKA_ModMul( LenID_n, rv1/*OpA*/, rv1/*OpB*/, rv1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			LLF_PKI_PKA_ModAdd( LenID_n, rq1/*OpA*/, rq1/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			LLF_PKI_PKA_ModSub( LenID_n, rv1/*OpA*/, rT/*OpB*/, rv1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		}
		
		else
		{
			/* q1 = q0 */        	
			LLF_PKI_PKA_Copy( LenID_n+1, rq1/*dest*/, rq0/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
			
			/* v1 = v0*v1 - P*q0  mod n */
			LLF_PKI_PKA_ModMul( LenID_n, rv0/*OpA*/, rv1/*OpB*/, rv1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			LLF_PKI_PKA_ModMul( LenID_n, rP/*OpA*/, rq0/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			LLF_PKI_PKA_ModSub( LenID_n, rv1/*OpA*/, rT/*OpB*/, rv1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
						
			/* v0 = v0^2  - 2*q0 mod n */
			LLF_PKI_PKA_ModMul( LenID_n, rv0/*OpA*/, rv0/*OpB*/, rv0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			LLF_PKI_PKA_ModAdd( LenID_n, rq0/*OpA*/, rq0/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
			LLF_PKI_PKA_ModSub( LenID_n, rv0/*OpA*/, rT/*OpB*/, rv0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		}                                		
	}
    
	return Error;
		
} /* End of LLF_ECPKI_LucasSequence() function */



/**************************************************************************
 *	              LLF_ECPKI_SquareRootModPrime()  function                *
 **************************************************************************/
/**
   @brief  This function computes a square root from g modulo odd prime p
           (if one exists). The calculations are performed according 
           to algorithm  IEEE Std 1363-2000, A.2.5:
           
           Input: An odd integer modulus n, and integer g such that 0 < g < n. 
           Output: an integer z - square root of g modolo n, if one exists,
                   otherwice output an error message "LLF_ECPKI_NO_SQUARE_ROOT_EXISTS" .
           
           The algorithm steps are:
           
           1. In case n = 3 (mod 4),  means n = 4*k + 3:
           	  1.1. Output: z = g^(k+1) mod n.
           
           2. In case n = 5 (mod 8),  means n = 8*k + 5:
              2.1. y = (2*g)^k  mod n,
              2.2. u = 2*g*y^2  mod n,	  	          
              2.3. Output: z = g*y*(u-1) mod n.
           
           3. In case n = 1 (mod 8),  means n = 8*k + 1: 
              3.1. Set  Q = g,
              3.2. Genrate value P:  0 < P < n,
              3.3. Compute via LLF_ECPKI_LucasSequence() function the quantities
                   V = v0, and Q0 = q0  for k = (n+1)/2,
              3.4. Set z = V/2 mod n,
              3.5. If(z^2 mod n) == g, then output z and stop.
              3.6. If 1 > Q0 < n-1, then output the error message 
                   "LLF_ECPKI_NO_SQUARE_ROOT_EXISTS"       
              3.7. Go to step 3.2.      
   
   Assuming: The PKA is initialised on default mode, including initialization 
             of modular operations.          
			
   @param[in]  rg     - A virtual pointer to integer g. 
   @param[in]  rz     - A virtual pointer to result z.
   @param[in]  isRootExists  - a variable designates is root of g exists or not:
				IsRootExists = DX_TRUE if root exists,
				IsRootExists = DX_FALSE if root not exists, 
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
                               
   @return CRYSError_t - On success CRYS_OK is returned, on failure a
                         appropriate error value from called functions.   
*/

CRYSError_t  LLF_ECPKI_SquareRootModPrime(
			DxInt8_t	 rg,                /*in*/
			DxInt8_t    	 rz,                /*out*/									 
			DxBool_t  	*isRootExists,      /*out*/ 						 
			DxInt8_t        *regTemps_ptr,      /*in*/
			DxUint32_t       countTemps,        /*in*/
			DxUint32_t       VirtualHwBaseAddr  /*in*/ )
{
	/* Local variables and structures definitions */
	
	CRYSError_t Error; 
	
	/* virtual pointers  */
	DxInt8_t  rk, rQ0, rV, rT, rP, ry;
	
	/* last word of modulus */
	DxUint32_t   modLSW;
	
	DxInt32_t   status1, status2; /* Variables for logic results */
	
	
	/*   Initialization of variables and  pointers  */
	
	Error = CRYS_OK;
	
 #ifdef LLF_PKI_PKA_DEBUG   
	/* check count of temp registers used */
	if( countTemps < 5 )
		return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
 #endif	

    
	/* Pointers to variables on temp buffer */
	rk   = regTemps_ptr[0];
	rT   = regTemps_ptr[1];
	rQ0  = regTemps_ptr[2];
	rV   = regTemps_ptr[3];
	ry   = regTemps_ptr[4];
	rP   = rT;
	
	
	/* ***********************   FUNCTION LOGIC   ************************* */ 
	*isRootExists = DX_FALSE;
	
	/* get LS word of modulus */
	modLSW = LLF_PKI_PKA_GetWordFromRegister( LenID_n, 0/*mod reg*/, 0/*i*/, VirtualHwBaseAddr );
	
	/*----------------------------------------------------------------------*/
	/* 1. Case n = 3 (mod 4),  means n = 4*k + 3:                           */
	/*----------------------------------------------------------------------*/
	if( modLSW % 4 == 3 )
	{
		/* 1.1. Calculate k+1 = n / 4 + 1 // by shift operation */ 
		LLF_PKI_PKA_SHR0( LenID_n, 0/*OpA=mod*/, 2-1/*shift*/, rk/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		LLF_PKI_PKA_ModAddIm( LenID_n, rk/*OpA*/, 1/*OpB imm*/, rk/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		
		/* 1.2. Calculate z = g^(k+1) mod n and return */     	
		LLF_PKI_PKA_ModExp( LenID_n, rg/*OpA*/, rk/*OpB imm*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
	}
    
	/*----------------------------------------------------------------------*/
	/* 2. Case n = 5 (mod 8),  means n = 8*k + 5:                        */
	/*----------------------------------------------------------------------*/
	if( modLSW % 8 == 5 )
	{
		/* 2.1. Calculate k = n / 8 // by shift operation */    	
		LLF_PKI_PKA_SHR0( LenID_n, 0/*OpA=mod*/, 3-1/*shift*/, rk/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
			
		/* 2.2. Calculate y = (2*g)^k mod n          */		
		
		/* T = 2*g */                                                      
		LLF_PKI_PKA_ModAdd( LenID_n, rg/*OpA*/, rg/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		/* y = T^k mod n                                                     		 */
		LLF_PKI_PKA_ModExp( LenID_n, rT/*OpA*/, rk/*OpB imm*/, ry/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		
		/* 2.3. Calculate rz = u = 2*g*y^2 = rT*rY*rY */  
		LLF_PKI_PKA_ModMul( LenID_n, rT/*OpA*/, ry/*OpB*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		LLF_PKI_PKA_ModMul( LenID_n, rz/*OpA*/, ry/*OpB*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		
		/* 2.4. Calculate rz = (u-1)*y*g mod n   */         
		LLF_PKI_PKA_ModSubIm( LenID_n, rz/*OpA*/, 1/*OpB imm*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		/* z = z*y    */
		LLF_PKI_PKA_ModMul( LenID_n, rz/*OpA*/, ry/*OpB*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		/* z = z*g       */                          		
		LLF_PKI_PKA_ModMul( LenID_n, rz/*OpA*/, rg/*OpB*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
	}

	/*----------------------------------------------------------------------*/
	/* 3. Case n = 1 (mod 8),  let: n = 8*k + 1:                            */
	/*----------------------------------------------------------------------*/
	else if( modLSW % 8 == 1 )
	{
		/* Copy modulus n into rP */
		LLF_PKI_PKA_Copy( LenID_n+1, rP/*dst*/, 0/*src=mod*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		
		while(1)
		{
			/* 3.1. Generate new value P: 0 < P < n by subtracting 1 from P. 
			If P == 0 then set IsRootExists = 0 and return */
			LLF_PKI_PKA_SubIm( LenID_n, rP/*OpA*/, 1/*OpB */, rP/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
			LLF_PKI_PKA_CompareIm( LenID_n, rP/*OpA*/, 0/*OpB imm*/, 0/*Tag*/, VirtualHwBaseAddr);      	
			LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);
			if( status1 == 1 ){
				*isRootExists = DX_FALSE;
				return Error;
			}
	
			/* 3.2. Compute via LLF_ECPKI_LucasSequence() function the quantities
			V = v0 and Q0 = q0  for k = (n+1)/2, see IEEE 1363 A.2.5 */    	
			
			/* 3.2.1. k = (n+1)/2 */ 
			LLF_PKI_PKA_AddIm( LenID_n+1, 0/*OpA=mod*/, 1/*OpB imm*/, rk/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
			LLF_PKI_PKA_SHR0( LenID_n+1, rk/*OpA*/, 1-1/*shift*/, rk/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
			
			/* 3.2.2. Lucas numbers V, Q0 calculation */
			LLF_ECPKI_LucasSequence( rP, rg, rk, rV, rQ0, 
				 regTemps_ptr+5, countTemps-5, VirtualHwBaseAddr  );
			
			/* 3.2.4. Set z = V/2 mod n */
			LLF_PKI_PKA_ModDivideBy2( LenID_n, rV, 0/*mod*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );			
			
			/* If(z^2 mod n) == g, then output z and return   */   
			/* Calculate y = z^2   */
			LLF_PKI_PKA_ModMul( LenID_n, rz/*OpA*/, rz/*OpB*/, ry/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
			
			/* Compare z^2 to g: if (z^2 mod n) == g then output z and return */
			LLF_PKI_PKA_Compare( LenID_n, ry/*OpA*/, rg/*OpB imm*/, 0/*Tag*/, VirtualHwBaseAddr);      	
			LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);
			if( status1 == 1 ){
				*isRootExists = DX_TRUE; 
				return Error;
			}
			
			/* If 1 < Q0 < n-1, then output IsRootExists = 0 and return */
			
			/* Compare Q0 to 1 */
			LLF_PKI_PKA_Clr( LenID_n, rz/*OpA*/,0/*Tag*/, VirtualHwBaseAddr);
			LLF_PKI_PKA_Flip0( LenID_n, rz/*OpA*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);	
			LLF_PKI_PKA_Sub( LenID_n, rz/*OpA*/, rQ0/*OpB imm*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);      	
			LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status1);
			
			/* Set y = n-1 */
			LLF_PKI_PKA_SubIm( LenID_n, 0/*OpA=mod*/, 1/*OpB imm*/, rz/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
			
			/* Compare Q0 to n-1 */
			LLF_PKI_PKA_Sub( LenID_n, rz/*OpA*/, rQ0/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);
			LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status2);
			if( status1 == 0  && status2 == 1 ){        	
				*isRootExists = DX_FALSE;		        
				return Error;    		
			} 
		}                        		                           		
	}       							             	    
	
	/**   4. Check the result (for cases 1, 2)     **/
	
	if( modLSW % 4 == 3 || modLSW % 8 == 5 ){

		/* 4.1. Calculate y = z^2 mod n      */
		LLF_PKI_PKA_ModMul( LenID_n, rz/*OpA*/, rz/*OpB*/, ry/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		
		/* 4.2. Compare: If z^2 == g, then root is exists, else not exists  */
		LLF_PKI_PKA_Compare( LenID_n, ry/*OpA*/, rg/*OpB imm*/, 0/*Tag*/, VirtualHwBaseAddr);      	
		LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);
		if( status1 == 1 )
			*isRootExists = DX_TRUE; 		
		else
			*isRootExists = DX_FALSE;
	}
	
	return Error; 
		
} /* End of LLF_ECPKI_SquareRootModPrime() function */



/**********************************************************************************************************/

