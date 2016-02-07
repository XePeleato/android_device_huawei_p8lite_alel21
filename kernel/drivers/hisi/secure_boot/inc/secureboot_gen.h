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


#ifndef _SECUREBOOT_GEN_H
#define _SECUREBOOT_GEN_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#ifdef __cplusplus
extern "C"
{
#endif



  /*
   *  Object name     : gen.h
   *  State           :  %state%
   *  Creation date   :  Wed Feb 23 16:19:14 2005
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version SecureBootMain.h#1:incl:1
   *  \author adams
   *  \remarks Copyright (C) 2005 by Discretix Technologies Ltd.
   *           All Rights reserved
   */

/**
 * @brief      This function is doing poling until getting valid signature of the CC
 * 
 * @param[in] hwBaseAddress        -  cryptocell base address
 *
 * @return    DxError:  
 *                        
 */
DxUint32_t GEN_CC_start_Poling( DxUint32_t regsBaseAddr );

/**
 * @brief      This function acquires lock for accessing the CC. This function is polling until the lock is acquired
 * 
 * @param[in] hwBaseAddress        -  cryptocell base address
 *
 * @return    DxError:  
 *                        
 */
DxError_t GEN_AcquireCCLock( DxUint32_t hwBaseAddress );

/**
 * @brief      This function acquires lock for accessing the CC. This function is polling until the lock is acquired
 * 
 * @param[in] hwBaseAddress        -  cryptocell base address
 *
 * @return     DxError:  
 *                        
 */
void GEN_ReleaseCCLock( DxUint32_t hwBaseAddress );


#ifdef __cplusplus
}
#endif

#endif

