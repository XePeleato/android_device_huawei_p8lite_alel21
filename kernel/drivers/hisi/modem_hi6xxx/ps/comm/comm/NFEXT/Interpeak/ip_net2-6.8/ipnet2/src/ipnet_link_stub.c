/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_link_stub.c,v $ $Revision: 1.4 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_link_stub.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Balaji Raghavan(braghava@windriver.com)
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

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef  IPNET_USE_LINK_STUB

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_clib.h>
#include <ipcom_syslog.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_link_stub.h"
#include "ipnet_pkt_queue.h"

#ifdef IPCOM_USE_INET6
#include "ipnet_icmp6.h"
#endif



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

IP_FASTTEXT IP_STATIC int ipnet_link_stub_input(Ipnet_netif *netif, Ipcom_pkt *pkt);
IP_STATIC int ipnet_link_stub_ioevent(Ipcom_netif *ipcomif, int command, void *data, int fcflags);

#ifdef IPCOM_USE_INET
IP_FASTTEXT IP_STATIC int ipnet_link_stub_ip4_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt);
#endif

#ifdef IPCOM_USE_INET6
IP_STATIC int ipnet_link_stub_ip6_output(Ipnet_netif *netif,
    Ipnet_route_entry *re, Ipcom_pkt *pkt);

IP_STATIC int ipnet_link_stub_ip6_create_addr(Ipnet_netif *netif,
    IP_CONST struct Ip_in6_addr* prefix, struct Ip_in6_addr* in6_addr);
#endif /* IPCOM_USE_INET6 */


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
 *                         ipnet_eth_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_link_stub_ioevent(Ipcom_netif *ipcomif, int command, void *data, int fcflags)
{
    int ret = 0;

    switch (command)
    {
    case IP_EIOXUP :
    case IP_EIOXDOWN :
    case IP_EIOXRUNNING :
    case IP_EIOXSTOP :
        ipnet_kioevent((Ipnet_netif *) ipcomif, command, data, fcflags);
        break;

    default:
        ret = -1;
        break;
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_link_stub_ip4_output
 *===========================================================================
 * Description: Outputs a IPv4 packet on the custom netif.
 * Parameters:  netif - The network interface on which the packet should
 *              be sent.
 *              next_hop - Next hop IP address (should be on link).
 *              pkt - The packet to send.
 * Returns:     0 = success, <0 = error code.
 *
 */
#ifdef IPCOM_USE_INET
IP_FASTTEXT IP_STATIC int
ipnet_link_stub_ip4_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    int              ret;

    ip_assert((pkt->end - pkt->start) >= IPNET_IP_HDR_SIZE);

    /* Cloned route with or without valid llinfo needed */
    ip_assert(rt != IP_NULL);

    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP)))
    {
        ret = -IP_ERRNO_ENETDOWN;
        goto badout;
    }

    /* Pass route information to MUX for ARP updates */
    pkt->route_info = rt;

    ret = ipnet_if_output(netif, pkt);
    return ret;

 badout:
    ip_assert(ret < 0);
    ipcom_pkt_free(pkt);
    return ret;
}
#endif /* IPCOM_USE_INET */

#endif /* IPNET_USE_LINK_STUB */



#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_link_stub_ip6_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_link_stub_ip6_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    int                          ret;
    Ipnet_pkt_ip6               *ip_hdr;
    IP_CONST struct Ip_in6_addr *reply_addr;

    ip_assert((pkt->end - pkt->start) >= IPNET_IP6_HDR_SIZE);

    ip_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];
    reply_addr = &ip_hdr->src;

    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP)))
    {
        ipcom_pkt_free(pkt);
        ret = -IP_ERRNO_ENETDOWN;
        goto badout;
    }

    /* Pass route information to MUX for ARP updates */
    pkt->route_info = rt;

    ret = ipnet_if_output(netif, pkt);
    return ret;

 badout:
    return ret;

}


/*
 *===========================================================================
 *                    ipnet_link_stub_ip6_create_addr
 *===========================================================================
 * Description: Creates an unique address based on the passed prefix and the
 *              address of the ethernet card in the machine.
 * Parameters:  netif - The interface to which this address will be bound.
 *              prefix - The network identifier part of the address.
 *              in6_addr - Will hold a unique address if the function is
 *              successful.
 * Returns:     0 = succeess. <0 = error code.
 *
 */
IP_STATIC int
ipnet_link_stub_ip6_create_addr(Ipnet_netif *netif, IP_CONST struct Ip_in6_addr *prefix,
    struct Ip_in6_addr *in6_addr)
{

