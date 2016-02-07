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
#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif
#include "crys_common_math.h"
#include "crys_common.h"
#include "llf_pki.h"
#include "llf_pki_error.h"
#include "llf_pki_priv_key_db_def.h"

#include "dx_vos_mem.h"
#include "dx_cc_regs.h"
#include "llf_pki_hw_defs.h"
#include "dx_hal_plat.h"

/************************ Defines **********************************/

/* canceling the lint warning:
   Info 717: do ... while(0) */
/*lint --e{717} */

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/************************ Enums ************************************/

/************************ Typedefs *********************************/

/************************ Global Data ******************************/

/* importing the base address of the CRYPTO Cell . this value must be 
   initialized in the CRYS_Init function */
//extern DxUint32_t PLAT_CryptoCellBaseAddr;



#ifdef LLF_PKI_PKA_DEBUG
extern DxUint32_t tempRes[LLF_PKI_PKA_MAX_REGISTER_SIZE_WORDS + 1];
#endif


/************* Exported functions prototypes *************************/




/*******************************************************************************************/
/**
 * @brief This function executes the RSA primitive: private key CRT exponent
 *         
 *    Algorithm [PKCS #1 v2.1]:
 *
 *     1. If NonCRT exponent, then  M  =  C^D  mod N. 
 *          
 *     Where: M- message representative, C- ciphertext, D- priv.exponent, N- modulus,
 *            ^ - exponentiation symbol.
 *    
 *     Note: PKA registers used: r0,r1,r2,r3,r4,  r30,r31.
 *
 * @param[in]     PubKey_ptr    - the private key database.
 * @param[in/out] PrivData_ptr  - the structure, containing DataIn and DataOut buffers.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_ExecPrivKeyExpNonCrt( 
                                     CRYSRSAPrivKey_t    *PrivKey_ptr ,
                                     CRYS_RSAPrimeData_t *PrivData_ptr,
                                     DxUint32_t           VirtualHwBaseAddr )
{
   /* LOCAL DECLARATIONS */

  /* error identification */
   CRYSError_t Error;
   
   /* modulus and exponents sizes in bytes */ 
   DxUint32_t modSizeWords, dSizeInWords;

#ifndef CRYS_NO_RSA_SELF_TEST_SUPPORT
   DxUint32_t eSizeInWords;
   /* PKA status */
   DxUint32_t  status;
