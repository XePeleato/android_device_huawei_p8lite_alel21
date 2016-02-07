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

 
#ifndef RSA_PKI_PKA_H
#define RSA_PKI_PKA_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "secureboot_basetypes.h"
#include "rsa_hwdefs.h"


#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : % RSA_PKI_PKA.h
   *  State           :  %state%
   *  Creation date   :  02 Jan. 2008
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version RSA_PKI_PKA.h#1:incl:3
   *  \author R.Levin
   *  \remarks Copyright (C) 2008 by Discretix Technologies Ltd.
   *           All Rights reserved
   */

/************************ Defines ******************************/


/* values for defining, that PKA entry is not in use */ 
#define PKA_SIZE_ENTRY_NOT_USED  0xFFFFFFFF
#define PKA_ADDRESS_ENTRY_NOT_USED  0xFFC

/* difine result discard value */
#define RES_DISCARD  0x3F

/* define NpCreateFlag values */
#define PKA_CreateNP  1
#define PKA_SetNP     0

/* default LenID-s definitions. Last 3 names used in RSA KG.  
   Note: extended sizes are in the entries LenID+1 (i.e. 1,3) accordingly */
#define LenIDn    0  /* for operation or modulus size*/
#define LenIDpq   2  /* for P, Q size */
#define LenID101  4  /* for P1,P2,Q1,Q2 101 bit size */
#define LenIDmin  6  /* for min allowed operation size (now 128 bit) */
#define LenIDmax   7  /* for max allowed operation size (now 2112 bit) */
                    
/* define of PKA Tag for debug printing of PKA operations result 
   into tempRes global buffer */
#define PKA_TAG_DebugPtint  1                   
                    

/************************ Enums ********************************/

/* defines enumerator for all allowed PKA operations */
typedef enum
{
	PKA_Terminate_ID    =  0,
	PKA_Add_ID          =  1,
	PKA_AddIm_ID        =  2,
	PKA_Sub_ID          =  3,
	PKA_SubIm_ID        =  4, 
	PKA_Neg_ID          =  5,

	PKA_ModAdd_ID       =  6,
	PKA_ModAddIm_ID     =  7,
	PKA_ModSub_ID       =  8,
	PKA_ModSubIm_ID     =  9,
	PKA_ModNeg_ID       = 10,

	PKA_And_ID          = 11,
	PKA_Tst0_ID         = 12,
	PKA_Clr0_ID         = 13,
	PKA_Clr_ID          = 14, 
	
	PKA_OR_ID           = 15,
	PKA_Copy_ID         = 16,
	PKA_SetBit0_ID      = 17,
	
	PKA_XOR_ID          = 18, 
	PKA_Flip0_ID        = 19,
	PKA_InvBits_ID      = 20,
	PKA_Compare_ID      = 21,
	
	PKA_SHR0_ID         = 22,
	PKA_SHR1_ID         = 23,
	PKA_SHL0_ID         = 24,
	PKA_SHL1_ID         = 25,
	        
	PKA_LMul_ID         = 26,
	PKA_HMul_ID         = 27,
	PKA_ModMul_ID       = 28,
	PKA_ModMulNR_ID     = 29,
	PKA_ModExp_ID       = 30,
	
	PKA_Div_ID          = 31,
	PKA_ModInv_ID       = 32,	
	
	PKA_OpOffMode		        
		
} RSA_HW_PKI_PKA_OpID_t;


/*  Enumerator, defining whether the functions performs default settings 
    of PKI registers, or takes they from user passed arguments  */
typedef enum
{
      DefaultRegs       = 0,
      SetRegs           = 1,
      RegsSetOffMode
      
} RSA_HW_PKI_PKA_SetRegs_t;


/************************ Typedefs  ****************************/

/* definition of the Registers sizes array type, used for initialisation 
   of the PKA sizes table */
typedef DxUint32_t RSA_HW_PKI_PKA_RegsSizes_t[RSA_HW_PKI_PKA_MAX_COUNT_OF_REGS_SIZES];               
                    

