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
#include "dx_vos_memmap.h"
#include "crys_rnd.h"
#include "crys_ccm.h"
#include "plat_print_dbg.h"
#include "crys_common.h"
#include "crys_common_math.h"
#include "crys_ecpki_kg.h"
#include "crys_ecpki_error.h"
#include "crys_ecpki_local.h"

#include "llf_pki.h"
#include "llf_ecpki.h"
#include "llf_ecpki_ec_arithmetic.h"
#include "llf_ecpki_modular_arithmetic.h"
#include "llf_ecpki_defs.h"


#ifndef DX_OEM_FW
#include "dx_vos_sem.h"
#else //DX_OEM_FW
#define DX_VOS_SemWait(SemPkaId,timeout)  CRYS_OK
#define DX_VOS_SemGive(SemPkaId)
#endif //DX_OEM_FW

/************************ Defines ******************************/

/* canceling the lint warning:
  Info 740: Unusual pointer cast 
  (incompatible indirect types) */
/*lint --e{740} */

/* canceling the lint warning:
   Info 826: Suspicious pointer-to-pointer conversion 
   (area too small) */
/*lint --e{826} */

/* canceling the lint warning:
Info 716: while(1) */
/*lint --e{716} */


/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

extern const CRYS_ECPKI_Domain_t LLF_EC_DOMAINS_ARRAY[]; 


/* importing the base address of the CRYPTO Cell . this value must be initialized in the 
   CRYS_Init function */
extern DxUint32_t PLAT_CryptoCellBaseAddr;

#ifndef DX_OEM_FW
extern DX_PAL_SEM SemPkaId;
#endif //DX_OEM_FW


/************* Private function prototype ****************/

/************************ Public Functions ******************************/


