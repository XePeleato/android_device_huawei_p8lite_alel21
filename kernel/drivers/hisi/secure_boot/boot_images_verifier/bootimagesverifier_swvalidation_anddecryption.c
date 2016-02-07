
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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/
#include "secureboot_basetypes.h"
#include "rsa.h"
#include "sb_hash.h"
#include "bootimagesverifier_def.h"
#include "secureboot_error.h"
#include "bootimagesverifier_error.h"
#include "util.h"
#include "secureboot_gen.h"
#include "bootimagesverifier_parser.h"
#include "systemdep.h"

#include "crypto_driver_defs.h" 
#include "crypto_driver.h" 
#include "nvm.h" 
#include "nvm_no_sep.h" 
#include "bootimagesverifier_swvalidation_anddecryption.h"

#include "dx_pal_log.h"


/************************ Defines ******************************/
void UTIL_MemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );
DxUint32_t UTIL_MemCmp( DxUint8_t *buff1_ptr , DxUint8_t *buff2_ptr , DxUint32_t size );
void UTIL_MemSet( DxUint8_t *buff_ptr , DxUint8_t val , DxUint32_t size );

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/


/************************ Internal Functions ******************************/
static DxUint32_t UTIL_INVERSE_UINT32_BYTES (DxUint32_t val)
{
#ifdef BIG__ENDIAN
  return  (val);
#else

  return  ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24\
											 ));
#endif
}

/************************ Public Functions ******************************/
/** 
 * @brief This function is responsible to verification of the SW components.
 *        The function will go over the SW components load each component,
 *        compute its HASH and compare it with the HASH saved in the VRL.
 *         
 *
 * @param[in] flashRead_func - User's Flash read function used to read data from the flash to memory location
 * @param[in] userContext - User's context for the usage of flashRead_func
 * @param[in] hwBaseAddress - base address for the CC HW engines 
 * @param[in] swCompsData_ptr - s/w comps data, pointers to VRL locations of the s/w comps HASH data and the s/w comps 
 *            additional data (storage addresses & length)
 * @param[out] isSecVrlExist - indicator if secondary VRL exists
 * @param[in] workspace_ptr - temporary buffer to load the SW components to (SW components without 
 *            loading address).
 * @param[in] workspaceSize - the temporary buffer size in bytes
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */
DxError_t DX_BIV_SWCompValidation(DX_SB_FlashRead flashRead_func,
				  void *userContext,
				  DxUint32_t hwBaseAddress,                                  
				  VRL_Parser_CompsData_t *swCompsData_ptr, 
				  DxUint8_t *isSecVrlExist,
				  DxUint32_t *workspace_ptr,
				  DxUint32_t workspaceSize)
{
/* error variable */
DxError_t error = DX_OK;

/* record structure */  
DxUint32_t *vrlCurrentRecordHASHInfo_ptr, *vrlCurrentRecordAddInfo_ptr;

/* internal index */
DxUint32_t i = 0;

/* internal pointer for the VRL buffer */
DxUint32_t *currRecord_ptr = DX_NULL;
DxUint32_t *currRecordInfo_ptr = DX_NULL;

/* Hash output size */
DxUint16_t hashOutputSize = 0;

/* load address */
DxUint32_t loadAddress;

/* temporary buffer */
DxUint32_t VRLBufferSize = 0, swCompWorkspaceSize;

DxUint32_t *swCompWorkspace_ptr;

/* AES key buffer */
AES_Key_t AESKey;

/*------------------
    CODE
-------------------*/
/* Parse the VRL to get pointers to the SW image data (hash + load addresses and storage addresses + size),
The WSP contains the vrl data at this stage */
error = DX_BIV_ParseVrlGetSWComponentData(workspace_ptr, swCompsData_ptr, isSecVrlExist);

if (error != DX_OK)
	return error;

currRecord_ptr = swCompsData_ptr->swHASHData_ptr;
currRecordInfo_ptr = swCompsData_ptr->swAdditionalData_ptr;


DX_BIV_ParseVrlGetVrlSize(workspace_ptr, &VRLBufferSize);

if (workspaceSize > VRLBufferSize) {
	swCompWorkspace_ptr = workspace_ptr + VRLBufferSize;
	swCompWorkspaceSize = workspaceSize - VRLBufferSize*sizeof(DxUint32_t);
} else {
	swCompWorkspace_ptr = workspace_ptr;
	swCompWorkspaceSize = 0;
}

/* Step 1 - Get the hash algorithm, according to which the s/w component hash size is set */
/*----------------------------------------------------------------------------------------*/
/* Get hash output size in bytes */  
hashOutputSize = swCompsData_ptr->hashSizeWords* sizeof(DxUint32_t);

/* Step 2 - If SW components are encrypted, get the aes key from OTP */
/*---------------------------------------------------------------------*/
if (swCompsData_ptr->isSwComponentEncrypted) {
	NVM_ReadAESKey(hwBaseAddress, AESKey);
}

/* Step 3 - In a loop, read the SW component data, calculate the HASH and compare */
/*--------------------------------------------------------------------------------*/
for (i = 0; i < swCompsData_ptr->numOfSwComps; i++ ) {
	/* Point to the current s/w record HASH data */
	vrlCurrentRecordHASHInfo_ptr = currRecord_ptr;

	/* Point to the next record HASH data (each record size is HASH size + load address size ) */
	currRecord_ptr += hashOutputSize/sizeof(DxUint32_t) + 1;

	/* Point to current record additional info */
	vrlCurrentRecordAddInfo_ptr = currRecordInfo_ptr;

	/* Point to the next record additional data (record info size is fixed - storage address + length) */
	currRecordInfo_ptr += sizeof(VRL_ParamRecordInfo_t)/sizeof(DxUint32_t);

	/* calculate the HASH on the current block */             
	error = DX_BIV_CalcHASHOnSWRecDecryptAndCompare(flashRead_func, userContext,/* Flash Read function */
						 DX_NULL, DX_NULL,	     /* post hash operation not needed here */
						 hwBaseAddress,		     /* CC base address */
						 hashOutputSize,	     /* HASH output size */
						 vrlCurrentRecordHASHInfo_ptr, /* pointer to Hash and load address */
						 (VRL_ParamRecordInfo_t*)vrlCurrentRecordAddInfo_ptr, /* comp additional info */   
						 &loadAddress, 			/* output load address */
						 &AESKey,			/* code encryption key for sw component */
						 swCompsData_ptr->isSwComponentEncrypted, /* Indicator if SW image is encrypted */
						 swCompsData_ptr->nonce, 	/* nonce */
						 workspace_ptr, workspaceSize);	/* workspace & workspaceSize to load the SW component into */       

	if (error != DX_OK)
		return error;
}

return DX_OK;
}

