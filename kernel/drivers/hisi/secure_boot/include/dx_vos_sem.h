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

#ifndef _DX_VOS_SEM_H
#define _DX_VOS_SEM_H

#include "dx_pal_types.h"
#include "dx_pal_sem.h"
#include "dx_pal_types.h"
#ifdef __cplusplus
extern "C"
{
#endif

/*The functions below supporting SEM for Asymetric function (RSA & ECC), overrunning legacy code.*/


DxError_t DX_VOS_SemWait(DX_PAL_SEM aSemId, DxUint32_t aTimeout);

DxError_t DX_VOS_SemCreate (DX_PAL_SEM  *aSemId, DxUint_t  aInitialValue, const DxChar_t* aSemName);

DxError_t DX_VOS_SemDelete(DX_PAL_SEM *aSemId);

DxError_t DX_VOS_SemGive(DX_PAL_SEM aSemId);







#ifdef __cplusplus
}
#endif

#endif
