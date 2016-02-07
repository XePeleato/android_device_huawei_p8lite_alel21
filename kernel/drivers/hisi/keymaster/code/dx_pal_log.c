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
#include "dx_pal_types.h"
#include "dx_pal_log.h"


int DX_PAL_logLevel = DX_PAL_MAX_LOG_LEVEL;
DxUint32_t DX_PAL_logMask = 0xFFFFFFFF;
void DX_PAL_LogInit(void)
{
	//2012/11/5,modified by lixiaojie
	//openlog("DX_PAL_LOG", SYSLOG_OPTIONS, LOG_USER);
}

void DX_PAL_LogLevelSet(int setLevel)
{
	DX_PAL_logLevel = setLevel;
}

void DX_PAL_LogMaskSet(DxUint32_t setMask)
{
	DX_PAL_logMask = setMask;
}
