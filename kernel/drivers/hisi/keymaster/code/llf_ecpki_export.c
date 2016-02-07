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
#include "plat_print_dbg.h"
#include "crys_common.h"
#include "crys_common_math.h"
#include "crys_ecpki_types.h"
#include "crys_ecpki_kg.h"
#include "crys_ecpki_local.h"
#include "llf_ecpki_error.h"
#include "llf_ecpki_export.h"
#include "llf_ecpki_ec_arithmetic.h"
#include "llf_ecpki_modular_arithmetic.h"
#include "llf_pki.h"
#include "llf_ecpki_defs.h"

/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/* canceling the lint warning:
  Info 740: Unusual pointer cast 
  (incompatible indirect types) */
/*lint --e{740} */

/* canceling the lint warning:
   Info 826: Suspicious pointer-to-pointer conversion 
   (area too small) */
/*lint --e{826} */
/* r */

/************************ Enums *****************************************/

/************************ Typedefs **************************************/

/************************ Global Data ***********************************/

extern const CRYS_ECPKI_Domain_t LLF_EC_DOMAINS_ARRAY [];

/* importing the base address of the CRYPTO Cell . this value must be initialized in the 
   CRYS_Init function */
extern DxUint32_t PLAT_CryptoCellBaseAddr;


#ifndef DX_OEM_FW
extern DX_PAL_SEM SemPkaId;
#endif //DX_OEM_FW


/************* Private function prototype *******************************/

/************************ Public Functions ******************************/

/************* Exported functions prototypes ****************************/

/****************************************************************************************
                     _DX_LLF_ECPKI_InitPubPrivKeyDb function    
****************************************************************************************/
/**
 * @brief This function initializes the LLF public or private key database.
 *
 *        On the HW platform the Barrett NP and RP tags are initialized.
 *
 *        Note: 1. The function initializes only one of key database, which 
 *              pointer is not NULL.
 *              If both the public and the private keys pointers are NULL or
 *              both are not NULL, the function returns an error.
 *              2. For calling this function use the macros, given in LLF_ECPKI_Export.h.
 *         
 *
 * @param[in] PubKey_ptr - The pointer to public key structure.
 * @param[in] PrivKey_ptr - The pointer to private key structure.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t _DX_LLF_ECPKI_InitPubPrivKeyDb( CRYS_ECPKI_PublKey_t *PubKey_ptr, 
                                           CRYS_ECPKI_PrivKey_t *PrivKey_ptr )
{ 
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error;
   
   
   /* the virtual address - localy defined just for code clearnce */
   DxUint32_t  VirtualHwBaseAddr = 0;
   
   /* pointers to EC modulus and order */ 
   DxUint32_t  *mod_ptr, *ord_ptr;
 
    /* pointers to Barrett tags of EC modulus and order */ 
   DxUint32_t *NP_ptr, *RP_ptr;
  
   CRYS_ECPKI_DomainID_t	DomainID;
   
   DxUint32_t  modSizeInBits, ordSizeInBits;
   
  
   /* FUNCTION LOGIC */
   
   /* .................... initialize local variables ...................... */
   /* ---------------------------------------------------------------------- */
   
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
   
  
   if( PubKey_ptr != DX_NULL && PrivKey_ptr == DX_NULL )
   {
	DomainID = PubKey_ptr->DomainID;
   	NP_ptr = ((LLF_ecpki_pub_key_db_t *)(PubKey_ptr->crysPubKeyIntBuff))->NP;
   	RP_ptr = ((LLF_ecpki_pub_key_db_t *)(PubKey_ptr->crysPubKeyIntBuff))->RP;
   }
		
   else if( PrivKey_ptr != DX_NULL && PubKey_ptr == DX_NULL )
   {
	DomainID = PrivKey_ptr->DomainID;
   	NP_ptr = ((LLF_ecpki_priv_key_db_t *)(PrivKey_ptr->crysPrivKeyIntBuff))->NP;
   	RP_ptr = ((LLF_ecpki_priv_key_db_t *)(PrivKey_ptr->crysPrivKeyIntBuff))->RP;
   }

   else
   		return LLF_ECPKI_INIT_PKA_INVALID_PTR_ERROR;
		
   /* set pointers and size according */ 
   mod_ptr = (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].GF_Modulus;
   ord_ptr = (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].EC_GenerOrder;
   modSizeInBits = LLF_EC_DOMAINS_ARRAY[DomainID].EC_ModulusSizeInBits;
   ordSizeInBits = LLF_EC_DOMAINS_ARRAY[DomainID].EC_OrderSizeInBits;

   /* initialize the PKA engine on default mode */
   Error = LLF_PKI_PKA_DefaultInitPKA( modSizeInBits, CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+2, 
                                       VirtualHwBaseAddr );
   if( Error != CRYS_OK )   
     goto End;
   
   /* set EC order exact and extended sizes in bits into RegSizes entry 2,3 */
   LLF_PKI_PKA_SetRegSize( LLF_EC_DOMAINS_ARRAY[DomainID].EC_OrderSizeInBits, LenID_r, VirtualHwBaseAddr );
   LLF_PKI_PKA_SetRegSize( LLF_EC_DOMAINS_ARRAY[DomainID].EC_OrderSizeInBits+32, LenID_r+1, VirtualHwBaseAddr );
   
  /* set NP and RP into key LLF database from domain */	                             
  DX_VOS_FastMemCpy((DxUint8_t*)NP_ptr, (DxUint8_t*)(((LLF_ecpki_domains_db_t *)(LLF_EC_DOMAINS_ARRAY[DomainID].crysDomainIntBuff))->NP),
                     4*LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);
  DX_VOS_FastMemCpy((DxUint8_t*)RP_ptr, (DxUint8_t*)(((LLF_ecpki_domains_db_t *)(LLF_EC_DOMAINS_ARRAY[DomainID].crysDomainIntBuff))->RP),
                     4*LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);


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



