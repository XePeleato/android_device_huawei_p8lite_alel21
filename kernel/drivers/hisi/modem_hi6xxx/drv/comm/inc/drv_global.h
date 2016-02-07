/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_global.h
  版 本 号   : 初稿
  作    者   : 陈易超 d00209607
  生成日期   : 2013年2月2日
  最近修改   :
  功能描述   : drv_global.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年2月2日
    作    者   : 陈易超 d00209607
    修改内容   : 创建文件

******************************************************************************/
#ifndef __DRV_GLOBAL_H__
#define __DRV_GLOBAL_H__

#if defined(BSP_CORE_MODEM)
#if defined(__OS_RTOSCK__)
#include <sre_base.h>
#endif
#if defined(__VXWORKS__)
#include <vxWorks.h>
#endif
#endif

/* 基本数据类型定义 */
#ifndef _WIN32_COMPILE
typedef signed long long    BSP_S64;
#else
typedef double              BSP_S64;
#endif
typedef signed int          BSP_S32;
typedef signed short        BSP_S16;
typedef signed char         BSP_S8;
typedef char                BSP_CHAR;

#ifndef _WIN32_COMPILE
typedef unsigned long long  BSP_U64;
#else
typedef double              BSP_U64;
#endif
typedef unsigned int        BSP_U32;
typedef unsigned short      BSP_U16;
typedef unsigned char       BSP_U8;

typedef int                 BSP_BOOL;
typedef void                BSP_VOID;
typedef int                 BSP_STATUS;

#ifndef _WIN32_COMPILE
typedef signed long long*   BSP_PS64;
#else
typedef double*             BSP_PS64;
#endif
typedef signed int*         BSP_PS32;
typedef signed short*       BSP_PS16;
typedef signed char*        BSP_PS8;

#ifndef _WIN32_COMPILE
typedef unsigned long long* BSP_PU64;
#else
typedef double*             BSP_PU64;
#endif
typedef unsigned int*       BSP_PU32;
typedef unsigned short*     BSP_PU16;
typedef unsigned char*      BSP_PU8;


typedef int*                BSP_PBOOL;
typedef void*               BSP_PVOID;
typedef int*                BSP_PSTATUS;


/*typedef BSP_S32             UDI_HANDLE;*/

#ifndef BSP_CONST
#define BSP_CONST           const
#endif

#ifndef OK
#define OK                  (0)
#endif

#ifndef ERROR
#define ERROR               (-1)
#endif

#ifndef TRUE
#define TRUE                (1)
#endif

#ifndef FALSE
#define FALSE               (0)
#endif

#ifndef BSP_OK
#define BSP_OK              (0)
#endif

#ifndef BSP_ERROR
#define BSP_ERROR           (-1)
#endif

#ifndef BSP_TRUE
#define BSP_TRUE            (1)
#endif

#ifndef BSP_FALSE
#define BSP_FALSE           (0)
#endif

#ifndef BSP_NULL
#define BSP_NULL            (void*)0
#endif

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM) || defined(__VXWORKS__)
#ifdef __OS_RTOSCK__
#ifndef STATUS
typedef BSP_S32             STATUS;
#endif
#endif
#else
#ifndef UINT8
typedef unsigned char       UINT8;
#endif
#ifndef UINT32
typedef unsigned int        UINT32;
#endif
#ifndef VOID
typedef void                VOID;
#endif
#ifndef STATUS
typedef BSP_S32             STATUS;
#endif
#ifndef _LINUX_KERNEL_STAT_H  /* FUNCPTR defined in kernel_stat.h */
#ifndef FUNCPTR_IS_DEFINED
typedef BSP_S32 (*FUNCPTR)(void);
#define FUNCPTR_IS_DEFINED  1
#endif
#endif
#endif

typedef int (*PWRCTRLFUNCPTRVOID)(void);
/*typedef unsigned int (*PWRCTRLFUNCPTR)(unsigned int arg); */    /* ptr to function returning int */

#ifndef OSAL_INLINE
#define OSAL_INLINE __inline__
#endif

#ifndef INLINE
#if defined(_DRV_LLT_)
#define INLINE
#else
#define INLINE              __inline__
#endif
#endif

