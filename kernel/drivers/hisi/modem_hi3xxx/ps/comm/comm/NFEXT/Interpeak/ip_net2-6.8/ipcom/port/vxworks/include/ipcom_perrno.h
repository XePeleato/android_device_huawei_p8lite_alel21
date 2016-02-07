/*
 ******************************************************************************
 *                     INTERPEAK HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_perrno.h,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_perrno.h,v $
 *   $Author: lob $ $Date: 2007-01-02 14:25:36 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_PERRNO_H
#define IPCOM_PERRNO_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

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
 *                         errno
 *===========================================================================
 */
#define	IP_ERRNO_EPERM		     1
#define	IP_ERRNO_ENOENT		     2
#define	IP_ERRNO_ESRCH	         3
#define	IP_ERRNO_EINTR		     4
#define	IP_ERRNO_EIO		     5
#define IP_ERRNO_ENXIO           6
#define IP_ERRNO_E2BIG           7
#define	IP_ERRNO_EBADF	         9

#define IP_ERRNO_EAGAIN          11
#define	IP_ERRNO_ENOMEM	         12
#define	IP_ERRNO_EACCES		     13
#define	IP_ERRNO_EFAULT		     14
#define	IP_ERRNO_ENOTEMPTY       15
#define	IP_ERRNO_EBUSY		     16
#define	IP_ERRNO_EEXIST	         17
#define	IP_ERRNO_ENODEV	         19

#define	IP_ERRNO_ENOTDIR	     20
#define	IP_ERRNO_EISDIR		     21
#define IP_ERRNO_EINVAL          22
#define IP_ERRNO_ENFILE          23
#define	IP_ERRNO_EMFILE	         24
#define IP_ERRNO_ENOTTY          25
#define IP_ERRNO_ENAMETOOLONG    26
#define	IP_ERRNO_ENOSPC		     28

#define	IP_ERRNO_EPIPE		     32
#define	IP_ERRNO_EMSGSIZE	     36
#define	IP_ERRNO_EDOM		     37
#define	IP_ERRNO_ERANGE   	     38

#define	IP_ERRNO_EDESTADDRREQ	 40
#define	IP_ERRNO_EPROTOTYPE	     41
#define	IP_ERRNO_ENOPROTOOPT	 42
#define	IP_ERRNO_EPROTONOSUPPORT 43
#define	IP_ERRNO_ESOCKTNOSUPPORT 44
#define	IP_ERRNO_EOPNOTSUPP	     45
#define	IP_ERRNO_EPFNOSUPPORT    46
#define	IP_ERRNO_EAFNOSUPPORT    47
#define IP_ERRNO_EADDRINUSE      48
#define	IP_ERRNO_EADDRNOTAVAIL	 49

#define IP_ERRNO_ENETUNREACH     51
#define IP_ERRNO_ENETRESET       52
#define	IP_ERRNO_ECONNABORTED	 53
#define	IP_ERRNO_ECONNRESET	     54
#define	IP_ERRNO_ENOBUFS	     55
#define	IP_ERRNO_EISCONN	     56
#define	IP_ERRNO_ENOTCONN	     57
#define	IP_ERRNO_ESHUTDOWN	     58
#define	IP_ERRNO_ETOOMANYREFS	 59

#define IP_ERRNO_ETIMEDOUT       60
#define	IP_ERRNO_ECONNREFUSED	 61
#define IP_ERRNO_ENETDOWN        62
#define	IP_ERRNO_EHOSTUNREACH	 65
#define	IP_ERRNO_ENOTBLK	     66
#define IP_ERRNO_EHOSTDOWN       67

#define	IP_ERRNO_EINPROGRESS	 68
#define	IP_ERRNO_EALREADY	     69
#define IP_ERRNO_EWOULDBLOCK     70
#define IP_ERRNO_ENOSYS          71    /* Function not implemented */

#define IP_ERRNO_ENOMSG          80
#define IP_ERRNO_EDQUOT          83


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
