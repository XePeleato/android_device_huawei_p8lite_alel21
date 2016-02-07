/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_type.h,v $ $Revision: 1.29.26.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_type.h,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:20:55 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_TYPE_H
#define IPCOM_TYPE_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak types.
 */


/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_port.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         boolean
 *===========================================================================
 * Boolean values for use with the Ip_bool type.
 */
#define IP_TRUE     1L
#define IP_FALSE    0L


/*
 *===========================================================================
 *                         null pointer
 *===========================================================================
 */
#define IP_NULL     0


/*
 *===========================================================================
 *                         no operation
 *===========================================================================
 */
#define IP_NOOP     do {} while ((0))


/*
 *===========================================================================
 *                         line feed
 *===========================================================================
 */
#if IPCOM_USE_SHELL == IPCOM_SHELL_MINI
#define IP_LF      "\n\r"
#else
#define IP_LF      "\n"
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         standard size types
 *===========================================================================
 */
#ifndef IPCOM_STD_TYPE
#define IPCOM_STD_TYPE

/* unsigned and signed 32 bit */
#if defined(IP_U32) != defined(IP_S32)
# error You must define both IP_U32 and IP_S32
#elif defined(IP_U32) && defined(IP_S32)
 typedef IP_U32            Ip_u32;
 typedef IP_S32            Ip_s32;
#elif defined(IP_TARGET_X86_64) || defined(__mips64) || defined(__powerpc64__)
 typedef unsigned int      Ip_u32;
 typedef int               Ip_s32;
#else
 typedef unsigned long int Ip_u32;
 typedef long int          Ip_s32;
#endif

/* unsigned and signed 16 bit */
typedef unsigned short int Ip_u16;
typedef short int          Ip_s16;

/* unsigned and signed 8 bit */
typedef unsigned char      Ip_u8;
typedef signed char        Ip_s8;

#ifdef IPCOM_USE_FLOAT
#ifdef double
#error double_must_not_be_defined_here
#endif
typedef double             Ip_double;
#endif
#endif /* IPCOM_STD_TYPE */


/*
 *===========================================================================
 *                         64 bit type
 *===========================================================================
 * Use 64-bit type (long long) if the compiler supports it.
 */
#if defined(__GNUC__) || defined(__ghs__) || defined(_WIN32) || \
    defined(_lint) || defined(__DCC__)
#define IPCOM_USE_LONGLONG_TYPE
#endif

#ifdef IPCOM_USE_LONGLONG_TYPE

#if defined(_lint)
typedef unsigned long Ip_u64;
typedef signed long   Ip_s64;
#elif defined(__GNUC__) || defined(__ghs__) || defined(__DCC__)
typedef unsigned long long  Ip_u64;
typedef signed long long    Ip_s64;
#elif defined(_WIN32)
typedef unsigned _int64     Ip_u64;
typedef signed _int64       Ip_s64;
#else
typedef unsigned long long  Ip_u64;
typedef signed long long    Ip_s64;
typedef unsigned long long  uint64_t;
#endif
#define IP_U64_COPY(dst,src)      ((dst) = (src))
#define IP_U64_EQUAL(x,y)         ((x) == (y))
#define IP_U64_NEQUAL(x,y)        ((x) != (y))
#define IP_U64_SET(dst,xhi,xlo)   ((dst) = (Ip_u64)(xhi) << 32 | (xlo))
#define IP_U64_SETHI(dst,xhi)     ((dst) = (Ip_u64)(xhi) << 32 | IP_U64_GETLO(dst))
#define IP_U64_SETLO(dst,xlo)     ((dst) = IP_U64_GETHI(dst) | (xlo))
#define IP_U64_GETHI(x)           (Ip_u32)(((x) >> 32) & 0xffffffff)
#define IP_U64_GETLO(x)           (Ip_u32)((x) & 0xffffffff)
#define IP_U64_ISZERO(x)          ((x) == 0)
#define IP_U64_CLEAR(x)           ((x) = 0)
#define IP_U64_ADD(x,y)           ((x) += (y))
#define IP_U64_ADD32(x,y)         ((x) += (Ip_u64)(y))
#define IP_U64_SUB(x,y)           ((x) -= (y))

#else /* IPCOM_USE_LONGLONG_TYPE */
typedef struct
{
    Ip_u32   hi;
    Ip_u32   lo;
}
Ip_u64;

#define IP_U64_COPY(dst,src)      (dst).lo = (src).lo, (dst).hi = (src).hi
#define IP_U64_EQUAL(x,y)         ((x).lo == (y).lo && (x).hi == (y).hi)
#define IP_U64_NEQUAL(x,y)        (!IP_U64_EQUAL(x,y))
#define IP_U64_SET(dst,xhi,xlo)   (dst).hi = (xhi), (dst).lo = (xlo)
#define IP_U64_SETHI(dst,xhi)     (dst).hi = (xhi)
#define IP_U64_SETLO(dst,xlo)     (dst).lo = (xlo)
#define IP_U64_GETHI(x)           ((x).hi)
#define IP_U64_GETLO(x)           ((x).lo)
#define IP_U64_ISZERO(x)          ((x).lo == 0 && (x).hi == 0)
#define IP_U64_CLEAR(x)           ((x).lo = 0, (x).hi = 0)
#define IP_U64_ADD(a,b)           do { Ip_u32 _u = IP_U64_GETLO(a); \
                                       IP_U64_GETLO(a) += IP_U64_GETLO(b); \
                                       IP_U64_GETHI(a) += IP_U64_GETHI(b); \
                                       IP_U64_GETHI(a) += (IP_U64_GETLO(a) < _u ? 1 : 0); \
                                  } while ((0))
#define IP_U64_ADD32(a,b)           do { Ip_u32 _u = IP_U64_GETLO(a); \
                                       IP_U64_GETLO(a) += (b); \
                                       IP_U64_GETHI(a) += (IP_U64_GETLO(a) < _u ? 1 : 0); \
                                  } while ((0))
#endif


/*
 *===========================================================================
 *                         special types
 *===========================================================================
 */

/* function return value type. See ipcom_err.h for values. */
typedef int                  Ip_err;

/* boolean type */
typedef int                  Ip_bool;

/* size/length type */
#if defined(IP_SIZE_T)
 typedef IP_SIZE_T           Ip_size_t;
#else
 typedef unsigned long int   Ip_size_t;
#endif

#if defined(IP_SSIZE_T)
 typedef IP_SSIZE_T          Ip_ssize_t;
#else
 typedef long int            Ip_ssize_t;
#endif

/* core address type */
typedef char *               Ip_caddr_t;

/* normally from stddef.h */
typedef unsigned long        Ip_ptrdiff_t;

/* process id */
typedef Ip_u32               Ip_pid_t;

/* socket descriptor */
typedef int                  Ip_fd;

/* socket select() type */
typedef	long	             Ip_fd_mask;

/* time type */
typedef long int             Ip_time_t;

/* clock type */
typedef long                 Ip_clock_t;

/* user & group ID */
typedef Ip_u32               Ip_uid_t;
typedef Ip_u32               Ip_gid_t;

/* device */
typedef Ip_pid_t             Ip_dev_t;

/* inode */
typedef Ip_u32               Ip_ino_t;

/* File stream (abstract data type) */
struct _IP_FILE;
typedef struct _IP_FILE      IP_FILE;

/* Directory stream (abstract data type) */
struct _IP_DIR;
typedef struct _IP_DIR       IP_DIR;


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

