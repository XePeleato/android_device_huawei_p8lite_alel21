/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_loopback.c,v $ $Revision: 1.78 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_loopback.c,v $
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
 * Loopback interface implementation.
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

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_syslog.h>

#include "ipnet_h.h"
#include "ipnet_loopback.h"
#include "ipnet_netif.h"
#include "ipnet_usr_sock.h"


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

/* The "link layer" header for the loopback device */
typedef struct Ipnet_loopback_trailer_struct
{
    /* The original scope ID for the packet (only relevant for IPv6) */
    Ip_u32 scope_id;
}
Ipnet_loopback_trailer_t;

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

IP_STATIC int ipnet_loopback_input(Ipnet_netif *netif, Ipcom_pkt *pkt);

IP_STATIC int ipnet_loopback_ip_output(Ipnet_netif *netif,
                                       struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt);

IP_STATIC int ipnet_loopback_ioevent(Ipcom_netif *ipcomif,
                                     int command, void *data, int fcflags);

#ifdef IPCOM_USE_INET6
IP_STATIC int ipnet_loopback_ip6_create_addr(Ipnet_netif *netif,
    IP_CONST struct Ip_in6_addr* prefix, struct Ip_in6_addr* in6_addr);
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

/*
 *===========================================================================
 *                    ipnet_loopback_ioctl
 *===========================================================================
 * Description: IO control. Accept join/leave from multicast groups.
 * Parameters:  netif - The network interface on which to perform the operation.
 *              command - The command to perform.
 *              data - Data associated with the command.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_loopback_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    struct Ip_ifreq *ifreq = data;
    int              ret = 0;

    switch (command)
    {
    case IP_SIOCXOPEN:
#ifdef IP_PORT_LKM
        /* Call linux loopback device */
        ipnet_if_drv_ioctl(netif, IP_SIOCXOPEN, IP_NULL);
#endif /* IP_PORT_LKM */
        ipnet_kioevent(netif, IP_EIOXRUNNING, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        ipnet_kioevent(netif, IP_EIOXUP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCXCLOSE:
#ifdef IP_PORT_LKM
        /* Call linux loopback device */
        ipnet_if_drv_ioctl(netif, IP_SIOCXCLOSE, IP_NULL);
#endif /* IP_PORT_LKM */
        ipnet_kioevent(netif, IP_EIOXDOWN, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        ipnet_kioevent(netif, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCGIFHWADDR:
        /* Switch to our if type */
        ifreq->ip_ifr_addr.sa_family = netif->ipcom.type;
        /* Return */
        break;

    case IP_SIOCGIFLLADDR:
        ifreq->ip_ifr_addr.sa_family = IP_AF_LINK;
        IPCOM_SA_LEN_SET(&ifreq->ip_ifr_addr, 0);
        break;

    case IP_SIOCIFDESTROY:
        if (ipcom_strcmp(netif->ipcom.name, IPNET_IF_LOOPBACK_NAME) == 0)
            ret = -IP_ERRNO_EPERM;
        else
        {
            ret = ipcom_if_detach(&netif->ipcom);
            if (ret >= 0)
                ret = ipcom_if_free(&netif->ipcom);
        }
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
 *                    ipnet_loopback_input
 *===========================================================================
 * Description: Handles loopback packets from the input daemon.
 * Parameters:  netif - A loopback interface.
 *              pkt - The incoming packet.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_loopback_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_loopback_trailer_t *tr;

    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));

    tr = (Ipnet_loopback_trailer_t *) &pkt->data[pkt->end];
    pkt->ifindex = IP_GET_32ON8(&tr->scope_id);

    /* Verify that the indicated netif exists */
    if (IP_UNLIKELY(ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex) == IP_NULL))
    {
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EINVAL;
    }

#ifndef IP_PORT_LKM
    IPCOM_PKT_ADD_REF(pkt);
    (void)ipcom_pkt_output_done(&netif->ipcom, pkt, IP_FLAG_FC_STACKCONTEXT);
#endif

    switch (pkt->data[pkt->start] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        return ipnet_ip4_input(netif, pkt);
#endif
#ifdef IPCOM_USE_INET6
    case 0x60:
        return ipnet_ip6_input(netif, pkt);
#endif
    default:
        IP_PANIC2();
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EINVAL;
    }
}