    if (netif->ipcom.drv_ip6_create_addr)
    {
        netif->ipcom.drv_ip6_create_addr((void *)prefix, in6_addr);
        return 0;
    }
    else
    {
        return IPCOM_ERR_FAILED;
    }


}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_link_stub_input
 *===========================================================================
 * Description: Input handler for link stub frames. pkt->start is the offset
 *              to the link layer header ? or ipheader ?.
 * Parameters:  pkt - The ethernet frame.
 * Returns:     0 = success, <0 error code
 *
 */
IP_FASTTEXT IP_STATIC int
ipnet_link_stub_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    int ip_ver;

    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP)))
    {
        IPCOM_MIB2_NETIF(ifInDiscards++);
        goto free_pkt;
    }

    /* Setup the LLA offsets */
    pkt->lla_off    = 0;
    pkt->lla_size   = 0;

    /* Fetch the IP protocol number. Assume no llayer header is sent
       by drv_mux */
    ip_ver = pkt->data[pkt->start] & 0xf0;

    /*
     * rather than assuming the receiving link header size to be the
     * value that is configured the netif
     * (e.g. netif->ipcom.link_hdr_size), determine the size based on
     * what is being received. This is necessary because it is
     * possible that untagged frame is received even though the
     * underlying port is configured to send tagged frame for a given
     * vlan (or vice versa).
     */

    IP_PREPARE_FOR_READ(&pkt->data[pkt->start]);

    /* Process frame */
    switch (ip_ver)
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
        break;
    }

 free_pkt:

    ipcom_pkt_free(pkt);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_link_stub_set_mac_addr
 *===========================================================================
 * Description: Sets the MAC address and regenerates all IPv6 addresses
 *              created by automatic address configuration.
 * Parameters:  netif - The interface on which the multicast address will
 *              be used.
 *              ifreq - Contains the new
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_STATIC int
ipnet_link_stub_set_mac_addr(Ipnet_netif *netif, struct Ip_ifreq *ifreq)
{
#ifdef IPCOM_USE_INET
    Ipnet_ip4_addr_entry *ae;
#endif
#ifdef IPCOM_USE_INET6
    Ip_u32                addr_entry_flags;
    Ipnet_ip6_addr_entry *addr_entry;
    Ipnet_ip6_addr_entry *next_addr_entry;
    struct Ip_in6_addr    new_addr;
    int                   prefixlen;
    Ip_u32                preferred;
    Ip_u32                valid;
#endif /* IPCOM_USE_INET6 */
    int retval;

    retval = ipnet_if_drv_ioctl(netif, IP_SIOCSIFLLADDR, ifreq);
    if (retval < 0)
        return retval;


    ip_assert(IP_LINKNAMSIZ <= IPNET_LINK_STUB_ADDR_SIZE);
    ip_assert(IPCOM_SA_LEN_GET(&ifreq->ip_ifr_addr) == IPNET_LINK_STUB_ADDR_SIZE);

    ipcom_memcpy(netif->ipcom.link_addr,ifreq->ip_ifr_addr.sa_data, IPNET_LINK_STUB_ADDR_SIZE);

#ifdef IPCOM_USE_INET
    for (ae = netif->inet4_addr_list;
         ae != IP_NULL;
         ae = ae->next)
    {
        if (ae->type == IPNET_ADDR_TYPE_UNICAST)
        {
            /* Call ARP hook if necessary */
        }

    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    for (addr_entry = netif->inet6_addr_list;
         addr_entry != IP_NULL;
         addr_entry = addr_entry->next)
        IP_BIT_CLR(addr_entry->flags, IPNET_IP6_ADDR_FLAG_UPDATE_DONE);

    addr_entry = netif->inet6_addr_list;
    while (addr_entry != IP_NULL)
    {
        if (IP_BIT_ISSET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_UPDATE_DONE)
            || IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_AUTOMATIC))
        {
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_UPDATE_DONE);
            addr_entry = addr_entry->next;
        }
        else
        {
            Ip_u32 sec_now = ipnet_sec_now();

            /* Regenerate this address */
            prefixlen = addr_entry->prefixlen;
            if (addr_entry->preferred_sec == IPCOM_ADDR_INFINITE)
                preferred = IPCOM_ADDR_INFINITE;
            else
                preferred = addr_entry->preferred_sec - sec_now;
            if (addr_entry->expires_sec == IPCOM_ADDR_INFINITE)
                valid = IPCOM_ADDR_INFINITE;
            else
                valid = addr_entry->expires_sec - sec_now;

            ipcom_memcpy(&new_addr, &addr_entry->addr, sizeof(new_addr));

            addr_entry_flags = addr_entry->flags;
            next_addr_entry = addr_entry->next;
            retval = ipnet_ip6_remove_addr(netif, &addr_entry->addr);
            if (retval < 0)
                return retval;
            addr_entry = next_addr_entry;

            if (IP_BIT_ISFALSE(addr_entry_flags, IPNET_IP6_ADDR_FLAG_DEPRECATED))
            {
                /* Customer should implement link_ip6_create_addr */
                retval = netif->link_ip6_create_addr(netif, &new_addr, &new_addr);
                if (retval < 0)
                    return retval;

                retval = ipnet_ip6_add_addr(netif, &new_addr,
                                            IPNET_IP6_ADDR_FLAG_AUTOMATIC
                                            | IPNET_IP6_ADDR_FLAG_UPDATE_DONE
                                            | IPNET_IP6_ADDR_FLAG_TENTATIVE,
                                            preferred, valid, prefixlen);
                if (retval < 0)
                    return retval;
                /* Start over since adding the new address might have
                   altered the order of the list */
                addr_entry = netif->inet6_addr_list;
            }
            /* else: no reason to add the address since it is deprecated anyway */
        }
    }
