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

 
 #ifndef LLF_ECPKI_ModularArithmetic_H
#define LLF_ECPKI_ModularArithmetic_H
/*
 *  Object name:  LLF_ECPKI_ModularArithmetic.h
 */
  
/** @file
 * \brief This module defines the API for functions that supports the base modular 
          arithmetic operations.
 *
 * \version  LLF_ECPKI_ModularArithmetic.h - 1
 * \author R.Levin
 */

#include "crys_defs.h"
#include "crys_ecpki_types.h"
#include "llf_ecpki_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines *****************************************/

/************************ Enums *******************************************/
 
 typedef enum
 {
 	CompareEqual =	0,
 	FirstGrate	 =	1,
 	SecondGrate	 =	2,
 	
 	LLF_ECPKI_CompareResultLast= 0x7FFFFFFF,

 } LLF_ECPKI_CompareResult_t;

/************************ Typedefs ***************************************/


/************************ Global Data ************************************/

/************************ Public functions *******************************/


 									 

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
									 DxInt8_t			 rP,                /*in*/
									 DxInt8_t 		  	 rQ,                /*in*/
									 DxInt8_t 			 rk,                /*in*/									 
	 								 DxInt8_t			 rv0,               /*out*/
	 								 DxInt8_t			 rq0,               /*out*/
							         DxInt8_t           *regTemps_ptr,      /*in*/
							         DxUint32_t          countTemps,        /*in*/
							         DxUint32_t          VirtualHwBaseAddr  /*in*/ );
	 								 
/**************************************************************************
 *	              LLF_ECPKI_SquareRootModPrime  function                       *
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
                                     DxInt8_t				 rg,                /*in*/
									 DxInt8_t 		    	 rz,                /*out*/									 
									 DxBool_t   			*isRootExists,      /*out*/ 						 
							         DxInt8_t               *regTemps_ptr,      /*in*/
							         DxUint32_t              countTemps,        /*in*/
							         DxUint32_t              VirtualHwBaseAddr  /*in*/ );	 								 


/*************************************************************************/



#ifdef __cplusplus
}
#endif			

#endif