#ifdef IPCOM_USE_PACKET
/*
 *===========================================================================
 *                    ipnet_loopback_get_frame_info
 *===========================================================================
 * Description: Extracts frame information from a packet sent on the loopback
 *              device.
 * Parameters:  netif - Loopback interface
 *              pkt - The packet to extract information from
 *              pkt_type - Will contain the packet type if != IP_NULL
 *              frame_type - Will contain the frame type if != IP_NULL
 *              src - Unused since the loopback does not have a link layer address.
 *              dst - Unused since the loopback does not use link layer address.
 * Returns:
 *
 */
IP_STATIC int
ipnet_loopback_get_frame_info(struct Ipnet_netif_struct *netif,
                              Ipcom_pkt *pkt,
                              Ip_u8 *pkt_type,
                              Ip_u16 *frame_type,
                              Ip_u8 *src,
                              Ip_u8 *dst)
{
    IPCOM_UNUSED_ARG(src);
    IPCOM_UNUSED_ARG(dst);

    if (frame_type != IP_NULL)
    {
        Ip_u8 network_type = pkt->data[pkt->start + netif->ipcom.link_hdr_size];

        if ((network_type & 0xf0) == 0x40)
            *frame_type = ip_htons(IP_IPPROTO_IPV4);
        else if ((network_type & 0xf0) == 0x60)
            *frame_type = ip_htons(IP_IPPROTO_IPV6);
        else
            return -IP_ERRNO_EINVAL;
    }

    if (pkt_type != IP_NULL)
        *pkt_type = IP_PACKET_HOST;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_loopback_raw_output
 *===========================================================================
 * Description: Output function for AF_PACKET socket datagrams.
 * Parameters:  netif - The interface the packet was sent to.
 *              to - The recipient of this packet.
 *              pkt - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_loopback_raw_output(struct Ipnet_netif_struct *netif,
                          IP_CONST struct Ip_sockaddr_ll *to,
                          Ipcom_pkt *pkt)
{
#ifdef IPCOM_USE_INET
    if (ip_ntohs(to->sll_protocol) == IP_IPPROTO_IPV4)
    {
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPV4);
        return netif->link_ip4_output(netif, IP_NULL, pkt);
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (ip_ntohs(to->sll_protocol) == IP_IPPROTO_IPV6)
    {
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPV6);
        return netif->link_ip6_output(netif, IP_NULL, pkt);
    }
#endif /* IPCOM_USE_INET6 */

    IP_PANIC2();
    return -IP_ERRNO_EINVAL;
}

#endif /* IPCOM_USE_PACKET */


/*
 *===========================================================================
 *                    ipnet_loopback_ip_output
 *===========================================================================
 * Description: Updates statistics counter and add the link trailer.
 * Parameters:  netif - The interface the packet was sent to.
 *              rt - The route followed (ignored).
 *              pkt - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_loopback_ip_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    Ipnet_loopback_trailer_t *tr;

    IPCOM_UNUSED_ARG(rt);

    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP)))
    {
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EHOSTDOWN;
    }

#ifdef IPCOM_ZEROCOPY
    pkt = ipcom_pkt_make_linear(pkt);
    if (IP_UNLIKELY(pkt == IP_NULL))
        /* Not enough packets to make this packet linear, not much to
           about it */
        return 0;
#endif /* IPCOM_ZEROCOPY */

    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_OUTPUT);

#if defined(IPCOM_USE_INET) || defined(IPCOM_USE_INET6)
    ip_assert2((pkt->data[pkt->start] & 0xf0) == 0x40 || (pkt->data[pkt->start] & 0xf0) == 0x60);
#elif defined(IPCOM_USE_INET)
    ip_assert2((pkt->data[pkt->start] & 0xf0) == 0x40);
