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


#include "secureboot_basetypes.h"
#include "systemdep.h"
#include "secureboot_general_hwdefs.h"
#include "rsa_hwdefs.h"
#include "secureboot_error.h"

/**
 * @brief      This function acquires lock for accessing the CC. This function is polling until the lock is acquired
 * 
 * @param[in] hwBaseAddress        -  cryptocell base address
 *
 * @return    DxError:  
 *                        
 */
DxError_t GEN_AcquireCCLock( DxUint32_t hwBaseAddress )
{
	/* register value */
	DxUint32_t temp = 0;

	/* in loop until sep not busy */
	do
	{
		SYS_ReadRegister( SB_REG_ADDR(hwBaseAddress, LCS_IS_VALID), temp);
	}while( !(temp & 0x1));


	#ifdef BIG__ENDIAN
	SYS_WriteRegister(SB_REG_ADDR(hwBaseAddress, HOST_RGF_ENDIAN)  ,0xCCUL );
	#endif

	return DX_OK;
}

/**
 * @brief      This function acquires lock for accessing the CC. This function is polling until the lock is acquired
 * 
 * @param[in] hwBaseAddress        -  cryptocell base address
 *
 * @return     DxError:  
 *                        
 */
void GEN_ReleaseCCLock( DxUint32_t hwBaseAddress )
{

	/* disable the CRYPTO clocks */
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, AES_CLK_ENABLE)  ,0x0UL );
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HASH_CLK_ENABLE) ,0x0UL );
	SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, PKA_CLK_ENABLE)  ,0x0UL );
     
	return;
}

