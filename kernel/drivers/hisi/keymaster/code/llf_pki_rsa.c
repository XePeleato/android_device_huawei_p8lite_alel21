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

#include "dx_vos_mem.h"
#include "dx_vos_memmap.h"
#include "dx_vos_basetypes.h"

#include "crys_common_math.h"
#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif
#include "plat_print_dbg.h"

#include "crys_rsa_local.h"
#include "llf_pki.h"
#include "llf_pki_error.h"
#include "llf_pki_rsa.h"
#include "llf_pki_pub_key_db_def.h"
#include "llf_pki_priv_key_db_def.h"
#include "llf_pki_key_gen_db_def.h"
#include "dx_vos_sem.h"


/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/* canceling the lint warning:
Info 506: Constant value Boolean ... */
/*lint --e{506} */

/* canceling the lint warning:
Info 774: Boolean within 'if' always evaluates to False */
/*lint --e{774} */


/************************ Defines ***********************************/

/************************ Enums *************************************/

/************************ Typedefs **********************************/


/************************ Global Data *******************************/

/* importing the base address of the CRYPTO Cell . this value must be initialized in the 
   CRYS_Init function */
extern DxUint32_t PLAT_CryptoCellBaseAddr;

#ifndef DX_OEM_FW
extern DX_PAL_SEM SemPkaId;
#else //DX_OEM_FW
#define DX_VOS_SemWait(SemPkaId,timeout)                CRYS_OK
#define DX_VOS_SemGive(SemPkaId)
#endif //DX_OEM_FW

#ifdef LLF_PKI_PKA_DEBUG
extern DxUint32_t tempRes[LLF_PKI_PKA_MAX_REGISTER_SIZE_WORDS + 1];
#endif


/****************** External functions propotypes ******************/

/************* Private functions prototypes *************************/
                                         
/************* Exported functions prototypes ************************/


#if !defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)
/*******************************************************************************************/
/**
 * @brief This function initializes the low level key database public structure.
 *        On the HW platform the Barrett tag is initialized  
 *         
 *
 * @param[in] PubKey_ptr - The pointer to public key structure.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_RSA_InitPubKeyDb( CRYSRSAPubKey_t *PubKey_ptr )
{
   
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error;
   
   
   /* the virtual address - locally defined just for code clearance */
   DxUint32_t  VirtualHwBaseAddr = 0;
  
  
  /* FUNCTION LOGIC */
   
   /* .................... initialize local variables ...................... */
   /* ---------------------------------------------------------------------- */
   
   /* initialize the error identifier to success */
   Error = CRYS_OK;
   
    
   /* ............... getting the hardware semaphore ..................... */
   /* -------------------------------------------------------------------- */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK ) 
   
     goto Return;

   /* ............... mapping the physical memory to the virtual one ...... */
   /* --------------------------------------------------------------------- */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );           /* The virtual address - out */
    
   if( Error != CRYS_OK )
   
     goto End_OnlySemRelease;
     

   /* .................... PKA initialization ............................. */
   /* --------------------------------------------------------------------- */   
  
   /* initialize the PKA engine on default mode */
   
   Error = LLF_PKI_PKA_DefaultInitPKA( PubKey_ptr->nSizeInBits, 
                                       PubKey_ptr->nSizeInBits/32+1, VirtualHwBaseAddr );

   if( Error != CRYS_OK )   
     goto End;

   
   /* calculate Barrett tag NP by initialization PKA for modular operations. 
      Default settings: N=0, NP=1, T0=30, T1=31. 
      Our settings for temps: rT0=2, rT1=3, rT2=4 */
   Error = LLF_PKI_CalcNpAndInitModOp(
                              PubKey_ptr->n,                   /*in*/ 
                              PubKey_ptr->nSizeInBits,         /*in*/ 
                              ((LLF_pki_pub_key_db_t*)(PubKey_ptr->crysRSAIntBuff))->NP,              /*out*/
                              PKA_CreateNP,                    /*in*/
                              2,   /*rT0*/                     /*in*/
                              3,   /*rT1*/                     /*in*/
                              4,   /*rT2*/                     /*in*/
                              VirtualHwBaseAddr );             /*in*/ 

  #ifndef LLF_PKI_PKA_DEBUG   
   /* clear used PKA registers (0-4 and 30,31) for security goals */
   LLF_PKI_PKA_ClearBlockOfRegs(  0/*FirstReg*/, 5/*Count*/, 1/*LenID*/, VirtualHwBaseAddr );
   LLF_PKI_PKA_ClearBlockOfRegs( 30/*FirstReg*/, 2/*Count*/, 1/*LenID*/, VirtualHwBaseAddr );
  #endif  
   
   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */
 End:  
  
   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );

   
   /* .... un mappping the physical memory to the virtual one and releasing the semaphore ...... */
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE ); /* 16 LS bit space - in */

   
 End_OnlySemRelease:
   
   /* release the hardware semaphore */
   DX_VOS_SemGive ( SemPkaId );

            
 Return:   
   
   return Error;


}/* END OF LLF_PKI_RSA_InitPubKeyDb */

#endif

