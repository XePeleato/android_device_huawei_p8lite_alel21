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

 
 
#ifndef LLF_ECPKI2_H
#define LLF_ECPKI2_H
  
/*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  08/12/2005
   *  Last modified   :  %modify_time%
   */
  /** @file LLF_ECPKI.h
   *  \brief A brief description of this module
   *
   *  \version LLF_ECPKI.h#1:incl:1
   *  \author R.Levin
   */
/*
 * All the includes that are needed for code using this module to
 * compile correctly should be included here.
 */

#include "crys_ecpki_types.h"
#include "llf_ecpki_error.h"
#include "llf_pki_hw_defs.h"
#include "llf_pki.h"
#include "llf_ecpki_defs.h"


#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines *************************************/

/* default LenID-s definitions used in ECPKI. Note: extended sizes are 
   in the entries LenID+1 (i.e. 1,3) accordingly */
#define LenID_n    0  /* for operations with modulus size */
#define LenID_r    2  /* for  operations with EC order size */





/************************ Enums ***************************************/

/************************ Typedefs  ***********************************/

/* array 5 words, containing the PKA Barrett mod tag NP */
#define LLF_ECPKI_PKA_NP_t  LLF_PKI_PKA_NP_t   

	
/************************ Structures  *********************************/

/* elliptic curve point in PKA mode - coordinates are represented by 
   virtual registers pointers */
typedef struct{   
    
	DxInt8_t   rX;
	DxInt8_t   rY;
	DxInt8_t   rZ;
	CRYS_ECPKI_PointID_t  PointID;
		
} LLF_ECPKI_PKA_Point_t;

