/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_vlan.c,v $ $Revision: 1.56.22.1 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_vlan.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Processing of 802.1Q virtual LAN tagged frames.
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

#ifdef IPNET_USE_VLAN

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_hash.h>
#include <ipcom_type.h>
#include <ipcom_syslog.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_eth.h"

#ifdef IPNET_USE_NETLINKSOCK
#include "ipnet_netlink_h.h"
#include "ipnet_rtnetlink_h.h"
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_VLAN_TAG_SIZE  4u  /* The length of the vlan tag */

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
 *                    ipnet_vlan_obj_hash
 *===========================================================================
 * Description: Computes a hash value for the virtual LAN interface.
 * Parameters:  netif - A vlan interface.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_vlan_obj_hash(Ipnet_netif *netif)
{
    return ipcom_hash_update(&netif->eth->vlan_param.id, sizeof(netif->eth->vlan_param.id), 0);
}


/*
 *===========================================================================
 *                    ipnet_vlan_key_hash
 *===========================================================================
 * Description: Computes a hash value for the specified virtual LAN ID.
 * Parameters:  vid - A virtual interface ID.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_vlan_key_hash(int *vid)
{
    return ipcom_hash_update(vid, sizeof(*vid), 0);
}


/*
 *===========================================================================
 *                    ipnet_vlan_hash_cmp
 *===========================================================================
 * Description: Compares if the vlan interface and the virtual ID matches.
 * Parameters:  netif - An vlan interface
 *              vid - A pointer to a virtual interface ID.
 * Returns:     IP_TRUE if the interface matches the virtual interface.
 *
 */
IP_STATIC Ip_bool
ipnet_vlan_hash_cmp(Ipnet_netif *netif, int *vid)
{
    return netif->eth->vlan_param.id == *vid;
}


/*
 *===========================================================================
 *                    ipnet_vlan_bind
 *===========================================================================
 * Description: Binds a virtual interface with the specified parent
 *              Ethernet interface.
 * Parameters:  parent - An Ethernet network interface.
 *              vlan - A virtual LAN interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_vlan_bind(Ipnet_netif *parent, Ipnet_netif *vlan)
{
    if (parent->eth->vlan.ifs == IP_NULL)
    {
        parent->eth->vlan.ifs = ipcom_hash_new(
            (Ipcom_hash_obj_func) ipnet_vlan_obj_hash,
            (Ipcom_hash_key_func) ipnet_vlan_key_hash,
            (Ipcom_hash_cmp_func) ipnet_vlan_hash_cmp);
        if (parent->eth->vlan.ifs == IP_NULL)
            return -IP_ERRNO_ENOMEM;
    }

    if (ipcom_hash_get(parent->eth->vlan.ifs, &vlan->eth->vlan_param.id) != IP_NULL)
        /* This VLAN ID is already in use */
        return -IP_ERRNO_EBUSY;

    if (ipcom_hash_add(parent->eth->vlan.ifs, vlan) != IPCOM_SUCCESS)
        return -IP_ERRNO_ENOMEM;

    IPNET_IF_LOCK(parent);

    if (IP_BIT_ISSET(parent->ipcom.flags, IP_IFF_SIMPLEX))
        IP_BIT_SET(vlan->ipcom.flags, IP_IFF_SIMPLEX);
    else
        IP_BIT_CLR(vlan->ipcom.flags, IP_IFF_SIMPLEX);

    ipcom_memcpy(vlan->ipcom.link_addr,
                 parent->ipcom.link_addr,
                 vlan->ipcom.link_addr_size);
    vlan->eth->vlan.parent = parent;

    ipnet_kioevent(vlan, IP_EIOXRUNNING, IP_NULL, IP_FLAG_FC_STACKCONTEXT);

#ifdef IPCOM_USE_HW_VLAN_TAGGING
    /* copy the parent's driver capabilities */
    vlan->ipcom.drv_capabilities = parent->ipcom.drv_capabilities;