#if !defined(_INTERNAL_CRYS_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)
/*******************************************************************************************/
/**
 * @brief This function initializes the low level key database private structure.
 *        On the HW platform the Barrett tag is initialized  
 *         
 *
 * @param[in] PrivKey_ptr - The pointer to private key structure.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_RSA_InitPrivKeyDb( CRYSRSAPrivKey_t *PrivKey_ptr )
{
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error;
   
   
   /* the virtual address - localy defined just for code clearnce */
   DxUint32_t VirtualHwBaseAddr = 0;
  
   /* FUNCTION LOGIC */
   
   /* .................... initialize local variables ...................... */
   /* ---------------------------------------------------------------------- */
   
   /* initialize the error identifier to success */
   Error = CRYS_OK;

 
   /* ............... getting the hardware semaphore ..................... */
   /* -------------------------------------------------------------------- */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK ) 
   
     goto Return;


   /* ............... mapping the physical memory to the virtual one ...... */
   /* --------------------------------------------------------------------- */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );            /* The virtual address - out */
    
   if( Error != CRYS_OK )
   
     goto End_OnlySemRelease;
     

   /* ---------------------------------------------------------------------- */
   /* ..........initialize PKA and calculate the Barrett tag ............... */
   /* ---------------------------------------------------------------------- */
   
   /*------------------------------*/
   /* calculate NP on NonCRT mode  */
   /*------------------------------*/
   if( PrivKey_ptr->OperationMode == CRYS_RSA_NoCrt )
   {  
	   /* initialize the PKA engine on default mode for size = mod N size */
	   Error = LLF_PKI_PKA_DefaultInitPKA( PrivKey_ptr->nSizeInBits, 
	                                       PrivKey_ptr->nSizeInBits/32+1, VirtualHwBaseAddr );

	   if( Error != CRYS_OK ) 
	   
	     goto End;

	  
	   /* calculate NP by initialization PKA for modular operations (modulus PrivKey_ptr->n ). 
	      Default settings: N=0, NP=1, T0=30, T1=31. 
	      Our settings for temps: rT0=2, rT1=3, rT2=4 */
		  Error = LLF_PKI_CalcNpAndInitModOp(
		                              PrivKey_ptr->n,      /*in*/ 
		                              PrivKey_ptr->nSizeInBits,        /*in*/ 
		                              ((LLF_pki_priv_key_db_t *)(PrivKey_ptr->crysRSAPrivKeyIntBuff))->NonCrt.NP,      /*out*/
		                              PKA_CreateNP,                    /*in*/
		                              2,   /*rT0*/                     /*in*/
		                              3,   /*rT1*/                     /*in*/
		                              4,   /*rT2*/                     /*in*/
		                              VirtualHwBaseAddr );             /*in*/ 


	  #ifndef LLF_PKI_PKA_DEBUG   
	    /* clear used PKA registers (0-4 and 30,31) for security goals */
	    LLF_PKI_PKA_ClearBlockOfRegs(  0/*FirstReg*/, 5/*Count*/, 1/*LenID*/, VirtualHwBaseAddr );
	    LLF_PKI_PKA_ClearBlockOfRegs( 30/*FirstReg*/, 2/*Count*/, 1/*LenID*/, VirtualHwBaseAddr );
	  #endif
		     
   }/* end NonCRT case */							         
   
   /*-----------------------------------------*/     
   /* on CRT mode calculate the Barrett tags  */
   /*    PP and PQ for P and Q factors        */
   /*-----------------------------------------*/ 
   else
   {
	   /* initialize the PKA engine on default mode */
	   Error = LLF_PKI_PKA_DefaultInitPKA( PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits/*nSizeInBits*/,
	                                       LLF_PKI_PKA_MAX_REGISTER_SIZE_WORDS, VirtualHwBaseAddr );

	   if( Error != CRYS_OK )
	     goto End;

	  
	   /* calculate Barrett tag PP for modulus P */
	   Error = LLF_PKI_CalcNpAndInitModOp(
                                      PrivKey_ptr->PriveKeyDb.Crt.P,                /*in*/ 
	                              PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits,                  /*in*/ 
	                              ((LLF_pki_priv_key_db_t *)(PrivKey_ptr->crysRSAPrivKeyIntBuff))->Crt.PP,   /*out*/
	                              PKA_CreateNP,                                             /*in*/
	                              2,   /*rT0*/                                              /*in*/
	                              3,   /*rT1*/                                              /*in*/
	                              4,   /*rT2*/                                              /*in*/
	                              VirtualHwBaseAddr );                                      /*in*/ 
	                              
       if( Error != CRYS_OK )
        goto End;
		     
       /* calculate Barrett tag QP for modulus Q */
 	   Error = LLF_PKI_CalcNpAndInitModOp(
                                      PrivKey_ptr->PriveKeyDb.Crt.Q,                                /*in*/ 
	                              PrivKey_ptr->PriveKeyDb.Crt.QSizeInBits,                                  /*in*/ 
	                              ((LLF_pki_priv_key_db_t *)(PrivKey_ptr->crysRSAPrivKeyIntBuff))->Crt.QP,                   /*out*/
	                              PKA_CreateNP,                                                             /*in*/
	                              2,   /*rT0*/                                                              /*in*/
	                              3,   /*rT1*/                                                              /*in*/
	                              4,   /*rT2*/                               /*in*/
	                              VirtualHwBaseAddr );                       /*in*/ 
          
       if( Error != CRYS_OK )
        goto End;
        
	  #ifndef LLF_PKI_PKA_DEBUG   
	    /* clear the used PKA registers (0-4 and 30,31) for security goals */
	    LLF_PKI_PKA_ClearBlockOfRegs(  0/*FirstReg*/, 5/*Count*/, 1/*LenID*/, VirtualHwBaseAddr );
	    LLF_PKI_PKA_ClearBlockOfRegs( 30/*FirstReg*/, 2/*Count*/, 1/*LenID*/, VirtualHwBaseAddr );
	  #endif

   }/* end of CRT case */
       
    
   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */
 End:  
  
   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );
   
    
   /* .... un mappping the physical memory to the virtual one and releasing the semaphore ...... */
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE );  /* 16 LS bit space - in */

   
 End_OnlySemRelease:
   
   /* release the hardware semaphore */
   DX_VOS_SemGive ( SemPkaId );
            
 Return:   
   
   return Error;
 
 }/* END OF LLF_PKI_RSA_InitPrivKeyDb */
