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

/* .............. CRYS level includes ................. */

#include "dx_vos_mem.h"
#include "crys_rnd.h"

#include "crys_ecpki_kg.h"
#include "crys_ecpki_error.h"
#include "crys_ecpki_local.h"


#include "crys_common.h"
#include "crys_common_math.h"

/* .............. LLF level includes ................. */
#include "llf_pki.h"
#include "llf_ecpki_ec_arithmetic.h"
#include "llf_ecpki_modular_arithmetic.h"


/************************ Defines ******************************/

/* canceling the lint warning:
  Info 740: Unusual pointer cast 
  (incompatible indirect types) */
/*lint --e{740} */

/* canceling the lint warning:
   Info 826: Suspicious pointer-to-pointer conversion 
   (area too small) */
/*lint --e{826} */


/************************ Enums ****************************************/


/************************ Typedefs *************************************/


/************************ Global Data **********************************/

/* Extern global data */
 extern const  CRYS_ECPKI_Domain_t LLF_EC_DOMAINS_ARRAY [];


/************* Private and external functions prototypes  *****************************/
CRYSError_t DX_RND_GenerateWordsArrayInRange(
					DxUint32_t   rndSizeInBits, 
					DxUint32_t  *maxVect_ptr, 
					DxUint32_t  *rndVect_ptr,
					DxUint32_t  *tmp_ptr);

/************************ Public Functions ******************************/

/******************************************************************************************
 *                         LLF_ECPKI_GenKeyPair function                                  *
 ******************************************************************************************/
/**

   @brief  LLF_ECPKI_GenKeyPair generates a pair of private and public keys  
           in little endian ordinary form. 
	  
	  The function performs the following:
	
		1.  Generates randomly private EC key s by calling CRYS_RND_GenerateVectorInRange() 
		    and LLF_PKI_PKA_Div() functions.
		3.  Converts affine EC generator point G into projective representation.
		4.  Calls the LLF_ECPKI_ScalarMultProj function  for calculating of EC public
		    key W = s*G.		    
		5.	Converts generated public key data to affine ordinary output form.
		6.	Exits. 
 
   @param[in]  DomainID       - The enumerator variable defines current EC domain
   @param[out] rS             - A virtual pointer to register, containing  private key. 
   @param[out] W_ptr          - A pointer to the EC-point structure representing the 
                                 public key . 
   @param[in]  regTemps_ptr   - Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps     - Count of allowed temp PKA tegisters.
   @param[in]  tmp_ptr        - temp buffer in RAM for random generating or providing the 
                                private key.  
   @param[in]  isPrivKeyInternal - indicator: if isPrivKeyInternal = 1, the key will be 
                                 generated randomally by the function, if 0, then key
                                 provided by tempS_ptr buffer of sze equall EC order size.
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.

   @return CRYSError_t - CRYS_OK, or errors from called functions.
                      
*/
CRYSError_t LLF_ECPKI_GenKeyPair(		                                 
				CRYS_ECPKI_DomainID_t  	    DomainID,	      /*in*/ 
				DxInt8_t                    rS,    	      /*out*/
				LLF_ECPKI_PKA_Point_t      *W_ptr,            /*out*/
				DxInt8_t                   *regTemps_ptr,     /*in*/
				DxUint32_t                  countTemps,       /*in*/
				CRYS_ECPKI_KG_TempData_t   *tmpData_ptr,          /*in*/
				DxInt8_t                    isPrivKeyInternal,/*in*/
				DxUint32_t                  VirtualHwBaseAddr /*in*/ )

