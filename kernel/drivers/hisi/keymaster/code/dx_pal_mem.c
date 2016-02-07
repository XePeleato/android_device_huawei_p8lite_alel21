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



/************* Include Files ****************/
#include "dx_pal_types.h"
#include "dx_pal_error.h"
#include "dx_pal_mem.h"
#include "dx_pal_memint.h"


/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************************ Private Functions ******************************/

                                                        
/************************ Public Functions ******************************/

/** 
 * @brief This function purpose is to perform secured memory comparison between two given 
 *        buffers according to given size. The function will compare each byte till aSize 
 *        number of bytes was compared even if the bytes are different.
 *        The function should be used to avoid security timing attacks.  
 *         
 *
 * @param[in] aTarget - The target buffer to compare
 * @param[in] aSource - The Source buffer to compare to
 * @param[in] aSize - Number of bytes to compare
 *
 * @return The function will return DX_SUCCESS in case of success, else errors from 
 *         DX_PAL_Error.h will be returned.
 */
DxError_t DX_PAL_SecMemCmp(	const DxUint8_t* aTarget,	
	                  	      const DxUint8_t* aSource,	
		                        DxUint32_t  aSize		)
{
  /* internal index */
  DxUint32_t i = 0;
  
  /* error return */
  DxUint32_t error = DX_SUCCESS;
  
  /*------------------  
      CODE
  -------------------*/
  
  /* Go over aTarget till aSize is reached (even if its not equal) */
  for (i = 0; i < aSize; i++)
  {
    if (aTarget[i] != aSource[i])
    {
      if (error != DX_SUCCESS)
        continue;
      else
      {
        if (aTarget[i] < aSource[i])
          error = DX_PAL_MEM_BUF2_GREATER;
        else
          error = DX_PAL_MEM_BUF1_GREATER;
      }
    }
  }
  
  return error;  
}/* End of DX_PAL_SecMemCmp */

