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

 
#ifndef RSA_HWDEFS_H
#define RSA_HWDEFS_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "secureboot_general_hwdefs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/
/* The maximal size of allowed PKA physical registers memory and tables */
#define RSA_HW_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES                     4096 /*PKA_MAX_REGS_MEM_SIZE_BYTES*/
#define RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS                    32 /*PKA_MAX_COUNT_OF_PHYS_MEM_REGS*/

/* minimal and maximal allowed size of PKA data memory registers in bits */
#define RSA_HW_PKI_PKA_MIN_OPERATION_SIZE_BITS                       97
#define RSA_HW_PKI_PKA_MAX_OPERATION_SIZE_BITS                     2112

/* maximal size of extended register */
#define RSA_HW_PKI_PKA_MAX_REGISTER_SIZE_WORDS ((RSA_HW_PKI_PKA_MAX_OPERATION_SIZE_BITS/32) + 1)

/* The maximal count of allowed sizes of PKA operands or register-variables */
#define RSA_HW_PKI_PKA_MAX_COUNT_OF_REGS_SIZES                        8 /*PKA_MAX_COUNT_OF_REGS_SIZES */

/* Mapping table of PKA physical registers */  
//#define RSA_HW_PKI_PKA_REGS_MAP_TAB_ADDR                          0x0000UL   

#define RSA_HW_PKI_PKA_REGS_SIZES_TAB_ADDR                        0x0090UL  

/* PKA data registers base address */
/*should be always zero since it's the offset from the start of the PKA memory and not from the HOST memory*/
#define RSA_HW_PKI_PKA_DATA_REGS_BASE_ADDR                    0x00000000UL

/* SRAM definitions */
#define DX_PKA_MEMORY_OFFSET_IN_SRAM			      0x200	
#define RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR           0 


/* PKA accsessible data memory offset above the RSA_HW_PKI_PKA_BASE_ADDR */
#define RSA_HW_PKI_PKA_START_MEM_ADDR_REG_ADDR                    0x00D0UL 
#define RSA_HW_PKI_PKA_START_MEM_ADDR                             RSA_HW_PKI_PKA_DATA_REGS_BASE_ADDR

/* PKA OPCODE register fields positions (low bit position) */
#define RSA_HW_PKI_PKA_OPCODE_TAG_POS                               0  
#define RSA_HW_PKI_PKA_OPCODE_RESULT_POS                            6  
#define RSA_HW_PKI_PKA_OPCODE_R_DISCARD_POS                        11
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_2_POS                        12
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_2_IMMED_POS                  17
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_1_POS                        18  
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_1_IMMED_POS                  23  	
#define RSA_HW_PKI_PKA_OPCODE_LEN_POS                              24  
#define RSA_HW_PKI_PKA_OPCODE_OPERATION_ID_POS                     27        



/* PKA N_NP_T0_T1 register fields positions (low bit position) */
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_N_POS                         DX_N_NP_T0_T1_ADDR_N_VIRTUAL_ADDR_BIT_SHIFT	
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_NP_POS                        DX_N_NP_T0_T1_ADDR_NP_VIRTUAL_ADDR_BIT_SHIFT	
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T0_POS                        DX_N_NP_T0_T1_ADDR_T0_VIRTUAL_ADDR_BIT_SHIFT	
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T1_POS                        DX_N_NP_T0_T1_ADDR_T1_VIRTUAL_ADDR_BIT_SHIFT   

/* PKA N_NP_T0_T1 register default (reset) value: N=0, NP=1, T0=30, T1=31 */
#define PKA_N                                                    0UL
#define PKA_NP                                                   1UL
#define PKA_T0                                                  30UL
#define PKA_T1                                                  31UL
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL                  ( PKA_N  << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_N_POS  | \
                                                                  PKA_NP << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_NP_POS | \
                                                                  PKA_T0 << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T0_POS | \
                                                                  PKA_T1 << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T1_POS )

/* PKA control values  */
#define RSA_HW_PKI_PKA_PIPE_READY                                   1
#define RSA_HW_PKI_PKA_OP_DONE                                      1
#define RSA_HW_PKI_PKA_SW_REST                                      1


/* Machine Opcodes definitions (according to HW CRS ) */
#define   RSA_HW_PKI_PKA_MIN_OPCODE 0x00

