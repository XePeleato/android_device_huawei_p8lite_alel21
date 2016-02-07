/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_windnet_compat.c,v $ $Revision: 1.111 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_windnet_compat.c,v $
 *   $Author: jyang $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This file implements a socket back-end. It provides binary compatibility
 * towards WindNet.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#define IPCOM_SKIP_NATIVE_SOCK_API

#include "ipcom_config.h"

#ifdef IPNET
#include <ipnet_config.h>
#include <ipnet_h.h>
#include <ipnet_usr_sock.h>
#endif

#ifdef IPLITE
#include <iplite_config.h>
#include <iplite_h.h>
#include <iplite_timeout.h>
#endif
#ifdef IPSCTP
#include <ipsctp_sock.h>
#endif

#include <ipcom_cstyle.h>

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_cstyle.h>

#ifdef IPCOM_USE_SOCK_COMPAT

#include <vxWorks.h>
#include <semLib.h>

/* IPCOM includes */
#include <ipcom_type.h>
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_vxworks.h>
#include <ipcom_netif.h>
#include <ipcom_sock.h>
#include <ipcom_pkt.h>

#undef s6_addr /* Remove the ipcom definition, vxWorks 6.0 has a conflicting macro definition */

#include <string.h>
#include <sockLib.h>
#include <sockFunc.h>
#include <stdlib.h>
#include <errno.h>
#include <taskLib.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <version.h>
#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6
#include <net/if.h>
#include <net/if_arp.h>
#include <net/socketvar.h>
#endif
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#undef ipcom_socketioctl
#include <iosLib.h>
#undef ipcom_socketselect
#include <selectLib.h>

#ifdef INCLUDE_NET_REM_IO
#include <private/funcBindP.h>
#include <private/ftpLibP.h>
#include <netDrv.h>
#include <remLib.h>
#endif

#define IPNET_HAVE_UINT_TYPE
#if IP_PORT_VXWORKS >= 55
#define IPNET_HAVE_UINT64_TYPE
#endif

#ifdef INCLUDE_DHCPC
#error INCLUDE_DHCPC_not_supported_Please_use_dhcp_client_from_Interpeak_ipappl
#endif

#if IP_PORT_VXWORKS < 65
#define IPCOM_WINDNET_IFNAME_REMAP
#else
#undef IPCOM_WINDNET_IFNAME_REMAP
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifdef IPCOM_SYSLOG_PRIORITY
#undef IPCOM_SYSLOG_PRIORITY
#endif
#ifdef IPCOM_SYSLOG_FACILITY
#undef IPCOM_SYSLOG_FACILITY
#endif
#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6

/* The struct socket member the IPNET/IPLITE fd is stored in */
#if IP_PORT_VXWORKS >= 65
#define IP_FD_MEMBER       so_bkendaux
#else
#define IP_FD_MEMBER       so_upcallarg
#endif

#define IP_FD_MEMBER_TYPE  void *
#define IP_VX_FD_TYPE      struct socket*
#define IP_GET_IP_FD(fd)   ((Ip_fd)fd->IP_FD_MEMBER)
#else
#define IP_VX_FD_TYPE      int
#define IP_GET_IP_FD(fd)   ((Ip_fd)iosFdValue(fd))
#endif


/* Read/write/except indexes in the select_set which is used by our
   windnet select implementation */
#define IPCOM_VX_RD_SET 0
#define IPCOM_VX_WR_SET 1
#define IPCOM_VX_EX_SET 2

/* Select types */
#define IPCOM_VX_SELECT_TYPE_RD  (1 << 0)
#define IPCOM_VX_SELECT_TYPE_WR  (1 << 1)
#define IPCOM_VX_SELECT_TYPE_EX  (1 << 2)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/* From <WIND_BASE>/target/h/private/funcBindP.h.
 * Needed to get remote IO to work over IPNET/IPLITE
 */
extern FUNCPTR     _func_netLsByName;
extern STATUS ftpLs (char *dirName);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/* Select related functions */
static void ipcom_windnet_socket_eventcb(Ipcom_socket *sock,
                                         IP_CONST Ipcom_pkt *pkt,
                                         enum Ipcom_socket_event_type event);
static int ipcom_windnet_select_add_node(int kernel_sock_fd, SEL_WAKEUP_NODE *swn);
static void ipcom_windnet_select_remove_node(int kernel_sock_fd, SEL_WAKEUP_NODE *swn);
static Ip_bool ipcom_windnet_select_poll(Ipnet_socket *sock,
                                         SELECT_TYPE select_type,
                                         Ip_bool owns_list_mutex);
#ifdef IPCOM_WINDNET_IFNAME_REMAP
static void ipcom_fix_ifname(struct ifreq *req, char *vx_ifname);
#endif


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#ifdef IPCOM_USE_INET6
const struct in6_addr in6addr_any      = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;
#endif

/* { level, Windnet socket option value, Interpeak option value }
 * IP option  0 = return ok, i.e. ignore option
 *           -1 = not supported, return -1 and set errno
 */
