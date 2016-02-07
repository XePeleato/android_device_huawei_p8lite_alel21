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

 
 
#ifndef LLF_PKI_2_H
#define LLF_PKI_2_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "crys_rsa_types.h"
#include "llf_pki_error.h"
#include "llf_pki_hw_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : % llf_pki.h
   *  State           :  %state%
   *  Creation date   :  02 Jan. 2008
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version llf_pki.h#1:incl:3
   *  \author R.Levin
   */

/************************ Defines ******************************/
/* maximal allowed size of RSA key for this LLF version */
#ifndef CRYS_NO_RSA_MAX_KEY_SIZE_3072_BIT_SUPPORT  
	#define LLF_PKI_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS  3072
#else
	#define LLF_PKI_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS  2048
#endif

/* maximal size of key, which may be generated on current engine */
#define LLF_PKI_PKA_MAX_KEY_GENERATION_SIZE_BITS      2048


/* the primes P1,P2,Q1,Q2 size for RSA Key generation according to ANSI X9.31 */
#define LLF_PKI_KG_X931_PRIME1_SIZE_IN_BITS  101 
#define LLF_PKI_KG_X931_PRIME1_SIZE_IN_8BIT_WORDS  ((101/8) + 1)
#define LLF_PKI_KG_X931_PRIME1_SIZE_IN_32BIT_WORDS  ((101/32) + 1)

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
#define LLF_PKI_QUICK_PRIME_TEST_DIVISIONS_COUNT  128
                    
/***********************  Macros *******************************/
#define LLF_PKI_SwapInt8(x,y)  \
{ \
   DxInt8_t  temp; \
   temp = (x); x = (y); y = temp; \
}


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
		
} LLF_PKI_PKA_OpID_t;


/*  Enumerator, defining whether the functions performs default settings 
    of PKI registers, or takes they from user passed arguments  */
typedef enum
{
      DefaultRegs       = 0,
      SetRegs           = 1,
      RegsSetOffMode
      
} LLF_PKI_PKA_SetRegs_t;


/************************ Typedefs  ****************************/

/* definition of the Registers sizes array type, used for initialisation 
   of the PKA sizes table */
typedef DxUint32_t LLF_PKI_PKA_RegsSizes_t[LLF_PKI_PKA_MAX_COUNT_OF_REGS_SIZES];               
                    
/* type definition of the list of temp registers, containing numbers, which 
   may be used as temp registes registers  */
typedef DxUint8_t LLF_PKI_PKATempRegs_t[LLF_PKI_PKA_MAX_COUNT_OF_REGS_SIZES];

/* Barrett modulus tag type - 5 words size array */
typedef DxUint32_t LLF_PKI_PKA_NP_t[LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];


/************************ Structs  ******************************/

 /* This structure contains virtual pointers (numbers in range 0-31) of 
    PKI registers RegsNum[]  and registers physical addresses RegsAddr[]. 
    For example: RegsNum[3] = 5 and RegsAddr[3] = 0x400 means, that address of 
    register 5 is 0x400. The user may set into this structure only needed registers, 
    used in planned PKA operations. */

typedef  struct  
{
       DxUint32_t    RegsNum[LLF_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS] ;
       DxUint32_t    RegsAddr[LLF_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS] ;
}LLF_PKI_PKA_RegsMap_t;

/************************ Public Variables **********************/

/************************ Public Functions ******************************/


/***********************************************************************************************/
/***********************************************************************************************/
/*                                                                                             */ 
/*                   LLF PKI FUNCTIONS, PERFORMING PKA OPERATIONS                              */
/*                                                                                             */
/***********************************************************************************************/
/***********************************************************************************************/



/*******************************************************************************/
/**********      LLF PKI PKA initialisation functions and macros      **********/
/*******************************************************************************/

/***********      LLF_PKI_PKA_SetRegsSizesTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table. 
 *
 *   The function has two modes of working: default and user defined.
 *   1. User defined mod (IsDefaultMap = 0):
 *      The function receives a count of entries in the registers sizes table (minimum 1, maximum 8), 
 *      which must be set, and pointer to sizes array (sizes in bits). Then the function writes the 
 *      needed sizes into the table, placed in the HW defined address, and sets remaining entries 
 *      of the table equaled to PKA_SIZE_ENTRY_NOT_USED (for preventing wrong using not initialized).
 *      In case of error in received values the function returns an internal error, defined in 
 *      the llf_pki_error.h file. Error cases: 
 *        -  RegsSizes_ptr = NULL;
 *        -  1 > CountOfSizes >  8;
 *        -  minimal size in the array is small than 128-32 bit ;
 *        -  maximal size in the array is grate, than 2112+32 bit.
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
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       LLF_PKI_PKA_ILLEGAL_PTR_ERROR	
 *                       LLF_PKI_PKA_ENTRIES_COUNT_ERROR
 *                       LLF_PKI_PKA_SET_MAP_MODE_ERROR 
 *                       CRYS_PKI_PKA_REGISTER_SIZES_ERROR	
 *
 */
CRYSError_t LLF_PKI_PKA_SetRegsSizesTab(
                             LLF_PKI_PKA_RegsSizes_t   RegsSizes_ptr,    /*in*/   
                             DxUint32_t                CountOfSizes,     /*in*/
                             DxUint32_t                MaxSizeBits,      /*in*/
                             DxUint32_t                IsDefaultMap,     /*in*/ 
                             DxUint32_t                VirtualHwBaseAddr /*in*/); 
                             
                             
/******    MACROS for calling the  LLF_PKI_PKA_SetRegsSizesTab function  *******/
/**
 * @brief This Macros is intended for initialization of the PKA registers sizes table
 *        on default mode. 
 *
 *        The macros calls the LLF_PKI_PKA_SetRegsSizesTab function with some parameters 
 *        equaled to 0. 
 *
 * @param[in] MaxSizeBits    -  Maximal size of operands in bits. The value must be in interval 
 *                              from 128 to 2112 bits. This argument is used on default mode only.  
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 */                         
 #define LLF_PKI_PKA_DefaultSetRegsSizesTab( MaxSizeBits, VirtualHwBaseAddr ) \
 LLF_PKI_PKA_SetRegsSizesTab( DX_NULL, 0, (MaxSizeBits), 1, (VirtualHwBaseAddr) );
 
                                                            
 
