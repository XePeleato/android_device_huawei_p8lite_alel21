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

 
 #ifndef LLF_ECPKI_ECDH_DB_DEF_H
#define LLF_ECPKI_ECDH_DB_DEF_H
  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  28/05/2006
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A abow stated brief description of this module
   *
   *  \version LLF_ECPKI_ECDH_db_def.h#1:incl:1
   *  \author R.Levin
   */
/*
 * This file is included in the middle of the struct declaration for CRYS_ECPKI data types
 * It contains the platform-specific parts of the context struct. As such:
 *
 *  1) file should not use any includes it is a part of the CRYS_ECPKI_Types.h file !!!!
 *  2) only the CRYS_ECPKI_Types.h file should include this file. 
 */

      struct
      {
		   /*! The aligned temporary buffers for two projective points */
		   DxUint32_t  s[CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1];
      }LLF;
   
#endif /*LLF_ECPKI_ECDH_DB_DEF_H*/