/************************ Structs  ******************************/

 /* This structure contains virtual pointers (numbers in range 0-31) of 
    PKI registers RegsNum[]  and registers physical addresses RegsAddr[]. 
    For example: RegsNum[3] = 5 and RegsAddr[3] = 0x400 means, that address of 
    register 5 is 0x400. The user may set into this structure only needed registers, 
    used in planned PKA operations. */

typedef  struct  
{
       DxUint32_t    RegsNum[RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS] ;
       DxUint32_t    RegsAddr[RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS] ;
}RSA_HW_PKI_PKA_RegsMap_t;

/************************ Public Variables **********************/

/************************ Public Functions ******************************/


/***********************************************************************************************/
/***********************************************************************************************/
/*                                                                                             */ 
/*                   RSA PKI FUNCTIONS, PERFORMING PKA OPERATIONS                              */
/*                                                                                             */
/***********************************************************************************************/
/***********************************************************************************************/



/*******************************************************************************/
/**********      RSA PKI PKA initialisation functions and macros      **********/
/*******************************************************************************/

/***********      RSA_HW_PKI_PKA_SetRegsSizesTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table. 
 *
 *   The function has two modes of working: default and user defined.
 *   1. User defined mod (IsDefaultMap = 0):
 *      The function receives a count of entries in the registers sizes table (minimum 1, maximum 8), 
 *      which must be set, and pointer to sizes array (sizes in bits). Then the function writes the 
 *      needed sizes into the table, placed in the HW defined address, and sets remaining entries 
 *      of the table equaled to PKA_SIZE_ENTRY_NOT_USED (for preventing wrong using not initialized).
 *   2. Default mode (IsDefaultMap = 1):
 *      The function sets sizes table as follows:
 *            -  tab[0] = MaxSizeBits; //maximal size, usually this is exact modulus size in bits
 *            -  tab[1] = 32*MaxSizeWords + 32.
 *            -  other entries = PKA_SIZE_ENTRY_NOT_USED, means - not used. 
 *
 *
 * @param[in] RegsSizes_ptr  -  A  pointer to array of defined type, containing register sizes  
 *                              in bits, which must be set. This argument is not used on default mode. 
 * @param[in] CountOfSizes   -  Count of sizes to set: must be minimum 1, maximum 8.
 * @param[in] MaxSizeBits    - Maximal size of operands in bits. The value must be in interval 
 *                           from 128 to 2112 bits. This argument is used on default mode only.  
 * @param[in] IsDefaultMap - Defines whether the default settings are used or not:
 *                           1 - default, 0 - settings from array RegsSizes_ptr. 
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 */
void RSA_HW_PKI_PKA_SetRegsSizesTab(
                             RSA_HW_PKI_PKA_RegsSizes_t   RegsSizes_ptr,    /*in*/   
                             DxUint32_t                CountOfSizes,     /*in*/
                             DxUint32_t                MaxSizeBits,      /*in*/
                             DxUint32_t                IsDefaultMap,     /*in*/ 
                             DxUint32_t                VirtualHwBaseAddr /*in*/); 
                                                      
                                                            
 
