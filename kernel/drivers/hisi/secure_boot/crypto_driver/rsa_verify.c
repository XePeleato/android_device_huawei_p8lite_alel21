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

/*
 *  Object %name    : %
 *  State           :  %state%
 *  Creation date   :  Thu Jan 13 09:23:24 2005
 *  Last modified   :  %modify_time%
 */
/** @file
 *  \brief A brief description of this module
 *
 *  \version CRYS_RSA_Verify.c#1:csrc:6
 *  \author ohads
 *  \remarks Copyright (C) 2004 by Discretix Technologies Ltd.
 *           All Rights reserved
 */






/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "systemdep.h"
#include "sb_hash.h"
#include "rsa.h"
#include "util.h"
#include "secureboot_gen.h"
#include "secureboot_general_hwdefs.h"

/************************ Defines ******************************/
void UTIL_MemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );
DxUint32_t UTIL_MemCmp( DxUint8_t *buff1_ptr , DxUint8_t *buff2_ptr , DxUint32_t size );
void UTIL_MemSet( DxUint8_t *buff_ptr , DxUint8_t val , DxUint32_t size );
void UTIL_ReverseBuff( DxUint8_t *buff_ptr , DxUint32_t size );

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/
#ifdef DX_SB_SEC_BOOT_PKCS_V15
/* SHA256 algorithm identifier */
 static const DxUint8_t AlgorithmID_SHA256[] =
 {0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20};
#endif

/************* Private function prototype ****************/

static DxUint32_t UTIL_INVERSE_UINT32_BYTES (DxUint32_t val)
{
#ifdef BIG__ENDIAN
  return  (val);
#else
return  ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24));
#endif
}

/************************ Private Functions ******************************/
#ifdef DX_SB_SEC_BOOT_PKCS_V15			      
/**

   \brief RSA_PKCS15_Verify implements the Verify algorithm
          as defined in PKCS#1 v1.5

   @param[in] DataInHashResult - The DataIn hash result.
   @param[in] N_ptr            - the modolus (1024 bits).
   @param[in] Np_ptr           - the 'H' cofficient.   
   @param[in] RevSig_ptr       - Pointer to the signature to be verified.
                     
                                 Note the signature is reversed. 

   @return CRYSError_t - TRUE - success , FALSE - failure 
   
   \
*/
 
DxUint32_t RSA_PKCS15_Verify( DxUint32_t hwBaseAddress,
                         HASH_Result_t  DataInHashResult, 
                         DxUint32_t      *N_ptr,
                         DxUint32_t      *Np_ptr,
                         DxUint32_t      *RevSig_ptr )			      
{
   /* FUNCTION DECLARATIONS */
   
   /*a buffer used for the encryption result on the signiture */
   DxUint32_t ED[RSA_MOD_SIZE_IN_BYTES/sizeof(DxUint32_t) + 1];

   /* a byte pointer to the ED buffer */
   DxUint8_t *ED_Byte_ptr;
   
   /* loop variable */
   DxUint32_t i, Status;     	
   
   /* FUNCTION LOGIC */
  
   /* ...................... initialize local variables .................... */
   /* ---------------------------------------------------------------------- */
   
   /* set Status to True */
   Status = DX_TRUE;
   /* setting the ED byte pointer */
   ED_Byte_ptr = (DxUint8_t*)ED;
     
   /* ...................... executing the encryption exponent on the signiture ........ */
   /* ---------------------------------------------------------------------------------- */
    
   /* execute the encryption */
   RSA_CalcExponent(hwBaseAddress ,RevSig_ptr,N_ptr,Np_ptr,ED );        

   /* reverse the data */
   UTIL_ReverseBuff( (DxUint8_t*)ED , RSA_MOD_SIZE_IN_BYTES );

   /* ...................... operating the verify primitive ............................. */
   /* ----------------------------------------------------------------------------------- */	
	 
   /*
	*  Check the control octets of encrypted message according to 
	*  PKCS1 v.1.5 paragraph 10.2 <5>: 
	*          EM = 0x00||0x01||PS||0x00||T , where:
	*      size of T = HASH_SIZE + ALG_ID_SIZE = 32 + 19 // for SHA256
	*      size of PS = RSA_MOD_SIZE - HASH_SIZE - 22.
	*/
   if( ED_Byte_ptr[0] != 0x00 ||
       ED_Byte_ptr[1] != 0x01 ||
       ED_Byte_ptr[2 + RSA_MOD_SIZE_IN_BYTES - HASH_RESULT_SIZE_IN_BYTES - 22] != 0x00 )
   { 
       Status = DX_FALSE;
       goto  End;
   } 
   
   /* check that PS = 0xFF,0xFF ... */
   for( i = 2; i < 2 + RSA_MOD_SIZE_IN_BYTES - HASH_RESULT_SIZE_IN_BYTES - 22; i++ )
   {
   		if( ED_Byte_ptr[i] != 0xFF )
   		{
			Status = DX_FALSE;
			goto  End;
   		}
   }
   
   /* check HASH AlgorithmID value */
   Status = UTIL_MemCmp( (DxUint8_t*)&AlgorithmID_SHA256, 
                         (DxUint8_t*)&ED_Byte_ptr[RSA_MOD_SIZE_IN_BYTES - HASH_RESULT_SIZE_IN_BYTES - sizeof(AlgorithmID_SHA256)] , 
                         sizeof(AlgorithmID_SHA256) );
   
   if( Status != DX_TRUE )
   {
   		goto  End;
   }
   
   
   /* check HASH value */
   Status = UTIL_MemCmp( (DxUint8_t*)DataInHashResult , 
                         (DxUint8_t*)&ED_Byte_ptr[RSA_MOD_SIZE_IN_BYTES - HASH_RESULT_SIZE_IN_BYTES] , 
                         sizeof(HASH_RESULT_SIZE_IN_BYTES) );
   
   if( Status != DX_TRUE )
   {
   		goto  End;
   }
  
End:  
  return Status;
    
  
}/* END OF RSA_PKCS15_Verify */



