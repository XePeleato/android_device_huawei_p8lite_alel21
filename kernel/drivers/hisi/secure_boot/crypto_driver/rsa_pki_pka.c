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

#define DX_PAL_LOG_CUR_COMPONENT HOST_LOG_MASK_SECURE_BOOT

#include "secureboot_basetypes.h"
#include "util.h"
#include "rsa_pki_pka.h"
#include "rsa.h"
#include "systemdep.h"


/************************ Defines ******************************/
void UTIL_MemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );
void UTIL_MemSet( DxUint8_t *buff_ptr , DxUint8_t val , DxUint32_t size );                    
/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions ******************************/
 
#ifdef PKA_DEBUG
  DxUint8_t tempRes[268];
#endif  

/* ***************************************************************************** */
/* *********      RSA PKI PKA initialisation functions and macros      ********* */
/* ***************************************************************************** */


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
                             DxUint32_t                VirtualHwBaseAddr /*in*/)
{
   /* LOCAL DECLARATIONS */
   
   /* maximal and minimal sizes stated in RegsSizes_ptr */
   DxUint32_t  maxSize, minSize, MaxSizeWords;
   
   /* loop variable */
   DxUint32_t  i;
 
  
   /* FUNCTION LOGIC */
   
   /* ....... initialize local variables ...... */
  
   
   maxSize = 0;
   minSize = PKA_SIZE_ENTRY_NOT_USED;
  
   
   /* check the input parameters */
 
   
   /* 1. Case of user defined settings */
   if( IsDefaultMap == 0 )  
   {
	      
	   /* find maximal and minimal sizes  */  
	   for( i = 0; i < CountOfSizes; i++ )
	   {       
	   	   if( maxSize < RegsSizes_ptr[i] && RegsSizes_ptr[i] != PKA_SIZE_ENTRY_NOT_USED )
	   	    
	   	      maxSize = RegsSizes_ptr[i];
	   	      
	   	   if( minSize > RegsSizes_ptr[i] )
	   	    
	   	      minSize = RegsSizes_ptr[i];
	   }
	      
	   /* set sizes into PKA registers sizes table */
	   for( i = 0; i < CountOfSizes; i++ )
	   {
		  SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr,PKA_L0), RegsSizes_ptr[i] );
	   }  
   }
   
   /* 2. Case of default settings */
   else  
   {
	  
	   MaxSizeWords = ( MaxSizeBits + 31 ) / 32;  
   
   	   /* write exact size into first table entry */
   	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr,PKA_L0), MaxSizeBits );
   	   
    	   /* write size with extra word into tab[1] = tab[0] + 32 */
   	   SYS_WriteRegister(SB_REG_ADDR(VirtualHwBaseAddr,PKA_L0) + 4, 
   	                                32*MaxSizeWords + 32 ); 

       /* count of entries, which was set */
       CountOfSizes = 2;
   }
   

} /* END of the finction  RSA_HW_PKI_PKA_SetRegsSizesTab */
                                     



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
                          DxUint32_t              VirtualHwBaseAddr   /*in*/ )
{
   /* LOCAL DECLARATIONS */

      
   /* current register physical address */
   DxUint32_t  CurrentAddr;
   
   /* default max size and count of registers */
   DxUint32_t  defaultMaxSize, defaultCountOfRegs ;

   
   /* loop variable */
   DxUint32_t  i;
 
  
   /* FUNCTION LOGIC */
   
   /* ....... initialize local variables ...... */
   
   CurrentAddr = RSA_HW_PKI_PKA_DATA_REGS_BASE_ADDR;
      
       
   /* Step 2. Default mapping mode */
   if( IsDefaultMap == 1 )
   {
       /* set default registers size and their count according to maximal allowed size
          of operands 2112 bit = 66 words. Note register must be by 1 word longer, than operand
          for some modular operations  */          
       defaultMaxSize =  32*MaxSizeWords;
       defaultCountOfRegs =  min((DxUint32_t)32, (DxUint32_t)(8*RSA_HW_PKI_PKA_MAX_REGS_MEM_SIZE_BYTES) / defaultMaxSize);
      
       /* set registers addresses (offsets) into the mapping table,
          size of each register = MaxRegsSizeWords */ 
          
       /* set addresses of N,NP and user requested registers (excluding 2 temp registers T0,T1) */        
   	   for( i = 0; i < 32-2; i++ )
   	   {
   	   	  if( i < defaultCountOfRegs - 2 )
   	   	  {
 			  SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*i, 
	   	   	                               CurrentAddr ); 
	          	CurrentAddr = CurrentAddr + defaultMaxSize/8;          
   	   	  }
   	   	  
   	   	  else
   	   	  {
   	   	      /* write designation, that PKI entry is not in use */ 
        	      SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*i, 
	   	   	                               PKA_ADDRESS_ENTRY_NOT_USED );
   	   	  }
   	   }
   	   
   	   /* set addresses of 2 temp registers: T0=30, T1=31 */
   	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*30, 
   	   	                            CurrentAddr );
   	   	                                
   	   CurrentAddr = CurrentAddr + defaultMaxSize/8;
   	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*31, 
   	   	                            CurrentAddr );
       /* output maximal count of allowed registers */
       *CountOfRegs  =  defaultCountOfRegs;	  
       
	   /* set default virtual addresses of N,NP,T0,T1 registers into N_NP_T0_T1_Reg */	   
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, N_NP_T0_T1_ADDR), 
	   	                            (DxUint32_t)RSA_HW_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL ); 
   }
  
   /* Step 3. User defined mapping mode */
   if( IsDefaultMap == 0 )
   {
    
       /* set registers addresses (offsets), including N,NP,T0,T1 , into the mapping table;
          Size of each register = MaxRegsSizeWords */           
   	   for( i = 0; i < *CountOfRegs; i++ )
   	   {
		  SYS_WriteRegister( 
   	   	          SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*RegsMap_ptr->RegsNum[i], 
   	   	          RegsMap_ptr->RegsAddr[i] ); 
   	   } 
   	   
	   /* set virtual addresses of N,NP,T0,T1 registers into N_NP_T0_T1_Reg */
	   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, N_NP_T0_T1_ADDR), N_NP_T0_T1 ); 
   }
  
   
}  /* END of the finction  RSA_HW_PKI_PKA_SetRegsMapTab */



 

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
                 DxUint32_t               VirtualHwBaseAddr   /*in*/ )
 {
  
   /* LOCAL DECLARATIONS */
   
   DxUint32_t  addr;
   
   
   /* FUNCTION LOGIC */
     
   /*     enabling the PKA clocks      */   
   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, PKA_CLK_ENABLE), 0x1UL );
   
   /* set PKI offset to upper 4K */
   /* (for DX debug to use SEP SRAM upper 4K ) */
   /*SYS_WriteRegister(0xC00000cc, 0x1UL);*/
  		
 
   /* setting the PKA registers sizes table   */
   /*-----------------------------------------*/   	
   RSA_HW_PKI_PKA_SetRegsSizesTab(
                                       RegsSizes_ptr,     /*in*/   
                                       CountOfSizes,      /*in*/
                                       OpSizeBits,        /*in*/
                                       IsDefaultMap,      /*in*/ 
                                       VirtualHwBaseAddr  /*in*/ );
   
       
   /* setting the PKA registers mapping table */
   /*-----------------------------------------*/   
   RSA_HW_PKI_PKA_SetRegsMapTab(
                                     RegsMap_ptr,     	 /*in*/   
                                     &CountOfRegs,       /*in/out*/
                                     RegSizeWords,       /*in*/ 
                                     N_NP_T0_T1,         /*in*/
                                     IsDefaultMap,       /*in*/
                                     VirtualHwBaseAddr   /*in*/ );
   
  
   /* set size of register into RegsSizesTable */ 
   SYS_WriteRegister(SB_REG_ADDR(VirtualHwBaseAddr,PKA_L0) + 3*4, 
                                32*RegSizeWords ); 
  
   /* clean PKA data memory */
   RSA_HW_PKI_PKA_ClearBlockOfRegs( 0/* FirstReg*/, CountOfRegs-2, 
                                     3/*LenID*/, VirtualHwBaseAddr );	                      

   /* clean temp PKA registers 30,31 */ 
   RSA_HW_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );	
   addr = RSA_HW_PKI_PKA_GetRegAddress(30/*VirtReg*/, VirtualHwBaseAddr); 
   RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , addr , RegSizeWords );
   addr = RSA_HW_PKI_PKA_GetRegAddress(31/*VirtReg*/, VirtualHwBaseAddr); 
   RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , addr , RegSizeWords );                     
                     

   /* ......  End of function ...... */  

 }
 
 