#endif
   
   /* FUNCTION LOGIC */
   
   /* .................... initialize local variables ...................... */
   /* ---------------------------------------------------------------------- */
   
   /* initialize the error identifier to success */
   Error = CRYS_OK;

   /* modulus N size in bytes */ 
   modSizeWords = (PrivKey_ptr->nSizeInBits + 31) / 32;

   /* priv. exponent size in bytes */
   dSizeInWords = (PrivKey_ptr->PriveKeyDb.NonCrt.dSizeInBits + 31) / 32;   

   /* check that there is enough memory */ 
   if(LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES < 7*(modSizeWords+1)*sizeof(DxUint32_t))
      return LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR;

   /* initialize the PKA engine on default mode */
   Error = LLF_PKI_PKA_DefaultInitPKA(PrivKey_ptr->nSizeInBits, modSizeWords+1, VirtualHwBaseAddr);
   if( Error != CRYS_OK )     
		return Error;

   /* -------------------------------------------------------------------- */
   /*      copy the N, Np DataIn and D into PKA registers                  */
   /* -------------------------------------------------------------------- */
   /* N => r0 */
   /* copy modulus N into PKA register: N=>r0 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 0/*dstReg*/, 1/*LenID*/, PrivKey_ptr->n/*src_ptr*/, 
                                   modSizeWords , VirtualHwBaseAddr );
   /* copy Barrett tag NP into PKA register: NP=>r1 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 1/*dstReg*/, 1/*LenID*/, ((LLF_pki_priv_key_db_t*)(PrivKey_ptr->crysRSAPrivKeyIntBuff))->NonCrt.NP/*src_ptr*/, 
                                   LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr);

   /* copy input data into PKI register: DataIn=>r2 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 2/*dstReg*/, 1/*LenID*/, PrivData_ptr->DataIn/*src_ptr*/,  
                                   modSizeWords , VirtualHwBaseAddr);
                                   
   /* copy priv. exponent D into PKA register: D=>r3 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 3/*dstReg*/, 1/*LenID*/, PrivKey_ptr->PriveKeyDb.NonCrt.d/*src_ptr*/, 
                                   dSizeInWords , VirtualHwBaseAddr);

  /* -------------------------------------------------------------------- */
  /* .. calculate the exponent: DataOut= Res = DataIn^D mod N;        ... */
  /* -------------------------------------------------------------------- */
  LLF_PKI_PKA_ModExp( 0/*LenID*/, 2/*OpA*/, 3/*OpB*/, 4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
                                   
  /*--------------------------------------------------------------------------*/    
  /* ................ START : Countermeasuers DFA test    ................... */
  /*--------------------------------------------------------------------------*/
#ifndef CRYS_NO_RSA_SELF_TEST_SUPPORT

   /* set public exponent into PKA reg. r3 for Counter measuers DFA, if necessary */ 
   if(PrivKey_ptr->PriveKeyDb.NonCrt.eSizeInBits != 0)
   {
       /* publ. exponent size in bytes */
       eSizeInWords = (PrivKey_ptr->PriveKeyDb.NonCrt.eSizeInBits + 31) / 32;
	   /* copy publ. e=> r3 */
       LLF_PKI_PKA_CopyDataIntoPkaReg( 3/*dstReg*/, 1/*LenID*/, PrivKey_ptr->PriveKeyDb.NonCrt.e/*src_ptr*/, 
                                       (eSizeInBytes+3)/4, VirtualHwBaseAddr );

	/*    Execue the inverse exponent operation Res1=>r3:               */
	/*          Res1 = Res^e mod N      (Note: D=r3 not needed)         */
	LLF_PKI_PKA_ModExp( 0/*LenID*/, 4/*OpA*/, 3/*OpB*/, 3/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
				    
	/* if the result is not the same as the original DataIn, set the test as failure */		
	LLF_PKI_PKA_Compare(1/*LenID*/, 2/*OpA*/, 3/*OpB*/, 0/*Tag*/, VirtualHwBaseAddr);
       
        /* read zero bit from the status; if Z != 1 then test fail */
        LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);       
        if( status != 1 )
        { 
		  #ifndef CRYS_NO_FIPS_SUPPORT
			DX_GLOBAL_FIPS_MODE |= DX_CRYS_FIPS_MODE_ERROR_STATE;			 
		  #endif
		     
			Error = LLF_PKI_COUNTERMEASURES_DFA_FAIL_ERROR;
			
			goto End;
        }
        
	} /* end if eSizeInBits!=0 */

#endif /*CRYS_NO_RSA_SELF_TEST_SUPPORT*/

  
   /*-----------------------------*/
   /*  Finish PKA and copy result */
   /*-----------------------------*/       

    /* copy result into output buffer */
    LLF_PKI_PKA_CopyDataFromPkaReg(PrivData_ptr->DataOut, modSizeWords, 4/*srcReg*/, VirtualHwBaseAddr);

#ifndef CRYS_NO_RSA_SELF_TEST_SUPPORT
End:
#endif                                   
   /* clear used PKA registers (0-5 and 30,31) for security goals */
   LLF_PKI_PKA_ClearBlockOfRegs( 0/*FirstReg*/, 5/*Count*/, 1/*LenID*/, VirtualHwBaseAddr);
   LLF_PKI_PKA_ClearBlockOfRegs(30/*FirstReg*/, 2/*Count*/, 1/*LenID*/, VirtualHwBaseAddr);

   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );

   return Error;
   
}/* END OF LLF_PKI_ExecPrivKeyExpNonCrt */                                              


