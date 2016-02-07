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
  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Tue Feb 01 17:15:04 2005
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version CRYS_DH.c#1:csrc:8
   *  \author ohads
   */



/************* Include Files ****************/

#include "dx_vos_mem.h"
#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif

#include "crys_common.h"
#include "crys_common_math.h"
#include "crys_rnd.h"
#include "crys_rnd_local.h"
#include "crys_ccm.h"
#include "crys_dh_error.h"
#include "crys_dh.h"
#include "crys_dh_kg.h"
#include "llf_pki_rsa.h"

#include "debug.h"//2012/11/17
/************************ Defines *******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/************************ Enums *********************************/

/************************ macros ********************************/

/* this macro is required to remove compilers warnings if the HASH or PKI is not supported */

#if defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) || defined(CRYS_NO_DH_SUPPORT)
#define RETURN_IF_DH_UNSUPPORTED( a , b , c , d , e , f , g , h , i , j , k , l , a1 , b1 , c1 , d1 , e1 , f1 , g1 , h1 , i1 , j1) \
  (a)=0;(b)=0;(c)=0;(d)=0;(e)=0;(f)=0;(g)=0;(h)=0;(i)=0;(j)=0;(k)=0;(l)=0; \
  (a1)=0;(b1)=0;(c1)=0;(d1)=0;(e1)=0;(f1)=0;(g1)=0;(h1)=0;(i1)=0;(j1)=0; \
  (a)=(a);(b)=(b);(c)=(c);(d)=(d);(e)=(e);(f)=(f);(g)=(g);(h)=(h);(i)=(i);(j)=(j);(k)=(k);(l)=(l); \
  (a1)=(a1);(b1)=(b1);(c1)=(c1);(d1)=(d1);(e1)=(e1);(f1)=(f1);(g1)=(g1);(h1)=(h1);(i1)=(i1);(j1)=(j1); \
  return CRYS_DH_IS_NOT_SUPPORTED
#else  /* !CRYS_NO_HASH_SUPPORT && ! CRYS_NO_PKI_SUPPORT && !CRYS_NO_DH_SUPPORT */
#define RETURN_IF_DH_UNSUPPORTED( a , b , c , d , e , f , g , h , i , j , k , l , a1 , b1 , c1 , d1 , e1 , f1 , g1 , h1 , i1 , j1) 
#endif /* !CRYS_NO_HASH_SUPPORT && ! CRYS_NO_PKI_SUPPORT */

