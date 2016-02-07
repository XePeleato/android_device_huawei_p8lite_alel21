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



#ifndef _SBROM_MNG_HW_DEFS_H
#define _SBROM_MNG_HW_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif


/* ********************** Macros ******************************* */

/* Poll on the AIB acknowledge bit */
#define DX_MNG_WAIT_ON_AIB_ACK_BIT(hwBaseAddress) 						\
	do { 											\
		DxUint32_t regVal; 								\
		do { 										\
			SYS_ReadRegister( SB_REG_ADDR(hwBaseAddress, AIB_FUSE_ACK) , regVal ); 	\
		}while( !(regVal & 0x1));							\
	}while(0)


/* Poll on the AIB prog complete bit */
#define DX_MNG_WAIT_ON_AIB_PROG_COMP_BIT(hwBaseAddress) 					\
	do { 											\
		DxUint32_t regVal; 								\
		do { 										\
			SYS_ReadRegister( SB_REG_ADDR(hwBaseAddress, AIB_FUSE_PROG_COMPLETED) , regVal ); \
		}while( !(regVal & 0x1 )); 							\
	}while(0)


/* Poll on the LCS valid register */
#define DX_MNG_WAIT_ON_LCS_VALID_BIT(hwBaseAddress) 						\
	do { 											\
		DxUint32_t regVal; 								\
		do { 										\
			SYS_ReadRegister( SB_REG_ADDR(hwBaseAddress, LCS_IS_VALID), regVal); 	\
		}while( !(regVal & 0x1)); 							\
	}while(0)


/* Check session key validity */
#define DX_MNG_IS_SESSION_KEY_VALID(hwBaseAddress, regVal) 					\
	do { 											\
		SYS_ReadRegister(SB_REG_ADDR(hwBaseAddress, HOST_SEESION_KEY_IS_VALID), regVal);\
		regVal = (regVal >> DX_HOST_SEESION_KEY_IS_VALID_VALUE_BIT_SHIFT) & DX_HOST_SEESION_KEY_IS_VALID_VALUE_BIT_SIZE; \
	}while(0)


/* Check KDR error bit in LCS register */
#define DX_MNG_IS_OTP_KDR_ERROR(hwBaseAddress, regVal) 						\
	do { 											\
		SYS_ReadRegister(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);			\
		regVal = (regVal >> DX_LCS_REG_ERROR_KDR_ZERO_CNT_BIT_SHIFT) & DX_LCS_REG_ERROR_KDR_ZERO_CNT_BIT_SIZE;	\
	}while(0)


/* Check Provision error bit in LCS register */
#define DX_MNG_IS_OTP_PROV_ERROR(hwBaseAddress, regVal) 					\
	do { 											\
		SYS_ReadRegister(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);			\
		regVal = (regVal >> DX_LCS_REG_ERROR_PROV_ZERO_CNT_BIT_SHIFT) & DX_LCS_REG_ERROR_PROV_ZERO_CNT_BIT_SIZE; \
	}while(0)


/* Poll on the AIB acknowledge bit */
#define DX_MNG_READ_WORD_FROM_AIB(hwBaseAddress, nvmAddr, nvmData)				\
	do { 											\
		SYS_WriteRegister( SB_REG_ADDR(hwBaseAddress, HOST_AIB_ADDR_REG), nvmAddr); 	\
		DX_MNG_WAIT_ON_AIB_ACK_BIT(hwBaseAddress);                                    	\
		SYS_ReadRegister( SB_REG_ADDR(hwBaseAddress, HOST_AIB_RDATA_REG), nvmData);  	\
	}while(0)

#ifdef __cplusplus
}
#endif

#endif