#endif 
 
 
#if !defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)
/*******************************************************************************************/
/**
 * @brief This function executes the RSA primitive public key exponent :
 *       
 *    PubData_ptr->DataOut =  PubData_ptr->DataIn ** PubKey_ptr->e  mod  PubKey_ptr->n,
 *    where: ** - exponent symbol.    
 *   
 *    Note: PKA registers used: r0-r4,   r30,r31, size of registers - Nsize.
 *
 * @param[in] PubKey_ptr  - The public key database.
 * @param[in] PubData_ptr - The structure, containing input data and output buffer.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */ 
CRYSError_t LLF_PKI_RSA_ExecPubKeyExp( CRYSRSAPubKey_t     *PubKey_ptr,
                                       CRYS_RSAPrimeData_t *PubData_ptr )
{
  /* error identification */
   CRYSError_t Error;
   
   /* modulus and exponent sizes in bytes */ 
   DxUint32_t  nSizeInWords, eSizeInWords;
   
   /* the virtual address - localy defined just for code clearnce */
   DxUint32_t VirtualHwBaseAddr = 0;
   
  
   /* FUNCTION LOGIC */
   
   /* .................... initialize local variables ...................... */
   /* ---------------------------------------------------------------------- */
   
   /* initialize the error identifier to success */
   Error = CRYS_OK;

   /* modulus size in bytes */ 
   nSizeInWords = (PubKey_ptr->nSizeInBits + 31) / 32;
   eSizeInWords = (PubKey_ptr->eSizeInBits + 31) / 32;
 
   /* ............... getting the hardware semaphore ..................... */
   /* -------------------------------------------------------------------- */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK ) 
   
     goto Return;

   /* ............... mapping the physical memory to the virtual one ...... */
   /* --------------------------------------------------------------------- */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );            /* The virtual address - out */
    
   if( Error != CRYS_OK )
   
     goto End_OnlySemRelease;
     
   /* initialize the PKA engine on default mode */
   Error = LLF_PKI_PKA_DefaultInitPKA(PubKey_ptr->nSizeInBits,
				nSizeInWords+1 /*LLF_PKI_PKA_MAX_REGISTER_SIZE_WORDS*/, VirtualHwBaseAddr );
                               
   if( Error != CRYS_OK )     
         return Error;
                               

   /* calculate NP by initialization PKA for modular operations */
   Error = LLF_PKI_CalcNpAndInitModOp(
                              PubKey_ptr->n,       /*in*/ 
                              PubKey_ptr->nSizeInBits,         /*in*/ 
                              ((LLF_pki_pub_key_db_t *)(PubKey_ptr->crysRSAIntBuff))->NP,              /*out*/
                              PKA_SetNP,                    /*in*/
                              2,   /*rT0*/                     /*in*/
                              3,   /*rT1*/                     /*in*/
                              4,   /*rT2*/                     /*in*/
                              VirtualHwBaseAddr );             /*in*/ 
   if( Error != CRYS_OK )     
         return Error;
                              

   /* copy input data into PKI register: DataIn=>r2 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 2/*dstReg*/, 1/*LenID*/, 
                     PubData_ptr->DataIn, nSizeInWords, VirtualHwBaseAddr );
                     
#ifdef LLF_PKI_PKA_DEBUG
   LLF_PKI_PKA_CopyDataFromPkaReg(tempRes, nSizeInWords, 2/*srcReg*/, VirtualHwBaseAddr);
                                   
   if( DX_VOS_MemCmp((DxUint8_t*)tempRes, (DxUint8_t*)PubData_ptr->DataIn, nSizeInWords*4) != 0){
	PLAT_LOG_DEV_PRINT(( 0 , " Write in ExpInput is failed; \n" ));                                      
   }
#endif                                         

   /* copy exponent data PKI register: e=>r3 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 3/*dstReg*/, 1/*LenID*/, 
				PubKey_ptr->e, eSizeInWords, VirtualHwBaseAddr ); 

   /* .. calculate the exponent Res = OpA**OpB mod N;                  ... */
   /* -------------------------------------------------------------------- */      
   LLF_PKI_PKA_ModExp( 0/*LenID*/, 2/*OpA*/, 3/*OpB*/, 4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
                                   
      
   /* copy result into output: r4 =>DataOut */
   LLF_PKI_PKA_CopyDataFromPkaReg(PubData_ptr->DataOut, nSizeInWords, 
                                   4/*srcReg*/, VirtualHwBaseAddr );

  #ifndef LLF_PKI_PKA_DEBUG   
   /* clear used PKA registers (0-4 and 30,31) for security goals */
   LLF_PKI_PKA_ClearBlockOfRegs(  0/*FirstReg*/, 5/*Count*/, 1/*LenID*/, VirtualHwBaseAddr );
   LLF_PKI_PKA_ClearBlockOfRegs( 30/*FirstReg*/, 2/*Count*/, 1/*LenID*/, VirtualHwBaseAddr );
  #endif

   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );
   
                                
   /* .. un mappping the physical memory and releasing the semaphore ....  */
   /* -------------------------------------------------------------------- */
   
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,             /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE ); /* 16 LS bit space - in */

   
 End_OnlySemRelease:
   
   /* release the hardware semaphore */
   DX_VOS_SemGive ( SemPkaId );

            
 Return:   
   
   return Error;
 
   
}/* END OF LLF_PKI_RSA_ExecPubKeyExp */                                              
#endif


