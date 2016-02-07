/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name    : common.h
 * Version       : Initial Draft
 * Author        : Hash
 * Created       : 2012/9/21
 * Last Modified :
 * Description   : hwifi_cfg80211.c header file
 * Function List :
 * History       :
 * 1.Date    : 2012/9/21
      Author      : Hash
      Modification: Created file
 *
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include <linux/slab.h>


/*
 * 2 Macro Definition
 */
#define MAC_LEN             6
#define PMKID_LEN           16

/*wapi key len*/
#define WAPI_KEY_LEN        16
#define WAPI_PN_LEN         16

#define MAX_BA_SESS         4
#define ZERO_INDEX          0

#ifdef WIN32
#define STATIC
#else
#define STATIC              static
#endif

#define FALSE               0
#define TRUE                1

/* TBD: remove all */
#define OS_CPU_TO_LE16                  cpu_to_le16
#define OS_CPU_TO_LE32                  cpu_to_le32
#define OS_LE32_TO_CPU                  le32_to_cpu
#define OS_LE16_TO_CPU                  le16_to_cpu
#define OS_MEM_FREE(buf)                kfree(buf)
#define OS_MEM_CPY(dst, src, size)      memcpy(dst, src, size)
#define OS_MEM_SET(dst, data, size)     memset(dst, data, size)
#define OS_MEM_CMP(dst, src, size)      memcmp(dst, src, size)
#define OS_STR_CPY(dst, src)            strcpy(dst, src)
#define OS_MALLOC_GFP(size)             kmalloc(size, GFP_KERNEL)
#define OS_MALLOC_ATOMIC(size)          kmalloc(size, GFP_ATOMIC)
#define OS_MALLOC_USER(size)            kmalloc(size, GFP_USER)
#define OS_MALLOC_HIGHUSER(size)        kmalloc(size, GFP_HIGHUSER)
#define OS_ZMALLOC_GFP(size)            kzalloc(size, GFP_KERNEL)

/*
 * 3 Enum Type Definition
 */

/*
 * 4 Global Variable Declaring
 */


/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */


/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */
typedef unsigned char               uint8;
typedef signed char                 int8;
typedef short                       int16;
typedef unsigned short              uint16;
typedef unsigned int                uint32;
typedef int                         int32;
typedef long long                   int64;
typedef unsigned long long          uint64;

typedef int                         WORD32;
typedef short                       WORD16;
typedef char                        WORD8;
typedef unsigned int                UWORD32;
typedef unsigned short              UWORD16;
typedef unsigned char               UWORD8;
typedef signed char                 SWORD8;

/*
 * 10 Function Declare
 */


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of common.h */