#if IP_PORT_VXWORKS < 65
static int sockopt_optnames[][3] =
{
    /* IP_SOL_SOCKET options: */
    { SOL_SOCKET, SO_DEBUG, 0 },
    { SOL_SOCKET, SO_ACCEPTCONN, IP_SO_ACCEPTCONN },
    /* IPSTACK and windnet has different opinions about the semantics for
     * REUSEADDR. Windnet's REUSEADDR matches IPSTACK's REUSEPORT
     */
    { SOL_SOCKET, SO_REUSEADDR,   IP_SO_REUSEPORT },
    { SOL_SOCKET, SO_KEEPALIVE,   IP_SO_KEEPALIVE },
    { SOL_SOCKET, SO_DONTROUTE,   IP_SO_DONTROUTE },
    { SOL_SOCKET, SO_BROADCAST,   IP_SO_BROADCAST },
    { SOL_SOCKET, SO_USELOOPBACK, IP_SO_USELOOPBACK },
    { SOL_SOCKET, SO_LINGER,      IP_SO_LINGER },
    { SOL_SOCKET, SO_OOBINLINE,   IP_SO_OOBINLINE },
    { SOL_SOCKET, SO_REUSEPORT,   IP_SO_REUSEPORT },
#ifdef SO_TIMESTAMP
    { SOL_SOCKET, SO_TIMESTAMP,   -1 },
#endif
#ifdef SO_LINGERRESET
    { SOL_SOCKET, SO_LINGERRESET, -1 },
#endif
#ifdef SO_VSLOCK
    { SOL_SOCKET, SO_VSLOCK,      -1 },
#endif
#ifdef SO_PRIVBUF
    { SOL_SOCKET, SO_PRIVBUF,     -1 },
#endif

    { SOL_SOCKET, SO_SNDBUF,      IP_SO_SNDBUF },
    { SOL_SOCKET, SO_RCVBUF,      IP_SO_RCVBUF },
    { SOL_SOCKET, SO_SNDLOWAT,    IP_SO_SNDLOWAT },
    { SOL_SOCKET, SO_RCVLOWAT,    IP_SO_RCVLOWAT },
    { SOL_SOCKET, SO_SNDTIMEO,    -1 },
    { SOL_SOCKET, SO_RCVTIMEO,    IP_SO_RCVTIMEO },
    { SOL_SOCKET, SO_ERROR,       IP_SO_ERROR },
    { SOL_SOCKET, SO_TYPE,        IP_SO_TYPE },
#ifdef SO_PROTOTYPE
    { SOL_SOCKET, SO_PROTOTYPE,   -1 }, /* WRS Depcreated */
#endif
#ifdef SO_CONNTIMEO
    { SOL_SOCKET, SO_CONNTIMEO,   -1 },
#endif

    /* IPPROTO_IP options: */
#ifdef IPCOM_USE_INET
    { IPPROTO_IP, IP_OPTIONS,          -1 },
    { IPPROTO_IP, IP_HDRINCL,          IP_IP_HDRINCL },
    { IPPROTO_IP, IP_TOS,              IP_IP_TOS },
    { IPPROTO_IP, IP_TTL,              IP_IP_TTL },
#ifdef IP_RECVOPTS
    { IPPROTO_IP, IP_RECVOPTS,         -1 },
#endif
#ifdef IP_RECVRETOPTS
    { IPPROTO_IP, IP_RECVRETOPTS,      -1 },
#endif
    { IPPROTO_IP, IP_RECVDSTADDR,      -1 },
#ifdef IP_RETOPTS
    { IPPROTO_IP, IP_RETOPTS,          -1 },
#endif
#ifdef IP_PKTINFO
    { IPPROTO_IP, IP_PKTINFO,          IP_IP_PKTINFO},
#endif
    { IPPROTO_IP, IP_MULTICAST_IF,     IP_IP_MULTICAST_IF },
    { IPPROTO_IP, IP_MULTICAST_TTL,    IP_IP_MULTICAST_TTL },
    { IPPROTO_IP, IP_MULTICAST_LOOP,   IP_IP_MULTICAST_LOOP },
    { IPPROTO_IP, IP_ADD_MEMBERSHIP,   IP_IP_ADD_MEMBERSHIP },
    { IPPROTO_IP, IP_DROP_MEMBERSHIP,  IP_IP_DROP_MEMBERSHIP },
#ifdef IP_MULTICAST_VIF
    { IPPROTO_IP, IP_MULTICAST_VIF,    -1 },
#endif
#ifdef IP_RSVP_ON
    { IPPROTO_IP, IP_RSVP_ON,          -1 },
#endif
#ifdef IP_RSVP_OFF
    { IPPROTO_IP, IP_RSVP_OFF,         -1 },
#endif
#ifdef IP_PORTRANGE
    { IPPROTO_IP, IP_PORTRANGE,        -1 },
#endif
#ifdef IP_RECVIF
    { IPPROTO_IP, IP_RECVIF,           IP_IP_RECVIF },
#endif
#ifdef IP_IPSEC_POLICY
    { IPPROTO_IP, IP_IPSEC_POLICY,     -1 },
#endif
#ifdef IP_FAITH
    { IPPROTO_IP, IP_FAITH,            -1 },
#endif
#ifdef IP_ROUTER_ALERT
    { IPPROTO_IP, IP_ROUTER_ALERT,     -1 /*diff usage? IP_IP_ROUTER_ALERT*/ },
#endif
#ifdef IP_DONTFRAG
    { IPPROTO_IP, IP_DONTFRAG,          IP_IP_DONTFRAG},
#endif
#ifdef IP_RECVTTL
    { IPPROTO_IP, IP_RECVTTL,           IP_IP_RECVTTL},
#endif
#ifdef IP_RECVTOS
    { IPPROTO_IP, IP_RECVTOS,           IP_IP_RECVTOS },
#endif
#ifdef IP_UDP_XCHKSUM
    { IPPROTO_IP, IP_UDP_XCHKSUM,       IP_UDP_X_CHKSUM },
#endif
#ifdef IP_NAT
    { IPPROTO_IP, IP_NAT,               IP_IP_NAT},
#endif
#ifdef IP_FW
    { IPPROTO_IP, IP_FW,                IP_IP_FW},
#endif
#ifdef IP_NEXTHOP
    { IPPROTO_IP, IP_NEXTHOP,           IP_IP_NEXTHOP },
#endif
#ifdef IP_X_SENDERLINKADDR
    { IPPROTO_IP, IP_X_SENDERLINKADDR,  IP_IP_X_SENDERLINKADDR},
#endif
#ifdef IP_X_VRID
    { IPPROTO_IP, IP_X_VRID,            IP_IP_X_VRID},
#endif
#endif /* IPCOM_USE_INET */

#if defined(IPCOM_USE_INET6) && defined(IPPROTO_IPV6)
    /* IPPROTO_IPV6 options: */
    { IPPROTO_IPV6, IPV6_UNICAST_HOPS,   IP_IPV6_UNICAST_HOPS },
    { IPPROTO_IPV6, IPV6_MULTICAST_IF,   IP_IPV6_MULTICAST_IF },
    { IPPROTO_IPV6, IPV6_MULTICAST_HOPS, IP_IPV6_MULTICAST_HOPS },
    { IPPROTO_IPV6, IPV6_MULTICAST_LOOP, IP_IPV6_MULTICAST_LOOP },
    { IPPROTO_IPV6, IPV6_JOIN_GROUP,     IP_IPV6_JOIN_GROUP },
    { IPPROTO_IPV6, IPV6_LEAVE_GROUP,    IP_IPV6_LEAVE_GROUP },
#ifdef IPV6_PORTRANGE
    { IPPROTO_IPV6, IPV6_PORTRANGE,      -1 },
#endif
    { IPPROTO_IPV6, ICMP6_FILTER,        IP_ICMP6_FILTER },
    { IPPROTO_IPV6, IPV6_PKTINFO,        IP_IPV6_PKTINFO },
    { IPPROTO_IPV6, IPV6_HOPLIMIT,       -1 /*not used with sockopt*/},
    { IPPROTO_IPV6, IPV6_NEXTHOP,        IP_IPV6_NEXTHOP },
    { IPPROTO_IPV6, IPV6_HOPOPTS,        IP_IPV6_HOPOPTS },
    { IPPROTO_IPV6, IPV6_DSTOPTS,        IP_IPV6_DSTOPTS },
    { IPPROTO_IPV6, IPV6_RTHDR,          IP_IPV6_RTHDR },
#ifdef IPV6_PKTOPTIONS /* VxWorks 6.2 does not define IPV6_PKTOPTIONS, but IPV6_2292PKTOPTIONS */
    { IPPROTO_IPV6, IPV6_PKTOPTIONS,     -1 },
#endif
    { IPPROTO_IPV6, IPV6_CHECKSUM,       IP_IPV6_CHECKSUM },
    { IPPROTO_IPV6, IPV6_V6ONLY,         IP_IPV6_V6ONLY },
#ifdef IPV6_BINDV6ONLY
    { IPPROTO_IPV6, IPV6_BINDV6ONLY,     IP_IPV6_V6ONLY },
#endif
#ifdef IPV6_IPSEC_POLICY
    { IPPROTO_IPV6, IPV6_IPSEC_POLICY,   -1 },
#endif
#ifdef IPV6_FAITH
    { IPPROTO_IPV6, IPV6_FAITH,          -1 },
#endif
#ifdef IPV6_RECVSAINFO
    { IPPROTO_IPV6, IPV6_RECVSAINFO,    IP_IPV6_RECVSAINFO},
#endif
#ifdef IPV6_ADDR_PREFERENCES
    { IPPROTO_IPV6, IPV6_ADDR_PREFERENCES, IP_IPV6_ADDR_PREFERENCES},
#endif
#ifdef IPV6_FW
    { IPPROTO_IPV6, IPV6_FW,            IP_IPV6_FW},
#endif
#ifdef IPV6_RECVIF
    { IPPROTO_IPV6, IPV6_RECVIF,        IP_IPV6_RECVIF},
#endif
#ifdef IPV6_X_SENDERLINKADDR
    { IPPROTO_IPV6, IPV6_X_SENDERLINKADDR,  IP_IPV6_X_SENDERLINKADDR},
#endif
#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_USE_TCP
    /* IPPROTO_TCP options: */
    { IPPROTO_TCP, TCP_NODELAY,          IP_TCP_NODELAY },
    { IPPROTO_TCP, TCP_MAXSEG,           -1 },
#ifdef TCP_NOPUSH
    { IPPROTO_TCP, TCP_NOPUSH,           -1 },
#endif
#ifdef TCP_NOOPT
    { IPPROTO_TCP, TCP_NOOPT,            -1 },
#endif
#endif /* IPCOM_USE_TCP */

    /* end marker */
    { -1, -1 , -1 }
};

/*
 * The following ioctl_requests[][3] which is used to map vxWorks ioctl
 * commands to IPNET is no longer used for VxWorks 6.5 release.
 *
 * ioctl_requests[2] = 1 -> convert interface name from mux to interface.
 */
