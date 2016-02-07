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



#ifndef _DX_PAL_TIME_H
#define _DX_PAL_TIME_H


#ifdef __cplusplus
extern "C"
{
#endif

/** 
* @brief File Description:
*        This file conatins functions for time handling. The functions implementations
*        are generally just wrappers to different operating system calls.
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/** 
 * @brief This function returns the number of seconds passed from 1/1/1970 00:00 UTC.
 *         
 *
 * @param[in] void
 *
 * @return Number of seconds passed from 1/1/1970 00:00 UTC
 */
DxInt32_t DX_PAL_GetTime( void ); 
		                      

    
#ifdef __cplusplus
}
#endif

#endif


