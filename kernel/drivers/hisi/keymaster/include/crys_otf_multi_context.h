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

 
 
#ifndef CRYS_OTF_MULTI_CONTEXT_H
#define CRYS_OTF_MULTI_CONTEXT_H

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
   *  Object % CRYS_OTF_MultiContext.h    : %
   *  State           :  %state%
   *  Creation date   :  09 Marth. 2009
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This file contains all of definitions that are used for the 
   *         CRYS_OTF_MultiContext OTF APIs, as well as the APIs themselves.
   *
   *  \version CRYS_OTF_MultiContext#1:incl:3
   *  \author R.Levin, 
   */


/************************ Defines ******************************/

#define CRYS_OTF_MC_MAX_AES_KEY_SIZE_IN_BYTES   64
#define CRYS_OTF_MC_MAX_AES_KEY_SIZE_IN_WORDS   (CRYS_OTF_MC_MAX_AES_KEY_SIZE_IN_BYTES / sizeof(DxUint32_t))

#define CRYS_OTF_MC_MAX_IV_SIZE_IN_BYTES        16
#define CRYS_OTF_MC_MAX_IV_SIZE_IN_WORDS 	    (CRYS_OTF_MC_MAX_IV_SIZE_IN_BYTES / sizeof(DxUint32_t))

#define CRYS_OTF_MC_MAX_CONTEXT_ID               1  /* allowed values from 0 to MAX_CONTEXT_ID */

/* Enum defining the user's key size argument */
typedef enum
{
   CRYS_OTF_MC_AES_Key128BitSize   = 0,
   CRYS_OTF_MC_AES_Key192BitSize   = 1,
   CRYS_OTF_MC_AES_Key256BitSize   = 2,
   CRYS_OTF_MC_AES_Key512BitSize   = 3,

   CRYS_OTF_MC_AES_KeySizeNumOfOptions,
   
   CRYS_OTF_MC_AES_KeySizeLast    = 0x7FFFFFFF,

}CRYS_OTF_MC_AES_KeySizeID_t;  


/* Enum defining the Encrypt or Decrypt operation mode */
typedef enum 
{
   CRYS_OTF_MC_Encrypt = 0,
   CRYS_OTF_MC_Decrypt = 1,
   
   CRYS_OTF_MC_EncryptNumOfOptions,

   CRYS_OTF_MC_EncryptModeLast= 0x7FFFFFFF,

}CRYS_OTF_MC_EncryptMode_t;


/* OTF_MultiContext operation mode */
typedef enum
{
   CRYS_OTF_MC_AES_ECB_mode          = 0,
   CRYS_OTF_MC_AES_CBC_mode          = 1,
   CRYS_OTF_MC_AES_CTR_mode          = 2,
   CRYS_OTF_MC_AES_XTS_mode          = 3,
   
   CRYS_OTF_MC_AESOpNumOfModes,

   CRYS_OTF_MC_AESOpModeLast= 0x7FFFFFFF,

}CRYS_OTF_MC_AESOperationMode_t;


/* data stream direction: Queue to Memory  or Memory to Queue */

typedef enum
{
   CRYS_OTF_MC_MemToQueue           = 0,
   CRYS_OTF_MC_QueueToMem           = 1,
 
   CRYS_OTF_MC_DirectionOfModes,

   CRYS_OTF_MC_DirectionLast= 0x7FFFFFFF,

}CRYS_OTF_MC_DataDirection_t;


/*------------------------------------------------*/
/* CRYS OTF MultiContext SW AES context structure */
/*------------------------------------------------*/
typedef struct {
 
  /* AES Key   */
  /* NOTE: 1. When EncrDecrMode = CRYS_OTF_MC_Decrypt the AES Key will be changed by decrypted key 
           2. On XTS mode Key = Key1|Key2, where Key2 used for encrypting the Tweak         */
  DxUint32_t                        Key [CRYS_OTF_MC_MAX_AES_KEY_SIZE_IN_WORDS];
  
  /* IV, Tweak, CTR data */
  /* NOTE: IvTweakCTR will be changed by LLF functions   */
  DxUint32_t                        IvTweakCTR [CRYS_OTF_MC_MAX_IV_SIZE_IN_WORDS];	  

  /* AES KeyID and sizes. Note: On XTS mode KeySizeWords defines a 
                                full size of two concatenated keys */
  CRYS_OTF_MC_AES_KeySizeID_t       KeySizeID;
  DxUint32_t                        KeySizeWords;  /*full key size*/
  /* sngle key size: relevant to XTS mode (size of TweakKey or DataKey = KeySizeWords/2) */
  DxUint32_t                        SingleKeySizeWords;
  
  /* OTF_MultiContext modes */
  CRYS_OTF_MC_AESOperationMode_t    OperationMode;
  CRYS_OTF_MC_EncryptMode_t         EncrDecrMode;
  CRYS_OTF_MC_DataDirection_t       DataDirection;
  DxUint8_t                         IsBypass;  
  
  /* Data specific to the platform we are working on - include from LLF */
  #include "LLF_OTF_MultiContext_AesCont_def.h"
  
  /* validation tag */
  DxUint32_t valid_tag;

} CRYS_OTF_MC_SwAesContext_t;



/* Define the IV counter buffer  - 16 bytes array */
typedef DxUint8_t CRYS_OTF_MC_IvTweakCTR_t[CRYS_OTF_MC_MAX_IV_SIZE_IN_BYTES];

/* Define the AES key buffer */
typedef DxUint8_t CRYS_OTF_MC_Key_t[CRYS_OTF_MC_MAX_AES_KEY_SIZE_IN_BYTES];


