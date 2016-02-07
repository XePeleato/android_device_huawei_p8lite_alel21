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

 #ifndef CRYS_NO_HASH_SUPPORT                                      
 #ifndef CRYS_NO_PKI_SUPPORT                                      

 //#include "CRYS_CCM.h"
 #include "dx_vos_mem.h"
#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif

#include "crys_common_math.h"
#include "crys_rsa_local.h"
#include "crys_rsa_error.h"
#include "plat_print_dbg.h"

 
/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/


/************* Private function prototype ****************/

#if !defined(_INTERNAL_CRYS_NO_RSA_SCHEME_15_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)

/************************ Public Functions ******************************/

/**
 	Function Name: CRYS_RSA_PKCS1_v1_5_Decode
 	Date:   18-01-2005
 	Author:	Ohad Shperling
 
 
    \brief CRYS_RSA_PKCS1_v1_5_Encode implements EME-PKCS1_v1_5_Encoding algorithm 
   as defined in PKCS#1 v1.5 Sec 8.1
 
   @param[in] EncryptedBlock - Pointer to the Encrypted block to parse - given after an exponent operation
   @param[in] K - The modulus size denoted in octets.   
   @param[out] D_ptr - Pointer to the output Data - to parse out of the EncryptedBlock buffer
   @param[out] DSize - Denotes the Data size
	Note BlockType - 
					BlockType for PUB Key is 02
					BlockType for Priv Key is 01
					
   @return CRYSError_t - CRYS_OK,
                         
*/
CRYSError_t CRYS_RSA_PKCS1_v1_5_Decode(DxUint8_t     *EncryptedBlock,
									   DxUint16_t     K,
                                       DxUint8_t     *D_ptr,
                                       DxUint16_t    *DSize_ptr)
{
	/* The return error identifier */
	CRYSError_t Error;
	
	/* Padding String Size                   */
	DxUint16_t  I = 0;      
  
    /*The block Type parsed from the input*/
	CRYS_PKCS1_Block_Type 	BlockType;
	
	DxUint8_t Mask=0x00;
	
	/* FUNCTION LOGIC */
	/* .................. initializing local variables ................... */
	/* ------------------------------------------------------------------- */   
   
	/* initializing the Error to O.K */
	Error = CRYS_OK;      

		 /*-------------------------------------------*/
		 /*                                           */
		 /* Step 4 : Decrypted block parsing          */
		 /*                                           */
		 /*          00 || BT || PS || 00 || D        */
		 /*                                           */
		 /*          MSB                     LSB      */
		 /*          |<----- Size = K ------->|       */
		 /*                                           */
		 /*                                           */
		 /*-------------------------------------------*/
		 
	if(EncryptedBlock[0] != 0)
		return CRYS_RSA_15_ERROR_IN_DECRYPTED_BLOCK_PARSING;
		
	BlockType = (CRYS_PKCS1_Block_Type)EncryptedBlock[1];

	switch(BlockType)
	{
		  case CRYS_RSA_PKCS1_15_MODE00:  /*Mask=0x00;*/

			  /* Please refer to PKCS1 Ver 1.5 Standart for the security argument why
			     not to use this kind of Block Type */
			  return CRYS_RSA_PKCS1_15_BLOCK_TYPE_NOT_SUPPORTED ;
		  								  
		  case CRYS_RSA_PKCS1_15_MODE01:  
			    
			  Mask=0xFF;
		  	  break;

		  case CRYS_RSA_PKCS1_15_MODE02:  
			  
			  Mask=Mask; /* junk - was created using Random */
		  	  break;

		  default:
			  return CRYS_RSA_15_ERROR_IN_DECRYPTED_BLOCK_PARSING;
	}
	
		  /* scan and check the padding string for CRYS_RSA_PKCS1_15_MODE01 */
		  if(BlockType==CRYS_RSA_PKCS1_15_MODE01)
		  {
			  for(I=2;I<K;I++)
			  {
			     if(EncryptedBlock[I]!=Mask) /* start scanning from cell 3 */
			        break;
			  }

			  /* check that the current octet (next after after PS) is 0x00 */
			  if( EncryptedBlock[I] != 0 )
				  return CRYS_RSA_15_ERROR_IN_DECRYPTED_DATA;
		  }	
          
		  /* scan the padding string for CRYS_RSA_PKCS1_15_MODE02 */
  		  if(BlockType==CRYS_RSA_PKCS1_15_MODE02)
  		  {
		 	  for(I=2;I<K;I++)
			  {
			     if(EncryptedBlock[I] == 0x00)
			     	break;
			  }
  		  }
  		  
		  /* check that PS size is not less than 8;   
		     note: at this point I is equal to PS size + 2  */
		  if( I - 2 < 8 )	
			  	return CRYS_RSA_15_ERROR_IN_DECRYPTED_DATA_SIZE;
			  	
		  /*Checking that the Output buffer Size is enough large for result output */
	      if( *DSize_ptr < (DxUint16_t)(K-(I+1)) )
	      {
		      *DSize_ptr = (DxUint16_t)(K-(I+1));
		      return CRYS_RSA_DECRYPT_INVALID_OUTPUT_SIZE;
	      }

		  else if( *DSize_ptr > (DxUint16_t)(K-(I+1))) 
			  /* set actual size of decrypted message */
			  *DSize_ptr = (DxUint16_t)(K-(I+1));		  		    		  

          /* copy decrypted message into output */
		  /*note - if I+1 == K than the decrypted data size is zero*/
		  DX_VOS_FastMemCpy( D_ptr, &EncryptedBlock[I+1], *DSize_ptr );
		   
		  return Error;
		  
}
#endif /* !defined(_INTERNAL_CRYS_NO_RSA_SCHEME_15_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)*/



