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

 
#ifndef CRYS_SELF_TEST_ERROR_H
#define CRYS_SELF_TEST_ERROR_H
  
/*
   *  Object % crys_self_test_error.h    : %
   *  State           :  %state%
   *  Creation date   :  07/03/ 2012 Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module containes the definitions of the crys "self test"
   *         errors.
   *
   *  \version crys_self_test_error.h#1:incl:1
   *  \author G.Koltun
  */
/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "crys_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/****************************************************************************************
* CRYS SELF TEST MODULE ERRORS 							*                     
***************************************************************************************/
#define CRYS_SELF_TEST_MSG_GENERAL_RPC_A_ERROR    (CRYS_SELF_TEST_MODULE_ERROR_BASE + 0UL)
#define CRYS_SELF_TEST_MSG_GENERAL_RPC_B_ERROR    (CRYS_SELF_TEST_MODULE_ERROR_BASE + 1UL)
#define CRYS_SELF_TEST_MSG_GENERAL_RPC_C_ERROR    (CRYS_SELF_TEST_MODULE_ERROR_BASE + 2UL)



















/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif


