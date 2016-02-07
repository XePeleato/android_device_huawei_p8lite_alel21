
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

 
#ifndef VALIDATE_CRYS_BYPASS_H
#define VALIDATE_CRYS_BYPASS_H

/*!
 * validate DMA object structure 
 *  
 * \param dmaObj the DMA object  
 * \param dataSize Data size given by the user (relevent only for SRAM data)
 *  
 * \return 0 on success, (-1) if ( dataSize != dma object data size), (-2) if sram pointer is out of range
 */

int validateParams(uint8_t *data, uint32_t dataSize);


#endif