#ifndef IPCOM_VXWORKS_USE_MUX_L2
    /*
     * If MUX-L2 is not enabled for 802.1Q vlan management, the vlan
     * pseudo-interface will always send vlan-tagged frame. Hence, the
     * link_hdr_size must be adjusted. However, if the hardware supports
     * vlan tagging, the stack will send down untagged frame to the driver
     * and the driver will insert the vlan tag to the frame being transmitted.
     * In that case, link_hdr_size does not have to be adjusted.
     */
    if (IP_BIT_ISFALSE(vlan->ipcom.drv_capabilities,
                       IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_TX))
        vlan->ipcom.link_hdr_size  += IPNET_VLAN_TAG_SIZE;
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */
#endif /* IPCOM_USE_HW_VLAN_TAGGING */

    /*
     * Adjust the mtu size if the driver is not capable of handling slightly
     * larger than normal frames (e.g. frames with vlan tag)
     */
    if (IP_BIT_ISFALSE(vlan->ipcom.drv_capabilities, IPCOM_IF_DRV_CAP_VLAN_MTU))
        vlan->ipcom.mtu -=IPNET_VLAN_TAG_SIZE;

#ifdef IPCOM_VXWORKS_USE_MUX_L2
    {
        struct Ip_ethreq  ethreq;

        ipcom_memset(&ethreq, 0, sizeof(struct Ip_ethreq));
        ipcom_strcpy(ethreq.ethr_name, parent->ipcom.name);

        /*
         * 802.1Q requires that a port may transmit vlan-tagged frame only if
         * the port is a member of the vlan. Determine the membership
         * information for the underlying parent interface that this vlan
         * pseudo-interface is attached to. When a port is attached to layer
         * 2, an IP_EIOXVLAN link layer notification is sent up to the stack
         * which in turn adjust the netif->ipcom.link_hdr_size for the vlan
         * pseudo-interface.
         */
        ethreq.ethru.vlan.vid = vlan->eth->vlan_param.id;
        if (parent->link_ioctl(parent, IP_SIOCXETHGVLAN, &ethreq) == 0 &&
            ethreq.ethru.vlan.is_member == IP_TRUE)
        {
            /*
             * the underlying parent interface is already a member of the
             * vlan. That means the port is attached and joined to the vlan
             * before this vlan pseudo-interface is created. Since the network
             * stack ignores IP_EIOXVLAN link layer notification for vlan that
             * is not configured at the time the notification is received, it
             * is necessary to adjust the netif->ipcom.link_hdr_size if
             * vlan-tagged frame should be sent for this vlan.
             */
            if (ethreq.ethru.vlan.tagged == IP_TRUE)
            {
                vlan->eth->vlan_param.tag_required = IP_TRUE;

#ifdef IPCOM_USE_HW_VLAN_TAGGING
                /*
                 * Avoid the link_hdr_size adjustment if hardware supports
                 * vlan tag insertion on transmit. Untagged frame will be
                 */
                if (IP_BIT_ISSET(vlan->ipcom.drv_capabilities,
                                 IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_TX))
                    return 0;
#endif /* IPCOM_USE_HW_VLAN_TAGGING */

                vlan->ipcom.link_hdr_size += IPNET_VLAN_TAG_SIZE;
            }
            return 0;
        }

        /*
         * the underlying parent interface is not already a member of the vlan.
         * Attach and join the port to the vlan and enable tagging for the
         * vlan. The IP_SIOCXETHSVLAN ioctl command will cause layer 2 to
         * trigger a IP_EIOXVLAN link layer notification back up to the
         * network stack.
         */
        if (parent->link_ioctl(parent, IP_SIOCXETHSPORT, IP_NULL) != 0)
            return -IP_ERRNO_EINVAL;

        /*
         * join the parent port to the given vlan - packets that transmit
         * through this vlan pseudo-interface should always be tagged.
         */
        ethreq.ethru.vlan.is_member = IP_TRUE;
        ethreq.ethru.vlan.tagged = IP_TRUE;
        if (parent->link_ioctl(parent, IP_SIOCXETHSVLAN, &ethreq) != 0)
            return -IP_ERRNO_EINVAL;
    }
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_vlan_unbind
 *===========================================================================
 * Description: Unbinds a virtual interface from the specified parent
 *              Ethernet interface.
 * Parameters:  parent - An Ethernet network interface.
 *              vlan - A virtual LAN interface.
 * Returns:
 *
 */
