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

 
 #ifndef LLF_PKI_PUB_KEY_DB_DEF_H
#define LLF_PKI_PUB_KEY_DB_DEF_H

/*
 * This file is #included in the middle of the struct declaration for CRYSRSAPubKey_t
 * It contains the platform-specific parts of the context struct. As such:
 *
 *  1) file should not use any includes it is a part of the crys_rsa_types.h file !!!!
 *  2) only the crys_rsa_types.h file should include this file. 
 */


  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  17 Jan. 2008
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version LLF_RSA_pub_key_db_def.h#1:incl:1
   *  \author adams, R.Levin
   */

   typedef struct
   {  
       /* the Barrett mod tag  NP for N-modulus - used in the modular multiplication and 
          exponentiation, calculated in CRYS_RSA_Build_PrivKey function */
       DxUint32_t NP[LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
   }LLF_pki_pub_key_db_t;

#endif