static unsigned long ioctl_requests[][3] =
{
    { FIONBIO,  IP_FIONBIO,  0},
    { FIONREAD, IP_FIONREAD, 0},

    { SIOCATMARK, IP_SIOCATMARK, 0},
    { SIOCSPGRP,  IP_SIOCSPGRP, 0},
    { SIOCGPGRP,  IP_SIOCGPGRP, 0},

#ifdef SIOCADDRT
    { SIOCADDRT, IP_SIOCADDRT, 0 },
#endif
#ifdef SIOCDELRT
    { SIOCDELRT, IP_SIOCDELRT, 0 },
#endif
#ifdef SIOCSIFADDR
    { SIOCSIFADDR,  IP_SIOCSIFADDR, 1},
#endif
#ifdef OSIOCGIFADDR
    { OSIOCGIFADDR, 0xffffffff, 1},
#endif
#ifdef SIOCGIFADDR
    { SIOCGIFADDR,  IP_SIOCGIFADDR, 1},
#endif

#ifdef SIOCSIFDSTADDR
    { SIOCSIFDSTADDR,  IP_SIOCSIFDSTADDR, 1},
#endif
#ifdef OSIOCGIFDSTADDR
    { OSIOCGIFDSTADDR, 0xffffffff, 1},
#endif
#ifdef SIOCGIFDSTADDR
    { SIOCGIFDSTADDR,  IP_SIOCGIFDSTADDR, 1},
#endif

#ifdef SIOCSIFFLAGS
    { SIOCSIFFLAGS, IP_SIOCSIFFLAGS, 1},
#endif
#ifdef SIOCGIFFLAGS
    { SIOCGIFFLAGS, IP_SIOCGIFFLAGS, 1},
#endif

#ifdef OSIOCGIFBRDADDR
    { OSIOCGIFBRDADDR, 0xffffffff, 1},
#endif
#ifdef SIOCGIFBRDADDR
    { SIOCGIFBRDADDR,  IP_SIOCGIFBRDADDR, 1},
#endif
#ifdef SIOCSIFBRDADDR
    { SIOCSIFBRDADDR,  0, 1}, /*!!*/
#endif

#ifdef SIOCSARP
    { SIOCSARP, IP_SIOCSARP, 0},
#endif
#ifdef SIOCGARP
    { SIOCGARP, IP_SIOCGARP, 0},
#endif
#ifdef SIOCDARP
    { SIOCDARP, IP_SIOCDARP, 0},
#endif

#ifdef OSIOCGIFCONF
    { OSIOCGIFCONF, 0xffffffff, 0},
#endif
#ifdef SIOCGIFCONF
    { SIOCGIFCONF,  IP_SIOCGIFCONF, 0},
#endif

#ifdef OSIOCGIFNETMASK
    { OSIOCGIFNETMASK, 0xffffffff, 0},
#endif
#ifdef SIOCGIFNETMASK
    { SIOCGIFNETMASK,  IP_SIOCGIFNETMASK, 0},
#endif
#ifdef SIOCSIFNETMASK
    { SIOCSIFNETMASK,  IP_SIOCSIFNETMASK, 0},
#endif

#ifdef SIOCGIFMETRIC
#ifdef IP_SIOCGIFMETRIC
    { SIOCGIFMETRIC, IP_SIOCGIFMETRIC, 0},
#else
    { SIOCGIFMETRIC, 0xffffffff, 0},
#endif
#endif

#ifdef SIOCSIFMETRIC
#ifdef IP_SIOCSIFMETRIC
    { SIOCSIFMETRIC, IP_SIOCSIFMETRIC, 0},
#else
    { SIOCSIFMETRIC, 0xffffffff, 0},
#endif
#endif

#ifdef SIOCDIFADDR
    { SIOCDIFADDR, IP_SIOCDIFADDR, 0},
#endif
#ifdef SIOCAIFADDR
    { SIOCAIFADDR, IP_SIOCAIFADDR, 0},
#endif

#ifdef SIOCADDMULTI
    { SIOCADDMULTI, 0xffffffff, 0},
#endif
#ifdef SIOCDELMULTI
    { SIOCDELMULTI, 0xffffffff, 0},
#endif

#ifdef SIOCSIFMTU
    { SIOCSIFMTU, IP_SIOCSIFMTU, 1},
#endif
#ifdef SIOCGIFMTU
    { SIOCGIFMTU, IP_SIOCGIFMTU, 1},
#endif

#ifdef SIOCSIFASYNCMAP
    { SIOCSIFASYNCMAP,   0xffffffff, 0},
#endif
#ifdef SIOCGIFASYNCMAP
    { SIOCGIFASYNCMAP,   0xffffffff, 0},
#endif
#ifdef SIOCSIFASYNCFLAGS
    { SIOCSIFASYNCFLAGS, 0xffffffff, 0},
#endif
    /*!!{ SIOCMUXPASSTHRU,   0xffffffff, 0},*/

#ifdef SIOCGIFLLADDR
    { SIOCGIFLLADDR, IP_SIOCGIFLLADDR, 1 },
#endif /* SIOCGIFLLADDR */

    /* end marker */
    { 0xffffffff, 0xffffffff, 0 }
};

#endif /* IP_PORT_VXWORKS < 65 */


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_fix_ifname
 *===========================================================================
 * Description: vxWorks applications use the MUX names in the ioctl calls and
 *              IPNET/IPLITE requires IPNET/IPLITE names. This function maps
 *              MUX names to IPLITE/IPNET interface names.
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_WINDNET_IFNAME_REMAP
static void
ipcom_fix_ifname(struct ifreq *req, char *vx_ifname)
{
    const char *ip_if_name;

    ipcom_strcpy(vx_ifname, req->ifr_name);

    ip_if_name = ipcom_get_ip_if_name(req->ifr_name);
    if (ip_if_name != IP_NULL)
        ipcom_strcpy(req->ifr_name, ip_if_name);
    else
        IPCOM_LOG1(ERR, "ipcom_fix_ifname :: unknown MUX device name: %s",
                   req->ifr_name);
}
#endif /* IPCOM_WINDNET_IFNAME_REMAP */


/*
 *===========================================================================
 *                    ipcom_windnet_msgflags
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_INLINE int
ipcom_windnet_msgflags(int flags)
{
    /* This mapping is now 1-to-1 */
    return flags;
}


/*
 ****************************************************************************
 * 10                    I/O DRIVER FUNCTIONS
 ****************************************************************************
 */

#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6

/*
 *===========================================================================
 *                    ipcom_windnet_socketclose
 *===========================================================================
 */
static int
ipcom_windnet_socketclose(struct socket *sock)
{
    int ret;

    IPCOM_LOG2(DEBUG2, "ipcom_windnet_socketclose() :: fd=%d, sock=%x",
               (Ip_fd)sock->IP_FD_MEMBER, sock);

    ret = ipcom_socketclose((Ip_fd)sock->IP_FD_MEMBER);
    ipcom_free(sock);

    return ret;
}

#else

/*
 *===========================================================================
 *                    ipcom_windnet_socketclose
 *===========================================================================
 */
static int
ipcom_windnet_socketclose(int fd)
{
    IPCOM_LOG1(DEBUG2, "ipcom_windnet_socketclose() :: fd=%d", fd);

    /* Wind socket lib translates socket to ip_fd */
    return ipcom_socketclose((Ip_fd)fd);
}
#endif /* defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6 */


/*
 *===========================================================================
 *                    ipcom_windnet_socketwrite
 *===========================================================================
 */
static int
ipcom_windnet_socketwrite(IP_VX_FD_TYPE fd_in, char *buf, int len)
{
    Ip_fd fd;

#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6
    fd = IP_GET_IP_FD(fd_in);
#else
    fd = fd_in;
#endif

    return ipcom_send(fd, buf, len, 0);
}


/*
 *===========================================================================
 *                    ipcom_windnet_socketread
 *===========================================================================
 */
static int
ipcom_windnet_socketread(IP_VX_FD_TYPE fd_in, char *buf, int len)
{
    Ip_fd fd;

#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6
    fd = IP_GET_IP_FD(fd_in);
#else
    fd = fd_in;
#endif

    return ipcom_recv(fd, buf, len, 0);
}


/*
 *===========================================================================
 *                    ipcom_windnet_socketioctl
 *===========================================================================
 */
