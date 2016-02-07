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

 
#ifndef LLF_PKI_CC54_HW_DEFS_H
#define LLF_PKI_CC54_HW_DEFS_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object % llf_pki_cc54_hw_defs.h    : %
   *  State           :  %state%
   *  Creation date   :  27 Dec. 2007
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This file contains the HW addresses and macros, relevant to the PKI module.
   *
   *  \version llf_pki_cc54_hw_defs.h#1:incl:1
   *  \author R.Levin
   */

#include "crys_rsa_types.h"
#include "llf_pki_defs.h"
#include "dx_crys_kernel.h"
//#include "dx_host.h"
#include "dx_cc_regs.h"
#include "bsp.h"
#include "crys_common.h"
#include "dx_hal_plat.h"


DxUint32_t g_MemOffsetAddr;

/************************ Defines ******************************/

/* The maximal size of allowed PKA physical registers memory and tables */
#define LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES                     4096 /*PKA_MAX_REGS_MEM_SIZE_BYTES*/
#define LLF_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS                    32 /*PKA_MAX_COUNT_OF_PHYS_MEM_REGS*/

/* minimal and maximal allowed size of PKA data memory registers in bits */
#define LLF_PKI_PKA_MIN_OPERATION_SIZE_BITS                       97
#ifndef CRYS_NO_RSA_MAX_KEY_SIZE_3072_BIT_SUPPORT  
	#define LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS                  3072
#else
	#define LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS                  2048

#endif

/* maximal size of extended register */
#define LLF_PKI_PKA_MAX_REGISTER_SIZE_WORDS ((LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS/32) + 1)

/* maximal size of key, which may be generated on present HW */
#define LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS                2048

/* The maximal count of allowed sizes of PKA operands or register-variables */
#define LLF_PKI_PKA_MAX_COUNT_OF_REGS_SIZES                        8 /*PKA_MAX_COUNT_OF_REGS_SIZES */

/* the HW address space */
#define LLF_PKI_HW_CRYPTO_ADDR_SPACE                            ( 1UL << 12 )


/* PKA data registers base address */
/*should be always zero since it's the offset from the start of the PKA memory and not from the HOST memory*/
#define LLF_PKI_PKA_DATA_REGS_BASE_ADDR                    g_MemOffsetAddr
#define LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR g_MemOffsetAddr


/*!!!!!! must be reconsidered !!!! PKA accsessible data memory offset above the LLF_PKI_PKA_BASE_ADDR */
#define LLF_PKI_PKA_START_MEM_ADDR_REG_ADDR                    0x00D0UL /*TBD !!!!!! NOW Not defined in CRS*/
#define LLF_PKI_PKA_START_MEM_ADDR                             LLF_PKI_PKA_DATA_REGS_BASE_ADDR /*TBD !!!!!*/

/* PKA OPCODE register fields positions (low bit position) */
#define LLF_PKI_PKA_OPCODE_TAG_POS                               0
#define LLF_PKI_PKA_OPCODE_RESULT_POS                            6
#define LLF_PKI_PKA_OPCODE_R_DISCARD_POS                        11
#define LLF_PKI_PKA_OPCODE_OPERAND_2_POS                        12
#define LLF_PKI_PKA_OPCODE_OPERAND_2_IMMED_POS                  17
#define LLF_PKI_PKA_OPCODE_OPERAND_1_POS                        18
#define LLF_PKI_PKA_OPCODE_OPERAND_1_IMMED_POS                  23
#define LLF_PKI_PKA_OPCODE_LEN_POS                              24
#define LLF_PKI_PKA_OPCODE_OPERATION_ID_POS                     27         

/* PKA STATUS register fields positions (low bit position) */
#define LLF_PKI_PKA_STATUS_PIPE_IS_REDY_POS                      0
#define LLF_PKI_PKA_STATUS_PKA_BUSY_POS                          1
#define LLF_PKI_PKA_STATUS_ALU_OUT_ZERO_POS                      2   
#define LLF_PKI_PKA_STATUS_ALU_MODOVRFLW_POS                     3        
#define LLF_PKI_PKA_STATUS_DIV_BY_ZERO_POS                       4
#define LLF_PKI_PKA_STATUS_ALU_CARRY_POS                         5
#define LLF_PKI_PKA_STATUS_ALU_SIGN_OUT_POS                      6
#define LLF_PKI_PKA_STATUS_MODINV_OF_ZERO_POS                    7 
#define LLF_PKI_PKA_STATUS_PKA_CPU_BUSY_POS                      8
#define LLF_PKI_PKA_STATUS_OPCODE_POS                            9
#define LLF_PKI_PKA_STATUS_TAG_POS                              14

