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


/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "dx_vos_mem.h"
#include "dx_vos_memmap.h"
#include "dx_vos_basetypes.h"
#ifndef DX_OEM_FW
#include "crys.h"
#else
#include "oem_crys.h"
#endif

#include "crys_common.h"
#include "crys_common_math.h"
#include "llf_pki.h"
#include "llf_pki_error.h"
#include "llf_pki_rsa.h"
#include "bsp.h"
#include "dx_cc_regs.h"
#include "llf_pki_hw_defs.h"
#include "dx_hal_plat.h"
#ifdef LLF_PKI_PKA_DEBUG /* for debug goals only */
#include "plat_print_dbg.h"
#endif


 
/************************ Defines ******************************/

                    
/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/* importing the base address of the CRYPTO Cell; this value must be  
   initialized in the CRYS_Init function */
extern DxUint32_t PLAT_CryptoCellBaseAddr;


#ifdef LLF_PKI_PKA_DEBUG
DxUint32_t tempRes[LLF_PKI_PKA_MAX_REGISTER_SIZE_WORDS];
DxUint32_t tempRes1[LLF_PKI_PKA_MAX_REGISTER_SIZE_WORDS];
#endif


/************************ Public Functions ******************************/
 


/* ***************************************************************************** */
/* *********      LLF PKI PKA initialisation functions and macros      ********* */
/* ***************************************************************************** */

