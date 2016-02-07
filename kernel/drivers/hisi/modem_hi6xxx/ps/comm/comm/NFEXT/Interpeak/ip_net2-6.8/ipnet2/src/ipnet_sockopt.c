/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_sockopt.c,v $ $Revision: 1.222 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_sockopt.c,v $
 *     $Author: jyang $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * ipcom_getsockopt() and ipcom_setsockopt() functions.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#define IPCOM_SKIP_NATIVE_SOCK_API

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>

#include "ipnet_h.h"
#include "ipnet_usr_sock.h"

#ifdef IPCOM_USE_INET6
#include "ipnet_icmp6.h"
#endif

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPFIREWALL
#include <ipfirewall.h>
#include <ipfirewall_h.h>
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_MIN_RCVBUF_SIZE 2000
#define IPNET_MIN_SNDBUF_SIZE 3000
#ifdef IP_PORT_LKM
/* Linux always doubles the socket buffer value */
#define IPNET_SOCKBUF_MULTIPLIER 2
#else
#define IPNET_SOCKBUF_MULTIPLIER 1
#endif


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

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int ipnet_getsockopt_sol(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen);
IP_STATIC int ipnet_setsockopt_sol(Ipnet_socket *sock, int optname, IP_CONST void *optval, Ip_socklen_t optlen);

#ifdef IPCOM_USE_INET
IP_STATIC int ipnet_getsockopt_ip4(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen);
IP_STATIC int ipnet_setsockopt_ip4(Ipnet_socket *sock, int optname, IP_CONST void *optval, Ip_socklen_t optlen);
#endif

#ifdef IPCOM_USE_INET6
IP_STATIC int ipnet_getsockopt_ip6(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen);
#endif


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

IP_STATIC void
ipnet_sockopt_invalidate_sock_cache(Ipnet_socket *sock)
{
#ifdef IPCOM_USE_INET
    if (sock->ipcom.domain == IP_AF_INET)
        ipnet_sock_invalidate_ip4_cache(sock);
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (sock->ipcom.domain == IP_AF_INET6)
        ipnet_sock_invalidate_ip6_cache(sock);
#endif /* IPCOM_USE_INET6 */
}


/*
 *===========================================================================
 *                   ipnet_sockopt_enter_exit_promisc
 *===========================================================================
 * Description: Enters/exits promiscuous mode using the specified socket
 *              as placeholder. Closing the socket will also leave trigger
 *              one leave for each unmatched join.
 * Parameters:  sock - A socket.
 *              netif - The network interface to join promisc mode.
 *              enter_promisc - IP_TRUE if promiscuous mode should be entered
 *                              IP_FALSE to exit promiscuous mode.
 * Returns:     0 = success, <0 error code.
 *
 */
IP_STATIC int
ipnet_sockopt_enter_exit_promisc(Ipnet_socket *sock, Ipnet_netif *netif, Ip_bool enter_promisc)
{
    if (enter_promisc)
        sock->promisc_count++;
    else
    {
        if (sock->promisc_count == 0)
            return -IP_ERRNO_EADDRNOTAVAIL;
        sock->promisc_count--;
    }

    IPCOM_LOG2(INFO, "%s %s promiscuous mode",
               netif->ipcom.name,
               enter_promisc ? "enter" : "exit");

    return ipnet_if_drv_ioctl(netif, IP_SIOCXPROMISC, &enter_promisc);
}


/*
 *===========================================================================
 *                    ipnet_receive_cb_init
 *===========================================================================
 * Description: Calls the receive callback for each packet in the receive
 *              buffer.
 * Parameters:  sock - The socket to which a receive callback was installed.
 * Returns:     0 = success, <0 error code.
 *
 */
IP_STATIC int
ipnet_receive_cb_init(Ipnet_socket *sock)
{
    Ipcom_pkt           *pkt;
    Ipcom_socket_eventcb event_cb = sock->ipcom.event_cb;

    ip_assert(event_cb != IP_NULL);

    for (pkt = sock->rcv_head; pkt != IP_NULL; pkt = pkt->next)
        event_cb(&sock->ipcom, pkt, IP_SOEVENT_CB_INPKT);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_getsockopt_sol
 *===========================================================================
 * Description: Socket option where level was set to IP_SOL_SOCKET.
 * Parameters:
 * Returns:     0 = success, <0 error code.
 *
 */
IP_STATIC int
ipnet_getsockopt_sol(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen)
{
    int val = 0;

    switch (optname)
    {
    case IP_UDP_X_CHKSUM:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM);
        break;

#ifdef IPCOM_USE_INET
    case IP_SO_BROADCAST:
        if (sock->ip4 == IP_NULL)
            return -IP_ERRNO_EINVAL;
        val = (int) IP_BIT_ISTRUE(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_ALLOW_BROADCAST);
        break;
#endif /* IPCOM_USE_INET */

    case IP_SO_X_BYPASS_FLOW:
#ifdef IPCOM_USE_INET
        if (sock->ip4 != IP_NULL)
            val = (int) IP_BIT_ISTRUE(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_BYPASS_FLOW);
        else
#endif
#ifdef IPCOM_USE_INET6
        if (sock->ip6 != IP_NULL)
            val = (int) IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_BYPASS_FLOW);
        else
#endif
            return -IP_ERRNO_EINVAL;

        break;

#ifdef IPTCP
    case IP_SO_LINGER:
        if (*optlen >= sizeof(struct Ip_linger))
        {
            struct Ip_linger *l = optval;

            if(sock->tcb == IP_NULL)
                return -IP_ERRNO_EOPNOTSUPP;

            *optlen = sizeof(struct Ip_linger);
            l->l_onoff  = (sock->tcb->sec_to_linger != IPCOM_ADDR_INFINITE);
            l->l_linger =  sock->tcb->sec_to_linger;
            return 0;
        }
        return -IP_ERRNO_EINVAL;
#endif /* IPTCP */

#ifdef IPNET_USE_RFC2890
    case IP_SO_X_GRE_KEY:
        if (*optlen >= sizeof(struct Ip_gre_key))
        {
            struct Ip_gre_key *gk = optval;

            *optlen = sizeof(struct Ip_gre_key);
            gk->gk_onoff = IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_GRE_KEY);
            gk->gk_key   = sock->gre_key;
            return 0;
        }
        return -IP_ERRNO_EINVAL;
#endif /* IPNET_USE_RFC2890 */

    case IP_SO_REUSEADDR:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_REUSEADDR);
        break;

    case IP_SO_REUSEPORT:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_REUSEPORT);
        break;

    case IP_SO_DONTROUTE:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_DONTROUTE);
        break;

    case IP_SO_ERROR:
        val = sock->ipcom.so_errno;
        sock->ipcom.so_errno = 0;
        break;

    case IP_SO_SNDBUF:
        val = IP_MAX(sock->send_max_bytes / IPNET_SOCKBUF_MULTIPLIER, IPNET_MIN_SNDBUF_SIZE);
        break;

    case IP_SO_RCVBUF:
        val = IP_MAX(sock->recv_max_bytes / IPNET_SOCKBUF_MULTIPLIER, IPNET_MIN_RCVBUF_SIZE);
        break;

    case IP_SO_RCVTIMEO:
        if (*optlen >= sizeof(struct Ip_timeval))
        {
            *optlen = sizeof(struct Ip_timeval);
            if (sock->rcv_timeout == IP_NULL)
                ipcom_memset(optval, 0, *optlen);
            else
                ipcom_memcpy(optval, sock->rcv_timeout, *optlen);
            return 0;
        }
        return -IP_ERRNO_EINVAL;

    case IP_SO_TYPE:
        val = sock->ipcom.type;
        break;

    case IP_SO_KEEPALIVE:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_KEEPALIVE);
        break;

    case IP_SO_X_PROBEDELAY:
        val = sock->keep_alive_probe_delay;
        break;

    case IP_SO_BINDTODEVICE:
        if (*optlen >= IP_IFNAMSIZ)
        {
            Ipnet_netif *netif;

            *optlen = IP_IFNAMSIZ;
            ipcom_memset(optval, 0, IP_IFNAMSIZ);
            netif = ipnet_if_indextonetif(sock->vr_index, sock->bind_to_ifindex);
            if (netif != IP_NULL)
                ipcom_strncpy(optval, netif->ipcom.name, IP_IFNAMSIZ);
            return 0;
        }
        return -IP_ERRNO_EINVAL;

    case IP_SO_OOBINLINE:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_OOBINLINE);
        break;

#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
    case IP_SO_VLAN:
        if (*optlen >= sizeof(struct Ip_sovlan))
        {
            struct Ip_sovlan *sovlan = optval;

            sovlan->vlan_onoff      = (sock->vlan_tag != 0);
            sovlan->priority_tagged = !!(sock->vlan_tag & 0x1000);
            sovlan->vid             = (unsigned short) (sock->vlan_tag & 0x0fff);
            sovlan->upriority       = (unsigned short) (sock->vlan_tag >> 13);
            return 0;
        }
        return -IP_ERRNO_EINVAL;
#endif /* IPNET_USE_PER_SOCKET_VLAN_TAG */

    case IP_SO_X_EVENTCB_RO:
        if (*optlen >= sizeof(Ipcom_socket_eventcb))
        {
            *optlen = sizeof(Ipcom_socket_eventcb);
            *(Ipcom_socket_eventcb *) optval = sock->ipcom.event_cb;
            return 0;
        }
        return -IP_ERRNO_EINVAL;

    case IP_SO_X_VR:
        val = sock->vr_index;
        break;

    case IP_SO_X_COOKIE:
        if (*optlen < sizeof(void*))
            goto errout;

        *(void**) optval = sock->ipcom.cookie;
        *optlen = sizeof(void*);
        return 0;

    case IP_SO_X_DRAINABLE:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_DRAINABLE);
        break;

    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    if (*optlen >= sizeof(int))
    {
        *(int*) optval = val;
        *optlen = sizeof(int);
        return 0;
    }
 errout:
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_setsockopt_sol
 *===========================================================================
 * Description: Socket option where level was set to IP_SOL_SOCKET.
 * Parameters:
 * Returns:     0 = success, <0 error code.
 *
 */