#define LLF_PKI_PKA_STATUS_OPCODE_MASK                      0x1FUl
#define LLF_PKI_PKA_STATUS_TAG_MASK                         0x3FUl


/* PKA N_NP_T0_T1 register fields positions (low bit position) */
#define LLF_PKI_PKA_N_NP_T0_T1_REG_N_POS                       DX_N_NP_T0_T1_ADDR_N_VIRTUAL_ADDR_BIT_SHIFT
#define LLF_PKI_PKA_N_NP_T0_T1_REG_NP_POS                      DX_N_NP_T0_T1_ADDR_NP_VIRTUAL_ADDR_BIT_SHIFT
#define LLF_PKI_PKA_N_NP_T0_T1_REG_T0_POS                      DX_N_NP_T0_T1_ADDR_T0_VIRTUAL_ADDR_BIT_SHIFT
#define LLF_PKI_PKA_N_NP_T0_T1_REG_T1_POS                      DX_N_NP_T0_T1_ADDR_T1_VIRTUAL_ADDR_BIT_SHIFT

/* PKA N_NP_T0_T1 register default (reset) value: N=0, NP=1, T0=30, T1=31 */
#define PKA_N                                                    0UL
#define PKA_NP                                                   1UL
#define PKA_T0                                                  30UL
#define PKA_T1                                                  31UL
#define LLF_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL                  ( PKA_N  << LLF_PKI_PKA_N_NP_T0_T1_REG_N_POS  | \
                                                                  PKA_NP << LLF_PKI_PKA_N_NP_T0_T1_REG_NP_POS | \
                                                                  PKA_T0 << LLF_PKI_PKA_N_NP_T0_T1_REG_T0_POS | \
                                                                  PKA_T1 << LLF_PKI_PKA_N_NP_T0_T1_REG_T1_POS )

/* PKA control values  */
#define LLF_PKI_PKA_PIPE_READY                                   1
#define LLF_PKI_PKA_OP_DONE                                      1
#define LLF_PKI_PKA_SW_REST                                      1


/* Machine Opcodes definitions (according to HW CRS ) */
#define   LLF_PKI_PKA_MIN_OPCODE 0x00

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

#define   LLF_PKI_PKA_MAX_OPCODE 0x17


/* **********************           MACROS          ******************************* */

/*************************************************************/
/* Macros for waiting PKA machine ready states               */
/*************************************************************/


/* defining a macro for waiting to the PKA_PIPE_READY */
#define LLF_PKI_WAIT_ON_PKA_PIPE_READY( VirtualHwBaseAddr ) \
do \
{ \
   volatile DxUint32_t output_reg_val; \
   for(output_reg_val = 0; output_reg_val < 2 ; output_reg_val++);\
   do \
   { \
      output_reg_val = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, PKA_PIPE_RDY) ); \
   }while( (output_reg_val & 0x01) != LLF_PKI_PKA_PIPE_READY ); \
}while(0)    


/* defining a macro for waiting to the PKA_OP_DONE */
#define LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ) \
do \
{ \
   volatile DxUint32_t output_reg_val; \
   for(output_reg_val = 0; output_reg_val < 2 ; output_reg_val++);\
   do \
   { \
      output_reg_val = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_DONE) ); \
   }while( (output_reg_val & 0x01) != LLF_PKI_PKA_OP_DONE ); \
}while(0) 
   

/**************************************************************/
/*  Macros for controlling PKA machine and changing           */
/*  PKA sizes table and mapping table settings.               */
/**************************************************************/

/*  Set_StartAddress_REG:  Sets user passed start address for PKA accessible 
    data memory registers into special PKA register */
#define   LLF_PKI_PKA_Set_StartMemAddr_Reg(PKA_StartMemAddr , VirtualHwBaseAddr) \
		DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, PKA_MON_READ), \
                             (DxUint32_t)(PKA_StartMemAddr) )   