IP_STATIC void
ipnet_vlan_unbind(Ipnet_netif *parent, Ipnet_netif *vlan)
{
    ipnet_kioevent(vlan, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);

    while (vlan->eth->vlan_param.promisc_count)
    {
        Ip_bool enter = IP_FALSE;
        (void)vlan->link_ioctl(vlan, IP_SIOCXPROMISC, &enter);
    }

    if (ipcom_memcmp(vlan->ipcom.link_addr, parent->ipcom.link_addr, vlan->ipcom.link_addr_size) != 0)
    {
        /* We're switching back to our parents' */
        Ip_bool promisc = IP_FALSE;
        ipnet_if_drv_ioctl(parent, IP_SIOCXPROMISC, &promisc);
    }

    (void) ipcom_hash_remove(parent->eth->vlan.ifs, vlan);
    if (parent->eth->vlan.ifs->elem == 0)
    {
        ipcom_hash_delete(parent->eth->vlan.ifs);
        parent->eth->vlan.ifs = IP_NULL;
    }

    /*
     * Explicitly bring down the vlan pseudo-interface. For some reasons,
     * ipcom_if_detach() will not allow the vlan pseudo-interface to be
     * detached if it is still up.
     */
    ipnet_kioevent(vlan, IP_EIOXDOWN, IP_NULL, IP_FLAG_FC_STACKCONTEXT);

#ifdef IPCOM_VXWORKS_USE_MUX_L2
    {
        struct Ip_ethreq  ethreq;

        ipcom_memset(&ethreq, 0, sizeof(struct Ip_ethreq));
        ipcom_strcpy(ethreq.ethr_name, parent->ipcom.name);

        /* remove the underlying parent interface from the VLAN */
        ethreq.ethru.vlan.vid = vlan->eth->vlan_param.id;
        ethreq.ethru.vlan.is_member = IP_FALSE;
        (void)parent->link_ioctl(parent, IP_SIOCXETHSVLAN, &ethreq);
    }
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

    /* Restore original values; regardless of what they were earlier */
#ifdef IP_PORT_VXWORKS
    vlan->ipcom.mtu            = IPNET_ETH_MTU;
    vlan->ipcom.link_hdr_size  = IPNET_ETH_HDR_SIZE;
#endif

    IPNET_IF_UNLOCK(parent);
}


/*
 *===========================================================================
 *                    ipnet_vlan_link_ioctl
 *===========================================================================
 * Description: Ethernet IO control. Support joining and leaving IPv4 and
 *              IPv6 multicast groups.
 * Parameters:  netif - The network interface to operate on.
 *              command - IPNET_IP4_SIOCxxx or IPNET_IP6_SIOCxxx style command.
 *              data - Depends on command.
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_STATIC int
ipnet_vlan_link_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    struct Ip_vlanreq *vlanreq;
    struct Ip_ifreq   *ifreq;
    int                ret = 0;
    Ipnet_netif       *parent;
    Ipnet_netif       *new_parent;

    ip_assert(netif->ipcom.type == IP_IFT_L2VLAN);

    parent = netif->eth->vlan.parent;

    switch (command)
    {
#ifdef IPCOM_USE_INET
    case IP_SIOCXADDMULTI_IN:
    case IP_SIOCXDELMULTI_IN:
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IP_SIOCXADDMULTI_IN6:
    case IP_SIOCXDELMULTI_IN6:
#endif /* IPCOM_USE_INET6 */
    case IP_SIOCXCLOSE:
    case IP_SIOCXOPEN:
        if (parent == IP_NULL)
            return -IP_ERRNO_ENXIO;
        return parent->link_ioctl(netif, command, data);
    default:
        break;
    }

#ifdef IP_PORT_LKM
    if (data == IP_NULL)
        return ipnet_if_drv_ioctl(netif, command, data);
