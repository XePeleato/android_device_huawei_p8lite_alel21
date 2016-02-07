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



#ifndef _DX_PAL_STRING_INT_H
#define _DX_PAL_STRING_INT_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
/** 
* @brief File Description:
*        This file conatins wrapper functions for string manipulation. 
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/** 
 * @brief A wrapper function for strlen functionality. The function returns the size of a given string
 *         
 */
#define  _DX_PAL_StrLen     strlen

/** 
 * @brief A wrapper function for strstr functionality. The functions find a string in a string and
 *        return a pointer to it.
 *         
 */
#define _DX_PAL_FindStr     strstr

/** 
 * @brief A wrapper function for strchr functionality. The function finds a char in a given string.
 *          
 */
#define _DX_PAL_StrChr      strchr

/** 
 * @brief A wrapper function for strrchr functionality. The function finds a char inside a string 
 *        (from the end) and returns a pointer to it  
 *
 */
#define _DX_PAL_StrRChr     strrchr

/** 
 * @brief A wrapper for strncpy functionality. The function copies a string.
 *         
 */
#define _DX_PAL_StrNCopy    strncpy

    
#ifdef __cplusplus
}
#endif

#endif


