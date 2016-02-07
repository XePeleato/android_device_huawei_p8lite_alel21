/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ipcom2bsd.h,v $ $Revision: 1.5 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_ipcom2bsd.h,v $
 *     $Author: kenneth $ $Date: 2007-02-16 15:38:46 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Maps the IPCOM socket API to the VxWorks native socket API (sockLib).
 * This file must always be included as the last header file in every file
 * that want to use the VxWorks native socket API through IPCOM.
 * NOTE: remeber to include ipcom_ipcom2bsd_finish.h last in the C file that
 * uses this header.
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


#ifndef IPCOM_IPCOM2BSD
#define IPCOM_IPCOM2BSD

#ifdef __cplusplus
extern "C" {
#endif

#include <inetLib.h>
#include <selectLib.h>
#include <sockLib.h>
#include <sys/stat.h>

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Arrange for test applications to use sockLib */

/*
 * Remap constant definitions
 */
#ifdef IP_FD_CLR
# undef IP_FD_CLR
#endif
#ifdef IP_FD_ISSET
# undef IP_FD_ISSET
#endif
#ifdef IP_FD_SET
# undef IP_FD_SET
#endif
#ifdef IP_FD_ZERO
# undef IP_FD_ZERO
#endif
#ifdef Ip_stat
# undef Ip_stat
#endif

#define IP_FD_CLR   FD_CLR
#define IP_FD_ISSET FD_ISSET
#define IP_FD_SET   FD_SET
#define IP_FD_ZERO  FD_ZERO

/* Required in order to implement FD_ZERO */
#define bzero(buffer, nbytes) ipcom_memset(buffer, 0, nbytes)


/*
 * Remap type definitions
 */
#define Ip_cmsghdr      cmsghdr
#define Ip_fd_set       fd_set
#define Ip_iovec        iovec
#define Ip_msghdr       msghdr
#define Ip_sockaddr     sockaddr
#define Ip_sockaddr_in  sockaddr_in
#define Ip_sockaddr_in6 sockaddr_in6
#define Ip_socklen_t    int
#define Ip_timeval      timeval
#define Ip_stat         stat

/*
 * Remap socket function calls
 */
#ifdef ipcom_accept
# undef ipcom_accept
#endif
#ifdef ipcom_bind
# undef ipcom_bind
#endif
#ifdef ipcom_connect
# undef ipcom_connect
#endif
#ifdef ipcom_getpeername
# undef ipcom_getpeername
#endif
#ifdef ipcom_getsockname
# undef ipcom_getsockname
#endif
#ifdef ipcom_getsockopt
# undef ipcom_getsockopt
#endif
#ifdef ipcom_listen
# undef ipcom_listen
#endif
#ifdef ipcom_recv
# undef ipcom_recv
#endif
#ifdef ipcom_recvfrom
# undef ipcom_recvfrom
#endif
#ifdef ipcom_recvmsg
# undef ipcom_recvmsg
#endif
#ifdef ipcom_socketselect
# undef ipcom_socketselect
#endif
#ifdef ipcom_send
# undef ipcom_send
#endif
#ifdef ipcom_sendmsg
# undef ipcom_sendmsg
#endif
#ifdef ipcom_sendto
# undef ipcom_sendto
#endif
#ifdef ipcom_setsockopt
# undef ipcom_setsockopt
#endif
#ifdef ipcom_shutdown
# undef ipcom_shutdown
#endif
#ifdef ipcom_socket
# undef ipcom_socket
#endif
#ifdef ipcom_socketclose
# undef ipcom_socketclose
#endif
#ifdef ipcom_socketioctl
# undef ipcom_socketioctl
#endif
#ifdef ipcom_socketread
# undef ipcom_socketread
#endif
#ifdef ipcom_socketwrite
# undef ipcom_socketwrite
#endif

/* Direct all ipcom socket calls to the sockLib */
#define ipcom_accept       accept
#define ipcom_bind         bind
#define ipcom_connect      connect
#define ipcom_getpeername  getpeername
#define ipcom_getsockname  getsockname
#define ipcom_getsockopt   getsockopt
#define ipcom_listen       listen
#define ipcom_recv         recv
#define ipcom_recvfrom     recvfrom
#define ipcom_recvmsg      recvmsg
#define ipcom_send         send
#define ipcom_sendmsg      sendmsg
#define ipcom_sendto       sendto
#define ipcom_setsockopt   setsockopt
#define ipcom_shutdown     shutdown
#define ipcom_socket       socket
#define ipcom_socketclose  close
#define ipcom_socketread   read
#define ipcom_socketselect select
#define ipcom_socketwrite  write
/* Cast to long instead of int in the 3:rd argument so we will
   get build error (or at least a warning) when building for
   64-bit where using an int is broken (sizeof(int) != sizeof(void*)
   on most 64-bit arch) */
#define ipcom_socketioctl(fd, function, opt)  ioctl(fd, function, (long) opt)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


#ifdef __cplusplus
}
#endif

#endif /* IPCOM_IPCOM2BSD */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