/*  Set_N_NP_T0_T1_REG:  Sets addresses of registers N,NP,T0,T1 into special register */
#define   LLF_PKI_PKA_Set_N_NP_T0_T1_Reg(N, NP, T0, T1, VirtualHwBaseAddr) \
        LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
        DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), \
       (DxUint32_t)( (N) << LLF_PKI_PKA_N_NP_T0_T1_REG_N_POS   | \
                    (NP) << LLF_PKI_PKA_N_NP_T0_T1_REG_NP_POS  | \
                    (T0) << LLF_PKI_PKA_N_NP_T0_T1_REG_T0_POS  | \
                    (T1) << LLF_PKI_PKA_N_NP_T0_T1_REG_T1_POS  ) )

/*  Set default N_NP_T0_T1_REG:  Sets default values of registers addresses 
    N=0, NP=1, T0=30, T1=31 into special register N_NP_T0_T1_REG */
#define   LLF_PKI_PKA_SetDefault_N_NP_T0_T1_Reg(VirtualHwBaseAddr)  \
		DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), \
                              LLF_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL)

/* GET_PKA_STATUS  - this macro returns the value contained in PKA_STATUS register */
#define LLF_PKI_PKA_GET_STATUS(VirtualHwBaseAddr, status)   \
{ \
   LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
   status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
}
  
/* GET_PKA_StatusAluOutZero - this macro returns the ALU Zero-bit from PKA_STATUS register */
#define LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status)  \
{ \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> LLF_PKI_PKA_STATUS_ALU_OUT_ZERO_POS ) & 1UL; \
}
    
/* GET_PKA_StatusModOverfl - returns the ModularOverflow from the PKA_STATUS reg;
   means, that modular add or subtract operations result is grate, than modulus (in modAdd) 
   or negative (in modSub). For other operations, such modMul, modInv this bit is not relevant */
#define LLF_PKI_PKA_GET_StatusModOverfl(VirtualHwBaseAddr, status)  \
{ \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> LLF_PKI_PKA_STATUS_ALU_MODOVRFLW_POS ) & 1; \
}

/* GET_PKA_StatusDivByZero - this macro returns the DividerIsZero-bit from PKA_STATUS register */
#define LLF_PKI_PKA_GET_StatusDivByZero(VirtualHwBaseAddr, status)  \
{ \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> LLF_PKI_PKA_STATUS_DIV_BY_ZERO_POS ) & 1; \
}

/* GET_PKA_StatusCarry - this macro returns the ALU Carry-bit from PKA_STATUS register */
#define LLF_PKI_PKA_GET_StatusCarry(VirtualHwBaseAddr, status)  \
{ \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> LLF_PKI_PKA_STATUS_ALU_CARRY_POS ) & 1; \
}

/* GET_PKA_StatusAluOutNeg - this macro returns the ALU out Negative-bit from PKA_STATUS register */
#define LLF_PKI_PKA_GET_StatusAluSignOut(VirtualHwBaseAddr, status)  \
{ \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> LLF_PKI_PKA_STATUS_ALU_SIGN_OUT_POS ) & 1; \
}

/* GET_PKA_StatusModInvOfZero - this macro returns the OperandIsZero-bit from PKA_STATUS register */
#define LLF_PKI_PKA_GET_StatusModInvOfZero(VirtualHwBaseAddr, status)  \
{ \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> LLF_PKI_PKA_STATUS_MODINV_OF_ZERO_POS ) & 1; \
}

/* GET_PKA_StatusOpCode - this macro returns the Opcode from PKA_STATUS register */
#define LLF_PKI_PKA_GET_StatusOpcode(VirtualHwBaseAddr, status)  \
{ \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> LLF_PKI_PKA_STATUS_OPCODE_POS ) & LLF_PKI_PKA_STATUS_OPCODE_MASK; \
}

/* GET_PKA_StatusTag - this macro returns the Tag from PKA_STATUS register */
#define LLF_PKI_PKA_GET_StatusTag(VirtualHwBaseAddr, status)  \
{ \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    status = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> LLF_PKI_PKA_STATUS_TAG_POS ) & LLF_PKI_PKA_STATUS_TAG_MASK; \
}

 
/******************************************************************/
/* Macros for setting and reading sizes from PKA regsSizesTable   */
/******************************************************************/

/*  SetRegSizeInTable:  Sets the size into regsSizesTable entry */
#define LLF_PKI_PKA_SetRegSize(SizeBits, EntryNum, VirtualHwBaseAddr) \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    DX_HAL_WriteCcRegister( (DX_CC_REG_OFFSET (CRY_KERNEL ,PKA_L0) + 4*(EntryNum)), \
                                 (SizeBits) )
