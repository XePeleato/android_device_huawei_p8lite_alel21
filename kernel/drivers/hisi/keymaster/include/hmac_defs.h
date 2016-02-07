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

#ifndef  __SEP_HMAC_DEFS_H__
#define  __SEP_HMAC_DEFS_H__

/* this files provides definitions required for HMAC engine drivers
   it is used by both FW driver as well as OEM-CRYS driver.          */
#define HMAC_DECRYPTED_OPAD_CONST_BLOCK 0x601D1102, 0xAD34E4AA, 0xB9351FAA, 0xD7356DF1
#define HMAC_DECRYPTED_IPAD_CONST_BLOCK 0xA8473C7E, 0x2AE67627, 0x50ADFC61, 0xEE6F3117

#define AES_CTR_NO_COUNTER_INC_REG_ADDR	0x4D8

#endif /*__SEP_HMAC_DEFS_H__*/