#endif /* IPCOM_USE_INET6 */
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_link_stub_ioctl
 *===========================================================================
 * Description: link stub IO control. Support joining and leaving IPv4 and
 *              IPv6 multicast groups.
 * Parameters:  netif - The network interface to operate on.
 *              command - IPNET_IP4_SIOCxxx or IPNET_IP6_SIOCxxx style command.
 *              data - Depends on command.
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_STATIC int
ipnet_link_stub_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    int retval = 0;
    Ip_u8 link_addr[IPNET_LINK_STUB_ADDR_SIZE];
    struct Ip_ifreq *ifreq = data;

    ip_assert(netif->ipcom.link_addr_size == IPNET_LINK_STUB_ADDR_SIZE);

    switch (command)
    {
    case IP_SIOCXOPEN:
        retval = ipnet_if_drv_ioctl(netif, IP_SIOCXOPEN, IP_NULL);
        if (retval == 0)
            ipnet_kioevent(netif, IP_EIOXUP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCXCLOSE:
        retval = ipnet_if_drv_ioctl(netif, IP_SIOCXCLOSE, IP_NULL);
        if (retval == 0)
            ipnet_kioevent(netif, IP_EIOXDOWN, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCSIFLLADDR:
        retval = ipnet_link_stub_set_mac_addr(netif, ifreq);
        break;
    case IP_SIOCGIFHWADDR:
    case IP_SIOCGIFLLADDR:

        /* 14 bytes max size that can be passed via sockaddr structs */
        ip_assert(IPNET_LINK_STUB_ADDR_SIZE <= 14);

        ifreq->ip_ifr_addr.sa_family = IP_AF_LINK;
        IPCOM_SA_LEN_SET(&ifreq->ip_ifr_addr, IPNET_LINK_STUB_ADDR_SIZE);
        ipcom_memcpy(ifreq->ip_ifr_addr.sa_data,netif->ipcom.link_addr, IPNET_LINK_STUB_ADDR_SIZE);

        break;
    case IP_SIOCADDMULTI:
    case IP_SIOCDELMULTI:
        {
            /* See above */
            struct Ip_sockaddr *sa = (struct Ip_sockaddr *) data;
            ipcom_memcpy(link_addr, sa->sa_data, sizeof(link_addr));
            if (command == IP_SIOCADDMULTI)
                retval = ipnet_if_drv_ioctl(netif, IP_SIOCXADDMULTI, link_addr);
            else
                retval = ipnet_if_drv_ioctl(netif, IP_SIOCXDELMULTI, link_addr);
        }
        break;
    case IP_SIOCXPROMISC:
        {
            /* Enable/disable promiscuous mode */
            Ip_bool promisc = ifreq->ifr_ifru.ifru_opt;
            retval = ipnet_if_drv_ioctl(netif, IP_SIOCXPROMISC, &promisc);
        }
        break;
#ifdef IPCOM_USE_INET
    case IP_SIOCXADDMULTI_IN:
    case IP_SIOCXDELMULTI_IN:
    /* Drop down let link layer implementation handle this */
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_SIOCXADDMULTI_IN6:
    case IP_SIOCXDELMULTI_IN6:
    /* Drop down let link layer implementation handle this */

#endif /* IPCOM_USE_INET6 */

    default:
        /* Pass it down to interface */
        retval = ipnet_if_drv_ioctl(netif, command, data);
        break;
    }

    return retval;
}


#ifdef IPCOM_USE_PACKET
/*
 *===========================================================================
 *                      ipnet_link_stub_raw_output
 *===========================================================================
 * Description: Adds a link layer header and outputs the frame to the driver
 *              ARP (or similar) is bypassed.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              to - The link layer address of the recipient of this frame.
 *              pkt - The packet which should have a link layer header appended.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_link_stub_raw_output(Ipnet_netif *netif,
                     IP_CONST struct Ip_sockaddr_ll *to,
                     Ipcom_pkt *pkt)
{

    if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
    {
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_ENETDOWN;
    }

    ip_assert(pkt->start < pkt->end);

    return ipnet_if_output(netif, pkt);
}


/*
 *===========================================================================
 *                      ipnet_link_stub_get_frame_info
 *===========================================================================
 * Description: Gets header information from the frame header.
 * Parameters:  [in] netif - A pointer to the network interface structure for this
 *              driver.
 *              [in] pkt - A compleate link layer fram. pkt->start is the offset
 *              to the link layer header.
 *              [out] pkt_type - Will be set to one of the IP_PACKET_xxx type,
 *              can be IP_NULL if this value is not needed.
 *              [out] frame_type - Will be set to the frame type (i.e. one of the
 *              IPNET_ETH_P_xxx constants). can be IP_NULL if this value is not needed.
 *              [out] src - Will be set to the source address of the frame, can
 *              be IP_NULL.
 *              [out] dst - Will be set to the destination address of the frame,
 *              can be IP_NULL.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_link_stub_get_frame_info(Ipnet_netif *netif,
                         Ipcom_pkt *pkt,
                         Ip_u8 *pkt_type,
                         Ip_u16 *frame_type,
                         Ip_u8 *src,
                         Ip_u8 *dst)
{

    ip_assert(pkt);

    if (pkt_type != IP_NULL)
    {
        *pkt_type = IP_PACKET_OTHERHOST;
    }

    if (frame_type != IP_NULL)
    {
        *frame_type = 0;
    }

    return 0;
}


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_link_stub_if_init
 *===========================================================================
 * Description: Initializes the Link layer specific part of an interface.
 * Parameters:  netif - The generic interface structure.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_PUBLIC int
ipnet_link_stub_if_init(Ipnet_netif *netif)
{
    int i;

    i = 0;
    do
    {
        ipcom_sprintf(netif->ipcom.name, "lnst%d", i++);
    } while (ipnet_if_nametonetif(netif->vr_index, netif->ipcom.name) != IP_NULL);

    netif->ipcom.flags          = IP_IFF_BROADCAST | IP_IFF_MULTICAST;
    /* Dummy definitions */
    netif->ipcom.mtu            = IPNET_LINK_STUB_MTU;
    netif->ipcom.link_hdr_size  = IPNET_LINK_STUB_HDR_SIZE;
    netif->ipcom.link_addr_size = IPNET_LINK_STUB_ADDR_SIZE;


    /* Setup link functions for muxdrv use. */
    netif->link_input           = ipnet_link_stub_input;
    netif->link_ioctl           = ipnet_link_stub_ioctl;
    netif->ipcom.link_ioevent   = ipnet_link_stub_ioevent;

#ifdef IPCOM_USE_INET
    netif->link_ip4_output      = ipnet_link_stub_ip4_output;
#endif

#ifdef IPCOM_USE_INET6
    netif->link_ip6_output      = ipnet_link_stub_ip6_output;
    netif->link_ip6_create_addr = ipnet_link_stub_ip6_create_addr;
    netif->inet6_if_id_bits     = 64; /* Change this based on link
                                       * layer type */
#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_USE_PACKET
    netif->link_raw_output      = ipnet_link_stub_raw_output;
    netif->link_get_frame_info  = ipnet_link_stub_get_frame_info;
#endif /* IPCOM_USE_PACKET */

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_link_stub_add_hdr
 *===========================================================================
 * Description:
 *
 * Parameters:
 *
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_GLOBAL int
ipnet_link_stub_add_hdr(Ipnet_netif *netif, Ipcom_pkt *pkt,
                       struct Ip_sockaddr *addr, Ip_u16 type)
{

    if (pkt->start < 0)
    {
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EFAULT;
    }

    return 0;
}


#endif /* IPCOM_USE_ETHERNET */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
