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



#ifndef _DX_PAL_STRING_H
#define _DX_PAL_STRING_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "dx_pal_types.h"
#include "dx_pal_string_plat.h"
/** 
* @brief File Description:
*        This file conatins functions for string manipulation. The functions implementations
*        are generally just wrappers to different operating system calls.
*        None of the described functions will check the input parameters so the behavior 
*        of the APIs in illegal parameters case is dependent on the operating system behavior.
*
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/** 
 * @brief This function purpose is to return the string length (Null terminated buffer)
 *         
 *
 * @param[in] aStr - Pointer to string 
 *
 * @return The string length
 */
DxUint32_t DX_PAL_StrLen(DxChar_t *aStr); 

/* Definition for StrLen */
#define DX_PAL_StrLen   _DX_PAL_StrLen

/** 
 * @brief This function purpose is to find the string specified in aStrSearchFor in the 
 *        string specified in aStrSearchIn
 *         
 *
 * @param[in] aStrSearchIn - Pointer to string to search in 
 * @param[in] aStrSearchFor - Pointer to string to search for in aStrSearchIn 
 *
 * @return Pointer to the first occurrence of searchFor in searchIn or DX_NULL if string
 *	       was not found.
 */
DxChar_t* DX_PAL_FindStr(const DxChar_t* aStrSearchIn, const DxChar_t* aStrSearchFor  ); 

/* Definition for */
#define DX_PAL_FindStr  _DX_PAL_FindStr

/** 
 * @brief This function purpose is to find the first appearance of aChr in aStr.
 *         
 *
 * @param[in] aStr - Pointer to string 
 * @param[in] aChr - Char to look for in string 
 *
 * @return A pointer to the first appearance of aChr in aStr, if not found NULL is returned
 */
DxChar_t* DX_PAL_StrChr(DxChar_t *aStr, DxChar_t aChr  );

/* Definition for StrChr */
#define DX_PAL_StrChr _DX_PAL_StrChr

/** 
 * @brief This function purpose is to find the last appearance of aChr in aStr
 *         
 *
 * @param[in] aStr - Pointer to string 
 * @param[in] aChr - Char to look for in string 
 *
 * @return A pointer to the last appearance of aChr in aStr, if not found NULL is returned
 */
DxChar_t* DX_PAL_StrRChr(DxChar_t *aStr, DxChar_t aChr  );

/* Definition for StrRChr */
#define DX_PAL_StrRChr  _DX_PAL_StrRChr

/** 
 * @brief This function purpose is to copy aSize bytes from aSrcStr to aDestStr.          
 *         
 *
 * @param[in] aDestStr - Pointer to destination string
 * @param[in] aSrcStr - Pointer to source string (to copy from)
 * @param[in] aSize - Number of bytes to copy  
 *
 * @return A pointer to destination string
 */
DxChar_t* DX_PAL_StrNCopy(	DxChar_t *aDestStr, DxChar_t *aSrcStr, DxUint32_t aSize );

/* Definition for StrNCopy */
#define DX_PAL_StrNCopy  _DX_PAL_StrNCopy

    
#ifdef __cplusplus
}
#endif

#endif


