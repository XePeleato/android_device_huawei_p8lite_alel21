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
#include "crys.h"
#include "crys_common.h"
#include "crys_common_math.h"
#include "crys_ecpki_error.h"
#include "crys_ecpki_local.h"
#include "plat_print_dbg.h"
#include "cc_acl.h"

/* .............. LLF level includes and definitions.............. */
#include "llf_ecpki_export.h"

/* canceling the lint warning:
   Info 717: do ... while(0) */
/*lint --e{717} */

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/* canceling the lint warning:
Info 716: while(1) ... */ 
/*lint --e{716} */

/************************ Defines ***************************************/

/* canceling the lint warning:
   Unusual pointer cast (incompatible indirect types) */
/*lint --e{740} */

/************************ Enums *****************************************/

/************************ Typedefs **************************************/

/************************ Global Data ***********************************/

/************* Private function prototype *******************************/

/************************ Public Functions ******************************/


/**********************************************************************************
 *                    CRYS_ECPKI_BuildPrivKey function                            *
 **********************************************************************************/
/*!\brief Builds (imports) the user private key structure from an existing 
            private key so that this structure can be used by other EC
            primitives.

            When operating the EC cryptographic operations with existing and saved 
            EC private keys, this function should be called first.

		    The function performs the following operations:
			  - Checks validity of incoming variables and pointers.
			  - Checks, that 0 < PrivKey < r (r - EC generator order).
			  - Converts incoming key data from big endian into little endian form.
			  - Initializes variables and structures. 
   
   @param[in]  DomainID           The enumerator variable defines current EC domain.
   @param[in]  PrivKeyIn_ptr      Pointer to private key data. 
   @param[in]  PrivKeySizeInBytes Size of private key data in bytes. Must be great than null and
                                  less or equall to EC OrderSizeInBytes.
   @param[out] UserPrivKey_ptr    Pointer to the private key structure. 
                                  This structure is used as input to the ECPKI 
                                  cryptographic primitives.
   @return   CRYSError_t: 
			CRYS_OK
			CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_IN_PTR_ERROR
			CRYS_ECPKI_BUILD_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR
			CRYS_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR			 
*/	
CEXPORT_C CRYSError_t CRYS_ECPKI_BuildPrivKey(
				CRYS_ECPKI_DomainID_t      DomainID,	      /*in */   
				DxUint8_t  	          *PrivKeyIn_ptr,     /*in*/
				DxUint32_t                 PrivKeySizeInBytes,/*in*/
				CRYS_ECPKI_UserPrivKey_t  *UserPrivKey_ptr    /*out*/ ) 
{
	/* FUNCTION DECLARATIONS */
	
	/* the private key structure pointer */
	CRYS_ECPKI_PrivKey_t *PrivKey_ptr;
	
	/*  EC domain info structure and parameters */
	CRYS_ECPKI_DomainInfo_t  DomainInfo;  
	DxUint32_t  OrderSizeInBytes;
	
	/* the Error return code identifier */
	CRYSError_t Error; 
	
	
	
	/* FUNCTION LOGIC */
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_ECPKI_UNSUPPORTED( DomainID , PrivKeyIn_ptr , PrivKeySizeInBytes , 
			      UserPrivKey_ptr , PrivKey_ptr , DomainInfo.EC_ModulusSizeInBits ,                              
			      OrderSizeInBytes ,Error ,  Error , Error , Error , Error , Error , Error ,
			      Error , Error , Error , Error , Error , Error , Error ,Error ); 
				   
	
#ifndef CRYS_NO_ECPKI_SUPPORT  
    
	/* Error initialization */
	Error = CRYS_OK;
	
	/* ................. checking the validity of the pointer arguments ....... */
	/* ------------------------------------------------------------------------ */
	
	/* ...... checking the key database handle pointer ....................     */
	if( PrivKeyIn_ptr == DX_NULL )   
		return  CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_IN_PTR_ERROR;   	   
	 
	/* ...... checking the validity of the User Private Key pointer ........... */
	if( UserPrivKey_ptr == DX_NULL ) 
		return  CRYS_ECPKI_BUILD_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR;   		
	
	/* ...... checking the EC domain ID.................... */
	if( DomainID >= CRYS_ECPKI_DomainID_OffMode )
		return  CRYS_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR;  	
	
	if(DxCcAcl_IsBuffAccessOk(ACCESS_READ, PrivKeyIn_ptr, PrivKeySizeInBytes) || 
	   DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPrivKey_ptr, sizeof(CRYS_ECPKI_UserPrivKey_t))){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/* Get EC Domain information from LLF level */
	Error = LLF_ECPKI_GetDomainInfo(DomainID, &DomainInfo);
	
	if( Error != CRYS_OK ) 
		 goto End;  
	
	/* EC order size in bytes */ 
	OrderSizeInBytes = (DomainInfo.EC_OrderSizeInBits + 7) / 8;
	
	if( PrivKeySizeInBytes == 0 || PrivKeySizeInBytes > OrderSizeInBytes)
		return  CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_SIZE_ERROR; 
	
	/****************  FUNCTION LOGIC  **************************************/
	
	/* ...... copy the buffers to the key handle structure ................ */
	/* -------------------------------------------------------------------- */
	
	/* setting the pointer to the key database */
	PrivKey_ptr = (CRYS_ECPKI_PrivKey_t *)((void*)UserPrivKey_ptr->PrivKeyDbBuff);
	
	/* clear the private key db */
	DX_VOS_MemSet(PrivKey_ptr , 0 , sizeof(CRYS_ECPKI_PrivKey_t));
	
	/* loading the private key db to little endian and domain ID */
	Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(
		PrivKey_ptr->PrivKey, sizeof(PrivKey_ptr->PrivKey), 
		PrivKeyIn_ptr, PrivKeySizeInBytes);
	if(Error != CRYS_OK)
		return Error;
			     
	PrivKey_ptr->DomainID = DomainID;                         
	
	/* partly check the validity of the private key */
	Error = LLF_ECPKI_CheckPrivKeySize(DomainID, PrivKey_ptr->PrivKey, PrivKeySizeInBytes );
	
	if( Error != CRYS_OK )      
		goto End;
	
	/* initialize LLF private key database */
	Error = LLF_ECPKI_InitPrivKeyDb( PrivKey_ptr);
	
	if( Error != CRYS_OK )      
		goto End;
	
	/* ................. end of the function ................................. */
	/* ----------------------------------------------------------------------- */
	
	End:      
	/* if the created structure is not valid - clear it */
	if( Error != CRYS_OK )
	{
		DX_VOS_MemSet( UserPrivKey_ptr , 0 , sizeof(CRYS_ECPKI_UserPrivKey_t) ); 
		
		return Error;
	}
	
	/* ................ set the private key validation tag ................... */  
	UserPrivKey_ptr->valid_tag = CRYS_ECPKI_PRIV_KEY_VALIDATION_TAG;
	
	return Error;
	
#endif /* !CRYS_NO_ECPKI_SUPPORT */

} /* End of CRYS_ECPKI_BuildPrivKey() */
 
 