{
   /* LOCAL DECLARATIONS */
   
   /* the error identifier */
   CRYSError_t Error;

   /* the pointers to the key structures and and domain parameters */
   DxUint32_t             orderSizeInBits;  /* size of EC order in bits */
   

   /* FUNCTION LOGIC */    
   
   /* ................ initializtions  ............ */
   /* ------------------------------------------------------------------- */
   
    /* initialize the error identifier to OK */
   Error = CRYS_OK;
   
   
   /* set the public and private key pointers */

   /* modulus and private key max size in words and in bytes */
   orderSizeInBits = LLF_EC_DOMAINS_ARRAY[DomainID].EC_OrderSizeInBits;
   
     
   /* ................ loading the structures .......... */
   /* ------------------------------------------------------------------------- */
   
   /* set W point ID to projective Jacobian */
   W_ptr->PointID = EC_PointJacobian;   
     
   /* ................ executing the key generation ........................... */
   /* ------------------------------------------------------------------------- */
   
   if( isPrivKeyInternal == DX_TRUE )
   {
	   /* set order into temp buff as LE bytes array */

	   /* generate the random vector for private key in range 1 < s < Order */
	   Error = DX_RND_GenerateWordsArrayInRange( 
				   orderSizeInBits, 
				   (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_GenerOrder/*MaxVect*/, 
				   ((LLF_ecpki_key_gen_db_t*)tmpData_ptr->crysKGIntBuff)->s/*PrivKey*/,
				   ((LLF_ecpki_key_gen_db_t*)tmpData_ptr->crysKGIntBuff)->tmp); 

	   if(Error != CRYS_OK)
		   return Error;
   }
	
   /* copy random data into priv key register */
   LLF_PKI_PKA_CopyDataIntoPkaReg( rS/*dstReg*/, LenID_r+1, 
				 ((LLF_ecpki_key_gen_db_t*)tmpData_ptr->crysKGIntBuff)->s/*src_ptr*/, 
                                 (orderSizeInBits+31)/32, VirtualHwBaseAddr );

   /* set EC generator into point W for scalar multiplication */
   LLF_ECPKI_SetPoint( 
		(DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_Gener_X,
		(DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_Gener_Y,
		DX_NULL/* Z_ptr */,
		LLF_EC_DOMAINS_ARRAY[DomainID].EC_ModulusSizeInWords,
		LenID_n,
		W_ptr/*Res point*/,EC_PointJacobian, VirtualHwBaseAddr );				  
   
   
   /*---------------------------------------------------------------------------*/
   /* .........generation of public key by calling low level function ......... */
   /*---------------------------------------------------------------------------*/

   Error = LLF_ECPKI_GFpScalarMultProj(rS, W_ptr/*EC_Order*/, W_ptr/*Res*/, DomainID,
					regTemps_ptr, countTemps, VirtualHwBaseAddr);
   if( Error != CRYS_OK )   
	   return Error;
   
   /* convert public key to affine ordinary representation */
   Error = LLF_ECPKI_GFpJacobianToAffine(W_ptr, W_ptr,	DomainID, 
                                         regTemps_ptr, countTemps, VirtualHwBaseAddr);  

   return Error;

}/* END OF LLF_ECPKI_GenKeyPair */                                       


/****************************************************************************************************
 *                       LLF_ECPKI_GFpCheckPublKey function                                         *
 ****************************************************************************************************/
/**
   @brief LLF_ECPKI_GfpCheckPublKey checks a validity of EC public key whih is  
          represented in affine little endian ordinary (non Montgomery) form. 
	  
	  The LLF_ECPKI_GFpCheckPublKey function performs the following checking according to
	  IEEE-1363 algorithms A.16.10:
	
	   1. Check validity of public key (full check):
	   		1.1. Checks that W(x,y) != O.
            1.2. If q = p, checks that x and y are integers, such that 0 < x < p 
                 and 0 < y < p and y^2 = x^3 + ax + b (mod p).
            1.3. If q = 2^m, checks that x and y are elements of GF2^m and that 
                 y^2 + xy = x^3 + ax^2+ b in GF2^m.
				 Note: This step for GF2^m is not implemented here. 
            1.4. If parameter isFullCheck == True, the function checks that r*W = O.
            1.5. Outputs "True" if the check is OK, and "False" otherwise. 
       
       2. Partly check of public key. In this case (isFullCheck = False) 
          the function  performs all above stated steps, excluding the step 1.4.  
          The last checking may be used in ECSVDP_DHC and ECSVDP_MQVC primitives.  
 
   @param[in]  DomainID    - ID of EC domain.
   @param[out] W_ptr       - A pointer to the LLF public key structure.                                  
   @param[in]  checkMode   - Parameter, defining wheter the full (IsFullCheck = 1) 
                             or partly (IsFullCheck = 0) checking of Key validity 
                             will be performed.
   @param[in]  regTemps_ptr- Pointer to list of allowed temp PKA tegisters.
   @param[in]  countTemps  - Count of allowed temp PKA tegisters.                               
   @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.

   @return  CRYS_OK, or error message according to module errors definitions
                     LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR 
                     LLF_ECPKI_DOMAIN_INFO_PUBL_KEY_ON_INFINITY_ERROR                       
*/
 CRYSError_t  LLF_ECPKI_GFpCheckPublKey(
				CRYS_ECPKI_DomainID_t      DomainID,         /*in*/
				LLF_ECPKI_PKA_Point_t     *W_ptr,            /*in*/ 
				EC_PublKeyCheckMode_t      checkMode,        /*in*/
				DxInt8_t                  *regTemps_ptr,     /*in*/
				DxUint32_t                 countTemps,       /*in*/
				DxUint32_t                 VirtualHwBaseAddr /*in*/ )
{
   /* LOCAL DECLARATIONS */
   
   /* error identifier */
   CRYSError_t Error;

   /* the virtual pointer to EC order in projective form */  
    DxInt8_t  rOrd;

   /* PKA status and indication variable */
   DxUint32_t status;
   
   LLF_ECPKI_PKA_Point_t  W1;

   /* FUNCTION LOGIC */   
   
   /* check temp registers count, used by this function */
   if( countTemps < 4 ) 
       return LLF_PKI_PKA_NOT_ENOUGH_TEMP_REGS_ERROR; 
   
   /* ................ initializtions  ............ */
   /* ------------------------------------------------------------------- */
   
    /* initialize the error identifier to OK and Valid to False*/
   Error = CRYS_OK; 
   
   /* set irtual pointer of order register */
   rOrd = regTemps_ptr[0];

   LLF_PKI_PKA_CopyDataIntoPkaReg( rOrd/*dstReg*/, LenID_r+1, 
                                   (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_GenerOrder/*src_ptr*/, 
                                   LLF_EC_DOMAINS_ARRAY[DomainID].EC_OrderSizeInWords , VirtualHwBaseAddr );
   
   /* ................ executing the key validation ........................... */
   /* ------------------------------------------------------------------------- */
   
   /* 1. Check that Wy != 0: this means also, that W  is not a point on infinity */
   LLF_PKI_PKA_CompareIm(LenID_n+1, W_ptr->rY/*OpA*/, 0/*imm OpB*/, 0/*Tag*/, VirtualHwBaseAddr);
   LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
   if( status == 1 )
   		return LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR;
 
   /* if PublKeyX > Modulus,  then Error */   
   LLF_PKI_PKA_Sub(LenID_n, W_ptr->rX/*OpA*/, 0/*rN*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);
   LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status);
    if( status == 1 )
   		return LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR;

   /* if PublKeyY > Modulus  then Error */ 
   LLF_PKI_PKA_Sub(LenID_n, W_ptr->rY/*OpA*/, 0/*rN*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);
   LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status);
   if( status == 1 )
   		return LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR;
   
   /* 2. If Partly or Full checkMode, then check that the point W is on curve */
   if ( checkMode >= ECpublKeyPartlyCheck )
   {
	Error = LLF_ECPKI_IsPointOnCurve( W_ptr, DomainID, regTemps_ptr+1, countTemps-1, VirtualHwBaseAddr );
	if(Error != CRYS_OK )
		return Error;
   } 

   /* 3. If Full checkMode, then validate that r*W == O */
   if( checkMode == ECpublKeyFullCheck )
   {   
   	   /* set virtual pointers of point W1 on temp registers */
       W1.rX = regTemps_ptr[1];
       W1.rY = regTemps_ptr[2];
       W1.rZ = regTemps_ptr[3];

       W1.PointID = EC_PointJacobian;
       
	   /* 3.2. Calculate W1 = Order*W                 */
	   Error = LLF_ECPKI_GFpScalarMultProj( rOrd, W_ptr, &W1, DomainID, 
	                                        regTemps_ptr+4, countTemps-4, VirtualHwBaseAddr );  

	   /* on failure exit the function */
	   if( Error != CRYS_OK )
	   	return Error;

	   /* 3.3. Check that W_proj is on infinity (1,1,0) */
	   
	   status = LLF_ECPKI_IsProjPointOnInfinity( LenID_n, &W1, VirtualHwBaseAddr );  

	   if( status != 1 )     
	   	return LLF_ECPKI_DOMAIN_INFO_PUBL_KEY_ON_INFINITY_ERROR;
   } 

   /* 4.   End      */   

   return Error;

}/* END OF LLF_ECPKI_CheckPublKey */                                       