IP_STATIC int
ipnet_setsockopt_sol(Ipnet_socket *sock, int optname, IP_CONST void *optval, Ip_socklen_t optlen)
{
    int          val;
    Ip_socklen_t this_optlen;
    Ip_u32       flag = 0;

    /* Verify optlen size */
    switch (optname)
    {
    case IP_SO_RCVTIMEO:
        this_optlen = sizeof(struct Ip_timeval);
        break;
    case IP_SO_BINDTODEVICE:
        this_optlen = IP_IFNAMSIZ;
        break;
#ifdef IPTCP
    case IP_SO_LINGER:
        this_optlen = sizeof(struct Ip_linger);
        break;
#endif
#ifdef IPNET_USE_RFC2890
    case IP_SO_X_GRE_KEY:
        this_optlen = sizeof(struct Ip_gre_key);
        break;
#endif /* IPNET_USE_RFC2890 */
    case IP_SO_X_COOKIE:
        this_optlen = sizeof(void*);
        break;
    case IP_SO_X_EVENTCB_RO:
        this_optlen = sizeof(Ipcom_socket_eventcb);
        break;
#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
    case IP_SO_VLAN:
        this_optlen = sizeof(struct Ip_sovlan);
        break;
#endif
#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
#ifdef IPNET_SO_WAKEUP_HOOK
    case IP_SO_X_WINDNET_SO:
        this_optlen = sizeof(void*);
        break;
#endif
#endif
    default:
        this_optlen = sizeof(int);
        break;
    }

    if (optlen != this_optlen)
        return -IP_ERRNO_EINVAL;
    val = *(int*) optval;

    /* Process option */
    switch (optname)
    {
#ifdef IPCOM_USE_INET
    case IP_SO_BROADCAST:
        if (sock->ip4 == IP_NULL)
            return -IP_ERRNO_EPERM;
        if (val)
            IP_BIT_SET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_ALLOW_BROADCAST);
        else
            IP_BIT_CLR(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_ALLOW_BROADCAST);
        return 0;
#endif /* IPCOM_USE_INET */

#if defined(IP_SO_USELOOPBACK) && (defined(IPNET_USE_ROUTESOCK) || defined(IPNET_USE_NETLINKSOCK))
    case IP_SO_USELOOPBACK:
        if (sock->ipcom.domain != IP_AF_ROUTE && sock->ipcom.domain != IP_AF_NETLINK)
            return -IP_ERRNO_ENOPROTOOPT;
        if (val)
            sock->bind_to_ifindex = 0;
        else
            sock->bind_to_ifindex = sock->ipcom.pid;
        return 0;
#endif /* defined(IP_SO_USELOOPBACK) && (defined(IPNET_USE_ROUTESOCK) || defined(IPNET_USE_NETLINKSOCK)) */

#ifdef IPTCP
    case IP_SO_LINGER:
        {
            IP_CONST struct Ip_linger *l = optval;

            if(sock->tcb == IP_NULL)
                return -IP_ERRNO_EOPNOTSUPP;

            if (l->l_onoff)
                sock->tcb->sec_to_linger = l->l_linger;
            else
                sock->tcb->sec_to_linger = IPCOM_ADDR_INFINITE;
        }
        return 0;
#endif /* IPTCP */

#ifdef IPNET_USE_RFC2890
    case IP_SO_X_GRE_KEY:
        {
            IP_CONST struct Ip_gre_key *gk = optval;

            if (gk->gk_onoff)
                IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_GRE_KEY);
            else
                IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_GRE_KEY);
            sock->gre_key = gk->gk_key;
        }
        return 0;
#endif /* IPNET_USE_RFC2890 */

    case IP_SO_BINDTODEVICE:
        {
            Ipnet_netif *netif;

            netif = ipnet_if_nametonetif(sock->vr_index, (char *) optval);
            if (netif == IP_NULL)
                return -IP_ERRNO_ENXIO;
            ipnet_sockopt_invalidate_sock_cache(sock);
            sock->bind_to_ifindex = netif->ipcom.ifindex;
            return 0;
        }

    case IP_SO_X_EVENTCB_RO:
        sock->ipcom.event_cb = *(Ipcom_socket_eventcb *)optval;
        if (sock->ipcom.event_cb != IP_NULL)
            return ipnet_receive_cb_init(sock);
        return 0;

    case IP_SO_SNDBUF:
        if (val < 0)
            return -IP_ERRNO_EINVAL;
        sock->send_max_bytes = IP_MAX(IPNET_SOCKBUF_MULTIPLIER * val, IPNET_MIN_SNDBUF_SIZE);
        return 0;

    case IP_SO_RCVBUF:
        if (val < 0)
            return -IP_ERRNO_EINVAL;
        sock->recv_max_bytes = IP_MAX(IPNET_SOCKBUF_MULTIPLIER * val, IPNET_MIN_RCVBUF_SIZE);
        return 0;

    case IP_SO_X_VR:
        if (ipnet_has_priv(IPNET_PRIV_CHVR) == IP_FALSE)
            return -IP_ERRNO_EPERM;
        if (IP_BIT_ISSET(sock->flags,
                         IPNET_SOCKET_FLAG_BOUND | IPNET_SOCKET_FLAG_MROUTE_CONTROL))
            /* Cannot change the virtual router when then socket has been bound since
               the vr affects which flow the socket will match */
            return -IP_ERRNO_EACCES;
        if (val < 0 || val > 0xffff)
            return -IP_ERRNO_EINVAL;

        ipnet_sockopt_invalidate_sock_cache(sock);
        if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_REACHABLE))
            sock->vr_index = (Ip_u16) val;
        else
        {
            ipnet_sock_set_unreachable(sock);
            sock->vr_index = (Ip_u16) val;
            ipnet_sock_set_reachable(sock);
        }
        return 0;

    case IP_SO_X_BYPASS_FLOW:
#ifdef IPCOM_USE_INET
        if (sock->ip4 != IP_NULL)
        {
            if (val)
                IP_BIT_SET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_BYPASS_FLOW);
            else
                IP_BIT_CLR(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_BYPASS_FLOW);
        }
        else
#endif
#ifdef IPCOM_USE_INET6
        if (sock->ip6 != IP_NULL)
        {
            if (val)
                IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_BYPASS_FLOW);
            else
                IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_BYPASS_FLOW);
        }
        else
#endif
            return -IP_ERRNO_EINVAL;

        break;

    case IP_SO_X_COOKIE:
        sock->ipcom.cookie = *(void**) optval;
        return 0;

    case IP_SO_X_PROBEDELAY:
        sock->keep_alive_probe_delay = val;
        return 0;

    case IP_SO_RCVTIMEO:
        if (sock->rcv_timeout == IP_NULL)
        {
            sock->rcv_timeout = ipcom_malloc(optlen);
            if (sock->rcv_timeout == IP_NULL)
                return -IP_ERRNO_ENOMEM;
        }

        ipcom_memcpy(sock->rcv_timeout, optval, optlen);

        if (sock->rcv_timeout->tv_sec == 0
            && sock->rcv_timeout->tv_usec == 0)
        {
            /* Infinite receive timeout */
            ipcom_free(sock->rcv_timeout);
            sock->rcv_timeout = IP_NULL;
        }
        return 0;

#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
    case IP_SO_VLAN:
        {
            IP_CONST struct Ip_sovlan *sovlan = optval;

            if (!sovlan->vlan_onoff)
            {
                sock->vlan_tag = 0;
                return 0;
            }

            if (sovlan->upriority > 7
                || (!sovlan->priority_tagged && sovlan->vid > 4094))
                return -IP_ERRNO_EINVAL;

            sock->vlan_tag = (sovlan->upriority << 13);
            if (!sovlan->priority_tagged)
            /* removed setting of the CFI bit
                IP_BIT_SET(sock->vlan_tag, 0x1000);
            else */
                sock->vlan_tag |= sovlan->vid;

            return 0;
        }
#endif /* IPNET_USE_PER_SOCKET_VLAN_TAG */

    /* Socket flags controlled by setsockopt */
    case IP_UDP_X_CHKSUM:
        flag = IPNET_SOCKET_FLAG_ADDCHECKSUM;
        break;
    case IP_SO_REUSEADDR:
        flag = IPNET_SOCKET_FLAG_REUSEADDR;
        break;
    case IP_SO_REUSEPORT:
        flag = IPNET_SOCKET_FLAG_REUSEPORT;
        break;
    case IP_SO_DONTROUTE:
        flag = IPNET_SOCKET_FLAG_DONTROUTE;
        break;
    case IP_SO_KEEPALIVE:
        flag = IPNET_SOCKET_FLAG_KEEPALIVE;
        break;
    case IP_SO_OOBINLINE:
        flag = IPNET_SOCKET_FLAG_OOBINLINE;
        break;
    case IP_SO_X_DRAINABLE:
        flag = IPNET_SOCKET_FLAG_DRAINABLE;
        break;
#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
#ifdef IPNET_SO_WAKEUP_HOOK
    case IP_SO_X_WINDNET_SO:
        sock->windnet_so = *(void**) optval;
        return 0;
#endif
#endif

    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    if (val)
        IP_BIT_SET(sock->flags, flag);
    else
        IP_BIT_CLR(sock->flags, flag);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_getsockopt_udp
 *===========================================================================
 * Description: Socket option where level was set to IP_IPPROTO_UDP.
 * Parameters:
 * Returns:     0 = success, <0 error code.
 *
 */
IP_STATIC int
ipnet_getsockopt_udp(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen)
{
    int val = 0;

    if (sock->proto == IP_IPPROTO_UDP)
    {
        switch (optname)
        {
        case IP_UDP_NAT_T:
            val = sock->udp_encapsulation;
            break;
            /* Fall through */
        default:
            goto sol_socket;

        }

        if (*optlen >= sizeof(int))
        {
            *(int*) optval = val;
            *optlen = sizeof(int);
            return 0;
        }
    }

sol_socket:
    /* Invoke the socket level option */
    return ipnet_getsockopt_sol(sock,optname,optval,optlen);
}


/*
 *===========================================================================
 *                    ipnet_setsockopt_sol
 *===========================================================================
 * Description: Socket option where level was set to IP_SOL_SOCKET.
 * Parameters:
 * Returns:     0 = success, <0 error code.
 *
 */