#endif

    ifreq = (struct Ip_ifreq *) data;
    vlanreq = (struct Ip_vlanreq *) (ifreq->ip_ifr_data);
    if (vlanreq == IP_NULL
        && (command == IP_SIOCSETVLAN || command == IP_SIOCGETVLAN ||
            command == IP_SIOCSETVLANPRI))
        return -IP_ERRNO_EINVAL;

    switch (command)
    {
    case IP_SIOCIFDESTROY:
#ifdef IP_PORT_LKM
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
            return -IP_ERRNO_EBUSY;
#endif
        if (parent)
            ipnet_vlan_unbind(parent, netif);

#ifndef IP_PORT_LKM
        ret = ipcom_if_detach(&netif->ipcom);
        if (ret >= 0)
            ret = ipcom_if_free(&netif->ipcom);
        else
            if (parent)
                /* Undo the unbind operation */
                ipnet_vlan_bind(parent, netif);
#endif
        break;

    case IP_SIOCSETVLAN:
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
            return -IP_ERRNO_EPERM;
        new_parent = ipnet_if_nametonetif(netif->vr_index, vlanreq->vlr_parent);
        if (new_parent == IP_NULL)
            return -IP_ERRNO_ENXIO;
        if (new_parent->ipcom.type != IP_IFT_ETHER)
            return -IP_ERRNO_EMEDIUMTYPE;

        if (parent)
            ipnet_vlan_unbind(parent, netif);

        netif->eth->vlan_param.id = vlanreq->vlr_tag;
        ret = ipnet_vlan_bind(new_parent, netif);
        if (ret < 0)
        {
            /* Failed to bind to the physical Ethernet interface */
            netif->eth->vlan.parent = IP_NULL;
            netif->eth->vlan_param.id = 0;
            ipcom_memset(netif->ipcom.link_addr,
                         0,
                         netif->ipcom.link_addr_size);
            return ret;
        }

#ifndef IPCOM_VXWORKS_USE_MUX_L2
        /*
         * If MUX-L2 is not enabled for 802.1Q vlan management, the
         * vlan pseudo-interface will always send vlan-tagged frame
         */
        netif->eth->vlan_param.tag_required = IP_TRUE; /* always */
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

        IPCOM_LOG3(DEBUG, "%s now has vlan ID %d and parent %s",
                   netif->ipcom.name, vlanreq->vlr_tag, new_parent->ipcom.name);
        break;

    case IP_SIOCGETVLAN:
        ipcom_memset(vlanreq->vlr_parent, 0, IP_IFNAMSIZ);
        if (parent)
            ipcom_strncpy(vlanreq->vlr_parent, parent->ipcom.name, IP_IFNAMSIZ);
        vlanreq->vlr_tag = (Ip_u16) netif->eth->vlan_param.id;
        vlanreq->vlr_pri = (Ip_u16) netif->eth->vlan_param.pri;
        break;

    case IP_SIOCSETVLANPRI:
        netif->eth->vlan_param.pri = vlanreq->vlr_pri;
        break;

    case IP_SIOCGIFHWADDR:
        if (parent == IP_NULL)
            return -IP_ERRNO_ENXIO;

        ifreq->ip_ifr_addr.sa_family = IPCOM_ARPHRD_TYPE(&netif->ipcom);
        IPNET_SET_ETHADDR(ifreq->ip_ifr_addr.sa_data, netif->ipcom.link_addr);
#if 0
        ret = parent->link_ioctl(parent, command, data);
#endif
        return ret;

    case IP_SIOCSIFLLADDR:
        {
            if (parent == IP_NULL)
                return -IP_ERRNO_ENXIO;
            else
            {
                void *ethernet_addr = ((struct Ip_ifreq *)data)->ip_ifr_addr.sa_data;
                if (ipcom_memcmp(netif->ipcom.link_addr, ethernet_addr, netif->ipcom.link_addr_size) == 0)
                    break;
                else if (ipcom_memcmp(netif->ipcom.link_addr, parent->ipcom.link_addr, netif->ipcom.link_addr_size) == 0)
                {
                    /* We're using our parents' right now, must go promisc */
                    Ip_bool promisc = IP_TRUE;

                    ipcom_memcpy(netif->ipcom.link_addr, ethernet_addr, netif->ipcom.link_addr_size);
                    return ipnet_if_drv_ioctl(parent, IP_SIOCXPROMISC, &promisc);
                }
                else if (ipcom_memcmp(parent->ipcom.link_addr, ethernet_addr, netif->ipcom.link_addr_size) == 0)
                {
                    /* We're switching back to our parents' */
                    Ip_bool promisc = IP_FALSE;

                    ipcom_memcpy(netif->ipcom.link_addr, ethernet_addr, netif->ipcom.link_addr_size);
                    return ipnet_if_drv_ioctl(parent, IP_SIOCXPROMISC, &promisc);
                }
                else
                {
                    ipcom_memcpy(netif->ipcom.link_addr, ethernet_addr, netif->ipcom.link_addr_size);
                    return 0;
                }
            }
        }

    case IP_SIOCGIFLLADDR:
        if (parent == IP_NULL)
            return -IP_ERRNO_ENXIO;

        ifreq->ip_ifr_addr.sa_family = IP_AF_LINK;
        IPCOM_SA_LEN_SET(&ifreq->ip_ifr_addr, IPNET_ETH_ADDR_SIZE);
        IPNET_SET_ETHADDR(ifreq->ip_ifr_addr.sa_data, netif->ipcom.link_addr);
#if 0
        return parent->link_ioctl(parent, command, data);
#endif
        break;

    default:
        ret = ipnet_if_drv_ioctl(netif, command, data);
        break;
    }

    return ret;

}


