/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_strerror.c,v $ $Revision: 1.21 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_strerror.c,v $
 *     $Author: lob $ $Date: 2009-09-08 12:47:47 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

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

#ifdef IP_PORT_VXWORKS
#include <vxWorks.h>
#include <symLib.h>
#include <stdlib.h>
#include <sysSymTbl.h>
#include <private/funcBindP.h>
#include <string.h>
#include <stdio.h>
#else
#define IPCOM_USE_CLIB_PROTO
#endif

#include "ipcom_clib.h"
#include "ipcom_errno.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65 && USE_STRERROR_IF
extern char *strerrorIf(int errcode);
#endif

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#if (defined(IPCOM_STRERROR) && IPCOM_STRERROR == 1) || \
    (defined(IPCOM_STRERROR_R) && IPCOM_STRERROR_R == 1)
#define IPCOM_USE_STRERROR_CODE
#endif


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#ifndef IPCOM_USE_STRERROR_CODE
/* to avoid compiler warning */
int ipcom_strerror_empty_file;
#endif


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */


#ifdef IPCOM_USE_STRERROR_CODE

/*
 *===========================================================================
 *                    ipcom_strerror_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_strerror_int(int errnum)
{
    switch(errnum)
    {
#ifdef IP_ERRNO_EPERM
    case IP_ERRNO_EPERM:	return "Operation not permitted";
#endif
#ifdef IP_ERRNO_ENOENT
    case IP_ERRNO_ENOENT:	return "No such file or directory";
#endif
#ifdef IP_ERRNO_ESRCH
    case IP_ERRNO_ESRCH:	return "No such process";
#endif
#ifdef IP_ERRNO_EINTR
    case IP_ERRNO_EINTR:	return "Interrupted system call";
#endif
#ifdef IP_ERRNO_EIO
    case IP_ERRNO_EIO:		return "Input/output error";
#endif
#ifdef IP_ERRNO_ENXIO
    case IP_ERRNO_ENXIO:	return "Device not configured";
#endif
#ifdef IP_ERRNO_E2BIG
    case IP_ERRNO_E2BIG:	return "Argument list too long";
#endif
#ifdef IP_ERRNO_ENOEXEC
    case IP_ERRNO_ENOEXEC:	return "Exec format error";
#endif
#ifdef IP_ERRNO_EBADF
    case IP_ERRNO_EBADF:	return "Bad file descriptor";
#endif
#ifdef IP_ERRNO_ECHILD
    case IP_ERRNO_ECHILD:	return "No child processes";
#endif
#ifdef IP_ERRNO_EDEADLK
    case IP_ERRNO_EDEADLK:	return "Resource deadlock avoided";
#endif
#ifdef IP_ERRNO_ENOMEM
    case IP_ERRNO_ENOMEM:	return "Cannot allocate memory";
#endif
#ifdef IP_ERRNO_EACCES
    case IP_ERRNO_EACCES:	return "Permission denied";
#endif
#ifdef IP_ERRNO_EFAULT
    case IP_ERRNO_EFAULT:	return "Bad address";
#endif
#ifdef IP_ERRNO_ENOTBLK
    case IP_ERRNO_ENOTBLK:	return "Block device required";
#endif
#ifdef IP_ERRNO_EBUSY
    case IP_ERRNO_EBUSY:	return "Device busy";
#endif
#ifdef IP_ERRNO_EEXIST
    case IP_ERRNO_EEXIST:	return "File exists";
#endif
#ifdef IP_ERRNO_EXDEV
    case IP_ERRNO_EXDEV:	return "Cross-device link";
#endif
#ifdef IP_ERRNO_ENODEV
    case IP_ERRNO_ENODEV:	return "Operation not supported by device";
#endif
#ifdef IP_ERRNO_ENOTDIR
    case IP_ERRNO_ENOTDIR:	return "Not a directory";
#endif
#ifdef IP_ERRNO_EISDIR
    case IP_ERRNO_EISDIR:	return "Is a directory";
#endif
#ifdef IP_ERRNO_EINVAL
    case IP_ERRNO_EINVAL:	return "Invalid argument";
#endif
#ifdef IP_ERRNO_ENFILE
    case IP_ERRNO_ENFILE:	return "Too many open files in system";
#endif
#ifdef IP_ERRNO_EMFILE
    case IP_ERRNO_EMFILE:	return "Too many open files";
#endif
#ifdef IP_ERRNO_ENOTTY
    case IP_ERRNO_ENOTTY:	return "Inappropriate ioctl for device";
#endif
#ifdef IP_ERRNO_ETXTBSY
    case IP_ERRNO_ETXTBSY:	return "Text file busy";
#endif
#ifdef IP_ERRNO_EFBIG
    case IP_ERRNO_EFBIG:	return "File too large";
#endif
#ifdef IP_ERRNO_ENOSPC
    case IP_ERRNO_ENOSPC:	return "No space left on device";
#endif
#ifdef IP_ERRNO_ESPIPE
    case IP_ERRNO_ESPIPE:	return "Illegal seek";
#endif
#ifdef IP_ERRNO_EROFS
    case IP_ERRNO_EROFS:	return "Read-only file system";
#endif
#ifdef IP_ERRNO_EMLINK
    case IP_ERRNO_EMLINK:	return "Too many links";
#endif
#ifdef IP_ERRNO_EPIPE
    case IP_ERRNO_EPIPE:	return "Broken pipe";
#endif
#ifdef IP_ERRNO_EDOM
    case IP_ERRNO_EDOM:		return "Numerical argument out of domain";
#endif
#ifdef IP_ERRNO_ERANGE
    case IP_ERRNO_ERANGE:	return "Result too large";
#endif
#ifdef IP_ERRNO_EAGAIN
    case IP_ERRNO_EAGAIN:	return "Resource temporarily unavailable";
#endif
#ifdef IP_ERRNO_EINPROGRESS
    case IP_ERRNO_EINPROGRESS:	return "Operation now in progress";
#endif
#ifdef IP_ERRNO_EALREADY
    case IP_ERRNO_EALREADY:	return "Operation already in progress";
#endif
#ifdef IP_ERRNO_ENOTSOCK
    case IP_ERRNO_ENOTSOCK:	return "Socket operation on non-socket";
#endif
#ifdef IP_ERRNO_EDESTADDRREQ
    case IP_ERRNO_EDESTADDRREQ:	return "Destination address required";
#endif
#ifdef IP_ERRNO_EMSGSIZE
    case IP_ERRNO_EMSGSIZE:	return "Message too long";
#endif
#ifdef IP_ERRNO_EPROTOTYPE
    case IP_ERRNO_EPROTOTYPE:	return "Protocol wrong type for socket";
#endif
#ifdef IP_ERRNO_ENOPROTOOPT
    case IP_ERRNO_ENOPROTOOPT:	return "Protocol not available";
#endif
#ifdef IP_ERRNO_EPROTONOSUPPORT
    case IP_ERRNO_EPROTONOSUPPORT: return "Protocol not supported";
#endif
#ifdef IP_ERRNO_ESOCKTNOSUPPORT
    case IP_ERRNO_ESOCKTNOSUPPORT: return "Socket type not supported";
#endif
#ifdef IP_ERRNO_EOPNOTSUPP
    case IP_ERRNO_EOPNOTSUPP:	return "Operation not supported";
#endif
#ifdef IP_ERRNO_EPFNOSUPPORT
    case IP_ERRNO_EPFNOSUPPORT:	return "Protocol family not supported";
#endif
#ifdef IP_ERRNO_EAFNOSUPPORT
    case IP_ERRNO_EAFNOSUPPORT:	return "Address family not supported by protocol family";
#endif
#ifdef IP_ERRNO_EADDRINUSE
    case IP_ERRNO_EADDRINUSE:	return "Address already in use";
#endif
#ifdef IP_ERRNO_EADDRNOTAVAIL
    case IP_ERRNO_EADDRNOTAVAIL:return "Can't assign requested address";
#endif
#ifdef IP_ERRNO_ENETDOWN
    case IP_ERRNO_ENETDOWN:	return "Network is down";
#endif
#ifdef IP_ERRNO_ENETUNREACH
    case IP_ERRNO_ENETUNREACH:	return "Network is unreachable";
#endif
#ifdef IP_ERRNO_ENETRESET
    case IP_ERRNO_ENETRESET:	return "Network dropped connection on reset";
#endif
#ifdef IP_ERRNO_ECONNABORTED
    case IP_ERRNO_ECONNABORTED:	return "Software caused connection abort";
#endif
#ifdef IP_ERRNO_ECONNRESET
    case IP_ERRNO_ECONNRESET:	return "Connection reset by peer";
#endif
#ifdef IP_ERRNO_ENOBUFS
    case IP_ERRNO_ENOBUFS:	return "No buffer space available";
#endif
#ifdef IP_ERRNO_EISCONN
    case IP_ERRNO_EISCONN:	return "Socket is already connected";
#endif
#ifdef IP_ERRNO_ENOTCONN
    case IP_ERRNO_ENOTCONN:	return "Socket is not connected";
#endif
#ifdef IP_ERRNO_ESHUTDOWN
    case IP_ERRNO_ESHUTDOWN:	return "Can't send after socket shutdown";
#endif
#ifdef IP_ERRNO_ETOOMANYREFS
    case IP_ERRNO_ETOOMANYREFS:	return "Too many references: can't splice";
#endif
#ifdef IP_ERRNO_ETIMEDOUT
    case IP_ERRNO_ETIMEDOUT:	return "Operation timed out";
#endif
#ifdef IP_ERRNO_ECONNREFUSED
    case IP_ERRNO_ECONNREFUSED:	return "Connection refused";
#endif
#ifdef IP_ERRNO_ELOOP
    case IP_ERRNO_ELOOP:	return "Too many levels of symbolic links";
#endif
#ifdef IP_ERRNO_ENAMETOOLONG
    case IP_ERRNO_ENAMETOOLONG:	return "File name too long";
#endif
#ifdef IP_ERRNO_EHOSTDOWN
    case IP_ERRNO_EHOSTDOWN:	return "Host is down";
#endif
#ifdef IP_ERRNO_EHOSTUNREACH
    case IP_ERRNO_EHOSTUNREACH:	return "No route to host";
#endif
#ifdef IP_ERRNO_ENOTEMPTY
    case IP_ERRNO_ENOTEMPTY:	return "Directory not empty";
#endif
#ifdef IP_ERRNO_EPROCLIM
    case IP_ERRNO_EPROCLIM:	return "Too many processes";
#endif
#ifdef IP_ERRNO_EUSERS
    case IP_ERRNO_EUSERS:	return "Too many users";
#endif
#ifdef IP_ERRNO_EDQUOT
    case IP_ERRNO_EDQUOT:	return "Disc quota exceeded";
#endif
#ifdef IP_ERRNO_ESTALE
    case IP_ERRNO_ESTALE:	return "Stale NFS file handle";
#endif
#ifdef IP_ERRNO_EREMOTE
    case IP_ERRNO_EREMOTE:	return "Too many levels of remote in path";
#endif
#ifdef IP_ERRNO_EBADRPC
    case IP_ERRNO_EBADRPC:	return "RPC struct is bad";
#endif
#ifdef IP_ERRNO_ERPCMISMATCH
    case IP_ERRNO_ERPCMISMATCH:	return "RPC version wrong";
#endif
#ifdef IP_ERRNO_EPROGUNAVAIL
    case IP_ERRNO_EPROGUNAVAIL:	return "RPC prog. not avail";
#endif
#ifdef IP_ERRNO_EPROGMISMATCH
    case IP_ERRNO_EPROGMISMATCH:return "Program version wrong";
#endif
#ifdef IP_ERRNO_EPROCUNAVAIL
    case IP_ERRNO_EPROCUNAVAIL:	return "Bad procedure for program";
#endif
#ifdef IP_ERRNO_ENOLCK
    case IP_ERRNO_ENOLCK:	return "No locks available";
#endif
#ifdef IP_ERRNO_ENOSYS
    case IP_ERRNO_ENOSYS:	return "Function not implemented";
#endif
#ifdef IP_ERRNO_EFTYPE
    case IP_ERRNO_EFTYPE:	return "Inappropriate file type or format";
#endif
#ifdef IP_ERRNO_EAUTH
    case IP_ERRNO_EAUTH:	return "Authentication error";
#endif
#ifdef IP_ERRNO_ENEEDAUTH
    case IP_ERRNO_ENEEDAUTH:	return "Need authenticator";
#endif
#ifdef IP_ERRNO_EIDRM
    case IP_ERRNO_EIDRM:	return "Identifier removed";
#endif
#ifdef IP_ERRNO_ENOMSG
    case IP_ERRNO_ENOMSG:	return "No message of desired type";
#endif
#ifdef IP_ERRNO_EOVERFLOW
    case IP_ERRNO_EOVERFLOW:	return "Value too large for data type";
#endif
#ifdef IP_ERRNO_EILSEQ
    case IP_ERRNO_EILSEQ:	return "Illegal byte sequence";
#endif
#ifdef IP_ERRNO_EINTERNAL
    case IP_ERRNO_EINTERNAL:	return "Internal error";
#endif
#if defined(IP_ERRNO_EWOULDBLOCK) && IP_ERRNO_EWOULDBLOCK != IP_ERRNO_EAGAIN
    case IP_ERRNO_EWOULDBLOCK: return "Operation would block";
#endif

    default:
        return IP_NULL;
    }
}


/*
 *===========================================================================
 *                    ipcom_strerror_int_vxworks
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 60
IP_STATIC char *
ipcom_strerror_int_vxworks(int errnum)
{
    char *name;

#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65 && USE_STRERROR_IF

#ifdef IP_USE_TRADITIONAL_STRERROR
    if (errnum > 0x0000ffff)
        name = strerrorIf(errnum);
    else
        name = ipcom_strerror_int(errnum);
#else
    name = strerrorIf(errnum);
#endif /* IP_USE_TRADITIONAL_STRERROR */

