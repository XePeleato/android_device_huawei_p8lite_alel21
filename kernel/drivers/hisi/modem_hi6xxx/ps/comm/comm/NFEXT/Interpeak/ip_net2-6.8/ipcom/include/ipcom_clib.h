/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_clib.h,v $ $Revision: 1.67 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_clib.h,v $
 *   $Author: uolofsso $ $Date: 2009-01-27 10:07:07 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_CLIB_H
#define IPCOM_CLIB_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
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

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clibmap.h"
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
#include "ipcom_file.h"
#endif

#ifdef __cplusplus
extern "C" {

/* Use native prototypes for C++ */
#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    clibmap auto-fix
 *===========================================================================
 */
#ifndef IP_PORT_LAS
#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM || IPCOM_USE_SHELL <= IPCOM_SHELL_IPCOM

#ifdef IPCOM_PRINTF
#undef IPCOM_PRINTF
#endif
#define IPCOM_PRINTF 1

#ifdef IPCOM_FPRINTF
#undef IPCOM_FPRINTF
#endif
#define IPCOM_FPRINTF 1

#endif

#ifdef IPEMANATE
#ifdef IPCOM_LOG
#undef IPCOM_LOG
#endif
#define IPCOM_LOG 1
#endif

#endif /* IP_PORT_LAS */

/*
 *===========================================================================
 *                         misc
 *===========================================================================
 */
#define IP_RAND_MAX 2147483647


/*
 *===========================================================================
 *            User library support
 *===========================================================================
 */
#ifndef IP_PORT_LKM
#if (!defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)) || defined(IPCOM_USE_REMOTE_RANDOM)
#define ipcom_random    ipcom_random_usr
#endif
#endif

#if (!defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)) || defined(IPCOM_USE_REMOTE_MALLOC)
#ifndef IPCOM_DMALLOC_C
#define IPCOM_DMALLOC_C
#endif
#define ipcom_malloc    ipcom_malloc_usr
#define ipcom_calloc    ipcom_calloc_usr
#define ipcom_realloc   ipcom_realloc_usr
#define ipcom_free      ipcom_free_usr
#define ipcom_strdup    ipcom_strdup_usr
#endif


/*
 *===========================================================================
 *                         stdin, stdout, stderr
 *===========================================================================
 */
#ifndef IPCOM_USE_STDXXX
#define IPCOM_USE_STDXXX
#define ip_stdin  ipcom_stdin()
#define ip_stdout ipcom_stdout()
#define ip_stderr ipcom_stderr()
#endif


/*
 *===========================================================================
 *                         IP_ASSERT
 *===========================================================================
 */
#ifdef IP_ASSERT
#define ip_assert(xexpr) ((xexpr) != 0 ? (void)0 : ipcom_assert(#xexpr, __FILE__, __LINE__))
#define IP_ASSERTLINE(xline)    xline
#define IP_PANIC()              ipcom_assert("panic", __FILE__, __LINE__)
#else
#define ip_assert(xexpr)        IP_NOOP
#define IP_ASSERTLINE(xline)    IP_NOOP
#define IP_PANIC()              IP_NOOP
#endif

/* level 2 for API asserts. */
#ifdef IP_ASSERT2
#define ip_assert2(xexpr) ((xexpr) != 0 ? (void)0 : ipcom_assert(#xexpr, __FILE__, __LINE__))
#define IP_PANIC2()             ipcom_assert("panic", __FILE__, __LINE__)
#else
#define ip_assert2(xexpr)       IP_NOOP
#define IP_PANIC2()             IP_NOOP
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ip_assert
 *===========================================================================
 * Interpeak assert structure. A global Ip_assert structure 'ip_assert' is filled
 * in by the ipcom_assert() function upon failing an assert.
 */
typedef struct Ip_assert_struct
{
  const char    *expr;   /* Expression that failed */
  const char    *file;   /* File name where the assert macro triggered. */
  Ip_u32         line;   /* File line number. */
}
Ip_assert;


/*
 *===========================================================================
 *                         Ip_utsname
 *===========================================================================
 * for ipcom_uname()
 */

struct Ip_utsname
{
    char sysname[20];
    char nodename[20];
    char release[20];
    char version[20];
    char machine[20];
};


/*
 ****************************************************************************
 * 6                    SPECIAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipcom_assert
 *===========================================================================
 */
IP_PUBLIC void ipcom_assert(const char *expr, const char *file, Ip_u32 line);


/*
 *===========================================================================
 *                         ipcom_random_seed_state
 *===========================================================================
 */
IP_PUBLIC Ip_s32 ipcom_random_seed_state(void);


/*
 *===========================================================================
 *                         ipcom_debug_printf
 *===========================================================================
 */
IP_PUBLIC int ipcom_debug_printf(const char *format, ...);


/*
 *===========================================================================
 *                         ipcom_ffs
 *===========================================================================
 */
IP_PUBLIC int ipcom_ffs(Ip_u32 mask);


/*
 *===========================================================================
 *                         ipcom_uname
 *===========================================================================
 */
IP_PUBLIC int ipcom_uname(struct Ip_utsname *buf);


/*
 ****************************************************************************
 * 7                    STANDARD FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         ipcom_memcpy
 *===========================================================================
 */

#if defined(IPCOM_MEMCPY) && IPCOM_MEMCPY != 1
#define ipcom_memcpy IPCOM_MEMCPY
#elif !defined(IPCOM_MEMCPY)
#define ipcom_memcpy memcpy
#endif

#if (defined(IPCOM_MEMCPY) && IPCOM_MEMCPY == 1) || (defined(IPCOM_USE_CLIB_PROTO) && !defined(IPCOM_CLIB_BUILTIN))
IP_PUBLIC void *ipcom_memcpy(void *s1, const void *s2, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_memset
 *===========================================================================
 */

#if defined(IPCOM_MEMSET) && IPCOM_MEMSET != 1
#define ipcom_memset IPCOM_MEMSET
#elif !defined(IPCOM_MEMSET)
#define ipcom_memset memset
#endif

#if (defined(IPCOM_MEMSET) && IPCOM_MEMSET == 1) || (defined(IPCOM_USE_CLIB_PROTO) && !defined(IPCOM_CLIB_BUILTIN))
IP_PUBLIC void *ipcom_memset(void *s, int c, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_memcmp
 *===========================================================================
 */

#if defined(IPCOM_MEMCMP) && IPCOM_MEMCMP != 1
#define ipcom_memcmp IPCOM_MEMCMP
#elif !defined(IPCOM_MEMCMP)
#define ipcom_memcmp memcmp
#endif

#if (defined(IPCOM_MEMCMP) && IPCOM_MEMCMP == 1) || (defined(IPCOM_USE_CLIB_PROTO) && !defined(IPCOM_CLIB_BUILTIN))
IP_PUBLIC int   ipcom_memcmp(const void *s1, const void *s2, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_memmove
 *===========================================================================
 */

#if defined(IPCOM_MEMMOVE) && IPCOM_MEMMOVE != 1
#define ipcom_memmove IPCOM_MEMMOVE
#elif !defined(IPCOM_MEMMOVE)
#define ipcom_memmove memmove
#endif

#if (defined(IPCOM_MEMMOVE) && IPCOM_MEMMOVE == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void *ipcom_memmove(void *s1, const void *s2, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_memchr
 *===========================================================================
 */

#if defined(IPCOM_MEMCHR) && IPCOM_MEMCHR != 1
#define ipcom_memchr IPCOM_MEMCHR
#elif !defined(IPCOM_MEMCHR)
#define ipcom_memchr memchr
#endif

#if (defined(IPCOM_MEMCHR) && IPCOM_MEMCHR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void *ipcom_memchr(const void *s, int c, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_strlen
 *===========================================================================
 */

#if defined(IPCOM_STRLEN) && IPCOM_STRLEN != 1
#define ipcom_strlen IPCOM_STRLEN
#elif !defined(IPCOM_STRLEN)
#define ipcom_strlen strlen
#endif

#if (defined(IPCOM_STRLEN) && IPCOM_STRLEN == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_size_t ipcom_strlen(const char *s);
#endif


/*
 *===========================================================================
 *                         ipcom_strcpy
 *===========================================================================
 */

#if defined(IPCOM_STRCPY) && IPCOM_STRCPY != 1
#define ipcom_strcpy IPCOM_STRCPY
#elif !defined(IPCOM_STRCPY)
#define ipcom_strcpy strcpy
#endif

#if (defined(IPCOM_STRCPY) && IPCOM_STRCPY == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strcpy(char *s1, const char *s2);
#endif


/*
 *===========================================================================
 *                         ipcom_strncpy
 *===========================================================================
 */

#if defined(IPCOM_STRNCPY) && IPCOM_STRNCPY != 1
#define ipcom_strncpy IPCOM_STRNCPY
#elif !defined(IPCOM_STRNCPY)
#define ipcom_strncpy strncpy
#endif

#if (defined(IPCOM_STRNCPY) && IPCOM_STRNCPY == 1) || (defined(IPCOM_USE_CLIB_PROTO) && !defined(IPCOM_CLIB_BUILTIN))
IP_PUBLIC char *ipcom_strncpy(char *s1, const char *s2, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_strlcpy
 *===========================================================================
 */

#if defined(IPCOM_STRLCPY) && IPCOM_STRLCPY != 1
#define ipcom_strlcpy IPCOM_STRLCPY
#elif !defined(IPCOM_STRLCPY)
#define ipcom_strlcpy strlcpy
#endif

#if (defined(IPCOM_STRLCPY) && IPCOM_STRLCPY == 1) || (defined(IPCOM_USE_CLIB_PROTO) && !defined(IPCOM_CLIB_BUILTIN))
IP_PUBLIC Ip_size_t ipcom_strlcpy(char *s1, const char *s2, Ip_size_t c);
#endif


/*
 *===========================================================================
 *                         ipcom_strcmp
 *===========================================================================
 */

#if defined(IPCOM_STRCMP) && IPCOM_STRCMP != 1
#define ipcom_strcmp IPCOM_STRCMP
#elif !defined(IPCOM_STRCMP)
#define ipcom_strcmp strcmp
#endif

#if (defined(IPCOM_STRCMP) && IPCOM_STRCMP == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_strcmp (const char *s1, const char *s2);
#endif


/*
 *===========================================================================
 *                         ipcom_strncmp
 *===========================================================================
 */

#if defined(IPCOM_STRNCMP) && IPCOM_STRNCMP != 1
#define ipcom_strncmp IPCOM_STRNCMP
#elif !defined(IPCOM_STRNCMP)
#define ipcom_strncmp strncmp
#endif

#if (defined(IPCOM_STRNCMP) && IPCOM_STRNCMP == 1) || (defined(IPCOM_USE_CLIB_PROTO) && !defined(IPCOM_CLIB_BUILTIN))
IP_PUBLIC int ipcom_strncmp(const char *s1, const char *s2, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_strcasecmp
 *===========================================================================
 */

#if defined(IPCOM_STRCASECMP) && IPCOM_STRCASECMP != 1
#define ipcom_strcasecmp IPCOM_STRCASECMP
#elif !defined(IPCOM_STRCASECMP)
#define ipcom_strcasecmp strcasecmp
#endif

#if (defined(IPCOM_STRCASECMP) && IPCOM_STRCASECMP == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_strcasecmp(const char *s1, const char *s2);
#endif


/*
 *===========================================================================
 *                         ipcom_strncasecmp
 *===========================================================================
 */

#if defined(IPCOM_STRNCASECMP) && IPCOM_STRNCASECMP != 1
#define ipcom_strncasecmp IPCOM_STRNCASECMP
#elif !defined(IPCOM_STRNCASECMP)
#define ipcom_strncasecmp strncasecmp
#endif

#if (defined(IPCOM_STRNCASECMP) && IPCOM_STRNCASECMP == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_strncasecmp(const char *s1, const char *s2, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_strcat
 *===========================================================================
 */

#if defined(IPCOM_STRCAT) && IPCOM_STRCAT != 1
#define ipcom_strcat IPCOM_STRCAT
#elif !defined(IPCOM_STRCAT)
#define ipcom_strcat strcat
#endif

#if (defined(IPCOM_STRCAT) && IPCOM_STRCAT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strcat(char *s1, const char *s2);
#endif


/*
 *===========================================================================
 *                         ipcom_strncat
 *===========================================================================
 */

#if defined(IPCOM_STRNCAT) && IPCOM_STRNCAT != 1
#define ipcom_strncat IPCOM_STRNCAT
#elif !defined(IPCOM_STRNCAT)
#define ipcom_strncat strncat
#endif

#if (defined(IPCOM_STRNCAT) && IPCOM_STRNCAT == 1) || (defined(IPCOM_USE_CLIB_PROTO) && !defined(IPCOM_CLIB_BUILTIN))
IP_PUBLIC char *ipcom_strncat(char *s1, const char *s2, Ip_size_t n);
#endif


/*
 *===========================================================================
 *                         ipcom_strlcat
 *===========================================================================
 */

#if defined(IPCOM_STRLCAT) && IPCOM_STRLCAT != 1
#define ipcom_strlcat IPCOM_STRLCAT
#elif !defined(IPCOM_STRLCAT)
#define ipcom_strlcat strlcat
#endif

#if (defined(IPCOM_STRLCAT) && IPCOM_STRLCAT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_size_t ipcom_strlcat(char *s1, const char *s2, Ip_size_t c);
#endif


/*
 *===========================================================================
 *                         ipcom_strchr
 *===========================================================================
 */

#if defined(IPCOM_STRCHR) && IPCOM_STRCHR != 1
#define ipcom_strchr IPCOM_STRCHR
#elif !defined(IPCOM_STRCHR)
#define ipcom_strchr strchr
#endif

#if (defined(IPCOM_STRCHR) && IPCOM_STRCHR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strchr(const char *s, int c);
#endif


/*
 *===========================================================================
 *                         ipcom_strrchr
 *===========================================================================
 */

#if defined(IPCOM_STRRCHR) && IPCOM_STRRCHR != 1
#define ipcom_strrchr IPCOM_STRRCHR
#elif !defined(IPCOM_STRRCHR)
#define ipcom_strrchr strrchr
#endif

#if (defined(IPCOM_STRRCHR) && IPCOM_STRRCHR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strrchr(const char *s, int c);
#endif


/*
 *===========================================================================
 *                         ipcom_strstr
 *===========================================================================
 */

#if defined(IPCOM_STRSTR) && IPCOM_STRSTR != 1
#define ipcom_strstr IPCOM_STRSTR
#elif !defined(IPCOM_STRSTR)
#define ipcom_strstr strstr
#endif

#if (defined(IPCOM_STRSTR) && IPCOM_STRSTR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strstr(const char *s1, const char *s2);
#endif


/*
 *===========================================================================
 *                         ipcom_strcasestr
 *===========================================================================
 */

#if defined(IPCOM_STRCASESTR) && IPCOM_STRCASESTR != 1
#define ipcom_strcasestr IPCOM_STRCASESTR
#elif !defined(IPCOM_STRCASESTR)
#define ipcom_strcasestr strcasestr
#endif

#if (defined(IPCOM_STRCASESTR) && IPCOM_STRCASESTR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strcasestr(const char *s1, const char *s2);
#endif


/*
 *===========================================================================
 *                         ipcom_strtok
 *===========================================================================
 * NOTE: Use ipcom_strtok_r()!
 */

#if defined(IPCOM_STRTOK) && IPCOM_STRTOK != 1
#define ipcom_strtok IPCOM_STRTOK
#elif !defined(IPCOM_STRTOK)
#define ipcom_strtok strtok
#endif

#if (defined(IPCOM_STRTOK) && IPCOM_STRTOK == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strtok(char *s, const char *st);
#endif


/*
 *===========================================================================
 *                         ipcom_strtok_r
 *===========================================================================
 */

#if defined(IPCOM_STRTOK_R) && IPCOM_STRTOK_R != 1
#define ipcom_strtok_r IPCOM_STRTOK_R
#elif !defined(IPCOM_STRTOK_R)
#define ipcom_strtok_r strtok_r
#endif

#if (defined(IPCOM_STRTOK_R) && IPCOM_STRTOK_R == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strtok_r(char *s, const char *st, char **last);
#endif


/*
 *===========================================================================
 *                         ipcom_strsep
 *===========================================================================
 */

#if defined(IPCOM_STRSEP) && IPCOM_STRSEP != 1
#define ipcom_strsep IPCOM_STRSEP
#elif !defined(IPCOM_STRSEP)
#define ipcom_strsep strsep
#endif

#if (defined(IPCOM_STRSEP) && IPCOM_STRSEP == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strsep(char **cp, const char *d);
#endif


/*
 *===========================================================================
 *                         ipcom_strspn
 *===========================================================================
 */

#if defined(IPCOM_STRSPN) && IPCOM_STRSPN != 1
#define ipcom_strspn IPCOM_STRSPN
#elif !defined(IPCOM_STRSPN)
#define ipcom_strspn strspn
#endif

#if (defined(IPCOM_STRSPN) && IPCOM_STRSPN == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_size_t ipcom_strspn(const char *s1, const char *s2);
#endif


/*
 *===========================================================================
 *                         ipcom_strcspn
 *===========================================================================
 */

#if defined(IPCOM_STRCSPN) && IPCOM_STRCSPN != 1
#define ipcom_strcspn IPCOM_STRCSPN
#elif !defined(IPCOM_STRCSPN)
#define ipcom_strcspn strcspn
#endif

#if (defined(IPCOM_STRCSPN) && IPCOM_STRCSPN == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_size_t ipcom_strcspn(const char *s1, const char *s2);
#endif


/*
 *===========================================================================
 *                         ipcom_strtol
 *===========================================================================
 */

#if defined(IPCOM_STRTOL) && IPCOM_STRTOL != 1
#define ipcom_strtol IPCOM_STRTOL
#elif !defined(IPCOM_STRTOL)
#define ipcom_strtol strtol
#endif

#if (defined(IPCOM_STRTOL) && IPCOM_STRTOL == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC long ipcom_strtol(const char *str, char **endptr, int base);
#endif


/*
 *===========================================================================
 *                         ipcom_strtoul
 *===========================================================================
 */

#if defined(IPCOM_STRTOUL) && IPCOM_STRTOUL != 1
#define ipcom_strtoul IPCOM_STRTOUL
#elif !defined(IPCOM_STRTOUL)
#define ipcom_strtoul strtoul
#endif

#if (defined(IPCOM_STRTOUL) && IPCOM_STRTOUL == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC unsigned long ipcom_strtoul(const char *str, char **endptr, int base);
#endif

/*
 *===========================================================================
 *                         ipcom_strtoull
 *===========================================================================
 */

#if defined(IPCOM_STRTOULL) && IPCOM_STRTOULL != 1
#define ipcom_strtoull IPCOM_STRTOULL
#elif !defined(IPCOM_STRTOULL)
#define ipcom_strtoull strtoull
#endif

#if (defined(IPCOM_STRTOULL) && IPCOM_STRTOULL == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC unsigned long long ipcom_strtoull(const char *str, char **endptr, int base);
#endif


/*
 *===========================================================================
 *                         ipcom_strpbrk
 *===========================================================================
 */

#if defined(IPCOM_STRPBRK) && IPCOM_STRPBRK != 1
#define ipcom_strpbrk IPCOM_STRPBRK
#elif !defined(IPCOM_STRPBRK)
#define ipcom_strpbrk strpbrk
#endif

#if (defined(IPCOM_STRPBRK) && IPCOM_STRPBRK == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strpbrk(const char *s, const char *charset);
#endif


/*
 *===========================================================================
 *                         ipcom_index
 *===========================================================================
 */

#if defined(IPCOM_INDEX) && IPCOM_INDEX != 1
#define ipcom_index IPCOM_INDEX
#elif !defined(IPCOM_INDEX)
#define ipcom_index index
#endif

#if (defined(IPCOM_INDEX) && IPCOM_INDEX == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_index(const char *s, int c);
#endif


/*
 *===========================================================================
 *                         ipcom_rindex
 *===========================================================================
 */

#if defined(IPCOM_RINDEX) && IPCOM_RINDEX != 1
#define ipcom_rindex IPCOM_RINDEX
#elif !defined(IPCOM_RINDEX)
#define ipcom_rindex rindex
#endif

#if (defined(IPCOM_RINDEX) && IPCOM_RINDEX == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_rindex(const char *s, int c);
#endif


/*
 *===========================================================================
 *                         ipcom_atoi
 *===========================================================================
 */

#if defined(IPCOM_ATOI) && IPCOM_ATOI != 1
#define ipcom_atoi IPCOM_ATOI
#elif !defined(IPCOM_ATOI)
#define ipcom_atoi atoi
#endif

#if (defined(IPCOM_ATOI) && IPCOM_ATOI == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_atoi(const char *str);
#endif


/*
 *===========================================================================
 *                         ipcom_atol
 *===========================================================================
 */

#if defined(IPCOM_ATOL) && IPCOM_ATOL != 1
#define ipcom_atol IPCOM_ATOL
#elif !defined(IPCOM_ATOL)
#define ipcom_atol atol
#endif

#if (defined(IPCOM_ATOL) && IPCOM_ATOL == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC long ipcom_atol(const char *str);
#endif


/*
 *===========================================================================
 *                         ipcom_malloc
 *===========================================================================
 */

#if defined(IPCOM_USE_DMALLOC) && !defined(IPCOM_DMALLOC_C)
IP_PUBLIC void *ipcom_dmalloc(Ip_size_t size, const char *file, const char *function, Ip_u32 line);
#define ipcom_malloc(xsz) ipcom_dmalloc(xsz, __FILE__, __FUNCTION__, __LINE__)
#else

#if defined(IPCOM_MALLOC) && IPCOM_MALLOC != 1
#define ipcom_malloc IPCOM_MALLOC
#elif !defined(IPCOM_MALLOC)
#define ipcom_malloc malloc
#endif

#if (defined(IPCOM_MALLOC) && IPCOM_MALLOC == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void *ipcom_malloc(Ip_size_t size);
#endif

#endif

/* Note: use below function for function pointers to malloc() with dmalloc */
#if defined(IPCOM_USE_DMALLOC) || defined(IPCOM_DMALLOC_C)
IP_PUBLIC void *ipcom_dmalloc_ptr(Ip_size_t size);
#endif


/*
 *===========================================================================
 *                         ipcom_calloc
 *===========================================================================
 */

#if defined(IPCOM_USE_DMALLOC) && !defined(IPCOM_DMALLOC_C)
IP_PUBLIC void *ipcom_dcalloc(Ip_size_t nelem, Ip_size_t elsize, const char *file, const char *function, Ip_u32 line);
#define ipcom_calloc(n,s)  ipcom_dcalloc(n,s, __FILE__, __FUNCTION__, __LINE__)
#else

#if defined(IPCOM_CALLOC) && IPCOM_CALLOC != 1
#define ipcom_calloc IPCOM_CALLOC
#elif !defined(IPCOM_CALLOC)
#define ipcom_calloc calloc
#endif

#if (defined(IPCOM_CALLOC) && IPCOM_CALLOC == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void *ipcom_calloc(Ip_size_t nelem, Ip_size_t elsize);
#endif

#endif


/*
 *===========================================================================
 *                         ipcom_realloc
 *===========================================================================
 */

#if defined(IPCOM_USE_DMALLOC) && !defined(IPCOM_DMALLOC_C)
IP_PUBLIC void *ipcom_drealloc(void *ptr, Ip_size_t size, const char *file, const char *function, Ip_u32 line);
#define ipcom_realloc(p,s)  ipcom_drealloc(p,s, __FILE__, __FUNCTION__, __LINE__)
#else

#if defined(IPCOM_REALLOC) && IPCOM_REALLOC != 1
#define ipcom_realloc IPCOM_REALLOC
#elif !defined(IPCOM_REALLOC)
#define ipcom_realloc realloc
#endif

#if (defined(IPCOM_REALLOC) && IPCOM_REALLOC == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void *ipcom_realloc(void *ptr, Ip_size_t size);
#endif

#endif


/*
 *===========================================================================
 *                         ipcom_cache_line_size
 *===========================================================================
 */

IP_PUBLIC Ip_u32 ipcom_cache_line_size( void );


/*
 *===========================================================================
 *                         ipcom_strdup
 *===========================================================================
 */

#if defined(IPCOM_USE_DMALLOC) && !defined(IPCOM_DMALLOC_C)
IP_PUBLIC char *ipcom_dstrdup(const char *s1, const char *file, const char *function, Ip_u32 line);
#define ipcom_strdup(s)  ipcom_dstrdup(s, __FILE__, __FUNCTION__, __LINE__)
#else

#if defined(IPCOM_STRDUP) && IPCOM_STRDUP != 1
#define ipcom_strdup IPCOM_STRDUP
#elif !defined(IPCOM_STRDUP)
#define ipcom_strdup strdup
#endif

#if (defined(IPCOM_STRDUP) && IPCOM_STRDUP == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strdup(const char *s1);
#endif

#endif


/*
 *===========================================================================
 *                         ipcom_free
 *===========================================================================
 */

#if defined(IPCOM_USE_DMALLOC) && !defined(IPCOM_DMALLOC_C)
IP_PUBLIC void ipcom_dfree(void *ptr);
#define ipcom_free  ipcom_dfree
#else

#if defined(IPCOM_FREE) && IPCOM_FREE != 1
#define ipcom_free IPCOM_FREE
#elif !defined(IPCOM_FREE)
#define ipcom_free free
#endif

#if (defined(IPCOM_FREE) && IPCOM_FREE == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void ipcom_free(void *ptr);
#endif

#endif


/*
 *===========================================================================
 *                         ipcom_bufcheck
 *===========================================================================
 */
#if defined(IPCOM_USE_DMALLOC) || defined(IPCOM_DMALLOC_C)
IP_PUBLIC void ipcom_bufcheck(void *ptr);
#else
#define ipcom_bufcheck(ptr)  IP_NOOP
#endif


/*
 *===========================================================================
 *                         ipcom_sprintf
 *===========================================================================
 */

#if defined(IPCOM_SPRINTF) && IPCOM_SPRINTF != 1
#define ipcom_sprintf IPCOM_SPRINTF
#elif !defined(IPCOM_SPRINTF)
#define ipcom_sprintf sprintf
#endif

#if (defined(IPCOM_SPRINTF) && IPCOM_SPRINTF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_sprintf(char *s, const char *format, /*args*/ ...);
#endif


/*
 *===========================================================================
 *                         ipcom_snprintf
 *===========================================================================
 */

#if defined(IPCOM_SNPRINTF) && IPCOM_SNPRINTF != 1
#define ipcom_snprintf IPCOM_SNPRINTF
#elif !defined(IPCOM_SNPRINTF)
#define ipcom_snprintf snprintf
#endif

#if (defined(IPCOM_SNPRINTF) && IPCOM_SNPRINTF == 1) || (defined(IPCOM_USE_CLIB_PROTO) && !defined(IPCOM_CLIB_BUILTIN))
IP_PUBLIC int ipcom_snprintf(char *s, Ip_size_t n, const char  *format, /*args*/ ...);
#endif


/*
 *===========================================================================
 *                         ipcom_sscanf
 *===========================================================================
 */

#if defined(IPCOM_SSCANF) && IPCOM_SSCANF != 1
#define ipcom_sscanf IPCOM_SSCANF
#elif !defined(IPCOM_SSCANF)
#define ipcom_sscanf sscanf
#endif

#if (defined(IPCOM_SSCANF) && IPCOM_SSCANF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_sscanf(const char *s, const char *format, ...);
#endif

/*
 *===========================================================================
 *                         ipcom_fscanf
 *===========================================================================
 */

#if defined(IPCOM_FSCANF) && IPCOM_FSCANF != 1
#define ipcom_fscanf IPCOM_FSCANF
#elif !defined(IPCOM_FSCANF)
#define ipcom_fscanf fscanf
#endif

#if (defined(IPCOM_FSCANF) && IPCOM_FSCANF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_fscanf(IP_FILE *stream, const char *format, ...);
#endif


/*
 *===========================================================================
 *                         ipcom_getuid
 *===========================================================================
 * port/src/ipcom_getuid.c - ipcom_getuid()
 */

#if defined(IPCOM_GETUID) && IPCOM_GETUID != 1
#define ipcom_getuid IPCOM_GETUID
#elif !defined(IPCOM_GETUID)
#define ipcom_getuid getuid
#endif

#if (defined(IPCOM_GETUID) && IPCOM_GETUID == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_uid_t ipcom_getuid(void);
#endif


/*
 *===========================================================================
 *                         ipcom_stdxxx
 *===========================================================================
 */
#ifdef IPCOM_USE_STDXXX
IP_PUBLIC IP_FILE *ipcom_stdin(void);
IP_PUBLIC IP_FILE *ipcom_stdout(void);
IP_PUBLIC IP_FILE *ipcom_stderr(void);
#endif


/*
 *===========================================================================
 *                         ipcom_printf
 *===========================================================================
 */

#if defined(IPCOM_PRINTF) && IPCOM_PRINTF != 1
#define ipcom_printf IPCOM_PRINTF
#elif !defined(IPCOM_PRINTF)
#define ipcom_printf printf
#endif

#if (defined(IPCOM_PRINTF) && IPCOM_PRINTF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_printf(const char *format, /*args*/ ...);
#endif


/*
 *===========================================================================
 *                         ipcom_fprintf
 *===========================================================================
 */
#if defined(IPCOM_FPRINTF) && IPCOM_FPRINTF != 1
#define ipcom_fprintf IPCOM_FPRINTF
#elif !defined(IPCOM_FPRINTF)
#define ipcom_fprintf fprintf
#endif

#if (defined(IPCOM_FPRINTF) && IPCOM_FPRINTF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_fprintf(IP_FILE *stream, const char *format, /*args*/ ...);
#endif


/*
 *===========================================================================
 *                         ipcom_random
 *===========================================================================
 */

#if defined(IPCOM_RANDOM) && IPCOM_RANDOM != 1
#define ipcom_random IPCOM_RANDOM
#elif !defined(IPCOM_RANDOM)
#define ipcom_random random
#endif

#if (defined(IPCOM_RANDOM) && IPCOM_RANDOM == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_u32 ipcom_random(void);
#endif


/*
 *===========================================================================
 *                         ipcom_srandom
 *===========================================================================
 */

#if defined(IPCOM_SRANDOM) && IPCOM_SRANDOM != 1
#define ipcom_srandom IPCOM_SRANDOM
#elif !defined(IPCOM_SRANDOM)
#define ipcom_srandom srandom
#endif

#if (defined(IPCOM_SRANDOM) && IPCOM_SRANDOM == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void ipcom_srandom(unsigned int seed);
#endif


/*
 *===========================================================================
 *                         ipcom_rand
 *===========================================================================
 */

#if defined(IPCOM_RAND) && IPCOM_RAND != 1
#define ipcom_rand IPCOM_RAND
#elif !defined(IPCOM_RAND)
#define ipcom_rand rand
#endif

#if (defined(IPCOM_RAND) && IPCOM_SRAND == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_rand(void);
#endif


/*
 *===========================================================================
 *                         ipcom_srand
 *===========================================================================
 */

#if defined(IPCOM_SRAND) && IPCOM_SRAND != 1
#define ipcom_srand IPCOM_SRAND
#elif !defined(IPCOM_SRAND)
#define ipcom_srand srand
#endif

#if (defined(IPCOM_SRAND) && IPCOM_SRAND == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void ipcom_srand(unsigned int seed);
#endif


/*
 *===========================================================================
 *                         ipcom_exit
 *===========================================================================
 */

#if defined(IPCOM_EXIT) && IPCOM_EXIT != 1
#define ipcom_exit IPCOM_EXIT
#elif !defined(IPCOM_EXIT)
#define ipcom_exit exit
#endif

#if (defined(IPCOM_EXIT) && IPCOM_EXIT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void ipcom_exit(int status);
#endif


/*
 *===========================================================================
 *                         ipcom_abort
 *===========================================================================
 */

#if defined(IPCOM_ABORT) && IPCOM_ABORT != 1
#define ipcom_abort IPCOM_ABORT
#elif !defined(IPCOM_ABORT)
#define ipcom_abort abort
#endif

#if (defined(IPCOM_ABORT) && IPCOM_ABORT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void ipcom_abort(void);
#endif


/*
 *===========================================================================
 *                         ipcom_perror
 *===========================================================================
 * port/src/ipcom_perror.c - ipcom_perror()
 */

#if defined(IPCOM_PERROR) && IPCOM_PERROR != 1
#define ipcom_perror IPCOM_PERROR
#elif !defined(IPCOM_PERROR)
#define ipcom_perror perror
#endif

#if (defined(IPCOM_PERROR) && IPCOM_PERROR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void ipcom_perror(const char *s);
#endif


/*
 *===========================================================================
 *                         ipcom_strerror
 *===========================================================================
 * port/src/ipcom_strerror.c - ipcom_strerror()
 * This routine is non-reentrant. ipcom_strerror_r() is preferred.
 */

#if defined(IPCOM_STRERROR) && IPCOM_STRERROR != 1
#define ipcom_strerror IPCOM_STRERROR
#elif !defined(IPCOM_STRERROR)
#define ipcom_strerror strerror
#endif

#if (defined(IPCOM_STRERROR) && IPCOM_STRERROR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_strerror(int errnum);
#endif


/*
 *===========================================================================
 *                         ipcom_strerror_r
 *===========================================================================
 * port/src/ipcom_strerror.c - ipcom_strerror_r()
 */

#if defined(IPCOM_STRERROR_R) && IPCOM_STRERROR_R != 1
#define ipcom_strerror_r IPCOM_STRERROR_R
#elif !defined(IPCOM_STRERROR_R)
#define ipcom_strerror_r strerror_r
#endif

#if (defined(IPCOM_STRERROR_R) && IPCOM_STRERROR_R == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_strerror_r(int errnum, char *buf, Ip_size_t buflen);
#endif


/*
 *===========================================================================
 *                         ipcom_isspace
 *===========================================================================
 */
#if defined(IPCOM_ISSPACE) && IPCOM_ISSPACE != 1
#define ipcom_isspace IPCOM_ISSPACE
#elif !defined(IPCOM_ISSPACE)
#define ipcom_isspace isspace
#endif

#if (defined(IPCOM_ISSPACE) && IPCOM_ISSPACE == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isspace(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_isalpha
 *===========================================================================
 */
#if defined(IPCOM_ISALPHA) && IPCOM_ISALPHA != 1
#define ipcom_isalpha IPCOM_ISALPHA
#elif !defined(IPCOM_ISALPHA)
#define ipcom_isalpha isalpha
#endif

#if (defined(IPCOM_ISALPHA) && IPCOM_ISALPHA == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isalpha(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_isgraph
 *===========================================================================
 */
#if defined(IPCOM_ISGRAPH) && IPCOM_ISGRAPH != 1
#define ipcom_isgraph IPCOM_ISGRAPH
#elif !defined(IPCOM_ISGRAPH)
#define ipcom_isgraph isgraph
#endif

#if (defined(IPCOM_ISGRAPH) && IPCOM_ISGRAPH == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isgraph(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_isupper
 *===========================================================================
 */
#if defined(IPCOM_ISUPPER) && IPCOM_ISUPPER != 1
#define ipcom_isupper IPCOM_ISUPPER
#elif !defined(IPCOM_ISUPPER)
#define ipcom_isupper isupper
#endif

#if (defined(IPCOM_ISUPPER) && IPCOM_ISUPPER == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isupper(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_islower
 *===========================================================================
 */
#if defined(IPCOM_ISLOWER) && IPCOM_ISLOWER != 1
#define ipcom_islower IPCOM_ISLOWER
#elif !defined(IPCOM_ISLOWER)
#define ipcom_islower islower
#endif

#if (defined(IPCOM_ISLOWER) && IPCOM_ISLOWER == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_islower(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_isdigit
 *===========================================================================
 */
#if defined(IPCOM_ISDIGIT) && IPCOM_ISDIGIT != 1
#define ipcom_isdigit IPCOM_ISDIGIT
#elif !defined(IPCOM_ISDIGIT)
#define ipcom_isdigit isdigit
#endif

#if (defined(IPCOM_ISDIGIT) && IPCOM_ISDIGIT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isdigit(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_isprint
 *===========================================================================
 */
#if defined(IPCOM_ISPRINT) && IPCOM_ISPRINT != 1
#define ipcom_isprint IPCOM_ISPRINT
#elif !defined(IPCOM_ISPRINT)
#define ipcom_isprint isprint
#endif

#if (defined(IPCOM_ISPRINT) && IPCOM_ISPRINT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isprint(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_isalnum
 *===========================================================================
 */
#if defined(IPCOM_ISALNUM) && IPCOM_ISALNUM != 1
#define ipcom_isalnum IPCOM_ISALNUM
#elif !defined(IPCOM_ISALNUM)
#define ipcom_isalnum isalnum
#endif

#if (defined(IPCOM_ISALNUM) && IPCOM_ISALNUM == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isalnum(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_iscntrl
 *===========================================================================
 */
#if defined(IPCOM_ISCNTRL) && IPCOM_ISCNTRL != 1
#define ipcom_iscntrl IPCOM_ISCNTRL
#elif !defined(IPCOM_ISCNTRL)
#define ipcom_iscntrl iscntrl
#endif

#if (defined(IPCOM_ISCNTRL) && IPCOM_ISCNTRL == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_iscntrl(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_ispunct
 *===========================================================================
 */
#if defined(IPCOM_ISPUNCT) && IPCOM_ISPUNCT != 1
#define ipcom_ispunct IPCOM_ISPUNCT
#elif !defined(IPCOM_ISPUNCT)
#define ipcom_ispunct ispunct
#endif

#if (defined(IPCOM_ISPUNCT) && IPCOM_ISPUNCT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_ispunct(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_isxdigit
 *===========================================================================
 */
#if defined(IPCOM_ISXDIGIT) && IPCOM_ISXDIGIT != 1
#define ipcom_isxdigit IPCOM_ISXDIGIT
#elif !defined(IPCOM_ISXDIGIT)
#define ipcom_isxdigit isxdigit
#endif

#if (defined(IPCOM_ISXDIGIT) && IPCOM_ISXDIGIT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isxdigit(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_isascii
 *===========================================================================
 */
#if defined(IPCOM_ISASCII) && IPCOM_ISASCII != 1
#define ipcom_isascii IPCOM_ISASCII
#elif !defined(IPCOM_ISASCII)
#define ipcom_isascii isascii
#endif

#if (defined(IPCOM_ISASCII) && IPCOM_ISASCII == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_isascii(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_toupper
 *===========================================================================
 */
#if defined(IPCOM_TOUPPER) && IPCOM_TOUPPER != 1
#define ipcom_toupper IPCOM_TOUPPER
#elif !defined(IPCOM_TOUPPER)
#define ipcom_toupper toupper
#endif

#if (defined(IPCOM_TOUPPER) && IPCOM_TOUPPER == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_toupper(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_tolower
 *===========================================================================
 */
#if defined(IPCOM_TOLOWER) && IPCOM_TOLOWER != 1
#define ipcom_tolower IPCOM_TOLOWER
#elif !defined(IPCOM_TOLOWER)
#define ipcom_tolower tolower
#endif

#if (defined(IPCOM_TOLOWER) && IPCOM_TOLOWER == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_tolower(int c);
#endif


/*
 *===========================================================================
 *                         ipcom_qsort
 *===========================================================================
 */
#ifdef _MSVC
#pragma warning(push)
#pragma warning(disable:4028)
#endif

#if defined(IPCOM_QSORT) && IPCOM_QSORT != 1
#define ipcom_qsort IPCOM_QSORT
#elif !defined(IPCOM_QSORT)
#define ipcom_qsort qsort
#endif

#if (defined(IPCOM_QSORT) && IPCOM_QSORT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void  ipcom_qsort(void *base, Ip_size_t nel, Ip_size_t width, int (*compar)(const void *, const void *));
#endif


/*
 *===========================================================================
 *                         ipcom_bsearch
 *===========================================================================
 */

#if defined(IPCOM_BSEARCH) && IPCOM_BSEARCH != 1
#define ipcom_bsearch IPCOM_BSEARCH
#elif !defined(IPCOM_BSEARCH)
#define ipcom_bsearch bsearch
#endif

#if (defined(IPCOM_BSEARCH) && IPCOM_BSEARCH == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void *ipcom_bsearch(const void *key, const void *base, Ip_size_t nel, Ip_size_t size, int (*compar)(const void *,const void *));
#endif

#ifdef _MSVC
#pragma warning(pop)
#endif


/*
 *===========================================================================
 *                         ipcom_signal
 *===========================================================================
 */

#if defined(IPCOM_SIGNAL) && IPCOM_SIGNAL != 1
#define ipcom_signal IPCOM_SIGNAL
#elif !defined(IPCOM_SIGNAL)
#define ipcom_signal signal
#endif

#if (defined(IPCOM_SIGNAL) && IPCOM_SIGNAL == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void (*ipcom_signal(int, void (*)(int)))(int);
#endif


/*
 ****************************************************************************
 * 8                    ENVIRONMENT VARIABLES
 ****************************************************************************
 */

IP_PUBLIC char *ipcom_getenv(const char *name);
IP_PUBLIC int ipcom_setenv(const char *name, const char *value, int rewrite);
IP_PUBLIC int ipcom_unsetenv(const char *name);

IP_PUBLIC int ipcom_clearenv(void);
IP_PUBLIC int ipcom_getenv_as_int(const char *name, int default_value);
IP_PUBLIC int ipcom_setenv_as_int(const char *name, int value, int rewrite);


/*
 ****************************************************************************
 * 9                    FLOATING POINT/MATH FUNCTIONS
 ****************************************************************************
 */
#ifdef IPCOM_USE_FLOAT

/*
 *===========================================================================
 *                         ipcom_atof
 *===========================================================================
 */

#if defined(IPCOM_ATOF) && IPCOM_ATOF != 1
#define ipcom_atof IPCOM_ATOF
#elif !defined(IPCOM_ATOF)
#define ipcom_atof atof
#endif

#if (defined(IPCOM_ATOF) && IPCOM_ATOF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_atof(const char *str);
#endif


/*
 *===========================================================================
 *                         ipcom_strtod
 *===========================================================================
 */

#if defined(IPCOM_STRTOD) && IPCOM_STRTOD != 1
#define ipcom_strtod IPCOM_STRTOD
#elif !defined(IPCOM_STRTOD)
#define ipcom_strtod strtod
#endif

#if (defined(IPCOM_STRTOD) && IPCOM_STRTOD == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_strtod(const char *nptr, char **endptr);
#endif


/*
 *===========================================================================
 *                         ipcom_floor
 *===========================================================================
 */

#if defined(IPCOM_FLOOR) && IPCOM_FLOOR != 1
#define ipcom_floor IPCOM_FLOOR
#elif !defined(IPCOM_FLOOR)
#define ipcom_floor floor
#endif

#if (defined(IPCOM_FLOOR) && IPCOM_FLOOR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_floor(Ip_double x);
#endif


/*
 *===========================================================================
 *                         ipcom_pow
 *===========================================================================
 */

#if defined(IPCOM_POW) && IPCOM_POW != 1
#define ipcom_pow IPCOM_POW
#elif !defined(IPCOM_POW)
#define ipcom_pow pow
#endif

#if (defined(IPCOM_POW) && IPCOM_POW == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_pow(Ip_double x, Ip_double y);
#endif


/*
 *===========================================================================
 *                         ipcom_log
 *===========================================================================
 */

#if defined(IPCOM_LOG) && IPCOM_LOG != 1
#define ipcom_log IPCOM_LOG
#elif !defined(IPCOM_LOG)
#define ipcom_log log
#endif

#if (defined(IPCOM_LOG) && IPCOM_LOG == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_log(Ip_double x);
#endif


/*
 *===========================================================================
 *                         ipcom_log10
 *===========================================================================
 */

#if defined(IPCOM_LOG10) && IPCOM_LOG10 != 1
#define ipcom_log10 IPCOM_LOG10
#elif !defined(IPCOM_LOG10)
#define ipcom_log10 log10
#endif

#if (defined(IPCOM_LOG10) && IPCOM_LOG10 == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_log10(Ip_double x);
#endif


/*
 *===========================================================================
 *                         ipcom_modf
 *===========================================================================
 */

#if defined(IPCOM_MODF) && IPCOM_MODF != 1
#define ipcom_modf IPCOM_MODF
#elif !defined(IPCOM_MODF)
#define ipcom_modf modf
#endif

#if (defined(IPCOM_MODF) && IPCOM_MODF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_modf(Ip_double x, Ip_double *iptr);
#endif


/*
 *===========================================================================
 *                         ipcom_sqrt
 *===========================================================================
 */

#if defined(IPCOM_SQRT) && IPCOM_SQRT != 1
#define ipcom_sqrt IPCOM_SQRT
#elif !defined(IPCOM_SQRT)
#define ipcom_sqrt sqrt
#endif

#if (defined(IPCOM_SQRT) && IPCOM_SQRT == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_sqrt(Ip_double x);
#endif


/*
 *===========================================================================
 *                         ipcom_finite
 *===========================================================================
 */

#if defined(IPCOM_FINITE) && IPCOM_FINITE != 1
#define ipcom_finite IPCOM_FINITE
#elif !defined(IPCOM_FINITE)
#define ipcom_finite finite
#endif

#if (defined(IPCOM_FINITE) && IPCOM_FINITE == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_finite(Ip_double value);
#endif


/*
 *===========================================================================
 *                         ipcom_exp
 *===========================================================================
 */

#if defined(IPCOM_EXP) && IPCOM_EXP != 1
#define ipcom_exp IPCOM_EXP
#elif !defined(IPCOM_EXP)
#define ipcom_exp exp
#endif

#if (defined(IPCOM_EXP) && IPCOM_EXP == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_exp(Ip_double x);
#endif


/*
 *===========================================================================
 *                         ipcom_ceil
 *===========================================================================
 */

#if defined(IPCOM_CEIL) && IPCOM_CEIL != 1
#define ipcom_ceil IPCOM_CEIL
#elif !defined(IPCOM_CEIL)
#define ipcom_ceil ceil
#endif

#if (defined(IPCOM_CEIL) && IPCOM_CEIL == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_double ipcom_ceil(Ip_double x);
#endif

#endif /* IPCOM_USE_FLOAT */


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