/*
 *===========================================================================
 *                    ipnet_vlan_drv_ioctl
 *===========================================================================
 * Description: Driver level ioctl for virtual LAN interface.
 * Parameters:	netif - A virtual LAN interface.
 *              command - The ioctl command.
 *              data - Command specific data.
 * Returns:	    0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_vlan_drv_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    Ipcom_netif     *parent = (Ipcom_netif *) netif->eth->vlan.parent;
    struct Ip_ifreq *ifreq = data;

    switch (command)
    {
    case IP_SIOCXOPEN:
    case IP_SIOCXCLOSE:
#ifdef IP_PORT_LKM
        return parent->drv_ioctl(&netif->ipcom, command, data);
#else
        break;
#endif
    case IP_SIOCXPROMISC:
        if (parent == IP_NULL)
            return -IP_ERRNO_ENXIO;
        if (*(Ip_bool *)data)
        {
            netif->eth->vlan_param.promisc_count++;
            IP_BIT_SET(netif->ipcom.flags, IP_IFF_PROMISC);
        }
        else
        {
            if (netif->eth->vlan_param.promisc_count == 0)
                return -IP_ERRNO_EPERM;
            if (--netif->eth->vlan_param.promisc_count == 0)
                IP_BIT_CLR(netif->ipcom.flags, IP_IFF_PROMISC);
        }
        return parent->drv_ioctl(parent, command, data);

    case IP_SIOCSIFMTU:
        if (parent == IP_NULL)
            /*
             * This is just so we are backwards compatible with the
             * old behaviour. Accept any value since there is no
             * parent interface configured yet.
             */
            break;
        if (parent->mtu < ifreq->ifr_ifru.ifru_mtu)
            /*
             * The parent interface cannot handle this MTU size
             */
            return -IP_ERRNO_EINVAL;
        break;

    default:
        if (parent == IP_NULL)
            return -IP_ERRNO_ENXIO;
        return parent->drv_ioctl(parent, command, data);
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_vlan_drv_output
 *===========================================================================
 * Description: Output function for virtual LAN interface.
 * Parameters:	netif - The interface to use to output the frame.
 *              pkt - The frame to send.
 * Returns:	    0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_vlan_drv_output(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipcom_netif *parent = (Ipcom_netif *) netif->eth->vlan.parent;
    if(netif->ipcom.link_tap)
    {
        Ip_u16          frame_type;
        Ipnet_pkt_eth   *eth;

        /* Resolve the protocol type; depending on hardware VLAN support
         * and whatever, we may need to resolve it */
        eth = (Ipnet_pkt_eth *)&pkt->data[pkt->start];
        if ((frame_type = eth->d.type) ==  IPNET_ETHFRAME_8021Q_VLAN)
            frame_type = eth->d.vlan.type;

        /* Let the tap handler copy the packet */
        netif->ipcom.link_tap(&netif->ipcom, pkt, IP_PACKET_OUTGOING, frame_type, pkt->start, pkt->ipstart);
    }

    return parent->drv_output(parent, pkt);
}


#ifdef IPNET_USE_NETLINKSOCK

IP_STATIC int
ipnet_vlan_netlink_validate(struct Ip_nlattr *tb[],
                            struct Ip_nlattr *data[])
{
    (void)tb;

    if (!data)
        return -IP_ERRNO_EINVAL;

    return 0;
}

IP_STATIC int
ipnet_vlan_netlink_changelink(Ipnet_netif  *netif,
                              struct Ip_nlattr *tb[],
                              struct Ip_nlattr *data[])
{
    Ipnet_netif *parent     = netif->eth->vlan.parent;
    Ipnet_netif *new_parent = IP_NULL;
    Ip_u16      vid;
    int         ret;