IP_STATIC int
ipnet_setsockopt_udp(Ipnet_socket *sock, int optname, IP_CONST void *optval, Ip_socklen_t optlen)
{
    int          val;
    Ip_socklen_t this_optlen;

    if (sock->proto == IP_IPPROTO_UDP)
    {
        /* Verify optlen size */
        switch (optname)
        {
        default:
            this_optlen = sizeof(int);
            break;
        }

        /* Does the argument match? */
        if (optlen == this_optlen)
        {
            val = *(int*) optval;

            /* Process option */
            switch (optname)
            {
            case IP_UDP_NAT_T:
                switch (val)
                {
                case IP_UDP_NAT_T_DISABLED:
                    sock->udp_encapsulation = IP_UDP_NAT_T_DISABLED;
                    break;
#ifdef IPCOM_USE_INET
                case IP_UDP_NAT_T_RFC3519:
                    sock->udp_encapsulation = IP_UDP_NAT_T_RFC3519;
                    break;
                case IP_UDP_NAT_T_RFC3948:
                    sock->udp_encapsulation = IP_UDP_NAT_T_RFC3948;
                    break;
#endif
                case IP_UDP_NAT_T_DSMIPV6:
                    sock->udp_encapsulation = IP_UDP_NAT_T_DSMIPV6;
                    break;
                default:
                    return -IP_ERRNO_EINVAL;
                }
                return 0;
            }
        }
    }

    /* SOL_SOCKET socket option */
    return ipnet_setsockopt_sol(sock, optname, optval, optlen);
}


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_getsockopt_ip4
 *===========================================================================
 * Description: Socket option where level was set to IP_IPPROTO_IP.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_getsockopt_ip4(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen)
{
    int          val;
    Ip_u8        val_u8;
    Ipnet_netif *netif;

#ifdef IPNET_USE_MCAST_ROUTING
    if (optname & IP_MRT_BASE)
        return ipnet_ip4_mcast_getsockopt(sock, optname, optval, optlen);
#endif /* IPNET_USE_MCAST_ROUTING */

#ifdef IPNET_USE_NAT
    if (optname == IP_IP_NAT)
        return (int)ipnet_nat_getsockopt(sock, optname, optval, optlen);
#endif /* IPNET_USE_NAT */

#ifdef IPFIREWALL
    if (optname == IP_IP_FW)
    {
        if (ipfirewall == IP_NULL)
            return -IP_ERRNO_EOPNOTSUPP;
        return (int)ipfirewall_getsockopt(sock, optname, optval, optlen);
    }
#endif /* IPFIREWALL */

    switch (optname)
    {
    case IP_IP_OPTIONS:
        if (sock->ip4->opts == IP_NULL)
        {
            *optlen = 0;
            return 0;
        }

        if (sock->ip4->opts->len > (int) *optlen)
            return -IP_ERRNO_EINVAL;
        *optlen = sock->ip4->opts->len;
        ipcom_memcpy(optval, &sock->ip4->opts->opts, *optlen);
        return 0;
    case IP_IP_HDRINCL:
        val = (int) IP_BIT_ISTRUE(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_HDRINCL);
        break;
    case IP_IP_TOS:
        val = sock->ip4->type_of_service;
        break;
    case IP_IP_RECVTOS:
        val = (int) IP_BIT_ISTRUE(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVTOS);
        break;
    case IP_IP_TTL:
        val = sock->uni_hop_limit;
        break;
    case IP_IP_RECVTTL:
        val = (int) IP_BIT_ISTRUE(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVTTL);
        break;
    case IP_IP_RECVDSTADDR:
        val = (int) IP_BIT_ISTRUE(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVDSTADDR);
        break;
    case IP_IP_PKTINFO:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_RECVPKTINFO);
        break;
    case IP_IP_ROUTER_ALERT:
        val_u8 = sock->ip4->router_alert;
        goto u8_val;
    case IP_IP_DONTFRAG:
        val_u8 = sock->ip4->dont_frag;
        goto u8_val;
    case IP_IP_MULTICAST_TTL:
        val_u8 = (Ip_u8)sock->multi_hop_limit;
        goto u8_val;
    case IP_IP_MULTICAST_LOOP:
        val_u8 = (Ip_u8) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST);
u8_val:
        if (*optlen >= sizeof(Ip_u8))
        {
            *(Ip_u8 *) optval = val_u8;
            *optlen = sizeof(Ip_u8);
            return 0;
        }
        return -IP_ERRNO_EINVAL;
    case IP_IP_RECVIF:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_RECV_IF);
        break;
    case IP_IP_X_SENDERLINKADDR:
        val = (int) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_SENDER_LINK_ADDR);
        break;
    case IP_IP_MULTICAST_IF:
#ifndef IP_PORT_LKM
        if (*optlen < sizeof(struct Ip_in_addr))
            return -IP_ERRNO_EINVAL;
        *optlen = sizeof(struct Ip_in_addr);

        netif = ipnet_if_indextonetif(sock->vr_index, sock->ip4->multicast_if);
        if (netif == IP_NULL || netif->inet4_addr_list == IP_NULL)
            ipcom_memset(optval, 0, sizeof(struct Ip_in_addr));
        else
            ipcom_memcpy(optval, &netif->inet4_addr_list->ipaddr_n, sizeof(struct Ip_in_addr));
#else
        /* Linux uses the Ip_mreqn struct for this */
        if (*optlen < sizeof(struct Ip_ip_mreqn))
            return -IP_ERRNO_EINVAL;
        *optlen = sizeof(struct Ip_ip_mreqn);

        netif = ipnet_if_indextonetif(sock->vr_index, sock->ip4->multicast_if);
        if (netif == IP_NULL || netif->inet4_addr_list == IP_NULL)
            ipcom_memset(optval, 0, sizeof(struct Ip_ip_mreqn));
        else
        {
            struct Ip_ip_mreqn *mreqn = (struct Ip_ip_mreqn*)optval;

            ipcom_memcpy(&mreqn->imr_address,
                         &netif->inet4_addr_list->ipaddr_n,
                         sizeof(struct Ip_in_addr));
            mreqn->imr_multiaddr.s_addr = IP_INADDR_ANY;
            mreqn->imr_ifindex          = netif->ipcom.ifindex;
        }
#endif

        return 0;
    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    if (*optlen >= sizeof(int))
    {
        *(int*) optval = val;
        *optlen = sizeof(int);
        return 0;
    }
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *               ipnet_sockopt_ip4_mcast_create_data
 *===========================================================================
 * Description: Creates a socket control block for one multicast address
 *              controlled via the socket.
 * Parameters:  sock - The controlling socket for the multicast address.
 *              netif - The interface the multicast address will be assigned to.
 *              maddr - An multicast address.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_sockopt_ip4_mcast_create_data(Ipnet_socket *sock,
                                    Ipnet_netif *netif,
                                    struct Ip_in_addr *maddr)
{

    Ipnet_ip4_sock_mcast_data **pstorage;
    Ipnet_ip4_sock_mcast_data  *mcast_data;
    int                         ret;

    if (IP_IN_CLASSD(maddr->s_addr) == IP_FALSE)
        return -IP_ERRNO_EINVAL;

    pstorage = ipnet_sock_ip4_get_mcast_data(sock, netif, maddr);
    if (*pstorage != IP_NULL)
        /* The group is already joined */
        return -IP_ERRNO_EADDRINUSE;

    mcast_data = ipcom_malloc(sizeof(Ipnet_ip4_sock_mcast_data));
    if (mcast_data == IP_NULL)
        return -IP_ERRNO_ENOMEM;

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    mcast_data->sources = ipcom_set_new(sizeof(struct Ip_in_addr));
    if (mcast_data->sources == IP_NULL)
    {
        ipcom_free(mcast_data);
        return -IP_ERRNO_ENOMEM;
    }
    mcast_data->sources->user = IPCOM_SET_IS_EXCLUDE;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    mcast_data->netif         = netif;
    mcast_data->maddr.s_addr  = maddr->s_addr;
    mcast_data->next          = IP_NULL;
    *pstorage = mcast_data;
    ret = ipnet_ip4_add_addr(netif,
                             maddr->s_addr,
                             IP_IN_CLASSD_NET,
                             IP_FALSE,
                             IPNET_ADDR_TYPE_MULTICAST);

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    if (ret >= 0)
    {
        Ipnet_ip4_addr_entry *addr_entry;

        addr_entry = ipnet_ip4_get_addr_entry(maddr->s_addr, sock->vr_index, netif);
        if (addr_entry == IP_NULL)
            ret = -IP_ERRNO_ENXIO;
        else if (ipcom_set_add(addr_entry->mcast.sockets, &sock->ipcom.fd) != IPCOM_SUCCESS)
            ret = -IP_ERRNO_ENOMEM;
    }
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    if (ret < 0)
        (void)ipnet_sock_ip4_mcast_delete_data(sock, netif, maddr);
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    else
        ipnet_igmpv3_report_change(netif, maddr);
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    return ret;
}


