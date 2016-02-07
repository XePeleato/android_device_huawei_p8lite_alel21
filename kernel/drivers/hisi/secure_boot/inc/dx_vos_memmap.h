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

 
 #ifndef _DX_VOS_MEMMAP_H
#define _DX_VOS_MEMMAP_H
	 
/*! \file DX_VOS_Memmap.h
    \brief This file enables mapping memory operations 
*/
	 
#include "dx_vos_basetypes.h"
 
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NUMBER_ENTRIES 100

#define DX_VOS_MemMap(aPhysicalAddress, aMapSize, aVirtualAddr) ( { CRYS_OK; } )

#define DX_VOS_MemUnMap(aVirtualAddr,  aMapSize ) ( { CRYS_OK; } )

#define DX_VOS_MemVirtualToPhysical(aVirtualAddr, aMapSize, aPhysicalAdrr ) ( { CRYS_OK; } )

#ifdef __cplusplus
}
#endif

#endif /*_DX_VOS_MEMMAP_H*/