#else
    ip_assert2((pkt->data[pkt->start] & 0xf0) == 0x60);
#endif

    tr = (Ipnet_loopback_trailer_t *) &pkt->data[pkt->end];
    IP_SET_32ON8(&tr->scope_id, pkt->ifindex);
    pkt->ifindex = netif->ipcom.ifindex;

    return ipnet_if_output(netif, pkt);
}


/*
 *===========================================================================
 *                    ipnet_loopback_drv_output
 *===========================================================================
 * Description: Loops back the packet.
 * Parameters:  netif - The interface the packet was sent to.
 *              pkt - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_loopback_drv_output(Ipcom_netif *netif, Ipcom_pkt *pkt)
{
    int ret = 0;

    /* The ipcom_pkt_input_queue() frees the packet if it cannot be queued,
       add a reference so that packet is still allocated if that happens */
    IPCOM_PKT_ADD_REF(pkt);
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RX);
    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK);
    ret = ipcom_pkt_input_queue(netif, pkt);
    if (IP_UNLIKELY(ret != -IP_ERRNO_EWOULDBLOCK))
    {
        ip_assert(IPCOM_PKT_HAS_MULTIPLE_OWNERS(pkt));
        ipcom_pkt_free(pkt);
    }
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_loopback_ip6_create_addr
 *===========================================================================
 * Description: Should never be called, but is needed by IPNET link layer
 *              infrastructure.
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipnet_loopback_ip6_create_addr(Ipnet_netif *netif,
                               IP_CONST struct Ip_in6_addr* prefix,
                               struct Ip_in6_addr* in6_addr)
{
    (void) netif;