/***********      LLF_PKI_PKA_SetRegsMapTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table. 
 *
 *   The function checks input parameters and sets the physical memory registers mapping-table 
 *   according to IsDefaultMap parameter, passed by user: 
 *     - if IsDefaultMap = 1, then the function sets default mapping. 
 *     - if IsDefaultMap = 0, then the mapping addresses should be taken from the mapping structure, 
 *       transferred by the pointer RegsMap_ptr and by count of registers to be set.
 *     - on default mode the function returns the allowed counnt of registers.
 *
 *    1. Default mapping is following:
 *     - start address of register 0 is the start address of PKA data registers memory 
 *       (defined in HwDefs file); 
 *     - special registers are set as follows: N=0,NP=1,T0=30,T1=31, therefore the N_NP_T0_T1 
 *       parameter on default mode is ignored;
 *     - all registers have the same size, equalled to maximal allowed size of registers + 32 bits;
 *     - count of registers (including 4 special registers) is calculated according to maximal 
 *       size of registers and total size of data registers memory;
 *     - for calling the function on default mode user may use the macros, defined in llf_pki.h file.  
 *     
 *    2. The mapping, defined by user through the RegsMap_ptr structure:
 *     - the user sets count of needed registers (including special registers N,NP,T0,T1);  
 *     - the user sets virtual pointers (numbers in range 0-31) of all needed registers and their 
 *       physical addresses (offsets from virtual base address) into RegsMap_ptr structure;
 *     - the user sets numbers of special registers N,NP,T0,T1 (5 bits each one), concatenated 
 *       together, into input parameter N_NP_T0_T1;
 *     - the function checks, that user passed mapping is meet to the PKA data memory size, if it  
 *       is not - the function returns an error;    
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
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       LLF_PKI_PKA_ILLEGAL_PTR_ERROR	
 *                       LLF_PKI_PKA_ENTRIES_COUNT_ ERROR
 *                       LLF_PKI_PKA_REGISTER_SIZES_ERROR
 *                       LLF_PKI_PKA_SET_MAP_MODE_ERROR
 *                       LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR	
 *
 */
CRYSError_t LLF_PKI_PKA_SetRegsMapTab(
                          LLF_PKI_PKA_RegsMap_t  *RegsMap_ptr,     	  /*in*/   
                          DxUint32_t             *CountOfRegs,        /*in*/
                          DxUint32_t              MaxSizeWords,       /*in*/ 
                          DxUint32_t              N_NP_T0_T1,         /*in*/
                          DxUint32_t              IsDefaultMap,       /*in*/
                          DxUint32_t              VirtualHwBaseAddr   /*in*/ );
                          

/******    MACRO for calling the  LLF_PKI_PKA_SetRegsMapTab function  *******/
/**
 * @brief This macro is intended for initialization of the PKA registers sizes table
 *        on default mode. 
 *
 *        The macro calls the LLF_PKI_PKA_SetRegsSizesTab function with some parameters 
 *        equalled to 0 according to default mode of called function (see above). 
 *
 * @param[in] MaxSizeWords - Maximal size of operands in words. The value must be in interval 
 *                           from 4 to 66 words. Note: the function allocates one additional word 
 *                           for each register, so that their sizes = MaxSizeWords + 1. 
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 */                         
 #define LLF_PKI_PKA_DefaultSetRegsMapTab( MaxSizeWords, CountOfRegs, VirtualHwBaseAddr ) \
 LLF_PKI_PKA_SetRegsMapTab( DX_NULL, (CountOfRegs), (MaxSizeWords), 0, 1, (VirtualHwBaseAddr) );
                             


/***********      LLF_PKI_PKA_InitPKA function      **********************/
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
 *    The function calls the LLF_PKI_PKA_SetRegsSizesTab  and LLF_PKI_PKA_SetRegsMapTab 
 *    functions and sets N_NP_T0_T1 value into N_NP_T0_T1 register.
 *  
 *
 *    Notes: 
 *            - See remarks to LLF_PKI_PKA_SetRegsSizesTab and LLF_PKI_PKA_SetRegsMapTab functions.
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
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       LLF_PKI_PKA_ILLEGAL_PTR_ERROR	
 *                       LLF_PKI_PKA_ENTRIES_COUNT_ERROR
 *                       LLF_PKI_PKA_REGISTER_SIZES_ERROR	
 *                       LLF_PKI_PKA_SET_MAP_MODE_ERROR	
 *                       LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR
 *
 */
 CRYSError_t LLF_PKI_PKA_InitPKA(
                 LLF_PKI_PKA_RegsSizes_t  RegsSizes_ptr,      /*in*/   
                 DxUint32_t               CountOfSizes,       /*in*/
                 LLF_PKI_PKA_RegsMap_t   *RegsMap_ptr,        /*in*/   
                 DxUint32_t               CountOfRegs,        /*in*/
                 DxUint32_t               OpSizeBits,         /*in*/
                 DxUint32_t               RegSizeWords,       /*in*/
                 DxUint32_t               N_NP_T0_T1,         /*in*/
                 DxUint32_t               IsDefaultMap,       /*in*/
                 DxUint32_t               VirtualHwBaseAddr   /*in*/ );  
 

/**  Macros for calling the LLF_PKI_PKA_InitPKA function on default mode  **/
/**
 * @brief This macros calls the LLF_PKI_PKA_InitPKA function with default parametrs.
 * 
 *        By default: RegsSizes_ptr = NULL, RegsMap_ptr = NULL, 
 *                    N_NP_T0_T1 = 0 (ignored), IsDefaultMap = 1.
 *
 */    
 #define LLF_PKI_PKA_DefaultInitPKA( MaxSizeBits, RegSizeWords, VirtualHwBaseAddr ) \
 LLF_PKI_PKA_InitPKA( DX_NULL, 0, DX_NULL, 0, (MaxSizeBits), (RegSizeWords), 0, 1, (VirtualHwBaseAddr) )
 

/***********      LLF_PKI_PKA_InitPkaEx function      **********************/
/**
 * @brief This function initializes the PKA engine with exact sizes. 
 *
 *    The function performs the following:
 *      - enables PKA clocks, 
 *      - cleans all PKA memory registers.
 *      - initializes the PKA_SizesTable, PKA_MappingTable according to default 
 *        registers mapping. 
 *        
 *    Note: - The function allocates PKA registers of size equal to operation size + one word. 
 *
 * @param[in] opSizeBits     - Operation (modulus) exact size in bits. 
 * @param[in] virtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - CRYS_OK 
 *
 */
 CRYSError_t LLF_PKI_PKA_InitPkaEx(
			DxUint32_t  opSizeBits,         /*in*/
			DxUint32_t  virtualHwBaseAddr   /*in*/ );
   						    

/***********      LLF_PKI_PKA_FinishPKA function      **********************/
/**
 * @brief This function ends the PKA engine session and disables PKA clocks. 
 *
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void LLF_PKI_PKA_FinishPKA( DxUint32_t VirtualHwBaseAddr /*in*/ );
 
 
 
 
 						    
/*******************************************************************************/
/************    LLF PKI PKA mathmatic functions and macros     ****************/
/*******************************************************************************/


/*******   LLF_PKI_PKA_ExecCalcNpAndInitModOp function (virtual pointers)   **********/
/**
 * @brief The function calculates the Barret modulus tag NP, used in   
 *        modular operations, and sets the result into PKA NP-register . 
 *
 *        The modulus tag is calculated as: NP = A / N, 
 *             where: A = 2^(sizeN + 132 ) -according to HW requierements,
 *                    N - modulus, sizeN - size of N in bits,
 *                    ^ - symbol of exponentiation.
 *        
 *        Algorithm of calculation (according to registers sizes limitation):
 *
 *        1. Set A := r2 := 2^(sizeN + 32);
 *        2. Set S := 132-32 = 100;
 *        3. Call LLF_PKI_PKA_DivLongNum function for performing NP = (A * 2^S) / N;
 *        4. Exit.
 *        
 *        Assuming: - the registers sizes table and mapping table are set on default mode,
 *                    according to modulus size; that means: 
 *                      -- the function uses 7 PKA data registers : rT0,rT1,rT2,0,1,30,31    
 *                         (3 temp registers and 4 special registers N,NP,T0,T1);
 *                      -- registers sizes table entries are set by default as follows:
 *                           0 - N_sizeBits, 1 - (32*N_sizeWords + 32 bit);  
 *                  - the modulus N is set in the N-register; 
 *                  - the PKA clocks are initialized.
 *
 * @param[in] LenID - The RegsSizesTable entry, containing the modSizeBits.
 * @param[in] modSizeBits  - exact modulus size in bits.
 * @param[in] rT0, rT1, rT2  - virtual pointers to temp registers.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return  - On success CRYS_OK is returned, on failure an error code.
 *
 */
