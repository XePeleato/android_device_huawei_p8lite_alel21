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
   *  Creation date   :  17 Jul. 2007
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version CRYS_KDF.c#1:csrc:8
   *  \author R.Levin
   * 
   */

/************* Include Files ****************/

#include "dx_vos_mem.h"
#include "crys_ccm.h"
#include "crys_common_math.h"
#include "crys_kdf.h"
#include "crys_kdf_error.h"
#include "crys_hash.h"
#include "debug.h"

/************************ Defines *******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/************************ Enums *********************************/

/************************ macros ********************************/

/* this macro is required to remove compilers warnings if the HASH or PKI is not supported */

#if( defined CRYS_NO_HASH_SUPPORT || defined CRYS_NO_KDF_SUPPORT )
#define RETURN_IF_KDF_UNSUPPORTED( a , b , c , d , e , f , g , h , i , j , k , l , a1 , b1 , c1 , d1 , e1 , f1 , g1 , h1 , i1 ) \
  (a)=0;(b)=0;(c)=0;(d)=0;(e)=0;(f)=0;(g)=0;(h)=0;(i)=0;(j)=0;(k)=0;(l)=0; \
  (a1)=0;(b1)=0;(c1)=0;(d1)=0;(e1)=0;(f1)=0;(g1)=0;(h1)=0;(i1)=0;(j1)=0; \
  (a)=(a);(b)=(b);(c)=(c);(d)=(d);(e)=(e);(f)=(f);(g)=(g);(h)=(h);(i)=(i);(j)=(j);(k)=(k);(l)=(l); \
  (a1)=(a1);(b1)=(b1);(c1)=(c1);(d1)=(d1);(e1)=(e1);(f1)=(f1);(g1)=(g1);(h1)=(h1);(i1)=(i1); \
  return CRYS_KDF_IS_NOT_SUPPORTED
#else  /* CRYS_NO_HASH_SUPPORT */
#define RETURN_IF_KDF_UNSUPPORTED( a , b , c , d , e , f , g , h , i , j , k , l , a1 , b1 , c1 , d1 , e1 , f1 , g1 , h1 , i1 ) 
#endif /* !CRYS_NO_KDF_SUPPORT !CRYS_NO_HASH_SUPPORT */

/************************    Global Data    ******************************/ 
DxUint32_t  gKdfHashTempBuff[CRYS_HASH_SHA512_BLOCK_SIZE_IN_BYTES/4];

/************************ Private Functions ******************************/ 

/************************************************************************
*   The function performs  Hash update for data with the size not        
*   aligned to Hash block. 
*
*   Note: remBuffSize_ptr - a pointer to the remaining size of the 
*         temp buffer to fill by the data.
*
************************************************************************/
static CRYSError_t _DX_KDF_HASH_UnalignUpdate(
					CRYS_HASHUserContext_t  *hashContext_ptr, 
					DxUint8_t *data_ptr, DxUint32_t dataSize, 
					DxUint8_t *buff_ptr, DxUint32_t *remBuffSize_ptr)
{
	CRYSError_t error = CRYS_OK;
	DxUint32_t  tmpSize;
	DxUint8_t *tmp_ptr;

	/* set buff_ptr to begin of empty part of temp buffer */
	tmp_ptr = buff_ptr + CRYS_HASH_BLOCK_SIZE_IN_BYTES - *remBuffSize_ptr;
	
	/* if the temp buffer not empty, append it by the data and update Hash on it */
	if(*remBuffSize_ptr < CRYS_HASH_BLOCK_SIZE_IN_BYTES) {
		if(dataSize > *remBuffSize_ptr) {

			DX_VOS_FastMemCpy(buff_ptr, data_ptr, *remBuffSize_ptr);
			
			/* update on the data in temp buffer */
			error = CRYS_HASH_Update( hashContext_ptr, buff_ptr, 
						CRYS_HASH_BLOCK_SIZE_IN_BYTES);
			if(error != CRYS_OK)
				return error; 

			/* update pointers and sizes */
			data_ptr += *remBuffSize_ptr;
			dataSize -= *remBuffSize_ptr;
			*remBuffSize_ptr = CRYS_HASH_BLOCK_SIZE_IN_BYTES;
			tmp_ptr = buff_ptr;
		}
		else if(dataSize < *remBuffSize_ptr) {
			DX_VOS_FastMemCpy(tmp_ptr, data_ptr, dataSize);
			*remBuffSize_ptr -= dataSize;
			return error;
		}
	}

	/* Update Hash on remaining input data */
	tmpSize = dataSize % CRYS_HASH_BLOCK_SIZE_IN_BYTES;
	if (tmpSize > 0)
	{
		dataSize -= tmpSize;
		DX_VOS_FastMemCpy(tmp_ptr, data_ptr + dataSize, tmpSize);
		*remBuffSize_ptr -= tmpSize;
	} 

	if(dataSize > 0)
		error = CRYS_HASH_Update( hashContext_ptr, data_ptr, dataSize);

	return error;
}


