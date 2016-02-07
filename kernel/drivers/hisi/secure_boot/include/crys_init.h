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

 
 #ifndef CRYS_INIT_H
#define CRYS_INIT_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
 
#include "DX_VOS_BaseTypes.h"
#include "crys_defs.h"
#include "crys_init_error.h"
 
#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object % CRYS_init.c    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:44:02 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version CRYS_init.h#1:incl:1
   *  \author adams
   */




/************************ Defines ******************************/

#define CRYS_INIT_PARAM_128_BIT_RKEK		0x1
#define CRYS_INIT_PARAM_256_BIT_RKEK		0x0

#define CRYS_INIT_PARAM_RSA_DMA_MODE		0x0
#define CRYS_INIT_PARAM_RSA_DIRECT_MODE		0x2

#define CRYS_INIT_PARAM_AHB_32_BITS_MODE	0x0
#define CRYS_INIT_PARAM_AHB_64_BITS_MODE	0x4
/************************ Enums ********************************/

#ifdef DX_CRYS_MULTI_CPU_SUPPORT
/*This enum is relevant only to the multi CPU projects*/
typedef enum
{
   CRYS_MULTI_CPU_CP_ID = 1,
   CRYS_MULTI_CPU_AP_ID = 2,
   
   CRYS_MULTI_CPU_NumOfCpuId,
   
   CRYS_MULTI_CPU_NumOfCpuIdLast= 0x7FFFFFFF,

}CRYS_MULTI_CPU_NumOfCpuId_t;
#endif

/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

/**
 * @brief This function initializes the CRYS SW.
 *
 *  The function calls the init functions of all of the modules in the CRYS.
 *
 * @param[in] - HwBaseAddress the hardware base address.
 *
 * \note The last two arguments are have different use in the different used LLF.
 *
 * @param[in] - param_PTR - pointer to parameter for the CRYS_Init (the seed used to create the SK - relevant only on CRYS_SW).
 *               
 * 
 *
 * @param[in] - param - parameter by value.
 *				CRYS software -  (the size of the seed in bytes when seed is in param_PTR).
 * 				CRYS with SEP - RKEK size and PKI mode
 *				CRYS with multi CPU support - CPU Id.
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

CIMPORT_C CRYSError_t  CRYS_Init(DxUint32_t CryptoCellBaseAddress , DxUint8_t *param_PTR , DxUint16_t param );

/**
 * @brief This function terminates the CRYS SW.
 *
 *  The function calls the terminate functions of all of the relevant modules in the CRYS.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

CIMPORT_C CRYSError_t  CRYS_Terminate(void);

#ifdef __cplusplus
}
#endif

#endif