/*DxUint32_t UTIL_INVERSE_UINT32_BYTES (DxUint32_t val)
{
#ifdef BIG__ENDIAN
  return  (val);
#else

  return  ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) );
#endif
}*/

/** 
 * @brief This function load the SW component to RAM, calculates HASH on it and compares the
 *        result with the given HASH (taken from the VRL).
 *        This function calculates the HASH simultaneously to reading data from the Flash. 
 *         
 *
 * @param[in] preHashflashRead_func - User's Flash read function used to read data from the flash to memory location. 
 *	      this is the first function used (before the hash)
 * @param[in] preHashUserContext - User's context for the usage of preHashflashRead_func
 * @param[in] postHashflashRead_func - User's Flash read function used to copy the data to the final location 
 *	      (used only in cases that the hash cannot be performed in th final location)
 * @param[in] postHashUserContext - User's context for the usage of postHashflashRead_func
 * @param[in] hwBaseAddress - base address for the CC HW engines
 * @param[in] hashOutputSize - hash output size (defined according to given HASH algorithm) in bytes
 * @param[in] recHashData_ptr - a pointer to record hash data and memory load address
 * @param[in] recHashData_ptr - a pointer to record data (storage address and length) 
 * @param[in] aesKey_ptr - a pointer to code encryption key for sw component 
 * @param[in] isSwComponentEncrypted - indicator if SW image is encrypted
 * @param[in] nonce - nonce used in composing iv for sw components decryption
 * @param[out] swCompLoadAddress - the final address that the sw comp was loaded to
 * @param[in] workspace_ptr - temporary buffer to load the SW components to (SW components without 
 *            loading address).
 * @param[in] workspaceSize - the temporary buffer size in bytes, minimal allowed size is 
 *            DX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES
 *
 * @return DxError_t - On success the value DX_OK is returned, 
 *         on failure - a value from BootImagesVerifier_error.h
 */