#if !defined(_INTERNAL_CRYS_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)
/*******************************************************************************************/
/**
 * @brief This function executes the RSA private key exponentiation  
 *         
 *    Algorithm [PKCS #1 v2.1]:
 *
 *     1. If NonCRT exponent, then  M  =  C^D  mod N. 
 *          
 *     2. If CRT exponent, then:  
 *        2.1. M1  =  C^dP mod P, 
 *        2.2. M2  =  C^dQ mod Q;         
 *        2.3  h = (M1-M2)*qInv mod P;
 *        2.4. M = M2 + Q * h. 
 *
 *     Where: M- message representative, C- ciphertext, N- modulus,
 *            P,Q,dP,dQ, qInv - CRT private key parameters;
 *            ^ - exponentiation symbol.
 *    
 *     Note: PKA registers used: NonCrt: r0-r4,   r30,r31, size of registers - Nsize;
 *                               Crt:    r0-r10,  r30,r31, size of registers - Nsize;
 *
 * @param[in] PubKey_ptr - the private key database.
 * @param[in/out] PrivData_ptr - the structure, containing DataIn and DataOut buffers.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_RSA_ExecPrivKeyExp( CRYSRSAPrivKey_t    *PrivKey_ptr ,
                                        CRYS_RSAPrimeData_t *PrivData_ptr )
{
   /* LOCAL DECLARATIONS */

  /* error identification */
   CRYSError_t Error;

   /* the virtual address - localy defined just for code clearnce */
   DxUint32_t VirtualHwBaseAddr = 0;
   
  
   /* FUNCTION LOGIC */
   
   /* .................... initialize local variables ...................... */
   /* ---------------------------------------------------------------------- */
   
   /* initialize the error identifier to success */
   Error = CRYS_OK;


   /* ............... getting the hardware semaphore ..................... */
   /* -------------------------------------------------------------------- */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK ) 
   
     goto Return;


   /* ............... mapping the physical memory to the virtual one ...... */
   /* --------------------------------------------------------------------- */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );           /* The virtual address - out */
    
   if( Error != CRYS_OK )
   
     goto End_OnlySemRelease;
 
 
 
   /*==============================================*/
   /*         1.  NonCRT  case                     */
   /*==============================================*/
   if( PrivKey_ptr->OperationMode == CRYS_RSA_NoCrt )
   {

       Error = LLF_PKI_ExecPrivKeyExpNonCrt( 
                              PrivKey_ptr, PrivData_ptr, VirtualHwBaseAddr );

       if( Error != CRYS_OK )     
         goto End;
                              
   }
   
   /*===============================================*/
   /*         2.  CRT  case                         */
   /*===============================================*/
   else   
   {
       Error = LLF_PKI_ExecPrivKeyExpCrt( 
                              PrivKey_ptr, PrivData_ptr, VirtualHwBaseAddr );
                              
   }

   /* .... un mappping the physical memory and releasing the semaphore ...... */
   /*-------------------------------------------------------------------------*/

End:
   
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE );  /* 16 LS bit space - in */

   
 End_OnlySemRelease:
   
   /* release the hardware semaphore */
   DX_VOS_SemGive ( SemPkaId );
 Return:   
   
   return Error;
   
}/* END OF LLF_PKI_RSA_ExecPrivKeyExp */ 
                                             
#endif