#ifndef _INTERNAL_CRYS_BASE_PKI_IN_ROM
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
                             DxUint32_t                VirtualHwBaseAddr /*in*/)
{
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error;
   
   /* maximal and minimal sizes stated in RegsSizes_ptr */
   DxUint32_t  maxSize, minSize, MaxSizeWords;
   
   /* loop variable */
   DxUint32_t  i;
 
  
   /* FUNCTION LOGIC */
   
   /* ....... initialize local variables ...... */
  
   
   Error = CRYS_OK;
   
   maxSize = 0;
   minSize = PKA_SIZE_ENTRY_NOT_USED;
  
   
   /* check the input parameters */
   
   if( IsDefaultMap > 1 )
      return  LLF_PKI_PKA_SET_MAP_MODE_ERROR;
   
   
   /* 1. Case of user defined settings */
   if( IsDefaultMap == 0 )  
   {
      #ifdef LLF_PKI_PKA_DEBUG
       /* check sizes array pointer */
	   if( RegsSizes_ptr == DX_NULL )   
	   	  return  LLF_PKI_PKA_ILLEGAL_PTR_ERROR;   

	   if( CountOfSizes == 0 || CountOfSizes > 8 )   
	      return  LLF_PKI_PKA_ENTRIES_COUNT_ERROR;
	  #endif
	      
	   /* find maximal and minimal sizes  */  
	   for( i = 0; i < CountOfSizes; i++ )
	   {       
	   	   if( maxSize < RegsSizes_ptr[i] && RegsSizes_ptr[i] != PKA_SIZE_ENTRY_NOT_USED )
	   	      maxSize = RegsSizes_ptr[i];
	   	      
	   	   if( minSize > RegsSizes_ptr[i] )
	   	      minSize = RegsSizes_ptr[i];
	   }
      #ifdef LLF_PKI_PKA_DEBUG
	   /*  check, that maximal size is not less 128 bit and not grate 2112 bit */
	   if( (minSize < LLF_PKI_PKA_MIN_OPERATION_SIZE_BITS - 32) || 
	       (maxSize > LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS + 32) )
	      return  LLF_PKI_PKA_REGISTER_SIZES_ERROR;
	  #endif 
	       
	   /* set sizes into PKA registers sizes table */
	   for( i = 0; i < CountOfSizes; i++ )
	   {
	   	  DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL ,PKA_L0), RegsSizes_ptr[i] );   

	   }  
   }
   
   /* 2. Case of default settings */
   else  
   {
      #ifdef LLF_PKI_PKA_DEBUG
	   /* check maximal size */
	   if( MaxSizeBits < LLF_PKI_PKA_MIN_OPERATION_SIZE_BITS || 
	       MaxSizeBits > LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS )
	       
	      return  LLF_PKI_PKA_REGISTER_SIZES_ERROR;
	  #endif
	      
	   MaxSizeWords = ( MaxSizeBits + 31 ) / 32;  
   	   /* write exact size into first table entry */
   	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL ,PKA_L0), MaxSizeBits );


   	   /* write size with extra word into tab[1] = tab[0] + 32 */
   	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL ,PKA_L0) + 4, 
   	                                32*MaxSizeWords + 32 ); 


       /* count of entries, which was set */
       CountOfSizes = 2;
   }
   
   /* remaining entries set to PKA_SIZE_ENTRY_NOT_USED for debugging goals */ 
   for( i = CountOfSizes; i < 8; i++ )
   {
   	  DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_L0) + 4*i, PKA_SIZE_ENTRY_NOT_USED );
   }   
 
   return Error;

} /* END of the finction  LLF_PKI_PKA_SetRegsSizesTab */
                                     



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
                          DxUint32_t              VirtualHwBaseAddr   /*in*/ )
{
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error;   
   
   /* current register physical address */
   DxUint32_t  CurrentAddr;
   
   /* default max size and count of registers */
   DxUint32_t  defaultMaxSize, defaultCountOfRegs ;

   
   /* loop variable */
   DxUint32_t  i;
 
  
   /* FUNCTION LOGIC */
   
   /* ....... initialize local variables ...... */
    
   Error = CRYS_OK;
   
   CurrentAddr = LLF_PKI_PKA_DATA_REGS_BASE_ADDR;
  
   /* Step1. Check the input parameters (besides the RegsMap_ptr) */
 
 #ifdef LLF_PKI_PKA_DEBUG
   
   if( IsDefaultMap > 1 )
      return LLF_PKI_PKA_SET_MAP_MODE_ERROR;
      
   if( MaxSizeWords < 4 || 
       MaxSizeWords > LLF_PKI_PKA_MAX_REGISTER_SIZE_WORDS )
      return  LLF_PKI_PKA_REGISTER_SIZES_ERROR; 
 #endif	      
       
   /* Step 2. Default mapping mode */
   if( IsDefaultMap == 1 )
   {
       /* set default registers size and their count according to maximal allowed size
          of operands 2112 bit = 66 words. Note register must be by 1 word longer, than operand
          for some modular operations  */          
       defaultMaxSize =  32*MaxSizeWords;
       defaultCountOfRegs =  min(32, (DxUint32_t)(8*LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES) / defaultMaxSize);
      
       /* set registers addresses (offsets) into the mapping table,
          size of each register = MaxRegsSizeWords */ 
          
       /* set addresses of N,NP and user requested registers (excluding 2 temp registers T0,T1) */        
   	   for( i = 0; i < 32-2; i++ )
   	   {
   	   	  if( i < defaultCountOfRegs - 2 )
   	   	  {
	   	   	  DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, MEMORY_MAP0) + 4*i, 
	   	   	                               CurrentAddr ); 

			  CurrentAddr = CurrentAddr + defaultMaxSize/8;          
   	   	  }
   	   	  
   	   	  else
   	   	  {
   	   	          /* write designation, that PKI entry is not in use */ 
   	   	  	  DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, MEMORY_MAP0)+ 4*i, 
	   	   	                               PKA_ADDRESS_ENTRY_NOT_USED );
   	   	  }
   	   }
   	   /* set addresses of 2 temp registers: T0=30, T1=31 */
   	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, MEMORY_MAP0)+ 4*30, 
   	   	                            CurrentAddr );

   	   CurrentAddr = CurrentAddr + defaultMaxSize/8;

   	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, MEMORY_MAP0) + 4*31, 
   	   	                            CurrentAddr );

	   /* output maximal count of allowed registers */
	   *CountOfRegs  =  defaultCountOfRegs;	  

	   /* set default virtual addresses of N,NP,T0,T1 registers into N_NP_T0_T1_Reg */
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), 
	   	                            (DxUint32_t)LLF_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL ); 

   }
  
   /* Step 3. User defined mapping mode */
   if( IsDefaultMap == 0 )
   {
     #ifdef LLF_PKI_PKA_DEBUG
     
       /* maximal size in input buffer RegsSizes_ptr */
	   DxUint32_t  TotalRegsSizeWords;
	   
		/* check the mapping structure pointer */
	   if( RegsMap_ptr == DX_NULL )   
	   	  return  LLF_PKI_PKA_ILLEGAL_PTR_ERROR;

	   /* check count of registers */
	   if( *CountOfRegs == 0 || 
	       *CountOfRegs > 31 )   
	      return  LLF_PKI_PKA_ENTRIES_COUNT_ERROR;
	   
	   /* check total memory size for registers, including special registers */	   
	   TotalRegsSizeWords = (MaxSizeWords + 1)*(*CountOfRegs) ;
	    
	   if( TotalRegsSizeWords > LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES / 4 )	   
	      return LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR;
	   
     #endif	
    
       /* set registers addresses (offsets), including N,NP,T0,T1 , into the mapping table;
          Size of each register = MaxRegsSizeWords */           
   	   for( i = 0; i < *CountOfRegs; i++ )
   	   {
   	   	  DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, MEMORY_MAP0) + 4*RegsMap_ptr->RegsNum[i], 
   	   	          RegsMap_ptr->RegsAddr[i] );    
   	   } 
   	   
	   /* set virtual addresses of N,NP,T0,T1 registers into N_NP_T0_T1_Reg */
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), N_NP_T0_T1 );    
   }
  
  
   return Error;
   
}  /* END of the finction  LLF_PKI_PKA_SetRegsMapTab */



 

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
                 DxUint32_t               VirtualHwBaseAddr   /*in*/ )
 {
  
   /* LOCAL DECLARATIONS */

   /* Error identification */
   CRYSError_t Error;
   
   DxUint32_t  addr ;
   
   
   /* FUNCTION LOGIC */
   
   /* ....... initialize local variables ...... */
    
   Error = CRYS_OK;
   
   /*     enabling the PKA clocks      */
   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET(CRY_KERNEL,PKA_CLK_ENABLE) , 0x1UL );   
   /* Get the PKA memory address offset from start of SRAM*/
   addr = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL,PKA_MSB_ADDR));	
   g_MemOffsetAddr = addr*0x1000;
   
   /* set PKI offset to upper 4K */
   /* (for DX debug to use SEP SRAM upper 4K ) */
   /*CRYS_PLAT_SYS_WriteRegister(0xC00000cc, 0x1UL);*/
 
  #ifdef LLF_PKI_PKA_DEBUG   
   /* if IsDefaultMap = 1, then set default mapping and sizes */
   if( IsDefaultMap > 1 )   
       return LLF_PKI_PKA_SET_MAP_MODE_ERROR;            
  #endif
  
   /* !!!! This is for the future using. Now the START_MEM_ADDR not exists in HW. 
      Set_StartAddress_REG:  Sets user passed start address for PKA accessible 
      data memory registers into special PKA register */
  /* LLF_PKI_PKA_Set_StartMemAddr_Reg (LLF_PKI_PKA_START_MEM_ADDR , VirtualHwBaseAddr); */
   		

  /* setting the PKA registers sizes table   */
   /*-----------------------------------------*/   
   Error = LLF_PKI_PKA_SetRegsSizesTab(
                                       RegsSizes_ptr,     /*in*/   
                                       CountOfSizes,      /*in*/
                                       OpSizeBits,        /*in*/
                                       IsDefaultMap,      /*in*/ 
                                       VirtualHwBaseAddr  /*in*/ );

   if( Error != CRYS_OK )       
       return Error;  

     
   /* setting the PKA registers mapping table */
   /*-----------------------------------------*/   
   Error = LLF_PKI_PKA_SetRegsMapTab(
                                     RegsMap_ptr,     	 /*in*/   
                                     &CountOfRegs,       /*in/out*/
                                     RegSizeWords,       /*in*/ 
                                     N_NP_T0_T1,         /*in*/
                                     IsDefaultMap,       /*in*/
                                     VirtualHwBaseAddr   /*in*/ );
 
   if( Error != CRYS_OK )       
       return Error; 
 
   /* set size of register into RegsSizesTable */ 
   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_L0) + 3*4, 
                                32*RegSizeWords ); 

   /* clean PKA data memory */
   LLF_PKI_PKA_ClearBlockOfRegs(0/* FirstReg*/, CountOfRegs-2, 3/*LenID*/, VirtualHwBaseAddr );	                      

   /* clean temp PKA registers 30,31 */ 
   LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );	
    
   LLF_PKI_PKA_GetRegAddress(30/*VirtReg*/,addr/*physAddr*/, VirtualHwBaseAddr); 
   LLF_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , addr , RegSizeWords );
   LLF_PKI_PKA_GetRegAddress(31/*VirtReg*/,addr/*physAddr*/, VirtualHwBaseAddr); 
   LLF_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , addr , RegSizeWords );                     

   /* ......  End of function ...... */  
       
   return Error;                                      

 }
 