static int
ipcom_windnet_socketioctl(IP_VX_FD_TYPE fd_in, int request, int arg)
{
    Ip_fd fd;
    int ret = IPCOM_SUCCESS;
    SEL_WAKEUP_NODE *swn = (SEL_WAKEUP_NODE *) arg;

#ifdef IPCOM_WINDNET_IFNAME_REMAP
    char vx_ifname[IFNAMSIZ];
#endif


#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6
    fd = IP_GET_IP_FD(fd_in);
    IPCOM_LOG5(DEBUG2, "ipcom_windnet_socketioctl() :: so=%x, fd=%d, vx_fd=%d, request=%d(0x%x)",
               fd_in, fd, fd_in->so_fd, request, request);
#else
    fd = fd_in;
    IPCOM_LOG3(DEBUG2, "ipcom_windnet_socketioctl() :: fd=%d, request=%d(0x%x)",
               fd_in, request, request);
#endif

    switch(request)
    {
    case FIOSELECT:
        return ipcom_windnet_select_add_node(fd, swn);

    case FIOUNSELECT:
        ipcom_windnet_select_remove_node(fd, swn);
        return OK;

    case SIOCSHIWAT:    /* set high watermark */
    case SIOCGHIWAT:    /* get high watermark */
    case SIOCSLOWAT:    /* set low watermark */
    case SIOCGLOWAT:    /* get low watermark */
        ipcom_errno_set(IP_ERRNO_ENOPROTOOPT);
        return ERROR;

    case FIOFSTATGET:   /* get stat info - POSIX  */
        {
            struct stat *ub = (struct stat *)arg;
            struct socket * sock = fd_in;
            int  bytes;

            ipcom_memset((char *)ub, 0, sizeof(*ub));
            if (sock->so_oobmark)
                ub->st_mode = S_IFCHR; /* pretend to be a TTY */
            else
                ub->st_mode = S_IFSOCK;

            if (ipcom_socketioctl((Ip_fd)fd, IP_FIONREAD, (void *)&bytes) >= 0)
            {
                /*!! Socket is marked as always readable/writeable */
                ub->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
                ub->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
                ub->st_size = bytes;
                return OK;
            }
        }
        return ERROR;

#if IP_PORT_VXWORKS >= 66
    case FIOISATTY:
        {
            /* Hack - almost nothing in struct socket is used by this back
               end. We steal a bit. */
            struct socket * sock = fd_in;
            return sock->so_oobmark;
        }

#ifdef SIOCBEATTY
    case SIOCBEATTY:
        {
            /* Hack: Make a socket appear as a tty. */
            struct socket * sock = fd_in;
            int * pEnable = (int *) arg;
            if (pEnable == NULL)
                return ERROR;
            sock->so_oobmark = (*pEnable != 0);
            return OK;
        }
#endif /* SIOCBEATTY */
#endif

#if IP_PORT_VXWORKS >= 65
    case FIONBIO:       /* set non-blocking I/O; SOCKETS ONLY!*/
        request = IP_FIONBIO;
        break;

    case FIONREAD:      /* get num chars available to read */
        request = IP_FIONREAD;
        break;

    case SIOCATMARK:     /* at oob mark? */
        request = IP_SIOCATMARK;
        break;

    case SIOCSPGRP:     /* set process group */
        request = IP_SIOCSPGRP;
        break;

    case SIOCGPGRP:    /* get process group */
        request = IP_SIOCGPGRP;
        break;
#endif /* IP_PORT_VXWORKS >= 65 */

    default:

#if IP_PORT_VXWORKS < 65
        {
            unsigned long ip_request = (unsigned long)request;
            int i;

            for (i = 0; ioctl_requests[i][0] != 0xffffffff; i++)
            {
                /* map ioctl command if necessary */
                if (ip_request == ioctl_requests[i][0])
                {
                    ip_request = ioctl_requests[i][1];

#ifdef IPCOM_WINDNET_IFNAME_REMAP
                    if (ioctl_requests[i][2] == 1)
                        ipcom_fix_ifname((struct ifreq *)arg, vx_ifname);
#endif
                    ret = ipcom_socketioctl((Ip_fd)fd, ip_request, (void *)arg);
                    if (ret < 0)
                    {
                        IPCOM_LOG3(DEBUG2, "ipcom_windnet_socketioctl :: "
                                   "failed, fd=%x, fd=%d, errno:%d",
                                   fd_in, fd, ipcom_errno);
                    }
#ifdef IPCOM_WINDNET_IFNAME_REMAP
                    /* Restore MUX if name */
                    if (ioctl_requests[i][2] == 1)
                        ipcom_strcpy(((struct ifreq *)arg)->ifr_name,
                                     vx_ifname);
#endif
                    return ret;
                }
            }

            /* Not found */
            IPCOM_LOG1(DEBUG2, "ipcom_windnet_socketioctl :: unknown ioctl "
                       "option 0x%x", request);
            ipcom_errno_set(IP_ERRNO_ENOPROTOOPT);
            return ERROR;
        }
#endif /* IP_PORT_VXWORKS < 65 */

        break;  /* never gets here if IP_PORT_VXWORKS < 65 */
    }


#if IP_PORT_VXWORKS >= 65
    {
        /* If we are here, calls ipcom_socketioctl() unconditionally */

#ifdef IPCOM_WINDNET_IFNAME_REMAP
        Ip_bool remap = IP_FALSE;

        switch (request)
        {
        case SIOCSIFADDR:
        case SIOCGIFADDR:
        case SIOCGIFDSTADDR:
        case SIOCSIFFLAGS:
        case SIOCGIFFLAGS:
        case SIOCGIFBRDADDR:
        case SIOCSIFMTU:
        case SIOCGIFMTU:
#ifdef SIOCGIFLLADDR
        case SIOCGIFLLADDR:
#endif
            remap = IP_TRUE;
            ipcom_fix_ifname((struct ifreq *)arg, vx_ifname);
            break;
        default:
            break;
        }
#endif /* IPCOM_WINDNET_IFNAME_REMAP */

        ret = ipcom_socketioctl((Ip_fd)fd, request, (void *)arg);
        if (ret < 0)
        {
            IPCOM_LOG3(DEBUG2, "ipcom_windnet_socketioctl :: "
                       "failed, fd=%x, fd=%d, errno:%d", fd_in, fd, ipcom_errno);
        }

#ifdef IPCOM_WINDNET_IFNAME_REMAP
        if (remap)
            ipcom_strcpy(((struct ifreq *)arg)->ifr_name, vx_ifname);
#endif

    }
#endif /* IP_PORT_VXWORKS >= 65 */

    return ret;
}


/*
 ****************************************************************************
 * 11                    SOCKET BACK-END FUNCTIONS
 ****************************************************************************
 */

/* The socket backend API has been changed in vxWorks 6.0 */
#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6

/*
 *===========================================================================
 *                    ipcom_windnet_socket
 *===========================================================================
 */
static int
ipcom_windnet_socket(int domain, int type, int protocol, struct socket **so)
{
    int ip_fd;

    ip_fd = (int)ipcom_socket(domain, type, protocol);

    if (ip_fd != IP_INVALID_SOCKET)
    {
        struct socket *ret;

        ret = ipcom_calloc(1,sizeof(struct socket));
        if (ret == IP_NULL)
        {
            ipcom_socketclose(ip_fd);
            goto error;
        }

        ret->IP_FD_MEMBER = (IP_FD_MEMBER_TYPE)ip_fd;

        IPCOM_LOG4(DEBUG2, "ipcom_windnet_socket() :: fd=%d, domain=%d, type=%d, prot=%d",
                   ip_fd, domain, type, protocol);
#ifdef IPNET_SO_WAKEUP_HOOK
        ipcom_setsockopt(ip_fd, IP_SOL_SOCKET, IP_SO_X_WINDNET_SO, &ret, sizeof(ret));
#endif

        *so = ret;
        return OK;
    }
    else
    {
    error:
        IPCOM_LOG3(DEBUG2, "ipcom_windnet_socket() :: FAILED, domain=%d, type=%d, errno=%d",
                   domain, type, errno);
        return ERROR;
    }
}


/*
 *===========================================================================
 *                    ipcom_windnet_accept
 *===========================================================================
 */
#ifdef IPCOM_USE_TCP
static int
ipcom_windnet_accept(struct socket **so, struct sockaddr *addr, int *addrlen)
{
    Ip_fd fd;
    struct socket *listen_sock = *so;

    fd = ipcom_accept((Ip_fd)listen_sock->IP_FD_MEMBER,
                      (struct Ip_sockaddr *)addr,
                      (Ip_socklen_t *)addrlen);
    if (fd != IP_INVALID_SOCKET)
    {
        struct socket *new_sock;

        new_sock = ipcom_calloc(1, sizeof(struct socket));
        if (new_sock == IP_NULL)
        {
            ipcom_socketclose(fd);
            *so = IP_NULL;
            return ERROR;
        }

        IPCOM_LOG5(DEBUG2, "ipcom_windnet_accept() :: lst_so=%x, ip_lst_fd=%d, vx_lst_fd=%d, so=%x, fd=%d",
                   listen_sock, listen_sock->IP_FD_MEMBER, listen_sock->so_fd, new_sock, fd);

        new_sock->IP_FD_MEMBER = (IP_FD_MEMBER_TYPE)fd;

#ifdef IPNET_SO_WAKEUP_HOOK
        ipcom_setsockopt(fd, IP_SOL_SOCKET, IP_SO_X_WINDNET_SO, &new_sock, sizeof(new_sock));
#endif

        *so = new_sock;

        return OK;
    }

    return ERROR;
}
#endif /* IPCOM_USE_TCP */

#else /* defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6 */

/*
 *===========================================================================
 *                    ipcom_windnet_socket
 *===========================================================================
 */
static int
ipcom_windnet_socket(int domain, int type, int protocol)
{
    int ip_fd, ext_fd;

    ip_fd = (int)ipcom_socket(domain, type, protocol);
    if (ip_fd == -1)
        return -1;

    ext_fd = iosFdNew(&(ipcom_port.dev_hdr.dev_hdr), "(ipcom_socket)", 0);
    iosFdSet(ext_fd, &(ipcom_port.dev_hdr.dev_hdr),  "(ipcom_socket)", ip_fd);

    return ext_fd;
}


/*
 *===========================================================================
 *                    ipcom_windnet_accept
 *===========================================================================
 */
#ifdef IPCOM_USE_TCP
static int
ipcom_windnet_accept(int s, struct sockaddr *addr, int *addrlen)
{
    int fd, ext_fd;

    fd = ipcom_accept((Ip_fd)iosFdValue(s), (struct Ip_sockaddr*)addr, (Ip_socklen_t *)addrlen);
    if (fd == -1)
        return -1;

    ext_fd = iosFdNew(&(ipcom_port.dev_hdr.dev_hdr), "(ipcom_socket)", 0);
    iosFdSet(ext_fd, &(ipcom_port.dev_hdr.dev_hdr),  "(ipcom_socket)", fd);

    return ext_fd;
}
#endif /* IPCOM_USE_TCP */
#endif /* defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6 */


/*
 *===========================================================================
 *                    ipcom_windnet_bind
 *===========================================================================
 */
