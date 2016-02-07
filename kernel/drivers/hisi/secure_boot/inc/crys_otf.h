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

 
 
#ifndef CRYS_OTF_H
#define CRYS_OTF_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "DX_VOS_BaseTypes.h"
#include "crys_error.h"
#include "crys_defs.h"
#include "crys_hash.h"
#include "crys_aes.h"


#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object % CRYS_OTF.h    : %
   *  State           :  %state%
   *  Creation date   :  02 Sept. 2007
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This file contains all of definitions that are used for the 
   *         CRYS OTF APIs, as well as the APIs themselves.
   *
   *  \version CRYS_OTF.h#1:incl:3
   *  \author yakovg, 
   */


/************************ Defines ******************************/
/* OTF support only in AES 128/192/256 bit key and 3 DES key, and 512 bit for XTS (Tweak key| data key) */
#define CRYS_OTF_MAX_KEY_SIZE_IN_BYTES 64
#define CRYS_OTF_MAX_IV_SIZE_IN_BYTES 16

#define CRYS_OTF_MAX_KEY_SIZE_IN_WORDS (CRYS_OTF_MAX_KEY_SIZE_IN_BYTES / sizeof(DxUint32_t))
#define CRYS_OTF_MAX_IV_SIZE_IN_WORDS 	(CRYS_OTF_MAX_IV_SIZE_IN_BYTES / sizeof(DxUint32_t))



typedef struct CRYS_OTFUserKeysInfo_st{
 
  DxUint8_t Key0[CRYS_OTF_MAX_KEY_SIZE_IN_BYTES];  
  DxUint16_t Key0Size;  
  DxUint8_t Iv0[CRYS_OTF_MAX_IV_SIZE_IN_BYTES];	  
  DxUint16_t Iv0Size;
  DxUint8_t Key1[CRYS_OTF_MAX_KEY_SIZE_IN_BYTES];  
  DxUint16_t Key1Size;  
  DxUint8_t Iv1[CRYS_OTF_MAX_IV_SIZE_IN_BYTES];	  
  DxUint16_t Iv1Size;
}CRYS_OTFUserKeysInfo_t;

typedef struct CRYS_OTFKeysInfo_st{
 
  DxUint32_t Key0[CRYS_OTF_MAX_KEY_SIZE_IN_WORDS];  
  DxUint16_t Key0Size;  
  DxUint32_t Iv0 [CRYS_OTF_MAX_IV_SIZE_IN_WORDS];	  
  DxUint16_t Iv0Size;
  DxUint32_t Key1[CRYS_OTF_MAX_KEY_SIZE_IN_WORDS];  
  DxUint16_t Key1Size;  
  DxUint32_t Iv1[CRYS_OTF_MAX_IV_SIZE_IN_WORDS];	  
  DxUint16_t Iv1Size;
}CRYS_OTFKeysInfo_t;

typedef enum CRYS_OTFEngCfg_st
{
	/* bypass */
	OTF_DIN_TO_DOUT=0,
	/* AES OTF */
	OTF_DIN_TO_AES_TO_DOUT=1,
	/* DIN into AES. output AES into AES */
	OTF_DIN_TO_AES_TO_AES_TO_DOUT=2,
	/* HASH OTF */
	OTF_DIN_TO_HASH=3,
		/* DIN into HASH and into output buffer */
	OTF_DIN_TO_HASH_AND_DOUT=4,
	/* DIN into AES. output AES into HASH */
	OTF_DIN_TO_AES_TO_HASH=5,
	/* DIN into AES. output AES into hash and output AES into output buffer */
	OTF_DIN_TO_AES_TO_HASH_AND_DOUT=6,
		/* DIN into AES and DIN into HASH in parallel */
	OTF_DIN_TO_AES_AND_HASH_TO_DOUT=7,
	/* DIN into AES. output AES into hash and into AES in parallel */ 
	OTF_DIN_TO_AES_TO_AES_AND_HASH_TO_DOUT=8,
	/* DIN into AES. output AES into AES.output AES into HASH */ 
	OTF_DIN_TO_AES_TO_AES_TO_HASH_AND_DOUT=9,
	/* DES OTF */
	OTF_DIN_TO_DES_TO_DOUT=10,
	/* DIN into DES. output DES into HASH */
	OTF_DIN_TO_DES_TO_HASH=11,
	/* DIN into AES and DIN into HASH in parallel */
	OTF_DIN_TO_DES_AND_HASH_TO_DOUT=12,
	/* DIN into DES. output DES into HASH and into output buffer */
	OTF_DIN_TO_DES_TO_HASH_AND_DOUT=13,
	/* OTF RC4 */
	OTF_DIN_TO_RC4=14,
	/* DIN into AES MAC and into output buffer */
	OTF_DIN_TO_AES_MAC_AND_BYPASS=15,
	
	OTFEngCfg_OffMode,
	
	CRYS_OTFEngCfgLast=0x7FFFFFFF,

}CRYS_OTFEngCfg_t;

typedef enum CRYS_OTFEngMode_st
{
	ENC_ECB=0,
	ENC_CBC=1,
	ENC_CTR=2,
	DEC_ECB=3,
	DEC_CBC=4,
	DEC_CTR=5,
	CMAC=6,
	HASH_SHA1=7,
	HASH_SHA256=8,
	ENC_XTS=9,
	DEC_XTS=10 ,
	ENC_CBC_IV_ENC=11,
	DEC_CBC_IV_ENC=12,
	ENC_CBC_IV_ENC_UPDATE=13,
	DEC_CBC_IV_ENC_UPDATE=14,
	N_A_VALUE=0xff,

	CRYS_OTFEngModeLast=0x7FFFFFFF,

}CRYS_OTFEngMode_t;