#ifndef DX_OEM_FW
/***********      LLF_PKI_PKA_InitPKA function      **********************/
/**
 * @brief This function initializes the PKA engine with exact sizes. 
 *
 *    The function performs the following:
 *      - enables PKA clocks, 
 *      - cleans all PKA memory registers.
 *      - initializes the PKA_SizesTable, PKA_MappingTable, so that all registers 
 *        are set sequentionqlly in the mapping table and rT0, rT1 are the last. 
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
			 DxUint32_t  virtualHwBaseAddr   /*in*/ )
 {
  
	/* LOCAL DECLARATIONS */

	/* Error identification */
	CRYSError_t Error;

	DxUint32_t  regSizeWords, countOfRegs, i;

	/* FUNCTION LOGIC */
	
#ifdef LLF_PKI_PKA_DEBUG
	/* check maximal size */
	if( opSizeBits < LLF_PKI_PKA_MIN_OPERATION_SIZE_BITS || 
	    opSizeBits > LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS )
	   
	  return  LLF_PKI_PKA_OPERATION_SIZE_ERROR;
#endif


	Error = CRYS_OK;
	/*     enabling the PKA clocks      */   
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, PKA_CLK_ENABLE), 0x1UL); 

	/* set PKI offset to upper 4K (for DX debug to use SEP SRAM upper 4K ) */
	/*CRYS_PLAT_SYS_WriteRegister(0xC00000cc, 0x1UL);*/

	/* clean PKA registers memory */
	LLF_PKI_HW_CLEAR_PKA_MEM(virtualHwBaseAddr , LLF_PKI_PKA_DATA_REGS_BASE_ADDR , LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES/4);
    
	/*-----------------------------------------*/
	/* setting the PKA registers sizes table   */
	/*-----------------------------------------*/

	regSizeWords = (opSizeBits+31)/32 + 1;  

	/* write operation and register sizes into first table entries */
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, PKA_L0), opSizeBits); 
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, PKA_L0) + 4, 32*regSizeWords); 

	/*-----------------------------------------*/ 
	/* setting the PKA registers mapping table */
	/*-----------------------------------------*/ 
	  
	/* set count registers according to memory and register sizes */          
	countOfRegs =  min(32, (DxUint32_t)(LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES) / (4*regSizeWords));

	/* set registers addresses (offsets) into the mapping table,
	   size of each register = MaxRegsSizeWords */ 
	  
	/* set addresses of rN=0,rNP=1 and user requested registers (excluding 2 temp registers rT0=30, rT1=31) */ 
	for(i = 0; i < 32-2; i++)
	{
		if(i < countOfRegs)
		{
			DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, MEMORY_MAP0) + 4*i, 
	   	   	                               LLF_PKI_PKA_DATA_REGS_BASE_ADDR + 4*i*regSizeWords);
		}
		else
		{
			/* write designation, that PKI entry is not in use */ 
			DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, MEMORY_MAP0) + 4*i, PKA_ADDRESS_ENTRY_NOT_USED);	
		}
	}

	   /* set default virtual addresses of N,NP,T0,T1 registers into N_NP_T0_T1_Reg */
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), 
	   	                            (DxUint32_t)LLF_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL ); 

	return Error;                                      

 } /* End of LLF_PKI_PKA_InitPkaEx function */
 
 #endif
/***********      LLF_PKI_PKA_FinishPKA function      **********************/
/**
 * @brief This function ends the PKA engine session and disables PKA clocks. 
 *
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void LLF_PKI_PKA_FinishPKA( DxUint32_t VirtualHwBaseAddr /*in*/ )
 {
  
   /* FUNCTION LOGIC */
   
   /*  terminate PKA operations (Tag is not needed) */
   LLF_PKI_PKA_Terminate(0/*Tag*/, VirtualHwBaseAddr);
   
   /* close PKA clocks */
   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,PKA_CLK_ENABLE), 0x0UL );   
    
 }
 

 						    
/*******************************************************************************/
/************    LLF PKI PKA mathematic functions and macros     ***************/
/*******************************************************************************/