static int
ipcom_windnet_bind(IP_VX_FD_TYPE s, struct sockaddr *name, int namelen)
{
    IPCOM_LOG1(DEBUG2, "ipcom_windnet_bind() :: fd=%d", IP_GET_IP_FD(s));

    return ipcom_bind(IP_GET_IP_FD(s), (struct Ip_sockaddr *)name, (Ip_socklen_t)namelen);
}


/*
 *===========================================================================
 *                    ipcom_windnet_connect
 *===========================================================================
 */
static STATUS
ipcom_windnet_connect(IP_VX_FD_TYPE s, struct sockaddr *name, int namelen)
{
    IPCOM_LOG1(DEBUG2, "ipcom_windnet_connect() :: fd=%d", IP_GET_IP_FD(s));

    return ipcom_connect(IP_GET_IP_FD(s), (struct Ip_sockaddr *)name, (Ip_socklen_t)namelen);
}


/*
 *===========================================================================
 *                   ipcom_windnet_connect_with_timeout
 *===========================================================================
 * Description: Blocking connect with a specific upper limit on how long
 *              the call can block.
 * Parameters:  s - VxWorks socket.
 *              to - address of the node to connect to.
 *              to_len - length of the 'to' address
 *              tv - upper limit on how long this call might block,
 *                   NULL=infinite.
 * Returns:
 *
 */
static STATUS
ipcom_windnet_connect_with_timeout(IP_VX_FD_TYPE s,
                                   struct sockaddr *to,
                                   int to_len,
                                   struct timeval *tv)
{
    Ip_fd_set wr_set;
    STATUS    status = ERROR;
    int       non_blocking;
    int       on = 1;
    int       so_errno;
    Ip_socklen_t so_errno_len = sizeof(so_errno);
    int       sock = IP_GET_IP_FD(s);

    /* Read the current non-blocking status */
    if (ipcom_socketioctl(sock, IP_X_FIOGNBIO, &non_blocking) < 0)
        return status;

    if (ipcom_socketioctl(sock, IP_FIONBIO, &on) < 0)
        goto cleanup;

    if (ipcom_connect(sock, (struct Ip_sockaddr *)to, (Ip_socklen_t)to_len) < 0)
    {
        if (non_blocking)
            /* Cannot see why anyone would call this function on a
               non-blocking function, but make sure it behaves
               correctly in that case. */
            return ERROR;

        switch (ipcom_errno)
        {
        case IP_ERRNO_EINPROGRESS:
            /* Connection attempt is in progress, this is the expected
               return value of a non-blocking connect */
            break;
        default:
            goto cleanup;
        }
    }

    IP_FD_ZERO(&wr_set);
    IP_FD_SET(sock, &wr_set);
    switch (ipcom_socketselect(sock + 1,
                               IP_NULL,
                               &wr_set,
                               IP_NULL,
                               (struct Ip_timeval *) tv))
    {
    case 1:
        /* Check the error status of the socket */
        if (ipcom_getsockopt(sock,
                             IP_SOL_SOCKET,
                             IP_SO_ERROR,
                             &so_errno,
                             &so_errno_len) < 0)
            break;

        if (so_errno == 0)
            /* Success */
            status = OK;
        else
            ipcom_errno_set(so_errno);
        break;
    case 0:
        /* Connect timeout */
        ipcom_errno_set(IP_ERRNO_EWOULDBLOCK);
        break;
    default:
        /* Other error, errno already set */
        break;
    }

 cleanup:
    /* Restore non-blocking status */
    ipcom_socketioctl(sock, IP_FIONBIO, &non_blocking);
    return status;
}


/*
 *===========================================================================
 *                    ipcom_windnet_getpeername
 *===========================================================================
 */
static STATUS
ipcom_windnet_getpeername(IP_VX_FD_TYPE s, struct sockaddr *name, int *namelen)
{
    IPCOM_LOG1(DEBUG2, "ipcom_windnet_getpeername() :: fd=%d", IP_GET_IP_FD(s));

    return ipcom_getpeername(IP_GET_IP_FD(s), (struct Ip_sockaddr *) name, (Ip_socklen_t *)namelen);
}


/*
 *===========================================================================
 *                    ipcom_windnet_getsockname
 *===========================================================================
 */
static STATUS
ipcom_windnet_getsockname(IP_VX_FD_TYPE s, struct sockaddr *name, int *namelen)
{
    IPCOM_LOG1(DEBUG2, "ipcom_windnet_getsockname() :: fd=%d", IP_GET_IP_FD(s));

    return ipcom_getsockname(IP_GET_IP_FD(s), (struct Ip_sockaddr *)name, (Ip_socklen_t *)namelen);
}


/*
 *===========================================================================
 *                    ipcom_windnet_getsockopt
 *===========================================================================
 */
static STATUS
ipcom_windnet_getsockopt(IP_VX_FD_TYPE s, int level, int optname, char *optval, int *optlen)
{
    int ret;
#if IP_PORT_VXWORKS < 65
    int i;
    int ip_optname = -1;

    for (i = 0; sockopt_optnames[i][0] != -1; i++)
    {
        if (level != sockopt_optnames[i][0])
            continue;
        if (optname == sockopt_optnames[i][1])
        {
            ip_optname = sockopt_optnames[i][2];
            break;
        }
    }

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_getsockopt() :: fd=%d, level=%d, optname=%d",
               IP_GET_IP_FD(s), level, ip_optname);

    switch(ip_optname)
    {
    case 0:
        return OK;
    case -1:
        ipcom_errno_set(IP_ERRNO_ENOPROTOOPT);
        return -1;
    default:
        ret = ipcom_getsockopt(IP_GET_IP_FD(s), level, ip_optname, optval, (Ip_socklen_t *)optlen);
    }

#else
    IPCOM_LOG3(DEBUG2, "ipcom_windnet_getsockopt() :: fd=%d, level=%d, optname=%d",
               IP_GET_IP_FD(s), level, optname);

    ret = ipcom_getsockopt(IP_GET_IP_FD(s), level, optname, optval, (Ip_socklen_t *)optlen);
#endif

#ifdef IPSCTP
    if ((ERROR != ret)
#if IP_PORT_VXWORKS < 65
        && (IPSCTP_GET_PEELOFF == ip_optname)
#else
        && (IPSCTP_GET_PEELOFF == optname)
#endif
        && (IP_IPPROTO_SCTP == level))
    {
        struct Ipsctp_peeloff *po = IP_NULL;
        struct socket *new_sock;

        po = (struct Ipsctp_peeloff *)optval;
        if (IP_INVALID_SOCKET == po->fd)
            return ret;

        new_sock = ipcom_calloc(1, sizeof(struct socket));
        if (IP_NULL == new_sock)
        {
            ipcom_socketclose(po->fd);
            po->ret = IP_NULL;
            return ERROR;
        }

#if 0
        IPCOM_LOG5(DEBUG2, "ipcom_windnet_accept() :: lst_so=%x, ip_lst_fd=%d, vx_lst_fd=%d, so=%x, fd=%d",
                   listen_sock, listen_sock->IP_FD_MEMBER, listen_sock->so_fd, new_sock, fd);
#endif

        new_sock->IP_FD_MEMBER = (IP_FD_MEMBER_TYPE)(po->fd);

#ifdef IPNET_SO_WAKEUP_HOOK
        ipcom_setsockopt(po->fd, IP_SOL_SOCKET, IP_SO_X_WINDNET_SO, &new_sock, sizeof(new_sock));
#endif

        po->ret = (void *)new_sock;

        return OK;
    }
#endif

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_windnet_setsockopt
 *===========================================================================
 */
static STATUS
ipcom_windnet_setsockopt(IP_VX_FD_TYPE s, int level, int optname, char *optval, int optlen)
{
    if (level == IP_SOL_SOCKET && optname == IP_SO_X_EVENTCB_RO)
    {
        /* The socket fd returned in the callback is the kernel fd
           which is not the same as the fd seen by the user, so this
           socket option cannot be supported in a good way through the
           WindNet API. */
        ipcom_errno_set(IP_ERRNO_EOPNOTSUPP);
        return -1;
    }

#if IP_PORT_VXWORKS < 65
    {
        int i;
        int ip_optname = -1;

        for (i = 0; sockopt_optnames[i][0] != -1; i++)
        {
            if (level != sockopt_optnames[i][0])
                continue;
            if (optname == sockopt_optnames[i][1])
            {
                ip_optname = sockopt_optnames[i][2];
                break;
            }
        }

        IPCOM_LOG3(DEBUG2, "ipcom_windnet_setsockopt() :: fd=%d, level=%d, optname=%d",
                   IP_GET_IP_FD(s), level, ip_optname);

        switch(ip_optname)
        {
        case 0:
            return 0;
        case -1:
            ipcom_errno_set(IP_ERRNO_ENOPROTOOPT);
            return -1;
        default:
            return ipcom_setsockopt(IP_GET_IP_FD(s), level, ip_optname, optval, optlen);
        }
    }
#else
    IPCOM_LOG3(DEBUG2, "ipcom_windnet_setsockopt() :: fd=%d, level=%d, optname=%d",
               IP_GET_IP_FD(s), level, optname);

    return ipcom_setsockopt(IP_GET_IP_FD(s), level, optname, optval, (Ip_socklen_t)optlen);
#endif
}