/* Define count of registers used in LLF_PKI_ExecPrivKeyExpCrtLowMem() function*/
#define CRT_BIG_MEM_MIN_COUNT_REGS  10   
#if (CRT_BIG_MEM_MIN_COUNT_REGS*LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS >= 8*LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES)
/*******************************************************************************************/
/**
 * @brief This function executes the RSA primitive private key CRT exponent,
 *        adapted for long Keys, great than 2K bits.   
 *         
 *    Algorithm [PKCS #1 v2.1]:
 *
 *   CRT exponentiation algorithm:  
 *        1. Mq  =  C^dQ mod Q;         
 *        2. Mp  =  C ^dP mod P, 
 *        3  h = (Mp-Mq)*qInv mod P;
 *        4. M = Mq + Q * h. 
 *
 *     Where: M- message representative, C- ciphertext, D- priv.exponent, N- modulus,
 *            P,Q,dP,dQ, qInv - CRT private key parameters;
 *            ^ - exponentiation symbol.
 *    
 *     Note: 7 PKA registers are used: r0-r4, r30,r31.
 *
 * @param[in]     PubKey_ptr    - the private key database.
 * @param[in/out] PrivData_ptr  - the structure, containing DataIn and DataOut buffers.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_ExecPrivKeyExpCrtLowMem( 
                                     CRYSRSAPrivKey_t    *PrivKey_ptr ,
                                     CRYS_RSAPrimeData_t *PrivData_ptr,
                                     DxUint32_t           VirtualHwBaseAddr )
{
   /* LOCAL DECLARATIONS */

	/* set N_NP_T0_T1 register, so that N->0 NP->2 T0->12 T1->14 */
	#define rN   0
	#define rNp  2
	#define rT0 10
	#define rT1 12
	#define N_NP_T0_T1  (((rT1<<5|rT0)<<5|rNp)<<5|rN)
	#define LenID_PQ  0	 	
	#define LenID_PQ1 1	 	
	#define LenID_N   2
	#define LenID_N1  3
  /* error identification */
   CRYSError_t Error;
   
   /* modulus and exponents sizes in bytes */ 
   DxUint32_t  modSizeWords, PQSizeInWords;
   
   /* PKA tag - used for internal goals: if Tag == 1 and PKA debug flag is defined, then
      the PKA operation copies data in special buffer for debugging */
   DxUint8_t Tag = 1; 
	volatile DxUint32_t r=N_NP_T0_T1;  
   
   
   /* FUNCTION LOGIC */
   
   /* ---------------------------------------------------------------------- */
   /* .................... initializations            ...................... */
   /* ---------------------------------------------------------------------- */
   
   /* error identifier */
   Error = CRYS_OK;

   /* modulus N size in bytes */ 
   modSizeWords = (PrivKey_ptr->nSizeInBits + 31) / 32;
   
   /* check that there is enough memory for 7 PKA registers needed for 
      CRT exponentiation */ 
   if( LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES < 7*(modSizeBytes+4) )
      return LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR;

	Error = LLF_PKI_PKA_InitPkaEx(PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits, 
	                              VirtualHwBaseAddr);
	if( Error != CRYS_OK )     
		return Error;
	/*  update the Sizes table: 2- Nsize, 3- Nsize+1w; 0- PQSize, 1- PQSize+1w (done)  */
	LLF_PKI_PKA_SetRegSize( PrivKey_ptr->nSizeInBits,    LenID_N/*EntryNum*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_SetRegSize( PrivKey_ptr->nSizeInBits+32, LenID_N1/*EntryNum*/, VirtualHwBaseAddr );
	
   /* update N_NP_T0_T1 addresses of rT0 and rT1 registers according to double sizes */
   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), (DxUint32_t)N_NP_T0_T1);


   /* P and Q size in bytes */ 
   PQSizeInWords = (PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits + 31) / 32;
   
   /*--------------------------------------------------------------*/
   /* PKA modular operations  according to modulus Q:              */
   /*--------------------------------------------------------------*/

   /* copy Q->0, DataIn->r3  */
	LLF_PKI_PKA_CopyDataIntoPkaReg(0/*dstReg*/, LenID_N1, PrivKey_ptr->PriveKeyDb.Crt.Q/*src_ptr*/, 
	                               PQSizeInWords , VirtualHwBaseAddr);
	LLF_PKI_PKA_CopyDataIntoPkaReg(6/*3dstReg*/, LenID_N1, PrivData_ptr->DataIn/*src_ptr*/, 
	                               modSizeWords , VirtualHwBaseAddr);
 
	/* reduction of the input data:  r3 = dataIn mod Q */
	LLF_PKI_PKA_Div(LenID_N, 6/*3OpA*/, 0/*OpB=r0=Q*/, 2/*1Res not used*/, 1/*Tag*/, VirtualHwBaseAddr);
	LLF_PKI_PKA_Clr( LenID_N1, 2/*dest*/, Tag, VirtualHwBaseAddr );
   
	/* copy Qp->1, dQ->2 into PKA registers */
	LLF_PKI_PKA_CopyDataIntoPkaReg( 2/*1dstReg*/, LenID_N1, PrivKey_ptr->LLF.Crt.QP/*src_ptr*/, 
	                               LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS , VirtualHwBaseAddr );

	LLF_PKI_PKA_CopyDataIntoPkaReg(4/*2dstReg*/, LenID_N1, PrivKey_ptr->PriveKeyDb.Crt.dQ/*src_ptr*/, 
	                               PQSizeInWords , VirtualHwBaseAddr);

	/*  calculate the Mq = r3 = DataIn^dQ mod Q:  */
	LLF_PKI_PKA_ModExp(LenID_PQ, 6/*3OpA*/, 4/*2OpB*/, 6/*3Res*/, Tag, VirtualHwBaseAddr);

   /*--------------------------------------------------------------*/
   /* PKA modular operations  according to modulus P:              */
   /*--------------------------------------------------------------*/
   
	/* copy P->r0, DataIn->r4, */
	LLF_PKI_PKA_CopyDataIntoPkaReg(0/*dstReg*/, LenID_N, PrivKey_ptr->PriveKeyDb.Crt.P/*src_ptr*/, 
	                               PQSizeInWords , VirtualHwBaseAddr );
                                   
	LLF_PKI_PKA_CopyDataIntoPkaReg(8/*4dstReg*/, LenID_N, PrivData_ptr->DataIn/*src_ptr*/, 
	                               modSizeWords , VirtualHwBaseAddr);

	/* reduction of input data by modulus P:  r4 = rT1 mod P  */
	LLF_PKI_PKA_Div( LenID_N, 8/*4OpA and remainder*/, 0/*OpB*/, 2/*1res not used*/, 0/*Tag*/, VirtualHwBaseAddr );
	LLF_PKI_PKA_Clr( LenID_N1, 2/*dest*/, Tag, VirtualHwBaseAddr );
   
	/* copy PP->r1, dP->r2 */
	LLF_PKI_PKA_CopyDataIntoPkaReg(2/*1dstReg*/, LenID_N, PrivKey_ptr->LLF.Crt.PP/*src_ptr*/, 
	                               LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS , VirtualHwBaseAddr);

	LLF_PKI_PKA_CopyDataIntoPkaReg(4/*2dstReg*/, LenID_N, PrivKey_ptr->PriveKeyDb.Crt.dP/*src_ptr*/, 
	                               PQSizeInWords , VirtualHwBaseAddr);
   
   /*-----------------------------*/
   /*  Calculation of Mp          */
   /*-----------------------------*/  
    
	/* calculate exponent Mp = DataIn^dP mod P , i.e: r4 = r4^r2 mod r0  */	   	      
	LLF_PKI_PKA_ModExp(LenID_PQ, 8/*4OpA*/, 4/*2exp*/, 8/*4Res*/, Tag, VirtualHwBaseAddr );

   /*-------------------------------------------*/
   /* Calculation of  h = (Mp-Mq)*qInv mod P    */
   /*-------------------------------------------*/
	/* r3 = Mq mod P - needed for right calculating in next operation if Mq>P */
	LLF_PKI_PKA_ModAddIm(LenID_PQ, 6/*3OpA*/, 0/*immed OpB*/, 6/*3Res*/, Tag, VirtualHwBaseAddr); 

	/* r4 = Mp - Mq mod P */
	LLF_PKI_PKA_ModSub(LenID_PQ, 8/*4OpA*/, 6/*3OpB*/, 8/*4Res*/, Tag, VirtualHwBaseAddr);
   
	/* copy: qInv->2 */
	LLF_PKI_PKA_CopyDataIntoPkaReg( 4/*2dstReg*/, LenID_N1, PrivKey_ptr->PriveKeyDb.Crt.qInv/*src_ptr*/, 
	                               PQSizeInWords , VirtualHwBaseAddr );
   

	/* h = r4 = (Mp - Mq)*qInv mod P */
	LLF_PKI_PKA_ModMul( LenID_PQ, 8/*4OpA*/, 4/*2rqInv*/, 8/*4Res*/, Tag, VirtualHwBaseAddr );

   /*-----------------------------*/
   /*   M = Mq+Q*h = r3+r1*r4     */
   /*  OpSize according Nsize     */
   /*-----------------------------*/
  
	/* copy: Q->1 */	
	LLF_PKI_PKA_CopyDataIntoPkaReg( 2/*1dstReg*/, LenID_N1, PrivKey_ptr->PriveKeyDb.Crt.Q/*src_ptr*/, 
	                               PQSizeInWords , VirtualHwBaseAddr );

	/* Q*h => r4 = r1*r4 */
	LLF_PKI_PKA_LMul( LenID_N, 2/*1OpA*/, 8/*4OpB*/, 8/*4Res*/, Tag, VirtualHwBaseAddr );


	/* M = r3 = r3 + r4 */
	LLF_PKI_PKA_Add( LenID_N, 6/*3OpA*/, 8/*4OpB*/, 8/*4Res*/, Tag, VirtualHwBaseAddr );

   /*-----------------------------*/
   /*  Finish PKA and copy result */
   /*-----------------------------*/
	LLF_PKI_PKA_CopyDataFromPkaReg(PrivData_ptr->DataOut,  modSizeWords, 8/*4srcReg*/, VirtualHwBaseAddr );
                                   
 #ifndef LLF_PKI_PKA_DEBUG_   
    LLF_PKI_HW_CLEAR_PKA_MEM(VirtualHwBaseAddr , LLF_PKI_PKA_DATA_REGS_BASE_ADDR , LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES/4);
 #endif

   return Error;
   
	#undef rN
	#undef rNp
	#undef rT0 
	#undef rT1	
	#undef N_NP_T0_T1 	
	#undef LenID_PQ    
	#undef LenID_PQ1   
	#undef LenID_N  
	#undef LenID_N1 

}/* END OF LLF_PKI_ExecPrivKeyExpCrtLowMem */