/***********      RSA_HW_PKI_PKA_FinishPKA function      **********************/
/**
 * @brief This function ends the PKA engine session and disables PKA clocks. 
 *
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_FinishPKA( DxUint32_t VirtualHwBaseAddr /*in*/ )
 {
  
   /* FUNCTION LOGIC */
   
   /*  terminate PKA operations (Tag is not needed) */
   RSA_HW_PKI_PKA_Terminate(0/*Tag*/, VirtualHwBaseAddr);
   
   /* close PKA clocks */
   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, PKA_CLK_ENABLE), 0x0UL );
    
 }
 

 						    
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
                                DxUint32_t      VirtualHwBaseAddr  /*in*/ )
 {
 	 
   /* LOCAL DECLARATIONS */
 
   /* full Operation Code word */
   DxUint32_t FullOpCode;
  
 
   /* FUNCTION LOGIC */
   
   /* if Res == RES_DISCARD , then result is discarded */
   if( Res == (DxInt8_t)RES_DISCARD )
   {
   		ResDiscard = 1;
   		Res = 0; 
   }
 
  
   /*************************************************/
   /*      main PKI operation of this function      */
   /*************************************************/
   
   /* Create full opcode word  */
   FullOpCode = RSA_HW_PKI_PKA_FullOpCode( Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag );
                  
   /* write full opcode into PKA OPCODE register */   
   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, OPCODE) , FullOpCode );
   
  #ifdef PKA_DEBUG
  {
      DxUint8_t  sizeBits;
      RSA_HW_PKI_PKA_ReadRegSize(sizeBits, LenID/*EntryNum*/, VirtualHwBaseAddr);
      RSA_HW_PKI_PKA_CopyDataFromPkaReg( tempRes/*out*/, sizeBits/8, Res, VirtualHwBaseAddr );
 }
  
  #endif
   /*************************************************/  
   /* finishing operations for different cases      */
   /*************************************************/
   
   switch( Opcode )
   {
		   
     case PKA_Terminate:
     
       /* wait for PKA done bit */
       RSA_HW_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );

       break;
       
     default:
     
       /* wait for PKA pipe ready bit */
       RSA_HW_PKI_WAIT_ON_PKA_PIPE_READY( VirtualHwBaseAddr );
   }
   
 } /* END OF function _RSA_HW_PKI_PKA_ExecOperation */
 


