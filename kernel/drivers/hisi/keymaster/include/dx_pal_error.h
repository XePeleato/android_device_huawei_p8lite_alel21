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



#ifndef _DX_PAL_ERROR_H
#define _DX_PAL_ERROR_H


#ifdef __cplusplus
extern "C"
{
#endif

#define DX_PAL_BASE_ERROR                0x0F000000

/* Memory error returns */
#define DX_PAL_MEM_BUF1_GREATER          DX_PAL_BASE_ERROR + 0x01UL
#define DX_PAL_MEM_BUF2_GREATER          DX_PAL_BASE_ERROR + 0x02UL

/* Semaphore error returns */
#define DX_PAL_SEM_CREATE_FAILED         DX_PAL_BASE_ERROR + 0x03UL
#define DX_PAL_SEM_DELETE_FAILED         DX_PAL_BASE_ERROR + 0x04UL
#define DX_PAL_SEM_WAIT_TIMEOUT          DX_PAL_BASE_ERROR + 0x05UL                   
#define DX_PAL_SEM_WAIT_FAILED           DX_PAL_BASE_ERROR + 0x06UL                   
#define DX_PAL_SEM_RELEASE_FAILED        DX_PAL_BASE_ERROR + 0x07UL                   

#ifdef __cplusplus
}
#endif

#endif