/************************ Public Functions ******************************/

/************************ Public Functions ******************************/


/****************************************************************/
/**
 * @brief CRYS_KDF_KeyDerivFunc performs key derivation according to one of some modes defined in standards: 
		ANSI X9.42-2001, ANSI X9.63, OMA_TS_DRM_DRM_V2_0-20050712-C, ISO/IEC 18033-2.
	
	The present implementation of the function allows the following operation modes:
	- CRYS_KDF_ASN1_DerivMode - mode based on  ASN.1 DER encoding;
	- CRYS_KDF_ConcatDerivMode - mode based on concatenation;
	- CRYS_KDF_X963_DerivMode = CRYS_KDF_ConcatDerivMode;
	- CRYS_KDF_OMA_DRM_DerivMode - specific mode for OMA DRM;
	- CRYS_KDF_ISO18033_KDF1_DerivMode - specific mode according to 
		ECIES-KEM algorithm (ISO/IEC 18033-2).

	The purpose of this function is to derive a keying data from the shared secret value and some 
	other optional shared information (SharedInfo).
		
	For calling the API on some specific modes may be used the following macros:
	- CRYS_KDF_ASN1_KeyDerivFunc ;			
	- CRYS_KDF_ConcatKeyDerivFunc ;
	- CRYS_KDF_OMADRM_DerivFunc.
 			
  \note The length in Bytes of the hash result buffer is denoted by "hashlen".
  \note All buffers arguments are represented in Big-Endian format.
 
  @param[in] ZZSecret_ptr - A pointer to shared secret value octet string. 
  @param[in] ZZSecretSize - The size of the shared secret value in bytes. The maximal
		size is defined as: CRYS_KDF_MAX_SIZE_OF_SHARED_SECRET_VALUE.
  @param[in] OtherInfo - The  pointer to structure , containing the data, 
		shared by two entities of agreement and the data sizes. 
		This argument is optional for some modes (if it is not needed - set NULL).
		On KDF OMA_DRM and two ISO/IEC 18033-2 modes - set NULL. 
        	On KDF ASN1 mode the OtherInfo and its AlgorithmID entry are mandatory.
  @param[in] KDFhashMode - The KDF identifier of hash function to be used. 
		The hash function output must be at least 160 bits.
  @param[in] derivation_mode - Specifies one of above described derivation modes.
  @param[out] KeyingData_ptr - A pointer to the buffer for derived keying data.
  @param[in] KeyingDataSizeBytes - The size in bytes of the keying data to be derived. 
		The maximal size is defined as: CRYS_KDF_MAX_SIZE_OF_KEYING_DATA.
 
  @return CRYSError_t - On success the value CRYS_OK is returned, 
			and on failure an ERROR as defined in CRYS_KDF_error.h:
			CRYS_KDF_INVALID_ARGUMENT_POINTER_ERROR
			CRYS_KDF_INVALID_KEY_DERIVATION_MODE_ERROR
			CRYS_KDF_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR
			CRYS_KDF_INVALID_SIZE_OF_DATA_TO_HASHING_ERROR
			CRYS_KDF_INVALID_ARGUMENT_HASH_MODE_ERROR
			CRYS_KDF_INVALID_OTHER_INFO_SIZE_ERROR
			CRYS_KDF_INVALID_KEYING_DATA_SIZE_ERROR
*/
CEXPORT_C CRYSError_t  CRYS_KDF_KeyDerivFunc( 
					DxUint8_t                *ZZSecret_ptr,
					DxUint32_t                ZZSecretSize,
					CRYS_KDF_OtherInfo_t     *OtherInfo_ptr,
					CRYS_KDF_HASH_OpMode_t    KDFhashMode,
					CRYS_KDF_DerivFuncMode_t  derivation_mode,
					DxUint8_t                *KeyingData_ptr,
					DxUint32_t                KeyingDataSizeBytes )				                    
{

	/* FUNCTION DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t Error;
	/* HASH function context structure buffer and parameters  */
	CRYS_HASHUserContext_t  HashContext;
	CRYS_HASH_OperationMode_t hashMode;
	DxUint32_t  HashOutputSizeBytes;
	
	/*The result buffer for the Hash*/
	CRYS_HASH_Result_t   HashResultBuff;
	/* Total count of full HASH blockss for deriving the keying data */
	DxUint32_t  CountOfHashBlocks;

	/* Loop counters */
	DxUint32_t  i, j;
	/*counter of Hash blocks (to be hashed with ZZ and OtherInfo) */
	DxUint32_t Counter;
	/* Current output buffer position */
	DxUint32_t CurrentOutputBuffPos = 0;

	DxUint32_t  *OtherInfoEntry_ptr;
	DxUint8_t   *temp_ptr;
	DxUint32_t  remBuffSize;

	/*.... HASH Init function .....*/
	
	/* FUNCTION LOGIC */
	
	/* ............... local initializations .............................. */
	/* -------------------------------------------------------------------- */
	
	/* initializing the Error to O.K */
	Error = CRYS_OK;
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	PRINT_INFO("--->NOW inter into CRYS_KDF_KeyDerivFunc\n");
	RETURN_IF_KDF_UNSUPPORTED( ZZSecret_ptr , ZZSecretSize , OtherInfo_ptr , KDFhashMode , 
				derivation_mode , KeyingData_ptr , KeyingDataSizeBytes , hashMode , 
				HashOutputSizeBytes , HashContext.valid_tag , CountOfHashBlocks , 
				i , j , Counter , CurrentOutputBuffPos , HashResultBuff[0] , 
				OtherInfoEntry_ptr , temp_ptr ,  remBuffSize , Error , Error ); 
			      
	#ifndef CRYS_NO_HASH_SUPPORT 
	#ifndef CRYS_NO_KDF_SUPPORT 
			  
	
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */
	
	/* if an argument pointer is DX_NULL return an error */
	if( ZZSecret_ptr == DX_NULL || KeyingData_ptr == DX_NULL )
		return CRYS_KDF_INVALID_ARGUMENT_POINTER_ERROR;
	
	if( derivation_mode >= CRYS_KDF_DerivFunc_NumOfModes )
		return CRYS_KDF_INVALID_KEY_DERIVATION_MODE_ERROR;

	if( derivation_mode == CRYS_KDF_ASN1_DerivMode && 
	   (OtherInfo_ptr == DX_NULL || OtherInfo_ptr->SizeOfAlgorithmID == 0))
		return CRYS_KDF_INVALID_ARGUMENT_POINTER_ERROR;
	
	/*On KDF1 and KDF2 derivation modes set OtherInfo_ptr = DX_NULL */
	if(derivation_mode == CRYS_KDF_ISO18033_KDF1_DerivMode ||
	   derivation_mode == CRYS_KDF_ISO18033_KDF2_DerivMode){
		OtherInfo_ptr = DX_NULL;
	}
        
	/* Check sizes of the input data to be hashed according to KDF        * 
	*  limitations) 						      */
	if(ZZSecretSize == 0 || ZZSecretSize > CRYS_KDF_MAX_SIZE_OF_SHARED_SECRET_VALUE )
   		return CRYS_KDF_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR;
	
   	if( OtherInfo_ptr != DX_NULL )
	{
		if( OtherInfo_ptr->SizeOfAlgorithmID > CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY )
		    return CRYS_KDF_INVALID_OTHER_INFO_SIZE_ERROR;

		if( OtherInfo_ptr->SizeOfPartyUInfo > CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY )
		    return CRYS_KDF_INVALID_OTHER_INFO_SIZE_ERROR;

		if( OtherInfo_ptr->SizeOfPartyVInfo > CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY )
		    return CRYS_KDF_INVALID_OTHER_INFO_SIZE_ERROR;

		if( OtherInfo_ptr->SizeOfSuppPrivInfo > CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY )
	  	    return CRYS_KDF_INVALID_OTHER_INFO_SIZE_ERROR;

		if( OtherInfo_ptr->SizeOfSuppPubInfo > CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY )
		    return CRYS_KDF_INVALID_OTHER_INFO_SIZE_ERROR;
	}
	
	/* Check the size of keying data  */
	if( KeyingDataSizeBytes == 0 || KeyingDataSizeBytes > CRYS_KDF_MAX_SIZE_OF_KEYING_DATA) 
		return CRYS_KDF_INVALID_KEYING_DATA_SIZE_ERROR;


	/*................ Setting parameters according to current operation modes .......... */
	/*------------------------------------------------------------------------------------*/
	switch( KDFhashMode )
	{
		case CRYS_KDF_HASH_SHA1_mode:
			hashMode = CRYS_HASH_SHA1_mode;
			HashOutputSizeBytes = CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES;
			break;
		case CRYS_KDF_HASH_SHA224_mode:
			hashMode = CRYS_HASH_SHA224_mode;
			HashOutputSizeBytes = CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES;
			break;
		case CRYS_KDF_HASH_SHA256_mode:
			hashMode = CRYS_HASH_SHA256_mode;
			HashOutputSizeBytes = CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES;
			break;
	
		case CRYS_KDF_HASH_SHA384_mode:
			hashMode = CRYS_HASH_SHA384_mode;
			HashOutputSizeBytes = CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES;
			break;
		case CRYS_KDF_HASH_SHA512_mode:
			hashMode = CRYS_HASH_SHA512_mode;
			HashOutputSizeBytes = CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES;
			break;
	
		default:
		    return CRYS_KDF_INVALID_ARGUMENT_HASH_MODE_ERROR;
	}
	
	
	/* Count of HASH blocks needed for key derivation etc. */
	CountOfHashBlocks = ( KeyingDataSizeBytes + HashOutputSizeBytes - 1 )/ HashOutputSizeBytes;
	temp_ptr = (DxUint8_t*)&gKdfHashTempBuff[0];
	remBuffSize = CRYS_HASH_BLOCK_SIZE_IN_BYTES;

	/* **********  Keying data derivation loop ************ */

	for( i = 0; i < CountOfHashBlocks; i++ )
	{
		/*.... HASH Init function .....*/
		Error = CRYS_HASH_Init(&HashContext,
                           		hashMode);
		PRINT_INFO("--->NOW after CRYS_HASH_Init\n");		
		if(Error != CRYS_OK)
			return Error; 

		/*....... Hashing input data by calling HASH_Update function .......*/
		/*------------------------------------------------------------------*/

		/*.... Hashing of the shared secret value ....*/
		Error = _DX_KDF_HASH_UnalignUpdate(&HashContext,  
						ZZSecret_ptr,ZZSecretSize, 
						temp_ptr, &remBuffSize);
		PRINT_INFO("--->NOW after  _DX_KDF_HASH_UnalignUpdate\n");
		if(Error != CRYS_OK)
			goto End; 

		/*.... Hashing of the AlgorithmID (on ASN1 Derivation Mode only) ....*/
		if( derivation_mode == CRYS_KDF_ASN1_DerivMode )
		{
			Error = _DX_KDF_HASH_UnalignUpdate(&HashContext,  
						(DxUint8_t *)(OtherInfo_ptr->AlgorithmID),
						OtherInfo_ptr->SizeOfAlgorithmID, 
						temp_ptr, &remBuffSize);
			 PRINT_INFO("--->NOW after derivation_mode == CRYS_KDF_ASN1_DerivMode , _DX_KDF_HASH_UnalignUpdate\n");
			if(Error != CRYS_OK)
				goto End; 
		} 

		/* Set the blocks counter in big endianness mode */
		if(derivation_mode == CRYS_KDF_ISO18033_KDF1_DerivMode)
			Counter = CRYS_COMMON_REVERSE32(i);
		else		
			Counter = CRYS_COMMON_REVERSE32(i+1);
             PRINT_INFO("--->NOW after CRYS_COMMON_REVERSE32()\n");
		/*.... Hashing of the blocks counter ....*/ 
		Error = _DX_KDF_HASH_UnalignUpdate(&HashContext,  
					(DxUint8_t *)&Counter,
					sizeof(DxUint32_t), 
					temp_ptr, &remBuffSize);
		if(Error != CRYS_OK)
			goto End; 

		/* ..... Hashing of remaining data of the OtherInfo ..... */
		if( OtherInfo_ptr != DX_NULL )
		{
			/* Set OtherInfoEntry_ptr to second entry pointer */
			OtherInfoEntry_ptr = (DxUint32_t*)OtherInfo_ptr + CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY/4 + 1;

			/* OtherInfo data concatenating and hashing loop */
			for( j = 0; j < CRYS_KDF_COUNT_OF_OTHER_INFO_ENTRIES - 1; j++ )
			{
				/* if entry exists hash it */
				if( *(OtherInfoEntry_ptr + CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY/4) != 0 )
				{
					Error = _DX_KDF_HASH_UnalignUpdate(&HashContext,  
							(DxUint8_t *)OtherInfoEntry_ptr/*pointer to entry*/,
							*(OtherInfoEntry_ptr + CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY/4)/*size of entry*/, 
							temp_ptr, &remBuffSize);
					if(Error != CRYS_OK)
						goto End; 
				}
				/* Shift the pointer to the next entry */
				OtherInfoEntry_ptr += CRYS_KDF_MAX_SIZE_OF_OTHER_INFO_ENTRY/4 + 1;
			}
		} 

		/* last Hash update on remaining data in the temp buffer */
		if(remBuffSize < CRYS_HASH_BLOCK_SIZE_IN_BYTES) {
			Error = CRYS_HASH_Update(&HashContext, temp_ptr, 
						CRYS_HASH_BLOCK_SIZE_IN_BYTES - remBuffSize);
			 PRINT_INFO("--->NOW after CRYS_HASH_Update()\n");
			if(Error != CRYS_OK)
				goto End; 
		}

		/* ..........  HASH Finish operation ............. */
		Error = CRYS_HASH_Finish( &HashContext ,
					HashResultBuff );
		PRINT_INFO("--->NOW after CRYS_HASH_Finish()\n");
		if(Error != CRYS_OK)
			goto End; 

		/* Correction of output data size for last block ( if it is not full ) */
		if( i == CountOfHashBlocks - 1 )		
			HashOutputSizeBytes = KeyingDataSizeBytes - i * HashOutputSizeBytes;
		PRINT_INFO("--->NOW begin DX_VOS_FastMemCpy\n");
		/* Copying HASH data into output buffer */
		DX_VOS_FastMemCpy(&KeyingData_ptr[CurrentOutputBuffPos],(DxUint8_t *)HashResultBuff, HashOutputSizeBytes);
		
		/* Increment the output buffer position */
		CurrentOutputBuffPos += HashOutputSizeBytes;
	}
End:
	DX_VOS_MemSetZero(gKdfHashTempBuff, sizeof(gKdfHashTempBuff));
	
	return Error;

  #endif /*CRYS_NO_KDF_SUPPORT*/
  #endif /*CRYS_NO_HASH_SUPPORT*/

}/* END OF CRYS_KDF_KeyDerivationFunc */	