CRYSError_t LLF_PKI_PKA_ExecCalcNpAndInitModOp( 
                                         DxUint32_t  LenID,
                                         DxUint32_t  modSizeBits, 
                                         DxInt8_t    rT0, 
                                         DxInt8_t    rT1, 
                                         DxInt8_t    rT2, 
                                         DxUint32_t  VirtualHwBaseAddr );


/***********   LLF_PKI_PKA_DivLongNum function      **********************/
/**
 * @brief The function divides long number A*(2^S) by B: 
 *            Res =  A*(2^S) / B,  remainder A = A*(2^S) % B.
 *        where: A,B - are numbers of size, which is not grate than, maximal operands size,
 *               and B > 2^S;
 *               S  - exponent of binary factor of A.
 *               ^  - exponentiation operator. 
 *  
 *        The function algorithm:
 *
 *        1. Let nWords = S/32; nBits = S % 32; 
 *        2. Set Res = 0, rT1 = OpA;
 *        3. for(i=0; i<=nWords; i++) do:
 *            3.1. if(i < nWords ) 
 *                   s1 = 32;  
 *                 else
 *                   s1 = nBits;
 *            3.2. rT1 = rT1 << s1;  
 *            3.3. call PKA_div for calculating the quotient and remainder: 
 *                      rT2 = floor(rT1/opB) //quotient; 
 *                      rT1 = rT1 % opB      //remainder (is in rT1 register);
 *            3.4. Res = (Res << s1) + rT2;  
 *           end do;
 *        4. Exit.
 *        
 *        Assuming: 
 *                  - 5 PKA rgisters are used: OpA, OpB, Res, rT1, rT2.
 *                  - The registers sizes and mapping tables are set on default mode 
 *                    according to operands size. 
 *                  - The PKA clocks are initialized.
 *        NOTE !   Operand OpA shall be overwritten by remainder. 
 *
 * @param[in] LenID    - ID of operation size (modSize+32). 
 * @param[in] OpA      - Operand A: virtual register pointer of A . 
 * @param[in] S        - exponent of binary factor of A.
 * @param[in] OpB      - Operand B: virtual register pointer of B . 
 * @param[in] Res      - Virtual register pointer for result quotient. 
 * @param[in] rT1      - Virtual pointer to remainder.
 * @param[in] rT2      - Virtual pointer of temp register.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       
 *
 */
CRYSError_t LLF_PKI_PKA_DivLongNum( 
		                        DxUint8_t      LenID,
		                        DxInt8_t       OpA,
		                        DxUint32_t     S,
		                        DxInt8_t       OpB,
		                        DxInt8_t       Res, /* div result - out */
		                        DxInt8_t       rT1, /* remainder -  out */
		                        DxInt8_t       rT2,   
		                        DxUint32_t     VirtualHwBaseAddr );
                        
                              
/******    LLF_PKI_CalcNpAndInitModOp function (physical pointers)     ***************/
/**
 * @brief The function initializes  modulus and Barret tag NP, used in modular PKA operations. 
 *
 *        The function does the following:
 *          - calculates mod size in bits and sets it into PKA table sizes;
 *          - if parameter NpCreateFlag = PKA_CreateNP, then the function writes the modulus and the 
 *            tag into registers r0 and r1 accordingly;
 *          - if NpCreateFlag= PKA_SetNP, the function calls the LLF_PKI_PKA_ExecCalcNpAndInitModOp,
 *            which calculates the Barret tag NP and initializes PKA registers; then the
 *            function outputs calculated NP value.
 *
 *       Assumings: - The registers mapping table is set on default mode, according to modulus size: 
 *                      -- count of allowed registers is not less, than 7 (including 3 registers 
 *                         rT0,rT2,rT3 for internal calculations and 4 default special registers N,NP,T0,T1);
 *                      -- modulus exact and exact+32 bit sizes should be set into first two entries
 *                         of sizes-table accordingly. 
 *
 * @param[in]  N_ptr  	    - The pointer to the buffer, containing modulus N,	
 * @param[in]  N_sizeBits   - The size of modulus in bytes, must be 16 <= N_sizeBytes <= 264.
 * @param[out] NP_ptr       - The pointer to the buffer, containing result - modulus tag NP.
 * @param[in]  NpCreateFlag - Parameter, defining whether the NP shall be taken from NP buffer and set into 
 *                            PKA register NP ( NpCreateFlag= PKA_CreateNP= 1 ) or it shall be calculated and send to 
 *                            NP buffer ( NpCreateFlag= PKA_SetNP= 0 ).
 * @param[in]  rT0,rT1,rT2  - Virtual pointers to temp registers (sequence numbers).
 * @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       LLF_PKI_PKA_ILLEGAL_PTR_ERROR
 *                       LLF_PKI_PKA_ILLEGAL_OPERAND_LEN_ERROR	
 *
 */
CRYSError_t LLF_PKI_CalcNpAndInitModOp(
                          DxUint32_t        *N_ptr,	            /*in*/ 
                          DxUint32_t         N_sizeBits,        /*in*/ 
                          LLF_PKI_PKA_NP_t   NP_ptr,            /*in/out*/
                          DxUint8_t          NpCreateFlag,      /*in*/
                          DxInt8_t           rT0,               /*in*/
                          DxInt8_t           rT1,               /*in*/
                          DxInt8_t           rT2,               /*in*/
                          DxUint32_t         VirtualHwBaseAddr  /*in*/ );


/******         LLF_PKI_DefaultInitModOp macros      ***************/
/**
 * @brief This macros initializes  modulus and Barret tag NP, useing calculated values. 
 *
 *        The macros calls the LLF_PKI_CalcNpAndInitModOp function with some not used parameters
 *        equalled to NULL and isNpYetReady = 1. 
 *        Assuming: - The registers mapping table is set on default mode, according to modulus size; 
 *                      -- registers used: N=0, NP=1;
 *                      -- modulus exact and exact+32 bit sizes should be set into first two entries
 *                         of sizes-table accordingly. 
 *                  - The PKA clocks are initialized.
 *
 * @param[in]  N_ptr  	    - The pointer to the buffer, containing modulus N,	
 * @param[in]  N_sizeBits   - The size of modulus in bytes, must be 16 <= N_sizeBytes <= 264.
 * @param[out] NP_ptr       - The pointer to the buffer, containing result - modulus tag NP.
 * @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       LLF_PKI_PKA_ILLEGAL_PTR_ERROR
 *                       LLF_PKI_PKA_ILLEGAL_OPERAND_LEN_ERROR	
 *
 */
 #define LLF_PKI_DefaultInitModOp( N_ptr, N_sizeBits, NP_ptr, VirtualHwBaseAddr ) \
         LLF_PKI_CalcNpAndInitModOp( (N_ptr), (N_sizeBits),	(NP_ptr), 1, 0,0,0, (VirtualHwBaseAddr) )

                               
