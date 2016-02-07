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

 
#ifndef LLF_RND_ENTR_ESTIM_DB_DEF_H
#define LLF_RND_ENTR_ESTIM_DB_DEF_H

/*
 * This file is #included in the middle of the struct declaration for CRYS_RND_EntropyEstimatData_t.
 * It contains the platform-specific parts of the context struct. 
 *
 *  Note:
 *    - file should not use any includes it is a part of the CRYS_RND.h file !!!!
 *    - only the CRYS_RND.h file should include this file. 
 */

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  24 Feb 2010
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version LLF_RND_entr_estim_db_def.h#1:incl:1
   *  \author R.Levin
   */  

/* structure containing parameters and buffers for entropy estimator */
typedef struct
{ 
   /* estimated entropy size */
   DxUint32_t EstimEntropySizeBits;     
   /* estimated error of entropy size */
   DxUint32_t EstimEntropySizeErrorInBits;  

   /* special buffers */
   DxUint32_t h[CRYS_RND_NB];         /* histogram */
   DxUint32_t ec[CRYS_RND_NB/2];      /* equality counter for prefix */
   
   /*!!! Note: The p buffer is not in use now, but saved for compatibility
      with CC52 ROM version. It may be deleted in the future */
   DxUint32_t p[CRYS_RND_NB/2];       /* previous bit for prefix */
   
}LLF_rnd_entr_estim_db_t;



#endif