#else /* IP_PORT_VXWORKS < 65 */

    if (errnum > 0x0000ffff)
    {
        name = NULL;

        if ((_func_symFindSymbol != (FUNCPTR) NULL) && statSymTbl != NULL)
        {
            char *statName;
            void *value;
            SYMBOL_ID   symId;

            (* _func_symFindSymbol) (statSymTbl, NULL, (void *)errnum,
                                     SYM_MASK_NONE, SYM_MASK_NONE, &symId);
            (* _func_symNameGet) (symId, &statName);
            (* _func_symValueGet) (symId, &value);

            if (value == (void *)errnum)
                name = statName;
        }
    }
    else
        name = ipcom_strerror_int(errnum);

#endif /* IP_PORT_VXWORKS < 65 */

    return name;
}
#endif /* IP_PORT_VXWORKS */

#endif /* IPCOM_USE_STRERROR_CODE */


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_strerror_r
 *===========================================================================
 * Description: Reentrant version of ipcom_strerror()
 * Parameters: <errnum> is the error code.
 *             <buf> points to a buffer of length <buflen> in which to
 *                   return the error string corresponding to <errnum>.
 * Returns: 0 on success, -1 if <buf> is NULL, or ERANGE if buflen is
 *          too small to hold the error string.
 */
#if defined(IPCOM_STRERROR_R) && IPCOM_STRERROR_R == 1
IP_PUBLIC int
ipcom_strerror_r(int errnum, char * buf, Ip_size_t buflen)
{
    char * name;
    int n;

    if (buf == NULL)
        return -1;

#if IP_PORT_VXWORKS >= 60
    name = ipcom_strerror_int_vxworks(errnum);
#else
    name = ipcom_strerror_int(errnum);
#endif

    if (name)
    {
        ipcom_strncpy(buf, name, buflen);
        buf[buflen-1] = '\0';
        n = ipcom_strlen(name);
    }
    else
    {

#ifdef IP_PORT_VXWORKS
        n = ipcom_snprintf(buf, buflen, "errno = %#x", errnum);
#else
        n = ipcom_snprintf(buf, buflen, "Unknown error: %d", 0xffff & errnum);
#endif
    }

    return (n < buflen) ? 0 : IP_ERRNO_ERANGE;
}
#endif /* #if defined(IPCOM_STRERROR_R) && IPCOM_STRERROR_R == 1 */


/*
 *===========================================================================
 *                    ipcom_strerror
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_STRERROR) && IPCOM_STRERROR == 1
IP_PUBLIC char *
ipcom_strerror(int errnum)
{
    static char buf[128];
    char *name;

#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 60
    name = ipcom_strerror_int_vxworks(errnum);
#else
    name = ipcom_strerror_int(errnum);
#endif
    if (name != IP_NULL)
        return name;

#ifdef IP_PORT_VXWORKS
    ipcom_sprintf(buf, "errno = %#x", errnum);
#else
    ipcom_sprintf(buf, "Unknown error: %d", 0xffff & errnum);
#endif

    return buf;
}
#endif /* #if defined(IPCOM_STRERROR) && IPCOM_STRERROR == 1 */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