#if !defined(_INTERNAL_CRYS_NO_RSA_SCHEME_15_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_SIGN_SUPPORT)
/**********************************************************************************************************/
/**
 	Function Name: CRYS_RSA_PKCS1_v1_5_Encode
 	Date:   18-01-2005
 	Author:	Ohad Shperling
 
 
    \brief CRYS_RSA_PKCS1_v1_5_Encode implements EME-PKCS1_v1_5_Encoding algorithm 
   as defined in PKCS#1 v1.5 Sec 8.1
 
   @param[in] K - The modulus size denoted in octets.
   @param[in] M_ptr - Pointer to the Message to pad the ouput stream
   @param[in] MSize - Denotes the Message size
   @param[in] BlockType - 
					BlockType for PUB Key is 02
					BlockType for Priv Key is 01
   @param[out] Output_ptr - Pointer to a buffer which is at least K octets long
   
   @return CRYSError_t - CRYS_OK,
                         
*/
CRYSError_t CRYS_RSA_PKCS1_v1_5_Encode(DxUint16_t K,
									   CRYS_PKCS1_Block_Type 	BlockType,
                                       DxUint8_t     *M_ptr,
                                       DxUint32_t     MSize,
                                       DxUint8_t     *Output_ptr)
{

	/* The return error identifier */
	CRYSError_t Error;
	
	/* Padding String Size                   */
	DxUint16_t  PSSize,I,J;      
  
    
	/* FUNCTION LOGIC */
   
	/* .................. initializing local variables ................... */
	/* ------------------------------------------------------------------- */   
   
	/* initializing the Error to O.K */
	Error = CRYS_OK;      

	/*Initializing the EB buffer to zero*/
	DX_VOS_MemSet(Output_ptr,0x0,K);
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */	
	
	if(MSize>(DxUint32_t)(K-11))
		return CRYS_RSA_ENCODE_15_MSG_OUT_OF_RANGE;
		
	/*-------------------------------------------*/
	/*                                           */
	/* Step 1 : Encryption block formating       */
	/*                                           */
	/*          00 || BT || PS || 00 || D        */
	/*         MSByte                   LSByte   */
	/* Note: - 						    		 */
	/*       - BT for PUBKey is 02               */
	/*       - PS size is k-3-||D|| , padding    */
	/*            value is pseudorandomly non    */
	/*            zero                           */
	/*-------------------------------------------*/

	Output_ptr[0]=0x00; /* set the 00     */
	Output_ptr[1]=BlockType;    /* set Block Type */
	
	PSSize=(DxUint16_t)(K-3-MSize);
	
			  /*  In PKCS#1 V2 : The padding string PS in EME-PKCS1-v1_5
		      is at least eight octets long, which is a security 
		      condition for public-key operations that prevents an
		      attacker from recovering data by trying all possible 
		      encryption blocks. */
		      
	if(PSSize<0x08)
 	{		      
 		return CRYS_RSA_ENCODE_15_PS_TOO_SHORT;
 	}
 	
 	
	switch(BlockType)
	{

				 	 case CRYS_RSA_PKCS1_15_MODE00: 
				 	 		/* Please refer to PKCS1 Ver 1.5 Standart for the security argument why
				 	 		   not to use this kind of Block Type*/
				 	 		return CRYS_RSA_PKCS1_15_BLOCK_TYPE_NOT_SUPPORTED ;
											   
					 case CRYS_RSA_PKCS1_15_MODE01:  DX_VOS_MemSet(&Output_ptr[2],0xFF,PSSize);
											    break;
					 case CRYS_RSA_PKCS1_15_MODE02:

				            Error = CRYS_RND_GenerateVector((DxUint16_t)(K-2),&Output_ptr[2]);
							if(Error != CRYS_OK)
							{
								return Error;
							}
							/*The random generation should not generate a zero octet according to the standart*/	
							J = K-2;
					 		for(I=2;I<PSSize+2;I++)
					 		{
					 			if(Output_ptr[I] == 0x00)
					 			{
					 			    /*Take a non zero octet from the end of the vector*/
					 				while(Output_ptr[J] == 0)
					 				{
					 				   J--;
					 				   continue;
					 				}
					 				
					 				/*Change the zero octet*/
									Output_ptr[I] = Output_ptr[J];
									J--;
								}
								if(J == I)
								{	/*This happens in very low probability*/
									/*All the extra random generated octets are zero or used*/
									/*regenerate */
									Error = CRYS_RND_GenerateVector((DxUint16_t)(K-2),&Output_ptr[2]);
									if(Error != CRYS_OK)
									{
										return Error;
									}															
									J = K-2;
									I=1;/*go back to the start of the loop*/
								}
							}
									
					 		break;
					 default:
					 		return CRYS_RSA_PKCS1_15_BLOCK_TYPE_NOT_SUPPORTED;
			 
	}

					
	/* add 00 */
	Output_ptr[K-MSize-1]=0x00;
	
	/* copy the data */
	DX_VOS_FastMemCpy(&Output_ptr[K-MSize],M_ptr,MSize);
	
	return CRYS_OK;
							 	
		
	
}