/*
 *===========================================================================
 *               ipnet_sockopt_ip4_mcast_add_remove_source
 *===========================================================================
 * Description: Adds or removes a multicast channel.
 * Parameters:  sock - The socket that contains the
 *              netif - The network interface the multicast address is assigned to.
 *              group - The group to add/remove.
 *              source - The source address to add/remove.
 *              filter_mode - IPCOM_SET_IS_INCLUDE or IPCOM_SET_IS_EXCLUDE
 *              is_add - IP_TRUE if the source address should be added.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_STATIC int
ipnet_sockopt_ip4_mcast_add_remove_source(Ipnet_socket *sock,
                                          Ipnet_netif *netif,
                                          struct Ip_in_addr *group,
                                          struct Ip_in_addr *source,
                                          int filter_mode,
                                          Ip_bool is_add)
{
    int                        ret = 0;
    Ipnet_ip4_sock_mcast_data *mcast_data;

    mcast_data = *ipnet_sock_ip4_get_mcast_data(sock, netif, group);
    if (mcast_data == IP_NULL)
    {
        if (is_add && filter_mode == IPCOM_SET_IS_INCLUDE)
        {
            /* Create a new data entry for the multicast address */
            ret = ipnet_sockopt_ip4_mcast_create_data(sock, netif, group);
            if (ret < 0)
                return ret;

            mcast_data = *ipnet_sock_ip4_get_mcast_data(sock, netif, group);
            mcast_data->sources->user = filter_mode;
        }
        if (mcast_data == IP_NULL)
            return -IP_ERRNO_EADDRNOTAVAIL;
    }

    if (mcast_data->sources->user != filter_mode)
        return -IP_ERRNO_EINVAL;

    if (!is_add)
    {
        if (ipcom_set_remove(mcast_data->sources, source) != IPCOM_SUCCESS)
            ret = -IP_ERRNO_EADDRNOTAVAIL;
        else if (filter_mode == IPCOM_SET_IS_INCLUDE
                 && mcast_data->sources->obj_count == 0)
        {
            /* Not member of any channels that contains this multicast address */
            ret = ipnet_sock_ip4_mcast_delete_data(sock,
                                                   netif,
                                                   group);
        }
    }
    else
    {
        switch (ipcom_set_add(mcast_data->sources, source))
        {
        case IPCOM_ERR_ALREADY_INSTALLED:
            ret = -IP_ERRNO_EADDRNOTAVAIL;
            break;
        case IPCOM_ERR_NO_MEMORY:
            ret = -IP_ERRNO_ENOMEM;
            break;
        default:
            break;
        }
    }

    if (ret == 0)
    {
        ipnet_igmpv3_report_change(netif, group);
        IPCOM_LOG5(INFO, "IPv4: %s source %s to group %s at interface %s, filter mode is %s",
                   is_add ? "added" : "removed",
                   ipcom_inet_ntop(IP_AF_INET, source, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   ipcom_inet_ntop(IP_AF_INET, group, ipnet->log_buf + IP_INET_ADDRSTRLEN, sizeof(ipnet->log_buf)),
                   netif->ipcom.name,
                   filter_mode == IPCOM_SET_IS_INCLUDE ? "include" : "exclude");
    }

    return ret;
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */


/*
 *===========================================================================
 *                 ipnet_sockopt_ip4_set_ip_options
 *===========================================================================
 * Description: Checks the IP options for sanity and copies them to the
 *              socket buffer.
 * Parameters:  opts - The socket buffer where the options will be stored.
 *              optval - User supplied options.
 *              optlen - The length of the user supplied options.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_sockopt_ip4_set_ip_options(struct Ipnet_ip4_sock_opts *o,
                                 IP_CONST Ip_u8 *optval,
                                 Ip_socklen_t optlen)
{
    Ip_u8        olen;
    Ip_socklen_t i;

    for (i = 0;
         i < optlen && optval[i] != IP_IPOPT_END;
         i += olen)
    {
        switch (optval[i])
        {
        case IP_IPOPT_NOOP:
            olen = 1;
            break;
        default:
            if (optlen < i + 1)
                return -IP_ERRNO_EINVAL;
            olen = optval[i + 1];
            break;
        }
    }

    ipcom_memset(o->opts, IP_IPOPT_END, sizeof(o->opts));
    ipcom_memcpy(o->opts, optval, i);
    o->len = IP_ROUNDUP(i, 4);

    return 0;
}


/*
 *===========================================================================
 *           ipnet_setsockopt_ip4_update_sticky_option_flag
 *===========================================================================
 * Description: Sets the sticky option flag if sticky option(s) is
 *              present, clears the flag otherwise.
 * Parameters:  sock - The socket on which the option should be set.
 *              optname - The option to set.
 *              optval - The value to set.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC void
ipnet_setsockopt_ip4_update_sticky_option_flag(Ipnet_socket *sock)
{
    if (sock->ip4->router_alert)
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_HAS_STICKY_OPT);
    else
        IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_HAS_STICKY_OPT);
}


/*
 *===========================================================================
 *                    ipnet_setsockopt_ip4
 *===========================================================================
 * Description: Socket option where level was set to IP_IPPROTO_IP.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_setsockopt_ip4(Ipnet_socket *sock, int optname, IP_CONST void *optval, Ip_socklen_t optlen)
{
    int                         val = 0;
    Ip_u8                       val_u8 = 0;
    struct Ip_ip_mreq          *mreq;
    struct Ip_group_source_req *gsreq;
    Ipnet_netif                *netif;
    struct Ip_sockaddr_in      *group;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    struct Ip_sockaddr_in      *source;
#endif

#ifdef IPNET_USE_MCAST_ROUTING
    if (optname & IP_MRT_BASE)
        return ipnet_ip4_mcast_setsockopt(sock, optname, optval, optlen);
#endif /* IPNET_USE_MCAST_ROUTING */

#ifdef IPNET_USE_NAT
    if (optname == IP_IP_NAT)
        return (int)ipnet_nat_setsockopt(sock, optname, optval, optlen);
#endif /* IPNET_USE_NAT */

#ifdef IPFIREWALL
    if (optname == IP_IP_FW)
    {
        if (ipfirewall == IP_NULL)
            return -IP_ERRNO_EOPNOTSUPP;
        return (int)ipfirewall_setsockopt(sock, optname, optval, optlen);
    }
#endif /* IPFIREWALL */

    switch (optname)
    {
    case IP_IP_OPTIONS:
        if (optlen > 40 || sock->ip4->router_alert)
            return -IP_ERRNO_EINVAL;
        break;
    case IP_IP_ROUTER_ALERT:
        if (sock->ip4->opts != IP_NULL)
            /* IP_OPTIONS and IP_IP_ROUTER_ALERT are mutually exclusive */
            return -IP_ERRNO_EINVAL;
        /* else, fall through */
    case IP_IP_MULTICAST_LOOP:
        if (optlen == sizeof(int))
        {
            /* These options is an int on some platforms */
            val = *(int*) optval;
            break;
        }
        /* else, fall through */
    case IP_IP_MULTICAST_TTL:
    case IP_IP_DONTFRAG:
        if (optlen != sizeof(Ip_u8))
            return -IP_ERRNO_EINVAL;
        val_u8 = *(Ip_u8 *) optval;
        break;
    case IP_IP_HDRINCL:
    case IP_IP_TOS:
    case IP_IP_RECVTOS:
    case IP_IP_TTL:
    case IP_IP_RECVTTL:
    case IP_IP_RECVDSTADDR:
    case IP_IP_PKTINFO:
    case IP_IP_RECVIF:
    case IP_IP_X_SENDERLINKADDR:
        if (optlen != sizeof(int))
            return -IP_ERRNO_EINVAL;
        val = *(int*) optval;
        break;
    default:
        break;
    }

    switch (optname)
    {
    case IP_IP_OPTIONS:
        if (optlen == 0)
        {
            ipcom_free(sock->ip4->opts);
            sock->ip4->opts = IP_NULL;
            break;
        }
        if (sock->ip4->opts == IP_NULL)
        {
            sock->ip4->opts = ipcom_malloc(sizeof(struct Ipnet_ip4_sock_opts));
            if (sock->ip4->opts == IP_NULL)
                return -IP_ERRNO_ENOMEM;
        }
        val = ipnet_sockopt_ip4_set_ip_options(sock->ip4->opts, optval, optlen);
        if (val < 0)
            return val;
        break;
    case IP_IP_TOS:
        if (val == -1)
            sock->ip4->type_of_service = 0;
        else if (val < 0 || val > 0xff)
            return -IP_ERRNO_EINVAL;
        else
            sock->ip4->type_of_service = (Ip_u8) val;
        break;
    case IP_IP_RECVTOS:
        if (val)
            IP_BIT_SET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVTOS);
        else
            IP_BIT_CLR(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVTOS);
        break;
    case IP_IP_TTL:
        if (val == -1)
            sock->uni_hop_limit = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.inet.BaseHopLimit", 64);
        else if (val >= 0 && val <= 255)
            sock->uni_hop_limit = (Ip_u8)val;
        else
            return -IP_ERRNO_EINVAL;
        break;
    case IP_IP_RECVTTL:
        if (val)
            IP_BIT_SET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVTTL);
        else
            IP_BIT_CLR(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVTTL);
        break;
    case IP_IP_HDRINCL:
        if (val)
            IP_BIT_SET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_HDRINCL);
        else
            IP_BIT_CLR(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_HDRINCL);
        break;
    case IP_IP_RECVIF:
        if (val)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_RECV_IF);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_RECV_IF);
        break;
    case IP_IP_X_SENDERLINKADDR:
        if (val)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_SENDER_LINK_ADDR);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_SENDER_LINK_ADDR);
        break;
    case IP_IP_RECVDSTADDR:
        if (val)
            IP_BIT_SET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVDSTADDR);
        else
            IP_BIT_CLR(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVDSTADDR);
        break;
    case IP_IP_PKTINFO:
        if (val)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_RECVPKTINFO);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_RECVPKTINFO);
        break;
    case IP_IP_MULTICAST_TTL:
        if (val_u8 > 0)
            sock->multi_hop_limit = val_u8;
        else
            sock->multi_hop_limit = 1;
        break;
    case IP_IP_MULTICAST_LOOP:
        if (val_u8 || val)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST);
        break;
    case IP_IP_MULTICAST_IF:
        {
            struct Ip_in_addr   *ifaddr;

            if (optlen < sizeof(struct Ip_in_addr))
                return -IP_ERRNO_EINVAL;

            /* Use the size to determine what we're dealing with */
            if (optlen == sizeof(struct Ip_ip_mreqn))
            {
                /* This is a new 'linux' style mreqn */
                struct Ip_ip_mreqn  *mreqn = (struct Ip_ip_mreqn *) optval;

                /* Try resolve using the ifindex if specified */
                if (mreqn->imr_ifindex != 0)
                {
                    /* It' set; if we fail to resolve the interface, exit. */
                    netif = ipnet_if_indextonetif(sock->vr_index, mreqn->imr_ifindex);
                    if (netif == IP_NULL)
                        return -IP_ERRNO_EADDRNOTAVAIL;

                    /* Set the index */
                    sock->ip4->multicast_if = netif->ipcom.ifindex;
                    break;
                }

                /* Store address */
                ifaddr = &mreqn->imr_address;
            }
            else if (optlen == sizeof(struct Ip_ip_mreq))
            {
                /* This is an old BSD style mreq */
                mreq = (struct Ip_ip_mreq *) optval;
                ifaddr = &mreq->imr_interface;
            }
            else
            {
                /* This is a BSD style address only argument */
                ifaddr = (struct Ip_in_addr *) optval;
            }

            /* Resolve using the provided address */
            if (ifaddr->s_addr == IP_INADDR_ANY)
                sock->ip4->multicast_if = 0;
            else
            {
                netif = ipnet_sockopt_ip4_addr_to_netif(sock->vr_index, ifaddr);
                if (netif == IP_NULL)
                    return -IP_ERRNO_EADDRNOTAVAIL;
                sock->ip4->multicast_if = netif->ipcom.ifindex;
            }
        }

        break;
    case IP_IP_JOIN_GROUP:
    case IP_IP_LEAVE_GROUP:
        if (optlen < sizeof(struct Ip_ip_mreq))
            return -IP_ERRNO_EINVAL;

        mreq = (struct Ip_ip_mreq *) optval;

        /* Is it a new structure containing the ifindex? */
        netif = IP_NULL;
        if (optlen == sizeof(struct Ip_ip_mreqn))
        {
            struct Ip_ip_mreqn *mreqn = (struct Ip_ip_mreqn *) mreq;

            /* Try resolve using the ifindex if specified */
            if (mreqn->imr_ifindex != 0)
            {
                /* It' set; if we fail to resolve the interface, exit. */
                netif = ipnet_if_indextonetif(sock->vr_index, mreqn->imr_ifindex);
                if (netif == IP_NULL)
                    return -IP_ERRNO_EADDRNOTAVAIL;
            }
        }

        /* Resolve using the interface */
        if (netif == IP_NULL)
            netif = ipnet_sockopt_ip4_addr_to_netif(sock->vr_index, &mreq->imr_interface);

        if (netif != IP_NULL)
        {
            struct Ip_group_req gr_req;

            ipcom_memset(&gr_req, 0, sizeof(gr_req));
            gr_req.gr_interface = netif->ipcom.ifindex;
            group = (struct Ip_sockaddr_in *) &gr_req.gr_group;
            IPCOM_SA_LEN_SET(group, sizeof(gr_req.gr_group));
            group->sin_family = IP_AF_INET;
            group->sin_addr   = mreq->imr_multiaddr;
            return ipnet_setsockopt_ip4(sock,
                                        optname == IP_IP_JOIN_GROUP ? IP_MCAST_JOIN_GROUP : IP_MCAST_LEAVE_GROUP,
                                        &gr_req,
                                        sizeof(gr_req));
        }

        return -IP_ERRNO_EADDRNOTAVAIL;

    case IP_MCAST_JOIN_GROUP:
    case IP_MCAST_LEAVE_GROUP:
        gsreq = (struct Ip_group_source_req *) optval;
        if (optlen < sizeof(struct Ip_group_req)
            || gsreq->gsr_group.sa_family != IP_AF_INET
            || IPCOM_SA_LEN_GET(&gsreq->gsr_group) < sizeof(struct Ip_sockaddr_in))
            return -IP_ERRNO_EINVAL;

        netif = ipnet_if_indextonetif(sock->vr_index, gsreq->gsr_interface);
        if (netif == IP_NULL)
            return -IP_ERRNO_EADDRNOTAVAIL;

        group = (struct Ip_sockaddr_in *) &gsreq->gsr_group;

        if (group->sin_addr.s_addr == IP_INADDR_ANY)
            return ipnet_sockopt_enter_exit_promisc(sock, netif, optname == IP_MCAST_JOIN_GROUP);

        if (optname == IP_MCAST_LEAVE_GROUP)
            return ipnet_sock_ip4_mcast_delete_data(sock, netif, &group->sin_addr);

        return ipnet_sockopt_ip4_mcast_create_data(sock, netif, &group->sin_addr);

    case IP_MCAST_BLOCK_SOURCE:
    case IP_MCAST_UNBLOCK_SOURCE:
    case IP_MCAST_JOIN_SOURCE_GROUP:
    case IP_MCAST_LEAVE_SOURCE_GROUP:
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
        gsreq = (struct Ip_group_source_req *) optval;
        if (optlen < sizeof(struct Ip_group_source_req)
            || gsreq->gsr_group.sa_family != IP_AF_INET
            || IPCOM_SA_LEN_GET(&gsreq->gsr_group) < sizeof(struct Ip_sockaddr_in)
            || gsreq->gsr_source.sa_family != IP_AF_INET
            || IPCOM_SA_LEN_GET(&gsreq->gsr_source) < sizeof(struct Ip_sockaddr_in))
            return -IP_ERRNO_EINVAL;

        netif = ipnet_if_indextonetif(sock->vr_index, gsreq->gsr_interface);
        if (netif == IP_NULL)
            return -IP_ERRNO_EADDRNOTAVAIL;

        group = (struct Ip_sockaddr_in *) &gsreq->gsr_group;
        source = (struct Ip_sockaddr_in *) &gsreq->gsr_source;

        return ipnet_sockopt_ip4_mcast_add_remove_source(sock,
                                                         netif,
                                                         &group->sin_addr,
                                                         &source->sin_addr,
                                                         (optname == IP_MCAST_BLOCK_SOURCE
                                                          || optname == IP_MCAST_UNBLOCK_SOURCE
                                                          ? IPCOM_SET_IS_EXCLUDE
                                                          : IPCOM_SET_IS_INCLUDE),
                                                         (optname == IP_MCAST_BLOCK_SOURCE
                                                          || optname == IP_MCAST_JOIN_SOURCE_GROUP));