typedef struct CRYS_OTFHWOp_st
{
	DxUint16_t Bypass;
	DxUint16_t Aes;
	DxUint16_t Des;
	DxUint16_t Hash;

}CRYS_OTFHWOp_t;

typedef enum
{
	HOST_TO_MEDIA = 0,
	MEDIA_TO_HOST = 1,
	CRYS_OTFOpDirectionLast = 0x7FFFFFFF,
}CRYS_OTFOpDirection_t;



typedef struct OTFContext_st
{
	/* General data section */	
	CRYS_OTFKeysInfo_t	Keys;
	
	CRYS_OTFEngCfg_t   OTFEngCfg;
	
	CRYS_OTFEngMode_t  OTFEngMode_ptr[3];
	
	/* direction - media-to-host, host-to-media */
	CRYS_OTFOpDirection_t	direction;
	
	/* Private hash section */	
	CRYS_HASH_Result_t        HASH_Result;
	
	DxUint32_t TotalDataSizeProcessed_128Bit[2];
	
	/* Private tunnel section */
	DxUint32_t TunnelMode;
	
	/* if set, the output from the first operation of the tunel should be sent to another engine (HASH) */
	DxUint32_t	midTunnelToHash;
	
	/* General menagement section */	
	DxUint32_t valid_tag;
		
}OTFContext_t;


/* The user's context prototype - the argument type that will be passed by the user 
   to the APIs called by him */
typedef struct CRYS_OTFUserContext_t 
{
   DxUint32_t valid_tag; 
   DxUint32_t AES_iv;/* For use of the AES CBC mode of Encryption and Decryption of the context in CCM*/
   DxUint8_t  context_buff[ sizeof(OTFContext_t) ]; 
 

}CRYS_OTFUserContext_t;

/************************ Enums ********************************/

   
/************************ Typedefs  ****************************/

/************************ Public Variables **********************/


/************************ Public Functions **********************/


/**************************************************************************************************
 * @brief CRYS_OTF_InitOperation - This function sets the OTF context with the 
 * 								   required operation scenario, mode of operation, keys and Ivs.  
 *
 * @param[in] OTFContext_ptr - Pointer to the OTF context.
 *
 * @param[in] CRYS_OTFEngCfg - The mode of the engine to configure the Crypto Flashfor the process.
 *							   The engine modes are defined at the CRYS_OTFEngCfg_t type.
 *
 * @param[in] CRYS_OTFKeysInfo_ptr - A pointer to a structure contains the Key and IV.
 *
 * @param[in] CRYS_OTFEngMode_ptr  - Pointer to CRYS_OTFEngMode_t array.
 *									 Each cell in the array indicates on the mode of operation of
 *									 this stage in the path from DIN to DOUT/CPU.
 *									 The mode is the mode to set the Crypto Flash hardware operation.
 *								     The operation mode are defined in the CRYS_OTFEngMode_t type.
 *
 * @param[in] OTFDirection - Direction of the data from. Media-to-host , or Host-to-media
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 *
 **************************************************************************************************/
												 
CIMPORT_C CRYSError_t CRYS_OTF_InitOperation (CRYS_OTFUserContext_t*	OTFContext_ptr,
																							CRYS_OTFEngCfg_t				OTFEngCfg,
																							CRYS_OTFUserKeysInfo_t*	OTFUserKeysInfo_ptr,
																							CRYS_OTFEngMode_t*			OTFEngMode_ptr,
																							CRYS_OTFOpDirection_t		OTFDirection);
									          
									          
/**************************************************************************************************
 * @brief CRYS_OTF_ResumeOperation - This function restart the hw operation stopped by the Rsume operation.
 *												 The function load to the HW the last state stored in the given context
 *												  and continue the operation exactly from the point it stopped in the Suspend function.
 * @param[in] OTFContext_ptr - Pointer to the OTF user context.
 *
 * 
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 *
 **************************************************************************************************/
CIMPORT_C CRYSError_t CRYS_OTF_ResumeOperation (CRYS_OTFUserContext_t *ContextID_ptr);




/**************************************************************************************************
 * @brief CRYS_OTF_SuspendOperation - This function stop the hw operation started in the Init operation.
 *												  The function store the last state to enable the user 
 *												  to resume to this state exactly by the CRYS_OTF_ResumeOperation
 * @param[in] OTFContext_ptr - Pointer to the OTF context.
 *
 * 
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 *
 **************************************************************************************************/
CIMPORT_C CRYSError_t CRYS_OTF_SuspendOperation (CRYS_OTFUserContext_t *ContextID_ptr);


/**************************************************************************************************
 * @brief CRYS_OTF_EndOperation - This function stop the hw operation.
 *												  The function read the last data to the user 
 *												  
 * @param[in] OTFContext_ptr - Pointer to the OTF context.
 *
 * 
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 *
 **************************************************************************************************/
CIMPORT_C CRYSError_t CRYS_OTF_EndOperation ( CRYS_OTFUserContext_t *ContextID_ptr,
			 												 CRYS_HASH_Result_t       HashResultBuff,
			 											 	 CRYS_AES_MAX_MAC_RESULT_t  MacResult);
 											 	

												          
#ifdef __cplusplus
}
#endif

#endif