/*******   LLF_PKI_PKA_ExecCalcNpAndInitModOp function (virtual pointers)   **********/
/**
 * @brief The function calculates the Barret modulus tag NP, used in   
 *        modular operations, and sets the result into PKA NP-register . 
 *
 *        The modulus tag is calculated as: NP = A / N, 
 *             where: A = 2^(sizeN + 132 ) -according to HW requirements,
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
                                         DxUint32_t  VirtualHwBaseAddr )
{
  
   /* LOCAL DECLARATIONS */

   /* Error identification */
   CRYSError_t Error;
       
   /* local variables */
   DxUint32_t  numBits, numWords; 
   
   /* shift count */
   DxUint32_t S;
   
   DxUint32_t i;
   
   
   /* initializations */
   
   /* Set S = 132 */
   S = 132;
       
   /*---------------------------------------------------------------------*/
   /* Step 1,2. Set registers: Set OpA = 2^(sizeN+32)                    */
   /*           Registers using: 0 - N (is set in register 0,         */
   /*           1 - NP, temp regs: rT0 (A), rT1, rT2.                     */
   /*           LenID: 0 - exact size, 1 - exact+32 bit                   */
   /*---------------------------------------------------------------------*/
   
   /* set register rT0 = 0 */   
   LLF_PKI_PKA_Clr( LenID+1, rT0/*OpA*/, 0/*Tag*/,VirtualHwBaseAddr); /* r2 = 0 */
  
   /* calculate bit position of said bit in the word */
   numBits = modSizeBits % 32;
   numWords = modSizeBits / 32;
   
   /* set 1 into register rT0 */
   LLF_PKI_PKA_Set0( LenID+1, rT0/*OpA*/, rT0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
   
   /* shift 1 to numBits+31 position */
   if(numBits > 0)
   {
	   LLF_PKI_PKA_SHL0( LenID+1, rT0/*OpA*/, numBits-1/*S*/, rT0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
   }
 
   /* shift to word position */
   for( i = 0; i < numWords; i++ )
   {
	   LLF_PKI_PKA_SHL0( LenID+1, rT0/*OpA*/, 31/*S*/, rT0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
   }
   
      
   /*--------------------------------------------------------------------*/
   /* Step 3.  Dividing:  (OpA * 2**S) / N                               */
   /*--------------------------------------------------------------------*/
   Error = LLF_PKI_PKA_DivLongNum(
				LenID,        /*LenID*/
				rT0,          /*opA*/
				S,            /*shift*/
				0,            /*opB = N*/
				1,            /*Res NP*/
				rT1,          /*temp reg*/
				rT2,          /*temp reg*/  
				VirtualHwBaseAddr ); 
   
   /* Note: PKA done is not needed, it was performed in previous called function*/
    
    return Error;
    
}  /* END OF LLF_PKI_PKA_ExecCalcNpAndInitModOp */



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
 *                  - 5 PKA registers are used: OpA, OpB, Res, rT1, rT2.
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
			DxUint32_t     VirtualHwBaseAddr )
{
   /* LOCAL DECLARATIONS */

   /* local variables */
   DxUint32_t  nBits, nWords;
   
   /* loop variable */
   DxUint32_t  i;
 
   /* current shift count */
   DxInt8_t s1;
   

   /* FUNCTION LOGIC */
   
#ifdef LLF_PKI_PKA_DEBUG_ 

   DxUint32_t RegSizeBits, RegSizeBytes;
   
   LLF_PKI_PKA_ReadRegSize(RegSizeBits, LenID+1, VirtualHwBaseAddr);
   RegSizeBytes = (RegSizeBits+31)/32;
   
#endif   
  
   /* initializations */
   
   /* calculate shifting parameters (words and bits ) */
   nWords = ((DxUint32_t)S + 31) / 32;
   nBits  = (DxUint32_t)S % 32;
      
   /* copy operand OpA (including extra word) into temp reg rT1 */ 
   LLF_PKI_PKA_Copy( LenID+1, rT1/*dst*/, OpA/*src*/, 0 /*Tag*/, VirtualHwBaseAddr );
   
   /* set Res = 0 (including extra word) */
   LLF_PKI_PKA_Clear( LenID+1, Res/*dst*/, 0 /*Tag*/, VirtualHwBaseAddr );   
   
   /* set s1 = 0 for first dividing in loop */
   s1 = 0;
    
   /*----------------------------------------------------*/
   /* Step 1.  Shifting and dividing loop                */
   /*----------------------------------------------------*/
      
   for( i = 0; i < nWords; i++ )
   {
	/* 3.1 set shift value s1  */
	if( i > 0 )   	    
	s1 = 32;
	
	else   	    
	s1 = nBits;   	    
	
	/* 3.2. shift: rT1 = rT1 * 2**s1 (in code (s1-1), because PKA performs S+1 shifts) */
	if( s1 > 0 ){
		LLF_PKI_PKA_SHL0( LenID+1, rT1/*OpA*/, (s1-1)/*S*/, rT1 /*Res*/, 0 /*Tag*/, VirtualHwBaseAddr );    	    	
	}
	
	/* 3.3. perform PKA_Div for calculating a quotient rT2 = floor(rT1 / N) 
		and remainder rT1 = rT1 % OpB  */
	LLF_PKI_PKA_Div( LenID+1, rT1 /*OpA*/, OpB /*B*/, rT2/*Res*/, 0 /*Tag*/, VirtualHwBaseAddr );
	
#ifdef LLF_PKI_PKA_DEBUG_ 	
	/* debug copy result into temp buffer */
	PLAT_LOG_DEV_PRINT(( 0 , " Func DivLongNum; St3.3 i= %d.  rT1= = rT1 % OpB\n",i ));
	DX_VOS_MemSetZero((DxUint8_t*)tempRes, sizeof(tempRes));
	LLF_PKI_PKA_CopyDataFromPkaReg(
		tempRes/*dst_ptr*/, RegSizeWords,
		rT1/*srcReg*/, VirtualHwBaseAddr );
#endif 
                              
         /* 3.4. Res = Res * 2**s1 + Res;   */
        if(s1 > 0){
        	LLF_PKI_PKA_SHL0( LenID+1, Res /*OpA*/, (s1-1)/*S*/, Res /*Res*/, 0 /*Tag*/, VirtualHwBaseAddr ); 	
        }
        
        LLF_PKI_PKA_Add( LenID+1, Res /*OpA*/, rT2 /*OpB*/, Res /*Res*/, 0 /*Tag*/, VirtualHwBaseAddr );
   }
   
   LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );
    
   return CRYS_OK;
    
}  /* END OF LLF_PKI_PKA_DivLongNum */




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
                          DxUint32_t         VirtualHwBaseAddr  /*in*/ )
{
 
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error = CRYS_OK;
   
   /* exact size of modulus in bytes and in words  */
   DxUint32_t  N_sizeWords;
   
 
   /* FUNCTION LOGIC */
   
   /* check input parameters */
  
  #ifdef LLF_PKI_PKA_DEBUG
   
   if( N_ptr  == DX_NULL ||
       NP_ptr == DX_NULL )
       
       return LLF_PKI_PKA_ILLEGAL_PTR_ERROR;
       
   if( N_sizeBits  < LLF_PKI_PKA_MIN_OPERATION_SIZE_BITS  ||
       N_sizeBits  > LLF_PKI_PKA_MAX_OPERATION_SIZE_BITS  )
       
       return LLF_PKI_PKA_ILLEGAL_OPERAND_LEN_ERROR;

  #endif
       
   /* calculate size of modulus in bytes and in words */
   N_sizeWords = (N_sizeBits + 31) / 32;
   
   /* copy modulus N into r0 register */
   LLF_PKI_PKA_CopyDataIntoPkaReg(0/*dstReg*/, 1/*LenID*/,  N_ptr/*src_ptr*/, 
                                  N_sizeWords, VirtualHwBaseAddr); 

   /* if NpCreateFlag == PKA_SetNP, then set NP into PKA register r1 */
   if(NpCreateFlag == PKA_SetNP){
	/* copy the NP into r1 register NP */
	LLF_PKI_PKA_CopyDataIntoPkaReg(1/*dstReg*/, 1/*LenID*/, NP_ptr/*src_ptr*/, 
				     LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, VirtualHwBaseAddr); 
   }
   
   else
   {
	   /*---------------------------------------------------------------*/
	   /*       execute calculation of NP and initialization of PKA     */ 
	   /*---------------------------------------------------------------*/
	   
	   LLF_PKI_PKA_ExecCalcNpAndInitModOp(
	                                 0/*LenID*/, 
	                                 N_sizeBits,
	                                 rT0,rT1,rT2, 
	                                 VirtualHwBaseAddr);
	   
	   /* output of NP value */
	   LLF_PKI_PKA_CopyDataFromPkaReg(NP_ptr/*dst_ptr*/, LLF_PKI_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS, 
					1/*srcReg*/, VirtualHwBaseAddr);
   }
                      
   /* End of the function */
   return Error;
                          

} /* END OF LLF_PKI_CalcNpAndInitModOp */


                               
/***********    _LLF_PKI_PKA_ExecOperation (with virtual pointers)     ******************/
/**
 * @brief This function executes any allowed PKA mathematic operation according to 
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
                                DxUint32_t      VirtualHwBaseAddr  /*in*/ )
 {
 	 
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error = CRYS_OK;
   
   /* full Operation Code word */
   DxUint32_t FullOpCode;
   
   /* testing status  */
   DxUint32_t status;   
  
 #ifdef LLF_PKI_PKA_DEBUG
    /* register size */
   DxUint8_t  OpPrint; 
   DxUint32_t OpSizeBits, OpSizeWords;
 #endif   

 
   /* FUNCTION LOGIC */
   
   /* if Res == RES_DISCARD , then result is discarded */
   if( Res == (DxInt8_t)RES_DISCARD )
   {
   		ResDiscard = 1;
   		Res = 0; 
   }
 #ifdef LLF_PKI_PKA_DEBUG
   /* set operation size according to LenID+1for debug copy and clearing registers */
   LLF_PKI_PKA_ReadRegSize(OpSizeBits, LenID, VirtualHwBaseAddr);
   OpSizeWords = (OpSizeBits+31)/32;
 #endif
 
   /*************************************************/
   /*      check input parameters                   */
   /*************************************************/

 
 #ifdef LLF_PKI_PKA_DEBUG
    
   if( Opcode  > LLF_PKI_PKA_MAX_OPCODE )
   {
       Error =  LLF_PKI_PKA_ILLEGAL_OPCODE_ERROR;
   	   goto End;
   }
       
   if( LenID > 7 )
   {
       Error = LLF_PKI_PKA_ILLEGAL_OPERAND_LEN_ERROR;
   	   goto End;
   }
       
   if( IsAImmed   > 1 || 
       IsBImmed   > 1 ||
       ResDiscard > 1 )  
   {
       Error = LLF_PKI_PKA_ILLEGAL_OPERAND_TYPE_ERROR; 
   	   goto End;
   }
       
   if(  ((int)OpA < -15) || 
        (OpA >  31)      ||
        ((int)OpB < -15) || 
        (OpB >  31)      || 
        ((int)Res < -15) ||
        (Res >  31)      || 
        (Tag >  31) ) 
   {
       Error = LLF_PKI_PKA_ILLEGAL_OPERAND_ERROR;
   	   goto End;
   }
       
  #endif     
  

 #ifdef LLF_PKI_PKA_DEBUG
   /* for ModInv and Div operation check, that modulus or divider are not 0 */		
   if( Opcode == PKA_ModInv || Opcode == PKA_Div)
   {
       DxInt8_t OpT;  /* number of register to test its Value = 0 */
       
       /* Set OpT: 0 - for ModInv, OpB - for Div */
       if(Opcode == PKA_ModInv)
       {
       	  Error = LLF_PKI_PKA_MODULUS_IS_NULL_ERROR;
       	  OpT = 0;
       }       		
       else
       {
       	  Error = LLF_PKI_PKA_DIVIDER_IS_NULL_ERROR;
       	  OpT = OpB;
       }
        		
	   /* Create full opcode word for add immediate 0 operation */
	   FullOpCode = LLF_PKI_PKA_FullOpCode( PKA_AddIm, LenID,0/*isAImmed*/,OpT/*N*/,1,0/*imm 0*/,1/*ResDiscard*/,0,Tag );

	   /* write full opcode into PKA OPCODE register */
	   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,OPCODE) , FullOpCode );
	                  
	   /* test zero bit of STATUS register */
	   LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	   
	   if( status == 1 )	      
	       goto End;
	   else
	       Error = CRYS_OK;    
   }
  
   /* for ModInv operation check, that OpB is odd, else return Error (can't calculate,
      the user must use other function) */
   if( Opcode == PKA_ModInv )
   {
	   /* Create full opcode word for Test bit 0 operation */
	   FullOpCode = LLF_PKI_PKA_FullOpCode(PKA_Test0, LenID, 0/*isAImmed*/, 0/*N*/, 1, 1/*imm 1*/, 1/*ResDiscard*/, 0, Tag);

	   /* write full opcode into PKA OPCODE register */
	   DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET (CRY_KERNEL, OPCODE), FullOpCode);
	                  
	   /* test zero bit of STATUS register */
	   LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	   
	   if( status == 1 )
	   {
	       Error = LLF_PKI_PKA_MOD_EVEN_USE_OTHER_FUNC_ERROR;
	   	   goto End;
	   }
	   
   }
 #endif  
 
  
   /*************************************************/
   /*      main PKI operation of this function      */
   /*************************************************/
  
   /* Create full opcode word  */
   FullOpCode = LLF_PKI_PKA_FullOpCode( Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag );
   
   /* write full opcode into PKA OPCODE register */
   DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,OPCODE) , FullOpCode );

  
   /*************************************************/  
   /* finishing operations for different cases      */
   /*************************************************/
   
   switch( Opcode )
   {
   	 case  PKA_Div:
       /* for Div operation check, that OpB != 0, else return Error (inverse not exists) */
       LLF_PKI_PKA_GET_StatusDivByZero(VirtualHwBaseAddr, status);	   
	   if( status == 1 )
	   {
	       Error = LLF_PKI_PKA_DIVIDER_IS_NULL_ERROR;
	   	   goto End;
	   }
	   
	   break; 
		   
     case PKA_Terminate:
     
       /* wait for PKA done bit */
       LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );

       break;
       
     default:
  
       /* wait for PKA pipe ready bit */
       LLF_PKI_WAIT_ON_PKA_PIPE_READY( VirtualHwBaseAddr );
   }
   
   
   #ifdef LLF_PKI_PKA_DEBUG /* for debug goals only */
 
       /* wait for PKA done bit */
	   LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );

       /* if operation Tag = Print, then copy result into tempRes buffer */
       if( 1/*Tag == PKA_TAG_DebugPtint*/ &&
           ResDiscard == 0 && Opcode != PKA_Terminate )
       {
	       DX_VOS_MemSetZero( tempRes, sizeof(tempRes) );	       
	       
	       LLF_PKI_PKA_CopyDataFromPkaReg( 
	                                tempRes/*dst_ptr*/, OpSizeWords, 
	                                Res/*srcReg*/, VirtualHwBaseAddr );
	       
	       if( Opcode == PKA_Div || Opcode == PKA_ModInv )
	       {
	       		if( Opcode == PKA_Div )
	       		    OpPrint = OpA;
	       		else
	       		    OpPrint = OpB;
	       		
   	 	        LLF_PKI_PKA_CopyDataFromPkaReg( 
	                                tempRes1/*dst_ptr*/, OpSizeWords, 
	                                OpPrint/*srcReg*/, VirtualHwBaseAddr );
	       }
      }
   #endif  
     
      
   /* End of function */