#ifndef INLINE_ALWAYS
#if defined(_DRV_LLT_)
#define INLINE_ALWAYS
#else
#define INLINE_ALWAYS       __inline__ __attribute__((always_inline))
#endif
#endif

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#ifdef __OS_RTOSCK__
/*
#ifndef FUNCPTR
typedef BSP_S32 (*FUNCPTR)(void);
#endif
*/
/*
#ifndef VOIDFUNCPTR
typedef BSP_VOID (*VOIDFUNCPTR)(BSP_U32);
#endif
*/
#ifndef LOCAL
#define LOCAL               static
#endif
#ifndef SEM_FULL
#define SEM_FULL            (1)
#endif
#ifndef SEM_EMPTY
#define SEM_EMPTY           (0)
#endif
#endif /*end of FEATURE_RTOSCK == FEATURE_ON*/
#else
/*typedef BSP_VOID (*VOIDFUNCPTR)(BSP_U32);*/
#define SEM_FULL            (1)
#define SEM_EMPTY           (0)
#define LOCAL                   static
#define IVEC_TO_INUM(intVec)    ((int)(intVec))
#endif


/* 错误码宏定义 */
#define BSP_ERR_MODULE_OFFSET (0x1000)    /* 防止和系统的错误码重叠 */
#define BSP_DEF_ERR( mod, errid) \
    ((((BSP_U32) mod + BSP_ERR_MODULE_OFFSET) << 16) | (errid))

#define BSP_REG(base, reg) (*(volatile BSP_U32 *)((BSP_U32)base + (reg)))

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM) || defined(__VXWORKS__)
#define BSP_REG_READ(base, reg, result) \
    ((result) = BSP_REG(base, reg))

#define BSP_REG_WRITE(base, reg, data) \
    (BSP_REG(base, reg) = (data))

#else
#define BSP_REG_READ(base, reg, resule) \
    (resule = readl(base + reg))

#define BSP_REG_WRITE(base, reg, data) \
    (writel(data, (base + reg)))
#endif

/*****************************************************************************
  1 GU侧驱动对外接口，GUPorting项目添加.
*****************************************************************************/

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define DRV_OK                          (0)
#define DRV_ERROR                       (-1)
#define DRV_INTERFACE_RSLT_OK           (0)

/* 基本数据类型定义 */
/*typedef void (*PVOIDFUNC)(void);*/

typedef unsigned long (*pFUNCPTR2)( unsigned long ulPara1, unsigned long ulPara2);


/*需要移到别的头文件中 start*/
#if 0
/*升级Flash信息*/
typedef struct
{
    BSP_U32 ulblockCount;    /*Block个数*/
    BSP_U32 ulpageSize;    /*page页大小*/
    BSP_U32 ulpgCntPerBlk;    /*一个Block中的page个数*/
}DLOAD_FLASH_STRU;
/*需要移到别的头文件中 end*/
#endif

/* 上行线路收包函数指针 */
typedef BSP_VOID (*UpLinkRxFunc)(BSP_U8 *buf, BSP_U32 len);

/* 包封装释放函数指针 */
typedef BSP_VOID (*FreePktEncap)(BSP_VOID *PktEncap);

/* GMAC调用者枚举 */
typedef enum tagGMAC_OWNER_E
{
    GMAC_OWNER_VXWORKS = 0, /* Vxworks网络协议栈 */
    GMAC_OWNER_PS,          /* LTE网络协议栈 */
    GMAC_OWNER_MSP,         /* MSP */
    GMAC_OWNER_MAX          /* 边界值 */
}GMAC_OWNER_E;

typedef enum tagWDT_TIMEOUT_E
{
    WDT_TIMEOUT_1   = 0,        /*0xFFFF000/WDT_CLK_FREQ,  about 3657ms*/      /*WDT_CLK_FREQ = ARM_FREQ/6 = 70M*/
    WDT_TIMEOUT_2,              /*0x1FFFE000/WDT_CLK_FREQ, about 7314ms*/
    WDT_TIMEOUT_3,              /*0x3FFFC000/WDT_CLK_FREQ, about 14628ms*/
    WDT_TIMEOUT_4,              /*0x7FFF8000/WDT_CLK_FREQ, about 29257ms*/
    WDT_TIMEOUT_BUTT
}WDT_TIMEOUT_E;


BSP_U8* BSP_GetMacAddr(BSP_VOID);
extern int ddmPhaseScoreBoot(const char * phaseName, unsigned int param);

#endif