/*****************************************************************************************
 *		       LLF_ECDSA_Sign_Calc function	/ named ECSP_DSA in IEEE-1363 std.
 *                        with internal or external ephemeral key												  
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
   @param[in]  EphemeralKeyData_ptr - Pointer to source for external ephemeral 
		    key data which, contains: 
			- ephemeral private key U of size ModSizeInBytes+4,
			- ephemeral public key Vx and Vy - each of length ModSizeInBytes.
   @param[out] SignatureC_ptr - Pointer to output of C value of signature.     
   @param[out] SignatureD_ptr - Pointer to otput of D value of signature.  
   @param[in]  TempBuff_ptr -   Pointer to temporary buffer of size not less, than (3*ModulusSizeInWords + 1).                          
   
   @return CRYSError_t - CRYS_OK,    						 			 
			Error from called functions 
*/
CRYSError_t LLF_ECDSA_SignCalc(  
			CRYS_ECPKI_PrivKey_t   *SignerPrivateKey_ptr,      /*in*/							
			DxUint32_t             *MessageRepresent_ptr,      /*in*/ 
			DxUint32_t              MessageRepresentSizeWords, /*in*/
			DxUint32_t              IsEphemerKeyInternal,      /*in*/ 
			DxUint32_t             *EphemerKeyData_ptr,        /*in*/							              
			DxUint32_t             *SignatureC_ptr,            /*out*/
			DxUint32_t             *SignatureD_ptr,            /*out*/								 
			DxUint32_t             *TempBuff_ptr               /*in*/ )
{
   /* LOCAL DECLARATIONS */
   
   /* errors identifier */
   CRYSError_t Error;
    
   /* EC point for ephemeral key */
   LLF_ECPKI_PKA_Point_t  V;
   
   /* virtual pointers to PKA registers: private key rS, ephemeral private key rU, 
      message rF, EC order rR, temp registers rD etc. */
   DxInt8_t   rS, rU, rF, rR, rT, rN, rNp, rRp;

   /* list of PKA registers allowed for using : Note: numbers 0xFF - for debug goals */
   DxInt8_t regTemps[30] = {0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
                            0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
                            0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0xFF,0xFF }; 
   
   /* EC domain parameters */
   CRYS_ECPKI_DomainID_t  	DomainID;
   const CRYS_ECPKI_Domain_t   *Domain_ptr;
   DxUint32_t  modSizeInBits, modSizeInWords, ordSizeInWords;
   
   /* indication variables */
   DxUint32_t isModGrateOrder, status;
   
   DxUint32_t  VirtualHwBaseAddr = 0;
   
   DxUint32_t  tryNum;
   
   /* pointer to temp buff for Key generation */
   CRYS_ECPKI_KG_TempData_t  *kgTempBuff_ptr;
   
   /* for preventing compiler warnings */
   *TempBuff_ptr = *TempBuff_ptr;
   
   /* ................ initializtions  ............ */
   /* --------------------------------------------- */
   
   /* initialize the error */
   Error = CRYS_OK; 
   
   /* set domain parameters */
   DomainID = SignerPrivateKey_ptr->DomainID;
   Domain_ptr = &LLF_EC_DOMAINS_ARRAY[DomainID];
   modSizeInBits  = Domain_ptr->EC_ModulusSizeInBits;
   modSizeInWords = Domain_ptr->EC_ModulusSizeInWords;
   ordSizeInWords = Domain_ptr->EC_OrderSizeInWords;
  
   /* set the PKA virtual pointers */
   rS   = regTemps[0];   /* private key */
   rU   = regTemps[1];   /* ephemeral private key */
   V.rX = regTemps[2];   /* ephemeral public key */
   V.rY = regTemps[3];
   V.rZ = regTemps[4];
   rF   = regTemps[5];   /* message representative */
   rR   = regTemps[6];   /* EC order */
   rRp  = regTemps[7];   /* temp for Barrett tag */
   rT   = regTemps[8];   /* other temp regs */
   
   /* default registers */    
   rN  = 0; /* modulus */
   rNp = 1; /* Barrett tag */
   
 
   /* FUNCTION LOGIC */
   
   /* ............... getting the hardware semaphore ..................... */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK )   
     goto Return;

   /* ............... mapping the physical memory to the virtual one ...... */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );           /* The virtual address - out */
   if( Error != CRYS_OK )   
     goto End_OnlySemRelease;
     
   /* ----------------------------------------------------------------------- */
   /* .................... initialisation of PKA  ........................... */
   /* ----------------------------------------------------------------------- */   
  
   /* initialize the PKA engine on default mode. Note: Registers size is set 
      by 1 word greate, than max modulus size for operations with EC order*/
   Error = LLF_PKI_PKA_DefaultInitPKA( modSizeInBits, CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+2, VirtualHwBaseAddr );

   if( Error != CRYS_OK )    
     goto End;
 
   /* set EC order exact and extended sizes in bits into RegSizes entry 2,3 */
   LLF_PKI_PKA_SetRegSize( Domain_ptr->EC_OrderSizeInBits, LenID_r, VirtualHwBaseAddr );
   LLF_PKI_PKA_SetRegSize( Domain_ptr->EC_OrderSizeInBits+32, LenID_r+1, VirtualHwBaseAddr );

    /* ----------------------------------------------------------------------- */
    /*............ setting the input data into PKA registers ..................*/
    /* ----------------------------------------------------------------------- */

    /* modulus and its Barrett tag */
    LLF_PKI_PKA_CopyDataIntoPkaReg( rN/*dstReg*/, LenID_r+1, (DxUint32_t*)Domain_ptr->GF_Modulus/*src_ptr*/, 
                                    modSizeInWords, VirtualHwBaseAddr );

    LLF_PKI_PKA_CopyDataIntoPkaReg( rNp/*dstReg*/, LenID_r+1, ((LLF_ecpki_priv_key_db_t *)(SignerPrivateKey_ptr->crysPrivKeyIntBuff))->NP/*src_ptr*/, 
                                    LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr );
                                    
    /* EC order and its Barrett tag */
    LLF_PKI_PKA_CopyDataIntoPkaReg( rR/*dstReg*/, LenID_r+1, (DxUint32_t*)Domain_ptr->EC_GenerOrder/*src_ptr*/, 
                                    ordSizeInWords, VirtualHwBaseAddr );
    LLF_PKI_PKA_CopyDataIntoPkaReg( rRp/*dstReg*/, LenID_r+1, ((LLF_ecpki_priv_key_db_t *)(SignerPrivateKey_ptr->crysPrivKeyIntBuff))->RP/*src_ptr*/, 
                                    LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr );
    
    /* private key */
    LLF_PKI_PKA_CopyDataIntoPkaReg( rS/*dstReg*/, LenID_r+1, SignerPrivateKey_ptr->PrivKey/*src_ptr*/, 
                                    ordSizeInWords, VirtualHwBaseAddr );
    /* message representative rF */
    LLF_PKI_PKA_CopyDataIntoPkaReg( rF/*dstReg*/, LenID_r+1, MessageRepresent_ptr/*src_ptr*/, 
                                    MessageRepresentSizeWords, VirtualHwBaseAddr );
   
    /* check is the modulus greate, than EC order: if it is, then isModGrateOrder = 1, else 0 */                               
    LLF_PKI_PKA_Sub( LenID_r , rN/*OpA*/, rR/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);
    LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, isModGrateOrder);


   
   /* ----------------------------------------------------------------------- */
   /*......... main loop of SIGN  ............................................*/
   /* ----------------------------------------------------------------------- */
   tryNum = 0;
   while(1){

	/* 1. Generate ephemeral key pair  U, V(Xv,Yv)  */
	if(IsEphemerKeyInternal == DX_TRUE){
		if( tryNum > 0x100 )
			return LLF_ECDSA_SIGN_CALC_CANNOT_CREATE_SIGNATURE;

		kgTempBuff_ptr = (CRYS_ECPKI_KG_TempData_t*)TempBuff_ptr;
	   
		Error = LLF_ECPKI_GenKeyPair(DomainID,	rU/*ephemer.priv*/, &V/*ephemer.publ*/,
					regTemps+9, 28-9/*countTemps*/, kgTempBuff_ptr, 
					DX_TRUE/*isKeyInternal*/, VirtualHwBaseAddr);
		if(Error != CRYS_OK)
			return Error;
	}
	else{
		if( tryNum > 0 )
			return LLF_ECDSA_SIGN_CALC_CANNOT_CREATE_SIGNATURE;
		
		kgTempBuff_ptr = (CRYS_ECPKI_KG_TempData_t*)EphemerKeyData_ptr;

		/* copy ephemeral private Key */   
		LLF_PKI_PKA_CopyDataIntoPkaReg( rU/*dstReg*/, LenID_r+1, EphemerKeyData_ptr/*src_ptr*/, 
				ordSizeInWords, VirtualHwBaseAddr);
		
		/* copy ephemeral public Key X */   
		LLF_PKI_PKA_CopyDataIntoPkaReg( 
		       V.rX/*dstReg*/, LenID_r+1, (EphemerKeyData_ptr + modSizeInWords + 1)/*src_ptr*/, 
		       modSizeInWords, VirtualHwBaseAddr );
	} 

        /* increment tryCount */
        tryNum++;
        
	/* 2. If Mod > Order then compute c = Xv = Xv mod r,  else c = Xv. 
	      Note: below reg V.rX is used as c */ 
    	if( isModGrateOrder == 1){
		/* c = Xv mod r  */ 
		LLF_PKI_PKA_Div( LenID_r, V.rX/*OpA=remainder*/, rR/*OpB*/, V.rZ/*Res not used*/, 0/*Tag*/, VirtualHwBaseAddr ); 
	}   
		   
	/* 3. If c = Xv == 0 go to  step 1  */		
	LLF_PKI_PKA_CompareIm( LenID_r , V.rX/*OpA*/, 0/*OpB imm*/, 0/*Tag*/, VirtualHwBaseAddr);
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	if( status == 1)	   
		continue;
	
	
	/*---------------------------------------------------------------------*/ 	      
	/* 5. Compute d = rT = u^-1*(f + s*c) mod r                            */  
	/*---------------------------------------------------------------------*/  
	
	/* 5.0. Swap N, R  and Np, Rp: (set 0,1 registers according to EC order and its Barrett tag) */
	LLF_PKI_PKA_SwapRegsPhisAddr( &rN, &rR, &rNp, &rRp, VirtualHwBaseAddr ); 	
	
	/* 5.1. Compute u = u^-1 mod r  */
	LLF_PKI_PKA_Copy( LenID_r+1, rT/*dst*/, rU/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_ModInv( LenID_r, rT/*OpB*/, rU/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* 5.2. rT = s*c mod r */
	LLF_PKI_PKA_ModMul( LenID_r, rS/*OpA*/, V.rX/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	 
	/* 5.3. rT = (f + s*c) mod r  */
	LLF_PKI_PKA_Div( LenID_r, rF/*OpA=remainder*/, rR/*OpB*/, V.rZ/*Res not used*/, 0/*Tag*/, VirtualHwBaseAddr ); 
	LLF_PKI_PKA_ModAdd( LenID_r, rF/*OpA*/, rT/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	
	/* 5.4. rT = d = u*rT mod r */
	LLF_PKI_PKA_ModMul( LenID_r, rU/*OpA*/, rT/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
	   
	/* 5.5. If d != 0 then end the loop */
	LLF_PKI_PKA_CompareIm( LenID_r , rT/*OpA*/, 0/*OpB imm*/, 0/*Tag*/, VirtualHwBaseAddr);
	LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	
	if( status == 0)			  
		break;			
	else /* restore N,Np registers according to EC modulus and continue */
		LLF_PKI_PKA_SwapRegsPhisAddr( &rN, &rR, &rNp, &rRp, VirtualHwBaseAddr ); 
	 	   
   }   /* End of the loop */		

   /* 7.  Copy the pair (c,d) in signature output */
   LLF_PKI_PKA_CopyDataFromPkaReg(SignatureC_ptr,  ordSizeInWords,  
                                  V.rX/*srcReg*/, VirtualHwBaseAddr);
   LLF_PKI_PKA_CopyDataFromPkaReg(SignatureD_ptr,  ordSizeInWords,  
                                    rT/*srcReg*/, VirtualHwBaseAddr);

                            
   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */
 End:  

   /* clear all PKA registers (0-31) for security goals */
 #ifndef LLF_PKI_PKA_DEBUG   
   LLF_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , 0/*Addr*/ , LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES/4 );
 #endif
  
   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );
   
     
   /* .... un mappping the physical memory to the virtual one and releasing the semaphore ...... */
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE ); /* 16 LS bit space - in */
   
 End_OnlySemRelease:
   
   DX_VOS_SemGive ( SemPkaId );
            
 Return: 

   return Error;


}/* END OF LLF_ECDSA_SignCalc */                                       



/****************************************************************************************
 *            LLF_ECDSA_Verify_Calc function	/ named ECVP_DSA in IEEE-1363 std.      *
 *            with internal or external ephemeral key   				*
 ****************************************************************************************/
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
CRYSError_t LLF_ECDSA_VerifyCalc( CRYS_ECPKI_PublKey_t *SignerPublKey_ptr,          /*in*/							
				DxUint32_t             *MessageRepresent_ptr,      /*in*/ 
				DxUint32_t              MessageRepresentSizeWords, /*in*/
				DxUint32_t             *SignatureC_ptr,            /*in*/
				DxUint32_t             *SignatureD_ptr,            /*in*/
				DxUint32_t             *TempBuff_ptr               /*in*/ )
  
 {
   /* LOCAL DECLARATIONS */
   
   /* errors identifier */
   CRYSError_t Error;
    
   /* EC points: G - generator, W - public key, P - temp  */
   LLF_ECPKI_PKA_Point_t  P1, P2;
   
   /* virtual pointers to PKA registers: private key rS, ephemeral private key rU, 
      message rF, EC order rR, temp registers rD etc. */
   DxInt8_t   rC, rD, rF, rR, rT, rT1, rT2, rN, rNp, rRp;

   /* list of PKA registers: 28 allowed for using : Note: numbers 0xFF - for debug goals */
   DxInt8_t regTemps[30] = {0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
                            0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
                            0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0xFF,0xFF }; 
   
   /* EC domain parameters */
   CRYS_ECPKI_DomainID_t  	   DomainID;
   const CRYS_ECPKI_Domain_t   *Domain_ptr;
   DxUint32_t  modSizeInBits, modSizeInWords, ordSizeInWords;
   
   /* indication variables */
   DxUint32_t isOnInfinity, status1, status2;
   
   DxUint32_t  VirtualHwBaseAddr = 0;
   
   
   /* for preventing compiler warnings */
   *TempBuff_ptr = *TempBuff_ptr;
   
   /* ................ initializtions  ............ */
   /* --------------------------------------------- */
   
   /* initialize the error */
   Error = CRYS_OK; 
   
   /* set domain parameters */
   DomainID = SignerPublKey_ptr->DomainID;
   Domain_ptr = &LLF_EC_DOMAINS_ARRAY[DomainID];
   modSizeInBits  = Domain_ptr->EC_ModulusSizeInBits;
   modSizeInWords = (modSizeInBits + 31)/32;
   ordSizeInWords = Domain_ptr->EC_OrderSizeInWords;
  
   /* set the PKA virtual pointers */
 
   /* order and Barrett tag. Note:  The rR, rRp pointers will be 
      exchanged with rN,rNp in step 3 */  
   rR  = 0;  
   rRp = 1;

   rN  = regTemps[0];    /* EC modulus */ 
   rNp  = regTemps[1];   /* temp for Barrett tag */
   rT1  = regTemps[2];
   rT2  = regTemps[3];   
   rT   = regTemps[4]; 
   rC   = regTemps[5];   /* signature C */
   
    /* temp points */
   P1.rX = regTemps[6];  
   P1.rY = regTemps[7];
   P1.rZ = regTemps[8];
 
   P2.rX = regTemps[9];   
   P2.rY = regTemps[10];
   P2.rZ = regTemps[11];

   /* these registers are owerlapped with temp points,
      because they are not used together */
   rF   = regTemps[6];   /* message representative */
   rD   = regTemps[7];   /* signature D */
   
 
   /* FUNCTION LOGIC */
   
   /* ............... getting the hardware semaphore ..................... */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK )   
     goto Return;

   /* ............... mapping the physical memory to the virtual one ...... */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );           /* The virtual address - out */
   if( Error != CRYS_OK )   
     goto End_OnlySemRelease;
     
   /* ----------------------------------------------------------------------- */
   /* .................... initialization of PKA  ........................... */
   /* ----------------------------------------------------------------------- */   
  
   /* initialize the PKA engine on default mode. Note: Registers size is set 
      by 1 word greate, than max modulus size for operations with EC order*/
   Error = LLF_PKI_PKA_DefaultInitPKA(modSizeInBits, CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+2, VirtualHwBaseAddr);

   if( Error != CRYS_OK )    
     goto End;
 
   /* set EC order exact and extended sizes in bits into RegSizes entry 2,3 */
   LLF_PKI_PKA_SetRegSize( Domain_ptr->EC_OrderSizeInBits, LenID_r, VirtualHwBaseAddr );
   LLF_PKI_PKA_SetRegSize( Domain_ptr->EC_OrderSizeInBits+32, LenID_r+1, VirtualHwBaseAddr );

    /* ----------------------------------------------------------------------- */
    /*............ setting the input data into PKA registers ..................*/
    /* ----------------------------------------------------------------------- */
    
    /* set the first EC order and its Barrett tag in registers 0, 1 for modular operations */
    LLF_PKI_PKA_CopyDataIntoPkaReg(rR/*dstReg*/, LenID_r+1, (DxUint32_t*)Domain_ptr->EC_GenerOrder/*src_ptr*/, 
                                   ordSizeInWords, VirtualHwBaseAddr);
                                   
    LLF_PKI_PKA_CopyDataIntoPkaReg(rRp/*dstReg*/, LenID_r+1, ((LLF_ecpki_pub_key_db_t *)SignerPublKey_ptr->crysPubKeyIntBuff)->RP/*src_ptr*/, 
                                   LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr );
 
    /* save modulus and its Barrett tag */
    LLF_PKI_PKA_CopyDataIntoPkaReg(rN/*dstReg*/, LenID_r+1, (DxUint32_t*)&(Domain_ptr->GF_Modulus)/*src_ptr*/, 
                                   modSizeInWords, VirtualHwBaseAddr);
    LLF_PKI_PKA_CopyDataIntoPkaReg(rNp/*dstReg*/, LenID_r+1, ((LLF_ecpki_pub_key_db_t *)SignerPublKey_ptr->crysPubKeyIntBuff)->NP/*src_ptr*/, 
                                   LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr );
    
    /* signature C */
    LLF_PKI_PKA_CopyDataIntoPkaReg(rC/*dstReg*/, LenID_r+1, SignatureC_ptr/*src_ptr*/, 
                                   ordSizeInWords, VirtualHwBaseAddr);
    /* signature D */   
    LLF_PKI_PKA_CopyDataIntoPkaReg(rD/*dstReg*/, LenID_r+1, SignatureD_ptr/*src_ptr*/, 
                                   ordSizeInWords, VirtualHwBaseAddr );
    
    /* message representative rF */
    LLF_PKI_PKA_CopyDataIntoPkaReg(rF/*dstReg*/, LenID_r+1, MessageRepresent_ptr/*src_ptr*/, 
                                   MessageRepresentSizeWords, VirtualHwBaseAddr);
    
   
    /* ----------------------------------------------------------------------- */
    /*  1. If  C or D are not in interval [1,r-1] then output "invalid"        */
    /* ----------------------------------------------------------------------- */
 
    /* temporary set rR = rR - 1 for the following checking */
    LLF_PKI_PKA_Flip0( LenID_r , rR/*OpA*/, rR/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
  
    /* check C */
    LLF_PKI_PKA_SubIm( LenID_r , rC/*OpA*/, 1/*OpB imm*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);
    LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status1 ); /* if rC >= 1, then status = 1 */    
    LLF_PKI_PKA_Sub( LenID_r , rR/*OpA*/, rC/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);
    LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status2 ); /* if rC <= rR, then status = 1 */
 
    if( status1 == 0 || status2 == 0 )
    {
    	Error = LLF_ECDSA_VERIFY_CALC_SIGN_C_INVALID_ERROR;
        goto End;    	
    }

    /* check D */
    LLF_PKI_PKA_SubIm( LenID_r , rD/*OpA*/, 1/*OpB imm*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);
    LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status1); /* if rC >= 1, then status = 1 */    
    LLF_PKI_PKA_Sub( LenID_r , rR/*OpA*/, rD/*OpB*/, RES_DISCARD, 0/*Tag*/, VirtualHwBaseAddr);
    LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status2); /* if rC <= rR, then status = 1 */
 
    if(status1 == 0 || status2 == 0){
    	Error = LLF_ECDSA_VERIFY_CALC_SIGN_D_INVALID_ERROR;
        goto End;    	
    }

    /* restore rR  */
    LLF_PKI_PKA_Flip0( LenID_r , rR/*OpA*/, rR/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);

    /* ----------------------------------------------------------------------- */
    /* 2. Calculate h, h1, h2 and normalize rF                                 */
    /* ----------------------------------------------------------------------- */

    /* 2.1. rT = h = d^-1  mod r */    
    LLF_PKI_PKA_ModInv( LenID_r , rD/*OpB*/, rT/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
    /* 2.2. rT1 = h1 = f*h  mod r */
    LLF_PKI_PKA_Div( LenID_r, rF/*OpA=rem*/, rR/*OpB*/, rT1/*Res not used*/, 0/*Tag*/, VirtualHwBaseAddr ); 
    LLF_PKI_PKA_ModMul( LenID_r, rF/*OpA*/, rT/*OpB*/, rT1/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
    /* 2.3. rT2 = h2 = c*h mod r  */
    LLF_PKI_PKA_ModMul( LenID_r, rC/*OpA*/, rT/*OpB*/, rT2/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
    
    /* ----------------------------------------------------------------------- */
    /* 3. Compute EC points   P1 =  h1*G,  P2 = h2*W, P3 = P1 + P2             */ 
    /* ----------------------------------------------------------------------- */
  
    /* 3.1. Set 0,1 registers according to EC modulus and its Barrett tag by swapping */
    LLF_PKI_PKA_SwapRegsPhisAddr( &rN, &rR, &rNp, &rRp, VirtualHwBaseAddr ); 

    /* P1 =  h1*G */
    LLF_ECPKI_SetPoint((DxUint32_t*)(Domain_ptr->EC_Gener_X)/*X*/,  
                       (DxUint32_t*)(Domain_ptr->EC_Gener_Y)/*Y*/, (DxUint32_t*)DX_NULL/*Z*/,
                       modSizeInWords, LenID_r, &P1/*Res*/, EC_PointJacobian, VirtualHwBaseAddr);                                    

    Error = LLF_ECPKI_GFpScalarMultProj(rT1, &P1/*EC_generator*/, &P1/*Res*/, DomainID,
					regTemps+12, 28-12/*countTemps*/, VirtualHwBaseAddr);
    if( Error != CRYS_OK )
	    goto End;    	

    /* 3.2. P2 =  h2*W */
    LLF_ECPKI_SetPoint( SignerPublKey_ptr->PublKeyX/*X*/,  
                        SignerPublKey_ptr->PublKeyY/*Y*/, (DxUint32_t*)DX_NULL/*Z*/,
                        modSizeInWords, LenID_r, &P2/*Res*/, EC_PointJacobian, VirtualHwBaseAddr);                                    

    Error = LLF_ECPKI_GFpScalarMultProj(rT2, &P2/*EC_generator*/, &P2/*Res*/, DomainID,
					regTemps+12, 28-12/*countTemps*/, VirtualHwBaseAddr);
    if( Error != CRYS_OK )   
	    goto End;    	

    /* 3.3. P3 = P1 + P2 */
    Error = LLF_ECPKI_FullAdd(&P1, &P2, &P2/*Res*/, DomainID,  
			regTemps+12, 28-12/*countTemps*/, VirtualHwBaseAddr);
 
    if( Error != CRYS_OK ) 
	    goto End;
    
    /* convert public key to affine ordinary representation */
    Error = LLF_ECPKI_GFpJacobianToAffine(&P2, &P2, DomainID, 
                                          regTemps+12, 28-12, VirtualHwBaseAddr);  
    if( Error != CRYS_OK ) 
	    goto End;


    /* 3.4. Check that P3 != O */
    isOnInfinity = LLF_ECPKI_IsProjPointOnInfinity(LenID_n, &P2, VirtualHwBaseAddr);
    
    if(isOnInfinity == 1){
    	Error = LLF_ECDSA_VERIFY_CALC_POINT_ON_INFINITY_ERROR;
    	goto End;
    }

    /* ----------------------------------------------------------------------- */
    /* 4. Normalize: C' = P2.rX mod r. Compare C' == C                         */ 
    /* ----------------------------------------------------------------------- */
    LLF_PKI_PKA_Div( LenID_r, P2.rX/*OpA=rem*/, rR/*OpB*/, rT1/*Res not used*/, 0/*Tag*/, VirtualHwBaseAddr);
 
    LLF_PKI_PKA_Compare( LenID_r, P2.rX/*dst*/, rC/*src*/, 0/*Tag*/, VirtualHwBaseAddr);
    LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status1);
    
    if(status1 != 1){
    	Error = LLF_ECDSA_VERIFY_CALC_SIGNATURE_IS_INVALID;
    }

   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */
 End:  

   
 #ifndef LLF_PKI_PKA_DEBUG    
   /* clear all PKA registers (0-31) for security goals */
   LLF_PKI_HW_CLEAR_PKA_MEM(VirtualHwBaseAddr , 0/*Addr*/ , LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES/4);
 #endif
  
   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );
   
     
   /* .... un mappping the physical memory to the virtual one and releasing the semaphore ...... */
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE ); /* 16 LS bit space - in */
   
 End_OnlySemRelease:
   
   DX_VOS_SemGive ( SemPkaId );
            
 Return: 

   return Error;
 

}/* END OF LLF_ECDSA_VerifyCalc */  