End: 

#ifdef LLF_PKI_PKA_DEBUG /* for debug goals only */
   if(Error != CRYS_OK) 
    	PLAT_LOG_DEV_PRINT(( 0 , "Error= 0x%08X Opcode= 0x%02X\n", Error, Opcode )); 
#endif
 
   return Error;  
         
 } /* END OF function _LLF_PKI_PKA_ExecOperation */
#endif //_INTERNAL_CRYS_BASE_PKI_IN_ROM


#ifndef _INTERNAL_CRYS_NO_RSA_KG_SUPPORT
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
                                DxUint32_t       VirtualHwBaseAddr  /*in*/ )
 {
 	 
   /* LOCAL DECLARATIONS */

   /* error identification */
   CRYSError_t Error = CRYS_OK;
   
   /* virtual pointer to modulus register, by default: N=0 */
   DxUint8_t  N = 0;
   
   DxUint32_t ModSizeBits, ModSizeWords;
   
   DxUint32_t status;
   
   /*  Initializations */ 
   
   LenID = 0; /*default ID of modulus size in RegsSizesTab */  
  
   
   /* FUNCTION LOGIC */   
     
   Tag = Tag;
   /*-------------------------------------------*/  
   /* Step 1.  Check the parity of the modulus  */
   /*-------------------------------------------*/
   /* test: is the modulus even? */
   LLF_PKI_PKA_Tst0(LenID, 0/*N*/, 0/*Tag*/, VirtualHwBaseAddr );   
   LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
   
   /* get modulus size */
   LLF_PKI_PKA_ReadRegSize(ModSizeBits, LenID, VirtualHwBaseAddr);
   ModSizeWords = (ModSizeBits + 31) / 32;
                 
   if( status == 0 /*odd N*/) 
   {
	   /*-------------------------------------------*/
	   /* Step 2.  Process case of odd modulus      */ 
       /*-------------------------------------------*/
	   Error = LLF_PKI_PKA_ModInv(LenID, OpB, Res, 0/*Tag*/, VirtualHwBaseAddr);
	                               
	   if( Error != CRYS_OK )
	   {
	   	   LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
	   	   
	   	   return Error;
	   }
   }
   
   else /*even N*/
   {
       /*-------------------------------------------*/  
       /* Step 3. Process case of even modulus      */ 
       /*-------------------------------------------*/
        
        /*------------------------------------------------------------*/
        /* in case of even B: calculate GCD and return error message, */
        /*  that inverse does not exists                              */
        /*------------------------------------------------------------*/
        /* check, is the operand B odd or even */
        LLF_PKI_PKA_Tst0(LenID, OpB, 0/*Tag*/, VirtualHwBaseAddr );   
        LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
        if( status == 1 ){
            return LLF_PKI_PKA_INVERSION_NOT_EXISTS_ERROR;
        }
        
        /*------------------------------------------------------------*/
        /* in case of odd B: calculate modular inverse and GCD        */
        /*------------------------------------------------------------*/
        
   		/* 3.1. Save previous modulus also into rT0 and into rT1 (rT1 - working copy)*/
   	LLF_PKI_PKA_Copy( LenID+1/*LenID*/, rT0/*OpDest*/, N/*OpSrc*/, 0/*Tag*/, VirtualHwBaseAddr );
   	LLF_PKI_PKA_Copy( LenID+1/*LenID*/, rT1/*OpDest*/, N/*OpSrc*/, 0/*Tag*/, VirtualHwBaseAddr );
   	    	
	/* 3.2. Set OpB into modulus register 0 ) */
	LLF_PKI_PKA_Copy( LenID+1/*LenID*/, N/*OpDest*/, OpB/*OpSrc*/, 0/*Tag*/, VirtualHwBaseAddr );		
   		
    	/* 3.3 Calculate Res =  1/N mod B  */	
   	Error = LLF_PKI_PKA_ModInv( LenID/*LenID*/, rT1/*mod N*/, Res, 1/*Tag*/, VirtualHwBaseAddr );

	if( Error != CRYS_OK )
		return Error;
			
	    /* restore modulus */
        LLF_PKI_PKA_Copy( LenID+1/*LenID*/, N/*OpDest*/, rT0/*OpSrc*/, 0/*Tag*/, VirtualHwBaseAddr );                         
	
  	/* 3.4. Calculate rT0 = PKA_HMul(N*Res) i.e. HighHalf + 1 word of(N*Res)
   	        Note: LenId=0, because this operation adds extra word itself */
        LLF_PKI_PKA_HMul( LenID, N, Res, rT0/*Result*/, 1/*Tag*/, VirtualHwBaseAddr );
        
        /* 3.5. Shift right rT0 for deleting 1 low word */
        LLF_PKI_PKA_SHR0( LenID, rT0/*N*/, 32-1/*S*/, rT0/*Result*/, 1/*Tag*/, VirtualHwBaseAddr );
               
        /* 3.6. Calculate rT2 = PKA_LMul(N*Res) i.e. LowHalf of(N*Res) */
        LLF_PKI_PKA_LMul( LenID, N, Res, rT2/*Result*/, 1/*Tag*/, VirtualHwBaseAddr );
        
        /* 3.6. Divide long num Res = (rT1 * 2**(ModSizeBits - 32))/B */        
   		Error = LLF_PKI_PKA_DivLongNum( 
			LenID,               /*LenID of exact size*/
			rT0,                 /*numerator*/ 
			32*ModSizeWords+32,     /*Shift*/
			OpB,                 /*divider*/ 
			Res,                 /*result*/
			rT1,
			rT3,   
			VirtualHwBaseAddr );
                                  
	   if( Error != CRYS_OK ){
	   	   LLF_PKI_PKA_Terminate( 0 /*Tag*/, VirtualHwBaseAddr );	   	   
	   	   return Error;
	   }
        

       /* 3.7. Subtract 1 from low part and divide it by B */
       LLF_PKI_PKA_SubIm( LenID, rT2/*numerat*/, 1/*OpB*/, rT2/*Result*/, 1/*Tag*/, VirtualHwBaseAddr );
       LLF_PKI_PKA_Div( LenID, rT2/*numerat*/, OpB/*divider*/, rT0/*Result*/, 1/*Tag*/, VirtualHwBaseAddr );
       
       /* 3.8. Calculate: Res = Res+rT0, Res=Res+1, Res = N - Res; */       
       LLF_PKI_PKA_Add( LenID, Res, rT0, Res, 1/*Tag*/, VirtualHwBaseAddr );

       /* 3.9. If remainder rT2 is not 0, then add 1 to rT0 result */
       LLF_PKI_PKA_CompareIm( LenID, rT2/*OpA*/, 0/*OpB*/, 1/*Tag*/, VirtualHwBaseAddr );
       LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status); 
       if( status != 1 ) 
       		LLF_PKI_PKA_AddIm( LenID, Res, 1 ,Res, 0/*Tag*/, VirtualHwBaseAddr );
       /* 3.10. Res = N - Res; */ 
       LLF_PKI_PKA_Sub( LenID, N, Res, Res, 1/*Tag*/, VirtualHwBaseAddr );   
   }
   
   /* End of function */
   
   return Error;  
         
 } /* END OF function LLF_PKI_PKA_ExecFullModInv */