/***********    _LLF_PKI_PKA_ExecOperation (witrh virtual pointers)     ******************/
/**
 * @brief This function executes any allowed PKA mathmatic operation according to 
 *        user passed Opcode.
 *
 *        The function receives code of operation, virtual pointers to PKI registers  
 *        (sequence number), for arguments and result, and operates PKA machine by writing  
 *        full operation code into OPCODE register. Then the function calls macros for  
 *        waiting the PKA pipe ready signal.
 *        If opcode is illegal or one of operands is illegal, the function returns an 
 *        error code defined in llf_pki_error.h file.
 *
 *        The user don't call this function directly. For user convenience, in llf_pki.h file  are 
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
 *          In case of even modulus the function returns an error. Therefore in this case 
 *          (also for odd modulus) the user may call the LLF_PKI_PKA_ExecFullModInv function.  
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
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       LLF_PKI_PKA_ILLEGAL_OPCODE_ERROR
 *                       LLF_PKI_PKA_ILLEGAL_OPERAND_LEN_ERROR
 *                       LLF_PKI_PKA_ILLEGAL_OPERAND_TYPE_ERROR
 *                       LLF_PKI_PKA_ILLEGAL_OPERAND_ERROR
 *                       LLF_PKI_PKA_INVERSION_NOT_EXISTS_ERROR
 */
 CRYSError_t _LLF_PKI_PKA_ExecOperation(
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
 
/*----------------------------------*/
/*   1.  ADD - SUBTRACT operations  */
/*----------------------------------*/
/*  Add:   Res =  OpA + OpB  */
#define   LLF_PKI_PKA_Add(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_Add,(LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  AddIm:  Res =  OpA + OpBIm  */
#define   LLF_PKI_PKA_AddIm(LenID, OpA, OpBIm, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_Add,(LenID), 0, (OpA), 1, (OpBIm), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Sub:  Res =  OpA - OpB  */
#define   LLF_PKI_PKA_Sub(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_Sub,(LenID),0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  SubIm:  Res =  OpA - OpBIm  */
#define   LLF_PKI_PKA_SubIm(LenID, OpA, OpBIm, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_Sub,(LenID), 0, (OpA), 1, (OpBIm), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Neg:  Res =  0 - OpB  */
#define   LLF_PKI_PKA_Neg(LenID, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_Sub, (LenID), 1, 0, 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  ModAdd:  Res =  (OpA + OpB) mod N  */
#define   LLF_PKI_PKA_ModAdd(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_ModAdd, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  ModAddIm:  Res =  (OpA + OpBIm) mod N  */
#define   LLF_PKI_PKA_ModAddIm(LenID, OpA, OpBIm, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_ModAdd, (LenID), 0, (OpA), 1, (OpBIm), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  ModSub:  Res =  (OpA - OpB) mod N  */
#define   LLF_PKI_PKA_ModSub(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _LLF_PKI_PKA_ExecOperation( PKA_ModSub, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  ModSubIm:  Res =  (OpA - OpBIm) mod N  */
#define   LLF_PKI_PKA_ModSubIm(LenID, OpA, OpBIm, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_ModSub, (LenID), 0, (OpA), 1, (OpBIm), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  ModNeg:  Res =  (0 - OpB) mod N  */
#define   LLF_PKI_PKA_ModNeg(LenID, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_ModSub, (LenID), 1, 0, 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )


/*----------------------------------*/
/*   2.  Logical   operations       */
/*----------------------------------*/

/*  AND:  Res =  OpA & OpB  */
#define   LLF_PKI_PKA_AND(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_AND, (LenID), 0, (OpA), 0, (OpB), 0, (Res) , (Tag), (VirtualHwBaseAddr) )
/*  AndIm:  Res =  OpA & OpB  */
#define   LLF_PKI_PKA_AndIm(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_AND, (LenID), 0, (OpA), 1, (OpB), 0, (Res) , (Tag), (VirtualHwBaseAddr) )
/*  Tst0:  OpA & 0x1 -  tests the bit 0 of operand A. If bit0 = 0, then ZeroOfStatus = 1, else 0  */
#define   LLF_PKI_PKA_Tst0(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_AND, (LenID), 0, (OpA), 1, 0x01, 1, RES_DISCARD , (Tag), (VirtualHwBaseAddr)  )
/*  Clr0:  Res =  OpA & (-2)  - clears the bit 0 of operand A.  Note:  -2 = 0x1E  for 5-bit size */
#define   LLF_PKI_PKA_Clr0(LenID, OpA, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_AND, (LenID), 0, (OpA), 1, 0x1E, 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Clr:  Res =  OpA & 0  - clears the operand A.  */
#define   LLF_PKI_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_AND, (LenID), 0, (OpA), 1, 0x00, 0, (OpA), (Tag), (VirtualHwBaseAddr) )
/*  Clear:  for full clearing the actual register opA, this macro calls Clr operation twice.  */
#define   LLF_PKI_PKA_Clear(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            LLF_PKI_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr);  \
            LLF_PKI_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr)
/*  OR:  Res =  OpA || OpB  */
#define   LLF_PKI_PKA_OR(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_OR, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  OrIm:  Res =  OpA || OpB  */
#define   LLF_PKI_PKA_OrIm(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_OR, (LenID), 0, (OpA), 1, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Copy:  OpDest =  OpSrc || 0  */
#define   LLF_PKI_PKA_Copy(LenID, OpDest, OpSrc, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_OR, (LenID), 0, (OpSrc), 1, 0x00, 0, (OpDest), (Tag), (VirtualHwBaseAddr) )
/*  Set0:  Res =  OpA || 1  : set bit0 = 1, other bits are not changed */
#define   LLF_PKI_PKA_Set0(LenID, OpA, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_OR, (LenID), 0, (OpA), 1, 0x01, 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Xor:  Res =  OpA ^ OpB  */
#define   LLF_PKI_PKA_Xor(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_XOR, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  XorIm:  Res =  OpA ^ OpB  */
#define   LLF_PKI_PKA_XorIm(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_XOR, (LenID), 0, (OpA), 1, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Flip0:  Res =  OpA || 1  - inverts the bit 0 of operand A  */
#define   LLF_PKI_PKA_Flip0(LenID, OpA, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_XOR, (LenID), 0, (OpA), 1, 0x01, 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Invert:  Res =  OpA ^ 0xFFF.FF  :  inverts all bits of OpA . 
                    Note: 0xFFFFF =  0x1F for 5 bits size of second operand */