/***********      RSA_HW_PKI_PKA_SetRegsMapTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table. 
 *
 *   The function checks input parameters and sets the physical memory registers mapping-table 
 *   according to IsDefaultMap parameter, passed by user: 
 *     - if IsDefaultMap = 1, then the function sets default mapping. 
 *     - if IsDefaultMap = 0, then the mapping addresses should be taken from the mapping structure, 
 *       transferred by the pointer RegsMap_ptr and by count of registers to be set.
 *
 *    1. Default mapping is following:
 *     - start address of register 0 is the start address of PKA data registers memory 
 *       (defined in HwDefs file); 
 *     - special registers are set as follows: N=0,NP=1,T0=30,T1=31, therefore the N_NP_T0_T1 
 *       parameter on default mode is ignored;
 *     - all registers have the same size, equalled to maximal allowed size of registers + 32 bits;
 *     - count of registers (including 4 special registers) is calculated according to maximal 
 *       size of registers and total size of data registers memory;
 *     - for calling the function on default mode user may use the macros, defined in RSA_PKI.h file.  
 *     
 *    2. The mapping, defined by user through the RegsMap_ptr structure:
 *     - the user sets count of needed registers (including special registers N,NP,T0,T1);  
 *     - the user sets virtual pointers (numbers in range 0-31) of all needed registers and their 
 *       physical addresses (offsets from virtual base address) into RegsMap_ptr structure;
 *     - the user sets numbers of special registers N,NP,T0,T1 (5 bits each one), concatenated 
 *       together, into input parameter N_NP_T0_T1;
 *     - the function writes the user passed memory mapping into PKA mapping table and sets the special 
 *       register N_NP_T0_T1_Reg.
 * 
 *
 * @param[in] RegsMap_ptr  - A pointer to structure, containing the virtual and physical 
 *                           addresses of needed PKA registers (including special registers N,NP,T0,T1).  
 * @param[in/out] CountOfRegs_ptr  - A pointer to count of all registeres to set: the value must be 
 *                           minimum 1, maximum 32 (including 4 special registers N,NP,T0,T1). 
 *                           On default mode the function returns trough this pointer the actual 
 *                           allowed count of PKA registers
 * @param[in] MaxSizeWords - Maximal size of operands in words. The value must be in interval 
 *                           from 4 to 66 words. Note: the function allocates one additional word 
 *                           for each register, so that their sizes = MaxSizeWords + 1. 
 * @param[in] N_NP_T0_T1   - Virtual pointers (numbers) of special registers (5 bits each one,  
 *                           concatenated together and placed into 4-bytes word).   
 * @param[in] IsDefaultMap - Defines whether the default settings are used or not:
 *                           1 - default, 0 - settings from structure RegsMap_ptr
 * @param[in] VirtualHwBaseAddr - Virtual HW base address, passed by user.
 *
 */
void RSA_HW_PKI_PKA_SetRegsMapTab(
                          RSA_HW_PKI_PKA_RegsMap_t  *RegsMap_ptr,     	  /*in*/   
                          DxUint32_t             *CountOfRegs,        /*in*/
                          DxUint32_t              MaxSizeWords,       /*in*/ 
                          DxUint32_t              N_NP_T0_T1,         /*in*/
                          DxUint32_t              IsDefaultMap,       /*in*/
                          DxUint32_t              VirtualHwBaseAddr   /*in*/ );
                          


/***********      RSA_HW_PKI_PKA_InitPKA function      **********************/
/**
 * @brief This function initializes the PKA engine. 
 *
 *    The function performs the following:
 *      - enables PKA clocks, 
 *      - initializes the PKA_SizesTable, PKA_MappingTable and special register 
 *        N_NP_T0_T1 according to user passed register sizes, registers mapping 
 *        and initialization mode.
 *      - if IsDefaultMap = 1, then the function sets all parameters according to default 
 *        mode, else they should be set from the user passed parameters. 
 *      - cleans all PKA memory registers which are defined above (according to their
 *        size and count).
 *        
 *    The function calls the RSA_HW_PKI_PKA_SetRegsSizesTab  and RSA_HW_PKI_PKA_SetRegsMapTab 
 *    functions and sets N_NP_T0_T1 value into N_NP_T0_T1 register.
 *  
 *
 *    Notes: 
 *            - See remarks to RSA_HW_PKI_PKA_SetRegsSizesTab and RSA_HW_PKI_PKA_SetRegsMapTab functions.
 *            - The function allocates one additional word for each register. 
 *            - For user convenience defined the macros for calling this functions on default mode.
 *
 *
 * @param[in] RegsSizes_ptr  - A  pointer to array, containing PKA data registers sizes,  
 *                             which must be set.  
 * @param[in] CountOfSizes   - Count of sizes to set: must be minimum 1, maximum 8.
 * @param[in] RegsMap_ptr    - A pointer to structure, containing the virtual and physical 
 *                             addresses of needed PKA registers.  
 * @param[in] CountOfRegs    - A count of all registeres to set (depending on memory size): 
 *                             minimum 1, maximum 32 (including 4 special registers N,NP,T0,T1).
 * @param[in] OpSizeBits     - Operation (modulus) exact size in bits. The value must 
 *                             be in interval from 128 to 2112 bits. If modulus is not used,
 *                             this is the maximal size of operands in bits.
 * @param[in] RegSizeWords   - Extended registers size in words. The value must be in range 
 *                             min-max defined size + 1 word (5 - 67 words).
 * @param[in] N_NP_T0_T1     - Virtual pointers (sequence numbers) of special registers (pointers
 *                             of 5 bits size each one, concatenated together and placed into 
 *                             4-bytes word).   
 * @param[in] IsDefaultMap   - Defines whether the default settings are used or not:
 *                             1 - default, 0 - user passed settings.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 */
 void RSA_HW_PKI_PKA_InitPKA(
                 RSA_HW_PKI_PKA_RegsSizes_t  RegsSizes_ptr,      /*in*/   
                 DxUint32_t               CountOfSizes,       /*in*/
                 RSA_HW_PKI_PKA_RegsMap_t   *RegsMap_ptr,        /*in*/   
                 DxUint32_t               CountOfRegs,        /*in*/
                 DxUint32_t               OpSizeBits,         /*in*/
                 DxUint32_t               RegSizeWords,       /*in*/
                 DxUint32_t               N_NP_T0_T1,         /*in*/
                 DxUint32_t               IsDefaultMap,       /*in*/
                 DxUint32_t               VirtualHwBaseAddr   /*in*/ );  
 