/*****************************************************************************************
 *		       LLF_ECPKI_CheckPublKeyCall function	
 *****************************************************************************************/
/** 
  @brief  This function used as interface between CRYS and LLF levels for 
            calling of LLF_ECPKI_CheckPublKeyCall() function from CRYS functions.          
   
   @param[out] PubKey_ptr      - A pointer to the public key structure. 
                                 This structure is used as input to the ECPKI 
                                 cryptographic primitives.
   @param[in]  CheckMode       - Parameter defines what checking of validity is performed:
                                 preliminary check - 0, partly check - 1, full check - 2.
   @param[in]  tempBuff_ptr    - pointer to temp buffer /not used in PKA HW platform /. 

   @return  CRYS_OK,
            Error from called function
*/
 CRYSError_t  LLF_ECPKI_CheckPublKeyCall(
				    	CRYS_ECPKI_PublKey_t      *PublKey_ptr,        /*in*/ 
				    	EC_PublKeyCheckMode_t      CheckMode,          /*in*/ 
				        DxUint32_t                *tempBuff_ptr        /*in*/ )
{
   /* LOCAL DECLARATIONS */
   
   /* the error identifier */
   CRYSError_t Error;
   
   DxUint32_t modSizeInBits, modSizeInWords;
   
   /* ECC domain ID */
   CRYS_ECPKI_DomainID_t  DomainID;

   /* EC point for public key */
   LLF_ECPKI_PKA_Point_t  W;
   
   /* set list of allowed 28 temp PKA registers. Note: numbers 0xFF - for debug goals */
   DxInt8_t regTemps[30] = {0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
                            0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
                            0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0xFF,0xFF}; 
  
   /* the virtual address */
   DxUint32_t  VirtualHwBaseAddr = 0;
   
   
   
   /*  INITIALIZATIONS  */
   
   /* for preventing compiler warnings */
   *tempBuff_ptr = *tempBuff_ptr;
   
   /* EC domain parameters */
   DomainID = PublKey_ptr->DomainID;
  
   modSizeInBits = LLF_EC_DOMAINS_ARRAY[DomainID].EC_ModulusSizeInBits;
   modSizeInWords = (modSizeInBits + 31)/32;
   
   /* set W-point on PKA registers */
   W.rX =  regTemps[0];
   W.rY =  regTemps[1];
   W.rZ =  regTemps[2];
   
  
   /* FUNCTION LOGIC */

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
  
   
   /* .................... initialisation of PKA  ........................... */
   /* ----------------------------------------------------------------------- */   
  
   /* initialize the PKA engine on default mode. Note: Registers size is set 
      by 1 word greate, than max modulus size for operations with EC order*/
   Error = LLF_PKI_PKA_DefaultInitPKA( modSizeInBits, CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+1, VirtualHwBaseAddr );

   if( Error != CRYS_OK )    
     goto End;
 
   /* ............... copy the data into PKA registers  ..................... */
   /* ----------------------------------------------------------------------- */ 

   /* set EC order exact and extended sizes in bits into RegSizes entry 2,3 */
   LLF_PKI_PKA_SetRegSize( LLF_EC_DOMAINS_ARRAY[DomainID].EC_OrderSizeInBits, LenID_r, VirtualHwBaseAddr );
   LLF_PKI_PKA_SetRegSize( LLF_EC_DOMAINS_ARRAY[DomainID].EC_OrderSizeInBits+32, LenID_r+1, VirtualHwBaseAddr );
   
   
   /* set modulus N and Barrett tag NP into PKA registers 0, 1 */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 0/*dstReg*/, LenID_n+1, 
                          (DxUint32_t*)LLF_EC_DOMAINS_ARRAY[DomainID].GF_Modulus/*src_ptr*/, 
                          modSizeInWords, VirtualHwBaseAddr );

   LLF_PKI_PKA_CopyDataIntoPkaReg( 1/*dstReg*/, LenID_n+1,
                          ((LLF_ecpki_domains_db_t *)(LLF_EC_DOMAINS_ARRAY[DomainID].crysDomainIntBuff))->NP/*src_ptr*/,
                          LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr );
                          
   /* set public key coordinates and ID into PKA registers  */ 
   LLF_PKI_PKA_CopyDataIntoPkaReg( W.rX/*dstReg*/, LenID_n+1,PublKey_ptr->PublKeyX/*src_ptr*/, 
                                   modSizeInWords, VirtualHwBaseAddr );                         
   LLF_PKI_PKA_CopyDataIntoPkaReg( W.rY/*dstReg*/, LenID_n+1, PublKey_ptr->PublKeyY/*src_ptr*/, 
                                   modSizeInWords, VirtualHwBaseAddr );
   /* set Z = 1 */                                
   LLF_PKI_PKA_Set0( LenID_n, W.rZ/*OpA*/, W.rZ/*Res*/, 0/*Tag*/, VirtualHwBaseAddr ); 
   
   W.PointID = EC_PointJacobian;
                                  
   /* ----------------------------------------------------------------------- */
   /*......call LLF functions for checking the public key ....................*/
   /* ----------------------------------------------------------------------- */

   if( CheckMode == ECpublKeyPartlyCheck )
   
        /* partly checking of the public key validity (size) */
        Error = LLF_ECPKI_CheckPublKeySize( DomainID, PublKey_ptr->PublKeyX, 
                                            2*((modSizeInBits+7)/8) );
  
   else if( CheckMode < EC_PublKeyCheckModeLast )
   
  		/* full checking of the public key validity */
		Error = LLF_ECPKI_GFpCheckPublKey( DomainID, &W, CheckMode, 
		                                   regTemps+3, 28-3/*countTemps*/, VirtualHwBaseAddr );
 

   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */
 End:  
   
   /* clear all PKA registers (0-31) for security goals */
  #ifndef LLF_PKI_PKA_DEBUG   
   LLF_PKI_PKA_ClearBlockOfRegs(  0/*FirstReg*/, 32/*Count*/, LenID_r+1/*LenID*/, VirtualHwBaseAddr );
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

}/* END OF LLF_ECPKI_CheckPublKeyCall */   