/*
 *===========================================================================
 *                    ipcom_windnet_listen
 *===========================================================================
 */
#ifdef IPCOM_USE_TCP
static STATUS
ipcom_windnet_listen(IP_VX_FD_TYPE s, int backlog)
{
    IPCOM_LOG2(DEBUG2, "ipcom_windnet_listen() :: fd=%d, backlog=%d", IP_GET_IP_FD(s), backlog);

    return ipcom_listen(IP_GET_IP_FD(s), backlog);
}
#endif /* IPCOM_USE_TCP */


/*
 *===========================================================================
 *                    ipcom_windnet_recv
 *===========================================================================
 */
static int
ipcom_windnet_recv(IP_VX_FD_TYPE s, char *buf, int bufLen, int flags)
{
    int ret, ip_flags;

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_recv() :: fd=%d, buf_len=%d, flags=%d", IP_GET_IP_FD(s), bufLen, flags);

    ip_flags = ipcom_windnet_msgflags(flags);

    ret = ipcom_recv(IP_GET_IP_FD(s), buf, bufLen, ip_flags);

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_recv() :: fd=%d, read=%d, errno=%d",
               IP_GET_IP_FD(s), ret, ret == -1 ? ipcom_errno : 0);

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_windnet_recvfrom
 *===========================================================================
 */
static int
ipcom_windnet_recvfrom(IP_VX_FD_TYPE s, char *buf, int bufLen, int flags,
                       struct sockaddr *from, int *pFromLen)
{
    int ret, ip_flags;

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_recvfrom() :: fd=%d, buf_len=%d, flags=%d", IP_GET_IP_FD(s), bufLen, flags);

    ip_flags = ipcom_windnet_msgflags(flags);

    ret = ipcom_recvfrom(IP_GET_IP_FD(s),
                         buf,
                         bufLen,
                         ip_flags,
                         (struct Ip_sockaddr*)from,
                         (Ip_socklen_t *)pFromLen);

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_recvfrom() :: fd=%d, read=%d, errno=%d",
               IP_GET_IP_FD(s), ret, ret == -1 ? ipcom_errno : 0);

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_windnet_recvmsg
 *===========================================================================
 */
static int
ipcom_windnet_recvmsg(IP_VX_FD_TYPE s, struct msghdr *mp, int flags)
{
    int ret, ip_flags;

    IPCOM_LOG2(DEBUG2, "ipcom_windnet_recvmsg() :: fd=%d, flags=%d",
               IP_GET_IP_FD(s), flags);

    ip_flags = ipcom_windnet_msgflags(flags);

    ret = ipcom_recvmsg(IP_GET_IP_FD(s), (struct Ip_msghdr *)mp, ip_flags);

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_recvmsg() :: fd=%d, read=%d, errno=%d",
               IP_GET_IP_FD(s), ret, ret == -1 ? ipcom_errno : 0);

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_windnet_send
 *===========================================================================
 */
static int
ipcom_windnet_send(IP_VX_FD_TYPE s, const char *buf, int bufLen, int flags)
{
    int ret, ip_flags;

    /*IPCOM_LOG3(DEBUG2, "ipcom_windnet_send() :: fd=%d, buf_len=%d, flags=%d",
      IP_GET_IP_FD(s), bufLen, flags);*/

    ip_flags = ipcom_windnet_msgflags(flags);

    ret = (int)ipcom_send(IP_GET_IP_FD(s), buf, bufLen, ip_flags);

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_send() :: fd=%d, wrote=%d, errno=%d",
               IP_GET_IP_FD(s), ret, ret == -1 ? ipcom_errno : 0);

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_windnet_sendto
 *===========================================================================
 */
static int
ipcom_windnet_sendto(IP_VX_FD_TYPE s, caddr_t buf, int bufLen, int flags,
                      struct sockaddr *to, int tolen)
{
    int ret, ip_flags;

    /*IPCOM_LOG3(DEBUG2, "ipcom_windnet_sendto() :: fd=%d, buf_len=%d, flags=%d",
      IP_GET_IP_FD(s), bufLen, flags);*/

    ip_flags = ipcom_windnet_msgflags(flags);

