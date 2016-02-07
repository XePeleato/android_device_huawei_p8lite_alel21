/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_psock.c,v $ $Revision: 1.34 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_psock.c,v $
 *     $Author: kenneth $ $Date: 2008-12-18 14:27:55 $
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
 * ipcom_sock.c contains source code for BSD sockets functions calls.
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

#if IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE || defined(IPCOM_VXWORKS_TEST_SOCKET_BACKEND) || defined(IPCOM_USE_NATIVE_SOCK_API)

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */
#include <ipcom_sock.h>
#include <ipcom_clib.h>
#include <ipcom_syslog.h>
#include <ipcom_os.h>
#include <ipcom_ipcom2coreip.h>

#include <vxWorks.h>
#include <sockLib.h>
#include <selectLib.h>
#include <inetLib.h>
#include <ioLib.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <sys/ioctl.h>
#include <version.h>
#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6
#include <net/if.h>
#endif
#ifdef IP_DEBUG
#include <stdio.h>
#endif


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM


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
 *===========================================================================
 *                    ipcom_psock_ipsockopt_to_sockopt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_psock_ipoptname_to_optname(int optname)
{
    switch(optname)
    {
    case IP_SO_REUSEADDR:
        optname = SO_REUSEADDR;
        break;
    case IP_SO_BROADCAST:
        optname = SO_BROADCAST;
        break;
    case IP_SO_REUSEPORT:
        optname = SO_REUSEPORT;
        break;
    case IP_SO_DONTROUTE:
        optname = SO_DONTROUTE;
        break;
    case IP_SO_SNDBUF:
        optname = SO_SNDBUF;
        break;
    case IP_SO_RCVBUF:
        optname = SO_RCVBUF;
        break;
    case IP_SO_RCVTIMEO:
        optname = SO_RCVTIMEO;
        break;
    case IP_SO_ERROR:
        optname = SO_ERROR;
        break;
    case IP_SO_TYPE:
        optname = SO_TYPE;
        break;
    case IP_SO_KEEPALIVE:
        optname = SO_KEEPALIVE;
        break;
    default:
        IP_PANIC2();
        break;
    }
    return optname;
}


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#ifdef IP_DEBUG
IP_STATIC int ipcom_psock_tot_no_socks = 0;
IP_STATIC int ipcom_curr_no_socks = 0;
#endif


/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */

#ifdef IP_DEBUG
IP_PUBLIC void
ipcom_show_sock_stat(void)
{
    ipcom_printf("total no of sockets created: %d\n", ipcom_psock_tot_no_socks);
    ipcom_printf("current no of sockets in use: %d\n", ipcom_curr_no_socks);
}
#endif