DxError_t DX_BIV_CalcHASHOnSWRecDecryptAndCompare(DX_SB_FlashRead preHashflashRead_func,
					   void *preHashUserContext,
					   DX_SB_FlashRead postHashflashRead_func,
					   void *postHashUserContext,
					   DxUint32_t hwBaseAddress,
					   DxUint32_t hashOutputSize,
					   DxUint32_t *recHashData_ptr,
					   VRL_ParamRecordInfo_t *recAddInfo_ptr,  
					   DxUint32_t *swCompLoadAddress,
					   AES_Key_t *aesKey_ptr,			
					   DxUint8_t isSwComponentEncrypted,
					   VRL_Parser_tableNonce_t nonce,  
					   DxUint32_t *workspace_ptr,
					   DxUint32_t workspaceSize)
{
/* error variable */
DxError_t error = DX_OK;
/* SW component temp size */
DxUint32_t sizeToReadInBytes = 0, currentSize = 0, sizeOfTotalSections = workspaceSize;

/* a flag indicating if its last operation or not */
DxUint8_t isLastBlock = DX_FALSE;

/* HASH result buffer */
HASH_Result_t HASHResult; 

/* AES IV buffer */
AES_Iv_t AESIv; 

DxUint32_t tempWord;

/* the address in the Flash to read from */
DxUint32_t currStoreAddress = 0, currLoadAddress = 0, finalLoadAddress = 0; 

/* temp buf pointers */
DxUint32_t *workRam1=DX_NULL, *workRam2=DX_NULL, *temp_ptr;

/* flag indicating if to load to memory or use temporary memory */
DxUint8_t  isLoadMemory = DX_TRUE;

/* set default crypto mode to hash only (wo encryption) */
CryptoDriverMode_t cryptoDriverMode = CRYPTO_DRIVER_HASH_MODE; 

/*------------------
    CODE
-------------------*/

/* In order to improve performance the Loading from Flash will be done simultaneously
   to calculating the HASH */  
/* Initialize parameters */
/*-----------------------*/

/*validate the s/w component size - we do not support size zero*/
if (recAddInfo_ptr->Len == 0){
	DX_PAL_LOG_DEBUG("SW comp size is NULL\n");
	return DX_BOOT_IMG_VERIFIER_SW_COMP_SIZE_IS_NULL;
}

/* Get the memory load address */
currLoadAddress = *(recHashData_ptr + hashOutputSize/sizeof(DxUint32_t));

/* Set the output address */
*swCompLoadAddress = currLoadAddress;

/* In case the s/w component should not be loaded, or the loading is done after calculating the HASH,
   the function will use the temporary memory given */
if (currLoadAddress == DX_SW_COMP_NO_MEM_LOAD_INDICATION || postHashflashRead_func != DX_NULL) {
	isLoadMemory = DX_FALSE;

	/* The workspace minimum size must be at least DX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES,
	   if its not the function will return error (if temp memory should be used) */
	if (workspaceSize < DX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES){
		DX_PAL_LOG_DEBUG("workspace size too small\n");	
		return DX_BOOT_IMG_VERIFIER_WORKSPACE_SIZE_TOO_SMALL;
	}

	/* Divide the workspace into 2 buffers, in order to allow reading and calculating HASH 
	 simultaneously , each buffer size is DX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES/2 */
	workRam1 = workspace_ptr; /* Size of this section is DX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES/2 */
	workRam2 = workspace_ptr + (DX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES/2)/sizeof(DxUint32_t);     

	/* Starting with the first buffer */
	temp_ptr = workRam1;
	if (postHashflashRead_func != DX_NULL) {
		finalLoadAddress = currLoadAddress;
	}
} else { /* point to the current memory address */
	temp_ptr = (DxUint32_t*)currLoadAddress;    
}

sizeOfTotalSections = DX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES;  


/* Set the storage address */  
currStoreAddress = recAddInfo_ptr->StoreAddr;

/* Set the s/w component size */
currentSize = recAddInfo_ptr->Len * sizeof(DxUint32_t);

/* Fixed size to read */
sizeToReadInBytes = sizeOfTotalSections / 2;

/* Initialize the crypto process  */
/*--------------------------------*/  
  
if (isSwComponentEncrypted) {

	/* AES IV compose of : nonce[0], nonce[1], Load Addres, 0 */
	UTIL_MemSet((DxUint8_t*)AESIv, 0, AES_IV_COUNTER_SIZE_IN_BYTES);
	
	tempWord = UTIL_INVERSE_UINT32_BYTES(nonce[0]);
	UTIL_MemCopy((DxUint8_t*)&AESIv[0], (DxUint8_t*)&tempWord, sizeof(DxUint32_t));
	
	tempWord = UTIL_INVERSE_UINT32_BYTES(nonce[1]);
	UTIL_MemCopy((DxUint8_t*)&AESIv[1], (DxUint8_t*)&tempWord, sizeof(DxUint32_t));
	
	tempWord = UTIL_INVERSE_UINT32_BYTES(currLoadAddress);
	UTIL_MemCopy((DxUint8_t*)&AESIv[2], (DxUint8_t*)&tempWord, sizeof(DxUint32_t));

	cryptoDriverMode = CRYPTO_DRIVER_HASH_AES_CTR_MODE;
}

/* initialize the AES and HASH */
SBROM_CryptoInit(hwBaseAddress, aesKey_ptr, &AESIv, cryptoDriverMode); 


/* Read the SW component from the flash in blocks and calculate HASH on it */
/*-------------------------------------------------------------------------*/  

/* In a loop - read to buffer1 start the HASH on it and continue with buffer2 */
while (currentSize > 0) {
	/* In case the remaining bytes to read are less than the size of the buffer */
	if (sizeToReadInBytes > currentSize || sizeToReadInBytes == currentSize) {
		/* Set the size to read to the size of the remaining bytes */
		sizeToReadInBytes = currentSize;

		/* Set the flag indicating this is the last operation */
		isLastBlock = DX_TRUE;
	}

	/* Read a block from the flash, according to the size of the buffer */
	error = preHashflashRead_func(currStoreAddress, (DxUint8_t*)temp_ptr, sizeToReadInBytes, preHashUserContext);
	if (error != DX_OK)
		return error;

	/* For each chunk that is read , perform hash operation and decryption (if needed) */
	SBROM_CryptoUpdateBlock (hwBaseAddress,
				temp_ptr,                
				temp_ptr,                
				sizeToReadInBytes,
				isLastBlock,
				cryptoDriverMode);

	/* In case a post hash copying operation is needed, do it after the hash calculation */
	if (postHashflashRead_func != DX_NULL) {
		/* read from the temporary location (on the WSP) to the final location */
		error = postHashflashRead_func((DxUint32_t)temp_ptr, (DxUint8_t*)finalLoadAddress, sizeToReadInBytes, postHashUserContext);
		if (error != DX_OK)
			return error;

		finalLoadAddress = finalLoadAddress + sizeToReadInBytes; 
	}

	/* Update the size to read, Flash address and buffer pointer */
	currentSize = currentSize - sizeToReadInBytes;
	currStoreAddress = currStoreAddress + sizeToReadInBytes;

	if (isLoadMemory) { /* In case load to memory */
		/* advance the load address pointer */
		temp_ptr = temp_ptr + sizeToReadInBytes/sizeof(DxUint32_t);      
	} else { /* if using temp memory */
		/* switch buffer pointers */
		if ( temp_ptr == workRam1 )
			temp_ptr = workRam2;
		else
			temp_ptr = workRam1;  
	}    
}  

/* Finish the HASH operation and get the results */
/*-----------------------------------------------*/    
/* get the results */
SBROM_CryptoFinish(hwBaseAddress, HASHResult);

	
/* Compare the HASH result with the SW component given HASH */
/*----------------------------------------------------------*/
error = UTIL_MemCmp((DxUint8_t*)recHashData_ptr, (DxUint8_t*)HASHResult, hashOutputSize);
if (error != DX_TRUE){
	DX_PAL_LOG_DEBUG("SW comp failed verification\n");
	return DX_BOOT_IMG_VERIFIER_SW_COMP_FAILED_VERIFICATION;
}

return DX_OK;

}/* End of DX_BIV_CalcHASHOnSWRecAndCompare */
