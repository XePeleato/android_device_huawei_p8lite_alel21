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

 /* ----------------------------------------------------------
 *
 * Inculde Files
 *
 * ----------------------------------------------------------*/

 #ifndef CRYS_NO_HASH_SUPPORT                                      
 #ifndef CRYS_NO_PKI_SUPPORT                                      

#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif


#include "dx_vos_mem.h"
#include "crys_common_math.h"
#include "crys_rsa_local.h"
#include "crys_rsa_error.h"
#include "plat_print_dbg.h"
#include "crys_hash.h"
 
 /* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

 #ifdef CRYS_RSA_SIGN_USE_TEMP_SALT
	#include "CRYS_RSA_PSS21_defines.h"	
	extern DxUint8_t SaltDB_T[NUM_OF_SETS_TEST_VECTORS][NUM_OF_TEST_VECTOR_IN_SET][CRYS_RSA_PSS_SALT_LENGTH];
	extern DxUint16_t Global_Set_Index_T;
	extern DxUint16_t Global_vector_Index_T;
 #endif


#if !defined(_INTERNAL_CRYS_NO_RSA_SCHEME_21_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)

/**********************************************************************************************************/
/**
 	Function Name: CRYS_RSA_PSS_Verify21
 	Date:   06-12-2004
 	Author:	Ohad Shperling
 
 
    \brief CRYS_RSA_PSS_Verify21 implements EMSA-PSS-Verify algorithm 
   as defined in PKCS#1 v2.1 Sec 9.1.2
 
   @param[in] Context_ptr - Pointer to a valid context as
                            given from the VerifyFinish function.
                            
   The field HASH_Result inside the Context_ptr is initialized with the Hashed digested message.
   The field HASH_Result_Size inside the Context_ptr is initialized with the Hash digested message size

   @return CRYSError_t - CRYS_OK,
                         CRYS_BAD_PARAM, CRYS_OUT_OF_RANGE 
*/




