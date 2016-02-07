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

 
#ifndef SYSTEM_DEP_H
#define SYSTEM_DEP_H

/*
 * Includes
 */
//#define DEBUG_HW 1

#if DX_TST_HARDWARE_REG_PRINT_MODE
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:09:43 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version PLAT_SystemDep.h#1:incl:1
   *  \author adams
   *  \remarks Copyright (C) 2004 by Discretix Technologies Ltd.
   *           All Rights reserved
   */

/************************ Defines ******************************/



/* .................... basic Macros for accessing hardware registers ...................... */
/* ----------------------------------------------------------------------------------------- */

#if DX_TST_HARDWARE_REG_PRINT_MODE

#define SYS_WriteRegister(addr,val) \
do \
{ \
  ( (*((volatile DxUint32_t*)(addr))) = (DxUint32_t)(val) ); \
  printf( "master_write(32'h%08lX , 32'h%08lX); \n",(addr),(val)); \
}while(0)   

#define SYS_ReadRegister(addr,val) \
do \
{ \
  ( (val) = (*((volatile DxUint32_t*)(addr))) ); \
  printf( "master_read(32'h%08lX , 32'h%08lX); \n",(addr),(DxUint32_t)(val)); \
}while(0)   

#else

#define SYS_WriteRegister(addr,val)  ( (*((volatile DxUint32_t*)(addr))) = (DxUint32_t)(val) )
#define SYS_ReadRegister(addr,val)  ( (val) = (*((volatile DxUint32_t*)(addr))) )

#endif

/* .................... Macros for hardware registers block utilities ...................... */
/* ----------------------------------------------------------------------------------------- */

#define SYS_WriteRegistersBlock( addr , buffer , size_in_words ) \
do \
{ \
  DxUint32_t i,k; \
  for ( i = 0 , k=(addr) ; i < (size_in_words) ; k+=sizeof(DxUint32_t) , i++ ) \
  { \
    SYS_WriteRegister( k , (buffer)[i] ); \
  } \
}while(0)       

#define SYS_ClearRegistersBlock( addr , size_in_words ) \
do \
{ \
  DxUint32_t i,k; \
  for ( i = 0 , k=(addr) ; i < (size_in_words) ; k+=sizeof(DxUint32_t) , i++ ) \
  { \
    SYS_WriteRegister( k , 0UL ); \
  } \
}while(0)       

#define SYS_ReadRegistersBlock( addr , buffer , size_in_words ) \
do \
{ \
  DxUint32_t i,k; \
  for ( i = 0 , k=(addr) ; i < (size_in_words) ; k+=sizeof(DxUint32_t) , i++ ) \
  { \
    SYS_ReadRegister( k , (buffer)[i] ); \
  } \
}while(0)       

/* defining a MACRO to load a block to the SRAM */
#ifdef BIG__ENDIAN
#define SYS_HW_READ_BLOCK_FROM_SRAM(HwBaseAddr, Addr , ptr , Size ) \
do \
{ \
   DxUint32_t ii; \
   DxUint32_t dummy; \
   SYS_WriteRegister( SB_REG_ADDR(HwBaseAddr, SRAM_ADDR) ,(Addr) ); \
   SYS_ReadRegister( SB_REG_ADDR(HwBaseAddr, SRAM_DATA), (dummy) ); \
   for( ii = 0 ; ii < (Size) ; ii++ ) \
   { \
      do \
      { \
         SYS_ReadRegister( SB_REG_ADDR(HwBaseAddr, SRAM_DATA_READY), (dummy) ); \
      }while(!dummy); \
      SYS_ReadRegister( SB_REG_ADDR(HwBaseAddr, SRAM_DATA), ptr[ii] ); \
      UTIL_ReverseBuff( (DxUint8_t*)&ptr[ii] , sizeof(DxUint32_t) ); \
   } \
}while(0)
#else
#define SYS_HW_READ_BLOCK_FROM_SRAM(HwBaseAddr, Addr , ptr , Size ) \
do \
{ \
   DxUint32_t ii; \
   DxUint32_t dummy; \
   SYS_WriteRegister( SB_REG_ADDR(HwBaseAddr, SRAM_ADDR) ,(Addr) ); \
   SYS_ReadRegister( SB_REG_ADDR(HwBaseAddr, SRAM_DATA), (dummy) ); \
   for( ii = 0 ; ii < (Size) ; ii++ ) \
   { \
      do \
      { \
         SYS_ReadRegister( SB_REG_ADDR(HwBaseAddr, SRAM_DATA_READY), (dummy) ); \
      }while(!dummy); \
      SYS_ReadRegister( SB_REG_ADDR(HwBaseAddr, SRAM_DATA), ptr[ii] ); \
   } \
}while(0)
#endif
	   
/* .................................. Debug MACRO ................................... */
/* ---------------------------------------------------------------------------------- */
#ifndef DX_ARM1176
#define LLF_COMMON_ARM_DebugMemCpy( Dst_ptr, Src_ptr, SizeBytes ) \
   \
	SYS_WriteRegister( 0xC0008010UL , 1UL ); \
	SYS_WriteRegister( 0xC0008014UL , 1UL ); \
  UTIL_MemCopy( (Dst_ptr) , (Src_ptr) , (SizeBytes) ); \
	SYS_WriteRegister( 0xC0008010UL , 0UL ); \
	SYS_WriteRegister( 0xC0008014UL , 0UL ); 
#else
#define LLF_COMMON_ARM_DebugMemCpy( Dst_ptr, Src_ptr, SizeBytes ) \
   \
	SYS_WriteRegister( 0x80008010UL , 1UL ); \
	SYS_WriteRegister( 0x80008014UL , 1UL ); \
  UTIL_MemCopy( (Dst_ptr) , (Src_ptr) , (SizeBytes) ); \
	SYS_WriteRegister( 0x80008010UL , 0UL ); \
	SYS_WriteRegister( 0x80008014UL , 0UL ); 

#endif

/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif





