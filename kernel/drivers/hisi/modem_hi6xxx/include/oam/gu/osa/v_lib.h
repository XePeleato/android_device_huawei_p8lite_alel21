/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_lib.h                                                         */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement general function                                   */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/* 2. Date: 2006-10                                                          */
/*    Author: Xu Cheng                                                       */
/*    Modification: Standardize code                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _V_MINILIB_H
#define _V_MINILIB_H

#include "dopra_def.h"
#include "vos_config.h"
#include "v_typdef.h"
#include "string.h"

#if (VOS_VXWORKS== VOS_OS_VER)
#include "stdio.h"
#include "stdlib.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/* errno definiens */
#define VOS_ERRNO_LIB_64DIV32_QUOTIENT_NULL                 0x20290010
#define VOS_ERRNO_LIB_64DIV32_DIVIDE_ZERO                   0x20290011
#define VOS_ERRNO_LIB_64DIV64_QUOTIENT_NULL                 0x20290020
#define VOS_ERRNO_LIB_64DIV64_DIVIDE_ZERO                   0x20290021
#define VOS_ERRNO_LIB_PRINTF_INPUTFORMATTOOLONG             0x20290001
#define VOS_ERRNO_LIB_PRINTF_UNKNOWNINTERERROR              0x20290002

#define VOS_ERRNO_LIB_64MUL32_QUOTIENT_NULL                 0x20290025
#define VOS_ERRNO_LIB_64MUL32_OVERFLOW                      0x20290026

enum E_VOS_MEM_PT
{
    STATIC_DOPRA_MEM_PT,
    DYNAMIC_DOPRA_MEM_PT,
    STATIC_MEM_PT,
    DYNAMIC_MEM_PT,
    BLOCK_MEM_PT,
    MEM_PT_BUTT
};

VOS_BOOL   VOS_CharIsUpper( VOS_CHAR Char );

VOS_BOOL   VOS_CharIsLower( VOS_CHAR Char );

VOS_CHAR   VOS_CharToUpper( VOS_CHAR Char );

VOS_CHAR   VOS_CharToLower( VOS_CHAR Char );

VOS_VOID   VOS_StrToUpper( VOS_CHAR* Str );

VOS_VOID   VOS_StrToLower( VOS_CHAR* Str );

VOS_INT8 VOS_StrNiCmp( VOS_CHAR * Str1, VOS_CHAR * Str2, VOS_SIZE_T Length );

VOS_CHAR * VOS_StrCpy( VOS_CHAR * Dest, VOS_CHAR * Src );

VOS_CHAR * VOS_StrNCpy( VOS_CHAR * Dest, VOS_CHAR * Src, VOS_SIZE_T Count );

VOS_INT   VOS_StrCmp( VOS_CHAR * Str1, VOS_CHAR * Str2 );

VOS_UINT32 VOS_StrLen( VOS_CHAR * Str );

VOS_CHAR * VOS_StrStr( VOS_CHAR * Str1, VOS_CHAR * Str2 );

VOS_CHAR* VOS_StrRChr( VOS_CHAR * Str, VOS_CHAR Char );

VOS_UINT32 VOS_StrNLen( VOS_CHAR * Str, VOS_UINT32 Count );

/*lint -function(memcmp,VOS_MemCmp)*/
VOS_INT VOS_MemCmp( const VOS_VOID * Dest, const VOS_VOID * Src, VOS_SIZE_T Count );

/*lint -function(memset,V_MemSet)*/
VOS_VOID * V_MemSet( VOS_VOID * ToSet, VOS_CHAR Char, VOS_SIZE_T Count,
                     VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_MemSet( ToSet, Char, Count )\
    V_MemSet( (ToSet), (Char), (Count), VOS_FILE_ID, __LINE__ )

/*lint -function(memcpy,V_MemCpy)*/
VOS_VOID * V_MemCpy( VOS_VOID * Dest, const VOS_VOID * Src, VOS_SIZE_T Count,
                       VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_MemCpy( Dest, Src, Count )\
    V_MemCpy( (Dest), (Src), (Count), VOS_FILE_ID, __LINE__ )

/*lint -function(memmove,V_MemMove)*/
VOS_VOID * V_MemMove( VOS_VOID * Dest, const VOS_VOID * Src, VOS_SIZE_T Count,
                        VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_MemMove( Dest, Src, Count )\
    V_MemMove( (Dest), (Src), (Count), VOS_FILE_ID, __LINE__ )

/*lint -function(memchr,VOS_MemChr)*/
VOS_VOID * VOS_MemChr( const VOS_VOID *buf, VOS_UCHAR c, VOS_INT n );

VOS_CHAR * VOS_StrTrim( VOS_CHAR *pcStr );

VOS_UINT32 VOS_64Div32( VOS_UINT32 ulDividendHigh,
                        VOS_UINT32 ulDividendLow,
                        VOS_UINT32 ulDivisor,
                        VOS_UINT32 *pulQuotientHigh,
                        VOS_UINT32 *pulQuotientLow,
                        VOS_UINT32 *pulRemainder );

VOS_UINT32 V_SetErrorNo( VOS_UINT32 ulErrorNo, 
                         VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_SetErrorNo( ulErrorNo )\
    V_SetErrorNo( (ulErrorNo), VOS_FILE_ID, __LINE__ )

VOS_UINT32 VOS_GetErrorNo(VOS_VOID);

void add64(   VOS_UINT32 *low,
              VOS_UINT32 *high,
              VOS_UINT32 oldlow,
              VOS_UINT32 oldhigh );


VOS_UINT32 VOS_64Multi32( VOS_UINT32 ulMultiplicandHigh,
                          VOS_UINT32 ulMultiplicandLow,
                          VOS_UINT32 ulMultiplicator,
                          VOS_UINT32 *pulProductHigh,
                          VOS_UINT32 *pulProductLow );

VOS_UINT32 VOS_Rand( VOS_UINT32 ulRange );

VOS_VOID VOS_SetSeed( VOS_UINT32 ulSeed );

VOS_SIZE_T VOS_StrSpn(const VOS_CHAR *Str1, const VOS_CHAR *Str2);

VOS_CHAR *VOS_StrPbrk(const VOS_CHAR *Str1, const VOS_CHAR *Str2);

VOS_CHAR *VOS_StrTok(VOS_CHAR *Str1, const VOS_CHAR *Str2);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_MINILIB_H */



