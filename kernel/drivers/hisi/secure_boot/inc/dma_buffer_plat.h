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

#ifndef __DMA_BUFFER_PLAT_H__
#define __DMA_BUFFER_PLAT_H__

/* Host pointer is always "smart pointer" hence, no manipulation has
   to be made but compiling to an empty macro */
#define IS_SMART_PTR(ptr) (0)
#define PTR_TO_DMA_BUFFER(ptr)  ((DmaBuffer_s *)((uint32_t)(ptr)))
#define DMA_BUFFER_TO_PTR(pDmaBuffer) ((void*)(((uint32_t)(pDmaBuffer))))
#endif /*__DMA_BUFFER_PLAT_H__*/