#define   PKA_Add           0x04
#define   PKA_AddIm         0x04
#define   PKA_Sub           0x05
#define   PKA_SubIm         0x05
#define   PKA_Neg           0x05
#define   PKA_ModAdd        0x06
#define   PKA_ModAddIm      0x06
#define   PKA_ModSub        0x07
#define   PKA_ModSubIm      0x07
#define   PKA_ModNeg        0x07
#define   PKA_AND           0x08
#define   PKA_Test0         0x08
#define   PKA_Clr0          0x08
#define   PKA_Clr           0x08
#define   PKA_OR            0x09
#define   PKA_Copy	        0x09
#define   PKA_SetBit0   	0x09
#define   PKA_XOR  	        0x0A
#define   PKA_Flip0	        0x0A
#define   PKA_InvertBits	0x0A
#define   PKA_Compare	    0x0A
#define   PKA_SHR0	        0x0C
#define   PKA_SHR1      	0x0D
#define   PKA_SHL0	        0x0E
#define   PKA_SHL1	        0x0F
#define   PKA_LMul	        0x10
#define   PKA_ModMul	    0x11
#define   PKA_ModMulNR	    0x12
#define   PKA_ModExp	    0x13
#define   PKA_Div 	        0x14
#define   PKA_ModInv 	    0x15
#define   PKA_HMul	        0x17
#define   PKA_Terminate     0x00

#define   RSA_HW_PKI_PKA_MAX_OPCODE 0x17


/* **********************           MACROS          ******************************* */

/*************************************************************/
/* Macros for waiting PKA machine ready states               */
/*************************************************************/


/* defining a macro for waiting to the PKA_PIPE_READY */
#define RSA_HW_PKI_WAIT_ON_PKA_PIPE_READY( VirtualHwBaseAddr ) \
do \
{ \
   volatile DxUint32_t output_reg_val; \
   for(output_reg_val = 0; output_reg_val < 10 ; output_reg_val++);\
   do \
   { \
      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, PKA_PIPE_RDY) , output_reg_val ); \
   }while( (output_reg_val & 0x01) != RSA_HW_PKI_PKA_PIPE_READY ); \
}while(0)


/* defining a macro for waiting to the PKA_OP_DONE */
#define RSA_HW_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ) \
do \
{ \
   volatile DxUint32_t output_reg_val; \
   for(output_reg_val = 0; output_reg_val < 10 ; output_reg_val++);\
   do \
   { \
      SYS_ReadRegister(SB_REG_ADDR(VirtualHwBaseAddr, PKA_DONE) , output_reg_val ); \
   }while( (output_reg_val & 0x01) != RSA_HW_PKI_PKA_OP_DONE ); \
}while(0) 
 
 
/******************************************************************/
/* Macros for setting and reading sizes from PKA regsSizesTable   */
/******************************************************************/

/*  ReadRegSizeInTable:  Readss the size from regsSizesTable entry */
#define RSA_HW_PKI_PKA_ReadRegSize(SizeBits, EntryNum, VirtualHwBaseAddr) \
    RSA_HW_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    SYS_ReadRegister( (SB_REG_ADDR(VirtualHwBaseAddr,PKA_L0) + 4*(EntryNum)), \
                                (SizeBits) )

/******************************************************************/
/* Macros for setting and reading addresses of PKA data registers */
/******************************************************************/

/*The following macros are used for setting and reading the data registers addresses in mapping table.*/

/*  GetRegAddress:  Returns the physical address of register VirtReg from mapping table  */
#define RSA_HW_PKI_PKA_GetRegAddress(VirtReg, VirtualHwBaseAddr) \
 ( *((volatile DxUint32_t*)(SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*(VirtReg))) )
 
#define RSA_HW_PKI_PKA_ReadRegAddress(VirtReg, PhysAddr, VirtualHwBaseAddr) \
    RSA_HW_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    SYS_ReadRegister( (SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*(VirtReg)), \
                                (PhysAddr) )
/******************************************************************/
/*          Macros for setting Full PKI opcode                    */
/******************************************************************/
#define RSA_HW_PKI_PKA_FullOpCode( Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag ) \
   ( ((DxUint32_t)(Opcode)&31)      << RSA_HW_PKI_PKA_OPCODE_OPERATION_ID_POS     | \
     ((DxUint32_t)(LenID)&7)        << RSA_HW_PKI_PKA_OPCODE_LEN_POS              | \
     ((DxUint32_t)(IsAImmed)&1)     << RSA_HW_PKI_PKA_OPCODE_OPERAND_1_IMMED_POS  | \
     ((DxUint32_t)(OpA)&31)         << RSA_HW_PKI_PKA_OPCODE_OPERAND_1_POS        | \
     ((DxUint32_t)(IsBImmed)&1)     << RSA_HW_PKI_PKA_OPCODE_OPERAND_2_IMMED_POS  | \
     ((DxUint32_t)(OpB)&31)         << RSA_HW_PKI_PKA_OPCODE_OPERAND_2_POS        | \
     ((DxUint32_t)(ResDiscard)&1)   << RSA_HW_PKI_PKA_OPCODE_R_DISCARD_POS        | \
     ((DxUint32_t)(Res)&31)         << RSA_HW_PKI_PKA_OPCODE_RESULT_POS           | \
     ((DxUint32_t)(Tag)&31)         << RSA_HW_PKI_PKA_OPCODE_TAG_POS  )             
	
 
/******************************************************************/
/*          Macros for reading and loading PKA memory data        */
/******************************************************************/
                  
/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

	/*These registers  not included in the HW_CC definitions because they are HOST registers */
