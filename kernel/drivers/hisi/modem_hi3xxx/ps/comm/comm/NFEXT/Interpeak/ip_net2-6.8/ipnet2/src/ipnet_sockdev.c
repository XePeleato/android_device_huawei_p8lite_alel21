/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_sockdev.c,v $ $Revision: 1.6 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_sockdev.c,v $
 *     $Author: kenneth $
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
 * Application device interface implementation.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_USE_SOCKDEV

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_syslog.h>

#include "ipnet_h.h"
#include "ipnet_netif.h"
#include "ipnet_sockdev.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

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

/*
 *===========================================================================
 *                    ipnet_sockdev_ioctl
 *===========================================================================
 * Description: IO control. Accept join/leave from multicast groups.
 * Parameters:  netif - The network interface on which to perform the operation.
 *              command - The command to perform.
 *              data - Data associated with the command.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_sockdev_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    struct Ip_ifreq *ifreq = data;
    int              ret = 0;

    switch (command)
    {
    case IP_SIOCXOPEN:
        ipnet_kioevent(netif, IP_EIOXRUNNING, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        ipnet_kioevent(netif, IP_EIOXUP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCXCLOSE:
        ipnet_kioevent(netif, IP_EIOXDOWN, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        ipnet_kioevent(netif, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCGIFLLADDR:
        ifreq->ip_ifr_addr.sa_family = IP_AF_LINK;
        /* NOTE: BSD incorrectly sets the length to 0 here -> hence so must we! */
        ifreq->ip_ifr_addr.sa_len = 0;
        break;

#ifdef IPCOM_USE_INET
    case IP_SIOCXADDMULTI_IN:
    case IP_SIOCXDELMULTI_IN:
#endif
#ifdef IPCOM_USE_INET6
    case IP_SIOCXADDMULTI_IN6:
    case IP_SIOCXDELMULTI_IN6:
#endif
        break;
    default:
        return -IP_ERRNO_EINVAL;
    }
    return ret;
}


/*
 *===========================================================================
 *                         ipnet_sockdev_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_sockdev_ioevent(Ipcom_netif *ipcomif, int command, void *data, int fcflags)
{
    int ret = 0;

    IPCOM_UNUSED_ARG(ipcomif);
    IPCOM_UNUSED_ARG(data);

    if (IP_BIT_ISFALSE(fcflags, IP_FLAG_FC_ISRCONTEXT | IP_FLAG_FC_STACKCONTEXT))
        IPNET_CODE_LOCK_WRITE();

    switch (command)
    {
    case IP_EIOXUP:
        break;
    case IP_EIOXDOWN:
        break;
    default:
        ret = -1;
        break;
    }

    if (IP_BIT_ISFALSE(fcflags, IP_FLAG_FC_ISRCONTEXT | IP_FLAG_FC_STACKCONTEXT))
        IPNET_CODE_UNLOCK();

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sockdev_link_input
 *===========================================================================
 * Description: Handles socket device input (from user send()).
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_sockdev_link_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    pkt->ifindex = netif->ipcom.ifindex;
    switch (pkt->data[pkt->start] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        return ipnet_ip4_input(netif, pkt);
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case 0x60:
        return ipnet_ip6_input(netif, pkt);
#endif /* IPCOM_USE_INET6 */

    default:
        return -IP_ERRNO_EPROTONOSUPPORT;
    }
}