/*****************************************************************************************
 *		       LLF_ECPKI_GenKeyPairCall function
 *                        											  
 *****************************************************************************************/
/**
   @brief   This function used as interface between CRYS and LLF levels for 
            calling of LLF_ECPKI_GenKeyPair function from CRYS functions.	  

   @param[in]  DomainID        - The enumerator variable defines current EC domain
   @param[out] UserPrivKey_ptr - A pointer to the private key structure. 
                                 This structure is used as input to the ECPKI 
                                 cryptographic primitives.
   @param[out] UserPubKey_ptr  - A pointer to the public key structure. 
                                 This structure is used as input to the ECPKI 
                                 cryptographic primitives.
   @param[in]  TempData_ptr    - temporary buffers of defined size.

   @return CRYSError_t - CRYS_OK,

*/
CRYSError_t LLF_ECPKI_GenKeyPairCall(
				    	CRYS_ECPKI_DomainID_t  	    DomainID,	        /*in*/   
				    	CRYS_ECPKI_UserPrivKey_t   *UserPrivKey_ptr,    /*out*/
				 	CRYS_ECPKI_UserPublKey_t   *UserPublKey_ptr,    /*out*/   
				        CRYS_ECPKI_KG_TempData_t   *TempData_ptr        /*in*/ )
{
   /* LOCAL DECLARATIONS */
   
   /* the error identifier */
   CRYSError_t Error;   

   /* the pointers to the key structures and and domain parameters */
   CRYS_ECPKI_PublKey_t  *publKey_ptr;  
   CRYS_ECPKI_PrivKey_t  *privKey_ptr; 
   DxUint32_t             modSizeInBits;    /* size of EC modulus in bits */
   DxUint32_t             modSizeInWords;   /* size of EC modulus in words */
   const CRYS_ECPKI_Domain_t   *Domain_ptr;
   
   /* EC point for public key */
   LLF_ECPKI_PKA_Point_t  W;
   
   /* virtual pointers to PKA registers: private key rS */
   DxInt8_t   rS;

   /* list of PKA registers. 28 allowed for using : Note: numbers 0xFF - for debug goals */
   DxInt8_t regTemps[30] = {0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
                            0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
                            0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0xFF,0xFF }; 

   DxUint32_t  VirtualHwBaseAddr = 0;
 
   
   /* FUNCTION LOGIC */
   
   /* for preventing compiler warnings */
   *TempData_ptr = *TempData_ptr;
   
   /* ................ initializtions  ............ */
   /* ------------------------------------------------------------------- */
   
    /* initialize the error */
   Error = CRYS_OK;   

   /* set the PKA virtual pointers */
   rS   = 2;/*regTemps[0];*/
   W.rX = 3;/*regTemps[1];*/
   W.rY = 4;/*regTemps[2];*/
   W.rZ = 5;/*regTemps[3];*/
   
      
   /* set the domain pointer */
   Domain_ptr = &LLF_EC_DOMAINS_ARRAY[DomainID];
    
   /* modulus sizes in words */
   modSizeInBits  = Domain_ptr->EC_ModulusSizeInBits;
   modSizeInWords = Domain_ptr->EC_ModulusSizeInWords;
   
   /* set key pointers */
   publKey_ptr = (CRYS_ECPKI_PublKey_t*)(void*)&UserPublKey_ptr->PublKeyDbBuff;
   privKey_ptr = (CRYS_ECPKI_PrivKey_t*)(void*)&UserPrivKey_ptr->PrivKeyDbBuff;
   
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
     
  
   
   /* .................... initialisation of PKA  ........................... */
   /* ----------------------------------------------------------------------- */   
  
   /* initialize the PKA engine on default mode. Note: Registers size is set 
      by 2 word greate, than max modulus size for operations with EC order*/
   Error = LLF_PKI_PKA_DefaultInitPKA( modSizeInBits, CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+2, VirtualHwBaseAddr );

   if( Error != CRYS_OK )    
     goto End;
 
   /* set EC order exact and extended sizes in bits into RegSizes entry 2,3 */
   LLF_PKI_PKA_SetRegSize( Domain_ptr->EC_OrderSizeInBits, LenID_r, VirtualHwBaseAddr );
   LLF_PKI_PKA_SetRegSize( Domain_ptr->EC_OrderSizeInBits+32, LenID_r+1, VirtualHwBaseAddr );
   
   /* ............init modular operations and LLF keys database .... .......... */
   /* ------------------------------------------------------------------------- */

   /* copy modulus N and its Barrett tag into r0,r1 registers */
   LLF_PKI_PKA_CopyDataIntoPkaReg( 0/*dstReg*/, LenID_n+1, 
				   (DxUint32_t*)Domain_ptr->GF_Modulus/*src_ptr*/, 
				   modSizeInWords, VirtualHwBaseAddr ); 
      
   LLF_PKI_PKA_CopyDataIntoPkaReg( 1/*dstReg*/, LenID_n+1, 
				   (DxUint32_t*)((LLF_ecpki_domains_db_t *)(Domain_ptr->crysDomainIntBuff))->NP/*src_ptr*/, 
				   LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr );    

   /* set Barrett tags into keys data bases  */         
   DX_VOS_FastMemCpy( (DxUint8_t*)(((LLF_ecpki_priv_key_db_t *)(privKey_ptr->crysPrivKeyIntBuff))->NP), (DxUint8_t*)(((LLF_ecpki_domains_db_t *)(LLF_EC_DOMAINS_ARRAY[DomainID].crysDomainIntBuff))->NP),
                      4*LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS );
   DX_VOS_FastMemCpy( (DxUint8_t*)(((LLF_ecpki_priv_key_db_t *)(privKey_ptr->crysPrivKeyIntBuff))->RP), (DxUint8_t*)(((LLF_ecpki_domains_db_t *)(LLF_EC_DOMAINS_ARRAY[DomainID].crysDomainIntBuff))->RP),
                      4*LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS );
   DX_VOS_FastMemCpy( (DxUint8_t*)(((LLF_ecpki_pub_key_db_t *)publKey_ptr->crysPubKeyIntBuff)->NP), (DxUint8_t*)(((LLF_ecpki_domains_db_t *)(LLF_EC_DOMAINS_ARRAY[DomainID].crysDomainIntBuff))->NP),
                      4*LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS );
   DX_VOS_FastMemCpy( (DxUint8_t*)(((LLF_ecpki_pub_key_db_t *)publKey_ptr->crysPubKeyIntBuff)->RP), (DxUint8_t*)(((LLF_ecpki_domains_db_t *)(LLF_EC_DOMAINS_ARRAY[DomainID].crysDomainIntBuff))->RP),
                      4*LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS );



   /* ............ call the PKA LLF_ECPKI_GenKeyPair function ...... .......... */
   /* ------------------------------------------------------------------------- */
   
   if( LLF_EC_DOMAINS_ARRAY[DomainID].FieldID == GFp ){
	   Error = LLF_ECPKI_GenKeyPair(DomainID, rS/*privKey*/, &W/*pubKey*/, regTemps+4, 28-4/*countTemps*/, 
					TempData_ptr ,1/*isKeyInternal*/, VirtualHwBaseAddr );
   } else{
	   Error = LLF_ECPKI_EC_FIELD_NOT_SUPPORTED;
   }
   
   if( Error != CRYS_OK )
       goto End;
   
   
   /* ....... output the generated key data in little endianness ...... ..   */
   /* ---------------------------------------------------------------------- */
   
   LLF_PKI_PKA_CopyDataFromPkaReg(publKey_ptr->PublKeyX/*dst*/, modSizeInWords,  
                                  W.rX/*src reg*/, VirtualHwBaseAddr);
 
   LLF_PKI_PKA_CopyDataFromPkaReg(publKey_ptr->PublKeyY/*dst*/, modSizeInWords, 
                                  W.rY/*src reg*/, VirtualHwBaseAddr);
                                    
   LLF_PKI_PKA_CopyDataFromPkaReg(privKey_ptr->PrivKey/*dst*/, Domain_ptr->EC_OrderSizeInWords, 
                                  rS/*src reg*/, VirtualHwBaseAddr);
                                    
   publKey_ptr->DomainID = DomainID;                              
   privKey_ptr->DomainID = DomainID;

   /* .............. end of the function ................................... */
   /* ---------------------------------------------------------------------- */
 End:  
  
   /* clear all PKA registers (0-31) for security goals */
 #ifndef LLF_PKI_PKA_DEBUG   
   LLF_PKI_PKA_ClearBlockOfRegs(0/*FirstReg*/, 32/*Count*/, LenID_r+1/*LenID*/, VirtualHwBaseAddr);
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

}/* END OF CRYS_ECPKI_GenKeyPairCall */  