/*
 *===========================================================================
 *                    ipcom_socket
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_fd
ipcom_socket(int domain, int type, int protocol)
{
#ifdef IP_DEBUG
    Ip_fd sock;

    ipcom_psock_tot_no_socks++;
    ipcom_curr_no_socks++;
    sock = (Ip_fd)socket((int)domain, (int)type, (int)protocol);

    return sock;
#else
    return (Ip_fd)socket((int)domain, (int)type, (int)protocol);
#endif
}


/*
 *===========================================================================
 *                    ipcom_socketclose
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_socketclose(Ip_fd fd)
{
    int ret;
#ifdef IP_DEBUG
    ipcom_curr_no_socks--;
#endif

    IPCOM_LOG1(DEBUG2, "ipcom_socketclose(%d)", fd);

    ret = (int)close((int)fd);
    if (ret < 0)
    {
        IPCOM_LOG2(DEBUG2, "ipcom_socketclose(%d) :: failed, errno=%d", fd, errno);
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_shutdown
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_shutdown(Ip_fd fd, int how)
{
    int ret;

    IPCOM_LOG1(DEBUG2, "ipcom_shutdown(%d)", fd);

    ret = shutdown((int)fd, how);
    if (ret < 0)
    {
        IPCOM_LOG2(DEBUG2, "ipcom_shutdown(%d) :: failed, errno=%d", fd, errno);
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_bind
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_bind(Ip_fd fd, IP_CONST struct Ip_sockaddr *addr, Ip_socklen_t addrlen)
{
    int ret;

    ret = (int)bind((int)fd, (struct sockaddr *)addr, (int)addrlen);
    if (ret < 0)
    {
        IPCOM_LOG2(DEBUG2, "ipcom_bind(%d) :: failed, errno=%d", fd, errno);
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_connect
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_connect(Ip_fd sock, IP_CONST struct Ip_sockaddr *addr, Ip_socklen_t addrlen)
{
    return (int)connect((int)sock, (struct sockaddr *)addr, (int)addrlen);
}


/*
 *===========================================================================
 *                    ipcom_listen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_listen(Ip_fd sock, int backlog)
{
    return (int)listen((int)sock, (int) backlog);
}


/*
 *===========================================================================
 *                    ipcom_accept
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_accept(Ip_fd sock, struct Ip_sockaddr *addr, Ip_socklen_t *addrlenp)
{
    int tmp_sock;

    tmp_sock = (int)accept((int)sock, (struct sockaddr *)addr, (int *)addrlenp);

#ifdef IP_DEBUG
    ipcom_curr_no_socks++;
    ipcom_psock_tot_no_socks++;
#endif

    return tmp_sock;
}


/*
 *===========================================================================
 *                    ipcom_setsockopt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_setsockopt(Ip_fd sock, int level, int optname, const void *optval, Ip_socklen_t optlen)
{
    optname = ipcom_psock_ipoptname_to_optname(optname);

    return setsockopt((int)sock, level, optname, (void*)optval, (int)optlen);
}


/*
 *===========================================================================
 *                    ipcom_getsockopt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getsockopt(Ip_fd sock, int level, int optname, void *optval, Ip_socklen_t *optlenp)
{
    optname = ipcom_psock_ipoptname_to_optname(optname);

    return (int)getsockopt((int)sock, (int)level, (int)optname, optval, (int *)optlenp);
}


/*
 *===========================================================================
 *                    ipcom_getsockname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getsockname(Ip_fd sock, struct Ip_sockaddr *addr, Ip_socklen_t *addrlenp)
{
    return getsockname((int)sock, (struct sockaddr *)addr, (int*)addrlenp);
}


/*
 *===========================================================================
 *                    ipcom_getpeername
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getpeername(Ip_fd sock, struct Ip_sockaddr *addr, Ip_socklen_t *addrlenp)
{
    return getpeername((int)sock, (struct sockaddr *)addr, (int*)addrlenp);
}


/*
 *===========================================================================
 *                    ipcom_socketwrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_socketwrite(Ip_fd sock, IP_CONST void *buf, Ip_size_t nbytes)
{
    return ipcom_send(sock, buf, nbytes, 0);
}


/*
 *===========================================================================
 *                    ipcom_socketwritev
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_socketwritev(Ip_fd fd, IP_CONST struct Ip_iovec *iov, int iovlen)
{
    return (Ip_ssize_t) writev((int) fd, (struct iovec *) iov, iovlen);
}


/*
 *===========================================================================
 *                    ipcom_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_send(Ip_fd sock, IP_CONST void *msg, Ip_size_t len, int flags)
{
    return (Ip_ssize_t)send((int)sock, msg, (int)len, (int)flags);
}


/*
 *===========================================================================
 *                    ipcom_sendto
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_sendto(Ip_fd sock, IP_CONST void *msg, Ip_size_t len, int flags, const struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    return (Ip_ssize_t)sendto((int)sock, (void*)msg, (int)len, (int)flags, (struct sockaddr *)to, (int)tolen);
}


/*
 *===========================================================================
 *                    ipcom_socketread
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_socketread(Ip_fd sock, void *buf, Ip_size_t nbytes)
{
    return (Ip_ssize_t)recv((int)sock, buf, (int)nbytes, 0);
}


/*
 *===========================================================================
 *                    ipcom_recv
 *===========================================================================
 */
IP_PUBLIC Ip_ssize_t
ipcom_recv(Ip_fd fd, void *buf, Ip_size_t len, int flags)
{
    return recv((int) fd, (char*) buf, (int) len, flags);
}