/**  Macros for calling the RSA_HW_PKI_PKA_InitPKA function on default mode  **/
/**
 * @brief This macros calls the RSA_HW_PKI_PKA_InitPKA function with default parametrs.
 * 
 *        By default: RegsSizes_ptr = NULL, RegsMap_ptr = NULL, 
 *                    N_NP_T0_T1 = 0 (ignored), IsDefaultMap = 1.
 *
 */    
 #define RSA_HW_PKI_PKA_DefaultInitPKA( MaxSizeBits, RegSizeWords, VirtualHwBaseAddr ) \
 RSA_HW_PKI_PKA_InitPKA( DX_NULL, 0, DX_NULL, 0, (MaxSizeBits), (RegSizeWords), 0, 1, (VirtualHwBaseAddr) )
 
   						    

/***********      RSA_HW_PKI_PKA_FinishPKA function      **********************/
/**
 * @brief This function ends the PKA engine session and disables PKA clocks. 
 *
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_FinishPKA( DxUint32_t VirtualHwBaseAddr /*in*/ );
 
 
 			    
/*******************************************************************************/
/************    RSA PKI PKA mathmatic functions and macros     ****************/
/*******************************************************************************/
                               
/***********    _RSA_HW_PKI_PKA_ExecOperation (witrh virtual pointers)     ******************/
/**
 * @brief This function executes any allowed PKA mathmatic operation according to 
 *        user passed Opcode.
 *
 *        The function receives code of operation, virtual pointers to PKI registers  
 *        (sequence number), for arguments and result, and operates PKA machine by writing  
 *        full operation code into OPCODE register. Then the function calls macros for  
 *        waiting the PKA pipe ready signal.
 *
 *        The user don't call this function directly. For user convenience, in RSA_PKI.h file  are 
 *        given some macros for calling this function according to each performed operation.  
 *
 *     NOTES:  
 *       -	Before executing modular operations, the modulus must be set into N=r0 register of PKA.
 *       -	Before modular multiplication and exponentiation must be calculated and set into NP=r1 
 *          register the Barrett modulus tag NP = 2**(sizeN+132) / N.   
 *       -	In operations with immediate operands (IsImmediate bit = 1), the operand value (5-bit)
 *          is treated as sign-extended. That means: low 4 bits are treated as unsigned operand 
 *          value in range 0-15 and bit 5 is a sign (with extension to all high bits of register, 
 *          in which the full operand shall be set).
 *       -	In shift operations the 5-bits shift operand is treated as unsigned value in range 0-31
 *          (count of shifts is equaled to shift operand value + 1).
 *       -  The LMul operation gives the low half of multiplication result of length equaled to 
 *          operation size. The leading not significant bits of the operands and result (including 
 *          the the extra word) must be zeroed.  
 *       -  The HMul operation gives the high half of multiplication result plus one high word of low 
 *          half of full multiplication result. Therefore this result is by one word large, than 
 *          operation size. The leading not significant bits of the operands and result, 
 *          including extra word must be zeroed. 
 *       -  The ModInv operation calculates Res = 1/OpB mod N for odd modulus. Operand A is ignored.
 *
 *
 * @param[in] Opcode   - The operation code according HW PKA definitions. Valid values: 0 - max Opcode.
 * @param[in] LenID    - ID of the length of operands according to register sizes table 
 *                       (means the number of entry in the table). Valid values: 0...7.
 * @param[in] IsAImmed - If IsAImmed = 1, then operand A treated as immediate value, else - 
 *                       as virtual register pointer. Valid values:	0,1.
 * @param[in] OpA      - Operand A: an immediate value or virtual register pointer, according to IsAImmed 
 *                       IsAImmed parameter. Valid values: 0 <= OpA <= 31. 
 * @param[in] IsBImmed - If IsBImmed = 1, then operand B treated as immediate value, else - 
 *                       as virtual register pointer. Valid values:	0,1.
 * @param[in] OpB      - Operand B: an immediate value or virtual register pointer, according to IsAImmed 
 *                       IsBImmed parameter. Valid values: 0 <= OpA <= 31.
 * @param[in] ResDiscard -	If ResDiscard = 1, then result is discarded.
 * @param[in] Res        - Virtual register pointer for result data. 
 *                         Valid values: 0 <= Res <= 31. Value Res = RES_DISCARD means result must be discarded.
 * @param[in] Tag        - The user defined value (Tag <= 31), used for indication goals. 
 *   
 * @param[in] VirtualHwBaseAddr - The user passed virtual base address of HW. 
 * 
 *
 */
 void _RSA_HW_PKI_PKA_ExecOperation(
                                DxUint32_t      Opcode,            /*in*/ 
                                DxUint8_t       LenID,             /*in*/ 
                                DxUint8_t       IsAImmed,          /*in*/
                                DxInt8_t        OpA,               /*in*/
                                DxUint8_t       IsBImmed,          /*in*/
                                DxInt8_t        OpB,               /*in*/ 
                                DxUint8_t       ResDiscard,        /*in*/
                                DxInt8_t        Res,               /*in*/
                                DxUint8_t       Tag,               /*in*/ 
                                DxUint32_t      VirtualHwBaseAddr  /*in*/ );

 
 