/*****************************************************************************************
 *		       LLF_ECPKI_DomainInfoAndCheckKey function	
 *                        											  
 *****************************************************************************************/
/**
@brief   The function transfers EC Domain data to CRYS level  and performs preliminary  
         checking of the user private and public keys.

   @param[in] DomainID -            EC Domain identifier.
   @param[in] DomainInfo_ptr -      A pointer to a domain information data .    			   				
   @param[in] PrivKey_ptr         - A pointer to private key (little endian).                         
   @param[in] PrivKeySizeInBytes  - Size of private key .
   @param[in] PublKey_ptr         - A pointer to public key (must containing X,Y coordinats
                                    of size = modulus size each one (little endian words order).                         
   @param[in] PublKeySizeInBytes  - Size of public key (without point control byte), 
                                    must be equalled 2*ModSizeBytes.
   @param[in] CheckKey            - Parameter, defines whether private ahd public key
                                    must be checked: 1 - check private key, 2 - check 
					public key, 3 - check both keys, 0 - not check keys.
   @return CRYSError_t - CRYS_OK,
			LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_SIZE_ERROR 
			LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_DATA_ERROR
			LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_SIZE_ERROR
			LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR
**/
CRYSError_t LLF_ECPKI_DomainInfoAndCheckKey(
				CRYS_ECPKI_DomainID_t      DomainID,	       /*in*/							  
				CRYS_ECPKI_DomainInfo_t   *DomainInfo_ptr,     /*in*/
				DxUint32_t		  *PrivKey_ptr,        /*in*/
				DxUint32_t                 PrivKeySizeInBytes, /*in*/
				DxUint32_t	          *PublKey_ptr,        /*in*/
				DxUint32_t                 PublKeySizeInBytes, /*in*/
				DxInt8_t                   CheckKey            /*in*/ )
{  
    /* FUNCTION DECLARATIONS */
  
   /* the EC generator order and modulus size in bits */
   DxUint16_t OrderSizeInBits, OrderSizeInBytes; 
   DxUint16_t ModSizeInBits, ModSizeInBytes, ModSizeInWords;    
   
   /* the Error return code identifier */
   CRYSError_t Error =   CRYS_OK; 
   CRYS_ECPKI_Domain_t  *Domain_ptr;


   /* FUNCTION LOGIC */
   
 
   /************ initialisations  ********/
   
   /* Check DomainID */
   if(DomainID >= CRYS_ECPKI_DomainID_OffMode)
        return LLF_ECPKI_DOMAIN_INFO_INVALID_DOMAIN_ID_ERROR;
        
   /* pointer to current EC domain */
   Domain_ptr = (CRYS_ECPKI_Domain_t *)&LLF_EC_DOMAINS_ARRAY[DomainID];
   
   OrderSizeInBits = Domain_ptr->EC_OrderSizeInBits;
   OrderSizeInBytes = (OrderSizeInBits + 7) / 8;
   ModSizeInBits = Domain_ptr->EC_ModulusSizeInBits;
   ModSizeInBytes = (ModSizeInBits + 7) / 8;
   ModSizeInWords = Domain_ptr->EC_ModulusSizeInWords;
 
   if(DomainInfo_ptr != DX_NULL)  
   {
	/* set domain data into DomainInfo structure    */   
	DomainInfo_ptr->EC_OrderSizeInBits = OrderSizeInBits;   
	DomainInfo_ptr->EC_ModulusSizeInBits = ModSizeInBits; 
	DomainInfo_ptr->FieldID = Domain_ptr->FieldID;
	DomainInfo_ptr->DomainID = DomainID;
	DomainInfo_ptr->ModulusMSByte = (DxUint8_t)(Domain_ptr->GF_Modulus[ModSizeInWords - 1] & 0xFF);
   }

   /* Check private key:  if (PrivKey >= EC_GenerOrder || PrivKey == 0) then set Error */  
   if((CheckKey & 1) == 1)  /* Check Private Key */
   {
		if( PrivKeySizeInBytes == 0 || PrivKeySizeInBytes > OrderSizeInBytes )
			   return LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_SIZE_ERROR;

		if(PrivKeySizeInBytes == OrderSizeInBytes)
		{
			if(CRYS_COMMON_CmpLsWordsUnsignedCounters(
				PrivKey_ptr, (DxUint16_t)(PrivKeySizeInBytes+3)/4, 
				Domain_ptr->EC_GenerOrder, (DxUint16_t)(PrivKeySizeInBytes+3)/4) != 
				CRYS_COMMON_CmpCounter2GraterThenCounter1){
					return LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_DATA_ERROR;
			}
		}

		if(CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(
			PrivKey_ptr, (DxUint16_t)(PrivKeySizeInBytes+3)/4) == 0){
				return LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_DATA_ERROR;		
		}
   }
   
   /* Check public key:  if (PublKey >= EC_Modulus || PublKey == 0) then set Error */  
   if((CheckKey & 2) == 2) /* Check Public Key */
   {
		/* if size of publ key > 2*ModSizeInBytes  then Error */ 
		if(PublKeySizeInBytes != 2*(DxUint32_t)ModSizeInBytes)
			return LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_SIZE_ERROR;                                
     
		/* if PublKeyX > Modulus  then Error */   
		if(CRYS_COMMON_CmpLsWordsUnsignedCounters(
			PublKey_ptr, ModSizeInWords, 
			(DxUint32_t*)Domain_ptr->GF_Modulus, ModSizeInWords) != 
			CRYS_COMMON_CmpCounter2GraterThenCounter1){
				return LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR;
		}
     
		/* if PublKeyY > Modulus  then Error */ 
		if(CRYS_COMMON_CmpLsWordsUnsignedCounters( 
			((CRYS_ECPKI_PointAffine_t*)(void*)PublKey_ptr)->CoordY, ModSizeInWords, 		  
			(DxUint32_t*)Domain_ptr->GF_Modulus, ModSizeInWords ) != 
			CRYS_COMMON_CmpCounter2GraterThenCounter1 ){
				return LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR;
		}

            
		/* if PublKeyY = 0  then Error */ 
		if( CRYS_COMMON_GetWordsCounterEffectiveSizeInBits( 
			((CRYS_ECPKI_PointAffine_t*)(void*)PublKey_ptr)->CoordY, 
			ModSizeInWords) == 0 ){
				return LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR;
		}
    }

   return Error;

} /* End of LLF_ECPKI_DomainInfoAndCheckKey function */