#ifndef _INTERNAL_CRYS_NO_RSA_KG_SUPPORT
/*******************************************************************************************/
/**
 * @brief This function generates a key pair
 *         
 *
 * @param[in] PubKey_ptr - the public key database.
 * @param[in] PrivKey_ptr - the private key database.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_RSA_GenerateKeyPair( 
                                         CRYSRSAPubKey_t  *PubKey_ptr,
                                         CRYSRSAPrivKey_t *PrivKey_ptr,
                                         CRYS_RSAKGData_t *KeyGenData_ptr )
{
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error;

   /* the P,Q primitive pointers */
   DxUint32_t *P_ptr,*Q_ptr;
   
   DxUint32_t Success, maxCountRegs, regSizeWords;

   /* the virtual address - localy defined just for code clearnce */
   DxUint32_t VirtualHwBaseAddr = 0;
   
   
   /* FUNCTION LOGIC */


   /* check that key size is not great, than allowed for Key Generation
      because pKA memory limit */
   if( PubKey_ptr->nSizeInBits > LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS )
	   return LLF_PKI_KG_UNSUPPORTED_KEY_SIZE;
   
   /* ............... initialize local variables ......................... */
   /* -------------------------------------------------------------------- */
   
   /* initialize the error identifier to the CRYS_OK ( success ) */
   Error = CRYS_OK;
   
   /* for avoid compailer warning */
   Success = 0;

   /* ............... getting the hardware semaphore ..................... */
   /* -------------------------------------------------------------------- */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if(Error) 
     goto Return;

   /* ............... mapping the physical memory to the virtual one ...... */
   /* --------------------------------------------------------------------- */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );           /* The virtual address - out */
    
   if(Error)
     goto End_OnlySemRelease;
 
   /* ...................   initialize the PKA .............................*/
   
   /* maximal count of allocated registers */
   regSizeWords = (LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS/32)/2 + 1;
 
   /* maximal count registers */
   maxCountRegs = LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES / ((LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS/8)/2 + 4);
   maxCountRegs = maxCountRegs > LLF_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS ? LLF_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS : maxCountRegs;
   
   Error = LLF_PKI_PKA_DefaultInitPKA( PubKey_ptr->nSizeInBits/2, 
                                       regSizeWords, 
                                       VirtualHwBaseAddr );
   if( Error != CRYS_OK )
       	   goto End;

   /* initialize the P,Q pointers to the buffers on the keygen data structure */
   P_ptr = KeyGenData_ptr->KGData.p;
   Q_ptr = KeyGenData_ptr->KGData.q;


   /* ............... calling the Non CRT or CRT KeyGen functions ......... */
   /* --------------------------------------------------------------------- */

   if( PrivKey_ptr->OperationMode == CRYS_RSA_NoCrt )
   {
     do
     {
	    Error = LLF_PKI_genKeyNonCrt(
				PubKey_ptr->e,  PubKey_ptr->eSizeInBits,    
				PubKey_ptr->nSizeInBits,
				0, /*testsCount should be set automatically*/ 
				&Success, 
				P_ptr,
				Q_ptr,
				PubKey_ptr->n,  
				PrivKey_ptr->PriveKeyDb.NonCrt.d, 
				((LLF_pki_key_gen_db_t *)(KeyGenData_ptr->KGData.crysRSAKGDataIntBuff))->temp, 
				VirtualHwBaseAddr );
					                
       if( Error != CRYS_OK )
	       goto End;
   
    #if !defined RSA_KG_NO_RND    
       if( !Success )
       {
   		   Error = CRYS_RSA_GenerateVectorInRangeX931( PubKey_ptr->nSizeInBits/(2*32), P_ptr);          
		   if( Error != CRYS_OK )			   
			   goto End;

		   Error = CRYS_RSA_GenerateVectorInRangeX931( PubKey_ptr->nSizeInBits/(2*32), Q_ptr);          
		   if( Error != CRYS_OK )			   
			   goto End;
       }
    #endif

     }while(!Success);
     
	/* set the key source as external - a PATCH since 'D' is not decrypted in SK2 */
	PrivKey_ptr->KeySource = CRYS_RSA_ExternalKey;
     
	 /* set the length of d in bits */
	 PrivKey_ptr->PriveKeyDb.NonCrt.dSizeInBits = \
			CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(
						PrivKey_ptr->PriveKeyDb.NonCrt.d, 
						(DxUint16_t)(PubKey_ptr->nSizeInBits+31)/32);

   }/* end of non CRT case */
   
   else  
   {        /* CRT case */ 
       do
       {
          Error = LLF_PKI_genKeyCrt(
			PubKey_ptr->e , PubKey_ptr->eSizeInBits,  
			PubKey_ptr->nSizeInBits,
			0, /*testsCount should be set automatically*/
			&Success,
			P_ptr, 
			Q_ptr,
			PubKey_ptr->n , 
			PrivKey_ptr->PriveKeyDb.Crt.dP,
			PrivKey_ptr->PriveKeyDb.Crt.dQ,
			PrivKey_ptr->PriveKeyDb.Crt.qInv,
			((LLF_pki_key_gen_db_t *)(KeyGenData_ptr->KGData.crysRSAKGDataIntBuff))->temp, 
			VirtualHwBaseAddr);

         if( Error != CRYS_OK )        
             goto End;
	
     #if !defined RSA_KG_NO_RND
         if( !Success )
         {
   		    Error = CRYS_RSA_GenerateVectorInRangeX931(PubKey_ptr->nSizeInBits/(2*32), P_ptr);          
		    if( Error != CRYS_OK )			   
			    goto End;

		    Error = CRYS_RSA_GenerateVectorInRangeX931(PubKey_ptr->nSizeInBits/(2*32), Q_ptr);          
		    if( Error != CRYS_OK )			   
			    goto End;
         }
     #endif     
       
       }while( !Success );
					                              
       /* Load P,Q vectors */
       DX_VOS_FastMemCpy(PrivKey_ptr->PriveKeyDb.Crt.P , P_ptr , PubKey_ptr->nSizeInBits / 16);  					                      
       DX_VOS_FastMemCpy(PrivKey_ptr->PriveKeyDb.Crt.Q , Q_ptr , PubKey_ptr->nSizeInBits / 16); 
	   
   }/* end of CRT case */	    					                      
           
   /* load 'n' to the private */
   DX_VOS_FastMemCpy(PrivKey_ptr->n , PubKey_ptr->n , PubKey_ptr->nSizeInBits / 8); 
   
End:
   /* clear secure sensitive data from allocated registers, including rT0,rT1 */
   LLF_PKI_PKA_ClearBlockOfRegs(0/*first*/, maxCountRegs - 2/*CountOfRegs*/, LenIDn+1, VirtualHwBaseAddr);
   LLF_PKI_PKA_ClearBlockOfRegs(30/*first*/, 2/*CountOfRegs*/, LenIDn+1, VirtualHwBaseAddr);
 

   /* end of PKA operations */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );	  


   /* .... un mappping the physical memory and releasing the semaphore ...... */
   /*-------------------------------------------------------------------------*/
   
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE );  /* 16 LS bit space - in */

   
 End_OnlySemRelease:
   
   /* release the hardware semaphore */
   DX_VOS_SemGive ( SemPkaId );
            
 Return:   

   return Error;
   
    
}/* END OF CC_LLF_PKI_RSA_GenerateKeyPair */

#endif




#ifndef _INTERNAL_CRYS_NO_RSA_CRT_SUPPORT
/*******************************************************************************************/
/**
 * @brief This function calculates the modulus N using P,Q ( CRT ) : N = P*Q
 *         
 *
 * @param[in] P_ptr - the first prime.
 * @param[in] P_SizeInBits - the first prime size in bits.
 * @param[in] Q_ptr - the second prime.
 * @param[in] Q_SizeInBits - the second prime size in bits.
 * @param[in] N_ptr        - the N vector. 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */ 