/************************ global data ***********************************/
#ifdef CRYS_DH_SET_PRIV_KEY_NON_RANDOM
DxUint8_t CRYS_DH_DEBUG_PrivKey[4*CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
#endif


/************************ Public Functions ******************************/


/*******************************************************************************************/

/**
 * @brief _DX_DH_GeneratePubPrv has 2 functionalities:

 	   1. Randomly generates the Client private key Prv.
           2. Compute the Client public key which is: 
		ClientPub = Generator ^Prv mod Prime.
 		
 	Note: All buffers arguments are represented in Big-Endian
 	
  @param[in] Generator_ptr 	- Pointer to the Generator octet string
  @param[in] GeneratorSize 	- Size of the Generator String (in bytes): 
				  0 < GeneratorSize <= PrimeSize.
  @param[in] Prime_ptr 		- Pointer to the Prime octet string P.
  @param[in] PrimeSize 		- Size of the Prime string (in bytes): 
			0 < PrimeSize <= max. valid DH key size.
  @param[in] L 	- Exact length of Private key in bits (relevant only for 
		  PKCS#3), defined by central authority (L <= max. PrimeSize).  
        	- If L != 0, then L must met to requirements:  1 <= L and
        	  2^(L-1) <= P, Note: in our implementation required L >= 3. In
        	  this case force the private key size to be 2^(L-1) <= Prv <
        	  2^L.
		- If L = 0, then: 0 < Prv < P-1.
  @param[in] Q_ptr		- Pointer to the order Q of generator relevant 
        			  only for ANSI X9.42): 1<= Prv <= q-1 or 1 <
        			  Prv < q-1.
  @param[in] QSize		- Size of the Q string in bytes. Relevant only 
				  for ANSI X9.42.
  @param[in] DH_mode		- Enumerator, declaring whether the mode is 
				  PKCS#3 or ANSI X9.42.
  @param[in] tmpPubKey_ptr	- A pointer to the public key structure. Used   
        			  for the Exp operation. The structure doesn't
        			  need to be initialized.
  @param[in] tmpPrimeData_ptr 	- A pointer to a structure containing internal 
        			  buffers, the structure doesn't need to be
        			  initialized.
  @param[out] ClientPrvKey_ptr 	- Pointer to the buffer for output the Private  
        			  key octet string (Prv). In PKCS#3 this buffer
        			  should be at least the following size:
        			- if L is provided the size of buffer:
        			  bufSizeBytes*8 >= L.
        			- if L is DX_NULL, then size of buffer >=
        			  PrimeSize. In X9.42 size of buffer >= QSize.
  @param[in/out] ClientPrvKeySize_ptr - The user should provide the size of the 
					buffer indicated by ClientPrvKey_ptr.
        				The function returns the actual size in
        				bytes of the Private key.
  @param[out] ClientPub_ptr 	- Pointer to the Public key octet 
        			  string. This Buffer should be at least
        			  PrimeSize bytes.
  @param[in/out] ClientPubSize_ptr    - The user should provide the size of 
					the buffer indicated by ClientPub_ptr.The
        				function returns the actual size in
        				bytes of the generated client public
        				key.
 
  @return CRYSError_t - On success CRYS_OK is returned, on failure an ERROR as 
			defined CRYS_DH_error.h:
			CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;      
			CRYS_DH_INVALID_ARGUMENT_SIZE_ERROR;
			CRYS_DH_INVALID_ARGUMENT_OPERATION_MODE_ERROR;   						
			CRYS_DH_ARGUMENT_GENERATOR_SMALLER_THAN_ZERO_ERROR;
			CRYS_DH_ARGUMENT_PRIME_SMALLER_THAN_GENERATOR_ERROR;						
			CRYS_DH_INVALID_L_ARGUMENT_ERROR;
			CRYS_DH_ARGUMENT_PRV_SIZE_ERROR;						
 */
CEXPORT_C CRYSError_t _DX_DH_GeneratePubPrv(
					DxUint8_t *Generator_ptr,              /*generator*/
					DxUint16_t GeneratorSize,
					DxUint8_t *Prime_ptr,                  /*modulus*/
					DxUint16_t PrimeSize,
					DxUint16_t L,         /*Exact length of Private key in bits*/           
					DxUint8_t *Q_ptr,                      /*order*/ 
					DxUint16_t QSize,
					CRYS_DH_OpMode_t DH_mode,
					CRYS_DHUserPubKey_t *tmpPubKey_ptr,    /*temp buff*/
					CRYS_DHPrimeData_t  *tmpPrimeData_ptr, /*temp buff*/
					DxUint8_t *ClientPrvKey_ptr,           /*out*/
					DxUint16_t *ClientPrvKeySize_ptr,      /*in/out*/
					DxUint8_t *ClientPub1_ptr,             /*out*/
					DxUint16_t *ClientPubSize_ptr)         /*in/out*/

{
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* temporary byte shift masks */
	DxUint8_t tmpByte, mask, mask1, shift;
	
	/* the vector 2^(L-1) size*/
	DxUint16_t tmpSize;
	
	/* the comparing value, returned from the vectors compare */
	CRYS_COMMON_CmpCounter_t comp;
	

	/* FUNCTION LOGIC */
	
	/* ............... local initializations .............................. */
	/* -------------------------------------------------------------------- */
	
	/* initializing the Error to O.K */
	Error = CRYS_OK;
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_DH_UNSUPPORTED( Generator_ptr, GeneratorSize, Prime_ptr, 
			     PrimeSize, L, Q_ptr, QSize , 
			     Error/*DH_mode*/, tmpPubKey_ptr, tmpPrimeData_ptr, ClientPrvKey_ptr ,
			     ClientPrvKeySize_ptr, ClientPub1_ptr, ClientPubSize_ptr,
			     tmpByte, tmpSize, comp, mask, mask1, shift, Error, Error); 
			      
	#ifndef CRYS_NO_HASH_SUPPORT                                      
	#ifndef CRYS_NO_PKI_SUPPORT
	#ifndef CRYS_NO_DH_SUPPORT
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */
	
	/* if an argument pointer is DX_NULL return an error */
	if( Generator_ptr == DX_NULL || Prime_ptr == DX_NULL || 
	ClientPrvKey_ptr == DX_NULL || ClientPub1_ptr == DX_NULL ||
	ClientPrvKeySize_ptr == DX_NULL || ClientPubSize_ptr == DX_NULL ||
	tmpPubKey_ptr == DX_NULL || tmpPrimeData_ptr == DX_NULL)
	   
	return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;      
	
	/* check prime size */
	if( PrimeSize > CRYS_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES )
	   return CRYS_DH_INVALID_ARGUMENT_SIZE_ERROR;
	/*If an argument buffer size is zero return an error*/
	if( GeneratorSize == 0 || PrimeSize == 0 || 
	   *ClientPrvKeySize_ptr == 0 || *ClientPubSize_ptr < PrimeSize )   
		return CRYS_DH_INVALID_ARGUMENT_SIZE_ERROR;
	
	if(DH_mode > CRYS_DH_NumOfModes)
		return CRYS_DH_INVALID_ARGUMENT_OPERATION_MODE_ERROR;
	
	if(DH_mode == CRYS_DH_ANSI_X942_mode)
	{
		if(Q_ptr == DX_NULL)
			return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;
		if(QSize == 0)       
			return CRYS_DH_INVALID_ARGUMENT_SIZE_ERROR;
	}

	/*Compare the generator with zero: requested g > 0 */
	tmpByte = 0;
	comp = CRYS_COMMON_CmpMsbUnsignedCounters( Generator_ptr,
					      GeneratorSize, 
					      &tmpByte, 1);
	
	if(comp != CRYS_COMMON_CmpCounter1GraterThenCounter2)
		return CRYS_DH_ARGUMENT_GENERATOR_SMALLER_THAN_ZERO_ERROR;
	PRINT_INFO("--->after if(comp != CRYS_COMMON_CmpCounter1GraterThenCounter2)    1\n");
	/*Compare the generator and the Prime vectors: requested that g < P */
	comp = CRYS_COMMON_CmpMsbUnsignedCounters( Generator_ptr,
					      GeneratorSize, 
					      Prime_ptr,
					      PrimeSize );
	
	if(comp != CRYS_COMMON_CmpCounter2GraterThenCounter1)
		return CRYS_DH_ARGUMENT_PRIME_SMALLER_THAN_GENERATOR_ERROR;
	PRINT_INFO("--->after if(comp != CRYS_COMMON_CmpCounter2GraterThenCounter1)    2\n");
	/*--------------------------------------------------------------------------------*/
	/*         DH public-private keys generation                                      */ 
	/*--------------------------------------------------------------------------------*/
	
	/* temporary set prime modulus into temp buffer in little endianness */
	CRYS_COMMON_ReverseMemcpy( (DxUint8_t*)tmpPrimeData_ptr->DataIn , Prime_ptr , PrimeSize );
	
	/* get actual size of prime in bits */
	tmpSize = (DxUint16_t)CRYS_COMMON_GetBytesCounterEffectiveSizeInBits( 
								 (DxUint8_t*)tmpPrimeData_ptr->DataIn, 
								 PrimeSize );
	
	/* correction of Prime_ptr pointer and Size for removing of not significant zero-bytes */
	if( PrimeSize - (tmpSize + 7) / 8 > 0 )
	{
	   Prime_ptr += PrimeSize - (tmpSize + 7) / 8;
	   PrimeSize = (tmpSize + 7) / 8;
	}

	switch(DH_mode) {
	case CRYS_DH_PKCS3_mode:
		/* ----------------------------------------------------------- *
		PKCS#3:  set x private random value according to following:
			   1) If L = 0: set    0 < x < P-1;
			   2) If L > 0: set  2^(L-1) <= x < 2^L , 
				   where 2^(L-1) <= P.
		----------------------------------------------------------- */
		if(L == 0)
		{    
			/* Option 1: L is not provided - check the minimum size of the private key buffer */
			if(*ClientPrvKeySize_ptr < PrimeSize)
				return CRYS_DH_ARGUMENT_PRV_SIZE_ERROR;
			
			/* random generation in range:  0 < x < P-1  (in little endian */
			Error = CRYS_RND_GenerateVectorInRange( tmpSize /*rndSizeInBits*/, 
							       (DxUint8_t*)tmpPrimeData_ptr->DataIn/*maxVect*/, 
							       (DxUint8_t*)tmpPrimeData_ptr->DataOut/*out*/ );
			if(Error != CRYS_OK)
				goto End;
			
			/* reverse privKey to big endianness */
			CRYS_COMMON_ReverseMemcpy( ClientPrvKey_ptr , (DxUint8_t*)tmpPrimeData_ptr->DataOut , PrimeSize );
			
			/* private key size in bytes */
			*ClientPrvKeySize_ptr = PrimeSize;
		}
		/* Option 2:  L > 0 and bit length of privKey must be exactly L bit */
		else  {  
			/* check L and the minimum size of the private key buffer */
			if( L > tmpSize || L < CRYS_RSA_MIN_PUB_EXP_VALUE ) {
				Error = CRYS_DH_INVALID_L_ARGUMENT_ERROR;
				goto End;
			}
			if((*ClientPrvKeySize_ptr)*8 < L) {
				Error = CRYS_DH_ARGUMENT_PRV_SIZE_ERROR;
				goto End;
			}
			
			/* actual private key size in bytes and shift value */
			*ClientPrvKeySize_ptr = (L+7)/8;
			shift = ((8 - (L & 7)) & 7);
			
			/* if L = modulus size, then generate random x with exact bit-size = L  
			   and value in range:  2^(L-1) < x < P */
			if( L == tmpSize ) {
				mask   = 0x7F >> shift;
				mask1  = 0x80 >> shift;
			
				/* set temporary MSBit of modulus = 0 for generation random in range without MSbit */
				((DxUint8_t*)tmpPrimeData_ptr->DataIn)[*ClientPrvKeySize_ptr - 1] &= mask;
				
				/* generate random in range */
				Error = CRYS_RND_GenerateVectorInRange( 
							tmpSize /*rndSizeInBits*/, 
							(DxUint8_t*)tmpPrimeData_ptr->DataIn/*maxVect*/, 
							(DxUint8_t*)tmpPrimeData_ptr->DataOut/*out*/ );
			
				if(Error != CRYS_OK)
					goto End;
			
				/* set MSBit of random to 1 */
				((DxUint8_t*)tmpPrimeData_ptr->DataIn)[*ClientPrvKeySize_ptr - 1] |= mask1;
			
				/* reverse privKey to big endianness */
				CRYS_COMMON_ReverseMemcpy( ClientPrvKey_ptr , (DxUint8_t*)tmpPrimeData_ptr->DataOut , *ClientPrvKeySize_ptr );
			}
			/* if L < modulus size, then generate random x of size L bits */ 
			else {
				/* random generation */	
				Error = CRYS_RND_GenerateVector( *ClientPrvKeySize_ptr, ClientPrvKey_ptr );
				
				if(Error != CRYS_OK)
					goto End;
				
				/* set two appropriate high bits of privKey to 00..1 to met the requirement 2^(L-1) <= x < 2^L */
				if( (L & 7) > 0 )
				{
					mask  = 0xFF >> shift;
					mask1 = 0x80 >> shift;
					ClientPrvKey_ptr[0] = (ClientPrvKey_ptr[0] & mask) | mask1;
				}
				else /*if ( (L & 7) == 0 )*/
				{
					ClientPrvKey_ptr[0] |= 0x80;
				}
			}
		}
							     
		break;
				
	case CRYS_DH_ANSI_X942_mode:   	
		/* ----------------------------------------------------------- *
				      ANSI X9.42:
			 1<= X <= q -1 or    1< X <= q -1
		----------------------------------------------------------- */
		if( *ClientPrvKeySize_ptr < QSize ) {
			Error = CRYS_DH_ARGUMENT_PRV_SIZE_ERROR;
			goto End;
		}
		      
		/* set order Q into temp buffer in little endianness */
		CRYS_COMMON_ReverseMemcpy( (DxUint8_t*)tmpPrimeData_ptr->DataIn , Q_ptr , QSize );
		
		/* get actual size in bits */
		tmpSize = (DxUint16_t)CRYS_COMMON_GetBytesCounterEffectiveSizeInBits( 
					       (DxUint8_t*)tmpPrimeData_ptr->DataIn, 
					       QSize );
		/* private key size in bytes */
		*ClientPrvKeySize_ptr = (tmpSize + 7) / 8;
		
		/* random generation in range:  0 < x < Q-1  (little endianness) */
		Error = CRYS_RND_GenerateVectorInRange( tmpSize /*rndSizeInBits*/, 
							   (DxUint8_t*)tmpPrimeData_ptr->DataIn/*maxVect*/, 
							   (DxUint8_t*)tmpPrimeData_ptr->DataOut/*out*/ );
		if(Error != CRYS_OK)
			goto End;
		
		/* reverse privKey to big endianness */
		CRYS_COMMON_ReverseMemcpy( ClientPrvKey_ptr , (DxUint8_t*)tmpPrimeData_ptr->DataOut , *ClientPrvKeySize_ptr );
		
		break;   				
		
	default: 
		Error = CRYS_DH_INVALID_ARGUMENT_OPERATION_MODE_ERROR;
		goto End;
	
	}
	
	/* ----------------------------------------------------------- */
	/*           Create the public key                             */
	/* ----------------------------------------------------------- */
	
	/* Build the RSA PublKey data structure for the Exp operation, using RSA_Encrypt primitive */    
	Error = CRYS_RSA_Build_PubKey(  
				tmpPubKey_ptr,
				ClientPrvKey_ptr,
				*ClientPrvKeySize_ptr,
				Prime_ptr,
				PrimeSize );  
	/* check error */
	if(Error != CRYS_OK) {
		goto End;
	}

	/*Call the exponent operation to calculate the ClientPub1 = Generator^privKey mod Prime */
	Error = CRYS_RSA_PRIM_Encrypt( 
				tmpPubKey_ptr,
				tmpPrimeData_ptr,
				Generator_ptr,
				GeneratorSize,
				ClientPub1_ptr ); 
	if(Error != CRYS_OK) {
		goto End;
	}
							  
	*ClientPubSize_ptr = PrimeSize;
	
	
End:
	
	/* delete secure sensitive data */
	DX_VOS_MemSetZero( tmpPubKey_ptr, sizeof(CRYS_DHUserPubKey_t) );
	DX_VOS_MemSetZero( tmpPrimeData_ptr, sizeof(CRYS_DHPrimeData_t) );
	
	return Error;  

#endif /*CRYS_NO_DH_SUPPORT*/
#endif /*CRYS_NO_PKI_SUPPORT*/
#endif /*CRYS_NO_HASH_SUPPORT*/


}/* END OF _DX_DH_GeneratePubPrv function */				                    


/*******************************************************************************************/
/**
 * @brief CRYS_DH_GetSecretKey computes the shared secret key in the following computation:
 	               SecretKey = ServerPubKey ^ ClientPrvKey mod Prime.

   Note:
	- All buffers arguments are represented in Big-Endian.
	- The actual size of private key in bits must be not less than 2 and not great, 
	than actual size of Prime (modulus) in bits (in our implementation); 
	
  @param[in] ClientPrvKey_ptr 	- Pointer to the private key octet string. 
  @param[in] ClientPrvKeySize  	- The Private key Size (in bytes).
  @param[in] ServerPubKey_ptr   - Pointer to the Server public key octet string  
  @param[in] ServerPubKeySize   - The Server Public key Size (in bytes)
  @param[in] Prime_ptr 		- Pointer to the Prime octet string.
  @param[in] PrimeSize 		- Size of the Prime string. 
  @param[in] tmprPubKey_ptr	- A pointer to the temp buffer (public key structure), used 
                                  for the Exponentiation function; the struct doesn't need 
                                  to be initialized.
  @param[in] tmpPrimeData_ptr 	- A pointer to temp buffer (structure containing internal buffers);
                                  the structure doesn't need to be initialized.    
  @param[out] SecretKey_ptr 	- Pointer to output buffer for the secret key octet string.
  				This buffer should be at least of PrimeSize bytes.  							  
  @param[in/out] SecretKeySize_ptr - The user should provide the size in bytes of the buffer indicated by SecretKey_ptr.
                                    The function will return the actual size in bytes of the output secret key.

  @return CRYSError_t - On success CRYS_OK is returned, on failure an ERROR as defined CRYS_DH_error.h:
					    CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;      
   					    CRYS_DH_INVALID_ARGUMENT_SIZE_ERROR;
					    CRYS_DH_SECRET_KEY_SIZE_OUTPUT_ERROR;
*/
 CEXPORT_C CRYSError_t CRYS_DH_GetSecretKey(
					DxUint8_t *ClientPrvKey_ptr,
					DxUint16_t ClientPrvKeySize,
					DxUint8_t *ServerPubKey_ptr,
					DxUint16_t ServerPubKeySize,				                    
					DxUint8_t *Prime_ptr,
					DxUint16_t PrimeSize,
					CRYS_DHUserPubKey_t *tmpPubKey_ptr,
					CRYS_DHPrimeData_t  *tmpPrimeData_ptr, 				                    
					DxUint8_t *SecretKey_ptr,
					DxUint16_t *SecretKeySize_ptr)
{
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	CRYS_COMMON_CmpCounter_t cmpResult;
	
	
	/* FUNCTION LOGIC */
	
	/* ............... local initializations .............................. */
	/* -------------------------------------------------------------------- */
	
	/* initializing the Error to O.K */
	Error = CRYS_OK;
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_DH_UNSUPPORTED( ClientPrvKey_ptr , ClientPrvKeySize , ServerPubKey_ptr , 
			     ServerPubKeySize , Prime_ptr , PrimeSize , tmpPubKey_ptr , 
			     tmpPrimeData_ptr , SecretKey_ptr , SecretKeySize_ptr , 
			     cmpResult , Error , Error , Error , Error , Error , 
			     Error , Error , Error , Error , Error , Error); 
			      
	#ifndef CRYS_NO_HASH_SUPPORT                                      
	#ifndef CRYS_NO_PKI_SUPPORT                                      
	#ifndef CRYS_NO_DH_SUPPORT
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */
	
	/* if an argument pointer is DX_NULL return an error */
	if(ClientPrvKey_ptr == DX_NULL || ServerPubKey_ptr == DX_NULL || 
	   Prime_ptr == DX_NULL || tmpPubKey_ptr == DX_NULL ||
	   tmpPrimeData_ptr == DX_NULL || SecretKey_ptr == DX_NULL || SecretKeySize_ptr == DX_NULL)
	   
		return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;      

	/*If an argument buffer size is zero return an error*/
	if(PrimeSize == 0 || PrimeSize > CRYS_DH_MAX_SIZE_OF_KEYING_DATA ||
	   ClientPrvKeySize == 0 || ClientPrvKeySize > PrimeSize ||
	   ServerPubKeySize == 0 || ServerPubKeySize > PrimeSize ||
	   *SecretKeySize_ptr == 0 || *SecretKeySize_ptr > PrimeSize)   
		return CRYS_DH_INVALID_ARGUMENT_SIZE_ERROR;

	/* 1. verifying that the private exponent is less than modulus, else subtract the modulus */
	cmpResult = CRYS_COMMON_CmpMsbUnsignedCounters( ClientPrvKey_ptr, ClientPrvKeySize,
							Prime_ptr, PrimeSize );
	
	if( cmpResult != CRYS_COMMON_CmpCounter2GraterThenCounter1 ) {
		/* subtract modulus prime from private key and set result in temp buffer */
		CRYS_COMMON_SubtractMSBUint8Arrays(ClientPrvKey_ptr, ClientPrvKeySize, Prime_ptr, PrimeSize, 
						   (DxUint8_t*)tmpPrimeData_ptr->DataIn);
	 
		/* build the Data for the Exp operation. 
		  Note: the user private key is set into public key structure */
		Error = CRYS_RSA_Build_PubKey(  
			tmpPubKey_ptr, 
			(DxUint8_t*)tmpPrimeData_ptr->DataIn,
			ClientPrvKeySize,
			Prime_ptr,
			PrimeSize); 
	} 
	else {       
		/* build the Data for the Exp operation */
		Error = CRYS_RSA_Build_PubKey(  
			tmpPubKey_ptr, 
			ClientPrvKey_ptr,
			ClientPrvKeySize,
			Prime_ptr,
			PrimeSize); 
	}
	
	if( Error != CRYS_OK ) 
		goto End;
	
	/* 3. create: Secret_key = Server_public_key ^ Prv mod Prime*/
	Error = CRYS_RSA_PRIM_Encrypt( 
			tmpPubKey_ptr, /* Note: this is the private key */
			tmpPrimeData_ptr,
			ServerPubKey_ptr,
			ServerPubKeySize,
			SecretKey_ptr); 

	if(Error != CRYS_OK) 
		goto End;
	
	*SecretKeySize_ptr = PrimeSize;
	
End:
	/* delete secure sensitive data */
	DX_VOS_MemSetZero(tmpPubKey_ptr, sizeof(CRYS_DHUserPubKey_t));
	DX_VOS_MemSetZero(tmpPrimeData_ptr->DataIn, sizeof(tmpPrimeData_ptr->DataIn));
	
	return Error;

#endif /*CRYS_NO_DH_SUPPORT*/
#endif /*CRYS_NO_PKI_SUPPORT*/
#endif /*CRYS_NO_HASH_SUPPORT*/

}  /* END of CRYS_DH_GetSecretKey function */


				                    
/****************************************************************/
/**
 * @brief CRYS_DH_X942_GetSecretData computes the shared secret key as follows:
 		1. OPTIONAL - validate the correctness of the argument keys
 		2. SecretKey = ServerPubKey ^ ClientPrvKey mod Prime
 		3. Use of Derivation function to derive a data key from the secret key 
 
 	The actual APIs that will be used by the user are:
 	CRYS_DH_X942_GetSecret_ASN1
 	CRYS_DH_X942_GetSecret_Concat
 
  	Note: All buffers arguments are represented in Big-Endian format

  @param[in] ClientPrvKey_ptr 	- A pointer to the Private key octet string 
  @param[in] ClientPrvKeySize  	- The Private key size, in bytes
  @param[in] ServerPubKey_ptr   - A pointer to the Server public key octet string - 
  @param[in] ServerPubKeySize   - The Server Public key size, in bytes
  @param[in] Prime_ptr 		- A pointer to the Prime octet string
  @param[in] PrimeSize 		- The size of the Prime string
  @param[in] OtherInfo - The  pointer to structure , containing the data, 
		shared by two entities of agreement and the data sizes. 
		This argument is optional (if not needed - set it to NULL).
  @param[in] hashMode			- The hash function to be used
  @param[in] DerFunc_mode 	- The type of function to derive the secret key to the key data.
  				We use ASN.1-based function or Hash concatenation function mode.
  @param[in] tmpBuff_ptr	- A pointer to the DH temp buffer structure. Not initialized.
  @param[out] SecretKeyData_ptr - A pointer to the buffer for secret keying data.
  @param[in] SecretKeyDataSize - Specifies the derived Secret Key data size needed in bytes.  
				The size can't be larger than CRYS_DH_MAX_SIZE_OF_KEYING_DATA 
				(required by implementation). 

  @return CRYSError_t - On success CRYS_OK is returned, on failure an ERROR as defined CRYS_DH_error.h:
        		CRYS_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;
 
*/
CEXPORT_C CRYSError_t CRYS_DH_X942_GetSecretData(
					DxUint8_t                  *ClientPrvKey_ptr,
					DxUint16_t                  ClientPrvKeySize,
					DxUint8_t                  *ServerPubKey_ptr,
					DxUint16_t                  ServerPubKeySize,				                    
					DxUint8_t                  *Prime_ptr,
					DxUint16_t                  PrimeSize,
					CRYS_DH_OtherInfo_t        *otherInfo_ptr,
					CRYS_DH_HASH_OpMode_t       hashMode,									
					CRYS_DH_DerivationFunc_Mode DerivFunc_mode,
					CRYS_DH_Temp_t             *tmpBuff_ptr,
					DxUint8_t                  *SecretKeyingData_ptr,
					DxUint16_t                  SecretKeyingDataSize )
{
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;

	DxUint16_t  SecretKeySize = PrimeSize;       

	/* FUNCTION LOGIC */
	
	/* ............... local initializations .............................. */
	/* -------------------------------------------------------------------- */
	
	/* initializing the Error to O.K */
	Error = CRYS_OK;
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_DH_UNSUPPORTED(ClientPrvKey_ptr, ClientPrvKeySize,  ServerPubKey_ptr, ServerPubKeySize, 
				Prime_ptr, PrimeSize, otherInfo_ptr, Error/*hashMode*/, Error/*DerivFunc_mode*/,  
				tmpBuff_ptr, SecretKeyingData_ptr, SecretKeyingDataSize, 
				SecretKeySize, Error, Error, Error, Error, 
				Error, Error, Error, Error, Error); 
			      
	#ifndef CRYS_NO_HASH_SUPPORT                                      
	#ifndef CRYS_NO_PKI_SUPPORT                                      
	#ifndef CRYS_NO_DH_SUPPORT
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */

	/* check pointers */
	if(tmpBuff_ptr == DX_NULL || SecretKeyingData_ptr == DX_NULL)
		return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;  

	/*check that the size of derived secret key is not NULL  */
	if(SecretKeyingDataSize == 0)
		return CRYS_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;
	
	/*check that the keying data size is not too large  */
	if(SecretKeyingDataSize > CRYS_DH_MAX_SIZE_OF_KEYING_DATA )
		return CRYS_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;
	
	/*Call the PKCS#3 get secret key function*/
	Error = CRYS_DH_GetSecretKey(
				ClientPrvKey_ptr,
				ClientPrvKeySize,
				ServerPubKey_ptr,
				ServerPubKeySize,				                    
				Prime_ptr,
				PrimeSize,
				&tmpBuff_ptr->UserPubKey,
				&tmpBuff_ptr->PrimeData, 				                    
				(DxUint8_t*)tmpBuff_ptr->TempBuff,
				&SecretKeySize);
				                    
	if(Error != CRYS_OK)
		goto ExitOnError;
	
					                
	/*Let the keydataSize from the previous function determine the key data length in the next function*/					                    
	Error = CRYS_KDF_KeyDerivFunc(
				(DxUint8_t*)tmpBuff_ptr->TempBuff,
				SecretKeySize,
				otherInfo_ptr,
				hashMode,				                    
				DerivFunc_mode,				                    
				SecretKeyingData_ptr,
				SecretKeyingDataSize);

ExitOnError:
	
	DX_VOS_MemSetZero(&tmpBuff_ptr->PrimeData, sizeof(tmpBuff_ptr->PrimeData));
	DX_VOS_MemSetZero(&tmpBuff_ptr->UserPubKey, sizeof(tmpBuff_ptr->UserPubKey));

			
	return Error;
  
#endif /*CRYS_NO_DH_SUPPORT*/
#endif /*CRYS_NO_PKI_SUPPORT*/
#endif /*CRYS_NO_HASH_SUPPORT*/

}/* END OF _DX_DH_X942_GetSecretData */				                    


/****************************************************************/
/**
 * @brief CRYS_DH_X942_HybridGetSecretData computes the shared secret key as follows:
 		1. OPTIONAL - validate the correctness of the argument keys
 		2. SecretKey1 = ServerPubKey1 ^ ClientPrvKey1 mod Prime
 		3. SecretKey2 = ServerPubKey2 ^ ClientPrvKey2 mod Prime
 		3. Use of Derivation function to derive a secret data key from the 2 secret keys 
 
 	The actual APIs that will be used by the user are:
 	CRYS_DH_X942_Hybrid_GetSecret_ASN1
 	CRYS_DH_X942_Hybrid_GetSecret_Concat
 
  	Note: All buffers arguments are represented in Big-Endian format.
 
  @param[in] ClientPrvKey_ptr1 	- A pointer to the First Private key octet string number 
  @param[in] ClientPrvKeySize1 	- The First Private key Size, in bytes
  @param[in] ClientPrvKey_ptr2 	- A pointer to the Second Private key octet string
  @param[in] ClientPrvKeySize2 	- The Second Private key Size, in bytes
  @param[in] ServerPubKey_ptr1  - A pointer to the First Server public key octet string
  @param[in] ServerPubKeySize1  - The First Server Public key Size, in bytes
  @param[in] ServerPubKey_ptr2  - A pointer to the Second Server public key octet string
  @param[in] ServerPubKeySize2  - The Second Server Public key Size, in bytes
  @param[in] Prime_ptr 		- A pointer to the Prime octet string
  @param[in] PrimeSize 		- The size of the Prime string
  @param[in] OtherInfo - The  pointer to structure , containing the data, 
		shared by two entities of agreement and the data sizes. 
		This argument is optional (if not needed - set it to NULL).
  @param[in] hashMode		- The hash function to be used.
  @param[in] DerFunc_mode 	- The type of function to derive the secret key to the key data.
                                  We use an ASN.1-based function or a Hash concatenation function mode.
  @param[in] tmpDhHybr_ptr      - A pointer to the DH hybrid temp byffer structure. Not initialized.
  @param[out] SecretKeyData_ptr - A pointer to the buffer for secret keying data.
  @param[in] SecretKeyDataSize - Specifies the derived Secret Key data size needed in bytes.  
				The size can't be 0 or larger than CRYS_DH_MAX_SIZE_OF_KEYING_DATA 
				(required by implementation). 

  @return CRYSError_t - On success the value CRYS_OK is returned, and on failure an ERROR as defined in CRYS_DH_error.h:
			CRYS_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;
			CRYS_DH_X942_HYBRID_SIZE1_BUFFER_ERROR;
			CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;      
			CRYS_DH_INVALID_ARGUMENT_SIZE_ERROR;
			CRYS_DH_INVALID_ARGUMENT_OPERATION_MODE_ERROR;   						
			CRYS_DH_ARGUMENT_GENERATOR_SMALLER_THAN_ZERO_ERROR;
			CRYS_DH_ARGUMENT_PRIME_SMALLER_THAN_GENERATOR_ERROR;						
			CRYS_DH_INVALID_L_ARGUMENT_ERROR;
			CRYS_DH_ARGUMENT_PRV_SIZE_ERROR;						
*/
CEXPORT_C CRYSError_t CRYS_DH_X942_HybridGetSecretData(
				DxUint8_t            *ClientPrvKey_ptr1,
				DxUint16_t            ClientPrvKeySize1,
				DxUint8_t            *ClientPrvKey_ptr2,
				DxUint16_t            ClientPrvKeySize2,				                    
				DxUint8_t            *ServerPubKey_ptr1,
				DxUint16_t            ServerPubKeySize1,
				DxUint8_t            *ServerPubKey_ptr2,
				DxUint16_t            ServerPubKeySize2,
				DxUint8_t            *Prime_ptr,
				DxUint16_t            PrimeSize,
				CRYS_DH_OtherInfo_t  *otherInfo_ptr,
				CRYS_DH_HASH_OpMode_t hashMode,
				CRYS_DH_DerivationFunc_Mode DerivFunc_mode,
				CRYS_DH_HybrTemp_t   *tmpDhHybr_ptr,
				DxUint8_t            *SecretKeyingData_ptr,
				DxUint16_t            SecretKeyingDataSize)
{
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/*The assignment to Prime size is according to the real size of the buffer SecretKeyData_ptr*/
	DxUint16_t SecretKeyDataSize1 = PrimeSize;
	DxUint16_t SecretKeyDataSize2 = PrimeSize;
	
	/* FUNCTION LOGIC */
	
	/* ............... local initializations .............................. */
	/* -------------------------------------------------------------------- */
	
	/* initializing the Error to O.K */
	Error = CRYS_OK;
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_DH_UNSUPPORTED( 
		ClientPrvKey_ptr1, ClientPrvKeySize1, ClientPrvKey_ptr2, ClientPrvKeySize2,  
		ServerPubKey_ptr1, ServerPubKeySize1, ServerPubKey_ptr2, ServerPubKeySize2, 
		Prime_ptr, PrimeSize, otherInfo_ptr, Error/*hashMode*/, Error/*DerFunc_mode*/, 
		tmpDhHybr_ptr, SecretKeyingData_ptr, SecretKeyingDataSize,
		SecretKeyDataSize1, SecretKeyDataSize2, Error, Error, Error, Error); 
			      
	#ifndef CRYS_NO_HASH_SUPPORT                                      
	#ifndef CRYS_NO_PKI_SUPPORT 
	#ifndef CRYS_NO_DH_SUPPORT
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */
	
	/* check pointers */
	if(tmpDhHybr_ptr == DX_NULL || SecretKeyingData_ptr == DX_NULL)
		return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;  

	/*check that the size of derived secret key is not NULL  */
	if(SecretKeyingDataSize == 0)
		return CRYS_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;

	/*check that the keying data size is not too large  */
	if(SecretKeyingDataSize > CRYS_DH_MAX_SIZE_OF_KEYING_DATA )
		return CRYS_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;


	/* get shared secret key (value) 1 */
	Error = CRYS_DH_GetSecretKey(
			ClientPrvKey_ptr1,
			ClientPrvKeySize1,
			ServerPubKey_ptr1,
			ServerPubKeySize1,				                    
			Prime_ptr,
			PrimeSize,
			&tmpDhHybr_ptr->UserPubKey,
			&tmpDhHybr_ptr->PrimeData, 				                    
			(DxUint8_t*)&tmpDhHybr_ptr->TempBuff,
			&SecretKeyDataSize1);
	if(Error != CRYS_OK)
		goto End;
		
	/* get shared secret key (value) 2 */
	Error = CRYS_DH_GetSecretKey(
			ClientPrvKey_ptr2,
			ClientPrvKeySize2,
			ServerPubKey_ptr2,
			ServerPubKeySize2,				                    
			Prime_ptr,
			PrimeSize,
			&tmpDhHybr_ptr->UserPubKey,
			&tmpDhHybr_ptr->PrimeData, 				                    
			(DxUint8_t*)&tmpDhHybr_ptr->TempBuff+SecretKeyDataSize1,
			&SecretKeyDataSize2);
	if(Error != CRYS_OK)
		goto End;		
	
	/* Derive the secret key according to the secret key size and value   * 
	*  key1||key2   						      */
	Error = CRYS_KDF_KeyDerivFunc(
			(DxUint8_t*)&tmpDhHybr_ptr->TempBuff,
			(DxUint16_t)(SecretKeyDataSize1 + SecretKeyDataSize2),
			otherInfo_ptr,
			hashMode,
			DerivFunc_mode,				                     				                    
			SecretKeyingData_ptr,
			SecretKeyingDataSize );
	
	if(Error != CRYS_OK)
		goto End;
	
End:	
	DX_VOS_MemSetZero(tmpDhHybr_ptr->TempBuff, sizeof(tmpDhHybr_ptr->TempBuff));

	return Error;
 
#endif /*CRYS_NO_DH_SUPPORT*/
#endif /*CRYS_NO_PKI_SUPPORT*/
#endif /*CRYS_NO_HASH_SUPPORT*/
 
}/* END OF CRYS_DH_X942_HybridGetSecretData */
				                    
/******************************************************************************************/
/** 
 * @brief The function checks the obtained DH public key according to its domain 
 *        parameters (see X9.42-2001)
 *
 * @param[out] modP_ptr           - The modulus (prime) P.
 * @param[in]  modPSizeBytes      - The modulus size in bytes.
 * @param[in]  orderQ_ptr         - The pointer to prime order Q of generator,
 * @param[in]  orderQSizeBytes    - The size of order of generator in bytes.
 * @param[in]  pubKey_ptr         - The pointer to public key to be validated . 
 * @param[in]  pubKeySizeBytes    - The public key size in bytes. 
 * @param[in]  tempBuff_ptr       - The temp buffer for internal calculations.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure or if one or more domain  
 *                       parameters are invalid the function returns a predefined error code.
 *
 *              Assuming: The DH domain parameters are valid.
 */
CEXPORT_C CRYSError_t CRYS_DH_CheckPubKey( 
                          DxUint8_t              *modP_ptr,             /*in */  
                          DxUint32_t              modPsizeBytes,        /*in */
                          DxUint8_t              *orderQ_ptr,           /*in */
                          DxUint32_t              orderQsizeBytes,      /*in */
                          DxUint8_t              *pubKey_ptr,           /*in */
                          DxUint32_t              pubKeySizeBytes,      /*in */
                          CRYS_DH_Temp_t  	 *tempBuff_ptr          /*in */)
{
  
	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	
	/* size in bits of modulus P and order Q and public key */
	DxUint32_t  modPsizeBits;
	DxUint32_t  orderQsizeBits;   
	DxUint32_t  pubKeySizeBits; 
	
	/* comparing result */
	DxInt_t  cmpRes;
	CRYS_COMMON_CmpCounter_t cmpCounters;
	CRYS_DHPubKey_t  *tmpPubKey_ptr;
	
	
	/* FUNCTION  LOGIC */
	
	Error = CRYS_OK;
	
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_DH_UNSUPPORTED( modP_ptr, modPsizeBytes, orderQ_ptr, orderQsizeBytes,   
			     pubKey_ptr, pubKeySizeBytes, tempBuff_ptr,                              
			     modPsizeBits, orderQsizeBits, cmpRes, cmpCounters,     
			     pubKeySizeBits,  tmpPubKey_ptr, Error, Error, Error, Error, 
			     Error, Error, Error, Error, Error ); 
			      
	#ifndef CRYS_NO_HASH_SUPPORT                                      
	#ifndef CRYS_NO_PKI_SUPPORT                                      
	#ifndef CRYS_NO_DH_SUPPORT
	
	/*------------------------------- */
	/* Step 1. Check input pointers   */
	/*------------------------------- */
	
	/* check pointers: modP, generator and tempBuff. Note: other pointers may be NULL  */
	if( modP_ptr == DX_NULL ||
	orderQ_ptr == DX_NULL ||
	pubKey_ptr == DX_NULL ||
	tempBuff_ptr == DX_NULL  ) {
		return CRYS_DH_INVALID_ARGUMENT_POINTER_ERROR;
	}
	/* temp public key buffer */
	tmpPubKey_ptr = (CRYS_DHPubKey_t*)((void*)&tempBuff_ptr->UserPubKey.PublicKeyDbBuff);
	
	/*----------------------------------------------------------- */    
	/* Step 2. Calculate and check the sizes of modulus and order */
	/*----------------------------------------------------------- */ 
	/* preliminary check */
	if( modPsizeBytes > CRYS_DH_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / 8 )
		return CRYS_DH_INVALID_MODULUS_SIZE_ERROR;
		
	if( orderQsizeBytes > modPsizeBytes )  
		return CRYS_DH_INVALID_ORDER_SIZE_ERROR;


	/* convert input data into LSW arrays */
	/*------------------------------------*/ 
	DX_VOS_MemSetZero( tempBuff_ptr, sizeof(CRYS_DH_ExpTemp_t) );    

	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(tmpPubKey_ptr->n, modPsizeBytes, modP_ptr, modPsizeBytes);
	if(Error)
		return Error;

	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(tmpPubKey_ptr->e, modPsizeBytes, orderQ_ptr, orderQsizeBytes);
	if(Error)
		return Error;

	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(tempBuff_ptr->PrimeData.DataIn, modPsizeBytes, pubKey_ptr, pubKeySizeBytes);
	if(Error)
		return Error;

	/* calculate sizes in bits of input parameters */
	modPsizeBits   = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(tmpPubKey_ptr->n, (DxUint16_t)modPsizeBytes/4);
	orderQsizeBits = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(tmpPubKey_ptr->e, (DxUint16_t)(orderQsizeBytes+3)/4);
	pubKeySizeBits = CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(tempBuff_ptr->PrimeData.DataIn, (DxUint16_t)(pubKeySizeBytes+3)/4);

	/* check sizes */
	if(modPsizeBits < CRYS_DH_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ||
	   modPsizeBits % 256 != 0 ||
	   modPsizeBits > CRYS_DH_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) {
		Error = CRYS_DH_INVALID_MODULUS_SIZE_ERROR;	
		goto End;  	
	}
	
	if(orderQsizeBits < CRYS_DH_SEED_MIN_SIZE_IN_BITS ||
	   orderQsizeBits % 32 != 0 ) {
		Error = CRYS_DH_INVALID_ORDER_SIZE_ERROR;   	
		goto End;  	
	}
	
	if(pubKeySizeBits > modPsizeBits ||
	   pubKeySizeBits <= 1 ) {
		Error = CRYS_DH_INVALID_PUBLIC_KEY_SIZE_ERROR;   	   
		goto End;  	
	}
	

	/*----------------------------------------------------------- */    
	/* Step 2. Check value of public key:   pubKey < P-1          */
	/*         Note: pubKey > 1 already is checked above          */
	/*----------------------------------------------------------- */
	
	/* decrement modulus in temp buffer n (in little endianness). Note: the modulus is odd */
	tmpPubKey_ptr->n[0] -= 1;
	
	/* compare with pub key saved in temp buff e */
	cmpCounters = CRYS_COMMON_CmpLsWordsUnsignedCounters( 
					  tmpPubKey_ptr->e, /* counter1 - pubKey */
					  (DxUint16_t)(pubKeySizeBytes+3)/4, 
					  tmpPubKey_ptr->n, /* counter2 - (P-1) */
					  (DxUint16_t)modPsizeBytes/4);
					  
	if(cmpCounters != CRYS_COMMON_CmpCounter2GraterThenCounter1) {
		Error = CRYS_DH_INVALID_PUBLIC_KEY_ERROR;
		goto End;  	
	}
	
	/*----------------------------------------------------*/
	/* Step 4. Initialization of PubKey and PrivData      */
	/*         structures for exponentiation              */
	/*----------------------------------------------------*/ 
	
	/* increment (revert) modulus in temp buffer n (in little endianness) */
	tmpPubKey_ptr->n[0] += 1;
	
	/* set modulus and exponent sizes in DH_PubKey structure  */
	tmpPubKey_ptr->nSizeInBits = modPsizeBits;
	tmpPubKey_ptr->eSizeInBits = orderQsizeBits;
	
	/*  initialize the H value in LLF of PubKey for exponentiation  */
	Error = LLF_PKI_RSA_InitPubKeyDb( tmpPubKey_ptr );
	
	if( Error != CRYS_OK )
		goto End; 
	
	/*-----------------------------------------------------------*/
	/* Step 3. Calculate Res = Key ^ Q mod P , if Res == 1,      */
	/*         then key is valid, else non valid                 */
	/*-----------------------------------------------------------*/
	
	/* exponentiation DataOut = DataIn ^ exp mod n */   
	Error = LLF_PKI_RSA_ExecPubKeyExp(tmpPubKey_ptr, &tempBuff_ptr->PrimeData);
									 
	if(Error != CRYS_OK)
		goto End;
	
	/* set 1 to PubKey_ptr->n buffer (used as temp buffer) */
	DX_VOS_MemSetZero((DxUint8_t*)&tmpPubKey_ptr->n, modPsizeBytes);
	tmpPubKey_ptr->n[0] = 1;
	   
	/* compare DataOut to 1: */
	cmpRes = CRYS_COMMON_CmpLsWordsUnsignedCounters(
				tempBuff_ptr->PrimeData.DataOut, modPsizeBytes/4, tmpPubKey_ptr->n, modPsizeBytes/4); 
	   
	if(cmpRes != 0)  /* if Res != 1 */
		return CRYS_DH_INVALID_PUBLIC_KEY_ERROR;
	
	/* End of function */	
End:

	/* clean temp buffers */
	DX_VOS_MemSetZero(tempBuff_ptr, sizeof(CRYS_DH_Temp_t));
	
	return Error;
  	
  #endif /*CRYS_NO_DH_SUPPORT*/
  #endif /*CRYS_NO_PKI_SUPPORT*/
  #endif /*CRYS_NO_HASH_SUPPORT*/	
}
				                    
