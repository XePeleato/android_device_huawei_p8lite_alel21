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

 
 #ifndef LLF_ECPKI_PRIV_KEY_DB_DEF_H
#define LLF_ECPKI_PRIV_KEY_DB_DEF_H 
/*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  03.042008
   *  Last modified   :  % 21 Feb. 2008 %
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version LLF_RSA_priv_key_db_def.h#1:incl:1
   *  \author R.Levin
   */

/*
 * This file is #included in the middle of the struct declaration for CRYS_ECPKI_PrivKey_t
 * It contains the platform-specific parts of the context struct. As such:
 *
 *  1) file should not use any includes it is a part of the CRYS_ECPKI_Types.h file !!!!
 *  2) only the CRYS_ECPKI_Types.h file should include this file. 
 */ 
 
  
  
     typedef struct
     {   
        DxUint32_t NP[LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /* Barrett for modulus*/
        DxUint32_t RP[LLF_ECPKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /* Barrett for order */
     } LLF_ecpki_priv_key_db_t;
  

#endif