#else
        return -IP_ERRNO_EOPNOTSUPP;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    case IP_IP_ROUTER_ALERT:
        if (sock->ipcom.type != IP_SOCK_RAW)
            /* Is only allowed for RAW sockets */
            return -IP_ERRNO_EPERM;
        sock->ip4->router_alert = (Ip_u8)((val_u8 != 0)|| (val != 0));
        break;

    case IP_IP_DONTFRAG:
        sock->ip4->dont_frag = (Ip_u8)(val_u8 != 0);
        break;

    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    ipnet_sock_invalidate_ip4_cache(sock);
    ipnet_setsockopt_ip4_update_sticky_option_flag(sock);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_getsockopt_icmp
 *===========================================================================
 * Description: Get socket option where level was was set to IP_SOL_RAW and
 *              socket protocol is IP_IPPROTO_ICMP.
 * Parameters:  sock - The socket on which the option should be get.
 *              optname - The option to get.
 *              optval - The buffer into which the value should be copied.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_getsockopt_icmp(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen)
{
    switch (optname)
    {
    case IP_ICMP_FILTER:
        if (*optlen < sizeof(struct Ip_icmp_filter))
            return -IP_ERRNO_EINVAL;

        *optlen = sizeof(struct Ip_icmp_filter);
        ipcom_memcpy(optval, sock->ip4->icmp_filter, sizeof(struct Ip_icmp_filter));
        break;
    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_setsockopt_icmp
 *===========================================================================
 * Description: Set socket option where level was set to IP_SOL_RAW and
 *              socket protocol is IP_IPPROTO_ICMP..
 * Parameters:  sock - The socket on which the option should be set.
 *              optname - The option to set.
 *              optval - The value to set.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_setsockopt_icmp(Ipnet_socket *sock, int optname, IP_CONST void *optval, Ip_socklen_t optlen)
{
    switch (optname)
    {
    case IP_ICMP_FILTER:
        if (optlen != sizeof(struct Ip_icmp_filter))
            return -IP_ERRNO_EINVAL;
        ipcom_memcpy(sock->ip4->icmp_filter, optval, optlen);
        break;

    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    return 0;
}
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6

/*
 *===========================================================================
 *               ipnet_sockopt_ip6_mcast_create_data
 *===========================================================================
 * Description: Creates a socket control block for one multicast address
 *              controlled via the socket.
 * Parameters:  sock - The controlling socket for the multicast address.
 *              netif - The interface the multicast address will be assigned to.
 *              maddr - An multicast address.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_sockopt_ip6_mcast_create_data(Ipnet_socket *sock,
                                    Ipnet_netif *netif,
                                    struct Ip_in6_addr *maddr)
{

    Ipnet_ip6_sock_mcast_data **pstorage;
    Ipnet_ip6_sock_mcast_data  *mcast_data;
    int                         ret;

    if (IP_IN6_IS_ADDR_MULTICAST(maddr) == IP_FALSE)
        return -IP_ERRNO_EINVAL;

    pstorage = ipnet_sock_ip6_get_mcast_data(sock, netif, maddr);
    if (*pstorage != IP_NULL)
        /* The group is already joined */
        return -IP_ERRNO_EADDRINUSE;

    mcast_data = ipcom_malloc(sizeof(Ipnet_ip6_sock_mcast_data));
    if (mcast_data == IP_NULL)
        return -IP_ERRNO_ENOMEM;

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    mcast_data->sources = ipcom_set_new(sizeof(struct Ip_in6_addr));
    if (mcast_data->sources == IP_NULL)
    {
        ipcom_free(mcast_data);
        return -IP_ERRNO_ENOMEM;
    }
    mcast_data->sources->user = IPCOM_SET_IS_EXCLUDE;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    mcast_data->netif         = netif;
    mcast_data->maddr         = *maddr;
    mcast_data->next          = IP_NULL;
    *pstorage = mcast_data;
    ret = ipnet_ip6_add_addr(netif,
                             maddr,
                             0,
                             IPCOM_ADDR_INFINITE,
                             IPCOM_ADDR_INFINITE,
                             16);
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    if (ret >= 0)
    {
        Ipnet_ip6_addr_entry *addr_entry;

        addr_entry = ipnet_ip6_get_addr_entry(maddr, sock->vr_index, netif);
        if (addr_entry == IP_NULL)
            ret = -IP_ERRNO_ENXIO;
        else if (ipcom_set_add(addr_entry->mcast.sockets, &sock->ipcom.fd) != IPCOM_SUCCESS)
            ret = -IP_ERRNO_ENOMEM;
    }
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    if (ret < 0)
        (void)ipnet_sock_ip6_mcast_delete_data(sock, netif, maddr);
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    else
        ipnet_mldv2_report_change(netif, maddr);
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    return ret;
}


/*
 *===========================================================================
 *               ipnet_sockopt_ip6_mcast_add_remove_source
 *===========================================================================
 * Description: Adds or removes a multicast channel.
 * Parameters:  sock - The socket that contains the
 *              netif - The network interface the multicast address is assigned to.
 *              group - The group to add/remove.
 *              source - The source address to add/remove.
 *              filter_mode - IPCOM_SET_IS_INCLUDE or IPCOM_SET_IS_EXCLUDE
 *              is_add - IP_TRUE if the source address should be added.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_STATIC int
ipnet_sockopt_ip6_mcast_add_remove_source(Ipnet_socket *sock,
                                          Ipnet_netif *netif,
                                          struct Ip_in6_addr *group,
                                          struct Ip_in6_addr *source,
                                          int filter_mode,
                                          Ip_bool is_add)
{
    int                        ret = 0;
    Ipnet_ip6_sock_mcast_data *mcast_data;

    mcast_data = *ipnet_sock_ip6_get_mcast_data(sock, netif, group);
    if (mcast_data == IP_NULL)
    {
        if (is_add && filter_mode == IPCOM_SET_IS_INCLUDE)
        {
            /* Create a new data entry for the multicast address */
            ret = ipnet_sockopt_ip6_mcast_create_data(sock, netif, group);
            if (ret < 0)
                return ret;

            mcast_data = *ipnet_sock_ip6_get_mcast_data(sock, netif, group);
            mcast_data->sources->user = filter_mode;
        }
        if (mcast_data == IP_NULL)
            return -IP_ERRNO_EADDRNOTAVAIL;
    }

    if (mcast_data->sources->user != filter_mode)
        return -IP_ERRNO_EINVAL;

    if (!is_add)
    {
        if (ipcom_set_remove(mcast_data->sources, source) != IPCOM_SUCCESS)
            ret = -IP_ERRNO_EADDRNOTAVAIL;
        else if (filter_mode == IPCOM_SET_IS_INCLUDE
                 && mcast_data->sources->obj_count == 0)
        {
            /* Not member of any channels that contains this multicast address */
            ret = ipnet_sock_ip6_mcast_delete_data(sock,
                                                   netif,
                                                   group);
        }
    }
    else
    {
        switch (ipcom_set_add(mcast_data->sources, source))
        {
        case IPCOM_ERR_ALREADY_INSTALLED:
            ret = -IP_ERRNO_EADDRNOTAVAIL;
            break;
        case IPCOM_ERR_NO_MEMORY:
            ret = -IP_ERRNO_ENOMEM;
            break;
        default:
            break;
        }
    }

    if (ret == 0)
    {
        ipnet_mldv2_report_change(netif, group);
        IPCOM_LOG5(INFO, "IPv6: %s source %s to group %s at interface %s, filter mode is %s",
                   is_add ? "added" : "removed",
                   ipcom_inet_ntop(IP_AF_INET6, source, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   ipcom_inet_ntop(IP_AF_INET6, group, ipnet->log_buf + IP_INET6_ADDRSTRLEN, sizeof(ipnet->log_buf)),
                   netif->ipcom.name,
                   filter_mode == IPCOM_SET_IS_INCLUDE ? "include" : "exclude");
    }

    return ret;
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */


/*
 *===========================================================================
 *                    ipnet_getsockopt_ip6
 *===========================================================================
 * Description: Get socket option where level was set to IP_IPPROTO_IPV6.
 * Parameters:  sock - The socket on which the option should be get.
 *              optname - The option to get.
 *              optval - The buffer into which the value should be copied.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_getsockopt_ip6(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen)
{
    Ip_socklen_t actual_optlen = 0;
    int          val = 0;
    void        *ptr = IP_NULL;

#ifdef IPNET_USE_MCAST_ROUTING
    if (optname & IP_MRT6_BASE)
        return ipnet_ip6_mcast_getsockopt(sock, optname, optval, optlen);
#endif /* IPNET_USE_MCAST_ROUTING */

#ifdef IPFIREWALL
    if (optname == IP_IPV6_FW)
    {
        if (ipfirewall == IP_NULL)
            return -IP_ERRNO_EOPNOTSUPP;
        return (int)ipfirewall_getsockopt(sock, optname, optval, optlen);
    }
#endif /* IPFIREWALL */


    /* Determine length (or minimal length) of option */
    switch (optname)
    {
    case IP_IPV6_PKTINFO:
        actual_optlen = sizeof(struct Ip_in6_pktinfo);
        break;
    case IP_IPV6_NEXTHOP:
        actual_optlen = sizeof(struct Ip_sockaddr_in6);
        break;
    case IP_IPV6_RTHDR:
        ptr = sock->ip6->exthdr_rthdr;
        break;
    case IP_IPV6_HOPOPTS:
        ptr = sock->ip6->exthdr_hbh;
        break;
    case IP_IPV6_DSTOPTS:
        ptr = sock->ip6->exthdr_dst;
        break;
    case IP_IPV6_RTHDRDSTOPTS:
        ptr = sock->ip6->exthdr_rthdrdst;
        break;
    default:
        actual_optlen = sizeof(int);
        break;
    }
    if (ptr != IP_NULL)
        actual_optlen = (((Ip_pkt_ip6_ext_hdr *) ptr)->len + 1) * 8;

    if (*optlen < actual_optlen)
    {
        *optlen = actual_optlen;
        return -IP_ERRNO_ENOBUFS;
    }
    *optlen = actual_optlen;

    /* Copy the option value into the user buffer */
    switch (optname)
    {
    case IP_IPV6_V6ONLY:
        val = (int) IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_V6ONLY);
        break;
    case IP_IPV6_CHECKSUM:
        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM))
            val = -1;
        else
            val = sock->chk_offset;
        break;
    case IP_IPV6_UNICAST_HOPS:
        val = sock->uni_hop_limit;
        break;
    case IP_IPV6_MULTICAST_HOPS:
        val = sock->multi_hop_limit;
        break;
    case IP_IPV6_RECVIF:
        val = IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_RECV_IF);
        break;
    case IP_IPV6_X_SENDERLINKADDR:
        val = IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_SENDER_LINK_ADDR);
        break;
    case IP_IPV6_X_UDPENCAP:
        val = IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVENCAP);
        break;
    case IP_IPV6_RECVPKTINFO:
        val = IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_RECVPKTINFO);
        break;
    case IP_IPV6_RECVHOPLIMIT:
        val = IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVHOPLIMIT);
        break;
    case IP_IPV6_RECVTCLASS:
        val = IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVTCLASS);
        break;
    case IP_IPV6_RECVRTHDR:
        val = IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVRTHDR);
        break;
    case IP_IPV6_RECVHOPOPTS:
        val = IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVHOPOPTS);
        break;
    case IP_IPV6_RECVDSTOPTS:
        val = IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVDSTOPTS);
        break;
    case IP_IPV6_RECVSAINFO:
        val = IP_BIT_ISTRUE(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVSAINFO);
        break;
    case IP_IPV6_MULTICAST_LOOP:
        val = IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST);
        break;
    case IP_IPV6_ADDR_PREFERENCES:
        val = sock->ip6->prefer_src_flags;
        break;
    case IP_IPV6_MULTICAST_IF:
        val = (int)sock->ip6->multicast_if;
        break;
    case IP_IPV6_TCLASS:
        val = sock->ip6->tclass;
        break;
    case IP_IPV6_PKTINFO:
        ptr = sock->ip6->pktinfo;
        if (ptr == IP_NULL)
        {
            /* Must return a IPV6_PKTINFO option with ifindex == 0 and
               the unspecified address if no set */
            ipcom_memset(optval, 0, actual_optlen);
            return 0;
        }
        break;
    case IP_IPV6_NEXTHOP:
        ptr = sock->ip6->nexthop;
        break;
    case IP_IPV6_RTHDR:
    case IP_IPV6_HOPOPTS:
    case IP_IPV6_DSTOPTS:
    case IP_IPV6_RTHDRDSTOPTS:
        /* 'ptr' is already set */
        break;
    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    if (actual_optlen > 0)
    {
        if (ptr != IP_NULL)
            ipcom_memcpy(optval, ptr, actual_optlen);
        else
            *(int*) optval = val;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_setsockopt_ip6_update_sticky_option_flag
 *===========================================================================
 * Description: Sets the sticky option flag if sticky option(s) is
 *              present, clears the flag otherwise.
 * Parameters:  sock - The socket on which the option should be set.
 *              optname - The option to set.
 *              optval - The value to set.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC void
ipnet_setsockopt_ip6_update_sticky_option_flag(Ipnet_socket *sock)
{
    if (sock->ip6->pktinfo != IP_NULL
        || sock->ip6->nexthop != IP_NULL
        || sock->ip6->tclass != 0
        || sock->ip6->exthdr_hbh != IP_NULL
        || sock->ip6->exthdr_dst != IP_NULL
        || sock->ip6->exthdr_rthdr != IP_NULL
        || sock->ip6->exthdr_rthdrdst != IP_NULL
        || sock->ip6->prefer_src_flags != 0)
    {
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_HAS_STICKY_OPT);
    }
    else
        IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_HAS_STICKY_OPT);
}