/**********************************************************************************
 *	              _DX_ECPKI_BuildPublKey function                                *
 **********************************************************************************/
/**
   @brief The _DX_ECPKI_BuildPublKey function checks the validity and builds the user public 
          key structure from imported public key data for using  it in other ECC primitives. 

	When operating the EC cryptographic algorithms with imported EC public
	key, this function should be called before using of the public key.
	
	The user must call this function by appropriate macros, according to necessary
	validation level [SEC1. ECC standard: 3.2]:
	- full checking of public key - CRYS_ECPKI_BuildPublKeyFullCheck,
	- partly checking of public key - CRYS_ECPKI_BuildPublKeyPartCheck,
	- checking the input pointers and sizes only - CRYS_ECPKI_BuildPublKey.
	
	The function performs the following operations:
	- Checks validity of incoming variables and pointers;
	- Converts incoming key data from big endian into little endian as follows:
	  If public key is given in uncompressed form the function converts 
	  coordinates X and Y separately to words arrays with little endian order of 
	  the wordsand copies them in output buffer, else returns an error;
	-   according to CheckMode parameter performs full or partly checking of public 
	    key validaty by calling the LLF function.
	-   Initializes variables and structures. 
	
	Incoming public key PublKeyIn is given in big endianness as butes array, containing
	concatenation PC||X||Y, where: 
		PC - point control single byte, defining the type of point: 0x4 - uncompressed,
		     other types not supported. 
		X,Y - EC point coordinates of public key,  size of X and Y equal to size of EC modulus,  
		      Size of buffers for X and also Y must be equal ModSizeInBytes.
 
   @param[in]  ECPKI_DomainID  - The enumerator variable defines current EC domain.
   @param[in]  PublKeyIn_ptr   - The pointer to private key data.
   @param[in]  PublKeySizeInBytes - Size of private key data in bytes 2*modulusSize + 1byte.
   @param[in]  CheckMode       - The parameter defining what checking of public key is necessary:
                                 preliminary check - 0, partly check - 1, full check - 2 .
   @param[out] UserPublKey_ptr - A pointer to the private key structure.
   @param[in]  TempBuff_ptr    - A pointer to the temp buffer structure for build function.

   @return CRYSError_t - CRYS_OK,
			CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_IN_PTR_ERROR
			CRYS_ECPKI_BUILD_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR
			CRYS_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR
			CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR
			CRYS_ECPKI_BUILD_KEY_INVALID_COMPRESSION_MODE_ERROR
*/
CEXPORT_C CRYSError_t _DX_ECPKI_BuildPublKey(
					CRYS_ECPKI_DomainID_t        DomainID,	          /*in*/				
					DxUint8_t	            *PublKeyIn_ptr,       /*in*/									
					DxUint32_t                   PublKeySizeInBytes,  /*in*/
					EC_PublKeyCheckMode_t        CheckMode,           /*in*/
					CRYS_ECPKI_UserPublKey_t    *UserPublKey_ptr,     /*out*/
					CRYS_ECPKI_BUILD_TempData_t *TempBuff_ptr         /*in*/ )
{
	/* FUNCTION DECLARATIONS */
	
	/* the private key structure pointer */
	CRYS_ECPKI_PublKey_t *PublKey_ptr;
	
	/* structure containing part of domain information */
	CRYS_ECPKI_DomainInfo_t   DomainInfo;
	
	/* EC modulus size in bytes*/
	DxUint32_t  ModSizeInBytes; 
	
	/* Point control PC ( 1 byte) PC and PC1 = PC&6*/
	DxUint8_t     PC, PC1;
	
	/* the Error return code identifier */
	CRYSError_t Error; 
	
	/* FUNCTION LOGIC */
	
	/* ............... if not supported exit .............................. */
	/* -------------------------------------------------------------------- */   
	
	RETURN_IF_ECPKI_UNSUPPORTED( DomainID, PublKeyIn_ptr, PublKeySizeInBytes, CheckMode, 
				UserPublKey_ptr, TempBuff_ptr, PublKey_ptr,                                 
				DomainInfo.EC_ModulusSizeInBits, ModSizeInBytes, PC, PC1, 
				Error, Error, Error, Error, Error, Error, Error, 
				Error, Error, Error, Error );   
				  
#ifndef CRYS_NO_ECPKI_SUPPORT  

	/* ...... checking the validity of the User Private Key pointer ......... */
	if( UserPublKey_ptr == DX_NULL ) 
		return  CRYS_ECPKI_BUILD_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR;  
	
	/* ...... checking the key database handle pointer ....................  */
	if(PublKeyIn_ptr == DX_NULL)    
	   return  CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_IN_PTR_ERROR;   	   
	
	/* ...... checking the EC domain ID.................... */
	if(DomainID >= CRYS_ECPKI_DomainID_OffMode)
		return  CRYS_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR;  
	
	if( CheckMode >= PublKeyChecingOffMode )
		return  CRYS_ECPKI_BUILD_KEY_INVALID_CHECK_MODE_ERROR;
	
	if(CheckMode != CheckPointersAndSizesOnly && TempBuff_ptr == DX_NULL)
		return  CRYS_ECPKI_BUILD_KEY_INVALID_TEMP_BUFF_PTR_ERROR;
	
	if(DxCcAcl_IsBuffAccessOk(ACCESS_READ, PublKeyIn_ptr, PublKeySizeInBytes) || 
	DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, UserPublKey_ptr, sizeof(CRYS_ECPKI_UserPublKey_t)) ||
	((TempBuff_ptr != DX_NULL) && DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, TempBuff_ptr, sizeof(CRYS_ECPKI_BUILD_TempData_t)))){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/* ...... Initializations  ............... */ 
	
	Error = CRYS_OK;
	
	/* get domain info and modulus size */
	Error = LLF_ECPKI_GetDomainInfo(DomainID, &DomainInfo);
	
	if(Error != CRYS_OK) 
		return Error;
	
	ModSizeInBytes = (DomainInfo.EC_ModulusSizeInBits + 7)/8;
	
	/* point control */
	PC = PublKeyIn_ptr[0];
	PC1 = PC & 0x6;
	
	/* preliminary check key size */
        if( PublKeySizeInBytes != 2*ModSizeInBytes + 1)
   		return  CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR; 

	/* ...... copy the buffers to the key handle structure ................ */
	/* -------------------------------------------------------------------- */
	
	/* setting the pointer to the key database */
	PublKey_ptr = (CRYS_ECPKI_PublKey_t *)((void*)UserPublKey_ptr->PublKeyDbBuff);
	
	/* clear the public key db */
	DX_VOS_MemSet((DxUint8_t*)UserPublKey_ptr , 0 , sizeof(CRYS_ECPKI_UserPublKey_t));
	
	/* Set DomainID into Builded public key */
	PublKey_ptr->DomainID = DomainID;
	
	if(PC1 == CRYS_EC_PointUncompressed || PC1 == CRYS_EC_PointHybrid)  /*  PC1 = 4 or PC1 = 6 */
	{
		/* Reverse mem copy public key Xin to X, Yin to Y */
		Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PublKey_ptr->PublKeyX, sizeof(PublKey_ptr->PublKeyX),
							PublKeyIn_ptr + 1, ModSizeInBytes);
		if(Error != CRYS_OK)
			return Error;
		
		Error = CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords(PublKey_ptr->PublKeyY, sizeof(PublKey_ptr->PublKeyY),
							PublKeyIn_ptr + 1 + ModSizeInBytes, ModSizeInBytes);
		if(Error != CRYS_OK)
			return Error;                              
	}
	else
		return CRYS_ECPKI_BUILD_KEY_INVALID_COMPRESSION_MODE_ERROR;
	
	
	/* initialize LLF public key database   */
	/*--------------------------------------*/
	LLF_ECPKI_InitPubKeyDb( PublKey_ptr);
	
	
	/*................ checking the public key ............................*/
	/*---------------------------------------------------------------------*/
	if(CheckMode >= ECpublKeyPartlyCheck){
		/* full checking of the public key validity */
		Error = LLF_ECPKI_CheckPublKeyCall(PublKey_ptr, CheckMode, (DxUint32_t*)TempBuff_ptr);
	}
	else{
		/* partly checking of the public key validity */
		Error = LLF_ECPKI_CheckPublKeySize(DomainID, PublKey_ptr->PublKeyX, 2*ModSizeInBytes);
	}
	
	if(Error != CRYS_OK){
		/* if created structure is not valid - clear it and exit */
		DX_VOS_MemSet(UserPublKey_ptr, 0, sizeof(CRYS_ECPKI_UserPublKey_t));    	    
		return Error;
	}
	
	/* ................ set the private key validation tag ................... */  
	UserPublKey_ptr->valid_tag = CRYS_ECPKI_PUBL_KEY_VALIDATION_TAG;
	
	
	/* ................. end of the function ................................. */
	/* ----------------------------------------------------------------------- */
	
	return Error;
	