#else

/**

   \brief RSA_PSS_Verify implements the Verify algorithm
   as defined in PKCS#1 v2.1

   @param[in] hwBaseAddress - HW base address
   @param[in] DataInHashResult - The DataIn hash result.
   @param[in] N_ptr           - the modulus (2048 bits).
   @param[in] Np_ptr          - the Barrett tag.   
   @param[in] RevSig_ptr - Pointer to the signature to be
			   verified - note the signiture is reversed. 

   @return CRYSError_t - TRUE - success , FALSE - failure 
*/

DxUint32_t RSA_PSS_Verify(DxUint32_t hwBaseAddress, 
                           HASH_Result_t   DataInHashResult, 
                           DxUint32_t      *N_ptr,
                           DxUint32_t      *Np_ptr,
                           DxUint32_t      *RevSig_ptr )			      
{
/* FUNCTION DECLERATIONS */

#define MASKED_DB_SIZE (RSA_MOD_SIZE_IN_BYTES - HASH_RESULT_SIZE_IN_BYTES - 1)

/* the HASH signiture on the ED buffer after the encryption */
HASH_Result_t HashSigOnED;

/*a buffer used for the encryption result on the signiture */
DxUint32_t ED[RSA_MOD_SIZE_IN_BYTES/sizeof(DxUint32_t) + 1];

/* a byte pointer to the ED buffer */
DxUint8_t *ED_Byte_ptr;

/* temporary buffers and byte pointers to point on them */
DxUint32_t TempBuff[HASH_RESULT_SIZE_IN_WORDS + 1];
DxUint32_t dbMask[RSA_MOD_SIZE_IN_BYTES/sizeof(DxUint32_t)];
DxUint8_t  *dbMask_ptr; 
DxUint8_t  *Output_ptr;

/* loop variable */
DxUint16_t i,Counter;        

/* FUNCTION LOGIC */

/* ...................... initialize local variables .................... */
/* ---------------------------------------------------------------------- */

/* setting the ED byte pointer */
ED_Byte_ptr = (DxUint8_t*)ED;

/* setting the temp buffers byte pointer */
dbMask_ptr     = (DxUint8_t*)dbMask;

/* ...................... executing the encryption exponent on the signiture ........ */
/* ---------------------------------------------------------------------------------- */

/* execute the encryption */
   RSA_CalcExponent( hwBaseAddress,RevSig_ptr,N_ptr,Np_ptr,ED );        

/* reverse the data */
UTIL_ReverseBuff( (DxUint8_t*)ED , RSA_MOD_SIZE_IN_BYTES );

/* ...................... operating the verify primitive ............................. */
/* ----------------------------------------------------------------------------------- */    

/*
     *  9.1.2 <4> Check that the rightmost octet of EM have the hexadecimal value 0xbc
     */
if (ED_Byte_ptr[ RSA_MOD_SIZE_IN_BYTES -1 ] != 0xbc) {
	return DX_FALSE;
}

/*
     *  9.1.2 <5> Define the H and the maskedDB
     */
/*need to save H because ED_ptr is to be used */
UTIL_MemCopy((DxUint8_t *)HashSigOnED,&ED_Byte_ptr[MASKED_DB_SIZE],HASH_RESULT_SIZE_IN_BYTES);

/*
     *  9.1.2 <6> Check that the leftmost bits in the leftmost octet of EM have the value 0
     */

if (ED_Byte_ptr[0] & 0x80) {
	return DX_FALSE;
}

/*
     *  9.1.2 <7> Let dbMask = MGF(H,emLen-hLen-1) 
     */

Output_ptr=dbMask_ptr;

/* copy the HASH on the EB to the temp buffer */  
UTIL_MemCopy((DxUint8_t*)TempBuff,(DxUint8_t*)HashSigOnED,HASH_RESULT_SIZE_IN_BYTES);

/*  For counter from 0 to  | l / hLen | -1 , do the following: *
 *  a.	Convert counter to an octet string C of length 4       *
 *      with the primitive I2OSP:                              * 
 *      C = I2OSP (counter, 4)                                 *
 *  b.	Concatenate the hash of the seed Z and C to the octet  *
 *      string T:                                              *
 *      T = T || Hash (Z || C)                                 *
 *                                                             *
 *                                                             */

for (Counter=0; Counter <= (MASKED_DB_SIZE/HASH_RESULT_SIZE_IN_BYTES) ;Counter ++ ) {
	/* the HASH result in the loop */
	HASH_Result_t HashRes;

	/*--------------------------------------------------------------------
	 *          a.	Convert counter to an octet string C of length 4       
	     *              with the primitive I2OSP:                               
	     *               C = I2OSP (counter, 4)                                
	 *--------------------------------------------------------------------*/

	/* T_TMP = H||C */
	TempBuff[HASH_RESULT_SIZE_IN_WORDS] = UTIL_INVERSE_UINT32_BYTES(Counter);

	/*--------------------------------------------------------------------
	*          b.	Concatenate the hash of the seed Z and C to the octet  
	    *               string T:                                             
	    *                           T = T || Hash (Z || C)                   
	    *--------------------------------------------------------------------*/

	/* Hash Z||C */
	HASH_Init(hwBaseAddress);

	HASH_UpdateData ( hwBaseAddress, TempBuff ,HASH_RESULT_SIZE_IN_WORDS + 1,HASH_RESULT_SIZE_IN_WORDS + 1 , DX_TRUE, DX_FALSE );                

	HASH_GetResults(hwBaseAddress, HashRes );

	UTIL_MemCopy(Output_ptr,(DxUint8_t *)HashRes,HASH_RESULT_SIZE_IN_BYTES);

	Output_ptr+=HASH_RESULT_SIZE_IN_BYTES;

}

/*
 *  9.1.2 <8> Xor operation on length (PubNNewSizeBytes - Context_ptr->hLen - 1)
 */                   

for (i=0;i<MASKED_DB_SIZE;i++)

	dbMask_ptr[i] = dbMask_ptr[i] ^ ED_Byte_ptr[i] ;

/*
 *  9.1.2 <9> Set the leftmost 8emLen - emBits bits of the leftmost octet in DB to zero
 */
dbMask_ptr[0] &= 0x7F;

/*
 *  9.1.2 <10> 
 */

for (i=0;i< RSA_MOD_SIZE_IN_BYTES - HASH_RESULT_SIZE_IN_BYTES - RSA_PSS_SALT_LENGTH - 2	/*TempIndex*/;i++) {
	if (dbMask_ptr[i] != 0) {
		return DX_FALSE;
	}
}

if (dbMask_ptr[i] != 0x01) {
	return DX_FALSE;
}

/*
 *  9.1.2 <11> Let salt be the last sLen octets in DB
 */

/*
 *  9.1.2 <12> Let M' ==> 
 *	 (0x) 00 00 00 00 00 00 00 00 || mHash || salt
 */

UTIL_MemSet(ED_Byte_ptr,0x00,RSA_PSS_PAD1_LEN);

/*copy the Hash output */     
UTIL_MemCopy(&ED_Byte_ptr[RSA_PSS_PAD1_LEN],(DxUint8_t *)DataInHashResult,HASH_RESULT_SIZE_IN_BYTES);  
UTIL_MemCopy(&ED_Byte_ptr[RSA_PSS_PAD1_LEN + HASH_RESULT_SIZE_IN_BYTES],&dbMask_ptr[MASKED_DB_SIZE - RSA_PSS_SALT_LENGTH],RSA_PSS_SALT_LENGTH);        

/*
 *  9.1.2 <13> H' = Hash(M')
 */

HASH_Init(hwBaseAddress);

HASH_UpdateData ( hwBaseAddress, ED, (RSA_PSS_PAD1_LEN + HASH_RESULT_SIZE_IN_BYTES + RSA_PSS_SALT_LENGTH)/sizeof(DxUint32_t),
		  (RSA_PSS_PAD1_LEN + HASH_RESULT_SIZE_IN_BYTES + RSA_PSS_SALT_LENGTH)/sizeof(DxUint32_t),DX_TRUE, DX_FALSE );                

HASH_GetResults( hwBaseAddress, DataInHashResult );


/* check the result */
return(UTIL_MemCmp( (DxUint8_t*)DataInHashResult , (DxUint8_t*)HashSigOnED , sizeof(DataInHashResult) ));

/* ........... STEP5 : releasing the CC device ........................... */
/* ------------------------------------------------------------------------ */

#undef MASKED_DB_SIZE

}/* END OF RSA_PSS_Verify */                          

#endif