/***********   RSA_HW_PKI_PKA_DivLongNum function      **********************/
/*!
 * 
 * \author reuvenl (7/28/2011)
 *
 * \brief The function divides long number A*(2^S) by B: 
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
 * \param  LenID    - ID of operation size (modSize+32). 
 * \param  OpA      - Operand A: virtual register pointer of A . 
 * \param  S        - exponent of binary factor of A.
 * \param  OpB      - Operand B: virtual register pointer of B . 
 * \param  Res      - Virtual register pointer for result quotient. 
 * \param  rT1      - Virtual pointer to remainder.
 * \param  rT2      - Virtual pointer of temp register.
 * \param  VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 * 
 */
void RSA_HW_PKI_PKA_DivLongNum( 
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
   
   /* initializations */
   
   /* calculate shifting parameters (words and bits ) */
   nWords = ((DxUint32_t)S + 31) / 32;
   nBits  = (DxUint32_t)S % 32;
      
   /* copy operand OpA (including extra word) into temp reg rT1 */ 
   RSA_HW_PKI_PKA_Copy( LenID+1, rT1/*dst*/, OpA/*src*/, 0 /*Tag*/, VirtualHwBaseAddr );
   
   /* set Res = 0 (including extra word) */
   RSA_HW_PKI_PKA_Clr( LenID+1, Res/*dst*/, 0 /*Tag*/, VirtualHwBaseAddr );   
   
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
   	    if( s1 > 0 )
   	    {
 	  	    RSA_HW_PKI_PKA_SHL0( LenID+1, rT1/*OpA*/, (s1-1)/*S*/, rT1 /*Res*/, 0 /*Tag*/, VirtualHwBaseAddr );    	    	
   	    }
   	       
   		/* 3.3. perform PKA_Div for calculating a quotient rT2 = floor(rT1 / N) 
   		        and remainder rT1 = rT1 % OpB  */
   	    RSA_HW_PKI_PKA_Div( LenID+1, rT1 /*OpA*/, OpB /*B*/, rT2/*Res*/, 0 /*Tag*/, VirtualHwBaseAddr );
   	
         /* 3.4. Res = Res * 2**s1 + Res;   */
        if( s1 > 0 )
        {
        	RSA_HW_PKI_PKA_SHL0( LenID+1, Res /*OpA*/, (s1-1)/*S*/, Res /*Res*/, 0 /*Tag*/, VirtualHwBaseAddr ); 	
        }
        
        RSA_HW_PKI_PKA_Add( LenID+1, Res /*OpA*/, rT2 /*OpB*/, Res /*Res*/, 0 /*Tag*/, VirtualHwBaseAddr );
   }
   
   RSA_HW_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );
    
   return;
    
}  /* END OF RSA_HW_PKI_PKA_DivLongNum */

 
/*******   RSA_HW_PKI_PKA_ExecCalcNpAndInitModOp function (virtual pointers)   **********/
/*!
 * 
 * \author reuvenl (7/28/2011)
 * 
 * \brief The function calculates the Barret modulus tag NP, used in   
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
 * \param LenID : The RegsSizesTable entry, containing the modSizeBits.
 * \param modSizeBits : exact modulus size in bits.
 * \param rT0 : virtual pointers to temp registers.
 * \param rT1 : - " -
 * \param rT2 : - " -
 * \param VirtualHwBaseAddr : Virtual HW base address, passed by user.
 */