#endif /*defined(_INTERNAL_CRYS_NO_RSA_SCHEME_15_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CRYS_NO_RSA_VERIFY_SUPPORT)*/

#ifndef _INTERNAL_CRYS_NO_RSA_SCHEME_15_SUPPORT
/******************BER PARSER UTILS functions************************/

#ifndef DX_OEM_FW


CRYSError_t CRYS_RSA_BER_BerEncoder( DxUint8_t 		**Buffer_ptr,
			   		    DxUint8_t         *BufferSize,
			   		    DxUint16_t        BufferTotalSize,
                        BerParserObj_t 	*BerParserObj_ptr)
{

 /*
    The structure of the buffer that need to be formated

    step 1  : [30  = Object TAG]
    step 2  : [ Total size  ]
    step 2.1: [30  = Object TAG]
    step 2.2: [ The Size of the Digest Algo info upto 00]
    step 3  : [06 =  Object TAG]
    step 4  : [The size of Digest Alg ]
    step 5  : [The Digest Alg ident]
    step 6  : [05] [00] | [04]
    step 7  : [The Size of Message Digest]
    step 8  : [ The Message digest ]
 */

 static const DxUint8_t AlgorithmID_MD5_V2[] ={0x2A,0x86,0x48,0x86,0xf7,0x0d,0x02,0x05};
 static const DxUint8_t AlgorithmID_SHA[]    ={0x2b,0x0e,0x03,0x02,0x1a};
 static const DxUint8_t AlgorithmID_SHA224[] ={0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x04};
 static const DxUint8_t AlgorithmID_SHA256[] ={0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01};
 static const DxUint8_t AlgorithmID_SHA384[] ={0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02};
 static const DxUint8_t AlgorithmID_SHA512[] ={0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03};
 

 DxUint8_t TotalSize=0;
 DxUint8_t  *BufferInt_ptr;
 DxUint8_t  MDSize  = (DxUint8_t)(BerParserObj_ptr->MessageDigestSize);

 #ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED
 DxUint8_t TotalSizeTmp;
 #endif

 BufferInt_ptr=*Buffer_ptr+BufferTotalSize;

 PLAT_LOG_DEV_PRINT(( 25 ," \nStart Printing from CRYS_RSA_BER_BerEncoder\n" ));

 PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe input buffer pointer (*Buffer_ptr)  =  " ,(DxUint8_t *)*Buffer_ptr,*BufferSize));            
 PLAT_LOG_DEV_PRINT(( 25 ," \nThe input buffer pointer after copy (BufferInt_ptr)= %p \n", BufferInt_ptr)); 
 PLAT_LOG_DEV_PRINT(( 25 ," \nTotal size = %d \n", BufferTotalSize));  
 

 /* step 8 : Set the MD at the end of the buffer */
 BufferInt_ptr-= MDSize;
 DX_VOS_FastMemCpy(BufferInt_ptr, BerParserObj_ptr->MessageDigest_ptr, MDSize);

 PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 8 - adding MD =   " ,(DxUint8_t *)BufferInt_ptr,MDSize));             

 /******************************************************************
  * Need to check the type of the size  ( at this stage its 1 octet)
  ******************************************************************/

 /* step 7 */
 BufferInt_ptr-=1;
 *BufferInt_ptr=MDSize;
 
 #ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
 TotalSizeTmp = CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES +1 ;
 /* TotalSizeTmp+=1;*/
 PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 7 - adding MD size " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));              
 #endif
 
  /* step 6 */
 BufferInt_ptr-= 3;
 *(BufferInt_ptr+0)=0x05;
 *(BufferInt_ptr+1)=0x00;
 *(BufferInt_ptr+2)=0x04;

 
 #ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
 TotalSizeTmp+=3;
 PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 6 - adding 050004 =  " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));               
 #endif


  /*step 5 + 4  */
 switch(BerParserObj_ptr->DigestAlg){
  case CRYS_HASH_MD5_mode:
							PLAT_LOG_DEV_PRINT(( 25 ," \nCRYS_ALG_HASH_MD_5 size = %d pointer before %p \n",sizeof(AlgorithmID_MD5_V2),BufferInt_ptr));
  							BufferInt_ptr-=sizeof(AlgorithmID_MD5_V2);

							PLAT_LOG_DEV_PRINT(( 25 ," \n BufferInt_ptr after = %p \n",BufferInt_ptr));
                            DX_VOS_FastMemCpy(BufferInt_ptr,AlgorithmID_MD5_V2,sizeof(AlgorithmID_MD5_V2));

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
                                TotalSizeTmp+=sizeof(AlgorithmID_MD5_V2);
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 5 - adding digest = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                               
                            #endif							

                            BufferInt_ptr-=1;
                            /* step 4 */
 							*BufferInt_ptr=sizeof(AlgorithmID_MD5_V2);

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
 							    TotalSizeTmp+=1;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 4 - adding digest size = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));
 							#endif
 							TotalSize+=sizeof(AlgorithmID_MD5_V2);
                            break;
                            
  case CRYS_HASH_SHA1_mode:
							PLAT_LOG_DEV_PRINT(( 25 ," \n CRYS_ALG_HASH_SHA_1 size = %d pointer before %p \n",sizeof(AlgorithmID_SHA),BufferInt_ptr));  							
									  
  							BufferInt_ptr-=sizeof(AlgorithmID_SHA);

							PLAT_LOG_DEV_PRINT(( 25 ," \n  BufferInt_ptr after = %p \n",BufferInt_ptr));  							

                            DX_VOS_FastMemCpy(BufferInt_ptr,AlgorithmID_SHA,sizeof(AlgorithmID_SHA));

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
                                TotalSizeTmp+=TotalSizeTmp;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 5 - adding digest = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                
 							#endif


                            BufferInt_ptr-=1;
                            /* step 4 */
 							*BufferInt_ptr=sizeof(AlgorithmID_SHA);

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
 							    TotalSizeTmp+=1;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 4 - adding digest size = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                 							    
 							#endif

                            TotalSize+=sizeof(AlgorithmID_SHA);
							break;
  
  case CRYS_HASH_SHA224_mode:
							PLAT_LOG_DEV_PRINT(( 25 ," \n CRYS_ALG_HASH_SHA_224 size = %d pointer before %p \n",sizeof(AlgorithmID_SHA224),BufferInt_ptr));  							
									  
  							BufferInt_ptr-=sizeof(AlgorithmID_SHA224);

							PLAT_LOG_DEV_PRINT(( 25 ," \n  BufferInt_ptr after = %p \n",BufferInt_ptr));  							

                            DX_VOS_FastMemCpy(BufferInt_ptr,AlgorithmID_SHA224,sizeof(AlgorithmID_SHA224));

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
                                TotalSizeTmp+=TotalSizeTmp;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 5 - adding digest = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                
 							#endif


                            BufferInt_ptr-=1;
                            /* step 4 */
 							*BufferInt_ptr=sizeof(AlgorithmID_SHA224);

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
 							    TotalSizeTmp+=1;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 4 - adding digest size = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                 							    
 							#endif

                            TotalSize+=sizeof(AlgorithmID_SHA224);
							break;
							
							
  case CRYS_HASH_SHA256_mode:
							PLAT_LOG_DEV_PRINT(( 25 ," \n CRYS_ALG_HASH_SHA_256 size = %d pointer before %p \n",sizeof(AlgorithmID_SHA256),BufferInt_ptr));  							
									  
  							BufferInt_ptr-=sizeof(AlgorithmID_SHA256);

							PLAT_LOG_DEV_PRINT(( 25 ," \n  BufferInt_ptr after = %p \n",BufferInt_ptr));  							

                            DX_VOS_FastMemCpy(BufferInt_ptr,AlgorithmID_SHA256,sizeof(AlgorithmID_SHA256));

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
                                TotalSizeTmp+=TotalSizeTmp;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 5 - adding digest = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                
 							#endif


                            BufferInt_ptr-=1;
                            /* step 4 */
 							*BufferInt_ptr=sizeof(AlgorithmID_SHA256);

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
 							    TotalSizeTmp+=1;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 4 - adding digest size = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                 							    
 							#endif

                            TotalSize+=sizeof(AlgorithmID_SHA256);
							break;
							
  case CRYS_HASH_SHA384_mode:
							PLAT_LOG_DEV_PRINT(( 25 ," \n CRYS_ALG_HASH_SHA_384 size = %d pointer before %p \n",sizeof(AlgorithmID_SHA384),BufferInt_ptr));  							
									  
  							BufferInt_ptr-=sizeof(AlgorithmID_SHA384);

							PLAT_LOG_DEV_PRINT(( 25 ," \n  BufferInt_ptr after = %p \n",BufferInt_ptr));  							

                            DX_VOS_FastMemCpy(BufferInt_ptr,AlgorithmID_SHA384,sizeof(AlgorithmID_SHA384));

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
                                TotalSizeTmp+=TotalSizeTmp;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 5 - adding digest = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                
 							#endif


                            BufferInt_ptr-=1;
                            /* step 4 */
 							*BufferInt_ptr=sizeof(AlgorithmID_SHA384);

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
 							    TotalSizeTmp+=1;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 4 - adding digest size = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                 							    
 							#endif

                            TotalSize+=sizeof(AlgorithmID_SHA384);
							break;
  case CRYS_HASH_SHA512_mode:
							PLAT_LOG_DEV_PRINT(( 25 ," \n CRYS_ALG_HASH_SHA_512 size = %d pointer before %p \n",sizeof(AlgorithmID_SHA512),BufferInt_ptr));  							
									  
  							BufferInt_ptr-=sizeof(AlgorithmID_SHA512);

							PLAT_LOG_DEV_PRINT(( 25 ," \n  BufferInt_ptr after = %p \n",BufferInt_ptr));  							

                            DX_VOS_FastMemCpy(BufferInt_ptr,AlgorithmID_SHA512,sizeof(AlgorithmID_SHA512));

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
                                TotalSizeTmp+=TotalSizeTmp;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 5 - adding digest = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                
 							#endif


                            BufferInt_ptr-=1;
                            /* step 4 */
 							*BufferInt_ptr=sizeof(AlgorithmID_SHA512);

							#ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
 							    TotalSizeTmp+=1;
								PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 4 - adding digest size = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));                                 							    
 							#endif

                            TotalSize+=sizeof(AlgorithmID_SHA512);
							break;
							
  default :

							PLAT_LOG_DEV_PRINT(( 25 ," \n  Error in BerParserObj_ptr->HashType = %d ",BerParserObj_ptr->DigestAlg));  							       

       return  CRYS_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
 }

 /* step 3 */
 BufferInt_ptr-=1;
 *BufferInt_ptr=0x06;
 
 #ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED
    TotalSizeTmp+=1;
	PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 3 - adding 06 = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));
 #endif

 /* step 2.2 */
  BufferInt_ptr-=1;
  *BufferInt_ptr = (DxUint8_t)(TotalSize + 4); /* total size stores the step5 size */

 /* step 2.1 */
  BufferInt_ptr-=1;
  *BufferInt_ptr =0x30;

 /* step 2 */

 BufferInt_ptr-=1 ;
 *BufferInt_ptr= (DxUint8_t)(4  + TotalSize+ 4 + MDSize);

 #ifdef CRYS_LOG_DEV_MAX_LEVEL_ENABLED 
    TotalSizeTmp=*BufferInt_ptr;
	PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after step 2 - adding total size of buffer = " ,(DxUint8_t *)BufferInt_ptr,TotalSizeTmp));    
 #endif

 *BufferSize=(DxUint8_t)(*BufferInt_ptr+2); /* The +1 is for the octet of the suze and
                                      +1 is for the octet 30 that will be added in the next step */


 /* step 1 */
 BufferInt_ptr-=1;
 *BufferInt_ptr=0x30  ;


 /**********************************
  * At this point BufferInt_ptr points to the start of the data to send to
  **********************************/

  *Buffer_ptr=BufferInt_ptr;

  PLAT_LOG_DEV_PRINT(( 25 ," \n  The input buffer pointer            (*Buffer_ptr)  = %p",*Buffer_ptr));  							       
  PLAT_LOG_DEV_PRINT(( 25 ," \n  The input buffer pointer            (**Buffer_ptr )= %p",**Buffer_ptr));  							       
  PLAT_LOG_DEV_PRINT(( 25 ," \nThe input buffer pointer after copy (BufferInt_ptr)= %p",BufferInt_ptr));  							       
  PLAT_LOG_DEV_PRINT(( 25 ," \nTotal size = %d",*BufferSize));  							       
  PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe Buffer after formating = " ,(DxUint8_t *)BufferInt_ptr,CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES));    

  return CRYS_RSA_BER_ENCODING_OK ;
}

