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

#define DX_PAL_LOG_CUR_COMPONENT HOST_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/
#include "rsa_pki_pka.h"
#include "secureboot_gen.h"
#include "rsa.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

#ifdef PKA_DEBUG
  extern DxUint8_t tempRes[268];
#endif  

/* ********** Internal private functions ****************** */




/* ************************** public functions ********************************* */


/**
 * @brief The RSA_CalcExponent calculates The following:
 *
 *                   Res = (Base ^ Exp) mod N. ( Exp = 0x10001 )
 *
 *        The calculation is done in a secured way using the PIC.
 *
 * @hwBaseAddress        - Cryptocell base address
 * @Base_ptr[in]         - The pointer to the base buffer.
 * @N_ptr[in]            - The pointer to the modulus buffer.
 * @Np_ptr[in]           - The Np vector buffer..
 *                       - Its size must be 160. 
 * @Res_ptr[out]         - The pointer to the buffer that will contain the result.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void RSA_CalcExponent( DxUint32_t hwBaseAddress,
                       DxUint32_t *Base_ptr,    
							         DxUint32_t *N_ptr,        
							         DxUint32_t *Np_ptr,        
                       DxUint32_t *Res_ptr )
{
   
   /* Fix value for the Exponent */
   DxUint32_t Exp = RSA_EXP_VAL;
   
  /* FUNCTION LOGIC */
   
   /* .................... initialize local variables ...................... */
   /* ---------------------------------------------------------------------- */
   
   
   /* initialize the PKA engine on default mode */
   RSA_HW_PKI_PKA_DefaultInitPKA( (RSA_MOD_SIZE_IN_BYTES*8),
                               RSA_HW_PKI_PKA_MAX_REGISTER_SIZE_WORDS, hwBaseAddress );
                               
   /* copy modulus N into r0 register */
   RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 0/*dstReg*/, 1/*LenID*/, 
                                   N_ptr/*src_ptr*/, RSA_MOD_SIZE_IN_BYTES, hwBaseAddress ); 

   /* copy the NP into r1 register NP */
   RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 1/*dstReg*/, 1/*LenID*/, Np_ptr/*src_ptr*/, 
                                      4*RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS, hwBaseAddress );   

   /* copy input data into PKI register: DataIn=>r2 */
   RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 2/*dstReg*/, 1/*LenID*/, 
                     Base_ptr, RSA_MOD_SIZE_IN_BYTES, hwBaseAddress );
                                                          
   /* copy exponent data PKI register: e=>r3 */
   RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 3/*dstReg*/, 1/*LenID*/, 
                     &Exp, RSA_EXP_SIZE_IN_BYTES, hwBaseAddress ); 


   /* .. calculate the exponent Res = OpA**OpB mod N;                  ... */
   /* -------------------------------------------------------------------- */      
   RSA_HW_PKI_PKA_ModExp( 0/*LenID*/, 2/*OpA*/, 3/*OpB*/, 4/*Res*/, 0/*Tag*/, hwBaseAddress );
                                   
   /* copy result into output: r4 =>DataOut */
   RSA_HW_PKI_PKA_CopyDataFromPkaReg( Res_ptr, RSA_MOD_SIZE_IN_BYTES, 
                                   4/*srcReg*/, hwBaseAddress );

   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   RSA_HW_PKI_PKA_FinishPKA( hwBaseAddress );
   
   
}/* END OF RSA_CalcExponent */