/************************ Enums ********************************/
   
/************************ Typedefs  ****************************/

/************************ Public Variables **********************/


/************************ Public Functions **********************/

/**************************************************************************************************
 * @brief CRYS_OTF_MC_StartSession  - This function initializes the HW AES mashine on MultiCore mode.
 *
 *               This function calls LLF function, which enables AES clocks an sets multi-core mode.
 *
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value 
 *                       from CRYS_OTF_MultiContext_error.h
 *
 **************************************************************************************************/
CIMPORT_C CRYSError_t  CRYS_OTF_MC_StartSession ( void );                   



/**************************************************************************************************
 * @brief CRYS_OTF_MC_EndSession  - This function calls LLF function, which ends AES Multi context 
 *                                  session and closes AES HW clocks.
 *
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value 
 *                       from CRYS_OTF_MultiContext_error.h
 *
 **************************************************************************************************/
CIMPORT_C CRYSError_t  CRYS_OTF_MC_EndSession( void );                   



/**************************************************************************************************
 * @brief CRYS_OTF_MC_AES_ContextInit - This function initializes the HW MultiContext for AES operation.
 *
 * @param[in] HwContextID  - ID of HW context to load (now allowed value is: 1, 2).
 * @param[in] SwContext_ptr - pointer to SW OTP_MC AES context buffer. Used as temp buffer for internal calculations.
 * @param[in] IvTweakCTR   - pointer to Tweak data, representing an integer number in big endian 
 *                           bytes order
 * @param[in] Key          - pointer to Key data. On XTS mode the key includes concatenated Key1, 
 *                           used for encrypting/decrypting the input data and Key2,used for 
 *                           encrypting the Tweak.  Key = Key1 | Key2.
 * @param[in] KeySizeID    - ID of the key size (enumerator).  For XTS the ID is related to full  
 *                           key size and may be only CRYS_AES_Key256BitSize or CRYS_AES_Key512BitSize.
 * @param[in] EncrDecrMode - A flag specifying whether the Encrypt or Decrypt AES should perform: 
 *                                 0 - Encrypt; 1 - Decrypt. 
 * @param[in] OperationMode  - The AES machine operation mode: Bypass, ECB, CBC, CTR, XTS.
 *
 * @param[in] DataDirection  - The data stream direction: 0 - Queue to Memory, 1 - Memory to Queue.
 * @param[in] IsBypass       - Defining whether the bypass mode is set or not: 1 - bypass, 0 - perform AES.  
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value 
 *                       from CRYS_OTF_MultiContext_error.h
 *
 **************************************************************************************************/
CIMPORT_C CRYSError_t  CRYS_OTF_MC_AES_ContextInit (
						          DxUint32_t                        HwContextID,
						          CRYS_OTF_MC_SwAesContext_t       *SwContext_ptr,  
						          CRYS_OTF_MC_IvTweakCTR_t          IvTweakCTR,
						          CRYS_OTF_MC_Key_t                 Key,   
						          CRYS_OTF_MC_AES_KeySizeID_t       KeySizeID,
                                  CRYS_OTF_MC_EncryptMode_t         EncrDecrMode,
                                  CRYS_OTF_MC_AESOperationMode_t    OperationMode,
                                  CRYS_OTF_MC_DataDirection_t       DataDirection,
                                  DxUint8_t                         IsBypass );
                                  
                                  

/**************************************************************************************************
 * @brief CRYS_OTF_MC_GetHWContextStatus  - This function the HW Context status: 0 - free, 1 - busy.
 *
 *
 * @param[in] HwContextID - ID of the context (0,1,2 ... ).
 * @param[in] status_ptr  - the pointer to status variable each i-bit of which, describes status of 
 *                          appropriate i-Context; if this i-bit = 1, then the i-Context is busy, else -free.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value 
 *                       from CRYS_OTF_MultiContext_error.h
 *
 **************************************************************************************************/
CIMPORT_C DxUint32  CRYS_OTF_MC_GetHWContextStatus ( DxUint32_t   HwContextID, 
                                                     DxUint32_t  *status_ptr );                   



/**************************************************************************************************
 * @brief CRYS_OTF_MultiContextSuspend - This function stops the HW operation and stores the last 
 *                                       state of HW context. 
 * @param[in] HwContextID  - ID of HW context to load (now allowed values are: 1, 2).
 * @param[in] SwContext_ptr - pointer to user context for storing HW context data.
 * 
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value 
 *                       from CRYS_OTF_MultiContext_error.h
 *
 **************************************************************************************************/
CIMPORT_C CRYSError_t CRYS_OTF_MC_AES_SuspendContext (
                                                DxUint32_t                  HwContextID,
                                                CRYS_OTF_MC_SwAesContext_t *SwContext_ptr );



/**************************************************************************************************
 * @brief CRYS_OTF_MultiContextResume - This function resets the HW operation stopped by the Suspend 
 *									    operation.The function loads the HW context by the data 
 *                                      stored in the CRYS SW context and starts the HW operation again.
 *
 * @param[in] HwContextID   - ID of HW context to load (now allowed values are: 1, 2).
 * @param[in] SwContext_ptr - pointer to user context for storing HW context data.
 * 
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value 
 *                       from CRYS_OTF_MultiContext_error.h
 *
 **************************************************************************************************/
CIMPORT_C CRYSError_t CRYS_OTF_MC_AES_ResumeContext (
                                                DxUint32_t                  HwContextID,
                                                CRYS_OTF_MC_SwAesContext_t *SwContext_ptr );



												          
#ifdef __cplusplus
}
#endif

#endif