#define   LLF_PKI_PKA_Invert(LenID, OpA, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_XOR, (LenID), 0, (OpA), 1, 0x1F, 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Compare:  OpA ^ OpB . Rsult of compare in ZeroBitOfStatus:  If OpA == OpB then Z = 1 */
#define   LLF_PKI_PKA_Compare(LenID, OpA, OpB, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_XOR, (LenID), 0, (OpA), 0, (OpB), 1, (0), (Tag), (VirtualHwBaseAddr) )
/*  CompareImmediate:  OpA ^ OpB . Rsult of compare in ZeroBitOfStatus:  If OpA == OpB then status Z = 1 */
#define   LLF_PKI_PKA_CompareIm(LenID, OpA, OpB, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_XOR, (LenID), 0, (OpA), 1, (OpB), 1, (0), (Tag), (VirtualHwBaseAddr) )


/*----------------------------------*/
/*   3.  SHIFT    operations        */
/*----------------------------------*/

/*  SHR0:  Res =  OpA >> (S+1) :   shifts right operand A by S+1 bits, insert 0 to left most bits */
#define   LLF_PKI_PKA_SHR0(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_SHR0, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  SHR1:  Res =  OpA >> (S+1) :   shifts right operand A by S+1 bits, insert 1 to left most bits */
#define   LLF_PKI_PKA_SHR1(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_SHR1, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  SHL0:  Res =  OpA << (S+1) :   shifts left operand A by S+1 bits, insert 0 to right most bits */
#define   LLF_PKI_PKA_SHL0(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_SHL0, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  SHL1:  Res =  OpA << (S+1) :   shifts left operand A by S+1 bits, insert 1 to right most bits */
#define   LLF_PKI_PKA_SHL1(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_SHL1, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr) )


/*-----------------------------------------------------*/
/*   2.  Multiplication and other   operations         */
/*       Note:  See notes to LLF_PKI_PKA_ExecOperation */
/*-----------------------------------------------------*/

/*  RMul:  Res =  LowHalfOf(OpA * OpB), where size of operands and result is equaled to operation
           size, defined by LenID. Note: for receiving full result, the LenID must be set according 
           to (sizeA + sizeB) and leading not significant bits of operands must be zeroed */
#define   LLF_PKI_PKA_LMul(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _LLF_PKI_PKA_ExecOperation( PKA_LMul, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  HMul:  Res =  HighHalfOf(OpA * OpB) + one high word of low half of (OpA * OpB), where size of 
           operands is equaled to operation size, defined by LenID. Note: Size of operation result  
           is by one word large, than operation size */
#define   LLF_PKI_PKA_HMul(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _LLF_PKI_PKA_ExecOperation( PKA_HMul, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )             
/*  ModMul:  Res =  OpA * OpB  mod N - modular multiplication */
#define   LLF_PKI_PKA_ModMul(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _LLF_PKI_PKA_ExecOperation( PKA_ModMul, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  ModMulN:  Res =  OpA * OpB  mod N - modular multiplication (final reduction is omitted) */
#define   LLF_PKI_PKA_ModMulN(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _LLF_PKI_PKA_ExecOperation( PKA_ModMulNR, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  ModExp:  Res =  OpA ** OpB  mod N - modular exponentiation */
#define   LLF_PKI_PKA_ModExp(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _LLF_PKI_PKA_ExecOperation( PKA_ModExp, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  Divide:  Res =  OpA / OpB , OpA = OpA mod OpB - division,  */
#define   LLF_PKI_PKA_Div(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_Div, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  ModInv:  Modular inversion: calculates   Res = 1/OpB mod N  */
#define   LLF_PKI_PKA_ModInv(LenID, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_ModInv, (LenID), 0, 1, 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )

/*  Terminate  - special operation, which allows HOST access 
    to PKA data memory registers after end of PKA operations */
#define   LLF_PKI_PKA_Terminate(Tag, VirtualHwBaseAddr)   \
            _LLF_PKI_PKA_ExecOperation( PKA_Terminate, 0, 0, 0, 0, 0, 0, 0, (Tag), (VirtualHwBaseAddr) )

			                    

/********************     LLF_PKI_PKA_ExecFullModInv    ************************/
/**
 * @brief This function calculates modular inversion Res = 1/B mod N for both odd and even modulus.
 *
 *        The function works with virtual pointers to PKA registers (sequence numbers)  
 *        and does the following: 
 * 
 *        1. Checks the parity of modulus N (in register 0) and operand B. If they both are even, 
 *           returns an Error (inverse is not exist) 
 *        2. If the modulus is odd, then calls the LLF_PKI_PKA_ModInv function for calculating 
 *           the inverse.
 *        3. If the modulus is even, then the function performs the following:
 *           3.1  Saves modulus N: rT0<=N; 
 *           3.2. Sets B into reg N: N<=B.  
 *           3.3. Res = N^-1 mod B (call LLF_PKI_PKA_ModInv ); Restore mod: N<=rT0;
 *           3.4. rT0 = high(N*N^-1) = LLF_PKI_PKA_HMul(N,Res,rT0);
 *           3.5. Shift right rT0 >> 32;
 *           3.6. rT1 = low(N*N^-1) = LLF_PKI_PKA_LMul(N,Res,rT1);
 *           3.7. Res = rT0 / B : call LLF_PKI_PKA_LongDiv(rT0,B,Res);
 *           3.7. rT0 = rT1 / B : call LLF_PKI_PKA_Div(rT1,B,rT0);
 *           3.8. Res = Res + rT0 : ModAdd(Res,rT0,Res);
 *           3.9. If reminder of division > 0, then Res= Res+1; 
 *           3.10. Res = N-Res; 
 *        4. Exit.       
 *
 *     NOTE: 
 *       -  The operand B shal be rewritten by GCD(N,B). 
 *       -  The function needs 6 PKA regs: N(0), OpB, Res, rT0, rT1, rT2.
 *       -  PKA sizes table entrys must be set:  0 - exact modSizeBits, 1 - modSizeBits+32 bits, 
 *       -	Before executing modular operations, the modulus must be set into r0 register of PKA.
 *       -  The function not checks the input parameters, because they must be checked previously.
 *
 * @param[in] OpB   - Operand B: virtual register pointer. Valid values: 0 <= OpA <= 31.
 * @param[in] Res   - Virtual register pointer for result data. Valid values: 0 <= Res <= 31.
 * @param[in] LenID    - ID of the length of operands according to register sizes table 
 *                       (means the number of entry in the table). Valid values: 0...7.
 * @param[in] rT0,rT1,rT2,rT3  - The virtual pointers to temp registers (sequence numbers).
 * @param[in] Tag        - The user defined value (Tag <= 31), used for indication goals. 
 * @param[in] VirtualHwBaseAddr - The user passed virtual base address of HW. 
 * 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       LLF_PKI_PKA_ILLEGAL_OPCODE_ERROR
 *                       LLF_PKI_PKA_ILLEGAL_OPERAND_LEN_ERROR
 *                       LLF_PKI_PKA_ILLEGAL_OPERAND_TYPE_ERROR
 *                       LLF_PKI_PKA_ILLEGAL_OPERAND_ERROR
 *                       LLF_PKI_PKA_INVERSION_NOT_EXISTS_ERROR
 *
 */
 CRYSError_t LLF_PKI_PKA_ExecFullModInv(
                                DxInt8_t         OpB,               /*in*/ 
                                DxInt8_t         Res,               /*in*/
                                DxUint32_t       LenID,             /*in*/ 
                                DxInt8_t         rT0,               /*in*/
                                DxInt8_t         rT1,               /*in*/
                                DxInt8_t         rT2,               /*in*/
                                DxInt8_t         rT3,               /*in*/
                                DxUint8_t        Tag,               /*in*/                                 
                                DxUint32_t       VirtualHwBaseAddr  /*in*/ );