/*
 *===========================================================================
 *                    ipnet_sockdev_drv_output
 *===========================================================================
 * Description: Queues the packet on the socket that matches the sockdev
 *              network interface.
 * Parameters:  netif - The interface the packet was sent to.
 *              pkt - The packet to sent.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_sockdev_drv_output(Ipcom_netif *netif, Ipcom_pkt *pkt)
{
    int ret;

    ret = ipnet_queue_received_packet(pkt, netif->plink);
    if (ret == -IP_ERRNO_EMSGSIZE)
        return -IP_ERRNO_EWOULDBLOCK;
    if (ret >= 0)
    {
        IPCOM_PKT_ADD_REF(pkt);
        (void)ipcom_pkt_output_done(netif, pkt, IP_FLAG_FC_STACKCONTEXT);
    }
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sockdev_ip_output
 *===========================================================================
 * Description: Updates statistics counter and add the link header (i.e.
 *              sets the LOOPED_BACK flag).
 * Parameters:  netif - The interface the packet was sent to.
 *              rt - The route followed (ignored).
 *              pkt - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_sockdev_ip_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    IPCOM_UNUSED_ARG(rt);
    return ipnet_if_output(netif, pkt);
}


/*
 *===========================================================================
 *                    ipnet_sockdev_ip6_create_addr
 *===========================================================================
 * Description: Should never be called, but is needed by IPNET link layer
 *              infrastructure.
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipnet_sockdev_ip6_create_addr(Ipnet_netif *netif,
                             IP_CONST struct Ip_in6_addr* prefix,
                             struct Ip_in6_addr* in6_addr)
{
    in6_addr->in6.addr16[0] = prefix->in6.addr16[0];
    in6_addr->in6.addr16[1] = prefix->in6.addr16[1];
    in6_addr->in6.addr16[2] = prefix->in6.addr16[2];
    in6_addr->in6.addr16[3] = prefix->in6.addr16[3];
    in6_addr->in6.addr16[4] = 0;
    in6_addr->in6.addr16[5] = 0;
    IP_SET_32ON16(&in6_addr->in6.addr32[3],
                  ip_htonl(((Ipnet_socket *) netif->ipcom.plink)->ipcom.fd));
    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_sockdev_init
 *===========================================================================
 * Description: Create and attach application device interface.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_sockdev_init(Ipnet_socket *s, Ip_bool user_context)
{
    Ipcom_netif *sockdev_if;
    int          ret = -IP_ERRNO_ENOMEM;

    IPCOM_UNUSED_ARG(user_context);

    /* Allocate loopback interface */
    sockdev_if = ipcom_if_malloc(IP_IFT_SOCKDEV);
    if (sockdev_if == IP_NULL)
        goto errout;

    ipcom_sprintf(sockdev_if->name, "%s%d", IPNET_SOCKDEV_IF_NAME_BASE, s->proto);

    /* Attach loopback interface */
    ret = ipcom_if_attach(sockdev_if);
    if (ret < 0)
        goto errout;

    s->bind_to_ifindex = sockdev_if->ifindex;
    sockdev_if->plink = s;

    ret = ipnet_if_link_ioctl((Ipnet_netif *)sockdev_if, IP_SIOCXOPEN, IP_NULL);

 errout:
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sockdev_destroy
 *===========================================================================
 * Description: Detach and free an application device interface.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_sockdev_destroy(Ipnet_socket *sock)
{
    Ipnet_netif *netif = ipnet_if_indextonetif(sock->vr_index, sock->bind_to_ifindex);

    netif->ipcom.plink = IP_NULL;
    (void) ipnet_if_link_ioctl(netif, IP_SIOCXCLOSE, IP_NULL);
    (void)ipcom_if_detach(&netif->ipcom);
    (void)ipcom_if_free(&netif->ipcom);
}

/*
 *===========================================================================
 *                  ipnet_sockdev_send
 *===========================================================================
 * Description: Inputs a packet to the sockdev interface.
 * Parameters:  sock - The socket to use when sending.
 *              msg - Contains the data to send.
 *              flags - IP_MSG_xxx flags.
 * Returns:     >0 = application bytes sent, <0 = error code.
 *
 */
IP_STATIC int
ipnet_sockdev_send(Ipnet_socket *sock, IP_CONST struct Ip_msghdr *msg, int flags)
{
    int          ret;
    int          length;
    Ipcom_pkt   *pkt;
    Ipnet_netif *netif = ipnet_if_indextonetif(sock->vr_index, sock->bind_to_ifindex);

    length = ipnet_sock_create_pkt_from_user_iov(sock, msg, flags, &pkt);
    if (length < 0)
        return length;

    IPNET_CODE_UNLOCK();
    ipcom_pkt_input(&netif->ipcom, pkt);
    IPNET_CODE_LOCK_READ();

    return ret < 0 ? ret : length;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sock_sockdev_register
 *===========================================================================
 * Description: Registers the IP_AF_SOCKDEV domain.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_sockdev_register(void)
{
    Ipnet_sock_ops *ops;

    ops = ipcom_calloc(1, sizeof(*ops));
    if (ops == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "Could not register the IP_AF_SOCKDEV domain, out of memory");
        IP_PANIC();
        return -IP_ERRNO_ENOMEM;
    }

    ops->domain     = IP_AF_SOCKDEV;
    ops->type       = IP_SOCK_RAW;
    ops->proto      = -1;
    ops->destroy    = ipnet_sockdev_destroy;
    ops->init       = ipnet_sockdev_init;
    ops->recv       = ipnet_sock_pkt_recv;
    ops->send       = ipnet_sockdev_send;

    return ipnet_sock_register_ops(ops);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sockdev_if_init
 *===========================================================================
 * Description: Initializes a new loopback interface.
 * Parameters:  netif - The interface to initialize.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_PUBLIC int
ipnet_sockdev_if_init(Ipnet_netif *netif)
{
    /* Init loopback interface */
    netif->ipcom.mtu            = 1500;
    netif->link_ioctl           = ipnet_sockdev_ioctl;
    netif->ipcom.flags          = IP_IFF_NOARP;
    netif->link_input           = ipnet_sockdev_link_input;
    netif->ipcom.link_ioevent   = ipnet_sockdev_ioevent;
    netif->ipcom.drv_output     = ipnet_sockdev_drv_output;
#ifdef IPCOM_USE_INET
    netif->link_ip4_output      = ipnet_sockdev_ip_output;
#endif
#ifdef IPCOM_USE_INET6
    netif->link_ip6_output      = ipnet_sockdev_ip_output;
    netif->link_ip6_create_addr = ipnet_sockdev_ip6_create_addr;
    netif->inet6_if_id_bits     = 64;
#endif /* IPCOM_USE_INET6 */
    return 0;
}

#endif /* IPNET_USE_SOCKDEV */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