/*  ReadRegSizeInTable:  Readss the size from regsSizesTable entry */
#define LLF_PKI_PKA_ReadRegSize(SizeBits, EntryNum, VirtualHwBaseAddr) \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    SizeBits = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL ,PKA_L0)+ 4*(EntryNum))
/******************************************************************/
/* Macros for setting and reading addresses of PKA data registers */
/******************************************************************/

/*The following macros are used for setting and reading the data registers addresses in mapping table.*/

/*  SetRegAddress:  Sets the physical address PhysAddr of register VirtReg in mapping table */
#define LLF_PKI_PKA_SetRegAddress(VirtReg, PhysAddr, VirtualHwBaseAddr) \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    DX_HAL_WriteCcRegister( (DX_CC_REG_OFFSET (CRY_KERNEL ,MEMORY_MAP0) + 4*(VirtReg)),\
                                 (PhysAddr) )

/*  GetRegAddress:  Returns the physical address of register VirtReg from mapping table  */
#define LLF_PKI_PKA_GetRegAddress(VirtReg,PhysAddr, VirtualHwBaseAddr) \
	(PhysAddr) = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET (CRY_KERNEL ,MEMORY_MAP0) + 4*(VirtReg))
	
#define LLF_PKI_PKA_ReadRegAddress(VirtReg, PhysAddr, VirtualHwBaseAddr) \
    LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    (PhysAddr) = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET (CRY_KERNEL,MEMORY_MAP0) + 4*(VirtReg))
                             
/******************************************************************/
/*          Macros for setting Full PKI opcode                    */
/******************************************************************/
#define LLF_PKI_PKA_FullOpCode( Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag ) \
   ( ((DxUint32_t)(Opcode)&31)      << LLF_PKI_PKA_OPCODE_OPERATION_ID_POS     | \
     ((DxUint32_t)(LenID)&7)        << LLF_PKI_PKA_OPCODE_LEN_POS              | \
     ((DxUint32_t)(IsAImmed)&1)     << LLF_PKI_PKA_OPCODE_OPERAND_1_IMMED_POS  | \
     ((DxUint32_t)(OpA)&31)         << LLF_PKI_PKA_OPCODE_OPERAND_1_POS        | \
     ((DxUint32_t)(IsBImmed)&1)     << LLF_PKI_PKA_OPCODE_OPERAND_2_IMMED_POS  | \
     ((DxUint32_t)(OpB)&31)         << LLF_PKI_PKA_OPCODE_OPERAND_2_POS        | \
     ((DxUint32_t)(ResDiscard)&1)   << LLF_PKI_PKA_OPCODE_R_DISCARD_POS        | \
     ((DxUint32_t)(Res)&31)         << LLF_PKI_PKA_OPCODE_RESULT_POS           | \
     ((DxUint32_t)(Tag)&31)         << LLF_PKI_PKA_OPCODE_TAG_POS  )             
	
 
/******************************************************************/
/*          Macros for reading and loading PKA memory data        */
/******************************************************************/
                  
#ifndef CRYS_PKA_SRAM_DATA_MEMORY_MODE
/* MACRO DEFINITIONS FOR WORKING WITH DIRECT ACCESS TO PKA DATA REGISTERS */

	/* defining a macro to load a value to the PKA data memory */
	#define LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	   /**(DxUint32_t*)((DxUint8_t*)(Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) = ((DxUint32_t)Val); \*/ \
       *(DxUint32_t*)((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) = ((DxUint32_t)Val); \
	}while(0)
	/* defining a macro to load a block to the PKA data memory */
	#define LLF_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	    DX_VOS_FastMemCpy((DxUint8_t*)((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(DxUint8_t*)(ptr),(SizeWords)*sizeof(DxUint32_t)); \
	}while(0) 
	 
	/* defining a macro to reversive loading a block to the PKA data memory */
	#define LLF_PKI_HW_REVERSE_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	    CRYS_COMMON_ReverseMemcpy((DxUint8_t*)((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(DxUint8_t*)(ptr),(SizeWords)*sizeof(DxUint32_t)); \
	}while(0)   
  

	/* defining a macro to clear PKA data memory */
	#define LLF_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , Addr , SizeWords ) \
	do \
	{ \
	    DX_VOS_MemSetZero((DxUint8_t*)((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(SizeWords)*sizeof(DxUint32_t)); \
	}while(0)   

	/* defining a macro to read a value from the PKA data memory */
	#define LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	    Val = *(DxUint32_t*)((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR); \
	}while(0)
	/* defining a macro to read a block from the PKA data memory */
	#define LLF_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	    DX_VOS_FastMemCpy((DxUint8_t*)(ptr),(DxUint8_t*)((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(SizeWords)*sizeof(DxUint32_t)); \
	}while(0)   

	/* defining a macro to read a block from the PKA data memory */
	#define LLF_PKI_HW_REVERSE_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	    CRYS_COMMON_ReverseMemcpy((DxUint8_t*)(ptr),(DxUint8_t*)((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(SizeWords)*sizeof(DxUint32_t)); \
	}while(0)   