/*
 *===========================================================================
 *                    ipcom_recvfrom
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_recvfrom(Ip_fd sock, void *buf, Ip_size_t len, int flags, struct Ip_sockaddr *from, Ip_socklen_t *fromlenp)
{
    return (Ip_ssize_t)recvfrom((int)sock, buf, (int)len, flags, (struct sockaddr *)from, (int *)fromlenp);
}


/*
 *===========================================================================
 *                    ipcom_recvmsg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_recvmsg(Ip_fd sock, struct Ip_msghdr *msg, int flags)
{
    return (Ip_ssize_t)recvmsg((int)sock, (struct msghdr*)msg, flags);
}

/*
 *===========================================================================
 *                    ipcom_sendmsg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_sendmsg(Ip_fd sock, IP_CONST struct Ip_msghdr *msg, int flags)
{
    return (Ip_ssize_t)sendmsg((int)sock, (struct msghdr*)msg, flags);
}


/*
 *===========================================================================
 *                    ipcom_socketselect
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_socketselect(int width, Ip_fd_set *rfds, Ip_fd_set *wfds, Ip_fd_set *exfds, struct Ip_timeval *tmo)
{
    int ret;

    ret = (int)select((int)width, (fd_set *)rfds, (fd_set *)wfds, (fd_set *)exfds, (struct timeval *)tmo);
    if (ret < 0)
    {
        IPCOM_LOG1(DEBUG2, "ipcom_socketselect() :: failed, errno=%d", errno);
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_socketioctl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_socketioctl(Ip_fd sock, unsigned long request, void *argp)
{
    /* the ioctl function is declared as
     *  int ioctl(int sock, int request, int arg);
     * in the header ioLib.h. This is also how it is described in
     * the manuals. However, it is wrong. The "standard" declaration
     * found on most other OSs should be used, i.e.
     *  int ioctl(int sock, int request, void* argp);
     */
    switch(request)
    {
        /* 'Ip_pid_t' ioctls: */
    case IP_SIOCSPGRP:
        return 0;
    case IP_SIOCGPGRP:
        *(Ip_pid_t *)argp = ipcom_getpid();
        return 0;

        /* 'int' ioctls: */
    case IP_FIONBIO:
        request = FIONBIO;
        break;
    case IP_FIONREAD:
        request = FIONREAD;
        break;

        /* 'struct Ip_ifreq' ioctls: */
    case IP_SIOCGIFADDR:
        request = SIOCGIFADDR;
        break;
    case IP_SIOCSIFADDR:
        request = SIOCSIFADDR;
        break;
    case IP_SIOCGIFBRDADDR:
        request = SIOCGIFBRDADDR;
        break;
    case IP_SIOCGIFNETMASK:
        request = SIOCGIFNETMASK;
        break;
    case IP_SIOCSIFNETMASK:
        request = SIOCSIFNETMASK;
        break;
    case IP_SIOCGIFDSTADDR:
        request = SIOCGIFDSTADDR;
        break;
    case IP_SIOCSIFDSTADDR:
        request = SIOCSIFDSTADDR;
        break;
    case IP_SIOCAIFADDR:
        request = SIOCAIFADDR;
        break;
    case IP_SIOCDIFADDR:
        request = SIOCDIFADDR;
        break;
    case IP_SIOCGIFFLAGS:
        request = SIOCGIFFLAGS; /* warning: do IFF flags match? */
        break;
    case IP_SIOCSIFFLAGS:
        request = SIOCSIFFLAGS; /* warning: do IFF flags match? */
        break;
    case IP_SIOCGIFMTU:
        request = SIOCGIFMTU;
        break;
    case IP_SIOCSIFMTU:
        request = SIOCSIFMTU;
        break;
#ifdef IP_SIOCGIFMETRIC
    case IP_SIOCGIFMETRIC:
        request = SIOCGIFMETRIC;
        break;
#endif
#ifdef IP_SIOCSIFMETRIC
    case IP_SIOCSIFMETRIC:
        request = SIOCSIFMETRIC;
        break;
#endif
    default:
        IP_PANIC2();
        break;
    }
    return ioctl((int)sock, request, (int)argp);
}


#if !defined(IPCOM_VXWORKS_TEST_SOCKET_BACKEND) && !defined(IPCOM_USE_NATIVE_SOCK_API)
/*
 *===========================================================================
 *                    ipcom_if_nametoindex
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u32
ipcom_if_nametoindex(IP_CONST char *ifname)
{
  IP_PANIC();
  return 0;
}


/*
 *===========================================================================
 *                    ipcom_if_indextoname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_if_indextoname(Ip_u32 ifindex, char *ifname)
{
  IP_PANIC();
  return IP_NULL;
}
#endif /* !defined(IPCOM_VXWORKS_TEST_SOCKET_BACKEND) && !defined(IPCOM_USE_NATIVE_SOCK_API) */

#else
int ipcom_psock_empty_file;
#endif /* IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE || defined(IPCOM_VXWORKS_TEST_SOCKET_BACKEND) */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