    in6_addr->in6.addr16[0] = prefix->in6.addr16[0];
    in6_addr->in6.addr16[1] = prefix->in6.addr16[1];
    in6_addr->in6.addr16[2] = prefix->in6.addr16[2];
    in6_addr->in6.addr16[3] = prefix->in6.addr16[3];
    in6_addr->in6.addr16[4] = 0;
    in6_addr->in6.addr16[5] = 0;
    in6_addr->in6.addr16[6] = 0;
    in6_addr->in6.addr16[7] = ip_htons(1);
    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_loopback_attach
 *===========================================================================
 * Description: Create and attach a loopback interface.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_loopback_attach(Ip_u16 vr, char *name)
{
    Ipcom_netif *loopback_if;
    int          ret;

    /* Allocate loopback interface */
    loopback_if = ipcom_if_malloc(IP_IFT_LOOP);
    if (loopback_if == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    ((Ipnet_netif *)loopback_if)->vr_index = vr;

    ipcom_strcpy(loopback_if->name, name);

    /* Attach loopback interface */
    ret = ipnet_sys_if_attach(loopback_if);
    if (ret < 0)
      return ret;

    /* Success. */
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_loopback_get_netif
 *===========================================================================
 * Description: Returns the loopback interface for a specific virtual router.
 * Parameters:  vr - The virtual router for which the interface should be
 *                   returned.
 * Returns:     The pointer to a loopback interface, can only be IP_NULL
 *              if the virtual router does not exists.
 *
 */
IP_GLOBAL Ipnet_netif *
ipnet_loopback_get_netif(Ip_u16 vr)
{
    return ipnet_if_nametonetif(vr, IPNET_IF_LOOPBACK_NAME);
}



/*
 *===========================================================================
 *                    ipnet_is_loopback
 *===========================================================================
 * Description: Returns IP_TRUE if the 'ifindex' is a loopback interface.
 * Parameters:  ifindex - A index of a network interface.
 * Returns:
 *
 */
IP_GLOBAL Ip_bool
ipnet_is_loopback(Ip_u16 vr, Ip_u32 ifindex)
{
    Ipnet_netif *netif;

    netif = ipnet_if_indextonetif(vr, ifindex);
    if (netif == IP_NULL)
        return IP_FALSE;
    return IP_BIT_ISTRUE(netif->ipcom.flags, IP_IFF_LOOPBACK);
}

#ifdef IP_PORT_LKM
IP_GLOBAL int
ipnet_loopback_if_setup(Ipnet_netif *netif)
{
    int ret = 0;
#ifdef IPCOM_USE_INET6
    struct Ip_in6_addr lo_addr = IP_IN6ADDR_LOOPBACK_INIT;
    struct Ipnet_route_add_param param;
    struct Ipnet_ipv6_key        key;
    struct Ipnet_ipv6_key        mask;

    /* Add ::1 loopback address to interface */
    (void)ipnet_ip6_add_addr(netif,
                             &lo_addr,
                             IP_TRUE,
                             0xffffffff,
                             0xffffffff,
                             128);

    /* All packets matching ff02::/16 with no scope specified
       must be rejected */
    ipcom_memset(&key, 0, sizeof(key));
    ipcom_memset(&mask, 0, sizeof(mask));
    ipcom_memset(&param, 0, sizeof(param));

    key.addr.in6.addr16[0] = ip_htons(0xff02);
    ipnet_route_create_mask(&mask.addr, 16);

    param.domain  = IP_AF_INET6;
    param.key     = &key;
    param.netmask = &mask;
    param.flags   = IPNET_RTF_X_HIDDEN | IPNET_RTF_REJECT | IPNET_RTF_DONE;
    param.netif   = netif;
    param.vr      = netif->vr_index;
    param.table   = IPCOM_ROUTE_TABLE_DEFAULT;

    ret = ipnet_route_add(&param);

    if (ret < 0)
        IPCOM_LOG2(ERR, "Failed to setup routes on %s in vr %d",
                   netif->ipcom.name,
                   netif->vr_index);

#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_USE_INET
    /* Add 127.0.0.1 loopback address to interface */
    (void)ipnet_ip4_add_addr(netif,
                             ip_htonl(IP_INADDR_LOOPBACK),
                             ip_htonl(0xff000000),
                             IP_TRUE,
                             IPNET_ADDR_TYPE_UNICAST);
#endif /* IPCOM_USE_INET */

    return ret;
}

#endif


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_loopback_if_init
 *===========================================================================
 * Description: Initializes a new loopback interface.
 * Parameters:  netif - The interface to initialize.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_PUBLIC int
ipnet_loopback_if_init(Ipnet_netif *netif)
{
    /* Init loopback interface */
#ifndef IP_PORT_LKM
    /* Set the MTU, unless we're in the LKM in which case
     * its already dealt with */
    netif->ipcom.mtu            = 1500;
#endif
    netif->link_ioctl           = ipnet_loopback_ioctl;
    netif->ipcom.flags          = IP_IFF_LOOPBACK | IP_IFF_MULTICAST;
    netif->link_input           = ipnet_loopback_input;
    netif->ipcom.link_ioevent   = ipnet_loopback_ioevent;
#ifdef IPCOM_USE_PACKET
    netif->link_raw_output      = ipnet_loopback_raw_output;
    netif->link_get_frame_info  = ipnet_loopback_get_frame_info;
#endif
#ifdef IPCOM_USE_INET
    netif->link_ip4_output      = ipnet_loopback_ip_output;
#endif
#ifdef IPCOM_USE_INET6
    netif->link_ip6_output      = ipnet_loopback_ip_output;
    netif->link_ip6_create_addr = ipnet_loopback_ip6_create_addr;
    netif->inet6_if_id_bits     = 64;
#endif /* IPCOM_USE_INET6 */
    netif->ipcom.drv_output     = ipnet_loopback_drv_output;
    return 0;
}


/*
 *===========================================================================
 *                         ipnet_loopback_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_loopback_ioevent(Ipcom_netif *ipcomif,
                       int command,
                       void *data,
                       int fcflags)
{
    IPCOM_UNUSED_ARG(ipcomif);
    IPCOM_UNUSED_ARG(data);
    IPCOM_UNUSED_ARG(fcflags);

    switch (command)
    {
    case IP_EIOXUP:
        break;
    case IP_EIOXDOWN:
        break;
    default:
        return -1;
    }

    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