CRYSError_t LLF_PKI_RSA_CRTCalcN( DxUint32_t  *P_ptr, DxUint32_t PSizeInBits,
                                  DxUint32_t  *Q_ptr, DxUint32_t QSizeInBits,
                                  DxUint32_t  *N_ptr )
{

   /* LOCAL DECLARATIONS */   

   /* error identification */
   CRYSError_t Error = CRYS_OK;
   
   
   Error = LLF_PKI_RSA_CallRMul( P_ptr , max( PSizeInBits, QSizeInBits ), Q_ptr , N_ptr );
                         
   return Error;
   
}/* END OF LLF_PKI_RSA_CRTCalcN */

/*******************************************************************************************/
/**
   @brief The LLF_PKI_RSA_ConvertPrivKeyToCRT function converts the non CRT private
          key to CRT private Key and initializes appropriate fields of 
          output structure provided by pointer PrivCRTKey_ptr.
          All input and output data is in little endian representation.
          
          NOTE 1: The input (non CRT) and output (CRT) key structures are in common union buffer,
                  Therefore in the function all input (non CRT) data is used or saved befor 
                  inserting any computed data into CRT key structure.
                
          NOTE 2: The function is not supported now on this project.    
   
   @param[in] PrivNonCRTKey_ptr -  A pointer to the private non CRT key structure. 
                                   This structure is used as input to the CRYS_RSA_PRIM_Encrypt API.
   @param[in] n_ptr             -  A pointer to a buffer containing modulus.
   @param[in] nSizeInBits       -  Size of modulus in bits.
   @param[in] PrivCRTKey_ptr    -  A pointer to the private non CRT key structure.
   @param[in] TempBuffers_ptr   -  A pointer to the temporary buffers used by LLF functions called by
                                   this function. Size of buffer: 4 + 36 = 40 maximum modulus sizes.
*/

CRYSError_t LLF_PKI_RSA_ConvertPrivKeyToCRT( 
			CRYSRSAPrivNonCRTKey_t  *PrivNonCRTKey_ptr, /*in*/
			DxUint32_t  *n_ptr, /*in*/
			DxUint32_t nSizeInBits, /*in*/
			CRYSRSAPrivCRTKey_t  *PrivCRTKey_ptr,/*out*/
			CRYS_RSAConvertKeyToCrtBuffers_t *TempBuffers_ptr /*in*/ )
{
	/* VARIABLES DECLERATIONS */
	
	/* The Error return code identifier */
	CRYSError_t Error ; 
	
	
	/*use to deprecate lint warnings*/
	PrivNonCRTKey_ptr = PrivNonCRTKey_ptr;
	n_ptr = n_ptr;
	nSizeInBits = nSizeInBits;
	PrivCRTKey_ptr = PrivCRTKey_ptr;
	TempBuffers_ptr = TempBuffers_ptr;
	
	/***************  FUNCTION LOGIC **********************/
	
	/* Return because the function is not supported now on this project */
	Error = LLF_PKI_CONVERT_PRIV_KEY_TO_CRT_NOT_SUPPORTED;
	
	
	return Error;
	
 	
} /* end of CRYS_RSA_Build_ConvertPrivKeyToCRT() */
#endif

#ifndef _INTERNAL_CRYS_NO_RSA_KG_SUPPORT
/*******************************************************************************************/
/**
 * 
 * Function name: LLF_PKI_RSA_Call_Div
 *
 * Description: This function performs division of big numbers, passed by physical pointers, 
 *              using the PKA.
 *              .
 *     Computes modRes = A mod B. divRes_ptr = floor(A/B) 
 *     Lengths: A[ALen], B[BLen], modRes[BLen], divRes[ALen].
 *     Assumes:  c > 0.
 *
 *     PKA registers using: A=>r2, B=>r3, divRes=>r4, modRes=>r2 (r2 is rewritten by remainder).
 * 
 * Author: R.Levin 
 *
 * Last Revision: 1.00.00
 *
 * @param[in] A_ptr          - The pointer to numerator A vector.
 * @param[in] ASizeInWords   - Length of numerator A in words.
 * @param[in] B_ptr          - The pointer to divider B (modulus).  
 * @param[in] BSizeInWords   - The size of B vector in words.
 * @param[out] modRes_ptr    - The pointer to modulus result (reminder of division).
 * @param[out] divRes_ptr    - The pointer to result of division.
 * @param[in] tempBuff_ptr   - The pointer to temp buffer - not used, may be set NULL.
 *
 * @return  - no return value
 *
 * Update History:
 * Rev 1.00.00, Date 4 Feb. 2008,
 * 
 */
  
