/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_type.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2005/4/23
Last Modified :
Description   : 系统共用的数据类型定义
Function List :
History       :
* main\1    2007-11-29      modify for Hi3560
******************************************************************************/
#ifndef __HI_TYPE_H__
#define __HI_TYPE_H__

#include <asm/io.h>


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

//#include <asm/io.h>
//#include <linux/delay.h>
//#include <mach/hardware.h>
//#include <mach/platform.h>

/* * 基本数据类型定义  **/
typedef unsigned char           	HI_U8;
typedef unsigned char           	HI_UCHAR;
typedef unsigned short          	HI_U16;
typedef unsigned int               	HI_U32;
typedef char                    		HI_S8;
typedef short                   		HI_S16;
typedef int                     		HI_S32;
typedef char                    		HI_CHAR;
typedef char*                   		HI_PCHAR;
typedef void                    		HI_VOID;
typedef unsigned int      		UINT32;
typedef unsigned short 		UINT16;
typedef unsigned char  		UINT8;
typedef unsigned int 		UINT;
typedef unsigned long 		ULONG;
typedef unsigned long 		*PULONG;
typedef unsigned int 		DWORD;
typedef unsigned char 		*PUINT8;
typedef long	LONG, *PLONG;


/* * 常量定义        **/
typedef enum {
    HI_FALSE    = 0,
    HI_TRUE     = 1,
} HI_BOOL;

#ifndef FALSE
#define FALSE HI_FALSE
#endif

#ifndef TRUE
#define TRUE  HI_TRUE
#endif

#ifndef BOOL
#define BOOL  unsigned int
#endif

#ifndef NULL
#define NULL             0L
#endif

#define HI_NULL          0L
#define HI_NULL_PTR      0L

#define HI_SUCCESS       0
#define HI_FAILURE       (-1)

#define  SUCCESS         0
#define  FAILURE          -1

#define set32(reg, value) (*((volatile unsigned long *)(reg)) = (value))
#define get32(reg) (*(volatile unsigned long *)(reg))
#define INREG32(reg)  (*(unsigned long *)(reg))
#define OUTREG32(reg, value) (*((unsigned long *)(reg)) = (value))
#define OALPAtoVA(phyaddr, size)  ioremap(phyaddr,size)
#define OALPAtoUA(phyaddr) IO_ADDRESS(phyaddr)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_TYPE_H__ */