/*************************************************************************/ 
/* Macros for calling PKA operations (names according to operation issue */
/*************************************************************************/

/*  Add:   Res =  OpA + OpB  */
#define   RSA_HW_PKI_PKA_Add(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_HW_PKI_PKA_ExecOperation( PKA_Add,(LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )


/*----------------------------------*/
/*   2.  Logical   operations       */
/*----------------------------------*/
/*  Clr:  Res =  OpA & 0  - clears the operand A.  */
#define   RSA_HW_PKI_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            _RSA_HW_PKI_PKA_ExecOperation( PKA_AND, (LenID), 0, (OpA), 1, 0x00, 0, (OpA), (Tag), (VirtualHwBaseAddr) )

/*  Clear:  for full clearing the actual register opA, this macro calls Clr operation twice.  */
#define   RSA_HW_PKI_PKA_Clear(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            RSA_HW_PKI_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr);  \
            RSA_HW_PKI_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr)
            
/*  Set0:  Res =  OpA || 1  : set bit0 = 1, other bits are not changed */
#define   RSA_HW_PKI_PKA_Set0(LenID, OpA, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_HW_PKI_PKA_ExecOperation( PKA_OR, (LenID), 0, (OpA), 1, 0x01, 0, (Res), (Tag), (VirtualHwBaseAddr) )
            

/*-----------------------------------------------------*/
/*   2.  Multiplication and other   operations         */
/*       Note:  See notes to RSA_HW_PKI_PKA_ExecOperation */
/*-----------------------------------------------------*/

/*  ModExp:  Res =  OpA ** OpB  mod N - modular exponentiation */
#define   RSA_HW_PKI_PKA_ModExp(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _RSA_HW_PKI_PKA_ExecOperation( PKA_ModExp, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )

/*  Copy:  OpDest =  OpSrc || 0  */
#define   RSA_HW_PKI_PKA_Copy(LenID, OpDest, OpSrc, Tag, VirtualHwBaseAddr)   \
            _RSA_HW_PKI_PKA_ExecOperation( PKA_OR, (LenID), 0, (OpSrc), 1, 0x00, 0, (OpDest), (Tag), (VirtualHwBaseAddr) )