#endif
/* -------------------------------------------------------------
 *  Function Name: BerParser
 *  Date: 24/09/2001
 *  Author: Evgeny Belenky
 *
 *      Inpruts:
 *      Outputs:
 *
 *  change tested certificate by:
 *
 *  Local Data Structures:
 *
 *  Description and Design info:
 *      purpose:This function returns the pointer to the signature and
 *               the signature length.
 *
 *      Assumptions,
 *      Constrains,
 *      Limitations,
 *      Side effects
 *
 *  Algorithm:
 *
 *  List Of Units Used:
 *
 *  Trace info to SRS, IDD and Detailed SDD:
 *
 *  History:
 *  Date:       Description:
 *  
 *  --------------------------------------
 *
 * ----------------------------------------------------------- */

CRYSError_t  CRYS_RSA_BER_BerParser(DxUint8_t 		*Data_ptr,
			   				        DxUint16_t 		DataSize,
                                    BerParserObj_t 	*BerParserObj_ptr)
{

    /*** Variables definitions and initializations ***/
    
    static const DxUint8_t AlgorithmID_MD5[]       ={0x2A,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x04};
    static const DxUint8_t AlgorithmID_MD5_V2[]    ={0x2A,0x86,0x48,0x86,0xf7,0x0d,0x02,0x05};
    static const DxUint8_t AlgorithmID_SHA[]       ={0x2b,0x0e,0x03,0x02,0x1a};
    static const DxUint8_t AlgorithmID_SHA224[]    ={0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x04};
	static const DxUint8_t AlgorithmID_SHA256[]    ={0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01};
	static const DxUint8_t AlgorithmID_SHA384[]    ={0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02};
	static const DxUint8_t AlgorithmID_SHA512[]    ={0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03};

    /* Temporary variable for size*/
    DxUint16_t	TempLength=0;
    /* Calculated size of passed data and size of Message Digest (HASH) */
    DxUint16_t	CalcSize = 0, MDSize = 0;
    
    /* Certificate pointer */
    DxUint8_t*	Cert_ptr = Data_ptr;
    
    /* to avoid compilers warnings */
    DataSize = DataSize;

    /***** FUNCTION LOGIC ****/

    /* skip on the DigestInfo sequence tag */
    Cert_ptr++; /* skip on SEQUENCE tag */
    CRT_API_GetLengthField(&Cert_ptr, &TempLength);

    /* skip on the AlgorithmIdentifier sequence tag */
    Cert_ptr++; /* skip on SEQUENCE tag */
    CRT_API_GetLengthField(&Cert_ptr, &TempLength);

    /* skip on the OBJECT IDENTIFIER length field */
    if(*Cert_ptr++ != ASN1_OBJECT_IDENTIFIER)/* skip on OBJECT IDENTIFIER tag */
		return (CRYS_RSA_ERROR_BER_PARSING);
    
    CRT_API_GetLengthField(&Cert_ptr, &TempLength);
    
    if(!DX_VOS_MemCmp(Cert_ptr,AlgorithmID_MD5,TempLength))         /* md5 hash algorithm */
    {
        BerParserObj_ptr->DigestAlg = CRYS_HASH_MD5_mode;
       
        MDSize = CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES;
    }

    else if(!DX_VOS_MemCmp(Cert_ptr,AlgorithmID_MD5_V2,TempLength)) /* md5_v2 hash algorithm */
  	{
    	BerParserObj_ptr->DigestAlg =CRYS_HASH_MD5_mode;
    	
        MDSize = CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES;
  	}
  
    else if(!DX_VOS_MemCmp(Cert_ptr,AlgorithmID_SHA,TempLength))    /* sha1 hash algorithm */
	{
        BerParserObj_ptr->DigestAlg = CRYS_HASH_SHA1_mode;
        
        MDSize = CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES;
    }
    
    else if(!DX_VOS_MemCmp(Cert_ptr,AlgorithmID_SHA224,TempLength)) /* sha224 hash algorithm */
	{
		BerParserObj_ptr->DigestAlg = CRYS_HASH_SHA224_mode;
		
        MDSize = CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES;
	    
	}

	else if(!DX_VOS_MemCmp(Cert_ptr,AlgorithmID_SHA256,TempLength)) /* sha256 hash algorithm */
	{
		BerParserObj_ptr->DigestAlg = CRYS_HASH_SHA256_mode;
		
        MDSize = CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES;
	}

	else if(!DX_VOS_MemCmp(Cert_ptr,AlgorithmID_SHA384,TempLength)) /* sha384 hash algorithm */
	{
		BerParserObj_ptr->DigestAlg = CRYS_HASH_SHA384_mode;
		
	    MDSize = CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES;	    
	}
	
	else if(!DX_VOS_MemCmp(Cert_ptr,AlgorithmID_SHA512,TempLength)) /* sha512 hash algorithm */
	{
		BerParserObj_ptr->DigestAlg = CRYS_HASH_SHA512_mode;
		
		MDSize = CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES;	
	}
	
	else

		return (CRYS_RSA_ERROR_BER_PARSING);/* unknown algorithm */


    /* move the Cert_ptr to next position */
    Cert_ptr += TempLength;

    /* skip on the DX_NULL tag */
    if(*Cert_ptr ++ != ASN1_NULL)/* skip on DX_NULL tag */
		return (CRYS_RSA_ERROR_BER_PARSING);
    
    CRT_API_GetLengthField(&Cert_ptr, &TempLength);

    /* skip on the BIT STRING length field + 0 (unused) byte */
    if(*Cert_ptr ++ != ASN1_OCTET_STRING) /* skip on BIT STRING tag */
		return (CRYS_RSA_ERROR_BER_PARSING);
    
    CRT_API_GetLengthField(&Cert_ptr, &TempLength);
    
    /* Countermeasure against CVE-2006-4339 (candidate) when using an RSA key with exponent 3,
       which allows remote attackers to forge a PKCS #1 v1.5 signature that is signed by the RSA */
            
    /* Calculate data size = the actual size we progress untill now (should be equal to
      the ASN.1 size)  + HASH size in bytes */
    CalcSize = (DxUint16_t)(Cert_ptr - Data_ptr) + MDSize;
      
    /* if there is more data (passed garbage), than return an error */
    if (DataSize > CalcSize)
       
         return CRYS_RSA_ERROR_BER_PARSING;


    /* Write a Message digest from the certificate to the output parameter */
    /*
       BerParserObj_ptr->MessageDigest_ptr = Cert_ptr;
    */
    switch(BerParserObj_ptr->DigestAlg)
    {
    	case CRYS_HASH_MD5_mode:
    		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD before copy = " ,(DxUint8_t *)Cert_ptr,CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES));        
		    /* MD5 size */
		    DX_VOS_FastMemCpy((DxUint8_t*)BerParserObj_ptr->MessageDigest_ptr,(DxUint8_t*)Cert_ptr,CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES);
			PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD After copy = " ,(DxUint8_t *)BerParserObj_ptr->MessageDigest_ptr,CRYS_HASH_MD5_DIGEST_SIZE_IN_BYTES));
		    break;
		    	
    	case CRYS_HASH_SHA1_mode:
    	
    		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD before copy = " ,(DxUint8_t *)Cert_ptr,CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES));               
	        /* SHA1 size */
	        DX_VOS_FastMemCpy((DxUint8_t*)BerParserObj_ptr->MessageDigest_ptr,(DxUint8_t*)Cert_ptr,CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES);
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD After copy = " ,(DxUint8_t *)BerParserObj_ptr->MessageDigest_ptr,CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES));
	    	break;
	    	
	    case CRYS_HASH_SHA224_mode:
    	
    		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD before copy = " ,(DxUint8_t *)Cert_ptr,CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES));               
	        /* SHA224 size */
	        DX_VOS_FastMemCpy((DxUint8_t*)BerParserObj_ptr->MessageDigest_ptr,(DxUint8_t*)Cert_ptr,CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES);
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD After copy = " ,(DxUint8_t *)BerParserObj_ptr->MessageDigest_ptr,CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES));
	    	break;
	    	
    	case CRYS_HASH_SHA256_mode:
    	
    		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD before copy = " ,(DxUint8_t *)Cert_ptr,CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES));               
	        /* SHA256 size */
	        DX_VOS_FastMemCpy((DxUint8_t*)BerParserObj_ptr->MessageDigest_ptr,(DxUint8_t*)Cert_ptr,CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES);
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD After copy = " ,(DxUint8_t *)BerParserObj_ptr->MessageDigest_ptr,CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES));
	    	break;
	    	
    	case CRYS_HASH_SHA384_mode:
    	
    		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD before copy = " ,(DxUint8_t *)Cert_ptr,CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES));               
	        /* SHA384 size */
	        DX_VOS_FastMemCpy((DxUint8_t*)BerParserObj_ptr->MessageDigest_ptr,(DxUint8_t*)Cert_ptr,CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES);
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD After copy = " ,(DxUint8_t *)BerParserObj_ptr->MessageDigest_ptr,CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES));
	    	break;
	    	
    	case CRYS_HASH_SHA512_mode:
    			
    		PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD before copy = " ,(DxUint8_t *)Cert_ptr,CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES));               
	        /* SHA512 size */
	        DX_VOS_FastMemCpy((DxUint8_t*)BerParserObj_ptr->MessageDigest_ptr,(DxUint8_t*)Cert_ptr,CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES);
		    PLAT_LOG_DEV_PRINT_DisplayBuffer(( 25 , (char *)" \nThe MD After copy = " ,(DxUint8_t *)BerParserObj_ptr->MessageDigest_ptr,CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES));
	    	break;
    	
    	default:
    		break;
    }
      
    return (CRYS_OK);
}