#endif /*_INTERNAL_CRYS_NO_RSA_KG_SUPPORT*/



#ifndef _INTERNAL_CRYS_BASE_PKI_IN_ROM
/***********      LLF_PKI_PKA_CopyDataFromPkaReg      **********************/
/**
 * @brief This function copies data from PKA register into output buffer . 
 *
 *        Assumings: - PKA is initialized.
 *                   - Length of extended (by word) registers is placed into LenID entry of 
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
 void LLF_PKI_PKA_CopyDataFromPkaReg( 
                                DxUint32_t *dst_ptr,           /*out*/
                                DxUint32_t  sizeWords,         /*in*/
                                DxInt8_t    srcReg,            /*in*/
                                DxUint32_t  VirtualHwBaseAddr  /*in*/ )
 {
   
	/* LOCAL DECLARATIONS */
	
	/* current register address and size */
	DxUint32_t  currAddr;
	
	
	/* FUNCTION LOGIC */
	
	/* write terminate opcode into PKA OPCODE register */
	DX_HAL_WriteCcRegister( DX_CC_REG_OFFSET (CRY_KERNEL,OPCODE) , 0 );
		  
	/* wait for PKA done bit */
	LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );
	
	/* get address of PKA register */
	LLF_PKI_PKA_ReadRegAddress(srcReg, currAddr/*PhysAddr*/, VirtualHwBaseAddr);
	
	/* copy data from PKA register into output buffer */ 
	LLF_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr, currAddr, dst_ptr, sizeWords );
	
 } /* END OF function LLF_PKI_PKA_CopyDataFromPkaReg */


 /***********      LLF_PKI_PKA_CopyDataIntoPkaReg function      **********************/
 /**
 * @brief This function  copies source data into PKA register . 
 *
 *        Assumings: - PKA is initialized.
 *                   - Length of extended (by word) registers is placed into LenID entry of 
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must 
 *                     set LenID according to extended register size
 *
 * @param[in] dstReg       - Virtual address (number) of destination register.
 * @param[in] LenId        - ID of entry of regsSizesTable defines registers length with word extension.
 * @param[in] src_ptr      - Pointer to source buffer.
 * @param[in] sizeWords    - Data size in words.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void LLF_PKI_PKA_CopyDataIntoPkaReg( 
	 DxInt8_t    dstReg,            /*out*/
	 DxUint8_t   LenID,             /*in*/
	 DxUint32_t  *src_ptr,           /*in*/
	 DxUint32_t  sizeWords,         /*in*/
	 DxUint32_t  VirtualHwBaseAddr  /*in*/ )
 {

	 /* LOCAL DECLARATIONS */


	 /* current register address and size */
	 DxUint32_t  currAddr;
	 DxUint32_t  regSize;


	 /* FUNCTION LOGIC */

	 LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );

	 /* get address of PKA register */
	 LLF_PKI_PKA_ReadRegAddress(dstReg, currAddr/*PhysAddr*/, VirtualHwBaseAddr);

	 /* calculate register size in words */
	 LLF_PKI_PKA_ReadRegSize(regSize, LenID, VirtualHwBaseAddr);
	 regSize = (regSize + 31) / 32; 

	 /* copy data from PKA register into output buffer */ 
         LLF_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr, currAddr, src_ptr, sizeWords );
	 currAddr = currAddr + sizeof(DxUint32_t)*sizeWords;

	 /* zeroe not significant high bytes of the register */
	 LLF_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr, currAddr, regSize - sizeWords);