#ifdef DX_SB_INDIRECT_SRAM_ACCESS
/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

	/*These registers  not included in the HW_CC definitions because they are HOST registers */

	#ifndef BIG__ENDIAN
	/* defining a macro to load a value to the PKA data memory */        
	#define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	   volatile DxUint32_t dummy; \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (Val) ); \
	   do \
	   { \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	}while(0)   
    #else
    #define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
    do \
	{ \
	   volatile DxUint32_t dummy; \
	   DxUint32_t tempVal; \
	   UTIL_ReverseMemCopy((DxUint8_t*)&tempVal , (DxUint8_t*)&Val , sizeof(DxUint32_t) ); \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (tempVal) ); \
	   do \
	   { \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	}while(0)   
    #endif
    
	#ifndef BIG__ENDIAN
	/* defining a macro to load a block to the PKA data memory */
	#define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   for( ii = 0 ; ii < (SizeWords) ; ii++ ) \
	   { \
		   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (ptr)[ii] ); \
		   do \
		   { \
		      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
		   }while(!(dummy & 0x1)); \
	   } \
	}while(0)
	
	#else
	#define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   DxUint32_t tempWord; \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   for( ii = 0 ; ii < (SizeWords) ; ii++ ) \
	   { \
		   UTIL_ReverseMemCopy((DxUint8_t*)&tempWord , (DxUint8_t*)&(ptr)[ii] , sizeof(DxUint32_t) ); \
		   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), tempWord ); \
		   do \
		   { \
		      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
		   }while(!(dummy & 0x1)); \
	   } \
	}while(0)
	#endif
	

	/* defining a macro to clear PKA data memory */
	#define RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , Addr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   for( ii = 0 ; ii < (DxUint32_t)(SizeWords) ; ii++ ) \
	   { \
	      SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), 0x0UL ); \
	      do \
	      { \
	         SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	      }while(!(dummy & 0x1)); \
	   }\
	}while(0)   

	#ifndef BIG__ENDIAN
	/* defining a macro to read a value from the PKA data memory */
	#define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	   volatile DxUint32_t dummy; \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (dummy) ); \
	   do \
	   { \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	   SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (Val) ); \
	   do \
	   { \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	}while(0)   
    
    #else
    
    #define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	   volatile DxUint32_t dummy; \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (dummy) ); \
	   do \
	   { \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	   SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (Val) ); \
	   do \
	   { \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	   UTIL_ReverseBuff((DxUint8_t*)&Val , sizeof(DxUint32_t) ); \
	}while(0)
    #endif
    
	#ifndef BIG__ENDIAN
	/* defining a macro to read a block from the PKA data memory */
	#define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (dummy) ); \
	   for( ii = 0 ; ii < (SizeWords) ; ii++ ) \
	   { \
	      do \
	      { \
	         SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	      }while(!(dummy & 0x1)); \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (ptr)[ii] ); \
	   } \
	   do \
	   { \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	}while(0)   

	#else

	#define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_ADDR) ,((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (dummy) ); \
	   for( ii = 0 ; ii < (SizeWords) ; ii++ ) \
	   { \
	      do \
	      { \
	         SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	      }while(!(dummy & 0x1)); \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA), (ptr)[ii] ); \
	      UTIL_ReverseBuff((DxUint8_t*)&(ptr)[ii] , sizeof(DxUint32_t) ); \
	   } \
	   do \
	   { \
	      SYS_ReadRegister( SB_REG_ADDR(VirtualHwBaseAddr, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	}while(0)
	#endif

#else //DX_SB_INDIRECT_SRAM_ACCESS
    	/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

	/*These registers  not included in the HW_CC definitions because they are HOST registers */

	/* defining a macro to load a value to the PKA data memory */        
	#define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	   *(DxUint32_t*)(((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)) = (DxUint32_t)Val; \
	}while(0)   

    
	/* defining a macro to load a block to the PKA data memory */
	#define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	    UTIL_MemCopy((DxUint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(DxUint8_t*)(ptr),(SizeWords)*sizeof(DxUint32_t)); \
	}while(0) 	
	

	/* defining a macro to clear PKA data memory */
	#define RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , Addr , SizeWords ) \
	do \
	{ \
	    UTIL_MemSet((DxUint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),0x0,(SizeWords)*sizeof(DxUint32_t)); \
	}while(0)   


	/* defining a macro to read a value from the PKA data memory */
	#define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	    Val = *(DxUint32_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR); \
	}while(0)   

    
	/* defining a macro to read a block from the PKA data memory */
	#define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	    UTIL_MemCopy((DxUint8_t*)(ptr),(DxUint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(SizeWords)*sizeof(DxUint32_t)); \
	}while(0)   

#endif //DX_SB_INDIRECT_SRAM_ACCESS

	
/************************ Defines ******************************/

/* ********************** Macros ******************************* */
  
/************************ Enums ********************************/

#ifdef __cplusplus
}
#endif

#endif