#else
/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

	/*These registers  not included in the HW_CC definitions because they are HOST registers */

	/* defining a macro to load a value to the PKA data memory */        
	#define LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	   volatile DxUint32_t dummy; \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_ADDR),((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA), (Val) ); \
	   do \
	   { \
	   dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY)); \
	   }while(!(dummy & 0x1)); \
	}while(0)   

	/* defining a macro to load a block to the PKA data memory */
	#define LLF_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_ADDR),((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   for( ii = 0 ; ii < (SizeWords) ; ii++ ) \
	   { \
		   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA), (((DxUint32_t*)ptr)[ii])); \
		   do \
		   { \
		     dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY)); \
		   }while(!(dummy & 0x1)); \
	   } \
	}while(0)  
	
	/* defining a macro to reversive loading a block to the PKA data memory */
	#define LLF_PKI_HW_REVERSE_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_ADDR), ((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   for( ii = 0 ; ii < (SizeWords) ; ii++ ) \
	   { \
		   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA), CRYS_COMMON_INVERSE_UINT32_BYTES(((DxUint32_t*)ptr)[(SizeWords) - ii - 1]) ); \
		   do \
		   { \
		     dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY)); \
		   }while(!(dummy & 0x1)); \
	   } \
	}while(0)   
 

	/* defining a macro to clear PKA data memory */
	#define LLF_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , Addr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_ADDR) ,((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   for( ii = 0 ; ii < (DxUint32_t)(SizeWords) ; ii++ ) \
	   { \
	      DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA), 0x0UL ); \
	      do \
	      { \
	         dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY)); \
	      }while(!(dummy & 0x1)); \
	   }\
	}while(0)   

	/* defining a macro to read a value from the PKA data memory */
	#define LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	   volatile DxUint32_t dummy; \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_ADDR) ,((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA)); \
	   do \
	   { \
	      dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY)); \
	   }while(!(dummy & 0x1)); \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA), (Val) ); \
	   do \
	   { \
	      dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY) ); \
	   }while(!(dummy & 0x1)); \
	}while(0)   

	/* defining a macro to read a block from the PKA data memory */
	#define LLF_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_ADDR) ,((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA)); \
	   for( ii = 0 ; ii < (SizeWords) ; ii++ ) \
	   { \
	      do \
	      { \
	         dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY)); \
	      }while(!(dummy & 0x1)); \
	      DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA), ((DxUint32_t*)ptr)[ii] ); \
	   } \
	   do \
	   { \
	      dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY)); \
	   }while(!(dummy & 0x1)); \
	}while(0)   

	/* defining a macro to reversive reading a block from the PKA data memory */
	#define LLF_PKI_HW_REVERSE_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	   DxUint32_t ii; \
	   volatile DxUint32_t dummy; \
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_ADDR) ,((Addr) + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR) ); \
	   dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA) ); \
	   for( ii = 0 ; ii < (SizeWords)  ; ii++ ) \
	   { \
	      do \
	      { \
	         dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY), (dummy) ); \
	      }while(!(dummy & 0x1)); \
	      dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST,SRAM_DATA), dummy ); \
	      (ptr)[ (SizeWords) - 1 - ii] = CRYS_COMMON_INVERSE_UINT32_BYTES( dummy ); \
	   } \
	   do \
	   { \
	      dummy = DX_HAL_ReadCcRegister( DX_CC_REG_OFFSET (HOST, SRAM_DATA_READY), (dummy) ); \
	   }while(!(dummy & 0x1)); \
	}while(0)   

#endif  


 
/************************ Enums ********************************/

#ifdef __cplusplus
}
#endif

#endif