    if (tb[IP_IFLA_LINK - 1] == IP_NULL)
        new_parent = parent;
    else
        new_parent = ipnet_if_indextonetif(netif->vr_index, IP_GET_32ON8(IP_RTA_DATA(tb[IP_IFLA_LINK - 1])));

    if (new_parent  == IP_NULL)
        return -IP_ERRNO_ENXIO;

    if (new_parent->ipcom.type != IP_IFT_ETHER)
        return -IP_ERRNO_EMEDIUMTYPE;

    if (parent)
        ipnet_vlan_unbind(parent, netif);

    if (data[IP_IFLA_VLAN_ID - 1] == IP_NULL)
        vid = netif->eth->vlan_param.id;
    else
        ipcom_memcpy(&vid, IP_RTA_DATA(data[IP_IFLA_VLAN_ID - 1]), sizeof(vid));

    netif->eth->vlan_param.id = (int) vid;
    ret = ipnet_vlan_bind(new_parent, netif);
    if (ret < 0)
    {
        /* Failed to bind to the physical Ethernet interface */
        netif->eth->vlan.parent = IP_NULL;
        netif->eth->vlan_param.id = 0;
        ipcom_memset(netif->ipcom.link_addr,
                     0,
                     netif->ipcom.link_addr_size);
        return ret;
    }

#ifndef IPCOM_VXWORKS_USE_MUX_L2
    /*
     * If MUX-L2 is not enabled for 802.1Q vlan management, the
     * vlan pseudo-interface will always send vlan-tagged frame
     */
    netif->eth->vlan_param.tag_required = IP_TRUE; /* always */
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

    IPCOM_LOG3(DEBUG, "%s now has vlan ID %d and parent %s",
               netif->ipcom.name, vid, new_parent->ipcom.name);

    return 0;
}

IP_STATIC int
ipnet_vlan_netlink_newlink(Ipnet_netif  *netif,
                           struct Ip_nlattr *tb[],
                           struct Ip_nlattr *data[])
{
    int ret;

    if (tb[IP_IFLA_LINK - 1] == IP_NULL)
        return -IP_ERRNO_EINVAL;

    if (data[IP_IFLA_VLAN_ID - 1] == IP_NULL)
        return -IP_ERRNO_EINVAL;

    ret = ipcom_if_attach(&netif->ipcom);
    if (ret < 0)
        return ret;

    /* OK; Time to resolve the vlan ID & parent interface */
    ret = ipnet_vlan_netlink_changelink(netif, tb, data);

    /* Verify the changelink */
    if (ret < 0)
        (void)ipcom_if_detach(&netif->ipcom);

    return ret;
}

IP_STATIC void
ipnet_vlan_netlink_dellink(Ipnet_netif *netif)
{
    Ipnet_netif *parent     = netif->eth->vlan.parent;

    if (parent)
        ipnet_vlan_unbind(parent, netif);

    if (ipcom_if_detach(&netif->ipcom) >= 0)
        ipcom_if_free(&netif->ipcom);
}

IP_STATIC Ip_size_t
ipnet_vlan_netlink_get_size(Ipnet_netif *netif)
{
    Ip_size_t   vsize = IP_RTA_LENGTH(2);
    (void)netif;
    return vsize;
}


IP_STATIC int
ipnet_vlan_netlink_fill_info(Ipnet_netlink_mem_t    *mem,
                             Ipnet_netif            *netif)
{
    Ip_u16 vlan_id =  netif->eth->vlan_param.id;
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_VLAN_ID, sizeof(vlan_id), &vlan_id))
        return -1;

    return 0;
}

IP_STATIC int
ipnet_vlan_netlink_fill_link(Ipnet_netlink_mem_t    *mem,
                             Ipnet_netif            *netif)
{
    Ipcom_netif       *parent = (Ipcom_netif *)netif->eth->vlan.parent;

    if (parent != IP_NULL)
    {
        Ip_u32 ifindex =  parent->ifindex;
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_LINK, sizeof(ifindex), &ifindex))
            return -1;
    }

    return 0;
}