/***********************************************************************************************/
/***********************************************************************************************/
/*                                                                                             */ 
/*                FUNCTIONS PERFORMING ALGORITHMS and USED IN PKI                              */
/*                                                                                             */
/***********************************************************************************************/
/***********************************************************************************************/


/************************************************************************************/
/**
 * @brief This function executes the RSA primitive: private key CRT exponent
 *         
 *    Algorithm [PKCS #1 v2.1]:
 *
 *     1. If NonCRT exponent, then  M  =  C ** D  mod N. 
 *          
 *     Where: M- message representative, C- ciphertext, D- priv.exponent, N- modulus,
 *            ** - exponentiation symbol.
 *    
 *     Note: PKA registers used: r0,r1,r2,r3,r4,  r30,r31.
 *
 * @param[in]     PubKey_ptr    - the private key database.
 * @param[in/out] PrivData_ptr  - the structure, containing DataIn and DataOut buffers.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_ExecPrivKeyExpNonCrt( 
                                     CRYSRSAPrivKey_t    *PrivKey_ptr ,
                                     CRYS_RSAPrimeData_t *PrivData_ptr,
                                     DxUint32_t           VirtualHwBaseAddr );
                                     

/***********************************************************************************/                                     
/**
 * @brief This function executes the RSA primitive: private key CRT exponent  
 *         
 *    Algorithm [PKCS #1 v2.1]:
 *
 *   CRT exponentiation algorithm:  
 *        1. M1  =  C ** dP mod P, 
 *        2. M2  =  C ** dQ mod Q;         
 *        3  h = (M1-M2)*qInv mod P;
 *        4. M = M2 + Q * h. 
 *
 *     Where: M- message representative, C- ciphertext, D- priv.exponent, N- modulus,
 *            P,Q,dP,dQ, qInv - CRT private key parameters;
 *            ** - exponentiation symbol.
 *    
 *     Note: 12 PKA registers are used: r0-r9,  r30,r31.
 *
 * @param[in]     PubKey_ptr    - the private key database.
 * @param[in/out] PrivData_ptr  - the structure, containing DataIn and DataOut buffers.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CRYSError_t LLF_PKI_ExecPrivKeyExpCrt( 
                                     CRYSRSAPrivKey_t    *PrivKey_ptr ,
                                     CRYS_RSAPrimeData_t *PrivData_ptr,
                                     DxUint32_t           VirtualHwBaseAddr ); 
                                     
                                                                              
                                          

/* ***************************************************************************** */
/* ****************        KeyGen functions              *********************** */
/* ***************************************************************************** */

/*************************************************************************************/
/**
 * @brief This function is used to fined a valid prime for the Key Gen.
 *
 * @param[in,out] rP - The virtual pointer to the prime P register.
 * @param[in] PSizeInBits - The prime size in bits.
 * @param[in] rE - The virtual pointer to the public exponent register.
 * @param[in] ESizeInBits - The public exponent size in bits.
 * @param[in] testsCount         - Count of Miller-Rabin tests. If testsCount = 0,
 *                                 then count of tests will be set automatically
 * @param[in] regTemps_ptr - pointer to temp PKA registers list (9 single registers
 *            of size according to PSizeInBits ).
 * @param[in] tempsCount - count of temp registers in the list (9). 
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * assumptions : .  
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_PKI_KG_FindPrime(
                                 DxInt8_t    rP, 
                                 DxUint32_t  PSizeInBits,
                                 DxInt8_t    rE, 
                                 DxUint32_t  ESizeInBits,
                                 DxUint32_t  testsCount,
                                 DxInt8_t   *regTemps_ptr,
                                 DxUint32_t  tempsCount, 
                                 DxUint32_t *temp_ptr, 
                                 DxUint32_t  VirtualHwBaseAddr);
                                 
/*************************************************************************************/
/**
 * @brief The LLF_PKI_genKeyNonCrt generates a public and private RSA keys in NonCRT mode.
 *
 *    The function initializes the PKA according to used registers size, calculates
 *    keys components and finishes PKA operations.
 *           
 *
 * @param[in] e_ptr              - The pointer to the public exponent.
 * @param[in] eSizeInBits        - Size of public exponent in bits.
 * @param[in] n_ptr              - The pointer .
 * @param[in] eSizeInBits        - The public exponent size in bits.
 * @param[in] nLenInBits         - The required size of the key in bits.
 * @param[in] testsCount         - Count of Miller-Rabin tests. If testsCount = 0,
 *                                 then count of tests will be set automatically
 * @param[in]  Success_ptr       - The pointer to the flag of success generation of P,Q.              
 * @param[out] p_ptr             - The pointer to the first factor.
 * @param[out] q_ptr             - The ponter to the second factor.
 * @param[out] n_ptr             - The pointer to the public modolus key.
 * @param[out] d_ptr             - The pointer to the private exponent (non CRT).
 * @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

CRYSError_t LLF_PKI_genKeyNonCrt(
			DxUint32_t *e_ptr,  
			DxUint32_t  eSizeInBits,  
			DxUint32_t  nSizeInBits,
			DxUint32_t  testsCount,            
			DxUint32_t *Success_ptr,
			DxUint32_t *p_ptr,
			DxUint32_t *q_ptr,
			DxUint32_t *n_ptr,  
			DxUint32_t *d_ptr, 
			DxUint32_t *temp_ptr, 
			DxUint32_t  VirtualHwBaseAddr );


/*************************************************************************************/
/**
 * @brief The LLF_PKI_genKeyCrt generates a public and private keys on CRT mode
 *
 *
 * @param[in]  e_ptr             - The pointer to the public exponent.
 * @param[in]  eSizeInBits       - The public exponent size in bits.
 * @param[in]  nSizeInBits       - The size of the key modulus in bits.
 * @param[out] Success_ptr       - The pointer to index of success of the generation.
 * @param[out] p_ptr             - The first factor pointer. In - random number, 
 *                                 out - prime (if successed).
 * @param[out] q_ptr             - The second factor pointer. In - random number, 
 *                                 out - prime (if successed).
 * @param[out] n_ptr             - The pointer to the modulus of the key.
 * @param[out] dp_ptr            - The private exponent for first factor.
 * @param[out] dq_ptr            - The private exponent for second factor.
 * @param[out] qinv_ptr          - The modular inverse of q relatively to modulus p.
 * @param[in]  VirtualHwBaseAddr - Virtual HW base address, passed by user.
 *
 *    Assuming: - eSizeInBits < nSizeInBits/2.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_PKI_genKeyCrt(
			DxUint32_t *e_ptr,         /*in*/ 
			DxUint32_t  eSizeInBits,   /*in*/  
			DxUint32_t  nSizeInBits,   /*in*/
			DxUint32_t  testsCount,    /*in*/        
			DxUint32_t *Success_ptr,   /*out*/
			DxUint32_t *p_ptr,         /*in/out*/
			DxUint32_t *q_ptr,         /*in/out*/
			DxUint32_t *n_ptr,         /*out*/
			DxUint32_t *dp_ptr,        /*out*/
			DxUint32_t *dq_ptr,        /*out*/
			DxUint32_t *qInv_ptr,      /*out*/
			DxUint32_t *temp_ptr,      /*in*/
			DxUint32_t   VirtualHwBaseAddr ) /*in*/;
		                       