#ifdef LLF_PKI_PKA_DEBUG 
	 LLF_PKI_PKA_Copy( LenID/*LenID*/, dstReg, dstReg, 0/*Tag*/, VirtualHwBaseAddr );
#endif

 } /* END OF function LLF_PKI_PKA_CopyDataIntoPkaReg */


 /***********      LLF_PKI_PKA_ClearBlockOfRegs function      **********************/
 /**
 * @brief This function clears block of PKA registers. 
 *
 *        Assumings: - PKA is initialized.
 *                   - Length of extended (by word) registers is placed into LenID entry of 
 *                     sizes table.
 *
 * @param[in] firstReg    - Virtual address (number) of first register in block.
 * @param[in] countOfRegs - Count of registers to clear.
 * @param[in] LenId       - ID of entry of regsSizesTable defines registers length with word extension.
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
	 DxUint32_t VirtualHwBaseAddr  /*in*/ )
 {

	 /* LOCAL DECLARATIONS */

	 DxUint32_t i;
	 DxUint32_t  size, addr, countTemps;


	 /* FUNCTION LOGIC */

	 /* get registers size in bits */
	 LLF_PKI_PKA_ReadRegSize( size, LenID, VirtualHwBaseAddr);
    
#ifdef LLF_PKI_PKA_DEBUG    
	 /* check memory size and registers count */
	 if( (firstReg + countOfRegs)*size > 8*LLF_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES ||
		 countOfRegs > 31 )

		 return LLF_PKI_PKA_NOT_ENOUGH_MEMORY_ERROR;
#endif 

	 countTemps = 0;

	 if( countOfRegs > 29 )
	 {
		 countOfRegs = 29;
		 countTemps = countOfRegs - 29;
	 }


	 /* clear registers */
	 for( i = 0; i < countOfRegs; i++ )
	 {
		 LLF_PKI_PKA_Clr(LenID, firstReg + i/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );
	 }

	 /* clear PKA temp registers using macros (without PKA operations */
	 if( countTemps > 0 )
	 {
		 /* calculate size of register in words */
		 size = (size + 31) / 32;
		 LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );
		 LLF_PKI_PKA_GetRegAddress(30/*VirtReg*/,addr/*physAddr*/, VirtualHwBaseAddr); 
		 LLF_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr, addr, size );
		 if( countTemps > 1 )
		 {
			 LLF_PKI_PKA_GetRegAddress(31/*VirtReg*/,addr/*physAddr*/, VirtualHwBaseAddr); 
			 LLF_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr, addr, size );   	
		 }
	 }

	 return CRYS_OK;

 } /* END OF function LLF_PKI_PKA_ClearBlockOfRegs */

#endif //_INTERNAL_CRYS_BASE_PKI_IN_ROM

 
#ifndef _INTERNAL_CRYS_NO_RSA_KG_SUPPORT
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
                                        DxUint32_t VirtualHwBaseAddr )
 {
   
   /* LOCAL DECLARATIONS */
   
   /* loop variable */
   DxUint32_t j;
   
   /* number shifts by word and by bit */
   DxUint32_t numWords, numBits;
   
   /* PKA status */
   DxUint32_t status;
   
   
   /* FUNCTION LOGIC */
   
   /* INITIALIZATIONS */
   
   /* copy extended rX=>rT */
   if( rX != rT )
	   LLF_PKI_PKA_Copy( LenID+1, rT/*dst*/, rX/*src*/, 0/*Tag*/, VirtualHwBaseAddr );

   /* number shifts */
   numWords = i / 32;
   numBits =  i % 32;  
    	
   /* shift by words */
   for( j = 0; j < numWords; j++ )
   {
	    LLF_PKI_PKA_SHR0( LenID, rT/*N*/, 32-1/*S*/ , rT/*Result*/, 0/*Tag*/, VirtualHwBaseAddr );
   }
   
   /* shift by bits */
   if( numBits >= 1 )
	   LLF_PKI_PKA_SHR0( LenID, rT/*N*/, numBits-1/*S*/ , rT/*Result*/, 0/*Tag*/, VirtualHwBaseAddr );
   
   /* test LS Bit */
   LLF_PKI_PKA_Tst0( LenID, rT/**/, 0/*Tag*/, VirtualHwBaseAddr );   

   LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
   
   if( status == 1 )
   	return 0;
   else
        return 1;		
    
    
 } /* END OF function LLF_PKI_PKA_GetBitFromPkaReg */
 

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
                            DxUint32_t  VirtualHwBaseAddr )
 {
   
     /* DECLARATIONS */
     
     /* PKA status */
     DxUint32_t status;
     
   
     /* FUNCTION LOGIC */
     Tag = Tag;
     if( rX != rRes ){
   	  LLF_PKI_PKA_Copy( LenID+1, rRes/*dst*/, rX/*src*/, 0/*Tag*/, VirtualHwBaseAddr );
     }
     
     /* if the vector rX is odd, then add the modulus and then  divide by 2 */ 
         
     LLF_PKI_PKA_Tst0(LenID+1, rRes/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );

     LLF_PKI_PKA_GET_StatusAluOutZero(VirtualHwBaseAddr, status);
	   
     if( status != 1 ){
     	LLF_PKI_PKA_Add( LenID+1, rRes/*P*/, rN/*OpB=N=0*/, rRes/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );
     }
     
     /* divide by 2 */ 	  
     LLF_PKI_PKA_SHR0( LenID+1, rRes/*P*/, 1-1/*S*/, rRes/*Res*/, 0/*Tag*/, VirtualHwBaseAddr );   
    
    
 } /* END OF function LLF_PKI_PKA_ModDivideBy2 */
 