struct Ip_rtnl_link_ops vlan_ops =
{
    IP_NULL,
    "vlan",
    IP_IFLA_VLAN_MAX,
    IP_IFT_L2VLAN,
    IP_NULL,
    ipnet_vlan_netlink_validate,
    ipnet_vlan_netlink_newlink,
    ipnet_vlan_netlink_changelink,
    ipnet_vlan_netlink_dellink,
    ipnet_vlan_netlink_get_size,
    ipnet_vlan_netlink_fill_info,
    IP_NULL,
    IP_NULL,
    ipnet_vlan_netlink_fill_link
};
#endif


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

#ifdef IPNET_USE_NETLINKSOCK
IP_GLOBAL void
ipnet_vlan_rtnetlink_init(void)
{
    ipnet_rtnetlink_register_ops(&vlan_ops);
}
#endif

/*
 *===========================================================================
 *                    ipnet_vlan_create
 *===========================================================================
 * Description: Create a new virtual LAN interface.
 * Parameters:  [in] ifreq - Contains the name of the vlan interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_vlan_create(Ipnet_socket *sock, struct Ip_ifreq *ifreq)
{
    Ipcom_netif *netif;
    int          ret;

    IPCOM_UNUSED_ARG(sock);

    netif = ipcom_if_malloc(IP_IFT_L2VLAN);
    if (netif == IP_NULL)
    {
        ret = -IP_ERRNO_ENOMEM;
        goto done;
    }

    ipcom_strncpy(netif->name, ifreq->ifr_name, IP_IFNAMSIZ);
    netif->metric       = 0;

    ret = ipcom_if_attach(netif);
    if (ret < 0)
    {
        (void)ipcom_if_free(netif);
        goto done;
    }

done:
    return ret >= 0 ? 0 : ret;
}

/*
 *===========================================================================
 *                    ipnet_vlan_compact_create
 *===========================================================================
 * Description: Create a new virtual LAN interface with the compact naming
 *              style to support <etherif>.<vlan> interface name, where
 *              <etherif> is the parent interface name that the vlan
 *              pseudo-interface is bound to and <vlan> is the vlan id
 *              associated with the vlan pseudo-interface.
 * Parameters:  [in] ifreq - Contains the name of the vlan interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_vlan_compact_create(Ipnet_socket *sock, struct Ip_ifreq *ifreq)
{
    Ipcom_netif *netif;
    Ipnet_netif *parent;
    char        name_buf[IP_IFNAMSIZ];
    char        *ifname;
    char        *tag;
    int         vlr_tag;
    int         ret;

    IPCOM_UNUSED_ARG(sock);

    ipcom_strncpy(name_buf, ifreq->ifr_name, sizeof(name_buf));
    ifname = ipcom_strtok_r(name_buf, ".", &tag);
    if (tag == IP_NULL)
        return -IP_ERRNO_EINVAL;

    /* make sure the parent indeed exists */
    parent = ipnet_if_nametonetif(sock->vr_index, ifname);
    if (parent  == IP_NULL)
        return -IP_ERRNO_ENXIO;

    if (parent->ipcom.type != IP_IFT_ETHER)
        return -IP_ERRNO_EMEDIUMTYPE;

    /* verify that the vlan id is within the acceptable range */
    vlr_tag = ipcom_atoi(tag);
    if (vlr_tag < 1 || vlr_tag > 4094)
        return -IP_ERRNO_EINVAL;

    netif = ipcom_if_malloc(IP_IFT_L2VLAN);
    if (netif == IP_NULL)
    {
        ret = -IP_ERRNO_ENOMEM;
        goto errout;
    }

    ipcom_strncpy(netif->name, ifreq->ifr_name, IP_IFNAMSIZ);
    ret = ipcom_if_attach(netif);
    if (ret < 0)
    {
        (void)ipcom_if_free(netif);
        goto errout;
    }

    netif->metric = 0;
    ((Ipnet_netif *)netif)->eth->vlan_param.id = vlr_tag;
    ((Ipnet_netif *)netif)->eth->vlan_param.pri = 0;

    /* bind to the parent interface */
    ret = ipnet_vlan_bind(parent, (Ipnet_netif *)netif);
    if (ret < 0)
    {
        (void)ipcom_if_detach(netif);
        (void)ipcom_if_free(netif);
    }

