/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: dopra_def.h                                                     */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description:                                                              */
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


#ifndef  _DOPRA_DEF_H
#define  _DOPRA_DEF_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/* To define VOS_DOPRA_VER */
#define VOS_DEBUG                      1
#define VOS_RELEASE                    2


/* To define VOS_OS_VER */
#define VOS_WIN32                      1
#define VOS_PSOS                       2
#define VOS_VXWORKS                    3
#define VOS_LINUX                      4
#define VOS_UNIX                       5    /* No use now */
#define VOS_SOLARIS                    6
#define VOS_TLINUX                     7
#define VOS_HPUNIX                     8
#define VOS_IBMUNIX                    9
#define VOS_RTT                        10
#define VOS_WINCE                      11
#define VOS_NUCLEUS                    12
#define VOS_RTOSCK                     13


/* To define VOS_CPU_TYPE */
#define VOS_X86                        0
#define VOS_PPC                        2
#define VOS_MIPS                       3
#define VOS_SPARC                      7
#define VOS_ARM                        8
#define VOS_PA8700                     9
#define VOS_ZSP                        19

#define VOS_68K                        1
#define VOS_750                        4
#define VOS_860                        5
#define VOS_8260                       6
#define VOS_8240                       10
#define VOS_8245                       11
#define VOS_MIPS32                     12
#define VOS_MIPS64                     13

#define OSA_CPU_CCPU                   0
#define OSA_CPU_ACPU                   1

/* To define VOS_BYTE_ORDER (the CPU byte order) */
#define VOS_LITTLE_ENDIAN              1234
#define VOS_BIG_ENDIAN                 4321

#define VOS_YES                        1
#define VOS_NO                         0

#define VOS_NOT_USED                   0x00
#define VOS_USED                       0x01

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _DOPRA_DEF_H */