/*****************************************************************************************
 		         LLF_ECPKI_SVDP_DH function 													  
 *****************************************************************************************/
/**
 @brief:    @brief: 	This function performs calculation of SharedSecretValue according 
                        ECC Diffie-Hellman algorithm (IEEE Std. 1363-2000):
                        1. Computes an elliptic curve point P(x,y) = s*W', 
                           where s - user private key, W' - partner public key.
                        2. Reverse copies and outputs the coordinate x as SharedSecretValue.                        
 
 @param[in]  PartnerPublKey_ptr    - A pointer to a Partner Public Key W 
 @param[in]  UserPrivKey_ptr	   - A pointer to a User Private Key  
 @param[Out] SharedSecretValue_ptr - A pointer to a buffer for output of SharedSecretValue
                                     (in big endian)
 @param[in]  TempBuff_ptr          - A pointer to a temporary buffers
  		
 @return CRYSError_t - CRYS_OK,
 					   Error from called functions
*/
CRYSError_t LLF_ECPKI_SVDP_DH( 
			CRYS_ECPKI_PublKey_t     *PartnerPublKey_ptr,    /*in*/
			DxUint32_t               *UserPrivKey_ptr,       /*in*/
			DxUint32_t    	         *SharedSecretValue_ptr, /*out*/
			CRYS_ECDH_TempData_t     *TempBuff_ptr           /*in*/ )
{
 
   /* LOCAL DECLARATIONS */

   /* errors identifier */
   CRYSError_t Error;
    
   /* EC points for public key and secret shared value */
   LLF_ECPKI_PKA_Point_t  W, sP;
   
   /* virtual pointers to PKA registers: private key rS */
   DxInt8_t   rS;

   /* list of PKA registers. 28 allowed for using : Note: numbers 0xFF - for debug goals */
   DxInt8_t regTemps[30] = {0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
                            0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
                            0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0xFF,0xFF }; 

   /* domain parameters: modulus and order R sizes */
   CRYS_ECPKI_DomainID_t     DomainID; 
   const CRYS_ECPKI_Domain_t  *Domain_ptr;
   DxUint32_t  modSizeInBits, modSizeInWords, ordSizeInWords;
   
   DxUint32_t  VirtualHwBaseAddr = 0;
 
   DxUint32_t isOnInfinity;
   
   
   /* FUNCTION LOGIC */
   
   /* for preventing compiler warnings */
   *TempBuff_ptr = *TempBuff_ptr;
   
   /* ................ initializtions  ............ */
   /* ------------------------------------------------------------------- */
   
   /* initialize the error */
   Error = CRYS_OK;   

   /* set the PKA virtual pointers */
   W.rX = regTemps[0];
   W.rY = regTemps[1];
   W.rZ = regTemps[2];
   sP.rX = regTemps[3];
   sP.rY = regTemps[4];
   sP.rZ = regTemps[5];
   rS   = regTemps[6];
      
   /* modulus sizes in words */
   DomainID   = PartnerPublKey_ptr->DomainID;
   Domain_ptr = &LLF_EC_DOMAINS_ARRAY[DomainID];
   modSizeInBits  = Domain_ptr->EC_ModulusSizeInBits;
   modSizeInWords = Domain_ptr->EC_ModulusSizeInWords;
   ordSizeInWords = Domain_ptr->EC_OrderSizeInWords;
   
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
     
   
   /* .................... initialisation of PKA  ........................... */
   /* ----------------------------------------------------------------------- */   
  
   /* initialize the PKA engine on default mode. Note: Registers size is set 
      by 1 word greate, than max modulus size for operations with EC order*/
   Error = LLF_PKI_PKA_DefaultInitPKA( modSizeInBits, CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+1, VirtualHwBaseAddr );

   if( Error != CRYS_OK )    
     goto End;
 
   /* set EC order exact and extended sizes in bits into RegSizes entry 2,3 */
   LLF_PKI_PKA_SetRegSize( Domain_ptr->EC_OrderSizeInBits, LenID_r, VirtualHwBaseAddr );
   LLF_PKI_PKA_SetRegSize( Domain_ptr->EC_OrderSizeInBits+32, LenID_r+1, VirtualHwBaseAddr );

    /*............ setting the input data into PKA registers ..................*/
    /* ----------------------------------------------------------------------- */
    
    /* modulus and Barrett tag*/
    LLF_PKI_PKA_CopyDataIntoPkaReg( 0/*modulus*/, LenID_n+1, (DxUint32_t*)Domain_ptr->GF_Modulus/*src_ptr*/, 
                                    ordSizeInWords, VirtualHwBaseAddr );

    LLF_PKI_PKA_CopyDataIntoPkaReg( 1/*BarTag*/, LenID_n+1, ((LLF_ecpki_pub_key_db_t *)PartnerPublKey_ptr->crysPubKeyIntBuff)->NP/*src_ptr*/, 
                                    LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr );

    /* private key */
    LLF_PKI_PKA_CopyDataIntoPkaReg( rS/*dstReg*/, LenID_n+1, UserPrivKey_ptr/*src_ptr*/, 
                                    ordSizeInWords, VirtualHwBaseAddr );
    /* public key */
    LLF_ECPKI_SetPoint(PartnerPublKey_ptr->PublKeyX, PartnerPublKey_ptr->PublKeyY, DX_NULL/*Z*/,
                       modSizeInWords, LenID_n, &W/*Res*/, EC_PointJacobian, VirtualHwBaseAddr ); 
                                       
    /* ....... calculation of the shared secret point sP .................. */
    Error = LLF_ECPKI_GFpScalarMultProj(rS, &W, &sP/*Res*/, DomainID,
				        regTemps+7, 28-7/*countTemps*/, VirtualHwBaseAddr );
    if( Error != CRYS_OK )   
		goto End;
		                                  
    /* if sP = O then output error and exit */
  
    isOnInfinity = LLF_ECPKI_IsProjPointOnInfinity( LenID_n, &sP, VirtualHwBaseAddr );
  
    if( isOnInfinity == 1 )   	
 	   Error = LLF_ECPKI_SVDP_DH_SHARED_VALUE_IS_ON_INFINITY_ERROR;   
    else
    {
	Error = LLF_ECPKI_GFpJacobianToAffine( &sP/*PJac*/, &W/*PAff*/, DomainID, 
				 regTemps+7, 28-7/*countTemps*/, VirtualHwBaseAddr );
	if( Error != CRYS_OK )       
		goto End;
	
	/* copy X-coordinate of secret sP into output */
	LLF_PKI_PKA_CopyDataFromPkaReg(SharedSecretValue_ptr/*dst*/, modSizeInWords, W.rX/*src reg*/, 
				   VirtualHwBaseAddr);
	
			  
	if(Error != CRYS_OK)
		return Error;
    } 


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

}/* END OF LLF_ECPKI_SVDP_DH */   