#ifndef IPCOM_VXWORKS_USE_MUX_L2
    /*
     * If MUX-L2 is not enabled for 802.1Q vlan management, the
     * vlan pseudo-interface will always send vlan-tagged frame
     */
    ((Ipnet_netif *)netif)->eth->vlan_param.tag_required = IP_TRUE; /* always */
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

    IPCOM_LOG3(DEBUG, "%s is created with vlan ID %d and parent %s",
               netif->name, vlr_tag, parent->ipcom.name);
    return ret >= 0 ? 0 : ret;


errout:
    return ret >= 0 ? 0 : ret;
}

#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
/*
 *===========================================================================
 *                    ipnet_vlan_add_frame_header
 *===========================================================================
 * Description: Adds an Ethernet frame header with a VLAN tag.
 * Parameters:  pkt - A packet buffer.
 *              frame_type_n - The type of data this frame contains.
 *              netif - The network interface on which the packet should
 *              be sent.
 * Returns:     Pointer to the Ethernet frame header
 *
 */
IP_GLOBAL Ipnet_pkt_eth *
ipnet_vlan_add_frame_header(Ipcom_pkt *pkt,
                            Ip_u16 frame_type_n,
                            struct Ipnet_netif_struct *netif)
{
    Ipnet_pkt_eth *eth;

    ip_assert(IPNET_ETH_PKT_GET_VLAN_TAG(pkt) & 0xffff);

#ifdef IPCOM_USE_HW_VLAN_TAGGING
    /*
     * Check if the interface supports inserting vlan tag to the transmitting
     * frame. The netif could be a vlan pseudo-interface or physical ethernet
     * interface. For vlan pseudo-interface, the parent's driver capabilities
     * are copied when binding to the parent is done.
     */
    if (IP_BIT_ISSET(netif->ipcom.drv_capabilities,
                     IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_TX))
    {
        /*
         * hardware shall take care inserting the vlan tag to the
         * ethernet header. Simply create an untagged ethernet header.
         */
        pkt->start -= IPNET_ETH_HDR_SIZE;
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_VLAN_TAG);
        eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];
        eth->d.type = frame_type_n;
        return eth;
    }
    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_VLAN_TAG);
#else
    IPCOM_UNUSED_ARG(netif);
#endif /* IPCOM_USE_HW_VLAN_TAGGING */

    /*
     * if we are here, hardware does not support inserting vlan to the
     * ethernet header. Do software tagging
     */
    pkt->start -= IPNET_ETH_HDR_SIZE + IPNET_VLAN_TAG_SIZE;
    eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];
    eth->d.vlan.tag_type    = IPNET_ETHFRAME_8021Q_VLAN;
    eth->d.vlan.pri_cif_vid = IPNET_ETH_PKT_GET_VLAN_TAG(pkt);
    eth->d.vlan.pri_cif_vid = ip_htons(eth->d.vlan.pri_cif_vid);
    eth->d.vlan.type        = frame_type_n;
    return eth;
}
#endif /* IPNET_USE_PER_SOCKET_VLAN_TAG */

/*
 *===========================================================================
 *                    ipnet_vlan_if_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_vlan_if_init(Ipnet_netif *netif)
{
    int ret;

    ret = ipnet_eth_if_init(netif);
    if (ret < 0)
        return ret;

#ifndef IP_PORT_VXWORKS
    /*
     * the vlan pseudo-interface mtu and link header size will be determine
     * later when it is bound to a vlan.
     */
    netif->ipcom.mtu            -= IPNET_VLAN_TAG_SIZE;
#endif /* !IP_PORT_VXWORKS */
#ifndef IPCOM_USE_HW_VLAN_TAGGING
    netif->ipcom.link_hdr_size  += IPNET_VLAN_TAG_SIZE;
#endif

    netif->link_ioctl           = ipnet_vlan_link_ioctl;
    netif->ipcom.drv_ioctl      = (Ipcom_drv_ioctl) ipnet_vlan_drv_ioctl;
    netif->ipcom.drv_output     = (Ipcom_drv_output) ipnet_vlan_drv_output;

    netif->ipcom.arphrd_type    = IP_ARPHRD_ETHER;

#ifdef IPNET_USE_NETLINKSOCK
    netif->rtnetlink_ops       = &vlan_ops;
#endif
    ipcom_memset(netif->ipcom.name, 0, sizeof(netif->ipcom.name));

    return 0;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#endif /* IPNET_USE_VLAN */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

