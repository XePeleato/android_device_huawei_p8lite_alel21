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

#include "dx_pal_sem.h"
/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/



/*typedef struct 
{
  DxUint32_t DX_PAL_SemId;
   
}_DX_PAL_Sem_t; */


/************************ Global Data ******************************/

/************************ Private Functions ******************************/
                                                    
/************************ Public Functions ******************************/

/** 
 * @brief This function purpose is to create a semaphore.
 *         
 *
 * @param[out] aSemId - Pointer to created semaphor handle
 * @param[in] aInitialVal - Initial semaphore value 
 *
 * @return The return values will be according to operating system return values.
 */		
DxError_t _DX_PAL_SemCreate( DX_PAL_SEM *aSemId, DxUint32_t aInitialVal )
{
  
  *aSemId = (DX_PAL_SEM)1;
  
  return DX_SUCCESS;
  
}/* End of DX_PAL_SemCreate */

/** 
 * @brief This function purpose is to delete a semaphore
 *         
 *
 * @param[in] aSemId - Semaphore handle 
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t _DX_PAL_SemDelete( DX_PAL_SEM *aSemId )
{
  
  *aSemId = (DX_PAL_SEM)0;
  return DX_SUCCESS;
  
}/* End of DX_PAL_SemDelete */

/** 
 * @brief This function purpose is to Wait for semaphore with aTimeOut. aTimeOut is 
 *        specified in milliseconds.
 *         
 *
 * @param[in] aSemId - Semaphore handle
 * @param[in] aTimeOut - timeout in mSec, or DX_INFINITE
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t _DX_PAL_SemWait(DX_PAL_SEM aSemId, DxUint32_t aTimeOut)
{
  return DX_SUCCESS;
}/* End of DX_PAL_SemWait */

/** 
 * @brief This function purpose is to signal the semaphore.
 *         
 *
 * @param[in] aSemId - Semaphore handle 
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t _DX_PAL_SemGive(DX_PAL_SEM aSemId)
{
  return DX_SUCCESS;
}/* End of DX_PAL_SemGive */
