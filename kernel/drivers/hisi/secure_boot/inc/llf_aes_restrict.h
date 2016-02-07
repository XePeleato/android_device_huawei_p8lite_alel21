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
 
 
#ifndef LLF_AES_RESTRICT_H
#define LLF_AES_RESTRICT_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "dx_vos_basetypes.h"
#include "crys_aes.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  01 Apr. 2010
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version LLF_AES.h#1:incl:1
   *  \author R.Levin
   */




/************************ Defines ******************************/

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/* The AES restricted context data-base used by some algorithms on the low level */
typedef struct  
{ 
	CRYS_AES_OperationMode_t aesMode;
	CRYS_AES_EncryptMode_t   encryptMode;
	DxUint16_t       	 keySizeWords;
	DxUint32_t 		 state[4];
}AESResrtictContext_t; 

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/


/******************************************************************************************/
/********************        Restricted LLF AES Functions     *****************************/
/******************************************************************************************/

/************************************************************************************/
/**
 * @brief The low level LLF_AES_RESTR_StartInit function gets the AES HW semaphore.
 *
 *   The function must be called once before starting AES_RESTRICTED processing.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */                              
CRYSError_t  LLF_AES_RESTR_StartInit ( void );


/************************************************************************************/
/**
 * @brief The low level LLF_RND_AES_Init function initializes the hardware to operate 
 *        AES at CBC-MAC or CTR modes
 *
 *
 * @param[in] key_ptr - The pointer to AES key.
 * @param[in] keySizeWords  -  The key size in words.
 * @param[in] iv_ptr        -  The pointer to the initial (chaining) value.
 * @param[in] aesMode       -  AES Operation mode.
 * @param[in] encrMode      -  AES encypt-decrypt mode.
 * @param[in] aesCont_ptr   -  The pointer to restricted AES context used for specific operations.
 *                             In HW implementation it contains virtual Hw Base address.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */ 
CRYSError_t  LLF_AES_RESTR_Init (	                       
	                       DxUint32_t      *key_ptr,
	                       DxUint16_t       keySizeWords,
	                       DxUint32_t      *iv_ptr,
	                       CRYS_AES_OperationMode_t aesMode,
	                       CRYS_AES_EncryptMode_t   encrMode,
	                       AESResrtictContext_t *aesCont_ptr );
	                       

/****************************************************************************************/
/**
 * @brief The function performs AES operations on full AES blocks of data.
 *        
 * @param[in] in_ptr - The pointer to the input buffer.
 * @param[in] out_ptr - The pointer to the output buffer.
 * @param[in] sizeInBlocks - size of data in AES blocks.
 * @param[in] aesCont_ptr   -  The pointer to restricted AES context used for specific operations.
 *                             In HW implementation it contains virtual Hw Base address.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_AES_RESTR_BlockExec( 
                        DxUint32_t *in_ptr,
                        DxUint32_t *out_ptr,
                        DxUint32_t  sizeInBlocks,
                        AESResrtictContext_t *aesCont_ptr );
                        
                        
/*********************************************************************************************************/
/**
 * @brief The low level LLF_RND_AES_Finish reads the last block from the DATA_OUT.
 *        and closes the hardware clock.     
 *         
 * @param[in] v_ptr         -  The pointer to the initial (chaining) or CTR value.
 * @param[in] aesCont_ptr   -  The pointer to restricted AES context used for specific operations.
 *                             In HW implementation it contains virtual Hw Base address.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */                              
CRYSError_t  LLF_AES_RESTR_Finish( 
	                       DxUint32_t      *v_ptr,              /*in*/
                    	   AESResrtictContext_t *aesCont_ptr ); /*in*/  
                    	   
/************************************************************************************/
/**
 * @brief The low level LLF_AES_RESTR_EndFinish function releases the AES HW semaphore.
 *
 *   The function must be called after finish AES_RESTRICTED processing.
 *
 * @return - no return value
 *
 */                              
void  LLF_AES_RESTR_EndFinish ( void );

                    	   
/*********************************************************************************************************/
/**
 * @brief The LLF_AES_RESTR_ReadIV function gets the IV fron HW registers in MAC or CBC modes 
 *         
 * @param[in] iv_ptr        -  The pointer to the initial (chaining) value.
 * @param[in] aesCont_ptr   -  The pointer to restricted AES context used for specific operations.
 *                             In HW implementation it contains virtual Hw Base address.  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */                              
void  LLF_AES_RESTR_ReadIV( 
                       DxUint32_t           *iv_ptr,        /*in*/
                	   AESResrtictContext_t *aesCont_ptr ); /*in*/
                	   
                	   

/****************************************************************************************/
/**
* @brief This macro waits for AES busy and loads words into AES machine. 
*  
*   The macro is used in LLF RND module.     
*   NOTE: The macros waits AES busy before loading the data. No waiting after load.
*
* @param[in] VirtualHwBaseAddr - The base address.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure a
*                        value MODULE_* as defined in ...
*/
#define LLF_AES_RESTR_WaitAndLoadWords( virtualHwBaseAddr, in_ptr, sizeInWords ) \
do \
{ \
   LLF_AES_HW_WAIT_ON_AES_BUSY_BIT( virtualHwBaseAddr ); \
   CRYS_AES_SYS_WriteRegistersBlock( (virtualHwBaseAddr) + LLF_AES_HW_DIN_DOUT_ADDR, (in_ptr), (sizeInWords) ); \
}while (0) 
   

/****************************************************************************************/
/**
* @brief This macro waits for AES busy . 
*  
*   The macro is used in LLF RND module.     
*
* @param[in] VirtualHwBaseAddr - The base address.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure a
*                        value MODULE_* as defined in ...
*/
#define LLF_AES_RESTR_WaitAesBusy( virtualHwBaseAddr ) \
do \
{ \
	LLF_AES_HW_WAIT_ON_AES_BUSY_BIT( (virtualHwBaseAddr) ); \
}while (0) 
                	   
                    	                         	                       
          
#ifdef __cplusplus
}
#endif

#endif