CRYSError_t  LLF_PKI_RSA_Call_Div( 
			DxUint32_t *A_ptr,        
			DxUint32_t ASizeInWords,
			DxUint32_t *B_ptr,        
			DxUint32_t BSizeInWords,
			DxUint32_t *modRes_ptr, 
			DxUint32_t *divRes_ptr,
			DxUint32_t *tempBuff_ptr)
{
  
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error;

   /* operation size */ 
   DxUint32_t  opSizeWords;
   
   /* the virtual address - locally defined just for code clearance */
   DxUint32_t VirtualHwBaseAddr = 0;
   
   /* PKA status */
   DxUint32_t status;
  

   /* FUNCTION LOGIC */
   
   /* ............... initialize local variables ......................... */
   /* -------------------------------------------------------------------- */
   
   /* initialize the error identifier to the CRYS_OK ( success ) */
   Error = CRYS_OK;
   
   /* for avoid compiler warning */
   tempBuff_ptr = tempBuff_ptr;
   
  
   /* ............... getting the hardware semaphore ..................... */
   /* -------------------------------------------------------------------- */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK ) 
   
     goto Return;

   /* ............... mapping the physical memory to the virtual one ...... */
   /* --------------------------------------------------------------------- */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );           /* The virtual address - out */
    
   if( Error != CRYS_OK )
   
     goto End_OnlySemRelease;
     
   /*------------------------------------------------------------------------*/
   /* initialize the PKA engine on default mode with size of registers       */
   /* according to operation size = max(Asize,Bsize)                         */
   /*------------------------------------------------------------------------*/
      
   opSizeWords = max( ASizeInWords, BSizeInWords );
      
   LLF_PKI_PKA_DefaultInitPKA( 32*opSizeWords, opSizeWords+1, VirtualHwBaseAddr );
   
   if( Error != CRYS_OK )
       	   goto End;
	
   /*------------------------------------------------------------------------*/
   /* copying all needed data into PKA memory before starting PKA operations */
   /* A=>r2, B=>r3,                                                          */
   /*------------------------------------------------------------------------*/
   
   /* copy numerator into PKA register: A=>r2 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 2/*dstReg*/, 1/*LenID*/, A_ptr/*src_ptr*/, 
                                   ASizeInWords , VirtualHwBaseAddr );

   /* copy divisor into PKA register: B=>r3 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 3/*dstReg*/, 1/*LenID*/, B_ptr/*src_ptr*/, 
                                   BSizeInWords , VirtualHwBaseAddr );
 
   /* check, that divisor is not null, else return error */
   LLF_PKI_PKA_AddIm( 0/*LenID*/, 3/*OpA*/, 0/*Imm OpB*/, 4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); 
   
   LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	   
   if( status == 1 )
   {
      Error = LLF_PKI_PKA_DIVIDER_IS_NULL_ERROR;
      
      goto End;   	
   }

   /*-------------------------------------------------------------------------*/
   /* division in PKA: quotient: r4 = r2 / r3; remainder: r2 = r2 % r3        */ 
   /*-------------------------------------------------------------------------*/
   
   LLF_PKI_PKA_Div( 0/*LenID*/, 2/*OpA*/, 3/*OpB*/, 4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
   
   LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
 
 
   /*-------------------------------------------------------------------------*/
   /*        output the results                                               */
   /*-------------------------------------------------------------------------*/
  
   if( divRes_ptr != DX_NULL ){
   	   LLF_PKI_PKA_CopyDataFromPkaReg(divRes_ptr,  ASizeInWords,  
                                       4/*srcReg*/, VirtualHwBaseAddr );
   }
  
   if( modRes_ptr != DX_NULL ){
   	   LLF_PKI_PKA_CopyDataFromPkaReg(modRes_ptr,  BSizeInWords,  
                                       2/*srcReg*/, VirtualHwBaseAddr );
   }

   /* ---------------------------------------------------------------------- */
   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */
 End:  
  
   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );
   
    
   /* .... un mapping the physical memory to the virtual one and releasing the semaphore ...... */
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE ); /* 16 LS bit space - in */

   
 End_OnlySemRelease:
   
   /* release the hardware semaphore */
   DX_VOS_SemGive ( SemPkaId );
            
 Return:   
   
   return Error;
                     
}

#endif
#ifndef _INTERNAL_CRYS_NO_RSA_CRT_SUPPORT
/*******************************************************************************************/
/**
 *
 * Function name: LLF_PKI_RSA_CallRMul
 *
 * Description: This function performs multiplication of big numbers, passed by physical  
 *              pointers, using the PKA. 
 *
 *        The RMul operation is : (A * B) 
 *
 *        The function performs the following algorithm:
 *
 *
 * @param[in] A_ptr       - The pointer of A words array (LS word is left most).
 * @param[in] B_ptr       - The pointer of B words array (LS word is left most).  
 * @param[in] ASizeInBits - The size of vectors in bits.
 * @param[out] Res_ptr    - The pointer to the result buffer.
 *
 * @return CRYSError_t - CRYS_OK 
 */
 CRYSError_t LLF_PKI_RSA_CallRMul( DxUint32_t *A_ptr , 
	                           DxUint32_t  ASizeInBits,
                                   DxUint32_t *B_ptr ,   
                                   DxUint32_t *Res_ptr )
 {
 	
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error;

   /* operation size */
    
   DxUint32_t  OpSizeInWords;
   
   /* the virtual address - locally defined just for code clearance */
   DxUint32_t VirtualHwBaseAddr = 0;
   
  

   /* FUNCTION LOGIC */
   
   /* ............... initialize local variables ......................... */
   /* -------------------------------------------------------------------- */
   
   /* initialize the error identifier to the CRYS_OK ( success ) */
   Error = CRYS_OK;

#ifdef LLF_PKI_PKA_DEBUG
   /* check the operands size */
   if( 2*ASizeInBits > LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS )
      return LLF_PKI_PKA_ILLEGAL_OPERAND_LEN_ERROR;
#endif
      
   /* set operation size in words */
   if( (2*ASizeInBits + 31)/32 < (LLF_PKI_PKA_MIN_OPERATION_SIZE_BITS+31)/32 )
      OpSizeInWords = (LLF_PKI_PKA_MIN_OPERATION_SIZE_BITS+31)/32;
  
   else
      OpSizeInWords = (2*ASizeInBits + 31)/32;     
  
   /* ............... getting the hardware semaphore ..................... */
   /* -------------------------------------------------------------------- */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK ) 
   
     goto Return;

   /* ............... mapping the physical memory to the virtual one ...... */
   /* --------------------------------------------------------------------- */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );           /* The virtual address - out */
    
   if( Error != CRYS_OK )
     goto End_OnlySemRelease;
     
 
   /*------------------------------------------------------------------------*/
   /* initialize the PKA engine on default mode with size of registers       */
   /* according to operation size = 2*ASizeInWords                         */
   /*------------------------------------------------------------------------*/
      
   Error = LLF_PKI_PKA_DefaultInitPKA( 32*OpSizeInWords, OpSizeInWords+1, VirtualHwBaseAddr );
	
   if( Error != CRYS_OK )
       	   goto End;
       	   
   /*------------------------------------------------------------------------*/
   /* copying all needed data into PKA memory before starting PKA operations */
   /* A=>r2, B=>r3,                                                          */
   /*------------------------------------------------------------------------*/
   
   /* copy A into PKA register: A=>r2 */
   LLF_PKI_PKA_CopyDataIntoPkaReg(2/*dstReg*/, 1/*LenID*/, A_ptr/*src_ptr*/, 
                                  (ASizeInBits+31)/32, VirtualHwBaseAddr);

   /* copy B into PKA register: B=>r2 */
   LLF_PKI_PKA_CopyDataIntoPkaReg(3/*dstReg*/, 1/*LenID*/, B_ptr/*src_ptr*/, 
                                  (ASizeInBits+31)/32, VirtualHwBaseAddr);
 
 
   /*-------------------------------------------------------------------------*/
   /* multiply in PKA:  r4 = r2 * r3;                                         */ 
   /*-------------------------------------------------------------------------*/
   
   LLF_PKI_PKA_LMul( 0/*LenID*/, 2/*OpA*/, 3/*OpB*/, 4/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
   
   LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
 
 
   /*-------------------------------------------------------------------------*/
   /*        output the results                                               */
   /*-------------------------------------------------------------------------*/
  
   LLF_PKI_PKA_CopyDataFromPkaReg(Res_ptr, OpSizeInWords, 4/*srcReg*/, VirtualHwBaseAddr );

   /* ---------------------------------------------------------------------- */
   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */

End:
   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   LLF_PKI_PKA_FinishPKA( VirtualHwBaseAddr );
   
    
   /* .... un mappping the physical memory to the virtual one and releasing the semaphore ...... */
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE ); /* 16 LS bit space - in */

   
 End_OnlySemRelease:
   
   /* release the hardware semaphore */
   DX_VOS_SemGive ( SemPkaId );
            
 Return:   
   
   return Error;
}
#endif // _INTERNAL_CRYS_NO_RSA_CRT_SUPPORT




