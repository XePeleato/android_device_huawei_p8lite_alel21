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
#include "plat_print_dbg.h"
#include "bsp.h"
#include "crys_common_math.h"
#include "llf_ecpki.h"
#include "crys_ecpki_types.h"
#include "crys_ecpki_error.h"
#include "llf_ecpki_modular_arithmetic.h"
#include "llf_ecpki_ec_arithmetic.h"

/* canceling the lint warning:
  Info 740: Unusual pointer cast 
  (incompatible indirect types) */
/*lint --e{740} */

/* canceling the lint warning:
   Info 826: Suspicious pointer-to-pointer conversion 
   (area too small) */
/*lint --e{826} */

#ifdef __cplusplus
extern "C"
{
#endif
 
/* Extern global data */
 extern const CRYS_ECPKI_Domain_t LLF_EC_DOMAINS_ARRAY [];


 
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
				DxInt8_t	   	       rn, 	         /*in*/
				LLF_ECPKI_PKA_Point_t         *P0,               /*in*/
				LLF_ECPKI_PKA_Point_t	      *Res,              /*out*/
				CRYS_ECPKI_DomainID_t	       DomainID,         /*in*/ 
				DxInt8_t                      *regTemps_ptr,     /*in*/
				DxUint32_t                     countTemps,       /*in*/
				DxUint32_t                     VirtualHwBaseAddr /*in*/)
{
   /* DECLARATIONS */
   
   /* the error identifier */
   CRYSError_t Error;
   
   /* virtual pointers to PKA temp registers */
   DxInt8_t  rZ;

 
   /* FUNCTION LOGIC */ 
   
   /* check temp registers */
#ifdef LLF_PKI_PKA_DEBUG   
   if( countTemps < 1 )
       return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#endif
   
   /*********** initializtions  ******************************/
  
    /* initialize the error identifier to OK */
   Error = CRYS_OK;

   /* set pointers to temporary registers */
   rZ = regTemps_ptr[0];
   
   /* convert P0 to projective Jacobian form by setting Z0 = 1  */
   if( P0->PointID == EC_PointAffine )
   {
	   P0->rZ = rZ;  /* add register for Z coordinate */
	   LLF_PKI_PKA_Clr( LenID_n+1, P0->rZ/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
	   LLF_PKI_PKA_Set0( LenID_n+1, P0->rZ/*OpA*/, P0->rZ/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	   P0->PointID  = EC_PointJacobian; 
   }
   
#ifdef LLF_PKI_PKA_DEBUG   
   else if( P0->PointID != EC_PointJacobian )   
       return LLF_ECPKI_INVALID_POINT_ID_ERROR;
#endif       

   /* set PointID of P0 and result to Jacobian */
   Res->PointID = EC_PointJacobian;    

   /* .........perform scalar multiplication by calling low level function ......... */
   Error = LLF_ECPKI_GFpScalarMultProj( rn, P0, Res, DomainID, 
                                        regTemps_ptr+1, countTemps-1, VirtualHwBaseAddr );  
   if( Error != CRYS_OK )   
	return Error;         
   
   /* normalize the result point according affine ordinary representation */
   Error = LLF_ECPKI_GFpJacobianToAffine( Res/*In*/, Res/*Out*/, DomainID, 
                                          regTemps_ptr+1, countTemps-1, VirtualHwBaseAddr );  
   if( Error != CRYS_OK )   
	return Error;      
   
   return Error;

}/* END OF LLF_ECPKI_ScalarMultAff function */                                       



/**************************************************************************
 *	              LLF_ECPKI_FullAddAff function                           *
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
			DxUint32_t                  VirtualHwBaseAddr /*in*/ )
{
   /* DECLARATIONS */
   
   /* the error identifier */
   CRYSError_t Error;
 
   /* virtual pointers to PKA temp registers */
   DxInt8_t  rZ1, rZ2; 
 
 
   /* FUNCTION LOGIC */    
   

   /* check temp registers */
#ifdef LLF_PKI_PKA_DEBUG   
   if( countTemps < 2 )
       return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#endif

   /*********** initializtions  ******************************/
   
    Error = CRYS_OK;
   
   /* set pointers to temporary registers */
   rZ1 = regTemps_ptr[0];
   rZ2 = regTemps_ptr[1];
   
   /* convert P1, P2 to projective Jacobian form by setting Z0 = 1  */
   if( P1->PointID == EC_PointAffine &&
       P2->PointID == EC_PointAffine )
   {
	   /* add registers for Z coordinates */
	   P1->rZ = rZ1;  
	   P2->rZ = rZ2;
	   /* set 1 into registers Z1,Z2 */
	   LLF_PKI_PKA_Clr( LenID_n+1, P1->rZ/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
	   LLF_PKI_PKA_Set0( LenID_n+1, P1->rZ/*OpA*/, P1->rZ/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	   LLF_PKI_PKA_Copy( LenID_n+1, P2->rZ/*dst*/, P1->rZ/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	   
	   P1->PointID  = EC_PointJacobian;
	   P2->PointID  = EC_PointJacobian;    	
   }
   
#ifdef LLF_PKI_PKA_DEBUG   
   else if( P1->PointID != EC_PointJacobian ||
            P2->PointID != EC_PointJacobian )   
       return LLF_ECPKI_INVALID_POINT_ID_ERROR;
#endif       

   /* set PointID of P0 and result to Jacobian */
   Res->PointID = EC_PointJacobian;    

   /* ....call low level Full Add function ......... */
   Error = LLF_ECPKI_FullAdd( P1, P2, Res, DomainID, 
                              regTemps_ptr+2, countTemps-2, VirtualHwBaseAddr );   	  
   if( Error != CRYS_OK )   
	   return Error;         
   
   /* convert the result point to affine ordinary representation */
   Error = LLF_ECPKI_GFpJacobianToAffine( Res/*in*/, Res/*out*/, DomainID, 
                                          regTemps_ptr+2, countTemps-2, VirtualHwBaseAddr );
   if( Error != CRYS_OK )   
		return Error;      

   return Error;
                               

}/* END OF LLF_ECPKI_Full Add Affine */                                       

  

 

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
			LLF_ECPKI_PKA_Point_t	  *Res,                 /*out*/
			CRYS_ECPKI_DomainID_t	   DomainID,            /*in*/ 
			DxInt8_t                  *regTemps_ptr,        /*in*/
			DxUint32_t                 countTemps,          /*in*/
			DxUint32_t                 VirtualHwBaseAddr    /*in*/ )
{

	/* DECLARATIONS */

	CRYSError_t Error = 0;

   /* virtual pointers to PKA temp registers */
   DxInt8_t  rT1, rT2, rT3, rT4, rT5, rT6, rT7; 
   
   /* PKA status */
   DxUint32_t  status;

 
   /* FUNCTION LOGIC */    
   
	/* Check input parameters */
#ifdef LLF_PKI_PKA_DEBUG

	if( countTemps < 7 )
		return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
	
	if(DomainID >= CRYS_ECPKI_DomainID_OffMode)
		return  LLF_ECPKI_GFP_ADD_MONT_INVALID_DOMAIN_ID_ERROR;
	
	if(P1->PointID != EC_PointJacobian || P2->PointID != EC_PointJacobian)
		return  LLF_ECPKI_GFP_ADD_MONT_INVALID_POINTS_ID_ERROR;	
#endif
	

	/**** Initialization of variables and pointers ****/

	Error = CRYS_OK;
	DomainID = DomainID;
    countTemps = countTemps;
	/* Set PointID to tRes */
	Res->PointID = EC_PointJacobian;
		
	/* virtual pointers to temp PKA registers */
	rT1 = regTemps_ptr[0];
	rT2 = regTemps_ptr[1];
	rT3 = regTemps_ptr[2];
	rT4 = regTemps_ptr[3];
	rT5 = regTemps_ptr[4];
	rT6 = regTemps_ptr[5];
	rT7 = regTemps_ptr[6];

	 
	/*******************************************************************************
	 *          Performing of points adding operations                             *
	 *    Numbers of operations according to algorithm IEEE Std 1363-2000: A.10.5. *                    
	 *******************************************************************************/	 
		 
	/* 1. T1 = X1 */
	LLF_PKI_PKA_Copy( LenID_n+1, rT1/*dst*/, P1->rX/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 2. T2 = Y2 */
	LLF_PKI_PKA_Copy( LenID_n+1, rT2/*dst*/, P1->rY/*src*/, 0/*Tag*/, VirtualHwBaseAddr );  
	/* 3. T3 = Z3 */ 
	LLF_PKI_PKA_Copy( LenID_n+1, rT3/*dst*/, P1->rZ/*src*/, 0/*Tag*/, VirtualHwBaseAddr ); 
	
	/* 4. T4 = X2 */
	LLF_PKI_PKA_Copy( LenID_n+1, rT4/*dst*/, P2->rX/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 5. T5 = Y2*/
	LLF_PKI_PKA_Copy( LenID_n+1, rT5/*dst*/, P2->rY/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* 6. Do if Point2 is projective (Z2 != 1)*/

	LLF_PKI_PKA_CompareIm( LenID_n, P2->rZ/*OpA*/, 1/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);    
	if( status != 1 )
	{
		/* T6 = Z2*/
		LLF_PKI_PKA_Copy( LenID_n+1, rT6/*dst*/, P2->rZ/*src*/, 0/*Tag*/, VirtualHwBaseAddr );		
		/* T7 = T6^2 */
		LLF_PKI_PKA_ModMul( LenID_n, rT6/*OpA*/, rT6/*OpB*/, rT7/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		/* T1 = T1*T7 */
		LLF_PKI_PKA_ModMul( LenID_n, rT1/*OpA*/, rT7/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		/* T7 = T6*T7 */
		LLF_PKI_PKA_ModMul( LenID_n, rT6/*OpA*/, rT7/*OpB*/, rT7/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		/* T2 = T2*T7 */
		LLF_PKI_PKA_ModMul( LenID_n, rT2/*OpA*/, rT7/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	} /* end if( Point2 is projective ) */	
	
	/* 7. T7 = T3^2 */
	LLF_PKI_PKA_ModMul( LenID_n, rT3/*OpA*/, rT3/*OpB*/, rT7/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 8. T4 = T4*T7 */
	LLF_PKI_PKA_ModMul( LenID_n, rT4/*OpA*/, rT7/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 9. T7 = T3*T7 */
	LLF_PKI_PKA_ModMul( LenID_n, rT3/*OpA*/, rT7/*OpB*/, rT7/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 10. T5 = T5*T7 */
	LLF_PKI_PKA_ModMul( LenID_n, rT5/*OpA*/, rT7/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 11. T4 = T1 - T4 */
	LLF_PKI_PKA_ModSub( LenID_n, rT1/*OpA*/, rT4/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                                			                                    			
	/* 12. T5 = T2 - T5 */
	LLF_PKI_PKA_ModSub( LenID_n, rT2/*OpA*/, rT5/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                                			                                    			
	
	/* 13. Check is the result point the point on infinity or two input points 
	are the same. Output appropriate values of result point coordinates */       
	
	/* If T4 == 0 */     
	LLF_PKI_PKA_CompareIm( LenID_n, rT4/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);    
	if( status == 1 )    
	{
		/* If (T5 == 0) then  output PointRes = (0,0,0) and stop, that means: 
		use doubling instead add operation */
		LLF_PKI_PKA_CompareIm( LenID_n, rT5/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);	    
		
		if( status == 1 ){
			/* Set Null to result point */
			LLF_ECPKI_SetNullToPoint( LenID_n, Res, VirtualHwBaseAddr );
			return Error /*LLF_ECPKI_GFpAddMont_ThePointsAreIdentic_ERROR*/ ;
		}
		else{ 
			/* output PointRes = (1,1,0) and stop, that means: result on infinity */
			LLF_ECPKI_SetProjPointOnInfinity( LenID_n, Res, VirtualHwBaseAddr ); 
			return Error /*LLF_ECPKI_GFpAddMont_ResultOnInfinity_ERROR*/ ;
		}    	  	
	}
	  
	/* 14.  T1 = 2*T1 - T4 */
	LLF_PKI_PKA_ModAdd( LenID_n, rT1/*OpA*/, rT1/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModSub( LenID_n, rT1/*OpA*/, rT4/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* 15.  T2 = 2*T2 - T5 */
	LLF_PKI_PKA_ModAdd( LenID_n, rT2/*OpA*/, rT2/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModSub( LenID_n, rT2/*OpA*/, rT5/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );    
	
	/* 16. If (Z2 != 1) then T3 = T3*T6  */    
	LLF_PKI_PKA_CompareIm( LenID_n, P2->rZ/*OpA*/, 1/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);	    
	if( status != 1 ){
		/* 16a. T3 = T3*T6 */
		LLF_PKI_PKA_ModMul( LenID_n, rT3/*OpA*/, rT6/*OpB*/, rT3/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );		
	}
    
	/* 17. T3 = T3*T4 */
	LLF_PKI_PKA_ModMul( LenID_n, rT3/*OpA*/, rT4/*OpB*/, rT3/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 18. T7 = T4^2 */
	LLF_PKI_PKA_ModMul( LenID_n, rT4/*OpA*/, rT4/*OpB*/, rT7/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 19. T4 = T4*T7 */
	LLF_PKI_PKA_ModMul( LenID_n, rT4/*OpA*/, rT7/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 20. T7 = T1*T7 */
	LLF_PKI_PKA_ModMul( LenID_n, rT1/*OpA*/, rT7/*OpB*/, rT7/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 21. T1 = T5^2 */
	LLF_PKI_PKA_ModMul( LenID_n, rT5/*OpA*/, rT5/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 22. T1 = T1 - T7 */                                			                                    			
	LLF_PKI_PKA_ModSub( LenID_n, rT1/*OpA*/, rT7/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 23.  T7 = T7 - 2*T1 */
	LLF_PKI_PKA_ModSub( LenID_n, rT7/*OpA*/, rT1/*OpB*/, rT7/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModSub( LenID_n, rT7/*OpA*/, rT1/*OpB*/, rT7/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 24. T5 = T5*T7 */
	LLF_PKI_PKA_ModMul( LenID_n, rT5/*OpA*/, rT7/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 25. T4 = T2*T4 */
	LLF_PKI_PKA_ModMul( LenID_n, rT2/*OpA*/, rT4/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 26. T2 = T5 - T4 */
	LLF_PKI_PKA_ModSub( LenID_n, rT5/*OpA*/, rT4/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                                			                                    			
	/* 27. T2 = T2/2   */
	LLF_PKI_PKA_ModDivideBy2( LenID_n, rT2, 0/*mod reg*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
						
	/* 28, 29, 30  - Copy result into output point */
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rX/*dst*/, rT1/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rY/*dst*/, rT2/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rZ/*dst*/, rT3/*src*/, 0/*Tag*/, VirtualHwBaseAddr );    
	
	return Error;
		
} /* end of function */
	


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
			DxUint32_t                 VirtualHwBaseAddr    /*in*/ )
{
	/*Local variables */
	CRYSError_t Error;	
	
	/* virtual pointers to PKA temp registers */
	DxInt8_t  rT1, rT2, rT3, rT4, rT5, rT6; 
	
	/* PKA status */
	DxUint32_t  status1, status2;
	
	/* EC domain parameters */
	DxUint32_t modSizeInWords;
	
	
	/* FUNCTION LOGIC */    
	
	/* Check input parameters */
#ifdef LLF_PKI_PKA_DEBUG

	if(countTemps < 6)
		return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
	
	if(DomainID >= CRYS_ECPKI_DomainID_OffMode)
		return  LLF_ECPKI_GFP_ADD_MONT_INVALID_DOMAIN_ID_ERROR;
	
	if(P0->PointID != EC_PointJacobian)	
		return  LLF_ECPKI_GFP_ADD_MONT_INVALID_POINTS_ID_ERROR;	
#endif
	

	/**** Initialization of variables and pointers ****/

	Error = CRYS_OK;
	countTemps = countTemps; 	
	/* Set PointID to tRes */
	Res->PointID = EC_PointJacobian;
		
	/* virtual pointers to temp PKA registers */
	rT1 = regTemps_ptr[0];
	rT2 = regTemps_ptr[1];
	rT3 = regTemps_ptr[2];
	rT4 = regTemps_ptr[3];
	rT5 = regTemps_ptr[4];
	rT6 = regTemps_ptr[5];
	
	/* EC domain parameters */
	modSizeInWords = (LLF_EC_DOMAINS_ARRAY[DomainID].EC_ModulusSizeInBits + 31)/32;

	/* copy EC parameter A into register rT4 */ 
	LLF_PKI_PKA_CopyDataIntoPkaReg( rT4/*dstReg*/, LenID_n+1, 
		(DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_Param_A/*src_ptr*/,
		modSizeInWords, VirtualHwBaseAddr );   
	
	/*******************************************************************************
	 *          Performing of points doubling operations                           *
	 *    Numbers of operations according to algorithm IEEE Std 1363-2000: A.10.4. *                   
	 *******************************************************************************/
	 
	/* 1. T1 = X0 */
	LLF_PKI_PKA_Copy( LenID_n+1, rT1/*dst*/, P0->rX/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 2. T2 = Y0 */ 
	LLF_PKI_PKA_Copy( LenID_n+1, rT2/*dst*/, P0->rY/*src*/, 0/*Tag*/, VirtualHwBaseAddr ); 
	/* 3. T3 = Z0 */  
	LLF_PKI_PKA_Copy( LenID_n+1, rT3/*dst*/, P0->rZ/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
			
	/* 4. if T2 = 0 or T3 = 0 then output (1,1,0) and stop */
  
	LLF_PKI_PKA_CompareIm( LenID_n, rT2/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);
	LLF_PKI_PKA_CompareIm( LenID_n, rT3/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status2);
	
	if( status1 == 1 || status2 == 1 ){
		/* set point on infinity */
		LLF_ECPKI_SetProjPointOnInfinity( LenID_n, Res, VirtualHwBaseAddr );    	
		return Error;
	}
	  
	/* 5. If (T5 = (P-A) = 3) then      / equal to if (A = P - 3) then */  
	
	/* 5.1. T5 = P - A */ 
	LLF_PKI_PKA_Sub( LenID_n, 0/*mod: OpA*/, rT4/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
    
	/* 5.2. if (T5 = 3) */ 
	LLF_PKI_PKA_CompareIm( LenID_n, rT5/*OpA*/, 3/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);
	
	if( status1 == 1 ) 
	{
		/* 5.3. T4 = T3^2 */
		LLF_PKI_PKA_ModMul( LenID_n, rT3/*OpA*/, rT3/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		/* 5.4. T5 = T1 - T4 */                                			                                    			
		LLF_PKI_PKA_ModSub( LenID_n, rT1/*OpA*/, rT4/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );    
		/* 5.5. T4 = T1 + T4 */                            			                                   			
		LLF_PKI_PKA_ModAdd( LenID_n, rT1/*OpA*/, rT4/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );    
		/* 5.6. T5 = T4*T5 */
		LLF_PKI_PKA_ModMul( LenID_n, rT4/*OpA*/, rT5/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		/* 5.7. T4 = 3*T5 */
		LLF_PKI_PKA_ModAdd( LenID_n, rT5/*OpA*/, rT5/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_ModAdd( LenID_n, rT4/*OpA*/, rT5/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );    	
	}
	
	else
	{
		/* 5.8. T4 = A  done */
		/* 5.9. T5 = T3^2 */
		LLF_PKI_PKA_ModMul( LenID_n, rT3/*OpA*/, rT3/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                                 			
		/* 5.10. T5 = T5^2 */
		LLF_PKI_PKA_ModMul( LenID_n, rT5/*OpA*/, rT5/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                                 			
		/* 5.11. T5 = T4*T5 = A*T5 */
		LLF_PKI_PKA_ModMul( LenID_n, rT4/*OpA*/, rT5/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                                 			
		/* 5.12. T4 = T1^2 */
		LLF_PKI_PKA_ModMul( LenID_n, rT1/*OpA*/, rT1/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		/* 5.13. T4 = 3*T4: two steps   T6 = 2*T4 ; T4 = T4 + T6 where X2 is used temporary */
		LLF_PKI_PKA_ModAdd( LenID_n, rT4/*OpA*/, rT4/*OpB*/, rT6/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_ModAdd( LenID_n, rT6/*OpA*/, rT4/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );    	
		/* 5.14. T4 = T4 + T5 */ 
		LLF_PKI_PKA_ModAdd( LenID_n, rT4/*OpA*/, rT5/*OpB*/, rT4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                           			                                   			
	}

	/* 6. T3 = T2*T3 */
	LLF_PKI_PKA_ModMul( LenID_n, rT2/*OpA*/, rT3/*OpB*/, rT3/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 7. T3_ptr = 2*T3 */ 
	LLF_PKI_PKA_ModAdd( LenID_n, rT3/*OpA*/, rT3/*OpB*/, rT3/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                           			                                   			
	/* 8. T2 = T2^2 */
	LLF_PKI_PKA_ModMul( LenID_n, rT2/*OpA*/, rT2/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 9. T5 = T1*T2 */
	LLF_PKI_PKA_ModMul( LenID_n, rT1/*OpA*/, rT2/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                            			
	/* 5.10. T5 = 4*T5: */
	LLF_PKI_PKA_ModAdd( LenID_n, rT5/*OpA*/, rT5/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModAdd( LenID_n, rT5/*OpA*/, rT5/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 11. T1 = T4^2 */
	LLF_PKI_PKA_ModMul( LenID_n, rT4/*OpA*/, rT4/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 12.  T1 = T1 - 2*T5 */
	LLF_PKI_PKA_ModSub( LenID_n, rT1/*OpA*/, rT5/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModSub( LenID_n, rT1/*OpA*/, rT5/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );	
	/* 13. T2 = T2^2 */
	LLF_PKI_PKA_ModMul( LenID_n, rT2/*OpA*/, rT2/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 14.  T2 = 8*T2 */
	LLF_PKI_PKA_ModAdd( LenID_n, rT2/*OpA*/, rT2/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModAdd( LenID_n, rT2/*OpA*/, rT2/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModAdd( LenID_n, rT2/*OpA*/, rT2/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                        			                                   			
	/* 15. T5 = T5 - T1 */
	LLF_PKI_PKA_ModSub( LenID_n, rT5/*OpA*/, rT1/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 16. T5 = T4*T5 */
	LLF_PKI_PKA_ModMul( LenID_n, rT4/*OpA*/, rT5/*OpB*/, rT5/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	/* 17. T2 = T5 - T2  */ 
	LLF_PKI_PKA_ModSub( LenID_n, rT5/*OpA*/, rT2/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                               			                                    			
	
	/* 18, 19, 20  - Copy result into output point PointRes */    
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rX/*dst*/, rT1/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rY/*dst*/, rT2/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rZ/*dst*/, rT3/*src*/, 0/*Tag*/, VirtualHwBaseAddr );

	return Error;
		
} /* end of function */
	

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
			LLF_ECPKI_PKA_Point_t	  *Res,                 /*out*/
			CRYS_ECPKI_DomainID_t	   DomainID,            /*in*/ 
			DxInt8_t                  *regTemps_ptr,        /*in*/
			DxUint32_t                 countTemps,          /*in*/
			DxUint32_t                 VirtualHwBaseAddr    /*in*/ )
{
	
	/*  LOCAL DECLARATIONS */
	
	CRYSError_t Error;
	
	/* PKA status */
	DxUint32_t  status;	
	
	/* Check input parameters */
	if(DomainID == CRYS_ECPKI_DomainID_OffMode)
		return  LLF_ECPKI_GFP_FULL_ADD_INVALID_DOMAIN_ID_ERROR;
	
	if( P1->PointID   != EC_PointJacobian || 
	    P2->PointID   != EC_PointJacobian  )	
		return  LLF_ECPKI_GFP_FULL_ADD_POINTS_NOT_JACOBIAN_ERROR;	
	
	/* FUNCTION LOGIC */
	
	/******** Initialization of variables and pointers **********/
	
	Error = CRYS_OK;
	
	/* Set PointID to PointRes */
	Res->PointID = EC_PointJacobian; 	
	
	/********************************************************************************
	 * Performing of Full points addition  by switching to appropriate LLF function *
	 *    according to algorithm IEEE Std 1363-2000: A.10.8.                        *
	 ********************************************************************************/	 
	
	/********************************************************************************
	 * 1. If Z1 = 0, then output (X3,Y3,Z3) = (X2,Y2,Z2) and stop   /P3 is Res      * 
	 ********************************************************************************/
	LLF_PKI_PKA_CompareIm( LenID_n, P1->rZ/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	
	if( status == 1 ){
	    LLF_ECPKI_CopyPoint( LenID_n+1, Res, P2, VirtualHwBaseAddr);
		return Error;	
	}	  		
	
	/********************************************************************************
	 * 2. If Z2 = 0, then output (X3,Y3,Z3) = (X1,Y1,Z1) and stop  /P3 is Res       * 
	 ********************************************************************************/
	LLF_PKI_PKA_CompareIm( LenID_n, P2->rZ/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	
	if( status == 1 ){
		LLF_ECPKI_CopyPoint( LenID_n+1, Res, P1, VirtualHwBaseAddr);
		return Error;	
	}	  		
	
	/********************************************************************************
	 * 3. Perform Add operation, if Res = (0,0,0), then perform EC Doubling         * 
	 ********************************************************************************/
     
	Error = LLF_ECPKI_GFpAddProj( P1, P2, Res, DomainID, 
	                          regTemps_ptr, countTemps, VirtualHwBaseAddr );
	if(Error != CRYS_OK)
		return Error;

	/* 4. If (X3,Y3,Z3) = (0,0,0) then perform doubling instead adding operation */	                               
	if( LLF_ECPKI_CompareProjPointToNull( LenID_n, Res, VirtualHwBaseAddr ) == 1 )
     
    	Error = LLF_ECPKI_GFpDoubleProj( P1, Res, DomainID, 
	                               regTemps_ptr, countTemps, VirtualHwBaseAddr );	
   
	return Error;	

} /* end of function */
	

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
CRYSError_t  LLF_ECPKI_SubtractProj(					   					   
			LLF_ECPKI_PKA_Point_t     *P1,                  /*in*/    		   
			LLF_ECPKI_PKA_Point_t     *P2,                  /*in*/
			LLF_ECPKI_PKA_Point_t	  *Res,                 /*out*/
			CRYS_ECPKI_DomainID_t	   DomainID,            /*in*/ 
			DxInt8_t                  *regTemps_ptr,        /*in*/
			DxUint32_t                 countTemps,          /*in*/
			DxUint32_t                 VirtualHwBaseAddr    /*in*/ )
{	
	/*Local variables */
	CRYSError_t Error;

	/* virtual pointers to PKA registers */
	DxInt8_t  r_Y;	

    /* check temp registers */
#ifdef LLF_PKI_PKA_DEBUG   
	if( countTemps < 1 )
		return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#endif


	/******** Initialization of variables and pointers **********/
	
	Error = CRYS_OK;	
	
	/* set virtual pointers	 */
	r_Y = regTemps_ptr[0];
	
	
	/********************************************************************************
	 *   Calculate  minus Y2  according to algorithm IEEE Std 1363-2000: A.10.8.    *
	 ********************************************************************************/	 
	
	/* If field modulus is prime (GFp) set coordinate Y of TempPoint1 to -Y1 
	   (it is equal to P-Y1) */   
	if( LLF_EC_DOMAINS_ARRAY[DomainID].FieldID == GFp ){
		LLF_PKI_PKA_ModSub( LenID_n, 0/*OpA=mod*/, P2->rY/*OpB*/, r_Y/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	}
	/* else if field modulus is base 2 (GF2^m) set coordinate Y of P2: Y = (X2*Z2 + Y2) */   
	else{
		LLF_PKI_PKA_ModMul( LenID_n, P2->rX/*OpA*/, P2->rZ/*OpB*/, r_Y/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_ModMul( LenID_n, r_Y/*OpA*/, P2->rY/*OpB*/, r_Y/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );                                
	}
	
	/* swap pointers, that mean negate P2 */
	LLF_PKI_SwapInt8( P2->rY, r_Y );
	
	/* Call the LLF_ECPKI_FullAdd function */
	
	Error = LLF_ECPKI_FullAdd ( P1, P2, Res, DomainID, 
				    regTemps_ptr+1, countTemps-1, VirtualHwBaseAddr );
						    
	/* restore Y2 */
	LLF_PKI_SwapInt8( P2->rY, r_Y );					            
	
	
	return Error;
		
} /* end of function */
					   
					   

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
			LLF_ECPKI_SCALAR_MULT_INVALID_DOMAIN_ID_ERROR,						   
			LLF_ECPKI_ScalarMult_INVALID_POINTS_ID_ERROR,
			LLF_ECPKI_ScalarMult_ARITHM_OPERATIONS_OFF_MODE_ERROR  
*/
	
CRYSError_t  LLF_ECPKI_GFpScalarMultProj(
				DxInt8_t	   	      rn, 	   	/*in*/
				LLF_ECPKI_PKA_Point_t         *P0,              /*in*/
				LLF_ECPKI_PKA_Point_t	     *Res,              /*out*/
				CRYS_ECPKI_DomainID_t	      DomainID,         /*in*/ 
				DxInt8_t                     *regTemps_ptr,     /*in*/
				DxUint32_t                    countTemps,       /*in*/
				DxUint32_t                    VirtualHwBaseAddr /*in*/ )
{	
	/***** Local variables *****/
	
	CRYSError_t Error;	
	
	
	DxInt32_t  i; /* indice */
	
	/* virtual pointers to: K= Sc, H = 3*K and temp T */
	DxInt8_t  rK, rH;
	/* virtual pointers to: -QY and temp T */
	DxInt8_t  r_QY, rT;
	
	/* auxiliary point Q in */
	LLF_ECPKI_PKA_Point_t Q;
	
	/* Size of k and h in words and in bits */
	DxUint32_t  hSizeInBits;
		
	/* Auxiliary variables for shift and bit operations */
	DxUint32_t  hi, ki;  
	
	/* PKA status indicies */
	DxUint32_t  status1, status2; 	
	
	
	/**** Check input parameters   *****/
	
#ifdef LLF_PKI_PKA_DEBUG
	
	if(DomainID == CRYS_ECPKI_DomainID_OffMode)
		return  LLF_ECPKI_SCALAR_MULT_INVALID_DOMAIN_ID_ERROR;
	
	if( countTemps < 7 )
		return  LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;   
	
	if( P0->PointID   != EC_PointJacobian )	
		return  LLF_ECPKI_SCALAR_MULT_POINTS_NOT_JACOBIAN_ERROR;	
#endif	        	
	
	/******** Initialization of variables and pointers **********/
	
	Error = CRYS_OK;
	
	/* set registers pointers */   
	rK   = regTemps_ptr[0];
	rH   = regTemps_ptr[1];
	Q.rX = regTemps_ptr[2]; 
	Q.rY = regTemps_ptr[3]; 
	Q.rZ = regTemps_ptr[4];
	r_QY = regTemps_ptr[5]; 
	rT   = regTemps_ptr[6]; /* note: this register may be used in regTemps list for functions call */
	
	/* Set PointID-s  */
	Q.PointID = EC_PointJacobian;
	Res->PointID = EC_PointJacobian;
	
 #ifdef LLF_PKI_PKA_DEBUG 
   
	{
		/* EC order size in words */
		DxUint32_t  rSizeInWords;	
		
		/* size of EC order */
		rSizeInWords = LLF_EC_DOMAINS_ARRAY[DomainID].EC_OrderSizeInWords;
		
		/**  Set order R into register rT    **/    
		LLF_PKI_PKA_CopyDataIntoPkaReg( rT/*dstReg*/, LenID_r+1, 
			      (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_GenerOrder/*src_ptr*/, 
			      rSizeInWords, VirtualHwBaseAddr );
		
		/* Check that scalar n <= Ord */
		
		LLF_PKI_PKA_Sub( LenID_r, rT/*OpA= mod*/, rn/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status1);
		if( status1 == 0 )
			return LLF_ECPKI_INVALID_SCALAR_IN_POINT_MULT_ERROR ;
	}
 #endif   
     

	/********************************************************************************
	 * Perform  scalar multiplication operations  according to algorithm            *                       
	 *          IEEE Std 1363-2000: A.10.9.                                         *
	 ********************************************************************************/	 
	
	/** 1. If n = 0 or Z0 = 0, then output Res = (1,1,0) and stop             **/
	LLF_PKI_PKA_CompareIm( LenID_r, rn/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);
	
	LLF_PKI_PKA_CompareIm( LenID_n, P0->rZ/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status2);
	
	if( status1 == 1 || status2 == 1 ){
		LLF_ECPKI_SetProjPointOnInfinity( LenIDn, Res, VirtualHwBaseAddr); 
		return Error;	
	}	  		
	
	/** 2. SetResX = X0, ResY = Y0, ResZ = Z0, r_QY = -QY  **/	
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rX/*dst*/, P0->rX/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rY/*dst*/, P0->rY/*src*/, 0/*Tag*/, VirtualHwBaseAddr );  
	LLF_PKI_PKA_Copy( LenID_n+1, Res->rZ/*dst*/, P0->rZ/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/** calculate r_QY = -Y0 = (mod - Y0) for changing sign of Q point **/
	LLF_PKI_PKA_Sub( LenID_n, 0/*OpA= mod*/, P0->rY/*OpB*/, r_QY/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/** 3. If ResZ = 1, then set QX=X0, QY= Y0, QZ=1, else normalize: QX = X0/(Z0^2),  QY = Y0/(Z0^3) */
	LLF_PKI_PKA_CompareIm( LenID_n, Res->rZ/*OpA*/, 1/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);
	if( status1 == 1 )
	{
		LLF_PKI_PKA_Copy( LenID_n+1, Q.rX/*dst*/, P0->rX/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
		LLF_PKI_PKA_Copy( LenID_n+1, Q.rY/*dst*/, P0->rY/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
		/* QZ = 1 */
		LLF_PKI_PKA_Copy( LenID_n+1, Q.rZ/*dst*/, P0->rZ/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	}
	
	else  /* normalize X,Y of Q by call the LLF_ECPKI_GFpJacobianToAffine */
	{
		LLF_ECPKI_GFpJacobianToAffine( Res/*in*/, &Q/*out*/, DomainID, 
					       regTemps_ptr+7, countTemps-7, VirtualHwBaseAddr );
	}
	
	/** 4. since n > 0, then K=n, QY = Y0. Calculations for negative n (K= -n, QY= -Y0) are not needed **/
	LLF_PKI_PKA_Copy( LenID_r+1, rK/*dst*/, rn/*src*/, 0/*Tag*/, VirtualHwBaseAddr );  	
	
	/** 5. Calculate  H = 3k  **/
	LLF_PKI_PKA_Add( LenID_r+1, rK/*OpA*/, rK/*OpB*/, rH/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Add( LenID_r+1, rH/*OpA*/, rK/*OpB*/, rH/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/*  6. Calculate size of h in bits: hSizeInBits */
	hSizeInBits = LLF_PKI_PKA_GetRegEffectiveSizeInBits( LenID_r, rH/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* 7. Perform loop  of double - add - subtract operations according to sequence of bit  
	       values of h and k */	
	for(i = (DxInt32_t)hSizeInBits - 2; i >= 1; i--)
	{
		/* 7.1. Set Res = 2*Res  / Double  */
		Error = LLF_ECPKI_GFpDoubleProj( Res, Res, DomainID, 
		                                 regTemps_ptr+7, countTemps-7, VirtualHwBaseAddr );
		if(Error != CRYS_OK)
			return Error;

		/* Get bits number i of h and k:  hi, ki */ 
		hi = LLF_PKI_PKA_GetBitFromPkaReg( rH, LenID_r, i, rT, VirtualHwBaseAddr );
		ki = LLF_PKI_PKA_GetBitFromPkaReg( rK, LenID_r, i, rT, VirtualHwBaseAddr );
		
		/* 7.2. If hi = 1 and ki = 0 set Res = Res + Q  /FullAdd */
		if( hi == 1 && ki == 0)
		{
			Error = LLF_ECPKI_FullAdd( Res, &Q, Res, DomainID, regTemps_ptr+7, countTemps-7, VirtualHwBaseAddr );         					                   
		
			if(Error != CRYS_OK)
				return Error;
		}

		/* 7.3. If hi = 0 and ki = 1 set Res = Res - Q  /Subtract */
		if( hi == 0 && ki == 1)
		{
			/* negate point Q: QY = -QY */
			LLF_PKI_SwapInt8( Q.rY, r_QY );
			
			Error = LLF_ECPKI_FullAdd( Res, &Q, Res, DomainID, regTemps_ptr+7, countTemps-7, VirtualHwBaseAddr );
			
			/* restore QY */
			LLF_PKI_SwapInt8( Q.rY, r_QY );
			
			if(Error != CRYS_OK)
				return Error;
		}
		

        WATCH_DOG_RESET();

	}


	return Error;
		
} /* end of function */


					   
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
			DxUint32_t   	       countTemps,       /*in*/
			DxUint32_t   	       VirtualHwBaseAddr /*in*/ )

{
	/* LOCAL DECLARATIONS */
	
	/* Field modulus P sizes */
	DxUint32_t	modSizeInWords;	
	
	/* Virtual pointers to EC coefficients A,B and temps T1, T2 */	
	DxInt8_t rA, rT1, rT2;
	
	DxUint32_t  status;
	
	/* check count of temp registers used */
#ifdef LLF_PKI_PKA_DEBUG   
	if( countTemps < 3 )
		return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#endif	
  
	/*  FUNCTION LOGIC */
	countTemps = countTemps;

	/******** Initialization of variables and pointers **********/
	
	/* Domain parameters */
	modSizeInWords =  LLF_EC_DOMAINS_ARRAY[DomainID].EC_ModulusSizeInWords;
	
	/* Pointers to temp buffers */
	rT1 = regTemps_ptr[0];
	rT2 = regTemps_ptr[1];
	rA  = regTemps_ptr[2];
	
	/* set EC parameters A,B into PKA registers */
	
	LLF_PKI_PKA_CopyDataIntoPkaReg( rA/*dstReg*/, LenID_n+1, 
		      (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_Param_A/*src_ptr*/, 
		      modSizeInWords , VirtualHwBaseAddr );
	
	LLF_PKI_PKA_CopyDataIntoPkaReg( rT2/*dstReg*/, LenID_n+1, 
		      (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_Param_B/*src_ptr*/, 
		      modSizeInWords , VirtualHwBaseAddr );
	
	
	/******** Performing operations ****************/
	
	/* 1. Calculate T1 = X^3+AX+B = (X^2+A)X+B  */
	
	/* T1 = X^2 */ 
	LLF_PKI_PKA_ModMul( LenID_n, W_ptr->rX/*OpA*/, W_ptr->rX/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
	/* T1 = T1 + A */
	LLF_PKI_PKA_ModAdd( LenID_n, rT1/*OpA*/, rA/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);    
	/* T1 = T1*X */                                              
	LLF_PKI_PKA_ModMul( LenID_n, rT1/*OpA*/, W_ptr->rX/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);  
	/* add B */
	LLF_PKI_PKA_ModAdd( LenID_n, rT1/*OpA*/, rT2/*OpB = B*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
	
	/* 2. Calculate T2 = Y^2  */
	LLF_PKI_PKA_ModMul( LenID_n, W_ptr->rY/*OpA*/, W_ptr->rY/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);  
	
	/* 3. Compare: if T1 != T2, return Error:  */
	LLF_PKI_PKA_Compare( LenID_n , rT1/*OpA*/, rT2/*OpB*/, 0/*Tag*/, VirtualHwBaseAddr);
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	
	if( status == 1)
		return CRYS_OK;	
	
	else
		return LLF_ECPKI_EC_POINT_IS_NOT_ON_CURVE_ERROR;
	   
} /* End of LLF_ECPKI_IsPointOnCurve function */



/**************************************************************************
 *	              LLF_ECPKI_GFp_ConvArrayToECaffine function
 **************************************************************************/
/** 
   @brief  This function converts the input array Arr into affine point PAff on 
	elliptic curve. The function calculates right part of curve equation 
	with point coordinate X and than calculates Y as square root from the right part.
	The function doing the following:		   
		1. Copies array data into previously nulled buffer X of  EC modulus size. 
		2. Calculates  R = (X^3 + A*X + B) mod P, where P - modulus, A,B - EC parameters.
		3. Calculates Y = R^(-2) mod P by calling square root function.
        	4. If square root is not exist, changes the MS byte += 1 and
        	goes to step 2.
		   Note: The changed byte is the high byte for big endian representation of the input array.  
		5. If count of tries is greater than 0x100, then the function returns an error message.   		  
               
   @param[in]  rArr           - A virtual pointer to register, containing the data to be converted.
   @param[in]  PAff_ptr       - A pointer to output point.
   @param[in]  DomainID       - EC domain identifier. 
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user. 

   @return     Error.  

         NOTE: 1. Only first (EC_modulus_size - 1) low bytes of array may be
        		  recovered right from converted point.
*/
CRYSError_t  LLF_ECPKI_GFp_ConvArrayToECaffine(
					DxInt8_t 	        rArr,             /*in*/
					LLF_ECPKI_PKA_Point_t	*PAff_ptr,         /*out*/
					CRYS_ECPKI_DomainID_t   DomainID,         /*in*/ 
					DxInt8_t               *regTemps_ptr,     /*in*/
					DxUint32_t              countTemps,       /*in*/
					DxUint32_t              VirtualHwBaseAddr /*in*/ )

{
	/* LOCAL DECLARATIONS */

 	CRYSError_t Error;	

	/* modulus P and its size */
	DxUint32_t *P_ptr;
	DxUint32_t	modSizeInBytes, modSizeInWords;
	
	/* value for adding to changed word */
	DxUint32_t	addValue;	

	/* Virtual pointers to temporary registers */
	DxUint32_t rT1, rA, rB;
	
	/* Count of tries to convert array to EC point */
	DxUint32_t   tries;
	DxBool_t     isRootExists;	

	/* changed word and byte of the message */
	DxUint32_t    changedWord, changedWordNum, changedByteNum;
	
	
	
	/******** Initialization of variables and pointers **********/

	Error = CRYS_OK;	


    /* check count of temp registers used */
 #ifdef LLF_PKI_PKA_DEBUG   
    if( countTemps < 3 )
        return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
 #endif	

	/* Domain parameters */
	P_ptr = (DxUint32_t *)LLF_EC_DOMAINS_ARRAY[DomainID].GF_Modulus;
	modSizeInBytes = (LLF_EC_DOMAINS_ARRAY[DomainID].EC_ModulusSizeInBits+7)/8;
	modSizeInWords = (modSizeInBytes + 3) / 4;	

	/* Pointers to temp buffers */
	rT1 = regTemps_ptr[0];
	rA  = regTemps_ptr[1];
	rB  = regTemps_ptr[2];
    	
	isRootExists = DX_FALSE;
	
	/*********  FUNCTION LOGIC  ************/
	
	/* set A and B into PKA registers */
	LLF_PKI_PKA_CopyDataIntoPkaReg( rA/*dstReg*/, LenID_n+1, 
		      (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_Param_A/*src_ptr*/, 
		      modSizeInWords, VirtualHwBaseAddr );

	LLF_PKI_PKA_CopyDataIntoPkaReg( rB/*dstReg*/, LenID_n+1, 
		      (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_Param_B/*src_ptr*/, 
		      modSizeInWords, VirtualHwBaseAddr );
	
	/* copy array into coordinate X register */
	LLF_PKI_PKA_Copy( LenID_n+1, PAff_ptr->rX/*dst*/, rArr/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* get MS word of array rArr */
	changedWordNum = modSizeInWords-1; 
	changedWord = LLF_PKI_PKA_GetWordFromRegister( LenID_n,  PAff_ptr->rX, changedWordNum, VirtualHwBaseAddr );
	
	/* if value of MSByte of modulus > 100, than set pointer of changing byte to
	   MSB byte of message buffer, else to antecedent byte and set 0 to these bytes */
	if(((P_ptr[modSizeInWords-1] >> 24) & 0xFF) > 100){
		changedByteNum = (modSizeInBytes-1) % 4;
		addValue = 1UL << 8*changedByteNum;
		/* set 0 into changed byte */
		changedWord &= ~(0xFFUL << 8*changedByteNum);
	}
	else{
		if((modSizeInBytes-1) % 4 == 0){
			/* set MSByte = 0 */
			LLF_PKI_PKA_SetWordIntoRegister( LenID_n, 0/*changedWord*/, PAff_ptr->rX, changedWordNum/*i*/, VirtualHwBaseAddr ); 

			/* set changed word number to antecedent word and set changed byte number */
			changedWordNum--;
			changedWord = LLF_PKI_PKA_GetWordFromRegister( LenID_n,  PAff_ptr->rX, changedWordNum, VirtualHwBaseAddr );
			addValue = 1UL << 24;
			/* set 0 into changed byte */
			changedWord &= ~(0xFFUL << 24);
		}
		else{
			changedByteNum = (modSizeInBytes-1) % 4 - 1;
			addValue = 1UL << 8*changedByteNum;
			/* set 0 into changed byte and into MSByte */
			changedWord &= ~(0xFFFFUL << 8*changedByteNum);
		} 
	}
	
	/* try to convert current X into EC point */
	for( tries = 0; tries < 100; tries++ )
	{
		/* set changing byte */
		LLF_PKI_PKA_SetWordIntoRegister( LenID_n, changedWord, PAff_ptr->rX, changedWordNum/*i*/, VirtualHwBaseAddr ); 
		
		/* 1. calculate right part = X^3+AX+B = (X^2+A)X+B  */
		
		/* 1.1.  T1 = X^2 */
		LLF_PKI_PKA_ModMul( LenID_n, PAff_ptr->rX/*OpA*/, PAff_ptr->rX/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);   
		
		/* 1.2. calculate T1 = T1 + A */
		LLF_PKI_PKA_ModAdd( LenID_n, rT1/*OpA*/, rA/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);   
		
		/* 1.4. Calculate T1 = T1*X */ 
		LLF_PKI_PKA_ModMul( LenID_n, rT1/*OpA*/, PAff_ptr->rX/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);   
		
		/* 1.6. Calculate T1 = T1 + B */
		LLF_PKI_PKA_ModAdd( LenID_n, rT1/*OpA*/, rB/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);   
			 
		/* 2. Calculate Y coordinate of EC point Y = T1^(1/2) by calling SquareRootModPrime 
		      function . If square root is not exist return error message */
		Error =  LLF_ECPKI_SquareRootModPrime(rT1/*in*/, PAff_ptr->rY/*out*/, &isRootExists/*out*/, 						 
						regTemps_ptr+3, countTemps-3, VirtualHwBaseAddr);	 								 
		if(Error != CRYS_OK)
			return Error;
		
		/* 3. If square root is exist stop and output point (X,Y), else change low 
		      byte of X and and try next calculations */ 
		if( isRootExists == DX_TRUE )
			return Error;
		
		/* change the MS byte (word) of converted message */
		changedWord += addValue;
		
		WATCH_DOG_RESET();
	}

	return LLF_ECPKI_CONV_TO_EC_AFFINE_CAN_NOT_CONVERT_ERROR;	

} /* End of LLF_ECPKI_GFp_ConvArrayToECaffine function */

					   
					   
/**************************************************************************
 *	              LLF_ECPKI_GFpJacobianToAffine
 **************************************************************************/
/**
   @brief  This function converts GFp point from projective Jacobian to
           standard affine representation. 
   		   
   		   Functions algorithm (IEEE Std 1363-2000, A.9.6): 
   		      PJac(X,Y,Z) => PAff(x,y) 
              x = X * Z'^2 mod P, y = Y * Z'^3 mod P, where Z' = Z^-1  mod P.
              
           Note: PJac, PAff must be not overlapped;   
               
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
				DxUint32_t                 VirtualHwBaseAddr /*in*/ )

{
	/* LOCAL DECLARATIONS */
	
	CRYSError_t Error;
	
	/* virtual pointers to temporary registers */
	DxInt8_t rT1, rT2, rT3;
	
	DxUint32_t status;
	
	
	/******** Initialization of variables and pointers **********/

	Error = CRYS_OK;	
	
    /* check count of temp registers used */
 #ifdef LLF_PKI_PKA_DEBUG   
    if( countTemps < 3 )
        return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
 #endif	

    countTemps = countTemps;
	/* set virtual pointers */
	rT1 = regTemps_ptr[0];
	rT2 = regTemps_ptr[1];
	rT3 = regTemps_ptr[2];


	/*********  FUNCTION LOGIC  ************/
	
	/* copy EC parameter B into temp register */
	LLF_PKI_PKA_CopyDataIntoPkaReg( rT1/*dstReg*/, LenID_n+1, 
		      (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_Param_B/*src_ptr*/, 
		      (DxUint32_t)LLF_EC_DOMAINS_ARRAY[DomainID].EC_ModulusSizeInWords, VirtualHwBaseAddr );
	 
	/* If Z = 0 (point on infinity), then output:
	       if (B != 0)  output (0,0), else output (0,1),  where B -
	       parameter of elliptic curve   */

	LLF_PKI_PKA_CompareIm(LenID_n, PJac->rZ/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr);
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);    
	if( status == 1 )
	{
		LLF_PKI_PKA_Clr(LenID_n, PAff->rX/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr);
		LLF_PKI_PKA_Clr(LenID_n, PAff->rY/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr);
		
		LLF_PKI_PKA_CompareIm(LenID_n, rT1/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr);
		LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);    
		if( status == 1 )
			LLF_PKI_PKA_Set0(LenID_n, PAff->rY/*OpA*/, PAff->rY/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
		
		return Error;
	}
	
	/* Calculate T1 = Z^-1 mod P by calling LLF_InvMod function*/
	LLF_PKI_PKA_Copy(LenID_n+1, rT2/*dst*/, PJac->rZ/*src*/, 0/*Tag*/, VirtualHwBaseAddr);
	LLF_PKI_PKA_ModInv(LenID_n, rT2/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
	
	/* Calculate T2 = T1^2 = Z^-2 */
	LLF_PKI_PKA_ModMul(LenID_n, rT1/*OpA*/, rT1/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr); 
		       
	/* Calculate T3 = T1*T2 = Z^-3 */ 
	LLF_PKI_PKA_ModMul(LenID_n, rT1/*OpA*/, rT2/*OpB*/, rT3/*Res*/, 0/*Tag*/, VirtualHwBaseAddr); 
	
	/* Calculate PAff->rX = X_proj*T2 = X_proj*(Z^-2) */ 
	LLF_PKI_PKA_ModMul(LenID_n, PJac->rX/*OpA*/, rT2/*OpB*/, PAff->rX/*Res*/, 0/*Tag*/, VirtualHwBaseAddr); 
		
	/* Calculate T1 = Y_proj*T3 = X_proj*(Z^-3) */ 
	LLF_PKI_PKA_ModMul(LenID_n, PJac->rY/*OpA*/, rT3/*OpB*/, PAff->rY/*Res*/, 0/*Tag*/, VirtualHwBaseAddr); 
	
	if( PAff->rZ < 30 && (int)PAff->rZ >= 0){
		LLF_PKI_PKA_Clr(LenID_n, PAff->rZ/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr);
		LLF_PKI_PKA_Set0(LenID_n, PAff->rZ/*OpA*/, PAff->rZ/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
	}
	return Error;	
}
	
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
			DxUint32_t             VirtualHwBaseAddr)
{	
	/* Copy X,Y,Z coordinates  */
	LLF_PKI_PKA_CopyDataIntoPkaReg(Res->rX/*dstReg*/, LenID_n+1, X_ptr/*src_ptr*/, 
			sizeInWords , VirtualHwBaseAddr);
	
	LLF_PKI_PKA_CopyDataIntoPkaReg(Res->rY/*dstReg*/, LenID_n+1, Y_ptr/*src_ptr*/, 
				sizeInWords , VirtualHwBaseAddr);
	
	if(pointID > EC_PointAffine && 
	   pointID < EC_PointID_OffMode){
		if( Z_ptr != DX_NULL ){
			LLF_PKI_PKA_CopyDataIntoPkaReg(Res->rZ/*dstReg*/, LenID_n+1, Z_ptr/*src_ptr*/, 
						sizeInWords , VirtualHwBaseAddr);
		}        
		else{
			/* set Z = 1 */
			LLF_PKI_PKA_Clr(LenID+1, Res->rZ/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr);
			LLF_PKI_PKA_Set0(LenID, Res->rZ/*OpA*/, Res->rZ/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
		}
	}
	
	Res->PointID = pointID;
	
	return;
}
					   
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
			DxUint32_t             VirtualHwBaseAddr )
{	
    /* Copy X,Y,Z coordinates  */
	LLF_PKI_PKA_Copy( LenID, To->rX/*dst*/, From->rX/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Copy( LenID, To->rY/*dst*/, From->rY/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	if( From->PointID > EC_PointAffine && From->PointID < EC_PointID_OffMode )
		LLF_PKI_PKA_Copy( LenID, To->rZ/*dst*/, From->rZ/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* Copy PointID  */
	To->PointID = From->PointID; 
	
	return;  
}
				   


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
                                DxUint32_t             VirtualHwBaseAddr )
{	
    /* Set null to X,Y,Z   */
    LLF_PKI_PKA_Clr( LenID+1, P->rX/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_Clr( LenID+1, P->rY/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
   
    if( P->PointID > EC_PointAffine && P->PointID < EC_PointID_OffMode )
   		 LLF_PKI_PKA_Clr( LenID+1, P->rZ/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	return;
}


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
                                          DxUint32_t             VirtualHwBaseAddr )
{	
    /* Set null to X,Y,Z   */
    LLF_PKI_PKA_Clr( LenID+1, P->rX/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_Clr( LenID+1, P->rY/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_Clr( LenID+1, P->rZ/*OpA*/, 0/*Tag*/, VirtualHwBaseAddr );
   
    /*Set 1 to X, Y */
    LLF_PKI_PKA_Set0( LenID+1, P->rX/*OpA*/, P->rX/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_Set0( LenID+1, P->rY/*OpA*/, P->rY/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); 
    
    /* Set PointID to projective Jacobian */
    P->PointID = EC_PointJacobian; 
    
    return;     
}



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
			     DxUint32_t              VirtualHwBaseAddr /*in*/ )
{
    DxUint32_t status1, status2;
    
    LLF_PKI_PKA_CompareIm( LenID, P0->rX/*OpA*/, 1/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);

    LLF_PKI_PKA_CompareIm( LenID, P0->rY/*OpA*/, 1/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status2);
    
    status1 = status1 && status2;
    
    LLF_PKI_PKA_CompareIm( LenID, P0->rZ/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status2);
    
    status1 = status1 && status2;
  
    return status1;
	    
}

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
				     DxUint32_t              VirtualHwBaseAddr /*in*/ )
{
    DxUint32_t status1, status2;
    
    LLF_PKI_PKA_CompareIm( LenID, P0->rX/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);

    LLF_PKI_PKA_CompareIm( LenID, P0->rY/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status2);
    
    status1 = status1 && status2;
    
    LLF_PKI_PKA_CompareIm( LenID, P0->rZ/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr );
    LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status2);
    
    status1 = status1 && status2;
  
    return status1;
	    
}

/**********************************************************************************************************/


#ifdef __cplusplus
}
#endif			