/*****************************************************************************************
 *		       LLF_ECDSA_SignCalcCall function	/ named ECSP_DSA in IEEE-1363 std.
 *                        with internal or external ephemeral key												  
 *****************************************************************************************/
/**
   @brief: 	This function used as interface between CRYS and LLF levels for 
            calling the LLF_ECDSA_SignCalc() function from CRYS functions.
			
	
   @param[in]  SignerPrivateKey_ptr - A pointer to the User Private Key   
   @param[in]  MessageRepresent_ptr - A pointer to message representative  
   @param[in]  MessageRepresentSizeWord - Size of message representative in words
   @param[in]  IsEphemerKeyInternal, - indicates source for random ephemeral key:
                    IsEphemerKeyInternal, = 1 - source internal,
                    IsEphemerKeyInternal, = 0 - source external from RandEphemeralKey_ptr.
   @param[in]  EphemeralKeyData_ptr - Pointer to source for external ephemeral key data which
               contains: ephemeral priv. key u of length ModSizeInBytes + 4,
                         ephemeral public key Vx and Vy - each of length ModSizeInBytes .
   @param[out] SignatureC_ptr - Pointer to output of C value of signature. 
   @param[out] SignatureD_ptr - Pointer to output of D value of signature. 
   @param[in]  TempBuff_ptr -   Pointer to temporary buffer.                          
   
   @return CRYSError_t - CRYS_OK,    						 			 
						 Error from called functions 
*/
CRYSError_t LLF_ECDSA_SignCalcCall(  
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
   
   /* the error identifier */
   CRYSError_t Error;  

   /* pointers to temporary buffers for:     */
   
 
   /* FUNCTION LOGIC */   
   
   /*********** initializtions  ******************************/
   Error = CRYS_OK;

 
   /* LLF function call */
   Error = LLF_ECDSA_SignCalc(SignerPrivateKey_ptr, MessageRepresent_ptr, 
			MessageRepresentSizeWords, IsEphemerKeyInternal,
			EphemerKeyData_ptr, SignatureC_ptr, SignatureD_ptr, 
			TempBuff_ptr );    								 
								    
   return Error;

}/* END OF LLF_ECDSA_SignCalcCall */                                       



