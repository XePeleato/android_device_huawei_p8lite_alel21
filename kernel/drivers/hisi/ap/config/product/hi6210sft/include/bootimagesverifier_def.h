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



#ifndef _BOOT_IMAGES_VERIFIER_DEF_H
#define _BOOT_IMAGES_VERIFIER_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/* General definitions */
/***********************/

/* VRL Magic number */
#define DX_VRL_MAGIC_NUMBER_DEFAULT_VALUE 0xE59FF052

/* VRL structure is as follows :
  
structure      Field                         Size in words          Description
=========      =====                         =============          ===========
Header         Magic number                  1                        
	       VRL size                      1                      num of components + offset to signature  
	       Flags                         1                      identify HASH alg + RSA alg + if secondary VRL exist + if SW revocation supported

Pub key        N                             64                     public key
	       Np                            5                      barrett n' value
	       
Sw version     Cur version                   1                      current version if SW revocation supported else 0
	       Min version                   1                      Min allowed version if SW revocation is supported

Sec key HASH   Sec Key HASH                  8 || 4                 HASH(N || Np), size depends on HASH alg, on Prim VRL, if sec VRL exist, if not filled with 0 according to hash size

SW Comp        HASH                          8 || 4                 HASH of s/w component, size depends on HASH alg     
(* num of      MEM load address              1                      memory Load address     
components )        

VRL signature  RSA signature                 64                     RSA PSS signature

VRL address    Sec VRL address               1                      Sec VRL address, if Sec VRL not exist = 0 

SW comp params storage address               1                      Storage address to where the s/w component is read from 
(* num of      size of s/w comp              1                      Size in words 
components )



The VRL primary table's layout is as follows:
=============================================
1. Secondary VRL exist :
	   Header
	   Pub key
	   Sw version  (if sw revocation is not supported it will be set to 0)
	   Sec key HASH (if secondary VRL does not exist, filled with 0)
	   M * SW Comp (M is the number of SW components)
	   VRL signature
	   VRL address
	   M * SW comp params (M is the number of SW components)
	   


The VRL secondary table's layout is as follows:
===============================================
1. Secondary VRL :           
	   Header
	   Pub key
	   Sw version  (set to 0)           
	   Sec key HASH (filled with 0)
	   M * SW Comp (M is the number of SW components)
	   VRL signature
	   VRL address
	   M * SW comp params (M is the number of SW components)
*/           


/********* VRL structure definitions ***********/
/* SB HASH length can be SHA256 length or SHA256 truncated to 128 bits, this define is set according 
   to product */

#ifndef DX_SB_HASH_LENGTH_IN_WORDS
#error No Hash size is defined
#endif

#define DX_SB_HASH_LENGTH_IN_BYTES 	DX_SB_HASH_LENGTH_IN_WORDS * sizeof(DxUint32_t)

/*RSA defenitions*/
#define SB_RSA_MOD_SIZE_IN_WORDS 64
#define SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS 5

/* VRL Flag offset */
#define SB_VRL_FLAG_OFFSET_IN_WORDS 2

/* 1 . VRL header structure */
typedef struct {
	/* Magic number to validate VRL */
	DxUint32_t magicNumber;  

	/* Bits [0:15] holds direct offset in words to the VRL signature, 
	   Bits [16:31] holds the number of sw components */   
	DxUint32_t vrlSize;

	/* Bits [0:7] holds HASH algorithm identifier 
	   Bits [8:15] holds pub key algorithm identifier
	   Bits [16] is SW revocation supported
	   Bits [17] is Secondary VRL exist
	   Bits [18:31] reserved */
	DxUint32_t vrlFlags;

}VRL_Header_t;

/* 2. Public key data structure */
typedef struct {
	/* N public key, big endian representation */
	DxUint32_t N[SB_RSA_MOD_SIZE_IN_WORDS];

#ifndef DX_NP_NOT_USED_IN_SB
	/* Np (Barrett n' value) */
	DxUint32_t Np[SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS];  
#else  
	/* H value */
	DxUint32_t H[SB_RSA_MOD_SIZE_IN_WORDS + 1];  
#endif 

}VRL_N_Params_t;


/* 3. SW version structure */
typedef struct {
	/* Current SW version */
	DxUint32_t currSwVersion;

	/* Minimum allowed SW version */
	DxUint32_t minAllowedSwVersion;

}VRL_SW_Version_t;


/* 4. Record HASH structure */
typedef struct {
	/* HASH calculated on the record */
	DxUint32_t SwHashResult[DX_SB_HASH_LENGTH_IN_WORDS];  

	/* Memory load address */
	DxUint32_t    memLoadAddr;

}VRL_HashRecordInfo_t; 


/* 5. Secondary HASH typedef */
typedef DxUint32_t*  VRL_SecHASHPtr_t;


/* 6. Signature structure */
typedef struct {
	/* RSA PSS signature */
	DxUint32_t sig[SB_RSA_MOD_SIZE_IN_WORDS];

}VRL_Sign_t;   


/* 7. Secondary address typedef */
typedef struct {
	/* Secondary VRL storage address*/
	DxUint32_t SecVrlAddress;

}VRL_SecAddress_t;


