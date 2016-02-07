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


#ifndef PLAT_PRINT_DBG_H
#define PLAT_PRINT_DBG_H

/*
 * Includes
 */

#include "dx_vos_basetypes.h"
#include "dx_pal_log.h" 
//deleted by lixiaojie,2012/11/6
//#include <stdio.h>


#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:09:43 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version PLAT_SystemDep.h#1:incl:1
   *  \author adams
   *  \remarks Copyright (C) 2004 by Discretix Technologies Ltd.
   *           All Rights reserved
   */


/************************ Defines ******************************/

/* .................... basic Macros for accessing hardware registers ...................... */
/* ----------------------------------------------------------------------------------------- */



#ifdef DX_DBG_PLAT_PRINT

#define PLAT_LOG_LEVEL (DX_DBG_PLAT_PRINT + 11)//to convert it to system plat log (as defined in cc52)
	   
#define _SEP_LOG_PRINT(logLevel, format, ...)       \
		do {\
		if(logLevel < PLAT_LOG_LEVEL){\
		PRINTF("%s: " format, __func__, ##__VA_ARGS__);}\
	   }while (0)
	   
#define PLAT_LOG_DEV_PRINT(args) _SEP_LOG_PRINT args



/**
 * @brief The folowing function prints a buffer. 
 *        
 *
 * @param[in] level - the printing level if it is below the CRYS_LOG_DEV_MAX_LEVEL_ENABLED
 *                    flag setting it will not be printed.
 *
 * @param[in] Label - a lable string.
 * @param[in] Buffer - the buffer to be printed.
 * @param[in] Size  - the buffer size in bytes.
 * 
 */
 /* ********************* Provate Functions *******************************/



#define _SEP_LOG_PRINT_BUF(level, label, buffer, size)    \
	do{\
	   DxUint32_t  i; \
	   if(level < PLAT_LOG_LEVEL){\
	        PRINTF("\n level %d  %s -[size %lu ] Addr : %p\n", (int)level, (char*)label, (long unsigned)size, (void*)buffer); \
		for( i=0 ; i<size ;i++) {\
			if ( !(i % 16) ){ \
				PRINTF( "\n" ); }\
			PRINTF("0x%02x,",((DxUint8_t*)buffer)[i]); \
		} \
		PRINTF("\n"); \
	   }\
	}while(0)

#define PLAT_LOG_DEV_PRINT_DisplayBuffer(args)  _SEP_LOG_PRINT_BUF args

#define PLAT_LOG_DEV_PRINT_DisplayDataMapleLittleEndian(args) (PLAT_LOG_DEV_PRINT args)

#else // DX_DEBUG_PRINT

#define PLAT_LOG_DEV_PRINT(args)
#define PLAT_LOG_DEV_PRINT_DisplayBuffer(args)
#define PLAT_LOG_DEV_PRINT_DisplayDataMapleLittleEndian(args)

#endif


#ifdef __cplusplus
}
#endif

#endif