/*****************************************************************************************
 *		       LLF_ECDSA_VerifyCalcCall function	/ named ECVP_DSA in IEEE-1363 std.
 *****************************************************************************************/
/**
   @brief: 	This function used as interface between CRYS and LLF levels for 
            calling of LLF_ECDSA_SignVerify() function from CRYS functions.
	
			If signature is not valid the function returns CRYS_OK, else returns 
			an error message from called function. 
					
 @param[in]  SignerPublicKey_ptr  - A pointer to the Signer Public Key. 
 @param[in]  MessageRepresent_ptr - A pointer to the message representative.
 @param[in]  SignatureC_ptr       - A pointer to the signature C value. 
 @param[in]  SignatureD_ptr       - A pointer to the signature D value. 
 @param[in]  TempBuff_ptr         - A pointer to temporary buffer.
 		
 @return   CRYSError_t - CRYS_OK,				   
					     Error from called functions
*/
CRYSError_t LLF_ECDSA_VerifyCalcCall( 
			CRYS_ECPKI_PublKey_t   *SignerPublKey_ptr,         /*in*/							
			DxUint32_t             *MessageRepresent_ptr,      /*in*/ 								  							 							              
			DxUint32_t             *SignatureC_ptr,            /*in*/
			DxUint32_t             *SignatureD_ptr,            /*in*/
			DxUint32_t             *TempBuff_ptr               /*in*/ )
{
   /* LOCAL DECLARATIONS */
   
   /* the error identifier */
   CRYSError_t Error; 
   
   const CRYS_ECPKI_Domain_t  *Domain_ptr; 
   
   /* FUNCTION LOGIC */
   
   Domain_ptr = &LLF_EC_DOMAINS_ARRAY[SignerPublKey_ptr->DomainID];  
    
   /*     LLF function call    */

   Error = LLF_ECDSA_VerifyCalc( SignerPublKey_ptr, MessageRepresent_ptr, 
                                 Domain_ptr->EC_OrderSizeInWords,   
	                         SignatureC_ptr, SignatureD_ptr, TempBuff_ptr ); 
   
   return Error;   
 

}/* END OF LLF_ECDSA_VerifyCalcCall */                                       


