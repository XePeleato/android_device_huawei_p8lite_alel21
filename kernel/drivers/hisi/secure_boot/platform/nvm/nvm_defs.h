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

 
#ifndef _NVM_DEF_H
#define _NVM_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/* General definitions */
/***********************/

/* message token to sep */
#ifndef HOST_SEP_MSG_TOKEN_HEAD
  #define HOST_SEP_MSG_TOKEN_HEAD             0x02558808UL
#endif  

/* start address of the shared RAM */
#ifndef DX_SHARED_RAM_START_ADDR
  #define DX_SHARED_RAM_START_ADDR            0x60000000
#endif  

/* size of the shared RAM */
#ifndef DX_SHARED_RAM_SIZE
  #define DX_SHARED_RAM_SIZE                  0x4000
#endif  


/*------------------------------
    DEFINES from SEPDriver.c
--------------------------------*/
#define SEPDRIVER_START_MESSAGE_TOKEN   0X02558808


#define MAX_NUM_OF_INPUT_RANGES 4
#define MAX_NUM_OF_OUTPUT_RANGES 4

#define DX_CC_INIT_TOKEN                			0x08641326
#define DX_CC_INIT_PRIMARY_TYPE                 1
#define DX_CC_INIT_DISABLED_MODULES_TYPE        2
#define DX_CC_INIT_INPUT_INVALID_RANGES_TYPE		3
#define DX_CC_INIT_OUTPUT_INVALID_RANGES_TYPE 	4
#define DX_CC_INIT_SW_MINIMUM_VERSION_TYPE 	    5



#ifdef __cplusplus
}
#endif

#endif