/* LLF strucure for EC domain Specific buffers */
typedef struct
{  
	/* Barrett tag for modulus */ 
	DxUint32_t NP[LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
	/* Barrett tag for order R */
	DxUint32_t RP[LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
} LLF_ecpki_domains_db_t;


/* LLF strucure for ECPKI Private Key temp buffers */
typedef struct{   
	DxUint32_t NP[LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /* Barrett for modulus*/
	DxUint32_t RP[LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /* Barrett for order */
} LLF_ecpki_priv_key_db_t;

/* LLF strucure for ECPKI Private Key temp buffers */
typedef struct{   
	DxUint32_t NP[LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /* Barrett for modulus*/
	DxUint32_t RP[LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /* Barrett for order */
} LLF_ecpki_pub_key_db_t;

/* LLF strucure for ECPKI Build temp buffers */
typedef struct{	
       DxUint32_t dummy;
} LLF_ecpki_build_db_t;

/* LLF strucure for ECPKI KG temp buffers */
typedef struct{
	/*! The aligned temporary buffers for private key */
	DxUint32_t s[CRYS_ECPKI_ORDER_MAX_LENGTH_IN_WORDS];
	DxUint32_t tmp[CRYS_ECPKI_ORDER_MAX_LENGTH_IN_WORDS];
} LLF_ecpki_key_gen_db_t;

///* LLF strucure for ECDH temp buffers */
typedef struct{
	/* The aligned temporary buffer for private key */
	DxUint32_t  s[CRYS_ECPKI_ORDER_MAX_LENGTH_IN_WORDS];
}LLF_ecdh_db_t;
//
/* Temporary buffers used for the functions called from ECDSA */
typedef struct{      
	DxUint32_t Tempbuff[5*CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS]; 
} LLF_ecdsa_sign_db_t;
//
///* Temporary buffers used for the  functions called from ECDSA */
typedef struct{      
	DxUint32_t Tempbuff[3*CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS]; 
} LLF_ecdsa_verify_db_t;

/************************ Public Variables ****************************/

/************************ Public Functions ****************************/


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
				CRYS_ECPKI_KG_TempData_t   *tmp_ptr,          /*in*/
				DxInt8_t                    isPrivKeyInternal,/*in*/
				DxUint32_t                  VirtualHwBaseAddr /*in*/ );



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
			LLF_ECPKI_PKA_Point_t     *W,                /*in*/ 
			EC_PublKeyCheckMode_t      checkMode,        /*in*/
			DxInt8_t                  *regTemps_ptr,     /*in*/
			DxUint32_t                 countTemps,       /*in*/
			DxUint32_t                 VirtualHwBaseAddr /*in*/ );


/*****************************************************************************************
 *		       LLF_ECDSA_Sign_Calc function	/ named ECSP_DSA in IEEE-1363 std.           *
 *                        with internal or external ephemeral key						 *						  
 *****************************************************************************************/
/**
   @brief: 	This function calculates digital signature accordingly to EC DSA algorithm and 
            and WM DRM requirements by the following steps:
			
		1.	Checks input parameters , which was not checked on CRYS level;
		2.	Initializes variables and structures from input data and appropriate EC_Domain,  
		    (provided by  Signing Context pointer and other inputs) including initialization 
		    of  pointers to the following arrays:
			-	r - order of generator, 
			-	f - HASH  representative of message;
			-	s - signers private key;
		3.	Generate  new random Key Pair   (u, V(X,Y)) by calling CRYS_ECPKI_GenKeyPair 
		    function (with output little endian data).
		4.	Converts X into integer i, and computes integer  c  =  i  mod r  by calling  
		    LLF_PKI_UTIL_div  function.
		5.	Compare c to 0 by calling CRYS_COMMON_CmpCounterToNull, if (c =  0)  go to 3;
		6.	Computes  d = u^-1*(f + sc)  mod r  by calling LLF  functions.			
	    7.  Compares   d to zero by calling CRYS_COMMON_CmpCounterToNull, 
	    	if (d = = 0) then go to step 3;
		8.	Outputs (c, d) as signature;
		9.	Clears temporary arrays, which may content secret information;
		10.	Exits.

   @param[in]  SignerPrivateKey_ptr - A pointer to the User Private Key   
   @param[in]  MessageRepresent_ptr - A pointer to message representative  
   @param[in]  MessageRepresentSizeWord - Size of message representative in words
   @param[in]  IsEphemerKeyInternal, - indicates source for random ephemeral key:
                    IsEphemerKeyInternal = 1 - source internal,
                    IsEphemerKeyInternal = 0 - source external from RandEphemeralKey_ptr.
   @param[in]  EphemeralKeyData_ptr - Pointer to source for external ephemeral key data which
                                      contains: 
									  ephemeral private key u of size 4*OrderSizeInWords,
                                      ephemeral public key Vx and Vy - each of length ModSizeInBytes .
   @param[out] SignatureC_ptr - Pointer to output of C value of signature.     
   @param[out] SignatureD_ptr - Pointer to otput of D value of signature.  
   @param[in]  TempBuff_ptr -   Pointer to temporary buffer.                          
   
   @return CRYSError_t - CRYS_OK,    						 			 
						 Error from called functions 
*/
CRYSError_t LLF_ECDSA_SignCalc( 
			CRYS_ECPKI_PrivKey_t    *SignerPrivateKey_ptr,      /*in*/							
			DxUint32_t             *MessageRepresent_ptr,      /*in*/ 
			DxUint32_t              MessageRepresentSizeWords, /*in*/
			DxUint32_t              IsEphemerKeyInternal,      /*in*/ 
			DxUint32_t             *EphemerKeyData_ptr,        /*in*/							              
			DxUint32_t             *SignatureC_ptr,            /*out*/
			DxUint32_t             *SignatureD_ptr,            /*out*/								 
			DxUint32_t             *TempBuff_ptr               /*in*/ );


/*****************************************************************************************
 *		       LLF_ECDSA_Verify_Calc function	/ named ECVP_DSA in IEEE-1363 std.       *
 *                        with internal or external ephemeral key						 *						  
 *****************************************************************************************/
/**
  @brief:	This function calculates signature verifying parameters 
         
		Steps:	
		1.	Initializes variables and structures from input data and appropriate EC_Domain,   
		    including initialization of  pointers to the following arrays:
		             p - modulus,
		             r - order of generator, 
		             f - HASH  representative of message calculated on CRYS level,
		             W(Xw,Yw)  - signer public key (EC point coordinates Xw,Yw ),
		             c, d  -  signature to be verified (integer arrays).		
		2.	Checks c and d:  if  c  or d  is not in interval [1,r-1] then outputs "invalid" 
		    and exits.
		3.	Computes  h = d^-1  mod r    by calling  LLF_PKI_UTIL_InvMod.
		4.	Computes the following integers by calling appropriate LLF functions:    
		    h1 = f*h mod r,  h2 = c*h mod r.  
		5.	Computes EC point  P(Xp,Yp)  =  h1*G  + h2*W.
		6.	Compares point P with O (point on infinity). If  P == O , then outputs "Invalid" and exits.
		7.	Computes  c1  =  i mod r   -  by calling  LLF_PKI_UTIL_div  function.
		8.	If  ( c1 != c ) then output "Invalid".
		9.	Exits.
	
		NOTE: 1. The signature is "Valid" if the function returns "CRYS_OK", and error message otherwise.
		      2. The high extra bytes of input message representative array ( of length 
		         ModSizeInWords + 1) must be zeroed.
		
 @param[in]  SignerPublicKey_ptr   - A pointer to the Signer Public Key 
 @param[in]  MessageRepresent_ptr  - A pointer to the message representative
 @param[in]  MessageRepresentSizeWord - Size of message representative in words
 @param[in]  SignatureC_ptr        - A pointer to the signature c value. 
 @param[in]  SignatureD_ptr        - A pointer to the signature d value.
 @param[in]  TempBuff_ptr          - Apointer to temporary buffer.
 		
 @return   CRYSError_t - CRYS_OK,
                         LLF_ECDSA_VERIFY_CALC_SIGNATURE_IS_INVALID					     
*/
CRYSError_t LLF_ECDSA_VerifyCalc( 
			CRYS_ECPKI_PublKey_t   *SignerPublKey_ptr,          /*in*/							
			DxUint32_t             *MessageRepresent_ptr,      /*in*/ 
			DxUint32_t              MessageRepresentSizeWords, /*in*/
			DxUint32_t             *SignatureC_ptr,            /*in*/
			DxUint32_t             *SignatureD_ptr,            /*in*/
			DxUint32_t             *TempBuff_ptr               /*in*/ );


/***************************************************************************/
#ifdef __cplusplus
}
#endif

#endif