/*******************************************************************************************/
/* This function returns length of the following fields and move pointer to the next field */
/*******************************************************************************************/

void CRT_API_GetLengthField(DxUint8_t** PtrToLengthField, DxUint16_t* LengthFieldValue)
{
    DxUint8_t    NumberOfBytes;
    DxUint16_t   Length= 0;
    DxUint8_t    Index;

    if (**PtrToLengthField & TEST_MSB_BIT) /* if the length > 127 */
    {
        /* How many bytes are in  the LengthField? */
        NumberOfBytes =(DxUint8_t)(**PtrToLengthField & 0x7F);
        /* calculate length */
        for (Index = 0;Index < NumberOfBytes; Index++)
        {
            (*PtrToLengthField)++;/* go to the next byte of the length field */
            Length <<= 8;
            Length += (DxUint16_t)((**PtrToLengthField)); /*concatenate length */
        }/* extract length from the certificate */

        *LengthFieldValue = Length;/* copy concatenated length to the output */
    }
    else /* if length <=127 */
    {
        *LengthFieldValue = **PtrToLengthField; /*length consists of only 1 byte
                                                so copy it to the output */
    }
    (*PtrToLengthField)++;/* go to the data */
}

#endif /*CRYS_NO_HASH_SUPPORT*/
#endif /*CRYS_NO_PKI_SUPPORT*/                                      

#endif /*#ifndef CRYS_NO_RSA_SCHEME_15_SUPPORT */

/******************************************************************************************/
#if defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT)                                     

/* just to have a declearion so the 'C' file passes compilation */
void CRTS_RSA_VER15_UTIL_foo(void) {}

#endif /* defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) */

    