/*
 *===========================================================================
 *                    ipnet_setsockopt_ip6
 *===========================================================================
 * Description: Set socket option where level was set to IP_IPPROTO_IPV6.
 * Parameters:  sock - The socket on which the option should be set.
 *              optname - The option to set.
 *              optval - The value to set.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
/*lint -efunc(429,ipnet_setsockopt_ip6)  Custodial pointer 'ptr' (line X) has not been freed or returned*/
IP_STATIC int
ipnet_setsockopt_ip6(Ipnet_socket *sock, int optname, IP_CONST void *optval, Ip_socklen_t optlen)
{
    struct Ip_ipv6_mreq        *mreq = (void *)optval;
    struct Ip_group_source_req *gsreq = (void *)optval;
    int                         val = 0;
    void                       *ptr = IP_NULL;
    void                       *old_ptr = IP_NULL;
    Ipnet_netif                *netif;
    Ip_socklen_t                actual_optlen;
    Ip_bool                     is_minimal_len = IP_FALSE;
    struct Ip_sockaddr_in6     *group;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    struct Ip_sockaddr_in6     *source;
#endif

#ifdef IPNET_USE_MCAST_ROUTING
    if (optname & IP_MRT6_BASE)
        return ipnet_ip6_mcast_setsockopt(sock, optname, optval, optlen);
#endif /* IPNET_USE_MCAST_ROUTING */

#ifdef IPFIREWALL
    if (optname == IP_IPV6_FW)
    {
        if (ipfirewall == IP_NULL)
            return -IP_ERRNO_EOPNOTSUPP;
        return (int)ipfirewall_setsockopt(sock, optname, optval, optlen);
    }
#endif /* IPFIREWALL */

    switch (optname)
    {
    case IP_IPV6_PKTINFO:
    case IP_IPV6_NEXTHOP:
    case IP_IPV6_RTHDR:
    case IP_IPV6_HOPOPTS:
    case IP_IPV6_DSTOPTS:
    case IP_IPV6_RTHDRDSTOPTS:
        if (optlen == 0)
            /* Remove sticky option */
            goto remove_sticky_option;
        break;
    default:
        break;
    }

    /* Set the actual length of the option */
    switch (optname)
    {
    case IP_IPV6_PKTINFO:
        actual_optlen = sizeof(struct Ip_in6_pktinfo);
        break;
    case IP_IPV6_NEXTHOP:
        actual_optlen = sizeof(struct Ip_sockaddr_in6);
        is_minimal_len = IP_TRUE;
        break;
    case IP_IPV6_RTHDR:
        actual_optlen = sizeof(struct Ip_ip6_rthdr);
        is_minimal_len = IP_TRUE;
        break;
    case IP_IPV6_HOPOPTS:
        actual_optlen = sizeof(struct Ip_ip6_hbh);
        is_minimal_len = IP_TRUE;
        break;
    case IP_IPV6_DSTOPTS:
    case IP_IPV6_RTHDRDSTOPTS:
        actual_optlen = sizeof(struct Ip_ip6_dest);
        is_minimal_len = IP_TRUE;
        break;
    case IP_IPV6_ADD_MEMBERSHIP:
    case IP_IPV6_DROP_MEMBERSHIP:
        actual_optlen = sizeof(struct Ip_ipv6_mreq);
        break;
    case IP_IPV6_JOIN_ANYCAST:
    case IP_IPV6_LEAVE_ANYCAST:
        actual_optlen = sizeof(struct Ip_ipv6_mreq);
        break;
    case IP_MCAST_JOIN_GROUP:
    case IP_MCAST_LEAVE_GROUP:
        actual_optlen = sizeof(struct Ip_group_req);
        break;
    case IP_MCAST_BLOCK_SOURCE:
    case IP_MCAST_UNBLOCK_SOURCE:
    case IP_MCAST_JOIN_SOURCE_GROUP:
    case IP_MCAST_LEAVE_SOURCE_GROUP:
        actual_optlen = sizeof(struct Ip_group_source_req);
        break;
    default:
        actual_optlen = sizeof(int);
        break;
    }

    /* Check that the options has the correct length */
    if ((is_minimal_len && optlen < actual_optlen)
        || (is_minimal_len == IP_FALSE && optlen != actual_optlen))
    {
        return -IP_ERRNO_EINVAL;
    }

 remove_sticky_option:
    /* Copy the value to a local buffer */
    switch (optname)
    {
    case IP_IPV6_RTHDR:
    case IP_IPV6_PKTINFO:   /*lint !e616 control flows into case/default */
    case IP_IPV6_NEXTHOP:
    case IP_IPV6_HOPOPTS:
    case IP_IPV6_DSTOPTS:
    case IP_IPV6_RTHDRDSTOPTS:
        ptr = ipcom_malloc(optlen);/*lint !e423 */
        if (ptr == IP_NULL)
            return -IP_ERRNO_ENOMEM;
        ipcom_memcpy(ptr, optval, optlen);
        break;
    default:
        val = *(int*) optval;
        break;
    }

    ipnet_sock_invalidate_ip6_cache(sock);

    /* Set the value */
    switch (optname)
    {
    case IP_IPV6_V6ONLY:
        if (val)
            IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_V6ONLY);
        else
            IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_V6ONLY);
        break;
    case IP_IPV6_CHECKSUM:
        if (sock->proto == IP_IPPROTO_TCP
            || sock->proto == IP_IPPROTO_UDP
            || sock->proto == IP_IPPROTO_ICMPV6)
        {
            /* Can't change the checksum offset for those protocols. */
            return -IP_ERRNO_EACCES;
        }
        if (val == -1)
        {
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM);
            sock->chk_offset = 0;
        }
        else
        {
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM);
            sock->chk_offset = (Ip_u16) val;
        }
        break;
    case IP_IPV6_RECVIF:
        if (val)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_RECV_IF);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_RECV_IF);
        break;
    case IP_IPV6_X_UDPENCAP:
        if (val)
            IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVENCAP);
        else
            IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVENCAP);
        break;
    case IP_IPV6_X_SENDERLINKADDR:
        if (val)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_SENDER_LINK_ADDR);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_SENDER_LINK_ADDR);
        break;
    case IP_IPV6_RECVPKTINFO:
        if (val)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_RECVPKTINFO);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_RECVPKTINFO);
        break;
    case IP_IPV6_RECVHOPLIMIT:
        if (val)
            IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVHOPLIMIT);
        else
            IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVHOPLIMIT);
        break;
    case IP_IPV6_RECVTCLASS:
        if (val)
            IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVTCLASS);
        else
            IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVTCLASS);
        break;
    case IP_IPV6_RECVRTHDR:
        if (val)
            IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVRTHDR);
        else
            IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVRTHDR);
        break;
    case IP_IPV6_RECVHOPOPTS:
        if (val)
            IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVHOPOPTS);
        else
            IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVHOPOPTS);
        break;
    case IP_IPV6_RECVDSTOPTS:
        if (val)
            IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVDSTOPTS);
        else
            IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVDSTOPTS);
        break;
    case IP_IPV6_RECVSAINFO:
        if (val)
            IP_BIT_SET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVSAINFO);
        else
            IP_BIT_CLR(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVSAINFO);
        break;
    case IP_IPV6_UNICAST_HOPS:
        if (val == -1)
            sock->uni_hop_limit = IPNET_IP6_OUTPUT_PARAM_USE_DEFAULT_HLIM;
        else if (val >= 0 && val <= 255)
            sock->uni_hop_limit = (Ip_u8) val;
        else
            return -IP_ERRNO_EINVAL;
        break;
    case IP_IPV6_MULTICAST_HOPS:
        if (val == -1)
            sock->multi_hop_limit = 1;
        else if (val >= 0 && val <= 255)
            sock->multi_hop_limit = (Ip_u8)val;
        else
            return -IP_ERRNO_EINVAL;
        break;
    case IP_IPV6_MULTICAST_LOOP:
        if (val)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST);
        break;
    case IP_IPV6_MULTICAST_IF:
        if (val < 0)
            return -IP_ERRNO_EINVAL;

        if (val == 0)
        {
            /* Let the stack choose again */
            sock->ip6->multicast_if = 0;
            return 0;
        }

        if (ipnet_if_indextonetif(sock->vr_index, (Ip_u32) val) == IP_NULL)
            return -IP_ERRNO_ENXIO;

        sock->ip6->multicast_if = (Ip_u32)val;
        return 0;
    case IP_IPV6_JOIN_ANYCAST:
    case IP_IPV6_LEAVE_ANYCAST:
        {
            Ipnet_ip6_addr_entry *addr;

            netif = ipnet_if_indextonetif(sock->vr_index, mreq->ipv6mr_interface);
            if (netif == IP_NULL)
                return -IP_ERRNO_ENXIO;

            addr = ipnet_ip6_get_addr_entry(&mreq->ipv6mr_acaddr, netif->vr_index, netif);

            /* Sanity */
            if (addr && addr->type != IPNET_ADDR_TYPE_ANYCAST)
                return -IP_ERRNO_EINVAL;

            /* Join or leave */
            if (optname == IP_IPV6_JOIN_ANYCAST)
            {
                return ipnet_ip6_add_addr(netif,
                                          &mreq->ipv6mr_acaddr,
                                          IPNET_IP6_ADDR_FLAG_DONT_ADD_NETWORK_RT | IPNET_IP6_ADDR_FLAG_ANYCAST,
                                          IPCOM_ADDR_INFINITE,
                                          IPCOM_ADDR_INFINITE,
                                          128);
            }
            else
            {
                return ipnet_ip6_remove_addr(netif,
                                             &mreq->ipv6mr_acaddr);
            }
        }

    case IP_IPV6_JOIN_GROUP:
    case IP_IPV6_LEAVE_GROUP:
        {
            struct Ip_group_req gr_req;

            ipcom_memset(&gr_req, 0, sizeof(gr_req));
            gr_req.gr_interface = mreq->ipv6mr_interface;
            group = (struct Ip_sockaddr_in6 *) &gr_req.gr_group;
            IPCOM_SA_LEN_SET(group, sizeof(gr_req.gr_group));
            group->sin6_family = IP_AF_INET6;
            group->sin6_addr   = mreq->ipv6mr_multiaddr;
            return ipnet_setsockopt_ip6(sock,
                                        optname == IP_IPV6_JOIN_GROUP ? IP_MCAST_JOIN_GROUP : IP_MCAST_LEAVE_GROUP,
                                        &gr_req,
                                        sizeof(gr_req));
        }
    case IP_MCAST_JOIN_GROUP:
    case IP_MCAST_LEAVE_GROUP:
        gsreq = (struct Ip_group_source_req *) optval;
        if (optlen < sizeof(struct Ip_group_req)
            || gsreq->gsr_group.sa_family != IP_AF_INET6
            || IPCOM_SA_LEN_GET(&gsreq->gsr_group) < sizeof(struct Ip_sockaddr_in6))
            return -IP_ERRNO_EINVAL;

        netif = ipnet_if_indextonetif(sock->vr_index, gsreq->gsr_interface);
        if (netif == IP_NULL)
            return -IP_ERRNO_EADDRNOTAVAIL;

        group = (struct Ip_sockaddr_in6 *) &gsreq->gsr_group;

        if (IP_IN6_IS_ADDR_UNSPECIFIED(&group->sin6_addr))
            return ipnet_sockopt_enter_exit_promisc(sock, netif, optname == IP_MCAST_JOIN_GROUP);

        if (optname == IP_MCAST_LEAVE_GROUP)
            return ipnet_sock_ip6_mcast_delete_data(sock, netif, &group->sin6_addr);

        return ipnet_sockopt_ip6_mcast_create_data(sock, netif, &group->sin6_addr);

    case IP_MCAST_BLOCK_SOURCE:
    case IP_MCAST_UNBLOCK_SOURCE:
    case IP_MCAST_JOIN_SOURCE_GROUP:
    case IP_MCAST_LEAVE_SOURCE_GROUP:
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
        gsreq = (struct Ip_group_source_req *) optval;
        if (optlen < sizeof(struct Ip_group_source_req)
            || gsreq->gsr_group.sa_family != IP_AF_INET6
            || IPCOM_SA_LEN_GET(&gsreq->gsr_group) < sizeof(struct Ip_sockaddr_in6)
            || gsreq->gsr_source.sa_family != IP_AF_INET6
            || IPCOM_SA_LEN_GET(&gsreq->gsr_source) < sizeof(struct Ip_sockaddr_in6))
            return -IP_ERRNO_EINVAL;

        netif = ipnet_if_indextonetif(sock->vr_index, gsreq->gsr_interface);
        if (netif == IP_NULL)
            return -IP_ERRNO_EADDRNOTAVAIL;

        group = (struct Ip_sockaddr_in6 *) &gsreq->gsr_group;
        source = (struct Ip_sockaddr_in6 *) &gsreq->gsr_source;

        return ipnet_sockopt_ip6_mcast_add_remove_source(sock,
                                                         netif,
                                                         &group->sin6_addr,
                                                         &source->sin6_addr,
                                                         (optname == IP_MCAST_BLOCK_SOURCE
                                                          || optname == IP_MCAST_UNBLOCK_SOURCE
                                                          ? IPCOM_SET_IS_EXCLUDE
                                                          : IPCOM_SET_IS_INCLUDE),
                                                         (optname == IP_MCAST_BLOCK_SOURCE
                                                          || optname == IP_MCAST_JOIN_SOURCE_GROUP));