#endif /* !CRYS_NO_ECPKI_SUPPORT */
 
 } /* End of _DX_ECPKI_BuildPublKey() */



 /***********************************************************************************
  *                     CRYS_ECPKI_ExportPublKey function                           *
  ***********************************************************************************/
 /**
   @brief The function converts an existed public key into the big endian and outputs it.

		  The function performs the following steps:
		  - checks input parameters,
		  - Converts the X,Y coordinates of public key EC point to big endianness.
		  - Sets the public key as follows:
			   In case "Uncompressed" point:  PubKey = PC||X||Y, PC = 0x4 - single byte;
        		   In other cases returns an error.
		  - Exits.

        	  NOTE: - At this stage supported only uncompressed point form,
        		- Size of output X and Y coordinates is equal to ModSizeInBytes.
     
   @param[in]  UserPublKey_ptr -   A pointer to the public key structure initialized by CRYS. 
   @param[in]  Compression     -   An enumerator parameter, defines point compression.
   @param[out] ExternPublKey_ptr - A pointer to the buffer for export the public key bytes array in big 
                                   endian order of bytes. Size of buffer must be not less than:
				   2*ModSiseInBytes+1 bytes.   
   @param[in/out] PublKeySizeInBytes - A pointer to size of user passed public key buffer (in) and
				   the actual size of exported public key (out).

   @return CRYSError_t - CRYS_OK,
                         CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR      
                         CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR       
                         CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR    
                         CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR      
                         CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR          
                         CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_DOMAIN_ID_ERROR  
*/
CEXPORT_C CRYSError_t CRYS_ECPKI_ExportPublKey(
			CRYS_ECPKI_UserPublKey_t      *UserPublKey_ptr,       /*in*/
			CRYS_ECPKI_PointCompression_t  Compression,           /*in*/
			DxUint8_t		      *ExternPublKey_ptr,     /*in*/
			DxUint32_t                    *PublKeySizeInBytes_ptr /*in/out*/)
{
	/*-------------------- FUNCTION DECLARATIONS ------------------------*/
	
	/* the private key structure pointer */
	CRYS_ECPKI_PublKey_t *PublKey_ptr;
	
	/* Domain ID and DomainInfo - structure containing part of domain information */
	CRYS_ECPKI_DomainID_t       DomainID;
	CRYS_ECPKI_DomainInfo_t   DomainInfo; 
	
	/* EC modulus size in words and in bytes*/
	DxUint32_t   ModSizeInBytes; 
	
	/* the Error return code identifier */
	CRYSError_t Error; 
	
	/* FUNCTION LOGIC */
	
	/* .................. if not supported exit .............................. */
	RETURN_IF_ECPKI_UNSUPPORTED( UserPublKey_ptr, Compression, ExternPublKey_ptr,  
			      PublKeySizeInBytes_ptr, PublKey_ptr, DomainInfo.EC_ModulusSizeInBits,                                 
			      DomainID, ModSizeInBytes, Error, Error, Error, Error, Error, Error,
			      Error, Error, Error, Error, Error, Error, Error, Error ); 
			      
	
	#ifndef CRYS_NO_ECPKI_SUPPORT  
	
	/* .................. INITIALIZATIONS  ................................. */ 
	   
	Error = CRYS_OK; /* Error initialization */
	
	
	/*............. Checking input parameters   ..............................*/
	
	/* ...... checking the key database handle pointer ....................  */
	if( UserPublKey_ptr == DX_NULL )   
		return  CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR;   	   
	 
	/* ...... checking the validity of the extern Public Key pointer ........ */
	if( ExternPublKey_ptr == DX_NULL ) 
		return  CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR;   	
	
	/* ... checking the validity of the extern Public Key size pointer ...... */
	if( PublKeySizeInBytes_ptr == DX_NULL ) 
		return  CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR;   		
	
	
	PublKey_ptr = (CRYS_ECPKI_PublKey_t *)((void*)UserPublKey_ptr->PublKeyDbBuff); 
	DomainID = PublKey_ptr->DomainID;
	
	/* ...... checking the EC domain ID...................................... */
	if( DomainID >= CRYS_ECPKI_DomainID_OffMode )
		return  CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_DOMAIN_ID_ERROR;    
	
	if (DxCcAcl_IsBuffAccessOk(ACCESS_READ, UserPublKey_ptr, sizeof(CRYS_ECPKI_UserPublKey_t)) || 
	    DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, ExternPublKey_ptr, *PublKeySizeInBytes_ptr)  || 
	    DxCcAcl_IsBuffAccessOk(ACCESS_READ_WRITE, PublKeySizeInBytes_ptr, sizeof(DxUint32_t))){
		return CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR;
	}
	
	/*------------------------ FUNCTION LOGIC --------------------------------*/
	
	/* get domain info and modulus size */
	Error = LLF_ECPKI_GetDomainInfo(DomainID, &DomainInfo); 
	
	if(Error != CRYS_OK)
		return Error;
	
	ModSizeInBytes = (DomainInfo.EC_ModulusSizeInBits + 7)/8; 
	
	/* Convert public key to big endianness export form */
	
	switch( Compression ){
	
	case CRYS_EC_PointUncompressed:
		/* check uzer passed size of buffer for public key*/
		if( *PublKeySizeInBytes_ptr < 2*ModSizeInBytes + 1 )
			return  CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR;
		
		/* Set ExternPublKey[0] = PC = 4 */
		ExternPublKey_ptr[0] = 4;
		
		Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
			ExternPublKey_ptr + 1, 4*((ModSizeInBytes+3)/4),
			PublKey_ptr->PublKeyX, ModSizeInBytes );
		if(Error != CRYS_OK)
			return Error;
		Error = CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes(
			ExternPublKey_ptr + 1 + ModSizeInBytes, 4*((ModSizeInBytes+3)/4),
			PublKey_ptr->PublKeyY, ModSizeInBytes ); 
		if(Error != CRYS_OK)
			return Error;
		
		/* Set PublKeySizeInBytes */
		*PublKeySizeInBytes_ptr = 2*ModSizeInBytes + 1;
		break;
	
	case CRYS_EC_PointHybrid:
	case CRYS_EC_PointCompressed:
	case CRYS_EC_PointContWrong:
	case CRYS_EC_PointCompresOffMode:
	default:
	
		return CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR;
	}   
 
   return Error;

#endif /* !CRYS_NO_ECPKI_SUPPORT */
 
 } /* End of CRYS_ECPKI_ExportPublKey */
 
 