/*  SHL0:  Res =  OpA << (S+1) :   shifts left operand A by S+1 bits, insert 0 to right most bits */
#define   RSA_HW_PKI_PKA_SHL0(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_HW_PKI_PKA_ExecOperation( PKA_SHL0, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr) )

/*  Terminate  - special operation, which allows HOST access 
    to PKA data memory registers after end of PKA operations */
#define   RSA_HW_PKI_PKA_Terminate(Tag, VirtualHwBaseAddr)   \
            _RSA_HW_PKI_PKA_ExecOperation( PKA_Terminate, 0, 0, 0, 0, 0, 0, 0, (Tag), (VirtualHwBaseAddr) )

/*  Divide:  Res =  OpA / OpB , OpA = OpA mod OpB - division,  */
#define   RSA_HW_PKI_PKA_Div(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_HW_PKI_PKA_ExecOperation( PKA_Div, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
			                    

				    

/* ***************************************************************************** */
/* ************   Auxiliary functions used in PKA               **************** */
/* ***************************************************************************** */

 
/***********      RSA_HW_PKI_PKA_ClearBlockOfRegs function      **********************/
/**
 * @brief This function clears block of PKA registers. 
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of 
 *                     sizes table.
 *
 * @param[in] firstReg    - Virtual address (number) of first register in block.
 * @param[in] countOfRegs - Count of registers to clear.
 * @param[in] LenId       - ID of entry of regsSizesTable defines registers lengts with word extension.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 */
 void RSA_HW_PKI_PKA_ClearBlockOfRegs( 
                                DxUint8_t  firstReg,          /*in*/
                                DxUint8_t  countOfRegs,       /*in*/
                                DxUint8_t  LenID,             /*in*/
                                DxUint32_t VirtualHwBaseAddr  /*in*/ ); 
                                
                                
 
/***********      RSA_HW_PKI_PKA_CopyDataIntoPkaReg function      **********************/
/**
 * @brief This function  copies source data into PKA register . 
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of 
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must 
 *                     set LenID according to extended register size
 *
 * @param[in] dstReg       - Virtual address (number) of destination register.
 * @param[in] LenId        - ID of entry of regsSizesTable defines registers lengts with word extension.
 * @param[in] src_ptr      - Pointer to source buffer.
 * @param[in] sizeBytes    - Data size in bytes.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 
                                DxInt8_t    dstReg,            /*out*/
                                DxUint8_t   LenID,             /*in*/
                                DxUint32_t  *src_ptr,           /*in*/
                                DxUint32_t  sizeBytes,         /*in*/
                                DxUint32_t  VirtualHwBaseAddr  /*in*/ ); 
                                
                                
                          
                                
/***********      RSA_HW_PKI_PKA_CopyDataFromPkaReg      **********************/
/**
 * @brief This function copies data from PKA register into output buffer . 
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of 
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must 
 *                     set LenID according to extended register size
 *
 * @param[in] srcReg       - Virtual address (number) of source PKA register.
 * @param[in] dst_ptr      - Pointer to destination buffer.
 * @param[in] sizeBytes - Source size in bytes.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_CopyDataFromPkaReg( 
                                DxUint32_t  *dst_ptr,           /*out*/
                                DxUint32_t  sizeBytes,      /*in*/
                                DxInt8_t    srcReg,            /*in*/
                                DxUint32_t  VirtualHwBaseAddr  /*in*/ ); 
                                                    

 /***********      RSA_HW_PKI_SET_MSB_ADDR      **********************/
/**
 * @brief This function sets the PKI MSB register according ot the SRAM address . 
 *
 *        Assumings: - PKA registers are accessable.
 *                   - SRAM address is multiple of 4KB.
 *
 * @param[in] pkaAdrress       - base address of the SRAM for PKA.
 *
 * @return - no return parameters.
 *
 */
void RSA_HW_PKI_SET_MSB_ADDR(DxUint32_t virtualAddress, DxUint32_t pkaAdrress);

#ifdef __cplusplus
}
#endif

#endif