#else
        return -IP_ERRNO_EOPNOTSUPP;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    case IP_IPV6_TCLASS:
        if (val == -1)
            sock->ip6->tclass = 0;
        else if (val >= 0 && val <= 255)
            sock->ip6->tclass = (Ip_u8)val;
        else
            return -IP_ERRNO_EINVAL;
        break;
    case IP_IPV6_PKTINFO:
        if (sock->proto == IP_IPPROTO_TCP
            && IP_IN6_IS_ADDR_UNSPECIFIED(&((struct Ip_in6_pktinfo *) ptr)->ipi6_addr) == IP_FALSE)
            /* It is not allowed to change the source address for TCP */
        {
            ipcom_free(ptr);
            return -IP_ERRNO_EINVAL;
        }

        old_ptr = sock->ip6->pktinfo;
        sock->ip6->pktinfo = ptr;

        if (sock->ip6->pktinfo->ipi6_ifindex == 0
            && IP_IN6_IS_ADDR_UNSPECIFIED(&sock->ip6->pktinfo->ipi6_addr))
        {
            /* Set the default value */
            sock->ip6->pktinfo = IP_NULL;
            ipcom_free(ptr);
        }
        break;
    case IP_IPV6_NEXTHOP:
        old_ptr = sock->ip6->nexthop;
        sock->ip6->nexthop = ptr;
        break;
    case IP_IPV6_RTHDR:
        /* Only allow rthdr type 0 as sticky */
        if (((Ip_pkt_ip6_rthdr *) ptr)->ip6r_type != IP_IPV6_RTHDR_TYPE_0)
        {
            ipcom_free(ptr);
            return -IP_ERRNO_EINVAL;
        }
        old_ptr = sock->ip6->exthdr_rthdr;
        sock->ip6->exthdr_rthdr = ptr;
        break;
    case IP_IPV6_HOPOPTS:
        old_ptr = sock->ip6->exthdr_hbh;
        sock->ip6->exthdr_hbh = ptr;
        break;
    case IP_IPV6_DSTOPTS:
        old_ptr = sock->ip6->exthdr_dst;
        sock->ip6->exthdr_dst = ptr;
        break;
    case IP_IPV6_RTHDRDSTOPTS:
        old_ptr = sock->ip6->exthdr_rthdrdst;
        sock->ip6->exthdr_rthdrdst = ptr;
        break;
    case IP_IPV6_ADDR_PREFERENCES:
        sock->ip6->prefer_src_flags = (Ip_u8) val;
        break;
    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    ipnet_setsockopt_ip6_update_sticky_option_flag(sock);
    ipcom_free(old_ptr);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_getsockopt_icmp6
 *===========================================================================
 * Description: Get socket option where level was set to IP_IPPROTO_ICMPV6.
 * Parameters:  sock - The socket on which the option should be get.
 *              optname - The option to get.
 *              optval - The buffer into which the value should be copied.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_getsockopt_icmp6(Ipnet_socket *sock, int optname, void *optval, Ip_socklen_t *optlen)
{
    switch (optname)
    {
    case IP_ICMP6_FILTER:
        if (*optlen < sizeof(struct Ip_icmp6_filter))
            return -IP_ERRNO_EINVAL;

        *optlen = sizeof(struct Ip_icmp6_filter);
        ipcom_memcpy(optval, sock->ip6->icmp6_filter, sizeof(struct Ip_icmp6_filter));
        break;
    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_setsockopt_icmp6
 *===========================================================================
 * Description: Set socket option where level was set to IP_IPPROTO_ICMPV6.
 * Parameters:  sock - The socket on which the option should be set.
 *              optname - The option to set.
 *              optval - The value to set.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_setsockopt_icmp6(Ipnet_socket *sock, int optname, IP_CONST void *optval, Ip_socklen_t optlen)
{
    switch (optname)
    {
    case IP_ICMP6_FILTER:
        if (optlen != sizeof(struct Ip_icmp6_filter))
            return -IP_ERRNO_EINVAL;
        ipcom_memcpy(sock->ip6->icmp6_filter, optval, optlen);
        break;

    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sockopt_ip4_addr_to_netif
 *===========================================================================
 * Description: Returns the interface that has the specified address.
 * Parameters:  vr - The virtual router the returned interface must be
 *                   assigned to.
 *              addr - The address the interface must have.
 * Returns:     Pointer to the interface structure or IP_NULL if no interface
 *              has the specified address.
 *
 */
#ifdef IPCOM_USE_INET
IP_GLOBAL Ipnet_netif *
ipnet_sockopt_ip4_addr_to_netif(Ip_u16 vr, struct Ip_in_addr *in_addr)
{
    Ipnet_ip4_addr_entry *addr;

    if (in_addr->s_addr == IP_INADDR_ANY)
    {
        /* Return the default multicast interface in this case */
        struct Ip_in_addr  mcast_addr;
        Ipnet_route_entry *rt;

        mcast_addr.s_addr = ip_htonl(IP_INADDR_UNSPEC_GROUP);
        if (ipnet_route_lookup(IP_AF_INET, vr, IPCOM_ROUTE_TABLE_DEFAULT, 0, &mcast_addr, 0, 0, &rt) >= 0)
            return rt->netif;
    }

    addr = ipnet_ip4_get_addr_entry(in_addr->s_addr, vr, IP_NULL);
    return addr != IP_NULL ? addr->netif : IP_NULL;
}
#endif /* IPCOM_USE_INET */


/*
 ****************************************************************************
 * 11                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sys_getsockopt
 *===========================================================================
 * Description: Gets the specified option on a socket. Options may
 *              exist at multiple protocol levels.
 * Parameters:  fd - A socket descriptor.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Pointer to a buffer where the option data should be
 *              copied..
 *              optlen - The length of the buffer poited to by optval.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_GLOBAL int
ipnet_sys_getsockopt(Ip_fd fd,
                     int level,
                     int optname,
                     void *optval,
                     Ip_socklen_t *optlen)
{
    Ipnet_socket *sock;
    int           err = -IP_ERRNO_EINVAL;

    ip_assert2(optval != IP_NULL);
    ip_assert2(fd >= IPNET_SOCKET_STARTFD);
    ip_assert2(fd < ipnet_conf_max_sockets);
    ip_assert2(ipnet->sockets[fd] != IP_NULL);

    IPNET_FD_TO_SOCKET(fd, sock);

    if (sock->ops->getopt != IP_NULL)
    {
        err = sock->ops->getopt(sock, level, optname, optval, optlen);
        if (err != -IP_ERRNO_EOPNOTSUPP)
            return err;
        /* else: try the IPNET socket options handlers */
    }

    switch (level)
    {
    case IP_SOL_SOCKET:
        err = ipnet_getsockopt_sol(sock, optname, optval, optlen);
        break;
    case IP_IPPROTO_UDP:
        err = ipnet_getsockopt_udp(sock, optname, optval, optlen);
        break;
    case IP_SOL_RAW:
#ifdef IPCOM_USE_INET
        if (sock->ipcom.domain == IP_AF_INET && sock->proto == IP_IPPROTO_ICMP)
            err = ipnet_getsockopt_icmp(sock, optname, optval, optlen);
#endif /* IPCOM_USE_INET */
        break;
#ifdef IPCOM_USE_INET
    case IP_IPPROTO_IP:
        if (sock->ipcom.domain == IP_AF_INET)
            err = ipnet_getsockopt_ip4(sock, optname, optval, optlen);
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_IPPROTO_IPV6:
        if (sock->ipcom.domain == IP_AF_INET6)
            err = ipnet_getsockopt_ip6(sock, optname, optval, optlen);
        break;
    case IP_IPPROTO_ICMPV6:
        if (sock->ipcom.domain == IP_AF_INET6 && sock->proto == IP_IPPROTO_ICMPV6)
            err = ipnet_getsockopt_icmp6(sock, optname, optval, optlen);
        break;
#endif
#ifdef IPCOM_USE_PACKET
    case IP_SOL_PACKET:
        err = ipnet_packet_getsockopt(optname, optval, optlen);
        break;
#endif /* IPCOM_USE_PACKET */
    default:
        err = -IP_ERRNO_EOPNOTSUPP;
        break;
    }

    return err;
}


/*
 *===========================================================================
 *                    ipnet_sys_setsockopt
 *===========================================================================
 * Description: Sets the specified option on a socket. Options may
 *              exist at multiple protocol levels.
 * Parameters:  fd - A socket descriptor.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Data to set for the option.
 *              optlen - The length of the buffer poited to by optval.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_GLOBAL int
ipnet_sys_setsockopt(Ip_fd fd,
                     int level,
                     int optname,
                     IP_CONST void *optval,
                     Ip_socklen_t optlen)
{
    Ipnet_socket  *sock;
    int            err = -IP_ERRNO_EINVAL;

    ip_assert2(optval != IP_NULL);
    ip_assert2(fd >= IPNET_SOCKET_STARTFD);
    ip_assert2(fd < ipnet_conf_max_sockets);
    ip_assert2(ipnet->sockets[fd] != IP_NULL);

    IPNET_FD_TO_SOCKET(fd, sock);

    if (sock->ops->setopt != IP_NULL)
    {
        err = sock->ops->setopt(sock, level, optname, optval, optlen);
        if (err != -IP_ERRNO_EOPNOTSUPP)
            return err;
        /* else: try the IPNET socket options handlers */
    }

    switch (level)
    {
    case IP_SOL_SOCKET:
        err = ipnet_setsockopt_sol(sock, optname, optval, optlen);
        break;
    case IP_IPPROTO_UDP:
        err = ipnet_setsockopt_udp(sock, optname, optval, optlen);
        break;
    case IP_SOL_RAW:
#ifdef IPCOM_USE_INET
        if (sock->ipcom.domain == IP_AF_INET && sock->proto == IP_IPPROTO_ICMP)
            err = ipnet_setsockopt_icmp(sock, optname, optval, optlen);
#endif /* IPCOM_USE_INET */
        break;
#ifdef IPCOM_USE_INET
    case IP_IPPROTO_IP:
        if (sock->ipcom.domain == IP_AF_INET)
            err = ipnet_setsockopt_ip4(sock, optname, optval, optlen);
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IP_IPPROTO_IPV6:
        if (sock->ipcom.domain == IP_AF_INET6)
            err = ipnet_setsockopt_ip6(sock, optname, optval, optlen);
        break;
    case IP_IPPROTO_ICMPV6:
        if (sock->ipcom.domain == IP_AF_INET6 && sock->proto == IP_IPPROTO_ICMPV6)
            err = ipnet_setsockopt_icmp6(sock, optname, optval, optlen);
        break;
#endif /* IPCOM_USE_INET6 */
#ifdef IPCOM_USE_PACKET
    case IP_SOL_PACKET:
        err = ipnet_packet_setsockopt(sock, optname, optval, optlen);
        break;
#endif /* IPCOM_USE_PACKET */
    default:
        err = -IP_ERRNO_EOPNOTSUPP;
        break;
    }

    return err;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