#endif


/*******************************************************************************************/
/**
 * @brief This function executes the RSA primitive: private key CRT exponent.
 *        adapted for Keys up to 2K bits size.   
 *         
 *    Algorithm [PKCS #1 v2.1]:
 *
 *   CRT exponentiation algorithm:  
 *        1. Mq  =  C^dQ mod Q;         
 *        2. Mp  =  C ^dP mod P, 
 *        3  h = (Mp-Mq)*qInv mod P;
 *        4. M = Mq + Q * h. 
 *
 *     Where: M- message representative, C- ciphertext, D- priv.exponent, N- modulus,
 *            P,Q,dP,dQ, qInv - CRT private key parameters;
 *            ^ - exponentiation symbol.
 *    
 *     Note: 9 PKA registers are used: r0-r6,  r30,r31.
 *
 * @param[in]     PubKey_ptr    - the private key database.
 * @param[in/out] PrivData_ptr  - the structure, containing DataIn and DataOut buffers.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_ExecPrivKeyExpCrt_( 
                                     CRYSRSAPrivKey_t    *PrivKey_ptr ,
                                     CRYS_RSAPrimeData_t *PrivData_ptr,
                                     DxUint32_t           VirtualHwBaseAddr )
{
   /* LOCAL DECLARATIONS */

  /* error identification */
   CRYSError_t Error;
   
   /* modulus and exponents sizes in bytes */ 
   DxUint32_t  modSizeWords, PQSizeInWords;
   
   /* virtual registers pointers 
      Note: don't change rQ = 6  */
   DxInt8_t  rN=0, rNP=1, rD=2, rT=3, rT1=4, rMq=5, rQ=6, rqInv=7;
   
   /* PKA tag - used for internal goals: if Tag == 1 and PKA debug flag is defined, then
      the PKA operation copies data in special buffer for debugging */
   DxUint8_t Tag = 1; 
   
   
   /* FUNCTION LOGIC */
   
   /* ---------------------------------------------------------------------- */
   /* .................... initializations            ...................... */
   /* ---------------------------------------------------------------------- */
   
   /* error identifier */
   Error = CRYS_OK;

   /* modulus N size in bytes */ 
   modSizeWords = (PrivKey_ptr->nSizeInBits + 31) / 32;
   
   /* check that there is enough memory for 10 PKA registers needed for 
      CRT exponentiation */ 
   if(LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES < 10*(modSizeWords+1)*sizeof(DxUint32_t))
      return LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR;

   /* initialize the PKA engine on default mode */
   Error = LLF_PKI_PKA_DefaultInitPKA(PrivKey_ptr->nSizeInBits,
                                      (PrivKey_ptr->nSizeInBits>>5)+1, VirtualHwBaseAddr);
   if( Error != CRYS_OK )     
         return Error;
                               
   /*  set Sizes table: 0- Nsize, 1- Nsize+1w (is done), 2- Psize  */
   LLF_PKI_PKA_SetRegSize(PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits,    2/*EntryNum*/, VirtualHwBaseAddr);
   LLF_PKI_PKA_SetRegSize(PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits+32, 3/*EntryNum*/, VirtualHwBaseAddr);
   
   /* P and Q size in bytes */ 
   PQSizeInWords = (PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits + 31) / 32;
   
   /*--------------------------------------------------------------*/
   /* PKA modular operations  according to modulus Q:              */
   /*--------------------------------------------------------------*/

   /* copy CRT parametersrs Q, dQ, QP into PKA registers */
   LLF_PKI_PKA_CopyDataIntoPkaReg(rN/*0 dstReg*/, 1/*LenID*/, PrivKey_ptr->PriveKeyDb.Crt.Q/*src_ptr*/, 
                                  PQSizeInWords , VirtualHwBaseAddr );

   LLF_PKI_PKA_CopyDataIntoPkaReg(rD/*2 dstReg*/, 1/*LenID*/, PrivKey_ptr->PriveKeyDb.Crt.dQ/*src_ptr*/, 
                                  PQSizeInWords , VirtualHwBaseAddr);
                                                                
   LLF_PKI_PKA_CopyDataIntoPkaReg(rNP/*1 dstReg*/, 1/*LenID*/, ((LLF_pki_priv_key_db_t*)(PrivKey_ptr->crysRSAPrivKeyIntBuff))->Crt.QP/*src_ptr*/, 
                                  LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS , VirtualHwBaseAddr);
   /* copy DataIn into rT and rT! */
   LLF_PKI_PKA_CopyDataIntoPkaReg(rT/*3 dstReg*/, 1/*LenID*/, PrivData_ptr->DataIn/*src_ptr*/, 
                                   modSizeWords , VirtualHwBaseAddr);
   LLF_PKI_PKA_Copy(1/*LenID*/, rT1/*4 dest*/, rT/*3 src*/, 0/*Tag*/, VirtualHwBaseAddr);
  
   /*-----------------------------*/
   /*  Calculation of Mq          */
   /*-----------------------------*/
   /* reduction of the input data by modulus Q  rT = rT mod Q */
   LLF_PKI_PKA_Div(0/*LenID*/, rT/*3 OpA*/, rN/*0 OpB=rN=Q*/, rQ/*6 Res not used*/, 1/*Tag*/, VirtualHwBaseAddr);
  
   /*  calculate of Mq = DataIn^dQ mod Q: Mq = rT^rD mod rN        */
   LLF_PKI_PKA_ModExp(2/*LenID*/, rT/*3 OpA*/, rD/*2 OpB*/, rMq/*5 Res*/, Tag, VirtualHwBaseAddr);

   /*--------------------------------------------------------------*/
   /* PKA modular operations  according to modulus P:              */
   /*--------------------------------------------------------------*/
   
   /* copy prime factor P into rQ for swapping with rN */
   LLF_PKI_PKA_CopyDataIntoPkaReg(rQ/*6 dstReg*/, 1/*LenID*/, PrivKey_ptr->PriveKeyDb.Crt.P/*src_ptr*/, 
                                  PQSizeInWords, VirtualHwBaseAddr);
   /* swap rQ <-> rN so that Q->rQ and P->rN */
   rQ = 0; rN = 6;   
   
   /* set new value to N_NP_TO_T1 register according N->6, Np->1,T0->30,T1->31: 0x000FF826*/
   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), 0x000FF826);

   /* copy Barrett tag PP: PP=>NP */                            
   LLF_PKI_PKA_CopyDataIntoPkaReg(rNP/*1 dstReg*/, 1/*LenID*/, ((LLF_pki_priv_key_db_t*)(PrivKey_ptr->crysRSAPrivKeyIntBuff))->Crt.PP/*src_ptr*/, 
                                  LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS , VirtualHwBaseAddr);

   /* copy priv. exponent factor dP: dP=>rD */
   LLF_PKI_PKA_CopyDataIntoPkaReg(rD/*2 dstReg*/, 1/*LenID*/, PrivKey_ptr->PriveKeyDb.Crt.dP/*src_ptr*/, 
                                  PQSizeInWords, VirtualHwBaseAddr);
   
   /* copy qInv coefficient: qInv=>rqInv   */
   LLF_PKI_PKA_CopyDataIntoPkaReg(rqInv/*7 dstReg*/, 1/*LenID*/, PrivKey_ptr->PriveKeyDb.Crt.qInv/*src_ptr*/, 
                                  PQSizeInWords , VirtualHwBaseAddr);

   /*-----------------------------*/
   /*  Calculation of Mp          */
   /*-----------------------------*/
   /* reduction of input data by modulus P:  rT = rT1 mod P  */
   LLF_PKI_PKA_Div(0/*LenID*/, rT1/*4 OpA and remainder*/, rN/*0 OpB*/, rT/*3 res not used*/, 0/*Tag*/, VirtualHwBaseAddr);
   
   /* calculate exponent Mp = DataIn^dP mod P , i.e: rT = rT1^rD mod rP  */	   	      
   LLF_PKI_PKA_ModExp(2/*LenID*/, rT1/*4 OpA*/, rD/*2 exp*/, rT/*3 Res*/, Tag, VirtualHwBaseAddr);

   /*-------------------------------------------*/
   /* Calculation of  h = (Mp-Mq)*qInv mod P    */
   /*-------------------------------------------*/
   
   /* rT1 = Mq mod P - needed for right calculating in next operation if Mq>P */
   LLF_PKI_PKA_ModAddIm( 2/*LenID*/, rMq/*5 OpA*/, 0/*immed OpB*/, rT1/*Res*/, Tag, VirtualHwBaseAddr ); 

   /* rT = Mp - Mq mod P */
   LLF_PKI_PKA_ModSub(2/*LenID*/, rT/*OpA*/, rT1/*OpB*/, rT/*Res*/, Tag, VirtualHwBaseAddr);
   
   /* rT1 = h = (Mp - Mq)*qInv mod P */
   LLF_PKI_PKA_ModMul(2/*LenID*/, rT/*OpA*/, rqInv/*rqInv*/, rT1/*Res*/, Tag, VirtualHwBaseAddr);

   /*-----------------------------*/
   /*       M = Mq + Q*h;         */
   /*  OpSize according Nsize     */
   /*-----------------------------*/

   /* copy rT1 and Mq in other registers for clearing junk from registers high part  */
   LLF_PKI_PKA_Clr(1/*LenID*/, 30/*dest*/, Tag, VirtualHwBaseAddr);
   LLF_PKI_PKA_Copy(2/*LenID*/, rT/*dest*/, rT1/*src*/, 0/*Tag*/, VirtualHwBaseAddr);
   LLF_PKI_PKA_Clr(1/*LenID*/, 30/*dest*/, Tag, VirtualHwBaseAddr);
   LLF_PKI_PKA_Copy( 2/*LenID*/, rT1/*dest*/, rMq/*src*/, 0/*Tag*/, VirtualHwBaseAddr);

   /* Q*h => rT = rQ*rT */
   LLF_PKI_PKA_LMul(0/*LenID*/, rT/*OpA*/, rQ/*OpB*/, rT/*Res*/, Tag, VirtualHwBaseAddr);
   /* M = rT1 = rMq + rT */
   LLF_PKI_PKA_Add(0/*LenID*/, rT1/*OpA*/, rT/*OpB*/, rT/*Res*/, Tag, VirtualHwBaseAddr);

   /*-----------------------------*/
   /*  Finish PKA and copy result */
   /*-----------------------------*/
   LLF_PKI_PKA_CopyDataFromPkaReg(PrivData_ptr->DataOut,  modSizeWords, rT/*srcReg*/, VirtualHwBaseAddr);
                                   
 #ifndef LLF_PKI_PKA_DEBUG_   
   /* clear used PKA registers (0-7 and 30,31) for security goals */
   LLF_PKI_PKA_ClearBlockOfRegs(0/*FirstReg*/, 8/*Count*/, 1/*LenID*/, VirtualHwBaseAddr);
   LLF_PKI_PKA_ClearBlockOfRegs(30/*FirstReg*/, 2/*Count*/, 1/*LenID*/, VirtualHwBaseAddr);
 #endif

   return Error;
   
}/* END OF LLF_PKI_ExecPrivKeyExpCrt_ */