/* 8. SW component additional parameters */
typedef struct {
	/* Storage address */
	DxUint32_t    StoreAddr;

	/* Size of the sw component in words */
	DxUint32_t    Len;

}VRL_ParamRecordInfo_t; 

/********* End of VRL structure definitions ***********/


/********* Supported algorithms definitions ***********/

/* HASH supported algorithms */
typedef enum {
	/* HASH SHA 256 output */
	HASH_SHA256_Alg_Output = 0x01,

	/* HASH SHA 256 output truncated to 128 bits */
	HASH_SHA256_Alg_128_Output = 0x02,

	/* Last */
	HASH_Last              = 0x7FFFFFFF 

}VRL_HashAlg_t;


/* RSA supported algorithms */
typedef enum {
	/* RSA PSS 2048 after HASH SHA 256 */
	RSA_PSS_2048           = 0x01,
	RSA_PKCS15_2048	       = 0x02,

	/* Last */
	RSA_Last               = 0x7FFFFFFF

}VRL_RSAAlg_t;

/********* End of Supported algorithms definitions ***********/


/********* Function pointer definitions ***********/

/* Flash Read function pointer defintion, this function is used inside the secure boot APIs 
   To read data from the Flash */

typedef DxUint32_t (*DX_SB_FlashRead) (DxUint32_t flashAddress,	/* Flash address to read from */
				       DxUint8_t *memDst,	/* memory destination to read the data to */
				       DxUint32_t sizeToRead,	/* size to read from Flash (in bytes) */
				       void* context);		/* context for user's needs */


/* Flash Write functions pointer definition, this function is used inside the secure boot APIs 
   to write data to the Flash */
typedef DxUint32_t (*DX_SB_FlashWrite) (DxUint32_t flashAddress, /* Flash address to write to */
					DxUint8_t *memSrc,	 /* memory source to write the data from */
					DxUint32_t sizeToWrite,	 /* size to write to Flash (in bytes) */ 
					void* context);		 /* context for user's needs */

/* Get OTP HASH function pointer definition, this function is used to retrieve the OEM key HASH
   from OTP */
typedef DxUint32_t (*Dx_SB_GetOTPHASH) (DxUint32_t hwBaseAddress, /* CC base address*/
					DxUint32_t keyIndex, /* The OTP index*/ 
					DxUint32_t *hashBuffer /* The HASH output*/);

/********* End of Function pointer definitions ***********/

/* VRL list MAX size */
#define VRL_LIST_MAX_SIZE                       20

/* If this value is set in the OTP index field than the HASH comparison is done with external HASH 
   (and not HASH taken from the OTP) */
#define DX_SB_HASH_INDEX_NOT_USE_INTERNAL_HASH  0xFFFFFFFF





/* Definitions for additional inputs to the SW verification function */
typedef struct {
	/* VRL address in the storage */
	DxUint32_t VRL_Address;

	/* Magic number, optional, if set to 0, DX_DEFAULT_VALUE will be used */
	DxUint32_t Magic_Number;

	/* OTP key index */
	DxUint32_t OTP_Key_Index;

	/* External HASH value */
	DxUint32_t Ext_HASH_Value[DX_SB_HASH_LENGTH_IN_WORDS];

}SW_Verification_Data_t;

/* VRL list definition, each cell contains data regarding a specific VRL (address, magic number etc.) */
typedef SW_Verification_Data_t DX_BIM_VRL_TABLE_t[VRL_LIST_MAX_SIZE];


/* HASH minimum size */
#define DX_HASH_MIN_SIZE_IN_BYTES               64 


/* Indication to load or not the component to memory */
#define DX_SW_COMP_NO_MEM_LOAD_INDICATION       0xFFFFFFFFUL

#define DX_ROM_PUBKEY_ARRAY_START_ADDR          0x1000

#ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED
#define ADDITIONAL_DATA_SIZE_IN_BYTES           128
#define ADDITIONAL_DATA_SIZE_IN_WORDS		ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t)
#endif

/* Definitions for HASH and SW data outputs */
/********************************************/
/* The data structure is combined of: 
   1. a header with buffer size info
   2. For each VRL:
      a. VRL storage address
      b. SW version
      c. number of SW components in VRL
      d. SW components HASH values
*/         

/* Structure header , contains the total buffer size and the available free size */
typedef struct {
	/* */
	/* total buffer size */
	DxUint32_t Total_BuffSize;

	/* remaining size */
	DxUint32_t Remaining_BuffSize;

}SB_HASH_SwRev_Data_Header_t;

/* Each VRL HASH output data structure */
typedef struct {
	/* VRL's storage address (used as the identifier for the VRL) */
	DxUint32_t Vrl_Address;

	/* SW version */
	DxUint32_t Sw_Version;

	/* Num of components in VRL */
	DxUint32_t Num_Of_Comps;

} SB_HASH_SwRev_VRL_Header_t;

/* Definition for PKA offset (if required) */
#define DX_NOT_TO_SET_PKA_OFFSET	0xFFFFFFFF

#ifdef __cplusplus
}
#endif

#endif