void RSA_HW_PKI_PKA_ExecCalcNpAndInitModOp( 
                                         DxUint32_t  LenID,
                                         DxUint32_t  modSizeBits, 
                                         DxInt8_t    rT0, 
                                         DxInt8_t    rT1, 
                                         DxInt8_t    rT2, 
                                         DxUint32_t  VirtualHwBaseAddr )
{
  
   /* LOCAL DECLARATIONS */

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
   RSA_HW_PKI_PKA_Clr( LenID+1, rT0/*OpA*/, 0/*Tag*/,VirtualHwBaseAddr); /* r2 = 0 */
  
   /* calculate bit position of said bit in the word */
   numBits = modSizeBits % 32;
   numWords = modSizeBits / 32;
   
   /* set 1 into register rT0 */
   RSA_HW_PKI_PKA_Set0( LenID+1, rT0/*OpA*/, rT0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
   
   /* shift 1 to numBits+31 position */
   if(numBits > 0)
   {
	   RSA_HW_PKI_PKA_SHL0( LenID+1, rT0/*OpA*/, numBits-1/*S*/, rT0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
   }
 
   /* shift to word position */
   for( i = 0; i < numWords; i++ )
   {
	   RSA_HW_PKI_PKA_SHL0( LenID+1, rT0/*OpA*/, 31/*S*/, rT0/*Res*/, 0/*Tag*/, VirtualHwBaseAddr);
   }
   
      
   /*--------------------------------------------------------------------*/
   /* Step 3.  Dividing:  (OpA * 2**S) / N                               */
   /*--------------------------------------------------------------------*/
   RSA_HW_PKI_PKA_DivLongNum( 
                        LenID,        /*LenID*/
                        rT0,          /*opA*/
                        S,            /*shift*/
                        0,            /*opB = N*/
                        1,            /*Res NP*/
                        rT1,          /*temp reg*/
                        rT2,          /*temp reg*/  
                        VirtualHwBaseAddr ); 
   
   /* Note: PKA done is not needed, it was performed in previous called function*/
    
    return;
    
}  /* END OF RSA_HW_PKI_PKA_ExecCalcNpAndInitModOp */



/******    RSA_HW_PKI_PKA_CalcNp function (physical pointers)     ***************/
/*!
 * 
 * \author reuvenl (7/28/2011)
 * 
 * \brief The function initializes  modulus and Barret tag NP, used in modular 
 *        PKA operations. 
 *
 *        The function does the following:
 *          - calculates mod size in bits and sets it into PKA table sizes;
 *          - calls the RSA_HW_PKI_PKA_ExecCalcNpAndInitModOp, which calculates
 *            the Barret tag NP.
 *          - initializes PKA registers and outputs calculated NP value.
 *
 *        Assumings: - The registers mapping table is set on default mode,
 *                     according to modulus size:
 *                      -- count of allowed registers is not less, than 7
 *                         (including 3 registers rT0,rT2,rT3 for internal
 *                         calculations and 4 default special registers
 *                         N,NP,T0,T1);modulus exact and exact+32 bit sizes
 *                         should be set into first two entries of sizes-table
 *                         accordingly.
 *
 * \param N_ptr : The pointer to the buffer, containing modulus N as little 
 *        endian words array - LSWord is left most one. 
 * \param N_sizeBits : The size of modulus in bytes, must be 
 *        16 <= N_sizeBytes <= 264.
 * \param NP_ptr : The pointer to the buffer, containing result - Barrett tag NP 
 *        given as little endian words array - LSWord is left most one.
 * \param VirtualHwBaseAddr : Virtual HW base address, passed by user.
 */
void RSA_HW_PKI_PKA_CalcNp(
                          DxUint32_t        *N_ptr,	        /*in*/ 
                          DxUint32_t         N_sizeBits,        /*in*/ 
                          DxUint32_t        *NP_ptr,            /*out*/
                          DxUint32_t         VirtualHwBaseAddr  /*in*/ )
{
 
   /* LOCAL DECLARATIONS */

   /* exact size of modulus in bytes and in words  */
   DxUint32_t  N_sizeBytes;
   
   /* temp PKA registers used */
   DxInt8_t rT0 = 2, rT1 = 3, rT2 = 4;
 
   /* FUNCTION LOGIC */
   
   /* calculate size of modulus in bytes and in words */
   N_sizeBytes = (N_sizeBits + 7) / 8;
   
   /* initialize the PKA engine on default mode */
   RSA_HW_PKI_PKA_DefaultInitPKA(  N_sizeBits, RSA_HW_PKI_PKA_MAX_REGISTER_SIZE_WORDS, VirtualHwBaseAddr );
                               
   /* copy modulus N into r0 register */
   RSA_HW_PKI_PKA_CopyDataIntoPkaReg( 0/*dstReg*/, 1/*LenID*/, 
                                   N_ptr/*src_ptr*/, N_sizeBytes, VirtualHwBaseAddr ); 

   /*---------------------------------------------------------------*/
   /*       execute calculation of NP and initialization of PKA     */ 
   /*---------------------------------------------------------------*/
   
   RSA_HW_PKI_PKA_ExecCalcNpAndInitModOp(
                                 0/*LenID*/, 
                                 N_sizeBits,
                                 rT0,rT1,rT2, 
                                 VirtualHwBaseAddr );
   
   
   /* output of NP value */
   RSA_HW_PKI_PKA_CopyDataFromPkaReg( 
                  NP_ptr/*dst_ptr*/, 4*RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS, 
                  1/*srcReg*/, VirtualHwBaseAddr );
                      
   /* Finish PKA operations (waiting PKI done and close PKA clocks) */
   RSA_HW_PKI_PKA_FinishPKA( VirtualHwBaseAddr );
   
   /* End of the function */
   return;
                          
} /* END OF RSA_HW_PKI_PKA_CalcNp */

 
 

/*******************************************************************************/
/************    RSA PKI PKA mathmatic functions and macros     ****************/
/*******************************************************************************/

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
                                DxUint32_t VirtualHwBaseAddr  /*in*/ )
 {
   
   /* LOCAL DECLARATIONS */
   
   DxUint32_t i;
   DxUint32_t  size, addr, countTemps;
   
   
   /* FUNCTION LOGIC */
   


   /* get registers size in bits */
   RSA_HW_PKI_PKA_ReadRegSize( size, LenID, VirtualHwBaseAddr);
  		
   countTemps = 0;

   if( countOfRegs > 29 )
   {
   	   countOfRegs = 29;
   	   countTemps = countOfRegs - 29;
   }

   
   /* clear registers */
   for( i = 0; i < countOfRegs; i++ )
   {
	   	RSA_HW_PKI_PKA_Clr(LenID, firstReg + i/*regNum*/, 0/*Tag*/, VirtualHwBaseAddr );
   }
   
   /* clear PKA temp registers using macros (without PKA operations */
   if( countTemps > 0 )
   {
       /* calculate size of register in words */
       size = (size + 31) / 32;
	   RSA_HW_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );
	   addr = RSA_HW_PKI_PKA_GetRegAddress(30/*firsttempReg*/, VirtualHwBaseAddr);
	   RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr, addr, size );
	   if( countTemps > 1 )
	   {
		   addr = RSA_HW_PKI_PKA_GetRegAddress(31/*secondtempReg*/, VirtualHwBaseAddr);
		   RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr, addr, size );   	
	   }
   }

    
 } /* END OF function RSA_HW_PKI_PKA_ClearBlockOfRegs */
 
 
 
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
                                DxUint32_t  VirtualHwBaseAddr  /*in*/ )
 {
   
   /* LOCAL DECLARATIONS */
   
   
   /* current register address and size */
   DxUint32_t  currAddr;
   DxUint32_t  sizeWords, regSize;
   
   DxUint32_t  oneWord;
   
   DxInt32_t   i;
   
 
   /* FUNCTION LOGIC */
   
   /* data size in words */
   sizeWords = ( sizeBytes + 3) / 4;
     
   RSA_HW_PKI_PKA_Terminate( 0/*Tag*/, VirtualHwBaseAddr );

   /* get address of PKA register */
   RSA_HW_PKI_PKA_ReadRegAddress(dstReg, currAddr/*PhysAddr*/, VirtualHwBaseAddr);
   
   /* calculate register size in words */
   RSA_HW_PKI_PKA_ReadRegSize(regSize, LenID, VirtualHwBaseAddr);
   regSize = (regSize + 31) / 32; 
   
   /* if src data is aligned to words */
   if( ((DxUint32_t)src_ptr & 3) == 0 && (sizeBytes & 3) == 0 )
   {
       /* copy data from PKA register into output buffer */ 
	   RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr, currAddr, (DxUint32_t*)src_ptr, sizeWords );
	   currAddr = currAddr + 4*sizeWords;
   }
    
   else /* for not aligned data */
   {
   	   /* copy full words */
   	   for( i = 0; i < sizeBytes/4; i++ ) 
   	   {
   	   	   oneWord =   (DxUint32_t)src_ptr[0]  | 
   	   	              ((DxUint32_t)src_ptr[1] << 8)  | 
   	   	              ((DxUint32_t)src_ptr[2] << 16) | 
   	   	              ((DxUint32_t)src_ptr[3] << 24 );
   	   	              
   	   	   RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr, currAddr, oneWord );
           
   	   	   currAddr += 4;
   	   	   src_ptr  += 4;   
   	   }
   	   /* copy last word if it exists */
   	   if( 4*sizeWords - sizeBytes != 0 )
   	   {
   	       oneWord = 0;
   	   	   UTIL_MemCopy( (DxUint8_t*)&oneWord,  (DxUint8_t*)src_ptr, sizeBytes % 4 );   	   	
   	       RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr, currAddr, oneWord );
   	       currAddr += 4;
   	   }
   } 
   
   /* zeroe not significant high bytes of the register */
   RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr, currAddr, regSize - sizeWords);
   

 } /* END OF function RSA_HW_PKI_PKA_CopyDataIntoPkaReg */
 


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
                                DxUint32_t  sizeBytes,         /*in*/
                                DxInt8_t    srcReg,            /*in*/
                                DxUint32_t  VirtualHwBaseAddr  /*in*/ )
 {
   
   /* LOCAL DECLARATIONS */
   
   /* current register address and size */
   DxUint32_t  currAddr;
   DxUint32_t  sizeWords;
   
   DxUint32_t  oneWord;
   
   DxInt32_t   i;
   
 
   /* FUNCTION LOGIC */
   
   sizeWords = (sizeBytes + 3) / 4;
   
   /* write terminate opcode into PKA OPCODE register */   
   SYS_WriteRegister( SB_REG_ADDR(VirtualHwBaseAddr, OPCODE) , 0 );
                  
   /* wait for PKA done bit */
   RSA_HW_PKI_WAIT_ON_PKA_DONE( VirtualHwBaseAddr );
   
   /* get address of PKA register */
   RSA_HW_PKI_PKA_ReadRegAddress(srcReg, currAddr/*PhysAddr*/, VirtualHwBaseAddr);
     
   /* if dst data is aligned to words */
   if( ((DxUint32_t)dst_ptr & 3) == 0 && (sizeBytes & 3) == 0 )
   {
       /* copy data from PKA register into output buffer */ 
	   RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr, currAddr, (DxUint32_t*)dst_ptr, sizeWords );
   }
   else /* for not aligned data */
   {
   	   /* copy full words */
   	   for( i = 0; i < sizeBytes/4; i++ ) 
   	   {
   	   	   RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , currAddr , oneWord );

   	   	   *dst_ptr++ = (DxUint8_t)(oneWord & 0xFF);
   	   	   *dst_ptr++ = (DxUint8_t)((oneWord >>  8) & 0xFF);
   	   	   *dst_ptr++ = (DxUint8_t)((oneWord >> 16) & 0xFF);
   	   	   *dst_ptr++ = (DxUint8_t)((oneWord >> 24) & 0xFF);
   	   	   
   	   	   currAddr += 4;
   	   }
   	   /* copy last word if it exists */
   	   if( 4*sizeWords - sizeBytes != 0 )
   	   {
   	       RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , currAddr , oneWord );
   	   	   UTIL_MemCopy( (DxUint8_t*)dst_ptr, (DxUint8_t*)&oneWord, sizeBytes % 4 );   	   	
   	   }
   } 
 
 
 } /* END OF function RSA_HW_PKI_PKA_CopyDataFromPkaReg */

 void RSA_HW_PKI_SET_MSB_ADDR(DxUint32_t virtualAddress, DxUint32_t pkaAdrress)
 {
	 SYS_WriteRegister(SB_REG_ADDR(virtualAddress,PKA_CLK_ENABLE),0x1);
	 SYS_WriteRegister(SB_REG_ADDR(virtualAddress,PKA_MSB_ADDR),((DxUint32_t)pkaAdrress>> 12));
	 SYS_WriteRegister(SB_REG_ADDR(virtualAddress,PKA_CLK_ENABLE),0x0);
 }