/*******************************************************************************************/
/**
 * @brief This function calls the specific RSA primitives for CRT exponentiation,
 *        adapted for small Keys (up to 2K bits) or large Keys (up to 4K bits).   
 *         
 *    Algorithm :
 *
 *   1. Check the Key size.
 *   2. If defined compilation flag CRYS_NO_RSA_MAX_KEY_SIZE_4096_BIT_SUPPORT, then
 *      calls the function LLF_PKI_ExecPrivKeyExpCrt_();
 *   3. I the said flag is not defined, then: 
 *        if the Key size <= 2K bits, then calls the function LLF_PKI_ExecPrivKeyExpCrt_(), 
 *        else the function LLF_PKI_ExecPrivKeyExpCrtLowMem();      
 *
 * @param[in]     PubKey_ptr    - the private key database.
 * @param[in/out] PrivData_ptr  - the structure, containing DataIn and DataOut buffers.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_ExecPrivKeyExpCrt( 
                                     CRYSRSAPrivKey_t    *PrivKey_ptr ,
                                     CRYS_RSAPrimeData_t *PrivData_ptr,
                                     DxUint32_t           VirtualHwBaseAddr) 
{
	CRYSError_t error;

#if (CRT_BIG_MEM_MIN_COUNT_REGS*LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS > 8*LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES)

	if(PrivKey_ptr->nSizeInBits > 2048)
		error = LLF_PKI_ExecPrivKeyExpCrtLowMem(PrivKey_ptr, PrivData_ptr, VirtualHwBaseAddr);
	else
#endif
		error = LLF_PKI_ExecPrivKeyExpCrt_(PrivKey_ptr, PrivData_ptr, VirtualHwBaseAddr);
  

    return error;	
	
} /* END OF LLF_PKI_ExecPrivKeyExpCrt */