/***********        LLF_PKI_PKA_BigShift            **********************/
/**
 * @brief The function shifts the operand A in PKA register by S bits according to operation code. 
 *
 *        Legal operation codes are: 
 *                          - SHR0 (shift right and inserting left 0-s). 
 *                          - SHR1 (shift right and inserting left 1-s).
 *                          - SHL0 (shift left and inserting right 0-s). 
 *                          - SHL1 (shift left and inserting right 1-s).
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
                            DxUint32_t      VirtualHwBaseAddr /*in*/  ) 
 {
   
     /* DECLARATIONS */
     
     CRYSError_t Error = CRYS_OK;
     
     /* shift countss */
     DxUint32_t S1, numWords, numBits;
     
     DxUint32_t i;
     
   
     /* FUNCTION LOGIC */
     
     /* check Opcode */
     if( Opcode < PKA_SHR0 || Opcode > PKA_SHL1 )
         return  LLF_PKI_PKA_ILLEGAL_OPCODE_ERROR;
   
     /* set shift counts */     
	 S1  = S; /* this one instead casting: for preventing compiler error */
	 numBits  = S1 % 32;  
     numWords = (S1 - numBits) / 32;
      
     /* shift by bits */     
     if( numBits != 0 ){
         _LLF_PKI_PKA_ExecOperation( Opcode, LenID, 0/*IsAImmed*/, OpA, 
                                     0/*IsBImmed*/, (DxInt8_t)numBits - 1 /*OpB*/, 
                                     0/*ResDiscard*/, Res, 0, VirtualHwBaseAddr);
     }

     /* shift by words */
     for( i = 0; i < numWords; i++ )
     {
         _LLF_PKI_PKA_ExecOperation( Opcode, LenID, 0/*IsAImmed*/, OpA, 
                                     0/*IsBImmed*/, 32 - 1 /*OpB*/, 
                                     0/*ResDiscard*/, Res, Tag, VirtualHwBaseAddr);
     }
     
     return Error;
    
 } /* END OF function LLF_PKI_PKA_BigShift */


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
                            DxUint32_t      VirtualHwBaseAddr /*in*/  ) 
 {
   
     /* DECLARATIONS */
     
     
     /* shift countss */
     DxUint32_t numWords, size = 1;
     
     DxInt32_t i;
     
     DxUint32_t  addr;
     
#ifdef CRYS_PKA_INDIRECT_SRAM_ACCESS_MODE
	 DxUint32_t  currWord = 0;
#endif          
     
     Tag = Tag;
	 /* FUNCTION LOGIC */
     
     LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
     
     /* read register address and full operation size in bits */
     LLF_PKI_PKA_ReadRegAddress( reg, addr, VirtualHwBaseAddr );
     LLF_PKI_PKA_ReadRegSize(size, LenID+1, VirtualHwBaseAddr);
     
     /* register size in words */
     size = (size + 31) / 32;
     
 #ifndef CRYS_PKA_INDIRECT_SRAM_ACCESS_MODE /* working with HOST RAM */
     
     return CRYS_COMMON_GetWordsCounterEffectiveSizeInBits(
		(DxUint32_t*)(addr + LLF_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR), (DxUint16_t)size);

 #else  /* working with SRAM */	 
  
     /* read words and find MSWord */
	 for( i = size-1 ; i >= 0  ; i-- ) 
	 { 
	      LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM(VirtualHwBaseAddr, addr + 4*i, currWord);
	      
	      if(currWord != 0)
	         break;	      
	 }
	 
	 numWords = i;
	 
	 /* find number of bits in the MS word */
	 for( i = 1; i <= 32; i++ )
	 {
	 	currWord = currWord >> 1;
	 	
	 	if( currWord == 0 )
	 	   break;
	 }
	 
	 
 #endif
   
     return 32*numWords + i;
    
 } /* END OF function LLF_PKI_PKA_GetRegEffectiveSizeInBits */
#endif /*_INTERNAL_CRYS_NO_RSA_KG_SUPPORT*/


#ifndef CRYS_NO_ECPKI_SUPPORT
/***********   LLF_PKI_PKA_GetWordFromRegister  **********************/
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
 * @return - word i from register rReg.
 *
 */
 DxUint32_t   LLF_PKI_PKA_GetWordFromRegister( 
                            DxUint8_t       LenID,            /*in*/
                            DxInt8_t        rReg,             /*in*/
                            DxUint32_t      i,                /*in*/
                            DxUint32_t      VirtualHwBaseAddr /*in*/  ) 
 {
   
     /* DECLARATIONS */
     
     
     DxUint32_t  currAddr;
     
     DxUint32_t  oneWord;
     
    
     LenID = LenID;
     /* FUNCTION LOGIC */
     
     LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
     
     /* read register address  */
     LLF_PKI_PKA_ReadRegAddress( rReg, currAddr, VirtualHwBaseAddr );
     
     /* read the word */
     LLF_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , currAddr + 4*i , oneWord );
     
     return  oneWord;

    
 } /* END OF function LLF_PKI_PKA_GetWordFromRegister */




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
                            DxUint32_t      VirtualHwBaseAddr /*in*/  ) 
 {
   
     /* DECLARATIONS */
     
     DxUint32_t  currAddr;
     
     
     LenID = LenID;
	 /* FUNCTION LOGIC */
     
     LLF_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );
     
     /* read register address  */
     LLF_PKI_PKA_ReadRegAddress( rReg, currAddr, VirtualHwBaseAddr );
     
     /* set word */
     LLF_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , currAddr + 4*i, word);
     
     
     return;

    
 } /* END OF function LLF_PKI_PKA_SetWordIntoRegister */  	 




/***********   LLF_PKI_PKA_SwapRegsPhisAddr  **********************/
/**
 * @brief This function swaps the physical addresses of PKA registers in RegsMapTable.
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
                            DxUint32_t      VirtualHwBaseAddr /*in*/  ) 
 {
     /* DECLARATIONS */
     
     DxUint32_t  addrA, addrB;
     
     
     /* FUNCTION LOGIC */
     
     LLF_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); 
     
     if( *r0 != RES_DISCARD && *r1 != RES_DISCARD )
     {
        /* swap physical addresses */
		LLF_PKI_PKA_GetRegAddress(*r0/*VirtReg*/,addrA/*physAddr*/, VirtualHwBaseAddr); 
		LLF_PKI_PKA_GetRegAddress(*r1/*VirtReg*/,addrB/*physAddr*/, VirtualHwBaseAddr); 
       
		LLF_PKI_PKA_SetRegAddress( *r0, addrB, VirtualHwBaseAddr);
		LLF_PKI_PKA_SetRegAddress( *r1, addrA, VirtualHwBaseAddr);
		/* swap virtual pointers */
		LLF_PKI_SwapInt8(*r0,*r1);
     }
     
     if( *r2 != RES_DISCARD && *r3 != RES_DISCARD )
     {
		LLF_PKI_PKA_GetRegAddress(*r2/*VirtReg*/,addrA/*physAddr*/, VirtualHwBaseAddr); 
		LLF_PKI_PKA_GetRegAddress(*r3/*VirtReg*/,addrB/*physAddr*/, VirtualHwBaseAddr); 
       
		LLF_PKI_PKA_SetRegAddress( *r2, addrB, VirtualHwBaseAddr);
		LLF_PKI_PKA_SetRegAddress( *r3, addrA, VirtualHwBaseAddr);
	
		LLF_PKI_SwapInt8(*r2,*r3);
     }
     
     return;

    
 } /* END OF function LLF_PKI_PKA_SwapRegsPhisAddr */
#endif /*CRYS_NO_ECPKI_SUPPORT*/
 

