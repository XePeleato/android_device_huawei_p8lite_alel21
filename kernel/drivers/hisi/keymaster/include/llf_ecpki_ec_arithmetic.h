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

 
 #ifndef LLF_ECPKI_ECArithmetic_H
#define LLF_ECPKI_ECArithmetic_H

/*
 *  Object name     :  %
 *  State           :  %state%
 *  Creation date   :  08/12/2005
 *  Last modified   :  %modify_time%
 */
  
/** @file LLF_ECPKI_ECArithmetic.h
 * \brief This module defines the API for functions that supports the base mathematic 
 *        operations on finite fields over elliptic curve.
 *
 * \version LLF_ECPKI_ECArithmetic.h#1:incl:1
 * \author R.Levin
 */

#include "crys_defs.h"
#include "crys_ecpki_types.h"
#include "llf_ecpki_error.h"
#include "llf_ecpki.h"
#include "llf_ecpki_ec_arithmetic.h"

#ifdef __cplusplus
extern "C"
{
#endif

 
/************************ Defines *************************************/

/************************ Enums ***************************************/

/************************ Typedefs  ***********************************/

/************************ Structs  ************************************/

/************************ Public Variables ****************************/

/************************ Public Functions ****************************/


/**************************************************************************/
/**
   @brief  This function performs scalar multiplication of EC point by big number n. 
   
           Res = n*P0:   where: n - big integer positive number n < EC_order; 
                                    P0, Res - affine EC GFp points.                       
           
   @param[in]  rn             - A virtual pointer to register, containing scalar n.
   @param[in]  rP0            - A pointer to structure, containing  point P0 in 
   								projective Jacobian coordinates.
   @param[in]  rRes           - A pointer to structure, containing result point in
   								projective Jacobian coordinates.
   @param[in]  DomainID       - EC domain identifier.      
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
   
   @return CRYSError_t - CRYS_OK,
                         LLF_ECPKI_SCALAR_MULT_INVALID_DOMAIN_ID_ERROR,						   
						 LLF_ECPKI_ScalarMult_INVALID_POINTS_ID_ERROR,
						 LLF_ECPKI_ScalarMult_ARITHM_OPERATIONS_OFF_MODE_ERROR  
*/	
CRYSError_t  LLF_ECPKI_ScalarMultAff(
				DxInt8_t   	   	      rn, 	   	        /*in*/
				LLF_ECPKI_PKA_Point_t         *P0,               /*in*/
				LLF_ECPKI_PKA_Point_t	     *Res,             /*out*/
				CRYS_ECPKI_DomainID_t		  DomainID,         /*in*/ 
				DxInt8_t                      *regTemps_ptr,     /*in*/
				DxUint32_t                     countTemps,       /*in*/
				DxUint32_t                     VirtualHwBaseAddr /*in*/ );


/**************************************************************************
 *	              LLF_ECPKI_FullAddAff function
 **************************************************************************/
/**
   @brief  This function checks reduction mode and calls appropriate LLF function,
           which adds two points on elliptic curve PointRes = Point0 + Point1 
           for different cases of points combinations. 
   
   @param[in]  P1       - A pointer to structure, containing first point.
   @param[in]  P2       - A pointer to structure, containing second point.       				
   @param[out] Res      - A pointer to structure, containing result point.
   @param[in]  DomainID - EC domain identifier.   
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
  
   @return CRYSError_t - CRYS_OK,
                         LLF_ECPKI_SCALAR_MULT_BARRET_REDUCT_NOT_SUPPORTED_ERROR     
*/	
CRYSError_t  LLF_ECPKI_FullAddAff(					   					   
			LLF_ECPKI_PKA_Point_t      *P1,               /*in*/    		   
			LLF_ECPKI_PKA_Point_t      *P2,               /*in*/
			LLF_ECPKI_PKA_Point_t      *Res,              /*out*/
			CRYS_ECPKI_DomainID_t	   DomainID,         /*in*/ 
			DxInt8_t                   *regTemps_ptr,     /*in*/
			DxUint32_t                  countTemps,       /*in*/
			DxUint32_t                  VirtualHwBaseAddr /*in*/ );




/**************************************************************************
 *	              LLF_ECPKI_GFpAddProj function
 **************************************************************************/
/**
   @brief  This function adds an EC GF(P) point to another point: 
                        
                     Res = P1 + P2.  
           
           The input and result points are in projective Jacobian coordinates.
           The operations are performed with Barrett reduction.
           
           If the input points are the same (P1 = P2) the function returns 
           an indication Res = (0,0,0), that mines it is neccessary to
           use Doubling operation instead Addition.           
           If the result of addition is point on infinity, then the function returns
           Res = (1,1,0).
                      
           The function algorithm according to IEEE Std 1363-2000, A.10.7.
   
   @param[in]  P1             - A pointer to structure, containing first point 
   @param[in]  P2             - A pointer to structure, containing second point.       				
   @param[out] Res            - A pointer to structure, containing result point.
   @param[in]  ECPKI_DomainID - EC domain identifier.   
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
  
   @return CRYSError_t - CRYS_OK,
                         LLF_ECPKI_GFP_ADD_MONT_INVALID_DOMAIN_ID_ERROR						   
						 LLF_ECPKI_GFP_ADD_MONT_INVALID_POINTS_ID_ERROR  
						 LLF_ECPKI_GFpAddMont_TWO_POINTS_THE_SAME_ERROR 
*/	

CRYSError_t  LLF_ECPKI_GFpAddProj(					   					   
			LLF_ECPKI_PKA_Point_t     *P1,                  /*in*/    		   
			LLF_ECPKI_PKA_Point_t     *P2,                  /*in*/
			LLF_ECPKI_PKA_Point_t	 *Res,                 /*out*/
			CRYS_ECPKI_DomainID_t	  DomainID,            /*in*/ 
			DxInt8_t                  *regTemps_ptr,        /*in*/
			DxUint32_t                 countTemps,          /*in*/
			DxUint32_t                 VirtualHwBaseAddr    /*in*/ );


/**************************************************************************
 *	              LLF_ECPKI_GFpDoubbleProj function
 **************************************************************************/
/**
   @brief  This function performs doubling of EC GF(P) point (adds one point 
   		   to the same point: Res = P0 + P0 = 2*P0.
           
           The operations are performed with Barrett reduction. Coordinates
           of input and output points must be in projective Jacobian form.
           
           The function performs point doubling according to IEEE Std 1363-2000
           algorithm A.10.4
           
               
   @param[in]  P0             - A pointer to structure, containing first point                                 .
   @param[out] Res            - A pointer to structure, containing result point.
   @param[in]  ECPKI_DomainID - EC domain identifier.   
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
   
   @return CRYSError_t - CRYS_OK,
                         LLF_ECPKI_GFP_DOUBLE_INVALID_DOMAIN_ID_ERROR						   
						 LLF_ECPKI_GFpDouble_INVALID_POINTS_ID_ERROR 					 
*/	

CRYSError_t  LLF_ECPKI_GFpDoubleProj(					   					   		   
				LLF_ECPKI_PKA_Point_t     *P0,                  /*in*/
				LLF_ECPKI_PKA_Point_t	 *Res,                 /*out*/
				CRYS_ECPKI_DomainID_t	  DomainID,            /*in*/ 
				DxInt8_t                  *regTemps_ptr,        /*in*/
				DxUint32_t                 countTemps,          /*in*/
				DxUint32_t                 VirtualHwBaseAddr    /*in*/ );
					   

/**************************************************************************
 *	              LLF_ECPKI_FullAdd function
 **************************************************************************/
/**
   @brief  This function performs full addition and subtraction of EC GF(P) points 
           according to IEEE Std 1363-2000 algorithm A.10.8. 
                     
           If P1 != P2, then the function performs point addition, else - doubling.           
           The operations are performed with Barrett reduction. 
           If the result of addition is point on infinity, the function returns
           Res = (1,1,0).                      
                      
           The points must be set in projective Jacobian coordinates,
                       
   @param[in]  P1             - A pointer to structure, containing first point                                 .
   @param[in]  P2             - A pointer to structure, containing second point.       				
   @param[out] Res            - A pointer to structure, containing result point.
   @param[in]  ECPKI_DomainID - EC domain identifier.   
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
  
   @return CRYSError_t - CRYS_OK,
                         LLF_ECPKI_GFP_FULL_ADD_INVALID_DOMAIN_ID_ERROR						   
						 LLF_ECPKI_GFP_FULL_ADD_POINTS_NOT_JACOBIAN_ERROR 
						 LLF_ECPKI_GFP_FULL_ADD_ARITHM_OPERATIONS_OFF_MODE_ERROR						 
*/
CRYSError_t  LLF_ECPKI_FullAdd(					   					   
			LLF_ECPKI_PKA_Point_t     *P1,                  /*in*/    		   
			LLF_ECPKI_PKA_Point_t     *P2,                  /*in*/
			LLF_ECPKI_PKA_Point_t	 *Res,                 /*out*/
			CRYS_ECPKI_DomainID_t	  DomainID,            /*in*/ 
			DxInt8_t                  *regTemps_ptr,        /*in*/
			DxUint32_t                 countTemps,          /*in*/
			DxUint32_t                 VirtualHwBaseAddr    /*in*/ );
					   

/**************************************************************************
 *	              LLF_ECPKI_SubtractProj function
 **************************************************************************/
/**
   @brief  This function performs  subtraction of EC  points according to 
           IEEE Std 1363-2000 algorithm A.10.8: 
                                
                Res = P1 - P2 = P1 + minusP2, 
                
            The operation is performed by computing MinusPoint1 = - Point1 and 
            calling  LLF_ECPKI_FullAdd function for adding.     
            
            Coordinates of the point minusP2(X*,Y*,Z*) are: X* = X1, Z* = Z1,
                          -
                         |  -Y1 = P - Y1   if modulus is prime P,
                    Y* = |
                         |  X1*Z1 + Y1     if modulus is q = 2^n  
                          -
           All other requirements and defines are the same as for LLF_ECPKI_FullAdd 
           function.
                       
   @param[in]  P0             - A pointer to structure, containing first point                                 .
   @param[in]  P1             - A pointer to structure, containing second point.       				
   @param[out] Res            - A pointer to structure, containing result point.
   @param[in]  ECPKI_DomainID - EC domain identifier.   
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
  
   @return CRYSError_t - CRYS_OK,
                         LLF_ECPKI_SUBTRACT_POINTS_INVALID_DOMAIN_ID_ERROR						   
						 LLF_ECPKI_SUBTRACT_POINTS_NOT_JACOBIAN_ERROR 
						 LLF_ECPKI_SUBTRACT_POINTS_ARITHM_OPERATIONS_OFF_MODE_ERROR						 
*/
CRYSError_t  LLF_ECPKI_SubtractPoints(					   					   
				LLF_ECPKI_PKA_Point_t     *P1,                  /*in*/    		   
				LLF_ECPKI_PKA_Point_t     *P2,                  /*in*/
				LLF_ECPKI_PKA_Point_t	 *Res,                 /*out*/
				CRYS_ECPKI_DomainID_t	  DomainID,            /*in*/ 
				DxInt8_t                  *regTemps_ptr,        /*in*/
				DxUint32_t                 countTemps,          /*in*/
				DxUint32_t                 VirtualHwBaseAddr    /*in*/ );


/**************************************************************************
 *	              LLF_ECPKI_GFpScalarMultProj function
 **************************************************************************/
/**
@brief  This function performs scalar multiplication  of EC point P0 by integer n.

          Res = n*P0,  where: n - big integer positive number n < EC_order; 
                              P0, Res - EC GFp projective Jacobian points.                       
           
          The function performs scalar multiplication according to 
          IEEE Std 1363-2000 algorithm A.10.9.
            
   @param[in]  rn             - A virtual pointer to register, containing the scalar n.
   @param[in]  P0             - A pointer to structure, containing  point P0 in 
   								projective Jacobian coordinates.
   @param[in]  Res            - A pointer to structure, containing result point in
   								projective Jacobian coordinates.
   @param[in]  DomainID       - EC domain identifier.      
   @param[in]  regTemps_ptr   - A pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
   
   @return CRYSError_t - CRYS_OK,
   
   @return CRYSError_t - CRYS_OK,
                         LLF_ECPKI_SCALAR_MULT_INVALID_DOMAIN_ID_ERROR,						   
						 LLF_ECPKI_ScalarMult_INVALID_POINTS_ID_ERROR,
						 LLF_ECPKI_ScalarMult_ARITHM_OPERATIONS_OFF_MODE_ERROR  
*/
	
CRYSError_t  LLF_ECPKI_GFpScalarMultProj(
					DxInt8_t  	   	      rn, 	        /*in*/
					LLF_ECPKI_PKA_Point_t         *P0,              /*in*/
					LLF_ECPKI_PKA_Point_t	     *Res,             /*out*/
					CRYS_ECPKI_DomainID_t	      DomainID,         /*in*/ 
					DxInt8_t                      *regTemps_ptr,     /*in*/
					DxUint32_t                     countTemps,       /*in*/
					DxUint32_t                     VirtualHwBaseAddr /*in*/ );					   
						   
					   
/**************************************************************************
 *	              LLF_ECPKI_IsPointOnCurve function
 **************************************************************************/
/**
   @brief  This function checks is the affine point Point0 on elliptic curve or not.  
           The function calculates a curve equation with points coordinates X,Y 
		   as following:  
   		   - checks that X,Y are in interval [1,P-1];
   		   - converts coordinates (X,Y) of point into Montgomery representation;
   		   - calculates  L = Y^2 mod P,  R = (X^3 + A*X + B) mod P, 
   		     where A,B - curve parameters;
   		   - If (L - R) = 0  outputs *IsPointOnCurve = True, otherwice outputs False.  		  
               
   @param[in]  W_ptr          - A pointer to structure, containing the point.
   @param[in]  DomainID       - EC domain identifier. 
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user. 

   @return     If the point is on curve, the function returns CRYS_OK, else returns an Errror: 
                  LLF_ECPKI_EC_POINT_IS_NOT_ON_CURVE_ERROR                         
*/	

CRYSError_t  LLF_ECPKI_IsPointOnCurve( 
				LLF_ECPKI_PKA_Point_t *W_ptr,            /*in*/					   
				CRYS_ECPKI_DomainID_t  DomainID,         /*in*/ 
				DxInt8_t              *regTemps_ptr,     /*in*/
				DxUint32_t   		  countTemps,       /*in*/
				DxUint32_t   		  VirtualHwBaseAddr /*in*/ );
					   
/**************************************************************************
 *	              LLF_ECPKI_GFp_ConvArrayToECaffine function
 **************************************************************************/
/**
   @brief  This function converts the input array Arr into affine point PAff on 
           elliptic curve. The function calculates right part of curve equation 
		   with point coordinate X and than calqulates Y as square root from the right part.
		   The function does the following:		   
		   1. Copies array data into previously nulled buffer X of size of EC modulus size. 
           2. Calculates  R = (X^3 + A*X + B) mod P, where P - modulus, A,B - EC parameters.
   		   3. Calculates Y = R^(-2) mod P by calling square root function.
		   4. If square root is not exist, changes the low byte X[0] += 1 and go to step 2.
		      Note: this low byte is the high byte for big endian representation of the input array.  
     	   5. If count of tries is greater than 0x100, then the function returns an error message.   		  
               
   @param[in]  rArr           - A virtual pointer to register, containing the data to be converted.
   @param[in]  PAff_ptr       - A pointer to output point.
   @param[in]  DomainID       - EC domain identifier. 
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user. 

   @return     Error.  

         NOTE: 1. Only first (EC_modulus_size  1) low bytes of array may be recovered right
		          from converted point.
*/
CRYSError_t  LLF_ECPKI_GFp_ConvArrayToECaffine(
					DxInt8_t	         rArr,             /*in*/
					LLF_ECPKI_PKA_Point_t	*PAff_ptr,         /*out*/
					CRYS_ECPKI_DomainID_t   DomainID,         /*in*/ 
					DxInt8_t               *regTemps_ptr,     /*in*/
					DxUint32_t              countTemps,       /*in*/
					DxUint32_t              VirtualHwBaseAddr /*in*/ );					   

					   
					   
/**************************************************************************
 *	              LLF_ECPKI_GFpJacobianToAffine
 **************************************************************************/
/**
   @brief  This function converts GFp point from projective Jacobian to
           standard affine representation. 
   		   
   		   Functions algorithm (IEEE Std 1363-2000, A.9.6): 
   		      PJac(X,Y,Z) => PAff(x,y) 
              x = X * Z'^2 mod P, y = Y * Z'^3 mod P, where Z' = Z^-1  mod P.
               
   @param[in]  PJac              - A pointer to structure, containing the point 
   								   in projective Jacobian coordinates.
   @param[in]  PAff              - A pointer to structure, containing  point 
   								   converted into affine standard representation.
   @param[in]  ECPKI_DomainID    - EC domain identifier.      
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
   
   @return CRYSError_t - CRYS_OK, or Error from called functions.
                         
*/	

CRYSError_t  LLF_ECPKI_GFpJacobianToAffine(					   					   		   
				LLF_ECPKI_PKA_Point_t     *PJac,             /*in*/
				LLF_ECPKI_PKA_Point_t     *PAff,             /*out*/					   
				CRYS_ECPKI_DomainID_t	  DomainID, 	    /*in*/ 
				DxInt8_t                  *regTemps_ptr,     /*in*/
				DxUint32_t                 countTemps,       /*in*/
				DxUint32_t                 VirtualHwBaseAddr /*in*/ );
					   


/**************************************************************************
 *	              LLF_ECPKI_SetPoint function
 **************************************************************************/
/**
   @brief  This function sets projective coordinates of EC point .       
   		   
   @param[in]  X_ptr,Y_ptr,Z_ptr      - A pointers to coordinates.
   @param[in]  sizeInBWords - Size of coordinates in words.
   @param[in]  LenID   - ID of RegsSizesTab entry, containing exact modulus size.
   @param[in]  Res     - A pointer to structure, containing EC point coordinates.
   @param[in]  pointID - ID of Res point (enum).
   
   @return     No return value.  
   			   
	   Note: The function copies only ModSizeInWords words of
	   each coordinate  / not all reserved length of coordinates arrays  
	   (CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS words).  
*/
void   LLF_ECPKI_SetPoint( 
			DxUint32_t            *X_ptr,
			DxUint32_t            *Y_ptr,
			DxUint32_t            *Z_ptr,
			DxUint32_t             sizeInWords,
			DxUint32_t             LenID,
			LLF_ECPKI_PKA_Point_t *Res,
			CRYS_ECPKI_PointID_t   pointID,							 
			DxUint32_t             VirtualHwBaseAddr);


/**************************************************************************
 *	              LLF_ECPKI_CopyPoint function
 **************************************************************************/
/**
   @brief  This function copies one projective point into other.       
   		   
   @param[in]  PointFrom  - A pointer to structure, containing first point.
   @param[in]  PointTo    - A pointer to structure, containing second point.
   @param[in]  ModSizeInWords - Size of EC modulus in words.
   @return     No return value.  
   			   
   			   Note: The function copies only ModSizeInWords words of
   			   each coordinate  / not all reserved length of coordinates arrays  
   			   (CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS words).  
*/
void	LLF_ECPKI_CopyPoint( 
			DxUint32_t             LenID,
			LLF_ECPKI_PKA_Point_t *To,
			LLF_ECPKI_PKA_Point_t *From,
			DxUint32_t             VirtualHwBaseAddr );
 								 

 								 
/**************************************************************************
 *	               LLF_ECPKI_SetNullToProjPoint function
 **************************************************************************/
/**
   @brief  This function sets projective point to infinity that means sets
           point coordinates as following  X=1, Y=1, Z=0.    		   

   @param[in]  LenID - RegSizes entey, containing exact size of coordinates in bits.
   @param[in]  P  - A pointer to structure, containing the point.

   @return     No return value. 
*/

void  LLF_ECPKI_SetNullToPoint( DxUint32_t             LenID, 
                                LLF_ECPKI_PKA_Point_t *P,
                                DxUint32_t             VirtualHwBaseAddr );


/**************************************************************************
 *	              LLF_ECPKI_SetProjPointOnInfinity function
 **************************************************************************/
/**
   @brief  This function sets projective point to infinity that means sets
           point coordinates as following  X=1, Y=1, Z=0.    		   

   @param[in]  LenID - RegSizes entey, containing exact size of coordinates in bits.
   @param[in]  P  - A pointer to structure, containing the point.

   @return     No return value. 
*/

void	LLF_ECPKI_SetProjPointOnInfinity( DxUint32_t             LenID, 
                                          LLF_ECPKI_PKA_Point_t *P,
                                          DxUint32_t             VirtualHwBaseAddr );


/**************************************************************************
 *	              LLF_ECPKI_IsProjPointOnInfinity function
 **************************************************************************/
/**
   @brief  This function compares projective point coordinates X,Y,Z to infinity: 
            X=1, Y=1, Z=0.    		   
   
   @param[in]  LenID  - ID of RegSizesEntry, containing the exact modulus size.          
   @param[in]  P0  - A pointer to structure, containing the point.
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
   @return     if X=1 and Y=1 and Z=0, then return 1, otherwise return 0. 
*/

DxInt32_t	LLF_ECPKI_IsProjPointOnInfinity( 
                                             DxUint32_t              LenID,            /*in*/                                             
                                             LLF_ECPKI_PKA_Point_t  *P0,               /*in*/
                                             DxUint32_t              VirtualHwBaseAddr /*in*/ );
                                             
                                             
/**************************************************************************
 *	              LLF_ECPKI_CompareProjPointToNull
 **************************************************************************/
/**
   @brief  This function compares projective point coordinates X,Y,Z to 0: 
            X=0, Y=0, Z=0.    		   
               
   @param[in]  LenID  - ID of RegSizesEntry, containing the exact modulus size.          
   @param[in]  P0  - A pointer to structure, containing the point.
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 
   @return     1 if X=0 and Y=0 and Z=0,
               0 otherwice. 
*/

DxInt32_t	LLF_ECPKI_CompareProjPointToNull( 
                                             DxUint32_t              LenID,            /*in*/                                             
                                             LLF_ECPKI_PKA_Point_t  *P0,               /*in*/
                                             DxUint32_t              VirtualHwBaseAddr /*in*/ );                                             
                                             
                                             

/**********************************************************************************************************/

#ifdef __cplusplus
}
#endif			

#endif