CRYSError_t CRYS_RSA_PSS_Verify21(RSAPubContext_t *Context_ptr)
{
		/**********Fitting to the spec******************************/
		/* Context_ptr->MsgDigestCAL = mHash = Hash(M)
	     * &Context_ptr->KeyObj.PubObj.EBD[0] = pointer to EM = S^E mod N
	     * &Context_ptr->KeyObj.PubObj.EBDSize = pointer to EM size
  		*/
  

	CRYSError_t Error ;
	DxUint8_t   *ED_ptr;
	DxUint32_t  EDSizeInBytes;
	DxUint32_t PubNNewSizeBytes,i;
	DxUint8_t *maskedDB_ptr;
	
	DxUint32_t maskedDB_size;
	DxUint32_t TempIndex ;
	DxUint8_t TempByte;
	DxUint8_t	*dbMask_ptr = Context_ptr->T_Buf;

    CRYS_HASH_Result_t H_Saved_buf;
       /* Set the ED block pointer */  
	
	/*Temporary - only for the size of N*/       
    CRYSRSAPubKey_t *PubKey_ptr = (CRYSRSAPubKey_t *)Context_ptr->PubUserKey.PublicKeyDbBuff;

   	/* the hash operation mode for inner hash */
	CRYS_HASH_OperationMode_t HashOperationMode;

    /* FUNCTION LOGIC */   

    PLAT_LOG_DEV_PRINT(( 20 ," \nStart Printing from CRYS_RSA_PSS_Verify21\n" ));
    
    ED_ptr=(DxUint8_t*) &Context_ptr->EBD[0];/* = EM*/
    EDSizeInBytes= Context_ptr->EBDSizeInBits/8;
    if(Context_ptr->EBDSizeInBits % 8)
    	EDSizeInBytes++;
    
    	/*Round up the new bytes number - According to the Spec*/
    PubNNewSizeBytes = (PubKey_ptr->nSizeInBits -1)/8;    	


    	
    if(((PubKey_ptr->nSizeInBits -1) % 8) != 0)
    {/*Rounding Only in case that (PubNSizebits -1) is not divisble by 8 */
    	PubNNewSizeBytes++;
   	}
    else
    {/*(PubNSizebits -1) is divisble by 8 hence ED_ptr has to be shortened by the first octet according to the spec*/
    	ED_ptr+=1;
    	EDSizeInBytes-=1;
    }	
    
    
 	/*
	 *  9.1.2 <3> Check restriction of PubNNewSizeBytes - already checked in Verify Init
	 */	 

	 
 	/*
	 *  9.1.2 <4> Check that the rightmost octet of EM have the hexadecimal value 0xbc
	 */
	 if(ED_ptr[EDSizeInBytes-1] != 0xbc)
		return CRYS_RSA_ERROR_PSS_INCONSISTENT_VERIFY;

 	/*
	 *  9.1.2 <5> Define the H and the maskedDB
	 */

	 maskedDB_ptr = ED_ptr;
	 maskedDB_size = PubNNewSizeBytes - Context_ptr->HASH_Result_Size*4 -1 ;
	 
	 /*need to save H because ED_ptr is to be used - Context_ptr->MsgDigestSRC = H*/
	 DX_VOS_FastMemCpy((DxUint8_t *)H_Saved_buf,&ED_ptr[maskedDB_size],Context_ptr->HASH_Result_Size*4);

 	/*
	 *  9.1.2 <6> Check that the leftmost bits in the leftmost octet of EM have the value 0
	 */

	 TempIndex = 8*PubNNewSizeBytes - (PubKey_ptr->nSizeInBits - 1);
	 /*Note TempIndex is size in bits*/
	 TempByte = 0x80;
	 for(i=0;i<TempIndex;i++)
	 {
	 	if(maskedDB_ptr[0] & TempByte)
			return CRYS_RSA_ERROR_PSS_INCONSISTENT_VERIFY;
	 	TempByte >>=1;
	 }

 	/*
	 *  9.1.2 <7> Let dbMask = MGF(H,emLen-hLen-1)
	 */
	 
	/*Setting the correct hash function mode*/
    switch(Context_ptr->HashOperationMode)
	{
		case CRYS_RSA_HASH_SHA1_mode:
		case CRYS_RSA_After_SHA1_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA1_mode; 
	      			break;
	      			
	    case CRYS_RSA_HASH_SHA224_mode:
		case CRYS_RSA_After_SHA224_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA224_mode; 
	      			break;
	      			
	    case CRYS_RSA_HASH_SHA256_mode:
		case CRYS_RSA_After_SHA256_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA256_mode; 
	      			break;
	      			
	    case CRYS_RSA_HASH_SHA384_mode:
		case CRYS_RSA_After_SHA384_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA384_mode; 
	      			break;
	      			
	    case CRYS_RSA_HASH_SHA512_mode:
		case CRYS_RSA_After_SHA512_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA512_mode; 
	      			break;
	      			
		case CRYS_RSA_HASH_MD5_mode:
   		case CRYS_RSA_After_MD5_mode:	 		   
	      			HashOperationMode = CRYS_HASH_MD5_mode;  		   
	      			break;
			 		      			
		case CRYS_RSA_HASH_NO_HASH_mode:
	 		      	default:
	      			/*No other Hash functions are operating for now*/
	      			return CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
	}		   				   	
	
	
	switch(Context_ptr->MGF_2use)
	{
		case CRYS_PKCS1_MGF1:

			Error = CRYS_RSA_OAEPMGF1( (DxUint16_t)(Context_ptr->HASH_Result_Size*4),
									   (DxUint8_t *)H_Saved_buf,/*H*/
									   (DxUint16_t)(Context_ptr->HASH_Result_Size*4),		  /*H_Size*/
									   PubNNewSizeBytes - Context_ptr->HASH_Result_Size*4 -1,
									   dbMask_ptr,
									   HashOperationMode,
							    	   (DxUint8_t *)Context_ptr->PrimeData.DataOut,
							    	   (DxUint8_t *)Context_ptr->PrimeData.DataIn);
			if(Error != CRYS_OK)
			{
				return Error;
			}															
								
			break;

	    /*Currently for PKCS1 Ver 2.1 only MGF1 is implemented*/								
		case CRYS_PKCS1_NO_MGF:	 
		default: 				 
			
			return CRYS_RSA_MGF_ILLEGAL_ARG_ERROR;
							
	}/* end of MGF type switch case */							



        PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \ndbMask = MGF(H,emLen-hLen-1) = " ,(DxUint8_t *)dbMask_ptr,PubNNewSizeBytes - Context_ptr->HASH_Result_Size*4 -1));            
  	/*
	 *  9.1.2 <8> Xor operation on length (PubNNewSizeBytes - Context_ptr->hLen - 1)
	 */			

    for(i=0;i<maskedDB_size;i++)
    {
		dbMask_ptr[i] = dbMask_ptr[i] ^ maskedDB_ptr[i]	;
	}

    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \ndbMask_ptr = dbMask_ptr XOR maskedDB_ptr = " ,(DxUint8_t *)dbMask_ptr , maskedDB_size));            
    
 	/*
	 *  9.1.2 <9> Set the leftmost 8emLen - emBits bits of the leftmost octet in DB to zero
	 */

	 TempIndex = 8*PubNNewSizeBytes - (PubKey_ptr->nSizeInBits-1);
	 /*Note TempIndex is size in bits*/
	 TempByte = 0x7F;
	 for(i=0;i<TempIndex;i++)
	 {
	 	dbMask_ptr[0] &= TempByte;
	 	TempByte >>=1;
	 }
	 
  	/*
	 *  9.1.2 <10> 
	 */
	 
     if(Context_ptr->SaltLen == CRYS_RSA_VERIFY_SALT_LENGTH_UNKNOWN)
     {
        i=0;
        while(dbMask_ptr[i] == 0)
            i++;
     }
     else
     {
	    TempIndex = PubNNewSizeBytes - Context_ptr->HASH_Result_Size*4 - Context_ptr->SaltLen - 2;
	    for(i=0;i<TempIndex;i++)
	    {
	 	    if(dbMask_ptr[i] != 0)
	 		    return CRYS_RSA_ERROR_PSS_INCONSISTENT_VERIFY;
	    }
     }

     if(dbMask_ptr[i] != 0x01)
	 	    return CRYS_RSA_ERROR_PSS_INCONSISTENT_VERIFY;

     /*Derive the salt length if not supported*/
     if(Context_ptr->SaltLen == CRYS_RSA_VERIFY_SALT_LENGTH_UNKNOWN)
     {
         Context_ptr->SaltLen = (DxUint16_t)(PubNNewSizeBytes - Context_ptr->HASH_Result_Size*4 - 2 - i);
     }

  	/*
	 *  9.1.2 <11> Let salt be the last sLen octets in DB
	 */

  	/*
	 *  9.1.2 <12> Let M' ==> 
	 *	 (0x) 00 00 00 00 00 00 00 00 || mHash || salt
	 */	 	


	DX_VOS_MemSet(ED_ptr,0x00,CRYS_RSA_PSS_PAD1_LEN);/*DX_CRYS_RSA_PSS_PAD1_LEN = 8*/

	/*copy the Hash output */	
	DX_VOS_FastMemCpy(&ED_ptr[CRYS_RSA_PSS_PAD1_LEN],(DxUint8_t *)Context_ptr->HASH_Result,Context_ptr->HASH_Result_Size*4);	 
	DX_VOS_FastMemCpy(&ED_ptr[CRYS_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*4],&dbMask_ptr[maskedDB_size - Context_ptr->SaltLen],Context_ptr->SaltLen);	 

    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nM' = (0x) 00 00 00 00 00 00 00 00 || mHash || salt = " ,(DxUint8_t *)ED_ptr , CRYS_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*4 + Context_ptr->SaltLen));            
  	/*
	 *  9.1.2 <13> H' = Hash(M')
	 */

	

	Error = CRYS_HASH(HashOperationMode,
					ED_ptr,
					CRYS_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*4 + Context_ptr->SaltLen,/*8+20+20*/
					Context_ptr->HASH_Result);
		
	if(Error != CRYS_OK)
	{
		return Error;
	}
	
    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nAfter CRYS_HASH - Hash(M') = " ,(DxUint8_t *)Context_ptr->HASH_Result,(DxUint32_t)Context_ptr->HASH_Result_Size*4));            

    PLAT_LOG_DEV_PRINT(( 20 ," \nEnd Printing from CRYS_RSA_PSS_Verify21\n" ));
    
	if(DX_VOS_MemCmp((DxUint8_t *)Context_ptr->HASH_Result,(DxUint8_t *)H_Saved_buf,Context_ptr->HASH_Result_Size*4))
	{
		return CRYS_RSA_ERROR_PSS_INCONSISTENT_VERIFY;
	}
	else 
	{
		return CRYS_OK;
	}
	
	
}

#endif /*!defined(_INTERNAL_CRYS_NO_RSA_SCHEME_21_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)*/

#if !defined(_INTERNAL_CRYS_NO_RSA_SCHEME_21_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)
#ifndef _INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT
/* -------------------------------------------------------------
 *	Function Name: CRYS_RSA_PSS_Sign21
 *	Date:   06-12-2004
 *	Author:	Ohad Shperling
 *
 *	Inputs:
 *
 *	Outputs:
 *
 *	Local Data Structures:
 *
 *	Description and Design info:
 *		Purpose,
 *		Assumptions,
 *		Constrains,
 *		Limitations,
 *		Side effects
 *
 *	Algorithm:
 *
 *
 *	List Of Units Used:
 *
 *	Trace info to SRS, IDD and Detailed SDD:
 *
 *	Update History:
 *	Date:		Description:
 *	--------------------------------------
 *
 * ----------------------------------------------------------- */




CRYSError_t CRYS_RSA_PSS_Sign21(RSAPrivContext_t *Context_ptr,
                                    DxUint8_t     *Output_ptr)
{
	
	#ifdef CRYS_RSA_SIGN_USE_TEMP_SALT	
		/*only for debug of signing*/
		/*Using a known Salt for debug*/
		DxUint8_t *Salt = SaltDB_T[Global_Set_Index_T][Global_vector_Index_T]; 
	#else
		/*In operational mode Salt is a random number*/
		DxUint8_t *Salt = Output_ptr;/*This stack memory saving is ok because Output_ptr is used only in the Primitive operation*/
	#endif

	/* The return error identifier */
	CRYSError_t Error = CRYS_OK;
	DxUint32_t i;
	DxUint32_t TempIndex;

	/*Parameter for the actual size of the modulus N in bits*/
	DxUint32_t PrvNSizebits;
	
	/*Parameter for the new size of the modulus N in bytes according to PKCS1 Ver 2.1*/
	DxUint32_t PrvNNewSizeBytes;/*rounded number of Bytes for padding2 length*/
	DxUint32_t Index4PSLength;

	DxUint8_t *EMPadOutputBuffer;
	DxUint8_t	*MaskOutput_ptr = Context_ptr->T_Buf;/*for stack space saving*/
	
	/*Parameter for bitwise operation on one Byte*/
	DxUint8_t TempByte;
	
	CRYSRSAPrivKey_t *PrivKey_ptr = (CRYSRSAPrivKey_t *)Context_ptr->PrivUserKey.PrivateKeyDbBuff;
	
	/* the hash operation mode for inner hash */
	CRYS_HASH_OperationMode_t HashOperationMode;
		
   /* FUNCTION LOGIC */
   
   /* .................. initializing local variables ................... */
   /* ------------------------------------------------------------------- */   
   
   PLAT_LOG_DEV_PRINT(( 20 ," \nStart Printing from CRYS_RSA_PSS_Sign21\n" ));
   
   /* initializing the Error to O.K */
   Error = CRYS_OK;    
   
   EMPadOutputBuffer = (DxUint8_t*) Context_ptr->EBD;
      
	/*
	 * ? 9.1.1 <1> checking length restriction of the message M - done in the Update phase
	 */
	 
 	/*
	 *  9.1.1 <2> Hash operation - done in the Update phase
	 */
	 
    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nCRYS_RSA_PSS_Sign21 mHash = " ,(DxUint8_t *)Context_ptr->HASH_Result,(DxUint32_t)Context_ptr->HASH_Result_Size*4));            
    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nCRYS_RSA_PSS_Sign21 Salt  = " ,(DxUint8_t *)Salt,Context_ptr->SaltLen));            
    
    /*
     *	Finding Actual size in bits and new size of Bytes of the modulus N
     *	This value is already calculated in 
     *	Context_ptr->KeyObj.PrvCRTObj.nSizeInBits   or in
     *	Context_ptr->KeyObj.PrvPAIRObj.nSizeInBits
     */

    /*Reset the working buffer*/	   
    DX_VOS_MemSet(EMPadOutputBuffer,0x00,CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4);
    
    /*Round up the new bytes number*/
    PrvNNewSizeBytes = (PrivKey_ptr->nSizeInBits -1)/8;
    PrvNSizebits = PrivKey_ptr->nSizeInBits;
     
    if(((PrvNSizebits -1) % 8) != 0)
    	PrvNNewSizeBytes++;
    	/*rounding */
    
 	/*
	 *  9.1.1 <3> Check restriction of PrvNNewSizeBytes - already checked in Sign Init
	 */	 
	 
		
  	/*
	 *  9.1.1 <5> Generating M' ==> using the output buffer as a container
	 *	EMPadOutputBuffer = (0x) 00 00 00 00 00 00 00 00 || mHash || salt
	 */		
			
	DX_VOS_MemSet(EMPadOutputBuffer,0x00,CRYS_RSA_PSS_PAD1_LEN);/*DX_CRYS_RSA_PSS_PAD1_LEN = 8*/

	/*copy the Hash output */	
	DX_VOS_FastMemCpy(&EMPadOutputBuffer[CRYS_RSA_PSS_PAD1_LEN],(DxUint8_t *)Context_ptr->HASH_Result,Context_ptr->HASH_Result_Size*4);
	
	
  	/*
	 *  9.1.1 <4> Generating a random salt ==> using the output buffer as a container
	 */
	#ifndef CRYS_RSA_SIGN_USE_TEMP_SALT /*If not using a known salt for Debug then generate random*/
		Error = CRYS_RND_GenerateVector(Context_ptr->SaltLen,Salt);
		if(Error != CRYS_OK)
		{
			return Error;
		}	
	#endif

	DX_VOS_FastMemCpy(&EMPadOutputBuffer[CRYS_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*4],Salt,Context_ptr->SaltLen);
	
    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nCRYS_RSA_PSS_Sign21 M' = " ,(DxUint8_t *)EMPadOutputBuffer,(DxUint32_t)CRYS_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*4 + Context_ptr->SaltLen));            

  	/*
	 *  9.1.1 <6> Hash(M')
	 */	
	
	/*Setting the correct hash function mode*/
    switch(Context_ptr->HashOperationMode)
	{
		case CRYS_RSA_HASH_SHA1_mode:
		case CRYS_RSA_After_SHA1_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA1_mode; 
	      			break;
	      			
	    case CRYS_RSA_HASH_SHA224_mode:
		case CRYS_RSA_After_SHA224_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA224_mode; 
	      			break;
	      			
	    case CRYS_RSA_HASH_SHA256_mode:
		case CRYS_RSA_After_SHA256_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA256_mode; 
	      			break;
	      			
	    case CRYS_RSA_HASH_SHA384_mode:
		case CRYS_RSA_After_SHA384_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA384_mode; 
	      			break;
	      			
	    case CRYS_RSA_HASH_SHA512_mode:
		case CRYS_RSA_After_SHA512_mode:	 		   
	     			HashOperationMode = CRYS_HASH_SHA512_mode; 
	      			break;
	      			
		case CRYS_RSA_HASH_MD5_mode:
   		case CRYS_RSA_After_MD5_mode:	 		   
	      			HashOperationMode = CRYS_HASH_MD5_mode;  		   
	      			break;
			 		      			
		case CRYS_RSA_HASH_NO_HASH_mode:
	 		      	default:
	      			/*No other Hash functions are operating for now*/
	      			return CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
	}		   				   		
	
	Error = CRYS_HASH(HashOperationMode,
					EMPadOutputBuffer,
					CRYS_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*4 + Context_ptr->SaltLen,/*8+hLen+20*/
					Context_ptr->HASH_Result);
		
	if(Error != CRYS_OK)
	{
		return Error;
	}

    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nAfter CRYS_HASH - Hash(M') = " ,(DxUint8_t *)Context_ptr->HASH_Result,(DxUint32_t)Context_ptr->HASH_Result_Size*4));            	

  	/*
	 *  9.1.1 <7+8> Generate an octet string of zeros of size emLen-sLen-hLen-2 ==> use the output buffer as a container
	 *				DB = PS || 0x01 || salt
	 */	
	 

    Index4PSLength = PrvNNewSizeBytes - Context_ptr->SaltLen - Context_ptr->HASH_Result_Size*4 - 2;
	
	DX_VOS_MemSet(EMPadOutputBuffer,0x00,Index4PSLength) ; 
	EMPadOutputBuffer[Index4PSLength] = 0x01;
	DX_VOS_FastMemCpy(&(EMPadOutputBuffer[Index4PSLength+1]),Salt,Context_ptr->SaltLen);

    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nDB = Padding || salt = " ,(DxUint8_t *)EMPadOutputBuffer,Index4PSLength + 1 + Context_ptr->SaltLen));            		

  	/*
	 *  9.1.1 <9> MGF operation
	 */	

	switch(Context_ptr->MGF_2use)
	{
		case CRYS_PKCS1_MGF1:

			Error = CRYS_RSA_OAEPMGF1( (DxUint16_t)(Context_ptr->HASH_Result_Size*4),
									   (DxUint8_t *)Context_ptr->HASH_Result,
									   (DxUint16_t)(Context_ptr->HASH_Result_Size*4),
										PrvNNewSizeBytes - Context_ptr->HASH_Result_Size*4 - 1,
										MaskOutput_ptr,
										HashOperationMode,
										(DxUint8_t *)Context_ptr->PrimeData.DataOut,
										(DxUint8_t *)Context_ptr->PrimeData.DataIn);
			if(Error != CRYS_OK)
			{
				return Error;
			}															
								
		    break;

	    /*Currently for PKCS1 Ver 2.1 only MGF1 is implemented*/								
		case CRYS_PKCS1_NO_MGF:	 
			
		default: 				 
			
			return CRYS_RSA_MGF_ILLEGAL_ARG_ERROR;
							
	}/* end of MGF type switch case */							


    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \ndbMask = MGF(H, length(DB)) = " ,(DxUint8_t *)MaskOutput_ptr,PrvNNewSizeBytes - Context_ptr->HASH_Result_Size*4 - 1));
										
  	/*
	 *  9.1.1 <10> Xor operation on length (PrvNNewSizeBytes - Context_ptr->hLen - 1)
	 */			
	 
	TempIndex = PrvNNewSizeBytes - Context_ptr->HASH_Result_Size*4 - 1;
    for(i=0;i<TempIndex;i++)
    {
		EMPadOutputBuffer[i] = EMPadOutputBuffer[i] ^ *(MaskOutput_ptr+i)	;
	}


    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nmaskedDB = DB xor dbMask (leftmost bit set to zero) = " ,(DxUint8_t *)EMPadOutputBuffer,TempIndex));
  	/*
	 *  ? 9.1.1 <11> Set the leftmost 8*emLen-emBits bits of the leftmost octet in maskedDB to zero
	 *		By convention the pointer is pointing to the leftmost Octet which is the most significant octet
	 */
	 
	 TempIndex = 8*PrvNNewSizeBytes - (PrvNSizebits-1);
	 
     PLAT_LOG_DEV_PRINT(( 20 , " \nNumber of bits to reset in First Byte = TempIndex = %lX\n", TempIndex)); 	 
     PLAT_LOG_DEV_PRINT(( 20 , " \nPrvNSizebits= %lX\n", PrvNSizebits)); 	      
     
	 /*Note TempIndex is size in bits*/
	 TempByte = 0x7F;
	 for(i=0;i<TempIndex;i++)
	 {
	 	EMPadOutputBuffer[0] &= TempByte;
	 	TempByte >>=1;
	 }

   	 
	/*
	 *  ? 9.1.1 <12> Let EM = maskedDB || H || 0xbc 
	 *			Note that maskedDB is already generated from the Xor operation
	 */	
	 DX_VOS_FastMemCpy(&(EMPadOutputBuffer[PrvNNewSizeBytes - Context_ptr->HASH_Result_Size*4 - 1]),(DxUint8_t *)Context_ptr->HASH_Result,Context_ptr->HASH_Result_Size*4);

	 EMPadOutputBuffer[PrvNNewSizeBytes - Context_ptr->HASH_Result_Size*4 - 1 + Context_ptr->HASH_Result_Size*4] = 0xbc;

    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nInput to the Exp = EM = maskedDB || H || 0xbc = " ,(DxUint8_t *)EMPadOutputBuffer,PrvNNewSizeBytes));
    
	/*
	 *	FINISH 9.1.1
	 */
	 
	/*
	 *	8.1.1 <2.a>
	 *	Convert the encoded message EM to an integer message representative m
	 *	  NO NEED TO ! :-) 
	 */	
	 
	/*
	 *	8.1.1 <2.b>
	 *	Apply the RSASP1 signature primitive to the RSA private key K and the message 
	 *  representative m to produce an integer signature representative s
	 *	
	 */	 
	 

	 /*-------------------------------------------*/
	 /*                                           */
	 /* RSA computation			                  */
	 /*                                           */
	 /*-------------------------------------------*/

 
    /* ......................... execute RSA encryped ....................... */
    /* ---------------------------------------------------------------------- */
   
    PLAT_LOG_DEV_PRINT(( 20 , " \nPrintings before CRYS_RSA_PRIM_Decrypt:\n" )); 
    /*Note PubNNewSizeBytes is correct in all cases except 1025*/
    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nModulus N = " ,(DxUint8_t *)PrivKey_ptr->n , PrvNNewSizeBytes));   
    #ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED
	    if(PrivKey_ptr->OperationMode == CRYS_RSA_NoCrt)
	    {
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nExp D =\n" ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.NonCrt.d , PrivKey_ptr->PriveKeyDb.NonCrt.dSizeInBits/8));   	    
	    }
	    else
	    {
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nP = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.P , PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits/8));   	    	    
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nQ = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.Q , PrivKey_ptr->PriveKeyDb.Crt.QSizeInBits/8));   	    	    
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \ndP = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.dP , PrivKey_ptr->PriveKeyDb.Crt.dPSizeInBits/8));   	    	    
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \ndQ = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.dQ , PrivKey_ptr->PriveKeyDb.Crt.dQSizeInBits/8));   	    	    
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nqInv = " ,(DxUint8_t *)PrivKey_ptr->PriveKeyDb.Crt.qInv , PrivKey_ptr->PriveKeyDb.Crt.qInvSizeInBits/8));   	    	    
	    }
	#endif

	/* execute the expomnent - after correct building it is working with either CRT or PAIR mode*/ 
   Error = CRYS_RSA_PRIM_Decrypt(&Context_ptr->PrivUserKey,
                                 &Context_ptr->PrimeData,
                                 EMPadOutputBuffer,
                                 (DxUint16_t)PrvNNewSizeBytes,
                                 Output_ptr);
	
   if(Error!=CRYS_OK)
   {
	    PLAT_LOG_DEV_PRINT(( 20 , " \nCRYS_RSA_PRIM_Decrypt error code %lX ; returning from CRYS_RSA_PSS_Sign21\n" , Error ));               
		return Error;
   }

   PLAT_LOG_DEV_PRINT_DisplayBuffer(( 20 , (char *)" \nCRYS_RSA_PRIM_Decrypt output = " ,(DxUint8_t *)Output_ptr , PrvNNewSizeBytes));   	    	    

   PLAT_LOG_DEV_PRINT(( 20 ," \nEnd Printing from CRYS_RSA_PSS_Sign21\n" )); 		  		          

	return Error;	

}

#endif /*!defined(_INTERNAL_CRYS_NO_RSA_SCHEME_21_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)*/

#endif /*CRYS_NO_HASH_SUPPORT*/
#endif /*CRYS_NO_PKI_SUPPORT*/                                      

#if defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT)                                     

void CRYS_RSA_PSS21_UTIL_foo(void) {}
#endif //_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT
#endif /* defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) */