/*************************************************************************************/
/**
 * @brief The LLF_PKI_CalculateNandD calculates RSA modulus and private key in NonCRT mode.
 *
 *    The function initializes the PKA according to used registers size, calculates
 *    keys components and finishes PKA operations.
 *           
 *
 * @param[in]  e_ptr             - The pointer to the public exponent.
 * @param[in]  eSizeInBits       - The public exponent size in bits.
 * @param[in]  p_ptr             - The first factor pointer (LSWord is the left most).
 * @param[in]  q_ptr             - The second factor pointer (LSWord is the left most).
 * @param[in]  primeSizeInBits   - The sze of the prime factors in bits.
 * @param[out] n_ptr             - The pointer to the RSA modulus buffer.
 * @param[out] d_ptr             - The pointer to the private exponent (non CRT).
 * @param[in]  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

CRYSError_t LLF_PKI_CalculateNandD(
			DxUint32_t *e_ptr,  
			DxUint32_t  eSizeInBits,  
			DxUint32_t *p_ptr,
			DxUint32_t *q_ptr,
			DxUint32_t  primeSizeInBits,
			DxUint32_t *n_ptr,  
			DxUint32_t *d_ptr, 
			DxUint32_t  VirtualHwBaseAddr );                                                            
			
                           
/*************************************************************************************/
/**
 * @brief The LLF_PKI_CalculateCrtParams calculates a private key on CRT mode
 *
 *
 * @param[in]  e_ptr             - The pointer to the public exponent.
 * @param[in]  eSizeInBits       - The public exponent size in bits.
 * @param[in]  nSizeInBits       - The size of the key modulus in bits.
 * @param[in]  p_ptr             - The first factor pointer.
 * @param[in]  q_ptr             - The second factor pointer.
 * @param[out] dp_ptr            - The private exponent for first factor.
 * @param[out] dq_ptr            - The private exponent for second factor.
 * @param[out] qinv_ptr          - The modular inverse of q relatively to modulus p.
 * @param[in]  VirtualHwBaseAddr - Virtual HW base address, passed by user.
 *
 *   ????? Assuming: - eSizeInBits < nSizeInBits/2.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_PKI_CalculateCrtParams(
				DxUint32_t *e_ptr,  
				DxUint32_t eSizeInBits,  
				DxUint32_t nSizeInBits,
				DxUint32_t *p_ptr, 
				DxUint32_t *q_ptr,
				DxUint32_t *dp_ptr,
				DxUint32_t *dq_ptr,
				DxUint32_t *qInv_ptr,
				DxUint32_t  VirtualHwBaseAddr );  
                           


/*************************************************************************************/
/**
 * @brief This function is used to test a primality of big numbers.
 *
 *        The function performs assigned count of Rabin-Miller tests and one Lucas-Lehmer
 *        test according to testing mode:
 *              - for RSA according to ANSI X9.31 standard. 
 *              - for DH  according to ANSI X9.42 standard. 
 *
 * @param[in] P_ptr           - The pointer to the prime buff.
 * @param[in] sizeWords       - The prime size in words.
 * @param[in] rabinTestsCount - The count of Rabin-Miller tests repetition.
 * @param[in] isPrime         - The flag indicates primality:   
 *                                  if is not prime - DX_FALSE, otherwise - DX_TRUE.  
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 *         Assumptions : the max size supported is 2112 bits.
 *         
 *         NOTE:  For using in RSA module  size of each temp buffer must be of minimum size 
 *                of prime number P in words. 
 *                For using in ANSI X9.42 standard (DH,DSA algorithms) size of each temp buffer 
 *                must be of minimum 2 size of prime number P in words.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code.
 *
 */

 CRYSError_t LLF_PKI_primeTest(  
			DxUint32_t *P_ptr,                              
			DxInt32_t   sizeWords, 
			DxInt32_t   rabinTestsCount,
			DxInt8_t   *isPrime_ptr,
			CRYS_RSA_DH_PrimeTestMode_t primeTestMode,
			DxUint32_t *temp_ptr,
			DxUint32_t  VirtualHwBaseAddr );   
						    
						    
						       
						    

/* ***************************************************************************** */
/* ************   Auxiliary functions used in PKA               **************** */
/* ***************************************************************************** */

 
/***********      LLF_PKI_PKA_ClearBlockOfRegs function      **********************/
/**
 * @brief This function clears block of PKA registers. 
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is placed into LenID entry of 
 *                     sizes table.
 *
 * @param[in] firstReg    - Virtual address (number) of first register in block.
 * @param[in] countOfRegs - Count of registers to clear.
 * @param[in] LenId       - ID of entry of regsSizesTable defines registers lengts with word extension.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure an error code:
 *                       LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR 
 *
 */
 CRYSError_t LLF_PKI_PKA_ClearBlockOfRegs( 
                                DxUint8_t  firstReg,          /*in*/
                                DxUint8_t  countOfRegs,       /*in*/
                                DxUint8_t  LenID,             /*in*/
                                DxUint32_t VirtualHwBaseAddr  /*in*/ ); 
                                
                                
 
/***********      LLF_PKI_PKA_CopyDataIntoPkaReg function      **********************/
/**
 * @brief This function  copies source data into PKA register . 
 *
 *        Assumings: - PKA is initialized.
 *                   - Length of extended (by word) registers is placed into LenID entry of 
 *                     sizes table.
 *                   - If the extra word of register must be cleared then the user must 
 *                     set LenID according to extended register size.
 *
 * @param[in] dstReg       - Virtual address (number) of destination register.
 * @param[in] LenId        - ID of entry of regsSizesTable defines registers length with word extension.
 * @param[in] src_ptr      - Pointer to source buffer.
 * @param[in] sizeWords    - Data size in Words.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void LLF_PKI_PKA_CopyDataIntoPkaReg( 
                                DxInt8_t    dstReg,            /*out*/
                                DxUint8_t   LenID,             /*in*/
                                DxUint32_t *src_ptr,           /*in*/
                                DxUint32_t  sizeWords,         /*in*/
                                DxUint32_t  VirtualHwBaseAddr  /*in*/ ); 

                                