    ret = ipcom_sendto(IP_GET_IP_FD(s), buf, bufLen, ip_flags, (struct Ip_sockaddr *)to, tolen);

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_sendto() :: fd=%d, wrote=%d, errno=%d",
               IP_GET_IP_FD(s), ret, ret == -1 ? ipcom_errno : 0);

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_windnet_sendmsg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
ipcom_windnet_sendmsg(IP_VX_FD_TYPE s, struct msghdr *mp, int flags)
{
    int ret, ip_flags;

    /*IPCOM_LOG2(DEBUG2, "ipcom_windnet_sendmsg() :: fd=%d, flags=%d",
      IP_GET_IP_FD(s), flags);*/

    ip_flags = ipcom_windnet_msgflags(flags);

    ret = ipcom_sendmsg(IP_GET_IP_FD(s), (struct Ip_msghdr*)mp, ip_flags);

    IPCOM_LOG3(DEBUG2, "ipcom_windnet_sendmsg() :: fd=%d, wrote=%d, errno=%d",
               IP_GET_IP_FD(s), ret, ret == -1 ? ipcom_errno : 0);

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_windnet_shutdown
 *===========================================================================
 */
static int
ipcom_windnet_shutdown(IP_VX_FD_TYPE s, int how)
{
    IPCOM_LOG2(DEBUG2, "ipcom_windnet_shutdown() :: fd=%d, how=%d",
               IP_GET_IP_FD(s), how);

    return ipcom_shutdown(IP_GET_IP_FD(s), how);
}


static SOCK_FUNC ipcom_windnet_sockfuncs =
{
    (FUNCPTR) NULL,             /* libInitRtn */
#ifdef IPCOM_USE_TCP
    (FUNCPTR) ipcom_windnet_accept,
#else
    (FUNCPTR) NULL,
#endif
    (FUNCPTR) ipcom_windnet_bind,
    (FUNCPTR) ipcom_windnet_connect,
    (FUNCPTR) ipcom_windnet_connect_with_timeout,
    (FUNCPTR) ipcom_windnet_getpeername,
    (FUNCPTR) ipcom_windnet_getsockname,
#ifdef IPCOM_USE_TCP
    (FUNCPTR) ipcom_windnet_listen,
#else
    (FUNCPTR) NULL,
#endif
    (FUNCPTR) ipcom_windnet_recv,
    (FUNCPTR) ipcom_windnet_recvfrom,
    (FUNCPTR) ipcom_windnet_recvmsg,
    (FUNCPTR) ipcom_windnet_send,
    (FUNCPTR) ipcom_windnet_sendto,
    (FUNCPTR) ipcom_windnet_sendmsg,
    (FUNCPTR) ipcom_windnet_shutdown,
    (FUNCPTR) ipcom_windnet_socket,
    (FUNCPTR) ipcom_windnet_getsockopt,
    (FUNCPTR) ipcom_windnet_setsockopt,
    (FUNCPTR) NULL,             /* zbufRtn, watch out 5.3 */

#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6
    (FUNCPTR) ipcom_windnet_socketclose,
    (FUNCPTR) ipcom_windnet_socketread,
    (FUNCPTR) ipcom_windnet_socketwrite,
    (FUNCPTR) ipcom_windnet_socketioctl,
#endif
};

#ifdef IPCOM_USE_VXSHELL_CMD

/*
 *===========================================================================
 *                    ipcom_windnet_socket_use_existing
 *===========================================================================
 */
int
ipcom_windnet_socket_use_existing(int ip_fd)
{
    return socket(IP_AF_WINDNET, SOCK_RAW, ip_fd);
}

/*
 *===========================================================================
 *                    ipcom_windnet_socket_af_windnet
 *===========================================================================
 */
static int
ipcom_windnet_socket_af_windnet(int domain, int type, int ip_fd, struct socket **so)
{
    struct socket *ret;

    if (ip_fd != IP_INVALID_SOCKET)
    {
        ret = ipcom_calloc(1,sizeof(struct socket));
        if (ret != IP_NULL)
        {
            ret->IP_FD_MEMBER = (IP_FD_MEMBER_TYPE)ip_fd;

            IPCOM_LOG3(DEBUG2, "ipcom_windnet_socket_af_windnet() :: fd=%d, domain=%d, type=%d",
                       ip_fd, domain, type);

            *so = ret;
            return OK;
        }
    }
    IPCOM_LOG3(DEBUG2, "ipcom_windnet_socket_af_windnet() :: FAILED, domain=%d, ip_socket=%d, errno=%d", domain, ip_fd, errno);

    return ERROR;
}

/*
 *===========================================================================
 *                    ipcom_windnet_socketclose_af_windnet
 *===========================================================================
 */
static int
ipcom_windnet_socketclose_af_windnet(struct socket *sock)
{
    IPCOM_LOG2(DEBUG2, "ipcom_windnet_socketclose_af_windnet() :: fd=%d, sock=%x",
               (Ip_fd)sock->IP_FD_MEMBER, sock);

    return OK;
}


static SOCK_FUNC ipcom_windnet_sockfuncs_af_windnet =
{
    (FUNCPTR) NULL,             /* libInitRtn */
#ifdef IPCOM_USE_TCP
    (FUNCPTR) ipcom_windnet_accept,
#else
    (FUNCPTR) NULL,
#endif
    (FUNCPTR) ipcom_windnet_bind,
    (FUNCPTR) ipcom_windnet_connect,
    (FUNCPTR) ipcom_windnet_connect_with_timeout,
    (FUNCPTR) ipcom_windnet_getpeername,
    (FUNCPTR) ipcom_windnet_getsockname,
#ifdef IPCOM_USE_TCP
    (FUNCPTR) ipcom_windnet_listen,
#else
    (FUNCPTR) NULL,
#endif
    (FUNCPTR) ipcom_windnet_recv,
    (FUNCPTR) ipcom_windnet_recvfrom,
    (FUNCPTR) ipcom_windnet_recvmsg,
    (FUNCPTR) ipcom_windnet_send,
    (FUNCPTR) ipcom_windnet_sendto,
    (FUNCPTR) ipcom_windnet_sendmsg,
    (FUNCPTR) ipcom_windnet_shutdown,
    (FUNCPTR) ipcom_windnet_socket_af_windnet,
    (FUNCPTR) ipcom_windnet_getsockopt,
    (FUNCPTR) ipcom_windnet_setsockopt,
    (FUNCPTR) NULL,             /* zbufRtn, watch out 5.3 */

#if defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR == 6
    (FUNCPTR) ipcom_windnet_socketclose_af_windnet,
    (FUNCPTR) ipcom_windnet_socketread,
    (FUNCPTR) ipcom_windnet_socketwrite,
    (FUNCPTR) ipcom_windnet_socketioctl,
#endif
};
#endif /* IPCOM_USE_VXSHELL_CMD */

/*
 ****************************************************************************
 * 12                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_windnet_socklib_init_cb
 *===========================================================================
 */
IP_GLOBAL SOCK_FUNC *
ipcom_windnet_socklib_init_cb(void)
{
    return &ipcom_windnet_sockfuncs;
}


/*
 *===========================================================================
 *                    ipcom_windnet_socklib_init_af_windnet_cb
 *===========================================================================
 */
#ifdef IPCOM_USE_VXSHELL_CMD
IP_GLOBAL SOCK_FUNC *
ipcom_windnet_socklib_init_af_windnet_cb(void)
{
    return &ipcom_windnet_sockfuncs_af_windnet;
}
#endif


/*
 *===========================================================================
 *                    ipcom_windnet_socklib_init
 *===========================================================================
 */
IP_GLOBAL Ip_err
ipcom_windnet_socklib_init(void)
{
    /* Sanity checks */
    ip_assert(IFNAMSIZ <= IP_IFNAMSIZ);

#if !defined(_WRS_VXWORKS_MAJOR) || _WRS_VXWORKS_MAJOR != 6
    {
        int ipcom_drv_no;

        ipcom_drv_no = iosDrvInstall((FUNCPTR) NULL,
                                     (FUNCPTR) NULL,
                                     (FUNCPTR) NULL,
                                     (FUNCPTR) ipcom_windnet_socketclose,
                                     (FUNCPTR) ipcom_windnet_socketread,
                                     (FUNCPTR) ipcom_windnet_socketwrite,
                                     (FUNCPTR) ipcom_windnet_socketioctl);
        if (ipcom_drv_no == -1)
        {
            IPCOM_LOG1(ERR, "ipcom_windnet_socklib_init :: iosDevInstall() failed, errno=%d", errno);
            return IPCOM_ERR_FAILED;
        }

        ipcom_port.dev_hdr.dev_hdr.name = "(ipcom_socket)";
        if (iosDevAdd(&ipcom_port.dev_hdr.dev_hdr, "ipcom_sock_dev", ipcom_drv_no) != OK)
        {
            IPCOM_LOG1(ERR, "ipcom_windnet_socklib_init :: iosDevAdd() failed, errno=%d", errno);
            return IPCOM_ERR_FAILED;
        }
    }
#else /* !defined(_WRS_VXWORKS_MAJOR) || _WRS_VXWORKS_MAJOR != 6 */

    sockLibInit();

#endif
    /* install socket library interface, providing binary compatibility with WindNet */
#ifdef IPCOM_USE_INET
    if (sockLibAdd ((FUNCPTR) ipcom_windnet_socklib_init_cb, IP_AF_INET, IP_AF_INET) == -1)
    {
        IPCOM_LOG0(ERR, "ipcom_windnet_socklib_init :: sockLibAdd() failed for IP_AF_INET");
    }
#endif

#ifdef IPCOM_USE_INET6
    if (sockLibAdd ((FUNCPTR) ipcom_windnet_socklib_init_cb, IP_AF_INET6, IP_AF_INET6) == -1)
    {
        IPCOM_LOG0(ERR, "ipcom_windnet_socklib_init :: sockLibAdd() failed for IP_AF_INET6");
    }
#endif

#ifdef IPNET_USE_ROUTESOCK
    if (sockLibAdd ((FUNCPTR) ipcom_windnet_socklib_init_cb, IP_AF_ROUTE, IP_AF_ROUTE) == -1)
    {
        IPCOM_LOG0(ERR, "ipcom_windnet_socklib_init :: sockLibAdd() failed for IP_AF_ROUTE");
    }
#endif

#ifdef IPNET_USE_NETLINKSOCK
    if (sockLibAdd ((FUNCPTR) ipcom_windnet_socklib_init_cb, IP_AF_NETLINK, IP_AF_NETLINK) == -1)
    {
        IPCOM_LOG0(ERR, "ipcom_windnet_socklib_init :: sockLibAdd() failed for IP_AF_ROUTE");
    }
#endif

#ifdef IPCOM_USE_PACKET
    if (sockLibAdd ((FUNCPTR) ipcom_windnet_socklib_init_cb, IP_AF_PACKET, IP_AF_PACKET) == -1)
    {
        IPCOM_LOG0(ERR, "ipcom_windnet_socklib_init :: sockLibAdd() failed for IP_AF_ROUTE");
    }
#endif

#ifdef IPIPSEC2
    if (sockLibAdd ((FUNCPTR) ipcom_windnet_socklib_init_cb, IP_AF_KEY, IP_AF_KEY) == -1)
    {
        IPCOM_LOG0(ERR, "ipcom_windnet_socklib_init :: sockLibAdd() failed for IP_AF_KEY");
    }
#endif

#ifdef IPCOM_USE_VXSHELL_CMD
    if (sockLibAdd ((FUNCPTR) ipcom_windnet_socklib_init_af_windnet_cb, IP_AF_WINDNET, IP_AF_WINDNET) == -1)
    {
        IPCOM_LOG0(ERR, "ipcom_windnet_socklib_init :: sockLibAdd() failed for IP_AF_WINDNET");
    }
#endif

#ifdef INCLUDE_NET_REM_IO
    _func_ftpLs = ftpLs;               /* init ptr to ftp dir listing routine */
    _func_netLsByName = netLsByName;   /* init ptr to netDrv listing routine */
    _func_remCurIdGet = (FUNCPTR) remCurIdGet;
    _func_remCurIdSet = (FUNCPTR) remCurIdSet;
#endif

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_windnet_socketwritev
 *===========================================================================
 * Description:
 * This function is a wrapper for ipcom_socketwritev, used when
 * IPCOM_USE_NATIVE_SOCK_API is used (i.e. the ipcom socket API is mapped
 * by the preprocessor to the VxWorks socket lib directly). ipcom_socketwritev
 * is not possible to map directly to writev, since the VxWorks IO library
 * does not 'honour' the datagram aspect of writev (it loops over the buffers
 * in iovec make one write() call per buffer).
 */
IP_PUBLIC Ip_ssize_t
ipcom_windnet_socketwritev(Ip_fd fd, IP_CONST struct Ip_iovec *iov, int iovlen)
{
    struct msghdr msghdr;

    memset(&msghdr, 0, sizeof(msghdr));
    msghdr.msg_iov = (struct iovec *) iov;
    msghdr.msg_iovlen = iovlen;

    return sendmsg(fd, &msghdr, 0);
}


/*
 ****************************************************************************
 * 13                    SELECT RELATED FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                       ipcom_windnet_select_poll
 *===========================================================================
 * Description: Checks if a socket is ready.
 * Parameters:  sock - socket to check for readiness.
 *              select_type - how the socket should be ready.
 *              owns_list_mutex - IP_TRUE of the
 *              ((SEL_WAKEUP_LIST*)sock->select_ctx)->listMutex is held.
 * Returns:     IP_TRUE - socket is ready.
 *              IP_FALSE - socket is not ready.
 *
 */
static Ip_bool
ipcom_windnet_select_poll(Ipnet_socket *sock,
                          SELECT_TYPE select_type,
                          Ip_bool owns_list_mutex)
{
    Ip_bool          is_ready;
    SEL_WAKEUP_LIST *wl = sock->select_ctx;

    IPCOM_LOG2(DEBUG2,
               "ipcom_windnet_socket_poll() :: fd=%d, sel_type=%d",
               sock->ipcom.fd,
               select_type);

    if (!owns_list_mutex)
        (void) semTake (&wl->listMutex, WAIT_FOREVER);

    switch (select_type)
    {
    case SELREAD:
        is_ready = ipnet_is_sock_readable(sock, 0);
        break;
    case SELWRITE:
        is_ready = ipnet_is_sock_writable(sock, 0);
        break;
    default: /* SELEXCEPT */
        is_ready = ipnet_is_sock_exceptional(sock, 0);
        break;
    }


    if (is_ready)
        selWakeupAll(sock->select_ctx, select_type);

    if (!owns_list_mutex)
        semGive(&wl->listMutex);

    return is_ready;
}


/*
 *===========================================================================
 *                     ipcom_windnet_select_add_node
 *===========================================================================
 * Description: Starts select monitoring of a socket.
 * Parameters:  kernel_sock_fd - kernel socket descriptor value.
 *              swn - handle to the task doing the select call.
 * Returns:     OK - task is now selecting on the socket
 *              ERROR - failed to start selecting
 *
 */
static int
ipcom_windnet_select_add_node(int kernel_sock_fd, SEL_WAKEUP_NODE *swn)
{
    Ipnet_socket    *sock;
    SEL_WAKEUP_LIST *wl;
    int              ret;

    sock = ipnet_get_sock(kernel_sock_fd);
    if (sock == IP_NULL)
        return ERROR;

    wl = sock->select_ctx;
    (void) semTake (&wl->listMutex, WAIT_FOREVER);

    ret = selNodeAdd(wl, swn);

    if (ret == OK)
    {
        sock->ipcom.event_cb =
             (Ipcom_socket_eventcb)ipcom_windnet_socket_eventcb;

        /* Check if socket is ready since any wakup event received
           between the call to ipcom_windnet_select_poll() and before
           the ipcom_setsockopt() installs the event callback is
           lost */
        (void)ipcom_windnet_select_poll(sock,
                                        selWakeupType(swn),
                                        IP_TRUE);
    }

    (void) semGive (&wl->listMutex);

    ipnet_release_sock(sock);
    return ret;
}


/*
 *===========================================================================
 *                    ipcom_windnet_select_remove_node
 *===========================================================================
 * Description: Stops select monitoring of a socket
 * Parameters:  kernel_sock_fd - kernel socket descriptor value.
 *              swn - handle to task that should stop selecting.
 * Returns:
 *
 */
static void
ipcom_windnet_select_remove_node(int kernel_sock_fd, SEL_WAKEUP_NODE *swn)
{
    Ipnet_socket *sock;

    sock = ipnet_get_sock(kernel_sock_fd);
    if (sock)
    {
        SEL_WAKEUP_LIST *wl = sock->select_ctx;

        (void) semTake (&wl->listMutex, WAIT_FOREVER);
        (void) selNodeDelete(wl, swn);

        if (selWakeupListLen (wl) == 0)
            sock->ipcom.event_cb = IP_NULL;

        (void) semGive (&wl->listMutex);
        ipnet_release_sock(sock);
    }
}


/*
 *===========================================================================
 *                      ipcom_windnet_socket_eventcb
 *===========================================================================
 * Description: Socket event callback, called every time something
 *                      happens one a socket that is part of at least
 *                      one select.
 * Parameters:  sock - socket the event occured on.
 *              pkt - packet that triggered the event or IP_NULL if no
 *                      packet is associated with the event.
 *              event - one of the IP_SOEVENT_CB_xxx constans.
 * Returns:
 *
 */
static void
ipcom_windnet_socket_eventcb(Ipcom_socket *sock,
                             IP_CONST Ipcom_pkt *pkt,
                             enum Ipcom_socket_event_type event)
{
    Ip_bool     wakeup = IP_TRUE;
    SELECT_TYPE select_type;

    IPCOM_UNUSED_ARG(pkt);
    ip_assert(sock);

    IPCOM_LOG2(DEBUG2,
               "ipcom_windnet_socket_eventcb() :: fd=%d, event=%d",
               sock->fd,
               event);

    switch(event)
    {
    case IP_SOEVENT_CB_CLOSE:
    case IP_SOEVENT_CB_ERRNO:
    case IP_SOEVENT_CB_TIMED_OUT:
        /* Poll for read... */
        ipcom_windnet_socket_eventcb(sock, pkt, IP_SOEVENT_CB_SELREAD);
        /* ...for write... */
        ipcom_windnet_socket_eventcb(sock, pkt, IP_SOEVENT_CB_SELWRITE);
        /* ...and for execpt */
        ipcom_windnet_socket_eventcb(sock, pkt, IP_SOEVENT_CB_SELEXCEPT);
        wakeup = IP_FALSE;
        break;

    case IP_SOEVENT_CB_INPKT:
    case IP_SOEVENT_CB_ACCEPT:
    case IP_SOEVENT_CB_TCP_STATE:
        wakeup = IP_FALSE;
        break;

    case IP_SOEVENT_CB_SELREAD:
        select_type = SELREAD;
        break;

    case IP_SOEVENT_CB_ACK:
    case IP_SOEVENT_CB_OUTPKT:
    case IP_SOEVENT_CB_CONNECT:
        wakeup = IP_FALSE;
        break;

    case IP_SOEVENT_CB_SELWRITE:
        select_type = SELWRITE;
        break;

#ifdef SELEXCEPT
    case IP_SOEVENT_CB_OOB:
        wakeup = IP_FALSE;
        break;
    case IP_SOEVENT_CB_SELEXCEPT:
        select_type = SELEXCEPT;
        break;
#endif

    default:
        /* Ignore all other events */
        wakeup = IP_FALSE;
        break;
    }

    if (wakeup)
        ipcom_windnet_select_poll((Ipnet_socket *) sock,
                                  select_type,
                                  IP_FALSE);
}


/*
 *===========================================================================
 *                    ipcom_windnet_socklib_sock_size
 *===========================================================================
 * Description: Returns the number of octets that has to be reserved in each
 *              socket for this module.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipcom_windnet_socklib_sock_size(void)
{
    return sizeof(SEL_WAKEUP_LIST);
}


/*
 *===========================================================================
 *                   ipcom_windnet_socklib_socket_init
 *===========================================================================
 * Description: Allocates and initializes the per-socket memory needed by
 *              this module.
 * Parameters:  sock - the socket this module will allocate memory for.
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_GLOBAL int
ipcom_windnet_socklib_socket_init(Ipnet_socket *sock)
{
    sock->select_ctx = ipnet_sock_alloc_private_data(sock, ipcom_windnet_socklib_sock_size());
    selWakeupListInit(sock->select_ctx);
#ifdef IPNET_SO_WAKEUP_HOOK
    sock->windnet_so = IP_NULL;
#endif
    return 0;
}


/*
 *===========================================================================
 *                   ipcom_windnet_socklib_socket_free
 *===========================================================================
 * Description: Free all resources allocated by
 *              ipcom_windnet_socklib_socket_init()
 * Parameters:  sock - the socket to free the resources for.
 * Returns:
 *
 */
IP_GLOBAL void
ipcom_windnet_socklib_socket_free(Ipnet_socket *sock)
{
    if (sock->select_ctx != IP_NULL)
        selWakeupListTerm(sock->select_ctx);
#ifdef IPNET_SO_WAKEUP_HOOK
    sock->windnet_so = IP_NULL;
#endif
}

#ifdef IPNET_SO_WAKEUP_HOOK

VOIDFUNCPTR  sowakeupHook = IP_NULL;

IP_GLOBAL void
ipcom_windnet_socklib_sowakeupHook(Ipnet_socket *sock, int how)
{
    if (sowakeupHook != IP_NULL && sock->windnet_so != IP_NULL)
    {
        if (how == IP_SHUT_RD || how == IP_SHUT_RDWR || sock->ipcom.so_errno)
            (*sowakeupHook)(sock->windnet_so, SELREAD);

        if (how == IP_SHUT_WR || how == IP_SHUT_RDWR || sock->ipcom.so_errno)
            (*sowakeupHook)(sock->windnet_so, SELWRITE);
    }

    return;
}
#endif
#else
int ipcom_windnet_compat_empty_file;
#endif /* IPCOM_USE_SOCK_COMPAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