#ifndef _INTERNAL_CRYS_NO_RSA_KG_SUPPORT
/*******************************************************************************************/
/**
 * @brief This function is used to test a primality according to ANSI X9.42 standard.
 *
 *        The function calls the LLF_PKI_primeTest function which performs said algorithm. 
 *
 * @param[in] P_ptr           - The pointer to the prime buff.
 * @param[in] sizeWords       - The prime size in words.
 * @param[in] rabinTestsCount - The count of Rabin-Miller tests repetition.
 * @param[in] isPrime         - The flag indicates primality:   
 *                                  if is not prime - DX_FALSE, otherwise - DX_TRUE.  
 * @param[in] TempBuff_ptr   - The temp buffer of minimum size:
 *                               - on HW platform  8*MaxModSizeWords,
 *                               - on SW platform  41*MaxModSizeWords.
 * @param[in] primeTestMode - primality testing mode (RSA or DH - defines how are performed some 
 *            operations on temp buffers.
 */
CRYSError_t LLF_PKI_RSA_primeTestCall(  
				DxUint32_t *P_ptr,                              
				DxInt32_t   sizeWords, 
				DxInt32_t   rabinTestsCount,
				DxInt8_t   *isPrime_ptr,
				DxUint32_t *TempBuff_ptr,
				CRYS_RSA_DH_PrimeTestMode_t primeTestMode ) 
{
   /* LOCAL DECLARATIONS */ 
 	  
   /* the Error identifier */
   CRYSError_t Error;
  
   
   DxUint32_t  VirtualHwBaseAddr = 0;
   	  

   /* FUNCTION  LOGIC */ 
      
 
	TempBuff_ptr = TempBuff_ptr;
	primeTestMode = primeTestMode;
   /* ............... getting the hardware semaphore ..................... */
   /* -------------------------------------------------------------------- */
   
   Error = DX_VOS_SemWait( SemPkaId , DX_INFINITE );
   
   if( Error != CRYS_OK ) 
   
     goto Return;


   /* ............... mapping the physical memory to the virtual one ...... */
   /* --------------------------------------------------------------------- */
   
   Error = DX_VOS_MemMap( PLAT_CryptoCellBaseAddr,        /* low address - in */
                          LLF_PKI_HW_CRYPTO_ADDR_SPACE,   /* 16 LS bit space - in */
                          &VirtualHwBaseAddr );            /* The virtual address - out */
    
   if( Error != CRYS_OK )
   
     goto End_OnlySemRelease;
     
        
   /*------------------------------------------------------------------------*/
   /*               test the primality                                       */
   /*------------------------------------------------------------------------*/   
      Error =  LLF_PKI_primeTest(  
                            P_ptr,                              
                            sizeWords, 
                            rabinTestsCount,
                            isPrime_ptr,
                            primeTestMode,
                            TempBuff_ptr,
						    VirtualHwBaseAddr );
						    
   /* ---------------------------------------------------------------------- */
   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */
  
    
   /* .... un mappping the physical memory to the virtual one and releasing the semaphore ...... */
   DX_VOS_MemUnMap( &VirtualHwBaseAddr,              /* virtual address - in */
                    LLF_PKI_HW_CRYPTO_ADDR_SPACE ); /* 16 LS bit space - in */

   
 End_OnlySemRelease:
   
   /* release the hardware semaphore */
   DX_VOS_SemGive ( SemPkaId );
            
 Return:   
   
   return Error;
 	
} /* End of LLF_PKI_RSA_primeTestCall */


#endif