/***********      LLF_PKI_PKA_CopyDataFromPkaReg      **********************/
/**
 * @brief This function copies data from PKA register into output buffer . 
 *
 *        Assumings: - PKA is initialized.
 *                   - Length of extended (by word) registers is placed into LenID entry of 
 *                     sizes table.
 *                   - If the extra word of register must be cleared, then the user must 
 *                     set LenID according to extended register size.
 *
 * @param[in] dst_ptr      - Pointer to destination buffer.
 * @param[in] sizeWords - Source size in Words.
 * @param[in] srcReg       - Virtual address (number) of source PKA register.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void LLF_PKI_PKA_CopyDataFromPkaReg( 
                                DxUint32_t *dst_ptr,           /*out*/
                                DxUint32_t  sizeWords,         /*in*/
                                DxInt8_t    srcReg,            /*in*/
                                DxUint32_t  VirtualHwBaseAddr  /*in*/ ); 
                                
                              
/***********      LLF_PKI_PKA_GetBitFromPkaReg     **********************/
/**
 * @brief This function returns bit i from PKA register. 
 *
 *
 * @param[in] rX       - Virtual pointer to PKA register.
 * @param[in] LenId    - ID of entry of regsSizesTable containing rX register length
 *                       with word extension.  
 * @param[in] i        - number of bit to be tested.
 * @param[in] rT       - temp register. If it is not necessary to keep rX, then 
 *                       set rT=rX for saving memory space.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - returns the bit number i (counting from left).
 *
 */
 DxUint8_t  LLF_PKI_PKA_GetBitFromPkaReg( 
                                        DxInt8_t   rX,                                        
                                        DxUint32_t LenID, 
                                        DxInt32_t  i,
                                        DxInt8_t   rT,
                                        DxUint32_t VirtualHwBaseAddr );
                                        
                                        
/***********        LLF_PKI_PKA_ModDivideBy2            **********************/
/**
 * @brief This function performs modular division by 2: rRes = rX / 2 mod rN. 
 *
 *
 * @param[in] LenId  - ID of entry of regsSizesTable containing rX register length.
 * @param[in] rX     - Virtual pointer to PKA register X.
 * @param[out] rN    - Virtual pointer to PKA register, containing the modulus N. 
 * @param[out] rRes  - Virtual pointer to PKA register, containing the result.  
 * @param[in] Tag    - The user defined value (Tag <= 31), used for indication goals. 
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void  LLF_PKI_PKA_ModDivideBy2( 
                            DxUint8_t   LenID,
                            DxInt8_t    rX,
                            DxInt8_t    rN,
                            DxInt8_t    rRes, 
                            DxUint8_t   Tag,                                             
                            DxUint32_t  VirtualHwBaseAddr );                                        
                                        
                                                                                                                                      
/***********        LLF_PKI_PKA_BigShift            **********************/
/**
 * @brief The function shifts the operand A in PKA register by S bits according to operation code. 
 *
 *        Legal operation codes are: 
 *                          - SHR0 shift right, inserting left  0-s. 
 *                          - SHR1 shift right, inserting left  1-s.
 *                          - SHL0 shift left,  inserting right 0-s. 
 *                          - SHL1 shift left,  inserting right 1-s.
 *
 * @param[in] Opcode - Operation code (one of above stated codes).
 * @param[in] LenId  - ID of entry of regsSizesTable containing operation size. 
 * @param[in] OpA    - Virtual pointer to PKA register A.
 * @param[in] S      - Count of bits to shift (exactly). 
 * @param[out] Res   - Virtual pointer to PKA register, containing the result.  
 * @param[in] Tag    - The user defined value (Tag <= 31), used for indication goals. 
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 CRYSError_t   LLF_PKI_PKA_BigShift( 
                            DxUint32_t      Opcode,           /*in*/
                            DxUint8_t       LenID,            /*in*/
                            DxInt8_t        OpA,              /*in*/
                            DxUint32_t      S,                /*in*/
                            DxInt8_t        Res,              /*out*/
                            DxUint8_t       Tag,              /*in*/                                 
                            DxUint32_t      VirtualHwBaseAddr /*in*/  );
                            
                            
/***********   LLF_PKI_PKA_GetRegEffectiveSizeInBits  **********************/
/**
 * @brief This function returns effective size in bits of data placed in PKA register. 
 *
 *
 * @param[in] LenId  - ID of entry of regsSizesTable containing rX register length 
 *                     with word extension.
 * @param[in] reg    - Virtual pointer to PKA register.
 * @param[in] Tag    - The user defined value (Tag <= 31), used for indication goals. 
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - effective size of data in register (bits).
 *
 */
 DxUint32_t   LLF_PKI_PKA_GetRegEffectiveSizeInBits( 
                            DxUint8_t       LenID,            /*in*/
                            DxInt8_t        reg,              /*in*/
                            DxUint8_t       Tag,              /*in*/                                 
                            DxUint32_t      VirtualHwBaseAddr /*in*/  ); 
                            

/***********   LLF_PKI_PKA_GetWordFromRegister  **********************/
/**
 * @brief This function returns word i from PKA register rReg. 
 *
 *
 * @param[in] LenId  - ID of entry of regsSizesTable containing rX register lengts 
 *                     with word extension.
 * @param[in] rReg   - Virtual pointer to PKA register.
 * @param[in] i      - The word index in register.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - word i from register rReg.
 *
 */
 DxUint32_t   LLF_PKI_PKA_GetWordFromRegister( 
                            DxUint8_t       LenID,            /*in*/
                            DxInt8_t        rReg,             /*in*/
                            DxUint32_t      i,                /*in*/
                            DxUint32_t      VirtualHwBaseAddr /*in*/  );
                            
                            
/***********   LLF_PKI_PKA_SetWordIntoRegister  **********************/
/**
 * @brief This function returns word i from PKA register rReg. 
 *
 *
 * @param[in] LenId  - ID of entry of regsSizesTable containing rX register length 
 *                     with word extension.
 * @param[in] rReg   - Virtual pointer to PKA register.
 * @param[in] i      - The word index in register.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return value.
 *
 */
 void   LLF_PKI_PKA_SetWordIntoRegister( 
                            DxUint8_t       LenID,            /*in*/
                            DxUint32_t      word,             /*in*/ 
                            DxInt8_t        rReg,             /*in*/
                            DxUint32_t      i,                /*in*/
                            DxUint32_t      VirtualHwBaseAddr /*in*/  ); 
                            
/***********   LLF_PKI_PKA_SwapRegsPhisAddr  **********************/
/**
 * @brief This function swaps the phisical addresses of PKA registers in RegsMapTable.
 *
 *        The function may performs swapping for one or two pair of registers
 *        r0<=>r1, r2<=>r3. If some register pointer is equalled RES_DISCARD
 *        value, then the function don't performs swapping for this pair registers.  
 *
 *        This function is effective for changing the modulus and Barrett tag during   
 *        execution of program with different moduluses together.
 *
 * @param[in] r0,r1  - Pointers to virtual pointers to PKA registers to be swapped.
 * @param[in] r2,r3  - Pointers to virtual pointers to PKA registers to be swapped.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return value.
 *
 */
 void  LLF_PKI_PKA_SwapRegsPhisAddr( 
                            DxInt8_t        *r0,               /*in*/ 
                            DxInt8_t        *r1,               /*in*/
                            DxInt8_t        *r2,               /*in*/ 
                            DxInt8_t        *r3,               /*in*/
                            DxUint32_t      VirtualHwBaseAddr /*in*/  ); 
                            
                            

#ifdef __cplusplus
}
#endif

#endif
